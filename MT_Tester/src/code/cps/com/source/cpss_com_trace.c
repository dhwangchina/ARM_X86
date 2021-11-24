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
#include "cpss_com_common.h"
#include "cpss_com_trace.h"
#include "cpss_com_link.h"
#include "cpss_err.h"

/******************************* 局部宏定义 ***********************************/

/******************************* 全局变量定义/初始化 **************************/
UINT32 g_ulCpssComTraceTaskNum = 0;  /* 消息跟踪任务计数 */

/******************************* 局部常数和类型定义 ***************************/
static CPSS_COM_TRACE_TASK_T *g_pstCpssComTraceHead = NULL; /* 消息跟踪任务信息链表头指针 */

CPSS_COM_TRACE_MANAGE_T g_stCpssComTraceMan = {0} ;
/******************************* 局部函数原型声明 *****************************/
static INT32 cpss_com_trace_dstsrcpid_filter
(
 CPSS_COM_TRACE_FILTER_PID_T *pstPidFilter,
 CPSS_COM_PID_T *pstProc
 );

static INT32 cpss_com_trace_judge
( 
 CPSS_COM_TRACE_TASK_T *pstTraceNode, 
 CPSS_COM_MSG_HEAD_T *pstMsgHead 
 );
/******************************* 函数实现 *************************************/

/*******************************************************************************
* 函数名称: cpss_com_trace_init
* 功    能: 消息跟踪部分初始化 
* 函数类型: INT32
* 参    数: 无。
* 参数名称            类型        输入/输出         描述
* 
* 函数返回: 消息跟踪成功初始化返回CPSS_OK，否则返回CPSS_ERROR。
* 说    明: 该函数只被初始化函数调用。
*******************************************************************************/
INT32 cpss_com_trace_init()
{    
    /* 初始化消息跟踪任务头指针 */
    g_pstCpssComTraceHead = NULL;

    cpss_mem_memset(&g_stCpssComTraceMan,0,sizeof(g_stCpssComTraceMan));
    /*申请TRACE缓冲区内存*/
    g_stCpssComTraceMan.pucTraceBufHdr = cpss_mem_malloc(CPSS_COM_TRACE_BUF_SIZE) ;
    if(NULL == g_stCpssComTraceMan.pucTraceBufHdr)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
            "cpss_com_trace: malloc memory failed.\n");
        return CPSS_ERROR ;
    }

    g_stCpssComTraceMan.ulBufPositionHdr = 0 ;

    g_stCpssComTraceMan.ulTurnPositon = CPSS_COM_TRACE_TURN_POS_INVALID ;

    /*设置默认上报速率*/
    g_stCpssComTraceMan.ulTraceToken = CPSS_COM_TRACE_REATE_DEFAULT ;
    g_stCpssComTraceMan.ulTraceTokenValid = CPSS_COM_TRACE_REATE_DEFAULT ;

    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_com_trace_add_msg_hton
* 功    能: 
* 函数类型: 
* 参    数: 
* 参数名称       类型                      输入/输出   描述
* 函数返回: 
* 说    明: 
*******************************************************************************/
VOID cpss_com_trace_add_msg_hton(CPSS_COM_TRACE_TASK_ADD_MSG_T *pstAddMsg)
{
    if(pstAddMsg == NULL)
    {
        return ;
    }

    pstAddMsg->stLdtPid.stLogicAddr.usGroup = cpss_htons(pstAddMsg->stLdtPid.stLogicAddr.usGroup) ;
    pstAddMsg->stLdtPid.ulAddrFlag = cpss_htonl(pstAddMsg->stLdtPid.ulAddrFlag) ;
    pstAddMsg->stLdtPid.ulPd       = cpss_htonl(pstAddMsg->stLdtPid.ulPd) ;
    
    pstAddMsg->ulTraceId           = cpss_htonl(pstAddMsg->ulTraceId) ;
    
    pstAddMsg->stComTraceFilter.enAssPtChoice = cpss_htonl(pstAddMsg->stComTraceFilter.enAssPtChoice) ;
    pstAddMsg->stComTraceFilter.enMsgDirFlag  = cpss_htonl(pstAddMsg->stComTraceFilter.enMsgDirFlag) ;
    pstAddMsg->stComTraceFilter.stAssLogicAddr.usGroup = cpss_htons(pstAddMsg->stComTraceFilter.stAssLogicAddr.usGroup) ;
    pstAddMsg->stComTraceFilter.ulAssPtPdMax = cpss_htonl(pstAddMsg->stComTraceFilter.ulAssPtPdMax) ;
    pstAddMsg->stComTraceFilter.ulAssPtPdMin = cpss_htonl(pstAddMsg->stComTraceFilter.ulAssPtPdMin) ;
    pstAddMsg->stComTraceFilter.ulMsgIdMax   = cpss_htonl(pstAddMsg->stComTraceFilter.ulMsgIdMax) ;
    pstAddMsg->stComTraceFilter.ulMsgIdMin   = cpss_htonl(pstAddMsg->stComTraceFilter.ulMsgIdMin) ;
    pstAddMsg->stComTraceFilter.ulTrcPtPdMax = cpss_htonl(pstAddMsg->stComTraceFilter.ulTrcPtPdMax) ;
    pstAddMsg->stComTraceFilter.ulTrcPtPdMin = cpss_htonl(pstAddMsg->stComTraceFilter.ulTrcPtPdMin) ;

}

/*******************************************************************************
* 函数名称: cpss_com_trace_add_msg_ntoh
* 功    能: 
* 函数类型: 
* 参    数: 
* 参数名称       类型                      输入/输出   描述
* 函数返回: 
* 说    明: 
*******************************************************************************/
VOID cpss_com_trace_add_msg_ntoh(CPSS_COM_TRACE_TASK_ADD_MSG_T *pstAddMsg)
{
    if(pstAddMsg == NULL)
    {
        return ;
    }
    
    pstAddMsg->stLdtPid.stLogicAddr.usGroup = cpss_ntohs(pstAddMsg->stLdtPid.stLogicAddr.usGroup) ;
    pstAddMsg->stLdtPid.ulAddrFlag = cpss_ntohl(pstAddMsg->stLdtPid.ulAddrFlag) ;
    pstAddMsg->stLdtPid.ulPd       = cpss_ntohl(pstAddMsg->stLdtPid.ulPd) ;
    
    pstAddMsg->ulTraceId           = cpss_ntohl(pstAddMsg->ulTraceId) ;
    
    pstAddMsg->stComTraceFilter.enAssPtChoice = cpss_ntohl(pstAddMsg->stComTraceFilter.enAssPtChoice) ;
    pstAddMsg->stComTraceFilter.enMsgDirFlag  = cpss_ntohl(pstAddMsg->stComTraceFilter.enMsgDirFlag) ;
    pstAddMsg->stComTraceFilter.stAssLogicAddr.usGroup = cpss_ntohs(pstAddMsg->stComTraceFilter.stAssLogicAddr.usGroup) ;
    pstAddMsg->stComTraceFilter.ulAssPtPdMax = cpss_ntohl(pstAddMsg->stComTraceFilter.ulAssPtPdMax) ;
    pstAddMsg->stComTraceFilter.ulAssPtPdMin = cpss_ntohl(pstAddMsg->stComTraceFilter.ulAssPtPdMin) ;
    pstAddMsg->stComTraceFilter.ulMsgIdMax   = cpss_ntohl(pstAddMsg->stComTraceFilter.ulMsgIdMax) ;
    pstAddMsg->stComTraceFilter.ulMsgIdMin   = cpss_ntohl(pstAddMsg->stComTraceFilter.ulMsgIdMin) ;
    pstAddMsg->stComTraceFilter.ulTrcPtPdMax = cpss_ntohl(pstAddMsg->stComTraceFilter.ulTrcPtPdMax) ;
    pstAddMsg->stComTraceFilter.ulTrcPtPdMin = cpss_ntohl(pstAddMsg->stComTraceFilter.ulTrcPtPdMin) ;    
}

/*******************************************************************************
* 函数名称: cpss_com_trace_start_dsp
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称       类型                      输入/输出   描述
* ulTraceID      UINT32                    输入        跟踪标识号,由OAMS提供
* pstTraceFilter CPSS_COM_TRACE_FILTER_T * 输入        消息的跟踪过滤条件
* pstLdtPid      CPSS_COM_PID_T *          输入        LDT消息跟踪窗口对应的PID
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 在消息收发过程中，如果该消息满足跟踪条件，则该消息被以线程间通讯的
*           方式上报给LDT。消息跟踪上报的内容是CPSS_COM_TRACE_HEAD_T，后面有多
*           条跟踪项。每一个跟踪项的内容是CPSS_COM_TRACE_ENTRY_T，之后附加被跟
*           踪消息的消息头（CPSS_COM_MSG_HEAD_T）和消息体（部分或全部）。
*******************************************************************************/
INT32 cpss_com_trace_start_dsp
(
 UINT8 ucDspNo,
 UINT32 ulTraceID,
 CPSS_COM_TRACE_FILTER_T *pstTraceFilter,
 CPSS_COM_PID_T *pstLdtPid
)
{
    CPSS_COM_LOGIC_ADDR_T stDspLogAddr ;
    CPSS_COM_TRACE_TASK_ADD_MSG_T stTraceInfo ;
    CPSS_COM_PID_T stDspPid ;
    INT32 lRet ;

    if((pstTraceFilter == NULL)||(pstLdtPid == NULL))
    {
        return CPSS_ERROR ;
    }

    stTraceInfo.stLdtPid  = *pstLdtPid ;
    stTraceInfo.ulTraceId = ulTraceID ;
    stTraceInfo.stComTraceFilter = *pstTraceFilter ;
    
    /*得到DSP的逻辑地址*/
    cpss_com_dsp_logic_addr_get(ucDspNo,&stDspLogAddr) ;

    /*调用发送接口发送数据*/
    stDspPid.stLogicAddr = stDspLogAddr ;
    stDspPid.ulAddrFlag  = CPSS_COM_ADDRFLAG_MASTER ;
    stDspPid.ulPd        = CPSS_DBG_MNGR_PROC ;

    /*转化发送消息的字节序*/
    cpss_com_trace_add_msg_hton(&stTraceInfo) ;
    lRet = cpss_com_send(&stDspPid,CPSS_TRACE_TASK_ADD_MSG,
        (UINT8*)&stTraceInfo,sizeof(CPSS_COM_TRACE_TASK_ADD_MSG_T)) ;
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "send trace add msg to debug proc failed.destAddr=0x%x,ulAddrFlag=%d,ulPid=0x%x\n",
            *(UINT32*)&stDspPid.stLogicAddr,stDspPid.ulAddrFlag,stDspPid.ulPd);        
        return CPSS_ERROR ;
    }
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_trace_start
* 功    能: 该函数接口完成消息跟踪的条件设置并启动跟踪。
*        
* 函数类型: 
* 参    数: 
* 参数名称       类型                      输入/输出   描述
* ulTraceID      UINT32                    输入        跟踪标识号,由OAMS提供
* pstTraceFilter CPSS_COM_TRACE_FILTER_T * 输入        消息的跟踪过滤条件
* pstLdtPid      CPSS_COM_PID_T *          输入        LDT消息跟踪窗口对应的PID
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 在消息收发过程中，如果该消息满足跟踪条件，则该消息被以线程间通讯的
*           方式上报给LDT。消息跟踪上报的内容是CPSS_COM_TRACE_HEAD_T，后面有多
*           条跟踪项。每一个跟踪项的内容是CPSS_COM_TRACE_ENTRY_T，之后附加被跟
*           踪消息的消息头（CPSS_COM_MSG_HEAD_T）和消息体（部分或全部）。
*******************************************************************************/
INT32 cpss_com_trace_start
(
 UINT32 ulTraceID,
 CPSS_COM_TRACE_FILTER_T *pstTraceFilter,
 CPSS_COM_PID_T *pstLdtPid
)
{
    INT32 lRet ;
    CPSS_COM_PID_T stDstPid ;
    CPSS_COM_TRACE_TASK_ADD_MSG_T stTaskAdd = {0} ;

    /*得到目的纤程地址*/
    lRet = cpss_com_logic_addr_get(&stDstPid.stLogicAddr,&stDstPid.ulPd) ;
    if(CPSS_OK != lRet)
    {
        return lRet ;
    }
    stDstPid.ulPd = CPSS_DBG_MNGR_PROC ;

    stTaskAdd.stComTraceFilter = *pstTraceFilter ;
    stTaskAdd.stLdtPid = *pstLdtPid ;
    stTaskAdd.ulTraceId = ulTraceID ;

    /*发送消息给dbg纤程*/
    lRet = cpss_com_send(&stDstPid,CPSS_TRACE_TASK_ADD_MSG,
        (UINT8*)&stTaskAdd,sizeof(CPSS_COM_TRACE_TASK_ADD_MSG_T)) ;
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "send trace msg to debug proc failed.destAddr=0x%x,ulAddrFlag=%d,ulPid=0x%x\n",
            *(UINT32*)&stDstPid.stLogicAddr,stDstPid.ulAddrFlag,stDstPid.ulPd);
        return lRet ;
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_trace_recv_addtask_deal
* 功    能: 该函数接口完成消息跟踪的条件设置并启动跟踪。
*        
* 函数类型: 
* 参    数: 
* 参数名称       类型                      输入/输出   描述
* ulTraceID      UINT32                    输入        跟踪标识号,由OAMS提供
* pstTraceFilter CPSS_COM_TRACE_FILTER_T * 输入        消息的跟踪过滤条件
* pstLdtPid      CPSS_COM_PID_T *          输入        LDT消息跟踪窗口对应的PID
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 在消息收发过程中，如果该消息满足跟踪条件，则该消息被以线程间通讯的
*           方式上报给LDT。消息跟踪上报的内容是CPSS_COM_TRACE_HEAD_T，后面有多
*           条跟踪项。每一个跟踪项的内容是CPSS_COM_TRACE_ENTRY_T，之后附加被跟
*           踪消息的消息头（CPSS_COM_MSG_HEAD_T）和消息体（部分或全部）。
*******************************************************************************/
INT32 cpss_com_trace_recv_addtask_deal(UINT8* pucRecvBuf,UINT32* pulFirstTask)
{
    UINT32 ulTraceID ;
    CPSS_COM_TRACE_FILTER_T *pstTraceFilter ;
    CPSS_COM_PID_T *pstLdtPid ;   
    CPSS_COM_TRACE_TASK_T *pstTraceNode, *pstTraceTail;
    CPSS_COM_TRACE_TASK_ADD_MSG_T *pstTaskAddMsg ;
    
    pstTaskAddMsg = (CPSS_COM_TRACE_TASK_ADD_MSG_T*)pucRecvBuf ;

#ifndef CPSS_VOS_WINDOWS
#ifdef CPSS_DSP_CPU
    /*转化字节序为本机字节序*/
    cpss_com_trace_add_msg_ntoh(pstTaskAddMsg) ;
#endif
#endif
    
    ulTraceID = pstTaskAddMsg->ulTraceId ;
    pstLdtPid = &pstTaskAddMsg->stLdtPid ;
    pstTraceFilter = &pstTaskAddMsg->stComTraceFilter ;

    /* 判断跟踪任务是否已达到最大 */
    if ((g_ulCpssComTraceTaskNum + 1 > CPSS_COM_TRACE_TASK_MAX_NUM) 
        || (pstTraceFilter == NULL) 
        || (pstLdtPid == NULL))
    {
        return (CPSS_ERROR);
    }
    /* 判断任务号是否已经启动 */
    for (pstTraceNode = g_pstCpssComTraceHead; pstTraceNode != NULL; pstTraceNode = pstTraceNode->pstNext)
    {
        if (pstTraceNode->ulTraceID == ulTraceID)/* 比较每个节点的ulTraceID */
        {
            return (CPSS_ERROR);
        }
    }
    
    /* 申请跟踪任务节点 */
    pstTraceNode = (CPSS_COM_TRACE_TASK_T *)cpss_mem_malloc(sizeof(CPSS_COM_TRACE_TASK_T));
    if (pstTraceNode == NULL)
    {
        return (CPSS_ERROR);
    }
    
    /* 填入相应的链表节点信息 */
    /* 填入对应的SRC和DST */
    cpss_mem_memcpy((UINT8*)&pstTraceNode->stTraceFilterInfo,(UINT8*)pstTraceFilter,sizeof(CPSS_COM_TRACE_FILTER_T));
    /* 填入LDT消息跟踪窗口对应的PID */
    cpss_mem_memcpy((UINT8*)&pstTraceNode->stLdtPid,(UINT8*)pstLdtPid,sizeof(CPSS_COM_PID_T));
    /* 填入其它信息 */
    pstTraceNode->ulTraceID = ulTraceID;     /* 消息跟踪任务号 */
    pstTraceNode->ulSerialNo = 0;            /* 初始化跟踪消息流水号 */
    pstTraceNode->ulTraceLen = CPSS_TRACE_PACKET_LEN;/* 初始设为最大，等于打包的大小 */        
    pstTraceNode->pstNext = NULL;            /* 下一节点为空 */
    
    /* 增加第1个跟踪任务 */
    if (g_pstCpssComTraceHead == NULL)
    {
        g_pstCpssComTraceHead = pstTraceNode;
    }
    /* 增加到已有跟踪任务链表尾 */
    else
    {
        /*找到链表尾*/
        for (pstTraceTail = g_pstCpssComTraceHead; 
        pstTraceTail->pstNext != NULL; 
        pstTraceTail = pstTraceTail->pstNext)/* 找到链表尾 */
        {
        }             
        /*在链表尾加入节点*/
        pstTraceTail->pstNext = pstTraceNode;
    }            
    g_ulCpssComTraceTaskNum = g_ulCpssComTraceTaskNum + 1;  /* 确定任务成功加入后将计数加1 */
    
    if(g_ulCpssComTraceTaskNum == 1)
    {
        *pulFirstTask = 1 ;
    }
    
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_com_trace_rate_set_dsp
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称      类型         输入/输出         描述
* ulTraceID     UINT32       输入              消息跟踪任务号，由OAMS提供
* ulRate        UINT32       输入              上报速率选项，单位：条/秒
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*******************************************************************************/
INT32 cpss_com_trace_rate_set_dsp
(
 UINT8  ucDspNo,
 UINT32 ulTraceID,
 UINT32 ulRate
)
{
    CPSS_COM_LOGIC_ADDR_T stDspLogAddr ;
    CPSS_COM_TRACE_RATE_SET_MSG_T stTraceRateSetInfo ;
    CPSS_COM_PID_T stDspPid ;
    INT32 lRet ;

    stTraceRateSetInfo.ulRate = cpss_htonl(ulRate) ;
    stTraceRateSetInfo.ulTraceID = cpss_htonl(ulTraceID) ;

    /*得到DSP的逻辑地址*/
    cpss_com_dsp_logic_addr_get(ucDspNo,&stDspLogAddr) ;
    
    /*调用发送接口发送数据*/
    stDspPid.stLogicAddr = stDspLogAddr ;
    stDspPid.ulAddrFlag  = CPSS_COM_ADDRFLAG_MASTER ;
    stDspPid.ulPd        = CPSS_DBG_MNGR_PROC ;
    lRet = cpss_com_send(&stDspPid,CPSS_TRACE_TASK_RATE_SET_MSG,
        (UINT8*)&stTraceRateSetInfo,sizeof(CPSS_COM_TRACE_RATE_SET_MSG_T)) ;
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "send trace rate set msg to debug proc failed.destAddr=0x%x,ulAddrFlag=%d,ulPid=0x%x\n",
            *(UINT32*)&stDspPid.stLogicAddr,stDspPid.ulAddrFlag,stDspPid.ulPd);                
        return CPSS_ERROR ;
    }
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_trace_rate_set
* 功    能: 该函数接口完成对指定任务号的消息跟踪设置上报速率限制
*        
* 函数类型: 
* 参    数: 
* 参数名称      类型         输入/输出         描述
* ulTraceID     UINT32       输入              消息跟踪任务号，由OAMS提供
* ulRate        UINT32       输入              上报速率选项，单位：条/秒
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*******************************************************************************/
INT32 cpss_com_trace_rate_set
(
 UINT32 ulTraceID,
 UINT32 ulRate
 )
{
    INT32 lRet ;
    CPSS_COM_PID_T stDstPid ;    
    CPSS_COM_TRACE_RATE_SET_MSG_T stTraceRateSetMsg = {0};
    
    stTraceRateSetMsg.ulRate = ulRate ;
    stTraceRateSetMsg.ulTraceID = ulTraceID ;

    /*得到目的纤程地址*/
    lRet = cpss_com_logic_addr_get(&stDstPid.stLogicAddr,&stDstPid.ulPd) ;
    if(CPSS_OK != lRet)
    {
        return lRet ;
    }
    stDstPid.ulPd = CPSS_DBG_MNGR_PROC ;

    /*发送消息给dbg纤程*/
    lRet = cpss_com_send(&stDstPid,CPSS_TRACE_TASK_RATE_SET_MSG,
        (UINT8*)&stTraceRateSetMsg,sizeof(CPSS_COM_TRACE_RATE_SET_MSG_T)) ;
    if(CPSS_OK != lRet)
    {
        return lRet ;
    }
    
    return CPSS_OK ;    
}

/*******************************************************************************
* 函数名称: cpss_com_trace_recv_setrate_deal
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称      类型         输入/输出         描述
* ulTraceID     UINT32       输入              消息跟踪任务号，由OAMS提供
* ulRate        UINT32       输入              上报速率选项，单位：条/秒
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*******************************************************************************/
INT32 cpss_com_trace_recv_setrate_deal
(
UINT8* pucRecvBuf
)
{
    CPSS_COM_TRACE_RATE_SET_MSG_T *pstTraceRateSetMsg ;
    UINT32 ulTraceId ;
    UINT32 ulRate ;

    pstTraceRateSetMsg = (CPSS_COM_TRACE_RATE_SET_MSG_T*)pucRecvBuf ;

#ifndef CPSS_VOS_WINDOWS
#ifdef CPSS_DSP_CPU
    pstTraceRateSetMsg->ulRate = cpss_ntohl(pstTraceRateSetMsg->ulRate) ;
    pstTraceRateSetMsg->ulTraceID = cpss_ntohl(pstTraceRateSetMsg->ulTraceID) ;
#endif
#endif

    ulTraceId = pstTraceRateSetMsg->ulTraceID ;
    ulRate = pstTraceRateSetMsg->ulRate * 30  ;

    g_stCpssComTraceMan.ulTraceToken = ulRate ;
    g_stCpssComTraceMan.ulTraceTokenValid = ulRate ;

    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_trace_stop_dsp
* 功    能: 该函数停止指定的消息跟踪
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型       输入/输出          描述
* ulTraceID     UINT32       输入               消息跟踪任务号，由OAMS提供
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*******************************************************************************/
INT32 cpss_com_trace_stop_dsp
(
 UINT8  ucDspNo,
 UINT32 ulTraceID
)
{
    CPSS_COM_LOGIC_ADDR_T stDspLogAddr ;
    CPSS_COM_TRACE_TASK_DEL_T stDelTaskMsg ;    
    CPSS_COM_PID_T stDspPid ;
    INT32 lRet ;

    stDelTaskMsg.ulTraceId = cpss_htonl(ulTraceID) ;
        
    /*得到DSP的逻辑地址*/
    cpss_com_dsp_logic_addr_get(ucDspNo,&stDspLogAddr) ;
        
    /*调用发送接口发送数据*/
    stDspPid.stLogicAddr = stDspLogAddr ;
    stDspPid.ulAddrFlag  = CPSS_COM_ADDRFLAG_MASTER ;
    stDspPid.ulPd        = CPSS_DBG_MNGR_PROC ;
    lRet = cpss_com_send(&stDspPid,CPSS_TRACE_TASK_DEL_MSG,
        (UINT8*)&stDelTaskMsg,sizeof(CPSS_COM_TRACE_TASK_DEL_T)) ;
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "send trace stop msg to debug proc failed.destAddr=0x%x,ulAddrFlag=%d,ulPid=0x%x\n",
            *(UINT32*)&stDspPid.stLogicAddr,stDspPid.ulAddrFlag,stDspPid.ulPd);                
        return CPSS_ERROR ;
    }
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_trace_stop
* 功    能: 该函数停止指定的消息跟踪
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型       输入/输出          描述
* ulTraceID     UINT32       输入               消息跟踪任务号，由OAMS提供
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*******************************************************************************/
INT32 cpss_com_trace_stop 
(
 UINT32 ulTraceID
)
{
    INT32 lRet ;
    CPSS_COM_TRACE_TASK_DEL_T stDelTaskMsg ;
    CPSS_COM_PID_T stDstPid ;    

    stDelTaskMsg.ulTraceId = ulTraceID ;

    /*得到目的纤程地址*/
    lRet = cpss_com_logic_addr_get(&stDstPid.stLogicAddr,&stDstPid.ulPd) ;
    if(CPSS_OK != lRet)
    {
        return lRet ;
    }
    stDstPid.ulPd = CPSS_DBG_MNGR_PROC ;

    /*发送消息给dbg纤程*/
    lRet = cpss_com_send(&stDstPid,CPSS_TRACE_TASK_DEL_MSG,
        (UINT8*)&stDelTaskMsg,sizeof(CPSS_COM_TRACE_TASK_DEL_T)) ;
    if(CPSS_OK != lRet)
    {
        return lRet ;
    }

    return CPSS_OK ;    
}

/*******************************************************************************
* 函数名称: cpss_com_trace_recv_deltask_deal
* 功    能: 处理停止消息跟踪消息
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型       输入/输出          描述
* ulTraceID     UINT32       输入               消息跟踪任务号，由OAMS提供
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*******************************************************************************/
INT32 cpss_com_trace_recv_deltask_deal 
(
 UINT8* pucRecvBuf,
 UINT32* pulNullFlag
 )
{
    CPSS_COM_TRACE_TASK_T *pstTraceNode, *pstTraceTail;
    CPSS_COM_TRACE_TASK_DEL_T* pstDelTaskMsg ;
    UINT32 ulTraceID ;

    pstDelTaskMsg = (CPSS_COM_TRACE_TASK_DEL_T*)pucRecvBuf ;

#ifndef CPSS_VOS_WINDOWS
#ifdef CPSS_DSP_CPU
    /*转化字节序为本机字节序*/
    pstDelTaskMsg->ulTraceId = cpss_ntohl(pstDelTaskMsg->ulTraceId) ;
#endif
#endif

    ulTraceID = pstDelTaskMsg->ulTraceId ;

    /* 找到ulTraceID的位置所在 */
    pstTraceNode = g_pstCpssComTraceHead;
    pstTraceTail = pstTraceNode;
    for (; pstTraceNode != NULL; pstTraceNode = pstTraceNode->pstNext)
    {
        if (pstTraceNode->ulTraceID == ulTraceID)/* 找到位置，跳出 */
        {
            break;
        }
        pstTraceTail = pstTraceNode;   /* 除链首外,pstTraceTail始终为pstTraceNode的前一个节点 */
    }
    if (pstTraceNode == NULL)          /* 查找信息不存在，返回错误 */
    {
        return (CPSS_ERROR);
    }
    
    /* 判断删除的节点位置 */
    if (pstTraceNode == g_pstCpssComTraceHead)/* 删除第一个节点，前述已判断过pstTraceNode == NULL的情况 */
    {
        g_pstCpssComTraceHead = pstTraceTail->pstNext;        
    }
    else                               /* 删除非链首的节点 */
    {
        pstTraceTail->pstNext = pstTraceTail->pstNext->pstNext;
    }    
    cpss_mem_free(pstTraceNode);    
    
    g_ulCpssComTraceTaskNum = g_ulCpssComTraceTaskNum - 1;                /* 确定任务成功删除后将计数减1 */
    
    if(g_ulCpssComTraceTaskNum == 0)
    {
        *pulNullFlag = 1 ; 
    }
    
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_com_trace_pid_set
* 功    能: 对filter_pid结构进行赋值
* 函数类型: 
* 参    数: 
* 参数名称       类型                            输入/输出     描述
* 函数返回: 
* 说    明: 
*******************************************************************************/
VOID cpss_com_trace_pid_set
(
CPSS_COM_TRACE_FILTER_T* pstFilter,
CPSS_COM_TRACE_FILTER_PID_T* pstPid, 
INT32 lType
)
{    
    if(lType == CPSS_COM_TRACE_PID_LOCAL_TYPE)
    {
        /*赋值逻辑地址、主备标志、物理地址*/
        cpss_com_logic_addr_get(&pstPid->stLogAddr,&pstPid->ulAddrFlag);

        /*如果是跟踪本地消息*/
        if(pstFilter->enAssPtChoice == CPSS_COM_TRACE_ASSPT_LOCAL_CPU)
        {
            cpss_com_phy_addr_get(&pstPid->stPhyAddr);
        }
        else
        {
            pstPid->stPhyAddr = pstFilter->stTrcPtPhyAddr ;
        }
        
        pstPid->enAddrFlagFilter = pstFilter->enAssPtChoice ;
        pstPid->ulPnoMax = pstFilter->ulTrcPtPdMax ;
        pstPid->ulPnoMin = pstFilter->ulTrcPtPdMin ;
    }
    else
    {
        /*赋值逻辑地址、物理地址*/

        /*如果是跟踪本地消息*/
        if(pstFilter->enAssPtChoice == CPSS_COM_TRACE_ASSPT_LOCAL_CPU)
        {
            cpss_com_logic_addr_get(&pstPid->stLogAddr,&pstPid->ulAddrFlag);
            cpss_com_phy_addr_get(&pstPid->stPhyAddr);            
        }
        else
        {
            pstPid->stLogAddr = pstFilter->stAssLogicAddr ;
            pstPid->stPhyAddr = pstFilter->stAssPtPhyAddr ;
        }

        pstPid->enAddrFlagFilter = pstFilter->enAssPtChoice ;
        pstPid->ulPnoMax = pstFilter->ulAssPtPdMax ;
        pstPid->ulPnoMin = pstFilter->ulAssPtPdMin ;        
    }
}

/*******************************************************************************
* 函数名称: cpss_com_trace_phyaddr_judge
* 功    能: 
* 函数类型: 
* 参    数: 
* 参数名称          类型                       输入/输出         描述
* 函数返回: 
* 说    明: 
*******************************************************************************/ 
INT32 cpss_com_trace_phyaddr_judge
(
 CPSS_COM_TRACE_FILTER_PID_T *pstPidFilter,
 CPSS_COM_PHY_ADDR_T *pstPhyAddr 
)
{  
    /*过滤物理地址*/
    if(pstPidFilter->stPhyAddr.ucCpu != 0xff)
    {
        if(pstPidFilter->stPhyAddr.ucCpu != pstPhyAddr->ucCpu)
        {
            return CPSS_ERROR ;
        }
    }
    if(pstPidFilter->stPhyAddr.ucFrame != 0xff)
    {
        if(pstPidFilter->stPhyAddr.ucFrame != pstPhyAddr->ucFrame)
        {
            return CPSS_ERROR ;
        }
    }
    if(pstPidFilter->stPhyAddr.ucShelf != 0xff)
    {
        if(pstPidFilter->stPhyAddr.ucShelf != pstPhyAddr->ucShelf)
        {
            return CPSS_ERROR ;
        }
    }
    if(pstPidFilter->stPhyAddr.ucSlot != 0xff)
    {
        if(pstPidFilter->stPhyAddr.ucSlot != pstPhyAddr->ucSlot)
        {
            return CPSS_ERROR ;
        }
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_trace_phyaddr_extend_judge
* 功    能: 
* 函数类型: 
* 参    数: 
* 参数名称       类型                            输入/输出     描述
* 函数返回: 
* 说    明: 
*******************************************************************************/
INT32 cpss_com_trace_phyaddr_extend_judge
(
 CPSS_COM_TRACE_FILTER_PID_T *pstPidFilter,
 CPSS_COM_PID_T *pstProc 
 )
{
    CPSS_COM_PHY_ADDR_T  stPhyAddr ;
    CPSS_COM_ROUTE_T* pstRoute ;
    
    CPSS_COM_LOGIC_ADDR_T stLogAddr ;
    UINT32 ulAddrFlag = 0  ;
    INT32 lRet ;

    if(pstProc->ulAddrFlag == CPSS_COM_ADDRFLAG_PHYSICAL)
    {
        cpss_mem_memcpy((UINT8*)&stPhyAddr,
            (UINT8*)&pstProc->stLogicAddr,sizeof(CPSS_COM_PHY_ADDR_T)) ;
    }
    else
    {
        /*查找逻辑地址对应的物理地址*/
        cpss_com_logic_addr_get(&stLogAddr,&ulAddrFlag) ;
        
        if((CPSS_COM_LOGIC_ADDR_SAME(pstProc->stLogicAddr,stLogAddr)==TRUE)&&
            (pstProc->ulAddrFlag == ulAddrFlag))
        {
            cpss_com_phy_addr_get(&stPhyAddr);
        }
        else
        {
            /*如果是TCP链路,不进行物理地址的跟踪*/
            lRet = CPSS_TCP_LOGIC_ADDR_IF(pstProc->stLogicAddr.ucModule) ;
            if(TRUE == lRet)
            {
                return CPSS_ERROR ;
            }

            pstRoute = cpss_com_route_find(pstProc->stLogicAddr) ;
            if(NULL == pstRoute)
            {
                return CPSS_ERROR ;
            }
            stPhyAddr = pstRoute->stPhyAddr[pstProc->ulAddrFlag] ;
        }
    }

    lRet = cpss_com_trace_phyaddr_judge(pstPidFilter,&stPhyAddr) ;

    return lRet ; 
    }

/*******************************************************************************
* 函数名称: cpss_com_trace_logaddr_judge
* 功    能: 
* 函数类型: 
* 参    数: 
* 参数名称          类型                       输入/输出         描述
* 函数返回: 
* 说    明: 
*******************************************************************************/ 
INT32 cpss_com_trace_logaddr_judge
(
 CPSS_COM_TRACE_FILTER_PID_T *pstPidFilter,
 CPSS_COM_PID_T *pstProc
)
{
    /* 过滤逻辑地址 */
    if (pstPidFilter->stLogAddr.ucModule != 0xff)        /* 过滤模块号 */
    {
        if (pstPidFilter->stLogAddr.ucModule != pstProc->stLogicAddr.ucModule)
        {
            return (CPSS_ERROR);
        }
    }
    if (pstPidFilter->stLogAddr.ucSubGroup != 0xff)    /* 过滤子组号 */
    {
        if (pstPidFilter->stLogAddr.ucSubGroup != pstProc->stLogicAddr.ucSubGroup)
        {
            return (CPSS_ERROR);
        }
    }
    if (pstPidFilter->stLogAddr.usGroup != 0xffff)     /* 过滤组号 */
    {
        if (pstPidFilter->stLogAddr.usGroup != pstProc->stLogicAddr.usGroup)
        {
            return (CPSS_ERROR);
        }
    }
    
    /* 过滤逻辑地址主备标记 */
    if (pstPidFilter->enAddrFlagFilter == CPSS_COM_TRACE_ASSPT_LOGICADDR_MASTER)
    {
        if (pstProc->ulAddrFlag != CPSS_COM_ADDRFLAG_MASTER)
        {
            return (CPSS_ERROR);
        }       
    }
    else if (pstPidFilter->enAddrFlagFilter == CPSS_COM_TRACE_ASSPT_LOGICADDR_SLAVE)
    {
        if (pstProc->ulAddrFlag != CPSS_COM_ADDRFLAG_SLAVE)
        {
            return (CPSS_ERROR);
        }       
    }
    else if (pstPidFilter->enAddrFlagFilter == CPSS_COM_TRACE_ASSPT_LOGICADDR_BOTH)
    {          
        if ((pstProc->ulAddrFlag != CPSS_COM_ADDRFLAG_MASTER) 
            && (pstProc->ulAddrFlag != CPSS_COM_ADDRFLAG_SLAVE))
        {
            return (CPSS_ERROR);
        }       
    }
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_trace_local_judge
* 功    能: 
* 函数类型: 
* 参    数: 
* 参数名称          类型                       输入/输出         描述
* 函数返回: 
* 说    明: 
*******************************************************************************/ 
INT32 cpss_com_trace_local_judge
(
 CPSS_COM_TRACE_FILTER_PID_T *pstPidFilter,
 CPSS_COM_PID_T *pstProc 
)
{
    INT32 lRet ;

    if(pstProc->ulAddrFlag == CPSS_COM_ADDRFLAG_PHYSICAL)
    {
        lRet = cpss_com_trace_phyaddr_judge(pstPidFilter,(CPSS_COM_PHY_ADDR_T*)&pstProc->stLogicAddr) ;
    }
    else
    {
        lRet = cpss_com_trace_logaddr_judge(pstPidFilter,pstProc) ;
    }

    return lRet ;
    }


/*******************************************************************************
* 函数名称: cpss_com_trace_dstsrcpid_filter
* 功    能: 判断SRC和DST是否满足过滤条件 
* 函数类型: 
* 参    数: 
* 参数名称       类型                            输入/输出     描述
* pstPidFilter   CPSS_COM_TRACE_FILTER_PID_T *   输入          源或目的PID过滤条件
* pstProc        CPSS_COM_PID_T *                输入          通信消息的PID
* 函数返回: 满足过滤条件CPSS_OK, 不满足为CPSS_ERROR。
* 说    明: 
*******************************************************************************/
INT32 cpss_com_trace_dstsrcpid_filter
(
 CPSS_COM_TRACE_FILTER_PID_T *pstPidFilter,
 CPSS_COM_PID_T *pstProc 
 )
{    
    INT32 lRet ;
    
    if ((pstPidFilter == NULL) || (pstProc == NULL))
    {
        return (CPSS_ERR_COM_PARA_ILLEGAL);
    }
    
    switch(pstPidFilter->enAddrFlagFilter)
    {
    case CPSS_COM_TRACE_ASSPT_ALL_CPU:
        {
            break;
        }
    case CPSS_COM_TRACE_ASSPT_LOCAL_CPU:
        {
            lRet = cpss_com_trace_local_judge(pstPidFilter,pstProc) ;
            if(CPSS_OK != lRet)
    {
                return CPSS_ERROR ;
    }
    
            break ;
        }
    case CPSS_COM_TRACE_ASSPT_PHYADDR:
        {
            lRet = cpss_com_trace_phyaddr_extend_judge(pstPidFilter,pstProc) ;
            if(CPSS_OK != lRet)
            {
                return CPSS_ERROR ;
        }
            
            break;
    }
    case CPSS_COM_TRACE_ASSPT_LOGICADDR_MASTER:
    case CPSS_COM_TRACE_ASSPT_LOGICADDR_SLAVE:
    case CPSS_COM_TRACE_ASSPT_LOGICADDR_BOTH:
    {
            lRet = cpss_com_trace_logaddr_judge(pstPidFilter,pstProc) ;
            if(CPSS_OK != lRet)
        {
                return CPSS_ERROR ;
            }

            break ;           
        }
    default:
        return CPSS_ERROR ;
    }
    
    /* 过滤纤程描述符 */
    if (!((pstPidFilter->ulPnoMin == 0) && (pstPidFilter->ulPnoMax == 0xffff0000)))
    {
        if ((pstProc->ulPd > pstPidFilter->ulPnoMax)
            || (pstProc->ulPd < pstPidFilter->ulPnoMin))
        {
            return (CPSS_ERROR);
        }
    }
    
    return (CPSS_OK);
}


/*******************************************************************************
* 函数名称: cpss_com_trace_judge
* 功    能: 判断该条消息是否需要跟踪
* 函数类型: 
* 参    数: 
* 参数名称          类型                       输入/输出         描述
* pstTraceNode      CPSS_COM_TRACE_TASK_T *    输入              跟踪任务信息节点
* pstMsgHead        CPSS_COM_MSG_HEAD_T *      输入              消息头
* 函数返回: 需要跟踪返回CPSS_OK,否则返回CPSS_ERROR.
* 说    明: 
*******************************************************************************/
INT32 cpss_com_trace_judge
(
 CPSS_COM_TRACE_TASK_T *pstTraceNode,
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
    CPSS_COM_TRACE_FILTER_PID_T stDstFilterPid = {{0}} ;
    CPSS_COM_TRACE_FILTER_PID_T stSrcFilterPid = {{0}} ;
    
    if ((pstTraceNode == NULL) || (pstMsgHead == NULL))
    {
        return (CPSS_ERR_COM_PARA_ILLEGAL);
    }
    /* 首先根据消息ID判断 */
    if (!((pstTraceNode->stTraceFilterInfo.ulMsgIdMin == 0)
        && (pstTraceNode->stTraceFilterInfo.ulMsgIdMax == 0xffffffff)))
    {
        if ((pstMsgHead->ulMsgId < pstTraceNode->stTraceFilterInfo.ulMsgIdMin)
            || (pstMsgHead->ulMsgId > pstTraceNode->stTraceFilterInfo.ulMsgIdMax))
        {
            return (CPSS_ERROR);
        }
    }
    
    /*filterpid进行赋值*/
    cpss_com_trace_pid_set(&pstTraceNode->stTraceFilterInfo,&stDstFilterPid,CPSS_COM_TRACE_PID_REMOTE_TYPE) ;
    cpss_com_trace_pid_set(&pstTraceNode->stTraceFilterInfo,&stSrcFilterPid,CPSS_COM_TRACE_PID_LOCAL_TYPE) ;
    
    /* 根据过滤方向进行过滤 */
    /* SRC_TO_DST,分别比较消息和设置过滤条件的SRC和DST，
    *  消息的SRC对设置过滤条件的SRC，
    *  消息的DST对设置过滤条件的DST，
    *  出错返回
    */
    if (pstTraceNode->stTraceFilterInfo.enMsgDirFlag == CPSS_COM_TRACE_MSGDIR_TRCPT_TO_ASSPT)
    {
        
        if ((CPSS_OK != cpss_com_trace_dstsrcpid_filter(&stDstFilterPid, &pstMsgHead->stDstProc))
            || (CPSS_OK != cpss_com_trace_dstsrcpid_filter(&stSrcFilterPid, &pstMsgHead->stSrcProc)))
        {
            return (CPSS_ERROR);
        }
    }
    /* DST_TO_SRC，将消息和设置过滤条件的SRC和DST反过来比较，
    *  消息的SRC对设置过滤条件的DST，
    *  消息的DST对设置过滤条件的SRC，
    *  出错返回
    */
    else if (pstTraceNode->stTraceFilterInfo.enMsgDirFlag == CPSS_COM_TRACE_MSGDIR_ASSPT_TO_TRCPT)
    {
        if ((CPSS_OK != cpss_com_trace_dstsrcpid_filter(&stDstFilterPid,&pstMsgHead->stSrcProc))
            || (CPSS_OK != cpss_com_trace_dstsrcpid_filter(&stSrcFilterPid,&pstMsgHead->stDstProc)))
        {
            return (CPSS_ERROR);
        }
    }
    /* DUAL_DIRECTION
    *  将SRC_TO_DST和DST_TO_SRC分别比较一次
    *  有一个条件满足就返回成功
    */
    else if (pstTraceNode->stTraceFilterInfo.enMsgDirFlag == CPSS_COM_TRACE_MSGDIR_DUAL)
    {
        if ((CPSS_OK == cpss_com_trace_dstsrcpid_filter(&stDstFilterPid,&pstMsgHead->stDstProc))
            && (CPSS_OK == cpss_com_trace_dstsrcpid_filter(&stSrcFilterPid,&pstMsgHead->stSrcProc)))
        {
            return (CPSS_OK);
        }
        if ((CPSS_OK == cpss_com_trace_dstsrcpid_filter(&stDstFilterPid,&pstMsgHead->stSrcProc))
            && (CPSS_OK == cpss_com_trace_dstsrcpid_filter(&stSrcFilterPid,&pstMsgHead->stDstProc)))
        {
            return (CPSS_OK);
        }
        return (CPSS_ERROR);
    }
    else/* 方向选择出错 */
    {
        return (CPSS_ERROR);
    }
    
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_com_trace_data_add
* 功    能: 消息跟踪数据增加函数
* 函数类型: INT32
* 参    数: 
* 参数名称      类型                     输入/输出       描述
* pstMsgHead    CPSS_COM_MSG_HEAD_T *    输入            通信消息头
* 函数返回: 成功执行返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 本函数在消息通信时被调用。
*******************************************************************************/
INT32 cpss_com_trace_data_add
(
 UINT8 *pucBuf
)
{
    UINT32 ulPosCurr ;
    UINT32 ulUsedLen ;
    UINT32 ulDataLen ;
    UINT32 ulBottonLen ;
    UINT8* pucBufHdr ;

    CPSS_COM_TRACE_HEAD_T* pstTraceHdr ;
    CPSS_COM_TRACE_ENTRY_T* pstTraceEntry ;

    pstTraceHdr = (CPSS_COM_TRACE_HEAD_T*)pucBuf ;
    pstTraceEntry = (CPSS_COM_TRACE_ENTRY_T*)(pucBuf + CPSS_COM_TRACE_HDR_SIZE) ;
    
    pucBufHdr = g_stCpssComTraceMan.pucTraceBufHdr ;
    ulPosCurr = g_stCpssComTraceMan.ulBufPositionCurr ;
    ulUsedLen = g_stCpssComTraceMan.ulUsedLen ;
    ulDataLen = pstTraceEntry->ulInfoLen + CPSS_COM_TRACE_HDR_ENTRY_SIZE ;
    ulBottonLen = CPSS_COM_TRACE_BUF_SIZE - ulPosCurr ;
    
    if((CPSS_COM_TRACE_BUF_SIZE - ulUsedLen) < ulDataLen)
    {
        g_stCpssComTraceMan.ulLostNum = g_stCpssComTraceMan.ulLostNum + 1 ;
        return CPSS_ERROR ;
    }

    if((ulPosCurr + ulDataLen) > CPSS_COM_TRACE_BUF_SIZE)
    {
        if(ulUsedLen+ulBottonLen+ulDataLen > CPSS_COM_TRACE_BUF_SIZE)
        {
            g_stCpssComTraceMan.ulLostNum = g_stCpssComTraceMan.ulLostNum + 1 ;
            return CPSS_ERROR ;
        }
        else
        {
            /*拷贝数据*/
            cpss_mem_memcpy(pucBufHdr,pucBuf,ulDataLen) ;

            /*填写相关信息*/
            g_stCpssComTraceMan.ulTurnPositon = ulPosCurr ;
            g_stCpssComTraceMan.ulUsedLen = ulUsedLen+ulBottonLen+ulDataLen ;
            g_stCpssComTraceMan.ulBufPositionCurr = ulDataLen ;
        }
    }
    else
    {
        /*拷贝数据*/
        cpss_mem_memcpy(pucBufHdr+ulPosCurr,pucBuf,ulDataLen) ;
        
        /*填写相关信息*/
        g_stCpssComTraceMan.ulBufPositionCurr = ulPosCurr + ulDataLen;
        g_stCpssComTraceMan.ulUsedLen = ulUsedLen + ulDataLen ;
    }

    if(g_stCpssComTraceMan.ulBufPositionCurr == CPSS_COM_TRACE_BUF_SIZE)
    {
        g_stCpssComTraceMan.ulBufPositionCurr = 0 ;
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_trace_dstpid_get
* 功    能: 消息跟踪数据发送接口
* 函数类型: INT32
* 参    数: 
* 参数名称      类型                     输入/输出       描述
* pstMsgHead    CPSS_COM_MSG_HEAD_T *    输入            通信消息头
* 函数返回: 成功执行返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 本函数在消息通信时被调用。
*******************************************************************************/
INT32 cpss_com_trace_dstpid_get
(
 UINT32 ulTraceId,
 CPSS_COM_PID_T *pstLdtPid
)
{
    CPSS_COM_TRACE_TASK_T *pstTraceTask ;

    pstTraceTask = g_pstCpssComTraceHead ;

    while(pstTraceTask != NULL)
    {
        if(pstTraceTask->ulTraceID == ulTraceId)
        {
            *pstLdtPid = pstTraceTask->stLdtPid ;
            return CPSS_OK ;
        }
        pstTraceTask = pstTraceTask->pstNext ;
    }
    
    return CPSS_ERROR ;
}


/*******************************************************************************
* 函数名称: cpss_com_trace_data_send
* 功    能: 消息跟踪数据发送接口
* 函数类型: INT32
* 参    数: 
* 参数名称      类型                     输入/输出       描述
* pstMsgHead    CPSS_COM_MSG_HEAD_T *    输入            通信消息头
* 函数返回: 成功执行返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 本函数在消息通信时被调用。
*******************************************************************************/
INT32 cpss_com_trace_data_send()
{
    INT32 lRet1 ;
    INT32 lRet2 ;
    UINT32 ulPosHdr ;
    UINT32 ulUsedLen ;
    UINT32 ulDataLen ;
    UINT32 ulTurnPositon ;
    UINT8* pucBufHdr ;
    static UINT32 ulTraceId ;
    UINT32 ulToken ;
    CPSS_COM_PID_T stLdtPid ;
    
    CPSS_COM_TRACE_HEAD_T* pstTraceHdr ;
    CPSS_COM_TRACE_ENTRY_T* pstTraceEntry ;
    CPSS_COM_TRACE_INFO_T* pstTraceInfo ;
    
    ulPosHdr = g_stCpssComTraceMan.ulBufPositionHdr ;
    ulUsedLen = g_stCpssComTraceMan.ulUsedLen ;
    ulTurnPositon = g_stCpssComTraceMan.ulTurnPositon ;
    ulToken = g_stCpssComTraceMan.ulTraceTokenValid ;

    pucBufHdr = g_stCpssComTraceMan.pucTraceBufHdr + ulPosHdr ;
    
    while((ulUsedLen != 0)&&(ulToken>0))
    {
        if(ulPosHdr == ulTurnPositon)
        {
            pucBufHdr = g_stCpssComTraceMan.pucTraceBufHdr ;
            ulPosHdr  = 0 ;
            ulUsedLen = ulUsedLen - (CPSS_COM_TRACE_BUF_SIZE-ulTurnPositon) ;
            ulTurnPositon = CPSS_COM_TRACE_TURN_POS_INVALID ;

            g_stCpssComTraceMan.ulTurnPositon = CPSS_COM_TRACE_TURN_POS_INVALID ;
            g_stCpssComTraceMan.ulBufPositionHdr = 0 ;
            g_stCpssComTraceMan.ulUsedLen = ulUsedLen ;
        }
        
        pstTraceHdr   = (CPSS_COM_TRACE_HEAD_T*)pucBufHdr ;
        pstTraceEntry = (CPSS_COM_TRACE_ENTRY_T*)(pucBufHdr + CPSS_COM_TRACE_HDR_SIZE) ;
        pstTraceInfo  = (CPSS_COM_TRACE_INFO_T*)pucBufHdr ;
        
        ulTraceId = pstTraceHdr->ulTraceID ;
        ulDataLen = pstTraceEntry->ulInfoLen + CPSS_COM_TRACE_HDR_ENTRY_SIZE ;

        cpss_com_trace_hton(pstTraceInfo) ;
#if 1        
        lRet1 = cpss_com_trace_dstpid_get(ulTraceId,&stLdtPid) ;
        lRet2 = cpss_com_send(&stLdtPid,CPSS_COM_TRACE_MSG,pucBufHdr,ulDataLen) ;
#if 0
        {
            CPSS_COM_MSG_HEAD_T* pstMsgHdr ;            
            lRet2 = CPSS_OK ;            
            pstMsgHdr = (CPSS_COM_MSG_HEAD_T*)(pucBufHdr+CPSS_COM_TRACE_HDR_ENTRY_SIZE);
            printf("cpss com trace send msgid=0x%x,msgLen=%d\n",pstMsgHdr->ulMsgId,pstMsgHdr->ulLen) ;
        }
#endif
        if((CPSS_OK != lRet1)||(CPSS_OK != lRet2))
        {
            g_stCpssComTraceMan.ulLostNum = g_stCpssComTraceMan.ulLostNum + 1 ;
        }
#endif
        ulUsedLen = ulUsedLen - ulDataLen ;
        ulToken = ulToken - 1 ;

        g_stCpssComTraceMan.ulBufPositionHdr = g_stCpssComTraceMan.ulBufPositionHdr + ulDataLen ;
        g_stCpssComTraceMan.ulUsedLen = ulUsedLen ; 

        if(g_stCpssComTraceMan.ulBufPositionHdr == CPSS_COM_TRACE_BUF_SIZE)
        {
            g_stCpssComTraceMan.ulBufPositionHdr = 0 ;
        }

        ulPosHdr = g_stCpssComTraceMan.ulBufPositionHdr ;
        pucBufHdr = g_stCpssComTraceMan.pucTraceBufHdr + ulPosHdr ;
    }

    g_stCpssComTraceMan.ulTraceTokenValid = ulToken ;
    
    return CPSS_OK ;
}

VOID cpss_com_trace_lost_show()
{
    printf("g_stCpssComTraceMan.ulLostNum=%d\n",g_stCpssComTraceMan.ulLostNum) ;
}

/*******************************************************************************
* 函数名称: cpss_com_trace_hton
* 功    能: 
* 函数类型: INT32
* 参    数: 
* 参数名称      类型                     输入/输出       描述
* pstMsgHead    CPSS_COM_MSG_HEAD_T *    输入            通信消息头
* 函数返回: 成功执行返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 本函数在消息通信时被调用。
*******************************************************************************/
VOID cpss_com_trace_hton
(
 CPSS_COM_TRACE_INFO_T* pstTraceInfo
)
{
    /* 转换网络字节序 */
    cpss_com_comm_hdr_hton(&(pstTraceInfo->stMsgHead));
    pstTraceInfo->stTraceHead.ulSeqID = cpss_htonl(pstTraceInfo->stTraceHead.ulSeqID);
    pstTraceInfo->stTraceHead.ulTraceID = cpss_htonl(pstTraceInfo->stTraceHead.ulTraceID);
    pstTraceInfo->stTraceHead.usInfoNum = cpss_htons(pstTraceInfo->stTraceHead.usInfoNum);            
    pstTraceInfo->stTraceEntry.stTime.usYear = cpss_htons(pstTraceInfo->stTraceEntry.stTime.usYear);
    pstTraceInfo->stTraceEntry.ulInfoLen = cpss_htonl(pstTraceInfo->stTraceEntry.ulInfoLen);
    pstTraceInfo->stTraceEntry.ulSerialNo = cpss_htonl(pstTraceInfo->stTraceEntry.ulSerialNo);
    pstTraceInfo->stTraceEntry.ulTick = cpss_htonl(pstTraceInfo->stTraceEntry.ulTick);    
}


/*******************************************************************************
* 函数名称: cpss_com_trace
* 功    能: 消息跟踪函数接口
* 函数类型: INT32
* 参    数: 
* 参数名称      类型                     输入/输出       描述
* pstMsgHead    CPSS_COM_MSG_HEAD_T *    输入            通信消息头
* 函数返回: 成功执行返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 本函数在消息通信时被调用。
*******************************************************************************/
INT32 cpss_com_trace_ex(CPSS_COM_MSG_HEAD_T *pstMsgHead)
{
    INT32 lRet ;
    UINT32 ulTraceLen;
    CPSS_COM_TRACE_TASK_T *pstTraceNode;
    CPSS_COM_TRACE_INFO_T *pstTraceInfo;
    CPSS_COM_PID_T stDstPid ;
    UINT32 ulInfoLen ;

#if 0
    /*如果没有跟踪任务则不进行跟踪*/
    if(g_ulCpssComTraceTaskNum == 0)
    {
        return CPSS_OK ;
    }
#endif    
    /* 检查发送的消息是否是发送的跟踪消息，如果是，则不跟踪跟踪消息 */
    if ((pstMsgHead->ulMsgId == CPSS_COM_TRACE_MSG)||(pstMsgHead->ulMsgId == CPSS_TRACE_ADD_MSG)
        ||(pstMsgHead->ulMsgId == CPSS_TRACE_TIMER_MSG)||(pstMsgHead->ulMsgId == CPSS_TRACE_TASK_ADD_MSG)
        ||(pstMsgHead->ulMsgId == CPSS_TRACE_TASK_DEL_MSG)||(pstMsgHead->ulMsgId == CPSS_TRACE_TASK_RATE_SET_MSG))
    {
        return (CPSS_OK);
    }
    /* 在跟踪任务节点中循环执行 */
    for (pstTraceNode = g_pstCpssComTraceHead; pstTraceNode != NULL; pstTraceNode = pstTraceNode->pstNext)
    {
        if (CPSS_OK == cpss_com_trace_judge(pstTraceNode, pstMsgHead))
        {
            /*申请TraceInfo内存*/
            pstTraceInfo = cpss_mem_malloc(sizeof(CPSS_COM_TRACE_INFO_T)) ;
            if(NULL == pstTraceInfo)
            {
                return CPSS_ERROR ;
            }
            
            /* 消息跟踪包头结构 */
            pstTraceInfo->stTraceHead.ulSeqID = 0;        /* 接口消息流水号（固定为0） */
            pstTraceInfo->stTraceHead.ulTraceID = pstTraceNode->ulTraceID;
            cpss_com_phy_addr_get(&(pstTraceInfo->stTraceHead.stPhyAddr));
            pstTraceInfo->stTraceHead.ucBoardType = 1;    /* 待定 */
            pstTraceInfo->stTraceHead.usInfoNum = 1;      /* 加入第一条数据计数 */
            /* 填充消息跟踪头结构 */
            pstTraceInfo->stTraceEntry.ulSerialNo = pstTraceNode->ulSerialNo;            
            pstTraceNode->ulSerialNo = pstTraceNode->ulSerialNo + 1;
            pstTraceInfo->stTraceEntry.ulTick = cpss_tick_get(); /* 填充tick时间 */
            cpss_clock_get(&(pstTraceInfo->stTraceEntry.stTime));/* 填充本地时间 */            
            /* 判断消息长度是否超出限定的范围 */
            if (pstMsgHead->ulLen + sizeof(CPSS_COM_MSG_HEAD_T) 
                + sizeof(CPSS_COM_TRACE_ENTRY_T) + sizeof(CPSS_COM_TRACE_HEAD_T) 
                > pstTraceNode->ulTraceLen)
            {
                pstTraceInfo->stTraceEntry.ulInfoLen = pstTraceNode->ulTraceLen
                    - sizeof(CPSS_COM_TRACE_ENTRY_T)
                    - sizeof(CPSS_COM_TRACE_HEAD_T);
            }
            else
            {
                pstTraceInfo->stTraceEntry.ulInfoLen = pstMsgHead->ulLen + sizeof(CPSS_COM_MSG_HEAD_T);
                ulInfoLen = CPSS_MOD(pstTraceInfo->stTraceEntry.ulInfoLen , 4);
                if(ulInfoLen != 0)
                {
                    pstTraceInfo->stTraceEntry.ulInfoLen = pstTraceInfo->stTraceEntry.ulInfoLen + (4-ulInfoLen) ;
                }
            }
            pstTraceInfo->stMsgHead = *pstMsgHead;   
            ulTraceLen = pstTraceInfo->stTraceEntry.ulInfoLen;

            /* 加入被跟踪的消息体 */
            if(ulTraceLen < (pstMsgHead->ulLen+sizeof(CPSS_COM_MSG_HEAD_T)))
            {
                cpss_mem_memcpy(pstTraceInfo->cBuf,pstMsgHead->pucBuf,ulTraceLen-sizeof(CPSS_COM_MSG_HEAD_T)) ;         
            }
            else
            {
                cpss_mem_memcpy(pstTraceInfo->cBuf,pstMsgHead->pucBuf,pstMsgHead->ulLen) ;         
            }        
            
            /*得到目的纤程地址*/
            lRet = cpss_com_logic_addr_get(&stDstPid.stLogicAddr,&stDstPid.ulAddrFlag) ;
            if(CPSS_OK != lRet)
            {
                cpss_mem_free(pstTraceInfo) ;
                return CPSS_ERROR ;
            }
            stDstPid.ulPd = CPSS_DBG_MNGR_PROC ;
                
            /*发送消息给dbg纤程*/
            lRet = cpss_com_send(&stDstPid,CPSS_TRACE_ADD_MSG,(UINT8*)pstTraceInfo,
                ulTraceLen + sizeof(CPSS_COM_TRACE_ENTRY_T) + sizeof(CPSS_COM_TRACE_HEAD_T)) ;
            if(CPSS_OK != lRet)
            {
                cpss_mem_free(pstTraceInfo) ;
                
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
                    "send trace msg to debug proc failed.\n");
                return CPSS_ERROR ;
            }

            cpss_mem_free(pstTraceInfo) ;            
        }
    }
    
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_com_g_ulCpssComTraceTaskNum_get
* 功    能: 获得g_ulCpssComTraceTaskNum地址
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出     描述
* 
* 函数返回: 
* 说    明:
*******************************************************************************/
INT32 cpss_com_g_ulCpssComTraceTaskNum_get()
{
    return (INT32)(&g_ulCpssComTraceTaskNum);
}

/*******************************************************************************
* 函数名称: cpss_com_g_pstCpssComTraceHead_get
* 功    能: 获得g_pstCpssComTraceHead地址
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出     描述
* 
* 函数返回: 
* 说    明:
*******************************************************************************/

INT32 cpss_com_g_pstCpssComTraceHead_get()
{
    return (INT32)(g_pstCpssComTraceHead);
}

/******************************* 源文件结束 ***********************************/

