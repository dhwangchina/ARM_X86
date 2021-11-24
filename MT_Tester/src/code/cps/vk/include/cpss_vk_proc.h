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
#ifndef __INCcpss_vk_proch
#define __INCcpss_vk_proch

#ifdef __cplusplus
extern "C" {
#endif
 
/********************************* 头文件包含 ********************************/
#include "cpss_type.h"
#include "cpss_common.h" 
#include "cpss_util_q_fifo_lib.h"
#include "cpss_vos_sem.h"
#include "cpss_com.h"

/******************************** 宏和常量定义 *******************************/
/* procedure control block(PCB) */
/* name */
#define CPSS_IPC_MSG_COUNTER
#define VK_PROC_NAME_LEN    32          /* 纤程名字长度 */
/* status */
#define VK_PROC_STATUS_READY    0x00    /* 就绪 */
#define VK_PROC_STATUS_SUSPEND  0x01    /* 显式悬挂 */
#define VK_PROC_STATUS_PEND 0x02        /* 悬挂*/
#define VK_PROC_STATUS_DELAY    0x04    /* 延迟 */
#define VK_PROC_STATUS_DEAD 0x08        /* 死亡 */
/* stack */
#define VK_PROC_DEF_STACK_SIZE  0x300   /* 默认堆栈大小 */
/* private data */
#define VK_PROC_DEF_VAR_SIZE    0x30    /* 默认私有数据区大小 */
/* state */
#define VK_PROC_STATE_INACTIVE  0x0000  /* 非活动 */
#define VK_PROC_STATE_INVALID   0xFFFF  /* 无效 */
/* Inter-Procedure Communication(IPC) message */
/* pointer operation macro */
#define IPC_MSG_NODE_TO_HDR(pNode)  (((INT8 *) pNode) + sizeof (CPSS_VK_IPC_MSG_NODE_T))
#define IPC_MSG_HDR_TO_NODE(pHdr)   (((INT8 *) pHdr) - sizeof (CPSS_VK_IPC_MSG_NODE_T))
#define IPC_MSG_HDR_TO_DATA(pHdr)   (((INT8 *) pHdr) + sizeof (IPC_MSG_HDR_T))
/* flags */
#define IPC_MSG_FLAG_IN_FREE 0x00000000
#define IPC_MSG_FLAG_IN_USE 0x00000001      /* 使用 */
#define IPC_MSG_FLAG_MEM_ALLOC  0x00000002 /* 内存分配: 
                                            * 0 = IPC消息池释放 
                                            * 1 = 内存池释放 
                                            */
#define IPC_MSG_FLAG_EXPIRE 0x00000004      /* 过期 */ 
/* ID */
/*#define IPC_MSG_ID_ACTIVE   0xFFFF0000  active */
#define IPC_MSG_ID_ACTIVE   0x73000101
#define IPC_MSG_ID_RTP      0x73000901 /* real-time patch */

/* type */
#define IPC_MSG_TYPE_ACTIVE 0x1         /* 活动 */
#define IPC_MSG_TYPE_TIMEOUT    0x2     /* 超时 */
#define IPC_MSG_TYPE_LOCAL   0x4        /* 本地 */
#define IPC_MSG_TYPE_REMOTE  0x8        /* 远端 */

#define CPSS_IPC_POOL_SIZE ((IPC_MAX_MSGS)*((sizeof(CPSS_VK_IPC_MSG_NODE_T)) + \
    (sizeof(IPC_MSG_HDR_T)) + (IPC_MAX_MSG_LEN)))
#define CPSS_IPC_MSG_HEAD_LEN  (sizeof(CPSS_VK_IPC_MSG_NODE_T)+sizeof(IPC_MSG_HDR_T))
#define CPSS_IPC_MSG_SIZE (sizeof(CPSS_VK_IPC_MSG_NODE_T)+sizeof(IPC_MSG_HDR_T)+IPC_MAX_MSG_LEN)

/*#define CPSS_IPC_MSG_COUNTER*/

/******************************** 类型定义 ***********************************/
typedef struct vk_proc_class_config_t
    {
    UINT16  usGuid; /* globally unique identifier */
    INT8    acName [32]; /* name */
    INT32   lPri; /* priority */
    INT32   lOptions; /* option */
    INT32   lStackSize; /* stack size */
    INT32   lVarSize; /* variable size */
    VOID_FUNC_PTR   pfEntry; /* entry point of procedure */
    UINT32  ulSchedDesc; /* scheduler descriptor */
    UINT16  usTotalInst; /* total number of instance */
    BOOL    bInuse; /* in use */
    } VK_PROC_CLASS_CONFIG_T;

/* PCB */
typedef struct tagCPSS_VK_PROC_PCB
    {
    CPSS_UTIL_Q_FIFO_NODE_T    node;    /* 节点 */
    INT8    acName [VK_PROC_NAME_LEN];  /* 名字 */
    INT32   lOptions;                   /* 选项 */
    INT32   lStatus;                    /* 状态 */
    INT32   lPri;                       /* 优先级 */
    VOID_FUNC_PTR   pfEntry;            /* 入口点 */
    INT8 *  pcStackBase;                /* 堆栈基 */
    INT8 *  pcStackLimit;               /* 堆栈限制 */
    INT8 *  pcStackEnd;                 /* 堆栈底 */
    INT32   lStackSize;                 /* 堆栈大小 */
    void *  pcVar;                      /* 私有数据区指针 */
    INT32   lVarSize;                   /* 私有数据区大小*/
    CPSS_UTIL_Q_FIFO_HEAD_T    mailBox; /* 邮箱 */
    INT32   lMailNum;                   /* 消息数目 */
    UINT32  ulHash;                     /* hash */
    UINT16  usState;                    /* 业务状态 */
    UINT32  ulParaTMCBHead;
    UINT32  ulParaTMCBAVLRoot;    
    UINT32  aulTimerId [MAX_PROC_TIMER];  
    UINT32  aulParaRoot[MAX_PROC_TIMER];
    UINT16  usTimerCounts;
    UINT16  usTimerPeak;
    UINT32 ulTimerPeakGmt;
    INT32   lMaxTicks; /* maximal ticks */
    UINT32 ulProcNTimee;
    UINT32 ulProcNTimeb;
    UINT32 ulProcNTime;
    UINT32 ulDirCopyFiles;
    } CPSS_VK_PROC_PCB_T;

/* procedure ID */

typedef struct tagCPSS_VK_PROC_PCB *  VK_PROC_ID;

/* procedure descriptor entry */

typedef struct tagCPSS_VK_PROC_DESC
    {
    CPSS_UTIL_Q_FIFO_NODE_T    node;    /* 节点 */
    VK_PROC_ID  tProcId;                /* 纤程标识 */
    UINT32  ulIndex;                    /* 索引 */
    BOOL    bInuse;                     /* 占用标志 */
    } CPSS_VK_PROC_DESC_T;

/* procedure information */

typedef struct tagCPSS_VK_PROC_INFO
    {
    UINT32  ulProcDesc;         /* 纤程描述符 */
    VK_PROC_ID  tProcId;        /* 纤程标识 */
    INT8 *  pcName;             /* 名字 */
    INT32   lOptions;           /* 选项 */
    INT32   lPri;               /* 优先级 */
    INT32   lStatus;            /* 状态 */
    INT8 *  pcStackBase;        /* 堆栈基 */    
    INT8 *  pcStackLimit;       /* 堆栈大小 */  
    INT8 *  pcStackEnd;         /* 堆栈底 */ 
    INT32   lStackSize;         /* 堆栈大小 */
    void *  pcVar;              /* 私有数据区指针 */
    INT32   lVarSize;           /* 私有数据区大小 */
    INT32   lMaxTicks; /* maximal ticks */
    UINT32 ulProcNTimee;
    UINT32 ulProcNTimeb;
    UINT32 ulProcNTime;

    } CPSS_VK_PROC_INFO_T;

/* 
 * procedure statistics: after read, the following will be clean and count again
 */ 

typedef struct tagCPSS_VK_PROC_STAT
    {
    UINT32  ulInteCallNum;      /* 周期内调用数目 */
    UINT32  ulInteMailSent;     /* 周期内发送消息数目 */
    UINT32  ulInteBytesSent;    /* 周期内发送字节数目 */
    UINT32  ulInteMailRecv;     /* 周期内接收消息数目 */
    UINT32  ulInteBytesRecv;    /* 周期内接收字节数目 */
    UINT32  ulTotalCallNum;     /* 总的调用数目 */
    UINT32  ulTotalMailSent;    /* 总的发送消息数目 */
    UINT32  ulTotalBytesSent;   /* 总的发送字节数目*/
    UINT32  ulTotalMailRecv;    /* 总的接收消息数目 */
    UINT32  ulTotalBytesRecv;   /* 总的接收字节数目 */
    } CPSS_VK_PROC_STAT_T;

/* procedure class */

typedef struct tagCPSS_VK_PROC_CLASS
    {
    CPSS_UTIL_Q_FIFO_HEAD_T    qFree;   /* FREE队列 */
    CPSS_UTIL_Q_FIFO_HEAD_T    qInuse;  /* INUSE队列 */
    UINT16  usGuid;                     /* 全局标识 */
    INT8    acName [VK_PROC_NAME_LEN];  /* 名字 */
    VOID_FUNC_PTR   pfEntry;            /* 入口点 */
    INT32   lPri;                       /* 优先级 */
    INT32   lOptions;                   /* 选项 */
    INT32   lStackSize;                 /* 堆栈大小 */
    INT32   lVarSize;                   /* 私有数据区大小 */
    UINT32  ulBegin;                    /* 开始位置 */
    UINT16  usTotalInst;                /* 总的实例数目 */
    UINT16  usCurrInst;                 /* 当前实例数目 */
    UINT32  ulSchedDesc;                /* 调度器描述符scheduler descriptor */
    BOOL    bInuse;                     /* 占用标志 */
    INT32   lKwTmOutLimitType;
    UINT32 ulKwId;	
   UINT32 ulRunTicks;
    } CPSS_VK_PROC_CLASS_T;

/* Inter-Procedure Communication(IPC) message pool */

typedef struct tagCPSS_VK_IPC_MSG_POOL
    {
    CPSS_UTIL_Q_FIFO_HEAD_T    qFree;   /* FREE队列 */
    CPSS_UTIL_Q_FIFO_HEAD_T    qData;   /* DATA队列 */
    UINT32  ulSemDesc;                  /* 信号量 */
    INT32   lMaxMsgs;                   /* 最大消息数目 */
    INT32   lMaxMsgLen;                 /* 消息最大长度 */
    INT32   lCurrMsgsAlloc;             /* 当前分配出去的数目 */
    INT32   lReFreeNum ;                /* 重复释放IPC内存的次数 */
    INT32   lPeakMsgNum;                /* 统计消息的峰值 */
    } CPSS_VK_IPC_MSG_POOL_T;

/* Inter-Procedure Communication(IPC) message pool ID */

typedef struct tagCPSS_VK_IPC_MSG_POOL * IPC_MSG_POOL_ID;

/* Inter-Procedure Communication(IPC) message node */

typedef struct tagCPSS_VK_IPC_MSG_NODE
    {
    CPSS_UTIL_Q_FIFO_NODE_T node;   /* 节点 */
    INT32  lFlags;                  /* 标志 */
#ifdef CPSS_IPC_MSG_COUNTER
    UINT32 ulTaskId;
    UINT32 ulProcId;
#endif
    } CPSS_VK_IPC_MSG_NODE_T;

typedef struct ipc_msg_hdr_t
    {
    struct ipc_msg_hdr_t *  ptNext; /* 指向下一个消息 */
    INT32   lTimeout;               /* 超时 */

#ifdef CPSS_VOS_VXWORKS
#if _BYTE_ORDER == _LITTLE_ENDIAN    	
    UINT8   hdrLen:5;               /* 头长度 */
    UINT8   ver:3;                  /* 版本 */
#endif                              /* _BYTE_ORDER == _LITTLE_ENDIAN */
#if _BYTE_ORDER == _BIG_ENDIAN
    UINT8   ver:3;                  /* 版本 */
    UINT8   hdrLen:5;               /* 头长度 */
#endif /* _BYTE_ORDER == _BIG_ENDIAN */
#endif /* CPSS_VOS_VXWORKS */

#ifdef CPSS_VOS_WINDOWS    	
    UINT8   hdrLen:5;               /* 头长度 */
    UINT8   ver:3;                  /* 版本 */
#endif /* CPSS_VOS_WINDOWS */

    UINT8  ucPType;                 /* 类型 */

#ifdef CPSS_VOS_VXWORKS
#if _BYTE_ORDER == _LITTLE_ENDIAN    	
    UINT8   fragOffset:5 ;
    UINT8   fragFlag:3 ;    
#endif
#if _BYTE_ORDER == _BIG_ENDIAN
    UINT8   fragFlag:3 ;
    UINT8   fragOffset:5 ;
#endif
#endif

#ifdef CPSS_VOS_WINDOWS    	
    UINT8   fragOffset:5 ;
    UINT8   fragFlag:3 ;
#endif
    
    UINT8   fragOffsetEx ; /*数据片偏移量的低8位*/

    UINT16  usReserved;
    UINT16  usSliceLen;             /* 片长度 */
    UINT16  usSeq;                  /* 序号 */
    UINT16  usAck;                  /* 确认 */
    UINT16  usFlowCtrl;             /* 流控 */ 
    UINT16  usCheckSum;             /* 头长度 */
#ifdef CPSS_COM_HDR_WITH_TIMESTAMP
    UINT32  ulTimeStamp[10];
#endif
#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
    CPSS_TRACE_INFO_T  stTimeStamp;
#endif

    } IPC_MSG_HDR_T;

/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/
extern INT32    cpss_vk_proc_lib_init (void);
extern UINT32   cpss_vk_proc_spawn (UINT16 usGuid, UINT16 usInst);
extern INT32    cpss_vk_proc_delete (UINT32 usProcDesc);
extern INT32    cpss_vk_proc_suspend (UINT32 usProcDesc);
extern CPSS_VK_PROC_PCB_T *cpss_vk_proc_pcb_get (UINT32 ulProcDesc);
extern INT32    cpss_vk_proc_resume (UINT32 usProcDesc);
extern INT32    cpss_vk_proc_ticks_flush (UINT32 usProcDesc);
extern UINT32   cpss_vk_proc_sched_get (UINT32 usProcDesc);
extern INT32    cpss_vk_proc_activate (UINT32 usProcDesc);
extern INT32    cpss_vk_proc_info_get (UINT32 usProcDesc, 
                                       CPSS_VK_PROC_INFO_T *ptProcInfo);
extern INT32    cpss_vk_proc_stat_get (UINT32 usProcDesc, 
                                       CPSS_VK_PROC_STAT_T *ptProcStat);
extern INT32    cpss_vk_proc_tatus_string (UINT32 usProcDesc, INT8 *pcString);
extern INT32    cpss_vk_proc_show (UINT16 usGuid, UINT32 usProcDesc, 
                                   INT32 ulLevel);
extern INT32    cpss_ipc_msg_pool_create (void);
extern void *   cpss_ipc_msg_alloc (INT32 lNBytes);
extern void     cpss_ipc_msg_free (void *ptr);
extern void     cpss_ipc_msg_expire (void *ptr);
extern INT32    cpss_ipc_msg_send (IPC_MSG_HDR_T *ptHdr, INT32 lType);
extern INT32    cpss_ipc_msg_receive (IPC_MSG_HDR_T *ptHdr);
extern INT32    cpss_vk_ipc_msg_num_get();

#ifdef CPSS_IPC_MSG_COUNTER
extern VOID cpss_vk_proc_info_add(CPSS_VK_IPC_MSG_NODE_T *  ptNode) ;
#endif

/******************************** 头文件保护结尾 *****************************/
#ifdef __cplusplus
}
#endif

#endif /* __INCcpss_vk_proch */

/******************************** 头文件结束 *********************************/

