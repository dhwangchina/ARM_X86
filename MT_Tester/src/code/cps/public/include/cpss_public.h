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
#ifndef CPSS_PUBLIC_H
#define CPSS_PUBLIC_H

/********************************* 头文件包含 ********************************/
#ifdef  __cplusplus
extern "C" {
#endif

/* includes */
#include "swp_public.h"
#include "cpss_type.h"
#include "cpss_vos.h"
#include "cpss_com.h"
#include "cpss_tm.h"
#include "cpss_dbg.h"
#include "cpss_fs.h"
#include "cpss_kw.h"
#include "cpss_mm.h"
#include "cpss_vk.h"
#include "smss_public.h"
#include "cps_acct.h"

/******************************** 宏和常量定义 *******************************/
/* 架,框,槽,CPU号的最大值(最小值为1) */
#define CPSS_COM_MAX_FRAME           (5)   /* 最大机架数 */
#define CPSS_COM_MAX_SHELF           (4)   /* 每机架最大机框数 */
#define CPSS_COM_MAX_SLOT            (20)  /* 每机框最大槽位数 */
#define CPSS_COM_MAX_CPU             (20)  /* 每板最大CPU个数 */

/* 定时器消息 */
#define CPSS_TIMER_00_MSG            (0x21000000)
#define CPSS_TIMER_01_MSG            (0x21000001)
#define CPSS_TIMER_02_MSG            (0x21000002)
#define CPSS_TIMER_03_MSG            (0x21000003)
#define CPSS_TIMER_04_MSG            (0x21000004)
#define CPSS_TIMER_05_MSG            (0x21000005)
#define CPSS_TIMER_06_MSG            (0x21000006)
#define CPSS_TIMER_07_MSG            (0x21000007)
#define CPSS_TIMER_08_MSG            (0x21000008)
#define CPSS_TIMER_09_MSG            (0x21000009)
#define CPSS_TIMER_10_MSG            (0x2100000A)
#define CPSS_TIMER_11_MSG            (0x2100000B)
#define CPSS_TIMER_12_MSG            (0x2100000C)
#define CPSS_TIMER_13_MSG            (0x2100000D)
#define CPSS_TIMER_14_MSG            (0x2100000E)
#define CPSS_TIMER_15_MSG            (0x2100000F)
#define CPSS_TIMER_16_MSG            (0x21000010)
#define CPSS_TIMER_17_MSG            (0x21000011)
#define CPSS_TIMER_18_MSG            (0x21000012)
#define CPSS_TIMER_19_MSG            (0x21000013)
#define CPSS_TIMER_20_MSG            (0x21000014)
#define CPSS_TIMER_21_MSG            (0x21000015)
#define CPSS_TIMER_22_MSG            (0x21000016)
#define CPSS_TIMER_23_MSG            (0x21000017)
#define CPSS_TIMER_24_MSG            (0x21000018)
#define CPSS_TIMER_25_MSG            (0x21000019)
#define CPSS_TIMER_26_MSG            (0x2100001A)
#define CPSS_TIMER_27_MSG            (0x2100001B)
#define CPSS_TIMER_28_MSG            (0x2100001C)
#define CPSS_TIMER_29_MSG            (0x2100001D)
#define CPSS_TIMER_30_MSG            (0x2100001E)
#define CPSS_TIMER_31_MSG            (0x2100001F)

/* 通信消息 */
#define CPSS_COM_POWER_ON_MSG        ((UINT32)(0x21000101))
#define CPSS_COM_IRQ_IND_MSG         ((UINT32)(0x21000102))
#define CPSS_COM_TRACE_MSG           ((UINT32)(0x40060706))
#define CPSS_COM_TCP_CONN_IND_MSG    ((UINT32)(0x21000104))
#define CPSS_COM_TCP_DISCONN_IND_MSG ((UINT32)(0x21000105))

/* 调试消息 */
#define CPSS_PRINT_MSG               ((UINT32)(0x40060506))
#define CPSS_LOG_MSG                 ((UINT32)(0x21000202))
#define CPSS_SBBR_REQ_MSG            ((UINT32)(0x21000203))
#define CPSS_SBBR_RSP_MSG            ((UINT32)(0x21000204))

/* 文件传输消息 */
#define CPSS_FTP_REQ_MSG             ((UINT32)(0x21000301))
#define CPSS_FTP_RSP_MSG             ((UINT32)(0x21000302))
#define CPSS_TFTP_REQ_MSG            ((UINT32)(0x21000303))
#define CPSS_TFTP_RSP_MSG_MSG        ((UINT32)(0x21000304))

#define PROC_MSG_ID_ACTIVE           ((UINT32)(0x73000101))
/* 普通任务名称规划 */
#define CPSS_NULL_TASKNAME       "tNULL"
#define CPSS_TIMER_TASKNAME      "tCpssTimer"
#define CPSS_UDPRECV_TASKNAME    "tCpssUdp"
#define CPSS_TCPLISTEN_TASKNAME  "tCpssTcpListen"
#define CPSS_DC_LISTEN_TASKNAME  "tCpssDcListen"
#define CPSS_TCPRECV_TASKNAME    "tCpssTcpRecv"
#define CPSS_HAMONITOR_TASKNAME  "tCpssHaMonitor"
#define CPSS_SLIDTIMER_TASKNAME  "tCpssSlidTimer"
#define CPSS_COM_RECV1_TASKNAME  "tCpssComRecv1"
#define CPSS_COM_RECV2_TASKNAME  "tCpssComRecv2"
#define CPSS_COM_IRQ_TASKNAME    "tCpssIrqRecv"
#define CPSS_COM_PCI_TASKNAME    "tCpssPciRecv"
#define CPSS_IDLE_TASKNAME       "tCpssIdle"

/* 驱动的任务名称规划 */
#define DRV_SHMCSEND_TASKNAME    "tDrvShmcSend"
#define DRV_SHMCRECV_TASKNAME    "tDrvShmcRecieve"
#define DRV_IPHAMSG_TASKNAME     "tDrvIphaMsgProc"
#define DRV_IPHATIMER_TASKNAME   "tDrvIphaTimerProc"
#define DRV_RSTP_TASKNAME        "tDrvRstpTask"
#define DRV_APS_TASKNAME         "tDrvApsTask"
#define DRV_PLLMAIN_TASKNAME     "tDrvPllMainTask"
#define DRV_TEMALARMSCAN_TASKNAME   "tDrvTemAlarmScanTask"

/* 调度任务名 */
#define SWP_SCHED_M_TASKNAME   "sSchedMaster"
#define SWP_SCHED_D_TASKNAME   "sSchedDbg"
#define SWP_SCHED_T_TASKNAME   "sSchedTest"
#define SWP_SCHED_S_TASKNAME   "sSchedSlid"
#define SWP_SCHED_FH_TASKNAME  "sSchedFtpH"
#define SWP_SCHED_FL_TASKNAME  "sSchedFtpL"
#define SWP_SCHED_N_TASKNAME   "sSchedSntp"
#define SWP_SCHED_FIO_TASKNAME "sSchedFileIO"
#define SWP_SCHED_IMA_TASKNAME "sSchedIma"

/* 任务优先级宏定义 */
#define SWP_TASKPRI_TIME_CRITICAL 0
#define SWP_TASKPRI_HIGHEST       1
#define SWP_TASKPRI_ABOVE_NORMAL  2
#define SWP_TASKPRI_NORMAL        3
#define SWP_TASKPRI_BELOW_NORMAL  4
#define SWP_TASKPRI_LOWEST        5
#define SWP_TASKPRI_IDLE          6
#define SWP_TASKPRI_SYSTEM_HIGH   7
#define SWP_TASKPRI_SYSTEM_LOW    8

/* 调度任务优先级 */
#define SWP_SCHED_M_TASKPRI   SWP_TASKPRI_NORMAL
#define SWP_SCHED_D_TASKPRI   SWP_TASKPRI_BELOW_NORMAL
#define SWP_SCHED_T_TASKPRI   SWP_TASKPRI_LOWEST
#define SWP_SCHED_S_TASKPRI   SWP_TASKPRI_ABOVE_NORMAL
#define SWP_SCHED_FH_TASKPRI  SWP_TASKPRI_BELOW_NORMAL
#define SWP_SCHED_FL_TASKPRI  SWP_TASKPRI_LOWEST
#define SWP_SCHED_N_TASKPRI   SWP_TASKPRI_HIGHEST
#define SWP_SCHED_FIO_TASKPRI SWP_TASKPRI_BELOW_NORMAL
#define SWP_SCHED_IMA_TASKPRI SWP_TASKPRI_NORMAL

/* 普通任务优先级 */
#define CPSS_NULL_TASKPRI       0xff
#define CPSS_TIMER_TASKPRI      SWP_TASKPRI_TIME_CRITICAL
#define CPSS_HAMONITOR_TASKPRI  SWP_TASKPRI_TIME_CRITICAL
#define CPSS_UDPRECV_TASKPRI    SWP_TASKPRI_ABOVE_NORMAL
#define CPSS_TCPLISTEN_TASKPRI  SWP_TASKPRI_ABOVE_NORMAL
#define CPSS_TCPRECV_TASKPRI    SWP_TASKPRI_ABOVE_NORMAL
#define CPSS_PCIRECV_TASKPRI    SWP_TASKPRI_ABOVE_NORMAL
#define CPSS_IRQ_TASKPRI        SWP_TASKPRI_HIGHEST
#define CPSS_SLIDTIMER_TASKPRI  SWP_TASKPRI_NORMAL
#define CPSS_COM_RECV_TASKPRI   SWP_TASKPRI_ABOVE_NORMAL
#define CPSS_CPU_LOAD_TASKPRI   SWP_TASKPRI_IDLE
#define DRV_SHMCSEND_TASKPRIO      SWP_TASKPRI_SYSTEM_HIGH 
#define DRV_SHMCRECV_TASKPRIO      SWP_TASKPRI_SYSTEM_LOW 
#define DRV_IPHAMSG_TASKPRIO       SWP_TASKPRI_NORMAL
#define DRV_IPHATIMER_TASKPRIO     SWP_TASKPRI_NORMAL
#define DRV_RSTP_TASKPRIO          SWP_TASKPRI_NORMAL
#define DRV_APS_TASKPRIO           SWP_TASKPRI_NORMAL
#define DRV_PLLMAIN_TASKPRIO       SWP_TASKPRI_NORMAL
#define DRV_TEMALARMSCAN_TASKPRIO  SWP_TASKPRI_NORMAL
#define CPSS_COM_DIAG_COMM_TEST_MAXLEN  2*1024*1024
#define CPSS_COM_DIAG_COMM_TEST_MAXNUM 10
#define CPS_VERM_AC_UPDATE_REQ_MSG 0x75026001 /*AC版本升级请求*/
#define CPS_VERM_AC_UPDATE_RSP_MSG 0x75026002 /*AC版本升级响应*/
#define CPS_VERM_VER_REQ_MSG 0x75027001 /*版本查询请求*/
#define CPS_VERM_VER_RSP_MSG 0x75027002 /*版本查询响应*/


/*AC版本升级请求结构*/
typedef struct _CPS_VERM_AC_UPDATE_REQ_
{
	UINT8 aucAcVersionPath[256]; /*AC版本的绝对路径*/
}CPS_VERM_AC_UPDATE_REQ_T;

/*AC版本升级响应结构*/
typedef struct _CPS_VERM_AC_UPDATE_RSP_
{
	UINT32 ulRet; /*CPSS_OK成功CPSS_ERROR失败*/
}CPS_VERM_AC_UPDATE_RSP_T;

/*版本查询请求结构*/
/*现在未用*/
typedef struct _CPS_VERM_VER_REQ_
{
	UINT32 ulRsv;
}CPS_VERM_VER_REQ_T;

/*版本查询响应结构*/
typedef struct _CPS_VERM_VER_RSP_
{
	UINT8 aucSoftwareName[64];     /*系统软件名称*/
	UINT8 aucSoftwareVersion[64];     /*设备里安装软件的版本号*/
	UINT8 aucSoftwareVendor[64];     /*系统软件的开发厂商*/
}CPS_VERM_VER_RSP_T;
/* add by hudingxuan end */
/******************************** 全局变量声明 *******************************/
/*extern */
/* 打印时将子系统号、模块号转换为字符串的配置结构 */
typedef struct
{
    UINT8 ucSubSys;
    UINT8 ucSubSysName[SWP_SUBSYSNAME_MAX_LEN];
    UINT8 ucModule[SWP_MODULE_MAX_NUM][SWP_MODULE_NAME_MAX_LEN];
}SWP_MODULE_CONFIG_T;

typedef struct tagCPSS_MEM_CONFIG
{
  UINT32 ulBlkSize;                /*大小*/
  UINT32 ulBlkNum;                 /*数量*/

}CPSS_MEM_CONFIG_T;

/* 调度任务和纤程配置结构 */

typedef struct vk_task_config_t
    {
    INT8    acName [32]; /* name */
    INT32   lPri; /* priority */
    INT32   lOptions; /* option */
    INT32   lStackSize; /* stack size */
    VOID_FUNC_PTR   pfEntry; /* entry point of task */
    INT32   lArg; /* argument */
    BOOL    bInuse; /* in use */
    UINT32 ulSbbrSize;
    INT32   lKwTmOutLimitType;
    } VK_TASK_CONFIG_T;

/* scheduler config */

typedef struct vk_sched_config_t
    {
    UINT32  ulSchedDesc; /* scheduler descriptor */
    INT8    acName [32]; /* name */
    INT32   lPri; /* priority */
    INT32   lOptions; /* option */
    INT32   lStackSize; /* stack size */
    VOID_FUNC_PTR   pfEntry; /* entry point of task */
    BOOL    bInuse; /* in use */
    UINT32 ulSbbrSize;
    } VK_SCHED_CONFIG_T;

/* procedure class config */

typedef struct CPSS_VK_PROC_CONFIG
    {
    UINT32  ulPID; /* 纤程描述符 */
    INT8     acName [32]; /* name */
    INT32   lPri; /* priority */
    INT32   lOptions; /* option */
    INT32   lStackSize; /* stack size */
    INT32   lVarSize; /* variable size */
    VOID_FUNC_PTR   pfEntry; /* entry point of procedure */
    UINT32  ulSchedDesc; /* scheduler descriptor */
    UINT16  usTotalInst; /* total number of instance */
    BOOL    bInuse; /* in use */
    INT32   lKwTmOutLimitType;	
    } CPSS_VK_PROC_CONFIG_T;

/* procedure instance config */

typedef struct vk_proc_inst_config_t
    {
    UINT16  usGuid; /* globally unique identifier */
    UINT16  usInst; /* instance */
    BOOL    bInuse; /* in use */
    } VK_PROC_INST_CONFIG_T;
    
typedef struct
{
    UINT32 ulMacCall;
    UINT32 ulTimerScan;
    UINT32 ulPciRecv;
    UINT32 ulChipPoll;
    UINT32 ulProcRun;
}CPSS_DSP_SBBR_COUNTER_T;

#define CPS_IPV4_ADDR_LEN 4

typedef struct {
   UINT32 ulPrefix;
   UINT8 aucIpAddr[CPS_IPV4_ADDR_LEN];
}CPS_IPV4_ADDR_T;

typedef struct {
   UINT32 ulPort;
   UINT32 ulBandWidth;
   UINT32 ulMetric;
   UINT32 ulMtu;
   CPS_IPV4_ADDR_T stIpv4Addr;
}CPS_IP_IPPORT_T;   /* 配置IP地址  */

typedef struct
{
   UINT32 ulDestIpPrefix;                           /* 目的IP地址的掩码*/
   UINT8 aucDestIp[CPS_IPV4_ADDR_LEN];              /* 目的IP地址*/
   UINT8 aucNextHopIp[CPS_IPV4_ADDR_LEN];           /* 下一跳IP地址*/
}CPS_IP_SV4ROUTE_T;  /* 配置路由 */

//extern INT32 cpss_ipport_cfg(INT32 iCmd, CPS_IP_IPPORT_T *pstIpPort);
//extern INT32 cpss_route_ipv4_cfg(INT32 cmd, CPS_IP_SV4ROUTE_T *rt);
extern INT32 cpss_ipport_cfg(INT32 iCmd, CPS_IP_IPPORT_T *pstIpPort);
extern INT32 cpss_route_ipv4_cfg(INT32 cmd, CPS_IP_SV4ROUTE_T *rt);

/* task config */
extern VK_TASK_CONFIG_T    gatVkTaskConfig [];
extern INT32   glVkTaskConfigNumEnt;

/* scheduler config */
extern VK_SCHED_CONFIG_T    gatVkSchedConfig [];
extern INT32   glVkSchedConfigNumEnt;

/* procedure class config */
extern CPSS_VK_PROC_CONFIG_T    gatVkProcClassConfig [];
extern INT32   glVkProcClassConfigNumEnt;

/* procedure instance config */
extern VK_PROC_INST_CONFIG_T gatVkProcInstConfig [];
extern INT32   glVkProcInstConfigNumEnt;
 

/******************************** 外部函数原形声明 ***************************/
/* CPSS初始化入口函数 */
extern INT32 cpss_init (VOID);
extern INT32 cpss_file_init();
extern void cpss_proc_activate();

/* 中断消息分发任务入口 */
extern void cpss_com_irq_dispatch_task() ;

/* 通信PCI接收任务入口 */
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
extern INT32 cpss_com_drv_pci_recv_task() ;
#else
extern void cpss_com_drv_pci_recv_task() ;
#endif

/* 通信UDP接收任务入口 */
extern VOID cpss_com_drv_udp_task(VOID);
/* tcp服务器任务入口 */
extern VOID cpss_com_drv_tcp_listen_task(VOID);
/* tcp接收任务入口 */
extern VOID cpss_com_drv_tcp_recv_task(VOID);

/*可靠传输定时器任务入口 */
extern VOID cpss_com_slid_timer_task(VOID) ;
/* HA监守任务入口 */
extern void cpss_kw_task(void);
/* CPSS提供的注册任务入口，目前只提供给TNBS */
extern void cpss_reg_task_entry(UINT32 ulP1);
/* CPU占用率检测任务 */
extern VOID cpss_cpu_load_task(VOID);
extern INT32 cpss_pci_info_write(UINT32 vuiCpuID);
extern void cpss_callbak_reg(UINT8 subSys, VOID *pfFun);
extern int cpss_get_rfn_count();

/* dsp的函数声明 */
extern UINT32 cpss_dsp6x_mod(UINT32 ulA,UINT32 ulB);
extern UINT32 cpss_dsp6x_div(UINT32 ulA,UINT32 ulB);
extern void cpss_dsp6x_memcpy(UINT8 *pucSrcBuf,UINT8 *pucDesBuf,UINT32 ulLen);
extern void cpss_dsp6x_bitcpy(UINT8 *pucSrcBuf,UINT8 ucSrc,UINT8 *pucDesBuf,UINT8 ucDes,UINT32 ulLen);
extern void cpss_dsp6x_master_write(UINT16  uslen,   UINT32 uldspAddr,  UINT32 ulpciAddr);
extern void cpss_dsp6x_master_read(UINT16   uslen,   UINT32 uldspAddr,  UINT32 ulpciAddr);
extern void cpss_dsp6x_cache_setup();
extern void cpss_dsp6x_reset_config();
extern void cpss_dsp6x_start();
extern INT32 cpss_dsp_sbbr_counter_read(UINT32 ulDspId,CPSS_DSP_SBBR_COUNTER_T *pstDspSbbrCounter);

/******************************** 头文件保护结尾 *****************************/
#ifdef  __cplusplus
}
#endif /* End "#ifdef  __cplusplus" */

/***********************************************************
strcpy((UINT8*)(CPSS_DSP_FILE_ADDR), __FILE__);
*(UINT32*)(CPSS_DSP_LINE_ADDR) = cpss_htonl(__LINE__);
*(UINT32*)(CPSS_DSP_RUN_ADDR) |= CPSS_DSP_POWER_ON_END ;
*(UINT32*)(CPSS_DSP_MSGID_ADDR) = cpss_htonl( ___ );
*(UINT32*)(CPSS_DSP_PRCID_ADDR) = cpss_htonl( ___ );
***********************************************************/
#define CPSS_TEST_PCI_ADDR_BASE 0x83f00010
#define CPSS_TEST_PCI_MEM_SIZE  1024
#define CPSS_DSP_FILE_ADDR (CPSS_TEST_PCI_ADDR_BASE)
#define CPSS_DSP_LINE_ADDR (CPSS_TEST_PCI_ADDR_BASE + 100 + 4)
#define CPSS_DSP_RUN_ADDR (CPSS_TEST_PCI_ADDR_BASE + 100 + 8)
#define CPSS_DSP_MSGID_ADDR (CPSS_TEST_PCI_ADDR_BASE + 100 + 12)
#define CPSS_DSP_PRCID_ADDR (CPSS_TEST_PCI_ADDR_BASE + 100 + 16)
#define CPSS_DSP_GLOB_ADDR (CPSS_TEST_PCI_ADDR_BASE + 100 + 100)
#define CPSS_DSP_CPU_LOAD_ADDR  (CPSS_TEST_PCI_ADDR_BASE + 100 + 100 + 256)

#define CPSS_DSP_SBBR_COUNTER   (CPSS_DSP_CPU_LOAD_ADDR + 20)

#define CPSS_DSP_CPSS_INIT_END 0x00000001
#define CPSS_DSP_CPSS_LINK_PROC_ENTRY 0x00000002
#define CPSS_DSP_COM_INIT_END 0x00000004
#define CPSS_DSP_CPSS_REG_REQ_END 0x00000008
#define CPSS_DSP_CPSS_REG_RES_END 0x00000010
#define CPSS_DSP_POWER_ON_END 0x00000020
#define CPSS_DSP_SMSS_REG_REQ_END 0x00000040
#define CPSS_DSP_SMSS_RES_REQ_END 0x00000080
#define CPSS_DSP_SMSS_BEAT_START 0x00000100

#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
#define CPSS_DSP_FILE_REC() strcpy((char*)(CPSS_DSP_FILE_ADDR), __FILE__);
#define CPSS_DSP_ERROR_WAIT while(1);
#define CPSS_DSP_LINE_REC() *(UINT32*)(CPSS_DSP_LINE_ADDR) = cpss_htonl(__LINE__);
#define CPSS_DSP_RUN_ADDR_REC(x) *(UINT32*)(CPSS_DSP_RUN_ADDR) |= (x);
#define CPSS_DSP_MSGID_REC(x) *(UINT32*)(CPSS_DSP_MSGID_ADDR) = cpss_htonl(x);
#define CPSS_DSP_PRCID_REC(x) *(UINT32*)(CPSS_DSP_PRCID_ADDR) = cpss_htonl(x);
#define CPSS_MOD(x,y) cpss_dsp6x_mod(x,y)
#define CPSS_DIV(x,y) cpss_dsp6x_div(x,y)
#else
#define CPSS_DSP_FILE_REC()
#define CPSS_DSP_ERROR_WAIT 
#define CPSS_DSP_LINE_REC()
#define CPSS_DSP_RUN_ADDR_REC(x)
#define CPSS_DSP_MSGID_REC(x)
#define CPSS_DSP_PRCID_REC(x)
#define CPSS_MOD(x,y) ((x)%(y))
#define CPSS_DIV(x,y) ((x)/(y))
#endif

extern UINT32 g_ulCpssDspSbbrMsgRecSwitch;
#define CPSS_DSP_REVERSE_ADDR_LEN      (20*1024)
#define CPSS_DSP_SBBR_CPSS_VK_ADDR     (CPSS_TEST_PCI_ADDR_BASE + CPSS_DSP_SBBR_SUBSYS_LEN)
#define CPSS_DSP_SBBR_START_ADDR       (CPSS_TEST_PCI_ADDR_BASE + CPSS_DSP_REVERSE_ADDR_LEN)
#define CPSS_DSP_SBBR_SUBSYS_LEN       (10*1024)
#define CPSS_DSP_SBBR_CPSS_START_ADDR  (CPSS_DSP_SBBR_START_ADDR)
#define CPSS_DSP_SBBR_CPS__TNBS_START_ADDR  (CPSS_DSP_SBBR_START_ADDR + CPSS_DSP_SBBR_SUBSYS_LEN)
#define CPSS_DSP_SBBR_TPSS_START_ADDR  (CPSS_DSP_SBBR_START_ADDR + 2*CPSS_DSP_SBBR_SUBSYS_LEN)
#define CPSS_DSP_SBBR_CPS__OAMS_START_ADDR  (CPSS_DSP_SBBR_START_ADDR + 3*CPSS_DSP_SBBR_SUBSYS_LEN)
#define CPSS_DSP_SBBR_SMSS_START_ADDR  (CPSS_DSP_SBBR_START_ADDR + 4*CPSS_DSP_SBBR_SUBSYS_LEN)
#define CPSS_DSP_SBBR_HSPS_START_ADDR  (CPSS_DSP_SBBR_START_ADDR + 5*CPSS_DSP_SBBR_SUBSYS_LEN)
#define CPSS_DSP_SBBR_LEN              (CPSS_DSP_REVERSE_ADDR_LEN + 6*CPSS_DSP_SBBR_SUBSYS_LEN)
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
#define CPSS_DSP_SBBR_COUNTER_REC(COUNTER) (COUNTER = COUNTER + 1);
#else
#define CPSS_DSP_SBBR_COUNTER_REC(COUNTER)
#endif
 

#endif /* CPSS_PUBLIC_H */
/******************************** 头文件结束 *********************************/

