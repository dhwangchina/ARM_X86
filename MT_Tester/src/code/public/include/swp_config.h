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
#ifndef SWP_CONFIG_H
#define SWP_CONFIG_H

#ifdef  __cplusplus
extern "C" {
#endif

/********************************* 头文件包含 ********************************/
#include "swp_common.h"
/******************************** 宏和常量定义 *******************************/

/******************************** 宏和常量定义 *******************************/

/*******************************************************************
                  统一平台编译开关宏定义开始
*******************************************************************/
/* 针对RNC网元的编译开关 */
#if (SWP_NE_TYPE == SWP_NETYPE_RNC)||((SWP_NE_TYPE == SWP_NETYPE_TAN))/**zengjian add for TAN 20070301*/
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
        #define SWP_FNBLK_BRDTYPE_MAIN_CTRL   /* 主控 */
#endif
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
        #define SWP_FNBLK_BRDTYPE_SRVC_CTRL   /* 业务主控 */
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_RTPA)
        #define SWP_FNBLK_BRDTYPE_SRV   /* 业务 */
#endif
/** tzl 20070323 ***/
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_TAN_UPPB_N)
        #define SWP_FNBLK_BRDTYPE_SRV   /* 业务 */
    
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_PTPA)
        #define SWP_FNBLK_BRDTYPE_IPPKT_PROC   /* 适用于各网元的GTPU处理板 */
#endif
#if ((SWP_FUNBRD_TYPE == SWP_FUNBRD_GTSA) ||\
        (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCSA)  ||\
        (SWP_FUNBRD_TYPE == SWP_FUNBRD_TCSA))
        #define SWP_FNBLK_IP_SWITCH   /* IP交换 */
#endif
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_CGDA)
        #define SWP_FNBLK_BRDTYPE_CLOCK   /* 适用于各网元的时钟处理板 */
#endif
#if (SWP_NE_TYPE == SWP_NETYPE_TAN)
#if ((SWP_FUNBRD_TYPE == SWP_FUNBRD_NASA) ||\
        (SWP_FUNBRD_TYPE == SWP_FUNBRD_CASA))/*  ||\
        (SWP_FUNBRD_TYPE == SWP_FUNBRD_IAEA))*/ /* 窄带.20070716 chengguoxiang change for TAN*/
        #define SWP_FNBLK_ATM_LGIF   /* 逻辑ATM接口 */
#endif
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_IAEA)
        #define SWP_FNBLK_E1_IF   /* 逻辑ATM接口 ,for TAN*/
#endif
#else
#if ((SWP_FUNBRD_TYPE == SWP_FUNBRD_NASA) ||\
        (SWP_FUNBRD_TYPE == SWP_FUNBRD_CASA)  ||\
        (SWP_FUNBRD_TYPE == SWP_FUNBRD_IAEA)||\
        (SWP_FUNBRD_TYPE == SWP_FUNBRD_IAPA)  ||\
        (SWP_FUNBRD_TYPE == SWP_FUNBRD_CAPA))
        #define SWP_FNBLK_ATM_LGIF   /* 逻辑ATM接口 */
#endif
#endif

#if ((SWP_FUNBRD_TYPE == SWP_FUNBRD_IPEA) ||\
        (SWP_FUNBRD_TYPE == SWP_FUNBRD_IPSA)  ||\
        (SWP_FUNBRD_TYPE == SWP_FUNBRD_IPUA)||\
        (SWP_FUNBRD_TYPE == SWP_FUNBRD_IPCA))
        #define SWP_FNBLK_IP_LGIF   /* 逻辑IP接口 */
#endif

#if ((SWP_PHYBRD_TYPE == SWP_PHYBRD_MEIA) ||\
        (SWP_PHYBRD_TYPE == SWP_PHYBRD_MNPA)  ||\
        (SWP_PHYBRD_TYPE == SWP_PHYBRD_MDPA))
        #define SWP_FNBLK_STRUCT_MS /* 主从结构 */
#endif
#if ((SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA) ||\
        (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA) ||\
        (SWP_FUNBRD_TYPE == SWP_FUNBRD_CGDA))
        #define SWP_FNBLK_STRUCT_AS   /* 主备结构 */
#endif

#ifdef SWP_FNBLK_IP_SWITCH
#if(SWP_FUNBRD_TYPE == SWP_FUNBRD_TCSA) 
        #define SWP_FNBLK_STRUCT_AS   /* 主备结构 */ 
#endif
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_ONCA)
        #define SWP_FNBLK_BRDTYPE_ONCA   /* 操作维护路由，目前为RNC特有 */
#endif

#if(SWP_FUNBRD_ABOX == SWP_FUNBRD_TYPE)
#define SWP_FNBLK_BRDTYPE_ABOX
#endif
    
/* RNC业务部分部署编译开关：交换板、时钟板、操作维护路由板上不需要部署 */
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
#ifndef SWP_FNBLK_BRDTYPE_CLOCK
#ifndef SWP_FNBLK_IP_SWITCH
#ifndef SWP_FNBLK_BRDTYPE_ONCA
#ifndef SWP_FNBLK_BRDTYPE_ABOX
        #define SWP_USER_RNC_TRAFIC
#endif
#endif
#endif
#endif
#endif
#endif /*#if (SWP_NE_TYPE == SWP_NETYPE_RNC)*/

#define SWP_OPTION_WITH_STANDBY    FALSE
#define SWP_OPTION_WITH_SLAVE_CPU  FALSE
#define SWP_OPTION_WITH_DSP        FALSE
#define SWP_OPTION_SW_ASCOM        FALSE
#define SWP_OPTION_HW_ASCOM        FALSE
#if (SWP_NE_TYPE == SWP_NETYPE_TAN)
#define SWP_OPTION_TDM_SWITCH      TRUE  
#else
#define SWP_OPTION_TDM_SWITCH      FALSE
#endif

#define SWP_HA_TIME_TYPE_3S   0
#define SWP_HA_TIME_TYPE_2M   1
#define SWP_HA_TIME_TYPE_30M  2

#define SWP_ACT_TIME_TYPE_3S  0
#define SWP_ACT_TIME_TYPE_20S 1
#define SWP_ACT_TIME_TYPE_2M  2
#define SWP_ACT_TIME_TYPE_30M 3
#define SWP_ACT_TIME_TYPE_ASY 4


/*******************************************************************
                  统一平台编译开关宏定义结束
*******************************************************************/

/*******************************************************************
                  CPU容量配置相关宏定义开始
*******************************************************************/
/* 本CPU支持的最大任务配置 */
#define SWP_MAX_TASK_NUM              (64)
/* 本CPU支持的最大信号量配置 */
#define SWP_MAX_SEM_NUM               (256)
/* 本CPU支持的最大调度任务配置 */
#define SWP_MAX_SCHED_NUM             (16)
/* 本CPU支持的最大纤程类型配置 */
#define SWP_MAX_PROC_CLASS_NUM        (512)
/* 本CPU支持的最大纤程实例配置，是所有纤程类型的实例总和 */
#define SWP_MAX_PROC_INST_NUM         (512)

/* 每个调度任务的堆栈大小配置 */
#define SWP_SCHED_M_TASK_STACK_SIZE   (0x100000)  /* 推荐值为1M，按各纤程分别统计得到值再乘以120% */

/* 内存池规格最大值 */
#define SWP_MAX_POOL_TYPE_NUM         (128)   
/* 本CPU支持的最大定时器数量 */

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
#define SWP_MAX_TIMER_NUM             (38241)
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
#define SWP_MAX_TIMER_NUM             (38241)
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_PTPA)
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
#define SWP_MAX_TIMER_NUM             (1000)
#elif (SWP_CPUROLE_TYPE == SWP_CPUROLE_SLAVE)
#define SWP_MAX_TIMER_NUM             (1286)
#endif
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_RTPA)
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
#define SWP_MAX_TIMER_NUM             (194)
#elif (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
#define SWP_MAX_TIMER_NUM             (2723)
#endif
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_NASA)
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)

#define SWP_MAX_TIMER_NUM             (2000)

#elif (SWP_CPUROLE_TYPE == SWP_CPUROLE_SLAVE)
#define SWP_MAX_TIMER_NUM             (1286)
#endif
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_CASA)
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
#define SWP_MAX_TIMER_NUM             (2228)
#elif (SWP_CPUROLE_TYPE == SWP_CPUROLE_SLAVE)
#define SWP_MAX_TIMER_NUM             (1286)
#endif
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_IAEA)
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
#define SWP_MAX_TIMER_NUM             (2228)
#elif (SWP_CPUROLE_TYPE == SWP_CPUROLE_SLAVE)
#define SWP_MAX_TIMER_NUM             (1286)
#endif
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_ONCA)
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
#define SWP_MAX_TIMER_NUM             (192)
#elif (SWP_CPUROLE_TYPE == SWP_CPUROLE_SLAVE)
#define SWP_MAX_TIMER_NUM             (1286)
#endif
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_TCSA)
#define SWP_MAX_TIMER_NUM             (192)
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCSA)
#define SWP_MAX_TIMER_NUM             (192)
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GTSA)
#define SWP_MAX_TIMER_NUM             (192)
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_CGDA)
#define SWP_MAX_TIMER_NUM             (192)
#endif

#ifndef SWP_MAX_TIMER_NUM
#define SWP_MAX_TIMER_NUM             (1000)
#endif

#if (SWP_OS_TYPE == SWP_OS_WINDOWS)
#undef CPSS_CPS__TNBS_TCP_SIM
#define CPSS_CPS__TNBS_TCP_SIM
#endif

#if (SWP_OS_TYPE == SWP_OS_LINUX)
#undef CPSS_CPS__TNBS_TCP_SIM
#define CPSS_CPS__TNBS_TCP_SIM
#endif

#ifdef SWP_FNBLK_BRDTYPE_ABOX
#undef  CPSS_CPS__TNBS_TCP_SIM
#define CPSS_CPS__TNBS_TCP_SIM
#endif


#if (SWP_OS_TYPE == SWP_OS_WINDOWS)
#undef CPSS_TNBS_TCP_SIM
#define CPSS_TNBS_TCP_SIM
#endif


#if (SWP_OS_TYPE == SWP_OS_LINUX)
#undef CPSS_TNBS_TCP_SIM
#define CPSS_TNBS_TCP_SIM
#endif


#ifdef SWP_FNBLK_BRDTYPE_ABOX
#undef  CPSS_TNBS_TCP_SIM
#define CPSS_TNBS_TCP_SIM
#endif


#ifndef SWP_OPTION_NO_RDBS
#define SWP_OPTION_NO_RDBS FALSE
#endif

/* RDBS的公共宏定义 */
#ifndef __func__
#define __func__ "swp_unknown_func"
#endif

/*******************************************************************
                  CPU容量配置相关宏定义结束
*******************************************************************/

/******************************** 类型定义 ***********************************/

/******************************** 全局变量声明 *******************************/
 

/******************************** 外部函数原形声明 ***************************/

/******************************** 头文件保护结尾 *****************************/
#ifdef  __cplusplus
}
#endif

#endif /* SWP_CONFIG_H */
/******************************** 头文件结束 *********************************/

