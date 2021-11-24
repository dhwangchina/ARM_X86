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
#include "cps_replant.h"

#define  CPS__RDBS_HTONS(A,B)    ( (CPS__RDBS_IF_ASYNMSG == (B)) ? (cpss_htons(A)) : (A))
#define  CPS__RDBS_HTONL(A,B)    ( (CPS__RDBS_IF_ASYNMSG == (B)) ? (cpss_htonl(A)) : (A))

INT32 cpss_dir_used_space_get_extend( const STRING szDirName, UINT32 *pulSizeHigh, UINT32 *pulSizeLow, UINT32 *pulFileNum)
{
    return CPSS_OK;
}

VOID cps__oams_moid_get_ne_id(UINT16 *pusNeId)
{
	return;
}

VOID smss_sysctl_state_stoa_func(CPSS_COM_MSG_HEAD_T * pstMsg)
{
	return;
}

INT32 cps__oams_moid_get_local_moid(UINT32 ulMoIdType, SWP_MO_ID_T *pstOutMoId)
{
	return 0;
}

INT32 cps__oams_moid_get_moid_by_phy(UINT32 ulMoIdType,
                                CPSS_COM_PHY_ADDR_T *pstPhyAddr,
                                SWP_MO_ID_T *pstOutMoId)
{
	return 0;
}

STATUS drv_board_led_set(UINT8 ucLedType, UINT8 ucLedSw)
{
	return 0;
}

BOOL smss_sysctl_idle_as_func(CPSS_COM_MSG_HEAD_T *pstMsg)
{
	return FALSE;
}

VOID smss_sysctl_stoa_complete(VOID)
{
	return;
}

INT32 cpss_disk_used_space_get(const STRING szDiskName,UINT32 *pulSizeHigh,UINT32 *pulSizeLow)
{
    return CPSS_OK;
}

void cps__oams_shcmd_reg(STRING szCmdName, STRING szCmdHelp, STRING szArgFmt,CPS__OAMS_SHCMD_PF  pfCmdFunc)
{
    return;
}

BOOL smss_sysctl_init_as_func(CPSS_COM_MSG_HEAD_T *pstMsg)
{
	return FALSE;
}

INT32 drv_clk_syn_select_byslot(UINT8 ucSlot)
{
	return 0;
}

VOID smss_sysctl_idle_mate_init(VOID)
{
	return;
}
/*
INT32 cpss_fs_fd_close()
{
	return CPSS_OK;
}
*/
VOID  cps__oams_am_send_alarm(CPS__OAMS_AM_ORIGIN_ALARM_T  *pstAlarm)
{
	return;
}

STATUS drv_ipmi_set_fru_led(UINT8 ucLedState)
{
	return 0;
}

BOOL smss_sysctl_normal_as_func(CPSS_COM_MSG_HEAD_T *pstMsg)
{
	return FALSE;
}

VOID  cps__oams_am_send_event(CPS__OAMS_AM_ORIGIN_EVENT_T  *pstEvent)
{
	return;
}

//BOOL  smss_get_proc_active_state(VOID){return TRUE;};

VOID cps__rdbs_if_entry(UINT32 ulMsgId, UINT8 ucCallType, UINT8 *pucInPara, UINT32 ulInParaLen, \
                          UINT8 *pucOutPara, UINT32 ulOutParaLen)
{
	switch(ulMsgId)
	{
		case CPS__RDBS_DEV_GETLOGICADDR_MSG:
		{
			CPS__RDBS_DEV_GETLOGICADDR_MSG_REQ_T *pstReqMsg;
			CPS__RDBS_DEV_GETLOGICADDR_MSG_RSP_T *pstRspMsg;
		    pstReqMsg = (CPS__RDBS_DEV_GETLOGICADDR_MSG_REQ_T*)pucInPara;
		    pstRspMsg = (CPS__RDBS_DEV_GETLOGICADDR_MSG_RSP_T*)pucOutPara;



		    switch(*(UINT32*)(&pstReqMsg->stCpuPhyAddr))
		    {

		    case 0x01010101:

		    {/* 出参设置 */
		    /** 逻辑地址 **/
		    /** 模块号 **/
		    pstRspMsg->stCpuLogicAddr.ucModule   = ((UINT8)(16));
		    /** 组号 **/
		    pstRspMsg->stCpuLogicAddr.usGroup    = CPS__RDBS_HTONS(((UINT16)(0x8111)), ucCallType);
		    /** 子组号 **/
		    pstRspMsg->stCpuLogicAddr.ucSubGroup = ((UINT8)(1));
		    /** 备份类型 **/
		    pstRspMsg->ucBackupType              = ((UINT8)(0));
		    /** 结果成功 **/
		    pstRspMsg->ulResult = CPS__RDBS_HTONL( ((UINT32)(0)), ucCallType);
		    }
		    break;

		    case 0x01020101:

		    {/* 出参设置 */
		    /** 逻辑地址 **/
		    /** 模块号 **/
		    pstRspMsg->stCpuLogicAddr.ucModule   = ((UINT8)(16));
		    /** 组号 **/
		    pstRspMsg->stCpuLogicAddr.usGroup    = CPS__RDBS_HTONS(((UINT16)(0x8112)), ucCallType);
		    /** 子组号 **/
		    pstRspMsg->stCpuLogicAddr.ucSubGroup = ((UINT8)(2));
		    /** 备份类型 **/
		    pstRspMsg->ucBackupType              = ((UINT8)(0));
		    /** 结果成功 **/
		    pstRspMsg->ulResult = CPS__RDBS_HTONL( ((UINT32)(0)), ucCallType);
		    }

		    break;

		    case 0x01030101:/*dpb1*/

		    {/* 出参设置 */
		    /** 逻辑地址 **/
		    /** 模块号 **/
		    pstRspMsg->stCpuLogicAddr.ucModule   = ((UINT8)(16));
		    /** 组号 **/
		    pstRspMsg->stCpuLogicAddr.usGroup    = CPS__RDBS_HTONS(((UINT16)(0x8113)), ucCallType);
		    /** 子组号 **/
		    pstRspMsg->stCpuLogicAddr.ucSubGroup = ((UINT8)(2));
		    /** 备份类型 **/
		    pstRspMsg->ucBackupType              = ((UINT8)(0));
		    /** 结果成功 **/
		    pstRspMsg->ulResult = CPS__RDBS_HTONL( ((UINT32)(0)), ucCallType);
		    }

		    break;

		    case 0x01040101:/*dpb2*/

		    {/* 出参设置 */
		    /** 逻辑地址 **/
		    /** 模块号 **/
		    pstRspMsg->stCpuLogicAddr.ucModule   = ((UINT8)(16));
		    /** 组号 **/
		    pstRspMsg->stCpuLogicAddr.usGroup    = CPS__RDBS_HTONS(((UINT16)(0x8114)), ucCallType);
		    /** 子组号 **/
		    pstRspMsg->stCpuLogicAddr.ucSubGroup = ((UINT8)(2));
		    /** 备份类型 **/
		    pstRspMsg->ucBackupType              = ((UINT8)(0));
		    /** 结果成功 **/
		    pstRspMsg->ulResult = CPS__RDBS_HTONL( ((UINT32)(0)), ucCallType);
		    }

		    break;

		    }

		}
	}
}

int smss_sysctl_as_evoked_atos()
{
	return 0;
}

int smss_sysctl_as_start_mate_heartbeat()
{
	return 0;
}

STATUS  drv_ipmi_addr_get(UINT8  ucAddrType,UINT32 *pulAddr)
{
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)

	FILE* fProc;
	char pcBuf[50];
	int lBayId;

	*pulAddr = 0x01010301;

	fProc = fopen("/proc/octeon_info", "r");
	if(NULL == fProc)
	{
		cpss_message("open /proc/octeon_info fail!, use default\n");
		return 0;
	}

	while(!feof(fProc))
	{
		fscanf(fProc, "%s", pcBuf);
		if(0 == strcmp("bay", pcBuf))
		{
			fscanf(fProc, "%s", pcBuf);
			if(0 == strcmp("id:", pcBuf))
			{
				fscanf(fProc, "%d", &lBayId);
				switch(lBayId)
				{
					case 1:
						cpss_message("Get DpbId = 1\n");
						*pulAddr = 0x01010301;
						break;
					case 2:
						cpss_message("Get DpbId = 2\n");
						*pulAddr = 0x01010401;
						break;
					default:
						cpss_message("Get Error CpuId, use Default\n");
						break;
				}
				break;
			}
		}
	}

	fclose(fProc);
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
	*pulAddr = 0x01010101;
#endif
	return 0;
}

INT32 cpss_fs_init()
{
	return (CPSS_OK);
}

UINT32 drv_gmpa_temp_monitor_init()
{
	return DRV_BSP_OK;
}

STATUS drv_host_cpu_init(void)
{
	return DRV_BSP_OK;
}

STATUS  drv_ether_port_comm_init
(UINT8  ucPortType,UINT8  *pucMacAddr,UINT32  *pulIpAddr)
{
  return OK;
}

INT32 drv_shcmd_output_redirect
(DRV_SHCMD_OUTPUT_ACCEPT_PF  pfAccept)
{
    return DRV_BSP_OK;
}

INT32 cps__oams_moid_hton(SWP_MO_ID_T  *pstMOID)
{
	return CPS__OAMS_OK;
}

INT32 drv_atca717_cpu_temp_get(UINT32 *pulCpuTemp)
{
	return DRV_BSP_OK;
}

INT32 drv_board_control_active_port_get(UINT32 *pulActivePort)
{
	return DRV_BSP_OK;
}

INT32 drv_shcmd_info_get(UINT32 ulIndex,DRV_SHCMD_INFO_T *pstOut)
{
	return DRV_BSP_OK;
}

UINT32 cps__oams_am_agt_rpt_get_actalarm_num(VOID)
{
	return 0;
}
/*
UINT32 cpss_file_open(const STRING szFileName,INT32 lFlag)
{
	return CPSS_OK;
}

INT32 cpss_file_close(UINT32 ulFd)
{
	return CPSS_OK;
}

INT32 cpss_file_write(UINT32 ulFd,const VOID *pvBuf,UINT32 ulMaxLen,UINT32 *pulWriteLen)
{
	return CPSS_OK;
}

INT32 cpss_file_read(UINT32 ulFd, VOID   *pvBuf, UINT32 ulMaxLen, UINT32 *pulReadLen)
{
	return CPSS_OK;
}

INT32 cpss_file_seek(UINT32 ulFd, UINT32 ulOffset, INT32 lOrigin, UINT32 *pulSeekPos)
{
	return CPSS_OK;
}
*/
UINT32  drv_ipmc_bt_enable(UINT8 ucEnable)
{
	return DRV_BSP_OK;
}

UINT32  drv_ipmc_update(UINT32 ulVerLen,VOID  *pvIpmcVer,UINT8  ucIpmcId)
{
	return DRV_BSP_OK;
}
/*
INT32 cpss_file_delete(const STRING szFileName)
{
	return CPSS_OK;
}
*/
STATUS  drv_ether_macip_get
(
    CPSS_COM_PHY_ADDR_T *pstCpuPhyAddr,     /* 单板物理地址存储空间；*/
    UINT8  ucPortType,   /* 以太网络通信端口的类别 */
    UINT8  ucDevId,      /* 板卡上以太端口号ulDevId */
    UINT8  *pucMacAddr,  /*此端口的配置MAC地址*/
    UINT32  *pulIpAddr   /* 此端口的配置IP地址*/
)
{
	return DRV_BSP_OK;
}
/*
BOOL cpss_file_exist(const CHAR *pcFileName)
{
	return CPSS_OK;
}

INT32 cpss_file_get_size(const STRING szFileName, UINT32 *pulSize)
{
	return CPSS_OK;
}
*/
typedef enum
{
    WATCHDOG_OPEN = 0x0,/* 打开WatchDog*/
    WATCHDOG_TIME_SET,   /*时间间隔设置*/
    WATCHDOG_FEED_SET,  /* 喂狗操作*/
    WATCHDOG_CLOSE,      /*关闭WatchDog*/
}DRV_WATCHDOG_COTL_E;

STATUS  drv_wd_control(DRV_WATCHDOG_COTL_E  enWdAction,UINT16 usWdTime)
{
	return(DRV_BSP_OK);
}

/*单板指定FPGA芯片的逻辑文件下载*/
STATUS drv_fpga_load
(
UINT8 ucChipNo,   /*板内的FPGA芯片编号；范围最大为0－1.有些板卡最为1片,有些板卡为两片*/
UINT8 *pucBuf,    /*存储需要烧写的FPGA二进制逻辑文件的缓存地址*/
UINT32 ulLen      /*FPGA二进制逻辑在内存的缓冲区长度*/
)
{
    return DRV_BSP_OK;
}

INT32 cps__oams_comm_get_ftpsvr_info(UINT32 ulFtpSvrType,
                                CPS__OAMS_COMM_FTP_SERVER_INFO_T *pstFtpSvr)
{
	return (CPS__OAMS_OK);
}
/*
UINT32 cpss_dir_open
(
const STRING szDirName
)
{
	return 0;
}

INT32 cpss_dir_close
(
UINT32 ulDd
)
{
	return CPSS_OK;
}

STRING cpss_dir_read
(
UINT32 ulDd
)
{
	return CPSS_OK;
}
*/
STATUS  drv_board_hwinfo_get
(
  DRV_PHYBRD_INTEGRATED_INFO_VER_T *pstBoardInfo
)
{
	return DRV_BSP_OK;
}
/*
INT32 cpss_file_copy
(
const STRING szSrcFile,
const STRING szDstFile
)
{
	return CPSS_OK;
}
*/
STATUS  drv_board_pcb_info_get
(
    DRV_PHYBRD_INTEGRATED_INFO_PCB_T *pstPcbInfo
)
{
	return DRV_BSP_OK;
}

STATUS  drv_board_manu_info_get
(
DRV_PHYBRD_INTEGRATED_INFO_MANU_T *pstManuInfo
)
{
    return DRV_BSP_OK;
}

INT32  drv_logic_chip_filever_get
(
  UINT8  ucChipNo,
UINT32 *pulLogicFileVer
)
{
    return DRV_BSP_OK;
}

#if 0
INT32 cpss_file_stat
(
const CHAR *pcFileName,
CPSS_FILE_STAT_T *pstStat
)
{
	return CPSS_OK;
}
#endif

/*************************************/
UINT32 smss_verm_set_upgstatus(UINT32 ulStatus)
{
	return 0;
}

VOID smss_verm_set_gcpa_cps__rdbs_verstate(CPSS_COM_PHY_ADDR_T stPhy, UINT16 usVerState)
{
	return;
}

UINT32 smss_get_gcpa_upgstatus(VOID)
{
	return 0;
}

VOID smss_devm_SetISled(UINT32 ulFlag)
{
	return;
}

VOID smss_devm_SetActled(UINT32 ulFlag)
{
	return;
}

VOID smss_devm_send_active_port()
{
	return;
}
