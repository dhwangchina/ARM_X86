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
#ifndef SMSS_DEVM_H
#define SMSS_DEVM_H

/********************************* 头文件包含 ********************************/
#include "cpss_public.h"
#include "cps_replant.h"
/******************************** 宏和常量定义 *******************************/
/* 告警查询定时器 */
#define SMSS_DEVM_TIMER_POLL                     CPSS_TIMER_00
#define SMSS_DEVM_TIMER_POLL_MSG                 CPSS_TIMER_00_MSG
#define SMSS_DEVM_TIMER_POLL_DELAY               500                /* 500ms */
 /* 根据驱动事件手册变更，删除所有驱动相关事件 */
/* 驱动事件查询定时器 */
#define SMSS_DEVM_TIMER_EVENT                    CPSS_TIMER_02
#define SMSS_DEVM_TIMER_EVENT_MSG                CPSS_TIMER_02_MSG
#define SMSS_DEVM_TIMER_EVENT_DELAY              60000                /* 1分钟 */
#define SMSS_DEVM_TIMER_WAIT_IND                     CPSS_TIMER_01
#define SMSS_DEVM_TIMER_WAIT_IND_MSG                 CPSS_TIMER_01_MSG
#define SMSS_DEVM_TIMER_WAIT_IND_DELAY               8000                /* 8s */
/* 主用MASA收到驱动上报的上行端口改变消息, 启定时器, 等待备用通告 */
#define SMSS_DEVM_TIMER_SBY_NOTIFY                     CPSS_TIMER_04
#define SMSS_DEVM_TIMER_SBY_NOTIFY_MSG                 CPSS_TIMER_04_MSG
#define SMSS_DEVM_TIMER_SBY_NOTIFY_DELAY               300                /* 300Ms */

/* 30s超时后再发请求 */
#define SMSS_DEVM_TIMER_CPLD_WAIT_UPDATE                  CPSS_TIMER_05
#define SMSS_DEVM_TIMER_CPLD_WAIT_UPDATE_MSG              CPSS_TIMER_05_MSG
#define SMSS_DEVM_TIMER_CPLD_WAIT_UPDATE_DELAY            30 * 1000                /* 30s */

/* 1s超时后挂任务 */
#define SMSS_DEVM_TIMER_CPLD_WAIT_SUSPEND                  CPSS_TIMER_06
#define SMSS_DEVM_TIMER_CPLD_WAIT_SUSPEND_MSG              CPSS_TIMER_06_MSG
#define SMSS_DEVM_TIMER_CPLD_WAIT_SUSPEND_DELAY            1000                /* 1s */

/* 告警连续产生次数 */
#define SMSS_DEVM_ALARM_OCCUR_COUNT   (4)
#if 0
#define DEVM_DRV_STUB
#define DEVM_DEBUG

#ifndef DRV_ALARM_OCCUR
#define DRV_ALARM_OCCUR     1
#endif
#endif

/* 驱动告警号宏定义:根据系统告警信息表定义 */
#define SMSS_DEVM_ALMNO_NSM           0x00010a00  /* NSM模块告警 */
#ifdef  DRV_ETHER_ALM
#define SMSS_DEVM_ALMNO_ETHER_IPV4    0x00011e00  /* 根据驱动告警手册删除此告警 */
#endif
#define SMSS_DEVM_ALMNO_L2M           0x00011a00   /* 以太交换管理模块告警 */
#define SMSS_DEVM_ALMNO_CLOCK         0x00010200
#define SMSS_DEVM_ALMNO_SHELF_POWER   0x00015c10
#define SMSS_DEVE_ALMNO_SHELF_FAN     0x00015c20  /* 机框风扇盒告警 */
#define SMSS_DEVE_ALMNO_SHM_BOARD     0x00011202  /* 机框管理器单板告警 */
#define SMSS_DEVE_ALMNO_CPUTEMP     0x00017400  /* ATCA-717/7107单板CPU温度主动告警号*/
#define SMSS_DEVE_ALMNO_MEDIA_PORT     0x00011a10  /* 媒体面以态端口告警编号*/

#ifdef  DRV_EVENT
#define SMSS_DEVM_EVENT_NSM_BASE      0x00010a00
#define SMSS_DEVM_EVENT_IPHA_BASE     0X00011800

#endif

/* 驱动告警事件号 */
#define SMSS_DEVM_EVENT_UPDATE_MAC_BASE     0X00011e00   /* UPDATE的MAC芯片事件号 */
#define SMSS_DEVM_EVENT_UPLINK_STATUS_IND   0x00012a00   /* 上行业务端口状态变化指示 */


#define SMSS_DEVM_EVENT_APP550_BASE   0x00017800   /* APP550事件基准 */
#define SMSS_DEVM_ALARM_IPHA_BASE     0X00011800
#define SMSS_DEVM_ALMNO_256           256
#define SMSS_DEVM_ALMNO_258           258
#define SMSS_DEVM_ALMNO_259           259

/* 驱动一次返回的告警最大个数 */
#define SMSS_DRV_ALM_MAX_NUM 512
/* 驱动一次返回的事件最大个数 */
#define SMSS_DRV_EVENT_MAX_NUM 512

/* 各个驱动告警的设备数 */
#define SMSS_DRV_NSM_DEV_NUM  (DRV_NSM_CHIP_NUM * DRV_NSM_PORT_NUM * DRV_NSM_LINK_NUM)
#define SMSS_DRV_ETHER_DEV_NUM  (DRV_ETHER_MAC_CHIP_NUM * DRV_ETHER_MAC_PORT_NUM *DRV_ETHER_MAC_LINK_NUM) 
#define SMSS_DRV_L2M_DEV_NUM_5645  (DRV_L2M_BCM5645_CHIP_NUM * DRV_L2M_BCM5645_PORT_NUM)
#define SMSS_DRV_L2M_DEV_NUM_5695  (DRV_L2M_BCM5695_CHIP_NUM * DRV_L2M_BCM5695_PORT_NUM)
#define SMSS_DRV_CLOCK_DEV_NUM  (1)
#define SMSS_DRV_POWER_DEV_NUM  DRV_SHELF_POWER_NUM
#define SMSS_DRV_FAN_DEV_NUM  DRV_SHELF_FAN_NUM

#define SMSS_DRV_SHMC_BOARD_DEV_NUM  DRV_SHMC_BOARD_NUM


#ifdef  DRV_EVENT
#define SMSS_DRV_IPHA_DEV_NUM 1
#endif


/* 机框温度周期查询次数宏定义 */
#define SMSS_SHMTEMPER_POLL_TIME         1800

/* 机框上电启动阶段机框温度告警周期查询定时器 */
#define SMSS_SHMTEMPER_TIMER_POLL                     CPSS_TIMER_00
#define SMSS_SHMTEMPER_TIMER_POLL_MSG                 CPSS_TIMER_00_MSG
#define SMSS_SHMTEMPER_TIMER_POLL_DELAY               100  
/* 机框上电启动阶段定时器 */
#define SMSS_SHMTEMPER_START_TIMER                     CPSS_TIMER_01
#define SMSS_SHMTEMPER_START_TIMER_MSG                 CPSS_TIMER_01_MSG
#define SMSS_SHMTEMPER_START_TIMER_DELAY               480000  /* 即8分钟 */

/* 日常运行阶段机框温度告警周期查询定时器 */
#define SMSS_DEVM_SHMTEMP_TIMER_POLL                     CPSS_TIMER_03
#define SMSS_DEVM_SHMTEMP_TIMER_POLL_MSG                 CPSS_TIMER_03_MSG
#define SMSS_DEVM_SHMTEMP_TIMER_POLL_DELAY               100  

/* 机框温度告警细节号 */
#define SMSS_SHMTEMP_ALM_UPPER_NONRECOVER   1   /* 风扇盒温度上限不可恢复告警 */
#define SMSS_SHMTEMP_ALM_UPPER_CRITICAL   2   /* 风扇盒温度上限严重告警 */
#define SMSS_SHMTEMP_ALM_UPPER_NONCRITICAL   3   /* 风扇盒温度上限非严重告警 */
#define SMSS_SHMTEMP_ALM_LOWER_NONRECOVER   4   /* 风扇盒温度下限不可恢复告警 */
#define SMSS_SHMTEMP_ALM_LOWER_CRITICAL   5   /* 风扇盒温度下限严重告警 */
#define SMSS_SHMTEMP_ALM_LOWER_NONCRITICAL   6   /* 风扇盒温度下限严重告警 */

/* 加电启动阶段完成时向日常运行阶段发送指示消息 */
#define SMSS_SHMTEMPER_FAN_SPEED_IND_MSG  0x75070000
/* 时钟状态指示消息编号宏定义 */
#define SMSS_DEVM_NSCA_SYN_CLKSTATE_IND_MSG  0x75040005
#define SMSS_DEVM_UL_PORT_STATE_IND_MSG  0x75040006

/* 外围板向主用全局板发CPLD FTP下载请求消息 */
#define SMSS_DEVM_CPLD_FTP_REQ_MSG       0x75040007
/* 主用全局板向外围板发CPLD FTP下载响应消息 */
#define SMSS_DEVM_CPLD_FTP_RSP_MSG       0x75040008


#define SMSS_DEVM_MEDIA_PORT_ALM_NUM  10 

/******************************** 类型定义 ***********************************/
typedef struct  
{
    UINT8  ucFanSpeed;   /* 风扇转速，初始默认值为正常转速 */
    UINT32 ulNonAlarmCycTimes;  /* 查询无告警次数 */
    UINT16 usCycTimes;   /* 3分钟定时器计时 */
    UINT8  ucPowerOnTime;    /* 上电控制时长, 默认8分钟 */
    UINT8  ucFanUpperLimit;  /* 风速控制上限, 默认8级 */
    UINT8  ucFanLowLimit;    /* 风速控制下限, 默认2级 */
    UINT16 usTimerIntev;     /* 调整间隔, 默认 60s */
    UINT8  ucFanUpperIntev;  /* 上调粒度, 默认 3 */
    UINT8  ucFanLowIntev;    /* 下调粒度, 默认 1 */
}SMSS_DEVM_FAN_ALARM_T;

/* 记录测试诊断请求消息， 用于串行控制 */
typedef struct
{
   UINT8 ucStatus;  /* 0-空闲，可以执行机框管理诊断测试；1-正在执行机框管理诊断测试 */
   CPS__OAMS_DIAG_START_TEST_REQ_MSG_T  stTestReqMsg;  /* 保存测试诊断请求消息内容 */	
}SMSS_SHELF_DEVM_DIAG_TEST_MSG_T;

/* 记录测试诊断请求消息， 用于串行控制 */
typedef struct
{
	INT32 iClkState; /* 时钟状态 */
}SMSS_DEVM_NSCA_SYN_CLKSTATE_IND_MSG_T;

/* 交换板上端口状态指示消息 */
typedef struct
{
	INT32 iPortState; /* 正常：SMSS_OK,故障:SMSS_ERROR */
}SMSS_DEVM_UL_PORT_STATE_IND_MSG_T;

/******************************** 全局变量声明 *******************************/
/*
 * 前期各驱动模块初始化函数指针，指向特定CPU的初始化函数。在CPSS初始化前被调用。
 * 返回值：SMSS_OK (0) - 成功；其它 - 失败。
 */
extern UINT32 (*g_pfSmssDevmPreInit)(VOID);

/*
 * 后期各驱动模块初始化函数指针，指向特定CPU的初始化函数。在DEVM纤程激活时被调用。
 * 返回值：SMSS_OK (0) - 成功；其它 - 失败。
 */
extern UINT32 (*g_pfSmssDevmPostInit)(VOID);

/*
 * 告警查询函数指针，指向特定CPU的告警查询函数。
 */
extern VOID   (*g_pfSmssDevmPoll)(VOID);
extern VOID   (*g_pfSmssDevmEvent)(VOID);

/*
 * 消息处理函数指针，指向特定CPU的消息处理函数。
 * 返回值：TRUE - 输入消息已处理；FALSE - 未知消息，未处理。
 */
extern VOID   (*g_pfSmssDevmMsgProcess)(UINT32 ulMsgId, UINT8 *pucMsgBuf);

extern VOID smss_devm_testdiagstart_msgdisp(UINT8* pucBuf);
extern INT32 smss_devm_gssa_msgdisp(UINT8* pucBuf);

/* 20070409 begin */
extern VOID smss_devm_diagtest_qury_brdtemprature(CPS__OAMS_DIAG_START_TEST_REQ_MSG_T *pstDiagStartTest);
extern VOID smss_devm_diagtest_qury_brdvoltage(CPS__OAMS_DIAG_START_TEST_REQ_MSG_T *pstDiagStartTest);
extern VOID smss_devm_diagtest_qury_shelfan(CPS__OAMS_DIAG_START_TEST_REQ_MSG_T *pstDiagStartTest);
extern VOID smss_devm_diagtest_qury_shelftemprature(CPS__OAMS_DIAG_START_TEST_REQ_MSG_T *pstDiagStartTest);
extern UINT32 smss_devm_nsca_nsm_query(CPS__OAMS_M_REQ_HDR_T stReqHeader, UINT32 ulTestId);
extern VOID smss_devm_send_active_port();

#if (SWP_PHYBRD_TYPE == SWP_PHYBRD_MASA)
extern VOID smss_devm_improve_fanspeed(UINT32 ulAlmNo,UINT32 ulDevIndex);
extern VOID smss_devm_disposal_if_noalarm(UINT32 ulAlmNo,UINT32 ulDevIndex);
#endif

/* 存储机框风机盘告警信息 */
extern DRV_ALM_ELM_T g_astSmssShmAlm[DRV_SHELF_FAN_ALM_NUM][SMSS_DRV_FAN_DEV_NUM];
SMSS_DEVM_FAN_ALARM_T g_stShmAlarm;   /* 存储告警次数和当前风扇转速，用于无告警时， */
extern DRV_IPHA_UL_PORT_INFO_MSG_T g_stAPortInfo;/* 在主用二级交换板上存储主用二级交换板上行业务端口信息 */
extern DRV_IPHA_UL_PORT_INFO_MSG_T g_stSPortInfo;/* 在主用二级交换板上存储备用二级交换板上行业务端口信息 */
extern DRV_IPHA_UL_PORT_INFO_MSG_T g_stSCPortInfo;/* 在备用二级交换板上存储备用二级交换板控制面端口信息 */
extern INT32 g_iAPortRet;/* 主用板端口状态标记 */
extern INT32 g_iSPortRet;/* 备用板端口状态标记 */
extern UINT8  g_ucDevId;/* MDPA媒体端口告警类型，0：MDPA媒体端口告警，1：MDPA DPS媒体端口告警 */
extern UINT32 g_ultimes[DRV_BOARD_MEDIA_PORT_NUM];/*MDPA媒体面端口告警次数 */
extern UINT32 g_ulDtimes[DRV_DSP_MEDIA_PORT_NUM];/*MDPA DSP媒体面端口告警次数 */

/******************************** 外部函数原形声明 ***************************/
VOID smss_devm_alarm_process(UINT32 ulAlmNo, DRV_ALM_ELM_T *pstAlmOld, const DRV_ALM_ELM_T *pstAlmNew, 
                             UINT32 ulAlmCnt, UINT32 ulDevCnt, UINT32 ulPortType);

VOID smss_devm_event_process(UINT32 ulEventBase, DRV_EVENT_ELM_T *pstEventOld, const DRV_EVENT_ELM_T *pstEventNew, 
                             UINT32 ulEventCnt, UINT32 ulDevCnt);

INT32 smss_devm_diagtest_qury_portstate(CPS__OAMS_M_REQ_HDR_T stReqHeader, UINT32 ulTestId, UINT8 ucPortId);
VOID smss_devm_masa_shmtemp_alm_query(VOID);
VOID smss_devm_alarm_deal(UINT32 ulAlmNo, DRV_ALM_ELM_T *pstAlm, UINT32 *pstCount, 
                             UINT32 ulAlmCnt, UINT32 ulDevCnt, UINT32 ulPortType);
VOID smss_devm_clk_alarm_deal(UINT32 ulAlmNo, DRV_ALM_ELM_T *pstAlm, UINT32 *pstCount, 
                             UINT32 ulAlmCnt, UINT32 ulDevCnt, UINT32 ulPortType);                             

/*最大保留的抽样记录数*/
#define CPS_DEVM_MAX_NUM 900

/*物理端口状态*/
typedef struct _CPS_DEVM_PORT_STATE_
{
	char cName[16];			/*端口名*/
	UINT8 ucPortNum;			/*端口号*/
	UINT8 ucPrevState;		/*上一个状态*/
	UINT8 ucNowState;		/*当前状态*/
}CPS_DEVM_PORT_STATE_T;

/*端口改变消息*/
typedef struct _CPS_DEVM_PORT_STATE_CHANGE_MSG_T_
{
	UINT8 ucPortTableNum;
	CPS_DEVM_PORT_STATE_T stProtstate;
}CPS_DEVM_PORT_STATE_CHANGE_MSG_T;

/*lmp发来的端口改变的udp消息结构*/
typedef struct _CPS_DEVM_UDP_MSG_T_
{
	UINT32 ulVersion;			/*版本*/
	UINT32 ulTimestamp;		/*时间戳*/
	UINT16 usSequence;		/*消息号*/
	UINT8 Reserved[6];		/*保留字段*/
	UINT16 Port[46];			/*端口状态*/
}CPS_DEVM_UDP_MSG_T;

typedef struct _CPS_DEVM_USAGE_T_
{
//	UINT32 aulUsage[CPS_DEVM_MAX_NUM];	/*抽样保存数组*/
	UINT32 ulFlag;								/*当前数据下标*/
	UINT32 ulAlarmFlag;						/*告警表示 0－已发送告警，1－已发送告警*/
	UINT32 ulNowUse;							/*当前利用率1~100*/
	UINT32 ulTotalUse;							/*记录总的利用率*/
	UINT32 ulAvgUse;							/*平均利用率1~100*/
	UINT32 ulHigh;								/*告警上阈值1~100*/
	UINT32 ulLow;								/*告警下阈值1~100*/
	UINT32 ulTime;								/*统计时长，单位秒*/
	UINT32 ulSaTime;							/*抽样时长，单位秒*/
}CPS_DEVM_USAGE_T;

#define SMSS_DEVM_SAMPLE_TIMER				CPSS_TIMER_00			/*抽样定时器*/
#define SMSS_DEVM_SAMPLE_TIMER_MSG		CPSS_TIMER_00_MSG		/*抽样定时器消息*/

typedef struct _CPS_DEVM_SYSINFO_T_
{
	UINT8  aucCpuType[128];		/* CPU型号,固定值 */
	UINT8  aucAcMemoryType[16];	/* 内存卡类型, 包含SDRAM等 */
	UINT32 ulAcFlashSize;		/* 闪存容量，单位：KB */
	UINT32 ulRamVol;			/* 物理内存容量单位：KB */
}CPS_DEVM_SYSINFO_T;


/******************************** 头文件保护结尾 *****************************/
#endif /* SMSS_DEVM_H */
/******************************** 头文件结束 *********************************/

