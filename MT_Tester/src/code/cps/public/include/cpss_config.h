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
#ifndef CPSS_CONFIG_H
#define CPSS_CONFIG_H

#ifdef  __cplusplus
extern "C" {
#endif

/********************************* 头文件包含 ********************************/
#include "swp_public.h"
#include "cpss_type.h"

/******************************** 宏和常量定义 *******************************/
/* CPU类型定义 */
#define CPSS_CPU_PPC (0x01)
#define CPSS_CPU_ARM (0x02)
#define CPSS_CPU_I86 (0x03)
#define CPSS_CPU_BIG	04
#define CPSS_CPU_LITTLE	05

#if ((SWP_CPU_TYPE == SWP_CPU_MPC8247) \
    || (SWP_CPU_TYPE == SWP_CPU_MPC8560) \
    || (SWP_CPU_TYPE == SWP_CPU_MPC7447A) \
    || (SWP_CPU_TYPE == SWP_CPU_MPC7448) \
    || (SWP_CPU_TYPE == SWP_CPU_MPC8260) \
    || (SWP_CPU_TYPE == SWP_CPU_MPC7410) \
    || (SWP_CPU_TYPE == SWP_CPU_MPC8266))
    #define CPSS_CPU_TYPE CPSS_CPU_PPC
    #define CPSS_ESF ESFPPC
#elif (SWP_CPU_TYPE == SWP_CPU_XSCALE)
    #define CPSS_CPU_TYPE CPSS_CPU_ARM
    #define CPSS_ESF ESF
#elif (SWP_CPU_TYPE == SWP_CPU_PENTIUMM)
#define CPSS_CPU_TYPE CPSS_CPU_I86
#elif (SWP_CPU_TYPE == SWP_CPU_BIG)
#define CPSS_CPU_TYPE CPSS_CPU_BIG
#elif (SWP_CPU_TYPE == SWP_CPU_LITTLE)
#define CPSS_CPU_TYPE CPSS_CPU_LITTLE
#define CPSS_ESF ESF0 /* 选ESF0/1/2需要进一步研究手册，目前先定义0 */

#endif

/*******************************************************************************
*                               任务优先级映射宏定义
*******************************************************************************/
/*  说明 : 在不同的OS下，任务优先级定义不同。此处根据统一软件平台公共约定文档
 *         对不同OS下的任务优先级进行映射。
*/
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
#define CPSS_TASKPRI_TIME_CRITICAL   100
#define CPSS_TASKPRI_HIGHEST         110
#define CPSS_TASKPRI_ABOVE_NORMAL    120
#define CPSS_TASKPRI_NORMAL          130
#define CPSS_TASKPRI_BELOW_NORMAL    140
#define CPSS_TASKPRI_LOWEST          150
#define CPSS_TASKPRI_IDLE            255
#define CPSS_TASKPRI_SHMCSEND        40
#define CPSS_TASKPRI_SHMCRECV        41

#endif
#if (SWP_OS_TYPE == SWP_OS_WINDOWS)
#define CPSS_TASKPRI_TIME_CRITICAL   0
#define CPSS_TASKPRI_HIGHEST         1
#define CPSS_TASKPRI_ABOVE_NORMAL    2
#define CPSS_TASKPRI_NORMAL          3
#define CPSS_TASKPRI_BELOW_NORMAL    4
#define CPSS_TASKPRI_LOWEST          5
#define CPSS_TASKPRI_IDLE            6
#endif

#if (SWP_OS_TYPE == SWP_OS_LINUX)
#define CPSS_TASKPRI_TIME_CRITICAL   35
#define CPSS_TASKPRI_HIGHEST         30
#define CPSS_TASKPRI_ABOVE_NORMAL    25
#define CPSS_TASKPRI_NORMAL          20
#define CPSS_TASKPRI_BELOW_NORMAL    15
#define CPSS_TASKPRI_LOWEST          10
#define CPSS_TASKPRI_IDLE            5
#endif

/*******************************************************************************
*                               驱动模块部署编译开关
*******************************************************************************/
/*  说明 : CPSS代码中的部署编译开关有如下几个
*       CPSS_HOST_CPU
*           允许部署UDP驱动模块,
*           此编译开关在所有主处理器上为开
*       CPSS_SLAVE_CPU
*           允许部署PCI驱动模块,
*           此编译开关在所有从处理器上为开
*       CPSS_DSP_CPU
*           允许部署PCI驱动模块,
*           此编译开关在DSP处理器上为开.
*       CPSS_MULTI_CPU
*           允许部署UDP和PCI驱动模块,
*           此编译开关在带有主从结构板卡的主处理器上为开.
*       CPSS_UDP_INCLUDE
*           部署UDP驱动模块
*       CPSS_PCI_INCLUDE
*           部署PCI驱动模块
*       CPSS_TCP_INCLUDE
*           部署TCP驱动模块
*
*******************************************************************************/
/* 将平台的编译宏定义重新定义为CPSS自己的编译宏 */
/* CPU类型编译宏定义 */
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
#define CPSS_HOST_CPU
#elif (SWP_CPUROLE_TYPE == SWP_CPUROLE_SLAVE)
#define CPSS_SLAVE_CPU
#elif (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
#define CPSS_DSP_CPU
#endif


/* 带DSP的主CPU的宏定义 */
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
#if (SWP_PHYBRD_TYPE == SWP_PHYBRD_MDPA )
#define CPSS_HOST_CPU_WITH_DSP
#endif
#endif

/* 有主从结构板卡的宏定义，只有主CPU上定义CPSS自己的宏定义 */
#ifdef SWP_FNBLK_STRUCT_MS
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
#define CPSS_MULTI_CPU
#endif
#endif

/* 有主从结构的主CPU上部署UDP、PCI驱动模块 */
#ifdef CPSS_MULTI_CPU
#define CPSS_UDP_INCLUDE
#define CPSS_PCI_INCLUDE
#endif

/* 主处理器上部署UDP模块 */
#ifdef CPSS_HOST_CPU
#define CPSS_UDP_INCLUDE
#endif

/* 从处理器上部署PCI模块 */
#ifdef CPSS_SLAVE_CPU
#define CPSS_PCI_INCLUDE
#endif

/* DSP上部署PCI模块 */
#ifdef CPSS_DSP_CPU
#define CPSS_PCI_INCLUDE
#endif

/* 主控功能块重定义CPSS主控板宏定义 */
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL
#define CPSS_FUNBRD_MC
#endif

/* 主控板上部署磁盘文件系统和TCP模块 */
#ifdef CPSS_FUNBRD_MC
#define CPSS_DISK_FILESYS
#define CPSS_TCP_INCLUDE
#endif

/* WINDOWS环境下使用UDP模拟PCI通信时需要定义的宏定义 */
#ifdef CPSS_VOS_WINDOWS
#ifdef CPSS_PCI_SIM_BY_UDP
#undef CPSS_SLAVE_CPU
#undef CPSS_DSP_CPU
#undef CPSS_PCI_INCLUDE
#define CPSS_HOST_CPU
#define CPSS_UDP_INCLUDE
#endif
#endif

#ifdef CPSS_VOS_LINUX
#ifdef CPSS_PCI_SIM_BY_UDP
#undef CPSS_SLAVE_CPU
#undef CPSS_DSP_CPU
#undef CPSS_PCI_INCLUDE
#define CPSS_HOST_CPU
#define CPSS_UDP_INCLUDE
#endif
#endif

#ifdef CPSS_IT_TEST
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
#define CPSS_DISK_FILESYS /* only for current test case use H:. */
#endif
#endif

#if (SWP_PHYBRD_TYPE == SWP_PHYBRD_MASA)
#define CPSS_SWITCH_BRDTYPE
#endif

#ifdef SWP_FNBLK_BRDTYPE_ABOX
#define CPSS_TCP_INCLUDE
#endif

#ifdef CPSS_VOS_WINDOWS
#define CPSS_SYS_SOCKET_FTP
#endif

#ifdef CPSS_VOS_LINUX
#define CPSS_SYS_SOCKET_FTP
#endif

/******************************************************************
                  CPSS子系统容量配置宏定义开始
******************************************************************/
/* cpss_vk模块配置 */
#define VOS_MAX_TASK_NUM         (SWP_MAX_TASK_NUM)
#define VOS_MAX_SEM_NUM          (SWP_MAX_SEM_NUM)
#define VOS_MAX_MSG_Q_NUM        (32) /* max message queue number */
#define VK_MAX_SCHED_NUM         (SWP_MAX_SCHED_NUM) /* max sheduler number */
#define VK_MAX_PROC_CLASS_NUM    (SWP_MAX_PROC_CLASS_NUM) /* max procedure class number */
#define VK_MAX_PROC_INST_NUM     (SWP_MAX_PROC_INST_NUM) /* max procedure instance number */
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
#define IPC_MAX_MSGS             (20000) /* IPC max message number */
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
#define IPC_MAX_MSGS             (20000) /* IPC max message number */
#else
#define IPC_MAX_MSGS             (4096) /* IPC max message number */
#endif
#define IPC_MAX_MSG_LEN          (1520) /* IPC max length of message */

/* 内存池最大数量 */
#define MAX_POOL_NUM             (SWP_MAX_POOL_TYPE_NUM)
/* 最大定时器数 */
#define MAX_TIMER_NUM            (SWP_MAX_TIMER_NUM)
/* 最大定时器类别，取值依据《G1接口手册》 */
#define MAX_PROC_TIMER           (32)

/* 同时打开文件最大数，取值依据《G1接口手册》 */
#define FS_MAX_FILES             (60)

/******************************************************************
                  CPSS子系统容量配置宏定义结束
******************************************************************/

/******************************************************************
                  CPSS子系统功能配置宏开关定义开始
******************************************************************/
/* 调度是否支持多实例，支持则定义下面的宏定义 */
#undef CPSS_CONFIG_MULTI_INST

/******************************************************************
                  CPSS子系统功能配置宏开关定义结束
******************************************************************/

#undef  SWP_MC_BOARD_PHY1
#define SWP_MC_BOARD_PHY1 (0x01010101)

#undef SWP_MC_BOARD_PHY2
#define SWP_MC_BOARD_PHY2 (0x01010d01)
#define CPSS_MC_MODULE        (16)
#define CPSS_MC_SUBGROUP    (1)

/******************************** 类型定义 ***********************************/


/******************************** 全局变量声明 *******************************/

/******************************** 函数原形声明 ***************************/


/******************************** 头文件保护结尾 *****************************/
#ifdef  __cplusplus
}
#endif

#endif /* CPSS_CONFIG_H */
/******************************** 头文件结束 *********************************/


