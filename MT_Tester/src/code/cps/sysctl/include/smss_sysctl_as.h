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
#ifndef SMSS_SYSCTL_AS_H
#define SMSS_SYSCTL_AS_H

/********************************* 头文件包含 ********************************/
#include "cpss_public.h"

/******************************** 宏和常量定义 *******************************/
/* 定义倒换结果 */
#define SMSS_BSWITCH_OK           70  /* 倒换成功 */
#define SMSS_BSWITCH_FAIL         71  /* 倒换失败 */
/* 定义倒换原因 */
#define SMSS_BSWITCH_DEVM_ALARM     90 /* DEVM上报故障 */
#define SMSS_BSWITCH_CPS__OAMS_FORCE     91 /* OAMS强制倒换 */
#define SMSS_BSWITCH_DRV_BUTTON     92 /*前面板按钮发起的倒换 */
#define SMSS_BSWITCH_DRV_REPORT     93 /*DRIVER 发起的主备状态改变 */
#define SMSS_BSWITCH_MATE_FAULT     94 /* 伙伴板故障发起的倒换 */

/* 处理倒换禁止的纤程个数 */
#define SMSS_MAX_FORBID_NUM         20
/******************************** 类型定义 ***********************************/
/* 1+1主备板间心跳消息 */
typedef struct
{
    UINT32  ulSeqNum;             /* 心跳序号 */
    UINT8  ucRStatus;             /* 自己的运行状态 */
} SMSS_SYSCTL_AS_HEARTBEAT_IND_T;

/* 倒换完成指示消息 */
typedef struct
{
    UINT32 ulResult;      /* SMSS_OK表示倒换成功，其他表示错误码 */
    UINT32 ulReason;      /* 倒换原因 见上面宏定义 */
    CPSS_COM_PHY_ADDR_T stPhyAddr;  /* 本板物理地址 */
} SMSS_SYSCTL_BSWITCH_IND_MSG_T;

/* 主用板向备用板发的倒换请求消息 */
typedef struct
{
    UINT32 ulReason;   /* 倒换原因，见上面宏定义 */
    UINT32 ulSeqId;    /* 倒换序列号，仅OAMS发起的全局板主备倒换使用 */
    CPSS_COM_PID_T    stSrcPid;      /* 源纤程描述符,SMSS保存，在响应消息中回填 */
    BOOL              bIsMainCtrlBrd;/* 是否为全局板 */
} SMSS_SYSCTL_BSWITCH_REQ_MSG_T;

/* 备用板向主用板发送的倒换应答消息结构 */
typedef struct
{
    UINT32 ulResult;      /* SMSS_OK表示可以倒换，其他表示错误码 */
} SMSS_SYSCTL_BSWITCH_RSP_MSG_T;
/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/
extern VOID smss_sysctl_state_stoa_func(CPSS_COM_MSG_HEAD_T *pstMsg);
extern VOID smss_sysctl_stoa_complete(VOID);
extern VOID smss_sysctl_as_start_mate_heartbeat(VOID);
extern VOID smss_sysctl_idle_mate_init(VOID);
extern VOID smss_sysctl_as_evoked_atos(VOID);
extern BOOL smss_mate_in_service(VOID);
extern BOOL smss_sysctl_normal_as_func(CPSS_COM_MSG_HEAD_T *pstMsg);
extern BOOL smss_sysctl_init_as_func(CPSS_COM_MSG_HEAD_T *pstMsg);
extern BOOL smss_sysctl_idle_as_func(CPSS_COM_MSG_HEAD_T *pstMsg);
/******************************** 头文件保护结尾 *****************************/
#endif /* SMSS_SYSCTL_AS_H */
/******************************** 头文件结束 *********************************/
