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
#ifndef CPSS_OVS_MSG_H
#define CPSS_OVS_MSG_H

#ifdef __cplusplus
extern "C" {
#endif
 
/********************************* 头文件包含 ********************************/
#include "cpss_type.h"
#include "cpss_util_q_fifo_lib.h"
#include "cpss_vos_sem.h"
#include "cpss_config.h"

/******************************** 宏和常量定义 *******************************/


#define VOS_MSG_Q_NAME_LEN  32 /* 消息队列名的长度*/



#ifdef CPSS_VOS_VXWORKS
#define VOS_MSG_Q_FIFO  MSG_Q_FIFO /* 任务按FIFO算法调度  */
#define VOS_MSG_Q_PRI   MSG_Q_PRIORITY /* 任务按优先权算法调度 */
#endif

#ifdef CPSS_VOS_WINDOWS
#define VOS_MSG_Q_FIFO  0x00 /* 任务按FIFO算法调度  */
#define VOS_MSG_Q_PRI   0x01 /* 任务按优先级调度 */
#endif

#ifdef CPSS_DSP_CPU
#define VOS_MSG_Q_FIFO  0x00 /* 任务按FIFO算法调度  */
#define VOS_MSG_Q_PRI   0x01 /* 任务按优先级调度 */
#endif

/*优先级 */

#ifdef CPSS_VOS_VXWORKS
#define VOS_MSG_PRI_NORMAL  MSG_PRI_NORMAL /* 优先级为普通的消息*/
#define VOS_MSG_PRI_URGENT  MSG_PRI_URGENT /* 优先级为紧急的消息 */
#endif

#ifdef CPSS_VOS_WINDOWS
#define VOS_MSG_PRI_NORMAL  0 /* 优先级为普通的消息*/
#define VOS_MSG_PRI_URGENT  1  /* 优先级为紧急的消息 */
#endif

#ifdef CPSS_DSP_CPU
#define VOS_MSG_PRI_NORMAL  0 /* 优先级为普通的消息*/
#define VOS_MSG_PRI_URGENT  1  /* 优先级为紧急的消息 */
#endif

#define VOS_MSG_NODE_SIZE(msgLen)   (sizeof (CPSS_VOS_MSG_NODE_T) + msgLen)
#define VOS_MSG_NODE_DATA(pNode) \
        (((INT8 *) pNode) + sizeof (CPSS_VOS_MSG_NODE_T))

/* 消息队列描述 */

/******************************** 类型定义 ***********************************/

/* 消息队列 */

#ifdef CPSS_VOS_WINDOWS
typedef struct tagCPSS_VOS_MSG_Q
    {
    INT32   lMaxMsgs; /* 最大消息数 */
    INT32   lMaxMsgLen; /* 最大消息长度 */
    INT32   lOptions; /* 选项 */
    CPSS_UTIL_Q_FIFO_HEAD_T qFree; /* 空头FIFO消息队列*/
    CPSS_UTIL_Q_FIFO_HEAD_T qMsg; /* 消息 FIFO消息队列*/
    INT32   lMsgNum; /* 消息数 */
    UINT32  ulSemDescRecv; /* 接收信号 */
    UINT32  ulSemDescSend; /* 发送信号 */
    UINT32  ulSemDescAccess; /* 访问信号 */
    } CPSS_VOS_MSG_Q_T;
#endif

#ifdef CPSS_DSP_CPU
typedef struct tagCPSS_VOS_MSG_Q
    {
    INT32   lMaxMsgs; /* 最大消息数 */
    INT32   lMaxMsgLen; /* 最大消息长度 */
    INT32   lOptions; /* 选项 */
    CPSS_UTIL_Q_FIFO_HEAD_T qFree; /* 空头FIFO消息队列*/
    CPSS_UTIL_Q_FIFO_HEAD_T qMsg; /* 消息 FIFO消息队列*/
    INT32   lMsgNum; /* 消息数 */
    } CPSS_VOS_MSG_Q_T;
#endif

#ifdef CPSS_VOS_LINUX
typedef struct tagCPSS_VOS_MSG_Q
    {
    INT32   lMaxMsgs; /* 最大消息数 */
    INT32   lMaxMsgLen; /* 最大消息长度 */
    INT32   lOptions; /* 选项 */
    CPSS_UTIL_Q_FIFO_HEAD_T qFree; /* 空头FIFO消息队列*/
    CPSS_UTIL_Q_FIFO_HEAD_T qMsg; /* 消息 FIFO消息队列*/
    INT32   lMsgNum; /* 消息数 */
    UINT32  ulSemDescRecv; /* 接收信号 */
    UINT32  ulSemDescSend; /* 发送信号 */
    UINT32  ulSemDescAccess; /* 访问信号 */
    } CPSS_VOS_MSG_Q_T;
#endif

/* 消息队列ID */

#ifdef CPSS_VOS_VXWORKS
typedef MSG_Q_ID    VOS_MSG_Q_ID;
#endif

#ifdef CPSS_VOS_WINDOWS
typedef struct tagCPSS_VOS_MSG_Q *    VOS_MSG_Q_ID;
#endif

#ifdef CPSS_DSP_CPU
typedef struct tagCPSS_VOS_MSG_Q *    VOS_MSG_Q_ID;
#endif

#ifdef CPSS_VOS_LINUX
typedef struct tagCPSS_VOS_MSG_Q *    VOS_MSG_Q_ID;
#endif

/* 消息结点*/

typedef struct tagCPSS_VOS_MSG_NODE
    {
    CPSS_UTIL_Q_FIFO_NODE_T node;   /* 结点*/
    INT32   lMsgLen;                /* 数据的字节数 */
    } CPSS_VOS_MSG_NODE_T;

/* 消息队列描述 */

typedef struct tagCPSS_VOS_MSG_Q_DESC
    {
    INT8    acName [VOS_MSG_Q_NAME_LEN]; /* 名称*/
    VOS_MSG_Q_ID    msgQId; /* 消息队列ID*/
    BOOL    bInuse; /*使用状态0：空闲 1:使用 */
    BOOL	bTimer;
    } CPSS_VOS_MSG_Q_DESC_T;

/* 消息队列信息*/

typedef struct tagCPSS_VOS_MSG_Q_INFO
    {
    UINT32  ulMsgQDesc; /* 消息队列描述 */
    INT8 *  pcName; /* 名字*/
    VOS_MSG_Q_ID    msgQId; /* 消息队列ID*/
    INT32   lMsgNum; /* 消息数 */
    INT32   lMaxMsgs; /* 最大消息数 */
    INT32   lMaxMsgLen; /* 最大消息长度 */
    INT32   lOptions; /* 选项 */
    } CPSS_VOS_MSG_Q_INFO_T;

/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/

/******************************** 头文件保护结尾 *****************************/
#ifdef __cplusplus
}
#endif

#endif /* CPSS_OVS_MSG_H*/
/******************************** 头文件结束 *********************************/


