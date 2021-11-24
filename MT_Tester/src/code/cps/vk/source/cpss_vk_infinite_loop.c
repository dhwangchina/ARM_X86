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
#include "swp_type.h"
#include "cpss_common.h"
#include "cpss_config.h"
#include "cpss_vos_task.h"
#include "swp_common.h"
#include "cpss_dbg_sbbr.h"
#include "cpss_dbg_print.h"
#include "cpss_vk_sched.h"
#include "cpss_com_irq.h"
#include "cpss_vos.h"
#include "cpss_err.h"

/******************************** 宏定义 及全局量  ********************************/
#define CPSS_VK_INFINITE_LOOP_TIMER        CPSS_TIMER_00      /* 重发定时器号 */
#define KW_INFO_LEN      200
#define KW_MAX_MSG       20
#define KW_MAX_MSG_LEN   300
#define KW_DEAD_INFO_LEN 180

#define VOS_MSG_Q_FIFO  0x00 /* 任务按FIFO算法调度  */
#define VOS_MSG_Q_PRI   0x01 /* 任务按优先级调度 */
#define VOS_MSG_PRI_NORMAL  0 /* 优先级为普通的消息*/
#define VOS_MSG_PRI_URGENT  1  /* 优先级为紧急的消息 */

#define CPSS_INFINITE_LOOP_DETECT_PERIODS_S 1		/* 检测死循环函数的执行周期 单位：秒 */
#define CPSS_INFINITE_LOOP_DETECT_PERIODS_MS 10		/* 检测死循环函数的执行周期 单位：毫秒 */

//#define FALSE   (0)
//#define TRUE    (1)

UINT32		g_ulCpssKwUserId = -1;
UINT32		g_ulCpssKwMsgQueue;				/* 消息队列消息号 */
INT32		g_bCpssExcCount = 0;

extern CPSS_VOS_TASK_DESC_T    g_astCpssVosTaskDescTbl[VOS_MAX_TASK_NUM];
extern CPSS_VK_SCHED_DESC_T    g_astCpssVkSchedDescTbl[VK_MAX_SCHED_NUM];

extern UINT32 g_aulKwMsgId[VK_MAX_SCHED_NUM];

/****************************** 结构体 **********************************/
typedef struct tagCPSS_KW_KWINFOHEAD
{
    UINT32 ulMsgId;
    UINT32 ulLength;
    UINT8  acBuf[KW_INFO_LEN];      /*根据msgId套用下面结构*/
}CPSS_KW_KWINFOHEAD_T;

typedef struct tagCPSS_KW_DEADLOOPINFO
{
    UINT32 ulTid;                   /*调度任务号*/
    UINT32 ulprocDescCurr;          /*正在调度的纤程描述符*/
    UINT32 ulMsgId;                 /* 正在处理的消息ID*/
    CPSS_COM_MSG_HEAD_T *pstAddr;                  /* 存放消息头指?CPSS_COM_MSG_HEAD_T*/
}CPSS_KW_DEADLOOPINFO_T;

/*异常信息*/
typedef struct tagCPSS_KW_EXCINFO
{
    INT32  lTid;                    /*发生异常的任务描述符*/
    INT32  lVecNum;                 /*中断向量*/
//    CPSS_ESF  *pEsf;                /* stack frame */
}CPSS_KW_EXCINFO_T;

typedef struct tagCPSS_KW_SCAN_INFO
{
    CHAR acName[32];
    BOOL bRunFlag;              /* 0 No Run ; 1 Run */
    INT32 lKwTmOutLimitType;    /* s */
    INT32 lCurTicks;
    BOOL bValidFlag;            /* 0 invalid ; 1 valid */
    BOOL bIsProc;               /* 0 task ; 1 proc */
    INT32 lPid;                 /* task id or proc id */
    INT32 lMaxTicks;
} CPSS_KW_SCAN_INFO_T;

extern STRING g_szKwProcName;
extern void cps_boot_reboot(CPS_REBOOT_T* stRbt);


/******************************** 函数实现 **************************************/
/*******************************************************************************
* 函数名称: cpss_kw_routine
* 功    能: 纤程问题查找处理
* 函数类型:
* 参    数:
* 函数返回:
* 说    明: 判断是否出现问题
*******************************************************************************/
UINT32  cpss_kw_routine()
{
#if 0
    INT32  lResult;
    VK_SCHED_ID schedId;
    CPSS_KW_KWINFOHEAD_T stKWinfo;
    CPSS_KW_DEADLOOPINFO_T  *pstDeadLoopInfo ;
    CPSS_COM_MSG_HEAD_T *    pstData;
    pstDeadLoopInfo = (CPSS_KW_DEADLOOPINFO_T  *)stKWinfo.acBuf;
    CPSS_VOS_TASK_DESC_T	stInfo;
#endif
    INT8 acBuf[256];

  {
    UINT32 lExcId;
    BOOL bIsProc;
    lExcId = cpss_kw_find_exception_occur(&bIsProc);		/* 调用函数 - 检查当前监视表中的各项是否异常 */
		if(lExcId < 0)
			{
				cpss_message("TASK and PROC not include infinite loop, Not found any error");
			}
		else
			{
//				cpss_message("\n@***Checked the table and get lExcId is %d***\n", lExcId);
			}

    if(    lExcId != VOS_TASK_DESC_INVALID)	/* 判断是任务还是纤程，后面进行相应处理 */
        {
            if(bIsProc == TRUE)/* 对纤程的处理  TRUE<->FALSE  VALUE:1*/
                {
				/*add for kw msg*/
					CPS_REBOOT_T stRebt;

					cpss_message("\nOne Proc Has been down, SchId: 0x%d, Proc: %s, MsgId: 0x%x\n",
							lExcId, g_szKwProcName, g_aulKwMsgId[lExcId]);
//					cpss_kw_show_scan_info();
					/* 发送消息  - 异常中断处理*/

                    stRebt.ucRebtType = CPS_CPU_REBOOT_HOT;
                    stRebt.ucRebtRsn = CPS_REBOOT_BY_KW;

                    memset(acBuf, 0, sizeof(acBuf));
                    snprintf(acBuf, sizeof(acBuf), "%s, 0x%x", g_szKwProcName, g_aulKwMsgId[lExcId]);
                    stRebt.szExtra = acBuf;

                    cps_boot_reboot(&stRebt);
#if 0
					INT32    lResult, lTaskId, lVecNum;

				    CPSS_KW_KWINFOHEAD_T stKWinfo;
				    CPSS_KW_EXCINFO_T  *pstExcInfo;
				    pstExcInfo = (CPSS_KW_EXCINFO_T *)stKWinfo.acBuf;
				    stKWinfo.ulMsgId = CPSS_KW_EXCEPTION_NOTIFY_MSG;
				    stKWinfo.ulLength = sizeof(CPSS_KW_DEADLOOPINFO_T);
					
				    /* test for record the error Proc */
				    //cpss_message("PROC: %d\n", pstExcInfo->lTid);

					
				    lTaskId = lExcId;
				    lVecNum = 0xfffffff;
				    pstExcInfo->lTid = (UINT32)lTaskId;
				    pstExcInfo->lVecNum = lVecNum;
				    //pstExcInfo->pEsf   = NULL;
				    lResult = cpss_vos_msg_q_send(g_ulCpssKwMsgQueue, (CHAR *)&stKWinfo, sizeof(CPSS_KW_KWINFOHEAD_T), NO_WAIT, VOS_MSG_PRI_NORMAL);
				    if(lResult == CPSS_OK)
				    {
//				    	cpss_message("@####################   message of infinite loop has been sent . . .   ###########################");
				    }
				    else
				    {
				        cpss_message("\n@cpss_kw_routine msgQsend Fail \n");
				    }
				    /* 发送消息 end */
#endif
                }
				else
				{/* 对任务的处理 bIsProc = TRUE */
#if 0
             schedId = g_astCpssVkSchedDescTbl[lExcId].tSchedId;   /* 获得当前控制结构*/
             if(NULL != schedId->pIpcMsgCurr)
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
//                        	cpss_message("@cpss_vos_msg_q_send success !!\n");
                        }
                        else
                        {
                            cpss_message("\n@cpss_kw_routine msgQsend Fail \n");
                        }
                }/* end if(NULL != */
#endif

        }
        }
  }
  return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_kw_routine_delay
* 功    能: 检测函数循环处理 创建周期任务
* 函数类型:
* 参    数:
* 函数返回:
* 说    明:
*******************************************************************************/
UINT32 cpss_kw_routine_delay()
{
	INT8 lResult;
	while(1)
	{
		 lResult = cpss_kw_routine();
		 if(lResult != CPSS_OK)
		 {
			 cpss_message("@cpss_kw_routine failed !!! can not find error\n");
		 }
		 else
		 {
//			 cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_VK, CPSS_PRINT_WARN, "\n@The Detector of INFINITE LOOP is running...\n");
//			 cpss_message("\n@The Detector of INFINITE LOOP is running...\n");
//			 cpss_kw_show_scan_info();
		 }
		cpss_vos_task_delay(CPSS_INFINITE_LOOP_DETECT_PERIODS_MS);
	}
	return CPSS_OK;
}

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
	UINT32 ulTaskDesc;
	INT8 lResult;
#if 0
	 g_ulCpssKwMsgQueue =  cpss_vos_msg_q_create("MsgQueue for KW", KW_MAX_MSG, KW_MAX_MSG_LEN,VOS_MSG_Q_FIFO); /*创建消息队列1*/

	    if(VOS_MSG_Q_DESC_INVALID == g_ulCpssKwMsgQueue)
	    {
	        cpss_output(CPSS_MODULE_KW,CPSS_PRINT_FATAL ,"@cpss create msgQueue Fail !!! \n\r");
	        return CPSS_ERR_KW_SYS_CALL_FAIL;
	    }
	    else
	    {
//	    	cpss_message("@msg_q_create DONE . . .\n");
	    }
#endif
	 /* 创建周期任务 - 检测纤程问题 */

	 ulTaskDesc = cpss_vos_task_spawn("CpssKwRoutineTask", 0, 0, 0, cpss_kw_routine_delay, 1);

//	 	 lResult = cpss_kw_routine_delay();
//	 if (CPSS_OK != lResult)
	   if (ulTaskDesc == 0xFFFFFFFE)
	       {
		   cpss_message("@cpss vk infinite loop receive wait FAILED !!!\n");
	           return (CPSS_ERROR);
	       }
	   else
	   {
//		   cpss_message("@cpss kw routine delay set DONE ...\n");
	   }

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_kw_task
* 功    能: 内核监控任务
* 函数类型:
* 参    数: 无
* 函数返回:
* 说    明: 收到检测到问题的消息后，进行什么样的处理
*******************************************************************************/
void cpss_kw_task(void)
{
    CPSS_KW_KWINFOHEAD_T *pstKWinfo;
    CHAR aucBuf[300];
    INT32 lResult;
    CPS_REBOOT_T stRebt;

    if(cpss_kw_init() != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_KW,CPSS_PRINT_FATAL ,"@cpss kw module init Fail \n\r");
        return;
    }
    else
    {
//    	cpss_message("@cpss vk init DONE ...\n");
    }
    g_ulCpssKwUserId = cpss_sbbr_get_userid();			/* 获得当前调度任务的user id  ???  */

    pstKWinfo = (CPSS_KW_KWINFOHEAD_T *)aucBuf;
    memset(pstKWinfo, 0, sizeof(CPSS_KW_KWINFOHEAD_T));
    while(1)
    {
        lResult = cpss_vos_msg_q_receive(g_ulCpssKwMsgQueue ,aucBuf ,sizeof(CPSS_KW_KWINFOHEAD_T),WAIT_FOREVER);
//        cpss_message("\n@msg_q_receive WILL WAIT_FOREVER DONE . . .\n");
        if(CPSS_ERROR != lResult)
        {
            switch(pstKWinfo->ulMsgId)
            {
            case CPSS_KW_DEAD_LOOP_NOTIFY_MSG:			/* 任务异常 通知 */
                {
                	cpss_message("\n@##########    NOW found one TASK error !!!    ###########\n");
                }
                break;
            case CPSS_KW_EXCEPTION_NOTIFY_MSG:			/*纤程异常通知*/
                   {
                       cpss_message("\n@##########    NOW found one PROC error !!!    ###########\n");

                       stRebt.ucRebtType = CPS_CPU_REBOOT_HOT;
                       stRebt.ucRebtRsn = CPS_REBOOT_BY_KW;
                       stRebt.szExtra = g_szKwProcName;

                       cps_boot_reboot(&stRebt);

                       /* 显示当前时间 */
                       CPSS_TIME_T stTime;
                       cpss_clock_get(&stTime);

                    	cpss_message(" \nTime update:\t %d/%d/%d  %d:%d:%d\n",
                   	            stTime.usYear,
                   	            stTime.ucMonth,
                   	            stTime.ucDay,
                   	            stTime.ucHour,
                   	            stTime.ucMinute,
                   	            stTime.ucSecond);
						/* 显示当前时间 end  */
                   }
                   break;
            default:
                cpss_output(CPSS_MODULE_KW,CPSS_PRINT_INFO,"Moniter Task Receive Unknown MsgId = 0x%x",pstKWinfo->ulMsgId);
                break;
            }/* end switch */
        }
	  else
	  {
	         cpss_message("@call cpss_vos_msg_q_receive return %d msg queue id %d \n\r");

	  }
    }/* end while(1) */
}
/*******************************************************************************
* 函数名称: cpss_vk_infinite_loop_detect_start
* 功    能: 死循环检测主函数
* 函数类型:
* 参    数: 无
* 函数返回: cpss_ok ,cpss_error
* 说    明:
*******************************************************************************/
UINT32 cpss_vk_infinite_loop_detect_start()
{
	UINT32 ulTaskDesc;
	/* 开启错误消息等待任务 */
//	ulTaskDesc = cpss_vos_task_spawn("CpssKwTask", 0, 0, 0, cpss_kw_task, 1);

	ulTaskDesc = cpss_vos_task_spawn("CpssKwRoutineTask", 0, 0, 0, cpss_kw_routine_delay, 1);

	if(ulTaskDesc == 0xFFFFFFFE)
	{
			cpss_message("@cpss task create failed . . .infinite loop wait message\n");
			return (CPSS_ERROR);
	}
	cpss_message("@DETECTOR START ...\n");
	return CPSS_OK;
}



/*******************************************************************************
* 函数名称: cpss_kw_find_exception_occur
* 功    能: 检查当前监视表中的各项是否异常
* 函数类型:
* 参    数:              类型                      IN/OUT          描述
*  pbIsProc                  bool                             out             0 true:任务
*  																			  1 false:纤程
* ?
* 函数返回: taskid or procdesc
* 说    明:
*******************************************************************************/
#if 0
extern BOOL g_bCpssRecvProcEndInd;
UINT32 cpss_kw_find_exception_occur(BOOL *pbIsProc)

{
	UINT16 usLoop = 0;
#if 0
    if (FALSE == g_bCpssRecvProcEndInd)
    {
        g_bCpssRecvProcEndInd = smss_get_proc_active_state();
    }

    if (g_bCpssCloseException == TRUE)
    {
        return VOS_TASK_DESC_INVALID;
    }

#endif
    for ( usLoop = 1; usLoop < CPSS_KW_MAX_SCAN_ITEM; usLoop++)
    {
 /* VALUE:1 VALUE:1 */       if ((g_pstCpssKwScanInfo[usLoop].bValidFlag == TRUE) && (g_pstCpssKwScanInfo[usLoop].bRunFlag == CPSS_KW_RUN_FLAG_RUN))		/*1<->TRUE*/
        {
            /* if proc is being init return */
 /* VALUE:1 VALUE:1 */           if ((g_pstCpssKwScanInfo[usLoop].bIsProc == TRUE) && (g_bCpssRecvProcEndInd == TRUE))//FALSE 改初值
            {
                return VOS_TASK_DESC_INVALID;
            }
            if (g_pstCpssKwScanInfo[usLoop].lCurTicks-- <= 0)
            {
            	cpss_message("#############   FOUND lCurTicks   ##################\n");
                *pbIsProc = g_pstCpssKwScanInfo[usLoop].bIsProc;
                g_pstCpssKwScanInfo[usLoop].lCurTicks = swp_get_ha_time(g_pstCpssKwScanInfo[usLoop].lKwTmOutLimitType) / 10;

                g_bCpssSbbrCurExcOccur = TRUE;

                return g_pstCpssKwScanInfo[usLoop].lPid;
            }
        }
    }
    return VOS_TASK_DESC_INVALID;

}


/* 放在注册纤程后cpss_common.c */
cpss_message ("done.\n");
/***************** 检测纤程死循环 ********************/
cpss_message ("\n Infinite LOOP detect Init...\n");
lRet = cpss_vk_infinite_loop_detect_start();
if (CPSS_OK != lRet)
{
    cpss_message ("failed.\n");

    return (CPSS_ERROR);
}
cpss_message ("done.\n");
/***************** 检测纤程死循环 end ********************/
/* sbbr init */

#endif
