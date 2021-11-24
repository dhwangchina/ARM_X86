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
#include "cpss_tm_timer.h"
#include "cpss_dbg.h"
#include "cpss_util_q_fifo_lib.h"
#include "cpss_vos_msg_q.h"
#include "cpss_vos_sem.h"

#define VOS_MSG_Q_FIFO  0x00 /* 任务按FIFO算法调度  */
#define VOS_MSG_Q_PRI   0x01 /* 任务按优先级调度 */
#define VOS_MSG_PRI_NORMAL  0 /* 优先级为普通的消息*/
#define VOS_MSG_PRI_URGENT  1  /* 优先级为紧急的消息 */

/******************************* 局部宏定义 **********************************/

/******************************* 全局变量定义/初始化 *************************/

/******************************* 局部常数和类型定义 **************************/
 BOOL g_bCpssVosMsgQLibInited = FALSE; /* 初始值 */
/* 消息队列描述表 */
 CPSS_VOS_MSG_Q_DESC_T   g_astCpssVosMsgQDescTbl [VOS_MAX_MSG_Q_NUM];

/******************************* 局部函数原型声明 ****************************/
 UINT32   cpss_vos_msg_q_desc_alloc (void);
 INT32    cpss_vos_msg_q_desc_free (UINT32 ulMsgQDesc);
#ifdef CPSS_VOS_WINDOWS
 INT32    cpss_vos_msg_q_init (CPSS_VOS_MSG_Q_T *ptMsgQ, INT32 lMaxMsgs, 
                                     INT32 lMaxMsgLen, INT32 lOptions);
#endif

#ifdef CPSS_VOS_LINUX
 INT32    cpss_vos_msg_q_init (CPSS_VOS_MSG_Q_T *ptMsgQ, INT32 lMaxMsgs, 
                                     INT32 lMaxMsgLen, INT32 lOptions);
#endif


/*******************************************************************************
* 函数名称:cpss_vos_msg_q_lib_init 
* 功    能: 消息队列初始化
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* 
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:                                  
*******************************************************************************/
INT32 cpss_vos_msg_q_lib_init (void)
{
    UINT32  ulMsgQDesc;
    
    /* 初始化开始*/
    
    if (g_bCpssVosMsgQLibInited == TRUE)
    {
        return (CPSS_ERROR);
    }
    
    /* 清空消息描述符表*/
    
    memset ((void *) (& g_astCpssVosMsgQDescTbl[0]), 0, 
        (VOS_MAX_MSG_Q_NUM * sizeof (CPSS_VOS_MSG_Q_DESC_T)));
    
    /* 设置所有消息的使用标志为空闲*/
    
    for (ulMsgQDesc = 0; ulMsgQDesc < VOS_MAX_MSG_Q_NUM; ulMsgQDesc++)
    {
        g_astCpssVosMsgQDescTbl[ulMsgQDesc].bInuse = FALSE;
        g_astCpssVosMsgQDescTbl[ulMsgQDesc].bTimer = FALSE;
    }
    
    /*初始化结束*/
    
    g_bCpssVosMsgQLibInited = TRUE;
    
    return (CPSS_OK);
}


/*******************************************************************************
* 函数名称:cpss_vos_msg_q_desc_alloc 
* 功    能: 分配消息描述符
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* 
* 函数返回: 
*       成功：分配的消息描述符;
*       失败：CPSS_ERROR；
* 说    明:                                  
*******************************************************************************/
 UINT32 cpss_vos_msg_q_desc_alloc (void)
{
    UINT32  ulMsgQDesc;
    
    /* 找到一个空闲的消息描述符实体，第0个保留不用，从1开始查找*/
    
    for (ulMsgQDesc = 1; ulMsgQDesc < VOS_MAX_MSG_Q_NUM; ulMsgQDesc++)
    {
        if (g_astCpssVosMsgQDescTbl[ulMsgQDesc].bInuse == FALSE)
        {
            break;
        }
    }
    
    /* 没有空闲 消息描述符实体*/
    
    if (ulMsgQDesc == VOS_MAX_MSG_Q_NUM)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_desc_alloc: insufficient decriptor available.\n"
            );
        
        return (VOS_MSG_Q_DESC_INVALID);
    }
    
    /* 修改使用标志为“TRUE：使用”*/
    
    g_astCpssVosMsgQDescTbl[ulMsgQDesc].bInuse = TRUE;
    g_astCpssVosMsgQDescTbl[ulMsgQDesc].bTimer = FALSE;
    
    return (ulMsgQDesc);
}



/*******************************************************************************
* 函数名称:cpss_vos_msg_q_desc_free  
* 功    能: 释放消息描述符
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* 
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:                                  
*******************************************************************************/
 INT32 cpss_vos_msg_q_desc_free 
(
 UINT32  ulMsgQDesc
 )
{
    /* 检查消息描述符是否合法*/
    
    if ((ulMsgQDesc <= 0) || (ulMsgQDesc >= VOS_MAX_MSG_Q_NUM))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_desc_free: descriptor invalid, descriptor = %d.\n"
            , ulMsgQDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 清空消息描述符实体 */
    
    memset ((void *) (& g_astCpssVosMsgQDescTbl[ulMsgQDesc]), 0,  
        sizeof (CPSS_VOS_MSG_Q_DESC_T));
    
    /* 修改使用标志为空闲 */
    
    g_astCpssVosMsgQDescTbl[ulMsgQDesc].bInuse = FALSE;
    g_astCpssVosMsgQDescTbl[ulMsgQDesc].bTimer = FALSE;
    
    return (CPSS_OK);
}

#ifdef CPSS_VOS_WINDOWS


/*******************************************************************************
* 函数名称:cpss_vos_msg_q_init   
* 功    能:初始化一个消息 队列
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* 
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:                                  
*******************************************************************************/
 INT32 cpss_vos_msg_q_init 
(
 CPSS_VOS_MSG_Q_T * ptMsgQ, /* 消息队列指针 */
 INT32   lMaxMsgs, /* 最大消息数 */
 INT32  lMaxMsgLen, /* 最大消息长度*/
 INT32   lOptions /* 选项 */
 )
{
    void *  pPool;
    INT32   lNodeSize;
    INT32   lCount;
    UINT32  ulSemDescRecv;
    UINT32  ulSemDescSend;
    UINT32  ulSemDescAccess;
    INT32  lRet;
    
    /*初始化是否成功*/
    
    /*if ((!g_bCpssVosMsgQLibInited) && (cpss_vos_msg_q_lib_init () == CPSS_ERROR))*/  
    
    lRet = cpss_vos_msg_q_lib_init ();
    if ((g_bCpssVosMsgQLibInited == FALSE ) && (lRet == CPSS_ERROR))
    {
        return (CPSS_ERROR);
    }
    
    /* 清空消息队列结构 */
    
    memset ((void *) ptMsgQ, 0, sizeof (CPSS_VOS_MSG_Q_T));
    
    /* Initialize internal queues. */
    
    switch (lOptions)
    {
    case VOS_MSG_Q_FIFO : 
        {
            break;
        }
        
    case VOS_MSG_Q_PRI :
        {
            break;
        }
        
    default :
        {
            cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_init: options invalid, options = 0x%x.\n"
                , lOptions);
            
            return (CPSS_ERROR);
            
            break;
        }
    }
    
    /* 初始化空闲和FIFO队列*/
    
    cpss_util_q_fifo_init (& ptMsgQ->qFree);
    cpss_util_q_fifo_init (& ptMsgQ->qMsg);
    
    /* 向FIFO消息队列中加入消息结点 */
    
    pPool = (void *) (((INT8 *) ptMsgQ) + sizeof (CPSS_VOS_MSG_Q_T));
    lNodeSize = VOS_MSG_NODE_SIZE (lMaxMsgLen);
    
    for (lCount = 0; lCount < lMaxMsgs; lCount++)
    {
        cpss_util_q_fifo_put (& ptMsgQ->qFree, (CPSS_UTIL_Q_FIFO_NODE_T *) pPool
            , UTIL_Q_FIFO_KEY_TAIL);      /*cpss_type_cast*/
        pPool = (void *) (((INT8 *) pPool) + lNodeSize);      /*cpss_type_cast*/
    }
    
    /* 创建接收信号*/
    
    ulSemDescRecv = cpss_vos_sem_c_create (VOS_SEM_Q_FIFO, 0, lMaxMsgs);
    if (ulSemDescRecv == VOS_SEM_DESC_INVALID)
    {
        return (CPSS_ERROR);
    }
    
    /*创建发送信号*/
    
    ulSemDescSend = cpss_vos_sem_c_create (VOS_SEM_Q_FIFO, lMaxMsgs, lMaxMsgs);
    if (ulSemDescSend == VOS_SEM_DESC_INVALID)
    {
        cpss_vos_sem_delete (ulSemDescRecv);
        
        return (CPSS_ERROR);
    }
    
    /* 创建访问信号*/
    
    ulSemDescAccess = cpss_vos_sem_m_create (VOS_SEM_Q_FIFO);
    if (ulSemDescAccess == VOS_SEM_DESC_INVALID)
    {
        cpss_vos_sem_delete (ulSemDescRecv);
        cpss_vos_sem_delete (ulSemDescSend);
        
        return (CPSS_ERROR);
    }
    
    ptMsgQ->ulSemDescRecv = ulSemDescRecv;
    ptMsgQ->ulSemDescSend = ulSemDescSend;
    ptMsgQ->ulSemDescAccess = ulSemDescAccess;
    
       
    ptMsgQ->lOptions = lOptions;
    ptMsgQ->lMaxMsgs = lMaxMsgs;
    ptMsgQ->lMaxMsgLen = lMaxMsgLen;
    
    return (CPSS_OK);
    }
#endif/* CPSS_VOS_WINDOWS */
    
#ifdef CPSS_DSP_CPU
/*******************************************************************************
* 函数名称:cpss_vos_msg_q_init   
* 功    能:初始化一个消息 队列
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* 
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:                                  
*******************************************************************************/
static INT32 cpss_vos_msg_q_init 
    (
    CPSS_VOS_MSG_Q_T * ptMsgQ, /* 消息队列指针 */
    INT32   lMaxMsgs, /* 最大消息数 */
    INT32   lMaxMsgLen, /* 最大消息长度*/
    INT32   lOptions /* 选项 */
    )
    {
    void *  pPool;
    INT32   lNodeSize;
    INT32   lCount;

    /*初始化是否成功*/
    
    if ((!g_bCpssVosMsgQLibInited) && (cpss_vos_msg_q_lib_init () == CPSS_ERROR)
        ) 
        {
        return (CPSS_ERROR);
        }
    
    /* 清空消息队列结构 */
    
    memset ((void *) ptMsgQ, 0, sizeof (CPSS_VOS_MSG_Q_T));
    
    /* Initialize internal queues. */
    
    switch (lOptions)
        {
        case VOS_MSG_Q_FIFO : 
            {
            break;
            }
            
        case VOS_MSG_Q_PRI :
            {
            break;
            }
            
        default :
            {
            cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_init: options invalid, options = 0x%x.\n"
                         , lOptions);

            return (CPSS_ERROR);
            }
        }
    
    /* 初始化空闲和FIFO队列*/
    
    cpss_util_q_fifo_init (& ptMsgQ->qFree);
    cpss_util_q_fifo_init (& ptMsgQ->qMsg);

    /* 向FIFO消息队列中加入消息结点 */
    
    pPool = (void *) (((INT8 *) ptMsgQ) + sizeof (CPSS_VOS_MSG_Q_T));
    lNodeSize = VOS_MSG_NODE_SIZE (lMaxMsgLen);
    
    for (lCount = 0; lCount < lMaxMsgs; lCount++)
        {
        cpss_util_q_fifo_put (& ptMsgQ->qFree, (CPSS_UTIL_Q_FIFO_NODE_T *) pPool
                              , UTIL_Q_FIFO_KEY_TAIL); 
        pPool = (void *) (((INT8 *) pPool) + lNodeSize);
        }
       
    ptMsgQ->lOptions = lOptions;
    ptMsgQ->lMaxMsgs = lMaxMsgs;
    ptMsgQ->lMaxMsgLen = lMaxMsgLen;
    ptMsgQ->lMsgNum = 0;

    return (CPSS_OK);
    }
#endif

#ifdef CPSS_VOS_LINUX


/*******************************************************************************
* 函数名称:cpss_vos_msg_q_init   
* 功    能:初始化一个消息 队列
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* 
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:                                  
*******************************************************************************/
 INT32 cpss_vos_msg_q_init 
(
 CPSS_VOS_MSG_Q_T * ptMsgQ, /* 消息队列指针 */
 INT32   lMaxMsgs, /* 最大消息数 */
 INT32  lMaxMsgLen, /* 最大消息长度*/
 INT32   lOptions /* 选项 */
 )
{
    void *  pPool;
    INT32   lNodeSize;
    INT32   lCount;
    UINT32  ulSemDescRecv;
    UINT32  ulSemDescSend;
    UINT32  ulSemDescAccess;
    INT32  lRet;
    
    /*初始化是否成功*/
    
    /*if ((!g_bCpssVosMsgQLibInited) && (cpss_vos_msg_q_lib_init () == CPSS_ERROR))*/  
    
    lRet = cpss_vos_msg_q_lib_init ();
    if ((g_bCpssVosMsgQLibInited == FALSE ) && (lRet == CPSS_ERROR))
    {
        return (CPSS_ERROR);
    }
    
    /* 清空消息队列结构 */
    
    memset ((void *) ptMsgQ, 0, sizeof (CPSS_VOS_MSG_Q_T));
    
    /* Initialize internal queues. */
    
    switch (lOptions)
    {
    case VOS_MSG_Q_FIFO : 
        {
            break;
        }
        
    case VOS_MSG_Q_PRI :
        {
            break;
        }
        
    default :
        {
            cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_init: options invalid, options = 0x%x.\n"
                , lOptions);
            
            return (CPSS_ERROR);
            
            break;
        }
    }
    
    /* 初始化空闲和FIFO队列*/
    
    cpss_util_q_fifo_init (& ptMsgQ->qFree);
    cpss_util_q_fifo_init (& ptMsgQ->qMsg);
    
    /* 向FIFO消息队列中加入消息结点 */
    
    pPool = (void *) (((INT8 *) ptMsgQ) + sizeof (CPSS_VOS_MSG_Q_T));
    lNodeSize = VOS_MSG_NODE_SIZE (lMaxMsgLen);
    
    for (lCount = 0; lCount < lMaxMsgs; lCount++)
    {
        cpss_util_q_fifo_put (& ptMsgQ->qFree, (CPSS_UTIL_Q_FIFO_NODE_T *) pPool
            , UTIL_Q_FIFO_KEY_TAIL);      /*cpss_type_cast*/
        pPool = (void *) (((INT8 *) pPool) + lNodeSize);      /*cpss_type_cast*/
    }
    
    /* 创建接收信号*/
    
    ulSemDescRecv = cpss_vos_sem_c_create (VOS_SEM_Q_FIFO, 0, lMaxMsgs);
    if (ulSemDescRecv == VOS_SEM_DESC_INVALID)
    {
        return (CPSS_ERROR);
    }
    
    cpss_delay(1);

    /*创建发送信号*/
    
    ulSemDescSend = cpss_vos_sem_c_create (VOS_SEM_Q_FIFO, lMaxMsgs, lMaxMsgs);
    if (ulSemDescSend == VOS_SEM_DESC_INVALID)
    {
        cpss_vos_sem_delete (ulSemDescRecv);
        
        return (CPSS_ERROR);
    }
    
    /* 创建访问信号*/
    
    ulSemDescAccess = cpss_vos_sem_m_create (VOS_SEM_Q_FIFO);
    if (ulSemDescAccess == VOS_SEM_DESC_INVALID)
    {
        cpss_vos_sem_delete (ulSemDescRecv);
        cpss_vos_sem_delete (ulSemDescSend);
        
        return (CPSS_ERROR);
    }
    
    ptMsgQ->ulSemDescRecv = ulSemDescRecv;
    ptMsgQ->ulSemDescSend = ulSemDescSend;
    ptMsgQ->ulSemDescAccess = ulSemDescAccess;
    
       
    ptMsgQ->lOptions = lOptions;
    ptMsgQ->lMaxMsgs = lMaxMsgs;
    ptMsgQ->lMaxMsgLen = lMaxMsgLen;
    
    return (CPSS_OK);
    }
#endif/* CPSS_VOS_LINUX */

/*******************************************************************************
* 函数名称:cpss_vos_msg_q_create  
* 功    能:创建一个消息 队列
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* 
* 函数返回:  消息描述符
*      
* 说    明:                                  
*******************************************************************************/
UINT32 cpss_vos_msg_q_create
    (
    INT8 *  pcName, /* 名称*/
    INT32   lMaxMsgs, /* 最大消息数 */
    INT32	lMaxMsgLen, /* 最大消息长度*/
    INT32   lOptions /* options */
    )
{
    UINT32  ulMsgQDesc;
    VOS_MSG_Q_ID    tMsgQId;
#ifdef CPSS_VOS_WINDOWS
    INT32   lSize;
#endif /* CPSS_VOS_WINDOWS */
#ifdef CPSS_DSP_CPU
    INT32   lSize;
#endif

#ifdef CPSS_VOS_LINUX
    INT32   lSize;
#endif /* CPSS_VOS_LINUX */

    INT32 lRet;
    /* 初始化是否成功*/
    lRet = cpss_vos_msg_q_lib_init () ;
    if ((g_bCpssVosMsgQLibInited == FALSE) && (lRet== CPSS_ERROR))
    {
        return (VOS_MSG_Q_DESC_INVALID);
    }
    
    /* 分配消息描述符是否成功*/
    
    ulMsgQDesc = cpss_vos_msg_q_desc_alloc ();
    if (ulMsgQDesc == VOS_MSG_Q_DESC_INVALID)
    {
        return (VOS_MSG_Q_DESC_INVALID);
    }
    
#ifdef CPSS_VOS_VXWORKS
    /* 创建消息队列 */
    
    tMsgQId = msgQCreate (lMaxMsgs, lMaxMsgLen, lOptions);
    if (tMsgQId == NULL)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_create: msgQCreate failed.\n"
            );
        
        cpss_vos_msg_q_desc_free (ulMsgQDesc);
        
        return (VOS_MSG_Q_DESC_INVALID);
    }
#endif /* CPSS_VOS_VXWORKS */
    
#ifdef CPSS_VOS_WINDOWS
    /* 计算需要空间大小*/
    
    lSize = sizeof (CPSS_VOS_MSG_Q_T) + lMaxMsgs * VOS_MSG_NODE_SIZE (lMaxMsgLen);
    
    /*分配内存 */
    
    tMsgQId = (VOS_MSG_Q_ID) malloc (lSize);
    if (tMsgQId == NULL)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_create: malloc failed.\n"
            );
        
        cpss_vos_msg_q_desc_free (ulMsgQDesc);
        
        return (VOS_MSG_Q_DESC_INVALID);
    }
    
    /*初始化消息队列*/
    
    lRet = cpss_vos_msg_q_init (tMsgQId, lMaxMsgs, lMaxMsgLen, lOptions) ;
    if (lRet == CPSS_ERROR)
    {
        free ((void *) tMsgQId);     /*cpss_type_cast*/
        cpss_vos_msg_q_desc_free (ulMsgQDesc);
        
        return (VOS_MSG_Q_DESC_INVALID);
    }
#endif /* CPSS_VOS_WINDOWS */
    
#ifdef CPSS_DSP_CPU
    /* 计算需要空间大小*/
    
    lSize = sizeof (CPSS_VOS_MSG_Q_T) + lMaxMsgs * VOS_MSG_NODE_SIZE (lMaxMsgLen);
    
    /*分配内存 */
    
    tMsgQId = (VOS_MSG_Q_ID) malloc (lSize);
    if (tMsgQId == NULL)
        {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_create: malloc failed.\n"
                     );
        
        cpss_vos_msg_q_desc_free (ulMsgQDesc);
        
        return (VOS_MSG_Q_DESC_INVALID);
        }
    
    /*初始化消息队列*/
    
    lRet = cpss_vos_msg_q_init (tMsgQId, lMaxMsgs, lMaxMsgLen, lOptions) ;
    if (lRet == CPSS_ERROR)
        {
        free ((void *) tMsgQId); 
        cpss_vos_msg_q_desc_free (ulMsgQDesc);
        
        return (VOS_MSG_Q_DESC_INVALID);
        }
#endif

#ifdef CPSS_VOS_LINUX
    /* 计算需要空间大小*/
    
    lSize = sizeof (CPSS_VOS_MSG_Q_T) + lMaxMsgs * VOS_MSG_NODE_SIZE (lMaxMsgLen);
    
    /*分配内存 */
    
    tMsgQId = (VOS_MSG_Q_ID) malloc (lSize);
    if (tMsgQId == NULL)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_create: malloc failed.\n"
            );
        
        cpss_vos_msg_q_desc_free (ulMsgQDesc);
        
        return (VOS_MSG_Q_DESC_INVALID);
    }
    
    /*初始化消息队列*/
    
    lRet = cpss_vos_msg_q_init (tMsgQId, lMaxMsgs, lMaxMsgLen, lOptions) ;
    if (lRet == CPSS_ERROR)
    {
        free ((void *) tMsgQId);     /*cpss_type_cast*/
        cpss_vos_msg_q_desc_free (ulMsgQDesc);
        
        return (VOS_MSG_Q_DESC_INVALID);
    }
#endif /* CPSS_VOS_LINUX */

    /* 保存消息队列ID*/
    g_astCpssVosMsgQDescTbl[ulMsgQDesc].msgQId = tMsgQId;
    
    /* 
    * Initialize rest. Now not fix problem that message queue name not 
    * specified and message queue name is too long. 
    */
    
    if (pcName != NULL)
    {
        strcpy (g_astCpssVosMsgQDescTbl[ulMsgQDesc].acName, pcName); 
    }
    
    return (ulMsgQDesc);
}

    
 /*******************************************************************************
* 函数名称:cpss_vos_msg_q_delete  
* 功    能:删除一个消息 队列
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* 
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:                                  
*******************************************************************************/
INT32 cpss_vos_msg_q_delete
    (
    UINT32  ulMsgQDesc
    )
{
    VOS_MSG_Q_ID    tMsgQId;
    
    /* 检查消息描述符是否合法*/
    
    if ((ulMsgQDesc <= 0) || (ulMsgQDesc >= VOS_MAX_MSG_Q_NUM) || 
        (g_astCpssVosMsgQDescTbl[ulMsgQDesc].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_delete: descriptor invalid, descriptor = %d.\n"
            , ulMsgQDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得消息队列ID*/
    
    tMsgQId = g_astCpssVosMsgQDescTbl[ulMsgQDesc].msgQId;
    
#ifdef CPSS_VOS_VXWORKS
    if (msgQDelete (tMsgQId) == ERROR)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_delete: msgQDelete failed.\n"
            );
        
        return (CPSS_ERROR);
    }
#endif /* CPSS_VOS_VXWORKS */
    
#ifdef CPSS_VOS_WINDOWS
    /* 删除信号量*/
    
    cpss_vos_sem_delete (tMsgQId->ulSemDescRecv);
    cpss_vos_sem_delete (tMsgQId->ulSemDescSend);
    cpss_vos_sem_delete (tMsgQId->ulSemDescAccess);
    
    /*释放内存空间*/
    
    free ((void *) tMsgQId);
#endif /* CPSS_VOS_WINDOWS */
    
#ifdef CPSS_DSP_CPU
    /*释放内存空间*/
    
    free ((void *) tMsgQId);
#endif

#ifdef CPSS_VOS_LINUX
    /* 删除信号量*/
    
    cpss_vos_sem_delete (tMsgQId->ulSemDescRecv);
    cpss_vos_sem_delete (tMsgQId->ulSemDescSend);
    cpss_vos_sem_delete (tMsgQId->ulSemDescAccess);
    
    /*释放内存空间*/
    
    free ((void *) tMsgQId);
#endif /* CPSS_VOS_WINDOWS */

    /* 释放消息队列描述符*/
    
    cpss_vos_msg_q_desc_free (ulMsgQDesc);
    
    return (CPSS_OK);
}



/*******************************************************************************
* 函数名称: cpss_vos_msg_q_get 
* 功    能:获取一个消息 队列
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* 
* 函数返回: 消息队列描述符
*
* 说    明:                                  
*******************************************************************************/
UINT32 cpss_vos_msg_q_get
    (
    INT8 *  pcName /* 消息队列名 */
    )
{
    UINT32  ulMsgQDesc;
    
    /* 消息队列名未指定*/
    
    if (pcName == NULL)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_get: name not specified.\n"
            );
        
        return (VOS_MSG_Q_DESC_INVALID);
    }
    
    /* 从消息队列表中查找指定的消息队列，表中元素0保留，从第一个元素开始查找*/
    
    for (ulMsgQDesc = 1; ulMsgQDesc < VOS_MAX_MSG_Q_NUM; ulMsgQDesc++)
    {
        INT32 lRet;
        lRet = strcmp (pcName, g_astCpssVosMsgQDescTbl[ulMsgQDesc].acName);
        if ((g_astCpssVosMsgQDescTbl[ulMsgQDesc].bInuse == TRUE) && 
            ( lRet == 0))
        {
            break;
        }
    }
    
    /* 没有找到 */
    
    if (ulMsgQDesc == VOS_MAX_MSG_Q_NUM)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_get: message queue not found, name = %s.\n"
            , pcName);
        
        return (VOS_MSG_Q_DESC_INVALID);
    }
    
    return (ulMsgQDesc);
}

/*******************************************************************************
* 函数名称: cpss_vos_msg_q_send
* 功    能:发送一条消息到消息 队列
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* 
* 函数返回: 
*             成功：CPSS_OK
*             失败：CPSS_ERROR
* 说    明:                                  
*******************************************************************************/
INT32 cpss_vos_msg_q_send
    (
    UINT32  ulMsgQDesc, /* 消息描述符 */
    INT8 *  pcBuf, /* 待发送的消息*/
    UINT32  ulNBytes, /* 消息长度 */
    INT32   lTimeout, /* 等待亳秒数*/
    INT32   lPri /* VOS_MSG_PRI_NORMAL or VOS_MSG_PRI_URGENT（优先级：普通，紧急） */
    )
{
#ifdef CPSS_VOS_WINDOWS
    VOS_MSG_Q_ID    tMsgQId;
    CPSS_VOS_MSG_NODE_T *    ptNode;
    INT32 lRet;
#endif
#ifdef CPSS_DSP_CPU
    VOS_MSG_Q_ID    tMsgQId;
    CPSS_VOS_MSG_NODE_T *    ptNode;
#endif

#ifdef CPSS_VOS_LINUX
    VOS_MSG_Q_ID    tMsgQId;
    CPSS_VOS_MSG_NODE_T *    ptNode;
    INT32 lRet;
#endif

#ifdef CPSS_VOS_VXWORKS
    if ((ulMsgQDesc > 0) && (ulMsgQDesc < VOS_MAX_MSG_Q_NUM) && 
        g_astCpssVosMsgQDescTbl[ulMsgQDesc].bInuse)
    {
        if ((lTimeout != NO_WAIT) && (lTimeout != WAIT_FOREVER))
        {
            lTimeout = CPSS_DIV(lTimeout * CLOCK_RATE , 1000);
            if (lTimeout == 0)
            {
                lTimeout = 1;
            }
        }
        return msgQSend (g_astCpssVosMsgQDescTbl[ulMsgQDesc].msgQId, pcBuf, ulNBytes, lTimeout, lPri);
    }
    return (CPSS_ERROR);
#else
    /* 检查消息队列描述符是否合法*/
    
    if ((ulMsgQDesc <= 0) || (ulMsgQDesc >= VOS_MAX_MSG_Q_NUM) || 
        (g_astCpssVosMsgQDescTbl[ulMsgQDesc].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_send: descriptor invalid, descriptor = %d.\n"
            , ulMsgQDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得消息队列ID */
#endif
    
#ifdef CPSS_VOS_WINDOWS
    tMsgQId = g_astCpssVosMsgQDescTbl[ulMsgQDesc].msgQId;
    /* 检查消息长度*/
    
    if (ulNBytes > ((UINT32) tMsgQId->lMaxMsgLen))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_send: buffer length invalid, bytes = %d.\n"
            , ulNBytes);
        
        return (CPSS_ERROR);
    }
    
    /* 获取发送信号量*/

    lRet = cpss_vos_sem_p (tMsgQId->ulSemDescSend, lTimeout) ;
    if (lRet == CPSS_ERROR)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_send: cpss_vos_sem_p failed.\n"
            );
        
        return (CPSS_ERROR);
    }

    /* 获取访问信号量 */
    
    lRet = cpss_vos_sem_p (tMsgQId->ulSemDescAccess, WAIT_FOREVER);
    if (lRet  == CPSS_ERROR)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_send: cpss_vos_sem_p failed.\n"
            );
        
        cpss_vos_sem_v (tMsgQId->ulSemDescSend);
        return (CPSS_ERROR);
    }
    
    
    /* 从空闲的FIFO队列中获取一个节点*/
    
    ptNode = (CPSS_VOS_MSG_NODE_T *) cpss_util_q_fifo_get (& tMsgQId->qFree);
    if (ptNode == NULL)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_send: insufficient message node available.\n"
            );
        
        cpss_vos_sem_v (tMsgQId->ulSemDescAccess);
        cpss_vos_sem_v (tMsgQId->ulSemDescSend);
        
        return (CPSS_ERROR);
    }
    
    /* 消息内容到写入获取的节点*/
    
    ptNode->lMsgLen = ulNBytes;
    cpss_mem_memcpy ((void *) VOS_MSG_NODE_DATA (ptNode), (void *) pcBuf, ulNBytes);
    
    /* 将消息节点插入消息队列 */
    
    if (lPri == VOS_MSG_PRI_URGENT)
    {
        cpss_util_q_fifo_put (& tMsgQId->qMsg, & ptNode->node, 
            UTIL_Q_FIFO_KEY_HEAD);
    }
    
    else
    {
        cpss_util_q_fifo_put (& tMsgQId->qMsg, & ptNode->node, 
            UTIL_Q_FIFO_KEY_TAIL);
    }
    
    tMsgQId->lMsgNum = tMsgQId->lMsgNum + 1;
    
    cpss_vos_sem_v (tMsgQId->ulSemDescAccess);
    cpss_vos_sem_v (tMsgQId->ulSemDescRecv);
    
    return (CPSS_OK);
#endif /* CPSS_VOS_WINDOWS */
#ifdef CPSS_DSP_CPU
    /* 检查消息长度*/
    
    tMsgQId = g_astCpssVosMsgQDescTbl[ulMsgQDesc].msgQId;
    if (ulNBytes > ((UINT32) tMsgQId->lMaxMsgLen))
        {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_send: buffer length invalid, bytes = %d.\n"
                     , ulNBytes);
        
        return (CPSS_ERROR);
        }

    /* 从空闲的FIFO队列中获取一个节点*/
    
    ptNode = (CPSS_VOS_MSG_NODE_T *) cpss_util_q_fifo_get (& tMsgQId->qFree);
    if (ptNode == NULL)
        {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_send: insufficient message node available.\n"
                     );
        
        return (CPSS_ERROR);
        }
    
    /* 消息内容到写入获取的节点*/
    
    ptNode->lMsgLen = ulNBytes;
    cpss_mem_memcpy ((void *) VOS_MSG_NODE_DATA (ptNode), (void *) pcBuf, ulNBytes);
    
    /* 将消息节点插入消息队列 */
    
    if (lPri == VOS_MSG_PRI_URGENT)
        {
        cpss_util_q_fifo_put (& tMsgQId->qMsg, & ptNode->node, 
                              UTIL_Q_FIFO_KEY_HEAD);
        }
    
    else
        {
        cpss_util_q_fifo_put (& tMsgQId->qMsg, & ptNode->node, 
                              UTIL_Q_FIFO_KEY_TAIL);
        }
    
    tMsgQId->lMsgNum = tMsgQId->lMsgNum + 1;
    
    return (CPSS_OK);
#endif

#ifdef CPSS_VOS_LINUX
    tMsgQId = g_astCpssVosMsgQDescTbl[ulMsgQDesc].msgQId;
    /* 检查消息长度*/
    
    if (ulNBytes > ((UINT32) tMsgQId->lMaxMsgLen))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_send: buffer length invalid, bytes = %d.\n"
            , ulNBytes);
        
        return (CPSS_ERROR);
    }
    
    /* 获取发送信号量*/

    lRet = cpss_vos_sem_p (tMsgQId->ulSemDescSend, lTimeout) ;
    if (lRet == CPSS_ERROR)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_send: cpss_vos_sem_p failed.\n"
            );
        
        return (CPSS_ERROR);
    }

    /* 获取访问信号量 */
    
    lRet = cpss_vos_sem_p (tMsgQId->ulSemDescAccess, WAIT_FOREVER);
    if (lRet  == CPSS_ERROR)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_send: cpss_vos_sem_p failed.\n"
            );
        
        cpss_vos_sem_v (tMsgQId->ulSemDescSend);
        return (CPSS_ERROR);
    }
    
    
    /* 从空闲的FIFO队列中获取一个节点*/
    
    ptNode = (CPSS_VOS_MSG_NODE_T *) cpss_util_q_fifo_get (& tMsgQId->qFree);
    if (ptNode == NULL)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_send: insufficient message node available.\n"
            );
        
        cpss_vos_sem_v (tMsgQId->ulSemDescAccess);
        cpss_vos_sem_v (tMsgQId->ulSemDescSend);
        
        return (CPSS_ERROR);
    }
    
    /* 消息内容到写入获取的节点*/
    
    ptNode->lMsgLen = ulNBytes;
    cpss_mem_memcpy ((void *) VOS_MSG_NODE_DATA (ptNode), (void *) pcBuf, ulNBytes);
    
    /* 将消息节点插入消息队列 */
    
    if (lPri == VOS_MSG_PRI_URGENT)
    {
        cpss_util_q_fifo_put (& tMsgQId->qMsg, & ptNode->node, 
            UTIL_Q_FIFO_KEY_HEAD);
    }
    
    else
    {
        cpss_util_q_fifo_put (& tMsgQId->qMsg, & ptNode->node, 
            UTIL_Q_FIFO_KEY_TAIL);
    }
    
    tMsgQId->lMsgNum = tMsgQId->lMsgNum + 1;
    
    cpss_vos_sem_v (tMsgQId->ulSemDescAccess);
    cpss_vos_sem_v (tMsgQId->ulSemDescRecv);
    
    return (CPSS_OK);
#endif /* CPSS_VOS_LINUX */
}


/*******************************************************************************
* 函数名称:cpss_vos_msg_q_receive
* 功    能:从消息队列接收一条消息，收到的消息长度不超过ulMaxNBytes的部分
*                 写入指定的缓冲区，超过长度部分丢掉（不返回错误指示）
*    
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* 
* 函数返回: 
*             成功：写入缓冲区的字节数
*             失败：CPSS_ERROR
* 说    明:                                  
*******************************************************************************/  
INT32 cpss_vos_msg_q_receive
(
    UINT32  ulMsgQDesc, /*消息队列描述符 */
    INT8 *  pcBuf, /* 接收消息缓冲区 */
    UINT32  ulMaxNBytes, /* 缓冲区字节数 */
    INT32   lTimeout /* 等待毫秒数*/
)
{
    VOS_MSG_Q_ID    tMsgQId;    
    INT32   lBytesRtn;
#ifdef CPSS_VOS_WINDOWS
    INT32   lRet;
    CPSS_VOS_MSG_NODE_T *  ptNode;
#endif
#ifdef CPSS_DSP_CPU
    CPSS_VOS_MSG_NODE_T *  ptNode;
#endif

#ifdef CPSS_VOS_LINUX
    INT32   lRet;
    CPSS_VOS_MSG_NODE_T *  ptNode;
#endif

    /* 检查消息队列描述符 */
    
#ifdef CPSS_VOS_VXWORKS
    if ((ulMsgQDesc > 0) && (ulMsgQDesc < VOS_MAX_MSG_Q_NUM) && 
        g_astCpssVosMsgQDescTbl[ulMsgQDesc].bInuse)
    {
        if ((lTimeout != NO_WAIT) && (lTimeout != WAIT_FOREVER))
        {
            lTimeout = CPSS_DIV(lTimeout * CLOCK_RATE , 1000);
            if (lTimeout == 0)
            {
                lTimeout = 1;
            }
        }
        return msgQReceive (g_astCpssVosMsgQDescTbl[ulMsgQDesc].msgQId, pcBuf, ulMaxNBytes, lTimeout);
    }
    return (CPSS_ERROR);
#else

    if ((ulMsgQDesc <= 0) || (ulMsgQDesc >= VOS_MAX_MSG_Q_NUM) || 
        (g_astCpssVosMsgQDescTbl[ulMsgQDesc].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_receive: descriptor invalid, descriptor = %d.\n"
            , ulMsgQDesc);
        
        return (CPSS_ERROR);
    }

    /* 获得消息队列ID*/

    tMsgQId = g_astCpssVosMsgQDescTbl[ulMsgQDesc].msgQId;
#endif  /* CPSS_VOS_VXWORKS */

#ifdef CPSS_VOS_WINDOWS

    /* 检查缓冲区长度 */

    if ((INT32) ulMaxNBytes < 0)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_receive: buffer length invalid, max bytes = %d.\n"
            , ulMaxNBytes);
        
        return (CPSS_ERROR);
    }

    /* 
    * 获得接收信号量
    */
    lRet = cpss_vos_sem_p (tMsgQId->ulSemDescRecv, lTimeout);
    if ( lRet  == CPSS_ERROR)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_receive: cpss_vos_sem_p failed.\n"
            );
        return (CPSS_ERROR);
    }
     /* 获得访问信号量*/
    lRet = cpss_vos_sem_p(tMsgQId->ulSemDescAccess, WAIT_FOREVER);
    if (lRet == CPSS_ERROR)
    {            
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_receive: cpss_vos_sem_p failed.\n"
            );
        
        cpss_vos_sem_v (tMsgQId->ulSemDescRecv);
        
        return (CPSS_ERROR);
    }

    /* 从FIFO队列中取得一个消息节点*/

    ptNode = (CPSS_VOS_MSG_NODE_T *) cpss_util_q_fifo_get (& tMsgQId->qMsg);
    if (ptNode == NULL)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_receive: insufficient message node available.\n"
            );
        
        tMsgQId->lMsgNum = 0;
        cpss_vos_sem_v (tMsgQId->ulSemDescAccess);
        cpss_vos_sem_v (tMsgQId->ulSemDescRecv);
        
        return (CPSS_ERROR);
    }

    tMsgQId->lMsgNum = tMsgQId->lMsgNum - 1;

    /* 判断消息长度并拷贝消息到缓冲区*/

    lBytesRtn = min (((UINT) ptNode->lMsgLen), ulMaxNBytes);
    cpss_mem_memcpy ((void *) pcBuf, (void *) VOS_MSG_NODE_DATA (ptNode), lBytesRtn);

    /* 将已空闲的消息结点挂入空闲FIFO队列*/

    cpss_util_q_fifo_put (& tMsgQId->qFree, & ptNode->node, UTIL_Q_FIFO_KEY_TAIL);   

    cpss_vos_sem_v (tMsgQId->ulSemDescAccess);
    cpss_vos_sem_v (tMsgQId->ulSemDescSend);

    return (lBytesRtn);
#endif /* CPSS_VOS_WINDOWS */
#ifdef CPSS_DSP_CPU
    /* 检查缓冲区长度 */

    if ((INT32) ulMaxNBytes < 0)
        {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_receive: buffer length invalid, max bytes = %d.\n"
                     , ulMaxNBytes);
        
        return (CPSS_ERROR);
        }

    /* 从FIFO队列中取得一个消息节点*/

    ptNode = (CPSS_VOS_MSG_NODE_T *) cpss_util_q_fifo_get (& tMsgQId->qMsg);
    if (ptNode == NULL)
        {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_receive: insufficient message node available.\n"
                     );
        
        tMsgQId->lMsgNum = 0;
        
        return (CPSS_ERROR);
        }
        
    tMsgQId->lMsgNum = tMsgQId->lMsgNum - 1;

    /* 判断消息长度并拷贝消息到缓冲区*/

    lBytesRtn = (((UINT) ptNode->lMsgLen) > ulMaxNBytes) ? ptNode->lMsgLen : ulMaxNBytes;
    cpss_mem_memcpy ((void *) pcBuf, (void *) VOS_MSG_NODE_DATA (ptNode), lBytesRtn);

    /* 将已空闲的消息结点挂入空闲FIFO队列*/

    cpss_util_q_fifo_put (& tMsgQId->qFree, & ptNode->node, UTIL_Q_FIFO_KEY_TAIL
                          );   

    return (lBytesRtn);
#endif

#ifdef CPSS_VOS_LINUX

    /* 检查缓冲区长度 */

    if ((INT32) ulMaxNBytes < 0)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_receive: buffer length invalid, max bytes = %d.\n"
            , ulMaxNBytes);
        
        return (CPSS_ERROR);
    }

    /* 
    * 获得接收信号量
    */
    lRet = cpss_vos_sem_p (tMsgQId->ulSemDescRecv, lTimeout);
    if ( lRet  == CPSS_ERROR)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_receive: cpss_vos_sem_p failed.\n"
            );
        return (CPSS_ERROR);
    }
     /* 获得访问信号量*/
    lRet = cpss_vos_sem_p(tMsgQId->ulSemDescAccess, WAIT_FOREVER);
    if (lRet == CPSS_ERROR)
    {            
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_receive: cpss_vos_sem_p failed.\n"
            );
        
        cpss_vos_sem_v (tMsgQId->ulSemDescRecv);
        
        return (CPSS_ERROR);
    }

    /* 从FIFO队列中取得一个消息节点*/

    ptNode = (CPSS_VOS_MSG_NODE_T *) cpss_util_q_fifo_get (& tMsgQId->qMsg);
    if (ptNode == NULL)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_receive: insufficient message node available.\n"
            );
        
        tMsgQId->lMsgNum = 0;
        cpss_vos_sem_v (tMsgQId->ulSemDescAccess);
        cpss_vos_sem_v (tMsgQId->ulSemDescRecv);
        
        return (CPSS_ERROR);
    }

    tMsgQId->lMsgNum = tMsgQId->lMsgNum - 1;

    /* 判断消息长度并拷贝消息到缓冲区*/

    lBytesRtn = ((((UINT) ptNode->lMsgLen))<(ulMaxNBytes)?(((UINT) ptNode->lMsgLen)):(ulMaxNBytes));
    cpss_mem_memcpy ((void *) pcBuf, (void *) VOS_MSG_NODE_DATA (ptNode), lBytesRtn);

    /* 将已空闲的消息结点挂入空闲FIFO队列*/

    cpss_util_q_fifo_put (& tMsgQId->qFree, & ptNode->node, UTIL_Q_FIFO_KEY_TAIL);   

    cpss_vos_sem_v (tMsgQId->ulSemDescAccess);
    cpss_vos_sem_v (tMsgQId->ulSemDescSend);

    return (lBytesRtn);
#endif /* CPSS_VOS_LINUX */

}



    
/*******************************************************************************
* 函数名称: cpss_vos_msg_q_num_msgs
* 功    能:获得在消息队列中的消息数
*    
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* 
* 函数返回: 
*             成功：消息队列中的消息数
*             失败：CPSS_ERROR
* 说    明:                                  
*******************************************************************************/
INT32 cpss_vos_msg_q_num_msgs
    (
    UINT32  ulMsgQDesc /* 消息队列描述符 */
    )
{
    VOS_MSG_Q_ID    tMsgQId;
#ifdef CPSS_VOS_VXWORKS
    INT32   lCount;
#endif /* CPSS_VOS_VXWORKS */
    
    /* 检查消息队列描述符*/
    
    if ((ulMsgQDesc <= 0) || (ulMsgQDesc >= VOS_MAX_MSG_Q_NUM) || 
        (g_astCpssVosMsgQDescTbl[ulMsgQDesc].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_num_msgs: descriptor invalid, descriptor = %d.\n"
            , ulMsgQDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得消息队列ID*/
    
    tMsgQId = g_astCpssVosMsgQDescTbl[ulMsgQDesc].msgQId;
    
#ifdef CPSS_VOS_VXWORKS
    lCount = msgQNumMsgs (tMsgQId);
    if (lCount == ERROR)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_num_msgs: msgQNumMsgs failed.\n"
            );
        
        return (CPSS_ERROR);
    }
    
    return (lCount);
#endif  /* CPSS_VOS_VXWORKS */
    
#ifdef CPSS_VOS_WINDOWS
    return (tMsgQId->lMsgNum);
#endif /* CPSS_VOS_WINDOWS */
#ifdef CPSS_DSP_CPU
    return (tMsgQId->lMsgNum);
#endif

#ifdef CPSS_VOS_LINUX
    return (tMsgQId->lMsgNum);
#endif /* CPSS_VOS_LINUX */

}


/*******************************************************************************
* 函数名称: cpss_vos_msg_q_info_get 
* 功    能:获得在消息队列的信息
*    
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* 
* 函数返回: 
*             成功：CPSS_OK
*             失败：CPSS_ERROR
* 说    明:                                  
*******************************************************************************/
INT32 cpss_vos_msg_q_info_get 
    (
    UINT32  ulMsgQDesc, /* 消息队列描述符 */
    CPSS_VOS_MSG_Q_INFO_T *  ptMsgQInfo /* 消息队列信息指针 */
    )
{
    VOS_MSG_Q_ID    tMsgQId;
#ifdef CPSS_VOS_VXWORKS
    MSG_Q_INFO  tInfo;
#endif /* CPSS_VOS_VXWORKS */
    
    /* 检查消息队列描述符 */
    
    if ((ulMsgQDesc <= 0) || (ulMsgQDesc >= VOS_MAX_MSG_Q_NUM) || 
        (g_astCpssVosMsgQDescTbl[ulMsgQDesc].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_info_get: descriptor invalid, descriptor = %d.\n"
            , ulMsgQDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得消息队列ID*/
    
    tMsgQId = g_astCpssVosMsgQDescTbl[ulMsgQDesc].msgQId;
    
#ifdef CPSS_VOS_VXWORKS
    if (msgQInfoGet (tMsgQId, & tInfo) == ERROR)
    {
        cpss_output (CPSS_MODULE_VOS, CPSS_PRINT_WARN, "cpss_vos_msg_q_info_get: msgQInfoGet failed.\n"
            );
        
        return (CPSS_ERROR);
    }
    
    /* 获得消息队列描信息 */
    
    ptMsgQInfo->ulMsgQDesc = ulMsgQDesc;
    /* 2006/5/15, jlm */
    ptMsgQInfo->pcName = g_astCpssVosMsgQDescTbl[ulMsgQDesc].acName;
    /* 2006/5/15, jlm */
    ptMsgQInfo->msgQId = g_astCpssVosMsgQDescTbl[ulMsgQDesc].msgQId;
    ptMsgQInfo->lMaxMsgs = tInfo.maxMsgs;
    ptMsgQInfo->lMaxMsgLen = tInfo.maxMsgLength;
    ptMsgQInfo->lOptions = tInfo.options;
    ptMsgQInfo->lMsgNum = tInfo.numMsgs;
    
    return (CPSS_OK);
#endif  /* CPSS_VOS_VXWORKS */
    
#ifdef CPSS_VOS_WINDOWS
    /* Get message queue information. */
    
    ptMsgQInfo->ulMsgQDesc = ulMsgQDesc;
    ptMsgQInfo->pcName = g_astCpssVosMsgQDescTbl[ulMsgQDesc].acName;
    ptMsgQInfo->msgQId = g_astCpssVosMsgQDescTbl[ulMsgQDesc].msgQId;
    ptMsgQInfo->lMaxMsgs = tMsgQId->lMaxMsgs;
    ptMsgQInfo->lMaxMsgLen = tMsgQId->lMaxMsgLen;
    ptMsgQInfo->lOptions = tMsgQId->lOptions;
    ptMsgQInfo->lMsgNum = cpss_vos_msg_q_num_msgs (ulMsgQDesc);
    
    return (CPSS_OK);
#endif /* CPSS_VOS_WINDOWS */
#ifdef CPSS_DSP_CPU
    /* Get message queue information. */
    
    ptMsgQInfo->ulMsgQDesc = ulMsgQDesc;
    ptMsgQInfo->pcName = g_astCpssVosMsgQDescTbl[ulMsgQDesc].acName;
    ptMsgQInfo->msgQId = g_astCpssVosMsgQDescTbl[ulMsgQDesc].msgQId;
    ptMsgQInfo->lMaxMsgs = tMsgQId->lMaxMsgs;
    ptMsgQInfo->lMaxMsgLen = tMsgQId->lMaxMsgLen;
    ptMsgQInfo->lOptions = tMsgQId->lOptions;
    ptMsgQInfo->lMsgNum = cpss_vos_msg_q_num_msgs (ulMsgQDesc);
    
    return (CPSS_OK);
#endif

#ifdef CPSS_VOS_LINUX
    /* Get message queue information. */
    
    ptMsgQInfo->ulMsgQDesc = ulMsgQDesc;
    ptMsgQInfo->pcName = g_astCpssVosMsgQDescTbl[ulMsgQDesc].acName;
    ptMsgQInfo->msgQId = g_astCpssVosMsgQDescTbl[ulMsgQDesc].msgQId;
    ptMsgQInfo->lMaxMsgs = tMsgQId->lMaxMsgs;
    ptMsgQInfo->lMaxMsgLen = tMsgQId->lMaxMsgLen;
    ptMsgQInfo->lOptions = tMsgQId->lOptions;
    ptMsgQInfo->lMsgNum = cpss_vos_msg_q_num_msgs (ulMsgQDesc);
    
    return (CPSS_OK);
#endif /* CPSS_VOS_LINUX */
}



/*******************************************************************************
* 函数名称:  cpss_vos_msg_q_show
* 功    能:显示消息队列信息
*    
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* 
* 函数返回: 
*             成功：CPSS_OK
*             失败：CPSS_ERROR
* 说    明:                                  
*******************************************************************************/
INT32 cpss_vos_msg_q_show 
    (
    UINT32  ulMsgQDesc, /*消息队列描述符 */
    INT32   lLevel /* 0 = 概况, 2 = 所有消息队列 */    
    )
{
    CPSS_VOS_MSG_Q_INFO_T  tMsgQInfo;

    switch (lLevel)
        {
        case 0 :
            {

            if (cpss_vos_msg_q_info_get (ulMsgQDesc, & tMsgQInfo) == CPSS_ERROR)
                {
                return (CPSS_ERROR);
                }
    
            cps__oams_shcmd_printf ("\n");
            cps__oams_shcmd_printf ("Message queue name: %s\n", tMsgQInfo.pcName);
            cps__oams_shcmd_printf ("Message queue descriptor: %d\n", tMsgQInfo.ulMsgQDesc);
            cps__oams_shcmd_printf ("Message queue ID: 0x%-8x\n", (INT32) tMsgQInfo.msgQId);
            cps__oams_shcmd_printf ("Maximum messages: %d\n", tMsgQInfo.lMaxMsgs);
            cps__oams_shcmd_printf ("Maximum message length: %d\n", tMsgQInfo.lMaxMsgLen);
            cps__oams_shcmd_printf ("Options: 0x%x\t%s\n", tMsgQInfo.lOptions, 
                ((tMsgQInfo.lOptions == VOS_MSG_Q_FIFO) ? "FIFO" : "PRIORITY"));
            cps__oams_shcmd_printf ("Message queued: %d\n", tMsgQInfo.lMsgNum);
    
            break;
            }

        case 2 :
        default :
            {
            for (ulMsgQDesc = 1; ulMsgQDesc < VOS_MAX_MSG_Q_NUM; ulMsgQDesc++)
                {
                if (g_astCpssVosMsgQDescTbl[ulMsgQDesc].bInuse == TRUE)
                    {
                    if (cpss_vos_msg_q_info_get (ulMsgQDesc, & tMsgQInfo) == CPSS_OK)
                        {
                        cps__oams_shcmd_printf ("\n");
                        cps__oams_shcmd_printf ("Message queue name: %s\n", tMsgQInfo.pcName);
                        cps__oams_shcmd_printf ("Message queue descriptor: %d\n", tMsgQInfo.ulMsgQDesc);
                        cps__oams_shcmd_printf ("Message queue ID: 0x%-8x\n", (INT32) tMsgQInfo.msgQId);
                        cps__oams_shcmd_printf ("Maximum messages: %d\n", tMsgQInfo.lMaxMsgs);
                        cps__oams_shcmd_printf ("Maximum message length: %d\n", tMsgQInfo.lMaxMsgLen);
                        cps__oams_shcmd_printf ("Options: 0x%x\t%s\n", tMsgQInfo.lOptions, 
                            ((tMsgQInfo.lOptions == VOS_MSG_Q_FIFO) ? "FIFO" : "PRIORITY"));
                        cps__oams_shcmd_printf ("Message queued: %d\n", tMsgQInfo.lMsgNum);
                        }
                    }
                }

            break;
            }
         }

    return (CPSS_OK);
}

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
 VOID   cpss_sbbr_output_task_msg_queue(UINT32 ulDrvUserId)
 {
   UINT16 ulMsgQDesc;
   UINT32 ulArgs[4];
    CPSS_VOS_MSG_Q_INFO_T  tMsgQInfo;
   
   for (ulMsgQDesc = 1; ulMsgQDesc < VOS_MAX_MSG_Q_NUM; ulMsgQDesc++)
   {
      if (g_astCpssVosMsgQDescTbl[ulMsgQDesc].bInuse == TRUE)
          {
            if (cpss_vos_msg_q_info_get (ulMsgQDesc, & tMsgQInfo) == CPSS_OK)
                 {
                    cpss_sbbr_write_text(ulDrvUserId,ulArgs,"\n\r msg queue name: %s msg queued:%d",tMsgQInfo.pcName,tMsgQInfo.lMsgNum);
            	   }
          }
   }
   
  }

#endif

INT32 cpss_task_base_timer_set(UINT32 vulMsgQueId, BOOL vbIsOpen)
{
	if(g_astCpssVosMsgQDescTbl[vulMsgQueId].bInuse == FALSE)
	{
		return CPSS_ERROR;
	}
	if((vbIsOpen > 1)||(vbIsOpen < 0))
	{
		return CPSS_ERROR;
	}

	g_astCpssVosMsgQDescTbl[vulMsgQueId].bTimer = TRUE;

	return CPSS_OK;
}

void send_msgq_loop_timer()
{
	CPSS_TIMER_BASE_MSG_T timerBuf;
	UINT32 ulMsgQDesc;

	timerBuf.ulMsgId = CPSS_TIMER_BASE_MSG;

	for(ulMsgQDesc = 0; ulMsgQDesc<VOS_MAX_MSG_Q_NUM; ulMsgQDesc++)
	{
		if(g_astCpssVosMsgQDescTbl[ulMsgQDesc].bTimer == TRUE)
		{
			cpss_vos_msg_q_send (ulMsgQDesc, (CHAR*)&timerBuf, sizeof(CPSS_TIMER_BASE_MSG_T), NO_WAIT, VOS_MSG_PRI_URGENT);
		}
	}
	return;
}


