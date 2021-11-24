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
#ifndef SMSS_SYSCTL_H
#define SMSS_SYSCTL_H

/********************************* 头文件包含 ********************************/
#include "swp_public.h"
#include "cpss_public.h"
#include "cps_replant.h"


/******************************** 宏和常量定义 *******************************/
/* 错误码定义 */
#define SMSS_ERRORCODE_BASE               (0x03000000)
#define SMSS_BOOT_ERROR_BASE              (SMSS_ERRORCODE_BASE + 0x00010000) 
#define SMSS_SYSCTRL_ERROR_BASE           (SMSS_ERRORCODE_BASE + 0x00030000) 
#define SMSS_VERM_ERROR_BASE              (SMSS_ERRORCODE_BASE + 0x00050000) 
#define SMSS_DEVM_ERROR_BASE              (SMSS_ERRORCODE_BASE + 0x00070000) 
#define SMSS_RESET_ERROR_BASE             (SMSS_ERRORCODE_BASE + 0x00080000)

/* 倒换失败的原因定义 */
#define SMSS_SYSCTRL_ERR_BSWITCH_FORBID           (SMSS_SYSCTRL_ERROR_BASE + 1)  /* 倒换禁止 */
#define SMSS_SYSCTRL_ERR_BSWITCH_STANDLONE        (SMSS_SYSCTRL_ERROR_BASE + 2)  /* 未配置伙伴板 */
#define SMSS_SYSCTRL_ERR_BSWITCH_MATE_WRONG       (SMSS_SYSCTRL_ERROR_BASE + 3)  /* 伙伴板故障 */
#define SMSS_SYSCTRL_ERR_BSWITCH_STAGE_FORBID     (SMSS_SYSCTRL_ERROR_BASE + 4)  /* 本板不在正常状态 */
#define SMSS_SYSCTRL_ERR_BSWITCH_RUNING           (SMSS_SYSCTRL_ERROR_BASE + 5)  /* 本板正在倒换 */
#define SMSS_SYSCTRL_ERR_BSWITCH_NO_MEMORY        (SMSS_SYSCTRL_ERROR_BASE + 6)  /* 开辟倒换节点空间失败 */
#define SMSS_SYSCTRL_ERR_BSWITCH_TIMEOUT          (SMSS_SYSCTRL_ERROR_BASE + 7)  /* 倒换超时 */
#define SMSS_SYSCTRL_ERR_BSWITCH_NOT_ACTIVE       (SMSS_SYSCTRL_ERROR_BASE + 8)  /* 单板不是主用板 */
#define SMSS_SYSCTRL_ERR_INVALID_MO               (SMSS_SYSCTRL_ERROR_BASE + 9)  /* 无效MO */
#define SMSS_SYSCTRL_ERR_UPLINK_PORT_ERROR        (SMSS_SYSCTRL_ERROR_BASE + 10) /* 上行端口故障 */
#define SMSS_SYSCTRL_ERR_LOST_HEARTBEAT           (SMSS_SYSCTRL_ERROR_BASE + 11)  /* 失去心跳 */
#define SMSS_SYSCTRL_ERR_BOARDFAULT               (SMSS_SYSCTRL_ERROR_BASE + 12)  /* 板级故障 */
#define SMSS_VERM_ERR_VERLIB_FILE_DOWNLOAD        (SMSS_VERM_ERROR_BASE + 1)     /* 版本库版本文件下载失败 */
#define SMSS_VERM_ERR_VERLIB_FILE_CHECK           (SMSS_VERM_ERROR_BASE + 2)     /* 版本库版本文件校验失败 */
#define SMSS_VERM_ERR_FLASH_FILE_DOWNLOAD         (SMSS_VERM_ERROR_BASE + 3)     /* Flash文件下载失败 */
#define SMSS_VERM_ERR_FLASH_FILE_CHECK            (SMSS_VERM_ERROR_BASE + 4)     /* Flash文件校验失败 */
#define SMSS_VERM_ERR_BUF_SIZE                    (SMSS_VERM_ERROR_BASE + 5)     /* 计算校验和时缓冲区大小不是4的整数倍 */
#define SMSS_VERM_ERR_MALLOC                      (SMSS_VERM_ERROR_BASE + 6)     /* 内存分配失败 */
#define SMSS_VERM_ERR_FILE_SIZE                   (SMSS_VERM_ERROR_BASE + 7)     /*  */
#define SMSS_VERM_ERR_FILE_OPEN                   (SMSS_VERM_ERROR_BASE + 8)     /* 文件长度错误 */
#define SMSS_VERM_ERR_FILE_READ                   (SMSS_VERM_ERROR_BASE + 9)     /* 打开文件失败 */
#define SMSS_VERM_ERR_HEAD_WORD                   (SMSS_VERM_ERROR_BASE + 10)     /* 读取文件失败 */
#define SMSS_VERM_ERR_HEAD_LEN                    (SMSS_VERM_ERROR_BASE + 11)     /* 版本文件头部长度错误 */
#define SMSS_VERM_ERR_HEAD_VER                    (SMSS_VERM_ERROR_BASE + 12)     /* 版本文件头部版本错误 */
#define SMSS_VERM_ERR_HEAD_CHECKSUM               (SMSS_VERM_ERROR_BASE + 13)     /* 版本文件头部校验和错误 */
#define SMSS_VERM_ERR_HEAD_FILE_LEN               (SMSS_VERM_ERROR_BASE + 14)     /* 版本文件头部中文件长度错误 */
#define SMSS_VERM_ERR_FILE_CHECKSUM               (SMSS_VERM_ERROR_BASE + 15)     /* 文件校验和错误 */
#define SMSS_VERM_ERR_BUF_CHECKSUM                (SMSS_VERM_ERROR_BASE + 16)     /* 计算缓冲区校验和失败 */
#define SMSS_VERM_ERR_FILE_NAME                   (SMSS_VERM_ERROR_BASE + 17)     /* 版本文件错误 */
#define SMSS_DEVM_ERR_RSN_INVALID                 (SMSS_DEVM_ERROR_BASE + 1)     /* RNC序列号不存在 */
#define SMSS_DEVM_ERR_MUlT_SEND_TESTDIAG          (SMSS_DEVM_ERROR_BASE + 2)     /* 同时多次发送测试诊断消息错误 */
#define SMSS_DEVM_ERR_WAIT_DRV_IND_TIMEROUT        (SMSS_DEVM_ERROR_BASE + 3)  /* 等待驱动指示消息超时 */ 
#define SMSS_DEVM_ERR_PHYBRDTYPE_MISMATCH          (SMSS_DEVM_ERROR_BASE + 4)  /* 物理板类型不匹配 */
#define SMSS_DEVM_ERR_WAIT_RSP_TIMEROUT            (SMSS_DEVM_ERROR_BASE + 5)  /* 查询物理版本超时 */
#define SMSS_DEVM_ERR_NOT_READY                    (SMSS_DEVM_ERROR_BASE + 6)  /* 状态不在升级就绪态 */
#define SMSS_DEVM_ERR_NOT_SUPPORT_INSBY            (SMSS_DEVM_ERROR_BASE + 7)  /* 此功能备用板不支持 */
#define SMSS_DEVM_ERR_UPDATE_CANCEL                 (SMSS_DEVM_ERROR_BASE + 8)  /* 升级取消 */

/* 事件宏定义 */
#define SMSS_EVENTNO_BOARD_STANDBY_TO_ACTIVE    0x00030001
#define SMSS_EVENTNO_BOARD_STARTUP              0x00030002
#define SMSS_EVENTNO_BOARD_SCPU_STARTUP         0x00030003
#define SMSS_EVENTNO_BOARD_ACT_PROC_RSP         0x00030004
#define SMSS_EVENTNO_BOARD_STOA_RSP             0x00030005
#define SMSS_EVENTNO_BOARD_STANDBY_TO_ACTIVE_FAIL  0x00030008

/* 告警宏定义 */
#define SMSS_ALMID_CPU_OVERLOADED               0x00030001
#define SMSS_ALMID_BOARD_NOT_INSTALLED          0x00030002
#define SMSS_ALMID_BOARD_HWVER_ERROR            0x00030003
#define SMSS_ALMID_BOARD_SW_CHECK_FAILED        0x00030004
#define SMSS_ALMID_BOARD_SW_INIT_FAILED         0x00030005
#define SMSS_ALMID_BOARD_SLAVE_TO_ACTIVE_FAILED 0x00030006
#define SMSS_ALMID_BOARD_NOT_RESPONDED          0x00030007
#define SMSS_ALMID_BOARD_SCPU_FAULTY            0x00030008
#define SMSS_ALMID_BOARD_DSP_FAULTY             0x00030009
#define SMSS_ALMID_VERLIB_FILE_ERROR            0x0003000a

/* 告警相关个数定义 */
#define SMSS_SCPU_DSP_ALARM_SUBNO_SUM           4
#define SMSS_DSP_MAX_SUM                        12

/* 告警细节号定义 */
#define SMSS_ALMSUBNO_0                         0
#define SMSS_ALMSUBNO_1                         1
#define SMSS_ALMSUBNO_2                         2
#define SMSS_ALMSUBNO_3                         3
#define SMSS_ALMSUBNO_4                         4
#define SMSS_ALMSUBNO_5                         5

/* CPU No. 宏定义 */
#define SMSS_CPUNO_HOST         1
#define SMSS_CPUNO_SLAVE        2
#define SMSS_CPUNO_1ST_DSP      5

/* IDLE态下的超时定时器 */
#define SMSS_TIMER_IDLE                     CPSS_TIMER_00
#define SMSS_TIMER_IDLE_MSG                 CPSS_TIMER_00_MSG
#define SMSS_TIMER_IDLE_DELAY               1000

/* INIT态下的超时定时器 */
#define SMSS_TIMER_INIT                     CPSS_TIMER_01
#define SMSS_TIMER_INIT_MSG                 CPSS_TIMER_01_MSG
#define SMSS_TIMER_INIT_DELAY               1000

/* 实现上下级周期心跳的定时器 */
#define SMSS_TIMER_PERIODLY_HEARTBEAT       CPSS_TIMER_02
#define SMSS_TIMER_PERIODLY_HEARTBEAT_MSG   CPSS_TIMER_02_MSG
#define SMSS_TIMER_PERIODLY_HEARTBEAT_INTERVAL 3000

/* 实现主备间周期心跳的定时器 */
#define SMSS_TIMER_MATE_HEARTBEAT           CPSS_TIMER_03
#define SMSS_TIMER_MATE_HEARTBEAT_MSG       CPSS_TIMER_03_MSG
#define SMSS_TIMER_MATE_HEARTBEAT_INTERVAL  1000

/* END态的超时定时器 */
#define SMSS_TIMER_END                      CPSS_TIMER_04
#define SMSS_TIMER_END_MSG                  CPSS_TIMER_04_MSG
#define SMSS_TIMER_END_DELAY                3000

/* CPU占用率查询定时器 */
#define SMSS_TIMER_CPU_USAGE_QUERY          CPSS_TIMER_05
#define SMSS_TIMER_CPU_USAGE_QUERY_MSG      CPSS_TIMER_05_MSG
#define SMSS_TIMER_CPU_USAGE_QUERY_DELAY    5000

/* 硬件主备竞争定时器 */
#define SMSS_TIMER_HARD_CMP_TIMER           CPSS_TIMER_06
#define SMSS_TIMER_HARD_CMP_MSG             CPSS_TIMER_06_MSG
#define SMSS_TIMER_HARD_CMP_DELAY           1500
/* 单板延迟复位定时器 */
#define SMSS_TIMER_BRD_RST_TIMER            CPSS_TIMER_07
#define SMSS_TIMER_BRD_RST_MSG              CPSS_TIMER_07_MSG

/* 整框延迟复位定时器 */
#define SMSS_TIMER_FRAME_RST_TIMER          CPSS_TIMER_08
#define SMSS_TIMER_FRAME_RST_MSG            CPSS_TIMER_08_MSG

/* 机框管理定义 */
/* 定义用于巡检机框内各单板状态的定时器 */
#define SMSS_TIMER_SHM_MONITOR              CPSS_TIMER_09
#define SMSS_TIMER_SHM_MONITOR_MSG          CPSS_TIMER_09_MSG
#define SMSS_TIMER_SHM_MONITOR_DELAY        200

/* 向主用外围板发送完倒换命令后起的定时器 */
#define SMSS_TIMER_BSWITCH_CMD              CPSS_TIMER_10
#define SMSS_TIMER_BSWITCH_CMD_MSG          CPSS_TIMER_10_MSG
#define SMSS_TIMER_BSWITCH_CMD_DELAY        10000

/* 向伙伴板发送完倒换请求后起的定时器 */
#define SMSS_TIMER_BSWITCH_REQ              CPSS_TIMER_11
#define SMSS_TIMER_BSWITCH_REQ_MSG          CPSS_TIMER_11_MSG
#define SMSS_TIMER_BSWITCH_REQ_DELAY        2000

/* 向主用全局板发送完倒换完成指示消息后起的定时器 */
#define SMSS_TIMER_BSWITCH_IND              CPSS_TIMER_12
#define SMSS_TIMER_BSWITCH_IND_MSG          CPSS_TIMER_12_MSG
#define SMSS_TIMER_BSWITCH_IND_DELAY        1000

/* 发送状态改变消息后起的超时定时器 */
#define SMSS_TIMER_STACHG_IND               CPSS_TIMER_13
#define SMSS_TIMER_STACHG_IND_MSG           CPSS_TIMER_13_MSG
#define SMSS_TIMER_STACHG_IND_DELAY         1000

/* 定义启动DSP或者从CPU的定时器 */
#define SMSS_TIMER_SD_START                 CPSS_TIMER_14
#define SMSS_TIMER_SD_START_MSG             CPSS_TIMER_14_MSG
#define SMSS_TIMER_SD_START_DELAY           (60 * 1000)
#define SMSS_TIMER_SD_START_SLAVE_DELAY           (30 * 60 * 1000)

/* 整网元延迟复位定时器 */
#define SMSS_TIMER_GLOBAL_RST_TIMER          CPSS_TIMER_15
#define SMSS_TIMER_GLOBAL_RST_MSG            CPSS_TIMER_15_MSG

/* 备用全局板，在整网元升级过程中在备升主失败时的定时器 */
#define SMSS_TIMER_STANDBY_RESET_IND               CPSS_TIMER_16
#define SMSS_TIMER_STANDBY_RESET_MSG           CPSS_TIMER_16_MSG
#define SMSS_TIMER_STANDBY_RESET_DELAY         10000

/* 主CPU等从CPU正常后，向全局板发状态指示定时器 */
#define SMSS_TIMER_STATUS_NORMAL_IND           CPSS_TIMER_17
#define SMSS_TIMER_STATUS_NORMAL_MSG           CPSS_TIMER_17_MSG
#define SMSS_TIMER_STATUS_NORMAL_DELAY         1000   /* 1s */

/* cpu占用率超阀值时启15分钟定时器 */
#define SMSS_TIMER_CPU_USAGE               CPSS_TIMER_18
#define SMSS_TIMER_CPU_USAGE_MSG           CPSS_TIMER_18_MSG
#define SMSS_TIMER_CPU_USAGE_DELAY         (15 * 60 * 1000)   /* 15min */

/* FPGA复位后延迟上报单板状态正常 */
#define SMSS_TIMER_FPGA_RESET               CPSS_TIMER_19
#define SMSS_TIMER_FPGA_RESET_MSG           CPSS_TIMER_19_MSG
#define SMSS_TIMER_FPGA_RESET_DELAY         (1000)   /* 1s */

/* CPU运行状态宏定义 */
#define SMSS_RSTATUS_IDLE     0
#define SMSS_RSTATUS_INIT     1
#define SMSS_RSTATUS_NORMAL   2
#define SMSS_RSTATUS_STOA     3
#define SMSS_RSTATUS_END      4

/* CPU管理状态宏定义  */
#define SMSS_MSTATUS_NORMAL   0
#define SMSS_MSTATUS_BLOCK    1
#define SMSS_MSTATUS_DBLOCK   2
#define SMSS_BRD_RSTATUS_INVALID     0
#define SMSS_BRD_RSTATUS_OFFLINE     1
#define SMSS_BRD_RSTATUS_TYPERR      2
#define SMSS_BRD_RSTATUS_NORMAL      3
#define SMSS_BRD_RSTATUS_FAULT       4

/* 激活纤程的阶段 */
#define SMSS_SEND_PROC   20  /* 正在向某纤程发激活消息 */
#define SMSS_SEND_ALL    21  /* 向所有纤程发完激活消息 */
#define SMSS_SEND_OK     22  /* 收到所有纤程的激活应答 */

/* 主控纤程状态宏定义 */
#define SMSS_SYSCTL_STATE_IDLE    CPSS_VK_PROC_USER_STATE_IDLE   /* 空闲态 */
#define SMSS_SYSCTL_STATE_INIT    31   /* 初始化态 */
#define SMSS_SYSCTL_STATE_NORMAL  32   /* 正常态 */
#define SMSS_SYSCTL_STATE_STOA    33   /* 备升主过程 */
#define SMSS_SYSCTL_STATE_END     34   /* 故障态 */

/* 故障上报原因宏定义 */
#define SMSS_BFAULT_ALL_DSPS_FAILED 41
#define SMSS_BFAULT_SLAVE_FAILED    42
#define SMSS_BFAULT_DEVM_ALARM      43
#define SMSS_BFAULT_INIT_FAILED     44
#define SMSS_BFAULT_STOA_FAILED     45

#define SMSS_VER_CHG                46

/* 定义故障上报类型 */
#define SMSS_BFAULT_SLAVE_TO_HOST   46
#define SMSS_BFAULT_HOST_TO_GCPA    47
#define SMSS_BFAULT_MATE_TO_MATE    48

/* 定义状态改变原因，用于发送status_chg_ind消息 */
#define SMSS_INIT_OK                51
#define SMSS_STOA_START             52

/* OAMS发起的复位的类型宏定义 */
#define SMSS_FORCE_RST_SPC_SLOT     61  /* 复位制定槽位的单板 */
#define SMSS_FORCE_RST_ALL_SHELF    62  /* 复位整框 */
#define SMSS_FORCE_RST_GLOBAL       63  /* 复位整个网元，仅复位两个全局板 */

/* IDLE态的不同阶段 */
#define SMSS_IDLE_INIT              70
#define SMSS_IDLE_ACT_CPSS          71
#define SMSS_IDLE_REG               72
#define SMSS_IDLE_AS_CMP            73

/* 定义单板未配置的宏 */
#define SMSS_BOARD_CONFIG_NONE      0xFF


/* 定义事件上报类型 */
#define SMSS_EVENT_SLAVE_TO_HOST   80
#define SMSS_EVENT_HOST_TO_GCPA    81

/* 对单板在线状态的宏定义 */
#define SMSS_BOARD_ONLINE  0   /* 单板在线 */
#define SMSS_BOARD_OFFLINE 1   /* 单板离线 */
#define SMSS_BOARD_UNKNOWN 2   /* 单板在线状态未知 */

/* 定义未知主备状态的宏 */
#define SMSS_UNKNOWN_AS_STATUS      0xFF

/* 消息宏定义 */
#define SMSS_SYSCTL_REG_RSP_MSG          0x75010000
#define SMSS_SYSCTL_REG_REQ_MSG          0x75010001
#define SMSS_SYSCTL_STATUS_CHG_IND_MSG   0x75010002
#define SMSS_SYSCTL_STATUS_CHG_ACK_MSG   0x75010003
#define SMSS_SYSCTL_BFAULT_IND_MSG       0x75010004
#define SMSS_SYSCTL_GP_HEARTBEAT_IND_MSG 0x75010005
#define SMSS_SYSCTL_AS_HEARTBEAT_IND_MSG 0x75010006
#define SMSS_SYSCTL_HS_HEARTBEAT_IND_MSG 0x75010007
#define SMSS_SYSCTL_DSP_START_CMD_MSG    0x75010009
#define SMSS_SYSCTL_BRD_OL_CHG_IND_MSG   0x7501000a
#define SMSS_ACTIVE_STANDBY_CMP_REQ_MSG  0x7501000b
#define SMSS_ACTIVE_STANDBY_CMP_RSP_MSG  0x7501000c
#define SMSS_ACT_STOA_RSP_IND_MSG        0x7501000d
#define SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG    0x7501000e
#define SMSS_SYSCTL_FORCE_RST_CMD_RSP_MSG    0x7501000f
#define SMSS_SYSCTL_BSWITCH_IND_MSG      0x75010020
#define SMSS_SYSCTL_BSWITCH_CMD_MSG      0x75010021
#define SMSS_SYSCTL_BSWITCH_REQ_MSG      0x75010022
#define SMSS_SYSCTL_BSWITCH_RSP_MSG      0x75010023
#define SMSS_SYSCTL_BSWITCH_ACK_MSG      0x75010024

/* 板级故障指示消息 */
#define SMSS_DEVM_BFAULT_IND_MSG         0x75040001
/* 从CPU告警数目改变上报消息 */
#define SMSS_DEVM_ALMCHG_IND_MSG         0x75040002
/* NSCA设置对端时钟信息消息 */
#define SMSS_DEVM_SET_NSCA_CLKINFO_MSG  0x75040003
/* 备用NSCA设置对端时钟信息消息 */
#define SMSS_DEVM_SBYNSCA_SET_CLKINFO_MSG  0x75040004
/* 用于调试的消息 */
#define SMSS_SHOW_BOARD_INFO_MSG         0x75010031
#define SMSS_RESET_BOARD_CMD_MSG         0x75010032
#define SMSS_RESET_SLAVE_CMD_MSG         0x75010033

/* 全局板正常后向VERM纤程发单板正常指示消息 */
#define SMSS_SYSCTL_PROC_NORMAL_IND_MSG         0x75010034
#define SMSS_SYSCTL_LOST_MATEHB_MSG      0x75010056
#define SMSS_SYSCTL_LOST_GCPAHB_MSG      0x75010057
#define SMSS_SYSCTL_LOST_HOSTHB_MSG      0x75010058
#define SMSS_SYSCTL_LOST_SLAVEHB_MSG     0x75010059
#define SMSS_SYSCTL_LOST_DSPHB_MSG       0x7501005a
#define SMSS_VERSION_UPDATE_IND_MSG      0x7501005c
#define SMSS_CPU_RSTATE_SET_IND_MSG      0x7501005d
#define SMSS_BOARD_INFO_QUERY_REQ_MSG    0x7501005e
#define SMSS_BOARD_INFO_QUERY_RSP_MSG    0x7501005f

/* 备用全局板向主用全局板发送整网元升级失败指示 */
#define SMSS_SYSCTL_GCPA_UPGRADE_FAIL_IND_MSG       (0x75010060)

#define SMSS_SYSCTL_BRD_ALARM_SEND_IND       (0x75010061)

#define SMSS_CFG_MASA_ABOUT_GSSA_MSG         (0x75010062)

/* 主用交换板收到此消息, 向本框内的单板发清黑匣子标志消息 */
#define SMSS_SYSCTL_CLEAR_SBBR_FLAG_MSG          (0x75010063)

/* 二级交换板向本框其他单板发更新上级时钟指示
 * 或时钟板向所有二级交换板发更新上级时钟指示
 */
#define SMSS_SYSCTL_UPPERCLK_IND_MSG           (0x75010064)
/* 二级交换板向主用全局板查询主用时钟板请求
 * 或其他单板向主用全局板查询本框主用MASA请求
 */
#define SMSS_SYSCTL_UPPERCLK_QUERY_REQ_MSG   (0x75010065)
/* 主用全局板向二级交换板查询主用时钟板响应
 * 或主用全局板向其他单板主用全局板查询本框主用MASA响应
 */
#define SMSS_SYSCTL_UPPERCLK_QUERY_RSP_MSG   (0x75010066)

/* 用于一级交换板, 二级交换板特殊处理 */ 
#define SMSS_SYSCTL_BOARD_STATUS_IND_MSG     (0x75010067)
#define SMSS_SYSCTL_BOARD_STATUS_ACK_MSG     (0x75010068)
/* 要求sysctl删除dsp定时器 */
#define SMSS_SYSCTL_DEL_DSP_TIMER_MSG        (0x75010069)

/* MASA设置风扇参数请求 */
#define SMSS_SYSCTL_FANPARA_REQ_MSG      (0x7501006a)
/* MASA设置风扇参数响应 */
#define SMSS_SYSCTL_FANPARA_RSP_MSG      (0x7501006b) 
/* 查询CPU负荷控制信息请求 */
#define SMSS_SYSCTL_CPULOADINFO_REQ_MSG      (0x7501006c) 
/* 查询CPU负荷控制信息响应 */
#define SMSS_SYSCTL_CPULOADINFO_RSP_MSG      (0x7501006d) 
/* 查询单板负荷控制信息请求 */
#define SMSS_SYSCTL_BOARDLOADINFO_REQ_MSG      (0x7501006e) 
/* 查询单板负荷控制信息响应 */
#define SMSS_SYSCTL_BOARDLOADINFO_RSP_MSG      (0x7501006f) 
/* 从CPU向主CPU通知CPU占用率 */
#define SMSS_SYSCTL_SLAVECPU_IND_MSG           (0x75010070)
/* 向主用GCPA发单板级别改变消息 */
#define SMSS_SYSCTL_BOARDLEVELCHG_IND_MSG      (0x75010071)
/* 向自身发消息查询RDBS配置的硬件版本信息 */
#define SMSS_SYSCTL_CHECKPHYVER_REQ_MSG      (0x75010072)

/* SMSS_FIRMWARE_PROC 消息 */
/* 向被升级单板发物理板类型查询请求 */
#define SMSS_FIRMWARE_CHECK_PHYVER_REQ_MSG  0x75050001
/* 被升级单板回物理板类型查询响应 */
#define SMSS_FIRMWARE_CHECK_PHYVER_RSP_MSG  0x75050002
/* 外围板向主用全局板发FTP下载请求消息 */
#define SMSS_FIRMWARE_FTP_REQ_MSG       0x75050003 
/* 主用全局板向外围板发FTP下载响应消息 */
#define SMSS_FIRMWARE_FTP_RSP_MSG       0x75050004
/* 外围板向全局板发删除IPMC文件消息 */
#define SMSS_FIRMWARE_DELETE_IPMC_REQ_MSG  0x75050005
/* SMSS_FIRMWARE_PROC 发送物理版本信息请求定时器,  */
#define SMSS_FIRMWARE_TIMER_PHYVER             CPSS_TIMER_01
#define SMSS_FIRMWARE_TIMER_PHYVER_MSG         CPSS_TIMER_01_MSG
#define SMSS_FIRMWARE_TIMER_PHYVER_DELAY       3000



/*主CPU丢失DSP的心跳*/
typedef struct 
{
   UINT32 ulDspIndex;/*0--11*/
}SMSS_SYSCTL_LOST_DSPHB_MSG_T;  

typedef  CPSS_FTP_REQ_MSG_T  SMSS_FIRMWARE_FTP_REQ_MSG_T;
typedef  CPSS_FTP_RSP_MSG_T  SMSS_FIRMWARE_FTP_RSP_MSG_T;

/* SMSS单板复位原因定义 */
#define SMSS_BRD_RST_CAUSE_BASE                             (SMSS_RESET_ERROR_BASE + 100)

/* 备用板在IDLE状态下收到伙伴板的故障指示 */
#define SMSS_BRD_RST_CAUSE_MATE_FAULT_SLAVE_BRD_IDLE                 (SMSS_BRD_RST_CAUSE_BASE+1)
/* 非主用全局单板注册时超过指定次数未收到全局单板注册响应消息 */
#define SMSS_BRD_RST_CAUSE_REG_TIMEOUT                               (SMSS_BRD_RST_CAUSE_BASE+2)
/* 单板激活CPSS纤程超时引起单板复位 */
#define SMSS_BRD_RST_CAUSE_ACT_PROC_CPSS_TIMEOUT_IDLE                (SMSS_BRD_RST_CAUSE_BASE+3)
/* 备用板在初始化状态下收到伙伴板的故障指示 */
#define SMSS_BRD_RST_CAUSE_RECV_MATA_FAULT_SLAVE_BRD_INIT            (SMSS_BRD_RST_CAUSE_BASE+4)
/* 备用单板在备升主过程中收到纤程激活失败响应 */
#define SMSS_BRD_RST_CAUSE_ACT_PROC_FAILURE_SLAVE_BRD_STOA           (SMSS_BRD_RST_CAUSE_BASE+5)
/* 备用单板在备升主过程中纤程激活超时 */
#define SMSS_BRD_RST_CAUSE_ACT_PROC_TIMEOUT_SLAVE_BRD_STOA           (SMSS_BRD_RST_CAUSE_BASE+6)
/* 向主用全局板发送单板状态改变消息超时十次未收到全局单板的响应 */
#define SMSS_BRD_RST_CAUSE_STATUS_CHG_IND_TIMEOUT                    (SMSS_BRD_RST_CAUSE_BASE+7)
/* 单板初始化阶段激活某一个纤程超时引起单板复位 */
#define SMSS_BRD_RST_CAUSE_ACT_PROC_TIMEOUT_INIT                     (SMSS_BRD_RST_CAUSE_BASE+8)
/* 单板初始化时内存分配失败引起单板复位 */
#define SMSS_BRD_RST_CAUSE_ALLOC_MEM_FAILURE_INIT                    (SMSS_BRD_RST_CAUSE_BASE+9)
/* 单板发现主用全局单板的心跳缺失超过5次引起的单板复位 */
#define SMSS_BRD_RST_CAUSE_LOST_HEARTBEAT_GCPA                       (SMSS_BRD_RST_CAUSE_BASE+10)
/* 单板主控模块收到设备监控模块上报严重告警 */
#define SMSS_BRD_RST_CAUSE_RECV_DEVM_BRD_FAULT                       (SMSS_BRD_RST_CAUSE_BASE+11)
/* 单板IDLE状态下激活收到激活CPSS纤程失败响应 */
#define SMSS_BRD_RST_CAUSE_ACT_PROC_CPSS_FAILED_IDLE                 (SMSS_BRD_RST_CAUSE_BASE+12)
/* 备用单板在非正常运行状态下收到主用板故障引起的切换请求 */
#define SMSS_BRD_RST_CAUSE_SLAVE_BRD_RECV_BSWITCH_REQ_DISNORMAL      (SMSS_BRD_RST_CAUSE_BASE+13)
/* 主用单板降备引起单板复位 */
#define SMSS_BRD_RST_CAUSE_ACTIVE_BRD_ATOS                           (SMSS_BRD_RST_CAUSE_BASE+14)
/* 备用单板向全局单板发送单板切换完成指示超过十次未收到全局单板的响应 */
#define SMSS_BRD_RST_CAUSE_BSWITCH_IND_TIMEOUT                       (SMSS_BRD_RST_CAUSE_BASE+15)
/* 备用板在备升主过程中收到设备监控模块上报板级故障指示 */
#define SMSS_BRD_RST_CAUSE_DEVM_REPORT_BRD_FAULT_IND_STOA            (SMSS_BRD_RST_CAUSE_BASE+16)
/* 备用板在IDLE状态下收到收到驱动的上报的本板状态改变指示 */
#define SMSS_BRD_RST_CAUSE_DRV_REPORT_BRD_STAT_CHG_SLAVE_BRD_IDLE    (SMSS_BRD_RST_CAUSE_BASE+17)
/* 备用板在IDLE状态下收到收到驱动的上报的伙伴板复位指示消息 */
#define SMSS_BRD_RST_CAUSE_DRV_REPORT_MATA_RST_IND_SLAVE_BRD_IDLE    (SMSS_BRD_RST_CAUSE_BASE+18)
/* 备用板在IDLE状态下收到主板故障要求切换指示 */
#define SMSS_BRD_RST_CAUSE_BSWITCH_REQ_ACT_BRD_FAULT_SLAVE_BRD_IDLE  (SMSS_BRD_RST_CAUSE_BASE+19)
/* 备用单板在初始化状态下收到驱动的上报的倒换指示 */
#define SMSS_BRD_RST_CAUSE_DRV_REPORT_BSWITCH_IND_SLAVE_BRD_INIT     (SMSS_BRD_RST_CAUSE_BASE+20)
/* 备用板在初始化状态下收到主板故障要求切换指示 */
#define SMSS_BRD_RST_CAUSE_BSWITCH_REQ_ACT_BRD_FAULT_SLAVE_BRD_INIT  (SMSS_BRD_RST_CAUSE_BASE+21)
/* 单板初始化过程中激活纤程失败引起单板复位 */
#define SMSS_BRD_RST_CAUSE_ACT_PROC_FAILURE_INIT                     (SMSS_BRD_RST_CAUSE_BASE+22)
/* 如果是本板故障引起的倒换在等待切换响应超时 */
#define SMSS_BRD_RST_CAUSE_WAIT_SWITCH_RSP_TIMEOUT                   (SMSS_BRD_RST_CAUSE_BASE+23)
/* 备用全局板升主完成存储RDBS的状态时开辟内存失败 */
#define SMSS_BRD_RST_CAUSE_SAVE_CPS__RDBS_STAT_GCPA_ALLOC_MEM_FAILURE     (SMSS_BRD_RST_CAUSE_BASE+24)
/* 全局单板收到RDBS的单板的管理状态改变时开辟内存失败 */
#define SMSS_BRD_RST_CAUSE_MSTATE_CHG_ALLOC_MEM_FAILURE              (SMSS_BRD_RST_CAUSE_BASE+25)
/* 主用全局单板收到单板注册消息时开辟内存失败 */
#define SMSS_BRD_RST_CAUSE_RECV_BRD_REG_ALLOC_MEM_FAILURE            (SMSS_BRD_RST_CAUSE_BASE+26)
/* 全局单板初始化时开辟内存空间失败 */
#define SMSS_BRD_RST_CAUSE_ALLOC_MEM_FAILURE_GCPA_INIT               (SMSS_BRD_RST_CAUSE_BASE+27)
/* 主CPU收到从CPU上报的CPU级故障时 */
#define SMSS_BRD_RST_CAUSE_SLAVE_CPU_FAULT_MASTER_CPU                (SMSS_BRD_RST_CAUSE_BASE+28)
/* 主CPU对从CPU的心跳丢失超过5次 */
#define SMSS_BRD_RST_CAUSE_LOST_HEARTBEAT_MASTER_CPU                 (SMSS_BRD_RST_CAUSE_BASE+29)
/* 所有DSP全部故障 */
#define SMSS_BRD_RST_CAUSE_ALL_DSP_FAULT                             (SMSS_BRD_RST_CAUSE_BASE+30)
/* 激活版本 */
#define SMSS_BRD_RST_CAUSE_ACTIVATE_VERSION                          (SMSS_BRD_RST_CAUSE_BASE+31)

/* 驱动初始化失败 */
#define SMSS_BRD_RST_CAUSE_DRV_INIT_FAIL                             (SMSS_BRD_RST_CAUSE_BASE+32)

/* 主用全局板在升级前校验配置版本失败 */
#define SMSS_BRD_RST_CAUSE_ACTIVE_GCPA_VERIFY_FAIL                   (SMSS_BRD_RST_CAUSE_BASE+33) 
/* 二级交换主用板上行业务端口故障引起的复位 */
#define SMSS_BRD_RST_CAUSE_UL_PORT_FAIL                   (SMSS_BRD_RST_CAUSE_BASE+34) 
/* 媒体面业务端口故障引起的复位 */
#define SMSS_BRD_RST_CAUSE_MEDIA_PORT_FAIL                   (SMSS_BRD_RST_CAUSE_BASE+35) 
/* 主CPU上ME通告状态失败引起的复位 */
#define SMSS_BRD_RST_CAUSE_ME_FAIL                                   (SMSS_BRD_RST_CAUSE_BASE+36) 
/* CPLD升级导致的复位 */
#define SMSS_BRD_RST_CAUSE_CPLD_UPDATE                               (SMSS_BRD_RST_CAUSE_BASE+37) 

/* 硬件版本不一致导致的复位 */
#define SMSS_BRD_RST_CAUSE_HWINFO_FAIL                                (SMSS_BRD_RST_CAUSE_BASE+38)

/* 双主用复位 */
#define SMSS_BRD_RST_CAUSE_ALLACTIVE                                  (SMSS_BRD_RST_CAUSE_BASE+39)

/* 正常阶段, 主用收到驱动的倒换指示, 复位 */
#define SMSS_BRD_RST_CAUSE_DRV_REPORT_BSWITCH_IND_NORMAL              (SMSS_BRD_RST_CAUSE_BASE+40)
#define SMSS_BRD_RST_FORCE_RESET                                     (1)

/* CPU占用率当前的状态定义 */
#define SMSS_CPU_USAGE_OVER_LOAD                 1
#define SMSS_CPU_USAGE_OVER_NORMAL               0
#define SMSS_CPU_USAGE_CHECK_COUNT               3
/* 单板类型定义 */
#if SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE
/* 主CPU属性宏定义 */
#if ( (SWP_PHYBRD_MEIA == SWP_PHYBRD_TYPE)||(SWP_PHYBRD_MNPA == SWP_PHYBRD_TYPE)\
     &&(SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE))
#define SMSS_BOARD_WITH_SLAVE

#elif (  ((SWP_PHYBRD_MDPA == SWP_PHYBRD_TYPE)||(SWP_PHYBRD_IWFA == SWP_PHYBRD_TYPE))\
       &&(SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE))
#define SMSS_BOARD_WITH_DSP
#define SMSS_DSP_TOTAL_NUM      12 /* MDPA有12个DSP */
#endif

/* 机框管理定义 */
#if (SWP_FUNBRD_TCSA == SWP_FUNBRD_TYPE)
#define SMSS_BOARD_WITH_SHM

#endif


#if ((SWP_PHYBRD_GSSA == SWP_PHYBRD_TYPE)||(SWP_PHYBRD_MASA == SWP_PHYBRD_TYPE)\
||defined(SWP_FNBLK_BRDTYPE_ONCA))
#define SMSS_BOARD_WITHOUT_RDBS
#endif


/* 单板备份方式宏定义 */

#if ( (SWP_PHYBRD_MMPA == SWP_PHYBRD_TYPE)||(SWP_PHYBRD_MMPA_OEM == SWP_PHYBRD_TYPE)\
    ||(SWP_PHYBRD_NSCA == SWP_PHYBRD_TYPE)||(SWP_PHYBRD_MASA == SWP_PHYBRD_TYPE))

#define SMSS_BACKUP_MATE

#ifndef SMSS_AS_CMP_SOFT
#if (SWP_PHYBRD_MMPA_OEM == SWP_PHYBRD_TYPE)   /* 确定主备竞争类型 */
#define SMSS_AS_CMP_SOFT    /* 软件主备竞争 */
#else
#define SMSS_AS_CMP_HARD    /* 硬件主备竞争 */
#endif
#endif

#else
#define SMSS_BACKUP_ALONE
#endif
#endif

#define SMSS_ADDR_IS_EQUAL(a, b)  ((*(UINT32*)&(a)) == (*(UINT32*)&(b)))

#ifdef SMSS_STUB
extern VOID smss_stub_cpss_reset(UINT32 ulCause);
extern VOID smss_stub_cps__rdbs_dsync(VOID);
extern VOID smss_stub_cps__oams_am_send_event(CPS__OAMS_AM_ORIGIN_EVENT_T * pstEvent);
extern VOID smss_stub_cps__oams_am_send_alarm(CPS__OAMS_AM_ORIGIN_ALARM_T * pstAlarm);
extern VOID smss_stub_cps__rdbs_if_entry(UINT32 ulMsgId, UINT8 ucCallType,
                   UINT8 *pucInPara, UINT32 ulInParaLen,
                   UINT8 *pucOutPara, UINT32 ulOutParaLen);
#define cps__rdbs_if_entry      smss_stub_cps__rdbs_if_entry
extern VOID smss_stub_cps__oams_shcmd_reg(STRING szFuncName, STRING szFuncHelp,
                   STRING szArgFmt, CPS__OAMS_SHCMD_PF  pfShcmdFunc);
#define cpss_reset         smss_stub_cpss_reset
#define cps__oams_am_send_event smss_stub_cps__oams_am_send_event
#define cps__oams_am_send_alarm smss_stub_cps__oams_am_send_alarm
#define cps__oams_shcmd_printf  printf
#define cps__oams_shcmd_reg     smss_stub_cps__oams_shcmd_reg
#endif
#ifdef SMSS_STUB_V3
#ifndef SMSS_BOARD_WITHOUT_RDBS
extern VOID smss_stub_cps__rdbs_if_entry(UINT32 ulMsgId, UINT8 ucCallType,
                   UINT8 *pucInPara, UINT32 ulInParaLen,
                   UINT8 *pucOutPara, UINT32 ulOutParaLen);
#define cps__rdbs_if_entry      smss_stub_cps__rdbs_if_entry
#endif
#endif

#define  SMSS_HEART_RECORD_MAX_CNT  10
typedef struct 
{
    CPSS_TIME_T stTimer; /* 当前的绝对时间 时分秒 */
    UINT32      ultickcnt; /* 当前的TICK计数 */
    UINT32      ulSendSeq; /* 当前发送的序号 */
    UINT32      ulRecvSeq; /* 当前接收的序号 */ 
}SMSS_HEART_TIME_T;

typedef struct 
{
    UINT32 ulRecvHBCnt;       /* 当前接收到的心跳计数 */
    SMSS_HEART_TIME_T stRecvHB[SMSS_HEART_RECORD_MAX_CNT];
    UINT32 ulSendHBCnt;       /* 当前发送到的心跳计数 */
    SMSS_HEART_TIME_T stSendHB[SMSS_HEART_RECORD_MAX_CNT];
}SMSS_HEART_BRD_T;

#define  SMSS_SEND_HEART   0
#define  SMSS_RECV_HEART   1
#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */

/* 全局板上保存各外围板的相关信息 */
typedef struct _SMSS_BOARD_INFO_T
{
    struct _SMSS_BOARD_INFO_T * pstPrev;
    struct _SMSS_BOARD_INFO_T * pstNext;
    BOOL   bInList;             /* 该节点是否在心跳链表中 */
    UINT8  ucBeatCount;         /* 心跳次数 */

    CPSS_COM_PHY_ADDR_T   stPhyAddr; /* 单板物理地址 */
    CPSS_COM_LOGIC_ADDR_T stLogAddr; /*  单板逻辑地址 */

    UINT32 ulPhyBrdType;           /* 物理单板类型 */
    UINT8  ucFunBrdType;           /* 逻辑单板类型 */
    
    UINT8  ucAStatus;           /* 主备状态 */
    UINT8  ucRStatus;           /* 运行状态 */
    UINT8  ucMStatus;           /* 管理状态 */

    UINT16 usVerState;    /* 单板版本状态字
                           *   Bit0 : 0-正常,         1-故障
                           *   Bit1 : 0-没有BOOT,     1-正在BOOT
                           *   Bit2 : 0-没有在下载,   1-正在下载
                           *   Bit3 : 0-没有延迟激活, 1-有延迟激活
                           */
    UINT8  aucRsv[2];             /* 记录单板故障的告警 */
    UINT8  ucOldRStatus ;         /*原来的运行状态*/
    SWP_PHYBRD_INTEGRATED_INFO_VER_T stPhyBoardVer;
    UINT8  ucHardVerAlmFlag;      /* 单板硬件版本错误告警标志 */   
    SMSS_HEART_BRD_T stHeartInfo;  /* 此结构改为记录纤程初始化失败的信息, 等单板正常后清除 */
    UINT32 ulSndHeartSeq;          /* 此变量记录是否此单板曾经激和失败, 0: 无, 1: 有 */
    UINT32 ulRevHeartSeq;  /* 接收的心跳消息序列号 */
    UINT8 ucBrdRStatus;    /* 单板运行状态 */
    UINT8 ucSubNoFlag[2];  /* 告警细节号产生标志 */
} SMSS_BOARD_INFO_T;
extern  SMSS_BOARD_INFO_T  **apstBoardIndex;  /* 保存各外围板节点的指针数组 */

#define Phy2BoardIndex(phy) \
        (((phy).ucFrame-1) * CPSS_COM_MAX_SHELF * CPSS_COM_MAX_SLOT \
         + ((phy).ucShelf-1) * CPSS_COM_MAX_SLOT \
         + (phy).ucSlot-1)

#endif /* 全局板 */

#ifndef DRV_BOARD_AS_STOA
#define DRV_BOARD_AS_STOA 2
#endif

#ifndef DRV_BOARD_AS_ATOS
#define DRV_BOARD_AS_ATOS 1
#endif



VOID smss_sysctl_record_heart(SMSS_HEART_BRD_T *pstHeartInfo,UINT8 ucFlag,UINT32 ulHeartSeq);

/******************************** 类型定义 ***********************************/
/* 保存本CPU所关心的基本信息 */
typedef struct
{
    UINT8  ucRStatus;                 /* 本CPU的运行状态值，参考宏定义 */
    UINT8  ulBeatCount;               /* 主用全局板的心跳次数 */
    STRING                 szRStatus;  /* 本CPU的运行状态字符串，用于输出 */
    CPSS_COM_PHY_ADDR_T    stPhyAddr; /* 本CPU的物理地址 */
    CPSS_COM_LOGIC_ADDR_T  stLogAddr; /* 本CPU的逻辑地址 */
    UINT32 ulResetReason;             /* 保存本板复位的原因 */
    SMSS_HEART_BRD_T stHeartInfo;
    UINT32 ulHeartBeatSndSeq;        /* 此变量用于在全局板上打印一个单板的心跳 */ 
    UINT32 ulMateHBSndSeq;
}SMSS_CPU_BASE_INFO_T;

/* 注册请求消息 */
typedef struct
{
    CPSS_COM_PHY_ADDR_T  stPhyAddr;    /* 物理地址 */
    UINT8   ucAStatus;                 /* 单板的主备状态
                                          SMSS_ACTIVE: 主用
                                          SMSS_STANDBY:备用
                                       */
    UINT32 ulPhyBrdType;  /* 物理单板类型 */
} SMSS_SYSCTL_REG_REQ_MSG_T;

/* 注册响应消息结构体 */
typedef struct
{
    UINT32  ulResult;     /* 注册结果
                             SMSS_OK: 注册成功
                             其它: 注册失败原因
                           */
} SMSS_SYSCTL_REG_RSP_MSG_T;

/* 全局板与外围板间心跳消息 */
typedef struct
{
    CPSS_COM_PHY_ADDR_T  stPhyAddr;    /* 物理地址 */
    UINT32  ulSeqNum;                  /* 心跳序号 */
} SMSS_SYSCTL_GP_HEARTBEAT_IND_T;

/* 1+1主备板上，保存和自己倒换相关的信息。 */
typedef struct
{
    UINT32 ulReason;     /* 倒换原因 */
    UINT32 ulSeqId;      /* 倒换时的序列号 */
    BOOL   bSwitching;   /* 表示单板是否正在倒换 TRUE：表示正在倒换
                                                 FALSE：不在倒换 */
    CPS__OAMS_MO_ID_T      stTargetMoId;
    CPSS_COM_PID_T    stSrcPid;      /* 源纤程描述符,SMSS保存，在响应消息中回填 */
    BOOL              bIsMainCtrlBrd;/* 是否为全局板 */
} SMSS_SWITCH_INFO_T;

/* 保存和主备态相关的信息 */
typedef struct
{
    BOOL   bMateExist;                  /* 伙伴板是否存在 */
    UINT8  ucMateRStatus;               /* 伙伴板的运行状态 */
    UINT8  ucMateBeatCount;             /* 伙伴板的心跳次数 */
    UINT8  ucSelfAStatus;               /* 本板的主备状态值 */
    STRING szSelfAStatus;               /* 本板的主备状态字符串 */
    CPSS_COM_PHY_ADDR_T stMatePhyAddr;  /* 伙伴板物理地址 */
    SMSS_SWITCH_INFO_T stSwitchInfo;    /* 保存本板相关的倒换信息 */
} SMSS_AS_INFO_T;

/* 状态改变指示消息 */
typedef struct
{
    CPSS_COM_PHY_ADDR_T  stPhyAddr;    /* 状态改变的单板的物理地址 */
    UINT32 ulReason;                   /* 改变原因，见上面状态改变原因宏定义 */
    UINT8  ucRStatus;                  /* 当前运行状态 */
    UINT8  ucAStatus;                  /* 当前主备状态
                                          SMSS_ACTIVE：  主用
                                          SMSS_STANDBY： 备用
                                        */
    UINT8  ucBoardState;               /* 单板状态 */
} SMSS_SYSCTL_STATUS_CHG_IND_MSG_T;

typedef struct
{
    CPSS_COM_PHY_ADDR_T  stPhyAddr;    /* 状态改变的单板的物理地址 */
    UINT8  ucRStatus;                  /* 当前运行状态 */
    UINT8  ucAStatus;                  /* 当前主备状态
                                          SMSS_ACTIVE：  主用
                                          SMSS_STANDBY： 备用
                                        */

    UINT16 usResult;        /* 状态改变处理结果 */                           
} SMSS_SYSCTL_STATUS_CHG_ACK_MSG_T;

/* 外围板发给全局板或者DSP/从CPU 发送给主CPU的严重故障指示消息 */
typedef struct
{
    CPSS_COM_PHY_ADDR_T  stPhyAddr;        /* 本处理器物理地址 */
    UINT32  ulType;                        /* 消息类型，见上面故障上报类型宏定义 */
    UINT32  ulReason;                      /* 见上面故障上报原因宏定义 */
    UINT8   pucFaultInfor[20];             /* 故障数据 */
} SMSS_SYSCTL_BFAULT_IND_MSG_T;
/* 主用全局板发送的强制复位请求消息的结构 */

typedef struct
{
    UINT32  ulSeqId;                   /* 消息序列号 */
    CPSS_COM_PHY_ADDR_T  stPhyAddr;    /* 要复位的物理地址 */   
    UINT32   ulResetType;               /* 复位类型，见上面OAMS发起的复位类型宏定义 */
    UINT32  ulDelayTime;               /* 延迟复位时间（单位：秒） */
    UINT32  ulResetReason;             /* 复位原因,0表示SMSS自己发起的复位,1表示OAMS发起的复位*/
} SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG_T;

/* 主用全局板接收的强制复位响应消息的结构 */
typedef struct
{
    UINT32  ulSeqId;                   /* 消息序列号 */
    CPSS_COM_PHY_ADDR_T  stPhyAddr;    /* 要复位的物理地址 */
    UINT32  ulResetType;               /* 复位类型，见上面OAMS发起的复位类型宏定义 */
    UINT32  ulReturnCode;              /* 返回码，0表示成功，其他失败 */
} SMSS_SYSCTL_FORCE_RST_CMD_RSP_MSG_T;

/* 在全局板上，用于保存OAMS发起的倒换信息，鉴于倒换的随机性，使用链表处理。
   由于该链表结构在gcpa 和 as 文件内都要使用，将其在此处定义。 */
typedef struct _SMSS_CPS__OAMS_FORCE_SWITCH_T
{
     struct _SMSS_CPS__OAMS_FORCE_SWITCH_T *pstPrev; /* 后指针 */
     struct _SMSS_CPS__OAMS_FORCE_SWITCH_T *pstNext; /* 前指针 */
     UINT32 ulSeqId;                            /* OAMS的流水号 */
     UINT32 ulTimerId;                          /* 为该倒换所起的定时器号 */
     CPSS_COM_LOGIC_ADDR_T stLogicAddr;         /* 倒换的单板的逻辑地址 */
     CPSS_COM_PID_T stSrcPid;
} SMSS_CPS__OAMS_FORCE_SWITCH_T;

/* 主从CPU间心跳消息 */
typedef struct
{
    UINT32  ulSeqNum;     /* 心跳序号 */
    UINT32  ulCpuNo;      /* CPU号 */
} SMSS_SYSCTL_HS_HEARTBEAT_IND_T;

/* 主备竞争请求消息结构 */
typedef struct
{
    UINT8 ucCmpData;        /* 主备竞争数据，这里用本板的槽位号 */
} SMSS_ACTIVE_STANDBY_CMP_REQ_MSG_T;

/* 主备竞争应答消息结构 */
typedef struct
{
    UINT8 ucCmpResult;        /* 主备竞争结果: SMSS_ACTIVE:  本板为主
                                               SMSS_STANDBY: 本板为备
                               */
} SMSS_ACTIVE_STANDBY_CMP_RSP_MSG_T;

/* 设置二级交换板是否配置GSSA单板的信息 */
typedef struct
{
    UINT32 ulGtsaFlag;  /*Gtsa配置标志 */
    UINT32 ulGcsaFlag;  /*Gcsa配置标志 */
    UINT32 ulReserved;
} SMSS_CFG_MASA_ABOUT_GSSA_MSG_T;

/* for 调试 */
/* driver 上报的主备状态改变消息 */
typedef struct
{
    UINT8 ucBoardState;      /* 当前主备状态 */
} DRV_BOARD_HA_NOTIFY_MSG_T;

/* 单板在线状态改变消息 */
typedef struct
{
    CPSS_COM_PHY_ADDR_T stPhyAddr;   /* 单板的物理地址 */
    UINT8  ucCurOLStatus;            /* 当前的在线状态:
                                         SMSS_BOARD_ONLINE:在线
                                         SMSS_BOARD_OFFLINE:不在线
                                      */
} SMSS_SYSCTL_BRD_OL_CHG_IND_MSG_T;

/* 复位单板命令消息 */
typedef struct
{
    UINT32 ulReason;    /* 复位原因 */
} SMSS_RESET_BOARD_CMD_MSG_T;

#if defined(SMSS_BOARD_WITH_DSP)
/* 主控通知版本管理模块启动某个DSP */
typedef struct
{
    UINT8 ucDsp;      /* DSP号 */
} SMSS_SYSCTL_DSP_START_CMD_MSG_T;
#endif

#if defined(SMSS_BOARD_WITH_SLAVE) || defined(SMSS_BOARD_WITH_DSP)
/* 版本管理模块通知主控从CPU或某个DSP已启动 */
typedef struct
{
    UINT8 ucDsp;      /* DSP号，仅在DSP板上有效 */
} SMSS_VERM_SLAVE_DSP_START_IND_MSG_T;
#endif

/* Shell调试命令结构 */
typedef struct
{
    STRING        szFuncName;   /* 函数名称 */
    STRING        szFuncHelp;   /* 函数帮助信息 */
    STRING        szArgFmt;     /*参数格式字符串，如果输入NULL则参数格式为缺省方式，即“uuuu*/
    CPS__OAMS_SHCMD_PF pfShcmdFunc;  /* 函数地址 */
} SMSS_SHCMD_INFO_T;

typedef struct
{
  /* CPU占用率阀值下限 */
  UINT32 ulSmssCpuUsageLow;
  /* CPU超过阀值下限的次数 */
  UINT32 ulCurCpuUsageLowCount;

  /* CPU占用率阀值上限 */
  UINT32 ulSmssCpuUsageHigh;
  /* CPU超过阀值上限的次数 */
  UINT32 ulCurCpuUsageHighCount;

  /* CPU占用率当前的状态 */
  UINT32 ulCurSmssCpuUsageState;
} SMSS_CPU_USAGE_INFO_T;

/* 外围板发给全局板或者DSP/从CPU 发送给主CPU的激活纤程事件上报 */

typedef struct
{
    UINT32  ulEventNo;                  /* 事件号 */
    UINT32  ulEventSubNo;               /* 事件细节号 */
    CPSS_COM_PHY_ADDR_T  stPhyAddr;     /* 事件所对应对象的MO_ID */
    UINT32  ulObjId;                    /* 上报激活对象的ID */
    UINT32  ulResult;                   /* 上报激活对象的结果 */
} SMSS_ACT_STOA_RSP_IND_MSG_T;


typedef struct
{
    UINT32  ulType;                        /* 消息类型 包含从向主上报及主向全局单板上报*/
    CPS__OAMS_AM_ORIGIN_EVENT_T stEventReport;  /* 故障数据 */
} SMSS_SYSCTL_EVENT_IND_MSG_T;

#define SMSS_MAX_MSG_COTENT 128
#define SMSS_MSG_COUNT 10
typedef struct 
{
	  struct
    {
        UINT32 SeqNum;  /* 序列号 */
        UINT32 MsgId;   /* 消息号 */
        UINT32 CurTimer; /* 当前时间 */
        UINT32 CurTick;  /* 当前tick */
        UINT32 Len; /* 消息长度 */
        UINT8  ucMsgCotent[SMSS_MAX_MSG_COTENT];/* 消息码流 */
    }astMsgCount[SMSS_MSG_COUNT];
   UINT32 ulFunc_1InCnt;  /* 函数1的入计数 */
   UINT32 ulFunc_1OutCnt; /* 函数1的出计数 */
   UINT32 ulFunc_2InCnt;  /* 函数2的入计数 */
   UINT32 ulFunc_2OutCnt; /* 函数2的出计数 */
   UINT32 ulFunc_3InCnt;  /* 函数3的入计数 */
   UINT32 ulFunc_3OutCnt; /* 函数3的出计数 */
   UINT32 ulFunc_4InCnt;  /* 函数4的入计数 */
   UINT32 ulFunc_4OutCnt; /* 函数4的出计数 */
   UINT32 ulFunc_5InCnt;  /* 函数5的入计数 */
   UINT32 ulFunc_5OutCnt; /* 函数5的出计数 */
   UINT32 ulFunc_6InCnt;  /* 函数6的入计数 */
   UINT32 ulFunc_6OutCnt; /* 函数6的出计数 */
   UINT32 ulFunc_7InCnt;  /* 函数7的入计数 */
   UINT32 ulFunc_7OutCnt; /* 函数7的出计数 */
   UINT32 ulFunc_8InCnt;  /* 函数8的入计数 */
   UINT32 ulFunc_8OutCnt; /* 函数8的出计数 */
   UINT32 ulFunc_9InCnt;  /* 函数9的入计数 */
   UINT32 ulFunc_9OutCnt; /* 函数9的出计数 */
   UINT32 ulFunc_10InCnt;  /* 函数10的入计数 */
   UINT32 ulFunc_10OutCnt; /* 函数10的出计数 */
   
}SMSS_SBBR_MSG_RECORD_T;

typedef struct
{
    UINT32 ulMeID;                         /* MEID: 0-7 */
    UINT32 ulMeState;                      /* ME状态:0 正常，1故障 */
} SMSS_RESET_SLAVE_CMD_MSG_T;

typedef struct 
{
   CPSS_COM_PHY_ADDR_T stPhyAddr;
}SMSS_SYSCTL_LOST_HOSTHB_T;

typedef struct
{
    UINT32 ulAlmNum;                      /* 告警数 */
} SMSS_DEVM_ALMCHG_IND_MSG_T;

#define SMSS_MAX_STRING_LEN  256
typedef struct 
{
    UINT8 ucRouteExist;  /* 路由信息是否存在 0-不存在,1-存在*/
    UINT32 ulIfRouteSwitch; /* 是否需要路由切换功能 0-不需要,1-需要 */
    UINT32 ulPortType;  /* 采用单网口还是双网口配置 0-单网口,1-双网口*/
    UINT8 aucDesIp1[SMSS_MAX_STRING_LEN]; /* 目的IP地址1,用字符串的形式表示IP地址 */
    UINT8 aucMask1[SMSS_MAX_STRING_LEN];  /* 掩码1 */
    UINT8 aucMacAddr[SMSS_MAX_STRING_LEN]; /* MAC地址 */
    UINT8 aucDesIp2[SMSS_MAX_STRING_LEN]; /* 目的IP地址2 */
    UINT8 aucMask2[SMSS_MAX_STRING_LEN];  /* 掩码2 */
    UINT8 aucGateWay1[SMSS_MAX_STRING_LEN];  /* 网关地址1 */
    UINT8 aucGateWay2[SMSS_MAX_STRING_LEN];  /* 网关地址2 */
    UINT8 aucGCPA1[SMSS_MAX_STRING_LEN];  /* GCPA地址1 */
    UINT8 aucGCPA2[SMSS_MAX_STRING_LEN];  /* GCPA地址2 */
}SMSS_ABOX_ROUT_T;

typedef struct 
{
    UINT8 ucExist;
    UINT8 ucDesIp1[SMSS_MAX_STRING_LEN];  /*目的IP地址1,用字符串的形式表示IP地址*/
    UINT8 ucDesIp2[SMSS_MAX_STRING_LEN];  /*目的IP地址2 */
    UINT8 ucGateWay1[SMSS_MAX_STRING_LEN];  /* 网关地址1 */
    UINT8 ucGateWay2[SMSS_MAX_STRING_LEN];  /* 网关地址2 */
    UINT32 ulIp;    /* 通讯网口IP */
    UINT32 ulMask;  /* 子网掩码 */
}SMSS_ABOX_ROUT_TEMP_T;

typedef struct 
{
    CHAR aucSvrIpAddr[SMSS_MAX_STRING_LEN];  /* 服务器IP地址 */
    CHAR aucFileName[SMSS_MAX_STRING_LEN];   /* 文件名 */
}SMSS_VERSION_UPDATE_IND_MSG_T;

/* 通知SYSCTL修改CPU运行状态消息 */
typedef struct  
{
    CPSS_COM_PHY_ADDR_T  stPhyAddr;       /* 物理地址 */
    UINT8  ucRStatus;                     /* CPU的运行状态 */    
}SMSS_CPU_RSTATE_SET_IND_MSG_T;

#define SMSS_MAX_CHILD_BRD_NUM  4
typedef struct
{
    UINT32  ulBrdType;    /* 物理板类型 */
    UINT32  ulBrdVer;     /* 物理板版本 */
} SMSS_PHYBRD_INFO_VER_T;

typedef struct
{
    SMSS_PHYBRD_INFO_VER_T    stMotherBrd;  /* 母板（前插板） */
    SMSS_PHYBRD_INFO_VER_T    stBackBrd;    /* 后插板 */
    UINT32  ulChildBrdNum;     /* 子卡总数 */
    SMSS_PHYBRD_INFO_VER_T  astChildBrd[SMSS_MAX_CHILD_BRD_NUM];/*后插板*/
} SMSS_PHYBRD_VERINFO_T;

typedef struct
{
    CPSS_COM_PHY_ADDR_T  stPhyAddr;       /* 物理地址 */
    CPSS_COM_LOGIC_ADDR_T stLogAddr;      /* 单板逻辑地址 */
    UINT32 ulPhyBrdType;                  /* 物理单板类型 */
    UINT8  ucFunBrdType;                  /* 逻辑单板类型 */
    UINT8  ucAStatus;                     /* 单板的主备状态 */
    UINT8  ucBrdRStatus;                  /* 单板运行状态 */
    UINT8  ucReserved;                    /* 保留字节 */
    SMSS_PHYBRD_VERINFO_T stPhyBoardVer;  /* 单板硬件版本信息 */
}SMSS_BOARD_INFO_QUERY_RSP_MSG_T;

/* SMSS_SYSCTL_BRD_ALARM_SEND_IND */
typedef struct
{
    CPSS_COM_PHY_ADDR_T  stPhyAddr;       /* 物理地址 */
}SMSS_BRD_ALARM_SEND_IND_MSG_T;

/* SMSS_DEVM_SET_NSCA_CLKINFO_MSG */
typedef struct
{
	UINT32 ulSyncInfoLen;  /* 同步信息长度 */
	UINT8  aucBuf[DRV_NSM_CLK_INFO_SYNC_MSG_MAX_LEN];
}SMSS_DEVM_CLKINFOSET_MSG_T;

typedef struct
{
	UINT8  ucIndex;
	CPSS_COM_PHY_ADDR_T stPhyAddr;  /* 本板物理地址 */
}SMSS_FIRMWARE_CHECK_PHYVER_REQ_MSG_T;

typedef struct
{
	UINT8  ucIndex; /* 升级槽索引 */
  UINT16 usPhyBrdType;	/* 物理单板类型 */
}SMSS_FIRMWARE_CHECK_PHYVER_RSP_MSG_T;

#define SMSS_MAX_FILENAME_LEN 100
typedef struct
{
  UINT8  aucFileName[SMSS_MAX_FILENAME_LEN];	/* 版本文件名 */
}SMSS_FIRMWARE_DELETE_IPMC_REQ_MSG_T;

typedef struct
{
  UINT8 ucClkBrdSlot; /* 时钟板槽位号 */
  UINT8 aucRsv[3];	
}SMSS_SYSCTL_UPPERCLK_BRDSLOT_IND_MSG_T;

typedef struct
{
  CPSS_COM_PHY_ADDR_T stPhyAddr; 	/* 本版物理地址 */
}SMSS_SYSCTL_UPPERCLK_QUERY_REQ_MSG_T;

/* 查询失败，不发响应消息 */
typedef struct
{	
  UINT8 ucSlot;  /* 槽位号 */ 	
}SMSS_SYSCTL_UPPERCLK_QUERY_RSP_MSG_T;

typedef struct
{
	CPSS_COM_PHY_ADDR_T  stPhyAddr; /* 本版物理地址 */
}SMSS_SYSCTL_BOARD_STATUS_IND_MSG_T;

typedef struct
{
  UINT8  ucDspIndex;	/* DSP索引号 */
}SMSS_SYSCTL_DEL_DSP_TIMER_MSG_T;


typedef struct
{
  CPSS_COM_PHY_ADDR_T  stPhyAddr; /* 本版物理地址 */	
}SMSS_SYSCTL_FANPARA_REQ_MSG_T;
  
typedef struct
{
  UINT8 ucCpuUsage;	/* cpu利用率 */
}SMSS_SYSCTL_SLAVECPU_IND_MSG_T;

typedef struct
{
	UINT8 ucFrame;
	UINT8 ucShelf;
	UINT8 ucSlot;
	UINT8 ucRsv;
}SMSS_SYSCTL_CHECKPHYVER_REQ_MSG_T;

#if 0
typedef CPS__OAMS_O_ABOX_UPDATE_SWVER_REQ_MSG_T SMSS_ABOX_UPDATE_COMMAND_IND_MSG_T;
typedef CPS__OAMS_O_ABOX_UPDATE_SWVER_IND_MSG_T SMSS_ABOX_UPDATE_RESULT_IND_MSG_T;
#endif

/******************************** 全局变量声明 *******************************/
extern SMSS_CPU_BASE_INFO_T g_stSmssBaseInfo;     /* 保存本CPU的基本信息 */
extern SMSS_AS_INFO_T       g_stSmssAsInfo;       /* 保存本板的主备相关信息 */
extern UINT32 g_ulSmssSendIndex ;                 /* 索引当前纤程 */
extern UINT32 g_ulSmssRspedProcNum ;              /* 保存返回应答的纤程的总数 */
extern UINT8  g_ucSmssIdleStage;                  /* 保存Idle态下的各阶段 */
extern UINT8  g_ucSmssSendCount;                  /* 重发计数器 */
extern UINT8 g_ucBoardState;                      /* 保存本单板状态 */
extern UINT8 g_ucDspAlarmFlag[SMSS_SCPU_DSP_ALARM_SUBNO_SUM][SMSS_DSP_MAX_SUM];
extern UINT8 g_ucSlaveAlarmFlag[SMSS_SCPU_DSP_ALARM_SUBNO_SUM];

/****************************** 外部函数原形声明 *****************************/
extern VOID smss_sysctl_recv_regreq(SMSS_SYSCTL_REG_REQ_MSG_T *pstMsg);
extern VOID smss_sysctl_recv_statuschgind(CPSS_COM_MSG_HEAD_T* pstMsgHead);
extern VOID smss_sysctl_send_gcpa(UINT32 ulPd, UINT32 ulMsgId, UINT8 *pucMsg, UINT32 ulMl);
extern VOID smss_sysctl_send_local(UINT32 ulPd, UINT32 ulMsgId, UINT8 *pucMsg, UINT32 ulMl);
extern VOID smss_sysctl_send_host(UINT32 ulPd, UINT32 ulMsgId, UINT8 *pucMsg, UINT32 ulMl);
extern UINT32 smss_sysctl_set_cps__rdbs_astatus(CPSS_COM_PHY_ADDR_T stPhy, UINT8 ucValue);
extern UINT32 smss_sysctl_set_cps__rdbs_rstatus(CPSS_COM_PHY_ADDR_T stPhy, UINT8 ucValue);
extern INT32 smss_sysctl_query_cps__rdbs_status(CPSS_COM_PHY_ADDR_T pstPhy, UINT8 *pucRstatus, UINT8 *pucMstatus,UINT8 *pucAstatus );
extern VOID smss_sysctl_broadcast(UINT32 ulMsgId, UINT8 *pucMsg, UINT32 ulMsgLen);
extern VOID smss_sysctl_act_stoa_fail(VOID);
extern VOID smss_sysctl_send_act_stoa_req(VOID);
extern VOID smss_sysctl_recv_act_stoa_rsp(CPSS_COM_MSG_HEAD_T *pstMsg);
extern VOID smss_sysctl_state_init_timeout(VOID);
extern VOID smss_sysctl_send_periodly_heartbeat(VOID);
extern VOID smss_sysctl_recv_gp_heartbeat(SMSS_SYSCTL_GP_HEARTBEAT_IND_T *pstMsg);
extern VOID smss_sysctl_recv_statuschgack(CPSS_COM_MSG_HEAD_T *pstMsg);
extern VOID smss_sysctl_send_statuschgind(VOID);
extern INT32 smss_sysctl_query_actresult(VOID);
extern VOID smss_sysctl_gcpa_initboardtable(VOID);
extern VOID smss_sysctl_gcpa_stoa_complete(VOID);
extern INT32 smss_sysctl_mo2logic(SWP_MO_ID_T *pstMo, CPSS_COM_LOGIC_ADDR_T *pstLogic);
extern INT32 smss_sysctl_mo2phy(SWP_MO_ID_T  *pstMoId, CPSS_COM_PHY_ADDR_T *pstPhy);
extern INT32 smss_sysctl_enter_end(BOOL bResetNow,UINT32 ulResetReason);
extern VOID smss_sysctl_brd_reset(UINT32 ulReason);
extern VOID smss_shcmd_reg(SMSS_SHCMD_INFO_T astShcmd[]);

extern INT32 smss_show_board_info(UINT32 ulPhyAddr);
extern VOID smss_send_alarm_to_oams(CPS__OAMS_AM_ORIGIN_ALARM_T * pstAlarm);
extern VOID smss_send_event_to_oams(CPS__OAMS_AM_ORIGIN_EVENT_T * pstEvent);

extern VOID  smss_send_alarm_brdnotinstalled_to_oams(UINT32 ulAlarmSubNo, UINT8 ucCurOLStatus,
                                                     CPSS_COM_PHY_ADDR_T stPhyAddr);
extern VOID  smss_send_alarm_brdnotresponded_to_oams(UINT32 ulAlarmSubNo, UINT32 ulAlarmType, 
                                                     CPSS_COM_PHY_ADDR_T stPhyAddr);
extern VOID  smss_send_alarm_brdfailed_to_oams(SMSS_ACT_STOA_RSP_IND_MSG_T *pstActStoaInd);
extern VOID  smss_send_alarm_cpufailed_to_oams(SMSS_ACT_STOA_RSP_IND_MSG_T *pstActStoaInd);
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_ABOX) 

/* 告警箱启用/禁用状态设置、查询，状态取值：0 - 禁用；1 - 启用 */
extern UINT32 smss_abox_mstate_set(UINT32 ulMState);
extern UINT32 smss_abox_mstate_get(UINT32 *pulMState);

/* 告警箱清音状态设置、查询，状态取值：0 - 清音；1 - 清音解除 */
extern UINT32 smss_abox_mute_set(UINT32 ulMute);
extern UINT32 smss_abox_mute_get(UINT32 *pulMute);

/* 告警箱生产制造信息查询 */
extern UINT32 smss_abox_manu_info_get(SMSS_ABOX_MANU_INFO_T *pstManuInfo);

/* 告警箱软件版本信息查询 */
extern UINT32 smss_abox_ver_info_get(SMSS_ABOX_VER_INFO_T *pstVerInfo);

/* 告警箱复位 */
extern VOID smss_abox_reset(VOID);

/* 告警箱检测 */
extern UINT32 smss_abox_test(VOID);  /* 阻塞方式 */

/* 告警箱声音告警 */
extern UINT32 smss_abox_sound_alarm(UINT32 ulSoundLevel); /* 参数取值：0-3 */

/* 告警箱灯光告警 */
extern UINT32 smss_abox_light_alarm(UINT32 ulCritLevel, UINT32 ulMajorLevel, UINT32 ulMinorLevel, UINT32 ulWarnLevel); /* 各参数取值：0-3 */

#endif

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */
extern INT32 smss_sysctl_gcpa_show_board_info(UINT32 ulPhyAddr);
extern INT32 smss_show_hblist(VOID);
extern INT32 smss_showbrd(void);
extern INT32 smss_sysctrl_set_smss_rstatus(CPSS_COM_PHY_ADDR_T stPhyaddr, UINT8 ucValue);
extern INT32 smss_sysctrl_set_smss_phyver(CPSS_COM_PHY_ADDR_T stPhyaddr, 
                                   SWP_PHYBRD_INTEGRATED_INFO_VER_T stPhyBoardVer);
extern INT32 smss_sysctrl_set_smss_funbrd(CPSS_COM_PHY_ADDR_T stPhyaddr, 
                                   UINT8 ucFuncBoard);
extern UINT8 smss_sysctrl_get_smss_funbrd(CPSS_COM_PHY_ADDR_T stPhyaddr,
                                    SWP_PHYBRD_INTEGRATED_INFO_VER_T *pstBrdInfo);
extern INT32 smss_sysctrl_set_smss_hardveralmflag(CPSS_COM_PHY_ADDR_T stPhyaddr, 
                                           UINT8 ucHardVerAlmFlag);
extern INT32 smss_sysctrl_get_smss_hardveralmflag(CPSS_COM_PHY_ADDR_T stPhyaddr,
                                           UINT8 *pucHardVerAlmFlag);                                         
VOID smss_sysctl_show_gcpa_heartinfo(VOID);                                  
#endif

extern INT32 smss_sysctrl_show_rst_brd_reason(UINT32 ulRstReason);
extern VOID smss_test_PrintMsgStream(UINT32 ulMsgId,UINT8 *pMsgBuf,
                              UINT32 MsgLen,UINT8 ucisSend );
extern INT32 smss_com_send_mate(UINT32 ulDstPd,UINT32 ulMsgId,
                     UINT8 *pucData, UINT32 ulDataLen);
extern INT32 smss_com_send_phy
(
 CPSS_COM_PHY_ADDR_T  stDstPhyAddr,
 UINT32 ulDstPd,
 UINT32 ulMsgId, 
 UINT8 *pucData, 
 UINT32 ulDataLen
 );
extern INT32 smss_com_send 
(
CPSS_COM_PID_T *pstDstPid,
UINT32 ulMsgId,
UINT8 *pucData,
UINT32 ulDataLen
);
extern INT32 smss_showself(VOID);
extern INT32 smss_showmate(VOID);

extern VOID smss_sysctl_set_cps__rdbs_brdstatus(CPSS_COM_PHY_ADDR_T stPhyAdd, UINT8 ucState);
extern INT32 smss_sysctrl_set_smss_boardstate(CPSS_COM_PHY_ADDR_T stPhyaddr, 
                                           UINT8 ucBoardState);
extern VOID smss_sysctl_alarm_sendto_oams(UINT32 ulAlarmNo, 
                                   UINT32 ulAlarmSubNo,
                                   CPS__OAMS_AM_ALARM_TYPE_E enAlarmType, 
                                   UINT32 ulCpuNo, 
                                   UINT32 ulFailObj, 
                                   UINT32 ulResult);
extern  VOID smss_sysctl_oosled_set(UINT8 ucLedState);
void smss_sysctl_lost_gcpahb_handle(void);
VOID smss_sysctl_lost_matehb_handle(VOID);
VOID smss_sysctl_gcpa_lost_cpuhb_handle(SMSS_SYSCTL_LOST_HOSTHB_T *pstMsg);
VOID smss_sysctl_lost_slavehb_handle(VOID);
VOID smss_sysctl_lost_dsphb_handle(SMSS_SYSCTL_LOST_DSPHB_MSG_T *pstMsg);
VOID smss_sysctl_sendbrdalarmind_tosysrt(CPSS_COM_PHY_ADDR_T  stPhyAddr);
VOID smss_sysctl_send_upperclk(UINT8 ucSlot);
VOID smss_sysctl_nsca_stoa_notify(UINT8 ucSlot);
VOID smss_sysctl_set_drv_upperclk(UINT8 ucSlot);
VOID smss_sysctl_write_sbbr(STRING  szFormat, ...);

#if 0 //dhwang added
void cps_reboot_msg_deal(OM_AC_CPU_REBOOT_T* stOmReboot);
#endif
void cps_boot_reboot(CPS_REBOOT_T* stRbt);

/******************************** 头文件保护结尾 *****************************/
#endif /* SMSS_SYSCTL */
/******************************** 头文件结束 *********************************/
