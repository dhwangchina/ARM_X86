/******************************************************************************
* COPYRIGHT @ Reserved
*******************************************************************************
* File         : 
* Function     : 
* Statement    : 
* Creation Info：
******************************************************************************/
/******************************************************************************
* Modification: 
* Time        ：
* Modifier    ：
* Statement   ：
******************************************************************************/
#include "cpss_type.h"
#include "cpss_public.h"
#include "cpss_common.h"
#include "cpss_config.h"
#include "cpss_dbg.h"
#include "cpss_vos_task.h"
#include "cpss_vos_sem.h"

#ifdef CPSS_VOS_VXWORKS
#include "taskHookLib.h"
#endif

#ifdef CPSS_VOS_LINUX
#include <sched.h>
#include <pthread.h>
#endif

/******************************* 局部宏定义 **********************************/

/******************************* 全局变量定义/初始化 *************************/
 BOOL g_bCpssVosTaskLibInited = FALSE;
/* task descriptor table */
 CPSS_VOS_TASK_DESC_T    g_astCpssVosTaskDescTbl[VOS_MAX_TASK_NUM];

UINT32 g_ulCpssMicroSec = 0;

UINT32 g_azCpssSwitchInTime[32][32][2]={0};

UINT32 g_azCpssSwitchInCount[32]={0};

BOOL g_bCpssTaskSwitch = FALSE;
/******************************* 局部常数和类型定义 **************************/

/******************************* 局部函数原型声明 ****************************/
 UINT32   cpss_vos_task_desc_alloc (void);
 INT32    cpss_vos_task_desc_free (UINT32 ulTaskDesc);
#ifdef CPSS_VOS_WINDOWS
 INT32    cpss_vos_task_init (CPSS_VOS_TASK_TCB_T *ptTcb, INT8 *pcName, 
                                    INT32 lPri, INT32 lOptions, INT32 lStackSize,
                                    VOID_FUNC_PTR pfEntry, INT32 lArg);
#endif

#ifdef CPSS_VOS_LINUX
 INT32    cpss_vos_task_init (CPSS_VOS_TASK_TCB_T *ptTcb, INT8 *pcName, 
                                    INT32 lPri, INT32 lOptions, INT32 lStackSize,
                                    VOID_FUNC_PTR pfEntry, INT32 lArg);
#endif

void cpss_vos_task_summary (CPSS_VOS_TASK_INFO_T *ptTaskInfo);
extern UINT32 g_ulCpssSchedTaskDescBase;

#ifdef CPSS_VOS_VXWORKS
extern VOID cpss_kw_set_task_run_time(UINT32 ulInterval);
#endif

/******************************* 函数实现 ************************************/

/*******************************************************************************
* 函数名称: cpss_vos_task_lib_init
* 功    能: 任务库初始化
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTaskDesc     INT32     输入
* 函数返回: CPSS_OK or CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_vos_task_lib_init (void)
{
INT32   ulTaskDesc;

if (g_bCpssVosTaskLibInited == TRUE)
{
    return (CPSS_ERROR);
}

/* 清除任务描述符表. */

memset ((void *) (& g_astCpssVosTaskDescTbl[0]), 0, 
    (VOS_MAX_TASK_NUM * sizeof (CPSS_VOS_TASK_DESC_T)));

/* 设置为未使用. */

for (ulTaskDesc = 0; ulTaskDesc < VOS_MAX_TASK_NUM; ulTaskDesc++)
{        
    g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse = FALSE;
    g_astCpssVosTaskDescTbl[ulTaskDesc].ulSchedDesc = VK_SCHED_DESC_INVALID;
}

/* 初始化完毕. */

g_bCpssVosTaskLibInited = TRUE;

return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_vos_task_desc_alloc
* 功    能: 分配任务描述符
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTaskDesc    UINT32      输入
* 函数返回: task descriptor
* 说    明:                                 
*******************************************************************************/
 UINT32 cpss_vos_task_desc_alloc (void)
{
UINT32  ulTaskDesc;

/* 寻找一个空闲任务描述符, NO.0 保留. */    
for (ulTaskDesc = 1; ulTaskDesc < VOS_MAX_TASK_NUM; ulTaskDesc++)
{
    if (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == FALSE)
    {
        break;
    }
}

/* 未发现. */    
if (ulTaskDesc == VOS_MAX_TASK_NUM)
{
    cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, 
        "cpss_vos_task_desc_alloc: insufficient decriptor available.\n");        
    return (VOS_TASK_DESC_INVALID);
}

/* 设置为使用. */    
g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse = TRUE;

return (ulTaskDesc);
}

/*******************************************************************************
* 函数名称: cpss_vos_task_desc_free
* 功    能: 释放任务描述符
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTaskDesc     UINT32     输入
* 函数返回: CPSS_OK or CPSS_ERROR
* 说    明: 此程序释放一个任务描述符.                                
*******************************************************************************/
 INT32 cpss_vos_task_desc_free 
(
 UINT32  ulTaskDesc
 )
{
    /* 检查任务描述符. */    
    if ((ulTaskDesc <= 0) || (ulTaskDesc >= VOS_MAX_TASK_NUM))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_desc_free: descriptor invalid, descriptor = %d.\n"
            , ulTaskDesc);        
        return (CPSS_ERROR);
    }
    
    /* 清除此任务描述符. */    
    memset ((void *) (& g_astCpssVosTaskDescTbl[ulTaskDesc]), 0,  
        sizeof (CPSS_VOS_TASK_DESC_T));
    
    /* 设置为未使用. */    
    g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse = FALSE;
    
    return (CPSS_OK);
}

#ifdef CPSS_VOS_WINDOWS

/*******************************************************************************
* 函数名称: cpss_vos_task_init
* 功    能: 初始化任务
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* pstTcb        CPSS_VOS_TASK_TCB_T *  输入
*pcName    INT8 *       输入
*lPri       INT32       输入
*lOptions    INT32       输入
*lStackSize    INT32     输入
*pfEntry       VOID_FUNC_PTR     输入
*lArg      INT32      输入
* 函数返回:  CPSS_OK or CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
 INT32 cpss_vos_task_init 
(
 CPSS_VOS_TASK_TCB_T *  pstTcb, /* 指向 TCB的指针 */
 INT8 *  pcName, /* 名字 */
 INT32   lPri, /* 优先级 */
 INT32   lOptions,  /* 选项 */
 INT32   lStackSize, /* 所需栈大小(字节) */
 VOID_FUNC_PTR   pfEntry, /* 函数入口 */
 INT32   lArg /* 参数 */
 )
{
HANDLE  hObj; /* 句柄 */
ULONG   ulThreadId; /* 线程ID */

/* 初始化问题. */

if ((!g_bCpssVosTaskLibInited) && (cpss_vos_task_lib_init () == CPSS_ERROR))
{
    return (CPSS_ERROR);
}

/* 检查优先级范围, ! (0 <= x <= 6). */

if ((lPri < 0) || (lPri > 6))
{
    cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_cpss_vos_task_init: priority illegal, priority = %d.\n"
        , lPri);
    return (CPSS_ERROR);
}

/* 创建线程. */

hObj = CreateThread (NULL, lStackSize, (LPTHREAD_START_ROUTINE) pfEntry, 
    (LPVOID) lArg, CREATE_SUSPENDED, & ulThreadId);
if (hObj == NULL)
{
    cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_init: CreateThread failed.\n"
        );
    
    return (CPSS_ERROR);
}  

/* 
* 初始化TCB的其它部分. 暂未修复任务名为指定和名字过长的问题
*/

if (pcName != NULL)
{
    strcpy (pstTcb->acName, pcName);
}

pstTcb->lOptions = lOptions;
pstTcb->lPri = lPri;
pstTcb->pfEntry = pfEntry;
pstTcb->lStackSize = lStackSize;
pstTcb->ulThreadId = ulThreadId;
pstTcb->hObj = hObj;

return (CPSS_OK);
}
#endif /* CPSS_VOS_WINDOWS */

#ifdef CPSS_DSP_CPU
/*******************************************************************************
* 函数名称: cpss_vos_task_init
* 功    能: 初始化任务
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* pstTcb        CPSS_VOS_TASK_TCB_T *  输入
*pcName    INT8 *       输入
*lPri       INT32       输入
*lOptions    INT32       输入
*lStackSize    INT32     输入
*pfEntry       VOID_FUNC_PTR     输入
*lArg      INT32      输入
* 函数返回:  CPSS_OK or CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
static INT32 cpss_vos_task_init 
(
 CPSS_VOS_TASK_TCB_T *  pstTcb, /* 指向 TCB的指针 */
 INT8 *  pcName, /* 名字 */
 INT32   lPri, /* 优先级 */
 INT32   lOptions,  /* 选项 */
 INT32   lStackSize, /* 所需栈大小(字节) */
 VOID_FUNC_PTR   pfEntry, /* 函数入口 */
 INT32   lArg /* 参数 */
 )
{
/* 初始化问题. */

if ((!g_bCpssVosTaskLibInited) && (cpss_vos_task_lib_init () == CPSS_ERROR))
{
    return (CPSS_ERROR);
}

/* 检查优先级范围, ! (0 <= x <= 6). */

if ((lPri < 0) || (lPri > 6))
{
    cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_cpss_vos_task_init: priority illegal, priority = %d.\n"
        , lPri);
    return (CPSS_ERROR);
}

/* 
* 初始化TCB的其它部分. 暂未修复任务名为指定和名字过长的问题
*/

if (pcName != NULL)
{
    strcpy (pstTcb->acName, pcName);
}

pstTcb->lOptions = lOptions;
pstTcb->lPri = lPri;
pstTcb->pfEntry = pfEntry;
pstTcb->lStackSize = lStackSize;

return (CPSS_OK);
}
#endif

#ifdef CPSS_VOS_LINUX
 INT32 cpss_vos_task_init 
(
 CPSS_VOS_TASK_TCB_T *  pstTcb, /* 指向 TCB的指针 */
 INT8 *  pcName, /* 名字 */
 INT32   lPri, /* 优先级 */
 INT32   lOptions,  /* 选项 */
 INT32   lStackSize, /* 所需栈大小(字节) */
 VOID_FUNC_PTR   pfEntry, /* 函数入口 */
 INT32   lArg /* 参数 */
 )
{

	pthread_attr_t attr;	/* 线程属性*/
	struct sched_param stSchedParam;

	/* 初始化问题. */

	if ((!g_bCpssVosTaskLibInited) && (cpss_vos_task_lib_init () == CPSS_ERROR))
	{
		return (CPSS_ERROR);
	}

	/* 检查优先级范围, ! (0 <= x <= 6). */

	if ((lPri < 0) || (lPri > 6))
	{
		cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_cpss_vos_task_init: priority illegal, priority = %d.\n"
			, lPri);
		return (CPSS_ERROR);
	}
    /* 优先级映射. */
    
    switch (lPri)
    {
    case TASK_PRIORITY_TIME_CRITICAL:
        {
            lPri = CPSS_TASKPRI_TIME_CRITICAL;
            break;
        }
        
    case TASK_PRIORITY_HIGHEST:
        {
            lPri = CPSS_TASKPRI_HIGHEST;
            break;
        }
        
    case TASK_PRIORITY_ABOVE_NORMAL:
        {
            lPri = CPSS_TASKPRI_ABOVE_NORMAL;
            break;
        }
        
    case TASK_PRIORITY_NORMAL:
        {
            lPri = CPSS_TASKPRI_NORMAL;
            break;
        }
        
    case TASK_PRIORITY_BELOW_NORMAL:
        {
            lPri = CPSS_TASKPRI_BELOW_NORMAL;
            break;
        }
        
    case TASK_PRIORITY_LOWEST:
        {
            lPri = CPSS_TASKPRI_LOWEST;
            break;
        }
        
    case TASK_PRIORITY_IDLE:
        {
            lPri = CPSS_TASKPRI_IDLE;
            break;
        }
    	}
	
	/* 创建线程. */

	if(0 != pthread_attr_init(&attr))/*定义线程属性*/
	{
		perror("pthread_attr_init");
	}


	/*设置为不继承父进程的调度策略和优先级*/
	if(0 != pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED))
	{
		perror("pthread_attr_setinheritsched");
	}

	if(0 != pthread_attr_setstacksize(&attr, lStackSize))/*设置线程堆栈大小*/
	{
		perror("pthread_attr_setstacksize");
	}

	if(0 != pthread_attr_setschedpolicy(&attr, SCHED_RR))/*设置线程调度策略*/
	{
		perror("pthread_attr_setschedpolicy");
	}

	stSchedParam.sched_priority = lPri;

	if(0 != pthread_attr_setschedparam(&attr, &stSchedParam))/*设置线程优先级*/
	{
		perror("pthread_attr_setschedparam");
	}

	if (pthread_create(&pstTcb->ptThreadId, &attr, (void *)pfEntry, (void *)lArg) != 0)/*创建线程*/
	{
		cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_init: pthread_create failed.\n"
			);

		pthread_attr_destroy(&attr);/*销毁线程属性*/

		return (CPSS_ERROR);
	}

	pthread_attr_destroy(&attr);/*销毁线程属性*/

	/*
	* 初始化TCB的其它部分. 暂未修复任务名为指定和名字过长的问题
	*/

	if (pcName != NULL)
	{
		strcpy (pstTcb->acName, pcName);
	}

	pstTcb->lOptions = lOptions;
	pstTcb->lPri = lPri;
	pstTcb->pfEntry = pfEntry;
	pstTcb->lStackSize = lStackSize;
/*	pstTcb->ptThreadId = ThreadId;*/

	return (CPSS_OK);
}
#endif /* CPSS_VOS_LINUX */

/*******************************************************************************
* 函数名称: cpss_vos_task_spawn
* 功    能: 创建任务
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* pcName      INT8 *      输入
* lPri         INT32       输入
* lOptions      INT32   输入
* lStackSize     INT32    输入
* pfEntry       VOID_FUNC_PTR      输入
* lArg        INT32        输入
* 函数返回: 任务描述符
* 说    明: 此程序创建并激活一个任务
*******************************************************************************/
UINT32 cpss_vos_task_spawn
(
 INT8 *  pcName, /* 名字 */
 INT32   lPri, /* 优先级 */
 INT32   lOptions,  /* 选项 */
 INT32   lStackSize, /* 栈所需空间大小(字节) */
 VOID_FUNC_PTR   pfEntry, /* 函数入口 */
 INT32   lArg /* 参数 */
 )
{
    UINT32  ulTaskDesc;
    VOS_TASK_ID taskId;
    
    /* 分配任务描述符. */
    
    ulTaskDesc = cpss_vos_task_desc_alloc ();
    if (ulTaskDesc == VOS_TASK_DESC_INVALID)
    {
        return (VOS_TASK_DESC_INVALID);
    }
    
#ifdef CPSS_VOS_VXWORKS
    /* 优先级映射. */
    
    switch (lPri)
    {
    case TASK_PRIORITY_TIME_CRITICAL:
        {
            lPri = CPSS_TASKPRI_TIME_CRITICAL;
            break;
        }
        
    case TASK_PRIORITY_HIGHEST:
        {
            lPri = CPSS_TASKPRI_HIGHEST;
            break;
        }
        
    case TASK_PRIORITY_ABOVE_NORMAL:
        {
            lPri = CPSS_TASKPRI_ABOVE_NORMAL;
            break;
        }
        
    case TASK_PRIORITY_NORMAL:
        {
            lPri = CPSS_TASKPRI_NORMAL;
            break;
        }
        
    case TASK_PRIORITY_BELOW_NORMAL:
        {
            lPri = CPSS_TASKPRI_BELOW_NORMAL;
            break;
        }
        
    case TASK_PRIORITY_LOWEST:
        {
            lPri = CPSS_TASKPRI_LOWEST;
            break;
        }
        
    case TASK_PRIORITY_IDLE:
        {
            lPri = CPSS_TASKPRI_IDLE;
            break;
        }


    case TASK_PRIORITY_SHMCSEND:
        {
            lPri = CPSS_TASKPRI_SHMCSEND;
            break;
        }
    case TASK_PRIORITY_SHMCRECV:
        {
            lPri = CPSS_TASKPRI_SHMCRECV;
            break;
        }

    default:
        {
            cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_spawn: priority illegal, priority = %d.\n"
                , lPri);
            cpss_vos_task_desc_free (ulTaskDesc);
            return (VOS_TASK_DESC_INVALID);
            break;
        }
        /* 2006/5/15, jlm added */
    }
    /* 2006/5/15, jlm */
    
#endif /* CPSS_VOS_VXWORKS */
    
    /* 计算栈大小. */
    
    if (lStackSize == 0)
    {
        lStackSize = VOS_TASK_DEF_STACK_SIZE;
    }
    
#ifdef CPSS_VOS_VXWORKS
    /* 创建任务. */
    taskId = taskSpawn (pcName, lPri, lOptions, lStackSize, (FUNCPTR) pfEntry,
        lArg, 0, 0, 0, 0, 0, 0, 0, 0, 0);
    if (taskId == ERROR)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_spawn: taskSpawn failed.\n"
            );
        
        cpss_vos_task_desc_free (ulTaskDesc);
        
        return (VOS_TASK_DESC_INVALID);
    }
#endif /* CPSS_VOS_VXWORKS */
    
#ifdef CPSS_VOS_WINDOWS
    /* 分配内存. */
    
    taskId = (VOS_TASK_ID) malloc (sizeof (CPSS_VOS_TASK_TCB_T));
    if (taskId == NULL)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_spawn: malloc failed.\n");        
        cpss_vos_task_desc_free (ulTaskDesc);        
        return (VOS_TASK_DESC_INVALID);
    }
    
    /* 初始化TCB. */
    
    if (cpss_vos_task_init (taskId, pcName, lPri, lOptions, lStackSize, pfEntry, 
        lArg) == CPSS_ERROR)
    {
        free ((void *) taskId);
        cpss_vos_task_desc_free (ulTaskDesc);        
        return (VOS_TASK_DESC_INVALID);
    }
#endif /* CPSS_VOS_WINDOWS */    

#ifdef CPSS_VOS_LINUX
    /* 分配内存. */
    
    taskId = (VOS_TASK_ID) malloc (sizeof (CPSS_VOS_TASK_TCB_T));
    if (taskId == NULL)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_spawn: malloc failed.\n");        
        cpss_vos_task_desc_free (ulTaskDesc);        
        return (VOS_TASK_DESC_INVALID);
    }
    
    /* 初始化TCB. */
    
    if (cpss_vos_task_init (taskId, pcName, lPri, lOptions, lStackSize, pfEntry, 
        lArg) == CPSS_ERROR)
    {
        free ((void *) taskId);
        cpss_vos_task_desc_free (ulTaskDesc);        
        return (VOS_TASK_DESC_INVALID);
    }
#endif /* CPSS_VOS_LINUX */   

#ifdef CPSS_DSP_CPU
    /* 分配内存. */
    
    taskId = (VOS_TASK_ID) malloc (sizeof (CPSS_VOS_TASK_TCB_T));
    if (taskId == NULL)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_spawn: malloc failed.\n");        
        cpss_vos_task_desc_free (ulTaskDesc);        
        return (VOS_TASK_DESC_INVALID);
    }
    
    /* 初始化TCB. */
    
    if (cpss_vos_task_init (taskId, pcName, lPri, lOptions, lStackSize, pfEntry, 
        lArg) == CPSS_ERROR)
    {
        free ((void *) taskId);
        cpss_vos_task_desc_free (ulTaskDesc);        
        return (VOS_TASK_DESC_INVALID);
    }
#endif

    /* 保存任务ID. */    
    g_astCpssVosTaskDescTbl[ulTaskDesc].taskId = taskId;
    
#ifdef CPSS_VOS_WINDOWS
    /* 设置优先级. */    
    /* 2006/10/09 李军增加开始*/
    switch (lPri)
    {
    case TASK_PRIORITY_TIME_CRITICAL:
        {
            lPri = CPSS_TASKPRI_TIME_CRITICAL;
            break;
        }
        
    case TASK_PRIORITY_HIGHEST:
        {
            lPri = CPSS_TASKPRI_HIGHEST;
            break;
        }
        
    case TASK_PRIORITY_ABOVE_NORMAL:
        {
            lPri = CPSS_TASKPRI_ABOVE_NORMAL;
            break;
        }
        
    case TASK_PRIORITY_NORMAL:
        {
            lPri = CPSS_TASKPRI_NORMAL;
            break;
        }
        
    case TASK_PRIORITY_BELOW_NORMAL:
        {
            lPri = CPSS_TASKPRI_BELOW_NORMAL;
            break;
        }
        
    case TASK_PRIORITY_LOWEST:
        {
            lPri = CPSS_TASKPRI_LOWEST;
            break;
        }
        
    case TASK_PRIORITY_IDLE:
        {
            lPri = CPSS_TASKPRI_IDLE;
            break;
        }
      }
   /* 2006/10/09 李军增加结束*/
    cpss_vos_task_pri_set (ulTaskDesc, lPri);
    cpss_vos_task_resume (ulTaskDesc);
#endif /* CPSS_VOS_WINDOWS */
    
    return (ulTaskDesc);
}

/*******************************************************************************
* 函数名称: cpss_vos_task_delete
* 功    能: 此函数删除一个任务
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTaskDesc     UINT32     输入
* 函数返回: 
* 说    明: CPSS_OK or CPSS_ERROR                                
*******************************************************************************/
INT32 cpss_vos_task_delete 
(
    UINT32  ulTaskDesc /* 任务描述符 */
)
{
    VOS_TASK_ID taskId;
    
    /* 检查任务描述符. */
    
    if ((ulTaskDesc <= 0) || (ulTaskDesc >= VOS_MAX_TASK_NUM) ||
        (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_delete: descriptor invalid, descriptor = %d.\n"
            , ulTaskDesc);
        return (CPSS_ERROR);
    }
    
    /* 获取任务ID. */
    
    taskId = g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;
    
#ifdef CPSS_VOS_VXWORKS
    if (taskDelete (taskId) == ERROR)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_delete: taskDelete failed.\n");
        return (CPSS_ERROR);
    }
    
    cpss_vos_task_desc_free (ulTaskDesc);        
    return (CPSS_OK);
#endif /* CPSS_VOS_VXWORKS */
    
#ifdef CPSS_VOS_WINDOWS
    if (CloseHandle (taskId->hObj) == FALSE)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_delete: CloseHandle failed.\n");
        return (CPSS_ERROR);
    }
    
    /* 释放内存. */        
    free ((void *) taskId);
    
    /* 释放任务描述符 */        
    cpss_vos_task_desc_free (ulTaskDesc);
    
    return (CPSS_OK);
#endif /* CPSS_VOS_WINDOWS */

#ifdef CPSS_DSP_CPU
    /* 释放内存. */        
    free ((void *) taskId);
    
    /* 释放任务描述符 */        
    cpss_vos_task_desc_free (ulTaskDesc);
    
    return (CPSS_OK);
#endif

#ifdef CPSS_VOS_LINUX
    if (pthread_cancel (taskId->ptThreadId) != 0)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_delete: CloseHandle failed.\n");
        return (CPSS_ERROR);
    }
    
    /* 释放内存. */        
    free ((void *) taskId);
    
    /* 释放任务描述符 */        
    cpss_vos_task_desc_free (ulTaskDesc);
    
    return (CPSS_OK);
#endif /* CPSS_VOS_LINUX */
}

/*******************************************************************************
* 函数名称: cpss_vos_task_suspend
* 功    能: 此函数挂起一个任务
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTaskDesc    UINT32      输入
* 函数返回: CPSS_OK or CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_vos_task_suspend 
(
    UINT32  ulTaskDesc /* 任务描述符 */
)
{
    VOS_TASK_ID taskId;
    
    /* 检查任务描述符. */        
    if ((ulTaskDesc <= 0) || (ulTaskDesc >= VOS_MAX_TASK_NUM) ||
        (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_suspend: descriptor invalid, descriptor = %d.\n"
            , ulTaskDesc);            
        return (CPSS_ERROR);
    }
    
    /* 获得任务ID. */        
    taskId = g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;        
#ifdef CPSS_VOS_VXWORKS
    if (taskSuspend (taskId) == ERROR)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_suspend: taskSuspend failed.\n");            
        return (CPSS_ERROR);
    }
    
    return (CPSS_OK);
#endif /* CPSS_VOS_VXWORKS */
    
#ifdef CPSS_VOS_WINDOWS
    if (SuspendThread (taskId->hObj) == 0xFFFFFFFF)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_suspend: SuspendThread failed.\n");            
        return (CPSS_ERROR);
    }
    
    return (CPSS_OK);
#endif /* CPSS_VOS_WINDOWS */

#ifdef CPSS_DSP_CPU
    if (taskId == NULL)
        {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_suspend: task ID invalid.\n"
                     );
        
        return (CPSS_ERROR);
        }

    return (CPSS_OK);
#endif

#ifdef CPSS_VOS_LINUX
    if (pthread_suspend_stub(taskId->ptThreadId) != 0)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_suspend: SuspendThread failed.\n");            
        return (CPSS_ERROR);
    }
    
    return (CPSS_OK);
#endif /* CPSS_VOS_LINUX */
}

/*******************************************************************************
* 函数名称: cpss_vos_task_resume
* 功    能: 此函数恢复一个任务
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTaskDesc    UINT32      输入
* 函数返回: CPSS_OK or CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_vos_task_resume 
(
    UINT32  ulTaskDesc /* 任务描述符 */
)
{
    VOS_TASK_ID taskId;
    
    /* 检查任务描述符. */    
    if ((ulTaskDesc <= 0) || (ulTaskDesc >= VOS_MAX_TASK_NUM) ||
        (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_resume: descriptor invalid, descriptor = %d.\n"
            , ulTaskDesc);        
        return (CPSS_ERROR);
    }
    
    /* 获得任务ID. */
    
    taskId = g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;    
#ifdef CPSS_VOS_VXWORKS
    if (taskResume (taskId) == ERROR)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_resume: taskResume failed.\n");        
        return (CPSS_ERROR);
    }
    
    return (CPSS_OK);
#endif /* CPSS_VOS_VXWORKS */
    
#ifdef CPSS_VOS_WINDOWS
    if (ResumeThread (taskId->hObj) == 0xFFFFFFFF)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_resume: ResumeThread failed.\n");        
        return (CPSS_ERROR);
    }
    
    return (CPSS_OK);
#endif /* CPSS_VOS_WINDOWS */

#ifdef CPSS_DSP_CPU
    if (taskId == NULL)
        {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_resume: task ID invalid.\n"
                     );
        
        return (CPSS_ERROR);
        }
        
    return (CPSS_OK);
#endif

#ifdef CPSS_VOS_LINUX
    if (pthread_continue_stub(taskId->ptThreadId) != 0)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_resume: ResumeThread failed.\n");        
        return (CPSS_ERROR);
    }
    
    return (CPSS_OK);
#endif /* CPSS_VOS_LINUX */
}

/*******************************************************************************
* 函数名称: cpss_vos_task_pri_set
* 功    能: 此函数设置任务优先级
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTaskDesc      UINT32      输入
* lNewPri        INT32       输入
* 函数返回:  CPSS_OK or CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
    INT32 cpss_vos_task_pri_set 
(
UINT32  ulTaskDesc, /* 任务描述符 */
INT32   lNewPri /* 新优先级 */
)
{
VOS_TASK_ID taskId;

/* 检查任务描述符. */

if ((ulTaskDesc <= 0) || (ulTaskDesc >= VOS_MAX_TASK_NUM) ||
    (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == FALSE))
{
    cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_set: descriptor invalid, descriptor = %d.\n"
        , ulTaskDesc);
    
    return (CPSS_ERROR);
}

/* 获得任务ID. */

taskId = g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;

#ifdef CPSS_VOS_VXWORKS
/* 优先级映射. */

switch (lNewPri)
{
case TASK_PRIORITY_TIME_CRITICAL:
    {
        lNewPri = 104;
        break;
    }
    
case TASK_PRIORITY_HIGHEST:
    {
        lNewPri = 134;
        break;
    }
    
case TASK_PRIORITY_ABOVE_NORMAL:
    {
        lNewPri = 164;
        break;
    }
    
case TASK_PRIORITY_NORMAL:
    {
        lNewPri = 194;
        break;
    }
    
case TASK_PRIORITY_BELOW_NORMAL:
    {
        lNewPri = 224;
        break;
    }
    
case TASK_PRIORITY_LOWEST:
    {
        lNewPri = 254;
        break;
    }
    
case TASK_PRIORITY_IDLE:
    {
        lNewPri = 255;
        break;
    }
    
default:
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_set: priority illegal, priority = %d.\n"
            , lNewPri);
        
        return (CPSS_ERROR);
        break;
    }
}

if (taskPrioritySet (taskId, lNewPri) == ERROR)
{
    cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_set: taskPrioritySet failed.\n"
        );
    
    return (CPSS_ERROR);
}

return (CPSS_OK);
#endif /* CPSS_VOS_VXWORKS */

#ifdef CPSS_VOS_WINDOWS
/* 检查优先级范围, ! (0 <= lNewPri <= 255) */

if ((lNewPri < 0) || (lNewPri > 6))
{
    cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_set: priority illegal, new priority = %d.\n"
        , lNewPri);
    
    return (CPSS_ERROR);
}

/* Windows 只有7 种优先级. */

switch (lNewPri)
{
case TASK_PRIORITY_TIME_CRITICAL:
    {
        lNewPri = THREAD_PRIORITY_TIME_CRITICAL;
        break;
    }
    
case TASK_PRIORITY_HIGHEST:
    {
        lNewPri = THREAD_PRIORITY_HIGHEST;
        break;
    }
    
case TASK_PRIORITY_ABOVE_NORMAL:
    {
        lNewPri = THREAD_PRIORITY_ABOVE_NORMAL;
        break;
    }
    
case TASK_PRIORITY_NORMAL:
    {
        lNewPri = THREAD_PRIORITY_NORMAL;
        break;
    }
    
case TASK_PRIORITY_BELOW_NORMAL:
    {
        lNewPri = THREAD_PRIORITY_BELOW_NORMAL;
        break;
    }
    
case TASK_PRIORITY_LOWEST:
    {
        lNewPri = THREAD_PRIORITY_LOWEST;
        break;
    }
    
case TASK_PRIORITY_IDLE:
    {
        lNewPri = THREAD_PRIORITY_IDLE;
        break;
    }
    
default:
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_set: priority illegal, priority = %d.\n"
            , lNewPri);
        
        return (VOS_TASK_DESC_INVALID);
        break;
    }
    /* 2006/5/15, jlm added */
}
/* 2006/5/15, jlm */

/* 设置线程优先级. */

if (SetThreadPriority (taskId->hObj, lNewPri) == FALSE)
{
    cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_set: SetThreadPriority failed.\n"
        );
    
    return (CPSS_ERROR);
}

taskId->lPri = lNewPri;

return (CPSS_OK);
#endif /* CPSS_VOS_WINDOWS */

#ifdef CPSS_VOS_LINUX
/* 检查优先级范围, ! (0 <= lNewPri <= 255) */

if ((lNewPri < 0) || (lNewPri > 6))
{
    cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_set: priority illegal, new priority = %d.\n"
        , lNewPri);
    
    return (CPSS_ERROR);
}

switch (lNewPri)
{
case TASK_PRIORITY_TIME_CRITICAL:
    {
        lNewPri = 35;
        break;
    }
    
case TASK_PRIORITY_HIGHEST:
    {
        lNewPri = 30;
        break;
    }
    
case TASK_PRIORITY_ABOVE_NORMAL:
    {
        lNewPri = 25;
        break;
    }
    
case TASK_PRIORITY_NORMAL:
    {
        lNewPri = 20;
        break;
    }
    
case TASK_PRIORITY_BELOW_NORMAL:
    {
        lNewPri = 15;
        break;
    }
    
case TASK_PRIORITY_LOWEST:
    {
        lNewPri = 10;
        break;
    }
    
case TASK_PRIORITY_IDLE:
    {
        lNewPri = 5;
        break;
    }
    
default:
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_set: priority illegal, priority = %d.\n"
            , lNewPri);
        
        return (VOS_TASK_DESC_INVALID);
        break;
    }

}


/* 设置线程优先级. */

if (pthread_setschedparam (taskId->ptThreadId, SCHED_FIFO, &lNewPri) !=0)
{
    cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_set: SetThreadPriority failed.\n"
        );
    
    return (CPSS_ERROR);
}

taskId->lPri = lNewPri;

return (CPSS_OK);
#endif /* CPSS_VOS_LINUX */

#ifdef CPSS_DSP_CPU
    taskId->lPri = lNewPri;

    return (CPSS_OK);
#endif
}

/*******************************************************************************
* 函数名称: cpss_vos_task_pri_get
* 功    能: 此函数获得一个任务的优先级
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTaskDesc      UINT32      输入
* plPri       INT32 *       输入
* 函数返回: CPSS_OK or CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
    INT32 cpss_vos_task_pri_get 
        (
        UINT32  ulTaskDesc, /* 任务描述符 */
        INT32 * plPri /* 新优先级 */
        )
    {
        VOS_TASK_ID taskId;
        
        /* 检查任务描述符. */
        
        if ((ulTaskDesc <= 0) || (ulTaskDesc >= VOS_MAX_TASK_NUM) ||
            (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == FALSE))
        {
            cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_get: descriptor invalid, descriptor = %d.\n"
                , ulTaskDesc);
            
            return (CPSS_ERROR);
        }
        
        /* 检查参数. */
        
        if (plPri == NULL)
        {
            cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_get: argument not specified.\n"
                );
            
            return (CPSS_ERROR);
        }
        
        /* 获得任务ID. */
        
        taskId = g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;
        
#ifdef CPSS_VOS_VXWORKS
        if (taskPriorityGet (taskId, plPri) == ERROR)
        {
            cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_get: taskPriorityGet failed.\n"
                );
            
            return (CPSS_ERROR);
        }
        
        /* 优先级反映射. */
        
        switch (*plPri)
        {
        case 104:
            {
                *plPri = TASK_PRIORITY_TIME_CRITICAL;
                break;
            }
            
        case 134:
            {
                *plPri = TASK_PRIORITY_HIGHEST;
                break;
            }
            
        case 164:
            {
                *plPri = TASK_PRIORITY_ABOVE_NORMAL;
                break;
            }
            
        case 194:
            {
                *plPri = TASK_PRIORITY_NORMAL;
                break;
            }
            
        case 224:
            {
                *plPri = TASK_PRIORITY_BELOW_NORMAL;
                break;
            }
            
        case 244:
            {
                *plPri = TASK_PRIORITY_LOWEST;
                break;
            }
            
        case 255:
            {
                *plPri = TASK_PRIORITY_IDLE;
                break;
            }
            
        default:
            {
                cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_get: priority illegal, priority = %d.\n"
                    , *plPri);
                
                return (VOS_TASK_DESC_INVALID);
                break;
            }
        }
        
        return (CPSS_OK);
#endif /* CPSS_VOS_VXWORKS */
        
#ifdef CPSS_VOS_WINDOWS
        /* 优先级反映射. */
        
        switch (taskId->lPri)
        {
        case THREAD_PRIORITY_TIME_CRITICAL:
            {
                *plPri = TASK_PRIORITY_TIME_CRITICAL;
                break;
            }
            
        case THREAD_PRIORITY_HIGHEST:
            {
                *plPri = TASK_PRIORITY_HIGHEST;
                break;
            }
            
        case THREAD_PRIORITY_ABOVE_NORMAL:
            {
                *plPri = TASK_PRIORITY_ABOVE_NORMAL;
                break;
            }
            
        case THREAD_PRIORITY_NORMAL:
            {
                *plPri = TASK_PRIORITY_NORMAL;
                break;
            }
            
        case THREAD_PRIORITY_BELOW_NORMAL:
            {
                *plPri = TASK_PRIORITY_BELOW_NORMAL;
                break;
            }
            
        case THREAD_PRIORITY_LOWEST:
            {
                *plPri = TASK_PRIORITY_LOWEST;
                break;
            }
            
        case THREAD_PRIORITY_IDLE:
            {
                *plPri = TASK_PRIORITY_IDLE;
                break;
            }
            
        default:
            {
                cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_get: priority illegal, priority = %d.\n"
                    , *plPri);
                
                return (VOS_TASK_DESC_INVALID);
                break;
            }
        }
        
        return (CPSS_OK);
#endif /* CPSS_VOS_WINDOWS */

#ifdef CPSS_VOS_LINUX
         if (pthread_getschedparam(taskId->ptThreadId, SCHED_FIFO, plPri) == ERROR)
        {
            cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_get: taskPriorityGet failed.\n"
                );
            
            return (CPSS_ERROR);
        }

		 /* 优先级反映射. */
        
        switch (taskId->lPri)
        {
        case 35:
            {
                *plPri = TASK_PRIORITY_TIME_CRITICAL;
                break;
            }
            
        case 30:
            {
                *plPri = TASK_PRIORITY_HIGHEST;
                break;
            }
            
        case 25:
            {
                *plPri = TASK_PRIORITY_ABOVE_NORMAL;
                break;
            }
            
        case 20:
            {
                *plPri = TASK_PRIORITY_NORMAL;
                break;
            }
            
        case 15:
            {
                *plPri = TASK_PRIORITY_BELOW_NORMAL;
                break;
            }
            
        case 10:
            {
                *plPri = TASK_PRIORITY_LOWEST;
                break;
            }
            
        case 5:
            {
                *plPri = TASK_PRIORITY_IDLE;
                break;
            }
            
        default:
            {
                cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_get: priority illegal, priority = %d.\n"
                    , *plPri);
                
                return (VOS_TASK_DESC_INVALID);
                break;
            }
        }
        
        return (CPSS_OK);
#endif /* CPSS_VOS_LINUX */

#ifdef CPSS_DSP_CPU
   *plPri = taskId->lPri;

    return (CPSS_OK);
#endif
    }

/*******************************************************************************
* 函数名称: cpss_vos_task_delay
* 功    能: 此函数延迟一个任务
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* lMs           INT32       输入
* 函数返回: CPSS_OK or CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_vos_task_delay 
(
INT32   lMs /* 毫秒 */
)
{
#ifdef CPSS_VOS_VXWORKS
/* 将毫秒转化为ticks . */

lMs = CPSS_DIV(lMs * sysClkRateGet () , 1000);

/*至少为 1 tick. */

if (lMs == 0)
{
    lMs = 1;
}

if (taskDelay (lMs) == ERROR)
{
    cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_delay: taskDelay failed.\n"
        );
    
    return (CPSS_ERROR);
}

return (CPSS_OK);
#endif /* CPSS_VOS_VXWORKS */

#ifdef CPSS_VOS_WINDOWS
Sleep (lMs);

return (CPSS_OK);
#endif /* CPSS_VOS_WINDOWS */

#ifdef CPSS_DSP_CPU
    return (CPSS_OK);
#endif

#ifdef CPSS_VOS_LINUX
    cpss_delay (lMs);

return (CPSS_OK);
#endif /* CPSS_VOS_Linux */
}

/*******************************************************************************
* 函数名称: cpss_vos_task_desc_self
* 功    能: 此函数获得当前任务描述符
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: task descriptor
* 说    明:                                 
*******************************************************************************/
UINT32 cpss_vos_task_desc_self (void)
{
    VOS_TASK_ID taskId;
#ifdef CPSS_VOS_WINDOWS
    ULONG   ulThreadId;
#endif /* CPSS_VOS_WINDOWS */
    UINT32   ulTaskDesc;

#ifdef CPSS_VOS_LINUX
    pthread_t   ThreadId;
#endif /* CPSS_VOS_LINUX */

#ifdef CPSS_VOS_VXWORKS
    taskId = taskIdSelf ();
    
    /* 找到一个任务描述符. */
    for (ulTaskDesc = g_ulCpssSchedTaskDescBase; ulTaskDesc < VOS_MAX_TASK_NUM; ulTaskDesc++)
    {
        if ((g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == TRUE) && 
            (g_astCpssVosTaskDescTbl[ulTaskDesc].taskId == taskId))
        {
            return (ulTaskDesc);
        }
    }
    for (ulTaskDesc = 1; (ulTaskDesc < g_ulCpssSchedTaskDescBase) && (ulTaskDesc < VOS_MAX_TASK_NUM); ulTaskDesc++)
    {
        if ((g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == TRUE) && 
            (g_astCpssVosTaskDescTbl[ulTaskDesc].taskId == taskId))
        {
            return (ulTaskDesc);
        }
    }
#endif /* CPSS_VOS_VXWORKS */
    
#ifdef CPSS_VOS_WINDOWS
    ulThreadId = GetCurrentThreadId ();
    
    /* 找到一个任务描述符. */
    
    for (ulTaskDesc = 1; ulTaskDesc < VOS_MAX_TASK_NUM; ulTaskDesc++)
    {
        taskId = g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;
        if ((g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == TRUE) && 
            (taskId->ulThreadId == ulThreadId))
        {
            return (ulTaskDesc);
        }
    }
#endif /* CPSS_VOS_WINDOWS */
    
/* Only one task. */
    
#ifdef CPSS_DSP_CPU
    /* 找到一个任务描述符. */
    
    for (ulTaskDesc = 1; ulTaskDesc < VOS_MAX_TASK_NUM; ulTaskDesc++)
        {
        taskId = g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;
        if ((g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == TRUE) && (taskId !=
            NULL))
            {
                return (ulTaskDesc);
            }
        }
#endif

#ifdef CPSS_VOS_LINUX
    ThreadId = pthread_self();
    
    /* 找到一个任务描述符. */
    
    for (ulTaskDesc = 1; ulTaskDesc < VOS_MAX_TASK_NUM; ulTaskDesc++)
    {
    	taskId = g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;
        if (taskId == NULL)
        {
        	break;
        }
        if ((g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == TRUE) && 
            (taskId->ptThreadId == ThreadId))
        {
            return (ulTaskDesc);
        }
    }
#endif /* CPSS_VOS_LINUX */

/* 没有找到. */
return (VOS_TASK_DESC_INVALID);
}

/*******************************************************************************
*函数名称:cpss_vos_task_desc_get_by_name
*功能:通过任务名字获得运行任务描述符
*函数类型: 
*参数: 
*参数名称 类型   输入/输出 描述
*pcName   pcName 输入
*函数返回:task descriptor
*说明:                                 
*******************************************************************************/
UINT32 cpss_vos_task_desc_get_by_name 
    (
    INT8 *  pcName    
    )
    {
    UINT32  ulTaskDesc;
    VOS_TASK_ID taskId;
    
    if (pcName == NULL)
        {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_desc_get_by_name: name not specified.\n"
                     );
        return (VOS_TASK_DESC_INVALID);
        }
        
    for (ulTaskDesc = 1; ulTaskDesc < VOS_MAX_TASK_NUM; ulTaskDesc++)
        {
        /* Task do not exist. */
        
        if (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == FALSE)
            {
            continue;
            }
        
        taskId = g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;

        /* Compare task name. */
            
        if (strcmp (pcName, g_astCpssVosTaskDescTbl[ulTaskDesc].acName) == 0)
            {
            break;
            }
        }
    
    /* Not found. */
        
    if (ulTaskDesc == VOS_MAX_TASK_NUM)
        {
        return (VOS_TASK_DESC_INVALID);
        }
        
    return (ulTaskDesc);
    }
    
INT8 * cpss_vos_task_name_get_by_name 
	 (
	 UINT32 ulTaskDesc   
	 )
 {
	 if((ulTaskDesc < 0) ||(ulTaskDesc>= VOS_MAX_TASK_NUM))
	 {
		 return NULL;
	 }
	 
	 
	 if (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == FALSE)
	 {
		 return NULL;
	 }
#ifdef CPSS_VOS_VXWORKS
     return (g_astCpssVosTaskDescTbl[ulTaskDesc].acName);
#else
	 return (g_astCpssVosTaskDescTbl[ulTaskDesc].taskId->acName);
#endif
}
    
/*******************************************************************************
* 函数名称: cpss_vos_task_info_get
* 功    能: 此函数获得任务信息
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTaskDesc     UINT32     输入
* pstTaskInfo      CPSS_VOS_TASK_INFO_T *    输入
* 函数返回: CPSS_OK or CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_vos_task_info_get 
(
UINT32  ulTaskDesc, /* 任务描述符 */
CPSS_VOS_TASK_INFO_T * pstTaskInfo /* 指向任务信息的指针 */
)
{
    VOS_TASK_ID taskId;
#ifdef CPSS_VOS_VXWORKS
    TASK_DESC   tDesc;
#endif /* CPSS_VOS_VXWORKS */
    
    /* 检查任务描述符. */
    
    if ((ulTaskDesc <= 0) || (ulTaskDesc >= VOS_MAX_TASK_NUM) ||
        (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_info_get: descriptor invalid, descriptor = %d.\n"
            , ulTaskDesc);
        return (CPSS_ERROR);
    }
    
    /* 获得任务ID. */
    
    taskId = g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;
    
#ifdef CPSS_VOS_VXWORKS
    if (taskInfoGet (taskId, & tDesc) == ERROR)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_pri_get: taskInfoGet failed.\n"
            );
        return (CPSS_ERROR);
    }
    
    /* 获得任务信息. */
    
    pstTaskInfo->ulTaskDesc = ulTaskDesc;
    pstTaskInfo->taskId = tDesc.td_id;
    pstTaskInfo->pcName = tDesc.td_name;
    /* 优先级逆映射. */

#if 0    
    switch (tDesc.td_priority)
    {
    case 104:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_TIME_CRITICAL;
            break;
        }
        
    case 134:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_HIGHEST;
            break;
        }
        
    case 164:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_ABOVE_NORMAL;
            break;
        }
        
    case 194:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_NORMAL;
            break;
        }
        
    case 224:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_BELOW_NORMAL;
            break;
        }
        
    case 254:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_LOWEST;
            break;
        }
        
    case 255:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_IDLE;
            break;
        }
        
    default:
        {
            cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_info_get: priority illegal, priority = %d.\n"
                , tDesc.td_priority);
            
            break;
        }
    }
  #endif
 
    pstTaskInfo->lPri = tDesc.td_priority;
    pstTaskInfo->lStatus = tDesc.td_status;
    
    return (CPSS_OK);
#endif /* CPSS_VOS_VXWORKS */
    
#ifdef CPSS_VOS_WINDOWS
    /* 获得任务信息. */
    
    pstTaskInfo->ulTaskDesc = ulTaskDesc;
    pstTaskInfo->taskId = g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;
    pstTaskInfo->pcName = taskId->acName;
    switch (taskId->lPri)
    {
    case THREAD_PRIORITY_TIME_CRITICAL:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_TIME_CRITICAL;
            break;
        }
        
    case THREAD_PRIORITY_HIGHEST:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_HIGHEST;
            break;
        }
        
    case THREAD_PRIORITY_ABOVE_NORMAL:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_ABOVE_NORMAL;
            break;
        }
        
    case THREAD_PRIORITY_NORMAL:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_NORMAL;
            break;
        }
        
    case THREAD_PRIORITY_BELOW_NORMAL:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_BELOW_NORMAL;
            break;
        }
        
    case THREAD_PRIORITY_LOWEST:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_LOWEST;
            break;
        }
        
    case THREAD_PRIORITY_IDLE:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_IDLE;
            break;
        }
        
    default:
        {
            cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_info_get: priority illegal, priority = %d.\n"
                , taskId->lPri);
            break;
        }
    }
    
    pstTaskInfo->lStatus = 0;
    
    return (CPSS_OK);
#endif /* CPSS_VOS_WINDOWS */

#ifdef CPSS_DSP_CPU
    /* 获得任务信息. */
    
    pstTaskInfo->ulTaskDesc = ulTaskDesc;
    pstTaskInfo->taskId = g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;
    pstTaskInfo->pcName = taskId->acName;
    pstTaskInfo->lPri = taskId->lPri;
    pstTaskInfo->lStatus = 0;
    
    return (CPSS_OK);
    
#endif

#ifdef CPSS_VOS_LINUX
    /* 获得任务信息. */

    pstTaskInfo->ulTaskDesc = ulTaskDesc;
    pstTaskInfo->taskId = g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;
    pstTaskInfo->pcName = taskId->acName;
    switch (taskId->lPri)
    {
    case 35:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_TIME_CRITICAL;
            break;
        }

    case 30:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_HIGHEST;
            break;
        }

    case 25:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_ABOVE_NORMAL;
            break;
        }

    case 20:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_NORMAL;
            break;
        }

    case 15:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_BELOW_NORMAL;
            break;
        }

    case 10:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_LOWEST;
            break;
        }

    case 5:
        {
            pstTaskInfo->lPri = TASK_PRIORITY_IDLE;
            break;
        }

    default:
        {
            cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_task_info_get: priority illegal, priority = %d.\n"
                , taskId->lPri);
            break;
        }
    }

    pstTaskInfo->lStatus = 0;

    return (CPSS_OK);
#endif /* CPSS_VOS_LINUX */
}

/*******************************************************************************
* 函数名称: cpss_vos_task_summary
* 功    能: 此函数获得任务i概况
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* pstTaskInfo       CPSS_VOS_TASK_INFO_T *       输入
* 函数返回: 
* 说    明:                                 
*******************************************************************************/    
 void cpss_vos_task_summary
(
CPSS_VOS_TASK_INFO_T * pstTaskInfo /* 指向任务信息的指针 */
)
{
#ifdef CPSS_VOS_VXWORKS
    INT8    acStatusString [10];
    
    /* 获得任务字符. */
    
    taskStatusString (pstTaskInfo->taskId, acStatusString);
    
    /* 2006/5/15, jlm added */
    cps__oams_shcmd_printf ("%-20s  %-15d  0x%-8x  %-8d  %-10s\n", 
                       pstTaskInfo->pcName, pstTaskInfo->ulTaskDesc, 
                       pstTaskInfo->taskId, pstTaskInfo->lPri, acStatusString);
    /* 2006/5/15, jlm */
    
#endif /* CPSS_VOS_VXWORKS */
    
#ifdef CPSS_VOS_WINDOWS
    cps__oams_shcmd_printf ("%-18s  %-12d  0x%-8x  %-8d  -\n", pstTaskInfo->pcName, 
                       pstTaskInfo->ulTaskDesc, (INT32) pstTaskInfo->taskId, 
                       pstTaskInfo->lPri);
#endif /* CPSS_VOS_WINDOWS */

#ifdef CPSS_VOS_LINUX
    cps__oams_shcmd_printf ("%-18s  %-12d  0x%-8x  %-8d  -\n", pstTaskInfo->pcName,
                       pstTaskInfo->ulTaskDesc, (INT32) pstTaskInfo->taskId,
                       pstTaskInfo->lPri);
#endif /* CPSS_VOS_WINDOWS */

#ifdef CPSS_DSP_CPU
    cps__oams_shcmd_printf ("%-10s  %-15d  0x%-8x  %-8d  -\n", pstTaskInfo->pcName, 
        pstTaskInfo->ulTaskDesc, (INT32) pstTaskInfo->taskId, 
        pstTaskInfo->lPri);
#endif /* CPSS_VOS_WINDOWS */
}

/*******************************************************************************
* 函数名称: cpss_vos_task_show
* 功    能: 此函数显示一个任务信息
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: CPSS_OK or CPSS_ERROR
* 说    明:                                 
*******************************************************************************/    
INT32 cpss_vos_task_show 
(
UINT32  ulTaskDesc, /* 任务描述符 */
INT32   lLevel /* 0 = 概况, 2 = 所有任务 */
)
{
CPSS_VOS_TASK_INFO_T   stTaskInfo;

switch (lLevel)
{
case 0 :
    {
        /* 获得任务信息. */
        
        if (cpss_vos_task_info_get (ulTaskDesc, & stTaskInfo) == CPSS_ERROR)
        {
            return (CPSS_ERROR);
        }
        
        cps__oams_shcmd_printf ("\n");
        cps__oams_shcmd_printf (" Task  name      Task descriptor   Task ID   Priority  Status\n");
        cps__oams_shcmd_printf ("-------------    ---------------  ---------  --------  ------\n");
        
        /* 任务概况. */
        cpss_vos_task_summary (& stTaskInfo);
        
        break;
    }
    
case 2 :
default :
    {
        cps__oams_shcmd_printf ("\n");
        cps__oams_shcmd_printf (" Task  name      Task descriptor   Task ID   Priority  Status\n");
        cps__oams_shcmd_printf ("-------------    ---------------  ---------  --------  ------\n");
        
        /* 找到一个任务描述符. */
        
        for (ulTaskDesc = 1; ulTaskDesc < VOS_MAX_TASK_NUM; ulTaskDesc++)
        {
            if (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == TRUE)
            {
                /* 获得信息. */
                if (cpss_vos_task_info_get (ulTaskDesc, & stTaskInfo) == 
                    CPSS_OK)
                {
                    /* 任务概况. */
                    cpss_vos_task_summary (& stTaskInfo);
                }
            }
        }
        
        break;
    }
}

return (CPSS_OK);
}


CPSS_TASK_REG_MNG_T g_pfCpssRegTaskEntry[CPSS_TASK_REG_NUM_MAX];

void cpss_task_mng_init()
{
    cpss_mem_memset((void*)g_pfCpssRegTaskEntry, 0, 
        CPSS_TASK_REG_NUM_MAX*sizeof(CPSS_TASK_REG_MNG_T));
}

/* ulP1 配置的顺序，从0开始，最大为: CPSS_TASK_REG_NUM_MAX */
UINT32 cgx_test_count = 0;
void cpss_test_task_dealing()
{
    cgx_test_count = cgx_test_count + 1;
    printf("test=%d\n",cgx_test_count);
}

extern VOID cpss_kw_set_task_run(VOID);
extern VOID cpss_kw_set_task_stop(VOID);

void cpss_reg_task_entry(UINT32 ulP1)
{
    if (CPSS_TASK_REG_NUM_MAX <= ulP1)
    {
        printf ("\n cpss_task_entry: input ulP1(%d) overflow. it must below %d \n", 
            ulP1, CPSS_TASK_REG_NUM_MAX);

        return ;
    }
    
    g_pfCpssRegTaskEntry[ulP1].ulSemDesc = cpss_vos_sem_c_create(VOS_SEM_Q_FIFO, 0, 1);
    if (VOS_SEM_DESC_INVALID == g_pfCpssRegTaskEntry[ulP1].ulSemDesc)
    {
        printf ("\n cpss_task_entry: Sem Create failed! \n");

        return ;
    }
    
    while (TRUE)
    {
        cpss_kw_set_task_stop();
        cpss_vos_sem_p(g_pfCpssRegTaskEntry[ulP1].ulSemDesc, WAIT_FOREVER);
/*for test ,there can not be blocked!*//*cpss_delay(g_pfCpssRegTaskEntry[ulP1].ulIntVal);*/
	    cpss_kw_set_task_run();	
        if (g_pfCpssRegTaskEntry[ulP1].ulUseFlag != 0)
        {
            g_pfCpssRegTaskEntry[ulP1].pfTaskEntry();
        }
    }
}

/*INT32 cpss_task_register(UINT32 ulTaskIdx, CPSS_TASK_ENTRY_FUNCPTR pfTaskEntry, UINT32 ulIntVal)*/
INT32 cpss_vos_task_register(CPSS_TASK_ENTRY_FUNCPTR pfTaskEntry, UINT32 ulIntVal)
{

    UINT32 ulTaskIdx = 2;
    if (g_pfCpssRegTaskEntry[ulTaskIdx].ulUseFlag == 0)
        {
            g_pfCpssRegTaskEntry[ulTaskIdx].pfTaskEntry = pfTaskEntry;
            g_pfCpssRegTaskEntry[ulTaskIdx].ulIntVal = ulIntVal;
            g_pfCpssRegTaskEntry[ulTaskIdx].lCurIntVal = (INT32)ulIntVal;
            g_pfCpssRegTaskEntry[ulTaskIdx].ulTaskIdx = ulTaskIdx;
            g_pfCpssRegTaskEntry[ulTaskIdx].ulUseFlag = 1;
            g_pfCpssRegTaskEntry[ulTaskIdx].ulCallCount = 0;

            return (CPSS_OK);
        }
    else
    {
        printf("\n Input ulTaskIdx is overflow!!! \n");
        return (CPSS_ERROR);
    }

    return (CPSS_ERROR);
}

INT32 cpss_pci_recv_task_register(CPSS_TASK_ENTRY_FUNCPTR pfTaskEntry, UINT32 ulIntVal)
{
    UINT32 ulTaskIdx = 1;

    if (g_pfCpssRegTaskEntry[ulTaskIdx].ulUseFlag == 0)
    {
        g_pfCpssRegTaskEntry[ulTaskIdx].pfTaskEntry = pfTaskEntry;
        g_pfCpssRegTaskEntry[ulTaskIdx].ulIntVal = ulIntVal;
        g_pfCpssRegTaskEntry[ulTaskIdx].lCurIntVal = (INT32)ulIntVal;
        g_pfCpssRegTaskEntry[ulTaskIdx].ulTaskIdx = ulTaskIdx;
        g_pfCpssRegTaskEntry[ulTaskIdx].ulUseFlag = 1;
        g_pfCpssRegTaskEntry[ulTaskIdx].ulCallCount = 0;

        return (CPSS_OK);
    }
    else
    {
        printf("\n Input ulTaskIdx is overflow!!! \n");
        return (CPSS_ERROR);
    }
}

void mytest()
{

    /*cpss_test_task_dealing();*/
    cpss_vos_task_register((CPSS_TASK_ENTRY_FUNCPTR)cpss_test_task_dealing, 100);
}

#define CPSS_TM_HDR_INT_UNIT (2) /* unit as ms. */
VOID cpss_tm_hdr_int_isr()
{
    INT32 lCount;
	
    g_ulCpssMicroSec+=(CPSS_TM_HDR_INT_UNIT*1000);
	
    for (lCount = 0; lCount < CPSS_TASK_REG_NUM_MAX; lCount++)
    {
        if (g_pfCpssRegTaskEntry[lCount].ulUseFlag != 0)
        {
            g_pfCpssRegTaskEntry[lCount].lCurIntVal -= CPSS_TM_HDR_INT_UNIT; 
            if (g_pfCpssRegTaskEntry[lCount].lCurIntVal <= 0)
            {
                cpss_vos_sem_v(g_pfCpssRegTaskEntry[lCount].ulSemDesc);
                g_pfCpssRegTaskEntry[lCount].lCurIntVal = g_pfCpssRegTaskEntry[lCount].ulIntVal;
                g_pfCpssRegTaskEntry[lCount].ulCallCount = g_pfCpssRegTaskEntry[lCount].ulCallCount + 1;
            }
        }
    }
#ifdef CPSS_VOS_VXWORKS
     cpss_kw_set_task_run_time(CPSS_TM_HDR_INT_UNIT);
#endif
}

void cpss_regtask_show()
{
    INT32 lCount;

    cps__oams_shcmd_printf ("\n CPSS RegTask Info: \n");
    for (lCount = 0; lCount < CPSS_TASK_REG_NUM_MAX; lCount++)
    {
        if (g_pfCpssRegTaskEntry[lCount].ulUseFlag != 0)
        {
            cps__oams_shcmd_printf ("\n The %d taskInfo: RegPtr[%#x], IntVal[%d ms]. CurIntVal[%d ms]. taskIdx[%d], call count[%d]\n", 
                            lCount,
                            g_pfCpssRegTaskEntry[lCount].pfTaskEntry,
                            g_pfCpssRegTaskEntry[lCount].ulIntVal,
                            g_pfCpssRegTaskEntry[lCount].lCurIntVal,
                            g_pfCpssRegTaskEntry[lCount].ulTaskIdx,
                            g_pfCpssRegTaskEntry[lCount].ulCallCount);
        }
    }

    return ;
}

#ifndef CPSS_DSP_CPU
extern VK_TASK_CONFIG_T gatVkTaskConfig[];

#define CPSS_TASK_REGISTER_MAGIC 0xf0e0d0c0
#define CPSS_TASK_USED_MAGIC     0x7f6f5f4f
typedef struct
{
    CPSS_TASK_INIT_FUNCPTR    pfInit;
    CPSS_TASK_BLOCK_FUNCPTR   pfBlock;
    CPSS_TASK_DEALING_FUNCPTR pfDealing;
    
    UINT32 ulTaskType;
    UINT8  aucTaskName[VOS_TASK_NAME_LEN];
    UINT32 ulBlockParamList;
    UINT32 ulTaskIdx; /* only for check with self's index. */
    UINT32 ulUseFlag; 
    UINT32 ulRegFlag;
    UINT32 ulCallCount;
    
    /* event flag */
    UINT32 ulEventFlag;
    UINT32 ulEventParam;
}CPSS_NORMAL_TASK_INFO_T;

typedef struct
{
    CPSS_NORMAL_TASK_INFO_T *pstNTaskInfo;
    INT32 lNTaskMax;
    UINT32 ulSemDesc;
    BOOL   bInitFlag;
}CPSS_NORMAL_TASK_MNG_T;

/*  通用的任务框架 */
CPSS_NORMAL_TASK_MNG_T g_stCpssNTaskMng;
/*******************************************************************************
* 函数名称: cpss_vos_ntask_reg_mng_init
* 功    能: 遍历普通任务配置表，得到最大任务数目，赋值给 g_ulCpssVosNTaskMax;
*           初始化gp_pfCpssNTaskMng，申请内存，并初始化各成员变量。创建每个普通任务。
* 函数类型: INT32
* 参    数: 无
* 参数名称  类型   输入/输出  描述
*           无
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
INT32 cpss_vos_ntask_reg_mng_init()
{
    INT32 lC = 0;
    UINT32 i = 0;
    if (g_stCpssNTaskMng.bInitFlag)
    {
        cpss_output(CPSS_MODULE_VOS,CPSS_PRINT_ERROR,
                "cpss_vos_ntask_reg_mng_init: I am Init already.!");
        return (CPSS_OK);
    }
    
    memset(&g_stCpssNTaskMng, 0, sizeof(CPSS_NORMAL_TASK_MNG_T));
    
    while (strlen(gatVkTaskConfig[g_stCpssNTaskMng.lNTaskMax].acName))
    {
        g_stCpssNTaskMng.lNTaskMax = g_stCpssNTaskMng.lNTaskMax + 1;
        if (g_stCpssNTaskMng.lNTaskMax > VOS_MAX_TASK_NUM)
        {
            cpss_output(CPSS_MODULE_VOS,CPSS_PRINT_ERROR,
                "Config Normal Task Num(%d) is overflow:!");
            return (CPSS_ERROR);
        }
    }
    
    g_stCpssNTaskMng.pstNTaskInfo = (CPSS_NORMAL_TASK_INFO_T*)malloc(
                 g_stCpssNTaskMng.lNTaskMax*sizeof(CPSS_NORMAL_TASK_INFO_T));
    if (NULL == g_stCpssNTaskMng.pstNTaskInfo)
    {
        cpss_output(CPSS_MODULE_VOS,CPSS_PRINT_ERROR,
                "CPSS VOS NTASK INIT failed by malloc(%d) \n", g_stCpssNTaskMng.lNTaskMax*sizeof(CPSS_NORMAL_TASK_MNG_T));
        return (CPSS_ERROR);
    }
    /* init all member to 0. */
    memset((void*)g_stCpssNTaskMng.pstNTaskInfo, 0, 
        g_stCpssNTaskMng.lNTaskMax*sizeof(CPSS_NORMAL_TASK_INFO_T));

    while (strlen(gatVkTaskConfig[i].acName))
    {
        memcpy(g_stCpssNTaskMng.pstNTaskInfo[i].aucTaskName,gatVkTaskConfig[i].acName,strlen(gatVkTaskConfig[i].acName));
        i++;
        if (g_stCpssNTaskMng.lNTaskMax > VOS_MAX_TASK_NUM)
        {
            cpss_output(CPSS_MODULE_VOS,CPSS_PRINT_ERROR,
                "Config Normal Task Num(%d) is overflow: %d \n", g_stCpssNTaskMng.lNTaskMax, VOS_MAX_TASK_NUM);
            return (CPSS_ERROR);
        }
        
    }
    /* Init sem for register. */
    g_stCpssNTaskMng.ulSemDesc = cpss_vos_sem_m_create(VOS_SEM_Q_PRI);
    if (VOS_SEM_DESC_INVALID == g_stCpssNTaskMng.ulSemDesc)
    {
        cpss_output(CPSS_MODULE_VOS,CPSS_PRINT_ERROR,
                "cpss_vos_ntask_reg_mng_init: Sem Create failed! \n");
        return (CPSS_ERROR);
    }
    
    g_stCpssNTaskMng.bInitFlag = TRUE;

    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: INT32 cpss_vos_ntask_register
* 功    能: 注册任务的初始化函数 阻塞函数 处理函数
* 函数类型: INT32
* 参    数: 无
* 参数名称  类型   输入/输出  描述
*           UINT32
* 函数返回: 
* 说    明:可以调用2次，但是第2次调用时，必需将所有参数填充正确，会覆盖第1次填充的内容。                                 
*******************************************************************************/
INT32 cpss_vos_ntask_register(
              UINT32 ulTaskType,
              CPSS_TASK_INIT_FUNCPTR    pfInit, 
              CPSS_TASK_BLOCK_FUNCPTR   pfBlock,
              CPSS_TASK_DEALING_FUNCPTR pfDealing,
              UINT32 *pulBlockParamList,
              UINT8  *pucTaskName)
{
    CPSS_NORMAL_TASK_INFO_T *pstNTaskMng;
    INT32 lC;
    UINT32 ulTaskNameLen;
    
    if (g_stCpssNTaskMng.bInitFlag == FALSE )
    {
        cpss_output(CPSS_MODULE_VOS,CPSS_PRINT_ERROR,
                " cpss_vos_ntask_register: failed! I have not init yet now..., Please Init me first! ");
        return (CPSS_ERROR);
    }
    if (strlen(pucTaskName) >= VOS_TASK_NAME_LEN)
    {
        ulTaskNameLen = VOS_TASK_NAME_LEN;
    }
    else
    {
        ulTaskNameLen = strlen(pucTaskName) + 1;
    }
    pstNTaskMng = g_stCpssNTaskMng.pstNTaskInfo;

    cpss_vos_sem_p(g_stCpssNTaskMng.ulSemDesc, WAIT_FOREVER);
    /* Maybe app register 2 times. first time only register init funcPtr, the Second times register others. */
    for ( lC = 0; lC < g_stCpssNTaskMng.lNTaskMax; lC++ )
    {
        if (memcmp(pucTaskName, pstNTaskMng->aucTaskName, ulTaskNameLen - 1) == 0)
        {		
            break;
        }
        pstNTaskMng++;/* add by zhangpeng 2007-1-22 14:41*/
    }
    cpss_vos_sem_v(g_stCpssNTaskMng.ulSemDesc);

    if (lC >= g_stCpssNTaskMng.lNTaskMax )
    {
        cpss_output(CPSS_MODULE_VOS,CPSS_PRINT_ERROR,
                "Input ulTaskIdx is overflow!!! max is %d", g_stCpssNTaskMng.lNTaskMax);
        return (CPSS_ERROR);
    }

    if (pstNTaskMng->ulRegFlag == CPSS_TASK_REGISTER_MAGIC)
    {
        cpss_output(CPSS_MODULE_VOS,CPSS_PRINT_ERROR,
                "task already reg ");
        return (CPSS_ERROR);
    }
     
    pstNTaskMng->pfInit = pfInit;
    pstNTaskMng->pfBlock = pfBlock;
    pstNTaskMng->pfDealing = pfDealing;
    pstNTaskMng->ulTaskType = ulTaskType;
    pstNTaskMng->ulBlockParamList = (UINT32)pulBlockParamList;

    /*memcpy(pstNTaskMng->aucTaskName, pucTaskName, ulTaskNameLen);*/

    pstNTaskMng->ulRegFlag = CPSS_TASK_REGISTER_MAGIC;
    pstNTaskMng->ulCallCount = 0;

    return (CPSS_OK);
}

void cpss_vos_ntask_entry(INT32 lIdx)
{
    CPSS_TASK_BLOCK_FUNCPTR pfBlock;
    CPSS_TASK_DEALING_FUNCPTR pfDealing ;
    CPSS_TASK_INIT_FUNCPTR pfInit;
    UINT32 *pulBlockParamList;
    CPSS_NORMAL_TASK_INFO_T *pstNTaskMng;
    INT32 lRet;
    UINT32 i = 0;
    UINT32 ulTaskNameLen = 0;
    INT32 lC = 0;
    
    cpss_vos_sem_p(g_stCpssNTaskMng.ulSemDesc, WAIT_FOREVER);
	
    while (strlen(gatVkTaskConfig[i].acName))
    {
        if (lIdx == gatVkTaskConfig[i].lArg)
        {
            break;
        }
        i++;
    }

    if (strlen(gatVkTaskConfig[i].acName) >= VOS_TASK_NAME_LEN)
    {
        ulTaskNameLen = VOS_TASK_NAME_LEN;
    }
    else
    {
        ulTaskNameLen = strlen(gatVkTaskConfig[i].acName) + 1;
    }
    pstNTaskMng = g_stCpssNTaskMng.pstNTaskInfo;

    for ( lC = 0; lC < g_stCpssNTaskMng.lNTaskMax; lC++ )
    {
        if (memcmp(gatVkTaskConfig[i].acName, pstNTaskMng->aucTaskName, ulTaskNameLen - 1) == 0)
        {
            break;
        }
        pstNTaskMng += 1;
    }

    cpss_vos_sem_v(g_stCpssNTaskMng.ulSemDesc);

    if(lC >= g_stCpssNTaskMng.lNTaskMax)
    {
        return ;
    }
	
    if (CPSS_TASK_USED_MAGIC == pstNTaskMng->ulUseFlag )
    {
        cpss_output(CPSS_MODULE_VOS,CPSS_PRINT_ERROR,
                "cpss_vos_ntask_entry: Idx(%d) config repeat... ", lIdx);
        return ;
    }

    pstNTaskMng->ulUseFlag = CPSS_TASK_USED_MAGIC;

    while (1)
    {
        if (CPSS_TASK_REGISTER_MAGIC == pstNTaskMng->ulRegFlag)
        {
            break;
        }
        else
        {
            cpss_vos_task_delay(CPSS_TASK_NO_INIT_DELAY_TIME);
        }
    }
	
    pfInit = pstNTaskMng->pfInit;
    if (pfInit == NULL)
    {
        cpss_output(CPSS_MODULE_VOS,CPSS_PRINT_ERROR,
                "Task(%d) init function ptr is NULL! Maybe u do not need one init function... ", lIdx);
    }
    else
    {
        lRet = pfInit();
        if (lRet != CPSS_OK)
        {
            cpss_output(CPSS_MODULE_VOS,CPSS_PRINT_ERROR,
                "Task(%d) init failed! errcode = %#x \n", lIdx, lRet);
            return ;
        }
    }
    
    pfBlock = pstNTaskMng->pfBlock;
    if (NULL == pfBlock)
    {
        cpss_output(CPSS_MODULE_VOS,CPSS_PRINT_ERROR,
                "cpss_vos_ntask_entry: Idx(%d) Block func is NULL... ", lIdx);
        return ;
    }
    pfDealing = pstNTaskMng->pfDealing;
    if (NULL == pfDealing)
    {
        cpss_output(CPSS_MODULE_VOS,CPSS_PRINT_ERROR,
                "cpss_vos_ntask_entry: Idx(%d) Dealing func is NULL... ", lIdx);
        return ;
    }
    pulBlockParamList = (UINT32*)pstNTaskMng->ulBlockParamList;
    if (NULL == pulBlockParamList)
    {
        cpss_output(CPSS_MODULE_VOS,CPSS_PRINT_ERROR,
                "cpss_vos_ntask_entry: Idx(%d) Block func's param is NULL...,Maybe u do not need one param list. ", lIdx);
    }
    
    while(1)
    {
        
        /* set self enter "No Run" status. */
        /* insert check point for control event. such as soft update online event, etc. */
         cpss_kw_set_task_stop() ;
        lRet = pfBlock(pulBlockParamList);       
        /* set self enter "Run" status. */
        /* insert check point for control event. such as soft update online event, etc. */
        cpss_kw_set_task_run() ;
        /* call app's dealing func. */
        lRet = pfDealing(lRet, pulBlockParamList);
        if (lRet != CPSS_OK)
        {
            /* set errcode */
            continue;
        }
    }
    
    return ;
}
#endif

#ifdef CPSS_VOS_VXWORKS

UINT32  cpss_vos_find_task_desc_by_tcb(WIND_TCB *pTcb)
{
   UINT32 usTaskDesc;
   
   for(usTaskDesc = 0; usTaskDesc < VOS_MAX_TASK_NUM;usTaskDesc++)
   {
     if(g_astCpssVosTaskDescTbl[usTaskDesc].bInuse == TRUE)
     	{
     	    if(pTcb == taskTcb(g_astCpssVosTaskDescTbl[usTaskDesc].taskId))
     	    	{
     	    	   return usTaskDesc;
     	    	}
     	}
    
   }
   return VOS_TASK_DESC_INVALID;

}


void cpss_taskSwitchHook
(
WIND_TCB *pOldTcb,    

WIND_TCB *pNewTcb  
)

{
  UINT32 ulTaskDescOld;
  UINT32 ulTaskDescNew;
  UINT32 ulMicroTimeStart;
  UINT16 usSwitchTimes;

  if(!g_bCpssTaskSwitch)
  {
    return;
  }
  
  ulTaskDescOld = cpss_vos_find_task_desc_by_tcb(pOldTcb);
  ulTaskDescNew = cpss_vos_find_task_desc_by_tcb(pNewTcb);

  if((ulTaskDescOld == VOS_TASK_DESC_INVALID)||(ulTaskDescNew == VOS_TASK_DESC_INVALID))
  {
     return ;
  }

  usSwitchTimes =  g_azCpssSwitchInCount[ulTaskDescNew];
/* 纪录切入时间*/
 if(usSwitchTimes < 32)
  g_azCpssSwitchInTime[ulTaskDescNew][usSwitchTimes][0]=cpss_microsecond_counter_get();
/* 纪录切出次数*/
  g_azCpssSwitchInCount[ulTaskDescOld]++;

 usSwitchTimes =  g_azCpssSwitchInCount[ulTaskDescOld];

/* 纪录切出时间*/  
 if(usSwitchTimes < 32)
  g_azCpssSwitchInTime[ulTaskDescOld][usSwitchTimes][1] = cpss_microsecond_counter_get();

#if 1
  if(ulTaskDescNew <VOS_MAX_TASK_NUM)
  g_astCpssVosTaskDescTbl[ulTaskDescNew].lMicroTimeStart = cpss_microsecond_counter_get();

   if(ulTaskDescOld <VOS_MAX_TASK_NUM)
  g_astCpssVosTaskDescTbl[ulTaskDescOld].lMicroTimeEnd = cpss_microsecond_counter_get();

   if((ulTaskDescOld <VOS_MAX_TASK_NUM)&&(g_astCpssVosTaskDescTbl[ulTaskDescOld].lMicroTimeStart != 0))
  g_astCpssVosTaskDescTbl[ulTaskDescOld].lMicroTime = g_astCpssVosTaskDescTbl[ulTaskDescOld].lMicroTimeEnd - g_astCpssVosTaskDescTbl[ulTaskDescOld].lMicroTimeStart;

#endif
}



VOID cpss_show_task_run_time()
{
     UINT32 usTaskDesc;
   printf("\n\r%-20s%-15s%-15s%-15s\n\r","NAME","Start(us)","End(us)","Run(us)"); 
   for(usTaskDesc = 0; usTaskDesc < VOS_MAX_TASK_NUM;usTaskDesc++)
   {
     if(g_astCpssVosTaskDescTbl[usTaskDesc].bInuse == TRUE)
     {
       printf("%-20s%-15d%-15d%-15d\n\r",g_astCpssVosTaskDescTbl[usTaskDesc].acName,g_astCpssVosTaskDescTbl[usTaskDesc].lMicroTimeStart,
          g_astCpssVosTaskDescTbl[usTaskDesc].lMicroTimeEnd,
           g_astCpssVosTaskDescTbl[usTaskDesc].lMicroTime);
      
        g_astCpssVosTaskDescTbl[usTaskDesc].lMicroTimeStart = 0;
        g_astCpssVosTaskDescTbl[usTaskDesc].lMicroTimeEnd = 0;
        g_astCpssVosTaskDescTbl[usTaskDesc].lMicroTime = 0;
        
                                    
     }
   } 
    
}

#endif
/*******************************************************************************
* 函数名称:  cpss_sbbr_output_task_msg_queue
* 功    能:输出所有任务的消息队列排队消息数
*    
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* ulDrvUserId            UINT32                  in                           黑匣子用户id
* 函数返回: 
*            
* 说    明:                                  
*******************************************************************************/
#ifndef CPSS_DSP_CPU
 VOID   cpss_sbbr_output_task_status(UINT32 ulDrvUserId)
 {
 
#ifdef CPSS_VOS_VXWORKS
   UINT16 ulTaskDesc;
   UINT32 ulArgs[4];
   CPSS_VOS_TASK_INFO_T   stTaskInfo;

  INT8    acStatusString [10];
    
  
   for (ulTaskDesc = 1; ulTaskDesc < VOS_MAX_TASK_NUM; ulTaskDesc++)
        {
            if (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == TRUE)
            {
                /* 获得信息. */
                if (cpss_vos_task_info_get (ulTaskDesc, &stTaskInfo) == 
                    CPSS_OK)
                {
                    /* 获得任务字符. */
    
                    taskStatusString (stTaskInfo.taskId, acStatusString);
                   cpss_sbbr_write_text(ulDrvUserId,ulArgs,"\n\r  task name: %s  status:%s ",stTaskInfo.pcName,acStatusString);
                }
            }
        }
  #endif 
  }

#endif

VOID cpss_vos_suspend_all_task(VOID)
{

#ifdef CPSS_VOS_VXWORKS
   UINT16 ulTaskDesc;

     for (ulTaskDesc = 1; ulTaskDesc < VOS_MAX_TASK_NUM; ulTaskDesc++)
        {
            if (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == TRUE)
            {
                taskSuspend(g_astCpssVosTaskDescTbl[ulTaskDesc].taskId);
               
            }
        }

#endif


}


VOID cpss_delay(UINT32 useconds)
{
#if (SWP_OS_TYPE == SWP_OS_LINUX)
usleep(useconds * 1000);
#endif

#if (SWP_OS_TYPE == SWP_OS_WINDOWS)
Sleep(useconds);
#endif
}

int pthread_suspend_stub(pthread_t pt)
{
	return 0;
}

int pthread_continue_stub(pthread_t pt)
{
	return 0;
}

/******************************* 源文件结束 **********************************/
