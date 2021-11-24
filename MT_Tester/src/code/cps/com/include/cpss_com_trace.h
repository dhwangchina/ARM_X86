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
#ifndef CPSS_COM_TRACE_H
#define CPSS_COM_TRACE_H
/********************************* 头文件包含 ********************************/
#include "cpss_config.h"

/******************************** 宏和常量定义 *******************************/
#define CPSS_COM_TRACE_PID_LOCAL_TYPE  0
#define CPSS_COM_TRACE_PID_REMOTE_TYPE 1

/*消息跟踪的缓冲区大小*/
#ifdef CPSS_DSP_CPU
#define CPSS_COM_TRACE_BUF_SIZE  8*1024
#else
#define CPSS_COM_TRACE_BUF_SIZE  512*1024
#endif
#define CPSS_COM_TRACE_HDR_SIZE   sizeof(CPSS_COM_TRACE_HEAD_T)
#define CPSS_COM_TRACE_ENTRY_SIZE sizeof(CPSS_COM_TRACE_ENTRY_T)
#define CPSS_COM_TRACE_HDR_ENTRY_SIZE   CPSS_COM_TRACE_HDR_SIZE + CPSS_COM_TRACE_ENTRY_SIZE

#define CPSS_COM_TRACE_TURN_POS_INVALID    0xffffffff

#define CPSS_COM_TRACE_REATE_DEFAULT    100

#define CPSS_TRACE_TIMER_NO          CPSS_TIMER_02     /* 日志定时器 */
#define CPSS_TRACE_TIMER_MSG         CPSS_TIMER_02_MSG /* 日志定时器超时消息 */
#define CPSS_TRACE_TIMER_PERIOD      1000              /**/

/******************************** 类型定义 ***********************************/
/* 定义消息跟踪任务结构 */
typedef struct tagCPSS_COM_TRACE_TASK
{
    CPSS_COM_TRACE_FILTER_T stTraceFilterInfo;    /* 消息过滤条件 */
    CPSS_COM_PID_T stLdtPid;                      /* LDT消息跟踪窗口对应的PID */
    UINT32 ulTraceID;                             /* 跟踪任务号 */
    UINT32 ulSerialNo;                            /* 消息跟踪流水号 */    
    UINT32 ulTraceLen;                            /* 任务对应的跟踪消息上报长度 */
    struct tagCPSS_COM_TRACE_TASK *pstNext;       /* 下一跟踪任务指针 */
}CPSS_COM_TRACE_TASK_T;

typedef struct tagCPSS_COM_TRACE_INFO
{
    CPSS_COM_TRACE_HEAD_T  stTraceHead;           /* 消息跟踪包头结构 */
    CPSS_COM_TRACE_ENTRY_T stTraceEntry;          /* 每条跟踪消息头结构 */
    CPSS_COM_MSG_HEAD_T    stMsgHead;             /* 消息头结构 */
    INT8                   cBuf[CPSS_TRACE_PACKET_LEN 
                                - sizeof(CPSS_COM_MSG_HEAD_T) 
                                - sizeof(CPSS_COM_TRACE_ENTRY_T) 
                                - sizeof(CPSS_COM_TRACE_HEAD_T)];   /* 消息体 */
}CPSS_COM_TRACE_INFO_T;

/*消息跟踪管理结构*/
typedef struct tagCPSS_COM_TRACE_MANAGE_T
{
    UINT8* pucTraceBufHdr ;
    UINT32 ulBufPositionHdr ;
    UINT32 ulBufPositionCurr ;
    UINT32 ulUsedLen ;
    UINT32 ulTurnPositon ;
    UINT32 ulTraceToken ;
    UINT32 ulTraceTokenValid ;
    UINT32 ulLostNum ;
}CPSS_COM_TRACE_MANAGE_T ;

/*增加消息跟踪任务消息*/
typedef struct tagCPSS_COM_TRACE_TASK_ADD_MSG
{
    UINT32 ulTraceId ;
    CPSS_COM_TRACE_FILTER_T stComTraceFilter ;
    CPSS_COM_PID_T stLdtPid ;
}CPSS_COM_TRACE_TASK_ADD_MSG_T;

/*删除消息跟踪任务消息*/
typedef struct tagCPSS_COM_TRACE_TASK_DEL_MSG
{
    UINT32 ulTraceId ;
}CPSS_COM_TRACE_TASK_DEL_T;

typedef struct tagCPSS_COM_TRACE_RATE_SET_MSG
{
    UINT32 ulTraceID ;
    UINT32 ulRate ;
}CPSS_COM_TRACE_RATE_SET_MSG_T;


/******************************** 全局变量声明 *******************************/
/*extern */
extern CPSS_COM_TRACE_MANAGE_T g_stCpssComTraceMan ;

/******************************** 外部函数原形声明 ***************************/
extern UINT32 g_ulCpssComTraceTaskNum;
extern INT32 cpss_com_trace_ex(CPSS_COM_MSG_HEAD_T *pstMsgHead);
#define cpss_com_trace(x) if(0 != g_ulCpssComTraceTaskNum) cpss_com_trace_ex(x);
extern INT32 cpss_com_g_ulCpssComTraceTaskNum_get();
extern INT32 cpss_com_g_pstCpssComTraceHead_get();

extern INT32 cpss_com_trace_init();
extern INT32 cpss_com_trace_data_add(UINT8 *pucBuf) ;
extern INT32 cpss_com_trace_data_send() ;
extern INT32 cpss_com_trace_recv_addtask_deal(UINT8* pucRecvBuf,UINT32* pulFirstTask) ;
extern INT32 cpss_com_trace_recv_deltask_deal (UINT8* pucRecvBuf, UINT32* pulNullFlag) ;
extern INT32 cpss_com_trace_recv_setrate_deal(UINT8* pucRecvBuf) ;
extern VOID cpss_com_trace_hton(CPSS_COM_TRACE_INFO_T* pstTraceInfo) ;


/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_COM_TRACE_H */
/******************************** 头文件结束 *********************************/

