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
#include "swp_config.h"
#include "swp_public.h"
#include "cpss_public.h"
#include "smss_public.h"

/* (CPSS)普通任务配置表，目前只允许CPSS有普通任务 */

VK_TASK_CONFIG_T gatVkTaskConfig[] =
{
		//任务名称	优先级	选项		堆栈大小		入口函数		参数		坚守时长
    {CPSS_TIMER_TASKNAME,          CPSS_TIMER_TASKPRI,        0,      0,    cpss_vos_ntask_entry,           1,      TRUE ,       0,          SWP_HA_TIME_TYPE_3S },
    {CPSS_COM_RECV1_TASKNAME,      CPSS_COM_RECV_TASKPRI,     0,      0,    cpss_reg_task_entry,            2,      TRUE ,       0,          SWP_HA_TIME_TYPE_3S },
    {CPSS_COM_IRQ_TASKNAME,        CPSS_IRQ_TASKPRI,          0,      0,    cpss_vos_ntask_entry,          15,      TRUE ,       0,          SWP_HA_TIME_TYPE_3S },                                                                                                                                                                                                                                           
/*  {CPSS_HAMONITOR_TASKNAME,      CPSS_HAMONITOR_TASKPRI,    0,      0,    cpss_kw_task,                   4,      TRUE ,       0,          SWP_HA_TIME_TYPE_3S },
    {CPSS_IDLE_TASKNAME,           SWP_TASKPRI_BELOW_NORMAL,  0,      0,    cpss_cpu_load_task,             5,      TRUE ,       0,          SWP_HA_TIME_TYPE_2M }, */
    {CPSS_UDPRECV_TASKNAME,        CPSS_UDPRECV_TASKPRI,      0,      0,    cpss_vos_ntask_entry,           6,      TRUE ,       0,          SWP_HA_TIME_TYPE_3S },                                                                                                                                                                                                                                           
    {CPSS_SLIDTIMER_TASKNAME,      CPSS_SLIDTIMER_TASKPRI,    0,      0,    cpss_vos_ntask_entry,           7,      TRUE ,       0,          SWP_HA_TIME_TYPE_3S },                                                                                                                                                                                                                                           
    {CPSS_TCPLISTEN_TASKNAME,      CPSS_TCPLISTEN_TASKPRI,    0,      0,    cpss_vos_ntask_entry,           8,      TRUE ,       0,          SWP_HA_TIME_TYPE_3S },                                                                                                                                                                                                                                           
    {CPSS_DC_LISTEN_TASKNAME,      CPSS_TCPLISTEN_TASKPRI,    0,      0,    cpss_vos_ntask_entry,          16,      TRUE ,       0,          SWP_HA_TIME_TYPE_3S },                                                                                                                                                                                                                                           
    {CPSS_TCPRECV_TASKNAME,        CPSS_TCPRECV_TASKPRI,      0,      0,    cpss_vos_ntask_entry,           9,      TRUE ,       0,          SWP_HA_TIME_TYPE_3S },
    {DRV_TEMALARMSCAN_TASKNAME,    DRV_TEMALARMSCAN_TASKPRIO, 0,      0,   cpss_vos_ntask_entry,          17,      TRUE ,       0,          SWP_HA_TIME_TYPE_3S },
    {CPSS_NULL_STRING,             CPSS_NULL_TASKPRI,         0,      0,    NULL,                           0,      FALSE,       0,          SWP_HA_TIME_TYPE_3S }
};

/* (CPSS)调度任务配置表
*   SWP_SCHED_M_TASKNAME    主调度任务，各纤程都配置在该调度任务内。
*   SWP_SCHED_D_TASKNAME    打印控制调度任务，目前主要配置了CPSS的调试管理纤程。
*   SWP_SCHED_N_TASKNAME    SNTP调度任务，网元时间同步和单板时间同步纤程都配置在该调度任务内。
*   SWP_SCHED_S_TASKNAME    通信可靠传输调度任务，只能配置CPSS的可靠传输纤程。
*   SWP_SCHED_FL_TASKNAME   FTP/TFTP低优先级调度任务，处理操作时间较长的FTP/TFTP消息。
*   SWP_SCHED_FH_TASKNAME   FTP/TFTP高优先级调度任务，处理操作时间较短的FTP/TFTP消息。
*/

VK_SCHED_CONFIG_T gatVkSchedConfig[] =
{
    {  1,                          SWP_SCHED_M_TASKNAME,      SWP_SCHED_S_TASKPRI,    0,  0x200000,   cpss_vk_sched,    TRUE  ,       0 },
    {  2,                          SWP_SCHED_D_TASKNAME,      SWP_SCHED_D_TASKPRI,    0,      0,      cpss_vk_sched,    TRUE  ,       0 },
    {  3,                          SWP_SCHED_N_TASKNAME,      SWP_SCHED_N_TASKPRI,    0,      0,      cpss_vk_sched,    TRUE  ,       0 },
    {  4,                          SWP_SCHED_S_TASKNAME,      SWP_SCHED_S_TASKPRI,    0,      0,      cpss_vk_sched,    TRUE  ,       0 },
    {  5,                          SWP_SCHED_FL_TASKNAME,     SWP_SCHED_FL_TASKPRI,   0,      0,      cpss_vk_sched,    TRUE  ,       0 },
    {  6,                          SWP_SCHED_FH_TASKNAME,     SWP_SCHED_FH_TASKPRI,   0,      0,      cpss_vk_sched,    TRUE  ,       0 },
    {  7,                          SWP_SCHED_FIO_TASKNAME,    SWP_SCHED_FIO_TASKPRI,  0,      0,      cpss_vk_sched,    TRUE  ,       0 },
    {  VK_SCHED_DESC_INVALID,      CPSS_NULL_STRING,          0,                      0,      0,      NULL,             FALSE ,       0 }
};

/* 纤程配置表 */

CPSS_VK_PROC_CONFIG_T gatVkProcClassConfig[] =
{
	/*平台部分*/
	{CPSS_COM_LINK_MNGR_PROC,		CPSS_COM_LINK_MNGR_PROCNAME,		0,	0,	0,	0,		(VOID_FUNC_PTR)cpss_com_link_proc,		1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{CPSS_DBG_MNGR_PROC,			CPSS_DBG_MNGR_PROCNAME,				0,	0,	0,	0,		(VOID_FUNC_PTR)cpss_debug_proc,			3,        1,         TRUE ,			SWP_HA_TIME_TYPE_2M },
	{CPSS_COM_SLID_PROC,			CPSS_COM_SLID_PROCNAME,				0,	0,	0,	0,		(VOID_FUNC_PTR)cpss_com_slid_proc,		4,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{CPSS_FS_FTP_LOW_PROC,			CPSS_FS_FTP_LOW_PROCNAME,			0,	0,	0,	0,		(VOID_FUNC_PTR)cpss_ftp_proc,			5,        1,         TRUE ,			SWP_HA_TIME_TYPE_30M},
//	{CPSS_FS_FTP_HIGH_PROC,			CPSS_FTP_H_PROCNAME,				0,	0,	0,	0,		(VOID_FUNC_PTR)cpss_ftp_proc,			6,        1,         TRUE ,			SWP_HA_TIME_TYPE_30M},
	{CPSS_TM_SNTP_PROC,				CPSS_TM_SNTP_PROCNAME,				0,	0,	0,	0,		(VOID_FUNC_PTR)cpss_tm_sntp_proc,		3,        1,         TRUE ,			SWP_HA_TIME_TYPE_2M },
//	{CPSS_COM_TCP_PROC,				CPSS_COM_TCP_PROCNAME,				0,	0,	0,	0,		(VOID_FUNC_PTR)cpss_com_tcp_proc,		1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{SMSS_SYSCTL_PROC,				SMSS_SYSCTL_PROCNAME,				0,	0,	0,	0,		(VOID_FUNC_PTR)smss_sysctl_proc,		1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
//	{SMSS_VERM_PROC,				SMSS_VERM_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)smss_verm_proc,			6,        1,         TRUE ,			SWP_HA_TIME_TYPE_30M },
//	{SMSS_DEVM_PROC,				SMSS_DEVM_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)cps_devm_proc,			3,        1,         TRUE ,			SWP_HA_TIME_TYPE_2M },
	{SMSS_SYSRT_PROC,				SMSS_SYSRT_PROCNAME,				0,	0,	0,	0,		(VOID_FUNC_PTR)smss_sysrt_proc,			4,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
//	{SMSS_FIRMWARE_PROC,			SMSS_FIRMWARE_PROCNAME,				0,	0,	0,	0,		(VOID_FUNC_PTR)smss_firmware_proc,		5,        1,         TRUE ,			SWP_HA_TIME_TYPE_30M},

	/*20110314 add for mutitest*/
	{MT_MAIN_PROC,					MT_MAIN_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)MT_Main_Proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_30M },
#if 0
	/*rdbs部分*/
	{RDBS_FILE_PROC,				RDBS_FILE_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)rdbs_file_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{RDBS_VERIFY_PROC,				RDBS_VERIFY_PROCNAME,				0,	0,	0,	0,		(VOID_FUNC_PTR)rdbs_verify_proc,		6,        1,         TRUE ,			SWP_HA_TIME_TYPE_30M},
	{RDBS_INIT_PROC,				RDBS_INIT_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)rdbs_init_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{RDBS_IF_PROC,					RDBS_IF_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)rdbs_if_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{RDBS_DCM_PROC,					RDBS_DCM_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)rdbs_dcm_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_30M},
#endif
	/*om部分*/
#if 0 //dhwang added
	{OAMS_PD_M_OM_IF_PROC,			OAMS_PD_M_TM_PROCNAME,				0,	0,	0,	0,		(VOID_FUNC_PTR)om_if_proc,				1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{OAMS_PD_M_AM_MNGR_RCV_PROC,	OAMS_PD_M_AM_MNGR_RCV_PROCNAME,		0,	0,	0,	0,		(VOID_FUNC_PTR)oams_am_mngr_rcv_proc,	1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{OAMS_PD_M_AM_MNGR_RPT_PROC,	OAMS_PD_M_AM_MNGR_RPT_PROCNAME,		0,	0,	0,	0,		(VOID_FUNC_PTR)oams_am_mngr_rpt_proc,	1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{OAMS_PD_A_AM_AGT_PROC,			OAMS_PD_A_AM_AGT_PROCNAME,			0,	0,	0,	0,		(VOID_FUNC_PTR)oams_am_agt_proc,		1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{OAMS_PD_A_AM_AGT_NOTIFY_PROC,	OAMS_PD_A_AM_AGT_NOTIFY_PROCNAME,	0,	0,	0,	0,		(VOID_FUNC_PTR)oams_am_agt_notify_proc,	1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{OAMS_PD_M_PM_MNGR_PROC,        OAMS_PD_A_PM_MNGR_PROCNAME,			0,	0,  0,	0,  	(VOID_FUNC_PTR)om_pm_proc,				1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{OAMS_PD_M_LOGM_PROC,           OAMS_PD_M_LOGM_PROCNAME,			0,	0,  0,	0,  	(VOID_FUNC_PTR)oams_logm_proc,			6,        1,		 TRUE ,			SWP_HA_TIME_TYPE_30M },
#endif
	/*cpm部分*/
#if 0//dhwang added
	{AC_CPMCC_PROC,					AC_CPMCC_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)ac_cpmcc_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{AC_8021X_PROC,					AC_8021X_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)ac_8021x_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{AC_CAPWAP_PROC,				AC_CAPWAP_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)ac_capwap_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{AC_PORTAL_PROC,				AC_PORTAL_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)ac_portal_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{AC_RADIUS_PROC,				AC_RADIUS_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)ac_radius_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
//	{AC_DHCP_PROC,					AC_DHCP_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)ac_dhcp_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{AC_WAPI_PROC,					AC_WAPI_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)ac_wapi_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{AC_CHARGE_PROC,				AC_CHARGE_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)ac_charge_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{AC_PPPOE_PROC,					AC_PPPOE_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)ac_pppoe_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
	{AC_MSCC_PROC,					AC_MSCC_PROCNAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)ac_mscc_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },
#endif
	/*upm部分*/
#if 0//dhwang added
//	{UPM_CCM_TEST_PROC,             UPM_CCM_TEST_PROCNAME,		       	0,  0,  0,  0,		(VOID_FUNC_PTR)upm_ccm_test_proc,		2,        1,         TRUE ,      	SWP_HA_TIME_TYPE_3S },
	{UPM_MAIN_CONTROL_PROC,         UPM_CCM_PROCNAME,        		   	0,  0,  0,  0,		(VOID_FUNC_PTR)upm_main_control_proc,	2,        1,         TRUE ,      	SWP_HA_TIME_TYPE_3S },
	{TNBS_IPSTACK_PROC,             TNBS_IPSTACK_PROCNAME,             	0,  0,  0,  0,		(VOID_FUNC_PTR)tnbs_ipstack_proc,		2,        1,         TRUE ,       	SWP_HA_TIME_TYPE_3S },
	{UPM_CAPWAPDATA_PROC,           UPM_DPM_PROCNAME,                  	0,  0,  0,  0,		(VOID_FUNC_PTR)upm_capwadata_proc,		2,        1,         TRUE ,       	SWP_HA_TIME_TYPE_3S },
//	{UPM_DHCPRELAY_PROC,            UPM_DHCPRELAY_PROCNAME,            	0,  0,  0,  0,		(VOID_FUNC_PTR)upm_dhcprelay_proc,		2,        1,         TRUE ,       	SWP_HA_TIME_TYPE_3S },

//	{COM_TEST_PROC,					COM_TEST_PROC_NAME,					0,	0,	0,	0,		(VOID_FUNC_PTR)com_test_proc,			1,        1,         TRUE ,			SWP_HA_TIME_TYPE_3S },

#endif
    {CPSS_VK_PD_INVALID,			CPSS_NULL_STRING,					0,	0,	0,	0,		(VOID_FUNC_PTR)NULL,					0,        0,         FALSE,			SWP_HA_TIME_TYPE_3S },
};

/* (SMSS)纤程激活配置 */
/* 需要激活的纤程在这里添加 */

SMSS_PROC_TABLE_ITEM_T g_astSmssProcTable[] =
{
	/*平台部分*/
	{CPSS_COM_LINK_MNGR_PROC,			SWP_ACT_TIME_TYPE_3S},
//	{CPSS_TM_SNTP_PROC,					SWP_ACT_TIME_TYPE_2M},
	{SMSS_SYSRT_PROC,					SWP_ACT_TIME_TYPE_20S},
	{CPSS_FS_FTP_LOW_PROC,				SWP_ACT_TIME_TYPE_ASY},
//	{CPSS_FS_FTP_HIGH_PROC,				SWP_ACT_TIME_TYPE_ASY},
	{CPSS_DBG_MNGR_PROC,				SWP_ACT_TIME_TYPE_3S},
//	{CPSS_COM_TCP_PROC,					SWP_ACT_TIME_TYPE_3S},

	/*20110314 Mod by huxp for mutitest*/
	{MT_MAIN_PROC,					SWP_ACT_TIME_TYPE_30M},

/*	rdbs部分
	{RDBS_FILE_PROC,					SWP_HA_TIME_TYPE_3S},
	{RDBS_VERIFY_PROC,					SWP_HA_TIME_TYPE_30M},
	{RDBS_INIT_PROC,					SWP_ACT_TIME_TYPE_20S},
	{RDBS_IF_PROC,						SWP_HA_TIME_TYPE_3S},
	{RDBS_DCM_PROC,						SWP_HA_TIME_TYPE_30M},

	{CPSS_TM_SNTP_PROC,					SWP_HA_TIME_TYPE_3S},
	{SMSS_VERM_PROC,					SWP_ACT_TIME_TYPE_30M},

	om部分
	{OAMS_PD_M_OM_IF_PROC,				SWP_ACT_TIME_TYPE_3S},网管接口管理
	{OAMS_PD_M_AM_MNGR_RCV_PROC,		SWP_ACT_TIME_TYPE_3S},
	{OAMS_PD_M_AM_MNGR_RPT_PROC,		SWP_ACT_TIME_TYPE_3S},
	{OAMS_PD_A_AM_AGT_PROC,				SWP_ACT_TIME_TYPE_3S},
	{OAMS_PD_A_AM_AGT_NOTIFY_PROC,		SWP_ACT_TIME_TYPE_3S},
	{OAMS_PD_M_PM_MNGR_PROC,			SWP_HA_TIME_TYPE_3S},
	{OAMS_PD_M_LOGM_PROC,				SWP_HA_TIME_TYPE_30M},

	upm部分
//	{UPM_CCM_TEST_PROC,              SWP_ACT_TIME_TYPE_3S},
	{UPM_MAIN_CONTROL_PROC,          SWP_ACT_TIME_TYPE_2M},
	{UPM_CAPWAPDATA_PROC,            SWP_ACT_TIME_TYPE_3S},
//	{UPM_DHCPRELAY_PROC,             SWP_ACT_TIME_TYPE_3S},
	{TNBS_IPSTACK_PROC,              SWP_ACT_TIME_TYPE_3S},

	cpm部分
	{AC_CPMCC_PROC,						SWP_ACT_TIME_TYPE_3S},
	{AC_CAPWAP_PROC,					SWP_ACT_TIME_TYPE_3S},
	{AC_8021X_PROC,						SWP_ACT_TIME_TYPE_3S},
	{AC_PORTAL_PROC,					SWP_ACT_TIME_TYPE_3S},
	##########
	{AC_WAPI_PROC,						SWP_ACT_TIME_TYPE_3S},
	##########
	{AC_CHARGE_PROC,					SWP_ACT_TIME_TYPE_3S},
	{AC_PPPOE_PROC,						SWP_ACT_TIME_TYPE_3S},
	{AC_RADIUS_PROC,					SWP_ACT_TIME_TYPE_3S},
	//{AC_DHCP_PROC,						SWP_ACT_TIME_TYPE_3S},
	{AC_MSCC_PROC,						SWP_ACT_TIME_TYPE_3S},
*/

//	{COM_TEST_PROC,						SWP_ACT_TIME_TYPE_3S},
	{CPSS_VK_PD_INVALID,				0},
};

/*****内存配置表*****/

CPSS_MEM_CONFIG_T stMemConfigTbl[]=
{
		{64, 1000},
		{0,  0},
};  

UINT32 g_ulCpssMemConfigNum = NUM_ENTS(stMemConfigTbl);

extern INT32 swp_usr_init();
                                                                                             
extern INT32 cpss_vos_task_delay(INT32 lMs) ;

INT32 swp_get_ha_time(INT32 lType)
{
    INT32 lTimer;
    switch(lType)
    {
        case SWP_HA_TIME_TYPE_3S:
            lTimer = 3000;
            break;
        case SWP_HA_TIME_TYPE_2M:
            lTimer = 2*60*1000;
            break;
        case SWP_HA_TIME_TYPE_30M:
            lTimer = 30*60*1000;
            break;  
        default:
            lTimer = 30*60*1000;
            break;
    }
    return lTimer;
       
}
INT32 swp_get_act_proc_time(INT32 lType)
{
    INT32 lTimer;
     switch(lType)
    {
        case SWP_ACT_TIME_TYPE_3S:
            lTimer = 3000;
            break;
        case SWP_ACT_TIME_TYPE_20S:
            lTimer = 20*1000;
            break;
        case SWP_ACT_TIME_TYPE_2M:
            lTimer = 2*60*1000;
            break;   
        case SWP_ACT_TIME_TYPE_30M:
            lTimer = 30*60*1000;
            break;   
        case SWP_ACT_TIME_TYPE_ASY:
            lTimer = 0;
            break;      
        default:
            lTimer = 30*60*1000;
            break;
    }   
    return lTimer;
}

void wlan_init(int argc, char * argv[])
{   
/*	memcpy(g_RuningVerFile,(char *)argv[1],sizeof(g_RuningVerFile));
	printf("************running file is %s!\n",g_RuningVerFile);*/

    cpss_init();
    cpss_proc_activate();
}


