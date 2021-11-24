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
#ifndef CPSS_TM_TIMER_H
#define CPSS_TM_TIMER_H

/********************************* 头文件包含 ********************************/
#include "cpss_common.h"
#include "cpss_vk_proc.h"

/******************************** 宏和常量定义 *******************************/
#ifdef CPSS_DSP_CPU
#define CLOCK_RATE              1000
#else
#define CLOCK_RATE              100
#endif
/* 定时器的类型 */
#define  LOOP_TIMER             3      /* 循环定时器 */
#define  SYN_TIMER              1      /* 同步定时器 */
#define  RELATIVE_TIMER         2      /* 普通相对定时器 */
#define  ABS_TIMER              4      /* 绝对定时器 */
#define  NULL_PARA       (UINT32)(0xFFFFFFFF)     /* 2006/05/19 李军修改*/
#define  LEOF            (UINT32)(0xFFFFFFFF)

/******************************** 类型定义 ***********************************/
/* 空闲定时器控制块管理结构 */
typedef struct tagCPSS_TM_TMCBPOOLITEM
{ 
    UINT32     ulFreeCount ;                    /* 空闲定时器控制块个数 */
    UINT32     ulHead ;                         /* 空闲定时器控制块数组的头索引 */
    UINT32     ulTail ;                         /* 空闲定时器控制块数组的尾索引 */
    UINT32     aulElement[MAX_TIMER_NUM] ;      /* 存放所有定时器标识的数组 */
}CPSS_TM_TMCBPOOLITEM_T ;

/* 绝对定时器队列管理结构 */
typedef struct tagCPSS_TM_ABSTIMERQUEUE
{
    UINT32    ulElementCount ;                  /* 该队列上所有定时器数目 */    
    UINT32    ulHead;                           /* 该队列中第一个TMCB索引 */        
    UINT32    ulTail;                           /* 该队列中最后一个TMCB索引 */
}CPSS_TM_ABSTIMERQUEUE_T ;

/* 相对定时器队列管理结构 */
typedef struct tagCPSS_TM_RELTIMERQUEUE
{
    UINT32    ulElementCount;                   /* 所有相对定时器数目 */
    UINT32    ulCursorForTick;                  /* 当前游标所在的tick数组索引 */
    UINT32    ulCursorForTid;                   /* 当前游标所在的tid数组索引 */
    UINT32    aulTickArray[MAX_TIMER_NUM];      /* tick数组，存放每个队列的首tid */
    UINT8    aucTidArray[MAX_TIMER_NUM];        /* tid数组，确定定时器是否设置 */
}CPSS_TM_RELTIMERQUEUE_T ;

/*定时器资源统计信息结构：*/
typedef struct tagCPSS_TM_TIMERINFO
{
    UINT32   ulMaxTMCBCount;              /* 最大定时器数目 */
    UINT32   ulFreeTMCBCount;             /* 空闲定时器块数 */
    UINT32   ulTimerPeerCount;            /* 定时器使用峰值 */
    UINT32   ulSuccessCount;              /* 定时器申请成功次数 */
    UINT32   ulFailureCount;              /* 定时器申请失败次数 */
}CPSS_TM_TIMERINFO_T;

/* 定时器控制块 */
typedef struct tagCPSS_TM_TMCB
{
    UINT8        bIsInUse;                  /* 使用标志 */
    UINT8        bIsTimeOut;                /* 超时标志 */
    UINT8        ucTimerType;               /* 定时器类型 */
    UINT8        ucTimerNo ;                /* 定时器号 */
    UINT32       ulPno;                     /* 设置该定时器的进程号 */
    UINT32       ulOriginalCount;           /* 长时循环定时器的原始时长 */
    UINT32       ulCount ;                  /* 定时长，以tick为单位*/
    UINT32       ulTimerParam;              /* 定时器参数 */
    UINT32       ulTickIndex;               /* 该定时器在tick[]数组中的下标     */ 
    UINT32       ulPrev ;                   /* 同tick队列中的上一个TMCB标号    */
    UINT32       ulNext ;                   /* 同tick队兄械南乱桓鲶MCB标号    */
    UINT32       ulPrevParamTid;            /* 同进程设置的上一个带参数TMCB标号 */
    UINT32       ulNextParamTid;            /* 同进程设置的下一个带参数TMCB标号 */
    UINT32       ulLeftChildTid;            /* 相同进程设置的不同参数TMCB标号AVL树的左孩子*/
    UINT32       ulRightChildTid;           /* 相同进程设置的不同参数TMCB标号AVL树的右孩子 */
    UINT32       ulNextTmrNoTid;            /* 在平衡二杈树中，同进程同参数不同定时器号形成的队列的链接 */
    INT16        sBalancefactor;            /* 平衡二叉树中的平衡因*/
    UINT8        ucReserved1;               /* 保留字*/
    UINT8        ucReserved2;               /* 保留字*/
    UINT32       ulMsgHeadAddr;             /* 该定时器超时消息首地址*/
    UINT32       ulPcb;
}CPSS_TM_TMCB_T ;

typedef struct tagCPSS_TM_TICK
{
    UINT32  ulTickCountHigh;        /* 倍数 */
    UINT32  ulTickCountLow;         /* 当前tick数MOD（2的32次方－1）之值 */
}CPSS_TM_TICK_T;                    /* 该结构用于存放自开机时的TICK数 */
  
typedef struct tagCPSS_TM_SYSSOFTCLOCK
{
    UINT16   usSysYear;         /* 年 */
    UINT8    ucSysMon;          /* 月 */
    UINT8    ucSysDay;          /* 日 */
    
    UINT8    ucSysHour;         /* 时 */
    UINT8    ucPading0;         /* 填充字*/
    UINT8    ucSysMin;          /* 分 */
    UINT8    ucSysSec;          /* 秒 */
    
    UINT16   usMilliSec;        /* 毫秒 */
    UINT8    ucSysWeek;         /* 周 */
    UINT8    ucPading1;         /* 填充字*/
}CPSS_TM_SYSSOFTCLOCK_T;        /* 系统软时钟结构，表示绝对时间 */

typedef struct tagCPSS_TM_TIMER
{
    UINT32  ulSecond;           /* 秒 */
    UINT16  usMilliSec;         /* 毫秒 */
    UINT8   ucPad[2];
}CPSS_TM_TIMER_T;               /* 相对时间结构，以X秒Y毫秒的形式表示 */

typedef struct tagCPSS_TM_COREDATA
{   
      UINT32                    ulSecFromPowerOn;
      CPSS_TM_TICK_T            stTickCountsFromPowerOn;
      CPSS_TM_TIMER_T           stTimeFrom2K;              
      CPSS_TM_TMCB_T            astTMCB[MAX_TIMER_NUM];
      CPSS_TM_TMCBPOOLITEM_T    stTMCBPool;
      CPSS_TM_ABSTIMERQUEUE_T   stAbsTimerQueue;
      CPSS_TM_RELTIMERQUEUE_T   stRelTimerQueue;
      UINT32          ulMutexSemForTMCB;
      UINT32          ulMutexSemForTimerQueue;
      UINT32          ulSynSemForTick;
      UINT32          ulMutexSemForTickCounts;
      UINT32          ulMutexSemForSysSoftClock;  
}CPSS_TM_COREDATA_T;

typedef struct tagCPSS_TM_TIMER_MSG_EXTEND
{
        UINT32  ulPara;         /* 定时器参数 */
        UINT32  ulTimerId;      /* 定时器标识 */
        UINT8   ucTimerType;    /* 定时器类型 */        
}CPSS_TM_TIMER_MSG_EXTEND_T;

#ifndef CPSS_DSP_CPU
typedef INT(*TFuncPtr)();
#endif

/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/

extern INT32 cpss_timer_init();  
extern VOID cpss_timer_scan_task();
extern UINT32 cpss_timer_get_cur_seconds(VOID);
extern INT32 cpss_timer_show(VOID);
extern INT32 cpss_timer_show_proc(UINT32 ulPno);
extern INT32 cpss_timer_scan_task_reg();
/******************************** 头文件保护结尾 *****************************/
#endif /* 文件名_H */
/******************************** 头文件结束 *********************************/

