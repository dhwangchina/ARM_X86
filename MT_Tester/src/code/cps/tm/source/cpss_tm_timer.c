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
#include <signal.h>
#include <sys/time.h>
#include "cpss_public.h"
#include "cpss_common.h"
#include "cpss_err.h"
/* 内部头文件 */
#include "cpss_vos_msg_q.h"
#include "cpss_vk_proc.h"
#include "cpss_tm_timer.h"

#ifdef CPSS_VOS_VXWORKS
#include "kernelLib.h"
#include "drv_public.h"
#include "wdLib.h"                         /* 软件狗头文件*/
#include "drv_pub_common.h"
#endif

/******************************* 局部宏定义 **********************************/
#define CPSS_SYSAUXCLK_SUPPORT
#define  CPSS_TIMER_OUT_MSG     0x21000000
#define  DURATION_DELAY         5
#define  AVL_MAX_HEIGHT         100     /* 平衡二叉树的深度 */
#define  M_tSubSysData          (g_pstCpssTmCoreData->tSubSysData)
#define  DAY_SECONDS            86400   /* 24 * 3600，一天中的秒数 */

#define CPSS_TM_DEFAULT_TIME_ZONE       20
#define CPSS_HW_TIMER_INTERVAL       2    /* 硬件定时器精度*/
/******************************* 全局变量定义/初始化 *************************/
CPSS_TM_COREDATA_T  *g_pstCpssTmCoreData;  
UINT32      g_ulCpssTmAnsicTimeTo2k = 0;
UINT16      g_usCpssTmClockRate = CLOCK_RATE;
/* 初始时区*/
UINT8  g_ucCpssTmGmtOffset = CPSS_TM_DEFAULT_TIME_ZONE;                                /* gmt 偏移 0~25 ,defaule 20*/
#ifdef CPSS_DSP_CPU
volatile INT32  g_lAppSysTicks = 0;
volatile int    g_lIsrCondVar;
volatile int    g_lIsrSysTicks;
volatile int    g_lRfnSynTicks = 0;

volatile UINT32 g_ulCpssBusyTimeCntr;          /* 为计算CPU占用率增加 */
volatile UINT32 g_ConstCpssBusyRateCalPeriod;  /* 为计算CPU占用率增加 */
volatile UINT32 g_ulCpssCpuBusyRate;           /* 为计算CPU占用率增加 */
volatile UINT32 g_ulCpssTimer0Period;
volatile UINT32 g_ulCpssBusyRateCalCntr;
volatile UINT32 g_ulHeartBeartCntr = 0;
#endif

extern CPSS_VK_PROC_PCB_T *  cpss_vk_proc_pcb_get 
(
    UINT32  ulProcDesc /* procedure descriptor */
 /*CPSS_VK_PROC_PCB_T *       pPcb / pointer to control block */
);

extern UINT32 cpss_vk_proc_sched_get
(
    UINT32  ulProcDesc /* procedure descriptor */
);
#ifdef CPSS_VOS_VXWORKS   
extern VOID cpss_tm_hdr_int_isr();
#endif
/******************************* 局部常数和类型定义 **************************/

 const UINT16 usDeltaDays[12] = {
    0,   31,   59, 90,  120, 151, 
    181, 212, 243, 273, 304, 334
};

/* 定时器消息缓冲结构*/
typedef struct tagCPSS_TM_MSGBUFFER
{
    IPC_MSG_HDR_T      *ptMsgHead;      /* 缓存的第一个定时器消息 */
    IPC_MSG_HDR_T      *ptMsgTail;      /* 缓存的最后一个定时器消息 */
}CPSS_TM_MSGBUFFER_T;

/* 在二叉树中进行查找和删除时用到的栈 */   
typedef struct tagCPSS_TM_PARAMTIMERSTACK
{ 
    UINT32    aulScanedTid[AVL_MAX_HEIGHT];     /* 删除时扫描过的定时器标识 */
    INT16     asBranch[AVL_MAX_HEIGHT];         /* 将被删除节点在当前节点的左枝还是右枝*/
    INT16     sStackTop;                        /* 栈顶指针 */
}CPSS_TM_PARAMTIMERSTACK_T;

 CPSS_TM_TIMERINFO_T          g_stCpssTmTimerInfo;                    /* 定时器统计信息数据结构 */
 UINT32                       g_ulCpssTmMutexSemForTimerInfo;         /* 定时器统计信息数据结构的互斥信号量 */
 CPSS_TM_MSGBUFFER_T          g_astCpssTmMsgBuffer[VK_MAX_SCHED_NUM]; /* 消息缓冲区 */                      
 CPSS_TM_PARAMTIMERSTACK_T    g_astCpssTmParamTimerStack[VK_MAX_SCHED_NUM];  /* 带参定时器栈 */         

#ifdef CPSS_VOS_VXWORKS                                                  
 BOOL         g_bCpssTmIsUseAuxClock = FALSE;             /* 是否使用辅助时钟 */
 WDOG_ID    g_CpssTmDogId;                                       /* 软件狗描述符*/
 UINT16       g_usCpssTmDelayTicks = 1;                       /* 软件狗延迟*/
#endif

 UINT16       g_usCpssTmMsNumInOneSec = 1000;             /* 默认情况下为每秒1000毫秒 */
/* static UINT8                g_uc10msInitOver = FALSE;  */
 UINT16       g_usCpssTmDurationOfTick;  
extern UINT32 g_ulCpssMicroSec;
extern CPSS_VK_PROC_CLASS_T  g_astCpssVkProcClassTbl [VK_MAX_PROC_CLASS_NUM];

BOOL g_bCpssTmCmosErr = FALSE;    /* 记录cmos时间是否出错*/

/******************************* 局部函数原型声明 ****************************/

 UINT32   cpss_timer_set_no_para_timer(UINT8 ucTimerNo, UINT8 ucTimerType,UINT32 ulTickSum);
 UINT32   cpss_timer_set_para_timer(UINT8 ucTimerNo,UINT8 ucTimerType,UINT32 ulTickSum,UINT32 ulParam);
 VOID     cpss_timer_append_para_timer_to_pcb(UINT32 ulTimerId, CPSS_VK_PROC_PCB_T *pstPCB);
 INT32    cpss_timer_remove_para_timer(UINT32 ulTimerId);
 VOID     cpss_timer_add_relative_timer(UINT32 ulTimerId);

 INT32    cpss_timer_rel_queue_pick_tmcb(UINT32 ulTimerId);

 VOID     cpss_timer_tick_array_insert_tmcb(UINT32 ulTimerId,UINT8 bIsFromTidToTick);
 INT32    cpss_timer_pcb_delete_tmcb(UINT32 ulTimerId,CPSS_VK_PROC_PCB_T *ptPcb);
#ifdef CPSS_VOS_VXWORKS
 void     cpss_timer_sem_give_isr(VOID);
 BOOL    cpss_timer_set_aux_clock(UINT16 usClockRate);
#endif
 INT32  cpss_timer_insert_tmcb_node(UINT32 ulTimerId,UINT32 ulTimerParam,UINT32 ulPNo,UINT8 ucTimerNo);
 INT32  cpss_timer_delete_tmcb_node(UINT32 ulTimerParam,UINT32 ulPNo,UINT8 ucTimerNo);
 VOID     cpss_timer_relative_timer_scan(VOID);
 VOID     cpss_timer_tick_array_send_tm_msg(UINT32 ulHeadTid);
 VOID     cpss_timer_scan_tick_array(VOID);
 void     cpss_timer_set_initial_clk(void);
 UINT32   cpss_timer_get_tmcb(VOID);
 INT32    cpss_timer_free_tmcb(UINT32 ulTimerId);
 INT32    cpss_timer_remove_no_para_timer(UINT32 ulTimerId);
 INT32    cpss_timer_init_timer_module(VOID);
INT32 cpss_timer_loop_timer_restart(UINT32 ulTimerId);
/*static INT32    cpss_timer_pcb_clean_all_timer(CPSS_VK_PROC_PCB_T  *pstPCB);*/
 INT32    cpss_timer_pcb_clear_timer_record(UINT32 ulTimerId,CPSS_VK_PROC_PCB_T *pstDestPcb);
/*static UINT32   cpss_timer_get_idle_tmcb_count(void);*/
 #ifndef CPSS_DSP_CPU
 INT32    cpss_timer_set_sys_clock(CPSS_TM_SYSSOFTCLOCK_T *pstNewClock);
 VOID     cpss_timer_add_abs_timer(UINT32 ulTimerId);
 INT32    cpss_timer_abs_queue_pick_tmcb(UINT32 ulTimerId);
 INT32    cpss_timer_abs_timer_scan(UINT32 ulSeconds);
INT32    cpss_timer_clock2time(CPSS_TM_SYSSOFTCLOCK_T *ptClock,CPSS_TM_TIMER_T  *pstSecMillisec);
 #endif


void cpss_timer_sem_give_isr();
void cpss_tm_sig_rcv();
VOID cpss_tm_loop_task();

/******************************* 函数实现 ************************************/
#ifdef CPSS_VOS_VXWORKS
/*******************************************************************************
* 函数名称: cpss_timer_get_cmos()
* 功    能: 读取CMOS时间 
* 函数类型: 
* 参    数: 
* 参数名称    类型                               输入/输出    描述
* ptClocK            CPSS_TM_SYSSOFTCLOCK_T *    输出             存放读取的时间
* 函数返回: CPSS_OK, CPSS_ERROR
* 说    明:                                 
*******************************************************************************/

INT32 cpss_timer_get_cmos(CPSS_TM_SYSSOFTCLOCK_T      * ptClock)
{

  #ifdef CPSS_FUNBRD_MC  
     
    DRV_RTC_DATE_TIME_T stRtcTime;
    if(CPSS_OK == drv_rt_timer_get(&stRtcTime))
        {
             ptClock->usSysYear = stRtcTime.usYear + 2000;      /*vxWorks 设置初始时间为2006/01/01/00:/00:/00*/
             ptClock->ucSysMon  = stRtcTime.usMonth;
             ptClock->ucSysDay  = stRtcTime.usDayOfMonth;
             ptClock->ucSysHour = stRtcTime.usHour;
             ptClock->ucSysMin  = stRtcTime.usMinute;
             ptClock->ucSysSec  = stRtcTime.usSecond;

              if (2000 > ptClock->usSysYear 
           || ptClock->usSysYear > 2120
           || ptClock->ucSysMon > 12
           || ptClock->ucSysMon == 0
           || 23 < ptClock->ucSysHour        
           || 59 < ptClock->ucSysMin
           || 59 < ptClock->ucSysSec)
           {
               cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input field error in cpss_timer_get_cmos \n");           
               return CPSS_ERROR;
           }
       
           /* 检查系统软时钟域值 */    
           if (  (( 1 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 31))
               || (( 3 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 31))
               || (( 5 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 31))           
               || (( 7 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 31))
               || (( 8 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 31))
               || ((10 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 31))
               || ((12 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 31))
               || (( 4 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 30))
               || (( 6 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 30))
               || (( 9 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 30))
               || ((11 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 30)))
           {
               cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input field error in cpss_timer_get_cmos\n");           
               return CPSS_ERROR;
           }
           
           /* 检查系统软时钟域值，考虑闰年时的2月份 */
        /*   if ((ptClock->usSysYear % 4 == 0 && ptClock->usSysYear % 100 != 0)
               || (ptClock->usSysYear % 400 == 0))
               */
           if ((CPSS_MOD(ptClock->usSysYear , 4) == 0 && CPSS_MOD(ptClock->usSysYear ,100) != 0)
               || (CPSS_MOD(ptClock->usSysYear , 400) == 0))
    
           {
               /* 闰年时的2月份时的天数为29天 */
               if ((2 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 29))
               {
                   cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input field error in cpss_timer_get_cmos \n");               
                   return CPSS_ERROR;
               }
           }
           else
           {
               /* 非闰年时的2月份的天数为28天 */
               if ((2 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 28))
               {
                   cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input field error in cpss_timer_get_cmos \n");               
                   return CPSS_ERROR;
               }
           }       
           return CPSS_OK;
        }
        else
        {
          return CPSS_ERROR;
         }
#endif
   return CPSS_ERROR;

}

/*******************************************************************************
* 函数名称: cpss_timer_set_cmos()
* 功    能: 设置cmos时间 
* 函数类型: 
* 参    数: 
* 参数名称    类型                               输入/输出    描述
* ptClocK            CPSS_TM_SYSSOFTCLOCK_T *     输入            存放设置的时间
* 函数返回: CPSS_OK, CPSS_ERROR
* 说    明:                                 
*******************************************************************************/

INT32 cpss_timer_set_cmos(CPSS_TM_SYSSOFTCLOCK_T      *ptClocK)
{
  #ifdef CPSS_FUNBRD_MC 
     INT32 lRet ; 
     DRV_RTC_DATE_TIME_T stRtcTime;
  /* 若当前时间发生错误*/
     if((ptClocK->usSysYear < 2007) || (ptClocK->usSysYear > 2018))
        {
           DRV_RTC_DATE_TIME_T stRtcOldTime;
        
        /* 只记录一次*/
           if(g_bCpssTmCmosErr == FALSE)
            {
               if(drv_rt_timer_get(&stRtcOldTime) == CPSS_OK)
                {
                      cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM,CPSS_PRINT_IMPORTANT,
                     " drv_rt_timer_get return century = %d year = %d month = %d\n\r",stRtcOldTime.usCentury, stRtcOldTime.usYear,stRtcOldTime.usMonth);
                }
                else
                {
                      cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM,CPSS_PRINT_IMPORTANT,
                     " drv_rt_timer_get return fail\n\r");
                   
                }
                  cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM,CPSS_PRINT_IMPORTANT," write cmos year = %d\n\r",ptClocK->usSysYear);
               
            }
           g_bCpssTmCmosErr = TRUE;
           
        }
     stRtcTime.usCentury  = 21;
     stRtcTime.usYear      = ptClocK->usSysYear - 2000;
     stRtcTime.usMonth    = ptClocK->ucSysMon;
     stRtcTime.usDayOfMonth = ptClocK->ucSysDay;
     stRtcTime.usDayOfWeek  = ((ptClocK->ucSysWeek == 0)?  7:ptClocK->ucSysWeek);
     stRtcTime.usHour = ptClocK->ucSysHour;
     stRtcTime.usMinute = ptClocK->ucSysMin;
     stRtcTime.usSecond = ptClocK->ucSysSec;
     
     lRet =  (drv_rt_timer_init_set(&stRtcTime)); 
     
     return lRet;
  
  #endif  
   return CPSS_ERROR; 
}

/*******************************************************************************
* 函数名称: cpss_timer_read_cur_drv_clock()
* 功    能: 在收到激活请求后记录当前的驱动时间 
* 函数类型: 
* 参    数: 
* 参数名称    类型                               输入/输出    描述
* 
* 说    明:                                 
*******************************************************************************/
DRV_RTC_DATE_TIME_T g_CpssInitCmosTime;

VOID cpss_timer_save_cmos_init_clock(VOID)
{
 #ifdef CPSS_FUNBRD_MC 
     cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM,CPSS_PRINT_IMPORTANT,
                     "init state  drv_rt_timer_get return century = %d year = %d month = %d day = %d usHour= %d usMin = %d usSec= %d\n\r",
                                     g_CpssInitCmosTime.usCentury, 
                                     g_CpssInitCmosTime.usYear,
                                     g_CpssInitCmosTime.usMonth,
                                     g_CpssInitCmosTime.usDayOfMonth,
                                     g_CpssInitCmosTime.usHour,
                                     g_CpssInitCmosTime.usMinute,
                                     g_CpssInitCmosTime.usSecond
                                                   );
 #endif

}

VOID cpss_timer_read_cmos_init_clock(VOID)
{
 #ifdef CPSS_FUNBRD_MC 
         cpss_mem_memset(&g_CpssInitCmosTime,0, sizeof(DRV_RTC_DATE_TIME_T));
         drv_rt_timer_get(&g_CpssInitCmosTime);

#endif
}
#endif

/*******************************************************************************
* 函数名称: cpss_timer_set_initial_clk()
* 功    能: 初始时钟和精度 
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
VOID cpss_timer_set_initial_clk(VOID)
{
#ifdef CPSS_VOS_VXWORKS
    
    CPSS_TM_SYSSOFTCLOCK_T       stClock;
    /*CPSS_TM_TIMER_T               stSeconds;  */
    struct timespec      stTime;
    time_t               ulSecond;
    struct tm            *pstBreakDownTime;
    struct tm            stBreakDownTime;    
    INT32                lResult = CPSS_ERROR;
    
    /* 计算ANSI C时间基准和2000年1月1日0时0分0秒之间的秒数 */
    stTime.tv_sec  = 0;
    stTime.tv_nsec = 0;
    clock_settime(CLOCK_REALTIME, &stTime);
    ulSecond = time(NULL);
    pstBreakDownTime = localtime(&ulSecond);
    ulSecond = mktime(pstBreakDownTime);
    
    stBreakDownTime.tm_year  = 100; /* 2000年 */
    stBreakDownTime.tm_mon   = 0;   /* 在这种时间结构中，月份从0到11 */
    stBreakDownTime.tm_mday  = 1;
    stBreakDownTime.tm_hour  = 0;
    stBreakDownTime.tm_min   = 0;
    stBreakDownTime.tm_sec   = 0;
    stBreakDownTime.tm_isdst = 0;   /* 不是夏令时 */
    pstBreakDownTime = &stBreakDownTime;
    g_ulCpssTmAnsicTimeTo2k = mktime(pstBreakDownTime) - ulSecond;
    
    memset(&stClock , 0, sizeof(CPSS_TM_SYSSOFTCLOCK_T));

    lResult = cpss_timer_get_cmos(&stClock);
    if( lResult != CPSS_OK)
    {
        stClock.usSysYear = 2006;      /*vxWorks 设置初始时间为2006/01/01/00:/00:/00*/
        stClock.ucSysMon  = 01;
        stClock.ucSysDay  = 01;
        stClock.ucSysHour = 0;
        stClock.ucSysMin  = 0;
        stClock.ucSysSec  = 0;
    }
    
    cpss_timer_set_sys_clock(&stClock);
#endif
    
#ifdef CPSS_VOS_WINDOWS
    time_t ulSeconds = 0;
    struct tm            *pstBreakDownTime;
    CPSS_TM_SYSSOFTCLOCK_T       stClock;
     struct tm            tBreakDownTime;
     
    memset(&stClock , 0, sizeof(CPSS_TM_SYSSOFTCLOCK_T));
    ulSeconds = time(NULL);                
    pstBreakDownTime = localtime(&ulSeconds); 

   
    stClock.usSysYear = (UINT16)(pstBreakDownTime->tm_year +1900);
    stClock.ucSysMon  = (UINT8)(pstBreakDownTime->tm_mon +1);
    stClock.ucSysDay  = (UINT8)pstBreakDownTime->tm_mday;
    stClock.ucSysHour = (UINT8)pstBreakDownTime->tm_hour;
    stClock.ucSysMin  = (UINT8)pstBreakDownTime->tm_min;
    stClock.ucSysSec  = (UINT8)pstBreakDownTime->tm_sec;
    cpss_timer_set_sys_clock(&stClock);
     tBreakDownTime.tm_year  = 100; /* 2000年 */
     tBreakDownTime.tm_mon   = 0;   /* 在这种时间结构中，月份从0到11 */
     tBreakDownTime.tm_mday  = 1;
     tBreakDownTime.tm_hour  = 0;
     tBreakDownTime.tm_min   = 0;
     tBreakDownTime.tm_sec   = 0;
     tBreakDownTime.tm_isdst = 0;   /* 不是夏令时 */
     pstBreakDownTime = &tBreakDownTime; 
     g_ulCpssTmAnsicTimeTo2k = mktime(pstBreakDownTime) + 3600*cpss_timer_get_gmtoffset();

#endif

#ifdef CPSS_VOS_LINUX
    time_t ulSeconds = 0;
    struct tm            *pstBreakDownTime;
    CPSS_TM_SYSSOFTCLOCK_T       stClock;
     struct tm            tBreakDownTime;
     
    memset(&stClock , 0, sizeof(CPSS_TM_SYSSOFTCLOCK_T));
    ulSeconds = time(NULL);                
    pstBreakDownTime = localtime(&ulSeconds); 

   
    stClock.usSysYear = (UINT16)(pstBreakDownTime->tm_year +1900);
    stClock.ucSysMon  = (UINT8)(pstBreakDownTime->tm_mon +1);
    stClock.ucSysDay  = (UINT8)pstBreakDownTime->tm_mday;
    stClock.ucSysHour = (UINT8)pstBreakDownTime->tm_hour;
    stClock.ucSysMin  = (UINT8)pstBreakDownTime->tm_min;
    stClock.ucSysSec  = (UINT8)pstBreakDownTime->tm_sec;
    cpss_timer_set_sys_clock(&stClock);

    /* 2006/08/21 增加WIN下的g_ulCpssTmAnsicTimeTo2k的获得*/
     tBreakDownTime.tm_year  = 100; /* 2000年 */
     tBreakDownTime.tm_mon   = 0;   /* 在这种时间结构中，月份从0到11 */
     tBreakDownTime.tm_mday  = 1;
     tBreakDownTime.tm_hour  = 0;
     tBreakDownTime.tm_min   = 0;
     tBreakDownTime.tm_sec   = 0;
     tBreakDownTime.tm_isdst = 0;   /* 不是夏令时 */
     pstBreakDownTime = &tBreakDownTime;
 
     g_ulCpssTmAnsicTimeTo2k = mktime(pstBreakDownTime) + 3600*cpss_timer_get_gmtoffset();

#endif
}

/*******************************************************************************
* 函数名称: cpss_timer_set_aux_clock
* 功    能: 设置系统辅助时钟
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* usClockRate       UINT16                IN            中断频率
* 函数返回: 成功    TRUE
*           失败    FALSE
* 说    明:                                 
*******************************************************************************/
#ifdef CPSS_VOS_VXWORKS
BOOL cpss_timer_set_aux_clock(UINT16 usClockRate)
{    

#ifdef CPSS_SYSAUXCLK_SUPPORT
  INT32 lResult;
/* 2006/08/04 用狗代替辅助时钟，避免和驱动冲突*/
  #if 0
    ulResult = sysAuxClkRateSet((INT32)usClockRate);   /* cpss_type_cast */
    if (ulResult == CPSS_ERROR)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"cpss_timer_set_aux_clock: set system auxclock rate fail!\n");
        return FALSE;
    }
    sysAuxClkEnable();    
   #endif 
  
   g_CpssTmDogId = wdCreate();   /*创建软件狗*/
    if(NULL == g_CpssTmDogId)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL ," cpss Create Tm Watch Dog Fail \n\r");
        return FALSE;
    }
    /* 启动软件狗*/
    lResult = wdStart(g_CpssTmDogId, usClockRate, (FUNCPTR)cpss_timer_sem_give_isr, 0); /*启动软件狗*/
    if(CPSS_OK != lResult)
    {
        wdDelete(g_CpssTmDogId);
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL ," cpss wdStart Tm Watch Dog Fail \n\r");
        return FALSE;
    }
   /* 如果用狗代替辅助时钟,则采用sysClkRateGet() */
    g_usCpssTmClockRate =  sysClkRateGet();
    return TRUE;

#else
    return   FALSE ;
#endif
    
}
#endif

/*******************************************************************************
* 函数名称: cpss_timer_aux_clk_conn
* 功    能: 连接中断处理函数
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* auxClkISR       TFuncPtr               IN            中断处理函数
* lArgument       INT32                  IN            参数
* 函数返回: 成功    TRUE
*           失败    FALSE
* 说    明:                                 
*******************************************************************************/
#ifdef CPSS_VOS_VXWORKS
BOOL cpss_timer_aux_clk_conn
(
    TFuncPtr auxClkISR, 
    INT32 lArgument
)
{
    INT32 lResult = CPSS_OK;

#ifdef CPSS_SYSAUXCLK_SUPPORT
/* 2006/08/04 用狗代替辅助时钟，避免和驱动冲突*/
  #if 0
    ulResult = sysAuxClkConnect((TFuncPtr)auxClkISR, (INT)lArgument); /* cpss_type_cast */
  #endif
  
#else
    lResult = CPSS_ERROR;
#endif
    if (lResult != CPSS_OK)
    {  
        
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"cpss_timer_aux_clk_conn fail!\n");
        return FALSE;
    }
    
    return TRUE;
}
#endif

/*******************************************************************************
* 函数名称: cpss_timer_init_timer_module
* 功    能: 初始化定时器资源
* 函数类型: 
* 参    数: 
* 参数名称    类型  输入/输出   描述
* 
* 函数返回: 成功    CPSS_OK
*           失败    CPSS_FAIL
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_init_timer_module(VOID)
{
    CPSS_TM_RELTIMERQUEUE_T      *pstRelTQueue;
    CPSS_TM_ABSTIMERQUEUE_T      *pstAbsTQueue;
    UINT32               ulTmcbIndex;
    CPSS_TM_TMCB_T               *pstTMCB;
    CPSS_TM_TMCBPOOLITEM_T       *pstTMCBPool;
    UINT32               ulLoop;
    UINT32 ulResult = CPSS_ERROR;
    struct itimerval ticks;
    sigset_t set;
    pthread_t ptSigRcv;
#ifdef CPSS_VOS_VXWORKS
    sysClkRateSet(CLOCK_RATE);

/* removed for init system failed 12-25 */    
#if 0    
    ulResult = sysAuxClkRateSet(CLOCK_RATE);  
    if (ulResult != CPSS_OK)
    {
        printf("cpss_timer_set_aux_clock: set system auxclock rate fail!\n");
        return CPSS_ERROR;
    }
    sysAuxClkEnable();  
#endif
   
  /*  enables round-robin  */ 
      kernelTimeSlice(1);
   /* 注册hw timer回调函数*/
   
   if(CPSS_OK !=  drv_hw_timer_func_reg(cpss_tm_hdr_int_isr))
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL ," cpss reg HW timer func Fail \n\r");
        return CPSS_ERROR;    
    }
    /* 启动hw timer */
    if(CPSS_OK != drv_hw_timer_control(CPSS_HW_TIMER_INTERVAL,DRV_HW_TIMER_OPEN))
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL ," cpss open HW timer Fail \n\r");
        return CPSS_ERROR;    
    }
#endif 
    
    /* g_pstCpssTmCoreData = (CPSS_TM_COREDATA_T *)cpss_mem_malloc(sizeof(CPSS_TM_COREDATA_T)); */
    
    g_pstCpssTmCoreData = (CPSS_TM_COREDATA_T *)malloc(sizeof(CPSS_TM_COREDATA_T)); /* cpss_type_cast */
    
    if(NULL == g_pstCpssTmCoreData)
    {
        return CPSS_ERR_TM_NO_ENOUGH_SYS_MEM;
    }
    memset(g_pstCpssTmCoreData, 0, sizeof(CPSS_TM_COREDATA_T));
        
    /* 创建定时器控制块互斥信号量 */
    g_pstCpssTmCoreData->ulMutexSemForTMCB = cpss_vos_sem_m_create(VOS_SEM_Q_PRI);
    if( g_pstCpssTmCoreData->ulMutexSemForTMCB == VOS_SEM_DESC_INVALID)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL," \ncreate timer MutexSemForTMCB Fail \n");
        return CPSS_ERROR;
    }
        
    /* 创建定时器队列互斥信号量 */
    g_pstCpssTmCoreData->ulMutexSemForTimerQueue = cpss_vos_sem_m_create(VOS_SEM_Q_PRI);
    if( g_pstCpssTmCoreData->ulMutexSemForTimerQueue == VOS_SEM_DESC_INVALID)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL," \n create timer MutexSemForTimerQueue \n");
        return CPSS_ERROR;
    }
        
    
    /* 创建TICK计数数据结构互斥信号量 */
    g_pstCpssTmCoreData->ulMutexSemForTickCounts = cpss_vos_sem_m_create(VOS_SEM_Q_PRI);
    if( g_pstCpssTmCoreData->ulMutexSemForTickCounts == VOS_SEM_DESC_INVALID)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL," \n create timer MutexSemForTickCounts Fail \n");
        return CPSS_ERROR;
    }
    
    /* 创建系统软时钟结构互斥信号量 */
    
    g_pstCpssTmCoreData->ulMutexSemForSysSoftClock = cpss_vos_sem_m_create(VOS_SEM_Q_PRI);
    if( g_pstCpssTmCoreData->ulMutexSemForSysSoftClock == VOS_SEM_DESC_INVALID)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL," \n create timer MutexSemForSysSoftClock Fail \n");
        return CPSS_ERROR;
    }  
    
    
    /* 创建定时器统计信息互斥信号量 */   
    g_ulCpssTmMutexSemForTimerInfo = cpss_vos_sem_m_create(VOS_SEM_Q_PRI);
    if( g_ulCpssTmMutexSemForTimerInfo == VOS_SEM_DESC_INVALID)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n create timer MutexSemForTimerInfo Fail \n");
        return CPSS_ERROR;
    }     
    
    /* 初使化上电后的TICK数 */
    g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountHigh = 0;
    g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow  = 0;
    g_pstCpssTmCoreData->ulSecFromPowerOn = 0;    
    
    /* 设置系统的初始时间 */
    cpss_timer_set_initial_clk();
    
    /* 初使化定时器消息缓冲区 */
    for (ulLoop = 0; ulLoop < VK_MAX_SCHED_NUM; ulLoop ++)
    {
        g_astCpssTmMsgBuffer[ulLoop].ptMsgHead = NULL;
        g_astCpssTmMsgBuffer[ulLoop].ptMsgTail = NULL; 
    }  
    
    /* 初使化绝对定时器队列 */
    pstAbsTQueue = (CPSS_TM_ABSTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stAbsTimerQueue);    
    pstAbsTQueue->ulElementCount = 0;
    pstAbsTQueue->ulHead         = LEOF;            
    pstAbsTQueue->ulTail         = LEOF; 
    
    /* 初使化相对定时器队列 */
    pstRelTQueue = (CPSS_TM_RELTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stRelTimerQueue);    
    pstRelTQueue->ulElementCount     = 0;
    pstRelTQueue->ulCursorForTick    = 0;
    pstRelTQueue->ulCursorForTid     = 0;
    memset((UINT8 *)&(pstRelTQueue->aulTickArray[0]), 0xff, sizeof(UINT32) * MAX_TIMER_NUM);/* cpss_type_cast */
    memset((UINT8 *)&(pstRelTQueue->aucTidArray[0]), FALSE, sizeof(UINT8) * MAX_TIMER_NUM); /* cpss_type_cast */
    
    
    /* 初使化所有的定时器控制块 */  
    pstTMCB         = (CPSS_TM_TMCB_T  *)&(g_pstCpssTmCoreData->astTMCB[0]);       
    pstTMCBPool     = (CPSS_TM_TMCBPOOLITEM_T *)&(g_pstCpssTmCoreData->stTMCBPool);
    memset((UINT8 *)pstTMCB, 0xff, sizeof(CPSS_TM_TMCB_T)*MAX_TIMER_NUM);          /* cpss_type_cast */
    for (ulTmcbIndex = 0; ulTmcbIndex < MAX_TIMER_NUM; ulTmcbIndex ++)
    {
        pstTMCB[ulTmcbIndex].bIsInUse        = FALSE;
        pstTMCB[ulTmcbIndex].bIsTimeOut      = FALSE;  
        pstTMCB[ulTmcbIndex].ulPno           = CPSS_VK_PD_INVALID;
        pstTMCB[ulTmcbIndex].ulCount         = 0;   /* 定时时长初使化为0 */    
        pstTMCB[ulTmcbIndex].ulMsgHeadAddr   = 0;     
        pstTMCBPool->aulElement[ulTmcbIndex] = ulTmcbIndex;
        pstTMCB[ulTmcbIndex].ulPcb  = 0;
    }
    
    /* 初使化定时器控制块池统计信息 */
    pstTMCBPool->ulFreeCount = MAX_TIMER_NUM;
    pstTMCBPool->ulHead      = 0;
    pstTMCBPool->ulTail      = 0;
    
    /* 初使化定时器信息统计结构 */  
    g_stCpssTmTimerInfo.ulTimerPeerCount = 0;
    g_stCpssTmTimerInfo.ulSuccessCount   = 0;
    g_stCpssTmTimerInfo.ulFailureCount   = 0;
    
#ifdef CPSS_VOS_VXWORKS
    /*创建定时器扫描任务同步信号量*/
    g_pstCpssTmCoreData->ulSynSemForTick = cpss_vos_sem_b_create(VOS_SEM_Q_FIFO,VOS_SEM_EMPTY);
    if( g_pstCpssTmCoreData->ulSynSemForTick==VOS_SEM_DESC_INVALID)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n create timer SynSemForTick Fail \n");
        return CPSS_ERROR;
    }
    
    /* 设置辅助时钟速率。如果不能使用辅助时钟，置使用标志为FALSE；否则TRUE */ 
   /* 2006/08/04 用狗代替辅助时钟，避免和驱动冲突*/
    g_bCpssTmIsUseAuxClock = cpss_timer_set_aux_clock(g_usCpssTmDelayTicks);
    if (g_bCpssTmIsUseAuxClock == FALSE)
    {        
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n cpss_timer_set_aux_clock Fail \n"); 
    }    
    
    /* 将中断处理函数挂到辅助时钟中断上 */
    if (TRUE == g_bCpssTmIsUseAuxClock)
    {
        if (cpss_timer_aux_clk_conn((TFuncPtr)cpss_timer_sem_give_isr, 0)
            == FALSE)
        {
            
            cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n cpss_timer_aux_clk_conn Fail \n");
            
            g_bCpssTmIsUseAuxClock = FALSE;
        }
    }
    
    if (FALSE == g_bCpssTmIsUseAuxClock)
    {
        g_usCpssTmClockRate = sysClkRateGet();
    }
#endif

#ifndef SWP_CYGWIN
    g_pstCpssTmCoreData->ulSynSemForTick = cpss_vos_sem_b_create(VOS_SEM_Q_FIFO,VOS_SEM_EMPTY);
    if( g_pstCpssTmCoreData->ulSynSemForTick==VOS_SEM_DESC_INVALID)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n create timer SynSemForTick Fail \n");
        return CPSS_ERROR;
    }
	
#if 0
      signal(SIGALRM, cpss_timer_sem_give_isr);

  	sigemptyset(&set);
  	sigaddset(&set, SIGALRM);
  	sigprocmask(SIG_BLOCK, &set, NULL);

  	pthread_create(&ptSigRcv, NULL, (void*)cpss_tm_sig_rcv, NULL);
  	/*add end by zhoachuan fo sig block*/

	ticks.it_value.tv_sec = 0;
	ticks.it_value.tv_usec = 1000 / g_usCpssTmClockRate * 1000;
	ticks.it_interval.tv_sec = 0;
	ticks.it_interval.tv_usec = 1000 / g_usCpssTmClockRate * 1000;

	setitimer(ITIMER_REAL, &ticks, NULL);
#endif
  	pthread_create(&ptSigRcv, NULL, (void*)cpss_tm_loop_task, NULL);
#endif
    /* cpss_type_cast */
  /*  g_usCpssTmDurationOfTick = (UINT16)(1000 / g_usCpssTmClockRate);               / TICK的长度 */
   g_usCpssTmDurationOfTick = (UINT16)(CPSS_DIV(1000 , g_usCpssTmClockRate)); 
   g_usCpssTmMsNumInOneSec  = (UINT16)(g_usCpssTmDurationOfTick * g_usCpssTmClockRate);  /* 每秒内的毫秒数 */    
    
    
    return CPSS_OK;
    
}

/*******************************************************************************
* 函数名称: cpss_timer_get_tmcb
* 功    能: 获取空闲控制块
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 成功    定时器描述符
*          失败    CPSS_TD_INVALID
* 说    明:                                 
*******************************************************************************/
UINT32 cpss_timer_get_tmcb(VOID)
{
    CPSS_TM_TMCB_T              *pstTMCB;
    UINT32              ulTid;
    UINT32              ulHead;    
    CPSS_TM_TMCBPOOLITEM_T      *pstTmcbPool;
    
    /* 获得定时器控制块池的指针 */
    pstTmcbPool = (CPSS_TM_TMCBPOOLITEM_T *)&(g_pstCpssTmCoreData->stTMCBPool);  
    
    /* 获取互斥信号量 */
    cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTMCB,WAIT_FOREVER);
    if(pstTmcbPool->ulFreeCount <= 0) /* 可用定时器控制块为0 */
    {
        /* 申请定时器控制块不成功，释放互斥信号量 */
        cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTMCB);
        return  CPSS_TD_INVALID;
    }
    
    /* 获得第一个可用的定时器控制块 */
    ulHead  = pstTmcbPool->ulHead;
    ulTid   = pstTmcbPool->aulElement[ulHead];
    pstTmcbPool->ulFreeCount =  pstTmcbPool->ulFreeCount -1;
   /* pstTmcbPool->ulFreeCount --; / 可用定时器控制块数目 -1 */
    
    /* 统计定时器信息 */    
    if (g_stCpssTmTimerInfo.ulTimerPeerCount < MAX_TIMER_NUM - pstTmcbPool->ulFreeCount)
    {
        g_stCpssTmTimerInfo.ulTimerPeerCount = MAX_TIMER_NUM - pstTmcbPool->ulFreeCount;
    }    
    
    /* 修改空闲定时器控制块池的头部 */
	pstTmcbPool->ulHead  = pstTmcbPool->ulHead  +1;
  /*  pstTmcbPool->ulHead ++;*/
    
    if(pstTmcbPool->ulHead >= MAX_TIMER_NUM)
    {        
        pstTmcbPool->ulHead = 0;
    }
    
    /* 置定时器控制块使用标志为 TRUE   */
    pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTid]);      
    pstTMCB->bIsInUse = TRUE;
    
    /* 释放互斥信号量 */
    cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTMCB);
    return ulTid;
}

/*******************************************************************************
* 函数名称: cpss_timer_free_tmcb
* 功    能: 释放定时器控制块
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTinerID          UINT32                IN           控制块描述符
* 函数返回: 成功    CPSS_OK
*          失败    CPSS_FAIL
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_free_tmcb(UINT32 ulTimerId)
{
    UINT32              ulTail;
    CPSS_TM_TMCB_T              *pstTMCB;
    CPSS_TM_TMCBPOOLITEM_T      *pstTmcbPool;
    
    pstTmcbPool = (CPSS_TM_TMCBPOOLITEM_T *)&(g_pstCpssTmCoreData->stTMCBPool);   
    pstTMCB     = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTimerId]); 
    
    /* 检查输入参数 */
    if (ulTimerId >= MAX_TIMER_NUM)
    {  
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n Timer Id Error in cpss_timer_free_tmcb\n");        
        return CPSS_ERROR;         
    }
    
    /* 获取互斥信号量 */
    cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTMCB,WAIT_FOREVER);

    /* 清除定时器控制块中的信息并放入空闲定时器队列的尾部 */
    if (pstTmcbPool->ulFreeCount < MAX_TIMER_NUM && pstTMCB->bIsInUse)
    {
        /* 清除定时器控制块中的信息 */
        pstTMCB->bIsInUse           = FALSE;
        pstTMCB->ulPno              = CPSS_VK_PD_INVALID; 
        pstTMCB->ulMsgHeadAddr      = 0;
        
        pstTMCB->ulPcb = 0;
        
        /* 放入空闲定时器队列的尾部 */      
        ulTail                         = pstTmcbPool->ulTail;
        pstTmcbPool->aulElement[ulTail] = ulTimerId;
		
        pstTmcbPool->ulFreeCount = pstTmcbPool->ulFreeCount +1;
        pstTmcbPool->ulTail = pstTmcbPool->ulTail +1;		
	  /*		
        pstTmcbPool->ulFreeCount ++;
        pstTmcbPool->ulTail ++;       
        */
        
        if (pstTmcbPool->ulTail >= MAX_TIMER_NUM)
        {
            pstTmcbPool->ulTail = 0;
        }
        cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTMCB);
        return CPSS_OK;
    }
    /* 出现错误，释放互斥信号量 */
    cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTMCB);
    return CPSS_ERROR;
}

/*******************************************************************************
* 函数名称: cpss_timer_set_abs_timer
* 功    能: 设置绝对定时器
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ucTimerNo         UINT8               IN              定时器号
* ulParam          UINT32               IN              参数
* pstAbsTime        CPSS_TM_SYSSOFTCLOCK_T *     IN              绝对时钟
* 函数返回: 成功    控制块描述符
*          失败    CPSS_TD_INVALID
* 说    明:                                 
*******************************************************************************/
#ifndef CPSS_DSP_CPU
UINT32 cpss_timer_set_abs_timer
(
 UINT8 ucTimerNo,
 UINT32 ulParam,
 CPSS_TM_SYSSOFTCLOCK_T *pstAbsTime
 )
{
    CPSS_TM_TIMER_T       stSecMillisec;
    CPSS_TM_TIMER_T       *pstSecMillisec = &stSecMillisec;
    INT32        ulStatus;
    UINT32       ulTid;    
    
    if(ucTimerNo >= MAX_PROC_TIMER)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n TimerNo overflow in cpss_timer_set_abs_timer\n");        
        return CPSS_TD_INVALID;
    }
    if (NULL == pstAbsTime)
    { 
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n Input NULL pointer Abs time in cpss_timer_set_abs_timer\n");        
        return CPSS_TD_INVALID;
    }
    
    /* 绝对时间应该在2000年1月1日0时0分0秒和2121年1月1日0时0分0秒之间 */
    if ((2000 > pstAbsTime->usSysYear) 
        || (pstAbsTime->usSysYear > 2120)
        || (pstAbsTime->ucSysMon > 12)
        || (pstAbsTime->ucSysMon == 0)
        || (23 < pstAbsTime->ucSysHour)        
        || (59 < pstAbsTime->ucSysMin)
        || (59 < pstAbsTime->ucSysSec))
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n Input Abs time  overflow in cpss_timer_set_abs_timer\n");        
        return CPSS_TD_INVALID;
    }
    
    /* 检查绝对时间域值 */    
    if (  (( 1 == pstAbsTime->ucSysMon) && !(1 <= pstAbsTime->ucSysDay && pstAbsTime->ucSysDay <= 31))
        || (( 3 == pstAbsTime->ucSysMon) && !(1 <= pstAbsTime->ucSysDay && pstAbsTime->ucSysDay <= 31))
        || (( 5 == pstAbsTime->ucSysMon) && !(1 <= pstAbsTime->ucSysDay && pstAbsTime->ucSysDay <= 31))        
        || (( 7 == pstAbsTime->ucSysMon) && !(1 <= pstAbsTime->ucSysDay && pstAbsTime->ucSysDay <= 31))
        || (( 8 == pstAbsTime->ucSysMon) && !(1 <= pstAbsTime->ucSysDay && pstAbsTime->ucSysDay <= 31))
        || ((10 == pstAbsTime->ucSysMon) && !(1 <= pstAbsTime->ucSysDay && pstAbsTime->ucSysDay <= 31))
        || ((12 == pstAbsTime->ucSysMon) && !(1 <= pstAbsTime->ucSysDay && pstAbsTime->ucSysDay <= 31))
        || (( 4 == pstAbsTime->ucSysMon) && !(1 <= pstAbsTime->ucSysDay && pstAbsTime->ucSysDay <= 30))
        || (( 6 == pstAbsTime->ucSysMon) && !(1 <= pstAbsTime->ucSysDay && pstAbsTime->ucSysDay <= 30))
        || (( 9 == pstAbsTime->ucSysMon) && !(1 <= pstAbsTime->ucSysDay && pstAbsTime->ucSysDay <= 30))
        || ((11 == pstAbsTime->ucSysMon) && !(1 <= pstAbsTime->ucSysDay && pstAbsTime->ucSysDay <= 30)))
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n Input Abs time  overflow in cpss_timer_set_abs_timer\n");        
        return CPSS_TD_INVALID;
    }
    
    /* 检查绝对时间域值 */
  /*  if ((pstAbsTime->usSysYear % 4 == 0 && pstAbsTime->usSysYear % 100 != 0)
        || (pstAbsTime->usSysYear % 400 == 0))*/
     if ((CPSS_MOD(pstAbsTime->usSysYear ,4) == 0 && CPSS_MOD(pstAbsTime->usSysYear,100) != 0)
        || (CPSS_MOD(pstAbsTime->usSysYear , 400) == 0))   
    {
        if ((2 == pstAbsTime->ucSysMon) && !(1 <= pstAbsTime->ucSysDay && pstAbsTime->ucSysDay <= 29))
        {
            cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n Input Abs time  overflow in cpss_timer_set_abs_timer\n");            
            return CPSS_TD_INVALID;
        }
    }
    else
    {
        if ((2 == pstAbsTime->ucSysMon) && !(1 <= pstAbsTime->ucSysDay && pstAbsTime->ucSysDay <= 28))
        {
            cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n Input Abs time  overflow in cpss_timer_set_abs_timer\n");            
            return CPSS_TD_INVALID;
        }
    }
    
    /* 将绝对时间转换成秒数，时间域的判断将在OSS_ClockToTime中判断 */
    ulStatus = cpss_timer_clock2time(pstAbsTime, pstSecMillisec);
    if (ulStatus != CPSS_OK) /* 错误 */
    {
        
        return  CPSS_TD_INVALID;
    }
    
    if (ulParam == NULL_PARA)
    {
        /* 设置无参定时器 */
        ulTid = cpss_timer_set_no_para_timer(ucTimerNo, ABS_TIMER, stSecMillisec.ulSecond);
    }
    else   
    {
        /* 设置带参定时器 */
        ulTid = cpss_timer_set_para_timer(ucTimerNo,ABS_TIMER, stSecMillisec.ulSecond, ulParam);
    }
    
    /* 统计定时器信息 */
    if (ulTid == CPSS_TD_INVALID)  
    {
        /* 设置定时器失败，失败次数 +1 */    
        g_stCpssTmTimerInfo.ulFailureCount = g_stCpssTmTimerInfo.ulFailureCount +1;
    }
    else  
    {
        /* 设置定时器成功，成功次数 +1 */    
	  g_stCpssTmTimerInfo.ulSuccessCount = g_stCpssTmTimerInfo.ulSuccessCount +1;
    }
    
    return ulTid;    
}
#endif
/*******************************************************************************
* 函数名称: cpss_timer_pcb_clear_timer_record
* 功    能: 清除PCB中某个定时器
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTimerId         UINT32              IN              定时器标识
* pstDestPcb         CPSS_VK_PROC_PCB_T *       IN              纤程控制结构
* 函数返回: 成功    CPSS_OK
*          失败    CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_pcb_clear_timer_record
(
 UINT32 ulTimerId,
 CPSS_VK_PROC_PCB_T *pstDestPcb
 )
{
    CPSS_TM_TMCB_T        *pstTMCB;
    UINT8          ucTimerNo;
    
    
    /* 获得要释放的定时器控制块指针 */
    pstTMCB    = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTimerId]);  
    ucTimerNo = pstTMCB->ucTimerNo;  
    
    /* 判断是否正在使用，若使用则错误 */
/**    
    if (!pstTMCB->bIsInUse)
    { 
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n timer non-use   in cpss_timer_pcb_clear_timer_record\n");        
        return CPSS_ERROR;
    }
**/    
    if (pstTMCB->ulTimerParam == NULL_PARA && ucTimerNo < MAX_PROC_TIMER) /* 无参定时器 */
    {
        /* 将本进程的定时器数目减1 */
        pstDestPcb->aulTimerId[ucTimerNo] = CPSS_TD_INVALID;
        pstDestPcb->usTimerCounts = pstDestPcb->usTimerCounts -1;
	   /* pstDestPcb->usTimerCounts --;*/
    }
    else if (pstTMCB->ulTimerParam != NULL_PARA) /* 带参定时器 */
    {
        /* 从平衡二杈树中删除 */        
        cpss_timer_delete_tmcb_node(pstTMCB->ulTimerParam,pstTMCB->ulPno,ucTimerNo);        
        /* 从进程控制块中删除 */
        cpss_timer_pcb_delete_tmcb(ulTimerId, pstDestPcb);        
        /* 将本进程的定时器数目减1 */
        pstDestPcb->usTimerCounts = pstDestPcb->usTimerCounts -1;
	/*  pstDestPcb->usTimerCounts --;                 */
    }
    else  /* 错误的定时器控制块 */
    {            
        return CPSS_ERROR;
    }
    
    return CPSS_OK;    
}

/*******************************************************************************
* 函数名称: cpss_timer_remove_no_para_timer
* 功    能: 根据定时器标识杀死无参定时器
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTimerId         UINT32              IN              定时器标识
* 函数返回: 成功    CPSS_OK
*          失败    CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_remove_no_para_timer(UINT32 ulTimerId)
{
    CPSS_VK_PROC_PCB_T    *pstPCB = NULL;
    CPSS_TM_TMCB_T   *pstTMCB;
    INT32            lResult;
    
#if 0
    T_TimerMsg       *pstTimerMsg;
#endif
    /* 定时器标识应该小于MAX_TMCB_NUM */
    if (ulTimerId >= MAX_TIMER_NUM)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n Input timer id overflow  in cpss_timer_remove_no_para_timer\n");        
        return CPSS_ERROR;
    }   
    
    /* 获得该定时器控制块的指针 */
    pstTMCB    = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTimerId]);  
    /* 定时器的使用状态应为TRUE */
    if (pstTMCB->bIsInUse != TRUE)
    {         
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n timer status error in cpss_timer_remove_no_para_timer\n");        
        return CPSS_ERROR;
    }
    
    /* 获得当前进程的PCB */    
    /*pstPCB = cpss_vk_proc_pcb_get(pstTMCB->ulPno);*/

    pstPCB  =(CPSS_VK_PROC_PCB_T   *) pstTMCB->ulPcb;

    if(NULL == pstPCB)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n cpss_vk_proc_pcb_get return null  in cpss_timer_remove_no_para_timer\n");        
        return CPSS_ERROR;
    }
    
    /* 获取互斥信号量 */
    lResult = cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTimerQueue,WAIT_FOREVER);
    if(lResult != CPSS_OK)
    {
        return CPSS_ERROR;
    }    
    
    /* 定时器是否超时 */
    if (pstTMCB->bIsTimeOut == FALSE)  /* 没有超时 */
    {
        /* 如果是绝对定时器，从绝对定时器队列删除 */
        if (pstTMCB->ucTimerType == ABS_TIMER)
        {
          #ifndef CPSS_DSP_CPU
            cpss_timer_abs_queue_pick_tmcb(ulTimerId);                               
	   #endif	  
        }    
        else  /* 相对定时器，从相对定时器队列删除 */
        {             
            cpss_timer_rel_queue_pick_tmcb(ulTimerId); 
        }
    }    
    else   /* 定时器已经超时，置定时器消息为不可用 */
    {
        cpss_ipc_msg_expire((VOID *)pstTMCB->ulMsgHeadAddr);
        
    }
    
    /* 释放互斥信号量 */
    lResult = cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTimerQueue);
    if(lResult != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n v ulMutexSemForTimerQueue in cpss_timer_remove_no_para_timer\n");        
        return CPSS_ERROR;
    }
    pstPCB->usTimerCounts = pstPCB->usTimerCounts -1;
  /*  pstPCB->usTimerCounts --; / 进程的定时器数目减1 */
    pstPCB->aulTimerId[pstTMCB->ucTimerNo] = CPSS_TD_INVALID;  /* 清除PCB中的定时器记录 */
    return cpss_timer_free_tmcb(ulTimerId);  /* 释放定时器控制块 */
    
}

/*******************************************************************************
* 函数名称: cpss_timer_kill_timer_by_tid
* 功    能: 根据定时器标识杀死定时器
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTimerId         UINT32              IN              定时器标识
* 函数返回: 成功    CPSS_OK
*          失败    CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_kill_timer_by_tid(UINT32 ulTimerId)
{
    CPSS_TM_TMCB_T           *pstTMCB;
    
    /* 检查输入参数 */
    if (ulTimerId >= MAX_TIMER_NUM)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n timer id overflow in cpss_timer_kill_timer_by_tid\n");        
        return CPSS_ERR_TM_INPUT_INVALID_PARA;
    }
    
    /* 获取该定时器控制块 */
    pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTimerId]);     
    
    /* TMCB中的进称号应和本进程的进称号一致 */
    if (cpss_vk_pd_self_get() != pstTMCB->ulPno)
    { 
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n timer procid error in cpss_timer_kill_timer_by_tid\n");        
        return CPSS_ERR_TM_NOT_IN_PCB;
    }
    
    if ((pstTMCB->bIsInUse) && (NULL_PARA != pstTMCB->ulTimerParam)) 
    {
        /* 杀死带参定时器 */
        return cpss_timer_remove_para_timer(ulTimerId);
    }
    else if ((pstTMCB->bIsInUse) && (NULL_PARA == pstTMCB->ulTimerParam)) 
    {
        /* 杀死无参定时器 */
        return cpss_timer_remove_no_para_timer(ulTimerId);
    }
    else /* 错误的定时器 */
    {        
        return CPSS_ERROR;
    }      
    
}

/*******************************************************************************
* 函数名称: cpss_timer_kill_timer
* 功    能: 利用定时器参数和定时器号杀死定时器
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ucTimerNo         UINT8                IN              定时器号
* ulParam           UINT32              IN              参数
* 函数返回: 成功    CPSS_OK
*          失败    CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_kill_timer
(
 UINT8 ucTimerNo,
 UINT32 ulParam
 )
{
    CPSS_VK_PROC_PCB_T   *pstPCB = NULL;
    UINT32  ulTimerId;
    UINT32  ulLoopVar;
    UINT32  ulNextTid;
    CPSS_TM_TMCB_T  *pstTMCB;
    CPSS_TM_TMCB_T  *pstFirstTMCB;
    UINT8 bResult = FALSE;
    UINT32 ulPno;
    
    /* 检查参数。TimerNo >= PROC_TIMER_NUM，错误的定时器 */    
    if (ucTimerNo >= MAX_PROC_TIMER) 
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n timer no overflow in cpss_timer_kill_timer\n");        
        return CPSS_ERR_TM_INPUT_INVALID_PARA;
    }
    
    /* 获得当前进程的pcb */    
    ulPno = cpss_vk_pd_self_get();
    /* cpss_vk_proc_pcb_get( ulPno,pstPCB);*/
    pstPCB = cpss_vk_proc_pcb_get(ulPno);
    if (pstPCB == NULL)
    {        
        return CPSS_ERR_TM_INPUT_INVALID_PARA;
    }    
    if (ulParam == NULL_PARA) /* 杀死无参定时器 */
    {
        ulTimerId = pstPCB->aulTimerId[ucTimerNo];
        if (ulTimerId >= MAX_TIMER_NUM)
        {            
            return CPSS_ERR_TM_NOT_IN_PCB;
        }        
        return cpss_timer_remove_no_para_timer(ulTimerId);
    } 
    else    /* 杀死带参定时器 */
    {
        ulTimerId = pstPCB->ulParaTMCBHead;      
        if (ulTimerId >= MAX_TIMER_NUM)
        {            
            return CPSS_ERR_TM_NOT_IN_PCB;
        }
        
        pstFirstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[0]);       
        /* 在进程控制块中查找要杀死的定时器 */
        for (ulLoopVar = 0;ulLoopVar < MAX_TIMER_NUM;ulLoopVar ++)
        {    
            pstTMCB    = pstFirstTMCB + ulTimerId;
            ulNextTid = pstTMCB->ulNextParamTid;
            /* 找到 */
            if (pstTMCB->ucTimerNo == ucTimerNo && pstTMCB->ulTimerParam == ulParam)
            {
                bResult = TRUE;
                break;
            }
            
            /* 只有一个定时器，但不是要杀死的定时器 */
            if (ulTimerId == ulNextTid) 
            {
                bResult = FALSE;
                break;
            }
            
            ulTimerId = ulNextTid;            
            /* 错误情况 */
            if (ulTimerId >= MAX_TIMER_NUM) 
            {
                bResult = FALSE;
                break;
            }
        }/* end for (ulLoopVar... */
        
        /* 没有找到要杀死的定时器 */
        if (ulLoopVar >=  MAX_TIMER_NUM || TRUE != bResult)
        {
            return CPSS_ERR_TM_NOT_IN_PCB;
        }        
        /* 杀死已经找到的定时器 */  
        return cpss_timer_remove_para_timer(ulTimerId);
    }
    
}

/*******************************************************************************
* 函数名称: cpss_timer_set_loop_timer
* 功    能: 设置循环定时器
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ucTimerNo         UINT8                IN              定时器号
* ulParam           UINT32              IN              参数
* lDuration         INT32                IN             延迟时间
* 函数返回: 成功    定时器标示
*          失败    CPSS_TD_INVALID
* 说    明:                                 
*******************************************************************************/
UINT32     cpss_timer_set_loop_timer
(
 UINT8 ucTimerNo, 
 INT32 lDuration,
 UINT32 ulParam
 )
{
    UINT32   ulTickSum =0;
    UINT32   ulTid;
    UINT32   ulSec;
    UINT32   ulMs;    
    
    /* 检查输入参数 */
    
    if (ucTimerNo >= MAX_PROC_TIMER)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n timer no overflow in cpss_timer_set_loop_timer\n");        
        return CPSS_TD_INVALID;
    }
    /* 将时长转换成TICK */
    if (lDuration >= 0 && lDuration <= g_usCpssTmDurationOfTick) /* 如果时长小于等于1个TICK，置TICK数为1 */
    { 
        ulTickSum = 1;
    }
    else if (lDuration < 0) /* 设置预定义定时器 */
    {
        return CPSS_TD_INVALID;
    }
    else /* 一般情况 */
    {
      /*  ulSec  = lDuration / 1000; / 秒数 */
	  ulSec  = CPSS_DIV(lDuration , 1000); 
      /*  ulMs   = lDuration % 1000; / 剩余的毫秒数 */
	 ulMs   = CPSS_MOD(lDuration ,1000); 
        /* 四舍五入 */  
       /* ulTickSum = ulSec * g_usCpssTmClockRate
            + (ulMs * g_usCpssTmClockRate + 500) / 1000;        */
        ulTickSum = ulSec * g_usCpssTmClockRate
            + CPSS_DIV(ulMs * g_usCpssTmClockRate + 500 , 1000);   
            
    }
    
    if (ulParam == NULL_PARA) 
    {
        /* 设置无参定时器 */
        ulTid = cpss_timer_set_no_para_timer(ucTimerNo,LOOP_TIMER,ulTickSum);
    }
    else   
    {  
        /* 设置带参定时器 */
        ulTid = cpss_timer_set_para_timer(ucTimerNo,LOOP_TIMER,ulTickSum,ulParam);
    }
    
    return ulTid; 
    
}

/*******************************************************************************
* 函数名称: cpss_timer_loop_timer_restart
* 功    能: 重设循环定时器
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTimerId         UINT32               IN              定时器标示
* 函数返回: 成功    CPSS_OK
*          失败    CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_loop_timer_restart(UINT32 ulTimerId)
{
    CPSS_TM_TMCB_T  *pstTMCB; 
    
    /* 设置该定时器控制块的指针 */
    pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTimerId]);     
    /* 获取互斥信号量 */
    cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTimerQueue, WAIT_FOREVER);
    /* 将循环定时器的消息置为NULL */
    pstTMCB->ulMsgHeadAddr = 0;    
    /* 置定时器超时标志为FALSE */
    pstTMCB->bIsTimeOut = FALSE;    
    /* 把循环定时器加到相对定时器队列 */
    cpss_timer_add_relative_timer(ulTimerId);     
    /* 释放互斥信号量 */
    cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTimerQueue);
    return CPSS_OK;      
}

/*******************************************************************************
* 函数名称: cpss_timer_pcb_clean_all_timer
* 功    能: 清除某进程中的所有定时器
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* CPSS_VK_PROC_PCB_T  *pstPCB：进程控制块
* 函数返回: CPSS_OK       成功
*           CPSS_ERROR    失败
* 说    明:                                 
*******************************************************************************/
#if 0
INT32 cpss_timer_pcb_clean_all_timer(CPSS_VK_PROC_PCB_T  *pstPCB)
{
    
    INT32     lResult;
    UINT32         ulTempParamTid; 
    UINT32         ulNextParamTid;
    UINT32         ulHeadParamTid;
    UINT32         ulIdIndex;
    CPSS_TM_TMCB_T         *pstFirstTMCB;
    CPSS_TM_TMCB_T         *pstTMCB;
    
    /* 检查输入参数 */
    if (NULL == pstPCB)
    {
        
        return CPSS_ERROR;
    }
    
    pstFirstTMCB    = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[0]);
    
    /* 杀死进程中的无参定时器 */
    for (ulIdIndex = 0;ulIdIndex < MAX_PROC_TIMER;ulIdIndex++)
    {
        if (pstPCB->aulTimerId[ulIdIndex] < MAX_TIMER_NUM)
        {
            if (cpss_timer_remove_no_para_timer(pstPCB->aulTimerId[ulIdIndex]) == CPSS_ERROR)
            {
                return CPSS_ERROR;
            }
        }
    }
    
    ulHeadParamTid = pstPCB->ulParaTMCBHead;
    
    /* 带参定时器数目为 0 */
    if (ulHeadParamTid >= MAX_TIMER_NUM) 
    {        
        return CPSS_OK;
    }
    
    /* 有带参定时器 */
    lResult       = CPSS_OK;
    ulTempParamTid = ulHeadParamTid;
    
    /* 杀死进程中的带参定时器 */
    for (ulIdIndex = 0;ulIdIndex < MAX_TIMER_NUM;ulIdIndex++)
    {   
        pstTMCB         = pstFirstTMCB + ulTempParamTid;
        ulNextParamTid = pstTMCB->ulNextParamTid;
        
        /* 根据定时器标识清除定时器 */
        if (cpss_timer_remove_para_timer(ulTempParamTid) == CPSS_ERROR)
        { 
            return CPSS_ERROR;
        }
        
        /* 清除完毕 */
        if (ulTempParamTid == ulNextParamTid) 
        {
            break;
        }
        
        /* 获得下一个 */
        ulTempParamTid = ulNextParamTid;
        if (ulTempParamTid >= MAX_TIMER_NUM)
        {
            
            lResult = CPSS_ERROR;
            break;
        }
    }
    
    /* 置进程中的定时器数目为0 */
    pstPCB->usTimerCounts = 0;  
    return lResult; 
    
}
#endif

/*******************************************************************************
* 函数名称: cpss_timer_tmcb_release
* 功    能: 根据定时器消息释放控制块
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* pMsgHead       CPSS_COM_MSG_HEAD_T *   IN              定时器消息头
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
VOID cpss_timer_tmcb_release(CPSS_COM_MSG_HEAD_T *pMsgHead)
{
    UINT32                   ulTimerId; 
    CPSS_TM_TIMER_MSG_EXTEND_T  *psstTimerMsg;
    CPSS_VK_PROC_PCB_T            *pstDestPCB;
    CPSS_TM_TMCB_T                   *pstTMCB;    
    
    if (NULL != pMsgHead)
    {
        if(CPSS_TIMER_OUT_MSG!=(pMsgHead->ulMsgId&0xFFFFFF00))
        {
            return;
        }
        psstTimerMsg = (CPSS_TM_TIMER_MSG_EXTEND_T  *)(pMsgHead+1);             /* cpss_type_cast */
        ulTimerId = psstTimerMsg->ulTimerId;
        if (ulTimerId >= MAX_TIMER_NUM)
        {
            cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"  timer id overflow in  cpss_timer_tmcb_release\n\r");            
            return;
        }
        pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTimerId]); 

        if (!pstTMCB->bIsInUse)
        {         
           return ;
         }
        if (LOOP_TIMER == psstTimerMsg->ucTimerType)
        {
            cpss_timer_loop_timer_restart(ulTimerId);
        }
        else
        {               
           /* pstDestPCB =  cpss_vk_proc_pcb_get(pstTMCB->ulPno);*/
           pstDestPCB = (CPSS_VK_PROC_PCB_T            *) pstTMCB->ulPcb;
           
            if(NULL == pstDestPCB)
            {
              /*  cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"  cpss_vk_proc_pcb_get() fail in  cpss_timer_tmcb_release\n\r");  2006/06/08 李军删除不必要的信息  */         
                return;
            }
            cpss_timer_pcb_clear_timer_record(ulTimerId, pstDestPCB); 
            cpss_timer_free_tmcb(ulTimerId);
        }        
    } 
}

/*******************************************************************************
* 函数名称: cpss_timer_get_idle_tmcb_count
* 功    能: 获得空闲的定时器控制块数目，统计资源使用情况时使用
* 函数类型: INT32  
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 空闲的定时器控制块数目
* 说    明:                                 
*******************************************************************************/
#if 0
UINT32 cpss_timer_get_idle_tmcb_count(void)
{
    UINT32 ulRetVal;
    CPSS_TM_TMCBPOOLITEM_T *pstTmcbPool;
    
    /* 获得定时器控制块池的地址 */
    pstTmcbPool = (CPSS_TM_TMCBPOOLITEM_T *)&(g_pstCpssTmCoreData->stTMCBPool);
    
    if(cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTMCB, WAIT_FOREVER)==CPSS_ERROR)
    {
        ;
    }
    
    /* 得到空闲定时器控制块数目 */       
    ulRetVal = pstTmcbPool->ulFreeCount;
    
    if(cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTMCB)==CPSS_ERROR)
    {
        ;
    }
    
    return ulRetVal;
    
}

#endif

/*******************************************************************************
* 函数名称: cpss_timer_sem_give_isr
* 功    能: 每个TICK释放一次同步信号量
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
#ifdef CPSS_VOS_VXWORKS
 VOID cpss_timer_sem_give_isr(VOID)
{
    INT32 lResult;
	
    if (g_bCpssTmIsUseAuxClock != FALSE)	
    {      
         lResult = wdStart(g_CpssTmDogId, g_usCpssTmDelayTicks, (FUNCPTR)cpss_timer_sem_give_isr, 0);    
        if (CPSS_OK != lResult)
        {        
          logMsg("start WatchDog in cpss_timer_sem_give_isr function fail\n",
            0, 0, 0, 0, 0, 0);
       }
      /*  cpss_tm_hdr_int_isr();  2006/11/08 删除,改为由硬件定时器周期调用*/
        cpss_vos_sem_v(g_pstCpssTmCoreData->ulSynSemForTick);
    }
    return;
}

#endif

#ifdef CPSS_VOS_LINUX
VOID cpss_timer_sem_give_isr(VOID)
{
    cpss_vos_sem_v(g_pstCpssTmCoreData->ulSynSemForTick);
    return;
}
#endif

/*******************************************************************************
* 函数名称: cpss_timer_set_no_para_timer
* 功    能: 设置无参定时器
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ucTimerNo          UINT8                 IN            定时器号
* ucTimerType        UINT8                IN            定时器类型  
* ulTickSum          UINT32               IN            延迟的ticks
* 函数返回: 成功: 定时器标示
*           失败: CPSS_TD_INVALID
* 说    明:                                 
*******************************************************************************/
 UINT32 cpss_timer_set_no_para_timer
(
 UINT8 ucTimerNo,
 UINT8 ucTimerType,
 UINT32 ulTickSum
 )
{
    CPSS_VK_PROC_PCB_T     *pstPCB = NULL;
    CPSS_TM_TMCB_T    *pstTMCB;
    UINT32    ulTid;
    UINT32    ulNewTid;
    UINT32    ulPno;   
    INT32     lResult;
    
    /* 获得本进程的进程控制块 */    
    ulPno = cpss_vk_pd_self_get();    
    pstPCB = cpss_vk_proc_pcb_get(ulPno);
    if(NULL == pstPCB)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"  cpss_vk_proc_pcb_get fail  in  cpss_timer_set_no_para_timer\n\r");        
        return CPSS_TD_INVALID;
    }
    
    /* 获得一个可用的TMCB */
    ulNewTid = cpss_timer_get_tmcb();
    if(ulNewTid == CPSS_TD_INVALID) 
    {    
        return CPSS_TD_INVALID;
    }    
    ulTid = pstPCB->aulTimerId[ucTimerNo];    
    /* 如果该进程中已经有一个相同的定时器，则杀死原来的定时器 */
    if(ulTid != CPSS_TD_INVALID)
    {
        cpss_timer_remove_no_para_timer(ulTid);
    }    
    /* 填充定时器控制块中的信息 */
    pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulNewTid]);      
    /*  pstTMCB->ulTimerParam    = LEOF;*/
    pstTMCB->ulTimerParam    = NULL_PARA;
    pstTMCB->ucTimerNo       = ucTimerNo;
    pstTMCB->ucTimerType     = ucTimerType;
    pstTMCB->ulOriginalCount = ulTickSum;
    pstTMCB->ulCount         = ulTickSum;
    pstTMCB->bIsTimeOut      = FALSE;        
    pstTMCB->ulPno           = ulPno;    
    pstTMCB->ulPcb            = (UINT32)pstPCB;
    
    if (ucTimerType == ABS_TIMER) /* 加入绝对定时器队列 */
    {
     #ifndef CPSS_DSP_CPU
        /* 获取互斥信号量 */
        lResult = cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTimerQueue, WAIT_FOREVER);
        if(lResult != CPSS_OK)
        {
            cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"  p MutexSemForTimerQueue fail  in  cpss_timer_set_no_para_timer\n\r");            
            return CPSS_TD_INVALID;
        }        
        /* 加入到绝对定时器队列 */ 
        cpss_timer_add_abs_timer(ulNewTid);        
     #endif		
    }
    else if ((ucTimerType == LOOP_TIMER)
        ||(ucTimerType == SYN_TIMER)              
        ||(ucTimerType == RELATIVE_TIMER)) /* 加入相对定时器队列 */
    {
        /* 获取互斥信号量 */
        lResult = cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTimerQueue, WAIT_FOREVER);
        if(lResult != CPSS_OK)
        {
            cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"  p MutexSemForTimerQueue fail  in  cpss_timer_set_no_para_timer\n\r");            
            return CPSS_TD_INVALID;
        }        
        /* 加入到相对定时器队列 */
        cpss_timer_add_relative_timer(ulNewTid);
    }
    
    /* 进程中的定时器数目加1 */                    
	pstPCB->usTimerCounts = pstPCB->usTimerCounts +1;
   /* pstPCB->usTimerCounts ++;    */
	if(pstPCB->usTimerPeak < pstPCB->usTimerCounts)
	{
        pstPCB->usTimerPeak = pstPCB->usTimerCounts;
        pstPCB->ulTimerPeakGmt = cpss_gmt_get();
	}
    pstPCB->aulTimerId[ucTimerNo] = ulNewTid;
    
    /* 释放互斥信号量 */
    lResult = cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTimerQueue);
    if(lResult != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"  v MutexSemForTimerQueue fail  in  cpss_timer_set_no_para_timer\n\r");        
        return CPSS_TD_INVALID;
    }    
    return ulNewTid;    
}

/*******************************************************************************
* 函数名称: cpss_timer_set_para_timer
* 功    能: 设置带参定时器
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ucTimerNo          UINT8                 IN            定时器号
* ucTimerType        UINT8                IN            定时器类型  
* ulTickSum          UINT32               IN            延迟的ticks
* ulParam            UINT32               IN            定时器参数
* 函数返回: 
* 成功:          定时器标示
* 失败:          CPSS_TD_INVALID     
* 说    明:                                 
*******************************************************************************/
 UINT32 cpss_timer_set_para_timer
(
 UINT8 ucTimerNo,
 UINT8 ucTimerType,
 UINT32 ulTickSum,
 UINT32 ulParam
 )
{
    CPSS_VK_PROC_PCB_T     *pstPCB;
    CPSS_TM_TMCB_T    *pstTMCB;   
    UINT32    ulNewTid;
    UINT32    ulPno;  
    
    /* 获得该进程的PCB */    
    ulPno = cpss_vk_pd_self_get();    
    pstPCB = cpss_vk_proc_pcb_get(ulPno);
    if(NULL == pstPCB)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"  cpss_vk_proc_pcb_get fail  in  cpss_timer_set_para_timer\n\r");        
        return CPSS_TD_INVALID;
    }
    
    /* 申请可用的定时器控制块 */
    ulNewTid = cpss_timer_get_tmcb();
    if(ulNewTid == CPSS_TD_INVALID) 
    {    
        return CPSS_TD_INVALID;
    }
    
    /* 获得该定时器控制块的地址 */
    pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulNewTid]);
    /* 填充定时器控制块信息 */
    pstTMCB->ulTimerParam    = ulParam;
    pstTMCB->ucTimerNo       = ucTimerNo;
    pstTMCB->ucTimerType     = ucTimerType;
    pstTMCB->ulOriginalCount = ulTickSum;
    pstTMCB->ulCount         = ulTickSum;
    pstTMCB->bIsTimeOut      = FALSE;        
    pstTMCB->ulPno           = ulPno;
    
    pstTMCB->ulPcb            = (UINT32)pstPCB;
    /* 插入到平衡二杈树中 */
    cpss_timer_insert_tmcb_node(ulNewTid, ulParam, ulPno, ucTimerNo);    
    if (ucTimerType == ABS_TIMER)   /* 加入到绝对定时器队列 */
    {
     #ifndef CPSS_DSP_CPU
        /* 获取互斥信号量 */
        cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTimerQueue, WAIT_FOREVER);
        /* 加入到绝对定时器队列 */
        cpss_timer_add_abs_timer(ulNewTid);        
    #endif
    }
    else if ((ucTimerType == LOOP_TIMER)              
        ||(ucTimerType == RELATIVE_TIMER)) /* 加入到相对定时器队列 */
    {
        /* 获取互斥信号量 */
        cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTimerQueue, WAIT_FOREVER);
        /* 加入到相对定时器队列 */
        cpss_timer_add_relative_timer(ulNewTid);
    }
    
    /* 将新的定时器加入进程控制块中，该进程的定时器数目加1 */         
    cpss_timer_append_para_timer_to_pcb(ulNewTid, pstPCB);        
    pstPCB->usTimerCounts = pstPCB->usTimerCounts +1;
   /*pstPCB->usTimerCounts ++;    */
	if(pstPCB->usTimerPeak < pstPCB->usTimerCounts)
	{
        pstPCB->usTimerPeak = pstPCB->usTimerCounts;
        pstPCB->ulTimerPeakGmt = cpss_gmt_get();
	}
    /* 释放互斥信号量 */
    cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTimerQueue);
    return ulNewTid;    
}

/*******************************************************************************
* 函数名称: cpss_timer_remove_para_timer
* 功    能: 根据定时器标识杀死带参定时器
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTimerId         UINT32                 IN            定时器标示
* 函数返回: 
* 成功:          CPSS_OK
* 失败:          CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
 INT32  cpss_timer_remove_para_timer(UINT32 ulTimerId)
{    
    CPSS_VK_PROC_PCB_T   *pstPCB;
    CPSS_TM_TMCB_T          *pstTMCB;
    
    /* 获得定时器控制块指针 */
    pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTimerId]);     
    /* 获得PCB */    
 /*   pstPCB = cpss_vk_proc_pcb_get(pstTMCB->ulPno);*/
      pstPCB  =(CPSS_VK_PROC_PCB_T   *) pstTMCB->ulPcb;
    if(NULL == pstPCB)
    {        
        return CPSS_ERROR;
    }    
    /* 获取互斥信号量 */
    cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTimerQueue, WAIT_FOREVER);
    /* 判断定时器是否超时  */
    if (!pstTMCB->bIsTimeOut)  /* 没有超时 */
    {
        if (pstTMCB->ucTimerType == ABS_TIMER) 
        {
        #ifndef CPSS_DSP_CPU
            /* 从绝对定时器队列中删除绝对定时器 */
            cpss_timer_abs_queue_pick_tmcb(ulTimerId);                               
        #endif	  
        }    
        else  
        {
            /* 从相对定时器队列中删除相对定时器 */
            cpss_timer_rel_queue_pick_tmcb(ulTimerId);           
        }/* end if (pstTMCB->ucTimerType == ABS_TIMER) */
    }    
    else /* 超时 */
    {        
        cpss_ipc_msg_expire((VOID *)pstTMCB->ulMsgHeadAddr);        /* cpss_type_cast */
    }/* end if if (pstTMCB->bIsTimeOut == FALSE) */
    
    /* 释放互斥信号量 */
    cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTimerQueue);
    
    /* 该进程的定时器数目减1 */  
    pstPCB->usTimerCounts = pstPCB->usTimerCounts -1;
    /*pstPCB->usTimerCounts --;    */
    /* 从平衡二杈树中删除该定时器 */
    cpss_timer_delete_tmcb_node(pstTMCB->ulTimerParam,pstTMCB->ulPno,pstTMCB->ucTimerNo);    
    /* 从PCB中清除该定时器记录 */
    cpss_timer_pcb_delete_tmcb(ulTimerId,pstPCB);    
    /* 释放TMCB */
    return cpss_timer_free_tmcb(ulTimerId);
    
}

/*******************************************************************************
* 函数名称: cpss_timer_add_relative_timer
* 功    能: 将相对定时器队列加到相对定时器队列
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTimerId         UINT32                 IN            定时器标示
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
 VOID  cpss_timer_add_relative_timer(UINT32 ulTimerId)
{
    CPSS_TM_TMCB_T             *pstTMCB;
    CPSS_TM_RELTIMERQUEUE_T    *pstRelTQue;    
    
    /*获得定时器控制块的指针*/
    pstTMCB    = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTimerId]);  
    if (pstTMCB->ulOriginalCount > MAX_TIMER_NUM) /* 如果定时器时长 > MAX_TMCB_NUM，加入TID队列（长时队列） */
    {
        /*获得相对定时器队列的指针*/
        pstRelTQue = (CPSS_TM_RELTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stRelTimerQueue);    
        pstRelTQue->aucTidArray[ulTimerId] = TRUE;
        pstTMCB->ulTickIndex               = LEOF;
        pstRelTQue->ulElementCount = pstRelTQue->ulElementCount +1;		
       /* pstRelTQue->ulElementCount++;        */
        /* 计算定时器中实际要保存的时长 */        
        if (ulTimerId > pstRelTQue->ulCursorForTid)
        {
            pstTMCB->ulCount = pstTMCB->ulOriginalCount + MAX_TIMER_NUM 
                - (ulTimerId - pstRelTQue->ulCursorForTid);
        }
        else
        {
            pstTMCB->ulCount = pstTMCB->ulOriginalCount 
                + pstRelTQue->ulCursorForTid - ulTimerId;
        }
    }    
    else  /* 定时器时长 <= MAX_TIMER_NUM, 加入TICK队列（短时队列） */
    {
        /* 插入到TICK队列 */
        cpss_timer_tick_array_insert_tmcb(ulTimerId, FALSE); 
    }
    
    return;
}

/*******************************************************************************
* 函数名称: cpss_timer_add_abs_timer
* 功    能: 将绝对定时器加到绝对定时器队列中
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTimerId         UINT32                 IN            定时器标示
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
#ifndef CPSS_DSP_CPU
 VOID  cpss_timer_add_abs_timer(UINT32 ulTimerId)
{    
    CPSS_TM_TMCB_T             *pstTMCB;
    CPSS_TM_TMCB_T             *pstFirstTMCB;
    UINT32                     ulHead, ulTail, ulPrev, ulTMCBIndex;
    UINT32                     ulCurrentTid;
    CPSS_TM_ABSTIMERQUEUE_T    *pstAbsTQue;
    
    /*获得定时器控制块*/
    pstTMCB      = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTimerId]);    
    /*获得第一个定时器控制块*/
    pstFirstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[0]);    
    /*获得绝对定时器队列的指针*/
    pstAbsTQue   = (CPSS_TM_ABSTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stAbsTimerQueue);
    
    if (pstAbsTQue->ulElementCount <= 0)    /* 如果绝对定时器队列头部为空，则将该定时器置为绝对定时器队列的头部 */
    {
        pstAbsTQue->ulHead   = ulTimerId;
        pstAbsTQue->ulTail   = ulTimerId;
        pstTMCB->ulPrev      = ulTimerId;
        pstTMCB->ulNext      = ulTimerId;
        pstAbsTQue->ulElementCount = pstAbsTQue->ulElementCount +1;		
      /*  pstAbsTQue->ulElementCount ++;    */
        pstTMCB->ulTickIndex = LEOF;        
        return;
    }
    
    /* 定时器队列非空 */
    ulHead       = pstAbsTQue->ulHead;
    ulTail       = pstAbsTQue->ulTail;
    ulCurrentTid = pstAbsTQue->ulHead;    
    /* 根据绝对定时器的时长的大小，插入到绝对定时器队列中 */    
    for(ulTMCBIndex = 0; ulTMCBIndex < pstAbsTQue->ulElementCount; ulTMCBIndex ++)
    {
        if (pstFirstTMCB[ulCurrentTid].ulCount > pstTMCB->ulCount)
        {
            ulPrev = pstFirstTMCB[ulCurrentTid].ulPrev;
            pstFirstTMCB[ulPrev].ulNext       = ulTimerId;
            pstFirstTMCB[ulCurrentTid].ulPrev = ulTimerId;
            pstTMCB->ulPrev                   = ulPrev;
            pstTMCB->ulNext                   = ulCurrentTid;            
            if (ulCurrentTid == ulHead)
            {
                pstAbsTQue->ulHead = ulTimerId;
            }
            break;    
        }
        ulCurrentTid = pstFirstTMCB[ulCurrentTid].ulNext;        
    }/* end of for loop */
    
    /* 如果该定时器时长大于所有的绝对定时器的时长，则将其放在末尾 */        
    if(ulTMCBIndex >= pstAbsTQue->ulElementCount)  
    {
        pstTMCB->ulPrev             = ulTail;
        pstTMCB->ulNext             = ulHead;
        pstFirstTMCB[ulHead].ulPrev = ulTimerId;
        pstFirstTMCB[ulTail].ulNext = ulTimerId;
        pstAbsTQue->ulTail          = ulTimerId;
    }    
    /* 绝对定时器数目加1 */
	pstAbsTQue->ulElementCount = pstAbsTQue->ulElementCount +1;
  /*  pstAbsTQue->ulElementCount++;    */
    pstTMCB->ulTickIndex = LEOF;
    
    return;
}
#endif
/*******************************************************************************
* 函数名称: cpss_timer_rel_queue_pick_tmcb
* 功    能: 将相对定时器从相对定时器队列中摘除
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTimerId         UINT32                 IN            定时器标示
* 函数返回: 
*  成功:     CPSS_OK
*  失败:     CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
 INT32 cpss_timer_rel_queue_pick_tmcb(UINT32 ulTimerId)
{
    UINT32    ulPos;
    UINT32    ulHead;
    UINT32    ulPrev;
    UINT32    ulNext;
    CPSS_TM_TMCB_T    * pstTMCB;
    CPSS_TM_TMCB_T    * pstFirstTMCB;
    CPSS_TM_RELTIMERQUEUE_T    *pstRelTQue;
    
    /*获得定时器控制块的指针*/   
    pstTMCB       = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTimerId]);    
    /*获得第一定时器控制块的指针*/
    pstFirstTMCB  = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[0]);    
    /*获得相对定时器队列的指针*/
    pstRelTQue    = (CPSS_TM_RELTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stRelTimerQueue);    
    if (pstRelTQue->ulElementCount == 0)
    {        
        return CPSS_ERROR;
    }    
    /* 如果定时器在长时队列（TID队列） */                        
    if (pstRelTQue->aucTidArray[ulTimerId] == TRUE)
    {        
        pstRelTQue->aucTidArray[ulTimerId] = FALSE;
        pstRelTQue->ulElementCount = pstRelTQue->ulElementCount -1;
	  /* pstRelTQue->ulElementCount --;*/
        return CPSS_OK;    
    }    
    /*获得该定时器在定时器队列的位置*/
    ulPos = pstTMCB->ulTickIndex;
    if (ulPos >= MAX_TIMER_NUM) /* 位置错误  */
    {
        return CPSS_ERROR;
    }    
    /*获得该处的定时器短时队列的头部*/
    ulHead = pstRelTQue->aulTickArray[ulPos];
    
    if (ulHead >= MAX_TIMER_NUM) /* 在该处无定时器（错误情况）*/
    { 
        return CPSS_ERROR;
    }    
    ulPrev = pstFirstTMCB[ulHead].ulPrev;    
    if (ulHead == ulPrev) /* 只有一个定时器 */
    {
        if (ulHead != ulTimerId)
        {
            return CPSS_ERROR;
        }
        pstRelTQue->aulTickArray[ulPos]    = LEOF;    /* 将该处置空 */    
    }
    else /* 有多个定时器 */
    {
        if (ulHead == ulTimerId)
        {
            pstRelTQue->aulTickArray[ulPos] = pstTMCB->ulNext;
        }        
        /* 将该定时器的前后连接起来 */        
        ulPrev    = pstTMCB->ulPrev;
        ulNext    = pstTMCB->ulNext;
        pstFirstTMCB[ulPrev].ulNext    = ulNext;
        pstFirstTMCB[ulNext].ulPrev    = ulPrev;                
    }
    
    /* 清除被删除的定时器控制块中的信息 */
    pstTMCB->ulTickIndex = LEOF;
    pstTMCB->ulPrev      = LEOF;
    pstTMCB->ulNext      = LEOF;    
    /*相对定时器数目减1*/
    pstRelTQue->ulElementCount = pstRelTQue->ulElementCount -1;	
    /*pstRelTQue->ulElementCount --;*/
    
    return CPSS_OK;        
}

#ifndef CPSS_DSP_CPU
/*******************************************************************************
* 函数名称: cpss_timer_abs_queue_pick_tmcb
* 功    能: 将绝对定时器从绝对定时器队列中摘除
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTimerId         UINT32                 IN            定时器标示
* 函数返回: 成功:     CPSS_OK
*           失败:     CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
 INT32 cpss_timer_abs_queue_pick_tmcb(UINT32 ulTimerId)
{
    UINT32             ulPrev;
    UINT32             ulNext;
    CPSS_TM_TMCB_T             *pstTMCB;
    CPSS_TM_TMCB_T             *pstFirstTMCB;
    CPSS_TM_ABSTIMERQUEUE_T    *pstAbsTQue;    
    
    /* 获得该定时器控制块 */
    pstTMCB       = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTimerId]);    
    /* 获得第一个定时器控制块 */
    pstFirstTMCB  = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[0]);    
    /* 获得绝对定时器队列的指针 */
    pstAbsTQue    = (CPSS_TM_ABSTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stAbsTimerQueue);
    
    ulPrev       = pstTMCB->ulPrev;
    ulNext       = pstTMCB->ulNext;
    if (pstAbsTQue->ulElementCount > 1)/* 多个绝对定时器 */
    {
        pstFirstTMCB[ulPrev].ulNext = ulNext;
        pstFirstTMCB[ulNext].ulPrev = ulPrev;         
        if (ulTimerId == pstAbsTQue->ulTail) /* 要删除的定时器在绝对定时器队列尾部 */
        {
            pstAbsTQue->ulTail    = ulPrev;
        }
        else if(ulTimerId == pstAbsTQue->ulHead) /* 要删除的定时器在绝对定时器队列的头部 */
        {
            pstAbsTQue->ulHead    = ulNext;
        }            
        /*绝对定时器数目减1*/
	  pstAbsTQue->ulElementCount = pstAbsTQue->ulElementCount -1;	
      /*  pstAbsTQue->ulElementCount --;*/
    }
    else if (pstAbsTQue->ulElementCount == 1) /* 绝对定时器队列中只有一个绝对定时器 */
    {
        pstAbsTQue->ulHead = LEOF;
        pstAbsTQue->ulTail = LEOF;
        pstAbsTQue->ulElementCount = 0;
    }    
    else /* 绝对定时器队列中没有绝对定时器（属于错误情况） */
    {
        return CPSS_ERROR;
    }
    
    return CPSS_OK;
}
#endif
/*******************************************************************************
* 函数名称: cpss_timer_pcb_delete_tmcb
* 功    能: 将某带参定时器从其所在的PCB中删除
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTimerId         UINT32                 IN            定时器标示
* ptPcb             CPSS_VK_PROC_PCB_T *          IN            纤程控制块
* 函数返回: 
*  成功:     CPSS_OK
*  失败:     CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
 INT32 cpss_timer_pcb_delete_tmcb
(
 UINT32 ulTimerId,
 CPSS_VK_PROC_PCB_T *ptPcb
 )
{
    CPSS_TM_TMCB_T    *pstFirstTMCB;  
    CPSS_TM_TMCB_T    *pstTMCB;
    UINT32    ulPrev;
    UINT32    ulNext;
    
    /* 获得第一个定时器控制块的指针 */
    pstFirstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[0]);    
    /* 检查输入参数 */
/**
    if (ulTimerId >= MAX_TIMER_NUM)
    {
        return CPSS_ERROR;
    }    
    if (ptPcb == NULL)
    {
        return CPSS_ERROR;
    }    
**/    
    if (ptPcb->ulParaTMCBHead == LEOF) 
    { 
        return CPSS_ERROR;    
    }    
    /* 得到要删除的定时器控制块的指针 */
    pstTMCB    = pstFirstTMCB + ulTimerId;    
    /* 将该定时器的前后连接起来 */
    ulPrev    = pstTMCB->ulPrevParamTid;
    ulNext    = pstTMCB->ulNextParamTid;
    pstFirstTMCB[ulPrev].ulNextParamTid = ulNext;
    pstFirstTMCB[ulNext].ulPrevParamTid = ulPrev;
    if (ulTimerId == ptPcb->ulParaTMCBHead) /* 要删除的定时器在进程的带参定时器队列的头部，则修改头部 */
    {
        ptPcb->ulParaTMCBHead = ulNext;
    }
    if (ulTimerId == ulNext)   /* 若只有一个带参定时器，则将进程的带参定时器队列置为空 */
    {
        ptPcb->ulParaTMCBHead = LEOF;
    }
    
    return CPSS_OK;        
}

/*******************************************************************************
* 函数名称: cpss_timer_tick_array_insert_tmcb
* 功    能: 将带参定时器插入到TICK队列
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTimerId         UINT32                 IN            定时器标示
* bIsFromTidToTick  UINT8                  IN            是否新设置的定时器
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
 VOID cpss_timer_tick_array_insert_tmcb
(
 UINT32 ulTimerId,
 UINT8 bIsFromTidToTick
 )
{
    UINT32             ulPrev;
    UINT32             ulHead;
    UINT32             ulPos;
    CPSS_TM_TMCB_T             *pstFirstTMCB;
    CPSS_TM_TMCB_T             *pstTMCB;
    CPSS_TM_RELTIMERQUEUE_T    *pstRelTQue;
    
    /* 获得第一个定时器控制块的指针 */
    pstFirstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[0]);    
    /* 获得该定时器控制块的指针 */
    pstTMCB      = pstFirstTMCB + ulTimerId;    
    /* 获得相对定时器队列的指针 */
    pstRelTQue   = (CPSS_TM_RELTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stRelTimerQueue);    
    /* 计算该定时器要插入TICK队列的位置 */
    ulPos       = CPSS_MOD((pstRelTQue->ulCursorForTick + pstTMCB->ulCount) , MAX_TIMER_NUM); 	
    /*ulPos       = (pstRelTQue->ulCursorForTick + pstTMCB->ulCount) % MAX_TIMER_NUM;*/
    
    /* 得到应插入位置的短时队列的头部 */        
    ulHead      = pstRelTQue->aulTickArray[ulPos];
    if (ulHead >= MAX_TIMER_NUM) /* 若是空队列，则将其作为头部 */
    {
        pstRelTQue->aulTickArray[ulPos] = ulTimerId;
        pstTMCB->ulPrev                 = ulTimerId;
        pstTMCB->ulNext                 = ulTimerId;         
    }
    else /* 若非空队列，挂在循环队列尾部 */
    {
        ulPrev         = pstFirstTMCB[ulHead].ulPrev;  /* 指向循环队列尾部*/
        pstTMCB->ulNext = ulHead;                /*新插入的next指向原头部*/
        pstTMCB->ulPrev = ulPrev;                /*新插入的prev指向原尾部*/
        pstFirstTMCB[ulPrev].ulNext = ulTimerId; /*原尾部的next指向新节点*/
        pstFirstTMCB[ulHead].ulPrev = ulTimerId; /*原头部的prev指向新节点*/      
    }
    
    /* 在定时器控制块中填入该定时器所在的位置 */
    pstFirstTMCB[ulTimerId].ulTickIndex    = ulPos;    
    /* 是否是新设置的定时器，若是，则将相对定时器数加1 */
    if (bIsFromTidToTick == FALSE)  /*如果不是从Tid队列搬过来的*/
    {
        pstRelTQue->ulElementCount = pstRelTQue->ulElementCount +1;
        /*pstRelTQue->ulElementCount ++;    / 统计++*/
    }
    
    return;    
}

/*******************************************************************************
* 函数名称: cpss_timer_append_para_timer_to_pcb
* 功    能: 将带参定时器加到PCB中
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTimerId         UINT32                 IN            定时器标示
* pstPCB           CPSS_VK_PROC_PCB_T *            IN            纤程控制块
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
 VOID cpss_timer_append_para_timer_to_pcb
(
 UINT32 ulTimerId, 
 CPSS_VK_PROC_PCB_T *pstPCB
 )
{
    CPSS_TM_TMCB_T      *pstTMCB;
    CPSS_TM_TMCB_T      *pstFirstTMCB;
    UINT32      ulHead;
    UINT32      ulTail;
    
    /* 获得第一个定时器控制块 */
    pstFirstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[0]);    
    /* 获得该定时器控制块的指针 */
    pstTMCB      = pstFirstTMCB + ulTimerId;    
    /* 获得PCB中带参定时器队列的头部 */
    ulHead      = pstPCB->ulParaTMCBHead;    
    if (ulHead == LEOF)  /* 若PCB中带参定时器数目为0，则将该定时器置为头部 */ 
    {
        pstPCB->ulParaTMCBHead  = ulTimerId;
        pstTMCB->ulPrevParamTid = ulTimerId;
        pstTMCB->ulNextParamTid = ulTimerId;
    }
    else  /* 若PCB中定时器数目非0，则将定时器放在尾部 */
    {
        ulTail = pstFirstTMCB[ulHead].ulPrevParamTid;
        pstFirstTMCB[ulTail].ulNextParamTid = ulTimerId;
        pstFirstTMCB[ulHead].ulPrevParamTid = ulTimerId;
        pstTMCB->ulNextParamTid             = ulHead;
        pstTMCB->ulPrevParamTid             = ulTail;
    }    
    
    return;    
}

#ifndef CPSS_DSP_CPU
/*******************************************************************************
* 函数名称: cpss_timer_abs_timer_scan
* 功    能: 扫描绝对定时器队列
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulSeconds         UINT32                 IN            当前系统时间
* 函数返回: 
*  成功:   CPSS_OK
*  失败:   CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
 INT32 cpss_timer_abs_timer_scan(UINT32 ulSeconds)
{
    CPSS_TM_ABSTIMERQUEUE_T   *pstAbsTQue;
    CPSS_TM_TMCB_T            *pstFirstTMCB;
    CPSS_TM_TMCB_T            *pstTMCB;    
    IPC_MSG_HDR_T     *pstHdr;
    CPSS_TM_TIMER_MSG_EXTEND_T  *psstTimerMsg;
    CPSS_COM_MSG_HEAD_T *    pstData;
    UINT16            usMsgSize;    
    UINT32            ulPrev;
    UINT32            ulNext;
    UINT32            ulHead;
    UINT32            ulLoopVar;
    INT32             lResult;
    
    /* 获得绝对定时器队列的头部 */
    pstAbsTQue        = (CPSS_TM_ABSTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stAbsTimerQueue);    
    /* 获得第一个定时器控制块的指针 */
    pstFirstTMCB      = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[0]);    
    /* 定时器消息的总长度 */    
    usMsgSize         = sizeof(CPSS_TIMER_MSG_T);
    /* 获取互斥信号量 */
    lResult = cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTimerQueue, WAIT_FOREVER);
    if(lResult != CPSS_OK)
    {
        return CPSS_ERROR;
    }    
    /* 扫描绝对定时器队列 */
    for (ulLoopVar = 0; ulLoopVar < MAX_TIMER_NUM; ulLoopVar ++)
    {
        if (pstAbsTQue->ulElementCount == 0) /* 没有绝对定时器，则退出 */
        {
            break;
        }
        ulHead = pstAbsTQue->ulHead;
        pstTMCB = pstFirstTMCB + ulHead;        
        /* 判断定时器的时间是否大于当前时间 */
        if(pstTMCB->ulCount > ulSeconds) /* 若无超时定时器，则退出循环 */
        {
            break;
        }        
        /* 生成定时器消息 */        
        pstHdr= (IPC_MSG_HDR_T *)cpss_ipc_msg_alloc (sizeof(CPSS_TM_TIMER_MSG_EXTEND_T)+ sizeof(CPSS_COM_MSG_HEAD_T));
        if (pstHdr == NULL) /* 申请消息内存失败，退出循环 */
        {
            break;
        }        
        /* 申请消息内存成功，填充消息 */
        cpss_mem_memset ((void *) pstHdr, 0, sizeof (IPC_MSG_HDR_T));
        pstHdr->ptNext = NULL;
        pstHdr->lTimeout = NO_WAIT;       
        pstData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (pstHdr);
        cpss_mem_memset ((void *) pstData, 0, sizeof (CPSS_COM_MSG_HEAD_T));        
        psstTimerMsg = (CPSS_TM_TIMER_MSG_EXTEND_T *)(pstData+1);
        
        /* 2006/05/19 李军修改*/
        psstTimerMsg->ulPara = pstTMCB->ulTimerParam;
      /*  psstTimerMsg->ulPara =  ((pstTMCB->ulTimerParam == NULL_PARA)? 0:pstTMCB->ulTimerParam);              */
        psstTimerMsg->ulTimerId    = ulHead;
        psstTimerMsg->ucTimerType  = pstTMCB->ucTimerType;   
        pstData->stDstProc.ulPd = pstTMCB->ulPno;
        pstData->stSrcProc.ulPd = pstTMCB->ulPno;;
        pstData->ulMsgId = CPSS_TIMER_OUT_MSG + pstTMCB->ucTimerNo;
        pstData->ulLen = sizeof(CPSS_TIMER_MSG_T);
        pstData->ucPriFlag = VOS_MSG_PRI_NORMAL;
        pstData->pucBuf = (UINT8 *)psstTimerMsg;
        pstData->ucShareFlag = CPSS_COM_MEM_SHARE;
        pstTMCB->ulMsgHeadAddr = (UINT32)pstHdr;
        
        lResult = cpss_com_send_local(pstData);
        if (CPSS_OK != lResult)
        {            
            pstTMCB->ulMsgHeadAddr = 0;
            cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FAIL,
                "CPSS_COM: send_timer_out_msg failed!\n");            
            break;
        }        
        
        /* 发送消息成功，则从绝对定时器队列中删除该定时器，设置下一个定时器为绝对定时器队列的头部 
        若绝对定时器数为0，则置绝对定时器队列的头尾为空*/
       pstAbsTQue->ulElementCount = pstAbsTQue->ulElementCount -1;
       /* pstAbsTQue->ulElementCount --;*/
        if (pstAbsTQue->ulElementCount == 0)  /* 定时器数目为0 */
        {
            pstAbsTQue->ulHead  = LEOF;
            pstAbsTQue->ulTail  = LEOF;
        }
        else   /* 定时器数目非0。摘除当前定时器，置下个定时器为队列头 */
        {
            ulPrev = pstTMCB->ulPrev;
            ulNext = pstTMCB->ulNext;            
            pstFirstTMCB[ulPrev].ulNext  = ulNext;
            pstFirstTMCB[ulNext].ulPrev  = ulPrev;
            pstAbsTQue->ulHead = ulNext;            
        }
        pstTMCB->bIsTimeOut = TRUE;
    }/* end for(ulLoopVar....) */
    
    /* 释放互斥信号量 */
    lResult = cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTimerQueue);
    if(lResult != CPSS_OK)
    {
        return CPSS_ERROR;
    }
    
    return CPSS_OK;    
}
#endif
/*******************************************************************************
* 函数名称: cpss_timer_relative_timer_scan
* 功    能: 扫描相对定时器队列
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
 VOID cpss_timer_relative_timer_scan(VOID)
{        
    UINT32             ulLoopVar;
    CPSS_TM_TMCB_T             *pstTMCB;
    CPSS_TM_RELTIMERQUEUE_T    *pstRelTQue;
    INT32              lResult;
    
    /* 获得相对定时器队列的头部 */
    pstRelTQue    = (CPSS_TM_RELTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stRelTimerQueue);    
    /* 获取互斥信号量 */
    lResult = cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTimerQueue, WAIT_FOREVER);
    if(lResult != CPSS_OK)
    {
        return;
    }    
    /* 扫描TICK（短时）队列 */
    cpss_timer_scan_tick_array();     
    /* 扫描TID（长时）队列 */
	 pstRelTQue->ulCursorForTid = CPSS_MOD((pstRelTQue->ulCursorForTid + 1), MAX_TIMER_NUM);
   /* pstRelTQue->ulCursorForTid = (pstRelTQue->ulCursorForTid + 1) % MAX_TIMER_NUM;*/
    ulLoopVar                  = pstRelTQue->ulCursorForTid;    
    /* 如果TID队列的当前位置有定时器 */
    if (pstRelTQue->aucTidArray[ulLoopVar] == TRUE)
    {
        pstTMCB    = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulLoopVar]);        
        /* 该定时器的时长减MAX_TMCB_NUM */
        pstTMCB->ulCount -= MAX_TIMER_NUM;         
        /* 如果定时器的当前时间已经小于等于MAX_TMCB_NUM，则将其插入到TICK队列 */
        if (pstTMCB->ulCount <= MAX_TIMER_NUM)
        {
            pstRelTQue->aucTidArray[ulLoopVar] = FALSE;            
            /* 插入到短时队列 */
            cpss_timer_tick_array_insert_tmcb(ulLoopVar, TRUE);
        }            
    }    
    /* 释放互斥信号量 */
    lResult = cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTimerQueue);
    if(lResult != CPSS_OK)
    {
        return;
    }    
    return;    
}

/*******************************************************************************
* 函数名称: cpss_timer_tick_array_send_tm_msg
* 功    能: 将定时器超时消息发到相应的调度任务
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulHeadTid         UINT32                 IN           当前TICK队列头
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
 VOID cpss_timer_tick_array_send_tm_msg(UINT32 ulHeadTid)
{
    
    IPC_MSG_HDR_T             *pstHdr;
    CPSS_TM_TIMER_MSG_EXTEND_T   *psstTimerMsg;
    CPSS_COM_MSG_HEAD_T *     pstData;
    /*    CPSS_VK_PROC_PCB_T *pstPCB;*/
    UINT32            ulCurrentTickHead;
    UINT32            ulTickHead;
    UINT32            ulTickTail;
    UINT32            ulNextTickHead;
    UINT32            ulNextTickTail;
    UINT32            ulMaxCount;
    UINT32            ulNextTickIndex;
    UINT32            ulLoopVar;
    UINT32            ulTempTickTid;
    CPSS_TM_RELTIMERQUEUE_T   *pstRelTimerQueue;
    CPSS_TM_TMCB_T            *pstTMCB;
    CPSS_TM_TMCB_T            *pstFirstTMCB;
    UINT32            ulTno;
    
    
    /* 获得相对定时器队列的头部 */
    pstRelTimerQueue = (CPSS_TM_RELTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stRelTimerQueue);    
    /* 获得第一个定时器控制块的指针 */
    pstFirstTMCB     = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[0]);    
    /* 相对定时器的数目 */
    ulMaxCount      = pstRelTimerQueue->ulElementCount;
    /*  usMsgSize        = sizeof(T_TimerMsg);*/    
    /* TICK队列的当前位置的短时队列的头部 */
    ulCurrentTickHead = ulHeadTid;
    ulTickHead = ulHeadTid;    
    /* TICK队列的当前位置的短时队列的尾部 */
    ulTickTail = pstFirstTMCB[ulHeadTid].ulPrev;    
    /* 将当前的短时队列的定时器消息按照调度任务连接起来 */
    for (ulLoopVar = 0; ulLoopVar < ulMaxCount; ulLoopVar ++)
    {
        pstHdr = (IPC_MSG_HDR_T *)cpss_ipc_msg_alloc (sizeof (CPSS_TM_TIMER_MSG_EXTEND_T)+ sizeof(CPSS_COM_MSG_HEAD_T));        
        /* 如果申请消息内存失败，则将剩余的定时器挪到下一个TICK的短时队列的头部，退出循环 */
        if (pstHdr == NULL)
        {  
            /* 得到下一个TICK的TICK队列的位置 */
          /*  ulNextTickIndex = (pstRelTimerQueue->ulCursorForTick + 1) % MAX_TIMER_NUM;            */
            ulNextTickIndex = CPSS_MOD((pstRelTimerQueue->ulCursorForTick + 1) , MAX_TIMER_NUM);      			
            /* 修改当前定时器控制块中的位置信息为下一个TICK */
            pstFirstTMCB[ulTickHead].ulTickIndex = ulNextTickIndex;
            ulTempTickTid = ulTickHead;            
            /* 修改剩余的定时器控制块中的位置信息为下一个TICK */
            while (ulTempTickTid != ulTickTail)
            { 
                /* 获取下个定时器 */
                ulTempTickTid = pstFirstTMCB[ulTempTickTid].ulNext;
                pstFirstTMCB[ulTempTickTid].ulTickIndex = ulNextTickIndex;
            }   
            
            /* 获得下一个TICK的短时定时器队列在相对定时器队列所处的位置 */
            ulNextTickHead = pstRelTimerQueue->aulTickArray[ulNextTickIndex];            
            /* 将剩余的定时器加大下一个短时队列的头部 */
            if (ulNextTickHead >= MAX_TIMER_NUM) 
            {
                /* 下一个短时队列为空，则将剩余的定时器作为下一个短时队列 */
                pstRelTimerQueue->aulTickArray[ulNextTickIndex] = ulTickHead;
                pstFirstTMCB[ulTickHead].ulPrev = ulTickTail;
                pstFirstTMCB[ulTickTail].ulNext = ulTickHead;
                break;
            } 
            else 
            { 
                /* 下一个短时队列非空，将剩余定时器挂到队列头部，以保证时序 */
                ulNextTickTail = pstFirstTMCB[ulNextTickHead].ulPrev;  
                pstFirstTMCB[ulTickHead].ulPrev     = ulNextTickTail;
                pstFirstTMCB[ulTickTail].ulNext     = ulNextTickHead;
                pstFirstTMCB[ulNextTickHead].ulPrev = ulTickTail;
                pstFirstTMCB[ulNextTickTail].ulNext = ulTickHead;
                pstRelTimerQueue->aulTickArray[ulNextTickIndex] = ulTickHead;
                break;                         
            }            
        } /* end if (ptMsg == NULL) */
        
        /* 申请消息内存成功，将相对定时器数目减1 */
	 pstRelTimerQueue->ulElementCount = pstRelTimerQueue->ulElementCount -1;	
       /* pstRelTimerQueue->ulElementCount    --;*/
        pstTMCB = (CPSS_TM_TMCB_T*) (&g_pstCpssTmCoreData->astTMCB[ulTickHead]);        
        /* 置超时标志为TRUE */
        pstTMCB->bIsTimeOut    = TRUE;
        pstTMCB->ulTickIndex   = LEOF;        
        /* 构造设置定时器的进程的PID */
        
        ulTno = cpss_vk_proc_sched_get(pstTMCB->ulPno);
        cpss_mem_memset ((void *) pstHdr, 0, sizeof (IPC_MSG_HDR_T));
        pstHdr->ptNext = NULL;
        pstHdr->lTimeout = NO_WAIT;        
        pstData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (pstHdr);
        cpss_mem_memset ((void *) pstData, 0, sizeof (CPSS_COM_MSG_HEAD_T));        
        psstTimerMsg = (CPSS_TM_TIMER_MSG_EXTEND_T *)(pstData+1);        
        psstTimerMsg->ulPara = pstTMCB->ulTimerParam;

       /* psstTimerMsg->ulPara =  ((pstTMCB->ulTimerParam == NULL_PARA)? 0:pstTMCB->ulTimerParam);*/
        psstTimerMsg->ulTimerId    = ulTickHead;
        psstTimerMsg->ucTimerType  = pstTMCB->ucTimerType;        

        cpss_com_logic_addr_get( &pstData->stDstProc.stLogicAddr,&pstData->stDstProc.ulAddrFlag );
        cpss_com_logic_addr_get( &pstData->stSrcProc.stLogicAddr,&pstData->stSrcProc.ulAddrFlag );   

	    pstData->stDstProc.ulPd = pstTMCB->ulPno;
        pstData->stSrcProc.ulPd = pstTMCB->ulPno;
        pstData->ulMsgId = CPSS_TIMER_OUT_MSG + pstTMCB->ucTimerNo;
        pstData->ulLen = sizeof(CPSS_TIMER_MSG_T);   /* 提供给应用的结构是G1接口中*/
        pstData->ucPriFlag = VOS_MSG_PRI_NORMAL;
        pstData->pucBuf = (UINT8 *)psstTimerMsg;
        pstData->ucShareFlag = CPSS_COM_MEM_SHARE;
        pstTMCB->ulMsgHeadAddr = (UINT32)pstHdr;
#if 0
        lResult = cpss_com_send_local(pstData);
        if (CPSS_OK != lResult)
        {
            pstTMCB->ulMsgHeadAddr = 0;
            cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FAIL,
                "CPSS_COM: send_timer_out_msg failed!\n");
        }        
#endif
           /* 将定时器消息按照调度任务串起来，放在缓冲区中 */
        if (g_astCpssTmMsgBuffer[ulTno].ptMsgHead == NULL)
        {
            g_astCpssTmMsgBuffer[ulTno].ptMsgHead = pstHdr;
            g_astCpssTmMsgBuffer[ulTno].ptMsgTail = pstHdr;
        }
        else
        {
            (g_astCpssTmMsgBuffer[ulTno].ptMsgTail)->ptNext = pstHdr;
            g_astCpssTmMsgBuffer[ulTno].ptMsgTail = pstHdr;
        }
        ulTickHead = pstTMCB->ulNext;        
        if (ulTickHead == ulCurrentTickHead) 
        {            
            break;
        }           
    }/*  end for   */
	    /* 将串好的定时器消息发送到调度任务 */
    for (ulLoopVar = 0; ulLoopVar < VK_MAX_SCHED_NUM; ulLoopVar ++)
    {
        if (g_astCpssTmMsgBuffer[ulLoopVar].ptMsgHead != NULL)
        {

          if(cpss_ipc_msg_send(g_astCpssTmMsgBuffer[ulLoopVar].ptMsgHead,IPC_MSG_TYPE_REMOTE)!=CPSS_ERROR)
      
            {
              
                g_astCpssTmMsgBuffer[ulLoopVar].ptMsgHead = NULL;
                g_astCpssTmMsgBuffer[ulLoopVar].ptMsgTail = NULL;
            }
                        
        } 
    }/* end for */
    return;    
}

/*******************************************************************************
* 函数名称: cpss_timer_scan_tick_array
* 功    能: 扫描TICK队列
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
 VOID cpss_timer_scan_tick_array(VOID)
{
    UINT32          ulHead;
    UINT32          ulLoopVar;
    CPSS_TM_RELTIMERQUEUE_T *pstRelTQue;
    
    /* 获得相对定时器队列的头部 */
    pstRelTQue = (CPSS_TM_RELTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stRelTimerQueue);    
    /* 获得要扫描的短时队列所处的位置 */
/*    pstRelTQue->ulCursorForTick = (pstRelTQue->ulCursorForTick + 1) % MAX_TIMER_NUM;    */
    pstRelTQue->ulCursorForTick = CPSS_MOD((pstRelTQue->ulCursorForTick + 1) , MAX_TIMER_NUM);  	
    ulLoopVar   = pstRelTQue->ulCursorForTick;    
    ulHead     = pstRelTQue->aulTickArray[ulLoopVar];    
    /* 没有定时器则返回 */
    if (ulHead >= MAX_TIMER_NUM)
    {
        return ;
    }    
    /* 发送超时消息 */
    cpss_timer_tick_array_send_tm_msg(ulHead);    
    /* 将该处短时队列清空 */
    pstRelTQue->aulTickArray[ulLoopVar]    = LEOF;
    
    return ;
}

/*******************************************************************************
* 函数名称: cpss_timer_insert_tmcb_node
* 功    能: 将带参定时器插入到平衡二叉树中，对于重复设置的定时器，替换掉
*           已经存在的。
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTimerTid        UINT32               IN              定时器标示
* ulTimerParam      UINT32               IN              参数
* ulPNo             UINT32               IN              纤程标示
* ucTimerNo         UINT8                IN              定时器号
* 函数返回: 
*  成功:  CPSS_OK
*  失败:  CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_insert_tmcb_node
(
 UINT32 ulTimerTid, 
 UINT32 ulTimerParam,     
 UINT32 ulPNo,            
 UINT8   ucTimerNo        
 )
{
    /* 局部变量声明 */
    CPSS_VK_PROC_PCB_T       *pstPCB = NULL;   
    CPSS_TM_TMCB_T              *pstTMCB;
    CPSS_TM_TMCB_T              *pstNewTMCB;
    CPSS_TM_TMCB_T              *pstmpTMCB;
#if 0
    T_TimerMsg          *pstTimerMsg;    
#endif
    /* IPC_MSG_HDR_T         *pstHdr;*/
    UINT32              ulTno;
    UINT32              ulAVLRoot;
    UINT32              ulFather;
    UINT32              ulCurrent;
    /* p,q,tmp,m临时变量 */
    UINT32              p,q,tmp,m;
    INT16               usCounter;    
    CPSS_TM_PARAMTIMERSTACK_T   *pstStack;
    INT32               lState;
    
    lState  = CPSS_OK; 
    /*******************************************************
    第一步:参照进程号，找到相应AVL树根 
    ********************************************************/
    
    /* cpss_vk_proc_pcb_get(ulPNo,pstPCB);*/
    pstPCB = cpss_vk_proc_pcb_get(ulPNo);

   /* ulAVLRoot = pstPCB->ulParaTMCBAVLRoot;*/
   ulAVLRoot = pstPCB->aulParaRoot[ucTimerNo];
    
    ulTno      = cpss_vk_proc_sched_get(ulPNo);
    pstNewTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTimerTid]);
    pstNewTMCB->ulRightChildTid = LEOF;
    pstNewTMCB->ulLeftChildTid  = LEOF;
    pstNewTMCB->ulNextTmrNoTid  = LEOF;
    pstNewTMCB->sBalancefactor  = 0;
    /* 获取栈的指针 */
    /* 用于存取遍历的节点相关信息 */
    pstStack             = &g_astCpssTmParamTimerStack[ulTno];
    pstStack->sStackTop = 0;
    
    /*******************************************************
    第二步:根据定时参数，在AVL树中寻找插入节点的位置 
    ********************************************************/
    /* ulFather用于存储需要调整的子树的根结点的父亲 */
    /* ulCurrent用于存储需要调整的子树的根结点 */
    /* p用于查找过程的控制变量；q用于记录p的父亲节点 */
    /* 查找结束，如果在AVL中，存在相同定时参数的定时器，
    那么p非空，指向具有相同定时参数的定时器；q是p的父亲 */
    /* 查找结束，如果在AVL中，不存在相同定时参数的定时器，
    那么p为空；q是待插入节点的位置（父亲） */
    
    ulFather = ulAVLRoot;
    ulCurrent = ulAVLRoot;
    p = ulFather;
    q = ulFather;    
    while (p != LEOF)
    {
        pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[p]);
        if (ulTimerParam == pstTMCB->ulTimerParam) 
        {
            break;
        }
        if (0 != pstTMCB->sBalancefactor)
        {
            ulFather            = q;     
            ulCurrent           = p;
            pstStack->sStackTop = 0;
        }
        q = p;
        if (ulTimerParam > pstTMCB->ulTimerParam)
        {
            pstStack->asBranch[pstStack->sStackTop]   = 1;
            pstStack->aulScanedTid[pstStack->sStackTop] = p;
            p = pstTMCB->ulRightChildTid;
        }
        else
        {    /* need add stack of tid */
            pstStack->asBranch[pstStack->sStackTop]   = -1;
            pstStack->aulScanedTid[pstStack->sStackTop] = p;
            p = pstTMCB->ulLeftChildTid;
        }
	  pstStack->sStackTop = pstStack->sStackTop +1;
       /* pstStack->sStackTop++;*/
    }
    
    /*******************************************************
    第三步:分类进行插入节点的操作 
    ********************************************************/
    /* q是上一步查找记录下的插入节点位置 */
    /* p是查找成功与否的标志；不成功为空（LEOF）*/
    /* dwTimerTid是待插入节点的Tid */
    /* 获得待插入节点的定时器控制块的指针 */
    pstNewTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTimerTid]);    
    /* 首先判断上一步是否查找到相同定时参数的定时器 */
    if (LEOF != p) 
    {        
        /* 情况：在AVL树中查找到相同定时参数的节点 */
        /* 下面的操作：根据定时器号，在相同定时参数组成的单项链表中 */
        /* 继续查找 */
        /* tmp是循环控制变量，tmp记录查找到的节点位置；m记录tmp的前趋节点 */
        /*for (tmp = p,m = tmp;LEOF != tmp; )*/
        tmp = p;
        m   = tmp;
        while (LEOF != tmp)
        {
            pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[tmp]);
            if (pstTMCB->ucTimerNo == ucTimerNo) 
            {
                break;
            }
            m = tmp;
            tmp = pstTMCB->ulNextTmrNoTid;
        }
        /* 判断上一步的查找过程是否存在相同定时器号的定时器 */
        if(LEOF != tmp)
        {
            /* 情况：存在相同定时器号的定时器 */
            /* 下面的操作：简单，只需要用新的定时器替代已有的定时器就OK */
            /* 步骤：1.根据定时器类型，从定时器扫描队列摘除； 
            2.修改定时器的链接指针                */
            pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[tmp]);
            /* 判断已存在的定时器到时否 */
            if(cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTimerQueue, 
                WAIT_FOREVER)!=CPSS_OK)
            {
                return CPSS_ERROR;
            }            
            if (!pstTMCB->bIsTimeOut)
            {
                /* 已存在的定时器尚未到时 */
                if (LOOP_TIMER == pstTMCB->ucTimerType||
                    RELATIVE_TIMER == pstTMCB->ucTimerType) 
                {
                    /* 把存在的定时器从相对定时器扫描队列摘除 */
                    lState = cpss_timer_rel_queue_pick_tmcb(tmp);                    
                }                
                if (ABS_TIMER == pstTMCB->ucTimerType) 
                {                   
                 #ifndef CPSS_DSP_CPU
                    /* 把存在的定时器从绝对定时器扫描队列中摘除 */
                    lState = cpss_timer_abs_queue_pick_tmcb(tmp);                    
		 #endif		 
                }
                /* 从队列摘除时失败的处理 */
                if (CPSS_OK != lState) 
                {   
                    cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTimerQueue);	  
                    return lState;
                }                
            }
            else
            {
                /* 已存在的定时器已经到时 */
                /* 置已经发送的到时消息为无效 */
                cpss_ipc_msg_expire((VOID *)pstTMCB->ulMsgHeadAddr);                
            }/* end if if (pstTMCB->bIsTimeOut == FALSE) */
            if(cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTimerQueue) !=CPSS_OK)
            {
                return CPSS_ERROR;
            }
            
            /* 修改定时器的指针信息 */
            /* 用已存在定时器的指针信息填充待插入定时器的相应信息 */
            /* AVL树的相关信息 */
            pstNewTMCB->ulLeftChildTid  = pstTMCB->ulLeftChildTid;
            pstNewTMCB->ulRightChildTid = pstTMCB->ulRightChildTid;
            pstNewTMCB->sBalancefactor  = pstTMCB->sBalancefactor;
            /* 定时器号链接的单向链表信息 */
            pstNewTMCB->ulNextTmrNoTid  = pstTMCB->ulNextTmrNoTid;            
            /* 更新被替换定时器的父亲的左右孩子指针 */
            /* 判断被替换定时器是否位于AVL树中 */
            if (p == tmp)
            {
                /* 在AVL树中，就要更新被替换定时器的父亲的左右孩子指针 */
                /* 如果是树根，则情况特殊 */
                if (tmp == ulAVLRoot)
                {
                    /*pstPCB->ulParaTMCBAVLRoot = ulTimerTid;*/
                     pstPCB->aulParaRoot[ucTimerNo]=ulTimerTid;
                }
                else
                {
                    pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[q]);            
                    if (pstTMCB->ulTimerParam < ulTimerParam)
                    {
                        pstTMCB->ulRightChildTid = ulTimerTid;
                    }
                    else
                    {
                        pstTMCB->ulLeftChildTid  = ulTimerTid;
                    }
                }
            }
            else
            {
                /* 不在AVL树中，则简单 */
                pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[m]);            
                pstTMCB->ulNextTmrNoTid = ulTimerTid;
            }             
            
            /* 释放被替换的定时器控制块 */
            /* 从进程控制块中清楚相应的信息 */
            cpss_timer_pcb_delete_tmcb(tmp,pstPCB);
            pstPCB->usTimerCounts = pstPCB->usTimerCounts  -1;
	     /* pstPCB->usTimerCounts --;*/
            lState = cpss_timer_free_tmcb(tmp);
            /* 这种情况下，不破坏树的平衡，不需要调整，直接返回 */            
            return lState;
        }
        else  
        {
            /* 情况：不存在相同定时器号的定时器 */
            /* 下面的操作：只需要用把新的定时器插入到链表中就OK */
            /* p记录了在AVL树中具有相同定时参数的定时器 */
            /* pstNewTMCB指向待插入定时器的定时器控制块 */
            pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[p]);
            pstNewTMCB->ulNextTmrNoTid = pstTMCB->ulNextTmrNoTid;
            pstTMCB->ulNextTmrNoTid    = ulTimerTid;            
            /* 以下操作没有必要 */
            pstNewTMCB->ulLeftChildTid  = LEOF;
            pstNewTMCB->ulRightChildTid = LEOF;
            pstNewTMCB->sBalancefactor  = 0;
            /* 此时不需要调整树的平衡，直接返回 */
            return lState;
        }
    }
    else
    {
        /* 情况：在AVL树中查找不到相同定时参数的节点 */
        /* 下面的操作：在适当的位置插入新的定时器 */
        /* q是需要插入新的定时器的位置；新的定时器将成为q的孩子 */
        /* 插入是根据q树是否空树（LEOF）进行分类插入 */        
        /* 判断q是否树根 */
        if (LEOF == q)   
        {
            /* 空树情况下，使PCB指向定时器控制块 */
            /* pstNewTMCB指向待插入定时器的定时器控制块 */            
             pstPCB->aulParaRoot[ucTimerNo]= ulTimerTid;;
            /* 设置左右孩子 */ 
            pstNewTMCB->ulLeftChildTid  = LEOF;
            pstNewTMCB->ulRightChildTid = LEOF;            
            /* 设置指向下一个相同定时器号的指针为空（LEOF） */
            pstNewTMCB->ulNextTmrNoTid  = LEOF;            
            /* 设置平衡因子 */
            pstNewTMCB->sBalancefactor  = 0; 
            /* 这种情况下，树只有一个节点，不需要调整 */
            return lState;
        }
        else 
        {
            /* 非空树情况下，使新的节点成为q的孩子 */
            /* 获得q的定时器控制块 */
            pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[q]);
            /* 改变q的左孩子或右孩子的指针 */
            /* 判断应该插入左支还是右支 */
            if (pstStack->asBranch[pstStack->sStackTop-1] > 0)  
            {
                /* 插入到右支 */ 
                pstTMCB->ulRightChildTid = ulTimerTid;
            }
            else
            {
                /* 插入到左支 */ 
                pstTMCB->ulLeftChildTid = ulTimerTid;
            }
            
            /* 设置左右孩子 */ 
            pstNewTMCB->ulRightChildTid = LEOF;
            pstNewTMCB->ulLeftChildTid  = LEOF;
            /* 设置指向下一个相同定时器号的指针为空（LEOF） */
            pstNewTMCB->ulNextTmrNoTid  = LEOF;
            /* 设置平衡因子 */
            pstNewTMCB->sBalancefactor  = 0;
        }
    }    /* end if(LEOF != p), end insetion */    
         /*******************************************************
         第四步:调整平衡因子 
         下一步操作:根据第二步查找记录下的信息，调整平衡因子
    ********************************************************/
    /* p是控制变量 */
    /* ulFather存储需要调整的子树的根结点的父亲 */
    /* ulCurrent存储需要调整的子树的根结点 */
    /* 只需要调整以ulCurrent为根的子树就OK */    
    for (usCounter = 0,p = ulCurrent;p != ulTimerTid;usCounter++ )
    {
        pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[p]);
        if (pstStack->asBranch[usCounter] > 0)  
        {
           pstTMCB->sBalancefactor = pstTMCB->sBalancefactor +1;
           /* pstTMCB->sBalancefactor++;*/
            p = pstTMCB->ulRightChildTid;
        }
        else
        {
           pstTMCB->sBalancefactor = pstTMCB->sBalancefactor -1;
           /* pstTMCB->sBalancefactor--;*/
            p = pstTMCB->ulLeftChildTid;
        }
    } /* end for */    
      /*******************************************************
      第四步:调整平衡
      分为四种情况：右插且右子树深度的平衡因子为1；
      右插且右子树深度的平衡因子为0或-1；
      左插且左子树深度的平衡因子为-1；
      左插且左子树深度的平衡因子为0或1；
    ********************************************************/
    /* ulCurrent存储需要调整的子树的根结点 */
    /* ulFather存储需要调整的子树的根结点的父亲 */
    /* tmp用于保存被调整子树的根节点 */
    
    /* 获得ulCurrent所指定时器的控制块指针 */
    pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulCurrent]);
    tmp    = ulCurrent;
    
    if (2 == pstTMCB->sBalancefactor)
    {
        /* 右插入后不平衡 */
        p = pstTMCB->ulRightChildTid;
        pstNewTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[p]);
        if (1 == pstNewTMCB->sBalancefactor)  
        {
            /* 右插且右子树深度的平衡因子为1 */
            /* 在ulCurrent节点处进行旋转就OK */        
            pstTMCB->ulRightChildTid   = pstNewTMCB->ulLeftChildTid;
            pstNewTMCB->ulLeftChildTid = ulCurrent;
            pstNewTMCB->sBalancefactor = 0;
            pstTMCB->sBalancefactor    = 0;
            /* tmp记录下旋转后树的节点 */
            tmp = p;
        }
        else
        {
            /* 右插且右子树深度的平衡因子为0或-1 */
            /* 先进行右转，然后左转 */            
            q = pstNewTMCB->ulLeftChildTid;
            pstmpTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[q]);
            pstNewTMCB->ulLeftChildTid = pstmpTMCB->ulRightChildTid;
            pstmpTMCB->ulRightChildTid = p;
            pstTMCB->ulRightChildTid   = pstmpTMCB->ulLeftChildTid;
            pstmpTMCB->ulLeftChildTid  = ulCurrent;            
            /* 更新平衡因子 */
            if(1 == pstmpTMCB->sBalancefactor)
            {
                pstNewTMCB->sBalancefactor = 0;
                pstTMCB->sBalancefactor    = -1;
            }
            else if(0 == pstmpTMCB->sBalancefactor)
            {
                pstNewTMCB->sBalancefactor = 0;
                pstTMCB->sBalancefactor    = 0;
            }
            else
            {
                pstNewTMCB->sBalancefactor = 1;
                pstTMCB->sBalancefactor = 0;
            } /* end if(1 == pstmpTMCB->sBalancefactor) */
            
            pstmpTMCB->sBalancefactor = 0;
            tmp = q;
        }
    }
    
    else if (-2 == pstTMCB->sBalancefactor) 
    {
        /* 左插入后不再平衡 */
        p = pstTMCB->ulLeftChildTid;
        pstNewTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[p]);
        if (-1 == pstNewTMCB->sBalancefactor)  
        {
            /* 左插且左子树深度的平衡因子为-1 */
            /* 在ulCurrent处进行右旋转就OK */
            pstTMCB->ulLeftChildTid     = pstNewTMCB->ulRightChildTid;
            pstNewTMCB->ulRightChildTid = ulCurrent;
            pstNewTMCB->sBalancefactor  = 0;
            pstTMCB->sBalancefactor     = 0;
            tmp = p;
        }
        else
        {
            /* 左插且左子树深度的平衡因子为0或1 */
            /* 先进行左转，然后右转 */
            q = pstNewTMCB->ulRightChildTid;
            pstmpTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[q]);
            pstNewTMCB->ulRightChildTid = pstmpTMCB->ulLeftChildTid;
            pstmpTMCB->ulLeftChildTid   = p;
            pstTMCB->ulLeftChildTid     = pstmpTMCB->ulRightChildTid;
            pstmpTMCB->ulRightChildTid  = ulCurrent;
            
            if (-1 == pstmpTMCB->sBalancefactor)
            {
                pstNewTMCB->sBalancefactor = 0;
                pstTMCB->sBalancefactor    = 1;
            }
            else if(0 == pstmpTMCB->sBalancefactor)
            {
                pstNewTMCB->sBalancefactor = 0;
                pstTMCB->sBalancefactor    = 0;
            }
            else
            {
                pstNewTMCB->sBalancefactor = -1;
                pstTMCB->sBalancefactor    = 0;
            } /* end if (-1 == pstmpTMCB->sBalancefactor) */
            
            pstmpTMCB->sBalancefactor  = 0;
            tmp = q;
        } /* end if (-1 == pstNewTMCB->sBalancefactor) */
    }
    
    else
    {
        /*  其它情况下，没有破坏平衡，不需要调整 */
    } /* end if (2 == pstTMCB->sBalancefactor) */    
    
      /*******************************************************
      第五步:结束调整
      调整PCB中的树根或者调整ulFather的孩子指针
    ********************************************************/
    /* ulCurrent存储需要调整的子树的根结点 */
    /* ulFather存储需要调整的子树的根结点的父亲 */
    if (ulCurrent == ulFather)
    {
       /* pstPCB->ulParaTMCBAVLRoot = tmp;*/
       pstPCB->aulParaRoot[ucTimerNo] =  tmp;
    }
    else
    {
        pstmpTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulFather]);
        if (pstmpTMCB->ulLeftChildTid == ulCurrent)  
        {
            pstmpTMCB->ulLeftChildTid = tmp;
        }
        else
        {
            pstmpTMCB->ulRightChildTid = tmp;
        }
        
    } /* end if (ulCurrent == ulFather) */
    
    return lState;    
} /* end  */

/*******************************************************************************
* 函数名称: cpss_timer_delete_tmcb_node
* 功    能: 从平衡二叉树中删除定时器
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTimerParam      UINT32               IN              参数
* ulPNo             UINT32               IN              纤程标示
* ucTimerNo         UINT8                IN              定时器号
* 函数返回: 
*  成功:  CPSS_OK
*  失败:  CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_delete_tmcb_node
      (
      UINT32 ulTimerParam, 
      UINT32 ulPNo, 
      UINT8 ucTimerNo
      )
  {
      /* 局部变量声明 */
      CPSS_VK_PROC_PCB_T                *pstPCB = NULL;
      CPSS_TM_TMCB_T               *pstTMCB;
      CPSS_TM_TMCB_T               *pstNewTMCB;
      CPSS_TM_TMCB_T               *pstmpTMCB;
      UINT32               ulTno;
      UINT32               ulAVLRoot;
      /* p,q,tmp,m临时变量 */
      UINT32               p,q,tmp,m;
      INT16                usCounter;
      UINT8                 k; 
      CPSS_TM_PARAMTIMERSTACK_T    *pstStack;
      INT32           lState;
      
      lState = CPSS_OK;      
      if(ucTimerNo >= MAX_PROC_TIMER)
      {
      	return CPSS_ERROR;
      }
      /*******************************************************
      第一步:参照进程号，找到相应AVL树根 
      ********************************************************/      
      /*cpss_vk_proc_pcb_get (ulPNo,pstPCB);*/
      pstPCB = cpss_vk_proc_pcb_get(ulPNo);
      ulAVLRoot = pstPCB->aulParaRoot[ucTimerNo];
      ulTno      = cpss_vk_proc_sched_get(ulPNo);
      /* 获取栈的指针 */
      /* 用于存取遍历的节点相关信息 */
      pstStack = (CPSS_TM_PARAMTIMERSTACK_T *)&g_astCpssTmParamTimerStack[ulTno];
      pstStack->sStackTop = 0;
      
      /*******************************************************
      第二步:根据定时参数，在AVL树中寻找删除节点的位置 
      ********************************************************/
      /* p用于查找过程的控制变量 */
      /* 查找结束，栈中保存了遍历过的节点的信息
      （Tid，下一步是左支还是右支） */
      /* 查找成功，p指向待删除节点；否则p为空（LEOF） */      
      /*for (p = ulAVLRoot;LEOF != p; )*/
      p = ulAVLRoot;
      while (LEOF != p)
      {
          pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[p]);          
          if (ulTimerParam == pstTMCB->ulTimerParam)  
          {
              break;
          }          
          pstStack->aulScanedTid[pstStack->sStackTop] = p;
          if (ulTimerParam > pstTMCB->ulTimerParam)  
          {
              pstStack->asBranch[pstStack->sStackTop] = 1;
              p = pstTMCB->ulRightChildTid;
          }
          else
          {
              pstStack->asBranch[pstStack->sStackTop] = -1;
              p = pstTMCB->ulLeftChildTid;
          }  
         pstStack->sStackTop = pstStack->sStackTop +1;
       /*   pstStack->sStackTop++;          */
      } /* end for (p = ulAVLRoot;LEOF != p; ) */
      
      /* 如果不能找到，返回错误 */
      if (LEOF == p)   
      {
          lState = CPSS_ERROR;          
          return lState;
      }      
      /*******************************************************
      第三步:如果在AVL树中查找到相同定时参数的节点，
      根据定时器号，在相同定时参数组成的单项链表中继续查找
      ********************************************************/
      /* tmp是循环控制变量，tmp记录查找到的节点位置 */
      /* q记录了tmp的前趋节点 */
      /* p是链表的头节点 */      
      /*for (tmp = p,q = tmp;LEOF != tmp; )*/
      tmp = p;
      q   = tmp;
      while (LEOF != tmp)
      {
          pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[tmp]);
          if (pstTMCB->ucTimerNo == ucTimerNo)  
          {
              break;
          }
          q   = tmp;
          tmp = pstTMCB->ulNextTmrNoTid;  /* q = tmp;  tmp = pstTMCB->ulNextTmrNoTid;*/
      }
      
      /* 如果不能找到，那么有错误 */
      if (LEOF == tmp)   
      {
          lState = CPSS_ERROR;          
          return lState;
      }      
      /*******************************************************
      第四步:分类，根据不同的情况，删除找到的节点
      ********************************************************/
      /* p is the first timer in the timerno liner */
      /* tmp is the deleted timer */
      /* q is the precursor of tmp in the timerno liner */      
      pstTMCB    = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[p]);
      pstNewTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[tmp]);      
      /*case 1: if the deleted timer is not the first timer in the linker  */
      if(p != tmp)  
      {
          /* remove it from the linker */
          pstmpTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[q]);
          pstmpTMCB->ulNextTmrNoTid = pstNewTMCB->ulNextTmrNoTid;
          return lState;
      }      
      /* case 2:if the deleted timer is the first timer in the linker  */
      /* and the first is not the last one */
      tmp  = pstNewTMCB->ulNextTmrNoTid;
      if (LEOF != tmp)  
      {
          /* replace it with the successive one */
          pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[tmp]);          
          /* set the balance factor and left and right child */
          pstTMCB->ulLeftChildTid  = pstNewTMCB->ulLeftChildTid;
          pstTMCB->ulRightChildTid = pstNewTMCB->ulRightChildTid;
          pstTMCB->sBalancefactor  = pstNewTMCB->sBalancefactor;          
          if (p == ulAVLRoot)
          {
             pstPCB->aulParaRoot[ucTimerNo]= tmp;
          }
          else
          {
              usCounter = (UINT8)(pstStack->sStackTop - 1);
              pstmpTMCB  =(CPSS_TM_TMCB_T *)                                          \
                  &(g_pstCpssTmCoreData->astTMCB[pstStack->aulScanedTid[usCounter]]);
              if (pstStack->asBranch[usCounter] > 0)
              {
                  pstmpTMCB->ulRightChildTid = tmp;
              }
              else
              {
                  pstmpTMCB->ulLeftChildTid  = tmp;
              }
          }
          return lState;
      }
      
      /* case 3:if the deleted timer is the first timer in the linker  */
      /* and it is the only one,we need to rebanlance the avl */
      /* p is the deleted timer */
      /* 1.remove it from the timerno liner--not necessary */
      /* 2.remove it from the param liner */
      /* 3.remove it from the AVL tree */
      /* case 3.1: p has no right child */
      if (LEOF == pstNewTMCB->ulRightChildTid)     
      {
          tmp = pstNewTMCB->ulLeftChildTid;
          /* link tmp to the possible parent */
          /* p is the AVL root */
          if (ulAVLRoot != p)  
          {
              usCounter = (UINT8)(pstStack->sStackTop - 1);
              q         = pstStack->aulScanedTid[usCounter];
              pstmpTMCB  = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[q]);
              if (pstStack->asBranch[usCounter] == 1) 
              { 
                  pstmpTMCB->ulRightChildTid = tmp;
              }
              else
              {
                  pstmpTMCB->ulLeftChildTid = tmp;
              }
          }
          else
          {
              pstPCB->aulParaRoot[ucTimerNo]= tmp;
              return lState;
          }/* end if  (ulAVLRoot != p) */
      }
      /* case 3.2:p has right child */
      else 
      {
          q = pstNewTMCB->ulRightChildTid;
          pstmpTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[q]);          
          /* p's right child q has no left child */
          if (LEOF == pstmpTMCB->ulLeftChildTid)  
          {
              pstmpTMCB->ulLeftChildTid = pstNewTMCB->ulLeftChildTid;
              pstmpTMCB->sBalancefactor = pstNewTMCB->sBalancefactor;
              if (ulAVLRoot != p)
              {
                  usCounter = (UINT8)(pstStack->sStackTop - 1);
                  tmp       = pstStack->aulScanedTid[usCounter];
                  pstTMCB    = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[tmp]);
                  if (pstStack->asBranch[usCounter] == 1)
                  {  
                      pstTMCB->ulRightChildTid = q;
                  }
                  else
                  {
                      pstTMCB->ulLeftChildTid  = q;
                  } /* end if (ulAVLRoot != p) */
              }
              else
              {
                  ulAVLRoot = q;
              } /* end if (ulAVLRoot != p) */
              usCounter = pstStack->sStackTop;
              pstStack->asBranch[usCounter]     = 1;
              pstStack->aulScanedTid[usCounter] = q;
              pstStack->sStackTop = pstStack->sStackTop +1;			  
            /*  pstStack->sStackTop++;*/
          }
          /* case 3.3:q, son of p, has left child */
          else 
          {
              k = (UINT8)pstStack->sStackTop;
              pstStack->sStackTop =pstStack->sStackTop +1;			  
            /*  pstStack->sStackTop++;*/
              /*for (tmp = q; ; )*/
              tmp = q;
              while (1)
              {
                  pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[tmp]);
                  if (LEOF == pstTMCB->ulLeftChildTid)
                  {  
                      break;
                  }
                  pstStack->aulScanedTid[pstStack->sStackTop] = tmp;
                  pstStack->asBranch[pstStack->sStackTop]     = -1;
		     pstStack->sStackTop = pstStack->sStackTop +1;		  
                  /*pstStack->sStackTop++;*/
                  tmp = pstTMCB->ulLeftChildTid;
              }  /* end for(....)  */
              
              /* now tmp is the direct sucessive one of p */
              pstTMCB    = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[tmp]);
              usCounter = (UINT8)(pstStack->sStackTop - 1);
              pstmpTMCB  = (CPSS_TM_TMCB_T *)                                         \
                  &(g_pstCpssTmCoreData->astTMCB[pstStack->aulScanedTid[usCounter]]);
              pstmpTMCB->ulLeftChildTid = pstTMCB->ulRightChildTid;
              
              /* replace the p with tmp */
              pstTMCB->ulLeftChildTid  = pstNewTMCB->ulLeftChildTid;
              pstTMCB->ulRightChildTid = pstNewTMCB->ulRightChildTid;
              pstTMCB->sBalancefactor  = pstNewTMCB->sBalancefactor;
              
              /* push the tmp into the stack */
              pstStack->aulScanedTid[k] = tmp;
              pstStack->asBranch[k]     = 1;
              
              /* link to the tree */
              /* if p is the AVL root */
              if (ulAVLRoot != p)
              {
                  usCounter = (UINT8)(k - 1);
                  q = pstStack->aulScanedTid[usCounter];
                  pstmpTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[q]);
                  if (pstStack->asBranch[usCounter] == 1)
                  {  
                      pstmpTMCB->ulRightChildTid = tmp;
                  }
                  else
                  {
                      pstmpTMCB->ulLeftChildTid  = tmp;
                  }
              }
              else
              {
                  ulAVLRoot = tmp;
              } /* end if (ulAVLRoot != p) */
              /* now tmp is the root */
              }   /* p's right child q has no left child 
          end if (LEOF == pstmpTMCB->ulLeftChildTid) */
      } /* end if (LEOF == pstNewTMCB->ulRightChildTid) */
      
      /* now finish deleting */
      /* p is the timer we will delete */
      /* step 5:update the balance factor */
      usCounter = pstStack->sStackTop;      
     usCounter	  = usCounter -1;
      /*usCounter--;*/
      while (usCounter >= 0)
      {
          tmp      = pstStack->aulScanedTid[usCounter];
          pstmpTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[tmp]);
          /* left deletion */
          if (-1 ==pstStack->asBranch[usCounter])  
          {
             pstmpTMCB->sBalancefactor = pstmpTMCB->sBalancefactor +1;
             /* pstmpTMCB->sBalancefactor++;*/
              /* case 1:donnot need to rebalance */
              if (1 == pstmpTMCB->sBalancefactor)
              {
                  break;
              }
              /* case 2:need to rebanlance */
              else if (2 == pstmpTMCB->sBalancefactor)
              {
                  q      = pstmpTMCB->ulRightChildTid;
                  pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[q]);
                  
                  if (-1 == pstTMCB->sBalancefactor)  
                  {
                      /* Rotate right at q   left at tmp in AVL tree */
                      m         = pstTMCB->ulLeftChildTid;
                      pstNewTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[m]);
                      
                      pstTMCB->ulLeftChildTid     = pstNewTMCB->ulRightChildTid;
                      pstNewTMCB->ulRightChildTid = q;
                      pstmpTMCB->ulRightChildTid  = pstNewTMCB->ulLeftChildTid;
                      pstNewTMCB->ulLeftChildTid  = tmp;
                      
                      if (1 == pstNewTMCB->sBalancefactor)
                      {
                          pstmpTMCB->sBalancefactor = -1;
                          pstTMCB->sBalancefactor   = 0;
                      }
                      else if (0 == pstNewTMCB->sBalancefactor)
                      {
                          pstmpTMCB->sBalancefactor = 0;
                          pstTMCB->sBalancefactor   = 0;
                      }
                      else
                      {
                          pstmpTMCB->sBalancefactor = 0;
                          pstTMCB->sBalancefactor   = 1;
                      } /* end if (1 == pstNewTMCB->sBalancefactor) */
                      
                      pstNewTMCB->sBalancefactor = 0;
                      if (usCounter != 0)  
                      {
                          pstNewTMCB = (CPSS_TM_TMCB_T *)                              
                              &(g_pstCpssTmCoreData->astTMCB                             
                              [pstStack->aulScanedTid[usCounter - 1]]);
                          if (pstStack->asBranch[usCounter-1] == 1 )
                          {  
                              pstNewTMCB->ulRightChildTid = m;
                          }
                          else
                          {
                              pstNewTMCB->ulLeftChildTid = m;
                          }
                      }
                      else
                      {
                          ulAVLRoot = m;
                      }
                      usCounter = usCounter -1;
                     /* usCounter--;*/
                  }
                  /* rotate left */
                  else     /* else of if (-1 == pstTMCB->sBalancefactor) */
                  {
                      pstmpTMCB->ulRightChildTid = pstTMCB->ulLeftChildTid;
                      pstTMCB->ulLeftChildTid    = tmp;
                      if (usCounter != 0)  
                      {
                          pstNewTMCB = (CPSS_TM_TMCB_T *)                              
                              &(g_pstCpssTmCoreData->astTMCB                            
                              [pstStack->aulScanedTid[usCounter - 1]]);
                          if (pstStack->asBranch[usCounter-1] == 1 ) 
                          { 
                              pstNewTMCB->ulRightChildTid = q;
                          }
                          else
                          {
                              pstNewTMCB->ulLeftChildTid = q;
                          }
                      }
                      else
                      {
                          ulAVLRoot = q;
                      } /* end if (usCounter != 0) */
                      
                      if (0 == pstTMCB->sBalancefactor)  
                      {
                          pstTMCB->sBalancefactor   = -1;
                          pstmpTMCB->sBalancefactor = 1;
                          break;
                      }
                      else
                      { 
                          pstTMCB->sBalancefactor   = 0;
                          pstmpTMCB->sBalancefactor = 0;
                      }
			 usCounter = usCounter -1;
                     /* usCounter--;*/
                  }  /* end if (-1 == pstTMCB->sBalancefactor) */
              }
              /* this loop donnot need,but cannot guarteen the next*/
              else
              {
                 usCounter = usCounter -1;
                 /* usCounter--;*/
                  continue;
              } /*if (1 == pstmpTMCB->sBalancefactor) */
        }
        /* right deletion */
        else      /* else of if (-1 ==pstStack->asBranch[usCounter]) */
        {
           pstmpTMCB->sBalancefactor = pstmpTMCB->sBalancefactor -1;
           /* pstmpTMCB->sBalancefactor--;*/
            
            /* case 1:donnot need to rebalance */
            if (-1 == pstmpTMCB->sBalancefactor)
            {
                break;
            }
            /* case 2:need to rebanlance */
            else if (-2 == pstmpTMCB->sBalancefactor)
            {
                q      = pstmpTMCB->ulLeftChildTid;
                pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[q]);
                
                if (1 == pstTMCB->sBalancefactor)  
                {
                    /* Rotate left at q   right at tmp in AVL tree */
                    m         = pstTMCB->ulRightChildTid;
                    pstNewTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[m]);
                    pstTMCB->ulRightChildTid    = pstNewTMCB->ulLeftChildTid;
                    pstNewTMCB->ulLeftChildTid  = q;
                    pstmpTMCB->ulLeftChildTid   = pstNewTMCB->ulRightChildTid;
                    pstNewTMCB->ulRightChildTid = tmp;                    
                    if (-1 == pstNewTMCB->sBalancefactor)
                    {
                        pstmpTMCB->sBalancefactor = 1;
                        pstTMCB->sBalancefactor   = 0;
                    }
                    else if (0 == pstNewTMCB->sBalancefactor)
                    {
                        pstmpTMCB->sBalancefactor = 0;
                        pstTMCB->sBalancefactor   = 0;
                    }
                    else
                    {
                        pstmpTMCB->sBalancefactor = 0;
                        pstTMCB->sBalancefactor   = -1;
                    }
                    
                    pstNewTMCB->sBalancefactor = 0;
                    
                    if (usCounter != 0)  
                    {
                        pstNewTMCB = (CPSS_TM_TMCB_T *)                                   
                            &(g_pstCpssTmCoreData->astTMCB                          
                            [pstStack->aulScanedTid[usCounter - 1]]);
                        if (pstStack->asBranch[usCounter-1] == 1 ) 
                        { 
                            pstNewTMCB->ulRightChildTid = m;
                        }
                        else
                        {
                            pstNewTMCB->ulLeftChildTid  = m;
                        }
                    }
                    else
                    {
                        ulAVLRoot = m;
                    }
		       usCounter = usCounter -1;			
                    /*usCounter--;*/
                }
                /* rotate left */
                else /*   else of if (1 == pstTMCB->sBalancefactor) */
                {
                    pstmpTMCB->ulLeftChildTid = pstTMCB->ulRightChildTid;
                    pstTMCB->ulRightChildTid  = tmp;
                    if (usCounter!=0)  
                    {
                        pstNewTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[pstStack->aulScanedTid[usCounter - 1]]);
                        if (pstStack->asBranch[usCounter-1] == 1 ) 
                        { 
                            pstNewTMCB->ulRightChildTid = q;
                        }
                        else
                        {
                            pstNewTMCB->ulLeftChildTid = q;
                        }
                    }
                    else
                    {
                        ulAVLRoot = q;
                    } /* end if (usCounter!=0)  */
                    if (0 == pstTMCB->sBalancefactor)  
                    {
                        pstTMCB->sBalancefactor   = 1;
                        pstmpTMCB->sBalancefactor = -1;
                        break;
                    }
                    else
                    {
                        pstTMCB->sBalancefactor   = 0;
                        pstmpTMCB->sBalancefactor = 0;
                    } /* end if (0 == pstTMCB->sBalancefactor) */

		      usCounter = usCounter -1;
                   /* usCounter--;*/
                }  /* end if (1 == pstTMCB->sBalancefactor) */
            } 
            /* this loop donnot need,but cannot guarteen the next*/
            else
            {
                usCounter = usCounter -1;
                /*usCounter--;*/
                continue;
            } /* end if (-1 == pstmpTMCB->sBalancefactor) */
        }/* if (-1 ==pstStack->asBranch[usCounter]) */
    } /* end while (usCounter >= 0) */
    /* step 6: finally */
    /*pstPCB->ulParaTMCBAVLRoot = ulAVLRoot;*/
    pstPCB->aulParaRoot[ucTimerNo]= ulAVLRoot;
    return lState;
}  /* end  */

#ifndef CPSS_DSP_CPU
/*******************************************************************************
* 函数名称: cpss_timer_clock2time
* 功    能: 将绝对时间转换成相对时间
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ptClock          CPSS_TM_SYSSOFTCLOCK_T *      IN              绝对时间
* pstSecMillisec    CPSS_TM_TIMER_T *              IN              相对时间
* 函数返回: 
*  成功:  CPSS_OK
*  失败:  CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_clock2time
       (
       CPSS_TM_SYSSOFTCLOCK_T *ptClock,
       CPSS_TM_TIMER_T  *pstSecMillisec
       )
   {
       UINT8       ucYear;
       UINT8       ucLeapYear = 0;
       UINT16      usDays;
       
       if ((ptClock == NULL) || (pstSecMillisec == NULL))
       {
           cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input parameter error in cpss_timer_clock2time \n");           
           return CPSS_ERR_TM_INPUT_INVALID_PARA;           
       }
       
       /* 系统软时钟在2000年1月1日0时0分0秒和2121年1月1日0时0分0秒之间 */
       if (2000 > ptClock->usSysYear 
           || ptClock->usSysYear > 2120
           || ptClock->ucSysMon > 12
           || ptClock->ucSysMon == 0
           || 23 < ptClock->ucSysHour        
           || 59 < ptClock->ucSysMin
           || 59 < ptClock->ucSysSec)
       {
           cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input field error in cpss_timer_clock2time \n");           
           return CPSS_ERR_TM_INPUT_INVALID_PARA;
       }
       
       /* 检查系统软时钟域值 */    
       if (  (( 1 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 31))
           || (( 3 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 31))
           || (( 5 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 31))           
           || (( 7 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 31))
           || (( 8 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 31))
           || ((10 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 31))
           || ((12 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 31))
           || (( 4 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 30))
           || (( 6 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 30))
           || (( 9 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 30))
           || ((11 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 30)))
       {
           cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input field error in cpss_timer_clock2time \n");           
           return CPSS_ERR_TM_INPUT_INVALID_PARA;
       }
       
       /* 检查系统软时钟域值，考虑闰年时的2月份 */
     /*  if ((ptClock->usSysYear % 4 == 0 && ptClock->usSysYear % 100 != 0)
           || (ptClock->usSysYear % 400 == 0))*/
       if ((CPSS_MOD(ptClock->usSysYear ,4) == 0 && CPSS_MOD(ptClock->usSysYear , 100) != 0)
           || (CPSS_MOD(ptClock->usSysYear , 400) == 0))    
       {
           /* 闰年时的2月份时的天数为29天 */
           if ((2 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 29))
           {
               cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input field error in cpss_timer_clock2time \n");               
               return CPSS_ERR_TM_INPUT_INVALID_PARA;
           }
       }
       else
       {
           /* 非闰年时的2月份的天数为28天 */
           if ((2 == ptClock->ucSysMon) && !(1 <= ptClock->ucSysDay && ptClock->ucSysDay <= 28))
           {
               cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input field error in cpss_timer_clock2time \n");               
               return CPSS_ERR_TM_INPUT_INVALID_PARA;
           }
       }       
       /* 毫秒域最大不能超过999毫秒 */
       if (ptClock->usMilliSec > 999)
       {
           cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input field error in cpss_timer_clock2time \n");           
           return CPSS_ERR_TM_INPUT_INVALID_PARA;
       }
       
       ucYear     = (UINT8)(ptClock->usSysYear - 2000);  /* 从2000年开始的年数 */    
      /* ucLeapYear = (UINT8)((ucYear + 3) / 4);          / 闰年数 */
	ucLeapYear = (UINT8)(CPSS_DIV(ucYear + 3 , 4));   
       if (ptClock->usSysYear >= 2100)        /* 2100年不是闰年 */
       {
           ucLeapYear = ucLeapYear -1;
           /*ucLeapYear --;*/
       }       
       /* 天数 */
       usDays    = (UINT16)(ucYear * 365 + ucLeapYear);  
       usDays    = (UINT16)(usDays + usDeltaDays[ptClock->ucSysMon - 1]);
       
       /* 是否闰年 */
/*       if (((ptClock->usSysYear % 4) == 0) && (ptClock->usSysYear != 2100))*/
	if ((CPSS_MOD(ptClock->usSysYear , 4) == 0) && (ptClock->usSysYear != 2100))
       {
           if (ptClock->ucSysMon > 2)    /* 大于2月份 */
           {
               usDays = usDays +1;
               /*usDays ++;*/
           }
       }
       usDays = (UINT16)(usDays + ptClock->ucSysDay - 1); /* 剩余天数 */       
       pstSecMillisec->ulSecond    = usDays * DAY_SECONDS + ptClock->ucSysHour * 3600 
           + ptClock->ucSysMin * 60 + ptClock->ucSysSec;/* 秒数 */       
       pstSecMillisec->usMilliSec   = ptClock->usMilliSec;
       return CPSS_OK;
       
}

/*******************************************************************************
* 函数名称: cpss_timer_time2clock
* 功    能: 将相对时间转换成绝对时间
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ptClock          CPSS_TM_SYSSOFTCLOCK_T *      IN              绝对时间
* pstSecMillisec    CPSS_TM_TIMER_T *              IN              相对时间
* 函数返回: 
*  成功:  CPSS_OK
*  失败:  CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_time2clock
(
 CPSS_TM_TIMER_T *pstSecMillisec,
 CPSS_TM_SYSSOFTCLOCK_T  *ptClock
 )
{
    UINT32     ulDays;
    UINT32     ulYearDays;
    UINT32     ulSeconds;
    UINT16     usYears;
    UINT8      bIsLeapYear;
    UINT8      ucLoopMon;    
    
    if ((pstSecMillisec == NULL) || (ptClock == NULL))
    {   
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input field error in cpss_timer_time2clock \n");        
        return CPSS_ERR_TM_INPUT_INVALID_PARA;
    }

  /*  memset(ptClock , 0, sizeof(CPSS_TM_SYSSOFTCLOCK_T));*/

    if (pstSecMillisec->usMilliSec >= 1000)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input field error in cpss_timer_time2clock \n");        
        return CPSS_ERR_TM_INPUT_INVALID_PARA;
    }
    
  /*  ulDays    = pstSecMillisec->ulSecond / DAY_SECONDS;*/
    ulDays    = CPSS_DIV(pstSecMillisec->ulSecond , DAY_SECONDS);
  /*  ulSeconds = pstSecMillisec->ulSecond % DAY_SECONDS;*/
   ulSeconds = CPSS_MOD(pstSecMillisec->ulSecond , DAY_SECONDS);
    /* 星期 */
 /*   ptClock->ucSysWeek = (UINT8)((ulDays + 6) % 7);    / 2000年1月1日是星期六 */    
    ptClock->ucSysWeek = (UINT8)(CPSS_MOD((ulDays + 6) , 7)); 	
    /* 年  */
    /* 首先计算大致的年数, 可能比实际的年数要多一年 */
  /*   usYears        = (UINT16)(ulDays / 365);
    ulYearDays    = usYears * 365 + (usYears + 3) / 4;
    */
     usYears        = (UINT16)(CPSS_DIV(ulDays , 365));
    ulYearDays    = usYears * 365 + CPSS_DIV(usYears + 3, 4);

    if (usYears >= 100)    /* 2100年不是闰年 */
    {
       ulYearDays = ulYearDays -1;
       /* ulYearDays --;*/
    }    
    if (ulYearDays > ulDays)
    {
        /* 修正年数 */
        usYears    --;
        /* 计算这些年的天数 */
       /* ulYearDays    = usYears * 365 + (usYears + 3) /4;*/
	  ulYearDays    = usYears * 365 + CPSS_DIV(usYears + 3,4);
        if (usYears >= 100)    /* 2100年不是闰年 */
        {
           ulYearDays = ulYearDays -1;
          /*  ulYearDays    --;*/
        }
    }
    
    ulYearDays           = ulDays - ulYearDays;
    ptClock->usSysYear    = (UINT16)(usYears + 2000);    
    /* 月、日 */
  /*  if (((usYears % 4) == 0) && (usYears != 100))*/
   if ((CPSS_MOD(usYears , 4) == 0) && (usYears != 100))
    {
        bIsLeapYear    = TRUE;
    }
    else
    {
        bIsLeapYear    = FALSE;
    }
    
    for (ucLoopMon = 1; ucLoopMon < 12; ucLoopMon ++)
    {
        if (bIsLeapYear && (ucLoopMon > 1))
        {
            if (ulYearDays < (UINT16)(usDeltaDays[ucLoopMon] + 1))
            {  
                break;
            }
        }
        else
        {
            if (ulYearDays < usDeltaDays[ucLoopMon])
            {
                break;
            }
        }
    }
    ulYearDays    = ulYearDays - usDeltaDays[ucLoopMon - 1];
    if (bIsLeapYear && (ucLoopMon > 2))    /* 是闰年并且大于2月份 */
    {
        ulYearDays = ulYearDays -1;
       /* ulYearDays --;*/
    }
    
    ptClock->ucSysMon    = ucLoopMon;
    ptClock->ucSysDay    = (UINT8)(ulYearDays + 1);    
    /* 时分秒 */
   /* ptClock->ucSysHour   = (UINT8)(ulSeconds / 3600);*/
    ptClock->ucSysHour   = (UINT8)(CPSS_DIV(ulSeconds , 3600));
   /* ptClock->ucSysMin    = (UINT8)((ulSeconds % 3600) / 60);
    ptClock->ucSysSec    = (UINT8)(ulSeconds % 60);    
    */
    ptClock->ucSysMin    = (UINT8)(CPSS_DIV(CPSS_MOD(ulSeconds ,3600) , 60));
    ptClock->ucSysSec    = (UINT8)(CPSS_MOD(ulSeconds , 60));
    
    /* 毫秒 */
    ptClock->usMilliSec   = (UINT16)(pstSecMillisec->usMilliSec);
    
    return CPSS_OK;    
}
#endif
/*******************************************************************************
* 函数名称: cpss_timer_get_clock_rate
* 功    能: 获得系统的每秒的TICK数
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 系统每秒的TICK数
* 说    明:                                 
*******************************************************************************/
UINT16  cpss_timer_get_clock_rate(VOID)
{
    return g_usCpssTmClockRate;
}

/*******************************************************************************
* 函数名称: cpss_timer_get_cur_tick
* 功    能: 获得自系统上电后的TICK数
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ptTick            CPSS_TM_TICK_T *               IN            系统上电后的TICK数
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_get_cur_tick(CPSS_TM_TICK_T  *ptTick)
{
    /* 检查输入参数 */
/**    
    if (ptTick == NULL)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input patatemem error in cpss_timer_get_cur_tick \n");        
        return CPSS_ERROR;
    }
**/    
/****    
    if(cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTickCounts, WAIT_FOREVER) !=CPSS_OK)
    {
        return CPSS_ERROR;
    }
****/
    
    /*获得系统自上电时的TICK数*/
    ptTick->ulTickCountHigh = g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountHigh;
    ptTick->ulTickCountLow  = g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow;
/****    
    lResult = cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTickCounts);
    if(lResult != CPSS_OK)
    {
        return CPSS_ERROR;
    }
****/    
    
    return CPSS_OK;    
}

#ifndef CPSS_DSP_CPU
/*******************************************************************************
* 函数名称:cpss_timer_set_sys_clock 
* 功    能:设置系统时钟 
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* pstNewClock      CPSS_TM_SYSSOFTCLOCK_T *         IN          以年月日时分秒表示的时间
* 函数返回: 
* 成功: CPSS_OK
* 失败: CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_set_sys_clock(CPSS_TM_SYSSOFTCLOCK_T *pstNewClock)
{
    CPSS_TM_SYSSOFTCLOCK_T    stTempClock;
    CPSS_TM_SYSSOFTCLOCK_T    *pstTempClock = &stTempClock;
    INT32             lState;
    UINT16            usDurationOfTick;
    UINT16            usMsInOneSecInFact;
#ifdef CPSS_VOS_VXWORKS
    struct timespec   stTime;
#endif
    
   /* usDurationOfTick   = (UINT16)(1000/g_usCpssTmClockRate);    */
   usDurationOfTick   = (UINT16)(CPSS_DIV(1000,g_usCpssTmClockRate));  
    /* 实际每秒钟内的毫秒数 */
    usMsInOneSecInFact = (UINT16)(usDurationOfTick * g_usCpssTmClockRate);      
    if (pstNewClock == NULL)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input patatemem error in cpss_timer_set_sys_clock \n");
        return CPSS_ERROR;
    }
    
    /* 时间应该大于等于2000年1月1日12时*/
    if (pstNewClock->usSysYear   == 2000      
        && pstNewClock->ucSysMon == 1
        && pstNewClock->ucSysDay == 1
        && pstNewClock->ucSysHour < 12)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input patatemem error in cpss_timer_set_sys_clock \n");
        return CPSS_ERROR;
    }
    
    /* 对时间的其它域值的检查放在 cpss_timer_clock2time()中 */
    if(cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock, WAIT_FOREVER)!=CPSS_OK)
    {
        return CPSS_ERROR;
    }    
    cpss_mem_memcpy((void *)pstTempClock,(void *)pstNewClock, sizeof(CPSS_TM_SYSSOFTCLOCK_T));    
    /* 把绝对时间转换成秒的形式（GREENWICH） */
    lState = cpss_timer_clock2time(pstTempClock, (CPSS_TM_TIMER_T *)(&(g_pstCpssTmCoreData->stTimeFrom2K)));    
    if (lState != CPSS_OK)
    {
        /* 错误情况下释放信号量 */
        if(cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock)!=CPSS_OK)
        {
            return CPSS_ERROR;
        }        
        return lState;
    }
    
    /* 把GREENWICH时间转换成当前时区的时间 */
    g_pstCpssTmCoreData->stTimeFrom2K.ulSecond = g_pstCpssTmCoreData->stTimeFrom2K.ulSecond ;
    /* + 30 * 60 * M_stTimeZoneInfo.swTimeZone
    + (M_tSummerTimeInfo.ucSummerTimeFlag & g_pstCpssTmCoreData->bSummerTimeIsInUse) * 3600;
    */    
#ifdef CPSS_VOS_VXWORKS
    /* 修正POSIX时钟 */
    stTime.tv_sec = g_pstCpssTmCoreData->stTimeFrom2K.ulSecond + g_ulCpssTmAnsicTimeTo2k;
    stTime.tv_nsec = 1000000 * g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec;   /* 毫秒转换成纳秒，1毫秒 ＝ 1000000纳秒 */ 
    stTime.tv_sec =  stTime.tv_sec - cpss_timer_get_gmtoffset()*3600;       /* 转为格林威治时间*/
    clock_settime(CLOCK_REALTIME, &stTime);
#endif    
    /* 将设置的毫秒转换成系统中实际要保存的毫秒 */
   /* g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec = (UINT16)((g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec * usMsInOneSecInFact) / 1000);    */
    g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec = (UINT16)(CPSS_DIV(g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec * usMsInOneSecInFact , 1000));
    if(cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock)!=CPSS_OK)
    {
        return CPSS_ERROR;
    }
    
    return CPSS_OK;    
}
#endif
/*******************************************************************************
* 函数名称: cpss_timer_get_cur_seconds
* 功    能: 获得自2000年1月1日0时0分0秒起的秒数
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 自2000年1月1日0时0分0秒起的秒数
* 说    明:                                 
*******************************************************************************/
UINT32 cpss_timer_get_cur_seconds(VOID)
{
#if 1
    return g_pstCpssTmCoreData->stTimeFrom2K.ulSecond;
#else
    UINT32  ulSeconds;
    INT32   lResult;
    lResult = cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock, WAIT_FOREVER);
    if(lResult != CPSS_OK)
    {
        ;
    }        
    /* 获得当前的秒数 */
    ulSeconds  = g_pstCpssTmCoreData->stTimeFrom2K.ulSecond;
    lResult = cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock);
    if(lResult != CPSS_OK)
    {
        ;
    }    
    return ulSeconds;    
#endif
}

#ifndef CPSS_DSP_CPU
/*******************************************************************************
* 函数名称: cpss_timer_get_sys_clock
* 功    能: 获得系统时钟
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ptSysClock      CPSS_TM_SYSSOFTCLOCK_T *        IN             系统时钟
* 函数返回: 自2000年1月1日0时0分0秒起的秒数
* 说    明:                                 
*******************************************************************************/
#define CPSS_TIMER_DURATION_OF_TICK  ((UINT16)CPSS_DIV(1000,CLOCK_RATE))

#define CPSS_TIMER_MS_IN_SECOND     (CPSS_TIMER_DURATION_OF_TICK * CLOCK_RATE)

INT32 cpss_timer_get_sys_clock(CPSS_TM_SYSSOFTCLOCK_T *ptSysClock)
{
    CPSS_TM_TIMER_T            stTime;
   /* CPSS_TM_TIMER_T            *pstTime = &stTime;
    UINT16            usDurationOfTick;
    UINT16            usMsInOneSecInFact;
    INT32             lResult;
    */
    
    #if 0
/*    usDurationOfTick   = (UINT16)(1000/g_usCpssTmClockRate);    */
     usDurationOfTick   = (UINT16)(CPSS_DIV(1000,g_usCpssTmClockRate));  
    /* 实际每秒钟内的毫秒数 */
    usMsInOneSecInFact = (UINT16)(usDurationOfTick * g_usCpssTmClockRate);      
    #endif
    
    if (ptSysClock == NULL)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input patatemem error in cpss_timer_get_sys_clock \n");        
        return CPSS_ERROR;
    } 
/*    
    if(cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock, WAIT_FOREVER)!=CPSS_OK)
    {
        return CPSS_ERROR;
    } 
*/
    
    /*获得当前的秒数*/
    stTime.ulSecond  = g_pstCpssTmCoreData->stTimeFrom2K.ulSecond;    
    /* 转换成标准的毫秒数 */
  /*  pstTime->usMilliSec = (UINT16)((g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec * 1000) / usMsInOneSecInFact);      */
    stTime.usMilliSec = (UINT16)(CPSS_DIV(g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec * 1000 , CPSS_TIMER_MS_IN_SECOND));  
/*
    lResult = cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock);
    if(lResult != CPSS_OK)
    {
        return CPSS_ERROR;
    }   
*/
    
    return cpss_timer_time2clock(&stTime, ptSysClock); 
}
#endif
/*******************************************************************************
* 函数名称: cpss_timer_set        
* 功    能: 设置无参定时器 
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ucTimerNo          UINT8                  输入           定时器号
* lDelayLength       INT32                  输入           时长
* 函数返回: 
*  成功  CPSS_OK
*  失败  CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_set
(
 UINT8 ucTimerNo,
 INT32 lDelayLength
 )
{
    if(cpss_timer_para_set(ucTimerNo, lDelayLength, NULL_PARA) 
        != CPSS_TD_INVALID)
    {
        return CPSS_OK;  
    }
    else
    {
        return CPSS_ERROR;
    }
}

/*******************************************************************************
* 函数名称: cpss_timer_para_set     
* 功    能: 设置带参定时器  
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ucTimerNo          UINT8                  输入           定时器号
* lDelayLength       INT32                  输入           定时器时长
* ulPara             UINT32                 输入           参数
* 函数返回: 
*  成功  定时器标识
*  失败  CPSS_TD_INVALID
* 说    明:                                 
*******************************************************************************/
UINT32 cpss_timer_para_set
(
 UINT8 ucTimerNo,
 INT32 lDuration,
 UINT32 ulParam
 )
{
    UINT32   ulTickSum;   
    UINT32   ulTid;
    
    /* 检查输入参数 */
    
    if (ucTimerNo >= MAX_PROC_TIMER)
    {
        return CPSS_TD_INVALID; 
    }
    
    /* 将时长转换成TICK数 */
    if ((lDuration >= 0) && (lDuration <= g_usCpssTmDurationOfTick)) 
    { 
        /* 如果时长小于等于一个TICK,则令TICK数为1 */
        ulTickSum = 1;
    }
    else if (lDuration < 0) /* 暂不提供 */
    {
        
        return CPSS_TD_INVALID;
    }
    else /* 一般情况 */
    {
#if 1
        ulTickSum = CPSS_DIV(((lDuration * g_usCpssTmClockRate) + 500) , 1000);
#else        
       /* ulSec  = lDuration / 1000; / 秒数 */
	   ulSec  = CPSS_DIV(lDuration , 1000);
       /* ulMs   = lDuration % 1000; / 剩余的毫秒数 */
	  ulMs   = CPSS_MOD(lDuration ,1000); 
        /* 四舍五入 */  
     /*   ulTickSum = ulSec * g_usCpssTmClockRate
            + (ulMs * g_usCpssTmClockRate + 500) / 1000;*/
          ulTickSum = ulSec * g_usCpssTmClockRate
            + CPSS_DIV(ulMs * g_usCpssTmClockRate + 500 , 1000);
#endif
    }
    
    if (ulParam == NULL_PARA) 
    {
        /* 设置无参定时器 */
        ulTid = cpss_timer_set_no_para_timer(ucTimerNo,RELATIVE_TIMER,ulTickSum);
    }
    else  
    {  
        /* 设置带参定时器 */
        ulTid = cpss_timer_set_para_timer(ucTimerNo,RELATIVE_TIMER,ulTickSum,ulParam);
        
    }
    
    /* 统计定时器信息 */
    if (ulTid == CPSS_TD_INVALID) 
    {
        /* 统计设置定时器失败次数 */
	  g_stCpssTmTimerInfo.ulFailureCount = g_stCpssTmTimerInfo.ulFailureCount +1;
    }
    else  
    {
        /* 统计设置定时器成功次数 */
       g_stCpssTmTimerInfo.ulSuccessCount = g_stCpssTmTimerInfo.ulSuccessCount +1;
    }
    
    return ulTid; 
}

/*******************************************************************************
* 函数名称: cpss_timer_loop_set       
* 功    能: 设置无参周期定时器    
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ucTimerNo          UINT8                  输入           定时器号
* lDelayLength       INT32                  输入           时长
* 函数返回: 
*  成功  CPSS_OK
*  失败  CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_loop_set
(
 UINT8 ucTimerNo, 
 INT32 lDelayLength
 )
{
    if( cpss_timer_set_loop_timer(ucTimerNo,  lDelayLength, NULL_PARA) != CPSS_TD_INVALID)
    {
        return CPSS_OK;
    }
    else
    {
        return CPSS_ERROR;
    }
}

/*******************************************************************************
* 函数名称: cpss_timer_para_loop_set   
* 功    能: 设置带参周期定时器    
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ucTimerNo          UINT8                  输入           定时器号
* lDelayLength       INT32                  输入           定时器时长
* ulPara             UINT32                 输入           参数
* 函数返回: 
*  成功  CPSS_OK
*  失败  CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
UINT32 cpss_timer_para_loop_set
(
 UINT8 ucTimerNo,
 INT32 lDelayLength,
 UINT32 ulPara
 )
{
  /* 2006/09/06 del */
   /* if(NULL_PARA == ulPara)
    {
        return CPSS_TD_INVALID;
    }
    else
    */
    {
        return (cpss_timer_set_loop_timer(ucTimerNo,  lDelayLength, ulPara));
    }
}
#ifndef CPSS_DSP_CPU
/*******************************************************************************
* 函数名称: cpss_timer_abs_set  
* 功    能: 设置无参绝对定时器   
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ucTimerNo          UINT8                  输入           定时器号
* pstClock        CPSS_TIME_T *             输入           绝对时钟
* 函数返回: 
*  成功  CPSS_OK
*  失败  CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_abs_set
(
 UINT8 ucTimerNo,
 CPSS_TIME_T *pstClock
 )
{
    CPSS_TM_SYSSOFTCLOCK_T stSoftClock;    
    UINT32         ulResult;
    
    if(NULL == pstClock)
    {
        return CPSS_ERR_TM_INPUT_INVALID_PARA;
    }
    memset(&stSoftClock, 0, sizeof(CPSS_TM_SYSSOFTCLOCK_T));    
    stSoftClock.usSysYear = pstClock->usYear;
    stSoftClock.ucSysMon = pstClock->ucMonth;
    stSoftClock.ucSysDay = pstClock->ucDay;
    stSoftClock.ucSysHour= pstClock->ucHour;
    stSoftClock.ucSysMin = pstClock->ucMinute;
    stSoftClock.ucSysSec = pstClock->ucSecond;  

    ulResult = cpss_timer_set_abs_timer(ucTimerNo,NULL_PARA,(CPSS_TM_SYSSOFTCLOCK_T *)&stSoftClock);
    if(ulResult != CPSS_TD_INVALID)
    {
        return CPSS_OK;  
    }
    else
    {
        return CPSS_ERROR;
    }    
}

/*******************************************************************************
* 函数名称: cpss_timer_para_abs_set  
* 功    能: 设置带参绝对定时器   
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ucTimerNo          UINT8                  输入           定时器号
* pstClock         CPSS_TIME_T *            输入           绝对时钟
* ulPara            UINT32                 输入           参数
* 函数返回: 
*  成功  定时器标识
*  失败  CPSS_TD_INVALID
* 说    明:                                 
*******************************************************************************/
UINT32 cpss_timer_para_abs_set
(
 UINT8 ucTimerNo,
 CPSS_TIME_T *pstClock,
 UINT32 ulPara
 )
{
    CPSS_TM_SYSSOFTCLOCK_T stSoftClock;
  /* 2006/09/06 del */  
/*    if((NULL == pstClock)||(NULL_PARA == ulPara))
    {
        return CPSS_TD_INVALID;
    }
    */
    if(NULL == pstClock)
     {
         return CPSS_TD_INVALID;
     }
    memset(&stSoftClock, 0, sizeof(CPSS_TM_SYSSOFTCLOCK_T));    
    stSoftClock.usSysYear = pstClock->usYear;
    stSoftClock.ucSysMon = pstClock->ucMonth;
    stSoftClock.ucSysDay = pstClock->ucDay;
    stSoftClock.ucSysHour= pstClock->ucHour;
    stSoftClock.ucSysMin = pstClock->ucMinute;
    stSoftClock.ucSysSec = pstClock->ucSecond;
    
    return( cpss_timer_set_abs_timer(ucTimerNo,ulPara,(CPSS_TM_SYSSOFTCLOCK_T *)&stSoftClock));
}
#endif
/*******************************************************************************
* 函数名称: cpss_timer_delete    
* 功    能: 删除无参定时器  
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ucTimerNo          UINT8                  输入           定时器号
* 函数返回: 
*  成功  CPSS_OK
*  失败  CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_delete
(
 UINT8 ucTimerNo
 )
{
    return (cpss_timer_kill_timer(ucTimerNo,NULL_PARA));
}

/*******************************************************************************
* 函数名称: cpss_timer_para_delete  
* 功    能: 删除带参定时器   
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* ulTid            UINT32                     输入         定时器描述符
* 函数返回: 
*  成功  CPSS_OK
*  失败  CPSS_ERROR
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_para_delete
(
 UINT32 ulTid
 )
{
    return( cpss_timer_kill_timer_by_tid( ulTid) );
}

/*******************************************************************************
* 函数名称:  cpss_timer_get_gmtoffset    
* 功    能:  获得时区              
* 相关文档: <无>                    
* 函数类型:                                    
* 参    数:                          
* 参数名称           类型                  输入/输出        描述
* 
* 返回值: 
*       
*  
*                           
* 说   明:
*                                          
****************************************************************************/


INT32 cpss_timer_get_gmtoffset(VOID)
{
   return( g_ucCpssTmGmtOffset - 12);
}


/*******************************************************************************
* 函数名称: cpss_gmt_get    
* 功    能: 获取自GMT时间1970年1月1日0时0分0秒起经历的秒数。  
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 经历的秒数。
* 说    明:                                 
*******************************************************************************/
UINT32 cpss_gmt_get(VOID)
{
    return (g_pstCpssTmCoreData->stTimeFrom2K.ulSecond 
                + g_ulCpssTmAnsicTimeTo2k - (g_ucCpssTmGmtOffset - 12) * 3600);
}

/*******************************************************************************
* 函数名称: cpss_tick_get  
* 功    能: 获取当前运行的tick。单位为ms
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 当前运行的tick
* 说    明:                                 
*******************************************************************************/
UINT32 cpss_tick_get (VOID)
{
#if 0
    CPSS_TM_TICK_T stTick;
    cpss_timer_get_cur_tick((CPSS_TM_TICK_T  *)&stTick);
    return (stTick.ulTickCountLow * g_usCpssTmDurationOfTick);
#endif
    return (g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow * g_usCpssTmDurationOfTick);
}

/*******************************************************************************
* 函数名称: cpss_clock_get   
* 功    能: 获取当前系统时钟（本地时间）。时区信息可配置，缺省为北京时间。 
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* pstClock           CPSS_TIME_T *          输出          获取当前系统时钟
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
VOID cpss_clock_get
(   
 CPSS_TIME_T *pstClock   
 )
{
#ifdef CPSS_VOS_VXWORKS
    CPSS_TM_SYSSOFTCLOCK_T stSoftClock;
    INT32          lResult;
    
    if(NULL == pstClock)
    {
        return;
    }
    if(CPSS_OK == cpss_timer_get_sys_clock((CPSS_TM_SYSSOFTCLOCK_T *)&stSoftClock))
    {
        pstClock->usYear= stSoftClock.usSysYear;
        pstClock->ucMonth = stSoftClock.ucSysMon;
        pstClock->ucDay = stSoftClock.ucSysDay;
        pstClock->ucHour = stSoftClock.ucSysHour;
        pstClock->ucMinute = stSoftClock.ucSysMin;
        pstClock->ucSecond = stSoftClock.ucSysSec;
        pstClock->ucWeek   = stSoftClock.ucSysWeek;
    }
    else
    {
        memset(pstClock,0, sizeof(CPSS_TIME_T));
    }
#endif

/* 由于WIN 环境下时钟不准,直接从系统获取 .李军 @2006/06/08 */
#ifdef CPSS_VOS_WINDOWS
   struct tm *pstTime;
   time_t stTime;
   time(&stTime);
   pstTime = localtime(&stTime);
   if(NULL != pstTime)
   {
       pstClock->usYear  = pstTime->tm_year + 1900;
       pstClock->ucMonth = pstTime->tm_mon + 1;
       pstClock->ucDay   = pstTime->tm_mday;
       pstClock->ucHour  = pstTime->tm_hour;
       pstClock->ucMinute  = pstTime->tm_min;
       pstClock->ucSecond  = pstTime->tm_sec;
       pstClock->ucWeek    = pstTime->tm_wday;
   }
   else
   {
       memset(pstClock,0, sizeof(CPSS_TIME_T));
   }

#endif

#ifdef CPSS_VOS_LINUX
    CPSS_TM_SYSSOFTCLOCK_T stSoftClock;
    INT32          lResult;

    if(NULL == pstClock)
    {
        return;
    }
    if(CPSS_OK == cpss_timer_get_sys_clock((CPSS_TM_SYSSOFTCLOCK_T *)&stSoftClock))
    {
        pstClock->usYear= stSoftClock.usSysYear;
        pstClock->ucMonth = stSoftClock.ucSysMon;
        pstClock->ucDay = stSoftClock.ucSysDay;
        pstClock->ucHour = stSoftClock.ucSysHour;
        pstClock->ucMinute = stSoftClock.ucSysMin;
        pstClock->ucSecond = stSoftClock.ucSysSec;
        pstClock->ucWeek   = stSoftClock.ucSysWeek;
    }
    else
    {
        memset(pstClock,0, sizeof(CPSS_TIME_T));
    }
#endif


#if 0
#ifdef CPSS_VOS_LINUX
   struct tm *pstTime;
   time_t stTime;
   time(&stTime);
   pstTime = localtime(&stTime);
   if(NULL != pstTime)
   {
       pstClock->usYear  = pstTime->tm_year + 1900;
       pstClock->ucMonth = pstTime->tm_mon + 1;
       pstClock->ucDay   = pstTime->tm_mday;
       pstClock->ucHour  = pstTime->tm_hour;
       pstClock->ucMinute  = pstTime->tm_min;
       pstClock->ucSecond  = pstTime->tm_sec;
       pstClock->ucWeek    = pstTime->tm_wday;
   }
   else
   {
       memset(pstClock,0, sizeof(CPSS_TIME_T));
   }

#endif

#endif

}

#ifndef CPSS_DSP_CPU
/*******************************************************************************
* 函数名称: cpss_clock_set    
* 功    能: 设置当前系统时钟（本地时间）?     
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* pstClock           CPSS_TIME_T *          输入           系统时钟
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
VOID cpss_clock_set
( 
 CPSS_TIME_T *pstClock  
 )
{
    CPSS_TM_SYSSOFTCLOCK_T stSoftClock;
    INT32 lRet;
    
    if(NULL == pstClock)
    {
        return;
    }
    memset(&stSoftClock,0,sizeof(CPSS_TM_SYSSOFTCLOCK_T));
    stSoftClock.usSysYear = pstClock->usYear;
    stSoftClock.ucSysMon = pstClock->ucMonth;
    stSoftClock.ucSysDay = pstClock->ucDay;
    stSoftClock.ucSysHour= pstClock->ucHour;
    stSoftClock.ucSysMin = pstClock->ucMinute;
    stSoftClock.ucSysSec = pstClock->ucSecond;
    
    lRet = cpss_timer_set_sys_clock((CPSS_TM_SYSSOFTCLOCK_T *)&stSoftClock);
    #ifdef CPSS_FUNBRD_MC  /*主控板需要设置cmos时间*/
        #ifdef CPSS_VOS_VXWORKS
         if(lRet == CPSS_OK)
         {
            cpss_timer_set_cmos(&stSoftClock);
         }   
        #endif
    #endif
}

/*******************************************************************************
* 函数名称: cpss_offset_time_get    
* 功    能: 计算发生偏移后的绝对时间          
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* psstTimeStart      const CPSS_TIME_T *    IN             起始时间
* lOffset           INT32                  IN             + 表示向后,-表示向前
* psstTime           CPSS_TIME_T *          IN             偏移后的时间
* 函数返回: 
* 成功: CPSS_OK      
* 失败: CPSS_ERROR 
* 说    明:                                 
*******************************************************************************/
INT32 cpss_offset_time_get
(
 const CPSS_TIME_T *pstTimeStart,
 INT32 lOffset,
 CPSS_TIME_T *pstTime
 )
{
    CPSS_TM_TIMER_T         stSecMillisec;
    CPSS_TM_SYSSOFTCLOCK_T stStartClock, stEndClock;
    INT32          lResult;
    
    memset(&stSecMillisec,0 ,sizeof(CPSS_TM_TIMER_T));
    memset(&stStartClock,0,sizeof(CPSS_TM_SYSSOFTCLOCK_T));
    memset(&stEndClock,0,sizeof(CPSS_TM_SYSSOFTCLOCK_T));
    
    if((NULL == pstTimeStart)||(NULL == pstTime))
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_WARN,"\n input null pointer in cpss_offset_time_get \n");
        return CPSS_ERR_TM_INPUT_INVALID_PARA;
    }
    stStartClock.usSysYear = pstTimeStart->usYear;
    stStartClock.ucSysMon  = pstTimeStart->ucMonth;
    stStartClock.ucSysDay  = pstTimeStart->ucDay;
    stStartClock.ucSysHour = pstTimeStart->ucHour;
    stStartClock.ucSysMin  = pstTimeStart->ucMinute;
    stStartClock.ucSysSec  = pstTimeStart->ucSecond;

    lResult = cpss_timer_clock2time(&stStartClock, &stSecMillisec);
    if(CPSS_OK != lResult)
    {
        return lResult;
    }
    
    stSecMillisec.ulSecond += lOffset;
    lResult = cpss_timer_time2clock(&stSecMillisec, &stEndClock);
    if(CPSS_OK != lResult)
    {
        return lResult;
    }    
    pstTime->usYear   = stEndClock.usSysYear;
    pstTime->ucMonth  = stEndClock.ucSysMon;
    pstTime->ucDay    = stEndClock.ucSysDay;
    pstTime->ucHour   = stEndClock.ucSysHour;
    pstTime->ucMinute = stEndClock.ucSysMin;
    pstTime->ucSecond = stEndClock.ucSysSec;
    pstTime->ucWeek   = stEndClock.ucSysWeek;
    
    return CPSS_OK;    
}

/*******************************************************************************
* 函数名称: cpss_gmt2clock    
* 功    能: gmt时间转为绝对时间          
* 函数类型: 
* 参    数: 
* 参数名称    类型                 输入/输?   描述
* ulGmtSeconds      UINT32                    IN                gmt时间
* pstClock            CPSS_TIME_T *          OUT           绝对时间
* 函数返回: 
* 成功: CPSS_OK      
* 失败: CPSS_ERROR 
* 说    明:                                 
*******************************************************************************/

INT32 cpss_gmt2clock
 (
   UINT32 ulGmtSeconds,
   CPSS_TIME_T *pstClock
 )
{
     CPSS_TM_TIMER_T stTime;
     CPSS_TM_SYSSOFTCLOCK_T stSoftClock;
     INT32 lRet;
 
     if(NULL==pstClock)
     {
         return CPSS_ERR_TM_INPUT_INVALID_PARA;
     }
     
     stTime.ulSecond    = ulGmtSeconds + cpss_timer_get_gmtoffset()*3600 - g_ulCpssTmAnsicTimeTo2k;
     stTime.usMilliSec  = 0;

     lRet = cpss_timer_time2clock(&stTime, &stSoftClock);

     if(CPSS_OK == lRet)
     {
          pstClock->usYear= stSoftClock.usSysYear;
          pstClock->ucMonth = stSoftClock.ucSysMon;
          pstClock->ucDay = stSoftClock.ucSysDay;
          pstClock->ucHour = stSoftClock.ucSysHour;
          pstClock->ucMinute = stSoftClock.ucSysMin;
          pstClock->ucSecond = stSoftClock.ucSysSec;
          pstClock->ucWeek   = stSoftClock.ucSysWeek;

     }

     return lRet;
}

/*******************************************************************************
* 函数名称: cpss_clock2gmt    
* 功    能: 绝对时间转为gmt时间          
* 函数类型: 
* 参    数: 
* 参数名称    类型                 输入/输?   描述
* ulGmtSeconds      UINT32  *                  OUT                gmt时间
* pstClock            CPSS_TIME_T *         IN            绝对时间
* 函数返回: 
* 成功: CPSS_OK      
* 失败: CPSS_ERROR 
* 说    明:                                 
*******************************************************************************/

INT32 cpss_clock2gmt
(
    CPSS_TIME_T *pstClock,
    UINT32 *pulGmtSeconds
)
{
    CPSS_TM_SYSSOFTCLOCK_T stSoftCLock;
    CPSS_TM_TIMER_T stTime;
    INT32 lRet;

    if((NULL == pstClock)||(NULL == pulGmtSeconds))
    {
        return CPSS_ERR_TM_INPUT_INVALID_PARA;
    }
     
    memset(&stSoftCLock,0,sizeof(CPSS_TM_SYSSOFTCLOCK_T));
    stSoftCLock.usSysYear = pstClock->usYear;
    stSoftCLock.ucSysMon = pstClock->ucMonth;
    stSoftCLock.ucSysDay = pstClock->ucDay;
    stSoftCLock.ucSysHour= pstClock->ucHour;
    stSoftCLock.ucSysMin = pstClock->ucMinute;
    stSoftCLock.ucSysSec = pstClock->ucSecond;
    lRet = cpss_timer_clock2time(&stSoftCLock,&stTime);
    if(CPSS_OK ==lRet)
    {
        *pulGmtSeconds = stTime.ulSecond + g_ulCpssTmAnsicTimeTo2k - cpss_timer_get_gmtoffset()*3600;
    }
    
    return lRet;

}
#endif
/*******************************************************************************
* 函数名称: cpss_high_precision_counter_get 
* 功    能: 提供高精度时钟计数器读取功能，精度为10毫秒，单位为毫秒。      
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 时钟计数
* 说    明:                                 
*******************************************************************************/

UINT32 cpss_high_precision_counter_get(VOID)
{
#if 0    
    CPSS_TM_TICK_T stTick;
    cpss_timer_get_cur_tick((CPSS_TM_TICK_T  *)&stTick);
    return(stTick.ulTickCountLow);
#endif    
    return g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow;
}

UINT32 cpss_microsecond_counter_get (VOID)
{
   #ifndef CPSS_DSP_CPU
   {
      UINT32 ulMicroSec = 0;
#ifdef CPSS_VOS_VXWORKS
      drv_hw_timer_get(&ulMicroSec);
#endif
      return g_ulCpssMicroSec +ulMicroSec;
	 
   }	 	
   #else
     return CPSS_TD_INVALID;
   #endif
}

/*******************************************************************************
* 函数名称: cpss_timer_init 
* 功    能: 定时器初始化       
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_init(VOID)
{
    return cpss_timer_init_timer_module();
}

/*******************************************************************************
* 函数名称: cpss_timer_scan_task  
* 功    能: 定时器扫描处理任务  
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
#ifndef CPSS_DSP_CPU
UINT32 g_ulCpssTimerScanTaskPList[3];
INT32 cpss_timer_scan_task_init()
{
    UINT16              usDurationOfTick; 
    UINT16              usMsInOneSecInFact;
    UINT16              usTwoMins;
    
    usDurationOfTick   = (UINT16)(CPSS_DIV(1000,g_usCpssTmClockRate));
    usTwoMins          = (UINT16)(g_usCpssTmClockRate * 60 * 2);
    usMsInOneSecInFact = (UINT16)(usDurationOfTick * g_usCpssTmClockRate);    
    g_ulCpssTimerScanTaskPList[0] = usMsInOneSecInFact;
    g_ulCpssTimerScanTaskPList[1] = 1;
    
    return (CPSS_OK);
}

INT32 cpss_timer_scan_task_block(UINT32 *pulParamList)
{
#ifdef CPSS_VOS_VXWORKS
    /* 如果能使用辅助时钟，则获取同步信号量，否则采用DELAY任务的方法进行延时 */

    if (g_bCpssTmIsUseAuxClock != FALSE)	  	
    {
        if(cpss_vos_sem_p(g_pstCpssTmCoreData->ulSynSemForTick, WAIT_FOREVER)!=CPSS_OK)
        {
            cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n p SynSemForTick Fail in cpss_timer_scan_task \n");                
            return CPSS_ERROR;
        }            
    }
    else
    {
        taskDelay(pulParamList[1]);   /* 不能使用辅助时钟时，采用系统时钟来计时 */
    }
#endif

#ifdef  CPSS_VOS_WINDOWS
    Sleep(1000/g_usCpssTmClockRate);
#endif  

#ifndef SWP_CYGWIN
    if(cpss_vos_sem_p(g_pstCpssTmCoreData->ulSynSemForTick, WAIT_FOREVER)!=CPSS_OK)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n p SynSemForTick Fail in cpss_timer_scan_task \n");
        return CPSS_ERROR;
    }
#else
	cpss_delay(1000/g_usCpssTmClockRate);
#endif

    return CPSS_OK;
}

INT32 cpss_timer_scan_task_dealing(INT32 lRet, UINT32 *pulParamList)
{
    CPSS_TM_TMCB_T              *pstFirstTMCB;
    static UINT32              ulScanCount = 0;
    UINT32              ulLoop;      
    CPSS_TM_TIMER_T              stTime;
    CPSS_TM_TIMER_T              *pstTime = &stTime;    
    INT32               lResult;
    
     

    pstFirstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[0]);       

    /* 将缓冲池中的消息发送出去 */ 
        for (ulLoop = 0; ulLoop < VK_MAX_SCHED_NUM; ulLoop ++)
        {
            if (g_astCpssTmMsgBuffer[ulLoop].ptMsgHead != NULL)
            {
               /* 将缓存的定时器消息挂到相应的调度任务的消息队列 */
               
               
               if( cpss_ipc_msg_send(g_astCpssTmMsgBuffer[ulLoop].ptMsgHead,IPC_MSG_TYPE_REMOTE)!=CPSS_ERROR)
               /* 发送成功，将缓冲区清空 */                          
             
               {                         
                   g_astCpssTmMsgBuffer[ulLoop].ptMsgHead = NULL;
                   g_astCpssTmMsgBuffer[ulLoop].ptMsgTail = NULL;
               } 
              
            }
 
        }	
    /* 获取互斥信号量 */
    lResult = cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTickCounts, WAIT_FOREVER);
    if(lResult != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n p MutexSemForTickCounts Fail in cpss_timer_scan_task \n");            
        return (CPSS_ERROR);
    }        
    /* 如果开机以来的TICK数超过了4G-1，则向高位进1 */
    if (g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow == 0xffffffff)
    {
        g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountHigh = g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountHigh +1;
       /* g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountHigh ++;*/
        g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow = 0;
    }
    else
    {
       g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow = g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow +1;
       /* g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow  ++;*/
    }
    
    /* 释放互斥信号量 */
    lResult = cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTickCounts);
    if(lResult != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n v MutexSemForTickCounts Fail in cpss_timer_scan_task \n");            
        return (CPSS_ERROR);
    }
    
    /* 扫描相对定时器队列 */
    cpss_timer_relative_timer_scan();
    ulScanCount = ulScanCount +1;		
    /*ulScanCount ++;        */
    /*OSS_DbgPrintf(PRINT_MODULE_OSS, PRINT_LEVEL_ONE, "ulScanCount = %lu \n", ulScanCount ); */      
    /* 获取互斥信号量 */
    lResult = cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock, WAIT_FOREVER);
    if(lResult != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n p MutexSemForSysSoftClock Fail in cpss_timer_scan_task \n");            
        return (CPSS_ERROR);
    }
    
    g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec = (UINT16)(g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec + g_usCpssTmDurationOfTick);        
    /* 维护系统时间，一秒内的毫秒数实际上为usMsInOneSecInFact */
    if (g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec >= pulParamList[0])
    {    
       g_pstCpssTmCoreData->stTimeFrom2K.ulSecond = g_pstCpssTmCoreData->stTimeFrom2K.ulSecond +1;
       /* g_pstCpssTmCoreData->stTimeFrom2K.ulSecond ++;            */
        g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec = (UINT16)(g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec - pulParamList[0]);
    }        
    /* 每秒钟扫描一次绝对定时器队列 */
    /*  if(0 == (ulScanCount % g_usCpssTmClockRate))  */
	if(0 == CPSS_MOD(ulScanCount , g_usCpssTmClockRate))  	
    {
         /* 维护开机以来的秒数 */
	    g_pstCpssTmCoreData->ulSecFromPowerOn = g_pstCpssTmCoreData->ulSecFromPowerOn +1;	

        send_msgq_loop_timer();
	 
	  /* WIN 下校绝对时间*/
#ifdef CPSS_VOS_WINDOWS
		{
			CPSS_TIME_T tAbs;
			cpss_clock_get(&tAbs);
			cpss_clock_set(&tAbs);
		}
#endif	 
#if 0
#ifdef CPSS_VOS_LINUX
		{
			CPSS_TIME_T tAbs;
			cpss_clock_get(&tAbs);
			cpss_clock_set(&tAbs);
		}
#endif
#endif
        /*  g_pstCpssTmCoreData->ulSecFromPowerOn++;                        */
        pstTime->ulSecond = g_pstCpssTmCoreData->stTimeFrom2K.ulSecond;
#ifndef CPSS_DSP_CPU
        /* 扫描绝对定时器队列 */
        cpss_timer_abs_timer_scan(pstTime->ulSecond);
#endif
    }         
    
    /* 释放互斥信号量 */
    lResult = cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock);
    if(lResult != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n v MutexSemForSysSoftClock Fail in cpss_timer_scan_task \n");            
        return (CPSS_ERROR);
    }            
    return (CPSS_OK);
    
}
INT32 cpss_timer_scan_task_reg()
{
    INT32 lRet;
    UINT32 ulTaskType;
#ifdef CPSS_VOS_VXWORKS
    if (g_bCpssTmIsUseAuxClock == TRUE)
    {
        ulTaskType = CPSS_NORMAL_TASK_TYPE_SEM ;
    }
    else
    {
        ulTaskType = CPSS_NORMAL_TASK_TYPE_DELAY;
    }
#endif

#ifdef  CPSS_VOS_WINDOWS
        ulTaskType = CPSS_NORMAL_TASK_TYPE_DELAY;
#endif

#ifdef  CPSS_VOS_LINUX
        ulTaskType = CPSS_NORMAL_TASK_TYPE_DELAY;
#endif
    lRet = cpss_vos_ntask_register(
              ulTaskType,
              cpss_timer_scan_task_init, 
              cpss_timer_scan_task_block,
			  cpss_timer_scan_task_dealing,
              g_ulCpssTimerScanTaskPList,
              CPSS_TIMER_TASKNAME);
    if ( lRet != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                "TimerScan task register failed! errCode = %#x \n", lRet);
        return (CPSS_ERROR);
    }

    return (CPSS_OK);
    
}
#endif

extern VOID cpss_kw_set_task_run(VOID);
extern VOID cpss_kw_set_task_stop(VOID);

VOID cpss_timer_scan_task(VOID)
{
    CPSS_TM_TMCB_T              *pstFirstTMCB;
    UINT32              ulScanCount = 0;
    UINT32              ulLoop;      
    UINT16              usDurationOfTick; 
    CPSS_TM_TIMER_T              stTime;
    CPSS_TM_TIMER_T              *pstTime = &stTime;    
    UINT16              usMsInOneSecInFact;
    UINT16              usTwoMins;
    INT32               lResult;
    
/*    usDurationOfTick   = (UINT16)(1000/g_usCpssTmClockRate);*/
     usDurationOfTick   = (UINT16)(CPSS_DIV(1000,g_usCpssTmClockRate));
    usTwoMins          = (UINT16)(g_usCpssTmClockRate * 60 * 2);
    /* 实际每秒钟内的毫秒数 */
    usMsInOneSecInFact = (UINT16)(usDurationOfTick * g_usCpssTmClockRate);    

#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS)|| defined(CPSS_VOS_LINUX)    
    while (1)
#endif
#ifdef CPSS_DSP_CPU
    while (g_lAppSysTicks != g_lIsrSysTicks)
#endif
    {
         cpss_kw_set_task_stop();
#ifdef CPSS_VOS_VXWORKS
        /* 如果能使用辅助时钟，则获取同步信号量，否则采用DELAY任务的方法进行延时 */
	    if (g_bCpssTmIsUseAuxClock != FALSE)
     {
            if(cpss_vos_sem_p(g_pstCpssTmCoreData->ulSynSemForTick, WAIT_FOREVER)!=CPSS_OK)
            {
                cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n p SynSemForTick Fail in cpss_timer_scan_task \n");                
                return;
            }            
        }
        else
        {
            taskDelay(1);   /* 不能使用辅助时钟时，采用系统时钟来计时 */
        }
#endif
        
#ifdef  CPSS_VOS_WINDOWS
        Sleep(1000/g_usCpssTmClockRate);
#endif     

#ifndef SWP_CYGWIN
        if(cpss_vos_sem_p(g_pstCpssTmCoreData->ulSynSemForTick, WAIT_FOREVER)!=CPSS_OK)
        {
            cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n p SynSemForTick Fail in cpss_timer_scan_task \n");
            return;
        }
#else
        cpss_delay(1000/g_usCpssTmClockRate);
#endif   
        cpss_kw_set_task_run();

        pstFirstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[0]);        
        
        /* 将缓冲池中的消息发送出去 */ 

 /* 将缓冲池中的消息发送出去 */ 
        for (ulLoop = 0; ulLoop < VK_MAX_SCHED_NUM; ulLoop ++)
        {
            if (g_astCpssTmMsgBuffer[ulLoop].ptMsgHead != NULL)
            {
               /* 将缓存的定时器消息挂到相应的调度任务的消息队列 */
               
               if( cpss_ipc_msg_send(g_astCpssTmMsgBuffer[ulLoop].ptMsgHead,IPC_MSG_TYPE_REMOTE)!=CPSS_ERROR)
               /* 发送成功，将缓冲区清空 */                          
             
               {                         
                   g_astCpssTmMsgBuffer[ulLoop].ptMsgHead = NULL;
                   g_astCpssTmMsgBuffer[ulLoop].ptMsgTail = NULL;
               } 
              
            }
 
        }

        /* 获取互斥信号量 */
        lResult = cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTickCounts, WAIT_FOREVER);
        if(lResult != CPSS_OK)
        {
            cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n p MutexSemForTickCounts Fail in cpss_timer_scan_task \n");            
            return;
        }        
        /* 如果开机以来的TICK数超过了4G-1，则向高位进1 */
        if (g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow == 0xffffffff)
        {
          g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountHigh = g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountHigh +1;
           /* g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountHigh ++;*/
            g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow = 0;
        }
        else
        {
           g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow = g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow +1;
           /* g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow  ++;*/
        }
        
        /* 释放互斥信号量 */
        lResult = cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTickCounts);
        if(lResult != CPSS_OK)
        {
            cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n v MutexSemForTickCounts Fail in cpss_timer_scan_task \n");            
            return;
        }
        
        /* 扫描相对定时器队列 */
        cpss_timer_relative_timer_scan();
        ulScanCount = ulScanCount +1;		
        /*ulScanCount ++;        */
        /*OSS_DbgPrintf(PRINT_MODULE_OSS, PRINT_LEVEL_ONE, "ulScanCount = %lu \n", ulScanCount ); */      
        /* 获取互斥信号量 */
        lResult = cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock, WAIT_FOREVER);
        if(lResult != CPSS_OK)
        {
            cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n p MutexSemForSysSoftClock Fail in cpss_timer_scan_task \n");            
            return;
        }
        
        g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec = (UINT16)(g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec + g_usCpssTmDurationOfTick);        
        /* 维护系统时间，一秒内的毫秒数实际上为usMsInOneSecInFact */
        if (g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec >= usMsInOneSecInFact)
        {    
           g_pstCpssTmCoreData->stTimeFrom2K.ulSecond = g_pstCpssTmCoreData->stTimeFrom2K.ulSecond +1;
           /* g_pstCpssTmCoreData->stTimeFrom2K.ulSecond ++;            */
            g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec = (UINT16)(g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec - usMsInOneSecInFact);
        }        
        /* 每秒钟扫描一次绝对定时器队列 */
     /*   if(0 == (ulScanCount % g_usCpssTmClockRate))  */
	  if(0 == CPSS_MOD(ulScanCount , g_usCpssTmClockRate))  	
        {
            /* 维护开机以来的秒数 */
		g_pstCpssTmCoreData->ulSecFromPowerOn = g_pstCpssTmCoreData->ulSecFromPowerOn +1;	
          /*  g_pstCpssTmCoreData->ulSecFromPowerOn++;                        */
            pstTime->ulSecond = g_pstCpssTmCoreData->stTimeFrom2K.ulSecond;
#ifndef CPSS_DSP_CPU
            /* 扫描绝对定时器队列 */
            cpss_timer_abs_timer_scan(pstTime->ulSecond);
#endif
        }         
        
        /* 释放互斥信号量 */
        lResult = cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock);
        if(lResult != CPSS_OK)
        {
            cpss_output(CPSS_MODULE_TM,CPSS_PRINT_FATAL,"\n v MutexSemForSysSoftClock Fail in cpss_timer_scan_task \n");            
            return;
        }        
#ifdef CPSS_DSP_CPU
        g_lAppSysTicks = g_lAppSysTicks + 1;
#endif        
        
    }/* end while(1) */
    
    return;    
}
/*********************** for UPLAT End ***************************************/

/*******************************************************************************
* 函数名称: cpss_timer_show_info   
* 功    能: 显示定时器使用信息  
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_show(VOID)
{
    INT32              lMaxTimerCount = MAX_TIMER_NUM;
    CPSS_TM_TMCBPOOLITEM_T     *pstTmcbPool;
    CPSS_TM_ABSTIMERQUEUE_T    *pstAbsTQue;
    CPSS_TM_RELTIMERQUEUE_T    *pstRelTQue;
    INT32              lResult;
    
    cps__oams_shcmd_printf("\n\n");
    cps__oams_shcmd_printf("Max Timer Count is %d \n", lMaxTimerCount);

    lResult = cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTMCB,WAIT_FOREVER);
    if(lResult != CPSS_OK)
    {
        cps__oams_shcmd_printf(" p ulMutexSemForTMCB Fail in cpss_timer_show_timer_info\n");
        return CPSS_ERROR;
    }
    
    /* 获得定时器控制块池的地址 */
    pstTmcbPool = (CPSS_TM_TMCBPOOLITEM_T *)&(g_pstCpssTmCoreData->stTMCBPool);        
    cps__oams_shcmd_printf("Idle Timer Count is %d \n", pstTmcbPool->ulFreeCount);    
    /* 定时器使用峰值 */
    /* 因为该函数是为调试而写的函数，所以不应该受到其它因素的影响。因此没有使用OSS_Printf  */
    cps__oams_shcmd_printf("Used Peak Count is  %d \n ",g_stCpssTmTimerInfo.ulTimerPeerCount);

    lResult = cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTMCB);
    if(lResult != CPSS_OK)
    {
        cps__oams_shcmd_printf(" V ulMutexSemForTMCB Fail in cpss_timer_show_timer_info\n");
        return CPSS_ERROR;
    }
    lResult = cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForTimerQueue,WAIT_FOREVER);
    if(lResult != CPSS_OK)
    {
        cps__oams_shcmd_printf(" p ulMutexSemForTimerQueue Fail in cpss_timer_show_timer_info\n");
        return CPSS_ERROR;
    }

    /* 获得绝对定时器队列的指针 */
    pstAbsTQue   = (CPSS_TM_ABSTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stAbsTimerQueue);
    cps__oams_shcmd_printf("Abs Timer Count is  %d \n", pstAbsTQue->ulElementCount);    
    /* 获得相对定时器队列的指针 */
    pstRelTQue = (CPSS_TM_RELTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stRelTimerQueue);
    cps__oams_shcmd_printf("Relative TimerCount is %d \n", pstRelTQue->ulElementCount);
    cps__oams_shcmd_printf("Being Expire State Count is %d \n", lMaxTimerCount - pstTmcbPool->ulFreeCount - pstAbsTQue->ulElementCount - pstRelTQue->ulElementCount);

    lResult = cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForTimerQueue);
    if(lResult != CPSS_OK)
    {
        cps__oams_shcmd_printf(" v ulMutexSemForTimerQueue Fail in cpss_timer_show_timer_info\n");
        return CPSS_ERROR;
    }
    lResult = cpss_vos_sem_p(g_ulCpssTmMutexSemForTimerInfo,WAIT_FOREVER);
    if(lResult != CPSS_OK)
    {
       cps__oams_shcmd_printf(" p g_ulCpssTmMutexSemForTimerInfo Fail in cpss_timer_show_timer_info\n");
        return CPSS_ERROR;
    }
   
    /* 设置定时器成功次数和失败次数 */    
    /* 因为该函数是为调试而写的函数，所以不应该受到其它因素的影响。因此没有使用OSS_Printf  */   
    cps__oams_shcmd_printf("Set Timer Success Count is %d \n ", g_stCpssTmTimerInfo.ulSuccessCount);
    cps__oams_shcmd_printf("Set Timer Fail Count is %d \n ", g_stCpssTmTimerInfo.ulFailureCount);

    lResult = cpss_vos_sem_v(g_ulCpssTmMutexSemForTimerInfo);
    if(lResult != CPSS_OK)
    {
        cps__oams_shcmd_printf(" v g_ulCpssTmMutexSemForTimerInfo Fail in cpss_timer_show_timer_info\n");
        return CPSS_ERROR;
    }
    
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_timer_show_proc_info    
* 功    能: 显示某个纤程定时器使用信息      
* 函数类型: 
* 参    数: 
* 参数名称    类型   输入/输出  描述
* 
* 函数返回: 
* 说    明:                                 
*******************************************************************************/
INT32 cpss_timer_show_proc(UINT32 ulPno)
{
    CPSS_VK_PROC_PCB_T     *pstPCB;    
    UINT32            ulParamTimerCount = 0;
    INT16             usLoop;
    
    pstPCB = cpss_vk_proc_pcb_get(ulPno);
    if(pstPCB == NULL)
    {
       cps__oams_shcmd_printf("get pcb fail \n");
        return CPSS_ERROR;
    }

    cps__oams_shcmd_printf("The Timer being Used Count is %d \n", pstPCB->usTimerCounts);    
    for (usLoop = 0; usLoop < MAX_PROC_TIMER; usLoop++)
    {
        if (pstPCB->aulTimerId[usLoop] != LEOF)
        {
           ulParamTimerCount = ulParamTimerCount +1;
           /* ulParamTimerCount++;*/
        }
    }    
    cps__oams_shcmd_printf("No Para Timer being Used Count is %d \n", ulParamTimerCount);
    cps__oams_shcmd_printf("Para Timer being Used Count is %d \n", pstPCB->usTimerCounts - ulParamTimerCount);
    
    return CPSS_OK;    
}

#ifndef CPSS_DSP_CPU
/*******************************************************************************
* 函数名称:  cpss_sntp_set_local_clock    
* 功    能:  设置本板时间信息               
* 相关文档: <无>                    
* 函数类型:                                    
* 参    数:                          
* 参数名称           类型                  输入/输出        描述
* pSecond             UINT32            IN            秒信息
* pMilliSec           UINT16            IN             毫秒信息
* 返回值: 
*       
*  
*                           
* 说   明:（可选 ，标识本函数未完成或者可能改动，在本函数相应位置要加"/*TBD    
*        TBD/"批注，即留待完成）
*                                          
*******************************************************************/
/* 2006/08/07 李军*/

INT32 cpss_timer_set_local_clock(UINT32  ulSecond, UINT16  usMilliSec)
{
    CPSS_TM_TIMER_T stTime;
    CPSS_TM_SYSSOFTCLOCK_T stClock;
   if(cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock, WAIT_FOREVER) !=CPSS_OK)
        {
           return CPSS_ERROR;
        }
      /* printf(" \n cpss_timer_set_local_clock  ulSecond = 0x%x, usMilliSec= 0x%x",
        ulSecond,
        usMilliSec);
       */
       g_pstCpssTmCoreData->stTimeFrom2K.ulSecond   = ulSecond - g_ulCpssTmAnsicTimeTo2k;
       g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec = usMilliSec;     

        /*2006/08/08 实际要保存的毫秒*/
      /* g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec = (UINT16)((g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec * g_usCpssTmMsNumInOneSec) / 1000);    */

       g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec = (UINT16)(CPSS_DIV(g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec * g_usCpssTmMsNumInOneSec , 1000)); 		
       stTime.ulSecond =  g_pstCpssTmCoreData->stTimeFrom2K.ulSecond;
       stTime.usMilliSec = g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec;
       
      if(cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock)!=CPSS_OK)
        {
           return CPSS_ERROR;
        }
 
    
      cpss_timer_time2clock(&stTime,&stClock);
      #if 0
      printf( "\ncpss_timer_set_local_clock year = %d mon = %d, day = %d hour = %d min = %d sec = %d\n",
                stClock.usSysYear,stClock.ucSysMon,stClock.ucSysDay,stClock.ucSysHour,stClock.ucSysMin,stClock.ucSysSec);
      #endif   
      #ifdef CPSS_FUNBRD_MC  /*主控板需要设置cmos时间*/
        #ifdef CPSS_VOS_VXWORKS
         cpss_timer_set_cmos(&stClock);
        #endif
     
      #endif
      return CPSS_OK;
}



/*******************************************************************************
* 函数名称:  cpss_timer_get_local_clock    
* 功    能:  获得本板时间信息               
* 相关文档: <无>                    
* 函数类型:                                    
* 参    数:                          
* 参数名称           类型                  输入/输出        描述
* pSecond             UINT32 *            out             秒信息
* pMilliSec           UINT16 *            out             毫秒信息
* 返回值: 
*       
*  
*                           
* 说   明:（可选 ，标识本函数未完成或者可能改动，在本函数相应位置要加"/TBD    
*        TBD/"批注，即留待完成）
*                                          
*******************************************************************/
/* 2006/08/07 李军*/
INT32 cpss_timer_get_local_clock(UINT32 *pSecond,UINT16 *pMilliSec)
{
    if(cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock, WAIT_FOREVER)!=CPSS_OK)
        {
           return CPSS_ERROR;
        }
        *pSecond   = g_pstCpssTmCoreData->stTimeFrom2K.ulSecond + g_ulCpssTmAnsicTimeTo2k;
        *pMilliSec = g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec;
               
      if(cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock)!=CPSS_OK)
        {
           return CPSS_ERROR;
        }
      
      return CPSS_OK;
}

/*******************************************************************************
* 函数名称:  cpss_timer_diff    
* 功    能:  计算获得的时间和本版时间差值              
* 相关文档: <无>                    
* 函数类型:                                    
* 参    数:                          
* 参数名称           类型                  输入/输出        描述
* ulNewSec          UINT32                 IN           从SNTP获得的秒
* usNewMilliSec     UINT16                 IN           毫秒
* 返回值: 
*  误差     
*  
*                           
* 说   明:（可选 ，标识本函数未完成或者可能改动，在本函数相应位置要加"/TBD    
*        TBD/"批注，即留待完成）
*                                          
*******************************************************************/
/* 2006/08/07 李军*/

UINT32 cpss_timer_diff(UINT32 ulNewSec,UINT16 usNewMilliSec)
{
    UINT32 ulDiffSec = 0,ulDiffMilliSec = 0,ulSec = 0;
    UINT16 usMilliSec  = 0;
   if(cpss_vos_sem_p(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock, WAIT_FOREVER)!=CPSS_OK)
        {
          ;
        }

      ulSec = g_pstCpssTmCoreData->stTimeFrom2K.ulSecond + g_ulCpssTmAnsicTimeTo2k ;
     
     /* 2006/08/08 转为标准毫秒*/
    /*  usMilliSec = (UINT16)((g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec * 1000) / g_usCpssTmMsNumInOneSec);      */

     usMilliSec = (UINT16)(CPSS_DIV(g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec * 1000 , g_usCpssTmMsNumInOneSec));  
     if(cpss_vos_sem_v(g_pstCpssTmCoreData->ulMutexSemForSysSoftClock)!=CPSS_OK)
        {
          ;
        }
     
      ulDiffSec = (ulSec > ulNewSec)?ulSec - ulNewSec:ulNewSec - ulSec;
                   
      ulDiffMilliSec =   (usMilliSec > usNewMilliSec)?usMilliSec - usNewMilliSec:usNewMilliSec - usMilliSec;         
      
    /*  printf(" \ndiff sec = %d millisec = %d\n", ulDiffSec,ulDiffMilliSec);*/
         
      return (ulDiffSec*1000 + ulDiffMilliSec);

}

/*******************************************************************************
* 函数名称:  cpss_timer_show_clock    
* 功    能:  显示当前时间          
* 相关文档: <无>                    
* 函数类型:                                    
* 参    数:                          
* 参数名称           类型                  输入/输出        描述

* 返回值: CPSS_OK or CPSS_ERROR
*  误差     
*  
*                           
* 说   明:（可选 ，标识本函数未完成或者可能改动，在本函数相应位置要加"/TBD    
*        TBD/"批注，即留待完成）
*                                          
*******************************************************************/

/*  调测函数*/
INT32 cpss_timer_show_clock(VOID)
{
  CPSS_TIME_T stTime;
  
  cpss_clock_get(&stTime);

 cps__oams_shcmd_printf(" \nYear =  %d Month = %d Day = %d Hour = %d Minute = %d Second = %d\n",
            stTime.usYear,
            stTime.ucMonth,
            stTime.ucDay,
            stTime.ucHour,
            stTime.ucMinute,
            stTime.ucSecond);
 return CPSS_OK;

}
#endif

/*******************************************************************************
* 函数名称:  cpss_timer_show_id    
* 功    能:  显示指定定时器的控制信息     
* 相关文档: <无>                    
* 函数类型:                                    
* 参    数:                          
* 参数名称           类型                  输入/输出        描述
* ulTid                         UINT32                     IN                         定时器描述符
* 返回值: CPSS_OK or CPSS_ERROR
*  误差     
*  
*                           
* 说   明:（可选 ，标识本函数未完成或者可能改动，在本函数相应位置要加"/TBD    
*        TBD/"批注，即留待完成）
*                                          
*******************************************************************/
INT32 cpss_timer_show_id(UINT32 ulTid)
{
    CPSS_TM_TMCB_T    *pstTMCB;
    CPSS_TM_RELTIMERQUEUE_T *pstRelTQue;
    
    if(ulTid >=MAX_TIMER_NUM)
        {
           return CPSS_ERROR;
        }
     /* 获得相对定时器队列的头部 */
    pstRelTQue = (CPSS_TM_RELTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stRelTimerQueue);    

    /* 获得定时器控制块*/    
    pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulTid]);

   cps__oams_shcmd_printf(" \n\rCursorTicks : %d",   pstRelTQue->ulCursorForTick);
   cps__oams_shcmd_printf(" \n\rTimerId :%d\n\r",ulTid);
   cps__oams_shcmd_printf("\n\rbisInuse :%d\n\r",pstTMCB->bIsInUse);
    cps__oams_shcmd_printf("\n\rbisTimeOut :%d\n\r",pstTMCB->bIsTimeOut);
   cps__oams_shcmd_printf("\n\rTimerType :%d\n\r",pstTMCB->ucTimerType);
    cps__oams_shcmd_printf("\n\rTimerNo :%d\n\r",pstTMCB->ucTimerNo);
   cps__oams_shcmd_printf("\n\rPno :%d\n\r",pstTMCB->ulPno);
   cps__oams_shcmd_printf("\n\rInit Ticks :%d\n\r",pstTMCB->ulOriginalCount);
   cps__oams_shcmd_printf("\n\rPresent Ticks :%d\n\r",pstTMCB->ulCount);
  cps__oams_shcmd_printf("\n\rPara :%d\n\r",pstTMCB->ulTimerParam);
    cps__oams_shcmd_printf("\n\rTickIndex :%d\n\r",pstTMCB->ulTickIndex);
   cps__oams_shcmd_printf("\n\rTickPrevId :%d\n\r",pstTMCB->ulPrev);
    cps__oams_shcmd_printf("\n\rTickNextId :%d\n\r",pstTMCB->ulNext);
    cps__oams_shcmd_printf("\n\rExpireMsgHdr :%#x\n\r",pstTMCB->ulMsgHeadAddr);
    
    return CPSS_OK;

}

/*******************************************************************************
* 函数名称:  cpss_timer_show_no_para_timer    
* 功    能:  显示指定无参定时器的控制信息     
* 相关文档: <无>                    
* 函数类型:                                    
* 参    数:                          
* 参数名称           类型                  输入/输出        描述
* ulPno                        UINT32                     IN                       纤程成描述符
* ucTimerNo                UINT8                       IN                        定时器号
* 返回值: CPSS_OK or CPSS_ERROR
*  误差     
*  
*                           
* 说   明:（可选 ，标识本函数未完成或者可能改动，在本函数相应位置要加"/TBD    
*        TBD/"批注，即留待完成）
*                                          
*******************************************************************/
INT32 cpss_timer_show_no_para_timer(UINT32 ulPno, UINT8    ucTimerNo)
{
   CPSS_VK_PROC_PCB_T            *pstPCB;
   UINT32 ulTid;
   
   pstPCB =  cpss_vk_proc_pcb_get(ulPno);

   if((NULL == pstPCB)||(ucTimerNo>=  MAX_PROC_TIMER))
    {
        cps__oams_shcmd_printf("input para error\n\r");
        return CPSS_ERROR;
    }
   ulTid = pstPCB->aulTimerId[ucTimerNo];    

   if(ulTid == CPSS_TD_INVALID)
    {
       cps__oams_shcmd_printf(" This timer not exist\n\r");
    }
   else
    {
        cpss_timer_show_id(ulTid);
    }
   return CPSS_OK;
}  


/*******************************************************************************
* 函数名称:  cpss_timer_show_queue    
* 功    能:  显示定时器队列信息    
* 相关文档: <无>                    
* 函数类型:                                    
* 参    数:                          
* 参数名称           类型                  输入/输出        描述
* ulTid                          INT32                      IN                         定时器描述符

* 返回值: 
*  误差     
*  
*                           
* 说   明:（可选 ，标识本函数未完成或者可能改动，在本函数相应位置要加"/TBD    
*        TBD/"批注，即留待完成）
*                                          
*******************************************************************/
VOID cpss_timer_show_queue(INT32 ulTid)
{
    CPSS_TM_RELTIMERQUEUE_T *pstRelTQue;
     UINT32          ulLoopVar,ulHead,ulCurrentHead;
   CPSS_TM_TMCB_T            *pstTMCB;

   if(ulTid >= 0)
    {
      cpss_timer_show_id(ulTid);
      return;
    }
   if(ulTid ==-1)
    {
          /* 获得相对定时器队列的头部 */
        pstRelTQue = (CPSS_TM_RELTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stRelTimerQueue);    
     
        printf(" g_pstCpssTmCoreData Addr = %p\n\r",g_pstCpssTmCoreData);
        printf(" pstRelTQue Addr = %p\n\r",pstRelTQue);
        printf(" Timer Id in tick Queue  List:\n\r");
        for(ulLoopVar=0; ulLoopVar < MAX_TIMER_NUM; ulLoopVar++)
        {
          
           ulHead = pstRelTQue->aulTickArray[ulLoopVar];   
           if( ulHead >= MAX_TIMER_NUM)
            {
               continue;
            }
           ulCurrentHead = ulHead;
           while(1)
            {
                pstTMCB = (CPSS_TM_TMCB_T *)&(g_pstCpssTmCoreData->astTMCB[ulHead]);
         
                printf("%d  ", ulHead);
                ulHead = pstTMCB->ulNext;
                if(ulCurrentHead == ulHead)
                    {
                       break;
                    }
           }
           
         }   
    }

    if(ulTid == -2)
    {
         printf(" Timer Id in Tid Queue  List:\n\r");
        /* 获得相对定时器队列的头部 */
        pstRelTQue = (CPSS_TM_RELTIMERQUEUE_T *)&(g_pstCpssTmCoreData->stRelTimerQueue);    
        for(ulLoopVar=0; ulLoopVar < MAX_TIMER_NUM; ulLoopVar++)
        {
          if (pstRelTQue->aucTidArray[ulLoopVar] == TRUE)
            {
               printf("%d ",ulLoopVar);
            }
             
         }  
    }
    printf(" Timer Id in Queue  List End:\n\r");
   
}

 /*******************************************************************************
* 函数名称:  cpss_timer_show_ctrl    
* 功    能:  显示定时器控制信息    
* 相关文档: <无>                    
* 函数类型:                                    
* 参    数:                          
* 参数名称           类型                  输入/输出        描述
*

* 返回值: 
*  误差     
*  
*                           
* 说   明:（可选 ，标识本函数未完成或者可能改动，在本函数相应位置要加"/TBD    
*        TBD/"批注，即留待完成）
*                                          
*******************************************************************/


VOID cpss_timer_show_ctrl(VOID)
{
  printf(" Timer manage info list :\n\r");
  printf(" ulSecFromPowerOn = %d\n\r",g_pstCpssTmCoreData->ulSecFromPowerOn);
  printf(" stTickCountsFromPowerOn.ulTickCountHigh = %d\n\r",g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountHigh);
  printf(" stTickCountsFromPowerOn.ulTickCountLow = %d\n\r",g_pstCpssTmCoreData->stTickCountsFromPowerOn.ulTickCountLow);
  printf(" stTimeFrom2K.ulSecond = %d\n\r",g_pstCpssTmCoreData->stTimeFrom2K.ulSecond);
  printf(" stTimeFrom2K.usMilliSec = %d\n\r",g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec);
  printf(" stTMCBPool.ulFreeCount = %d\n\r",g_pstCpssTmCoreData->stTMCBPool.ulFreeCount);
  printf(" stTMCBPool.ulHead = %d\n\r",g_pstCpssTmCoreData->stTMCBPool.ulHead);
  printf(" stTMCBPool.ulTail = %d\n\r",g_pstCpssTmCoreData->stTMCBPool.ulTail);
  printf(" stAbsTimerQueue.ulElementCount = %d\n\r",g_pstCpssTmCoreData->stAbsTimerQueue.ulElementCount);
  printf(" stAbsTimerQueue.ulHead = %d\n\r",g_pstCpssTmCoreData->stAbsTimerQueue.ulHead);
  printf(" stAbsTimerQueue.ulTail = %d\n\r",g_pstCpssTmCoreData->stAbsTimerQueue.ulTail);
  printf(" stRelTimerQueue.ulElementCount = %d\n\r",g_pstCpssTmCoreData->stRelTimerQueue.ulElementCount);
  printf(" ulMutexSemForTMCB= %d\n\r",g_pstCpssTmCoreData->ulMutexSemForTMCB);
  printf(" ulMutexSemForTimerQueue= %d\n\r",g_pstCpssTmCoreData->ulMutexSemForTimerQueue);
  printf(" ulSynSemForTick= %d\n\r",g_pstCpssTmCoreData->ulSynSemForTick);
  printf(" ulMutexSemForTickCounts= %d\n\r",g_pstCpssTmCoreData->ulMutexSemForTickCounts);
  printf(" ulMutexSemForSysSoftClock= %d\n\r",g_pstCpssTmCoreData->ulMutexSemForSysSoftClock);
  

}

#ifdef CPSS_DSP_CPU
 
/* 为计算CPU占用率增加 */
void timer0_period()
{
    /*    period = CLK_RATE/CLK_DIV/TICKS_PER_SEC;   */
    g_ulCpssTimer0Period = CPSS_DIV(CPSS_DIV(CLK_RATE,CLK_DIV),TICKS_PER_SEC);
}
 
void timer0_init (void)
    {
    int period;
     
    period = g_ulCpssTimer0Period;
 
    timer0_reset (period, CLK_SRC);
        
    /*int_enable ();*/
        
    g_lIsrCondVar = 0;
    g_lIsrSysTicks = 0;
    
    timer0_start (CLK_SRC);
    }

/* 为计算CPU占用率增加 */
void cpss_dsp6x_timer1_start(UINT32 ulClockSrc)
{
    UINT32 ulTimer1CntrNow;
	
	if (ulClockSrc == 0)
	{
		*(UINT32 *)(0x01980000) = 0x00000001;
	}
	else
	{
	    *(UINT32 *)(0x01980000) = 0x00000201;
	}

	ulTimer1CntrNow = *(UINT32 *)(0x01980008);
	
	g_ulCpssBusyTimeCntr = g_ulCpssBusyTimeCntr + ulTimer1CntrNow;
	
	if (ulClockSrc == 0)
	{
		*(UINT32 *)(0x01980000) = 0x000000c1;
	}
	else
	{
	    *(UINT32 *)(0x01980000) = 0x000002c1;
	}
	
	return;
}
/* 为计算CPU占用率增加 */
void cpss_dsp6x_timer1_stop(UINT32 ulClockSrc)
{
	if (ulClockSrc == 0)
	{
		*(UINT32 *)(0x01980000) = 0x00000001;
	}
	else
	{
	    *(UINT32 *)(0x01980000) = 0x00000201;
	}

	return;
}
/* 为计算CPU占用率增加 */
void cpss_dsp6x_timer1_reset(UINT32 ulPeriod,UINT32 ulClockSrc)
{
	if (ulClockSrc == 0)
	{
		*(UINT32 *)(0x01980000) = 0x00000001;
	}
	else
	{
	    *(UINT32 *)(0x01980000) = 0x00000201;
	}
 
    *(UINT32 *)(0x01980008) = 0x0;
    
    *(UINT32 *)(0x01980004) = ulPeriod;
	
	return;
}
/* 为计算CPU占用率增加 */
void timer1_init (void)
{
    int period;
     
    /*  period = TIMER0_PERIOD * 5;*/
    period = g_ulCpssTimer0Period * 5;
 
    cpss_dsp6x_timer1_reset (period, CLK_SRC);
        
}

/* 为计算CPU占用率增加 */
UINT32 cpss_calc_cpu_busy_rate(UINT32 ulBusyTime,UINT32 ulPeriod)
{
     UINT32 k;
     UINT32 ulBitResult;
     UINT32 ulResult;
     
     ulResult = 0;
     
     if (ulBusyTime >= ulPeriod)
     {
     	ulResult = 0x7f;
     	return ulResult;
     }
     
     for (k = 0; k < 7; k++)
     {
     	ulBusyTime = ulBusyTime << 1;
     	ulResult = ulResult << 1;
     	
     	if (ulBusyTime >= ulPeriod)
     	{
     	    ulBitResult = 1;
        	ulBusyTime = ulBusyTime - ulPeriod;
     	}
     	else
     	{
            ulBitResult = 0;
        }

        ulResult = ulResult + ulBitResult;
     }
     
     return ulResult;           
}
volatile UINT32 g_ulTpssIsr = 0;

interrupt void timer0_isr (void)
{

    if((CPSS_MOD(g_lIsrCondVar,1000)) == 0)
    {
        g_ulHeartBeartCntr = 1;
    }

    if((CPSS_MOD(g_lIsrCondVar,20)) == 0)
    {
        g_ulTpssIsr += 1;
    }            
    
    g_lIsrCondVar = g_lIsrCondVar + 1;
    g_lIsrSysTicks = g_lIsrSysTicks + 1;
    g_lRfnSynTicks = g_lRfnSynTicks+ 8;

    /* 为计算CPU占用率增加 */
    g_ulCpssBusyRateCalCntr = g_ulCpssBusyRateCalCntr + 1;
    
    if(g_lRfnSynTicks >= 327680)
    {
        g_lRfnSynTicks = 0;
    }
    
    /* 为计算CPU占用率增加 */   
    if(g_ulCpssBusyRateCalCntr == g_ConstCpssBusyRateCalPeriod)
    {
        *(UINT32*)(CPSS_DSP_CPU_LOAD_ADDR+4) = cpss_htonl(g_ulCpssBusyRateCalCntr);
	*(UINT32*)(CPSS_DSP_CPU_LOAD_ADDR+8) = cpss_htonl(g_ulCpssBusyTimeCntr);
	 *(UINT32*)(CPSS_DSP_CPU_LOAD_ADDR+12) = cpss_htonl(g_ulCpssTimer0Period);

	 g_ulCpssCpuBusyRate = cpss_calc_cpu_busy_rate(g_ulCpssBusyTimeCntr,g_ConstCpssBusyRateCalPeriod*g_ulCpssTimer0Period);
	
	    /* 调用CPSS的PCI函数，报告CPU占用率*/
	
	 *(UINT32*)(CPSS_DSP_CPU_LOAD_ADDR) = cpss_htonl(g_ulCpssCpuBusyRate);

	 g_ulCpssBusyTimeCntr = 0; 

	 g_ulCpssBusyRateCalCntr = 0;	
    }
    
    return;
}

int cpss_get_rfn_count()
{
    return g_lRfnSynTicks;
}

void cpss_dsp6x_start(void)
{
    g_ulCpssBusyTimeCntr = 0;          /* 为计算CPU占用率增加 */
    g_ConstCpssBusyRateCalPeriod = 5000;  /* 为计算CPU占用率增加 */
    g_ulCpssCpuBusyRate = 0;           /* 为计算CPU占用率增加 */
    g_ulCpssTimer0Period = 0;           /* 为计算CPU占用率增加 */
    g_ulCpssBusyRateCalCntr = 0;       /* 为计算CPU占用率增加 */

    timer0_period();

    int_enable ();
 
    /* 为计算CPU占用率增加 */
    
    timer1_init();
        
    timer0_init();

    return;
}
void cpss_dsp6x_reset_config (void)
{
     int_disable();
 
     cpss_dsp6x_cache_setup();
     
     return;
 
} 
#endif



 /*******************************************************************************
* 函数名称:  cpss_timer_show_all_proc    
* 功    能:  显示纤程占用的峰值信息    
* 相关文档: <无>                    
* 函数类型:                                    
* 参    数:                          
* 参数名称           类型                  输入/输出        描述
*

* 返回值: 
*  误差     
*  
*                           
* 说   明:（可选 ，标识本函数未完成或者可能改动，在本函数相应位置要加"/TBD    
*        TBD/"批注，即留待完成）
*                                          
*******************************************************************/

 VOID cpss_timer_show_all_proc()
 {
    CPSS_VK_PROC_PCB_T     *pstPCB;   
	UINT32 ulPno;
     UINT16 usLoop = 0;
   
     cps__oams_shcmd_printf("\n\r\n\r%-32s%-25s%-12s","ProcName","TimerPeakCount","GMT");
				
     for(usLoop = 0; usLoop < VK_MAX_PROC_CLASS_NUM; usLoop++)
     	{
     	   if (g_astCpssVkProcClassTbl[usLoop].bInuse == TRUE)
     	   {
            ulPno = CPSS_VK_PD(usLoop,0);
             pstPCB = cpss_vk_proc_pcb_get(ulPno);

			 if(pstPCB != NULL)
			 {
			   cps__oams_shcmd_printf("\n\r\n\r%-32s%-25d%-12d",g_astCpssVkProcClassTbl[usLoop].acName,pstPCB->usTimerPeak,pstPCB->ulTimerPeakGmt);
             }
     	    
     	   }		
     	}
     	  
 }


INT32 cpss_timer_get_sec_and_ms(UINT32 *pulSec, UINT32 *pulMs)
{
   *pulSec  = 0;
   *pulMs  = 0;
  
   if(g_pstCpssTmCoreData !=NULL)
   {
     *pulSec = g_pstCpssTmCoreData->stTimeFrom2K.ulSecond;
     *pulMs   = g_pstCpssTmCoreData->stTimeFrom2K.usMilliSec;
     return CPSS_OK;
   }  
   return CPSS_ERROR;
}


void cpss_tm_sig_rcv()
{
	sigset_t set;
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	sigprocmask(SIG_UNBLOCK, &set, NULL);

	while(1)
	{
		sleep(10000);
	}
}

INT32 cpss_tm_set_local_time(CPSS_TIME_T *pstClock)
{
	UINT32 ulSec;
	INT32 lRet;
	struct timeval stLocaltime;

	if(NULL == pstClock)
	{
		return CPSS_ERROR;
	}

	lRet = cpss_clock2gmt(pstClock, &ulSec);
	if(CPSS_OK != lRet)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_ERROR, "cpss_clock2gmt error! in cpss_tm_set_local_time, ret = %x\n", lRet);
		return lRet;
	}

	stLocaltime.tv_sec = ulSec;
	stLocaltime.tv_usec = 0;

	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_INFO, "in cpss_tm_set_local_time, second = %d\n", ulSec);

	cpss_clock_set(pstClock);

	lRet = settimeofday(&stLocaltime, 0);
	if(0 != lRet)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_ERROR, "settimeofday error! in cpss_tm_set_local_time, ret = %x\n", lRet);
		perror("settimeofday");
	}

	return CPSS_OK;
}

VOID cpss_tm_loop_task()
{
	INT32 lLoop;
	INT32 lTimeDelay;
	struct timeval stBtime;
	struct timeval stNtime;

	gettimeofday(&stBtime, NULL);

	while(1)
	{
		usleep(1000);

		gettimeofday(&stNtime, NULL);

		lTimeDelay = (int)(stNtime.tv_usec / 10000 - stBtime.tv_usec / 10000);

		if(0 == lTimeDelay)
		{
			continue;
		}
		else if(0 < lTimeDelay)
		{
			for(lLoop = 0; lLoop < lTimeDelay; lLoop++)
			{
		        cpss_vos_sem_v(g_pstCpssTmCoreData->ulSynSemForTick);

		        stBtime.tv_usec = stNtime.tv_usec;
			}
		}
		else if(0 > lTimeDelay)
		{
			for(lLoop = 0; lLoop < lTimeDelay + 100; lLoop++)
			{
		        cpss_vos_sem_v(g_pstCpssTmCoreData->ulSynSemForTick);

		        stBtime.tv_usec = stNtime.tv_usec;
			}
		}
	}

	return;

}
