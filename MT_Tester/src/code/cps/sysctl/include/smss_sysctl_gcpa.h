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
#ifndef SMSS_SYSCTL_GCPA_H
#define SMSS_SYSCTL_GCPA_H

/********************************* 头文件包含 ********************************/
#include "cpss_public.h"
#include "cps_replant.h"


/******************************** 宏和常量定义 *******************************/

/******************************** 类型定义 ***********************************/

/* 调试消息结构 */
/*typedef struct
{
  unsigned char ucFrame;
  unsigned char ucShelf;
  unsigned char ucSlot;    
} SMSS_SHOW_BOARD_INFO_MSG_T;*/
/******************************** 全局变量声明 *******************************/

/****************************** 外部函数原形声明 *****************************/
extern VOID smss_sysctl_gcpa_initboardtable(VOID);
extern VOID smss_sysctl_gcpa_recv_regreq(SMSS_SYSCTL_REG_REQ_MSG_T *pstMsg);
extern VOID smss_sysctl_gcpa_recv_gp_heartbeat(SMSS_SYSCTL_GP_HEARTBEAT_IND_T *pstMsg);
extern VOID smss_sysctl_gcpa_recv_bfault(SMSS_SYSCTL_BFAULT_IND_MSG_T *pstMsg);
extern VOID smss_sysctl_gcpa_send_heartbeat(VOID);
extern VOID smss_sysctl_idle_gcpa_init(VOID);
extern VOID smss_sysctl_gcpa_free_source(VOID);
extern VOID smss_sysctl_recv_cps__oams_force_reset(CPS__OAMS_DYND_FORCE_RST_REQ_MSG_T *pstMsg);
extern VOID smss_sysctl_recv_cps__rdbs_mstatus_chg(CPS__RDBS_DEV_CPUMSTATUSCHG_IND_MSG_T *pstMsg);
extern VOID smss_sysctl_recv_oamsforceho(CPS__OAMS_DYND_FORCE_HO_REQ_MSG_T *pstMsg);
extern UINT32 smss_sysctl_gcpa_recv_statuschgind(SMSS_SYSCTL_STATUS_CHG_IND_MSG_T *pstMsgHead);
extern BOOL smss_sysctl_normal_gcpa_func(CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern INT32 smss_sysctrl_set_smss_rstatus(CPSS_COM_PHY_ADDR_T stPhyaddr, UINT8 ucValue);
extern INT32 smss_sysctrl_set_smss_astatus(CPSS_COM_PHY_ADDR_T stPhyaddr, UINT8 ucValue);
extern INT32 smss_sysctrl_set_smss_mstatus(CPSS_COM_PHY_ADDR_T stPhyaddr, UINT8 ucValue);
extern INT32 smss_sysctrl_get_brd_state_byphy(CPSS_COM_PHY_ADDR_T  stPhyaddr,
                                                   UINT8 *pucAStatus,
                                                   UINT8 *pucRStatus,
                                                   UINT8 *pucMStatus);
extern VOID smss_sysctl_show_gcpa_heartinfo(VOID);
extern VOID smss_sysctl_update_board_info(CPSS_COM_PHY_ADDR_T stPhyAddr);

/******************************** 头文件保护结尾 *****************************/
#endif /* SMSS_SYSCTL_GCPA */
/******************************** 头文件结束 *********************************/
