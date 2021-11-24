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
#ifndef SWP_COMMON_AC_H
#define SWP_COMMON_AC_H
#include "swp_type.h"

/* CPS纤程描述符定义 */
/* CPSS 1~20 */
#define	CPSS_COM_LINK_MNGR_PROC			CPSS_VK_PD(1, 0)		/*通信管理纤程*/
#define	CPSS_FS_FTP_LOW_PROC			CPSS_VK_PD(2, 0)		/*FTP/TFTP低优先级处理纤程*/
#define	CPSS_FS_FTP_HIGH_PROC			CPSS_VK_PD(3, 0)		/*FTP/TFTP高优先级处理纤程*/
#define	CPSS_DBG_MNGR_PROC				CPSS_VK_PD(4, 0)		/*调试管理纤程*/
#define	CPSS_TM_SNTP_PROC				CPSS_VK_PD(5, 0)		/*SNTP处理纤程*/
#define	CPSS_COM_SLID_PROC				CPSS_VK_PD(6, 0)		/*可靠传输管理纤程*/

/* CPS纤程名称定义 */
/* CPSS */
#define	CPSS_COM_LINK_MNGR_PROCNAME		"pCpssLink "				/*通信管理纤程*/
#define	CPSS_FS_FTP_LOW_PROCNAME		"pCpssFtpL "				/*FTP/TFTP低优先级处理纤程*/
#define	CPSS_FS_FTP_HIGH_PROCNAME		"pCpssFtpH "				/*FTP/TFTP高优先级处理纤程*/
#define	CPSS_DBG_MNGR_PROCNAME			"pCpssDbg "				/*调试管理纤程*/
#define	CPSS_TM_SNTP_PROCNAME			"pCpssSntp "				/*SNTP处理纤程*/
#define	CPSS_COM_SLID_PROCNAME			"pCpssSlid "				/*可靠传输管理纤程*/

/* CPS纤程入口函数声明 */
/* 链路纤程 */
extern void cpss_com_link_proc(UINT16  usUserState, VOID *  pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* 滑窗纤程 */
extern void cpss_com_slid_proc(UINT16  usUserState, VOID *  pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* 调试管理纤程 */
extern void cpss_debug_proc(UINT16  usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* SNTP纤程 */
extern void cpss_tm_sntp_proc(UINT16 usState, void *pVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* FTP纤程 */
extern void tnbs_ftp_proc(UINT16  usState, void *pVar,CPSS_COM_MSG_HEAD_T *ptHdr);

/* SMSS纤程描述符定义 */
/* SMSS 21~40 */
#define	SMSS_SYSCTL_PROC			CPSS_VK_PD(21, 0)		/*系统主控纤程*/
#define	SMSS_VERM_PROC				CPSS_VK_PD(22, 0)		/*版本管理纤程*/
#define	SMSS_DEVM_PROC				CPSS_VK_PD(23, 0)		/*设备监控纤程*/
#define SMSS_SYSRT_PROC				CPSS_VK_PD(25, 0)		/*系统实时纤程*/

/* SMSS纤程名称定义 */
/* SMSS */
#define	SMSS_SYSCTL_PROCNAME		"pSmssSysctl "			/*系统主控纤程*/
#define	SMSS_VERM_PROCNAME			"pSmssVerm "				/*版本管理纤程*/
#define	SMSS_DEVM_PROCNAME			"pSmssDevm "				/*设备监控纤程*/
#define SMSS_SYSRT_PROCNAME			"pSmssSysrt "			/*系统实时纤程*/

/* SMSS纤程入口函数声明 */
/*系统主控纤程*/
extern VOID smss_sysctl_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/*版本管理纤程*/
extern VOID smss_verm_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/*设备监控纤程*/
extern VOID smss_devm_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/*系统实时纤程*/
extern VOID smss_sysrt_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

/* RDBS纤程描述符定义 */
/* RDBS 41~60 */
#if 0 //dhwang added
#define RDBS_INIT_PROC				CPSS_VK_PD(41, 0)		/*RDBS初始化纤程*/
#define RDBS_IF_PROC				CPSS_VK_PD(42, 0)		/*接口纤程*/
#define RDBS_DCM_PROC				CPSS_VK_PD(43, 0)		/*配置管理纤程*/
#define RDBS_FILE_PROC				CPSS_VK_PD(52, 0)		/*文件纤程*/
#define RDBS_VERIFY_PROC			CPSS_VK_PD(53, 0)		/*校验纤程*/

/* RDBS纤程名称定义 */
/* RDBS */
#define RDBS_INIT_PROCNAME			"pRdbsInit "				/*RDBS初始化纤程*/
#define RDBS_IF_PROCNAME			"pRdbsIf "				/*接口纤程*/
#define RDBS_DCM_PROCNAME			"pRdbsDcm "				/*配置管理纤程*/
#define RDBS_FILE_PROCNAME			"pRdbsFile "				/*文件纤程*/
#define RDBS_VERIFY_PROCNAME		"pRdbsVerify "			/*校验纤程*/

/* RDBS纤程入口函数声明 */
/*RDBS初始化纤程*/
extern VOID rdbs_init_proc(UINT16 usUserState, void *pvVar,	CPSS_COM_MSG_HEAD_T *pstMsgHead);
/*接口纤程*/
extern VOID rdbs_if_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/*配置管理纤程*/
extern VOID rdbs_dcm_proc(UINT16 usUserState, VOID* pvVar,	CPSS_COM_MSG_HEAD_T* pstMsgHead);
/*文件纤程*/
extern VOID rdbs_file_proc(UINT16 usUserState, VOID *pvVar,	CPSS_COM_MSG_HEAD_T *pstMsgHead);
/*校验纤程*/
extern VOID rdbs_verify_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
#endif
/* OAMS纤程描述符定义 */
/* OAMS 61~90 */
#if 0 //dhwang added
#define OAMS_PD_M_AM_MNGR_RCV_PROC			CPSS_VK_PD(61, 0)		/* 告警管理主控接收纤程 */
#define OAMS_PD_M_AM_MNGR_RPT_PROC			CPSS_VK_PD(62, 0)		/* 告警管理主控上报纤程 */
#define OAMS_PD_M_LOGM_PROC				    CPSS_VK_PD(65, 0)		/* 日志管理 */
#define OAMS_PD_M_PM_MNGR_PROC				CPSS_VK_PD(67, 0)		/* 性能管理 */
#define OAMS_PD_A_AM_AGT_PROC				CPSS_VK_PD(77, 0)		/* 告警管理代理纤程 */
#define OAMS_PD_M_OM_IF_PROC				CPSS_VK_PD(86, 0)		/* 接口模块纤程 */
#define OAMS_PD_A_AM_AGT_NOTIFY_PROC		CPSS_VK_PD(87, 0)		/* 事件代理模块纤程 */

/* OAMS纤程名称定义 */
/* OAMS */
#define OAMS_PD_M_AM_MNGR_RCV_PROCNAME		"pOamsAmMngrRecv "		/* 告警管理主控接收纤程 */
#define OAMS_PD_M_AM_MNGR_RPT_PROCNAME		"pOamsAmMngrRpt "		/* 告警管理主控上报纤程 */
#define OAMS_PD_M_LOGM_PROCNAME				"pOamsLogm "				/* 日志管理纤程 */
#define OAMS_PD_A_PM_MNGR_PROCNAME			"pOmPmMngr "				/* 性能管理纤程 */
#define OAMS_PD_A_AM_AGT_PROCNAME			"pOamsAmAgt "			/* 告警管理代理纤程 */
#define OAMS_PD_M_OM_IF_PROCNAME			"pOmIf "					/* 接口模块纤程 */
#define OAMS_PD_A_AM_AGT_NOTIFY_PROCNAME    "pOamsAmAgtNotify "		/* 事件代理模块纤程 */


/* OAMS纤程入口函数声明 */
/* 告警管理主控接收纤程 */
extern void oams_am_mngr_rcv_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 告警管理主控上报纤程 */
extern void oams_am_mngr_rpt_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 告警管理代理纤程 */
extern void oams_am_agt_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 事件代理模块纤程 */
extern void oams_am_agt_notify_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 接口模块纤程 */
extern void om_if_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgBuff);
/* 性能管理纤程 */
extern void om_pm_proc(UINT32 vulStatus,VOID *pvVar, CPSS_COM_MSG_HEAD_T *vpstMsgHead);
/* 日志管理纤程 */
extern void oams_logm_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
#endif
/* CPM纤程描述符定义 */
/* CPM 91~130 */
#if 0//dhwang added
#define AC_CAPWAP_PROC					CPSS_VK_PD(91, 0)		/*CAPWAP控制面协议模块*/
#define AC_8021X_PROC					CPSS_VK_PD(92, 0)		/*802.11i认证模块*/
#define AC_CHARGE_PROC					CPSS_VK_PD(93, 0)		/*计费模块*/
#define AC_WAPI_PROC					CPSS_VK_PD(94, 0)		/*WAPI认证模块*/
#define AC_RADIUS_PROC					CPSS_VK_PD(95, 0)		/*RADIUS协议模块*/
#define AC_PORTAL_PROC					CPSS_VK_PD(96, 0)		/*Portal协议模块*/
#define AC_PPPOE_PROC					CPSS_VK_PD(97, 0)		/*PPPoE协议模块*/
#define AC_CPMCC_PROC					CPSS_VK_PD(98, 0)		/*主控模块*/
#define AC_DHCP_PROC					CPSS_VK_PD(99, 0)		/*DHCP协议模块*/
#define AC_MSCC_PROC					CPSS_VK_PD(100, 0)
#endif
/*20110314 Mod by huxp for mutitest*/
#define MT_MAIN_PROC                    CPSS_VK_PD(101, 0)

/* CPM纤程名称定义 */
/* CPM */
#if 0 //dhwang added
#define AC_CAPWAP_PROCNAME				"pPAcCapwap "			/*CAPWAP控制面协议模块*/
#define	AC_8021X_PROCNAME				"pAc8021x "				/*802.11i认证模块*/
#define	AC_CHARGE_PROCNAME				"pAcCharge "				/*计费模块*/
#define	AC_WAPI_PROCNAME				"pAcWapi "				/*WAPI认证模块*/
#define	AC_RADIUS_PROCNAME				"pAcRadius "				/*RADIUS协议模块*/
#define	AC_PORTAL_PROCNAME				"pAcPortal "				/*Portal协议模块*/
#define	AC_PPPOE_PROCNAME				"pAcPppoe "				/*PPPoE协议模块*/
#define AC_CPMCC_PROCNAME				"pAcCpmcc "				/*主控模块*/
#define	AC_DHCP_PROCNAME				"pAcDhcp "				/*DHCP协议模块*/
#define	AC_MSCC_PROCNAME				"pAcMscc "				/*MSCC模块*/
#endif
/*20110314 Mod by huxp for mutitest*/
#define	MT_MAIN_PROCNAME				"pMutiTest "				/*MUTITEST模块*/

/* CPM纤程入口函数声明 */
/*CAPWAP控制面协议模块*/
#if 0//dhwang added
extern void ac_capwap_proc(UINT16 vusUserState, void *vpVar, CPSS_COM_MSG_HEAD_T *vpstMsgHead);
/*802.11i认证模块*/
extern void ac_8021x_proc(UINT16 vusUserState, void *vpVar, CPSS_COM_MSG_HEAD_T *vpstMsgHead);
/*计费模块*/
extern void ac_charge_proc(UINT16 vusUserState, void *vpVar, CPSS_COM_MSG_HEAD_T *vpstMsgHead);
/*WAPI认证模块*/
extern void ac_wapi_proc(UINT16 vusUserState, void *vpVar, CPSS_COM_MSG_HEAD_T *vpstMsgHead);
/*RADIUS协议模块*/
extern void ac_radius_proc(UINT16 vusUserState, void *vpVar, CPSS_COM_MSG_HEAD_T *vpstMsgHead);
/*Portal协议模块*/
extern void ac_portal_proc(UINT16 vusUserState, void *vpVar, CPSS_COM_MSG_HEAD_T *vpstMsgHead);
/*PPPoE协议模块*/
extern void ac_pppoe_proc(UINT16 vusUserState, void *vpVar, CPSS_COM_MSG_HEAD_T *vpstMsgHead);
/*主控模块*/
extern void ac_cpmcc_proc(UINT16 vusUserState, void *vpVar, CPSS_COM_MSG_HEAD_T *vpstMsgHead);
/*DHCP协议模块*/
//extern void ac_dhcp_proc(UINT16 vusUserState, void *vpVar, CPSS_COM_MSG_HEAD_T *vpstMsgHead);
extern void ac_mscc_proc(UINT16	 vusUserState, void *vpVar,CPSS_COM_MSG_HEAD_T *pstMsgHead);
#endif

/*20110314 Mod by huxp for Mutitest*/
extern void MT_Main_Proc(UINT16 vusUserState, void *vpVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);


/* UPM纤程描述符定义 */
/* UPM 131~160 */
#if 0//dhwang added
#define UPM_MAIN_CONTROL_PROC			CPSS_VK_PD(131, 0)		/* UPM主控纤程  */
#define UPM_CAPWAPDATA_PROC				CPSS_VK_PD(132, 0)		/* 用户面数据分发纤程*/
#define TNBS_IPSTACK_PROC				CPSS_VK_PD(133, 0)		/* IP协议栈纤程 */
#define UPM_DHCPRELAY_PROC				CPSS_VK_PD(134, 0)		/* IP协议栈纤程 */

/* UPM纤程名称定义 */
/* UPM */
#define UPM_CCM_PROCNAME				"pUpmCcm "				/* UPM主控纤程  */
#define UPM_DPM_PROCNAME				"pUpmCapWapData "		/* 用户面数据分发纤程*/
#define TNBS_IPSTACK_PROCNAME			"pTnbsIpStack "			/* IP协议栈纤程 */
#define UPM_DHCPRELAY_PROCNAME  		"pUpmDhcpRelay "		/* DhcpRelay纤程*/

/* UPM纤程入口函数声明 */
/* UPM主控纤程  */
extern void upm_main_control_proc(UINT16 ulState, VOID *pVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* 用户面数据分发纤程*/
extern void upm_capwadata_proc(UINT16 usState, void* pVar, CPSS_COM_MSG_HEAD_T* pstMsgHead);
/* IP协议栈纤程 */
extern void tnbs_ipstack_proc(UINT16 ulState, VOID *pVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern void upm_dhcprelay_proc(UINT16 ulState, VOID   *pVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
#if 0
#define UPM_CCM_TEST_PROCNAME       "pUpmCcmTest"
#define UPM_CCM_TEST_PROC                CPSS_VK_PD(160,0) /* ccm test */
extern void upm_ccm_test_proc(UINT16 ulState,VOID   *pVar,CPSS_COM_MSG_HEAD_T *pstMsgHead);
#endif
#endif

#endif //dhwang added
