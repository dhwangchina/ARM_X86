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
#ifndef CPSS_KW_H
#define CPSS_KW_H

/********************************* 头文件包含 ********************************/
#include "cpss_type.h"
/******************************** 宏和常量定义 *******************************/
/* cpss_reset,复位原因 */
/* 复位原因, 用于cpss_reset */
#define CPSS_RESET_CPS__OAMS_FORCE           ((UINT32)(1))   /* 网管下达强制复位 */
#define CPSS_RESET_SMSS_SWITCH_OVER          ((UINT32)(2))   /* 主备倒换复位 */
#define CPSS_RESET_SMSS_FAULT_DETECT         ((UINT32)(3))   /* SMSS检测到致命故障 */
#define CPSS_RESET_SMSS_INIT_FAILURE         ((UINT32)(4))   /* SMSS初始化失败 */
#define CPSS_RESET_CPS__RDBS_SYN_FAILURE     ((UINT32)(5))   /* RDBS数据同步失败 */
#define CPSS_KW_CPU_USAGE_INVALID            ((UINT32)(0xFFFFFFFF)) 
#define CPSS_SBBR_EXC_BUF_LENGTH             (24*1024)       /* 存储异常信息的长度*/

/******************************** 类型定义 ***********************************/
typedef struct
{
    UINT32  ulDspNo;    /* 5~20 */
    UINT32  ulDspUsage;
} CPSS_KW_DSP_USAGE_T;

/******************************** 全局变量声明 *******************************/
/*extern */
/******************************** 外部函数原形声明 ***************************/

extern INT32 cpss_reset
( 
    UINT32 ulCause
);

extern INT32 cpss_cpu_usage_get(UINT32 *pulUsage);
extern INT32 cpss_kw_dsp_cpuusage_get(
    UINT32            ulUsageListMaxLen,
    CPSS_KW_DSP_USAGE_T  *pstUsageList,
    UINT32            *pulUsageListLen
);

#ifdef CPSS_VOS_VXWORKS
extern VOID cpss_cpu_load_task(VOID);         
#endif
/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_KW_H */
/******************************** 头文件结束 *********************************/

