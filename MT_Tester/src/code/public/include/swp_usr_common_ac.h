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
#ifndef SWP_USR_COMMON_AC_H_
#define SWP_USR_COMMON_AC_H_

#include "cpss_public.h"

/*add for AC begin */
#define SWP_SUBSYS_AC         15    /* AC子系统 */
/*add for AC end */
#if 0
#define AC_CAPWAP_PROC                 CPSS_VK_PD(167,0)
#define AC_8021X_PROC                  CPSS_VK_PD(168,0)
#define AC_CHARGE_PROC                 CPSS_VK_PD(169,0)
#define AC_WAPI_PROC                   CPSS_VK_PD(170,0)
#define AC_RADIUS_PROC                 CPSS_VK_PD(171,0)
#define AC_PORTAL_PROC                 CPSS_VK_PD(172,0)
#define AC_DTLS_PROC                   CPSS_VK_PD(173,0)
#define AC_CAPWAP_DATA_PROC            CPSS_VK_PD(174,0)
#define AC_RDBSAC_PROC                 CPSS_VK_PD(175,0)
#define AC_PPPOE_PROC                  CPSS_VK_PD(176,0)
#define WTP_CAPWAP_PROC                CPSS_VK_PD(177,0)
#define AC_CPMCC_PROC                  CPSS_VK_PD(178,0)
#define AC_DHCP_PROC                   CPSS_VK_PD(179,0)

#define OM_PM_PROC                     CPSS_VK_PD(180,0)

#define AC_CAPWAP_PROCNAME              "pPAcCapwap"
#define	AC_8021X_PROCNAME               "pAc8021x"
#define	AC_CHARGE_PROCNAME              "pAcCharge"
#define	AC_WAPI_PROCNAME                "pAcWapi"
#define	AC_RADIUS_PROCNAME              "pAcRadius"
#define	AC_PORTAL_PROCNAME              "pAcPortal"
#define	AC_DTLS_PROCNAME                "pAcDtls"
#define	AC_CAPWAP_DATA_PROCNAME         "pAcCapwapData"
#define	AC_RDBSAC_PROCNAME              "pAcRdbsac"
#define	AC_PPPOE_PROCNAME               "pAcPppoe"
#define AC_CPMCC_PROCNAME               "pAcCpmcc"
#define	AC_DHCP_PROCNAME                "pAcDhcp"
#endif

#define  AC_MODULE_CPMCC	    ((UINT8)(0x01))
#define  AC_MODULE_APM    		((UINT8)(0x02))
#define  AC_MODULE_STAM   		((UINT8)(0x03))
#define  AC_MODULE_DHCP   		((UINT8)(0x04))
#define  AC_MODULE_RADIUS    	((UINT8)(0x05))
#define  AC_MODULE_WAPI    		((UINT8)(0x06))
#define  AC_MODULE_8021X    	((UINT8)(0x07))
#define  AC_MODULE_PPPOE    	((UINT8)(0x08))
#define  AC_MODULE_PORTAL    	((UINT8)(0x09))
#define  AC_MODULE_CAPWAP    	((UINT8)(0x0a))
#define  AC_MODULE_CHARGE    	((UINT8)(0x0b))
#define  AC_MODULE_PUB    		((UINT8)(0x0c))
#define  AC_MODULE_DTLS    		((UINT8)(0x0d))
#define	 TNBS_MSCC_MODULE	    ((UINT8)(0x0e))

#define  AC_MODULENAME_CAPWAP          "CAPWAP"
#define  AC_MODULENAME_8021X           "8021X"
#define  AC_MODULENAME_CHARGE          "CHARGE"
#define  AC_MODULENAME_WAPI            "WAPI"
#define  AC_MODULENAME_RADIUS          "RADIUS"
#define  AC_MODULENAME_PORTAL          "PORTAL"
#define  AC_MODULENAME_DTLS            "ACDTLS"
#define  AC_MODULENAME_PUB             "PUB"
#define  AC_MODULENAME_CAPWAPDATA      "CAPWAPDATA"
#define  AC_MODULENAME_RDBSAC          "RDBSAC"
#define	 AC_MODULENAME_PPPOE		"PPPOE"
#define AC_MODULENAME_CPMCC		 	"CPMCC"
#define AC_MODULENAME_AP			"APM"
#define AC_MODULENAME_STAM			"STAM"
#define AC_MODULENAME_DHCP			"DHCP"
#define AC_MODULENAME_MSCC			"MSCC"



#if 0
extern void ac_capwap_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
extern void ac_8021x_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
extern void ac_charge_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
extern void ac_wapi_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
extern void ac_radius_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
extern void ac_portal_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
/*extern void ac_dtls_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);*/
extern void ac_capwap_data_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
extern void ac_rdbsac_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
#endif

/*extern void wtp_capwap_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);*/
extern void ac_capwap_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
extern void ac_cpmcc_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
extern void ac_8021x_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
extern void ac_charge_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
extern void ac_wapi_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
extern void ac_radius_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
extern void ac_portal_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
/*extern void ac_dtls_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);*/
/*extern void ac_capwap_data_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);*/
/*extern void ac_rdbsac_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);*/
extern void ac_pppoe_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
//extern void ac_dhcp_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);

#define COM_TEST_PROC                   CPSS_VK_PD(200,0)
#define COM_TEST_PROC_NAME				"ComTestProc"
extern void com_test_proc(UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);

#endif /* SWP_USR_COMMON_AC_H_ */
