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
#ifndef CPSS_DBG_PRINT_H
#define CPSS_DBG_PRINT_H
/********************************* 头文件包含 ********************************/
#include "cpss_dbg.h"

/******************************** 宏和常量定义 *******************************/
#ifdef CPSS_DSP_CPU
#define CPSS_PRINT_BUF_LEN        (8*1024)
#else
#define CPSS_PRINT_BUF_LEN        (4096*1024)         /* 打印缓存长 */
#endif
#define CPSS_PRINT_TIMER_NO       CPSS_TIMER_00      /* 打印定时器 */
#define CPSS_PRINT_TIMER_MSG      CPSS_TIMER_00_MSG  /* 打印定时器超时消息 */
#define CPSS_PRINT_LEN_FOR_PRTINFO (128)             /* 打印级别为CPSS_PRINT_INFO
                                                        时限制其长度为128 */
#define CPSS_PRINT_ON             ((UINT8)(0x01))    /* 打印开关开*/

/******************************** 类型定义 ***********************************/

#define CPSS_MAX_MODULE_NUM 256
#define CPSS_MAX_SUBSYS_NUM 256

/* 各个模块的打印级别 */
typedef struct tagCPSS_PRINT_LEVEL
{
    UINT8    ucPrintTag;                               /* 该子系统的打印开关 */
    UINT8    ucPrintLevel[CPSS_MAX_MODULE_NUM];         /* 各个模块的打印级别 */
}CPSS_PRINT_LEVEL_T;

/* 打印管理信息 */
typedef struct tagCPSS_PRINT_MANAGE
{
    UINT32 ulBoardTag;           /* 标志整板是否需要打印 */
    UINT32 ulPrtID;              /* 打印流水号 */
    UINT32 ulPrintID;            /* 初始打印标识为0 */
    UINT8  *pucPrtBufHead;       /* 指向打印缓冲区头指针 */
    UINT8  *pucPrtDataHead;      /* 打印缓冲队列头指针，为NULL表示队列为空 */
    UINT8  *pucPrtDataTail;      /* 打印缓冲队列尾指针，指向队列中最后一包数据 */
    UINT8  *pucPrtDataAdd;       /* 打印缓冲队列加入数据的位置指针，在g_pucPrtDataTail后 */
    INT32  lPrtTimerID;          /* 打印流控定时器是否有效 */
    UINT32 ulPrtTimerPeriod;     /* 打印流控定时器周期 */
    UINT32 ulPrtToken;           /* 打印令牌数 */
    UINT32 ulPrtTokenValid;      /* 当前持有的有效令牌数 */
    UINT32 ulPrtInfoLostNum;     /* 丢失的打印信息计数 */
    UINT32 ulSemForPrtLost;      /* 丢失打印信息计数信号量 */
    CPSS_COM_PID_T stPrtDstPid;            /* 打印上报PID */
    BOOL bPrtSendtoMate;                   /* 打印发送到伙伴板标志 */
    CPSS_PRINT_LEVEL_T astPrintLevel[CPSS_MAX_SUBSYS_NUM];/* 用数组存放各子系统的信息 */
}CPSS_PRINT_MANAGE_T;

typedef struct tagCPSS_PRINT_INFO
{
    CPSS_PRINT_ENTRY_T stPrtEntry; /* 打印每条信息结构 */
    INT8 cBuf[CPSS_PRINT_PACKET_LEN];/*[CPSS_MAX_PRINT_LEN];*/ /* 打印信息体 */
}CPSS_PRINT_INFO_T;

#if defined CPSS_DBG_REC_TIME_ON

/*==== 最大记录数据的长度 ====*/
#define CPSS_DBG_REC_TIME_MAX_DATA_BUF_LEN 16
/*==== 最大记录记录项的个数 ====*/
#define CPSS_DBG_REC_TIME_MAX_RECID_SUM 1024
/*==== 一个记录项默认的记录个数，用户不指定的情况下使用该值 ====*/
#define CPSS_DBG_REC_TIME_DEF_REC_SUM 1024
/*==== 一个记录项名称的最大字符串个数 ====*/
#define CPSS_DBG_REC_TIME_MAX_NAME_LEN 64
/*==== 无效的记录项编号 ====*/
#define CPSS_DBG_REC_TIME_INVALID_RECID ((UINT32)(-1))
/*==== 一个记录的存储信息 ====*/
typedef struct structCpssDbgTimeRecItem{
    UINT32 ulStartTime;
    UINT32 ulEndTime;
    UINT32 ulPara;
    UINT32 ulDataLen;
    UINT32 ulResult;
    CPSS_TIME_T tAbsTime;
    UINT8 aucDataBuf[CPSS_DBG_REC_TIME_MAX_DATA_BUF_LEN];
}CPSS_DBG_TIME_REC_ITEM_T;

/*==== 一个记录项的存储信息，即需要打点的项目 ====*/
typedef struct structCpssDbgRecTimeHead{
    UINT32 ulSemId;
    UINT32 ulMaxRecSum;
    UINT32 ulIsNeedLock;
    UINT32 ulCurPosition;
    CPSS_DBG_TIME_REC_ITEM_T *pRecInfo;
    UINT8 aucRecName[CPSS_DBG_REC_TIME_MAX_NAME_LEN];
}CPSS_DBG_REC_TIME_HEAD_T;


#endif /* endof defined CPSS_DBG_REC_TIME_ON */

/******************************** 全局变量声明 *******************************/
/*extern */
/******************************** 外部函数原形声明 ***************************/
extern INT32 cpss_dbg_g_stCpssDbgPrtManage_get();
extern INT32 cpss_dbg_g_bCpssDbgPrtValid_get();
extern INT32 cpss_dbg_g_pstCpssDbgPrtDstPid_get();

/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_DBG_PRINT_H */
/******************************** 头文件结束 *********************************/
