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

/******************************** 头文件保护开头 *****************************/
#ifndef SWP_COMMON_STR_H
#define SWP_COMMON_STR_H

#ifdef  __cplusplus
extern "C" {
#endif

/********************************* 头文件包含 ********************************/
#include "swp_type.h"
#include "swp_common_ac.h"

/******************************** 宏和常量定义 *******************************/
/* 平台部分的纤程描述符定义 */
/* CPSS 1~20 */

#define CPSS_COM_TCP_PROC               CPSS_VK_PD(7,0) /* tcp纤程 */
/* SMSS 21~40 */


#define SMSS_BOOT_PROC                  CPSS_VK_PD(24,0)  /* BOOT */


#define SMSS_FIRMWARE_PROC              CPSS_VK_PD(26,0)  /* IPMC升级 */
#define SMSS_SHMTEMPER_PROC             CPSS_VK_PD(27,0)  /* 机框温度纤程 */

/* RDBS 41~60 */

#define RDBS_SDSYN_RX_PROC              CPSS_VK_PD(44,0)  /* 静态数据同步接收纤程*/
#define RDBS_SDSYN_TX_PROC              CPSS_VK_PD(45,0)  /* 静态数据同步发送纤程*/
#define RDBS_MSSYN_RX_PROC              CPSS_VK_PD(46,0)  /* 主备静态数据同步接收纤程*/
#define RDBS_MSSYN_TX_PROC              CPSS_VK_PD(47,0)  /* 主备静态数据同步发送纤程*/
#define RDBS_DDSYN_RX_PROC              CPSS_VK_PD(48,0)  /* 动态数据同步接收纤程*/
#define RDBS_DDSYN_TX_PROC              CPSS_VK_PD(49,0)  /* 动态数据同步发送纤程*/
#define RDBS_MDSYN_RX_PROC              CPSS_VK_PD(50,0)  /* 主备周期动态数据同步纤程*/
#define RDBS_MDSYN_TX_PROC              CPSS_VK_PD(51,0)  /* 主备周期动态数据同步纤程*/


/* OAMS */
/* OAMS子系统纤程描述符定义
*   纤程类型范围 :  61 ~ 90
*   目前分配如下
*       主控纤程    61 ~ 70
*       保留        71 ~ 76
*       代理纤程    77 ~ 92
*       保留        83 ~ 90
*/
/* 主控模块的纤程描述符 */

#define OAMS_PD_M_AM_MNGR_RPT_PROC      CPSS_VK_PD(62, 0)   /* 告警管理主控上报 */
#define OAMS_PD_M_COMM_PROC             CPSS_VK_PD(63, 0)   /* 前后台通信 */
#define OAMS_PD_M_FTM_PROC              CPSS_VK_PD(64, 0)   /* 文件传输管理 */
#define OAMS_PD_M_LOGM_PROC             CPSS_VK_PD(65, 0)   /* 日志管理 */
#define OAMS_PD_M_MSGDSP_PROC           CPSS_VK_PD(66, 0)   /* 消息分发 */
#define OAMS_PD_M_PM_MNGR_RCV_PROC      CPSS_VK_PD(67, 0)   /* 性能管理主控接收 */
#define OAMS_PD_M_PM_MNGR_RPT_PROC      CPSS_VK_PD(68, 0)   /* 性能管理主控上报 */
#define OAMS_PD_M_TM_PROC               CPSS_VK_PD(69, 0)   /* 事务管理 */
#define OAMS_PD_M_TRC_MNGR_PROC         CPSS_VK_PD(70, 0)   /* 跟踪管理主控 */
#define OAMS_PD_M_DIAG_MNGR_PROC        CPSS_VK_PD(71, 0)   /* 诊断测试管理主控 */
#define OAMS_PD_M_ABOX_MNGR_PROC        CPSS_VK_PD(72, 0)   /* 告警箱管理主控 */
#define OAMS_PD_M_ABOX_DRV_PROC         CPSS_VK_PD(73, 0)   /* 告警箱驱动 */
#define OAMS_PD_M_DLM_MSGDSP_PROC       CPSS_VK_PD(74, 0)   /* 网元直连管理消息分发 */
#define OAMS_PD_M_DLM_FS_PROC           CPSS_VK_PD(75, 0)   /* 网元直连管理文件服务 */

/* 代理模块的纤程描述符 */
#define OAMS_PD_A_AM_AGT_PROC           CPSS_VK_PD(77, 0)   /* 告警管理代理 */
#define OAMS_PD_A_DYND_AGT_PROC         CPSS_VK_PD(78, 0)   /* 动态数据代理 */
#define OAMS_PD_A_DTPRB_AGT_PROC        CPSS_VK_PD(79, 0)   /* 数据探针代理 */
#define OAMS_PD_A_PM_AGT_PROC           CPSS_VK_PD(80, 0)   /* 性能管理代理 */
#define OAMS_PD_A_SHCMD_AGT_PROC        CPSS_VK_PD(81, 0)   /* Shell命令代理 */
#define OAMS_PD_A_TRC_AGT_PROC          CPSS_VK_PD(82, 0)   /* 跟踪管理代理 */
#define OAMS_PD_A_LOG_AGT_PROC          CPSS_VK_PD(83, 0)   /* 日志管理代理 */
#define OAMS_PD_A_DIAG_AGT_PROC         CPSS_VK_PD(84, 0)   /* 诊断测试管理代理 */

/* OAMS_N侧计费模块(CDF)的纤程描述符*/
#define OAMS_N_PD_CDF_PROC              CPSS_VK_PD(85, 0)   /* OAMS_N侧计费模块 */

/* TNSS 91~130 */
#define TNSS_MTP2_PROC                  CPSS_VK_PD(91, 0)
#define TNSS_SSCOP_PROC                 CPSS_VK_PD(92, 0)
#define TNSS_SSCF_NNI_PROC              CPSS_VK_PD(93, 0)
#define TNSS_SSCF_UNI_PROC              CPSS_VK_PD(94, 0)
#define TNSS_MTP3B_SMH_PROC             CPSS_VK_PD(95, 0)
#define TNSS_MTP3B_SLM_PROC             CPSS_VK_PD(96, 0)
#define TNSS_MTP3B_SPCM_PROC            CPSS_VK_PD(97, 0)
#define TNSS_MTP3B_PCC_PROC             CPSS_VK_PD(98, 0)
#define TNSS_MTP3B_TLAC_PROC            CPSS_VK_PD(99, 0)
#define TNSS_MTP3B_TCOC_PROC            CPSS_VK_PD(100, 0)
#define TNSS_MTP3B_TCBC_PROC            CPSS_VK_PD(101, 0)
#define TNSS_MTP3B_TPRC_PROC            CPSS_VK_PD(102, 0)
#define TNSS_MTP3B_RTPCAC_PROC          CPSS_VK_PD(103, 0)
#define TNSS_MTP3B_TFCRC_PROC           CPSS_VK_PD(104, 0)
#define TNSS_MTP3B_CONF_PROC            CPSS_VK_PD(105, 0)
#define TNSS_M3UA_SMAH_PROC             CPSS_VK_PD(106, 0)
#define TNSS_M3UA_ASPM_PROC             CPSS_VK_PD(107, 0)
#define TNSS_M3NIF_PROC                 CPSS_VK_PD(108, 0)
#define TNSS_SCCP_SCRC_PROC             CPSS_VK_PD(109, 0)
#if (SWP_OPTION_TDM_SWITCH != TRUE)
#define TNSS_SCCP_SCLC_PROC             CPSS_VK_PD(110, 0)
#define TNSS_SCCP_SCOC_PROC             CPSS_VK_PD(111, 0)
#define TNSS_SCCP_SCMG_PROC             CPSS_VK_PD(112, 0)
#else
#define TNSS_SCCP_SCLC_PROC             TNSS_SCCP_SCRC_PROC
#define TNSS_SCCP_SCOC_PROC             TNSS_SCCP_SCRC_PROC
#define TNSS_SCCP_SCMG_PROC             TNSS_SCCP_SCRC_PROC
#endif
#define TNSS_TCAP_TSL_PROC              CPSS_VK_PD(113, 0)
#define TNSS_TCAP_CSL_PROC              CPSS_VK_PD(114, 0)
#define TNSS_NF_PROC                    CPSS_VK_PD(115, 0)
#define TNSS_ALCAP_PROC                 CPSS_VK_PD(116, 0)
#define TNSS_STC_PROC                   CPSS_VK_PD(118, 0)
/* TNBS 131~160*/
#define  TNBS_ATMLM_PROC                CPSS_VK_PD(131, 0)
#define  TNBS_IMA_PROC                  CPSS_VK_PD(132, 0)
#define  TNBS_APS_PROC                  CPSS_VK_PD(133, 0)
#define  TNBS_TDM_PROC                  CPSS_VK_PD(134, 0)
#define  TNBS_IKE_PROC                  CPSS_VK_PD(136, 0)
#define  TNBS_L2TP_PROC                 CPSS_VK_PD(137, 0)
#define  TNBS_DNS_PROC                  CPSS_VK_PD(138, 0)
#define  TNBS_DHCPV4_PROC               CPSS_VK_PD(139, 0)
#define  TNBS_DHCPV6_PROC               CPSS_VK_PD(140, 0)
#define  TNBS_RADIUS_PROC               CPSS_VK_PD(141, 0)
#define  TNBS_OSPF_PROC                 CPSS_VK_PD(142, 0)
#define  TNBS_RIP_PROC                  CPSS_VK_PD(143, 0)
#define  TNBS_NSM_PROC                  CPSS_VK_PD(144, 0)
#define  TNBS_ROUTEMNG_PROC             CPSS_VK_PD(145, 0)
#define  TNBS_MCM_PROC                  CPSS_VK_PD(146, 0)
#define  TNBS_GTPU_PROC                 CPSS_VK_PD(147, 0)
#define  TNBS_CHIPMNG_PROC              CPSS_VK_PD(148, 0)
#define  TNBS_DSP_TRAFFIC_PROC          CPSS_VK_PD(149, 0)
#define  TNBS_OSPF6_PROC                CPSS_VK_PD(150, 0)
#define  TNBS_RIP6_PROC                 CPSS_VK_PD(151, 0)
#define  TNBS_PIM6_PROC                 CPSS_VK_PD(152, 0)
#define  TNBS_PPP_PROC                  CPSS_VK_PD(153, 0)
#define  TNBS_FTP_PROC                  CPSS_VK_PD(154, 0)
#define  TNBS_NODEBMNG_PROC             CPSS_VK_PD(155, 0)
#define  TNBS_PIM_PROC                  CPSS_VK_PD(156, 0)

/* ... */

/* 平台部分的纤程名称定义 */
/* CPSS */

#define CPSS_COM_TCP_PROCNAME           "pCpssTcp"
/* SMSS */
#define SMSS_FIRMWARE_PROCNAME          "pSmssFirmware"
#define SMSS_SHMTEMPER_PROCNAME         "pSmssShmTemper"

/* OAMS Mngr */

#define OAMS_PD_M_COMM_PROCNAME         "pOamsComm"         /* 前后台通信 */
#define OAMS_PD_M_FTM_PROCNAME          "pOamsFtm"          /* 文件传输管理 */

#define OAMS_PD_M_MSGDSP_PROCNAME       "pOamsMsgDsp"       /* 消息分发 */
#define OAMS_PD_M_PM_MNGR_RCV_PROCNAME  "pOamsPmMngrRecv"   /* 性能管理主控接收 */
#define OAMS_PD_M_PM_MNGR_RPT_PROCNAME  "pOamsPmMngrRpt"    /* 性能管理主控上报 */
#define OAMS_PD_M_TM_PROCNAME           "pOamsTm"           /* 事务管理 */
#define OAMS_PD_M_TRC_MNGR_PROCNAME     "pOamsTrcMngr"      /* 跟踪管理主控 */
#define OAMS_PD_M_DIAG_MNGR_PROCNAME    "pOamsDiagMngr"     /* 诊断测试管理主控 */
#define OAMS_PD_M_ABOX_MNGR_PROCNAME    "pOamsABoxMngr"     /* 告警箱管理主控 */
#define OAMS_PD_M_ABOX_DRV_PROCNAME     "pOamsABoxDrv"      /* 告警箱驱动 */
#define OAMS_PD_M_DLM_MSGDSP_PROCNAME   "pOamsDlmMsgDsp"      /* 网元直连管理消息分发 */
#define OAMS_PD_M_DLM_FS_PROCNAME       "pOamsDlmFs"      /* 网元直连管理文件服务 */

/* OAMS Agt */

#define OAMS_PD_A_DYND_AGT_PROCNAME     "pOamsDyndAgt"      /* 动态数据代理 */
#define OAMS_PD_A_DTPRB_AGT_PROCNAME    "pOamsDtprbAgt"     /* 数据探针代理 */
#define OAMS_PD_A_PM_AGT_PROCNAME       "pOamsPmAgt"        /* 性能管理代理 */
#define OAMS_PD_A_SHCMD_AGT_PROCNAME    "pOamsShcmdAgt"     /* Shell命令代理 */
#define OAMS_PD_A_TRC_AGT_PROCNAME      "pOamsTrcAgt"       /* 跟踪管理代理 */
#define OAMS_PD_A_LOG_AGT_PROCNAME      "pOamsLomAgt"       /* 日志管理代理 */
#define OAMS_PD_A_DIAG_AGT_PROCNAME     "pOamsDiagAgt"      /* 诊断测试管理代理 */

/*RDBS*/

#define RDBS_SDSYNRX_PROCNAME           "pRdbsSdsynRx"
#define RDBS_SDSYNTX_PROCNAME           "pRdbsSdsynTx"
#define RDBS_MSSYNRX_PROCNAME           "pRdbsMssynRx"
#define RDBS_MSSYNTX_PROCNAME           "pRdbsMssynTx"
#define RDBS_DDSYNRX_PROCNAME           "pRdbsDdsynRx"
#define RDBS_DDSYNTX_PROCNAME           "pRdbsDdsynTx"
#define RDBS_MDSYNRX_PROCNAME           "pRdbsMdsynRx"
#define RDBS_MDSYNTX_PROCNAME           "pRdbsMdsynTx"

#define RDBS_SGSN_NOTIFY_PROCNAME       "pRdbsSgsnNotify"
#define RDBS_GGSN_NOTIFY_PROCNAME       "pRdbsGgsnNotify"

/* TNSS */
#define TNSS_MTP2_PROCNAME              "pTnssMtp2"
#define TNSS_SSCOP_PROCNAME             "pTnssSscop"
#define TNSS_SSCF_NNI_PROCNAME          "pTnssSscfNni"
#define TNSS_SSCF_UNI_PROCNAME          "pTnssSscfUni"
#define TNSS_MTP3B_SMH_PROCNAME         "pTnssMtp3bSmh"
#define TNSS_MTP3B_SLM_PROCNAME         "pTnssMtp3bSlm"
#define TNSS_MTP3B_SPCM_PROCNAME        "pTnssMtp3bSpcm"
#define TNSS_MTP3B_PCC_PROCNAME         "pTnssMtp3bPcc"
#define TNSS_MTP3B_TLAC_PROCNAME        "pTnssMtp3bTlac"
#define TNSS_MTP3B_TCOC_PROCNAME        "pTnssMtp3bTcoc"
#define TNSS_MTP3B_TCBC_PROCNAME        "pTnssMtp3bTcbc"
#define TNSS_MTP3B_TPRC_PROCNAME        "pTnssMtp3bTprc"
#define TNSS_MTP3B_RTPCAC_PROCNAME      "pTnssMtp3bRtpcac"
#define TNSS_MTP3B_TFCRC_PROCNAME       "pTnssMtp3bTfcrc"
#define TNSS_MTP3B_CONF_PROCNAME        "pTnssMtp3bConf"
#define TNSS_M3UA_SMAH_PROCNAME         "pTnssM3uaSmah"
#define TNSS_M3UA_ASPM_PROCNAME         "pTnssM3uaAspm"
#define TNSS_M3NIF_PROCNAME             "pTnssM3nif"
#define TNSS_SCCP_SCRC_PROCNAME         "pTnssSccpScrc"
#define TNSS_SCCP_SCLC_PROCNAME         "pTnssSccpSclc"
#define TNSS_SCCP_SCOC_PROCNAME         "pTnssSccpScoc"
#define TNSS_SCCP_SCMG_PROCNAME         "pTnssSccpScmg"
#define TNSS_TCAP_TSL_PROCNAME          "pTnssTcapTsl"
#define TNSS_TCAP_CSL_PROCNAME          "pTnssTcapCsl"
#define TNSS_NF_PROCNAME                "pTnssNf"
#define TNSS_ALCAP_PROCNAME             "pTnssAlcap"
#define TNSS_STC_PROCNAME               "pTnssStc"
/* TNBS */
#define TNBS_IMA_PROCNAME               "pTnbsIma"
#define TNBS_ATMLM_PROCNAME             "pTnbsAtmLm"
#define TNBS_APS_PROCNAME               "pTnbsAps"
#define TNBS_TDM_PROCNAME               "pTnbsTdm"
#define TNBS_ROUTEMNG_PROCNAME          "pTnbsRouteMng"
#define TNBS_IKE_PROCNAME               "pTnbsIke"
#define TNBS_GTPU_PROCNAME              "pTnbsGtpu"
#define TNBS_L2TP_PROCNAME              "pTnbsL2tp"
#define TNBS_MCM_PROCNAME               "pTnbsMcm"
#define TNBS_RADIUS_PROCNAME            "pTnbsRadius"
#define TNBS_DNS_PROCNAME               "pTnbsDns"
#define TNBS_DHCPV4_PROCNAME            "pTnbsDhcp4"
#define TNBS_DHCPV6_PROCNAME            "pTnbsDhcp6"
#define TNBS_OSPF_PROCNAME              "pTnbsOspf"
#define TNBS_RIP_PROCNAME               "pTnbsRip"
#define TNBS_NSM_PROCNAME               "pTnbsNsm"
#define TNBS_SIMU2_PROCNAME             "pTnbsSimu2"
#define TNBS_SIMU3_PROCNAME             "pTnbsSimu3"
#define TNBS_SIMU4_PROCNAME             "pTnbsSimu4"
#define TNBS_CHIPMNG_PROCNAME           "pTnbsChipMng"
#define TNBS_DSP_TRAFFIC_PROCNAME       "pTnbsDspTraffic"
#define TNBS_FTP_PROCNAME               "pTnbsFtp"
#define TNBS_NODEBMNG_PROCNAME          "pTnbsNodebMng"
#define TNBS_PPP_PROCNAME               "pTnbsPPP"
#define TNBS_PIM_PROCNAME               "pTnbsPim"

/* ... */

/* 平台部分的模块编号定义(子系统内统一编号，范围1~64) */
#define SWP_MODULE_ALL          255                 /* 所有模块 */
#define SWP_MODULE_MAX_NUM      96                  /* 模块最大个数 */



/* CPSS */
#define CPSS_MODULE_COM         ((UINT8)(0x01))     /* 通信管理 */
#define CPSS_MODULE_DBG         ((UINT8)(0x02))     /* 调试管理 */
#define CPSS_MODULE_FS          ((UINT8)(0x03))     /* 文件管理 */
#define CPSS_MODULE_KW          ((UINT8)(0x04))     /* 内核监控 */
#define CPSS_MODULE_TM          ((UINT8)(0x05))     /* 定时器管理 */
#define CPSS_MODULE_VK          ((UINT8)(0x06))     /* 纤程调度 */
#define CPSS_MODULE_VOS         ((UINT8)(0x07))     /* 虚拟OS */
#define CPSS_MODULE_MM          ((UINT8)(0x08))     /* 内存管理 */
#define CPSS_MODULE_CMN         ((UINT8)(0x09))     /* 公共功能 */
/* SMSS */
#define SMSS_MODULE_SYSCTL      ((UINT8)(0x01))
#define SMSS_MODULE_VERM        ((UINT8)(0x02))
#define SMSS_MODULE_DEVM        ((UINT8)(0x03))
#define SMSS_MODULE_BOOT        ((UINT8)(0x04))
#define SMSS_MODULE_SYSRT       ((UINT8)(0x05))
/* RDBS */
#define  RDBS_MODULE_KNL        ((UINT8)(0x01))
#define  RDBS_MODULE_DCM        ((UINT8)(0x02))
#define  RDBS_MODULE_DDSYN      ((UINT8)(0x03))
#define  RDBS_MODULE_MDSYN      ((UINT8)(0x04))
#define  RDBS_MODULE_SDSYN      ((UINT8)(0x05))
#define  RDBS_MODULE_MSSYN      ((UINT8)(0x06))
#define  RDBS_MODULE_IF         ((UINT8)(0x07))
#define  RDBS_MODULE_INIT       ((UINT8)(0x08))
#define  RDBS_MODULE_PROBE      ((UINT8)(0x09))
#define  RDBS_MODULE_DEV        ((UINT8)(0x0A))
#define  RDBS_MODULE_ATM        ((UINT8)(0x0B))
#define  RDBS_MODULE_IPOA       ((UINT8)(0x0C))
#define  RDBS_MODULE_IP         ((UINT8)(0x0D))
#define  RDBS_MODULE_N7         ((UINT8)(0x0E))
#define  RDBS_MODULE_SIGTRAN    ((UINT8)(0x0F))
#define  RDBS_MODULE_COMMON     ((UINT8)(0x10))
#define  RDBS_MODULE_DEBUG      ((UINT8)(0x11))
#define	 RDBS_MODULE_RDBSAC		((UINT8)(0x0A))
#define  AC_MODULE_RDBSAC               ((UINT8)(0x0a))

/* OAMS代理模块编号 */
#define OAMS_MDLNO_CF           ((UINT8)(0x01))     /* 公共功能 */
#define OAMS_MDLNO_AM_AGT       ((UINT8)(0x02))     /* 告警管理代理 */
#define OAMS_MDLNO_DYND_AGT     ((UINT8)(0x03))     /* 动态数据代理 */
#define OAMS_MDLNO_DTPRB_AGT    ((UINT8)(0x04))     /* 数据探针代理 */
#define OAMS_MDLNO_PM_AGT       ((UINT8)(0x05))     /* 性能管理代理 */
#define OAMS_MDLNO_SHCMD_AGT    ((UINT8)(0x06))     /* Shell命令代理 */
#define OAMS_MDLNO_TRC_AGT      ((UINT8)(0x07))     /* 跟踪管理代理 */
#define OAMS_MDLNO_LOG_AGT      ((UINT8)(0x08))     /* 日志管理代理 */
#define OAMS_MDLNO_DIAG_AGT     ((UINT8)(0x09))     /* 诊断测试管理代理 */

#define	OAMS_MDLNO_IF			((UINT8)(0x0a))

#define OAMS_MDLNO_WMC          ((UINT8)(0x10))     /* 网管模块 */

/* OAMS主控模块编号 */
#define OAMS_MDLNO_AM_MNGR      ((UINT8)(0x14))     /* 告警管理主控 */
#define OAMS_MDLNO_COMM         ((UINT8)(0x15))     /* 前后台通信 */
#define OAMS_MDLNO_FTM          ((UINT8)(0x16))     /* 文件传输管理 */
#define OAMS_MDLNO_LOGM         ((UINT8)(0x17))     /* 日志管理 */
#define OAMS_MDLNO_MSGDSP       ((UINT8)(0x18))     /* 消息分发 */
#define OAMS_MDLNO_PM_MNGR      ((UINT8)(0x05))     /* 性能管理主控 */
#define OAMS_MDLNO_TM           ((UINT8)(0x1a))     /* 事务管理 */
#define OAMS_MDLNO_TRC_MNGR     ((UINT8)(0x1b))     /* 跟踪管理主控 */
#define OAMS_MDLNO_DIAG_MNGR    ((UINT8)(0x1c))     /* 诊断测试管理主控 */
#define OAMS_MDLNO_ABOX_MNGR    ((UINT8)(0x1d))     /* 告警箱管理主控 */
#define OAMS_MDLNO_ABOX_DRV     ((UINT8)(0x1e))     /* 告警箱驱动 */
#define OAMS_MDLNO_DLM          ((UINT8)(0x1f))     /* 网元直连管理模块 */


/* 计费模块编号 */
#define OAMS_N_MODULE_CDF       ((UINT8)(0x01))


/* TNSS */
#define TNSS_MODULE_MTP2                ((UINT8)(0x01))
#define TNSS_MODULE_SSCOP               ((UINT8)(0x02))
#define TNSS_MODULE_SSCF_NNI            ((UINT8)(0x03))
#define TNSS_MODULE_SSCF_UNI            ((UINT8)(0x04))
#define TNSS_MODULE_STC                 ((UINT8)(0x05))
#define TNSS_MODULE_MTP3B               ((UINT8)(0x06))
#define TNSS_MODULE_M3UA                ((UINT8)(0x07))
#define TNSS_MODULE_M3NIF               ((UINT8)(0x08))
#define TNSS_MODULE_SCCP                ((UINT8)(0x09))
#define TNSS_MODULE_TCAP                ((UINT8)(0x0a))
#define TNSS_MODULE_NF                  ((UINT8)(0x0b))
#define TNSS_MODULE_ALCAP               ((UINT8)(0x0c))
#define TNSS_MODULE_COMMON              ((UINT8)(0x0d))

/* TNBS */
#define TNBS_APS_MODULE                 ((UINT8)(0x01))
#define TNBS_ATMLM_MODULE               ((UINT8)(0x02))
#define TNBS_IMA_MODULE                 ((UINT8)(0x03))
#define TNBS_TDM_MODULE                 ((UINT8)(0x04))
#define TNBS_HDLC_MODULE                ((UINT8)(0x05))
#define TNBS_AAL2_MODULE                ((UINT8)(0x06))
#define TNBS_AAL5_MODULE                ((UINT8)(0x07))
#define TNBS_CHIPMNG_MOUDULE            ((UINT8)(0x08))
#define TNBS_L2TP_MODULE                ((UINT8)(0x09))
#define TNBS_RADIUS_MODULE              ((UINT8)(0x0a))
#define TNBS_DNS_MODULE                 ((UINT8)(0x0b))
#define TNBS_DHCP_MODULE                ((UINT8)(0x0c))
#define TNBS_DHCPV6_MODULE              ((UINT8)(0x0d))
#define TNBS_IKE_MODULE                 ((UINT8)(0x0e))
#define TNBS_PPP_MODULE                 ((UINT8)(0x0f))
#define TNBS_MLPPP_MODULE               ((UINT8)(0x10))
#define TNBS_UCOM_MODULE                ((UINT8)(0x11))
#define TNBS_MCTM_MODULE                ((UINT8)(0x12))
#define TNBS_GTPU_MODULE                ((UINT8)(0x13))
#define TNBS_GTPU_R_MODULE              ((UINT8)(0x14))
#define TNBS_GTPU_S_MODULE              ((UINT8)(0x15))
#define TNBS_GTPU_G_MODULE              ((UINT8)(0x16))
#define TNBS_RTCP_MODULE                ((UINT8)(0x17))
#define TNBS_SOCKET_MODULE              ((UINT8)(0x18))
#define TNBS_TCP_MODULE                 ((UINT8)(0x19))
#define TNBS_SCTP_MODULE                ((UINT8)(0x1a))
#define TNBS_UDP_MODULE                 ((UINT8)(0x1b))
#define TNBS_ARP_MODULE                 ((UINT8)(0x1c))
#define TNBS_ND_MODULE                  ((UINT8)(0x1d))
#define TNBS_ICMP_MODULE                ((UINT8)(0x1e))
#define TNBS_ICMPV6_MODULE              ((UINT8)(0x1f))
#define TNBS_IPV4_MODULE                ((UINT8)(0x20))
#define TNBS_IPV6_MODULE                ((UINT8)(0x21))
#define TNBS_IGMP_MODULE                ((UINT8)(0x22))
#define TNBS_MLD_MODULE                 ((UINT8)(0x23))
#define TNBS_IPOA_MODULE                ((UINT8)(0x24))
#define TNBS_IF_MODULE                  ((UINT8)(0x25))
#define TNBS_ROUTE_MODULE               ((UINT8)(0x26))
#define TNBS_NSM_MODULE                 ((UINT8)(0x27))
#define TNBS_OSPFV2_MODULE              ((UINT8)(0x28))
#define TNBS_OSPFV3_MODULE              ((UINT8)(0x29))
#define TNBS_RIPV2_MODULE               ((UINT8)(0x2a))
#define TNBS_RIPV3_MODULE               ((UINT8)(0x2b))
#define TNBS_PIMV4_MODULE               ((UINT8)(0x2c))
#define TNBS_PIMV6_MODULE               ((UINT8)(0x2d))
#define TNBS_IPIP_MODULE                ((UINT8)(0x2e))
#define TNBS_NAT_MODULE                 ((UINT8)(0x2f))
#define TNBS_ACL_MODULE                 ((UINT8)(0x30))
#define TNBS_AH_MODULE                  ((UINT8)(0x31))
#define TNBS_ESP_MODULE                 ((UINT8)(0x32))
#define TNBS_DSCP_MODULE                ((UINT8)(0x33))
#define TNBS_RTP_MODULE                 ((UINT8)(0x34))
#define TNBS_E1TTP_MODULE               ((UINT8)(0x35))
#define TNBS_DSPTTP_MODULE              ((UINT8)(0x36))
#define TNBS_COM_MODULE                 ((UINT8)(0x37))
#define TNBS_DBG_MODULE                 ((UINT8)(0x38))
#define TNBS_FTP_MODULE                 ((UINT8)(0x39))
#define TNBS_NODEBMNG_MODULE            ((UINT8)(0x3a))


/* 平台部分的模块名称(不超过10字符, 不带子系统前缀) */
#define SWP_MODULE_NAME_MAX_LEN (20)
/* DRV */
#define DRV_COMMON_PRINT_ABRV                   "DRV"
#define DRV_COMMON_PRINT_ABRV_COMMON            "COMMON" /*驱动公共组件模块*/
#define DRV_COMMON_PRINT_ABRV_PM5384            "PM5384" /* PM5384 模块 */
#define DRV_COMMON_PRINT_ABRV_PM5320            "PM5320"/* PM5320 模块 */
#define DRV_COMMON_PRINT_ABRV_APC               "APC"/* ATM交换APC芯片驱动软件模块*/
#define DRV_COMMON_PRINT_ABRV_NSS               "NSS"/*网同步算法软件模块*/
#define DRV_COMMON_PRINT_ABRV_IMA               "IMA"/*IMA驱动模块*/
#define DRV_COMMON_PRINT_ABRV_SAR               "SAR"/*ATM SAR模块*/
#define DRV_COMMON_PRINT_ABRV_ETH64460           "ETH64460"/*64460的千兆网口驱动模块*/
#define DRV_COMMON_PRINT_ABRV_IPMI               "IPMI"/* 机框管理IPMI模块 */
#define DRV_COMMON_PRINT_ABRV_FPGA               "FPGA"/* FPGA自动下载驱动软件模块 */
#define DRV_COMMON_PRINT_ABRV_BOND               "BOND"/*    以太端口控制模块（END的通用部分） */
#define DRV_COMMON_PRINT_ABRV_IPHA               "IPHA"/* IP交换HA软件模块    */
#define DRV_COMMON_PRINT_ABRV_L2M               "L2M"/*以太交换管理软件模块 */
#define DRV_COMMON_PRINT_ABRV_UPDTC                "UPDTC"/*板卡间UPDATE通道的控制软件模块 */
#define DRV_COMMON_PRINT_ABRV_FE8247             "FE8247"/* MPC8247以太驱动软件模块 */
#define DRV_COMMON_PRINT_ABRV_FE8560              "FE8560"/* MPC8560以太驱动软件模块 */
#define DRV_COMMON_PRINT_ABRV_PCI8247             "PCI8247"/* MPC8247 PCI驱动软件模块 */
#define DRV_COMMON_PRINT_ABRV_PCI8560            "PCI8560"/* MPC8560 PCI驱动软件模块 */
#define DRV_COMMON_PRINT_ABRV_TSEC                "TSEC"/* MPC8560以太tsec驱动软件模块 */
#define DRV_COMMON_PRINT_ABRV_TCAM                "TCAM"/*    TCAM模块 */
#define DRV_COMMON_PRINT_ABRV_BCM                 "BCM"/* IP交换芯片BCM56xx驱动软件模块 */
#define DRV_COMMON_PRINT_ABRV_FEI                 "FEI8255X"/*以太MAC芯片Intel 8255x网络驱动软件模块    */
#define DRV_COMMON_PRINT_ABRV_ETH06885             "ETH06885"/*以太TXC-06885驱动软件模块 */
#define DRV_COMMON_PRINT_ABRV_E12288               "E12288"/* E1 FRAME芯片 DS2288驱动软件模块    */
#define DRV_COMMON_PRINT_ABRV_GEI                   "GEI82546"/*以太MAC芯片82546驱动软件模块*/
#define DRV_COMMON_PRINT_ABRV_MV64460                 "MV64460"/*外设接口芯片64460 驱动软件模块*/
#define DRV_COMMON_PRINT_ABRV_SCSI                 "SCSI"/* SCSI控制器驱动软件模块    */
#define DRV_COMMON_PRINT_ABRV_USB                   "USB"/* USB控制器ISP1760驱动软件模块*/
#define DRV_COMMON_PRINT_ABRV_PM8316              "PM8316"/* SDH映射器PM8316芯片驱动软件模块*/
#define DRV_COMMON_PRINT_ABRV_APP550              "APP550"/*网络处理器APP550驱动软件模块*/
#define DRV_COMMON_PRINT_ABRV_APP100               "APP100"/* AAL2处理芯片APP100驱动软件模块*/
#define DRV_COMMON_PRINT_ABRV_TDM                 "TDM"/* TDM交换驱动软件模块    */
#define DRV_COMMON_PRINT_ABRV_MCC                 "TDMHDLC"/* TDM－HDLC驱动软件模块*/
#define DRV_COMMON_PRINT_ABRV_L2PRSTP            "RSTP"/* 二层的rstp协议模块 */
#define DRV_COMMON_PRINT_ABRV_L2PLACP            "LACP"/* 二层的lacp协议模块 */
#define DRV_COMMON_PRINT_ABRV_L2PGMRP               "GMRP"/* 二层的gmrp协议模块 */
#define DRV_COMMON_PRINT_ABRV_L2PVLAN            "VLAN"/* 二层的vlan协议模块 */
#define DRV_COMMON_PRINT_ABRV_L2PCOS              "COS"/* 二层的cos协议模块 */
#define DRV_COMMON_PRINT_ABRV_L2PTRUNK            "TRUNK"/* 二层的trunk协议模块 */
#define DRV_COMMON_PRINT_ABRV_L2PIGMPSNP           "IGMPSNP"/* 二层的IGMP Snooping协议模块 */
#define DRV_COMMON_PRINT_ABRV_L2PGVRP               "GVRP"/* 二层的GVRP协议模块 */
#define DRV_COMMON_PRINT_ABRV_L2PMIRROR            "MIRROR"/* 二层的Mirror协议模块 */
#define DRV_COMMON_PRINT_ABRV_BSP                   "BSP"/* bsp模块 */
#define DRV_COMMON_PRINT_ABRV_GSSABRDCFG           "GSSABRDCFG"/* GSSA单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_TSSABRDCFG            "TSSABRDCFG"/* TSSA单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_MASABRDCFG            "MASABRDCFG"/*MASA单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_MMPABRDCFG            "MMPABRDCFG"/* MMPA单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_MNPA8560BRDCFG       "MNPA8560BRDCFG"/* MNPA_8560单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_MNPAXSCALEBRDCFG       "MNPAXSCALEBRDCFG"/* MNPA_XSCALE单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_MDPABRDCFG             "MDPABRDCFG"/*MDPA单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_IWFABRDCFG             "IWFABRDCFG"/*IWFA单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_MEIA8247BRDCFG       "MEIA8247BRDCFG"/* MEIA_8247单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_MEIA8560BRDCFG       "MEIA8560BRDCFG"/* MEIA_8560单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_MAPABRDCFG              "MAPABRDCFG"/* MAPA单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_ERIABRDCFG              "ERIABRDCFG"/*ERIA单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_ORIABRDCFG              "ORIABRDCFG"/* ORIA单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_NSCABRDCFG              "NSCABRDCFG"/* NSCA单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_MMPAOEMBRDCFG         "MMPAOEMBRDCFG"/* MMPA_OEM单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_APS                 "APS"/* APS模块*/
#define DRV_COMMON_PRINT_ABRV_CAM                     "CAM"/*CAM模块 */
#define DRV_COMMON_PRINT_ABRV_ALBABRDCFG             "ALBABRDCFG"/* ALBA单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_IPMCHOST                "IPMCHOST"/* IPMC_HOST模块 */
#define DRV_COMMON_PRINT_ABRV_LLP                  "LLP"/*LLP模块 */
#define DRV_COMMON_PRINT_ABRV_MEPABRDCFG           "MEPABRDCFG"/* MEPA单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_LIUDS26334           "LIUDS26334"/* LIUDS26334模块 */
#define DRV_COMMON_PRINT_ABRV_CSPABRDCFG           "CSPABRDCFG"/* CSPA单板配置模块 */
#define DRV_COMMON_PRINT_ABRV_CPLD                 "CPLD"/* CPLD模块 */
#define DRV_COMMON_PRINT_ABRV_ULTRAMAPPER          "ULTRAMAPPER"




/* CPSS */
#define CPSS_MODULENAME_COM             "COM"
#define CPSS_MODULENAME_DBG             "DBG"
#define CPSS_MODULENAME_FS              "FS"
#define CPSS_MODULENAME_KW              "KM"
#define CPSS_MODULENAME_TM              "TM"
#define CPSS_MODULENAME_VK              "VK"
#define CPSS_MODULENAME_VOS             "VOS"
#define CPSS_MODULENAME_MM              "MM"
#define CPSS_MODULENAME_CMN             "COMMON"
/* SMSS */
#define SMSS_MODULENAME_SYSCTL          "SYSCTL"
#define SMSS_MODULENAME_VERM            "VERM"
#define SMSS_MODULENAME_DEVM            "DEVM"
#define SMSS_MODULENAME_BOOT            "BOOT"
#define SMSS_MODULENAME_SYSRT           "SYSRT"
/* RDBS */
#define  RDBS_MODULENAME_KNL            "KNL"
#define  RDBS_MODULENAME_DCM            "DCM"
#define  RDBS_MODULENAME_DDSYN          "DDSYN"
#define  RDBS_MODULENAME_MDSYN          "MDSYN"
#define  RDBS_MODULENAME_SDSYN          "SDSYN"
#define  RDBS_MODULENAME_MSSYN          "MSSYN"
#define  RDBS_MODULENAME_IF             "IF"
#define  RDBS_MODULENAME_INIT           "INIT"
#define	 RDBS_MODULENAME_RDBSAC			"RDBSAC"
#define  RDBS_MODULENAME_PROBE          "PROBE"
#define  RDBS_MODULENAME_DEV            "DEV"
#define  RDBS_MODULENAME_ATM            "ATM"
#define  RDBS_MODULENAME_IPOA           "IPOA"
#define  RDBS_MODULENAME_IP             "IP"
#define  RDBS_MODULENAME_N7             "N7"
#define  RDBS_MODULENAME_SIGTRAN        "SIGTRAN"
#define  RDBS_MODULENAME_COMMON         "COMMON"
#define  RDBS_MODULENAME_DEBUG          "DEBUG"
/* OAMS代理模块名称 */
#define OAMS_MDLNAME_CF                 "CF"        /* 公共功能 */
#define OAMS_MDLNAME_AM_AGT             "AM_AGT"    /* 告警管理代理 */
#define OAMS_MDLNAME_DYND_AGT           "DYND_AGT"  /* 动态数据代理 */
#define OAMS_MDLNAME_DTPRB_AGT          "DTPRB_AGT" /* 数据探针代理 */
#define OAMS_MDLNAME_PM_AGT             "PM_AGT"    /* 性能管理代理 */
#define OAMS_MDLNAME_SHCMD_AGT          "SHCMD_AGT" /* Shell命令代理 */
#define OAMS_MDLNAME_TRC_AGT            "TRC_AGT"   /* 跟踪管理代理 */
#define OAMS_MDLNAME_LOG_AGT            "LOG_AGT"       /* 日志管理代理 */
#define OAMS_MDLNAME_DIAG_AGT           "DIAG_AGT"      /* 诊断测试管理代理 */
#define OAMS_MDLNAME_IF					"IF"
#define OAMS_MDLNAME_WMC                "WMC"       /* 网管模块 */

/* OAMS主控模块名称 */
#define OAMS_MDLNAME_AM_MNGR            "AM_MNGR"   /* 告警管理主控 */
#define OAMS_MDLNAME_COMM               "COMM"      /* 前后台通信 */
#define OAMS_MDLNAME_FTM                "FTM"       /* 文件传输管理 */
#define OAMS_MDLNAME_LOGM               "LOGM"      /* 日志管理 */
#define OAMS_MDLNAME_MSGDSP             "MSGDSP"    /* 消息分发 */
#define OAMS_MDLNAME_PM_MNGR            "PM_MNGR"   /* 性能管理主控 */
#define OAMS_MDLNAME_TM                 "TM"        /* 事务管理 */
#define OAMS_MDLNAME_TRC_MNGR           "TRC_MNGR"  /* 跟踪管理主控 */
#define OAMS_MDLNAME_DIAG_MNGR          "DIAG_MNGR"     /* 诊断测试管理主控 */
#define OAMS_MDLNAME_ABOX_MNGR          "ABOX_MNGR" /* 告警箱管理主控 */
#define OAMS_MDLNAME_ABOX_DRV           "ABOX_DRV"  /* 告警箱驱动 */


#define OAMS_MDLNAME_DLM                "DLM"  /* 网元直连管理模块 */

/* OAMS_N */
#define OAMS_N_MDLNAME_CDF              "CDF"       /* 计费模块 */

/* TNSS */
#define  TNSS_MODULENAME_COMMON         "COMMON"
#define  TNSS_MODULENAME_MTP2           "MTP2"
#define  TNSS_MODULENAME_SSCOP          "SSCOP"
#define  TNSS_MODULENAME_SSCFNNI        "SSCFNNI"
#define  TNSS_MODULENAME_SSCFUNI        "SSCFUNI"
#define  TNSS_MODULENAME_STC            "STC"
#define  TNSS_MODULENAME_MTP3B          "MTP3B"
#define  TNSS_MODULENAME_M3UA           "M3UA"
#define  TNSS_MODULENAME_M3NIF          "M3NIF"
#define  TNSS_MODULENAME_SCCP           "SCCP"
#define  TNSS_MODULENAME_TCAP           "TCAP"
#define  TNSS_MODULENAME_NF             "NF"
#define  TNSS_MODULENAME_ALCAP          "ALCAP"

/* TNBS */
#define TNBS_APSNAME_MODULE                     "APS"
#define TNBS_ATMLMNAME_MODULE                   "ATMLM"
#define TNBS_IMANAME_MODULE                     "IMA"
#define TNBS_TDMNAME_MODULE                     "TDM"
#define TNBS_HDLCNAME_MODULE                    "HDLC"
#define TNBS_AAL2NAME_MODULE                    "AAL2"
#define TNBS_AAL5NAME_MODULE                    "AAL5"
#define TNBS_CHIPMNGNANE_MODULE                 "CHIPMNG"
#define TNBS_L2TPNAME_MODULE                    "L2TP"
#define TNBS_RADIUSNAME_MODULE                  "RADIUS"
#define TNBS_DNSNAME_MODULE                     "DNS"
#define TNBS_DHCPNAME_MODULE                    "DHCP"
#define TNBS_DHCPV6NAME_MODULE                  "DHCPV6"
#define TNBS_IKENAME_MODULE                     "IKE"
#define TNBS_PPPNAME_MODULE                     "PPP"
#define TNBS_MLPPPNAME_MODULE                   "MLPPP"
#define TNBS_UCOMNAME_MODULE                    "UCOM"
#define TNBS_MCTMNAME_MODULE                    "MCTM"
#define TNBS_GTPUNAME_MODULE                    "GTPU"
#define TNBS_GTPU_RNAME_MODULE                  "GTPU_R"
#define TNBS_GTPU_SNAME_MODULE                  "GTPU_S"
#define TNBS_GTPU_GNAME_MODULE                  "GTPU_G"
#define TNBS_RTCPNAME_MODULE                    "RTCP"
#define TNBS_SOCKETNAME_MODULE                  "SOCKET"
#define TNBS_TCPNAME_MODULE                     "TCP"
#define TNBS_SCTPNAME_MODULE                    "SCTP"
#define TNBS_UDPNAME_MODULE                     "UDP"
#define TNBS_ARPNAME_MODULE                     "ARP"
#define TNBS_NDNAME_MODULE                      "ND"
#define TNBS_ICMPNAME_MODULE                    "ICMP"
#define TNBS_ICMPV6NAME_MODULE                  "ICMPV6"
#define TNBS_IPV4NAME_MODULE                    "IPV4"
#define TNBS_IPV6NAME_MODULE                    "IPV6"
#define TNBS_IGMPNAME_MODULE                    "IGMP"
#define TNBS_MLDNAME_MODULE                     "MLD"
#define TNBS_IPOANAME_MODULE                    "IPOA"
#define TNBS_IFNAME_MODULE                      "IF"
#define TNBS_ROUTENAME_MODULE                   "ROUTE"
#define TNBS_NSMNAME_MODULE                     "NSM"
#define TNBS_OSPFV2NAME_MODULE                  "OSPFV2"
#define TNBS_OSPFV3NAME_MODULE                  "OSPFV3"
#define TNBS_RIPV2NAME_MODULE                   "RIPV2"
#define TNBS_RIPV3NAME_MODULE                   "RIPV3"
#define TNBS_PIMV4NAME_MODULE                   "PIMV4"
#define TNBS_PIMV6NAME_MODULE                   "PIMV6"
#define TNBS_IPIPNAME_MODULE                    "IPIP"
#define TNBS_NATNAME_MODULE                     "NAT"
#define TNBS_ACLNAME_MODULE                     "ACL"
#define TNBS_AHNAME_MODULE                      "AH"
#define TNBS_ESPNAME_MODULE                     "ESP"
#define TNBS_DSCPNAME_MODULE                    "DSCP"
#define TNBS_RTPNAME_MODULE                     "RTP"
#define TNBS_E1TTPNAME_MODULE                   "E1TTP"
#define TNBS_DSPTTPAME_MODULE                   "DSPTTP"
#define TNBS_COMNAME_MODULE                     "COM"
#define TNBS_DBGNAME_MODULE                     "DBG"
#define TNBS_FTPNAME_MODULE                     "FTP"

/******************************** 类型定义 ************************************/
/* 网元类型名称 */
#if !defined(SWP_NE_TYPE)
	#define SWP_NETYPE_NAME     "UNDEF_NE"
#elif (SWP_NE_TYPE == SWP_NETYPE_NODEB)
	#define SWP_NETYPE_NAME     "NODEB"
#elif (SWP_NE_TYPE == SWP_NETYPE_RNC)
	#define SWP_NETYPE_NAME     "RNC"
#elif (SWP_NE_TYPE == SWP_NETYPE_MSCS)
	#define SWP_NETYPE_NAME     "MSCS"
#elif (SWP_NE_TYPE == SWP_NETYPE_MGW)
	#define SWP_NETYPE_NAME     "MGW"
#elif (SWP_NE_TYPE == SWP_NETYPE_SGSN)
	#define SWP_NETYPE_NAME     "SGSN"
#elif (SWP_NE_TYPE == SWP_NETYPE_GGSN)
	#define SWP_NETYPE_NAME     "GGSN"
#elif (SWP_NE_TYPE == SWP_NETYPE_HLR)
	#define SWP_NETYPE_NAME     "HLR"
#else
	#define SWP_NETYPE_NAME     "UNKNOWN_NE"
#endif

/* 物理单板类型名称 */
#if !defined(SWP_PHYBRD_TYPE)
	#define SWP_PHYBRD_NAME "UNDEF_PHYBRD"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_GSSA)
	#define SWP_PHYBRD_NAME "GSSA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_TSSA)
	#define SWP_PHYBRD_NAME "TSSA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_MASA)
	#define SWP_PHYBRD_NAME "MASA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_MMPA)
	#define SWP_PHYBRD_NAME "MMPA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_MNPA)
	#define SWP_PHYBRD_NAME "MNPA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_MDPA)
	#define SWP_PHYBRD_NAME "MDPA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_IWFA)
	#define SWP_PHYBRD_NAME "IWFA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_MEIA)
	#define SWP_PHYBRD_NAME "MEIA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_MAPA)
	#define SWP_PHYBRD_NAME "MAPA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_ERIA)
	#define SWP_PHYBRD_NAME "ERIA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_ORIA)
	#define SWP_PHYBRD_NAME "ORIA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_NSCA)
	#define SWP_PHYBRD_NAME "NSCA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_MMPA_OEM)
	#define SWP_PHYBRD_NAME "MMPA_OEM"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_V2_805)
  #define SWP_PHYBRD_NAME "V2_805"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_V2_NPEA)
  #define SWP_PHYBRD_NAME "V2_NPEA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_ALBA)
  #define SWP_PHYBRD_NAME "ALBA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_MEPA)
  #define SWP_PHYBRD_NAME "MEPA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_CSPA)
  #define SWP_PHYBRD_NAME "CSPA"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_GSSB)
	#define SWP_PHYBRD_NAME "GSSB"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_TSSB)
	#define SWP_PHYBRD_NAME "TSSB"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_MASB)
	#define SWP_PHYBRD_NAME "MASB"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_MEIB)
	#define SWP_PHYBRD_NAME "MEIB"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_SOIB)
	#define SWP_PHYBRD_NAME "SOIB"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_FEIB)
	#define SWP_PHYBRD_NAME "FEIB"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_GEIB)
	#define SWP_PHYBRD_NAME "GEIB"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_ORIB)
	#define SWP_PHYBRD_NAME "ORIB"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_NSCB)
	#define SWP_PHYBRD_NAME "NSCB"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_MEPB)
	#define SWP_PHYBRD_NAME "MEPB"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_MABP)
	#define SWP_PHYBRD_NAME "MABP"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_MABP_OEM)
	#define SWP_PHYBRD_NAME "MABP_OEM "
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_CSIC)
	#define SWP_PHYBRD_NAME "CSIC"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_NSIC)
	#define SWP_PHYBRD_NAME "NSIC"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_FEIC)
	#define SWP_PHYBRD_NAME "FEIC"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_GEIC)
	#define SWP_PHYBRD_NAME "GEIC"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_PMCFE_OEM)
	#define SWP_PHYBRD_NAME "PMCFE_OEM"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_SPMC)
	#define SWP_PHYBRD_NAME "SPMC"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_NQIC)
	#define SWP_PHYBRD_NAME "NQIC"
#elif (SWP_PHYBRD_TYPE == SWP_PHYBRD_FTUB)
	#define SWP_PHYBRD_NAME "FTUB"
#else
	#define SWP_PHYBRD_NAME "UNKNOW_PHYBRD"
#endif

/* 功能单板类型名称 */
#if !defined(SWP_FUNBRD_TYPE)
	#define SWP_FUNBRD_NAME "UNDEF_FUNBRD"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_SAIB)
	#define SWP_FUNBRD_NAME "SAIB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_CAIB)
	#define SWP_FUNBRD_NAME "CAIB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_EAIB)
	#define SWP_FUNBRD_NAME "EAIB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_IPIB)
	#define SWP_FUNBRD_NAME "IPIB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_ERIB)
	#define SWP_FUNBRD_NAME "ERIB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_SRIB)
	#define SWP_FUNBRD_NAME "SRIB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_NSIB)
	#define SWP_FUNBRD_NAME "NSIB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_CGIB)
	#define SWP_FUNBRD_NAME "CGIB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_TSNB)
	#define SWP_FUNBRD_NAME "TSNB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_MPNB)
	#define SWP_FUNBRD_NAME "MPNB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_CPNB)
	#define SWP_FUNBRD_NAME "CPNB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_USNB)
	#define SWP_FUNBRD_NAME "USNB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_CLKB)
	#define SWP_FUNBRD_NAME "CLKB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_OMCB)
	#define SWP_FUNBRD_NAME "OMCB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_SSPB)
	#define SWP_FUNBRD_NAME "SSPB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_GSPB)
	#define SWP_FUNBRD_NAME "GSPB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_MSPB)
	#define SWP_FUNBRD_NAME "MSPB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_MGPB)
	#define SWP_FUNBRD_NAME "MGPB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_HSPB)
	#define SWP_FUNBRD_NAME "HSPB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_TCPB)
	#define SWP_FUNBRD_NAME "TCPB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_IWFB)
	#define SWP_FUNBRD_NAME "IWFB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_MRPB)
	#define SWP_FUNBRD_NAME "MRPB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_GUPB)
	#define SWP_FUNBRD_NAME "GUPB"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
	#define SWP_FUNBRD_NAME "GCPA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
	#define SWP_FUNBRD_NAME "RSPA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_RTPA)
	#define SWP_FUNBRD_NAME "RTPA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_PTPA)
	#define SWP_FUNBRD_NAME "PTPA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_ONCA)
	#define SWP_FUNBRD_NAME "ONCA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_CGDA)
	#define SWP_FUNBRD_NAME "CGDA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_GTSA)
	#define SWP_FUNBRD_NAME "GTSA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCSA)
	#define SWP_FUNBRD_NAME "GCSA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_TCSA)
	#define SWP_FUNBRD_NAME "TCSA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_CASA)
	#define SWP_FUNBRD_NAME "CASA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_NASA)
	#define SWP_FUNBRD_NAME "NASA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_IAEA)
	#define SWP_FUNBRD_NAME "IAEA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_IAPA)
	#define SWP_FUNBRD_NAME "IAPA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_CAPA)
	#define SWP_FUNBRD_NAME "CAPA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_IPEA)
	#define SWP_FUNBRD_NAME "IPEA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_IPSA)
	#define SWP_FUNBRD_NAME "IPSA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_IPUA)
	#define SWP_FUNBRD_NAME "IPUA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_IPCA)
	#define SWP_FUNBRD_NAME "IPCA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_CADA)
	#define SWP_FUNBRD_NAME "CADA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_PADA)
	#define SWP_FUNBRD_NAME "PADA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_RADA)
	#define SWP_FUNBRD_NAME "RADA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_NADA)
	#define SWP_FUNBRD_NAME "NADA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_CIDA)
	#define SWP_FUNBRD_NAME "CIDA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_PIDA)
	#define SWP_FUNBRD_NAME "PIDA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_RIDA)
	#define SWP_FUNBRD_NAME "RIDA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_NIDA)
	#define SWP_FUNBRD_NAME "NIDA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_CAEA)
	#define SWP_FUNBRD_NAME "CAEA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_PAEA)
	#define SWP_FUNBRD_NAME "PAEA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_RAEA)
	#define SWP_FUNBRD_NAME "RAEA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_NAEA)
	#define SWP_FUNBRD_NAME "NAEA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_CIEA)
	#define SWP_FUNBRD_NAME "CIEA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_PIEA)
	#define SWP_FUNBRD_NAME "PIEA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_RIEA)
	#define SWP_FUNBRD_NAME "RIEA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_NIEA)
	#define SWP_FUNBRD_NAME "NIEA"
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_ABOX)
	#define SWP_FUNBRD_NAME "ABOX"
#elif (SWP_FUNBRD_TYPE == SWP_ABS_FUNBRD_IF_ATM)
	#define SWP_FUNBRD_NAME "IF_ATM"
#elif (SWP_FUNBRD_TYPE == SWP_ABS_FUNBRD_IF_IP)
	#define SWP_FUNBRD_NAME "IF_IP"
#elif (SWP_FUNBRD_TYPE == SWP_ABS_FUNBRD_IF_LTDM)
	#define SWP_FUNBRD_NAME "IF_LTDM"
#elif (SWP_FUNBRD_TYPE == SWP_ABS_FUNBRD_CLK)
	#define SWP_FUNBRD_NAME "CLK"
#elif (SWP_FUNBRD_TYPE == SWP_ABS_FUNBRD_SW_LS)
	#define SWP_FUNBRD_NAME "SW_LS"
#elif (SWP_FUNBRD_TYPE == SWP_ABS_FUNBRD_SW_IGS)
	#define SWP_FUNBRD_NAME "SW_IGS"
#elif (SWP_FUNBRD_TYPE == SWP_ABS_FUNBRD_C_MAIN)
	#define SWP_FUNBRD_NAME "C_MAIN"
#elif (SWP_FUNBRD_TYPE == SWP_ABS_FUNBRD_C_APP)
	#define SWP_FUNBRD_NAME "C_APP"
#elif (SWP_FUNBRD_TYPE == SWP_ABS_FUNBRD_U_GTPU)
	#define SWP_FUNBRD_NAME "U_GTPU"
#elif (SWP_FUNBRD_TYPE == SWP_ABS_FUNBRD_U_APP)
	#define SWP_FUNBRD_NAME "U_APP"
#else
	#define SWP_FUNBRD_NAME "UNKNOWN_FUNBRD"
#endif
/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/
/* 纤程入口函数声明 */
/******************************** CPSS BEGIN ***************************/
/* 链路纤程 */
extern VOID cpss_com_link_proc(UINT16  usUserState, VOID *  pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/*滑窗纤程*/
extern VOID cpss_com_slid_proc(UINT16  usUserState, VOID *  pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* 调试管理纤程 */
extern void cpss_debug_proc(UINT16  usUserState, void    *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* SNTP纤程 */
VOID cpss_tm_sntp_proc(UINT16 usState, void *pVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* TCP纤程*/
extern VOID cpss_com_tcp_proc(UINT16  usUserState,VOID *  pvVar,CPSS_COM_MSG_HEAD_T *pstMsgHead);

/*DEVM纤程*/
extern VOID cps_devm_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsg);

/******************************** CPSS END   ***************************/

/******************************** SMSS BEGIN ***************************/
extern VOID smss_sysctl_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID smss_verm_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID smss_devm_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID smss_sysrt_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID smss_firmware_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID smss_shmtemper_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID rdbs_file_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

/******************************** SMSS END   ***************************/

/******************************** OAMS BEGIN ***************************/
/* 主控模块的纤程入口 */
/* 告警管理主控接收 */
VOID oams_am_mngr_rcv_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 告警管理主控上报 */
VOID oams_am_mngr_rpt_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 前后台通信 */
VOID oams_comm_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 文件传输管理 */
VOID oams_ftm_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 日志管理 */
VOID oams_logm_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 消息分发 */
VOID oams_msgdsp_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 性能管理主控接收 */
VOID oams_pm_mngr_recvdata_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 性能管理主控上报 */
VOID oams_pm_mngr_rptdata_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 事务管理 */
VOID oams_tm_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 跟踪管理主控 */
VOID oams_trc_mngr_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 诊断测试管理主控 */
VOID oams_diag_mngr_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 告警箱管理主控 */
VOID oams_abox_mngr_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 网元直连管理消息分发 */
VOID oams_dlm_msgdsp_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 网元直连管理文件服务 */
VOID oams_dlm_fs_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 告警箱驱动 */
VOID oams_abox_drv_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);

/* 代理模块的纤程入口 */
/* 告警管理代理 */
VOID oams_am_agt_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 动态数据代理 */
VOID oams_dynd_agt_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 数据探针代理 */
VOID oams_dtprb_agt_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 性能管理代理 */
VOID oams_pm_agt_proc(UINT16 usUserState, VOID * pvVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* Shell命令代理 */
VOID oams_shcmd_agt_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 跟踪管理代理 */
VOID oams_trc_agt_proc(UINT16 usUserState, VOID * pvVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 诊断测试管理代理 */
VOID oams_diag_agt_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* 日志管理代理 */
VOID oams_log_agt_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);


/******************************** OAMS END   ***************************/

/******************************** OAMS_N BEGIN***************************/
/* CDF模块 */
VOID oams_n_cdf_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

/******************************** OAMS_N END  ***************************/


/******************************** RDBS BEGIN ***************************/
extern VOID rdbs_if_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID rdbs_dcm_proc(UINT16 usUserState, VOID* pvVar, CPSS_COM_MSG_HEAD_T* pstMsgHead);
extern VOID rdbs_init_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

extern VOID rdbs_sdsyn_rx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID rdbs_sdsyn_tx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

extern VOID rdbs_mssyn_rx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID rdbs_mssyn_tx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

extern VOID rdbs_ddsyn_rx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID rdbs_ddsyn_tx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

extern VOID rdbs_mdsyn_rx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID rdbs_mdsyn_tx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

extern VOID rdbs_verify_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/******************************** RDBS END   ***************************/

/******************************** TNBS BEGIN ***************************/
extern  VOID tnbs_aps_proc (UINT16   usUserState,
							VOID     *pvVar,
							CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_atmlm_proc (UINT16   usUserState,
							  VOID     *pvVar,
							  CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_dhcpv4_proc (UINT16   usUserState,
							   VOID     *pvVar,
							   CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_dns_proc (UINT16   usUserState,
							 VOID     *pvVar,
							 CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_gtpu_proc (UINT16   usUserState,
							 VOID     *pvVar,
							 CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_hdlc_proc (UINT16   usUserState,
							 VOID     *pvVar,
							 CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_ike_proc (UINT16   usUserState,
							VOID     *pvVar,
							CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_ima_proc (UINT16   usUserState,
							VOID     *pvVar,
							CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_l2tp_proc (UINT16   usUserState,
							 VOID     *pvVar,
							 CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_mcm_proc (UINT16   usUserState,
							VOID     *pvVar,
							CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_radius_proc (UINT16   usUserState,
							   VOID     *pvVar,
							   CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_routemng_proc (UINT16   usUserState,
								 VOID     *pvVar,
								 CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_tdm_proc (UINT16   usUserState,
							VOID     *pvVar,
							CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_chipmng_proc (UINT16   usUserState,
							VOID     *pvVar,
							CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_nodebmng_proc (UINT16   usUserState,
							VOID     *pvVar,
							CPSS_COM_MSG_HEAD_T  *pstMsgHead);
/*
 * 增加对gtpu-s, gtpu-g纤程入口函数的声明。
 * Added by Yang Jie. 2006-11-17.
 */
extern  VOID tnbs_gtpu_sgsn_proc (UINT16   usUserState,
								  VOID     *pvVar,
								  CPSS_COM_MSG_HEAD_T  *pstMsgHead);

extern  VOID tnbs_gtpu_ggsn_proc (UINT16   usUserState,
								  VOID     *pvVar,
								  CPSS_COM_MSG_HEAD_T  *pstMsgHead);

/*
 *	tzl
 *
 */
extern  VOID tnbs_host_gtpu_main (UINT16   usUserState,
								  VOID     *pvVar,
								  CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID tnbs_gtpu_sgsn_ps_simu_proc (UINT16   usUserState,
								  VOID     *pvVar,
								  CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID ups_host_css_simu_proc (UINT16   usUserState,
								  VOID     *pvVar,
								  CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern	void tnbs_simu_udp_proc(UINT16  usState, void *  pVar,CPSS_COM_MSG_HEAD_T *    ptHdr);


/* end of add. 2006-11-17. */
extern	void tnbs_rtpa_traffic_recv(UINT16  usState, void *  pVar,CPSS_COM_MSG_HEAD_T *    ptHdr);
/* added end ! */
extern	void tnbs_ftp_proc(UINT16  usState, void *  pVar,CPSS_COM_MSG_HEAD_T *    ptHdr);

extern	void tnbs_ppp_proc(UINT16  usState, void *  pVar,CPSS_COM_MSG_HEAD_T *    ptHdr);

/******************************** TNBS END   ***************************/

/******************************** TNSS BEGIN ***************************/
/* MTP2模块纤程入口 */
extern VOID tnss_mtp2_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* SSCOP模块纤程入口 */
extern VOID tnss_sscop_proc(UINT16 usState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* SSCF UNI模块纤程入口 */
extern VOID tnss_sscfuni_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* SSCF NNI模块纤程入口 */
extern VOID tnss_sscfnni_proc( UINT16 usUserState,void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* MTP3模块纤程入口 */
extern VOID tnss_mtp3b_smh_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_mtp3b_slm_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_mtp3b_spcm_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_mtp3b_pcc_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_mtp3b_tlac_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_mtp3b_tcoc_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_mtp3b_tcbc_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_mtp3b_tprc_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_mtp3b_conf_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_mtp3b_tfcrc_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_mtp3b_rtpcac_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* M3UA模块纤程入口 */
extern VOID tnss_m3ua_smah_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_m3ua_aspm_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* M3NIF模块纤程入口 */
extern VOID tnss_m3nif_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* SCCP模块纤程入口 */
extern VOID tnss_sccp_scrc_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_sccp_sclc_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_sccp_scoc_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_sccp_scmg_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_sccp_test_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* ALCAP模块纤程入口 */
extern VOID tnss_alcap_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* NF模块纤程入口 */
extern VOID tnss_nf_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* STC模块纤程入口 */
extern VOID tnss_stc_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* TCAP模块纤程入口 */
extern VOID tnss_tcap_tsl_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID tnss_tcap_csl_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/******************************** TNSS END ***************************/

/******************************** RDBSR BEGIN   **************************/

/******************************** RDBSR END     **************************/

/******************************** 宏和常量定义 *******************************/
     
/* RDBS 41~60 */                                                                         
#define CPS__RDBS_INIT_PROC                  CPSS_VK_PD(41,0)  /* RDBS初始化纤程*/            
#define CPS__RDBS_IF_PROC                    CPSS_VK_PD(42,0)  /* 接口纤程*/                  
#define CPS__RDBS_DCM_PROC                   CPSS_VK_PD(43,0)  /* 配置管理纤程*/              
#define CPS__RDBS_SDSYN_RX_PROC              CPSS_VK_PD(44,0)  /* 静态数据同步接收纤程*/      
#define CPS__RDBS_SDSYN_TX_PROC              CPSS_VK_PD(45,0)  /* 静态数据同步发送纤程*/      
#define CPS__RDBS_MSSYN_RX_PROC              CPSS_VK_PD(46,0)  /* 主备静态数据同步接收纤程*/  
#define CPS__RDBS_MSSYN_TX_PROC              CPSS_VK_PD(47,0)  /* 主备静态数据同步发送纤程*/  
#define CPS__RDBS_DDSYN_RX_PROC              CPSS_VK_PD(48,0)  /* 动态数据同步接收纤程*/      
#define CPS__RDBS_DDSYN_TX_PROC              CPSS_VK_PD(49,0)  /* 动态数据同步发送纤程*/      
#define CPS__RDBS_MDSYN_RX_PROC              CPSS_VK_PD(50,0)  /* 主备周期动态数据同步纤程*/  
#define CPS__RDBS_MDSYN_TX_PROC              CPSS_VK_PD(51,0)  /* 主备周期动态数据同步纤程*/  
#define CPS__RDBS_FILE_PROC                  CPSS_VK_PD(52,0)  /* 主备周期动态数据同步纤程*/  
#define CPS__RDBS_VERIFY_PROC                CPSS_VK_PD(53,0)  /* 纤程*/  

/* OAMS */
/* OAMS子系统纤程描述符定义
*   纤程类型范围 :  61 ~ 90
*   目前分配如下
*       主控纤程    61 ~ 70
*       保留        71 ~ 76
*       代理纤程    77 ~ 92
*       保留        83 ~ 90
*/
/* 主控模块的纤程描述符 */
#define CPS__OAMS_PD_M_AM_MNGR_RCV_PROC      CPSS_VK_PD(61, 0)   /* 告警管理主控接收 */
#define CPS__OAMS_PD_M_AM_MNGR_RPT_PROC      CPSS_VK_PD(62, 0)   /* 告警管理主控上报 */
#define CPS__OAMS_PD_M_COMM_PROC             CPSS_VK_PD(63, 0)   /* 前后台通信 */
#define CPS__OAMS_PD_M_FTM_PROC              CPSS_VK_PD(64, 0)   /* 文件传输管理 */
#define CPS__OAMS_PD_M_LOGM_PROC             CPSS_VK_PD(65, 0)   /* 日志管理 */
#define CPS__OAMS_PD_M_MSGDSP_PROC           CPSS_VK_PD(66, 0)   /* 消息分发 */
#define CPS__OAMS_PD_M_PM_MNGR_RCV_PROC      CPSS_VK_PD(67, 0)   /* 性能管理主控接收 */
#define CPS__OAMS_PD_M_PM_MNGR_RPT_PROC      CPSS_VK_PD(68, 0)   /* 性能管理主控上报 */
#define CPS__OAMS_PD_M_TM_PROC               CPSS_VK_PD(69, 0)   /* 事务管理 */
#define CPS__OAMS_PD_M_TRC_MNGR_PROC         CPSS_VK_PD(70, 0)   /* 跟踪管理主控 */
#define CPS__OAMS_PD_M_DIAG_MNGR_PROC        CPSS_VK_PD(71, 0)   /* 诊断测试管理主控 */
#define CPS__OAMS_PD_M_ABOX_MNGR_PROC        CPSS_VK_PD(72, 0)   /* 告警箱管理主控 */
#define CPS__OAMS_PD_M_ABOX_DRV_PROC         CPSS_VK_PD(73, 0)   /* 告警箱驱动 */
#define CPS__OAMS_PD_M_DLM_MSGDSP_PROC       CPSS_VK_PD(74, 0)   /* 网元直连管理消息分发 */
#define CPS__OAMS_PD_M_DLM_FS_PROC           CPSS_VK_PD(75, 0)   /* 网元直连管理文件服务 */

/* 代理模块的纤程描述符 */
#define CPS__OAMS_PD_A_AM_AGT_PROC           CPSS_VK_PD(77, 0)   /* 告警管理代理 */
#define CPS__OAMS_PD_A_DYND_AGT_PROC         CPSS_VK_PD(78, 0)   /* 动态数据代理 */
#define CPS__OAMS_PD_A_DTPRB_AGT_PROC        CPSS_VK_PD(79, 0)   /* 数据探针代理 */
#define CPS__OAMS_PD_A_PM_AGT_PROC           CPSS_VK_PD(80, 0)   /* 性能管理代理 */
#define CPS__OAMS_PD_A_SHCMD_AGT_PROC        CPSS_VK_PD(81, 0)   /* Shell命令代理 */
#define CPS__OAMS_PD_A_TRC_AGT_PROC          CPSS_VK_PD(82, 0)   /* 跟踪管理代理 */
#define CPS__OAMS_PD_A_LOG_AGT_PROC          CPSS_VK_PD(83, 0)   /* 日志管理代理 */
#define CPS__OAMS_PD_A_DIAG_AGT_PROC         CPSS_VK_PD(84, 0)   /* 诊断测试管理代理 */

/* CPS__OAMS_N侧计费模块(CDF)的纤程描述符*/
#define CPS__OAMS_N_PD_CDF_PROC              CPSS_VK_PD(85, 0)   /* CPS__OAMS_N侧计费模块 */

#define  CPS__TNBS_ATMLM_PROC                CPSS_VK_PD(131, 0) 
#define  CPS__TNBS_IMA_PROC                  CPSS_VK_PD(132, 0)
#define  CPS__TNBS_APS_PROC                  CPSS_VK_PD(133, 0)
#define  CPS__TNBS_TDM_PROC                  CPSS_VK_PD(134, 0)
#define  CPS__TNBS_IPSTACK_PROC              CPSS_VK_PD(135, 0)
#define  CPS__TNBS_IKE_PROC                  CPSS_VK_PD(136, 0)
#define  CPS__TNBS_L2TP_PROC                 CPSS_VK_PD(137, 0)
#define  CPS__TNBS_DNS_PROC                  CPSS_VK_PD(138, 0)
#define  CPS__TNBS_DHCPV4_PROC               CPSS_VK_PD(139, 0)
#define  CPS__TNBS_DHCPV6_PROC               CPSS_VK_PD(140, 0)
#define  CPS__TNBS_RADIUS_PROC               CPSS_VK_PD(141, 0)
#define  CPS__TNBS_OSPF_PROC                 CPSS_VK_PD(142, 0)
#define  CPS__TNBS_RIP_PROC                  CPSS_VK_PD(143, 0)
#define  CPS__TNBS_NSM_PROC                  CPSS_VK_PD(144, 0)
#define  CPS__TNBS_ROUTEMNG_PROC             CPSS_VK_PD(145, 0)
#define  CPS__TNBS_MCM_PROC                  CPSS_VK_PD(146, 0)
#define  CPS__TNBS_GTPU_PROC                 CPSS_VK_PD(147, 0)
#define  CPS__TNBS_CHIPMNG_PROC              CPSS_VK_PD(148, 0)
#define  CPS__TNBS_DSP_TRAFFIC_PROC          CPSS_VK_PD(149, 0)
#define  CPS__TNBS_OSPF6_PROC                CPSS_VK_PD(150, 0)
#define  CPS__TNBS_RIP6_PROC                 CPSS_VK_PD(151, 0)
#define  CPS__TNBS_PIM6_PROC                 CPSS_VK_PD(152, 0)
#define  CPS__TNBS_PPP_PROC                  CPSS_VK_PD(153, 0)
#define  CPS__TNBS_FTP_PROC                  CPSS_VK_PD(154, 0)
#define  CPS__TNBS_NODEBMNG_PROC             CPSS_VK_PD(155, 0)
#define  CPS__TNBS_PIM_PROC                  CPSS_VK_PD(156, 0)

/* OAMS Mngr */
#define CPS__OAMS_PD_M_AM_MNGR_RCV_PROCNAME  "pOamsAmMngrRecv"   /* 告警管理主控接收 */
#define CPS__OAMS_PD_M_AM_MNGR_RPT_PROCNAME  "pOamsAmMngrRpt"    /* 告警管理主控上报 */
#define CPS__OAMS_PD_M_COMM_PROCNAME         "pOamsComm"         /* 前后台通信 */
#define CPS__OAMS_PD_M_FTM_PROCNAME          "pOamsFtm"          /* 文件传输管理 */
#define CPS__OAMS_PD_M_LOGM_PROCNAME         "pOamsLogm"         /* 日志管理 */
#define CPS__OAMS_PD_M_MSGDSP_PROCNAME       "pOamsMsgDsp"       /* 消息分发 */
#define CPS__OAMS_PD_M_PM_MNGR_RCV_PROCNAME  "pOamsPmMngrRecv"   /* 性能管理主控接收 */
#define CPS__OAMS_PD_M_PM_MNGR_RPT_PROCNAME  "pOamsPmMngrRpt"    /* 性能管理主控上报 */
#define CPS__OAMS_PD_M_TM_PROCNAME           "pOamsTm"           /* 事务管理 */
#define CPS__OAMS_PD_M_TRC_MNGR_PROCNAME     "pOamsTrcMngr"      /* 跟踪管理主控 */
#define CPS__OAMS_PD_M_DIAG_MNGR_PROCNAME    "pOamsDiagMngr"     /* 诊断测试管理主控 */
#define CPS__OAMS_PD_M_ABOX_MNGR_PROCNAME    "pOamsABoxMngr"     /* 告警箱管理主控 */
#define CPS__OAMS_PD_M_ABOX_DRV_PROCNAME     "pOamsABoxDrv"      /* 告警箱驱动 */
#define CPS__OAMS_PD_M_DLM_MSGDSP_PROCNAME   "pOamsDlmMsgDsp"      /* 网元直连管理消息分发 */
#define CPS__OAMS_PD_M_DLM_FS_PROCNAME       "pOamsDlmFs"      /* 网元直连管理文件服务 */

/* OAMS Agt */
#define CPS__OAMS_PD_A_AM_AGT_PROCNAME       "pOamsAmAgt"        /* 告警管理代理 */
#define CPS__OAMS_PD_A_DYND_AGT_PROCNAME     "pOamsDyndAgt"      /* 动态数据代理 */
#define CPS__OAMS_PD_A_DTPRB_AGT_PROCNAME    "pOamsDtprbAgt"     /* 数据探针代理 */
#define CPS__OAMS_PD_A_PM_AGT_PROCNAME       "pOamsPmAgt"        /* 性能管理代理 */
#define CPS__OAMS_PD_A_SHCMD_AGT_PROCNAME    "pOamsShcmdAgt"     /* Shell命令代理 */
#define CPS__OAMS_PD_A_TRC_AGT_PROCNAME      "pOamsTrcAgt"       /* 跟踪管理代理 */
#define CPS__OAMS_PD_A_LOG_AGT_PROCNAME      "pOamsLomAgt"       /* 日志管理代理 */
#define CPS__OAMS_PD_A_DIAG_AGT_PROCNAME     "pOamsDiagAgt"      /* 诊断测试管理代理 */

/*RDBS*/
#define CPS__RDBS_INIT_PROCNAME              "pRdbsInit"
#define CPS__RDBS_SDSYNRX_PROCNAME           "pRdbsSdsynRx"
#define CPS__RDBS_SDSYNTX_PROCNAME           "pRdbsSdsynTx"
#define CPS__RDBS_MSSYNRX_PROCNAME           "pRdbsMssynRx"
#define CPS__RDBS_MSSYNTX_PROCNAME           "pRdbsMssynTx"
#define CPS__RDBS_DDSYNRX_PROCNAME           "pRdbsDdsynRx"
#define CPS__RDBS_DDSYNTX_PROCNAME           "pRdbsDdsynTx"
#define CPS__RDBS_MDSYNRX_PROCNAME           "pRdbsMdsynRx"
#define CPS__RDBS_MDSYNTX_PROCNAME           "pRdbsMdsynTx"
#define CPS__RDBS_IF_PROCNAME                "pRdbsIf"
#define CPS__RDBS_DCM_PROCNAME               "pRdbsDcm"
#define CPS__RDBS_SGSN_NOTIFY_PROCNAME       "pRdbsSgsnNotify"
#define CPS__RDBS_GGSN_NOTIFY_PROCNAME       "pRdbsGgsnNotify"
#define CPS__RDBS_FILE_PROCNAME              "pRdbsFile"
#define CPS__RDBS_VERIFY_PROCNAME            "pRdbsVerify"

#define CPS__TNBS_IMA_PROCNAME               "pTnbsIma"
#define CPS__TNBS_ATMLM_PROCNAME             "pTnbsAtmLm"
#define CPS__TNBS_APS_PROCNAME               "pTnbsAps"
#define CPS__TNBS_TDM_PROCNAME               "pTnbsTdm"
#define CPS__TNBS_IPSTACK_PROCNAME           "pTnbsIpStack"
#define CPS__TNBS_ROUTEMNG_PROCNAME          "pTnbsRouteMng"
#define CPS__TNBS_IKE_PROCNAME               "pTnbsIke"
#define CPS__TNBS_GTPU_PROCNAME              "pTnbsGtpu"
#define CPS__TNBS_L2TP_PROCNAME              "pTnbsL2tp"
#define CPS__TNBS_MCM_PROCNAME               "pTnbsMcm"
#define CPS__TNBS_RADIUS_PROCNAME            "pTnbsRadius"
#define CPS__TNBS_DNS_PROCNAME               "pTnbsDns"
#define CPS__TNBS_DHCPV4_PROCNAME            "pTnbsDhcp4"
#define CPS__TNBS_DHCPV6_PROCNAME            "pTnbsDhcp6"
#define CPS__TNBS_OSPF_PROCNAME              "pTnbsOspf"
#define CPS__TNBS_RIP_PROCNAME               "pTnbsRip"
#define CPS__TNBS_NSM_PROCNAME               "pTnbsNsm"
#define CPS__TNBS_SIMU2_PROCNAME             "pTnbsSimu2"
#define CPS__TNBS_SIMU3_PROCNAME             "pTnbsSimu3"
#define CPS__TNBS_SIMU4_PROCNAME             "pTnbsSimu4"
#define CPS__TNBS_CHIPMNG_PROCNAME           "pTnbsChipMng"
#define CPS__TNBS_DSP_TRAFFIC_PROCNAME       "pTnbsDspTraffic"
#define CPS__TNBS_FTP_PROCNAME               "pTnbsFtp"
#define CPS__TNBS_NODEBMNG_PROCNAME          "pTnbsNodebMng"
#define CPS__TNBS_PPP_PROCNAME               "pTnbsPPP"
#define CPS__TNBS_PIM_PROCNAME               "pTnbsPim"

/* ... */

/* 平台部分的模块编号定义(子系统内统一编号，范围1~64) */
#define SWP_MODULE_ALL          255                 /* 所有模块 */
#define SWP_MODULE_MAX_NUM      96                  /* 模块最大个数 */

#define  CPS__RDBS_MODULE_KNL        ((UINT8)(0x01)) 
#define  CPS__RDBS_MODULE_DCM        ((UINT8)(0x02))                                                         
#define  CPS__RDBS_MODULE_DDSYN      ((UINT8)(0x03))                                                           
#define  CPS__RDBS_MODULE_MDSYN      ((UINT8)(0x04)) 
#define  CPS__RDBS_MODULE_SDSYN      ((UINT8)(0x05)) 
#define  CPS__RDBS_MODULE_MSSYN      ((UINT8)(0x06)) 
#define  CPS__RDBS_MODULE_IF         ((UINT8)(0x07)) 
#define  CPS__RDBS_MODULE_INIT       ((UINT8)(0x08)) 
#define  CPS__RDBS_MODULE_PROBE      ((UINT8)(0x09)) 
#define  CPS__RDBS_MODULE_DEV        ((UINT8)(0x0A)) 
#define  CPS__RDBS_MODULE_ATM        ((UINT8)(0x0B)) 
#define  CPS__RDBS_MODULE_IPOA       ((UINT8)(0x0C)) 
#define  CPS__RDBS_MODULE_IP         ((UINT8)(0x0D)) 
#define  CPS__RDBS_MODULE_N7         ((UINT8)(0x0E)) 
#define  CPS__RDBS_MODULE_SIGTRAN    ((UINT8)(0x0F))                                                       
#define  CPS__RDBS_MODULE_COMMON     ((UINT8)(0x10)) 
#define  CPS__RDBS_MODULE_DEBUG      ((UINT8)(0x11))  
/* OAMS代理模块编号 */
#define CPS__OAMS_MDLNO_CF           ((UINT8)(0x01))     /* 公共功能 */
#define CPS__OAMS_MDLNO_AM_AGT       ((UINT8)(0x02))     /* 告警管理代理 */
#define CPS__OAMS_MDLNO_DYND_AGT     ((UINT8)(0x03))     /* 动态数据代理 */
#define CPS__OAMS_MDLNO_DTPRB_AGT    ((UINT8)(0x04))     /* 数据探针代理 */
#define CPS__OAMS_MDLNO_PM_AGT       ((UINT8)(0x05))     /* 性能管理代理 */
#define CPS__OAMS_MDLNO_SHCMD_AGT    ((UINT8)(0x06))     /* Shell命令代理 */
#define CPS__OAMS_MDLNO_TRC_AGT      ((UINT8)(0x07))     /* 跟踪管理代理 */
#define CPS__OAMS_MDLNO_LOG_AGT      ((UINT8)(0x08))     /* 日志管理代理 */
#define CPS__OAMS_MDLNO_DIAG_AGT     ((UINT8)(0x09))     /* 诊断测试管理代理 */

/* OAMS主控模块编号 */
#define CPS__OAMS_MDLNO_AM_MNGR      ((UINT8)(0x14))     /* 告警管理主控 */
#define CPS__OAMS_MDLNO_COMM         ((UINT8)(0x15))     /* 前后台通信 */
#define CPS__OAMS_MDLNO_FTM          ((UINT8)(0x16))     /* 文件传输管理 */
#define CPS__OAMS_MDLNO_LOGM         ((UINT8)(0x17))     /* 日志管理 */
#define CPS__OAMS_MDLNO_MSGDSP       ((UINT8)(0x18))     /* 消息分发 */
#define CPS__OAMS_MDLNO_PM_MNGR      ((UINT8)(0x05))     /* 性能管理主控 */
#define CPS__OAMS_MDLNO_TM           ((UINT8)(0x1a))     /* 事务管理 */
#define CPS__OAMS_MDLNO_TRC_MNGR     ((UINT8)(0x1b))     /* 跟踪管理主控 */
#define CPS__OAMS_MDLNO_DIAG_MNGR    ((UINT8)(0x1c))     /* 诊断测试管理主控 */
#define CPS__OAMS_MDLNO_ABOX_MNGR    ((UINT8)(0x1d))     /* 告警箱管理主控 */
#define CPS__OAMS_MDLNO_ABOX_DRV     ((UINT8)(0x1e))     /* 告警箱驱动 */
#define CPS__OAMS_MDLNO_DLM          ((UINT8)(0x1f))     /* 网元直连管理模块 */

/* 计费模块编号 */
#define CPS__OAMS_N_MODULE_CDF       ((UINT8)(0x01))     

/* TNBS */
#define CPS__TNBS_APS_MODULE                 ((UINT8)(0x01))
#define CPS__TNBS_ATMLM_MODULE               ((UINT8)(0x02))
#define CPS__TNBS_IMA_MODULE                 ((UINT8)(0x03))
#define CPS__TNBS_TDM_MODULE                 ((UINT8)(0x04))
#define CPS__TNBS_HDLC_MODULE                ((UINT8)(0x05))
#define CPS__TNBS_AAL2_MODULE                ((UINT8)(0x06))
#define CPS__TNBS_AAL5_MODULE                ((UINT8)(0x07))
#define CPS__TNBS_CHIPMNG_MOUDULE            ((UINT8)(0x08))
#define CPS__TNBS_L2TP_MODULE                ((UINT8)(0x09))
#define CPS__TNBS_RADIUS_MODULE              ((UINT8)(0x0a))
#define CPS__TNBS_DNS_MODULE                 ((UINT8)(0x0b))
#define CPS__TNBS_DHCP_MODULE                ((UINT8)(0x0c))
#define CPS__TNBS_DHCPV6_MODULE              ((UINT8)(0x0d))
#define CPS__TNBS_IKE_MODULE                 ((UINT8)(0x0e))
#define CPS__TNBS_PPP_MODULE                 ((UINT8)(0x0f))
#define CPS__TNBS_MLPPP_MODULE               ((UINT8)(0x10))
#define CPS__TNBS_UCOM_MODULE                ((UINT8)(0x11))
#define CPS__TNBS_MCTM_MODULE                ((UINT8)(0x12))
#define CPS__TNBS_GTPU_MODULE                ((UINT8)(0x13))
#define CPS__TNBS_GTPU_R_MODULE              ((UINT8)(0x14))
#define CPS__TNBS_GTPU_S_MODULE              ((UINT8)(0x15))
#define CPS__TNBS_GTPU_G_MODULE              ((UINT8)(0x16))
#define CPS__TNBS_RTCP_MODULE                ((UINT8)(0x17))
#define CPS__TNBS_SOCKET_MODULE              ((UINT8)(0x18))
#define CPS__TNBS_TCP_MODULE                 ((UINT8)(0x19))
#define CPS__TNBS_SCTP_MODULE                ((UINT8)(0x1a))
#define CPS__TNBS_UDP_MODULE                 ((UINT8)(0x1b))
#define CPS__TNBS_ARP_MODULE                 ((UINT8)(0x1c))
#define CPS__TNBS_ND_MODULE                  ((UINT8)(0x1d))
#define CPS__TNBS_ICMP_MODULE                ((UINT8)(0x1e))
#define CPS__TNBS_ICMPV6_MODULE              ((UINT8)(0x1f))
#define CPS__TNBS_IPV4_MODULE                ((UINT8)(0x20))
#define CPS__TNBS_IPV6_MODULE                ((UINT8)(0x21))
#define CPS__TNBS_IGMP_MODULE                ((UINT8)(0x22))
#define CPS__TNBS_MLD_MODULE                 ((UINT8)(0x23))
#define CPS__TNBS_IPOA_MODULE                ((UINT8)(0x24))
#define CPS__TNBS_IF_MODULE                  ((UINT8)(0x25))
#define CPS__TNBS_ROUTE_MODULE               ((UINT8)(0x26))
#define CPS__TNBS_NSM_MODULE                 ((UINT8)(0x27))
#define CPS__TNBS_OSPFV2_MODULE              ((UINT8)(0x28))
#define CPS__TNBS_OSPFV3_MODULE              ((UINT8)(0x29))
#define CPS__TNBS_RIPV2_MODULE               ((UINT8)(0x2a))
#define CPS__TNBS_RIPV3_MODULE               ((UINT8)(0x2b))
#define CPS__TNBS_PIMV4_MODULE               ((UINT8)(0x2c))
#define CPS__TNBS_PIMV6_MODULE               ((UINT8)(0x2d))
#define CPS__TNBS_IPIP_MODULE                ((UINT8)(0x2e))
#define CPS__TNBS_NAT_MODULE                 ((UINT8)(0x2f))
#define CPS__TNBS_ACL_MODULE                 ((UINT8)(0x30))
#define CPS__TNBS_AH_MODULE                  ((UINT8)(0x31))
#define CPS__TNBS_ESP_MODULE                 ((UINT8)(0x32))
#define CPS__TNBS_DSCP_MODULE                ((UINT8)(0x33))
#define CPS__TNBS_RTP_MODULE                 ((UINT8)(0x34))
#define CPS__TNBS_E1TTP_MODULE               ((UINT8)(0x35))
#define CPS__TNBS_DSPTTP_MODULE              ((UINT8)(0x36))
#define CPS__TNBS_COM_MODULE                 ((UINT8)(0x37))
#define CPS__TNBS_DBG_MODULE                 ((UINT8)(0x38))
#define CPS__TNBS_FTP_MODULE                 ((UINT8)(0x39))
#define CPS__TNBS_NODEBMNG_MODULE            ((UINT8)(0x3a))

#define  CPS__RDBS_MODULENAME_KNL            "KNL"
#define  CPS__RDBS_MODULENAME_DCM            "DCM"
#define  CPS__RDBS_MODULENAME_DDSYN          "DDSYN"
#define  CPS__RDBS_MODULENAME_MDSYN          "MDSYN"
#define  CPS__RDBS_MODULENAME_SDSYN          "SDSYN"
#define  CPS__RDBS_MODULENAME_MSSYN          "MSSYN"
#define  CPS__RDBS_MODULENAME_IF             "IF"
#define  CPS__RDBS_MODULENAME_INIT           "INIT"
#define  CPS__RDBS_MODULENAME_PROBE          "PROBE"
#define  CPS__RDBS_MODULENAME_DEV            "DEV"
#define  CPS__RDBS_MODULENAME_ATM            "ATM"
#define  CPS__RDBS_MODULENAME_IPOA           "IPOA"
#define  CPS__RDBS_MODULENAME_IP             "IP"
#define  CPS__RDBS_MODULENAME_N7             "N7"
#define  CPS__RDBS_MODULENAME_SIGTRAN        "SIGTRAN"
#define  CPS__RDBS_MODULENAME_COMMON         "COMMON"
#define  CPS__RDBS_MODULENAME_DEBUG          "DEBUG"
/* OAMS代理模块名称 */
#define CPS__OAMS_MDLNAME_CF                 "CF"        /* 公共功能 */
#define CPS__OAMS_MDLNAME_AM_AGT             "AM_AGT"    /* 告警管理代理 */
#define CPS__OAMS_MDLNAME_DYND_AGT           "DYND_AGT"  /* 动态数据代理 */
#define CPS__OAMS_MDLNAME_DTPRB_AGT          "DTPRB_AGT" /* 数据探针代理 */
#define CPS__OAMS_MDLNAME_PM_AGT             "PM_AGT"    /* 性能管理代理 */
#define CPS__OAMS_MDLNAME_SHCMD_AGT          "SHCMD_AGT" /* Shell命令代理 */
#define CPS__OAMS_MDLNAME_TRC_AGT            "TRC_AGT"   /* 跟踪管理代理 */
#define CPS__OAMS_MDLNAME_LOG_AGT            "LOG_AGT"       /* 日志管理代理 */
#define CPS__OAMS_MDLNAME_DIAG_AGT           "DIAG_AGT"      /* 诊断测试管理代理 */

/* OAMS主控模块名称 */
#define CPS__OAMS_MDLNAME_AM_MNGR            "AM_MNGR"   /* 告警管理主控 */
#define CPS__OAMS_MDLNAME_COMM               "COMM"      /* 前后台通信 */
#define CPS__OAMS_MDLNAME_FTM                "FTM"       /* 文件传输管理 */
#define CPS__OAMS_MDLNAME_LOGM               "LOGM"      /* 日志管理 */
#define CPS__OAMS_MDLNAME_MSGDSP             "MSGDSP"    /* 消息分发 */
#define CPS__OAMS_MDLNAME_PM_MNGR            "PM_MNGR"   /* 性能管理主控 */
#define CPS__OAMS_MDLNAME_TM                 "TM"        /* 事务管理 */
#define CPS__OAMS_MDLNAME_TRC_MNGR           "TRC_MNGR"  /* 跟踪管理主控 */
#define CPS__OAMS_MDLNAME_DIAG_MNGR          "DIAG_MNGR"     /* 诊断测试管理主控 */
#define CPS__OAMS_MDLNAME_ABOX_MNGR          "ABOX_MNGR" /* 告警箱管理主控 */
#define CPS__OAMS_MDLNAME_ABOX_DRV           "ABOX_DRV"  /* 告警箱驱动 */


#define CPS__OAMS_MDLNAME_DLM                "DLM"  /* 网元直连管理模块 */

/* CPS__OAMS_N */
#define CPS__OAMS_N_MDLNAME_CDF              "CDF"       /* 计费模块 */

/* TNBS */
#define CPS__TNBS_APSNAME_MODULE                     "APS"    
#define CPS__TNBS_ATMLMNAME_MODULE                   "ATMLM"  
#define CPS__TNBS_IMANAME_MODULE                     "IMA"    
#define CPS__TNBS_TDMNAME_MODULE                     "TDM"    
#define CPS__TNBS_HDLCNAME_MODULE                    "HDLC"   
#define CPS__TNBS_AAL2NAME_MODULE                    "AAL2"   
#define CPS__TNBS_AAL5NAME_MODULE                    "AAL5"   
#define CPS__TNBS_CHIPMNGNANE_MODULE                 "CHIPMNG"
#define CPS__TNBS_L2TPNAME_MODULE                    "L2TP"   
#define CPS__TNBS_RADIUSNAME_MODULE                  "RADIUS" 
#define CPS__TNBS_DNSNAME_MODULE                     "DNS"    
#define CPS__TNBS_DHCPNAME_MODULE                    "DHCP"   
#define CPS__TNBS_DHCPV6NAME_MODULE                  "DHCPV6" 
#define CPS__TNBS_IKENAME_MODULE                     "IKE"    
#define CPS__TNBS_PPPNAME_MODULE                     "PPP"    
#define CPS__TNBS_MLPPPNAME_MODULE                   "MLPPP"  
#define CPS__TNBS_UCOMNAME_MODULE                    "UCOM"   
#define CPS__TNBS_MCTMNAME_MODULE                    "MCTM"   
#define CPS__TNBS_GTPUNAME_MODULE                    "GTPU"   
#define CPS__TNBS_GTPU_RNAME_MODULE                  "GTPU_R" 
#define CPS__TNBS_GTPU_SNAME_MODULE                  "GTPU_S" 
#define CPS__TNBS_GTPU_GNAME_MODULE                  "GTPU_G" 
#define CPS__TNBS_RTCPNAME_MODULE                    "RTCP"   
#define CPS__TNBS_SOCKETNAME_MODULE                  "SOCKET" 
#define CPS__TNBS_TCPNAME_MODULE                     "TCP"    
#define CPS__TNBS_SCTPNAME_MODULE                    "SCTP"   
#define CPS__TNBS_UDPNAME_MODULE                     "UDP"    
#define CPS__TNBS_ARPNAME_MODULE                     "ARP"    
#define CPS__TNBS_NDNAME_MODULE                      "ND"     
#define CPS__TNBS_ICMPNAME_MODULE                    "ICMP"   
#define CPS__TNBS_ICMPV6NAME_MODULE                  "ICMPV6" 
#define CPS__TNBS_IPV4NAME_MODULE                    "IPV4"   
#define CPS__TNBS_IPV6NAME_MODULE                    "IPV6"   
#define CPS__TNBS_IGMPNAME_MODULE                    "IGMP"   
#define CPS__TNBS_MLDNAME_MODULE                     "MLD"    
#define CPS__TNBS_IPOANAME_MODULE                    "IPOA"   
#define CPS__TNBS_IFNAME_MODULE                      "IF"     
#define CPS__TNBS_ROUTENAME_MODULE                   "ROUTE"  
#define CPS__TNBS_NSMNAME_MODULE                     "NSM"    
#define CPS__TNBS_OSPFV2NAME_MODULE                  "OSPFV2" 
#define CPS__TNBS_OSPFV3NAME_MODULE                  "OSPFV3" 
#define CPS__TNBS_RIPV2NAME_MODULE                   "RIPV2"  
#define CPS__TNBS_RIPV3NAME_MODULE                   "RIPV3"  
#define CPS__TNBS_PIMV4NAME_MODULE                   "PIMV4"  
#define CPS__TNBS_PIMV6NAME_MODULE                   "PIMV6"  
#define CPS__TNBS_IPIPNAME_MODULE                    "IPIP"   
#define CPS__TNBS_NATNAME_MODULE                     "NAT"    
#define CPS__TNBS_ACLNAME_MODULE                     "ACL"    
#define CPS__TNBS_AHNAME_MODULE                      "AH"     
#define CPS__TNBS_ESPNAME_MODULE                     "ESP"    
#define CPS__TNBS_DSCPNAME_MODULE                    "DSCP"   
#define CPS__TNBS_RTPNAME_MODULE                     "RTP"    
#define CPS__TNBS_E1TTPNAME_MODULE                   "E1TTP"  
#define CPS__TNBS_DSPTTPAME_MODULE                   "DSPTTP"    
#define CPS__TNBS_COMNAME_MODULE                     "COM"    
#define CPS__TNBS_DBGNAME_MODULE                     "DBG"    
#define CPS__TNBS_FTPNAME_MODULE                     "FTP"    


/******************************** 全局变量声明 *******************************/

extern VOID cps__rdbs_file_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

/******************************** SMSS END   ***************************/

/******************************** OAMS BEGIN ***************************/
/* 主控模块的纤程入口 */
/* 告警管理主控接收 */
VOID cps__oams_am_mngr_rcv_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 告警管理主控上报 */
VOID cps__oams_am_mngr_rpt_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 前后台通信 */
VOID cps__oams_comm_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 文件传输管理 */
VOID cps__oams_ftm_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 日志管理 */
VOID cps__oams_logm_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 消息分发 */
VOID cps__oams_msgdsp_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 性能管理主控接收 */
VOID cps__oams_pm_mngr_recvdata_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 性能管理主控上报 */
VOID cps__oams_pm_mngr_rptdata_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 事务管理 */
VOID cps__oams_tm_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 跟踪管理主控 */
VOID cps__oams_trc_mngr_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 诊断测试管理主控 */
VOID cps__oams_diag_mngr_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 告警箱管理主控 */
VOID cps__oams_abox_mngr_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 网元直连管理消息分发 */
VOID cps__oams_dlm_msgdsp_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 网元直连管理文件服务 */
VOID cps__oams_dlm_fs_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 告警箱驱动 */
VOID cps__oams_abox_drv_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);

/* 代理模块的纤程入口 */
/* 告警管理代理 */
VOID cps__oams_am_agt_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 动态数据代理 */
VOID cps__oams_dynd_agt_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 数据探针代理 */
VOID cps__oams_dtprb_agt_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 性能管理代理 */
VOID cps__oams_pm_agt_proc(UINT16 usUserState, VOID * pvVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* Shell命令代理 */
VOID cps__oams_shcmd_agt_proc(UINT16 usUserState, VOID * pVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 跟踪管理代理 */
VOID cps__oams_trc_agt_proc(UINT16 usUserState, VOID * pvVar, CPSS_COM_MSG_HEAD_T * pstMsgHead);
/* 诊断测试管理代理 */
VOID cps__oams_diag_agt_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/* 日志管理代理 */
VOID cps__oams_log_agt_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);


/******************************** OAMS END   ***************************/

/******************************** CPS__OAMS_N BEGIN***************************/
/* CDF模块 */
VOID cps__oams_n_cdf_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

/******************************** CPS__OAMS_N END  ***************************/


/******************************** RDBS BEGIN ***************************/
extern VOID cps__rdbs_if_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID cps__rdbs_dcm_proc(UINT16 usUserState, VOID* pvVar, CPSS_COM_MSG_HEAD_T* pstMsgHead);
extern VOID cps__rdbs_init_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

extern VOID cps__rdbs_sdsyn_rx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID cps__rdbs_sdsyn_tx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

extern VOID cps__rdbs_mssyn_rx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID cps__rdbs_mssyn_tx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

extern VOID cps__rdbs_ddsyn_rx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID cps__rdbs_ddsyn_tx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

extern VOID cps__rdbs_mdsyn_rx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID cps__rdbs_mdsyn_tx_proc(UINT16 usUserState, void *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

extern VOID cps__rdbs_verify_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
/******************************** RDBS END   ***************************/

/******************************** TNBS BEGIN ***************************/
extern  VOID cps__tnbs_aps_proc (UINT16   usUserState, 
                            VOID     *pvVar, 
                            CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID cps__tnbs_atmlm_proc (UINT16   usUserState, 
                              VOID     *pvVar, 
                              CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID cps__tnbs_dhcpv4_proc (UINT16   usUserState, 
                               VOID     *pvVar, 
                               CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID cps__tnbs_dns_proc (UINT16   usUserState, 
                             VOID     *pvVar, 
                             CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID cps__tnbs_gtpu_proc (UINT16   usUserState, 
                             VOID     *pvVar, 
                             CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID cps__tnbs_hdlc_proc (UINT16   usUserState, 
                             VOID     *pvVar, 
                             CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID cps__tnbs_ike_proc (UINT16   usUserState, 
                            VOID     *pvVar, 
                            CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID cps__tnbs_ima_proc (UINT16   usUserState, 
                            VOID     *pvVar, 
                            CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern void cps__tnbs_ipstack_proc(UINT16 ulState, 
					          VOID   *pVar,
					          CPSS_COM_MSG_HEAD_T *pstMsgHead);   
extern  VOID cps__tnbs_l2tp_proc (UINT16   usUserState, 
                             VOID     *pvVar, 
                             CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID cps__tnbs_mcm_proc (UINT16   usUserState, 
                            VOID     *pvVar, 
                            CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID cps__tnbs_radius_proc (UINT16   usUserState, 
                               VOID     *pvVar, 
                               CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID cps__tnbs_routemng_proc (UINT16   usUserState, 
                                 VOID     *pvVar, 
                                 CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID cps__tnbs_tdm_proc (UINT16   usUserState, 
                            VOID     *pvVar, 
                            CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID cps__tnbs_chipmng_proc (UINT16   usUserState, 
                            VOID     *pvVar, 
                            CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID cps__tnbs_nodebmng_proc (UINT16   usUserState, 
                            VOID     *pvVar, 
                            CPSS_COM_MSG_HEAD_T  *pstMsgHead);
/* 
 * 增加对gtpu-s, gtpu-g纤程入口函数的声明。
 * Added by Yang Jie. 2006-11-17.
 */
extern  VOID cps__tnbs_gtpu_sgsn_proc (UINT16   usUserState, 
                                  VOID     *pvVar, 
                                  CPSS_COM_MSG_HEAD_T  *pstMsgHead);

extern  VOID cps__tnbs_gtpu_ggsn_proc (UINT16   usUserState, 
                                  VOID     *pvVar, 
                                  CPSS_COM_MSG_HEAD_T  *pstMsgHead);

/*
 *	tzl
 *
 */
extern  VOID cps__tnbs_host_gtpu_main (UINT16   usUserState, 
                                  VOID     *pvVar, 
                                  CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID cps__tnbs_gtpu_sgsn_ps_simu_proc (UINT16   usUserState, 
                                  VOID     *pvVar, 
                                  CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern  VOID ups_host_css_simu_proc (UINT16   usUserState, 
                                  VOID     *pvVar, 
                                  CPSS_COM_MSG_HEAD_T  *pstMsgHead);
extern	void cps__tnbs_simu_udp_proc(UINT16  usState, void *  pVar,CPSS_COM_MSG_HEAD_T *    ptHdr);


/* end of add. 2006-11-17. */

extern	void cps__tnbs_rtpa_traffic_recv(UINT16  usState, void *  pVar,CPSS_COM_MSG_HEAD_T *    ptHdr);
/* added end ! */
extern	void cps__tnbs_ftp_proc(UINT16  usState, void *  pVar,CPSS_COM_MSG_HEAD_T *    ptHdr);

extern	void cps__tnbs_ppp_proc(UINT16  usState, void *  pVar,CPSS_COM_MSG_HEAD_T *    ptHdr);

/******************************** TNBS END   ***************************/

VOID om_if_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgBuff);
extern void ac_rdbsac_proc(UINT16 usState,void* pVar,CPSS_COM_MSG_HEAD_T* pstMsgHead);

/******************************** 头文件保护结尾 *****************************/
#ifdef  __cplusplus
}
#endif

#if 0

/*liumingxia cpm ccu begin*/
/*add for AC begin */
#define SWP_SUBSYS_AC         15    /* AC��ϵͳ */
/*add for AC end */

#define  AC_MODULE_CAPWAP				((UINT8)(0x01))
#define  AC_MODULE_8021X				((UINT8)(0x02))
#define  AC_MODULE_CHARGE				((UINT8)(0x03))
#define  AC_MODULE_WAPI					((UINT8)(0x04))
#define  AC_MODULE_RADIUS				((UINT8)(0x05))
#define  AC_MODULE_PORTAL				((UINT8)(0x06))
#define  AC_MODULE_PUB					((UINT8)(0x08))
#define  AC_MODULE_PPPOE				((UINT8)(0x0b))
#define  AC_MODULE_CPMCC				((UINT8)(0x0c))
#define  AC_MODULE_DHCP					((UINT8)(0x0d))

#define  AC_MODULENAME_CAPWAP			"CAPWAPM"
#define  AC_MODULENAME_8021X			"8021XM"
#define  AC_MODULENAME_CHARGE			"ACCHGM"
#define  AC_MODULENAME_WAPI				"WAPIM"
#define  AC_MODULENAME_RADIUS			"ACRDUSM"
#define  AC_MODULENAME_PORTAL			"PORTALM"
#define  AC_MODULENAME_PUB				"ACPUBM"
#define  AC_MODULENAME_PPPOE			"PPPOEM"
#define  AC_MODULENAME_CPMCC			"CPMCCM"
#define  AC_MODULENAME_DHCP				"DHCPM"

/*liumingxia cpm ccu end*/

/*limeiying upm ppu begin*/

/* ģ���Ŷ���(��ϵͳ��ͳһ��ţ���Χ1~64) */
#define UPM_MODULE_CCM			((UINT8)(0x1))  /* CCMģ�� */
#define UPM_MODULE_DPM			((UINT8)(0x2))  /* DPMģ�� */
#define UPM_MODULE_IPSTACK		((UINT8)(0x3))  /* IPSTACKģ�� */
#define UPM_MODULE_NAT			((UINT8)(0x4))  /* NATģ�� */
#define UPM_MODULE_USRM			((UINT8)(0x5))  /* USRMģ�� */
#define UPM_MODULE_UTM			((UINT8)(0x6))  /* UTMģ�� */

/* ģ�����(������20�ַ�, ������ϵͳǰ׺) */
#define UPM_MODULENAME_CCM			"CCM"
#define UPM_MODULENAME_DPM			"DPM"
#define UPM_MODULENAME_IPSTACK		"IPSTACK"
#define UPM_MODULENAME_NAT			"NAT"
#define UPM_MODULENAME_USRM			"USRM"
#define UPM_MODULENAME_UTM			"UTM"

#endif

#endif /* SWP_COMMON_STR_H */
/******************************** 头文件结束 *********************************/

