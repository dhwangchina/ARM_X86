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
#include "cpss_common.h"
#include "cpss_tm_timer.h"
#include "cpss_dbg.h"
#include "cpss_vos_sem.h"

#ifdef CPSS_VOS_LINUX
#include "semaphore.h"
#endif
/******************************* 局部宏定义 **********************************/

/******************************* 全局变量定义/初始化 *************************/

/******************************* 局部常数和类型定义 **************************/
 BOOL g_bCpssVosSemLibInited = FALSE;
/* semaphore descriptor table */
 CPSS_VOS_SEM_DESC_T g_astCpssVosSemDescTbl [VOS_MAX_SEM_NUM];

/******************************* 局部函数原型声明 ****************************/
/* forward declarations */
 UINT32   cpss_vos_sem_desc_alloc (VOID);
 INT32    cpss_vos_sem_desc_free (UINT32 ulSemDesc);

/******************************* 函数实现 ************************************/

/*******************************************************************************
* 函数名称: cpss_vos_sem_lib_init
* 功    能: 初始化
*        
* 函数类型: 
* 参    数: 无
* 参数名称            类型            输入/输出         描述
* 
* 函数返回: 
*          成功：CPSS_OK
*          失败：CPSS_ERROR                    
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
#ifndef CPSS_DSP_CPU
INT32 cpss_vos_sem_lib_init (VOID)
{
    UINT32  ulSemDesc;
    
    if (g_bCpssVosSemLibInited == TRUE)
    {
        return (CPSS_ERROR);
    }
    
    /* 信号描述符表清 0 */
    
    memset ((VOID *) (& g_astCpssVosSemDescTbl[0]), 0, 
        (VOS_MAX_SEM_NUM * sizeof (CPSS_VOS_SEM_DESC_T)));
    
    /* 设置使用 */
    
    for (ulSemDesc = 0; ulSemDesc < VOS_MAX_SEM_NUM; ulSemDesc++)
    {
        g_astCpssVosSemDescTbl[ulSemDesc].bInuse = FALSE;
    }
    
    /* 初始化完成. */
    
    g_bCpssVosSemLibInited = TRUE;
    
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_vos_sem_desc_alloc
* 功    能: 分配信号量
*        
* 函数类型: 
* 参    数: 无
* 参数名称            类型            输入/输出         描述
* 
* 函数返回: 
*          成功：信号量索引
*          失败：VOS_SEM_DESC_INVALID                    
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
 UINT32 cpss_vos_sem_desc_alloc (VOID)
{
    UINT32  ulSemDesc;
    
    /* 查找空闲表项,第0项保留. */
    
    for (ulSemDesc = 1; ulSemDesc < VOS_MAX_SEM_NUM; ulSemDesc++)
    {
        if (g_astCpssVosSemDescTbl[ulSemDesc].bInuse == FALSE)
        {
            break;
        }
    }
    
    /* 查找失败. */
    
    if (ulSemDesc == VOS_MAX_SEM_NUM)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "\ncpss_vos_sem_desc_alloc: insufficient descriptor available."
            );
        
        return (VOS_SEM_DESC_INVALID);
    }
    
    /* Set in use. */
    
    g_astCpssVosSemDescTbl[ulSemDesc].bInuse = TRUE;
    
    return (ulSemDesc);
}

/*******************************************************************************
* 函数名称: cpss_vos_sem_desc_free
* 功    能: 释放信号量
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* ulSemDesc           UINT32           IN              信号量描述符
* 函数返回: 
*          成功：CPSS_OK
*          失败：CPSS_ERROR                  
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
 INT32 cpss_vos_sem_desc_free 
(
 UINT32  ulSemDesc /* semaphore descriptor */
 )
{
    /* 入参检查. */
    
    if ((ulSemDesc <= 0) || (ulSemDesc >= VOS_MAX_SEM_NUM))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "\ncpss_vos_sem_desc_free: descriptor invalid, descriptor = %d."
            , ulSemDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 该描述符表项清0 */
    
    memset ((VOID *) (& g_astCpssVosSemDescTbl[ulSemDesc]), 0,  sizeof (CPSS_VOS_SEM_DESC_T));
    
    /* Set in use. */
    
    g_astCpssVosSemDescTbl[ulSemDesc].bInuse = FALSE;
    
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_vos_sem_b_create
* 功    能: 创建二进制信号量
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* lOptions           INT32             IN              信号量选项
* enInitState        CPSS_VOS_SEM_STATE_E   IN              初始状态
* 函数返回: 
*          成功：信号量描述符
*          失败：VOS_SEM_DESC_INVALID                
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
UINT32 cpss_vos_sem_b_create 
(
 INT32   lOptions, /* options */
 CPSS_VOS_SEM_STATE_E enInitState  /* initial semaphore state */
 )
{
    UINT32  ulSemDesc;
    VOS_SEM_ID_T *pstSemId;

    ulSemDesc = cpss_vos_sem_desc_alloc ();
    if (ulSemDesc == VOS_SEM_DESC_INVALID)
    {
        return (VOS_SEM_DESC_INVALID);
    }

    pstSemId = malloc(sizeof(VOS_SEM_ID_T));
    if(NULL == pstSemId)
    {
    	cpss_vos_sem_desc_free(ulSemDesc);
        return (VOS_SEM_DESC_INVALID);
    }
    pstSemId->ucType = VOS_SEM_TYPE_BINARY;
    pstSemId->ucOptions = lOptions;
#ifdef CPSS_VOS_VXWORKS
    pstSemId->pvSem = malloc(sizeof(SEM_ID));
#elif defined CPSS_VOS_WINDOWS
    pstSemId->pvSem = malloc(sizeof(HANDLE));
#elif defined CPSS_VOS_LINUX
    pstSemId->pvSem = malloc(sizeof(sem_t));
#elif defined CPSS_DSP_CPU
    pstSemId->pvSem = NULL;
#else
#error not support
#endif

#ifndef CPSS_DSP_CPU
    if(NULL == pstSemId->pvSem)
    {
    	cpss_vos_sem_desc_free(ulSemDesc);
    	free(pstSemId);
        return (VOS_SEM_DESC_INVALID);
    }
#ifdef CPSS_VOS_VXWORKS
    *(SEM_ID*)pstSemId->pvSem = semBCreate(lOptions, enInitState);
    if(NULL == (SEM_ID)(*(SEM_ID*)pstSemId->pvSem))
#elif defined CPSS_VOS_WINDOWS
    *(HANDLE*)pstSemId->pvSem = CreateSemaphore(NULL, enInitState, 1, NULL);
    if(NULL == (HANDLE)(*(HANDLE*)pstSemId->pvSem))
#elif defined CPSS_VOS_LINUX
    if(sem_init(pstSemId->pvSem, 0, enInitState) != 0)
#endif
    {
    	cpss_vos_sem_desc_free(ulSemDesc);
    	free(pstSemId);
        return (VOS_SEM_DESC_INVALID);
    }
#endif /*endof #ifndef CPSS_DSP_CPU */
    /* Save semaphore ID. */
    g_astCpssVosSemDescTbl[ulSemDesc].semId = pstSemId;
    return (ulSemDesc);
}

/*******************************************************************************
* 函数名称: cpss_vos_sem_m_create
* 功    能: 创建互斥信号量
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* lOptions           INT32             IN              信号量选项
* 
* 函数返回: 
*          成功：信号量描述符
*          失败：VOS_SEM_DESC_INVALID                
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
UINT32 cpss_vos_sem_m_create 
(
 INT32   lOptions /* options */
 )
{
    UINT32  ulSemDesc;
    VOS_SEM_ID_T *pstSemId;

    ulSemDesc = cpss_vos_sem_desc_alloc ();
    if (ulSemDesc == VOS_SEM_DESC_INVALID)
    {
        return (VOS_SEM_DESC_INVALID);
    }

    pstSemId = malloc(sizeof(VOS_SEM_ID_T));
    if(NULL == pstSemId)
    {
    	cpss_vos_sem_desc_free(ulSemDesc);
        return (VOS_SEM_DESC_INVALID);
    }
    pstSemId->ucType = VOS_SEM_TYPE_MUTEX;
    pstSemId->ucOptions = lOptions;
#ifdef CPSS_VOS_VXWORKS
    pstSemId->pvSem = malloc(sizeof(SEM_ID));
#elif defined CPSS_VOS_WINDOWS
    pstSemId->pvSem = malloc(sizeof(HANDLE));
#elif defined CPSS_VOS_LINUX
    pstSemId->pvSem = malloc(sizeof(pthread_mutex_t));
#elif defined CPSS_DSP_CPU
    pstSemId->pvSem = NULL;
#else
#error not support
#endif

#ifndef CPSS_DSP_CPU
    if(NULL == pstSemId->pvSem)
    {
    	cpss_vos_sem_desc_free(ulSemDesc);
    	free(pstSemId);
        return (VOS_SEM_DESC_INVALID);
    }
#ifdef CPSS_VOS_VXWORKS
    *(SEM_ID*)pstSemId->pvSem = semMCreate(lOptions);
    if(NULL == (SEM_ID)(*(SEM_ID*)pstSemId->pvSem))
#elif defined CPSS_VOS_WINDOWS
    *(HANDLE*)pstSemId->pvSem = CreateMutex (NULL, FALSE, NULL);
    if(NULL == (HANDLE)(*(HANDLE*)pstSemId->pvSem))
#elif defined CPSS_VOS_LINUX
    	if (pthread_mutex_init (pstSemId->pvSem,NULL) != 0)
#endif
    {
    	cpss_vos_sem_desc_free(ulSemDesc);
    	free(pstSemId);
        return (VOS_SEM_DESC_INVALID);
    }
#endif /*endof #ifndef CPSS_DSP_CPU */
    /* Save semaphore ID. */
    g_astCpssVosSemDescTbl[ulSemDesc].semId = pstSemId;
    return (ulSemDesc);
}

/*******************************************************************************
* 函数名称: cpss_vos_sem_c_create
* 功    能: 创建计数信号量
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* lOptions           INT32             IN              信号量选项
* lInitCount         INT32             IN              初始计数
* lMaxCount          INT32             IN              最大计数        
* 函数返回: 
*          成功：信号量描述符
*          失败：VOS_SEM_DESC_INVALID                
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
UINT32 cpss_vos_sem_c_create 
(
 INT32   lOptions, /* options */
 INT32   lInitCount,  /* initial semaphore count */
 INT32   lMaxCount  /* maximum semaphore count */
 )
{
    UINT32  ulSemDesc;
    VOS_SEM_ID_T *pstSemId;

    ulSemDesc = cpss_vos_sem_desc_alloc ();
    if (ulSemDesc == VOS_SEM_DESC_INVALID)
    {
        return (VOS_SEM_DESC_INVALID);
    }

    pstSemId = malloc(sizeof(VOS_SEM_ID_T));
    if(NULL == pstSemId)
    {
    	cpss_vos_sem_desc_free(ulSemDesc);
        return (VOS_SEM_DESC_INVALID);
    }
    pstSemId->ucType = VOS_SEM_TYPE_COUNTING;
    pstSemId->ucOptions = lOptions;
#ifdef CPSS_VOS_VXWORKS
    pstSemId->pvSem = malloc(sizeof(SEM_ID));
#elif defined CPSS_VOS_WINDOWS
    pstSemId->pvSem = malloc(sizeof(HANDLE));
#elif defined CPSS_VOS_LINUX
    pstSemId->pvSem = malloc(sizeof(sem_t));
#elif defined CPSS_DSP_CPU
    pstSemId->pvSem = NULL;
#else
#error not support
#endif

#ifndef CPSS_DSP_CPU
    if(NULL == pstSemId->pvSem)
    {
    	cpss_vos_sem_desc_free(ulSemDesc);
    	free(pstSemId);
        return (VOS_SEM_DESC_INVALID);
    }
#ifdef CPSS_VOS_VXWORKS
    *(SEM_ID*)pstSemId->pvSem = semCCreate (lOptions, lInitCount);
    if(NULL == (SEM_ID)(*(SEM_ID*)pstSemId->pvSem))
#elif defined CPSS_VOS_WINDOWS
    *(HANDLE*)pstSemId->pvSem = CreateSemaphore (NULL, lInitCount, lMaxCount, NULL);
    if(NULL == (HANDLE)(*(HANDLE*)pstSemId->pvSem))
#elif defined CPSS_VOS_LINUX
	if (sem_init (pstSemId->pvSem, 0, lInitCount) != 0)
#endif
    {
    	cpss_vos_sem_desc_free(ulSemDesc);
    	free(pstSemId);
        return (VOS_SEM_DESC_INVALID);
    }
#endif /*endof #ifndef CPSS_DSP_CPU */
    /* Save semaphore ID. */
    g_astCpssVosSemDescTbl[ulSemDesc].semId = pstSemId;
    return (ulSemDesc);
}


/*******************************************************************************
* 函数名称: cpss_vos_sem_delete
* 功    能: 删除信号量
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* ulSemDesc           UINT32             IN              信号量描述符
*       
* 函数返回: 
*          成功：信号量描述符
*          失败：VOS_SEM_DESC_INVALID                
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
INT32 cpss_vos_sem_delete
(
 UINT32  ulSemDesc /* semaphore descriptor */
 )
{
    VOS_SEM_ID  semId;
    
    /* Check for semaphore descriptor. */
    
    if ((ulSemDesc <= 0) || (ulSemDesc >= VOS_MAX_SEM_NUM) || 
        (g_astCpssVosSemDescTbl[ulSemDesc].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "\ncpss_vos_sem_delete: descriptor invalid, descriptor = %d."
            , ulSemDesc);
        
        return (CPSS_ERROR);
    }
    
    /* Get semaphore ID. */
    
    semId = g_astCpssVosSemDescTbl[ulSemDesc].semId;
    
#ifdef CPSS_VOS_VXWORKS
    semDelete (*(SEM_ID*)semId->pvSem);
    free (semId->pvSem);
#elif defined CPSS_VOS_WINDOWS
    CloseHandle(*(HANDLE*)semId->pvSem);
    free (semId->pvSem);
#elif defined CPSS_VOS_LINUX
    if(semId->ucType == VOS_SEM_TYPE_MUTEX)
    {
    	pthread_mutex_destroy (semId->pvSem);
    }
    else
    {
    	sem_destroy (semId->pvSem);
    }
    free (semId->pvSem);
#endif /* CPSS_VOS_WINDOWS */
    free (semId);
    /* Free semaphore descriptor. */
    cpss_vos_sem_desc_free (ulSemDesc);
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_vos_sem_p
* 功    能: 信号量 p 操作
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* ulSemDesc           UINT32             IN              信号量描述符
* lTimeout           INT32              IN              延迟时间
* 函数返回: 
*          成功：CPSS_OK
*          失败：CPSS_ERROR
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
INT32 cpss_vos_sem_p 
(
 UINT32  ulSemDesc, /* semaphore descriptor */
 INT32   lTimeout /* millisecond */
 )
{
    VOS_SEM_ID  semId;
    time_t stTime;
    struct timespec stAtime;
    struct timeval stNow;

    if ((ulSemDesc == 0) || (ulSemDesc >= VOS_MAX_SEM_NUM) ||
        !g_astCpssVosSemDescTbl[ulSemDesc].bInuse)
    {
        return (CPSS_ERROR);
    }
    semId = g_astCpssVosSemDescTbl[ulSemDesc].semId;
#ifdef CPSS_VOS_VXWORKS
    if ((lTimeout != NO_WAIT) && (lTimeout != WAIT_FOREVER))
    {
        lTimeout = lTimeout * CLOCK_RATE / 1000;
        if (lTimeout == 0)
        {
            lTimeout = 1;
        }
    }
    return semTake(*(SEM_ID*)semId->pvSem, lTimeout);
#elif defined CPSS_VOS_WINDOWS
    if (WaitForSingleObject (*(HANDLE*)semId->pvSem, lTimeout) != WAIT_OBJECT_0)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "\ncpss_vos_sem_p: WaitForSingleObject failed."
            );

        return (CPSS_ERROR);
    }
#elif defined CPSS_VOS_LINUX
    /*互斥锁操作*/
    if(semId->ucType == VOS_SEM_TYPE_MUTEX)
    {
    	if (pthread_mutex_lock (semId->pvSem) != 0)
        {
            cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "\ncpss_vos_sem_p: pthread_mutex_lock failed."
                );
            return (CPSS_ERROR);
        }
    }
    /*信号量操作*/
    else
    {
    	/*阻塞等待*/
    	if(WAIT_FOREVER == lTimeout)
    	{
			while (sem_wait (semId->pvSem) != 0)
			{
				/*被系统中断*/
				if(EINTR == errno)
				{
					/*继续等待*/
					continue;
				}
				cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "\ncpss_vos_sem_p: sem_wait failed."
					);
				return (CPSS_ERROR);
			}
    	}
    	/*非阻塞等待*/
    	else if(NO_WAIT == lTimeout)
    	{
			if (sem_trywait (semId->pvSem) != 0)
			{
				cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "\ncpss_vos_sem_p: sem_wait failed."
					);
				return (CPSS_ERROR);
			}
    	}
    	/*超时等待*/
    	else
    	{
			gettimeofday(&stNow, NULL);

			stAtime.tv_sec = stNow.tv_sec + lTimeout / 1000;
			stAtime.tv_nsec = (stNow.tv_usec + (lTimeout % 1000) * 1000) * 1000;
			if(stAtime.tv_nsec > 1000000000)
			{
				stAtime.tv_sec += 1;
				stAtime.tv_nsec -= 1000000000ll;
			}

			while(0 != sem_timedwait(semId->pvSem, &stAtime))
			{
				/*被系统中断*/
				if(EINTR == errno)
				{
					/*继续等待*/
					continue;
				}
				cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "\ncpss_vos_sem_p: sem_timedwait failed."
					);
				return (CPSS_ERROR);
			}
    	}
    }
#endif
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_vos_sem_v
* 功    能: 信号量 v 操作
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* ulSemDesc           UINT32             IN              信号量描述符
*
* 函数返回:
*          成功：CPSS_OK
*          失败：CPSS_ERROR
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
INT32 cpss_vos_sem_v
(
 UINT32  ulSemDesc /* semaphore descriptor */
 )
{
    VOS_SEM_ID  semId;

    if ((ulSemDesc == 0) || (ulSemDesc >= VOS_MAX_SEM_NUM) ||
        !g_astCpssVosSemDescTbl[ulSemDesc].bInuse)
    {
        return (CPSS_ERROR);
    }
    semId = g_astCpssVosSemDescTbl[ulSemDesc].semId;
#ifdef CPSS_VOS_VXWORKS
    if ((lTimeout != NO_WAIT) && (lTimeout != WAIT_FOREVER))
    {
        lTimeout = lTimeout * CLOCK_RATE / 1000;
        if (lTimeout == 0)
        {
            lTimeout = 1;
        }
    }
    return semGive(*(SEM_ID*)semId->pvSem, lTimeout);
#elif defined CPSS_VOS_WINDOWS
    if (ReleaseSemaphore (*(HANDLE*)semId->pvSem, lTimeout) != WAIT_OBJECT_0)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "\ncpss_vos_sem_p: ReleaseSemaphore failed."
            );

        return (CPSS_ERROR);
    }
#elif defined CPSS_VOS_LINUX
    if(semId->ucType == VOS_SEM_TYPE_MUTEX)
    {
    	if (pthread_mutex_unlock (semId->pvSem) != 0)
        {
            cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "\ncpss_vos_sem_p: pthread_mutex_unlock failed."
                );
            return (CPSS_ERROR);
        }
    }
    else
    {
    	if (sem_post (semId->pvSem) != 0)
        {
            cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "\ncpss_vos_sem_p: sem_post failed."
                );

            return (CPSS_ERROR);
        }
    }
#endif
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_vos_sem_info_get
* 功    能: 获得信号量信息
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* ulSemDesc           UINT32             IN              信号量描述符
* ptSemInfo           CPSS_VOS_SEM_INFO_T *   IN              信号量信息
* 函数返回: 
*          成功：CPSS_OK
*          失败：CPSS_ERROR
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
INT32 cpss_vos_sem_info_get 
(
 UINT32  ulSemDesc, /* semaphore descriptor */
 CPSS_VOS_SEM_INFO_T * pstSemInfo /* pointer to semaphore information */
 )
{
    VOS_SEM_ID    semId;
    
    /* Check for semaphore descriptor. */
    
    if ((ulSemDesc <= 0) || (ulSemDesc >= VOS_MAX_SEM_NUM) || 
        (g_astCpssVosSemDescTbl[ulSemDesc].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "\ncpss_vos_sem_info_get: descriptor invalid, descriptor = %d."
            , ulSemDesc);
        
        return (CPSS_ERROR);
    }
    
    /* Get semaphore ID. */
    
    semId = g_astCpssVosSemDescTbl[ulSemDesc].semId;

    pstSemInfo->ulSemDesc = ulSemDesc;
    pstSemInfo->semId = g_astCpssVosSemDescTbl[ulSemDesc].semId;
    pstSemInfo->ucType = semId->ucType;
    pstSemInfo->ucOptions = semId->ucOptions;

    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_vos_sem_show
* 功    能: 显示信号量信息
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* ulSemDesc           UINT32             IN              信号量描述符
*
* 函数返回: 
*          成功：CPSS_OK
*          失败：CPSS_ERROR
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
INT32 cpss_vos_sem_show 
(
UINT32  ulSemDesc, /* semaphore descriptor */
INT32   lLevel /* 0 = 概况, 2 = 所有信号量 */
)
{
    CPSS_VOS_SEM_INFO_T  stSemInfo;

    switch (lLevel)
        {
        case 0 :
            {
     
            /* Get semaphore information. */
    
            if (cpss_vos_sem_info_get (ulSemDesc, & stSemInfo) == CPSS_ERROR)
                {
                return (CPSS_ERROR);
                }
    
            cps__oams_shcmd_printf ("\n");
            cps__oams_shcmd_printf ("Semaphore dscriptor: %d\n", stSemInfo.ulSemDesc);
            cps__oams_shcmd_printf ("Semaphore ID: 0x%-8x\n", (INT32) stSemInfo.semId);
            cps__oams_shcmd_printf ("Semaphore Type: 0x%x\t", stSemInfo.ucType);
            if (stSemInfo.ucType == VOS_SEM_TYPE_BINARY)
                {
                cps__oams_shcmd_printf ("%s\n", "BINARY");
                }
            if (stSemInfo.ucType == VOS_SEM_TYPE_MUTEX)
                {
                cps__oams_shcmd_printf ("%s\n", "MUTEX");
                }
            if (stSemInfo.ucType == VOS_SEM_TYPE_COUNTING)
                {
                cps__oams_shcmd_printf ("%s\n", "COUNTING");
                }

            cps__oams_shcmd_printf ("Options: 0x%x\t%s\n", stSemInfo.ucOptions, 
                    ((stSemInfo.ucOptions == VOS_SEM_Q_FIFO) ? "FIFO" : "PRIORITY"));

            break;
            }

        case 2 :
        default :
            {
            for (ulSemDesc = 1; ulSemDesc < VOS_MAX_SEM_NUM; ulSemDesc++)
                {
                if (g_astCpssVosSemDescTbl[ulSemDesc].bInuse == TRUE)
                    {
                    if (cpss_vos_sem_info_get (ulSemDesc, & stSemInfo) == CPSS_OK)
                        {
                        cps__oams_shcmd_printf ("\n");
                        cps__oams_shcmd_printf ("Semaphore dscriptor: %d\n", stSemInfo.ulSemDesc);
                        cps__oams_shcmd_printf ("Semaphore ID: 0x%-8x\n", (INT32) stSemInfo.semId);
                        cps__oams_shcmd_printf ("Semaphore Type: 0x%x\t", stSemInfo.ucType);
                        if (stSemInfo.ucType == VOS_SEM_TYPE_BINARY)
                            {
                            cps__oams_shcmd_printf ("%s\n", "BINARY");
                            }
                        if (stSemInfo.ucType == VOS_SEM_TYPE_MUTEX)
                            {
                            cps__oams_shcmd_printf ("%s\n", "MUTEX");
                            }
                        if (stSemInfo.ucType == VOS_SEM_TYPE_COUNTING)
                            {
                            cps__oams_shcmd_printf ("%s\n", "COUNTING");
                            }

                        cps__oams_shcmd_printf ("Options: 0x%x\t%s\n", stSemInfo.ucOptions, 
                                ((stSemInfo.ucOptions == VOS_SEM_Q_FIFO) ? "FIFO" : "PRIORITY"));
                        }
                    }
                }
            break;
            }
        }
    
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_vos_mutex_create
* 功    能: 创建互斥信号量
*        
* 函数类型: 
* 参    数: 无
* 参数名称            类型            输入/输出         描述
* 
*
* 函数返回: 
*          cpss_vos_sem_m_create
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
UINT32 cpss_vos_mutex_create (VOID)
{
    return (cpss_vos_sem_m_create (VOS_SEM_Q_PRI));
}

/*******************************************************************************
* 函数名称: cpss_vos_mutex_delete
* 功    能: 删除互斥信号量
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* ulMutexD            UINT32           IN               描述符
*
* 函数返回: 
*          cpss_vos_sem_delete
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/

INT32 cpss_vos_mutex_delete(UINT32 ulMutexD)
{
    return (cpss_vos_sem_delete (ulMutexD));
}

/*******************************************************************************
* 函数名称: cpss_vos_mutex_p
* 功    能:信号量p操作
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* ulMutexD            UINT32           IN               描述符
* ulWaitTime          UINT32           IN               等待时间
* 函数返回: 
*          cpss_vos_sem_p
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
INT32 cpss_vos_mutex_p (UINT32 ulMutexD,UINT32 ulWaitTime)
{
    return (cpss_vos_sem_p (ulMutexD, ulWaitTime));
}

/*******************************************************************************
* 函数名称: cpss_vos_mutex_v
* 功    能:信号量v操作
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* ulMutexD            UINT32           IN               描述符
*
* 函数返回: 
*          cpss_vos_sem_v
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
INT32 cpss_vos_mutex_v(UINT32 ulMutexD)
{
    return (cpss_vos_sem_v (ulMutexD));
}
#else
UINT32 cpss_vos_mutex_create (VOID)
{
    return CPSS_OK;
}
INT32 cpss_vos_mutex_p (UINT32 ulMutexD,UINT32 ulWaitTime)
{
    return CPSS_OK;
}
INT32 cpss_vos_mutex_v(UINT32 ulMutexD)
{
    return CPSS_OK;
}
UINT32 cpss_vos_sem_b_create 
(
 INT32   lOptions, /* options */
 CPSS_VOS_SEM_STATE_E enInitState  /* initial semaphore state */
)
{
    return CPSS_OK;
}
INT32 cpss_vos_sem_lib_init (VOID)
{
    return CPSS_OK;
}
INT32 cpss_vos_sem_show 
(
UINT32  ulSemDesc, /* semaphore descriptor */
INT32   lLevel /* 0 = 概况, 2 = 所有信号量 */
)
{
    return CPSS_OK;
}
UINT32 cpss_vos_sem_m_create 
(
 INT32   lOptions /* options */
)
{
    return CPSS_OK;
}
INT32 cpss_vos_sem_p 
(
 UINT32  ulSemDesc, /* semaphore descriptor */
 INT32   lTimeout /* millisecond */
 )
 {
     return CPSS_OK;
 }
INT32 cpss_vos_sem_v 
(
 UINT32  ulSemDesc /* semaphore descriptor */
)
{
     return CPSS_OK;
}
UINT32 cpss_vos_sem_c_create 
(
 INT32   lOptions, /* options */
 INT32   lInitCount,  /* initial semaphore count */
 INT32   lMaxCount  /* maximum semaphore count */
)
{
     return CPSS_OK;
}
#endif
/******************************* 源文件结束 ***********************************/

