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
#include "cpss_com_common.h"
#include "cpss_com_drv.h"
#include "cpss_com_win.h"
#include "cpss_com_link.h"
#include "cpss_com_app.h"
#include "cpss_com_trace.h"
#include "cpss_err.h"

/******************************* 局部宏定义 **********************************/


/******************************* 全局变量定义/初始化 *************************/

COM_SLID_LINK_T g_stSlidWinLink ;         /*滑窗使用的滑窗指针的单链表*/


UINT32 g_ulSlidSem = 0 ;                  /*可靠传输全局互斥信号量*/
#ifndef CPSS_DSP_CPU
/******************************* 局部常数和类型定义 **************************/


/******************************* 局部函数原型声明 ****************************/
extern VOID cpss_kw_set_task_stop() ;
extern VOID cpss_kw_set_task_run() ;

extern VOID cpss_com_slid_ack_timeout_deal(CPSS_COM_SLID_WIN_T* pstSlidWin) ;
extern VOID cpss_com_slid_ack_frame_send(CPSS_COM_SLID_WIN_T* pstSlidWin) ;
/******************************* 函数实现 ************************************/

/*******************************************************************************
* 函数名称: cpss_com_slid_timer_reset
* 功    能: 重置定时器
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_timer_reset
(
 UINT32 ulTimerClass,
 UINT32 ulTimerInter,
 CPSS_COM_SLID_WIN_T* pstSlidWin
 )
{
    switch(ulTimerClass)
    {
    case SLID_SYN_TIMER_CLASS:
        {
            pstSlidWin->stSynreqTimer.ulEnableFlag = 1 ;
            pstSlidWin->stSynreqTimer.ulTimerInter = ulTimerInter ;                
            pstSlidWin->stSynreqTimer.ulTimerCount = cpss_tick_get() ;
        }
        break ;
    case SLID_ACK_TIMER_CLASS:
        {
            pstSlidWin->stAckTimer.ulEnableFlag = 1 ;
            pstSlidWin->stAckTimer.ulTimerInter = ulTimerInter ;
            pstSlidWin->stAckTimer.ulTimerCount = cpss_tick_get() ;
        }
        break;
    case SLID_RESEND_TIMER_CLASS:
        {
            pstSlidWin->stResendTimer.ulEnableFlag = 1 ;
            pstSlidWin->stResendTimer.ulTimerInter = ulTimerInter ;
            pstSlidWin->stResendTimer.ulTimerCount = cpss_tick_get() ;
        }
        break ;
    case SLID_DETECT_TIMER_CLASS:
        {
            pstSlidWin->stDetectTimer.ulEnableFlag = 1 ;
            pstSlidWin->stDetectTimer.ulTimerInter = ulTimerInter ;
            pstSlidWin->stDetectTimer.ulTimerCount = cpss_tick_get() ;
        }
        break ;
    case SLID_SYN_END_TIMER_CLASS:
        {
            pstSlidWin->stSynEndTimer.ulEnableFlag = 1 ;
            pstSlidWin->stSynEndTimer.ulTimerInter = ulTimerInter ;
            pstSlidWin->stSynEndTimer.ulTimerCount = cpss_tick_get() ;
        }
        break ;
    default:
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "\nreset timer class(%d) unknown.") ;
        }
    }
}


/*******************************************************************************
* 函数名称: cpss_com_slid_timer_start
* 功    能: 启动定时器
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_timer_start
(
 UINT32 ulTimerClass,
 UINT32 ulTimerInter,
 CPSS_COM_SLID_WIN_T* pstSlidWin
 )
{
    switch(ulTimerClass)
    {
    case SLID_SYN_TIMER_CLASS:
        {
            if(pstSlidWin->stSynreqTimer.ulEnableFlag == 0)
            {
                pstSlidWin->stSynreqTimer.ulEnableFlag = 1 ;
                pstSlidWin->stSynreqTimer.ulTimerInter = ulTimerInter ;
                pstSlidWin->stSynreqTimer.ulTimerCount = cpss_tick_get() ;                
            }
        }
        break ;
    case SLID_ACK_TIMER_CLASS:
        {
            if(pstSlidWin->stAckTimer.ulEnableFlag == 0)
            {
                pstSlidWin->stAckTimer.ulEnableFlag = 1 ;
                pstSlidWin->stAckTimer.ulTimerInter = ulTimerInter ;
                pstSlidWin->stAckTimer.ulTimerCount = cpss_tick_get() ;
            }             
        }
        break;
    case SLID_RESEND_TIMER_CLASS:
        {
            if(pstSlidWin->stResendTimer.ulEnableFlag == 0)
            {
                pstSlidWin->stResendTimer.ulEnableFlag = 1 ;
                pstSlidWin->stResendTimer.ulTimerInter = ulTimerInter ;
                pstSlidWin->stResendTimer.ulTimerCount = cpss_tick_get() ;
            }          
        }
        break ;
    case SLID_DETECT_TIMER_CLASS:
        {
            if(pstSlidWin->stDetectTimer.ulEnableFlag == 0)
            {
                pstSlidWin->stDetectTimer.ulEnableFlag = 1 ;
                pstSlidWin->stDetectTimer.ulTimerInter = ulTimerInter ;
                pstSlidWin->stDetectTimer.ulTimerCount = cpss_tick_get() ;
            }
        }
        break ;
    case SLID_SYN_END_TIMER_CLASS:
        {
            if(pstSlidWin->stSynEndTimer.ulEnableFlag == 0)
            {
                pstSlidWin->stSynEndTimer.ulEnableFlag = 1 ;
                pstSlidWin->stSynEndTimer.ulTimerInter = ulTimerInter ;
                pstSlidWin->stSynEndTimer.ulTimerCount = cpss_tick_get() ;
            }         
        }
        break ;
    default:
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "\nstart timer class(%d) unknown.") ;
        }
    }
}

/*******************************************************************************
* 函数名称: cpss_com_slid_timer_stop
* 功    能: 停止定时器
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_timer_stop
(
 UINT32 ulTimerClass,
 CPSS_COM_SLID_WIN_T* pstSlidWin
 )
{
    switch(ulTimerClass)
    {
    case SLID_SYN_TIMER_CLASS:
        {
            if(pstSlidWin->stSynreqTimer.ulEnableFlag != 0)
            {
                pstSlidWin->stSynreqTimer.ulEnableFlag = 0 ;
                pstSlidWin->stSynreqTimer.ulTimerInter = 0 ;
                pstSlidWin->stSynreqTimer.ulTimerCount = 0 ;
            }
        }
        break ;
    case SLID_ACK_TIMER_CLASS:
        {
            if(pstSlidWin->stAckTimer.ulEnableFlag != 0)
            {
                pstSlidWin->stAckTimer.ulEnableFlag = 0 ;
                pstSlidWin->stAckTimer.ulTimerInter = 0 ;
                pstSlidWin->stAckTimer.ulTimerCount = 0 ;
            }
        }
        break;
    case SLID_RESEND_TIMER_CLASS:
        {
            if(pstSlidWin->stResendTimer.ulEnableFlag != 0)
            {
                pstSlidWin->stResendTimer.ulEnableFlag = 0 ;
                pstSlidWin->stResendTimer.ulTimerInter = 0 ;
                pstSlidWin->stResendTimer.ulTimerCount = 0 ;
            }
        }
        break ;
    case SLID_DETECT_TIMER_CLASS:
        {
            if(pstSlidWin->stDetectTimer.ulEnableFlag != 0)
            {
                pstSlidWin->stDetectTimer.ulEnableFlag = 0 ;
                pstSlidWin->stDetectTimer.ulTimerInter = 0 ;
                pstSlidWin->stDetectTimer.ulTimerCount = 0 ;
            }
        }
        break ;
    case SLID_SYN_END_TIMER_CLASS:
        {
            if(pstSlidWin->stSynEndTimer.ulEnableFlag != 0)
            {
                pstSlidWin->stSynEndTimer.ulEnableFlag = 0 ;
                pstSlidWin->stSynEndTimer.ulTimerInter = 0 ;
                pstSlidWin->stSynEndTimer.ulTimerCount = 0 ;
            }
        }
        break ;
    default:
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "\nstop timer class(%d) unknown.") ;
        }
    }
}

/*******************************************************************************
* 函数名称: cpss_com_slid_timer_exist
* 功    能: 判断定时器是否停止
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          CPSS_OK
*          CPSS_ERROR                          
* 说   明:
*******************************************************************************/
INT32 cpss_com_slid_timer_exist
( 
 UINT32 ulTimerClass,
 BOOL* bExist,
 CPSS_COM_SLID_WIN_T* pstSlidWin
)
{
    switch(ulTimerClass)
    {
    case SLID_SYN_TIMER_CLASS:
        {
            if(pstSlidWin->stSynreqTimer.ulEnableFlag != 0)
            {
                *bExist = TRUE ;
            }
            else
            {
                *bExist = FALSE ;
            }
        }
        break ;
    case SLID_ACK_TIMER_CLASS:
        {
            if(pstSlidWin->stAckTimer.ulEnableFlag != 0)
            {
                *bExist = TRUE ;
            }
            else
            {
                *bExist = FALSE ;
            }
        }
        break;
    case SLID_RESEND_TIMER_CLASS:
        {
            if(pstSlidWin->stResendTimer.ulEnableFlag != 0)
            {
                *bExist = TRUE ;
            }
            else
            {
                *bExist = FALSE ;
            }
        }
        break ;
    case SLID_DETECT_TIMER_CLASS:
        {
            if(pstSlidWin->stDetectTimer.ulEnableFlag != 0)
            {
                *bExist = TRUE ;
            }
            else
            {
                *bExist = FALSE ;
            }
        }
        break ;
    case SLID_SYN_END_TIMER_CLASS:
        {
            if(pstSlidWin->stSynEndTimer.ulEnableFlag != 0)
            {
                *bExist = TRUE ;
            }
            else
            {
                *bExist = FALSE ;
            }
        }
        break ;
    default:
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "\nstop timer class(%d) unknown.") ;
            return CPSS_ERROR ;
        }
    }
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_abs_get
* 功    能: 得到两个参数的绝对值
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
UINT32 cpss_com_slid_abs_get
(
 UINT32 ulA,
 UINT32 ulB
 )
{
    if( ulA < ulB) return (ulB - ulA) ;
    
    return  (ulA - ulB) ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_if_between
* 功    能: 判断B是否在A和C之间(A为左端值,C为右端值)
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_if_between 
(
 UINT16 usA, 
 UINT16 usB, 
 UINT16 usC
 )
{
    return ((usA <= usB ) && (usB <= usC)) || 
        ((usC < usA) && (usA <= usB)) || 
        ((usB <= usC) && (usC < usA) );
}


/*******************************************************************************
* 函数名称: cpss_com_slid_if_timeout
* 功    能: 判断定时器是否超时
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无
* 说   明:                                        
*******************************************************************************/
BOOL cpss_com_slid_if_timeout
(
 UINT32 ulTimerClass,
 CPSS_COM_SLID_WIN_T*pstSlidWin
 )
{
    UINT32 ulCurrTicks = 0 ;
    
    /*设置当前时间*/
    ulCurrTicks = cpss_tick_get() ;
    
    switch(ulTimerClass)
    {
    case SLID_ACK_TIMER_CLASS:
        {
            if( (pstSlidWin->stAckTimer.ulEnableFlag != 0 ) &&
                (cpss_com_slid_abs_get(ulCurrTicks ,pstSlidWin->stAckTimer.ulTimerCount) >= pstSlidWin->stAckTimer.ulTimerInter) )
            {
                return TRUE ;
            }
            return FALSE ;
        }
    case SLID_RESEND_TIMER_CLASS:
        {
            if( (pstSlidWin->stResendTimer.ulEnableFlag != 0 ) &&
                (cpss_com_slid_abs_get(ulCurrTicks ,pstSlidWin->stResendTimer.ulTimerCount) >= pstSlidWin->stResendTimer.ulTimerInter) )
            {
                return TRUE ;
            }
            return FALSE ;
        }
    case SLID_SYN_TIMER_CLASS:
        {
            if( (pstSlidWin->stSynreqTimer.ulEnableFlag != 0 ) &&
                (cpss_com_slid_abs_get(ulCurrTicks ,pstSlidWin->stSynreqTimer.ulTimerCount) >= pstSlidWin->stSynreqTimer.ulTimerInter) )          
            {
                return TRUE ;
            }
            return FALSE ;
        }
    case SLID_DETECT_TIMER_CLASS:
        {
            if( (pstSlidWin->stDetectTimer.ulEnableFlag != 0 ) &&
                (cpss_com_slid_abs_get(ulCurrTicks ,pstSlidWin->stDetectTimer.ulTimerCount) >= pstSlidWin->stDetectTimer.ulTimerInter) )          
            {
                return TRUE ;
            }
            return FALSE ;
        }
    case SLID_SYN_END_TIMER_CLASS:
        {
            if( (pstSlidWin->stSynEndTimer.ulEnableFlag != 0 ) &&
                (cpss_com_slid_abs_get(ulCurrTicks ,pstSlidWin->stSynEndTimer.ulTimerCount) >= pstSlidWin->stSynEndTimer.ulTimerInter) )          
            {
                return TRUE ;
            }
            return FALSE ;
        }
    }
    return FALSE ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_pid_ntoh
* 功    能: 将pid转化为主机字节序
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_pid_ntoh
(
 CPSS_COM_PID_T *pstPid
 )
{
    pstPid->stLogicAddr.usGroup=cpss_ntohs(pstPid->stLogicAddr.usGroup);
    pstPid->ulAddrFlag=cpss_ntohl(pstPid->ulAddrFlag);
    pstPid->ulPd=cpss_ntohl(pstPid->ulPd);
}    


/*******************************************************************************
* 函数名称: cpss_com_slid_pid_hton
* 功    能: 将pid转化为网络字节序
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_pid_hton
(
 CPSS_COM_PID_T *pstPid
 )
{
    pstPid->stLogicAddr.usGroup=cpss_htons(pstPid->stLogicAddr.usGroup);
    pstPid->ulAddrFlag=cpss_htonl(pstPid->ulAddrFlag);
    pstPid->ulPd=cpss_htonl(pstPid->ulPd);
}    


/*******************************************************************************
* 函数名称: cpss_com_slid_hdr_hton
* 功    能: 将IPC头和COM头转化为网络字节序
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_hdr_hton
(
CPSS_COM_TRUST_HEAD_T *pstHdr
)
{
    CPSS_COM_MSG_HEAD_T *pstMsg;
    
    /* 判断参数的有效性 */
    if(NULL==pstHdr)
    {
        return;
    }
    
    /* 处理字节序,传输层 */
    pstHdr->usSliceLen=cpss_htons(pstHdr->usSliceLen);
    pstHdr->usSeq=cpss_htons(pstHdr->usSeq);
    pstHdr->usAck=cpss_htons(pstHdr->usAck);
    pstHdr->usFlowCtrl=cpss_htons(pstHdr->usFlowCtrl);
    pstHdr->usCheckSum=cpss_htons(pstHdr->usCheckSum);
    
    if((pstHdr->fragFlag == COM_FRAG_NOFRAG)||
        (pstHdr->fragFlag == COM_FRAG_BEGIN))
    {
        /* 处理字节序,分发层 */
        
        pstMsg=(CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstHdr);
        
        cpss_com_slid_pid_hton(&pstMsg->stDstProc);
        cpss_com_slid_pid_hton(&pstMsg->stSrcProc);    
        
        pstMsg->ulMsgId=cpss_htonl(pstMsg->ulMsgId);
        pstMsg->ulLen=cpss_htonl(pstMsg->ulLen);
    }
}


/*******************************************************************************
* 函数名称: cpss_com_slid_hdr_ntoh
* 功    能: 将IPC头和COM头转化为主机字节序
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_hdr_ntoh
(
CPSS_COM_TRUST_HEAD_T *pstHdr
)
{
    CPSS_COM_MSG_HEAD_T *pstMsg;
    
    /* 判断参数的有效性 */
    if(NULL==pstHdr)
    {
        return;
    }
    
    /* 处理字节序,传输层 */
    pstHdr->usSliceLen=cpss_ntohs(pstHdr->usSliceLen);
    pstHdr->usSeq=cpss_ntohs(pstHdr->usSeq);
    pstHdr->usAck=cpss_ntohs(pstHdr->usAck);
    pstHdr->usFlowCtrl=cpss_ntohs(pstHdr->usFlowCtrl);
    pstHdr->usCheckSum=cpss_ntohs(pstHdr->usCheckSum);
    
    if((pstHdr->fragFlag == COM_FRAG_NOFRAG)||
        (pstHdr->fragFlag == COM_FRAG_BEGIN))
    {
        /* 处理字节序,分发层 */
        pstMsg=(CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstHdr);
        
        cpss_com_slid_pid_ntoh(&pstMsg->stDstProc);
        cpss_com_slid_pid_ntoh(&pstMsg->stSrcProc);    
        
        pstMsg->ulMsgId=cpss_ntohl(pstMsg->ulMsgId);
        pstMsg->ulLen=cpss_ntohl(pstMsg->ulLen);
    }
}

/*******************************************************************************
* 函数名称: cpss_com_slid_node_insert
* 功    能: 在单链表中插入一个节点
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_node_insert
(
 UINT32 ulData
 )
{
    COM_SLID_LINK_NODE_T *pstNode ;
    COM_SLID_LINK_NODE_T *pstNodeTmp ;
    
    pstNode = g_stSlidWinLink.pstHead ;    
    
    /*查找链表中是否有重复的节点项*/
    while(pstNode!=NULL)
    {
        if(pstNode->ulData == ulData)
        { 
            return CPSS_OK ;
        }
        pstNode = pstNode->pstNext ;
    }
    
    /*申请新的节点内存*/
    pstNodeTmp = (COM_SLID_LINK_NODE_T*)cpss_mem_malloc( sizeof(COM_SLID_LINK_NODE_T) ) ;
    if(pstNodeTmp == NULL)
    {
        return CPSS_ERROR ;
    }
    
    /*对节点中的成员变量进行赋值操作*/
    pstNodeTmp->ulData = ulData ;
    pstNodeTmp->pstNext = NULL ;
    
    /*将新的节点插入到链路头*/
    pstNode = g_stSlidWinLink.pstHead ;
    g_stSlidWinLink.pstHead = pstNodeTmp ;
    g_stSlidWinLink.pstHead->pstNext = pstNode ;
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_node_remove
* 功    能: 在单链表中删除一个节点
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_node_remove
(
 UINT32 ulData
 )
{
    COM_SLID_LINK_NODE_T *pstNodePre ;
    COM_SLID_LINK_NODE_T *pstNodeTmp ;
    
    pstNodePre = g_stSlidWinLink.pstHead ;
    pstNodeTmp = pstNodePre ;
    
    /*如果找到了需要删除的链路，删除此链路对象*/
    while(pstNodeTmp != NULL)
    {
        if(pstNodeTmp->ulData==ulData)
        {
            if(pstNodePre == pstNodeTmp)
            {
                g_stSlidWinLink.pstHead = pstNodeTmp->pstNext ;
            }
            else
            {
                pstNodePre->pstNext = pstNodeTmp->pstNext ;
            }
            cpss_mem_free( pstNodeTmp ) ;

            return ;
        }
        else
        {
            pstNodePre = pstNodeTmp ;
            pstNodeTmp = pstNodeTmp->pstNext ;
        }    
    }
}

/*******************************************************************************
* 函数名称: cpss_com_slid_local_proc_pid_fill
* 功    能: 填充本地纤程地址信息
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_local_proc_pid_fill
(
CPSS_COM_PID_T *pstSlidProcPid,
UINT32 ulPid
)
{
    cpss_com_host_logic_addr_get(&(pstSlidProcPid->stLogicAddr), &(pstSlidProcPid->ulAddrFlag));
    
    pstSlidProcPid->ulPd =ulPid;
    
    return ;
    
}


/*******************************************************************************
* 函数名称: cpss_com_slid_checksum_fill
* 功    能: 填充IPC帧头的校验和
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_checksum_fill
(
UINT8* pucBuf
)
{
    CPSS_COM_TRUST_HEAD_T* pstTrustHdr ;
    
    pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)pucBuf ;
    pstTrustHdr->usCheckSum = pstTrustHdr->usAck + pstTrustHdr->usSeq +
                              pstTrustHdr->usSliceLen ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_sum_check
* 功    能: 检查IPC帧头的校验和
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_sum_check
(
CPSS_COM_TRUST_HEAD_T* pstTrustHdr
)
{
    if( pstTrustHdr->usCheckSum != (pstTrustHdr->usAck + pstTrustHdr->usSeq + 
        pstTrustHdr->usSliceLen ) )
    {
        return CPSS_ERROR ;
    }
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_semaphore_init
* 功    能: 创建可靠传输需要的全局变量
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_semaphore_init()
{
    g_ulSlidSem = cpss_vos_sem_m_create(0) ;
    
    if(g_ulSlidSem == VOS_SEM_DESC_INVALID)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "\ncreate slid mutex sem failed.") ;
        return CPSS_ERROR ;
    }
    
    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "\nslid sem(%d) init success.",g_ulSlidSem) ;
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_sendwin_para_modify
* 功    能: 修改发送滑窗的参数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_sendwin_para_modify
(
CPSS_COM_SLID_WIN_T *pstSlidWin,
CPSS_COM_TRUST_HEAD_T *pstSliceNode
)
{
    pstSlidWin->usSendWinBusyNum = pstSlidWin->usSendWinBusyNum + 1 ;
    pstSlidWin->pucSlidSendWin[CPSS_MOD((pstSlidWin->usNextFrameToSend),COM_SLIDWIN_SIZE)] = (UINT8*)pstSliceNode ;
    pstSlidWin->usNextFrameToSend = CPSS_MOD((pstSlidWin->usNextFrameToSend+1),COM_SLID_PKGNUM_SIZE) ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_recv_win_forward
* 功    能: 向前滑动接收滑窗
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_recv_win_forward
(
CPSS_COM_SLID_WIN_T *pstSlidWin
)
{
    pstSlidWin->ucRecvWinFlag[CPSS_MOD(pstSlidWin->usFrameExpected , COM_SLIDWIN_SIZE)] = 0 ;    
    pstSlidWin->usFrameExpected = CPSS_MOD((pstSlidWin->usFrameExpected + 1) , COM_SLID_PKGNUM_SIZE) ;
    pstSlidWin->usTooFar = CPSS_MOD((pstSlidWin->usTooFar + 1) , COM_SLID_PKGNUM_SIZE) ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_data_link_add
* 功    能: 将拆包的数据挂在滑窗对象中
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_data_link_add
(
CPSS_COM_SLID_WIN_T *pstSlidWin,
UINT8* pucSliceHead, 
UINT8* pucSliceTail
)
{
    if( pstSlidWin->stSendBufManage.pstSlidSendBufLinkHead == NULL )
    {
        pstSlidWin->stSendBufManage.pstSlidSendBufLinkHead = (CPSS_COM_TRUST_HEAD_T*)pucSliceHead ;
        pstSlidWin->stSendBufManage.pstSlidSendBufLinkTail = (CPSS_COM_TRUST_HEAD_T*)pucSliceTail ;
    }
    else
    {
        cpss_com_frag_slice_put((CHAR*)(pstSlidWin->stSendBufManage.pstSlidSendBufLinkTail),pucSliceHead);
        pstSlidWin->stSendBufManage.pstSlidSendBufLinkTail = (CPSS_COM_TRUST_HEAD_T*)pucSliceTail ;
    }
}


/*******************************************************************************
* 函数名称: cpss_com_slid_hdr_pub_info_fill
* 功    能: 填充IPC消息头中一些普通的字节
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_hdr_pub_info_fill
(
CPSS_COM_TRUST_HEAD_T* pstTrustMsg
)
{
    /* 初始化传输层消息头 */
    cpss_com_trust_hrd_pub_info_fill(pstTrustMsg);
    pstTrustMsg->usSliceLen = 0 ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_info_fill
* 功    能: 填充IPC头中涉及滑窗的数据
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_info_fill
(
 UINT8 ucFrameKind,
 UINT8 *pucHdr,
 UINT16 usSeq,
 UINT16 usAck
)
{
    CPSS_COM_TRUST_HEAD_T  *pstHdr = NULL ;
    pstHdr = (CPSS_COM_TRUST_HEAD_T*)pucHdr;
    pstHdr->usAck   = usAck;
    pstHdr->usSeq   = usSeq;
    pstHdr->ucPType = ucFrameKind ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_win_info_fill
* 功    能: 从滑窗中将数据填写到发送数据包中
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_win_info_fill
(
 CPSS_COM_TRUST_HEAD_T  *pstSliceNode,
 CPSS_COM_SLID_WIN_T *pstSlidWin 
 )
{
    pstSliceNode->usSeq = pstSlidWin->usNextFrameToSend ;
    pstSliceNode->usAck = CPSS_MOD((pstSlidWin->usFrameExpected + COM_SLID_PKGNUM_SIZE -1) , COM_SLID_PKGNUM_SIZE) ;
    pstSliceNode->ucPType = COM_SLID_PTYPE_CREDIBLE ;
    
    pstSliceNode->usFlowCtrl = 0 ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_proc_send_msg_fill
* 功    能: 填充发送到SLID_PROC的消息(ipc hdr & com msg hdr)
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_proc_send_msg_fill
(
CPSS_COM_TRUST_HEAD_T *pstSendTrustHdr,
UINT32 ulLen
)
{
    CPSS_COM_MSG_HEAD_T *pstSendMsg ;
    
    pstSendMsg = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstSendTrustHdr);
    
    /* 初始化可靠传输头 */      
    cpss_com_trust_hrd_pub_info_fill(pstSendTrustHdr);
    pstSendTrustHdr->usSliceLen = 0 ;
    
    pstSendMsg->ulMsgId     = COM_SLID_PROC_SEND_MSG ;
    pstSendMsg->ulLen       = ulLen ;
    pstSendMsg->ucAckFlag   = CPSS_COM_NOT_ACK ;
    pstSendMsg->ucPriFlag   = CPSS_COM_PRIORITY_NORMAL ;
    pstSendMsg->ucShareFlag = CPSS_COM_MEM_NOT_SHARE ;
    pstSendMsg->pucBuf      = CPSS_DISP_HDR_TO_USR_HDR(pstSendMsg) ;
    
    /*原地址填充*/
    cpss_com_logic_addr_get(&pstSendMsg->stSrcProc.stLogicAddr,&pstSendMsg->stSrcProc.ulAddrFlag) ;
    pstSendMsg->stSrcProc.ulPd = cpss_vk_pd_self_get() ;
    
    /*目的地址充*/
    pstSendMsg->stDstProc.stLogicAddr = pstSendMsg->stSrcProc.stLogicAddr;
    pstSendMsg->stDstProc.ulAddrFlag = pstSendMsg->stSrcProc.ulAddrFlag;
    pstSendMsg->stDstProc.ulPd = CPSS_COM_SLID_PROC ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_send
* 功    能: 发送数据给可靠传输纤程,提供可靠传输服务
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述  
* pvSendBuf           VOID            输入              发送数据指针
* ulMsgLen            UINT32          输入              发送数据长度
* pstMsg      CPSS_COM_MSG_HEAD_T*    输入              数据通信头指针
* pstSlidWin  CPSS_COM_SLID_WIN_T*    输入              滑窗对象的指针
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR                            
* 说   明: 和通信接口子模块的接口函数                                   
*******************************************************************************/
INT32 cpss_com_slid_send
(
VOID *pvSendBuf,                  /*app需要传输的消息指针*/
UINT32 ulMsgLen,                  /*app的消息长度*/
CPSS_COM_MSG_HEAD_T *pstMsg,      /*com msg hdr的指针*/
CPSS_COM_SLID_WIN_T *pstSlidWin   /*滑窗对象的指针*/
)
{
    CPSS_COM_TRUST_HEAD_T *pstSendProcHdr;
    SLID_PROC_SEND_MSG_T  *pstSlidProcSend;
    
    CPSS_COM_TRUST_HEAD_T *pstSliceTail;
    CPSS_COM_TRUST_HEAD_T *pstSliceHead;
    CPSS_COM_TRUST_HEAD_T *pstSliceTmp;

    CPSS_COM_LINK_T* pstLink ;
    CPSS_COM_MSG_HEAD_T* pstSendMsgHdr ;

    INT32 lRet;

#if 1
    /*判断滑窗状态,如果正常才能进行发送*/
    if((pstSlidWin == NULL)||(pstSlidWin->ucWinEnableFlag != SLID_STAT_NORMAL))
    {
        /*判断发送数据缓冲区大小是否满*/
        if(TRUE == cpss_com_store_buf_overflow(&pstMsg->stDstProc,ulMsgLen))
        {
            pstSlidWin->stSlidStat.ulStoreOverFlowNum++ ;
            return CPSS_ERR_COM_STORE_DATA_OVERFLOW ;
        }        
    }

    /*判断IPC消息个数是否小于一个余量，如果小于此余量，说明IPC消息已经少到危险的地步，有可能
    危及系统的正常运行，则不进行数据发送操作，直接将数据丢弃*/
    if(cpss_vk_ipc_msg_num_get() < CPSS_COM_IPC_MSG_FREE_NUM_MIN)
    {
        pstSlidWin->stSlidStat.ulIpcNotEnoughNum++ ;
        return CPSS_ERROR ;
    }

#endif
    
    /*申请发送到slid_proc的消息内存*/
    pstSendProcHdr = (CPSS_COM_TRUST_HEAD_T*)cpss_com_mem_alloc(sizeof(SLID_PROC_SEND_MSG_T));
    if(pstSendProcHdr == NULL)
    {
        pstSlidWin->stSlidStat.ulMemAllocFailNum++ ;
        return CPSS_ERR_COM_MEM_ALLOC_FAIL ;
    }
    
    /*申请待发送数据的内存*/
    pstSliceHead = (CPSS_COM_TRUST_HEAD_T*)cpss_com_frag_pkg_split(pvSendBuf , ulMsgLen , (UINT8**)&pstSliceTail) ;
    if( pstSliceHead == NULL )
    {
        cpss_com_mem_free(pstSendProcHdr) ;
        pstSlidWin->stSlidStat.ulSplitFailNum++ ;
        return CPSS_ERR_COM_COMPKG_FAIL ;
    }
    
    /*填充pstSlidProcSend消息结构*/
    cpss_com_slid_proc_send_msg_fill(pstSendProcHdr,sizeof(SLID_PROC_SEND_MSG_T)) ;
    
    pstLink = (CPSS_COM_LINK_T*)pstSlidWin->pucLinkObj ;

    pstSlidProcSend = (SLID_PROC_SEND_MSG_T*)CPSS_TRUST_HDR_TO_USR_HDR(pstSendProcHdr) ;
    pstSlidProcSend->pucSendBufHead = (UINT8*)pstSliceHead ;
    pstSlidProcSend->pucSendBufTail = (UINT8*)pstSliceTail ;
    pstSlidProcSend->pstSlidWin     = pstSlidWin ;

    pstSlidProcSend->stLogicAddr    = pstLink->stDstLogAddr ;
    pstSlidProcSend->ulAddrFlag     = pstLink->ulAddrFlag ;
    
    /*填充应用的消息ID和消息长度*/
    pstSlidProcSend->ulAppMsgId  = pstMsg->ulMsgId ;
    pstSlidProcSend->ulAppMsgLen = pstMsg->ulLen ; 
    
    /*填充所有拆好的包的包头*/
    pstSliceTmp = pstSliceHead ;
    
    cpss_mem_memcpy(CPSS_TRUST_HDR_TO_DISP_HDR(pstSliceTmp),(UINT8*)pstMsg,
                    CPSS_COM_DISP_HEAD_LEN);
    
    while( pstSliceTmp != NULL )
    {
        /*填充pSendHdr的包头*/

        cpss_com_trust_hrd_pub_info_fill(pstSliceTmp);
        
        pstSliceTmp = cpss_com_frag_next_slice_get((UINT8*)pstSliceTmp);
    }
    
    /*设置发送内存为共享内存*/
    pstSendMsgHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstSliceHead) ;
    pstSendMsgHdr->ucShareFlag = CPSS_COM_MEM_SHARE ;
    
#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
    {
        BOOL bTraceIt = cpss_trace_route(pstSliceHead);
        if (bTraceIt == TRUE)
        {
            cpss_trace_route_info_set(CPSS_TRACE_NODE_LOC_SRC, CPSS_TRACE_LAYER_APPI, pstMsg->ulMsgId, 0,0,0, (UINT8*)pstSliceHead);
        }
    }
#endif

    /*发送到slid_proc纤程*/
    lRet = cpss_com_ipc_msg_send(pstSendProcHdr,IPC_MSG_TYPE_REMOTE);
    if(CPSS_OK != lRet)
    {
#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
        cpss_trace_route_error_hook((UINT8*)pstSliceHead, CPSS_IPC_SEND_ERR);
#endif
        cpss_com_frag_pkg_free((UINT8*)pstSliceHead) ;
        cpss_com_mem_free(pstSendProcHdr) ;

        pstSlidWin->stSlidStat.ulIpcSendFailNum++ ;
        
        return lRet ;
    }
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_frame_send
* 功    能: 可靠传输帧的发送函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述      
* pstSlidWin   CPSS_COM_SLID_WIN_T*   输入              滑窗对象指针
* ucFrameKind         UINT8           输入              帧类型
* usSeq               UINT16          输入              发送数据帧帧序号
* usAck               UINT16          输入              接收滑窗的ACK序号
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR                            
* 说   明:  
*******************************************************************************/
INT32 cpss_com_slid_frame_send
(
 CPSS_COM_SLID_WIN_T *pstSlidWin, 
 UINT8 ucFrameKind, 
 UINT16 usSeq,
 UINT16 usAck
 )
{
    CPSS_COM_TRUST_HEAD_T *pstHdr = NULL ;
    CPSS_COM_MSG_HEAD_T   *pstDispMsg = NULL ;
    UINT8  aucCtrFrame[CPSS_COM_LINK_HEAD_LEN]; 
    UINT32 ulDataLen ;
    UINT32 ulWriteSize ;

    ulWriteSize = 0;
	
    switch(ucFrameKind)
    {
       case COM_SLID_PTYPE_SYN_REQ:
             pstSlidWin->ulSynReqSend++;
	break;

	case  COM_SLID_PTYPE_SYN_RES:
	    pstSlidWin->ulSynResSend++;
        break;
		
	case  COM_SLID_PTYPE_SYN_END:
            pstSlidWin->ulSynEndSend++;
	break;	

	 case COM_SLID_PTYPE_SYN_END_RES:
	 	pstSlidWin->ulSynEndResSend++;
	 break;
    }
	
    if(ucFrameKind == COM_SLID_PTYPE_CREDIBLE) 
    {
        pstHdr = (CPSS_COM_TRUST_HEAD_T*)pstSlidWin->pucSlidSendWin[CPSS_MOD(usSeq,COM_SLIDWIN_SIZE)] ;
        
        cpss_com_slid_hdr_ntoh(pstHdr) ;
        
        if(pstHdr->usSeq != usSeq)
        {
            /*增加统计*/
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
                "seq not equal! pstHdr->usSeq=%d,usSeq=%d\n",pstHdr->usSeq,usSeq) ;

            pstSlidWin->stSlidStat.ulResendRealSeq = pstHdr->usSeq ;
        }        
        
        /*重新填充ACK*/
        pstHdr->usAck = usAck ;
        cpss_com_slid_checksum_fill((UINT8*)pstHdr) ;
        
        ulDataLen = pstHdr->usSliceLen ;
        
        cpss_com_slid_hdr_hton(pstHdr) ;
        
        /*直接进行发送*/
        if(cpss_com_drv_write((VOID*)pstSlidWin->pucLinkObj,(UINT8*)pstHdr,ulDataLen,&ulWriteSize)<0)
        {
            return CPSS_ERROR ;
        }
        return CPSS_OK ;
    }
    
    pstHdr = (CPSS_COM_TRUST_HEAD_T*)aucCtrFrame ;

    memset(aucCtrFrame, 0, sizeof(aucCtrFrame));
    cpss_com_trust_hrd_pub_info_fill(pstHdr);
    pstHdr->usSliceLen = 0 ;
    
    cpss_com_slid_info_fill(ucFrameKind,aucCtrFrame,usSeq ,usAck) ;
    cpss_com_slid_checksum_fill(aucCtrFrame) ;
    
    /*转换字节序*/
    cpss_com_slid_hdr_hton((CPSS_COM_TRUST_HEAD_T*)aucCtrFrame) ;
    
    /*将通信DISP头的长度赋值为零*/
    pstDispMsg = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(aucCtrFrame) ;
    pstDispMsg->ulLen = 0 ;
    
    /*直接进行发送*/
    if(cpss_com_drv_write((VOID*)pstSlidWin->pucLinkObj,aucCtrFrame,CPSS_COM_LINK_HEAD_LEN,&ulWriteSize)<0)
    {
        return CPSS_ERROR ;
    }
    return CPSS_OK ;    
}

/*******************************************************************************
* 函数名称: cpss_com_slid_synreq_msg_send
* 功    能: 发送synreq消息到对应的滑窗对象
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                      
*******************************************************************************/
INT32 cpss_com_slid_synreq_msg_send
( 
 CPSS_COM_SLID_WIN_T *pstSlidWin
)
{
    INT32 lRet = CPSS_OK ;
    BOOL  bExist ;

    /*检查syn定时器是否已经关闭*/
    cpss_com_slid_timer_exist(SLID_SYN_TIMER_CLASS,&bExist,pstSlidWin) ;
    
    /*如果syn定时器没有关闭*/
    if(bExist == TRUE)
    {
        /*发送SYN消息*/
        lRet = cpss_com_slid_frame_send(pstSlidWin,COM_SLID_PTYPE_SYN_REQ,0,0);
    }

    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_link_data_frame_send
* 功    能: slid线程发送链表中的数据函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* pstSlidWin   CPSS_COM_SLID_WIN_T*   输入              滑窗对象的指针
* pstSliceNode     IPC_MSG_HDR_T*       输入              IPC消息指针    
* 函数返回: 
*          无                          
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_link_data_frame_send
(
CPSS_COM_SLID_WIN_T     *pstSlidWin, 
CPSS_COM_TRUST_HEAD_T   *pstSliceNode
)
{
    CPSS_COM_TRUST_HEAD_T  *pstSlicePtr ;
    UINT32 ulLen ;
    UINT32 ulWriteSize ;
    
    while( pstSliceNode!=NULL )
    {
        if(pstSlidWin->usSendWinBusyNum < COM_SLIDWIN_SIZE)
        {        
            /*填充数据的包头*/
            cpss_com_slid_win_info_fill(pstSliceNode,pstSlidWin ) ;
            
            pstSlicePtr = (CPSS_COM_TRUST_HEAD_T*)cpss_com_frag_slice_unlink((UINT8*)pstSliceNode);
            
            cpss_com_slid_checksum_fill((UINT8*)pstSliceNode) ;
            
            /*得到发送数据的长度*/
            ulLen = pstSliceNode->usSliceLen;
            
            /*转换字节序*/
            cpss_com_slid_hdr_hton(pstSliceNode);
            
            /*调用驱动进行发送*/
            if(cpss_com_drv_write((VOID*)(pstSlidWin->pucLinkObj),(CHAR*)pstSliceNode,ulLen,&ulWriteSize)<0)
            {
                /*增加统计*/
            }
            
            cpss_com_slid_sendwin_para_modify(pstSlidWin,pstSliceNode) ;
            pstSliceNode = pstSlicePtr ;
        }
        else
        {
            pstSlidWin->stSendBufManage.pstSlidSendBufLinkHead = pstSliceNode ;
            return ;
        }
    }
    pstSlidWin->stSendBufManage.pstSlidSendBufLinkHead = NULL ;    
}

/*******************************************************************************
* 函数名称: cpss_com_slid_data_send
* 功    能: slid线程发送数据函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述     
* pstSlidWin   CPSS_COM_SLID_WIN_T*   输入              滑窗对象的指针
* pucSliceHead        UINT8*          输入              发送数据链的头指针
* pucSliceTail        UINT8*          输入              发送数据链的尾指针
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR                            
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_data_send
(
 CPSS_COM_SLID_WIN_T *pstSlidWin, 
 UINT8* pucSliceHead, 
 UINT8* pucSliceTail,
 UINT32 ulSendNum
 )
{
    CPSS_COM_TRUST_HEAD_T   *pstSliceNode ;
        
    /*开始挂链*/
    cpss_com_slid_data_link_add(pstSlidWin,pucSliceHead,pucSliceTail) ;
    
    /*发送的可靠数据包数目统计*/
    pstSlidWin->stSlidStat.ulSendDataNum += ulSendNum ;

    pstSliceNode = pstSlidWin->stSendBufManage.pstSlidSendBufLinkHead ;
    
    if( (pstSliceNode!=NULL)&&(pstSlidWin->usSendWinBusyNum < COM_SLIDWIN_SIZE) )
    {
        /*停止ACK定时器*/
        cpss_com_slid_timer_stop(SLID_ACK_TIMER_CLASS,pstSlidWin) ;
    }
    
    if(pstSlidWin->usSendWinBusyNum == 0)
    {
    /*开启重发定时器*/
        cpss_com_slid_timer_start(SLID_RESEND_TIMER_CLASS,COM_SLID_RESEND_TIMEOUT,pstSlidWin) ;
    }
    
    /*开始拆链、发送数据、修改发送滑窗*/
    cpss_com_slid_link_data_frame_send(pstSlidWin, pstSliceNode) ;
    
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_link_data_send
* 功    能: slid纤程得知发送滑窗由满到不满，发送挂在链上的数据
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述   
* pstSlidWin   CPSS_COM_SLID_WIN_T*   输入              滑窗对象的指针 
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR                            
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_link_data_send
(
CPSS_COM_SLID_WIN_T *pstSlidWin
)
{
    CPSS_COM_TRUST_HEAD_T  *pstSliceNode = NULL;
    
    pstSliceNode = pstSlidWin->stSendBufManage.pstSlidSendBufLinkHead ;
    
    if(pstSliceNode == NULL)
    {
        return CPSS_OK ;
    }
    
    if(cpss_vos_sem_p (g_ulSlidSem, WAIT_FOREVER) != CPSS_OK)
    {
        return CPSS_ERROR ;
    }
    
    /*判断是否需要开启重发定时器*/
    if(pstSlidWin->usSendWinBusyNum == 0)
    {
        /*开启重发定时器*/
        cpss_com_slid_timer_start(SLID_RESEND_TIMER_CLASS,COM_SLID_RESEND_TIMEOUT,pstSlidWin) ;
    }
    
    if(pstSliceNode != NULL)
    {
        /*停止ACK定时器*/
        cpss_com_slid_timer_stop(SLID_ACK_TIMER_CLASS,pstSlidWin) ;
    }
    
    /*开始拆链、发送数据、修改发送滑窗*/
    cpss_com_slid_link_data_frame_send(pstSlidWin, pstSliceNode) ;
    
    cpss_vos_sem_v (g_ulSlidSem)    ;
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_data_deliver
* 功    能: 将接收的数据发送给高层
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述     
* pstPkg       COM_FRAG_PACKAGE_T*    输入              组包的包结构指针
* pstHdr       IPC_MSG_HDR_T*           输入              向应用提交的IPC消息
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR                            
* 说   明:                                    
*******************************************************************************/
INT32 cpss_com_slid_data_deliver
(
CPSS_COM_SLID_WIN_T *pstSlidWin,
CPSS_COM_TRUST_HEAD_T *pstHdr
)
{
    COM_FRAG_PACKAGE_T *pstPkg ;
    CPSS_COM_TRUST_HEAD_T *pstSendBuf = NULL ;
    CPSS_COM_MSG_HEAD_T *pstMsgHrd ;
    INT32 lRet;
    
    pstPkg = &(pstSlidWin->stPkg) ;
    
    pstSlidWin->stSlidStat.ulDeliverNum = pstSlidWin->stSlidStat.ulDeliverNum + 1 ;
    
    if((pstHdr==NULL)||(pstPkg==NULL))
    {
        return CPSS_ERROR ;
    }
    
    /*调用组包函数将数据组包*/
    lRet = cpss_com_frag_pkg_com( pstPkg,(UINT8*)pstHdr,(UINT8**)&pstSendBuf);
    if( CPSS_OK != lRet )
    {
        pstSlidWin->stSlidStat.ulComPkgFailNum = pstSlidWin->stSlidStat.ulComPkgFailNum + 1 ;
        return CPSS_ERR_COM_COMPKG_FAIL ;
    }
    
    if(pstSendBuf==NULL)
    {
        return CPSS_OK ;
    }
    
    pstMsgHrd = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstSendBuf) ;
    pstMsgHrd->pucBuf = CPSS_TRUST_HDR_TO_USR_HDR(pstSendBuf);

    /*增加统计*/
    cpss_com_spec_msg_stat(pstMsgHrd->ulMsgId,SLID_PROC_RECV_PHASE) ;    

#if 0
    lRet = cpss_com_send_extend(pstMsgHrd);
#else
    
    cpss_com_trace(pstMsgHrd) ;
    
    lRet = cpss_com_send_forward(pstMsgHrd,&pstMsgHrd->stDstProc) ;
#endif

    if(CPSS_OK != lRet)
    {
        pstSlidWin->stSlidStat.ulRecvDeliverErrNum = pstSlidWin->stSlidStat.ulRecvDeliverErrNum + 1;
        cpss_output(CPSS_MODULE_COM, CPSS_PRINT_WARN,
                    " Slid deliver failed. sourcePid = 0x%x, destPid = 0x%x.",
                    pstMsgHrd->stSrcProc.ulPd,
                    pstMsgHrd->stDstProc.ulPd);
        
    }
    else
    {
        /*滑窗接收到的可靠数据的统计*/
        pstSlidWin->stSlidStat.ulRecvDataNum = pstSlidWin->stSlidStat.ulRecvDataNum + 1 ;
    }
    
    /*如果发送失败则释放内存*/
    if(lRet!=CPSS_OK)
    {
        cpss_com_mem_free(pstSendBuf) ;
    }
    
    /*设置组包结构为空*/
    cpss_com_frag_null_set(pstPkg);

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_data_send_notify
* 功    能: 如果发送滑窗由满到不满则发送消息到slid纤程，激活纤程发送数据
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              需要通知的滑窗指针              
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR                            
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_data_send_notify
(
 CPSS_COM_SLID_WIN_T *pstSlidWin
 )
{
    CPSS_COM_TRUST_HEAD_T *pstSendProcHdr ;
    SLID_PROC_SEND_MSG_T  *pstSlidProcSend ;
    CPSS_COM_MSG_HEAD_T   *pstMsgHdr ;
    INT32 lRet;
    
    /*申请发送到slid_proc的消息内存*/
    pstSendProcHdr = (CPSS_COM_TRUST_HEAD_T*)cpss_com_mem_alloc(sizeof(SLID_PROC_SEND_MSG_T)) ;
    if(pstSendProcHdr == NULL)
    {
        return CPSS_ERROR ;
    }
    
    pstSlidProcSend = (SLID_PROC_SEND_MSG_T*)CPSS_TRUST_HDR_TO_USR_HDR(pstSendProcHdr) ;
    pstMsgHdr       = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstSendProcHdr) ;
    
    /*填充pstSlidProcSend消息结构*/
    pstSlidProcSend->pucSendBufHead = NULL ;
    pstSlidProcSend->pucSendBufTail = NULL ;
    pstSlidProcSend->pstSlidWin = pstSlidWin ;
    
    /*填充pstSendProcHdr的包头*/
    cpss_com_trust_hrd_pub_info_fill(pstSendProcHdr);
    pstSendProcHdr->usSliceLen = 0 ;
    
    /*原地址填充*/
    cpss_com_logic_addr_get(&pstMsgHdr->stSrcProc.stLogicAddr,&pstMsgHdr->stSrcProc.ulAddrFlag) ;
    pstMsgHdr->stSrcProc.ulPd = cpss_vk_pd_self_get() ;
    
    /*目的地址充*/
    cpss_com_slid_local_proc_pid_fill(&pstMsgHdr->stDstProc,CPSS_COM_SLID_PROC) ;
    
    pstMsgHdr->pucBuf = (UINT8*)pstSlidProcSend ;
    pstMsgHdr->ulLen  = sizeof(SLID_PROC_SEND_MSG_T) ;
    pstMsgHdr->ucAckFlag = CPSS_COM_NOT_ACK ;
    pstMsgHdr->ulMsgId   = COM_SLID_PROC_SEND_LINK_DATA_MSG ;
    
    /*发送到slid_proc纤程*/
    lRet = cpss_com_ipc_msg_send(pstSendProcHdr,IPC_MSG_TYPE_REMOTE);
    if( CPSS_OK != lRet )
    {
        cpss_com_mem_free(pstSendProcHdr) ;
        return CPSS_ERROR ;
    }
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_local_win_clear
* 功    能: 清除本地滑窗的数据
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述  
* pstSlidWin   CPSS_COM_SLID_WIN_T*   输入              待清空的滑窗指针             
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_local_win_clear
(
 CPSS_COM_SLID_WIN_T *pstSlidWin 
 )
{
    CPSS_COM_TRUST_HEAD_T *pstHdrNodeTmp ;
    CPSS_COM_TRUST_HEAD_T *pstHdrNode ;
    UINT16 usLoop ;
    UINT16 usAckExpectedFrame = 0 ;
    UINT16 usNextFrameToSend = 0 ;
    UINT32 ulClearNum = 0 ;
    VOID *pvLinkObj ;
    CPSS_SLID_STAT_T stSlidStat ;
    
    usAckExpectedFrame = pstSlidWin->usAckExpectedFrame ;
    usNextFrameToSend  = pstSlidWin->usNextFrameToSend ;
    
    /*释放发送窗口*/
    while(usAckExpectedFrame != usNextFrameToSend)
    {
        cpss_com_mem_free(pstSlidWin->pucSlidSendWin[CPSS_MOD(usAckExpectedFrame,COM_SLIDWIN_SIZE)]) ;
        usAckExpectedFrame = CPSS_MOD((usAckExpectedFrame+1),COM_SLID_PKGNUM_SIZE) ;
        
        ulClearNum = ulClearNum + 1 ;
    }
    
    /*释放接收窗口*/
    for(usLoop = 0; usLoop < COM_SLIDWIN_SIZE; usLoop++)
    {
        if( pstSlidWin->ucRecvWinFlag[usLoop] == 1 )
        {
            cpss_com_mem_free(pstSlidWin->pucSlidRecvWin[usLoop]) ;
            pstSlidWin->pucSlidRecvWin[usLoop] = NULL ;
            pstSlidWin->ucRecvWinFlag[usLoop] = 0 ;

            ulClearNum = ulClearNum + 1 ;
        }
    }
    
    /*清空发送缓冲区中的数据*/
    pstHdrNode = pstSlidWin->stSendBufManage.pstSlidSendBufLinkHead ;
    pstHdrNodeTmp = pstHdrNode ;
    
    while(pstHdrNode != NULL)
    {
        /*得到链表的下一个节点*/
        pstHdrNodeTmp = (CPSS_COM_TRUST_HEAD_T*)cpss_com_frag_next_slice_get((UINT8*)pstHdrNode);

        cpss_com_mem_free(pstHdrNode);

        pstHdrNode = pstHdrNodeTmp;

        ulClearNum = ulClearNum + 1 ;
    }
    
    /*清空组包的内存*/
    if(pstSlidWin->stPkg.pucSlice!=NULL)
    {
        cpss_com_mem_free(pstSlidWin->stPkg.pucSlice) ;
        ulClearNum = ulClearNum + 1 ;
    }
    
    pstSlidWin->stSlidStat.ulClearDataNum = 
        pstSlidWin->stSlidStat.ulClearDataNum + ulClearNum ;
    
    pvLinkObj = pstSlidWin->pucLinkObj;
    
    cpss_mem_memcpy((UINT8*)&stSlidStat,(UINT8*)&(pstSlidWin->stSlidStat),sizeof(CPSS_SLID_STAT_T)) ;
    cpss_mem_memset(pstSlidWin,0,sizeof(CPSS_COM_SLID_WIN_T));
    cpss_mem_memcpy((UINT8*)&(pstSlidWin->stSlidStat),(UINT8*)&stSlidStat,sizeof(CPSS_SLID_STAT_T)) ;
    
    pstSlidWin->pucLinkObj = pvLinkObj;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_ack_comm_deal
* 功    能: 接收到ACK的通用处理
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述    
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针
* usAckValue          UINT16          输入              接收到的ACK值
* 函数返回: 
*          无
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_ack_comm_deal
(
 CPSS_COM_SLID_WIN_T *pstSlidWin,
 UINT16 usAckValue
 )
{
    UINT16 usSeq ;
    UINT16 usAckExpectedFrame ;
    
    usSeq = pstSlidWin->usAckExpectedFrame ;
    usAckExpectedFrame = pstSlidWin->usAckExpectedFrame ;
    

    while(1 == cpss_com_slid_if_between(usAckExpectedFrame, usSeq, usAckValue))
    {
        cpss_com_mem_free(pstSlidWin->pucSlidSendWin[CPSS_MOD(usSeq,COM_SLIDWIN_SIZE)]) ;
        pstSlidWin->usSendWinBusyNum = pstSlidWin->usSendWinBusyNum - 1 ;
        
        pstSlidWin->pucSlidSendWin[CPSS_MOD(usSeq,COM_SLIDWIN_SIZE)] = NULL ;
        usSeq = CPSS_MOD(( usSeq+1 ),COM_SLID_PKGNUM_SIZE) ;
        
        /*发送成功的可靠数据片的统计*/
        pstSlidWin->stSlidStat.ulSendSliceNum = pstSlidWin->stSlidStat.ulSendSliceNum + 1 ;
    }
    pstSlidWin->usAckExpectedFrame = CPSS_MOD((usAckValue + 1),COM_SLID_PKGNUM_SIZE) ; 
}


/*******************************************************************************
* 函数名称: cpss_com_slid_recv_data_ack_dealing
* 功    能: 接收到数据中的ACK的处理流程
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针
* usAckValue          UINT16          输入              接收到的ACK值
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR                            
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_recv_data_ack_dealing
(
 CPSS_COM_SLID_WIN_T *pstSlidWin, 
 UINT16 usAckValue
 )
{
    UINT16 usLastFrameSend = 0 ;
    INT32  lRet;
    
    if(pstSlidWin->usSendWinBusyNum!=0)  /*如果发送窗不空，则处理ACK消息*/
    {
        usLastFrameSend = CPSS_MOD((pstSlidWin->usNextFrameToSend + COM_SLID_PKGNUM_SIZE - 1),COM_SLID_PKGNUM_SIZE) ;
        
        /*如果接收的ACK帧在发送滑窗范围之内，发送滑窗向前滑动，释放发送滑窗对应的内存*/
        lRet = cpss_com_slid_if_between( pstSlidWin->usAckExpectedFrame, usAckValue, usLastFrameSend);
        if(1 == lRet)
        {
            if(pstSlidWin->usSendWinBusyNum == COM_SLIDWIN_SIZE)
            {
                /*向slid_proc发送消息，继续发送链上的数据*/
                lRet = cpss_com_slid_data_send_notify(pstSlidWin);
                if( CPSS_OK != lRet )
                {
                    return CPSS_ERROR ;
                }
            }
            
            /*ACK的通用处理流程*/
            cpss_com_slid_ack_comm_deal(pstSlidWin,usAckValue) ;
            
            if(pstSlidWin->usSendWinBusyNum == 0)
            {
                /*停止数据重发定时器*/
                cpss_com_slid_timer_stop(SLID_RESEND_TIMER_CLASS,pstSlidWin) ;
            }
        }
        return CPSS_OK ;
    }
    
    /*停止数据重发定时器*/
    cpss_com_slid_timer_stop(SLID_RESEND_TIMER_CLASS,pstSlidWin) ;
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_recv_ack_dealing
* 功    能: 接收到单独ACK的处理流程
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* pstSlidWin   CPSS_COM_SLID_WIN_T*   输入              滑窗对象指针
* pstHdr         IPC_MSG_HDR_T*         输入              接收到的ACK控制帧
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR                            
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_recv_ack_dealing
(
CPSS_COM_SLID_WIN_T   *pstSlidWin, 
CPSS_COM_TRUST_HEAD_T *pstHdr
)
{
    UINT16 usLastFrameSend ;
    UINT16 usAckExpectedFrame ;
    UINT16 usAckValue ;
    INT32  lRet;
    
    usLastFrameSend    = 0 ;
    usAckExpectedFrame = 0 ;
    usAckValue         = 0 ;
    usAckValue = pstHdr->usAck ;
    
    /*通过滑窗发送数据时，先锁住链路对象*/
    lRet = cpss_vos_sem_p (g_ulSlidSem, WAIT_FOREVER);
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }
    
    pstSlidWin->stSlidStat.ulAckRecvNum = pstSlidWin->stSlidStat.ulAckRecvNum + 1 ;
    
    if( pstSlidWin->usSendWinBusyNum != 0 )  /*如果发送窗不空，则处理ACK消息*/
    {    
        usLastFrameSend = CPSS_MOD((pstSlidWin->usNextFrameToSend + COM_SLID_PKGNUM_SIZE - 1),COM_SLID_PKGNUM_SIZE) ;
        
        /*如果接收的ACK帧在发送滑窗范围之内，发送滑窗向前滑动，释放发送滑窗对应的内存*/
        lRet = cpss_com_slid_if_between( pstSlidWin->usAckExpectedFrame, usAckValue, usLastFrameSend);
        if( 1 == lRet )
        {
            if(pstSlidWin->usSendWinBusyNum == COM_SLIDWIN_SIZE)
            {
                /*向slid_proc发送消息，继续发送链上的数据*/
                lRet = cpss_com_slid_data_send_notify(pstSlidWin);
                if( CPSS_OK != lRet )
                {
                    pstSlidWin->stSlidStat.ulSendNotifyFailNum = pstSlidWin->stSlidStat.ulSendNotifyFailNum + 1 ;

                    cpss_vos_sem_v (g_ulSlidSem) ;
                    return CPSS_ERROR ;
                }
            }
            
            /*ACK的通用处理流程*/
            cpss_com_slid_ack_comm_deal(pstSlidWin,usAckValue) ;
            
            if( pstSlidWin->usSendWinBusyNum == 0 )
            {
                /*停止数据重发定时器*/
                cpss_com_slid_timer_stop(SLID_RESEND_TIMER_CLASS,pstSlidWin) ;
            }
        }
        
        /*应该释放发送滑窗的互斥锁*/
        cpss_vos_sem_v (g_ulSlidSem) ;
        return CPSS_OK ;
    }
    
    cpss_vos_sem_v (g_ulSlidSem) ;
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: slid_recv_expect_data
* 功    能: 接收的数据是期望接收的数据帧
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针
* pstHdr           IPC_MSG_HDR_T*       输入              接收的数据帧
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR                            
* 说   明: 
*******************************************************************************/
INT32 cpss_com_slid_expect_data_recv
(
CPSS_COM_SLID_WIN_T *pstSlidWin, 
CPSS_COM_TRUST_HEAD_T *pstHdr
)
{
    INT32 lRet = CPSS_OK ;

    /*将当前接收的数据帧向高层发送*/
    lRet = cpss_com_slid_data_deliver(pstSlidWin,pstHdr);

    if(CPSS_OK != lRet)
    {        
        return CPSS_ERROR ;
    }
    
    /*打开nak发送功能*/
    pstSlidWin->ucNakFlag = 0 ;
    
    /*打开ACK定时器*/
    cpss_com_slid_timer_start(SLID_ACK_TIMER_CLASS,COM_SLID_ACK_TIMEOUT,pstSlidWin) ;
    
    /*将接收滑窗向前移动*/
    cpss_com_slid_recv_win_forward(pstSlidWin) ;
    
    /*处理收到的连续滑窗帧*/
    while( pstSlidWin->ucRecvWinFlag[CPSS_MOD(pstSlidWin->usFrameExpected , COM_SLIDWIN_SIZE)] == 1)
    {        
        /*如果向上提交接收数据失败*/
        lRet = cpss_com_slid_data_deliver(pstSlidWin,(CPSS_COM_TRUST_HEAD_T *)pstSlidWin->pucSlidRecvWin[CPSS_MOD(pstSlidWin->usFrameExpected , COM_SLIDWIN_SIZE)]);
        if(CPSS_OK != lRet)
        {        
            /*重新置接收标志为未接收*/
            pstSlidWin->ucRecvWinFlag[CPSS_MOD(pstSlidWin->usFrameExpected , COM_SLIDWIN_SIZE)] = 0 ;
            
            pstSlidWin->pucSlidRecvWin[CPSS_MOD(pstSlidWin->usFrameExpected , COM_SLIDWIN_SIZE)] = NULL ;
            return CPSS_ERROR ;
        }
        /*如果向上提交接收数据成功，则接收滑窗向前滑动*/
        cpss_com_slid_recv_win_forward(pstSlidWin) ;

        /*如果接收的数据等于滑窗长度的一半则发送ACK响应*/
        pstSlidWin->ulRecvExpectDataNum++ ;
        if(pstSlidWin->ulRecvExpectDataNum >= COM_SLIDWIN_SIZE/2)
        {
            pstSlidWin->ulRecvExpectDataNum = 0 ;
            cpss_com_slid_ack_frame_send(pstSlidWin) ;
        }
    }
    return CPSS_OK ;   
}


/*******************************************************************************
* 函数名称: slid_recv_unexpect_data
* 功    能: 接收的数据不是期望接收的数据帧
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针
* pstHdr           IPC_MSG_HDR_T*       输入              接收的数据帧
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR                            
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_unexpect_data_recv
(
CPSS_COM_SLID_WIN_T   *pstSlidWin,
CPSS_COM_TRUST_HEAD_T *pstHdr
)
{
    INT32 lRet = CPSS_OK ;

    /*如果接收的数据不是期望接收的数据帧，向对端发送NAK消息，且只发一次*/
    if(pstSlidWin->ucNakFlag == 0)
    {
        lRet = cpss_com_slid_frame_send(pstSlidWin,COM_SLID_PTYPE_NAK ,pstSlidWin->usFrameExpected ,0);
        if(CPSS_OK != lRet)
        {
            cpss_com_mem_free(pstHdr) ;
            return CPSS_ERROR ;
        }
        pstSlidWin->ucNakFlag = 1 ;
    }
    else
    {
        /*打开ACK定时器*/
        cpss_com_slid_timer_start(SLID_ACK_TIMER_CLASS,COM_SLID_ACK_TIMEOUT,pstSlidWin) ;
    }
    
    /*判断接收的帧是否在接收滑窗的范围之内*/
    lRet = cpss_com_slid_if_between(pstSlidWin->usFrameExpected, pstHdr->usSeq, pstSlidWin->usTooFar);

    if(lRet == 1)
    {
        /*如果此帧从未接收过，将此帧入接收滑窗*/
        if(pstSlidWin->ucRecvWinFlag[CPSS_MOD(pstHdr->usSeq , COM_SLIDWIN_SIZE)] == 0)
        {
            pstSlidWin->pucSlidRecvWin[CPSS_MOD(pstHdr->usSeq , COM_SLIDWIN_SIZE)] = (UINT8*)pstHdr ;
            pstSlidWin->ucRecvWinFlag[CPSS_MOD(pstHdr->usSeq , COM_SLIDWIN_SIZE)] = 1 ;
        }
        /*如果此帧接收过，则释放接收的数据的内存*/
        else
        {
            cpss_com_mem_free(pstHdr) ;
            return CPSS_OK ;
        }
    }
    /*如果接收的帧不在接收滑窗的范围之内,则丢弃该帧*/
    else
    {
        pstSlidWin->stSlidStat.ulRecvDataNotInWin = pstSlidWin->stSlidStat.ulRecvDataNotInWin + 1 ;
        pstSlidWin->stSlidStat.ulRecvDataNotInWinSeq = pstHdr->usSeq ;

        cpss_com_mem_free(pstHdr) ;
        return CPSS_OK ;
    }
    
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_recv_data_dealing
* 功    能: 接收到可靠数据帧的处理流程
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针
* pstHdr           IPC_MSG_HDR_T*       输入              接收的数据帧
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR                            
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_recv_data_dealing
(
CPSS_COM_SLID_WIN_T   *pstSlidWin, 
CPSS_COM_TRUST_HEAD_T *pstHdr
)
{    
    INT32 lRet;

    CPSS_COM_MSG_HEAD_T* pstComHdr ;
    pstComHdr = (CPSS_COM_MSG_HEAD_T*)(pstHdr+1);
        
    lRet = cpss_vos_sem_p(g_ulSlidSem, WAIT_FOREVER);

    if(CPSS_OK != lRet)
    {
        cpss_com_mem_free(pstHdr) ;
        return CPSS_ERROR ;
    }
    
    /*对接收数据中的ACK序号进行处理*/
    lRet = cpss_com_slid_recv_data_ack_dealing( pstSlidWin, pstHdr->usAck);

    if(CPSS_OK != lRet)
    {
        cpss_vos_sem_v (g_ulSlidSem) ;
        cpss_com_mem_free(pstHdr) ;
        return CPSS_ERROR ;
    }
    
    /*如果收到的数据帧是当前期望接收的帧*/
    if(pstSlidWin->usFrameExpected == pstHdr->usSeq)
    {        
        lRet = cpss_com_slid_expect_data_recv(pstSlidWin,pstHdr);
        
        if(CPSS_OK != lRet)
        {
            cpss_vos_sem_v (g_ulSlidSem) ;
            return CPSS_ERROR ;
        }
        
        cpss_vos_sem_v (g_ulSlidSem) ;
        return CPSS_OK ;
    }
    else
    {
        /*如果接收的数据不是期望接收的数据帧，向对端发送NAK消息，且只发一次*/
        lRet = cpss_com_slid_unexpect_data_recv(pstSlidWin,pstHdr);

        if(CPSS_OK != lRet)
        {
            cpss_vos_sem_v (g_ulSlidSem) ;
            return CPSS_ERROR ;
        }
        cpss_vos_sem_v (g_ulSlidSem) ;
        return CPSS_OK ;
    }
}

/*******************************************************************************
* 函数名称: cpss_com_slid_recv_nak_dealing
* 功    能: 接收到NAK帧的处理流程
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针
* pstHdr           IPC_MSG_HDR_T*       输入              接收的NAK控制帧
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_recv_nak_dealing
(
CPSS_COM_SLID_WIN_T   *pstSlidWin, 
CPSS_COM_TRUST_HEAD_T *pstHdr
)
{
    UINT16 usSeq = 0 ;
    UINT16 usAck = 0 ;
    UINT16 usLastSeq ;
    UINT16 usAckExpectedFrame = 0 ;
    UINT16 usNextFrameToSend = 0 ;
    UINT16 usLastFrameSend = 0 ;
    INT32  lRet;

    usLastSeq = 0;
    
    usSeq = pstHdr->usSeq ;
    
    pstSlidWin->stSlidStat.ulSendNakDataNum = pstSlidWin->stSlidStat.ulSendNakDataNum + 1 ;
    pstSlidWin->stSlidStat.ulRecvNakNum = pstSlidWin->stSlidStat.ulRecvNakNum + 1 ;
    
    lRet = cpss_vos_sem_p (g_ulSlidSem, WAIT_FOREVER);
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }
    
    /*如果发送滑窗没有发送的数据*/
    if(pstSlidWin->usSendWinBusyNum == 0)
    {
        cpss_vos_sem_v (g_ulSlidSem) ;
        return CPSS_OK ;
    }
    
    usAckExpectedFrame = pstSlidWin->usAckExpectedFrame ;
    usNextFrameToSend  = pstSlidWin->usNextFrameToSend ;
    
    if(usSeq == usNextFrameToSend)
    {
        cpss_vos_sem_v (g_ulSlidSem) ;
        return CPSS_OK ;
    }
    
    usLastFrameSend = CPSS_MOD((usNextFrameToSend + COM_SLID_PKGNUM_SIZE - 1),COM_SLID_PKGNUM_SIZE) ;
    
    lRet = cpss_com_slid_if_between(usAckExpectedFrame,usSeq,usLastFrameSend);
    if(lRet != 1)
    {
        cpss_vos_sem_v (g_ulSlidSem) ;
        return CPSS_ERROR ;
    }
    
    usAck = CPSS_MOD((pstSlidWin->usFrameExpected + COM_SLID_PKGNUM_SIZE -1),COM_SLID_PKGNUM_SIZE) ;
    
    pstSlidWin->stSlidStat.ulRecvNakSeq = usSeq ;
    
    lRet = cpss_com_slid_frame_send( pstSlidWin, COM_SLID_PTYPE_CREDIBLE, usSeq ,usAck);
    if(CPSS_OK != lRet)
    {
        cpss_vos_sem_v (g_ulSlidSem) ;

        pstSlidWin->stSlidStat.ulSendNakDataFailNum = pstSlidWin->stSlidStat.ulSendNakDataFailNum + 1 ;
        
        return CPSS_ERROR ;
    }
    
    /*停止ACK定时器*/
    cpss_com_slid_timer_stop(SLID_ACK_TIMER_CLASS,pstSlidWin) ;
    
    cpss_vos_sem_v (g_ulSlidSem) ;
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_recv_synreq_dealing
* 功    能: 接收到滑窗同步请求的处理过程
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针  
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR
* 说   明:                                         
*******************************************************************************/
INT32 cpss_com_slid_recv_synreq_dealing
(
 CPSS_COM_SLID_WIN_T *pstSlidWin
)
{    
    INT32 lRet;

    if(g_ulSlidSem==0)
    {
        return CPSS_ERROR;
    }

    lRet = cpss_vos_sem_p (g_ulSlidSem, WAIT_FOREVER);
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }
    
    if(pstSlidWin->ucWinEnableFlag == SLID_STAT_INIT)
    {
        cpss_vos_sem_v (g_ulSlidSem) ;
        return CPSS_OK ;
    }
    else if(pstSlidWin->ucWinEnableFlag == SLID_STAT_SYN)
    {
        /*向对端发送synres消息*/
        lRet = cpss_com_slid_frame_send( pstSlidWin,COM_SLID_PTYPE_SYN_RES,0,0);
        if( CPSS_OK != lRet )
        {
            cpss_vos_sem_v (g_ulSlidSem) ;
            return CPSS_ERROR ;
        }

        /*向对端发送synreq消息*/
        cpss_com_slid_synreq_msg_send(pstSlidWin) ;
    }
    else if(pstSlidWin->ucWinEnableFlag == SLID_STAT_NORMAL)
    {
        /*清空本地滑窗*/
        cpss_com_slid_local_win_clear( pstSlidWin ) ;
        
        pstSlidWin->ucWinEnableFlag = SLID_STAT_SYN ;
        pstSlidWin->usTooFar = COM_SLIDWIN_SIZE -1 ;        
        
        /*启动SYN同步定时器*/
        cpss_com_slid_timer_start(SLID_SYN_TIMER_CLASS,COM_SLID_SYNREQ_TIMEOUT,pstSlidWin) ;
        
        /*发送SYN消息*/
        lRet = cpss_com_slid_frame_send( pstSlidWin,COM_SLID_PTYPE_SYN_REQ,0,0);
        if(lRet != CPSS_OK)
        { 
            cpss_vos_sem_v (g_ulSlidSem) ;
            return CPSS_ERROR ;
        }
    }
    else
    {
        cpss_vos_sem_v(g_ulSlidSem) ;
        return CPSS_ERROR ;
    }
    
    cpss_vos_sem_v(g_ulSlidSem) ;
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_recv_synreq_dealing
* 功    能: 接收到滑窗同步响应的处理过程
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针  
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_recv_synres_dealing
(
CPSS_COM_SLID_WIN_T *pstSlidWin
)
{
    INT32 lRet;

    lRet = cpss_vos_sem_p (g_ulSlidSem, WAIT_FOREVER);
    if(lRet != CPSS_OK)
    {
        return CPSS_ERROR ;
    }
    
    /*停止SYN定时器*/
    cpss_com_slid_timer_stop(SLID_SYN_TIMER_CLASS,pstSlidWin) ;
    
    /*打开SYN_END定时器*/
    cpss_com_slid_timer_start(SLID_SYN_END_TIMER_CLASS,COM_SLID_SYN_END_TIMEOUT,pstSlidWin) ;
    
    /*向对端发送syn end消息*/
    lRet = cpss_com_slid_frame_send( pstSlidWin,COM_SLID_PTYPE_SYN_END,0,0);
    if(lRet != CPSS_OK)
    {
        cpss_vos_sem_v (g_ulSlidSem) ;
        return CPSS_ERROR ;
    }
    
    cpss_vos_sem_v (g_ulSlidSem) ;
    
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_recv_synend_dealing
* 功    能: 接收到syn end的处理过程
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针  
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR
* 说   明:                                       
*******************************************************************************/
INT32 cpss_com_slid_recv_synend_dealing
(
CPSS_COM_SLID_WIN_T *pstSlidWin
)
{
    INT32 lRet;
    
    lRet = cpss_vos_sem_p(g_ulSlidSem, WAIT_FOREVER);
    if(lRet != CPSS_OK)
    {
        return CPSS_ERROR ;
    }
    
    /*设置滑窗状态为正常*/
    pstSlidWin->ucWinEnableFlag = SLID_STAT_NORMAL ;
    
    /*发送syn end res消息*/
    lRet = cpss_com_slid_frame_send( pstSlidWin,COM_SLID_PTYPE_SYN_END_RES,0,0);
    if(lRet != CPSS_OK)
    {
        cpss_vos_sem_v (g_ulSlidSem) ;
        return CPSS_ERROR ;
    }
    
    /*向对端发送synreq消息*/
 /*   cpss_com_slid_synreq_msg_send(pstSlidWin) ;*/
  
    cpss_vos_sem_v (g_ulSlidSem) ;
    
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_recv_synend_res_dealing
* 功    能: 接收到syn end的处理过程
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针  
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_recv_synend_res_dealing
(
CPSS_COM_SLID_WIN_T *pstSlidWin
)
{
    INT32 lRet;

    lRet = cpss_vos_sem_p (g_ulSlidSem, WAIT_FOREVER);
    if(lRet != CPSS_OK)
    {
        return CPSS_ERROR ;
    }
        /*设置滑窗状态为正常*/
    pstSlidWin->ucWinEnableFlag = SLID_STAT_NORMAL ;
    /*关闭SYN END请求滑窗*/
    cpss_com_slid_timer_stop(SLID_SYN_END_TIMER_CLASS,pstSlidWin) ;
    
    cpss_vos_sem_v (g_ulSlidSem) ;
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_recv_detect_dealing
* 功    能: 处理接收到的detect消息
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针  
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_recv_detect_dealing
(
CPSS_COM_SLID_WIN_T *pstSlidWin 
)
{
    INT32 lRet ;

    lRet = cpss_vos_sem_p (g_ulSlidSem, WAIT_FOREVER);
    if(lRet != CPSS_OK)
    {
        return ;
    }

    /*发送detect_res响应消息*/
    if(pstSlidWin->ucWinEnableFlag==SLID_STAT_INIT)
    {
        cpss_vos_sem_v (g_ulSlidSem);
        return ;
    }
    cpss_com_slid_frame_send(pstSlidWin,COM_SLID_PTYPE_DETECT_RES,0,0) ;

    cpss_vos_sem_v (g_ulSlidSem);
}

/*******************************************************************************
* 函数名称: cpss_com_slid_recv_detect_res_dealing
* 功    能: 处理接收到的detect响应消息
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针  
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_recv_detect_res_dealing
(
CPSS_COM_SLID_WIN_T *pstSlidWin
)
{
    INT32 lRet;

    lRet = cpss_vos_sem_p(g_ulSlidSem,WAIT_FOREVER);
    if(lRet != CPSS_OK)
    {
        return ;
    }
    
    /*停止DETECT重发定时器*/
    cpss_com_slid_timer_stop(SLID_DETECT_TIMER_CLASS,pstSlidWin) ;
    
    pstSlidWin->usDetectNum = 0 ;
    
    cpss_vos_sem_v(g_ulSlidSem) ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_recv_enable_deal
* 功    能: 激活滑窗的处理流程
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针
* pvLinkObj           VOID*           输入              链路对象指针
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_recv_enable_deal
(
CPSS_COM_SLID_WIN_T *pstSlidWin,
VOID *pvLinkObj
)
{            
    INT32 lRet;
    CPSS_COM_LINK_T* pstLink ;
    
    /*初始化滑窗信号量*/
    if(g_ulSlidSem==0)
    {
        cpss_com_slid_semaphore_init() ;
    }    
    
    lRet = cpss_vos_sem_p(g_ulSlidSem, WAIT_FOREVER);
    if(lRet != CPSS_OK)
    {
        return CPSS_ERROR ;
    }
    
    /*如果滑窗不处于初始态则返回成功*/
    if(pstSlidWin->ucWinEnableFlag != SLID_STAT_INIT)
    {
        cpss_vos_sem_v(g_ulSlidSem) ;
        return CPSS_OK ;
    }
    
    /*将路由管理模块的链路对象增加到滑窗模块中*/
    lRet = cpss_com_slid_node_insert((UINT32)pstSlidWin);
    if(lRet != CPSS_OK)
    {
        cpss_vos_sem_v(g_ulSlidSem) ;
        return CPSS_ERROR ;
    }
    
    /*启动同步定时器，向对端滑窗发送同步消息*/
    cpss_com_slid_local_win_clear( pstSlidWin ) ;
    
    pstSlidWin->pucLinkObj = (UINT8*)pvLinkObj ;

    pstLink = (CPSS_COM_LINK_T*)pvLinkObj ;

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL, 
                            "set the win's linkId(%d)PeerLogAddr(%x)Peer PhyAddr(%x)Peer AddrFlag(%d)",
                            pstLink->ulLinkId,  *(UINT32*)&pstLink->stDstLogAddr, *(UINT32*)&pstLink->stDstPhyAddr, pstLink->ulAddrFlag) ;
    
    cpss_com_slid_frame_send( pstSlidWin,COM_SLID_PTYPE_SYN_REQ,0,0 ) ;
    
    cpss_com_slid_timer_start(SLID_SYN_TIMER_CLASS,COM_SLID_SYNREQ_TIMEOUT,pstSlidWin) ;
    
    pstSlidWin->ucWinEnableFlag = SLID_STAT_SYN ;
    pstSlidWin->usTooFar = COM_SLIDWIN_SIZE -1 ;
    
    cpss_vos_sem_v(g_ulSlidSem) ;
    
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_slid_disable
* 功    能: 去激活滑窗的处理流程
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_disable
(
CPSS_COM_SLID_WIN_T *pstSlidWin
)
{
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_ack_timeout_deal
* 功    能: 接收到ACK消息超时处理
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针
* 函数返回: 
*          无
* 说   明:
*******************************************************************************/
VOID cpss_com_slid_ack_timeout_deal
(
CPSS_COM_SLID_WIN_T* pstSlidWin
)
{
    /*发送ACK帧*/
    UINT16 usAck;
    INT32  lRet;
    
    pstSlidWin->stSlidStat.ulAckSendNum = pstSlidWin->stSlidStat.ulAckSendNum + 1 ;

    usAck = CPSS_MOD((pstSlidWin->usFrameExpected + COM_SLID_PKGNUM_SIZE -1),COM_SLID_PKGNUM_SIZE) ;
    
    pstSlidWin->stSlidStat.ulAckSendValue = usAck ;    

    lRet = cpss_com_slid_frame_send( pstSlidWin, COM_SLID_PTYPE_ACK, 0 ,usAck);
    if(lRet != CPSS_OK)
    {
        /*增加统计*/
        pstSlidWin->stSlidStat.ulAckSendFailNum = pstSlidWin->stSlidStat.ulAckSendFailNum + 1 ;    
    }   

    cpss_com_slid_timer_stop(SLID_ACK_TIMER_CLASS,pstSlidWin) ; 
}

/*******************************************************************************
* 函数名称: cpss_com_slid_ack_frame_send
* 功    能: 发送ACK响应消息
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针
* 函数返回: 
*          无
* 说   明:
*******************************************************************************/
VOID cpss_com_slid_ack_frame_send
(
 CPSS_COM_SLID_WIN_T* pstSlidWin
 )
{
    /*发送ACK帧*/
    UINT16 usAck;
    INT32  lRet;
    
    pstSlidWin->stSlidStat.ulAckSendNum = pstSlidWin->stSlidStat.ulAckSendNum + 1 ;
    
    usAck = CPSS_MOD((pstSlidWin->usFrameExpected + COM_SLID_PKGNUM_SIZE -1),COM_SLID_PKGNUM_SIZE) ;
    
    pstSlidWin->stSlidStat.ulAckSendValue = usAck ;    
    
    lRet = cpss_com_slid_frame_send( pstSlidWin, COM_SLID_PTYPE_ACK, 0 ,usAck);
    if(lRet != CPSS_OK)
    {
        /*增加统计*/
        pstSlidWin->stSlidStat.ulAckSendFailNum = pstSlidWin->stSlidStat.ulAckSendFailNum + 1 ;
    }
}


/*******************************************************************************
* 函数名称: cpss_com_slid_resend_timeout_deal
* 功    能: 接收到RESEND消息超时处理
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针
* 函数返回: 
*          无
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_resend_timeout_deal
(
CPSS_COM_SLID_WIN_T* pstSlidWin
)
{
    UINT16 usAck;
    UINT16 usSeq;
    INT32  lRet;
    
    /*重新发送数据帧*/
    if(pstSlidWin->usSendWinBusyNum == 0)
    {
        /*停止重发定时器*/
        cpss_com_slid_timer_stop(SLID_RESEND_TIMER_CLASS,pstSlidWin) ;
        return ;
    }
    
    usAck = CPSS_MOD((pstSlidWin->usFrameExpected + COM_SLID_PKGNUM_SIZE -1),COM_SLID_PKGNUM_SIZE) ;
    usSeq = pstSlidWin->usAckExpectedFrame ;
    
    /*重新发送数据帧*/
    lRet = cpss_com_slid_frame_send(pstSlidWin,COM_SLID_PTYPE_CREDIBLE,usSeq,usAck);
    if(lRet != CPSS_OK)
    {
        /*增加统计*/
        pstSlidWin->stSlidStat.ulResendFailNum = pstSlidWin->stSlidStat.ulResendFailNum + 1 ;
    }
    
    /*停止ACK定时器*/
    cpss_com_slid_timer_stop(SLID_ACK_TIMER_CLASS,pstSlidWin) ;
    
    /* 重置RESEND定时器*/
    cpss_com_slid_timer_reset(SLID_RESEND_TIMER_CLASS,COM_SLID_RESEND_TIMEOUT_MIN,pstSlidWin) ;
    
    /*启动探测消息定时器*/
    cpss_com_slid_timer_start(SLID_DETECT_TIMER_CLASS,COM_SLID_DETECT_TIMEOUT,pstSlidWin) ;
    
    /*重发可靠数据的统计*/
    pstSlidWin->stSlidStat.ulResendNum = pstSlidWin->stSlidStat.ulResendNum + 1 ;   
    pstSlidWin->stSlidStat.ulResendNeedSeq = usSeq ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_syn_timeout_deal
* 功    能: 接收到SYN消息超时处理
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针
* 函数返回: 
*          无
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_syn_timeout_deal
(
CPSS_COM_SLID_WIN_T* pstSlidWin
)
{
    UINT16 usAck;
    UINT16 usSeq;
    INT32  lRet;
    
    /*重新发送synreq消息*/
    usAck = CPSS_MOD((pstSlidWin->usFrameExpected + COM_SLID_PKGNUM_SIZE -1),COM_SLID_PKGNUM_SIZE) ;
    usSeq = pstSlidWin->usAckExpectedFrame ;
    
    lRet = cpss_com_slid_frame_send(pstSlidWin, COM_SLID_PTYPE_SYN_REQ, usSeq ,usAck);
    if(lRet != CPSS_OK)
    {
        /*增加统计*/
    }
    
    /*重置SYN定时器*/
    cpss_com_slid_timer_reset(SLID_SYN_TIMER_CLASS,COM_SLID_SYNREQ_TIMEOUT,pstSlidWin) ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_detect_timeout_deal
* 功    能: 接收到DETECT消息超时处理
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针
* 函数返回: 
*          无
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_detect_timeout_deal
(
CPSS_COM_SLID_WIN_T* pstSlidWin
)
{
    /*发送DETECT消息*/
    cpss_com_slid_frame_send(pstSlidWin,COM_SLID_PTYPE_DETECT,0,0) ;
    
    /*记录DETECT消息发送次数*/
    pstSlidWin->usDetectNum  = pstSlidWin->usDetectNum + 1 ;
    
    if(pstSlidWin->usDetectNum >SLID_DETECT_RESEND_NUM )
    {
        cpss_com_slid_timer_stop(SLID_DETECT_TIMER_CLASS,pstSlidWin) ;
        
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "\nreset local win!detectNum(%d)",pstSlidWin->usDetectNum) ;        
        
        pstSlidWin->usDetectNum = 0 ;
        
        /*清空本地滑窗*/
        cpss_com_slid_local_win_clear( pstSlidWin ) ;
        
        pstSlidWin->ucWinEnableFlag = SLID_STAT_SYN ;
        pstSlidWin->usTooFar = COM_SLIDWIN_SIZE -1 ;
        
        /*增加滑窗复位次数的统计信息*/
        pstSlidWin->stSlidStat.ulResetWinNum = pstSlidWin->stSlidStat.ulResetWinNum + 1 ;
        
        /*启动SYN定时器*/
        cpss_com_slid_timer_start(SLID_SYN_TIMER_CLASS,COM_SLID_SYNREQ_TIMEOUT,pstSlidWin) ;
        
        /*发送SYN消息*/
        cpss_com_slid_frame_send(pstSlidWin,COM_SLID_PTYPE_SYN_REQ,0,0) ;  
        
        return ;
    }
    
    /*重置DETECT定时器*/
    cpss_com_slid_timer_reset(SLID_DETECT_TIMER_CLASS,COM_SLID_DETECT_TIMEOUT,pstSlidWin) ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_win_reset
* 功    能: 复位本地滑窗
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针
* 函数返回: 
*          无
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_win_reset
(
    CPSS_COM_SLID_WIN_T* pstSlidWin
)
{

    /*清空本地滑窗*/
    cpss_com_slid_local_win_clear( pstSlidWin ) ;
    
    pstSlidWin->ucWinEnableFlag = SLID_STAT_SYN ;
    pstSlidWin->usTooFar = COM_SLIDWIN_SIZE -1 ;
    
    /*增加滑窗复位次数的统计信息*/
    pstSlidWin->stSlidStat.ulResetWinNum = pstSlidWin->stSlidStat.ulResetWinNum + 1 ;
    
    /*启动SYN定时器*/
    cpss_com_slid_timer_start(SLID_SYN_TIMER_CLASS,COM_SLID_SYNREQ_TIMEOUT,pstSlidWin) ;
    
    /*发送SYN消息*/
    cpss_com_slid_frame_send(pstSlidWin,COM_SLID_PTYPE_SYN_REQ,0,0) ;  
    
}



/*******************************************************************************
* 函数名称: cpss_com_slid_synend_timeout_deal
* 功    能: 接收到SYN END消息超时处理
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针
* 函数返回: 
*          无
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_synend_timeout_deal
(
CPSS_COM_SLID_WIN_T* pstSlidWin
)
{
    /*发送SYN END消息*/
    cpss_com_slid_frame_send(pstSlidWin,COM_SLID_PTYPE_SYN_END,0,0) ;
    
    /*重置SYN_END定时器*/
    cpss_com_slid_timer_reset(SLID_SYN_END_TIMER_CLASS,COM_SLID_SYN_END_TIMEOUT,pstSlidWin) ;
}


/*以下代码不属于滑窗部分,属于数据发送部分*/


/*******************************************************************************
* 函数名称: cpss_com_if_store_overflow
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*          无
* 说   明:                                        
*******************************************************************************/
BOOL cpss_com_store_buf_overflow
(    
    CPSS_COM_PID_T *pstDstPid,
    UINT32 ulDataLen
)
{
    CPSS_COM_ROUTE_T* pstRoute ;
    CPSS_COM_STORE_NODE_T *pstStoreNode ;    

    pstRoute  = cpss_com_route_find(pstDstPid->stLogicAddr) ;
    if (NULL == pstRoute)
    {
        return (FALSE);
    }
    if(pstRoute->pstStoreMan!=NULL)
    {
        /*得到pstStoreNode节点*/
        pstStoreNode = &(pstRoute->pstStoreMan->stStoreNode[pstDstPid->ulAddrFlag]) ;
        
        /*判断发送的内存是否已经超出缓存的范围*/
        if((pstStoreNode->ulDataNum+1>CPSS_COM_SEND_BUF_NUM_MAX)||
            (pstStoreNode->ulDataSize+ulDataLen>CPSS_COM_SEND_BUF_LEN_MAX))
        {
            return TRUE ;
        }
    }

    return FALSE ;  
}


/*******************************************************************************
* 函数名称: cpss_com_send_to_store
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*          无
* 说   明:                                     
*******************************************************************************/
INT32 cpss_com_send_to_store
(
    CPSS_COM_MSG_HEAD_T *pstSendBuf,
    CPSS_COM_LOGIC_ADDR_T stLogAddr,
    UINT32 ulAddrFlag
)
{
    INT32 lRet ;
    CPSS_COM_TRUST_HEAD_T  *pstTrustHdr ;
    CPSS_COM_TRUST_HEAD_T  *pstTrustTail ;
    CPSS_COM_MSG_HEAD_T    *pstDispHdr ;
    CPSS_COM_MSG_HEAD_T    stDispHdr ;
    CPSS_COM_PID_T         stDstPid ;
    CPSS_COM_SEND_DATA_STOR_REQ_T stStorReq ;
    CPSS_COM_TRUST_HEAD_T *pstSliceTmp ;
     
    stDstPid.stLogicAddr = stLogAddr ;
    stDstPid.ulAddrFlag  = ulAddrFlag ;
     
    if(TRUE == cpss_com_store_buf_overflow(&stDstPid,pstSendBuf->ulLen))
    {
        return CPSS_ERR_COM_STORE_DATA_OVERFLOW ;
    }

    /*对数据进行拆包*/
    pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)cpss_com_frag_pkg_split(pstSendBuf->pucBuf,
        pstSendBuf->ulLen,(UINT8**)&pstTrustTail);
    if(NULL == pstTrustHdr)
    {
        return CPSS_ERR_COM_COMPKG_FAIL ;
    }

    /*在第一帧数据上复制数据分发头*/
    pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr);
    cpss_mem_memcpy((VOID*)pstDispHdr,(VOID*)pstSendBuf,sizeof(CPSS_COM_MSG_HEAD_T));
    pstDispHdr->ucShareFlag = CPSS_COM_MEM_SHARE ;
    
    pstSliceTmp = pstTrustHdr ;
    while( pstSliceTmp != NULL )
    {
        /*填充pSendHdr的包头*/
        
        cpss_com_trust_hrd_pub_info_fill(pstSliceTmp);
        
        pstSliceTmp = cpss_com_frag_next_slice_get((UINT8*)pstSliceTmp);
    }

    /*填写消息成员*/
    stStorReq.ulSendDataHdr  = (UINT32)pstTrustHdr ;
    stStorReq.ulSendDataTail = (UINT32)pstTrustTail ;
    stStorReq.ulAddrFlag     = ulAddrFlag ;
    stStorReq.stLogAddr      = stLogAddr ;

    /*填写通知消息的DISP部分*/
    stDispHdr.stDstProc.ulPd = CPSS_COM_SLID_PROC;

    stDispHdr.pucBuf    = (UINT8*)&stStorReq;
    stDispHdr.ucPriFlag = CPSS_COM_PRIORITY_NORMAL ;
    stDispHdr.ucShareFlag = CPSS_COM_MEM_NOT_SHARE ;
    stDispHdr.ulMsgId     = CPSS_COM_SEND_DATA_STOR_REQ_MSG ;
    stDispHdr.ulLen       = sizeof(CPSS_COM_SEND_DATA_STOR_REQ_T) ;
        
    /*将数据发送到cpss_com_slid_proc纤程*/
    lRet = cpss_com_send_local(&stDispHdr);
    
    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_data_store_timer_set
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称   类型     输入/输出         描述
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*******************************************************************************/
VOID cpss_com_data_store_timer_set
(
    CPSS_COM_PID_T *pstDstPid
)
{
    CPSS_COM_ROUTE_T *pstRoute ;
    CPSS_COM_LOGIC_ADDR_T stLogAddr ;
    CPSS_LOGIC_ADDR_GROUP_T *pstGroup ;
    CPSS_COM_STORE_NODE_T *pstStoreNode ;

    stLogAddr = pstDstPid->stLogicAddr ;
    pstRoute = cpss_com_route_find(stLogAddr);
    if (NULL == pstRoute)
    {
        return ;
    }
    pstGroup = (CPSS_LOGIC_ADDR_GROUP_T*)&stLogAddr.usGroup ;

    /*修改逻辑地址，使其能区分主备*/
    pstGroup->btRsv = pstDstPid->ulAddrFlag ;

    /*得到STORE_NODE结构*/
    pstStoreNode = &(pstRoute->pstStoreMan->stStoreNode[pstDstPid->ulAddrFlag]);

    if(pstStoreNode->ulStoreTmr == 0)
    {
        /*设置数据缓存定时器*/
        pstStoreNode->ulStoreTmr = cpss_timer_para_set(CPSS_COM_DATA_STORE_TM_NO,
            CPSS_COM_DATA_STORE_TM_INTERVAL,*(UINT32*)&stLogAddr);
    }
}

/*******************************************************************************
* 函数名称: cpss_com_data_store_timer_delete
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称   类型     输入/输出         描述
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*******************************************************************************/
VOID cpss_com_data_store_timer_delete
(
    CPSS_COM_PID_T *pstDstPid
)
{
    CPSS_COM_ROUTE_T *pstRoute ;
    CPSS_COM_STORE_NODE_T *pstStoreNode ;
    
    pstRoute = cpss_com_route_find(pstDstPid->stLogicAddr);
    if (NULL == pstRoute)
    {
        return ;
    }
    if(pstRoute->pstStoreMan == NULL)
    {
        return ;
    }

    /*得到STORE_NODE结构*/
    pstStoreNode = &(pstRoute->pstStoreMan->stStoreNode[pstDstPid->ulAddrFlag]);
    
    /*删除数据缓存定时器*/
    if(pstStoreNode->ulStoreTmr!=0)
    {
        cpss_timer_para_delete(pstStoreNode->ulStoreTmr);
        pstStoreNode->ulStoreTmr = 0 ;
    }
}

/*******************************************************************************
* 函数名称: cpss_com_data_store_timer_delete_real
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称   类型     输入/输出         描述
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*******************************************************************************/
VOID cpss_com_data_store_timer_delete_real
(
    CPSS_COM_PID_T *pstDstPid
)
{
    CPSS_COM_ROUTE_T *pstRoute ;
    CPSS_COM_STORE_NODE_T *pstStoreNode ;
    
    pstRoute = cpss_com_route_find_real(pstDstPid->stLogicAddr);
    if (NULL == pstRoute)
    {
        return ;
    }
    if(pstRoute->pstStoreMan == NULL)
    {
        return ;
    }

    /*得到STORE_NODE结构*/
    pstStoreNode = &(pstRoute->pstStoreMan->stStoreNode[pstDstPid->ulAddrFlag]);
    
    /*删除数据缓存定时器*/
    if(pstStoreNode->ulStoreTmr!=0)
    {
        cpss_timer_para_delete(pstStoreNode->ulStoreTmr);
        pstStoreNode->ulStoreTmr = 0 ;
    }
}

/*******************************************************************************
* 函数名称: cpss_com_store_data_send
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称   类型     输入/输出         描述
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*******************************************************************************/
INT32 cpss_com_store_data_send
(
    CPSS_COM_SLID_WIN_T*   pstSlidWin,
    CPSS_COM_STORE_NODE_T* pstStoreNode,
    CPSS_COM_TRUST_HEAD_T* pstTrustHdr,
    CPSS_COM_TRUST_HEAD_T* pstTrustTail
)
{
    
    /*如果发送缓存有数据*/
    if(pstStoreNode->apvNodeHdr!=NULL)
    {
        /*先发送缓存的数据*/
        cpss_com_slid_data_send(pstSlidWin,pstStoreNode->apvNodeHdr,
            pstStoreNode->apvNodeTail,pstStoreNode->ulDataNum);
    }

    /*将缓冲区挂链指针清零*/
    pstStoreNode->apvNodeHdr  = NULL ;
    pstStoreNode->apvNodeTail = NULL ;

    pstStoreNode->ulDataNum = 0 ;
    pstStoreNode->ulDataSize= 0 ;

    /*再发送本次需要发送的数据*/
    cpss_com_slid_data_send(pstSlidWin,
        (UINT8*)pstTrustHdr,(UINT8*)pstTrustTail,1);

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_store_data_send
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称   类型     输入/输出         描述
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*******************************************************************************/
CPSS_COM_STORE_NODE_T* cpss_com_store_node_get
(
    CPSS_COM_PID_T *pstDstPid
)
{
    UINT32 ulIndex ;
    CPSS_COM_ROUTE_T *pstRoute ;
    
    ulIndex = pstDstPid->ulAddrFlag ;
    pstRoute  = cpss_com_route_find(pstDstPid->stLogicAddr) ;
    if (NULL == pstRoute)
    {
        return (NULL);
    }
    /*如果缓存管理结构不存在则创建*/
    if(pstRoute->pstStoreMan == NULL)
    {
        pstRoute->pstStoreMan = cpss_mem_malloc(sizeof(CPSS_COM_DATA_STORE_MAN_T));
        if(NULL == pstRoute->pstStoreMan)
        {
            return NULL ;
        }
        
        /*清零申请的内存缓冲区*/
        cpss_mem_memset(pstRoute->pstStoreMan,0,sizeof(CPSS_COM_DATA_STORE_MAN_T));
    }
    
    return &pstRoute->pstStoreMan->stStoreNode[ulIndex] ;
}


/*******************************************************************************
* 函数名称: cpss_com_data_store_req_deal
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*          无                     
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_data_store_req_deal
(
    CPSS_COM_MSG_HEAD_T* pstDispHdr
)
{
    INT32 lRet ;
    CPSS_COM_PID_T stDestPid ;    
    CPSS_COM_SLID_WIN_T   *pstSlidWin  ;
    CPSS_COM_MSG_HEAD_T   *pstStoreDispHdr ;
    CPSS_COM_TRUST_HEAD_T *pstStoreHdr ;
    CPSS_COM_TRUST_HEAD_T *pstStoreTail;
    CPSS_COM_STORE_NODE_T *pstStoreNode;
    CPSS_COM_SEND_DATA_STOR_REQ_T  *pstStoreReq ;

    /*得到数据缓存请求结构体*/
    pstStoreReq = (CPSS_COM_SEND_DATA_STOR_REQ_T*)pstDispHdr->pucBuf ;

    pstStoreHdr  = (CPSS_COM_TRUST_HEAD_T*)pstStoreReq->ulSendDataHdr ;
    pstStoreTail = (CPSS_COM_TRUST_HEAD_T*)pstStoreReq->ulSendDataTail;

    /*得到缓存数据的目的地址*/
    stDestPid.stLogicAddr = pstStoreReq->stLogAddr ;
    stDestPid.ulAddrFlag  = pstStoreReq->ulAddrFlag ;
    
    pstStoreDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstStoreHdr);
    
    if(cpss_vos_sem_p(g_ulSlidSem, WAIT_FOREVER) != CPSS_OK)
    {
        /*增加统计*/

        /*释放要求发送的内存*/
        cpss_com_frag_pkg_free((UINT8*)pstStoreHdr);        
        return CPSS_ERROR ; 
    }

    /*通过逻辑地址得到滑窗对象*/
    cpss_com_link_win_get_by_logaddr((VOID**)&pstSlidWin,&stDestPid);

    /*得到STORE_NODE对象*/
    pstStoreNode = cpss_com_store_node_get(&stDestPid) ;
    if(NULL == pstStoreNode)
    {
        cpss_vos_sem_v(g_ulSlidSem) ;

        /*释放要求发送的内存*/
        cpss_com_frag_pkg_free((UINT8*)pstStoreHdr);
        return CPSS_ERROR ;
    }

    /*如果滑窗状态不正常*/
    if((NULL == pstSlidWin)||(SLID_STAT_NORMAL!=pstSlidWin->ucWinEnableFlag))
    {
        /*将数据链入发送缓冲区*/
        lRet = cpss_com_link_data_store(pstStoreNode,pstStoreHdr,pstStoreTail);
        if(CPSS_OK != lRet)
        {
            cpss_vos_sem_v(g_ulSlidSem) ;
            
            /*释放要求发送的内存*/
            cpss_com_frag_pkg_free((UINT8*)pstStoreHdr);
            return CPSS_ERROR ;
        }
        /*启动缓存定时器*/
        cpss_com_data_store_timer_set(&stDestPid);
    }
    /*如果滑窗状态正常*/
    else
    { 
         /*发送缓冲的数据*/
         lRet = cpss_com_store_data_send(pstSlidWin,pstStoreNode,
             pstStoreHdr,pstStoreTail);
         if(CPSS_OK != lRet)
         {
             /*增加统计*/
         }
         /*关闭超时定时器*/
         cpss_com_data_store_timer_delete(&stDestPid) ;
    }

    cpss_vos_sem_v(g_ulSlidSem) ;

    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_store_timeout_deal
* 功    能: 接收到缓存定时器超时消息的处理
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*          无                     
* 说   明:                                                                       
*******************************************************************************/
INT32 cpss_com_store_timeout_deal(CPSS_COM_MSG_HEAD_T *pstMsgHead)
{
    UINT32 ulAddrFlag ;
    CPSS_TIMER_MSG_T *pstTmrMsg ;
    CPSS_COM_LOGIC_ADDR_T stLogAddr ;
    CPSS_COM_ROUTE_T *pstRoute ;
    CPSS_LOGIC_ADDR_GROUP_T *pstGroup ;
    CPSS_COM_STORE_NODE_T *pstStoreNode ;
    
    pstTmrMsg = (CPSS_TIMER_MSG_T*)pstMsgHead->pucBuf ;
    cpss_mem_memcpy((VOID*)&stLogAddr,(VOID*)&pstTmrMsg->ulPara,sizeof(CPSS_COM_LOGIC_ADDR_T));
    pstGroup = (CPSS_LOGIC_ADDR_GROUP_T*)&stLogAddr.usGroup;

    /*从逻辑地址得到主备标志*/
    ulAddrFlag = pstGroup->btRsv ;

    /*将逻辑地址恢复*/
    if(ulAddrFlag == CPSS_COM_ADDRFLAG_SLAVE)
    {
        pstGroup->btRsv = 0 ;
    }
    
    if(cpss_vos_sem_p(g_ulSlidSem, WAIT_FOREVER) != CPSS_OK)
    {
        /*增加统计*/
        return CPSS_ERROR ; 
    }    
    
    /*找到对应的路由*/
    pstRoute = cpss_com_route_find(stLogAddr);
    if (NULL == pstRoute)
    {
        cpss_vos_sem_v(g_ulSlidSem) ;
        return CPSS_ERROR ; 
    }
    pstStoreNode = &(pstRoute->pstStoreMan->stStoreNode[ulAddrFlag]) ;

    /*释放对应路由表中的数据*/
    cpss_com_frag_pkg_free(pstStoreNode->apvNodeHdr);

    /*清空缓存节点中保存的链路信息*/
    pstStoreNode->apvNodeHdr = NULL ;
    pstStoreNode->apvNodeTail = NULL ;

    /*增加统计*/
    pstStoreNode->stStoreStat.ulTimeoutNum = pstStoreNode->stStoreStat.ulTimeoutNum + 1 ;
    pstStoreNode->stStoreStat.ulDiscardNum = 
        pstStoreNode->stStoreStat.ulDiscardNum + pstStoreNode->ulDataNum ;

    pstStoreNode->ulDataNum  = 0 ;
    pstStoreNode->ulDataSize = 0 ;
    pstStoreNode->ulStoreTmr = 0 ;

    cpss_vos_sem_v(g_ulSlidSem) ;
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_normal_ind_send
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*          无                     
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_normal_ind_send
(
    VOID* pvLinkAddr
)
{    
    CPSS_COM_LINK_T *pstLink ;
    CPSS_COM_SLID_NORMAL_IND_MSG_T stSlidIndMsg ;
    CPSS_COM_MSG_HEAD_T stComDispHdr ;

    pstLink = (CPSS_COM_LINK_T*)pvLinkAddr ;

    /*填写消息体结构*/
    stSlidIndMsg.stLogAddr  = pstLink->stDstLogAddr ;
    stSlidIndMsg.ulAddrFlag = pstLink->ulAddrFlag ;
	memset(&stComDispHdr, 0, sizeof(CPSS_COM_MSG_HEAD_T));
	
    /*填写通信头*/
    stComDispHdr.pucBuf = (UINT8*)&stSlidIndMsg ;
    stComDispHdr.stDstProc.ulPd = CPSS_COM_SLID_PROC ;
    stComDispHdr.ucAckFlag = CPSS_COM_NOT_ACK ;
    stComDispHdr.ucPriFlag = CPSS_COM_PRIORITY_NORMAL ;
    stComDispHdr.ucShareFlag = 0 ;
    stComDispHdr.ulLen   = sizeof(CPSS_COM_SLID_NORMAL_IND_MSG_T) ;
    stComDispHdr.ulMsgId = CPSS_COM_SLID_NORMAL_IND_MSG ;

    /*向本地的link_proc发送滑窗状态正常消息*/
    while(cpss_com_send_local(&stComDispHdr)!=CPSS_OK)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
            "send to local CPSS_COM_SLID_NORMAL_IND_MSG failed(%d)",pstLink->ulLinkId) ;   
        continue;
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_send_req_recv_deal
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*          无                     
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_send_req_recv_deal
(
    CPSS_COM_MSG_HEAD_T *pstMsgHdr
)
{
    INT32 lRet ;
    SLID_PROC_SEND_MSG_T* pstProcSendMsg ;
    CPSS_COM_SLID_WIN_T * pstSlidWin ;
    CPSS_COM_MSG_HEAD_T * pstDispHdr ;
    CPSS_COM_PID_T  stDstPid ;
    CPSS_COM_STORE_NODE_T *pstStoreNode ;
    CPSS_COM_TRUST_HEAD_T *pstDataHead ;
    CPSS_COM_TRUST_HEAD_T *pstDataTail ;
    
    pstProcSendMsg = (SLID_PROC_SEND_MSG_T*)pstMsgHdr->pucBuf;
    pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(
        pstProcSendMsg->pucSendBufHead);

    stDstPid.stLogicAddr = pstProcSendMsg->stLogicAddr ;
    stDstPid.ulAddrFlag  = pstProcSendMsg->ulAddrFlag ;

    pstDataHead = (CPSS_COM_TRUST_HEAD_T*)pstProcSendMsg->pucSendBufHead ;
    pstDataTail = (CPSS_COM_TRUST_HEAD_T*)pstProcSendMsg->pucSendBufTail ;

    if(cpss_vos_sem_p(g_ulSlidSem, WAIT_FOREVER) != CPSS_OK)
    {
        /*增加统计*/

        cpss_com_frag_pkg_free((UINT8*)pstDataHead) ;
        return CPSS_ERROR ; 
    }

    pstStoreNode = cpss_com_store_node_get(&stDstPid) ;

    /*得到滑窗对象*/
    lRet = cpss_com_link_win_get_by_logaddr((VOID*)&pstSlidWin,&stDstPid) ;
    if(lRet != CPSS_OK)
    {
        cpss_vos_sem_v(g_ulSlidSem) ;
        cpss_com_frag_pkg_free((UINT8*)pstDataHead) ;
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,"get slid obj failed! destLogAddr=%x,ulAddrFlag=%d\n",
            *(UINT32*)&stDstPid.stLogicAddr,stDstPid.ulAddrFlag) ;
        return CPSS_ERROR ;
    }
    
    /*如果滑窗状态不正常*/
    if(SLID_STAT_NORMAL != pstSlidWin->ucWinEnableFlag)
    {
        /*将数据链入发送缓冲区*/
        lRet = cpss_com_link_data_store(pstStoreNode,pstDataHead,pstDataTail);
        if(CPSS_OK != lRet)
        {
            cpss_vos_sem_v(g_ulSlidSem) ;
            
            /*释放要求发送的内存*/
            cpss_com_frag_pkg_free((UINT8*)pstProcSendMsg->pucSendBufHead);
            return CPSS_ERROR ;
        }
        /*启动数据缓存超时定时器*/
        cpss_com_data_store_timer_set(&stDstPid) ;
    }
    /*如果滑窗状态正常*/
    else
    {
        if(pstSlidWin->ulStoreFlag == CPSS_COM_STORE_NOT_HAS_SEND_FLAG)
        {
            /*发送缓冲的数据*/
            lRet = cpss_com_store_data_send(pstSlidWin,pstStoreNode,pstDataHead,pstDataTail);

            pstSlidWin->ulStoreFlag = CPSS_COM_STORE_HAS_SEND_FLAG ;
        }
        else
        {
            /*直接调用滑窗发送待发的数据*/
            lRet = cpss_com_slid_data_send(pstSlidWin,
                (UINT8*)pstDataHead, (UINT8*)pstDataTail,1);
        }
    }
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,"send slid data failed.") ;   
    }

    cpss_vos_sem_v(g_ulSlidSem) ;
    
    return lRet ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_normal_ind_recv_deal
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*          无                     
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_normal_ind_recv_deal
(
    CPSS_COM_MSG_HEAD_T *pstMsgHdr
)
{
    INT32 lRet ;
    CPSS_COM_PID_T stDestPid; 
    CPSS_COM_SLID_WIN_T *pstSlidWin ; 
    CPSS_COM_STORE_NODE_T *pstStoreNode ;
    CPSS_COM_SLID_NORMAL_IND_MSG_T *pstSlidNormalInd ;

    pstSlidNormalInd = (CPSS_COM_SLID_NORMAL_IND_MSG_T*)pstMsgHdr->pucBuf ;
    
    stDestPid.stLogicAddr = pstSlidNormalInd->stLogAddr ;
    stDestPid.ulAddrFlag  = pstSlidNormalInd->ulAddrFlag ;

    if(cpss_vos_sem_p(g_ulSlidSem, WAIT_FOREVER) != CPSS_OK)
    {
        /*增加统计*/
        return CPSS_ERROR ; 
    }
    
    /*得到滑窗对象*/
    lRet = cpss_com_link_win_get_by_logaddr((VOID*)&pstSlidWin,&stDestPid) ;
    if(lRet != CPSS_OK)
    {
        cpss_vos_sem_v(g_ulSlidSem) ;
        return CPSS_ERROR ;
    }

    cpss_output(CPSS_MODULE_COM, CPSS_PRINT_DETAIL,
        "Slid Proc Recv a SlidCreate Msg, LinkObj = %#x, LogAddr = %#x.",
        pstSlidWin->pucLinkObj, *(UINT32*)&pstSlidNormalInd->stLogAddr); 
    
    /*如果滑窗状态不正常,则返回失败*/
    if(pstSlidWin->ucWinEnableFlag!=SLID_STAT_NORMAL)
    {
        cpss_vos_sem_v(g_ulSlidSem) ;
        return CPSS_ERROR ;
    }

    /*得到pstStorNode结构*/
    pstStoreNode = cpss_com_store_node_get(&stDestPid) ;

    /*发送路由表中主备链路上的数据*/
    cpss_com_slid_data_send(pstSlidWin,pstStoreNode->apvNodeHdr,
        pstStoreNode->apvNodeTail,pstStoreNode->ulDataNum);  

    /*清空store_node结构*/
    pstStoreNode->apvNodeHdr  = NULL ;
    pstStoreNode->apvNodeTail = NULL ;
    pstStoreNode->ulDataNum   = 0 ;
    pstStoreNode->ulDataSize  = 0 ;

    cpss_vos_sem_v(g_ulSlidSem) ;   

    cpss_output(CPSS_MODULE_COM, CPSS_PRINT_DETAIL,
        "Slid Proc Recv a SlidCreate Msg, Dealing End."); 
    
    return CPSS_OK ;
}

/*数据缓存代码结束*/


/*******************************************************************************
* 函数名称: cpss_slip_timer
* 功    能: 滑窗定时器任务
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
UINT32 g_ulCpssComSlidtimerTaskPList[1];
INT32 cpss_com_slidtimer_task_init()
{
    /*如果可靠传输信号量未初始化，则循环等待*/
    while(g_ulSlidSem==0)
    {
        cpss_vos_task_delay(1000) ;
    }
    g_ulCpssComSlidtimerTaskPList[0] = 1;
    
    return (CPSS_OK);
}

INT32 cpss_com_slidtimer_task_block(UINT32 *pulParamList)
{
    cpss_vos_task_delay(pulParamList[0]);
    
    return (CPSS_OK);
}

INT32 cpss_com_slidtimer_task_dealing(INT32 lRet, UINT32 *pulParamList)
{
    COM_SLID_LINK_NODE_T *pstNode = NULL ;
    CPSS_COM_SLID_WIN_T *pstSlidWin ;
    UINT16 usSeq ;
    UINT16 usAck ;

    usSeq = 0 ;
    usAck = 0 ;

    if(cpss_vos_sem_p(g_ulSlidSem, WAIT_FOREVER) != CPSS_OK)
    {
        /*增加统计*/
        return (CPSS_ERROR) ;
    }
   
    pstNode = g_stSlidWinLink.pstHead ;
    
    /*扫描所有链路上的重发和ACK定时器*/
    while(pstNode != NULL )
    {
        pstSlidWin = (CPSS_COM_SLID_WIN_T*)(pstNode->ulData) ;
        
        /*如果ACK定时器有效且已经超时则发送ACK帧*/
        if( cpss_com_slid_if_timeout(SLID_ACK_TIMER_CLASS, pstSlidWin) == TRUE )
        {
            cpss_com_slid_ack_timeout_deal(pstSlidWin) ;
        }
        
        /*如果RESEND定时器有效且已经超时则发送RESEND帧*/
        if( cpss_com_slid_if_timeout(SLID_RESEND_TIMER_CLASS, pstSlidWin) == TRUE )
        {
            cpss_com_slid_resend_timeout_deal(pstSlidWin) ;
        }
        
        /*如果synreq定时器有效且已经超时则发送synreq消息到对端*/
        if(cpss_com_slid_if_timeout(SLID_SYN_TIMER_CLASS, pstSlidWin)==TRUE)
        {
            cpss_com_slid_syn_timeout_deal(pstSlidWin) ;
            
        }
        
        /*如果DETECT重发定时器超时*/
        if(cpss_com_slid_if_timeout(SLID_DETECT_TIMER_CLASS, pstSlidWin)==TRUE)
        {
            cpss_com_slid_detect_timeout_deal(pstSlidWin) ;
        }
        
        /*如果SYN END重发定时器超时*/
        if(cpss_com_slid_if_timeout(SLID_SYN_END_TIMER_CLASS, pstSlidWin)==TRUE)
        {
            cpss_com_slid_synend_timeout_deal(pstSlidWin) ;
        }
        
        pstNode = pstNode->pstNext ;
    }
    cpss_vos_sem_v(g_ulSlidSem) ;

    return (CPSS_OK);
    
}
INT32 cpss_com_slidtimer_task_reg()
{
    INT32 lRet;
    lRet = cpss_vos_ntask_register(
              CPSS_NORMAL_TASK_TYPE_SEM,
              cpss_com_slidtimer_task_init, 
              cpss_com_slidtimer_task_block,
              cpss_com_slidtimer_task_dealing,
              g_ulCpssComSlidtimerTaskPList,
              CPSS_SLIDTIMER_TASKNAME);
    if ( lRet != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                "SlidTimer task register failed! errCode = %#x \n", lRet);
        return (CPSS_ERROR);
    }

    return (CPSS_OK);
    
}

VOID cpss_com_slid_timer_task(VOID)
{
    COM_SLID_LINK_NODE_T *pstNode = NULL ;
    CPSS_COM_SLID_WIN_T *pstSlidWin ;
    UINT16 usSeq ;
    UINT16 usAck ;

    usSeq = 0 ;
    usAck = 0 ;
    
    /*如果可靠传输信号量未初始化，则循环等待*/
    while(g_ulSlidSem==0)
    {
        cpss_vos_task_delay(10) ;
    }
    
    while(1)
    {        
        cpss_kw_set_task_run() ;
        
        if(cpss_vos_sem_p(g_ulSlidSem, WAIT_FOREVER) != CPSS_OK)
        {
            /*增加统计*/
            continue ;
        }
        pstNode = g_stSlidWinLink.pstHead ;
        
        /*扫描所有链路上的重发和ACK定时器*/
        while(pstNode != NULL )
        {
            pstSlidWin = (CPSS_COM_SLID_WIN_T*)(pstNode->ulData) ;
            
            /*如果ACK定时器有效且已经超时则发送ACK帧*/
            if( cpss_com_slid_if_timeout(SLID_ACK_TIMER_CLASS,pstSlidWin) == TRUE )
            {
                cpss_com_slid_ack_timeout_deal(pstSlidWin) ;
            }
            
            /*如果RESEND定时器有效且已经超时则发送RESEND帧*/
            if( cpss_com_slid_if_timeout(SLID_RESEND_TIMER_CLASS,pstSlidWin) == TRUE )
            {
                cpss_com_slid_resend_timeout_deal(pstSlidWin) ;
            }
            
            /*如果synreq定时器有效且已经超时则发送synreq消息到对端*/
            if(cpss_com_slid_if_timeout(SLID_SYN_TIMER_CLASS,pstSlidWin)==TRUE)
            {
                cpss_com_slid_syn_timeout_deal(pstSlidWin) ;
                
            }
            
            /*如果DETECT重发定时器超时*/
            if(cpss_com_slid_if_timeout(SLID_DETECT_TIMER_CLASS,pstSlidWin)==TRUE)
            {
                cpss_com_slid_detect_timeout_deal(pstSlidWin) ;
            }
            
            /*如果SYN END重发定时器超时*/
            if(cpss_com_slid_if_timeout(SLID_SYN_END_TIMER_CLASS,pstSlidWin)==TRUE)
            {
                cpss_com_slid_synend_timeout_deal(pstSlidWin) ;
            }
            
            pstNode = pstNode->pstNext ;
        }
        cpss_vos_sem_v(g_ulSlidSem) ;
        
#ifdef CPSS_VOS_VXWORKS
        cpss_kw_set_task_stop() ;

        taskDelay(1) ;
#endif
        
#ifdef CPSS_VOS_WINDOWS
        cpss_kw_set_task_stop() ;

        Sleep(1);
#endif

#ifdef CPSS_VOS_LINUX
        cpss_kw_set_task_stop() ;

        cpss_delay(1);
#endif
    }
}


/*******************************************************************************
* 函数名称: cpss_udp_slid_recv
* 功    能: 可靠传输的接收函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* pstSlidWin    CPSS_COM_SLID_WIN_T*  输入              滑窗对象指针 
* pstHdr          IPC_MSG_HDR_T*        输入              接收到的数据指针
* 函数返回: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR
* 说   明:                                
*******************************************************************************/
INT32 cpss_com_slid_recv
(
CPSS_COM_SLID_WIN_T   *pstSlidWin, 
CPSS_COM_TRUST_HEAD_T *pstHdr
)
{ 
    INT32 lRet=CPSS_OK ;
    
    /*对接收的数据进行检查*/
    if((pstSlidWin == NULL)||(pstHdr == NULL))
    {
        if(pstHdr != NULL)
        {
            cpss_com_mem_free(pstHdr) ;
        }
        return CPSS_ERROR ;
    }
    
    /*对接收到的数据帧进行字节序转换*/
    cpss_com_slid_hdr_ntoh(pstHdr);
    
    if(pstSlidWin->ucWinEnableFlag == SLID_STAT_INIT)
    {
        cpss_com_mem_free(pstHdr) ;
        return CPSS_ERROR ;
    }
    
    if(pstSlidWin->ucWinEnableFlag != SLID_STAT_NORMAL)
    {
    
        if((pstHdr->ucPType == COM_SLID_PTYPE_NAK)||
            (pstHdr->ucPType == COM_SLID_PTYPE_ACK)||
            (pstHdr->ucPType == COM_SLID_PTYPE_CREDIBLE))
        {
            cpss_com_mem_free(pstHdr) ;

            pstSlidWin->stSlidStat.ulRecvNotNormal = pstSlidWin->stSlidStat.ulRecvNotNormal + 1 ;

            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,"recv data when win status not normal,seq=%d,ack=%d,ptype=%d\n",
                pstHdr->usSeq,pstHdr->usAck,pstHdr->ucPType) ;
            return CPSS_ERROR ;
        }
    }
    
    /*对接收到的帧进行和校验*/
    lRet = cpss_com_slid_sum_check(pstHdr);
    if(lRet != CPSS_OK)
    {
        cpss_com_mem_free(pstHdr) ;

        pstSlidWin->stSlidStat.ulSumCheckErrNum = pstSlidWin->stSlidStat.ulSumCheckErrNum + 1 ;

        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,"sum check failed,seq=%d,ack=%d,ptype=%d,sliceLen=%d\n",
            pstHdr->usSeq,pstHdr->usAck,pstHdr->ucPType,pstHdr->usSliceLen) ;
        return CPSS_ERROR ;
    }  
    
    switch (pstHdr->ucPType)
    {
        /*收到可靠数据的处理过程*/
    case COM_SLID_PTYPE_CREDIBLE :
        {
            /*滑窗状态正常,才能接收数据*/
            if(pstSlidWin->ucWinEnableFlag == SLID_STAT_NORMAL)
            {
                lRet = cpss_com_slid_recv_data_dealing( pstSlidWin, pstHdr);
                if(lRet != CPSS_OK)
                {
                    pstSlidWin->stSlidStat.ulRecvDataDealErrNum = pstSlidWin->stSlidStat.ulRecvDataDealErrNum + 1 ;
                    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,"cpss_com_slid_recv_data_dealing failed,seq=%d,ack=%d,ptype=%d,sliceLen=%d\n",
                        pstHdr->usSeq,pstHdr->usAck,pstHdr->ucPType,pstHdr->usSliceLen) ;
                    return CPSS_ERROR ;
                }
            }
		else
		{
		 pstSlidWin->ulAbNormalRecv++;
		#if 0
		   cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,"cpss_com_slid_recv_data_dealing @ SLID_STAT_SYN,recv seq=%d,FrameExpected=%d,ptype=%d,sliceLen=%d\n",
                        pstHdr->usSeq,pstSlidWin->usFrameExpected,pstHdr->ucPType,pstHdr->usSliceLen) ;
             		/* 请求重发*/
		   cpss_vos_sem_p(g_ulSlidSem, WAIT_FOREVER);
		   cpss_com_slid_unexpect_data_recv(pstSlidWin,pstHdr);
		   pstSlidWin->ulAbNormalRecv++;
		   cpss_vos_sem_v (g_ulSlidSem) ;

		 #endif
		}
            break ;
        }
        /*收到滑窗ACK消息的处理过程*/
    case COM_SLID_PTYPE_ACK :
        {    
            lRet = cpss_com_slid_recv_ack_dealing( pstSlidWin, pstHdr);
            if(lRet != CPSS_OK)
            {
                pstSlidWin->stSlidStat.ulRecvAckDealErrNum = pstSlidWin->stSlidStat.ulRecvAckDealErrNum + 1 ;

                cpss_com_mem_free(pstHdr) ;
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,"cpss_com_slid_recv_ack_dealing failed,seq=%d,ack=%d,ptype=%d,sliceLen=%d\n",
                    pstHdr->usSeq,pstHdr->usAck,pstHdr->ucPType,pstHdr->usSliceLen) ;                
                return CPSS_ERROR ;
            }
            cpss_com_mem_free(pstHdr) ;            
            break ;
        }
        /*收到滑窗对齐请求消息的处理过程*/
    case COM_SLID_PTYPE_SYN_REQ :
        {    
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"recv win msg : synreq.") ;

	     pstSlidWin->ulSynReqRecv++;

            lRet = cpss_com_slid_recv_synreq_dealing(pstSlidWin);

            if(lRet != CPSS_OK)
            {
                cpss_com_mem_free(pstHdr) ;
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,"cpss_com_slid_recv_synreq_dealing failed,seq=%d,ack=%d,ptype=%d,sliceLen=%d\n",
                    pstHdr->usSeq,pstHdr->usAck,pstHdr->ucPType,pstHdr->usSliceLen) ;                
                return CPSS_ERROR ;
            }
    
            cpss_com_mem_free(pstHdr) ;            
            break ;
        }
        /*收到滑窗对齐请求响应消息的处理过程*/
    case COM_SLID_PTYPE_SYN_RES :
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"recv win msg : synrsp.") ;

            pstSlidWin->ulSynResRecv++;
            lRet = cpss_com_slid_recv_synres_dealing(pstSlidWin);
            if(lRet != CPSS_OK)
            {
                cpss_com_mem_free(pstHdr) ;   
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,"cpss_com_slid_recv_synres_dealing failed,seq=%d,ack=%d,ptype=%d,sliceLen=%d\n",
                    pstHdr->usSeq,pstHdr->usAck,pstHdr->ucPType,pstHdr->usSliceLen) ;                
                return CPSS_ERROR ;
            }
            cpss_com_mem_free(pstHdr) ;            
            break ;
        }
        /*收到NAK消息的处理过程*/
    case COM_SLID_PTYPE_NAK :
        {            
            lRet = cpss_com_slid_recv_nak_dealing(pstSlidWin, pstHdr);
            if(lRet != CPSS_OK)
            {
                pstSlidWin->stSlidStat.ulRecvNakDealErrNum = pstSlidWin->stSlidStat.ulRecvNakDealErrNum + 1 ;
                
                cpss_com_mem_free(pstHdr) ;
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,"cpss_com_slid_recv_nak_dealing failed,seq=%d,ack=%d,ptype=%d,sliceLen=%d\n",
                    pstHdr->usSeq,pstHdr->usAck,pstHdr->ucPType,pstHdr->usSliceLen) ;                
                return CPSS_ERROR ;
            }
            cpss_com_mem_free(pstHdr) ;            
            break ;
        }
        /*接收到DETECT消息*/
    case COM_SLID_PTYPE_DETECT :
        {
            cpss_com_slid_recv_detect_dealing(pstSlidWin) ;
            cpss_com_mem_free(pstHdr) ;            
            break ;
        }
        /*接收到DETECT响应消息*/
    case COM_SLID_PTYPE_DETECT_RES:
        {
            cpss_com_slid_recv_detect_res_dealing(pstSlidWin) ;
            cpss_com_mem_free(pstHdr) ;            
            break ;
        }
        /*接收到SYN_END消息*/
    case COM_SLID_PTYPE_SYN_END:
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"recv win msg : synend.") ;
            
	     pstSlidWin->ulSynEndRecv++;	
            cpss_com_slid_recv_synend_dealing(pstSlidWin) ;

            /*向slid_proc纤程发送滑窗状态正常通知消息*/
            cpss_com_slid_normal_ind_send(pstSlidWin->pucLinkObj) ;

            cpss_com_mem_free(pstHdr) ;   
            break ;
        }
        /*接收到SYN_END res消息*/
    case COM_SLID_PTYPE_SYN_END_RES:
        {  
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"recv win msg : synend rsp.") ;

            pstSlidWin->ulSynEndResRecv++;
			
            cpss_com_slid_recv_synend_res_dealing(pstSlidWin) ;

	    /*向slid_proc纤程发送滑窗状态正常通知消息*/
            cpss_com_slid_normal_ind_send(pstSlidWin->pucLinkObj) ;
            cpss_com_mem_free(pstHdr) ;
            break ;
        }
        
    default:
        {
            pstSlidWin->stSlidStat.ulRecvUnKnownNum = pstSlidWin->stSlidStat.ulRecvUnKnownNum + 1 ;
            
            cpss_com_mem_free(pstHdr) ;            
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,"recv unknown msg,seq=%d,ack=%d,ptype=%d,sliceLen=%d\n",
                pstHdr->usSeq,pstHdr->usAck,pstHdr->ucPType,pstHdr->usSliceLen) ;                
            return CPSS_ERROR ;
        }
    }
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_slid_proc
* 功    能: 滑窗的纤程处理函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* usUserState         UINT16          输入              用户纤程的状态
* pvVar               VOID*           输入              纤程的全局地址
* pstMsgHead    CPSS_COM_MSG_HEAD_T*  输入              纤程接收到的数据    
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_slid_proc
(  
 UINT16  usUserState,
 VOID *  pvVar,
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
    SLID_PROC_SEND_MSG_T *pstProcSendMsg;
    INT32 lRet;
    
    switch(pstMsgHead->ulMsgId)
    {
    case COM_SLID_PROC_SEND_MSG :
        {     
            /*如果是发送数据请求，则进行发送数据操作*/
            lRet = cpss_com_slid_send_req_recv_deal(pstMsgHead) ;
            if(lRet != CPSS_OK)
            {
                /*增加统计*/
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,"cpss_com_slid_send_req_recv_deal failed\n") ;
            }
            break ;
        }
    case COM_SLID_PROC_SEND_LINK_DATA_MSG :
        {
            /*发送链上的所有数据*/    
            pstProcSendMsg = (SLID_PROC_SEND_MSG_T*)pstMsgHead->pucBuf;
            
            lRet = cpss_com_slid_link_data_send(pstProcSendMsg->pstSlidWin);
            if(lRet != CPSS_OK)
            {
                /*增加统计*/
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,"cpss_com_slid_link_data_send failed\n") ;
            }
            
            break ;
        }
        
    case IPC_MSG_ID_ACTIVE: 
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "\nslid proc recv active msg") ;
            
            break ;
        }
        
    case COM_SLIE_WIN_ENABLE_MSG:
        {            
            pstProcSendMsg = (SLID_PROC_SEND_MSG_T*)pstMsgHead->pucBuf ;

            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"recv slid win enable msg.") ;

            lRet = cpss_com_slid_recv_enable_deal(pstProcSendMsg->pstSlidWin,
                                                  pstProcSendMsg->pvLinkObj);
            if(lRet != CPSS_OK)
            {
                    /*增加统计*/
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,"cpss_com_slid_recv_enable_deal failed\n") ;
            }
            break;
        }
    case CPSS_COM_SEND_DATA_STOR_REQ_MSG:
        {
            /*对接收到的数据缓存请求进行处理*/
            lRet = cpss_com_data_store_req_deal(pstMsgHead);

            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"recv STORE REQ msg.dstAddr=0x%x,addrflag=%d,pid=0x%x,srcAddr=0x%x,addrFlag=%d,pid=0x%x",
                *(UINT32*)&pstMsgHead->stDstProc.stLogicAddr,pstMsgHead->stDstProc.ulAddrFlag,pstMsgHead->stDstProc.ulPd,*(UINT32*)&pstMsgHead->stSrcProc.stLogicAddr,
                pstMsgHead->stSrcProc.ulAddrFlag,pstMsgHead->stSrcProc.ulPd) ;

            if(lRet != CPSS_OK)
            {
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,"deal STORE REQ msg failed.") ;
                /*增加统计*/
            }
            break;
        }
    case CPSS_COM_DATA_STORE_TM_MSG:
        {
            /*处理缓存数据超时消息*/
            cpss_com_store_timeout_deal(pstMsgHead) ;

            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"recv STORE TIMEOUT msg.");
            
            break;
        }
    case CPSS_COM_SLID_NORMAL_IND_MSG:
        {
            /*处理链路正常通知消息*/
            cpss_com_slid_normal_ind_recv_deal(pstMsgHead) ;

            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"recv SLID NORMAL IND msg.");

            break ;
        }
    default:
        {
            break ;
        }
    }
}


/*******************************************************************************
* 函数名称: cpss_com_slid_init
* 功    能: 链路的滑窗初始化函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* pvLinkObj           void*           输入              链路对象的指针
* pstSlidWin   CPSS_COM_SLID_WIN_T*   输入              待初始化滑窗的指针          
* 函数返回: 
*          成功：CPSS_OK
*          失败：CPSS_ERROR                     
* 说   明: 
*******************************************************************************/
INT32 cpss_com_slid_init
(
 void* pvLinkObj,
 CPSS_COM_SLID_WIN_T *pstSlidWin
 )
{
    SLID_PROC_SEND_MSG_T* pstWinEnableMsg ;
    CPSS_COM_MSG_HEAD_T*  pstComHdr ;
    CPSS_COM_PID_T       stSlidProcPid ;
    
    CPSS_COM_TRUST_HEAD_T* pstSendBuf ;
    
    /*填充滑窗纤程ID*/
    cpss_com_logic_addr_get(&stSlidProcPid.stLogicAddr,&stSlidProcPid.ulAddrFlag) ;
    stSlidProcPid.ulPd = CPSS_COM_SLID_PROC ;
    
    /*申请发送的内存*/
    pstSendBuf = cpss_com_mem_alloc(sizeof(SLID_PROC_SEND_MSG_T)) ;
    if(pstSendBuf == NULL)
    {
        return CPSS_ERROR ;
    }
    
    /*填充IPC消息的公共信息*/
    cpss_com_trust_hrd_pub_info_fill(pstSendBuf);
    pstSendBuf->usSliceLen = 0 ;
    
    /*填充滑窗激活消息*/    
    pstWinEnableMsg = (SLID_PROC_SEND_MSG_T*)CPSS_TRUST_HDR_TO_USR_HDR(pstSendBuf) ;
    pstWinEnableMsg->pstSlidWin = pstSlidWin ;
    pstWinEnableMsg->pvLinkObj  = pvLinkObj ;
    pstWinEnableMsg->pucSendBufHead = 0 ;
    pstWinEnableMsg->pucSendBufTail = 0 ;

    pstSlidWin->pucLinkObj = pvLinkObj ;
    
    /*填充COM头的信息*/
    pstComHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstSendBuf) ;
    pstComHdr->pucBuf = (UINT8*)pstWinEnableMsg ;
    pstComHdr->stDstProc = stSlidProcPid ;
    pstComHdr->ucAckFlag = CPSS_COM_NOT_ACK ;
    pstComHdr->ucShareFlag =  CPSS_COM_MEM_NOT_SHARE ;
    pstComHdr->ulMsgId     = COM_SLIE_WIN_ENABLE_MSG ;
    pstComHdr->ulLen = sizeof(SLID_PROC_SEND_MSG_T) ;
    
    cpss_com_logic_addr_get(&pstComHdr->stSrcProc.stLogicAddr,&pstComHdr->stSrcProc.ulAddrFlag) ;
    pstComHdr->stSrcProc.ulPd = cpss_vk_pd_self_get() ;
    
    if(cpss_com_ipc_msg_send(pstSendBuf,IPC_MSG_TYPE_REMOTE) != CPSS_OK)
    {
        cpss_com_mem_free(pstSendBuf) ;
        return CPSS_ERROR ;
    }
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_link_delete
* 功    能: 删除滑窗链表中对应的滑窗项
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* ulLinkId            UINT32          输入              链路ID
* 函数返回: 
*          无                     
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_slid_link_delete
(
    UINT32 ulLinkAddr
)
{
    /*加锁*/
    if(cpss_vos_sem_p(g_ulSlidSem, WAIT_FOREVER) != CPSS_OK)
    {
        return CPSS_ERROR ;
    }
    
    /*删除链表中的对应项*/
    cpss_com_slid_node_remove(ulLinkAddr) ;

    cpss_vos_sem_v(g_ulSlidSem) ;

    return CPSS_OK ;

}

/*******************************************************************************
* 函数名称: cpss_com_slid_recv_win_data_num_get
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* ulLinkId            UINT32          输入              链路ID
* 函数返回: 
*          无                     
* 说   明:                                                                             
*******************************************************************************/
UINT32 cpss_com_slid_recv_win_data_num_get(CPSS_COM_SLID_WIN_T *pstSlidWin)
{
    UINT32 ulLoop ;
    UINT32 ulRecvWinDataNum = 0 ;

    for(ulLoop=0;ulLoop<COM_SLIDWIN_SIZE;ulLoop++)
    {
        if (pstSlidWin->ucRecvWinFlag[0] == 1)
        {
            ulRecvWinDataNum++ ;
        }
    }

    return ulRecvWinDataNum ;
}

/*******************************************************************************
* 函数名称: cpss_com_slid_show
* 功    能: 滑窗的测试查看函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* ulLinkId            UINT32          输入              链路ID
* 函数返回: 
*          无                     
* 说   明:                                                                    
*******************************************************************************/
VOID cpss_com_slid_show(CPSS_COM_SLID_WIN_T *pstSlidWin)
{
    
    /*可靠传输定时器信息*/
    cps__oams_shcmd_printf("synreqTimerEnableFlag =%d\n",pstSlidWin->stSynreqTimer.ulEnableFlag) ;   
    cps__oams_shcmd_printf("synendTimerEnableFlag =%d\n",pstSlidWin->stSynEndTimer.ulEnableFlag) ;
    cps__oams_shcmd_printf("detectTimerEnableFlag =%d\n",pstSlidWin->stDetectTimer.ulEnableFlag) ;
    cps__oams_shcmd_printf("resendTimerEnableFlag =%d\n",pstSlidWin->stResendTimer.ulEnableFlag) ;
    
    /*可靠传输滑窗参数信息*/
    cps__oams_shcmd_printf("ucWinEnableFlag=%d\n",pstSlidWin->ucWinEnableFlag) ;
    cps__oams_shcmd_printf("usDetectNum=%d\n",pstSlidWin->usDetectNum) ;
    cps__oams_shcmd_printf("usFrameExpected=%d\n",pstSlidWin->usFrameExpected) ;
    cps__oams_shcmd_printf("usNextFrameToSend=%d\n",pstSlidWin->usNextFrameToSend) ;
    cps__oams_shcmd_printf("usSendWinBusyNum=%d\n",pstSlidWin->usSendWinBusyNum) ;
    cps__oams_shcmd_printf("usAckExpectedFrame=%d\n",pstSlidWin->usAckExpectedFrame) ;
    cps__oams_shcmd_printf("usTooFar=%d\n",pstSlidWin->usTooFar) ;
    
    /*可靠传输数据收发统计信息*/
    cps__oams_shcmd_printf("ulSendDataNum=%d\n",pstSlidWin->stSlidStat.ulSendDataNum) ;
    cps__oams_shcmd_printf("ulRecvDataNum=%d\n",pstSlidWin->stSlidStat.ulRecvDataNum) ;
    cps__oams_shcmd_printf("ulSendSliceNum=%d\n",pstSlidWin->stSlidStat.ulSendSliceNum) ;
    cps__oams_shcmd_printf("ulResendNum=%d\n",pstSlidWin->stSlidStat.ulResendNum) ;
    cps__oams_shcmd_printf("ulResetWinNum=%d\n",pstSlidWin->stSlidStat.ulResetWinNum) ; 
    cps__oams_shcmd_printf("ulRecvDeliverErrNum=%d\n",pstSlidWin->stSlidStat.ulRecvDeliverErrNum) ; 

    cps__oams_shcmd_printf("ulResendFailNum=%d\n",pstSlidWin->stSlidStat.ulResendFailNum) ;
    cps__oams_shcmd_printf("ulResendNeedSeq=%d\n",pstSlidWin->stSlidStat.ulResendNeedSeq) ;
    cps__oams_shcmd_printf("ulResendRealSeq=%d\n",pstSlidWin->stSlidStat.ulResendRealSeq) ;
    cps__oams_shcmd_printf("ulSendNakDataNum=%d\n",pstSlidWin->stSlidStat.ulSendNakDataNum) ;
    cps__oams_shcmd_printf("ulSendNakDataFailNum=%d\n",pstSlidWin->stSlidStat.ulSendNakDataFailNum) ;
    cps__oams_shcmd_printf("ulRecvNakSeq=%d\n",pstSlidWin->stSlidStat.ulRecvNakSeq) ;
    cps__oams_shcmd_printf("ulRecvDataNotInWin=%d\n",pstSlidWin->stSlidStat.ulRecvDataNotInWin) ;
    cps__oams_shcmd_printf("ulRecvDataNotInWinSeq=%d\n",pstSlidWin->stSlidStat.ulRecvDataNotInWinSeq) ;
    cps__oams_shcmd_printf("ulComPkgFailNum=%d\n",pstSlidWin->stSlidStat.ulComPkgFailNum);
    cps__oams_shcmd_printf("ulDeliverNum=%d\n",pstSlidWin->stSlidStat.ulDeliverNum);
    cps__oams_shcmd_printf("ulSendNotifyFailNum=%d\n",pstSlidWin->stSlidStat.ulSendNotifyFailNum);
    cps__oams_shcmd_printf("ulAckSendNum=%d\n",pstSlidWin->stSlidStat.ulAckSendNum);
    cps__oams_shcmd_printf("ulAckSendFailNum=%d\n",pstSlidWin->stSlidStat.ulAckSendFailNum) ;
    cps__oams_shcmd_printf("ulAckRecvNum=%d\n",pstSlidWin->stSlidStat.ulAckRecvNum);
    cps__oams_shcmd_printf("ulAckSendValue=%d\n",pstSlidWin->stSlidStat.ulAckSendValue);
    cps__oams_shcmd_printf("ulRecvNakNum=%d\n",pstSlidWin->stSlidStat.ulRecvNakNum);
    cps__oams_shcmd_printf("ulClearDataNum=%d\n",pstSlidWin->stSlidStat.ulClearDataNum);
    cps__oams_shcmd_printf("ulRecvNotNormal=%d\n",pstSlidWin->stSlidStat.ulRecvNotNormal);
    cps__oams_shcmd_printf("ulSumCheckErrNum=%d\n",pstSlidWin->stSlidStat.ulSumCheckErrNum) ;
    cps__oams_shcmd_printf("ulRecvDataDealErrNum=%d\n",pstSlidWin->stSlidStat.ulRecvDataDealErrNum) ;
    cps__oams_shcmd_printf("ulRecvAckDealErrNum=%d\n",pstSlidWin->stSlidStat.ulRecvAckDealErrNum) ;
    cps__oams_shcmd_printf("ulRecvNakDealErrNum=%d\n",pstSlidWin->stSlidStat.ulRecvNakDealErrNum);
    cps__oams_shcmd_printf("ulRecvUnKnownNum=%d\n",pstSlidWin->stSlidStat.ulRecvUnKnownNum);

    cps__oams_shcmd_printf("  ulStoreOverFlowNum    =%d\n",pstSlidWin->stSlidStat.ulStoreOverFlowNum);
    cps__oams_shcmd_printf("  ulIpcNotEnoughNum     =%d\n",pstSlidWin->stSlidStat.ulIpcNotEnoughNum);
    cps__oams_shcmd_printf("  ulMemAllocFailNum     =%d\n",pstSlidWin->stSlidStat.ulMemAllocFailNum);
    cps__oams_shcmd_printf("  ulSplitFailNum        =%d\n",pstSlidWin->stSlidStat.ulSplitFailNum);
    cps__oams_shcmd_printf("  ulIpcSendFailNum      =%d\n",pstSlidWin->stSlidStat.ulIpcSendFailNum);

    cps__oams_shcmd_printf("\n\r\n\r");
    cps__oams_shcmd_printf("  ulSynReqSend      =%d\n",pstSlidWin->ulSynReqSend);	
    cps__oams_shcmd_printf("  ulSynResSend      =%d\n",pstSlidWin->ulSynResSend);
    cps__oams_shcmd_printf("  ulSynEndSend      =%d\n",pstSlidWin->ulSynEndSend);
    cps__oams_shcmd_printf("  ulSynEndResSend      =%d\n",pstSlidWin->ulSynEndResSend);		
    cps__oams_shcmd_printf("  ulSynReqRecv      =%d\n",pstSlidWin->ulSynReqRecv);	
    cps__oams_shcmd_printf("  ulSynResRecv      =%d\n",pstSlidWin->ulSynResRecv);
    cps__oams_shcmd_printf("  ulSynEndRecv      =%d\n",pstSlidWin->ulSynEndRecv);	
    cps__oams_shcmd_printf("  ulSynEndResRecv      =%d\n",pstSlidWin->ulSynEndResRecv);	  
    cps__oams_shcmd_printf("  ulAbNormalRecv      =%d\n",pstSlidWin->ulAbNormalRecv);	  
	
    cps__oams_shcmd_printf("\n\r\n\r");  

    cpss_com_slid_recv_win_data_num_get(pstSlidWin) ;

    cps__oams_shcmd_printf("  ulRecvWinDataNum      =%d\n",
        cpss_com_slid_recv_win_data_num_get(pstSlidWin));
}

/*******************************************************************************
* 函数名称: cpss_com_win_show
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* ulLinkId            UINT32          输入              链路ID
* 函数返回: 
*          无                     
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_win_show(UINT32 ulPhyAddr)
{
    CPSS_COM_PHY_ADDR_T stPHyAddr ;
    CPSS_COM_LINK_T *pstLink ;
    CPSS_COM_SLID_WIN_T* pstSlidWin ;
    CPSS_COM_TRUST_HEAD_T* pstTrustHdr ;
    UINT16 usLoop ;
    UINT32 ulNum  = 0 ;
    CPSS_COM_TRUST_HEAD_T* pstHdrNode ;
    CPSS_COM_TRUST_HEAD_T* pstHdrNodeTmp ;

    stPHyAddr.ucFrame = (ulPhyAddr&0xff000000)>>24 ;
    stPHyAddr.ucShelf = (ulPhyAddr&0x00ff0000)>>16 ;
    stPHyAddr.ucSlot  = (ulPhyAddr&0x0000ff00)>>8 ;
    stPHyAddr.ucCpu   = (ulPhyAddr&0x000000ff) ;

    pstLink = cpss_com_link_find_real(stPHyAddr) ;
    if(NULL == pstLink)
    {
        cps__oams_shcmd_printf("cant find slid win phyaddr=%x\n",*(UINT32*)&stPHyAddr) ;
        return CPSS_ERROR ;
    }

    pstSlidWin = &(pstLink->stSlidWin) ;

    cps__oams_shcmd_printf("usAckExpectedFrame=%d,usNextFrameToSend=%d,usSendWinBusyNum=%d,usFrameExpected=%d,usTooFar=%d\n",
        pstSlidWin->usAckExpectedFrame,pstSlidWin->usNextFrameToSend,
        pstSlidWin->usSendWinBusyNum,pstSlidWin->usFrameExpected,pstSlidWin->usTooFar) ;

    cps__oams_shcmd_printf("<<send slid win data begin ...>>\n") ;

    for(usLoop = 0; usLoop < COM_SLIDWIN_SIZE; usLoop++)
    {
        if( pstSlidWin->pucSlidSendWin[usLoop] != NULL )
        {
            pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)pstSlidWin->pucSlidSendWin[usLoop] ;
            if(NULL == pstTrustHdr)
            {
                continue ;
            }
            
            cps__oams_shcmd_printf("seq=%d,ack=%d,sliceLen=%d,ucPType=%d\n",
                pstTrustHdr->usSeq,pstTrustHdr->usAck,pstTrustHdr->usSliceLen,
                pstTrustHdr->ucPType) ;
            ulNum = ulNum + 1 ;
        }
    }
    cps__oams_shcmd_printf("ulNum=%d\n",ulNum) ;

    cps__oams_shcmd_printf("<<recv slid win data begin ...>>\n") ;
    ulNum = 0 ;
    /*打印接收窗口数据*/
    for(usLoop = 0; usLoop < COM_SLIDWIN_SIZE; usLoop++)
    {
        if( pstSlidWin->ucRecvWinFlag[usLoop] == 1 )
        {
            pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)pstSlidWin->pucSlidRecvWin[usLoop] ;
            if(NULL == pstTrustHdr)
            {
                continue ;
            }
            
            cps__oams_shcmd_printf("seq=%d,ack=%d,sliceLen=%d,ucPType=%d\n",
                pstTrustHdr->usSeq,pstTrustHdr->usAck,pstTrustHdr->usSliceLen,
                pstTrustHdr->ucPType) ;
            ulNum = ulNum + 1 ;
        }
    }
    cps__oams_shcmd_printf("ulNum=%d\n",ulNum) ;

    cps__oams_shcmd_printf("<<send win link data begin ...>>\n") ;
    /*打印发送缓冲区数据*/
    pstHdrNode = pstSlidWin->stSendBufManage.pstSlidSendBufLinkHead ;
    pstHdrNodeTmp = pstHdrNode ;
    ulNum = 0 ;
    while(pstHdrNode != NULL)
    {
        cps__oams_shcmd_printf("seq=%d,ack=%d,sliceLen=%d,ucPType=%d\n",
            pstHdrNode->usSeq,pstHdrNode->usAck,pstHdrNode->usSliceLen,
            pstHdrNode->ucPType) ;
        ulNum = ulNum + 1 ;   
        /*得到链表的下一个节点*/
        pstHdrNodeTmp = (CPSS_COM_TRUST_HEAD_T*)cpss_com_frag_next_slice_get((UINT8*)pstHdrNode);

        pstHdrNode = pstHdrNodeTmp;
    }
    cps__oams_shcmd_printf("ulNum=%d\n",ulNum) ;

    return CPSS_OK ;
}
#else
INT32 cpss_com_send_to_store
(
    CPSS_COM_MSG_HEAD_T *pstSendBuf,
    CPSS_COM_LOGIC_ADDR_T stLogAddr,
    UINT32 ulAddrFlag
)
{
    return CPSS_OK;
}
INT32 cpss_com_slid_send
(
VOID *pvSendBuf,                  /*app需要传输的消息指针*/
UINT32 ulMsgLen,                  /*app的消息长度*/
CPSS_COM_MSG_HEAD_T *pstMsg,      /*com msg hdr的指针*/
CPSS_COM_SLID_WIN_T *pstSlidWin   /*滑窗对象的指针*/
)
{
    return CPSS_OK;
}
INT32 cpss_com_slid_recv
(
CPSS_COM_SLID_WIN_T   *pstSlidWin, 
CPSS_COM_TRUST_HEAD_T *pstHdr
)
{
    return CPSS_OK;
}
VOID cpss_com_slid_win_reset
(
    CPSS_COM_SLID_WIN_T* pstSlidWin
)
{
    return ;
}
VOID cpss_com_slid_show(CPSS_COM_SLID_WIN_T *pstSlidWin)
{
    return ;
}
VOID cpss_com_data_store_timer_delete_real
(
    CPSS_COM_PID_T *pstDstPid
)
{
    return ;
}
INT32 cpss_com_slid_init
(
 void* pvLinkObj,
 CPSS_COM_SLID_WIN_T *pstSlidWin
 )
 {
     return CPSS_OK;
 }
#endif


/*================  函数实现  <结束>  ================*/
