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
#include "smss_verm.h"
//#include "om_public.h"
#include "cpss_common.h"

#include <sys/ipc.h>
#include <sys/fcntl.h>
#include <sys/msg.h>

#if 0 /*fow test whitout om*/
#define OM_SOFTWARE_FILENAME_LENTH  64 /*版本文件名64*/

typedef struct
{
	UINT8 aucAcVerFileName[OM_SOFTWARE_FILENAME_LENTH];/*版本文件名64*/
}OM_AC_VER_T;
#endif

#define AC_VERM_NAME_LENTH			OM_SOFTWARE_FILENAME_LENTH
#define AC_VERSION_CONFIG_FILE		"/usr/local/wlan/ACVersion/version.config"

typedef struct _CPS_AM_AC_VER_EXTRATION_
{
	CPSS_COM_PHY_ADDR_T  stPhyAddr;
	UINT8 aucBeforeUpGradeVer[32];/*升级前版本*/
	UINT8 aucAfterUpGradeVer[32]; /*升级后版本*/
	UINT8 aucCause[64];
}CPS_AM_AC_VER_EXTRATION_T;

#if 0
INT32 msgid;

typedef struct
{
	int cpu;
	int msg;
}BOOT_ALARM_T;

typedef struct
{
	INT32 Msgtpype;
	CHAR Msg[512];
}IPC_MSG_T;

IPC_MSG_T MsgP;
#endif

#if 0 /*for test with out om*/
typedef struct
{
	UINT8 ucCpuType;/*CPU类型0-主控板CPU；1-Cavium CPU*/
	UINT8 aucAcVerFileName[OM_SW_NAME_LENTH];/*版本文件名*/
}OM_AC_VER_T;
#endif

VOID smss_verm_state_idle_func();
VOID smss_verm_state_normal_func();

INT32 cps_verm_get_data_from_file(UINT8* pucPath, UINT8* pucBuf, UINT32 ulBufSize);

/*-1 失败，0 成功，1 升级中*/
INT32 g_lCpsUpdateStat = -1;

#if 0
VOID receive_boot_alarm_msg();
VOID send_verm_ccu_update();
VOID send_verm_ppu_update();
VOID get_om_ver(CHAR* ver, UINT8 ucCpuType);
#endif

extern char g_RunningVer[256];

#if 0
VOID send_ccu_verm_update_fail(int msg)
{
	CPSS_COM_PID_T stDstPid;
	CPS_VER_UPDATE_RSP_MSG_T CpsVerRsp;

	stDstPid.stLogicAddr = CPSS_LOGIC_ADDR_GCPA;
	stDstPid.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER;
	stDstPid.ulPd = SMSS_VERM_PROC;

	CpsVerRsp.iResult = msg;

	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "send to ccu ver update fail msg\n");

	cpss_com_send(&stDstPid,CPS_VER_UPDATE_RSP_MSG,(UINT8*)& CpsVerRsp,sizeof(CPS_VER_UPDATE_RSP_MSG_T));
	return;
}
#endif

#if 0
/*向ppu的verm发送版本升级指示*/
VOID send_ppu_verm_ver_update_ind(OM_AC_SOFTWARE_VER_INFO_T* ver_info)
{
	CPSS_COM_PID_T stDstPid;

	stDstPid.stLogicAddr.ucModule = ((UINT8)(16));
	stDstPid.stLogicAddr.usGroup = (((UINT16)(2)));
	stDstPid.stLogicAddr.ucSubGroup = ((UINT8)(2));

	stDstPid.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER;
	stDstPid.ulPd = SMSS_VERM_PROC;

#if 0 /*for test*/
	printf("**************************************send to ppu ver update ind!\n");
	printf("**************************************send to ppu renew ver cpu = %d\n",ver_info->ucCpuType);
	printf("**************************************send to ppu renewver name = %s\n",ver_info->aucAcVerFileName);
#endif
	cpss_com_send(&stDstPid, CPS_VER_UPDATE_REQ_MSG, (UINT8*)ver_info, sizeof(OM_AC_SOFTWARE_VER_INFO_T));
	return;
}
#endif

#if 0
/*从om获得版本信息*/
VOID ver_get_OM(CHAR* ver,INT32 flag)
{
	if (flag == 0)
	{
		CHAR newver[AC_VERM_NAME_LENTH] = "ccu.exe";
		memcpy(ver, newver, AC_VERM_NAME_LENTH);
		return;
#if 0
		CHAR* s;
		s = (CHAR*)ver;
		CHAR newver[50] = "ccu.exe";
		memcpy(s,newver,sizeof(newver));
		printf("**************************************OM ver = %s\n",ver);
		return;
#endif
	}
	else if(flag == 1)
	{
		CHAR newver[AC_VERM_NAME_LENTH] = "ppu_dpb.exe";
		memcpy(ver, newver, AC_VERM_NAME_LENTH);
		return;
#if 0
		CHAR* s;
		s = (CHAR*)ver;
		CHAR newver[50] = "ppu_dpb.exe";
		memcpy(s,newver,sizeof(newver));
		printf("**************************************OM ver = %s\n",ver);
		return;
#endif
	}
}
#endif

#if 0
/*获得本地当前的版本信息*/
VOID ver_get_local(CHAR* ver)
{
	strcpy(ver, g_RunningVer);
#if 0
	CHAR* s;
	s = (CHAR*)ver;
	CHAR newver[AC_VERM_NAME_LENTH] = "ccu";
	memcpy(ver,newver,sizeof(newver));
#endif
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "local ver = %s\n",ver);
	return;
}
#endif

#if 0
/*版本比较*/
INT32 verm_ver_compare(CHAR* ver1,CHAR* ver2)
{
	if(strcmp(ver1,ver2) == 0)
	{
		return CPSS_OK;
	}
	else
	{
		return CPSS_ERROR;
	}
}
#endif

/*verm纤程激活响应*/
INT32 send_smss_proc_activate_response(UINT32 result)
{
	CPSS_COM_PHY_ADDR_T stPhyAddr;
	SMSS_PROC_ACTIVATE_RSP_MSG_T stRspMsg;
	stRspMsg.ulResult = result;

	cpss_com_phy_addr_get(&stPhyAddr);

	return cpss_com_send_phy(stPhyAddr, SMSS_SYSCTL_PROC,
			SMSS_PROC_ACTIVATE_RSP_MSG, (UINT8 *) (&stRspMsg),
			sizeof(SMSS_PROC_ACTIVATE_RSP_MSG_T));
}

#if 0

/*向boot发送版本下载指示*/
VOID send_boot_download(CHAR* ver)
{
	BOOT_MSG_T stBootMsg =
	{	0};
	stBootMsg.ulMsgId = CPS_BOOT_RENEW_VERSION_IND_MSG;
	strcpy(stBootMsg.ucMsg, ver);

	cps_com_boot_snd_msg((char*)&stBootMsg);

	return;
	/*进程间通信*/
}
#endif

#if 0
/*向OM上告报警*/
VOID OM_warning()
{
	printf("**************************************alread send om a warning!\n");
}
#endif

#if 0
/*向pp发送版本查询结果*/
VOID send_ppu_ver_info(CHAR* ver, CPSS_COM_LOGIC_ADDR_T stSrcLogAddr)
{
	CPSS_COM_LOGIC_ADDR_T stLogAddr;
	CPSS_COM_PHY_ADDR_T stPhyAddr;
	SMSS_PROC_ACTIVATE_RSP_MSG_T stRspMsg;
	CHAR sendbuf[100];
	char* pver;

	stLogAddr.ucModule = stSrcLogAddr.ucModule;
	stLogAddr.usGroup = cpss_ntohs(stSrcLogAddr.usGroup);
	stLogAddr.ucSubGroup = stSrcLogAddr.ucSubGroup;

	stRspMsg.ulResult = SMSS_OK;

	memcpy(&stPhyAddr, &stLogAddr, sizeof(CPSS_COM_PHY_ADDR_T));
	//	stPhyAddr.ucCpu = 1;
	//	stPhyAddr.ucFrame = 1;
	//	stPhyAddr.ucShelf = 1;
	//	stPhyAddr.ucSlot = 3;

	pver = (CHAR*)ver;
	memcpy(sendbuf,pver,sizeof(sendbuf));
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "sendbuf = %s\n",sendbuf);

	cpss_com_send_phy(stPhyAddr, SMSS_BOOT_PROC, CPS_VER_CFG_QUERY_RSP_MSG, (UINT8 *)(&sendbuf),sizeof(sendbuf));
}
#endif

/*设置纤程状态*/
VOID smss_verm_state_set_new(UINT16 usNewState)
{
	cpss_vk_proc_user_state_set(usNewState);
	/*g_szSmssVermStateStr = aszSmssVermState[usNewState];*/
}

/*verm纤程入口*/
VOID smss_verm_proc(UINT16 usUserState, VOID *pvVar,
		CPSS_COM_MSG_HEAD_T *pstMsg)
{
	/* 判断消息合法 */
	if (pstMsg == NULL)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO,
				"verm proc recieve null msg!\n");
		return;
	}
	/* 状态分发 */
	switch (usUserState)
	{
	case SMSS_VERM_STATE_IDLE: /* Idle态处理函数 */
		smss_verm_state_idle_func(pstMsg);
		break;
	case SMSS_VERM_STATE_NORMAL: /* Normal态处理函数 */
		smss_verm_state_normal_func(pstMsg);
		break;
	default: /* 无效状态 */
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO,
				"UserState error!");
		break;
	}
}

/* Idle态处理函数 */
VOID smss_verm_state_idle_func(CPSS_COM_MSG_HEAD_T* pstMsg)
{
#if 0
	CHAR local_ver[AC_VERM_NAME_LENTH];
	CHAR om_ver[AC_VERM_NAME_LENTH];
#endif

	if (pstMsg == NULL)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO,
				"smss_verm_state_idle_func recieve null msg!\n");
		return;
	}

	switch (pstMsg->ulMsgId)
	{
	case SMSS_PROC_ACTIVATE_REQ_MSG: /* 收到纤程激活请求消息 */
		/* 回应纤程激活请求消息 */
		send_smss_proc_activate_response(SMSS_OK);
		break;
#if 0                               /* CCU中Verm的IDLE态需要完成版本查询功能 */
		case SMSS_PROC_ACTIVATE_REQ_MSG: /* 收到纤程激活请求消息 */
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "verm recieve activate req msg!\n");
		/* 向CCU_OM模块发送版本查询请求消息 */
		ver_get_local(local_ver);
		/* 查版本配置（OM接口）*/
		get_om_ver(om_ver, 0);
#if 0 /*for test*/
		ver_get_OM(om_ver,0);
#endif
		/* 版本比较 */
		if(verm_ver_compare(local_ver,om_ver) == CPSS_OK) /* 一致 */
		{
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "verm version compare ok,send activate rsq msg!\n");
			/*回纤程激活相应*/
			send_smss_proc_activate_response(SMSS_OK);
		}
		else /* 不一致 */
		{
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "verm version compare error,send boot download new version!\n");
			/*向boot发送下载指示*/
			send_boot_download(om_ver);
			/*创建接收boot消息任务*/
			//			CHAR* Receive_task = "receive task idle";
			//			cpss_vos_task_spawn(Receive_task,0,0,0,receive_boot_alarm_msg,2);
			/*设置超时定时器*/
			//			cpss_timer_set(SMSS_VERM_FDL_DELAY_TIMER,SMSS_VERM_FDL_DELAY_TIMEOUT);/*（下载失败，回激活失败）*/
		}
		break;
#if 0
		/*这个地方有问题，通过板间通信能否向纤程发消息?*/
		/*通过新启动的任务在接收到boot响应之后向verm发送*/
		case CPS_VER_UPDATE_RSP_MSG: /* 收到CCU_BOOT下载失败消息 */
		/*下载失败，回激活失败*/
		printf("**************************************boot download error!\n");
		/*删除超时定时器*/
		cpss_timer_delete(SMSS_VERM_FDL_DELAY_TIMER);
		/*向OM发告警*/
		OM_warning();
		break;
		/* 收到超时定时器消息 */
#endif
#if 0
		case SMSS_VERM_FDL_DELAY_TIMER_MSG:
		printf("boot download no response!\n");
		/*删除超时定时器*/
		cpss_timer_delete(SMSS_VERM_FDL_DELAY_TIMER);
		/*向OM发告警*/
		OM_warning();
		break;
#endif
#endif
		/* 纤程激活完成指示 */
	case SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG:
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO,
				"verm proc activate ok , now in normal state!\n");
		/* 转入normal态 */
		smss_verm_state_set_new(SMSS_VERM_STATE_NORMAL);
#if 0        /*for test*/
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
		cpss_timer_set(SMSS_VERM_DSP_LOAD_TIMER0,40*1000);
		cpss_timer_set(SMSS_VERM_DSP_LOAD_TIMER1,20*1000);
#endif
		/*end for test*/
		/*        CHAR *test = "this is a test!";
		 memcpy(&MsgP.Msg,test,50);
		 msgid = msgget(KEY,IPC_CREAT|6000);
		 msgsnd(msgid,&MsgP,50,0);*/
#endif
		break;
	default: /* 收到非法消息 */
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_ERROR,
				"verm recieve unknown msg!msg id = 0x%x\n", pstMsg->ulMsgId);
		break;
	}
}

/* Normal态处理函数 */
void smss_verm_state_normal_func(CPSS_COM_MSG_HEAD_T* pstMsg)
{
#if 0
	char om_ver[AC_VERM_NAME_LENTH];
	OM_AC_SOFTWARE_VER_INFO_T* om_ac_ver_info;
	CPS_VER_UPDATE_RSP_MSG_T* bootalarm;
#endif

	CPS_VERM_AC_UPDATE_REQ_T* stVerInfo;
	CPSS_COM_PID_T stDstPid;
//	OM_AC_VER_INFO_RSP_T stVerRsp;
//	OM_AC_VER_INFO_REQ_T* stVerReq;
	FILE* pfVerInfo;
	UINT8 aucBuf[256];
	INT32 lRet;
	INT8 acBuf[512];
	UINT8* pucVerFile;

	CPS_AM_AC_VER_EXTRATION_T stOmVerFail;

	if (pstMsg == NULL)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO,
				"smss_verm_state_normal_func recieve null msg!\n");
		return;
	}

	switch (pstMsg->ulMsgId)
	{
#if 0
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)                              /* PPU在normal态只做CCU发来的升级指示转发*/

	case CPS_VER_UPDATE_REQ_MSG: /* 收到CCU发来的版本升级指示 */
	/* 向PPU_BOOT发送版本升级指示 */
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "recieve ccu ver update msg!\n");
	om_ac_ver_info = (OM_AC_SOFTWARE_VER_INFO_T*)(pstMsg->pucBuf);
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "recieve ccu ver is %s\n",om_ac_ver_info->aucAcVerFileName);
	send_boot_download(om_ac_ver_info->aucAcVerFileName);
	//		cpss_timer_set(SMSS_VERM_FDL_DELAY_TIMER,SMSS_VERM_FDL_DELAY_TIMEOUT);/*下载失败，发告警*/
	//		CHAR* Receive_task = "receive task normal";
	/*创建接收boot消息任务*/
	//		cpss_vos_task_spawn(Receive_task,0,0,0,receive_boot_alarm_msg,3);
	break;
	/*通过新启动的任务在接收到boot响应之后向verm发送*/
#if 0
	case CPS_VER_UPDATE_RSP_MSG: /* 收到PPU_BOOT发来的升级失败消息 */
	/*删除超时定时器*/
	cpss_timer_delete(SMSS_VERM_FDL_DELAY_TIMER);
	BOOT_ALARM_T *alarm = (BOOT_ALARM_T*)(pstMsg->pucBuf);
	/* 向CCU_Verm发送PPU升级失败消息*/
	send_ccu_verm_update_fail(alarm->msg);
	break;
	/*boot响应超时*/
	case SMSS_VERM_FDL_DELAY_TIMEOUT:
	cpss_timer_delete(SMSS_VERM_FDL_DELAY_TIMER);
	/*向ccu_verm发送升级失败告警*/
	send_ccu_verm_update_fail(3);
	break;
#endif
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
	/* PPU初始化时版本处理的消息 */
	case CPS_VER_CFG_QUERY_REQ_MSG: /* 收到PPU上BOOT版本查询消息 */
	/* 向CCU_OM发送PPU版本查询消息 */
	get_om_ver(om_ver,1);
#if 0 /*fot test with no om*/
	ver_get_OM(om_ver,1);/*查询版本，回响应*/
#endif
	/* 向ppu发送版本信息 */
	send_ppu_ver_info(om_ver, pstMsg->stSrcProc.stLogicAddr);
	break;
#endif
	/* OM发送版本升级时处理的消息 */
	case OM_AC_RENEW_VERSION_IND_MSG: /* 收到OM版本升级指示 */

	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "recieve update msg!\n");
	/* 判断OM是向CCU还是向PPU发升级指示 */
	om_ac_ver_info = (OM_AC_SOFTWARE_VER_INFO_T*)(pstMsg->pucBuf);

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
	if(om_ac_ver_info->ucCpuType == 0)
	{
		/*向boot发送升级指示*/
		send_boot_download(om_ac_ver_info->aucAcVerFileName);
		/*创建接收boot消息任务*/
		//			CHAR* Receive_task = "receive task normal";
		//			cpss_vos_task_spawn(Receive_task,0,0,0,receive_boot_alarm_msg,3);
		/*设置超时定时器*/
		//			cpss_timer_set(SMSS_VERM_FDL_DELAY_TIMER,SMSS_VERM_FDL_DELAY_TIMEOUT);/*（下载失败，发告警）*/
	}
	else if(om_ac_ver_info->ucCpuType == 1)
	{
		/* 向PPU_VERM发送版本升级指示 */
		send_ppu_verm_ver_update_ind(om_ac_ver_info);
	}
	break;
#if 0
	case CPS_VER_UPDATE_RSP_MSG: /* 收到CCU_BOOT或PPU_Verm升级失败消息 */
	bootalarm = (CPS_VER_UPDATE_RSP_MSG_T*)(pstMsg->pucBuf);
	printf("recieve update error msg!msgid = %d\n",bootalarm->iResult);
	/* 向OM发送升级失败告警 */
	OM_warning();
	/*删除超时定时器*/
	cpss_timer_delete(SMSS_VERM_FDL_DELAY_TIMER);
	break;
#endif
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
#if 0
	/*向boot发送升级指示*/
	send_boot_download(om_ac_ver_info->aucAcVerFileName);
	/*设置超时定时器*/
	cpss_timer_set(SMSS_VERM_FDL_DELAY_TIMER,SMSS_VERM_FDL_DELAY_TIMEOUT);/*（下载失败，发告警）*/
	CHAR* Receive_task = "receive task normal";
	/*创建接收boot消息任务*/
	cpss_vos_task_spawn(Receive_task,0,0,0,receive_boot_alarm_msg,3);
	break;
#endif
#endif
#if 0
	/*板间通信是否能收到发送给纤程的消息？*/
	case SMSS_VERM_FDL_DELAY_TIMER_MSG:
	printf("boot download no response!\n");
	/*删除超时定时器*/
	cpss_timer_delete(SMSS_VERM_FDL_DELAY_TIMER);
	/*向OM发送升级失败告警*/
	OM_warning();
	break;
#endif
	/*for test*/

#if 0
	case SMSS_VERM_DSP_LOAD_TIMER_MSG0:
	send_verm_ccu_update();
	break;
	case SMSS_VERM_DSP_LOAD_TIMER_MSG1:
	send_verm_ppu_update();
	break;
#endif
#endif
#if 0 //dhwang added
	case OM_PM_STATISTIC_REQ_MSG:
		stVerReq = (OM_AC_VER_INFO_REQ_T*)pstMsg->pucBuf;
		memset(&stVerRsp, 0, sizeof(stVerRsp));
		memcpy(&stVerRsp.stRspHead, &stVerReq->stReqHead, sizeof(OM_PM_MSG_HEAD_T));

		lRet = cps_verm_get_data_from_file("/devinfo/software-file", stVerRsp.stAcSoftwareVerInfo.aucAcVerFileName, 256);
		if(CPSS_ERROR == lRet)
		{
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_ERROR, "in OM_PM_STATISTIC_REQ_MSG get software-file ERROR!\n");
			return lRet;
		}
		else
		{
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "in OM_PM_STATISTIC_REQ_MSG get software-file = %s!\n", stVerRsp.stAcSoftwareVerInfo.aucAcVerFileName);
		}

		lRet = cps_verm_get_data_from_file("/devinfo/software-name", stVerRsp.stAcSoftwareVerInfo.aucSoftwareName, 64);
		if(CPSS_ERROR == lRet)
		{
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_ERROR, "in OM_PM_STATISTIC_REQ_MSG get software-name ERROR!\n");
			return lRet;
		}
		else
		{
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "in OM_PM_STATISTIC_REQ_MSG get software-name = %s!\n", stVerRsp.stAcSoftwareVerInfo.aucSoftwareName);
		}

		lRet = cps_verm_get_data_from_file("/devinfo/software-vendor", stVerRsp.stAcSoftwareVerInfo.aucAcSoftwareVendor, 64);
		if(CPSS_ERROR == lRet)
		{
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_ERROR, "in OM_PM_STATISTIC_REQ_MSG get software-vendor ERROR!\n");
			return lRet;
		}
		else
		{
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "in OM_PM_STATISTIC_REQ_MSG get software-vendor = %s!\n", stVerRsp.stAcSoftwareVerInfo.aucAcSoftwareVendor);
		}

		lRet = cps_verm_get_data_from_file("/devinfo/software-verno", stVerRsp.stAcSoftwareVerInfo.aucAcSoftwareVersion, 64);
		if(CPSS_ERROR == lRet)
		{
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_ERROR, "in OM_PM_STATISTIC_REQ_MSG get software-verno ERROR!\n");
			return lRet;
		}
		else
		{
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "in OM_PM_STATISTIC_REQ_MSG get software-verno = %s!\n", stVerRsp.stAcSoftwareVerInfo.aucAcSoftwareVersion);
		}

#if 0
		pfVerInfo = fopen(AC_VERSION_CONFIG_FILE, "r");

		if(NULL == pfVerInfo)
		{
			perror("fopen");
			stVerRsp.stRspHead.ulResult = cpss_ntohl(1);
		}
		else
		{
			while(fgets((INT8*)aucBuf, sizeof(aucBuf), pfVerInfo))
			{
				if(0 == strncmp("software-name:", (INT8*)aucBuf, strlen("software-name:")))
				{
					sscanf((INT8*)aucBuf + strlen("software-name:"), "%s", stVerRsp.stAcSoftwareVerInfo.aucSoftwareName);
				}

				if(0 == strncmp("software-verno:", (INT8*)aucBuf, strlen("software-verno:")))
				{
					sscanf((INT8*)aucBuf + strlen("software-verno:"), "%s", stVerRsp.stAcSoftwareVerInfo.aucAcSoftwareVersion);
				}

				if(0 == strncmp("software-vendor:", (INT8*)aucBuf, strlen("software-vendor:")))
				{
					sscanf((INT8*)aucBuf + strlen("software-vendor:"), "%s", stVerRsp.stAcSoftwareVerInfo.aucAcSoftwareVendor);
				}

				if(0 == strncmp("software-file:", (INT8*)aucBuf, strlen("software-file:")))
				{
					sscanf((INT8*)aucBuf + strlen("software-file:"), "%s", stVerRsp.stAcSoftwareVerInfo.aucAcVerFileName);
				}
			}

			fclose(pfVerInfo);

			stVerRsp.stRspHead.ulResult = cpss_ntohl(0);
		}
#endif

		cpss_com_logic_addr_get(&stDstPid.stLogicAddr, &stDstPid.ulAddrFlag);
		stDstPid.ulPd = OAMS_PD_M_PM_MNGR_PROC;

		cpss_com_send(&stDstPid, OM_PM_STATISTIC_RSP_MSG, (UINT8*)(&stVerRsp), sizeof(stVerRsp));
		break;
#endif
	/*om下发的版本升级指示*/
	case CPS_VERM_AC_UPDATE_REQ_MSG:
		g_lCpsUpdateStat = 1;

		stVerInfo = (CPS_VERM_AC_UPDATE_REQ_T*)(pstMsg->pucBuf);
		if(0 >= strlen((INT8*)stVerInfo->aucAcVersionPath))
		{
			g_lCpsUpdateStat = -1;
			
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_ERROR, "in CPS_VERM_AC_UPDATE_REQ_MSG ver  Path is NULL!\n");
			return;
		}

		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "in verm ac update !, file  = %s\n", stVerInfo->aucAcVersionPath);

		memset(acBuf, 0, sizeof(acBuf));

		sprintf(acBuf, "update %s", stVerInfo->aucAcVersionPath);

		lRet = cps_system(acBuf);

		lRet = lRet >> 8;

		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "Ver update result = %d\n", lRet);

		if(0 != lRet)
		{
			g_lCpsUpdateStat = -1;
			
			cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_ERROR, "Ver update FAIL! send om alarm! Ret = %d\n", lRet);

			memset(&stOmVerFail, 0, sizeof(CPS_AM_AC_VER_EXTRATION_T));

			stOmVerFail.stPhyAddr.ucCpu = 1;
			stOmVerFail.stPhyAddr.ucFrame = 1;
			stOmVerFail.stPhyAddr.ucShelf = 1;
			stOmVerFail.stPhyAddr.ucSlot = 1;

			switch(lRet)
			{
			case 1:
				sprintf(&stOmVerFail.aucCause, "file no exist");
				break;
			case 2:
				sprintf(&stOmVerFail.aucCause, "file format error");
				break;
			default:
				sprintf(&stOmVerFail.aucCause, "unknown error");
				break;
			}
//			/*填写错误原因*/
//			sprintf(&stOmVerFail.aucCause, "%d", lRet);

			/*获取原版本文件*/
			lRet = cps_verm_get_data_from_file("/devinfo/software-file", stOmVerFail.aucBeforeUpGradeVer, 32);
			if(CPSS_ERROR == lRet)
			{
				sprintf(&stOmVerFail.aucBeforeUpGradeVer, "unknown");
//				return lRet;
			}

			/*获取升级版本文件*/
			pucVerFile = stVerInfo->aucAcVersionPath + strlen(stVerInfo->aucAcVersionPath);
			while('/' != *pucVerFile)
			{
				if(pucVerFile == stVerInfo->aucAcVersionPath)
				{
					break;
				}

				pucVerFile--;
			}

			strncpy(&stOmVerFail.aucAfterUpGradeVer, pucVerFile + 1, 32);
#if 0 //dhwang added
			cps_send_om_event(OM_AM_CPS_EVENT_CODE_SOFTWARE_UPGRADE_FAIL, &stOmVerFail, sizeof(stOmVerFail));
#endif
		}
		else
		{
			g_lCpsUpdateStat = 0;
			memset(&stOmVerFail, 0, sizeof(CPS_AM_AC_VER_EXTRATION_T));

			stOmVerFail.stPhyAddr.ucCpu = 1;
			stOmVerFail.stPhyAddr.ucFrame = 1;
			stOmVerFail.stPhyAddr.ucShelf = 1;
			stOmVerFail.stPhyAddr.ucSlot = 1;

			/*获取原版本文件*/
			lRet = cps_verm_get_data_from_file("/devinfo/software-file", stOmVerFail.aucBeforeUpGradeVer, 32);
			if(CPSS_ERROR == lRet)
			{
				sprintf(&stOmVerFail.aucBeforeUpGradeVer, "unknown");
//				return lRet;
			}

			/*获取升级版本文件*/
			pucVerFile = stVerInfo->aucAcVersionPath + strlen(stVerInfo->aucAcVersionPath);
			while('/' != *pucVerFile)
			{
				if(pucVerFile == stVerInfo->aucAcVersionPath)
				{
					break;
				}

				pucVerFile--;
			}

			strncpy(&stOmVerFail.aucAfterUpGradeVer, pucVerFile + 1, 32);

			/*填写成功信息*/
			sprintf(&stOmVerFail.aucCause, "update success!");
#if 0 //dhwang added
			cps_send_om_event(OM_AM_CPS_EVENT_CODE_SOFTWARE_UPGRADE_SUCCESS, &stOmVerFail, sizeof(stOmVerFail));
#endif
		}
//		execl("update", stVerInfo->aucAcVersionPath, NULL);
		break;
	/*end for test*/
	default: /* 收到非法消息 */
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO,
				"verm recieve unkonw msg in normal state! msgid = 0x%x\n",
				pstMsg->ulMsgId);
		break;
	}
}

#if 0
VOID receive_boot_alarm_msg(INT32 flag)
{
#if 0 /*for test*/
	IPC_MSG_T rcvbuff;
	printf("\nwait for boot response!\n");
	msgrcv(msgid,&rcvbuff,AC_VERM_NAME_LENTH,2,0);
	printf("recieve msg =%s\n",rcvbuff.Msg);
	return;
#endif

	BOOT_ALARM_T bootalarm;
	IPC_MSG_T rcvbuff;
	printf("\nwait for boot response!\n");
	msgrcv(msgid,&rcvbuff,256/*sizeof(IPC_MSG_T)*/,flag,0);
	memcpy(&bootalarm,&rcvbuff.Msg,sizeof(BOOT_ALARM_T));
	printf("boot alarm cpu = %d\n",bootalarm.cpu);
	printf("boot alarm msg = %d\n",bootalarm.msg);
#if 0
	printf("send smss activate error!\n");
	/*不由自己判断错误，向verm发送错误信息*/
	send_smss_proc_activate_response(SMSS_ERROR);
#endif
	send_verm_alarm_msg(bootalarm);
}
#endif

#if 0
VOID send_verm_alarm_msg(BOOT_ALARM_T boot_alarm)
{
	CPSS_COM_PHY_ADDR_T stPhyAddr;

	cpss_com_phy_addr_get(&stPhyAddr);

	cpss_com_send_phy(stPhyAddr, SMSS_VERM_PROC, CPS_VER_UPDATE_RSP_MSG, (UINT8 *)(&boot_alarm),sizeof(BOOT_ALARM_T));

	return;
}
#endif

#if 0
//#define OAMS_AM_ALARM_EXTRA_INFO_LEN (64)
#if 0 /*for test with out om*/

typedef struct _OM_AM_ORIGIN_ALARM_
{
	UINT32 ulAlarmNo; /* 告警号 */
	UINT32 ulAlarmSubNo; /* 告警细节号*/
	UINT32 ulAlarmType; /* 告警类型：0告警产生；1告警清除 */
	UINT8 aucExtraInfo[OAMS_AM_ALARM_EXTRA_INFO_LEN]; /* 告警额外信息 */
}OAMS_AM_ORIGIN_ALARM_T;

VOID oams_am_send_alarm (OAMS_AM_ORIGIN_ALARM_T *pstAlarm)
{
	return;
}
#endif

#if 0
#if SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA
void OM_warning_bak()
{
	OAMS_AM_ORIGIN_ALARM_T oams_alarm;
	oams_alarm.ulAlarmNo = 1;
	oams_alarm.ulAlarmSubNo = 1;
	//	oams_am_send_alarm(&oams_alarm);
	return;
}
#endif
#endif

/*for test*/
VOID send_verm_ccu_update()
{
	OM_AC_SOFTWARE_VER_INFO_T verup;
	char* tmp = "ccu1.exe";
	CPSS_COM_PHY_ADDR_T stPhyAddr;

	verup.ucCpuType = 0;

	memcpy(&verup.aucAcVerFileName, tmp, AC_VERM_NAME_LENTH);

	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "ccu renew version = %s\n",verup.aucAcVerFileName);

	cpss_com_phy_addr_get(&stPhyAddr);

	cpss_com_send_phy(stPhyAddr, SMSS_VERM_PROC, OM_AC_RENEW_VERSION_IND_MSG, (UINT8 *)(&verup),sizeof(OM_AC_SOFTWARE_VER_INFO_T));

	return;
}

/*for test*/
VOID send_verm_ppu_update()
{
	OM_AC_SOFTWARE_VER_INFO_T verup;
	char* tmp = "ppu1.exe";
	CPSS_COM_PHY_ADDR_T stPhyAddr;

	verup.ucCpuType = 1;

	memcpy(&verup.aucAcVerFileName, tmp, AC_VERM_NAME_LENTH);

	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, "ppu renew version = %s\n",verup.aucAcVerFileName);
	cpss_com_phy_addr_get(&stPhyAddr);

	cpss_com_send_phy(stPhyAddr, SMSS_VERM_PROC, OM_AC_RENEW_VERSION_IND_MSG, (UINT8 *)(&verup),sizeof(OM_AC_SOFTWARE_VER_INFO_T));

	return;
}

#if 0 /*for om*/

typedef struct
{ /* */
	UINT8 ucCpuType;/*CPU类型0-主控板CPU；1-Cavium CPU*/
	UINT8 aucRev[3];
}OM_GET_AC_VER_MSG_REQ_T;

typedef struct
{
	UINT32 ulResult; /* 返回结果，0表示成功，其他表示错误码 */
	OM_AC_VER_T stAcVer;
}OM_GET_AC_VER_MSG_RSP_T;

#define OM_GET_AC_VER_MSG	                   0x24014001
#endif

#if 0
VOID rdbs_ac_if_entry(UINT32 ulMsgId,UINT8 ucMsgType, UINT8* pucInPara,UINT32 ulInParaLen, UINT8* pucOutPara,UINT32 ulOutParaLen)
{
	return;
}
#endif

#if SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA
VOID get_om_ver(CHAR* ver, UINT8 ucCpuType)
{
	OM_GET_AC_VER_MSG_REQ_T stReq;
	OM_GET_AC_VER_MSG_RSP_T stRsp;

	stReq.ucCpuType = ucCpuType;
	//	rdbs_ac_if_entry(OM_GET_AC_VER_MSG, 0, (UINT8*)&stReq,sizeof(OM_GET_AC_VER_MSG_REQ_T), (UINT8*)&stRsp, sizeof(OM_GET_AC_VER_MSG_RSP_T));

	if(NULL == stRsp.stAcVer.aucAcVerFileName)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_ERROR,"AC ver is NULL!\n");
		return;
	}
	strcpy(ver, stRsp.stAcVer.aucAcVerFileName);
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO,"AC ver in OM is %s\n", ver);
	return;
}
#endif

//#endif/*end for om*/
#endif

INT32 cps_verm_get_data_from_file(UINT8* pucPath, UINT8* pucBuf, UINT32 ulBufSize)
{
	FILE* pfFile;

	if((NULL == pucPath) || (NULL == pucBuf))
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_ERROR, "in cps_verm_get_data_from_file ver  pucPath or pucBuf is NULL!\n");
		return CPSS_ERROR;
	}

	pfFile = fopen(pucPath, "r");
	if(NULL == pfFile)
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_ERROR, "in cps_verm_get_data_from_file ver  fopen %s ERROR!\n", pucPath);
		return CPSS_ERROR;
	}

	if(NULL == fgets(pucBuf, ulBufSize, pfFile))
	{
		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_ERROR, "in cps_verm_get_data_from_file fgets %s!\n", pucBuf);
		fclose(pfFile);
		return CPSS_ERROR;
	}
	
	if(10 == pucBuf[strlen(pucBuf) - 1])
	{
		pucBuf[strlen(pucBuf) - 1] = 0;
	}

	fclose(pfFile);
	return CPSS_OK;
}

INT32 cps_verm_get_update_stat()
{
	return g_lCpsUpdateStat;
}


