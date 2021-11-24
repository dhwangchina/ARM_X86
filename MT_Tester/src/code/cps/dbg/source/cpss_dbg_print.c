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
#include "smss_public.h"
#include "cpss_common.h"
#include "cpss_err.h"
#include "cpss_vos_sem.h"
#include "cpss_dbg_log.h"
#include "cpss_dbg_print.h"
#include "cpss_com_trace.h"
#include "cpss_com_link.h"
#include "cpss_dbg_diagn.h"
/******************************* 局部宏定义 ***********************************/
/*  2006/09/19 李军增加*/

#define CPSS_DBG_DIAGN_MSGID_RSP   (0x731ffffe)

#ifndef CPSS_PRINT_FLAG
 #define CPSS_PRINT_FLAG CPSS_PRINT_OFF
#endif 

#define CPSS_MAX_MODULE_NAME_LENGTH  20

/* 打印设置结构*/
typedef struct CPSS_CPS__OAMS_PRT_LEVEL_SET
{
  UINT8 ucSubSystemID;
  UINT8 ucModuleID;
  UINT8 ucPrintLevel;

}CPSS_CPS__OAMS_PRT_LEVEL_SET_T;

/* 重定向结构*/
typedef struct CPSS_CPS__OAMS_REDIRECT_SET
{
  UINT32 ulPrintID;
  CPSS_COM_PID_T stPid;


}CPSS_CPS__OAMS_REDIRECT_SET_T;
/* 打印速率设置*/
typedef struct CPSS_CPS__OAMS_PRT_RATE_SET
{
  UINT32 ulRate;

}CPSS_CPS__OAMS_PRT_RATE_SET_T;
/* 日志速率设置*/
typedef struct CPSS_CPS__OAMS_LOG_RATE_SET
{
  UINT32 ulRate ;
}CPSS_CPS__OAMS_LOG_RATE_SET_T;
/* 日志等级设置*/
typedef struct CPSS_CPS__OAMS_LOG_LEVEL_SET
{
  UINT8 ucLogLevel;
}CPSS_CPS__OAMS_LOG_LEVEL_SET_T;
#define CPSS_SBBR_WAITRSP_TIMER_OUT_MSG  CPSS_TIMER_05_MSG
/******************************* 全局变量定义/初始化 **************************/
 CPSS_PRINT_MANAGE_T g_stCpssDbgPrtManage = {CPSS_PRINT_FLAG};       /* 打印相关管理信息 */ 
 CPSS_COM_PHY_ADDR_T g_stCpssDbgPhyAddr;         /* 本板物理地址 */
 BOOL g_bCpssDbgPrtValid = FALSE;                /* 打印是否可用标识 */
 CPSS_COM_PID_T *g_pstCpssDbgPrtDstPid = NULL;     /* 打印上报PID指针，初始为本板标准输出 */

 BOOL g_bCpssRecvProcEndInd = FALSE;

extern BOOL g_bCpssSbbrExcOccur;

#if 0
/* 存储子系统名 */
 STRING g_aszCpssDbgSubsysName[SWP_SUBSYS_MAX_NUM] = 
{
    "",SWP_SUBSYSNAME_DRV, SWP_SUBSYSNAME_CPSS, SWP_SUBSYSNAME_SMSS, SWP_SUBSYSNAME_RDBS,
        SWP_SUBSYSNAME_OAMS, SWP_SUBSYSNAME_CPM, SWP_SUBSYSNAME_UPM, SWP_SUBSYSNAME_HSPS,
        SWP_SUBSYSNAME_TPSS, SWP_SUBSYSNAME_PSSS, SWP_SUBSYSNAME_HSSS, SWP_SUBSYSNAME_CSSS,
        SWP_SUBSYSNAME_CSUP, SWP_SUBSYSNAME_RDBSR
};  
/* 存储模块名 */

 CHAR g_aaszCpssDbgModuleName[SWP_SUBSYS_MAX_NUM][SWP_MODULE_MAX_NUM][CPSS_MAX_MODULE_NAME_LENGTH] = 
{
    {"", "", "", ""},
    {DRV_COMMON_PRINT_ABRV  ,                
    DRV_COMMON_PRINT_ABRV_COMMON    ,       
    DRV_COMMON_PRINT_ABRV_PM5384  ,         
    DRV_COMMON_PRINT_ABRV_PM5320   ,        
    DRV_COMMON_PRINT_ABRV_APC  ,            
    DRV_COMMON_PRINT_ABRV_NSS   ,           
    DRV_COMMON_PRINT_ABRV_IMA   ,           
    DRV_COMMON_PRINT_ABRV_SAR    ,          
    DRV_COMMON_PRINT_ABRV_ETH64460  ,       
    DRV_COMMON_PRINT_ABRV_IPMI  ,           
    DRV_COMMON_PRINT_ABRV_FPGA   ,          
    DRV_COMMON_PRINT_ABRV_BOND    ,         
    DRV_COMMON_PRINT_ABRV_IPHA  ,           
    DRV_COMMON_PRINT_ABRV_L2M  ,            
    DRV_COMMON_PRINT_ABRV_UPDTC  ,          
    DRV_COMMON_PRINT_ABRV_FE8247  ,         
    DRV_COMMON_PRINT_ABRV_FE8560   ,        
    DRV_COMMON_PRINT_ABRV_PCI8247  ,        
    DRV_COMMON_PRINT_ABRV_PCI8560  ,        
    DRV_COMMON_PRINT_ABRV_TSEC   ,          
    DRV_COMMON_PRINT_ABRV_TCAM  ,           
    DRV_COMMON_PRINT_ABRV_BCM    ,          
    DRV_COMMON_PRINT_ABRV_FEI   ,           
    DRV_COMMON_PRINT_ABRV_ETH06885   ,      
    DRV_COMMON_PRINT_ABRV_E12288   ,        
    DRV_COMMON_PRINT_ABRV_GEI    ,          
    DRV_COMMON_PRINT_ABRV_MV64460    ,      
    DRV_COMMON_PRINT_ABRV_SCSI    ,         
    DRV_COMMON_PRINT_ABRV_USB    ,          
    DRV_COMMON_PRINT_ABRV_PM8316     ,      
    DRV_COMMON_PRINT_ABRV_APP550  ,         
    DRV_COMMON_PRINT_ABRV_APP100  ,         
    DRV_COMMON_PRINT_ABRV_TDM    ,          
    DRV_COMMON_PRINT_ABRV_MCC    ,          
    DRV_COMMON_PRINT_ABRV_L2PRSTP     ,     
    DRV_COMMON_PRINT_ABRV_L2PLACP   ,       
    DRV_COMMON_PRINT_ABRV_L2PGMRP  ,        
    DRV_COMMON_PRINT_ABRV_L2PVLAN   ,       
    DRV_COMMON_PRINT_ABRV_L2PCOS    ,       
    DRV_COMMON_PRINT_ABRV_L2PTRUNK    ,     
    DRV_COMMON_PRINT_ABRV_L2PIGMPSNP   ,    
    DRV_COMMON_PRINT_ABRV_L2PGVRP    ,      
    DRV_COMMON_PRINT_ABRV_L2PMIRROR   ,                       
    DRV_COMMON_PRINT_ABRV_BSP       ,       
    DRV_COMMON_PRINT_ABRV_GSSABRDCFG   ,    
    DRV_COMMON_PRINT_ABRV_TSSABRDCFG  ,     
    DRV_COMMON_PRINT_ABRV_MASABRDCFG    ,   
    DRV_COMMON_PRINT_ABRV_MMPABRDCFG    ,   
    DRV_COMMON_PRINT_ABRV_MNPA8560BRDCFG    ,
    DRV_COMMON_PRINT_ABRV_MNPAXSCALEBRDCFG    ,
    DRV_COMMON_PRINT_ABRV_MDPABRDCFG     ,  
    DRV_COMMON_PRINT_ABRV_IWFABRDCFG   ,    
    DRV_COMMON_PRINT_ABRV_MEIA8247BRDCFG   ,
    DRV_COMMON_PRINT_ABRV_MEIA8560BRDCFG     ,
    DRV_COMMON_PRINT_ABRV_MAPABRDCFG    ,   
    DRV_COMMON_PRINT_ABRV_ERIABRDCFG    ,   
    DRV_COMMON_PRINT_ABRV_ORIABRDCFG       ,
    DRV_COMMON_PRINT_ABRV_NSCABRDCFG     ,  
    DRV_COMMON_PRINT_ABRV_MMPAOEMBRDCFG     ,
    DRV_COMMON_PRINT_ABRV_APS    ,          
    DRV_COMMON_PRINT_ABRV_CAM     ,         
    DRV_COMMON_PRINT_ABRV_ALBABRDCFG      , 
    DRV_COMMON_PRINT_ABRV_IPMCHOST    ,     
    DRV_COMMON_PRINT_ABRV_MEPABRDCFG      , 
    DRV_COMMON_PRINT_ABRV_LLP        ,      
    DRV_COMMON_PRINT_ABRV_LIUDS26334    ,   
    DRV_COMMON_PRINT_ABRV_CSPABRDCFG     ,  
    DRV_COMMON_PRINT_ABRV_CPLD       ,      
    DRV_COMMON_PRINT_ABRV_ULTRAMAPPER   
    },
    {"", CPSS_MODULENAME_COM, CPSS_MODULENAME_DBG, CPSS_MODULENAME_FS, CPSS_MODULENAME_KW,
    CPSS_MODULENAME_TM, CPSS_MODULENAME_VK, CPSS_MODULENAME_VOS, CPSS_MODULENAME_MM,
    CPSS_MODULENAME_CMN},
    {"", SMSS_MODULENAME_SYSCTL, SMSS_MODULENAME_VERM, SMSS_MODULENAME_DEVM, SMSS_MODULENAME_BOOT, SMSS_MODULENAME_SYSRT},
    {"", RDBS_MODULENAME_KNL, RDBS_MODULENAME_DCM, CPS__RDBS_MODULENAME_DDSYN, CPS__RDBS_MODULENAME_MDSYN, CPS__RDBS_MODULENAME_SDSYN,
    CPS__RDBS_MODULENAME_MSSYN, RDBS_MODULENAME_IF, RDBS_MODULENAME_INIT, RDBS_MODULENAME_PROBE, AC_MODULENAME_RDBSAC,
    CPS__RDBS_MODULENAME_ATM, CPS__RDBS_MODULENAME_IPOA, CPS__RDBS_MODULENAME_IP, CPS__RDBS_MODULENAME_N7, CPS__RDBS_MODULENAME_SIGTRAN,
    RDBS_MODULENAME_COMMON, RDBS_MODULENAME_DEBUG},
    {"", OAMS_MDLNAME_CF, OAMS_MDLNAME_AM_AGT, CPS__OAMS_MDLNAME_DYND_AGT, CPS__OAMS_MDLNAME_DTPRB_AGT, OAMS_MDLNAME_PM_MNGR,
    CPS__OAMS_MDLNAME_SHCMD_AGT, CPS__OAMS_MDLNAME_TRC_AGT,CPS__OAMS_MDLNAME_LOG_AGT, CPS__OAMS_MDLNAME_DIAG_AGT, OAMS_MDLNAME_IF, "", "", "", "", "", OAMS_MDLNAME_WMC, "", "", "",
    OAMS_MDLNAME_AM_MNGR, CPS__OAMS_MDLNAME_COMM, CPS__OAMS_MDLNAME_FTM, OAMS_MDLNAME_LOGM, CPS__OAMS_MDLNAME_MSGDSP,
    OAMS_MDLNAME_PM_MNGR, CPS__OAMS_MDLNAME_TM, CPS__OAMS_MDLNAME_TRC_MNGR,CPS__OAMS_MDLNAME_DIAG_MNGR,CPS__OAMS_MDLNAME_ABOX_MNGR,CPS__OAMS_MDLNAME_ABOX_DRV,CPS__OAMS_MDLNAME_DLM
    },
   {"",AC_MODULENAME_CPMCC, AC_MODULENAME_APM, AC_MODULENAME_STAM, AC_MODULENAME_DHCP, AC_MODULENAME_RADIUS,
	   AC_MODULENAME_WAPI, AC_MODULENAME_8021X, AC_MODULENAME_PPPOE, AC_MODULENAME_PORTAL, AC_MODULENAME_CAPWAP,
	   AC_MODULENAME_CHARGE, AC_MODULENAME_PUB, CPS__TNBS_DHCPV6NAME_MODULE,CPS__TNBS_IKENAME_MODULE,
	   CPS__TNBS_PPPNAME_MODULE,CPS__TNBS_MLPPPNAME_MODULE,CPS__TNBS_UCOMNAME_MODULE,CPS__TNBS_MCTMNAME_MODULE,
	   CPS__TNBS_GTPUNAME_MODULE,CPS__TNBS_GTPU_RNAME_MODULE,CPS__TNBS_GTPU_SNAME_MODULE,CPS__TNBS_GTPU_GNAME_MODULE,
	   CPS__TNBS_RTCPNAME_MODULE,CPS__TNBS_SOCKETNAME_MODULE,CPS__TNBS_TCPNAME_MODULE,CPS__TNBS_SCTPNAME_MODULE,
	   CPS__TNBS_UDPNAME_MODULE,CPS__TNBS_ARPNAME_MODULE,CPS__TNBS_NDNAME_MODULE,CPS__TNBS_ICMPNAME_MODULE,
	   CPS__TNBS_ICMPV6NAME_MODULE,CPS__TNBS_IPV4NAME_MODULE,CPS__TNBS_IPV6NAME_MODULE,CPS__TNBS_IGMPNAME_MODULE,
	   CPS__TNBS_MLDNAME_MODULE,CPS__TNBS_IPOANAME_MODULE,CPS__TNBS_IFNAME_MODULE,CPS__TNBS_ROUTENAME_MODULE,
	   CPS__TNBS_NSMNAME_MODULE,CPS__TNBS_OSPFV2NAME_MODULE,CPS__TNBS_OSPFV3NAME_MODULE,CPS__TNBS_RIPV2NAME_MODULE,
	   CPS__TNBS_RIPV3NAME_MODULE,CPS__TNBS_PIMV4NAME_MODULE,CPS__TNBS_PIMV6NAME_MODULE,CPS__TNBS_IPIPNAME_MODULE,
	   CPS__TNBS_NATNAME_MODULE,CPS__TNBS_ACLNAME_MODULE,CPS__TNBS_AHNAME_MODULE,CPS__TNBS_ESPNAME_MODULE,
	   CPS__TNBS_DSCPNAME_MODULE,CPS__TNBS_RTPNAME_MODULE,CPS__TNBS_E1TTPNAME_MODULE,CPS__TNBS_COMNAME_MODULE,
	   CPS__TNBS_DBGNAME_MODULE,CPS__TNBS_FTPNAME_MODULE},
   
    {"", UPM_MODULENAME_CCM, UPM_MODULENAME_DPM, UPM_MODULENAME_IPSTACK, UPM_MODULENAME_NAT, UPM_MODULENAME_USRM, UPM_MODULENAME_UTM,TNSS_MODULENAME_M3UA,TNSS_MODULENAME_M3NIF,TNSS_MODULENAME_SCCP,TNSS_MODULENAME_TCAP,TNSS_MODULENAME_NF,TNSS_MODULENAME_ALCAP,TNSS_MODULENAME_COMMON},
	{"", HSPS_RRM_MODULE_NAME, HSPS_CC_MODULE_NAME, HSPS_CRM_MODULE_NAME,HSPS_MAIN_MODULE_NAME,HSPS_DRM_MODULE_NAME,HSPS_IUC_MODULE_NAME,HSPS_IUBC_MODULE_NAME,HSPS_GPM_MODULE_NAME,HSPS_RAC_MAIN_MODULE_NAME,HSPS_RB_MODULE_NAME,HSPS_SRB_MODULE_NAME,HSPS_SMC_MODULE_NAME,HSPS_CNT_CHK_MODULE_NAME,HSPS_MM_MODULE_NAME,HSPS_PNFE_MODULE_NAME,HSPS_MBC_MODULE_NAME},
    {"", "", "", ""},
	{"", "", "", ""},
    {"", "", "", ""},
	{"", "", "", ""},
    {"", "", "", ""},
   #if SWP_NE_TYPE == SWP_NETYPE_RNC
	{"","","","","","","","","","","","","","","","","","","","",RDBSR_MODULENAME_MAIN, RDBSR_MODULENAME_LRM, RDBSR_MODULENAME_RCM}
   #else
       {"", "", "", ""}
   #endif
};     
#endif

/* 存储子系统名 */
STRING g_aszCpssDbgSubsysName[SWP_SUBSYS_MAX_NUM] =
{
		"",SWP_SUBSYSNAME_DRV, SWP_SUBSYSNAME_CPSS, SWP_SUBSYSNAME_SMSS, SWP_SUBSYSNAME_RDBS,
		SWP_SUBSYSNAME_OAMS, SWP_SUBSYSNAME_CPM, SWP_SUBSYSNAME_UPM
};

/* 存储模块名 */
CHAR g_aaszCpssDbgModuleName[SWP_SUBSYS_MAX_NUM][SWP_MODULE_MAX_NUM][CPSS_MAX_MODULE_NAME_LENGTH] =
{
		{"", "", "", ""},

		{"", "", "", ""},

		{"", CPSS_MODULENAME_COM, CPSS_MODULENAME_DBG, CPSS_MODULENAME_FS, CPSS_MODULENAME_KW, CPSS_MODULENAME_TM,
		CPSS_MODULENAME_VK, CPSS_MODULENAME_VOS, CPSS_MODULENAME_MM, CPSS_MODULENAME_CMN},

		{"", SMSS_MODULENAME_SYSCTL, SMSS_MODULENAME_VERM, SMSS_MODULENAME_DEVM, SMSS_MODULENAME_BOOT, SMSS_MODULENAME_SYSRT},

		{"", RDBS_MODULENAME_KNL, RDBS_MODULENAME_DCM, "", "", "",
		"", RDBS_MODULENAME_IF, RDBS_MODULENAME_INIT, RDBS_MODULENAME_PROBE, AC_MODULENAME_RDBSAC,
		"", "", "", "", "",
		RDBS_MODULENAME_COMMON, RDBS_MODULENAME_DEBUG},

		{"", OAMS_MDLNAME_CF, OAMS_MDLNAME_AM_AGT, "", "", OAMS_MDLNAME_PM_MNGR,
		"", "", "", "", OAMS_MDLNAME_IF,
		"", "", "", "", "",
		OAMS_MDLNAME_WMC, "", "", "", OAMS_MDLNAME_AM_MNGR,
		"", "", OAMS_MDLNAME_LOGM, "", OAMS_MDLNAME_PM_MNGR,
		"", "", "", "", ""},

		{"", AC_MODULENAME_CPMCC, AC_MODULENAME_APM, AC_MODULENAME_STAM, AC_MODULENAME_DHCP, AC_MODULENAME_RADIUS,
		AC_MODULENAME_WAPI, AC_MODULENAME_8021X, AC_MODULENAME_PPPOE, AC_MODULENAME_PORTAL, AC_MODULENAME_CAPWAP,
		AC_MODULENAME_CHARGE, AC_MODULENAME_PUB, "", "", ""},

		{"", UPM_MODULENAME_CCM, UPM_MODULENAME_DPM, UPM_MODULENAME_IPSTACK, UPM_MODULENAME_NAT, UPM_MODULENAME_USRM,
		UPM_MODULENAME_UTM, "", "", "", ""},
};

#define CPSS_DBG_PRINT_LEVEL_MAX 7
#define CPSS_DBG_PRINT_LEVEL_STRING_LEN 7
UINT8 g_aszCpssDbgInfoType[CPSS_DBG_PRINT_LEVEL_MAX][CPSS_DBG_PRINT_LEVEL_STRING_LEN] = {
"OFF",
"FATAL",
"FAIL",
"ERROR",
"WARN",
"INFO",
"DETAIL"
};
/* CPU占用率*/
INT32 cpss_cpu_usage_get(UINT32 *pulUsage);  /* 2006/06/09 李军增加*/
/* 2006/08/28李军增加*/
extern INT32 cpss_extern_log(
  UINT8 ucSubSystemID,
 UINT8 ucModuleID,
 UINT8 ucLogLevel,
 STRING szLogInfo);
extern INT32 cpss_com_dsp_logic_addr_get(UINT8 ucCpuNo,CPSS_COM_LOGIC_ADDR_T* pstDspLogAddr);
extern INT32  cpss_sbbr_rsp_msg_proc
(
 CPSS_SBBR_RSP_MSG_T *pstRspMsg
);
extern VOID cpss_sbbr_transation_proc
(
 UINT32 ulCpuNo
);
extern BOOL cpss_sbbr_satisfy_auto_transfer
(
VOID
);
extern VOID cpss_sbbr_trans_sbbr_data
(
 VOID
);
extern VOID cpss_sbbr_wait_rsp_expire_proc(VOID);

extern VOID  cpss_sbbr_trans_data
(
 VOID
);

extern INT32 cpss_com_ldt_send(UINT8* pucSendBuf,UINT32 ulSendLen);
extern VOID  cpss_sbbr_trans_next_dsp(VOID);
extern void cpss_dsp_sbbr_record(UINT32 ulMsgId,UINT8 *pucBuf,UINT32 ulMsgLen);
/******************************* 局部常数和类型定义 ***************************/

/******************************* 局部函数原型声明 *****************************/
 INT32 cpss_debug_init();
 INT32 cpss_print_data_send();
 INT32 cpss_print_data_add
(
 UINT8 *pucBuf
 );
 INT32 cpss_print_init();

/******************************* 函数实现 *************************************/

/*******************************************************************************
* 函数名称: cpss_debug_init
* 功    能: 调试部分初始化
* 函数类型: 
* 参    数: 无。
* 参数名称            类型        输入/输出         描述
* 
* 函数返回: 初始化成功返回CPSS_OK, 失败返回CPSS_ERROR.
* 说    明: 
*******************************************************************************/
INT32 cpss_debug_init()
{
    INT32 lPrintInit = 0;
    INT32 lLogInit = 0;
    INT32 lTraceInit = 0 ;        
    
    lPrintInit = cpss_print_init();
    lLogInit = cpss_log_init();
    lTraceInit = cpss_com_trace_init() ;
 
    if ((CPSS_OK != lPrintInit)
        || (CPSS_OK != lLogInit))
    {
        return (CPSS_ERROR);             
    }
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_print_init
* 功    能: 打印部分初始化
* 函数类型: 
* 参    数: 无。
* 参数名称            类型        输入/输出         描述
* 
* 函数返回: 初始化成功返回CPSS_OK, 失败返回CPSS_ERROR.
* 说    明: 
*******************************************************************************/
INT32 cpss_print_init()
{
    INT32 lPrintRedirSet = 0;
    INT32 lPrintRateSet = 0;
    /* 初始化打印管理信息结构 */
    g_stCpssDbgPrtManage.ulPrtID = 0;
    g_stCpssDbgPrtManage.ulPrintID = 0;
 /* 如果当前打印开关未被打开,则用宏开关控制*/
    if(g_stCpssDbgPrtManage.ulBoardTag  != CPSS_PRINT_ON)
    {
      g_stCpssDbgPrtManage.ulBoardTag = CPSS_PRINT_FLAG;
    }  
    g_stCpssDbgPrtManage.lPrtTimerID = CPSS_ERROR;
    g_stCpssDbgPrtManage.pucPrtBufHead = NULL;
    g_stCpssDbgPrtManage.pucPrtDataAdd = NULL;
    g_stCpssDbgPrtManage.pucPrtDataHead = NULL;
    g_stCpssDbgPrtManage.pucPrtDataTail = NULL;
    g_stCpssDbgPrtManage.ulPrtTimerPeriod = 1000;
    g_stCpssDbgPrtManage.ulPrtToken = 0;
    g_stCpssDbgPrtManage.ulPrtTokenValid = 0;
    g_stCpssDbgPrtManage.ulPrtInfoLostNum = 0;
    g_stCpssDbgPrtManage.ulSemForPrtLost = VOS_SEM_DESC_INVALID;
    
    g_pstCpssDbgPrtDstPid = NULL;
    
    /* 初始化打印中用到的信号量 */
    g_stCpssDbgPrtManage.ulSemForPrtLost = cpss_vos_sem_m_create(VOS_SEM_Q_FIFO);
    if (VOS_SEM_DESC_INVALID == g_stCpssDbgPrtManage.ulSemForPrtLost)
    {
        return (CPSS_ERROR);
    }
    
    cpss_com_phy_addr_get(&g_stCpssDbgPhyAddr);
    
    /* 初始化打印缓冲区 */
    if (g_stCpssDbgPrtManage.pucPrtBufHead != NULL)
    {
        cpss_mem_free(g_stCpssDbgPrtManage.pucPrtBufHead);
    }
    g_stCpssDbgPrtManage.pucPrtBufHead = (UINT8 *)cpss_mem_malloc(CPSS_PRINT_BUF_LEN);    
    if (g_stCpssDbgPrtManage.pucPrtBufHead == NULL)
    {
        return (CPSS_ERROR);
    }
    
    g_stCpssDbgPrtManage.pucPrtDataHead = NULL;            /* 初始打印缓冲队列为空 */
    g_stCpssDbgPrtManage.pucPrtDataTail = g_stCpssDbgPrtManage.pucPrtBufHead;
    g_stCpssDbgPrtManage.pucPrtDataAdd = g_stCpssDbgPrtManage.pucPrtDataTail;
    
    /* 初始化打印级别、打印重定向 */
    g_pstCpssDbgPrtDstPid = NULL;
    g_stCpssDbgPrtManage.bPrtSendtoMate = FALSE;
    lPrintRedirSet = cpss_print_redirect_set(g_stCpssDbgPrtManage.ulPrintID, NULL);
    lPrintRateSet = cpss_print_rate_set(50);
    if ((CPSS_OK != lPrintRedirSet)  || (CPSS_OK != lPrintRateSet))
    {
        cpss_mem_free(g_stCpssDbgPrtManage.pucPrtBufHead);
        return (CPSS_ERROR);
    }
    
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_debug_proc
* 功    能: 调试消息接收纤程 
* 函数类型: 
* 参    数: 
* 参数名称          类型        输入/输出       描述
* usUserState       UINT16      输入            业务状态
* pvVar             VOID *      输入            纤程私有数据区指针
* pstMsgHead        CPSS_COM_MSG_HEAD_T * 输入  消息头指针
* 函数返回: 无。
* 说    明: 
*******************************************************************************/
VOID cpss_debug_proc
(
 UINT16  usUserState,
 VOID    *pvVar,
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
    INT32 lTimerDel = 0;
    INT32 lLogDataSend = 0;
    INT32 lDebugInit = 0;
    INT32 lActProcRspSend = 0;
    INT32 lStandbyToSend = 0;
    INT32 lRet = 0;

    UINT32 ulMsgID = pstMsgHead->ulMsgId;
    if ((ulMsgID < CPSS_TIMER_00_MSG) || (ulMsgID > CPSS_TIMER_31_MSG))
    {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_DETAIL,
                    "receive MsgID = %#x \n", pstMsgHead->ulMsgId);
    }

    cpss_dsp_sbbr_record(pstMsgHead->ulMsgId,pstMsgHead->pucBuf,pstMsgHead->ulLen);
    
    switch (pstMsgHead->ulMsgId)
    {

    case CPSS_DBG_DIAGN_MSGID:
	 cpss_com_send(&pstMsgHead->stSrcProc,CPSS_DBG_DIAGN_MSGID_RSP,pstMsgHead->pucBuf, pstMsgHead->ulLen);

	 printf("send msg back to module %d subgroup %d group %d addrflag %d ulpd %d \n\r", 
	   	pstMsgHead->stSrcProc.stLogicAddr.ucModule,
	   	pstMsgHead->stSrcProc.stLogicAddr.ucSubGroup,
	   	pstMsgHead->stSrcProc.stLogicAddr.usGroup,
	   	pstMsgHead->stSrcProc.ulAddrFlag,
	   	pstMsgHead->stSrcProc.ulPd
	     	
	   	);
	 
        cpss_dbg_test_data_check(pstMsgHead->pucBuf, pstMsgHead->ulLen);
	
    break;

    case 	CPSS_DBG_DIAGN_MSGID_RSP:
    #ifdef CPSS_VOS_VXWORKS
         printf(" cpss_com_send_test recv  msg @ tick %d    \n\r", tickGet());
    #endif
        cpss_dbg_test_data_check(pstMsgHead->pucBuf, pstMsgHead->ulLen);
        break;
		
#ifndef CPSS_DSP_CPU

    case CPSS_PRINT_ADD_MSG:
        {
            /* 将打印信息加入到缓存 */
            cpss_print_data_add(pstMsgHead->pucBuf);
            /* 判断是否有令牌，有就发送1包数据 */
            if (g_stCpssDbgPrtManage.ulPrtTokenValid > 0)
            {
                cpss_print_data_send();
                g_stCpssDbgPrtManage.ulPrtTokenValid = g_stCpssDbgPrtManage.ulPrtTokenValid - 1;/* 消耗令牌 */
            }
            break;    
        }
    case CPSS_PRINT_TIMER_MSG:    /* 打印定时器消息 */  
        {
            /* 发送打印缓存数据，直至缓存为空或则令牌耗尽 */
            for (g_stCpssDbgPrtManage.ulPrtTokenValid = g_stCpssDbgPrtManage.ulPrtToken; 
            g_stCpssDbgPrtManage.ulPrtTokenValid > 0; 
            g_stCpssDbgPrtManage.ulPrtTokenValid = g_stCpssDbgPrtManage.ulPrtTokenValid - 1)
            {         
                /* 缓存是否为空 */
                if (g_stCpssDbgPrtManage.pucPrtDataHead != NULL)
                {                
                    /* 发送失败退出 */
                    if (CPSS_OK != cpss_print_data_send())
                    {
                        break;
                    }
                }
                /* 缓存为空退出 */
                else
                {
                    break;
                }
            }
            break; 
        }
    case CPSS_PRTTIMER_CREATE_MSG:
        {
            if (g_stCpssDbgPrtManage.ulPrtTimerPeriod == 0) /* 当打印上报被关闭时，删除流控定时器 */
            {
                if (g_stCpssDbgPrtManage.lPrtTimerID != CPSS_ERROR)
                {
                    lTimerDel = cpss_timer_delete(CPSS_PRINT_TIMER_NO);
                    if (CPSS_OK == lTimerDel)
                    {
                        g_stCpssDbgPrtManage.lPrtTimerID = CPSS_ERROR;
                    }
                    else
                    {
                        g_stCpssDbgPrtManage.lPrtTimerID = CPSS_OK;
                    }
                }
            }
            else
            {
                /* 删除之前的打印流控定时器 */
                if (g_stCpssDbgPrtManage.lPrtTimerID != CPSS_ERROR)            
                {
                    cpss_timer_delete(CPSS_PRINT_TIMER_NO);
                }
                /* 设置打印流控定时器 */
                g_stCpssDbgPrtManage.lPrtTimerID = cpss_timer_loop_set(CPSS_PRINT_TIMER_NO, g_stCpssDbgPrtManage.ulPrtTimerPeriod);        
            }
            /* 设置打印令牌 */
            g_stCpssDbgPrtManage.ulPrtTokenValid = g_stCpssDbgPrtManage.ulPrtToken;
            break;
        }
    case CPSS_LOG_ADD_MSG:
        {
            /* 将日志信息加入到缓存 */
            cpss_log_data_add(pstMsgHead->pucBuf);
            /* 判断是否有令牌，有就发送1包数据 */
            if (g_stCpssDbgLogManage.ulLogTokenValid > 0)
            {
//                cpss_log_data_send();
                g_stCpssDbgLogManage.ulLogTokenValid = g_stCpssDbgLogManage.ulLogTokenValid - 1;/* 消耗令牌 */
            }
            break;
        }
    case CPSS_LOG_TIMER_MSG:    /* 日志定时器消息 */
        {
            /* 发送日志缓存数据，直至缓存为空或则令牌耗尽 */
            for (g_stCpssDbgLogManage.ulLogTokenValid = g_stCpssDbgLogManage.ulLogToken; 
            g_stCpssDbgLogManage.ulLogTokenValid > 0; 
            g_stCpssDbgLogManage.ulLogTokenValid = g_stCpssDbgLogManage.ulLogTokenValid - 1)
            {            
                if (g_stCpssDbgLogManage.pucLogDataHead != NULL)
                {       
                    /* 发送失败退出 */
//                    lLogDataSend = cpss_log_data_send();
                    if (CPSS_OK != lLogDataSend)
                    {
                        break;
                    }
                }
                /* 缓存为空退出 */
                else
                {
                    break;
                }
            }
            break;
        }
    case CPSS_LOGTIMER_CREATE_MSG:   
        {
            if (g_stCpssDbgLogManage.ulLogTimerPeriod == 0) /* 当日志上报被关闭时，删除流控定时器 */
            {
                if (g_stCpssDbgLogManage.lLogTimerID != CPSS_ERROR)
                {
                    lTimerDel = cpss_timer_delete(CPSS_LOG_TIMER_NO);
                    if (CPSS_OK == lTimerDel)
                    {
                        g_stCpssDbgLogManage.lLogTimerID = CPSS_ERROR;
                    }
                    else
                    {
                        g_stCpssDbgLogManage.lLogTimerID = CPSS_OK;
                    }
                }
            }
            else
            {
                /* 删除之前的日志流控定时器 */
                if (g_stCpssDbgLogManage.lLogTimerID != CPSS_ERROR)
                {
                    cpss_timer_delete(CPSS_LOG_TIMER_NO);
                }
                /* 重新启动日志流控定时器 */
                g_stCpssDbgLogManage.lLogTimerID = cpss_timer_loop_set(CPSS_LOG_TIMER_NO, g_stCpssDbgLogManage.ulLogTimerPeriod);
            }
            g_stCpssDbgLogManage.ulLogTokenValid = g_stCpssDbgLogManage.ulLogToken;
            break;
        }
#endif
    case CPSS_TRACE_ADD_MSG:
        {
         
            lRet = cpss_com_trace_data_add(pstMsgHead->pucBuf) ;
            if(CPSS_OK != lRet)
            {
                cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_ERROR,"add trace data failed.") ;
            }
            lRet = cpss_com_trace_data_send() ;
            if(CPSS_OK != lRet)
            {
                cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_ERROR,"add trace data send failed.") ;
            }
            break ;
        }
    case CPSS_TRACE_TIMER_MSG:
        {
            
            /*重置令牌的数量*/
            g_stCpssComTraceMan.ulTraceTokenValid = g_stCpssComTraceMan.ulTraceToken ;

            lRet = cpss_com_trace_data_send() ;
            if(CPSS_OK != lRet)
            {
                cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_ERROR,
                    "recv trace timer,add trace data send failed.") ;
            }
            break ;
        }
    case CPSS_TRACE_TASK_ADD_MSG:
        {
            UINT32 ulFirstFlag = 0 ;

            lRet = cpss_com_trace_recv_addtask_deal(pstMsgHead->pucBuf,&ulFirstFlag) ;
            if(CPSS_OK != lRet)
            {
                cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_ERROR,
                    "trace task add msg deal failed.") ;
            }
            if(ulFirstFlag == 1)
            {
                cpss_timer_loop_set(CPSS_TRACE_TIMER_NO,CPSS_TRACE_TIMER_PERIOD);
            }

            break;
        }
    case CPSS_TRACE_TASK_DEL_MSG:
        {
            UINT32 ulTaskNull = 0 ;
            
            lRet = cpss_com_trace_recv_deltask_deal(pstMsgHead->pucBuf,&ulTaskNull) ;
            if(CPSS_OK != lRet)
            {
                cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_ERROR,
                    "cpss_com_trace_recv_deltask_deal failed.") ;
            }
            if(ulTaskNull == 1)
            {
                cpss_timer_delete(CPSS_TRACE_TIMER_NO);
            }
            break;            
        }
    case CPSS_TRACE_TASK_RATE_SET_MSG:
        {
            
            lRet = cpss_com_trace_recv_setrate_deal(pstMsgHead->pucBuf) ;
            if(CPSS_OK != lRet)
            {
                cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_ERROR,
                    "cpss_com_trace_recv_setrate_deal failed.") ;
            }
            break;
        }
    case CPSS_DEBUG_LOGIC_ADDR_GET_TM_MSG:
        {
            /*处理得到逻辑地址请求定时器消息*/
            cpss_com_get_logic_addr_tm_recv() ;
            break ;
        }
    case CPSS_COM_GET_LOGIC_ADDR_RSP_MSG:
        {
            /*接收到得到逻辑地址响应的处理*/
            cpss_com_logic_addr_get_rsp_deal(pstMsgHead->pucBuf) ;
            break ;
        }
    case CPSS_DEBUG_DIAG_COMM_TEST_TM_MSG:
        {
            /*接收到链路测试请求定时器的处理*/
            cpss_com_comm_test_tm_msg_recv() ;
            break;
        }
    case CPSS_COM_COMM_TEST_REQ_MSG:
        {
            /*接收到链路互联测试请求的处理*/
            cpss_com_comm_test_req_deal(pstMsgHead->pucBuf) ;
            break;
        }
    case CPSS_COM_COMM_TEST_RSP_MSG:
        {
            /*接收到链路互联测试响应的处理*/
            cpss_com_comm_test_rsp_deal(pstMsgHead->pucBuf) ;
            break;
        }
    case CPS__OAMS_DIAG_STOP_TEST_IND_MSG:
        {
            /*接收到停止测试诊断消息的处理*/
            cpss_dbg_cps__oams_diag_stop_test_msg_deal(pstMsgHead->pucBuf) ;
            break ;
        }

        /* SMSS纤程激活消息 */
    case SMSS_PROC_ACTIVATE_REQ_MSG:
        {    
            lDebugInit = cpss_debug_init();
            if (CPSS_OK == lDebugInit)
            {
                lActProcRspSend = cpss_active_proc_rsp_send(CPSS_OK);
                if (CPSS_OK == lActProcRspSend)
                {
                    /* 初始化成功设置标志位 */
                    g_bCpssDbgPrtValid = TRUE;
                    g_bCpssDbgLogValid = TRUE;
                }  
                else
                {
                    /* 发送失败仍旧设置未初始化标志 */
                    g_bCpssDbgPrtValid = FALSE;
                    g_bCpssDbgLogValid = FALSE;
                }			
	#ifdef CPSS_DSP_CPU
         cpss_print_level_set(3,1,5);
      #endif  
            }
            else
            {
                /* 初始化失败设置未初始化标志位 */
                g_bCpssDbgPrtValid = FALSE;
                g_bCpssDbgLogValid = FALSE;
            }


            break;
        }
        /* 收到SMSS的“纤程激活完成指示”消息不处理事情 */
    case SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG:   
        {        	
            g_bCpssRecvProcEndInd = TRUE;					  
		
#ifndef CPSS_DSP_CPU

          /*  if(cpss_sbbr_satisfy_auto_transfer()==TRUE)*/
            if(g_bCpssSbbrExcOccur == TRUE)
              {
                cpss_sbbr_trans_sbbr_data();
           
              }
#endif	
	
            break;
        }
#ifndef CPSS_DSP_CPU
        /* Added by jiangliming, 2006/4/24 */
    case SMSS_STANDBY_TO_ACTIVE_REQ_MSG:    /* 备升主的请求消息 */
        {
            cpss_print_redirect_set(g_stCpssDbgPrtManage.ulPrintID, g_pstCpssDbgPrtDstPid);
            cpss_log_redirect_set();
            /* 发送响应消息 */
            lStandbyToSend = cpss_standby_to_active_send(CPSS_OK);
            if (CPSS_OK != lStandbyToSend)
            {                
                cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_ERROR,
                    "CPSS_COM: Debug proc send SMSS_STANDBY_TO_ACTIVE_RSP_MSG failed!\n");    
            }
            break;
        }
    case SMSS_ACTIVE_TO_STANDBY_IND_MSG:
        {
#if 0
            cpss_print_redirect_set(g_stCpssDbgPrtManage.ulPrintID, g_stCpssDbgPrtManage.g_pstCpssDbgPrtDstPid);
            cpss_log_redirect_set();
#endif
            break;
        }
        /* Add ended, 2006/4/24 CPS__OAMS_DIAG_REQ_MSG*/
#endif       
    case CPS__OAMS_DIAG_START_TEST_REQ_MSG:      /* 诊断测试消息  李军 增加 2006/06/09*/
        { 
            cpss_dbg_cps__oams_diag_req_msg_proc(pstMsgHead);
        }
        
        break;

    case CPSS_CPS__OAMS_PRT_LEVEL_SET_MSG:
            {
              CPSS_CPS__OAMS_PRT_LEVEL_SET_T *ptLevelSet = (CPSS_CPS__OAMS_PRT_LEVEL_SET_T *)pstMsgHead->pucBuf;

              cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO," recv CPSS_CPS__OAMS_PRT_LEVEL_SET_MSG\n\r");

              cpss_print_level_set(ptLevelSet->ucSubSystemID,ptLevelSet->ucModuleID,ptLevelSet->ucPrintLevel);
            }
    break;
    case CPSS_CPS__OAMS_REDIRECT_SET_MSG:
            {
              CPSS_COM_PID_T stPid;
              UINT32 ulPrintID;
              
              CPSS_CPS__OAMS_REDIRECT_SET_T *ptRedirectSet = (CPSS_CPS__OAMS_REDIRECT_SET_T *)pstMsgHead->pucBuf;

              cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO," recv CPSS_CPS__OAMS_REDIRECT_SET_MSG\n\r");

              stPid = ptRedirectSet->stPid;
              stPid.stLogicAddr.usGroup = cpss_ntohs(ptRedirectSet->stPid.stLogicAddr.usGroup);
              stPid.ulAddrFlag = cpss_ntohl(ptRedirectSet->stPid.ulAddrFlag);
              stPid.ulPd         = cpss_ntohl(ptRedirectSet->stPid.ulPd);
              ulPrintID           = cpss_ntohl(ptRedirectSet->ulPrintID);

              cpss_print_redirect_set(ulPrintID, &stPid);
            }
    break;
    case CPSS_CPS__OAMS_PRT_RATE_SET_MSG:
            {
              CPSS_CPS__OAMS_PRT_RATE_SET_T*ptRateSet = (CPSS_CPS__OAMS_PRT_RATE_SET_T *)pstMsgHead->pucBuf;

              cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO," recv CPSS_CPS__OAMS_PRT_RATE_SET_MSG\n\r");
              cpss_print_rate_set(cpss_ntohl(ptRateSet->ulRate));
              
              
            }
    break;
#ifndef CPSS_DSP_CPU
    case CPSS_CPS__OAMS_LOG_LEVEL_SET_MSG:
            {
              CPSS_CPS__OAMS_LOG_LEVEL_SET_T *ptLogSet = (CPSS_CPS__OAMS_LOG_LEVEL_SET_T *)pstMsgHead->pucBuf;

              cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO," recv CPSS_CPS__OAMS_LOG_LEVEL_SET_MSG\n\r");
              cpss_log_level_set(ptLogSet->ucLogLevel);
              
              
              
            }
    break;
    case CPSS_CPS__OAMS_LOG_RATE_SET_MSG:
            {
              CPSS_CPS__OAMS_LOG_RATE_SET_T *ptLogSet = ( CPSS_CPS__OAMS_LOG_RATE_SET_T *)pstMsgHead->pucBuf;

              cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO," recv CPSS_CPS__OAMS_LOG_RATE_SET_MSG\n\r");
              cpss_log_rate_set(cpss_ntohl(ptLogSet->ulRate));
              
              
              
            }
    break;
/****** SBBR    START  **************/
   case CPSS_SBBR_RSP_MSG:
        {
            CPSS_SBBR_RSP_MSG_T *pstRspMsg = (CPSS_SBBR_RSP_MSG_T *)pstMsgHead->pucBuf;
           
            cpss_sbbr_rsp_msg_proc(pstRspMsg);
        }
   break;

   case CPSS_SBBR_TRANSACTION_MSG:
            {
               UINT32 ulCpuNo = *(UINT32 *)pstMsgHead->pucBuf;
           
               cpss_sbbr_transation_proc(ulCpuNo);
             }
   break;

    case CPSS_SBBR_DSP_TRANSACTION_MSG:
            {
               UINT32 ulCpuNo = *(UINT32 *)pstMsgHead->pucBuf;
               cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO," recv CPSS_SBBR_DSP_TRANSACTION_MSG\n\r");
             
               cpss_sbbr_trans_data();
             }
   break;

   case CPSS_SBBR_WAITRSP_TIMER_OUT_MSG:
            {
                cpss_sbbr_wait_rsp_expire_proc();
               
            }
   break;
   case CPSS_TIMER_10_MSG:
	   {
          cpss_sbbr_trans_next_dsp();
	   }
   break;
#endif
/****** SBBR    END  **************/   
#ifdef CPSS_FUNBRD_MC
case CPSS_DBG_INIT_LOG_MSG:
    	{
          cpss_com_ldt_send(pstMsgHead->pucBuf,pstMsgHead->ulLen);
       }
break;
#endif
    default:
        break;
    }
}

/*******************************************************************************
* 函数名称: cpss_print
* 功    能: 打印函数实现对打印信息的采集与上报    
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出       描述
* ucSubSystemID       UINT8       输入            子系统号
* ucModuleID          UINT8       输入            模块号
* ucPrintLevel        UINT8       输入            打印信息级别
* szPrintInfo         STRING      输入            格式字符串，与printf相同
* ...                             输入            和szFormat配合的可变参数。
* 函数返回: 打印成功执行返回打印的字符数, 否则返回CPSS_ERROR。
* 说    明: 其它模块调用该函数实现对信息的打印。
*******************************************************************************/

/***  下面变量监视hsps打印是否乱序*/
UINT32 g_ulCpssDbgSys = SWP_SUBSYS_HSPS;
UINT32 g_ulCpssDbgModule = 0;
UINT32 g_ulCpssDbgCurSeq = 0;
UINT32 g_ulCpssDbgLastSeq = 0;

INT32 cpss_dbg_vprint(
                 UINT8   ucSubSystemID,
                 UINT8   ucModuleID,
                 UINT8   ucPrintLevel,
                 STRING  szFormat,
                 va_list argptr)
{
    CPSS_PRINT_INFO_T stPrtInfo;
    INT32 lSemP;
    INT32 lComSendPhy;
    UINT32 ulInfoLen;	
 #ifdef CPSS_DSP_CPU
    CPSS_COM_PHY_ADDR_T  tSelfPhyAddr;
    CHAR tempBuf[CPSS_PRINT_PACKET_LEN];
 #endif
   
#ifdef CPSS_VOS_VXWORKS
    if (intContext())
    {
        /* 将可变参数转换为字符串 */
        vsprintf(stPrtInfo.cBuf, szFormat, argptr);
        logMsg("\n%s",(INT32)stPrtInfo.cBuf,0,0,0,0,0);
        return (0);  
    }
#endif /* CPSS_VOS_VXWORKS */
    
#ifdef CPSS_DSP_CPU
     cpss_com_phy_addr_get(&tSelfPhyAddr);
     sprintf(stPrtInfo.cBuf, "[DspNo %d] ",tSelfPhyAddr.ucCpu);
#endif

    /* 对于级别为FATAL和FAIL的打印，自动生成一条异常日志 */
    if ((ucPrintLevel == CPSS_PRINT_FATAL) 
        || (ucPrintLevel == CPSS_PRINT_FAIL)
        || (ucPrintLevel == CPSS_PRINT_IMPORTANT)
        || (ucPrintLevel == CPSS_PRINT_ERROR))				/* add by HD */
    {
        INT32 lRet ;
        /* 将可变参数转换为字符串 */
		
     #ifdef CPSS_DSP_CPU
     {
     
      /* vsprintf(tempBuf, szFormat, argptr);*/  
	cpss_vsnprintf(tempBuf,CPSS_PRINT_PACKET_LEN,szFormat, argptr);   

       strcat(stPrtInfo.cBuf,tempBuf);
     }
     #else
     {
        vsnprintf(stPrtInfo.cBuf, CPSS_PRINT_PACKET_LEN, szFormat, argptr);
     }
    #endif	 

        lRet = cpss_extern_log(ucSubSystemID, ucModuleID, ucPrintLevel, stPrtInfo.cBuf);
        if( ucPrintLevel == CPSS_PRINT_IMPORTANT)
         {
              return lRet;
         }

          
    /*避免记日志在开关关闭时打印出来,具体的打印开关还需要再判断一下*/
    if(g_stCpssDbgPrtManage.ulBoardTag == CPSS_PRINT_OFF)
    {
        return 0;
    }
    if ((ucPrintLevel == CPSS_PRINT_OFF) 
        || (ucPrintLevel > CPSS_PRINT_DETAIL)         
        || (ucSubSystemID >= SWP_SUBSYS_MAX_NUM)        
        || (ucModuleID >= SWP_MODULE_MAX_NUM))
    {
        return 0;
    }
    
    /* 
    * 判断是否需要打印 *
    */
    if ((g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintTag == CPSS_PRINT_OFF)
        || (g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintLevel[ucModuleID] == CPSS_PRINT_OFF) 
        || (g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintLevel[ucModuleID] < ucPrintLevel))
    {
        return 0;
    }
    		
    }
    
 if ((ucPrintLevel != CPSS_PRINT_FATAL) 
        && (ucPrintLevel != CPSS_PRINT_FAIL))
 {
    /* 将可变参数转换为字符串 */
     #ifdef CPSS_DSP_CPU
     {
      /* vsprintf(tempBuf, szFormat, argptr); */
  
	cpss_vsnprintf(tempBuf,CPSS_PRINT_PACKET_LEN,szFormat, argptr);   
	  
       strcat(stPrtInfo.cBuf,tempBuf);
     }
     #else
     {
    vsnprintf(stPrtInfo.cBuf, CPSS_PRINT_PACKET_LEN, szFormat, argptr);
     }
    #endif
 }	

#ifndef CPSS_DSP_CPU	
    if ((g_bCpssDbgPrtValid == FALSE) || (g_pstCpssDbgPrtDstPid == NULL))   
     {          
        printf("[%s][%s][%s]: %s\n", 
            g_aszCpssDbgSubsysName[ucSubSystemID], 
            g_aaszCpssDbgModuleName[ucSubSystemID][ucModuleID], 
            g_aszCpssDbgInfoType[ucPrintLevel],
            stPrtInfo.cBuf);
        return (strlen(stPrtInfo.cBuf));
    }
#endif	

/* 监视hsps打印是否乱序*/
     if(ucSubSystemID == g_ulCpssDbgSys)
     	{
          stPrtInfo.stPrtEntry.ulSerialID = g_ulCpssDbgCurSeq++;
     	}   
    
    /* 填充打印信息结构体 */
    /* stPrtEntry.ulSerialID = g_ulPrtID++; */             /* 填充打印流水号，放到数据加入缓存时再做 */
    stPrtInfo.stPrtEntry.ulTick = cpss_tick_get();         /* 填充tick时间 */
    /* 绝对时间放在纤程里再填写 */
    /*cpss_clock_get(&(stPrtInfo.stPrtEntry.stTime));*/    /* 填充打印源绝对时间 */
    stPrtInfo.stPrtEntry.ucSubSystemID = ucSubSystemID;    /* 填充子系统号 */
    stPrtInfo.stPrtEntry.ucModuleID = ucModuleID;          /* 填充模块号 */
    stPrtInfo.stPrtEntry.ucPrintLevel = ucPrintLevel;      /* 填充打印级别 */
    stPrtInfo.stPrtEntry.ucPrintFormat = 0;                /* 打印格式, 目前固定填0(打印内容为字符串) */
    stPrtInfo.stPrtEntry.ulInfoLen = strlen(stPrtInfo.cBuf) + 1;    /* 打印信息体的长度 */
    /* 限制信息体的长度 */
    if (stPrtInfo.stPrtEntry.ulInfoLen > CPSS_MAX_PRINT_LEN)
    {        
        stPrtInfo.stPrtEntry.ulInfoLen = CPSS_MAX_PRINT_LEN;
    }

 #if 0
    /* 对于级别为CPSS_PRINT_INFO的打印信息，限制其信息体长度 */
    if (ucPrintLevel == CPSS_PRINT_INFO)
    {
        if (stPrtInfo.stPrtEntry.ulInfoLen > CPSS_PRINT_LEN_FOR_PRTINFO)
        {
            stPrtInfo.stPrtEntry.ulInfoLen = CPSS_PRINT_LEN_FOR_PRTINFO;
        }
    }
#endif

ulInfoLen = stPrtInfo.stPrtEntry.ulInfoLen ;
stPrtInfo.stPrtEntry.ulInfoLen  = cpss_htonl( stPrtInfo.stPrtEntry.ulInfoLen );
stPrtInfo.stPrtEntry.ulTick        = cpss_htonl(stPrtInfo.stPrtEntry.ulTick);

    /* 准备信息发送 */
    /* 判断发送结果，如为失败则返回打印失败 */
#ifndef CPSS_DSP_CPU
    lComSendPhy = cpss_com_send_phy(g_stCpssDbgPhyAddr,
        CPSS_DBG_MNGR_PROC,
        CPSS_PRINT_ADD_MSG,
        (UINT8 *)(&stPrtInfo),
        ulInfoLen + sizeof(CPSS_PRINT_ENTRY_T));
#else
{
 
 CPSS_COM_PID_T tDstPid;
 
 cpss_com_host_logic_addr_get(&tDstPid.stLogicAddr, &tDstPid.ulAddrFlag);
 
 tDstPid.ulPd = CPSS_DBG_MNGR_PROC;
 	
 lComSendPhy = cpss_com_send(&tDstPid,
        CPSS_PRINT_ADD_MSG,
        (UINT8 *)(&stPrtInfo),
        ulInfoLen + sizeof(CPSS_PRINT_ENTRY_T));
}
#endif
    if (CPSS_OK != lComSendPhy)
    {
        lSemP = cpss_vos_sem_p(g_stCpssDbgPrtManage.ulSemForPrtLost, NO_WAIT);
        if (lSemP == CPSS_OK)
        {
            g_stCpssDbgPrtManage.ulPrtInfoLostNum = g_stCpssDbgPrtManage.ulPrtInfoLostNum + 1;
        }
        cpss_vos_sem_v(g_stCpssDbgPrtManage.ulSemForPrtLost);
        
        return (CPSS_ERROR);
    }
    
    return (ulInfoLen);
}

/*******************************************************************************
* 函数名称: cpss_print_isenable
* 功    能:判断当前打印开关    
* 函数类型:BOOL
* 参    数: 
* 参数名称            类型        输入/输出       描述
* ucSubSystemID       UINT8       输入            子系统号
* ucModuleID          UINT8       输入            模块号
* ucPrintLevel        UINT8       输入            打印信息级别

* ...                             输入            和szFormat配合的可变参数。
* 函数返回: 打印成功执行返回打印的字符数, 否则返回CPSS_ERROR。
* 说    明: 其它模块调用该函数实现对信息的打印。
*******************************************************************************/

BOOL cpss_print_isenable(
    UINT8   ucSubSystemID,
    UINT8   ucModuleID,
    UINT8   ucPrintLevel)
{
    if ((ucPrintLevel == CPSS_PRINT_FATAL) 
        || (ucPrintLevel == CPSS_PRINT_FAIL)
        || (ucPrintLevel == CPSS_PRINT_IMPORTANT)
        || (ucPrintLevel == CPSS_PRINT_ERROR))						/* add by HD */
    {
        return TRUE;
    }
    if(g_stCpssDbgPrtManage.ulBoardTag == CPSS_PRINT_OFF)
    {
        return FALSE;
    }
    if ((ucPrintLevel == CPSS_PRINT_OFF) 
        || (ucPrintLevel > CPSS_PRINT_DETAIL)         
        || (ucSubSystemID >= SWP_SUBSYS_MAX_NUM)        
        || (ucModuleID >= SWP_MODULE_MAX_NUM))
    {
        return FALSE;
    }
    if ((g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintTag == CPSS_PRINT_OFF)
        || (g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintLevel[ucModuleID] == CPSS_PRINT_OFF) 
        || (g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintLevel[ucModuleID] < ucPrintLevel))
    {
        return FALSE;
    }
    return TRUE;
}

/*******************************************************************************
* 函数名称: cpss_vprint
* 功    能: 打印函数实现对打印信息的采集与上报 (供hsps调用)   
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出       描述
* ucSubSystemID       UINT8       输入            子系统号
* ucModuleID          UINT8       输入            模块号
* ucPrintLevel        UINT8       输入            打印信息级别
* szPrintInfo         STRING      输入            格式字符串，与printf相同
* ...                             输入            和szFormat配合的可变参数。
* 函数返回: 打印成功执行返回打印的字符数, 否则返回CPSS_ERROR。
* 说    明: 其它模块调用该函数实现对信息的打印。
*******************************************************************************/

INT32 cpss_vprint
(
 UINT8   ucSubSystemID,
 UINT8   ucModuleID,
 UINT8   ucPrintLevel,
 STRING  szFormat,
 va_list argptr
 )
{
    
    if(cpss_print_isenable(ucSubSystemID, ucModuleID, ucPrintLevel))
    {
        return cpss_dbg_vprint(ucSubSystemID, ucModuleID, ucPrintLevel, szFormat, argptr);
    }
    return 0;
}

/*******************************************************************************
* 函数名称: cpss_print
* 功    能: 打印函数实现对打印信息的采集与上报    
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出       描述
* ucSubSystemID       UINT8       输入            子系统号
* ucModuleID          UINT8       输入            模块号
* ucPrintLevel        UINT8       输入            打印信息级别
* szPrintInfo         STRING      输入            格式字符串，与printf相同
* ...                             输入            和szFormat配合的可变参数。
* 函数返回: 打印成功执行返回打印的字符数, 否则返回CPSS_ERROR。
* 说    明: 其它模块调用该函数实现对信息的打印。
*******************************************************************************/

INT32 cpss_print
(
 UINT8   ucSubSystemID,
 UINT8   ucModuleID,
 UINT8   ucPrintLevel,
 STRING  szFormat,
 ...
 )
{
    INT32 iLength;
    va_list args;

    if(cpss_print_isenable(ucSubSystemID, ucModuleID, ucPrintLevel))
    {
        va_start(args, szFormat);
        iLength = cpss_dbg_vprint(ucSubSystemID, ucModuleID, ucPrintLevel, szFormat, args);
        va_end(args);
        return iLength;
    }
    return 0;
}

/*******************************************************************************
* 函数名称: cpss_print_data_add
* 功    能: 将打印数据加入缓冲  
* 函数类型: 
* 参    数: 
* 参数名称          类型        输入/输出         描述
* pucBuf            UINT8 *     输入              缓冲区指针
* 函数返回: 成功执行返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 
*******************************************************************************/
INT32 cpss_print_data_add
(
 UINT8 *pucBuf
 )
{
#ifndef CPSS_DSP_CPU
    CPSS_PRINT_HEAD_T stPrintHead;    
    UINT32 ulPrtLen = 0;
    
    /* 2006/5/15, jiangliming  增加入参的合法性判断 */
    /* 判断到来的数据是否合法 */
    if (pucBuf == NULL)
    {
        return (CPSS_ERROR);
    }

    ((CPSS_PRINT_ENTRY_T *)pucBuf)->ulInfoLen = cpss_ntohl(((CPSS_PRINT_ENTRY_T *)pucBuf)->ulInfoLen);
   ((CPSS_PRINT_ENTRY_T *)pucBuf)->ulTick       = cpss_ntohl(((CPSS_PRINT_ENTRY_T *)pucBuf)->ulTick);
    if (((CPSS_PRINT_ENTRY_T *)pucBuf)->ulInfoLen > CPSS_MAX_PRINT_LEN)
    {
        return (CPSS_ERROR);
    }
    
    /* 数据加入到缓存 */
    /* 判断缓存空间是否存在 */
    if (NULL == g_stCpssDbgPrtManage.pucPrtBufHead)
    {
        return (CPSS_ERROR);
    }
/* 检查hsps打印是否存在乱序*/
    if(((CPSS_PRINT_ENTRY_T *)pucBuf)->ucSubSystemID == g_ulCpssDbgSys)
    	{
    	       /* 如果新来的序号< 以前的序号,则产生乱序*/
                if(((CPSS_PRINT_ENTRY_T *)pucBuf)->ulSerialID < g_ulCpssDbgLastSeq) 
                	{
                	   /* 纪录错误的模块号*/
                	   if(g_ulCpssDbgModule == 0)
                	   g_ulCpssDbgModule =  ((CPSS_PRINT_ENTRY_T *)pucBuf)->ucModuleID;
                	   
                	}
                    else
                    	{
                         g_ulCpssDbgLastSeq = ((CPSS_PRINT_ENTRY_T *)pucBuf)->ulSerialID;
                    	}
    	}
    /* 增加流水号计数 */
    ((CPSS_PRINT_ENTRY_T *)pucBuf)->ulSerialID = g_stCpssDbgPrtManage.ulPrtID;
    g_stCpssDbgPrtManage.ulPrtID = g_stCpssDbgPrtManage.ulPrtID + 1;
    cpss_clock_get(&(((CPSS_PRINT_ENTRY_T *)pucBuf)->stTime));/* 填充打印源绝对时间 */
    
    ulPrtLen = ((CPSS_PRINT_ENTRY_T *)pucBuf)->ulInfoLen;    
    if (g_stCpssDbgPrtManage.pucPrtDataHead == NULL)
    {
        g_stCpssDbgPrtManage.pucPrtDataHead = g_stCpssDbgPrtManage.pucPrtDataTail;    /* 定位缓存头指针 */
        /* 加入第一条数据 */
        stPrintHead.ulSeqID = 0;        /* 固定为0 */
        stPrintHead.ulPrintID = g_stCpssDbgPrtManage.ulPrintID;
        cpss_com_phy_addr_get(&stPrintHead.stPhyAddr);
        stPrintHead.ucBoardType = SWP_FUNBRD_TYPE;    /* 待定 */
        stPrintHead.usInfoNum = 1;        /* 信息条计数为1 */        
        /* 加入包头 */
        cpss_mem_memcpy(
            g_stCpssDbgPrtManage.pucPrtDataTail,
            &stPrintHead,
            sizeof(CPSS_PRINT_HEAD_T));
        /* 加入每条打印数据头及消息内容 */
        cpss_mem_memcpy(
            g_stCpssDbgPrtManage.pucPrtDataTail + sizeof(CPSS_PRINT_HEAD_T),
            pucBuf,
            sizeof(CPSS_PRINT_ENTRY_T) + ulPrtLen);    
        /* 移动数据写入指针的位置 */
        g_stCpssDbgPrtManage.pucPrtDataAdd = g_stCpssDbgPrtManage.pucPrtDataTail 
            + sizeof(CPSS_PRINT_HEAD_T) 
            + sizeof(CPSS_PRINT_ENTRY_T) 
            + ulPrtLen;
    }
    else
    {
        /* 判断Tail处是否有足够的空间容纳新数据 */
        if (g_stCpssDbgPrtManage.pucPrtDataAdd + sizeof(CPSS_PRINT_ENTRY_T) + ulPrtLen 
            <= g_stCpssDbgPrtManage.pucPrtDataTail + CPSS_PRINT_PACKET_LEN)
        {
            ((CPSS_PRINT_HEAD_T *)g_stCpssDbgPrtManage.pucPrtDataTail)->usInfoNum = ((CPSS_PRINT_HEAD_T *)g_stCpssDbgPrtManage.pucPrtDataTail)->usInfoNum + 1;            
            /* 直接在g_pucPrtDataAdd处加入数据 */
            cpss_mem_memcpy(
                g_stCpssDbgPrtManage.pucPrtDataAdd,
                pucBuf,
                sizeof(CPSS_PRINT_ENTRY_T) + ulPrtLen);    
            /* 移动数据写入指针的位置 */
            g_stCpssDbgPrtManage.pucPrtDataAdd += sizeof(CPSS_PRINT_ENTRY_T) + ulPrtLen;
        }
        else
        {
            /* 链尾节点后移 */
            g_stCpssDbgPrtManage.pucPrtDataTail += CPSS_PRINT_PACKET_LEN;
            /* 链尾是否越界 */
            if (g_stCpssDbgPrtManage.pucPrtDataTail + CPSS_PRINT_PACKET_LEN 
                > g_stCpssDbgPrtManage.pucPrtBufHead + CPSS_PRINT_BUF_LEN)
            {
                g_stCpssDbgPrtManage.pucPrtDataTail = g_stCpssDbgPrtManage.pucPrtBufHead;/* 越界后返回缓存头处 */
            }
            /* 加入第一条数据 */
            stPrintHead.ulSeqID = 0;        /* 固定为0 */
            stPrintHead.ulPrintID = g_stCpssDbgPrtManage.ulPrintID;
            cpss_com_phy_addr_get(&stPrintHead.stPhyAddr);
            stPrintHead.ucBoardType = SWP_FUNBRD_TYPE;    /* 待定 */
            stPrintHead.usInfoNum = 1;        /* 信息条计数为1 */            
            /* 加入包头 */
            cpss_mem_memcpy(
                g_stCpssDbgPrtManage.pucPrtDataTail,
                &stPrintHead,
                sizeof(CPSS_PRINT_HEAD_T));
            /* 加入每条打印数据头及消息内容 */
            cpss_mem_memcpy(
                g_stCpssDbgPrtManage.pucPrtDataTail + sizeof(CPSS_PRINT_HEAD_T),
                pucBuf,
                sizeof(CPSS_PRINT_ENTRY_T) + ulPrtLen);    
            /* 移动数据写入指针的位置 */
            g_stCpssDbgPrtManage.pucPrtDataAdd = g_stCpssDbgPrtManage.pucPrtDataTail 
                + sizeof(CPSS_PRINT_HEAD_T) 
                + sizeof(CPSS_PRINT_ENTRY_T) 
                + ulPrtLen;            
        }
    }
#endif
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_print_data_hton
* 功    能: 将整包打印数据由主机字节序转换为网络字节序
* 函数类型: 
* 参    数: 无。
* 参数名称            类型        输入/输出         描述
* 
* 函数返回: 成功转换数据返回该包数据的长度,否则返回0。
* 说    明: 
*******************************************************************************/
UINT32 cpss_print_data_hton(UINT8 *pucBuf)
{
    CPSS_PRINT_HEAD_T *pstPrintHead;
    CPSS_PRINT_ENTRY_T *pstPrintEntry;
    UINT16 usCount = 0;
    UINT32 ulPrintLen = 0;
    
    if (pucBuf != NULL)
    {
        pstPrintHead = (CPSS_PRINT_HEAD_T *)pucBuf;
        usCount = pstPrintHead->usInfoNum;        
        ulPrintLen = sizeof(CPSS_PRINT_HEAD_T);
        pstPrintHead->ulSeqID = cpss_htonl(pstPrintHead->ulSeqID);
        pstPrintHead->ulPrintID = cpss_htonl(pstPrintHead->ulPrintID);
        pstPrintHead->usInfoNum = cpss_htons(pstPrintHead->usInfoNum);
        while (usCount > 0)
        {
            pstPrintEntry = (CPSS_PRINT_ENTRY_T *)(pucBuf + ulPrintLen);                
            ulPrintLen += pstPrintEntry->ulInfoLen + sizeof(CPSS_PRINT_ENTRY_T);
            if (ulPrintLen > CPSS_PRINT_PACKET_LEN)   /* 此时数据包出现异常 */
            {
                ulPrintLen = 0;
                break;
            }
            /* 由主机字节序转换为网络字节序 */
            pstPrintEntry->ulSerialID = cpss_htonl(pstPrintEntry->ulSerialID);
            pstPrintEntry->ulTick = cpss_htonl(pstPrintEntry->ulTick);
            pstPrintEntry->stTime.usYear = cpss_htons(pstPrintEntry->stTime.usYear);
            pstPrintEntry->ulInfoLen = cpss_htonl(pstPrintEntry->ulInfoLen);
            usCount = usCount - 1;
        }
    }
    return (ulPrintLen);
}
/*******************************************************************************
* 函数名称: cpss_print_data_ntoh
* 功    能: 将整包打印数据由网络字节序转换为主机字节序
* 函数类型: 
* 参    数: 无。
* 参数名称            类型        输入/输出         描述
* 
* 函数返回:  成功转换数据返回该包数据的长度,否则返回0。
* 说    明: 
*******************************************************************************/
UINT32 cpss_print_data_ntoh(UINT8 *pucBuf)
{
    CPSS_PRINT_HEAD_T *pstPrintHead;
    CPSS_PRINT_ENTRY_T *pstPrintEntry;
    UINT16 usCount = 0;
    UINT32 ulPrintLen = 0;
    
    if (pucBuf != NULL)
    {
        pstPrintHead = (CPSS_PRINT_HEAD_T *)pucBuf;
        ulPrintLen = sizeof(CPSS_PRINT_HEAD_T);
        pstPrintHead->ulSeqID = cpss_ntohl(pstPrintHead->ulSeqID);
        pstPrintHead->ulPrintID = cpss_ntohl(pstPrintHead->ulPrintID);
        pstPrintHead->usInfoNum = cpss_ntohs(pstPrintHead->usInfoNum);
        usCount = pstPrintHead->usInfoNum;   
        while (usCount > 0)
        {
            pstPrintEntry = (CPSS_PRINT_ENTRY_T *)(pucBuf + ulPrintLen);    
            /* 由网络字节序转换为主机字节序 */
            pstPrintEntry->ulSerialID = cpss_ntohl(pstPrintEntry->ulSerialID);
            pstPrintEntry->ulTick = cpss_ntohl(pstPrintEntry->ulTick);
            pstPrintEntry->stTime.usYear = cpss_ntohs(pstPrintEntry->stTime.usYear);
            pstPrintEntry->ulInfoLen = cpss_ntohl(pstPrintEntry->ulInfoLen);
            ulPrintLen += pstPrintEntry->ulInfoLen + sizeof(CPSS_PRINT_ENTRY_T);
            if (ulPrintLen > CPSS_PRINT_PACKET_LEN)   /* 此时数据包出现异常 */
            {
                ulPrintLen = 0;
                break;
            }
            usCount = usCount - 1;
        }
    }
    return (ulPrintLen);
}

/*******************************************************************************
* 函数名称: cpss_print_data_send
* 功    能: 发送打印数据
* 函数类型: 
* 参    数: 无。
* 参数名称            类型        输入/输出         描述
* 
* 函数返回: 成功发送数据返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 
*******************************************************************************/
INT32 cpss_print_data_send()
{
#ifndef CPSS_DSP_CPU
    UINT32 ulPrintLen = 0;
    UINT16 usCount = 0;
    
    INT32 lComSend = 0;
    
    /* 判断缓存是否为空 */
    if (g_stCpssDbgPrtManage.pucPrtDataHead != NULL)
    {             
        /* 根据打印重定向决定打印方向 */
        if (g_pstCpssDbgPrtDstPid == NULL) /* 本板输出 */
        {
            CPSS_PRINT_HEAD_T *pstPrintHead;
            CPSS_PRINT_ENTRY_T *pstPrintEntry;
            INT8 acBuf[CPSS_PRINT_PACKET_LEN];
            
            pstPrintHead = (CPSS_PRINT_HEAD_T *)g_stCpssDbgPrtManage.pucPrtDataHead;   
            usCount = pstPrintHead->usInfoNum;
            ulPrintLen = sizeof(CPSS_PRINT_HEAD_T);
            /* 如果打印级别为全关，则清空缓存 */
            if (g_stCpssDbgPrtManage.ulBoardTag == CPSS_PRINT_OFF)
            {
                g_stCpssDbgPrtManage.pucPrtDataHead = NULL;
                return (CPSS_OK);
            }
            while (usCount > 0)
            {
                pstPrintEntry = (CPSS_PRINT_ENTRY_T *)(g_stCpssDbgPrtManage.pucPrtDataHead + ulPrintLen);                
                ulPrintLen += pstPrintEntry->ulInfoLen + sizeof(CPSS_PRINT_ENTRY_T);
                if (ulPrintLen > CPSS_PRINT_PACKET_LEN)  /* 此时数据包出现异常 */
                {
                    break;
                }
                /* 作为标准流输出打印 */
                cpss_mem_memcpy(acBuf, 
                    (UINT8 *)pstPrintEntry + sizeof(CPSS_PRINT_ENTRY_T),
                    pstPrintEntry->ulInfoLen);
                acBuf[pstPrintEntry->ulInfoLen +1] = '\0';
                printf("[%s][%s][%s]: %s\n", 
                    g_aszCpssDbgSubsysName[pstPrintEntry->ucSubSystemID], 
                    g_aaszCpssDbgModuleName[pstPrintEntry->ucSubSystemID][pstPrintEntry->ucModuleID], 
                    g_aszCpssDbgInfoType[pstPrintEntry->ucPrintLevel],
                    acBuf);              
                usCount = usCount - 1;
            }
        }
        else
        {
            ulPrintLen = cpss_print_data_hton(g_stCpssDbgPrtManage.pucPrtDataHead);
            if (ulPrintLen > 0) /* 字节序转换成功，开始发送数据；如转换失败，自动丢弃该包数据 */
            {
#if 0  /* 2006/5/31, jiangliming  added, 打印不需要考虑发送到GCPA的情况 */
                /* 发送数据，如发送失败 */
                if (g_stCpssDbgPrtManage.bPrtSendtoMate)
                {
                    if (CPSS_ERROR == 
                        cpss_com_send_mate((g_pstCpssDbgPrtDstPid)->ulPd,
                        CPSS_PRINT_MSG,
                        g_stCpssDbgPrtManage.pucPrtDataHead,
                        ulPrintLen))
                    {                    
                        ulPrintLen = cpss_print_data_ntoh(g_stCpssDbgPrtManage.pucPrtDataHead);
                        /* 发送失败，如成功转换回主机字节序，则返回失败，以便下一次发送，若转换失败，则丢弃该包数据 */
                        if (ulPrintLen > 0)   
                        {
                            return (CPSS_ERROR);
                        }
                    }
                }
                else
#endif  /* 2006/5/31, jiangliming added ended */
                {
                    lComSend = cpss_com_send(g_pstCpssDbgPrtDstPid,
                        CPSS_PRINT_MSG,
                        g_stCpssDbgPrtManage.pucPrtDataHead,
                        ulPrintLen);
                    if (CPSS_OK != lComSend)
                    {
                        ulPrintLen = cpss_print_data_ntoh(g_stCpssDbgPrtManage.pucPrtDataHead);
                        /* 发送失败，如成功转换回主机字节序，则返回失败，以便下一次发送，若转换失败，则丢弃该包数据 */
                        if (ulPrintLen > 0)
                        {
                            return (CPSS_ERROR);
                        }
                    }
                }
            }
        }
        /* 将数据头指针g_stCpssDbgPrtManage.pucPrtDataHead后移 */
        if (g_stCpssDbgPrtManage.pucPrtDataHead == g_stCpssDbgPrtManage.pucPrtDataTail)/* 缓存中只有一包数据 */
        {
            g_stCpssDbgPrtManage.pucPrtDataHead = NULL;/* 发完后缓存变空 */
        }
        else/* 缓存中的数据多于一包 */
        {
            /* 链表头后移 */     
            g_stCpssDbgPrtManage.pucPrtDataHead += CPSS_PRINT_PACKET_LEN;
            /* 如果越界，则返回到缓存开始处 */
            if (g_stCpssDbgPrtManage.pucPrtDataHead + CPSS_PRINT_PACKET_LEN 
                > g_stCpssDbgPrtManage.pucPrtBufHead + CPSS_PRINT_BUF_LEN)
            {
                g_stCpssDbgPrtManage.pucPrtDataHead = g_stCpssDbgPrtManage.pucPrtBufHead;
            }
        }
    }
#endif

    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_print_level_set
* 功    能: 打印级别设置
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出         描述
* ucSubSystemID       UINT8       输入              子系统号
* ucModuleID          UINT8       输入              模块号
* ucPrintLevel        UINT8       输入              打印级别
* 函数返回: 成功执行返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 设置指定的子系统和模块的打印级别。当指定的子系统ID为SWP_SUBSYS_ALL
*           时，对所有子系统的所有模块设置打印级别。当指定的模块ID为SWP_MODULE_
*           ALL时，对指定子系统的所有模块设置打印级别。
*******************************************************************************/
INT32 cpss_print_level_set
(
 UINT8 ucSubSystemID,
 UINT8 ucModuleID,
 UINT8 ucPrintLevel
 )
{
    UINT32 ulCountI, ulCountJ;
    /* 判断进入的各个参数及合法性 */
    /* 判断打印级别参数 */
    if (ucPrintLevel > CPSS_PRINT_DETAIL)
    {
        return (CPSS_ERR_DBG_PRT_PARAM_INVA);
    }
    /* 判断子系统号 */
    if (ucSubSystemID != SWP_SUBSYS_ALL)
    {
        if (ucSubSystemID >= SWP_SUBSYS_MAX_NUM)
        {
            return (CPSS_ERR_DBG_PRT_PARAM_INVA);
        }
        /* 判断模块号 */
        if (ucModuleID != SWP_MODULE_ALL)
        {
            if (ucModuleID >= SWP_MODULE_MAX_NUM)
            {
                return (CPSS_ERR_DBG_PRT_PARAM_INVA);
            }
        }
    }    
    /* 设置整板打印级别 */
    if (ucSubSystemID == SWP_SUBSYS_ALL)
    {
        /* 循环子系统 */
        for (ulCountI = 0; ulCountI < SWP_SUBSYS_MAX_NUM; ulCountI++)
        {
           if((ulCountI == SWP_SUBSYS_CPSS)&&(ucPrintLevel != CPSS_PRINT_OFF))
            {
              continue;
            }
            /* 循环模块 */
            for (ulCountJ = 0; ulCountJ < SWP_MODULE_MAX_NUM; ulCountJ++)
            { 
                g_stCpssDbgPrtManage.astPrintLevel[ulCountI].ucPrintLevel[ulCountJ] = ucPrintLevel;                
            }
            /* 设置子系统打印开关 */
            if (ucPrintLevel == CPSS_PRINT_OFF)
            {
                g_stCpssDbgPrtManage.astPrintLevel[ulCountI].ucPrintTag = CPSS_PRINT_OFF;
            }
            else
            {
                g_stCpssDbgPrtManage.astPrintLevel[ulCountI].ucPrintTag = CPSS_PRINT_ON;
            }
        }
    }
    /* 设置整个子系统打印级别 */
    else if (ucModuleID == SWP_MODULE_ALL)
    {
        /* 循环模块 */
        for (ulCountJ = 0; ulCountJ < SWP_MODULE_MAX_NUM; ulCountJ++)
        {
            g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintLevel[ulCountJ] = ucPrintLevel;
        }
        /* 设置子系统打印开关 */
        if (ucPrintLevel == CPSS_PRINT_OFF)
        {
            g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintTag = CPSS_PRINT_OFF;
        }
        else
        {
            g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintTag = CPSS_PRINT_ON;
        }
    }
    /* 设置单个模块的打印级别 */
    else
    {
        g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintLevel[ucModuleID] = ucPrintLevel;
        /* 设置子系统打印开关 */
        if (ucPrintLevel != CPSS_PRINT_OFF)
        {
            g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintTag = CPSS_PRINT_ON;
        }
        else
        {
            /* 循环查找，判断该子系统目前的打印级别情况，找出是否有级别不为0的 */
            for (ulCountJ = 0; ulCountJ < SWP_MODULE_MAX_NUM; ulCountJ++)
            {
                if (g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintLevel[ulCountJ] != CPSS_PRINT_OFF)
                {
                    break;
                }
            }
            /* 如有打印不为0的，设置开关为1 */
            if (ulCountJ != SWP_MODULE_MAX_NUM)
            {
                g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintTag = CPSS_PRINT_ON;
            }
            /* 所有打印级别为0 */
            else
            {
                g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintTag = CPSS_PRINT_OFF;
            }
        }
    }
    
    /* 设置整板打印开关标志 */ 
    for (ulCountI = 0; ulCountI < SWP_SUBSYS_MAX_NUM; ulCountI++)
    {
    /* 通过各子系统打印开关来判断是否有打印级别不为0的情况，
    *  有则跳出 *
        */
        if (g_stCpssDbgPrtManage.astPrintLevel[ulCountI].ucPrintTag != CPSS_PRINT_OFF)
        {
            break;
        }
    }
    /* 设置对应的整板打印标志 */
    if (ulCountI != SWP_SUBSYS_MAX_NUM)
    {
        g_stCpssDbgPrtManage.ulBoardTag = CPSS_PRINT_ON;/* 设置为开 */
    }
    else/* 设置为关 */
    {
        g_stCpssDbgPrtManage.ulBoardTag = CPSS_PRINT_OFF;
    }
    
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_print_level_get
* 功    能: 获得打印级别
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出         描述
* ucSubSystemID       UINT8       输入              子系统号
* ucModuleID          UINT8       输入              模块号
* ucPrintLevel        UINT8 *     输出              打印级别
* 函数返回: 成功执行返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 获得指定的子系统和模块的打印级别。当指定的子系统ID为SWP_SUBSYS_ALL
*           时，返回CPSS_ERROR；当指定的模块ID为SWP_MODULE_ALL时，返回CPSS_ERROR。
*******************************************************************************/
INT32 cpss_print_level_get
(
 UINT8   ucSubSystemID,
 UINT8   ucModuleID,
 UINT8   *pucPrintLevel
 )
{
#if 0
    /* 判断子系统号和模块号 */
    if ((ucSubSystemID >= SWP_SUBSYS_MAX_NUM) 
        || (ucModuleID >= SWP_MODULE_MAX_NUM)
        || (ucSubSystemID == SWP_SUBSYS_ALL)
        || (ucModuleID == SWP_MODULE_ALL))
    {
        return (CPSS_ERR_DBG_PRT_PARAM_INVA);/* 不合法返回 */
    }
    else
#endif    	
    {
        /* 获得打印级别 */
        *pucPrintLevel = g_stCpssDbgPrtManage.astPrintLevel[ucSubSystemID].ucPrintLevel[ucModuleID];
        return (CPSS_OK);
    }
}

/*******************************************************************************
* 函数名称: cpss_print_redirect_set    
* 功    能: 打印重定向设置 
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出     描述
* ulPrintID       UINT32              输入          打印标识，由OAMS提供
* pstPid          CPSS_COM_PID_T *    输入          LDT的打印窗口对应的PID，
*                                                   为NULL时表示要把打印输出到STDOUT。
* 函数返回: 成功设置后返回CPSS_OK，否则返回CPSS_ERROR.
* 说    明: 打印默认输出是本板的标准输出流，可以通过本接口把打印重定向到指定的LDT。
*******************************************************************************/
INT32 cpss_print_redirect_set
(
 UINT32 ulPrintID,
 CPSS_COM_PID_T *pstPid
 )
{     
    /* 判断传入的参数是否为空 */
    if (pstPid == NULL)
    {
        g_pstCpssDbgPrtDstPid = NULL;     /* 为空设置打印重定向到本板 */
        g_stCpssDbgPrtManage.bPrtSendtoMate = FALSE;  /* 不需要发送到伙伴板 */
    }
    else
    {
        cpss_mem_memcpy(&(g_stCpssDbgPrtManage.stPrtDstPid), pstPid, sizeof(CPSS_COM_PID_T));        
        g_pstCpssDbgPrtDstPid = &(g_stCpssDbgPrtManage.stPrtDstPid);/* 设置目的纤程PID指针 */
#if 0 /* 2006/5/31, jiangliming  added, 打印不需要考虑发送到GCPA的情况 */
        bFuncType = cpss_local_funcbrd_type_is_mc();
        cpss_com_logic_addr_get(&stPrtSendPid.stLogicAddr, &stPrtSendPid.ulAddrFlag);
        if(TRUE == bFuncType)
        {
            if (CPSS_COM_ADDRFLAG_MASTER != stPrtSendPid.ulAddrFlag)
            {
                g_stCpssDbgPrtManage.bPrtSendtoMate = TRUE;   /* 需要发送到伙伴板 */
            }
            else
            {                
                g_stCpssDbgPrtManage.bPrtSendtoMate = FALSE;  /* 不需要发送到伙伴板 */
            }
        }
        else
        {
            g_stCpssDbgPrtManage.bPrtSendtoMate = FALSE;  /* 不需要发送到伙伴板 */
        }
#endif
    }
    
    g_stCpssDbgPrtManage.ulPrintID = ulPrintID;/* 设置PrintID */
    
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_print_rate_set    
* 功    能: 打印上报速率设置
* 函数类型: INT32
* 参    数: 
* 参数名称          类型        输入/输出         描述
* ulRate            UINT32      输入              打印输出速率（单位：条/秒）
* 函数返回: 成功执行返回CPSS_OK, 否则返回CPSS_ERROR.
* 说    明: 该函数实现对打印速率的设置，系统启动时会设置默认的打印速率（50条/秒）。
*******************************************************************************/
INT32 cpss_print_rate_set(UINT32 ulRate)
{
    /* 计算相应的定时器周期和令牌 */
    /* 分两种情况计算，即ulRate大于10和小于10两种情况 */
    if ((ulRate < 11) && (ulRate > 0))
    {
        g_stCpssDbgPrtManage.ulPrtTimerPeriod = 1000;/* 设置对应的定时器周期 */
        g_stCpssDbgPrtManage.ulPrtToken = ulRate;     /* 打印令牌 */
    }
    else if ((ulRate <= 256) && (ulRate > 10))
    {
        g_stCpssDbgPrtManage.ulPrtTimerPeriod = 100;/* 设置对应的定时器周期 */
        if (CPSS_MOD(ulRate,10) > 4) /* 四舍五入取每周期打印令牌 */
        {
            g_stCpssDbgPrtManage.ulPrtToken = CPSS_DIV(ulRate,10) + 1;
        }
        else
        {
            g_stCpssDbgPrtManage.ulPrtToken = CPSS_DIV(ulRate,10);
        }
    }
    else if (ulRate == 0)
    {
        g_stCpssDbgPrtManage.ulPrtTimerPeriod = 0;
        g_stCpssDbgPrtManage.ulPrtToken = 0;
    }
    else/* ulRate超出范围 */
    {
        return (CPSS_ERR_DBG_PRT_PARAM_INVA);
    }
    
    /* 发送信息 */
    return cpss_com_send_phy(g_stCpssDbgPhyAddr,
        CPSS_DBG_MNGR_PROC,
        CPSS_PRTTIMER_CREATE_MSG,
        NULL,
        0);
}

/******************************************************************************
* 函数名称: cpss_dbg_active_proc_rsp_send                            
* 功    能: 向SMSS发送纤程激活响应消息
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* ulResult            UINT32          输入              调试纤程激活结果
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:纤程激活结果: 成功CPSS_OK,失败CPSS_ERROR                                               
*******************************************************************************/
INT32 cpss_active_proc_rsp_send
(
 INT32 lResult
 )
{
    CPSS_COM_PHY_ADDR_T stPhyAddr;
    SMSS_PROC_ACTIVATE_RSP_MSG_T stRspMsg;
    stRspMsg.ulResult = SMSS_OK;
    
    cpss_com_phy_addr_get(&stPhyAddr);
    if(CPSS_OK != lResult)
    {
        stRspMsg.ulResult = SMSS_ERROR;
    }
    
    /* 使用物理地址发送，确保发送给本板的SMSS_SYSCTL_PROC纤程 */
    return cpss_com_send_phy(stPhyAddr,
        SMSS_SYSCTL_PROC,
        SMSS_PROC_ACTIVATE_RSP_MSG,
        (UINT8 *)(&stRspMsg),
        sizeof(SMSS_PROC_ACTIVATE_RSP_MSG_T));
}

/*******************************************************************************
* 函数名称: cpss_standby_to_active_send                            
* 功    能: 向SMSS发送备升主响应消息
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* ulResult            UINT32          输入              
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:成功CPSS_OK,失败CPSS_ERROR           
*******************************************************************************/
INT32 cpss_standby_to_active_send
(
 INT32    lResult
 )
{
    CPSS_COM_PHY_ADDR_T stPhyAddr;
    SMSS_STANDBY_TO_ACTIVE_RSP_MSG_T stToActiveResult;
    
    stToActiveResult.ulResult = SMSS_OK;
    if(CPSS_OK != lResult)
    {
        stToActiveResult.ulResult = SMSS_ERROR;
    }
    cpss_com_phy_addr_get(&stPhyAddr);
    
    /* 使用物理地址发送，确保发送给本板的SMSS_SYSCTL_PROC纤程 */
    return cpss_com_send_phy(stPhyAddr,
        SMSS_SYSCTL_PROC,
        SMSS_STANDBY_TO_ACTIVE_RSP_MSG,
        (UINT8 *)(&stToActiveResult),
        sizeof(SMSS_PROC_ACTIVATE_RSP_MSG_T));
}

/*******************************************************************************
* 函数名称: cpss_output
* 功    能: CPSS子系统专用打印函数    
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出       描述
* ucModuleID          UINT8       输入            模块号
* ucPrintLevel        UINT8       输入            打印信息级别
* szPrintInfo         STRING      输入            格式字符串，与printf相同
* ...                             输入            和szFormat配合的可变参数。
* 函数返回: 打印成功执行返回打印的字符数, 否则返回CPSS_ERROR。
* 说    明: 本函数的只供CPSS子系统内部调用，避免直接调用cpss_print产生的循环调用
*******************************************************************************/
INT32 cpss_output
(
 UINT8   ucModuleID,
 UINT8   ucPrintLevel,
 STRING  szFormat,
 ...
 )
{
    va_list args;
    INT8 acBuf[CPSS_PRINT_PACKET_LEN]; /* 该函数打印长度遵循编码规范 */

#ifndef ZC
    /* 判断整板打印开关 */
    if (g_stCpssDbgPrtManage.ulBoardTag == CPSS_PRINT_OFF)
    {
        return (0);
    }
    
    /* 判断进入的各个参数及合法性 */
    /* 判断打印级别、子系统号、模块号参数是否合法 */        
    if ((ucPrintLevel == CPSS_PRINT_OFF) 
        || (ucPrintLevel > CPSS_PRINT_DETAIL)              
        || (ucModuleID >= SWP_MODULE_MAX_NUM))
    {
        return (CPSS_ERROR);
    }
#endif
    /* 
    * 判断是否需要打印 *
    */
#ifndef ZC
    if (g_stCpssDbgPrtManage.astPrintLevel[SWP_SUBSYS_CPSS].ucPrintLevel[ucModuleID] < CPSS_PRINT_LEVEL_DIAG)
    {
    if ((g_stCpssDbgPrtManage.astPrintLevel[SWP_SUBSYS_CPSS].ucPrintTag == CPSS_PRINT_OFF)
        || (g_stCpssDbgPrtManage.astPrintLevel[SWP_SUBSYS_CPSS].ucPrintLevel[ucModuleID] == CPSS_PRINT_OFF) 
        || (g_stCpssDbgPrtManage.astPrintLevel[SWP_SUBSYS_CPSS].ucPrintLevel[ucModuleID] < ucPrintLevel))
    {
        return (0);/* 对于子系统打印关闭的情况，判断语句不走到||后 */
    }
    }
#endif
    
    /* 将可变参数转换为字符串 */
    va_start(args, szFormat);
#ifdef CPSS_DSP_CPU	
	cpss_vsnprintf(acBuf,CPSS_PRINT_PACKET_LEN,szFormat, args);   
#else
    vsnprintf(acBuf, CPSS_PRINT_PACKET_LEN, szFormat, args);
#endif
    va_end(args);
    
#ifdef CPSS_VOS_VXWORKS
    if (TRUE == intContext())
    {
        /* 2006/4/29, jiangliming  */
        /*logMsg("\n%s", acBuf);*/
        logMsg("\n%s",(INT32)acBuf,0,0,0,0,0);    /* 2006/08/28李军修改logMsg调用方法*/
        return 0; 
        /* 2006/4/29, jiangliming  */
    }
#endif /* CPSS_VOS_VXWORKS */
    /* 直接打印 */
    printf("[%s][%s][%s]: %s\n", 
        g_aszCpssDbgSubsysName[SWP_SUBSYS_CPSS], 
        g_aaszCpssDbgModuleName[SWP_SUBSYS_CPSS][ucModuleID], 
        g_aszCpssDbgInfoType[ucPrintLevel],
        acBuf);/* 加入模块和子系统名称的解析 */
    
    return (strlen(acBuf));
}

/*******************************************************************************
* 函数名称: cpss_print_lost_info_get
* 功    能: 实现采集丢失的打印信息数    
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出       描述
* pulPrtLostInfo      UINT32 *    输入            丢失的打印信息计数 
* 函数返回: 执行后返回CPSS_OK。
* 说    明: 
*******************************************************************************/
INT32 cpss_print_lost_info_get
(
 UINT32 *pulPrtLostInfo
 )
{
    *pulPrtLostInfo = g_stCpssDbgPrtManage.ulPrtInfoLostNum;
    return (CPSS_OK);
}

#if 0  /* 2006/6/1   蒋黎明  修改CPSS_FILE_LINE为宏函数 */
/*******************************************************************************
* 函数名称: CPSS_FILE_LINE
* 功    能: 调用本函数的文件名、行号   
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出       描述
* ucSubSystemID       UINT8       输入            子系统号
* ucModuleID          UINT8       输入            模块号
* ucPrintLevel        UINT8       输入            打印信息级别
* 函数返回: 执行后返回CPSS_OK。
* 说    明: 
*******************************************************************************/
INT32 CPSS_FILE_LINE
(
 UINT8 ucSubsystemID,
 UINT8 ucModuleID,
 UINT8 ucPrintLevel 
 )
{
    return cpss_print(ucSubsystemID, ucModuleID, ucPrintLevel, "File: %s, Line:%d ",__FILE__, __LINE__);
}
#endif

#ifndef CPSS_DSP_CPU
/*******************************************************************************
* 函数名称: cpss_dbg_g_stCpssDbgPrtManage_get
* 功    能: 获得g_stCpssDbgPrtManage地址
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出     描述
* 
* 函数返回: 
* 说    明:
*******************************************************************************/
INT32 cpss_dbg_g_stCpssDbgPrtManage_get()
{
    return (INT32)(&g_stCpssDbgPrtManage);
}
/*******************************************************************************
* 函数名称: cpss_dbg_g_bCpssDbgPrtValid_get
* 功    能: 获得g_bCpssDbgPrtValid地址
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出     描述
* 
* 函数返回: 
* 说    明:
*******************************************************************************/
INT32 cpss_dbg_g_bCpssDbgPrtValid_get()
{
    return (INT32)(&g_bCpssDbgPrtValid);
}
/*******************************************************************************
* 函数名称: cpss_dbg_g_pstCpssDbgPrtDstPid_get
* 功    能: 获得 g_pstCpssDbgPrtDstPid 地址
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出     描述
* 
* 函数返回: 
* 说    明:
*******************************************************************************/
INT32 cpss_dbg_g_pstCpssDbgPrtDstPid_get()
{
    return (INT32)(g_pstCpssDbgPrtDstPid);
}

BOOL cpss_dbg_pl_get(UINT32 ulModuleId)
    {
    return (g_stCpssDbgPrtManage.astPrintLevel[2].ucPrintLevel[ulModuleId] == CPSS_PRINT_LEVEL_DIAG_BF);
}

VOID cpss_dbg_pl_set(UINT32 ulModuleId, UINT32 ulLev)
{
    g_stCpssDbgPrtManage.astPrintLevel[2].ucPrintLevel[ulModuleId] = ulLev;

    return ;
}

/*******************************************************************************
* 函数名称: cpss_dbg_print_module_register
* 功    能:      注册子系统模块名
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出     描述
* ulSubSysNum           UINT32                  IN                        子系统
* szModuleNameInfo[] STRING                  IN                        模块名
* ulModuleMax            UINT32                 IN                        模块总数
* 函数返回: 
* 说    明:
*******************************************************************************/

VOID cpss_dbg_print_module_register
(
 UINT32 ulSubSysNum, 
 STRING szModuleNameInfo[], 
 UINT32 ulModuleMax

)
{
  UINT16 usLoop,usLen;

  if((ulSubSysNum >= SWP_SUBSYS_MAX_NUM)||(ulModuleMax >= SWP_MODULE_MAX_NUM))
  {
     return;
  }
  
  for(usLoop = 0; usLoop < ulModuleMax; usLoop++)
  {
     usLen = strlen(szModuleNameInfo[usLoop]);
     if(usLen > CPSS_MAX_MODULE_NAME_LENGTH)
     {
       usLen = CPSS_MAX_MODULE_NAME_LENGTH;
     }
     strncpy(g_aaszCpssDbgModuleName[ulSubSysNum][usLoop],szModuleNameInfo[usLoop],strlen(szModuleNameInfo[usLoop]));   
  }
  

}

#endif

/*******************************************************************************
* 函数名称: cpss_print_level_set_dsp
* 功    能: 设置DSP打印级别,向dsp发送消息
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出         描述
* ucCpuNo                   UINT8               IN                      dsp号(5~16)
* ucSubSystemID       UINT8       输入              子系统号
* ucModuleID          UINT8       输入              模块号
* ucPrintLevel        UINT8       输入              打印级别
* 函数返回: 成功执行返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 设置指定的子系统和模块的打印级别。当指定的子系统ID为SWP_SUBSYS_ALL
*           时，对所有子系统的所有模块设置打印级别。当指定的模块ID为SWP_MODULE_
*           ALL时，对指定子系统的所有模块设置打印级别。
*******************************************************************************/

INT32 cpss_print_level_set_dsp
(
 UINT8 ucCpuNo,
 UINT8 ucSubSystemID,
 UINT8 ucModuleID,
 UINT8 ucPrintLevel
 )
{
     CPSS_COM_PID_T stDstPid;
     CPSS_CPS__OAMS_PRT_LEVEL_SET_T stLevel;
    
     /* 判断打印级别参数 */
    if (ucPrintLevel > CPSS_PRINT_DETAIL)
    {
        return (CPSS_ERROR);
    }
    /* 判断子系统号 */
    if (ucSubSystemID != SWP_SUBSYS_ALL)
    {
        if (ucSubSystemID >= SWP_SUBSYS_MAX_NUM)
        {
            return (CPSS_ERROR);
        }
        /* 判断模块号 */
        if (ucModuleID != SWP_MODULE_ALL)
        {
            if (ucModuleID >= SWP_MODULE_MAX_NUM)
            {
                return (CPSS_ERROR);
            }
        }
    }
    /* 获取dsp的逻辑地址*/
    if(CPSS_OK != cpss_com_dsp_logic_addr_get(ucCpuNo,&stDstPid.stLogicAddr))
     {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_WARN," cpss_com_dsp_logic_addr_get fail in cpss_print_level_set_dsp\n\r");
            
        return CPSS_ERROR;
     }
     
      stDstPid.ulAddrFlag = 0;
      stDstPid.ulPd = CPSS_DBG_MNGR_PROC;
                  
      stLevel.ucSubSystemID = ucSubSystemID;
      stLevel.ucModuleID = ucModuleID;
      stLevel.ucPrintLevel = ucPrintLevel;

       return(cpss_com_send(&stDstPid,CPSS_CPS__OAMS_PRT_LEVEL_SET_MSG,(UINT8 *)&stLevel, sizeof(CPSS_CPS__OAMS_PRT_LEVEL_SET_T)));
       

}

/*******************************************************************************
* 函数名称: cpss_print_redirect_set_dsp    
* 功    能: ?向DSP发送重定向消息. 
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出     描述
* ucCpuNo                 UINT8                   IN                         DSP(5~16)
* ulPrintID       UINT32              输入          打印标识，由OAMS提供
* pstPid          CPSS_COM_PID_T *    输入          LDT的打印窗口对应的PID，
*                                                   为NULL时表示要把打印输出到STDOUT。
* 函数返回: 成功设置后返回CPSS_OK，否则返回CPSS_ERROR.
* 说    明: 打印默认输出是本板的标准输出流，可以通过本接口把打印重定向到指定的LDT。
*******************************************************************************/


INT32 cpss_print_redirect_set_dsp
(
UINT8 ucCpuNo,
UINT32 ulPrintID,
CPSS_COM_PID_T *pstPid
)
{
   CPSS_CPS__OAMS_REDIRECT_SET_T stRedirect;
    CPSS_COM_PID_T stDstPid;
    
   if(NULL == pstPid)
    {
      return CPSS_ERROR;
    }
    /* 获取dsp的逻辑地址*/
    if(CPSS_OK != cpss_com_dsp_logic_addr_get(ucCpuNo,&stDstPid.stLogicAddr))
     {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_WARN," cpss_com_dsp_logic_addr_get fail in cpss_print_redirect_set_dsp\n\r");
            
        return CPSS_ERROR;
     }

     stDstPid.ulAddrFlag = 0;
     stDstPid.ulPd = CPSS_DBG_MNGR_PROC;
  
     stRedirect.ulPrintID = cpss_htonl(ulPrintID);
     stRedirect.stPid = *pstPid;
     stRedirect.stPid.stLogicAddr.usGroup = cpss_htons(pstPid->stLogicAddr.usGroup);
     stRedirect.stPid.ulAddrFlag  = cpss_htonl(pstPid->ulAddrFlag);
     stRedirect.stPid.ulPd          = cpss_htonl(pstPid->ulPd);
     
     return(cpss_com_send(&stDstPid,CPSS_CPS__OAMS_REDIRECT_SET_MSG,(UINT8 *)&stRedirect,sizeof(CPSS_CPS__OAMS_REDIRECT_SET_T)));
  

}

/*******************************************************************************
* 函数名称: cpss_print_rate_set_dsp    
* 功    能:设置DSP 打印上报速率
* 函数类型: INT32
* 参    数: 
* 参数名称          类型        输入/输出         描述
* ucCpuNo                   UINT8              in                          dsp(5~16)
* ulRate            UINT32      输入              打印输出速率（单位：条/秒）
* 函数返回: 成功执行返回CPSS_OK, 否则返回CPSS_ERROR.
* 说    明: 该函数实现对打印速率的设置，系统启动时会设置默认的打印速率（50条/秒）。
*******************************************************************************/

INT32 cpss_print_rate_set_dsp 
(
UINT8 ucCpuNo,
UINT32 ulRate
)
{
   CPSS_CPS__OAMS_PRT_RATE_SET_T stRate;
   CPSS_COM_PID_T stDstPid;

    if(ulRate > 256 )
        {
           return CPSS_ERROR;
        }
      /* 获取dsp的逻辑地址*/
    if(CPSS_OK != cpss_com_dsp_logic_addr_get(ucCpuNo,&stDstPid.stLogicAddr))
     {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_WARN," cpss_com_dsp_logic_addr_get fail in cpss_print_rate_set_dsp\n\r");
            
        return CPSS_ERROR;
     }

     stDstPid.ulAddrFlag = 0;
     stDstPid.ulPd = CPSS_DBG_MNGR_PROC;
     
     stRate.ulRate = cpss_htonl(ulRate);
    
     return(cpss_com_send(&stDstPid,CPSS_CPS__OAMS_PRT_RATE_SET_MSG,(UINT8 *)&stRate,sizeof(CPSS_CPS__OAMS_PRT_RATE_SET_T)));
       
  
  
}

/*******************************************************************************
* 函数名称: cpss_log_rate_set_dsp
* 功    能:设置dsp 日志上报速率
* 函数类型: 
* 参    数: 
* 参数名称            类型        输入/输出         描述
* ucCpuNo                   UINT8              in                          dsp(5~16)
* ulRate            UINT32        输入            日志上报速率（单位：条/秒）
* 函数返回: 成功执行返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 日志初始化时，系统会设置默认的上报速率（50条/秒）。
*******************************************************************************/

INT32 cpss_log_rate_set_dsp 
(
UINT8 ucCpuNo,
UINT32 ulRate 
)
{
    CPSS_CPS__OAMS_PRT_RATE_SET_T stRate;
    CPSS_COM_PID_T stDstPid;

    if(ulRate > 256 )
        {
           return CPSS_ERROR;
        }
       /* 获取dsp的逻辑地址*/
    if(CPSS_OK != cpss_com_dsp_logic_addr_get(ucCpuNo,&stDstPid.stLogicAddr))
     {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_WARN," cpss_com_dsp_logic_addr_get fail in cpss_log_rate_set_dsp\n\r");
            
        return CPSS_ERROR;
     }

     stDstPid.ulAddrFlag = 0;
     stDstPid.ulPd = CPSS_DBG_MNGR_PROC;
    
     stRate.ulRate = cpss_htonl(ulRate);
    
     return(cpss_com_send(&stDstPid,CPSS_CPS__OAMS_LOG_RATE_SET_MSG,(UINT8 *)&stRate,sizeof(CPSS_CPS__OAMS_PRT_RATE_SET_T)));
       
   
}

/*******************************************************************************
* 函数名称: cpss_log_level_set_dsp
* 功    能:设置dsp  日志级别
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出         描述
* ucCpuNo                   UINT8              in                          dsp(5~16)
* ucLogLevel          UINT8       输入              日志级别
* 函数返回: 成功设置返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 设置日志上报的级别，只允许该级别的日志上报。
*******************************************************************************/

INT32 cpss_log_level_set_dsp 
(
UINT8 ucCpuNo,
UINT8 ucLogLevel
)
{
    CPSS_CPS__OAMS_LOG_LEVEL_SET_T stLevel;
    CPSS_COM_PID_T stDstPid;
    
     if (ucLogLevel > CPSS_LOG_DETAIL)
    {
        return (CPSS_ERROR);
    }

       /* 获取dsp的逻辑地址*/
    if(CPSS_OK != cpss_com_dsp_logic_addr_get(ucCpuNo,&stDstPid.stLogicAddr))
     {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_WARN," cpss_com_dsp_logic_addr_get fail in cpss_log_level_set_dsp\n\r");
            
        return CPSS_ERROR;
     }

     stDstPid.ulAddrFlag = 0;
     stDstPid.ulPd = CPSS_DBG_MNGR_PROC;
  
     stLevel.ucLogLevel = ucLogLevel;
    
     return(cpss_com_send(&stDstPid,CPSS_CPS__OAMS_LOG_LEVEL_SET_MSG,(UINT8 *)&stLevel,sizeof(CPSS_CPS__OAMS_LOG_LEVEL_SET_T)));
       
   
}

UINT32 gulFlagIsHighSopport = FALSE;
UINT32 gulHighFrequency = 100;

/*******************************************************************************
* 函数名称: cpss_dbg_high_time_init
* 功    能: 调试用的高精度时间获取部分初始化
* 函数类型: 
* 参    数: 无
* 参数名称            类型            输入/输出         描述
* 函数返回: 无
*******************************************************************************/
void cpss_dbg_high_time_init()
{
#if defined CPSS_VOS_WINDOWS
    LARGE_INTEGER liFrequency;
    if(0 == QueryPerformanceFrequency(&liFrequency))
    {
        gulFlagIsHighSopport = FALSE;
        return;
    }
    gulHighFrequency = liFrequency.u.LowPart;
#elif defined CPSS_VOS_VXWORKS
    gulHighFrequency = sysTimestampFreq();
#else
    gulHighFrequency = 100; /* DSP: 10ms */
#endif
    if(0 == gulHighFrequency)
    {
        gulFlagIsHighSopport = FALSE;
    }
    else
    {
        gulFlagIsHighSopport = TRUE;
    }
}

/*******************************************************************************
* 函数名称: cpss_dbg_high_time_get
* 功    能: 获取当前的us数值
* 函数类型: 
* 参    数: 无
* 参数名称            类型            输入/输出         描述
* 函数返回: 获取当前的us数值
*******************************************************************************/
UINT32 cpss_dbg_high_time_get()
{
#if defined CPSS_VOS_WINDOWS
    if(TRUE == gulFlagIsHighSopport)
    {
        LARGE_INTEGER liCounter;
        QueryPerformanceCounter(&liCounter);
        return (UINT32)(liCounter.QuadPart * 1000000 / gulHighFrequency);
    }
    return GetTickCount();
#elif defined CPSS_VOS_VXWORKS
    UINT32 ulTemp;

    if(TRUE == gulFlagIsHighSopport)
    {
        /*==== 一个单位时间的时钟计数 ====*/
        ulTemp = gulHighFrequency / 1000000;
        /*==== 单位时间的个数 ====*/
        if(0 == ulTemp)
        {
            ulTemp = 1000000 / gulHighFrequency;
            ulTemp = sysTimestamp() * ulTemp;
        }
        else
        {
            ulTemp = sysTimestamp() / ulTemp;
            ulTemp = ulTemp % 10000;
            ulTemp = ulTemp + tickGet() * 10000;
        }
        return ulTemp;
    }
    return tickGet() * 10000;
#else
    return cpss_tick_get() * 10000;
#endif
}

#if defined CPSS_DBG_REC_TIME_ON

CPSS_DBG_REC_TIME_HEAD_T *gpCpssRecTimeHead = NULL;

/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_lock
* 功    能: 封装的taskLock
* 函数类型: 
* 参    数: 无
* 参数名称            类型            输入/输出         描述
* 函数返回: 锁任务
*******************************************************************************/
void cpss_dbg_rec_time_lock()
{
#ifdef CPSS_VOS_VXWORKS
    taskLock();
#endif
}

/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_unlock
* 功    能: 封装的taskUnlock
* 函数类型: 
* 参    数: 无
* 参数名称            类型            输入/输出         描述
* 函数返回: 解锁任务
*******************************************************************************/
void cpss_dbg_rec_time_unlock()
{
#ifdef CPSS_VOS_VXWORKS
    taskUnlock();
#endif
}

/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_init
* 功    能: 打点程序的初始化，需要在CPSS的初始化中调用
* 函数类型: 
* 参    数: 无
* 参数名称            类型            输入/输出         描述
* 函数返回: 
    成功： CPSS_OK
    失败： CPSS_ERROR
*******************************************************************************/
INT32 cpss_dbg_rec_time_init()
{
    if(NULL != gpCpssRecTimeHead)
    {
        return CPSS_ERROR;
    }
    gpCpssRecTimeHead = malloc(sizeof(CPSS_DBG_REC_TIME_HEAD_T) * CPSS_DBG_REC_TIME_MAX_RECID_SUM);
    if(NULL == gpCpssRecTimeHead)
    {
        return CPSS_ERROR;
    }
    memset(gpCpssRecTimeHead, 0, sizeof(CPSS_DBG_REC_TIME_HEAD_T) * CPSS_DBG_REC_TIME_MAX_RECID_SUM);
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_reg
* 功    能: 打点程序的初始化，需要在CPSS的初始化中调用
* 函数类型: 
* 参    数: 3
* 参数名称             类型            输入/输出         描述
记录项的名称           UINT8 *         输入              为空记录"UNKNOW"
记录项中记录的最大个数 UINT32          输入              为0取缺省值
是否需要任务锁         UINT32          输入              TRUE：需要，FALSE：不需要
* 函数返回: 
    成功： 记录项的编号
    失败： 无效的记录项编号 CPSS_DBG_REC_TIME_INVALID_RECID
*******************************************************************************/
UINT32 cpss_dbg_rec_time_reg(UINT8 *pucRecName, UINT32 ulRecSum, UINT32 ulIsNeedLock)
{
    UINT32 ulRecLoop;

    if(NULL == gpCpssRecTimeHead)
    {
        return CPSS_DBG_REC_TIME_INVALID_RECID;
    }
    if(0 == ulRecSum)
    {
        ulRecSum = CPSS_DBG_REC_TIME_DEF_REC_SUM;
    }
    for(ulRecLoop = 0; ulRecLoop < CPSS_DBG_REC_TIME_MAX_RECID_SUM; ulRecLoop++)
    {
        if(0 == gpCpssRecTimeHead[ulRecLoop].ulMaxRecSum)
        {
/*
            gpCpssRecTimeHead[ulRecLoop].ulSemId = cpss_vos_mutex_create();
            cpss_vos_mutex_p(gpCpssRecTimeHead[ulRecLoop].ulSemId, WAIT_FOREVER);
*/
            gpCpssRecTimeHead[ulRecLoop].ulMaxRecSum = (0 == ulRecSum) ? CPSS_DBG_REC_TIME_DEF_REC_SUM : ulRecSum;
            gpCpssRecTimeHead[ulRecLoop].ulIsNeedLock = ulIsNeedLock;
            gpCpssRecTimeHead[ulRecLoop].ulCurPosition = 0;
            if(NULL == pucRecName)
            {
                strcpy(gpCpssRecTimeHead[ulRecLoop].aucRecName, "UNKNOW");
            }
            else
            {
                strcpy(gpCpssRecTimeHead[ulRecLoop].aucRecName, pucRecName);
            }
/*
            cpss_vos_mutex_v(gpCpssRecTimeHead[ulRecLoop].ulSemId);
*/
            return ulRecLoop;
        }
    }
    return CPSS_DBG_REC_TIME_INVALID_RECID;
}

/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_is_enable
* 功    能: 根据记录项的编号查看记录项是否有效，即是否成功注册并增加到记录列表中
* 函数类型: 
* 参    数: 1
* 参数名称             类型            输入/输出         描述
记录项的编号           UINT32          输入              
* 函数返回: 
    成功： CPSS_OK，成功注册并增加到记录列表中
    失败： CPSS_ERROR，无效，没有注册或者没有加到记录列表中
*******************************************************************************/
UINT32 cpss_dbg_rec_time_is_enable(UINT32 ulRecId)
{
    if((NULL == gpCpssRecTimeHead) 
        || (CPSS_DBG_REC_TIME_MAX_RECID_SUM <= ulRecId)
        || (NULL == gpCpssRecTimeHead[ulRecId].pRecInfo) 
        || (0 == gpCpssRecTimeHead[ulRecId].ulMaxRecSum))
    {
        return CPSS_ERROR;
    }
    return CPSS_OK;
}
/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_delete
* 功    能: 从记录列表中删除一个记录项，不再统计，该函数需要加到LDT的模拟Shell中
* 函数类型: 
* 参    数: 1
* 参数名称             类型            输入/输出         描述
记录项的编号           UINT32          输入              
* 函数返回: 
    成功： CPSS_OK
    失败： CPSS_ERROR
*******************************************************************************/
INT32 cpss_dbg_rec_time_delete(UINT32 ulRecId)
{
    if((NULL == gpCpssRecTimeHead) 
        || (CPSS_DBG_REC_TIME_MAX_RECID_SUM <= ulRecId)
        || (NULL == gpCpssRecTimeHead[ulRecId].pRecInfo))
    {
        cps__oams_shcmd_printf("record time [%d] delete failure, because no this record.\n", ulRecId);
        return CPSS_ERROR;
    }
/*
    cpss_vos_mutex_p(gpCpssRecTimeHead[ulRecId].ulSemId, WAIT_FOREVER);
*/
    free(gpCpssRecTimeHead[ulRecId].pRecInfo);
    gpCpssRecTimeHead[ulRecId].pRecInfo = NULL;
    gpCpssRecTimeHead[ulRecId].ulCurPosition = 0;
/*
    cpss_vos_mutex_v(gpCpssRecTimeHead[ulRecId].ulSemId);
*/
    cps__oams_shcmd_printf("record time [%d] delete success.\n", ulRecId);
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_delete_all
* 功    能: 从记录列表中删除所有记录项，不再统计，该函数需要加到LDT的模拟Shell中
* 函数类型: 
* 参    数: 无
* 参数名称             类型            输入/输出         描述
* 函数返回: 
    成功： CPSS_OK
    失败： CPSS_ERROR
*******************************************************************************/
INT32 cpss_dbg_rec_time_delete_all()
{
    UINT32 ulRecLoop;

    if(NULL == gpCpssRecTimeHead)
    {
        cps__oams_shcmd_printf("time show failure, because has not been inited.\n");
        return CPSS_ERROR;
    }
    for(ulRecLoop = 0; ulRecLoop < CPSS_DBG_REC_TIME_MAX_RECID_SUM; ulRecLoop++)
    {
        if(CPSS_OK == cpss_dbg_rec_time_is_enable(ulRecLoop))
        {
            cpss_dbg_rec_time_delete(ulRecLoop);
        }
    }
    cps__oams_shcmd_printf("all record time deleted.\n");
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_add
* 功    能: 给记录列表中增加一个记录项，运行统计，该函数需要加到LDT的模拟Shell中
* 函数类型: 
* 参    数: 1
* 参数名称             类型            输入/输出         描述
记录项编号             UINT32          输入
* 函数返回: 
    成功： CPSS_OK
    失败： CPSS_ERROR
*******************************************************************************/
INT32 cpss_dbg_rec_time_add(UINT32 ulRecId)
{
    if((NULL == gpCpssRecTimeHead) 
        || (CPSS_DBG_REC_TIME_MAX_RECID_SUM <= ulRecId)
        || (0 == gpCpssRecTimeHead[ulRecId].ulMaxRecSum))
    {
        cps__oams_shcmd_printf("record time [%d] add failure, because not init or has been used.\n", ulRecId);
        return CPSS_ERROR;
    }
/*
    cpss_vos_mutex_p(gpCpssRecTimeHead[ulRecId].ulSemId, WAIT_FOREVER);
*/
    gpCpssRecTimeHead[ulRecId].pRecInfo = malloc(sizeof(CPSS_DBG_TIME_REC_ITEM_T) * gpCpssRecTimeHead[ulRecId].ulMaxRecSum);
    if(NULL == gpCpssRecTimeHead[ulRecId].pRecInfo)
    {
/*
        cpss_vos_mutex_v(gpCpssRecTimeHead[ulRecId].ulSemId);
*/
        cps__oams_shcmd_printf("record time [%d] add failure, because malloc failure.\n", ulRecId);
        return CPSS_ERROR;
    }
    memset(gpCpssRecTimeHead[ulRecId].pRecInfo, 0, sizeof(CPSS_DBG_TIME_REC_ITEM_T) * gpCpssRecTimeHead[ulRecId].ulMaxRecSum);
    gpCpssRecTimeHead[ulRecId].ulCurPosition = 0;
/*
    cpss_vos_mutex_v(gpCpssRecTimeHead[ulRecId].ulSemId);
*/
    cps__oams_shcmd_printf("record time [%d] add success.\n", ulRecId);
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_add_all
* 功    能: 给记录列表中增加所有记录项，运行统计，该函数需要加到LDT的模拟Shell中
* 函数类型: 
* 参    数: 无
* 参数名称             类型            输入/输出         描述
* 函数返回: 
    成功： CPSS_OK
    失败： CPSS_ERROR
*******************************************************************************/
INT32 cpss_dbg_rec_time_add_all()
{
    UINT32 ulRecLoop;

    if(NULL == gpCpssRecTimeHead)
    {
        cps__oams_shcmd_printf("add all failure, because has not been inited.\n");
        return CPSS_ERROR;
    }
    for(ulRecLoop = 0; ulRecLoop < CPSS_DBG_REC_TIME_MAX_RECID_SUM; ulRecLoop++)
    {
        if((0 != gpCpssRecTimeHead[ulRecLoop].ulMaxRecSum) && (NULL == gpCpssRecTimeHead[ulRecLoop].pRecInfo))
        {
            cpss_dbg_rec_time_add(ulRecLoop);
        }
    }
    cps__oams_shcmd_printf("all record time added.\n");
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_task_show_byid
* 功    能: 根据记录项编号打印记录项的信息，为cpss_dbg_rec_time_task_show提供
* 函数类型: 
* 参    数: 1
* 参数名称             类型            输入/输出         描述
记录项编号             UINT32          输入
* 函数返回: 
    成功： CPSS_OK
    失败： CPSS_ERROR
*******************************************************************************/
INT32 cpss_dbg_rec_time_task_show_byid(UINT32 ulRecId)
{
    UINT8 aucStateString[10];
    UINT8 aucLockString[10];
    if(NULL == gpCpssRecTimeHead[ulRecId].pRecInfo)
    {
        strcpy(aucStateString, "STOP");
    }
    else
    {
        strcpy(aucStateString, "START");
    }
    if(TRUE == gpCpssRecTimeHead[ulRecId].ulIsNeedLock)
    {
        strcpy(aucLockString, "LOCKED");
    }
    else
    {
        strcpy(aucLockString, "NOLOCK");
    }
    cps__oams_shcmd_printf("%-8d %-8s %-8s %-8d %-8d %-8d %-32s \n",
        ulRecId,
        aucStateString,
        aucLockString,
        gpCpssRecTimeHead[ulRecId].ulMaxRecSum,
        gpCpssRecTimeHead[ulRecId].ulCurPosition,
        gpCpssRecTimeHead[ulRecId].ulSemId,
        gpCpssRecTimeHead[ulRecId].aucRecName);
    return CPSS_OK;
}
/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_task_show
* 功    能: 打印所有记录项的信息，需要增加到LDT的模拟Shell上
* 函数类型: 
* 参    数: 无
* 参数名称             类型            输入/输出         描述
* 函数返回: 
    成功： CPSS_OK
    失败： CPSS_ERROR
*******************************************************************************/
INT32 cpss_dbg_rec_time_task_show()
{
    UINT32 ulRecId;

    if(NULL == gpCpssRecTimeHead)
    {
        cps__oams_shcmd_printf("task show failure, because has not been inited.\n");
        return CPSS_ERROR;
    }
    cps__oams_shcmd_printf("%-8s %-8s %-8s %-8s %-8s %-8s %-32s \n",
        "REC_ID", "STATE", "LOCK", "MAX_REC", "CUR_POS", "SEM_ID", "REC_NAME");
    for(ulRecId = 0; ulRecId < CPSS_DBG_REC_TIME_MAX_RECID_SUM; ulRecId++)
    {
        if(0 != gpCpssRecTimeHead[ulRecId].ulMaxRecSum)
        {
            cpss_dbg_rec_time_task_show_byid(ulRecId);
        }
    }
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_show_byid
* 功    能: 打印记录项的统计信息，为cpss_dbg_rec_time_show提供
* 函数类型: 
* 参    数: 3
* 参数名称             类型            输入/输出         描述
记录项编号             UINT32          输入
最小执行时间           UINT32          输入              只打印执行时间比该值大的记录
最小开始时间           UINT32          输入              只打印开始时间比该值大的记录
最大开始时间           UINT32          输入              只打印开始时间比该值小的记录
* 函数返回: 
    成功： CPSS_OK
    失败： CPSS_ERROR
*******************************************************************************/
INT32 cpss_dbg_rec_time_show_byid(UINT32 ulRecId, UINT32 ulMinTick, UINT32 ulMinStartTick, UINT32 ulMaxStartTick)
{
    UINT32 ulLoop;
    UINT32 ulLoop2;
    UINT32 ulCurPos;
    UINT32 ulDataLen;
    UINT8 aucAbsTimeString[50];
    UINT8 aucDataString[CPSS_DBG_REC_TIME_MAX_DATA_BUF_LEN * 4];
    UINT32 ulMaxSum = gpCpssRecTimeHead[ulRecId].ulMaxRecSum;
    UINT32 ulRecPos = gpCpssRecTimeHead[ulRecId].ulCurPosition;
    CPSS_DBG_TIME_REC_ITEM_T *pRecInfo = gpCpssRecTimeHead[ulRecId].pRecInfo;

    cps__oams_shcmd_printf("\n%-8s %-8s %-8s %-8s %-8s %-8s %-32s \n",
        "REC_ID", "STATE", "LOCK", "MAX_REC", "CUR_POS", "SEM_ID", "REC_NAME");
    cpss_dbg_rec_time_task_show_byid(ulRecId);

    cps__oams_shcmd_printf("\n%-8s %-10s %-8s %-10s %-8s %-32s \n",
        "PARA_HEX", "START_US", "ABS_TIME", "USE_US", "RET_HEX", "REC_INFO");
    for(ulLoop = 0; ulLoop < ulMaxSum; ulLoop++)
    {
        if((0 == pRecInfo[ulLoop].ulEndTime) && (0 == pRecInfo[ulLoop].ulStartTime))
        {
            continue;
        }
        if((pRecInfo[ulLoop].ulEndTime - pRecInfo[ulLoop].ulStartTime) < ulMinTick)
        {
            continue;
        }
        if((pRecInfo[ulLoop].ulStartTime > ulMaxStartTick) && (0 != ulMaxStartTick))
        {
            continue;
        }
        if((pRecInfo[ulLoop].ulStartTime < ulMinStartTick) && (0 != ulMinStartTick))
        {
            continue;
        }
        ulCurPos = (ulRecPos + ulLoop) % ulMaxSum;
        aucDataString[0] = 0;
        snprintf(aucDataString, CPSS_DBG_REC_TIME_MAX_DATA_BUF_LEN * 4, "[%d]", pRecInfo[ulLoop].ulDataLen);
        ulDataLen = (pRecInfo[ulLoop].ulDataLen > CPSS_DBG_REC_TIME_MAX_DATA_BUF_LEN) ? CPSS_DBG_REC_TIME_MAX_DATA_BUF_LEN : pRecInfo[ulLoop].ulDataLen;
        for(ulLoop2 = 0; ulLoop2 < ulDataLen; ulLoop2++)
        {
            snprintf(&aucDataString[strlen(aucDataString)], CPSS_DBG_REC_TIME_MAX_DATA_BUF_LEN * 4 - strlen(aucDataString), "%02X", pRecInfo[ulLoop].aucDataBuf[ulLoop2]);
        }
        snprintf(aucAbsTimeString, 50, "%02d:%02d:%02d", 
            pRecInfo[ulLoop].tAbsTime.ucHour,
            pRecInfo[ulLoop].tAbsTime.ucMinute,
            pRecInfo[ulLoop].tAbsTime.ucSecond);
        cps__oams_shcmd_printf("%08X %-10u %-8s %-10u %08X %-32s \n",
            pRecInfo[ulLoop].ulPara,
            pRecInfo[ulLoop].ulStartTime,
            aucAbsTimeString,
            pRecInfo[ulLoop].ulEndTime - pRecInfo[ulLoop].ulStartTime,
            pRecInfo[ulLoop].ulResult,
            aucDataString);
    }
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_show
* 功    能: 打印记录项的统计信息，该函数需要增加到LDT的模拟Shell上
* 函数类型: 
* 参    数: 3
* 参数名称             类型            输入/输出         描述
记录项编号             UINT32          输入              为0，打印所有记录项的统计信息
最小执行时间           UINT32          输入              只打印执行时间比该值大的记录
最小开始时间           UINT32          输入              只打印开始时间比该值大的记录
最大开始时间           UINT32          输入              只打印开始时间比该值小的记录
* 函数返回: 
    成功： CPSS_OK
    失败： CPSS_ERROR
*******************************************************************************/
INT32 cpss_dbg_rec_time_show(UINT32 ulRecId, UINT32 ulMinTick, UINT32 ulMinStartTick, UINT32 ulMaxStartTick)
{
    UINT32 ulRecLoop;

    if(NULL == gpCpssRecTimeHead)
    {
        cps__oams_shcmd_printf("time show failure, because has not been inited.\n");
        return CPSS_ERROR;
    }
    if(CPSS_DBG_REC_TIME_INVALID_RECID == ulRecId)
    {
        for(ulRecLoop = 0; ulRecLoop < CPSS_DBG_REC_TIME_MAX_RECID_SUM; ulRecLoop++)
        {
            if(CPSS_OK == cpss_dbg_rec_time_is_enable(ulRecLoop))
            {
                cpss_dbg_rec_time_show_byid(ulRecLoop, ulMinTick, ulMinStartTick, ulMaxStartTick);
            }
        }
        return CPSS_OK;
    }
    if(CPSS_OK != cpss_dbg_rec_time_is_enable(ulRecId))
    {
        cps__oams_shcmd_printf("record time show failure, because record [%d] is invalid.\n", ulRecId);
        return CPSS_ERROR;
    }
    cpss_dbg_rec_time_show_byid(ulRecId, ulMinTick, ulMinStartTick, ulMaxStartTick);
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_start
* 功    能: 记录一个记录的开始时间，由用户调用
* 函数类型: 
* 参    数: 3
* 参数名称             类型            输入/输出         描述
记录项编号             UINT32          输入
定位信息数据长度       UINT32          输入              为0不记录定位信息
定位信息数据缓存       UINT8*          输入              为空不记录定位信息
* 函数返回: 
    成功： CPSS_OK
    失败： CPSS_ERROR
*******************************************************************************/
INT32 cpss_dbg_rec_time_start(UINT32 ulRecId, UINT32 ulPara, UINT32 ulDataLen, UINT8 *pucDataBuf)
{
    if(CPSS_OK != cpss_dbg_rec_time_is_enable(ulRecId))
    {
        return CPSS_ERROR;
    }

    if((0 == ulDataLen) || (NULL == pucDataBuf))
    {
        gpCpssRecTimeHead[ulRecId].pRecInfo[gpCpssRecTimeHead[ulRecId].ulCurPosition].ulDataLen = 0;
    }
    else
    {
        gpCpssRecTimeHead[ulRecId].pRecInfo[gpCpssRecTimeHead[ulRecId].ulCurPosition].ulDataLen = ulDataLen;
        ulDataLen = (CPSS_DBG_REC_TIME_MAX_DATA_BUF_LEN < ulDataLen) ? CPSS_DBG_REC_TIME_MAX_DATA_BUF_LEN : ulDataLen;
        memcpy(gpCpssRecTimeHead[ulRecId].pRecInfo[gpCpssRecTimeHead[ulRecId].ulCurPosition].aucDataBuf, pucDataBuf, ulDataLen);
    }
    gpCpssRecTimeHead[ulRecId].pRecInfo[gpCpssRecTimeHead[ulRecId].ulCurPosition].ulPara = ulPara;
    gpCpssRecTimeHead[ulRecId].pRecInfo[gpCpssRecTimeHead[ulRecId].ulCurPosition].ulStartTime = cpss_dbg_high_time_get();
    gpCpssRecTimeHead[ulRecId].pRecInfo[gpCpssRecTimeHead[ulRecId].ulCurPosition].ulEndTime 
        = 100000000 + gpCpssRecTimeHead[ulRecId].pRecInfo[gpCpssRecTimeHead[ulRecId].ulCurPosition].ulStartTime;
    cpss_clock_get(&gpCpssRecTimeHead[ulRecId].pRecInfo[gpCpssRecTimeHead[ulRecId].ulCurPosition].tAbsTime);
    if(TRUE == gpCpssRecTimeHead[ulRecId].ulIsNeedLock)
    {
        cpss_dbg_rec_time_lock();
    }
    gpCpssRecTimeHead[ulRecId].ulCurPosition = (gpCpssRecTimeHead[ulRecId].ulCurPosition + 1) % gpCpssRecTimeHead[ulRecId].ulMaxRecSum;
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_dbg_rec_time_end
* 功    能: 记录一个记录的结束时间，由用户调用
* 函数类型: 
* 参    数: 1
* 参数名称             类型            输入/输出         描述
记录项编号             UINT32          输入
* 函数返回: 
    成功： CPSS_OK
    失败： CPSS_ERROR
*******************************************************************************/
INT32 cpss_dbg_rec_time_end(UINT32 ulRecId, UINT32 ulResult, UINT32 ulPara)
{
    UINT32 ulLoop;
    UINT32 ulCurPosition;

    if(CPSS_OK != cpss_dbg_rec_time_is_enable(ulRecId))
    {
        return CPSS_ERROR;
    }
    for(ulLoop = 0; ulLoop < gpCpssRecTimeHead[ulRecId].ulMaxRecSum; ulLoop++)
    {
        ulCurPosition = (gpCpssRecTimeHead[ulRecId].ulCurPosition - ulLoop - 1) % gpCpssRecTimeHead[ulRecId].ulMaxRecSum;
        if((gpCpssRecTimeHead[ulRecId].pRecInfo[ulCurPosition].ulPara == ulPara)
            && ((gpCpssRecTimeHead[ulRecId].pRecInfo[ulCurPosition].ulEndTime 
                - gpCpssRecTimeHead[ulRecId].pRecInfo[ulCurPosition].ulStartTime) == 100000000))
        {
            gpCpssRecTimeHead[ulRecId].pRecInfo[ulCurPosition].ulEndTime = cpss_dbg_high_time_get();
            gpCpssRecTimeHead[ulRecId].pRecInfo[ulCurPosition].ulResult = ulResult;
            if(TRUE == gpCpssRecTimeHead[ulRecId].ulIsNeedLock)
            {
                cpss_dbg_rec_time_unlock();
            }
            return CPSS_OK;
        }
    }
    return CPSS_ERROR;
}

#endif /* endof defined CPSS_DBG_REC_TIME_ON */

/*************************following  code  used   for  replace dsp vsprintf   **************************/

#ifndef AF_INET
#define AF_INET 2
#endif

struct pal_in4_addr
{
    UINT32 uladdr;
};

/* IPv4 and IPv6 unified prefix structure. */
struct prefix
{
  UINT8 family;
  UINT8 prefixlen;
  UINT8 pad1;
  UINT8 pad2;
  union 
  {
    UINT8 prefix;
    struct pal_in4_addr prefix4;
    struct 
    {
      struct pal_in4_addr id;
      struct pal_in4_addr adv_router;
    } lp;
    UINT8 val[9];
  } u;
};

/* IPv4 prefix structure */
struct prefix_ipv4
{
  UINT8 family;
  UINT8 prefixlen;
  UINT8 pad1;
  UINT8 pad2;
  struct pal_in4_addr prefix;
};

/*
** pal_modf()
*/
/* 分拆浮点数x为小数部分(返回值)和整数部分(*y) 
   主要作用在格式化输出时,格式为 %f的情形 */
double
pal_modf (double x, double *y)
{
  return 0.1;/* modf(x,y); */
}

/*
** pal_isnan()
*/
/* 判断一个数是否是NAN 需要包含 float.h 暂不支持 
   用在格式化输出时,格式为 %f 的情形 */
int pal_isnan (double x)
{
    return 0;
#if 0
#ifdef WIN32
	return (int) _isnan((double)x);
#else
   union 
   {
      unsigned long long l;
      double d;
   } u;
   u.d = x;
   return (u.l==0x7FF8000000000000ll || u.l==0x7FF0000000000000ll || u.l==0xfff8000000000000ll);
#endif	
#endif
}


/* Defines. */
#define ZVSNP_ERROR_PARSE	       1
#define ZVSNP_ERROR_OVERFLOW	       2

#define ZVSNP_TYPE_NONE		       0
#define ZVSNP_TYPE_SHORT	       1
#define ZVSNP_TYPE_INT		       2
#define ZVSNP_TYPE_LONG		       3
#define ZVSNP_TYPE_U_CHAR	       4
#define ZVSNP_TYPE_U_SHORT	       5
#define ZVSNP_TYPE_U_INT	       6
#define ZVSNP_TYPE_U_LONG	       7
#define ZVSNP_TYPE_DOUBLE	       8
#define ZVSNP_TYPE_LONG_DOUBLE	       9
#define ZVSNP_TYPE_POINTER	      10

#define ZVSNP_SIGN_PLUS                0
#define ZVSNP_SIGN_MINUS               1

#define ZVSNP_STATE_BEGIN              0
#define ZVSNP_STATE_FLAG	       1
#define ZVSNP_STATE_WIDTH	       2
#define ZVSNP_STATE_PRECDOT	       3
#define ZVSNP_STATE_PRECISION	       4
#define ZVSNP_STATE_QUALIFIER	       5
#define ZVSNP_STATE_OPERATOR	       6
#define ZVSNP_STATE_PERCENT	       7
#define ZVSNP_STATE_END		       8
#define ZVSNP_STATE_MAX		       9

#define ZVSNP_CLASS_FLAG               1
#define ZVSNP_CLASS_WIDTH	       2
#define ZVSNP_CLASS_PRECDOT	       3
#define ZVSNP_CLASS_PRECISION	       4
#define ZVSNP_CLASS_QUALIFIER	       5
#define ZVSNP_CLASS_OPERATOR           6
#define ZVSNP_CLASS_PERCENT            7
#define ZVSNP_CLASS_MAX		       8

#define ZVSNP_FLAG_NONE                0
#define ZVSNP_FLAG_MINUS	(1 << 0)
#define ZVSNP_FLAG_ZERO		(1 << 1)
#define ZVSNP_FLAG_PLUS		(1 << 2)
#define ZVSNP_FLAG_SPACE	(1 << 3)
#define ZVSNP_FLAG_HASH		(1 << 4)

#define ZVSNP_QUAL_DEFAULT             0
#define ZVSNP_QUAL_SHORT	       1
#define ZVSNP_QUAL_LONG		       2
#define ZVSNP_QUAL_LONG_DOUBLE         3

#define ZVSNP_NUM_STRLEN_MAX          16
#define ZVSNP_IN_ADDR_STRLEN_MAX      16
#define ZVSNP_IN6_ADDR_STRLEN_MAX     40
#define ZVSNP_PREFIX_IPV4_STRLEN_MAX  19
#define ZVSNP_PREFIX_IPV6_STRLEN_MAX  44

#define ZVSNP_BASE_DECIMAL            10
#define ZVSNP_BASE_OCTAL               8
#define ZVSNP_BASE_HEXA               16

/* Structures. */
struct zvsnp
{
  UINT8 flags;
  UINT8 width_star;
  INT32 width;
  INT32 prec;
  UINT8 qual;
  UINT8 opr;

  char *sp;
  char *ep;
};

struct zvsnp_val
{
  UINT8 sign;
  union
  {
    UINT8 c;
    UINT32 ul;
    double df;
    void *p;
  } u;
};

/* Macros. */
#define ZVSNP_CLASS(C)         (zvsnp_class[(C) - ' '])
#define ZVSNP_CLASS_RANGE(C)   ((C) >= ' ' && (C) <= 'z')
#define ZVSNP_FLAG(C)          (zvsnp_flags[(C) - ' '])
#define ZVSNP_FLAG_RANGE(C)    ((C) >= ' ' && (C) <= '0')
#define ZVSNP_DIGIT(C)         ((C) - '0')
#define ZVSNP_QUAL(C)	       ((C) == 'h' ? ZVSNP_QUAL_SHORT :               \
				((C) == 'l' ? ZVSNP_QUAL_LONG :               \
				 ((C) == 'L' ? ZVSNP_QUAL_LONG_DOUBLE :       \
				  ZVSNP_QUAL_DEFAULT)))
#define ZVSNP_QUAL_RANGE(C)    ((C) >= 'L' && (C) <= 'l')
#define ZVSNP_PAD(Z)	       ((Z)->flags & ZVSNP_FLAG_ZERO ? '0' : ' ')
#define ZVSNP_ALIGN(Z)	       ((Z)->flags & ZVSNP_FLAG_MINUS)
#define ZVSNP_EXPAND(Z,P,L,V)                                                 \
    zv_expand[(Z)->opr].func[ZVSNP_ALIGN (Z)] ((P), (L), (Z), (V))

#define ZVSNP_SIGN_CHAR_GET(Z,S,C,L)                                          \
  do {                                                                        \
    (C) = NULL;                                                               \
    (L) = 0;                                                                  \
    if ((S))                                                                  \
      {                                                                       \
        (C) = "-";                                                            \
        (L) = 1;                                                              \
      }                                                                       \
    else                                                                      \
      {                                                                       \
        if ((Z)->flags & ZVSNP_FLAG_PLUS)                                     \
          {                                                                   \
            (C) = "+";                                                        \
            (L) = 1;                                                          \
          }                                                                   \
        else if ((Z)->flags & ZVSNP_FLAG_SPACE)                               \
          {                                                                   \
            (C) = " ";                                                        \
            (L) = 1;                                                          \
          }                                                                   \
       }                                                                      \
  } while (0)

#define ZVSNP_EXPAND_OCTET(P,V)                                               \
    do {                                                                      \
      int _val = (V);                                                         \
      char _a, _b, _c;                                                        \
      _a = CPSS_DIV(_val , 100);                                                        \
      _b = CPSS_DIV(CPSS_MOD(_val ,100) , 10);                                                 \
      _c = CPSS_MOD(_val ,10);                                                         \
      if (_a)                                                                 \
        *(P)++ = '0' + _a;                                                    \
      if (_a || _b)                                                           \
        *(P)++ = '0' + _b;                                                    \
      *(P)++ = '0' + _c;                                                      \
    } while (0)

#define ZVSNP_STRLEN(S,L)                                                     \
    do {                                                                      \
      char *_zv_p = (S);                                                      \
      (L) = 0;                                                                \
      while (*_zv_p++ != '\0')                                                \
        (L)=(L)+1;                                                                \
    } while (0)

#define ZVSNP_MEMCPY(P,S,L,Z,W)                                               \
    do {                                                                      \
      int _zv_i;                                                              \
      int _zv_len = (P) + (Z) < (L) ? (Z) : (L) - (P);                        \
      if ((Z) <= 0)                                                           \
        break;                                                                \
      for (_zv_i = 0; _zv_i < _zv_len; _zv_i++)                               \
        *((P) + _zv_i) = *((S) + _zv_i);                                      \
      (P) += _zv_len;                                                         \
      (W) += (Z);                                                             \
    } while (0)

#define ZVSNP_MEMSET(P,C,L,Z,W)                                               \
    do {                                                                      \
      int _zv_i;                                                              \
      int _zv_len = (P) + (Z) < (L) ? (Z) : (L) - (P);                        \
      if ((Z) <= 0)                                                           \
        break;                                                                \
      for (_zv_i = 0; _zv_i < _zv_len; _zv_i++)                               \
        *((P) + _zv_i) = (C);                                                 \
      (P) += _zv_len;                                                         \
      (W) += (Z);                                                             \
    } while (0)

#define ZVSNP_VAL_GET(Q,O,V,A)                                                \
  {                                                                           \
    long l;                                                                   \
    double f;                                                                 \
    V.sign = ZVSNP_SIGN_PLUS;                                                 \
    switch (zv_expand[(O)].type[(Q)])                                         \
      {                                                                       \
      case ZVSNP_TYPE_U_CHAR:                                                 \
        V.u.c = (unsigned char)va_arg (A, int);                               \
	break;                                                                \
      case ZVSNP_TYPE_SHORT:                                                  \
        l = (short)va_arg (A, int);                                           \
        if (l < 0)                                                            \
          {                                                                   \
            V.u.ul = -l;                                                      \
            V.sign = ZVSNP_SIGN_MINUS;                                        \
          }                                                                   \
        else                                                                  \
          V.u.ul = l;                                                         \
	break;                                                                \
      case ZVSNP_TYPE_U_SHORT:                                                \
        V.u.ul = (unsigned short)va_arg (A, int);                             \
	break;                                                                \
      case ZVSNP_TYPE_INT:                                                    \
        l = va_arg (A, int);                                                  \
        if (l < 0)                                                            \
          {                                                                   \
            V.u.ul = -l;                                                      \
            V.sign = ZVSNP_SIGN_MINUS;                                        \
          }                                                                   \
        else                                                                  \
          V.u.ul = l;                                                         \
	break;                                                                \
      case ZVSNP_TYPE_U_INT:                                                  \
        V.u.ul = va_arg (A, unsigned int);                                    \
	break;                                                                \
      case ZVSNP_TYPE_LONG:                                                   \
        l = va_arg (A, long);                                                 \
        if (l < 0)                                                            \
          {                                                                   \
            V.u.ul = -l;                                                      \
            V.sign = ZVSNP_SIGN_MINUS;                                        \
          }                                                                   \
        else                                                                  \
          V.u.ul = l;                                                         \
	break;                                                                \
      case ZVSNP_TYPE_U_LONG:                                                 \
        V.u.ul = va_arg (A, unsigned long);                                   \
	break;                                                                \
      case ZVSNP_TYPE_DOUBLE:                                                 \
      case ZVSNP_TYPE_LONG_DOUBLE:                                            \
        f = va_arg (A, double);                                               \
        if (f < 0.0)                                                          \
	  {                                                                   \
            V.u.df = -f;                                                      \
            V.sign = ZVSNP_SIGN_MINUS;                                        \
	  }                                                                   \
        else                                                                  \
          V.u.df = f;                                                         \
	break;                                                                \
      case ZVSNP_TYPE_POINTER:                                                \
        V.u.p = va_arg (A, void *);                                           \
	break;                                                                \
      default:                                                                \
	break;                                                                \
      }                                                                       \
  }


  
/* Constants. */
static const UINT8 zvsnp_sm[] =
{
  /*                      7  6  5  4  3  2  1  0  */
  0xee, /* 0: BEGIN     { 1, 1, 1, 0, 1, 1, 1, 0 } */
  0x6e, /* 1: FLAG      { 0, 1, 1, 0, 1, 1, 1, 0 } */
  0x6c, /* 2: WIDTH     { 0, 1, 1, 0, 1, 1, 0, 0 } */
  0x10, /* 3: PRECDOT   { 0, 0, 0, 1, 0, 0, 0, 0 } */
  0x70, /* 4: PRECISION { 0, 1, 1, 1, 0, 0, 0, 0 } */
  0x40, /* 5: QUALIFIER { 0, 1, 0, 0, 0, 0, 0, 0 } */
  0x00, /* 6: OPERATOR  { 0, 0, 0, 0, 0, 0, 0, 0 } */
  0x00, /* 7: PERCENT   { 0, 0, 0, 0, 0, 0, 0, 0 } */
};

static const UINT8 zvsnp_class[] =
{
  /*    20 */ 0x02,  /* !  21 */ 0x00,  /* "  22 */ 0x00,  /* #  23 */ 0x02,
  /* $  24 */ 0x00,  /* %  25 */ 0x81,  /* &  26 */ 0x00,  /* '  27 */ 0x00,
  /* (  28 */ 0x00,  /* )  29 */ 0x00,  /* *  2a */ 0x04,  /* +  2b */ 0x02,
  /* ,  2c */ 0x00,  /* -  2d */ 0x02,  /* .  2e */ 0x08,  /* /  2f */ 0x00,
  /* 0  30 */ 0x16,  /* 1  31 */ 0x14,  /* 2  32 */ 0x14,  /* 3  33 */ 0x14,
  /* 4  34 */ 0x14,  /* 5  35 */ 0x14,  /* 6  36 */ 0x14,  /* 7  37 */ 0x14,
  /* 8  38 */ 0x14,  /* 9  39 */ 0x14,  /* :  3a */ 0x00,  /* ;  3b */ 0x00,
  /* <  3c */ 0x00,  /* =  3d */ 0x00,  /* >  3e */ 0x00,  /* ?  3f */ 0x00,
  /* @  40 */ 0x00,  /* A  41 */ 0x40,  /* B  42 */ 0x40,  /* C  43 */ 0x40,
  /* D  44 */ 0x40,  /* E  45 */ 0x40,  /* F  46 */ 0x40,  /* G  47 */ 0x40,
  /* H  48 */ 0x40,  /* I  49 */ 0x40,  /* J  4a */ 0x40,  /* K  4b */ 0x40,
  /* L  4c */ 0x20,  /* M  4d */ 0x40,  /* N  4e */ 0x40,  /* O  4f */ 0x40,
  /* P  50 */ 0x40,  /* Q  51 */ 0x40,  /* R  52 */ 0x40,  /* S  53 */ 0x40,
  /* T  54 */ 0x40,  /* U  55 */ 0x40,  /* V  56 */ 0x40,  /* W  57 */ 0x40,
  /* X  58 */ 0x40,  /* Y  59 */ 0x40,  /* Z  5a */ 0x40,  /* [  5b */ 0x00,
  /* \  5c */ 0x00,  /* ]  5d */ 0x00,  /* ^  5e */ 0x00,  /* _  5f */ 0x00,
  /* `  60 */ 0x00,  /* a  61 */ 0x40,  /* b  62 */ 0x40,  /* c  63 */ 0x40,
  /* d  64 */ 0x40,  /* e  65 */ 0x40,  /* f  66 */ 0x40,  /* g  67 */ 0x40,
  /* h  68 */ 0x20,  /* i  69 */ 0x40,  /* j  6a */ 0x40,  /* k  6b */ 0x40,
  /* l  6c */ 0x20,  /* m  6d */ 0x40,  /* n  6e */ 0x40,  /* o  6f */ 0x40,
  /* p  70 */ 0x40,  /* q  71 */ 0x40,  /* r  72 */ 0x40,  /* s  73 */ 0x40,
  /* t  74 */ 0x40,  /* u  75 */ 0x40,  /* v  76 */ 0x40,  /* w  77 */ 0x40,
  /* x  78 */ 0x40,  /* y  79 */ 0x40,  /* z  7a */ 0x40
};

static const char zvsnp_flags[] =
{
  ZVSNP_FLAG_SPACE,		/* ' ' */
  0,
  0,
  ZVSNP_FLAG_HASH,		/* '#' */
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  ZVSNP_FLAG_PLUS,		/* '+' */
  0,
  ZVSNP_FLAG_MINUS,		/* '-' */
  0,
  0,
  ZVSNP_FLAG_ZERO,		/* '0' */
};

static const UINT8 zvsnp_next[] =
{ 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

static char *zvsnp_digits_l = "0123456789abcdef";
static char *zvsnp_digits_u = "0123456789ABCDEF";


/* Parser. */
static int
zvsnp_ignore (UINT8 *sp, UINT8 *p, struct zvsnp *zv)
{
  return 0;
}

static int
zvsnp_parse_flags (UINT8 *p, UINT8 *lim, struct zvsnp *zv)
{
  zv->flags = 0;
  while (p < lim)
    zv->flags |= ZVSNP_FLAG (*p++);
  return 0;
}

static int
zvsnp_parse_width (UINT8 *p, UINT8 *lim, struct zvsnp *zv)
{
  zv->width = 0;
  if (*p == '*')
    zv->width_star = 1;
  else
    while (p < lim)
      zv->width = zv->width * 10 + ZVSNP_DIGIT (*p++);

  return 0;
}

static int
zvsnp_parse_precision (UINT8 *p, UINT8 *lim, struct zvsnp *zv)
{
  zv->prec = 0;
  while (p < lim)
    zv->prec = zv->prec * 10 + (ZVSNP_DIGIT (*p++));
  return 0;
}

static int
zvsnp_parse_qualifier (UINT8 *p, UINT8 *lim, struct zvsnp *zv)
{
  zv->qual = ZVSNP_QUAL (*p);
  return 0;
}

static int
zvsnp_parse_operator (UINT8 *p, UINT8 *lim, struct zvsnp *zv)
{
  zv->opr = *p;
  return 0;
}

int (*zvsnp_parse[]) (UINT8 *, UINT8 *, struct zvsnp *) =
{
  zvsnp_ignore,
  zvsnp_parse_flags,
  zvsnp_parse_width,
  zvsnp_ignore,
  zvsnp_parse_precision,
  zvsnp_parse_qualifier,
  zvsnp_parse_operator,
  zvsnp_ignore
};


/* Expander. */
#define ZVSNP_NUM_STRLEN(SP,BUF)	((BUF) + ZVSNP_NUM_STRLEN_MAX - (SP))

static int
zv_exp_num_l_space (char *pp, char *lim, char *sign, int slen,
		    unsigned long val, UINT8 base, char *digits,
		    int width, int prec)
{
  char buf[ZVSNP_NUM_STRLEN_MAX + 2];
  char *sp = buf + ZVSNP_NUM_STRLEN_MAX;
  int wlen = 0;

  ZVSNP_MEMCPY (pp, sign, lim, slen, wlen);

  if (!(prec == 0 && val == 0))
    do
      *--sp = digits[CPSS_MOD(val,base)];
    while ((val = CPSS_DIV(val,base)) && sp > buf);

  ZVSNP_MEMSET (pp, '0', lim, prec - ZVSNP_NUM_STRLEN (sp, buf), wlen);
  ZVSNP_MEMCPY (pp, sp, lim, ZVSNP_NUM_STRLEN (sp, buf), wlen);
  ZVSNP_MEMSET (pp, ' ', lim, width - wlen, wlen);

  return wlen;
}

static int
zv_exp_num_r_space (char *pp, char *lim, char *sign, int slen,
		    unsigned long val, UINT8 base, char *digits,
		    int width, int prec)
{
  char buf[ZVSNP_NUM_STRLEN_MAX];
  char *sp = buf + ZVSNP_NUM_STRLEN_MAX;
  int wlen = 0;

  if (!(prec == 0 && val == 0))
    do
      *--sp = digits[CPSS_MOD(val,base)];
    while ((val = CPSS_DIV(val,base)) && sp > buf);
  
  while (ZVSNP_NUM_STRLEN (sp, buf) < prec)
    *--sp = '0';

  ZVSNP_MEMSET (pp, ' ', lim,
		width - (slen + ZVSNP_NUM_STRLEN (sp, buf)), wlen);
  ZVSNP_MEMCPY (pp, sign, lim, slen, wlen);
  ZVSNP_MEMCPY (pp, sp, lim, ZVSNP_NUM_STRLEN (sp, buf), wlen);

  return wlen;
}

static int
zv_exp_num_r_zero (char *pp, char *lim, char *sign, int slen,
		   unsigned long val, UINT8 base, char *digits,
		   int width, int prec)
{
  char buf[ZVSNP_NUM_STRLEN_MAX];
  char *sp = buf + ZVSNP_NUM_STRLEN_MAX;
  int wlen = 0;
  int numlen;
  int zerolen = 0;

  if (!(prec == 0 && val == 0))
    do
      *--sp = digits[CPSS_MOD(val,base)];
    while ((val = CPSS_DIV(val,base)) && sp > buf);
  
  numlen = ZVSNP_NUM_STRLEN (sp, buf);
  if (prec < 0)
    zerolen = width - (slen + numlen);
  else if (numlen < prec)
    {
      zerolen = prec - numlen;
      if (width < numlen + slen + zerolen)
	zerolen = width - (numlen + slen);
    }

  ZVSNP_MEMSET (pp, ' ', lim, width - (slen + zerolen + numlen), wlen);
  ZVSNP_MEMCPY (pp, sign, lim, slen, wlen);
  ZVSNP_MEMSET (pp, '0', lim, zerolen, wlen);
  ZVSNP_MEMCPY (pp, sp, lim, numlen, wlen);

  return wlen;
}

#define ZVSNP_EXP_MASK		(ZVSNP_FLAG_MINUS|ZVSNP_FLAG_ZERO)

int (*zv_exp_num[]) (char *, char *, char *, int,
		     unsigned long, UINT8, char *, int, int) =
{
  zv_exp_num_r_space,		/* -,    -     */
  zv_exp_num_l_space,		/* -,    MINUS */
  zv_exp_num_r_zero,		/* ZERO, -     */
  zv_exp_num_l_space,		/* ZERO, MINUS */
};

#define ZVSNP_EXPAND_IPV4(P,A)                                                \
    do {                                                                      \
      ZVSNP_EXPAND_OCTET ((P), *(A));       *(P)++ = '.';                     \
      ZVSNP_EXPAND_OCTET ((P), *((A) + 1)); *(P)++ = '.';                     \
      ZVSNP_EXPAND_OCTET ((P), *((A) + 2)); *(P)++ = '.';                     \
      ZVSNP_EXPAND_OCTET ((P), *((A) + 3));                                   \
    } while (0)

#if 0
static int
zv_exp_r_prefix4 (char *pp, char *lim,
		  struct zvsnp *zv, struct zvsnp_val *val)
{
  struct prefix_ipv4 *p = val->u.p;
  UINT8 *addr = (UINT8 *)&p->prefix;
  char buf[ZVSNP_PREFIX_IPV4_STRLEN_MAX];
  char *sp = buf;
  int wlen = 0;

  ZVSNP_EXPAND_IPV4 (sp, addr);  *sp++ = '/';
  ZVSNP_EXPAND_OCTET (sp, p->prefixlen);

  ZVSNP_MEMSET (pp, ' ', lim, zv->width - (sp - buf), wlen);
  ZVSNP_MEMCPY (pp, buf, lim, sp - buf, wlen);

  return wlen;
}

static int
zv_exp_l_prefix4 (char *pp, char *lim,
		  struct zvsnp *zv, struct zvsnp_val *val)
{
  struct prefix_ipv4 *p = val->u.p;
  UINT8 *addr = (UINT8 *)&p->prefix;
  char buf[ZVSNP_PREFIX_IPV4_STRLEN_MAX];
  char *sp = buf;
  int wlen = 0;

  ZVSNP_EXPAND_IPV4 (sp, addr);  *sp++ = '/';
  ZVSNP_EXPAND_OCTET (sp, p->prefixlen);

  ZVSNP_MEMCPY (pp, buf, lim, sp - buf, wlen);
  ZVSNP_MEMSET (pp, ' ', lim, zv->width - (sp - buf), wlen);

  return wlen;
}

#endif

#ifdef HAVE_IPV6
#if 0
static int zv_exp_in6_addr (char *, char *, UINT8 *);


static int
zv_exp_r_prefix6 (char *pp, char *lim,
		  struct zvsnp *zv, struct zvsnp_val *val)
{
  int wlen = 0;
  struct prefix_ipv6 *p = val->u.p;
  UINT8 *addr = (UINT8 *)&p->prefix;
  char buf[ZVSNP_PREFIX_IPV6_STRLEN_MAX];
  char *sp;
  int len;

  len = zv_exp_in6_addr (buf, buf + ZVSNP_PREFIX_IPV6_STRLEN_MAX, addr);
  sp = buf + len;
  *sp++ = '/';
  ZVSNP_EXPAND_OCTET (sp, p->prefixlen);

  ZVSNP_MEMSET (pp, ' ', lim, zv->width - (sp - buf), wlen);
  ZVSNP_MEMCPY (pp, buf, lim, sp - buf, wlen);

  return wlen;
}

static int
zv_exp_l_prefix6 (char *pp, char *lim,
		  struct zvsnp *zv, struct zvsnp_val *val)
{
  int wlen = 0;
  struct prefix_ipv6 *p = val->u.p;
  UINT8 *addr = (UINT8 *)&p->prefix;
  char buf[ZVSNP_PREFIX_IPV6_STRLEN_MAX];
  char *sp;
  int len;

  len = zv_exp_in6_addr (buf, buf + ZVSNP_PREFIX_IPV6_STRLEN_MAX, addr);
  sp = buf + len;
  *sp++ = '/';
  ZVSNP_EXPAND_OCTET (sp, p->prefixlen);

  ZVSNP_MEMCPY (pp, buf, lim, sp - buf, wlen);
  ZVSNP_MEMSET (pp, ' ', lim, zv->width - (sp - buf), wlen);

  return wlen;
}
#endif
#endif /* HAVE_IPV6 */


#if 0
static int
zv_exp_l_prefix (char *pp, char *lim,
		 struct zvsnp *zv, struct zvsnp_val *val)
{
  struct prefix *p = val->u.p;

  if (p->family == AF_INET)
    return zv_exp_l_prefix4 (pp, lim, zv, val);
#ifdef HAVE_IPV6
  else if (p->family == AF_INET6)
    return zv_exp_l_prefix6 (pp, lim, zv, val);
#endif /* HAVE_IPV6 */

  return 0;
}

static int
zv_exp_r_prefix (char *pp, char *lim,
		 struct zvsnp *zv, struct zvsnp_val *val)
{
  struct prefix *p = val->u.p;

  if (p->family == AF_INET)
    return zv_exp_r_prefix4 (pp, lim, zv, val);
#ifdef HAVE_IPV6
  else if (p->family == AF_INET6)
    return zv_exp_r_prefix6 (pp, lim, zv, val);
#endif /* HAVE_IPV6 */

  return 0;
}
#endif

#define ZVSNP_WORD_GET(P,O)            ((*((P) + (O)) << 8) | *((P) + (O) + 1))

#define ZVSNP_EXP_HEXA(P,I,W)                                                 \
    do {                                                                      \
      int _flag = 0;                                                          \
      int _val = ((I) >> 12) & 0x0F;                                          \
      if (_val)                                                               \
        {                                                                     \
          *((P)++) = zvsnp_digits_l[_val];                                    \
          _flag = _flag+1;                                                            \
          (W)=(W)+1;                                                              \
        }                                                                     \
      _val = ((I) >> 8) & 0x0F;                                               \
      if (_flag || _val)                                                      \
        {                                                                     \
          *((P)++) = zvsnp_digits_l[_val];                                    \
           _flag = _flag+1;                                                            \
           (W)=(W)+1;                                                              \
        }                                                                     \
      _val = ((I) >> 4) & 0x0F;                                               \
      if (_flag || _val)                                                      \
        {                                                                     \
          *((P)++) = zvsnp_digits_l[_val];                                    \
         _flag = _flag+1;                                                             \
          (W)=(W)+1;                                                             \
        }                                                                     \
      *((P)++) = zvsnp_digits_l[((I)) & 0x0F];                                \
       (W)=(W)+1;                                                                \
    } while (0)

#ifdef HAVE_IPV6
#if 0
static int
zv_exp_in6_addr (char *buf, char *lim, UINT8 *addr)
{
  char *pp = buf;
  u_int16_t word[8];
  int i;
  int wlen = 0;

  word[0] = ZVSNP_WORD_GET (addr, 0);
  word[1] = ZVSNP_WORD_GET (addr, 2);
  word[2] = ZVSNP_WORD_GET (addr, 4);
  word[3] = ZVSNP_WORD_GET (addr, 6);
  word[4] = ZVSNP_WORD_GET (addr, 8);
  word[5] = ZVSNP_WORD_GET (addr, 10);
  word[6] = ZVSNP_WORD_GET (addr, 12);
  word[7] = ZVSNP_WORD_GET (addr, 14);

  if (word[0] == 0)
    {
      i = 1;
      while (word[i] == 0 && i < 8)
	i++;

      ZVSNP_MEMSET (pp, ':', lim, 2, wlen);
      if (i < 8)
	{
	  for (; i < 7; i++)
	    {
	      ZVSNP_EXP_HEXA (pp, word[i], wlen);
	      ZVSNP_MEMSET (pp, ':', lim, 1, wlen);
	    }
	  ZVSNP_EXP_HEXA (pp, word[i], wlen);
	}
    }
  else
    {
      for (i = 0; i < 7; i++)
	{
	  if (word[i] == 0 && word[i + 1] == 0)
	    {
	      ZVSNP_MEMSET (pp, ':', lim, 1, wlen);
	      while (word[i] == 0 && i < 8)
		i++;
	      break;
	    }

	  ZVSNP_EXP_HEXA (pp, word[i], wlen);
	  ZVSNP_MEMSET (pp, ':', lim, 1, wlen);
	}

      if (i < 8)
	{
	  for (; i < 7; i++)
	    {
	      ZVSNP_EXP_HEXA (pp, word[i], wlen);
	      ZVSNP_MEMSET (pp, ':', lim, 1, wlen);
	    }
	  ZVSNP_EXP_HEXA (pp, word[i], wlen);
	}
    }

  return wlen;
}

static int
zv_exp_l_in6_addr (char *pp, char *lim,
		   struct zvsnp *zv, struct zvsnp_val *val)
{
  UINT8 *addr = val->u.p;
  char buf[ZVSNP_IN6_ADDR_STRLEN_MAX];
  int wlen = 0;
  int len;

  len = zv_exp_in6_addr (buf,  buf + ZVSNP_IN6_ADDR_STRLEN_MAX, addr);
  ZVSNP_MEMCPY (pp, buf, lim, len, wlen);
  ZVSNP_MEMSET (pp, ' ', lim, zv->width - len, wlen);

  return wlen;
}

static int
zv_exp_r_in6_addr (char *pp, char *lim,
		   struct zvsnp *zv, struct zvsnp_val *val)
{
  UINT8 *addr = val->u.p;
  char buf[ZVSNP_IN6_ADDR_STRLEN_MAX];
  int wlen = 0;
  int len;

  len = zv_exp_in6_addr (buf,  buf + ZVSNP_IN6_ADDR_STRLEN_MAX, addr);
  ZVSNP_MEMSET (pp, ' ', lim, zv->width - len, wlen);
  ZVSNP_MEMCPY (pp, buf, lim, len, wlen);

  return wlen;
}
#endif
#endif /* HAVE_IPV6 */

static int
zv_exp_l_char (char *pp, char *lim,
	       struct zvsnp *zv, struct zvsnp_val *val)
{
  int padlen = zv->width - 1 < 0 ? 0 : zv->width - 1;
  int wlen = 0;

  ZVSNP_MEMSET (pp, val->u.ul, lim, 1, wlen);
  ZVSNP_MEMSET (pp, ' ', lim, padlen, wlen);

  return wlen;
}

static int
zv_exp_r_char (char *pp, char *lim,
	       struct zvsnp *zv, struct zvsnp_val *val)
{
  int padlen = zv->width - 1 < 0 ? 0 : zv->width - 1;
  int wlen = 0;

  ZVSNP_MEMSET (pp, ' ', lim, padlen, wlen);
  ZVSNP_MEMSET (pp, val->u.ul, lim, 1, wlen);

  return wlen;
}

static int
zv_exp_int (char *pp, char *lim, struct zvsnp *zv, struct zvsnp_val *val)
{
  char *sign = NULL;
  int slen = 0;

  ZVSNP_SIGN_CHAR_GET(zv, val->sign, sign, slen);
  return zv_exp_num[zv->flags & ZVSNP_EXP_MASK] (pp, lim, sign, slen,
						 val->u.ul, ZVSNP_BASE_DECIMAL,
						 zvsnp_digits_l, zv->width,
						 zv->prec);
}

static int
zv_exp_octal (char *pp, char *lim,
	      struct zvsnp *zv, struct zvsnp_val *val)
{
  char *sign = NULL;
  int slen = 0;

  if (zv->flags & ZVSNP_FLAG_HASH)
    {
      sign = "0";
      slen = 1;
    }

  return zv_exp_num[zv->flags & ZVSNP_EXP_MASK] (pp, lim, sign, slen,
						 val->u.ul, ZVSNP_BASE_OCTAL,
						 zvsnp_digits_l, zv->width,
						 zv->prec);
}

#define ZVSNP_FDIGITS_MAX  128

static void
zv_exp_double (char *ibuf, char *fbuf, int *ilen, int *flen,
	       double val, int prec)
{
  double fint;
  double ffrac;
  double fval;
  char *ip, *ep;
  char *fp = fbuf;
  int overflow = 0;

  /* Integer part. */
  ip = ibuf + ZVSNP_FDIGITS_MAX;
  fval = pal_modf (val, &fint);

  if (fint != 0)
    {
      do {
	ffrac = pal_modf (CPSS_DIV(fint ,10), &fint); 
	*--ip = zvsnp_digits_l[(int) (ffrac * 10 + 0.1)];
      } while (fint != 0 && ip > ibuf + 1);
      *(ip - 1) = '0';
    }
  else
    *--ip = '0';

  /* Fraction part. */
  if (prec)
    {
      ffrac = fval;
      while (fp - fbuf <= prec && fp < fbuf + ZVSNP_FDIGITS_MAX)
	{
	  ffrac *= 10;
	  ffrac = pal_modf (ffrac + 0.01, &fint) - 0.01;
	  *fp++ = zvsnp_digits_l[(int) fint];
	}

      ep = fp - 1;
      *ep += 5;

      while (*ep > '9')
	{
	  *ep-- = '0';
	  if (ep < fbuf)
	    {
	      break;
	      overflow = 1;
	    }
	  (*ep)=(*ep)+1;
	}

      if (overflow)
	{
	  ep = ibuf + ZVSNP_FDIGITS_MAX - 1;
	  while (*ep > '9')
	    {
	      *ep-- = '0';
	      if (ep < ibuf)
		break;

	      (*ep)=(*ep)+1;
	    }
	}
    }

  *ilen = ibuf + ZVSNP_FDIGITS_MAX - ip;
  *flen = fp - fbuf - 1;
}

static int
zv_exp_l_double (char *pp, char *lim,
		 struct zvsnp *zv, struct zvsnp_val *val)
{
  char ibuf[ZVSNP_FDIGITS_MAX];
  char fbuf[ZVSNP_FDIGITS_MAX];
  int ilen, flen;
  int wlen = 0;
  char sign = '\0';
    
  if (zv->prec < 0)
    zv->prec = 6;

  if (pal_isnan (val->u.df))
    {
      ZVSNP_MEMCPY (pp, "NaN", lim, 3, wlen);
      return wlen;
    }

  if (val->sign == ZVSNP_SIGN_MINUS)
    sign = '-';
  else if (zv->flags & ZVSNP_FLAG_PLUS)
    sign = '+';
  else if (zv->flags & ZVSNP_FLAG_SPACE)
    sign = ' ';

  zv_exp_double (ibuf, fbuf, &ilen, &flen, val->u.df, zv->prec);

  if (sign)
    ZVSNP_MEMSET (pp, sign, lim, 1, wlen);

  ZVSNP_MEMCPY (pp, ibuf + ZVSNP_FDIGITS_MAX - ilen, lim, ilen, wlen);
  if (zv->prec > 0 && flen)
    {
      ZVSNP_MEMSET (pp, '.', lim, 1, wlen);
      ZVSNP_MEMCPY (pp, fbuf, lim, flen, wlen);
    }

  if (wlen < zv->width)
    ZVSNP_MEMSET (pp, ' ', lim, zv->width - wlen, wlen);

  return wlen;
}

static int
zv_exp_r_double (char *pp, char *lim,
		 struct zvsnp *zv, struct zvsnp_val *val)
{
  char ibuf[ZVSNP_FDIGITS_MAX];
  char fbuf[ZVSNP_FDIGITS_MAX];
  int ilen, flen;
  int wlen = 0;
  int numlen;
  char sign = '\0';
    
  if (zv->prec < 0)
    zv->prec = 6;

  if (pal_isnan (val->u.df))
    {
      ZVSNP_MEMCPY (pp, "NaN", lim, 3, wlen);
      return wlen;
    }

  if (val->sign == ZVSNP_SIGN_MINUS)
    sign = '-';
  else if (zv->flags & ZVSNP_FLAG_PLUS)
    sign = '+';
  else if (zv->flags & ZVSNP_FLAG_SPACE)
    sign = ' ';

  zv_exp_double (ibuf, fbuf, &ilen, &flen, val->u.df, zv->prec);

  numlen = ilen;
  if (flen)
    numlen += 1 + flen;
  if (sign)
    numlen=numlen+1;

  if (!(zv->flags & ZVSNP_FLAG_ZERO))
    ZVSNP_MEMSET (pp, ' ', lim, zv->width - numlen, wlen);

  if (sign)
    ZVSNP_MEMSET (pp, sign, lim, 1, wlen);

  if (zv->flags & ZVSNP_FLAG_ZERO)
    ZVSNP_MEMSET (pp, '0', lim, zv->width - numlen, wlen);

  ZVSNP_MEMCPY (pp, ibuf + ZVSNP_FDIGITS_MAX - ilen, lim, ilen, wlen);
  if (zv->prec > 0 && flen)
    {
      ZVSNP_MEMSET (pp, '.', lim, 1, wlen);
      ZVSNP_MEMCPY (pp, fbuf, lim, flen, wlen);
    }

  return wlen;
}

static int
zv_exp_pointer (char *pp, char *lim,
		struct zvsnp *zv, struct zvsnp_val *val)
{
  char *sign = "0x";
  int slen = 2;
  UINT32 v = (UINT32)val->u.p;

  if (v == 0)
    {
      int wlen = 0;
      ZVSNP_MEMSET (pp, ' ', lim, zv->width - 5, wlen);
      ZVSNP_MEMCPY (pp, "(nil)", lim, 5, wlen);
      return wlen;
    }

  return zv_exp_num[zv->flags & ZVSNP_EXP_MASK] (pp, lim, sign, slen,
						 v, ZVSNP_BASE_HEXA,
						 zvsnp_digits_l, zv->width,
						 zv->prec);
}
#if 0
static int
zv_exp_l_in_addr (char *pp, char *lim,
		  struct zvsnp *zv, struct zvsnp_val *val)
{
  UINT8 *addr = val->u.p;
  char buf[ZVSNP_IN_ADDR_STRLEN_MAX];
  char *sp = buf;
  int wlen = 0;

  ZVSNP_EXPAND_IPV4 (sp, addr);

  ZVSNP_MEMCPY (pp, buf, lim, sp - buf, wlen);
  ZVSNP_MEMSET (pp, ' ', lim, zv->width - (sp - buf), wlen);

  return wlen;
}

static int
zv_exp_r_in_addr (char *pp, char *lim,
		  struct zvsnp *zv, struct zvsnp_val *val)
{
  UINT8 *addr = val->u.p;
  char buf[ZVSNP_IN_ADDR_STRLEN_MAX];
  char *sp = buf;
  int wlen = 0;

  ZVSNP_EXPAND_IPV4 (sp, addr);

  ZVSNP_MEMSET (pp, ' ', lim, zv->width - (sp - buf), wlen);
  ZVSNP_MEMCPY (pp, buf, lim, sp - buf, wlen);

  return wlen;
}
#endif

static int
zv_exp_l_string (char *pp, char *lim,
		 struct zvsnp *zv, struct zvsnp_val *val)
{
  char *str = (char *)val->u.p;
  int len, padlen;
  int wlen = 0;

  ZVSNP_STRLEN (str, len);
  if (zv->prec >= 0)
    if (zv->prec < len)
      len = zv->prec;

  padlen = zv->width - len < 0 ? 0 : zv->width - len;
  ZVSNP_MEMCPY (pp, str, lim, len, wlen);
  ZVSNP_MEMSET (pp, ' ', lim, padlen, wlen);

  return wlen;
}

static int
zv_exp_r_string (char *pp, char *lim,
		 struct zvsnp *zv, struct zvsnp_val *val)
{
  char *str = (char *)val->u.p;
  int len, padlen;
  int wlen = 0;

  ZVSNP_STRLEN (str, len);
  if (zv->prec >= 0)
    if (zv->prec < len)
      len = zv->prec;

  padlen = zv->width - len < 0 ? 0 : zv->width - len;
  ZVSNP_MEMSET (pp, ' ', lim, padlen, wlen);
  ZVSNP_MEMCPY (pp, str, lim, len, wlen);

  return wlen;
}

static int
zv_exp_unsigned (char *pp, char *lim,
		 struct zvsnp *zv, struct zvsnp_val *val)
{
  return zv_exp_num[zv->flags & ZVSNP_EXP_MASK] (pp, lim, NULL, 0, val->u.ul,
						 ZVSNP_BASE_DECIMAL,
						 zvsnp_digits_l, zv->width,
						 zv->prec);
}

static int
zv_exp_hexa_l (char *pp, char *lim,
	       struct zvsnp *zv, struct zvsnp_val *val)
{
  char *sign = NULL;
  int slen = 0;

  if (zv->flags & ZVSNP_FLAG_HASH)
    {
      sign = "0x";
      slen = 2;
    }

  return zv_exp_num[zv->flags & ZVSNP_EXP_MASK] (pp, lim, sign, slen,
						 val->u.ul, ZVSNP_BASE_HEXA,
						 zvsnp_digits_l, zv->width,
						 zv->prec);
}

static int
zv_exp_hexa_u (char *pp, char *lim,
	       struct zvsnp *zv, struct zvsnp_val *val)
{
  char *sign = NULL;
  int slen = 0;

  if (zv->flags & ZVSNP_FLAG_HASH)
    {
      sign = "0X";
      slen = 2;
    }

  return zv_exp_num[zv->flags & ZVSNP_EXP_MASK] (pp, lim, sign, slen,
						 val->u.ul, ZVSNP_BASE_HEXA,
						 zvsnp_digits_u, zv->width,
						 zv->prec);
}

static int
zv_exp_ignore (char *pp, char *lim,
	       struct zvsnp *zv, struct zvsnp_val *val)
{
  return -1;
}

/* Callback function to expand operater. */
struct
{
  int (*func[2]) (char *, char *, struct zvsnp *, struct zvsnp_val *);
  char type[4];
} zv_expand[] =
{
  /* { { R_FUNC, L_FUNC }, { DEFAULT, SHORT, LONG, LONG_DOUBLE } } */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* A */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* B */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* C */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* D */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* E */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* F */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* G */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* H */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* I */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* J */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* K */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* L */
  { { zv_exp_ignore,	  zv_exp_ignore },	{  7,  7,  5,  7 } },	/* M */
  { { zv_exp_ignore,	  zv_exp_ignore }, 	{  7,  7,  5,  7 } },	/* N */
  { { zv_exp_ignore,	  zv_exp_ignore },	{ 10, 10, 10, 10 } },	/* O */
  { { zv_exp_ignore,   zv_exp_ignore },	{ 10, 10, 10, 10 } },	/* P */
#ifdef HAVE_IPV6
  { { zv_exp_ignore,   zv_exp_ignore },   { 10, 10, 10, 10 } },   /* Q */
  { { zv_exp_ignore,  zv_exp_ignore },	{ 10, 10, 10, 10 } },	/* R */
#else /* HAVE_IPV6 */
  { { zv_exp_ignore,      zv_exp_ignore },      { 10, 10, 10, 10 } },   /* Q */
  { { zv_exp_ignore,      zv_exp_ignore },	{ 10, 10, 10, 10 } },	/* R */
#endif /* HAVE_IPV6 */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* S */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* T */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* U */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* V */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* W */
  { { zv_exp_hexa_u,      zv_exp_hexa_u },	{  6,  6,  7,  6 } },	/* X */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* Y */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* Z */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* a */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* b */
  { { zv_exp_r_char,      zv_exp_l_char },	{  2,  2,  2,  2 } },	/* c */
  { { zv_exp_int,         zv_exp_int },		{  2,  2,  3,  2 } },	/* d */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* e */
  { { zv_exp_r_double,    zv_exp_l_double },	{  8,  8,  8,  8 } },	/* f */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* g */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* h */
  { { zv_exp_int,         zv_exp_int },		{  2,  2,  3,  2 } },	/* i */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* j */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* k */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* l */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* m */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* n */
  { { zv_exp_octal,       zv_exp_octal },	{  6,  6,  7,  6 } },	/* o */
  { { zv_exp_pointer,     zv_exp_pointer },	{ 10, 10, 10, 10 } },	/* p */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* q */
  { { zv_exp_ignore,   zv_exp_ignore },	{ 10, 10, 10, 10 } },	/* r */
  { { zv_exp_r_string,    zv_exp_l_string },	{ 10, 10, 10, 10 } },	/* s */

  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* t */
  { { zv_exp_unsigned,    zv_exp_unsigned },	{  6,  6,  7,  6 } },	/* u */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* v */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* w */
  { { zv_exp_hexa_l,      zv_exp_hexa_l },	{  6,  6,  7,  6 } },	/* x */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* y */
  { { zv_exp_ignore,      zv_exp_ignore },	{  0,  0,  0,  0 } },	/* z */
};



INT32 cpss_vsnprintf (char *buf, INT32 size, const char *format, va_list args)
{



  char *lim = buf + size - 1;
  char *putp = buf;
  char *fmtp = (char *)format;
  int wlen = 0;
  int len;
  
  while (*fmtp != '\0')
    {
      char *fsp = fmtp;

      if (*fmtp == '%')
	{
	  struct zvsnp zvsnp;
	  struct zvsnp_val zvsnp_val;
	  int state = ZVSNP_STATE_BEGIN;
	  char *sp;
	  UINT8 class;
	  int error = 0;

	  /* Init zvsnp. */
	  cpss_mem_memset (&zvsnp, 0, sizeof (struct zvsnp));
	  zvsnp.prec = -1;
	  zvsnp.sp = fmtp;

	  /* First, get transform strings. */
	  sp = fmtp;
	  while (*++fmtp != '\0'
		 && state != ZVSNP_STATE_OPERATOR
		 && state != ZVSNP_STATE_PERCENT)
	    {
	      int next = ZVSNP_STATE_FLAG;

	      if (!ZVSNP_CLASS_RANGE (*fmtp))
		{
		  error = ZVSNP_ERROR_PARSE;
		  break;
		}

	      class = zvsnp_sm[state] & ZVSNP_CLASS (*fmtp);
	      if (class == 0)
		{
		  error = ZVSNP_ERROR_PARSE;
		  break;
		}

	      while (next < 8 && (class & zvsnp_next[next]) == 0)
		next=next+1;

	      if (state != next)
		{
		  zvsnp_parse[state] (sp, fmtp, &zvsnp);
		  sp = fmtp;
		  state = next;
		}
	    }

	  if (error == ZVSNP_ERROR_PARSE)
	    {
	      fmtp=fmtp+1;
	      ZVSNP_MEMCPY (putp, fsp, lim, fmtp - fsp, wlen);
	    }
	  else if (state == ZVSNP_STATE_OPERATOR)
	    {
	      zvsnp.ep = sp + 1;
	      zvsnp.opr = (*sp - 'A');

	      if (zvsnp.width_star)
		zvsnp.width = va_arg (args, int);

	      /* Second, get value with appropriate type. */
	      ZVSNP_VAL_GET (zvsnp.qual, zvsnp.opr, zvsnp_val, args);

	      /* If WIDTH is not specified, align LEFT. */
	      if (zvsnp.width == 0)
		zvsnp.flags |= ZVSNP_FLAG_MINUS;

	      /* Finally, expand string. */
	      len = ZVSNP_EXPAND (&zvsnp, putp, lim, &zvsnp_val);
	      if (len < 0)
		ZVSNP_MEMCPY (putp, fsp, lim, fmtp - fsp, wlen);
	      else
		{
		  putp += len;
		  wlen += len;
		}
	    }
	  else if (state == ZVSNP_STATE_PERCENT)
	    {
	      ZVSNP_MEMSET (putp, '%', lim, 1, wlen);
	    }

	  fsp = fmtp;
	}

      if (*fmtp == '%')
	continue;

      /* Otherwise, put char as is. */
      while (*fmtp != '\0' && *fmtp != '%')
	fmtp++;

      ZVSNP_MEMCPY (putp, fsp, lim, fmtp - fsp, wlen);

      if (*fmtp != '%')
	break;
    }

  if (lim < putp)
    *lim = '\0';
  else
    *putp = '\0';

  return wlen;
}

void cpss_print_help(UINT32 ulCmd)
{
	switch(ulCmd)
	{
		case 0:
			printf("**************************************************\n");
			printf("this function shows CPS print help\n");
			printf("you can type ONE parament for more information\n");
			printf("1 - show all CPS help functions\n");
			printf("n - details of each function\n");
			printf("**************************************************\n");
			break;
		case 1:
			printf("**************************************************\n");
			printf("2  - smss_showbrd\n");
			printf("3  - smss_showmsginfo\n");
			printf("4  - smss_showproc\n");
			printf("5  - smss_showself\n");
			printf("6  - cpss_com_interface_send_show\n");
			printf("7  - cpss_com_data_show\n");
			printf("8  - cpss_com_heartbeat_show\n");
			printf("9  - cpss_com_init_show\n");
			printf("10 - cpss_com_link_show\n");
			printf("11 - cpss_com_route_show\n");
			printf("12 - cpss_mem_show\n");
			printf("13 - cpss_timer_show\n");
			printf("14 - cpss_timer_show_all_proc\n");
			printf("15 - cpss_timer_show_ctrl\n");
			printf("16 - cpss_vk_ipc_msg_show\n");
			printf("17 - cpss_vk_ipc_send_error_show\n");
			printf("18 - cpss_vk_proc_list_all\n");
			printf("19 - cpss_vk_proc_list\n");
			printf("20 - cpss_vk_sched_show\n");
			printf("21 - cpss_vos_msg_q_show\n");
			printf("22 - cpss_vos_task_show\n");
			printf("23 - cpss_print_level_set\n");
			printf("**************************************************\n");
			break;
		case 2:
			printf("**************************************************\n");
			printf("2  - smss_showbrd\n");
			printf("show all broads state.\n");
			printf("**************************************************\n");
			break;
		case 3:
			printf("**************************************************\n");
			printf("3  - smss_showmsginfo\n");
			printf("show smss proc send & receive messages.\n");
			printf("**************************************************\n");
			break;
		case 4:
			printf("**************************************************\n");
			printf("4  - smss_showproc\n");
			printf("show self board initialized procs.\n");
			printf("**************************************************\n");
			break;
		case 5:
			printf("**************************************************\n");
			printf("5  - smss_showself\n");
			printf("show self board state.\n");
			printf("**************************************************\n");
			break;
		case 6:
			printf("**************************************************\n");
			printf("6  - cpss_com_interface_send_show\n");
			printf("show cpss com send & receive state.\n");
			printf("**************************************************\n");
			break;
		case 7:
			printf("**************************************************\n");
			printf("7  - cpss_com_data_show\n");
			printf("show cpss com global variables.\n");
			printf("**************************************************\n");
			break;
		case 8:
			printf("**************************************************\n");
			printf("8  - cpss_com_heartbeat_show\n");
			printf("show heartbeat links.\n");
			printf("**************************************************\n");
			break;
		case 9:
			printf("**************************************************\n");
			printf("9  - cpss_com_init_show\n");
			printf("show cpss com int procedure stat.\n");
			printf("**************************************************\n");
			break;
		case 10:
			printf("**************************************************\n");
			printf("10 - cpss_com_link_show\n");
			printf("show cpss com all existent links between boards.\n");
			printf("**************************************************\n");
			break;
		case 11:
			printf("**************************************************\n");
			printf("11 - cpss_com_route_show\n");
			printf("show cpss com all existent routes between boards.\n");
			printf("**************************************************\n");
			break;
		case 12:
			printf("**************************************************\n");
			printf("12 - cpss_mem_show\n");
			printf("show cpss mem help.\npara1:0~4(0-show the help for cpss_mem).\n");
			printf("**************************************************\n");
			break;
		case 13:
			printf("**************************************************\n");
			printf("13 - cpss_timer_show\n");
			printf("show timers summary.\n");
			printf("**************************************************\n");
			break;
		case 14:
			printf("**************************************************\n");
			printf("14 - cpss_timer_show_all_proc\n");
			printf("show every procs used timers.\n");
			printf("**************************************************\n");
			break;
		case 15:
			printf("**************************************************\n");
			printf("15 - cpss_timer_show_ctrl\n");
			printf("show timer manage info.\n");
			printf("**************************************************\n");
			break;
		case 16:
			printf("**************************************************\n");
			printf("16 - cpss_vk_ipc_msg_show\n");
			printf("show cpss vk ipc msg info.\n");
			printf("**************************************************\n");
			break;
		case 17:
			printf("**************************************************\n");
			printf("17 - cpss_vk_ipc_send_error_show\n");
			printf("show cpss vk ipc msg send erro detail info.\n");
			printf("**************************************************\n");
			break;
		case 18:
			printf("**************************************************\n");
			printf("18 - cpss_vk_proc_list_all\n");
			printf("show all procs info.\n");
			printf("**************************************************\n");
			break;
		case 19:
			printf("**************************************************\n");
			printf("19 - cpss_vk_proc_list\n");
			printf("show customed proc info.\npara1:proc descriptor.\npara2:0~1(0-summary 1-detail).\n");
			printf("**************************************************\n");
			break;
		case 20:
			printf("**************************************************\n");
			printf("20 - cpss_vk_sched_show\n");
			printf("show customed schedl info.\npara1:sched descriptor.\npara2:0~1(0-summary 1-detail).\n");
			printf("**************************************************\n");
			break;
		case 21:
			printf("**************************************************\n");
			printf("21 - cpss_vos_msg_q_show\n");
			printf("show customed message queue info.\npara1:msgq descriptor.\npara2:0~1(0-summary 1-detail)\n");
			printf("**************************************************\n");
			break;
		case 22:
			printf("**************************************************\n");
			printf("22 - cpss_vos_task_show\n");
			printf("show customed task info.\npara1:task descriptor.\npara2:0~1(0-summary 1-detail).\n");
			printf("**************************************************\n");
			break;
		case 23:
			printf("**************************************************\n");
			printf("23 - cpss_print_level_set\n");
			printf("set cpss_print level.\npara1:SubsysNo.\npara2:ModuleNo.\npara3:0~6(print level 0-OFF 1-FATAL 2-FAIL 3-ERROR 4-WARN 5-INFO 6-DETAIL).\n");
			printf("**************************************************\n");
			break;
	}
}



/******************************* 源文件结束 ***********************************/
