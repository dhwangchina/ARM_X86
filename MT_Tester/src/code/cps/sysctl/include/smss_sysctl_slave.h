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
#ifndef SMSS_SYSCTL_SLAVE_H
#define SMSS_SYSCTL_SLAVE_H

/********************************* 头文件包含 ********************************/
#include "cpss_public.h"
#include "smss_sysctl.h"
/******************************** 宏和常量定义 *******************************/

/* 主CPU号为1，从CPU号为2，DSP号从5开始*/
#define DSP_CPUNO_BASE  5

/* 主CPU属性宏定义 */
#ifdef  SMSS_BOARD_WITH_DSP
#define SMSSDSPNUM   12
#endif

/*索引与DSP号或从CPU号的转换的宏定义*/
#ifdef SMSS_BOARD_WITH_DSP
#define INDEX2CPUNO(i)  ((i) + 5)  /* 由索引获得DSP编号（从5开始）*/
#define CPUNO2INDEX(i)  ((i) - 5)  /* 由DSP编号获得其索引值*/
#else
#define INDEX2CPUNO(i)  2
#define CPUNO2INDEX(i)  0
#endif

/******************************** 类型定义 ***********************************/
/* 
* 外围板的主CPU上保存的从CPU相关信息，带从CPU的主CPU上要定义该结构的
  一个数组来保存所有从CPU的信息。*/
typedef struct
{
    UINT32  ulTimerId;    /* 等待该DSP或者从CPU启动的定时器 */
    UINT32  ulRStatus;    /* 运行状态 */    
    UINT32  ulMStatus;    /* 管理状态 */    
    UINT32  ulBeatCount;  /* 心跳次数 */
    UINT32  ulCpuNo;      /* CPU号 */
} SMSS_SCPU_INFO_T;

/******************************** 全局变量声明 *******************************/

/****************************** 外部函数原形声明 *****************************/
VOID smss_sysctl_host_recv_regreq(SMSS_SYSCTL_REG_REQ_MSG_T *pstMsg);
VOID smss_sysctl_idle_slave_init(VOID);
VOID smss_sysctl_host_recv_statuschgind(SMSS_SYSCTL_STATUS_CHG_IND_MSG_T *pstMsg);
VOID smss_sysctl_host_block_dsp(CPS__RDBS_DEV_CPUMSTATUSCHG_IND_MSG_T *pstMsg);
VOID smss_sysctl_host_recv_slave_bfault(SMSS_SYSCTL_BFAULT_IND_MSG_T *pstMsg);
VOID smss_sysctl_host_recv_dsp_bfault(SMSS_SYSCTL_BFAULT_IND_MSG_T *pstMsg);
VOID smss_sysctl_host_heartbeat_slave(VOID);
VOID smss_sysctl_host_heartbeat_dsp();
BOOL smss_sysctl_normal_slave_func(CPSS_COM_MSG_HEAD_T *pstMsg);
VOID smss_sysctl_slave_rstate_send(CPSS_COM_PHY_ADDR_T stPhyAddr);
/******************************** 头文件保护结尾 *****************************/
#endif /* SMSS_SYSCTL_SLAVE */
/******************************** 头文件结束 *********************************/


