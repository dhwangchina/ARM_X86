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
#include "cpss_public.h"
#include "cpss_common.h"
#include "cpss_util_q_fifo_lib.h"
#include "cpss_vos_msg_q.h"
#include "cpss_vos_task.h"
#include "cpss_vk_sched.h"
#include "cpss_vk_proc.h"
#include "smss_public.h"

/******************************* 全局变量定义/初始化 **************************/
extern CPSS_VK_SCHED_DESC_T    g_astCpssVkSchedDescTbl [VK_MAX_SCHED_NUM];
/* 纤程PCB表 */
CPSS_VK_PROC_PCB_T   g_astCpssVkProcPcbTbl [VK_MAX_PROC_INST_NUM];
/* 纤程描述符表 */
CPSS_VK_PROC_DESC_T  g_astCpssVkProcDescTbl [VK_MAX_PROC_INST_NUM];
/* 纤程统计表 */
CPSS_VK_PROC_STAT_T  g_astCpssVkProcStatTbl [VK_MAX_PROC_INST_NUM];
/* 纤程类表 */
CPSS_VK_PROC_CLASS_T g_astCpssVkProcClassTbl [VK_MAX_PROC_CLASS_NUM];

/******************************* 局部常数和类型定义 ***************************/
/* IPC消息池 */
 CPSS_VK_IPC_MSG_POOL_T   g_stCpssVkIpcMsgPool;
/* IPC消息标识 */
 IPC_MSG_POOL_ID g_stCpssVkIpcMsgPoolId = & g_stCpssVkIpcMsgPool;
/* 初始化 */
 BOOL g_bCpssVkProcLibInited = FALSE;

UINT8* g_pucIpcPoolAddrBegin = NULL ;

CPSS_IPC_MSG_SEND_T g_stCpssIpcStat = {0};
/******************************* 局部函数原型声明 ****************************/
/* forward declarations */
 UINT32   cpss_vk_proc_desc_alloc (UINT16 usGuid, UINT16 usInst);
 INT32    cpss_vk_proc_desc_free (UINT32 ulProcDesc);
 INT32    cpss_vk_proc_init (CPSS_VK_PROC_PCB_T *ptPcb, INT8 *pcName, 
                                   INT32 lPri, INT32 lOptions, INT8 *pcStackBase
                                   , INT32 lStackSize, void *pcVar, 
                                   INT32 lVarSize, VOID_FUNC_PTR pfEntry, 
                                   INT32 lArg);
 UINT32   cpss_vk_proc_create (UINT16 usGuid, UINT16 usInst);
 void cpss_vk_proc_summary (CPSS_VK_PROC_INFO_T *ptProcInfo);
 INT32    cpss_ipc_msg_add_to_pool (IPC_MSG_POOL_ID tPoolId, INT8 *pcPool,
                                          UINT32 ulMaxMsgs, UINT32 ulMaxMsgLen);
 INT32    cpss_ipc_msg_pool_init (IPC_MSG_POOL_ID tPoolId, INT8 *pcPool,
                                        UINT32 ulMaxMsgs, UINT32 ulMaxMsgLen);
 void *   cpss_ipc_msg_pool_alloc (IPC_MSG_POOL_ID tPoolId);
 INT32    cpss_ipc_msg_pool_free (IPC_MSG_POOL_ID tPoolId, 
                                        CPSS_VK_IPC_MSG_NODE_T *  ptNode);
 INT32    cpss_ipc_msg_pool_expire (IPC_MSG_POOL_ID tPoolId, 
                                          IPC_MSG_HDR_T *ptHdr);
                                          
/******************************* 函数实现 ************************************/

/*******************************************************************************
*函数名称:cpss_vk_proc_lib_init
*功能:纤程库初始化 
*函数类型: 
*参数: 
*参数名称 类型 输入/输出 描述
*函数返回:CPSS_OK or CPSS_ERROR
*说明:   初始化函数                              
*******************************************************************************/
STATUS cpss_vk_proc_lib_init (void)
{
    UINT32  ulIndex;
    UINT16  usGuid;
    
    if (g_bCpssVkProcLibInited == TRUE)
    {
        return (CPSS_ERROR);
    }
    
    /* 清除PCB表. */
    
    memset ((void *) (& g_astCpssVkProcPcbTbl[0]), 0, 
            (VK_MAX_PROC_INST_NUM * sizeof (CPSS_VK_PROC_PCB_T)));
    
    /* 清除纤程描述符表. */
    
    memset ((void *) (& g_astCpssVkProcDescTbl[0]), 0, 
        (VK_MAX_PROC_INST_NUM * sizeof (CPSS_VK_PROC_DESC_T)));
    
    for (ulIndex = 0; ulIndex < VK_MAX_PROC_INST_NUM; ulIndex++)
    {
        /* 索引用于PCB. */
        
        g_astCpssVkProcDescTbl[ulIndex].ulIndex = ulIndex;
        
        /* 设置bInuse. */
        
        g_astCpssVkProcDescTbl[ulIndex].bInuse = FALSE;
    }
    
    /* 清除纤程统计表. */
    
    memset ((void *) (& g_astCpssVkProcStatTbl[0]), 0, 
            (VK_MAX_PROC_INST_NUM * sizeof (CPSS_VK_PROC_STAT_T)));
    
    /* 清除纤程类表. */
    
    memset ((void *) (& g_astCpssVkProcClassTbl[0]), 0, 
            (VK_MAX_PROC_CLASS_NUM * sizeof (CPSS_VK_PROC_CLASS_T)));
    
    /* 设置bInuse. */
    
    for (usGuid = 0; usGuid < VK_MAX_PROC_CLASS_NUM; usGuid++)
    {
        g_astCpssVkProcClassTbl[usGuid].bInuse = FALSE;
    }
    
    /* Initialization finished. */
    
    g_bCpssVkProcLibInited = TRUE;
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_vk_proc_desc_alloc
*功能:纤程描述符分配 
*函数类型: 
*参数: 
*参数名称 类型   输入/输出 描述
*usGuid   UINT16 输入      GUID
*usInst   UINT16 输入     纤程实例
*函数返回:纤程描述符 or CPSS_VK_PD_INVALID
*说明:                                 
*******************************************************************************/
 UINT32 cpss_vk_proc_desc_alloc 
(
UINT16  usGuid, /* GUID */
UINT16  usInst /* 纤程实例 */
)
{
    CPSS_UTIL_Q_FIFO_NODE_T *  pNode;
    UINT32  ulIndex;
    UINT32  ulProcDesc;
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
        (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_desc_alloc: globally unique identifier invalid, GUID = 0x%04x, instance = 0x%04x.\n"
                     , usGuid, usInst);
        
        return (CPSS_VK_PD_INVALID);
    }
    
    /* 没有指定纤程描述符. */
    
    if (usInst == CPSS_VK_PNO_INVALID)
    {
        /* 检查实例 */
        
        if (g_astCpssVkProcClassTbl[usGuid].usCurrInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_desc_alloc: insufficient usInstance available, GUID = 0x%04x, instance = 0x%04x.\n"
                         , usGuid, usInst);
            
            return (CPSS_VK_PD_INVALID);
        }
        
        /* 获得节点. */
        
        pNode = cpss_util_q_fifo_get (& g_astCpssVkProcClassTbl[usGuid].qFree);
        
        /* 获得索引. */
        
        ulIndex = ((CPSS_VK_PROC_DESC_T *) pNode)->ulIndex;
        
        /* 获得实例. */
        
        usInst = (UINT16) (ulIndex - g_astCpssVkProcClassTbl[usGuid].ulBegin);
    }
    
    else
    {
        /* 检查实例 */
        
        if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN,"\ncpss_vk_proc_desc_alloc: usInstance invalid, GUID = 0x%04x, usInstance = 0x%04x.\n"
                         , usGuid, usInst);
            
            return (CPSS_VK_PD_INVALID);
        }
        
        /* 获得索引. */
        
        ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
        
        /* 检查纤程描述符. */
        
        if (g_astCpssVkProcDescTbl[ulIndex].bInuse == TRUE)
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_desc_alloc: descriptor invalid, GUID = 0x%04x, usInstance = 0x%04x.\n"
                , usGuid, usInst);
            
            return (CPSS_VK_PD_INVALID);
        }
        
        /* 获得节点. */
        
        pNode = & g_astCpssVkProcDescTbl[ulIndex].node;
        
        /* 从FREE队列移出 */
        
        cpss_util_q_fifo_remove (& g_astCpssVkProcClassTbl[usGuid].qFree, pNode);
    }
    
    /* 放入INUSE队列. */
    
    cpss_util_q_fifo_put (& g_astCpssVkProcClassTbl[usGuid].qInuse, pNode, 
        UTIL_Q_FIFO_KEY_TAIL);
    
    /* 设置bInuse. */
    
    g_astCpssVkProcDescTbl[ulIndex].bInuse = TRUE;
    
    /* 更新统计. */
    
    g_astCpssVkProcClassTbl[usGuid].usCurrInst = g_astCpssVkProcClassTbl[usGuid].usCurrInst + 1;
    
    /* 创建纤程描述符. */
    
    ulProcDesc = (UINT32) ((usGuid << 16) | (usInst)); 
    
    return (ulProcDesc);
}
 
/*******************************************************************************
*函数名称:cpss_vk_proc_desc_free
*功能:纤程描述符释放 
*函数类型: 
*参数: 
*参数名称   类型   输入/输出 描述
*ulProcDesc UINT32 输入     纤程描述符
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
 INT32 cpss_vk_proc_desc_free 
(
UINT32  ulProcDesc
)
{
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    CPSS_UTIL_Q_FIFO_NODE_T *  pNode;
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
        (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_desc_free: globally unique identifier invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得实例. */
    
    usInst = (UINT16) (ulProcDesc & 0x0000ffff);
    
    /* 检查实例. */
    
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_desc_free: instance invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 检查纤程描述符. */
    
    if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_desc_free: descriptor invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    } 
    
    /* 获得节点. */
    
    pNode = & g_astCpssVkProcDescTbl[ulIndex].node;
    
    /* 从INUSE队列移出. */
    
    cpss_util_q_fifo_remove (& g_astCpssVkProcClassTbl[usGuid].qInuse, pNode);
    
    /* 放入FREE队列. */
    
    cpss_util_q_fifo_put (& g_astCpssVkProcClassTbl[usGuid].qFree, pNode,
        UTIL_Q_FIFO_KEY_TAIL);
    
    /* 设置bInuse. */
    
    g_astCpssVkProcDescTbl[ulIndex].bInuse = FALSE;
    
    /* 更新统计. */
    
    g_astCpssVkProcClassTbl[usGuid].usCurrInst = g_astCpssVkProcClassTbl[usGuid].usCurrInst  - 1;
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_vk_proc_init
*功能:初始化纤程
*函数类型: 
*参数: 
*参数名称    类型            输入/输出 描述
*ptPcb       CPSS_VK_PROC_PCB_T * 输入 指向PCB
*pcName      INT8 *          输入      名字
*lPri        INT32           输入      优先级
*lOptions    INT32           输入      选项
*pcStackBase INT8 *          输入      堆栈基
*lStackSize  INT32           输入      堆栈大小
*pcVar       INT8 *          输入      私有数据区
*lVarSize    INT32           输入      私有数据区大小
*pfEntry     VOID_FUNC_PTR   输入      入口点
*lArg        INT32           输入      参数
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
 INT32 cpss_vk_proc_init 
(
CPSS_VK_PROC_PCB_T *   ptPcb, /* 指向PCB */
INT8 *  pcName, /* 名字 */
INT32   lPri, /* 优先级 */
INT32   lOptions,  /* 选项 */
INT8 *  pcStackBase, /* 堆栈基 */
INT32   lStackSize, /* 堆栈大小 */
void *  pcVar, /* 私有数据区 */
INT32   lVarSize, /* 私有数据区大小 */
VOID_FUNC_PTR   pfEntry, /* 入口点 */
INT32   lArg /* 参数 */
)
{
    UINT8   ucTimerNo;
    
    /* 初始化问题. */
    
    if ((!g_bCpssVkProcLibInited) && (cpss_vk_proc_lib_init () == CPSS_ERROR))
    {
        return (CPSS_ERROR);
    }
    
    /* 清除PCB结构. */
    
    memset ((void *) ptPcb, 0, sizeof (CPSS_VK_PROC_PCB_T));
    
    /* 检查优先级范围, ! (0 <= x <= 255). */
    
    if ((lPri & 0xFFFFFF00) != 0)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_init: priority illegal, priority = %d.\n"
                     , lPri);
        
        return (CPSS_ERROR);
    }
    
    /* 初始化PCB剩余部分. */
      
    if (pcName != NULL)
    {
        strcpy (ptPcb->acName, pcName);
    }
    ptPcb->lOptions = lOptions;
    ptPcb->lStatus = VK_PROC_STATUS_SUSPEND;
    ptPcb->lPri = lPri;
    ptPcb->pfEntry = pfEntry;
    ptPcb->pcStackBase = pcStackBase;
    ptPcb->pcStackLimit = pcStackBase - lStackSize;
    ptPcb->pcStackEnd = ptPcb->pcStackLimit;
    ptPcb->lStackSize = lStackSize;
    memset ((void *) ptPcb->pcStackLimit, 0, lStackSize);
    ptPcb->pcVar = pcVar;
    memset ((void *) ptPcb->pcVar, 0, lVarSize);
    ptPcb->lVarSize = lVarSize;
    cpss_util_q_fifo_init (& ptPcb->mailBox);
    ptPcb->lMailNum = 0;
    ptPcb->ulHash = (UINT32) lArg;
    ptPcb->usState = VK_PROC_STATE_INACTIVE;
    ptPcb->ulParaTMCBAVLRoot = 0xFFFFFFFF;
    ptPcb->ulParaTMCBHead = 0xFFFFFFFF;
    
    for (ucTimerNo = 0; ucTimerNo < 32; ucTimerNo++)
    {
        ptPcb->aulTimerId[ucTimerNo] = CPSS_TD_INVALID;
        ptPcb->aulParaRoot[ucTimerNo] = CPSS_TD_INVALID;
    }
    
    return (CPSS_OK);    
}

/*******************************************************************************
*函数名称:cpss_vk_proc_create
*功能:纤程创建 
*函数类型: 
*参数: 
*参数名称 类型   输入/输出 描述
*usGuid   UINT16 输入      GUID
*usInst   UINT16 输入      实例
*函数返回:纤程描述符 or CPSS_VK_PD_INVALID
*说明:                                 
*******************************************************************************/
 UINT32 cpss_vk_proc_create
(
UINT16  usGuid, /* GUID */
UINT16  usInst /* 实例 */
)
{
    UINT32  ulProcDesc;
    INT8    acName [VK_PROC_NAME_LEN];
    UINT32  ulIndex;
    VK_PROC_ID  tProcId;
    INT32   lStackSize; 
    INT8 *  pcMem;
    INT8 *  pcStackBase;
    INT32   lVarSize;
    INT8 *  pcVar;
    
    /* 分配纤程描述符. */
    
    ulProcDesc = cpss_vk_proc_desc_alloc (usGuid, usInst);
    if (ulProcDesc == CPSS_VK_PD_INVALID)
    {
        return (CPSS_VK_PD_INVALID);
    }
    
    /* 获得实例. */
    
    usInst = (UINT16) (ulProcDesc & 0x0000ffff);
    
    /* 创建新名字. */
    
    snprintf (acName, VK_PROC_NAME_LEN, "%s%d", g_astCpssVkProcClassTbl[usGuid].acName, usInst);
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 获得纤程标识. */
    
    tProcId = & g_astCpssVkProcPcbTbl[ulIndex];
    
    /* 计算堆栈大小. */
    
    lStackSize = g_astCpssVkProcClassTbl[usGuid].lStackSize;
    if (lStackSize == 0)
    {
        lStackSize = VK_PROC_DEF_STACK_SIZE;
    }
    
    /* 分配堆栈. */
    
    pcMem = (INT8 *) malloc (lStackSize);
    if (pcMem == NULL)
    {
        /* 释放纤程描述符. */
        
        cpss_vk_proc_desc_free (ulProcDesc);
        
        return (CPSS_VK_PD_INVALID);
    }
    
    /* 堆栈向下. */
    
    pcStackBase = pcMem + lStackSize;
    
    /* 计算私有数据区大小. */
    
    lVarSize = g_astCpssVkProcClassTbl[usGuid].lVarSize;
    if (lVarSize == 0)
    {
        lVarSize = VK_PROC_DEF_VAR_SIZE;
    }
    
    /* 分配私有数据区. */
    
    pcVar = malloc (lVarSize);
    if (pcVar == NULL)
    {
        /* 释放堆栈. */
        
        free ((void *) pcMem);
        
        /* 释放纤程描述符. */
        
        cpss_vk_proc_desc_free (ulProcDesc);
        
        return (CPSS_VK_PD_INVALID);
    }
    
    /* 初始化纤程. */
    
    if (cpss_vk_proc_init (tProcId, acName, g_astCpssVkProcClassTbl[usGuid].lPri, 
        g_astCpssVkProcClassTbl[usGuid].lOptions, pcStackBase, lStackSize, pcVar, 
        lVarSize, g_astCpssVkProcClassTbl[usGuid].pfEntry, (INT32) ulProcDesc) == 
        CPSS_ERROR)
    {
        /* 释放私有数据区. */
        
        free ((void *) pcVar);
        
        /* 释放堆栈. */
        
        free ((void *) pcMem);
        
        /* 释放纤程描述符. */
        
        cpss_vk_proc_desc_free (ulProcDesc);
        
        return (CPSS_VK_PD_INVALID);
    }
    
    if (cpss_vk_sched_proc_spawn (g_astCpssVkProcClassTbl[usGuid].ulSchedDesc, tProcId
        ) == CPSS_ERROR)
    {
        /* 释放私有数据区. */
        
        free ((void *) pcVar);
        
        /* 释放堆栈. */
        
        free ((void *) pcMem);
        
        /* 释放纤程描述符. */
        
        cpss_vk_proc_desc_free (ulProcDesc);
        
        return (CPSS_VK_PD_INVALID);
    }
    
    /* 保存纤程标识. */
    
    g_astCpssVkProcDescTbl[ulIndex].tProcId = tProcId;
    
    return (ulProcDesc);
    }
    
/*******************************************************************************
*函数名称:cpss_vk_proc_spawn
*功能:纤程创建(并激活) 
*函数类型: 
*参数: 
*参数名称 类型   输入/输出 描述
*usGuid   UINT16 输入      GUID
*usInst   UINT16 输入      实例
*函数返回:纤程描述符 or CPSS_VK_PD_INVALID
*说明:                                 
*******************************************************************************/
UINT32 cpss_vk_proc_spawn
(
UINT16  usGuid, /* GUID */
UINT16  usInst /* 实例 */
)
{
    UINT32  ulProcDesc;
    
    /* 创建纤程. */
    
    ulProcDesc = cpss_vk_proc_create (usGuid, usInst);
    if (ulProcDesc == CPSS_VK_PD_INVALID)
    {
        return (CPSS_VK_PD_INVALID);
    }
    
    /* 激活纤程. */
    /*
    vk_proc_activate (ulProcDesc);
    */
    return (ulProcDesc);
}
#ifndef CPSS_DSP_CPU
/*******************************************************************************
*函数名称:cpss_vk_proc_delete
*功能:纤程删除 
*函数类型: 
*参数: 
*参数名称 类型     输入/输出 描述
*ulProcDesc UINT32 输入      纤程描述符
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
INT32 cpss_vk_proc_delete 
(
UINT32  ulProcDesc /* 纤程描述符 */
)
{
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    VK_PROC_ID  tProcId;
    
    /* 纤程在运行? */
    
    if (ulProcDesc == (cpss_vk_pd_self_get ()))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_delete: procedure running, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
        (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_delete: globally unique identifier invalid, descriptor = 0x%08x.\n"
            , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得实例. */
    
    usInst = (UINT16) (ulProcDesc & 0x0000ffff);
    
    /* 检查实例. */
    
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN,"\ncpss_vk_proc_delete: instance invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 检查纤程描述符. */
    
    if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_delete: descriptor invalid, descriptor = 0x%08x.\n"
            , ulProcDesc);
        
        return (CPSS_ERROR);
    } 
    
    /* 获得纤程标识. */
    
    tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;
    
    /* 悬挂. */
    
    if (cpss_vk_sched_proc_suspend (g_astCpssVkProcClassTbl[usGuid].ulSchedDesc, 
        tProcId) == CPSS_ERROR)
    {
        return (CPSS_ERROR);
    }
    
    /* 删除. */
    
    if (cpss_vk_sched_proc_delete (g_astCpssVkProcClassTbl[usGuid].ulSchedDesc, 
        tProcId) == CPSS_ERROR)
    {
        return (CPSS_ERROR);
    }
    
    /* 释放私有数据区. */
    
    free ((void *) tProcId->pcVar);
    
    /* 释放堆栈. */
    
    free ((void *) tProcId->pcStackEnd);
    
    /* 释放纤程描述符. */
    
    cpss_vk_proc_desc_free (ulProcDesc);
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_vk_proc_suspend
*功能:纤程悬挂 
*函数类型: 
*参数: 
*参数名称   类型   输入/输出 描述
*ulProcDesc UINT32 输入      纤程描述符
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
INT32 cpss_vk_proc_suspend 
(
UINT32  ulProcDesc /* 纤程描述符 */
)
{
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    VK_PROC_ID  tProcId;
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    /* 检查GUID */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
        (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vos_proc_suspend: globally unique identifier invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得实例. */
    
    usInst = (UINT16) (ulProcDesc & 0x0000ffff);
    
    /* 检查实例. */
    
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vos_proc_suspend: instance invalid, descriptor = 0x%08x.\n"
            , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 检查纤程描述符. */
    
    if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vos_proc_suspend: descriptor invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    } 
    
    /* 获得纤程标识. */
    
    tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;
    
    /* 悬挂. */
    
    if (cpss_vk_sched_proc_suspend (g_astCpssVkProcClassTbl[usGuid].ulSchedDesc, 
        tProcId) == CPSS_ERROR)
    {
        return (CPSS_ERROR);
    }
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_vk_proc_resume
*功能:纤程恢复
*函数类型: 
*参数: 
*参数名称   类型   输入/输出 描述
*ulProcDesc UINT32 输入      纤程描述符
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
INT32 cpss_vk_proc_resume 
(
UINT32  ulProcDesc /* 纤程描述符 */
)
{
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    VK_PROC_ID  tProcId;
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
        (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vos_proc_resume: globally unique identifier invalid, descriptor = 0x%08x.\n"
            , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得实例. */
    
    usInst = (UINT16) (ulProcDesc & 0x0000ffff);
    
    /* 检查实例. */
    
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vos_proc_resume: instance invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 检查纤程描述符. */
    
    if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vos_proc_resume: descriptor invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    } 
    
    /* 获得纤程标识. */
    
    tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;
    
    /* 恢复. */
    
    if (cpss_vk_sched_proc_resume (g_astCpssVkProcClassTbl[usGuid].ulSchedDesc, 
        tProcId) == CPSS_ERROR)
    {
        return (CPSS_ERROR);
    }
    
    return (CPSS_OK);
}
#endif
/*******************************************************************************
*函数名称:cpss_vk_pd_get
*功能:纤程描述符获得
*函数类型: 
*参数: 
*参数名称 类型   输入/输出 描述
*usGuid   UINT16 输入      GUID
*usInst   UINT16 输入      实例
*函数返回:纤程描述符 or CPSS_VK_PD_INVALID
*说明:                                 
*******************************************************************************/
UINT32 cpss_vk_pd_get
(
UINT16  usGuid, /* GUID */
UINT16  usInst /* 实例 */
)
{
    UINT32  ulIndex;
    UINT32  ulProcDesc;
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
        (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_pd_get: globally unique identifier invalid, GUID = 0x%04x, instance = 0x%04x.\n"
                     , usGuid, usInst);
        
        return (CPSS_VK_PD_INVALID);
    }
    
    /* 检查实例. */
    
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_pd_get: instance invalid, GUID = 0x%04x, instance = 0x%04x.\n"
                     , usGuid, usInst);
        
        return (CPSS_VK_PD_INVALID);
    }
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 检查纤程描述符. */
    
    if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_pd_get: descriptor invalid, GUID = 0x%04x, instance = 0x%04x.\n"
            , usGuid, usInst);
        
        return (CPSS_VK_PD_INVALID);
    } 
    
    /* 创建纤程描述符. */
    
    ulProcDesc = (UINT32) ((usGuid << 16) | (usInst)); 
    
    return (ulProcDesc);
}

/*******************************************************************************
*函数名称:cpss_vk_pd_self_get
*功能:当前运行纤程描述符获得
*函数类型: 
*参数: 
*参数名称 类型 输入/输出 描述
*函数返回:纤程描述符 or CPSS_VK_PD_INVALID
*说明:                                 
*******************************************************************************/
UINT32 cpss_vk_pd_self_get (void)
{
    UINT32  ulSchedDesc;
        
    ulSchedDesc = cpss_vk_sched_desc_self ();
    if (ulSchedDesc == VK_SCHED_DESC_INVALID)
    {
        return (CPSS_VK_PD_INVALID);
    }   
    /* 获得调度器标识. */
    return g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId->ulProcDescCurr;
}

/*******************************************************************************
*函数名称:cpss_vk_proc_user_data_get
*功能:当前运行纤程私有数据区获得
*函数类型: 
*参数: 
*参数名称 类型 输入/输出 描述
*函数返回:私有数据区指针 or NULL
*说明:                                 
*******************************************************************************/
void * cpss_vk_proc_user_data_get (void)
{
    UINT32  ulProcDesc;
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    VK_PROC_ID  tProcId;
    
    ulProcDesc = cpss_vk_pd_self_get ();
    if (ulProcDesc == CPSS_VK_PD_INVALID)
    {
        return (NULL);
    }
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
        (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_user_data_get: globally unique identifier invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (NULL);
    }
    
    /* 获得实例. */
    
    usInst = (UINT16) (ulProcDesc & 0x0000ffff);
    
    /* 检查实例. */
    
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_user_data_get: instance invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (NULL);
    }
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 检查纤程描述符. */
    
    if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_user_data_get: descriptor invalid, descriptor = 0x%08x.\n"
            , ulProcDesc);
        
        return (NULL);
    } 
    
    /* 获得纤程标识. */
    
    tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;
    
    return ((void *) tProcId->pcVar);
}

/*******************************************************************************
*函数名称:cpss_vk_proc_user_state_set
*功能:当前运行纤程业务状态设置
*函数类型: 
*参数: 
*参数名称   类型   输入/输出 描述
*usNewState UINT16 输入      新业务状态
*函数返回:私有数据区指针 or NULL
*说明:                                 
*******************************************************************************/
INT32 cpss_vk_proc_user_state_set 
(
UINT16  usNewState /* 新业务状态 */
)
{
    UINT32  ulProcDesc;
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    VK_PROC_ID  tProcId;
    
    ulProcDesc = cpss_vk_pd_self_get ();
    if (ulProcDesc == CPSS_VK_PD_INVALID)
    {
        return (CPSS_ERROR);
    }
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
        (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_user_state_set: globally unique identifier invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得实例. */
    
    usInst = (UINT16) (ulProcDesc & 0x0000ffff);
    
    /* 检查实例. */
    
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_user_state_set: instance invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 检查纤程描述符. */
    
    if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_user_state_set: descriptor invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    } 
    
    /* 获得纤程标识. */
    
    tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;
    
    /* 设置业务状态. */
    
    tProcId->usState = usNewState;
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_vk_proc_user_state_get
*功能:当前运行纤程业务状态获得
*函数类型: 
*参数: 
*参数名称 类型     输入/输出 描述
*pusState UINT16 * 输出      指向业务状态
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
INT32 cpss_vk_proc_user_state_get 
(
UINT16 *    pusState /* 指向业务状态 */
)
{
    UINT32  ulProcDesc;
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    VK_PROC_ID  tProcId;
    
    ulProcDesc = cpss_vk_pd_self_get ();
    if (ulProcDesc == CPSS_VK_PD_INVALID)
    {
        return (CPSS_ERROR);
    }
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
        (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_user_usState_get: globally unique identifier invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得实例. */
    
    usInst = (UINT16) (ulProcDesc & 0x0000ffff);
    
    /* 检查实例. */
    
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_user_usState_get: instance invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 检查纤程描述符. */
    
    if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_user_usState_get: descriptor invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    } 
    
    /* 获得纤程标识. */
    
    tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;
    
    /* 获得业务状态. */
    
    *pusState = tProcId->usState;
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_vk_proc_ticks_flush
*功能:刷新ticks
*函数类型: 
*参数: 
*参数名称   类型   输入/输出 描述
*ulProcDesc UINT32 输入      纤程描述符
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
#ifndef CPSS_DSP_CPU
INT32 cpss_vk_proc_ticks_flush 
    (
    UINT32  ulProcDesc /* 纤程描述符 */
    )
    {
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    VK_PROC_ID  tProcId;
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
        (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_ticks_flush: globally unique identifier invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得实例. */
    
    usInst = (UINT16) (ulProcDesc & 0x0000ffff);
    
    /* 检查实例. */
    
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_ticks_flush: instance invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 检查纤程描述符. */
    
    if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_ticks_flush: descriptor invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    } 
    
    /* 获得纤程标识. */
    
    tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;
    
    /* 设置业务状态. */
    
    tProcId->lMaxTicks = 0;
    
    return (CPSS_OK);
    }
#endif
/*******************************************************************************
*函数名称:cpss_vk_proc_pcb_get
*功能:纤程PCB获得
*函数类型: 
*参数: 
*参数名称   类型   输入/输出 描述
*ulProcDesc UINT32 输入      纤程描述符
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
CPSS_VK_PROC_PCB_T * cpss_vk_proc_pcb_get 
(
UINT32  ulProcDesc /* 纤程描述符 */
)
{
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    if ((usGuid > 0) && (usGuid < VK_MAX_PROC_CLASS_NUM) &&
        g_astCpssVkProcClassTbl[usGuid].bInuse)
    {    
        /* 获得实例. */
        
        usInst = (UINT16) (ulProcDesc & 0x0000ffff);
        
        /* 检查实例. */
        
        if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_pcb_get: instance invalid, descriptor = 0x%08x.\n"
                         , ulProcDesc);
            
            return (NULL);
        }
        
        /* 获得索引. */
        
        ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
        
        /* 检查纤程描述符. */
        
        if (!g_astCpssVkProcDescTbl[ulIndex].bInuse)
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_pcb_get: descriptor invalid, descriptor = 0x%08x.\n"
                         , ulProcDesc);
    
            return (NULL);
        }

        return (g_astCpssVkProcDescTbl[ulIndex].tProcId);
    }
    return (NULL);
}

/*******************************************************************************
*函数名称:cpss_vk_proc_sched_get
*功能:纤程的调度器获得
*函数类型: 
*参数: 
*参数名称   类型   输入/输出 描述
*ulProcDesc UINT32 输入      纤程描述符
*函数返回:调度器描述符 or VK_SCHED_DESC_INVALID
*说明:                                 
*******************************************************************************/
UINT32 cpss_vk_proc_sched_get
(
UINT32  ulProcDesc /* 纤程描述符 */
)
{
    UINT16  usGuid;
#if 0
    UINT16  usInst;
    UINT32  ulIndex;
#endif
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
        (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_sched_get: globally unique identifier invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (VK_SCHED_DESC_INVALID);
    }
#if 0
    /* 获得实例. */
    
    usInst = (UINT16) (ulProcDesc & 0x0000ffff);
    
    /* 检查实例. */
    
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_sched_get: instance invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (VK_SCHED_DESC_INVALID);
    }
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 检查纤程描述符. */
    
    if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_sched_get: descriptor invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (VK_SCHED_DESC_INVALID);
    } 
#endif
    return (g_astCpssVkProcClassTbl[usGuid].ulSchedDesc);
}

/*******************************************************************************
*函数名称:cpss_vk_proc_activate
*功能:纤程激活
*函数类型: 
*参数: 
*参数名称   类型   输入/输出 描述
*ulProcDesc UINT32 输入      纤程描述符
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/

INT32 cpss_vk_proc_activate
(
UINT32  ulProcDesc /* 纤程描述符 */
)
{
    IPC_MSG_HDR_T *    ptHdr;
    CPSS_COM_MSG_HEAD_T *    ptData;
    
    /* 分配IPC消息. */
    CPSS_DSP_FILE_REC();
    CPSS_DSP_LINE_REC();
    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulVkActivateInCount);
    
    ptHdr = (IPC_MSG_HDR_T *) cpss_ipc_msg_alloc (sizeof (CPSS_COM_MSG_HEAD_T));
    if (ptHdr == NULL)
    {
        CPSS_DSP_LINE_REC();
        CPSS_DSP_ERROR_WAIT;
        return (CPSS_ERROR);
    }
    
    /* 清除头部. */
    
    memset ((void *) ptHdr, 0, sizeof (IPC_MSG_HDR_T));
    
    /* 设置头部. */
    
    ptHdr->ptNext = NULL;
    ptHdr->lTimeout = WAIT_FOREVER;
    /*
    ptHdr->totalLen = sizeof (IPC_MSG_HDR_T) + sizeof (CPSS_COM_MSG_HEAD_T);
    */
    /* 指向数据. */
    
    ptData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr);
    
    /* 清除数据. */
    
    memset ((void *) ptData, 0, sizeof (CPSS_COM_MSG_HEAD_T));
    
    ptData->stDstProc.ulPd = ulProcDesc;
    ptData->ulMsgId = IPC_MSG_ID_ACTIVE;
    ptData->ulLen = 0;
    ptData->pucBuf = NULL;
    ptData->ucPriFlag = VOS_MSG_PRI_NORMAL;
    
    /* 发送. */
    
    if (cpss_ipc_msg_send (ptHdr, IPC_MSG_TYPE_REMOTE) == CPSS_ERROR)
    {
        CPSS_DSP_LINE_REC();
        CPSS_DSP_ERROR_WAIT;
        return (CPSS_ERROR);
    }
    
    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulVkActivateOutCount);
    
    return (CPSS_OK);
}

#ifndef CPSS_DSP_CPU
/*******************************************************************************
*函数名称:cpss_vk_proc_info_get
*功能:纤程信息获得
*函数类型: 
*参数: 
*参数名称   类型             输入/输出 描述
*ulProcDesc UINT32           输入      纤程描述符
*ptProcInfo CPSS_VK_PROC_INFO_T * 输出 指向纤程信息
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
INT32 cpss_vk_proc_info_get 
(
UINT32  ulProcDesc, /* 纤程描述符 */
CPSS_VK_PROC_INFO_T * ptProcInfo /* 指向纤程信息 */
)
{
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    VK_PROC_ID  tProcId;
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
        (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_info_get: globally unique identifier invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得实例. */    
    usInst = (UINT16) (ulProcDesc & 0x0000ffff);    
    /* 检查实例. */    
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_info_get: instance invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 检查纤程描述符. */
    
    if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_info_get: descriptor invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    } 
    
    /* 获得纤程标识. */
    
    tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;
    
    /* 获得纤程信息. */
    
    ptProcInfo->ulProcDesc = ulProcDesc;
    ptProcInfo->tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;
    ptProcInfo->pcName = tProcId->acName;
    ptProcInfo->lOptions = tProcId->lOptions;
    ptProcInfo->lPri = tProcId->lPri;
    ptProcInfo->lStatus = tProcId->lStatus;
    ptProcInfo->pcStackBase = tProcId->pcStackBase;
    ptProcInfo->pcStackLimit = tProcId->pcStackLimit;
    ptProcInfo->pcStackEnd = tProcId->pcStackEnd;
    ptProcInfo->lStackSize = tProcId->lStackSize;
    ptProcInfo->pcVar = tProcId->pcVar;
    ptProcInfo->lVarSize = tProcId->lVarSize;
#ifdef CPSS_VOS_VXWORKS
    ptProcInfo->lMaxTicks = tProcId->lMaxTicks;
   ptProcInfo->ulProcNTime = tProcId->ulProcNTime;
    ptProcInfo->ulProcNTimeb = tProcId->ulProcNTimeb;
    ptProcInfo->ulProcNTimee = tProcId->ulProcNTimee;
   tProcId->ulProcNTime  = 0;
   tProcId->ulProcNTimeb = 0;
   tProcId->ulProcNTimee = 0;
   
   
#endif    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_vk_proc_stat_get
*功能:纤程统计获得
*函数类型: 
*参数: 
*参数名称   类型             输入/输出 描述
*ulProcDesc UINT32           输入      纤程描述符
*pProcStat  CPSS_VK_PROC_STAT_T * 输出 指向纤程统计
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
INT32 cpss_vk_proc_stat_get 
(
UINT32  ulProcDesc, /* 纤程描述符 */
CPSS_VK_PROC_STAT_T *  pProcStat /* 指向纤程统计 */
)
{
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    VK_PROC_ID  tProcId;
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
        (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_stat_get: globally unique identifier invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得实例. */
    
    usInst = (UINT16) (ulProcDesc & 0x0000ffff);
    
    /* 检查实例. */
    
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_stat_get: instance invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 检查纤程描述符. */
    
    if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_stat_get: descriptor invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    } 
    
    /* 获得纤程标识. */
    
    tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;
    
    /* 获得统计，更新总的统计. 读取后，清除它. */
    
    pProcStat->ulInteCallNum = g_astCpssVkProcStatTbl[ulIndex].ulInteCallNum;
    g_astCpssVkProcStatTbl[ulIndex].ulInteCallNum = 0;
    g_astCpssVkProcStatTbl[ulIndex].ulTotalCallNum += pProcStat->ulInteCallNum;
    pProcStat->ulInteMailSent = g_astCpssVkProcStatTbl[ulIndex].ulInteMailSent;
    g_astCpssVkProcStatTbl[ulIndex].ulInteMailSent = 0;
    g_astCpssVkProcStatTbl[ulIndex].ulTotalMailSent += pProcStat->ulInteMailSent;
    pProcStat->ulInteBytesSent = g_astCpssVkProcStatTbl[ulIndex].ulInteBytesSent;
    g_astCpssVkProcStatTbl[ulIndex].ulInteBytesSent = 0;
    g_astCpssVkProcStatTbl[ulIndex].ulTotalBytesSent += pProcStat->ulInteBytesSent;
    pProcStat->ulInteMailRecv = g_astCpssVkProcStatTbl[ulIndex].ulInteMailRecv;
    g_astCpssVkProcStatTbl[ulIndex].ulInteMailRecv = 0;
    g_astCpssVkProcStatTbl[ulIndex].ulTotalMailRecv += pProcStat->ulInteMailRecv;
    pProcStat->ulInteBytesRecv = g_astCpssVkProcStatTbl[ulIndex].ulInteBytesRecv;
    g_astCpssVkProcStatTbl[ulIndex].ulInteBytesRecv = 0;
    g_astCpssVkProcStatTbl[ulIndex].ulTotalBytesRecv += pProcStat->ulInteBytesRecv;
    
    /* 获得总的统计. */
    
    pProcStat->ulTotalCallNum = g_astCpssVkProcStatTbl[ulIndex].ulTotalCallNum;
    pProcStat->ulTotalMailSent = g_astCpssVkProcStatTbl[ulIndex].ulTotalMailSent;
    pProcStat->ulTotalBytesSent = g_astCpssVkProcStatTbl[ulIndex].ulTotalBytesSent;
    pProcStat->ulTotalMailRecv = g_astCpssVkProcStatTbl[ulIndex].ulTotalMailRecv;
    pProcStat->ulTotalBytesRecv = g_astCpssVkProcStatTbl[ulIndex].ulTotalBytesRecv;
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_vk_proc_tatus_string
*功能:纤程状态字符串
*函数类型: 
*参数: 
*参数名称   类型   输入/输出 描述
*ulProcDesc UINT32 输入      纤程描述符
*pcString   INT8 * 输出      打印的字符串
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
INT32 cpss_vk_proc_tatus_string
(
UINT32  ulProcDesc, /* 纤程描述符 */
INT8 *  pcString
)
{
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    VK_PROC_ID  tProcId;
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
        (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_tatus_string: globally unique identifier invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得实例. */
    
    usInst = (UINT16) (ulProcDesc & 0x0000ffff);
    
    /* 检查实例. */
    
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_tatus_string: instance invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 检查纤程描述符. */
    
    if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_tatus_string: descriptor invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        
        return (CPSS_ERROR);
    } 
    
    /* 获得纤程标识. */
    
    tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;
    
    switch (tProcId->lStatus)
    {
        case VK_PROC_STATUS_READY :
        {
            strcpy (pcString, "READY");
            
            break;
        }
    
        case VK_PROC_STATUS_PEND :
        {
            strcpy (pcString, "PEND");
            
            break;
        }
        
        case VK_PROC_STATUS_PEND | VK_PROC_STATUS_SUSPEND :
        {
            strcpy (pcString, "PEND+S");
            
            break;
        }
        
        case VK_PROC_STATUS_DEAD:
        {
            strcpy (pcString, "DEAD");
            
            break;
        }
        
        default:
        {
            /* 不想要的组合. */
            
            snprintf (pcString, 10, "0x%02x", tProcId->lStatus);
            
            return (CPSS_ERROR);
            
            break;
        }
    }

    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_vk_proc_summary
*功能:纤程总结
*函数类型: 
*参数: 
*参数名称   类型             输入/输出 描述
*ptProcInfo CPSS_VK_PROC_INFO_T * 输入 指向纤程信息
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
 void cpss_vk_proc_summary
(
CPSS_VK_PROC_INFO_T * ptProcInfo /* 指向纤程信息 */
)
{
    INT8    acStatusString [10];
    
    /* 获得统计字符串. */
    
    cpss_vk_proc_tatus_string (ptProcInfo->ulProcDesc, acStatusString);
    
    cps__oams_shcmd_printf ("%-10s  %-20x  0x%08x    %-8x  %s\n", ptProcInfo->pcName, 
                       ptProcInfo->ulProcDesc, (INT32) ptProcInfo->tProcId, 
                       ptProcInfo->lPri, acStatusString);
}

/*******************************************************************************
*函数名称:cpss_vk_proc_show
*功能:纤程显示
*函数类型: 
*参数: 
*参数名称   类型   输入/输出 描述
*usGuid     UINT16 输入      GUID
*ulProcDesc UINT32 输入      纤程描述符
*lLevel     INT32  输入      不同的类型
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
STATUS cpss_vk_proc_show 
(
UINT16  usGuid, /* GUID */
UINT32  ulProcDesc, /* 纤程描述符 */
INT32   lLevel /* 
               * 0 = 纤程类summary, 1 = 纤程类detail, 
               * 2 = 纤程summary, 3 = 纤程detail
               */
)
{
    CPSS_VK_PROC_INFO_T    tProcInfo;
    CPSS_VK_PROC_STAT_T    tProcStat;
    UINT32  ulIndex;
    UINT16  usInst;
    
    switch (lLevel)
    {
        case 0 :
        {
            /* 检查GUID. */
            
            if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
                (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
            {
                cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_show: globally unique identifier invalid, GUID = 0x%04x.\n"
                             , usGuid);
                
                return (CPSS_ERROR);
            }
            
            cps__oams_shcmd_printf ("\n");
            cps__oams_shcmd_printf ("Procedure class name: %s\n", 
                    g_astCpssVkProcClassTbl[usGuid].acName);
            cps__oams_shcmd_printf ("Globally unique identifier(GUID): 0x%04x\n", 
                    g_astCpssVkProcClassTbl[usGuid].usGuid);
            cps__oams_shcmd_printf ("Beginning index in procedure control block(PCB): %d\n", 
                    g_astCpssVkProcClassTbl[usGuid].ulBegin);
            cps__oams_shcmd_printf ("Total instance: %d\n", 
                    g_astCpssVkProcClassTbl[usGuid].usTotalInst);
            cps__oams_shcmd_printf ("Current instance: %d\n", 
                    g_astCpssVkProcClassTbl[usGuid].usCurrInst);
            cps__oams_shcmd_printf ("Scheduler descriptor: %d\n", 
                    g_astCpssVkProcClassTbl[usGuid].ulSchedDesc);
            
            break;
        }
    
    case 1 :
        {
            /* 检查GUID. */
            
            if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
                (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
            {
                cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_proc_show: globally unique identifier invalid, GUID = 0x%04x.\n"
                    , usGuid);
                
                return (CPSS_ERROR);
            }
            
            cps__oams_shcmd_printf ("\n");
            cps__oams_shcmd_printf ("Procedure class name: %s\n", g_astCpssVkProcClassTbl[usGuid].acName
                );
            cps__oams_shcmd_printf ("Globally unique identifier(GUID): 0x%04x\n", 
                g_astCpssVkProcClassTbl[usGuid].usGuid);
            cps__oams_shcmd_printf ("Beginning index in procedure control block(PCB): %d\n", 
                g_astCpssVkProcClassTbl[usGuid].ulBegin);
            cps__oams_shcmd_printf ("Total instance: %d\n", g_astCpssVkProcClassTbl[usGuid].usTotalInst)
                ;
            cps__oams_shcmd_printf ("Current instance: %d\n", g_astCpssVkProcClassTbl[usGuid].usCurrInst
                );
            cps__oams_shcmd_printf ("Scheduler descriptor: %d\n", 
                g_astCpssVkProcClassTbl[usGuid].ulSchedDesc);
            
            cps__oams_shcmd_printf ("\n");
            cps__oams_shcmd_printf ("Procedure descriptor\n");
            cps__oams_shcmd_printf ("--------------------\n");
            for (ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin; ulIndex < 
                (g_astCpssVkProcClassTbl[usGuid].ulBegin + 
                g_astCpssVkProcClassTbl[usGuid].usTotalInst); ulIndex++)
            {
                if (g_astCpssVkProcDescTbl[ulIndex].bInuse == TRUE)
                {
                    usInst = (UINT16) (ulIndex - g_astCpssVkProcClassTbl[usGuid].ulBegin
                        );
                    ulProcDesc = (usGuid << 16) | (usInst);
                    
                    cps__oams_shcmd_printf ("%-20d\n", ulProcDesc);                
                }
            }
            
            break;
        }
        
    case 2 :
        {
            /* 获得纤程信息. */
            
            if (cpss_vk_proc_info_get (ulProcDesc, & tProcInfo) == CPSS_ERROR)
            {
                return (CPSS_ERROR);
            }
            
            cps__oams_shcmd_printf ("\n");
            cps__oams_shcmd_printf ("Name        Procedure descriptor  Procedure ID  Priority  Status\n");
            cps__oams_shcmd_printf ("----        --------------------  ------------  --------  ------\n");
            
            /* 纤程总结. */
            
            cpss_vk_proc_summary (& tProcInfo);
            
            break;
        }
        
    case 3 :
    default :
        {
            /* 获得纤程信息. */
            
            if (cpss_vk_proc_info_get (ulProcDesc, & tProcInfo) == CPSS_ERROR)
            {
                return (CPSS_ERROR);
            }
            
            /* 获得纤程统计. */
            
            if (cpss_vk_proc_stat_get (ulProcDesc, & tProcStat) == CPSS_ERROR)
            {
                return (CPSS_ERROR);
            }
            
            cps__oams_shcmd_printf ("\n");
            cps__oams_shcmd_printf ("     Name            Procedure descriptor  Procedure ID  Priority  Status\n");
            cps__oams_shcmd_printf ("--------------       --------------------  ------------  --------  ------\n");
            
            /* 纤程总结. */
            
            cpss_vk_proc_summary (& tProcInfo);
            
            cps__oams_shcmd_printf ("\n");
            cps__oams_shcmd_printf ("Options: 0x%x\n", tProcInfo.lOptions);
            cps__oams_shcmd_printf ("Stack: base 0x%-8x end 0x%-8x size %-10d \n",
                    (INT32) tProcInfo.pcStackBase, (INT32) tProcInfo.pcStackEnd, 
                    tProcInfo.lStackSize);
            cps__oams_shcmd_printf ("Variable: address 0x%-8x size %-8d \n",
                    (INT32) tProcInfo.pcVar, (INT32) tProcInfo.lVarSize);

#ifdef CPSS_VOS_VXWORKS
            cps__oams_shcmd_printf ("Maximal run-time ticks: %-10d \n", tProcInfo.lMaxTicks);
            cps__oams_shcmd_printf(" ProcNTimeb %-10d\n",tProcInfo.ulProcNTimeb);
            cps__oams_shcmd_printf(" ProcNTimee %-10d\n",tProcInfo.ulProcNTimee);	
	     cps__oams_shcmd_printf(" ProcNTime %-10d\n",tProcInfo.ulProcNTime);
#endif

            cps__oams_shcmd_printf ("Number of procedure called: at intervals %-10d total %-10d\n"
                    , tProcStat.ulInteCallNum, tProcStat.ulTotalCallNum);
            cps__oams_shcmd_printf ("Number of mail sent: at intervals %-10d total %-10d\n"
                    , tProcStat.ulInteMailSent, tProcStat.ulTotalMailSent);
            cps__oams_shcmd_printf ("Number of bytes sent: at intervals %-10d total %-10d\n"
                    , tProcStat.ulInteBytesSent, tProcStat.ulTotalBytesSent);
            cps__oams_shcmd_printf ("Number of mail received: at intervals %-10d total %-10d\n"
                    , tProcStat.ulInteMailRecv, tProcStat.ulTotalMailRecv);
            cps__oams_shcmd_printf ("Number of bytes received: at intervals %-10d total %-10d\n"
                    , tProcStat.ulInteBytesRecv, tProcStat.ulTotalBytesRecv);
            
            break;
        }
    }
    
    return (CPSS_OK);
}
#else
STATUS cpss_vk_proc_show 
(
UINT16  usGuid, /* GUID */
UINT32  ulProcDesc, /* 纤程描述符 */
INT32   lLevel /* 
               * 0 = 纤程类summary, 1 = 纤程类detail, 
               * 2 = 纤程summary, 3 = 纤程detail
               */
)
{
    cps__oams_shcmd_printf ("cpss_vk_sched_show can not support in this board!\n");    
    return CPSS_OK;
}
#endif
/*******************************************************************************
*函数名称:cpss_ipc_msg_add_to_pool
*功能:将IPC消息加入IPC消息池
*函数类型: 
*参数: 
*参数名称    类型            输入/输出 描述
*tPoolId     IPC_MSG_POOL_ID 输入      池标识
*pcPool      INT8 *          输入      池内存
*ulMaxMsgs   UINT32          输入      最大消息数目
*ulMaxMsgLen UINT32          输入      消息最大长度
ulMaxor CPSS_ERROR
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
 INT32 cpss_ipc_msg_add_to_pool 
(
IPC_MSG_POOL_ID tPoolId, /* 池标识 */
INT8 *  pcPool, /* 池内存 */
UINT32  ulMaxMsgs, /* 最大消息数目 */
UINT32    ulMaxMsgLen /* 消息最大长度 */
)
{
    INT32   lNodeSize;
    UINT32  lCount;
    
    /* 节点大小. */
    
    lNodeSize = sizeof (CPSS_VK_IPC_MSG_NODE_T) + sizeof (IPC_MSG_HDR_T) + ulMaxMsgLen;
    
    cpss_vos_sem_p (tPoolId->ulSemDesc, WAIT_FOREVER);
    
    for (lCount = 0; lCount < ulMaxMsgs; lCount++)
    {
    /* 放入FREE队列. */
    
    cpss_util_q_fifo_put (& tPoolId->qFree, (CPSS_UTIL_Q_FIFO_NODE_T *) pcPool, 
                          UTIL_Q_FIFO_KEY_TAIL);
    
    /* 获得下一个消息地址. */
    
    pcPool = pcPool + lNodeSize;
    }
    
    /* 更新统计. */
    
    tPoolId->lMaxMsgs = ulMaxMsgs;
    tPoolId->lMaxMsgLen = ulMaxMsgLen;
    tPoolId->lCurrMsgsAlloc = 0;
    
    cpss_vos_sem_v (tPoolId->ulSemDesc);
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_ipc_msg_pool_init
*功能:IPC消息池初始化
*函数类型: 
*参数: 
*参数名称    类型            输入/输出 描述
*tPoolId     IPC_MSG_POOL_ID 输入      池标识
*pcPool      INT8 *          输入      池内存
*ulMaxMsgs   UINT32          输入      最大消息数目
*ulMaxMsgLen UINT32          输入      消息最大长度
ulMaxor CPSS_ERROR
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
 INT32 cpss_ipc_msg_pool_init 
(
IPC_MSG_POOL_ID tPoolId, /* 池标识 */
INT8 *  pcPool, /* 池内存 */
UINT32  ulMaxMsgs, /* 最大消息数目 */
UINT32    ulMaxMsgLen /* 消息最大长度 */
)
{
    /* 初始化信号量. */
    
    tPoolId->ulSemDesc = cpss_vos_sem_b_create (VOS_SEM_Q_PRI, VOS_SEM_FULL); 
    
    if (tPoolId->ulSemDesc == VOS_SEM_DESC_INVALID)
    {
        return (CPSS_ERROR);
    }
    
    /* 初始化链表. */
    
    cpss_util_q_fifo_init (& tPoolId->qFree);
    cpss_util_q_fifo_init (& tPoolId->qData);
    
    cpss_ipc_msg_add_to_pool (tPoolId, pcPool, ulMaxMsgs, ulMaxMsgLen);
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_ipc_msg_pool_create
*功能:IPC消息池创建
*函数类型: 
*参数: 
*参数名称 类型 输入/输出 描述
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
INT32 cpss_ipc_msg_pool_create (void)
{
    INT8 *  pcPool;
    
    /* 分配内存. */
    
    pcPool = (INT8 *) malloc (((IPC_MAX_MSGS) * (sizeof (CPSS_VK_IPC_MSG_NODE_T) + 
    sizeof (IPC_MSG_HDR_T) + IPC_MAX_MSG_LEN)));
    if (pcPool == NULL)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_cpss_ipc_msg_pool_init: malloc failed.\n"
                     );
    
        return (CPSS_ERROR);
    }
    
    cpss_mem_memset(pcPool,0,((IPC_MAX_MSGS) * (sizeof (CPSS_VK_IPC_MSG_NODE_T) + 
		sizeof (IPC_MSG_HDR_T) + IPC_MAX_MSG_LEN)));
		
    /*IPC消息池的开始地址*/
    g_pucIpcPoolAddrBegin = pcPool ;

    if (cpss_ipc_msg_pool_init (g_stCpssVkIpcMsgPoolId, pcPool, IPC_MAX_MSGS, 
        IPC_MAX_MSG_LEN) == CPSS_ERROR)
    {
        free ((void *) pcPool);
        return (CPSS_ERROR);
    }

    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_ipc_ram_range_check
*功能:
*函数类型: 
*参数: 
*参数名称 类型            输入/输出 描述
*pstNode  IPC_MSG_POOL_ID 输入      IPC的管理对象
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
VOID cpss_ipc_ram_range_check
(
 CPSS_VK_IPC_MSG_NODE_T* pstNode
)
{
    CPSS_COM_MSG_HEAD_T *pstComMsg = NULL ;
    /*
    UINT32 ulBufLen = 0 ;
    */
    
    if(pstNode == NULL)
    {
        return ;
    }

    /*
    ulBufLen = CPSS_IPC_POOL_SIZE ;
    ulBufLen = ulBufLen - CPSS_IPC_MSG_HEAD_LEN ;
    */
    
    /*释放消息前检查IPC消息是否在期望的范围之内*/
    if(((UINT32)pstNode < (UINT32)g_pucIpcPoolAddrBegin)||((UINT32)pstNode > (UINT32)(g_pucIpcPoolAddrBegin+CPSS_IPC_POOL_SIZE-CPSS_IPC_MSG_HEAD_LEN)))
    {
        pstComMsg = (CPSS_COM_MSG_HEAD_T*)IPC_MSG_NODE_TO_HDR(pstNode) ;
        
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "ipc msg out of alloc ram range!\n") ;
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN,"desrAddr=0x%x,destAddrFlag=%d,destPid=0x%x,srcAddr=0x%x,srcAddrFlag=%d,srcPid=0x%x,ulMsgId=0x%x,ulMsgLen=%d,pucBuf=0x%x\n",
        *(UINT32*)&pstComMsg->stDstProc.stLogicAddr,
        pstComMsg->stDstProc.ulAddrFlag,pstComMsg->stDstProc.ulPd,
        *(UINT32*)&pstComMsg->stSrcProc.stLogicAddr,
        pstComMsg->stSrcProc.ulAddrFlag,pstComMsg->stSrcProc.ulPd,
        pstComMsg->ulMsgId,pstComMsg->ulLen,pstComMsg->pucBuf) ;
#ifdef CPSS_VOS_VXWORKS
        taskSuspend(0) ;
#endif
    }

    if(((UINT32)pstNode-(UINT32)g_pucIpcPoolAddrBegin)%CPSS_IPC_MSG_SIZE != 0)
    {
        pstComMsg = (CPSS_COM_MSG_HEAD_T*)IPC_MSG_NODE_TO_HDR(pstNode) ;
        
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN,"ipc msg out of alloc ram range!\n") ;
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN,"desrAddr=0x%x,destAddrFlag=%d,destPid=0x%x,srcAddr=0x%x,srcAddrFlag=%d,srcPid=0x%x,ulMsgId=0x%x,ulMsgLen=%d,pucBuf=0x%x\n",
            *(UINT32*)&pstComMsg->stDstProc.stLogicAddr,
            pstComMsg->stDstProc.ulAddrFlag,pstComMsg->stDstProc.ulPd,
            *(UINT32*)&pstComMsg->stSrcProc.stLogicAddr,
            pstComMsg->stSrcProc.ulAddrFlag,pstComMsg->stSrcProc.ulPd,
            pstComMsg->ulMsgId,pstComMsg->ulLen,pstComMsg->pucBuf) ;
#ifdef CPSS_VOS_VXWORKS
        taskSuspend(0) ;
#endif
    }
}

/*******************************************************************************
*函数名称:cpss_ipc_alloc_err_code
*函数类型: INT32
*参数: 
*参数名称    类型                  输入/输出 描述
* tPoolId   IPC_MSG_POOL_ID        输入      IPC内存
* ptNode    CPSS_VK_IPC_MSG_NODE_T 输入      IPC管理对象
*函数返回:
*说明:                                                          
*******************************************************************************/
extern BOOL g_bCpssResetBoard ;
extern BOOL g_bCpssCloseException ;

INT32 cpss_ipc_alloc_err_code
(
IPC_MSG_POOL_ID tPoolId,
CPSS_VK_IPC_MSG_NODE_T *ptNode
)
{
    CPSS_VK_IPC_MSG_NODE_T* pstPreNode ;
    IPC_MSG_HDR_T* pstPreIpcHdr ;
    CPSS_COM_MSG_HEAD_T* pstDispMsg ;
    
    if((tPoolId->qFree.pstHead->pstNext == NULL)&&(tPoolId->qFree.pstHead->pstPrev == NULL))
    {          
        if(tPoolId->lCurrMsgsAlloc == tPoolId->lMaxMsgs)
        {
           cpss_output(CPSS_MODULE_VK, CPSS_PRINT_WARN,"****the IPC mem pool have been used off\n");
        }
        else
        {
            pstPreNode = (CPSS_VK_IPC_MSG_NODE_T*)((UINT32)ptNode - 
                sizeof (CPSS_VK_IPC_MSG_NODE_T) - sizeof (IPC_MSG_HDR_T) - IPC_MAX_MSG_LEN) ;
            pstPreIpcHdr = (IPC_MSG_HDR_T*)IPC_MSG_NODE_TO_HDR(pstPreNode) ;
            pstDispMsg   = (CPSS_COM_MSG_HEAD_T*)IPC_MSG_HDR_TO_DATA(pstPreIpcHdr) ;
            
            if((UINT32)pstPreNode < (UINT32)g_pucIpcPoolAddrBegin)
            {
              cpss_output(CPSS_MODULE_VK, CPSS_PRINT_WARN,"****the ipc mem have been modified by pre mem=0x%x\n",(UINT32)pstPreNode) ;
            }
            else
            {
                cpss_output(CPSS_MODULE_VK, CPSS_PRINT_WARN,"****the ipc mem have been modified by other app\n") ;
                cpss_output(CPSS_MODULE_VK, CPSS_PRINT_WARN,"****the msgId=0x%x,srcproc=0x%x,dstproc=0x%x,addr=0x%x",
                    pstDispMsg->ulMsgId,pstDispMsg->stSrcProc.ulPd,pstDispMsg->stDstProc.ulPd,(UINT32)pstPreNode) ;
            }
            
#ifdef CPSS_VOS_VXWORKS
#ifndef CPSS_SLAVE_CPU
 #if 0
            g_bCpssResetBoard = 0 ;        
            g_bCpssCloseException = 1 ;
            drv_wd_control(3,0) ;
#endif
#endif
#endif
        }
    }

    return CPSS_OK ;
}

/*******************************************************************************
*函数名称:cpss_vk_proc_info_add
*功能: 增加PROC信息在NODE头中
*函数类型: 
*参数: 
*参数名称    类型   输入/输出 描述
*函数返回:
*说明:                                                         
*******************************************************************************/
#ifdef CPSS_IPC_MSG_COUNTER
VOID cpss_vk_proc_info_add
(
 CPSS_VK_IPC_MSG_NODE_T *  ptNode
)
{
    ptNode->ulTaskId = cpss_vos_task_desc_self();
    ptNode->ulProcId = cpss_vk_pd_self_get();    
}
#endif

/*******************************************************************************
*函数名称:cpss_ipc_msg_pool_alloc
*功能:IPC消息分配
*函数类型: 
*参数: 
*参数名称 类型            输入/输出 描述
*tPoolId  IPC_MSG_POOL_ID 输入      池标识
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                                                  
*******************************************************************************/
 void * cpss_ipc_msg_pool_alloc
(
IPC_MSG_POOL_ID tPoolId /* 池标识 */
)
{
    CPSS_VK_IPC_MSG_NODE_T *  ptNode;

    cpss_vos_sem_p (tPoolId->ulSemDesc, WAIT_FOREVER);
   
    /* 获得一个节点. */
    
    ptNode = (CPSS_VK_IPC_MSG_NODE_T *) cpss_util_q_fifo_get (& tPoolId->qFree);
    if(ptNode == NULL)
    {
        cpss_vos_sem_v (tPoolId->ulSemDesc);
        cpss_output(CPSS_MODULE_VK, CPSS_PRINT_WARN,"ipc pool have no ipc memory!") ;
        return (NULL) ;
    }
    
    /* 设置标志. */
    
    ptNode->lFlags = IPC_MSG_FLAG_IN_USE;
    
    /* 获得头部. */
    
    /* ptHdr = (IPC_MSG_HDR_T *) IPC_MSG_NODE_TO_HDR (ptNode); */
    
    /* 更新统计. */
    #ifdef CPSS_IPC_MSG_COUNTER
    cpss_vk_proc_info_add(ptNode) ;
    #endif
    
    tPoolId->lCurrMsgsAlloc = tPoolId->lCurrMsgsAlloc + 1;

    if(tPoolId->lCurrMsgsAlloc > tPoolId->lPeakMsgNum)
    {
        tPoolId->lPeakMsgNum = tPoolId->lCurrMsgsAlloc ;
    }
    
    /*增加PCI内存被改的定位手段*/
    cpss_ipc_alloc_err_code(tPoolId,ptNode) ;

    /*对释放的IPC消息内存进行校验,如果异常,则挂起任务
    cpss_ipc_ram_range_check(ptNode) ;     
    */
    
    cpss_vos_sem_v (tPoolId->ulSemDesc);
    
    return IPC_MSG_NODE_TO_HDR (ptNode);
}

/*******************************************************************************
*函数名称:cpss_ipc_msg_pool_alloc
*功能:IPC消息分配
*函数类型: 
*参数: 
*参数名称 类型  输入/输出 描述
*lNBytes  INT32 输入      要分配的字节数
*函数返回:消息指针 or NULL
*说明:                                 
*******************************************************************************/
void * cpss_ipc_msg_alloc 
(
INT32   lNBytes /* 要分配的字节数 */
)
{
    CPSS_VK_IPC_MSG_NODE_T *  ptNode;
    
    /* 从IPC消息池分配. */
    
    if (lNBytes <= IPC_MAX_MSG_LEN)
    {
        return (cpss_ipc_msg_pool_alloc (g_stCpssVkIpcMsgPoolId));
    }
    
    else
    {
        /* 从内存池分配. */
        
        ptNode = (CPSS_VK_IPC_MSG_NODE_T *) cpss_mem_malloc (sizeof (CPSS_VK_IPC_MSG_NODE_T) + 
                                                     sizeof (IPC_MSG_HDR_T) + 
                                                     lNBytes);
        if (ptNode == NULL)
        {
            return (NULL);
        }
        
        /* 设置标志. */
        
        ptNode->lFlags = (IPC_MSG_FLAG_MEM_ALLOC | IPC_MSG_FLAG_IN_USE);
        
        return (IPC_MSG_NODE_TO_HDR (ptNode));
    }
}

/*******************************************************************************
*函数名称:cpss_ipc_msg_pool_free
*功能:IPC消息释放
*函数类型: 
*参数: 
*参数名称 类型            输入/输出 描述
*tPoolId  IPC_MSG_POOL_ID 输入      池标识
*ptHdr    IPC_MSG_HDR_T   输入      IPC消息
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
 INT32 cpss_ipc_msg_pool_free
(
IPC_MSG_POOL_ID tPoolId, /* 池标识 */
CPSS_VK_IPC_MSG_NODE_T *  ptNode
)
{
    cpss_vos_sem_p (tPoolId->ulSemDesc, WAIT_FOREVER);
    
    ptNode->lFlags = IPC_MSG_FLAG_IN_FREE ;    
    
    /*对释放的IPC消息内存进行校验,如果异常,则挂起任务*/
/**    
    cpss_ipc_ram_range_check(ptNode) ;
**/    
    
    /* 放入FREE队列. */
    
    cpss_util_q_fifo_put (& tPoolId->qFree, & ptNode->node, 
                          UTIL_Q_FIFO_KEY_TAIL);
    
    /* 更新统计. */
    
    tPoolId->lCurrMsgsAlloc = tPoolId->lCurrMsgsAlloc - 1;
    
    cpss_vos_sem_v (tPoolId->ulSemDesc);
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_ipc_msg_free
*功能:IPC消息释放
*函数类型: 
*参数: 
*参数名称 类型   输入/输出 描述
*ptr      void * 输入      释放对象
*函数返回:
*说明:                                
*******************************************************************************/
void cpss_ipc_msg_free 
(
void *  ptr
)
{
    CPSS_VK_IPC_MSG_NODE_T *  ptNode;
    
    
    /* 指向节点. */
    
    ptNode = (CPSS_VK_IPC_MSG_NODE_T *) IPC_MSG_HDR_TO_NODE (ptr);
    
    
    if ((ptNode->lFlags & IPC_MSG_FLAG_IN_USE) == 0)
    {
        cpss_output(CPSS_MODULE_VK,CPSS_PRINT_FATAL,"reFree the ipc msg memory.") ;

        g_stCpssVkIpcMsgPoolId->lReFreeNum = g_stCpssVkIpcMsgPoolId->lReFreeNum + 1;

    return;
    }
    
    /* 由IPC消息池释放. */
    
    if ((ptNode->lFlags & IPC_MSG_FLAG_MEM_ALLOC) == 0)
    {
        cpss_ipc_msg_pool_free (g_stCpssVkIpcMsgPoolId, ptNode);
    }
    
    /* 由内存池释放. */
    
    else
    {
        cpss_mem_free ((void *) ptNode);
    }
}

/*******************************************************************************
*函数名称:cpss_ipc_msg_pool_expire
*功能:IPC消息过期
*函数类型: 
*参数: 
*参数名称 类型            输入/输出 描述
*tPoolId  IPC_MSG_POOL_ID 输入      池标识
*ptHdr    IPC_MSG_HDR_T   输入      IPC消息
*函数返回:
*说明:                                 
*******************************************************************************/
 INT32 cpss_ipc_msg_pool_expire
(
IPC_MSG_POOL_ID tPoolId, /* 池标识 */
IPC_MSG_HDR_T *   ptHdr
)
{
    CPSS_VK_IPC_MSG_NODE_T *  ptNode;
    
    /* 检查池标识. */
    
    if (tPoolId == NULL)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_ipc_msg_pool_free: message pool ID invalid.\n"
                     );
        
        return (CPSS_ERROR);
    }
    
    /* 兼容性. */
    
    if (ptHdr == NULL)
    {
        return (CPSS_ERROR);
    }
    
    cpss_vos_sem_p (tPoolId->ulSemDesc, WAIT_FOREVER);
    
    /* 指向节点. */
    
    ptNode = (CPSS_VK_IPC_MSG_NODE_T *) IPC_MSG_HDR_TO_NODE (ptHdr);
    
    /* 设置标志. */
    
    ptNode->lFlags |= IPC_MSG_FLAG_EXPIRE;
    
    cpss_vos_sem_v (tPoolId->ulSemDesc);
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_ipc_msg_expire
*功能:IPC消息过期
*函数类型: 
*参数: 
*参数名称 类型   输入/输出 描述
*ptr      void * 输入
*函数返回:
*说明:                                 
*******************************************************************************/
void cpss_ipc_msg_expire 
(
void *  ptr
)
{
    CPSS_VK_IPC_MSG_NODE_T *  ptNode;
    
    /* 检查指针. */
    
    if (ptr == NULL)
    {
    return;
    }
    
    /* 指向节点. */
    
    ptNode = (CPSS_VK_IPC_MSG_NODE_T *) IPC_MSG_HDR_TO_NODE (ptr);
    
    if ((ptNode->lFlags & IPC_MSG_FLAG_IN_USE) == 0)
    {
        return;
    }
    
    cpss_ipc_msg_pool_expire (g_stCpssVkIpcMsgPoolId, (IPC_MSG_HDR_T *) ptr);
}


/*******************************************************************************
*函数名称:cpss_ipc_msg_send
*功能:IPC消息发送
*函数类型: 
*参数: 
*参数名称 类型          输入/输出 描述
*ptHdr    IPC_MSG_HDR_T 输入      指向头部
*lType    INT32         输入      类型
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
INT32 cpss_ipc_msg_send 
(
IPC_MSG_HDR_T *   ptHdr, /* 指向头部 */
INT32   lType /* 类型 */
)
{
    CPSS_COM_MSG_HEAD_T *    ptData;
    UINT32  ulProcDesc;
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    
    /* 检查头部. */
    
    if (ptHdr == NULL)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_ipc_msg_send: message header not specified.\n"
                     ); 
        g_stCpssIpcStat.ulIpcMsgNull = g_stCpssIpcStat.ulIpcMsgNull + 1;
        return (CPSS_ERROR);
    }
    
    /* 指向数据. */
    
    ptData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr);
    #ifdef CPSS_VOS_WINDOWS
    ptHdr->lTimeout = WAIT_FOREVER;
    #endif

    /* 获得纤程描述符. */
    
    ulProcDesc = ptData->stDstProc.ulPd;
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
    (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_ipc_msg_send: globally unique identifier invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        g_stCpssIpcStat.ulGuidOverFlow = g_stCpssIpcStat.ulGuidOverFlow + 1;
        return (CPSS_ERROR);
    }
    
    /* 获得实例. */
#ifdef CPSS_CONFIG_MULTI_INST
    usInst = (UINT16) (ulProcDesc & 0x0000ffff);
    
    /* 检查实例. */
    
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_ipc_msg_send: instance invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        g_stCpssIpcStat.ulInstOverFlow = g_stCpssIpcStat.ulInstOverFlow + 1;
        return (CPSS_ERROR);
    }
#else
     usInst = 0;
#endif
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
    
    /* 检查纤程描述符. */
    
    if (!g_astCpssVkProcDescTbl[ulIndex].bInuse)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_ipc_msg_send: descriptor invalid, descriptor = 0x%08x.\n"
                     , ulProcDesc);
        g_stCpssIpcStat.ulIndexOverFlow = g_stCpssIpcStat.ulIndexOverFlow + 1;
        return (CPSS_ERROR);
    }
    
    cpss_vk_ipc_pri_map(&ptData->ucPriFlag);
    if (cpss_vk_sched_send (g_astCpssVkProcClassTbl[usGuid].ulSchedDesc, (INT8 *) ptHdr, 
        ptHdr->lTimeout, (INT32) ptData->ucPriFlag, lType) == CPSS_ERROR)
	     {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_ipc_msg_send: cpss_vk_sched_send failed.\n"
            );
	        return (CPSS_ERROR);
	     }
    
    /* 更新统计. */

    g_astCpssVkProcStatTbl[ulIndex].ulInteMailSent = g_astCpssVkProcStatTbl[ulIndex].ulInteMailSent + 1;
    /*
    g_astCpssVkProcStatTbl[ulIndex].ulInteBytesSent += ptHdr->totalLen;
    */
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_ipc_msg_receive
*功能:IPC消息接收
*函数类型: 
*参数: 
*参数名称 类型            输入/输出 描述
*ptHdr    IPC_MSG_HDR_T * 输入      指向头部
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
INT32 cpss_ipc_msg_receive 
(
IPC_MSG_HDR_T *   ptHdr /* 指向头部 */
)
{
    CPSS_COM_MSG_HEAD_T *    ptData;
    UINT32  ulProcDesc;
    UINT16  usGuid;
    UINT32  ulIndex;
    VK_PROC_ID  tProcId;
    CPSS_VK_IPC_MSG_NODE_T *  ptNode;
    
    /* 指向数据. */
    
    ptData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr);

#ifdef CPSS_VOS_VXWORKS

    if (ptData->ulMsgId == IPC_MSG_ID_RTP)
        {
	#if (SWP_FUNBRD_ABOX != SWP_FUNBRD_TYPE) 
        smss_verm_patch_operation ((INT8 *) ptData->pucBuf);
    #endif
        cpss_ipc_msg_free ((void *) ptHdr);
       
        return (CPSS_OK);
        }
#endif    

    /* 获得纤程描述符. */
    
    ulProcDesc = ptData->stDstProc.ulPd;
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
    /* 获得实例. */
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + (ulProcDesc & 0x0000ffff);
    
    /* 获得纤程标识. */
    
    tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;
    
    /* 指向节点. */
    
    ptNode = (CPSS_VK_IPC_MSG_NODE_T *) IPC_MSG_HDR_TO_NODE (ptHdr);
    
    if (ptNode->lFlags & IPC_MSG_FLAG_EXPIRE)
    {
        cpss_ipc_msg_free ((void *) ptHdr);
        
        return (CPSS_ERROR);
    }
    
    /* 放入纤程邮箱. */
    
    cpss_util_q_fifo_put (& tProcId->mailBox, & ptNode->node, 
        UTIL_Q_FIFO_KEY_TAIL);
    
    /* 更新纤程邮箱数目. */
    
    tProcId->lMailNum = tProcId->lMailNum + 1;
    
    /* 更新统计. */
    
    g_astCpssVkProcStatTbl[ulIndex].ulInteMailRecv = g_astCpssVkProcStatTbl[ulIndex].ulInteMailRecv + 1;
    /*
    g_astCpssVkProcStatTbl[ulIndex].ulInteBytesRecv += ptHdr->totalLen;
    */
    return cpss_vk_sched_proc_load (g_astCpssVkProcClassTbl[usGuid].ulSchedDesc, tProcId);
}

/*******************************************************************************
*函数名称:cpss_vk_msg_sender_get
*功能:IPC消息发送者获得
*函数类型: 
*参数: 
*参数名称 类型             输入/输出 描述
*ptSender CPSS_COM_PID_T * 输出      发送的PID
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
INT32 cpss_vk_msg_sender_get
(
CPSS_COM_PID_T *    ptSender
)
{
    UINT32  ulSchedDesc;
    VK_SCHED_ID tSchedId;
    IPC_MSG_HDR_T *   ptHdr;
    CPSS_COM_MSG_HEAD_T *    ptData;
    
    /* 检查参数. */
    
    if (ptSender == NULL)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_msg_sender_get: argument invald.\n"
                     );
        
        return (CPSS_ERROR);
    }
    
    ulSchedDesc = cpss_vk_sched_desc_self ();
    if (ulSchedDesc == VK_SCHED_DESC_INVALID)
    {
        return (CPSS_ERROR);
    }
    
    /* 获得调度器标识. */
    
    tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
    
    ptHdr = (IPC_MSG_HDR_T *) tSchedId->pIpcMsgCurr;
    if (ptHdr == NULL)
    {
        return (CPSS_ERROR);
    }
    
    /* 指向数据. */
    
    ptData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr);
    
    /* 获得发送者. */   
    
    cpss_mem_memcpy ((void *) ptSender, (void *) (& ptData->stSrcProc), 
            sizeof (CPSS_COM_PID_T));
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_vk_msg_id_get
*功能:IPC消息标示获得
*函数类型: 
*参数: 
*参数名称 类型     输入/输出 描述
*pulId    UINT32 * 输出
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
INT32 cpss_vk_msg_id_get
(
UINT32 *    pulId
)
{
    UINT32  ulSchedDesc;
    VK_SCHED_ID tSchedId;
    IPC_MSG_HDR_T *   ptHdr;
    CPSS_COM_MSG_HEAD_T *    ptData;
    
    /* 检查参数. */
    
    if (pulId == NULL)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_msg_id_get: argument invald.\n"
                     );
        
        return (CPSS_ERROR);
    }
    
    ulSchedDesc = cpss_vk_sched_desc_self ();
    if (ulSchedDesc == VK_SCHED_DESC_INVALID)
    {
        return (CPSS_ERROR);
    }
    
    /* 获得调度器标识. */
    
    tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
    
    ptHdr = (IPC_MSG_HDR_T *) tSchedId->pIpcMsgCurr;
    if (ptHdr == NULL)
    {
        return (CPSS_ERROR);
    }
    
    /* 指向数据. */
    
    ptData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr);
    *pulId = ptData->ulMsgId;
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_vk_proc_name_get_by_pid
*功能: 根据纤程描述符获取纤程名称
*函数类型: 
*参数: 
*参数名称 类型             输入/输出 描述
*ulPID    UINT32           输出      ulPID
*函数返回:
    成功：纤程名称字符串；
    失败：NULL
*说明:
*    设计该函数主要是SMSS在纤程激活配置表内没有纤程名称，只有纤程描述符。而为了
*管理或SHOW方便，需要得到纤程名称，因此增加本函数。                          
*******************************************************************************/
STRING cpss_vk_proc_name_get_by_pid(UINT32 ulPID)
{
    UINT16 usGuid = CPSS_VK_PTYPE(ulPID);

    if ((usGuid == 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM))
    {
        return (NULL);
    }

    return (g_astCpssVkProcClassTbl[usGuid].acName);
    
}


/*******************************************************************************
*函数名称:cpss_vk_proc_pid_get_by_name
*功能: 根据纤程名称获取纤程描述符
*函数类型: 
*参数: 
*参数名称 类型             输入/输出 描述
*szProcName    STRING           输出
*函数返回:
    成功：纤程描述符；
    失败：-1
*说明:
*******************************************************************************/
UINT32 cpss_vk_proc_pid_get_by_name(STRING szProcName)
{	
	UINT32 ulIndex;
	UINT16 usGuid;
	
  if (szProcName == NULL)
  {
    return -1;	
  }	
  
  for (ulIndex = 0; ulIndex < VK_MAX_PROC_CLASS_NUM; ulIndex++)
  {
     if (strcmp(g_astCpssVkProcClassTbl[ulIndex].acName, szProcName) == 0)
     {
     	  usGuid = g_astCpssVkProcClassTbl[ulIndex].usGuid;
     	  return CPSS_VK_PD(usGuid, 0);
     }	
  }
    
  return -1;  
}



/*******************************************************************************
*函数名称:cpss_vk_ipc_msg_num_get
*功能:得到空闲IPC消息的个数
*函数类型: 
*参数: 
*参数名称 类型     输入/输出 描述
*说明:                                 
*******************************************************************************/
INT32 cpss_vk_ipc_msg_num_get()
{
    return (g_stCpssVkIpcMsgPoolId->lMaxMsgs - g_stCpssVkIpcMsgPoolId->lCurrMsgsAlloc) ;
}

VOID cpss_vk_dir_copy_stat(VOID)
{
  
			CPSS_VK_PROC_PCB_T     *pstPCB;
 
                    UINT32    ulPno;  
    
                   /* 获得该进程的PCB */    
                   ulPno = cpss_vk_pd_self_get(); 

		      if(CPSS_VK_PD_INVALID == ulPno) return;

                   pstPCB = cpss_vk_proc_pcb_get(ulPno);

		      if(pstPCB ==NULL) return;		   

			pstPCB->ulDirCopyFiles++;
}

VOID cpss_vk_clr_dir_copy_stat(VOID)
{

		      CPSS_VK_PROC_PCB_T     *pstPCB;
 
                    UINT32    ulPno;  
    
                   /* 获得该进程的PCB */    
                   ulPno = cpss_vk_pd_self_get(); 

			if(CPSS_VK_PD_INVALID == ulPno) return;

                   pstPCB = cpss_vk_proc_pcb_get(ulPno);

                   if(pstPCB ==NULL) return;	
					 
			pstPCB->ulDirCopyFiles = 0;
}

UINT32 cpss_vk_get_dir_copy_stat(VOID)
{

	
			 CPSS_VK_PROC_PCB_T     *pstPCB;
 
                    UINT32    ulPno;  
    
                   /* 获得该进程的PCB */    
                   ulPno = cpss_vk_pd_self_get(); 

			 if(CPSS_VK_PD_INVALID == ulPno) return 0;

                   pstPCB = cpss_vk_proc_pcb_get(ulPno);

                     if(pstPCB ==NULL) return 0;					 
				   
		       return pstPCB->ulDirCopyFiles ;
}

/*******************************************************************************
*函数名称:cpss_vk_ipc_msg_show
*功能:IPC消息统计打印
*函数类型: 
*参数: 
*参数名称 类型     输入/输出 描述
*说明:                                 
*******************************************************************************/
VOID cpss_vk_ipc_msg_show()
{
    UINT32 ulFreeQueueNum = 0 ;
    UINT32 ulBusyQueueNum = 0 ;
    CPSS_UTIL_Q_FIFO_HEAD_T stFreeQ ;
    CPSS_UTIL_Q_FIFO_HEAD_T stBusyQ ;
    struct util_dl_node *pstFreeNode ;
    struct util_dl_node *pstBusyNode ;
    stFreeQ = g_stCpssVkIpcMsgPoolId->qFree ;
    stBusyQ = g_stCpssVkIpcMsgPoolId->qData ;

    pstFreeNode = stFreeQ.pstHead ;
    pstBusyNode = stBusyQ.pstHead ;

    /*计算free队列长度*/
    while(pstFreeNode!=NULL)
    {
        ulFreeQueueNum = ulFreeQueueNum + 1 ;
        pstFreeNode = pstFreeNode->pstNext ;
    }

    /*计算busy队列长度*/
    while(pstBusyNode!=NULL)
    {
        ulBusyQueueNum = ulBusyQueueNum + 1;
        pstBusyNode = pstBusyNode->pstNext ;
    }
        
    cps__oams_shcmd_printf("\n") ;
    cps__oams_shcmd_printf("lCurrMsgsAlloc=%d\n",g_stCpssVkIpcMsgPoolId->lCurrMsgsAlloc) ;
    cps__oams_shcmd_printf("lPeakMsgNum=%d\n",g_stCpssVkIpcMsgPoolId->lPeakMsgNum) ;   
    cps__oams_shcmd_printf("lMaxMsgs=%d\n",g_stCpssVkIpcMsgPoolId->lMaxMsgs) ;
    cps__oams_shcmd_printf("lReFreeNum=%d\n",g_stCpssVkIpcMsgPoolId->lReFreeNum) ;   
    cps__oams_shcmd_printf("ulBusyQueueNum=%d\n",ulBusyQueueNum) ;
    cps__oams_shcmd_printf("ulFreeQueueNum=%d\n",ulFreeQueueNum) ;
    cps__oams_shcmd_printf("\n") ;
}
#ifdef CPSS_IPC_MSG_COUNTER
typedef struct
{
    UINT32 ultaskId;
    UINT32 ulprocId;
    UINT32 ulNum;
}CPSS_VK_IPC_INFO_T;

void cpss_vk_ipc_info_show()
{
    CPSS_VK_IPC_INFO_T  VkIpcInfo[256];    
    struct util_dl_node *pstFreeNode = NULL;
    struct util_dl_node *pstBusyNode = NULL;
    CPSS_VK_IPC_MSG_NODE_T *  ptNode = NULL;
    IPC_MSG_HDR_T *   ptHdr = NULL;
	UINT32 ulSum = 0;
    UINT32 i;
	
    CPSS_VK_IPC_MSG_NODE_T *pstNode;
    pstNode = (CPSS_VK_IPC_MSG_NODE_T*)g_pucIpcPoolAddrBegin;
    
    memset(VkIpcInfo,0,sizeof(CPSS_VK_IPC_INFO_T)*256);
    
    while((UINT32)pstNode < ((UINT32)(g_pucIpcPoolAddrBegin+CPSS_IPC_POOL_SIZE-CPSS_IPC_MSG_HEAD_LEN)))
    {
        if (pstNode->lFlags&IPC_MSG_FLAG_IN_USE)
        {
            for( i = 0; i < ulSum; i++)
            {
                if((VkIpcInfo[i].ultaskId == pstNode->ulTaskId)&&(VkIpcInfo[i].ulprocId == pstNode->ulProcId))
                {
                    VkIpcInfo[i].ulNum++;
                    break;
                }
			}
			if((i >= ulSum) && (ulSum < 256))
			{
				VkIpcInfo[ulSum].ultaskId = pstNode->ulTaskId;
				VkIpcInfo[ulSum].ulprocId = pstNode->ulProcId;
				VkIpcInfo[ulSum].ulNum = 1;
				ulSum++;
			}
        }
        pstNode = (CPSS_VK_IPC_MSG_NODE_T *)((UINT8*)pstNode+CPSS_IPC_POOL_SIZE/IPC_MAX_MSGS);
    }
	printf("IPC MSG USED INFO:\n");
    for( i = 0; i < ulSum; i++)
    {
        if(VkIpcInfo[i].ulNum != 0)
        {
            printf("TaskId :%15s,ProcId :%15s,UsedNum: %d\n",cpss_vos_task_name_get_by_name(VkIpcInfo[i].ultaskId),cpss_vk_proc_name_get_by_pid(VkIpcInfo[i].ulprocId),
				VkIpcInfo[i].ulNum);
        }
    }
}
#endif
/*******************************************************************************
*函数名称:cpss_vk_ipc_send_error_show
*功能:Ipc消息发送失败的统计
*函数类型: 
*参数: 
*参数名称 类型     输入/输出 描述
*说明:                                 
*******************************************************************************/
VOID cpss_vk_ipc_send_error_show()
{
    cps__oams_shcmd_printf("ulIpcMsgNull:         %5d\n",g_stCpssIpcStat.ulIpcMsgNull);
    cps__oams_shcmd_printf("ulGuidOverFlow:       %5d\n",g_stCpssIpcStat.ulGuidOverFlow);
    cps__oams_shcmd_printf("ulInstOverFlow:       %5d\n",g_stCpssIpcStat.ulInstOverFlow);
    cps__oams_shcmd_printf("ulIndexOverFlow:      %5d\n",g_stCpssIpcStat.ulIndexOverFlow);
    cps__oams_shcmd_printf("ulSchedDescOverFlow:  %5d\n",g_stCpssIpcStat.ulSchedDescOverFlow);
    cps__oams_shcmd_printf("ulMsgQSendError:      %5d\n",g_stCpssIpcStat.ulMsgQSendError);
}

/*******************************************************************************
*函数名称:cpss_need_patch_operation
*功能:补丁操作
*函数类型: 
*参数: 
*参数名称    类型   输入/输出 描述
*pucTaskName INT8 * 输入
*函数返回:
*说明:                                 
*******************************************************************************/
INT32 cpss_need_patch_operation
    (
    INT8 *  pucTaskName
    )
    {
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
    UINT32  ulSchedDesc;
    IPC_MSG_HDR_T *    ptHdr;
    CPSS_COM_MSG_HEAD_T *    ptData;
    
    if (pucTaskName == NULL)
        {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_need_patch_operation: name not specified.\n"
                     ); 
        return (CPSS_ERROR);
        }
        
    ulSchedDesc = cpss_vk_sched_desc_get_by_name (pucTaskName);
    if (ulSchedDesc == VK_SCHED_DESC_INVALID)
        {
        return (CPSS_ERROR);
        }
    
    /* 分配IPC消息. */
    
    ptHdr = (IPC_MSG_HDR_T *) cpss_ipc_msg_alloc (IPC_MAX_MSG_LEN + 1);
    if (ptHdr == NULL)
        {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_need_patch_operation: cpss_ipc_msg_alloc failed.\n"
                     ); 
        return (CPSS_ERROR);
        }
    
    /* 清除头部. */
    
    memset ((void *) ptHdr, 0, sizeof (IPC_MSG_HDR_T));
    
    /* 设置头部. */
    
    ptHdr->ptNext = NULL;
    ptHdr->lTimeout = WAIT_FOREVER;
    /*
    ptHdr->totalLen = sizeof (IPC_MSG_HDR_T) + sizeof (CPSS_COM_MSG_HEAD_T);
    */
    /* 指向数据. */
    
    ptData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr);
    
    /* 清除数据. */
    
    memset ((void *) ptData, 0, sizeof (CPSS_COM_MSG_HEAD_T));
    
    ptData->ulMsgId = IPC_MSG_ID_RTP;
    ptData->ulLen = strlen (pucTaskName);
    ptData->pucBuf = ((UINT8 *) ptData) + sizeof (CPSS_COM_MSG_HEAD_T);
    memcpy ((void *) ptData->pucBuf, (void *)pucTaskName, ptData->ulLen);
    
    *(ptData->pucBuf+ptData->ulLen) = '\0' ;
    
    ptData->ucPriFlag = VOS_MSG_PRI_NORMAL;
    
    /* 发送. */
    
    cpss_vk_ipc_pri_map (& ptData->ucPriFlag);
    if (cpss_vk_sched_send (ulSchedDesc, (INT8 *) ptHdr, ptHdr->lTimeout, 
        (INT32) ptData->ucPriFlag, IPC_MSG_TYPE_REMOTE) == CPSS_ERROR)
        {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_need_patch_operation: cpss_vk_sched_send failed.\n"
                     );

        cpss_ipc_msg_free ((void *) ptHdr);

        return (CPSS_ERROR);
        }
#endif
    return (CPSS_OK);
}

/******************************* 源文件结束 **********************************/

