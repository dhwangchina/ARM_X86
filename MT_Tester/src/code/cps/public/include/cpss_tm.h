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
#ifndef CPSS_TM_H
#define CPSS_TM_H

/********************************* 头文件包含 ********************************/
#include "cpss_type.h"
/******************************** 宏和常量定义 *******************************/

/* 定时器号 */
#define CPSS_TIMER_00  ((UINT8)(0x00))   /* 纤程内定时器：00号 */
#define CPSS_TIMER_01  ((UINT8)(0x01))   /* 纤程内定时器：01号 */
#define CPSS_TIMER_02  ((UINT8)(0x02))   /* 纤程内定时器：02号 */
#define CPSS_TIMER_03  ((UINT8)(0x03))   /* 纤程内定时器：03号 */
#define CPSS_TIMER_04  ((UINT8)(0x04))   /* 纤程内定时器：04号 */
#define CPSS_TIMER_05  ((UINT8)(0x05))   /* 纤程内定时器：05号 */
#define CPSS_TIMER_06  ((UINT8)(0x06))   /* 纤程内定时器：06号 */
#define CPSS_TIMER_07  ((UINT8)(0x07))   /* 纤程内定时器：07号 */
#define CPSS_TIMER_08  ((UINT8)(0x08))   /* 纤程内定时器：08号 */
#define CPSS_TIMER_09  ((UINT8)(0x09))   /* 纤程内定时器：09号 */
#define CPSS_TIMER_10  ((UINT8)(0x0a))   /* 纤程内定时器：10号 */
#define CPSS_TIMER_11  ((UINT8)(0x0b))   /* 纤程内定时器：11号 */
#define CPSS_TIMER_12  ((UINT8)(0x0c))   /* 纤程内定时器：12号 */
#define CPSS_TIMER_13  ((UINT8)(0x0d))   /* 纤程内定时器：13号 */
#define CPSS_TIMER_14  ((UINT8)(0x0e))   /* 纤程内定时器：14号 */
#define CPSS_TIMER_15  ((UINT8)(0x0f))   /* 纤程内定时器：15号 */
#define CPSS_TIMER_16  ((UINT8)(0x10))   /* 纤程内定时器：16号 */
#define CPSS_TIMER_17  ((UINT8)(0x11))   /* 纤程内定时器：17号 */
#define CPSS_TIMER_18  ((UINT8)(0x12))   /* 纤程内定时器：18号 */
#define CPSS_TIMER_19  ((UINT8)(0x13))   /* 纤程内定时器：19号 */
#define CPSS_TIMER_20  ((UINT8)(0x14))   /* 纤程内定时器：20号 */
#define CPSS_TIMER_21  ((UINT8)(0x15))   /* 纤程内定时器：21号 */
#define CPSS_TIMER_22  ((UINT8)(0x16))   /* 纤程内定时器：22号 */
#define CPSS_TIMER_23  ((UINT8)(0x17))   /* 纤程内定时器：23号 */
#define CPSS_TIMER_24  ((UINT8)(0x18))   /* 纤程内定时器：24号 */
#define CPSS_TIMER_25  ((UINT8)(0x19))   /* 纤程内定时器：25号 */
#define CPSS_TIMER_26  ((UINT8)(0x1a))   /* 纤程内定时器：26号 */
#define CPSS_TIMER_27  ((UINT8)(0x1b))   /* 纤程内定时器：27号 */
#define CPSS_TIMER_28  ((UINT8)(0x1c))   /* 纤程内定时器：28号 */
#define CPSS_TIMER_29  ((UINT8)(0x1d))   /* 纤程内定时器：29号 */
#define CPSS_TIMER_30  ((UINT8)(0x1e))   /* 纤程内定时器：30号 */
#define CPSS_TIMER_31  ((UINT8)(0x1f))   /* 纤程内定时器：31号 */

#define CPSS_TD_INVALID  ((UINT32)(0xFFFFFFFF))   /* 无效定时器描述符 */

/******************************** 类型定义 ***********************************/

/* 时间 */
typedef struct tagCPSS_TIME_T 
{
    UINT16 usYear ;      /* 年 [xxxx],例如2006*/
    UINT8  ucMonth ;     /* 月 [1..12]*/
    UINT8  ucDay;        /* 日 [1..31]*/
    UINT8  ucHour;       /* 时 [0..23]*/
    UINT8  ucMinute;     /* 分 [0..59]*/
    UINT8  ucSecond;     /* 秒 [0..59]*/
    UINT8  ucWeek;       /* 星期[0..6],星期日为0；当设置时间时，忽略星期信息 */
} CPSS_TIME_T;

/* 定时器消息体结构 
 * 如果参数为0，则是无参定时器的超时消息；
 * 参数不为0， 是有参定时器的超时消息；
 * 所以，有参定时器的参数不可以为0*/
typedef struct tagCPSS_TIMER_MSG 
{
    UINT32 ulPara;     /* 定时器参数，对于不带参定时器，值为0 */
} CPSS_TIMER_MSG_T;

/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/
/* 无参定时器类 */
extern INT32 cpss_timer_set(UINT8 ucTimerNo,INT32 lDelayLength);
extern INT32 cpss_timer_loop_set(UINT8 ucTimerNo, INT32 lDelayLength);
extern INT32 cpss_timer_abs_set(UINT8 ucTimerNo,  CPSS_TIME_T *pstClock);
extern INT32 cpss_timer_delete(UINT8 ucTimerNo);                                                      /*释放定时器*/
/* 有参定时器类 */                                               
extern UINT32 cpss_timer_para_set(UINT8 ucTimerNo,INT32 lDelayLength,UINT32 ulPara);       /*设置有参定时器*/  /** 统一平台 **/   
extern UINT32 cpss_timer_para_loop_set(UINT8 ucTimerNo,INT32 lDelayLength,UINT32 ulPara);
extern UINT32 cpss_timer_para_abs_set(UINT8 ucTimerNo,CPSS_TIME_T *pstClock,UINT32 ulPara);
extern INT32  cpss_timer_para_delete(UINT32 ulTd);
/* 时间类 */
extern UINT32 cpss_gmt_get(VOID);
extern UINT32 cpss_tick_get(VOID);
extern VOID cpss_clock_get(CPSS_TIME_T *pstClock);
extern VOID cpss_clock_set(CPSS_TIME_T *pstClock);
extern INT32 cpss_offset_time_get(const CPSS_TIME_T *pstTimeStart,INT32 ulOffset,CPSS_TIME_T *pstTime);
extern INT32 cpss_clock2gmt(  CPSS_TIME_T *pstClock,  UINT32 *pulGmtSeconds);
extern INT32 cpss_gmt2clock(  UINT32 ulGmtSeconds,   CPSS_TIME_T *pstClock );
extern VOID cpss_timer_scan_task();
extern UINT32 cpss_high_precision_counter_get(VOID);
extern UINT32 cpss_microsecond_counter_get (VOID);

#define INNER_CLK       1
#define OUTER_CLK       0

#define CLK_SRC INNER_CLK
#define CLK_RATE        800000000 /* Hz */
#define CLK_DIV 8                               
#define TICKS_PER_SEC   1000 /* ticks per second */

extern volatile int    g_lIsrCondVar;
extern volatile int    g_lIsrSysTicks;

extern void     timer0_init (void);
extern INT32 cpss_tm_set_local_time(CPSS_TIME_T *pstClock);
/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_TM_H */
/******************************** 头文件结束 *********************************/

