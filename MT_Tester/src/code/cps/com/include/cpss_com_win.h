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
#ifndef CPSS_COM_WIN_H
#define CPSS_COM_WIN_H

/********************************* 头文件包含 ********************************/
#include "cpss_public.h"
#include "cpss_common.h"
#include "cpss_com_frag.h"

/******************************** 宏和常量定义 *******************************/
#define COM_SLIDWIN_SIZE              128                   /*滑窗的窗口大小*/
#define COM_SLID_PKGNUM_SIZE          256           /*滑窗序号空间*/

#define COM_SLID_PTYPE_INCREDIBLE      0    /*不可靠传输帧*/
#define COM_SLID_PTYPE_CREDIBLE        1    /*可靠传输*/
#define COM_SLID_PTYPE_ACK             2    /*滑窗的ACK帧*/
#define COM_SLID_PTYPE_SYN_REQ         3    /*滑窗对齐请求*/
#define COM_SLID_PTYPE_SYN_RES         4    /*滑窗对齐响应*/
#define COM_SLID_PTYPE_NAK             5    /*NAK消息*/
#define COM_SLID_PTYPE_DETECT          6    /*DETECT探测消息*/
#define COM_SLID_PTYPE_DETECT_RES      7    /*DETECT探测响应消息*/
#define COM_SLID_PTYPE_SYN_END         8    /*syn过程结束消息*/
#define COM_SLID_PTYPE_SYN_END_RES     9    /*syn过程结束响应消息*/

#define COM_SLID_ACK_TIMEOUT           10      /*ack定时器超时间隔(循环次数)*/
#define COM_SLID_RESEND_TIMEOUT        500    /*resend定时器超时间隔*/
#define COM_SLID_SYNREQ_TIMEOUT        1000    /*synreq定时器超时时间间隔*/
#define COM_SLID_DETECT_TIMEOUT        1000     /*detect定时器超时时间间隔*/
#define COM_SLID_SYN_END_TIMEOUT       1000    /*synend定时器超时时间间隔*/
#define COM_SLID_RESEND_TIMEOUT_MIN    10    /*resend定时器最短的定时间隔*/

#define SLID_DETECT_RESEND_NUM         10        /*DETECT消息重发次数*/

#define SLID_SYN_TIMER_CLASS      0        /*同步类型定时器*/
#define SLID_ACK_TIMER_CLASS      1        /*ACK类型定时器*/
#define SLID_RESEND_TIMER_CLASS   2        /*重发定时器*/
#define SLID_DETECT_TIMER_CLASS   3        /*探测消息重发定时器*/
#define SLID_SYN_END_TIMER_CLASS  4        /*syn_end重发定时器*/

#define SLID_STAT_INIT            0        /*滑窗处于初始太态*/
#define SLID_STAT_SYN             1        /*滑窗处于同步状态*/
#define SLID_STAT_NORMAL          2        /*滑窗处于正常状态*/

#define IPC_MSG_TO_COM_HDR(pNode)  (((INT8 *) pNode) + sizeof (IPC_MSG_HDR_T))
#define IPC_MSG_TO_APP_HDR(pNode)  (((INT8 *) pNode) + sizeof (IPC_MSG_HDR_T) + \
                                                       sizeof(CPSS_COM_MSG_HEAD_T))
#define COM_MSG_TO_APP_HDR(pNode)  (((INT8 *) pNode) + sizeof (CPSS_COM_MSG_HEAD_T))


/*以下函数声明不属于滑窗部分,属于数据发送部分*/
/* 链路故障定时器消息 */
#define CPSS_COM_DATA_STORE_TM_MSG      CPSS_TIMER_01_MSG
/* 链路故障定时器号 */
#define CPSS_COM_DATA_STORE_TM_NO       (CPSS_COM_DATA_STORE_TM_MSG&0xFF)
/* 链路故障定时器超时时间,单位:毫秒 */
#define CPSS_COM_DATA_STORE_TM_INTERVAL (5000)

#define CPSS_COM_STORE_NOT_HAS_SEND_FLAG 0
#define CPSS_COM_STORE_HAS_SEND_FLAG     1  

/*最少的IPC消息裕量*/
#define CPSS_COM_IPC_MSG_FREE_NUM_MIN    400
/******************************** 类型定义 ***********************************/

/*滑窗使用的单链表节点定义*/
typedef struct COM_SLID_LINK_NODE_T
{
    struct COM_SLID_LINK_NODE_T *pstNext ;   
    UINT32 ulData ;
}COM_SLID_LINK_NODE_T ;

/*滑窗使用的单链表管理结构定义*/
typedef struct tagCOM_SLID_LINK_T
{
    COM_SLID_LINK_NODE_T *pstHead ;         /*链头指针*/
}COM_SLID_LINK_T ;


/*发送缓冲区的结构定义*/
typedef struct tagSLID_SENDBUF_MANAGE_T
{
    CPSS_COM_TRUST_HEAD_T* pstSlidSendBufLinkHead ;   /*发送缓冲区的头指针*/
    CPSS_COM_TRUST_HEAD_T* pstSlidSendBufLinkTail ;   /*发送缓冲区的尾指针*/
}SLID_SENDBUF_MANAGE_T ;

/*滑窗定时器的结构定义*/
typedef struct tagCPSS_SLID_TIMER_T
{
    UINT32 ulTimerCount ;    /*当前时间的tick值*/
    UINT32 ulTimerInter ;    /*定时器时间间隔*/
    UINT32 ulEnableFlag ;    /*定时器的使能标志   0: disable ; 1: enable*/
}CPSS_SLID_TIMER_T ;       

/*滑窗的统计结构*/
typedef struct tagCPSS_SLID_STAT_T
{
    UINT32 ulRecvSliceNum ;      /*接收的数据片个数*/
    UINT32 ulRecvDataNum ;       /*接收的可靠数据数目*/
    UINT32 ulSendDataNum ;       /*发送的可靠数据数目*/
    UINT32 ulSendSliceNum ;      /*发送的数据片的数目*/
    UINT32 ulComPkgFailNum ;     /*组包失败的次数*/
    UINT32 ulResendNum ;         /*重发数据的数目*/
    UINT32 ulResendFailNum ;
    UINT32 ulResetWinNum ;       /*复位滑窗次数*/
    UINT32 ulRecvDeliverErrNum;  /* 上交给高层时的失败统计 */
    UINT32 ulResendNeedSeq ;     /*发送seq值统计*/
    UINT32 ulResendRealSeq ;
    UINT32 ulSendNakDataNum ;        /**/
    UINT32 ulSendNakDataFailNum ;
    UINT32 ulRecvNakSeq ;
    UINT32 ulRecvNakNum ;
    UINT32 ulRecvDataNotInWin ;
    UINT32 ulRecvDataNotInWinSeq ;
    UINT32 ulSendNotifyFailNum ;
    UINT32 ulDeliverNum ;
    UINT32 ulAckRecvNum ;
    UINT32 ulAckSendNum ;
    UINT32 ulAckSendFailNum ;
    UINT32 ulAckSendValue ;
    UINT32 ulClearDataNum ;
    UINT32 ulRecvNotNormal ;
    UINT32 ulSumCheckErrNum ;
    UINT32 ulRecvDataDealErrNum ;
    UINT32 ulRecvAckDealErrNum ;
    UINT32 ulRecvNakDealErrNum ;

    UINT32 ulStoreOverFlowNum ;
    UINT32 ulIpcNotEnoughNum ;
    UINT32 ulMemAllocFailNum ;
    UINT32 ulSplitFailNum ;
    UINT32 ulIpcSendFailNum ;

    UINT32 ulRecvUnKnownNum ;    
}CPSS_SLID_STAT_T ;

/*滑窗的发送,接收,控制结构*/
typedef struct tagCPSS_COM_SLID_WIN_T
{
    UINT8* pucSlidSendWin[COM_SLIDWIN_SIZE] ;           /*发送滑窗的窗口*/
    UINT8* pucSlidRecvWin[COM_SLIDWIN_SIZE] ;           /*接收滑窗的窗口*/
    SLID_SENDBUF_MANAGE_T stSendBufManage ;             /*发送缓冲区的管理变量*/
    CPSS_SLID_TIMER_T stResendTimer ;                   /*重发定时器*/
    CPSS_SLID_TIMER_T stAckTimer ;                      /*ack定时器(0: 定时器关闭 1: 定时器清零 2: 定时器*/
    CPSS_SLID_TIMER_T stSynreqTimer ;                   /*synreq重发定时器*/
    CPSS_SLID_TIMER_T stDetectTimer ;                   /*链路检测定时器*/
    CPSS_SLID_TIMER_T stSynEndTimer ;                   /*synEnd定时器*/
    UINT16 usDetectNum ;                                /*链路检测消息发送次数*/
    /*发送滑窗的控制信息*/
    UINT16 usAckExpectedFrame ;     /*期望接收ack消息的帧序号*/
    UINT16 usNextFrameToSend ;      /*下一次发送的帧序号*/
    UINT16 usSendWinBusyNum ;       /*发送滑窗被占用的个数*/
    /*接收滑窗的控制信息*/
    UINT16 usFrameExpected ;        /*期望接收的帧序号*/
    UINT16 usTooFar ;               /*接收滑窗最远的接收边界*/
    COM_FRAG_PACKAGE_T stPkg;       /*包管理结构*/
    UINT8 *pucLinkObj ;             /*链路对象指针*/
    UINT32 ulStoreFlag ;            /*发送缓冲区标志1:已经发送过发送缓冲区,0:还未发送过发送缓冲区*/
    UINT8  ucRecvWinFlag[COM_SLIDWIN_SIZE] ;      /*接收滑窗的接收标志*/
    UINT8  ucWinEnableFlag ;          /*窗口使能标志 0: 1: 2: */
    UINT8  ucNakFlag ;                /*nak使能标志*/
    UINT8  ucReserve;
    CPSS_SLID_STAT_T stSlidStat ;     /*滑窗的统计结构*/
    UINT32 ulRecvExpectDataNum ;      /*期望接收的数据帧个数*/ 
    UINT32 ulSynReqSend;                   /* 发送synreq */
    UINT32 ulSynResSend;                   /* 发送synres*/
    UINT32 ulSynEndSend;                   /* 发送synend*/
    UINT32 ulSynEndResSend;            /* 发送synend res */
    UINT32 ulSynReqRecv;                   /* 接受synreq */
    UINT32 ulSynResRecv;	                 /* 接受synres*/
    UINT32 ulSynEndRecv;	                   /* 接受synend*/
    UINT32 ulSynEndResRecv;	            /* 接受synend res */
    UINT32 ulAbNormalRecv;
	
}CPSS_COM_SLID_WIN_T ;


/*滑窗的发送消息结构*/
typedef struct tagSLID_PROC_SEND_MSG_T
{    
    UINT8* pucSendBufHead ;           /*发送数据的链首指针*/
    UINT8* pucSendBufTail ;           /*发送数据的链尾指针*/
    CPSS_COM_SLID_WIN_T  *pstSlidWin ;/*滑窗的指针*/
    VOID                 *pvLinkObj  ;/*链路指针*/
    CPSS_COM_LOGIC_ADDR_T stLogicAddr ;
    UINT32 ulAddrFlag ;
    UINT32 ulAppMsgId ;
    UINT32 ulAppMsgLen ;
}SLID_PROC_SEND_MSG_T ;

/*以下结构定义不属于滑窗部分,属于数据发送部分*/

/*滑窗状态正常通知消息*/
typedef struct tagCPSS_COM_SLID_NORMAL_IND_MSG
{
    CPSS_COM_LOGIC_ADDR_T stLogAddr ;
    UINT32 ulAddrFlag ;
}CPSS_COM_SLID_NORMAL_IND_MSG_T;


/******************************** 全局变量声明 *******************************/



/******************************** 外部函数原形声明 ***************************/
/*可靠传输发送函数*/
extern INT32 cpss_com_slid_send
(
VOID *pvSendBuf,                  /*app需要传输的消息指针*/
UINT32 ulMsgLen,                  /*app的消息长度*/
CPSS_COM_MSG_HEAD_T *pstMsg,      /*com msg hdr的指针*/
CPSS_COM_SLID_WIN_T *pstSlidWin   /*滑窗对象的指针*/
) ;

/*可靠传输接收函数*/
extern INT32 cpss_com_slid_recv
(
CPSS_COM_SLID_WIN_T   *pstSlidWin, 
CPSS_COM_TRUST_HEAD_T *pstHdr
) ;

/*可靠传输初始化函数*/
extern INT32 cpss_com_slid_init
(
void* pvLinkObj,
CPSS_COM_SLID_WIN_T *pstSlidWin
) ;

extern VOID cpss_com_slid_local_win_clear
(
    CPSS_COM_SLID_WIN_T *pstSlidWin 
) ;

/*以下函数声明不属于滑窗部分,属于数据发送部分*/
extern INT32 cpss_com_data_store_req_deal
(
    CPSS_COM_MSG_HEAD_T* pstDispHdr
) ;

extern VOID cpss_com_data_store_timer_delete
(
    CPSS_COM_PID_T *pstDstPid
) ;

extern BOOL cpss_com_store_buf_overflow
(    
    CPSS_COM_PID_T *pstDstPid,
    UINT32 ulDataLen    
) ;

VOID cpss_com_slid_win_reset
(
 CPSS_COM_SLID_WIN_T* pstSlidWin
) ;

VOID cpss_com_data_store_timer_delete_real
(
 CPSS_COM_PID_T *pstDstPid
) ;

extern INT32 cpss_com_slidtimer_task_reg();


/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_COM_WIN_H */
/******************************** 头文件结束 *********************************/

