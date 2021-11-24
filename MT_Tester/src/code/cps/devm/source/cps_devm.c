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
//#include "om_public.h"
#include "cpss_com_link.h"
#include "smss_devm.h"

#include <stdio.h>
#include <asm/types.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/statfs.h>
#include <string.h>

/*devm纤程状态*/
#define CPS_DEVM_STATE_IDLE CPSS_VK_PROC_USER_STATE_IDLE
#define CPS_DEVM_STATE_NORMAL 1

/*端口状态上报消息*/
#define CPS_DEVM_PORT_STATE_CHANGE_MSG 0x75071000
/*udp端口状态上报消息*/
#define CPS_DEVM_UDP_MSG 0x75071001

//#define CPS_DEVM_LINK_UP 1
//#define CPS_DEVM_LINK_DOWN 0

#define CPS_LINK_NULL ""

//typedef struct _CPS_DEVM_PORT_STATE_
//{
//	char cName[16];
//	BOOL bState;
//}CPS_DEVM_PORT_STATE_T;

#if 0
typedef struct _CPS_DEVM_PORT_STATE_
{
	char cName[16];
	UINT8 ucPortNum;
	UINT8 ucPrevState;
	UINT8 ucNowState;
}CPS_DEVM_PORT_STATE_T;

typedef struct _CPS_DEVM_PORT_STATE_CHANGE_MSG_T_
{
	UINT8 ucPortTableNum;
	CPS_DEVM_PORT_STATE_T stProtstate;
}CPS_DEVM_PORT_STATE_CHANGE_MSG_T;

typedef struct _CPS_DEVM_UDP_MSG_T_
{
	UINT32 ulVersion;
	UINT32 ulTimestamp;
	UINT16 usSequence;
	UINT8 Reserved[6];
	UINT16 Port[46];
}CPS_DEVM_UDP_MSG_T;
#endif

CPS_DEVM_PORT_STATE_T gCpsDevmPortStateAll[] =
{
	/*name  perstate nowstate*/
	/*ccu front 0~4*/
	{"ETH1", 0, FALSE, FALSE},
	{"ETH2", 0, FALSE, FALSE},
	{"ETH3", 0, FALSE, FALSE},
	{"ETH4", 0, FALSE, FALSE},
	{"ETH5", 0, FALSE, FALSE},
	/*dpb1 5~7*/
	{"spi0", 41, FALSE, FALSE},
	{"spi13", 42, FALSE, FALSE},
	{"spi20", 4, FALSE, FALSE},
	/*dpb2 8~10*/
	{"spi0", 43, FALSE, FALSE},
	{"spi13", 44, FALSE, FALSE},
	{"spi20", 5, FALSE, FALSE},
	/*ppu front sfp 11~16*/
	{"SFP1", 6, FALSE, FALSE},
	{"SFP2", 7, FALSE, FALSE},
	{"SFP3", 8, FALSE, FALSE},
	{"SFP4", 9, FALSE, FALSE},
	{"SFP5", 10, FALSE, FALSE},
	{"SFP6", 11, FALSE, FALSE},
	/*ppu front sfp+ 17~20*/
	{"SFP+1", 31, FALSE, FALSE},
	{"SFP+2", 32, FALSE, FALSE},
	{"SFP+3", 33, FALSE, FALSE},
	{"SFP+4", 34, FALSE, FALSE},
	{CPS_LINK_NULL, 0},
};

#if SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA
	#define CPS_DVEM_PORT_CHECK_BEGIN 0
	#define CPS_DVEM_PORT_CHECK_END 4
#elif SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA
	#define CPS_DVEM_PORT_CHECK_BEGIN 5
	#define CPS_DVEM_PORT_CHECK_END 6
#endif

#if 0
CPS_DEVM_PORT_STATE_T gCpsDevmPortState[] =
#if SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA
{
	{"ETH2", FALSE},
	{"ETH3", FALSE},
	{"ETH4", FALSE},
	{"ETH5", FALSE},
	{CPS_LINK_NULL, 0},
};
#elif SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA
{
	{"SPI0", FALSE},
	{"SPI13", FALSE},
	{CPS_LINK_NULL, 0},
}
#endif
#endif

VOID cps_devm_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsg);
VOID cps_devm_state_idle_func(CPSS_COM_MSG_HEAD_T* pstMsg);
VOID cps_devm_state_normal_func(CPSS_COM_MSG_HEAD_T* pstMsg);
VOID cps_devm_state_set_new(UINT16 usNewState);
INT32 cps_devm_proc_activate_response(UINT32 result);
VOID cps_devm_port_check();
VOID cps_devm_port_state_check();
VOID cps_devm_udp_task();
VOID cps_devm_port_change(CPS_DEVM_PORT_STATE_CHANGE_MSG_T* pstBuf);
VOID cps_devm_port_compare(UINT8 ucPort);
VOID cps_devm_udp_msg_deal(CPS_DEVM_UDP_MSG_T* pcbuf);
VOID cps_devm_cpu_chg_msg_deal(CPS_CPU_STAT_CHG_IND_MSG_T* pstCpuStat);

BOOL is_port_linkup(char *pcPortName);

VOID cps_devm_cpu_usage_get();
UINT32 cps_devm_cpu_usage_now();
VOID cps_devm_disk_usage_get();
VOID cps_devm_mem_usage_get();
INT32 cpss_devm_usage_init();

UINT32 cpss_devm_cpu_sample_usage_get();
UINT32 cpss_devm_mem_sample_usage_get();
VOID cps_dev_cpu_usage_get_fast();
VOID cps_dev_mem_usage_get_fast();
extern VOID cps_send_om_alarm(UINT32 ulAlarmNO, UINT32 ulAlarmType, UINT8* pucbuf, UINT32 ulLen);
extern VOID cps_send_om_event(UINT32 ulEventNO, UINT8* pucbuf, UINT32 ulLen);
extern UINT32 cpss_mem_usage_take();
VOID cpss_devm_temp_get_task();

#if 0
#define CPS_DEVM_MAX_NUM 900

typedef struct _CPS_DEVM_USAGE_T_
{
	UINT32 aulUsage[CPS_DEVM_MAX_NUM];
	UINT32 ulFlag;
	UINT32 ulAlarmFlag;
	UINT32 ulNowUse;
	UINT32 ulAvgUse;
	UINT32 ulHigh;
	UINT32 ulLow;
	UINT32 ulTime;
	UINT32 ulSaTime;
}CPS_DEVM_USAGE_T;
#endif

CPS_DEVM_USAGE_T g_stCpsDevmCpuUsage;
CPS_DEVM_USAGE_T g_stCpsDevmDiskUsage;
CPS_DEVM_USAGE_T g_stCpsDevmMemUsage;
CPS_DEVM_USAGE_T g_stCpsDevmTempUsage;

VOID cps_devm_cpu_usage_get_new();
VOID cps_devm_mem_usage_get_new();
VOID cps_devm_cpu_alarm_check();
VOID cps_devm_mem_alarm_check();
#if 0 //dhwang add
VOID cps_devm_om_ac_para_chg(OM_AC_PARAMETER_CHG_IND_MSG_T* pstMsg);
#endif
VOID cps_devm_reboot_type_check();

UINT32 cpss_devm_tmp_take();

UINT8 g_ucPortStateInit = 0;
INT32 cps_devm_get_mem_size();
UINT32 g_ulMemTotalSize;

typedef struct _CPU_INFO_T
{
	char cpuid[64];
	int cpustat[8];
}CPU_INFO_T;

CPU_INFO_T gCpuUsage;

typedef struct _CPS_DEVM_RESTART_EXTRATION_
{
	UINT8 aucMac[PUBLIC_MAC_LENTH];/* MAC*/
	UINT8 aucTime[PUBLIC_NAME_LENTH];/*冷启动时间，字符串表示*/
}CPS_DEVM_RESTART_EXTRATION_T;

typedef struct _CPS_DEVM_TEMPERATURE_EXTRATION_
{
	UINT8 aucMac[PUBLIC_MAC_LENTH];/* MAC*/
	UINT8 ucTemperature;/*温度*/
} CPS_DEVM_TEMPERATURE_EXTRATION_T;

INT32 cps_devm_mac_trans(INT8* pucMacChar, INT8* pucMacNum, INT32 lLen);
INT32 cps_devm_sysinfo_init();
CPS_DEVM_SYSINFO_T* pstCpsDevmSysInfo;

VOID cps_devm_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsg)
{
	 /* 判断消息合法 */
	if (pstMsg == NULL)
	{
//		printf("..........cps devm proc recieve null msg!..........\n");
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cps_devm_proc receive NULL msg in State %d!\n", usUserState);
		return;
	}
	/* 状态分发 */
	switch(usUserState)
	{
	case CPS_DEVM_STATE_IDLE:          /* Idle态处理函数 */
		cps_devm_state_idle_func(pstMsg);
		break;
	case CPS_DEVM_STATE_NORMAL:        /* Normal态处理函数 */
		cps_devm_state_normal_func(pstMsg);
		break;
	default:                            /* 无效状态 */
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cps_devm_proc receive error State = %d!\n", usUserState);
		break;
	}
	return;
}

/* Idle态处理函数 */
VOID cps_devm_state_idle_func(CPSS_COM_MSG_HEAD_T* pstMsg)
{
	INT32 lRet;

	if(NULL == pstMsg)
	{
		return;
	}

	char* pcProcName = "CpsDevmUdpTask";

	switch(pstMsg->ulMsgId)
	{
	case SMSS_PROC_ACTIVATE_REQ_MSG:    /* 收到纤程激活请求消息 */
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "cps_devm_state_idle_func receive SMSS_PROC_ACTIVATE_REQ_MSG \n");
		//lRet = cpss_devm_usage_init();
		if(SMSS_OK == lRet)
		{
//			cps_devm_proc_activate_response(SMSS_ERROR);
		}
		else
		{
			/*初始化失败，使用默认数据*/
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cps_devm_state_idle_func init error! ,use default!\n");

			g_stCpsDevmCpuUsage.ulHigh = 80;
			g_stCpsDevmCpuUsage.ulLow = 60;
			g_stCpsDevmCpuUsage.ulTime = 120;
			g_stCpsDevmCpuUsage.ulSaTime = 2;

			g_stCpsDevmMemUsage.ulHigh = 80;
			g_stCpsDevmMemUsage.ulLow = 60;
			g_stCpsDevmMemUsage.ulTime = 120;
			g_stCpsDevmMemUsage.ulSaTime = 2;

			g_stCpsDevmTempUsage.ulHigh = 80;
			g_stCpsDevmTempUsage.ulLow = 60;
			g_stCpsDevmTempUsage.ulTime = 120;
			g_stCpsDevmTempUsage.ulSaTime = 2;
		}

		g_ulMemTotalSize = cps_devm_get_mem_size();
		cps_devm_proc_activate_response(SMSS_OK);

		break;
	case SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG:
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "cps_devm_state_idle_func receive SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG \n");
		/* 转入normal态 */
		cps_devm_state_set_new(CPS_DEVM_STATE_NORMAL);

		/*确定是冷启动还是热启动，并向om发送事件*/
		//cps_devm_reboot_type_check();//dhwang del

		/*创建采样定时器*/
		cpss_timer_set(SMSS_DEVM_SAMPLE_TIMER, g_stCpsDevmCpuUsage.ulSaTime * 1000);

#if 0
#if SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA
		cpss_timer_loop_set(CPSS_TIMER_01, 1000);
		cpss_timer_loop_set(CPSS_TIMER_02, 1000);
		cpss_timer_loop_set(CPSS_TIMER_03, g_stCpsDevmCpuUsage.ulSaTime * 1000);
#endif
#endif

		/*创建接受lmp端口udp消息的任务*/
//		cpss_vos_task_spawn(pcProcName, 0, 0, 0,(void*)cps_devm_udp_task, 0);

		cpss_vos_task_spawn("CpssDevmTempTask", 0, 0, 0,(void*)cpss_devm_temp_get_task, 0);

		break;
	default:                            /* 收到非法消息 */
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cps_devm_state_idle_func rcv unknown msg! msg id = 0x%x\n",pstMsg->ulMsgId);
		break;
	}
	return;
}

VOID cps_devm_state_normal_func(CPSS_COM_MSG_HEAD_T* pstMsg)
{
	UINT32 ulSamUsage;
	UINT8 ucUsage;
//	UINT32 ulLoop;

	if(NULL == pstMsg)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "cps_devm_state_normal_func rcv NULL msg!\n");
		return;
	}

	switch(pstMsg->ulMsgId)
	{
	/*现阶段不需要查询本地端口*/
#if 0
	/*本地端口查询定时器*/
	case CPSS_TIMER_01_MSG:
		cps_devm_port_check();
//		cps_devm_port_state_check();
		break;
#endif

	/*现阶段无ppu，该消息不存在*/
#if 0
	/*ppu上端口状态改变消息*/
	case CPS_DEVM_PORT_STATE_CHANGE_MSG:
		cps_devm_port_change((CPS_DEVM_PORT_STATE_CHANGE_MSG_T*)pstMsg->pucBuf);
		break;
#endif

	/*由lmp发送的端口状态改变消息*/
	case CPS_DEVM_UDP_MSG:
//		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "rcv udp port change msg.");
		cps_devm_udp_msg_deal((CPS_DEVM_UDP_MSG_T*)pstMsg->pucBuf);
		break;
	/*cpu状态改变消息*/
#if 0
	case CPS_CPU_STAT_CHG_IND_MSG:
		cps_devm_cpu_chg_msg_deal((CPS_CPU_STAT_CHG_IND_MSG_T*)pstMsg->pucBuf);
		break;
#endif

#if 0
	case CPSS_TIMER_02_MSG:
		cps_devm_cpu_usage_get();
//		cps_devm_disk_usage_get();
		cps_devm_mem_usage_get();
#if 0 //for test
		printf("Cpu Flag   = %d\n", g_stCpsDevmCpuUsage.ulFlag);
		printf("Cpu AvgUse = %d\n", g_stCpsDevmCpuUsage.ulAvgUse);
		for(ulLoop = 0; ulLoop < CPS_DEVM_MAX_NUM; ulLoop++)
		{
			printf("%02u ", g_stCpsDevmCpuUsage.aulUsage[ulLoop]);
		}
		printf("\n");
		printf("Disk Flag   = %d\n", g_stCpsDevmDiskUsage.ulFlag);
		printf("Disk AvgUse = %d\n", g_stCpsDevmDiskUsage.ulAvgUse);
		for(ulLoop = 0; ulLoop < CPS_DEVM_MAX_NUM; ulLoop++)
		{
			printf("%02u ", g_stCpsDevmDiskUsage.aulUsage[ulLoop]);
		}
		printf("\n");
		printf("Disk Flag   = %d\n", g_stCpsDevmMemUsage.ulFlag);
		printf("Disk AvgUse = %d\n", g_stCpsDevmMemUsage.ulAvgUse);
		for(ulLoop = 0; ulLoop < CPS_DEVM_MAX_NUM; ulLoop++)
		{
			printf("%02u ", g_stCpsDevmMemUsage.aulUsage[ulLoop]);
		}
		printf("\n");
#endif
		break;
	case CPSS_TIMER_03_MSG:

		/*cpu利用率判断*/
		ulSamUsage = cpss_devm_cpu_sample_usage_get();

		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "cpu_sample_usage = %d\n", ulSamUsage);

		/*判断是否产生告警*/
		/*当前利用率是否高于上阈值并且未发送告警*/
		if((g_stCpsDevmCpuUsage.ulHigh < ulSamUsage)&&(0 == g_stCpsDevmCpuUsage.ulAlarmFlag))
		{
			/*设置为已发送告警*/
			g_stCpsDevmCpuUsage.ulAlarmFlag = 1;
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "send om alarm occer! CpuUsage = %d\n", ulSamUsage);
			ucUsage = (UINT8)ulSamUsage;
			cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_CPUUSAGE_HIGH, OAMS_ALARM_OCCUR, (UINT8*)&ucUsage, sizeof(UINT8));
		}

		/*判断是否清除告警*/
		/*当前利用率是否低于下阈值并且已发送告警*/
		if((g_stCpsDevmCpuUsage.ulLow > ulSamUsage)&&(1 == g_stCpsDevmCpuUsage.ulAlarmFlag))
		{
			/*设置为未发送告警*/
			g_stCpsDevmCpuUsage.ulAlarmFlag = 0;
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "send om alarm clean! CpuUsage = %d\n", ulSamUsage);
			ucUsage = (UINT8)ulSamUsage;
			cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_CPUUSAGE_HIGH, OAMS_ALARM_CLEAR, (UINT8*)&ucUsage, sizeof(UINT8));
		}

		/*内存利用率判断*/

		ulSamUsage = cpss_devm_mem_sample_usage_get();

		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "mem_sample_usage = %d\n", ulSamUsage);

		/*判断是否产生告警*/
		/*当前利用率是否高于上阈值并且未发送告警*/
		if((g_stCpsDevmMemUsage.ulHigh < ulSamUsage)&&(0 == g_stCpsDevmMemUsage.ulAlarmFlag))
		{
			/*设置为已发送告警*/
			g_stCpsDevmMemUsage.ulAlarmFlag = 1;
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "send om alarm occer! MemUsage = %d\n", ulSamUsage);
			ucUsage = (UINT8)ulSamUsage;
			cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_CPUUSAGE_HIGH, OAMS_ALARM_OCCUR, (UINT8*)&ucUsage, sizeof(UINT8));
		}

		/*判断是否清楚告警*/
		/*当前利用率是否低于下阈值并且已发送告警*/
		if((g_stCpsDevmMemUsage.ulLow > ulSamUsage)&&(1 == g_stCpsDevmMemUsage.ulAlarmFlag))
		{
			/*设置为未发送告警*/
			g_stCpsDevmMemUsage.ulAlarmFlag = 0;
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "send om alarm clean! MemUsage = %d\n", ulSamUsage);
			ucUsage = (UINT8)ulSamUsage;
			cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_CPUUSAGE_HIGH, OAMS_ALARM_CLEAR, (UINT8*)&ucUsage, sizeof(UINT8));
		}
		break;
#endif

		/*收到采样定时器消息*/
	case SMSS_DEVM_SAMPLE_TIMER_MSG:
//		cps_devm_cpu_usage_get_new();
		/*获取cpu利用率信息*/
		//cps_dev_cpu_usage_get_fast(); //dhwang del
		/*获取内存利用率信息*/
		//cps_dev_mem_usage_get_fast();//dhwang del
		/*获取温度信息*/
		//cps_dev_temp_get_fast();//dhwang del
		/*cpu告警检查*/
		//cps_devm_cpu_alarm_check();//dhwang del
		/*内存告警检查*/
		//cps_devm_mem_alarm_check();//dhwang del
		/*温度告警检查*/
		//cps_devm_temp_alarm_check();//dhwang del
//		printf("cpu now useage = %d\n", g_stCpsDevmCpuUsage.ulNowUse);
//		printf("cpu avg useage = %d\n", g_stCpsDevmCpuUsage.ulAvgUse);
		/*重新发送采样定时器*/
		cpss_timer_set(SMSS_DEVM_SAMPLE_TIMER, g_stCpsDevmCpuUsage.ulSaTime * 1000);
		break;
		/*收到OM告警参数修改指示*/
#if 0 //dhwang added
	case OM_AC_PARAMETER_CHG_IND_MSG:
//		cps_timer_delete(SMSS_DEVM_SAMPLE_TIMER);

		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "cps_devm_state_normal_func rcv OM_AC_PARAMETER_CHG_IND_MSG msg!\n");
		/*参数修改处理*/
		cps_devm_om_ac_para_chg(pstMsg->pucBuf);

		/*重置采样定时器*/
		cpss_timer_set(SMSS_DEVM_SAMPLE_TIMER, g_stCpsDevmCpuUsage.ulSaTime * 1000);
		break;
#endif
	default:
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "cps_devm_state_normal_func rcv Error msg!,MsgId = %x\n", pstMsg->ulMsgId);
		break;
	}
	return;
}

VOID cps_devm_state_set_new(UINT16 usNewState)
{
	cpss_vk_proc_user_state_set(usNewState);
	return;
}

/*verm纤程激活响应*/
INT32 cps_devm_proc_activate_response(UINT32 result)
{
	CPSS_COM_PHY_ADDR_T stPhyAddr;
	SMSS_PROC_ACTIVATE_RSP_MSG_T stRspMsg;

	stRspMsg.ulResult = result;
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "devm proc send activate rsp msg!\n");
	cpss_com_phy_addr_get(&stPhyAddr);
	return cpss_com_send_phy(stPhyAddr, SMSS_SYSCTL_PROC, SMSS_PROC_ACTIVATE_RSP_MSG, (UINT8 *)(&stRspMsg),sizeof(SMSS_PROC_ACTIVATE_RSP_MSG_T));
}

VOID cps_devm_port_check()
{
	UINT8 ucPortId;
	BOOL bPortState;
#if SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA
	CPS_DEVM_PORT_STATE_CHANGE_MSG_T stPortState;
	CPSS_COM_PID_T stDstPid;
#endif

	for(ucPortId = CPS_DVEM_PORT_CHECK_BEGIN; ucPortId <= CPS_DVEM_PORT_CHECK_END; ucPortId++ )
	{
		bPortState = is_port_linkup(gCpsDevmPortStateAll[ucPortId].cName);
//		printf("CPS->DEVM PortNum = %d, PortState =%d\n", ucPortId, bPortState);
		gCpsDevmPortStateAll[ucPortId].ucNowState = bPortState;

#if SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA

#if 0
		cps_devm_port_compare(ucPortId);

		if((FALSE == gCpsDevmPortStateAll[ucPortId].ucNowState)&&(TRUE == gCpsDevmPortStateAll[ucPortId].ucPrevState))
		{
			/*send om alarm occer*/
			printf("send om alarm occer!\n");
		}

		if((TRUE == gCpsDevmPortStateAll[ucPortId].ucNowState)&&(FALSE == gCpsDevmPortStateAll[ucPortId].ucPrevState))
		{
			/*send om alarm clear*/
			printf("send om alarm clear!\n");
		}
#endif

#elif SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA
		/*ppu do it*/
		if(gCpsDevmPortStateAll[ucPortId].ucNowState != gCpsDevmPortStateAll[ucPortId].ucPrevState)
		{
			/*send ccu devm state change msg*/
			stPortState.ucPortTableNum = ucPortId;
			memcpy(&stPortState.stProtstate, &gCpsDevmPortStateAll[ucPortId], sizeof(CPS_DEVM_PORT_STATE_T));

			stDstPid.stLogicAddr = g_stLogicAddrGcpa;
			stDstPid.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER;
			stDstPid.ulPd = SMSS_DEVM_PROC;

			cpss_com_send(&stDstPid, CPS_DEVM_PORT_STATE_CHANGE_MSG, (UINT8*)&stPortState, sizeof(CPS_DEVM_PORT_STATE_CHANGE_MSG_T));
		}
#endif

		gCpsDevmPortStateAll[ucPortId].ucPrevState = gCpsDevmPortStateAll[ucPortId].ucNowState;
	}

#if 0
	while(CPS_LINK_NULL != gCpsDevmPortState[ulPortId].cName)
	{
		bPortState = is_port_linkup(gCpsDevmPortState[ulPortId].cName);
		if(bPortState != gCpsDevmPortState[ulPortId].bState)
		{
			continue;
		}
		if((FALSE == bPortState)&&(TRUE == gCpsDevmPortState[ulPortId].bState))
		{
			/*send om alarm occer*/
		}

		if((TRUE == bPortState)&&(FALSE == gCpsDevmPortState[ulPortId].bState))
		{
			/*send om alarm clear*/
		}
		ulPortId++;
	}
#endif
}

#if 0
VOID cps_devm_port_state_check()
{
	UINT8 ucPortId;

	ucPortId = 0;
	while(CPS_LINK_NULL != gCpsDevmPortStateAll[ucPortId].cName)
	{
		if((FALSE == gCpsDevmPortStateAll[ucPortId].ucNowState)&&(TRUE == gCpsDevmPortStateAll[ucPortId].ucPrevState))
		{
			/*send om alarm occer*/
		}

		if((TRUE == gCpsDevmPortStateAll[ucPortId].ucNowState)&&(FALSE == gCpsDevmPortStateAll[ucPortId].ucPrevState))
		{
			/*send om alarm clear*/
		}
		ucPortId++;
	}
}
#endif

VOID cps_devm_port_change(CPS_DEVM_PORT_STATE_CHANGE_MSG_T* pstBuf)
{
	gCpsDevmPortStateAll[pstBuf->ucPortTableNum].ucNowState = pstBuf->stProtstate.ucNowState;
//	gCpsDevmPortStateAll[pstBuf->ucPortNum].ucPrevState = pstBuf->ucPrevState;

	//cps_devm_port_compare(pstBuf->ucPortTableNum);

#if 0
	if((FALSE == gCpsDevmPortStateAll[pstBuf->ucPortTableNum].ucNowState)&&(TRUE == gCpsDevmPortStateAll[pstBuf->ucPortTableNum].ucPrevState))
	{
		/*send om alarm occer*/
	}

	if((TRUE == gCpsDevmPortStateAll[pstBuf->ucPortTableNum].ucNowState)&&(FALSE == gCpsDevmPortStateAll[pstBuf->ucPortTableNum].ucPrevState))
	{
		/*send om alarm clear*/
	}
#endif

	gCpsDevmPortStateAll[pstBuf->ucPortTableNum].ucPrevState = gCpsDevmPortStateAll[pstBuf->ucPortTableNum].ucNowState;
	return;
}

BOOL is_port_linkup(char *pcPortName)
{
	int iFd;
	struct ifreq stReq;
	struct {
		unsigned int uiCmd;
		int iData;
	}stEth;

	memset(&stReq, 0, sizeof(struct ifreq));

	iFd = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == iFd)
	{
		return FALSE;
	}
	strncpy(stReq.ifr_name, pcPortName, IF_NAMESIZE);
	stReq.ifr_data = (char*)&stEth;
	stEth.uiCmd = ETHTOOL_GLINK;

	if(-1 == ioctl(iFd, SIOCETHTOOL, &stReq))
	{
		close(iFd);
		return FALSE;
	}
	if(stEth.iData != IFF_UP)
	{
		close(iFd);
		return FALSE;
	}
	close(iFd);
	return TRUE;
}

VOID cps_devm_udp_task()
{
	INT lSock;
	struct sockaddr_in stUdpAddr;

#if 0
	struct sockaddr_in stSrcAddr;
	UINT32 ulSrcAddrLen;
#endif

	UINT32 ulRcvLen;
	CPSS_COM_PID_T stDstPid;
	INT32 lRet;
	char cRcvBuf[512];

	lSock = socket(AF_INET, SOCK_DGRAM, 0);
	if(-1 == lSock)
	{
		perror("socket");
	}
	stUdpAddr.sin_family = AF_INET;

//	stUdpAddr.sin_addr.s_addr = inet_addr("11.33.9.1");
	stUdpAddr.sin_addr.s_addr= g_pstComData->stAddrThis.ulIpAddr;

	stUdpAddr.sin_port = cpss_htons(6000);

	lRet = bind(lSock, (struct sockaddr*)&stUdpAddr, sizeof(struct sockaddr_in));
	if(-1 == lRet)
	{
		perror("cps_devm_udp_task bind");
	}

	stDstPid.stLogicAddr = g_stLogicAddrGcpa;
	stDstPid.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER;
	stDstPid.ulPd = SMSS_DEVM_PROC;

	while(1)
	{
		ulRcvLen = recvfrom(lSock, &cRcvBuf, sizeof(cRcvBuf), 0, NULL, NULL);
		if(ulRcvLen <= 0)
		{
			perror("recvfrom");
		}
		cpss_com_send(&stDstPid, CPS_DEVM_UDP_MSG, (UINT8*)&cRcvBuf, ulRcvLen);
	}
}

VOID cps_devm_udp_msg_deal(CPS_DEVM_UDP_MSG_T* pcbuf)
{
	UINT8 ucLoop;
	/*sfp 1~6*/
	for(ucLoop = 6; ucLoop <= 11; ucLoop++)
	{
		if(0 == pcbuf->Port[ucLoop])
		{
			gCpsDevmPortStateAll[ucLoop + 5].ucNowState = FALSE;
		}
		else
		{
			gCpsDevmPortStateAll[ucLoop + 5].ucNowState = TRUE;
		}

		//cps_devm_port_compare(ucLoop + 5);

		gCpsDevmPortStateAll[ucLoop + 5].ucPrevState = gCpsDevmPortStateAll[ucLoop + 5].ucNowState;
	}
	/*sfp+ 1~4*/
	for(ucLoop = 31; ucLoop <= 34; ucLoop++)
	{
		if(0 == pcbuf->Port[ucLoop])
		{
			gCpsDevmPortStateAll[ucLoop - 14].ucNowState = FALSE;
		}
		else
		{
			gCpsDevmPortStateAll[ucLoop - 14].ucNowState = TRUE;
		}

		//cps_devm_port_compare(ucLoop - 14);

		gCpsDevmPortStateAll[ucLoop - 14].ucPrevState = gCpsDevmPortStateAll[ucLoop - 14].ucNowState;
	}
	/*dpb1 spi0 spi13*/
	for(ucLoop = 41; ucLoop <= 42; ucLoop++)
	{
		if(0 == pcbuf->Port[ucLoop])
		{
			gCpsDevmPortStateAll[ucLoop - 36].ucNowState = FALSE;
		}
		else
		{
			gCpsDevmPortStateAll[ucLoop - 36].ucNowState = TRUE;
		}

		//cps_devm_port_compare(ucLoop - 36);

		gCpsDevmPortStateAll[ucLoop - 36].ucPrevState = gCpsDevmPortStateAll[ucLoop - 36].ucNowState;
	}
	/*dpb2 spi0 spi13*/
	for(ucLoop = 43; ucLoop <= 44; ucLoop++)
	{
		if(0 == pcbuf->Port[ucLoop])
		{
			gCpsDevmPortStateAll[ucLoop - 35].ucNowState = FALSE;
		}
		else
		{
			gCpsDevmPortStateAll[ucLoop - 35].ucNowState = TRUE;
		}

		//cps_devm_port_compare(ucLoop - 35);

		gCpsDevmPortStateAll[ucLoop - 35].ucPrevState = gCpsDevmPortStateAll[ucLoop - 35].ucNowState;
	}

	if(0 == g_ucPortStateInit)
	{
		/*向mscc上报端口状态*/
		//cps_send_ccm_port_chg_msg();

		g_ucPortStateInit = 1;
	}
}
#if 0 //dhwang added
VOID cps_devm_port_compare(UINT8 ucPort)
{
	CPS_EXT_IF_STAT_CHG_IND_MSG_T stPortChg;
	CPSS_COM_PID_T stDstPid;

	if(0 == g_ucPortStateInit)
	{
		return;
	}

	if((FALSE == gCpsDevmPortStateAll[ucPort].ucNowState)&&(TRUE == gCpsDevmPortStateAll[ucPort].ucPrevState))
	{
		if((11 <= ucPort) && (20 >= ucPort))
		{
			stPortChg.ulIf = ucPort - 10;
			stPortChg.ulState = 1;

			cpss_com_logic_addr_get(&stDstPid.stLogicAddr, &stDstPid.ulAddrFlag);
			stDstPid.ulPd = UPM_MAIN_CONTROL_PROC;
			cpss_com_send(&stDstPid, CPS_EXT_IF_STAT_CHG_IND_MSG, (UINT8*)&stPortChg, sizeof(CPS_EXT_IF_STAT_CHG_IND_MSG_T));
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "devm send um port error! port = %d\n", stPortChg.ulIf);
			cps_send_ccm_port_chg_msg();
		}
		/*send om alarm occer*/
//		printf("send om alarm occer! port = %d\n", ucPort);
	}

	if((TRUE == gCpsDevmPortStateAll[ucPort].ucNowState)&&(FALSE == gCpsDevmPortStateAll[ucPort].ucPrevState))
	{
		if((11 <= ucPort) && (20 >= ucPort))
		{
			stPortChg.ulIf = ucPort - 10;
			stPortChg.ulState = 0;

			cpss_com_logic_addr_get(&stDstPid.stLogicAddr, &stDstPid.ulAddrFlag);
			stDstPid.ulPd = UPM_MAIN_CONTROL_PROC;
			cpss_com_send(&stDstPid, CPS_EXT_IF_STAT_CHG_IND_MSG, (UINT8*)&stPortChg, sizeof(CPS_EXT_IF_STAT_CHG_IND_MSG_T));
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "devm send um port ok! port = %d\n", stPortChg.ulIf);
			cps_send_ccm_port_chg_msg();
		}
		/*send om alarm clear*/
//		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "send om alarm clear! port = %d\n", ucPort);
	}

//	gCpsDevmPortStateAll[ucPort].ucPrevState = gCpsDevmPortStateAll[ucPort].ucNowState;
}
#endif

VOID cps_devm_cpu_chg_msg_deal(CPS_CPU_STAT_CHG_IND_MSG_T* pstCpuStat)
{
	INT32 lLoop;
//	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "rcv cpu chg msg!\n");
	switch(pstCpuStat->stLogicAddr.ucSubGroup)
	{
		case 2:
			if(1 == cpss_ntohl(pstCpuStat->iState))
			{
				for(lLoop = 5; lLoop <= 7; lLoop++)
				{
					gCpsDevmPortStateAll[lLoop].ucNowState = FALSE;
					gCpsDevmPortStateAll[lLoop].ucPrevState = FALSE;
				}
			}
			break;
		case 3:
			if(-1 == cpss_ntohl(pstCpuStat->iState))
			{
				for(lLoop = 5; lLoop <= 7; lLoop++)
				{
					gCpsDevmPortStateAll[lLoop].ucNowState = FALSE;
					gCpsDevmPortStateAll[lLoop].ucPrevState = FALSE;
				}
			}
			break;
		default:
			break;
	}
//	pstCpuStat->stLogicAddr.usGroup;
//	pstCpuStat->stLogicAddr.ucSubGroup;
}

UINT32 get_usage()
{
	srand(time(NULL));
	return (UINT32)(rand() / (RAND_MAX / 100));
}

#if 0
VOID cps_devm_cpu_usage_get()
{
	UINT32 ulNowUsage;
	UINT32 ulLoop;
	UINT32 ulLoopBegin;
	UINT32 ulLoopNum;
	UINT32 ulTotal;
//	UINT8 ucUsage;

	/*获取当前利用率*/
	ulNowUsage = cps_devm_cpu_usage_now();
	/*for test*/
//	ulNowUsage = get_usage();

	/*将当前利用率加入表中*/
	/*如果表未满*/
	if(CPS_DEVM_MAX_NUM > g_stCpsDevmCpuUsage.ulFlag)
	{
		/*插入当前标志位*/
		g_stCpsDevmCpuUsage.aulUsage[g_stCpsDevmCpuUsage.ulFlag] = ulNowUsage;
		/*标志位后移*/
		g_stCpsDevmCpuUsage.ulFlag++;
	}
	/*如果表已满*/
	else
	{
		/*表内数据左移*/
		for(ulLoop = 0; ulLoop < CPS_DEVM_MAX_NUM - 1; ulLoop++)
		{
			g_stCpsDevmCpuUsage.aulUsage[ulLoop] = g_stCpsDevmCpuUsage.aulUsage[ulLoop+1];
		}
		/*插入到表末尾*/
		g_stCpsDevmCpuUsage.aulUsage[CPS_DEVM_MAX_NUM - 1] = ulNowUsage;
	}

	/*求平均利用率*/
	/*统计时长大于已记录的时长时*/
	/*由已记录的值总和／记录的数量*/

	/*统计时长小于已记录的时长时*/
	/*由当前记录的值向前追寻（统计时长／时间间隔）个数据求和，再除以数据数量*/
	ulTotal = 0;

	/*如果设定查询时间大于已统计时间处理，一般发生在设备启动时*/
	ulLoopBegin = (g_stCpsDevmCpuUsage.ulFlag  <= g_stCpsDevmCpuUsage.ulTime) ? 0 : g_stCpsDevmCpuUsage.ulFlag  - g_stCpsDevmCpuUsage.ulTime;
	ulLoopNum = (g_stCpsDevmCpuUsage.ulFlag <= g_stCpsDevmCpuUsage.ulTime) ? g_stCpsDevmCpuUsage.ulFlag : g_stCpsDevmCpuUsage.ulTime;

	/*求所设定时常内利用率之和*/
	for(ulLoop = ulLoopBegin; ulLoop <= g_stCpsDevmCpuUsage.ulFlag - 1; ulLoop++)
	{
		ulTotal +=  g_stCpsDevmCpuUsage.aulUsage[ulLoop];
	}

	/*求平均值*/
	g_stCpsDevmCpuUsage.ulAvgUse = ulTotal / ulLoopNum;

#if 0
	/*更新上个状态和当前状态*/
	g_stCpsDevmCpuUsage.ulPrevUse = g_stCpsDevmCpuUsage.ulNowUse;
	g_stCpsDevmCpuUsage.ulNowUse = ulNowUsage;

	printf("Prev = %02u, Now = %02u\n", g_stCpsDevmCpuUsage.ulPrevUse, g_stCpsDevmCpuUsage.ulNowUse);

	/*判断是否告警*/
	/*上个状态在高阈值之下，当前状态在高阈值之上*/
	if((g_stCpsDevmCpuUsage.ulPrevUse <= g_stCpsDevmCpuUsage.ulHigh)&&(g_stCpsDevmCpuUsage.ulNowUse > g_stCpsDevmCpuUsage.ulHigh))
	{
		/*send om alarm occer!*/
		printf("send om alarm occer! \n");
	}

	/*判断是否清楚告警*/
	/*上个状态在低阈值之上，当前状态在低阈值之下*/
	if((g_stCpsDevmCpuUsage.ulPrevUse >= g_stCpsDevmCpuUsage.ulLow)&&(g_stCpsDevmCpuUsage.ulNowUse < g_stCpsDevmCpuUsage.ulLow))
	{
		/*send om alarm clean!*/
		printf("send om alarm clean! \n");
	}
#endif

	/*更新当前状态*/
	g_stCpsDevmCpuUsage.ulNowUse = ulNowUsage;

#if 0
	/*判断是否产生告警*/
	/*当前利用率是否高于上阈值并且未发送告警*/
	if((g_stCpsDevmDiskUsage.ulHigh < g_stCpsDevmDiskUsage.ulNowUse)&&(0 == g_stCpsDevmDiskUsage.ulAlarmFlag))
	{
		/*设置为已发送告警*/
		g_stCpsDevmDiskUsage.ulAlarmFlag = 1;
		printf("send om alarm occer!\n");
	}

	/*判断是否清楚告警*/
	/*当前利用率是否低于下阈值并且已发送告警*/
	if((g_stCpsDevmDiskUsage.ulLow > g_stCpsDevmDiskUsage.ulNowUse)&&(1 == g_stCpsDevmDiskUsage.ulAlarmFlag))
	{
		/*设置为未发送告警*/
		g_stCpsDevmDiskUsage.ulAlarmFlag = 0;
		printf("send om alarm clean!\n");
	}
#endif
	return;
}

UINT32 cpss_brd_usage_get()
{
//	struct statfs stSysInfo;
//	statfs("/usr/local/wlan/", &stSysInfo);
//
//	return 100 - stSysInfo.f_bfree * 100 / stSysInfo.f_blocks;

	return 10;
}

VOID cps_devm_disk_usage_get()
{
	UINT32 ulNowUsage;
	UINT32 ulLoop;
	UINT32 ulLoopBegin;
	UINT32 ulLoopNum;
	UINT32 ulTotal;

	/*获取当前利用率*/
	ulNowUsage = cpss_brd_usage_get();

	/*将当前利用率加入表中*/
	/*如果表未满*/
	if(CPS_DEVM_MAX_NUM > g_stCpsDevmDiskUsage.ulFlag)
	{
		/*插入当前标志位*/
		g_stCpsDevmDiskUsage.aulUsage[g_stCpsDevmDiskUsage.ulFlag] = ulNowUsage;
		/*标志位后移*/
		g_stCpsDevmDiskUsage.ulFlag++;
	}
	/*如果表已满*/
	else
	{
		/*表内数据左移*/
		for(ulLoop = 0; ulLoop < CPS_DEVM_MAX_NUM - 1; ulLoop++)
		{
			g_stCpsDevmDiskUsage.aulUsage[ulLoop] = g_stCpsDevmDiskUsage.aulUsage[ulLoop+1];
		}
		/*插入到表末尾*/
		g_stCpsDevmDiskUsage.aulUsage[CPS_DEVM_MAX_NUM - 1] = ulNowUsage;
	}

	/*求平均利用率*/
	ulTotal = 0;

	/*如果设定查询时间大于已统计时间处理，一般发生在设备启动时*/
	ulLoopBegin = (g_stCpsDevmDiskUsage.ulFlag  <= g_stCpsDevmDiskUsage.ulTime) ? 0 : g_stCpsDevmDiskUsage.ulFlag  - g_stCpsDevmDiskUsage.ulTime;
	ulLoopNum = (g_stCpsDevmDiskUsage.ulFlag <= g_stCpsDevmDiskUsage.ulTime) ? g_stCpsDevmDiskUsage.ulFlag : g_stCpsDevmDiskUsage.ulTime;

	/*求所设定时常内利用率之和*/
	for(ulLoop = ulLoopBegin; ulLoop <= g_stCpsDevmDiskUsage.ulFlag - 1; ulLoop++)
	{
		ulTotal +=  g_stCpsDevmDiskUsage.aulUsage[ulLoop];
	}

	/*求平均值*/
	g_stCpsDevmDiskUsage.ulAvgUse = ulTotal / ulLoopNum;

	/*更新当前状态*/
	g_stCpsDevmDiskUsage.ulNowUse = ulNowUsage;

#if 0
	/*判断是否产生告警*/
	/*当前利用率是否高于上阈值并且未发送告警*/
	if((g_stCpsDevmDiskUsage.ulHigh < g_stCpsDevmDiskUsage.ulNowUse)&&(0 == g_stCpsDevmDiskUsage.ulAlarmFlag))
	{
		/*设置为已发送告警*/
		g_stCpsDevmDiskUsage.ulAlarmFlag = 1;
		printf("send om alarm occer!\n");
	}

	/*判断是否清楚告警*/
	/*当前利用率是否低于下阈值并且已发送告警*/
	if((g_stCpsDevmDiskUsage.ulLow > g_stCpsDevmDiskUsage.ulNowUse)&&(1 == g_stCpsDevmDiskUsage.ulAlarmFlag))
	{
		/*设置为未发送告警*/
		g_stCpsDevmDiskUsage.ulAlarmFlag = 0;
		printf("send om alarm clean!\n");
	}
#endif
	return;
}
#endif

UINT32 cps_devm_cpu_usage_now()
{
	FILE* fStat;
	fStat = NULL;
	CPU_INFO_T stCpuUsageNow;
	UINT32 ulRet[4];
	UINT32 ulLoop;

	fStat = fopen("/proc/stat", "r");
	if(NULL == fStat)
	{
		perror("fopen");
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cpu stat errno = %d\n", errno);
		return 0;
	}

	fscanf(fStat, "%s", stCpuUsageNow.cpuid);
	for(ulLoop = 0; ulLoop < 8; ulLoop++)
	{
		fscanf(fStat, "%d", &stCpuUsageNow.cpustat[ulLoop]);
	}

	fclose(fStat);

	for(ulLoop = 0; ulLoop < 4; ulLoop++)
	{
		ulRet[ulLoop] = stCpuUsageNow.cpustat[ulLoop] - gCpuUsage.cpustat[ulLoop];
	}

	for(ulLoop = 0; ulLoop < 4; ulLoop++)
	{
		gCpuUsage.cpustat[ulLoop] = stCpuUsageNow.cpustat[ulLoop];
	}

	if(0 == ulRet[0] + ulRet[1] + ulRet[2] + ulRet[3])
	{
		return 10;
	}

	return 100 * (ulRet[0] + ulRet[1] + ulRet[2]) / (ulRet[0] + ulRet[1] + ulRet[2] + ulRet[3]);
}

#if 0
VOID cps_devm_mem_usage_get()
{
	UINT32 ulNowUsage;
	UINT32 ulLoop;
	UINT32 ulLoopBegin;
	UINT32 ulLoopNum;
	UINT32 ulTotal;
//	UINT8 ucUsage;


	/*获取当前利用率*/
	ulNowUsage = cpss_mem_usage_take();

	/*将当前利用率加入表中*/
	/*如果表未满*/
	if(CPS_DEVM_MAX_NUM > g_stCpsDevmMemUsage.ulFlag)
	{
		/*插入当前标志位*/
		g_stCpsDevmMemUsage.aulUsage[g_stCpsDevmMemUsage.ulFlag] = ulNowUsage;
		/*标志位后移*/
		g_stCpsDevmMemUsage.ulFlag++;
	}
	/*如果表已满*/
	else
	{
		/*表内数据左移*/
		for(ulLoop = 0; ulLoop < CPS_DEVM_MAX_NUM - 1; ulLoop++)
		{
			g_stCpsDevmMemUsage.aulUsage[ulLoop] = g_stCpsDevmMemUsage.aulUsage[ulLoop+1];
		}
		/*插入到表末尾*/
		g_stCpsDevmMemUsage.aulUsage[CPS_DEVM_MAX_NUM - 1] = ulNowUsage;
	}

	/*求平均利用率*/
	ulTotal = 0;

	/*如果设定查询时间大于已统计时间处理，一般发生在设备启动时*/
	ulLoopBegin = (g_stCpsDevmMemUsage.ulFlag  <= g_stCpsDevmMemUsage.ulTime) ? 0 : g_stCpsDevmMemUsage.ulFlag  - g_stCpsDevmMemUsage.ulTime;
	ulLoopNum = (g_stCpsDevmMemUsage.ulFlag <= g_stCpsDevmMemUsage.ulTime) ? g_stCpsDevmMemUsage.ulFlag : g_stCpsDevmMemUsage.ulTime;

	/*求所设定时常内利用率之和*/
	for(ulLoop = ulLoopBegin; ulLoop <= g_stCpsDevmMemUsage.ulFlag - 1; ulLoop++)
	{
		ulTotal +=  g_stCpsDevmMemUsage.aulUsage[ulLoop];
	}

	/*求平均值*/
	g_stCpsDevmMemUsage.ulAvgUse = ulTotal / ulLoopNum;

	/*更新当前状态*/
	g_stCpsDevmMemUsage.ulNowUse = ulNowUsage;

#if 0
	/*判断是否产生告警*/
	/*当前利用率是否高于上阈值并且未发送告警*/
	if((g_stCpsDevmMemUsage.ulHigh < g_stCpsDevmMemUsage.ulNowUse)&&(0 == g_stCpsDevmMemUsage.ulAlarmFlag))
	{
		/*设置为已发送告警*/
		g_stCpsDevmMemUsage.ulAlarmFlag = 1;
		printf("send om alarm occer! MemUsage = %d\n", ulNowUsage);
		ucUsage = (UINT8)ulNowUsage;
		cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_MEMUSAGE_HIGH, OAMS_ALARM_OCCUR, (UINT8*)&ucUsage, sizeof(UINT8));
	}

	/*判断是否清楚告警*/
	/*当前利用率是否低于下阈值并且已发送告警*/
	if((g_stCpsDevmMemUsage.ulLow > g_stCpsDevmMemUsage.ulNowUse)&&(1 == g_stCpsDevmMemUsage.ulAlarmFlag))
	{
		/*设置为未发送告警*/
		g_stCpsDevmMemUsage.ulAlarmFlag = 0;
		printf("send om alarm clean! MemUsage = %d\n", ulNowUsage);
		ucUsage = (UINT8)ulNowUsage;
		cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_MEMUSAGE_HIGH, OAMS_ALARM_CLEAR, (UINT8*)&ucUsage, sizeof(UINT8));
	}
#endif
	return;
}
#endif

#if 0 //dhwang added
INT32 cpss_devm_usage_init()
{
	OM_GET_AC_ALARMPARAMETER_MSG_RSP_T stAlarmParaRsp;

	memset(&g_stCpsDevmCpuUsage, 0, sizeof(CPS_DEVM_USAGE_T));
	memset(&g_stCpsDevmMemUsage, 0, sizeof(CPS_DEVM_USAGE_T));
	memset(&g_stCpsDevmTempUsage, 0, sizeof(CPS_DEVM_USAGE_T));

	rdbs_ac_if_entry(OM_GET_AC_ALARMPARAMETER_MSG, 0, NULL, 0, (UINT8*)&stAlarmParaRsp, sizeof(OM_GET_AC_ALARMPARAMETER_MSG_RSP_T));
	if(0 != stAlarmParaRsp.ulResult)
	{
		return SMSS_ERROR;
	}

	/*利用率不在范围内0~100*/
	if((0 >= stAlarmParaRsp.stAlarmPara.ulAcCpuUsageLowThreshhd) || (100 < stAlarmParaRsp.stAlarmPara.ulAcCpuUsageLowThreshhd))
	{
		return SMSS_ERROR;
	}

	if((0 >= stAlarmParaRsp.stAlarmPara.ulAcCpuUsageThreshhd) || (100 < stAlarmParaRsp.stAlarmPara.ulAcCpuUsageThreshhd))
	{
		return SMSS_ERROR;
	}

	if((0 >= stAlarmParaRsp.stAlarmPara.ulAcMemUsageLowThreshhd) || (100 < stAlarmParaRsp.stAlarmPara.ulAcMemUsageLowThreshhd))
	{
		return SMSS_ERROR;
	}

	if((0 >= stAlarmParaRsp.stAlarmPara.ulApTemperatureThreshold) || (100 < stAlarmParaRsp.stAlarmPara.ulApTemperatureThreshold))
	{
		return SMSS_ERROR;
	}

	if((0 >= stAlarmParaRsp.stAlarmPara.ulApTemperatureLowThreshold) || (100 < stAlarmParaRsp.stAlarmPara.ulApTemperatureLowThreshold))
	{
		return SMSS_ERROR;
	}

	if((0 >= stAlarmParaRsp.stAlarmPara.ulAcMemUsageThreshhd) || (100 < stAlarmParaRsp.stAlarmPara.ulAcMemUsageThreshhd))
	{
		return SMSS_ERROR;
	}

	/*统计时长小于采样时长*/
	if(stAlarmParaRsp.stAlarmPara.ulAcStatWindowTime < stAlarmParaRsp.stAlarmPara.ulSampleTime)
	{
		return SMSS_ERROR;
	}

	/*下阈值比上阈值高*/
	if(stAlarmParaRsp.stAlarmPara.ulAcCpuUsageLowThreshhd > stAlarmParaRsp.stAlarmPara.ulAcCpuUsageThreshhd)
	{
		return SMSS_ERROR;
	}

	if(stAlarmParaRsp.stAlarmPara.ulAcMemUsageLowThreshhd > stAlarmParaRsp.stAlarmPara.ulAcMemUsageThreshhd)
	{
		return SMSS_ERROR;
	}

	if(stAlarmParaRsp.stAlarmPara.ulApTemperatureLowThreshold > stAlarmParaRsp.stAlarmPara.ulApTemperatureThreshold)
	{
		return SMSS_ERROR;
	}

	/*统计时间不在范围内*/
	if((0 >= stAlarmParaRsp.stAlarmPara.ulAcStatWindowTime) || (900 < stAlarmParaRsp.stAlarmPara.ulAcStatWindowTime))
	{
		return SMSS_ERROR;
	}

	/*采样时间不在范围内*/
	if((0 >= stAlarmParaRsp.stAlarmPara.ulSampleTime) || (900 < stAlarmParaRsp.stAlarmPara.ulAcStatWindowTime))
	{
		return SMSS_ERROR;
	}

	g_stCpsDevmCpuUsage.ulHigh = stAlarmParaRsp.stAlarmPara.ulAcCpuUsageThreshhd;
	g_stCpsDevmCpuUsage.ulLow = stAlarmParaRsp.stAlarmPara.ulAcCpuUsageLowThreshhd;
	g_stCpsDevmCpuUsage.ulTime = stAlarmParaRsp.stAlarmPara.ulAcStatWindowTime;
	g_stCpsDevmCpuUsage.ulSaTime = stAlarmParaRsp.stAlarmPara.ulSampleTime;

	g_stCpsDevmMemUsage.ulHigh = stAlarmParaRsp.stAlarmPara.ulAcMemUsageThreshhd;
	g_stCpsDevmMemUsage.ulLow = stAlarmParaRsp.stAlarmPara.ulAcMemUsageLowThreshhd;
	g_stCpsDevmMemUsage.ulTime = stAlarmParaRsp.stAlarmPara.ulAcStatWindowTime;
	g_stCpsDevmMemUsage.ulSaTime = stAlarmParaRsp.stAlarmPara.ulSampleTime;

	g_stCpsDevmTempUsage.ulHigh = stAlarmParaRsp.stAlarmPara.ulAcTemperatureThreshold;
	g_stCpsDevmTempUsage.ulLow = stAlarmParaRsp.stAlarmPara.ulAcTemperatureThreshold;
	g_stCpsDevmTempUsage.ulTime = stAlarmParaRsp.stAlarmPara.ulAcStatWindowTime;
	g_stCpsDevmTempUsage.ulSaTime = stAlarmParaRsp.stAlarmPara.ulSampleTime;

	return SMSS_OK;
}
#endif
#if 0
UINT32 cpss_devm_mem_sample_usage_get()
{
	UINT32 ulTotal;
	UINT32 ulLoop;
	UINT32 ulLoopBegin;
	UINT32 ulLoopNum;

	/*求平均利用率*/
	ulTotal = 0;

	/*如果设定查询时间大于已统计时间处理，一般发生在设备启动时*/
	ulLoopBegin = (g_stCpsDevmMemUsage.ulFlag  <= g_stCpsDevmMemUsage.ulSaTime) ? 0 : g_stCpsDevmMemUsage.ulFlag  - g_stCpsDevmMemUsage.ulSaTime;
	ulLoopNum = (g_stCpsDevmMemUsage.ulFlag <= g_stCpsDevmMemUsage.ulSaTime) ? g_stCpsDevmMemUsage.ulFlag : g_stCpsDevmMemUsage.ulSaTime;

	/*求所设定时常内利用率之和*/
	for(ulLoop = ulLoopBegin; ulLoop <= g_stCpsDevmMemUsage.ulFlag - 1; ulLoop++)
	{
		ulTotal +=  g_stCpsDevmMemUsage.aulUsage[ulLoop];
	}

	/*求平均值*/
	return ulTotal / ulLoopNum;
}

UINT32 cpss_devm_cpu_sample_usage_get()
{
	UINT32 ulTotal;
	UINT32 ulLoop;
	UINT32 ulLoopBegin;
	UINT32 ulLoopNum;

	/*求平均利用率*/
	ulTotal = 0;

	/*如果设定查询时间大于已统计时间处理，一般发生在设备启动时*/
	ulLoopBegin = (g_stCpsDevmCpuUsage.ulFlag  <= g_stCpsDevmCpuUsage.ulSaTime) ? 0 : g_stCpsDevmCpuUsage.ulFlag  - g_stCpsDevmCpuUsage.ulSaTime;
	ulLoopNum = (g_stCpsDevmCpuUsage.ulFlag <= g_stCpsDevmCpuUsage.ulSaTime) ? g_stCpsDevmCpuUsage.ulFlag : g_stCpsDevmCpuUsage.ulSaTime;

	/*求所设定时常内利用率之和*/
	for(ulLoop = ulLoopBegin; ulLoop <= g_stCpsDevmCpuUsage.ulFlag - 1; ulLoop++)
	{
		ulTotal +=  g_stCpsDevmCpuUsage.aulUsage[ulLoop];
	}

	/*求平均值*/
	return ulTotal / ulLoopNum;
}
#endif

#if 0
VOID cps_devm_cpu_usage_get_new()
{
	UINT32 ulNowUsage;
	UINT32 ulLoop;
	UINT32 ulLoopBegin;
	UINT32 ulLoopNum;
	UINT32 ulTotal;

	int count;

	/*获取当前利用率*/
//	ulNowUsage = cps_devm_cpu_usage_now();

	ulNowUsage = get_usage();

	/*将当前利用率加入表中*/
	/*如果表未满*/
	if(CPS_DEVM_MAX_NUM > g_stCpsDevmCpuUsage.ulFlag)
	{
		/*插入当前标志位*/
		g_stCpsDevmCpuUsage.aulUsage[g_stCpsDevmCpuUsage.ulFlag] = ulNowUsage;
		/*标志位后移*/
		g_stCpsDevmCpuUsage.ulFlag++;
	}
	/*如果表已满*/
	else
	{
		/*表内数据左移*/
		for(ulLoop = 0; ulLoop < CPS_DEVM_MAX_NUM - 1; ulLoop++)
		{
			g_stCpsDevmCpuUsage.aulUsage[ulLoop] = g_stCpsDevmCpuUsage.aulUsage[ulLoop+1];
		}
		/*插入到表末尾*/
		g_stCpsDevmCpuUsage.aulUsage[CPS_DEVM_MAX_NUM - 1] = ulNowUsage;
	}

	/*求平均利用率*/

	ulTotal = 0;

	/*统计时长大于已记录的时长时*/
	/*由已记录的值总和／记录的数量*/
	if(g_stCpsDevmCpuUsage.ulTime >= g_stCpsDevmCpuUsage.ulFlag * g_stCpsDevmCpuUsage.ulSaTime)
	{
		count = g_stCpsDevmCpuUsage.ulFlag;
		for(ulLoop = 0; ulLoop < g_stCpsDevmCpuUsage.ulFlag; ulLoop++)
		{
			ulTotal +=  g_stCpsDevmCpuUsage.aulUsage[ulLoop];
		}

		g_stCpsDevmCpuUsage.ulAvgUse = ulTotal / g_stCpsDevmCpuUsage.ulFlag;
	}

	/*统计时长小于已记录的时长时*/
	/*由当前记录的值向前追寻（统计时长／时间间隔）个数据求和，再除以数据数量*/
	if(g_stCpsDevmCpuUsage.ulTime < g_stCpsDevmCpuUsage.ulFlag * g_stCpsDevmCpuUsage.ulSaTime)
	{
		count = g_stCpsDevmCpuUsage.ulTime / g_stCpsDevmCpuUsage.ulSaTime;
		for(ulLoop = g_stCpsDevmCpuUsage.ulFlag - (g_stCpsDevmCpuUsage.ulTime / g_stCpsDevmCpuUsage.ulSaTime); ulLoop < g_stCpsDevmCpuUsage.ulFlag; ulLoop++)
		{
			ulTotal +=  g_stCpsDevmCpuUsage.aulUsage[ulLoop];
		}

		g_stCpsDevmCpuUsage.ulAvgUse = ulTotal / (g_stCpsDevmCpuUsage.ulTime / g_stCpsDevmCpuUsage.ulSaTime);
	}

	/*更新当前状态*/
	g_stCpsDevmCpuUsage.ulNowUse = ulNowUsage;

//	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "OLD : Now = %d, Total = %d, Count = %d, Avg = %d\n", g_stCpsDevmCpuUsage.ulNowUse, ulTotal, g_stCpsDevmCpuUsage.ulFlag, g_stCpsDevmCpuUsage.ulAvgUse);

	return;
}

VOID cps_devm_mem_usage_get_new()
{
	UINT32 ulNowUsage;
	UINT32 ulLoop;
	UINT32 ulLoopBegin;
	UINT32 ulLoopNum;
	UINT32 ulTotal;

	/*获取当前利用率*/
	ulNowUsage = cpss_mem_usage_take();

//	ulNowUsage = get_usage();

	/*将当前利用率加入表中*/
	/*如果表未满*/
	if(CPS_DEVM_MAX_NUM > g_stCpsDevmMemUsage.ulFlag)
	{
		/*插入当前标志位*/
		g_stCpsDevmMemUsage.aulUsage[g_stCpsDevmMemUsage.ulFlag] = ulNowUsage;
		/*标志位后移*/
		g_stCpsDevmMemUsage.ulFlag++;
	}
	/*如果表已满*/
	else
	{
		/*表内数据左移*/
		for(ulLoop = 0; ulLoop < CPS_DEVM_MAX_NUM - 1; ulLoop++)
		{
			g_stCpsDevmMemUsage.aulUsage[ulLoop] = g_stCpsDevmMemUsage.aulUsage[ulLoop+1];
		}
		/*插入到表末尾*/
		g_stCpsDevmMemUsage.aulUsage[CPS_DEVM_MAX_NUM - 1] = ulNowUsage;
	}

	ulTotal = 0;
	/*求平均利用率*/

	/*统计时长大于已记录的时长时*/
	/*由已记录的值总和／记录的数量*/
	if(g_stCpsDevmMemUsage.ulTime >= g_stCpsDevmMemUsage.ulFlag * g_stCpsDevmMemUsage.ulSaTime)
	{
		for(ulLoop = 0; ulLoop < g_stCpsDevmMemUsage.ulFlag; ulLoop++)
		{
			ulTotal +=  g_stCpsDevmMemUsage.aulUsage[ulLoop];
		}

		g_stCpsDevmMemUsage.ulAvgUse = ulTotal / g_stCpsDevmMemUsage.ulFlag;
	}

	/*统计时长小于已记录的时长时*/
	/*由当前记录的值向前追寻（统计时长／时间间隔）个数据求和，再除以数据数量*/
	if(g_stCpsDevmMemUsage.ulTime < g_stCpsDevmMemUsage.ulFlag * g_stCpsDevmMemUsage.ulSaTime)
	{
		for(ulLoop = g_stCpsDevmMemUsage.ulFlag - (g_stCpsDevmMemUsage.ulTime / g_stCpsDevmMemUsage.ulSaTime); ulLoop < g_stCpsDevmMemUsage.ulFlag; ulLoop++)
		{
			ulTotal +=  g_stCpsDevmMemUsage.aulUsage[ulLoop];
		}

		g_stCpsDevmMemUsage.ulAvgUse = ulTotal / (g_stCpsDevmMemUsage.ulTime / g_stCpsDevmMemUsage.ulSaTime);
	}

	/*更新当前状态*/
	g_stCpsDevmMemUsage.ulNowUse = ulNowUsage;

	return;
}
#endif

UINT32 g_ulUsageTotal;
UINT32 g_ulUsageNum;
#define USAGE_COUNT 30

/*获得cpu平均利用率快速方法*/
VOID cps_dev_cpu_usage_get_fast()
{
	UINT32 ulCount;

	/*统计时常/抽样时长获得统计时长内需要获取实时参数的次数*/
	ulCount = g_stCpsDevmCpuUsage.ulTime / g_stCpsDevmCpuUsage.ulSaTime;

	/*更新当前的实时利用率*/
	g_stCpsDevmCpuUsage.ulNowUse = cps_devm_cpu_usage_now();

	/*当前记录的数量增加1*/
	g_stCpsDevmCpuUsage.ulFlag++;

	/*记录的数量小于统计时长内的次数*/
	if(g_stCpsDevmCpuUsage.ulFlag <= ulCount)
	{
		g_stCpsDevmCpuUsage.ulTotalUse += g_stCpsDevmCpuUsage.ulNowUse;
		g_stCpsDevmCpuUsage.ulAvgUse = g_stCpsDevmCpuUsage.ulTotalUse / g_stCpsDevmCpuUsage.ulFlag;
	}
	/*记录的数量大于统计时长内的次数*/
	else
	{
		g_stCpsDevmCpuUsage.ulTotalUse = g_stCpsDevmCpuUsage.ulTotalUse - (g_stCpsDevmCpuUsage.ulTotalUse / ulCount) + g_stCpsDevmCpuUsage.ulNowUse;
		g_stCpsDevmCpuUsage.ulAvgUse = g_stCpsDevmCpuUsage.ulTotalUse / ulCount;
	}
}

/*获得内存平均利用率快速方法*/
VOID cps_dev_mem_usage_get_fast()
{
	UINT32 ulCount;

	ulCount = g_stCpsDevmMemUsage.ulTime / g_stCpsDevmMemUsage.ulSaTime;

	g_stCpsDevmMemUsage.ulNowUse = cpss_mem_usage_take();
	g_stCpsDevmMemUsage.ulFlag++;

	if(g_stCpsDevmMemUsage.ulFlag <= ulCount)
	{
		g_stCpsDevmMemUsage.ulTotalUse += g_stCpsDevmMemUsage.ulNowUse;
		g_stCpsDevmMemUsage.ulAvgUse = g_stCpsDevmMemUsage.ulTotalUse / g_stCpsDevmMemUsage.ulFlag;
	}
	else
	{
		g_stCpsDevmMemUsage.ulTotalUse = g_stCpsDevmMemUsage.ulTotalUse - (g_stCpsDevmMemUsage.ulTotalUse / ulCount) + g_stCpsDevmMemUsage.ulNowUse;
		g_stCpsDevmMemUsage.ulAvgUse = g_stCpsDevmMemUsage.ulTotalUse / ulCount;
	}
}

/*获得温度平均利用率快速方法*/
VOID cps_dev_temp_get_fast()
{
	UINT32 ulCount;
	ulCount = g_stCpsDevmTempUsage.ulTime / g_stCpsDevmTempUsage.ulSaTime;

//	g_stCpsDevmTempUsage.ulNowUse = cpss_devm_tmp_take();
	g_stCpsDevmTempUsage.ulFlag++;

	if(g_stCpsDevmTempUsage.ulFlag <= ulCount)
	{
		g_stCpsDevmTempUsage.ulTotalUse += g_stCpsDevmTempUsage.ulNowUse;
		g_stCpsDevmTempUsage.ulAvgUse = g_stCpsDevmTempUsage.ulTotalUse / g_stCpsDevmTempUsage.ulFlag;
	}
	else
	{
		g_stCpsDevmTempUsage.ulTotalUse = g_stCpsDevmTempUsage.ulTotalUse - (g_stCpsDevmTempUsage.ulTotalUse / ulCount) + g_stCpsDevmTempUsage.ulNowUse;
		g_stCpsDevmTempUsage.ulAvgUse = g_stCpsDevmTempUsage.ulTotalUse / ulCount;
	}
}

/*cpu告警检测*/
#if 0 //dhwang added
VOID cps_devm_cpu_alarm_check()
{
	UINT8 ucUsage;
	/*cpu利用率判断*/

	/*判断是否产生告警*/
	/*当前利用率是否高于上阈值并且未发送告警*/
	if((g_stCpsDevmCpuUsage.ulHigh < g_stCpsDevmCpuUsage.ulNowUse)&&(0 == g_stCpsDevmCpuUsage.ulAlarmFlag))
	{
		/*设置为已发送告警*/
		g_stCpsDevmCpuUsage.ulAlarmFlag = 1;
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "devm send om alarm occer! CpuUsage = %d\n", g_stCpsDevmCpuUsage.ulNowUse);
		ucUsage = (UINT8)g_stCpsDevmCpuUsage.ulNowUse;
		cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_CPUUSAGE_HIGH, OAMS_ALARM_OCCUR, (UINT8*)&ucUsage, sizeof(UINT8));
	}

	/*判断是否清除告警*/
	/*当前利用率是否低于下阈值并且已发送告警*/
	if((g_stCpsDevmCpuUsage.ulLow > g_stCpsDevmCpuUsage.ulNowUse)&&(1 == g_stCpsDevmCpuUsage.ulAlarmFlag))
	{
		/*设置为未发送告警*/
		g_stCpsDevmCpuUsage.ulAlarmFlag = 0;
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "devm send om alarm clean! CpuUsage = %d\n", g_stCpsDevmCpuUsage.ulNowUse);
		ucUsage = (UINT8)g_stCpsDevmCpuUsage.ulNowUse;
		cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_CPUUSAGE_HIGH, OAMS_ALARM_CLEAR, (UINT8*)&ucUsage, sizeof(UINT8));
	}
}
#endif
#if 0 //dhwang added
VOID cps_devm_mem_alarm_check()
{
	UINT8 ucUsage;
	/*内存利用率判断*/

	/*判断是否产生告警*/
	/*当前利用率是否高于上阈值并且未发送告警*/
	if((g_stCpsDevmMemUsage.ulHigh < g_stCpsDevmMemUsage.ulNowUse)&&(0 == g_stCpsDevmMemUsage.ulAlarmFlag))
	{
		/*设置为已发送告警*/
		g_stCpsDevmMemUsage.ulAlarmFlag = 1;
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "devm send om alarm occer! MemUsage = %d\n", g_stCpsDevmMemUsage.ulNowUse);
		ucUsage = (UINT8)g_stCpsDevmMemUsage.ulNowUse;
//		cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_CPUUSAGE_HIGH, OAMS_ALARM_OCCUR, (UINT8*)&ucUsage, sizeof(UINT8));
		cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_MEMUSAGE_HIGH, OAMS_ALARM_OCCUR, (UINT8*)&ucUsage, sizeof(UINT8));
	}

	/*判断是否清除告警*/
	/*当前利用率是否低于下阈值并且已发送告警*/
	if((g_stCpsDevmMemUsage.ulLow > g_stCpsDevmMemUsage.ulNowUse)&&(1 == g_stCpsDevmMemUsage.ulAlarmFlag))
	{
		/*设置为未发送告警*/
		g_stCpsDevmMemUsage.ulAlarmFlag = 0;
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "devm send om alarm clean! MemUsage = %d\n", g_stCpsDevmMemUsage.ulNowUse);
		ucUsage = (UINT8)g_stCpsDevmMemUsage.ulNowUse;
//		cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_CPUUSAGE_HIGH, OAMS_ALARM_CLEAR, (UINT8*)&ucUsage, sizeof(UINT8));
		cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_MEMUSAGE_HIGH, OAMS_ALARM_CLEAR, (UINT8*)&ucUsage, sizeof(UINT8));
	}
}
#endif
#if 0 //dhwang added

VOID cps_devm_temp_alarm_check()
{
	INT32 lRet;
	INT8 aucMacBuf[20];
	CPS_DEVM_TEMPERATURE_EXTRATION_T stTempExt;

	memset(&stTempExt, 0, sizeof(stTempExt));
	memset(aucMacBuf, 0, sizeof(aucMacBuf));

	/*判断是否产生告警*/
	/*当前利用率是否高于上阈值并且未发送告警*/
	if((g_stCpsDevmTempUsage.ulHigh < g_stCpsDevmTempUsage.ulNowUse)&&(0 == g_stCpsDevmTempUsage.ulAlarmFlag))
	{
		/*设置为已发送告警*/
		g_stCpsDevmTempUsage.ulAlarmFlag = 1;
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "devm send om alarm occer! MemUsage = %d\n", g_stCpsDevmTempUsage.ulNowUse);
		stTempExt.ucTemperature = (UINT8)g_stCpsDevmTempUsage.ulNowUse;

#if 0
		lRet = cps_verm_get_data_from_file("/devinfo/hardware-macaddr", aucMacBuf, sizeof(aucMacBuf));
		if(CPSS_OK == lRet)
		{
			lRet = cps_devm_mac_trans(aucMacBuf, &stTempExt.aucMac, sizeof(stTempExt.aucMac));
		}
#endif

		lRet = cpss_devm_mac_addr_get(&stTempExt.aucMac, sizeof(stTempExt.aucMac));
		cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_AC_TEMPERATURE_OVERHIGH, OAMS_ALARM_OCCUR, (UINT8*)&stTempExt, sizeof(stTempExt));
	}

	/*判断是否清除告警*/
	/*当前利用率是否低于下阈值并且已发送告警*/
	if((g_stCpsDevmTempUsage.ulLow > g_stCpsDevmTempUsage.ulNowUse)&&(1 == g_stCpsDevmTempUsage.ulAlarmFlag))
	{
		/*设置为未发送告警*/
		g_stCpsDevmTempUsage.ulAlarmFlag = 0;
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "devm send om alarm clean! MemUsage = %d\n", g_stCpsDevmTempUsage.ulNowUse);
		stTempExt.ucTemperature = (UINT8)g_stCpsDevmTempUsage.ulNowUse;
		

#if 0
		lRet = cps_verm_get_data_from_file("/devinfo/hardware-macaddr", aucMacBuf, sizeof(aucMacBuf));
		if(CPSS_OK == lRet)
		{
			lRet = cps_devm_mac_trans(aucMacBuf, &stTempExt.aucMac, sizeof(stTempExt.aucMac));
		}
#endif

		lRet = cpss_devm_mac_addr_get(&stTempExt.aucMac, sizeof(stTempExt.aucMac));
		cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_AC_TEMPERATURE_OVERHIGH, OAMS_ALARM_CLEAR, (UINT8*)&stTempExt, sizeof(stTempExt));
	}
}
#endif

#if 0 //dhwang added
VOID cps_devm_om_ac_para_chg(OM_AC_PARAMETER_CHG_IND_MSG_T* pstMsg)
{
	INT32 lRet;

	if(NULL == pstMsg)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cps_devm_om_ac_para_chg rcv NULL msg!\n");
		return;
	}

	lRet = 0;

#if 0
	memset(&g_stCpsDevmCpuUsage, 0, sizeof(CPS_DEVM_USAGE_T));
	memset(&g_stCpsDevmMemUsage, 0, sizeof(CPS_DEVM_USAGE_T));
	memset(&g_stCpsDevmTempUsage, 0, sizeof(CPS_DEVM_USAGE_T));
#endif

	g_stCpsDevmCpuUsage.ulFlag = 0;
	g_stCpsDevmCpuUsage.ulNowUse = 0;
	g_stCpsDevmCpuUsage.ulAvgUse = 0;
	g_stCpsDevmCpuUsage.ulTotalUse = 0;

	g_stCpsDevmMemUsage.ulFlag = 0;
	g_stCpsDevmMemUsage.ulNowUse = 0;
	g_stCpsDevmMemUsage.ulAvgUse = 0;
	g_stCpsDevmMemUsage.ulTotalUse = 0;

	g_stCpsDevmTempUsage.ulFlag = 0;
	g_stCpsDevmTempUsage.ulNowUse = 0;
	g_stCpsDevmTempUsage.ulAvgUse = 0;
	g_stCpsDevmTempUsage.ulTotalUse = 0;

	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "Cpu LowTh = %d, HighTh = %d\n", pstMsg->stChgAlarmPara.ulAcCpuUsageLowThreshhd, pstMsg->stChgAlarmPara.ulAcCpuUsageThreshhd);
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "Mem LowTh = %d, HighTh = %d\n", pstMsg->stChgAlarmPara.ulAcMemUsageLowThreshhd, pstMsg->stChgAlarmPara.ulAcMemUsageThreshhd);
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "Tem LowTh = %d, HighTh = %d\n", pstMsg->stChgAlarmPara.ulAcTemperatureLowThreshold, pstMsg->stChgAlarmPara.ulAcTemperatureThreshold);
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "SaTime = %d, WindowTime = %d\n\n", pstMsg->stChgAlarmPara.ulSampleTime, pstMsg->stChgAlarmPara.ulAcStatWindowTime);

	/*利用率不在范围内0~100*/
	if((0 >= cpss_ntohl(pstMsg->stChgAlarmPara.ulAcCpuUsageLowThreshhd)) || (100 < cpss_ntohl(pstMsg->stChgAlarmPara.ulAcCpuUsageLowThreshhd)))
	{
		lRet = 1;
	}

	if((0 >= cpss_ntohl(pstMsg->stChgAlarmPara.ulAcCpuUsageThreshhd)) || (100 < cpss_ntohl(pstMsg->stChgAlarmPara.ulAcCpuUsageThreshhd)))
	{
		lRet = 2;
	}

	if((0 >= cpss_ntohl(pstMsg->stChgAlarmPara.ulAcMemUsageLowThreshhd)) || (100 < cpss_ntohl(pstMsg->stChgAlarmPara.ulAcMemUsageLowThreshhd)))
	{
		lRet = 3;
	}

	if((0 >= cpss_ntohl(pstMsg->stChgAlarmPara.ulAcMemUsageThreshhd)) || (100 < cpss_ntohl(pstMsg->stChgAlarmPara.ulAcMemUsageThreshhd)))
	{
		lRet = 4;
	}

	if((0 >= cpss_ntohl(pstMsg->stChgAlarmPara.ulAcTemperatureLowThreshold)) || (100 < cpss_ntohl(pstMsg->stChgAlarmPara.ulAcTemperatureLowThreshold)))
	{
		lRet = 31;
	}

	if((0 >= cpss_ntohl(pstMsg->stChgAlarmPara.ulAcTemperatureThreshold)) || (100 < cpss_ntohl(pstMsg->stChgAlarmPara.ulAcTemperatureThreshold)))
	{
		lRet = 41;
	}

	/*统计时长小于采样时长*/
	if(cpss_ntohl(pstMsg->stChgAlarmPara.ulAcStatWindowTime) < cpss_ntohl(pstMsg->stChgAlarmPara.ulSampleTime))
	{
		lRet = 5;
	}

	/*下阈值比上阈值高*/
	if(cpss_ntohl(pstMsg->stChgAlarmPara.ulAcCpuUsageLowThreshhd) > cpss_ntohl(pstMsg->stChgAlarmPara.ulAcCpuUsageThreshhd))
	{
		lRet = 6;
	}

	if(cpss_ntohl(pstMsg->stChgAlarmPara.ulAcMemUsageLowThreshhd) > cpss_ntohl(pstMsg->stChgAlarmPara.ulAcMemUsageThreshhd))
	{
		lRet = 7;
	}

	if(cpss_ntohl(pstMsg->stChgAlarmPara.ulAcTemperatureLowThreshold) > cpss_ntohl(pstMsg->stChgAlarmPara.ulAcTemperatureThreshold))
	{
		lRet = 71;
	}

	/*统计时间不在范围内*/
	if((0 >= cpss_ntohl(pstMsg->stChgAlarmPara.ulAcStatWindowTime)) || (900 < cpss_ntohl(pstMsg->stChgAlarmPara.ulAcStatWindowTime)))
	{
		lRet = 8;
	}

	/*采样时间不在范围内*/
	if((0 >= cpss_ntohl(pstMsg->stChgAlarmPara.ulSampleTime)) || (900 < cpss_ntohl(pstMsg->stChgAlarmPara.ulAcStatWindowTime)))
	{
		lRet = 9;
	}

	if(0 == lRet)
	{
		g_stCpsDevmCpuUsage.ulHigh = cpss_ntohl(pstMsg->stChgAlarmPara.ulAcCpuUsageThreshhd);
		g_stCpsDevmCpuUsage.ulLow = cpss_ntohl(pstMsg->stChgAlarmPara.ulAcCpuUsageLowThreshhd);
		g_stCpsDevmCpuUsage.ulTime = cpss_ntohl(pstMsg->stChgAlarmPara.ulAcStatWindowTime);
		g_stCpsDevmCpuUsage.ulSaTime = cpss_ntohl(pstMsg->stChgAlarmPara.ulSampleTime);

		g_stCpsDevmMemUsage.ulHigh = cpss_ntohl(pstMsg->stChgAlarmPara.ulAcMemUsageThreshhd);
		g_stCpsDevmMemUsage.ulLow = cpss_ntohl(pstMsg->stChgAlarmPara.ulAcMemUsageLowThreshhd);
		g_stCpsDevmMemUsage.ulTime = cpss_ntohl(pstMsg->stChgAlarmPara.ulAcStatWindowTime);
		g_stCpsDevmMemUsage.ulSaTime = cpss_ntohl(pstMsg->stChgAlarmPara.ulSampleTime);

		g_stCpsDevmTempUsage.ulHigh = cpss_ntohl(pstMsg->stChgAlarmPara.ulAcTemperatureThreshold);
		g_stCpsDevmTempUsage.ulLow = cpss_ntohl(pstMsg->stChgAlarmPara.ulAcTemperatureThreshold);
		g_stCpsDevmTempUsage.ulTime = cpss_ntohl(pstMsg->stChgAlarmPara.ulAcStatWindowTime);
		g_stCpsDevmTempUsage.ulSaTime = cpss_ntohl(pstMsg->stChgAlarmPara.ulSampleTime);
	}
	else
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cps_devm_om_ac_para_chg receive ERROR PARA in part%d, use DEFAULT\n", lRet);

		g_stCpsDevmCpuUsage.ulHigh = 80;
		g_stCpsDevmCpuUsage.ulLow = 60;
		g_stCpsDevmCpuUsage.ulTime = 120;
		g_stCpsDevmCpuUsage.ulSaTime = 2;

		g_stCpsDevmMemUsage.ulHigh = 80;
		g_stCpsDevmMemUsage.ulLow = 60;
		g_stCpsDevmMemUsage.ulTime = 120;
		g_stCpsDevmMemUsage.ulSaTime = 2;

		g_stCpsDevmTempUsage.ulHigh = 80;
		g_stCpsDevmTempUsage.ulLow = 60;
		g_stCpsDevmTempUsage.ulTime = 120;
		g_stCpsDevmTempUsage.ulSaTime = 2;
	}

}
#endif

/*重启状态检查*/
#if 0 //dhwang added
VOID cps_devm_reboot_type_check()
{
	FILE* pFd;
	INT32 lRbtFlag;
	INT32 lRet;
	time_t timenow;
	struct tm *p;
	INT8 aucMacBuf[20];
	INT8* pCtime;
	CPS_DEVM_RESTART_EXTRATION_T stCpsRestartEx;

	/*获取启动状态信息*/
	pFd = fopen("/usr/local/wlan/ACSubSystem/CPS/reboot.log", "r");
	if(NULL == pFd)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cps_devm_reboot_type_check open /usr/local/wlan/ACSubSystem/CPS/reboot.log ERROR");
		return;
	}

	fscanf(pFd, "%d", &lRbtFlag);

	fclose(pFd);

	/*获取mac地址*/
	memset(aucMacBuf, 0, sizeof(aucMacBuf));
	memset(&stCpsRestartEx, 0, sizeof(stCpsRestartEx));

#if 0
	lRet = cps_verm_get_data_from_file("/devinfo/hardware-macaddr", aucMacBuf, sizeof(aucMacBuf));

//	printf("mac = %s\n", aucMacBuf);
//	printf("mac p = %p\n", &stCpsRestartEx.aucMac);

	if(CPSS_OK == lRet)
	{
		lRet = cps_devm_mac_trans(aucMacBuf, &stCpsRestartEx.aucMac, sizeof(stCpsRestartEx.aucMac));
	}
#endif

	lRet = cpss_devm_mac_addr_get(&stCpsRestartEx.aucMac, sizeof(stCpsRestartEx.aucMac));

//	printf("trans mac = %x-%x-%x-%x-%x-%x", stCpsRestartEx.aucMac[0], stCpsRestartEx.aucMac[1], stCpsRestartEx.aucMac[2], stCpsRestartEx.aucMac[3], stCpsRestartEx.aucMac[4], stCpsRestartEx.aucMac[5]);

	/*获取时间*/
#if 0
	time(&timenow);
	p = gmtime(&timenow);

	sprintf(stCpsRestartEx.aucTime, "%04u-%02u%02u-%02u:%02u:%02u",
					p->tm_year +1900, p->tm_mon +1, p->tm_mday, p->tm_hour +8, p->tm_min, p->tm_sec);

	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "in cps_devm_reboot_type_check "
			"Mac = %02X-%02X-%02X-%02X-%02X-%02X, time = %s\n",
			stCpsRestartEx.aucMac[0], stCpsRestartEx.aucMac[1], stCpsRestartEx.aucMac[2], stCpsRestartEx.aucMac[3], stCpsRestartEx.aucMac[4], stCpsRestartEx.aucMac[5],
			stCpsRestartEx.aucTime);
#endif

	time(&timenow);
	pCtime = ctime(&timenow);

	strncpy(&stCpsRestartEx.aucTime, pCtime, sizeof(stCpsRestartEx.aucTime));

	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "in cps_devm_reboot_type_check "
				"Mac = %02X-%02X-%02X-%02X-%02X-%02X, time = %s\n",
				stCpsRestartEx.aucMac[0], stCpsRestartEx.aucMac[1], stCpsRestartEx.aucMac[2], stCpsRestartEx.aucMac[3], stCpsRestartEx.aucMac[4], stCpsRestartEx.aucMac[5],
				stCpsRestartEx.aucTime);


	if(1 < lRbtFlag)
	{
		return;
	}
	else if(0 == lRbtFlag)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "cps send om COLD restart!\n");
		cps_send_om_event(OM_AM_CPS_EVENT_CODE_SYS_COLD_RESTART, &stCpsRestartEx, sizeof(stCpsRestartEx));
	}
	else if(1 == lRbtFlag)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "cps send om WARM restart!\n");
		cps_send_om_event(OM_AM_CPS_EVENT_CODE_SYS_WARM_RESTART, &stCpsRestartEx, sizeof(stCpsRestartEx));
	}
}
#endif
#if 0//dhwang added
void cps_send_ccm_port_chg_msg()
{
	CPS_DEVM_PORT_STATE_CHG_MSG_T stPortChg;
	CPSS_COM_PID_T stPid;
	INT32 lLoop;

	stPortChg.ulPortNum = 10;

	for(lLoop = 0; lLoop < stPortChg.ulPortNum; lLoop++)
	{
		stPortChg.astPortState[lLoop].ucSlotId = 1;
		stPortChg.astPortState[lLoop].ucPortId = lLoop + 1;
		stPortChg.astPortState[lLoop].ucState = gCpsDevmPortStateAll[lLoop + 11].ucNowState;;
		stPortChg.astPortState[lLoop].ucRsv = 0;
	}

#if 0
	for(lLoop = 0; lLoop < stPortChg.ulPortNum; lLoop++)
	{
		printf("Port = %d State = %d\n", stPortChg.astPortState[lLoop].ucPortId, stPortChg.astPortState[lLoop].ucState);
	}
#endif

	cpss_com_logic_addr_get(&stPid.stLogicAddr, &stPid.ulAddrFlag);
	stPid.ulPd = AC_MSCC_PROC;

	cpss_com_send(&stPid, CPS_DEVM_PORT_STATE_CHG_MSG, &stPortChg, sizeof(CPS_DEVM_PORT_STATE_CHG_MSG_T));
}
#endif

/*mac地址转换，由xx-xx-xx-xx-xx-xx字符串型转换为数组型*/
INT32 cps_devm_mac_trans(INT8* pucMacChar, INT8* pucMacNum, INT32 lLen)
{
	INT8* pcNum;
	INT8* pcSep;
	INT32 lNumLen;
	INT32 lLoop;
	INT32 lLoopin;
	INT32 lTemNum;
	INT32 lTim;
	INT32 lCount;
	INT8 acBuf[20];

	/*初始化指向数字的指针*/
	pcNum = pucMacChar;
	lCount = 0;

//	printf("pucMacNum = %p\n", pucMacNum);

	memset(acBuf, 0, sizeof(acBuf));

	while(1)
	{
		/*指向分隔符的指针*/
		pcSep = pcNum;
		if(0 == *pcSep)
		{
			break;
		}

		while('-' != *pcSep)
		{
			pcSep++;
			if(0 == *pcSep)
			{
				break;
			}
		}

		/*当前数字长度*/
		lNumLen = pcSep - pcNum;

		if(2 < lNumLen)
		{
			lNumLen = 2;
		}

//		printf("lNumLen = %d\n", lNumLen);

		/*由16进制字符串转换为数字*/
		for(lLoop = 0; lLoop < lNumLen; lLoop++)
		{
			if('a' <= pcNum[lLoop] && 'f' >= pcNum[lLoop])
			{
				lTemNum = pcNum[lLoop] - 'a' + 10;
			}
			else if('0' <= pcNum[lLoop] && '9' >= pcNum[lLoop])
			{
				lTemNum = pcNum[lLoop] - '0';
			}
			else if('A' <= pcNum[lLoop] && 'F' >= pcNum[lLoop])
			{
				lTemNum = pcNum[lLoop] - 'A' + 10;
			}
			else
			{
				break;
			}

			lTim = 1;

			for(lLoopin = 0; lLoopin < lNumLen - lLoop - 1; lLoopin ++)
			{
				lTim = lTim * 16;
			}

			acBuf[lCount] += lTemNum * lTim;

//			printf("pcuMacNum[%d] = %d\n", lCount, acBuf[lCount]);
		}

		pcNum = pcSep + 1;

		lCount++;
		if(lCount > lLen)
		{
			break;
		}
	}

	/*输出结果*/
	memcpy(pucMacNum, acBuf, lLen);

}

UINT32 cpss_devm_tmp_take()
{
	INT32 lRet;
	INT8* pcSlotId;
	INT32 lSlotId;

	pcSlotId = getenv("SlotId");
	if(NULL == pcSlotId)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cpss_devm_tmp_take error ! SlotId is NULL , use default\n");
		lSlotId = 1;
	}
	else
	{
		lSlotId = atoi(pcSlotId);
	}

	switch(lSlotId)
	{
	case 1:
		lRet = cps_system("getbrdtmp 11.33.9.1");
		lRet = lRet >> 8;
		break;
	case 2:
		lRet = cps_system("getbrdtmp 11.33.17.1");
		lRet = lRet >> 8;
		break;
	default:
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_WARN, "cpss_devm_tmp_take SlotId = %d Use default !\n", lSlotId);

		lRet = cps_system("getbrdtmp 11.33.9.1");
		lRet = lRet >> 8;
		break;
	}

	if(0 >= lRet)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cpss_devm_tmp_take error ! lRet = %d!\n", lRet);

		return 0;
	}

	return lRet;
}

INT32 cpss_devm_mac_addr_get(UINT8* pucBuf, INT32 lLen)
{
	FILE* pfd;
	UINT8 aucBuf[6];

	pfd = fopen("/devinfo/hardware-macaddr", "r");
	if(NULL == pfd)
	{
		return CPSS_ERROR;
	}

	fscanf(pfd, "%hhx-%hhx-%hhx-%hhx-%hhx-%hhx", &aucBuf[0], &aucBuf[1], &aucBuf[2], &aucBuf[3], &aucBuf[4], &aucBuf[5]);

	fclose(pfd);

	memcpy(pucBuf, aucBuf, lLen);
}

VOID cpss_devm_temp_get_task()
{
	while(1)
	{
		g_stCpsDevmTempUsage.ulNowUse = cpss_devm_tmp_take();

		cpss_delay(1000);
	}
}

INT32 cps_devm_get_mem_size()
{
	FILE* pFd;
	INT8 acBuf[256];
	INT8 acTmp[16];
	INT32 lSize;

	lSize = 2 * 1024 * 1024;

	pFd = fopen("/proc/meminfo", "r");
	if(NULL == pFd)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cps_devm_get_mem_size() Can't Open /proc/meminfo!\n");
		return lSize;
	}

	while(!feof(pFd))
	{
		fgets(acBuf, 256, pFd);

		if(0 == strncmp("MemTotal:", acBuf, strlen("MemTotal:")))
		{
			sscanf(acBuf, "%s %d %s", acTmp, &lSize, acTmp);

			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "cps_devm_get_mem_size() memsize = %d\n", lSize);
			break;
		}
	}

	fclose(pFd);

	return lSize;
}

INT32 cps_devm_rem_enter(INT8* pcBuf, UINT32 ulLen)
{
	INT32 lLoop;
	for(lLoop = 0; lLoop < ulLen; lLoop++)
	{
		if(('\n' == pcBuf[lLoop]) || ('\r' == pcBuf[lLoop]))
		{
			pcBuf[lLoop] = 0;
		}
	}

	return CPSS_OK;
}

INT32 cps_devm_get_sysinfo(INT8* pcFile, INT8* pcName, INT8* pcBuf, UINT32 ulLen)
{
	FILE* pFd;
	INT8* pTmp;
	INT8 acBuf[256];

	/*参数判断*/
	if(NULL == pcFile)
	{
		return CPSS_ERROR;
	}

	if(NULL == pcName)
	{
		return CPSS_ERROR;
	}

	if(NULL == pcBuf)
	{
		return CPSS_ERROR;
	}

	if(255 < strlen(pcName))
	{
		return CPSS_ERROR;
	}

	/*打开文件*/
	pFd = fopen(pcFile, "r");
	if(NULL == pFd)
	{
		return CPSS_ERROR;
	}

	/*查找标识*/
	while(!feof(pFd))
	{
		if(NULL == fgets(acBuf, 256, pFd))
		{
			fclose(pFd);
			return CPSS_ERROR;
		}

		if(0 == strncmp(pcName, acBuf, strlen(pcName)))
		{
			/*查找分隔符*/
			pTmp = strstr(acBuf, ":");
			if(NULL == pTmp)
			{
				continue;
			}

			/*消除空格*/
			do
			{
				pTmp = pTmp + 1;
			}
			while(*pTmp == ' ');

			/*拷入结果*/
			strncpy(pcBuf, pTmp, ulLen);
			/*消除回车*/
			cps_devm_rem_enter(pcBuf, ulLen);

			fclose(pFd);

			return CPSS_OK;
		}
	}

	fclose(pFd);

	return CPSS_ERROR;
}

INT32 cps_devm_get_mem_type(INT8* pcBuf, UINT32 ulLen)
{
	FILE* pFd;
	INT8* pTmp;
	INT8 acBuf[256];
	INT32 lLoop;

	/*打开执行命令文件*/
	pFd = popen("dmidecode -t 17", "r");
	if(NULL == pFd)
	{
		perror("popen");
		return CPSS_ERROR;
	}

	/*获取内存类型*/
	while(!feof(pFd))
	{
		fgets(acBuf, 256, pFd);
		/*找到Memory Device栏*/
		if(0 == strncmp(acBuf, "Memory Device", strlen("Memory Device")))
		{
			for(lLoop = 0; lLoop <5; lLoop ++)
			{
				fgets(acBuf, 256, pFd);
				/*找到Size项*/
				if(0 == strncmp(acBuf, "	Size:", strlen("	Size:")))
				{
					pTmp = strstr(acBuf, ":");

					do
					{
						pTmp = pTmp + 1;
					}
					while(*pTmp == ' ');
					/*判断是否安装内存*/
					if(0 != strncmp(pTmp, "No Module Installed", strlen("No Module Installed")))
					{
						for(lLoop = 0; lLoop <6; lLoop++)
						{
							fgets(acBuf, 256, pFd);
							if(0 == strncmp(acBuf, "	Type:", strlen("	Type:")))
							{
								pTmp = strstr(acBuf, ":");
								/*去掉空格*/
								do
								{
									pTmp = pTmp + 1;
								}
								while(*pTmp == ' ');
								/*拷入结果*/
								strncpy(pcBuf, pTmp, ulLen);
								/*去掉回车*/
								cps_devm_rem_enter(pcBuf, ulLen);

								pclose(pFd);

								return CPSS_OK;
							}
						}
					}
				}
			}
		}
	}

	pclose(pFd);

	return CPSS_ERROR;
}

INT32 cps_devm_sysinfo_init()
{
	INT32 lRet;
	INT8 acTmp[128];

	pstCpsDevmSysInfo = cpss_mem_malloc(sizeof(CPS_DEVM_SYSINFO_T));
	if(NULL == pstCpsDevmSysInfo)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cps_devm_sysinfo_init() malloc Error!");
		return CPSS_ERROR;
	}

	cpss_mem_memset(pstCpsDevmSysInfo, 0, sizeof(CPS_DEVM_SYSINFO_T));

	lRet = cps_devm_get_sysinfo("/proc/cpuinfo", "model name", pstCpsDevmSysInfo->aucCpuType, 128);
	if(CPSS_ERROR == lRet)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cps_devm_sysinfo_init() get cpuinfo Error!");
		strncpy(pstCpsDevmSysInfo->aucCpuType, "OCTEON Plus CN5860 Network Services Processor @1500MIPS", 128);
	}
	else
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "cps_devm_sysinfo_init() cpu type = %s", pstCpsDevmSysInfo->aucCpuType);
	}

	cpss_mem_memset(acTmp, 0, 128);
	lRet = cps_devm_get_sysinfo("/proc/meminfo", "MemTotal", acTmp, 128);
	if(CPSS_ERROR == lRet)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cps_devm_sysinfo_init() get mem size Error!");
		pstCpsDevmSysInfo->ulRamVol = 2 * 1024 * 1024;
	}
	else
	{
		pstCpsDevmSysInfo->ulRamVol = atoi(acTmp);
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "cps_devm_sysinfo_init() mem size = %d", pstCpsDevmSysInfo->ulRamVol);
	}


	lRet = cps_devm_get_mem_type(pstCpsDevmSysInfo->aucAcMemoryType, 16);
	if(CPSS_ERROR == lRet)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_ERROR, "cps_devm_sysinfo_init() get mem type Error!");
		strncpy(pstCpsDevmSysInfo->aucAcMemoryType, "DDR2", 16);
	}
	else
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_DEVM, CPSS_PRINT_INFO, "cps_devm_sysinfo_init() mem type = %s", pstCpsDevmSysInfo->aucAcMemoryType);
	}

	return lRet;
}

