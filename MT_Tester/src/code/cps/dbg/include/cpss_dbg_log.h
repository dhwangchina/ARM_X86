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
#ifndef CPSS_DBG_LOG_H
#define CPSS_DBG_LOG_H
/********************************* 头文件包含 ********************************/

/******************************** 宏和常量定义 *******************************/
#ifdef CPSS_DSP_CPU
#define CPSS_LOG_BUF_LEN           (8*1024)        /* 日志缓存长 */
#else
#define CPSS_LOG_BUF_LEN           (512*1024)        /* 日志缓存长 */
#endif
#define CPSS_LOG_TIMER_NO          CPSS_TIMER_01     /* 日志定时器 */
#define CPSS_LOG_TIMER_MSG         CPSS_TIMER_01_MSG /* 日志定时器超时消息 */

/******************************** 类型定义 ***********************************/
/*日志信息管理 */
typedef struct tagCPSS_LOG_MANAGE
{
    UINT32 ulLogID;                /* 日志流水号 */
    UINT8  *pucLogBufHead;         /* 指向日志缓冲区头指针 */
    UINT8  *pucLogDataTail;        /* 日志缓冲队列尾指针，指向队列中最后一包数据 */
    UINT8  *pucLogDataAdd;         /* 日志缓冲队列加入数据的位置指针，在g_pucLogDataTail后 */
    UINT8  *pucLogDataHead;        /* 日志缓冲队列头指针，为NULL表示队列为空*/
    INT32  lLogTimerID;            /* 日志流控定时器是否有效 */
    UINT32 ulLogTimerPeriod;       /* 日志流控定时器周期 */
    UINT32 ulLogToken;             /* 日志令牌数 */
    UINT32 ulLogTokenValid;        /* 当前持有的有效令牌数 */
    UINT8  ucLogLevel;             /* 日志级别 */
    UINT8  ucReserved[3];
    UINT32 ulLogInfoLostNum;       /* 丢失的日志信息计数 */
    UINT32 ulSemForLogLost;        /* 丢失日志信息计数信号量 */
    CPSS_COM_PID_T stLogDstPid;    /* 日志上报PID */           	
    BOOL   bLogSendtoMate;         /* 日志发送到伙伴板标志 */
}CPSS_LOG_MANAGE_T;


/******************************** 全局变量声明 *******************************/
/*extern */
extern BOOL   g_bCpssDbgLogValid;                      /* 日志是否可用标识 */
extern CPSS_LOG_MANAGE_T g_stCpssDbgLogManage;         /* 日志相关管理信息 */
/******************************** 外部函数原形声明 ***************************/
extern INT32 cpss_log_init(); 
extern INT32 cpss_log_data_send();
extern INT32 cpss_log_data_add
(
    UINT8 *pucBuf
);
extern INT32 cpss_log_redirect_set(VOID);
extern INT32 cpss_vos_g_stCpssDbgLogManage_get();
extern INT32 cpss_vos_g_bCpssDbgLogValid_get();
/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_DBG_LOG_H */
/******************************** 头文件结束 *********************************/


