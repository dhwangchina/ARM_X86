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
#include "cpss_com_frag.h"
#include "cpss_dbg_diagn.h"
/******************************* 局部宏定义 ***********************************/
#define CPSS_DBG_SEND_DIAGN_BEGSTR   ("Cpss Test")
#define CPSS_DBG_DIAGN_BEGSTR_SIZE   (10)
#define CPSS_DBG_SEND_DIAGN_ENDSTR   ("Data!")
#define CPSS_DBG_DIAGN_ENDSTR_SIZE   (6)
#define CPSS_DBG_SEND_DIAGN_MIDSTR   ("Hello Jason!")
#define CPSS_DBG_DIAGN_MIDSTR_SIZE   (16)
#define CPSS_DBG_DIAGN_MIDSTR_OFFSET (COM_FRAG_FRAME_SIZE - 3)
#define CPSS_DSP_MAX_MEM_SIZE           0x3200000

/******************************* 全局变量定义/初始化 **************************/
extern INT32 cpss_vos_task_show 
(
UINT32  ulTaskDesc, /* 任务描述符 */
INT32   lLevel /* 0 = 概况, 2 = 所有任务 */
);
extern INT32 cpss_vos_sem_show 
(
UINT32  ulSemDesc, /* semaphore descriptor */
INT32   lLevel /* 0 = 概况, 2 = 所有信号量 */
);
extern INT32 cpss_vos_msg_q_show 
(
UINT32  ulMsgQDesc, /*消息队列描述符 */
INT32   lLevel /* 0 = 概况, 2 = 所有消息队列 */    
);
extern INT32 cpss_vk_sched_show 
(
UINT32  ulSchedDesc, /* 调度器描述符 */
INT32   lLevel /* 0 = 概况, 1 = 详细 */
);
extern STATUS cpss_vk_proc_show 
(
UINT16  usGuid, /* GUID */
UINT32  ulProcDesc, /* 纤程描述符 */
INT32   lLevel /* 
               * 0 = 纤程类概况, 1 = 纤程类详细, 
               * 2 = 纤程实例概况, 3 = 纤程实例详细
               */
);
extern INT32 cpss_dir_used_space_get_extend
(
 const STRING szDirName,
 UINT32 *pulSizeHigh,
 UINT32 *pulSizeLow,
 UINT32 *pulFileNum
);
extern INT32 cpss_timer_get_gmtoffset(VOID);
extern VOID cpss_mem_usage_get(UINT32 *pulCount, CPSS_DBG_MEM_POOL_INFO_T *pstInfo);

/* 显示内存池信息*/
extern INT32  cpss_mem_show( );
/* 显示指定时间段内内存使用者的信息 */
extern   INT32 cpss_mem_show_leak
     (
     UINT32 ulPrcId,
     UINT32 ulStartTime, 
     UINT32 ulEndTime
     );
/* 显示指定内存池的使用者信息*/
/*extern  INT32 cpss_mem_show_pool(UINT32 ulPoolNo);*/
/* 显示指定地址的内容*/
extern  INT32 cpss_mem_show_buf(const VOID *pvtr,UINT16 usLen);
/*显示所有定时器信息*/
extern INT32 cpss_timer_show(VOID);
/* 显示指定纤程的定时器信息*/
extern INT32 cpss_timer_show_proc(UINT32 ulPno);
/* 显示指定定时器描述符的信息*/
extern INT32 cpss_timer_show_id(UINT32 ulTid);
/* 显示无参定时器信息*/
extern INT32 cpss_timer_show_no_para_timer(UINT32 ulPno, UINT8    ucTimerNo);

extern INT32 cpss_com_link_show();
extern INT32 cpss_com_route_show();
extern INT32 cpss_com_store_show();
extern INT32 cpss_com_pci_link_show();
extern INT32 cpss_com_init_show();
extern INT32 cpss_com_arp_show();
extern INT32 cpss_com_data_show();
extern INT32 cpss_com_constant_addr_show();
extern INT32 cpss_com_call_cps__rdbs_err_show();
extern INT32 cpss_com_switch_info_show();
extern INT32 cpss_pci_drv_info_show();
extern INT32 cpss_com_interface_send_show();
extern INT32 cpss_udp_drv_stat_show(UINT32 ulIp);
extern INT32 cpss_com_tcp_link_show();

extern INT32 cpss_msg_trace_start(UINT32 ulMsgId,UINT32 ulAddr,
                                  UINT32 ulAddrFlag,UINT32 ulTraceNum);
INT32 cpss_msg_trace_stop();
void cpss_dbg_simi();
void cpss_dbg_simtt(INT32 TaskID);
extern void cpss_dbg_pro_time_pause();
extern void cpss_dbg_pro_time_resume();
extern void cpss_dbg_pro_time_start(UINT32 ulRecSum, UINT32 ulLongTick);
extern void cpss_dbg_pro_time_end();
extern void cpss_dbg_pro_time_show(UINT8 *pProcName, UINT32 ulMinTick, UINT32 ulMinStartTick, UINT32 ulMaxStartTick);
extern void cpss_dbg_pro_time_show_long();
extern void cpss_dsp_sbbr_switch_set(INT32 flag);
extern VOID cpss_log_show_save(VOID);
/* 内存统计初始化开始*/
extern VOID cpss_mem_show_proc();
extern VOID cpss_mem_proc_record_init();
extern BOOL g_bCpssProcStartActive;
extern void cpss_mem_stat_show();
extern void cpss_vk_ipc_send_error_show();
/* 内存统计初始化结束*/
extern INT32 cpss_mem_occupy_get(UINT32 *pulFreeMem,UINT32 *pulUsedMem);
extern void cpss_mem_record_show();
extern void cpss_mem_record_reset();
extern INT32  cpss_spy(UINT32 ulInterval);
extern VOID cpss_spy_stop();
extern VOID cpss_enable_sbbr(VOID);
extern VOID cpss_disable_sbbr(VOID);
extern INT32 cpss_com_spec_msg_stat_add(UINT32 ulMsgId);
extern VOID cpss_com_spec_msg_stat_clear();
extern VOID cpss_com_spec_msg_stat_show();

/******************************* 局部常数和类型定义 ***************************/
typedef struct tagCPSS_DBG_SIM_SHEEL_REG_T
{
   CHAR aucCmdName[48];
   CHAR aucCmdHelp[48];
   CHAR aucArgFmt[5];
   CPS__OAMS_SHCMD_PF  pfCmdFunc;
}CPSS_DBG_SIM_SHELL_REG_TBL_T;

CPSS_DBG_SIM_SHELL_REG_TBL_T g_astCpssDbgShellRegTbl[]=
{
 /*  cmd name                            cmd help info                    argfmt                                pfCmdFunc                    */
 {"cpss_vos_task_show",                  "",                               "ui",                               (CPS__OAMS_SHCMD_PF)cpss_vos_task_show},             
 {"cpss_vos_sem_show",                   "",                               "ui",                               (CPS__OAMS_SHCMD_PF)cpss_vos_sem_show},             
 {"cpss_vos_msg_q_show",                 "",                               "ui",                               (CPS__OAMS_SHCMD_PF)cpss_vos_msg_q_show},             
 {"cpss_vk_sched_show",                  "",                               "ui",                               (CPS__OAMS_SHCMD_PF)cpss_vk_sched_show},             
 {"cpss_vk_proc_show",                   "",                               "uui",                              (CPS__OAMS_SHCMD_PF)cpss_vk_proc_show},             
 {"cpss_mem_show",                       "",                               "",                                 (CPS__OAMS_SHCMD_PF)cpss_mem_show},    
 {"cpss_timer_show",                     "",                               "",                                 (CPS__OAMS_SHCMD_PF)cpss_timer_show},  
 {"cpss_timer_show_proc",                "",                               "u",                                (CPS__OAMS_SHCMD_PF)cpss_timer_show_proc},  
 {"cpss_timer_show_id",                  "",                               "u",                                (CPS__OAMS_SHCMD_PF)cpss_timer_show_id},  
 {"cpss_timer_show_no_para_timer",       "",                               "uu",                               (CPS__OAMS_SHCMD_PF)cpss_timer_show_no_para_timer},  
 {"cpss_com_link_show",                       "",                               "",                            (CPS__OAMS_SHCMD_PF)cpss_com_link_show},    
 {"cpss_com_route_show",                       "",                               "",                           (CPS__OAMS_SHCMD_PF)cpss_com_route_show},    
 {"cpss_com_store_show",                       "",                               "",                           (CPS__OAMS_SHCMD_PF)cpss_com_store_show},    
 {"cpss_com_pci_link_show",                       "",                               "",                        (CPS__OAMS_SHCMD_PF)cpss_com_pci_link_show},    
 {"cpss_com_init_show",                       "",                               "",                            (CPS__OAMS_SHCMD_PF)cpss_com_init_show},    
 {"cpss_com_arp_show",                       "",                               "",                             (CPS__OAMS_SHCMD_PF)cpss_com_arp_show},    
 {"cpss_com_data_show",                       "",                               "",                            (CPS__OAMS_SHCMD_PF)cpss_com_data_show},   
 {"cpss_com_constant_addr_show",                       "",                               "",                   (CPS__OAMS_SHCMD_PF)cpss_com_constant_addr_show},    
 {"cpss_com_call_cps__rdbs_err_show",                       "",                               "",                   (CPS__OAMS_SHCMD_PF)cpss_com_call_cps__rdbs_err_show},     
 {"cpss_com_switch_info_show",                       "",                               "",                     (CPS__OAMS_SHCMD_PF)cpss_com_switch_info_show},    
 {"cpss_pci_drv_info_show",                       "",                               "",                        (CPS__OAMS_SHCMD_PF)cpss_pci_drv_info_show},
 {"cpss_com_interface_send_show",                       "",                               "",                  (CPS__OAMS_SHCMD_PF)cpss_com_interface_send_show},    
 {"cpss_com_tcp_link_show",                       "",                               "",                        (CPS__OAMS_SHCMD_PF)cpss_com_tcp_link_show},    
#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
 {"cpss_msg_trace_start",                       "",                               "uuuu",                     (CPS__OAMS_SHCMD_PF)cpss_msg_trace_start},
 {"cpss_msg_trace_stop",                       "",                               "",                           (CPS__OAMS_SHCMD_PF)cpss_msg_trace_stop},        
#endif
 {"i",                                         "",                               "",                           (CPS__OAMS_SHCMD_PF)cpss_dbg_simi},        
 {"tt",                                        "",                               "i",                           (CPS__OAMS_SHCMD_PF)cpss_dbg_simtt},        
 {"cpss_dbg_pro_time_pause",                       "",                               "",                       (CPS__OAMS_SHCMD_PF)cpss_dbg_pro_time_pause},    
 {"cpss_dbg_pro_time_resume",                       "",                               "",                      (CPS__OAMS_SHCMD_PF)cpss_dbg_pro_time_resume},    
 {"cpss_dbg_pro_time_start",                       "",                               "uu",                      (CPS__OAMS_SHCMD_PF)cpss_dbg_pro_time_start},    
 {"cpss_dbg_pro_time_end",                       "",                               "",                         (CPS__OAMS_SHCMD_PF)cpss_dbg_pro_time_end},    
 {"cpss_dbg_pro_time_show",                       "",                               "cuuu",                     (CPS__OAMS_SHCMD_PF)cpss_dbg_pro_time_show},    
 {"cpss_dbg_pro_time_show_long",                       "",                               "",                     (CPS__OAMS_SHCMD_PF)cpss_dbg_pro_time_show_long},    
#if defined CPSS_DBG_REC_TIME_ON

 {"cpss_dbg_rec_time_delete",                       "",                               "u",                     (CPS__OAMS_SHCMD_PF)cpss_dbg_rec_time_delete},    
 {"cpss_dbg_rec_time_delete_all",                       "",                               "",                     (CPS__OAMS_SHCMD_PF)cpss_dbg_rec_time_delete_all},    
 {"cpss_dbg_rec_time_add",                       "",                               "u",                     (CPS__OAMS_SHCMD_PF)cpss_dbg_rec_time_add},    
 {"cpss_dbg_rec_time_add_all",                       "",                               "",                     (CPS__OAMS_SHCMD_PF)cpss_dbg_rec_time_add_all},    
 {"cpss_dbg_rec_time_task_show",                       "",                               "",                     (CPS__OAMS_SHCMD_PF)cpss_dbg_rec_time_task_show},    
 {"cpss_dbg_rec_time_show",                       "",                               "uuuu",                     (CPS__OAMS_SHCMD_PF)cpss_dbg_rec_time_show},    

#endif
{"cpss_dsp_sbbr_switch_set",                       "",                               "i",                     (CPS__OAMS_SHCMD_PF)cpss_dsp_sbbr_switch_set},    
{"cpss_log_show_save",                       "",                               "",                     (CPS__OAMS_SHCMD_PF)cpss_log_show_save},    
{"cpss_vk_ipc_send_error_show",              "",                               "",                     (CPS__OAMS_SHCMD_PF)cpss_vk_ipc_send_error_show},    
{"cpss_spy",                                           "",                               "u",                     (CPS__OAMS_SHCMD_PF)cpss_spy},    
{"cpss_spy_stop",                                    "",                               "",                     (CPS__OAMS_SHCMD_PF)cpss_spy_stop},    
{"cpss_enable_sbbr",                               "",                               "",                     (CPS__OAMS_SHCMD_PF)cpss_enable_sbbr},    
{"cpss_disable_sbbr",                               "",                           "",                     (CPS__OAMS_SHCMD_PF)cpss_disable_sbbr},    
{"cpss_com_spec_msg_stat_add",              "",                           "u",                     (CPS__OAMS_SHCMD_PF)cpss_com_spec_msg_stat_add},    
{"cpss_com_spec_msg_stat_clear",            "",                           "",                     (CPS__OAMS_SHCMD_PF)cpss_com_spec_msg_stat_clear},    
{"cpss_com_spec_msg_stat_show",           "",                           "",                     (CPS__OAMS_SHCMD_PF)cpss_com_spec_msg_stat_show},    

};


UINT32   g_ulCpssDbgShellConfigNum= NUM_ENTS (g_astCpssDbgShellRegTbl);
/* CPU 性能统计开始*/
CPS__OAMS_PM_TMVAR_TO_CNTGRP_MAP_T g_astCpssTmvGrpToCntGrpMapTbl[]
= {
    { CPS__OAMS_PM_TMVAR_TO_CNTGRP_CALC_AVG,  0 },
    { CPS__OAMS_PM_TMVAR_TO_CNTGRP_CALC_MAX,  0 },

    { CPS__OAMS_PM_TMVAR_TO_CNTGRP_CALC_INVALID, 0 }  /* 映射表结束标记 */
};

/* 时变量组结构*/
typedef struct
{
    CPS__OAMS_MO_ID_T  stMoId;        /* 测量对象标识 */
    UINT32  ulStartTm;           /* 测量开始时间(GMT秒计数), */
                                    /* 取值CPS__OAMS_PM_TIME_INVALID表示本记录无效 */
    UINT32  ulReserved;          /* 保留 */
    UINT32  ulTmVarValue0;       /* 时变量0的数值 */
  
}APP_TMVGRP_CPSS_T;

APP_TMVGRP_CPSS_T  g_stAppTmvGrpCpssArray;

/* CPU 性能统计结束*/
#if 0
/* 内存池信息结构*/
typedef struct tagCPSS_MEM_POOL_INFO
{
   UINT32 ulPoolNo;                         /* 内存池编号*/
   UINT32 ulSize;                           /* 内存块规格*/
   UINT32 ulTotal;                          /* 内存块总数*/
   UINT32 ulFree;                           /* 空闲内存块数目*/
   UINT32 ulPeak;                           /* 峰值 */
   UINT32 ulMallocSuccTimes;                /* 申请成功次数*/
   UINT32 ulMallocFailTimes;                /* 申请失败次数*/
   UINT32 ulFreeSuccTimes;                  /* 释放成功次数*/

}CPSS_DBG_MEM_POOL_INFO_T;

/* 诊断请求消息,来自ldt */
typedef struct tagCPS__OAMS_O_CPSS_DIAG_REQ
{
  CPS__OAMS_O_REQ_HDR_T  stReqHeader;             /* 消息头*/
  UINT32            ulTestCode;              /* 测试码*/
  CPS__OAMS_MO_ID_T      stTargetMoId;            /* 管理对象标识*/
  UINT32            usTestParaLen;           /* 测试参数长度*/
  UINT8             *pucTestPara;            /* 测试参数*/
  
}CPS__OAMS_O_CPSS_DIAG_REQ_T;


/* cpu 占用率结构*/
typedef struct tagCPSS_DBG_CPU_USAGE
{
   UINT32 ulUsage;                           /*占用率*/
}CPSS_DBG_CPU_USAGE_T;


/*cpu 诊断测试响应消息*/
typedef struct tagCPSS_CPU_CPS__OAMS_O_DIAG_RSP
{
  CPS__OAMS_M_REQ_HDR_T tReqHeader;    /* 消息头*/ 
  UINT32        ulTestId;    /* 测试活动ID */
  UINT32        ulRptType;    /* 测试报告类型(中间、最终) */
  UINT32        ulTestResult; /* 测试结果(成功、失败) */ 
  UINT32        ulTestRptLen; /* 报告长度，最大为1024字节 */
  UINT8          aucRptInfo[sizeof(CPSS_DBG_CPU_USAGE_T)]; /* 报告信息 */
  
}CPSS_DBG_CPU_CPS__OAMS_O_DIAG_RSP_T;


/* 内存诊断测试响应消息*/
typedef struct tagCPSS_MEM_CPS__OAMS_O_DIAG_RSP
{
  CPS__OAMS_M_REQ_HDR_T tReqHeader;    /* 消息头*/ 
  UINT32        ulTestId;    /* 测试活动ID */
  UINT32        ulRptType;    /* 测试报告类型(中间、最终) */
  UINT32        ulTestResult; /* 测试结果(成功、失败) */ 
  UINT32        ulTestRptLen; /* 报告长度，最大为1024字节 */
  UINT8          aucRptInfo[sizeof(UINT32)+ MAX_POOL_NUM * sizeof(CPSS_DBG_MEM_POOL_INFO_T)]; /* 报告信息 */
   
}CPSS_DBG_MEM_CPS__OAMS_O_DIAG_RSP_T;


/*内存池信息结构*/
typedef struct tagCPSS_MEM_RSP_INFO
{
   UINT32 ulMemPoolCnt;                    /*内存池数量*/
   CPSS_DBG_MEM_POOL_INFO_T astMemPools[MAX_POOL_NUM];   /* 内存池信息*/
}CPSS_DBG_MEM_RSP_INFO_T;

typedef struct tagCPSS_CLOCK_INFO
{
  UINT32 ulMode;                  /* 0 Gmt ; 1 tick ;2 local time */
  UINT32 ulGmtValue;           /*  0 valid */
  UINT32 ulTickValue;           /* 1 valid */
  CPSS_TIME_T stLocalTime;  /* 2 valid */

}CPSS_DBG_CLOCK_INFO_T;

typedef struct tagCPSS_CLOCK_RSP_T
{
    CPS__OAMS_M_REQ_HDR_T tReqHeader;    /* 消息头*/ 
    UINT32        ulTestId;    /* 测试活动ID */
    UINT32        ulRptType;    /* 测试报告类型(中间、最终) */
    UINT32        ulTestResult; /* 测试结果(成功、失败) */ 
    UINT32        ulTestRptLen; /* 报告长度，最大为1024字节 */
    UINT8          aucRptInfo[sizeof(CPSS_DBG_CLOCK_INFO_T)]; /* 报告信息 */
 
}CPSS_DBG_CLOCK_RSP_INFO_T;
#endif
/**/
/******************************* 局部函数原型声明 *****************************/

/******************************* 函数实现 *************************************/
/*******************************************************************************
* 函数名称: cpss_dbg_send_test_buf_fill
* 功    能: 通信测试数据包的填充函数：所有测试函数填充的测试消息内容固定，这些测试
*           只测试功能，所以可以只填充固定信息。但是对一些特殊位置进行了校验：如拆
*           包的会在第一包和第二包的交界处填充一个字符串，接收后校验组包后的字符串
*           和发送的是否一致；另外，在包的头和尾巴都填充字符串，接收时观察是否一致
* 函数类型: 
* 参    数: 
* 参数名称      类型        输入/输出           描述
*  pucBuf      UINT8*         输入           缓冲区指针
*  ulLen       UINT32         输入           缓冲区长度
*
* 函数返回: 初始化成功返回CPSS_OK, 失败返回CPSS_ERROR.
* 说    明: 
*******************************************************************************/
/*#ifndef CPSS_DSP_CPU*/
VOID cpss_dbg_send_test_buf_fill(UINT8 *pucBuf, UINT32 ulLen)
{
    if (ulLen > (CPSS_DBG_DIAGN_BEGSTR_SIZE + CPSS_DBG_DIAGN_ENDSTR_SIZE))
    {
        cpss_mem_memcpy(pucBuf, CPSS_DBG_SEND_DIAGN_BEGSTR, CPSS_DBG_DIAGN_BEGSTR_SIZE);
        cpss_mem_memcpy(pucBuf + ulLen - 1 - 5, CPSS_DBG_SEND_DIAGN_ENDSTR, CPSS_DBG_DIAGN_ENDSTR_SIZE);
    }
    else
    {
        UINT32 ulC;
        for (ulC = 0; ulC < ulLen; ulC++)
        {
            pucBuf[ulC] = ulC;
        }
    }

    if (ulLen > (COM_FRAG_FRAME_SIZE + 10))
    {
        cpss_mem_memcpy(pucBuf + CPSS_DBG_DIAGN_MIDSTR_OFFSET, CPSS_DBG_SEND_DIAGN_MIDSTR, CPSS_DBG_DIAGN_MIDSTR_SIZE);
    }

    return ;
}
/*******************************************************************************
* 函数名称: cpss_dbg_com_send_test
* 功    能: 通信的cpss_com_send()函数的测试接口
* 函数类型: 
* 参    数: 
* 参数名称      类型        输入/输出           描述
*  ulModule    UINT32         输入           对端逻辑地址模组类型
*  ulGroup     UINT32         输入           对端逻辑地址的GROUP
*  ulSubGroup  UINT32         输入           对端逻辑地址的subGROUP
*  ulAddrFlag  UINT32         输入           对端的主备标记
*  ulLen       UINT32         输入           待发送的数据长度
*
* 函数返回: 初始化成功返回CPSS_OK, 失败返回CPSS_ERROR.
* 说    明: 
*******************************************************************************/
VOID cpss_dbg_com_send_test(UINT32 ulModule, UINT32 ulGroup, UINT32 ulSubGroup, UINT32 ulAddrFlag, UINT32 ulPd, UINT32 ulLen)
{
    CPSS_COM_PID_T stDstPid; 
    INT32 lRet;
    UINT8 *pucBuf = cpss_mem_malloc(ulLen);
    
    if (NULL == pucBuf)
    {
        printf("\n Malloc size failed! \n");

        return ;
    }

    ulPd = (ulPd << 16) & 0xffff0000;
    
    cpss_dbg_send_test_buf_fill(pucBuf, ulLen);

    stDstPid.stLogicAddr.ucModule = (UINT8)ulModule;
    stDstPid.stLogicAddr.ucSubGroup = (UINT8)ulSubGroup;
    stDstPid.stLogicAddr.usGroup = (UINT16)ulGroup;

/*    stDstPid.stLogicAddr = (CPSS_COM_LOGIC_ADDR_T)(*pulLogAddr);*/
    stDstPid.ulAddrFlag  = ulAddrFlag;
    stDstPid.ulPd        = ulPd;        
    
 #ifdef CPSS_VOS_VXWORKS
    printf(" cpss_com_send_test send msg @ tick %d\n\r", tickGet());
#endif
    lRet = cpss_com_send(&stDstPid, CPSS_DBG_DIAGN_MSGID, pucBuf, ulLen);
    if(CPSS_OK != lRet)            
    {
        printf("CPSS_COM: Send test data Error!!\n");
    }
    else
    {
        printf("CPSS_COM: Send test data OK!!!\n");
    }
    
    cpss_mem_free(pucBuf);

    return ;

}

/*******************************************************************************
* 函数名称: cpss_dbg_send_mate_test
* 功    能: 通信的cpss_send_mate()函数的测试接口
* 函数类型: 
* 参    数: 
* 参数名称      类型        输入/输出           描述
*  ulPd        UINT32         输入           目的接收纤程描述符
*  ulLen       UINT32         输入           待发送的数据长度
*
* 函数返回: 初始化成功返回CPSS_OK, 失败返回CPSS_ERROR.
* 说    明: 
*******************************************************************************/
VOID cpss_dbg_send_mate_test(UINT32 ulPd, UINT32 ulLen)
{
    INT32 lRet;
    UINT8 *pucBuf = cpss_mem_malloc(ulLen);

    ulPd = (ulPd << 16) & 0xffff0000;
    cpss_dbg_send_test_buf_fill(pucBuf, ulLen);

    lRet = cpss_com_send_mate(ulPd, CPSS_DBG_DIAGN_MSGID, pucBuf, ulLen);
    if(CPSS_OK != lRet)
    {
        printf("CPSS_COM: Send test data Error!!\n");
    }
    else
    {
        printf("CPSS_COM: Send test data OK!!!\n");
    }
    
    cpss_mem_free(pucBuf);

    return ;
}

/*******************************************************************************
* 函数名称: cpss_dbg_send_phy_test
* 功    能: 通信的cpss_send_phy()函数的测试接口
* 函数类型: 
* 参    数: 
* 参数名称      类型        输入/输出           描述
*  ulPd        UINT32         输入           目的接收纤程描述符
*  ulLen       UINT32         输入           待发送的数据长度
*
* 函数返回: 初始化成功返回CPSS_OK, 失败返回CPSS_ERROR.
* 说    明: 
*******************************************************************************/
VOID cpss_dbg_send_phy_test(UINT32 ulFrame, UINT32 ulShelf, UINT32 ulSlot, UINT32 ulCpu, UINT32 ulPd, UINT32 ulLen)
{
    INT32 lRet;
    UINT8 *pucBuf = cpss_mem_malloc(ulLen);
    CPSS_COM_PHY_ADDR_T  stDstPhyAddr;

    ulPd = (ulPd << 16) & 0xffff0000;
    cpss_dbg_send_test_buf_fill(pucBuf, ulLen);

    stDstPhyAddr.ucFrame = (UINT8)ulFrame;
    stDstPhyAddr.ucShelf = (UINT8)ulShelf;
    stDstPhyAddr.ucSlot = (UINT8)ulSlot;
    stDstPhyAddr.ucCpu = (UINT8)ulCpu;

   #ifdef CPSS_VOS_VXWORKS
    printf(" cpss_com_send_phy_test send msg @ tick %d\n\r", tickGet());
  #endif
		
    lRet = cpss_com_send_phy(stDstPhyAddr, ulPd, CPSS_DBG_DIAGN_MSGID, pucBuf, ulLen);
    if(CPSS_OK != lRet)
    {
        printf("CPSS_COM: Send test data Error!!\n");
    }
    else
    {
        printf("CPSS_COM: Send test data OK!!!\n");
    }
    
    cpss_mem_free(pucBuf);

    return ;
}

/*******************************************************************************
* 函数名称: cpss_dbg_send_tcp_test
* 功    能: 通信的cpss_send_tcp()函数的测试接口
* 函数类型: 
* 参    数: 
* 参数名称      类型        输入/输出           描述
*  ulPd        UINT32         输入           目的接收纤程描述符
*  ulLen       UINT32         输入           待发送的数据长度
*
* 函数返回: 初始化成功返回CPSS_OK, 失败返回CPSS_ERROR.
* 说    明: 
*******************************************************************************/
VOID cpss_dbg_send_tcp_test(UINT32 ulIp, UINT32 ulPort, UINT32 ulModule, UINT32 ulGroup, UINT32 ulSubGroup, UINT32 ulLen)
{
#ifdef CPSS_TCP_INCLUDE
    INT32 lRet;
    UINT8 *pucBuf = cpss_mem_malloc(ulLen);
    CPSS_COM_TCP_LINK_T stTcpLink;
    CPSS_COM_MSG_HEAD_T stMsg;
    
    cpss_dbg_send_test_buf_fill(pucBuf, ulLen);

    stTcpLink.ulIP = ulIp;
    stTcpLink.usTcpPort = ulPort;
    stTcpLink.usReserved = 0;

    stMsg.pucBuf = pucBuf;
    stMsg.stDstProc.stLogicAddr.ucModule = (UINT8)ulModule;
    stMsg.stDstProc.stLogicAddr.ucSubGroup = (UINT8)ulSubGroup;
    stMsg.stDstProc.stLogicAddr.usGroup = (UINT8)ulGroup;
    stMsg.stDstProc.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER;
    stMsg.stDstProc.ulPd = 10;
    stMsg.ucAckFlag = CPSS_COM_NOT_ACK;
    stMsg.ucPriFlag = 0;
    stMsg.ucShareFlag = 0;
    stMsg.ulLen = ulLen;
    stMsg.ulMsgId = CPSS_DBG_DIAGN_MSGID;
    stMsg.usReserved = 0;
    
    lRet = cpss_com_tcp_link_send(&stTcpLink, &stMsg);
    if(CPSS_OK != lRet)
    {
        printf("CPSS_COM: Send test data Error!!\n");
    }
    else
    {
        printf("CPSS_COM: Send test data OK!!!\n");
    }
    
    cpss_mem_free(pucBuf);
#endif
    return ;
}
/*******************************************************************************
* 函数名称: cpss_dbg_send
* 功    能: 通信的 cpss_dbg_send() 函数的测试接口
* 函数类型: 
* 参    数: 
* 参数名称      类型        输入/输出           描述
*  ulPd        UINT32         输入           目的接收纤程描述符
*  ulLen       UINT32         输入           待发送的数据长度
*
* 函数返回: 初始化成功返回CPSS_OK, 失败返回CPSS_ERROR.
* 说    明: 
*******************************************************************************/
VOID cpss_dbg_send(UINT32 ulType, UINT32 ulP1, UINT32 ulP2, UINT32 ulP3, UINT32 ulP4, UINT32 ulP5, UINT32 ulP6)
{
    switch (ulType)
    {
    case 0:
        cpss_dbg_com_send_test(ulP1, ulP2, ulP3, ulP4, ulP5, ulP6);
        break;
    case 1:
        cpss_dbg_send_mate_test(ulP1, ulP2);
        break;
    case 2:
        cpss_dbg_send_phy_test(ulP1, ulP2, ulP3, ulP4, ulP5, ulP6);
        break;
    case 3:
        cpss_dbg_send_tcp_test(ulP1, ulP2, ulP3, ulP4, ulP5, ulP6);
        break;
    default:
        printf ("\n Now, I am Not support this type's send! \n");
        break;
    }

    return ;
}

/*******************************************************************************
* 函数名称: cpss_dbg_test_data_check
* 功    能: 对接收到的数据进行校验：由于发送的数据填充信息格式固定，内容固定，
*           因此校验时只要分析填充格式即可，然后该格式比较内容。
* 函数类型: 
* 参    数: 
* 参数名称      类型        输入/输出           描述
*  ulPd        UINT32         输入           目的接收纤程描述符
*  ulLen       UINT32         输入           待发送的数据长度
*
* 函数返回: 初始化成功返回CPSS_OK, 失败返回CPSS_ERROR.
* 说    明: 
*******************************************************************************/
INT32 cpss_dbg_test_data_check(UINT8 *pucBuf, UINT32 ulLen)
{
    if (ulLen < (CPSS_DBG_DIAGN_BEGSTR_SIZE + CPSS_DBG_DIAGN_ENDSTR_SIZE))
    {
        UINT32 ulC;
        for (ulC = 0; ulC < ulLen; ulC++)
        {
            if (pucBuf[ulC] != ulC)
            {
                break;
            }
        }
        if (ulC != ulLen)
        {
            printf ("\n Size(%d) test data recv, and Check Failed! \n", ulLen);
        }
        else
        {
            printf ("\n Recv test Data, And Check OK! \n");
            return (CPSS_OK);
        }
    }
    else
    {
        INT32 lRet;

        lRet = cpss_mem_memcmp(pucBuf, CPSS_DBG_SEND_DIAGN_BEGSTR, CPSS_DBG_DIAGN_BEGSTR_SIZE);
        if (0 == lRet)
        {
            lRet = cpss_mem_memcmp(pucBuf + ulLen - 1 - 5, CPSS_DBG_SEND_DIAGN_ENDSTR, CPSS_DBG_DIAGN_ENDSTR_SIZE);
            if (0 == lRet)
            {
                if (ulLen > (COM_FRAG_FRAME_SIZE + 10))
                {
                    lRet = cpss_mem_memcmp(pucBuf + CPSS_DBG_DIAGN_MIDSTR_OFFSET, CPSS_DBG_SEND_DIAGN_MIDSTR, CPSS_DBG_DIAGN_MIDSTR_SIZE);
                    if (0 == lRet)
                    {
                        printf ("\n Recv test Data, And Check OK! \n");
                        return (CPSS_OK);
                    }
                }
                else
                {
                    printf ("\n Recv test Data, And Check OK! \n");
                    return (CPSS_OK);
                }
            }
            else
            {
                printf ("\n Size(%d) test data recv, and Check Failed! \n", ulLen);
            }
        }
        else
        {
            printf ("\n Size(%d) test data recv, and Check Failed! \n", ulLen);
        }
    }

    return (CPSS_ERROR);
}

/*#endif*/
/*******************************************************************************
* 函数名称: cpss_dbg_cps__oams_diag_req_msg_proc
* 功    能: 诊断测试消息处理流程 
* 函数类型: 
* 参    数: 
* 参数名称          类型                     输入/输出         描述
* pstMsgHead       CPSS_COM_MSG_HEAD_T *      输入            消息头指针
* 
* 函数返回: 无。
* 说    明: 
*******************************************************************************/

#ifdef CPSS_DSP_CPU
INT32 cpss_dsp_cpu_get(UINT32 *pulUsage)
{
    UINT32 ulCpuLoad = 0;

    ulCpuLoad = cpss_ntohl(*(UINT32*)(CPSS_DSP_CPU_LOAD_ADDR));

    if(ulCpuLoad > 127)
    {
        *pulUsage = 0;
        return CPSS_ERROR;
    }
    ulCpuLoad = CPSS_DIV((ulCpuLoad*100),127);
    *pulUsage = ulCpuLoad;
    return CPSS_OK;
}
#endif


VOID cpss_dbg_send_diag_to_ftp(CPSS_COM_MSG_HEAD_T *pstMsgHead)
{

       CPSS_COM_MSG_HEAD_T stToFtpMsg;
       CHAR  aucPara[sizeof(CPS__OAMS_DIAG_START_TEST_REQ_MSG_T)]={0};

       stToFtpMsg.stSrcProc   = pstMsgHead->stSrcProc;
       stToFtpMsg.stDstProc   = pstMsgHead->stDstProc;
       stToFtpMsg.ulMsgId      = pstMsgHead->ulMsgId;
       stToFtpMsg.ucPriFlag    = pstMsgHead->ucPriFlag;
       stToFtpMsg.ucAckFlag   = pstMsgHead->ucAckFlag;
       stToFtpMsg.ucShareFlag = 0;
       stToFtpMsg.ulMsgId       = pstMsgHead->ulMsgId;
       stToFtpMsg.ulLen          = pstMsgHead->ulLen;

       stToFtpMsg.stDstProc.ulPd = CPSS_FS_FTP_LOW_PROC;
       
       if(pstMsgHead->ulLen <= sizeof(CPS__OAMS_DIAG_START_TEST_REQ_MSG_T))
       {
        cpss_mem_memcpy(aucPara,pstMsgHead->pucBuf,pstMsgHead->ulLen);
       }
       
       stToFtpMsg.pucBuf = aucPara;
       
       cpss_com_send_extend(&stToFtpMsg);
       

  
}

extern INT32 cpss_disk_used_space_get(
const STRING szDiskName,
UINT32 *pulSizeHigh,
UINT32 *pulSizeLow
);

extern INT32 cpss_disk_space_get(
const STRING szDiskName,
UINT32 *pulSizeHigh,
UINT32 *pulSizeLow
);

VOID cpss_dbg_disk_proc(CPSS_COM_MSG_HEAD_T *pstMsgHead)
{
    CPS__OAMS_DIAG_START_TEST_REQ_MSG_T *pstReq  =  (CPS__OAMS_DIAG_START_TEST_REQ_MSG_T *)pstMsgHead->pucBuf;

   
    switch(pstReq->ulTestCode)
    	{
          case CPSS_DBG_DISK_SPACE_TEST_CODE:
	     {
		  CPSS_DISK_SPACE_INFO_T stDiskInfo;
		  CPSS_DBG_DISK_SPACE_RSP_INFO_T stDiskRspInfo;

            #ifdef CPSS_FUNBRD_MC
                UINT64 ulSumSize = 0;
            #endif    
                
               cpss_mem_memset(&stDiskRspInfo,0,sizeof(CPSS_DBG_DISK_SPACE_RSP_INFO_T));
			  
                 /* 填充消息头*/
                stDiskRspInfo.tReqHeader.ulSeqId   = pstReq->stReqHeader.ulSeqId;
          
              /* 填充内容*/
               /* 活动id */
               stDiskRspInfo.ulTestId = pstReq->ulTestId;
              /* 报告类型*/
               stDiskRspInfo.ulRptType = cpss_htonl(1);
           
               /* 填充长度*/
               stDiskRspInfo.ulTestRptLen = cpss_htonl(sizeof(CPSS_DISK_SPACE_INFO_T));
		  
	
					
		 /* strncpy(stDiskInfo.acName,pcName,CPS__OAMS_DIAG_DIRNAME_MAX_LEN);*/
	  
	#ifdef CPSS_FUNBRD_MC				
	 #ifdef CPSS_VOS_WINDOWS
		  if(cpss_disk_free_space_get("H:\\",&stDiskInfo.ulAllSizeHigh,&stDiskInfo.ulAllSizeLow)==CPSS_OK)
		    {
		          /*报告结果*/
                     stDiskRspInfo.ulTestResult = CPSS_OK;
		   
		    }
	 #elif defined CPSS_VOS_VXWORKS
                 if(cpss_disk_space_get("H:\\",&stDiskInfo.ulAllSizeHigh,&stDiskInfo.ulAllSizeLow)==CPSS_OK)
		    {
		          /*报告结果*/
                     stDiskRspInfo.ulTestResult = CPSS_OK;
		   
		    }
	 #endif

		    {
                         /*报告结果*/
              stDiskRspInfo.ulTestResult = cpss_htonl(1);
			  stDiskRspInfo.ulTestRptLen = 0;
		    }

		   if(cpss_disk_used_space_get("H:\\",&stDiskInfo.ulUsedSizeHigh,&stDiskInfo.ulUsedSizeLow)==CPSS_OK)
		    {
		          /*报告结果*/
                     stDiskRspInfo.ulTestResult = CPSS_OK;
		    }
		   else
		    {
                         /*报告结果*/
                     stDiskRspInfo.ulTestResult = cpss_htonl(1);
			  stDiskRspInfo.ulTestRptLen = 0;
		    }
		   if(stDiskRspInfo.ulTestResult == CPSS_OK)
		   	{
		 #ifdef CPSS_VOS_WINDOWS  	
		   	    stDiskInfo.ulAllSizeHigh += stDiskInfo.ulUsedSizeHigh;
				ulSumSize = (UINT64)stDiskInfo.ulAllSizeLow + (UINT64)stDiskInfo.ulUsedSizeLow;
                          stDiskInfo.ulAllSizeHigh +=(UINT32)(ulSumSize/0xffffffff);
			    stDiskInfo.ulAllSizeLow   =(UINT32)(ulSumSize%0xffffffff);
		#endif		
			   stDiskInfo.ulAllSizeHigh  = cpss_htonl(stDiskInfo.ulAllSizeHigh);
			   stDiskInfo.ulAllSizeLow = cpss_htonl(stDiskInfo.ulAllSizeLow );
			   stDiskInfo.ulUsedSizeHigh = cpss_htonl(stDiskInfo.ulUsedSizeHigh);
			   stDiskInfo.ulUsedSizeLow = cpss_htonl(stDiskInfo.ulUsedSizeLow);
				
		   	}
       #else
                   /*报告结果*/
                 stDiskRspInfo.ulTestResult = cpss_htonl(1);
	      	    stDiskRspInfo.ulTestRptLen = 0;
	 #endif  
              cpss_mem_memcpy(stDiskRspInfo.aucRptInfo,&stDiskInfo,sizeof(stDiskInfo))  ;
		    /* 发送到LDT */
                 cpss_com_send(&pstMsgHead->stSrcProc,
                    CPS__OAMS_DIAG_TEST_APP_RPT_IND_MSG,
                    (UINT8 *)&stDiskRspInfo,
                    sizeof(CPSS_DBG_DISK_SPACE_RSP_INFO_T));	 

                 break;

     	     }

     case CPSS_DBG_DIR_INFO_TEST_CODE:
	 	
		 {
		  CPSS_DIR_INFO_T stDiskInfo;
		  CPSS_DBG_DIR_RSP_INFO_T stDiskRspInfo;
              CHAR *pcName = (CHAR *)pstReq->aucTestParam; 
             
              cpss_mem_memset(&stDiskRspInfo,0,sizeof(CPSS_DBG_DIR_RSP_INFO_T));
			  
                 /* 填充消息头*/
               stDiskRspInfo.tReqHeader.ulSeqId   = pstReq->stReqHeader.ulSeqId;
          
              /* 填充内容*/
               /* 活动id */
               stDiskRspInfo.ulTestId = pstReq->ulTestId;
              /* 报告类型*/
               stDiskRspInfo.ulRptType = cpss_htonl(1);
           
               /* 填充长度*/
               stDiskRspInfo.ulTestRptLen = cpss_htonl(sizeof(CPSS_DIR_INFO_T));
		  
	
					
		  strncpy(stDiskInfo.acName,pcName,CPS__OAMS_DIAG_DIRNAME_MAX_LEN);
	  
	#ifdef CPSS_FUNBRD_MC				
		  if(cpss_dir_used_space_get_extend(pcName,&stDiskInfo.ulSizeHigh,&stDiskInfo.ulSizeLow,&stDiskInfo.ulFileNum)==CPSS_OK)
		    {
		          /*报告结果*/
                     stDiskRspInfo.ulTestResult = CPSS_OK;
		        stDiskInfo.ulSizeHigh = cpss_htonl(stDiskInfo.ulSizeHigh);
			  stDiskInfo.ulSizeLow = 	cpss_htonl(stDiskInfo.ulSizeLow);
			  stDiskInfo.ulFileNum = cpss_htonl(stDiskInfo.ulFileNum);
		   
		    }
		   else
		    {
                         /*报告结果*/
                     stDiskRspInfo.ulTestResult = cpss_htonl(1);
			  stDiskRspInfo.ulTestRptLen = 0;
		    }

		
		   
       #else
                   /*报告结果*/
                 stDiskRspInfo.ulTestResult = cpss_htonl(1);
	      	    stDiskRspInfo.ulTestRptLen = 0;
	 #endif  
              cpss_mem_memcpy(stDiskRspInfo.aucRptInfo,&stDiskInfo,sizeof(stDiskInfo))  ;
		    /* 发送到LDT */
                 cpss_com_send(&pstMsgHead->stSrcProc,
                    CPS__OAMS_DIAG_TEST_APP_RPT_IND_MSG,
                    (UINT8 *)&stDiskRspInfo,
                    sizeof(CPSS_DBG_DIR_RSP_INFO_T));	 

                 break;

     	     }	

    	}

}

VOID cpss_dbg_cps__oams_diag_req_msg_proc(CPSS_COM_MSG_HEAD_T *pstMsgHead)
{
    CPS__OAMS_DIAG_START_TEST_RSP_MSG_T tDiagTestRsp;
    CPS__OAMS_DIAG_START_TEST_REQ_MSG_T *pstReq  =  (CPS__OAMS_DIAG_START_TEST_REQ_MSG_T *)pstMsgHead->pucBuf;

    pstReq->ulTestCode = cpss_ntohl(pstReq->ulTestCode);
  
    
    tDiagTestRsp.stRspHeader.ulSeqId = pstReq->stReqHeader.ulSeqId;
    tDiagTestRsp.stRspHeader.ulReturnCode = 0;
    tDiagTestRsp.ulTestId = pstReq->ulTestId;
    
    switch(pstReq->ulTestCode)
    {
        /* cpu占用率测试码*/
    case CPSS_DBG_CPU_TEST_CODE:  
    case  CPS__OAMS_DIAG_CPSS_GET_DSP_USAGE_TESTCODE:		
        {           
            CPSS_DBG_CPU_CPS__OAMS_O_DIAG_RSP_T stRsp;
            CPSS_DBG_CPU_USAGE_T *pstCpuUsage;
            INT32 lRet;

          /* 发送响应消息*/
            cpss_com_send(&pstMsgHead->stSrcProc,
                CPS__OAMS_DIAG_START_TEST_RSP_MSG,
                (UINT8 *)&tDiagTestRsp,
                sizeof(CPS__OAMS_DIAG_START_TEST_RSP_MSG_T)); 

			
            /* 填充消息头*/
            stRsp.tReqHeader.ulSeqId   = pstReq->stReqHeader.ulSeqId;
          
            /* 填充内容*/
            /* 活动id */
            stRsp.ulTestId = pstReq->ulTestId;
           /* 报告类型*/
            stRsp.ulRptType = cpss_htonl(1);
         
         
            pstCpuUsage = (CPSS_DBG_CPU_USAGE_T *)&stRsp.aucRptInfo[0];
    #ifndef CPSS_DSP_CPU   
            if(CPSS_OK == cpss_cpu_usage_get(&pstCpuUsage->ulUsage))
    #else
	     if(CPSS_OK == cpss_dsp_cpu_get(&pstCpuUsage->ulUsage))
    #endif	
            {
                /* 报告结果*/ 
                stRsp.ulTestResult = 0;
                /* 报告长度*/
                stRsp.ulTestRptLen = cpss_htonl(sizeof(CPSS_DBG_CPU_USAGE_T) );
               /* 填充占用率*/
                pstCpuUsage->ulUsage  = cpss_htonl(pstCpuUsage->ulUsage);               
            }
            else
            {
                /*失败*/
                stRsp.ulTestResult  = cpss_htonl(1);
                 stRsp.ulTestRptLen = 0;
            }
            /* 发送到ldt */                          
            lRet = cpss_com_send(&pstMsgHead->stSrcProc,
                CPS__OAMS_DIAG_TEST_APP_RPT_IND_MSG,
                (UINT8 *)&stRsp,
                sizeof(CPSS_DBG_CPU_CPS__OAMS_O_DIAG_RSP_T)); 
            if(CPSS_OK != lRet)
            {
                cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_ERROR," cpss_com_send fail in cpss_dbg_cps__oams_diag_req_msg_proc\n");
            }          
        }     
        break;
     
        /* 内存信息测试码*/
    case CPSS_DBG_MEM_TEST_CODE:    
    case  CPS__OAMS_DIAG_CPSS_GET_DSP_MEMPOOL_USAGE_TESTCODE:		
        {
            
            CPSS_DBG_MEM_RSP_INFO_T *pstRspInfo;
            UINT32 ulSndSize;
            INT32 lRet;
            UINT16 usLoop;
            /* 采用申请内存方法,避免调用栈过大*/
            CPSS_DBG_MEM_CPS__OAMS_O_DIAG_RSP_T *pstMemRsp = cpss_mem_malloc(sizeof(CPSS_DBG_MEM_CPS__OAMS_O_DIAG_RSP_T));

              /* 发送响应消息*/
            cpss_com_send(&pstMsgHead->stSrcProc,
                CPS__OAMS_DIAG_START_TEST_RSP_MSG,
                (UINT8 *)&tDiagTestRsp,
                sizeof(CPS__OAMS_DIAG_START_TEST_RSP_MSG_T)); 

            if(NULL != pstMemRsp)
            {

                /* 填充消息头*/
                pstMemRsp->tReqHeader.ulSeqId   = pstReq->stReqHeader.ulSeqId;
          
               /* 填充内容*/
               /* 活动id */
                pstMemRsp->ulTestId = pstReq->ulTestId;
              /* 报告类型*/
               pstMemRsp->ulRptType = cpss_htonl(1);
                           
                /* 填写成功返回*/
                pstMemRsp->ulTestResult  = CPSS_OK;
              
                /* 填写信息区*/
                pstRspInfo = (CPSS_DBG_MEM_RSP_INFO_T *)pstMemRsp->aucRptInfo;
                
                cpss_mem_usage_get(&pstRspInfo->ulMemPoolCnt, &pstRspInfo->astMemPools[0]);
                /* 转为网络字节序*/
                for(usLoop = 0; usLoop < pstRspInfo->ulMemPoolCnt; usLoop++)
                {
                    pstRspInfo->astMemPools[usLoop].ulFree = cpss_htonl(pstRspInfo->astMemPools[usLoop].ulFree);
                    pstRspInfo->astMemPools[usLoop].ulFreeSuccTimes =  cpss_htonl(pstRspInfo->astMemPools[usLoop].ulFreeSuccTimes);
                    pstRspInfo->astMemPools[usLoop].ulMallocFailTimes = cpss_htonl(pstRspInfo->astMemPools[usLoop].ulMallocFailTimes);
                    pstRspInfo->astMemPools[usLoop].ulMallocSuccTimes =  cpss_htonl(pstRspInfo->astMemPools[usLoop].ulMallocSuccTimes);
                    pstRspInfo->astMemPools[usLoop].ulPeak = cpss_htonl(pstRspInfo->astMemPools[usLoop].ulPeak);
                    pstRspInfo->astMemPools[usLoop].ulPoolNo = cpss_htonl(pstRspInfo->astMemPools[usLoop].ulPoolNo);
                    pstRspInfo->astMemPools[usLoop].ulSize = cpss_htonl(pstRspInfo->astMemPools[usLoop].ulSize);
                    pstRspInfo->astMemPools[usLoop].ulTotal =  cpss_htonl(pstRspInfo->astMemPools[usLoop].ulTotal);
                }
                
                /* 填写长度*/
                      pstMemRsp->ulTestRptLen  = sizeof(pstRspInfo->ulMemPoolCnt) + pstRspInfo->ulMemPoolCnt * sizeof(CPSS_DBG_MEM_POOL_INFO_T);
                
                pstRspInfo->ulMemPoolCnt = cpss_htonl(pstRspInfo->ulMemPoolCnt);
                      ulSndSize = pstMemRsp->ulTestRptLen + sizeof(CPSS_DBG_CPU_CPS__OAMS_O_DIAG_RSP_T) - sizeof(CPSS_DBG_CPU_USAGE_T);
                      pstMemRsp->ulTestRptLen = cpss_htonl(pstMemRsp->ulTestRptLen);
                /* 发送到LDT */
                lRet = cpss_com_send(&pstMsgHead->stSrcProc,
                    CPS__OAMS_DIAG_TEST_APP_RPT_IND_MSG,
                    (UINT8 *)pstMemRsp,
                    ulSndSize);
                if(CPSS_OK != lRet)
                {
                    cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_ERROR," cpss_com_send fail in cpss_dbg_cps__oams_diag_req_msg_proc\n");
                }
            }
            else
            {   /* 失败*/
                CPSS_DBG_CPU_CPS__OAMS_O_DIAG_RSP_T stRsp;
                
                stRsp.ulTestResult = cpss_htonl(1);
                stRsp.tReqHeader.ulSeqId  = pstReq->stReqHeader.ulSeqId;
                stRsp.ulTestId =  pstReq->ulTestId;
                stRsp.ulTestRptLen = 0;
                /* 发送到LDT */
                lRet = cpss_com_send(&pstMsgHead->stSrcProc,
                    CPS__OAMS_DIAG_TEST_APP_RPT_IND_MSG,
                    (UINT8 *)&stRsp,
                    sizeof(CPSS_DBG_CPU_CPS__OAMS_O_DIAG_RSP_T));
                if(CPSS_OK != lRet)
                {
                    cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_ERROR," cpss_com_send fail in cpss_dbg_cps__oams_diag_req_msg_proc\n");
                }
                
            }
            if(NULL != pstMemRsp)
            {
                cpss_mem_free((VOID*)pstMemRsp);
            }
        }
        break;
 #ifndef CPSS_DSP_CPU  		
    case CPS__OAMS_DIAG_CPSS_CTRLP_COMM_TEST_TESTCODE:
        {
            /*处理链路连通诊断测试请求*/
            cpss_com_diag_comm_test_req_deal(pstReq->aucTestParam,
                cpss_ntohl(pstReq->stReqHeader.ulSeqId),pstMsgHead->stSrcProc.ulPd,
                cpss_ntohl(pstReq->ulTestId)) ; 
            break ;
        }
      /* 时间测试码*/
     case CPSS_DBG_CLOCK_TEST_CODE:
            {
                CPSS_DBG_CLOCK_RSP_INFO_T stClockRsp;
                CPSS_DBG_CLOCK_INFO_T *pstClock ;
                UINT32  ulMode = *((UINT32 *)pstReq->aucTestParam);

		   /* 发送响应消息*/
               cpss_com_send(&pstMsgHead->stSrcProc,
                CPS__OAMS_DIAG_START_TEST_RSP_MSG,
                (UINT8 *)&tDiagTestRsp,
                sizeof(CPS__OAMS_DIAG_START_TEST_RSP_MSG_T)); 

                ulMode = cpss_ntohl(ulMode);

                 cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO," Recv CPSS_DBG_CLOCK_TEST_CODE %d\n",ulMode);

                 /* 填充消息头*/
               stClockRsp.tReqHeader.ulSeqId   = pstReq->stReqHeader.ulSeqId;
          
              /* 填充内容*/
               /* 活动id */
               stClockRsp.ulTestId = pstReq->ulTestId;
              /* 报告类型*/
               stClockRsp.ulRptType = cpss_htonl(1);
               /*报告结果*/
               stClockRsp.ulTestResult = CPSS_OK;
               /* 填充长度*/
               stClockRsp.ulTestRptLen = sizeof(CPSS_DBG_CLOCK_INFO_T);
               
               pstClock = (CPSS_DBG_CLOCK_INFO_T *)&stClockRsp.aucRptInfo[0];
               
                switch(ulMode)
                    {
                        case 0:
                           {
                              pstClock->ulMode = 0;
		  
                             pstClock->ulGmtValue = cpss_htonl(cpss_gmt_get());


                           }

                        break;

                        case 1:
                            {
                                  pstClock->ulMode =  cpss_htonl(1);
                                  pstClock->ulTickValue = cpss_tick_get();
                                  pstClock->ulTickValue = cpss_htonl(pstClock->ulTickValue);
                            }

                        break;

                        case 2:
                             {
                                 pstClock->ulMode = cpss_htonl(2);
                                 cpss_clock_get(&pstClock->stLocalTime);
                                 pstClock->stLocalTime.usYear = cpss_htons(pstClock->stLocalTime.usYear);
                             }

                        break;

                        default:

                        break;   

                    }
                    /* 发送到LDT */
                 cpss_com_send(&pstMsgHead->stSrcProc,
                    CPS__OAMS_DIAG_TEST_APP_RPT_IND_MSG,
                    (UINT8 *)&stClockRsp,
                    sizeof(CPSS_DBG_CLOCK_RSP_INFO_T));
          
            }
     break;      

     /* 磁盘空间测试码*/
     case CPSS_DBG_DISK_SPACE_TEST_CODE:
	     {
		   /* 发送响应消息*/
             cpss_com_send(&pstMsgHead->stSrcProc,
                CPS__OAMS_DIAG_START_TEST_RSP_MSG,
                (UINT8 *)&tDiagTestRsp,
                sizeof(CPS__OAMS_DIAG_START_TEST_RSP_MSG_T)); 

		cpss_dbg_send_diag_to_ftp(pstMsgHead  );  
		  
             
     	     }

     break;	 
     case CPSS_DBG_DIR_INFO_TEST_CODE:
	 	
		 {
		
              /* 发送响应消息*/
              cpss_com_send(&pstMsgHead->stSrcProc,
                CPS__OAMS_DIAG_START_TEST_RSP_MSG,
                (UINT8 *)&tDiagTestRsp,
                sizeof(CPS__OAMS_DIAG_START_TEST_RSP_MSG_T)); 
		
		cpss_dbg_send_diag_to_ftp(pstMsgHead  );  
		
     	     }	
     	      
    break;	 

 #endif               /* #end ifndef CPSS_DSP_CPU */

 case CPS__OAMS_DIAG_CPSS_GET_CPU_MEM_USAGE_TESTCODE:
 case CPS__OAMS_DIAG_CPSS_GET_DSP_MEM_USAGE_TESTCODE:
 	{
	  CPSS_DBG_MEM_OCCUPY_RSP_INFO_T stRspInfo;
         CPSS_MEM_OCCUPY_INFO_T *pstOccupy;	
	 INT32 lRet;	 
	  UINT32 ulFreeBytes,ulUsedBytes;
              /* 发送响应消息*/
            cpss_com_send(&pstMsgHead->stSrcProc,
                CPS__OAMS_DIAG_START_TEST_RSP_MSG,
                (UINT8 *)&tDiagTestRsp,
                sizeof(CPS__OAMS_DIAG_START_TEST_RSP_MSG_T)); 
         /* 获得逻辑的空闲内存*/
/*           cpss_mem_occupy_get(&ulFreeBytes, &ulUsedBytes);*/

                /* 填充消息头*/
             stRspInfo.tReqHeader.ulSeqId   = pstReq->stReqHeader.ulSeqId;
          
               /* 填充内容*/
               /* 活动id */
               stRspInfo.ulTestId = pstReq->ulTestId;
              /* 报告类型*/
              stRspInfo.ulRptType = cpss_htonl(1);
                           
                /* 填写成功返回*/
            stRspInfo.ulTestResult  = CPSS_OK;

            stRspInfo.ulTestRptLen = sizeof(	 CPSS_MEM_OCCUPY_INFO_T);				
              
                /* 填写信息区*/
                pstOccupy = (CPSS_MEM_OCCUPY_INFO_T *)&stRspInfo.aucRptInfo[0];
   #ifndef CPSS_DSP_CPU
   
     #ifdef CPSS_VOS_VXWORKS
	        
		  /* 从驱动获得空闲的物理内存*/
	          stRspInfo.ulTestResult  = drv_meminfo_query(&pstOccupy->ulTotalMem , &pstOccupy->ulFreeMem) ;
	        /* 得到实际可用的空闲内存*/
                 pstOccupy->ulTotalMem = cpss_htonl(pstOccupy->ulTotalMem);
	          pstOccupy->ulFreeMem =  cpss_htonl(pstOccupy->ulFreeMem + ulFreeBytes);
     	      		  
			   
     #endif
     #ifdef CPSS_VOS_WINDOWS
     	{
     	        SYSTEM_INFO sysinfo;     
                memset(&sysinfo,0,sizeof(SYSTEM_INFO));
               GetSystemInfo(&sysinfo);
               pstOccupy->ulTotalMem  =(UINT8 *)sysinfo.lpMaximumApplicationAddress  - (UINT8 *)sysinfo.lpMinimumApplicationAddress;
		 pstOccupy->ulTotalMem = cpss_htonl( pstOccupy->ulTotalMem );
		 pstOccupy->ulFreeMem = cpss_htonl(ulUsedBytes );
     	}
     #endif	 
   
   #else
             /* dsp 上内存总数从配置得到*/
                pstOccupy->ulTotalMem  = CPSS_DSP_MAX_MEM_SIZE;
            /* 得到实际可用的内存*/
                pstOccupy->ulFreeMem  = 	CPSS_DSP_MAX_MEM_SIZE -ulUsedBytes ;

		  pstOccupy->ulTotalMem   = cpss_htonl( pstOccupy->ulTotalMem  );

		  pstOccupy->ulFreeMem  = cpss_htonl(pstOccupy->ulFreeMem );
				 
   	    		 
   #endif
    
	       /* 发送到LDT */
                lRet = cpss_com_send(&pstMsgHead->stSrcProc,
                    CPS__OAMS_DIAG_TEST_APP_RPT_IND_MSG,
                    (UINT8 *)&stRspInfo,
                    sizeof(CPSS_DBG_MEM_OCCUPY_RSP_INFO_T));
		   
                if(CPSS_OK != lRet)
                {
                    cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_ERROR," cpss_com_send fail in cpss_dbg_cps__oams_diag_req_msg_proc\n");
                }
       

       }
 
 break;
    default:
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_ERROR,
            " cpss_dbg_cps__oams_diag_req_msg_proc() receive invalid test code %x\n",pstReq->ulTestCode);
        break;
        
    }
    
    
}	

#ifndef CPSS_DSP_CPU
/*******************************************************************************
* 函数名称: cpss_dbg_cps__oams_diag_stop_test_msg_deal()
* 功    能: oams测试诊断停止消息的处理函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:                                            
*******************************************************************************/
INT32 cpss_dbg_cps__oams_diag_stop_test_msg_deal
(
 UINT8* pucRecvMsg
)
{
    CPS__OAMS_DIAG_STOP_TEST_IND_MSG_T stStopTestMsg ;

    stStopTestMsg = *(CPS__OAMS_DIAG_STOP_TEST_IND_MSG_T*)pucRecvMsg ;

    /*转换字节序*/
    stStopTestMsg.stReqHeader.ulSeqId = cpss_ntohl(stStopTestMsg.stReqHeader.ulSeqId) ;
    stStopTestMsg.ulTestId = cpss_ntohl(stStopTestMsg.ulTestId) ;
    stStopTestMsg.ulStopReason = cpss_ntohl(stStopTestMsg.ulStopReason) ;

    /*链路测试取消函数*/
    cpss_com_comm_stop_test_deal(stStopTestMsg.ulTestId) ;
   
    return CPSS_OK ;
}
#else
INT32 cpss_dbg_cps__oams_diag_stop_test_msg_deal
(
 UINT8* pucRecvMsg
)
{
    return CPSS_OK;
}
/*
VOID cpss_dbg_cps__oams_diag_req_msg_proc(CPSS_COM_MSG_HEAD_T *pstMsgHead)
{
    return ;
}
*/
/*INT32 cpss_dbg_test_data_check(UINT8 *pucBuf, UINT32 ulLen)
{
    return;
}
*/
#endif
/*******************************************************************************
* 函数名称: cpss_dbg_cps__oams_diag_req_msg_proc
* 功    能: 诊断测试消息处理流程 
* 函数类型: 
* 参    数: 
* 参数名称          类型                     输入/输出         描述
* pstMsgHead       CPSS_COM_MSG_HEAD_T *      输入            消息头指针
* 
* 函数返回: 无。
* 说    明: 
*******************************************************************************/

  VOID cpss_dbg_sim_shell_init(VOID)
{
  UINT32 ulLoop ;
  /* 统计纤程初始化*/
/*  g_bCpssProcStartActive = TRUE;*/
  
/*  cpss_mem_proc_record_init();*/
  
  for(ulLoop = 0; ulLoop <  g_ulCpssDbgShellConfigNum; ulLoop++)
    {
#ifndef SWP_FNBLK_BRDTYPE_ABOX
        cps__oams_shcmd_reg(g_astCpssDbgShellRegTbl[ulLoop].aucCmdName,
                                   g_astCpssDbgShellRegTbl[ulLoop].aucCmdHelp,
                                   g_astCpssDbgShellRegTbl[ulLoop].aucArgFmt,
                                   g_astCpssDbgShellRegTbl[ulLoop].pfCmdFunc);
#endif
                                   
    } 
#ifdef CPSS_VOS_WINDOWS
  g_stAppTmvGrpCpssArray.ulStartTm = cpss_tick_get();
  g_stAppTmvGrpCpssArray.ulTmVarValue0 = 10;
#endif

#if 0
#ifndef SWP_FNBLK_BRDTYPE_ABOX
#ifndef CPSS_DSP_CPU
  if(CPS__OAMS_OK != cps__oams_pm_register_tmvar_ptr(66, CPS__OAMS_PM_OPTION_NO_ADD_UP,
        g_astCpssTmvGrpToCntGrpMapTbl,
        3,
        sizeof(APP_TMVGRP_CPSS_T),
        1,
        &g_stAppTmvGrpCpssArray))
  	{
            cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_ERROR,
            " cps__oams_pm_register_tmvar_ptr return fail\n\r");  
  	}
 #endif 
 #endif 
 #endif

}
/*==== VX i CMD ====*/
#ifdef CPSS_VOS_VXWORKS
extern UINT32 g_ulCpssDbgModule ;
void cpss_dbg_simi()
{
    char String[64];
    INT32 i;
    int TaskPri;
    int TaskSum;
    int TaskList[100];

    TaskSum = taskIdListGet(TaskList, 100);

    cps__oams_shcmd_printf(" Dbg ModuleId:%d\n\r",g_ulCpssDbgModule);
    
    if(0 != TaskSum)
    {
        cps__oams_shcmd_printf("%-20s %-10s %-10s %-10s\n", "TASK_NAME", "TASK_ID", "TASK_PRI", "TASK_STAT");
    }
    for(i = 0; i < TaskSum; i++)
    {
        taskPriorityGet(TaskList[i], &TaskPri);
        taskStatusString(TaskList[i], String);
        cps__oams_shcmd_printf("%-20s %-10X %-10d %-10s\n", taskName(TaskList[i]), TaskList[i], TaskPri, String);
    }
}

/*==== VX tt CMD ====*/

void cpss_dbg_simtt(INT32 TaskID)
{
    STATUS Result;
    TASK_DESC TaskInfo;

    Result = taskInfoGet(TaskID, &TaskInfo);
    if(OK != Result)
    {
        cps__oams_shcmd_printf("no this task.\n");
        return;
    }
    cps__oams_shcmd_printf("task id : 0x%X\n", TaskInfo.td_id);
    cps__oams_shcmd_printf("name of task : %s\n", TaskInfo.td_name);
    cps__oams_shcmd_printf("task priority : %d\n", TaskInfo.td_priority);
    cps__oams_shcmd_printf("task status : %d\n", TaskInfo.td_status);
    cps__oams_shcmd_printf("task option bits (see below) : %d\n", TaskInfo.td_options);
    cps__oams_shcmd_printf("original entry point of task : 0x%X\n", TaskInfo.td_entry);
    cps__oams_shcmd_printf("saved stack pointer : 0x%X\n", TaskInfo.td_sp);
    cps__oams_shcmd_printf("the bottom of the stack : 0x%X\n", TaskInfo.td_pStackBase);
    cps__oams_shcmd_printf("the effective end of the stack : 0x%X\n", TaskInfo.td_pStackLimit);
    cps__oams_shcmd_printf("the actual end of the stack : 0x%X\n", TaskInfo.td_pStackEnd);
    cps__oams_shcmd_printf("size of stack in bytes : %d\n", TaskInfo.td_stackSize);
    cps__oams_shcmd_printf("current stack usage in bytes : %d\n", TaskInfo.td_stackCurrent);
    cps__oams_shcmd_printf("maximum stack usage in bytes : %d\n", TaskInfo.td_stackHigh);
    cps__oams_shcmd_printf("current stack margin in bytes : %d\n", TaskInfo.td_stackMargin);
    cps__oams_shcmd_printf("most recent task error status : 0x%X\n", TaskInfo.td_errorStatus);
    cps__oams_shcmd_printf("delay/timeout ticks : %d\n", TaskInfo.td_delay);
}
#else
void cpss_dbg_simi()
{
    return;
}
void cpss_dbg_simtt(INT32 TaskID)
{
    return;
}
#endif

/*******************************************************************************
* 函数名称: cpss_cpu_load_read
* 功    能: CPU load读取  
* 函数类型: 
* 参    数: 
* 参数名称          类型                     输入/输出         描述
* 
* 
* 函数返回: 无。
* 说    明: 
*******************************************************************************/
VOID cpss_cpu_load_read( )
{

#if 0
#ifndef SWP_FNBLK_BRDTYPE_ABOX
    cps__oams_moid_get_local_moid(CPS__OAMS_MOID_TYPE_CPU, &g_stAppTmvGrpCpssArray.stMoId);
#endif
    g_stAppTmvGrpCpssArray.ulStartTm = cpss_gmt_get();
    cpss_cpu_usage_get(&g_stAppTmvGrpCpssArray.ulTmVarValue0);
#endif	

}
/******************************* 源文件结束 ***********************************/


