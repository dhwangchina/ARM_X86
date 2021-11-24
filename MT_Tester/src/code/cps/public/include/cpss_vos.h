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
#ifndef CPSS_VOS_H
#define CPSS_VOS_H

/********************************* 头文件包含 ********************************/
#include "cpss_type.h"
/******************************** 宏和常量定义 *******************************/
/* 无效互斥量描述符 */
#define CPSS_VOS_MUTEXD_INVALID    ((UINT32)(0xFFFFFFFF)) 
/* 互斥量等待时间 */
#define CPSS_VOS_MUTEX_WAIT_FOREVER     (WAIT_FOREVER)      /* 永久等待 */
#define CPSS_VOS_MUTEX_NO_WAIT          (NO_WAIT)           /* 不等待 */

#ifdef CPSS_VOS_VXWORKS
#define CPSS_VOS_TASK_OP_FP VX_FP_TASK
#else
#define CPSS_VOS_TASK_OP_FP 0
#endif

#define CPSS_TASK_REG_NUM_MAX (5)
#define CPSS_TASK_NO_INIT_DELAY_TIME 1000
typedef INT32 (*CPSS_TASK_BLOCK_FUNCPTR)(UINT32 *pulBlockParamList);
typedef INT32 (*CPSS_TASK_DEALING_FUNCPTR)(INT32 lRet, UINT32 *pulParamList);
typedef INT32 (*CPSS_TASK_INIT_FUNCPTR)();
/* 普通任务类型，每种类型的任务根据需要配置的数量不同 */
#define CPSS_NORMAL_TASK_TYPE_SEM    0
#define CPSS_NORMAL_TASK_TYPE_UDP    1
#define CPSS_NORMAL_TASK_TYPE_MQ     2
#define CPSS_NORMAL_TASK_TYPE_DELAY  3
#define CPSS_NORMAL_TASK_TYPE_ACCEPT 4
#define CPSS_NORMAL_TASK_TYPE_SELECT 5
#define TASK_PRIORITY_TIME_CRITICAL 0
#define TASK_PRIORITY_HIGHEST   1
#define TASK_PRIORITY_ABOVE_NORMAL  2
#define TASK_PRIORITY_NORMAL    3
#define TASK_PRIORITY_BELOW_NORMAL  4
#define TASK_PRIORITY_LOWEST    5
#define TASK_PRIORITY_IDLE  6
#define TASK_PRIORITY_SHMCSEND  7
#define TASK_PRIORITY_SHMCRECV  8

/* stack size */
#define VOS_TASK_DEF_STACK_SIZE 0x100000 /* 栈大小 */
/* task descriptor */
#define VOS_TASK_DESC_INVALID   0xFFFFFFFF /* 非法 */

/******************************** 类型定义 ***********************************/

typedef INT32 (*CPSS_TASK_ENTRY_FUNCPTR)();

/*为消息队列添加*/

#ifdef CPSS_VOS_LINUX
#define VOS_MSG_Q_FIFO  0x00 /* 任务按FIFO算法调度  */
#define VOS_MSG_Q_PRI   0x01 /* 任务按优先级调度 */
#endif

#ifdef CPSS_VOS_LINUX
#define VOS_MSG_PRI_NORMAL  0 /* 优先级为普通的消息*/
#define VOS_MSG_PRI_URGENT  1  /* 优先级为紧急的消息 */
#endif


#define VOS_MSG_Q_DESC_INVALID  0xFFFFFFFF /* 无效值 */

extern INT32    cpss_vos_msg_q_lib_init (void);
extern UINT32   cpss_vos_msg_q_create (INT8 *pcName, INT32 lMaxMsgs,
                                       INT32 lMaxMsgLen, INT32 lOptions);
extern INT32    cpss_vos_msg_q_delete (UINT32 ulMsgQDesc);
extern UINT32   cpss_vos_msg_q_get (INT8 *pcName);
extern INT32    cpss_vos_msg_q_send (UINT32 ulMsgQDesc, INT8 *pcBuf,
                                     UINT32 ulNBytes, INT32 lTimeout,
                                     INT32 lPri);
extern INT32    cpss_vos_msg_q_receive (UINT32 ulMsgQDesc, INT8 *pcBuf,
                                        UINT32 ulMaxNBytes, INT32 lTimeout);
extern INT32    cpss_vos_msg_q_num_msgs (UINT32 ulMsgQDesc);

extern INT32    cpss_vos_msg_q_show (UINT32 ulMsgQDesc, INT32 lLevel);
/*为消息队列添加结束*/

/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/

extern UINT32 cpss_vos_mutex_create(VOID);
extern INT32 cpss_vos_mutex_delete(UINT32 ulMutexD);
extern INT32 cpss_vos_mutex_p(UINT32 ulMutexD,UINT32 ulWaitTime);
extern INT32 cpss_vos_mutex_v(UINT32 ulMutexD);
extern VOID cpss_vos_sleep(UINT32 ulMilliseconds);
extern void cpss_reg_task_entry(UINT32 ulP1);
extern INT32 cpss_vos_task_register(CPSS_TASK_ENTRY_FUNCPTR pfTaskEntry, UINT32 ulIntVal);
extern void cpss_vos_ntask_entry(INT32 lIdx);
extern INT32 cpss_vos_ntask_register(
              UINT32 ulTaskType,
              CPSS_TASK_INIT_FUNCPTR    pfInit, 
              CPSS_TASK_BLOCK_FUNCPTR   pfBlock,
              CPSS_TASK_DEALING_FUNCPTR pfDealing,
              UINT32 *pulBlockParamList,
              UINT8  *pucTaskName);
extern void cpss_delay(UINT32 useconds);
extern UINT32 cpss_vos_task_spawn(INT8 * pcName,INT32 lPri, INT32 lOptions,INT32 lStackSize,VOID_FUNC_PTR pfEntry,INT32 lArg);
extern INT32 cpss_vos_task_delete(UINT32 ulTaskDesc);
extern INT32 cpss_vos_task_pri_set(UINT32 ulTaskDesc,INT32 lNewPri);
extern INT32 cpss_vos_task_pri_get(UINT32 ulTaskDesc,INT32 * plPri);
extern INT32 cpss_vos_task_delay(INT32 lMs);
extern UINT32 cpss_vos_task_desc_self (void);
#define CPSS_TIMER_BASE_MSG (0x21000020)

typedef struct {
 UINT32 ulMsgId; /* 消息标识，网络字节序，固定填CPSS_TIMER_BASE_MSG */
}CPSS_TIMER_BASE_MSG_T ;

extern INT32 cpss_task_base_timer_set(UINT32 vulMsgQueId, BOOL vbIsOpen);

extern VOID send_msgq_loop_timer();

/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_VOS_H */
/******************************** 头文件结束 *********************************/

