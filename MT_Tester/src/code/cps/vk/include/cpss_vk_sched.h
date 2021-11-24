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
#ifndef __INCcpss_vk_schedh
#define __INCcpss_vk_schedh

#ifdef __cplusplus
extern "C" {
#endif
 
/******************************* 包含文件声明 *********************************/

#include "cpss_type.h"
#include "cpss_util_q_fifo_lib.h"
#include "cpss_vk_proc.h"

/******************************** 宏和常量定义 *******************************/
/* message queue */
#define VK_SCHED_DEF_MAX_MSGS  (2 * 1024) /* 最大消息数目 */
#define VK_SCHED_DEF_MAX_MSG_LEN    8 /* 消息最大长度 */

#define CPSS_VK_PROC_TIME_STAT_USE  1
#define CPSS_VK_PROC_TIME_EXEC_TIME_MAX  1000
#define CPSS_VK_PROC_TIME_STAT_NUM   20

/******************************** 类型定义 ***********************************/
/* SCB(scheduler control block) */
typedef struct tagCPSS_VK_SCHED_SCB
    {
#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS)|| defined(CPSS_VOS_LINUX)
    UINT32  ulMsgQDesc; /* 消息队列描述符 */
#endif

#ifdef CPSS_DSP_CPU    
    CPSS_UTIL_Q_FIFO_HEAD_T    msgQ; /* message queue */
    INT32   lMsgNum; /* message number */
#endif
    UINT32  ulTaskDesc; /* 任务描述符 */
    CPSS_UTIL_Q_FIFO_HEAD_T    qReady; /* READY纤程FIFO队列 */
    CPSS_UTIL_Q_FIFO_HEAD_T    qPend; /* PEND纤程FIFO队列 */
    INT32   lQReadyNum; /* READY纤程FIFO队列数目 */
    INT32   lQPendNum; /* PEND纤程FIFO队列数目 */
    INT32   lReadyNum; /* READY数目 */
    CPSS_UTIL_Q_FIFO_HEAD_T    mailBox; /* 邮箱 */
    INT32   lMailNum; /* 邮箱中消息数目 */
    UINT32  ulProcDescCurr; /* 当前运行纤程描述符 */
    void *  pIpcMsgCurr; /* 当前运行纤程处理消息 */
    INT32   lError; /* 错误 */
    INT32   lTick;   /* 纤程运行时间 */
    INT32   lMaxTicks;   /* 纤程最大运行时间 */
    } CPSS_VK_SCHED_SCB_T;

typedef struct tagCPSS_VK_SCHED_SCB * VK_SCHED_ID;

/* 调度器描述符入口 */

typedef struct tagCPSS_VK_SCHED_DESC
    {
    VK_SCHED_ID tSchedId; /* 调度器标识 */
    BOOL    bInuse; /* 占用标志 */
    } CPSS_VK_SCHED_DESC_T;

/* 调度器信息 */

typedef struct tagCPSS_VK_SCHED_INFO
    {
    UINT32  ulSchedDesc; /* 调度器描述符 */
    VK_SCHED_ID tSchedId; /* 调度器标识 */
#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS) || defined (CPSS_VOS_LINUX)
    UINT32  ulMsgQDesc; /* 消息队列描述符 */
#endif
    UINT32  ulTaskDesc; /* 任务描述符 */
    } CPSS_VK_SCHED_INFO_T;

/* 调度器统计 */

typedef struct tagCPSS_VK_SCHED_STAT
    {
    INT32   lQReadyNum; /* READY纤程FIFO队列数目 */
    INT32   lQPendNum; /* PEND纤程FIFO队列数目 */
    INT32   lProcNum; /* 纤程数目 */
    UINT32  ulCallNum; /* 调用数目 */
    } CPSS_VK_SCHED_STAT_T;
#if 1
/*纤程执行时间统计*/
typedef struct tagCPSS_PROC_EXECUT_TIME_T
{
    UINT32 ulUseFlag ;
    UINT32 ulExeProcId ;
    UINT32 ulExecTick ;
    UINT32 ulMsgId ;
    UINT32 ulTempBeginTick ;
    CPSS_COM_PID_T stSrcPid ;
    CPSS_COM_PID_T stDestPid ;
    CPSS_TIME_T  stTempBeginTime ;
    CPSS_TIME_T  stExecBeginTime ;
    CPSS_TIME_T  stExecEndTime ;
    struct tagCPSS_PROC_EXECUT_TIME_T *pNext ;
}CPSS_PROC_EXECUT_TIME_T ;
#endif
/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/

extern INT32    cpss_vk_sched_lib_init (void);
extern INT32    cpss_vk_sched_proc_load  (UINT32 ulSchedDesc, VK_PROC_ID procId)
                                          ;
extern void     cpss_vk_sched (INT32 lArg);
extern UINT32   cpss_vk_sched_spawn (INT8 *pcName, INT32 lPri, INT32 lOptions,
                                     INT32 lStackSize, VOID_FUNC_PTR pfEntry);
extern INT32    cpss_vk_sched_delete (UINT32 ulSchedDesc);
extern INT32    cpss_vk_sched_proc_spawn (UINT32 ulSchedDesc, VK_PROC_ID procId)
                                          ;
extern INT32    cpss_vk_sched_proc_delete (UINT32 ulSchedDesc, VK_PROC_ID procId
                                           );
extern INT32    cpss_vk_sched_proc_suspend (UINT32 ulSchedDesc, 
                                            VK_PROC_ID procId);
extern INT32    cpss_vk_sched_proc_resume (UINT32 ulSchedDesc, VK_PROC_ID procId
                                           );
extern INT32    cpss_vk_sched_error_set (UINT32 ulSchedDesc, INT32 lNewError);
extern INT32    cpss_vk_sched_error_get (UINT32 ulSchedDesc, INT32 *plError);
extern INT32    cpss_vk_sched_send (UINT32 ulSchedDesc, INT8 *pcBuf, 
                                    INT32 lTimeout, INT32 lPri, INT32 lType);
extern INT32    cpss_vk_sched_info_get (UINT32 ulSchedDesc, 
                                        CPSS_VK_SCHED_INFO_T *ptSchedInfo);
extern INT32    cpss_vk_sched_stat_get (UINT32 ulSchedDesc, 
                                        CPSS_VK_SCHED_STAT_T *ptSchedStat);
extern INT32    cpss_vk_sched_show (UINT32 ulSchedDesc, INT32 lLevel);

extern INT32 cpss_vk_proc_exec_time_stat_begin(UINT16  usGuid, IPC_MSG_HDR_T *ptHdr) ;
extern INT32 cpss_vk_proc_exec_time_stat_end(UINT16  usGuid, IPC_MSG_HDR_T *ptHdr) ;
/******************************** 头文件保护结尾 *****************************/
#ifdef __cplusplus
}
#endif

#endif /* __INCcpss_vk_schedh */
/******************************** 头文件结束 *********************************/

