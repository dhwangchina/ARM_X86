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
#ifndef CPSS_TM_SNTP_H
#define CPSS_TM_SNTP_H

/********************************* 头文件包含 ********************************/
#include "cpss_type.h"

/******************************** 宏和常量定义 *******************************/
 

/* sntp server 地址长度*/
#define CPSS_TM_SNTP_SERVER_IPADDR_LEN                         (20)

/* 等待server响应的延迟*/
#define CPSS_TM_SNTP_DELAY_TICKS                               (120)

/* 全局板主动更新外围板的时间消息*/
#define CPSS_TM_SNTP_BROADCAST_TIME_MSG               0x73300001

/* 全局板主动更新外围板的cfg 消息*/
#define CPSS_TM_SNTP_OTHERS_UPDATE_CFG_MSG                0x73300002

/*外围板在初始状态发送的请求配置消息*/ 
#define CPSS_TM_SNTP_OTHERS_CFG_REQ_MSG                     0x73300003

/* 配置响应消息*/
#define CPSS_TM_SNTP_OTHERS_CFG_RESP_MSG                    0x73300004

/* DSP或从cpu发往host的请求消息*/
#define CPSS_TM_SNTP_DSP_REQ_TIME_MSG                          0x73300005

/* 初始请求周期定时器时长( ms)*/
#define CPSS_TM_SNTP_INIT_REQ_TIMER_DELAY                    (2000)  

#define CPSS_TM_SNTP_INIT_REQ_TIMER_NO                         (CPSS_TIMER_01)

#define CPSS_TM_SNTP_INIT_REQ_TIMER_EXPIRE_MSG           (CPSS_TIMER_01_MSG)

/* 和 SNTP 同步周期定时器*/
#define CPSS_TM_SNTP_SYN_TIMER_NO                                (CPSS_TIMER_02)

#define CPSS_TM_SNTP_SYN_TIMER_EXPIRE_MSG                 (CPSS_TIMER_02_MSG)

/* 外围请求配置定时器*/
#define CPSS_TM_SNTP_REQ_CFG_TIMER_NO                                (CPSS_TIMER_03)

#define CPSS_TM_SNTP_REQ_CFG_TIMER_EXPIRE_MSG                 (CPSS_TIMER_03_MSG)

#define CPSS_TM_SNTP_REQ_CFG_TIMER_DELAY                            (1000)

/* DSP OR CPU 发往HOST的请求消息*/
#define CPSS_TM_SNTP_UNHOST_REQ_TIME_MSG                         0x73300005

#define CPSS_TM_SNTP_UNHOST_RSP_TIME_MSG                         0x73300006

/******************************** 类型定义 ***********************************/
#if 0
#ifdef CPSS_DSP_CPU               /* 2006/08/23 李军修改*/
struct timespec
{
   UINT32 tv_sec;
   UINT32 tv_nsec;
};
#endif
#endif

/* gcpa sntp 配置信息*/
typedef struct tagSNTP_GCPA_CFG_INFO
{ 
   CHAR    aucServerIp[CPSS_TM_SNTP_SERVER_IPADDR_LEN];           /*SNTP server地址*/
   UINT16   usSynPeriod;                                   /* 和 SNTP SERVER的同步周期,单位秒*/
   UINT16  usDiff;                                        /* 全局板和OMC的误差门限,单位ms*/  
   UINT16  usOtherSynPeriod;                              /* 外围板同步周期*/
   UINT8   ucBrdState;                                   /* 单板主备状态*/
    
}CPSS_TM_SNTP_GCPA_CFG_INFO_T;

/* 外围板配置信息*/
typedef struct tagSNTP_OTHERS_SYN_CFG
{ 
   UINT16 usCurrSynPeriod;                              /* 当前和gcpa的同步周期,单位秒*/
   UINT16 usNewSynPeriod;                               /* 新的同步周期,单位秒*/
 
}CPSS_TM_SNTP_OTHERS_SYN_CFG_T;

/* othes cfg */
typedef struct tagSNTP_OTHERS_CFG_NOTIFY
{
  UINT8 ucGmtOffset;
  UINT8 ucAlarmThreshold;
  UINT16 usSynPeriod;

}CPSS_TM_SNTP_OTHERS_CFG_NOTIFY_T;

/* 刷新dsp和从cpu的时间结构*/
typedef struct tagSNTP_CPSS_UPDATE_TIME
{
   UINT32 ulSec;
   UINT32 ulNsec;
}CPSS_TM_SNTP_UPDATE_TIME_T;

#define	SNTP_INIT_STATE	CPSS_VK_PROC_USER_STATE_IDLE
#define	SNTP_NORMAL_STATE	1

/* 定时器相关宏定义 */
#define CPSS_SNTP_LOOP_TIMER        CPSS_TIMER_00      /* 重发定时器号 */
#define CPSS_SNTP_LOOP_TIMER_MSG    CPSS_TIMER_00_MSG  /* 重发定时器超时消息 */
#define SMSS_VERM_RESEND_TIMEOUT      2000               /* 重发定时器时长 */

/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/
extern INT32 cpss_timer_set_local_clock(UINT32  ulSecond, UINT16  usMilliSec);
extern UINT32 cpss_timer_diff(UINT32 ulNewSec,UINT16 usNewMilliSec);

#endif


