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
#include "cpss_public.h"
#include "smss_public.h"

#ifdef CPSS_VOS_VXWORKS
#include "cpss_common.h"
#include "cpss_err.h"
#include "cpss_vk_sched.h"
#include "cpss_vos_task.h"
#include "cpss_vos_msg_q.h"
#include "cpss_com_drv.h"
#include "drv_public.h"
#include "tickLib.h"               
#include "wdLib.h"
#include "excLib.h"
#include "rebootLib.h"
#include "spyLib.h"
#if (CPSS_CPU_TYPE == CPSS_CPU_ARM)
#include "arch/arm/esfarm.h"
#endif
#if (CPSS_CPU_TYPE == CPSS_CPU_PPC)
#include "arch/ppc/esfppc.h"
#endif
#if (CPSS_CPU_TYPE == CPSS_CPU_I86)
#include "arch/I86/esfI86.h"
#endif

/****************************** 局部宏定义 cpss_common.h *********************/
#define KW_MAX_MSG       20
#define KW_MAX_MSG_LEN   300
#define KW_DEAD_INFO_LEN 180
#define KW_INFO_LEN      200
#define HW_DOG_WORK      (100)  

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA) ||(SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
  #define HW_DOG_DELAY_MS  (6000)       /* HW WatchDog Delay length */
 #else
#define HW_DOG_DELAY_MS  (5000)       /* HW WatchDog Delay length */
#endif

/* CPU burn time  */


#define SECONDS_TO_BURN    (2)

#define CPSS_ALMID_SW_RUNNING_ABNORMALLY  0x00020001

#define CPSS_MAX_SPY_TASKS    200

#define CPSS_LOAD_RESTART_SPY_INTERVAL       (10*60)              /* unit is second */

extern BOOL g_bCpssSbbrCurExcOccur;
/******************************* 局部常数和类型定义 **************************/
/* 消息头*/
typedef struct tagCPSS_KW_KWINFOHEAD
{
    UINT32 ulMsgId;
    UINT32 ulLength;
    UINT8  acBuf[KW_INFO_LEN];      /*根据msgId套用下面结构*/    
}CPSS_KW_KWINFOHEAD_T;

/*死循环信息*/
typedef struct tagCPSS_KW_DEADLOOPINFO
{    
    UINT32 ulTid;                   /*调度任务号*/
    UINT32 ulprocDescCurr;          /*正在调度的纤程描述符*/
    UINT32 ulMsgId;                 /* 正在处理的消息ID*/
    CPSS_COM_MSG_HEAD_T *pstAddr;                  /* 存放消息头指?CPSS_COM_MSG_HEAD_T*/
    
    #if 0
   
    UINT8  acBuf[KW_DEAD_INFO_LEN];    
    #endif
}CPSS_KW_DEADLOOPINFO_T;

/*异常信息*/
typedef struct tagCPSS_KW_EXCINFO
{
    INT32  lTid;                    /*发生异常的任务描述符*/
    INT32  lVecNum;                 /*中断向量*/    
    CPSS_ESF  *pEsf;                /* stack frame */
}CPSS_KW_EXCINFO_T;


typedef struct tagCPSS_KW_CPU_LOAD
{
  UINT32  ulticksNoContention;    /* 空闲时运行的ticks*/
   UINT32  ulBurnNoContention;    /* 空闲时运行burn的次数*/
   UINT32  ulTicksNow;          /* 当前运行的ticks */
   UINT32  ulBurnNow;           /* 当前运行burn的次数*/
   UINT32  Usage;
     UINT32 Idle;
   UINT32   Total;
}CPSS_KW_CPU_LOAD_T;

typedef struct tagCPSS_KW_SUSPENED_TASL_LIST
{
   INT32 lTid;
   INT32 lSuspendedTicks;
}CPSS_KW_SUSPENDED_TASK_LIST_T;

typedef struct
{
    CPSS_COM_PHY_ADDR_T  stPhyAddr;        /* 本处理器物理地址 */
    UINT32  ulType;                        /* 消息类型，见上面故障上报类型宏定义 */
    UINT32  ulReason;                      /* 见上面故障上报原因宏定义 */
    UINT8   pucFaultInfor[20];             /* 故障数据 */
} CPSS_SYSCTL_BFAULT_IND_MSG_T;

/* 从cpu故障时上报类型*/
#define CPSS_BFAULT_SLAVE_TO_HOST  46

/* 从cpu故障时上报的消息*/
#define CPSS_SYSCTL_BFAULT_IND_MSG       0x75010004

extern VOID smss_sysctl_send_host(UINT32 ulPd,
                           UINT32 ulMsgId,
                           UINT8 *pucMsg,
                           UINT32 ulMl);



/******************************* 全局变量定义/初始化 *************************/
extern CPSS_VK_SCHED_DESC_T    g_astCpssVkSchedDescTbl [VK_MAX_SCHED_NUM];
UINT32 g_ulCpssKwDogCount = 0;
WDOG_ID      g_CpssKwDogId;
UINT16       g_usCpssKwDelay =1;        /*watchdog delay */
UINT32      g_ulCpssKwMsgQueue ;

INT32 g_CpssLoadTaskID = 0;
INT32 g_CPssLoadTaskInterval = 0;
BOOL g_CpssLoadTaskOverFlow = FALSE;

BOOL g_bCpssTaskCreateEnd = FALSE;
extern CPSS_VOS_TASK_DESC_T    g_astCpssVosTaskDescTbl[VOS_MAX_TASK_NUM];
INT32 g_CpssKwBeingExc = 0;

   
CPSS_KW_CPU_LOAD_T   g_stCpssKwCpuLoad;

BOOL g_bCpssResetBoard = TRUE;

/* 延迟复位计数*/
INT32 g_bCpssExcCount = 0;

CPSS_KW_SUSPENDED_TASK_LIST_T g_tCpssSuspendedTaskList[32];
BOOL g_bCpssDetectSuspend = FALSE;

extern  BOOL g_bCpssRecvProcEndInd ;


extern BOOL g_bCpssSbbrExcOccur;
extern BOOL g_bCpssSbbrCurExcOccur  ;
extern BOOL g_bCpssCloseException;


extern  VOID cpss_sbbr_output_dead_info
(
 UINT32 ulTid 
);
extern VOID cpss_sbbr_exception_proc
 (
 INT32 lTid,
 INT32 lVecNum,
 CPSS_ESF  *pEsf
 );
extern VOID cpss_sbbr_clr_abnormal_reset_flag
(
 VOID
);

extern  VOID cpss_sbbr_set_abnormal_reset_flag(VOID);

extern VOID cpss_cpu_load_read( VOID);
extern VOID cpss_sbbr_write_exc_info_disk();
/*extern VOID cpss_kw_set_task_run_time(VOID);*/

extern INT32 cpss_fs_fd_close();


extern UINT32 cpss_sbbr_get_userid(VOID);

UINT32 g_ulCpssKwUserId = -1;

BOOL g_bCpssRecordDog =  TRUE;

UINT32 g_ulCpssFeedDogCount = 0;

extern INT32 cpss_drv_wd_callback(VOID);

extern UINT spyIncTicks;
extern UINT spyIdleIncTicks;
extern UINT spyKernelIncTicks;
extern UINT spyTotalTicks;
extern UINT spyInterruptTicks;
extern UINT spyKernelTicks;
extern UINT spyIdleTicks;
extern UINT spyInterruptIncTicks;


UINT32  g_ulCpssSpyInterval = 1;
UINT32 g_ulCpssShowInterval = 0;

BOOL g_bCpssOpenPrtSpy = FALSE;


 char *cpss_spyFmt1 = "%-12.12s %-10.10s %8x  %3d   %3d%% (%8d)  %3d%% (%8d) %-10s\n";
 char *cpss_spyFmt2 = "%-12.12s %-10.10s %8s  %3s   %3d%% (%8d)  %3d%% (%8d)\n";

extern CHAR *g_pcSbbrExcInfo;

extern  BOOL g_bCpssWithSbbr ;
/******************************* 局部函数原型声明 ****************************/
INT32 cpss_kw_init(VOID);
VOID cpss_kw_routine(VOID);
VOID cpss_kw_dead_loop_handle(CPSS_KW_DEADLOOPINFO_T  *pstDeadLoopInfo);
VOID cpss_kw_exception_handle(INT32 lTaskId,INT32 lVecNum, CPSS_ESF  *pEsf);

extern VOID cpss_sbbr_set_exc_flag
(
 UINT8 ucFlag
);


VOID cpss_kw_save_all_task_load();
extern VOID cpss_sbbr_save_Msched_task_stack(VOID);
extern VOID cpss_sbbr_save_task_stack(INT32 lTid);
extern cpss_com_local_is_mmc();

/*******************************************************************************
* 函数名称: cpss_kw_init
* 功    能: 内核监控初始
* 函数类型: 
* 参    数: 无
* 函数返回: cpss_ok ,cpss_error
* 说    明: 
*******************************************************************************/
INT32 cpss_kw_init()
{
    INT32 lResult = CPSS_OK;

#if 1  
#ifdef CPSS_HOST_CPU

   #if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA) ||(SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
    
    if(drv_wd_isr_callback_reg(cpss_drv_wd_callback) != CPSS_OK)
    {
      printf("\n\r drv_wd_isr_callback_reg return fail ");
      return CPSS_ERROR;
    }
   #endif	
  
    if(drv_wd_control(WATCHDOG_OPEN,HW_DOG_DELAY_MS ) ==ERROR)
    {
        printf("\n\rdrv_wd_control return error,ha init fail ");
        return CPSS_ERROR;
    }
 #endif 
 #endif
  
  
    g_CpssKwDogId = wdCreate();   /*创建软件狗*/
    if(NULL == g_CpssKwDogId)
    {
        cpss_output(CPSS_MODULE_KW,CPSS_PRINT_FATAL ," cpss Create KW Watch Dog Fail \n\r");
        return CPSS_ERR_KW_SYS_CALL_FAIL;
    }
    lResult = wdStart(g_CpssKwDogId, g_usCpssKwDelay, (FUNCPTR)cpss_kw_routine, 0); /*启动软件狗*/
    if(CPSS_OK != lResult)
    {
        cpss_output(CPSS_MODULE_KW,CPSS_PRINT_FATAL ," cpss wdStart KW Watch Dog Fail \n\r");
        return CPSS_ERR_KW_SYS_CALL_FAIL;
    }
    
    g_ulCpssKwMsgQueue =  cpss_vos_msg_q_create(" MsgQueue for KW ",KW_MAX_MSG,KW_MAX_MSG_LEN,VOS_MSG_Q_FIFO); /*创建消息队列1*/
    
    if(VOS_MSG_Q_DESC_INVALID == g_ulCpssKwMsgQueue)
    {
        cpss_output(CPSS_MODULE_KW,CPSS_PRINT_FATAL ," cpss create msgQueue Fail \n\r");        
        return CPSS_ERR_KW_SYS_CALL_FAIL;
    }    
    excHookAdd((FUNCPTR)cpss_kw_exception_handle);  /* 增加hook函数*/
     cpss_mem_memset(g_tCpssSuspendedTaskList , 0, 32 * sizeof(CPSS_KW_SUSPENDED_TASK_LIST_T));
    return CPSS_OK;     
}

/*******************************************************************************
* 函数名称: cpss_kw_dead_loop_handle
* 功    能: 死循环处理
* 函数类型: 
* 参    数:              类型                      IN/OUT     描述
* pstDeadLoopInfo      CPSS_KW_DEADLOOPINFO_T  * ,          IN       死循环信息
* 函数返回: 
* 说    明: 
*******************************************************************************/
VOID cpss_kw_dead_loop_handle(CPSS_KW_DEADLOOPINFO_T *pstInfo )
{    
   CPSS_COM_MSG_HEAD_T *pstData = NULL;
   
   pstData = pstInfo->pstAddr;

   if(NULL != pstData)
    {
    /* call save black box */     
    cpss_output(CPSS_MODULE_KW,CPSS_PRINT_INFO,"\n\r\n\r");
    cpss_output(CPSS_MODULE_KW,CPSS_PRINT_INFO," CPSS Moniter Dead Loop Occur @ Tno = %d ProcDesc = 0x%x MsgId = 0x%x\n",
    pstInfo->ulTid,pstInfo->ulprocDescCurr,pstInfo->ulMsgId);   
    cpss_sbbr_output_dead_info(pstInfo->ulTid);
#if 0
    drv_wd_control(WATCHDOG_CLOSE,HW_DOG_DELAY_MS);  /*关狗*/
 #ifdef CPSS_HOST_CPU  
    drv_board_reset();   
 #endif
#endif 
    }
}

/*******************************************************************************
* 函数名称: cpss_kw_send_msg_to_smss
* 功    能: 向smss发送 SMSS_CPU_FAULT_IND_MSG_T 消息
* 函数类型: 
* 参    数: 无
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_kw_send_msg_to_smss(UINT32 ulMsgId,UINT32 ulReason)
{
   CPSS_SYSCTL_BFAULT_IND_MSG_T stBFault;
   CPSS_COM_MSG_HEAD_T        tData;
 
   SMSS_CPU_FAULT_IND_MSG_T tFault;
   CPSS_COM_MSG_HEAD_T *     pstData = &tData;

 #ifdef CPSS_HOST_CPU
    tFault.ulReason = ulReason;
   
    pstData->stDstProc.ulPd = SMSS_SYSCTL_PROC;
    pstData->stSrcProc.ulPd = 0;
    pstData->ulMsgId = ulMsgId;
    pstData->ulLen = sizeof(SMSS_CPU_FAULT_IND_MSG_T);   /* 提供给应用的结构是G1接口中*/
    pstData->ucPriFlag = VOS_MSG_PRI_NORMAL;
    pstData->pucBuf = (UINT8*)&tFault;
    pstData->ucShareFlag = 0;
    if(cpss_com_send_local(pstData) != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_KW,CPSS_PRINT_FATAL ," send exception msg to smss fail\n\r");
    }
    else
    {
        cpss_output(CPSS_MODULE_KW,CPSS_PRINT_INFO," send exception msg to smss success\n\r");
    }
 #else
        /* 若为从CPU，向主CPU报故障 */
        stBFault.ulReason = cpss_htonl(ulReason);
        cpss_com_phy_addr_get(&stBFault.stPhyAddr);
        stBFault.ulType = CPSS_BFAULT_SLAVE_TO_HOST;
        stBFault.ulType = cpss_htonl(stBFault.ulType);
        smss_sysctl_send_host(SMSS_SYSCTL_PROC, CPSS_SYSCTL_BFAULT_IND_MSG,
                (UINT8 *)&stBFault, sizeof(stBFault));
        cpss_output(CPSS_MODULE_KW,CPSS_PRINT_INFO," \n\rsend exception msg to host");

  
  #endif

}

/*******************************************************************************
* 函数名称: cpss_kw_send_alarm_to_oams
* 功    能: 向oams发送 告警

* 函数类型: 
* 参    数: 
* ulId                  任务描述符或纤程描述符
* ulVecNo            中断向量
* ulArmSubNo       子告警号
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_kw_send_alarm_to_oams(UINT32 ulId, UINT32 ulVecNo, UINT32 ulAlarmSubNo)
{
    CPS__OAMS_AM_ORIGIN_ALARM_T stOamsAlarm;
    UINT32 *pulArg;
    stOamsAlarm.ulAlarmNo = cpss_htonl(CPSS_ALMID_SW_RUNNING_ABNORMALLY);
    stOamsAlarm.ulAlarmSubNo = cpss_htonl(ulAlarmSubNo);
    stOamsAlarm.enAlarmType = cpss_htonl(CPS__OAMS_ALARM_OCCUR);
#ifndef SWP_FNBLK_BRDTYPE_ABOX
    cps__oams_moid_get_local_moid(CPS__OAMS_MOID_TYPE_CPU,&stOamsAlarm.stFaultMoID);
#endif
    cpss_mem_memset(&stOamsAlarm.aucExtraInfo, 0, CPS__OAMS_AM_ALARM_EXTRA_INFO_LEN); 
    
    pulArg = (UINT32 *)stOamsAlarm.aucExtraInfo;
    *pulArg = cpss_htonl(ulId);
     pulArg++;
   if(ulAlarmSubNo == 1)
       {
          *pulArg = cpss_htonl(ulVecNo);
       }
#ifndef SWP_FNBLK_BRDTYPE_ABOX       
    cps__oams_am_send_alarm(&stOamsAlarm);
#endif   
    cpss_output(CPSS_MODULE_KW,CPSS_PRINT_INFO," send arlm to oams ulId = 0x%x ulArmSubNo = %d\n\r", ulId,ulAlarmSubNo);
}


/*******************************************************************************
* 函数名称: cpss_kw_task
* 功    能: 内核监控任务
* 函数类型: 
* 参    数: 无
* 函数返回: 
* 说    明: 
*******************************************************************************/

void cpss_kw_task(void)
{
    CPSS_KW_KWINFOHEAD_T *pstKWinfo;    
    CHAR aucBuf[300];
    INT32 lResult;
    
    if(cpss_kw_init() != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_KW,CPSS_PRINT_FATAL ," cpss kw module init Fail \n\r");
        return;
    }    

     taskPrioritySet(taskIdSelf(), 1);

    g_ulCpssKwUserId = cpss_sbbr_get_userid();
    
    pstKWinfo = (CPSS_KW_KWINFOHEAD_T *)aucBuf;
    memset(pstKWinfo, 0, sizeof(CPSS_KW_KWINFOHEAD_T));    
    while(1)
    {
        lResult = cpss_vos_msg_q_receive(g_ulCpssKwMsgQueue ,aucBuf ,sizeof(CPSS_KW_KWINFOHEAD_T),WAIT_FOREVER);
        if(CPSS_ERROR != lResult)
        {
            switch(pstKWinfo->ulMsgId)
            {
            case CPSS_KW_DEAD_LOOP_NOTIFY_MSG:      
                {
                    CPSS_KW_DEADLOOPINFO_T  *pstDeadLoopInfo;
                    pstDeadLoopInfo = (CPSS_KW_DEADLOOPINFO_T  *)pstKWinfo->acBuf;
                    cpss_kw_dead_loop_handle(pstDeadLoopInfo); 
 
	  #if 0
             cpss_kw_send_alarm_to_oams( pstDeadLoopInfo->ulprocDescCurr, 0, 0);
	  #endif
              #if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA) ||(SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
    
                  cpss_sbbr_write_exc_info_disk();
             #endif
	  #ifndef CPSS_HOST_CPU	 
             cpss_kw_send_msg_to_smss(SMSS_CPU_FAULT_IND_MSG, CPSS_RESET_SMSS_FAULT_DETECT);
	  #endif          
    
             g_bCpssExcCount = 3 ;

                  g_bCpssCloseException =TRUE;  /* 关闭监守保证单板在不被复位的情况下,记录有用信息*/
         
                }
                break;                
            case CPSS_KW_DOG_TIMER_EXPIRE_MSG:
                {
                        
                  #if 1  
             #ifdef CPSS_HOST_CPU          
                    {
                      UINT32 ulArgs[4];
                        /*喂狗，避免硬件复位*/
                        drv_wd_control(WATCHDOG_FEED_SET, HW_DOG_DELAY_MS);
                        cpss_output(CPSS_MODULE_KW,CPSS_PRINT_INFO ," \n\r drv_wd_control feed ");
                        
                       if(g_bCpssRecordDog == TRUE)     
                        cpss_sbbr_write_text(g_ulCpssKwUserId,ulArgs,"tick %d cpss feed watchDog  %d recv isr count %d  msg queue id %d feed count %d  \n\r",tickGet(),g_CpssKwDogId,g_ulCpssKwDogCount,g_ulCpssKwMsgQueue,g_ulCpssFeedDogCount+1);
                                
			   g_ulCpssFeedDogCount++;
                    }
                  #endif
             #endif          
                    
             if(g_bCpssExcCount !=  0)
                 {
                       if(-- g_bCpssExcCount <=0)
                           {
 
                              cpss_fs_fd_close();
    
                          #ifdef CPSS_HOST_CPU
                             if(TRUE == g_bCpssResetBoard)   
                              {
                                  cpss_output(CPSS_MODULE_KW,CPSS_PRINT_INFO,"cpss find exception and reset board\n\r");
                                drv_board_reset();  
                              }             
                          #endif        
 
                           }
                 }
             cpss_cpu_load_read();      
                }
                break;                
            case CPSS_KW_EXCEPTION_NOTIFY_MSG:
                {    
                    CPSS_KW_EXCINFO_T *pstExcInfo;
                    pstExcInfo = (CPSS_KW_EXCINFO_T *)pstKWinfo->acBuf;      
                    cpss_output(CPSS_MODULE_KW,CPSS_PRINT_INFO,"CPSS Moniter Exception Occur  @ Tid = 0x%x\n",pstExcInfo->lTid);                    

            if(g_CpssKwBeingExc == 0)   /*只处理一次异常*/
            {
              cpss_sbbr_exception_proc( pstExcInfo->lTid, pstExcInfo->lVecNum,pstExcInfo->pEsf);
               }  
	#if 0		
  
             cpss_kw_send_alarm_to_oams( pstExcInfo->lTid, pstExcInfo->lVecNum, 1);
	#endif
           #if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA) ||(SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
    
            cpss_sbbr_write_exc_info_disk();
         #endif
	  #ifndef CPSS_HOST_CPU	 
             cpss_kw_send_msg_to_smss(SMSS_CPU_FAULT_IND_MSG, CPSS_RESET_SMSS_FAULT_DETECT);        
	#endif
            g_bCpssExcCount = 3;
            /*g_CpssKwBeingExc ++;*/

             g_bCpssCloseException =TRUE;  /* 关闭监守保证单板在不被复位的情况下,记录有用信息*/

                }
                break;                
            default:
                cpss_output(CPSS_MODULE_KW,CPSS_PRINT_INFO,"Moniter Task Receive Unknown MsgId = 0x%x",pstKWinfo->ulMsgId);
                break;
            }/* end switch */            
        }
	  else
	  {
	      UINT32  ulArgs[4];
	         cpss_sbbr_write_text(0,ulArgs,"call cpss_vos_msg_q_receive return %d msg queue id %d \n\r",errnoGet(),g_ulCpssKwMsgQueue);
                    
	  }
    }/* end while(1) */    
}

/*******************************************************************************
* 函数名称: cpss_kw_find_suspended_task
* 功    能: 检查当前是否有任务被suspended
* 函数类型: 
* 参    数: 
* 函数返回: 
* 说    明: 
*******************************************************************************/

INT32 cpss_kw_find_suspended_task()
{
    UINT16 usLoop ;
    for(usLoop = 1; usLoop < VOS_MAX_TASK_NUM; usLoop++)
    {
      if(g_astCpssVosTaskDescTbl[usLoop].bInuse == TRUE)
        {
           if(taskIsSuspended(g_astCpssVosTaskDescTbl[usLoop].taskId )== TRUE)
               {
                 return g_astCpssVosTaskDescTbl[usLoop].taskId;
               }
         }
    }
   return VOS_TASK_DESC_INVALID;
}


/*******************************************************************************
* 函数名称: cpss_kw_apped_suspended_task_list
* 功    能: 添加被suspended的任务到列表
* 函数类型: 
* 参    数: 
* 函数返回: 
* 说    明: 
*******************************************************************************/


VOID cpss_kw_apped_suspended_task_list
(
 INT32 lTid
)
{
  UINT16 usLoop;

  for(usLoop = 0; usLoop < 32; usLoop++)
      {
         if(g_tCpssSuspendedTaskList[usLoop].lTid == lTid)
         {
             logMsg(" 1 append suspended task id 0x%x @ index %d ticks = %d\n\r",  lTid,usLoop,g_tCpssSuspendedTaskList[usLoop].lSuspendedTicks,0,0,0);
                 
             g_tCpssSuspendedTaskList[usLoop].lSuspendedTicks++;
        return;     
         }
         
      }
  
   for(usLoop = 0; usLoop < 32; usLoop++)
      {
         if(g_tCpssSuspendedTaskList[usLoop].lTid == 0)
         {
            g_tCpssSuspendedTaskList[usLoop].lTid = lTid;
            g_tCpssSuspendedTaskList[usLoop].lSuspendedTicks = 0;        
         logMsg(" 2 append suspended task id 0x%x @ index %d ticks = %d\n\r",  lTid,usLoop,g_tCpssSuspendedTaskList[usLoop].lSuspendedTicks,0,0,0);
            break;
         }
       }
}

/*******************************************************************************
* 函数名称: cpss_kw_suspended_task_time_expire
* 功    能: 被suspended的任务是否超过时限
* 函数类型: 
* 参    数: 
* 函数返回: 
* 说    明: 
*******************************************************************************/

BOOL cpss_kw_suspended_task_time_expire(INT32 lTid)
{
     UINT16 usLoop;

    
    for(usLoop = 0; usLoop < 32; usLoop++)
      {
          if(g_tCpssSuspendedTaskList[usLoop].lTid == lTid)
         {
              if(g_tCpssSuspendedTaskList[usLoop].lSuspendedTicks >= 10 * sysClkRateGet())
              {
                 g_tCpssSuspendedTaskList[usLoop].lSuspendedTicks = 0;
                 return TRUE;              
              }
              
         }
    }
    return FALSE;    
}
VOID cpss_kw_show_suspended_task()
{
     UINT16 usLoop;

    for(usLoop = 0; usLoop < 32; usLoop++)
        {
         if(g_tCpssSuspendedTaskList[usLoop].lTid !=0)
             {
                 printf(" tid 0x%x  ",g_tCpssSuspendedTaskList[usLoop].lTid);
             }
        }
}


/*******************************************************************************
* 函数名称: cpss_kw_routine
* 功    能: 软件狗中断处理
* 函数类型: 
* 参    数: 
* 函数返回: 
* 说    明: 
*******************************************************************************/
UINT32 g_ulCpssKwDogCountEnd  = 0;
VOID cpss_kw_routine(VOID)
{    
    INT32  lResult;
    INT32  lTid;
    VK_SCHED_ID schedId;
    CPSS_KW_KWINFOHEAD_T stKWinfo,stKWDogInfo;
    CPSS_KW_DEADLOOPINFO_T  *pstDeadLoopInfo ;
    static UINT16  usHardDogCount;    
    CPSS_COM_MSG_HEAD_T *    pstData;
    UINT32 ulArgs[4];
    
    pstDeadLoopInfo = (CPSS_KW_DEADLOOPINFO_T  *)stKWinfo.acBuf;    
    g_ulCpssKwDogCount++;                   /* 统计软件狗计数*/    
    lResult = wdStart(g_CpssKwDogId, g_usCpssKwDelay, (FUNCPTR)cpss_kw_routine, 0);    
    if (CPSS_OK != lResult)
    {        
        logMsg("start WatchDog in cpss_kw_routine function fail\n",
            0, 0, 0, 0, 0, 0);
	 cpss_sbbr_write_text(0,ulArgs, "wdstart fail DogId = %d Delay = %d \n\r",g_CpssKwDogId,g_usCpssKwDelay );	
    }
    else
    {
    /*     logMsg("start WatchDog  Success\n",
        0, 0, 0, 0, 0, 0) ;*/
    } 
      
    /* 如果当前调度的不是load task */
   lTid = taskIdSelf ();

 #if 0  
    if(g_CpssLoadTaskID != taskIdSelf ())
        {
          g_CPssLoadTaskInterval++; 
      /* 如果在SECONDS_TO_BURN 秒内没有调度load task */      
         if(g_CPssLoadTaskInterval >= SECONDS_TO_BURN * sysClkRateGet())
         {
                    g_CPssLoadTaskInterval = 0;
              /* 设置占用率达到100*/            
                    g_CpssLoadTaskOverFlow = TRUE;  
         }
        
          
        }
    else    /* 如果load task 得到调度*/
       {
                 g_CPssLoadTaskInterval = 0;
                 g_CpssLoadTaskOverFlow = FALSE;  
       }
#endif       

#if 0    

    /* 所有任务创建完毕*/
    if(g_bCpssTaskCreateEnd == TRUE)
        {
            INT32 lTaskId ;
        lTaskId = cpss_kw_find_suspended_task();

           if(lTaskId != VOS_TASK_DESC_INVALID)
               {
                   g_bCpssDetectSuspend = TRUE;
                   cpss_kw_apped_suspended_task_list(lTaskId);
            if(cpss_kw_suspended_task_time_expire(lTaskId) == TRUE)
                {
                    cpss_kw_exception_handle(lTaskId, 0xffffffff ,NULL);
                }        
            
               }
        else
        {
          
                 if(g_bCpssDetectSuspend == TRUE)
                 {
                   logMsg("clear suspended tash info\n\r",0,0,0,0,0,0);
       
                  cpss_mem_memset(g_tCpssSuspendedTaskList , 0, 32 * sizeof(CPSS_KW_SUSPENDED_TASK_LIST_T));
                  g_bCpssDetectSuspend = FALSE;
            }          
        }

        }

#endif
  {
    UINT32 lExcId;
    BOOL bIsProc;    
    lExcId = cpss_kw_find_exception_occur( &bIsProc);
  
    if(    lExcId != VOS_TASK_DESC_INVALID)
        {
           logMsg(" cpss_kw_find exception occur lExcid = 0x%x bisProc = %d",
                   lExcId,
                   bIsProc,
                   0,0,0,0);
           
            if(bIsProc == FALSE)
                {
                  cpss_kw_exception_handle(lExcId, 0xffffffff ,NULL);
                }
        else
        {
             schedId = g_astCpssVkSchedDescTbl[lExcId].tSchedId;   /* 获得当前控制结构*/
             if(NULL !=schedId->pIpcMsgCurr)  
                {            
                        pstData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (schedId->pIpcMsgCurr);
                        stKWinfo.ulMsgId  = CPSS_KW_DEAD_LOOP_NOTIFY_MSG;
                        stKWinfo.ulLength = sizeof(CPSS_KW_DEADLOOPINFO_T);
                        pstDeadLoopInfo->pstAddr = pstData;
                        pstDeadLoopInfo->ulTid     = lExcId;
                        pstDeadLoopInfo->ulprocDescCurr = schedId->ulProcDescCurr;
                        pstDeadLoopInfo->ulMsgId =pstData->ulMsgId;
                        lResult = cpss_vos_msg_q_send(g_ulCpssKwMsgQueue,(CHAR *)&stKWinfo,sizeof(CPSS_KW_KWINFOHEAD_T),NO_WAIT,VOS_MSG_PRI_NORMAL);
                        if(lResult == CPSS_OK)
                        {
                        }
                        else
                        {
                            logMsg("\ncpss_kw_routine msgQsend Fail \n",0,0,0,0,0,0);
                        }
                    
                }/* end if(NULL != */      
        }
        }
  }        
    /*发送超时消息到监守任务启动喂狗*/
    if(usHardDogCount++ >= HW_DOG_WORK)
    {
       
	  INT32 lRet= CPSS_OK;
        usHardDogCount = 0;
        stKWDogInfo.ulMsgId = CPSS_KW_DOG_TIMER_EXPIRE_MSG;
        lRet= cpss_vos_msg_q_send(g_ulCpssKwMsgQueue,(CHAR *)&stKWDogInfo,sizeof(CPSS_KW_KWINFOHEAD_T),NO_WAIT,VOS_MSG_PRI_NORMAL);
	  if(	lRet != CPSS_OK)
        	{
                cpss_sbbr_write_text(0,ulArgs, "cpss_kw_routine call msg_q_send fail errno = %d\n\r", errnoGet());
        	}
    } 
    g_ulCpssKwDogCountEnd++;

  /*  cpss_kw_set_task_run_time();*/


#if 0
  
  /* 如果没有收到纤程激活完成指示消,则对纤程的时间不予监视*/
    if(g_bCpssRecvProcEndInd == FALSE)
    {
         logMsg("cpss not recv proc active complete ind\n\r",0,0,0,0,0,0);
         return;
    }
           
    ulCurrentTid = cpss_vk_sched_desc_self();         /* 获得当前调度任务号*/    
    if(VOS_TASK_DESC_INVALID != ulCurrentTid)
    {        
        schedId = g_astCpssVkSchedDescTbl[ulCurrentTid].tSchedId;   /* 获得当前控制结构*/
        if(NULL !=schedId->pIpcMsgCurr)  
        {            
            schedId->lTick--;                                 /* 执行时间--*/            
            if(schedId->lTick <= 0)                            /* 该纤程一直没有被调度,则进行死循环处理*/
            {
                pstData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (schedId->pIpcMsgCurr);
                stKWinfo.ulMsgId  = CPSS_KW_DEAD_LOOP_NOTIFY_MSG;
                stKWinfo.ulLength = sizeof(CPSS_KW_DEADLOOPINFO_T);
                pstDeadLoopInfo->pstAddr = pstData;
                pstDeadLoopInfo->ulTid     = ulCurrentTid;
                pstDeadLoopInfo->ulprocDescCurr = schedId->ulProcDescCurr;
                pstDeadLoopInfo->ulMsgId =pstData->ulMsgId;
               
               /* pstDeadLoopInfo->ulTid = ulCurrentTid;
                pstDeadLoopInfo->ulMsgId =pstData->ulMsgId;
                pstDeadLoopInfo->ulprocDescCurr = schedId->ulProcDescCurr;*/
                /*memcpy(pstDeadLoopInfo->acBuf, pstData->pucBuf,KW_DEAD_INFO_LEN);*/
                schedId->lTick = 10*sysClkRateGet();            /*重新设置初始监视时间*/  
                lResult = cpss_vos_msg_q_send(g_ulCpssKwMsgQueue,(CHAR *)&stKWinfo,sizeof(CPSS_KW_KWINFOHEAD_T),NO_WAIT,VOS_MSG_PRI_NORMAL);
                if(lResult == CPSS_OK)
                {
                }
                else
                {
                    logMsg("\ncpss_kw_routine msgQsend Fail \n",0,0,0,0,0,0);
                }
            }
        }/* end if(NULL != */         
    }/* end if(VOS_TASK_DESC_INVALID != */    
  #endif  
        
}    

/*******************************************************************************
* 函数名称: cpss_kw_exception_handle
* 功    能: 异常中断处理
* 函数类型: 
* 参    数:
* 名称          类型      输入/输出        描述
* lTaskId       INT32      IN              发生异常的任务id
* lVecNum       INT32      IN              发生异常的中断向量
* pEsf          ESFPPC  *  IN              发生异常记录的堆栈
* 函数返回: 
* 说    明: 
*******************************************************************************/
VOID cpss_kw_exception_handle(INT32 lTaskId, INT32 lVecNum, CPSS_ESF  *pEsf)
{
    INT32    lResult;
    CPSS_KW_KWINFOHEAD_T stKWinfo;
    CPSS_KW_EXCINFO_T  *pstExcInfo;
    pstExcInfo = (CPSS_KW_EXCINFO_T  *)stKWinfo.acBuf;    
    stKWinfo.ulMsgId = CPSS_KW_EXCEPTION_NOTIFY_MSG;
    stKWinfo.ulLength = sizeof(CPSS_KW_DEADLOOPINFO_T);
    
    pstExcInfo->lTid = (UINT32)lTaskId;
    pstExcInfo->lVecNum = lVecNum;
    pstExcInfo->pEsf   = pEsf;
    lResult = cpss_vos_msg_q_send(g_ulCpssKwMsgQueue,(CHAR *)&stKWinfo,sizeof(CPSS_KW_KWINFOHEAD_T),NO_WAIT,VOS_MSG_PRI_NORMAL);
    if(lResult == CPSS_OK)
    {  
    }
    else
    {
        logMsg("\ncpss_kw_routine msgQsend Fail \n",0,0,0,0,0,0);
    }    
} 


VOID  cpss_kw_save_all_task_call_stack(VOID)
{
    UINT16 usLoop ;
    for(usLoop = 1; usLoop < VOS_MAX_TASK_NUM; usLoop++)
    {
      if(g_astCpssVosTaskDescTbl[usLoop].bInuse == TRUE)
        {
            cpss_sbbr_save_task_stack(g_astCpssVosTaskDescTbl[usLoop].taskId);

	  }
    }
}

/***********************************************************************
 * 函数名: INT32 cpss_reset(VOID)
 * 功能: 系统reboot
 * 相关文档
 * 函数类型
 * 参数:                                         
 * 参数名称          类型            输入/输出        描述
 * 
 * 输出参数:无         
 * 返回:无
***********************************************************************/

INT32 cpss_reset
( 
    UINT32 ulCause
)
{
 /* 正常启动,清异常标志*/

 /* 如果当前没有异常发生且上次异常已上传完*/
 if((g_bCpssSbbrExcOccur == FALSE)&&(g_bCpssSbbrCurExcOccur ==FALSE))
 {
  cpss_sbbr_clr_abnormal_reset_flag();
 } 
 if(ulCause != CPSS_RESET_CPS__OAMS_FORCE)
 {
    UINT32 ulArgs[4];
    cpss_sbbr_write_text(0, ulArgs,"GMT %d Reset Board Casue:0x%x\n\r",cpss_gmt_get(),ulCause);
    cpss_kw_save_all_task_load();	
/* 为了避免记录调用栈时运行时间过长，关闭监守*/	

/* 不是二级交换板和时钟板才记录所有任务调用栈*/

#if((SWP_PHYBRD_TYPE != SWP_PHYBRD_MASA)&&(SWP_PHYBRD_TYPE != SWP_PHYBRD_NSCA))
  if(cpss_com_local_is_mmc() != TRUE)        /* GCPA 主用不记 */
   {
   g_bCpssCloseException = TRUE;	
    cpss_kw_save_all_task_call_stack();
    cpss_sbbr_save_Msched_task_stack();
   g_bCpssCloseException = FALSE;		
  }	 
#endif   
  
    cpss_sbbr_set_abnormal_reset_flag();

    cpss_sbbr_set_exc_flag(1);

 }

  cpss_fs_fd_close();

#ifdef CPSS_HOST_CPU
  

   if(TRUE == g_bCpssResetBoard)   
    {
      drv_board_reset();                         
    }  

#endif  
    return CPSS_OK;
}

/***********************************************************************
 * 函数名: VOID cpss_cpu_burn(VOID)
 * 功能: cpu持续工作
 * 相关文档
 * 函数类型
 * 参数:                                         
 * 参数名称          类型            输入/输出        描述
 * 
 * 输出参数:无         
 * 返回:无
***********************************************************************/

VOID cpss_cpu_burn(VOID)
{
   UINT32 j = 0,i;
   for(i = 0;i<5;i++)
   {
    
     j = (i+1)*(i+1);
     
   }
}


/***********************************************************************
 * 函数名: VOID cpss_cpu_load_init(VOID)
 * 功能: cpu占用率初始化
 * 相关文档
 * 函数类型
 * 参数:                                         
 * 参数名称          类型            输入/输出        描述
 * 
 * 输出参数:无         
 * 返回:无
***********************************************************************/



INT32 cpss_cpu_load_init(VOID)
{
    
    
      return CPSS_OK;
#if 0    
   UINT32 ulTickStart;
   UINT32 tickNow;

   
   
   ulTickStart = tickGet();
   taskLock();
   /*wait for a tick*/
   while(ulTickStart==(tickNow = tickGet())) {;}

   ulTickStart = tickNow;
 
   /* 计算空闲时在SECONDS_TO_BURN时间内运行的次数*/
   while(1)
   {
      if((tickGet() - ulTickStart) >= (SECONDS_TO_BURN * sysClkRateGet()))
      {
         break;
      }
      cpss_cpu_burn();
      g_stCpssKwCpuLoad.ulBurnNoContention++;
   }
 
   taskUnlock();
    g_stCpssKwCpuLoad.ulticksNoContention = SECONDS_TO_BURN * sysClkRateGet() ;
    
    return CPSS_OK;
  
 #endif   
 
}



/*******************************************************************************
* 函数名: UINT32 cpss_cpu_usage_get() 
 * 功能: 计算单板运行程序占用率接口函数
* 相关文档
 * 函数类型
 * 参数:                                         
 * 参数名称          类型            输入/输出        描述
 * 
 * 输出参数:无         
 * 返回:无
***********************************************************************/
INT32 cpss_cpu_usage_get(UINT32 *pulUsage)
{


   *pulUsage = (UINT32)g_stCpssKwCpuLoad.Usage;

    return CPSS_OK;
 
         
}


/*******************************************************************************
* 函数名: UINT32 cpss_kw_cpu_usage_get() 
 * 功能: 计算单板运行程序占用率
* 相关文档
 * 函数类型
 * 参数:                                         
 * 参数名称          类型            输入/输出        描述
 * 
 * 输出参数:无         
 * 返回:无
***********************************************************************/


INT32 cpss_kw_cpu_usage_get(VOID)
{
    UINT32 temp,ticksNow;
    CPSS_KW_CPU_LOAD_T stTempLoad;
  
    stTempLoad = g_stCpssKwCpuLoad;
    stTempLoad.ulBurnNoContention /=100;
    stTempLoad.ulBurnNow /=100;
    ticksNow = stTempLoad.ulTicksNow;
  
   if(0 == stTempLoad.ulBurnNow)
    {
       return CPSS_ERROR;
    }
   temp = (100*stTempLoad.ulBurnNoContention)/ stTempLoad.ulBurnNow;
   ticksNow *=  temp;
 

  /*实际的ticks - 空闲时的ticks */
  temp = ticksNow - 100 * stTempLoad.ulticksNoContention;

  if(0 == ticksNow)
    {
       return CPSS_ERROR;
    }
  temp =  (100 * temp)/ticksNow ;

  if((temp<=0) ||(temp>100)) 
    {
      g_stCpssKwCpuLoad.Usage = 1;
      
    }  
   else
    {
         if(temp < 92)
         {
          /*  temp+=8;*/
         }
         g_stCpssKwCpuLoad.Usage = temp;

    }
    return CPSS_OK;
          
}

/*******************************************************************************
* 函数名:  cpss_get_cpu_load() 
 * 功能: 调测函数
* 相关文档
 * 函数类型
 * 参数:                                         
 * 参数名称          类型            输入/输出        描述
 * 
 * 输出参数:无         
 * 返回:无
***********************************************************************/


VOID cpss_get_cpu_load(VOID)
{
   
    
            printf("\nCPSS: Current CPU load = [%d]\n",g_stCpssKwCpuLoad.Usage);      
  


   
}

/*******************************************************************************
* 函数名: VOID cpss_spy_init(VOID)
 * 功能: 启动spy任务
* 相关文档
 * 函数类型
 * 参数:                                         
 * 参数名称          类型            输入/输出        描述
 * 
 * 输出参数:无         
 * 返回:无
***********************************************************************/


VOID cpss_spy_init()
{
    spyClkStartCommon(sysClkRateGet(), NULL);
}


/*******************************************************************************
* 函数名: cpss_spy
 * 功能: 兼容spy，输出各个任务的占用率
* 相关文档
 * 函数类型
 * 参数:                                         
 * 参数名称          类型            输入/输出        描述
 * ulInterval                 UINT32               IN                           输出间隔
 * 输出参数:无         
 * 返回:CPSS_OK,CPSS_ERROR
***********************************************************************/

INT32  cpss_spy(UINT32 ulInterval)
{
   if(g_bCpssOpenPrtSpy == TRUE)
   	{
   	  cps__oams_shcmd_printf("%s\n\r", "cpss spy has been executed");
	  return CPSS_ERROR;
   	}
     g_bCpssOpenPrtSpy = TRUE;

     if((ulInterval >=1)&&( ulInterval <=30*60))
     	{
     	   g_ulCpssSpyInterval = ulInterval;
     	}
	 else
	 {
	    g_ulCpssSpyInterval = 1;
	 }
     return CPSS_OK;	 
}

/*******************************************************************************
* 函数名: cpss_spy_stop
 * 功能: 兼容spyStop,停止各个任务占用率的输出
* 相关文档
 * 函数类型
 * 参数:                                         
 * 参数名称          类型            输入/输出        描述
 * 
 * 输出参数:无         
 * 返回:无
***********************************************************************/

VOID cpss_spy_stop()
{
    g_bCpssOpenPrtSpy = FALSE;
    g_ulCpssShowInterval = 0;

}

/*******************************************************************************
* 函数名:  cpss_spyTaskIdListSort
 * 功能: 按优先级排列任务
* 相关文档
 * 函数类型
 * 参数:                                         
 * 参数名称          类型            输入/输出        描述
 * 
 * 输出参数:无         
 * 返回:无
***********************************************************************/

void cpss_spyTaskIdListSort
    (
    int idList[],
    int nTasks
    )
    {
    FAST int temp;
    int prevPri;
    int curPri;
    FAST int *pCurId;
    BOOL change = TRUE;
    FAST int *pEndId = &idList [nTasks];

    if (nTasks == 0)
        return;

    while (change)
        {
        change = FALSE;

        taskPriorityGet (idList[0], &prevPri);

        for (pCurId = &idList[1]; pCurId < pEndId; ++pCurId, prevPri = curPri)
            {
            taskPriorityGet (*pCurId, &curPri);

            if (prevPri > curPri)
                {
                temp = *pCurId;
                *pCurId = *(pCurId - 1);
                *(pCurId - 1) = temp;
                change = TRUE;
                }
            }
        }
    }



/*******************************************************************************
* 函数名: cpss_spy_show
 * 功能: 采用spy算法算出各个占用率
* 相关文档
 * 函数类型
 * 参数:                                         
 * 参数名称          类型            输入/输出        描述
 * 
 * 输出参数:无         
 * 返回:无
***********************************************************************/
int g_unCpssCpuIdle;

VOID cpss_spy_show(BOOL bExcOccur)
    {
     FAST WIND_TCB *pTcb;
    FAST int 	   ix;
    SYMBOL_ID      symId;
    FUNCPTR 	   symbolAddress = 0;
    char 	   *name;       /* pointer to symbol table copy of string */
    int 	   taskPriority;
    char	   demangled [MAX_SYS_SYM_LEN + 1];
    char	   *nameToPrint;

    int 	   idList [CPSS_MAX_SPY_TASKS];	/* task specific statistics */
    int 	   taskIncTicks [CPSS_MAX_SPY_TASKS];
    int 	   taskTotalTicks [CPSS_MAX_SPY_TASKS];
    FAST int 	   nTasks;

    int 	   tmpIncTicks;			/* incremental snap shot */
    int 	   tmpIdleIncTicks;
    int 	   tmpKernelIncTicks;
    int 	   tmpInterruptIncTicks;

    int 	   totalPerCent;
    int 	   incPerCent;
    INT8    acStatusString [10];
    
    
    int 	   sumTotalPerCent = 0;
    int 	   sumIncPerCent   = 0;

    BOOL   bShow = FALSE;
    /* if there have been no ticks, there is nothing to report */

    if (spyIncTicks == 0)
	return;

    /* snap shot and clear task statistics */

    nTasks = taskIdListGet (idList, NELEMENTS (idList));

  /*  cpss_spyTaskIdListSort (idList, nTasks); */

    for (ix = 0; ix < nTasks; ++ix)
	{
	pTcb = taskTcb (idList [ix]);

	/*
	 * Need to make sure pTcb is a valid pointer
	 */

	if (pTcb == NULL)
	    continue;


	/* order is important: save and clear incremental, then update total */

	taskIncTicks [ix]    = pTcb->taskIncTicks;
	pTcb->taskIncTicks  = 0;

	pTcb->taskTicks    += taskIncTicks [ix];
	taskTotalTicks [ix]  = pTcb->taskTicks;
	}


    /* save and clear incremental counts and accumulate totals */

    tmpIncTicks          = spyIncTicks;
    tmpIdleIncTicks      = spyIdleIncTicks;
    tmpKernelIncTicks    = spyKernelIncTicks;
    tmpInterruptIncTicks = spyInterruptIncTicks;

    spyIncTicks = spyIdleIncTicks = spyKernelIncTicks = spyInterruptIncTicks =0;

    spyTotalTicks       += tmpIncTicks;
    spyInterruptTicks   += tmpInterruptIncTicks;
    spyKernelTicks      += tmpKernelIncTicks;
    spyIdleTicks        += tmpIdleIncTicks;

#if 0
    if (printRtn == NULL)	/* for host browser don't display result */
	return;
#endif

    /* print info */


if(g_bCpssOpenPrtSpy==TRUE)
{
   if(g_ulCpssShowInterval++ >= g_ulCpssSpyInterval)
   	{
   	    bShow = TRUE;
           g_ulCpssShowInterval = 0;
   	}
      else
      	{
      	  bShow = FALSE;
      	}
   
}


if(bShow == TRUE)
{
    cps__oams_shcmd_printf ("\n");
    cps__oams_shcmd_printf(
    "NAME          ENTRY         TID   PRI   total %% (ticks)  delta %% (ticks)   status \n");
   cps__oams_shcmd_printf(
    "--------     --------      -----  ---   ---------------  -----------   --------\n");
}
if(bExcOccur == TRUE)
{
  
   cpss_sbbr_sprintf(g_pcSbbrExcInfo,"\n");
   cpss_sbbr_sprintf(g_pcSbbrExcInfo,
    "NAME          ENTRY         TID   PRI   total %% (ticks)  delta %% (ticks)    status   \n");
   cpss_sbbr_sprintf(g_pcSbbrExcInfo,
    "--------     --------      -----  ---   ---------------  -----------   ----------\n");	
}

    for (ix = 0; ix < nTasks; ++ix)
	{
	/* find name in symbol table */

	pTcb = taskTcb (idList [ix]);

	/*
	 * Need to make sure pTcb is a valid pointer
	 */

	if (pTcb == NULL)
	    continue;

        /* 
	 * Only check one symLib function pointer (for performance's sake). All
	 * symLib functions are provided by the same library, by convention.    
	 */

	if ((_func_symFindSymbol !=(FUNCPTR) NULL) &&
	    (sysSymTbl != NULL))
	    {
	    if ((* _func_symFindSymbol) (sysSymTbl,  NULL, 
					 (void *)pTcb->entry, 
					 SYM_MASK_NONE, SYM_MASK_NONE, 
					 &symId) == OK)
	        {
		(* _func_symNameGet) (symId, &name);
		(* _func_symValueGet) (symId, (void **) &symbolAddress); 
		}
	    }

	if (symbolAddress != pTcb->entry)
	    name = "\0";	         /* no matching symbol */
	    
        taskPriorityGet (idList [ix], &taskPriority);

	/* print line for this task */

	totalPerCent     = (taskTotalTicks [ix] * 100) / spyTotalTicks;
	incPerCent       = (taskIncTicks [ix] * 100) / tmpIncTicks;
	sumTotalPerCent += totalPerCent;
	sumIncPerCent   += incPerCent;

	nameToPrint = cplusDemangle (name, demangled, sizeof (demangled));
	
       taskStatusString (idList [ix], acStatusString);
		 
	if(bShow == TRUE)
	cps__oams_shcmd_printf (cpss_spyFmt1, pTcb->name, nameToPrint, idList [ix], 
		      taskPriority, totalPerCent, taskTotalTicks [ix],
		      incPerCent, taskIncTicks [ix],acStatusString);

	if(bExcOccur == TRUE)
		{
		  
		   cpss_sbbr_sprintf(g_pcSbbrExcInfo,cpss_spyFmt1, pTcb->name, nameToPrint, idList [ix], 
		      taskPriority, totalPerCent, taskTotalTicks [ix],
		      incPerCent, taskIncTicks [ix],acStatusString);
		}
	}

    totalPerCent     = (spyKernelTicks * 100) / spyTotalTicks;
    incPerCent       = (tmpKernelIncTicks * 100) / tmpIncTicks;
    sumTotalPerCent += totalPerCent;
    sumIncPerCent   += incPerCent;

    if(bShow == TRUE)
    cps__oams_shcmd_printf(cpss_spyFmt2, "KERNEL", "", "", "", totalPerCent, spyKernelTicks,
				      incPerCent, tmpKernelIncTicks);

    if(bExcOccur == TRUE)
		{
		     cpss_sbbr_sprintf(g_pcSbbrExcInfo, cpss_spyFmt2, "KERNEL", "", "", "", totalPerCent, spyKernelTicks,
				      incPerCent, tmpKernelIncTicks);
		}	


    totalPerCent     = (spyInterruptTicks * 100) / spyTotalTicks;
    incPerCent       = (tmpInterruptIncTicks * 100) / tmpIncTicks;
    sumTotalPerCent += totalPerCent;
    sumIncPerCent   += incPerCent;

    if(bShow == TRUE)
    cps__oams_shcmd_printf (cpss_spyFmt2, "INTERRUPT", "", "", "", totalPerCent, spyInterruptTicks,
				      incPerCent, tmpInterruptIncTicks);

    if(bExcOccur == TRUE)
    	{
    	     cpss_sbbr_sprintf(g_pcSbbrExcInfo,cpss_spyFmt2, "INTERRUPT", "", "", "", totalPerCent, spyInterruptTicks,
				      incPerCent, tmpInterruptIncTicks);

    	}
    totalPerCent     = (spyIdleTicks * 100) / spyTotalTicks;
    incPerCent       = (tmpIdleIncTicks * 100) / tmpIncTicks;
    sumTotalPerCent += totalPerCent;
    sumIncPerCent   += incPerCent;

    
    cpss_output(CPSS_MODULE_KW,CPSS_PRINT_INFO,"IDLE = %d\%\tBUSY = %d\%\tTOTAL = %d\%\n", totalPerCent, sumTotalPerCent - totalPerCent, sumTotalPerCent);

     g_stCpssKwCpuLoad.Idle   = incPerCent;
     g_stCpssKwCpuLoad.Usage = sumIncPerCent - incPerCent;
    g_stCpssKwCpuLoad.Total  = sumIncPerCent;

   if(bShow == TRUE)
   {
    cps__oams_shcmd_printf(cpss_spyFmt2, "IDLE", "", "", "", totalPerCent, spyIdleTicks,
				 incPerCent, tmpIdleIncTicks);
    cps__oams_shcmd_printf(cpss_spyFmt2, "TOTAL", "", "", "", sumTotalPerCent, spyTotalTicks,
				  sumIncPerCent, tmpIncTicks);
    cps__oams_shcmd_printf ("\n");
   }

     if(bExcOccur == TRUE)
     	{
          cpss_sbbr_sprintf(g_pcSbbrExcInfo,cpss_spyFmt2, "IDLE", "", "", "", totalPerCent, spyIdleTicks,
				 incPerCent, tmpIdleIncTicks);
           cpss_sbbr_sprintf(g_pcSbbrExcInfo,cpss_spyFmt2, "TOTAL", "", "", "", sumTotalPerCent, spyTotalTicks,
				  sumIncPerCent, tmpIncTicks);
             cpss_sbbr_sprintf(g_pcSbbrExcInfo,"\n");
		
     	}
#if 0
    if (spyCreateCount > 0)
	{
	cps__oams_shcmd_printf ("%d task%s created.\n", spyCreateCount,
		spyCreateCount == 1 ? " was" : "s were");
	spyCreateCount = 0;
	}

    if (spyDeleteCount > 0)
	{
	cps__oams_shcmd_printf("%d task%s deleted.\n", spyDeleteCount,
		spyDeleteCount == 1 ? " was" : "s were");
	spyDeleteCount = 0;
	}
#endif	
    }



/*******************************************************************************
* 函数名: VOID cpss_cpu_load_task(VOID)
 * 功能: 计算占用率的任务
* 相关文档
 * 函数类型
 * 参数:                                         
 * 参数名称          类型            输入/输出        描述
 * 
 * 输出参数:无         
 * 返回:无
***********************************************************************/
UINT32 g_cpss_load_interval = 10*60;


VOID cpss_cpu_load_task(VOID)
{
   UINT32 ulDelayInterval = 0;
   cpss_spy_init();
    while(1)
    {
        taskDelay(sysClkRateGet());
        cpss_spy_show(FALSE);
	 if(ulDelayInterval++ >= g_cpss_load_interval)
	 {
	   ulDelayInterval = 0;
	   spyClkStopCommon();
          spyClkStartCommon(sysClkRateGet(), NULL);

	 }	
    }


}

UINT32 g_cpss_lj = 0xfff;
void cpss_cpu_test()
{
UINT32 a = g_cpss_lj;
  while(1)
  {
   
    a--;
    if(a == 0)
    {
      taskDelay(20);
      a = g_cpss_lj;
    } 
  }  
}

void cpss_load_test()
{
   taskSpawn("tTestCpuLoad",190,VX_NO_STACK_FILL,1024*1024,(FUNCPTR )cpss_cpu_test,0,0,0,0,0,0,0,0,0,0);

}


VOID cpss_kw_save_all_task_load()
{
  UINT32 ulArgs[4];
     cpss_mem_memset(g_pcSbbrExcInfo, 0, CPSS_SBBR_EXC_BUF_LENGTH);
    cpss_spy_show(TRUE);
   cpss_sbbr_write_text(0,ulArgs, "%s\n\n", g_pcSbbrExcInfo);
     cpss_mem_memset(g_pcSbbrExcInfo, 0, CPSS_SBBR_EXC_BUF_LENGTH);
}

/*******************************************************************************
* 函数名: cpss_kw_dsp_cpuusage_get
 * 功能: 在主CPU上查询dsp的CPU占用率函数
* 相关文档
 * 函数类型
 * 参数:                                         
 * 参数名称          类型              输入/输出        描述
 *                   ulUsageListMaxLen  输入            查询CPU占用率的个数
 *                   pstUsageList       输出            查询CPU占用率的详细信息
 *                   *pulUsageListLen   输出            查询CPU占用率的个数
 * 输出参数:无         
 * 返回:无
***********************************************************************/
#ifdef  CPSS_HOST_CPU_WITH_DSP
extern UINT32 smss_sysctl_get_dsp_status(UINT32 ucDspId);
UINT32 g_ulDspCpuUsge[CPSS_DSP_CPU_NUM] ;
INT32 cpss_kw_dsp_cpuusage_get(UINT32 ulUsageListMaxLen,CPSS_KW_DSP_USAGE_T  *pstUsageList,UINT32 *pulUsageListLen)
{
    UINT32 ulLen = 0 ;
    UINT32 ulCpuLoad = 0;
    UINT32 ulIndex;
    UINT32 ulListIndex = 0;
    UINT32 ulstatus = 0;
    
    if(ulUsageListMaxLen > CPSS_DSP_CPU_NUM)
    {
        return CPSS_ERROR;
    }
    if(pulUsageListLen == NULL)
    {
        return CPSS_ERROR;
    }
    
    *pulUsageListLen = 0;
    
    ulLen = CPSS_DSP_CPU_LOAD_ADDR - DRV_PCI_DSP_BASE_AADR;
    for(ulIndex = 0; ulIndex <ulUsageListMaxLen; ulIndex++)
    {
       ulstatus = smss_sysctl_get_dsp_status(ulIndex);
       if(ulstatus != 0)
       {
           continue;
       }
        
       ulCpuLoad = CPSS_DIV(((g_ulDspCpuUsge[ulIndex])*100),127);
       pstUsageList[ulListIndex].ulDspUsage = ulCpuLoad;
       pstUsageList[ulListIndex].ulDspNo = ulIndex+5;
       ulListIndex = ulListIndex + 1;
       *pulUsageListLen = ulListIndex;
    }
    return CPSS_OK;
}
void cpss_kw_dsp_cpu_usage_update(UINT32 ulDspNum, UINT32 ulDspCpuUsage)
{
    if(ulDspNum > CPSS_DSP_CPU_NUM)
    {
        return ;
    }
    g_ulDspCpuUsge[ulDspNum] = ulDspCpuUsage ;

    return ;
}
INT32 cpss_dsp_sbbr_counter_read(UINT32 ulDspId,CPSS_DSP_SBBR_COUNTER_T *pstDspSbbrCounter)
{
    UINT8 *pucBuf = NULL;
    UINT32 ulLen  = 0;
    CPSS_DSP_SBBR_COUNTER_T *pstTempDspSbbrCounter;
    
    pucBuf = cpss_mem_malloc(sizeof(CPSS_DSP_SBBR_COUNTER_T));
    if(pucBuf == NULL)
    {
        return (CPSS_ERROR);
    }
    
    cpss_mem_memset(pucBuf, 0 ,sizeof(CPSS_DSP_SBBR_COUNTER_T));
    ulLen = CPSS_DSP_SBBR_COUNTER - DRV_PCI_DSP_BASE_AADR;
    drv_dsp_ext_mem_read(ulDspId,ulLen,sizeof(CPSS_DSP_SBBR_COUNTER_T),pucBuf); 
    pstTempDspSbbrCounter = (CPSS_DSP_SBBR_COUNTER_T*)pucBuf;
    
    cpss_mem_memcpy(pstDspSbbrCounter,pstTempDspSbbrCounter,sizeof(CPSS_DSP_SBBR_COUNTER_T));

    cpss_mem_free(pucBuf);
    
    return 0 ;
}
#endif
#else
INT32 cpss_dsp_sbbr_counter_read(UINT32 ulDspId,CPSS_DSP_SBBR_COUNTER_T *pstDspSbbrCounter)
{
	return 0;
}

INT32 cpss_cpu_usage_get(UINT32 *pulUsage)
{
    *pulUsage = 10;
    return CPSS_OK;
}

INT32 cpss_reset
( 
    UINT32 ulCause
)
{
   return CPSS_OK;
}

INT32  cpss_spy(UINT32 ulInterval)
{
   return CPSS_OK;
}

VOID cpss_spy_stop()
{
   ;
}
#endif /* end #ifdef CPSS_VOS_VXWORKS */






