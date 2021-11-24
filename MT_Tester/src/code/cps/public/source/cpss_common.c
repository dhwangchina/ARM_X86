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
#include "swp_config.h"

#include "cpss_public.h"
#include "cpss_common.h"

#include "cpss_vk_proc.h"
#include "cpss_vk_sched.h"
#include "cpss_vos_msg_q.h"
#include "cpss_vos_sem.h"
#include "cpss_vos_task.h"

#include "cpss_com_link.h"
#include "cpss_tm_timer.h"
#include "cpss_mm_mem.h"
#ifdef CPSS_VOS_VXWORKS
#include "taskHookLib.h"
#endif


/*add begin 2010_0521 for Segmentation fault trace*/
#ifndef SWP_CYGWIN
#include <pthread.h>
#include <execinfo.h>
#include <signal.h>
#endif
/*add end 2010_0521 for Segmentation fault trace*/

#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

/******************************* 局部宏定义 **********************************/
/******************************* 全局变量定义/初始化 *************************/
/* VK模块自己的管理表：纤程类型表和纤程描述表 */
extern CPSS_VK_PROC_CLASS_T  g_astCpssVkProcClassTbl [VK_MAX_PROC_CLASS_NUM];
extern CPSS_VK_PROC_DESC_T  g_astCpssVkProcDescTbl [VK_MAX_PROC_INST_NUM];
/* 业务部分的纤程配置表 */
extern CPSS_VK_PROC_CONFIG_T gatCpssVkProcCfgForUser[];

/* 调度任务表项的个数 */
INT32   glVkSchedConfigNumEnt = 0;

/* 本CPU字节序全局标志位：BIG_ENDIAN/LITTLE_ENDIAN */
UINT32 g_ulByteOrder = 0xffffffff;  /* local CPU's byteOrder. */
INT32 g_CpsSystemDbg;
extern VOID cpss_dbg_sim_shell_init(VOID)  ; 
extern UINT32 g_BootMsgId;
#ifndef SWP_CYGWIN
static void trace_print(int viSig, void *pSigInfo, void *pContext);
void print_stack(FILE* cps_f, unsigned char *vpucBuf, unsigned int vuiSize);
#define MAX_TRACE_PCS 128
#endif


#ifdef CPSS_VOS_VXWORKS   
  extern INT32 cpss_cpu_load_init(VOID);
extern void cpss_taskSwitchHook
(
WIND_TCB *pOldTcb,    

WIND_TCB *pNewTcb  
);
#endif
 extern INT32 cpss_sbbr_init
(
VOID
);

extern VOID cpss_kw_read_normal_task_info(UINT32 ulTaskDesc,UINT32 ulConfig);
extern VOID   cpss_kw_read_proc_class_info(UINT16 usGuid);
extern INT32  cpss_kw_ha_init();
extern INT32 cpss_vos_ntask_reg_mng_init();
extern INT32 cpss_com_irq_task_reg();
void cpss_message(const CHAR *pcFormat, ...);
extern VOID cps_user_group_init();
extern INT32 cps_devm_sysinfo_init();

/******************************* 局部常数和类型定义 **************************/

/******************************* 局部函数原型声明 ****************************/

/******************************* 函数实现 ************************************/


/*******************************************************************************
* 函数名称: cpss_normal_task_create
* 功    能:        普通任务创建
* 函数类型:  内部
* 参    数: 无
* 参数名称          类型        输入/输出       描述
*
* 函数返回: 
* CPSS_OK       成功
* CPSS_ERROR    失败
* 说    明: 
*******************************************************************************/
INT32 cpss_normal_task_create()
{
    UINT32 ulConfig = 0;
    UINT32  ulTaskDesc;
    
    while (strlen(gatVkTaskConfig[ulConfig].acName))
    {
        if (ulConfig > VOS_MAX_TASK_NUM)
        {
            cpss_message ("\n Config Normal Task Num(%d) is overflow: %d \n", ulConfig, VOS_MAX_TASK_NUM);
            return (CPSS_ERROR);
        }
        
        if (gatVkTaskConfig[ulConfig].bInuse == FALSE)
        {
            ulConfig = ulConfig + 1;
            continue;
        }
        
        ulTaskDesc = cpss_vos_task_spawn (gatVkTaskConfig[ulConfig].acName, 
            gatVkTaskConfig[ulConfig].lPri,
            gatVkTaskConfig[ulConfig].lOptions,
            gatVkTaskConfig[ulConfig].lStackSize
            , gatVkTaskConfig[ulConfig].pfEntry,
            gatVkTaskConfig[ulConfig].lArg);
        
        if (ulTaskDesc == VOS_TASK_DESC_INVALID)
        {
            return (CPSS_ERROR);
        }
         cpss_kw_read_normal_task_info(ulTaskDesc,ulConfig);
        ulConfig = ulConfig + 1;
    }

    return (CPSS_OK);
    
}

/*******************************************************************************
* 函数名称: cpss_sch_task_create
* 功    能:        调度任务创建
* 函数类型:  内部
* 参    数: 无
* 参数名称          类型        输入/输出       描述
*
* 函数返回: 
* CPSS_OK       成功
* CPSS_ERROR    失败
* 说    明: 
*******************************************************************************/
INT32 cpss_sch_task_create()
{
    UINT32 ulConfig = 0;
    UINT32  ulSchedDesc;

    while (strlen(gatVkSchedConfig[ulConfig].acName))
    {
        if (ulConfig > VK_MAX_SCHED_NUM)
        {
            cpss_message ("\n Config Sch Task Num(%d) is overflow: %d \n", ulConfig, VK_MAX_SCHED_NUM);
            return (CPSS_ERROR);
        }
        
        if (gatVkSchedConfig[ulConfig].bInuse == FALSE)
        {
            ulConfig = ulConfig + 1;
            continue;
        }
        
        ulSchedDesc = cpss_vk_sched_spawn (gatVkSchedConfig[ulConfig].acName, 
            gatVkSchedConfig[ulConfig].lPri,
            gatVkSchedConfig[ulConfig].lOptions
            ,
            gatVkSchedConfig[ulConfig].lStackSize
            ,
            gatVkSchedConfig[ulConfig].pfEntry)
            ;
        
        if (ulSchedDesc == VK_SCHED_DESC_INVALID)
        {
            return (CPSS_ERROR);
        }
        ulConfig = ulConfig + 1;
    }

    glVkSchedConfigNumEnt = ulConfig;

    return (CPSS_OK);
    
}

/*******************************************************************************
* 函数名称: cpss_proc_create
* 功    能:        纤程创建
* 函数类型:  内部
* 参    数: 无
* 参数名称          类型        输入/输出       描述
*
* 函数返回: 
* CPSS_OK       成功
* CPSS_ERROR    失败
* 说    明: 
*******************************************************************************/
INT32 cpss_proc_create(CPSS_VK_PROC_CONFIG_T *pastProcConfig, UINT32 ulCurrentBase, UINT32 *pulTotalNum)
{
    UINT32  ulConfig=ulCurrentBase;
    UINT32  ulBegin = ulCurrentBase;
    UINT16 usProcInst;
    UINT16  usGuid;
    UINT32  ulIndex;
    UINT32  ulProcDesc;
    
    while (pastProcConfig->ulPID != CPSS_VK_PD_INVALID)
    {
        if (ulConfig > VK_MAX_PROC_CLASS_NUM)
        {
            cpss_message ("\n Config Proc Type Num(%d) is overflow: %d \n", ulConfig, VK_MAX_PROC_CLASS_NUM);
            return (CPSS_ERROR);
        }
        /* 该情况不注册. */
        if (pastProcConfig->bInuse == FALSE)
        {
            ulConfig = ulConfig + 1;
            continue;
        }
        
        /* 获得纤程类别. */
        
        usGuid = CPSS_VK_PTYPE(pastProcConfig->ulPID);
        if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM))
        {
            cpss_message ("\n ProcType value(%d) is overflow: %d.\n File: %s. Line: %d.\n", usGuid, VK_MAX_PROC_CLASS_NUM,__FILE__,__LINE__);
            return (CPSS_ERROR);
        }
        
        /* 初始化内部队列. */
        
        cpss_util_q_fifo_init (& g_astCpssVkProcClassTbl[usGuid].qFree);
        cpss_util_q_fifo_init (& g_astCpssVkProcClassTbl[usGuid].qInuse);
                
        /* 初始化纤程其余部分. */
        
        g_astCpssVkProcClassTbl[usGuid].usGuid = CPSS_VK_PTYPE(pastProcConfig->ulPID); 

        strcpy (g_astCpssVkProcClassTbl[usGuid].acName, 
            pastProcConfig->acName);
        
        g_astCpssVkProcClassTbl[usGuid].lPri = pastProcConfig->lPri; 
        g_astCpssVkProcClassTbl[usGuid].lOptions = pastProcConfig->lOptions
            ;
        g_astCpssVkProcClassTbl[usGuid].lStackSize = pastProcConfig->lStackSize
            ; 
        g_astCpssVkProcClassTbl[usGuid].lVarSize = pastProcConfig->lVarSize
            ;
        g_astCpssVkProcClassTbl[usGuid].pfEntry = pastProcConfig->pfEntry
            ;
        g_astCpssVkProcClassTbl[usGuid].ulBegin = ulBegin;
        g_astCpssVkProcClassTbl[usGuid].usTotalInst = pastProcConfig->usTotalInst
            ;
        g_astCpssVkProcClassTbl[usGuid].usCurrInst = 0;
        g_astCpssVkProcClassTbl[usGuid].ulSchedDesc = pastProcConfig->ulSchedDesc
            ;
        g_astCpssVkProcClassTbl[usGuid].bInuse = TRUE;
	  g_astCpssVkProcClassTbl[usGuid].lKwTmOutLimitType = pastProcConfig->lKwTmOutLimitType;
        
        /* 将所有实例放入空闲队列 */
        usProcInst = 0;
        for (ulIndex = ulBegin; 
        ulIndex < (ulBegin + pastProcConfig->usTotalInst); 
        ulIndex++)
        {
            if (ulIndex >= VK_MAX_PROC_INST_NUM)
            {
                cpss_message ("\n Config Proc Inst Num(%d) is overflow: %d \n", ulConfig, VK_MAX_PROC_INST_NUM);
                return (CPSS_ERROR);
            }
            
            cpss_util_q_fifo_put (& g_astCpssVkProcClassTbl[usGuid].qFree, 
                & g_astCpssVkProcDescTbl[ulIndex].node,
                UTIL_Q_FIFO_KEY_TAIL);
            /* 创建纤程 */
            ulProcDesc = cpss_vk_proc_spawn (usGuid, usProcInst);
            usProcInst = usProcInst + 1;
            if (ulProcDesc == CPSS_VK_PD_INVALID)
            {
                UINT16 usProcInstTmp = usProcInst - 1;
                cpss_message ("\n CPSS_PROC_CREATE: cpss_vk_proc_spawn failed. usGuid = %d, usProcInst = %d \n", usGuid, usProcInstTmp);
                return (CPSS_ERROR);
            }
        }
        /* 统计 */
        
        ulBegin += g_astCpssVkProcClassTbl[usGuid].usTotalInst;
        ulConfig = ulConfig + 1;
        pastProcConfig = pastProcConfig + 1;
        cpss_kw_read_proc_class_info(usGuid);		
        
    }

    *pulTotalNum = ulConfig;
    
    return (CPSS_OK);
    
}

INT32 cpss_proc_config_init()
{
    INT32 lRet;
    UINT32 ulTotalProcNum = 0;
    
    lRet = cpss_proc_create(gatVkProcClassConfig, 0, &ulTotalProcNum);
    if (CPSS_OK != lRet)
    {
    printf("File: %s.Line:%d\n",__FILE__,__LINE__);
    return (CPSS_ERROR);
    }
    
    return (CPSS_OK);
    
}
/*******************************************************************************
* 函数名称: cpss_message
* 功    能: cpss_message打印函数
* 函数类型: 外部
* 参    数: 无
* 参数名称          类型        输入/输出       描述
*
* 函数返回: （各个返回值的注释?
* CPSS_OK       成功
* CPSS_ERROR    失败
* 说    明: （可选。描述本函数使用是需要注意的地方，实现方法上的关键点。）                                 
*******************************************************************************/
void cpss_message(const CHAR *pcFormat, ...)
{
#ifdef CPSS_DSP_CPU
    return;
#else
	va_list pParList;
	va_start(pParList,pcFormat);
	vprintf(pcFormat,pParList);
#endif
}
/*******************************************************************************
* 函数名称: cpss_dsp_sbbr_mem_init
* 功    能: 
* 函数类型: 外部
* 参    数: 无
* 参数名称          类型        输入/输出       描述
*
* 函数返回: 
* 无
* 说    明: 
*******************************************************************************/
void cpss_dsp_sbbr_mem_init()
{
#ifdef CPSS_DSP_CPU
    memset((UINT8*)CPSS_TEST_PCI_ADDR_BASE, 0, CPSS_TEST_PCI_MEM_SIZE);  
    memset((UINT8*)CPSS_DSP_SBBR_START_ADDR, 0, 5*CPSS_DSP_SBBR_SUBSYS_LEN);
    memset((UINT8*)CPSS_DSP_CPU_LOAD_ADDR, 0xffffffff, 4);
#endif
}

/*******************************************************************************
* 函数名称: cpss_proc_activate
* 功    能: 纤程激活的封装函数
* 函数类型: 外部
* 参    数: 无
* 参数名称          类型        输入/输出       描述
*
* 函数返回: 
* 说    明: 
*******************************************************************************/
void cpss_proc_activate()
{
	cpss_vk_proc_activate(CPSS_COM_LINK_MNGR_PROC);
	
#if (SWP_PHYBRD_TYPE == SWP_PHYBRD_MASA)
	cpss_vk_proc_activate(SMSS_SHMTEMPER_PROC);
#endif
}
/*******************************************************************************
* 函数名称: cpss_vos_normal_task_reg
* 功    能: 普通任务的注册函数
* 函数类型: 外部
* 参    数: 无
* 参数名称          类型        输入/输出       描述
*
* 函数返回: 
* 说    明: 
*******************************************************************************/
INT32 cpss_vos_normal_task_reg()
{
#ifndef SWP_FNBLK_BRDTYPE_ABOX
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
    INT32 ret = 0;
    ret = cpss_com_udp_task_reg();
    if(CPSS_OK != ret)
    {
        cpss_message ("cpss_com_udp_task_reg error.\n");
        return CPSS_ERROR;
    }

    ret = cpss_com_irq_task_reg();
    if(CPSS_OK != ret)
    {
        cpss_message ("cpss_com_irq_task_reg error.\n");
        return CPSS_ERROR;
    }
#ifdef SWP_FNBLK_STRUCT_MS
    ret = cpss_com_pci_task_reg();
    if(CPSS_OK != ret)
    {
        cpss_message ("cpss_com_pci_task_reg error.\n");
        return CPSS_ERROR;
    }

#endif
    ret = cpss_com_slidtimer_task_reg();
    if(CPSS_OK != ret)
    {
        cpss_message ("cpss_com_slidtimer_task_reg error.\n");
        return CPSS_ERROR;
    }

    ret = cpss_timer_scan_task_reg();
    if(CPSS_OK != ret)
    {
        cpss_message ("cpss_timer_scan_task_reg error.\n");
        return CPSS_ERROR;
    }

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL                                                                                                             
#ifdef CPSS_CPS__TNBS_TCP_SIM   
    ret = cpss_com_tcp_listen_task_reg();
    if(CPSS_OK != ret)
    {
        cpss_message ("cpss_com_tcp_listen_task_reg error.\n");
        return CPSS_ERROR;
    }

    ret = cpss_com_dc_listen_task_reg();
    if(CPSS_OK != ret)
    {
        cpss_message ("cpss_com_dc_listen_task_reg error.\n");
        return CPSS_ERROR;
    }

    ret = cpss_com_tcp_recv_task_reg();
    if(CPSS_OK != ret)
    {
        cpss_message ("cpss_com_tcp_recv_task_reg error.\n");
        return CPSS_ERROR;
    }

#endif
#endif

#elif(SWP_CPUROLE_TYPE == SWP_CPUROLE_SLAVE)

    INT32 ret = 0;
    ret = cpss_timer_scan_task_reg();
    if(CPSS_OK != ret)
    {
        cpss_message ("cpss_timer_scan_task_reg error.\n");
        return CPSS_ERROR;
    }   
    ret = cpss_com_pci_task_reg();
    if(CPSS_OK != ret)
    {
        cpss_message ("cpss_com_pci_task_reg error.\n");
        return CPSS_ERROR;
    }
    ret = cpss_com_irq_task_reg();
    if(CPSS_OK != ret)
    {
        cpss_message ("cpss_com_irq_task_reg error.\n");
        return CPSS_ERROR;
    }
#ifdef CPSS_PCI_SIM_BY_UDP
    ret = cpss_com_slidtimer_task_reg();
    if(CPSS_OK != ret)
    {
        cpss_message ("cpss_com_slidtimer_task_reg error.\n");
        return CPSS_ERROR;
    }
    ret = cpss_com_udp_task_reg();
    if(CPSS_OK != ret)
    {
        cpss_message ("cpss_com_udp_task_reg error.\n");
        return CPSS_ERROR;
    }

#endif
#endif
#endif
    return CPSS_OK;
}
/*******************************************************************************
* 函数名称: cpss_file_init
* 功    能: 纤程激活的封装函数
* 函数类型: 外部
* 参    数: 无
* 参数名称          类型        输入/输出       描述
*
* 函数返回: 
* 说    明: 
*******************************************************************************/
INT32 cpss_file_init()
{
#ifndef CPSS_DSP_CPU 
    cpss_message ("\nFile system initialize...\n");
    if (cpss_fs_init () != CPSS_OK)
    {
        cpss_message ("\nFile system initialize... failed\n");   
        return (CPSS_ERROR);
    }    
    return CPSS_OK;
#endif

}
/*******************************************************************************
* 函数名称: cpss_init
* 功    能: cpss初始化
* 函数类型: 外部
* 参    数: 无
* 参数名称          类型        输入/输出       描述
*
* 函数返回: （各个返回值的注释?
* CPSS_OK       成功
* CPSS_ERROR    失败
* 说    明: （可选。描述本函数使用是需要注意的地方，实现方法上的关键点。） 
*******************************************************************************/
#ifdef CPSS_VOS_VXWORKS
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
extern INT32 tftpdResponsePriority;
extern INT32 tftpTaskPriority;
#endif
#endif
INT32 cpss_init (VOID)
{
    UINT32  ulConfig;
    INT32   lRet;

#ifndef SWP_CYGWIN
    signal(SIGSEGV, trace_print);
    signal(SIGBUS, trace_print);
    signal(SIGFPE, trace_print);
#endif

    cpss_dsp_sbbr_mem_init();
    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulCpssInitInCount);
    
#ifdef CPSS_VOS_VXWORKS
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)

    tftpdResponsePriority = CPSS_TASKPRI_NORMAL;
    tftpTaskPriority = CPSS_TASKPRI_NORMAL;
#endif
#endif
    ulConfig = 0;
    /* 检查本CPU字节序类型：BIG/LITTLE */
    cpss_byte_order_init();
    
    /* 初始库函数 */
    
    cpss_message ("\nTask library initialize...\n");
    
    if (cpss_vos_task_lib_init () != CPSS_OK)
    {
        cpss_message ("failed.\n");
        
        return (CPSS_ERROR);
    }
    
    cpss_message ("done.\n");
    
    cpss_message ("\nSemaphore library initialize...\n");
    
    if (cpss_vos_sem_lib_init () != CPSS_OK)
    {
        cpss_message ("failed.\n");
        
        return (CPSS_ERROR);
    }
    
    cpss_message ("done.\n");
    
    cpss_message ("\nMessage queue library initialize...\n");
    
    if (cpss_vos_msg_q_lib_init () != CPSS_OK)
    {
        cpss_message ("failed.\n");
        
        return (CPSS_ERROR);
    }
    
    cpss_message ("done.\n");
    
    cpss_message ("\nMemory library initialize...\n");  
    
    if (cpss_mem_init () != CPSS_OK)
    {
        cpss_message ("failed.\n");
        
        return (CPSS_ERROR);
    }
    
    cpss_message ("done.\n");
    
    cpss_message ("\nTimer library initialize...\n");
    
    if (cpss_timer_init () != CPSS_OK)
    {
        cpss_message ("failed.\n");
        
        return (CPSS_ERROR);
    }
    
    cpss_message ("done.\n");

#ifdef CPSS_VOS_VXWORKS    
    cpss_message ("\nCpu load  initialize...\n");
    
    if(cpss_cpu_load_init()!= CPSS_OK)
        {
           cpss_message ("failed.\n");
        
           return (CPSS_ERROR);
        }
    cpss_message ("done.\n");
#endif

    cpss_message ("\nScheduler library initialize...\n");
    
    if (cpss_vk_sched_lib_init () != CPSS_OK)
    {
        cpss_message ("failed.\n");
        
        return (CPSS_ERROR);
    }
    
    cpss_message ("done.\n");
    
    cpss_message ("\nProcedure library initialize...\n");
    
    if (cpss_vk_proc_lib_init () != CPSS_OK)
    {
        cpss_message ("failed.\n");
        
        return (CPSS_ERROR);
    }
    
    cpss_message ("done.\n");
    
    cpss_message ("\nMessage pool create...\n");
    
    if (cpss_ipc_msg_pool_create () != CPSS_OK)
    {
        cpss_message ("failed.\n");
        
        return (CPSS_ERROR);
    }
    
    cpss_message ("done.\n");
    
#ifdef CPSS_UDP_INCLUDE
    cpss_message ("\n OS SOCKET Init...\n");
    lRet = cpss_com_os_socket_init();
    if (CPSS_OK != lRet)
    {
        cpss_message ("failed.\n");
        
        return (CPSS_ERROR);
    }
    cpss_message ("done.\n");
#endif
#ifndef CPSS_DSP_CPU 
    cpss_message ("\n COM TIMESTAMP Init...\n");
    lRet = cpss_com_timestamp_init();
    if (CPSS_OK != lRet)
    {
        cpss_message ("failed.\n");
        
        return (CPSS_ERROR);
    }
    cpss_message ("done.\n");
#endif    
  cpss_message ("\n cpss_kw_ha_initt...\n");
  if(cpss_kw_ha_init() != CPSS_OK)
  	{
           cpss_message ("failed.\n");
        
         return (CPSS_ERROR);
  	}
    cpss_message ("done.\n");

    cpss_message ("\n Normal Reg mng_Init...\n");

#ifndef CPSS_DSP_CPU 
#if (SWP_FUNBRD_TYPE != SWP_FUNBRD_ABOX) 
    lRet = cpss_vos_ntask_reg_mng_init();
    if (CPSS_ERROR == lRet)
    {
        cpss_message ("failed.\n");
        
        return (CPSS_ERROR);
    }

    cpss_message ("done.\n");
#endif
#endif

    /* 创建任务 */
    cpss_message ("\n Normal Task Init...\n");
    lRet = cpss_normal_task_create();
    if (CPSS_OK != lRet)
    {
        cpss_message ("failed.\n");
        
        return (CPSS_ERROR);
    }
    cpss_message ("done.\n");

    cpss_message ("\n Normal Task Reg...\n");

    lRet = cpss_vos_normal_task_reg();
    if (CPSS_OK != lRet)
    {
        cpss_message ("failed.\n");
        
        return (CPSS_ERROR);
    }
    cpss_message ("done.\n");

    /* 创建调度任务. */
    cpss_message ("\n Sch Task Init...\n");
    lRet = cpss_sch_task_create();
    if (CPSS_OK != lRet)
    {
        cpss_message ("failed.\n");
        
        return (CPSS_ERROR);
    }
    cpss_message ("done.\n");

    
    /* 注册纤程类. */
    cpss_message ("\n Proc Init...\n");
    lRet = cpss_proc_config_init();
    if (CPSS_OK != lRet)
    {
    printf("File:%s. Line: %d.\n",__FILE__,__LINE__);
    cpss_message ("failed.\n");
        
    return (CPSS_ERROR);
    }
    cpss_message ("done.\n");
   
    /* sbbr init */
#ifndef CPSS_DSP_CPU 
    cpss_message ("\nSbbr  initialize...\n");
   
    if(cpss_sbbr_init() !=CPSS_OK)
     {
         cpss_message ("failed.\n");
        
     }
     cpss_message ("done.\n");
#endif 

    /***************** 检测纤程死循环 ********************/
    cpss_message ("\nInfinite LOOP detect Init...\n");
    lRet = cpss_vk_infinite_loop_detect_start();
    if (CPSS_OK != lRet)
    {
        cpss_message ("failed.\n");

        return (CPSS_ERROR);
    }
    cpss_message ("done.\n");
    /***************** 检测纤程死循环 end ********************/
    cpss_dbg_sim_shell_init();
    
    #if (CPSS_TRACE_ROUTE_FLAG == TRUE)
    cpss_trace_route_mng_init();
    #endif


#ifdef CPSS_DSP_CPU
    cpss_vk_proc_activate(CPSS_COM_LINK_MNGR_PROC);
#endif

    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulCpssInitOutCount);
    cpss_dbg_high_time_init();
    cpss_dbg_rec_time_init();
#ifndef SWP_CYGWIN
    cps_user_group_init();
    cps_devm_sysinfo_init();
	
#if 0
    cps_com_boot_msg_init();
    cpss_ipport_init();
    cpss_route_ipv4_init();
#endif
#endif
/****    
 #ifdef CPSS_VOS_VXWORKS
    taskSwitchHookAdd((FUNCPTR)cpss_taskSwitchHook);
 #endif
****/
    return (CPSS_OK);
}
  
/*******************************************************************************
* 函数名称: cpss_local_funcbrd_type_is_mc
* 功    能: 判断本板是否为全局板
* 函数类型: 内部
* 参    数: 无
* 参数名称        类型                输入/输出     描述
* 
* 函数返回: 
* TRUE ;
* FALSE;
* 说    明:
*******************************************************************************/
#if 0
BOOL cpss_local_funcbrd_type_is_mc(VOID)
{
    if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }    
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_rarp_allow
* 功    能:      判断本板是否需要发送RARP消息
* 函数类型: 内部
* 参    数: 无
* 参数名称        类型                输入/输出     描述
* 
* 函数返回: 
* TRUE ;
* FALSE;
* 说    明:
*******************************************************************************/
BOOL cpss_com_rarp_allow()
{
#ifndef CPSS_PCI_SIM_BY_UDP

    if ((SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
        /*|| (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP) */
        || (SWP_CPUROLE_TYPE == SWP_CPUROLE_SLAVE))
    {
        return (FALSE);
    }
    else
    {
        return (TRUE);
    }
#endif
    return TRUE ;
}

#ifndef CPSS_DSP_CPU

UINT32 g_ulCpssBFPLM = 16;
UINT32 g_ulCpssBFPM = 64;
void cpss_com_byteflow_plm_set(UINT32 ulLineMax)
{
    g_ulCpssBFPLM = ulLineMax;

    return ;
}

void cpss_com_byteflow_pm_set(INT32 lPrintMax)
{
    g_ulCpssBFPM = lPrintMax;

    return ;
}

VOID cpss_byteflow_print(UINT8 *pucBuf, UINT32 ulLen)
{
    UINT32 ulC;
    UINT32 ulPrintNum;

    printf (" CPSS BYTE FLOW PRINT Begin: \n\n");
    if (ulLen > g_ulCpssBFPM)
    {
        ulPrintNum = g_ulCpssBFPM;
    }
    else
    {
        ulPrintNum = ulLen;
    }
    for (ulC = 0; ulC < ulPrintNum; ulC++)
    {
        if (pucBuf[ulC] > 0xf)
        {
            printf(" 0x0%x ", pucBuf[ulC]);
        }
        else
        {
            printf(" 0x%x ", pucBuf[ulC]);
        }
        if ((CPSS_MOD((ulC + 1),g_ulCpssBFPLM) == 0) && (ulC != 0))
        {
            printf ("\n");
        }
    }
    printf ("\n\n CPSS BYTE FLOW PRINT End. \n");

    cpss_tm_current_time_print();

}

INT32 cpss_com_byteflow_print_ex(UINT8 *pucBuf, UINT32 ulLen)
{
    cpss_byteflow_print(pucBuf, ulLen);
    cpss_com_byteflow_print_hook(pucBuf, ulLen);
    return (CPSS_OK);
}

typedef struct
{
    UINT32 ulLocationA;
    UINT32 ulKeyA;
    UINT32 ulLocationB;
    UINT32 ulKeyB;
    UINT32 ulCounter;
}CPSS_COM_BYTEFLOW_PRINT_HOOK_T;

CPSS_COM_BYTEFLOW_PRINT_HOOK_T g_stCpssComBFPH;
#define CPSS_COM_BFPH_INVALID (0xaaaaaaaa)

void cpss_com_bfph_init()
{
    cpss_mem_memset(&g_stCpssComBFPH, (UINT8)0xaa, 
        sizeof(CPSS_COM_BYTEFLOW_PRINT_HOOK_T));

    return ;
}

void cpss_com_bfph_set(UINT32 ulP1, UINT32 ulP2, UINT32 ulP3, UINT32 ulP4)
{
    if (CPSS_COM_BFPH_INVALID != ulP1)
    {
        g_stCpssComBFPH.ulLocationA = ulP1;
    }
    else
    {
        g_stCpssComBFPH.ulLocationA = CPSS_COM_BFPH_INVALID;
    }

    if (CPSS_COM_BFPH_INVALID != ulP2)
    {
        g_stCpssComBFPH.ulKeyA = ulP2;
    }
    else
    {
        g_stCpssComBFPH.ulKeyA = CPSS_COM_BFPH_INVALID;
    }

    if (CPSS_COM_BFPH_INVALID != ulP3)
    {
        g_stCpssComBFPH.ulLocationB = ulP3;
    }
    else
    {
        g_stCpssComBFPH.ulLocationB = CPSS_COM_BFPH_INVALID;
    }

    if (CPSS_COM_BFPH_INVALID != ulP4)
    {
        g_stCpssComBFPH.ulKeyB = ulP4;
    }
    else
    {
        g_stCpssComBFPH.ulKeyB = CPSS_COM_BFPH_INVALID;
    }

    g_stCpssComBFPH.ulCounter = 0;

    return ;
}

void cpss_com_byteflow_print_hook(UINT8 *pucBuf, UINT32 ulLen)
{
    UINT32 ulA;
    UINT32 ulB;

    if (CPSS_COM_BFPH_INVALID != g_stCpssComBFPH.ulLocationA)
    {
        if (g_stCpssComBFPH.ulLocationA > ulLen)
        {
            return ;
        }
        ulA = *(UINT32*)(pucBuf + g_stCpssComBFPH.ulLocationA);
        if (ulA == g_stCpssComBFPH.ulKeyA)
        {
            if (CPSS_COM_BFPH_INVALID != g_stCpssComBFPH.ulLocationB)
            {
                if (g_stCpssComBFPH.ulLocationB > ulLen)
                {
                    return ;
                }
                ulB = *(UINT32*)(pucBuf + g_stCpssComBFPH.ulLocationB);
                if (ulB == g_stCpssComBFPH.ulKeyB)
                {
                    g_stCpssComBFPH.ulCounter = g_stCpssComBFPH.ulCounter + 1;
                }
            }
            else
            {
                g_stCpssComBFPH.ulCounter = g_stCpssComBFPH.ulCounter + 1;
            }
        }
    }
}

void cpss_tm_current_time_print()
{
    CPSS_TIME_T stClock;

    cpss_clock_get(&stClock);

    printf ("\n CPSS CUR TIME: %d-%d-%d %d:%d:%d \n", 
        stClock.usYear,
        stClock.ucMonth,
        stClock.ucDay,
        stClock.ucHour,
        stClock.ucMinute,
        stClock.ucSecond);
}

typedef INT32 (*CPSS_SWITCH_HOOK_FUNCPTR)(UINT32 ulP1, UINT32 ulP2, UINT32 ulP3);

CPSS_SWITCH_HOOK_FUNCPTR g_pfCpssSH = NULL;
void cpss_switch_hook_record(UINT8 *pucBuf, UINT32 ulLen)
{
    cpss_byteflow_print(pucBuf, ulLen);
}

void cpss_switch_hook(UINT32 ulPid, UINT32 ulMsgId)
{
    INT32 lRet;
    UINT32 ulSchId;
    
    if (NULL == g_pfCpssSH)
    {
        return ;
    }
    ulSchId = cpss_vk_sched_desc_self();

    lRet = g_pfCpssSH(ulPid, ulSchId, ulMsgId);
    if (CPSS_OK == lRet)
    {
        UINT32 ucBuf[8];
        ucBuf[0] = ulPid;
        ucBuf[1] = ulSchId;
        ucBuf[2] = ulMsgId;
        cpss_switch_hook_record((UINT8*)ucBuf, 12);
    }
    
}
#endif

/*******************************************************************************
* 函数名称: cpss_byte_order_init
* 功    能: 判断本CPU是BIG还是LITTLE。
* 函数类型: 内部
* 参    数: 
* 参数名称          类型        输入/输出       描述
* 
* 函数返回:
* 无
* 说    明: 
*******************************************************************************/
VOID cpss_byte_order_init()
{
    UINT16 ulX = 0;
    UINT8 *pP1 = NULL;

    ulX = 1;
    pP1 = (INT8*)&ulX;
    if(*pP1 == 1)
    {
        g_ulByteOrder = CPSS_BYTE_ORDER_LITTLE_ENDIAN;
    }
    else
    {
        g_ulByteOrder = CPSS_BYTE_ORDER_BIG_ENDIAN;
    }
    
    return ;
    
}

/*******************************************************************************
* 函数名称: cpss_byte_order_change_ushort
* 功    能: 16位字节序倒换函数。
* 函数类型: 外部
* 参    数: 
* 参数名称          类型        输入/输出       描述
*  usNet
* 函数返回: （各个返回值的注释?
* CPSS_OK       成功
* CPSS_ERROR    失败
* 说    明: 
*******************************************************************************/
UINT16 cpss_byte_order_change_ushort 
(
    UINT16 usNet
)
{
    UINT16 usRet = 0;
    
    if (CPSS_BYTE_ORDER_BIG_ENDIAN == g_ulByteOrder)
    {
        usRet = usNet;
    }
    else
    {
        usRet = (usNet & 0xff00) >> 8;
        usRet += ((usNet & 0x00ff) << 8);
    }
    
    return (usRet);
    
}

/*******************************************************************************
* 函数名称: cpss_byte_order_change_ulong
* 功    能: 32位字节序倒换函数。
* 函数类型: 外部
* 参    数: 
* 参数名称          类型        输入/输出       描述
*  usNet
* 函数返回: （各个返回值的注释?
* CPSS_OK       成功
* CPSS_ERROR    失败
* 说    明: 
*******************************************************************************/
UINT32 cpss_byte_order_change_ulong 
(
    UINT32 ulNet
)
{
    UINT32 ulRet = 0;
    
    if (CPSS_BYTE_ORDER_BIG_ENDIAN == g_ulByteOrder)
    {
        ulRet = ulNet;
    }
    else
    {
        ulRet = (ulNet & 0xff000000) >> 24;
        ulRet += ((ulNet & 0x00ff0000) >> 8);
        ulRet += ((ulNet & 0x0000ff00) << 8);
        ulRet += ((ulNet & 0x000000ff) << 24);
    }
    
    return (ulRet);
    
}

/*add begin 2010_0521 for Segmentation fault trace*/
#ifdef SWP_LINUX
int backtrace_x86(void **buffer, int size, void *upc)
{
	return backtrace(buffer, size);
}
#else
void get_trace_offset(unsigned long *pc, size_t *pra, size_t *psp)
{
	int endflag;
	unsigned long *addr;

	endflag = 0;
	*pra = *psp = 0;
	for (addr = pc; !*pra || !*psp; --addr)
	{
		switch (*addr & 0xffff0000)
		{
		case 0x27bd0000:
		case 0x67bd0000:
			if (0 == *psp)
				*psp = abs((short) (*addr & 0xffff));
			break;
		case 0xafbf0000:
		case 0xffbf0000:
			if (0 == *pra)
				*pra = (short) (*addr & 0xffff);
			break;
		case 0x3c1c0000: //出现两次再退出
			if (endflag)
				return;
			endflag = 1;
		default:
			break;
		}
	}
}

#include <ucontext.h>
int backtrace_mips32(void **buffer, int size, ucontext_t *upc)
{
#define abs(s) ((s) < 0 ? -(s) : (s))
	unsigned long *addr;
	unsigned long *pc;
	unsigned long *ra;
	unsigned long *sp;
	size_t ra_offset;
	size_t stack_size;
	int depth;

	if (!buffer || size <= 0)
		return 0;

	pc = (unsigned long*) upc->uc_mcontext.pc;
	ra = (unsigned long*) upc->uc_mcontext.gregs[31];
	sp = (unsigned long*) upc->uc_mcontext.gregs[29];

	printf("Segment fault: pc = %p, ra = %p, sp = %p\n", pc, ra, sp);

	buffer[0] = pc;

	get_trace_offset(pc, &ra_offset, &stack_size);
	ra = *(unsigned long **) ((unsigned long) sp + ra_offset + 4);
	sp = (unsigned long *) ((unsigned long) sp + stack_size);

	/* scanning to find the size of the current stack-frame */
	for (depth = 1; depth < size && ra; ++depth)
	{
		buffer[depth] = ra;
		get_trace_offset(ra, &ra_offset, &stack_size);
		if (*(unsigned long **) ((unsigned long) sp + ra_offset) != 0) //32位
			return depth + 1;
		ra = *(unsigned long **) ((unsigned long) sp + ra_offset + 4);
		sp = (unsigned long *) ((unsigned long) sp + stack_size);
	}
	return depth;
}
#endif

char** cps_get_fun_stack(void *pContext, unsigned int *pSum)
{
	unsigned int uiSum;
	char **pString;
	void* buffer[MAX_TRACE_PCS];
#ifdef SWP_LINUX
	uiSum = backtrace_x86(buffer, MAX_TRACE_PCS, pContext);
#else
	uiSum = backtrace_mips32(buffer, MAX_TRACE_PCS, pContext);
#endif
	pString = backtrace_symbols(buffer, uiSum);
	*pSum = uiSum;

	return pString;
}

#ifndef SWP_LINUX
int backtrace_mips32_nosignal(void **buffer, int size)
{
	unsigned long *ra;
	unsigned long *sp;
	size_t ra_offset;
	size_t stack_size;
	int depth = 0;

	if (!size)
		return 0;
	if (!buffer || size < 0)
		return -1;

	// get current $ra and $sp
	__asm__ __volatile__ (
			"       move    %0, $ra\n"
			"       move    %1, $sp\n"
			: "=r"(ra), "=r"(sp)
	);

	get_trace_offset((unsigned long) backtrace_mips32_nosignal + 32, &ra_offset, &stack_size);
	sp = (unsigned long *) ((unsigned long) sp + stack_size);

	// repeat backward scanning
	for (depth = 0; depth < size && ra; ++depth)
	{
		buffer[depth] = ra;
		get_trace_offset(ra, &ra_offset, &stack_size);
		if (0 != *(unsigned long **) ((unsigned long) sp + ra_offset))
			return depth + 1;
		ra = *(unsigned long **) ((unsigned long) sp + ra_offset + 4);
		sp = (unsigned long *) ((unsigned long) sp + stack_size);
	}

	return depth;
}
#endif

void show_current_call()
{
	void* buffer[MAX_TRACE_PCS];
	unsigned int uiSum;
	char **pString;
	unsigned int i;

#ifdef SWP_LINUX
	uiSum = backtrace(buffer, MAX_TRACE_PCS);
#else
	uiSum = backtrace_mips32_nosignal(buffer, MAX_TRACE_PCS);
#endif
	pString = backtrace_symbols(buffer, uiSum);
	printf("\n#################################################\n");
	for (i = 0; i < uiSum; i++)
	{
		printf("%03d:    %s\n", i, pString[i]);
	}
	printf("\n#################################################\n");
	free(pString);
}

void backtrace_print_stack(void *pContext)
{
	unsigned char *pucBaseStack;
	unsigned char *pucCurrentStack;
	unsigned long ulStackSize;
	unsigned long ulTotalSize;
	long iSize;
	unsigned int i;
	char **pString;
	unsigned int depth;

	pthread_t tThread;
	pthread_attr_t tThreadAttr;

	FILE* pFile;
	struct tm sttime;
	time_t timenow;
	char filename[256] = "/tmp/";

	time(&timenow);
	localtime_r(&timenow, &sttime);

	sprintf(filename + strlen(filename), "CPS_Segment_Fault_Log_%04u-%02u%02u-%02u:%02u:%02u.txt", sttime.tm_year + 1900, sttime.tm_mon + 1, sttime.tm_mday, sttime.tm_hour, sttime.tm_min,
			sttime.tm_sec);
	printf("%d-%d-%d-%d:%d reset see logfile %s\n", sttime.tm_year + 1900, sttime.tm_mon + 1, sttime.tm_mday, sttime.tm_hour, sttime.tm_min, filename);

	pFile = fopen(filename, "w");
	if (NULL == pFile)
	{
		printf("fopen %s failure\n", filename);
		return;
	}

	tThread = pthread_self();
	pthread_getattr_np(tThread, &tThreadAttr);
	pthread_attr_getstack(&tThreadAttr, &pucBaseStack, &ulTotalSize);

	printf("##############################################################################\n");
	fprintf(pFile, "##############################################################################\n");
#ifdef SWP_LINUX
	pucCurrentStack = &pucBaseStack;
#else
	pucCurrentStack = ((ucontext_t*) pContext)->uc_mcontext.gregs[29];
	printf("stack current pc address:    %p\n", ((ucontext_t*) pContext)->uc_mcontext.pc);
	fprintf(pFile, "stack current pc address:    %p\n", ((ucontext_t*) pContext)->uc_mcontext.pc);
#endif
	ulStackSize = (unsigned long) pucBaseStack + ulTotalSize - (unsigned long) pucCurrentStack;
	printf("stack high address:          %p\n", pucBaseStack + ulTotalSize);
	printf("stack low address:           %p\n", pucBaseStack);
	printf("stack current address:       %p\n", pucCurrentStack);
	printf("stack total size:            %lu\n", ulTotalSize);
	printf("stack current size:          %lu\n", ulStackSize);
	fprintf(pFile, "stack high address:          %p\n", pucBaseStack + ulTotalSize);
	fprintf(pFile, "stack low address:           %p\n", pucBaseStack);
	fprintf(pFile, "stack current address:       %p\n", pucCurrentStack);
	fprintf(pFile, "stack total size:            %lu\n", ulTotalSize);
	fprintf(pFile, "stack current size:          %lu\n", ulStackSize);

	iSize = (ulStackSize + 0xF) & 0xFFFFFFF0;
	pucCurrentStack = (char*) ((unsigned long) pucCurrentStack & 0xFFFFFFF0);
	while (iSize > 0)
	{
		printf("%08X:  ", (unsigned long) pucCurrentStack);
		fprintf(pFile, "%08X:  ", (unsigned long) pucCurrentStack);
		for (i = 0; i < 16; i++)
		{
			printf("%02X ", *pucCurrentStack);
			fprintf(pFile, "%02X ", *pucCurrentStack++);
			if (((i + 1) % 4) == 0)
			{
				printf(" ");
				fprintf(pFile, " ");
			}
		}
		iSize -= 16;
		printf("\n");
		fprintf(pFile, "\n");
	}
	printf("\n");
	fprintf(pFile, "\n");

	pString = cps_get_fun_stack(pContext, &depth);
	if (NULL == pString)
	{
		fclose(pFile);
		return;
	}
	printf("##############################################################################\n");
	fprintf(pFile, "##############################################################################\n");
	for (i = 0; i < depth; i++)
	{
		printf("%03u:\t%s\n", i, pString[i]);
		fprintf(pFile, "%03u:\t%s\n", i, pString[i]);
	}
	free(pString);
	fclose(pFile);
}

/*add begin 2010_0521 for Segmentation fault trace*/
static void trace_print(int viSig, void *pSigInfo, void *pContext)
{
	CPS_REBOOT_T stRebt;

	backtrace_print_stack(pContext);

	stRebt.ucRebtType = CPS_CPU_REBOOT_HOT;
	stRebt.ucRebtRsn = CPS_REBOOT_BY_SEGFAULT;
	stRebt.szExtra = "segment fault";

	cps_boot_reboot(&stRebt);

	//	exit(1);
}

/*system函数，新创建的进程不继承原进程的fd*/
INT32 cps_system(UINT8* pucCmd)
{
	pid_t stPid;
	INT32 lRet;
	INT32 lLoop;
	INT32 alFlag[256];
	INT32 lBegin;

	lBegin = 3;

#if 0
	if(0 == g_CpsSystemDbg)
	{
		/*消除调用system时产生的打印*/
		lBegin = 0;
	}
#endif

	/*参数判断*/
	if(NULL == pucCmd)
	{
		return CPSS_OK;
	}

	/*获取原进程的fd状态*/
	for(lLoop = lBegin; lLoop < 256; lLoop++)
	{
		alFlag[lLoop] = fcntl(lLoop, F_GETFD, 0);
		if(-1 == alFlag[lLoop])
		{
			/*获取fd状态失败*/
			continue;
		}
		/*设置fd属性添加FD_CLOEXEC*/
		fcntl(lLoop, F_SETFD, alFlag[lLoop] | FD_CLOEXEC);
	}

	/*创建信进程*/
	stPid = fork();
	if(-1 == stPid)
	{
		/*创建失败*/
		return -1;
	}
	else if(0 == stPid)
	{
		int loop;
		for(loop = 3; loop < 255; loop++)
		{
			close(loop);
		}
		/*子进程执行，并获取返回值*/
		execl("/bin/sh", "sh", "-c", pucCmd, NULL);
		exit(127);
	}
	else if (0 < stPid)
	{
		/*原进程等待子进程结束*/
		while(0 > waitpid(stPid, &lRet, 0))
		{
			if(errno != EINTR)
			{
				lRet = -1;
			}
		}
	}

	/*原进程将fd属性还原*/
	for(lLoop = lBegin; lLoop < 256; lLoop++)
	{
		if(-1 == fcntl(lLoop, F_GETFD, 0))
		{
			continue;
		}
		fcntl(lLoop, F_SETFD, alFlag[lLoop]);
	}

	return lRet;
}


void test_sig_n()
{
	return;
}
void test_sig()
{
#if 0
	pid_t now_pid;
	now_pid = getpid();
	kill(now_pid, SIGSEGV);
#endif

	char* test_p = 0;
	printf("addr = 0x%p\n", &test_p);
	*test_p = 0;
	return;
}
void test_sig_o()
{
	test_sig();
	return;
}
void test_sig_b()
{
	test_sig_o();
	return;
}

INT32 cps_tar(INT8* pcSrcFile, INT8* pcTargetFile)
{
    INT32 lRet;
    INT8 acCmd[256];
    memset(acCmd, 0, sizeof(acCmd));

    sprintf(acCmd, "zip -r %s %s", pcTargetFile, pcSrcFile);

    lRet = cps_system(acCmd);

    return lRet;
}

/*设置cps_system打印开关0为关闭，其他值为打开*/
void cps_set_system_print(INT32 lType)
{
	g_CpsSystemDbg = lType;
}

/******************************* 源文件结束 **********************************/



