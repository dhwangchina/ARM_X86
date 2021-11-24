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
#ifndef SWP_TROUBLE_SHOOT_H
#define SWP_TROUBLE_SHOOT_H

#ifdef __cplusplus
extern "C" {
#endif



/*******************************************************************************
*                               宏和常量定义                                   *
*******************************************************************************/



/*******************************************************************************
*                               类型定义                                       *
*******************************************************************************/
/*
 * SWP 问题定位函数配置信息
 */
typedef struct 
{
    STRING szCmdName;           /* 函数名称 */
    STRING szCmdHelp;           /* 函数帮助信息 */
    STRING szArgFmt;            /* 参数格式字符串 */
    CPS__OAMS_SHCMD_PF  pfCmdFunc;   /* 函数地址 */
}SWP_TROUBLE_SHOOT_ITEM_T;

typedef struct
{
    UINT16 usPortId;
    UINT16 usResv;
    UINT32 ulVPI;
    UINT32 ulVCI;
}SWP_TROUBLE_SHOOT_ATMLIKN_INFO_T;



/*******************************************************************************
*                               函数声明                                       *
*******************************************************************************/
/*  软件平台问题定位初始化，完成shell函数的注册 */
VOID swp_trouble_shoot_init(VOID);

/* IUB 信令链路问题分析函数 */
INT32 swp_show_iublinkinfo(UINT32 ulPara0, UINT32 ulPara1, 
                           UINT32 ulPara2, UINT32 ulPara3);
/* IU AAL5链路问题分析函数 */
INT32 swp_show_iu_aal5_info(UINT32 ulPara0, UINT32 ulPara1, 
                           UINT32 ulPara2, UINT32 ulPara3);
/* AAL2 链路问题分析函数 */
INT32 swp_show_aal2_info(UINT32 ulPara0, UINT32 ulPara1, 
                           UINT32 ulPara2, UINT32 ulPara3);
/* IPOA问题分析函数 */
INT32 swp_show_ipoa_info(UINT32 ulPara0, UINT32 ulPara1, 
                           UINT32 ulPara2, UINT32 ulPara3);
/* IP IUB 链路问题分析函数 */
INT32 wp_show_ip_iub_info(UINT32 ulPara0, UINT32 ulPara1, 
                           UINT32 ulPara2, UINT32 ulPara3);
/* IP IU 链路问题分析函数 */
INT32 swp_show_ip_iu_info(UINT32 ulPara0, UINT32 ulPara1, 
                           UINT32 ulPara2, UINT32 ulPara3);
/* IP IUB 链路问题分析函数 */
INT32 swp_show_ip_iub_info(UINT32 ulPara0, UINT32 ulPara1, 
                           UINT32 ulPara2, UINT32 ulPara3);
/* IP 业务面问题分析函数 */
INT32 swp_show_ip_service_info(UINT32 ulPara0, UINT32 ulPara1, 
                           UINT32 ulPara2, UINT32 ulPara3);
/* IP 诊断信息分析函数 */
INT32 swp_show_ip_diag_info(UINT32 ulPara0, UINT32 ulPara1, 
                           UINT32 ulPara2, UINT32 ulPara3);
/* DSP 业务面问题分析函数 */
INT32 swp_show_dsp_service_stat_info(UINT32 ulPara0, UINT32 ulPara1, 
                           UINT32 ulPara2, UINT32 ulPara3);
/* PTPA业务面问题分析函数 */
INT32 swp_show_gtpu_service_stat_info(UINT32 ulPara0, UINT32 ulPara1, 
                           UINT32 ulPara2, UINT32 ulPara3);
/* ATM统计分析函数 */
INT32 swp_show_atm_stat_info(UINT32 ulPara0, UINT32 ulPara1, 
                           UINT32 ulPara2, UINT32 ulPara3);
/* IP统计分析函数 */
INT32 swp_show_ip_stat_info(UINT32 ulPara0, UINT32 ulPara1, 
                           UINT32 ulPara2, UINT32 ulPara3);
/* IP交换分析函数 */
INT32 swp_show_ip_switch_info(UINT32 ulPara0, UINT32 ulPara1, 
                           UINT32 ulPara2, UINT32 ulPara3);
                           
                           
                           
/*******************************************************************************
*                               头文件保护结尾                                 *
*******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* SWP_TROUBLE_SHOOT_H */
/* 头文件结束, 末尾请保留一个空行 */

