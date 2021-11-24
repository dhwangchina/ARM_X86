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
#include "smss_public.h"
//#include "om_public.h"
#include "smss_sysctl.h"
#include "smss_verm.h"
#include "smss_devm.h"
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
#include "smss_sysctl_gcpa.h"
#endif

#ifdef SMSS_BACKUP_MATE
#include "smss_sysctl_as.h"
#endif

#ifdef SMSS_BOARD_WITH_SHM
#include "smss_sysctl_shm.h"
#endif

#if  defined(SMSS_BOARD_WITH_DSP) || defined(SMSS_BOARD_WITH_SLAVE)
#include "smss_sysctl_slave.h"
#endif

#include <sys/statfs.h>

/*******************************局部宏定义 **********************************/
#define MAX_MSG_STREAM_RECORD_SUM 100
#define CPSS_RM_TEST_PRINT_COLUM_SUM 48
#define SMSS_AS_CMP_REQ_MSG             0x75010081 /* 主备竞争请求消息 */
#define SMSS_AS_CMP_RSP_MSG             0x75010082 /* 主备竞争响应消息 */
#define SMSS_HEARTBEAT_UP_MSG           0x75010091 /* 向上级发送的心跳消息 */
#define SMSS_HEARTBEAT_DOWN_MSG         0x75010092 /* 向下级发送的心跳消息 */
#define SMSS_HEARTBEAT_MATE_MSG         0x75010093 /* 向伙伴板发送的心跳消息 */

/* 主备竞争定时器 */
#define SMSS_TIMER_AS_CMP               CPSS_TIMER_20
#define SMSS_TIMER_AS_CMP_MSG           CPSS_TIMER_20_MSG
#define SMSS_TIMER_AS_CMP_DELAY         1000 /* 主备竞争定时器超时时间：1s */

#define SMSS_AS_CMP_TIMEOUT             1    /* 主备竞争请求消息最多发送5次 */

/* 心跳基准定时器 */
#define SMSS_TIMER_HB_BASE              CPSS_TIMER_21
#define SMSS_TIMER_HB_BASE_MSG          CPSS_TIMER_21_MSG
#define SMSS_TIMER_HB_BASE_DELAY        500 /* 基准定时器超时时间：500ms */
#define SMSS_HB_UP_TIMEOUT              1
#define SMSS_HB_DOWN_TIMEOUT            1
#define SMSS_HB_MATE_TIMEOUT            1
#define SMSS_HB_UP_LOST_MAX             6
#define SMSS_HB_DOWN_LOST_MAX           6
#define SMSS_HB_MATE_LOST_MAX           6
#define SMSS_HB_DOWN_DSP_TIMEROUT       20
UINT8 g_ucHbUpLostMax   = SMSS_HB_UP_LOST_MAX;
UINT8 g_ucHbDownLostMax = SMSS_HB_DOWN_LOST_MAX;
UINT8 g_ucHbMateLostMax = SMSS_HB_MATE_LOST_MAX;

UINT32 g_ulPowerOnFlag = 0;

#if (SWP_OS_WINDOWS == SWP_OS_TYPE)
#define tickGet GetTickCount
#else

#if (SWP_CPUROLE_DSP == SWP_CPUROLE_TYPE)
#define tickGet cpss_tick_get
#endif


#if (SWP_OS_LINUX == SWP_OS_TYPE)
#include<sys/times.h>
UINT32 tickGet(void)
{
	struct tms tm;
	return times(&tm);
}
#endif

#endif

#define smss_print \
        if (g_bHbPrint) cpss_print

/*******************************全局变量定义/初始化 *************************/
SMSS_CPU_BASE_INFO_T g_stSmssBaseInfo; /* 保存本CPU的基本信息 */
SMSS_AS_INFO_T       g_stSmssAsInfo;   /* 保存本板的主备相关信息 */
UINT32 g_ulSmssSendIndex = 0;          /* 索引当前纤程     */
UINT32 g_ulSmssRspedProcNum = 0;       /* 已经应答的纤程数 */
BOOL   g_bRunGpHeartBeat = FALSE;       /* 是否进行全局板与外围板间的心跳 */
BOOL   g_bShowHeartbeatList = FALSE;   /* 是否打印心跳列表 */
UINT8  g_ucSmssIdleStage;              /* 保存Idle态下的各阶段 */
UINT8  g_ucSmssSendCount = 0;          /* 重发计数器 */
UINT8  g_ucBoardState = SMSS_BRD_RSTATUS_INVALID; /* 保存本板状态 */
INT32  g_lSetTimerResult = 0;
BOOL g_bHbPrint = TRUE;         /* 是否打印心跳 */
BOOL g_bHbMonitor = TRUE;       /* 是否监测心跳，缺省为TRUE，仅当调试时可设为FALSE */
UINT32 g_ulHbUpSeqNum = 0;      /* 向上级发送的心跳消息中的序列号 */
UINT32 g_ulHbDownSeqNum = 0;    /* 向下级发送的心跳消息中的序列号 */
UINT32 g_ulHbMateSeqNum = 0;    /* 向伙伴板发送的心跳消息中的序列号 */
SMSS_HEART_BRD_T g_stSmssHBTimerInfo;
SMSS_PROC_TABLE_ITEM_T *g_pstSmssSysctlProcTable = NULL;
UINT32 g_ulSmssSysctlProcNum;
UINT32 g_ulResetSlave = 1;
UINT32 g_ulMsgStreamRecordSum = 0;      /*消息跟踪记录个数*/
UINT32 g_aulMsgStreamRecordSum[MAX_MSG_STREAM_RECORD_SUM];

SMSS_VERM_VERINFO_T g_stAboxReqVerInfo;            /* 告警箱当前版本信息 */
SMSS_VERM_VERINFO_T g_stAboxCurVerInfo;            /* 告警箱当前版本信息 */
STRING szAboxVerInfoFile = "F:/smss/version/verinfo.dat";
extern UINT32 g_ulBoardVerState;
SMSS_ABOX_ROUT_TEMP_T g_stAboxRout;

VOID smss_abox_route_init();
static STRING szRoutInfoFile = "F:/smss/route.dat";
VOID smss_abox_rev_ver_update(CPSS_COM_MSG_HEAD_T *pstMsg);
UINT8 g_ucDspAlarmFlag[SMSS_SCPU_DSP_ALARM_SUBNO_SUM][SMSS_DSP_MAX_SUM];
UINT8 g_ucSlaveAlarmFlag[SMSS_SCPU_DSP_ALARM_SUBNO_SUM];
UINT32 g_smss_upperclk_brdslot = 0;

#if  defined(SMSS_BOARD_WITH_DSP)
INT32 smss_dsp_sbbr_show(UINT32 ulDspId);

INT32 smss_showdspinfo(VOID);

#endif

#if (SWP_CPUROLE_DSP == SWP_CPUROLE_TYPE)
SMSS_SBBR_MSG_RECORD_T *g_pstSmssSbbrRecord = (SMSS_SBBR_MSG_RECORD_T *) CPSS_DSP_SBBR_SMSS_START_ADDR; 
UINT32 g_ulSmssSubSystemMsgNum = 0;
UINT32 g_ulSmssCounter = 0;
#endif
INT32 smss_showrstreason(VOID);
INT32 smss_showproc(VOID);
INT32 smss_showphyver(UINT8 ucFrame, UINT8 ucShelf, UINT8 ucSlot);
#ifdef NO_RSN  /* 根据测试诊断手册，删除设置和重设置RSN功能 */
extern INT32 smss_devm_gcpa_readRsn(VOID);
#endif

extern UINT32 smss_show_slave_dsp(VOID);
/* DSP异常时CPSS记录信息 */
#ifdef SMSS_BOARD_WITH_DSP
typedef struct
{
  CPSS_TIME_T  stClock;
  UINT32 ulMacCall;
  UINT32 ulTimerScan;
  UINT32 ulPciRecv;
  UINT32 ulChipPoll;
  UINT32 ulProcRun;
  CPSS_TIME_T  stClock2;
  UINT32 ulMacCall2;
  UINT32 ulTimerScan2;
  UINT32 ulPciRecv2;
  UINT32 ulChipPoll2;
  UINT32 ulProcRun2;
}smss_dsp_sbbr_losthb_record_T;
smss_dsp_sbbr_losthb_record_T  g_smss_dsp_losthb_record[SMSSDSPNUM];
#endif


SMSS_SHCMD_INFO_T astSmssSysctlShcmd[] = 
{
#if (SWP_FUNBRD_ABOX != SWP_FUNBRD_TYPE) 
   {"smss_sysctrl_show_rst_brd_reason", "show cause of board rest", "u", (CPS__OAMS_SHCMD_PF)smss_sysctrl_show_rst_brd_reason},
   {"smss_showself", "show local board info", "", (CPS__OAMS_SHCMD_PF)smss_showself},
   {"smss_showmate", "show mate board info", "", (CPS__OAMS_SHCMD_PF)smss_showmate},
   {"smss_showrstreason", "smss_showrstreason", "", (CPS__OAMS_SHCMD_PF)smss_showrstreason},
#endif
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
   {"smss_showbrd", "show all board state", "", (CPS__OAMS_SHCMD_PF)smss_showbrd},
   {"smss_sysctl_gcpa_show_board_info", "show board info by phyaddr on gcpa", "u", (CPS__OAMS_SHCMD_PF)smss_sysctl_gcpa_show_board_info},
   {"smss_showphyver", "smss_showphyver", "uuu", (CPS__OAMS_SHCMD_PF)smss_showphyver},
#endif
   /* {"smss_show_board_info","show board info by phyaddr on any board","u",(CPS__OAMS_SHCMD_PF)smss_show_board_info}, */

#if  defined(SMSS_BOARD_WITH_DSP)
  {"smss_dsp_sbbr_show", "smss_dsp_sbbr_show", "u", (CPS__OAMS_SHCMD_PF)smss_dsp_sbbr_show},

  {"smss_showdspinfo", "smss_showdspinfo", "", (CPS__OAMS_SHCMD_PF)smss_showdspinfo},

#endif  
#if  defined(SMSS_BOARD_WITH_DSP)||defined(SMSS_BOARD_WITH_SLAVE)

  {"smss_show_slave_dsp", "smss_show_slave_dsp", "", (CPS__OAMS_SHCMD_PF)smss_show_slave_dsp},

#endif
  {"smss_showproc", "smss_showproc", "", (CPS__OAMS_SHCMD_PF)smss_showproc},
   {NULL, NULL, NULL, NULL}  /* 结束 */
};

SMSS_CPU_USAGE_INFO_T g_stCpuUsageInfo;

UINT32 g_ulHbMateTime = 0;    /* 接收到伙伴板的心跳次数 */
/*******************************局部常数和类型定义 **************************/
UINT8  ucActProcStage;          /* 保存纤程激活的各阶段 */
UINT32 *pulProcActResult = NULL;/* 保存纤程激活结果 */
/* begin of for GCPA 初始化监控修改 */
UINT32 *pulProcActRsp = NULL;
/* end of for GCPA 初始化监控修改 */

#ifdef SMSS_AS_CMP_SOFT_
UINT8  ucCmpCount = 0;          /* 保存主备竞争次数 */
#endif

/* 主备竞争请求消息 */
typedef struct
{
    UINT8 ucSlot;               /* 本板槽位号 */
} SMSS_AS_CMP_REQ_MSG_T;

/* 主备竞争响应消息 */
typedef struct
{
    UINT8 ucAStatus;            /* 告诉对方的主备状态: SMSS_ACTIVE 或 SMSS_STANDBY */
} SMSS_AS_CMP_RSP_MSG_T;

/* 心跳消息定义 */
typedef struct
{
    UINT32 ulSeqNum;               /* 心跳序号 */
    UINT32 ulAStatus;              /* 全局板主备标志，仅用于全局板向外围板发送的心跳 */
    CPSS_COM_PHY_ADDR_T stPhyAddr; /* 发送者的物理地址，仅用于外围板向全局板发送的心跳
                                      和DSP向主CPU发送的心跳 */
    UINT32 ulFlag;                 /* 单板是否存在RDBS 0:不存在 1:存在 */
} SMSS_HEARTBEAT_MSG_T;

/*单板复位原因定义*/
#define DEV_REPORT_BOARD_FAULT 0 

const UINT8 *passmss_brd_rst_reason[] = 
{   "SMSS_BRD_RST_CAUSE_MATE_FAULT_SLAVE_BRD_IDLE",
    "SMSS_BRD_RST_CAUSE_REG_TIMEOUT",
    "SMSS_BRD_RST_CAUSE_ACT_PROC_CPSS_TIMEOUT_IDLE",
    "SMSS_BRD_RST_CAUSE_RECV_MATA_FAULT_SLAVE_BRD_INIT",          
    "SMSS_BRD_RST_CAUSE_ACT_PROC_FAILURE_SLAVE_BRD_STOA",
    "SMSS_BRD_RST_CAUSE_ACT_PROC_TIMEOUT_SLAVE_BRD_STOA",        
    "SMSS_BRD_RST_CAUSE_STATUS_CHG_IND_TIMEOUT",                  
    "SMSS_BRD_RST_CAUSE_ACT_PROC_TIMEOUT_INIT",                   
    "SMSS_BRD_RST_CAUSE_ALLOC_MEM_FAILURE_INIT",                  
    "SMSS_BRD_RST_CAUSE_LOST_HEARTBEAT_GCPA",                     
    "SMSS_BRD_RST_CAUSE_RECV_DEVM_BRD_FAULT",                     
    "SMSS_BRD_RST_CAUSE_ACT_PROC_CPSS_FAILED_IDLE",               
    "SMSS_BRD_RST_CAUSE_SLAVE_BRD_RECV_BSWITCH_REQ_DISNORMAL",    
    "SMSS_BRD_RST_CAUSE_ACTIVE_BRD_ATOS",                         
    "SMSS_BRD_RST_CAUSE_BSWITCH_IND_TIMEOUT",                     
    "SMSS_BRD_RST_CAUSE_DEVM_REPORT_BRD_FAULT_IND_STOA",          
    "SMSS_BRD_RST_CAUSE_DRV_REPORT_BRD_STAT_CHG_IDLE",            
    "SMSS_BRD_RST_CAUSE_DRV_REPORT_MATA_RST_IND_SLAVE_BRD_IDLE",  
    "SMSS_BRD_RST_CAUSE_BSWITCH_REQ_ACT_BRD_FAULT_SLAVE_BRD_IDLE",
    "SMSS_BRD_RST_CAUSE_DRV_REPORT_BSWITCH_IND_SLAVE_BRD_INIT",   
    "SMSS_BRD_RST_CAUSE_BSWITCH_REQ_ACT_BRD_FAULT_SLAVE_BRD_INIT",
    "SMSS_BRD_RST_CAUSE_ACT_PROC_FAILURE_INIT",
    "SMSS_BRD_RST_CAUSE_WAIT_SWITCH_RSP_TIMEOUT",
    "SMSS_BRD_RST_CAUSE_SAVE_CPS__RDBS_STAT_GCPA_ALLOC_MEM_FAILURE",
    "SMSS_BRD_RST_CAUSE_MSTATE_CHG_ALLOC_MEM_FAILURE",
    "SMSS_BRD_RST_CAUSE_RECV_BRD_REG_ALLOC_MEM_FAILURE",
    "SMSS_BRD_RST_CAUSE_ALLOC_MEM_FAILURE_GCPA_INIT",
    "SMSS_BRD_RST_CAUSE_SLAVE_CPU_FAULT_MASTER_CPU",
    "SMSS_BRD_RST_CAUSE_LOST_HEARTBEAT_MASTER_CPU",
    "SMSS_BRD_RST_CAUSE_ALL_DSP_FAULT",
    "SMSS_BRD_RST_CAUSE_ACTIVATE_VERSION",
    "SMSS_BRD_RST_CAUSE_DRV_INIT_FAIL",
    "SMSS_BRD_RST_CAUSE_ACTIVE_GCPA_VERIFY_FAIL",
    "SMSS_BRD_RST_CAUSE_UL_PORT_FAIL",
    "SMSS_BRD_RST_CAUSE_MEDIA_PORT_FAIL",
    "SMSS_BRD_RST_CAUSE_ME_FAIL",
    "SMSS_BRD_RST_CAUSE_CPLD_UPDATE",
    "SMSS_BRD_RST_CAUSE_HWINFO_FAIL",
    "SMSS_BRD_RST_CAUSE_ALLACTIVE",
    "SMSS_BRD_RST_CAUSE_DRV_REPORT_BSWITCH_IND_NORMAL"
};


UINT32 g_ulBrdRstCauseNum = sizeof(passmss_brd_rst_reason) / sizeof((passmss_brd_rst_reason[0]));
#if (SWP_CPUROLE_DSP != SWP_CPUROLE_TYPE)
/* 消息统计结构, 本结构不统计心跳消息, 否则会影响性能 */
#define SMSS_RECORD_MESSAGE_NUMBER (200)
typedef struct
{
	UINT32 ulMsgId;  /* 记录接收、发送的消息 */
	UINT32 ulCount;  /* 记录接收、发送的消息的个数 */
  UINT32 ulProcId; /* 线程Id */
}SMSS_RECORD_MESSAGE_INFO_T;

SMSS_ABOX_ROUT_T g_stAboxRoute; 

SMSS_RECORD_MESSAGE_INFO_T  g_smss_recv_msg_info[SMSS_RECORD_MESSAGE_NUMBER];
SMSS_RECORD_MESSAGE_INFO_T  g_smss_send_msg_info[SMSS_RECORD_MESSAGE_NUMBER];
#endif

/* 负荷控制信息记录 */
#if (SWP_CPUROLE_DSP != SWP_CPUROLE_TYPE)
typedef struct  /* 主从CPU控制信息 */
{
  CPS__RDBS_DEV_GETCPULOADCTRL_MSG_RSP_T  stRsp;
  UINT16 usWatchNum;  /* 监测周期 */
  UINT16 usCurrNum;   /* 当前次数 */
  UINT8  ucCurrLevel; /* 当前级别 */  
  UINT8  ucCurrUsage; /* 当前CPU占用率*/
  UINT8  aucRsv[2];
  UINT32 ulTotelNum;  /* 用于计算CPU平均占用率 */	  
}SMSS_SYSCTL_CPULOADCTRL_INFO_T;  
SMSS_SYSCTL_CPULOADCTRL_INFO_T  g_smssCpuLoadInfo;
#ifdef SMSS_BOARD_WITH_DSP
 SMSS_SYSCTL_CPULOADCTRL_INFO_T  g_smssDspLoadInfo[SMSSDSPNUM];
#endif
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST) 
typedef struct  /* 单板负荷控制信息 */
{
  CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG_RSP_T  stRsp;
  UINT8 ucSlaveCpu; /* 当前从CPU占用率 */
  UINT8 ucDspCpu;   /* 当前DSP的平均占用率 */
  UINT8 ucCurrLevel;/* 当前级别 */
  UINT8 ucCurrUsage;/* 当前单板的CPU占用率 */	
}SMSS_SYSCTL_BOARDLOAD_CTRL_INFO_T;
SMSS_SYSCTL_BOARDLOAD_CTRL_INFO_T  g_smssBoardLoadInfo;
#endif
#endif
/*******************************局部函数原型明 ******************************/
/* 状态机函数 */
VOID smss_sysctl_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsg);
VOID smss_sysctl_state_idle_func( CPSS_COM_MSG_HEAD_T *pstMsg);
VOID smss_sysctl_state_init_func(CPSS_COM_MSG_HEAD_T *pstMsg);
VOID smss_sysctl_state_normal_func(CPSS_COM_MSG_HEAD_T *pstMsg);
VOID smss_sysctl_state_end_func(CPSS_COM_MSG_HEAD_T *pstMsg);

/* 消息接口函数 */
VOID smss_sysctl_power_on(CPSS_COM_POWER_ON_MSG_T *pstMsg);
VOID smss_sysctl_send_regreq(VOID);
VOID smss_sysctl_recv_regrsp(SMSS_SYSCTL_REG_RSP_MSG_T *pstMsg);
VOID smss_sysctl_state_idle_timeout(VOID);
VOID smss_sysctl_init(VOID);
VOID smss_sysctl_send_act_stoa_req(VOID);
VOID smss_sysctl_recv_act_stoa_rsp(CPSS_COM_MSG_HEAD_T *pstMsg);
VOID smss_sysctl_init_complete(VOID);
VOID smss_sysctl_send_statuschgind(VOID);
VOID smss_sysctl_recv_statuschgind(CPSS_COM_MSG_HEAD_T* pstMsgHead);
VOID smss_sysctl_recv_statuschgack(CPSS_COM_MSG_HEAD_T *pstMsg);
VOID smss_sysctl_state_init_timeout(VOID);
VOID smss_sysctl_recv_gp_heartbeat(SMSS_SYSCTL_GP_HEARTBEAT_IND_T *pstMsg);
VOID smss_sysctl_send_periodly_heartbeat(VOID);
VOID smss_sysctl_hostcpu_send_periodly_heartbeat(VOID);
VOID smss_sysctl_act_stoa_fail(VOID);
VOID smss_sysctl_recv_devmreportfault(VOID);
VOID smss_sysctl_state_end_reset(VOID);
VOID smss_sysctl_reg_procsrsp(UINT16 usIndex, INT32 lResult);
VOID smss_sysctl_broadcast(UINT32 ulMsgId, UINT8 *pucMsg, UINT32 ulMsgLen);
VOID smss_sysctl_enter_normal(VOID);
VOID smss_sysctl_recv_regreq(SMSS_SYSCTL_REG_REQ_MSG_T *pstMsg);
VOID smss_sysctl_recv_bfaultind(SMSS_SYSCTL_BFAULT_IND_MSG_T *pstMsg);
VOID smss_sysctl_idle_recv_actprocrsp(SMSS_PROC_ACTIVATE_RSP_MSG_T * pstMsg);
VOID smss_sysctl_slavecpu_send_periodly_heartbeat(VOID);

VOID smss_sysctl_cpu_usage_init(VOID);
VOID smss_sysctl_cpu_usage_poll(VOID);
BOOL smss_sysctl_pub_func(CPSS_COM_MSG_HEAD_T *pstMsg);
VOID smss_sysctl_actproc_stoarsp_event(UINT32 ulMsgId, UINT32 ulProcId, UINT32 ulResult);
/*API函数 */
INT32 smss_sysctl_procdes2index(UINT32 ulProcDes);
VOID smss_sysctl_send_local(UINT32 ulPd, UINT32 ulMsgId, UINT8 *pucMsg, UINT32 ulMl);
VOID smss_sysctl_send_gcpa(UINT32 ulPd, UINT32 ulMsgId, UINT8 *pucMsg, UINT32 ulMl);
VOID smss_sysctl_send_host(UINT32 ulPd, UINT32 ulMsgId, UINT8 *pucMsg, UINT32 ulMl);
INT32 smss_sysctl_query_cps__rdbs_status(CPSS_COM_PHY_ADDR_T stPhy, UINT8 *ucRStatus,
                                    UINT8 *ucMStatus,UINT8 *ucAStatus);
UINT32 smss_sysctl_set_cps__rdbs_rstatus(CPSS_COM_PHY_ADDR_T stPhy, UINT8 ucValue);
UINT32 smss_sysctl_set_cps__rdbs_astatus(CPSS_COM_PHY_ADDR_T stPhy, UINT8 ucValue);
INT32 smss_sysctl_mo2phy(SWP_MO_ID_T  *pstMoId, CPSS_COM_PHY_ADDR_T *pstPhy);
INT32 smss_sysctl_enter_end(BOOL bResetNow,UINT32 ulResetReason);

/* 告警发送函数 */
VOID  smss_send_alarm_brdnotinstalled_to_oams(UINT32 ulAlarmSubNo, UINT8 ucCurOLStatus, 
                                              CPSS_COM_PHY_ADDR_T stPhyAddr);
VOID  smss_send_alarm_brdnotresponded_to_oams(UINT32 ulAlarmSubNo, UINT32 ulAlarmType, 
                                              CPSS_COM_PHY_ADDR_T stPhyAddr);
VOID  smss_send_alarm_brdfailed_to_oams(SMSS_ACT_STOA_RSP_IND_MSG_T *pstActStoaInd);
VOID  smss_send_alarm_cpufailed_to_oams(SMSS_ACT_STOA_RSP_IND_MSG_T *pstActStoaInd);
VOID  smss_sysctl_actstoa_disp(VOID *pbuf);
VOID  smss_sysctl_hard_cmp_time_handle(VOID);
VOID smss_sysctl_force_reset(SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG_T * pstMsg);
extern void cpss_dbg_pro_time_pause();
VOID smss_sysctl_read_verinfo_file(VOID);
VOID smss_sysctl_write_verinfo_file(VOID);
VOID smss_sysctl_abox_send_act_stoa_req(VOID);
VOID smss_abox_reset();
VOID smss_sysctl_abox_power_on(CPSS_COM_POWER_ON_MSG_T *pstMsg);
VOID smss_sysctl_abox_recv_actprocrsp(CPSS_COM_MSG_HEAD_T *pstMsg);
VOID smss_sysctl_proc_table_init(VOID);
VOID smss_sysctl_state_abox_timeout(VOID);
VOID smss_sysctl_abox_recv_update_swverind(CPSS_COM_MSG_HEAD_T *pstMsg);
VOID smss_sysctl_abox_recv_update_resultind(CPSS_COM_MSG_HEAD_T *pstMsg);
VOID smss_sysctl_oosled_set(UINT8 ucLedState);
VOID smss_sysctl_cpufaultind_msgdisp(CPSS_COM_MSG_HEAD_T *pstMsg);
#ifdef SMSS_BOARD_WITHOUT_RDBS
VOID smss_sysctl_board_info_query_disp(VOID);
#endif

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL
INT32 cpss_file_unpack(const STRING szFileName, const STRING szDstDirName);
INT32 cpss_fs_ftp(CPSS_FTP_REQ_MSG_T* pstFTPReqMsg,UINT32 ulDstPid);
#endif
#if (SWP_CPUROLE_DSP != SWP_CPUROLE_TYPE)
VOID cpss_kw_prt_occupy_log();    
#endif

UINT32 cpss_vk_proc_pid_get_by_name(STRING szProcName);
VOID cpss_sbbr_clr_abnormal_reset_flag(VOID);
INT32 cpss_fs_fd_close();
void smss_sysctl_recv_brdself_fault_ind(CPSS_COM_MSG_HEAD_T *pstMsg);
VOID smss_sysctl_del_dsptimer(SMSS_SYSCTL_DEL_DSP_TIMER_MSG_T *pInd);
INT32 drv_clk_syn_select_byslot(UINT8 ucSlot);
UINT32 cpss_sbbr_get_userid(VOID);
INT32 cpss_sbbr_record(UINT32 ulUserId, UINT8 ucType, UINT32 ulArgs[4],  CHAR *pucData, UINT32 ulSize);
VOID smss_sysctl_gcpa_record_info(CPSS_COM_PHY_ADDR_T stPhyAddr, UINT32 ulAlarmNo,
          UINT32 ulAlarmSubNo,  UINT32 ulObjId, UINT32 ulResult);
VOID smss_sysctl_send_alarm_clean_to_oams(CPSS_COM_PHY_ADDR_T  stPhyAddr);          
#if defined(SMSS_BOARD_WITH_DSP)
VOID smss_sysctl_start_dsp(SMSS_VERM_SLAVE_DSP_START_IND_MSG_T * pstMsg);
#endif
UINT32 smss_sysctl_get_dsp_timer(UINT8 ucDspId);
VOID smss_devm_SetISled(UINT32 ulFlag);
VOID smss_devm_SetActled(UINT32 ulFlag);
VOID smss_sysctl_cpu_usage_timerout(VOID);
VOID smss_showverlen(VOID);
VOID smss_sysctl_cpu_alarm(UINT32 ulCpuUsage, UINT8 ucFlag);
UINT32 smss_sysctl_get_slave_status(VOID);
extern VOID cps_send_om_event(UINT32 ulEventNO, UINT8* pucbuf, UINT32 ulLen);
#if 0 //dhwang added
void cps_reboot_msg_deal(OM_AC_CPU_REBOOT_T* stOmReboot);
#endif
void cps_boot_reboot(CPS_REBOOT_T* stRbt);
UINT32 cpss_brd_usage_get();
#if 0 //dhwang added
void cps_om_phy_para(OM_AC_PHY_PM_STATISTIC_REQ_T* pucbuf);
#endif
extern UINT32 cpss_mem_usage_take();
extern UINT32 cps_devm_cpu_usage_now();

STRING g_szInitFailProcName;
UINT32 g_ulPowerOnTime;
extern CPS_DEVM_USAGE_T g_stCpsDevmCpuUsage;
extern CPS_DEVM_USAGE_T g_stCpsDevmDiskUsage;
extern CPS_DEVM_USAGE_T g_stCpsDevmMemUsage;
extern CPS_DEVM_USAGE_T g_stCpsDevmTempUsage;
extern UINT32 cpss_devm_tmp_take();
extern INT32 cps_devm_get_mem_size();
extern UINT32 g_ulMemTotalSize;
extern CPS_DEVM_SYSINFO_T* pstCpsDevmSysInfo;

/*******************************函数实现 ************************************/
VOID smss_sysctl_proc_table_init(VOID)
{
    UINT32 ulSize1 = 0;
    UINT32 ulSize2 = 0;
    UINT32 ulIndex;
    
    while (CPSS_VK_PD_INVALID != g_astSmssProcTable[ulSize1].ulProcDes)
    {
        ulSize1 = ulSize1 + 1;
    }

#ifndef CPSS_VOS_LINUX
    while (CPSS_VK_PD_INVALID != g_astSmssProcTableForUser[ulSize2].ulProcDes)
    {
        ulSize2 = ulSize2 + 1;
    }
#endif

    g_pstSmssSysctlProcTable = (SMSS_PROC_TABLE_ITEM_T *)cpss_mem_malloc(sizeof(SMSS_PROC_TABLE_ITEM_T)
        * (ulSize1 + ulSize2));
    if (NULL==g_pstSmssSysctlProcTable)
    {
    	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "<%s-%s>: g_pstSmssSysctlProcTable Malloc Failed!!",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
    }
    for (ulIndex = 0; ulIndex < ulSize1; ulIndex++)
    {
        g_pstSmssSysctlProcTable[ulIndex] = g_astSmssProcTable[ulIndex];
        g_pstSmssSysctlProcTable[ulIndex].ulWaitTime = swp_get_act_proc_time(g_astSmssProcTable[ulIndex].ulWaitTime);
    }

#ifndef CPSS_VOS_LINUX
    for (ulIndex = 0; ulIndex < ulSize2; ulIndex++)
    {
        g_pstSmssSysctlProcTable[ulSize1 + ulIndex] = g_astSmssProcTableForUser[ulIndex];
        g_pstSmssSysctlProcTable[ulSize1 + ulIndex].ulWaitTime = swp_get_act_proc_time(g_astSmssProcTableForUser[ulIndex].ulWaitTime);
    }
#endif
    g_ulSmssSysctlProcNum = ulSize1 + ulSize2;
    return;
}

/***************************************************************************
* 函数名称: smss_showproc
* 功    能: 显示单板激和纤程的个数及当前激和到的纤程。
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
INT32 smss_showproc(VOID)
{ 
	 UINT32 ulIndex;
	 /* 初始化监控修改 */

	 cps__oams_shcmd_printf("ProcName           IsSend?   time   ActResult\n");	
	 for (ulIndex = 0; ulIndex < g_ulSmssSysctlProcNum; ulIndex++)
	 {
	   cps__oams_shcmd_printf("%10s         %5s  %8d   %#X\n", cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[ulIndex].ulProcDes),
	   (ulIndex < g_ulSmssSendIndex) ? "TRUE" : "FALSE", g_pstSmssSysctlProcTable[ulIndex].ulWaitTime,pulProcActRsp[ulIndex]);		 	
	 }

	 /* 初始化监控修改 */
	 return CPS__OAMS_OK;
}

/***************************************************************************
* 函数名称: smss_sysctl_set_heart
* 功    能: 在主用全局板上打印一个单板的心跳, 在有DSP的主CPU上打印单个DSP心跳,测试用。
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 主用全局板看架框槽, 有DSP的主CPU上看CPU号
***************************************************************************/
VOID smss_sysctl_set_heart(UINT32 ulPhyAddr)
{
  g_stSmssBaseInfo.ulHeartBeatSndSeq = ulPhyAddr;
  return;	
}
VOID smss_sysctl_show_heart(VOID)
{
  cps__oams_shcmd_printf("phyAddr 0x%x.\n", g_stSmssBaseInfo.ulHeartBeatSndSeq);
  return; 	
}

/***************************************************************************
* 函数名称: smss_sysctl_send_local
* 功    能: 向本CPU的某纤程的发消息
* 函数类型  VOID
* 参    数:
* 参数名称               参数类型           输入/输出    参数描述
* ulPd                    UINT32               IN         纤程号
* ulMsgId                 UINT32               IN         消息号
* pucMsg                  UINT8 *              IN         消息体指针
* ulMl                    UINT32               IN         消息长度
* 函数返回:
*           无。
* 说    明: 该操作只能在本CPU上进行。
***************************************************************************/
VOID smss_sysctl_send_local(UINT32 ulPd,
                            UINT32 ulMsgId,
                            UINT8 *pucMsg,
                            UINT32 ulMl)
{
    CPSS_COM_MSG_HEAD_T stMsg;
    if (  ((NULL == pucMsg)&&(0 != ulMl))
        ||((NULL != pucMsg)&&(0 == ulMl))
        )
    {
        return;
    }
    
    cpss_mem_memset(&stMsg, 0, sizeof(CPSS_COM_MSG_HEAD_T));
    cpss_com_logic_addr_get(&stMsg.stDstProc.stLogicAddr, &stMsg.stDstProc.ulAddrFlag);            
    stMsg.stDstProc.ulPd   = ulPd;        
    stMsg.ulMsgId      = ulMsgId;
    stMsg.ucPriFlag  = CPSS_COM_PRIORITY_URGENT;
    stMsg.ucAckFlag  = CPSS_COM_ACK;
    stMsg.ucShareFlag = CPSS_COM_MEM_NOT_SHARE;
    stMsg.pucBuf       = pucMsg;
    stMsg.ulLen         = ulMl;    
    cpss_com_send_extend(&stMsg);
    smss_test_PrintMsgStream(ulMsgId,pucMsg,ulMl,TRUE);    
    return;
}

#if (SWP_CPUROLE_DSP != SWP_CPUROLE_TYPE)
/***************************************************************************
* 函数名称: smss_sysctl_msg_record
* 功    能: 记录smss 接收、发送的消息
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* ulMsgId            UINT32                  IN             消息ID
* ucFlag             UINT8                   IN      0: 接收到的消息 1：发送的消息
* ulProcId           UINT32                  IN      纤程id
* 函数返回:无
* 说    明:
***************************************************************************/
VOID smss_sysctl_msg_record(UINT32 ulMsgId, UINT8 ucFlag, UINT32 ulProcId)
{
	 UINT32 ulIndex;
	 UINT32 ulProcSelfId;
	 SMSS_RECORD_MESSAGE_INFO_T  *pRecordInfo;
	 	 
	 ulProcSelfId = ulProcId >> 16;
	 
	 if (0 == ucFlag)  /* 接收到的消息 */
	 {
	 	 pRecordInfo = 	g_smss_recv_msg_info; 	 
	 }
	 else if (1 == ucFlag) /* 发送的消息 */
	 {
	 	 pRecordInfo = 	g_smss_send_msg_info;
	 }
	 	
	 for (ulIndex = 0; ulIndex < SMSS_RECORD_MESSAGE_NUMBER; ulIndex++)
	 {
 	   if ((pRecordInfo + ulIndex)->ulMsgId == 0)
     {	   	  
        (pRecordInfo + ulIndex)->ulMsgId = ulMsgId;
        (pRecordInfo + ulIndex)->ulCount++;		      
        (pRecordInfo + ulIndex)->ulProcId = ulProcSelfId;
        return;
     }
   
     if (((pRecordInfo + ulIndex)->ulMsgId == ulMsgId) && ((pRecordInfo + ulIndex)->ulProcId == ulProcSelfId))
     {
   	   (pRecordInfo + ulIndex)->ulCount++;		   	 
   	   return;
     }
 	 }
	 
	 if (ulIndex == SMSS_RECORD_MESSAGE_NUMBER)
	 {
	 	 cps__oams_shcmd_printf("array %d overflow\n", ucFlag);
	 }
	 return;
}


/***************************************************************************
* 函数名称: smss_showmsginfo
* 功    能: show smss 接收、发送的消息
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* void
* 函数返回:无
* 说    明:
***************************************************************************/
VOID smss_showmsginfo(VOID)
{
   UINT32 ulIndex;
   
   cps__oams_shcmd_printf("recv message info:\n");
   for (ulIndex = 0; ulIndex < SMSS_RECORD_MESSAGE_NUMBER; ulIndex++)
   {
      if (0 != g_smss_recv_msg_info[ulIndex].ulMsgId)
      {
         cps__oams_shcmd_printf("msgId 0x%x, count %d, procId %d\n", g_smss_recv_msg_info[ulIndex].ulMsgId,
                            g_smss_recv_msg_info[ulIndex].ulCount, g_smss_recv_msg_info[ulIndex].ulProcId);	
      }
      else
      {
        break;	
      }       	
   }
   
   cps__oams_shcmd_printf("send message info:\n");
   for (ulIndex = 0; ulIndex < SMSS_RECORD_MESSAGE_NUMBER; ulIndex++)
   {
      if (0 != g_smss_send_msg_info[ulIndex].ulMsgId)
      {
         cps__oams_shcmd_printf("msgId 0x%x, count %d, procId %d\n", g_smss_send_msg_info[ulIndex].ulMsgId,
                            g_smss_send_msg_info[ulIndex].ulCount, g_smss_send_msg_info[ulIndex].ulProcId);	
      }
      else
      {
        break;
      } 	
   }
   return;	
}
#endif

#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
/***************************************************************************
* 函数名称: smss_get_slave_status
* 功    能: 接口函数, 获取从CPU的运行态
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 0: 从CPU正常, 1: 从CPU故障, 2: 从CPU不存在
***************************************************************************/
UINT32 smss_get_slave_status(VOID)
{
	#if defined(SMSS_BOARD_WITH_SLAVE)
	{
	  return smss_sysctl_get_slave_status();	
	}
	#else
	  return 2;
	#endif	
}
#endif

#if (SWP_CPUROLE_DSP != SWP_CPUROLE_TYPE)
/***************************************************************************
* 函数名称: smss_show_procname
* 功    能: 根据纤程号获取纤程名, SHOW函数用
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* ulProcId          UINT32                  IN              纤程号
* 函数返回:无
* 说    明: ulProcId 可以是具体的纤程号 21,4等,  也可以是通过CPSS_VK_PD宏计算的值.
***************************************************************************/
VOID smss_show_procname(UINT32 ulProcId)
{
	 if (0 == ulProcId)
	 {
	 	 cps__oams_shcmd_printf("param: %d error.\n", ulProcId);
	 	 return;
	 }
	 
	 if (ulProcId < 500)  /* 具体 */
	 {
     cps__oams_shcmd_printf("ProcId: %d, %d, ProcName: '%s'.\n", 
	       ulProcId, CPSS_VK_PD(ulProcId, 0), cpss_vk_proc_name_get_by_pid(CPSS_VK_PD(ulProcId, 0)));   	 	
	 }
	 else  /* 宏计算 */
	 {
     cps__oams_shcmd_printf("ProcId: %d, %d, ProcName: '%s'.\n", 
	       (ulProcId >> 16) & 0xffff, ulProcId, cpss_vk_proc_name_get_by_pid(ulProcId));   	 		 	
	 }
   return;	
}
#endif

/***************************************************************************
* 函数名称: MsgStreamRecordAdd
* 功    能: 添加指定的消息号到跟踪列表
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* ulRstReason          UINT32                  IN          单板复位原因编号
* 函数返回:无
* 说    明:
***************************************************************************/
VOID MsgStreamRecordAdd(UINT32 ulMsgId)
{
    UINT32 ulcount;

    for(ulcount = 0; ulcount < g_ulMsgStreamRecordSum; ulcount++)
    {
        if(g_aulMsgStreamRecordSum[ulcount] == ulMsgId)
        {
            return;
        }
    }
    if(g_ulMsgStreamRecordSum < MAX_MSG_STREAM_RECORD_SUM)
    {
        g_aulMsgStreamRecordSum[g_ulMsgStreamRecordSum] = ulMsgId;
        g_ulMsgStreamRecordSum = g_ulMsgStreamRecordSum + 1;
    }
}
/***************************************************************************
* 函数名称: MsgStreamRecordDel
* 功    能: 移出指定的消息号从跟踪列表
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
VOID MsgStreamRecordDel(UINT32 ulMsgId)
{
    UINT32 ulcount;

    for(ulcount = 0; ulcount < g_ulMsgStreamRecordSum; ulcount++)
    {
        if(g_aulMsgStreamRecordSum[ulcount] == ulMsgId)
        {
        	  g_ulMsgStreamRecordSum = g_ulMsgStreamRecordSum - 1;
            g_aulMsgStreamRecordSum[ulcount] = g_aulMsgStreamRecordSum[g_ulMsgStreamRecordSum];
            return;
        }
    }
}
/***************************************************************************
* 函数名称: MsgStreamRecordReset
* 功    能: 清除跟踪消息列表
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
VOID MsgStreamRecordReset(VOID)
{
    g_ulMsgStreamRecordSum = 0;
}
/***************************************************************************
* 函数名称: MsgStreamRecordShow
* 功    能: 跟踪消息列表显示
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
void MsgStreamRecordShow(VOID)
{
    UINT32 ulcount;

    cps__oams_shcmd_printf("The ulMsgId need to be recorded sum = %ld.\n", g_ulMsgStreamRecordSum);
    if (0 != g_ulMsgStreamRecordSum)
    {
        for (ulcount = 0; ulcount < g_ulMsgStreamRecordSum; ulcount++)
        {
            cps__oams_shcmd_printf("%#lx\n", g_aulMsgStreamRecordSum[ulcount]);
        }
    }
}
/***************************************************************************
* 函数名称: smss_test_PrintMsgStream
* 功    能: 跟踪消息字节流打印
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
void smss_test_PrintMsgStream(UINT32 ulMsgId,UINT8 *pMsgBuf,UINT32 ulMsgLen,UINT8 ucisSend )
{
    UINT32 ulcount;
    UINT16 usCounter;
    UINT16 usColum = 0;

    CPSS_TIME_T  stClock;

    #if (SWP_CPUROLE_DSP != SWP_CPUROLE_TYPE)
      smss_sysctl_msg_record(ulMsgId, ucisSend, cpss_vk_pd_self_get());
    #endif


    cpss_clock_get(&stClock);    

    for(ulcount = 0; ulcount < g_ulMsgStreamRecordSum; ulcount++)
    {
        if(g_aulMsgStreamRecordSum[ulcount] == ulMsgId)
        {
            break;
        }
    }
    if(ulcount >= g_ulMsgStreamRecordSum)
    {
        return;
    }

    if( FALSE == ucisSend)
    {
       cps__oams_shcmd_printf("\nReceived MsgId(%#x), %d-%d-%d %d:%d:%d\n", ulMsgId,
              stClock.usYear, stClock.ucMonth, stClock.ucDay,
              stClock.ucHour, stClock.ucMinute, stClock.ucSecond);
    }
    else
    {
       cps__oams_shcmd_printf("\nSending MsgId(%#x), %d-%d-%d %d:%d:%d\n", ulMsgId,
              stClock.usYear, stClock.ucMonth, stClock.ucDay,
              stClock.ucHour, stClock.ucMinute, stClock.ucSecond);
    }
    for(usCounter = 0;usCounter < ulMsgLen;usCounter++)
    {
        if(pMsgBuf[usCounter] < 0x10)
        {
            cps__oams_shcmd_printf("0");
        }
        cps__oams_shcmd_printf("%X ",pMsgBuf[usCounter]);
        usColum += 3;
        if(usColum >= CPSS_RM_TEST_PRINT_COLUM_SUM)
        {
            usColum = 0;
            cps__oams_shcmd_printf("\n");
        }
    }
    cps__oams_shcmd_printf("\n");
}

/***************************************************************************
* 函数名称: smss_sysctl_write_sbbr
* 功    能: 异常信息写黑匣子, vxWorks环境下,非DSP处理
* 函数类型  VOID
* 参    数:
* 参数名称        类型        输入/输出       描述
* 
* 函数返回:
*   
* 说    明:
***************************************************************************/
VOID smss_sysctl_write_sbbr(STRING  szFormat, ...)
{
  #if (SWP_OS_TYPE == SWP_OS_VXWORKS)
   #if (SWP_CPUROLE_DSP != SWP_CPUROLE_TYPE)
     va_list args;
     UINT32 ulArgs[4];
     INT32 lSize;
     CHAR azSbbrTextBuffer[5000];
                  
     va_start(args, szFormat);     
     lSize = vsprintf(azSbbrTextBuffer, szFormat, args) + 1;
     va_end(args);
   
     cpss_sbbr_record(0, 0, ulArgs, azSbbrTextBuffer, lSize);                            
   #endif
  #endif
  return;	
}

/***************************************************************************
* 函数名称: smss_send_act_req
* 功    能: 向纤程发送激和请求
* 函数类型  VOID
* 参    数:
* 参数名称        类型        输入/输出       描述
* szProcName      STRING          IN          纤程名
* 函数返回:
*   
* 说    明:
***************************************************************************/
UINT32 smss_send_act_req(STRING szProcName)
{
	 UINT32 ulProcDes;
	 SMSS_PROC_ACTIVATE_REQ_MSG_T stActProcMsg;
	 
   if (NULL == szProcName)
   {
     cps__oams_shcmd_printf("procName is NULL\n");
     return -1;	
   }
   
   /* 从纤程名获取纤程描述符 */
   ulProcDes = cpss_vk_proc_pid_get_by_name(szProcName);   
   stActProcMsg.ucAsStatus = g_stSmssAsInfo.ucSelfAStatus;
   
   smss_sysctl_send_local(ulProcDes,
                          SMSS_PROC_ACTIVATE_REQ_MSG, (UINT8 *)&stActProcMsg, sizeof(stActProcMsg)); 	
                          
   cps__oams_shcmd_printf("send SMSS_PROC_ACTIVATE_REQ_MSG to '%s'.\n",szProcName);
   return 0;                       
}

#ifdef SMSS_BOARD_WITH_DSP
STRING g_smss_dsp_file = "F:/smss/cpss_dsp.dat";

/***************************************************************************
* 函数名称: smss_showdspinfo
* 功    能: 将cpss记录的dsp信息显示出来
* 函数类型  VOID
* 参    数:
* 参数名称        类型        输入/输出       描述
* 函数返回:
*   
* 说    明: 文件路径 "F:/smss/cpss_dsp.dat"  
***************************************************************************/
INT32 smss_showdspinfo(VOID)
{
	 UINT32 ulFd;
	 UINT32 ulLen;
	 UINT32 ulIndex;
	 smss_dsp_sbbr_losthb_record_T  stRecord[SMSSDSPNUM];
	 
	 ulFd = cpss_file_open(g_smss_dsp_file, CPSS_FILE_ORDONLY | CPSS_FILE_OBINARY);	
	 if (CPSS_FD_INVALID != ulFd)
	 {
	 	  cpss_file_read(ulFd, stRecord, sizeof(smss_dsp_sbbr_losthb_record_T) * SMSSDSPNUM,
	 	              &ulLen);
	 	  cpss_file_close(ulFd);	
	 	  
	 	  for (ulIndex = 0; ulIndex < SMSSDSPNUM; ulIndex++)
	 	  {
	 	     cps__oams_shcmd_printf("\ndsp %d info:\n", ulIndex + DSP_CPUNO_BASE);
	 	     cps__oams_shcmd_printf("%d-%d-%d %d:%d:%d\n",
	 	         stRecord[ulIndex].stClock.usYear, stRecord[ulIndex].stClock.ucMonth, 
	 	         stRecord[ulIndex].stClock.ucDay, stRecord[ulIndex].stClock.ucHour, 
	 	         stRecord[ulIndex].stClock.ucMinute, stRecord[ulIndex].stClock.ucSecond);
	 	     cps__oams_shcmd_printf("ulMacCall %x, ulTimerScan %x, ulPciRecv %x, ulChipPoll %x, ulProcRun %x\n",
	 	              stRecord[ulIndex].ulMacCall, stRecord[ulIndex].ulTimerScan,
	 	              stRecord[ulIndex].ulPciRecv, stRecord[ulIndex].ulChipPoll,
	 	              stRecord[ulIndex].ulProcRun);  
	 	     
	 	     cps__oams_shcmd_printf("%d-%d-%d %d:%d:%d\n",
	 	         stRecord[ulIndex].stClock2.usYear, stRecord[ulIndex].stClock2.ucMonth, 
	 	         stRecord[ulIndex].stClock2.ucDay, stRecord[ulIndex].stClock2.ucHour, 
	 	         stRecord[ulIndex].stClock2.ucMinute, stRecord[ulIndex].stClock2.ucSecond);
	 	     cps__oams_shcmd_printf("ulMacCall %x, ulTimerScan %x, ulPciRecv %x, ulChipPoll %x, ulProcRun %x\n",
	 	              stRecord[ulIndex].ulMacCall2, stRecord[ulIndex].ulTimerScan2,
	 	              stRecord[ulIndex].ulPciRecv2, stRecord[ulIndex].ulChipPoll2,
	 	              stRecord[ulIndex].ulProcRun2);           	 	                  	
	 	  }	 	  
	 	 return CPS__OAMS_OK; 
	 }	 	  
	 cps__oams_shcmd_printf("file %s not exit\n", g_smss_dsp_file); 
	 return CPS__OAMS_OK;
}


/***************************************************************************
* 函数名称: smss_sysctl_dsphb_writefile
* 功    能: 将dsp信息写文件
* 函数类型  VOID
* 参    数:
* 参数名称        类型        输入/输出       描述
* 函数返回:
*   
* 说    明: 文件路径 "F:/smss/cpss_dsp.dat"  
***************************************************************************/
VOID smss_sysctl_dsphb_writefile(VOID)
{
	UINT32 ulFd;
	UINT32 ulLen;
	
	ulFd = cpss_file_open(g_smss_dsp_file,
            CPSS_FILE_OWRONLY | CPSS_FILE_OBINARY | CPSS_FILE_OCREATE | CPSS_FILE_OTRUNC);
  if (CPSS_FD_INVALID != ulFd)
  {
     cpss_file_write(ulFd, g_smss_dsp_losthb_record, sizeof(smss_dsp_sbbr_losthb_record_T) * SMSSDSPNUM, 
                 &ulLen);
     cpss_file_close(ulFd);	
  }
	return;
}


/***************************************************************************
* 函数名称: smss_sysctl_dsphb_record
* 功    能: 记录cpss发来的dsp信息
* 函数类型  VOID
* 参    数:
* 参数名称        类型        输入/输出       描述
* ucDspId        UINT8          IN          dspId
* ucCount        UINT8          IN          
* 函数返回:
*   
* 说    明:  dspId  0-11  ucCount 1 - 2 表示第一次或第二次记录
***************************************************************************/
VOID smss_sysctl_dsphb_record(UINT32 ucDspId, UINT8  ucCount)
{
	CPSS_DSP_SBBR_COUNTER_T  stCpssDsp;
	
	
	if (1 == ucCount)
	{
		/* 查询 */
	  cpss_dsp_sbbr_counter_read(ucDspId, &stCpssDsp);
	  /* 记录 */
	  cpss_clock_get(&g_smss_dsp_losthb_record[ucDspId].stClock);
	  g_smss_dsp_losthb_record[ucDspId].ulMacCall = stCpssDsp.ulMacCall;
	  g_smss_dsp_losthb_record[ucDspId].ulTimerScan = stCpssDsp.ulTimerScan;
	  g_smss_dsp_losthb_record[ucDspId].ulPciRecv = stCpssDsp.ulPciRecv;
	  g_smss_dsp_losthb_record[ucDspId].ulChipPoll = stCpssDsp.ulChipPoll;
	  g_smss_dsp_losthb_record[ucDspId].ulProcRun = stCpssDsp.ulProcRun;
	}
	else if (2 == ucCount)
	{
	  /* 查询 */
	  cpss_dsp_sbbr_counter_read(ucDspId, &stCpssDsp);
	  /* 记录 */
	  cpss_clock_get(&g_smss_dsp_losthb_record[ucDspId].stClock2);
	  g_smss_dsp_losthb_record[ucDspId].ulMacCall2 = stCpssDsp.ulMacCall;
	  g_smss_dsp_losthb_record[ucDspId].ulTimerScan2 = stCpssDsp.ulTimerScan;
	  g_smss_dsp_losthb_record[ucDspId].ulPciRecv2 = stCpssDsp.ulPciRecv;
	  g_smss_dsp_losthb_record[ucDspId].ulChipPoll2 = stCpssDsp.ulChipPoll;
	  g_smss_dsp_losthb_record[ucDspId].ulProcRun2 = stCpssDsp.ulProcRun;  	
	  
	  smss_sysctl_dsphb_writefile();
	}    
  return;
}
#endif

/***************************************************************************
* 函数名称: smss_sysctl_procdes2index
* 功    能: 根据纤程描述符获得纤程在纤程配置表内的下标
* 函数类型  VOID
* 参    数:
* 参数名称        类型        输入/输出       描述
* ulProcDes       UINT32          IN           纤程描述符
* 函数返回:
*           INT32: SMSS_OK 表示成功 SMSS_ERROR 表示失败
* 说    明:
***************************************************************************/
INT32 smss_sysctl_procdes2index(UINT32  ulProcDes)
{
    UINT32 ulIndex =0;

    for (ulIndex =0; ulIndex < g_ulSmssSysctlProcNum; ulIndex++)
    {
        if ( (g_pstSmssSysctlProcTable[ulIndex].ulProcDes == ulProcDes)
            &&(pulProcActResult[ulIndex] != SMSS_OK)
            /* 处理一个纤程被注册两次的情况 */
            )
        {
            break;
        }
    }
    if (ulIndex == g_ulSmssSysctlProcNum) /* 未查到，则返回失败 */
    {
        return SMSS_ERROR;
    }
    else
    {
        return ulIndex;
    }
}

/***************************************************************************
* 函数名称: smss_sysctl_broadcast
* 功    能: 向所有纤程发初始化完成消息
* 函数类型  VOID
* 参    数:
* 参数名称           参数类型            输入/输出            参数描述
* ulMsgId            UINT32                  IN               消息号
* pucMsg             UINT8 *                 IN               消息体
* ulMsgLen           UINT32                  IN               消息体长度
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_broadcast(UINT32 ulMsgId, UINT8 *pucMsg, UINT32 ulMsgLen)
{
    UINT32 ulIndex;

    for (ulIndex = 0; ulIndex < g_ulSmssSysctlProcNum; ulIndex++ )
    {
        smss_sysctl_send_local(g_pstSmssSysctlProcTable[ulIndex].ulProcDes,
                                             ulMsgId, pucMsg, ulMsgLen);
    }
    return;
}

#ifdef SMSS_BOARD_WITHOUT_RDBS
/***************************************************************************
* 函数名称: smss_sysctl_board_info_query_disp
* 功    能: 更新单板信息
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
*
* 函数返回: 
*           无。
* 说    明: 对查询单板信息消息进行处理
***************************************************************************/
VOID smss_sysctl_board_info_query_disp()
{
    UINT32 *pulTemp;    
    UINT32 i;    
    SMSS_BOARD_INFO_QUERY_RSP_MSG_T stBoardInfo;
    
    stBoardInfo.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
    stBoardInfo.stLogAddr = g_stSmssBaseInfo.stLogAddr;
    stBoardInfo.ulPhyBrdType = cpss_htonl(SWP_PHYBRD_TYPE);
    stBoardInfo.ucFunBrdType = SWP_FUNBRD_TYPE;
    stBoardInfo.ucAStatus = g_stSmssAsInfo.ucSelfAStatus;
    stBoardInfo.ucBrdRStatus = g_ucBoardState;
    stBoardInfo.ucReserved = 0;

#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE) /* 主CPU */
    /* 调用DRIVER提供的函数，获得硬件类型及版本信息 */
    drv_board_hwinfo_get((DRV_PHYBRD_INTEGRATED_INFO_VER_T *)&stBoardInfo.stPhyBoardVer);
#endif

    /* 转换为网络字节序 */
    pulTemp = (UINT32 *)(&stBoardInfo.stPhyBoardVer);

    for (i = 0; i < CPSS_DIV(sizeof(stBoardInfo.stPhyBoardVer), sizeof(UINT32)); i++)
    {
        *pulTemp = cpss_htonl(*pulTemp);
        pulTemp++;
    }
    
    smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_BOARD_INFO_QUERY_RSP_MSG,
        (UINT8 *)&stBoardInfo, sizeof(stBoardInfo));
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "<%s-%s>: Send SMSS_BOARD_INFO_QUERY_RSP_MSG to Gcpa!!!.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
    
#ifdef SMSS_BOARD_WITH_SLAVE
    {
        CPSS_COM_PHY_ADDR_T stSlavePhyAdd;
        stSlavePhyAdd = g_stSmssBaseInfo.stPhyAddr;
        stSlavePhyAdd.ucCpu = g_stSmssBaseInfo.stPhyAddr.ucCpu + 1;
        smss_sysctl_slave_rstate_send(stSlavePhyAdd);
    }
#endif
     smss_verm_set_gcpa_cps__rdbs_verstate(g_stSmssBaseInfo.stPhyAddr, ((UINT16)g_ulBoardVerState));    
    return;
}
#endif

#if (SWP_FUNBRD_ABOX != SWP_FUNBRD_TYPE)

/* 获取本网元标识 */
VOID smss_moid_get_ne_id(UINT16 *pusNeId)
{
    *pusNeId = 0;
#if ((SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE) || (SWP_CPUROLE_SLAVE == SWP_CPUROLE_TYPE))
    cps__oams_moid_get_ne_id(pusNeId);
#endif
}

/* 获取本地的MOID */
INT32 smss_moid_get_local_moid(UINT32 ulMoIdType, SWP_MO_ID_T *pstOutMoId)
{
    cpss_mem_memset(pstOutMoId,0,sizeof(SWP_MO_ID_T));
#if ((SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE) || (SWP_CPUROLE_SLAVE == SWP_CPUROLE_TYPE))
    return cps__oams_moid_get_local_moid(ulMoIdType,pstOutMoId);
#else
    return 0;
#endif
}

/* 获取指定物理地址的MOID */
INT32 smss_moid_get_moid_by_phy(UINT32 ulMoIdType, 
                                CPSS_COM_PHY_ADDR_T *pstPhyAddr,    
                                SWP_MO_ID_T *pstOutMoId)
{
    cpss_mem_memset(pstOutMoId,0,sizeof(SWP_MO_ID_T));
#if ((SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE) || (SWP_CPUROLE_SLAVE == SWP_CPUROLE_TYPE))
    return cps__oams_moid_get_moid_by_phy(ulMoIdType,pstPhyAddr,pstOutMoId);
#else
    return 0;
#endif
}

/*******************************************************************************
* 函数名称: smss_shcmd_reg
* 功    能: 向OAMS注册Shell测试命令函数。
* 函数类型: VOID
* 参    数: 无
* 函数返回: 无
* 说    明: 无
*******************************************************************************/
VOID smss_shcmd_reg(SMSS_SHCMD_INFO_T astShcmd[])
{
    UINT32 ulIndex = 0;
    while (NULL != astShcmd[ulIndex].szFuncName)
    {
        cps__oams_shcmd_reg(astShcmd[ulIndex].szFuncName, astShcmd[ulIndex].szFuncHelp,
                       astShcmd[ulIndex].szArgFmt, astShcmd[ulIndex].pfShcmdFunc);
        ulIndex = ulIndex + 1;
    }
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_dsp_record_init
* 功    能: dsp记录结构初始化.
* 函数类型  VOID
* 参    数:
* 参数名称      类型             输入/输出       描述
* void
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
#if (SWP_CPUROLE_DSP == SWP_CPUROLE_TYPE)
VOID smss_sysctl_dsp_record_init(VOID)
{
	cpss_mem_memset(g_pstSmssSbbrRecord, 0, sizeof(SMSS_SBBR_MSG_RECORD_T));
}	
#endif


#if  defined(SMSS_BOARD_WITH_DSP)
/***************************************************************************
* 函数名称: smss_sysctl_dsp_fun_show
* 功    能: 带有DSP的主CPU显示某个DSP黑匣子信息。
* 函数类型  
* 参    数:
* 参数名称      类型             输入/输出       描述
* void
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
INT32 smss_dsp_sbbr_show(UINT32 ulDspId)
{
    SMSS_SBBR_MSG_RECORD_T *pstSbbrData = NULL;
    
    pstSbbrData = cpss_mem_malloc(sizeof(SMSS_SBBR_MSG_RECORD_T));
    if(NULL == pstSbbrData)
    {
        cps__oams_shcmd_printf("smss dsp sbbr show malloc failed!\n");   
        return SMSS_ERROR;
    }
    drv_dsp_ext_mem_read(ulDspId,(UINT32)(CPSS_DSP_SBBR_SMSS_START_ADDR-0x80000000),sizeof(SMSS_SBBR_MSG_RECORD_T),(UINT8 *)pstSbbrData); 
    
    /* 函数的调用次数 */           
    cps__oams_shcmd_printf("smss dsp %d sbbr show begin.\n", ulDspId);   
    cps__oams_shcmd_printf("power_on: InCount: 0x%x, OutCount: 0x%x\n",pstSbbrData->ulFunc_1InCnt, pstSbbrData->ulFunc_1OutCnt);
    cps__oams_shcmd_printf("idle_recv_actprocrsp: InCount: 0x%x, OutCount: 0x%x\n",pstSbbrData->ulFunc_2InCnt,  pstSbbrData->ulFunc_2OutCnt);
    cps__oams_shcmd_printf("recv_regrsp: InCount: 0x%x, OutCount: 0x%x\n",pstSbbrData->ulFunc_3InCnt,  pstSbbrData->ulFunc_3OutCnt);
    cps__oams_shcmd_printf("send_act_stoa_req: InCount: 0x%x, OutCount: 0x%x\n",pstSbbrData->ulFunc_4InCnt,  pstSbbrData->ulFunc_4OutCnt);
    cps__oams_shcmd_printf("recv_act_stoa_rsp: InCount: 0x%x, OutCount: 0x%x\n",pstSbbrData->ulFunc_5InCnt,  pstSbbrData->ulFunc_5OutCnt);
    cps__oams_shcmd_printf("send_statuschgind: InCount: 0x%x, OutCount: 0x%x\n",pstSbbrData->ulFunc_6InCnt,  pstSbbrData->ulFunc_6OutCnt);
    cps__oams_shcmd_printf("recv_statuschgack: InCount: 0x%x, OutCount: 0x%x\n",pstSbbrData->ulFunc_7InCnt,  pstSbbrData->ulFunc_7OutCnt);
    cps__oams_shcmd_printf("slavecpu_send_periodly_heartbeat: InCount: 0x%x, OutCount: 0x%x\n",pstSbbrData->ulFunc_8InCnt,  pstSbbrData->ulFunc_8OutCnt);
    cpss_mem_free(pstSbbrData);
    return CPS__OAMS_OK;
}
#endif
/***************************************************************************
* 函数名称: smss_sysctl_dsp_record
* 功    能: 将smss的信息记入DSP的黑匣子中。
* 函数类型  VOID
* 参    数:
* 参数名称      类型             输入/输出       描述
* void
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
#if (SWP_CPUROLE_DSP == SWP_CPUROLE_TYPE)
VOID smss_sysctl_dsp_record(UINT32 ulMsgId,UINT8 *pMsgBuf,UINT32 ulMsgLen)
{
	 UINT32 ulSec;
   UINT32 ulMs;
   UINT8 *pucBuf =(UINT8*)g_pstSmssSbbrRecord;
		 
	 cpss_timer_get_sec_and_ms(&ulSec,&ulMs);
	 
	 *(UINT32*)(pucBuf + g_ulSmssCounter) = cpss_htonl(g_ulSmssSubSystemMsgNum);
   g_ulSmssCounter = g_ulSmssCounter + 4;
   
   *(UINT32*)(pucBuf + g_ulSmssCounter) = cpss_htonl(ulMsgId);
   g_ulSmssCounter = g_ulSmssCounter + 4;
   
   *(UINT32*)(pucBuf + g_ulSmssCounter) = cpss_htonl(ulSec);
   g_ulSmssCounter = g_ulSmssCounter + 4;
        
   *(UINT32*)(pucBuf + g_ulSmssCounter) = cpss_htonl(ulMs);
   g_ulSmssCounter = g_ulSmssCounter + 4;     

   *(UINT32*)(pucBuf + g_ulSmssCounter) = cpss_htonl(ulMsgLen);
   g_ulSmssCounter = g_ulSmssCounter + 4;     
	
   if(ulMsgLen > 128)
     {
      ulMsgLen = 128;
     } 
	 
   cpss_mem_memcpy((pucBuf + g_ulSmssCounter), pMsgBuf, ulMsgLen);   
   g_ulSmssCounter = g_ulSmssCounter + ulMsgLen;    
   g_ulSmssSubSystemMsgNum = g_ulSmssSubSystemMsgNum + 1; 
   
   if (0 == CPSS_MOD(g_ulSmssSubSystemMsgNum, 10))
   {
     g_ulSmssCounter = 0;
   }     
    
   return;    
}
#endif

/***************************************************************************
* 函数名称: smss_sysctl_proc
* 功    能: SMSS主控纤程的入口函数，根据该纤程的状态进入不同状态的处理模块。
* 函数类型  VOID
* 参    数:
* 参数名称      类型             输入/输出       描述
* usState      UINT16               IN         纤程状态
* pVar         VOID *               IN         纤程私有数据区指针
* pstMsg CPSS_COM_MSGHEAD_T *       IN         消息头结构
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_proc(UINT16 usUserState,
                      VOID *pvVar,
                      CPSS_COM_MSG_HEAD_T *pstMsg
                      )
{
    BOOL bDealMessage;
    if (NULL == pstMsg)                     /* 消息合法性校验 */
    {
        return;
    }
    
    smss_test_PrintMsgStream(pstMsg->ulMsgId,pstMsg->pucBuf,
         pstMsg->ulLen,FALSE);
    
    bDealMessage =  smss_sysctl_pub_func(pstMsg);
    if (TRUE == bDealMessage)
     /* 如果处理了和公共的消息，直接退出 */
    {
        return;
    }
#if 0 //dhwang added
    if(OM_AC_CPU_REBOOT_IND_MSG == pstMsg->ulMsgId)
    {
    	cps_reboot_msg_deal((OM_AC_CPU_REBOOT_T*)pstMsg->pucBuf);
    	return;
    }
#endif
    if(CPS_BOOT_CPU_REBOOT_IND_MSG == pstMsg->ulMsgId)
    {
    	cps_boot_reboot(pstMsg->pucBuf);
    	return;
    }
    /* yanghuanjun 20061206 begin */
		#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
		 smss_sysctl_dsp_record(pstMsg->ulMsgId,pstMsg->pucBuf,pstMsg->ulLen);
		#endif
		/* yanghuanjun 20061206 end */
		
    switch(usUserState)
    {
    case SMSS_SYSCTL_STATE_IDLE:    /* Idle态处理函数 */
        smss_sysctl_state_idle_func(pstMsg);
        break;

    case SMSS_SYSCTL_STATE_INIT:    /* INIT态处理函数 */
        smss_sysctl_state_init_func(pstMsg);
        break;

    case SMSS_SYSCTL_STATE_NORMAL:  /* Normal态处理函数 */
        smss_sysctl_state_normal_func(pstMsg);
        break;

 #if defined(SMSS_BACKUP_MATE)
    case SMSS_SYSCTL_STATE_STOA:    /* stoa态处理函数 */
        smss_sysctl_state_stoa_func(pstMsg);
        break;
 #endif

    case SMSS_SYSCTL_STATE_END:    /* END态处理函数 */
        smss_sysctl_state_end_func(pstMsg);
        break;

    default:                       /* 进入无效状态，告警 */
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "<%s-%s>: Enter unknown state.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        break;
    }
}


/***************************************************************************
* 函数名称: smss_sysctl_state_idle_func
* 功    能: 处理纤程初始化之前的消息，包括：CPSS的上电消息、注册应答消息等。
* 函数类型  VOID
* 参    数:
* 参数名称     类型            输入/输出       描述
* pstMsg CPSS_COM_MSGHEAD_T *     IN         消息头结构
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_state_idle_func( CPSS_COM_MSG_HEAD_T *pstMsg )
{
#ifdef SMSS_BACKUP_MATE
    BOOL bDealMessage;
    bDealMessage =  smss_sysctl_idle_as_func(pstMsg);
    if (TRUE == bDealMessage)
        /* 如果处理了和软件主备竞争相关的消息，直接退出 */
    {
        return;
    }
#endif

    switch(pstMsg->ulMsgId)
    {
    case CPSS_COM_POWER_ON_MSG:      /* CPSS的上电消息 */
        smss_sysctl_proc_table_init();
        /* yanghuanjun 20061206 begin */
        #if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
          smss_sysctl_dsp_record_init();
        #endif
        /* yanghuanjun 20061206 end */
        smss_sysctl_power_on((CPSS_COM_POWER_ON_MSG_T *)pstMsg->pucBuf);
        break;

    case SMSS_SYSCTL_REG_RSP_MSG:    /* 注册应答消息 */
        smss_sysctl_recv_regrsp((SMSS_SYSCTL_REG_RSP_MSG_T *)pstMsg->pucBuf);
        break;

    case SMSS_TIMER_IDLE_MSG:        /* 超时消息 */
        smss_sysctl_state_idle_timeout();
        break;

    case SMSS_PROC_ACTIVATE_RSP_MSG: /* 纤程激活应答消息 */
        smss_sysctl_idle_recv_actprocrsp((SMSS_PROC_ACTIVATE_RSP_MSG_T *)pstMsg->pucBuf);
        break;

    case SMSS_SYSCTL_BFAULT_IND_MSG:  /* 该状态下只能收到来自伙伴板的故障指示消息 */
        if (SMSS_STANDBY == g_stSmssAsInfo.ucSelfAStatus)
            /* 本板为备用板时直接复位, 为主用板时不做任何处理 */
        {
           smss_sysctl_enter_end(TRUE,
               SMSS_BRD_RST_CAUSE_MATE_FAULT_SLAVE_BRD_IDLE);
        }
        break;

    default:                         /* 收到非法消息，进行告警 */
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: Received unknown message 0x%08X.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, pstMsg->ulMsgId);
        break;
    }
}

/***************************************************************************
* 函数名称: smss_sysctl_power_on
* 功    能: 收到CPSS的激活消息后，根据本单板的不同类型进行不同处理。
* 函数类型  VOID
* 参    数:
* 参数名称        类型               输入/输出       描述
* pstMsg   CPSS_COM_POWER_ON_MSG_T *    IN        本板的备份方式
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_power_on(CPSS_COM_POWER_ON_MSG_T *pstMsg)
{  
    SMSS_PROC_ACTIVATE_REQ_MSG_T stActProc;
    UINT32 ulStatus = 0;
    g_ulPowerOnFlag = 1;    

    #if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
     CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_1InCnt);
    #endif

    g_ucSmssIdleStage = SMSS_IDLE_INIT;
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Received message %s.",
               "UNKNOWN", "IDLE", "CPSS_COM_POWER_ON_MSG");
    
    if (NULL == pstMsg) /* 判断消息的有效性 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "smss_sysctl_power_on: Received NULL Message.");
        return;
    }

    /* 向OAMS注册SYSCTL的Shell测试命令函数 */
    smss_shcmd_reg(astSmssSysctlShcmd);
    
#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)    /* 若为主CPU，设置面板灯 */ 
    drv_board_led_set(DRV_LED_TYPE_ATTN, DRV_LED_STATU_OFF);
    drv_board_led_set(DRV_LED_TYPE_ACTIVE, DRV_LED_STATU_OFF);
    /* 初始化置灭OOS灯 */
    smss_sysctl_oosled_set(DRV_LED_STATU_OFF);

#endif
        
    g_stSmssAsInfo.szSelfAStatus = pstMsg->bMateExist ? "UNKNOWN" : "ALONE";
    g_stSmssAsInfo.ucSelfAStatus = pstMsg->bMateExist ? SMSS_UNKNOWN_AS_STATUS : SMSS_ACTIVE;
    g_stSmssAsInfo.stSwitchInfo.bSwitching = FALSE;

    smss_sysctl_init();   /* SMSS自身初始化 */  
    
#ifdef SMSS_BACKUP_MATE  
    /* 设置相应的主备状态 */
    g_stSmssAsInfo.bMateExist = pstMsg->bMateExist;
    g_stSmssAsInfo.stMatePhyAddr = pstMsg->stMatePhyAddr;
    if (TRUE == g_stSmssAsInfo.bMateExist)  /* 配置有伙伴板时进行主备相关处理 */
    {
#if defined(SMSS_AS_CMP_SOFT_)  /* 软件实现主备竞争 */
        {
            SMSS_ACTIVE_STANDBY_CMP_REQ_MSG_T stCmpReq;
            
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                       "<%s-%s>: Software AS competing.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
            
            g_stSmssAsInfo.stMatePhyAddr = pstMsg->stMatePhyAddr;
            g_ucSmssIdleStage = SMSS_IDLE_AS_CMP;
            stCmpReq.ucCmpData = g_stSmssBaseInfo.stPhyAddr.ucSlot;
            smss_com_send_mate(SMSS_SYSCTL_PROC, SMSS_ACTIVE_STANDBY_CMP_REQ_MSG,
                               (UINT8 *)&stCmpReq, sizeof(stCmpReq));
            ucCmpCount = 1;
            cpss_timer_loop_set(SMSS_TIMER_IDLE, SMSS_TIMER_IDLE_DELAY);
        }
        return;
#elif defined(SMSS_AS_CMP_HARD)   /* 硬件主备竞争 */
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "<%s-%s>: Hardware AS competing.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);

#if (SWP_PHYBRD_TYPE != SWP_PHYBRD_MASA)      /* 如果不是交换板 */        
        drv_active_standby_enable(TRUE); /* 打开主备竞争 */

        /* 设置硬件主备竞争定时器*/
        cpss_timer_set(SMSS_TIMER_HARD_CMP_TIMER, SMSS_TIMER_HARD_CMP_DELAY);
        g_stSmssAsInfo.stMatePhyAddr = pstMsg->stMatePhyAddr;
        return;
#endif
        drv_active_standby_get(&g_stSmssAsInfo.ucSelfAStatus); /* 获取本板的主备状态 */
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                   "(%d-%d-%d %d) get state %d.",
                   g_stSmssBaseInfo.stPhyAddr.ucFrame, g_stSmssBaseInfo.stPhyAddr.ucShelf,
			             g_stSmssBaseInfo.stPhyAddr.ucSlot, g_stSmssBaseInfo.stPhyAddr.ucCpu,
			             g_stSmssAsInfo.ucSelfAStatus);

        if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
            /* 若为主用板，设置面板灯 */
        {
            g_stSmssAsInfo.szSelfAStatus = "ACTIVE";
#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)    /* 若为主CPU，设置面板灯 */ 
            drv_board_led_set(DRV_LED_TYPE_ACTIVE, DRV_LED_STATU_ON);
            smss_devm_SetActled(1);           
#endif
        }
        else
        {
            g_stSmssAsInfo.szSelfAStatus = "STANDBY";            
        }
#endif       
    }  
#endif

    /* 获取本板的逻辑地址 */
    cpss_com_logic_addr_get(&g_stSmssBaseInfo.stLogAddr, &ulStatus);
    
    /* 激活CPSS的通讯纤程，启动定时器等待其应答 */
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Send %s to Proc `%s'.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               "SMSS_PROC_ACTIVATE_REQ_MSG",
               cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulProcDes));

    stActProc.ucAsStatus = g_stSmssAsInfo.ucSelfAStatus;
    smss_sysctl_send_local(g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulProcDes,
                           SMSS_PROC_ACTIVATE_REQ_MSG, 
                           (UINT8*)&stActProc, sizeof(stActProc));   
    
    cpss_timer_set(SMSS_TIMER_IDLE, g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulWaitTime);
    g_ucSmssIdleStage = SMSS_IDLE_ACT_CPSS;
    g_ulSmssSendIndex = g_ulSmssSendIndex + 1;
    
    /* yanghuanjun 20061206 begin */
    #if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
     CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_1OutCnt);
    #endif
   /* yanghuanjun 20061206 end */
    
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_send_regreq
* 功    能: 外围板向主用全局板注册或者DSP/从CPU向主CPU注册
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_send_regreq(VOID)
{
    SMSS_SYSCTL_REG_REQ_MSG_T stRegMsg;

    /* 重发超过10次，复位本板  */
    g_ucSmssSendCount = g_ucSmssSendCount + 1;

    CPSS_DSP_FILE_REC();
    CPSS_DSP_LINE_REC();
    CPSS_DSP_RUN_ADDR_REC(CPSS_DSP_SMSS_REG_REQ_END);
    /* 备用全局板注册有超时, 其他单板一直注册 */
    #ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL   
    #if 1  /* 注册请求一直重发 */
    if (g_ucSmssSendCount > 120)
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "<%s-%s>: Send Gcpa Reg Req Over Ten Times, Reset Self.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);

        smss_sysctl_enter_end(TRUE,SMSS_BRD_RST_CAUSE_REG_TIMEOUT);
        return;
    }
    #endif
    #endif

#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE) /* 主CPU，向全局板注册 */
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Send %s to GCPA.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               "SMSS_SYSCTL_REG_REQ_MSG");

    stRegMsg.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
    stRegMsg.ucAStatus = g_stSmssAsInfo.ucSelfAStatus;
    stRegMsg.ulPhyBrdType = cpss_htonl(SWP_PHYBRD_TYPE);

    smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_REG_REQ_MSG,
        (UINT8 *)&stRegMsg, sizeof(stRegMsg));
#else /* 从CPU/DSP，向主CPU注册 */
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Send %s to HOST.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               "SMSS_SYSCTL_REG_REQ_MSG");

    stRegMsg.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
    smss_sysctl_send_host(SMSS_SYSCTL_PROC, SMSS_SYSCTL_REG_REQ_MSG,
        (UINT8 *)&stRegMsg, sizeof(stRegMsg));
    CPSS_DSP_LINE_REC();
#endif
    return;
}

/*******************************************************************************
* 函数名称: smss_sysctl_recv_regrsp
* 功    能: 本处理器收到注册应答时进行相应处理
* 函数类型  VOID
* 参    数:
* 参数名称        类型              输入/输出       描述
* pstMsg    SMSS_SYSCTL_REG_RSP_T *    IN          注册结果
* 函数返回:
*           无。
* 说    明:
*******************************************************************************/

VOID smss_sysctl_recv_regrsp(SMSS_SYSCTL_REG_RSP_MSG_T *pstMsg)
{

    CPSS_DSP_FILE_REC();
    CPSS_DSP_LINE_REC();
    CPSS_DSP_RUN_ADDR_REC(CPSS_DSP_SMSS_RES_REQ_END);
    if (NULL == pstMsg)  /* 判断消息的有效性 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: smss_sysctl_recv_regrsp() error: Received NULL message.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        CPSS_DSP_LINE_REC();
        CPSS_DSP_ERROR_WAIT;
        return;
    }

    /*  yanghuanjun 20061206 begin */
    #if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
     CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_3InCnt);
    #endif
    /*  yanghuanjun 20061206 end */

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Received %s from GCPA.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               "SMSS_SYSCTL_REG_RSP_MSG");

    if (SMSS_OK == pstMsg->ulResult)
        /* 若注册成功，则进入INIT态，开始激活纤程 */
    {
        cpss_timer_delete(SMSS_TIMER_IDLE);
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "<%s-%s>: Transit to state <%s>.",
                   g_stSmssAsInfo.szSelfAStatus, "IDLE", "INIT");
        g_stSmssBaseInfo.ucRStatus = SMSS_RSTATUS_INIT;
        g_stSmssBaseInfo.szRStatus = "INIT";
        cpss_vk_proc_user_state_set(SMSS_SYSCTL_STATE_INIT);

        /* 清空重发计数器 */
        g_ucSmssSendCount = 0;

        /* 激活各纤程 */
        ucActProcStage = SMSS_SEND_PROC;
        smss_sysctl_send_act_stoa_req();
        
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
        CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_3OutCnt);
#endif
    }
    CPSS_DSP_LINE_REC();
    /* 若注册失败，则返回；超时后会重发注册消息。 */
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_send_act_stoa_req()
* 功    能: 根据纤程激活表和当前状态依次向各纤程发消息。
* 函数类型  VOID
* 参    数:
* 参数名称  参数类型        输入/输出           参数描述
* ulMsgDes  UINT32             IN               要发送消息的描述符
* 函数返回:
*           无。
* 说    明: 该函数在单板初始化和备升主过程中被调用，不同的地方发送的消息不同
***************************************************************************/
VOID smss_sysctl_send_act_stoa_req(VOID)
{
    UINT8  *pstReq = NULL;
    UINT8  ucMsgLen = 0;
    UINT32 ulMsgId = 0;
    UINT32 ulTid = 0;
    SMSS_PROC_ACTIVATE_REQ_MSG_T stActProcMsg;
    stActProcMsg.ucAsStatus = g_stSmssAsInfo.ucSelfAStatus;

		/*  yanghuanjun 20061206 begin */
		#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
		 CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_4InCnt);
		#endif
		/*  yanghuanjun 20061206 end */


    if (SMSS_RSTATUS_INIT == g_stSmssBaseInfo.ucRStatus)
        /* 在INIT态下发激活消息 */
    {
        ulMsgId = SMSS_PROC_ACTIVATE_REQ_MSG;
        pstReq = (UINT8 *)&stActProcMsg;
        ucMsgLen = sizeof(stActProcMsg);
    }
    else if (SMSS_RSTATUS_STOA == g_stSmssBaseInfo.ucRStatus)
        /* 在STOA态下发激活消息 */
    {
        ulMsgId = SMSS_STANDBY_TO_ACTIVE_REQ_MSG;
    }

    /* 依次向各纤程发消息 */
    while (g_ulSmssSendIndex < g_ulSmssSysctlProcNum)
    {
        smss_sysctl_send_local(g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulProcDes,
                               ulMsgId, pstReq, ucMsgLen);

        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "<%s-%s>: Send %s to Proc `%s'.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   SMSS_RSTATUS_INIT == g_stSmssBaseInfo.ucRStatus ?
                   "SMSS_PROC_ACTIVATE_REQ_MSG" : "SMSS_STANDBY_TO_ACTIVE_REQ_MSG",
                   cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulProcDes));

        g_ulSmssSendIndex = g_ulSmssSendIndex + 1;

        if (0 != g_pstSmssSysctlProcTable[g_ulSmssSendIndex-1].ulWaitTime)
            /* 同步激活时设置定时器进行等待 */
        {
            ulTid = cpss_timer_para_set(SMSS_TIMER_INIT,
                g_pstSmssSysctlProcTable[g_ulSmssSendIndex-1].ulWaitTime, g_ulSmssSendIndex);
            pulProcActResult[g_ulSmssSendIndex-1] = ulTid;
            break;
        }  /*end of if */
        else
        {
            g_ulSmssRspedProcNum += 1 ;
            pulProcActResult[g_ulSmssSendIndex-1] = CPSS_TD_INVALID;
        }
    } /*end of for */

    if (g_ulSmssSendIndex == g_ulSmssSysctlProcNum)
        /* 向所有纤程发送完消息后进行处理 */
    {
        ucActProcStage = SMSS_SEND_ALL;
        if (g_ulSmssRspedProcNum != g_ulSmssSysctlProcNum)
            /* 没有收到所有纤程的应答消息则起统一定时器等待应答 */
        {
            if (0 != g_pstSmssSysctlProcTable[g_ulSmssSysctlProcNum-1].ulWaitTime)
                /* 若最后一个激活的纤程是同步纤程，则删除为其创建的定时器 */
            {
                cpss_timer_para_delete(pulProcActResult[g_ulSmssSendIndex-1]);
                pulProcActResult[g_ulSmssSendIndex-1] = CPSS_TD_INVALID;
            }
              /* 创建等待所有纤程应答消息的定时器 */
            cpss_timer_set(SMSS_TIMER_INIT, 100000);
        }
        else /* 收到所有的应答消息时初始化或者备升主结束 */
        {
            if (SMSS_RSTATUS_INIT == g_stSmssBaseInfo.ucRStatus)
                /* 若为初始化态，则初始化结束 */
            {
                smss_sysctl_init_complete();
            }
#ifdef  SMSS_BACKUP_MATE
            else if (SMSS_RSTATUS_STOA == g_stSmssBaseInfo.ucRStatus)
                /* 若为备升主，则备升主结束 */
            {
                smss_sysctl_stoa_complete();
                ucActProcStage = SMSS_SEND_PROC;
            }
#endif
        }
    }
    /*  yanghuanjun 20061206 begin */
		#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
		 CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_4OutCnt);
		#endif
		/*  yanghuanjun 20061206 end */
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_state_idle_timeout
* 功    能: 处理Idle态下的超时消息。
* 函数类型  VOID
* 参    数: 无。
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_state_idle_timeout(VOID)
{
    switch(g_ucSmssIdleStage)
    {
#ifdef SMSS_AS_CMP_SOFT_
    case SMSS_IDLE_AS_CMP:
        if (ucCmpCount > 5)  /* 竞争了5次，默认为自己为主 */
        {
            SMSS_PROC_ACTIVATE_REQ_MSG_T stActProc;
            UINT32 ulStatus;
            g_stSmssAsInfo.ucSelfAStatus = SMSS_ACTIVE;
            g_stSmssAsInfo.szSelfAStatus = "ACTIVE";
            cpss_timer_delete(SMSS_TIMER_IDLE);
#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)    /* 若为主CPU，设置面板灯 */ 
            drv_board_led_set(DRV_LED_TYPE_ACTIVE, DRV_LED_STATU_ON);
#endif

            /* 获取本板的逻辑地址 */
            ulStatus = SMSS_ACTIVE;
            cpss_com_logic_addr_get(&g_stSmssBaseInfo.stLogAddr, &ulStatus);

            g_ucSmssIdleStage = SMSS_IDLE_ACT_CPSS;
            stActProc.ucAsStatus = g_stSmssAsInfo.ucSelfAStatus;
            smss_sysctl_send_local(g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulProcDes,
                                   SMSS_PROC_ACTIVATE_REQ_MSG,
                                   (UINT8*)&stActProc, sizeof(stActProc));
            cpss_timer_set(SMSS_TIMER_IDLE,
                           g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulWaitTime);
            g_ulSmssSendIndex = g_ulSmssSendIndex + 1;
        }
        else  /* 否则，向伙伴板发竞争消息 */
        {
            SMSS_ACTIVE_STANDBY_CMP_REQ_MSG_T stCmpReq;
            stCmpReq.ucCmpData = g_stSmssBaseInfo.stPhyAddr.ucSlot;
            smss_com_send_mate(SMSS_SYSCTL_PROC, SMSS_ACTIVE_STANDBY_CMP_REQ_MSG,
                               (UINT8 *)&stCmpReq, sizeof(stCmpReq));
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
                       "<%s-%s>: Send AS_CMP_REQ to MATE, %d.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                       ucCmpCount);
            ucCmpCount = ucCmpCount + 1;
        }
        break;
#endif

    case SMSS_IDLE_ACT_CPSS:     /* 激活CPSS超时，则复位 */
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                   "<%s-%s>: Activating Proc `pCpssLink' timeout.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus );

        smss_sysctl_write_sbbr("<%s-%s>: Activating Proc `pCpssLink' timeout.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);

        if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)
            /* 主CPU激活失败时分主备进行处理 */
        {
            if (TRUE == g_stSmssAsInfo.bMateExist
                && (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus))
            {
                smss_sysctl_enter_end(FALSE,
                    SMSS_BRD_RST_CAUSE_ACT_PROC_CPSS_TIMEOUT_IDLE);  /* 若为主用板，延时后复位 */
            }
            else
            {
                smss_sysctl_enter_end(TRUE,
                    SMSS_BRD_RST_CAUSE_ACT_PROC_CPSS_TIMEOUT_IDLE);  /* 直接复位 */
            }
        }
        else  /* 从CPU激活失败时向主CPU报告,然后进入END态 */
        {
            /* 向主CPU报告 */
            SMSS_SYSCTL_BFAULT_IND_MSG_T stBFault;
            stBFault.ulType = cpss_htonl(SMSS_BFAULT_SLAVE_TO_HOST);
            stBFault.ulReason = cpss_htonl(SMSS_BFAULT_INIT_FAILED);
            stBFault.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
            smss_sysctl_send_host(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BFAULT_IND_MSG,
                                  (UINT8 *)&stBFault, sizeof(stBFault));

            /* 进入END态 */
            smss_sysctl_enter_end(TRUE,
                SMSS_BRD_RST_CAUSE_ACT_PROC_CPSS_TIMEOUT_IDLE);
        }
        break;

    case SMSS_IDLE_REG:          /* 发送注册请求超时，则重发 */
        smss_sysctl_send_regreq();
        break;

    default:
        break;
    }
}

#ifndef CPSS_VOS_LINUX
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL
/***************************************************************************
* 函数名称: smss_update_cps__rdbs_file
* 功    能: 在主用全局板的SHELL上输入命令，下载RDBS压缩文件
* 参    数:
* 参数名称        类型                 输入/输出       描述
* szSvrIpAddr     STRING                 IN           服务器IP地址。
* szSvrIpAddr     szFileName             IN           需要下载的文件名。   
* 函数返回:
*          无。
* 说   明: 
***************************************************************************/
VOID smss_update_cps__rdbs_file(STRING szSvrIpAddr, STRING szFileName)
{
     CPSS_FTP_REQ_MSG_T stFtpReq;
     INT32 lRet;
     UINT8 aucFullName[CPSS_FS_FILE_ABSPATH_LEN];
     
     stFtpReq.ulSerialID = 0;
     strcpy(stFtpReq.aucUserName, "anonymous"); 
     strcpy(stFtpReq.aucPasswd, "anonymous");

     stFtpReq.ulServerIP = cpss_ntohl(inet_addr(szSvrIpAddr));
     strcpy(stFtpReq.aucServerFile, szFileName);
     
     strcpy(aucFullName, "H:/rdbs/work/");
     strcat(aucFullName, szFileName);    
     strcpy(stFtpReq.aucClientFile, aucFullName);   /* 客户端文件名, 绝对路径 */


    stFtpReq.ulFtpCmd = CPSS_FTP_CMD_GET_FILE; /* 传输命令：下载*/
    stFtpReq.ulProgessStep = 0;
    stFtpReq.ulLen = 0;
    stFtpReq.pucMem = NULL;
    
    cps__oams_shcmd_printf("download %s begin\n",aucFullName);  
    
    /*调用ftp处理*/
    lRet = cpss_fs_ftp(&stFtpReq, 0);
    if (CPSS_OK == lRet)
    {
      cps__oams_shcmd_printf("file %s download success\n",aucFullName);      
      cpss_file_unpack(aucFullName, "H:/rdbs/work/");      
      cpss_file_delete(aucFullName);     
      cps__oams_shcmd_printf("rdbs file download success, please reset board\n");    
    }
    else
    {
      cps__oams_shcmd_printf("rdbs file download fail\n");
    }      
    return;    
}

/***************************************************************************
* 函数名称: smss_download_license_file
* 功    能: 在主用全局板下载LICENSE文件
* 参    数:
* 参数名称        类型                 输入/输出       描述
* szSvrIpAddr     STRING                 IN           服务器IP地址。
* szSvrIpAddr     szFileName             IN           需要下载的文件名。
* 函数返回:
*          无。
* 说   明: 
***************************************************************************/
VOID smss_download_license_file(STRING szSvrIpAddr, STRING szFileName)
{
	CPSS_FTP_REQ_MSG_T stFtpReq;
	INT32 lRet;
	UINT8 aucFullName[CPSS_FS_FILE_ABSPATH_LEN];
	
	stFtpReq.ulSerialID = 0;
	strcpy(stFtpReq.aucUserName, "anonymous"); 
	strcpy(stFtpReq.aucPasswd, "anonymous");
	
	stFtpReq.ulServerIP = cpss_ntohl(inet_addr(szSvrIpAddr));
	strcpy(stFtpReq.aucServerFile, szFileName);
	
	strcpy(aucFullName, "H:/license/");/* CPS__RDBS_R工作目录 */
	strcat(aucFullName, szFileName);
	strcpy(stFtpReq.aucClientFile, aucFullName);   /* 客户端文件名, 绝对路径 */


    stFtpReq.ulFtpCmd = CPSS_FTP_CMD_GET_FILE; /* 传输命令：下载*/
    stFtpReq.ulProgessStep = 0;
    stFtpReq.ulLen = 0;
    stFtpReq.pucMem = NULL;
    
    cps__oams_shcmd_printf("download %s begin\n",aucFullName);  
    
    /*调用ftp处理*/
    lRet = cpss_fs_ftp(&stFtpReq, 0);
    if (CPSS_OK == lRet)
    {
      cps__oams_shcmd_printf("License file %s download success,please reset board\n",aucFullName);      
    }
    else
    {
      cps__oams_shcmd_printf("License file download fail\n");
    }      
    return;    
}

#endif 
#endif

#if (SWP_CPUROLE_DSP != SWP_CPUROLE_TYPE)
/***************************************************************************
* 函数名称: smss_sysctl_recv_verm_active
* 功    能: 收到VERM发来的延迟复位命令
* 参    数:
* 参数名称        类型                 输入/输出       描述
* 函数返回:
*          无。
* 说   明: 
***************************************************************************/
VOID smss_sysctl_recv_verm_active(UINT32 ulReason)
{
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
           "<%s-%s>: Received message %s.",
           g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
           "SMSS_VERM_MASA_ACTIVEDELAYIND_MSG");
	g_stSmssBaseInfo.ulResetReason = ulReason;
	cpss_vk_proc_user_state_set(SMSS_SYSCTL_STATE_END);
  g_stSmssBaseInfo.ucRStatus = SMSS_RSTATUS_END;
  g_stSmssBaseInfo.szRStatus = "END";
  g_stSmssAsInfo.ucSelfAStatus = SMSS_UNKNOWN_AS_STATUS;
  g_stSmssAsInfo.szSelfAStatus = "UNKNOWN";
  #if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)  /* 如果为主CPU，设置面板灯 */
    drv_board_led_set(DRV_LED_TYPE_IS, DRV_LED_STATU_OFF);
    drv_board_led_set(DRV_LED_TYPE_ACTIVE, DRV_LED_STATU_OFF);
    smss_devm_SetISled(0);
    smss_devm_SetActled(0);
 #endif
 cpss_timer_set(SMSS_TIMER_END, SMSS_TIMER_END_DELAY);
	return;
}
#endif

/***************************************************************************
* 函数名称: smss_sysctl_state_init_func
* 功    能: 处理SMSS的主控模块初始化过程中的消息。
* 函数类型  VOID
* 参    数:
* 参数名称      类型                 输入/输出       描述
* pstMsg CPSS_COM_MSGHEAD_T *           IN         消息头结构
* 函数返回:
*           无
* 说    明: 2006.5.10添加对SMSS_SYSCTL_BFAULT_IND_MSG的处理。
***************************************************************************/
VOID smss_sysctl_state_init_func(CPSS_COM_MSG_HEAD_T *pstMsg )
{
#ifdef SMSS_BACKUP_MATE                    /* 进行主备相关的初始化 */
    BOOL bDealMessage;
    bDealMessage = smss_sysctl_init_as_func(pstMsg);
    if (TRUE == bDealMessage)
        return;
#endif


    switch(pstMsg->ulMsgId)
    {
    case SMSS_PROC_ACTIVATE_RSP_MSG:       /* 纤程激活应答消息 */
        smss_sysctl_recv_act_stoa_rsp(pstMsg);
        break;

    case SMSS_SYSCTL_STATUS_CHG_ACK_MSG:   /* 状态改变确认消息 */
        smss_sysctl_recv_statuschgack(pstMsg);
        break;
        
    case SMSS_RESET_BOARD_CMD_MSG:         /* 复位单板命令 */
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
           "<%s-%s>: Received message %s.",
           g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
           "SMSS_RESET_BOARD_CMD_MSG");
        smss_sysctl_brd_reset(((SMSS_RESET_BOARD_CMD_MSG_T *)pstMsg->pucBuf)->ulReason);
        break;

    case SMSS_DEVM_BFAULT_IND_MSG:         /* 收到设备故障进行处理 */
        smss_sysctl_recv_devmreportfault();
        break;

    case SMSS_TIMER_INIT_MSG:              /* 初始化过程中的超时处理 */
        smss_sysctl_state_init_timeout();
        break;

    case SMSS_SYSCTL_BFAULT_IND_MSG:
        /* 初始化阶段收到的板级故障消息只可能来自伙伴板，此时，
           若本板为备用板，则直接复位,若为主用板，不做任何处理 */
        if (SMSS_STANDBY == g_stSmssAsInfo.ucSelfAStatus)
        {
           smss_sysctl_enter_end(TRUE,
               SMSS_BRD_RST_CAUSE_RECV_MATA_FAULT_SLAVE_BRD_INIT);
        }
        break;

    case SMSS_SYSCTL_REG_RSP_MSG:    /* 收到GCPA重发的注册应答消息，不处理 */
        break;
    
    #if (SWP_CPUROLE_DSP != SWP_CPUROLE_TYPE)
    case SMSS_VERM_MASA_ACTIVEDELAYIND_MSG:  /* 收到延迟激活命令 */
        smss_sysctl_recv_verm_active(((SMSS_RESET_BOARD_CMD_MSG_T *)pstMsg->pucBuf)->ulReason);
        break;     
    #endif
        
    default:                            /* 收到无效消息，进行告警 */
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: Received unknown message 0x%08X.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, pstMsg->ulMsgId);
        break;
    } /*end of switch */
}

/***************************************************************************
* 函数名称: smss_sysctl_recv_act_stoa_rsp
* 功    能: 处理纤程应答消息
* 参    数:
* 参数名称        类型                 输入/输出       描述
* ulProcDes      UINT32                   IN          纤程在描述符
* ulResult       UINT32                   IN          纤程返回的结果
* 函数返回:
*           无。
* 说    明: 本函数在纤程被激活和备升主中都被使用。
***************************************************************************/
VOID smss_sysctl_recv_act_stoa_rsp(CPSS_COM_MSG_HEAD_T *pstMsg)
{
    UINT32 ulProcDes;
    UINT32 ulResult;
    UINT32 ulIndex;

		/*  yanghuanjun 20061206 begin */
		#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
		 CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_5InCnt);
		#endif

    ulProcDes = pstMsg->stSrcProc.ulPd;
    ulResult = (((SMSS_PROC_ACTIVATE_RSP_MSG_T*)pstMsg->pucBuf)->ulResult);  /* 本地序 */
    ulIndex = smss_sysctl_procdes2index(ulProcDes);

    /* begin of for GCPA 初始化监控修改 */
    pulProcActRsp[ulIndex] = ulResult;
    /* begin of for GCPA 初始化监控修改 */

    if (0 != ulResult) /* 激和失败, 写日志 */
    {
    	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
               "<%s-%s>: Received %s from Proc `%s', ulResult = %d.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               SMSS_RSTATUS_STOA == g_stSmssBaseInfo.ucRStatus ?
               "SMSS_STANDBY_TO_ACTIVE_RSP_MSG": "SMSS_PROC_ACTIVATE_RSP_MSG" ,
               cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[ulIndex].ulProcDes), 
               ulResult);
    }
    else
    {
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Received %s from Proc `%s', ulResult = %d.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               SMSS_RSTATUS_STOA == g_stSmssBaseInfo.ucRStatus ?
               "SMSS_STANDBY_TO_ACTIVE_RSP_MSG": "SMSS_PROC_ACTIVATE_RSP_MSG" ,
               cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[ulIndex].ulProcDes), 
               ulResult);
    }
           
    smss_sysctl_actproc_stoarsp_event(pstMsg->ulMsgId, ulProcDes, ulResult);
    if (SMSS_OK != ulResult)   /* 激活失败，进行告警，进入终止态并复位*/
    {
    	 smss_sysctl_write_sbbr("<%s-%s>: Received %s from Proc `%s', ulResult = %d.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               SMSS_RSTATUS_STOA == g_stSmssBaseInfo.ucRStatus ?
               "SMSS_STANDBY_TO_ACTIVE_RSP_MSG": "SMSS_PROC_ACTIVATE_RSP_MSG" ,
               cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[ulIndex].ulProcDes), 
               ulResult);
        
    	 /* 主用全局板收到CPS__RDBS_INIT_PROC纤程激活失败，停止运行，等待手动下载数据文件 */
    	 #ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL
    	  /* 主用全局板激和失败不复位 */
    	  if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
    	  {
    	     if (CPSS_TD_INVALID != pulProcActResult[ulIndex])   /* 如果设置有定时器，则删除定时器 */
           {
              cpss_timer_para_delete(pulProcActResult[ulIndex]);
					    pulProcActResult[ulIndex] = CPSS_TD_INVALID;
           }                       
           cps__oams_shcmd_printf("\nProc '%s' activete fail, please check\n",
                             cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[ulIndex].ulProcDes)); 
           

           /* 设置为END态, 等待备用升主用 */
           cpss_vk_proc_user_state_set(SMSS_SYSCTL_STATE_END);
				   g_stSmssBaseInfo.ucRStatus = SMSS_RSTATUS_END;
				   g_stSmssBaseInfo.szRStatus = "END";
				   g_stSmssAsInfo.ucSelfAStatus = SMSS_UNKNOWN_AS_STATUS;
				   g_stSmssAsInfo.szSelfAStatus = "UNKNOWN";        

		g_szInitFailProcName  = cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[ulIndex].ulProcDes);
		smss_sysctl_enter_end(FALSE,  SMSS_BRD_RST_CAUSE_ACT_PROC_FAILURE_SLAVE_BRD_STOA);

           return;	
    	  }
    	  
    	  if ((SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus) && (SMSS_RSTATUS_INIT == g_stSmssBaseInfo.ucRStatus)
    	        && (CPS__RDBS_INIT_PROC == g_pstSmssSysctlProcTable[ulIndex].ulProcDes))
    	  {
    	    	if (CPSS_TD_INVALID != pulProcActResult[ulIndex])   /* 如果设置有定时器，则删除定时器 */
            {
              cpss_timer_para_delete(pulProcActResult[ulIndex]);
					pulProcActResult[ulIndex] = CPSS_TD_INVALID;
            }
            
            cps__oams_shcmd_printf("\nProc '%s' activete fail, please download rdbs file\n",
                               cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[ulIndex].ulProcDes));              
            return;
    	  }

		  /* 主用全局板，初始化阶段，收到CPS__RDBS_R的RDBSR_MAIN_PROC纤程激活失败响应消息时，不复位单板，打印相关信息，
		  提示人工下载 license 文件。*/
		  if ((SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus) && (SMSS_RSTATUS_INIT == g_stSmssBaseInfo.ucRStatus)
			  && (RDBSR_MAIN_PROC == g_pstSmssSysctlProcTable[ulIndex].ulProcDes))
		  {
			  if (CPSS_TD_INVALID != pulProcActResult[ulIndex])   /* 如果设置有定时器，则删除定时器 */
			  {
				  cpss_timer_para_delete(pulProcActResult[ulIndex]);
				  pulProcActResult[ulIndex] = CPSS_TD_INVALID;
			  }
			  
			  cps__oams_shcmd_printf("\nProc '%s' activete fail, please download License file\n",
                               cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[ulIndex].ulProcDes));
			  return;
		  }

		  /* 在网元升级过程中，备用全局板备升主失败，即收到备升主失败响应消息 */
		 if ((SMSS_STANDBY == g_stSmssAsInfo.ucSelfAStatus) && (SMSS_RSTATUS_STOA == g_stSmssBaseInfo.ucRStatus)
    	        && (SMSS_UPGSTATUS_UPGRADING == g_NeUpgTagSmss))
    	  {
    	    if (CPSS_TD_INVALID != pulProcActResult[ulIndex])   /* 如果设置有定时器，则删除定时器 */
            {
				cpss_timer_para_delete(pulProcActResult[ulIndex]);
				pulProcActResult[ulIndex] = CPSS_TD_INVALID;
            }
            
            cps__oams_shcmd_printf("\nProc '%s' Proc activete fail, \n",
                               cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[ulIndex].ulProcDes)); 
			
			/* 置状态为正常 */
	  	    smss_verm_set_upgstatus(SMSS_UPGSTATUS_NORMAL);
	  	    g_NeUpgTagSmss = SMSS_UPGSTATUS_NORMAL;
            
			/* 向主用全局板发送整网元升级失败指示 */
			smss_com_send_mate(SMSS_SYSCTL_PROC, SMSS_SYSCTL_GCPA_UPGRADE_FAIL_IND_MSG,
                               NULL, 0);

			/* 设置定时器，准备复位单板 */
			cpss_timer_set(SMSS_TIMER_STANDBY_RESET_IND,SMSS_TIMER_STANDBY_RESET_DELAY);

            return;
    	  }

		 if ((SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus) && (SMSS_RSTATUS_INIT == g_stSmssBaseInfo.ucRStatus)
    	        && (SMSS_UPGSTATUS_UPGRADING == g_NeUpgTagSmss))
		 {
			 /* 置状态为正常 */
	  	    smss_verm_set_upgstatus(SMSS_UPGSTATUS_NORMAL);
	  	    g_NeUpgTagSmss = SMSS_UPGSTATUS_NORMAL;
		 }

    	 #endif
    	  
    	
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: Transit to state <%s>.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, "END");
        if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
            /* 主用板延时复位 */
        {
            smss_sysctl_enter_end(FALSE,
                                  SMSS_BRD_RST_CAUSE_ACT_PROC_FAILURE_SLAVE_BRD_STOA);
        }
        else /* 备用板立即复位 */
        {
            smss_sysctl_enter_end(TRUE,
                                  SMSS_BRD_RST_CAUSE_ACT_PROC_FAILURE_SLAVE_BRD_STOA);
        }        
        return;
    }

    if (CPSS_TD_INVALID != pulProcActResult[ulIndex])   /* 如果设置有定时器，则删除定时器 */
    {
        cpss_timer_para_delete(pulProcActResult[ulIndex]);
	    pulProcActResult[ulIndex] = CPSS_TD_INVALID;
    }

    pulProcActResult[ulIndex] = ulResult;   /* 登记该结果 */

    if (0 != g_pstSmssSysctlProcTable[ulIndex].ulWaitTime)
    {
        /* 异步激活时不处理响应消息 */
        g_ulSmssRspedProcNum = g_ulSmssRspedProcNum + 1;
    }

    if (g_ulSmssRspedProcNum == g_ulSmssSysctlProcNum)
        /* 收到所有纤程的应答消息，根据不同的状态进行不同的处理 */
    {
        /* 设置状态， 删除等待所有纤程应答消息的定时器 */
        ucActProcStage = SMSS_SEND_OK;
        cpss_timer_delete(SMSS_TIMER_INIT);

        if (SMSS_RSTATUS_INIT == g_stSmssBaseInfo.ucRStatus)
            /* 如果在初始化态，则初始化完成 */
        {
            smss_sysctl_init_complete();
        }
#ifdef SMSS_BACKUP_MATE
        else if (SMSS_RSTATUS_STOA == g_stSmssBaseInfo.ucRStatus)
            /* 如果在备升主态，则备升主完成 */
        {
            smss_sysctl_stoa_complete();
        }
#endif
    }
    /* 若收到的激活成功消息为同步纤程的应答消息且还有纤程需要激活，则接着激活 */
    else
    {
        if (  (ulIndex == g_ulSmssSendIndex-1)                /* 该纤程为刚激活的纤程 */
              &&(0 != g_pstSmssSysctlProcTable[ulIndex].ulWaitTime)   /* 该纤程为同步纤程 */
              &&(g_ulSmssSendIndex != g_ulSmssSysctlProcNum)          /* 还有纤程序要激活 */
            )
        {
            smss_sysctl_send_act_stoa_req();
        }
    }
    
    /*  yanghuanjun 20061206 begin */
		#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
		 CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_5OutCnt);
		#endif
		/*  yanghuanjun 20061206 end */
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_init_complete
* 功    能: 收到所有纤程的激活应答消息后进行处理
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_init_complete(VOID)
{
    /* 广播激活完成消息 */
    smss_sysctl_broadcast(SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG, NULL, 0);

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
    if (SMSS_STANDBY == g_stSmssAsInfo.ucSelfAStatus) 
        /* 若本板是非主用全局板则向主用全局板报告 */
    {
        /* 进行主备相关定时器设置 */
#ifdef SMSS_BACKUP_MATE
        smss_sysctl_as_start_mate_heartbeat();
#endif
        g_ucSmssSendCount = 0;
        smss_sysctl_send_statuschgind();
        cpss_timer_loop_set(SMSS_TIMER_INIT, SMSS_TIMER_INIT_DELAY);
    }
    else /* 若是主用全局板则进入服务态 */
    {
        CPS__OAMS_AM_ORIGIN_EVENT_T stOamsEvent;

        smss_sysctl_enter_normal();
        smss_sysctl_set_cps__rdbs_rstatus(g_stSmssBaseInfo.stPhyAddr, SMSS_RSTATUS_NORMAL);
        smss_sysctl_set_cps__rdbs_astatus(g_stSmssBaseInfo.stPhyAddr, SMSS_ACTIVE);
        

        smss_sysctl_set_cps__rdbs_brdstatus(g_stSmssBaseInfo.stPhyAddr,CPS__RDBS_DEV_VALUE_BOARD_NORMAL);
        smss_sysctrl_set_smss_boardstate(g_stSmssBaseInfo.stPhyAddr,SMSS_BRD_RSTATUS_NORMAL);

        
        /* 向OAMS上报“单板启动”事件 */
        stOamsEvent.ulEventNo = SMSS_EVENTNO_BOARD_STARTUP;
        stOamsEvent.ulEventSubNo = 0;

        smss_moid_get_local_moid(CPS__OAMS_MOID_TYPE_BOARD,&stOamsEvent.stEventMoId);

        stOamsEvent.ulExtraLen = 0;
        /* stOamsEvent.aucExtraInfo[0] = 0; */
        smss_send_event_to_oams(&stOamsEvent);
        /* 除备用全局板外，其它板均起上下级心跳定时器 */
        cpss_timer_loop_set(SMSS_TIMER_PERIODLY_HEARTBEAT, SMSS_TIMER_PERIODLY_HEARTBEAT_INTERVAL);

    }
#else  /* 非全局板 */ 
    /* 若本板是非全局板则向主用全局板报告 */
    {
        /* 进行主备相关定时器设置 */
#ifdef SMSS_BACKUP_MATE
        smss_sysctl_as_start_mate_heartbeat();
#endif
        g_ucSmssSendCount = 0;
        smss_sysctl_send_statuschgind();
        cpss_timer_loop_set(SMSS_TIMER_INIT, SMSS_TIMER_INIT_DELAY);
        /* 除备用全局板外，其它板均起上下级心跳定时器 */
        cpss_timer_loop_set(SMSS_TIMER_PERIODLY_HEARTBEAT, SMSS_TIMER_PERIODLY_HEARTBEAT_INTERVAL);
    }
#endif /* 全局板 */

#ifdef SMSS_BOARD_WITH_SLAVE  /* 带有从处理器 */
    if(g_ulResetSlave)
    {
       /* 复位从CPU */
       drv_reset_slave_cpu();
   
	   cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: setout call function:  %s()",g_stSmssAsInfo.szSelfAStatus,
			   g_stSmssBaseInfo.szRStatus,"drv_master_cpu_syn_notify");

	   /* 在主CPU上置主CPU已准备好状态，即写一个CPLD寄存器（写值为0xa5）通知从处理器主处理器已经准备好 */
	   drv_master_cpu_syn_notify();
    } 
#endif
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_normal_set_upperclk
* 功    能: 单板初始化完成后， 设置上级时钟
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_normal_set_upperclk(VOID)
{	
	/* 时钟板初始化完为主，向所有的二级交换板通告 */
	#if (SWP_PHYBRD_TYPE == SWP_PHYBRD_NSCA)
	  if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
	  {
	    smss_sysctl_nsca_stoa_notify(g_stSmssBaseInfo.stPhyAddr.ucSlot);	
	  }
	#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_MASA) /* 查询时钟板 */
	  {
	  	SMSS_SYSCTL_UPPERCLK_QUERY_REQ_MSG_T  stReq;
	     	
	    stReq.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
	    smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_UPPERCLK_QUERY_REQ_MSG,
                              (UINT8 *)&stReq, sizeof(stReq));
                              
      cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: send %s to gcpa self(%d-%d-%d %d)",g_stSmssAsInfo.szSelfAStatus,
			         g_stSmssBaseInfo.szRStatus,"SMSS_SYSCTL_UPPERCLK_QUERY_REQ_MSG",
			          g_stSmssBaseInfo.stPhyAddr.ucFrame, g_stSmssBaseInfo.stPhyAddr.ucShelf,
			          g_stSmssBaseInfo.stPhyAddr.ucSlot, g_stSmssBaseInfo.stPhyAddr.ucCpu);                          
			
		  /* 主用MASA， 向节点板通告 */	
		  if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
		  {
		 	  smss_sysctl_send_upperclk(g_stSmssBaseInfo.stPhyAddr.ucSlot);
		  } 	   
	  }	
	#else  /* 其他单板，查询主用MASA */
	  { 
      SMSS_SYSCTL_UPPERCLK_QUERY_REQ_MSG_T  stReq;
	     	
	    stReq.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
	    smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_UPPERCLK_QUERY_REQ_MSG,
                              (UINT8 *)&stReq, sizeof(stReq));
                              
      cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: send %s to gcpa self(%d-%d-%d %d)",g_stSmssAsInfo.szSelfAStatus,
			         g_stSmssBaseInfo.szRStatus,"SMSS_SYSCTL_UPPERCLK_QUERY_REQ_MSG",
			         g_stSmssBaseInfo.stPhyAddr.ucFrame, g_stSmssBaseInfo.stPhyAddr.ucShelf,
			         g_stSmssBaseInfo.stPhyAddr.ucSlot, g_stSmssBaseInfo.stPhyAddr.ucCpu);                          	 
	  }		   
	#endif
	
	/* MNPA, MEIA 主CPU初始化设置时钟提取 */
	#if ((SWP_PHYBRD_TYPE == SWP_PHYBRD_MNPA) || (SWP_PHYBRD_TYPE == SWP_PHYBRD_MEIA))
	 #if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)
	 {
	 	#ifndef SMSS_BOARD_WITHOUT_RDBS
	 	 UINT32 ulIndex;
	 	 CPS__RDBS_ATM_GETCLKEXTRACTCFG_MSG_RSP_T  stRspMsg;
	 	 
	   cps__rdbs_if_entry(CPS__RDBS_ATM_GETCLKEXTRACTCFG_MSG, CPS__RDBS_IF_FUNC,
                  NULL, 0,
                  (UINT8 *)&stRspMsg, sizeof(stRspMsg));
     if (0 == stRspMsg.ulResult) /* 查询成功 */
     {
     	  for (ulIndex = 0; ulIndex < stRspMsg.ulPortNum; ulIndex++)
     	  {
     	    drv_refclk_cfg(stRspMsg.astPortInfo[ulIndex].ucPortId,  DRV_REFCLK_CFG_EN);
     	    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: clkcfg devId %d, flag %d", g_stSmssAsInfo.szSelfAStatus,
			         g_stSmssBaseInfo.szRStatus, stRspMsg.astPortInfo[ulIndex].ucPortId,
			         DRV_REFCLK_CFG_EN); 
     	  }
     }     
    #endif         
	 }
	 #endif
	#endif
	return;
}

#if (SWP_CPUROLE_DSP != SWP_CPUROLE_TYPE)
/***************************************************************************
* 函数名称: smss_sysctl_save_cpuloadinfo
* 功    能: 保存CPU负荷控制信息(主/从)
* 函数类型  VOID
* 参    数: pRsp 
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_save_cpuloadinfo(CPS__RDBS_DEV_GETCPULOADCTRL_MSG_RSP_T *pRsp, SMSS_SYSCTL_CPULOADCTRL_INFO_T *pInfo)
{
	pInfo->stRsp = *pRsp;
  if (0 == pRsp->ulResult) /* 查询成功 */
  {
 	  pInfo->usWatchNum = pRsp->usWatchTimer / 5;
 	  if (0 == pInfo->usWatchNum) /* 参数保护 */ 
 	  {
 	 	  pInfo->usWatchNum = 1;
 	  }
 	  pInfo->ucCurrLevel = 255;  /* 默认 */ 
  }
  else  /* 查询失败, 设置默认值 */
  {
  	pInfo->usWatchNum = 1;
 	  pInfo->stRsp.usWatchTimer = 5;       /* 5s */
 	  pInfo->stRsp.usAlarmTimer = 15 * 60; /* 15min */
 	  pInfo->stRsp.ucLoadInfoNum = 1;
 	  pInfo->stRsp.astLoadInfo[0].ucLoadThreshold = 80;
  }
	return;
}

/***************************************************************************
* 函数名称: smss_sysctl_get_cpu_level
* 功    能: 查询CPU级别
* 函数类型  VOID
* 参    数:
* pInf  结构数组
* ucUsage CPU占用率
* 函数返回:
*           返回数组索引。
* 说    明: 
***************************************************************************/
UINT8 smss_sysctl_get_cpu_level(SMSS_SYSCTL_CPULOADCTRL_INFO_T *pInf, UINT8 ucUsage)
{
   UINT8 ulIndex;
   
   for (ulIndex = 0; ulIndex < pInf->stRsp.ucLoadInfoNum; ulIndex++)
   {
      if (ucUsage < pInf->stRsp.astLoadInfo[ulIndex].ucLoadThreshold)
      {
        break;	
      }	
   }   
   return ulIndex;
}

/***************************************************************************
* 函数名称: smss_sysctl_cpu_usage_process
* 功    能: 主从CPU占用率处理
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_cpu_usage_process(VOID)
{
	UINT8 ucTempLevel;
	UINT32 ulcurcpuusage = 0;  
	
	if (0 == g_smssCpuLoadInfo.usWatchNum) /* 没有获取控制信息 */
	{
	  return;	
	}
	
	cpss_cpu_usage_get(&ulcurcpuusage);
	g_smssCpuLoadInfo.usCurrNum = g_smssCpuLoadInfo.usCurrNum + 1;
	g_smssCpuLoadInfo.ulTotelNum = g_smssCpuLoadInfo.ulTotelNum + ulcurcpuusage;
	if (g_smssCpuLoadInfo.usCurrNum  == g_smssCpuLoadInfo.usWatchNum) /* 处理周期到 */
	{
	   g_smssCpuLoadInfo.ucCurrUsage = 	g_smssCpuLoadInfo.ulTotelNum / g_smssCpuLoadInfo.usWatchNum;
	   g_smssCpuLoadInfo.usCurrNum = 0;
	   g_smssCpuLoadInfo.ulTotelNum = 0;
	   
	   /* 获取级别 */
	   ucTempLevel = smss_sysctl_get_cpu_level(&g_smssCpuLoadInfo, g_smssCpuLoadInfo.ucCurrUsage);
	   if (ucTempLevel != g_smssCpuLoadInfo.ucCurrLevel)
	   {
	      /* 上报事件 */
	      
	      /* 设置速率 */
	      if (0 == g_smssCpuLoadInfo.stRsp.ulResult)
	      {
	      	cpss_log_rate_set(g_smssCpuLoadInfo.stRsp.astLoadInfo[ucTempLevel].usLogRate);
	      }
	      
	      g_smssCpuLoadInfo.ucCurrLevel = ucTempLevel;
	      if (ucTempLevel == g_smssCpuLoadInfo.stRsp.ucLoadInfoNum)	 /* 最高级别, 启定时器 */
	      {
	        cpss_timer_set(SMSS_TIMER_CPU_USAGE, SMSS_TIMER_CPU_USAGE_DELAY);	
	      }
	      else
	      {
	      	cpss_timer_delete(SMSS_TIMER_CPU_USAGE);
	      	if (1 == g_stCpuUsageInfo.ulCurCpuUsageHighCount) /* 有告警产生 */
	      	{
	      		 smss_sysctl_cpu_alarm(g_smssCpuLoadInfo.ucCurrUsage, CPS__OAMS_ALARM_CLEAR);
	      		 g_stCpuUsageInfo.ulCurCpuUsageHighCount = 0;
	      	}
	      }
	   }
	   
	   #if (SWP_CPUROLE_SLAVE == SWP_CPUROLE_TYPE) /* 从向主通知CPU占用率 */
	   {
	   	  SMSS_SYSCTL_SLAVECPU_IND_MSG_T  stInd;
	   	  stInd.ucCpuUsage = g_smssCpuLoadInfo.ucCurrUsage;
	   	  smss_sysctl_send_host(SMSS_SYSCTL_PROC, SMSS_SYSCTL_SLAVECPU_IND_MSG,
                              (UINT8 *)&stInd, sizeof(stInd));
	   }	   
	   #endif
	}
	return;
}


#ifdef SMSS_BOARD_WITH_DSP
/***************************************************************************
* 函数名称: smss_sysctl_dsp_usage_process
* 功    能: 处理DSP的CPU占用率
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_dsp_usage_process(VOID)
{
	 UINT32 ulIndex;
	 UINT32 ulDspLen = 0;
	 UINT32 ulDspNum;
	 UINT32 ulDspUsage;
	 UINT32 ulTotelUsage = 0;  /* 计算DSP平均CPU占用率 */
	 UINT8  ucTempLevel;
   CPSS_KW_DSP_USAGE_T  stDspUsage[SMSSDSPNUM];
   SMSS_SYSCTL_CPULOADCTRL_INFO_T *pDspInfo;
   
   cpss_kw_dsp_cpuusage_get(SMSSDSPNUM, (CPSS_KW_DSP_USAGE_T *)&stDspUsage, &ulDspLen);
   if (0 == ulDspLen)  /* 没有DSP正常 */
   {
     return;	
   }
   
   for (ulIndex = 0; ulIndex < ulDspLen; ulIndex++)
   {
     	ulDspNum = stDspUsage[ulIndex].ulDspNo - DSP_CPUNO_BASE;  /* 0-11 */
     	ulDspUsage = stDspUsage[ulIndex].ulDspUsage;
     	ulTotelUsage = ulTotelUsage + ulDspUsage;
     	     	
     	/* 单个DSP信息处理 */     	
     	pDspInfo = &g_smssDspLoadInfo[ulDspNum];
     	pDspInfo->ucCurrUsage = (UINT8)ulDspUsage;
     	ucTempLevel = smss_sysctl_get_cpu_level(pDspInfo, (UINT8)ulDspUsage);
     	if (ucTempLevel != pDspInfo->ucCurrLevel)
     	{
     	  /* 上报事件 */
     	  
     	  pDspInfo->ucCurrLevel = ucTempLevel;     	  
     	}     	
   }
   
   /* 记录平均占用率 */
   g_smssBoardLoadInfo.ucDspCpu = ulTotelUsage / ulDspLen;
   return;	
}
#endif


/***************************************************************************
* 函数名称: smss_sysctl_get_cpu_load_info
* 功    能: 获取CPU,DSP的负荷控制信息
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_get_cpu_load_info(VOID)
{
	CPS__RDBS_DEV_GETCPULOADCTRL_MSG_REQ_T  stReq;	
	
	stReq.stCpuPhyAddr = g_stSmssBaseInfo.stPhyAddr;	
  #ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL  /* 主用全局板直接查询 */  	
    if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus) 
    {       
       CPS__RDBS_DEV_GETCPULOADCTRL_MSG_RSP_T  stRsp;
       cps__rdbs_if_entry(CPS__RDBS_DEV_GETCPULOADCTRL_MSG,  CPS__RDBS_IF_FUNC, 
        (UINT8 *)&stReq, sizeof(stReq),
        (UINT8 *)&stRsp, sizeof(stRsp));
       smss_sysctl_save_cpuloadinfo(&stRsp, &g_smssCpuLoadInfo); /* 保存 */          	
    }
    else  /* 备用全局板发消息查询 */
    {
    	smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_CPULOADINFO_REQ_MSG,
                              (UINT8 *)&stReq, sizeof(stReq));
    }
  #else  /* 外围板 */
    #if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE) /* 主CPU, 向主用全局板发 */
      smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_CPULOADINFO_REQ_MSG,
                              (UINT8 *)&stReq, sizeof(stReq));
      #if defined(SMSS_BOARD_WITH_DSP) /* DSP在主CPU上处理 */
      {
      	 UINT32  ucIndex;
      	 for (ucIndex = DSP_CPUNO_BASE; ucIndex < DSP_CPUNO_BASE + SMSSDSPNUM; ucIndex++)
      	 {
      	 	 stReq.stCpuPhyAddr.ucCpu = ucIndex;
      	 	 smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_CPULOADINFO_REQ_MSG,
                              (UINT8 *)&stReq, sizeof(stReq));
      	 }
      }      
      #endif                        
    #elif (SWP_CPUROLE_SLAVE == SWP_CPUROLE_TYPE) /* 从CPU, 向主CPU发 */
      smss_sysctl_send_host(SMSS_SYSCTL_PROC, SMSS_SYSCTL_CPULOADINFO_REQ_MSG,
                              (UINT8 *)&stReq, sizeof(stReq));
    #endif
  #endif  
  return;	
}


#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
/***************************************************************************
* 函数名称: smss_sysctl_notify_board_level
* 功    能: 设置RDBS单板级别
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_notify_board_level(UINT32 ucLevel)
{
	CPS__RDBS_DEV_SETOVERLOADSTATE_MSG_REQ_T  stReq;
	
	stReq.stBoardPhyAddr = g_stSmssBaseInfo.stPhyAddr;
	stReq.ucOverloadState = (UINT8)ucLevel;
	
  #ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL  /* 主用全局板直接查询 */  	
    if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus) 
    {       
       CPS__RDBS_DEV_SETOVERLOADSTATE_MSG_RSP_T  stRsp;
       cps__rdbs_if_entry(CPS__RDBS_DEV_SETOVERLOADSTATE_MSG,  CPS__RDBS_IF_FUNC, 
        (UINT8 *)&stReq, sizeof(stReq),
        (UINT8 *)&stRsp, sizeof(stRsp));
    }
    else  /* 备用全局板发消息查询 */
    {
    	smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BOARDLEVELCHG_IND_MSG,
                              (UINT8 *)&stReq, sizeof(stReq));
    }
  #else  /* 外围板 */
      smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BOARDLEVELCHG_IND_MSG,
                              (UINT8 *)&stReq, sizeof(stReq));  
  #endif
  return;	
}

/***************************************************************************
* 函数名称: smss_sysctl_board_usage_process
* 功    能: 单板负荷控制信息处理
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_board_usage_process(VOID)
{
	UINT8  ucTemp;
	UINT32 ulIndex;
	
	if (0 == g_smssBoardLoadInfo.stRsp.ulResult)  /* 有信息才处理 */
	{
		 /* 获取单板的CPU占用率 */
		 #ifdef SMSS_BOARD_WITH_SLAVE
		   ucTemp = g_smssCpuLoadInfo.ucCurrUsage > (g_smssBoardLoadInfo.stRsp.ucSlaveCpuCoeff * g_smssBoardLoadInfo.ucSlaveCpu) ?
		           g_smssCpuLoadInfo.ucCurrUsage : (g_smssBoardLoadInfo.stRsp.ucSlaveCpuCoeff * g_smssBoardLoadInfo.ucSlaveCpu);
		 #elif defined(SMSS_BOARD_WITH_DSP)
		   ucTemp = g_smssCpuLoadInfo.ucCurrUsage > (g_smssBoardLoadInfo.stRsp.ucSlaveCpuCoeff * g_smssBoardLoadInfo.ucDspCpu) ?
		           g_smssCpuLoadInfo.ucCurrUsage : (g_smssBoardLoadInfo.stRsp.ucSlaveCpuCoeff * g_smssBoardLoadInfo.ucDspCpu); 
     #else
       ucTemp = g_smssCpuLoadInfo.ucCurrUsage;
     #endif
		
		 g_smssBoardLoadInfo.ucCurrUsage = ucTemp;		
		 /* 获取级别 */
		 for (ulIndex = 0; ulIndex < g_smssBoardLoadInfo.stRsp.ucLoadNum; ulIndex++)
		 {
		   if (ucTemp < g_smssBoardLoadInfo.stRsp.aucLoadThreshold[ulIndex])
		   {
		     break;	
		   }			   
		 }
		
		 if (ulIndex != g_smssBoardLoadInfo.ucCurrLevel)
		 {
		  /* 通知RDBS */
		  smss_sysctl_notify_board_level(ulIndex + 1);
		  g_smssBoardLoadInfo.ucCurrLevel = ulIndex;	
		 }
	} 
	return;
}

/***************************************************************************
* 函数名称: smss_sysctl_save_boardloadinfo
* 功    能: 保存单板的负荷控制信息
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_save_boardloadinfo(CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG_RSP_T *pRsp)
{
	g_smssBoardLoadInfo.stRsp = *pRsp;
	if (0 == pRsp->ulResult) /* 成功 */ 
	{
	   g_smssBoardLoadInfo.ucCurrLevel = 255;	/* 默认 */
	}
	return;
}

/***************************************************************************
* 函数名称: smss_sysctl_get_board_load_info
* 功    能: 获取单板的负荷控制信息
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_get_board_load_info(VOID)
{
  CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG_REQ_T  stReq;	
	
	stReq.stCpuPhyAddr = g_stSmssBaseInfo.stPhyAddr;	
  #ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL  /* 主用全局板直接查询 */	
    if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus) 
    {       
       CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG_RSP_T  stRsp;
       cps__rdbs_if_entry(CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG, CPS__RDBS_IF_FUNC, 
        (UINT8 *)&stReq, sizeof(stReq),
        (UINT8 *)&stRsp, sizeof(stRsp));
       smss_sysctl_save_boardloadinfo(&stRsp); /* 保存 */          	
    }
    else  /* 备用全局板发消息查询 */
    {
    	smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BOARDLOADINFO_REQ_MSG,
                              (UINT8 *)&stReq, sizeof(stReq));
    }
  #else  /* 外围板 */
    #if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE) /* 主CPU, 向主用全局板发 */
      smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BOARDLOADINFO_REQ_MSG,
                              (UINT8 *)&stReq, sizeof(stReq));
    #endif
  #endif    
  return;	
}


/***************************************************************************
* 函数名称: smss_sysctl_boardloadinfo_rsp
* 功    能: 收到单板的负荷控制信息
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_boardloadinfo_rsp(UINT8 *pucBuf)
{
	CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG_RSP_T  *pRsp;
	pRsp = (CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG_RSP_T *)pucBuf;
	
	if (NULL == pucBuf)
	{
	  cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
             "<%s-%s>: recv SMSS_SYSCTL_BOARDLOADINFO_RSP_MSG, para NULL",
             g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);  
	  return;	
	}
	
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
             "<%s-%s>: recv SMSS_SYSCTL_BOARDLOADINFO_RSP_MSG.",
             g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
  /* 转换为本地序 */
  pRsp->ulResult = cpss_ntohl(pRsp->ulResult);
  /* 保存 */
  smss_sysctl_save_boardloadinfo(pRsp);           
	return;
}

/***************************************************************************
* 函数名称: smss_sysctl_cpuloadchg_ind
* 功    能: 收到RDBS触发的负荷控制信息变更
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_cpuloadchg_ind(UINT8 *pucBuf)
{
	CPS__RDBS_DEV_CPULOADCTRLCHG_IND_MSG_T  *pInd;
	CPS__RDBS_DEV_GETCPULOADCTRL_MSG_RSP_T  stRsp;
	pInd = (CPS__RDBS_DEV_CPULOADCTRLCHG_IND_MSG_T *)pucBuf;
	
	if (NULL == pucBuf)
	{
    	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
             "<%s-%s>: recv CPS__RDBS_DEV_CPULOADCTRLCHG_IND_MSG, para NULL",
             g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);	  
	  return;	
	}
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
             "<%s-%s>: recv CPS__RDBS_DEV_CPULOADCTRLCHG_IND_MSG, cpu %d",
             g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
             pInd->ucCpu);
  
  /* 结构转换 */
  stRsp.ulResult = 0;
  stRsp.usWatchTimer = pInd->usWatchTimer;  /* 网络序 */
  stRsp.usAlarmTimer = pInd->usAlarmTimer;  /* 网络序 */
  stRsp.ucLoadInfoNum = pInd->ucLoadInfoNum;
  cpss_mem_memcpy(stRsp.astLoadInfo, pInd->astLoadInfo, sizeof(CPS__RDBS_DEV_CPULOADINFO_T) * (pInd->ucLoadInfoNum));
  
  if (CPS__RDBS_DEV_VT_SLAVECPU == pInd->ucCpu)  /* 从CPU信息变更 */
  {
  	smss_send_to_slave(SMSS_SYSCTL_PROC, SMSS_SYSCTL_CPULOADINFO_RSP_MSG,
                       &stRsp, sizeof(CPS__RDBS_DEV_GETCPULOADCTRL_MSG_RSP_T));
  	return;
  }  
  
  /* 转化为本地序 */
  stRsp.usWatchTimer = cpss_ntohs(pInd->usWatchTimer);
  stRsp.usAlarmTimer = cpss_ntohs(pInd->usAlarmTimer);
  if (CPS__RDBS_DEV_VT_HOSTCPU == pInd->ucCpu)  /* 主CPU, 保存 */
	{
		smss_sysctl_save_cpuloadinfo(&stRsp, &g_smssCpuLoadInfo);
	}
	else  /* DSP信息 */
	{
		#if defined(SMSS_BOARD_WITH_DSP)
		  if (DSP_CPUNO_BASE > pInd->ucCpu  || pInd->ucCpu > (DSP_CPUNO_BASE + SMSSDSPNUM))  /* 参数校验 */
		  {
		  	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
             "<%s-%s>: param  cpu %d error",
             g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
             pInd->ucCpu);
        return;     
		  }
		  smss_sysctl_save_cpuloadinfo(&stRsp, &g_smssDspLoadInfo[pInd->ucCpu - DSP_CPUNO_BASE]);
		#endif
	}		        
	return;
}

/***************************************************************************
* 函数名称: smss_sysctl_boardloadchg_ind
* 功    能: 收到RDBS触发的单板负荷控制信息变更
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_boardloadchg_ind(UINT8 *pucBuf)
{
	CPS__RDBS_DEV_BOARDLOADCTRLCHG_IND_MSG_T  *pInd;
	CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG_RSP_T  stRsp;
	pInd = (CPS__RDBS_DEV_BOARDLOADCTRLCHG_IND_MSG_T *)pucBuf;
	
	if (NULL == pucBuf)
	{
	  cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
             "<%s-%s>: recv CPS__RDBS_DEV_BOARDLOADCTRLCHG_IND_MSG, para NULL",
             g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);	  
	  return;	
	}
	
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
             "<%s-%s>: recv CPS__RDBS_DEV_BOARDLOADCTRLCHG_IND_MSG",
             g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
             
  /* 结构转换 */
  stRsp.ulResult = 0;
  stRsp.ucLoadNum = pInd->ucLoadNum;
  stRsp.ucSlaveCpuCoeff = pInd->ucSlaveCpuCoeff;
  cpss_mem_memcpy(stRsp.aucLoadThreshold, pInd->aucLoadThreshold, pInd->ucLoadNum);
   
  smss_sysctl_save_boardloadinfo(&stRsp);        
	return;
}

/***************************************************************************
* 函数名称: smss_sysctl_recv_slavecpu_ind
* 功    能: 收到从CPUCPU占用率指示消息
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_recv_slavecpu_ind(UINT8 *pucBuf)
{
  SMSS_SYSCTL_SLAVECPU_IND_MSG_T *pInd;
  pInd = (SMSS_SYSCTL_SLAVECPU_IND_MSG_T *)pucBuf;
  
  cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
             "<%s-%s>: recv SMSS_SYSCTL_SLAVECPU_IND_MSG, %d",
             g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
             pInd->ucCpuUsage);
  
  /* 记录 */
  g_smssBoardLoadInfo.ucSlaveCpu = pInd->ucCpuUsage;             
  return;	
}
#endif


/***************************************************************************
* 函数名称: smss_sysctl_get_load_info
* 功    能: 获取CPU,DSP和单板的负荷控制信息
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_get_load_info(VOID)
{
	smss_sysctl_get_cpu_load_info();
	#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
	 smss_sysctl_get_board_load_info();
	#endif
	return;
}

/***************************************************************************
* 函数名称: smss_sysctl_slavecpuload_req
* 功    能: 主CPU收到从CPU负荷控制请求
* 函数类型  VOID
* 参    数: pucBuf 
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_slavecpuload_req(UINT8 *pucBuf)
{
	CPS__RDBS_DEV_GETCPULOADCTRL_MSG_REQ_T *pReq;
	pReq = (CPS__RDBS_DEV_GETCPULOADCTRL_MSG_REQ_T *)pucBuf;
	
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
             "<%s-%s>: recv SMSS_SYSCTL_CPULOADINFO_REQ_MSG. para (%d-%d-%d %d)",
             g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
             pReq->stCpuPhyAddr.ucFrame, pReq->stCpuPhyAddr.ucShelf,
             pReq->stCpuPhyAddr.ucSlot,pReq->stCpuPhyAddr.ucCpu);
	/* 直接向主用全局板转发 */
	smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_CPULOADINFO_REQ_MSG,
                       (UINT8 *)pReq, sizeof(CPS__RDBS_DEV_GETCPULOADCTRL_MSG_REQ_T));
	return;
}

/***************************************************************************
* 函数名称: smss_sysctl_cpuloadinfo_rsp
* 功    能: 主CPU收到CPU负荷控制响应
* 函数类型  VOID
* 参    数: pucBuf
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_cpuloadinfo_rsp(UINT8 *pucBuf)
{
	CPS__RDBS_DEV_GETCPULOADCTRL_MSG_RSP_T *pRsp;
	pRsp = (CPS__RDBS_DEV_GETCPULOADCTRL_MSG_RSP_T *)pucBuf;
	
	if (NULL == pucBuf)
	{
	  cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
             "<%s-%s>: recv SMSS_SYSCTL_CPULOADINFO_RSP_MSG, para NULL",
             g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
	  return;	
	}
	
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
             "<%s-%s>: recv SMSS_SYSCTL_CPULOADINFO_RSP_MSG. cpu %d",
             g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
             pRsp->aucRsv[0]);
	if (CPS__RDBS_DEV_VT_SLAVECPU == pRsp->aucRsv[0]) /* 从CPU信息, 转发 */
	{
	  smss_send_to_slave(SMSS_SYSCTL_PROC, SMSS_SYSCTL_CPULOADINFO_RSP_MSG,
                       pRsp, sizeof(CPS__RDBS_DEV_GETCPULOADCTRL_MSG_RSP_T));
    return;                   	
	}
	
	/* 转换为本地序 */
	pRsp->ulResult = cpss_ntohl(pRsp->ulResult);
	pRsp->usWatchTimer = cpss_ntohs(pRsp->usWatchTimer);
	pRsp->usAlarmTimer = cpss_ntohs(pRsp->usAlarmTimer);
	
	if (CPS__RDBS_DEV_VT_HOSTCPU == pRsp->aucRsv[0])  /* 主CPU, 保存 */
	{
		smss_sysctl_save_cpuloadinfo(pRsp, &g_smssCpuLoadInfo);
	}
	else  /* DSP信息 */
	{
		#if defined(SMSS_BOARD_WITH_DSP)
		  smss_sysctl_save_cpuloadinfo(pRsp, &g_smssDspLoadInfo[pRsp->aucRsv[0] - DSP_CPUNO_BASE]);
		#endif
	}		
	return;
}


/***************************************************************************
* 函数名称: smss_sysctl_slavecpuloadinfo_rsp
* 功    能: 从CPU收到CPU负荷控制响应
* 函数类型  VOID
* 参    数: pucBuf
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_slavecpuloadinfo_rsp(UINT8 *pucBuf)
{
	CPS__RDBS_DEV_GETCPULOADCTRL_MSG_RSP_T *pRsp;
	pRsp = (CPS__RDBS_DEV_GETCPULOADCTRL_MSG_RSP_T *)pucBuf;
	
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
             "<%s-%s>: recv SMSS_SYSCTL_CPULOADINFO_RSP_MSG. cpu %d",
             g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
             pRsp->aucRsv[0]);
             
  /* 转换为本地序 */
	pRsp->ulResult = cpss_ntohl(pRsp->ulResult);
	pRsp->usWatchTimer = cpss_ntohs(pRsp->usWatchTimer);
	pRsp->usAlarmTimer = cpss_ntohs(pRsp->usAlarmTimer);
	smss_sysctl_save_cpuloadinfo(pRsp, &g_smssCpuLoadInfo);  /* 保存 */         
  return;           
}

/***************************************************************************
* 函数名称: smss_showloadinfo
* 功    能: show负荷控制响应
* 函数类型  VOID
* 参    数: 
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_showloadinfo(VOID)
{
	UINT32 ulIndex;
	
	if (0 != g_smssCpuLoadInfo.stRsp.ulResult)
	{
	   cps__oams_shcmd_printf("no load ctrl info.\n");
	   return;	
	}
	
  cps__oams_shcmd_printf("********* cpu load ctrl info**********\n");
  cps__oams_shcmd_printf("result 0x%x, currLevel %d, currUsgae %d\n",
                   g_smssCpuLoadInfo.stRsp.ulResult,
                   g_smssCpuLoadInfo.ucCurrLevel,
                   g_smssCpuLoadInfo.ucCurrUsage);
  cps__oams_shcmd_printf("watchTimer %d, alarmTimer %d, loadNum %d\n",
                   g_smssCpuLoadInfo.stRsp.usWatchTimer,
                   g_smssCpuLoadInfo.stRsp.usAlarmTimer,
                   g_smssCpuLoadInfo.stRsp.ucLoadInfoNum);                 
  for (ulIndex = 0; ulIndex < g_smssCpuLoadInfo.stRsp.ucLoadInfoNum; ulIndex++)
  {
     cps__oams_shcmd_printf("[%d]: Threshold %d, LogRate %d\n",
                   ulIndex,
                   g_smssCpuLoadInfo.stRsp.astLoadInfo[ulIndex].ucLoadThreshold,
                   g_smssCpuLoadInfo.stRsp.astLoadInfo[ulIndex].usLogRate);                           	     	
  }                    
  
  #ifdef SMSS_BOARD_WITH_DSP
  {
     UINT32 i;
     for (i = DSP_CPUNO_BASE; i < DSP_CPUNO_BASE + SMSSDSPNUM; i++)
     {
     	  cps__oams_shcmd_printf("****** dsp[%d] load ctrl info*******\n", i);
  	 	  cps__oams_shcmd_printf("Result 0x%x, currLevel %d, currUsgae %d\n",
                   g_smssDspLoadInfo[i - DSP_CPUNO_BASE].stRsp.ulResult,
                   g_smssDspLoadInfo[i - DSP_CPUNO_BASE].ucCurrLevel,
                   g_smssDspLoadInfo[i - DSP_CPUNO_BASE].ucCurrUsage);
			  cps__oams_shcmd_printf("watchTimer %d, alarmTimer %d, loadNum %d\n",
			                   g_smssDspLoadInfo[i - DSP_CPUNO_BASE].stRsp.usWatchTimer,
			                   g_smssDspLoadInfo[i - DSP_CPUNO_BASE].stRsp.usAlarmTimer,
			                   g_smssDspLoadInfo[i - DSP_CPUNO_BASE].stRsp.ucLoadInfoNum);                 
			  for (ulIndex = 0; ulIndex < g_smssDspLoadInfo[i - DSP_CPUNO_BASE].stRsp.ucLoadInfoNum; ulIndex++)
			  {
			     cps__oams_shcmd_printf("[%d]: Threshold %d, LogRate %d\n",
			                   ulIndex,
			                   g_smssDspLoadInfo[i - DSP_CPUNO_BASE].stRsp.astLoadInfo[ulIndex].ucLoadThreshold,
			                   g_smssDspLoadInfo[i - DSP_CPUNO_BASE].stRsp.astLoadInfo[ulIndex].usLogRate);                           	     	
			  }
     }
  }
  #endif
  #if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
    cps__oams_shcmd_printf("********* board load ctrl info**********\n");
    cps__oams_shcmd_printf("result 0x%x, currLevel %d, currUsgae %d\n" 
                      "ucSlave %d, ucDsp %d, coeff %d, loadNum %d\n",
                   g_smssBoardLoadInfo.stRsp.ulResult,
                   g_smssBoardLoadInfo.ucCurrLevel,
                   g_smssBoardLoadInfo.ucCurrUsage,
                   g_smssBoardLoadInfo.ucSlaveCpu,
                   g_smssBoardLoadInfo.ucDspCpu,
                   g_smssBoardLoadInfo.stRsp.ucSlaveCpuCoeff,
                   g_smssBoardLoadInfo.stRsp.ucLoadNum);
                                
    for (ulIndex = 0; ulIndex < g_smssBoardLoadInfo.stRsp.ucLoadNum; ulIndex++)
    {
      cps__oams_shcmd_printf("[%d]: Threshold %d\n",
                   ulIndex, g_smssBoardLoadInfo.stRsp.aucLoadThreshold[ulIndex]);                           	     	
    }    
  #endif
  return;	
}
#endif

/***************************************************************************
* 函数名称: smss_sysctl_enter_normal
* 功    能: 纤程激活完成后进入服务态
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 本函数在CPU初始化完成和备升主完成后被调用，初始化完成后要起定时器。
***************************************************************************/
VOID smss_sysctl_enter_normal(VOID)
{
	#if SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA
	CPSS_COM_PHY_ADDR_T stBrdPhyAddr;
	#endif
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
	"\n****************************************\n* All Proc INIT OK now in NORMAL state !\n****************************************\n");
	g_ulPowerOnTime = cpss_gmt_get();
#if 0
#if SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA

	stBrdPhyAddr.ucFrame = 1;
	stBrdPhyAddr.ucShelf = 1;
	stBrdPhyAddr.ucSlot = 1;
	stBrdPhyAddr.ucCpu = 1;

	cps_send_om_event(OM_AM_CPS_EVENT_CODE_BOARD_STARTUP, (UINT8*)&stBrdPhyAddr, sizeof(stBrdPhyAddr));
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
	"cps send om brd init finish ! brd = %d-%d-%d-%d\n",stBrdPhyAddr.ucFrame, stBrdPhyAddr.ucShelf, stBrdPhyAddr.ucSlot, stBrdPhyAddr.ucCpu);
#endif
#endif
    if (SMSS_RSTATUS_INIT == g_stSmssBaseInfo.ucRStatus)
        /* 如果是初始化完成，则设置心跳定时器 */
    {
            /* 如果是外围板或主用全局板，设置主用全局板和外围板间的心跳定时器 */
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL  /* 全局板 */
       if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
        {
            cpss_timer_loop_set(SMSS_TIMER_PERIODLY_HEARTBEAT,
                                SMSS_TIMER_PERIODLY_HEARTBEAT_INTERVAL);
        }       
#else  /* 非全局板 */
       cpss_timer_loop_set(SMSS_TIMER_PERIODLY_HEARTBEAT,
                           SMSS_TIMER_PERIODLY_HEARTBEAT_INTERVAL);

#endif
     #if (SWP_PHYBRD_TYPE != SWP_PHYBRD_MDPA)
      smss_devm_send_active_port();
     #endif 
     /* 查询CPU和单板的负荷控制信息 */

#if 0
     #if (SWP_CPUROLE_DSP != SWP_CPUROLE_TYPE)
       smss_sysctl_get_load_info();
     #endif
#endif
    }
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Transit to state <%s>.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, "NORMAL");

#ifdef SMSS_BACKUP_MATE
    g_stSmssAsInfo.stSwitchInfo.ulReason = SMSS_BSWITCH_MATE_FAULT;
#endif
 
    /* 清计数器，设置状态，进入正常态 */
    cpss_vk_proc_user_state_set(SMSS_SYSCTL_STATE_NORMAL);
    g_stSmssBaseInfo.ucRStatus = SMSS_RSTATUS_NORMAL;
    g_stSmssBaseInfo.szRStatus = "NORMAL";
    g_ulSmssSendIndex = 0;
    g_ulSmssRspedProcNum = 0;
    ucActProcStage = SMSS_SEND_PROC;

    /* 初始化完成，设置上级时钟 */
    smss_sysctl_normal_set_upperclk(); 

#if defined(SMSS_BOARD_WITH_DSP)||defined(SMSS_BOARD_WITH_SLAVE)
#else
#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)  /* 如果为主CPU，设置面板灯 */
    g_ucBoardState = SMSS_BRD_RSTATUS_NORMAL;    
        drv_board_led_set(DRV_LED_TYPE_IS, DRV_LED_STATU_ON);
        smss_devm_SetISled(1);
#endif
#endif

#ifdef SMSS_BOARD_WITH_SHM
    /* 如果带有机框管理功能且本板是主用板，则在启动定时器，定时监测框内单板状态 */
    if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
    {
        smss_sysctl_idle_shm_init();       /* 获得本框内各单板的当前状态 */
        cpss_timer_loop_set(SMSS_TIMER_SHM_MONITOR,  /* 起定时器定期巡检 */
                            SMSS_TIMER_SHM_MONITOR_DELAY);
    }
#endif

/* 全局板正常后向VERM纤程发单板正常指示消息 */
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL
	if(SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
	{
		 return;
	}

   if (smss_get_gcpa_upgstatus() == SMSS_UPGSTATUS_UPGRADING)
   {
  	   UINT32 ulTemp = 0;
       smss_send_to_local(SMSS_VERM_PROC, SMSS_SYSCTL_PROC_NORMAL_IND_MSG,
                         &ulTemp, sizeof(UINT32));
   } 
#endif   

    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_act_stoa_fail
* 功    能: 不是所有纤程都返回成功时进行打印输出并复位
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_act_stoa_fail(VOID)
{
    UINT32 ulCount;

    /* 输出出错的纤程及错误信息 */
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
               "       Failed ProcName           Failed No ");

    for (ulCount = 0; ulCount <  g_ulSmssSysctlProcNum; ulCount++)
    {
        if (SMSS_OK != pulProcActResult[ulCount])
        {
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                       "         %s            %d  ",
                       cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[ulCount].ulProcDes),
                       pulProcActResult[ulCount]);
        }
    }

    /* 若为主CPU，向全局板或者伙伴板报告状态 */
#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)
    {
        SMSS_SYSCTL_BFAULT_IND_MSG_T stBFault;
            /* 若为非主用全局板，则向全局板报告，延时后复位；*/
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL
        if (SMSS_STANDBY == g_stSmssAsInfo.ucSelfAStatus)
        {
            stBFault.ulType = SMSS_BFAULT_HOST_TO_GCPA;
            stBFault.ulReason = SMSS_BFAULT_INIT_FAILED;
            stBFault.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
            stBFault.ulReason = cpss_htonl(stBFault.ulReason);
            stBFault.ulType = cpss_htonl(stBFault.ulType);
            smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BFAULT_IND_MSG,
                                  (UINT8 *)&stBFault, sizeof(stBFault));

            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                       "<%s-%s>: Send %s to GCPA.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                       "SMSS_SYSCTL_BFAULT_IND_MSG");
        }
#else
        stBFault.ulType = SMSS_BFAULT_HOST_TO_GCPA;
        stBFault.ulReason = SMSS_BFAULT_INIT_FAILED;
        stBFault.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
        stBFault.ulReason = cpss_htonl(stBFault.ulReason);
        stBFault.ulType = cpss_htonl(stBFault.ulType);
        smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BFAULT_IND_MSG,
                              (UINT8 *)&stBFault, sizeof(stBFault));
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: Send %s to GCPA.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   "SMSS_SYSCTL_BFAULT_IND_MSG");
#endif

#ifdef SMSS_BACKUP_MATE
        /* 若有伙伴板，则向伙伴板报故障 */
        stBFault.ulType = SMSS_BFAULT_MATE_TO_MATE;
        stBFault.ulReason = SMSS_BFAULT_INIT_FAILED;
        stBFault.ulReason = cpss_htonl(stBFault.ulReason);
        stBFault.ulType = cpss_htonl(stBFault.ulType);
        smss_com_send_mate(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BFAULT_IND_MSG,
                           (UINT8 *)&stBFault, sizeof(stBFault));

        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: Act Proc Failed, Send BFault To Mate.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
#endif
    }

#elif (SWP_CPUROLE_SLAVE == SWP_CPUROLE_TYPE)
    /* 若为从CPU，通知主CPU */
    {
        SMSS_SYSCTL_BFAULT_IND_MSG_T stBFault;
        stBFault.ulType = SMSS_BFAULT_SLAVE_TO_HOST;
        stBFault.ulReason = SMSS_BFAULT_INIT_FAILED;
        stBFault.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
        stBFault.ulReason = cpss_htonl(stBFault.ulReason);
        stBFault.ulType = cpss_htonl(stBFault.ulType);
        smss_sysctl_send_host(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BFAULT_IND_MSG,
                              (UINT8 *)&stBFault, sizeof(stBFault));

        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: Act Proc Failed, Send BFault To Host.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
    }
#endif

    /* 设置状态，延时后复位 */
    smss_sysctl_enter_end(FALSE,
                          SMSS_BRD_RST_CAUSE_ACT_PROC_TIMEOUT_SLAVE_BRD_STOA);
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Transit to state <%s>.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, "END");

    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_send_statuschgind
* 功    能: 主CPU的主控模块向主用全局板或者从CPU向主CPU发送状态改变消息
* 函数类型  VOID
* 参    数: 无。
* 函数返回:
*           无。
* 说    明: 在初始化结束后和备升主前向主用全局板发送状态改变消息。
***************************************************************************/
VOID smss_sysctl_send_statuschgind(VOID)
{
    CPSS_COM_PID_T stPid;
    SMSS_SYSCTL_STATUS_CHG_IND_MSG_T stStaChg;
    cpss_mem_memset(&stPid, 0, sizeof(stPid));
    cpss_mem_memset(&stStaChg, 0, sizeof(stStaChg));

    g_ucSmssSendCount = g_ucSmssSendCount + 1;
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
    CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_6InCnt);
#endif

    #if 1
    if (g_ucSmssSendCount > 120)
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "<%s-%s>: Send %s to GCPA 10 times, reset self.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   "SMSS_SYSCTL_STATUS_CHG_IND_MSG");

        if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
        {
            smss_sysctl_enter_end(FALSE,
                                  SMSS_BRD_RST_CAUSE_STATUS_CHG_IND_TIMEOUT);
        }
        else
        {
            smss_sysctl_enter_end(TRUE,
                                  SMSS_BRD_RST_CAUSE_STATUS_CHG_IND_TIMEOUT);
        }
        
        return;
    }
    #endif

    stPid.stLogicAddr = CPSS_LOGIC_ADDR_GCPA;
    stPid.ulAddrFlag =  CPSS_COM_ADDRFLAG_MASTER;
    stPid.ulPd = SMSS_SYSCTL_PROC;

    if (SMSS_RSTATUS_INIT == g_stSmssBaseInfo.ucRStatus)
        /* 初始化完成后向上级CPU报告本板的状态 */
    {
        stStaChg.ucRStatus = SMSS_RSTATUS_NORMAL;
        stStaChg.ucAStatus = g_stSmssAsInfo.ucSelfAStatus;
        stStaChg.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
        stStaChg.ulReason = cpss_htonl(stStaChg.ulReason);

#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)
#if  defined(SMSS_BOARD_WITH_DSP) || defined(SMSS_BOARD_WITH_SLAVE)
        stStaChg.ucBoardState = SMSS_BRD_RSTATUS_FAULT;
        g_ucBoardState = SMSS_BRD_RSTATUS_FAULT;
#else
        stStaChg.ucBoardState = SMSS_BRD_RSTATUS_NORMAL;
        g_ucBoardState = SMSS_BRD_RSTATUS_NORMAL;      
#endif
        /* 若为主CPU，则向全局板报告 */
        smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_STATUS_CHG_IND_MSG,
                              (UINT8 *)&stStaChg, sizeof(stStaChg));

        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "<%s-%s>: Init finished, send %s to GCPA.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   "SMSS_SYSCTL_STATUS_CHG_IND_MSG");

#else
        stStaChg.ucBoardState = SMSS_BRD_RSTATUS_NORMAL;
        /* 若为从CPU，则向主CPU报告 */
        smss_sysctl_send_host(SMSS_SYSCTL_PROC, SMSS_SYSCTL_STATUS_CHG_IND_MSG,
                              (UINT8 *)&stStaChg, sizeof(stStaChg));

        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "<%s-%s>: Init finished, send %s to HOST.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   "SMSS_SYSCTL_STATUS_CHG_IND_MSG");
#endif
    }

#ifdef SMSS_BACKUP_MATE
    else if (SMSS_RSTATUS_NORMAL == g_stSmssBaseInfo.ucRStatus)
        /* 主备倒换前，外围板向全局板报告伙伴板状态要改变 */
    {
#ifndef SWP_FNBLK_BRDTYPE_MAIN_CTRL     /* 非全局板 */
            stStaChg.ucRStatus = SMSS_RSTATUS_END;
            stStaChg.ucAStatus = SMSS_STANDBY;
            stStaChg.stPhyAddr = g_stSmssAsInfo.stMatePhyAddr;
			stStaChg.ulReason = cpss_htonl(stStaChg.ulReason);
            stStaChg.ucBoardState = SMSS_BRD_RSTATUS_FAULT;
            smss_com_send(&stPid, SMSS_SYSCTL_STATUS_CHG_IND_MSG,
                          (UINT8 *)&stStaChg, sizeof(stStaChg));

            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                       "<%s-%s>: MATE RStatus changed to <END>, send %s to GCPA.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                       "SMSS_SYSCTL_STATUS_CHG_IND_MSG");
#endif
    }
#endif
	#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
	 CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_6OutCnt);
	#endif
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_recv_statuschgack
* 功    能: 收到状态改变确认是进行处理
* 函数类型  VOID
* 参    数:
* 函数返回:
*           无。
* 说    明: 该函数在初始化完成和备升主前都被调用
***************************************************************************/
VOID smss_sysctl_recv_statuschgack(CPSS_COM_MSG_HEAD_T *pstMsg)
{
    SMSS_SYSCTL_STATUS_CHG_ACK_MSG_T *pstAckMsg =
        (SMSS_SYSCTL_STATUS_CHG_ACK_MSG_T *)(pstMsg->pucBuf);
    SMSS_SYSCTL_STATUS_CHG_ACK_MSG_T stAckMsg;
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Received %s from %s.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               "SMSS_SYSCTL_STATUS_CHG_ACK_MSG",
               SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE ? "GCPA" : "HOST");

		#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
		 CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_7InCnt);
		#endif

    #if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)
    if (0 != cpss_ntohs(pstAckMsg->usResult))  /* 错误, 直接复位 */
    {
    	smss_sysctl_enter_end(TRUE,
                             SMSS_BRD_RST_CAUSE_STATUS_CHG_IND_TIMEOUT);
    	return;
    }
    #endif
    stAckMsg.ucRStatus = pstAckMsg->ucRStatus;
    stAckMsg.ucAStatus = pstAckMsg->ucAStatus;

    if( g_stSmssBaseInfo.ucRStatus == stAckMsg.ucRStatus )
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Received CurRStatus = AckRStatus %d.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               pstAckMsg->ucRStatus);
        return;
    }
    if (SMSS_RSTATUS_INIT == g_stSmssBaseInfo.ucRStatus)
        /* 初始化阶段收到该消息时进入服务态 */
    {
        CPS__OAMS_AM_ORIGIN_EVENT_T stOamsEvent;

        smss_sysctl_enter_normal();
        cpss_timer_delete(SMSS_TIMER_INIT);

#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE) /* 主CPU */
        /* 向OAMS上报“单板启动”事件 */
        stOamsEvent.ulEventNo = SMSS_EVENTNO_BOARD_STARTUP;
        stOamsEvent.ulEventSubNo = 0;
        smss_moid_get_local_moid(CPS__OAMS_MOID_TYPE_BOARD,&stOamsEvent.stEventMoId);
        stOamsEvent.ulExtraLen = 0;
        /* stOamsEvent.aucExtraInfo[0] = 0; */
#else  /* 从CPU */
        /* 向OAMS上报“单板从处理器启动”事件 */
        stOamsEvent.ulEventNo = SMSS_EVENTNO_BOARD_SCPU_STARTUP;
        stOamsEvent.ulEventSubNo = 0;
        smss_moid_get_local_moid(CPS__OAMS_MOID_TYPE_SLAVE_CPU,&stOamsEvent.stEventMoId);
        stOamsEvent.ulExtraLen = 0;
        /* stOamsEvent.aucExtraInfo[0] = 0; */
#endif
        smss_send_event_to_oams(&stOamsEvent);
    }

#ifdef SMSS_BACKUP_MATE
    else if (SMSS_RSTATUS_NORMAL == g_stSmssBaseInfo.ucRStatus)
        /* 正常运行阶段收到该改消息时转入备升主状态，进行备升主 */
    {
        cpss_timer_delete(SMSS_TIMER_STACHG_IND);

#ifdef SMSS_AS_CMP_HARD
        /* 硬件主备竞争时需要在硬件上
         * 报的主备状态变化消息中处
           理备升主流程 */
        return;
#endif
        cpss_vk_proc_user_state_set(SMSS_SYSCTL_STATE_STOA);
        g_stSmssBaseInfo.szRStatus = "STOA";
        g_stSmssBaseInfo.ucRStatus = SMSS_RSTATUS_STOA;
        smss_sysctl_send_act_stoa_req();
    }
#endif

    /* 清空计数器 */
    g_ucSmssSendCount = 0;
		#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
		 CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_7OutCnt);
		#endif

    return;
}

/***************************************************************************
* 函数名称: smss_showupperclk
* 功    能: 打印上级时钟槽位号.
* 函数类型  VOID
* 参    数:
* 参数名称      类型               输入/输出       描述
* 
* 函数返回:
*           无
* 说    明:
***************************************************************************/
VOID smss_showupperclk(VOID)
{
	#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
	 #if (SWP_PHYBRD_TYPE != SWP_PHYBRD_NSCA) 
	  #if (SWP_PHYBRD_TYPE == SWP_PHYBRD_MASA)  /* masa板 */
	    cps__oams_shcmd_printf("active nsca: %d\n",g_smss_upperclk_brdslot);
	  #else  /* 其他外围板 */
	   cps__oams_shcmd_printf("active masa: %d\n",g_smss_upperclk_brdslot); 
	  #endif
	 #endif  
	#endif
 return;	 
}

/***************************************************************************
* 函数名称: smss_sysctl_set_drv_upperclk
* 功    能: 封装函数, 设置上级时钟。
* 函数类型  VOID
* 参    数:
* 参数名称      类型               输入/输出       描述
* ucSlot        UINT8         IN         消息头结构
* 函数返回:
*           无
* 说    明:
***************************************************************************/
VOID smss_sysctl_set_drv_upperclk(UINT8 ucSlot)
{
	#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
    #if ((SWP_PHYBRD_TYPE == SWP_PHYBRD_MASA) || (SWP_PHYBRD_TYPE == SWP_PHYBRD_MNPA) || \
         (SWP_PHYBRD_TYPE == SWP_PHYBRD_MEIA) || (SWP_PHYBRD_TYPE == SWP_PHYBRD_MDPA) || \
         (SWP_PHYBRD_TYPE == SWP_PHYBRD_MAPA) || (SWP_PHYBRD_TYPE == SWP_PHYBRD_MMPA) || \
         (SWP_PHYBRD_TYPE == SWP_PHYBRD_MMPA_OEM))
        {
           drv_clk_syn_select_byslot(ucSlot);
	         g_smss_upperclk_brdslot = ucSlot;	
        }
    #endif      	  	
	#endif 
}


/***************************************************************************
* 函数名称: smss_sysctl_upperclk_ind
* 功    能: 收到更新上级时钟指示。
* 函数类型  VOID
* 参    数:
* 参数名称      类型               输入/输出       描述
* pstMsg        UINT8*         IN         消息头结构
* 函数返回:
*           无
* 说    明:
***************************************************************************/
VOID smss_sysctl_upperclk_ind(UINT8 *pucBuf)
{
	#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST) 
	 SMSS_SYSCTL_UPPERCLK_BRDSLOT_IND_MSG_T  *pInd;
	
	 pInd = (SMSS_SYSCTL_UPPERCLK_BRDSLOT_IND_MSG_T *)pucBuf;
	
	 cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: Receive SMSS_SYSCTL_UPPERCLK_IND_MSG, slot %d",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            pInd->ucClkBrdSlot);
	
	 smss_sysctl_set_drv_upperclk(pInd->ucClkBrdSlot);
 #endif
 return;
}


/***************************************************************************
* 函数名称: smss_sysctl_upperclk_rsp
* 功    能: 收到主用全局板发来的更新上级时钟响应消息。
* 函数类型  VOID
* 参    数:
* 参数名称      类型               输入/输出       描述
* pstMsg        UINT8*         IN         消息头结构
* 函数返回:
*           无
* 说    明:
***************************************************************************/
VOID smss_sysctl_upperclk_rsp(UINT8 *pucBuf)
{
	#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST) 
	  SMSS_SYSCTL_UPPERCLK_QUERY_RSP_MSG_T *pRsp;
	  
	  pRsp = (SMSS_SYSCTL_UPPERCLK_QUERY_RSP_MSG_T *)pucBuf;
	  cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: Receive SMSS_SYSCTL_UPPERCLK_QUERY_RSP_MSG, slot %d",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            pRsp->ucSlot);
	
	 smss_sysctl_set_drv_upperclk(pRsp->ucSlot);
	#endif
	return;
}

/***************************************************************************
* 函数名称: smss_sysctl_state_normal_func
* 功    能: 处理SMSS的主控在正常态需要处理的消息。
* 函数类型  VOID
* 参    数:
* 参数名称      类型               输入/输出       描述
* pstMsg CPSS_COM_MSGHEAD_T *         IN         消息头结构
* 函数返回:
*           无
* 说    明:
***************************************************************************/
VOID smss_sysctl_state_normal_func( CPSS_COM_MSG_HEAD_T *pstMsg )
{
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /*全局板上的特殊处理 */
    {
        BOOL bDealMessage;
        bDealMessage = smss_sysctl_normal_gcpa_func(pstMsg);
        if (TRUE == bDealMessage)
        {
            return;
        }
    }
#endif

#ifdef SMSS_BOARD_WITH_SHM   /*机框管理的特殊处理 */
    {
        BOOL bDealMessage;
        bDealMessage = smss_sysctl_normal_shm_func(pstMsg);
        if (TRUE == bDealMessage)
        {
            return;
        }
    }
#endif

#if defined(SMSS_BACKUP_MATE)  /*主备板上的特殊处理 */
    {
        BOOL bDealMessage;
        bDealMessage =smss_sysctl_normal_as_func(pstMsg);
        if (TRUE == bDealMessage)
        {
            return;
        }
    }
#endif

#if defined(SMSS_BOARD_WITH_DSP)||defined(SMSS_BOARD_WITH_SLAVE)
    /*带从CPU或者DSP的主CPU上的特殊处理 */
    {
        BOOL bDealMessage;
        bDealMessage = smss_sysctl_normal_slave_func(pstMsg);
        if (TRUE == bDealMessage)
        {
            return;
        }
    }
#endif

    switch(pstMsg->ulMsgId)
    {
#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE) /*主CPU上的特殊处理 */
    case SMSS_SYSCTL_GP_HEARTBEAT_IND_MSG:
        smss_sysctl_recv_gp_heartbeat((SMSS_SYSCTL_GP_HEARTBEAT_IND_T*)pstMsg->pucBuf);
        break;
#endif
        /* 以下消息为公共的消息 */
    
    case SMSS_RESET_BOARD_CMD_MSG:         /* 复位单板命令 */
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
           "<%s-%s>: Received message %s.",
           g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
           "SMSS_RESET_BOARD_CMD_MSG");
        smss_sysctl_brd_reset(((SMSS_RESET_BOARD_CMD_MSG_T *)pstMsg->pucBuf)->ulReason);
        break;

    case SMSS_DEVM_BFAULT_IND_MSG:           /* DEVM的板极故障通知 */
        smss_sysctl_recv_devmreportfault();
        break;

    case SMSS_SYSCTL_BFAULT_IND_MSG: /* 板级故障上报消息，用于全局板和主CPU */
        smss_sysctl_recv_bfaultind((SMSS_SYSCTL_BFAULT_IND_MSG_T *)pstMsg->pucBuf);
        break;

    case SMSS_TIMER_PERIODLY_HEARTBEAT_MSG:  /* 用于心跳的超时消息 */
        smss_sysctl_send_periodly_heartbeat();
        break;
    case SMSS_SYSCTL_LOST_GCPAHB_MSG:
        /*主CPU丢失全局板的心跳,系统主控需要做的操作*/
        smss_sysctl_lost_gcpahb_handle();
        break;
    case SMSS_CPU_FAULT_IND_MSG:
        smss_sysctl_cpufaultind_msgdisp(pstMsg);
        break;
#ifdef SMSS_BOARD_WITHOUT_RDBS
    case SMSS_BOARD_INFO_QUERY_REQ_MSG: 
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: Receive SMSS_BOARD_INFO_QUERY_REQ_MSG from Gcpa!!!.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus); 
        smss_sysctl_board_info_query_disp();
        break;
#endif
    case SMSS_SYSCTL_UPPERCLK_IND_MSG:     /* 更新上级时钟指示 */
      smss_sysctl_upperclk_ind(pstMsg->pucBuf);      
     break;
     
    case SMSS_SYSCTL_UPPERCLK_QUERY_RSP_MSG:   /* 收到查询上级时钟响应 */
      smss_sysctl_upperclk_rsp(pstMsg->pucBuf); 
     break;
     
    #if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)
    case SMSS_SYSCTL_CPULOADINFO_REQ_MSG:  /* 主CPU收到从CPU负荷控制请求 */
      smss_sysctl_slavecpuload_req(pstMsg->pucBuf);
     break;
    
    case SMSS_SYSCTL_CPULOADINFO_RSP_MSG: /* 主CPU收到负荷控制响应消息 */ 
     smss_sysctl_cpuloadinfo_rsp(pstMsg->pucBuf); 
     break;
    
    case SMSS_SYSCTL_BOARDLOADINFO_RSP_MSG:  /* 主CPU收到单板负荷控制响应消息 */
     smss_sysctl_boardloadinfo_rsp(pstMsg->pucBuf);
     break;
    
    case CPS__RDBS_DEV_CPULOADCTRLCHG_IND_MSG:  /* 本板CPU变更指示消息 */
      smss_sysctl_cpuloadchg_ind(pstMsg->pucBuf);
     break;
     
    case CPS__RDBS_DEV_BOARDLOADCTRLCHG_IND_MSG:  /* 本板单板变更指示消息 */
      smss_sysctl_boardloadchg_ind(pstMsg->pucBuf);
     break; 
     
    case SMSS_SYSCTL_SLAVECPU_IND_MSG:  /* 收到从CPU占用率指示消息 */
      smss_sysctl_recv_slavecpu_ind(pstMsg->pucBuf); 
     break;     
    #endif
    
    #if (SWP_CPUROLE_SLAVE == SWP_CPUROLE_TYPE)
     case SMSS_SYSCTL_CPULOADINFO_RSP_MSG:   /* 从CPU收到负荷控制响应消息 */
      smss_sysctl_slavecpuloadinfo_rsp(pstMsg->pucBuf); 
     break;
    #endif
#if 0 //dhwang added
     case OM_PM_STATISTIC_REQ_MSG:
    	 cps_om_phy_para(pstMsg->pucBuf);
    	 break;
#endif
    default:                                 /* 收到无效消息时进行告警 */
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: Received unknown message 0x%08X.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, pstMsg->ulMsgId);
        break;
    }
}

/***************************************************************************
* 函数名称: smss_sysctl_recv_regreq
* 功    能: 全局板或主CPU收到注册请求时进行处理
* 函数类型  VOID
* 参    数:
* 参数名称        类型               输入/输出       描述
* pstMsg  SMSS_SYSCTL_REG_REQ_MSG_T *   IN            注册信息
* 函数返回:
*           无。
* 说    明: 该消息处理函数应用于全局板和主CPU
***************************************************************************/
VOID smss_sysctl_recv_regreq(SMSS_SYSCTL_REG_REQ_MSG_T *pstMsg)
{
    if (NULL == pstMsg) /* 消息合法性检验 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
        "<%s-%s>: smss_sysctl_recv_regreq() error: Received NULL message.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        return;
    }

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
    /* 外围板向全局板进行注册 */
    smss_sysctl_gcpa_recv_regreq(pstMsg);

#elif defined(SMSS_BOARD_WITH_DSP) || defined(SMSS_BOARD_WITH_SLAVE)
    /* DSP或者从CPU向主CPU进行注册 */
    smss_sysctl_host_recv_regreq(pstMsg);
#endif
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_recv_gp_heartbeat
* 功    能: 处理全局板和外围板间的心跳消息
* 函数类型  VOID
* 参    数:
* 参数名称          参数类型                    输入/输出      参数描述
* pstMsg    SMSS_SYSCTL_GP_HEARTBEAT_IND_MSG_T*   IN         心跳消息指针
* 函数返回:
*           无。
* 说    明: 该函数只处理全局板和外围板间的心跳消息
***************************************************************************/
VOID smss_sysctl_recv_gp_heartbeat(SMSS_SYSCTL_GP_HEARTBEAT_IND_T *pstMsg)
{
    if (NULL == pstMsg) /* 消息合法性检验 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
        "<%s-%s>: In Function smss_sysctl_recv_gp_heartbeat,"
        "Received NULL Message.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        return;
    }

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
    /* 若为全局板，对外围板进行处理 */
    {
        smss_sysctl_gcpa_recv_gp_heartbeat(pstMsg);
    }

#else /* 若为单板，置全局板的心跳计数为0 */
    {
        smss_sysctl_record_heart(&g_stSmssBaseInfo.stHeartInfo,SMSS_RECV_HEART,cpss_ntohl(pstMsg->ulSeqNum));
        g_stSmssBaseInfo.ulBeatCount = 0;
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
            "<%s-%s>: Received heartbeat from GCPA.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
    }
#endif
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_recv_bfaultind
* 功    能: SMSS主控发收到处理器的严重故障告警消息时进行相应处理
* 函数类型  VOID
* 参    数:
* 参数名称        类型                输入/输出       描述
* pstMsg  SMSS_SYSCTL_FAULT_IND_T *IN         告警信息指针
* 函数返回:
*           无
* 说    明: 该函数处理上下级、主备板间及主从CPU间的严重故障告警
***************************************************************************/
VOID smss_sysctl_recv_bfaultind(SMSS_SYSCTL_BFAULT_IND_MSG_T *pstMsg)
{
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "<%s-%s>: Received message %s.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
        "SMSS_SYSCTL_BFAULT_IND_MSG");

    if (NULL == pstMsg) /* 消息合法性检验 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
        "<%s-%s>: In Function smss_sysctl_recv_bfaultind,"
        "Received NULL Message.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        return;
    }

    pstMsg->ulType = cpss_htonl(pstMsg->ulType);
    pstMsg->ulReason = cpss_htonl(pstMsg->ulReason);

    if (SMSS_BFAULT_SLAVE_TO_HOST == pstMsg->ulType)   /* 从CPU向主CPU报告 */
    {
#ifdef SMSS_BOARD_WITH_DSP                      /* DSP 故障 */
        #if 0
        smss_sysctl_alarm_sendto_oams(SMSS_ALMID_BOARD_DSP_FAULTY,
            SMSS_ALMSUBNO_3, CPS__OAMS_ALARM_OCCUR, pstMsg->stPhyAddr.ucCpu, 0, 0);
        #endif 

        if (pstMsg->stPhyAddr.ucCpu >= SMSS_CPUNO_1ST_DSP) 
        {
            g_ucDspAlarmFlag[SMSS_ALMSUBNO_3][pstMsg->stPhyAddr.ucCpu - SMSS_CPUNO_1ST_DSP] = 1;
        }

        smss_sysctl_host_recv_dsp_bfault(pstMsg);
#elif defined(SMSS_BOARD_WITH_SLAVE)            /* 从CPU故障 */
  
        #if 0
        smss_sysctl_alarm_sendto_oams(SMSS_ALMID_BOARD_SCPU_FAULTY,
            SMSS_ALMSUBNO_3, CPS__OAMS_ALARM_OCCUR, pstMsg->stPhyAddr.ucCpu, 0, 0);
        #endif

        g_ucSlaveAlarmFlag[SMSS_ALMSUBNO_3] = 1;
        smss_sysctl_host_recv_slave_bfault(pstMsg);
#endif
    }

    else if (SMSS_BFAULT_HOST_TO_GCPA == pstMsg->ulType)
        /* 外围板发向全局板报告板级故障 */
    {
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
        /* 向OAMS上报告警 */
        smss_send_alarm_brdnotresponded_to_oams(SMSS_ALMSUBNO_1, 1, pstMsg->stPhyAddr);
        smss_sysctl_sendbrdalarmind_tosysrt(pstMsg->stPhyAddr);
        smss_sysctl_gcpa_recv_bfault(pstMsg);
#endif
    }

#ifdef SMSS_BACKUP_MATE
    else if (SMSS_BFAULT_MATE_TO_MATE == pstMsg->ulType)
        /* 收到伙伴板发来的板极故障，修改自己保存的状态 */
    {
        if (SMSS_RSTATUS_NORMAL == g_stSmssAsInfo.ucMateRStatus)
        {
            cpss_timer_delete(SMSS_TIMER_MATE_HEARTBEAT);
        }
        g_stSmssAsInfo.ucMateRStatus = SMSS_RSTATUS_END;
        g_stSmssAsInfo.ucMateBeatCount = 0;
    }
#endif
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_send_periodly_heartbeat
* 功    能: 周期心跳定时器所触发的心跳处理。
* 函数类型  VOID
* 参    数: 无。
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_send_periodly_heartbeat(VOID)
{
    if (FALSE == g_bRunGpHeartBeat)  /* 判断是否要进行全局板和外围板间的心跳 */
    {
        return;
    }
    CPSS_DSP_FILE_REC();
    CPSS_DSP_LINE_REC();
    CPSS_DSP_RUN_ADDR_REC(CPSS_DSP_SMSS_BEAT_START);

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
           "<%s-%s>: Heartbeat timer.",
           g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
    if (FALSE != g_bShowHeartbeatList)
    {
        smss_show_hblist();
    }

    /* 主用全局板的心跳处理 */
    if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus
        ||SMSS_RSTATUS_STOA == g_stSmssBaseInfo.ucRStatus)
    {
         smss_sysctl_gcpa_send_heartbeat();
    }
#else /* 外围板 */
#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE) /* 主CPU */
        smss_sysctl_hostcpu_send_periodly_heartbeat();
#else /* 从CPU */
        smss_sysctl_slavecpu_send_periodly_heartbeat();
#endif
#endif
 
    CPSS_DSP_LINE_REC();
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_set_cps__rdbs_astatus()
* 功    能: 在RDBS内设置指定CPU的主备状态
* 函数类型  UINT32
* 参    数:
* 参数名称      参数类型       输入/输出        参数描述
* stPhy    CPSS_COM_PHY_ADDR_T    IN          CPU的物理地址
* ucValue       UINT8             IN           要设置的值
* 函数返回:
*           RDBS的操作返回值
* 说    明: 该函数是对RDBS函数的封装。
***************************************************************************/
UINT32 smss_sysctl_set_cps__rdbs_astatus(CPSS_COM_PHY_ADDR_T stPhy, UINT8 ucValue)
{
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */

    CPS__RDBS_DEV_SETCPUSTATUS_MSG_REQ_T stMsg;
    CPS__RDBS_DEV_SETCPUSTATUS_MSG_RSP_T stRspMsg;

    stMsg.stCpuPhyAddr = stPhy;
    stMsg.ucMode = CPS__RDBS_DEV_MODE_CPU_MASTERORSLAVE;

    /* 进行SMSS与RDBS的转换 */
    if (SMSS_ACTIVE == ucValue)
    {
        stMsg.ucValue = 0;      /* 主 */
    }
    else
    {
        stMsg.ucValue = 1;      /* 备 */
    }

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: In RDBS, set (%d-%d-%d) AStatus to <%s>.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               stMsg.stCpuPhyAddr.ucFrame, stMsg.stCpuPhyAddr.ucShelf,
               stMsg.stCpuPhyAddr.ucSlot,
               SMSS_ACTIVE == ucValue ? "ACTIVE" : "STANDBY");

    /* 设置RDBS中主备状态 */
    cps__rdbs_if_entry(CPS__RDBS_DEV_SETCPUSTATUS_MSG, CPS__RDBS_IF_FUNC,
                  (UINT8 *)&stMsg, sizeof(stMsg),
                  (UINT8 *)&stRspMsg, sizeof(stRspMsg));
    smss_sysctrl_set_smss_astatus(stPhy, ucValue);
    return stRspMsg.ulResult;
#else
    return SMSS_ERROR;
#endif
}

/***************************************************************************
* 函数名称: smss_sysctl_set_cps__rdbs_rstatus()
* 功    能: 在RDBS内设置指定CPU的运行状态
* 函数类型  UINT32
* 参    数:
* 参数名称      参数类型       输入/输出        参数描述
* stPhy    CPSS_COM_PHY_ADDR_T    IN          CPU的物理地址
* ucValue       UINT8             IN           要设置的值
* 函数返回:
*           RDBS的操作返回值
* 说    明: 该函数是对RDBS函数的封装。
***************************************************************************/
UINT32 smss_sysctl_set_cps__rdbs_rstatus(CPSS_COM_PHY_ADDR_T stPhy, UINT8 ucValue)
{
#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)
    CPS__RDBS_DEV_SETCPUSTATUS_MSG_REQ_T stMsg;
    CPS__RDBS_DEV_SETCPUSTATUS_MSG_RSP_T stRspMsg;

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: In RDBS, set board (%d-%d-%d-%d) RStatus to <%s>.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               stPhy.ucFrame, stPhy.ucShelf, stPhy.ucSlot, stPhy.ucCpu,
               SMSS_RSTATUS_NORMAL == ucValue ? "NORMAL" : "FAULT");
#else  /* 外围板 */
#ifdef SMSS_BOARD_WITH_SLAVE
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: In RDBS, set SLAVE (%d-%d-%d-%d) RStatus to <%s>.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               stPhy.ucFrame, stPhy.ucShelf, stPhy.ucSlot, stPhy.ucCpu,
               SMSS_RSTATUS_NORMAL == ucValue ? "NORMAL" : "FAULT");
#elif defined(SMSS_BOARD_WITH_DSP)
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: In RDBS, set DSP %d (%d-%d-%d-%d)RStatus to <%s>.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               stPhy.ucCpu - 5,
               stPhy.ucFrame, stPhy.ucShelf, stPhy.ucSlot, stPhy.ucCpu,
               SMSS_RSTATUS_NORMAL == ucValue ? "NORMAL" : "FAULT");
#endif
#endif
    /* 主CPU运行状态为故障,这时置单板版本状态为故障 */
    if ((SMSS_RSTATUS_END == ucValue) && (1 == stPhy.ucCpu))
    {
    	  #if 0
        /* 设置单板版本状态为故障态"SMSS_VERM_BOARD_VERSTATE_FAULT"*/
        smss_verm_set_gcpa_cps__rdbs_verstate(stPhy, SMSS_VERM_BOARD_VERSTATE_FAULT);
        #endif
    }

    stMsg.stCpuPhyAddr = stPhy;
    stMsg.ucMode = CPS__RDBS_DEV_MODE_CPU_HWFAULT;

    stRspMsg.ulResult = SMSS_ERROR;

    /* 进行SMSS与RDBS的转换 */
    if (SMSS_RSTATUS_NORMAL == ucValue)
    {
        stMsg.ucValue = 0;      /* 正常 */
    }
    else
    {
        stMsg.ucValue = 1;      /* 故障 */
    }

#ifndef SMSS_BOARD_WITHOUT_RDBS
    /* 设置RDBS中运行状态 */
    cps__rdbs_if_entry(CPS__RDBS_DEV_SETCPUSTATUS_MSG, CPS__RDBS_IF_FUNC,
                  (UINT8 *)&stMsg, sizeof(stMsg),
                  (UINT8 *)&stRspMsg, sizeof(stRspMsg));
#endif

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
    smss_sysctrl_set_smss_rstatus(stPhy, ucValue);
#endif
    return stRspMsg.ulResult;
#else
    return SMSS_ERROR;
#endif
}

/***************************************************************************
* 函数名称: smss_sysctl_query_cps__rdbs_astatus()
* 功    能: 查询RDBS内指定CPU的主备状态
* 函数类型  INT32
* 参    数:
* 参数名称      参数类型       输入/输出        参数描述
* stPhy    CPSS_COM_PHY_ADDR_T    IN          CPU的物理地址
* pucRStatus     UINT8 *          OUT         CPU运行状态(故障，正常)
* pucMStatus     UINT8 *          OUT         CPU管理状态(闭塞/未闭塞，延时闭塞/未延时闭塞)
* pucAStatus     UINT8 *          OUT         主备状态(闭塞/未闭塞，延时闭塞/未延时闭塞)
* 函数返回:
*           SMSS_OK:查询成功 SMSS_ERROR:查询失败
* 说    明: 该函数是对RDBS函数的封装。
***************************************************************************/
INT32 smss_sysctl_query_cps__rdbs_status(CPSS_COM_PHY_ADDR_T stPhy,
                                    UINT8 *pucRStatus,
                                    UINT8 *pucMStatus,
                                    UINT8 *pucAStatus )
{
	#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)
    CPS__RDBS_DEV_GETCPUSTATUS_MSG_REQ_T stReqMsg;
    CPS__RDBS_DEV_GETCPUSTATUS_MSG_RSP_T stRspMsg;
  #endif


#ifdef CPSS_VOS_LINUX
	*pucRStatus = SMSS_RSTATUS_NORMAL;
	*pucAStatus = SMSS_ACTIVE;
	*pucMStatus = SMSS_MSTATUS_NORMAL;

	return 0;
#endif

#ifndef SMSS_BOARD_WITHOUT_RDBS

#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)
    stReqMsg.stCpuPhyAddr = stPhy;
    cps__rdbs_if_entry(CPS__RDBS_DEV_GETCPUSTATUS_MSG, CPS__RDBS_IF_FUNC, (UINT8 *)&stReqMsg,
        sizeof(stReqMsg), (UINT8 *)&stRspMsg, sizeof(stRspMsg));

    if (0 == stRspMsg.ulResult)  /* 调用rdbs成功 */
    {
        /* 获取主备状态 */
        if (0 == (stRspMsg.usStatus & 0x8))
        {
            *pucAStatus = SMSS_ACTIVE;
        }
        else
        {
            *pucAStatus = SMSS_STANDBY;
        }

        /* 获取管理状态 */
        if (0 == (stRspMsg.usMStatus & 0x3))
        {
            *pucMStatus = SMSS_MSTATUS_NORMAL;
        }
        else if (1 == (stRspMsg.usMStatus & 0x3))
        {
            *pucMStatus = SMSS_MSTATUS_BLOCK;
        }
        else if (2 == (stRspMsg.usMStatus & 0x3))
        {
            *pucMStatus = SMSS_MSTATUS_DBLOCK;
        }

        /* 获取运行状态 */
        if (0 == (stRspMsg.usStatus & 0x1))
        {
            *pucRStatus = SMSS_RSTATUS_NORMAL;
        }
        else
        {
            *pucRStatus = SMSS_RSTATUS_END;
        }
        return SMSS_OK;
    }
    else
    {
        return SMSS_ERROR;
    }
#else
    return SMSS_ERROR;
#endif

#endif
    return SMSS_ERROR;
}

#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE) /* 主CPU */
/***************************************************************************
* 函数名称: smss_sysctl_mo2phy
* 功    能: 由管理对象信息转换为物理地址
* 函数类型  UINT32
* 参    数:
* 参数名称           参数类型               输入/输出               参数描述
* pstMoId            SWP_MO_ID_T*              IN                  管理对象信息
* pstPhy            CPSS_COM_PHY_ADDR_T*      OUT                  物理地址
* 函数返回:
*           SMSS_OK:成功 SMSS_ERROR:失败
* 说    明:
***************************************************************************/
INT32 smss_sysctl_mo2phy(SWP_MO_ID_T  *pstMoId, CPSS_COM_PHY_ADDR_T *pstPhy)
{
    if (NULL == pstMoId || NULL == pstPhy)
    {
        return SMSS_ERROR;
    }

    if (SWP_MOC_DEV_BOARD == pstMoId->ulMOC)
    {
        pstPhy->ucFrame = pstMoId->unMOI.stBoard.ucFrameNo;
        pstPhy->ucShelf = pstMoId->unMOI.stBoard.ucShelfNo;
        pstPhy->ucSlot = pstMoId->unMOI.stBoard.ucSlotNo;
        pstPhy->ucCpu = 1;
        return SMSS_OK;
    }
    else if (SWP_MOC_DEV_CPU == pstMoId->ulMOC)
    {
        *(UINT32 *)pstPhy = *(UINT32 *)&pstMoId->unMOI.stCpu;
        return SMSS_OK;
    }
    else
    {
        return SMSS_ERROR;
    }
}

/***************************************************************************
* 函数名称: smss_sysctl_mo2logic
* 功    能: 根据管理对象信息获取逻辑地址
* 函数类型  INT32
* 参    数:
* 参数名称      参数类型       输入/输出        参数描述
* pstMo    SWP_MO_ID_T *       IN            管理对象信息
* pstLogic CPSS_COM_LOGIC_ADDR_T*IN     存放逻辑地址的指针
* 函数返回:
*           SMSS_OK:转换成功  SMSS_ERROR:转换失败
* 说    明: (该函数为调试编写，入库时需要进行改动)
***************************************************************************/
INT32 smss_sysctl_mo2logic(SWP_MO_ID_T *pstMo, CPSS_COM_LOGIC_ADDR_T *pstLogic)
{
#ifndef SMSS_BOARD_WITHOUT_RDBS
    CPSS_COM_PHY_ADDR_T stPhy;
    CPS__RDBS_DEV_GETLOGICADDR_MSG_REQ_T stReqMsg;
    CPS__RDBS_DEV_GETLOGICADDR_MSG_RSP_T stRspMsg;

    if (NULL == pstMo || NULL == pstLogic)
    {
        return SMSS_ERROR;
    }

    if (SMSS_ERROR == smss_sysctl_mo2phy(pstMo, &stPhy))
    {
        return SMSS_ERROR;
    }

    stReqMsg.stCpuPhyAddr = stPhy;

    cps__rdbs_if_entry(CPS__RDBS_DEV_GETLOGICADDR_MSG, CPS__RDBS_IF_FUNC,
                  (UINT8 *)&stReqMsg, sizeof(stReqMsg),
                  (UINT8 *)&stRspMsg, sizeof(stRspMsg)
                  );
    if (CPS__RDBS_RESULT_OK != stRspMsg.ulResult)
    {
        return SMSS_ERROR;
    }
    else
    {
        *pstLogic = stRspMsg.stCpuLogicAddr;
        return SMSS_OK;
    }
#else
    return SMSS_ERROR;
#endif
}
#endif


/***************************************************************************
* 函数名称: smss_sysctl_send_gcpa
* 功    能: 向主用全局板的某纤程发消息
* 函数类型  VOID
* 参    数:
* 参数名称               参数类型           输入/输出    参数描述
* ulPd                    UINT32               IN         纤程号
* ulMsgId                 UINT32               IN         消息号
* pucMsg                  UINT8 *              IN         消息体指针
* usMl                    UINT16               IN         消息长度
* 函数返回:
*           无。
* 说    明: 该操作只能在主CPU上存在。
***************************************************************************/
VOID smss_sysctl_send_gcpa(UINT32 ulPd,
                           UINT32 ulMsgId,
                           UINT8 *pucMsg,
                           UINT32 ulMl)
{
    if (   ((NULL == pucMsg)&&(0 != ulMl))
        ||((NULL != pucMsg)&&(0 == ulMl))
        )
    {
        return;
    }

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL  /* 全局板 */
    if (SMSS_STANDBY == g_stSmssAsInfo.ucSelfAStatus)
    {
        /* 若自己为备用全局板，则用smss_com_send_mate发给主用全局板 */
        smss_com_send_mate(ulPd, ulMsgId, pucMsg, ulMl);
    }
    else
    {
        CPSS_COM_PID_T stPid;
        stPid.stLogicAddr = CPSS_LOGIC_ADDR_GCPA;
        stPid.ulAddrFlag  = CPSS_COM_ADDRFLAG_MASTER;
        stPid.ulPd        = ulPd;
        smss_com_send(&stPid, ulMsgId, pucMsg, ulMl);
    }
#else  /* 非全局板 */
    {
        CPSS_COM_PID_T stPid;
        stPid.stLogicAddr = CPSS_LOGIC_ADDR_GCPA;
        stPid.ulAddrFlag  = CPSS_COM_ADDRFLAG_MASTER;
        stPid.ulPd        = ulPd;
        smss_com_send(&stPid, ulMsgId, pucMsg, ulMl);
    }
#endif
    return ;
}

/***************************************************************************
* 函数名称: smss_sysctl_send_host
* 功    能: 向主CPU的某纤程发消息
* 函数类型  VOID
* 参    数:
* 参数名称               参数类型           输入/输出    参数描述
* ulPd                    UINT32               IN         纤程号
* ulMsgId                 UINT32               IN         消息号
* pucMsg                  UINT8 *              IN         消息体指针
* usMl                    UINT16               IN         消息长度
* 函数返回:
*           无。
* 说    明: 该操作只能在从CPU或DSP上存在。
***************************************************************************/
VOID smss_sysctl_send_host(UINT32 ulPd,
                           UINT32 ulMsgId,
                           UINT8 *pucMsg,
                           UINT32 ulMl)
{
    CPSS_COM_PHY_ADDR_T stPhy;

    if (  ((NULL == pucMsg)&&(0 != ulMl)) /* 消息合法性判断 */
        ||((NULL != pucMsg)&&(0 == ulMl))
       )
    {
        return;
    }
    stPhy = g_stSmssBaseInfo.stPhyAddr;
    stPhy.ucCpu = 1;
    cpss_com_send_phy(stPhy, ulPd, ulMsgId, pucMsg, ulMl);
    smss_test_PrintMsgStream(ulMsgId,pucMsg,ulMl,TRUE);
    return ;
}

/***************************************************************************
* 函数名称: smss_sysctl_state_init_timeout
* 功    能: 对SMSS的SYSCTL控制初始化阶段的超时消息进行处理
* 函数类型  VOID
* 参    数: 无。
* 函数返回:
*           无。
* 说    明: 在激活单个纤程超时时，可以通过超时参数确定是那个纤程超时，也可以
*           认为是当前同步激活的纤程超时。
***************************************************************************/
VOID smss_sysctl_state_init_timeout(VOID)
{
    UINT32 ulProcDes = g_pstSmssSysctlProcTable[g_ulSmssSendIndex-1].ulProcDes;

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL
    UINT32 ulIndex = smss_sysctl_procdes2index(ulProcDes);
    /* begin of for GCPA 初始化监控修改 */
    pulProcActRsp[ulIndex] = SMSS_BRD_RST_CAUSE_ACT_PROC_TIMEOUT_INIT;
    /* begin of for GCPA 初始化监控修改 */
    /* 在网元升级过程中，备用全局板备升主失败，即收到备升主失败响应消息 */
	if ((SMSS_STANDBY == g_stSmssAsInfo.ucSelfAStatus) && (SMSS_RSTATUS_STOA == g_stSmssBaseInfo.ucRStatus)
    	  && (SMSS_UPGSTATUS_UPGRADING == g_NeUpgTagSmss))
    {
    	if (CPSS_TD_INVALID != pulProcActResult[ulIndex])   /* 如果设置有定时器，则删除定时器 */
        {
			cpss_timer_para_delete(pulProcActResult[ulIndex]);
			pulProcActResult[ulIndex] = CPSS_TD_INVALID;
        }
            
        cps__oams_shcmd_printf("\nProc '%s' Proc activete fail, \n",
                               cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[ulIndex].ulProcDes)); 
			
	    /* 置状态为正常 */
	  	smss_verm_set_upgstatus(SMSS_UPGSTATUS_NORMAL);
	  	g_NeUpgTagSmss = SMSS_UPGSTATUS_NORMAL;
            
	    /* 向主用全局板发送整网元升级失败指示 */
		smss_com_send_mate(SMSS_SYSCTL_PROC, SMSS_SYSCTL_GCPA_UPGRADE_FAIL_IND_MSG,
                               NULL, 0);
        
		smss_sysctl_actproc_stoarsp_event(
                SMSS_STANDBY_TO_ACTIVE_RSP_MSG,
                ulProcDes,
                SMSS_BRD_RST_CAUSE_ACT_PROC_TIMEOUT_INIT);

		/* 设置定时器，准备复位单板 */
		cpss_timer_set(SMSS_TIMER_STANDBY_RESET_IND,SMSS_TIMER_STANDBY_RESET_DELAY);
        return;
    }

	if ((SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus) && (SMSS_RSTATUS_INIT == g_stSmssBaseInfo.ucRStatus)
    	   && (SMSS_UPGSTATUS_UPGRADING == g_NeUpgTagSmss))
	{
		/* 置状态为正常 */
		smss_verm_set_upgstatus(SMSS_UPGSTATUS_NORMAL);
		g_NeUpgTagSmss = SMSS_UPGSTATUS_NORMAL;
	}
#endif

    switch(ucActProcStage)
    {
    case SMSS_SEND_PROC: /* 激活某个纤程超时 */
       cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                   "<%s-%s>: Activating Proc `%s' timeout.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[g_ulSmssSendIndex-1].ulProcDes));
       smss_sysctl_write_sbbr("<%s-%s>: Activating Proc `%s' timeout.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[g_ulSmssSendIndex-1].ulProcDes));

       g_szInitFailProcName = cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[g_ulSmssSendIndex-1].ulProcDes);

        if (SMSS_RSTATUS_INIT == g_stSmssBaseInfo.ucRStatus)
        {
            smss_sysctl_actproc_stoarsp_event(
                SMSS_PROC_ACTIVATE_RSP_MSG,
                ulProcDes,
                SMSS_BRD_RST_CAUSE_ACT_PROC_TIMEOUT_INIT);
        }
        else if (SMSS_RSTATUS_STOA == g_stSmssBaseInfo.ucRStatus)
        {
            smss_sysctl_actproc_stoarsp_event(
                SMSS_STANDBY_TO_ACTIVE_RSP_MSG,
                ulProcDes,
                SMSS_BRD_RST_CAUSE_ACT_PROC_TIMEOUT_INIT);
        }
        /* 设置状态，延时后复位 */
        smss_sysctl_enter_end(TRUE,
                              SMSS_BRD_RST_CAUSE_ACT_PROC_TIMEOUT_INIT);
        break;

    case SMSS_SEND_ALL:  /* 激活所有纤程超时 */
        smss_sysctl_act_stoa_fail();
        break;

    case SMSS_SEND_OK:   /* 等待状态改变应答超时 */
        smss_sysctl_send_statuschgind();
        break;

    default:             /* 收到无效消息，进行打印并告警 */
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: Enter Unknown Stage...  ",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        break;
    }
}

/***************************************************************************
* 函数名称: smss_sysctl_init
* 功    能: SMSS的主控模块被激活后进行必要的初始化。
* 函数类型  VOID
* 参    数: 无。
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_init(VOID)
{
    /* 获取本板相关信息 */
    /*cpss_mem_memset(&g_stSmssBaseInfo, 0, sizeof(g_stSmssBaseInfo));*/
    cpss_com_phy_addr_get(&g_stSmssBaseInfo.stPhyAddr);
    g_stSmssBaseInfo.ucRStatus = SMSS_RSTATUS_IDLE;
    g_stSmssBaseInfo.szRStatus = "IDLE";

    /* 开辟用于存储纤程初始化结果的动态空间 */
    pulProcActResult = cpss_mem_malloc(g_ulSmssSysctlProcNum * sizeof(UINT32));

    if (NULL == pulProcActResult) /* 开辟空间失败时，直接复位 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                   "<%s-%s>: Malloc memory failed, Enter End.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        smss_sysctl_enter_end(TRUE,
                              SMSS_BRD_RST_CAUSE_ALLOC_MEM_FAILURE_INIT);
        return;
    }

    cpss_mem_memset(pulProcActResult, 0xFF, g_ulSmssSysctlProcNum*sizeof(UINT32));

    /* begin of for GCPA 初始化监控修改 */
    /* 在全局板上存储纤程激活结果的 */
    pulProcActRsp = cpss_mem_malloc(g_ulSmssSysctlProcNum * sizeof(UINT32));

    if (NULL == pulProcActResult) /* 开辟空间失败时，直接复位 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                   "<%s-%s>: pulProcActRsp Malloc memory failed, Enter End.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        smss_sysctl_enter_end(TRUE,
                              SMSS_BRD_RST_CAUSE_ALLOC_MEM_FAILURE_INIT);
        return;
    }
    cpss_mem_memset(pulProcActRsp, 0, g_ulSmssSysctlProcNum*sizeof(UINT32));
    /* end of for GCPA 初始化监控修改 */

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
    smss_sysctl_idle_gcpa_init();
#endif

#ifdef SMSS_BACKUP_MATE /* 伙伴板相关初始化 */
    smss_sysctl_idle_mate_init();
#endif

#if defined(SMSS_BOARD_WITH_SLAVE)||defined(SMSS_BOARD_WITH_DSP)
    /* DSP/从CPU相关的初始化 */
    smss_sysctl_idle_slave_init();
#endif

    /*CPU占用率初始化*/
#if 0
    smss_sysctl_cpu_usage_init();
#endif
    cpss_mem_memset(g_ucDspAlarmFlag,0,SMSS_SCPU_DSP_ALARM_SUBNO_SUM*SMSS_DSP_MAX_SUM);
    cpss_mem_memset(g_ucSlaveAlarmFlag,0,SMSS_SCPU_DSP_ALARM_SUBNO_SUM);

    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_state_end_func
* 功    能: 处理处理器故障状态下的消息，主要包括用于复位的定时器消息。
* 函数类型  VOID
* 参    数:
* 参数名称       类型         输入/输出       描述
* pstMsg CPSS_COM_MSGHEAD_T *    IN         消息头结构
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_state_end_func(CPSS_COM_MSG_HEAD_T *pstMsg )
{
    switch(pstMsg->ulMsgId)
    {
    case SMSS_TIMER_END_MSG:  /* 收到复位定时器消息，进行相应处理 */
        smss_sysctl_state_end_reset();
        break;

    default:
        break;
    }
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_state_end_reset
* 功    能: 在END态下收到超时消息后，释放空间资源，并复位自己
* 函数类型  VOID
* 参    数: 无。
* 函数返回:
*           无。
* 说    明: DSP或者从CPU不复位自己
***************************************************************************/
VOID smss_sysctl_state_end_reset(VOID)
{
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "<%s-%s>: Received message %s.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
        "SMSS_TIMER_END_MSG");

#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
    cpss_reset(g_stSmssBaseInfo.ulResetReason);
#endif

    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_hostcpu_send_periodly_heartbeat
* 功    能: 主CPU进行心跳处理
* 函数类型  VOID
* 参    数: 无。
* 函数返回: 无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_hostcpu_send_periodly_heartbeat(VOID)
{
    SMSS_SYSCTL_GP_HEARTBEAT_IND_T stHeartBeat;

#if defined(SMSS_BOARD_WITH_DSP)
    /* 对DSP进行心跳监测*/
    smss_sysctl_host_heartbeat_dsp();

#elif defined(SMSS_BOARD_WITH_SLAVE)
    /* 对从CPU进行心跳测试 */
    smss_sysctl_host_heartbeat_slave();
#endif

    g_stSmssBaseInfo.ulBeatCount = g_stSmssBaseInfo.ulBeatCount + 1;
    if (g_stSmssBaseInfo.ulBeatCount > 2)
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: Lost GCPA heartbeat %d.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   g_stSmssBaseInfo.ulBeatCount);
    }

    if (g_stSmssBaseInfo.ulBeatCount >= 5)
        /*发现主用全局板心跳缺失时复位自己 */
    { 
         /* 设置自己的状态，复位 */
        smss_sysctl_enter_end(TRUE,
            SMSS_BRD_RST_CAUSE_LOST_HEARTBEAT_GCPA);

        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: Lost GCPA heartbeat, transit to state <%s>.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, "END");
    }

    else /* 正常情况下，向全局板发心跳消息 */
    {
        stHeartBeat.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
        stHeartBeat.ulSeqNum = cpss_htonl(g_stSmssBaseInfo.ulHeartBeatSndSeq);
        smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_GP_HEARTBEAT_IND_MSG,
                              (UINT8 *)&stHeartBeat, sizeof(stHeartBeat));

        smss_sysctl_record_heart(&g_stSmssBaseInfo.stHeartInfo,SMSS_SEND_HEART,g_stSmssBaseInfo.ulHeartBeatSndSeq);
        g_stSmssBaseInfo.ulHeartBeatSndSeq += 1;

		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                       "<%s-%s>: g_stSmssBaseInfo.ulSndHeartSeq = %d.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                       g_stSmssBaseInfo.ulHeartBeatSndSeq);

        if (g_stSmssBaseInfo.ulBeatCount > 2)
        {
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
                       "<%s-%s>: Send heartbeat to GCPA, %d.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                       g_stSmssBaseInfo.ulBeatCount);
        }
    }
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_recv_devmreportfault
* 功    能: 主控模块收到设备监控模块的严重告警后,向全局板(/和)伙伴板报
* 函数类型  VOID
* 参    数: 无。
* 函数返回:
*           无。
*说     明: 该功能在单板初始化阶段和正常运行阶段都存在.在主备倒换阶段,如果收到
*           该消息,直接复位。
***************************************************************************/
VOID smss_sysctl_recv_devmreportfault(VOID)/*UINT32 ulReason)*/
{
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "<%s-%s>: Received message %s.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
        "SMSS_DEVM_BFAULT_IND_MSG");

#ifndef SWP_FNBLK_BRDTYPE_MAIN_CTRL
        /* 如果不是全局板，向上级CPU报告 */
    {
        SMSS_SYSCTL_BFAULT_IND_MSG_T stBFault;

        stBFault.ulReason = SMSS_BFAULT_DEVM_ALARM;
        stBFault.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
        stBFault.ulReason = cpss_htonl(stBFault.ulReason);
        if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)
            /* 若为主CPU，向全局板报故障 */
        {
            stBFault.ulType = SMSS_BFAULT_HOST_TO_GCPA;
            stBFault.ulType = cpss_htonl(stBFault.ulType);
            smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BFAULT_IND_MSG,
                (UINT8 *)&stBFault, sizeof(stBFault));
        }
        else /* 若为从CPU，向主CPU报故障 */
        {
            stBFault.ulType = SMSS_BFAULT_SLAVE_TO_HOST;
            stBFault.ulType = cpss_htonl(stBFault.ulType);
            smss_sysctl_send_host(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BFAULT_IND_MSG,
                (UINT8 *)&stBFault, sizeof(stBFault));
        }
    }
#endif    /* 非全局板 */

#ifdef SMSS_BACKUP_MATE
    if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
        /* 若为主用板，则向备用板发倒换请求 */
    {
        SMSS_SYSCTL_BSWITCH_REQ_MSG_T stBSwitchReq;
#ifdef SMSS_AS_CMP_HARD
        drv_active_standby_set(SMSS_STANDBY);
#endif
        g_stSmssAsInfo.stSwitchInfo.ulReason = SMSS_BSWITCH_DEVM_ALARM;
        stBSwitchReq.ulReason = cpss_htonl(SMSS_BSWITCH_DEVM_ALARM);
        smss_com_send_mate(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BSWITCH_REQ_MSG,
            (UINT8 *)&stBSwitchReq, sizeof(stBSwitchReq));
    }
    else if (SMSS_STANDBY == g_stSmssAsInfo.ucSelfAStatus)
        /* 若为备用板，向主用板发故障指示 */
    {
        SMSS_SYSCTL_BFAULT_IND_MSG_T stBFault;
        stBFault.ulType = SMSS_BFAULT_MATE_TO_MATE;
        stBFault.ulReason = SMSS_BFAULT_DEVM_ALARM;
        stBFault.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
        stBFault.ulType = cpss_htonl(stBFault.ulType);
        stBFault.ulReason = cpss_htonl(stBFault.ulReason);
        smss_com_send_mate(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BFAULT_IND_MSG,
            (UINT8 *)&stBFault, sizeof(stBFault));
    }
#endif

    if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
    {
        /* 若为主 ，进入END态,延时复位 */
        smss_sysctl_enter_end(FALSE,
            SMSS_BRD_RST_CAUSE_RECV_DEVM_BRD_FAULT);
    }
    else
    {
        smss_sysctl_enter_end(TRUE,
            SMSS_BRD_RST_CAUSE_RECV_DEVM_BRD_FAULT);    /* 若为备，立即复位 */
    }
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_recv_statuschgind
* 功    能: 上级处理器收到下级处理器的状态改变消息时进行相应处理，包括全
*           局板对外围板状态改变的处理及主CPU对从CPU或DSP状态改变的处理。
* 函数类型  VOID
* 参    数:
* 参数名称               参数类型          输入/输出    参数描述
* pstMsg            CPSS_COM_MSG_HEAD_T*      IN        消息头结构
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_recv_statuschgind(CPSS_COM_MSG_HEAD_T* pstMsgHead)
{
    SMSS_SYSCTL_STATUS_CHG_IND_MSG_T *pstMsg =
        (SMSS_SYSCTL_STATUS_CHG_IND_MSG_T *)pstMsgHead->pucBuf;

    SMSS_SYSCTL_STATUS_CHG_ACK_MSG_T stAckMsg;
    if (NULL == pstMsg)
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: smss_sysctl_recv_statuschgind(): "
                   "Received NULL message.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        return;
    }
    pstMsg->ulReason = cpss_ntohl(pstMsg->ulReason);

    stAckMsg.stPhyAddr = pstMsg->stPhyAddr;
    stAckMsg.ucAStatus = pstMsg->ucAStatus;
    stAckMsg.ucRStatus = pstMsg->ucRStatus;
    stAckMsg.usResult = 0;
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
    if (pstMsg->stPhyAddr.ucCpu == 1) /* 主CPU */
    {
    	  UINT32 ulResult;

        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "<%s-%s>: Received message %s of (%d-%d-%d).", /* 注意是of，不是from */
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   "SMSS_SYSCTL_STATUS_CHG_IND_MSG",
                   pstMsg->stPhyAddr.ucFrame, pstMsg->stPhyAddr.ucShelf,
                   pstMsg->stPhyAddr.ucSlot);
                   
        /* 处理外围板与备用全局板的状态改变请求 */
        ulResult = smss_sysctl_gcpa_recv_statuschgind(pstMsg);           
        if (ulResult != 0)
        {
        	stAckMsg.usResult = cpss_htons(100);  /* 失败 */
        }      

        if (TRUE == SMSS_ADDR_IS_EQUAL(pstMsgHead->stSrcProc.stLogicAddr,
                                       CPSS_LOGIC_ADDR_GCPA))
            /* 若为备用全局板的报告，则向备用全局板返回确认 */
        {
            smss_com_send_mate(SMSS_SYSCTL_PROC, SMSS_SYSCTL_STATUS_CHG_ACK_MSG,
                               (UINT8*)&stAckMsg, sizeof(stAckMsg));
        }
        else /* 对其他的处理器，按照逻辑地址返回确认 */
        {
            CPSS_COM_PID_T stPid;
            stPid = pstMsgHead->stSrcProc;
            smss_com_send(&stPid, SMSS_SYSCTL_STATUS_CHG_ACK_MSG, (UINT8*)&stAckMsg, sizeof(stAckMsg));
        }
    }
#endif
#if defined(SMSS_BOARD_WITH_DSP) || defined(SMSS_BOARD_WITH_SLAVE)
    if (pstMsg->stPhyAddr.ucCpu == 2) /* 从CPU */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "<%s-%s>: Received message %s from SLAVE.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   "SMSS_SYSCTL_STATUS_CHG_IND_MSG");
    }
    else if (pstMsg->stPhyAddr.ucCpu >= 5) /* DSP */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "<%s-%s>: Received message %s from DSP %d.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   "SMSS_SYSCTL_STATUS_CHG_IND_MSG",
                   pstMsg->stPhyAddr.ucCpu - 4);
    }

    /* 若本板带从CPU或者DSP，则在主CPU上进行处理 */
    smss_sysctl_host_recv_statuschgind(pstMsg);

    smss_com_send_phy(pstMsg->stPhyAddr, SMSS_SYSCTL_PROC,
                      SMSS_SYSCTL_STATUS_CHG_ACK_MSG, NULL, 0);
#endif
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_idle_recv_actprocrsp
* 功    能: 在IDLE态下收到纤程激活应答消息时，进行处理。
* 函数类型  VOID
* 参    数: 无。
* 函数返回:
*           无。
* 说    明: 该函数是IDLE态下收到CPSS的纤程激活应答时的处理，此时开始激活其他纤
*           程或发注册请求消息。
***************************************************************************/
VOID smss_sysctl_idle_recv_actprocrsp(SMSS_PROC_ACTIVATE_RSP_MSG_T * pstMsg)
{
    /* begin of for GCPA 初始化监控修改 */
    if (NULL == pstMsg) /* 消息合法性校验 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
        "<%s-%s>: In Function smss_sysctl_idle_recv_actprocrsp,"
        "Receive Null Message.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        return;
    }
    pulProcActRsp[g_ulSmssSendIndex-1] = pstMsg->ulResult;
    /* end of for GCPA 初始化监控修改 */
   #if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
     CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_2InCnt);
   #endif

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Received %s from Proc `%s', ulResult = %d.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               "SMSS_PROC_ACTIVATE_RSP_MSG",
               cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[g_ulSmssSendIndex-1].ulProcDes),
               pstMsg->ulResult);

    if (g_ulSmssRspedProcNum == 0)
    {
        SMSS_PROC_ACTIVATE_REQ_MSG_T stActProc;

        g_ulSmssRspedProcNum++;
        pulProcActResult[0] = SMSS_OK;
        cpss_timer_delete(SMSS_TIMER_IDLE);

        /* 激活CPSS */
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "<%s-%s>: Send %s to Proc `%s'.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   "SMSS_PROC_ACTIVATE_REQ_MSG",
                   cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulProcDes));

        g_ucSmssIdleStage = SMSS_IDLE_ACT_CPSS;
        stActProc.ucAsStatus = g_stSmssAsInfo.ucSelfAStatus;
        smss_sysctl_send_local(g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulProcDes,
                               SMSS_PROC_ACTIVATE_REQ_MSG,
                               (UINT8*)&stActProc, sizeof(stActProc));
        cpss_timer_set(SMSS_TIMER_IDLE,
                       g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulWaitTime);
        g_ulSmssSendIndex++;
        
        #if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
         CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_2OutCnt);
        #endif

        return;
    }

     if (SMSS_OK == pstMsg->ulResult) /* 激活成功，进行后续处理 */
    {
        /* 记录激活结果，删除等待该应答的定时器 */
        g_ulSmssRspedProcNum = g_ulSmssRspedProcNum + 1;
        pulProcActResult[1] = SMSS_OK;
        cpss_timer_delete(SMSS_TIMER_IDLE);

#ifndef SWP_FNBLK_BRDTYPE_MAIN_CTRL   /* 非全局板 */
            /* 若为非主用全局板，则接着进行注册 */
        g_ucSmssIdleStage = SMSS_IDLE_REG;
        smss_sysctl_send_regreq();
        cpss_timer_loop_set(SMSS_TIMER_IDLE, SMSS_TIMER_IDLE_DELAY);
#else  /* 全局板 */
        if (SMSS_STANDBY == g_stSmssAsInfo.ucSelfAStatus)
        {   /* 若为非主用全局板，则接着进行注册 */
            g_ucSmssIdleStage = SMSS_IDLE_REG;
            smss_sysctl_send_regreq();
            cpss_timer_loop_set(SMSS_TIMER_IDLE, SMSS_TIMER_IDLE_DELAY);
        }
        else
        {   /* 若是主用全局板，则进入INIT态，继续激活纤程 */
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                "<%s-%s>: Transit to state <%s>.",
                g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, "INIT");

            /* 设置状态 */
            g_stSmssBaseInfo.ucRStatus = SMSS_RSTATUS_INIT;
            g_stSmssBaseInfo.szRStatus = "INIT";
            cpss_vk_proc_user_state_set(SMSS_SYSCTL_STATE_INIT);

            /* 开始激活纤程 */
            ucActProcStage = SMSS_SEND_PROC;
            smss_sysctl_send_act_stoa_req();
        }
#endif

    /*  yanghuanjun 20061206 begin */
    #if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
     CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_2OutCnt);
    #endif
    /*  yanghuanjun 20061206 end */
    }
    else    /* 激活CPSS失败，进入终止态 */
    {
#ifdef SMSS_BACKUP_MATE
        if (TRUE == g_stSmssAsInfo.bMateExist)
            /* 如果配置了伙伴板 */
        {
            if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
                /* 若本板为主，则向伙伴板发倒换请求 */
            {
                SMSS_SYSCTL_BSWITCH_REQ_MSG_T stBSwitch;
                stBSwitch.ulReason = SMSS_BSWITCH_MATE_FAULT;
                stBSwitch.ulReason = cpss_htonl(stBSwitch.ulReason);
                smss_com_send_mate(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BSWITCH_REQ_MSG,
                    (UINT8 *)&stBSwitch, sizeof(stBSwitch));
            }
        }

#endif
        /* 设置状态进入终止态,延时后复位 */
        if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
        {
            smss_sysctl_enter_end(FALSE,
                SMSS_BRD_RST_CAUSE_ACT_PROC_CPSS_FAILED_IDLE);
        }
        else
        {
            smss_sysctl_enter_end(TRUE,
                SMSS_BRD_RST_CAUSE_ACT_PROC_CPSS_FAILED_IDLE);
        }
    }
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_phy2mo
* 功    能: 该函数实现物理地址向管理对象信息的转化。
* 函数类型  VOID
* 参    数:
* 参数名称        类型              输入/输出       描述
* pstPhy       CPSS_COM_PHY_ADDR_T *   IN           要转化的物理地址
* pstMo        CPS__OAMS_MO_ID_T *          IN           对应的管理对象信息
* 函数返回:
*           无。
* 说    明: 无。
***************************************************************************/
BOOL smss_sysctl_phy2mo(CPSS_COM_PHY_ADDR_T * pstPhy,  SWP_MO_ID_T *pstMo)
{
    if ((NULL == pstPhy)||(NULL == pstMo))
    {
        return FALSE;
    }

    pstMo->ulMOC = SWP_MOC_DEV_CPU;
    *(UINT32 *)&pstMo->unMOI.stCpu = *(UINT32 *)pstPhy;

    return TRUE;
}

/***************************************************************************
* 函数名称: smss_sysctl_slavecpu_send_periodly_heartbeat
* 功    能: 从CPU或者DSP周期地向主CPU发心跳
* 函数类型  VOID
* 参    数: 无。
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_slavecpu_send_periodly_heartbeat(VOID)
{
    SMSS_SYSCTL_HS_HEARTBEAT_IND_T stHs;

    stHs.ulCpuNo = g_stSmssBaseInfo.stPhyAddr.ucCpu;
    stHs.ulSeqNum = 0;
    stHs.ulCpuNo = cpss_htonl(stHs.ulCpuNo);
    stHs.ulSeqNum = cpss_htonl(stHs.ulSeqNum);
    smss_sysctl_send_host(SMSS_SYSCTL_PROC, SMSS_SYSCTL_HS_HEARTBEAT_IND_MSG,
                          (UINT8 *)&stHs, sizeof(stHs));

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
               "<%s-%s>: CPU %d send heartbeat to HOST.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               g_stSmssBaseInfo.stPhyAddr.ucCpu);

    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_enter_end
* 功    能: 将本板状态转入END态
* 函数类型  INT32
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* bResetNow          BOOL                       IN          是否立即复位本板
* 函数返回:
*           SMSS_OK:成功 SMSS_ERROR:失败
* 说    明:
***************************************************************************/
INT32 smss_sysctl_enter_end(BOOL bResetNow,UINT32 ulResetReason)
{
	CPS_REBOOT_T stRebt;
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
               "<%s-%s>: smss_sysctl_enter_end: ulResetReason = %d (0x%08X).",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               ulResetReason, ulResetReason);
    
    /*设置单板复位原因*/
    g_stSmssBaseInfo.ulResetReason = ulResetReason;
    /* 删除心跳定时器 */
    if (SMSS_RSTATUS_NORMAL == g_stSmssBaseInfo.ucRStatus)
    {
        cpss_timer_delete(SMSS_TIMER_PERIODLY_HEARTBEAT);

        if (SMSS_RSTATUS_NORMAL == g_stSmssAsInfo.ucMateRStatus)
        {
            cpss_timer_delete(SMSS_TIMER_MATE_HEARTBEAT);
        }
    }
    /* 设置状态 */
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
           "<%s-%s>: Transit to state <%s>.",
           g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, "END");

    cpss_vk_proc_user_state_set(SMSS_SYSCTL_STATE_END);
    g_stSmssBaseInfo.ucRStatus = SMSS_RSTATUS_END;
    g_stSmssBaseInfo.szRStatus = "END";
    g_stSmssAsInfo.ucSelfAStatus = SMSS_UNKNOWN_AS_STATUS;
    g_stSmssAsInfo.szSelfAStatus = "UNKNOWN";
    
#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)  /* 如果为主CPU，设置面板灯 */
    drv_board_led_set(DRV_LED_TYPE_IS, DRV_LED_STATU_OFF);
    drv_board_led_set(DRV_LED_TYPE_ACTIVE, DRV_LED_STATU_OFF);
    smss_devm_SetISled(0);
    smss_devm_SetActled(0);
#endif
    
    /* 关闭主备竞争 */
#ifdef SMSS_AS_CMP_HARD

#if 0
    if (TRUE == g_stSmssAsInfo.bMateExist)
    {
        drv_active_standby_set(SMSS_STANDBY); 
        drv_active_standby_enable(FALSE); /* 关闭主备竞争 */
    }
#endif
  
#endif
    
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
    /* 进行复位操作 */

    #if 0
    if (TRUE == bResetNow)
    #endif
    {
        cpss_reset(g_stSmssBaseInfo.ulResetReason);
    }
    #if 0
    else
    {
        cpss_timer_set(SMSS_TIMER_END, SMSS_TIMER_END_DELAY);
    }
    #endif

#else
    /* 从CPU写黑匣子记录复位原因 */
    #if (SWP_CPUROLE_SLAVE == SWP_CPUROLE_TYPE)
	  {
     UINT32 ulArgs[4];
     cpss_sbbr_write_text(0, ulArgs,"GMT %d, slave enter end,Casue: 0x%x\n\r",cpss_gmt_get(), ulResetReason);
	  }
    #endif
    /* 如果是从CPU或DSP，不进行复位操作 上报到主CPU进行复位*/
    smss_sysctl_send_host(SMSS_SYSCTL_PROC, SMSS_RESET_SLAVE_CMD_MSG,
               NULL, 0);
#endif

    cpss_dbg_pro_time_pause();

    cpss_timer_delete(SMSS_TIMER_HB_BASE);
    stRebt.ucRebtType = CPS_CPU_REBOOT_HOT;
    stRebt.ucRebtRsn = CPS_REBOOT_BY_ENTER_END;

    if(NULL != g_szInitFailProcName)
    {
		if(0 == strncmp(g_szInitFailProcName, "pRdbsInit", strlen("pRdbsInit")))
		{
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FAIL, "pRdbsInit proc init fail! please check!");
			return SMSS_OK;
		}
    }

    switch(ulResetReason)
    {
    case SMSS_BRD_RST_CAUSE_ACT_PROC_FAILURE_SLAVE_BRD_STOA:
        stRebt.szExtra = g_szInitFailProcName;
    	break;
    default:
        stRebt.szExtra = "enter end";
    	break;
    }

    cps_boot_reboot(&stRebt);
    return SMSS_OK;
}
/***************************************************************************
* 函数名称: smss_sysctl_normal_brd_reset
* 功    能: 在NORMAL状态下处理复位命令
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* ulReason            UINT32                   IN           复位原因
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_brd_reset(UINT32 ulReason)
{
#ifndef SWP_FNBLK_BRDTYPE_MAIN_CTRL
        SMSS_SYSCTL_STATUS_CHG_IND_MSG_T stStgChg;
        stStgChg.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
        stStgChg.ucAStatus = SMSS_STANDBY;
        stStgChg.ucRStatus = SMSS_RSTATUS_END;
        stStgChg.ulReason = cpss_htonl(ulReason);
        stStgChg.ucBoardState = SMSS_BRD_RSTATUS_FAULT;
        g_ucBoardState = SMSS_BRD_RSTATUS_FAULT;
        smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_STATUS_CHG_IND_MSG,
                              (UINT8 *)&stStgChg, sizeof(stStgChg));
#endif
   
    smss_sysctl_enter_end(TRUE, ulReason);    

}


/***************************************************************************
* 函数名称: smss_sysctl_clr_sbbr_flag
* 功    能: 单板主CPU收到清黑匣子标志
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* pstMsg            UINT8*                  IN           
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_clr_sbbr_flag(UINT8 *pstMsg)
{  
	 #if (SWP_CPUROLE_DSP != SWP_CPUROLE_TYPE) 
	   /* 清标志, 关句柄 */   	 
	   cpss_sbbr_clr_abnormal_reset_flag();
     cpss_fs_fd_close();   
   #endif
   #if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)                   
     #if defined(SMSS_BOARD_WITH_SLAVE)
       smss_send_to_slave(SMSS_SYSCTL_PROC,SMSS_SYSCTL_CLEAR_SBBR_FLAG_MSG,
            NULL,0);   
     #endif
   #endif

   #if (SWP_PHYBRD_MASA == SWP_PHYBRD_TYPE)
     smss_showverlen();
   #endif

   return;	
}

/***************************************************************************
* 函数名称: smss_sysctl_pub_func
* 功    能: 处理系统主控的公共的消息。
* 函数类型  VOID
* 参    数:
* 参数名称     类型            输入/输出       描述
* pstMsg CPSS_COM_MSGHEAD_T *     IN         消息头结构
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
BOOL smss_sysctl_pub_func(CPSS_COM_MSG_HEAD_T *pstMsg)
{
   switch (pstMsg->ulMsgId)
   {
   case SMSS_TIMER_CPU_USAGE_QUERY_MSG:
        #if 0
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>:  Received  <%s>.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, "SMSS_TIMER_CPU_USAGE_QUERY_MSG");
        #endif       

        smss_sysctl_cpu_usage_poll();
   	    return TRUE;
   case SMSS_ACT_STOA_RSP_IND_MSG:
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
           "<%s-%s>:  Received  <%s>.",
           g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, "SMSS_ACT_STOA_RSP_IND_MSG");
        smss_sysctl_actstoa_disp(pstMsg->pucBuf);
        return TRUE;
   case SMSS_TIMER_HARD_CMP_MSG:
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
           "<%s-%s>:  Received  <%s>.",
           g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, "SMSS_TIMER_HARD_CMP_MSG");
        smss_sysctl_hard_cmp_time_handle();
        return TRUE;
   case SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG:     
       /* 全局板发来的强制复位消息 */
       cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
           "<%s-%s>: Received message %s.",
           g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
           "SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG");
       smss_sysctl_force_reset((SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG_T *)pstMsg->pucBuf);
       return TRUE;
   case SMSS_TIMER_BRD_RST_MSG:
       cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
           "<%s-%s>:  Received  <%s>.",
           g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, "SMSS_TIMER_BRD_RST_MSG");

#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
       g_stSmssBaseInfo.ulResetReason = SMSS_BRD_RST_FORCE_RESET;
       cpss_reset(g_stSmssBaseInfo.ulResetReason);
#endif

       return TRUE;

  case SMSS_BOARDSELF_FAULT_IND_MSG:
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
           "<%s-%s>: Received message %s.",
           g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
           "SMSS_BOARDSELF_FAULT_IND_MSG");
         smss_sysctl_recv_brdself_fault_ind(pstMsg);
         return TRUE; 
  case SMSS_SYSCTL_CLEAR_SBBR_FLAG_MSG:
       cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
           "<%s-%s>:  Received  <%s>.",
           g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, "SMSS_SYSCTL_CLEAR_SBBR_FLAG_MSG");
       smss_sysctl_clr_sbbr_flag(pstMsg->pucBuf);
       return TRUE;

   #if defined(SMSS_BOARD_WITH_DSP)
    case SMSS_VERM_DSP_START_IND_MSG:       /* VERM通知某DSP启动 */
       smss_sysctl_start_dsp((SMSS_VERM_SLAVE_DSP_START_IND_MSG_T *)(pstMsg->pucBuf));
       return TRUE;
    case SMSS_SYSCTL_DEL_DSP_TIMER_MSG:     /* 删除等待DSP定时器 */  
       smss_sysctl_del_dsptimer((SMSS_SYSCTL_DEL_DSP_TIMER_MSG_T *)(pstMsg->pucBuf));
       return TRUE;   
   #endif 

    case  SMSS_TIMER_CPU_USAGE_MSG:  /* 15分钟定时器超时 */
      smss_sysctl_cpu_usage_timerout();      
      return TRUE;   

   default:
         return FALSE;
   }
}

/***************************************************************************
* 函数名称: smss_sysctl_cpu_usage_init
* 功    能: CPU占用率初始化
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* 无
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_cpu_usage_init(VOID)
{
     cpss_mem_memset(&g_stCpuUsageInfo, 0, sizeof(g_stCpuUsageInfo));
     g_stCpuUsageInfo.ulSmssCpuUsageHigh = 80;/* 上限阀值 */
     g_stCpuUsageInfo.ulSmssCpuUsageLow  = 10;/* 下限阀值 */     
     cpss_timer_loop_set(SMSS_TIMER_CPU_USAGE_QUERY, SMSS_TIMER_CPU_USAGE_QUERY_DELAY);
}


/***************************************************************************
* 函数名称: smss_showcpuusage
* 功    能: CPU占用率show
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* 无
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_showcpuusage(VOID)
{
  cps__oams_shcmd_printf("usage %d, timer %d, alarm %d, low %d, high %d\n",
                    g_stCpuUsageInfo.ulCurSmssCpuUsageState,
                    g_stCpuUsageInfo.ulCurCpuUsageLowCount,
                    g_stCpuUsageInfo.ulCurCpuUsageHighCount,
                    g_stCpuUsageInfo.ulSmssCpuUsageLow,
                    g_stCpuUsageInfo.ulSmssCpuUsageHigh);
  return;	
}

/***************************************************************************
* 函数名称: smss_sysctl_cpu_alarm
* 功    能: CPU占用率告警
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* ulCpuUsage         UINT32                 cpu占用率
* ucFlag             UINT8                  产生或清除
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_cpu_alarm(UINT32 ulCpuUsage, UINT8 ucFlag)
{
	CPS__OAMS_AM_ORIGIN_ALARM_T stOamsAlarm;
	
	stOamsAlarm.ulAlarmNo = SMSS_ALMID_CPU_OVERLOADED;
  stOamsAlarm.ulAlarmSubNo = 0;
  smss_moid_get_local_moid(CPS__OAMS_MOID_TYPE_CPU,&stOamsAlarm.stFaultMoID);  
  stOamsAlarm.enAlarmType = ucFlag;
  stOamsAlarm.aucExtraInfo[0] = (UINT8)(ulCpuUsage);
  stOamsAlarm.aucExtraInfo[1] = (UINT8)(g_stCpuUsageInfo.ulSmssCpuUsageHigh);
  stOamsAlarm.aucExtraInfo[2] = (UINT8)(g_stCpuUsageInfo.ulSmssCpuUsageLow);
  #if 0
   smss_send_alarm_to_oams(&stOamsAlarm);
  #endif
  #if (SWP_CPUROLE_DSP != SWP_CPUROLE_TYPE)
   if (CPS__OAMS_ALARM_OCCUR == ucFlag)
   {
    cpss_kw_prt_occupy_log();
   } 
  #endif
	return;
}

/***************************************************************************
* 函数名称: smss_sysctl_cpu_usage_poll
* 功    能: CPU占用率超时查询
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* 无
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_cpu_usage_poll(VOID)
{
	#if (SWP_CPUROLE_TYPE != SWP_CPUROLE_DSP)
	  #if ((SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE) || (SWP_CPUROLE_SLAVE == SWP_CPUROLE_TYPE))
	    smss_sysctl_cpu_usage_process(); /* 主从CPU占用率处理 */ 
	  #endif
	  #ifdef SMSS_BOARD_WITH_DSP
	    smss_sysctl_dsp_usage_process(); /* DSP占用率处理 */
	  #endif   
	#endif
	
	#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)  /* 处理单板的过载级别 */
	   smss_sysctl_board_usage_process();
	#endif
  return;
}

/***************************************************************************
* 函数名称: smss_sysctl_cpu_usage_timerout
* 功    能: 15分钟定时器超时
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* 无
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_cpu_usage_timerout(VOID)
{
 #if (SWP_CPUROLE_TYPE != SWP_CPUROLE_DSP)
	if (0 == g_stCpuUsageInfo.ulCurCpuUsageHighCount) /* 无告警 */
	{
	  smss_sysctl_cpu_alarm(g_smssCpuLoadInfo.ucCurrUsage, CPS__OAMS_ALARM_OCCUR);
	  g_stCpuUsageInfo.ulCurCpuUsageHighCount = 1; 	
	}
 #endif	
	return;
}


/***************************************************************************
* 函数名称: smss_sysctl_actproc_stoarsp_event
* 功    能: 激活纤程或备升主响应事件上报
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* 无
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_actproc_stoarsp_event(UINT32 ulMsgId, UINT32 ulProcId, UINT32 ulResult)
{
    SMSS_ACT_STOA_RSP_IND_MSG_T stEvent;
    UINT32                 ulProcState = 0;
    if (ulMsgId == SMSS_PROC_ACTIVATE_RSP_MSG)
    {
        stEvent.ulEventNo = cpss_htonl(SMSS_EVENTNO_BOARD_ACT_PROC_RSP);
    }
    else if(ulMsgId == SMSS_STANDBY_TO_ACTIVE_RSP_MSG)
    {
        stEvent.ulEventNo = cpss_htonl(SMSS_EVENTNO_BOARD_STOA_RSP);
        ulProcState = 1;/* 备升主阶段 */
    }
    stEvent.ulEventSubNo = 0;
    stEvent.ulObjId =  cpss_htonl(ulProcId);
    stEvent.ulResult = cpss_htonl(ulResult);
    stEvent.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
    if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)
    {      
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL
        if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)/* 主用全局板 */
           {
               /*直接上报到OAMS纤程激活事件上报*/
               smss_sysctl_send_local(SMSS_SYSCTL_PROC, SMSS_ACT_STOA_RSP_IND_MSG, 
                       (UINT8 *)&stEvent, sizeof(stEvent));
           }
        else  /*备用全局板上报的主用全局单板 */
           {
               if (1 == ulProcState)
               {
                   smss_sysctl_send_local(SMSS_SYSCTL_PROC, SMSS_ACT_STOA_RSP_IND_MSG, 
                       (UINT8 *)&stEvent, sizeof(stEvent));
               }
               else
               {
                   /*发送消息到主用全局板*/
                   smss_com_send_mate(SMSS_SYSCTL_PROC, SMSS_ACT_STOA_RSP_IND_MSG, 
                       (UINT8 *)&stEvent, sizeof(stEvent));
               }
           }
#else
               /*发送消息到主用全局板*/
               smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_ACT_STOA_RSP_IND_MSG,
               (UINT8 *)&stEvent, sizeof(stEvent));
#endif   
    }
    else if (SWP_CPUROLE_SLAVE == SWP_CPUROLE_TYPE
              ||SWP_CPUROLE_DSP == SWP_CPUROLE_TYPE)/*从CPU或者是DSP*/
    {
           /*上报到主CPU上*/
           smss_sysctl_send_host(SMSS_SYSCTL_PROC, SMSS_ACT_STOA_RSP_IND_MSG,
               (UINT8 *)&stEvent, sizeof(stEvent));
    }
}

/***************************************************************************
* 函数名称: smss_sysctrl_show_rst_brd_reason
* 功    能: 显示单板复位原因
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* ulRstReason          UINT32                  IN          单板复位原因编号
* 函数返回:无
* 说    明:
***************************************************************************/
INT32 smss_sysctrl_show_rst_brd_reason(UINT32 ulRstReason)
{
   UINT32 ulReasonIndex = ulRstReason-SMSS_BRD_RST_CAUSE_BASE;
   if( ulReasonIndex > g_ulBrdRstCauseNum )
   {
       cps__oams_shcmd_printf("\n[SMSS][SYSCTL]:<%s-%s>:invalid board reset reason code(%#x)!!\n",
           g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,ulRstReason);
       return CPS__OAMS_ERROR;
   }
   cps__oams_shcmd_printf("\n[SMSS][SYSCTL]:<%s-%s>:board reset reason %#x = %s\n",
       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,

       ulRstReason,passmss_brd_rst_reason[ulReasonIndex - 1]);

    return CPS__OAMS_OK;
}


/***************************************************************************
* 函数名称: smss_showrstreason
* 功    能: 显示单板复位原因,程序进入END态时,单板没有复位的情况下,显示进入END态的原因.
* 函数类型  INT32
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* 函数返回:无
* 说    明:
***************************************************************************/
INT32 smss_showrstreason(VOID)
{
	 smss_sysctrl_show_rst_brd_reason(g_stSmssBaseInfo.ulResetReason);
	 return CPS__OAMS_OK; 
}

/***************************************************************************
* 函数名称: smss_show_board_info
* 功    能: 显示单板的状态信息
* 函数类型  INT32 
* 参    数: 
* 参数名称   参数类型  输入/输出      参数描述
* ulPhyAddr   UINT32     IN        指定单板的物理地址，从最高位到最低位依次
*                                  为架框槽CPU号（CPU号不使用）。0表示本板。
* 函数返回: 
*           无。
* 说    明: 该函数用于调试
***************************************************************************/
INT32 smss_show_board_info(UINT32 ulPhyAddr)
{
    UINT32 ulNewPhyAddr = cpss_ntohl(ulPhyAddr);
    if (SMSS_ADDR_IS_EQUAL(ulNewPhyAddr, g_stSmssBaseInfo.stPhyAddr)
        || 0 == ulPhyAddr)      /* 本板 */
    {
        cps__oams_shcmd_printf("  PhyAddr: %d-%d-%d-%d\n",
               g_stSmssBaseInfo.stPhyAddr.ucFrame, g_stSmssBaseInfo.stPhyAddr.ucShelf, 
               g_stSmssBaseInfo.stPhyAddr.ucSlot, g_stSmssBaseInfo.stPhyAddr.ucCpu);
        cps__oams_shcmd_printf("  AStatus: %s\n", g_stSmssAsInfo.szSelfAStatus);
        cps__oams_shcmd_printf("  RStatus: %s\n", g_stSmssBaseInfo.szRStatus);
        return CPS__OAMS_OK;
    }
    else
    {
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
        if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus) /* 主用全局板 */
        {
            smss_sysctl_gcpa_show_board_info(ulPhyAddr);
            return CPS__OAMS_OK;
        }
#endif
        /* 非主用全局板 */
        cps__oams_shcmd_printf("I'm not ACTIVE GCPA.\n");
        return CPS__OAMS_ERROR;
    }   
}


/***************************************************************************
* 函数名称: smss_com_send_mate
* 功    能: 主备板纤程间消息发送，只允许在纤程中调用。
* 函数类型  VOID
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
INT32 smss_com_send_mate(UINT32 ulDstPd,UINT32 ulMsgId,
                     UINT8 *pucData, UINT32 ulDataLen)
{
    smss_test_PrintMsgStream(ulMsgId,pucData,ulDataLen,TRUE);
    return cpss_com_send_mate(ulDstPd,ulMsgId,pucData,ulDataLen);
}
/*******************************************************************************
* 函数名称: smss_com_send_phy
* 功    能: 使用物理地址发送，跟链路无关，直接使用UDP发送，无应答
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出         描述
* pstDstPid   CPSS_COM_MSG_HEAD_T*     输入            消息头结构指针
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 发送标志默认为可靠、普通优先级。
*           消息体的最大长度：2M BYTE。
*******************************************************************************/
INT32 smss_com_send_phy
(
   CPSS_COM_PHY_ADDR_T  stDstPhyAddr,
   UINT32 ulDstPd,
   UINT32 ulMsgId, 
   UINT8 *pucData, 
   UINT32 ulDataLen
 )
{
    smss_test_PrintMsgStream(ulMsgId,pucData,ulDataLen,TRUE);
    return cpss_com_send_phy(stDstPhyAddr,ulDstPd,ulMsgId,pucData,ulDataLen);
}
/*******************************************************************************
* 函数名称: smss_com_send
* 功    能: 纤程间消息发送，只允许在纤程中调用，以调用者的身份发送消息。
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型            输入/输出         描述
* pstDstPid   CPSS_COM_PID_T*     输入            目标纤程的PID
* ulMsgId     UINT32            输入            消息ID
* pucData     UINT8 *           输入            消息体指针
* ulDataLen   UINT32            输入            消息体长度
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: CPU内最大消息体长度：没有限制，受限于最大内存块。
*           CPU间最大消息体长度：（64K－256）BYTE
*           发送标志默认为可靠、普通优先级。
*******************************************************************************/
INT32 smss_com_send 
(
   CPSS_COM_PID_T *pstDstPid,
   UINT32 ulMsgId,
   UINT8 *pucData,
   UINT32 ulDataLen
)
{
    smss_test_PrintMsgStream(ulMsgId,pucData,ulDataLen,TRUE);
    return cpss_com_send(pstDstPid,ulMsgId,pucData,ulDataLen);
}

/***************************************************************************
* 函数名称: smss_showself
* 功    能: 显示本板的信息
* 函数类型  INT32
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
INT32 smss_showself(VOID)
{
   cps__oams_shcmd_printf("\nFrame   Shelf    Slot  Cpu   RStatus      AStatus    BeatCount\n");
   cps__oams_shcmd_printf("%5d%8d%8d%5d%12s%13s%10d\n",
                g_stSmssBaseInfo.stPhyAddr.ucFrame,
                g_stSmssBaseInfo.stPhyAddr.ucShelf,
                g_stSmssBaseInfo.stPhyAddr.ucSlot,
                g_stSmssBaseInfo.stPhyAddr.ucCpu,
                g_stSmssBaseInfo.szRStatus,
                g_stSmssAsInfo.szSelfAStatus,
                g_stSmssBaseInfo.ulBeatCount
                );
   return CPS__OAMS_OK;

}
/***************************************************************************
* 函数名称: smss_showmate
* 功    能: 显示伙伴板的信息
* 函数类型  INT32
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
INT32 smss_showmate(VOID)
{
   if (TRUE != g_stSmssAsInfo.bMateExist)
   {
      cps__oams_shcmd_printf("\nno mate board\n");
      return CPS__OAMS_OK;
   }
   cps__oams_shcmd_printf("\nMFrame   MShelf    MSlot    MRStatus     MAStatus  BeatCount\n");
   cps__oams_shcmd_printf("%5d%8d%8d%14s%13s%10d\n",
                g_stSmssAsInfo.stMatePhyAddr.ucFrame,
                g_stSmssAsInfo.stMatePhyAddr.ucShelf,
                g_stSmssAsInfo.stMatePhyAddr.ucSlot,
                (SMSS_RSTATUS_NORMAL ==  g_stSmssAsInfo.ucMateRStatus ? "NORMAL" : "FAULT"),
                (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus ? "STANDBY" :"ACTIVE"),
                g_stSmssAsInfo.ucMateBeatCount
                );
   return CPS__OAMS_OK;                
}

/***************************************************************************
* 函数名称: smss_send_event_to_oams
* 功    能: 向OAMS发送事件
* 函数类型  void
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
   pstAlarm     CPS__OAMS_AM_ORIGIN_EVENT_T          输入             发送的事件消息
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
VOID smss_send_event_to_oams(CPS__OAMS_AM_ORIGIN_EVENT_T * pstEvent)
{    
    CPS__OAMS_AM_ORIGIN_EVENT_T stEventSend;

    UINT32 ulMsgId;
    ulMsgId = pstEvent->ulEventNo;
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Send Alarm to Oams: EventNo = 0x%x",pstEvent->ulEventNo);

    stEventSend.ulEventNo = cpss_htonl(pstEvent->ulEventNo);
    stEventSend.ulEventSubNo = cpss_htonl(pstEvent->ulEventSubNo);
    cpss_mem_memcpy(&stEventSend.stEventMoId,&pstEvent->stEventMoId,sizeof(CPS__OAMS_MO_ID_T));
    stEventSend.ulExtraLen = cpss_htonl(pstEvent->ulExtraLen);
    cpss_mem_memcpy(&stEventSend.aucExtraInfo,&pstEvent->aucExtraInfo,CPS__OAMS_AM_EVENT_EXTRA_INFO_LEN);

    smss_test_PrintMsgStream(ulMsgId,(UINT8*)&stEventSend,sizeof(stEventSend),TRUE);

    cps__oams_am_send_event(&stEventSend);
}

/***************************************************************************
* 函数名称: smss_send_alarm_to_oams
* 功    能: 向OAMS发送告警
* 函数类型  void
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
   pstAlarm      CPS__OAMS_AM_ORIGIN_ALARM_T          输入            传送的告警消息
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
VOID smss_send_alarm_to_oams(CPS__OAMS_AM_ORIGIN_ALARM_T * pstAlarm)
{
    CPS__OAMS_AM_ORIGIN_ALARM_T stAlarmSend;
    UINT32 ulMsgId;
    ulMsgId = pstAlarm->ulAlarmNo;

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Send Alarm to Oams: AlarmNo = 0x%x, ulAlarmSubNo = %d, enAlarmType = %d",
        pstAlarm->ulAlarmNo,pstAlarm->ulAlarmSubNo,pstAlarm->enAlarmType);

    stAlarmSend.ulAlarmNo = cpss_htonl(pstAlarm->ulAlarmNo);
    stAlarmSend.ulAlarmSubNo = cpss_htonl(pstAlarm->ulAlarmSubNo);
    stAlarmSend.enAlarmType = cpss_htonl(pstAlarm->enAlarmType);
    cpss_mem_memcpy(&stAlarmSend.stFaultMoID,&pstAlarm->stFaultMoID,sizeof(CPS__OAMS_MO_ID_T));
    cpss_mem_memcpy(&stAlarmSend.aucExtraInfo,&pstAlarm->aucExtraInfo,CPS__OAMS_AM_ALARM_EXTRA_INFO_LEN);
    smss_test_PrintMsgStream(ulMsgId,(UINT8*)&stAlarmSend,sizeof(stAlarmSend),TRUE);  
    cps__oams_am_send_alarm(&stAlarmSend);
}

/***************************************************************************
* 函数名称: smss_send_alarm_brdnotinstalled_to_oams
* 功    能: 向OAMS发送告警
* 函数类型  void
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
   pstAlarm      CPS__OAMS_AM_ORIGIN_ALARM_T          输入            传送的告警消息
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
VOID  smss_send_alarm_brdnotinstalled_to_oams(UINT32 ulAlarmSubNo, UINT8 ucCurOLStatus, 
                                              CPSS_COM_PHY_ADDR_T stPhyAddr)
{
    CPS__OAMS_AM_ORIGIN_ALARM_T stOamsAlarm;
    stOamsAlarm.ulAlarmNo = SMSS_ALMID_BOARD_NOT_INSTALLED;
    stOamsAlarm.ulAlarmSubNo = ulAlarmSubNo;
    if(SMSS_BOARD_OFFLINE == ucCurOLStatus)
    {
        stOamsAlarm.enAlarmType = CPS__OAMS_ALARM_OCCUR;
    }
    else
    {
        stOamsAlarm.enAlarmType = CPS__OAMS_ALARM_CLEAR;
    }

    smss_moid_get_moid_by_phy(CPS__OAMS_MOID_TYPE_BOARD,&stPhyAddr,&stOamsAlarm.stFaultMoID);
    cpss_mem_memset(&stOamsAlarm.aucExtraInfo, 0, CPS__OAMS_AM_ALARM_EXTRA_INFO_LEN); 
    smss_send_alarm_to_oams(&stOamsAlarm);
}
/***************************************************************************
* 函数名称: smss_send_alarm_brdnotresponded_to_oams
* 功    能: 向OAMS发送告警
* 函数类型  void
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
   pstAlarm      CPS__OAMS_AM_ORIGIN_ALARM_T          输入            传送的告警消息
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
VOID  smss_send_alarm_brdnotresponded_to_oams(UINT32 ulAlarmSubNo, UINT32 ulAlarmType, 
                                              CPSS_COM_PHY_ADDR_T stPhyAddr)
{
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL
    CPS__OAMS_AM_ORIGIN_ALARM_T stOamsAlarm;
    UINT32 ulBoardIndex;
    SMSS_BOARD_INFO_T * pstNode;

    ulBoardIndex = Phy2BoardIndex(stPhyAddr);
    if (NULL == apstBoardIndex[ulBoardIndex])
    {
        return;
    }
    pstNode = apstBoardIndex[ulBoardIndex];
    if ((1 != ulAlarmType)                                         /* 告警解除 */
        && (pstNode->ucSubNoFlag[ulAlarmSubNo] != 1))              /* 该细节号的告警没有产生过*/
    {
        return;
    }
    
    if (1 == ulAlarmType)
    {
        pstNode->ucSubNoFlag[ulAlarmSubNo] = 1;
    }
    else
    {
        pstNode->ucSubNoFlag[ulAlarmSubNo] = 0;
    }

    stOamsAlarm.ulAlarmNo = SMSS_ALMID_BOARD_NOT_RESPONDED;
    stOamsAlarm.ulAlarmSubNo = ulAlarmSubNo;
    if(1 == ulAlarmType)
    {
        stOamsAlarm.enAlarmType = CPS__OAMS_ALARM_OCCUR;
    }
    else
    {
        stOamsAlarm.enAlarmType = CPS__OAMS_ALARM_CLEAR;
    }

    smss_moid_get_moid_by_phy(CPS__OAMS_MOID_TYPE_BOARD,&stPhyAddr,&stOamsAlarm.stFaultMoID);
    cpss_mem_memset(&stOamsAlarm.aucExtraInfo, 0, CPS__OAMS_AM_ALARM_EXTRA_INFO_LEN); 

   *(UINT32 *)stOamsAlarm.aucExtraInfo = cpss_htonl(SMSS_SYSCTRL_ERR_LOST_HEARTBEAT); 

    smss_send_alarm_to_oams(&stOamsAlarm);

    /* ulAlarmSubNo 0, ulAlarmType 0 只出现在全局板第一次收到心跳, 所以增加统计
     * 当报单板告警清除时, 记录.
    */
  	if (0 == ulAlarmType)  /* 告警清除 */
  	{
  	  pstNode->aucRsv[ulAlarmSubNo] = 10;
  	}
  	else if (1 == ulAlarmType) /* 告警产生 */
  	{
  		pstNode->aucRsv[ulAlarmSubNo] = 1;
  	}  
#endif
}
/***************************************************************************
* 函数名称: smss_send_alarm_brdfailed_to_oams
* 功    能: 向OAMS发送告警
* 函数类型  void
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
   pstAlarm      CPS__OAMS_AM_ORIGIN_ALARM_T          输入            传送的告警消息
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
VOID  smss_send_alarm_brdfailed_to_oams(SMSS_ACT_STOA_RSP_IND_MSG_T *pstActStoaInd)
{
    CPS__OAMS_AM_ORIGIN_EVENT_T stEvent;     
    CPS__OAMS_AM_ORIGIN_ALARM_T stOamsAlarm;
    smss_moid_get_moid_by_phy(CPS__OAMS_MOID_TYPE_BOARD,&pstActStoaInd->stPhyAddr,&stOamsAlarm.stFaultMoID);
    stEvent.ulEventNo = pstActStoaInd->ulEventNo;
    stEvent.ulEventSubNo = pstActStoaInd->ulEventSubNo;
    stEvent.stEventMoId = stOamsAlarm.stFaultMoID;
    stEvent.ulExtraLen = 8;
    *(UINT32*)stEvent.aucExtraInfo = cpss_htonl(pstActStoaInd->ulObjId);
    *(UINT32*)(stEvent.aucExtraInfo+4) = cpss_htonl(pstActStoaInd->ulResult);
#if (SWP_INIT_PROC_ACTIVE_EVENT_CTRL_FLAG == TRUE)
    smss_send_event_to_oams(&stEvent);
#endif
   if (0 != pstActStoaInd->ulResult)
    {
        smss_send_event_to_oams(&stEvent);
        /* 记录纤程激和失败信息 */
		    #ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
		     smss_sysctl_gcpa_record_info(pstActStoaInd->stPhyAddr, stOamsAlarm.ulAlarmNo, 
		                           stOamsAlarm.ulAlarmSubNo,
		                           cpss_htonl(pstActStoaInd->ulObjId),
		                           cpss_htonl(pstActStoaInd->ulResult)); 
		    #endif    
    }
    #if 0
    if (SMSS_EVENTNO_BOARD_ACT_PROC_RSP == pstActStoaInd->ulEventNo)
    {
        stOamsAlarm.ulAlarmNo = SMSS_ALMID_BOARD_SW_INIT_FAILED;
        stOamsAlarm.ulAlarmSubNo = SMSS_ALMSUBNO_1;
    }
    else
    {
    	  /* 备升主失败上报事件 */
    	  stEvent.ulEventNo = SMSS_EVENTNO_BOARD_STANDBY_TO_ACTIVE_FAIL;
    	  stEvent.ulEventSubNo = 0;
    	  smss_send_event_to_oams(&stEvent);
    	  return;
    }
    stOamsAlarm.enAlarmType = CPS__OAMS_ALARM_OCCUR;
    *(UINT32*)stOamsAlarm.aucExtraInfo = cpss_htonl(pstActStoaInd->ulObjId);
    *(UINT32*)(stOamsAlarm.aucExtraInfo+4) = cpss_htonl(pstActStoaInd->ulResult);
    smss_send_alarm_to_oams(&stOamsAlarm);
    #endif
}
/***************************************************************************
* 函数名称: smss_send_alarm_cpufailed_to_oams
* 功    能: 向OAMS发送告警
* 函数类型  void
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
   pstAlarm      CPS__OAMS_AM_ORIGIN_ALARM_T          输入            传送的告警消息
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
VOID  smss_send_alarm_cpufailed_to_oams(SMSS_ACT_STOA_RSP_IND_MSG_T *pstActStoaInd)
{
    CPS__OAMS_AM_ORIGIN_EVENT_T stEvent;     
    CPS__OAMS_AM_ORIGIN_ALARM_T stOamsAlarm;

    if (2 < pstActStoaInd->stPhyAddr.ucCpu)
    {
        smss_moid_get_moid_by_phy(CPS__OAMS_MOID_TYPE_CPU,&pstActStoaInd->stPhyAddr,&stOamsAlarm.stFaultMoID);
    }
    else
    {
        smss_moid_get_moid_by_phy(CPS__OAMS_MOID_TYPE_SLAVE_CPU,&pstActStoaInd->stPhyAddr,&stOamsAlarm.stFaultMoID);

    }

    stEvent.ulEventNo = pstActStoaInd->ulEventNo;
    stEvent.ulEventSubNo = pstActStoaInd->ulEventSubNo;
    stEvent.stEventMoId = stOamsAlarm.stFaultMoID;
    stEvent.ulExtraLen = 8;

    *(UINT32*)stEvent.aucExtraInfo = cpss_htonl(pstActStoaInd->ulObjId);
    *(UINT32*)(stEvent.aucExtraInfo+4) = cpss_htonl(pstActStoaInd->ulResult);

#if (SWP_INIT_PROC_ACTIVE_EVENT_CTRL_FLAG == TRUE)
    smss_send_event_to_oams(&stEvent);
#endif
    if (0 != pstActStoaInd->ulResult)
    {
      smss_send_event_to_oams(&stEvent);
      
      if (2 < pstActStoaInd->stPhyAddr.ucCpu)
	    {
	        stOamsAlarm.ulAlarmNo = SMSS_ALMID_BOARD_DSP_FAULTY;
	    }
	    else
	    {
	        stOamsAlarm.ulAlarmNo = SMSS_ALMID_BOARD_SCPU_FAULTY;
	    }

	    stOamsAlarm.ulAlarmSubNo = SMSS_ALMSUBNO_2;
	    stOamsAlarm.enAlarmType = CPS__OAMS_ALARM_OCCUR;
	    *(UINT32*)stOamsAlarm.aucExtraInfo = cpss_htonl(pstActStoaInd->ulObjId);
	    *(UINT32*)(stOamsAlarm.aucExtraInfo+4) = cpss_htonl(pstActStoaInd->ulResult);

	    smss_send_alarm_to_oams(&stOamsAlarm);
    }
}
/***************************************************************************
* 函数名称: smss_send_alarm_brdnotinstalled_to_oams
* 功    能: 向OAMS发送告警
* 函数类型  void
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
   pstAlarm      CPS__OAMS_AM_ORIGIN_ALARM_T          输入            传送的告警消息
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
VOID  smss_sysctl_actstoa_disp(VOID *pbuf)
{
    SMSS_ACT_STOA_RSP_IND_MSG_T stActStoaInd;
    SMSS_ACT_STOA_RSP_IND_MSG_T *pstActStoaInd = pbuf;

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
           "<%s-%s>:  Received  <%s> From Board(%d-%d-%d-%d) ProcName(%s),ActResult(%#x)",
           g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, "SMSS_ACT_OR_STOA_EVENT_IND_MSG",\
           pstActStoaInd->stPhyAddr.ucFrame,pstActStoaInd->stPhyAddr.ucShelf,pstActStoaInd->stPhyAddr.ucSlot,\
           pstActStoaInd->stPhyAddr.ucCpu,cpss_vk_proc_name_get_by_pid(cpss_ntohl(pstActStoaInd->ulObjId)),
           cpss_ntohl(pstActStoaInd->ulResult));
          
    stActStoaInd.ulEventNo = cpss_ntohl(pstActStoaInd->ulEventNo);
    stActStoaInd.ulEventSubNo = cpss_ntohl(pstActStoaInd->ulEventSubNo);
    stActStoaInd.stPhyAddr = pstActStoaInd->stPhyAddr;
    stActStoaInd.ulObjId = cpss_ntohl(pstActStoaInd->ulObjId);
    stActStoaInd.ulResult = cpss_ntohl(pstActStoaInd->ulResult);
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
    smss_send_alarm_brdfailed_to_oams(&stActStoaInd);
#else
    smss_send_alarm_cpufailed_to_oams(&stActStoaInd);
#endif
}
/***************************************************************************
* 函数名称: smss_sysctl_hard_cmp_time_handle
* 功    能: 硬件主备竞争定时器处理
* 函数类型  void
* 参    数: 
* 参数名称           参数类型               输入/输出       参数描述
* 无
* 函数返回:无
* 说    明:
***************************************************************************/
VOID  smss_sysctl_hard_cmp_time_handle(VOID)
{
#if defined(SMSS_AS_CMP_HARD)   /* 硬件主备竞争 */
    SMSS_PROC_ACTIVATE_REQ_MSG_T stActProc;
    UINT32 ulStatus = 0;

    drv_active_standby_get(&g_stSmssAsInfo.ucSelfAStatus); /* 获取本板的主备状态 */

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                   "(%d-%d-%d %d) get state %d.",
                   g_stSmssBaseInfo.stPhyAddr.ucFrame, g_stSmssBaseInfo.stPhyAddr.ucShelf,
			             g_stSmssBaseInfo.stPhyAddr.ucSlot, g_stSmssBaseInfo.stPhyAddr.ucCpu,
			             g_stSmssAsInfo.ucSelfAStatus);

    if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
    /* 若为主用板，设置面板灯 */
    {
        g_stSmssAsInfo.szSelfAStatus = "ACTIVE";
#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)    /* 若为主CPU，设置面板灯 */ 
        drv_board_led_set(DRV_LED_TYPE_ACTIVE, DRV_LED_STATU_ON);
        smss_devm_SetActled(1);           
#endif
    }
    else
    {
        g_stSmssAsInfo.szSelfAStatus = "STANDBY";            
    }
    /* 获取本板的逻辑地址 */
    cpss_com_logic_addr_get(&g_stSmssBaseInfo.stLogAddr, &ulStatus);
    
    /* 激活CPSS的通讯纤程，启动定时器等待其应答 */
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Send %s to Proc `%s'.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               "SMSS_PROC_ACTIVATE_REQ_MSG",
               cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulProcDes));

    stActProc.ucAsStatus = g_stSmssAsInfo.ucSelfAStatus;
    smss_sysctl_send_local(g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulProcDes,
                           SMSS_PROC_ACTIVATE_REQ_MSG, 
                           (UINT8*)&stActProc, sizeof(stActProc));   
    
    cpss_timer_set(SMSS_TIMER_IDLE, g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulWaitTime);
    g_ucSmssIdleStage = SMSS_IDLE_ACT_CPSS;
    g_ulSmssSendIndex = g_ulSmssSendIndex + 1;
#endif
}

/***************************************************************************
* 函数名称: smss_sysctl_force_reset
* 功    能: 在外围板上收到全局板的复位命令后进行相应处理。
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* pstMsg  SMSS_SYSCTL_forCE_RST_CMD_MSG_T *  IN         消息指针
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_force_reset(SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG_T * pstMsg)
{
    INT32  lRet;  
    SMSS_SYSCTL_FORCE_RST_CMD_RSP_MSG_T  stRstRsp;

    if (NULL == pstMsg)
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
        "<%s-%s>: In Function smss_sysctl_force_reset "
        "Receive Null Message.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        return;
    }
    pstMsg->ulResetReason = cpss_ntohl(pstMsg->ulResetReason);
    pstMsg->ulDelayTime = cpss_ntohl(pstMsg->ulDelayTime);
    pstMsg->ulResetType = cpss_ntohl(pstMsg->ulResetType);
    pstMsg->ulSeqId = cpss_ntohl(pstMsg->ulSeqId);

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "<%s-%s>: ResetType = %d , DelayTime = %d",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
        pstMsg->ulResetType, pstMsg->ulDelayTime);

    /* 如果延迟时间为0,直接复位 */
    if (0 == pstMsg->ulDelayTime)
    {
        lRet = SMSS_OK;            
        if (SMSS_FORCE_RST_ALL_SHELF == pstMsg->ulResetType)
        {
#ifdef SMSS_BOARD_WITH_SHM
            drv_ipmi_reset(0xff);
#endif
        }
        else if (SMSS_FORCE_RST_SPC_SLOT == pstMsg->ulResetType)
        {
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
            g_stSmssBaseInfo.ulResetReason = SMSS_BRD_RST_FORCE_RESET;
            cpss_reset(g_stSmssBaseInfo.ulResetReason);
#endif
        }      
        else if (SMSS_FORCE_RST_GLOBAL == pstMsg->ulResetType) /* 整网元复位 */
        {
        	#ifdef SMSS_BOARD_WITH_SHM
        	 drv_ipmi_reset(13);
        	 drv_ipmi_reset(1);
        	#endif
        }

        return;
    }

    /* 起定时器延时复位本框 */   
        if (SMSS_FORCE_RST_ALL_SHELF == pstMsg->ulResetType)
        {
#ifdef SMSS_BOARD_WITH_SHM
{
        UINT32 ulIndex;
        CPSS_COM_PHY_ADDR_T stPhy;
        /* LDT发起整网元复位消息, 单板复位后不需要上传黑匣子,          
          主用交换板收到此消息, 向本框内的单板发清黑匣子标志消息 */
        stPhy = g_stSmssBaseInfo.stPhyAddr;        
        for (ulIndex = 1; ulIndex < 15; ulIndex++)
        {
        	 stPhy.ucSlot = ulIndex;
           smss_com_send_phy(stPhy, SMSS_SYSCTL_PROC,
                SMSS_SYSCTL_CLEAR_SBBR_FLAG_MSG, NULL, 0);                
        }
        
        
            lRet = cpss_timer_set(SMSS_TIMER_FRAME_RST_TIMER,pstMsg->ulDelayTime*1000); 
        if (CPSS_OK != lRet) 
        {
            g_lSetTimerResult = lRet;
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                "<%s-%s>: Call cpss_timer_set, Result = 0x%x.",
                g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,lRet);
        }
}
#endif
        }
        else if (SMSS_FORCE_RST_SPC_SLOT == pstMsg->ulResetType)
        {
            lRet = cpss_timer_set(SMSS_TIMER_BRD_RST_TIMER,pstMsg->ulDelayTime*1000); 
            if (CPSS_OK != lRet) 
            {
              g_lSetTimerResult = lRet;
              cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                "<%s-%s>: Call cpss_timer_set, Result = 0x%x.",
                g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,lRet);
            }

            #if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
             #if defined(SMSS_BOARD_WITH_SLAVE)
               smss_send_to_slave(SMSS_SYSCTL_PROC,SMSS_SYSCTL_CLEAR_SBBR_FLAG_MSG,
                                  NULL,0);   
             #endif
            #endif           
        }
        else if (SMSS_FORCE_RST_GLOBAL == pstMsg->ulResetType) /* 整网元复位 */
        {
#ifdef SMSS_BOARD_WITH_SHM        	
            lRet = cpss_timer_set(SMSS_TIMER_GLOBAL_RST_TIMER, pstMsg->ulDelayTime * 1000); 
		        if (CPSS_OK != lRet) 
		        {
		            g_lSetTimerResult = lRet;
		            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
		                "<%s-%s>: Call cpss_timer_set,time %d, Result = 0x%x.",
		                g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
		                pstMsg->ulDelayTime * 1000,lRet);
		        }        	
#endif		        
        }

    if(0 == pstMsg->ulResetReason)
    {
        return;
    }
    /* 向全局板响应 */
    stRstRsp.ulSeqId = cpss_htonl(pstMsg->ulSeqId);
    stRstRsp.stPhyAddr = pstMsg->stPhyAddr;
    stRstRsp.ulResetType = cpss_htonl(pstMsg->ulResetType);  
    stRstRsp.ulReturnCode = cpss_htonl(SMSS_OK);   
    
    smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_FORCE_RST_CMD_RSP_MSG,
        (UINT8 *)&stRstRsp, sizeof(stRstRsp));

    
    return;
} 

/***************************************************************************
* 函数名称: smss_sysctl_record_heart
* 功    能: 用于记录心跳定时器的相关信息，以便于定位心跳丢失的问题
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* pstHeartInfo  SMSS_HEART_BRD_T          *  IN         消息指针
* ucFlag        UINT8                       IN          标志
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/

VOID smss_sysctl_record_heart(SMSS_HEART_BRD_T *pstHeartInfo,UINT8 ucFlag,UINT32 ulHeartSeq)
{
#if(SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE )
    if (SMSS_RECV_HEART==ucFlag)/* 记录接收到的心跳计数 */
    {
        if (pstHeartInfo->ulRecvHBCnt>=SMSS_HEART_RECORD_MAX_CNT)
        {
            pstHeartInfo->ulRecvHBCnt = 0;
        }
        pstHeartInfo->stRecvHB[pstHeartInfo->ulRecvHBCnt].ultickcnt = cpss_tick_get();
        pstHeartInfo->stRecvHB[pstHeartInfo->ulRecvHBCnt].ulRecvSeq = ulHeartSeq;
        cpss_clock_get(&(pstHeartInfo->stRecvHB[pstHeartInfo->ulRecvHBCnt].stTimer));
        pstHeartInfo->ulRecvHBCnt ++;
    }
    else
    {
        if (pstHeartInfo->ulSendHBCnt>=SMSS_HEART_RECORD_MAX_CNT)
        {
            pstHeartInfo->ulSendHBCnt = 0;
        }
        pstHeartInfo->stSendHB[pstHeartInfo->ulSendHBCnt].ultickcnt = cpss_tick_get();
        pstHeartInfo->stSendHB[pstHeartInfo->ulSendHBCnt].ulSendSeq = ulHeartSeq;
        cpss_clock_get(&(pstHeartInfo->stSendHB[pstHeartInfo->ulSendHBCnt].stTimer));
        pstHeartInfo->ulSendHBCnt ++;
    }
#endif
}

/***************************************************************************
* 函数名称: smss_sysctl_show_heartinfo
* 功    能: 用于记录心跳定时器的相关信息，以便于定位心跳丢失的问题
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_show_heartinfo(VOID)
{
    UINT32     ulCount;
    cps__oams_shcmd_printf("\nFrame   Shelf    Slot      RecvDATE     recvseq     recvTicks    SendDate    SendSeq    sendTick\n");
    for( ulCount =0;ulCount<SMSS_HEART_RECORD_MAX_CNT;ulCount++)
    {
        cps__oams_shcmd_printf("%5d%8d%8d   (%2d:%2d:%2d)%10d%15d     (%2d:%2d:%2d)%10d%10d%\n",
            g_stSmssBaseInfo.stPhyAddr.ucFrame,
            g_stSmssBaseInfo.stPhyAddr.ucShelf,
            g_stSmssBaseInfo.stPhyAddr.ucSlot,
            g_stSmssBaseInfo.stHeartInfo.stRecvHB[ulCount].stTimer.ucHour,
            g_stSmssBaseInfo.stHeartInfo.stRecvHB[ulCount].stTimer.ucMinute,
            g_stSmssBaseInfo.stHeartInfo.stRecvHB[ulCount].stTimer.ucSecond,
            g_stSmssBaseInfo.stHeartInfo.stRecvHB[ulCount].ulRecvSeq,
            g_stSmssBaseInfo.stHeartInfo.stRecvHB[ulCount].ultickcnt,
            g_stSmssBaseInfo.stHeartInfo.stSendHB[ulCount].stTimer.ucHour,
            g_stSmssBaseInfo.stHeartInfo.stSendHB[ulCount].stTimer.ucMinute,
            g_stSmssBaseInfo.stHeartInfo.stSendHB[ulCount].stTimer.ucSecond,
            g_stSmssBaseInfo.stHeartInfo.stSendHB[ulCount].ulSendSeq,
            g_stSmssBaseInfo.stHeartInfo.stRecvHB[ulCount].ultickcnt
            );
    }
}

/***************************************************************************
* 函数名称: smss_show_heartinfo
* 功    能: 显示上下级的心跳收发信息，全局板上显示所有到所有单板的信息 
            外围板上显示到全局单板心跳的信息
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_show_heartinfo(VOID)
{
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL
  smss_sysctl_show_gcpa_heartinfo();
#else
  smss_sysctl_show_heartinfo();
#endif
}


/***************************************************************************
* 函数名称: smss_show_timer_info
* 功    能: 显示上下级心跳的定时器信息
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_show_timer_info(VOID)
{
   UINT32     ulCount;
    cps__oams_shcmd_printf("\nFrame   Shelf    Slot      RecvDATE     recvTicks\n");
    for( ulCount =0;ulCount<SMSS_HEART_RECORD_MAX_CNT;ulCount++)
    {
        cps__oams_shcmd_printf("%5d%8d%8d   (%2d:%2d:%2d)%15d\n",
            g_stSmssBaseInfo.stPhyAddr.ucFrame,
            g_stSmssBaseInfo.stPhyAddr.ucShelf,
            g_stSmssBaseInfo.stPhyAddr.ucSlot,
            g_stSmssHBTimerInfo.stRecvHB[ulCount].stTimer.ucHour,
            g_stSmssHBTimerInfo.stRecvHB[ulCount].stTimer.ucMinute,
            g_stSmssHBTimerInfo.stRecvHB[ulCount].stTimer.ucSecond,
            g_stSmssHBTimerInfo.stRecvHB[ulCount].ultickcnt
            );
    }
}

#if (SWP_CPUROLE_DSP != SWP_CPUROLE_TYPE)    
INT32 smss_send_me_status(UINT32 ulMeID,UINT32 ulMeState)
{
	  UINT32 ulArgs[4];
    SMSS_RESET_SLAVE_CMD_MSG_T stResetSlave;

    /* 如果ME状态正常，直接返回 */
    if (0 == ulMeState)
    {
        return SMSS_OK;
    }
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "smss_send_me_status ulMeID = 0x%x, ulMeState = 0x%x",ulMeID, ulMeState);
        
    /* 写黑匣子记录复位原因 */    
    cpss_sbbr_write_text(0, ulArgs,"smss_send_me_status ulMeID: 0x%x, ulMeState: 0x%x\n\r",ulMeID, ulMeState);
    stResetSlave.ulMeID = cpss_htonl(ulMeID);
    stResetSlave.ulMeState = cpss_htonl(ulMeState);
    
    #if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)
      smss_sysctl_enter_end(FALSE, SMSS_BRD_RST_CAUSE_ME_FAIL);
    #elif (SWP_CPUROLE_SLAVE == SWP_CPUROLE_TYPE)   /* 通知主CPU上的SYSCTL复位从CPU */
      smss_sysctl_send_host(SMSS_SYSCTL_PROC, SMSS_RESET_SLAVE_CMD_MSG, 
        (UINT8 *)&stResetSlave, sizeof(SMSS_RESET_SLAVE_CMD_MSG_T));
    #endif    
    return SMSS_OK;
}
#endif

VOID smss_sysctl_oosled_set(UINT8 ucLedState)
{
    INT32 lResult = 0;
#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)  /* 如果为主CPU，设置面板灯 */
    lResult = drv_ipmi_set_fru_led(ucLedState);
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
        "Call drv_ipmi_set_fru_led Set OOS State = %d, Result = %d",ucLedState,lResult);
#endif
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_recv_devmreportfault
* 功    能: 主控模块收到设备监控模块的严重告警后,向全局板(/和)伙伴板报
* 函数类型  VOID
* 参    数: 无。
* 函数返回:
*           无。
*说     明: 该功能在单板初始化阶段和正常运行阶段都存在.在主备倒换阶段,如果收到
*           该消息,直接复位。
***************************************************************************/
VOID smss_sysctl_cpufaultind_msgdisp(CPSS_COM_MSG_HEAD_T *pstMsg)
{
    SMSS_CPU_FAULT_IND_MSG_T *pstCpuFaultInd;

    pstCpuFaultInd = (SMSS_CPU_FAULT_IND_MSG_T*)(pstMsg->pucBuf);

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "<%s-%s>: Received message %s, ulReason = %d.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
        "SMSS_CPU_FAULT_IND_MSG",pstCpuFaultInd->ulReason);
    
#ifndef SWP_FNBLK_BRDTYPE_MAIN_CTRL
    /* 如果不是全局板，向上级CPU报告 */
    {
        SMSS_SYSCTL_BFAULT_IND_MSG_T stBFault;
        
        stBFault.ulReason = pstCpuFaultInd->ulReason;
        stBFault.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
        
        if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)
            /* 若为主CPU，向全局板报故障 */
        {
            stBFault.ulType = SMSS_BFAULT_HOST_TO_GCPA;
            stBFault.ulType = cpss_htonl(stBFault.ulType);
            smss_sysctl_send_gcpa(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BFAULT_IND_MSG,
                (UINT8 *)&stBFault, sizeof(stBFault));
            
        }
        else /* 若为从CPU，向主CPU报故障 */
        {
            stBFault.ulType = SMSS_BFAULT_SLAVE_TO_HOST;
            stBFault.ulType = cpss_htonl(stBFault.ulType);
            smss_sysctl_send_host(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BFAULT_IND_MSG,
                (UINT8 *)&stBFault, sizeof(stBFault));
        }
    }
#endif    /* 非全局板 */
    
#ifdef SMSS_BACKUP_MATE
    if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
        /* 若为主用板，则向备用板发倒换请求 */
    {
        SMSS_SYSCTL_BSWITCH_REQ_MSG_T stBSwitchReq;
#ifdef SMSS_AS_CMP_HARD
        drv_active_standby_set(SMSS_STANDBY);
#endif
        g_stSmssAsInfo.stSwitchInfo.ulReason = pstCpuFaultInd->ulReason;

        stBSwitchReq.ulReason = cpss_htonl(SMSS_BSWITCH_MATE_FAULT);

        smss_com_send_mate(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BSWITCH_REQ_MSG,
            (UINT8 *)&stBSwitchReq, sizeof(stBSwitchReq));
    }
    else if (SMSS_STANDBY == g_stSmssAsInfo.ucSelfAStatus)
        /* 若为备用板，向主用板发故障指示 */
    {
        SMSS_SYSCTL_BFAULT_IND_MSG_T stBFault;
        stBFault.ulType = SMSS_BFAULT_MATE_TO_MATE;
        stBFault.ulReason = pstCpuFaultInd->ulReason;
        stBFault.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
        stBFault.ulType = cpss_htonl(stBFault.ulType);
        stBFault.ulReason = cpss_htonl(stBFault.ulReason);
        smss_com_send_mate(SMSS_SYSCTL_PROC, SMSS_SYSCTL_BFAULT_IND_MSG,
            (UINT8 *)&stBFault, sizeof(stBFault));
    }
#endif
    
    if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
    {
        /* 若为主 ，进入END态,延时复位 */
        smss_sysctl_enter_end(FALSE, SMSS_BRD_RST_CAUSE_RECV_DEVM_BRD_FAULT);
    }
    else
    {                                                                                    
        smss_sysctl_enter_end(FALSE, SMSS_BRD_RST_CAUSE_RECV_DEVM_BRD_FAULT);    /* 若为备，延时复位 */
    }
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_lost_gcpahb_handle
* 功    能: SYSCTL接收到SYSRT纤程的全局板心跳丢失的处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
void smss_sysctl_lost_gcpahb_handle(void)
{
   /* 设置自己的状态，复位 */
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: Receive Message: SMSS_SYSCTL_LOST_GCPAHB_MSG.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus ); 
    smss_sysctl_enter_end(TRUE,
        SMSS_BRD_RST_CAUSE_LOST_HEARTBEAT_GCPA);
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
               "<%s-%s>: Lost GCPA heartbeat, transit to state <%s>.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, "END");
}
#if defined(SMSS_BACKUP_MATE)   /* 存在伙伴板 */

#define SMSS_HB_MATE            /* 与伙伴板有心跳 */

UINT8 g_ucHbMateLost = 0;   /* 来自伙伴板的心跳连续缺失计数 */

UINT8 g_ucAStatus = SMSS_UNKNOWN_AS_STATUS;   /* 主备状态未知 */
/***************************************************************************
* 函数名称: smss_hb_mate_timeout
* 功    能: 伙伴板心跳超时的处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_hb_mate_timeout(VOID)
{
    SMSS_HEARTBEAT_MSG_T stHbMsg; /* 心跳消息结构变量 */
    stHbMsg.ulSeqNum = cpss_htonl(g_ulHbMateSeqNum); /* 序列号 */
    stHbMsg.ulAStatus = cpss_htonl(g_stSmssAsInfo.ucSelfAStatus);

    if (FALSE == g_stSmssAsInfo.bMateExist                      /* 不存在伙伴板 */
        || SMSS_RSTATUS_NORMAL != g_stSmssAsInfo.ucMateRStatus) /* 伙伴板状态非正常 */
    {
        return;
    }
    
    if (FALSE != g_bHbMonitor                      /* 监测心跳 */
        && g_ucHbMateLost <= g_ucHbMateLostMax)    /* 未超过心跳缺失上限 */
    {
        g_ucHbMateLost++;

        if (g_ucHbMateLost > CPSS_DIV(g_ucHbMateLostMax , 2))
        {
            smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_WARN,
                       "<%010d> %c: Lost MATE heartbeat %d times.",
                       tickGet(), SMSS_ACTIVE == g_ucAStatus ? 'A' : 'S',
                       g_ucHbMateLost - 1);
        }

        if (g_ucHbMateLost > g_ucHbMateLostMax)
        {        	  
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                       "Lost MATE heartbeat, matePhy (%d-%d-%d)!!!",
                       g_stSmssAsInfo.stMatePhyAddr.ucFrame,
                       g_stSmssAsInfo.stMatePhyAddr.ucShelf,
                       g_stSmssAsInfo.stMatePhyAddr.ucSlot);

            smss_sysctl_write_sbbr("Lost MATE heartbeat, matePhy (%d-%d-%d)!!!",
                       g_stSmssAsInfo.stMatePhyAddr.ucFrame,
                       g_stSmssAsInfo.stMatePhyAddr.ucShelf,
                       g_stSmssAsInfo.stMatePhyAddr.ucSlot); 
          
            /* 检测到来自伙伴板的心跳连续缺失，通知SYSCTL伙伴板故障。 */
            /* 模拟硬件主备时驱动上报的消息 */
            smss_send_to_local(SMSS_SYSCTL_PROC, SMSS_SYSCTL_LOST_MATEHB_MSG,
                               NULL, 0);
#if (SWP_PHYBRD_NSCA == SWP_PHYBRD_TYPE)
		    if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus) /* 主用NSCA板 */
			{
				SMSS_DEVM_NSCA_SYN_CLKSTATE_IND_MSG_T stClkState;
				
				stClkState.iClkState = DRV_BSP_ERROR; /* 通过心跳检测到备用NSCA板故障状态，即认为对端时钟状态故障。 */

				cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
					"clk state,ClkState: %d", stClkState.iClkState);
				/* 向本板DEVM发送时钟状态指示消息 */
				stClkState.iClkState = cpss_htonl(stClkState.iClkState);

				smss_send_to_local(SMSS_DEVM_PROC, SMSS_DEVM_NSCA_SYN_CLKSTATE_IND_MSG,
                               (UINT8 *)&stClkState, sizeof(SMSS_DEVM_NSCA_SYN_CLKSTATE_IND_MSG_T));
				
				cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
					"send SMSS_DEVM_NSCA_SYN_CLKSTATE_IND_MSG to local board.");
			}
#endif
            return;
        }
    }

    /* 向伙伴板发送心跳消息 */
    if (g_ucHbMateLost <= g_ucHbMateLostMax)
    {
        cpss_com_send_mate(SMSS_SYSRT_PROC, SMSS_HEARTBEAT_MATE_MSG,
                           (UINT8 *)&stHbMsg, sizeof(stHbMsg));

        smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
                   "<%010d> %c: Send heartbeat %d to MATE, lost %d.",
                   tickGet(), SMSS_ACTIVE == g_ucAStatus ? 'A' : 'S',
                   g_ulHbMateSeqNum, g_ucHbMateLost - 1);
    }
    g_ulHbMateSeqNum++;
    return;
}
/***************************************************************************
* 函数名称: smss_hb_mate_received
* 功    能: 伙伴板心跳接收的处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_hb_mate_received(SMSS_HEARTBEAT_MSG_T *pstMsg)
{	  
    g_ucHbMateLost = 0;
    g_ulHbMateTime++;

    /* 双主双备处理 */      
    if (cpss_ntohl(pstMsg->ulAStatus) == g_stSmssAsInfo.ucSelfAStatus)
    {
    	smss_sysctl_enter_end(FALSE, SMSS_BRD_RST_CAUSE_ALLACTIVE);
    	return; 
    }    	

    g_stSmssAsInfo.ucMateRStatus = SMSS_RSTATUS_NORMAL;

	if (1 == g_ulHbMateTime) /* 接收到心跳消息 */
	{
#if (SWP_PHYBRD_NSCA == SWP_PHYBRD_TYPE)
		if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus) /* 主用NSCA板 */
		{
			SMSS_DEVM_NSCA_SYN_CLKSTATE_IND_MSG_T stClkState;
			
			stClkState.iClkState = DRV_BSP_OK; /* 通过心跳检测到备用NSCA板正常状态，即认为对端时钟状态为正常。 */
			
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
				"clk state,ClkState: %d", stClkState.iClkState);
			/* 向本板DEVM发送时钟状态指示消息 */
			stClkState.iClkState = cpss_htonl(stClkState.iClkState);
			
			smss_send_to_local(SMSS_DEVM_PROC, SMSS_DEVM_NSCA_SYN_CLKSTATE_IND_MSG,
                               (UINT8 *)&stClkState, sizeof(SMSS_DEVM_NSCA_SYN_CLKSTATE_IND_MSG_T));
			
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
					"send SMSS_DEVM_NSCA_SYN_CLKSTATE_IND_MSG to local board.");
		}
#endif
	}

	if (3 <= g_ulHbMateTime)
	{
		g_ulHbMateTime = 3;
	}

    smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
               "<%010d> %c: Received heartbeat %d from MATE.",
               tickGet(), SMSS_ACTIVE == g_ucAStatus ? 'A' : 'S',
               cpss_ntohl(pstMsg->ulSeqNum));
    return;
}
#endif /* 存在伙伴板 */

#if defined(SWP_FNBLK_BRDTYPE_MAIN_CTRL) /* 全局板 */

#define SMSS_HB_DOWN            /* 与下级有心跳 */

/* 保存各外围板的心跳缺失计数的数组 */
UINT8 g_aucHbDownLost[CPSS_COM_MAX_FRAME][CPSS_COM_MAX_SHELF][CPSS_COM_MAX_SLOT];

extern SMSS_BOARD_INFO_T  ** apstBoardIndex;

#define smss_hb_down_timeout() smss_gcpa_hb_down_timeout()
/***************************************************************************
* 函数名称: smss_gcpa_hb_down_timeout
* 功    能: 全局板心跳检测的处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_gcpa_hb_down_timeout(VOID)
{
    INT32 i, j, k;
    CPSS_COM_PHY_ADDR_T stPhyAddr; /* 外围板物理地址 */
    SMSS_HEARTBEAT_MSG_T stHbMsg; /* 心跳消息结构变量 */
    SMSS_BOARD_INFO_T * pstBrdInfo = NULL;

    UINT8 ucDownLostMax;
    stPhyAddr.ucCpu = SMSS_CPUNO_HOST; /* 外围板主CPU */
    stHbMsg.ulSeqNum = cpss_htonl(g_ulHbDownSeqNum); /* 序列号 */
    stHbMsg.ulAStatus = cpss_htonl(g_ucAStatus);

    
    for (i = 0; i < CPSS_COM_MAX_FRAME; i++)
    {
        stPhyAddr.ucFrame = i + 1;         /* 外围板架号 */

        for (j = 0; j < CPSS_COM_MAX_SHELF; j++)
        {
            stPhyAddr.ucShelf = j + 1;     /* 外围板框号 */

            for (k = 0; k < CPSS_COM_MAX_SLOT; k++)
            {
                int iIndex;

                stPhyAddr.ucSlot = k + 1;  /* 外围板槽号 */

                if ((TRUE == SMSS_ADDR_IS_EQUAL(stPhyAddr, CPSS_PHY_ADDR_GCPA_MASTER))
                    || (TRUE == SMSS_ADDR_IS_EQUAL(stPhyAddr, CPSS_PHY_ADDR_GCPA_SLAVE)))
                    /* 全局板不向自已和伙伴板发下级心跳 */
                {
                    continue;
                }

                iIndex = Phy2BoardIndex(stPhyAddr);
                pstBrdInfo = apstBoardIndex[iIndex/*Phy2BoardIndex(stPhyAddr)*/];
                if (SMSS_ACTIVE == g_ucAStatus
                    && NULL == pstBrdInfo)
                {
                    continue;
                }
                       
                ucDownLostMax = g_ucHbDownLostMax;
                
                if (FALSE != g_bHbMonitor                        /* 监测心跳 */
                    && (SMSS_STANDBY == g_ucAStatus ||           /* 本板为备用全局板 */
                        SMSS_RSTATUS_NORMAL == pstBrdInfo->ucRStatus) /* 外围板正常 */
                    && g_aucHbDownLost[i][j][k] <= ucDownLostMax) /* 未超过心跳缺失上限 */
                {
                    g_aucHbDownLost[i][j][k]++;

                    if (g_aucHbDownLost[i][j][k] > CPSS_DIV(ucDownLostMax , 2))
                    {
                        smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_WARN,
                                   "<%010d> Lost heartbeat of board (%d-%d-%d) %d times.",
                                   tickGet(), stPhyAddr.ucFrame, stPhyAddr.ucShelf,
                                   stPhyAddr.ucSlot, g_aucHbDownLost[i][j][k] - 1);
                    }

                    if (g_aucHbDownLost[i][j][k] > ucDownLostMax)
                    {
                        SMSS_SYSCTL_LOST_HOSTHB_T stMsg;

                        
                        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,

                                   "Lost heartbeat of board (%d-%d-%d)!!!",
                                   stPhyAddr.ucFrame, stPhyAddr.ucShelf, stPhyAddr.ucSlot);
                                  
                        stMsg.stPhyAddr = stPhyAddr;
                        /* 检测到来自外围板的心跳连续缺失，通知SYSCTL该单板故障 */
                        smss_sysctl_send_local(SMSS_SYSCTL_PROC,SMSS_SYSCTL_LOST_HOSTHB_MSG,(UINT8*)&stMsg,
                            sizeof(stMsg));
                        smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                                   "send SMSS_SYSCTL_LOST_HOSTHB_MSG to SYSCTL board (%d-%d-%d)!!!",
                                   stPhyAddr.ucFrame, stPhyAddr.ucShelf, stPhyAddr.ucSlot);
                        continue;
                    }
                }

                /* 向该外围板发送心跳消息 */
                if (g_aucHbDownLost[i][j][k] <= ucDownLostMax /* 未超过心跳缺失上限 */
                    && (SMSS_STANDBY == g_ucAStatus ||            /* 本板为备用全局板 */
                        SMSS_RSTATUS_NORMAL == pstBrdInfo->ucRStatus)) /* 外围板正常 */
                {
                    cpss_com_send_phy(stPhyAddr, SMSS_SYSRT_PROC,
                                      SMSS_HEARTBEAT_DOWN_MSG, (UINT8 *)&stHbMsg, sizeof(stHbMsg));

                    smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
                               "<%010d> %c: Send heartbeat %d to board (%d-%d-%d), lost %d.",
                               tickGet(), SMSS_ACTIVE == g_ucAStatus ? 'A' : 'S',
                               g_ulHbDownSeqNum, stPhyAddr.ucFrame, stPhyAddr.ucShelf,
                               stPhyAddr.ucSlot, g_aucHbDownLost[i][j][k] - 1);
                }
            }
        }
    }
    g_ulHbDownSeqNum++;
    return;
}

#define smss_hb_up_received(a)          smss_gcpa_hb_up_received(a)
/***************************************************************************
* 函数名称: smss_gcpa_hb_up_received
* 功    能: 全局板心跳接收的处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_gcpa_hb_up_received(SMSS_HEARTBEAT_MSG_T *pstMsg)
{
    UINT32 ulFlag;
    UINT8  ucAStatus;
    UINT8  ucRStatus;
    UINT8  ucMStatus;
    INT32  lRet;

    if (g_aucHbDownLost[pstMsg->stPhyAddr.ucFrame-1][pstMsg->stPhyAddr.ucShelf-1][pstMsg->stPhyAddr.ucSlot-1] 
        > g_ucHbDownLostMax) 
    {

    	  if (g_stSmssAsInfo.ucSelfAStatus == SMSS_ACTIVE)
    	  {
	    	  ulFlag = cpss_ntohl(pstMsg->ulFlag);
	        if ((ulFlag == 0) && (g_stSmssBaseInfo.ucRStatus == SMSS_RSTATUS_NORMAL)) 
	        {
	          smss_sysctl_update_board_info(pstMsg->stPhyAddr);
	        }
        }
         
	      lRet = smss_sysctrl_get_brd_state_byphy(pstMsg->stPhyAddr, &ucAStatus, &ucRStatus, &ucMStatus);
	      if ((SMSS_OK == lRet) && (SMSS_RSTATUS_NORMAL != ucRStatus))
	      {
	      	return; 
	      }

            smss_send_alarm_brdnotresponded_to_oams(SMSS_ALMSUBNO_0, 0, pstMsg->stPhyAddr);
            smss_send_alarm_brdnotresponded_to_oams(SMSS_ALMSUBNO_1, 0, pstMsg->stPhyAddr);    
            smss_sysctl_send_alarm_clean_to_oams(pstMsg->stPhyAddr);           
    }

    g_aucHbDownLost[pstMsg->stPhyAddr.ucFrame-1][pstMsg->stPhyAddr.ucShelf-1][pstMsg->stPhyAddr.ucSlot-1] = 0;
    
    smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
               "<%010d> %c: Received heartbeat %d from board (%d-%d-%d).",
               tickGet(), SMSS_ACTIVE == g_ucAStatus ? 'A' : 'S',
               cpss_ntohl(pstMsg->ulSeqNum), pstMsg->stPhyAddr.ucFrame,
               pstMsg->stPhyAddr.ucShelf, pstMsg->stPhyAddr.ucSlot);


    if (0 != g_stSmssBaseInfo.ulHeartBeatSndSeq)
    {
       if ((((g_stSmssBaseInfo.ulHeartBeatSndSeq >> 24) & 0xff) == pstMsg->stPhyAddr.ucFrame) &&
           (((g_stSmssBaseInfo.ulHeartBeatSndSeq >> 16) & 0xff) == pstMsg->stPhyAddr.ucShelf) &&
           (((g_stSmssBaseInfo.ulHeartBeatSndSeq >> 8) & 0xff) == pstMsg->stPhyAddr.ucSlot))
        {
           cps__oams_shcmd_printf("<%010d> %c: Received heartbeat %d from board (%d-%d-%d).\n",
                           tickGet(), SMSS_ACTIVE == g_ucAStatus ? 'A' : 'S',
                           cpss_ntohl(pstMsg->ulSeqNum), pstMsg->stPhyAddr.ucFrame,
                            pstMsg->stPhyAddr.ucShelf, pstMsg->stPhyAddr.ucSlot);	
        }      	
    }

    if (g_stSmssAsInfo.ucSelfAStatus != SMSS_ACTIVE) 
    {
        return;
    }

    ulFlag = cpss_ntohl(pstMsg->ulFlag);
    if ((ulFlag == 0) && (g_stSmssBaseInfo.ucRStatus == SMSS_RSTATUS_NORMAL)) 
    {
        smss_sysctl_update_board_info(pstMsg->stPhyAddr);
    }
    return;
}

/***************************************************************************
* 函数名称: smss_showheart
* 功    能: 显示全局板记录的外围板的心跳信息
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* ucFrame         UINT8                   IN              架
* ucShelf         UINT8                   IN              框
* ucSlot          UINT8                   IN              槽
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_showheart(UINT8 ucFrame, UINT8 ucShelf, UINT8 ucSlot)
{
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL
  INT32 iIndex;
  CPSS_COM_PHY_ADDR_T stPhyAddr;
  SMSS_BOARD_INFO_T * pstBrdInfo = NULL;
  
  stPhyAddr.ucFrame = ucFrame;
  stPhyAddr.ucShelf = ucShelf;
  stPhyAddr.ucSlot =  ucSlot;
  
  iIndex = Phy2BoardIndex(stPhyAddr);
  pstBrdInfo = apstBoardIndex[iIndex];
  
  if (NULL == pstBrdInfo)
  {
    cps__oams_shcmd_printf("(%d-%d-%d) pstBrdInfo is NULL\n",ucFrame,
                      ucShelf, ucSlot);	
    return;                  
  }
  
  cps__oams_shcmd_printf("(%d-%d-%d) heart info:\n", ucFrame,
                      ucShelf, ucSlot);                   
  cps__oams_shcmd_printf("gcpa_AStatus %d, pstBrdInfo->ucRStatus %d, heart lost %d, alarmSub[0] %d, alarmSub[1] %d\n", 
                   g_ucAStatus, pstBrdInfo->ucRStatus, 
                   g_aucHbDownLost[ucFrame -1 ][ucShelf -1 ][ucSlot -1 ],
                   pstBrdInfo->aucRsv[0], pstBrdInfo->aucRsv[1]);              
  return; 
#endif
}

#else  /* 外围板 */
#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE) /* 主CPU */

#define SMSS_HB_UP              /* 与上级有心跳 */

/* 来自全局板的心跳连续缺失计数 */
UINT8 g_ucHbUpActiveLost = 0;   /* 主用全局板 */
UINT8 g_ucHbUpStandbyLost = 0;  /* 备用全局板 */

#define smss_hb_up_timeout() smss_host_hb_up_timeout()
/***************************************************************************
* 函数名称: smss_gcpa_hb_up_received
* 功    能: 外围板主CPU心跳超时的处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_host_hb_up_timeout(VOID)
{
    SMSS_HEARTBEAT_MSG_T stHbMsg;
    stHbMsg.stPhyAddr = g_stSmssBaseInfo.stPhyAddr; /* 心跳消息结构变量 */
    stHbMsg.ulSeqNum = cpss_htonl(g_ulHbUpSeqNum); /* 序列号 */
#ifdef SMSS_BOARD_WITHOUT_RDBS
    stHbMsg.ulFlag = cpss_htonl(0);
#else
    stHbMsg.ulFlag = cpss_htonl(1);
#endif

    if (FALSE != g_bHbMonitor)                /* 监测心跳 */
    {
        /* 与备用全局板的心跳 */
        if (g_ucHbUpStandbyLost <= g_ucHbUpLostMax)    /* 未超过心跳缺失上限 */
        {
            g_ucHbUpStandbyLost++;
            if (g_ucHbUpStandbyLost > CPSS_DIV(g_ucHbUpLostMax , 2))
            {
                smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_WARN,
                           "<%010d> Lost GCPA (S) heartbeat %d times.",
                           tickGet(), g_ucHbUpStandbyLost - 1);
            }
            if (g_ucHbUpStandbyLost > g_ucHbUpLostMax)
            {
                smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                           "Lost GCPA (S) heartbeat!!!");
            }
        }

        /* 与主用全局板的心跳 */       
#if 0        
        if (g_ucHbUpActiveLost <= g_ucHbUpLostMax * 3)    /* 未超过心跳缺失上限 */
#endif        
        {
            g_ucHbUpActiveLost++;

            if (g_ucHbUpActiveLost > CPSS_DIV(g_ucHbUpLostMax , 2))
            {
                smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_WARN,
                           "<%010d> Lost GCPA (A) heartbeat %d times.",
                           tickGet(), g_ucHbUpActiveLost - 1);
            }

            if (g_ucHbUpActiveLost >
                (g_ucHbUpStandbyLost > g_ucHbUpLostMax ? g_ucHbUpLostMax
                 : g_ucHbUpLostMax * 3))
            {                     	  
                cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                           "Lost GCPA (A) heartbeat, selfPhy (%d-%d-%d)!!!",
                           g_stSmssBaseInfo.stPhyAddr.ucFrame,
                           g_stSmssBaseInfo.stPhyAddr.ucShelf,
                           g_stSmssBaseInfo.stPhyAddr.ucSlot);
#ifndef SMSS_BOARD_WITHOUT_RDBS
                /* 调用函数直接处理 */
                if (g_stSmssBaseInfo.ucRStatus != SMSS_RSTATUS_END)
                {

                	smss_sysctl_write_sbbr("Lost GCPA (A) heartbeat, selfPhy (%d-%d-%d)!!!",
                           g_stSmssBaseInfo.stPhyAddr.ucFrame,
                           g_stSmssBaseInfo.stPhyAddr.ucShelf,
                           g_stSmssBaseInfo.stPhyAddr.ucSlot);

                  smss_sysctl_enter_end(FALSE, SMSS_BRD_RST_CAUSE_LOST_HEARTBEAT_GCPA);
                }
                #if 0                                
                /* 检测到来自全局板的心跳连续缺失，通知SYSCTL。 */
                smss_sysctl_send_local(SMSS_SYSCTL_PROC,SMSS_SYSCTL_LOST_GCPAHB_MSG,NULL,
                            0);
                smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                           "Send SMSS_SYSCTL_LOST_GCPAHB_MSG to SYSCTL");
                #endif
         
                return;              
#endif
            }
        }
    }
    
    /* 向主用全局板发送心跳消息 */
    cpss_com_send_phy(CPSS_PHY_ADDR_GCPA_MASTER, SMSS_SYSRT_PROC,
                      SMSS_HEARTBEAT_UP_MSG, (UINT8 *)&stHbMsg, sizeof(stHbMsg));
    /* 向备用全局板发送心跳消息 */
    cpss_com_send_phy(CPSS_PHY_ADDR_GCPA_SLAVE, SMSS_SYSRT_PROC,
                      SMSS_HEARTBEAT_UP_MSG, (UINT8 *)&stHbMsg, sizeof(stHbMsg));

    smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
               "<%010d> Send heartbeat %d to GCPA, lost %d(A) %d(S).",
               tickGet(), g_ulHbUpSeqNum,
               g_ucHbUpActiveLost - 1, g_ucHbUpStandbyLost - 1);

    g_ulHbUpSeqNum++;
    return;
}
#define smss_hb_down_received(a)        smss_host_hb_down_received(a)
/***************************************************************************
* 函数名称: smss_host_hb_down_received
* 功    能: 外围板主CPU心跳接收的处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_host_hb_down_received(SMSS_HEARTBEAT_MSG_T *pstMsg)
{
    if (SMSS_ACTIVE == pstMsg->ulAStatus) /* 主用全局板 (SMSS_ACTIVE为0，无需ntohl) */
    {
        g_ucHbUpActiveLost = 0;

        smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
                   "<%010d> Received heartbeat %d from GCPA (A).",
                   tickGet(), cpss_ntohl(pstMsg->ulSeqNum));
    }
    else                        /* 备用全局板 */
    {
        g_ucHbUpStandbyLost = 0;

        smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
                   "<%010d> Received heartbeat %d from GCPA (S).",
                   tickGet(), cpss_ntohl(pstMsg->ulSeqNum));
    }
    return;
}

#if defined(SMSS_BOARD_WITH_SLAVE) /* 带从CPU */

#define SMSS_HB_DOWN            /* 与下级有心跳 */

/* 来自从CPU的心跳连续缺失计数 */
UINT8 g_ucHbDownLost = SMSS_HB_DOWN_LOST_MAX + 1;

#define smss_hb_down_timeout() smss_host_hb_down_timeout()
/***************************************************************************
* 函数名称: smss_host_hb_down_timeout
* 功    能: 外围板主CPU下级心跳超时处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_host_hb_down_timeout(VOID)
{
    if (FALSE != g_bHbMonitor                   /* 监测心跳 */
        && g_ucHbDownLost <= g_ucHbDownLostMax) /* 未超过心跳缺失上限 */
    {
        g_ucHbDownLost++;

        if (g_ucHbDownLost > CPSS_DIV(g_ucHbDownLostMax , 2))
        {
            smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_WARN,
                       "<%010d> Lost SLAVE heartbeat %d times.",
                       tickGet(), g_ucHbDownLost);
        }

        if (g_ucHbDownLost > g_ucHbDownLostMax)
        {        	  
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                       "Lost SLAVE heartbeat selfPhy (%d-%d-%d)!!!",
                           g_stSmssBaseInfo.stPhyAddr.ucFrame,
                           g_stSmssBaseInfo.stPhyAddr.ucShelf,
                           g_stSmssBaseInfo.stPhyAddr.ucSlot);

            smss_sysctl_write_sbbr("Lost SLAVE heartbeat selfPhy (%d-%d-%d)!!!",
                           g_stSmssBaseInfo.stPhyAddr.ucFrame,
                           g_stSmssBaseInfo.stPhyAddr.ucShelf,
                           g_stSmssBaseInfo.stPhyAddr.ucSlot);
                           
            /* 检测到来自从CPU的心跳连续缺失，通知SYSCTL。 */
            smss_sysctl_send_local(SMSS_SYSCTL_PROC,SMSS_SYSCTL_LOST_SLAVEHB_MSG,NULL,
                            0);
            smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                           "Send SMSS_SYSCTL_LOST_SLAVEHB_MSG to SYSCTL");
            return;
        }
    }
    return;
}

#define smss_hb_up_received(a)          smss_host_hb_up_received(a)
/***************************************************************************
* 函数名称: smss_host_hb_up_received
* 功    能: 外围板主CPU接收下级心跳处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_host_hb_up_received(SMSS_HEARTBEAT_MSG_T *pstMsg)
{
    if (g_ucHbDownLost > g_ucHbDownLostMax) 
    {
        UINT32 i;
        for (i=SMSS_ALMSUBNO_0; i<SMSS_ALMSUBNO_3; i++) 
        {
            if ((1 == g_ucSlaveAlarmFlag[i]) && 
                 (SMSS_ALMSUBNO_0 == i || SMSS_ALMSUBNO_2 == i)) 
            {
                smss_sysctl_alarm_sendto_oams(SMSS_ALMID_BOARD_SCPU_FAULTY,
                    i, CPS__OAMS_ALARM_CLEAR, pstMsg->stPhyAddr.ucCpu, 0, 0);
                g_ucSlaveAlarmFlag[i] = 0;
            }       
        }
    }
    
    g_ucHbDownLost = 0;

    smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
               "<%010d> Received heartbeat %d from SLAVE.",
               tickGet(), cpss_ntohl(pstMsg->ulSeqNum));
    return;
}

#elif defined(SMSS_BOARD_WITH_DSP) /* 带DSP */

#define SMSS_HB_DOWN            /* 与下级有心跳 */

/* 保存各DSP的心跳缺失计数的数组 */
UINT8 g_aucHbDownLost[SMSS_DSP_TOTAL_NUM];

#define smss_hb_down_timeout() smss_host_hb_down_timeout()
/***************************************************************************
* 函数名称: smss_host_hb_down_timeout
* 功    能: 外围板主CPU处理下级DSP心跳处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_host_hb_down_timeout(VOID)
{
    INT32 i;

    for (i = 0; i < SMSS_DSP_TOTAL_NUM; i++)
    {
        if (FALSE != g_bHbMonitor /* 监测心跳 */
            && g_aucHbDownLost[i] <= SMSS_HB_DOWN_DSP_TIMEROUT) /* 未超过心跳缺失上限 */
        {
            g_aucHbDownLost[i]++;
            if (g_aucHbDownLost[i] > CPSS_DIV(SMSS_HB_DOWN_DSP_TIMEROUT , 2))
            {
                smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_WARN,
                           "<%010d> Lost DSP %d heartbeat %d times.",
                           tickGet(), (i+5), g_aucHbDownLost[i]);
            }
            /* DSP异常前记录 */
#if 0
            if (g_aucHbDownLost[i] == SMSS_HB_DOWN_DSP_TIMEROUT - 2)
            {
              smss_sysctl_dsphb_record(i, 1);	
            }
            else if (g_aucHbDownLost[i] == SMSS_HB_DOWN_DSP_TIMEROUT - 1)
            {
            	smss_sysctl_dsphb_record(i, 2);
            }
#endif            

            if (g_aucHbDownLost[i] > SMSS_HB_DOWN_DSP_TIMEROUT)
            {
                SMSS_SYSCTL_LOST_DSPHB_MSG_T stMsg;
			
                cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                           "Lost DSP %d heartbeat selfPhy (%d-%d-%d)!!!",
                           (i+5), g_stSmssBaseInfo.stPhyAddr.ucFrame,
                           g_stSmssBaseInfo.stPhyAddr.ucShelf,
                           g_stSmssBaseInfo.stPhyAddr.ucSlot);
               
                smss_sysctl_write_sbbr("Lost DSP %d heartbeat selfPhy (%d-%d-%d)!!!",
                           (i+5), g_stSmssBaseInfo.stPhyAddr.ucFrame,
                           g_stSmssBaseInfo.stPhyAddr.ucShelf,
                           g_stSmssBaseInfo.stPhyAddr.ucSlot);                            
                   
                stMsg.ulDspIndex = i;
                /* 检测到来自该DSP的心跳连续缺失，通知SYSCTL该DSP故障 */
                smss_sysctl_send_local(SMSS_SYSCTL_PROC,SMSS_SYSCTL_LOST_DSPHB_MSG,(UINT8*)&stMsg,
                            sizeof(stMsg));
                smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                           "Send SMSS_SYSCTL_LOST_DSPHB_MSG to SYSCTL");
                continue;
            }
        }
    }
    return;
}

#define smss_hb_up_received(a)          smss_host_hb_up_received(a)
/***************************************************************************
* 函数名称: smss_host_hb_up_received
* 功    能: 外围板主CPU接收下级DSP心跳处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_host_hb_up_received(SMSS_HEARTBEAT_MSG_T *pstMsg)
{
	  UINT8  ucIndex;

    SMSS_SYSCTL_DEL_DSP_TIMER_MSG_T  stInd;

    if (g_aucHbDownLost[pstMsg->stPhyAddr.ucCpu - SMSS_CPUNO_1ST_DSP] > g_ucHbDownLostMax)
    {
        UINT32 i;
        for (i=SMSS_ALMSUBNO_0; i<SMSS_ALMSUBNO_3; i++) 
        {
            if (1 == g_ucDspAlarmFlag[i][pstMsg->stPhyAddr.ucCpu - SMSS_CPUNO_1ST_DSP]
                && (SMSS_ALMSUBNO_2 == i)) 
            {
                smss_sysctl_alarm_sendto_oams(SMSS_ALMID_BOARD_DSP_FAULTY,
                    i, CPS__OAMS_ALARM_CLEAR, pstMsg->stPhyAddr.ucCpu, 0, 0);
                g_ucDspAlarmFlag[i][pstMsg->stPhyAddr.ucCpu - SMSS_CPUNO_1ST_DSP] = 0;
            }
        }       
    }

    g_aucHbDownLost[pstMsg->stPhyAddr.ucCpu - SMSS_CPUNO_1ST_DSP] = 0;

    smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
               "<%010d> Received heartbeat %d from DSP %d.",
               tickGet(), cpss_ntohl(pstMsg->ulSeqNum), pstMsg->stPhyAddr.ucCpu);
    
            
    /* 打印单个DSP的心跳 */           
    if (0 != g_stSmssBaseInfo.ulHeartBeatSndSeq)
    {
       if ((g_stSmssBaseInfo.ulHeartBeatSndSeq & 0xff) == pstMsg->stPhyAddr.ucCpu)
       {
       	  cps__oams_shcmd_printf("<%010d> Received heartbeat %d from DSP %d.",
               tickGet(), cpss_ntohl(pstMsg->ulSeqNum), pstMsg->stPhyAddr.ucCpu);
       }
    }    	

    ucIndex = pstMsg->stPhyAddr.ucCpu - SMSS_CPUNO_1ST_DSP;

    /* dsp定时器存在,向SYSCTL发定时器删除消息 */
    if (1 == smss_sysctl_get_dsp_timer(ucIndex))
    {
    	 stInd.ucDspIndex = ucIndex;
    	 smss_sysctl_send_local(SMSS_SYSCTL_PROC,SMSS_SYSCTL_DEL_DSP_TIMER_MSG,
    	               (UINT8*)&stInd, sizeof(stInd));
    }
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_set_dsp
* 功    能: 设置DSP状态, 告警清除用
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_set_dsp(UINT8  ucCpu)
{
  g_ucDspAlarmFlag[SMSS_ALMSUBNO_2][ucCpu] = 1;
  g_aucHbDownLost[ucCpu] = SMSS_HB_DOWN_DSP_TIMEROUT + 1;
	return;
}

#endif /* 带从CPU */

#else  /* 从CPU/DSP */

#define SMSS_HB_UP              /* 与上级有心跳 */

VOID smss_dsp_hb_up_timeout(VOID)
{
   return;
}
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
#define smss_hb_up_timeout() smss_dsp_hb_up_timeout()
#else
#define smss_hb_up_timeout() smss_slave_dsp_hb_up_timeout()
#endif

/***************************************************************************
* 函数名称: smss_slave_dsp_hb_up_timeout
* 功    能: 外围板从CPU或DSP心跳超时处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_slave_dsp_hb_up_timeout(VOID)
{
    CPSS_COM_PHY_ADDR_T stPhyAddr; /* 主CPU物理地址 */
    SMSS_HEARTBEAT_MSG_T stHbMsg; /* 心跳消息结构变量 */

    if ((SMSS_RSTATUS_NORMAL != g_stSmssBaseInfo.ucRStatus)
          || (g_ulPowerOnFlag == 0))
    {
    	#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
		   CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_8OutCnt);
		  #endif
      return;	
    }

    #if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
		 CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_8InCnt);
		#endif

    CPSS_DSP_FILE_REC();
    CPSS_DSP_LINE_REC();
    CPSS_DSP_RUN_ADDR_REC(CPSS_DSP_SMSS_BEAT_START);

    stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
    stPhyAddr.ucCpu = SMSS_CPUNO_HOST;    /* 主CPU */

    stHbMsg.ulSeqNum = cpss_htonl(g_ulHbUpSeqNum); /* 序列号 */

    stHbMsg.stPhyAddr = g_stSmssBaseInfo.stPhyAddr;

    /* 向主CPU发送心跳消息 */
    cpss_com_send_phy(stPhyAddr, SMSS_SYSRT_PROC,
                      SMSS_HEARTBEAT_UP_MSG, (UINT8 *)&stHbMsg, sizeof(stHbMsg));

    smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
                       "<%010d> Send heartbeat %d to HOST.",
                       tickGet(), g_ulHbUpSeqNum);
    g_ulHbUpSeqNum++;
    

    #if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
		 CPSS_DSP_SBBR_COUNTER_REC(g_pstSmssSbbrRecord->ulFunc_8OutCnt);
		#endif
    return;
}

#define smss_hb_down_received(a)  /* 从CPU/DSP对上级主CPU的心跳是单向的，
                                   不会收到来自上级主CPU的心跳。 */

#endif /* 主CPU */
#endif /* 全局板 */
/***************************************************************************
* 函数名称: smss_hb_base_timeout
* 功    能: 心跳超时处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_hb_base_timeout(VOID)
{
#if defined(SMSS_HB_UP)         /* 与上级有心跳 */
    {
        static UINT32 ulUpCount = 0;   /* 与上级间心跳定时计数 */
        ulUpCount++;
        if (ulUpCount >= SMSS_HB_UP_TIMEOUT)
        {
            ulUpCount = 0;
            smss_hb_up_timeout(); /* 与上级间心跳处理 */
        }
    }
#endif

#if defined(SMSS_HB_DOWN)       /* 与下级有心跳 */
    {
        static UINT32 ulDownCount = 0; /* 与下级间心跳定时计数 */
        ulDownCount++;
        if (ulDownCount >= SMSS_HB_DOWN_TIMEOUT)
        {
            ulDownCount = 0;
            smss_hb_down_timeout(); /* 与下级间心跳处理 */
        }
    }
#endif

#if defined(SMSS_HB_MATE)       /* 与伙伴板有心跳 */
    {
        static UINT32 ulMateCount = 0; /* 与伙伴板间心跳定时计数 */
        ulMateCount++;
        if (ulMateCount >= SMSS_HB_MATE_TIMEOUT)
        {
            ulMateCount = 0;
            smss_hb_mate_timeout(); /* 与伙伴板间心跳处理 */
        }
    }
#endif
    return;
}
/***************************************************************************
* 函数名称: smss_sysrt_init
* 功    能: sysrt纤程初始化
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysrt_init(VOID)
{
    SMSS_PROC_ACTIVATE_RSP_MSG_T stRsp;
    UINT32 ulFlag = 0;
    static BOOL bInit = FALSE;

    if (FALSE != bInit)               /* 已初始化过，返回 */
        return;

    bInit = TRUE;
    
    /* 向SMSS SYSCTL发送激活成功响应消息 */
    stRsp.ulResult = 0;
    smss_send_to_local(SMSS_SYSCTL_PROC, SMSS_PROC_ACTIVATE_RSP_MSG,
                       &stRsp, sizeof(stRsp));

#if defined(SMSS_BACKUP_MATE)
    g_stSmssAsInfo.ucSelfAStatus = g_ucAStatus;

    if (SMSS_ACTIVE == g_ucAStatus)
    {
        g_stSmssAsInfo.szSelfAStatus = "ACTIVE";
#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE)    /* 若为主CPU，设置面板灯 */ 
        drv_board_led_set(DRV_LED_TYPE_ACTIVE, DRV_LED_STATU_ON);
        smss_devm_SetActled(1);
#endif	
        g_ucHbMateLost = SMSS_HB_DOWN_LOST_MAX + 1;
    }
    else
    {
        g_stSmssAsInfo.szSelfAStatus = "STANDBY";
        g_ucHbMateLost = 0;
    }

    cpss_com_logic_addr_get(&g_stSmssBaseInfo.stLogAddr, &ulFlag);

#endif

#if defined(SWP_FNBLK_BRDTYPE_MAIN_CTRL) /* 全局板 */ \
    || defined(SMSS_BOARD_WITH_DSP) /* 带DSP的外围板 */

    cpss_mem_memset(&g_aucHbDownLost, SMSS_HB_DOWN_DSP_TIMEROUT + 1, sizeof(g_aucHbDownLost));
#endif
    return;
}

#if defined(SMSS_AS_CMP_SOFT)   /* 软件主备竞争 */
/***************************************************************************
* 函数名称: smss_sysrt_send_as_cmp_req
* 功    能: 软件主备竞争请求发送函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysrt_send_as_cmp_req(VOID)
{
    static UINT8 ucTimes = 0;
    SMSS_AS_CMP_REQ_MSG_T stReq;
    stReq.ucSlot = g_stSmssBaseInfo.stPhyAddr.ucSlot;

    if (++ucTimes > SMSS_AS_CMP_TIMEOUT)
    {
        g_ucAStatus = SMSS_ACTIVE; /* 本板为主用 */

        smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "No response from MATE, I'm ACTIVE.");

        smss_sysrt_init();      /* 全局变量初始化，并发送纤程激活响应 */
        return;
    }
        
    /* 向伙伴板发送主备竞争请求消息 */
    smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "%d: Send SMSS_AS_CMP_REQ_MSG to MATE (ucSlot = %d).",
               ucTimes, stReq.ucSlot);
        
    smss_com_send_mate(SMSS_SYSRT_PROC, SMSS_AS_CMP_REQ_MSG,
                       (UINT8 *)&stReq, sizeof(stReq));

    /* 设置重发定时器 */
    cpss_timer_set(SMSS_TIMER_AS_CMP, SMSS_TIMER_AS_CMP_DELAY);
    return;
}
/***************************************************************************
* 函数名称: smss_sysrt_received_as_cmp_req
* 功    能: 软件主备竞争请求处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysrt_received_as_cmp_req(SMSS_AS_CMP_REQ_MSG_T *pstReq)
{
    SMSS_AS_CMP_RSP_MSG_T stRsp;
        
    smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "Received SMSS_AS_CMP_REQ_MSG from MATE (ucSlot = %d).", pstReq->ucSlot);

    if (SMSS_UNKNOWN_AS_STATUS == g_ucAStatus) /* 本板主备状态未知 */
    {
        g_ucAStatus = (g_stSmssBaseInfo.stPhyAddr.ucSlot < pstReq->ucSlot ?
                       SMSS_ACTIVE : SMSS_STANDBY); /* 确定本板主备状态 */
    }
        
    stRsp.ucAStatus = (SMSS_ACTIVE == g_ucAStatus ?
                       SMSS_STANDBY : SMSS_ACTIVE); /* 确定伙伴板主备状态 */

    /* 向伙伴板发送主备竞争响应消息 */
    smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "Send SMSS_AS_CMP_RSP_MSG, I'm %s.",
               SMSS_ACTIVE == g_ucAStatus ? "ACTIVE" : "STANDBY");
        
    smss_com_send_mate(SMSS_SYSRT_PROC, SMSS_AS_CMP_RSP_MSG,
                       (UINT8 *)&stRsp, sizeof(stRsp));

    cpss_timer_delete(SMSS_TIMER_AS_CMP);    /* 删除重发定时器 */
    smss_sysrt_init();      /* 全局变量初始化，并发送纤程激活响应 */
    return;
}
/***************************************************************************
* 函数名称: smss_sysrt_received_as_cmp_rsp
* 功    能: 软件主备竞争响应处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysrt_received_as_cmp_rsp(SMSS_AS_CMP_RSP_MSG_T *pstRsp)
{
    g_ucAStatus = pstRsp->ucAStatus; /* 得到本板的主备状态 */
        
    smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "Received SMSS_AS_CMP_RSP_MSG, I'm %s.",
               SMSS_ACTIVE == g_ucAStatus ? "ACTIVE" : "STANDBY");

    cpss_timer_delete(SMSS_TIMER_AS_CMP);    /* 删除重发定时器 */
    smss_sysrt_init();      /* 全局变量初始化，并发送纤程激活响应 */
    return;
}

#endif /* 软件主备竞争 */

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
VOID smss_sysrt_boardalarmind_disp(SMSS_BRD_ALARM_SEND_IND_MSG_T *pstMsg)
{
    g_aucHbDownLost[pstMsg->stPhyAddr.ucFrame-1][pstMsg->stPhyAddr.ucShelf-1][pstMsg->stPhyAddr.ucSlot-1] 
        = SMSS_HB_DOWN_LOST_MAX + 3;
    return;
}
#endif

/***************************************************************************
* 函数名称: smss_sysrt_proc
* 功    能: SMSS SYSRT纤程的入口函数。
* 函数类型  VOID
* 参    数:
* 参数名称      类型             输入/输出       描述
* usState      UINT16               IN         纤程状态
* pVar         VOID *               IN         纤程私有数据区指针
* pstMsg CPSS_COM_MSGHEAD_T *       IN         消息头结构
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
UINT32 g_ulSysRtInitFlag = FALSE;

VOID smss_sysrt_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsg)
{
	  smss_test_PrintMsgStream(pstMsg->ulMsgId,pstMsg->pucBuf,
         pstMsg->ulLen,FALSE);

    /* 该纤程接受到的第一条消息必须是SMSS_PROC_ACTIVATE_REQ_MSG */
    if( SMSS_PROC_ACTIVATE_REQ_MSG == pstMsg->ulMsgId )
    {
       g_ulSysRtInitFlag = TRUE;
    }
    if(TRUE !=  g_ulSysRtInitFlag)
    {
       return;
    }

    switch(pstMsg->ulMsgId)
    {
#if defined(SMSS_BACKUP_MATE)   /* 存在伙伴板 */
#if defined(SMSS_AS_CMP_SOFT)   /* 软件主备竞争 */
    case SMSS_AS_CMP_REQ_MSG:   /* 主备竞争请求消息 */
       if (SMSS_RSTATUS_END != g_stSmssBaseInfo.ucRStatus)
       {
        smss_sysrt_received_as_cmp_req((SMSS_AS_CMP_REQ_MSG_T *)pstMsg->pucBuf);
       }
        break;

    case SMSS_AS_CMP_RSP_MSG:   /* 主备竞争响应消息 */
       if (SMSS_RSTATUS_END != g_stSmssBaseInfo.ucRStatus)
       { 
        smss_sysrt_received_as_cmp_rsp((SMSS_AS_CMP_RSP_MSG_T *)pstMsg->pucBuf);
       }
        break;
    
    case SMSS_TIMER_AS_CMP_MSG:     /* 主备竞争定时器超时消息 */
        smss_sysrt_send_as_cmp_req();
        break;
    
#endif /* 软件主备竞争 */
        
    case SMSS_STANDBY_TO_ACTIVE_REQ_MSG:          /* 备升主请求消息 */
    {
        /* 直接向SMSS SYSCTL发送备升主成功响应消息 */
        SMSS_STANDBY_TO_ACTIVE_RSP_MSG_T stRsp;
        stRsp.ulResult = SMSS_OK;
        smss_send_to_local(SMSS_SYSCTL_PROC, SMSS_STANDBY_TO_ACTIVE_RSP_MSG,
                           &stRsp, sizeof(stRsp));

        g_ucAStatus = SMSS_ACTIVE;

        break;
    }

    case SMSS_STANDBY_TO_ACTIVE_COMPLETE_IND_MSG: /* 备升主完成消息 */
#if defined(SWP_FNBLK_BRDTYPE_MAIN_CTRL) /* 全局板 */
        g_ucAStatus = SMSS_ACTIVE;
#endif /* 全局板 */
        break;

    case SMSS_ACTIVE_TO_STANDBY_IND_MSG:          /* 主降备消息 */
        g_ucAStatus = SMSS_STANDBY;
        cpss_timer_delete(SMSS_TIMER_HB_BASE); /* 删除基准定时器 */
        break;
#endif /* 存在伙伴板 */

    case SMSS_PROC_ACTIVATE_REQ_MSG:              /* 纤程激活请求消息 */
#if defined(SMSS_BACKUP_MATE)   /* 存在伙伴板 */
#if defined(SMSS_AS_CMP_SOFT)   /* 软件主备竞争 */
        smss_sysrt_send_as_cmp_req();
        break;
#else
        g_ucAStatus = ((SMSS_PROC_ACTIVATE_REQ_MSG_T *)pstMsg->pucBuf)->ucAsStatus;
#endif /* 软件主备竞争 */
#endif /* 存在伙伴板 */
        
        smss_sysrt_init();      /* 全局变量初始化，并发送纤程激活响应 */
        break;

    case SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG:     /* 激活完成消息 */
        /* 启动基准定时器 */
        cpss_timer_loop_set(SMSS_TIMER_HB_BASE, SMSS_TIMER_HB_BASE_DELAY);
        break;

    case SMSS_TIMER_HB_BASE_MSG: /* 心跳基准定时器超时消息 */
        smss_hb_base_timeout();
        break;

#if defined(SMSS_HB_DOWN)       /* 与下级有心跳 */
    case SMSS_HEARTBEAT_UP_MSG: /* 下级发来的心跳消息 */
        smss_hb_up_received((SMSS_HEARTBEAT_MSG_T *)pstMsg->pucBuf);
        break;
#endif /* 与下级有心跳 */
        
#if defined(SMSS_HB_UP)         /* 与上级有心跳 */
    case SMSS_HEARTBEAT_DOWN_MSG: /* 上级发来的心跳消息 */
        smss_hb_down_received((SMSS_HEARTBEAT_MSG_T *)pstMsg->pucBuf);
        break;
#endif /* 与上级有心跳 */
        
#if defined(SMSS_HB_MATE)       /* 与伙伴板有心跳 */
    case SMSS_HEARTBEAT_MATE_MSG: /* 伙伴板发来的心跳消息 */
        smss_hb_mate_received((SMSS_HEARTBEAT_MSG_T *)pstMsg->pucBuf);
        break;
#endif /* 与伙伴板有心跳 */

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
    case SMSS_SYSCTL_BRD_ALARM_SEND_IND:
        smss_sysrt_boardalarmind_disp((SMSS_BRD_ALARM_SEND_IND_MSG_T *)pstMsg->pucBuf);
        break;
#endif

     case SMSS_TIMER_END_MSG:  /* 在sysrt纤程启的定时器,在SYSRT纤程收 */
       smss_sysctl_state_end_reset();
     break;

    default:
        smss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "Received unknown message 0x%08X.",
                   pstMsg->ulMsgId);
        break;
    }
    return;
}

VOID smss_sysctl_sendbrdalarmind_tosysrt(CPSS_COM_PHY_ADDR_T  stPhyAddr)
{
    SMSS_BRD_ALARM_SEND_IND_MSG_T stIndMsg;
    stIndMsg.stPhyAddr = stPhyAddr;
    /* 向主CPU发送心跳消息 */
    cpss_com_send_phy(g_stSmssBaseInfo.stPhyAddr, SMSS_SYSRT_PROC,
        SMSS_SYSCTL_BRD_ALARM_SEND_IND, (UINT8 *)&stIndMsg, sizeof(stIndMsg));
    return;
}

/***************************************************************************
* 函数名称:  smss_sysctl_recv_brdself_fault_ind
* 功    能: SMSS 收到本板故障指示消息处理
* 函数类型   VOID
* 参    数:
* 参数名称      类型             输入/输出       描述
* pstMsg CPSS_COM_MSGHEAD_T *       IN         消息头结构
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
void smss_sysctl_recv_brdself_fault_ind(CPSS_COM_MSG_HEAD_T *pstMsg)
{
   UINT32 ulSrcProcID;
   SMSS_BOARDSELF_FAULT_IND_MSG_T *pMsg = 
      (SMSS_BOARDSELF_FAULT_IND_MSG_T *)(pstMsg->pucBuf);
   ulSrcProcID = pstMsg->stSrcProc.ulPd;
   cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
        "recv %s(%#x) self board fault ind fault cause = %#x",cpss_vk_proc_name_get_by_pid(ulSrcProcID),ulSrcProcID,cpss_ntohl(pMsg->ulCause));
   /* 写黑匣子 立即复位*/
   smss_sysctl_enter_end(TRUE,cpss_ntohl(pMsg->ulCause));
}

#else  /* 告警箱处理 */
VOID smss_sysctl_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsg)
{
    if (NULL == pstMsg)                     /* 消息合法性校验 */
    {
        return;
    }
    smss_test_PrintMsgStream(pstMsg->ulMsgId,pstMsg->pucBuf,
        pstMsg->ulLen,FALSE);
    
    switch(pstMsg->ulMsgId)
    {
    case CPSS_COM_POWER_ON_MSG:      /* CPSS的上电消息 */
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "recv CPSS_COM_POWER_ON_MSG.");
        smss_sysctl_proc_table_init();
	if (SMSS_OK == smss_sysctl_abox_configure())
        {
		smss_sysctl_abox_power_on((CPSS_COM_POWER_ON_MSG_T *)pstMsg->pucBuf);
	}
	else
	{
		cps__oams_shcmd_printf("please enter ip\n");
	}
        break;
        
    case SMSS_TIMER_IDLE_MSG:        /* 超时消息 */
        smss_sysctl_state_abox_timeout();
        break;
        
    case SMSS_PROC_ACTIVATE_RSP_MSG: /* 纤程激活应答消息 */
        smss_sysctl_abox_recv_actprocrsp(pstMsg);
        break;
    case SMSS_ABOX_UPDATE_SWVER_COMMAND_IND_MSG:
        smss_sysctl_abox_recv_update_swverind(pstMsg);
        break;
    case CPSS_FTP_RSP_MSG: /* 主用全局板接收到CPSS_FTP_RSP_MSG消息 */
        smss_sysctl_abox_recv_update_resultind(pstMsg);
        break;

    case SMSS_VERSION_UPDATE_IND_MSG:
        smss_abox_rev_ver_update(pstMsg);
        break;
        
    default:                         /* 收到非法消息，进行告警 */
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "Received unknown message 0x%08X.",
            pstMsg->ulMsgId);
        break;
    }
}
/***************************************************************************
* 函数名称: smss_sysctl_abox_power_on
* 功    能: 收到CPSS的激活消息后进行处理。
* 函数类型  VOID
* 参    数:
* 参数名称        类型               输入/输出       描述
* pstMsg   CPSS_COM_POWER_ON_MSG_T *    IN        
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
VOID smss_sysctl_abox_power_on(CPSS_COM_POWER_ON_MSG_T *pstMsg)
{  
    UINT32 ulStatus = 0;
        
    /* 判断消息的有效性 */
    if (NULL == pstMsg)
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "smss_sysctl_power_on: Received NULL Message.");
        return;
    }

    /*smss_abox_route_init();*/
    
    /* 读取版本文件信息,存入全局变量 */
    smss_sysctl_read_verinfo_file();

    /* 开辟用于存储纤程初始化结果的动态空间 */
    pulProcActResult = cpss_mem_malloc(g_ulSmssSysctlProcNum * sizeof(UINT32));
    
    if (NULL == pulProcActResult) /* 开辟空间失败时，直接复位 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
            "Malloc memory failed, Enter End.");
        smss_abox_reset();
        return;
    }

    cpss_mem_memset(pulProcActResult, 0xFF, g_ulSmssSysctlProcNum*sizeof(UINT32));
    
    /* 激活纤程*/
    smss_sysctl_abox_send_act_stoa_req(); 
    return;
}

/***************************************************************************
* 函数名称smss_sysctl_abox_send_act_stoa_req()
* 功    能: 告警箱根据纤程激活表和当前状态依次向各纤程发消息。
* 函数类型  VOID
* 参    数:
* 参数名称  参数类型        输入/输出           参数描述
* ulMsgDes  UINT32             IN               要发送消息的描述符
* 函数返回:
*           无。
* 说    明: 该函数在单板初始化和备升主过程中被调用，不同的地方发送的消息不同
***************************************************************************/
VOID smss_sysctl_abox_send_act_stoa_req(VOID)
{
    UINT32 ulTid = 0;
    SMSS_PROC_ACTIVATE_REQ_MSG_T stActProcMsg;
    
    stActProcMsg.ucAsStatus = SMSS_ACTIVE;

    /* 依次向各纤程发消息 */
    while (g_ulSmssSendIndex < g_ulSmssSysctlProcNum)
    {
        smss_sysctl_send_local(g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulProcDes,
                               SMSS_PROC_ACTIVATE_REQ_MSG, (UINT8 *)&stActProcMsg, 
                               sizeof(stActProcMsg));

        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "Send SMSS_PROC_ACTIVATE_REQ_MSG to Proc `%s'.",
                   cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[g_ulSmssSendIndex].ulProcDes));

        g_ulSmssSendIndex = g_ulSmssSendIndex + 1;

        if (0 != g_pstSmssSysctlProcTable[g_ulSmssSendIndex-1].ulWaitTime)
            /* 同步激活时设置定时器进行等待 */
        {
            pulProcActResult[g_ulSmssSendIndex-1] = CPSS_TD_INVALID;

            ulTid = cpss_timer_para_set(SMSS_TIMER_IDLE,
                g_pstSmssSysctlProcTable[g_ulSmssSendIndex-1].ulWaitTime, g_ulSmssSendIndex);

            if (CPSS_TD_INVALID != ulTid) 
            {
                pulProcActResult[g_ulSmssSendIndex-1] = ulTid+1;
            }
            break;
        }  /*end of if */
    } /*end of for */

    if (g_ulSmssSendIndex == g_ulSmssSysctlProcNum)
        /* 向所有纤程发送完消息后进行处理 */
    {
        if (g_ulSmssRspedProcNum != g_ulSmssSysctlProcNum)
            /* 没有收到所有纤程的应答消息则起统一定时器等待应答 */
        {
            if (0 != g_pstSmssSysctlProcTable[g_ulSmssSysctlProcNum-1].ulWaitTime)
                /* 若最后一个激活的纤程是同步纤程，则删除为其创建的定时器 */
            {
                cpss_timer_para_delete(pulProcActResult[g_ulSmssSendIndex-1]-1);
                pulProcActResult[g_ulSmssSendIndex-1] = CPSS_TD_INVALID;
            }
              /* 创建等待所有纤程应答消息的定时器 */
            cpss_timer_set(SMSS_TIMER_IDLE, 100000);
        }
        else /* 收到所有的应答消息时初始化或者备升主结束 */
        {
            smss_sysctl_broadcast(SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG, NULL, 0);
        }
    }
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_abox_recv_actprocrsp
* 功    能: 处理纤程应答消息
* 参    数:
* 参数名称        类型                 输入/输出       描述
* pstMsg      CPSS_COM_MSG_HEAD_T *       IN          
* 函数返回:
*           无。
* 说    明: 。
***************************************************************************/
VOID smss_sysctl_abox_recv_actprocrsp(CPSS_COM_MSG_HEAD_T *pstMsg)
{
    UINT32 ulProcDes;
    UINT32 ulResult;
    UINT32 ulIndex;

    ulProcDes = pstMsg->stSrcProc.ulPd;
    ulResult = cpss_htonl(((SMSS_PROC_ACTIVATE_RSP_MSG_T*)pstMsg->pucBuf)->ulResult);
    ulIndex = smss_sysctl_procdes2index(ulProcDes);

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               " Received %s from Proc `%s', ulResult = %d.",
               "SMSS_PROC_ACTIVATE_RSP_MSG" ,
               cpss_vk_proc_name_get_by_pid(g_pstSmssSysctlProcTable[ulIndex].ulProcDes), 
               ulResult);

    if (SMSS_OK != ulResult)   /* 激活失败复位*/
    {
        smss_abox_reset();
        return;
    }

    if (CPSS_TD_INVALID != pulProcActResult[ulIndex])   /* 如果设置有定时器，则删除定时器 */
    {
        cpss_timer_para_delete(pulProcActResult[ulIndex]-1);
    }

    pulProcActResult[ulIndex] = ulResult;   /* 登记该结果 */
    g_ulSmssRspedProcNum = g_ulSmssRspedProcNum + 1;

    if (g_ulSmssRspedProcNum == g_ulSmssSysctlProcNum)
        /* 收到所有纤程的应答消息，根据不同的状态进行不同的处理 */
    {
        /* 设置状态， 删除等待所有纤程应答消息的定时器 */
        ucActProcStage = SMSS_SEND_OK;
        cpss_timer_delete(SMSS_TIMER_IDLE);

        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "Active All Pro End!!!");
            
        g_stSmssBaseInfo.ucRStatus = SMSS_RSTATUS_NORMAL;
            
        /* 广播激活完成消息 */
        smss_sysctl_broadcast(SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG, NULL, 0);

        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "Send SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG to All Pro.");

        drv_alba_normal_led_set();  
        g_ulSmssSendIndex = 0;
    }
    /* 若收到的激活成功消息为同步纤程的应答消息且还有纤程需要激活，则接着激活 */
    else
    {
        if (  (ulIndex == g_ulSmssSendIndex-1)                /* 该纤程为刚激活的纤程 */
              &&(0 != g_pstSmssSysctlProcTable[ulIndex].ulWaitTime)   /* 该纤程为同步纤程 */
              &&(g_ulSmssSendIndex != g_ulSmssSysctlProcNum)          /* 还有纤程序要激活 */
            )
        {
            smss_sysctl_abox_send_act_stoa_req();
        }
    }
    
    return;
}

/***************************************************************************
* 函数名称:smss_sysctl_state_abox_timeout
* 功    能: 处理纤程应答消息
* 参    数:
* 参数名称        类型                 输入/输出       描述
* 函数返回:
*           无。
* 说    明: 。
***************************************************************************/
VOID smss_sysctl_state_abox_timeout(VOID)
{
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
        "Active Proc TimeOut!");
    smss_abox_reset();
    return;
}

/*******************************************************************************
* 函数名称: smss_sysctl_full_name
* 功    能: 获得文件的绝对路径。
* 函数类型:STRING
* 参    数: 
* 参数名称           类型                     输入/输出        描述
*
*  szFileName      STRING           IN     文件名指针。
*
* 函数返回: 成功：返回文件名(aucFullName),其它，返回NULL。
* 说    明: 无
*******************************************************************************/
STRING smss_sysctl_full_name(STRING szFileName)
{
    static UINT8 aucFullName[CPSS_FS_FILE_ABSPATH_LEN];
    /* 文件名格式为："F:/smss/version/gcpa_host_020000.ver" */
    strcpy(aucFullName, "F:/smss/version/");
    strcat(aucFullName, szFileName);
    return aucFullName;
}

/***************************************************************************
* 函数名称: smss_version_update
* 功    能: 告警箱升级
* 参    数:
* 参数名称        类型                 输入/输出       描述
* pstMsg          CPSS_COM_MSG_HEAD_T *  IN          
* 函数返回:
*           无。
* 说   明: 文件名可以带相对路径，但路径必须以'/'划分，文件名不能为空
***************************************************************************/
VOID smss_version_update(STRING szSvrIpAddr, STRING szFileName)
{
    SMSS_VERSION_UPDATE_IND_MSG_T stVerUpdateInd;

    cpss_mem_memset(&stVerUpdateInd, 0, sizeof(stVerUpdateInd));
    
    cpss_mem_memcpy(stVerUpdateInd.aucSvrIpAddr,szSvrIpAddr,strlen(szSvrIpAddr));
    cpss_mem_memcpy(stVerUpdateInd.aucFileName,szFileName,strlen(szFileName));

    smss_sysctl_send_local(SMSS_SYSCTL_PROC, SMSS_VERSION_UPDATE_IND_MSG,
        (UINT8*)&stVerUpdateInd, sizeof(stVerUpdateInd));

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Send SMSS_VERSION_UPDATE_IND to self.");
    
}

/***************************************************************************
* 函数名称: smss_abox_send_ftp_req
* 功    能: 向CPSS发送FTP请求
* 参    数:
* 参数名称        类型                 输入/输出       描述
* pstMsg          CPSS_COM_MSG_HEAD_T *  IN          
* 函数返回:
*           无。
* 说   明: 文件名可以带相对路径，但路径必须以'/'划分，文件名不能为空
***************************************************************************/
VOID smss_abox_send_ftp_req(STRING szSvrIpAddr, STRING szFileName)
{
    CPSS_FTP_REQ_MSG_T stReq;
    CHAR* pcSlash;
    
    /* 初始化FTP请求消息结构 */
    
    stReq.ulSerialID = cpss_ntohl(0);/* 用来识别同一个纤程的多个ftp传输 */
    strcpy(stReq.aucUserName, "anonymous"); 
    strcpy(stReq.aucPasswd, "anonymous");                                   /* 输入密码 */
    stReq.ulServerIP = cpss_ntohl(inet_addr(szSvrIpAddr));   /* 服务器IP地址 */
    strcpy(stReq.aucServerFile, szFileName);  /* 服务器端文件名 */
    
    pcSlash = strrchr(szFileName,'/');
    if (pcSlash != NULL) 
    {
        strcpy(stReq.aucClientFile,                             /* 客户端文件名 */
            smss_sysctl_full_name(pcSlash+1));
    }
    else
    {
    strcpy(stReq.aucClientFile,                             /* 客户端文件名 */
        smss_sysctl_full_name(szFileName));
    }

    stReq.ulFtpCmd = CPSS_FTP_CMD_GET_FILE; /* 传输命令：下载 CPSS_FILE_GET*/
    stReq.ulLen = 0;        /* 缓存空间大小，如果为0，则写入文件；否则写入缓存 */
    stReq.ulProgessStep = 0;
    /* stReq.ulLen = pstNode->stInfo.ulFileLen;*/
    stReq.pucMem = NULL;    /* 写入文件，不写入缓冲区 */
    
    /* 发送文件下载请求消息给CPSS */
    smss_sysctl_send_local(CPSS_FS_FTP_LOW_PROC, CPSS_FTP_REQ_MSG,
        (UINT8*)&stReq, sizeof(stReq));
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Sent CPSS_FTP_REQ_MSG to CPSS, user: `%s', password: `%s', "
        "IP: %d.%d.%d.%d, file: `%s'.",
        stReq.aucUserName, stReq.aucPasswd,
        (UINT8)(stReq.ulServerIP >> 24), (UINT8)(stReq.ulServerIP >> 16),
        (UINT8)(stReq.ulServerIP >> 8), (UINT8)stReq.ulServerIP,
        stReq.aucServerFile);
    
    g_stAboxReqVerInfo.ulVerInfoNum = 1;
    strcpy(g_stAboxReqVerInfo.astVerInfo[0].aucFileName,szFileName);    
    return;  
}

/***************************************************************************
* 函数名称: smss_abox_rev_ver_update
* 功    能: 版本升级指示处理
* 参    数:
* 参数名称        类型                 输入/输出       描述
* pstMsg          CPSS_COM_MSG_HEAD_T *  IN          
* 函数返回:
*           无。
* 说   明: 文件名可以带相对路径，但路径必须以'/'划分，文件名不能为空
***************************************************************************/
VOID smss_abox_rev_ver_update(CPSS_COM_MSG_HEAD_T *pstMsg)
{
    SMSS_VERSION_UPDATE_IND_MSG_T *pInd = (SMSS_VERSION_UPDATE_IND_MSG_T *)pstMsg->pucBuf; 
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Call smss_abox_send_ftp_req.");
    
    smss_abox_send_ftp_req(pInd->aucSvrIpAddr, pInd->aucFileName);
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_abox_recv_update_swverind
* 功    能: 告警箱升级指示消息处理
* 参    数:
* 参数名称        类型                 输入/输出       描述
* pstMsg          CPSS_COM_MSG_HEAD_T *  IN          
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_abox_recv_update_swverind(CPSS_COM_MSG_HEAD_T *pstMsg)
{
#if 0
    SMSS_ABOX_UPDATE_COMMAND_IND_MSG_T *pstAboxUpdateInd;
    CPSS_FTP_REQ_MSG_T stReq;
    
    pstAboxUpdateInd = (SMSS_ABOX_UPDATE_COMMAND_IND_MSG_T*)(pstMsg->pucBuf);
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Receive SMSS_ABOX_UPDATE_SWVER_COMMAND_IND_MSG, user: `%s', password: `%s', "
        "IP: %ld, file: `%s'.",
        pstAboxUpdateInd->acFtpSvrUserName, pstAboxUpdateInd->acFtpSvrPassWord,
        pstAboxUpdateInd->ulFtpSvrIp,
        pstAboxUpdateInd->acVerFileName);
    
    
    /* 初始化FTP请求消息结构 */
    
    stReq.ulSerialID = cpss_ntohl(pstAboxUpdateInd->stReqHeader.ulSeqId);/* 用来识别同一个纤程的多个ftp传输 */
    strcpy(stReq.aucUserName, pstAboxUpdateInd->acFtpSvrUserName); /* 获得用户名 */
    strcpy(stReq.aucPasswd, pstAboxUpdateInd->acFtpSvrPassWord);   /* 输入密码 */
    stReq.ulServerIP = cpss_ntohl(pstAboxUpdateInd->ulFtpSvrIp);   /* 服务器IP地址 */
    strcpy(stReq.aucServerFile,pstAboxUpdateInd->acFtpSvrFileDir); /* 服务器相对路径 */ 
    strcat(stReq.aucServerFile, pstAboxUpdateInd->acVerFileName);  /* 服务器端文件名 */
  
    strcpy(stReq.aucClientFile,                             /* 客户端文件名 */
        smss_sysctl_full_name(pstAboxUpdateInd->acVerFileName));
    stReq.ulFtpCmd = CPSS_FTP_CMD_GET_FILE; /* 传输命令：下载 CPSS_FILE_GET*/
    stReq.ulLen = 0;        /* 缓存空间大小，如果为0，则写入文件；否则写入缓存 */
    stReq.ulProgessStep = 0;
    /* stReq.ulLen = pstNode->stInfo.ulFileLen;*/
    stReq.pucMem = NULL;    /* 写入文件，不写入缓冲区 */
    
    /* 发送文件下载请求消息给CPSS */
    smss_send_to_local(CPSS_FS_FTP_LOW_PROC, CPSS_FTP_REQ_MSG,
        &stReq, sizeof(stReq));
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Sent CPSS_FTP_REQ_MSG to CPSS, user: `%s', password: `%s', "
        "IP: %d.%d.%d.%d, file: `%s'.",
        stReq.aucUserName, stReq.aucPasswd,
        (UINT8)(stReq.ulServerIP >> 24), (UINT8)(stReq.ulServerIP >> 16),
        (UINT8)(stReq.ulServerIP >> 8), (UINT8)stReq.ulServerIP,
        stReq.aucServerFile);

    g_stAboxCurVerInfo.ulVerInfoNum = 1;
    strcpy(g_stAboxCurVerInfo.astVerInfo[0].aucFileName,pstAboxUpdateInd->acVerFileName);
    smss_sysctl_write_verinfo_file();
#endif
    return;    
}


VOID smss_sysctl_get_verinfo(VOID)
{
    FILE *pFile;
    SWP_VER_FILE_HEAD_T  stFileHead;

   
    /* 打开FLASH文件系统中版本信息文件 */
    pFile = fopen(smss_sysctl_full_name(g_stAboxCurVerInfo.astVerInfo[0].aucFileName), "rb");
    if (NULL == pFile)  /*文件不存在, 收到版本信息时会创建文件*/
    {
        g_stAboxCurVerInfo.ulVerInfoNum = 0;   
        return;
    }

    /* 读取版本信息 */
    fread(&stFileHead, 1, sizeof(SWP_VER_FILE_HEAD_T), pFile);
    
   g_stAboxCurVerInfo.ulVerInfoNum = 1; 
   g_stAboxCurVerInfo.astVerInfo[0].ucVerType =  (UINT8)cpss_ntohs(stFileHead.usVerType);
   g_stAboxCurVerInfo.astVerInfo[0].ucFuncType =  CPS__RDBS_DEV_FUNCTYPE_CPU_INGRESS;
   g_stAboxCurVerInfo.astVerInfo[0].ucLogicUnitNo =  1;
   g_stAboxCurVerInfo.astVerInfo[0].aucRsv[0] =  0;
   g_stAboxCurVerInfo.astVerInfo[0].ulVerNo =  cpss_ntohl(stFileHead.ulVerNo);
   g_stAboxCurVerInfo.astVerInfo[0].ulVerDate = 0;
   g_stAboxCurVerInfo.astVerInfo[0].ulFileLen =  cpss_ntohl(stFileHead.ulVerFileLen); 
   g_stAboxCurVerInfo.astVerInfo[0].ulCheckSum =  cpss_ntohl(stFileHead.ulVerDataCheckSum);                     
     
   fclose(pFile);
    
   return;    
}


/***************************************************************************
* 函数名称: smss_sysctl_abox_recv_update_resultind
* 功    能: FTP下载结果消息处理
* 参    数:
* 参数名称        类型                 输入/输出       描述
* pstMsg          CPSS_COM_MSG_HEAD_T *  IN          
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_abox_recv_update_resultind(CPSS_COM_MSG_HEAD_T *pstMsg)
{
    CPSS_FTP_RSP_MSG_T *pstRsp = (CPSS_FTP_RSP_MSG_T *)pstMsg->pucBuf;
#if 0
    SMSS_ABOX_UPDATE_RESULT_IND_MSG_T stUpdateResultInd;
#endif

    cps__oams_shcmd_printf("Received CPSS_FTP_RSP_MSG from CPSS, lErrorNo = %d.",
        pstRsp->lErrorNo);

    if (CPSS_OK == pstRsp->lErrorNo) 
    {
        
        if(0 != strcmp(g_stAboxCurVerInfo.astVerInfo[0].aucFileName,g_stAboxReqVerInfo.astVerInfo[0].aucFileName))
        {
            cpss_file_delete(smss_sysctl_full_name(g_stAboxCurVerInfo.astVerInfo[0].aucFileName));
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                "Delete File : %s.",
                smss_sysctl_full_name(g_stAboxCurVerInfo.astVerInfo[0].aucFileName));
        }

        g_stAboxCurVerInfo = g_stAboxReqVerInfo;
        smss_sysctl_get_verinfo();      
        smss_sysctl_write_verinfo_file();
    }
  
#if 0
    stUpdateResultInd.ulSeqId = cpss_htonl(pstRsp->ulSerialID);
    stUpdateResultInd.ulReturnCode = cpss_htonl(pstRsp->lErrorNo); 

    smss_sysctl_send_local(CPS__OAMS_PD_M_ABOX_DRV_PROC,
        SMSS_ABOX_UPDATE_SWVER_RESULT_IND_MSG,(UINT8*)&stUpdateResultInd,
        sizeof(stUpdateResultInd));

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Send SMSS_ABOX_UPDATE_SWVER_RESULT_IND_MSG,ulSeqId = %d, ulReturnCode = %d",
        stUpdateResultInd.ulSeqId,stUpdateResultInd.ulReturnCode);
#endif
}


/*******************************************************************************
* 函数名称: smss_sysctl_read_verinfo_file
* 功    能: 读取FLASH文件系统中版本信息文件内容，存入当前版本信息结构中。
* 函数类型: VOID
* 参    数: 无
* 函数返回: 无
* 说    明: 无
*******************************************************************************/
VOID smss_sysctl_read_verinfo_file(VOID)
{
    FILE *pFile;

    /* 打开FLASH文件系统中版本信息文件 */
    pFile = fopen(szAboxVerInfoFile, "rb");
    if (NULL == pFile)  /*文件不存在, 收到版本信息时会创建文件*/
    {
        g_stAboxCurVerInfo.ulVerInfoNum = 0;   
        return;
    }

    /* 读取版本信息 */
    fread(&g_stAboxCurVerInfo, 1, sizeof(g_stAboxCurVerInfo), pFile);
    if (g_stAboxCurVerInfo.ulVerInfoNum > CPS__RDBS_DEV_VERITEM_MAX) /* 非法 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "File 'verinfo.dat' error: ulVerInfoNum = %d.",
            g_stAboxCurVerInfo.ulVerInfoNum);
        g_stAboxCurVerInfo.ulVerInfoNum = 0;
    }
    else
    {
        UINT32 i;
        
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "Current Version Info:");
        
        if (0 == g_stAboxCurVerInfo.ulVerInfoNum) /* 版本信息项个数为0 */
        {
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                "No version file");
            return;
        }
        
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "   No.     File Name   ulFileLen  ulCheckSum");
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            " ---------------------------------");
        
        for (i = 0; i < g_stAboxCurVerInfo.ulVerInfoNum; i++) /* 依次打印各版本信息项内容 */
        {
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                "    %d:  %s   %u   %u",
                i, g_stAboxCurVerInfo.astVerInfo[i].aucFileName,g_stAboxCurVerInfo.astVerInfo[i].ulFileLen, 
                g_stAboxCurVerInfo.astVerInfo[i].ulCheckSum);
        }
        
        if (0 == g_stAboxCurVerInfo.ulVerInfoNum) /* 版本信息项个数为0 */
        {
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                "File verinfo.dat' warning: ulVerInfoNum is 0.");
        }
    }
    
    /* 关闭版本信息文件 */
    fclose(pFile);
    
    return;
}

/*******************************************************************************
* 函数名称: smss_sysctl_write_verinfo_file
* 功    能: 读取FLASH文件系统中版本信息文件内容，存入当前版本信息结构中。
* 函数类型: VOID
* 参    数: 无
* 函数返回: 无
* 说    明: 无
*******************************************************************************/
VOID smss_sysctl_write_verinfo_file(VOID)
{
    FILE *pFile;
    
    /* 打开FLASH文件系统中版本信息文件 */
    pFile = fopen(szAboxVerInfoFile, "wb");
    if (NULL == pFile)
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "Cannot open `verinfo.dat' to write");
        return;
    }
    
    /* 读取版本信息 */
    fwrite(&g_stAboxCurVerInfo, 1, sizeof(g_stAboxCurVerInfo), pFile);
    
    if (g_stAboxCurVerInfo.ulVerInfoNum > CPS__RDBS_DEV_VERITEM_MAX) /* 非法 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "File 'verinfo.dat' error: ulVerInfoNum = %d.",
            g_stAboxCurVerInfo.ulVerInfoNum);
        g_stAboxCurVerInfo.ulVerInfoNum = 0;
    }
    else
    {
        UINT32 i;
        
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "Current Version Info:");
        
        if (0 == g_stAboxCurVerInfo.ulVerInfoNum) /* 版本信息项个数为0 */
        {
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                "No version file");
            return;
        }
        
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "   No.     File Name   ulFileLen  ulCheckSum");
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            " ---------------------------------");
        
        for (i = 0; i < g_stAboxCurVerInfo.ulVerInfoNum; i++) /* 依次打印各版本信息项内容 */
        {
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                "    %d:  %s   %u   %u",
                i, g_stAboxCurVerInfo.astVerInfo[i].aucFileName,g_stAboxCurVerInfo.astVerInfo[i].ulFileLen, 
                g_stAboxCurVerInfo.astVerInfo[i].ulCheckSum);
        }
        
        if (0 == g_stAboxCurVerInfo.ulVerInfoNum) /* 版本信息项个数为0 */
        {
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                "File verinfo.dat' warning: ulVerInfoNum is 0.");
        }
    }
    
    /* 关闭版本信息文件 */
    fclose(pFile);
    return;
}

/***************************************************************************
* 函数名称: smss_abox_mstate_set
* 功    能: 告警箱启用/禁用状态设置
* 函数类型  UINT32
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* ulMState        UINT32
* 函数返回: 
*  UINT32
* 说    明: 
***************************************************************************/
UINT32 smss_abox_mstate_set(UINT32 ulMState)
{
    INT32 lResult;
    lResult = drv_alba_mstate_set((UINT8)ulMState);
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Call drv_alba_mstate_set.Result = %d",lResult);
    return lResult;
}
/***************************************************************************
* 函数名称: smss_abox_mstate_get
* 功    能: 告警箱启用/禁用状态查询
* 函数类型  UINT32
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* pulMState       UINT32 *
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
UINT32 smss_abox_mstate_get(UINT32 *pulMState)
{
    UINT8 ucMState;
    INT32 lResult;
    lResult = drv_alba_mstate_get(&ucMState);
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Call drv_alba_mstate_get.Result = %d",lResult);
    if (lResult == DRV_BSP_OK) 
    {
        *pulMState = ucMState;
    }
    return lResult;
}

/***************************************************************************
* 函数名称: smss_abox_mute_set
* 功    能: 告警箱清音状态设置
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* ulMute          UINT32                  输入
* 函数返回: 
* UINT32
* 说    明: 
***************************************************************************/
UINT32 smss_abox_mute_set(UINT32 ulMute)
{
    INT32 lResult;
    lResult = drv_alba_mute_set((UINT8)ulMute);
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Call drv_alba_mute_set.Result = %d",lResult);
    return lResult;
}
/***************************************************************************
* 函数名称: smss_abox_mute_get
* 功    能: 告警箱清音状态查询
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* pulMute         UINT32 *                输出
* 函数返回: 
* UINT32
* 说    明: 
***************************************************************************/
UINT32 smss_abox_mute_get(UINT32 *pulMute)
{
    UINT8 ucMute;
    INT32 lResult;

    lResult = drv_alba_mute_get(&ucMute);
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Call drv_alba_mute_get.Result = %d",lResult);

    if (lResult == DRV_BSP_OK) 
    {
        *pulMute = ucMute;
    }
    return lResult;
}

/***************************************************************************
* 函数名称: smss_abox_mute_get
* 功    能: 告警箱生产制造信息查询
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* pstManuInfo     CPS__OAMS_ABOX_MANU_INFO_T * 输出
* 函数返回: 
* UINT32
* 说    明: 
***************************************************************************/
UINT32 smss_abox_manu_info_get(SMSS_ABOX_MANU_INFO_T *pstManuInfo)
{
    /* 地址清0 */
    cpss_mem_memset(pstManuInfo,0,sizeof(SMSS_ABOX_MANU_INFO_T));

    /* 得到单板生产制造信息 */
    drv_board_manu_info_get((DRV_PHYBRD_INTEGRATED_INFO_MANU_T *)pstManuInfo); 

    return SMSS_OK;
}

/***************************************************************************
* 函数名称: smss_abox_ver_info_get
* 功    能: 告警箱软件版本信息查询
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* pstVerInfo      CPS__OAMS_ABOX_VER_INFO_T *  输出
* 函数返回: 
* UINT32
* 说    明: 
***************************************************************************/
UINT32 smss_abox_ver_info_get(SMSS_ABOX_VER_INFO_T *pstVerInfo)
{
     strcpy(pstVerInfo->acRunVerFileName,g_stAboxCurVerInfo.astVerInfo[0].aucFileName);
     return SMSS_OK;   
}

/***************************************************************************
* 函数名称: smss_abox_reset
* 功    能:  告警箱复位 
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_abox_reset(VOID)
{
    drv_board_reset();
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Call drv_board_reset.");
    
}
/***************************************************************************
* 函数名称: smss_abox_test
* 功    能:  告警箱检测 
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
UINT32 smss_abox_test(VOID)  /* 阻塞方式 */
{
    drv_alba_test();
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Call drv_alba_test.");
    return SMSS_OK;
}

/***************************************************************************
* 函数名称: smss_abox_sound_alarm
* 功    能:  告警箱声音告警 
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* ulSoundLevel    UINT32                  输入
* 函数返回: 
* UINT32。
* 说    明: 
***************************************************************************/
UINT32 smss_abox_sound_alarm(UINT32 ulSoundLevel) /* 参数取值：0-3 */
{   
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Call drv_alba_sound_alm_set.");
    return(drv_alba_sound_alm_set(((UINT8)ulSoundLevel)));
}

/***************************************************************************
* 函数名称smss_abox_light_alarm
* 功    能:  告警箱灯光告警 
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* ulCritLevel     UINT32                  输入
* ulMajorLevel    UINT32                   输入
* ulMinorLevel    UINT32                  输入
* ulWarnLevel     UINT32                  输入
* 函数返回: 
* UINT32。
* 说    明: 
***************************************************************************/
UINT32 smss_abox_light_alarm(UINT32 ulCritLevel, UINT32 ulMajorLevel, UINT32 ulMinorLevel, UINT32 ulWarnLevel) /* 各参数取值：0-3 */
{
    INT32 lResult;
    lResult = drv_alba_light_alm_set((UINT8)ulCritLevel, (UINT8)ulMajorLevel, 
                           (UINT8)ulMinorLevel, (UINT8)ulWarnLevel);
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Call drv_alba_light_alm_set.Result = %d",lResult);
    return lResult;
}

/***************************************************************************
* 函数名称: smss_set_ip_addr
* 功    能:  告警箱声音告警 
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* ulSoundLevel    UINT32                  输入
* 函数返回: 
* UINT32。
* 说    明: 
***************************************************************************/
#define SMSS_ABOX_INTERFACE_NAME "abox_interface"
UINT32 smss_set_ip_addr(STRING szIpAddr, UINT32 ulSubnetMask)
{
    UINT32 ulResult = 0;
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
    ulResult = ifAddrSet(SMSS_ABOX_INTERFACE_NAME,szIpAddr);
#endif
    if (0 != ulResult ) 
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "Call ifAddrSet.Result = %d",ulResult);
    }
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
    ulResult = ifMaskSet(SMSS_ABOX_INTERFACE_NAME,ulSubnetMask);
#endif
    if (0 != ulResult ) 
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "Call ifMaskSet.Result = %d",ulResult);
    }
    return ulResult;
}


UINT32 smss_abox_linkstate_set(UINT32 ulLinkState)
{
    UINT32 ulResult;
    ulResult = drv_alba_linkstate_led_set((UINT8)ulLinkState);
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Call drv_alba_linkstate_led_set.ulLinkState = %d, Result = %d",ulLinkState,ulResult);
    return ulResult;
}


/***************************************************************************
* 函数名称: smss_devm_drv_init
* 功    能: 由CPSS调用，实现告警箱驱动初始化
* 函数类型  UINT32
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 函数返回: 
* 说    明: 实现收到POWER_ON之前的初始化
***************************************************************************/
UINT32 smss_devm_drv_init(VOID)
{ 
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST) /* 主CPU */
    UINT32 ulSmssDevmDrvInitError;

  /*  UINT8  aucMac[6];
    UINT32 ulIp; */

    ulSmssDevmDrvInitError = (UINT32)drv_host_cpu_init();
    if (DRV_BSP_OK != ulSmssDevmDrvInitError)
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_FATAL,
            "%s() failed: 0x%08X.",
            "drv_host_cpu_init", ulSmssDevmDrvInitError);
        return ulSmssDevmDrvInitError;
    }

    /* 控制面通道IPV4的通信端口的初始化 
    ulSmssDevmDrvInitError = (UINT32)drv_ether_port_comm_init(DRV_ETHER_CTRL_PORT_IPV4, aucMac, &ulIp);
    if (DRV_BSP_OK != ulSmssDevmDrvInitError)
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_FATAL,
            "%s(%s) failed: 0x%08X.", "drv_ether_port_comm_init",
            "CTRL", ulSmssDevmDrvInitError);
        return ulSmssDevmDrvInitError;
    }*/

#endif 
    return SMSS_OK;    
}

/***************************************************************************
* 函数名称: abox_ip_set
* 功    能: 告警箱IP及子网掩码设置
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* pIpAddr         UINT8*                  输入
* pIpAddr         UINT8*                  输入
* 函数返回: 
* UINT32。
* 说    明: 
***************************************************************************/
INT32 abox_ip_set(UINT8 *pucIpAddr,UINT8 *pucSubnetMask)
{
    INT32 lRet;
    UINT32 ulIpAddr;
    UINT32 ulSubnetMask;
    ulIpAddr = inet_addr(pucIpAddr);
    if(ulIpAddr==ERROR)
    {
       cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "pucIpAddr %s is invalid",pucIpAddr);
       return SMSS_ERROR;
    }
    ulSubnetMask = inet_addr(pucSubnetMask);
    if(ulSubnetMask==ERROR)
    {
       cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "pucSubnetMask %s is invalid",pucSubnetMask);
       return SMSS_ERROR;
    }

    lRet = drv_alba_boot_ip_set(ulIpAddr,ulSubnetMask);

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FAIL,
            "Call drv_alba_boot_ip_set.Result = %d ",lRet);
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FAIL,
            "if need this operation valid,must restart the board!!");
    return lRet;
}
/***************************************************************************
* 函数名称: abox_mac_set
* 功    能: 告警箱MAC地址设置
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* pucMacAddr         UINT8*               输入
* pIpAddr         UINT8*                  输入
* 函数返回: 
* UINT32。
* 说    明: 
***************************************************************************/
INT32 abox_mac_set(UINT8 *pucMacAddr)
{
    INT32 lRet;
    UINT8 aucMacAddr[6];
    UINT32 aulMacAddr[6];
    UINT8 i;
    lRet = sscanf(pucMacAddr,"%x-%x-%x-%x-%x-%x",
        &aulMacAddr[0],&aulMacAddr[1],&aulMacAddr[2],
        &aulMacAddr[3],&aulMacAddr[4],&aulMacAddr[5]);
    if(lRet != 6)
    {
         cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "pucMacAddr %s is invalid",pucMacAddr);
         return SMSS_ERROR;
    }

    for(i=0; i<6; i++)
    {
        aucMacAddr[i] = (UINT8)aulMacAddr[i];
    }
        
    lRet = drv_alba_boot_mac_set(aucMacAddr);

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FAIL,
            "Call drv_alba_boot_mac_set.Result = %d ",lRet);
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FAIL,
            "if need this operation valid,must restart the board!!");
    return lRet;
}

/***************************************************************************
* 函数名称: smss_abox_route_add
* 功    能: 告警箱路由增加
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* pucIp1          UINT8*                  IN              目标IP1
* pucIp2          UINT8*                  IN              目标IP2
* pucGateWay1     UINT8*                  IN              GateWay to ONCA1
* pucGateWay2     UINT8*                  IN              GateWay to ONCA2
* 函数返回: 
* 说    明: SHELL
***************************************************************************/
VOID smss_abox_route_add(UINT8* pucIp1, UINT8* pucIp2, UINT8* pucGateWay1, UINT8* pucGateWay2)
{
#if (SWP_OS_TYPE != SWP_OS_WINDOWS)
    UINT32 ulFd;
    UINT32 ulLen;  

    /* 如果原来有路由，则删除原有路由 */
    if (g_stAboxRout.ucExist == 1) 
    {
        routeDelete(g_stAboxRout.ucDesIp1,g_stAboxRout.ucGateWay1);
        routeDelete(g_stAboxRout.ucDesIp2,g_stAboxRout.ucGateWay2);
        g_stAboxRout.ucExist = 0;
    }
    /* 增加新的路由 */
    if(ERROR == routeAdd(pucIp1,pucGateWay1))
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "GateWay1: %s is invalid.", pucGateWay1);
    }
    
    if(ERROR == routeAdd(pucIp2,pucGateWay2))
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "GateWay2: %s is invalid.", pucGateWay2);
    }
 
    cpss_mem_memset(&g_stAboxRout,0,sizeof(g_stAboxRout));    
    /* 设置保存路由信息的全局变量 */
    g_stAboxRout.ucExist = 1;
    cpss_mem_memcpy(g_stAboxRout.ucGateWay1,pucGateWay1,strlen(pucGateWay1));
    cpss_mem_memcpy(g_stAboxRout.ucGateWay2,pucGateWay2,strlen(pucGateWay2));
    cpss_mem_memcpy(g_stAboxRout.ucDesIp1,pucIp1,strlen(pucIp1));
	cpss_mem_memcpy(g_stAboxRout.ucDesIp2,pucIp2,strlen(pucIp2));
    
    /* 将路由信息写入文件 */
    ulFd = cpss_file_open(szRoutInfoFile,
        CPSS_FILE_OWRONLY | CPSS_FILE_OBINARY | CPSS_FILE_OCREATE | CPSS_FILE_OTRUNC);
    if (ulFd == CPSS_FD_INVALID) /* create fail */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "Create file F:/smss/route.dat error");
        return;
    }
    cpss_file_write(ulFd, &g_stAboxRout, sizeof(g_stAboxRout), &ulLen);      
    cpss_file_close(ulFd); 
#endif
    return;
}

/***************************************************************************
* 函数名称: smss_abox_route_init
* 功    能: 告警箱路由初始化
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 函数返回: 
* 说    明: 初始化调用
***************************************************************************/
VOID smss_abox_route_init()
{
#if (SWP_OS_TYPE != SWP_OS_WINDOWS)    
    
    UINT32 ulFd;
    UINT32 ulLen;  
    
    /* 初始化路由信息全局变量 */
    cpss_mem_memset(&g_stAboxRout,0,sizeof(g_stAboxRout));

    /* 从文件读取路由信息 */
    ulFd = cpss_file_open(szRoutInfoFile, CPSS_FILE_ORDONLY | CPSS_FILE_OBINARY);
    if (ulFd == CPSS_FD_INVALID)  /* file not exist */
    {
        return;
    }
    
    cpss_file_read(ulFd, &g_stAboxRout, sizeof(g_stAboxRout), &ulLen);    
    cpss_file_close(ulFd); 
    
    /* yanghuanjun 20070420 begin */
    /* 设置通讯网口 */
    if (g_stAboxRout.ulIp != 0)
    {
      drv_alba_com_ip_set(g_stAboxRout.ulIp, g_stAboxRout.ulMask);	
    }
    /* yanghuanjun 20070420 end */
    
    /* 如果原来不存在路由信息，直接返回 */
    if (g_stAboxRout.ucExist == 0) 
    {
        return;
    }

    /* 增加路由信息 */    
    if(ERROR == routeAdd(g_stAboxRout.ucDesIp1,g_stAboxRout.ucGateWay1))
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "GateWay1: %s is invalid.", g_stAboxRout.ucGateWay1);
    }
    
    if(ERROR == routeAdd(g_stAboxRout.ucDesIp2,g_stAboxRout.ucGateWay2))
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "GateWay1: %s is invalid.", g_stAboxRout.ucGateWay2);
    }
#endif
    return;
}

/***************************************************************************
* 函数名称: smss_abox_route_show
* 功    能: 告警箱路由显示
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 函数返回: 
* 说    明: 调试调用
***************************************************************************/
VOID smss_abox_route_show()
{
    if (0 == g_stAboxRoute.ucRouteExist) 
    {
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "Route Info not Exist!\n");
        return;
    }
    
    cps__oams_shcmd_printf("Route Info:\n");	
    cps__oams_shcmd_printf("PortType     = %d\n",g_stAboxRoute.ulPortType);
    cps__oams_shcmd_printf("route switch = %d\n",g_stAboxRoute.ulIfRouteSwitch);
    cps__oams_shcmd_printf("IP1          = %s\n",g_stAboxRoute.aucDesIp1);
    cps__oams_shcmd_printf("Mask1        = %s\n",g_stAboxRoute.aucMask1);
    cps__oams_shcmd_printf("MacAddr      = %s\n",g_stAboxRoute.aucMacAddr);
    cps__oams_shcmd_printf("IP2          = %s\n",g_stAboxRoute.aucDesIp2);
    cps__oams_shcmd_printf("Mask2        = %s\n\n",g_stAboxRoute.aucMask2);
    cps__oams_shcmd_printf("GateWay1     = %s\n",g_stAboxRoute.aucGateWay1);
    cps__oams_shcmd_printf("GateWay2     = %s\n",g_stAboxRoute.aucGateWay2);
    cps__oams_shcmd_printf("GCPA1        = %s\n",g_stAboxRoute.aucGCPA1);
    cps__oams_shcmd_printf("GCPA2        = %s\n",g_stAboxRoute.aucGCPA2);
    return;
}

/***************************************************************************
* 函数名称: smss_abox_route_query
* 功    能: 获取告警箱路由信息
* 函数类型  INT32
* 参    数: 
* 参数名称             类型                           输入/输出       描述
pstOamsRouteInfo  SMSS_ABOX_ROUTE_QUERY_T *    out           告警箱路由
* 函数返回: 
* 说    明: 提供给OAMS
***************************************************************************/
INT32 smss_abox_route_query(SMSS_ABOX_ROUTE_QUERY_T *pstOamsRouteInfo)
{
    if (NULL == pstOamsRouteInfo)
    {
        return SMSS_ERROR;
    }
    if (0 == g_stAboxRoute.ucRouteExist)
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "Route Info not Exist!\n.");
	return SMSS_ERROR;
    }

    pstOamsRouteInfo->ulIfRouteSwitch = g_stAboxRoute.ulIfRouteSwitch;

    cpss_mem_memcpy(pstOamsRouteInfo->aucDesIp1,g_stAboxRoute.aucGCPA1,256);
    cpss_mem_memcpy(pstOamsRouteInfo->aucGateWay1,g_stAboxRoute.aucGateWay1,256);
	
    cpss_mem_memcpy(pstOamsRouteInfo->aucDesIp2,g_stAboxRoute.aucGCPA2,256);
    cpss_mem_memcpy(pstOamsRouteInfo->aucGateWay2,g_stAboxRoute.aucGateWay2,256);
		
    return SMSS_OK;
}

/***************************************************************************
* 函数名称: smss_sysctl_abox_configure
* 功    能: 
* 函数类型  INT32
* 参    数: 
* 参数名称             类型                           输入/输出       描述
* 函数返回: 
* 说    明: 
***************************************************************************/
INT32 smss_sysctl_abox_configure()
{
    UINT32 ulFd;
    UINT32 ulLen;
    UINT8 aucMac[6];
    UINT32 ulSmssDevmDrvInitError;
    UINT32 ulIp1;
    UINT32 ulIp2;
    UINT32 ulMaskAddr2;

    ulFd = cpss_file_open(szRoutInfoFile,CPSS_FILE_ORDONLY | CPSS_FILE_OBINARY);
    if (CPSS_FD_INVALID == ulFd)
    {
	return SMSS_ERROR;
    }
   /* 初始化全局结构 */
    cpss_mem_memset(&g_stAboxRoute,0,sizeof(g_stAboxRoute));
    cpss_file_read(ulFd,&g_stAboxRoute,sizeof(g_stAboxRoute),&ulLen);
    cpss_file_close(ulFd);

    if (sizeof(g_stAboxRoute) != ulLen)
    {
        return SMSS_ERROR;
    }

    if (0 == inet_addr(g_stAboxRoute.aucGateWay1) || 0 == inet_addr(g_stAboxRoute.aucGateWay2)
		|| 0 == inet_addr(g_stAboxRoute.aucGCPA1) || 0 == inet_addr(g_stAboxRoute.aucGCPA2))
    {
        cpss_print(SWP_SUBSYS_SMSS,SMSS_MODULE_SYSCTL,CPSS_PRINT_INFO,"no route info");
		return SMSS_ERROR;
    }

    /* 获取全局结构中的ip信息 */
    ulIp1 = inet_addr(g_stAboxRoute.aucDesIp1);
    ulIp2 = inet_addr(g_stAboxRoute.aucDesIp2);
    ulMaskAddr2 = inet_addr(g_stAboxRoute.aucMask2);
   

    #if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
    /* 单网口初始化 */
    if (0 == g_stAboxRoute.ulPortType)
    {
        ulSmssDevmDrvInitError = (UINT32)drv_ether_port_comm_init(DRV_ETHER_CTRL_PORT_IPV4,aucMac,&ulIp1);
	if (DRV_BSP_OK != ulSmssDevmDrvInitError)
	{
		cpss_print(SWP_SUBSYS_SMSS,SMSS_MODULE_DEVM,CPSS_PRINT_INFO,"%s(%s) faied: 0x%08X.",
				"drv_ether_port_comm_init","CTRL",ulSmssDevmDrvInitError);
		return ulSmssDevmDrvInitError;
	}

	drv_alba_com_ip_add(ulIp2,ulMaskAddr2);
    }
    /* 双网口初始化 */
    else
    {
        ulSmssDevmDrvInitError = (UINT32)drv_ether_port_comm_init(DRV_ETHER_CTRL_PORT_IPV4,aucMac,&ulIp1);
	if (DRV_BSP_OK != ulSmssDevmDrvInitError)
	{
		cpss_print(SWP_SUBSYS_SMSS,SMSS_MODULE_DEVM,CPSS_PRINT_INFO,"%s(%s) faied: 0x%08X.",
				"drv_ether_port_comm_init","CTRL",ulSmssDevmDrvInitError);
		return ulSmssDevmDrvInitError;
	}

        ulSmssDevmDrvInitError = (UINT32)drv_ether_port_comm_init(DRV_ETHER_DEBUG_PORT_IPV4,aucMac,&ulIp2);
        if (DRV_BSP_OK != ulSmssDevmDrvInitError)
	{
		cpss_print(SWP_SUBSYS_SMSS,SMSS_MODULE_DEVM,CPSS_PRINT_INFO,"%s(%s) faied: 0x%08X.",
				"drv_ether_port_comm_init","CTRL",ulSmssDevmDrvInitError);
		return ulSmssDevmDrvInitError;
	}
    }
    #endif
    return SMSS_OK;
}

/***************************************************************************
* 函数名称: smss_abox_ip_set
* 功    能: 设置ALBA板的ip信息
* 函数类型  INT32
* 参    数: 
* 参数名称             类型                输入/输出       描述
ulPortType            UINT32                 输入          单双网口配置标志
   pucIp1             UINT8 *                输入           ip1
  pucMask1            UINT8 *                输入           掩码1
 pucMacAddr           UINT8 *                输入           mac地址
   pucIp2             UINT8 *                输入           ip2
  pucMask2            UINT8 *                输入           掩码2
* 函数返回: 参数检验设置成功返回SMSS_OK,否则返回SMSS_ERROR;
* 说    明: 校验手动输入参数,并设置ip
***************************************************************************/
INT32 smss_abox_ip_set(UINT32 ulPortType,UINT8* pucIp1,UINT8* pucMask1, UINT8* pucMacAddr,UINT8* pucIp2,UINT8* pucMask2)
{ 
    UINT8 i;
    UINT8 aucMacAddr[6];
    INT32 lRet;
    UINT32 ulIpAddr1;
    UINT32 ulIpAddr2;
    UINT32 ulMask1;
    UINT32 ulMask2;
    UINT32 aulMacAddr[6];
    UINT32 ulFd;
    UINT32 ulLen;
    UINT32 ulNet1;
    UINT32 ulNet2;
    UINT32 ulIpAddr;
    UINT32 ulMask;

    cps__oams_shcmd_printf("ulPortType: %d\n", ulPortType);
    cps__oams_shcmd_printf("pucIp1: %s\n", pucIp1);
    cps__oams_shcmd_printf("pucMask1: %s\n", pucMask1);
    cps__oams_shcmd_printf("pucMacAddr: %s\n", pucMacAddr);
    cps__oams_shcmd_printf("pucIp2: %s\n", pucIp2);
    cps__oams_shcmd_printf("pucMask2: %s\n", pucMask2);
	
    if (NULL == pucIp1 || NULL == pucMask1 || NULL == pucMacAddr || NULL == pucIp2 ||NULL == pucMask2)
    {
        cps__oams_shcmd_printf("input null\n");
	return SMSS_ERROR;
    }
  	
	/* 转换为本地序*/
    ulIpAddr1 = cpss_ntohl(inet_addr(pucIp1));
    ulMask1 = cpss_ntohl(inet_addr(pucMask1));
    ulIpAddr2 = cpss_ntohl(inet_addr(pucIp2));
    ulMask2 = cpss_ntohl(inet_addr(pucMask2));

    if (ulIpAddr1 == ERROR || ulMask1 == ERROR || ulIpAddr2 == ERROR || ulMask2 == ERROR)
    {
        cps__oams_shcmd_printf("para error pucIp1 %s,  pucMask1 %s, pucIp2 %s, pucMask2 %s\n",
                      pucIp1, pucMask1, pucIp2, pucMask2);
        return SMSS_ERROR;    
    }
    
    ulNet1 = ulIpAddr1 & ulMask1;
    ulNet2 = ulIpAddr2 & ulMask2;

    /* 判断ip1和ip2是否在同一网段 */
    if (ulNet1 == ulNet2)
    {
        cps__oams_shcmd_printf("Ip1 and Ip2 in the same net\n");
	return SMSS_ERROR;
    }

    /* 获取mac地址,对其进行判断  */
    lRet = sscanf(pucMacAddr,"%x-%x-%x-%x-%x-%x",&aulMacAddr[0],
		&aulMacAddr[1],&aulMacAddr[2],&aulMacAddr[3],&aulMacAddr[4],
		&aulMacAddr[5]);

    if (6 != lRet)
    {
        cps__oams_shcmd_printf("pucMacAddr %s is invalid\n",pucMacAddr);
	return SMSS_ERROR;
    }

    for (i = 0; i < 6; i++)
    {
	aucMacAddr[i] = (UINT8)aulMacAddr[i];
    }

    cpss_mem_memset(g_stAboxRoute.aucDesIp1,0,256);
    cpss_mem_memset(g_stAboxRoute.aucDesIp2,0,256);
    cpss_mem_memset(g_stAboxRoute.aucMask1,0,256);
    cpss_mem_memset(g_stAboxRoute.aucMask2,0,256);
    cpss_mem_memset(g_stAboxRoute.aucMacAddr,0,256);
    	
	/* 检验无误,将信息保存至全局结构g_stAboxRoute中 */    
    g_stAboxRoute.ucRouteExist = 1;    
    g_stAboxRoute.ulPortType = ulPortType;

    cpss_mem_memcpy(g_stAboxRoute.aucDesIp1,pucIp1,256);
    cpss_mem_memcpy(g_stAboxRoute.aucDesIp2,pucIp2,256);
    cpss_mem_memcpy(g_stAboxRoute.aucMask1,pucMask1,256);
    cpss_mem_memcpy(g_stAboxRoute.aucMask2,pucMask2,256);
    cpss_mem_memcpy(g_stAboxRoute.aucMacAddr,pucMacAddr,256);

    /* 写文件 */
    ulFd = cpss_file_open(szRoutInfoFile,
		CPSS_FILE_OWRONLY|CPSS_FILE_OBINARY|CPSS_FILE_OCREATE|CPSS_FILE_OTRUNC);
    /* 打开文件失败 */
    if (CPSS_FD_INVALID == ulFd)
    {
        cps__oams_shcmd_printf("file not exist\n");
	return SMSS_ERROR;
    }

    cpss_file_write(ulFd,&g_stAboxRoute,sizeof(g_stAboxRoute),&ulLen);
    cpss_file_close(ulFd);

    ulIpAddr = inet_addr(pucIp1);
    ulMask = inet_addr(pucMask1);
    /* 设置ip,mac地址 */
    ulIpAddr = inet_addr(pucIp1);
    ulMask = inet_addr(pucMask1);

    lRet = drv_alba_boot_ip_set(ulIpAddr,ulMask);
    if (DRV_BSP_OK != lRet)
    {
        cps__oams_shcmd_printf("call drv_alba_boot_ip_set. Result = %d",lRet);
	return SMSS_ERROR;
    }

    lRet = drv_alba_boot_mac_set(aucMacAddr);
    if (DRV_BSP_OK != lRet)
    {
        cps__oams_shcmd_printf("call drv_alba_boot_mac_set. Result = %d",lRet);
	return SMSS_ERROR;
    }
    cps__oams_shcmd_printf("\nIP SET SUCCESS.\n");
    return SMSS_OK;
}

/***************************************************************************
* 函数名称: smss_abox_route_set
* 功    能: 告警箱路由设置
* 函数类型  INT32
* 参    数: 
* 参数名称             类型                           输入/输出       描述
* 函数返回: 
* 说    明: 
***************************************************************************/
INT32 smss_abox_route_set(UINT32 ulIfRouteSwitch,UINT8 *pucGateWay1,UINT8 *pucGCPA1,
					   UINT8 *pucGateWay2,UINT8 *pucGCPA2)
{
    UINT32 ulGateWay1;
    UINT32 ulGateWay2;
    UINT32 ulGCPA1;
    UINT32 ulGCPA2;
    UINT32 ulFd;
    UINT32 ulLen;
    UINT32 ulIp1;
    UINT32 ulIp2;
    UINT32 ulMaskAddr1;
    UINT32 ulMaskAddr2;
    UINT32 ulG1M1,ulG1M2,ulG2M1,ulG2M2;
    UINT32 ulGp1M1,ulGp1M2,ulGp2M1,ulGp2M2;
    UINT32 ulIp1M1,ulIp1M2,ulIp2M1,ulIp2M2;

    cps__oams_shcmd_printf("ulIfRouteSwitch: %d\n", ulIfRouteSwitch);
    cps__oams_shcmd_printf("pucGateWay1: %s\n", pucGateWay1);
    cps__oams_shcmd_printf("pucGCPA1: %s\n", pucGCPA1);
    cps__oams_shcmd_printf("pucGateWay2: %s\n", pucGateWay2);
    cps__oams_shcmd_printf("pucGCPA2: %s\n", pucGCPA2);
	
	/* 转换为本地序 */
    ulGateWay1 = cpss_ntohl(inet_addr(pucGateWay1));
    ulGateWay2 = cpss_ntohl(inet_addr(pucGateWay2));
    ulGCPA1 = cpss_ntohl(inet_addr(pucGCPA1));
    ulGCPA2 = cpss_ntohl(inet_addr(pucGCPA2));

    if (ulGateWay1 == ERROR || ulGateWay2 == ERROR || ulGCPA1 == ERROR || ulGCPA2 == ERROR)
    {
        cps__oams_shcmd_printf("para error pucGateWay1 %s,  pucGateWay2 %s, pucGCPA1 %s, pucGCPA2 %s\n",
                      pucGateWay1, pucGateWay2, pucGCPA1, pucGCPA2);
        return SMSS_ERROR;    
    }
    
    /* 网关校验 */
    if (ulGateWay1 == ulGateWay2)
    {
        cps__oams_shcmd_printf("pucGateWay1 = pucGateWay2 %s\n",pucGateWay1);
	return SMSS_ERROR;
    }

   /* 获取先前输入的ip和掩码 */
    ulIp1 = cpss_ntohl(inet_addr(g_stAboxRoute.aucDesIp1));
    ulIp2 = cpss_ntohl(inet_addr(g_stAboxRoute.aucDesIp2));
    ulMaskAddr1 = cpss_ntohl(inet_addr(g_stAboxRoute.aucMask1));
    ulMaskAddr2 = cpss_ntohl(inet_addr(g_stAboxRoute.aucMask2));
    
	/* 网关检验 */
    ulG1M1 = ulGateWay1 & ulMaskAddr1;
    ulG1M2 = ulGateWay1 & ulMaskAddr2;
    ulG2M1 = ulGateWay2 & ulMaskAddr1;
    ulG2M2 = ulGateWay2 & ulMaskAddr2;

    ulIp1M1 = ulIp1 & ulMaskAddr1;
    ulIp1M2 = ulIp1 & ulMaskAddr2;
    ulIp2M1 = ulIp2 & ulMaskAddr1;
    ulIp2M2 = ulIp2 & ulMaskAddr2;

    if (ulG1M1 != ulIp1M1 &&
        ulG1M1 != ulIp1M2 &&
        ulG1M1 != ulIp2M1 &&
        ulG1M1 != ulIp2M2 &&
		ulG1M2 != ulIp1M1 &&
        ulG1M2 != ulIp1M2 &&
        ulG1M2 != ulIp2M1 &&
        ulG1M2 != ulIp2M2
        )
    {
        cps__oams_shcmd_printf("pucGateWay1 %s check error\n",pucGateWay1);
	return SMSS_ERROR;
    }

    if (ulG2M1 != ulIp1M1 &&
        ulG2M1 != ulIp1M2 &&
        ulG2M1 != ulIp2M1 &&
        ulG2M1 != ulIp2M2 &&
		ulG2M2 != ulIp1M1 &&
        ulG2M2 != ulIp1M2 &&
        ulG2M2 != ulIp2M1 &&
        ulG2M2 != ulIp2M2
        )
    {
        cps__oams_shcmd_printf("pucGateWay2 %s check error\n",pucGateWay2);
	return SMSS_ERROR;
    }

	/* GCPA校验 */
    if (ulGCPA1 == ulGCPA2)
    {
        cps__oams_shcmd_printf("pucGCPA1 %s = pucGCPA2 %s \n",pucGCPA1,pucGCPA2);
	return SMSS_ERROR;
    }

    ulGp1M1 = ulGCPA1 & ulMaskAddr1;
    ulGp1M2 = ulGCPA1 & ulMaskAddr2;

    ulGp2M1 = ulGCPA2 & ulMaskAddr1;
    ulGp2M2 = ulGCPA2 & ulMaskAddr2;

    if (ulGp1M1 == ulIp1M1 || ulGp1M1 == ulIp1M2 || ulGp1M1 == ulIp2M1 || ulGp1M1 == ulIp2M2 ||
        ulGp1M2 == ulIp1M1 || ulGp1M2 == ulIp1M2 || ulGp1M2 == ulIp2M1 || ulGp1M2 == ulIp2M2)
    {
        cps__oams_shcmd_printf("pucGCPA1 %s check error\n",pucGCPA1);
	return SMSS_ERROR;
    }

    if (ulGp2M1 == ulIp1M1 || ulGp2M1 == ulIp1M2 || ulGp2M1 == ulIp2M1 || ulGp2M1 == ulIp2M2 ||
        ulGp2M2 == ulIp1M1 || ulGp2M2 == ulIp1M2 || ulGp2M2 == ulIp2M1 || ulGp2M2 == ulIp2M2)
    {
        cps__oams_shcmd_printf("pucGCPA2 %s check error\n",pucGCPA2);
	return SMSS_ERROR;
    }

    /* 校验成功,保存信息至全局结构中 */
    g_stAboxRoute.ucRouteExist = 1;
    g_stAboxRoute.ulIfRouteSwitch = ulIfRouteSwitch;
    cpss_mem_memset(g_stAboxRoute.aucGateWay1,0,256);
    cpss_mem_memset(g_stAboxRoute.aucGateWay2,0,256);
    cpss_mem_memset(g_stAboxRoute.aucGCPA1,0,256);
    cpss_mem_memset(g_stAboxRoute.aucGCPA2,0,256);

    cpss_mem_memcpy(g_stAboxRoute.aucGateWay1,pucGateWay1,256);
    cpss_mem_memcpy(g_stAboxRoute.aucGateWay2,pucGateWay2,256);
    cpss_mem_memcpy(g_stAboxRoute.aucGCPA1,pucGCPA1,256);
    cpss_mem_memcpy(g_stAboxRoute.aucGCPA2,pucGCPA2,256);

    /* 写文件 */
    ulFd = cpss_file_open(szRoutInfoFile,
		CPSS_FILE_OWRONLY|CPSS_FILE_OBINARY|CPSS_FILE_OCREATE|CPSS_FILE_OTRUNC);
    if (ulFd == CPSS_FD_INVALID)
    {
        cps__oams_shcmd_printf("pucGateWay1 %s pucGateWay2 %s pucGCPA1 %s pucGCPA2 %s is invalid\n",
                         pucGateWay1,pucGateWay2,pucGCPA1,pucGCPA2);
	return SMSS_ERROR;
    }

    cpss_file_write(ulFd,&g_stAboxRoute,sizeof(g_stAboxRoute),&ulLen);
    cpss_file_close(ulFd);
    cps__oams_shcmd_printf("\nROUTE SET SUCCESS, PLEASE REBOOT...\n");
    return SMSS_OK;
}

/***************************************************************************
* 函数名称: smss_com_ip_set
* 功    能: 告警箱通讯网口IP设置.
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
*          
* 函数返回: 
***************************************************************************/
INT32 smss_com_ip_set(UINT8 *pucIpAddr,UINT8 *pucSubnetMask)
{
    INT32 lRet;
    UINT32 ulIpAddr;
    UINT32 ulSubnetMask;
    UINT32 ulFd;
    UINT32 ulLen; 
        
    ulIpAddr = inet_addr(pucIpAddr);
    if(ulIpAddr == ERROR)
    {
       cps__oams_shcmd_printf("pucIpAddr '%s' is invalid\n",pucIpAddr);
       return SMSS_ERROR;
    }
    
    ulSubnetMask = inet_addr(pucSubnetMask);
    if(ulSubnetMask == ERROR)
    {
       cps__oams_shcmd_printf("pucSubnetMask '%s' is invalid\n",pucSubnetMask);
       return SMSS_ERROR;
    }
    
    lRet = drv_alba_com_ip_set(ulIpAddr, ulSubnetMask);
    if (lRet != DRV_BSP_OK)
    {
    	cps__oams_shcmd_printf("set com ip 0x%x, mask 0x%x error\n",ulIpAddr, ulSubnetMask);
    	return SMSS_ERROR;
    }
    
    /* 记录信息 */
    g_stAboxRout.ulIp = ulIpAddr;
    g_stAboxRout.ulMask = ulSubnetMask;
    
    /* 写文件 */
    ulFd = cpss_file_open(szRoutInfoFile,
        CPSS_FILE_OWRONLY | CPSS_FILE_OBINARY | CPSS_FILE_OCREATE | CPSS_FILE_OTRUNC);
    if (ulFd == CPSS_FD_INVALID) /* create fail */
    {
        cps__oams_shcmd_printf("Create file F:/smss/route.dat error\n");
        return SMSS_ERROR;
    }
    cpss_file_write(ulFd, &g_stAboxRout, sizeof(g_stAboxRout), &ulLen);      
    cpss_file_close(ulFd); 
    
    cps__oams_shcmd_printf("set com ip '%s', mask '%s' ok\n",pucIpAddr, pucSubnetMask);
  	
  	return SMSS_OK;
}

VOID smss_com_ip_show(VOID)
{
	 cps__oams_shcmd_printf("com ip 0x%x, mask 0x%x\n",g_stAboxRout.ulIp, g_stAboxRout.ulMask);
}
/*  yanghuanjun 20070420 end */

#endif /* 告警箱处理 */
BOOL  smss_get_proc_active_state(VOID)
{
    if(SMSS_RSTATUS_NORMAL == g_stSmssBaseInfo.ucRStatus)
       return TRUE;
    else
       return FALSE;
}
#if 0 //dhwang added
void cps_reboot_msg_deal(OM_AC_CPU_REBOOT_T* stOmReboot)
{
	CPSS_COM_PID_T stPid;
	CPS_REBOOT_T stRbt;

	if(NULL == stOmReboot)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR, "receive NULL msg in cps_reboot_msg_deal!\n");
		return;
	}

#if 0
	switch(stOmReboot->stPhyAddr.ucSlot)
	{
		case 1:
			stPid.stLogicAddr.ucModule = 16;
			stPid.stLogicAddr.usGroup = 1;
			stPid.stLogicAddr.ucSubGroup = 1;
			break;
		case 2:
			switch(stOmReboot->stPhyAddr.ucCpu)
			{
				case 1:
					stPid.stLogicAddr.ucModule = 16;
					stPid.stLogicAddr.usGroup = 2;
					stPid.stLogicAddr.ucSubGroup = 1;
					break;
				case 2:
					stPid.stLogicAddr.ucModule = 16;
					stPid.stLogicAddr.usGroup = 2;
					stPid.stLogicAddr.ucSubGroup = 2;
					break;
				case 3:
					stPid.stLogicAddr.ucModule = 16;
					stPid.stLogicAddr.usGroup = 2;
					stPid.stLogicAddr.ucSubGroup = 3;
					break;
				default:
					break;
			}
			break;
		default:
			break;
	}

	stPid.ulAddrFlag = 0;
#endif

	cpss_com_logic_addr_get(&stPid.stLogicAddr, &stPid.ulAddrFlag);

	stPid.ulPd = SMSS_SYSCTL_PROC;
	stRbt.ucRebtType = stOmReboot->ucRebootType;
	stRbt.ucRebtRsn = CPS_REBOOT_BY_OM;
	stRbt.szExtra = "om ind";
	cpss_com_send(&stPid, CPS_BOOT_CPU_REBOOT_IND_MSG, (UINT8*)&stRbt, sizeof(stRbt));
}
#endif
void cps_boot_reboot(CPS_REBOOT_T* stRbt)
{
	BOOT_MSG_T stBootMsg = {0};
	FILE* fd;
	time_t timenow;
	INT8 acBuf[256];
	struct tm stTm;

	if(NULL == stRbt)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR, "receive NULL msg in cps_boot_reboot!\n");
		return;
	}

	if(NULL == stRbt->szExtra)
	{
		stRbt->szExtra = "unknown";
	}

	time(&timenow);
	localtime_r(&timenow, &stTm);

	fd = fopen("/usr/local/wlan/ACLog/cps_reboot_log.txt", "a");
	if(NULL == fd)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO, "open cpss_reboot_log.txt error ! \n");
	}
	else
	{

//		fprintf(fd, "reboottime: %04u-%02u%02u-%02u:%02u:%02u, RebootType: %u, RebootReason: %u, Extra: %s",
//				p->tm_year +1900, p->tm_mon +1, p->tm_mday, p->tm_hour +8, p->tm_min, p->tm_sec,
//				stRbt->ucRebtType, stRbt->ucRebtRsn, stRbt->szExtra);
		fprintf(fd, "reboottime: %04u-%02u%02u-%02u:%02u:%02u, RebootType: %u, RebootReason: %u, ",
						stTm.tm_year +1900, stTm.tm_mon +1, stTm.tm_mday, stTm.tm_hour, stTm.tm_min, stTm.tm_sec,
						stRbt->ucRebtType, stRbt->ucRebtRsn);


		memset(acBuf, 0, sizeof(acBuf));

		switch(stRbt->ucRebtRsn)
		{
		case CPS_REBOOT_BY_OM:
			strncpy(acBuf, "OM ind", (sizeof(acBuf) - 1));
			break;
		case CPS_REBOOT_BY_KW:
			strncpy(acBuf, stRbt->szExtra, (sizeof(acBuf) - 1));
			break;
		case CPS_REBOOT_BY_ENTER_END:
			strncpy(acBuf, stRbt->szExtra, (sizeof(acBuf) - 1));
			break;
		case CPS_REBOOT_BY_VER_DOWNLOAD:
			strncpy(acBuf, "Ver Download", (sizeof(acBuf) - 1));
			break;
		case CPS_REBOOT_BY_SEGFAULT:
			strncpy(acBuf, "Seg Fault", (sizeof(acBuf) - 1));
			break;
		case CPS_REBOOT_BY_CPM:
			strncpy(acBuf, "CPM ind", (sizeof(acBuf) - 1));
			break;
		default:
			strncpy(acBuf, "unkown", (sizeof(acBuf) - 1));
			break;
		}

		fprintf(fd, "Extra: %s", acBuf);
		fprintf(fd, "\n");
		fclose(fd);
	}

	switch(stRbt->ucRebtType)
	{
	case CPS_CPU_REBOOT_COLD:
		{
			int i, iFd;
			char *pString = "1";
			char *pFileName[2] = {"/mnt/primary/software/wlan-ac/reboot.log", "/mnt/secondary/software/wlan-ac/reboot.log"};

			for(i = 0; i < sizeof(pFileName) / sizeof(pFileName[0]); i++)
			{
				iFd = open(pFileName[i], O_CREAT | O_WRONLY);
				if(iFd > 0)
				{
					write(iFd, pString, strlen(pString) + 1);
					close(iFd);
				}
			}
		}

		cps_system("reboot");
		break;
	case CPS_CPU_REBOOT_HOT:
		exit(1);
		break;
	}

#if 0
	stBootMsg.ulMsgId = CPS_BOOT_CPU_REBOOT_IND_MSG;
	memcpy(stBootMsg.ucMsg, &stRbt->ucRebtType, sizeof(UINT8));
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO, "send boot cpu reboot msg ! \n");
	cps_com_boot_snd_msg((char*)&stBootMsg);
#endif
	return;
}
#if 0 //dhwang added
void cps_om_phy_para(OM_AC_PHY_PM_STATISTIC_REQ_T* pucbuf)
{
    CPSS_COM_PID_T stPid;
    OM_AC_PHY_PM_STATISTIC_RSP_T stPhyStatRsp;

    stPhyStatRsp.stRspHead.ulSeqId = pucbuf->stReqHead.ulSeqId;
    stPhyStatRsp.stRspHead.ulPmGroupId = pucbuf->stReqHead.ulPmGroupId;
    stPhyStatRsp.stRspHead.ulRev = pucbuf->stReqHead.ulRev;
    stPhyStatRsp.stRspHead.ulResult = cpss_htonl(OM_OK);
    memset(&stPhyStatRsp.stAcPhyPara.aucCpuType, 0, sizeof(stPhyStatRsp.stAcPhyPara.aucCpuType));
//    strcpy(&stPhyStatRsp.stAcPhyPara.aucCpuType, "OCTEON Plus CN5860 Network Services Processor @1500MIPS");
    strncpy(&stPhyStatRsp.stAcPhyPara.aucCpuType, pstCpsDevmSysInfo->aucCpuType, 128);

    memset(&stPhyStatRsp.stAcPhyPara.aucAcMemoryType, 0, sizeof(stPhyStatRsp.stAcPhyPara.aucAcMemoryType));
//    strcpy(stPhyStatRsp.stAcPhyPara.aucAcMemoryType, "DDR2 SDRAM");
    strncpy(&stPhyStatRsp.stAcPhyPara.aucAcMemoryType, pstCpsDevmSysInfo->aucAcMemoryType, 16);

    stPhyStatRsp.stAcPhyPara.ulAcCPURTUsage = cpss_htonl(cps_devm_cpu_usage_now());
    stPhyStatRsp.stAcPhyPara.ulAcDiskUsageRate = cpss_htonl(10);
    stPhyStatRsp.stAcPhyPara.ulAcRamUsageRate = cpss_htonl(cpss_mem_usage_take());
//    stPhyStatRsp.stAcPhyPara.ulRamVol = cpss_htonl(g_ulMemTotalSize);
    stPhyStatRsp.stAcPhyPara.ulRamVol = cpss_htonl(pstCpsDevmSysInfo->ulRamVol);
    stPhyStatRsp.stAcPhyPara.ulAcBootUpTime = cpss_htonl(g_ulPowerOnTime);
    stPhyStatRsp.stAcPhyPara.ulAcCPUAvgUsage = cpss_htonl(g_stCpsDevmCpuUsage.ulAvgUse);
    stPhyStatRsp.stAcPhyPara.ulMemAvgUsage = cpss_htonl(g_stCpsDevmMemUsage.ulAvgUse);
    stPhyStatRsp.stAcPhyPara.ulAcFlashSize = cpss_htonl(256 * 1024ul);
    stPhyStatRsp.stAcPhyPara.ulAcCurrentTemperature = cpss_htonl(g_stCpsDevmTempUsage.ulNowUse);
    cpss_com_logic_addr_get(&stPid.stLogicAddr, &stPid.ulAddrFlag);
    stPid.ulPd = OAMS_PD_M_PM_MNGR_PROC;

    cpss_com_send(&stPid, OM_PM_STATISTIC_RSP_MSG, (UINT8*)&stPhyStatRsp, sizeof(OM_AC_PHY_PM_STATISTIC_RSP_T));
}
#endif
#if 0
UINT32 cpss_brd_usage_get()
{
	struct statfs stSysInfo;
	statfs("/usr/local/wlan/", &stSysInfo);

	return 100 - stSysInfo.f_bfree * 100 / stSysInfo.f_blocks;
}
#endif
/******************************源程序结束******************************************/

