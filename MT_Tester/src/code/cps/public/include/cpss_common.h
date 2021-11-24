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
#ifndef CPSS_COMMON_H
#define CPSS_COMMON_H

#ifdef  __cplusplus
extern "C" {
#endif

/********************************* 头文件包含 ********************************/
#include "cpss_type.h"
/*#include "cps__oams_public.h"*/
#include "cpss_config.h"

#ifdef CPSS_VOS_VXWORKS
#include "in.h"
#include "ramDrv.h"
#include "ftpLib.h"
#include "sockLib.h"
#include "errnoLib.h"
#include "ioLib.h"
#include "tftpdLib.h"
#include "inetLib.h"
#include "usrLib.h"
#include "usrConfig.h"
#endif

/******************************** 宏和常量定义 *******************************/

    /* 是否支持通信帧头中包含时间戳功能 */
#if 0
#define CPSS_COM_HDR_WITH_TIMESTAMP
/* 是否支持路径跟踪 */
#define CPSS_TRACE_ROUTE_FLAG TRUE
#endif

#define CPSS_COM_LOGIC_ADDR_LOCAL     0x00000000
/*CPU 测试码*/
#define CPSS_DBG_CPU_TEST_CODE  (0x00020001)
/*内存测试码*/
#define CPSS_DBG_MEM_TEST_CODE  (0x00020002)
/* 时钟测试码*/
#define CPSS_DBG_CLOCK_TEST_CODE  (0x00020003)
/* 磁盘空间测试码*/
#define CPSS_DBG_DISK_SPACE_TEST_CODE  (0x00020005) 
#define CPSS_DBG_DIR_INFO_TEST_CODE  (0x00020006)
/* CPSS内部诊断测试时使用的打印开关。 */
#define CPSS_PRINT_LEVEL_DIAG       ((UINT8)0x20)
#define CPSS_PRINT_LEVEL_DIAG_BF ((UINT8)0x21)
/* 自定义诊断模块 */
#define CPSS_MODULE_DRV_UDP (0x0a)

/************************* 时间戳相关宏定义 *******************************/
#define CPSS_COM_TIMESTAMP_LOC_APPI  (0)
#define CPSS_COM_TIMESTAMP_LOC_DRVSI (1)
#define CPSS_COM_TIMESTAMP_LOC_DRVRI (2)
#define CPSS_COM_TIMESTAMP_LOC_IPCI  (3)
#define CPSS_COM_TIMESTAMP_LOC_SCHI  (4)
#define CPSS_COM_TIMESTAMP_LOC_MAX   (5)

#ifdef CPSS_COM_HDR_WITH_TIMESTAMP
#define cpss_com_timestamp_add_hook_app(pstTrustHdr) (cpss_com_timestamp_add(CPSS_COM_TIMESTAMP_LOC_APPI, pstTrustHdr))
#define cpss_com_timestamp_add_hook_drvs(pstTrustHdr) (cpss_com_timestamp_add(CPSS_COM_TIMESTAMP_LOC_DRVSI, pstTrustHdr))
#define cpss_com_timestamp_add_hook_drvr(pstTrustHdr) (cpss_com_timestamp_add(CPSS_COM_TIMESTAMP_LOC_DRVRI, pstTrustHdr))
#define cpss_com_timestamp_add_hook_ipc(pstTrustHdr) (cpss_com_timestamp_add(CPSS_COM_TIMESTAMP_LOC_IPCI, pstTrustHdr))
#define cpss_com_timestamp_add_hook_sch(pstTrustHdr) (cpss_com_timestamp_add(CPSS_COM_TIMESTAMP_LOC_SCHI, pstTrustHdr))
#else
#define cpss_com_timestamp_add_hook_app(pstTrustHdr)
#define cpss_com_timestamp_add_hook_drvs(pstTrustHdr)
#define cpss_com_timestamp_add_hook_drvr(pstTrustHdr)
#define cpss_com_timestamp_add_hook_ipc(pstTrustHdr)
#define cpss_com_timestamp_add_hook_sch(pstTrustHdr)
#endif

#define CPSS_COM_TIMESTAMP_FILE ("c:\\timestamp.txt")
#define CPSS_COM_TIMESTAMP_STAT_NUM_MAX (10000)

/* 和通信定义的优先级不一致，需要调整。UINT8* pucPri */
#define cpss_vk_ipc_pri_map(pucPri) \
{\
    if (*(pucPri) > 0)\
    {\
        (*(pucPri)) = (*(pucPri)) - 1;\
    } \
}\

/******************************************************************
                  CPSS内部消息类型定义开始
******************************************************************/
/* 0x73000000～0x73ffffff	CPSS内部接口消息 */

/* 子系统内消息定义 */
#define CPSS_COM_LINK_HEARTBEAT_MSG      0x73000001 /* 链路心跳消息 */
#define CPSS_COM_INIT_MSG                0x73000101 /* 初始化消息 */
#define CPSS_COM_RARP_REQ_MSG            0x73000102 /* 反向地址解析请求消息 */
#define CPSS_COM_RARP_RSP_MSG            0x73000103 /* 反向地址解析响应消息 */
#define CPSS_COM_ARP_REQ_MSG             0x73000201 /* 地址解析请求消息 */
#define CPSS_COM_ARP_RSP_MSG             0x73000202 /* 地址解析响应消息 */
#define CPSS_COM_LINK_REQ_MSG            0x73000203 /* 建立链路请求消息 */
#define CPSS_COM_LINK_RSP_MSG            0x73000204 /* 建立链路响应消息 */
#define CPSS_COM_APP_ARP_REQ_MSG         0x73000301 /* 应用接口发送给链路管理纤程的地址解析请求消息 */
#define CPSS_COM_MATE_CHANNEL_REQ_MSG    0x73000401 /* 主备通道建立请求消息 */
#define CPSS_COM_MATE_CHANNEL_RSP_MSG    0x73000402 /* 主备通道建立响应消息 */
#define COM_SLID_PROC_SEND_MSG           0x73000501    /*可靠数据发送消息*/
#define COM_SLID_PROC_SEND_LINK_DATA_MSG 0x73000502    /*发送链路激活消息*/
#define COM_SLIE_WIN_ENABLE_MSG          0x73000503    /*滑窗激活消息*/
#define CPSS_COM_SCPU_REG_REQ_MSG        0x73000601
#define CPSS_COM_MCPU_REG_RSP_MSG        0x73000603
#define CPSS_COM_SEND_DATA_STOR_REQ_MSG  0x73000701      /*数据发送缓存请求*/
#define CPSS_COM_SLID_NORMAL_IND_MSG     0x73000702      /*滑窗状态正常时的通知消息*/
#define CPSS_COM_GET_LOGIC_ADDR_REQ_MSG  0x73000801
#define CPSS_COM_GET_LOGIC_ADDR_RSP_MSG  0x73000802
#define CPSS_COM_COMM_TEST_REQ_MSG       0x73000803
#define CPSS_COM_COMM_TEST_RSP_MSG       0x73000804
#define CPSS_COM_DSP_CPU_USAGE_MSG       0x73000805 /*DSP的CPU占用率查询消息*/
#define CPSS_PRTTIMER_CREATE_MSG         0x73100001 /* 打印流控定时器创建消息 */
#define CPSS_PRINT_ADD_MSG               0x73100002 /* 打印信息加入缓存消息 */
#define CPSS_LOGTIMER_CREATE_MSG         0x73100004 /* 日志流控定时器创建消息 */
#define CPSS_LOG_ADD_MSG                 0x73100008 /* 日志加入缓存消息 */
#define CPSS_DBG_INIT_LOG_MSG            0X73100009  /* 初始日志*/
#define CPSS_TRACE_ADD_MSG               0x7310000a /* 消息跟踪缓存消息 */
#define CPSS_TRACE_TASK_ADD_MSG          0x7310000b /* 消息跟踪任务增加消息 */
#define CPSS_TRACE_TASK_DEL_MSG          0x7310000c /* 消息跟踪任务删除消息 */ 
#define CPSS_TRACE_TASK_RATE_SET_MSG     0x7310000d /* 设置消息上报速度接口 */
#define CPSS_COM_TCP_SEND_REQ_MSG        0x731000010 /* TCP发送数据请求消息 */

/* 诊断测试的消息发送测试消息ID */
#define CPSS_DBG_DIAGN_MSGID               (0x731fffff)
#define CPSS_KW_DEAD_LOOP_NOTIFY_MSG        0x73200001 /*死循环通知*/
#define CPSS_KW_DOG_TIMER_EXPIRE_MSG        0x73200002 /* watch dog 超时*/
#define CPSS_KW_EXCEPTION_NOTIFY_MSG        0x73200003 /*异常通知*/
#define CPSS_SBBR_TRANSACTION_MSG           0x73200005  /* 内部启动SBBR上传消息*/
#define CPSS_SBBR_DSP_TRANSACTION_MSG       0x73200006
/* 发往dsp的打印控制消息*/
#define CPSS_CPS__OAMS_PRT_LEVEL_SET_MSG   0x73400001
#define CPSS_CPS__OAMS_REDIRECT_SET_MSG    0x73400002
#define CPSS_CPS__OAMS_PRT_RATE_SET_MSG    0x73400003
#define CPSS_CPS__OAMS_LOG_RATE_SET_MSG    0x73400004
#define CPSS_CPS__OAMS_LOG_LEVEL_SET_MSG   0x73400005

#define CPSS_BYTE_ORDER_BIG_ENDIAN    0
#define CPSS_BYTE_ORDER_LITTLE_ENDIAN 1

/******************************************************************
                  CPSS内部消息类型定义结束
******************************************************************/


/******************************** 类型定义 ***********************************/
/****  诊断测试数据类型 ***** 李军2006/06/09 增加*/


/*dsp 黑匣子功能相关结构和宏定义*/
typedef struct
{
    UINT32 ulMsgId;
    INT32 lMargin; /* 传输的时间差 */
    UINT32 ulTimeStamp[CPSS_COM_TIMESTAMP_LOC_MAX];
}CPSS_COM_TIMESTAMP_STAT_T;
#define CPSS_SBBR_MSG_FLOW_MAX     128
#define CPSS_SBBR_MSG_RECORD_MAX   10
typedef struct
{
     struct
    {
        UINT32 SeqNum;
        UINT32 MsgId;
        UINT32 CurTimer;
        UINT32 CurTick;
        UINT32 Len;
        UINT8  ucMsgCotent[CPSS_SBBR_MSG_FLOW_MAX];/* 消息码流 */
    }astMsgCount[CPSS_SBBR_MSG_RECORD_MAX];
    
    UINT32 ulCpssInitInCount;
    UINT32 ulCpssInitOutCount;
    UINT32 ulVkActivateInCount;
    UINT32 ulVkActivateOutCount;
    UINT32 ulComInitInCount;
    UINT32 ulComInitOutCount;
    UINT32 ulDrvPciInitInCount;
    UINT32 ulDrvPciInitOutCount;
    UINT32 ulPciNotifyInCount;
    UINT32 ulPciNotifyOutCount;
    UINT32 ulRegRspInCount;
    UINT32 ulRegRspOutCount;
    UINT32 ulPowerOnInCount;
    UINT32 ulPowerOnOutCount;
    UINT32 ulComSendInCount;
    UINT32 ulComSendOutCount;
    UINT32 ulMemFreeInCount;
    UINT32 ulMemFreeOutCount;	
}CPSS_SBBR_DSP_RECORD_T;

/* IPC消息的错误统计 */
typedef struct tagCPSS_IPC_MSG_SEND
{
    UINT32 ulIpcMsgNull;
    UINT32 ulGuidOverFlow;
    UINT32 ulInstOverFlow;
    UINT32 ulIndexOverFlow;
    UINT32 ulSchedDescOverFlow;
    UINT32 ulMsgQSendError;
}CPSS_IPC_MSG_SEND_T;

extern CPSS_SBBR_DSP_RECORD_T *g_pstCpssSbbrRecord;

/****  诊断测试数据类型 结束*****/



/******************************** 全局变量声明 *******************************/

/******************************** 函数原形声明 ***************************/
extern INT32 cpss_active_proc_rsp_send
(
    INT32 lResult
);
extern INT32 cpss_standby_to_active_send
(
    INT32  lResult
);
extern INT32 cpss_output
(
    UINT8  ucModuleID,
    UINT8  ucPrintLevel,
    STRING  szFormat,
    ...
);

/* 判断本板是否为主控板 */
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
#define cpss_local_funcbrd_type_is_mc() TRUE
#else
#define cpss_local_funcbrd_type_is_mc() FALSE
#endif

extern VOID cpss_dbg_pl_set(UINT32 ulModuleId, UINT32 ulLev);
extern BOOL cpss_dbg_pl_get(UINT32 ulModuleId);
extern void cpss_tm_current_time_print();
extern void cpss_com_byteflow_print_hook(UINT8 *pucBuf, UINT32 ulLen);
#if 0
extern INT32 cpss_com_byteflow_print(UINT8 ucModuleId, UINT8 *pucBuf, UINT32 ulLen);
#else
INT32 cpss_com_byteflow_print_ex(UINT8 *pucBuf, UINT32 ulLen);
#define cpss_com_byteflow_print(x,y,z) \
    if(cpss_dbg_pl_get(x)) cpss_com_byteflow_print_ex((y),(z));
#endif
extern void cpss_com_bfph_init();
extern VOID cpss_dbg_send(UINT32 ulType, UINT32 ulP1, UINT32 ulP2, UINT32 ulP3, UINT32 ulP4, UINT32 ulP5, UINT32 ulP6);
extern INT32 cpss_com_os_socket_init();

extern VOID cpss_com_timestamp_add();

extern INT32 cpss_com_timestamp_init();
extern void cpss_switch_hook(UINT32 ulPid, UINT32 ulMsgId);

/* 内存池使用信息*/
/*extern  VOID cpss_mem_usage_get(UINT32 *pulCount, CPSS_DBG_MEM_POOL_INFO_T *pstInfo);  / 2006/06/09 李军增加*/

extern INT32 cpss_com_send_local
(
CPSS_COM_MSG_HEAD_T *pstMsg
);

extern BOOL cpss_com_rarp_allow();

extern INT32    cpss_vos_task_delay (INT32 lMs);
extern VOID cpss_byte_order_init();
extern BOOL cpss_com_phyAddr_valid(CPSS_COM_PHY_ADDR_T *pstPhyAddr);
extern INT32 cpss_com_log_addr_get(UINT8 ucModule, CPSS_COM_PHY_ADDR_T *pstPhyAddr, CPSS_COM_LOGIC_ADDR_T *pstLogAddr);

extern INT32 cpss_sbbr_get_dsp(UINT32 ulCpuNo,UINT8 *pcAddr,UINT32 ulSize);
extern INT32 cpss_sbbr_trans_dsp(UINT32 ulCpuNo,UINT8 *pcAddr,UINT32 ulSize);

extern INT32 cps_system(UINT8* pucCmd);

/******************************** 头文件保护结尾 *****************************/
#ifdef  __cplusplus
}
#endif

#endif /* CPSS_COMMON_H */
/******************************** 头文件结束 *********************************/


