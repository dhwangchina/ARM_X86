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
#ifndef SWP_USR_COMMON_RNC_H
#define SWP_USR_COMMON_RNC_H

/********************************* 头文件包含 ********************************/

/******************************** 宏和常量定义 *******************************/
/* 模块编号定义(子系统内统一编号，范�?~64) */
#define UPM_MODULE_CCM          ((UINT8)(0x1))  /* CCM模块 */
#define UPM_MODULE_DPM          ((UINT8)(0x2))  /* DPM模块 */
#define UPM_MODULE_IPSTACK      ((UINT8)(0x3))  /* IPSTACK模块 */
#define UPM_MODULE_NAT          ((UINT8)(0x4))  /* NAT模块 */
#define UPM_MODULE_USRM         ((UINT8)(0x5))  /* USRM模块 */
#define UPM_MODULE_UTM          ((UINT8)(0x6))  /* UTM模块 */
#define UPM_MODULE_DHCPRELAY    ((UINT8)(0x7))  /* DhcpRelay模块 */

/* 模块名称(不超�?0字符, 不带子系统前缀) */
#define UPM_MODULENAME_CCM          "CCM"
#define UPM_MODULENAME_DPM          "DPM"
#define UPM_MODULENAME_IPSTACK      "IPSTACK"
#define UPM_MODULENAME_NAT          "NAT"
#define UPM_MODULENAME_USRM         "USRM"
#define UPM_MODULENAME_UTM          "UTM"
#define UPM_MODULENAME_DHCPRELAY    "DHCPRELAY"

#define AC_MODULENAME_CPMCC 	"CPMCC"
#define AC_MODULENAME_APM		"APM"
#define AC_MODULENAME_STAM		"STAM"
#define AC_MODULENAME_DHCP		"DHCP"
#define AC_MODULENAME_RADIUS	"RADIUS"
#define AC_MODULENAME_WAPI		"WAPI"
#define AC_MODULENAME_8021X		"8021X"
#define AC_MODULENAME_PPPOE		"PPPOE"
#define AC_MODULENAME_PORTAL	"PORTAL"
#define AC_MODULENAME_CAPWAP	"CAPWAP"
#define AC_MODULENAME_CHARGE	"CHARGE"
#define AC_MODULENAME_PUB		"PUB"


/* 纤程名称定义 */

#define UPM_CCM_TEST_PROCNAME       "pUpmCcmTest"


/* 纤程描述符定�?*/
/* RDBSR 131~160*/

/* RNC业务部分的模块编号定义(子系统内统一编号，范围1~64) */
/* RDBSR */
#define  RDBSR_MODULE_MAIN      ((UINT8)(0x14))  
#define  RDBSR_MODULE_LRM       ((UINT8)(0x15))  
#define  RDBSR_MODULE_RCM       ((UINT8)(0x16)) 
/*HSPS*/
#define HSPS_RRM_MODULE              ((UINT8)(0x1))   /*RRM模块*/
#define HSPS_CC_MODULE               ((UINT8)(0x2))   /*CC模块*/
#define HSPS_CRM_MODULE              ((UINT8)(0x3))   /*CRM模块*/
#define HSPS_MAIN_MODULE             ((UINT8)(0x4))   /*MAIN模块*/
#define HSPS_DRM_MODULE              ((UINT8)(0x5))   /*DRM模块*/
#define HSPS_IUC_MODULE              ((UINT8)(0x6))   /*IUC模块*/
#define HSPS_IUBC_MODULE             ((UINT8)(0x7))   /*IUBC模块*/
#define HSPS_GPM_MODULE              ((UINT8)(0x8))   /*GPM模块*/
#define HSPS_RAC_MAIN_MODULE         ((UINT8)(0x9))   /*RAC MAIN模块 */
#define HSPS_RB_MODULE               ((UINT8)(0x0A))  /*RB模块*/
#define HSPS_SRB_MODULE              ((UINT8)(0x0B))  /*SRB模块*/
#define HSPS_SMC_MODULE              ((UINT8)(0x0C))  /*SMC模块*/
#define HSPS_CNT_CHK_MODULE          ((UINT8)(0x0D))  /*SMC_COUNT_CHECK模块*/
#define HSPS_MM_MODULE               ((UINT8)(0x0E))  /*MM模块 */
#define HSPS_PNFE_MODULE             ((UINT8)(0xOF))  /*PNFE模块 */
#define HSPS_MBC_MODULE              ((UINT8)(0x10))  /*MBC模块 */

/* OAMS */
#define CPS__OAMS_MDLNO_PM_ADPTR         ((UINT8)(0x0A))     /* 性能管理适配模块 */
#define OAMS_MDLNO_PM_ADPTR         ((UINT8)(0x0A))     /* 性能管理适配模块 */


/* RNC业务部分的模块名称(不超过10字符, 不带子系统前缀) */
/* RDBSR */
#define  RDBSR_MODULENAME_MAIN          "MAIN"
#define  RDBSR_MODULENAME_LRM           "LRM"
#define  RDBSR_MODULENAME_RCM           "RCM"
/* HSPS */
#define HSPS_RRM_MODULE_NAME            "RRM"   /*RRM模块*/
#define HSPS_CC_MODULE_NAME              "CC"   /*CC模块*/
#define HSPS_CRM_MODULE_NAME            "CRM"   /*CRM模块*/
#define HSPS_MAIN_MODULE_NAME           "MAIN"   /*MAIN模块*/
#define HSPS_DRM_MODULE_NAME            "DRM"   /*DRM模块*/
#define HSPS_IUC_MODULE_NAME            "IUC"   /*IUC模块*/
#define HSPS_IUBC_MODULE_NAME           "IUBC"   /*IUBC模块*/
#define HSPS_GPM_MODULE_NAME            "GPM"   /*GPM模块*/
#define HSPS_RAC_MAIN_MODULE_NAME       "RAC_MAIN"   /*RAC MAIN模块 */
#define HSPS_RB_MODULE_NAME             "RB"  /*RB模块*/
#define HSPS_SRB_MODULE_NAME            "SRB"  /*SRB模块*/
#define HSPS_SMC_MODULE_NAME            "SMC"  /*SMC模块*/
#define HSPS_CNT_CHK_MODULE_NAME        "CNT_CHK"  /*SMC_COUNT_CHECK模块*/
#define HSPS_MM_MODULE_NAME             "MM"  /*MM模块 */
#define HSPS_PNFE_MODULE_NAME           "PNFE"  /*PNFE模块 */
#define HSPS_MBC_MODULE_NAME            "MBC"  /*MBC模块 */

/* OAMS */
#define CPS__OAMS_MDLNAME_PM_ADPTR       "IF"          /* 性能管理适配模块 */
#define OAMS_MDLNAME_PM_ADPTR       "PM_ADPTR"          /* 性能管理适配模块 */

/* RNC业务部分的纤程名称定义 */
/* RDBSR */
#define RDBSR_MAIN_PROCNAME            "pRdbsRadioMain"
#define CPS__RDBS_PRIORITY_PROCNAME         "pRdbsRadioPrority"
#define RDBS_PRIORITY_PROCNAME         "pRdbsRadioPrority"

/* TPSS */
#define TPSS_MESSAGE_ANALYSIS_PROCNAME   "pTpssMsgAnalysis"
/* HSPS */
#define HSPS_MAIN_PROCNAME                "pHspsMain"
#define HSPS_IUBC_PROCNAME                "pHspsIubc"
#define HSPS_RAC_PROCNAME                 "pHspsRac"
#define HSPS_IUC_PROCNAME                 "pHspsIuc"
#define HSPS_CC_PROCNAME                  "pHspsCc"
#define HSPS_PNFE_PROCNAME                "pHspsPnfe"

/* OAMS */
#define CPS__OAMS_PD_A_PM_ADPTR_PROCNAME "pOamsPmAdptr"      /* 性能管理适配模块 */
#define OAMS_PD_A_PM_ADPTR_PROCNAME "pOamsPmAdptr"      /* 性能管理适配模块 */

/* RNC业务部分的纤程描述符定义 */
/* RDBSR 231~250;251~280保留*/
#define RDBSR_MAIN_PROC              CPSS_VK_PD(231,0) /* RDBSR入口函数 */
#define RDBSR_PRIORITY_PROC          CPSS_VK_PD(232,0) /* RDBSR_LICENSE文件下载入口函数 */
/* TPSS */
#define TPSS_MESSAGE_ANALYSIS_PROC	 CPSS_VK_PD(211,0) /*TPSS纤程ID：全局规划 */
/*HSPS 模块线程ID标识符的定义*/
#define HSPS_MAIN_PROC               CPSS_VK_PD(165,0)  /*MAIN纤程ID:全局规划 */
#define HSPS_IUBC_PROC               CPSS_VK_PD(162,0)  /*IUBC纤程ID:全局规划 */
#define HSPS_RAC_PROC                CPSS_VK_PD(164,0)  /*RAC纤程ID:全局规划 */
#define HSPS_IUC_PROC                CPSS_VK_PD(163,0)  /*IUC纤程ID:全局规划 */
#define HSPS_CC_PROC                 CPSS_VK_PD(161,0)  /*CCCRMMM纤程ID:全局规划 */
#define HSPS_PNFE_PROC               CPSS_VK_PD(166,0)  /*SMSS子系统纤程ID:全局规划 */


/* OAMS */
#define CPS__OAMS_PD_A_PM_ADPTR_PROC     CPSS_VK_PD(85, 0)   /* 性能管理适配模块入口函数 */
#define OAMS_PD_A_PM_ADPTR_PROC     CPSS_VK_PD(85, 0)   /* 性能管理适配模块入口函数 */



/******************************** 类型定义 ***********************************/

/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/
/* upm subsystem */
extern void upm_capwadata_proc(UINT16 usState,void* pVar,CPSS_COM_MSG_HEAD_T* pstMsgHead);
extern void upm_main_control_proc(UINT16 ulState,VOID   *pVar,CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern void upm_ccm_test_proc(UINT16 ulState,VOID   *pVar,CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern void tnbs_ipstack_proc(UINT16 ulState,VOID   *pVar,CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern void upm_dhcprelay_proc(UINT16 ulState, VOID   *pVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

extern void cps__rdbs_radio_main(UINT16 usState,void* pVar,CPSS_COM_MSG_HEAD_T* pstMsgHead);
extern void cps__rdbs_radio_priority_proc(UINT16 usState,void* pVar,CPSS_COM_MSG_HEAD_T* pstMsgHead);
extern void rdbs_radio_main(UINT16 usState,void* pVar,CPSS_COM_MSG_HEAD_T* pstMsgHead);
extern void rdbs_radio_priority_proc(UINT16 usState,void* pVar,CPSS_COM_MSG_HEAD_T* pstMsgHead);
extern void rnc_tpss_message_analysis(UINT16 usState,void* pVar,CPSS_COM_MSG_HEAD_T* pstMsgHead);

/*HSPS  纤程入口函数*/
extern void hsps_main_proc (UINT16 vusUserState,void *vpVar,CPSS_COM_MSG_HEAD_T *vpstMsgHead);
extern void hsps_iubc_proc(UINT16 vusUserState, void *vpVar, CPSS_COM_MSG_HEAD_T * vpstMsgHead);
extern void hsps_pnfe_proc(UINT16  usState,  void *vpVar , CPSS_COM_MSG_HEAD_T  *vpstMsgHead );
extern void hsps_iuc_proc(UINT16 vusUserState, void *vpVar, CPSS_COM_MSG_HEAD_T * vpstMsgHead);
extern void hsps_rac_proc(UINT16  vusState,   void *  vpVar,CPSS_COM_MSG_HEAD_T * vpstMsgHead);
extern void hsps_cc_proc(UINT16 vusState, void *vpPvtData, CPSS_COM_MSG_HEAD_T *vpstMsg);

/* OAMS 性能管理适配模块入口函数 */
extern void cps__oams_pm_adptr_proc(UINT16 usState,void* pVar,CPSS_COM_MSG_HEAD_T* pstMsgHead);
extern void oams_pm_adptr_proc(UINT16 usState,void* pVar,CPSS_COM_MSG_HEAD_T* pstMsgHead);



/******************************** 头文件保护结尾 *****************************/
#endif /* SWP_USR_COMMON_RNC_H */
/******************************** 头文件结束 *********************************/
