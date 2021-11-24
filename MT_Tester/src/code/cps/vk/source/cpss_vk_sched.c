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
#include "cpss_common.h"
#include "cpss_util_q_fifo_lib.h"
#include "cpss_vos_msg_q.h"
#include "cpss_vos_task.h"
#include "cpss_vk_proc.h"
#include "cpss_vk_sched.h"
#include "cpss_com_common.h"

/******************************* 全局变量定义/初始化 **************************/
extern CPSS_VK_PROC_STAT_T   g_astCpssVkProcStatTbl [VK_MAX_PROC_INST_NUM];
extern CPSS_VK_PROC_DESC_T   g_astCpssVkProcDescTbl [VK_MAX_PROC_INST_NUM];
extern CPSS_VK_PROC_CLASS_T  g_astCpssVkProcClassTbl [VK_MAX_PROC_CLASS_NUM];

/* 调度器描述表 */

CPSS_VK_SCHED_DESC_T    g_astCpssVkSchedDescTbl [VK_MAX_SCHED_NUM];

#ifdef CPSS_DSP_CPU    
volatile int     g_lAppCondVar = 0;
BOOL    g_bSched = FALSE;
#endif

extern CPSS_IPC_MSG_SEND_T g_stCpssIpcStat;

/*proc执行时间统计*/
CPSS_PROC_EXECUT_TIME_T* g_apstProcTimeStat[VK_MAX_PROC_CLASS_NUM+1] ;
CPSS_PROC_EXECUT_TIME_T g_stCpssVkProcTimeStat[VK_MAX_PROC_CLASS_NUM+1] ;

extern VOID cpss_kw_read_sched_task_info(UINT32 ulTaskDesc,CHAR *pName);

extern UINT32 g_azCpssSwitchInTime[32][32][2];

extern UINT32 g_azCpssSwitchInCount[32];

extern BOOL g_bCpssTaskSwitch;

extern CPSS_VOS_TASK_DESC_T    g_astCpssVosTaskDescTbl[VOS_MAX_TASK_NUM];

UINT32 g_ulCpssSchedTaskDescBase = VOS_TASK_DESC_INVALID;

extern INT32 cpss_timer_get_sec_and_ms(UINT32 *pulSec, UINT32 *pulMs);

extern UINT32 g_aulKwMsgId[VK_MAX_SCHED_NUM];

/******************************* 局部常数和类型定义 ***************************/

/* 初始化*/

 BOOL g_bCpssVkSchedLibInited = FALSE;

/* 调度器统计表 */

 CPSS_VK_SCHED_STAT_T    g_astCpssVkSchedStatTbl [VK_MAX_SCHED_NUM];

/* forward declarations */

extern VOID cpss_timer_tmcb_release(CPSS_COM_MSG_HEAD_T *pMsgHead);

extern UINT32 cpss_sbbr_get_userid(VOID);
 UINT32   cpss_vk_sched_desc_alloc (void);
 INT32    cpss_vk_sched_desc_free (UINT32 ulSchedDesc);
 INT32    cpss_vk_sched_init (CPSS_VK_SCHED_SCB_T *ptScb, INT8 *pcName,
                                    INT32 lPri, INT32 lOptions, INT32 lStackSize
                                    , VOID_FUNC_PTR pfEntry, INT32 lArg);
 void     cpss_vk_sched_proc_run (VK_PROC_ID tProcId, IPC_MSG_HDR_T *ptHdr
                                        );

extern  void cpss_dbg_pro_time_pause();
void cpss_dbg_pro_time_add_start_ex(VK_PROC_ID tProcID, CPSS_COM_MSG_HEAD_T *ptMsgHead);
void cpss_dbg_pro_time_add_end_ex();

UINT32 gulCpssDbgProIsStatPause = 1;

#define cpss_dbg_pro_time_is_enable() (0 == gulCpssDbgProIsStatPause)
#define cpss_dbg_pro_time_add_start(x,y) if(cpss_dbg_pro_time_is_enable()) cpss_dbg_pro_time_add_start_ex((x), (y));
#define cpss_dbg_pro_time_add_end() if(cpss_dbg_pro_time_is_enable()) cpss_dbg_pro_time_add_end_ex();

extern VOID cpss_kw_set_proc_run(UINT16  usGuid);
extern VOID   cpss_kw_set_proc_stop(UINT16  usGuid);

extern INT32 cpss_sbbr_write_decode_data
(
UINT32 ulUserId, 
UINT32 ulArgs[4], 
UINT8 *pucData,
UINT32 ulLen
);
/*******************************************************************************
*函数名称:cpss_vk_sched_lib_init
*功能:调度器库初始化 
*函数类型: 
*参数: 
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
INT32 cpss_vk_sched_lib_init (void)
{
    UINT32  ulSchedDesc;
    
    if (g_bCpssVkSchedLibInited == TRUE)
    {
        return (CPSS_ERROR);
    }
    
    /* 初始化调度器描述符表. */
    
    memset ((void *) (& g_astCpssVkSchedDescTbl[0]), 0, 
        (VK_MAX_SCHED_NUM * sizeof (CPSS_VK_SCHED_DESC_T)));
    
    /* 设置bInuse. */
    
    for (ulSchedDesc = 0; ulSchedDesc < VK_MAX_SCHED_NUM; ulSchedDesc++)
    {
        g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse = FALSE;
    }
    
    /* 初始化调度器统计表. */
    
    memset ((void *) (& g_astCpssVkSchedStatTbl[0]), 0, 
        (VK_MAX_SCHED_NUM * sizeof (CPSS_VK_SCHED_STAT_T)));
    
    /* 初始化完成. */
    
    g_bCpssVkSchedLibInited = TRUE;
    
    /*清零统计内存*/
    cpss_mem_memset(g_stCpssVkProcTimeStat,0,
        sizeof(CPSS_PROC_EXECUT_TIME_T)*(VK_MAX_PROC_CLASS_NUM+1)) ;
    
    cpss_mem_memset(g_apstProcTimeStat,0,sizeof(UINT32)*(VK_MAX_PROC_CLASS_NUM+1)) ;
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_vk_sched_desc_alloc
*功能:调度器描述符分配 
*函数类型: 
*参数: 
数名称 类型 输入/输出 描述
*函数返回:调度器描述符 or VK_SCHED_DESC_INVALID
*说明:                                 
*******************************************************************************/
 UINT32 cpss_vk_sched_desc_alloc (void)
{
    UINT32  ulSchedDesc;
    
    /* 寻找调度器描述符入口, 第0个保留. */
    
    for (ulSchedDesc = 1; ulSchedDesc < VK_MAX_SCHED_NUM; ulSchedDesc++)
    {
        if (g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse == FALSE)
        {
            break;
        }
    }
    
    /* 没找到. */
    
    if (ulSchedDesc == VK_MAX_SCHED_NUM)
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_desc_alloc: insufficient decriptor available.\n"
            );
        
        return (VK_SCHED_DESC_INVALID);
    }
    
    /* 设置bInuse. */
    
    g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse = TRUE;
    
    return (ulSchedDesc);
}

/*******************************************************************************
*函数名称:cpss_vk_sched_desc_free
*功能:调度器描述释放符 
*函数类型: 
*参数: 
*参数名称    类型   输入/输出 描述
*ulSchedDesc UINT32 输入
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
 INT32 cpss_vk_sched_desc_free 
(
 UINT32  ulSchedDesc
 )
{
    /* 检查调度器描述符. */
    
    if ((ulSchedDesc <= 0) || (ulSchedDesc >= VK_MAX_SCHED_NUM))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_desc_free: descriptor invalid, descriptor = %d.\n"
            , ulSchedDesc);
        
        return (CPSS_ERROR);
    }
    
    /* 清除调度器描述符入口. */
    
    memset ((void *) (& g_astCpssVkSchedDescTbl[ulSchedDesc]), 0,  
        sizeof (CPSS_VK_SCHED_DESC_T));
    
    /* 设置bInuse. */
    
    g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse = FALSE;
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_vk_sched_proc_load
*功能:将纤程装入调度器 
*函数类型: 
*参数: 
*参数名称    类型       输入/输出 描述
*ulSchedDesc UINT32     输入
*tProcId     VK_PROC_ID 输入
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
INT32 cpss_vk_sched_proc_load 
(
 UINT32  ulSchedDesc, /* 调度器描述符 */
 VK_PROC_ID  tProcId /* 纤程标示 */
 )
{
    VK_SCHED_ID tSchedId;
    
    /* 获得调度器标示. */
    
    tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
    
    if (tProcId->lStatus & VK_PROC_STATUS_PEND)
    {
        /* 从pend队列移出. */
        
        cpss_util_q_fifo_remove (& tSchedId->qPend, & tProcId->node); 
        
        /* 设置状态(PEND->READY, PEND+S->SUSPEND. */
        
        tProcId->lStatus &= ~VK_PROC_STATUS_PEND;
   
        /* 放入READY队列. */

        cpss_util_q_fifo_put (& tSchedId->qReady, & tProcId->node, 
                              UTIL_Q_FIFO_KEY_TAIL);
        /* 更新统计. */
          
        g_astCpssVkSchedStatTbl[ulSchedDesc].lQPendNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQPendNum - 1;
    }
    
    /* 如果纤程状态是READY. */
    
    if ((tProcId->lStatus == VK_PROC_STATUS_READY) && (tProcId->lMailNum != 0))
    {
        /* 更新READY计数. */
        
        tSchedId->lReadyNum = tSchedId->lReadyNum + 1;
        
        /* 更新统计. */
        
        g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum + 1;
    }
    
    return (CPSS_OK);
}

/*******************************************************************************
*函数名称:cpss_vk_sched_proc_run
*功能:运行纤程
*函数类型: 
*参数: 
*参数名称 类型            输入/输出 描述
*tProcId  VK_PROC_ID      输入
*ptHdr    IPC_MSG_HDR_T * 输入
*函数返回:
*说明:                                                         
*******************************************************************************/

 UINT32 g_ulCpssProcTIme = 3*1000*1000;


 BOOL g_bCpssWithSbbr = TRUE;

VOID cpss_enable_sbbr(VOID)
{
  g_bCpssWithSbbr = TRUE;
}

VOID cpss_disable_sbbr(VOID)
{
  g_bCpssWithSbbr = FALSE;
}

UINT32 g_ulProcRunCOunter = 0;
#define CPSS_DSP_SBBR_RECORD_NUM 256
typedef struct
{
    UINT32 SeqNum;
    UINT32 MsgId;
    UINT32 SrcProcId;
    UINT32 SrcLogAddr;
    UINT32 DstProcAddr;
    UINT32 CurTime;
    UINT32 CurTick;
}CPSS_DSP_SBBR_T;
 CPSS_DSP_SBBR_T *g_pstCpssDspSbbrRecd = (CPSS_DSP_SBBR_T *)CPSS_DSP_SBBR_CPSS_VK_ADDR;
 UINT32 g_ulCpssDspSbbrRecd = 0;
 UINT32 g_ulCpssDspSeqNum = 0;
 void cpss_vk_dsp_sbbr_record(UINT32 MsgId,UINT32 SrcProcId,UINT32 SrcLogAddr,UINT32 DstProcId)
 {
    UINT32 ulSec = 0;
    UINT32 ulMs = 0;
    if(g_ulCpssDspSbbrRecd >= CPSS_DSP_SBBR_RECORD_NUM)
    {
        g_ulCpssDspSbbrRecd = 0;
    }
    cpss_timer_get_sec_and_ms(&ulSec,&ulMs);
    g_pstCpssDspSbbrRecd[g_ulCpssDspSbbrRecd].SeqNum = cpss_htonl(g_ulCpssDspSeqNum);
    g_pstCpssDspSbbrRecd[g_ulCpssDspSbbrRecd].MsgId = cpss_htonl(MsgId);
    g_pstCpssDspSbbrRecd[g_ulCpssDspSbbrRecd].SrcProcId = cpss_htonl(SrcProcId);
    g_pstCpssDspSbbrRecd[g_ulCpssDspSbbrRecd].SrcLogAddr = cpss_htonl(SrcLogAddr);
    g_pstCpssDspSbbrRecd[g_ulCpssDspSbbrRecd].DstProcAddr = cpss_htonl(DstProcId);
    g_pstCpssDspSbbrRecd[g_ulCpssDspSbbrRecd].CurTime = cpss_htonl(ulSec);
    g_pstCpssDspSbbrRecd[g_ulCpssDspSbbrRecd].CurTick = cpss_htonl(ulMs);
    g_ulCpssDspSbbrRecd++;
    g_ulCpssDspSeqNum++;
 }
 void cpss_vk_sched_proc_run 
(
 VK_PROC_ID  tProcId, /* 纤程标示 */
 IPC_MSG_HDR_T *   ptHdr /* 指向头部 */
 )
{
    UINT32  ulProcDesc;
    UINT16  usGuid;
    UINT32  ulIndex;
    #ifndef CPSS_DSP_CPU
      UINT32 ulArgs[4];
      UINT32 ulUserId;
    #endif
    #ifdef CPSS_VOS_VXWORKS
    UINT16 usTaskSwitchInCountb;
    UINT16 usTaskSwitchInCounte;
    UINT32 ulTaskDesc;
    #endif
    CPSS_COM_MSG_HEAD_T *pstMsg = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr);
    CPSS_VK_IPC_MSG_NODE_T *pstNodeHdr ;
    
    /*改变IPC头中记录的任务,纤程号*/
#ifdef CPSS_DSP_CPU
    g_ulProcRunCOunter = g_ulProcRunCOunter + 1;
    *(UINT32*)(CPSS_DSP_SBBR_COUNTER + 16) = cpss_htonl(g_ulProcRunCOunter);
#endif

#ifdef CPSS_IPC_MSG_COUNTER
    pstNodeHdr = (CPSS_VK_IPC_MSG_NODE_T*)IPC_MSG_HDR_TO_NODE(ptHdr) ;
    cpss_vk_proc_info_add(pstNodeHdr) ;
#endif
    
        /* 获得纤程描述符. */
    
    ulProcDesc = tProcId->ulHash;
    
    /* 获得GUID. */
    
    usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
    
#ifndef CPSS_DSP_CPU    
    cpss_com_byteflow_print(CPSS_MODULE_VK, pstMsg->pucBuf, pstMsg->ulLen);
#endif
    /* 增加时间戳HOOK */
    CPSS_DSP_FILE_REC();
    CPSS_DSP_LINE_REC();
    CPSS_DSP_MSGID_REC(pstMsg->ulMsgId);
    CPSS_DSP_PRCID_REC(usGuid);
#ifdef CPSS_DSP_CPU
    //cpss_vk_dsp_sbbr_record(pstMsg->ulMsgId,pstMsg->stSrcProc.ulPd,*(UINT32*)&pstMsg->stSrcProc.stLogicAddr,pstMsg->stDstProc.ulPd);
#endif
    cpss_com_timestamp_add_hook_sch(ptHdr);
    
    CPSS_DSP_LINE_REC();
/*    
#ifndef CPSS_DSP_CPU    
    cpss_switch_hook(usGuid, pstMsg->ulMsgId);
#endif
*/
#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
    {
        BOOL bTraceIt;
        UINT8 *pucBuf = IPC_MSG_HDR_TO_DATA (ptHdr);
        pucBuf = pucBuf - sizeof(IPC_MSG_HDR_T) + 8;
        
        bTraceIt = cpss_trace_route_it_recv(pucBuf, NULL);
        if (TRUE == bTraceIt)
        {
            cpss_trace_route_info_set(CPSS_TRACE_NODE_LOC_DST, CPSS_TRACE_LAYER_SCHI, ulProcDesc, 0,0,0, pucBuf);
            cpss_trace_route_event_send(pucBuf);
        }
    }
#endif

#ifndef CPSS_DSP_CPU

#if 0
  if(g_bCpssWithSbbr == TRUE) 
  {
    ulUserId = cpss_sbbr_get_userid();

   /* cpss_sbbr_write_text(ulUserId, ulArgs,
	" GMT %d Proc %s start sched MsgId = 0x%x  length =  %d \
	source Module %d SubGroup %d Group %d pno 0x%x \
	  Dest    Moduel %d SubGroup %d Group %d pno 0x%x \
	  Data ...... \n\r",
        cpss_gmt_get(),g_astCpssVkProcClassTbl[usGuid].acName,pstMsg->ulMsgId,pstMsg->ulLen,
        pstMsg->stSrcProc.stLogicAddr.ucModule,pstMsg->stSrcProc.stLogicAddr.ucSubGroup,pstMsg->stSrcProc.stLogicAddr.usGroup,pstMsg->stSrcProc.ulPd,
        pstMsg->stDstProc.stLogicAddr.ucModule,pstMsg->stDstProc.stLogicAddr.ucSubGroup,pstMsg->stDstProc.stLogicAddr.usGroup,pstMsg->stDstProc.ulPd
        );*/


    ulArgs[0] = pstMsg->ulMsgId;
    ulArgs[1] = pstMsg->stSrcProc.ulPd;
    ulArgs[2] = pstMsg->stDstProc.ulPd;
    ulArgs[3] = cpss_gmt_get();
    cpss_sbbr_write_decode_data(ulUserId,ulArgs,pstMsg->pucBuf,pstMsg->ulLen);
}
#endif

#endif
 
    CPSS_DSP_LINE_REC();
    pstMsg->ucShareFlag = 0 ;
#ifndef CPSS_DSP_CPU
    cpss_kw_set_proc_run(usGuid);
    cpss_dbg_pro_time_add_start(tProcId, (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr));
#endif

#if 0
#ifdef CPSS_VOS_VXWORKS
   {
    if(g_bCpssTaskSwitch == TRUE)

     {
      UINT16 usSwicthTimes;
      ulTaskDesc = cpss_vos_task_desc_self();
      usSwicthTimes = g_azCpssSwitchInCount[ulTaskDesc];
      usTaskSwitchInCountb = usSwicthTimes;
      tProcId->ulProcNTimeb = cpss_microsecond_counter_get();
     }
   }		
#endif
#endif

   /*增加统计*/
   if(pstMsg->stDstProc.ulPd != CPSS_COM_SLID_PROC)
   {
       cpss_com_spec_msg_stat(pstMsg->ulMsgId,DEST_PROC_RECV_PHASE) ;
   }


    (tProcId->pfEntry) (tProcId->usState, tProcId->pcVar, 
        (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr));

#if 0
#ifdef CPSS_VOS_VXWORKS
   {
      if(g_bCpssTaskSwitch == TRUE)
      	{
     usTaskSwitchInCounte = g_azCpssSwitchInCount[ulTaskDesc];

    if (usTaskSwitchInCounte != usTaskSwitchInCountb)

    {
         UINT32 i ,Tt;
        tProcId->ulProcNTimee = cpss_microsecond_counter_get();
   
      
        for (i = usTaskSwitchInCountb+1; i < usTaskSwitchInCounte; i++)

        {
              Tt += (g_azCpssSwitchInTime[ulTaskDesc][i][1] - g_azCpssSwitchInTime[ulTaskDesc][i][0]);
            
        }
        tProcId->ulProcNTime += 
   	        g_azCpssSwitchInTime[ulTaskDesc][usTaskSwitchInCountb][1] -  tProcId->ulProcNTimeb  + Tt\
   	        +tProcId->ulProcNTimee - g_azCpssSwitchInTime[ulTaskDesc][usTaskSwitchInCounte][0];
   	    if( tProcId->ulProcNTime >= g_ulCpssProcTIme)
   	    {
   	       tProcId->ulProcNTime = 0; 
   	    }

    }

   else
    {
        tProcId->ulProcNTimee = cpss_microsecond_counter_get();
       tProcId->ulProcNTime += tProcId->ulProcNTimee - tProcId->ulProcNTimeb;
        if( tProcId->ulProcNTime >= g_ulCpssProcTIme)
   	    {
   	       tProcId->ulProcNTime = 0; 
   	    }

    }

      	}

    }
#endif
#endif


#ifndef CPSS_DSP_CPU
    cpss_dbg_pro_time_add_end();
    cpss_kw_set_proc_stop(usGuid);
#endif
    
    CPSS_DSP_LINE_REC();
    
#ifndef CPSS_DSP_CPU

   /* cpss_sbbr_write_text(ulUserId, ulArgs, " GMT %d Proc %s End sched MsgId = 0x%x\n\r",cpss_gmt_get(),g_astCpssVkProcClassTbl[usGuid].acName,pstMsg->ulMsgId);*/
/*   
    cpss_switch_hook(usGuid, pstMsg->ulMsgId);
*/    
#endif
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + (UINT16)(ulProcDesc & 0x0000ffff);
    
    /* 更新统计. */
    
    g_astCpssVkProcStatTbl[ulIndex].ulInteCallNum = g_astCpssVkProcStatTbl[ulIndex].ulInteCallNum + 1;
}

/*******************************************************************************
*函数名称:cpss_vk_sched
*功能:调度任务
*函数类型: 
*参数: 
*参数名称 类型  输入/输出 描述
*lArg     INT32 输入
*函数返回:
*说明:                                 
*******************************************************************************/

#ifdef CPSS_DSP_CPU
UINT32 g_ulMacCallCounter = 0;
UINT32 g_ulChipPollCounter = 0;
UINT32 g_ulPciRecvCounter = 0 ;
UINT32 g_ulTimerScanCounter = 0 ;
void cpss_vk_sched
(
 INT32   lArg
 )
{
    UINT32  ulSchedDesc;
    VK_SCHED_ID tSchedId;
    IPC_MSG_HDR_T *   ptHdr;
#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS) || defined (CPSS_VOS_LINUX)   
    INT32   lBytesRecv;
#endif    
    IPC_MSG_HDR_T *   ptNext;
    CPSS_VK_IPC_MSG_NODE_T *  ptNode;
    CPSS_COM_MSG_HEAD_T *	ptData;
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    UINT32  ulProcDesc;
    VK_PROC_ID  tProcId;
    INT32 lRet = 0;
    
    /* 获得调度器描述符. */
    
    ulSchedDesc = (UINT32) lArg;
    
#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS) || defined (CPSS_VOS_LINUX)    
    while (1)
    {
        /* 调度器初始化完成? */
        
        if (g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId != NULL)
        {
            break;
        }
        
        /* 等待. */
        
        cpss_vos_task_delay (10);
    }
#endif
    
    /* 获得调度器标示. */
    
    tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
    
    while (1) 
    {

#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS) || defined (CPSS_VOS_LINUX)    
        ptHdr = NULL;
#if 0
        if(g_bCpssTaskSwitch == TRUE)
        {
	    UINT16 usSwicthTimes;		
	    UINT32 ulTaskDesc,ulTemp;
	  
          ulTaskDesc = cpss_vos_task_desc_self();
	    usSwicthTimes  = g_azCpssSwitchInCount[ulTaskDesc];
	    if(usSwicthTimes < 32)
          {
            ulTemp = g_azCpssSwitchInTime[ulTaskDesc][usSwicthTimes][0];
	    }
	    g_azCpssSwitchInCount[ulTaskDesc]	= 0;
	    memset(g_azCpssSwitchInTime , 0, 	32 *32 *2);
	     g_azCpssSwitchInTime[ulTaskDesc][0][0] = ulTemp;	
		
        }		
#endif        
        /* 消息队列接收. */
        
        lBytesRecv = cpss_vos_msg_q_receive (tSchedId->ulMsgQDesc, 
            (INT8 *) (& ptHdr),
            VK_SCHED_DEF_MAX_MSG_LEN,
            WAIT_FOREVER);
        
        /* 错误. */
        
        if (lBytesRecv == CPSS_ERROR)
        {
            continue;
        }
#endif        

#ifdef CPSS_DSP_CPU
		/* 调度使能 */
     g_bSched = TRUE; 
     cpss_dsp6x_timer1_stop(CLK_SRC); /* 为CPU占用率增加，假设空闲，Timer1停止工作*/
     
     if (g_lAppCondVar != g_lIsrCondVar)
            {
              cpss_dsp6x_timer1_start(CLK_SRC); /* 为CPU占用率增加，实际运行，Timer1开始工作*/

		   /* 如果出现过1ms中断，则
			  调用mac_call    (g_lIsrCondVar-g_lAppCondVar) 次 
		      调用cpss_timer_scan_task 1 次 
	              调用cpss_com_drv_pci_recv_task 直到1ms中断发生或交互区为空
		      调用chip_poll 直到1ms中断发生或交互区为空 */

            /* make callee to create timer. */
            
            tSchedId->ulProcDescCurr = TPSS_MESSAGE_ANALYSIS_PROC;
            tSchedId->pIpcMsgCurr = NULL;

             #if 0
             do 
             {
                g_ulMacCallCounter = g_ulMacCallCounter+1;
                *(UINT32*)(CPSS_DSP_SBBR_COUNTER) = cpss_htonl(g_ulMacCallCounter);
                mac_call ();
                g_lAppCondVar = g_lAppCondVar + 1;
            }while (g_lAppCondVar != g_lIsrCondVar);
            #else
            g_ulMacCallCounter = g_ulMacCallCounter+1;
            *(UINT32*)(CPSS_DSP_SBBR_COUNTER) = cpss_htonl(g_ulMacCallCounter);
            mac_call ();  
            g_lAppCondVar = g_lIsrCondVar;
            #endif

			/* 调度禁止 */
            g_bSched = FALSE;

            g_ulTimerScanCounter = g_ulTimerScanCounter + 1;
            *(UINT32*)(CPSS_DSP_SBBR_COUNTER + 4) = cpss_htonl(g_ulTimerScanCounter);
            cpss_timer_scan_task ();                         
            if (g_lAppCondVar != g_lIsrCondVar)
            {
                continue ;/* 1ms中断发生，中断后续处理 */
            }
            
            while(1)
            {
                g_ulPciRecvCounter = g_ulPciRecvCounter + 1;
                *(UINT32*)(CPSS_DSP_SBBR_COUNTER + 8 )= cpss_htonl(g_ulPciRecvCounter);
                lRet = cpss_com_drv_pci_recv_task(); 
                if ((CPSS_OK != lRet) || (g_lAppCondVar != g_lIsrCondVar))
            	{
                	break;  /* 判断pci是否还有数据，如果没有或1ms中断已经发生，则退出  */
            	}    
            }
                
            if (g_lAppCondVar != g_lIsrCondVar)
            {
                continue ;/* 1ms中断发生，中断后续处理 */
            }    
                
            /* 调度使能 */
            g_bSched = TRUE;                
            
            while(1)
            {
                g_ulChipPollCounter = g_ulChipPollCounter + 1;
                *(UINT32*)(CPSS_DSP_SBBR_COUNTER + 12) = cpss_htonl(g_ulChipPollCounter);
                lRet = chip_poll ();
    	        if ((0 == lRet) || (g_lAppCondVar != g_lIsrCondVar))
                {
                    break;  /* 判断chip_poll上是否还有数据，如果没有或1ms中断已经发生，则退出 */
            	}  
            }             
            
            if (g_lAppCondVar != g_lIsrCondVar)
            {
                continue ;/* 1ms中断发生，中断后续处理 */
            }  

            }
        
        if (tSchedId->lMsgNum == 0)
            {
            continue;
            }
            
        ptNode = NULL;
        
        /* Get message from message queue. */
                
        ptNode = (CPSS_VK_IPC_MSG_NODE_T *) cpss_util_q_fifo_get (& tSchedId->msgQ);

        /* No message. */
        
        if (ptNode == NULL)
            {
            tSchedId->lMsgNum = 0;
            
            continue;
            }
        
        /* Update message number. */
        
        tSchedId->lMsgNum = tSchedId->lMsgNum - 1;
        
        /* Pointer to header. */
        
        ptHdr = (IPC_MSG_HDR_T *) IPC_MSG_NODE_TO_HDR (ptNode);
#endif
        while (1)
        {
            /* 现在无消息. */
            
            if (ptHdr == NULL)
            {
                break;
            }
            
            /* 保存下一个消息. */
            
            ptNext = ptHdr->ptNext;
            
            /* 从消息链移出. */
            
            ptHdr->ptNext = NULL;
            
            /* IPC消息接收. */
            
            cpss_ipc_msg_receive (ptHdr);
            
            /* 获得下一个消息. */
            
            ptHdr = ptNext;
        }    

#ifdef CPSS_DSP_CPU        
            if (g_lAppCondVar != g_lIsrCondVar)
            {
            continue ;/* 1ms中断发生，中断后续处理*/
            }          


        /* 开始调度. */
        cpss_dsp6x_timer1_stop(CLK_SRC); /* 为CPU占用率增加，假设空闲，Timer1停止工作*/
#endif
        while ((tSchedId->lReadyNum > 0) || (tSchedId->lMailNum > 0))
        {
#ifdef CPSS_DSP_CPU        
            cpss_dsp6x_timer1_start(CLK_SRC); /* 为CPU占用率增加，实际运行，Timer1开始工作*/
        
          
            if (g_lAppCondVar != g_lIsrCondVar)
            {
                break ;/* 1ms中断发生，中断后续处理，退出循环 while ((tSchedId->lReadyNum > 0) || (tSchedId->lMailNum > 0))*/
            }
#endif
            while (tSchedId->lMailNum > 0)
            {
#ifdef CPSS_DSP_CPU
                if (g_lAppCondVar != g_lIsrCondVar)
                {
                   break; /* 1ms中断发生，中断后续处理，退出循环 while (tSchedId->lMailNum > 0) */
                }
#endif
                /* 当前纤程描述符无效. */
                
                tSchedId->ulProcDescCurr = CPSS_VK_PD_INVALID;
                
                /* 当前IPC消息空. */
                
                tSchedId->pIpcMsgCurr = NULL;
                
                /* 从调度器邮箱获得消息. */
                
                ptNode = (CPSS_VK_IPC_MSG_NODE_T *) cpss_util_q_fifo_get (
                    & tSchedId->mailBox);
                
                /* 没有消息. */
                
                if (ptNode == NULL)
                {
                    /* 更新调度器邮箱计数. */
                    
                    tSchedId->lMailNum = 0;
                    
                    break;
                }
                
                /* 更新调度器邮箱计数. */
                
                tSchedId->lMailNum = tSchedId->lMailNum -1;
                
                /* 指向头部. */
                
                ptHdr = (IPC_MSG_HDR_T *) IPC_MSG_NODE_TO_HDR (ptNode);
                
                /* 设置当前IPC消息. */
                
                tSchedId->pIpcMsgCurr = (void *) ptHdr;
                
                /* 指向数据. */
                
                ptData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr);
                
                /* 获得纤程描述符. */
                
                ulProcDesc = ptData->stDstProc.ulPd;
                
                /* 下面的检查将会避免纤程删除当消息在调度器邮箱的时候. */
                
                /* 获得GUID. */
                
                usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
                
                /* 检查GUID. */
                
                if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
                    (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
                {
                    /* 释放消息. */
                    
                    cpss_ipc_msg_free ((void *) ptHdr);
                    
                    continue;
                }
                
                /* 获得实例. */
                
                usInst = (UINT16) (ulProcDesc & 0x0000ffff);
                
                /* 检查实例. */
                
                if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
                {
                    /* 释放消息. */
                    
                    cpss_ipc_msg_free ((void *) ptHdr);
                    
                    continue;
                }
                
                /* 获得索引. */
                
                ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
                
                /* 检查纤程描述符. */
                
                if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
                {
                    /* 释放消息. */
                    
                    cpss_ipc_msg_free ((void *) ptHdr);
                    
                    continue;
                }
                
                /* 获得纤程标示. */
                
                tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;
                
                /* 没有纤程. */
                
                if (tProcId == NULL)
                {
                    /* 释放消息. */
                    
                    cpss_ipc_msg_free ((void *) ptHdr);
                    
                    continue;
                }
                
                /* 设置当前纤程描述符. */
                
                tSchedId->ulProcDescCurr = tProcId->ulHash;
                
                /* 设置纤程最大运行时间. */
#ifdef CPSS_VOS_VXWORKS
                tSchedId->lTick = 10*sysClkRateGet(); 
#endif
                /* 运行纤程. */
                
                cpss_vk_sched_proc_run (tProcId, ptHdr);
                
#ifdef CPSS_VOS_VXWORKS
                if (tSchedId->lTick <= 0)
                    {
                    tProcId->lMaxTicks =  10 * sysClkRateGet();   
                    }
                    
                else
                    {
                    if (((10 * sysClkRateGet()) - tSchedId->lTick) > 
                        tProcId->lMaxTicks)
                        {
                        tProcId->lMaxTicks = (10 * sysClkRateGet()) - 
                                             tSchedId->lTick;    
                        } 
                    }
#endif
                /* 释放消息. */
                
                cpss_ipc_msg_free ((void *) ptHdr);
                
                /* 当前IPC消息空. */
                
                tSchedId->pIpcMsgCurr = NULL;
                
                /* 当前纤程描述符无效. */
                
                tSchedId->ulProcDescCurr = CPSS_VK_PD_INVALID;
                
                /* 更新统计. */
                
                g_astCpssVkSchedStatTbl[ulSchedDesc].ulCallNum = g_astCpssVkSchedStatTbl[ulSchedDesc].ulCallNum + 1;
                }
                
#ifdef CPSS_DSP_CPU
            if (g_lAppCondVar != g_lIsrCondVar)
            {
                break ;  /* 1ms中断发生，中断后续处理，退出循环 while ((tSchedId->lReadyNum > 0) || (tSchedId->lMailNum > 0))*/
            }
#endif
                /* 当前纤程描述符无效. */
                
                tSchedId->ulProcDescCurr = CPSS_VK_PD_INVALID;
                
                /* Current IPC message null. */
                
                tSchedId->pIpcMsgCurr = NULL;
                
                /* 获得纤程标示. */
                
                tProcId = (VK_PROC_ID) cpss_util_q_fifo_get (& tSchedId->qReady);            
                
                /* 无纤程. */
                
                if (tProcId == NULL)
                {
                    /* 更新READY计数. */
                    
                    tSchedId->lReadyNum = 0;
                    
                    /* 更新统计. */
                    
                    g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum = 0;
                    
                    continue;
                }
                
                /* 更新READY计数. */
                
                tSchedId->lReadyNum = tSchedId->lReadyNum - 1;
                
                /* 设置当前纤程描述符. */
                
                tSchedId->ulProcDescCurr = tProcId->ulHash;
                
                /* 更新统计. */
                
                g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum - 1;
                
                while (tProcId->lMailNum > 0)
                {
                    /* 从纤程邮箱获得消息. */
                    
                ptNode = (CPSS_VK_IPC_MSG_NODE_T *) cpss_util_q_fifo_get (& tProcId->mailBox);
                    
                    /* 无消息. */
                    
                    if (ptNode == NULL)
                    {
                        /* 更新纤程邮箱计数. */
                        
                        tProcId->lMailNum = 0;
                        
                        break;
                    }
                    
                    /* 更新纤程邮箱计数. */
                    
                    tProcId->lMailNum = tProcId->lMailNum - 1;
                    
                    if (ptNode->lFlags & IPC_MSG_FLAG_EXPIRE)
                    {
                        cpss_ipc_msg_free ((void *) IPC_MSG_NODE_TO_HDR (ptNode));
                        
                        break;
                    }
                    
                    /* 指向头部 */
                    
                    ptHdr = (IPC_MSG_HDR_T *) IPC_MSG_NODE_TO_HDR (ptNode);
                    
                    /* 设置设置当前IPC消息. */
                    
                    tSchedId->pIpcMsgCurr = (void *) ptHdr;
                    
					
			/* 释放一次性定时器.  */
                    
                    ptData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr);
                    
                    cpss_timer_tmcb_release (ptData);
                    
                    /* 设置纤程最大运行时间 */
#ifdef CPSS_VOS_VXWORKS
                    tSchedId->lTick = 10*sysClkRateGet(); 
#endif
                    /* 运行纤程. */
                    
                    cpss_vk_sched_proc_run (tProcId, ptHdr);
                    
#ifdef CPSS_VOS_VXWORKS
                    if (tSchedId->lTick <= 0)
                        {
                        tProcId->lMaxTicks =  10 * sysClkRateGet();   
                        }
                        
                    else
                        {
                    if (((10 * sysClkRateGet()) - tSchedId->lTick) > tProcId->lMaxTicks)
                            {
                        tProcId->lMaxTicks = (10 * sysClkRateGet()) - tSchedId->lTick;    
                            } 
                        }
#endif

                    
                    
                    /* 释放消息. */
                    
                    cpss_ipc_msg_free ((void *) ptHdr);
                    
                    /* 当前IPC消息空. */
                    
                    tSchedId->pIpcMsgCurr = NULL;
                    
                    /* 当前纤程描述符无效. */
                    
                    tSchedId->ulProcDescCurr = CPSS_VK_PD_INVALID;
                    
                    /* 更新统计. */
                    
                    g_astCpssVkSchedStatTbl[ulSchedDesc].ulCallNum = g_astCpssVkSchedStatTbl[ulSchedDesc].ulCallNum + 1;
                    
                   break; 
                }
                
                /* 纤程邮箱空? */
                
                if (tProcId->lMailNum == 0)
                {
                    /* 放入PEND队列. */
                    
                cpss_util_q_fifo_put (& tSchedId->qPend, & tProcId->node, UTIL_Q_FIFO_KEY_TAIL);
                    
                    /* 更新统计. */
                    
                    g_astCpssVkSchedStatTbl[ulSchedDesc].lQPendNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQPendNum + 1;
                    
                    /* 设置状态. */
                    
                    tProcId->lStatus |= VK_PROC_STATUS_PEND;
                }
                
                else
                {
                    /* 放入READY队列尾部. */
                    
                cpss_util_q_fifo_put (& tSchedId->qReady, & tProcId->node, UTIL_Q_FIFO_KEY_TAIL);
                    
                    /* 更新统计. */
                    
                    g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum + 1;
                }
            }
        }
    }
#else
void cpss_vk_sched
(
 INT32   lArg
 )
{
    UINT32  ulSchedDesc;
    VK_SCHED_ID tSchedId;
    IPC_MSG_HDR_T *   ptHdr;
    INT32   lBytesRecv;
    IPC_MSG_HDR_T *   ptNext;
    CPSS_VK_IPC_MSG_NODE_T *  ptNode;
    CPSS_COM_MSG_HEAD_T *	ptData;
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    UINT32  ulProcDesc;
    VK_PROC_ID  tProcId;
    INT32 lRet = 0;
    
    /* 获得调度器描述符. */
    
    ulSchedDesc = (UINT32) lArg;
    
    while (1)
    {
        /* 调度器初始化完成? */
        
        if (g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId != NULL)
        {
            break;
        }
        
        /* 等待. */
        
        cpss_vos_task_delay (10);
    }
    
    /* 获得调度器标示. */
    
    tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
    
    while (1) 
    {

        ptHdr = NULL;
 #if 0       
        if(g_bCpssTaskSwitch == TRUE)
        {
	    UINT16 usSwicthTimes;		
	    UINT32 ulTaskDesc,ulTemp;
	  
          ulTaskDesc = cpss_vos_task_desc_self();
	    usSwicthTimes  = g_azCpssSwitchInCount[ulTaskDesc];
	    if(usSwicthTimes < 32)
          {
            ulTemp = g_azCpssSwitchInTime[ulTaskDesc][usSwicthTimes][0];
	    }
	    g_azCpssSwitchInCount[ulTaskDesc]	= 0;
	    memset(g_azCpssSwitchInTime , 0, 	32 *32 *2);
	     g_azCpssSwitchInTime[ulTaskDesc][0][0] = ulTemp;	
		
        }		
#endif        
        /* 消息队列接收. */

        lBytesRecv = cpss_vos_msg_q_receive (tSchedId->ulMsgQDesc, 
            (INT8 *) (& ptHdr),
            VK_SCHED_DEF_MAX_MSG_LEN,
            WAIT_FOREVER);
        
        /* 错误. */
        
        if (lBytesRecv == CPSS_ERROR)
        {
            continue;
        }
        while (1)
        {
            /* 现在无消息. */
            
            if (ptHdr == NULL)
            {
                break;
            }
            
            /* 保存下一个消息. */
            
            ptNext = ptHdr->ptNext;
            
            /* 从消息链移出. */
            
            ptHdr->ptNext = NULL;
            
            /* IPC消息接收. */
            
            cpss_ipc_msg_receive (ptHdr);
            
            /* 获得下一个消息. */
            
            ptHdr = ptNext;
        }    

        while ((tSchedId->lReadyNum > 0) || (tSchedId->lMailNum > 0))
        {
            while (tSchedId->lMailNum > 0)
            {
                /* 当前纤程描述符无效. */
                
                tSchedId->ulProcDescCurr = CPSS_VK_PD_INVALID;
                
                /* 当前IPC消息空. */
                
                tSchedId->pIpcMsgCurr = NULL;
                
                /* 从调度器邮箱获得消息. */
                
                ptNode = (CPSS_VK_IPC_MSG_NODE_T *) cpss_util_q_fifo_get (
                    & tSchedId->mailBox);
                
                /* 没有消息. */
                
                if (ptNode == NULL)
                {
                    /* 更新调度器邮箱计数. */
                    
                    tSchedId->lMailNum = 0;
                    
                    break;
                }
                
                /* 更新调度器邮箱计数. */
                
                tSchedId->lMailNum = tSchedId->lMailNum -1;
                
                /* 指向头部. */
                
                ptHdr = (IPC_MSG_HDR_T *) IPC_MSG_NODE_TO_HDR (ptNode);
                
                /* 设置当前IPC消息. */
                
                tSchedId->pIpcMsgCurr = (void *) ptHdr;
                
                /* 指向数据. */
                
                ptData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr);
                
                /* 获得纤程描述符. */
                
                ulProcDesc = ptData->stDstProc.ulPd;
                
                /* 下面的检查将会避免纤程删除当消息在调度器邮箱的时候. */
                
                /* 获得GUID. */
                
                usGuid = (UINT16) ((ulProcDesc >> 16) & 0x0000ffff);
                
                /* 检查GUID. */
                
                if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
                    (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
                {
                    /* 释放消息. */
                    
                    cpss_ipc_msg_free ((void *) ptHdr);
                    
                    continue;
                }
                
                /* 获得实例. */
                
                usInst = (UINT16) (ulProcDesc & 0x0000ffff);
                
                /* 检查实例. */
                
                if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
                {
                    /* 释放消息. */
                    
                    cpss_ipc_msg_free ((void *) ptHdr);
                    
                    continue;
                }
                
                /* 获得索引. */
                
                ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;
                
                /* 检查纤程描述符. */
                
                if (g_astCpssVkProcDescTbl[ulIndex].bInuse == FALSE)
                {
                    /* 释放消息. */
                    
                    cpss_ipc_msg_free ((void *) ptHdr);
                    
                    continue;
                }
                
                /* 获得纤程标示. */
                
                tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;
                
                /* 没有纤程. */
                
                if (tProcId == NULL)
                {
                    /* 释放消息. */
                    
                    cpss_ipc_msg_free ((void *) ptHdr);
                    
                    continue;
                }
                
                /* 设置当前纤程描述符. */
                
                tSchedId->ulProcDescCurr = tProcId->ulHash;
                
                /* 运行纤程. */
                
                g_aulKwMsgId[ulSchedDesc] = ptData->ulMsgId;

                cpss_vk_sched_proc_run (tProcId, ptHdr);
                
                /* 释放消息. */
                
                cpss_ipc_msg_free ((void *) ptHdr);
                
                /* 当前IPC消息空. */
                
                tSchedId->pIpcMsgCurr = NULL;
                
                /* 当前纤程描述符无效. */
                
                tSchedId->ulProcDescCurr = CPSS_VK_PD_INVALID;
                
                /* 更新统计. */
                
                g_astCpssVkSchedStatTbl[ulSchedDesc].ulCallNum = g_astCpssVkSchedStatTbl[ulSchedDesc].ulCallNum + 1;
                }
                
                /* 当前纤程描述符无效. */
                
                tSchedId->ulProcDescCurr = CPSS_VK_PD_INVALID;
                
                /* Current IPC message null. */
                
                tSchedId->pIpcMsgCurr = NULL;
                
                /* 获得纤程标示. */
                
                tProcId = (VK_PROC_ID) cpss_util_q_fifo_get (& tSchedId->qReady);            
                
                /* 无纤程. */
                
                if (tProcId == NULL)
                {
                    /* 更新READY计数. */
                    
                    tSchedId->lReadyNum = 0;
                    
                    /* 更新统计. */
                    
                    g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum = 0;
                    
                    continue;
                }
                
                /* 更新READY计数. */
                
                tSchedId->lReadyNum = tSchedId->lReadyNum - 1;
                
                /* 设置当前纤程描述符. */
                
                tSchedId->ulProcDescCurr = tProcId->ulHash;
                
                /* 更新统计. */
                
                g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum - 1;
                
                while (tProcId->lMailNum > 0)
                {
                    /* 从纤程邮箱获得消息. */
                    
                ptNode = (CPSS_VK_IPC_MSG_NODE_T *) cpss_util_q_fifo_get (& tProcId->mailBox);
                    
                    /* 无消息. */
                    
                    if (ptNode == NULL)
                    {
                        /* 更新纤程邮箱计数. */
                        
                        tProcId->lMailNum = 0;
                        
                        break;
                    }
                    
                    /* 更新纤程邮箱计数. */
                    
                    tProcId->lMailNum = tProcId->lMailNum - 1;
                    
                    if (ptNode->lFlags & IPC_MSG_FLAG_EXPIRE)
                    {
                        cpss_ipc_msg_free ((void *) IPC_MSG_NODE_TO_HDR (ptNode));
                        
                        break;
                    }
                    
                    /* 指向头部 */
                    
                    ptHdr = (IPC_MSG_HDR_T *) IPC_MSG_NODE_TO_HDR (ptNode);
                    
                    /* 设置设置当前IPC消息. */
                    
                    tSchedId->pIpcMsgCurr = (void *) ptHdr;
                    
					
			/* 释放一次性定时器.  */
                    
                    ptData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr);
                    
                    cpss_timer_tmcb_release (ptData);
                    
                    /* 运行纤程. */
                    
                    g_aulKwMsgId[ulSchedDesc] = ptData->ulMsgId;

                    cpss_vk_sched_proc_run (tProcId, ptHdr);
                    
                    /* 释放消息. */
                    
                    cpss_ipc_msg_free ((void *) ptHdr);
                    
                    /* 当前IPC消息空. */
                    
                    tSchedId->pIpcMsgCurr = NULL;
                    
                    /* 当前纤程描述符无效. */
                    
                    tSchedId->ulProcDescCurr = CPSS_VK_PD_INVALID;
                    
                    /* 更新统计. */
                    
                    g_astCpssVkSchedStatTbl[ulSchedDesc].ulCallNum = g_astCpssVkSchedStatTbl[ulSchedDesc].ulCallNum + 1;
                    
                   break; 
                }
                
                /* 纤程邮箱空? */
                
                if (tProcId->lMailNum == 0)
                {
                    /* 放入PEND队列. */
                    
                cpss_util_q_fifo_put (& tSchedId->qPend, & tProcId->node, UTIL_Q_FIFO_KEY_TAIL);
                    
                    /* 更新统计. */
                    
                    g_astCpssVkSchedStatTbl[ulSchedDesc].lQPendNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQPendNum + 1;
                    
                    /* 设置状态. */
                    
                    tProcId->lStatus |= VK_PROC_STATUS_PEND;
                }
                
                else
                {
                    /* 放入READY队列尾部. */
                    
                cpss_util_q_fifo_put (& tSchedId->qReady, & tProcId->node, UTIL_Q_FIFO_KEY_TAIL);
                    
                    /* 更新统计. */
                    
                    g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum + 1;
                }
            }
        }
    }

#endif

    /*******************************************************************************
    *函数名称:cpss_vk_sched_init
    *功能:初始化调度器
    *函数类型: 
    *参数: 
    *参数名称   类型             输入/输出 描述
    *ptScb      CPSS_VK_SCHED_SCB_T * 输入
    *pcName     INT8 *           输入
    *lPri       INT32            输入
    *lOptions   INT32            输入
    *lStackSize INT32            输入
    *pfEntry    VOID_FUNC_PTR    输入
    *lArg       INT32            输入
    *函数返回:CPSS_OK or CPSS_ERROR
    *说明:                                 
    *******************************************************************************/
     INT32 cpss_vk_sched_init 
        (
        CPSS_VK_SCHED_SCB_T *  ptScb, /* pointer to SCB */
        INT8 *  pcName, /* pcName */
        INT32   lPri, /* priority */
        INT32   lOptions,  /* options */
        INT32   lStackSize, /* size (bytes) of stack needed */
        VOID_FUNC_PTR   pfEntry, /* entry point */
        INT32   lArg /* argument */
        )
    {
#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS) || defined (CPSS_VOS_LINUX)   
        UINT32  ulMsgQDesc;
#endif
        UINT32  ulTaskDesc;
        
        /* 初始化问题. */
        
        if ((!g_bCpssVkSchedLibInited) && (cpss_vk_sched_lib_init () == CPSS_ERROR))
        {
            return (CPSS_ERROR);
        }
        
        /* 清除调度器结构. */
        
        memset ((void *) ptScb, 0, sizeof (CPSS_VK_SCHED_SCB_T));
        
#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS) || defined (CPSS_VOS_LINUX)    
        /* 创建消息队列. */
        
        ulMsgQDesc = cpss_vos_msg_q_create (pcName, VK_SCHED_DEF_MAX_MSGS,
            VK_SCHED_DEF_MAX_MSG_LEN, VOS_MSG_Q_FIFO
            );
        if (ulMsgQDesc == VOS_MSG_Q_DESC_INVALID)
        {
            return (CPSS_ERROR);
        }
#endif   
        
#ifdef CPSS_DSP_CPU
    cpss_util_q_fifo_init (& ptScb->msgQ);
    ptScb->lMsgNum = 0;
#endif 
        /* 创建任务. */
        
        ulTaskDesc = cpss_vos_task_spawn (pcName, lPri, lOptions, lStackSize, 
            pfEntry, lArg);

        if (ulTaskDesc == VOS_TASK_DESC_INVALID)
        {
            return (CPSS_ERROR);
        }
       /* 在任务表中设置调度描述符*/
        g_astCpssVosTaskDescTbl[ulTaskDesc].ulSchedDesc = (UINT32)lArg;
        
	 if(g_ulCpssSchedTaskDescBase == VOS_TASK_DESC_INVALID)
	 {
	    g_ulCpssSchedTaskDescBase = ulTaskDesc;
	 }
        
#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS) || defined (CPSS_VOS_LINUX)    
        /* 保存消息队列描述符和任务描述符. */
        
        ptScb->ulMsgQDesc = ulMsgQDesc;
        ptScb->ulTaskDesc = ulTaskDesc;
#endif    
        /* 初始化内部队列. */
        
        cpss_util_q_fifo_init (& ptScb->qPend);
        cpss_util_q_fifo_init (& ptScb->qReady);
        
        /* 清除计数. */
        
        ptScb->lQReadyNum = 0;
        ptScb->lQPendNum = 0;
        ptScb->lReadyNum = 0;
        
        /* 初始化内部队列. */
        
        cpss_util_q_fifo_init (& ptScb->mailBox);
        
        /* 清除计数. */
        
        ptScb->lMailNum = 0;
        
        ptScb->ulProcDescCurr = CPSS_VK_PD_INVALID;
        ptScb->pIpcMsgCurr = NULL;
        ptScb->lError = 0;
        
	  cpss_kw_read_sched_task_info(ulTaskDesc,pcName);	
        return (CPSS_OK);
    }
    
    /*******************************************************************************
    *函数名称:cpss_vk_sched_spawn
    *功能:创建调度器
    *函数类型: 
    *参数: 
    *参数名称   类型          输入/输出 描述
    *pcName     INT8 *        输入
    *lPri       INT32         输入
    *lOptions   INT32         输入
    *lStackSize INT32         输入
    *pfEntry    VOID_FUNC_PTR 输入
    *函数返回:调度器描述符 or VK_SCHED_DESC_INVALID
    *说明:                                 
    *******************************************************************************/
    UINT32 cpss_vk_sched_spawn
        (
        INT8 *  pcName, /* name */
        INT32   lPri, /* priority */
        INT32   lOptions,  /* options */
        INT32   lStackSize, /* size (bytes) of stack needed */
        VOID_FUNC_PTR   pfEntry /* entry point */
        )
    {
        UINT32  ulSchedDesc;
        VK_SCHED_ID  tSchedId;
        
        /* 分配调度器描述符. */
        
        ulSchedDesc = cpss_vk_sched_desc_alloc ();
        if (ulSchedDesc == VK_SCHED_DESC_INVALID)
        {
            return (VK_SCHED_DESC_INVALID);
        }
        
        /* 分配SCB结构. */
        
        tSchedId = (VK_SCHED_ID) malloc (sizeof (CPSS_VK_SCHED_SCB_T));
        if (tSchedId == NULL)
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_spawn: malloc failed.\n"
                );
            
            return (VK_SCHED_DESC_INVALID);
        }
        
        /* 初始化SCB. */
        
        if (cpss_vk_sched_init (tSchedId, pcName, lPri, lOptions, lStackSize, pfEntry, 
            (INT32) ulSchedDesc) == CPSS_ERROR)
        {
            /* 释放SCB. */
            
            free ((void *) tSchedId);
            
            /* 释放调度器描述符. */
            
            cpss_vk_sched_desc_free (ulSchedDesc);
            
            return (VK_SCHED_DESC_INVALID);
        }
        
        /* 保存调度器标示. */
        
        g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId = tSchedId;
        
        return (ulSchedDesc);
    }
    
    /*******************************************************************************
    *函数名称:cpss_vk_sched_delete
    *功能:删除调度器
    *函数类型: 
    *参数: 
    *参数名称    类型   输入/输出 描述
    *ulSchedDesc UINT32 输入
    *函数返回:CPSS_OK or CPSS_ERROR
    *说明:                                 
    *******************************************************************************/
    INT32 cpss_vk_sched_delete 
        (
        UINT32  ulSchedDesc /* 调度器描述符 */
        )
    {
        VK_SCHED_ID tSchedId;
        
        /* 检查调度器描述符. */
        
        if ((ulSchedDesc <= 0) || (ulSchedDesc >= VK_MAX_SCHED_NUM) ||
            (g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse == FALSE))
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_delete: descriptor invalid, descriptor = %d.\n"
                , ulSchedDesc);
            
            return (CPSS_ERROR);
        }
        
        /* 获得调度器标示. */
        
        tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
        
        /* 悬挂任务. */
        
        if (cpss_vos_task_suspend (tSchedId->ulTaskDesc) == CPSS_ERROR)
        {
            return (CPSS_ERROR);
        }
        
        /* 删除任务. */
        
        if (cpss_vos_task_delete (tSchedId->ulTaskDesc) == CPSS_ERROR)
        {
            return (CPSS_ERROR);
        }
        
#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS) || defined (CPSS_VOS_LINUX)    
        /* 删除消息队列. */
        
        if (cpss_vos_msg_q_delete (tSchedId->ulMsgQDesc) == CPSS_ERROR)
        {
            return (CPSS_ERROR);
        }
#endif    
        /* 释放内存. */
        
        free ((void *) tSchedId);
        
        /* 释放调度器描述符. */
        
        cpss_vk_sched_desc_free (ulSchedDesc);
        
        return (CPSS_OK);
    }
    
    /*******************************************************************************
    *函数名称:cpss_vk_sched_proc_spawn
    *功能:将纤程加入调度器
    *函数类型: 
    *参数: 
    *参数名称    类型       输入/输出 描述
    *ulSchedDesc UINT32     输入
    *tProcId     VK_PROC_ID 输入
    *函数返回:CPSS_OK or CPSS_ERROR
    *说明:                                 
    *******************************************************************************/
    INT32 cpss_vk_sched_proc_spawn 
        (
        UINT32  ulSchedDesc, /* 调度器描述符 */
        VK_PROC_ID  tProcId /* 纤程标示 */
        ) 
    {
        VK_SCHED_ID tSchedId;
        
        /* 检查调度器描述符. */
        
        if ((ulSchedDesc <= 0) || (ulSchedDesc >= VK_MAX_SCHED_NUM) ||
            (g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse == FALSE))
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_proc_spawn: descriptor invalid, descriptor = %d.\n"
                , ulSchedDesc);
            
            return (CPSS_ERROR);
        }
        
        /* 获得调度器标示. */
        
        tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
        
        /* 放入PEND队列. */
        
        cpss_util_q_fifo_put (& tSchedId->qPend, & tProcId->node, 
            UTIL_Q_FIFO_KEY_TAIL);
        
        /* 设置状态. */
        
        tProcId->lStatus = VK_PROC_STATUS_PEND;
        
        /* 更新统计. */
        
        g_astCpssVkSchedStatTbl[ulSchedDesc].lQPendNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQPendNum + 1;
        g_astCpssVkSchedStatTbl[ulSchedDesc].lProcNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lProcNum + 1;
        
        return (CPSS_OK);  
    }
    
    /*******************************************************************************
    *函数名称:cpss_vk_sched_proc_delete
    *功能:将纤程从调度器中删除
    *函数类型: 
    *参数: 
    *参数名称    类型       输入/输出 描述
    *ulSchedDesc UINT32     输入
    *tProcId     VK_PROC_ID 输入
    *函数返回:CPSS_OK or CPSS_ERROR
    *说明:                                 
    *******************************************************************************/
    INT32 cpss_vk_sched_proc_delete 
        (
        UINT32  ulSchedDesc, /* 调度器描述符 */
        VK_PROC_ID  tProcId /* 纤程标示 */
        ) 
    {
        VK_SCHED_ID tSchedId;
        CPSS_VK_IPC_MSG_NODE_T *  ptNode;
        
        /* 检查调度器描述符. */
        
        if ((ulSchedDesc <= 0) || (ulSchedDesc >= VK_MAX_SCHED_NUM) ||
            (g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse == FALSE))
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_proc_delete: descriptor invalid, descriptor = %d.\n"
                , ulSchedDesc);
            
            return (CPSS_ERROR);
        }
        
        /* 获得调度器标示. */
        
        tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
        
        /* 从READY队列移出. */
        
        if (tProcId->lStatus == VK_PROC_STATUS_READY)
        {
            cpss_util_q_fifo_remove (& tSchedId->qReady, & tProcId->node);
            
            /* 更新统计. */
            
            g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum - 1;
        }
        
        /* 从PEND队列移出. */
        
        if (tProcId->lStatus & VK_PROC_STATUS_PEND)
        {
            cpss_util_q_fifo_remove (& tSchedId->qPend, & tProcId->node);
            
            /* 更新统计. */
            
            g_astCpssVkSchedStatTbl[ulSchedDesc].lQPendNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQPendNum - 1;
        }
        
        /* 清空纤程邮箱. */
        
        while (1)
        {
            /* 获得节点. */
            
            ptNode = (CPSS_VK_IPC_MSG_NODE_T *) cpss_util_q_fifo_get (& tProcId->mailBox);
            
            /* 统计问题. */
            
            if (ptNode == NULL)
            {
                break;
            }
            
            /* 更新纤程邮箱计数. */
            
            tProcId->lMailNum = tProcId->lMailNum - 1;
            
            /* 释放消息. */
            
            cpss_ipc_msg_free ((void *) IPC_MSG_NODE_TO_HDR (ptNode));
        }
        
        /* 设置状态. */
        
        tProcId->lStatus = VK_PROC_STATUS_DEAD;
        
        /* 更新统计. */
        
        g_astCpssVkSchedStatTbl[ulSchedDesc].lProcNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lProcNum - 1;
        
        return (CPSS_OK);  
    }
    
    /*******************************************************************************
    *函数名称:cpss_vk_sched_proc_suspend
    *功能:将纤程在调度器中悬挂
    *函数类型: 
    *参数: 
    *参数名称    类型       输入/输出 描述
    *ulSchedDesc UINT32     输入
    *tProcId     VK_PROC_ID 输入
    *函数返回:CPSS_OK or CPSS_ERROR
    *说明:                                 
    *******************************************************************************/
    INT32 cpss_vk_sched_proc_suspend 
        (
        UINT32  ulSchedDesc, /* 调度器描述符 */
        VK_PROC_ID  tProcId /* 纤程标示 */
        ) 
    {
        VK_SCHED_ID tSchedId;
        
        /* 检查调度器描述符. */
        
        if ((ulSchedDesc <= 0) || (ulSchedDesc >= VK_MAX_SCHED_NUM) ||
            (g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse == FALSE))
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_proc_suspend: descriptor invalid, descriptor = %d.\n"
                , ulSchedDesc);
            
            return (CPSS_ERROR);
        }
        
        /* 获得调度器标示. */
        
        tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
        
        if (tProcId->lStatus == VK_PROC_STATUS_READY)
        {
            /* 从READY队列移出. */
            
            cpss_util_q_fifo_remove (& tSchedId->qReady, & tProcId->node);
            
            /* 更新统计. */
            
            g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum - 1;
        }
        
        /* 设置状态(SUSPEND or PEND+S). */
        
        tProcId->lStatus |= VK_PROC_STATUS_SUSPEND;
        
        return (CPSS_OK);  
    }
    
    /*******************************************************************************
    *函数名称:cpss_vk_sched_proc_resume
    *功能:将纤程在调度器中恢复
    *函数类型: 
    *参数: 
    *参数名称    类型       输入/输出 描述
    *ulSchedDesc UINT32     输入
    *tProcId     VK_PROC_ID 输入
    *函数返回:CPSS_OK or CPSS_ERROR
    *说明:                                 
    *******************************************************************************/
    INT32 cpss_vk_sched_proc_resume 
        (
        UINT32  ulSchedDesc, /* 调度器描述符 */
        VK_PROC_ID  tProcId /* 纤程标示 */
        ) 
    {
        VK_SCHED_ID tSchedId;
        
        /* 检查调度器描述符. */
        
        if ((ulSchedDesc <= 0) || (ulSchedDesc >= VK_MAX_SCHED_NUM) ||
            (g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse == FALSE))
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_proc_resume: descriptor invalid, descriptor = %d.\n"
                , ulSchedDesc);
            
            return (CPSS_ERROR);
        }
        
        /* 获得调度器标示. */
        
        tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
        
        if (tProcId->lStatus == VK_PROC_STATUS_SUSPEND)
        {
            /* 放入READY队列 */
            
            cpss_util_q_fifo_put (& tSchedId->qReady, & tProcId->node, 
                UTIL_Q_FIFO_KEY_TAIL);
            
            /* 更新统计. */
            
            g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum + 1;
        }
        
        /* 设置状态. */
        
        tProcId->lStatus &= ~VK_PROC_STATUS_SUSPEND;
        
        /* 更新READY计数. */
        
        tSchedId->lReadyNum += tProcId->lMailNum;
        
        return (CPSS_OK);  
    }
    
    /*******************************************************************************
    *函数名称:cpss_vk_sched_desc_self
    *功能:获得当前运行调度器描述符
    *函数类型: 
    *参数: 
    *参数名称 类型 输入/输出 描述
    *函数返回:调度器描述符 or VK_SCHED_DESC_INVALID
    *说明:                                 
    *******************************************************************************/
    #ifndef CPSS_DSP_CPU
    UINT32 cpss_vk_sched_desc_self (void)
    {
        UINT32  ulTaskDesc;
        
        /* 获得当前任务描述符. */
        
        ulTaskDesc = cpss_vos_task_desc_self ();   
        if (ulTaskDesc == VOS_TASK_DESC_INVALID)
        {
            return (VK_SCHED_DESC_INVALID);
        }
        return  g_astCpssVosTaskDescTbl[ulTaskDesc].ulSchedDesc ;
   }
    #else
     UINT32 cpss_vk_sched_desc_self (void)
    {
        UINT32  ulSchedDesc;
        
        /* 查找一个调度器描述符入口. */
        
        for (ulSchedDesc = 1; ulSchedDesc < VK_MAX_SCHED_NUM; ulSchedDesc++)
        {
            if (g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse == TRUE)
            {
            if (g_bSched == TRUE)
                {
                    return ulSchedDesc;
                }
            }
        }
            return (VK_SCHED_DESC_INVALID);
        }
    #endif
    
/*******************************************************************************
*函数名称:cpss_vk_sched_desc_get_by_name
*功能:通过调度器名字获得运行调度器描述符
*函数类型: 
*参数: 
*参数名称 类型   输入/输出 描述
*pcName   pcName 输入
*函数返回:调度器描述符 or VK_SCHED_DESC_INVALID
*说明:                                 
*******************************************************************************/
UINT32 cpss_vk_sched_desc_get_by_name 
    (
    INT8 *  pcName    
    )
    {
    UINT32  ulTaskDesc;
    UINT32  ulSchedDesc;
    VK_SCHED_ID tSchedId;

    /* Check name. */
    
    if (pcName == NULL)
        {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_desc_get_by_name: name not specified.\n"
                     );
        return (VK_SCHED_DESC_INVALID);
        }
            
    ulTaskDesc = cpss_vos_task_desc_get_by_name (pcName);
    if (ulTaskDesc == VOS_TASK_DESC_INVALID)
        {
        return (VK_SCHED_DESC_INVALID);
        }

    for (ulSchedDesc = 1; ulSchedDesc < VK_MAX_SCHED_NUM; ulSchedDesc++)
        {
        /* Judge whether scheduler existed? */

        if (g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse == FALSE)
            {
            continue;       
            }
        tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
        /* Compare task descriptor. */
                    
        if (ulTaskDesc == tSchedId->ulTaskDesc)
            {
            break;
            }
        }
    
    /* Not found.*/
    
    if (ulSchedDesc == VK_MAX_SCHED_NUM)
        {
        return (VK_SCHED_DESC_INVALID);
        }

    return (ulSchedDesc);
    }
 
    /*******************************************************************************
    *函数名称:cpss_vk_sched_error_set
    *功能:调度器错误设置
    *函数类型: 
    *参数: 
    *参数名称    类型   输入/输出 描述
    *ulSchedDesc UINT32 输入
    *lNewError   INT32  输入
    *函数返回:CPSS_OK or CPSS_ERROR
    *说明:                                 
    *******************************************************************************/
    INT32 cpss_vk_sched_error_set 
        (
        UINT32  ulSchedDesc, /* 调度器描述符 */
        INT32   lNewError /* 新错误 */
        )
    {
        VK_SCHED_ID tSchedId;
        
        /* 检查调度器描述符. */
        
        if ((ulSchedDesc <= 0) || (ulSchedDesc >= VK_MAX_SCHED_NUM) ||
            (g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse == FALSE))
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_error_set: descriptor invalid, descriptor = %d.\n"
                , ulSchedDesc);
            
            return (CPSS_ERROR);
        }
        
        /* 获得调度器标示. */
        
        tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
        
        /* 设置lError. */
        
        tSchedId->lError = lNewError;
        
        return (CPSS_OK);
    }
    
    /*******************************************************************************
    *函数名称:cpss_vk_sched_error_get
    *功能:调度器错误获得
    *函数类型: 
    *参数: 
    *参数名称    类型    输入/输出 描述
    *ulSchedDesc UINT32  输入
    *plError     INT32 * 输出
    *函数返回:CPSS_OK or CPSS_ERROR
    *说明:                                 
    *******************************************************************************/
    INT32 cpss_vk_sched_error_get 
        (
        UINT32  ulSchedDesc, /* 调度器描述符 */
        INT32 * plError /* 指向错误 */
        )
    {
        VK_SCHED_ID tSchedId;
        
        /* 检查调度器描述符. */
        
        if ((ulSchedDesc <= 0) || (ulSchedDesc >= VK_MAX_SCHED_NUM) ||
            (g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse == FALSE))
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_error_get: descriptor invalid, descriptor = %d.\n"
                , ulSchedDesc);
            
            return (CPSS_ERROR);
        }
        
        /* 检查指针. */
        
        if (plError == NULL)
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_error_get: argument not specified.\n"
                );
            
            return (CPSS_ERROR);
        }
        
        /* 获得调度器标示. */
        
        tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
        
        /* 获得错误. */
        
        *plError = tSchedId->lError;
        
        return (CPSS_OK);
    }
    
    /*******************************************************************************
    *函数名称:cpss_vk_sched_send
    *功能:调度器发送
    *函数类型: 
    *参数: 
    *参数名称    类型   输入/输出 描述
    *ulSchedDesc UINT32 输入
    *pcBuf       INT8 * 输入
    *lTimeout    INT32  输入
    *lPri        INT32  输入
    *lType       INT32  输入
    *函数返回:CPSS_OK or CPSS_ERROR
    *说明:                                 
    *******************************************************************************/
    INT32 cpss_vk_sched_send 
        (
        UINT32  ulSchedDesc, /* 调度器描述符 */
        INT8 *  pcBuf, /* 要发送的消息 */
        INT32   lTimeout, /* 等待的毫秒数 */
        INT32   lPri, /* 优先级VOS_MSG_PRI_NORMAL or VOS_MSG_PRI_URGENT */
        INT32   lType /* 类型 */
        )
    {
        VK_SCHED_ID tSchedId;
        CPSS_VK_IPC_MSG_NODE_T *  ptNode;
        
        /* 检查调度器描述符. */
        
        if ((ulSchedDesc <= 0) || (ulSchedDesc >= VK_MAX_SCHED_NUM) ||
            (g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse == FALSE))
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_send: descriptor invalid, descriptor = %d.\n"
                , ulSchedDesc);
            g_stCpssIpcStat.ulSchedDescOverFlow = g_stCpssIpcStat.ulSchedDescOverFlow + 1;
printf("ulSchedDesc=0x%x file=%s line=%d\n", ulSchedDesc, __FILE__, __LINE__);
            return (CPSS_ERROR);
        }
        
        /* 获得调度器标示. */
        
        tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
        
        if (lType == IPC_MSG_TYPE_LOCAL)
        {
            if (cpss_vk_sched_desc_self () == ulSchedDesc)
            {
                /* 指向节点. */
                
                ptNode = (CPSS_VK_IPC_MSG_NODE_T *) IPC_MSG_HDR_TO_NODE (pcBuf);
                
                /* 放入调度器邮箱. */
                
                cpss_util_q_fifo_put (& tSchedId->mailBox, & ptNode->node, 
                    UTIL_Q_FIFO_KEY_TAIL);
                
                /* 更新调度器邮箱计数. */
                
                tSchedId->lMailNum = tSchedId->lMailNum + 1;
                
                return (CPSS_OK);
            }
        }
        
#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS) || defined (CPSS_VOS_LINUX)    
        /* 消息队列发送 */
#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
    {
        BOOL bTraceIt;
        UINT8 *pucBuf = IPC_MSG_HDR_TO_DATA (pcBuf);
        pucBuf = pucBuf - sizeof(IPC_MSG_HDR_T) + 8;
        
        bTraceIt = cpss_trace_route_it_recv(pucBuf, NULL);
        if (TRUE == bTraceIt)
        {
            cpss_trace_route_info_set(CPSS_TRACE_NODE_LOC_DST, CPSS_TRACE_LAYER_TOSCHI, tSchedId->ulTaskDesc, 0,0,0, pucBuf);
        }
    }
#endif
        
        if (cpss_vos_msg_q_send (tSchedId->ulMsgQDesc, (INT8 *) (& pcBuf), 4, 
            lTimeout, lPri) == CPSS_ERROR)
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_send: cpss_vos_msg_q_send failed.\n");    
            g_stCpssIpcStat.ulMsgQSendError = g_stCpssIpcStat.ulMsgQSendError + 1;
            return (CPSS_ERROR);
        }
#endif

#ifdef CPSS_DSP_CPU    
    /* 指向节点. */
        
    ptNode = (CPSS_VK_IPC_MSG_NODE_T *) IPC_MSG_HDR_TO_NODE (pcBuf);

    /* Put in message queue. */

    cpss_util_q_fifo_put (& tSchedId->msgQ, & ptNode->node, UTIL_Q_FIFO_KEY_TAIL
                          );
    
    /* Update message number. */
        
    tSchedId->lMsgNum = tSchedId->lMsgNum + 1;
    
#endif
        return (CPSS_OK);
    }
#ifndef CPSS_DSP_CPU
    /*******************************************************************************
    *函数名称:cpss_vk_sched_info_get
    *功能:获得调度器信息
    *函数类型: 
    *参数: 
    *参数名称    类型              输入/输出 描述
    *ulSchedDesc UINT32            输入
    *ptSchedInfo CPSS_VK_SCHED_INFO_T * 输出
    *函数返回:CPSS_OK or CPSS_ERROR
    *说明:                                 
    *******************************************************************************/
    INT32 cpss_vk_sched_info_get 
        (
        UINT32  ulSchedDesc, /* 调度器描述符 */
        CPSS_VK_SCHED_INFO_T *   ptSchedInfo /* 指向调度器信息*/
        )
    {
        VK_SCHED_ID tSchedId;
        
        /* 检查调度器描述符. */
        
        if ((ulSchedDesc <= 0) || (ulSchedDesc >= VK_MAX_SCHED_NUM) ||
            (g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse == FALSE))
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_info_get: descriptor invalid, descriptor = %d.\n"
                , ulSchedDesc);
            
            return (CPSS_ERROR);
        }
        
        /* 获得调度器标示. */
        
        tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
        
        /* 获得调度器信息. */
        
        ptSchedInfo->ulSchedDesc = ulSchedDesc;
        ptSchedInfo->tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS) || defined (CPSS_VOS_LINUX)    
        ptSchedInfo->ulMsgQDesc = tSchedId->ulMsgQDesc;
#endif
        ptSchedInfo->ulTaskDesc = tSchedId->ulTaskDesc;
        
        return (CPSS_OK);
    }
    
    /*******************************************************************************
    *函数名称:cpss_vk_sched_stat_get
    *功能:获得调度器统计
    *函数类型: 
    *参数: 
    *参数名称    类型              输入/输出 描述
    *ulSchedDesc UINT32            输入
    *ptSchedStat CPSS_VK_SCHED_STAT_T * 输出
    *函数返回:CPSS_OK or CPSS_ERROR
    *说明:                                 
    *******************************************************************************/
    INT32 cpss_vk_sched_stat_get 
        (
        UINT32  ulSchedDesc, /* 调度器描述符 */
        CPSS_VK_SCHED_STAT_T *   ptSchedStat /* 指向调度器统计 */
        )
    {
        /* 检查调度器描述符. */
        
        if ((ulSchedDesc <= 0) || (ulSchedDesc >= VK_MAX_SCHED_NUM) ||
            (g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse == FALSE))
        {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_vk_sched_info_get: descriptor invalid, descriptor = %d.\n"
                , ulSchedDesc);
            
            return (CPSS_ERROR);
        }
        
        /* 获得调度器统计. */
        
        ptSchedStat->lQReadyNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQReadyNum;
        ptSchedStat->lQPendNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lQPendNum;
        ptSchedStat->lProcNum = g_astCpssVkSchedStatTbl[ulSchedDesc].lProcNum;
        ptSchedStat->ulCallNum = g_astCpssVkSchedStatTbl[ulSchedDesc].ulCallNum;
        
        /* 读取以后清除. */
        
        g_astCpssVkSchedStatTbl[ulSchedDesc].ulCallNum = 0;
        
        return (CPSS_OK);
    }
    
    /*******************************************************************************
    *函数名称:cpss_vk_sched_show
    *功能:显示调度器
    *函数类型: 
    *参数: 
    *参数名称    类型   输入/输出 描述
    *ulSchedDesc UINT32 输入
    *lLevel      INT32  输入
    *函数返回:CPSS_OK or CPSS_ERROR
    *说明:                                 
    *******************************************************************************/
    INT32 cpss_vk_sched_show 
        (
        UINT32  ulSchedDesc, /* 调度器描述符 */
        INT32   lLevel /* 0 = summary, 1 = detail */
        )
    {
        CPSS_VK_SCHED_INFO_T   tSchedInfo;
        CPSS_VK_SCHED_STAT_T   tSchedStat;
        VK_SCHED_ID tSchedId;
        IPC_MSG_HDR_T *   ptHdr;
        CPSS_COM_MSG_HEAD_T *    ptData;
        
        switch (lLevel)
        {
        case 0 :
            {
                /* 获得调度器信息. */
                
                if (cpss_vk_sched_info_get (ulSchedDesc, & tSchedInfo) == CPSS_ERROR)
                {
                    return (CPSS_ERROR);
                }
                
                cps__oams_shcmd_printf ("\n");
                cps__oams_shcmd_printf ("Scheduler descriptor: %d\n", tSchedInfo.ulSchedDesc);
                cps__oams_shcmd_printf ("Scheduler ID: 0x%-8x\n", (INT32) tSchedInfo.tSchedId);
#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS) || defined (CPSS_VOS_LINUX)    
                cps__oams_shcmd_printf ("Message queue descriptor: %d\n", tSchedInfo.ulMsgQDesc);
#endif
                cps__oams_shcmd_printf ("Task descriptor: %d\n", tSchedInfo.ulTaskDesc);
                
                break;
            }
            
        case 1 :
        default :
            {
                /* 获得调度器信息. */
                
                if (cpss_vk_sched_info_get (ulSchedDesc, & tSchedInfo) == CPSS_ERROR
                    )
                {
                    return (CPSS_ERROR);
                }
                
                /* 获得调度器统计. */
                
                if (cpss_vk_sched_stat_get (ulSchedDesc, & tSchedStat) == CPSS_ERROR
                    )
                {
                    return (CPSS_ERROR);
                }
                
                cps__oams_shcmd_printf ("\n");
                cps__oams_shcmd_printf ("Scheduler descriptor: %d\n", tSchedInfo.ulSchedDesc);
                cps__oams_shcmd_printf ("Scheduler ID: 0x%-8x\n", (INT32) tSchedInfo.tSchedId);
#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS) || defined (CPSS_VOS_LINUX)    
                cps__oams_shcmd_printf ("Message queue descriptor: %d\n", tSchedInfo.ulMsgQDesc);
#endif
                cps__oams_shcmd_printf ("Task descriptor: %d\n", tSchedInfo.ulTaskDesc);
                
#if defined(CPSS_VOS_VXWORKS) || defined (CPSS_VOS_WINDOWS) || defined (CPSS_VOS_LINUX)    
                /* 显示消息队列. */
                
                cpss_vos_msg_q_show (tSchedInfo.ulMsgQDesc, 0);
#endif                    
                /* 显示任务. */
                
                cpss_vos_task_show (tSchedInfo.ulTaskDesc, 0);
                
                tSchedId = tSchedInfo.tSchedId;
                cps__oams_shcmd_printf ("Current procedure descriptor: %d\n", tSchedId->ulProcDescCurr);
            
                ptHdr = (IPC_MSG_HDR_T *) tSchedId->pIpcMsgCurr;
                if (ptHdr != NULL)
                    {
                    ptData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr);
                    
                    cps__oams_shcmd_printf ("Current Inter-Procedure Communication(IPC) message ID: %08x\n", ptData->ulMsgId);
                    }
                
                cps__oams_shcmd_printf ("\n");
                cps__oams_shcmd_printf ("Number of procedure spawned: %d\n", tSchedStat.lProcNum);
                cps__oams_shcmd_printf ("Number of procedure in ready queue: %d\n", 
                    tSchedStat.lQReadyNum);
                cps__oams_shcmd_printf ("Number of procedure in pend queue: %d\n", 
                    tSchedStat.lQPendNum);
                cps__oams_shcmd_printf ("Number of procedure called: %d\n", tSchedStat.ulCallNum);
                
                
                break;
            }
        }
        
        return (CPSS_OK);
    }

VOID cpss_vk_proc_list 
(
    UINT32  ulSchedDesc, /* 调度器描述符 */
    INT32   lLevel /* 0 = summary, 1 = detail */
)
{
    UINT16 usGuid;
    switch (lLevel)
    {
        case 0 :
        {
            CPSS_VK_PROC_INFO_T    tProcInfo;
            CPSS_VK_PROC_INFO_T * ptProcInfo = &tProcInfo;
            INT32 lC;
            INT8    acStatusString [10];
            
            cps__oams_shcmd_printf ("\n----------------------- Schdel %d Info ----------------------------- \n", ulSchedDesc);
            cps__oams_shcmd_printf ("    Name        Procedure descriptor  Procedure ID  Priority  Status\n");
            cps__oams_shcmd_printf ("--------------  --------------------  ------------  --------  ------\n");
            
            for (lC = 1; lC < VK_MAX_PROC_CLASS_NUM; lC++)
            {
                if (g_astCpssVkProcClassTbl[lC].ulSchedDesc == ulSchedDesc)
                {
                    /* 获得纤程信息. */
                    
                    if (cpss_vk_proc_info_get (CPSS_VK_PD(lC, 0),  & tProcInfo) == CPSS_ERROR)
                    {
                        return ;
                    }
                    
                    /* 获得统计字符串. */
                    
                    cpss_vk_proc_tatus_string (ptProcInfo->ulProcDesc, acStatusString);
                    
                    cps__oams_shcmd_printf ("%-15s    0x%-15x   0x%08x     0x%-2x    %s\n", ptProcInfo->pcName, 
                        ptProcInfo->ulProcDesc, (INT32) ptProcInfo->tProcId, 
                        ptProcInfo->lPri, acStatusString);            
                    usGuid = (UINT16) ((ptProcInfo->ulProcDesc >> 16) & 0x0000ffff);
                    cpss_vk_proc_show(usGuid,ptProcInfo->ulProcDesc,4);          
                }
            }
            
            break;
        }
    default:
        break;
    }
        
        return ;
        
}
  
VOID cpss_vk_proc_list_all()
{
    INT32 lC;

    for (lC = 1; lC <= glVkSchedConfigNumEnt; lC++)
    {
        cpss_vk_proc_list(lC, 0);
    }
}

/*******************************************************************************
*函数名称:cpss_vk_proc_exec_time_stat_begin
*功能:
*函数类型: 
*参数: 
*参数名称    类型   输入/输出 描述
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                                               
*******************************************************************************/
BOOL g_bCpssMoniterProc = FALSE ;
INT32 cpss_vk_proc_exec_time_stat_begin
(
UINT16  usGuid, 
IPC_MSG_HDR_T *ptHdr 
)
{
    CPSS_COM_MSG_HEAD_T* pstComHdr ;

    if(g_bCpssMoniterProc == FALSE)
    {
        return CPSS_OK ;
    }

    pstComHdr = (CPSS_COM_MSG_HEAD_T*)IPC_MSG_HDR_TO_DATA(ptHdr) ;

    /*得到纤程号*/    
    if(usGuid > VK_MAX_PROC_CLASS_NUM)
    {
        return CPSS_ERROR ;
    }

    /*设置时间,消息ID,源目的地址信息,纤程信息*/
    cpss_clock_get(&g_stCpssVkProcTimeStat[usGuid].stTempBeginTime) ;
    g_stCpssVkProcTimeStat[usGuid].ulTempBeginTick = cpss_tick_get() ;
    g_stCpssVkProcTimeStat[usGuid].ulUseFlag = CPSS_VK_PROC_TIME_STAT_USE ;

    return CPSS_OK ;
}

/*******************************************************************************
*函数名称:cpss_vk_proc_exec_time_link_add
*功能:
*函数类型: 
*参数: 
*参数名称    类型   输入/输出 描述
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                                                
*******************************************************************************/
INT32 cpss_vk_proc_exec_time_link_add
(
 CPSS_PROC_EXECUT_TIME_T* pstExecTime,
 UINT16 usGuid
)
{
    CPSS_PROC_EXECUT_TIME_T* pstCurrTime = NULL;
    CPSS_PROC_EXECUT_TIME_T* pstPreTime = NULL;
    UINT32 usRecordNum = 0 ;
    
    if(g_apstProcTimeStat[usGuid] == NULL)
    {
        g_apstProcTimeStat[usGuid] = 
            (CPSS_PROC_EXECUT_TIME_T*)cpss_mem_malloc(sizeof(CPSS_PROC_EXECUT_TIME_T)) ;
        if(NULL == g_apstProcTimeStat[usGuid])
        {
            return CPSS_ERROR ;
        }
        pstCurrTime = g_apstProcTimeStat[usGuid] ;
    }
    else
    {
        pstCurrTime = g_apstProcTimeStat[usGuid] ;
        pstPreTime  = g_apstProcTimeStat[usGuid] ;

        /*得到当前链表的长度*/
        while(pstCurrTime != NULL)
        {
            usRecordNum = usRecordNum + 1 ;
            pstPreTime  = pstCurrTime ;
            pstCurrTime = pstCurrTime->pNext ;
        }

        /*如果链表长度已经超标则返回失败*/
        if(usRecordNum > CPSS_VK_PROC_TIME_STAT_NUM)
        {
            return CPSS_ERROR ;
        }

        /*申请链表内存并挂链*/
        pstPreTime->pNext = cpss_mem_malloc(sizeof(CPSS_PROC_EXECUT_TIME_T)) ;
        if(pstPreTime->pNext == NULL)
        {
            return CPSS_ERROR ;
        }

        pstCurrTime = pstPreTime->pNext ;
    }

    /*拷贝内容*/
    cpss_mem_memcpy(pstCurrTime,pstExecTime,sizeof(CPSS_PROC_EXECUT_TIME_T)) ;
    pstCurrTime->pNext = NULL ;

    return CPSS_OK ;
}

/*******************************************************************************
*函数名称:cpss_vk_proc_exec_time_link_free
*功能:
*函数类型: 
*参数: 
*参数名称    类型   输入/输出 描述
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                                              
*******************************************************************************/
VOID cpss_vk_proc_exec_time_link_free() 
{
    CPSS_PROC_EXECUT_TIME_T* pstExecTime;
    CPSS_PROC_EXECUT_TIME_T* pstLastTime ;
    UINT32 ulLoop ;

    for(ulLoop = 0 ;ulLoop<VK_MAX_PROC_CLASS_NUM+1 ;ulLoop++)
    {
        pstExecTime = g_apstProcTimeStat[ulLoop] ;

        while(pstExecTime != NULL)
        {
            pstLastTime = pstExecTime->pNext ;

            cpss_mem_free(pstExecTime) ;

            pstExecTime = pstLastTime ;
        }

        g_apstProcTimeStat[ulLoop] = NULL ;
    }
}

/*******************************************************************************
*函数名称:cpss_vk_proc_exec_time_stat_end
*功能:
*函数类型: 
*参数: 
*参数名称    类型   输入/输出 描述
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                                                 
*******************************************************************************/
INT32 cpss_vk_proc_exec_time_stat_end
(
UINT16  usGuid, 
IPC_MSG_HDR_T *ptHdr
)
{
    CPSS_COM_MSG_HEAD_T* pstComHdr ;
    UINT32 ulInterval = 0 ;

    if(g_bCpssMoniterProc == FALSE)
    {
        return CPSS_OK ;
    }
    
    pstComHdr = (CPSS_COM_MSG_HEAD_T*)IPC_MSG_HDR_TO_DATA(ptHdr) ;
    
    if(usGuid > VK_MAX_PROC_CLASS_NUM)
    {
        return CPSS_ERROR ;
    }
    
    ulInterval = cpss_tick_get() - g_stCpssVkProcTimeStat[usGuid].ulTempBeginTick;

    /*记录*/
    if(ulInterval >= g_stCpssVkProcTimeStat[usGuid].ulExecTick)
    {
        /*设置时间,消息ID,源目的地址信息,纤程信息*/
        g_stCpssVkProcTimeStat[usGuid].stExecBeginTime = g_stCpssVkProcTimeStat[usGuid].stTempBeginTime ;
        cpss_clock_get(&g_stCpssVkProcTimeStat[usGuid].stExecEndTime) ;
        g_stCpssVkProcTimeStat[usGuid].ulExecTick = ulInterval ;

        g_stCpssVkProcTimeStat[usGuid].ulMsgId   = pstComHdr->ulMsgId ;
        g_stCpssVkProcTimeStat[usGuid].stSrcPid  = pstComHdr->stSrcProc ;
        g_stCpssVkProcTimeStat[usGuid].stDestPid = pstComHdr->stDstProc ;
        g_stCpssVkProcTimeStat[usGuid].ulExeProcId = usGuid ;

        /*如果大于或等于此门限值则记录在连表中*/
        if(ulInterval >= CPSS_VK_PROC_TIME_EXEC_TIME_MAX)
        {
            cpss_vk_proc_exec_time_link_add(&g_stCpssVkProcTimeStat[usGuid],usGuid) ;
        }
    }
    
    return CPSS_OK ;
}

/*******************************************************************************
*函数名称:cpss_vk_proc_exec_time_stat_show
*功能:
*函数类型: 
*参数: 
*参数名称    类型   输入/输出 描述
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                                               
*******************************************************************************/
extern VOID cpss_com_time_print(UINT8* pucInfo,CPSS_TIME_T* pstTime) ;
VOID cpss_vk_proc_exec_time_stat_show()
{
    UINT32 ulLoop = 0 ;
    CPSS_PROC_EXECUT_TIME_T* pstExecTime;

    /*打印纤程执行的最长时间*/
    for(ulLoop = 0 ;ulLoop < VK_MAX_PROC_CLASS_NUM + 1 ;ulLoop++)
    {
        if(g_stCpssVkProcTimeStat[ulLoop].ulUseFlag == CPSS_VK_PROC_TIME_STAT_USE)
        {
            cps__oams_shcmd_printf("----------------Guid=%d--------------\n",g_stCpssVkProcTimeStat[ulLoop].ulExeProcId) ;
            cps__oams_shcmd_printf("g_stCpssVkProcTimeStat[%d].ulExecTick=%d\n",ulLoop,g_stCpssVkProcTimeStat[ulLoop].ulExecTick) ;
            cps__oams_shcmd_printf("g_stCpssVkProcTimeStat[%d].ulMsgId=0x%x\n",ulLoop,g_stCpssVkProcTimeStat[ulLoop].ulMsgId) ;
            cps__oams_shcmd_printf("g_stCpssVkProcTimeStat[%d].stSrcPid.stLogicAddr=0x%x\n",
                ulLoop,*(UINT32*)&g_stCpssVkProcTimeStat[ulLoop].stSrcPid.stLogicAddr) ;
            cps__oams_shcmd_printf("g_stCpssVkProcTimeStat[%d].stSrcPid.ulAddrFlag=%d\n",
                ulLoop,*(UINT32*)&g_stCpssVkProcTimeStat[ulLoop].stSrcPid.ulAddrFlag) ;
            cps__oams_shcmd_printf("g_stCpssVkProcTimeStat[%d].stSrcPid.ulPd=0x%x\n",
                ulLoop,*(UINT32*)&g_stCpssVkProcTimeStat[ulLoop].stSrcPid.ulPd) ;
            cps__oams_shcmd_printf("g_stCpssVkProcTimeStat[%d].stDestPid.stLogicAddr=0x%x\n",
                ulLoop,*(UINT32*)&g_stCpssVkProcTimeStat[ulLoop].stDestPid.stLogicAddr) ;
            cps__oams_shcmd_printf("g_stCpssVkProcTimeStat[%d].stDestPid.ulAddrFlag=%d\n",
                ulLoop,*(UINT32*)&g_stCpssVkProcTimeStat[ulLoop].stDestPid.ulAddrFlag) ;
            cps__oams_shcmd_printf("g_stCpssVkProcTimeStat[%d].stDestPid.ulPd=0x%x\n",
                ulLoop,*(UINT32*)&g_stCpssVkProcTimeStat[ulLoop].stDestPid.ulPd) ;
            
            cpss_com_time_print("stExecBeginTime=",&g_stCpssVkProcTimeStat[ulLoop].stExecBeginTime) ;
            cpss_com_time_print("stExecEndTime=",&g_stCpssVkProcTimeStat[ulLoop].stExecEndTime) ;
        }
    }

    cps__oams_shcmd_printf("----------------proc execut timeout recorders--------------\n") ;
    /*打印超过纤程的执行时间的记录*/
    for(ulLoop = 0 ;ulLoop < VK_MAX_PROC_CLASS_NUM + 1 ;ulLoop++)
    {
        pstExecTime = g_apstProcTimeStat[ulLoop] ;
        
        while(pstExecTime!=NULL)
        {
            cps__oams_shcmd_printf("<<<<<<<<<<<Guid=%d>>>>>>>>>>\n",pstExecTime->ulExeProcId) ;
            cps__oams_shcmd_printf("ulExecTick=%d\n",pstExecTime->ulExecTick) ;
            cps__oams_shcmd_printf("ulMsgId=0x%x\n",pstExecTime->ulMsgId) ;
            cps__oams_shcmd_printf("stSrcPid.stLogicAddr=0x%x\n",
                *(UINT32*)&pstExecTime->stSrcPid.stLogicAddr) ;
            cps__oams_shcmd_printf("stSrcPid.ulAddrFlag=%d\n",
                *(UINT32*)&pstExecTime->stSrcPid.ulAddrFlag) ;
            cps__oams_shcmd_printf("stSrcPid.ulPd=0x%x\n",
                *(UINT32*)&pstExecTime->stSrcPid.ulPd) ;
            cps__oams_shcmd_printf("stDestPid.stLogicAddr=0x%x\n",
                *(UINT32*)&pstExecTime->stDestPid.stLogicAddr) ;
            cps__oams_shcmd_printf("stDestPid.ulAddrFlag=%d\n",
                *(UINT32*)&pstExecTime->stDestPid.ulAddrFlag) ;
            cps__oams_shcmd_printf("stDestPid.ulPd=0x%x\n",
                *(UINT32*)&pstExecTime->stDestPid.ulPd) ;
            
            cpss_com_time_print("stExecBeginTime=",&pstExecTime->stExecBeginTime) ;
            cpss_com_time_print("stExecEndTime=",&pstExecTime->stExecEndTime) ;

            pstExecTime = pstExecTime->pNext ;
        }
    }
}

/*******************************************************************************
*函数名称:cpss_vk_proc_exec_time_stat_zero
*功能:
*函数类型: 
*参数: 
*参数名称    类型   输入/输出 描述
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                                                
*******************************************************************************/
VOID cpss_vk_proc_exec_time_stat_zero()
{
    /*清零统计内存*/
    cpss_mem_memset(&g_stCpssVkProcTimeStat,0,
        sizeof(CPSS_PROC_EXECUT_TIME_T)*(VK_MAX_PROC_CLASS_NUM+1)) ;

    /*释放链表内存*/
    cpss_vk_proc_exec_time_link_free() ;
}

/*******************************************************************************
*函数名称:cpss_vk_proc_exec_time_stat_zero
*功能:
*函数类型: 
*参数: 
*参数名称    类型   输入/输出 描述
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                                               
*******************************************************************************/
INT32 cpss_vk_proc_exec_time_stat_open()
{
    g_bCpssMoniterProc = TRUE ;

    return CPSS_OK ;
}

#define CPSS_DBG_MAX_SCH_TASK_SUM 10
#define CPSS_DBG_SAV_MSG_MAX_LEN 12
#define CPSS_DBG_DEF_SAV_CALL_SUM 4096
typedef struct structCpssDbgProTime{
    UINT8 aucProName[64];
    UINT32 ulStartTick;
    UINT32 ulEndTick;
    UINT32 ulMsgID;
    UINT32 ulMsgLen;
    UINT32 ulIndex;
    CPSS_TIME_T tAbsTime;
    UINT8 aucMsgBuf[CPSS_DBG_SAV_MSG_MAX_LEN];
}CPSS_DBG_PRO_TIME_T;

typedef struct structCpssDbgProTask{
    INT32 lTaskId;
    UINT32 ulCallPosition;
    CPSS_DBG_PRO_TIME_T *pastProTime;
}CPSS_DBG_PRO_TIME_TASK_T;


CPSS_DBG_PRO_TIME_TASK_T gastCpssProTimeTaskTab[CPSS_DBG_MAX_SCH_TASK_SUM] = {0};

UINT32 gulCpssDbgSavCallSum = CPSS_DBG_DEF_SAV_CALL_SUM;
UINT32 gulCpssDbgProTimeSem = CPSS_VOS_MUTEXD_INVALID;
UINT32 guiCpssDbgProTimeTaskLockFlag = 0;

#define CPSS_PRO_TIME_LONG_TICK_BASE 5
#define CPSS_PRO_TIME_LONG_STAT_MAX_SUM 1024
UINT32 gulCpssDbgProTimeLongSem = CPSS_VOS_MUTEXD_INVALID;
CPSS_DBG_PRO_TIME_T gastCpssDbgProTime[CPSS_PRO_TIME_LONG_STAT_MAX_SUM] = {0};
UINT32 gulCpssDbgProTimeLongBase = CPSS_PRO_TIME_LONG_TICK_BASE;
UINT32 gulCpssDbgProTimeLongPosition = 0;

CPSS_DBG_PRO_TIME_TASK_T * cpss_dbg_pro_time_get_ctrl_block(UINT32 *pvulIndex)
{
    UINT32 ulLoop;
    static UINT32 ulCallIndex = 0;
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
    INT32 lCurTaskId = taskIdSelf();
#endif
    if(CPSS_VOS_MUTEXD_INVALID == gulCpssDbgProTimeSem)
    {
        return NULL;
    }
    cpss_vos_mutex_p(gulCpssDbgProTimeSem, WAIT_FOREVER);
    for(ulLoop = 0; ulLoop < CPSS_DBG_MAX_SCH_TASK_SUM; ulLoop++)
    {
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
        if(gastCpssProTimeTaskTab[ulLoop].lTaskId == lCurTaskId)
        {
            *pvulIndex = ulCallIndex;
            ulCallIndex = ulCallIndex + 1;
            cpss_vos_mutex_v(gulCpssDbgProTimeSem);
            return &gastCpssProTimeTaskTab[ulLoop];
        }
        else if(gastCpssProTimeTaskTab[ulLoop].lTaskId == 0)
        {
            gastCpssProTimeTaskTab[ulLoop].pastProTime = malloc(sizeof(CPSS_DBG_PRO_TIME_T) * gulCpssDbgSavCallSum);
            if(NULL == gastCpssProTimeTaskTab[ulLoop].pastProTime)
            {
                cpss_vos_mutex_v(gulCpssDbgProTimeSem);
                return NULL;
            }
            memset(gastCpssProTimeTaskTab[ulLoop].pastProTime, 0, sizeof(CPSS_DBG_PRO_TIME_T) * gulCpssDbgSavCallSum);
            gastCpssProTimeTaskTab[ulLoop].lTaskId = lCurTaskId;
            gastCpssProTimeTaskTab[ulLoop].ulCallPosition = 0;
            *pvulIndex = ulCallIndex;
            ulCallIndex = ulCallIndex + 1;
            cpss_vos_mutex_v(gulCpssDbgProTimeSem);
            return &gastCpssProTimeTaskTab[ulLoop];
        }
#endif
    }
    cpss_vos_mutex_v(gulCpssDbgProTimeSem);
    return NULL;
}

UINT32 cpss_dbg_get_current_time()
{
#ifdef CPSS_VOS_VXWORKS
    return tickGet();
#else
    return cpss_tick_get();
#endif
}

void cpss_dbg_pro_time_tasklock()
{
#ifdef CPSS_VOS_VXWORKS
    if(0 != guiCpssDbgProTimeTaskLockFlag)
    {
        taskLock();
    }
#endif
}
void cpss_dbg_pro_time_taskunlock()
{
#ifdef CPSS_VOS_VXWORKS
    if(0 != guiCpssDbgProTimeTaskLockFlag)
    {
        taskUnlock();
    }
#endif
}

void cpss_dbg_pro_time_pause()
{
    gulCpssDbgProIsStatPause = 1;
    cps__oams_shcmd_printf("The test program paused in %d.\n", cpss_dbg_get_current_time());
}

void cpss_dbg_pro_time_resume()
{
    gulCpssDbgProIsStatPause = 0;
    cps__oams_shcmd_printf("The test program resumed in %d.\n", cpss_dbg_get_current_time());
}

void cpss_dbg_pro_time_start(UINT32 ulRecSum, UINT32 ulLongTick)
{

    UINT32 ulLoop;

    if(CPSS_VOS_MUTEXD_INVALID == gulCpssDbgProTimeSem)
    {
        gulCpssDbgProTimeSem = cpss_vos_mutex_create();
    }

    cpss_vos_mutex_p(gulCpssDbgProTimeSem, WAIT_FOREVER);
    for(ulLoop = 0; ulLoop < CPSS_DBG_MAX_SCH_TASK_SUM; ulLoop++)
    {
        if(NULL != gastCpssProTimeTaskTab[ulLoop].pastProTime)
        {
            cpss_vos_mutex_v(gulCpssDbgProTimeSem);
            cps__oams_shcmd_printf("The test program start failure, because memroy error.\n");
            return;
        }
    }
    memset(gastCpssProTimeTaskTab, 0, sizeof(CPSS_DBG_PRO_TIME_TASK_T) * CPSS_DBG_MAX_SCH_TASK_SUM);
    gulCpssDbgProIsStatPause = 0;
    if(0 != ulRecSum)
    {
        gulCpssDbgSavCallSum = ulRecSum;
    }
    cpss_vos_mutex_v(gulCpssDbgProTimeSem);

    if(CPSS_VOS_MUTEXD_INVALID == gulCpssDbgProTimeLongSem)
    {
        gulCpssDbgProTimeLongSem = cpss_vos_mutex_create();
    }
    cpss_vos_mutex_p(gulCpssDbgProTimeLongSem, WAIT_FOREVER);
    memset(gastCpssDbgProTime, 0, sizeof(CPSS_DBG_PRO_TIME_T) * CPSS_PRO_TIME_LONG_STAT_MAX_SUM);
    gulCpssDbgProTimeLongPosition = 0;
    if(0 != ulLongTick)
    {
        gulCpssDbgProTimeLongBase = ulLongTick;
    }
    cpss_vos_mutex_v(gulCpssDbgProTimeLongSem);

    cps__oams_shcmd_printf("The test program start.\n");
}

void cpss_dbg_pro_time_end()
{
    UINT32 ulLoop;

    if(CPSS_VOS_MUTEXD_INVALID == gulCpssDbgProTimeSem)
    {
        cps__oams_shcmd_printf("The test program has not been inited.\n");
        return;
    }
    cpss_vos_mutex_p(gulCpssDbgProTimeSem, WAIT_FOREVER);
    for(ulLoop = 0; ulLoop < CPSS_DBG_MAX_SCH_TASK_SUM; ulLoop++)
    {
        if(NULL != gastCpssProTimeTaskTab[ulLoop].pastProTime)
        {
            free(gastCpssProTimeTaskTab[ulLoop].pastProTime);
        }
    }
    memset(gastCpssProTimeTaskTab, 0, sizeof(CPSS_DBG_PRO_TIME_TASK_T) * CPSS_DBG_MAX_SCH_TASK_SUM);
    gulCpssDbgProIsStatPause = 1;
    cpss_dbg_pro_time_taskunlock();
    cpss_vos_mutex_v(gulCpssDbgProTimeSem);
    cps__oams_shcmd_printf("The test program end.\n");
}

void cpss_dbg_pro_time_add_start_ex(VK_PROC_ID tProcID, CPSS_COM_MSG_HEAD_T *ptMsgHead)
{
    UINT32 ulMsgLen;
    UINT32 ulIndex;
    CPSS_DBG_PRO_TIME_TASK_T *pstTask;
/**
    if(cpss_dbg_pro_time_is_enable())
**/
    {
        pstTask = cpss_dbg_pro_time_get_ctrl_block(&ulIndex);
        if(NULL == pstTask)
        {
            return;
        }
        pstTask->ulCallPosition %= gulCpssDbgSavCallSum;
        strcpy(pstTask->pastProTime[pstTask->ulCallPosition].aucProName, tProcID->acName);
        pstTask->pastProTime[pstTask->ulCallPosition].ulMsgID = ptMsgHead->ulMsgId;
        pstTask->pastProTime[pstTask->ulCallPosition].ulMsgLen = ptMsgHead->ulLen;
        cpss_clock_get(&pstTask->pastProTime[pstTask->ulCallPosition].tAbsTime);
        ulMsgLen = (ptMsgHead->ulLen > CPSS_DBG_SAV_MSG_MAX_LEN) ? CPSS_DBG_SAV_MSG_MAX_LEN : ptMsgHead->ulLen;
        memcpy(pstTask->pastProTime[pstTask->ulCallPosition].aucMsgBuf, ptMsgHead->pucBuf, ulMsgLen);
        pstTask->pastProTime[pstTask->ulCallPosition].ulIndex = ulIndex;
        pstTask->pastProTime[pstTask->ulCallPosition].ulStartTick = cpss_dbg_get_current_time();
        pstTask->pastProTime[pstTask->ulCallPosition].ulEndTick = 10000000 + cpss_dbg_get_current_time();
        cpss_dbg_pro_time_tasklock();
    }
}

void cpss_dbg_pro_time_add_long_stat(CPSS_DBG_PRO_TIME_T *pProTimeInfo)
{
    cpss_vos_mutex_p(gulCpssDbgProTimeLongSem, WAIT_FOREVER);
    memcpy(&gastCpssDbgProTime[gulCpssDbgProTimeLongPosition], pProTimeInfo, sizeof(CPSS_DBG_PRO_TIME_T));
    gulCpssDbgProTimeLongPosition = CPSS_MOD((gulCpssDbgProTimeLongPosition + 1), CPSS_PRO_TIME_LONG_STAT_MAX_SUM);
    cpss_vos_mutex_v(gulCpssDbgProTimeLongSem);
}

void cpss_dbg_pro_time_add_end_ex()
{
    UINT32 ulIndex;
    CPSS_DBG_PRO_TIME_TASK_T *pstTask;
/**
    if(cpss_dbg_pro_time_is_enable())
**/    
    {
        pstTask = cpss_dbg_pro_time_get_ctrl_block(&ulIndex);
        if(NULL == pstTask)
        {
            return;
        }
        cpss_dbg_pro_time_taskunlock();
        pstTask->pastProTime[pstTask->ulCallPosition].ulEndTick = cpss_dbg_get_current_time();
        if((pstTask->pastProTime[pstTask->ulCallPosition].ulEndTick - pstTask->pastProTime[pstTask->ulCallPosition].ulStartTick) 
            >= gulCpssDbgProTimeLongBase)
        {
            cpss_dbg_pro_time_add_long_stat(&pstTask->pastProTime[pstTask->ulCallPosition]);
        }
        pstTask->ulCallPosition = pstTask->ulCallPosition + 1;
        pstTask->ulCallPosition %= gulCpssDbgSavCallSum;
    }
}

void cpss_dbg_pro_time_show_long()
{
    UINT32 ulLoop;
    UINT32 ulLoop2;
    UINT32 ulMsgLen;
    UINT32 ulCounter;
    UINT8 aucAbsTimeString[50];
    UINT32 ulCurrentPosition = gulCpssDbgProTimeLongPosition;
    UINT8 aucMsgBufStr[CPSS_DBG_SAV_MSG_MAX_LEN * 3];

    cps__oams_shcmd_printf("%-16s %-8s %-8s %-10s %-8s %-8s\n", "PRO_NAME", "MSG_ID", "T_START", "ABS_TIME", "PRO_TIME", "MSG_INFO");
    for(ulCounter = 0; ulCounter < CPSS_PRO_TIME_LONG_STAT_MAX_SUM; ulCounter++)
    {
        ulLoop = CPSS_MOD((ulCounter + ulCurrentPosition) , CPSS_PRO_TIME_LONG_STAT_MAX_SUM);
        if(0 == gastCpssDbgProTime[ulLoop].aucProName[0])
        {
            continue;
        }
        aucMsgBufStr[0] = 0;
        ulMsgLen = (gastCpssDbgProTime[ulLoop].ulMsgLen > CPSS_DBG_SAV_MSG_MAX_LEN) ? CPSS_DBG_SAV_MSG_MAX_LEN : gastCpssDbgProTime[ulLoop].ulMsgLen;
        snprintf(&aucMsgBufStr[0], CPSS_DBG_SAV_MSG_MAX_LEN * 3, "[%d]",gastCpssDbgProTime[ulLoop].ulMsgLen);
        for(ulLoop2 = 0; ulLoop2 < ulMsgLen; ulLoop2++)
        {
            snprintf(&aucMsgBufStr[strlen(aucMsgBufStr)], CPSS_DBG_SAV_MSG_MAX_LEN * 3 - strlen(aucMsgBufStr), "%02X", gastCpssDbgProTime[ulLoop].aucMsgBuf[ulLoop2]);
        }

        snprintf(aucAbsTimeString, 50, "%02d:%02d:%02d", 
            gastCpssDbgProTime[ulLoop].tAbsTime.ucHour,
            gastCpssDbgProTime[ulLoop].tAbsTime.ucMinute,
            gastCpssDbgProTime[ulLoop].tAbsTime.ucSecond);

        cps__oams_shcmd_printf("%-16s %-8X %-8d %-10s %-8d %-20s\n",
            gastCpssDbgProTime[ulLoop].aucProName,
            gastCpssDbgProTime[ulLoop].ulMsgID,
            gastCpssDbgProTime[ulLoop].ulStartTick,
            aucAbsTimeString,
            gastCpssDbgProTime[ulLoop].ulEndTick - gastCpssDbgProTime[ulLoop].ulStartTick,
            aucMsgBufStr);
    }
}

void cpss_dbg_pro_time_show(UINT8 *pProcName, UINT32 ulMinTick, UINT32 ulMinStartTick, UINT32 ulMaxStartTick)
{
    UINT32 ulLoop;
    UINT32 ulLoop2;
    UINT32 ulMsgLen;
    UINT32 ulCounter;
    UINT8 aucAbsTimeString[50];
    UINT32 ulCurrentPosition;
    UINT8 aucMsgBufStr[CPSS_DBG_SAV_MSG_MAX_LEN * 3];

    UINT32 ulI;
    INT32 lTaskPri = 0;
    CPSS_DBG_PRO_TIME_TASK_T *pTask;

    for(ulI = 0; ulI < CPSS_DBG_MAX_SCH_TASK_SUM; ulI++)
    {
        pTask = &gastCpssProTimeTaskTab[ulI];
        if(pTask->lTaskId == 0)
        {
            break;
        }
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
        taskPriorityGet(pTask->lTaskId, &lTaskPri);
        cps__oams_shcmd_printf("\n%-16s %-8s %-8s\n", "TASK_NAME", "TASK_ID", "TASK_PRO");
        cps__oams_shcmd_printf("%-16s %-8X %-8d\n\n", taskName(pTask->lTaskId), pTask->lTaskId, lTaskPri);
#endif
        ulCurrentPosition = pTask->ulCallPosition;
        cps__oams_shcmd_printf("%-8s %-16s%-8s %-10s%-9s%-8s%-8s\n", "INDEX", "PRO_NAME", "MSG_ID", "T_START", "ABS_TIME", "PRO_TIME", "MSG_INFO");
        for(ulCounter = 0; ulCounter < gulCpssDbgSavCallSum; ulCounter++)
        {
            ulLoop = CPSS_MOD((ulCounter + ulCurrentPosition), gulCpssDbgSavCallSum);
            if(0 == pTask->pastProTime[ulLoop].aucProName[0])
            {
                continue;
            }
            if((pTask->pastProTime[ulLoop].ulEndTick - pTask->pastProTime[ulLoop].ulStartTick) < ulMinTick)
            {
                continue;
            }
            if((0 != ulMinStartTick) && (pTask->pastProTime[ulLoop].ulStartTick < ulMinStartTick))
            {
                continue;
            }
            if((0 != ulMaxStartTick) && (pTask->pastProTime[ulLoop].ulEndTick > ulMaxStartTick))
            {
                continue;
            }
            if(((NULL != pProcName) && (strcmp(pProcName, "all") != 0)) 
                && (strcmp(pTask->pastProTime[ulLoop].aucProName, pProcName) != 0))
            {
                continue;
            }
            aucMsgBufStr[0] = 0;
            ulMsgLen = (pTask->pastProTime[ulLoop].ulMsgLen > CPSS_DBG_SAV_MSG_MAX_LEN) ? CPSS_DBG_SAV_MSG_MAX_LEN : pTask->pastProTime[ulLoop].ulMsgLen;
            snprintf(&aucMsgBufStr[0], CPSS_DBG_SAV_MSG_MAX_LEN * 3, "[%d]",pTask->pastProTime[ulLoop].ulMsgLen);
            for(ulLoop2 = 0; ulLoop2 < ulMsgLen; ulLoop2++)
            {
                snprintf(&aucMsgBufStr[strlen(aucMsgBufStr)], CPSS_DBG_SAV_MSG_MAX_LEN * 3 - strlen(aucMsgBufStr), "%02X", pTask->pastProTime[ulLoop].aucMsgBuf[ulLoop2]);
            }

            snprintf(aucAbsTimeString, 50, "%02d:%02d:%02d", 
                pTask->pastProTime[ulLoop].tAbsTime.ucHour,
                pTask->pastProTime[ulLoop].tAbsTime.ucMinute,
                pTask->pastProTime[ulLoop].tAbsTime.ucSecond);

            cps__oams_shcmd_printf("%08X %-16s%-8X %-10d%-9s%-8d%-20s\n",
                pTask->pastProTime[ulLoop].ulIndex,
                pTask->pastProTime[ulLoop].aucProName,
                pTask->pastProTime[ulLoop].ulMsgID,
                pTask->pastProTime[ulLoop].ulStartTick,
            aucAbsTimeString,
                pTask->pastProTime[ulLoop].ulEndTick - pTask->pastProTime[ulLoop].ulStartTick,
            aucMsgBufStr);
        }
    }
}

extern CPSS_VOS_MSG_Q_DESC_T   g_astCpssVosMsgQDescTbl[];

VOS_MSG_Q_ID  cpss_vk_get_msg_q_id(UINT32 ulPd)
{   
    UINT32 ulSchedDesc;
    VK_SCHED_ID tSchedId;
    UINT16 usGuid;
    UINT32 ulIndex;
    
    usGuid = (UINT16) ((ulPd >> 16) & 0x0000ffff);
    
    /* 检查GUID. */
    
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) ||
    (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
    {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_ipc_msg_send: globally unique identifier invalid, descriptor = 0x%08x.\n"
                     , ulPd);
        g_stCpssIpcStat.ulGuidOverFlow = g_stCpssIpcStat.ulGuidOverFlow + 1;
        return (VOS_MSG_Q_ID)(CPSS_ERROR);
    }
    
    /* 获得索引. */
    
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin;
    
    /* 检查纤程描述符. */
    
    
    ulSchedDesc = g_astCpssVkProcClassTbl[usGuid].ulSchedDesc;
     
    tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;
     
    return g_astCpssVosMsgQDescTbl[tSchedId->ulMsgQDesc].msgQId;
}

#else
void cpss_dbg_pro_time_pause()
{
    cps__oams_shcmd_printf ("cpss_dbg_pro_time_pause can not support in this board!\n");    
    return;
}

INT32 cpss_vk_sched_show 
(
    UINT32  ulSchedDesc, /* 调度器描述符 */
    INT32   lLevel /* 0 = summary, 1 = detail */
)
{
    cps__oams_shcmd_printf ("cpss_vk_sched_show can not support in this board!\n");    
    return CPSS_OK;
}
void cpss_dbg_pro_time_resume()
{
    cps__oams_shcmd_printf ("cpss_dbg_pro_time_resume can not support in this board!\n");    
}
void cpss_dbg_pro_time_start(UINT32 ulRecSum, UINT32 ulLongTick)
{
    cps__oams_shcmd_printf ("cpss_dbg_pro_time_start can not support in this board!\n");    
}
void cpss_dbg_pro_time_end()
{
    cps__oams_shcmd_printf ("cpss_dbg_pro_time_end can not support in this board!\n");    
}
void cpss_dbg_pro_time_show(UINT8 *pProcName, UINT32 ulMinTick, UINT32 ulMinStartTick, UINT32 ulMaxStartTick)
{
    cps__oams_shcmd_printf ("cpss_dbg_pro_time_show can not support in this board!\n");    
}
void cpss_dbg_pro_time_show_long()
{
    cps__oams_shcmd_printf ("cpss_dbg_pro_time_show_long can not support in this board!\n");    
}
#endif


/*******************************************************************************
*函数名称:cpss_load_module
*功能:装载对象模块文件
*函数类型: 
*参数: 
*参数名称           类型   输入/输出 描述
*pucPatchFileName   INT8 * 输入
*ulOffset           INT32  输入
*ulSize             INT32  输入
*函数返回:模块ID or NULL
*说明:                                 
*******************************************************************************/
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
MODULE_ID cpss_load_module
#else
 UINT32 cpss_load_module 
#endif
    (
    INT8 *  pucPatchFileName, /* file name */
    UINT32  ulOffset, /* file offset */
    UINT32  ulSize /* file size */
    )
    {
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
    INT32   lFdIn;
    INT8	acOut [128];
    INT32	lFdOut;
    INT32   lBytesRead;  
    INT8	acBuffer [2];
    INT8 *	pcText;
    INT8 * 	pcData;
    INT8 *	pcBss;
    MODULE_ID   moduleId;
    extern int  errno;
    
    /* Check file name. */
    
    if (pucPatchFileName == NULL)
        {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_load_module: file name not specified.\n"
                     );
            
        return (NULL);
        }

    lFdIn = open (pucPatchFileName, O_RDONLY, 0);
    if (lFdIn == ERROR)
        {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_load_module: open failed, errno = 0x%x.\n",
                     errno);
            
        return (NULL);
        }
    
    memset ((void *) acOut, '\0', 128);
    sprintf (acOut, "%s%s", "R:/", "module");
    
    lFdOut = creat (acOut, O_RDWR);
    if (lFdOut == ERROR)
        {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_load_module: create failed, errno = 0x%x.\n",
                     errno);
            
        close (lFdIn);
        
        return (NULL);
        }

    if (lseek (lFdIn, ulOffset, SEEK_CUR) == ERROR)
        {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_load_module: lseek failed, errno = 0x%x.\n",
                     errno);
            
        close (lFdIn);
        close (lFdOut);
        remove (acOut);
                
        return (NULL);
        }
    
    /* If the length of object file is very long, we have to allocate a large memory
     * to buffer it. It is possible that allocation failed. So we have to use the 
     * stupid style.
     */
       
    while (ulSize)
        {
        lBytesRead = read (lFdIn, acBuffer, 1);
        if (lBytesRead == ERROR)
            {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_load_module: read failed, errno = 0x%x.\n",
                         errno);
            close (lFdIn);
            close (lFdOut);
            remove (acOut);
            
            return (NULL);
            }
        
        if ((lBytesRead == 0) && (ulSize != 0))
            {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_load_module: read the end of file.\n"
                         );
            close (lFdIn);
            close (lFdOut);
            remove (acOut);
            
            return (NULL);
            }

        if (write (lFdOut, acBuffer, 1) == ERROR)    
            {
            cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_load_module: write failed, errno = 0x%x.\n",
                         errno);
            close (lFdIn);
            close (lFdOut);
            remove (acOut);
            
            return (NULL);
            }

        ulSize--;
        
        if (ulSize == 0)
            {
            break;
            }
        }
    close (lFdIn);
    close (lFdOut);

    lFdOut = open (acOut, O_RDONLY, 0);
    if (lFdOut == ERROR)
        {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_load_module: open failed, errno = 0x%x.\n",
                     errno);
        return (NULL);
        }

    pcText = pcData = pcBss = LD_NO_ADDRESS;

    errno = 0;    
    moduleId = loadModuleAt (lFdOut, (LOAD_GLOBAL_SYMBOLS | LOAD_COMMON_MATCH_ALL), 
                             & pcText, & pcData, & pcBss);
    if ((moduleId == NULL) || (errno != 0))
        {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_load_module: loadModuleAt failed, errno = 0x%x.\n",
                     errno);
        close (lFdOut);
        remove (acOut);

        return (NULL);
        }
        
    close (lFdOut);
    remove (acOut);
    
    return (moduleId);
#endif 
	return  0;
    }

/*******************************************************************************
*函数名称:cpss_unload_module
*功能:卸载模块
*函数类型: 
*参数: 
*参数名称 类型      输入/输出 描述
*moduleId MODULE_ID 输入
*函数返回:CPSS_OK or CPSS_ERROR
*说明:                                 
*******************************************************************************/
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
  INT32 cpss_unload_module
    (
    MODULE_ID   moduleId /* module ID */
    )
#else
   INT32 cpss_unload_module
   (
    UINT32   moduleId /* module ID */
   )
#endif
    {
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
    extern int  errno;

    if (unldByModuleId (moduleId, 0) == ERROR)
        {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_unload_module: unldByModuleId failed, errno = 0x%x.\n",
                     errno);

        return (CPSS_ERROR);
        }
#endif
    return (CPSS_OK);
    }
    
/*******************************************************************************
*函数名称:cpss_lookup_func
*功能:查找函数的地址
*函数类型: 
*参数: 
*参数名称    类型   输入/输出 描述
*pucFuncName INT8 * 输入
*函数返回:函数地址
*说明:                                 
*******************************************************************************/
void * cpss_lookup_func
    (
    INT8 *    pucFuncName /* function name */
    )
    {
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
    INT8	acPrefixName [128];
    INT8	acValue [4];
    SYM_TYPE	type;
    void *  pAddress;
    extern int	errno;

    if (pucFuncName == NULL)
        {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_lookup_func: file name not specified.\n"
                     );
            
        return (NULL);
        }
    
    memset ((void *) (& acPrefixName[0]), '\0', 128);  
    sprintf (acPrefixName, "_%s", pucFuncName);

    memset ((void *) (& acValue[0]), 0, 4);  

    if ((symFindByName (sysSymTbl, pucFuncName, (INT8 **) (& acValue[0]), & type) == 
        ERROR) && (symFindByName (sysSymTbl, acPrefixName, (INT8 **) (& acValue[0]), 
        & type) == ERROR))
        {
        cpss_output (CPSS_MODULE_VK, CPSS_PRINT_WARN, "cpss_unload_module: symFindByName failed, errno = 0x%x.\n",
                     errno);
        return (NULL);
        }

    pAddress = (void *) (*((int *)(& acValue[0])));

    return (pAddress);
#endif
	return 0;
    }

/*******************************************************************************
*函数名称:cpss_apply_patch
*功能:应用补丁
*函数类型: 
*参数: 
*参数名称   类型     输入/输出 描述
*pucOldFunc void *   输入
*pucNewFunc void *   输入
*aucSaveBuf UINT8 *  输出
*函数返回:
*说明:                                 
*******************************************************************************/
void cpss_apply_patch
    (
    void *  pucOldFunc, /* old function address */
    void *  pucNewFunc, /* new function address */
    UINT8 * aucSaveBuf /* saved text */
    )
    {
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
    UINT8 *	pucText;
    INT32   address;
    UINT8	acText [128];

    pucText = (UINT8 *) pucOldFunc;
    address = (INT32) pucNewFunc;

#if ((CPU_FAMILY == I80X86) || (CPU_FAMILY == SIMNT))
    memcpy ((void *) aucSaveBuf, (void *) pucText, 7);
    memset ((void *) (& acText[0]), 0, 7);
      
    acText[0] = 0xb8;    
    acText[1] = (UINT8) (address & 0x000000ff);
    acText[2] = (UINT8) ((address >> 8) & 0x000000ff);
    acText[3] = (UINT8) ((address >> 16) & 0x000000ff);
    acText[4] = (UINT8) ((address >> 24) & 0x000000ff);
    acText[5] = 0xff;    
    acText[6] = 0xe0;    

    memcpy ((void *) pucText, (void *) (& acText[0]), 7);
#endif
        
#if (CPU_FAMILY == PPC)
    memcpy ((void *) aucSaveBuf, (void *) pucText, 16);

    memset ((void *) (& acText[0]), 0, 16);

    acText[0] = 0x3c;
    acText[1] = 0x00;
    acText[2] = (UINT8) ((address >> 24) & 0x000000ff);
    acText[3] = (UINT8) ((address >> 16) & 0x000000ff);
    acText[4] = 0x60;
    acText[5] = 0x00;
    acText[6] = (UINT8) ((address >> 8) & 0x000000ff);
    acText[7] = (UINT8) (address & 0x000000ff);
    acText[8] = 0x7c;
    acText[9] = 0x09;    
    acText[10] = 0x03;    
    acText[11] = 0xa6; 
    acText[12] = 0x4e;
    acText[13] = 0x80;    
    acText[14] = 0x04;    
    acText[15] = 0x20;    
    
    memcpy ((void *) pucText, (void *) (& acText[0]), 16);      
#endif

#if (CPU_FAMILY == ARM)
    memcpy ((void *) aucSaveBuf, (void *) pucText, 12);
    memset ((void *) (& acText[0]), 0, 12);
      
    acText[0] = 0xe5;
    acText[1] = 0x9f;
    acText[2] = 0x30;
    acText[3] = 0x00;
    acText[4] = 0xe1;    
    acText[5] = 0xa0;    
    acText[6] = 0xf0;    
    acText[7] = 0x03;
    acText[8] = (UINT8) ((address >> 24) & 0x000000ff);   
    acText[9] = (UINT8) ((address >> 16) & 0x000000ff);
    acText[10] = (UINT8) ((address >> 8) & 0x000000ff);
    acText[11] = (UINT8) (address & 0x000000ff);

    memcpy ((void *) pucText, (void *) (& acText[0]), 12);
#endif
#endif
    }
    
/*******************************************************************************
*函数名称:cpss_undo_patch
*功能:取消补丁
*函数类型: 
*参数: 
*参数名称   类型     输入/输出 描述
*pucOldFunc void *   输入
*aucSaveBuf UINT8 *  输入
*函数返回:
*说明:                                 
*******************************************************************************/
void cpss_undo_patch
    (
    void *  pucOldFunc, /* old function address */
    UINT8 * aucSaveBuf /* saved text */
    )
    {
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
    UINT8 *	pucText;

    pucText = (UINT8 *) pucOldFunc;

#if ((CPU_FAMILY == I80X86) || (CPU_FAMILY == SIMNT))
    memcpy ((void *) pucText, (void *) aucSaveBuf, 7);
#endif
        
#if (CPU_FAMILY == PPC)
    memcpy ((void *) pucText, (void *) aucSaveBuf, 16);
#endif

#if (CPU_FAMILY == ARM)
    memcpy ((void *) pucText, (void *) aucSaveBuf, 12);
#endif
#endif
    }


#ifdef CPSS_VOS_VXWORKS
/*******************************************************************************
*函数名称:    cpss_proc_msg_show
*功能:        显示所有在Shed和Proc中存储的消息个数
*参数名称   类型     输入/输出 描述
*   无
*函数返回:
*说明:                                 
*******************************************************************************/
VOID cpss_proc_msg_show()
{
    VK_SCHED_ID            tSchedId ;
    CPSS_VOS_TASK_INFO_T   stTaskInfo;
    VK_PROC_ID             pstProcId ;
    UINT32  ulLoop ;
    
    /*显示所有Shed中存储的消息*/
 
    printf("-----all the shed ipc msg num :-----\n");
 
    for(ulLoop=0;ulLoop<VK_MAX_SCHED_NUM;ulLoop++)
    {
        tSchedId = g_astCpssVkSchedDescTbl[ulLoop].tSchedId;
 
        if(tSchedId == NULL)
        {
            continue ;
        }
 
        /*得到任务名称*/
        cpss_vos_task_info_get(tSchedId->ulTaskDesc, &stTaskInfo) ;
       
        printf("schedTaskId=%d, schedName=%s, MailNum=%d\n",
            tSchedId->ulTaskDesc,stTaskInfo.pcName,tSchedId->lMailNum) ;
    }
 
    /*显示所有Proc中存储的消息*/
    printf("-----all the proc ipc msg num :-----\n");
 
    for(ulLoop=0;ulLoop<VK_MAX_PROC_INST_NUM;ulLoop++)
    {
        if(g_astCpssVkProcDescTbl[ulLoop].bInuse == FALSE)
        {
            continue ;
        }
 
        pstProcId = g_astCpssVkProcDescTbl[ulLoop].tProcId ;
 
        printf("procName=%d,mailNum=%d\n",pstProcId->acName,pstProcId->lMailNum) ;
    }
 
    printf("----- end -----\n");
 
    printf("\n") ;
    printf("\n") ;
 
}
#endif

