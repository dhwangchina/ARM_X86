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
#include "cpss_vos_sem.h"
#include "cpss_dbg_sbbr.h"
#include "cpss_vos_task.h"
#include "cpss_vk_sched.h"
#include "cpss_err.h"
#ifdef CPSS_VOS_VXWORKS
#include "trcLib.h"
#include "moduleLib.h"
#if (CPSS_CPU_TYPE == CPSS_CPU_PPC)
#include "arch/ppc/archPpc.h"
#include "arch/ppc/esfppc.h"
#include "arch/ppc/excPpcLib.h"
#include "arch/ppc/regsPpc.h"
#endif
#if (CPSS_CPU_TYPE == CPSS_CPU_I86)
#include "arch/i86/esfI86.h"
#include "arch/i86/ivI86.h"
#endif
#if (CPSS_CPU_TYPE == CPSS_CPU_ARM)
#include "arch/arm/esfArm.h"
#endif
#endif
#ifndef CPSS_DSP_CPU

/******************************* 局部宏定义 ***********************************/
#define CPSS_SBBR_TEXT_BUFFER_SIZE 5000      /* size of SBBR Text Buffer */ /* change from 512 to 5000 for record stackInfo */

#define CPSS_SBBR_MAX_INT_USER_NUM 20
/* 中断区 */
#define CPSS_SBBR_DRV_SECT_SIZE ( 1024 * 100)
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA) ||(SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
 /* 异常区 */
#define CPSS_SBBR_EXC_SECT_SIZE (1024 * 1024)
#else
 /* 异常区 */
#define CPSS_SBBR_EXC_SECT_SIZE (1024 * 200)
#endif
/* 用户区 */
#define CPSS_SBBR_USR_SECT_SIZE  (1024 * 500)
/* 特殊区 */
#define CPSS_SBBR_SPEC_SECT_SIZE  (1024 * 100)
/* 版本 */
#define CPSS_SBBR_VERSION  ((UINT8)0x10)

#define CPSS_SBBR_WRITE_ENABLE   (1)

#define CPSS_SBBR_WRITE_DISABLE  (0)

#define CPSS_SBBR_RECORED_TYPE_TEXT  0

#define CPSS_SBBR_RECORED_TYPE_DATA  1

#define CPSS_SBBR_RECORD_TYPE_DECODE_DATA 2
#define CPSS_SBBR_AUTO_TRANSFER_ENABLE     1

#define CPSS_SBBR_AUTO_TRANSFER_DISABLE    0

#define CPSS_SBBR_ALIGN(offset) (((offset) + 3) & 0xFFFFFFFC)

#define CPSS_SBBR_TRANS_END   0

#define CPSS_SBBR_MAX_HOOK_NUM 100
#if 0
#define CPSS_SBBR_EXC_BUF_LENGTH   (24*1024)    /* 存储异常信息的长度 */
#endif
#define CPSS_MAX_SYM_NAME_LENGTH  256

#define CPSS_SBBR_WIN_SIZE (1024 * 4096 )
/* 等待响应的定时器信息 */
#define CPSS_SBBR_WAITRSP_TIMER_NO (5)
/* 2007-05-18修改为10秒 */
#define CPSS_SBBR_WAITRSP_TIMER_LENGTH    (10000)

/* HOST CPU 编号 */
#define CPSS_SBBR_HOST_CPU_NO    1
/* SLAVE CPU 编号 */
#define CPSS_SBBR_SLAVE_CPU_NO   2
/* DSP 起始编号 */
#define CPSS_SBBR_DSP_START_NO   5
#define CPSS_SBBR_WAIT_RSP_MAX_COUNT           60

/* sbbr store struct
 * 
 * -------------------- |DRV Info Area | -------------------- |Man Info Area | ------------------- |Exception Info Area| -------------------- |User Info Area | -------------------- |spcial Area( isr) | --------------------
 * 
 * end sbbr store struct */
#define CPSS_KW_RUN_FLAG_NO_RUN     0
#define CPSS_KW_RUN_FLAG_RUN           1
#define CPSS_KW_MAX_SCAN_ITEM            256
#define CPSS_SBBR_DEFAULT_SIZE  100*1024
#define CPSS_SBBR_MAX_RECORD_LEN (64 * 1024)
#define CPSS_SBBR_WRITE_DATA_MAX_LEN   200

/* changed by laixf for changed brd_name XAB121 2009-6-29 */
/* #define CPSS_GCPA_SBBR_FILE_NAME "H:/oams/sbbr/bb_gcpa" */
/* #define CPSS_RSPA_SBBR_FILE_NAME "F:/sbbr/bb_rspa" */

#define CPSS_GCPA_SBBR_FILE_NAME   "H:/oams/sbbr/bb_ccpu"
#define CPSS_RSPA_SBBR_FILE_NAME   "F:/sbbr/bb_hspu"
/* changed end */

#define CPSS_SBBR_DECODE_DATA_MAX_LEN        64
#define CPSS_TASK_MAX_RECORD_LENGH  (60 * 1024)
#define CPSS_MODULE_NAME  "bootoem.out"

/******************************* 全局变量定义/初始化 **************************/
/* 全局管理对象 */

CPSS_SBBR_LOCAL_MNG_T g_stCpssSbbrLocalMng;

/* 存放注册的hook ,系统异常时由cpss 调用 */
CPSS_SBBR_HOOK_PF g_astCpssSbbrHookArray[CPSS_SBBR_MAX_HOOK_NUM] = { 0 };

/* 黑匣子管理区 */
CPSS_SBBR_SYS_T *g_pstCpssSbbrSys = NULL;

/* 存放硬件提供的黑匣子的起始地址 */
UINT8 *g_pucCpssSbbrStartAddr = NULL;

/* 存放硬件提供的黑匣子的大小 */
UINT32 g_ulCpssSbbrSize = 0;

/* 存放临时文本 */
CHAR g_ucSbbrTextBuffer[CPSS_SBBR_TEXT_BUFFER_SIZE];    /* used in `sbbr_text' */

/* 存放文本时用到的互斥 */
UINT32 g_ulCpssSbbrMutex;

UINT32 g_ulCpssSbbrWaitRspCount = 0;
/* 中断用户配置表 */
CPSS_SBB_INIT_CONFIG_T g_stCpssIntConfig[] = {
    /* 用户名 编号 大小 */
    {"", 0xffffffff, 0}
};

/* 中断用户数目 */
UINT32 g_ulCpssSbbrIsrNum = NUM_ENTS(g_stCpssIntConfig);

/* 上次发生异常标志 */
BOOL g_bCpssSbbrExcOccur = FALSE;
/* 当前发生异常标识 */
BOOL g_bCpssSbbrCurExcOccur = FALSE;
UINT8 g_ucCpssSbbrOldWriteFlag;
UINT8 *g_ucCpssSbbrTransBuf = NULL;
UINT32 g_ulCpssSbbrTransSize = 0;
CHAR *g_pcSbbrExcInfo = NULL;
UINT32 g_ulCpssSbbrTaskUserBase = 0;
BOOL g_bCpssFirstPowerOn = FALSE;
BOOL g_bCpssCloseException = FALSE;
extern INT32 swp_get_ha_time(INT32 lType);
extern CPSS_VK_PROC_CLASS_T g_astCpssVkProcClassTbl[VK_MAX_PROC_CLASS_NUM];
/* 
 * extern VK_TASK_CONFIG_T gatVkTaskConfig[VOS_MAX_TASK_NUM] ; 
extern VK_SCHED_CONFIG_T gatVkSchedConfig[VK_MAX_SCHED_NUM]; 
*/
extern CPSS_VK_SCHED_DESC_T g_astCpssVkSchedDescTbl[VK_MAX_SCHED_NUM];
extern INT32 cpss_vos_task_info_get(UINT32 ulTaskDesc,  /* 任务描述符 */
                                    CPSS_VOS_TASK_INFO_T * pstTaskInfo  /* 指向任务信息的指针 */
    );

/************ stub begin **************/
extern CPSS_VOS_TASK_DESC_T g_astCpssVosTaskDescTbl[VOS_MAX_TASK_NUM];
/* 
 * extern CHAR *taskName(INT32 lTid); */

/************ stub end **************/
extern INT32 cpss_com_dsp_logic_addr_get(UINT8 ucCpuNo, CPSS_COM_LOGIC_ADDR_T * pstDspLogAddr);

#ifdef CPSS_VOS_VXWORKS
extern BOOL g_bCpssTaskCreateEnd;
#endif

VOID cpss_sbbr_output_task_info(INT32 lTid);
void cpss_sbbr_tt(UINT32 ulTaskId);

unsigned char g_azCpssSbbrTempBuf[15][400];
extern int trcDefaultArgs;

INT32 g_lcurCallDep = 0;
VOID cpss_sbbr_hook_proc(INT32 ulTid);

/* SBB日志文件头部 */
/* changed by laixf for changed brd_name XAB121 2009-6-29 */
CPSS_LOGM_SBBLOG_FILE_HDR_T g_stCpssLogmSBBlogFileHdr = {
    {'n', 'p', 'c', 'b', 'b', 'l', 'o', 'g'},
    0,
    {0}
};

/* 查询纤程是否激活完 */
extern BOOL smss_get_proc_active_state(VOID);
UINT32 g_ulCpssSbbrHookUserId = 0;
UINT32 g_ulTaskStatusErr = 0;
UINT32 g_ulTaskStatusErr1 = 0, g_ulTaskStatusErr2 = 0, g_ulTaskStatusErr3 = 0;

BOOL g_bCpssShowOccupy = FALSE;

extern INT32 g_CpssLoadTaskID;
extern INT32 g_CPssLoadTaskInterval;
extern BOOL g_CpssLoadTaskOverFlow;
extern VOID cpss_sbbr_output_task_msg_queue(UINT32 ulDrvUserId);
extern VOID cpss_sbbr_output_task_status(UINT32 ulDrvUserId);
#ifdef CPSS_VOS_VXWORKS
extern UINT32 g_ulCpssFeedDogCount;
#endif

UINT32 g_ulCpssWatchDogHookUserId = 0;
#ifdef CPSS_VOS_VXWORKS
VOID cpss_taskRegsShow(INT32 lTid);
extern REG_INDEX taskRegName[];
#endif
extern cpss_com_local_is_mmc();

STRING g_szKwProcName;
UINT32 g_aulKwMsgId[VK_MAX_SCHED_NUM];

/******************************* 局部常数和类型定义 ***************************/

#ifdef CPSS_VOS_VXWORKS

typedef struct CPSS_OS_EXC_INFO
{

    CPSS_ESF tEsf;
    REG_SET tRegSet;

} CPSS_OS_EXC_INFO_T;

CPSS_OS_EXC_INFO_T g_stExcInfo;

#endif
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

CPSS_KW_SCAN_INFO_T *g_pstCpssKwScanInfo = NULL;

UINT32 g_ulKwAllTicks = 0;      /* 在该时间段内统计占用率 */

typedef struct tagCPSS_MAX_OCCUPY_RECORD
{
    CHAR acName[32];            /* 占用率最高的 任务名or纤程名 */
    UINT32 ulOccupy;            /* 在g_ulKwAllTicks时间内的占用率 */
} CPSS_MAX_OCCUPY_RECORD_T;

 /* 0 存储操作系统任务的最高占用率, 1 存储自研任务的最高占用率, 2 存储纤程的最高占用率 */

CPSS_MAX_OCCUPY_RECORD_T g_tCpssOccupyAnalysis[3];

/* 存储vxWorks 任务的占用率 */
typedef struct tagCPSS_VXWORKS_TASK_OCCUPY
{
    INT32 lTid;                 /* task id */
    UINT32 ulRunTicks;          /* 运行的ticks */
} CPSS_VXWORKS_TASK_OCCUPY_T;

CPSS_VXWORKS_TASK_OCCUPY_T g_tCpssVxworksTaskOccupy[30];    /* 最多30个vxWorks任务 */

UINT32 g_ulCpssVxworksTaskNum = 0;  /* vxWorks的任务数量 */
BOOL g_bCpssChkOccupy = FALSE;  /* 控制是否记录占用率开关 */
BOOL g_bCpssOccupyOver = FALSE; /* 同步smss的占用率日志输出 */

/******************************* 局部函数原型声明 *****************************/
INT32 cpss_sbbr_record(UINT32 ulUserId, UINT8 ucType, UINT32 ulArgs[4], CHAR * pucData, UINT32 ulSize);
VOID cpss_sbbr_sprintf(CHAR * pcBuf, CHAR * pcFmt, ...);
INT32 cpss_sbbr_start_transaction_dsp(UINT32 ulCpuNo);
#ifdef CPSS_VOS_VXWORKS
INT32 cpss_sbbr_write_task_stack_to_sbbr(INT32 lTid);
#endif

typedef struct
{
    CHAR *pTransBuf;
    UINT32 ulSize;

} CPSS_SAVE_DSP_BUF_T;

CPSS_SAVE_DSP_BUF_T g_tCpssSaveDspBuf[CPSS_COM_MAX_CPU];
#define CPSS_SBBR_WAIT_NEXT_DSP_TRANS_LENGTH 6000

/******************************* 函数实现 *************************************/
#if 0
INT32 cpss_dbg_get_init_sect_size(UINT32 * pulSize)
{
    UINT16 usLoop;

    *pulSize = 0;

    if (g_ulCpssSbbrIsrNum >= CPSS_SBBR_MAX_INT_NUM)
    {
        return CPSS_ERROR;
    }
    for (usLoop = 0; usLoop < g_ulCpssSbbrIsrNum; usLoop++)
    {
        *pulSize += g_stCpssIntConfig[usLoop].ulSize;
    }
    return CPSS_OK;
}
#endif

/*******************************************************************************
* 函数名称: cpss_sbbr_set_exc_flag
* 功    能: 设置本地管理结构中的exc flag.
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* ucFlag             UINT8             IN               0 or 1
* 函数返回: 
*                          
* 说   明:  
*******************************************************************************/

VOID cpss_sbbr_set_exc_flag(UINT8 ucFlag)
{
    g_stCpssSbbrLocalMng.ucExecActFlag = ucFlag;

    /* 不允许再向异常区写入,同时不允许再向其他区写入 */
    if (ucFlag == 1)
    {
        if (g_pstCpssSbbrSys != NULL)
            g_pstCpssSbbrSys->ucWriteAble = CPSS_SBBR_WRITE_DISABLE;
    }
}

/*******************************************************************************
* 函数名称: cpss_sbbr_set_normal_reset_flag
* 功    能: 设置启动标志为正常启动
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*
*                          
* 说   明:  该函数在初始化后或上传完数据后被调用
*******************************************************************************/

VOID cpss_sbbr_set_abnormal_reset_flag(VOID)
{
    if (NULL != g_pstCpssSbbrSys)
    {
        g_pstCpssSbbrSys->ulRunStatusFlag = CPSS_SBBR_RUN_STATUS_ABNORMAL;  /* 0x12345678 */
    }
}

/*******************************************************************************
* 函数名称: cpss_sbbr_set_normal_reset_flag
* 功    能: 设置启动标志为正常启动
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*
*                          
* 说   明:  该函数在cpss_reset()中被调用
*******************************************************************************/

VOID cpss_sbbr_clr_abnormal_reset_flag(VOID)
{
    if (NULL != g_pstCpssSbbrSys)
    {
        g_pstCpssSbbrSys->ulRunStatusFlag = CPSS_SBBR_RUN_STATUS_NORMAL;    /* 0x15263748 */
    }
}

/*******************************************************************************
* 函数名称: cpss_sbbr_first_power_on
* 功    能: 检查是否第一次加电
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*         TRUE 
*         FALSE                    
* 说   明:                                        
*******************************************************************************/

UINT32 cpss_sbbr_get_all_user_size(VOID)
{

    UINT32 ulAllSize = 0;
    UINT16 usLoop;
    CPSS_SBBR_USER_T *pstUser;

    pstUser = g_pstCpssSbbrSys->astUser;

    for (usLoop = 0; usLoop <= g_pstCpssSbbrSys->usUserNum; usLoop++)
    {
        ulAllSize += (UINT32) pstUser[usLoop].lSize;

    }
    return ulAllSize;

}

BOOL cpss_sbbr_ready_to_trans_overflow(VOID)
{
    return ((g_pstCpssSbbrSys->astUser[g_pstCpssSbbrSys->usUserNum].pucAddrStart + g_pstCpssSbbrSys->astUser[g_pstCpssSbbrSys->usUserNum].lSize - g_pstCpssSbbrSys->pucSbbrStartAddr) > g_pstCpssSbbrSys->ulSBBRTotalLen);

}

BOOL cpss_sbbr_first_power_on(VOID)
{
    /* 如果加电标志已有,则返回失败 */
    if (g_pstCpssSbbrSys->ulPowerOnFlag == CPSS_SBBR_POWER_ON_FIRST)
    {
        /* 管理信息区被破坏,返回第一次加电流程 */
        if ((g_pstCpssSbbrSys->astUser[0].pucAddrStart != (unsigned char *)g_pstCpssSbbrSys + sizeof(CPSS_SBBR_SYS_T)) || (g_pstCpssSbbrSys->astUser[0].lSize != CPSS_SBBR_EXC_SECT_SIZE) || (g_pstCpssSbbrSys->ulMngSysLen != sizeof(CPSS_SBBR_SYS_T)) || (g_pstCpssSbbrSys->usUserNum > SBBR_USER_MAX) || (cpss_sbbr_get_all_user_size() > g_pstCpssSbbrSys->ulSBBRTotalLen) || (cpss_sbbr_ready_to_trans_overflow() == TRUE))
        {
            return TRUE;
        }
        else
        {
            g_pstCpssSbbrSys->ucCheckSum = 0;   /* 标示不是第一次上电 */
            return FALSE;
        }
    }
    else                        /* 第一次加电 */
    {
        return TRUE;
    }

}

/*******************************************************************************
* 函数名称: cpss_sbbr_exception_user_init
* 功    能: 初始化异常用户区0
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*                             
* 说   明:                                        
*******************************************************************************/

VOID cpss_sbbr_exception_user_init(VOID)
{
    /* 初始化异常区,该区占用用户0 */
    snprintf(g_pstCpssSbbrSys->astUser[0].acName, 32, "Exception");
    g_pstCpssSbbrSys->astUser[0].pucAddrStart = (unsigned char *)g_pstCpssSbbrSys + sizeof(CPSS_SBBR_SYS_T);
    g_pstCpssSbbrSys->astUser[0].lSize = CPSS_SBBR_EXC_SECT_SIZE;
    g_pstCpssSbbrSys->astUser[0].lMin = g_pstCpssSbbrSys->astUser[0].lCurWriteLoc = 0;
    /* 初始化异常区长度 */
    *(unsigned short *)g_pstCpssSbbrSys->astUser[0].pucAddrStart = 0;
    /* 设置向异常区写能 */
    cpss_sbbr_set_exc_flag(0);

    cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, "cpss_sbbr_exception_user_init() finished\n\r");

}

/*******************************************************************************
* 函数名称: cpss_sbbr_init_sys_buf
* 功    能: 初时化系统管理区
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*                             
* 说   明:                                        
*******************************************************************************/

VOID cpss_sbbr_init_sys_buf(VOID)
{
    if (NULL != g_pstCpssSbbrSys)
    {

        UINT16 usTemp = 0x0001;
        /* 初始化管理区 */
        g_pstCpssSbbrSys->ulPowerOnFlag = CPSS_SBBR_POWER_ON_FIRST;
        g_pstCpssSbbrSys->ulRunStatusFlag = CPSS_SBBR_RUN_STATUS_ABNORMAL;
        g_pstCpssSbbrSys->ulVersion = CPSS_SBBR_VERSION;
        g_pstCpssSbbrSys->pucSbbrStartAddr = g_pucCpssSbbrStartAddr;
        g_pstCpssSbbrSys->ulSBBRTotalLen = g_ulCpssSbbrSize;
        g_pstCpssSbbrSys->pucMngStartAddr = (UINT8 *) g_pstCpssSbbrSys;
        g_pstCpssSbbrSys->ulMngSysLen = sizeof(CPSS_SBBR_SYS_T);
        g_pstCpssSbbrSys->pucIsrStartAddr = g_pucCpssSbbrStartAddr;
        g_pstCpssSbbrSys->ulMngIsrLen = CPSS_SBBR_DRV_SECT_SIZE;
        g_pstCpssSbbrSys->pucExecStartAddr = (UINT8 *) g_pstCpssSbbrSys + sizeof(CPSS_SBBR_SYS_T);
        g_pstCpssSbbrSys->ulExecLen = CPSS_SBBR_EXC_SECT_SIZE;
        g_pstCpssSbbrSys->pucUserStartAddr = g_pstCpssSbbrSys->pucExecStartAddr + CPSS_SBBR_EXC_SECT_SIZE;
        g_pstCpssSbbrSys->ulUserLen = CPSS_SBBR_USR_SECT_SIZE;
        g_pstCpssSbbrSys->pucSpecAddr = g_pstCpssSbbrSys->pucUserStartAddr + CPSS_SBBR_USR_SECT_SIZE;
        g_pstCpssSbbrSys->ulSpecLen = CPSS_SBBR_SPEC_SECT_SIZE;
        g_pstCpssSbbrSys->ucEndian = *((UINT8 *) & usTemp); /* 0 big ; 1 little */
        g_pstCpssSbbrSys->ucCheckSum = 1;   /* 设置第一次上电 */
        /* 初始化为仅当有异常发生时上传 */
        g_pstCpssSbbrSys->ucTransfer = CPSS_SBBR_TRANSACTION_FORCE;
        g_pstCpssSbbrSys->ucWriteAble = CPSS_SBBR_WRITE_ENABLE;
        g_pstCpssSbbrSys->usSeqNumNow = 0;
        g_pstCpssSbbrSys->usUserNum = 0;
        /* 初始化异常用户区 */
        cpss_sbbr_exception_user_init();
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, "cpss_sbbr_init_sys_buf() finished\n\r");

    }

}

/*******************************************************************************
* 函数名称: cpss_sbbr_abnormal_restart
* 功    能: 检查是否异常重起
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*                             
* 说   明:                                        
*******************************************************************************/

BOOL cpss_sbbr_abnormal_restart(VOID)
{
    /* 如果运行标志为异常,证明单板复位命令没有被调用过,属于异常重起 */
    if (g_pstCpssSbbrSys->ulRunStatusFlag == CPSS_SBBR_RUN_STATUS_ABNORMAL)
    {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, "cpss_sbbr_abnormal_restart() = TRUE\n\r");

        return TRUE;
    }
    else                        /* 调用单板复位cpss_reset(),异常标志被清 */
    {
        return FALSE;
    }
}

/*******************************************************************************
* 函数名称: cpss_sbbr_user_init
* 功    能: 初始化用户区
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*                             
* 说   明:                                        
*******************************************************************************/

VOID cpss_sbbr_user_init(VOID)
{
    UINT16 usLoop;
    CPSS_SBBR_USER_T *pstUser;
    CPSS_VOS_TASK_INFO_T tTaskInfo;
    pstUser = g_pstCpssSbbrSys->astUser;
    /* 当前用户数清0 */
    g_pstCpssSbbrSys->usUserNum = 0;
    /* 初始化中断用户区 */
    for (usLoop = 1; usLoop <= g_ulCpssSbbrIsrNum; usLoop++)
    {
        if (g_stCpssIntConfig[usLoop - 1].ulSize != 0)
        {

            g_pstCpssSbbrSys->usUserNum = usLoop;
            /* 调度器描述符有可能是不连续的,但usedId必须连续 */
            strncpy(pstUser[g_pstCpssSbbrSys->usUserNum].acName, g_stCpssIntConfig[usLoop - 1].acName, 32);
            pstUser[g_pstCpssSbbrSys->usUserNum].lSize = g_stCpssIntConfig[usLoop - 1].ulSize;
            pstUser[g_pstCpssSbbrSys->usUserNum].Hook = NULL;
            pstUser[g_pstCpssSbbrSys->usUserNum].lMin = pstUser[g_pstCpssSbbrSys->usUserNum].lCurWriteLoc = 0;
            pstUser[g_pstCpssSbbrSys->usUserNum].pucAddrStart = pstUser[usLoop - 1].pucAddrStart + pstUser[usLoop - 1].lSize;

        }
        else
        {
            break;
        }

    }
    cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, "cpss_sbbr_user_init()-->isr user finished\n\r");

    g_ulCpssSbbrTaskUserBase = g_pstCpssSbbrSys->usUserNum;

    /* 初始化任务用户区 */
    for (usLoop = 1; usLoop < VOS_MAX_TASK_NUM; usLoop++)
    {
        if (g_astCpssVosTaskDescTbl[usLoop].bInuse == TRUE)
        {

            cpss_vos_task_info_get(usLoop, &tTaskInfo);

            strncpy(pstUser[usLoop + g_ulCpssSbbrTaskUserBase].acName, tTaskInfo.pcName, 32);
            /* 由于采用二次调度,所以用户注册的hook在异常发生时,由监守任务调用,此处的hook已没有用 */
            pstUser[usLoop + g_ulCpssSbbrTaskUserBase].Hook = NULL;
            pstUser[usLoop + g_ulCpssSbbrTaskUserBase].pucAddrStart = pstUser[usLoop + g_ulCpssSbbrTaskUserBase - 1].pucAddrStart + pstUser[usLoop + g_ulCpssSbbrTaskUserBase - 1].lSize;
            pstUser[usLoop + g_ulCpssSbbrTaskUserBase].lSize = 1024 * 30;
            if (!strncmp(tTaskInfo.pcName, SWP_SCHED_M_TASKNAME, strlen(SWP_SCHED_M_TASKNAME)))
            {
                pstUser[usLoop + g_ulCpssSbbrTaskUserBase].lSize = 2 * 1024 * 1024;
            }
            pstUser[usLoop + g_ulCpssSbbrTaskUserBase].lMin = pstUser[usLoop + g_ulCpssSbbrTaskUserBase].lCurWriteLoc = 0;
            g_pstCpssSbbrSys->usUserNum = usLoop + g_ulCpssSbbrTaskUserBase;
        }
    }

    if (g_pstCpssSbbrSys->usUserNum + 1 < SBBR_USER_MAX - 1)
    {
        g_pstCpssSbbrSys->usUserNum++;

        snprintf(pstUser[g_pstCpssSbbrSys->usUserNum].acName, 32, "ExcHookUser");
        pstUser[g_pstCpssSbbrSys->usUserNum].Hook = NULL;
        pstUser[g_pstCpssSbbrSys->usUserNum].pucAddrStart = pstUser[g_pstCpssSbbrSys->usUserNum - 1].pucAddrStart + pstUser[g_pstCpssSbbrSys->usUserNum - 1].lSize;
        pstUser[g_pstCpssSbbrSys->usUserNum].lSize = 10 * 1024;
        pstUser[g_pstCpssSbbrSys->usUserNum].lMin = pstUser[g_pstCpssSbbrSys->usUserNum].lCurWriteLoc = 0;
        g_ulCpssSbbrHookUserId = g_pstCpssSbbrSys->usUserNum;
        /* g_ulCpssSbbrIsrStart = g_pstCpssSbbrSys->usUserNum +1; */
    }

    if (g_pstCpssSbbrSys->usUserNum + 1 < SBBR_USER_MAX - 1)
    {
        g_pstCpssSbbrSys->usUserNum++;

        snprintf(pstUser[g_pstCpssSbbrSys->usUserNum].acName, 32, "WatchDogHookUser");
        pstUser[g_pstCpssSbbrSys->usUserNum].Hook = NULL;
        pstUser[g_pstCpssSbbrSys->usUserNum].pucAddrStart = pstUser[g_pstCpssSbbrSys->usUserNum - 1].pucAddrStart + pstUser[g_pstCpssSbbrSys->usUserNum - 1].lSize;
        pstUser[g_pstCpssSbbrSys->usUserNum].lSize = 10 * 1024;
        pstUser[g_pstCpssSbbrSys->usUserNum].lMin = pstUser[g_pstCpssSbbrSys->usUserNum].lCurWriteLoc = 0;
        g_ulCpssWatchDogHookUserId = g_pstCpssSbbrSys->usUserNum;

    }
 
    cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, "cpss_sbbr_user_init()-->task user finished\n\r");

}

/*******************************************************************************
* 函数名称: cpss_sbbr_get_sched_userid
* 功    能: 获得当前调度任务的user id
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 
* 函数返回: 
*     usedId
*     CPSS_TD_INVALID                        
* 说   明:该函数提供给高层使用,返回的id总为0,保证异常发生时,用户全写入到异常区中.                                        
*******************************************************************************/

UINT32 cpss_sbbr_get_userid(VOID)
{
    return (cpss_vos_task_desc_self() + g_ulCpssSbbrTaskUserBase);
}

/*******************************************************************************
* 函数名称: cpss_sbbr_register
* 功    能: SBBR注册函数
*        
* 函数类型: G1
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* pfWriteHook    CPSS_SBBR_HOOK_PF    IN                hook函数
* 函数返回: 
*     0
*     CPSS_TD_INVALID                        
* 说   明:该函数提供给高层使用,返回的id总为0,保证异常发生时,用户全写入到异常区中.                                        
*******************************************************************************/

UINT32 cpss_sbbr_register(CPSS_SBBR_HOOK_PF pfWriteHook)
{
    UINT32 usLoop;

#ifdef CPSS_VOS_VXWORKS
    /* 在中断函数中不允许调用 */
    if ((TRUE == intContext()) || (NULL == pfWriteHook))
    {
        return CPSS_TD_INVALID;
    }
#else
    if (NULL == pfWriteHook)
    {
        return CPSS_TD_INVALID;
    }
#endif

#ifndef CPSS_DSP_CPU
    /* 存放注册的函数需要互斥 */
    cpss_vos_sem_p(g_ulCpssSbbrMutex, WAIT_FOREVER);

    for (usLoop = 0; usLoop < CPSS_SBBR_MAX_HOOK_NUM; usLoop++)
    {
        /* 该hook已被注册 */
        if (g_astCpssSbbrHookArray[usLoop] == pfWriteHook)
        {

            cpss_vos_sem_v(g_ulCpssSbbrMutex);
            return 0;
        }
    }

    /* 寻找空闲的位置 */
    for (usLoop = 0; usLoop < CPSS_SBBR_MAX_HOOK_NUM; usLoop++)
    {
        if (g_astCpssSbbrHookArray[usLoop] == NULL)
        {
            g_astCpssSbbrHookArray[usLoop] = pfWriteHook;
            break;
        }
    }
    cpss_vos_sem_v(g_ulCpssSbbrMutex);

    if (usLoop >= CPSS_SBBR_MAX_HOOK_NUM)
    {
        return CPSS_TD_INVALID;
    }
    else
    { 
        return CPSS_SBBR_MAX_HOOK_NUM - 1;
    }

#else
    return CPSS_TD_INVALID;
#endif

}

/*******************************************************************************
* 函数名称: cpss_sbbr_satisfy_auto_transfer
* 功    能: 检查是否满足上传
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
*
* 函数返回: 
*     TRUE
*     FALSE
*                          
* 说   明:
*******************************************************************************/

BOOL cpss_sbbr_satisfy_auto_transfer(VOID)
{

    if (g_bCpssFirstPowerOn == TRUE)
    {
        return FALSE;
    }
    /* 如果异常重起或设置了自动上传 */
    if ((TRUE == cpss_sbbr_abnormal_restart()) || (g_pstCpssSbbrSys->ucTransfer == CPSS_SBBR_TRANSACTION_AUTO))
    {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, "cpss_sbbr_satisfy_auto_transfer() return TRUE\n\r");

        return TRUE;
    }
    /* 否则 */
    return FALSE;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_get_drv_info
* 功    能: WIN下获取黑匣子信息
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
*
* 函数返回: 
*     CPSS_OK
*     CPSS_ERROR
*                          
* 说   明: 该函数在cpss_init()中被调用.
*******************************************************************************/

INT32 cpss_sbbr_get_drv_info(UINT32 * pulAddr, UINT32 * ulSize)
{
#ifndef CPSS_DSP_CPU
    UINT32 *p;

    if ((UINT32 *) * pulAddr != NULL)
    {
        return CPSS_OK;
    }
    p = malloc(CPSS_SBBR_WIN_SIZE);
    if (NULL == p)
        return CPSS_ERROR;
    (* pulAddr) = (UINT32 *)p;
    *ulSize = CPSS_SBBR_WIN_SIZE;

    memset(p, 0, CPSS_SBBR_WIN_SIZE);
#endif
    return CPSS_OK;

}

/*******************************************************************************
* 函数名称: cpss_sbbr_init
* 功    能: SBBR初始化函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
*
* 函数返回: 
*     CPSS_OK
*     CPSS_ERROR
*                          
* 说   明: 该函数在cpss_init()中被调用.
*******************************************************************************/

INT32 cpss_sbbr_init(VOID)
{
#ifdef CPSS_VOS_VXWORKS
    g_bCpssTaskCreateEnd = TRUE;
#endif
    /* 创建互斥量 */
    g_ulCpssSbbrMutex = cpss_vos_sem_m_create(VOS_SEM_Q_FIFO);

    if (g_ulCpssSbbrMutex == VOS_SEM_DESC_INVALID)
    {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_FATAL, "cpss_sbbr_init()create mutex fail!!!!!\n\r");
        return CPSS_ERROR;
    }

    g_pcSbbrExcInfo = cpss_mem_malloc(CPSS_SBBR_EXC_BUF_LENGTH);

    if (NULL == g_pcSbbrExcInfo)
    {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_FATAL, "cpss_sbbr_init()malloc fail!!!!!\n\r");
        return CPSS_ERROR;

    }
    else
    {
        cpss_mem_memset(g_pcSbbrExcInfo, 0, CPSS_SBBR_EXC_BUF_LENGTH);
    }
    /* 读取sbbr的配置数据 */
#ifdef CPSS_VOS_VXWORKS
    if (CPSS_OK != drv_sbb_info_get((UINT32 *) & g_pucCpssSbbrStartAddr, (UINT32 *) & g_ulCpssSbbrSize))
#endif
#ifdef CPSS_VOS_WINDOWS
        if (CPSS_OK != cpss_sbbr_get_drv_info((UINT32 *) & g_pucCpssSbbrStartAddr, (UINT32 *) & g_ulCpssSbbrSize))
#endif

#ifdef CPSS_VOS_LINUX
        if (CPSS_OK != cpss_sbbr_get_drv_info((UINT32 *) & g_pucCpssSbbrStartAddr, (UINT32 *) & g_ulCpssSbbrSize))
#endif
        {
            cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_FATAL, "!!!drv_sbb_info_get() fail!!!!!\n\r");
            return CPSS_ERROR;
        }

    /* 指向系统管理区 */
    g_pstCpssSbbrSys = (CPSS_SBBR_SYS_T *) (g_pucCpssSbbrStartAddr + CPSS_SBBR_DRV_SECT_SIZE);
    /* 清管理结构 */
    cpss_mem_memset(&g_stCpssSbbrLocalMng, 0, sizeof(CPSS_SBBR_LOCAL_MNG_T));
    /* 记录系统管理区地址 */
    g_stCpssSbbrLocalMng.pstSys = g_pstCpssSbbrSys;
    /* 第一次上电 */
    /* clear dsp trans buf */
    memset(&g_tCpssSaveDspBuf[0], 0, sizeof(CPSS_SAVE_DSP_BUF_T) * CPSS_COM_MAX_CPU);

    if (TRUE == cpss_sbbr_first_power_on())
    {
        g_bCpssFirstPowerOn = TRUE;
        /* 初始化系统管理区 */
        cpss_sbbr_init_sys_buf();

    }
    else
    {                           /* 发现异常重起或设置了自动上传,则等候在调试管理纤程中上传 */
        /* 传送完毕后,再调用cpss_sbbr_user_init() */
        if (TRUE == cpss_sbbr_satisfy_auto_transfer())
        {
            /* 保存旧的标志,传送完后再恢复 */
            g_ucCpssSbbrOldWriteFlag = g_pstCpssSbbrSys->ucWriteAble;
            /* 设置写禁止 */
            g_pstCpssSbbrSys->ucWriteAble = CPSS_SBBR_WRITE_DISABLE;

            g_bCpssSbbrExcOccur = TRUE;

            return CPSS_OK;

        }
        else                    /* 正常重起 */
        {
            /* 等候继续写入 */
            /* return CPSS_OK; */

        }

    }
    /* 初始化用户区 */
    cpss_sbbr_user_init();
    /* 设置监视异常重起标志 */
    cpss_sbbr_set_abnormal_reset_flag();
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_config
* 功    能: 配置sbbr
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
*
* 函数返回: 
*   
*                          
* 说   明: 该函数在cpss_init()中被调用.
*******************************************************************************/

VOID cpss_sbbr_config(UINT8 ucTrans, UINT8 ucWrite)
{
    g_pstCpssSbbrSys->ucTransfer = ucTrans;
    g_pstCpssSbbrSys->ucWriteAble = ucWrite;
}

/*******************************************************************************
* 函数名称: cpss_drv_wd_callback
* 功    能: 驱动回调函数
*        
* 函数类型: G1
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 
* 函数返回: 
*  
*                          
* 说   明: 
*******************************************************************************/
#ifdef CPSS_VOS_VXWORKS
extern UINT32 g_ulCpssKwDogCount;
extern UINT32 g_ulCpssKwDogCountEnd;
#endif
INT32 cpss_drv_wd_callback(VOID)
{
    UINT32 ulArgs[4];
    UINT32 ulusage = 0;

    /* 用户区未初始化 */
    if (g_ulCpssWatchDogHookUserId == 0)
    {
        return CPSS_ERROR;
    }

    cpss_sbbr_write_text(g_ulCpssWatchDogHookUserId, ulArgs, "\n\r  tick :%d  gmt :%d drv reset board ", cpss_high_precision_counter_get(), cpss_gmt_get());
#ifdef CPSS_VOS_VXWORKS
    cpss_cpu_usage_get(&ulusage);
    cpss_sbbr_write_text(g_ulCpssWatchDogHookUserId, ulArgs, "\n\r cpu load : %d  feed watchDog count: %d g_ulCpssKwDogCount %d  g_ulCpssKwDogCountEnd %d ", ulusage, g_ulCpssFeedDogCount, g_ulCpssKwDogCount, g_ulCpssKwDogCountEnd);
#endif
    cpss_sbbr_output_task_status(g_ulCpssWatchDogHookUserId);
    cpss_sbbr_output_task_msg_queue(g_ulCpssWatchDogHookUserId);
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_write_text
* 功    能: 记录文本调用接口
*        
* 函数类型: G1
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* ulUserId            UINT32            IN              用户id
* ulArgs[4]           UINT32            IN              用户参数
* szFormat           STRING             IN              格式化字符串
* 函数返回: 
*   CPSS_OK
*   CPSS_ERROR
*                          
* 说   明: 
*******************************************************************************/

INT32 cpss_sbbr_write_text(UINT32 ulUserId, UINT32 ulArgs[4], STRING szFormat, ...)
{
#ifndef CPSS_DSP_CPU
    INT32 lSize;
    va_list FmtList;
    CHAR azSbbrTextBuffer[CPSS_SBBR_TEXT_BUFFER_SIZE];

    if (g_pucCpssSbbrStartAddr == NULL)
    {
        return CPSS_ERROR;
    }

    /* hook user */
    if (ulUserId == CPSS_SBBR_MAX_HOOK_NUM - 1)
    {
        ulUserId = g_ulCpssSbbrHookUserId;
    }

    /* 如果不允许写.表明正在上传,则返回失败 */
    if ((ulUserId > g_pstCpssSbbrSys->usUserNum) || (g_pstCpssSbbrSys->ucWriteAble == CPSS_SBBR_WRITE_DISABLE))
    {
        return CPSS_ERROR;
    }
#if 0
    /* 任务用户 */
    if (ulUserId > g_ulCpssSbbrTaskUserBase)
    {
        /* 存储文本的全局量g_ucSbbrTextBuffer需要互斥 */
        if (cpss_vos_sem_p(g_ulCpssSbbrMutex, WAIT_FOREVER) != CPSS_OK)
        {
            return CPSS_ERROR;
        }
    }
#endif
    va_start(FmtList, szFormat);
    lSize = vsnprintf(azSbbrTextBuffer, CPSS_SBBR_TEXT_BUFFER_SIZE, szFormat, FmtList) + 1;
    va_end(FmtList);
    /* 记录到g_ucSbbrTextBuffer中 */
    cpss_sbbr_record(ulUserId, CPSS_SBBR_RECORED_TYPE_TEXT, ulArgs, azSbbrTextBuffer, lSize);
#if 0
    if (ulUserId > g_ulCpssSbbrTaskUserBase)
    {
        cpss_vos_sem_v(g_ulCpssSbbrMutex);
    }
#endif
#endif
    return CPSS_OK;

}

/*******************************************************************************
* 函数名称: cpss_sbbr_write_data
* 功    能: 记录文本调用接口
*        
* 函数类型: G1
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* ulUserId            UINT32            IN              用户id
* ulArgs[4]           UINT32            IN              用户参数
* pucData             UINT8 *           IN              数据区指针
* ulLen               UINT32            IN              数据区长度
* 函数返回: 
*   CPSS_OK
*   CPSS_ERROR
*                          
* 说   明: 
*******************************************************************************/

INT32 cpss_sbbr_write_data(UINT32 ulUserId, UINT32 ulArgs[4], UINT8 * pucData, UINT32 ulLen)
{
#ifndef CPSS_DSP_CPU
    if (g_pucCpssSbbrStartAddr == NULL)
    {
        return CPSS_ERROR;
    }

    /* hook user */
    if (ulUserId == CPSS_SBBR_MAX_HOOK_NUM - 1)
    {
        ulUserId = g_ulCpssSbbrHookUserId;
    }

    /* 如果不允许写.表明正在上传,则返回失败 */
    if ((ulUserId > g_pstCpssSbbrSys->usUserNum) || (g_pstCpssSbbrSys->ucWriteAble == CPSS_SBBR_WRITE_DISABLE))
    {
        return CPSS_ERROR;
    }
    /* 参数检查 */
    if ((pucData == NULL) || (0 == ulLen))
    {
        return CPSS_ERROR;
    }

    cpss_sbbr_record(ulUserId, CPSS_SBBR_RECORED_TYPE_DATA, ulArgs, pucData, ulLen);
#endif
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_write_decode_data
* 功    能: 记录需要ldt解码的数据
*        
* 函数类型: G1
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* ulUserId            UINT32            IN              用户id
* ulArgs[4]           UINT32            IN              用户参数
* pucData             UINT8 *           IN              数据区指针
* ulLen               UINT32            IN              数据区长度
* 函数返回: 
*   CPSS_OK
*   CPSS_ERROR
*                          
* 说   明: 
*******************************************************************************/

INT32 cpss_sbbr_write_decode_data(UINT32 ulUserId, UINT32 ulArgs[4], UINT8 * pucData, UINT32 ulLen)
{
#ifndef CPSS_DSP_CPU
    if (g_pucCpssSbbrStartAddr == NULL)
    {
        return CPSS_ERROR;
    }

    /* hook user */
    if (ulUserId == CPSS_SBBR_MAX_HOOK_NUM - 1)
    {
        ulUserId = g_ulCpssSbbrHookUserId;
    }

    /* 如果不允许写.表明正在上传,则返回失败 */
    if ((ulUserId > g_pstCpssSbbrSys->usUserNum) || (g_pstCpssSbbrSys->ucWriteAble == CPSS_SBBR_WRITE_DISABLE))
    {
        return CPSS_ERROR;
    }
    /* 参数检查 */
    if ((pucData == NULL) || (0 == ulLen))
    {
        return CPSS_ERROR;
    }
    if (ulLen > CPSS_SBBR_DECODE_DATA_MAX_LEN)
    {
        ulLen = CPSS_SBBR_DECODE_DATA_MAX_LEN;
    }
    cpss_sbbr_record(ulUserId, CPSS_SBBR_RECORD_TYPE_DECODE_DATA, ulArgs, pucData, ulLen);
#endif
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_record
* 功    能: 记录文本或数据到SBBR
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* ulUserId            UINT32            IN              用户id
* ucType              UINT8             IN              记录类型
* ulArgs[4]           UINT32            IN              用户参数
* pucData             UINT8 *           IN              数据区指针
* ulLen               UINT32            IN              数据区长度
* 函数返回: 
*   CPSS_OK
*   CPSS_ERROR
*                          
* 说   明: 
*******************************************************************************/

INT32 cpss_sbbr_record(UINT32 ulUserId, UINT8 ucType, UINT32 ulArgs[4], CHAR * pucData, UINT32 ulSize)
{
    UINT32 lLen;
    INT32 lLenAlign;
    CPSS_SBBR_RECORD_T *pstRecord;
    unsigned char *pcSectAddr;
    INT32 lSectSize;
    INT32 lMin;
    INT32 lNow;
#ifdef CPSS_VOS_WINDOWS
    UINT32 ulTime;
    time(&ulTime);
#endif


#ifdef CPSS_VOS_LINUX
    UINT32 ulTime;
    time(&ulTime);
#endif

    if (ulSize >= CPSS_SBBR_MAX_RECORD_LEN)
    {
        return CPSS_ERROR;
    }

    lLen = CPSS_SBBR_RECORD_HEAD_SIZE + ulSize;
    lLenAlign = CPSS_SBBR_ALIGN(lLen);

    /* 如果发生了异常 */

  /**
  if(g_bCpssSbbrExcOccur ==TRUE)
    {
       ulUserId = 0;
    }
  **/
    /* 已经向黑匣子异常区写过数据,则返回 */
    if ((0 == ulUserId) && (g_stCpssSbbrLocalMng.ucExecActFlag))
    {
        return CPSS_OK;

    }
    pcSectAddr = g_pstCpssSbbrSys->astUser[ulUserId].pucAddrStart;
    /* 管理区异常,停止写入 */
    if ((pcSectAddr < g_pstCpssSbbrSys->pucSbbrStartAddr) || (pcSectAddr > g_pstCpssSbbrSys->pucSbbrStartAddr + g_pstCpssSbbrSys->ulSBBRTotalLen))
    {
        return CPSS_ERROR;
    }
    lSectSize = g_pstCpssSbbrSys->astUser[ulUserId].lSize;
    if ((lLenAlign + 4) >= (UINT32) lSectSize)
    {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_ERROR, " sbbr User size %d ready write size %d\n\r", lSectSize, ulSize);
        return CPSS_ERROR;
    }
    lMin = g_pstCpssSbbrSys->astUser[ulUserId].lMin;
    lNow = g_pstCpssSbbrSys->astUser[ulUserId].lCurWriteLoc;
 
    /* 发生异常，清用户区 */
    if ((lMin >= lSectSize) || (lNow >= lSectSize))
    {
        lMin = (g_pstCpssSbbrSys->astUser[ulUserId].lMin = 0);
        lNow = (g_pstCpssSbbrSys->astUser[ulUserId].lCurWriteLoc = 0);
        *(unsigned short *)&pcSectAddr[lMin] = 0;
    }

    /* Move `lMin' to make room for current record. */
  again:
    if (lMin >= lNow)
    {
        while (lMin - lNow < lLenAlign + 4) /* Why add 2? Because we will set the new record_now's length to 0, so two more bytes are needed. */
        {

            INT32 lMinLen = *(unsigned short *)&pcSectAddr[lMin];

            if (lMinLen == 0)
            {
                lMin = 0;
                break;
            }
            lMin = CPSS_SBBR_ALIGN(lMin + lMinLen);
        }
    }
    if (lMin == 0)
    {
        if (lSectSize - lNow < lLenAlign + 4)   /* Why add 2? See above. */
        {
            lNow = 0;
            goto again;
        }
    }

    /* Now we have enough free space to store the record. */
    pstRecord = (CPSS_SBBR_RECORD_T *) & pcSectAddr[lNow];
    pstRecord->ulLength = lLen;
    pstRecord->usSeqNum = g_pstCpssSbbrSys->usSeqNumNow++;
#ifdef CPSS_VOS_VXWORKS
    pstRecord->ulTimeStamp = tickGet();
#endif
#ifdef CPSS_VOS_WINDOWS
    pstRecord->ulTimeStamp = ulTime;
#endif

#ifdef CPSS_VOS_LINUX
    pstRecord->ulTimeStamp = ulTime;
#endif

    pstRecord->ulArgs[0] = ulArgs[0];
    pstRecord->ulArgs[1] = ulArgs[1];
    pstRecord->ulArgs[2] = ulArgs[2];
    pstRecord->ulArgs[3] = ulArgs[3];
    pstRecord->ucUser = ulUserId;
    pstRecord->usType = ucType;

    cpss_mem_memcpy(pstRecord->aucInfo, pucData, ulSize);

    /* Set record_now's length to 0, that's why we add 2. */
    lNow = CPSS_SBBR_ALIGN(lNow + lLen);
    *(unsigned short *)&pcSectAddr[lNow] = 0;

    if (*(unsigned short *)&pcSectAddr[lMin] == 0)
        lMin = 0;

    /* Update the SBBR System Section attributes. */
    g_pstCpssSbbrSys->astUser[ulUserId].lMin = lMin;
    g_pstCpssSbbrSys->astUser[ulUserId].lCurWriteLoc = lNow;

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_data_get
* 功    能: 申请临时缓冲区,读SBBR数据到临时缓冲区
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* pvAddr             VOID**             OUT             临时缓冲区的地址              
* pulSize            INT32 *            OUT             临时缓冲区的长度(黑匣子的长度)
* 函数返回: 
*   CPSS_OK
*   CPSS_ERROR
*                          
* 说   明: 
*******************************************************************************/

INT32 cpss_sbbr_data_get(VOID ** pvAddr, INT32 * pulSize)
{

    cpss_vos_sem_p(g_ulCpssSbbrMutex, WAIT_FOREVER);

    /* Allocate space for SBBR Data of last running period. */
    *pulSize = g_pstCpssSbbrSys->astUser[g_pstCpssSbbrSys->usUserNum].pucAddrStart + g_pstCpssSbbrSys->astUser[g_pstCpssSbbrSys->usUserNum].lSize - g_pstCpssSbbrSys->pucSbbrStartAddr;
    *pvAddr = cpss_mem_malloc(*pulSize);
    if (*pvAddr == NULL)
    {
        cpss_vos_sem_v(g_ulCpssSbbrMutex);
        return CPSS_ERROR;
    }

    /* Copy SBBR Data into the space for Transfer */
    cpss_mem_memcpy(*pvAddr, g_pucCpssSbbrStartAddr, *pulSize);

    cpss_vos_sem_v(g_ulCpssSbbrMutex);

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_transfer_end_proc
* 功    能: 上传结束后的处理
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*
*                          
* 说   明: 该函数在cpss_dbg_proc()中
*******************************************************************************/

VOID cpss_sbbr_clear_dsp_buf(UINT8 ucCpuNo)
{

    if ((ucCpuNo >= 5) && (ucCpuNo < CPSS_COM_MAX_CPU) && (g_tCpssSaveDspBuf[ucCpuNo].pTransBuf != NULL))
    {
        cpss_mem_free(g_tCpssSaveDspBuf[ucCpuNo].pTransBuf);
        g_tCpssSaveDspBuf[ucCpuNo].pTransBuf = NULL;
        g_tCpssSaveDspBuf[ucCpuNo].ulSize = 0;
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, "cpss clear dsp %d sbbr buf ok\n\r", ucCpuNo);
    }

}

/*******************************************************************************
* 函数名称: cpss_sbbr_have_more_dsp
* 功    能: 检查是否还有没有上传的dsp
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*
*                          
* 说   明: 该函数在cpss_dbg_proc()中
*******************************************************************************/

BOOL cpss_sbbr_have_more_dsp(UINT8 * pucCpuNo)
{
    UINT8 ucLoop;

    for (ucLoop = 5; ucLoop < CPSS_COM_MAX_CPU; ucLoop++)
    {
        if (g_tCpssSaveDspBuf[ucLoop].pTransBuf != NULL)
        {
            *pucCpuNo = ucLoop;
            cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, "cpss find  dsp %d sbbr buf need trans \n\r", *pucCpuNo);
            return TRUE;
        }
    }
    cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, "cpss  trans all dsp sbbr over\n\r");
    return FALSE;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_transfer_end_proc
* 功    能: 上传结束后的处理
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*
*                          
* 说   明: 该函数在cpss_dbg_proc()中
*******************************************************************************/
void cpss_sbbr_show_dsp_buf(VOID)
{
    UINT8 ucLoop;

    for (ucLoop = 5; ucLoop < CPSS_COM_MAX_CPU; ucLoop++)
    {
        if (g_tCpssSaveDspBuf[ucLoop].pTransBuf != NULL)
        {
            printf("dsp %d has sbbr ready to send \n\r", ucLoop);
        }
    }
}

VOID cpss_sbbr_trans_next_dsp(VOID)
{
    UINT8 ucCpuNo = 0;
    if (cpss_sbbr_have_more_dsp(&ucCpuNo) == TRUE)
    {

        cpss_sbbr_trans_dsp(ucCpuNo, g_tCpssSaveDspBuf[ucCpuNo].pTransBuf, g_tCpssSaveDspBuf[ucCpuNo].ulSize);
    }
}

VOID cpss_sbbr_transfer_end_proc(VOID)
{
    /* cpss_sbbr_init_sys_buf(); */
    /* 初始化异常用户区 */
    /* 如果传递完dsp .则返回 */
    UINT8 ucCpuNo = 0;

    if (g_stCpssSbbrLocalMng.ucRsv[0] >= 5)
    {
        cpss_sbbr_clear_dsp_buf(g_stCpssSbbrLocalMng.ucRsv[0]);

        g_stCpssSbbrLocalMng.ucRsv[0] = 0;

        if (cpss_sbbr_have_more_dsp(&ucCpuNo) == TRUE)
        {
            cpss_timer_set(CPSS_TIMER_10, CPSS_SBBR_WAIT_NEXT_DSP_TRANS_LENGTH);

        }
        return;
    }
    cpss_sbbr_exception_user_init();
    /* 初始化用户区 */
    cpss_sbbr_user_init();
    /* 上传完毕,设置写允许 */
    g_pstCpssSbbrSys->ucWriteAble = CPSS_SBBR_WRITE_ENABLE;
    /* 记录序号清0 */
    g_pstCpssSbbrSys->usSeqNumNow = 0;
    /* 设置监视异常重起标志 */
    cpss_sbbr_set_abnormal_reset_flag();
    /* 清除上次异常标识 */
    g_bCpssSbbrExcOccur = FALSE;

    if (cpss_sbbr_have_more_dsp(&ucCpuNo) == TRUE)
    {
        cpss_timer_set(CPSS_TIMER_10, CPSS_SBBR_WAIT_NEXT_DSP_TRANS_LENGTH);

    }
}

/*******************************************************************************
* 函数名称: cpss_sbbr_send_to_oams
* 功    能: 发送数据到主控日志管理纤程
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* ulMsgId             UINT32           IN               消息ID
* pucData             UINT8 *          IN               数据区地址
* ulSize              UINT32           IN               数据区长度
* 函数返回: 
*  CPSS_OK
*  CPSS_ERROR
*                          
* 说   明:  
*******************************************************************************/

INT32 cpss_sbbr_send_to_oams(UINT32 ulMsgId, UINT8 * pucData, UINT32 ulSize)
{
    CPSS_COM_PID_T stDstPid;

    /* 主控板逻辑地址 */
    stDstPid.stLogicAddr = g_stLogicAddrGcpa;
    /* 发送到主用侧 */
    stDstPid.ulAddrFlag = 0;
    /* 日志管理纤程 */
    stDstPid.ulPd = CPS__OAMS_PD_M_LOGM_PROC;
    /* 调用通讯发送 */
#ifndef CPSS_FUNBRD_MC
    return (cpss_com_send(&stDstPid, ulMsgId, pucData, ulSize));
#else
    /* 如果是备用,采用send_mate发送 */
    if (TRUE != cpss_com_local_is_mmc())
    {
        return (cpss_com_send_mate(CPS__OAMS_PD_M_LOGM_PROC, ulMsgId, pucData, ulSize));
    }
#endif
    return (cpss_com_send(&stDstPid, ulMsgId, pucData, ulSize));

}

/*******************************************************************************
* 函数名称: cpss_sbbr_save_trans_data_to
* 功    能: 存放SBBR数据到缓冲区
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* pBuf                UINT8 *         IN                缓冲区地址
* DataLen             UINT32 *        OUT               缓冲区长度
* 函数返回: 
*  
*                          
* 说   明:  
*******************************************************************************/

VOID cpss_sbbr_save_trans_data_to(UINT8 * pBuf, UINT32 * DataLen)
{
    /* CPSS_SBBR_SYS_T *pstSys = g_stCpssSbbrLocalMng.pstSys; */
    /* 指向SBBR首地址 */
    /* UINT8 *pucSbbrStartAddr = pstSys->pucSbbrStartAddr; */

    UINT8 *pucSbbrStartAddr = g_stCpssSbbrLocalMng.pucTransBuf;
    /* 中间 */
    if ((g_stCpssSbbrLocalMng.ulCurOffset + CPSS_SBBR_TRANS_SIZE) < g_stCpssSbbrLocalMng.ulTransSize)
    {
        cpss_mem_memcpy(pBuf, pucSbbrStartAddr + g_stCpssSbbrLocalMng.ulCurOffset, CPSS_SBBR_TRANS_SIZE);
        /* 每次传送的长度 */
        *DataLen = CPSS_SBBR_TRANS_SIZE;
        /* 当前传送的总长度+=CPSS_SBBR_TRANS_SIZE */
        g_stCpssSbbrLocalMng.ulCurOffset += CPSS_SBBR_TRANS_SIZE;
    }                           /* 结束 */
    else if (g_stCpssSbbrLocalMng.ulCurOffset >= g_stCpssSbbrLocalMng.ulTransSize)
    {
        *DataLen = 0;

    }
    else                        /* 最后 一包 */
    {
        *DataLen = g_stCpssSbbrLocalMng.ulTransSize - g_stCpssSbbrLocalMng.ulCurOffset;

        cpss_mem_memcpy(pBuf, pucSbbrStartAddr + g_stCpssSbbrLocalMng.ulCurOffset, *DataLen);
        g_stCpssSbbrLocalMng.ulCurOffset = g_stCpssSbbrLocalMng.ulTransSize;

    }

}

/*******************************************************************************
* 函数名称: cpss_sbbr_trans_data
* 功    能: 传送SBBR数据
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* 函数返回: 
*  
*                          
* 说   明:  
*******************************************************************************/

VOID cpss_sbbr_trans_data(VOID)
{
    CPSS_SBBR_REQ_MSG_T stSbbrReqMsg;

    if (g_stCpssSbbrLocalMng.pucTransBuf != NULL)
    {
        memset(&stSbbrReqMsg, 0, sizeof(CPSS_SBBR_REQ_MSG_T));
        /* 填写本板物理地址 */
        cpss_com_phy_addr_get(&stSbbrReqMsg.stHead.stPhyAddr);
        if (g_stCpssSbbrLocalMng.ucRsv[0] >= 5)
        {
            stSbbrReqMsg.stHead.stPhyAddr.ucCpu = g_stCpssSbbrLocalMng.ucRsv[0];
        }
        /* 填写功能板类型 */
        stSbbrReqMsg.stHead.ucBoardType = SWP_FUNBRD_TYPE;
        stSbbrReqMsg.stHead.ulInfoLen = 0;
        /* 填写当前偏移 */
        stSbbrReqMsg.stHead.ulOffset = g_stCpssSbbrLocalMng.ulCurOffset;
        /* 流水号,固定填 0 */
        stSbbrReqMsg.stHead.ulSeqID = 0;
        /* SBBR的总长度 */
        stSbbrReqMsg.stHead.ulTotalLen = g_stCpssSbbrLocalMng.ulTransSize;
        /* 填充本次上传的数据和长度 */
        cpss_sbbr_save_trans_data_to(stSbbrReqMsg.aucData, &stSbbrReqMsg.stHead.ulInfoLen);
        /* 字节序转换 */
        stSbbrReqMsg.stHead.ulInfoLen = cpss_htonl(stSbbrReqMsg.stHead.ulInfoLen);
        stSbbrReqMsg.stHead.ulOffset = cpss_htonl(stSbbrReqMsg.stHead.ulOffset);
        stSbbrReqMsg.stHead.ulTotalLen = cpss_htonl(stSbbrReqMsg.stHead.ulTotalLen);
        /* 上传完毕发送到OAMS */
        if (stSbbrReqMsg.stHead.ulInfoLen != 0)
        {
            cpss_sbbr_send_to_oams(CPSS_SBBR_REQ_MSG, (UINT8 *) & stSbbrReqMsg, sizeof(CPSS_SBBR_REQ_MSG_T));
        }
        else
        {
            cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, "cpss_sbbr_trans_data() detect upload finished\n\r");

        }
        cpss_timer_loop_set(CPSS_SBBR_WAITRSP_TIMER_NO, CPSS_SBBR_WAITRSP_TIMER_LENGTH);
    }

}

/*******************************************************************************
* 函数名称: cpss_sbbr_save_dsp
* 功    能: DSP故障时调用,保留同时故障的dsp sbbr。
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描?
*ucCpuNo                                                IN
*pcAddr                                                  IN
*ulSize                                                    IN
* 函数返回: 
*  
*                          
* 说   明:  该函数在 cpss_dbg_proc()中收到激活消息后,被调用
*******************************************************************************/

INT32 cpss_sbbr_save_dsp(UINT32 ucCpuNo, UINT8 * pcAddr, UINT32 ulSize)
{

    if ((ucCpuNo >= 5) && (ucCpuNo < CPSS_COM_MAX_CPU))
    {
        if (g_tCpssSaveDspBuf[ucCpuNo].pTransBuf != NULL)
        {
            cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, " cpss save dsp %d fail ,this dsp just been reset  \n\r", ucCpuNo);
            return CPSS_ERROR;
        }
        g_tCpssSaveDspBuf[ucCpuNo].pTransBuf = cpss_mem_malloc(ulSize);
        if (g_tCpssSaveDspBuf[ucCpuNo].pTransBuf == NULL)
        {
            cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, " cpss save dsp fail\n\r");

            return CPSS_ERROR;
        }
        g_tCpssSaveDspBuf[ucCpuNo].ulSize = ulSize;
        cpss_mem_memcpy(g_tCpssSaveDspBuf[ucCpuNo].pTransBuf, pcAddr, ulSize);
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, " cpss save dsp %d  size %d  addr %p ok\n\r", ucCpuNo, ulSize, g_tCpssSaveDspBuf[ucCpuNo].pTransBuf);
        return CPSS_OK;

    }
    return CPSS_ERROR;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_trans_sbbr_data
* 功    能: DSP故障时调用,申请临时BUF,传送dsp SBBR数据
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描?
*ucCpuNo                                                IN
*pcAddr                                                  IN
*ulSize                                                    IN
* 函数返回: 
*  
*                          
* 说   明:  该函数在 cpss_dbg_proc()中收到激活消息后,被调用
*******************************************************************************/

INT32 cpss_sbbr_trans_dsp(UINT32 ucCpuNo, UINT8 * pcAddr, UINT32 ulSize)
{

    if ((pcAddr == NULL) || (ulSize == 0) || (ucCpuNo < 5))
    {
        return CPSS_ERROR;
    }

    if (g_stCpssSbbrLocalMng.pucTransBuf != NULL)
    {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, " \n\rcpss is processing sbbr ....");
        return cpss_sbbr_save_dsp(ucCpuNo, pcAddr, ulSize);
    }

    g_stCpssSbbrLocalMng.pucTransBuf = cpss_mem_malloc(ulSize);

    if (g_stCpssSbbrLocalMng.pucTransBuf == NULL)
    {
        return CPSS_ERROR;
    }

    cpss_mem_memcpy(g_stCpssSbbrLocalMng.pucTransBuf, pcAddr, ulSize);
    /* 设置传递dsp标识 */
    g_stCpssSbbrLocalMng.ucRsv[0] = (UINT8) ucCpuNo;
    g_stCpssSbbrLocalMng.ulCurOffset = 0;
    g_stCpssSbbrLocalMng.ulTransSize = ulSize;

    cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, " \n\r Begin trans dsp %d sbbr data  ", ucCpuNo);

    cpss_sbbr_start_transaction_dsp(ucCpuNo);
    /* cpss_sbbr_trans_data(); */

    return CPSS_OK;

}

/*******************************************************************************
* 函数名称: cpss_sbbr_trans_sbbr_data
* 功    能: DSP故障时调用,申请临时BUF,传送dsp SBBR数据
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描?
*ucCpuNo                                                IN
*pcAddr                                                  IN
*ulSize                                                    IN
* 函数返回: 
*  
*                          
* 说   明:  该函数在 cpss_dbg_proc()中收到激活消息后,被调用
*******************************************************************************/

VOID cpss_sbbr_trans_sbbr_data(VOID)
{
    if (CPSS_OK == cpss_sbbr_data_get((VOID *) & g_stCpssSbbrLocalMng.pucTransBuf, (INT32 *) & g_stCpssSbbrLocalMng.ulTransSize))
    {
        g_stCpssSbbrLocalMng.ucRsv[0] = 0;
        g_stCpssSbbrLocalMng.ulCurOffset = 0;
        cpss_sbbr_trans_data();
    }
}

/*******************************************************************************
* 函数名称: cpss_sbbr_rsp_msg_proc
* 功    能: 申请临时BUF,传送SBBR数据
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型                输入/输出         描述 
* pstRspMsg        CPSS_SBBR_RSP_MSG_T *   IN               OAMS的响应消息
* 函数返回: 
*  0
*  !0                        
* 说   明:  该函数在 cpss_dbg_proc()中收到oams的响应消息后被调用.
*******************************************************************************/

INT32 cpss_sbbr_rsp_msg_proc(CPSS_SBBR_RSP_MSG_T * pstRspMsg)
{
    INT32 lEnd;
    UINT32 ulExpectedOffset = cpss_ntohl(pstRspMsg->ulExpectedOffset);
    UINT32 ulTotalLen = cpss_ntohl(pstRspMsg->ulTotalLen);

    cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, "!Receive Rsp Msg offset = %d totallen = %d!\n\r", ulExpectedOffset, ulTotalLen);

    /* 清等待响应的计数 */
    g_ulCpssSbbrWaitRspCount = 0;

    /* 如果当前还没有启动上传返回传送结束 */
    if (g_stCpssSbbrLocalMng.pucTransBuf == NULL)
    {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_WARN, "sbbr upload is not start,quit\n\r");
        return CPSS_SBBR_TRANS_END;
    }
    /* 如果上传结束 */
    if (ulExpectedOffset >= ulTotalLen)
    {

        lEnd = CPSS_SBBR_TRANS_END;
    }
    else                        /* 否则 */
    {
        g_stCpssSbbrLocalMng.ulCurOffset = ulExpectedOffset;
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, "sbbr upload offset = %d, sbbr length = %d\n\r", g_stCpssSbbrLocalMng.ulCurOffset, g_stCpssSbbrLocalMng.ulTransSize);

        /* 继续上传 */
        cpss_sbbr_trans_data();
        lEnd = (g_stCpssSbbrLocalMng.ulTransSize - g_stCpssSbbrLocalMng.ulCurOffset);
    }
    /* 上传结束 */
    if (CPSS_SBBR_TRANS_END == lEnd)
    {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, "!SBBR Upload Trans End!\n\r");
        /* 释放申请的buf */
        cpss_mem_free(g_stCpssSbbrLocalMng.pucTransBuf);
        g_stCpssSbbrLocalMng.pucTransBuf = NULL;
        /* 清除上传长度,等待下一次 */
        g_stCpssSbbrLocalMng.ulCurOffset = 0;
        /* 删除周期定时器 */
        cpss_timer_delete(CPSS_SBBR_WAITRSP_TIMER_NO);

        /* 调用上传结束后的处理 */
        cpss_sbbr_transfer_end_proc();

    }
    else
    {
        /* 启动周期定时器检查响应超时次数 */
        /* cpss_timer_loop_set(CPSS_SBBR_WAITRSP_TIMER_NO,CPSS_SBBR_WAITRSP_TIMER_LENGTH); */

    }

    return lEnd;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_start_transaction
* 功    能: G1接口,供其它子系统向DBG纤程发送消息,启动上传
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型                输入/输出         描述 
* ulCpuNo            UINT32                 IN             指定上传cpu
* 函数返回: 
*  CPSS_OK
*  CPSS_ERROR                   
* 说   明:  
*******************************************************************************/

INT32 cpss_sbbr_start_transaction_dsp(UINT32 ulCpuNo)
{
    CPSS_COM_MSG_HEAD_T stData;

    stData.stDstProc.ulPd = CPSS_DBG_MNGR_PROC;
    stData.ulMsgId = CPSS_SBBR_DSP_TRANSACTION_MSG;
    stData.ulLen = sizeof(UINT32);
    stData.pucBuf = (UINT8 *) & ulCpuNo;
    stData.ucShareFlag = 0;
    stData.ucAckFlag = CPSS_COM_ACK;
    stData.ucPriFlag = CPSS_COM_PRIORITY_NORMAL;
    return (cpss_com_send_local(&stData));

}

INT32 cpss_sbbr_start_transaction(UINT32 ulCpuNo)
{

    CPSS_COM_MSG_HEAD_T stData;

    if (ulCpuNo <= 16)
    {
        stData.stDstProc.ulPd = CPSS_DBG_MNGR_PROC;
        stData.ulMsgId = CPSS_SBBR_TRANSACTION_MSG;
        stData.ulLen = sizeof(UINT32);
        stData.pucBuf = (UINT8 *) & ulCpuNo;
        stData.ucShareFlag = 0;
        stData.ucAckFlag = CPSS_COM_ACK;
        stData.ucPriFlag = CPSS_COM_PRIORITY_NORMAL;
        return (cpss_com_send_local(&stData));
    }
    else
    {
#if 0
        /* dsp */
        if ((ulCpuNo >= 5) && (ulCpuNo <= 16))
        {
            CPSS_COM_PID_T stDstPid;
            UINT32 ulDspNo = ulCpuNo;

            /* 获得dsp逻辑地址 */
            if (CPSS_OK != cpss_com_dsp_logic_addr_get((UINT8) ulDspNo, &stDstPid.stLogicAddr))
            {
                cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_WARN, " cpss_com_dsp_logic_addr_get fail in cpss_sbbr_start_transaction\n\r");

                return CPSS_ERROR;
            }

            stDstPid.ulAddrFlag = 0;
            stDstPid.ulPd = CPSS_DBG_MNGR_PROC;
            ulDspNo = cpss_htonl(ulDspNo);
            return (cpss_com_send(&stDstPid, CPSS_SBBR_TRANSACTION_MSG, (UINT8 *) & ulDspNo, sizeof(UINT32)));

        }
#endif
    }

    return CPSS_ERROR;

}

/*******************************************************************************
* 函数名称: cpss_sbbr_transation_proc
* 功    能: 收到启动上传的消息后的处理函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型                输入/输出         描述 
* ulCpuNo            UINT32                 IN             指定上传cpu
* 函数返回: 
*
* 说   明:  
*******************************************************************************/

VOID cpss_sbbr_transation_proc(UINT32 ulCpuNo)
{
#ifndef CPSS_DSP_CPU
    /* 传送host */
    if (ulCpuNo <= 2)
    {
        if (g_stCpssSbbrLocalMng.pucTransBuf != NULL)
        {
            /* 正在上传 */
            cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_WARN, "!SBBR is being upload!\n\r");

        }
        else
        {
            /* 设置写禁止 */
            g_pstCpssSbbrSys->ucWriteAble = CPSS_SBBR_WRITE_DISABLE;
            /* 开始上传 */
            cpss_sbbr_trans_sbbr_data();
        }
    }
    else                        /* trans dsp sbbr data */
    {
        if ((ulCpuNo >= 5) && (ulCpuNo <= 17))
        {
            if (g_stCpssSbbrLocalMng.pucTransBuf != NULL)
            {
                /* 正在上传 */
                cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_WARN, "! SBBR is being upload!\n\r");

            }
            else
            {
                g_stCpssSbbrLocalMng.pucTransBuf = cpss_mem_malloc(CPSS_DSP_SBBR_LEN);
                if (g_stCpssSbbrLocalMng.pucTransBuf == NULL)
                {
                    cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_WARN, " \n\rget dsp trans buf fail ");
                }
                else
                {
#ifdef CPSS_VOS_VXWORKS
#ifdef CPSS_HOST_CPU_WITH_DSP
                    cpss_sbbr_get_dsp(ulCpuNo, g_stCpssSbbrLocalMng.pucTransBuf, CPSS_DSP_SBBR_LEN);
#endif
#endif
                    cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_INFO, " \n\r Begin trans dsp %d sbbr data  ", ulCpuNo);
                    g_stCpssSbbrLocalMng.ucRsv[0] = (UINT8) ulCpuNo;
                    g_stCpssSbbrLocalMng.ulTransSize = CPSS_DSP_SBBR_LEN;
                    g_stCpssSbbrLocalMng.ulCurOffset = 0;
                    cpss_sbbr_trans_data();

                }
            }
        }
    }
#else                           /* 传送DSP */
    {
        UINT32 ulDspNo;

        ulDspNo = cpss_ntohl(ulCpuNo);

    }
#endif
}

/*******************************************************************************
* 函数名称: cpss_sbbr_hook_proc
* 功    能: 调用注册的hook
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型                输入/输出         描述 
* ulTidType           UINT32                IN              0 普通任务; 1 调度任务
* ulDesc              UINT32                IN              任务ID或纤程描述符
* 函数返回: 
*
* 说   明:  
*******************************************************************************/

/****
VOID cpss_sbbr_hook_proc
(
 UINT32 ulTidType,
 UINT32 ulDesc
)
{
  UINT16 usLoop ;
  for(usLoop = 0; usLoop < CPSS_SBBR_MAX_HOOK_NUM;usLoop++)
        {
            if(g_astCpssSbbrHookArray[usLoop]!= NULL)
                {
                   g_astCpssSbbrHookArray[usLoop](ulTidType,ulDesc);
                   
                }
        }
}

***/

/*******************************************************************************
* 函数名称: cpss_sbbr_record_data_code_segment
* 功    能: 记录717板的数据段和代码段
* 函数类型: 
* 参    数:              类型                      IN/OUT     描述
* pBuf                       CHAR*                       out           存储bootoem.out的数据段和代码段
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_sbbr_record_data_code_segment(CHAR * pBuf)
{
#ifdef CPSS_VOS_VXWORKS

    MODULE_ID pModuleId;
    MODULE_INFO tModuleInfo;

    if (pBuf == NULL)
        return;

#if ((SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA) ||(SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA))

    pModuleId = moduleFindByName(CPSS_MODULE_NAME);

    if (pModuleId == NULL)
        return;

    if (moduleInfoGet(pModuleId, &tModuleInfo) == CPSS_OK)
    {
        cpss_sbbr_sprintf(pBuf, "\n\r module name: %s   text addr: %p data addr: %p", CPSS_MODULE_NAME, tModuleInfo.segInfo.textAddr, tModuleInfo.segInfo.dataAddr);

    }

#endif

#endif
}

/*******************************************************************************
* 函数名称: cpss_sbbr_output_dead_info
* 功    能: 写死循环信息到SBBR
* 函数类型: 
* 参    数:              类型                      IN/OUT     描述
* ulTid                        UINT32                      IN           调度任务号
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_sbbr_output_dead_info(UINT32 ulTid)
{

    VK_SCHED_ID schedId;
    CPSS_COM_MSG_HEAD_T *pstData;
    UINT32 ulArgs[4];
    UINT32 ulLen;
    INT32 lTaskId;
    UINT32 ulTaskDesc;
    UINT16 usGuid;

    schedId = g_astCpssVkSchedDescTbl[ulTid].tSchedId;  /* 获得当前控制结构 */

    if (NULL == schedId)
    {
        return;
    }
    ulTaskDesc = schedId->ulTaskDesc;
    cpss_sbbr_record_data_code_segment(g_pcSbbrExcInfo);
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "\n\r**************** GMT %d ***********************\n\r", cpss_gmt_get());
    if (ulTaskDesc < VOS_MAX_TASK_NUM)
    {
        if (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == TRUE)
        {
            lTaskId = (INT32) g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;
#ifdef CPSS_VOS_VXWORKS
            cpss_sbbr_output_task_info(lTaskId);
#endif
        }
    }

    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "\n\r CPSS Moniter thread dead loop  Occur @ Tno%d\n\r", ulTid);
#ifdef CPSS_VOS_VXWORKS
    cpss_sbbr_tt(lTaskId);
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "  Fun Stack  Info start\n\n");
    {
        UINT16 usLoop;
        for (usLoop = 0; usLoop < g_lcurCallDep; usLoop++)
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n\r", g_azCpssSbbrTempBuf[usLoop]);
    }
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "  Fun Stack  Info End\n\n");
#endif

    /* 指向数据区 */
    pstData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA(schedId->pIpcMsgCurr);
    if (pstData != NULL)
    {
        usGuid = (UINT16) ((schedId->ulProcDescCurr >> 16) & 0x0000ffff);
        /* 打印当前处理的纤程，消息，消息地址，消息长度 */
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, " Current Thread Name :%s Current MsgId:0x%x Msg Addr %p,Length %d\n\r  ", g_astCpssVkProcClassTbl[usGuid].acName, pstData->ulMsgId, pstData->pucBuf, pstData->ulLen);
        /* 打印源逻辑地址 */
        usGuid = (UINT16) ((pstData->stSrcProc.ulPd >> 16) & 0x0000ffff);
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Source:  Module = %d SubGroup= %d  Group = %d AddrFlag = %d procName: %s\n\r", pstData->stSrcProc.stLogicAddr.ucModule, pstData->stSrcProc.stLogicAddr.ucSubGroup, pstData->stSrcProc.stLogicAddr.usGroup, pstData->stSrcProc.ulAddrFlag, g_astCpssVkProcClassTbl[usGuid].acName);
        /* 打印目的逻辑地址 */
        usGuid = (UINT16) ((pstData->stDstProc.ulPd >> 16) & 0x0000ffff);
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Dest:  Module = %d SubGroup= %d  Group = %d AddrFlag = %d procName: %s\n\r", pstData->stDstProc.stLogicAddr.ucModule, pstData->stDstProc.stLogicAddr.ucSubGroup, pstData->stDstProc.stLogicAddr.usGroup, pstData->stDstProc.ulAddrFlag, g_astCpssVkProcClassTbl[usGuid].acName);

        cpss_sbbr_sprintf(g_pcSbbrExcInfo, " Cpss Dead Loop write MsgInfo[] start:\n\r ");
        /* 写文本信息到sbbr */
        cpss_sbbr_write_text(0, ulArgs, "%s\n\r", g_pcSbbrExcInfo);
        /* 最多写入100个字节 */
        ulLen = pstData->ulLen;
        if (ulLen > CPSS_SBBR_WRITE_DATA_MAX_LEN)
        {
            ulLen = CPSS_SBBR_WRITE_DATA_MAX_LEN;
        }
        /* 写数据到sbbr */
        cpss_sbbr_write_data(0, ulArgs, pstData->pucBuf, ulLen);
        /* 写结束信息到sbbr */
        cpss_sbbr_write_text(0, ulArgs, "CPSS Dead Loop write MsgInfo[] end\n\r ");
#ifdef CPSS_VOS_VXWORKS

        cpss_mem_memset(g_pcSbbrExcInfo, 0, CPSS_SBBR_EXC_BUF_LENGTH);
        cpss_kw_save_all_task_load();

        cpss_taskRegsShow(lTaskId);

        /* 记录任务栈部分到sbbr */
        cpss_sbbr_write_task_stack_to_sbbr(lTaskId);
        /* 调用注册的hook */
        cpss_sbbr_hook_proc(lTaskId);
#endif
        /* 设置异常信息区已经写过标志,不能再写入 */
        cpss_sbbr_set_exc_flag(1);

    }

}

/*******************************************************************************
* 函数名称: cpss_sbbr_printf
* 功    能: 打印并存储
* 函数类型: 
* 参    数:                 类型                     IN/OUT                 描述
* pcBuf                          CHAR *                     OUT                    存放打印信息
* pcFmt                         CHAR *                    IN                        待打印的信息
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_sbbr_sprintf(CHAR * pcBuf, CHAR * pcFmt, ...)
{

    CHAR acBuf[500] = { 0 };
    va_list stArg;

    if ((pcBuf == NULL) || (pcFmt == NULL))
    {
        return;
    }
    va_start(stArg, pcFmt);
    vsnprintf(acBuf, 500, pcFmt, stArg);
    va_end(stArg);
    /* printf(acBuf); 由于硬件狗延迟只有3秒，所以关闭打印 */
    if ((strlen(pcBuf) + 1 + strlen(acBuf)) >= CPSS_SBBR_EXC_BUF_LENGTH)
    {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_FATAL, " write sbbr length overflow ,return \n\r");
        return;
    }
    if (NULL != g_pcSbbrExcInfo)
    {
        strcat(pcBuf, acBuf);
    }

    return;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_wait_rsp_expire_proc
* 功    能: 响应消息超时的处理
* 函数类型: 
* 参    数:                 类型                     IN/OUT                 描述
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_sbbr_wait_rsp_expire_proc(VOID)
{

    /* 如果等待响应次数超过CPSS_SBBR_WAIT_RSP_MAX_COUNT */
    if (++g_ulCpssSbbrWaitRspCount > CPSS_SBBR_WAIT_RSP_MAX_COUNT)
    {
        cpss_output(CPSS_MODULE_DBG, CPSS_PRINT_WARN, "Wait Rsp Timer Expire %d times,upload sbbr fail \n\r", CPSS_SBBR_WAIT_RSP_MAX_COUNT);
        /* 删除周期定时器 */
        cpss_timer_delete(CPSS_SBBR_WAITRSP_TIMER_NO);
        /* 如果有异常 */
        if (cpss_sbbr_abnormal_restart() == TRUE)
        {
            /* 设置写禁止 */
            g_pstCpssSbbrSys->ucWriteAble = CPSS_SBBR_WRITE_DISABLE;
        }
        else
        {                       /* 设置写允许 */
            g_pstCpssSbbrSys->ucWriteAble = CPSS_SBBR_WRITE_ENABLE;
        }
        /* 清偏移，等待重新上传 */
        g_stCpssSbbrLocalMng.ulCurOffset = 0;
        /* 释放申请的临时buf */
        if (g_stCpssSbbrLocalMng.pucTransBuf != NULL)
            cpss_mem_free(g_stCpssSbbrLocalMng.pucTransBuf);
        g_stCpssSbbrLocalMng.pucTransBuf = NULL;

    }
    else                        /* 继续重传原来的数据 */
    {
        cpss_sbbr_trans_data();
    }
}

#ifdef CPSS_VOS_VXWORKS

/*******************************************************************************
* 函数名称: cpss_sbbr_output_fun_stack
* 功    能: 输出函数调用栈
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* ulRetAddr                    UINT32                   IN               返回地址
* pulSFR                        UINT32 *                IN               当前 堆栈帧
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_sbbr_output_fun_stack(UINT32 ulRetAddr, UINT32 * pulSFR)
{
    CHAR acFunName[CPSS_MAX_SYM_NAME_LENGTH + 1];
    INT32 lValue;
    SYM_TYPE SymType;

    UINT32 *pulCurSFR;
    UINT32 ulCurRetAddr = ulRetAddr;
    UINT32 ulNestCount = 0;

    pulCurSFR = pulSFR;

    while (1)
    {
        /* 当前帧越界 */
        if ((pulCurSFR == NULL) || ((UINT32) pulCurSFR > (UINT32) sysMemTop()))
        {
            break;
        }
        /* 函数调用嵌套超过20个 */
        if (ulNestCount >= 20)
        {
            break;
        }
        /* 根据当前的返回地址在符号表中查找 */
        if (symFindByValue(sysSymTbl, ulCurRetAddr, acFunName, &lValue, &SymType) == CPSS_OK)
        {
            /* 保存当前查找到的函数地址和函数名 */
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, " Addr 0x%8x Name %s\n\r", ulCurRetAddr, acFunName);

        }
        /* 指向下一帧 */
        pulCurSFR = (UINT32 *) * pulCurSFR;
        /* 得到返回地址 */
        ulRetAddr = *(pulCurSFR + 1);

        ulNestCount++;

    }

}

/*******************************************************************************
* 函数名称: cpss_sbbr_is_simple_fun
* 功    能: 判断函数是否为简单函数
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* ulPC                    UINT32                   IN                      当前指令地址
* 函数返回: TRUE ，FALSE
* 说    明: 
*******************************************************************************/

#if (CPSS_CPU_TYPE == CPSS_CPU_PPC)
INT32 cpss_sbbr_is_simple_fun(UINT32 ulPC)
{
    UINT32 ulLoop;
    UINT32 ulInstruction;

    ulInstruction = *(UINT32 *) ulPC;

    for (ulLoop = 0; ulLoop < 200; ulLoop++)
    {
        if ((ulInstruction & 0xffff0000) == 0x94210000) /* 找到函数序言 */
        {
            return FALSE;
        }
        if (ulInstruction == 0x4e800020)    /* 找到函数尾声 */
        {
            return TRUE;
        }
        ulPC--;
        ulInstruction = *(UINT32 *) ulPC;
    }

    return FALSE;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_get_ppc_ret_addr
* 功    能: 获得函数的返回地址
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* ulPC                    UINT32                   IN                      当前指令地址
* pStackFrame        UINT32 *                 IN                     当前堆栈帧
* pulRet                 UINT32 *                 out                     返回地址
* 函数返回: TRUE ，FALSE
* 说    明: 
*******************************************************************************/

VOID cpss_sbbr_get_ppc_ret_addr(UINT32 ulPC, UINT32 * pStackFrame, UINT32 * pulRet)
{
    UINT32 ulCurInstruction;
    UINT32 *pulCurRet;
    UINT32 *pulCurSFR;
    UINT32 *pulTempPC = NULL;

    pulTempPC = (UINT32 *) ulPC;

    ulCurInstruction = *(UINT32 *) ulPC;
    /* 不是指令异常 */
    if ((g_stExcInfo.tEsf.vecOffset) != _EXC_OFF_INST)
    {
        /* 找到序言 */
        while ((ulCurInstruction & 0xffff0000) != 0x94210000)
        {
            pulTempPC--;

            if (pulTempPC == NULL)
            {
                return;
            }
            ulCurInstruction = *pulTempPC;

        }
        /* 检查序言后的语句是否是mfspr r0,lr */
        pulTempPC++;
        ulCurInstruction = *pulTempPC;

        if (ulCurInstruction == 0x7c0802a6)
        {
            *pulRet = g_stExcInfo.tRegSet.lr;
        }

        *pulRet = *(pStackFrame + 1);
    }
    else                        /* 指令异常,直接从栈中取返回地址 */
    {

        *pulRet = *(pStackFrame + 1);
    }

}

/*******************************************************************************
* 函数名称: cpss_sbbr_get_ppc_entry_addr
* 功    能: 获得函数的地址
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* ulPC                    UINT32                   IN                      当前指令地址
* ulRet                 UINT32 *                  IN                    返回地址
* 函数返回: TRUE ，FALSE
* 说    明: 
*******************************************************************************/

UINT32 cpss_sbbr_get_ppc_entry_addr(UINT32 ulPC, UINT32 ulRet)
{
    UINT32 *pulAddr = NULL;
    CHAR ucOptCode = 0;
    CHAR ucAA = 0;
    UINT32 ulAddr;
    UINT32 ulEntry;

    /* 返回地址的上一条指令是函数调用指令 */
    pulAddr = (UINT32 *) (ulRet - 1);
    ucOptCode = (*pulAddr & 0xfc000000) >> 26;
    ucAA = (*pulAddr & 0x2) >> 1;

    switch (ucOptCode)
    {
    case 18:                   /* 18: b ba bla bl指令的操作码 */
        ulAddr = *pulAddr & 0x03fffffc;
        if (ulAddr & 0x02000000)
        {
            ulAddr |= 0xfc000000;
        }
        if (ucAA == 0)          /* 相对地址 */
        {
            ulEntry = (UINT32) ((CHAR *) pulAddr + ulAddr);
        }
        else
        {
            ulEntry = ulAddr;
        }

        break;
    case 16:                   /* 16:bc ,bcl bca bcla指令的操作码 */

        ulAddr = (*pulAddr & 0xfffc);
        if (ulAddr & 0x8000)
        {
            ulAddr = (ulAddr | 0xffff0000);
        }

        if (ucAA == 0)
        {
            ulEntry = (UINT32) ((CHAR *) pulAddr + ulAddr);
        }
        else
        {

            ulEntry = ulAddr;
        }
        break;

    default:

        ulEntry = ulPC;
        break;
    }
    return ulEntry;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_output_ppc_fun_list
* 功    能: 输出函数调用栈
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* ulPC                    UINT32                   IN                      当前指令地址
* pStackFrame          UINT32 *                IN                    栈帧指针
* 函数返回: TRUE ，FALSE
* 说    明: 
*******************************************************************************/

VOID cpss_sbbr_output_ppc_fun_list(UINT32 lPC, UINT32 * pStackFrame)
{
    UINT32 ulCurRet;
    UINT32 *pulCurSFR = NULL;

    pulCurSFR = pStackFrame;

    if ((UINT32) pStackFrame > (UINT32) sysMemTop())
    {
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, " task stack has been destoryed\n\r");
        return;
    }

    if (cpss_sbbr_is_simple_fun(lPC) == TRUE)
    {

        CHAR acFunName[CPSS_MAX_SYM_NAME_LENGTH + 1];
        INT32 lValue;
        SYM_TYPE SymType;

        if (symFindByValue(sysSymTbl, lPC, acFunName, &lValue, &SymType) == CPSS_OK)
        {
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, " Addr 0x%8x Name %s\n\r", lPC, acFunName);
            ulCurRet = g_stExcInfo.tRegSet.lr;

            cpss_sbbr_output_fun_stack(ulCurRet, pulCurSFR);
        }

    }
    else
    {
        UINT32 ulEntry;
        cpss_sbbr_get_ppc_ret_addr(lPC, pStackFrame, &ulCurRet);
        ulEntry = cpss_sbbr_get_ppc_entry_addr(lPC, ulCurRet);
        cpss_sbbr_output_fun_stack(ulEntry, pStackFrame);
    }

}

#endif

/*******************************************************************************
* 函数名称: cpss_sbbr_output_x86_fun_list
* 功    能: 输出函数调用栈
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* ulPC                    UINT32                   IN                      当前指令地址
* pStackFrame          UINT32 *                IN                    栈帧指针
* 函数返回: TRUE ，FALSE
* 说    明: 
*******************************************************************************/

#if (CPSS_CPU_TYPE == CPSS_CPU_I86)

VOID cpss_sbbr_output_x86_fun_list(UINT32 lPC, UINT32 * pStackFrame)
{
    CHAR acFunName[CPSS_MAX_SYM_NAME_LENGTH + 1];
    INT32 lValue;
    SYM_TYPE SymType;
    UINT32 ulCurRet;
    UINT32 ulNestCount = 0;
    UINT32 *pulCurSFR = NULL;

    pulCurSFR = pStackFrame;

    if ((UINT32) pStackFrame > (UINT32) sysMemTop())
    {
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, " task stack has been destoryed\n\r");
        return;
    }

    if (symFindByValue(sysSymTbl, lPC, acFunName, &lValue, &SymType) == CPSS_OK)
    {
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, " Addr 0x%8x Name %s\n\r", lPC, acFunName);
        ulCurRet = *(pStackFrame + 1);

        while (1)
        {
            /* 当前帧越界 */
            if ((pulCurSFR == NULL) || ((UINT32) pulCurSFR > (UINT32) sysMemTop()))
            {
                break;
            }
            /* 函数调用嵌套超过20个 */
            if (ulNestCount >= 20)
            {
                break;
            }
            /* 根据当前的返回地址在符号表中查找 */
            if (symFindByValue(sysSymTbl, ulCurRet, acFunName, &lValue, &SymType) == CPSS_OK)
            {
                /* 保存当前查找到的函数地址和函数名 */
                cpss_sbbr_sprintf(g_pcSbbrExcInfo, " Addr 0x%8x Name %s\n\r", ulCurRet, acFunName);

            }
            /* 指向下一帧 */
            pulCurSFR = (UINT32 *) * pulCurSFR;

            if (NULL == pulCurSFR)
            {
                break;
            }
            /* 得到返回地址 */
            ulCurRet = *(pulCurSFR + 1);

            ulNestCount++;

        }

        /* cpss_sbbr_output_fun_stack(ulCurRet,pulCurSFR); */
    }

}
#endif

/*******************************************************************************
* 函数名称: cpss_sbbr_output_arm_fun_list
* 功    能: 输出函数调用栈
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* ulPC                    UINT32                   IN                      当前指令地址
* pStackFrame          UINT32 *                IN                    栈帧指针
* 函数返回: TRUE ，FALSE
* 说    明: 
*******************************************************************************/

#if (CPSS_CPU_TYPE == CPSS_CPU_ARM)

VOID cpss_sbbr_output_arm_fun_list(UINT32 lPC, UINT32 * pStackFrame)
{
    CHAR acFunName[CPSS_MAX_SYM_NAME_LENGTH + 1];
    INT32 lValue;
    SYM_TYPE SymType;

    UINT32 *pulCurSFR;
    UINT32 *pulNewSFR;

    UINT32 ulNestCount = 0;

    pulCurSFR = pStackFrame;

    while (1)
    {
        if ((pulCurSFR == NULL) || ((UINT32) pulCurSFR > (UINT32) sysMemTop()))
        {
            break;
        }
        if (ulNestCount >= 20)
        {
            break;
        }
        if (symFindByValue(sysSymTbl, *pulCurSFR, acFunName, &lValue, &SymType) == CPSS_OK)
        {
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, " Addr 0x%8x Name %s\n\r", *pulCurSFR, acFunName);

        }

        pulNewSFR = pulCurSFR;

        pulCurSFR = *(pulCurSFR - 3);

        if (pulCurSFR == NULL)
        {
            break;
        }
        ulNestCount++;

    }

    symFindByValue(sysSymTbl, *(pulNewSFR - 1), acFunName, &lValue, &SymType);

}

#endif

/*******************************************************************************
* 函数名称: cpss_sbbr_output_fun_list
* 功    能: 输出函数调用栈
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* ulPC                    UINT32                   IN                      当前指令地址
* pStackFrame          UINT32 *                IN                    栈帧指针
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_sbbr_output_fun_list(INT32 lTid, CPSS_ESF * pEsf)
{
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 4. Fun Stack  Info start\n\n");

#if (CPSS_CPU_TYPE == CPSS_CPU_PPC)
    /* 输出ppc函数调用关系 */
    cpss_sbbr_output_ppc_fun_list(g_stExcInfo.tRegSet.pc, (UINT32 *) g_stExcInfo.tRegSet.gpr[1]);
#endif

#if (CPSS_CPU_TYPE == CPSS_CPU_I86)
    /* 输出X86函数调用关系 */
    cpss_sbbr_output_x86_fun_list((UINT32) g_stExcInfo.tRegSet.pc, (UINT32 *) g_stExcInfo.tRegSet.ebp);
#endif

#if (CPSS_CPU_TYPE == CPSS_CPU_ARM)
    /* 输出ARM函数调用关系 */
    cpss_sbbr_output_arm_fun_list(g_stExcInfo.tRegSet.pc, g_stExcInfo.tRegSet.r[11]);
#endif
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 4. Fun Stack Info End\n\n");

}

/*******************************************************************************
* 函数名称: cpss_sbbr_output_vec_info
* 功    能: 输出异常向量描述
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* lVecNum                   INT32                   IN                     异常向量
*
* 函数返回: 
* 说    明: 
******************************************************************************/
VOID cpss_sbbr_output_vec_info(INT32 lVecNum)
{
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 1. Vec Info start\n\n");

#if (CPSS_CPU_TYPE == CPSS_CPU_PPC)
    switch (lVecNum)
    {
    case _EXC_OFF_DATA:
        {
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Data Access Exception");
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Exception current instruction address: 0x%08x\n", g_stExcInfo.tEsf.regSet.pc);

        }
        break;

    case _EXC_OFF_INST:
        {
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Instruction Access Exception");
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Exception current instruction address: 0x%08x\n", g_stExcInfo.tEsf.regSet.pc);

        }
        break;

    case _EXC_OFF_ALIGN:
        {
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "alignment Exception");
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Exception current instruction address: 0x%08x\n", g_stExcInfo.tEsf.regSet.pc);

        }
        break;

    case _EXC_OFF_PROG:
        {
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Program Exception");
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Exception current instruction address: 0x%08x\n", g_stExcInfo.tEsf.regSet.pc);

        }
        break;

    case _EXC_OFF_FPU:
        {
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Floating point unavailable Exception");
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Exception current instruction address: 0x%08x\n", g_stExcInfo.tEsf.regSet.pc);

        }
        break;
    default:
        {
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Unknown Exception");
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Exception current instruction address: 0x%08x\n", g_stExcInfo.tEsf.regSet.pc);

        }
        break;
    }
#if 0
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Machine Status Register: 0x%08x\n", g_stExcInfo.tEsf.regSet.msr);
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Condition Register: 0x%08x\n", g_stExcInfo.tEsf.regSet.cr);
#endif
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "VecNumber = 0x%x\n\n", lVecNum);

#endif

#if (CPSS_CPU_TYPE == CPSS_CPU_I86)
    switch (lVecNum)
    {
    case IN_DIVIDE_ERROR /* 0x0 */ :
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Divide Error");
        break;
    case IN_INVALID_OPCODE /* 0x6 */ :
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Invalid Opcode");
        break;
    case IN_INVALID_TSS /* 10 */ :
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Invalid Tss");
        break;
    case IN_NO_SEGMENT /* 11 */ :
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "No Segment");
        break;
    case IN_STACK_FAULT /* 12 */ :
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Stack Fault");
        break;
    case IN_PROTECTION_FAULT /* 0xD */ :
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "General Protection Fault");
        break;
    case IN_PAGE_FAULT /* 0xE */ :
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Page Fault");
        break;
    }
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Program Counter: 0x%x\n", g_stExcInfo.tEsf.pc);
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "VecNumber = 0x%x\n\n ", lVecNum);

#endif

#if (CPSS_CPU_TYPE == CPSS_CPU_ARM)
    switch (lVecNum)
    {

    case 0x0:
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Branch through zero");
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Branch to 0x%08x\n", g_stExcInfo.tRegSet.r[3]);
        break;
    case EXC_OFF_UNDEF /* 0x4 */ :
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Undefined instruction");
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Exception address: 0x%08x\n", g_stExcInfo.tRegSet.r[15]);
        break;
    case EXC_OFF_SWI /* 0x08 */ :
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Software Interrupt");
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Exception address: 0x%08x\n", g_stExcInfo.tRegSet.r[15]);
        break;
    case EXC_OFF_DATA /* 0x10 */ :
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Data abort");
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Exception address: 0x%08x\n", g_stExcInfo.tRegSet.r[15]);
        break;
    case EXC_OFF_PREFETCH /* 0x0c */ :
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n", "Prefetch Abort");
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Exception address: 0x%08x\n", g_stExcInfo.tRegSet.r[15]);
        break;
    }

    /* cpss_sbbr_sprintf(g_pcSbbrExcInfo,"Current Processor Status Register: 0x%08x\n",g_stExcInfo.tEsf.cpsr); */
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "VecNumber = 0x%x \n\n", lVecNum);   /* 打印异常向量号 */

#endif
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 1. Vec Info End\n\n");

}

/*******************************************************************************
* 函数名称: cpss_sbbr_output_task_info
* 功    能: 输出任务信息
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* lTid                   INT32                   IN                          任务描述符
*
* 函数返回: 
* 说    明: 
******************************************************************************/

VOID cpss_sbbr_output_task_info(INT32 lTid)
{
    TASK_DESC tDesc;
    WIND_TCB *pTcb = taskTcb(lTid);
    char acStatus[10] = { 0 };

    if (NULL == pTcb)
    {
        return;
    }

    taskInfoGet(lTid, &tDesc);
    taskStatusString(lTid, acStatus);

    if (pTcb->pExcRegSet != NULL)
    {
        g_stExcInfo.tRegSet = *(pTcb->pExcRegSet);

    }
    else
    {
        g_stExcInfo.tRegSet = pTcb->regs;
    }

    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "2. Task Info start\n\n");
    /* 输出任务信息 */
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " NAME        ENTRY     TID      PRI      STATUS        PC         SP       ERRNO  DELAY\n");
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "-----------------------------------------------------------------------------------------\n");
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%10s 0x%08x 0x%08x %3d  %10s 0x%08x 0x%08x %6d %6d", pTcb->name, pTcb->entry, lTid, pTcb->priority, acStatus, g_stExcInfo.tRegSet.pc,
#if (CPSS_CPU_TYPE == CPSS_CPU_PPC)
                      g_stExcInfo.tRegSet.gpr[1],
#endif
#if (CPSS_CPU_TYPE == CPSS_CPU_I86)
                      g_stExcInfo.tRegSet.esp,
#endif
#if (CPSS_CPU_TYPE == CPSS_CPU_ARM)
                      g_stExcInfo.tRegSet.r[11],
#endif
                      tDesc.td_errorStatus, tDesc.td_delay);

    /* 输出任务栈使用情况 */
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "\n\n Stack : base 0x%x  end 0x%x  size %d ", (INT32) tDesc.td_pStackBase, (INT32) tDesc.td_pStackEnd, tDesc.td_stackSize);
    if (tDesc.td_options & VX_NO_STACK_FILL)
    {
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "high %s marign %s\n", "???", "???");
    }
    else
    {
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "high %d marign %d\n", tDesc.td_stackHigh, tDesc.td_stackMargin);
    }

    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " \n\n2. Task Info End\n\n");

}

/*******************************************************************************
* 函数名称: cpss_sbbr_get_sched_by_tid
* 功    能: 获得调度任务描述符
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
*ulTid                   UINT32                   IN                          任务描述符
*
* 函数返回: 
* 说    明: 
******************************************************************************/
UINT32 cpss_sbbr_get_sched_by_tid(INT32 lTid)
{
    UINT32 ulSchedDesc;
    VK_SCHED_ID tSchedId;
    UINT32 ulTaskDesc;
    for (ulTaskDesc = 0; ulTaskDesc < VOS_MAX_TASK_NUM; ulTaskDesc++)
    {
        if (g_astCpssVosTaskDescTbl[ulTaskDesc].taskId == lTid)
        {
            break;
        }
    }
    if (ulTaskDesc >= VOS_MAX_TASK_NUM)
    {
        return VK_SCHED_DESC_INVALID;
    }

    for (ulSchedDesc = 1; ulSchedDesc < VK_MAX_SCHED_NUM; ulSchedDesc++)
    {
        if (g_astCpssVkSchedDescTbl[ulSchedDesc].bInuse == TRUE)
        {

            tSchedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;

            if (tSchedId->ulTaskDesc == ulTaskDesc)
            {
                return ulSchedDesc;
            }

        }
    }
    return VK_SCHED_DESC_INVALID;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_output_proc_info
* 功    能:  输出纤程信息
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
*ulSchedDesc                 UINT32                   IN                  调度任务描述符
*
* 函数返回: 
* 说    明: 
******************************************************************************/

VOID cpss_sbbr_output_proc_info(UINT32 ulSchedDesc)
{
    VK_SCHED_ID schedId;
    CPSS_COM_MSG_HEAD_T *pstData;
    UINT16 usGuid;

    schedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;    /* 获得当前控制结构 */

    pstData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA(schedId->pIpcMsgCurr);

    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 3. Proc Info start\n\n");

    if (pstData != NULL)
    {
        usGuid = (UINT16) ((schedId->ulProcDescCurr >> 16) & 0x0000ffff);
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, " Current Thread Name :%s Current MsgId:0x%x Msg Addr %p,Length %d\n\r  ", g_astCpssVkProcClassTbl[usGuid].acName, pstData->ulMsgId, pstData->pucBuf, pstData->ulLen);
        usGuid = (UINT16) ((pstData->stSrcProc.ulPd >> 16) & 0x0000ffff);

        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Source:  Module = %d SubGroup= %d  Group = %d AddrFlag = %d procName:%s\n\r", pstData->stSrcProc.stLogicAddr.ucModule, pstData->stSrcProc.stLogicAddr.ucSubGroup, pstData->stSrcProc.stLogicAddr.usGroup, pstData->stSrcProc.ulAddrFlag, g_astCpssVkProcClassTbl[usGuid].acName);

        usGuid = (UINT16) ((pstData->stDstProc.ulPd >> 16) & 0x0000ffff);

        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "Dest:  Module = %d SubGroup= %d  Group = %d AddrFlag = %d procName:%s", pstData->stDstProc.stLogicAddr.ucModule, pstData->stDstProc.stLogicAddr.ucSubGroup, pstData->stDstProc.stLogicAddr.usGroup, pstData->stDstProc.ulAddrFlag, g_astCpssVkProcClassTbl[usGuid].acName);

    }

    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 3. Proc Info End\n\n");

}

/*******************************************************************************
* 函数名称: cpss_sbbr_output_data
* 功    能:  输出正在处理的纤程数据
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
*ulSchedDesc                 UINT32                   IN                  调度任务描述符
*
* 函数返回: 
* 说    明: 
******************************************************************************/

VOID cpss_sbbr_output_data(UINT32 ulSchedDesc)
{
    UINT32 ulArgs[4];
    VK_SCHED_ID schedId;
    CPSS_COM_MSG_HEAD_T *pstData;
    UINT32 ulLen;

    schedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;    /* 获得当前控制结构 */

    if (schedId == NULL)
    {
        return;
    }
    pstData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA(schedId->pIpcMsgCurr);

    if (pstData != NULL)
    {
        ulLen = pstData->ulLen;
        /* 最长200个字节 */
        if (ulLen > CPSS_SBBR_WRITE_DATA_MAX_LEN)
        {
            ulLen = CPSS_SBBR_WRITE_DATA_MAX_LEN;
        }
        cpss_sbbr_write_data(0, ulArgs, pstData->pucBuf, ulLen);
    }

}

/*******************************************************************************
* 函数名称: cpss_sbbr_output_exc_info
* 功    能:  输出正在处理的纤程数据
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* lTid                        INT32                           IN              任务描述符
* lVecNum                 INT32                           IN                 异常向量
*pEsf                       CPSS_ESF *                  IN                栈帧指针
* 函数返回: 
* 说    明: 
******************************************************************************/

VOID cpss_sbbr_output_exc_info(INT32 lTid, INT32 lVecNum, CPSS_ESF * pEsf)
{

    UINT32 ulSchedDesc;
    UINT32 ulArgs[4];
    g_bCpssSbbrCurExcOccur = TRUE;
    cpss_sbbr_record_data_code_segment(g_pcSbbrExcInfo);
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "\n\r****************** GMT %d *****************************\n\r", cpss_gmt_get());
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "\n ***************SWP Exception Info Start***************\n");
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 1. Vec Info\n");
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 2. Task Info\n");
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 3. Sched Proc info\n");
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 4. Fun Stack  info\n");
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 5. Sched Proc Data\n\n");
    /* 输出异常向量信息 */
    cpss_sbbr_output_vec_info(lVecNum);
    /* 输出任务信息 */
    cpss_sbbr_output_task_info(lTid);

    ulSchedDesc = cpss_sbbr_get_sched_by_tid(lTid);
    /* 如果是调度任务输出纤程信息 */
    if (ulSchedDesc != VK_SCHED_DESC_INVALID)
    {
        cpss_sbbr_output_proc_info(ulSchedDesc);
    }
    else
    {
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 3. No  Sched Task\n\n");

    }

    /* 输出函数调用关系 */
#if 1
    /* cpss_sbbr_output_fun_list(lTid,pEsf); */
    cpss_sbbr_tt(lTid);
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 4. Fun Stack  Info start\n\n");
    {
        UINT16 usLoop;
        for (usLoop = 0; usLoop < g_lcurCallDep; usLoop++)
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n\r", g_azCpssSbbrTempBuf[usLoop]);
    }
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 4. Fun Stack Info End\n\n");
#endif

    /* 输出正在调度的数据 */
    if (ulSchedDesc != VK_SCHED_DESC_INVALID)
    {

        cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 5. Sched Proc Data Start\n\n");

        cpss_sbbr_write_text(0, ulArgs, "%s\n\n", g_pcSbbrExcInfo);

        cpss_sbbr_output_data(ulSchedDesc);

        cpss_sbbr_write_text(0, ulArgs, "%s\n\n", "5.Sched Proc Data End");

    }
    else
    {
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, " 5. No  Sched Proc Data \n\n");
        cpss_sbbr_write_text(0, ulArgs, "%s\n\n", g_pcSbbrExcInfo);
    }

    cpss_sbbr_write_text(0, ulArgs, "%s\n\n", " ***************SWP Exception Info End***************\n");

}

/*******************************************************************************
* 函数名称: cpss_sbbr_hook_proc
* 功    能: hook处理
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* lTid                          task Id                             IN            
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_sbbr_hook_proc(INT32 ulTid)
{
    UINT32 ulSchedDesc;
    UINT16 usLoop;
    UINT32 ulTidType;
    UINT32 ulDesc;

    ulSchedDesc = cpss_sbbr_get_sched_by_tid(ulTid);

    if (ulSchedDesc == VK_SCHED_DESC_INVALID)
    {
        ulTidType = 0;
        ulDesc = ulTid;
    }
    else
    {
        ulTidType = 1;
        ulDesc = ulSchedDesc;
    }

    for (usLoop = 0; usLoop < CPSS_SBBR_MAX_HOOK_NUM; usLoop++)
    {
        if (g_astCpssSbbrHookArray[usLoop] != NULL)
        {
            g_astCpssSbbrHookArray[usLoop] (ulTidType, ulDesc);

        }
    }
}

/*******************************************************************************
* 函数名称: cpss_sbbr_exception_proc
* 功    能: 异常处理函数，发生异常时被调用
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* lTid                          task Id                             IN            
* lVecNum                   INT32                          IN              异常向量
* pEsf                       CPSS_ESF *                   IN              堆栈帧指针
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_sbbr_exception_proc(INT32 lTid, INT32 lVecNum, CPSS_ESF * pEsf)
{

    WIND_TCB *ptTcb;

    ptTcb = taskTcb(lTid);

    if (NULL == ptTcb)
    {
        return;
    }

    /* 异常发生,保存当前任务的寄存器 */
    if (ptTcb->pExcRegSet != NULL)
    {
        g_stExcInfo.tRegSet = *(ptTcb->pExcRegSet);
        /* 保存异常栈 */
        g_stExcInfo.tEsf = *pEsf;
    }
    else                        /* 无异常, 任务运行时间过长 */
    {
        g_stExcInfo.tRegSet = ptTcb->regs;
        if (g_pcSbbrExcInfo != NULL)
        {
            UINT32 ulArgs[4];
            cpss_sbbr_record_data_code_segment(g_pcSbbrExcInfo);
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "\n\r****************** GMT %d *****************************\n\r", cpss_gmt_get());
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "\n *************CPSS moniter task 0x%x  dead loop ***************\n", lTid);

            cpss_sbbr_output_task_info(lTid);
            cpss_sbbr_tt(lTid);
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "  Fun Stack  Info start\n\n");
            {
                UINT16 usLoop;
                for (usLoop = 0; usLoop < g_lcurCallDep; usLoop++)
                    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n\r", g_azCpssSbbrTempBuf[usLoop]);
            }
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "  Fun Stack Info End\n\n");
            cpss_sbbr_write_text(0, ulArgs, "%s\n\n", g_pcSbbrExcInfo);
            cpss_mem_memset(g_pcSbbrExcInfo, 0, CPSS_SBBR_EXC_BUF_LENGTH);
            cpss_kw_save_all_task_load();

            cpss_taskRegsShow(lTid);
            /* 记录任务栈部分到sbbr */
            cpss_sbbr_write_task_stack_to_sbbr(lTid);
            /* 调用注册的hook */
            cpss_sbbr_hook_proc(lTid);

            /* 设置异常信息区已经写过标志,不能再写入 */
            cpss_sbbr_set_exc_flag(1);

        }
        return;
    }

    /* 输出异常信息到sbbr */
    if (g_pcSbbrExcInfo != NULL)
    {
        cpss_sbbr_output_exc_info(lTid, lVecNum, pEsf);
    }
    cpss_taskRegsShow(lTid);
    /* 记录任务栈部分到sbbr */
    cpss_sbbr_write_task_stack_to_sbbr(lTid);
    /* 调用注册的hook */
    cpss_sbbr_hook_proc(lTid);

    /* 设置异常信息区已经写过标志,不能再写入 */
    cpss_sbbr_set_exc_flag(1);

}

VOID cpss_sbbr_show_sys_buf()
{
    if (g_pstCpssSbbrSys != NULL)
    {
        printf(" \n\r ulPowerOnFlag = %x", g_pstCpssSbbrSys->ulPowerOnFlag);
        printf(" \n\r ulRunStatusFlag = %x", g_pstCpssSbbrSys->ulRunStatusFlag);
        printf(" \n\r ulVersion = %x", g_pstCpssSbbrSys->ulVersion);
        printf(" \n\r pucSbbrStartAddr = %x", g_pstCpssSbbrSys->pucSbbrStartAddr);

        printf(" \n\r ulSBBRTotalLen = %d", g_pstCpssSbbrSys->ulSBBRTotalLen);
        printf(" \n\r pucMngStartAddr = 0x%x", g_pstCpssSbbrSys->pucMngStartAddr);
        printf(" \n\r ulMngSysLen = %d", g_pstCpssSbbrSys->ulMngSysLen);
        printf(" \n\r pucIsrStartAddr = 0x%x", g_pstCpssSbbrSys->pucIsrStartAddr);
        printf(" \n\r ulMngIsrLen = %d", g_pstCpssSbbrSys->ulMngIsrLen);
        printf(" \n\r pucExecStartAddr = 0x%x", g_pstCpssSbbrSys->pucExecStartAddr);
        printf(" \n\r ulExecLen = %d", g_pstCpssSbbrSys->ulExecLen);
        printf(" \n\r pucUserStartAddr = 0x%x", g_pstCpssSbbrSys->pucUserStartAddr);
        printf(" \n\r ulUserLen = %d", g_pstCpssSbbrSys->ulUserLen);
        printf(" \n\r pucSpecAddr = 0x%x", g_pstCpssSbbrSys->pucSpecAddr);
        printf(" \n\r ulSpecLen = %d", g_pstCpssSbbrSys->ulSpecLen);
        printf(" \n\r ucEndian = 0x%x", g_pstCpssSbbrSys->ucEndian);
        printf(" \n\r ucTransfer = %d", g_pstCpssSbbrSys->ucTransfer);
        printf(" \n\r ucWriteAble = %d", g_pstCpssSbbrSys->ucWriteAble);
        printf(" \n\r usSeqNumNow = %d", g_pstCpssSbbrSys->usSeqNumNow);
        printf(" \n\r usUserNum = %d", g_pstCpssSbbrSys->usUserNum);

    }

}

void cpss_sbbr_show_user()
{
    if (g_pstCpssSbbrSys != NULL)
    {
        UINT32 ulLoop;

        CPSS_SBBR_USER_T *pstUser;
        pstUser = g_pstCpssSbbrSys->astUser;

        printf("\n\r UserNo      Name         Size       Hook         Min       CurLoc      StartAddr ");

        for (ulLoop = 0; ulLoop <= g_pstCpssSbbrSys->usUserNum; ulLoop++)
        {

            printf("\n\r%6d %16s %8d %10p %10d %10d %16p", ulLoop, pstUser[ulLoop].acName, pstUser[ulLoop].lSize, pstUser[ulLoop].Hook, pstUser[ulLoop].lMin, pstUser[ulLoop].lCurWriteLoc, pstUser[ulLoop].pucAddrStart);

        }
    }

}

#endif

/*******************************************************************************
* 函数名称: cpss_kw_ha_init
* 功    能: ha初始化
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* l
* 函数返回: CPSS_OK； CPSS_ERROR
* 说    明: 
*******************************************************************************/

INT32 cpss_kw_ha_init()
{
    g_pstCpssKwScanInfo = cpss_mem_malloc(CPSS_KW_MAX_SCAN_ITEM * sizeof(CPSS_KW_SCAN_INFO_T));

    if (NULL == g_pstCpssKwScanInfo)
    {
        return CPSS_ERROR;
    }
    else
    {
        cpss_mem_memset(g_pstCpssKwScanInfo, 0, CPSS_KW_MAX_SCAN_ITEM * sizeof(CPSS_KW_SCAN_INFO_T));
        return CPSS_OK;
    }
}

/*******************************************************************************
* 函数名称: cpss_kw_alloc_kwid
* 功    能: 分配kwid
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* 
* 函数返回: kwid or VOS_TASK_DESC_INVALID
* 说    明: 
*******************************************************************************/

UINT32 cpss_kw_alloc_kwid()
{
    UINT16 usLoop;

    for (usLoop = 1; usLoop < CPSS_KW_MAX_SCAN_ITEM; usLoop++)
    {
        if (g_pstCpssKwScanInfo[usLoop].bValidFlag == 0)
        {
            g_pstCpssKwScanInfo[usLoop].bValidFlag = 1;
            return usLoop;
        }
    }
    return VOS_TASK_DESC_INVALID;
}

/*******************************************************************************
* 函数名称: cpss_get_ticks_by_time_type
* 功    能: 根据时间类型 换算为ticks
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* llKwLimitsTime            INT32                         IN                时间类型
* 函数返回: 对应的ticks 
* 说    明: 
*******************************************************************************/

UINT32 cpss_kw_get_ticks_by_time_type(INT32 lKwTmOutLimitType)
{
#ifdef CPSS_VOS_VXWORKS
    switch (lKwTmOutLimitType)
    {
    case 1:
        return 3 * sysClkRateGet();
    case 2:
        return 20 * 60 * sysClkRateGet();
    default:
        return 10 * sysClkRateGet();

    }
#else
    return 1000;
#endif
}

/*******************************************************************************
* 函数名称: cpss_kw_fill_task_info
* 功    能: 填写任务信息到监视表中
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* ulKwId                       UINT32                           IN            监视表项
* ulTaskDesc                   INT32                          IN              任务描述符
*
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_kw_fill_task_info(UINT32 ulKwId, UINT32 ulTaskDesc)
{
    if (ulKwId > CPSS_KW_MAX_SCAN_ITEM)
        return;

    strncpy(g_pstCpssKwScanInfo[ulKwId].acName, g_astCpssVosTaskDescTbl[ulTaskDesc].acName, 32);
    g_pstCpssKwScanInfo[ulKwId].bIsProc = FALSE;
    g_pstCpssKwScanInfo[ulKwId].lKwTmOutLimitType = g_astCpssVosTaskDescTbl[ulTaskDesc].lKwTmOutLimitType;
    g_pstCpssKwScanInfo[ulKwId].bRunFlag = CPSS_KW_RUN_FLAG_NO_RUN;
    g_pstCpssKwScanInfo[ulKwId].lMaxTicks = swp_get_ha_time(g_pstCpssKwScanInfo[ulKwId].lKwTmOutLimitType) / 10;
    g_pstCpssKwScanInfo[ulKwId].lCurTicks = g_pstCpssKwScanInfo[ulKwId].lMaxTicks;
    g_pstCpssKwScanInfo[ulKwId].bValidFlag = TRUE;
    g_pstCpssKwScanInfo[ulKwId].lPid = (INT32) g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;

}

/*******************************************************************************
* 函数名称: cpss_kw_find_config_id_by_name
* 功    能: 根据调度任务名找到对应的表项
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* pName                      CHAR *                           IN           task name            
*
* 函数返回: schedconfig id 
* 说    明: 
*******************************************************************************/

UINT32 cpss_kw_find_config_id_by_name(CHAR * pName)
{
    UINT16 usLoop;

    for (usLoop = 0; usLoop < VOS_MAX_TASK_NUM; usLoop++)
    {
        if (strncmp(gatVkSchedConfig[usLoop].acName, pName, VOS_TASK_NAME_LEN) == 0)
        {
            return usLoop;
        }
    }
    return VOS_TASK_DESC_INVALID;
}

/*******************************************************************************
* 函数名称: cpss_kw_read_normal_task_info
* 功    能: fill  normal task config info  into g_astCpssVosTaskDescTbl
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* lTid                          task Id                             IN            
* lVecNum                   INT32                          IN              异常向量
* pEsf                       CPSS_ESF *                   IN              堆栈帧指针
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_kw_read_normal_task_info(UINT32 ulTaskDesc, UINT32 ulConfig)
{
    UINT32 ulKwId;

    ulKwId = cpss_kw_alloc_kwid();
    if (ulKwId != VOS_TASK_DESC_INVALID)
    {
        strncpy(g_astCpssVosTaskDescTbl[ulTaskDesc].acName, gatVkTaskConfig[ulConfig].acName, 32);
        g_astCpssVosTaskDescTbl[ulTaskDesc].ulKwId = ulKwId;
        g_astCpssVosTaskDescTbl[ulTaskDesc].ulSbbrSize = gatVkTaskConfig[ulConfig].ulSbbrSize;
        g_astCpssVosTaskDescTbl[ulTaskDesc].lKwTmOutLimitType = gatVkTaskConfig[ulConfig].lKwTmOutLimitType;
        if (g_astCpssVosTaskDescTbl[ulTaskDesc].ulSbbrSize == 0)
        {
            g_astCpssVosTaskDescTbl[ulTaskDesc].ulSbbrSize = CPSS_SBBR_DEFAULT_SIZE;
        }
        cpss_kw_fill_task_info(ulKwId, ulTaskDesc);
    }

}

/*******************************************************************************
* 函数名称: cpss_kw_read_sched_task_info
* 功    能:  fill sched task info into g_astCpssVosTaskDescTbl
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* lTid                          task Id                             IN            
* lVecNum                   INT32                          IN              异常向量
* pEsf                       CPSS_ESF *                   IN              堆栈帧指针
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_kw_read_sched_task_info(UINT32 ulTaskDesc, CHAR * pName)
{
    UINT32 ulConfig;

    strncpy(g_astCpssVosTaskDescTbl[ulTaskDesc].acName, pName, 32);

    ulConfig = cpss_kw_find_config_id_by_name(pName);
    if (ulConfig != VOS_TASK_DESC_INVALID)
    {
        g_astCpssVosTaskDescTbl[ulTaskDesc].ulSbbrSize = gatVkSchedConfig[ulConfig].ulSbbrSize;
        if (g_astCpssVosTaskDescTbl[ulTaskDesc].ulSbbrSize == 0)
        {
            g_astCpssVosTaskDescTbl[ulTaskDesc].ulSbbrSize = CPSS_SBBR_DEFAULT_SIZE;
        }
    }

}

/*******************************************************************************
* 函数名称: cpss_kw_read_proc_class_info
* 功    能:  fill proc class info into g_pstCpssKwScanInfo
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* usGuid                      UINT16                           IN              proc class
* 
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_kw_read_proc_class_info(UINT16 usGuid)
{
    UINT32 ulKwId;

    ulKwId = cpss_kw_alloc_kwid();
    if (ulKwId != VOS_TASK_DESC_INVALID)
    {
        strncpy(g_pstCpssKwScanInfo[ulKwId].acName, g_astCpssVkProcClassTbl[usGuid].acName, 32);
        g_pstCpssKwScanInfo[ulKwId].bIsProc = TRUE;
        g_pstCpssKwScanInfo[ulKwId].lKwTmOutLimitType = g_astCpssVkProcClassTbl[usGuid].lKwTmOutLimitType;
        g_pstCpssKwScanInfo[ulKwId].bRunFlag = CPSS_KW_RUN_FLAG_NO_RUN;
        g_pstCpssKwScanInfo[ulKwId].lMaxTicks = swp_get_ha_time(g_pstCpssKwScanInfo[ulKwId].lKwTmOutLimitType) / 10;
        g_pstCpssKwScanInfo[ulKwId].lCurTicks = g_pstCpssKwScanInfo[ulKwId].lMaxTicks;
        g_pstCpssKwScanInfo[ulKwId].bValidFlag = TRUE;
        g_pstCpssKwScanInfo[ulKwId].lPid = usGuid;

        g_astCpssVkProcClassTbl[usGuid].ulKwId = ulKwId;

    }
}

/*******************************************************************************
* 函数名称: cpss_kw_set_task_run
* 功    能: 设置当前任务开始运行
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* 
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_kw_set_task_run(VOID)
{
    UINT32 ulTaskDesc;
    UINT32 ulKwId;

    ulTaskDesc = cpss_vos_task_desc_self();
    ulKwId = g_astCpssVosTaskDescTbl[ulTaskDesc].ulKwId;

    if (ulKwId < CPSS_KW_MAX_SCAN_ITEM)
    {
        g_pstCpssKwScanInfo[ulKwId].bIsProc = FALSE;
        g_pstCpssKwScanInfo[ulKwId].lCurTicks = swp_get_ha_time(g_pstCpssKwScanInfo[ulKwId].lKwTmOutLimitType) / 10;
        g_pstCpssKwScanInfo[ulKwId].lPid = (INT32) g_astCpssVosTaskDescTbl[ulTaskDesc].taskId;
        g_pstCpssKwScanInfo[ulKwId].bRunFlag = CPSS_KW_RUN_FLAG_RUN;
        g_pstCpssKwScanInfo[ulKwId].bValidFlag = TRUE;
    }

}

/*******************************************************************************
* 函数名称: cpss_kw_set_task_stop
* 功    能: 设置任务结束
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* ?
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_kw_set_task_stop(VOID)
{
    UINT32 ulTaskDesc;
    UINT32 ulKwId;

    ulTaskDesc = cpss_vos_task_desc_self();
    ulKwId = g_astCpssVosTaskDescTbl[ulTaskDesc].ulKwId;
    if (ulKwId < CPSS_KW_MAX_SCAN_ITEM)
    {
        g_pstCpssKwScanInfo[ulKwId].bRunFlag = CPSS_KW_RUN_FLAG_NO_RUN;
    }
}

/*******************************************************************************
* 函数名称: cpss_kw_set_proc_run
* 功    能: 设置纤程运行
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* ulProcDesc                 UINT32                            IN            纤程描述符
* ?
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_kw_set_proc_run(UINT16 usGuid)
{

    UINT32 ulKwId;

    ulKwId = g_astCpssVkProcClassTbl[usGuid].ulKwId;
    if (ulKwId < CPSS_KW_MAX_SCAN_ITEM)
    {
        /* g_pstCpssKwScanInfo[ulKwId].bIsProc = TRUE; g_pstCpssKwScanInfo[ulKwId].lKwTmOutLimitType = g_astCpssVkProcClassTbl[usGuid].lKwTmOutLimitType; */
        g_pstCpssKwScanInfo[ulKwId].bRunFlag = CPSS_KW_RUN_FLAG_RUN;
        g_pstCpssKwScanInfo[ulKwId].lCurTicks = g_pstCpssKwScanInfo[ulKwId].lMaxTicks;

        /* g_pstCpssKwScanInfo[ulKwId].bValidFlag = TRUE; */
        g_pstCpssKwScanInfo[ulKwId].lPid = g_astCpssVkProcClassTbl[usGuid].ulSchedDesc;
    }
}

/*******************************************************************************
* 函数名称: cpss_kw_set_proc_stop
* 功    能: 设置县城停止
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* ulProcDesc            UINT32                            IN            纤程描述符     
* 
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_kw_set_proc_stop(UINT16 usGuid)
{

    UINT32 ulKwId;

    ulKwId = g_astCpssVkProcClassTbl[usGuid].ulKwId;
    if (ulKwId < CPSS_KW_MAX_SCAN_ITEM)
    {
        g_pstCpssKwScanInfo[ulKwId].bRunFlag = CPSS_KW_RUN_FLAG_NO_RUN;
    }
}

/*******************************************************************************
* 函数名称: cpss_kw_find_exception_occur
* 功    能: 检查当前监视表中的各项是否异常
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
*  pbIsProc                  bool                             out             0 :任务 1:纤程           
* ?
* 函数返回: taskid or procdesc
* 说    明: 
*******************************************************************************/

extern BOOL g_bCpssRecvProcEndInd;
UINT32 cpss_kw_find_exception_occur(BOOL * pbIsProc)
{
	UINT16 usLoop = 0;

    if (FALSE == g_bCpssRecvProcEndInd)
    {
        g_bCpssRecvProcEndInd = smss_get_proc_active_state();
    }

    if (g_bCpssCloseException == TRUE)
    {
        return VOS_TASK_DESC_INVALID;
    }

    for (usLoop = 1; usLoop < CPSS_KW_MAX_SCAN_ITEM; usLoop++)
    {
        if ((g_pstCpssKwScanInfo[usLoop].bValidFlag == TRUE) && (g_pstCpssKwScanInfo[usLoop].bRunFlag == CPSS_KW_RUN_FLAG_RUN))
        {
            /* if proc is being init return */
            if ((g_pstCpssKwScanInfo[usLoop].bIsProc == TRUE) && (g_bCpssRecvProcEndInd == FALSE))
            {
                return VOS_TASK_DESC_INVALID;
            }
            if (g_pstCpssKwScanInfo[usLoop].lCurTicks-- <= 0)
            {
                *pbIsProc = g_pstCpssKwScanInfo[usLoop].bIsProc;
                g_pstCpssKwScanInfo[usLoop].lCurTicks = swp_get_ha_time(g_pstCpssKwScanInfo[usLoop].lKwTmOutLimitType) / 10;
                g_bCpssSbbrCurExcOccur = TRUE;

                g_szKwProcName = g_pstCpssKwScanInfo[usLoop].acName;

                return g_pstCpssKwScanInfo[usLoop].lPid;
            }
        }
    }
    return VOS_TASK_DESC_INVALID;

}

/*******************************************************************************
* 函数名称: cpss_kw_show_scan_info
* 功    能: 显示当前监视表中的内容
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* 
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_kw_show_scan_info()
{
    if (g_pstCpssKwScanInfo != NULL)
    {
        UINT16 usLoop = 0;
        printf(" kwid   Name            bRunFlag  lKwTmOutLimitType  lCurTicks bValidFlag bIsProc     lPid \n\r");
        for (usLoop = 0; usLoop < CPSS_KW_MAX_SCAN_ITEM; usLoop++)
        {
            if (g_pstCpssKwScanInfo[usLoop].bValidFlag == TRUE)
            {
                printf(" %d %15s %10d %19d %10d %10d %6d 0x%x\n\r", usLoop, g_pstCpssKwScanInfo[usLoop].acName, g_pstCpssKwScanInfo[usLoop].bRunFlag, g_pstCpssKwScanInfo[usLoop].lKwTmOutLimitType, g_pstCpssKwScanInfo[usLoop].lCurTicks, g_pstCpssKwScanInfo[usLoop].bValidFlag, g_pstCpssKwScanInfo[usLoop].bIsProc, g_pstCpssKwScanInfo[usLoop].lPid);

            }

        }
    }
}

VOID cpss_kw_show_task_config_info()
{
    UINT16 usLoop;

    printf("   kwid   name               lKwTmOutLimitType   taskId \n\r");
    for (usLoop = 0; usLoop < VOS_MAX_TASK_NUM; usLoop++)
    {
        if (g_astCpssVosTaskDescTbl[usLoop].bInuse == TRUE)
        {
            printf("  %5d %15s %18d  0x%x\n\r", g_astCpssVosTaskDescTbl[usLoop].ulKwId, g_astCpssVosTaskDescTbl[usLoop].acName, g_astCpssVosTaskDescTbl[usLoop].lKwTmOutLimitType, g_astCpssVosTaskDescTbl[usLoop].taskId);
        }

    }

}

VOID cpss_kw_show_proc_class_info()
{
    UINT16 usLoop;
    printf(" kwid   name            lKwTmOutLimitType\n\r");
    for (usLoop = 0; usLoop < VK_MAX_PROC_CLASS_NUM; usLoop++)
    {
        if (g_astCpssVkProcClassTbl[usLoop].bInuse == TRUE)
        {
            printf(" %5d %15s  %18d\n\r", g_astCpssVkProcClassTbl[usLoop].ulKwId, g_astCpssVkProcClassTbl[usLoop].acName, g_astCpssVkProcClassTbl[usLoop].lKwTmOutLimitType);
        }

    }
}

/*******************************************************************************
* 函数名称: coss_kw_prt_occupy_log
* 功    能: 打印占用率最高的任务和纤程,在smss上报占用率过高时调用
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* 
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_kw_prt_occupy_log()
{
#ifdef CPSS_VOS_VXWORKS

    UINT32 ulArgs[4];
    CPSS_TIME_T tCurClock;

    if (g_bCpssRecvProcEndInd != FALSE)
    {
        cpss_clock_get(&tCurClock);
        cpss_cpu_usage_get(&ulArgs[0]);
        cpss_sbbr_write_text(0, ulArgs, "%.4d-%.2d-%.2d-%.2d:%.2d:%.2d cpu load[%d] \n\r", tCurClock.usYear, tCurClock.ucMonth, tCurClock.ucDay, tCurClock.ucHour, tCurClock.ucMinute, tCurClock.ucSecond, ulArgs[0]);
        cpss_mem_memset(g_pcSbbrExcInfo, 0, CPSS_SBBR_EXC_BUF_LENGTH);

        cpss_mem_memset(g_pcSbbrExcInfo, 0, CPSS_SBBR_EXC_BUF_LENGTH);
        cpss_kw_save_all_task_load();
        cpss_mem_memset(g_pcSbbrExcInfo, 0, CPSS_SBBR_EXC_BUF_LENGTH);

        if ((g_bCpssChkOccupy == FALSE) || (g_bCpssOccupyOver != TRUE))
            return;

        cpss_sbbr_write_text(0, ulArgs, "\n\r%-20s%-15d", g_tCpssOccupyAnalysis[2].acName, g_tCpssOccupyAnalysis[2].ulOccupy);
    }
#endif

#if 0

    UINT32 ulOccupy;
    if (g_bCpssChkOccupy == FALSE)
        return;

    if (g_bCpssOccupyOver != TRUE)
        return;

    cpss_cpu_usage_get(&ulOccupy);

    cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_DBG, CPSS_PRINT_IMPORTANT, "\n\r%-20s%-15d\n\r%-20s%-15d\n\r%-20s%-15d\n\r%-20s%-15d", "current occupy", ulOccupy, g_tCpssOccupyAnalysis[0].acName, g_tCpssOccupyAnalysis[0].ulOccupy, g_tCpssOccupyAnalysis[1].acName, g_tCpssOccupyAnalysis[1].ulOccupy, g_tCpssOccupyAnalysis[2].acName, g_tCpssOccupyAnalysis[2].ulOccupy);

#endif

}

#ifdef CPSS_VOS_VXWORKS

char *cpss_taskRegsFmt = "%-6s = %8x";

VOID cpss_taskRegsShow(INT32 tid)
{
    UINT32 ulArgs[4];

    int ix;
    int *pReg;                  /* points to register value */
    REG_SET regSet;             /* register set */

    cpss_mem_memset(g_pcSbbrExcInfo, 0, CPSS_SBBR_EXC_BUF_LENGTH);

    if (taskRegsGet(tid, &regSet) == ERROR)
    {
        printf("taskRegsShow: invalid task id %#x\n", tid);
        return;
    }

    /* print out registers */

    for (ix = 0; taskRegName[ix].regName != NULL; ix++)
    {
        if ((ix % 4) == 0)
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "\n");
        else
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%3s", "");

        if (taskRegName[ix].regName[0] != '\0')
        {
            pReg = (int *)((int)&regSet + taskRegName[ix].regOff);
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, cpss_taskRegsFmt, taskRegName[ix].regName, *pReg);
        }
        else
            cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%17s", "");
    }
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "\n");

    cpss_sbbr_write_text(0, ulArgs, "%s\n\n", g_pcSbbrExcInfo);
    cpss_mem_memset(g_pcSbbrExcInfo, 0, CPSS_SBBR_EXC_BUF_LENGTH);
}


void my_dbgPrintCall(INSTR * callAdrs,  /* address from which function was called */
                     int funcAdrs,  /* address of function called */
                     int nargs, /* number of arguments in function call */
                     UINT32 * args  /* pointer to function args */
    );

void cpss_sbbr_tt(UINT32 ulTaskId)
{
    REG_SET regSet;
    taskRegsGet(ulTaskId, &regSet);
    trcStack(&regSet, (FUNCPTR) my_dbgPrintCall, ulTaskId);
}

void my_dbgPrintCall(INSTR * callAdrs,  /* address from which function was called */
                     int funcAdrs,  /* address of function called */
                     int nargs, /* number of arguments in function call */
                     UINT32 * args  /* pointer to function args */
    )
{
    int ix;
    char *name;                 /* pointer to symbol tbl copy of function name goes here */
    void *val;                  /* address of named fn goes here */
    BOOL doingDefault = FALSE;
    char demangled[256 + 1];
    char *nameToPrint;

    int curBufLoc;

    if (g_lcurCallDep >= 15)
    {
        return;
    }
    /* print call address and name of calling function plus offset */
    curBufLoc = 0;
    sprintf(&g_azCpssSbbrTempBuf[g_lcurCallDep][curBufLoc], "0x%08x : 0x%08x (", callAdrs, funcAdrs);   /* print address from which called */
    curBufLoc += 25;
    /* make things line up properly */

    /* if no args are specified, print out default number (see doc at top) */

    if ((nargs == 0) && (trcDefaultArgs != 0))
    {
        doingDefault = TRUE;
        nargs = trcDefaultArgs;
        sprintf(&g_azCpssSbbrTempBuf[g_lcurCallDep][curBufLoc], "[");
        curBufLoc += 1;
    }

    /* print args */

    for (ix = 0; ix < nargs; ix++)
    {
        if (ix > 0)
        {
            sprintf(&g_azCpssSbbrTempBuf[g_lcurCallDep][curBufLoc], ", ");
            curBufLoc += 2;
        }

        if (args[ix] == 0)
        {
            sprintf(&g_azCpssSbbrTempBuf[g_lcurCallDep][curBufLoc], "0");
            curBufLoc += 1;
        }
        else
        {
            sprintf(&g_azCpssSbbrTempBuf[g_lcurCallDep][curBufLoc], "0x%08x", args[ix]);
            curBufLoc += 10;
        }
    }

    if (doingDefault)
    {
        sprintf(&g_azCpssSbbrTempBuf[g_lcurCallDep][curBufLoc], "]");
        curBufLoc += 1;
    }

    sprintf(&g_azCpssSbbrTempBuf[g_lcurCallDep][curBufLoc], ")\n");
    curBufLoc += 1;
    g_azCpssSbbrTempBuf[g_lcurCallDep][curBufLoc] = '\0';

    printf("%s\n", g_azCpssSbbrTempBuf[g_lcurCallDep]);

    g_lcurCallDep += 1;

}

void myshow()
{
    int i;

    for (i = 0; i < 14; i++)
    {
        printf("%s", g_azCpssSbbrTempBuf[i]);
    }
}

/*******************************************************************************
* 函数名称: cpss_sbbr_write_exc_info_disk
* 功    能: 主控写异常到磁盘
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* 
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_sbbr_write_exc_info_disk()
{
#ifdef CPSS_VOS_VXWORKS
    UINT32 ulFileId, ulBytesWrite;
    INT32 lRet;
    CHAR acBuf[64];
    CHAR acName[128];
    UINT16 usNum;
    CPSS_COM_PHY_ADDR_T tThisPhy;
    CPSS_TIME_T tThisTime;
    cpss_com_phy_addr_get(&tThisPhy);
    cpss_clock_get(&tThisTime);

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
    sprintf(acBuf, "%c%x%x%x%.2x%c%d%.2d%.2d%c%.2d%.2d", '_', tThisPhy.ucFrame, tThisPhy.ucShelf, tThisPhy.ucSlot, tThisPhy.ucCpu, '_', tThisTime.usYear, tThisTime.ucMonth, tThisTime.ucDay, '_', tThisTime.ucHour, tThisTime.ucMinute);

    strcpy(acName, CPSS_GCPA_SBBR_FILE_NAME);
    strcat(acName, acBuf);
#else                           /* flash file name is identical */
    /* added by laixf for changed brd_name XAB121 2009-6-29 */
    sprintf(acBuf, "%c%x%x%x%.2x%c%d%.2d%.2d%c%.2d%.2d", '_', tThisPhy.ucFrame, tThisPhy.ucShelf, tThisPhy.ucSlot, tThisPhy.ucCpu, '_', tThisTime.usYear, tThisTime.ucMonth, tThisTime.ucDay, '_', tThisTime.ucHour, tThisTime.ucMinute);

    strcpy(acName, CPSS_RSPA_SBBR_FILE_NAME);
    strcat(acName, acBuf);
    /* added end */
#endif

    strcat(acName, ".log");

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
    cpss_file_delete(acName);
#endif

    lRet = cpss_file_create(acName);
    if (lRet != CPSS_OK)
    {
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
        printf(" creat file  %s fail\n\r", acName);
        return;
#else
        if (lRet != CPSS_ERR_FS_FILE_EXIST)
        {
            printf(" creat flash file  %s fail\n\r", acName);
            return;
        }
        else
        {
            printf(" flash file  %s already exist\n\r", acName);
        }
#endif
    }
    printf("cpss create sbbr file %s ok\n\r", acName);
    ulFileId = cpss_file_open(acName, (CPSS_FILE_OBINARY | CPSS_FILE_ORDWR));
    if (ulFileId == CPSS_FD_INVALID)
    {
        printf(" cpss sbbr open file fail\n\r");
        return;
    }

    usNum = g_pstCpssSbbrSys->usUserNum;
    g_pstCpssSbbrSys->usUserNum = 1;

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)    /* 写入文件头 */
    cpss_file_write(ulFileId, &g_stCpssLogmSBBlogFileHdr, sizeof(g_stCpssLogmSBBlogFileHdr), &ulBytesWrite);
    /* 写入异常信息区 */
    cpss_file_write(ulFileId, g_pucCpssSbbrStartAddr, CPSS_SBBR_EXC_SECT_SIZE + CPSS_SBBR_DRV_SECT_SIZE + sizeof(CPSS_SBBR_SYS_T), &ulBytesWrite);
#else
    cpss_file_write(ulFileId, g_pcSbbrExcInfo, strlen(g_pcSbbrExcInfo), &ulBytesWrite);
#endif

    cpss_file_close(ulFileId);
    printf("cpss sbbr write end\n\r");
    g_pstCpssSbbrSys->usUserNum = usNum;

#endif

}

/*******************************************************************************
* 函数名称: cpss_kw_record_max_occupy
* 功    能:周期记录占用率最高的任务和纤程
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* 
* 函数返回: 
* 说    明: 
*******************************************************************************/
VOID cpss_kw_record_max_occupy(UINT32 ulInterval)
{
    UINT32 ulTaskDesc, ulProcDesc;
    /* 记录自研任务 */
    for (ulTaskDesc = 0; ulTaskDesc < VOS_MAX_TASK_NUM; ulTaskDesc++)
    {
        if (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == TRUE)
        {
            if (g_tCpssOccupyAnalysis[1].ulOccupy <= (100 * g_astCpssVosTaskDescTbl[ulTaskDesc].ulRunTicks) / (2 * 1000 / ulInterval))
            {
                g_tCpssOccupyAnalysis[1].ulOccupy = (100 * g_astCpssVosTaskDescTbl[ulTaskDesc].ulRunTicks) / (2 * 1000 / ulInterval);
                strncpy(g_tCpssOccupyAnalysis[1].acName, g_astCpssVosTaskDescTbl[ulTaskDesc].acName, 32);
            }

            g_astCpssVosTaskDescTbl[ulTaskDesc].ulRunTicks = 0;

        }
    }
    /* 记录vxWorks 任务 */
    for (ulTaskDesc = 0; ulTaskDesc < g_ulCpssVxworksTaskNum; ulTaskDesc++)
    {
        if (g_tCpssOccupyAnalysis[0].ulOccupy <= (100 * g_tCpssVxworksTaskOccupy[ulTaskDesc].ulRunTicks) / (2 * 1000 / ulInterval))
        {
            g_tCpssOccupyAnalysis[0].ulOccupy = (100 * g_tCpssVxworksTaskOccupy[ulTaskDesc].ulRunTicks) / (2 * 1000 / ulInterval);
            strncpy(g_tCpssOccupyAnalysis[0].acName, taskName(g_tCpssVxworksTaskOccupy[ulTaskDesc].lTid), 32);
        }

        g_tCpssVxworksTaskOccupy[ulTaskDesc].ulRunTicks = 0;

    }

    /* 记录纤程 */
    for (ulProcDesc = 0; ulProcDesc < VK_MAX_PROC_CLASS_NUM; ulProcDesc++)
    {
        if (g_astCpssVkProcClassTbl[ulProcDesc].bInuse == TRUE)
        {
            if (g_tCpssOccupyAnalysis[2].ulOccupy <= (100 * g_astCpssVkProcClassTbl[ulProcDesc].ulRunTicks) / (2 * 1000 / ulInterval))
            {
                g_tCpssOccupyAnalysis[2].ulOccupy = (100 * g_astCpssVkProcClassTbl[ulProcDesc].ulRunTicks) / (2 * 1000 / ulInterval);
                strncpy(g_tCpssOccupyAnalysis[2].acName, g_astCpssVkProcClassTbl[ulProcDesc].acName, 32);
            }

            g_astCpssVkProcClassTbl[ulProcDesc].ulRunTicks = 0;
        }
    }

}

/*******************************************************************************
* 函数名称: cpss_kw_record_vxWorks_task_occupy
* 功    能: 统计vxWorks的任务占用率
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* lTid                        INT32                          IN                  任务描述符
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_kw_record_vxWorks_task_occupy(INT32 lTid)
{
    UINT16 usLoop;

    for (usLoop = 0; usLoop < g_ulCpssVxworksTaskNum; usLoop++)
    {
        if (g_tCpssVxworksTaskOccupy[usLoop].lTid == lTid)
        {
            if (taskIsReady(lTid))
            {
                g_tCpssVxworksTaskOccupy[usLoop].ulRunTicks++;
            }
            else
            {
                g_ulTaskStatusErr1++;
            }

            return;
        }
    }

    /* 没有找到,则添加 */
    if (usLoop >= g_ulCpssVxworksTaskNum)
    {
        if (g_ulCpssVxworksTaskNum >= 30)
            return;
        g_tCpssVxworksTaskOccupy[usLoop].lTid = lTid;
        g_tCpssVxworksTaskOccupy[usLoop].ulRunTicks++;
        g_ulCpssVxworksTaskNum++;
    }
}

/*******************************************************************************
* 函数名称: cpss_kw_set_task_run_time
* 功    能: 统计在2秒内任务和现成运行时间
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* 
* 函数返回: 
* 说    明: 
*******************************************************************************/
VOID cpss_kw_set_task_run_time(UINT32 ulInterval)
{

    UINT32 ulTaskDesc, ulSchedDesc;
    UINT16 usGuid;
    VK_SCHED_ID schedId;

    INT32 lTid;

    if ((g_bCpssChkOccupy == FALSE) || (ulInterval <= 0))
        return;

    /* 初始化不统计 */
    if (g_bCpssRecvProcEndInd == FALSE)
    {
        memset(g_tCpssOccupyAnalysis, 0, sizeof(CPSS_MAX_OCCUPY_RECORD_T) * 3);
        memset(g_tCpssVxworksTaskOccupy, 0, sizeof(CPSS_VXWORKS_TASK_OCCUPY_T) * 30);
        return;
    }

    lTid = taskIdSelf();
    if (taskIsReady(lTid) == FALSE)
    {
        g_ulTaskStatusErr++;
        return;
    }

#if 0
    if (g_CpssLoadTaskID != lTid)
    {
        g_CPssLoadTaskInterval++;
        /* 如果在SECONDS_TO_BURN 秒内没有调度load task */
        if (g_CPssLoadTaskInterval >= (2 * 1000 / ulInterval))
        {
            g_CPssLoadTaskInterval = 0;
            /* 设置占用率达到100 */
            g_CpssLoadTaskOverFlow = TRUE;
        }

    }
    else                        /* 如果load task 得到调度 */
    {
        g_CPssLoadTaskInterval = 0;
        g_CpssLoadTaskOverFlow = FALSE;
    }
#endif
    if (++g_ulKwAllTicks >= (2 * 1000 / ulInterval))
    {

        if (g_bCpssShowOccupy == FALSE)
        {
            /* 每2秒统计一下 */
            g_bCpssOccupyOver = FALSE;
            memset(g_tCpssOccupyAnalysis, 0, sizeof(CPSS_MAX_OCCUPY_RECORD_T) * 3);
            cpss_kw_record_max_occupy(ulInterval);
            g_ulKwAllTicks = 0;
            g_bCpssOccupyOver = TRUE;
        }
        else
        {

            g_bCpssOccupyOver = TRUE;
        }
        return;
    }

    for (ulTaskDesc = 1; ulTaskDesc < VOS_MAX_TASK_NUM; ulTaskDesc++)
    {
        /* self task */
        if ((g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == TRUE) && (g_astCpssVosTaskDescTbl[ulTaskDesc].taskId == lTid))
        {
            if (taskIsReady(lTid))
            {
                g_astCpssVosTaskDescTbl[ulTaskDesc].ulRunTicks++;
            }
            else
            {
                g_ulTaskStatusErr2++;
            }
            break;
        }
    }
    /* os task */
    if (ulTaskDesc >= VOS_MAX_TASK_NUM)
    {
        cpss_kw_record_vxWorks_task_occupy(lTid);
    }
    else
    {
        ulSchedDesc = g_astCpssVosTaskDescTbl[ulTaskDesc].ulSchedDesc;
        if (VOS_TASK_DESC_INVALID != ulSchedDesc)   /* sched task */
        {
            schedId = g_astCpssVkSchedDescTbl[ulSchedDesc].tSchedId;    /* 获得当前控制结构 */
            if (NULL != schedId->pIpcMsgCurr)
            {
                usGuid = (UINT16) ((schedId->ulProcDescCurr >> 16) & 0x0000ffff);
                if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM) || (g_astCpssVkProcClassTbl[usGuid].bInuse == FALSE))
                {

                    return;
                }
                if (taskIsReady(lTid))
                {
                    g_astCpssVkProcClassTbl[usGuid].ulRunTicks++;
                }
                else
                {
                    g_ulTaskStatusErr3++;
                }

            }
        }
    }

}

/*******************************************************************************
* 函数名称: cpss_show_task_occupy
* 功    能: 统计在2秒内任务的occupy
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* 
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_show_task_occupy()
{
    UINT32 ulTaskDesc;

    if (g_bCpssChkOccupy == FALSE)
        return;

    if (g_bCpssOccupyOver == FALSE)
        return;
    printf("\n\r%-20s%-15s\n\r", "CpssTaskName", "Occupy(%)");

    for (ulTaskDesc = 0; ulTaskDesc < VOS_MAX_TASK_NUM; ulTaskDesc++)
    {
        if (g_astCpssVosTaskDescTbl[ulTaskDesc].bInuse == TRUE)
        {
            printf("%-20s%-15d\n\r", g_astCpssVosTaskDescTbl[ulTaskDesc].acName, (100 * g_astCpssVosTaskDescTbl[ulTaskDesc].ulRunTicks) / 1000);

            g_astCpssVosTaskDescTbl[ulTaskDesc].ulRunTicks = 0;

        }
    }
    printf("\n\r%-20s%-15s\n\r", "vxWorksTaskName", "Occupy(%)");
    for (ulTaskDesc = 0; ulTaskDesc < g_ulCpssVxworksTaskNum; ulTaskDesc++)
    {
        printf("%-20s%-15d\n\r", taskName(g_tCpssVxworksTaskOccupy[ulTaskDesc].lTid), (100 * g_tCpssVxworksTaskOccupy[ulTaskDesc].ulRunTicks) / 1000);

        g_tCpssVxworksTaskOccupy[ulTaskDesc].ulRunTicks = 0;
    }
    g_ulKwAllTicks = 0;
    g_bCpssOccupyOver = FALSE;

}

/*******************************************************************************
* 函数名称: cpss_kw_set_task_run_time
* 功    能: 统计在2秒内纤程的占用率 
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* 
* 函数返回: 
* 说    明: 
*******************************************************************************/

VOID cpss_show_proc_occupy()
{
    UINT32 ulProcDesc;

    if (g_bCpssChkOccupy == FALSE)
        return;

    if (g_bCpssOccupyOver == FALSE)
        return;

    printf("\n\r%-20s%-15s\n\r", "ProcName", "Occupy(%)");
    for (ulProcDesc = 0; ulProcDesc < VK_MAX_PROC_CLASS_NUM; ulProcDesc++)
    {
        if (g_astCpssVkProcClassTbl[ulProcDesc].bInuse == TRUE)
        {
            printf("%-20s%-15d\n\r", g_astCpssVkProcClassTbl[ulProcDesc].acName, (100 * g_astCpssVkProcClassTbl[ulProcDesc].ulRunTicks) / 1000);

            g_astCpssVkProcClassTbl[ulProcDesc].ulRunTicks = 0;
        }
    }
    g_ulKwAllTicks = 0;
    g_bCpssOccupyOver = FALSE;
}

/*******************************************************************************
* 函数名称: cpss_record_task_stack
* 功    能: 存储任务栈中的信息 
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* lTid                       INT32                          IN               任务描述符
*pcBuf                     char *                         out              存储栈中的信息
* ulBufLen               UINT32                        IN               记录的最大长度
* 函数返回:  实际记录的长度
* 说    明: 
*******************************************************************************/

/* 打印堆栈中前 ulBufLen 个数据到 pcBuf */
UINT32 cpss_record_task_stack(INT32 lTid, CHAR * pcBuf, UINT32 ulBufLen)
{
    WIND_TCB *pstTcb = NULL;
    UINT32 ulBufLimit = 0;

    pstTcb = (WIND_TCB *) lTid;

    ulBufLimit = ulBufLen;
    /* 若栈深度< CPSS_TASK_MAX_RECORD_LENGH,则记录实际的深度 */
    if (ulBufLimit > (pstTcb->pStackBase - pstTcb->pStackEnd))
    {
        ulBufLimit = pstTcb->pStackBase - pstTcb->pStackEnd;
    }

    /* 若栈深度> CPSS_TASK_MAX_RECORD_LENGH,则最大记录CPSS_TASK_MAX_RECORD_LENGH */
    if (ulBufLimit > ulBufLen)
    {
        ulBufLimit = ulBufLen;
    }

    /* 将栈内容复制到临时buf */
    memcpy(pcBuf, pstTcb->pStackBase + sizeof(WIND_TCB) - ulBufLimit, ulBufLimit);
    /* 返回实际的纪录长度 */
    return ulBufLimit;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_write_task_stack_to_sbbr
* 功    能: 存储任务栈中的信螪到sbbr?
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* lTid                       INT32                          IN               任务描述符
* 函数返回: CPSS_OK,CPSS_ERROR
* 说    明: 
*******************************************************************************/

INT32 cpss_sbbr_write_task_stack_to_sbbr(INT32 lTid)
{
    WIND_TCB *pstTcb;
    CHAR *pBuf;
    UINT32 ulWriteLen;
    UINT32 ulArgs[4];
    REG_SET tRegSet;

    pstTcb = taskTcb(lTid);

    if (NULL == pstTcb)
    {
        return CPSS_ERROR;
    }

    pBuf = malloc(CPSS_TASK_MAX_RECORD_LENGH);

    if (pBuf == NULL)
    {
        return CPSS_ERROR;
    }
    memset(pBuf, 0, CPSS_TASK_MAX_RECORD_LENGH);

    ulWriteLen = cpss_record_task_stack(lTid, pBuf, CPSS_TASK_MAX_RECORD_LENGH);
    cpss_sbbr_write_text(0, ulArgs, "Task  %s stack saved from :  Low(High - %d) to High(=stackbase + sizeof(WIND_TCB)) \n\r", taskName(lTid), ulWriteLen);
    cpss_sbbr_write_data(0, ulArgs, pBuf, ulWriteLen);

    cpss_sbbr_write_text(0, ulArgs, "Task  %s stack saved from :Low (sp) to High (sp+10k)\n\r", taskName(lTid));

    free(pBuf);

    if (pstTcb->pExcRegSet != NULL)
    {
        tRegSet = *(pstTcb->pExcRegSet);

    }
    else
    {
        tRegSet = pstTcb->regs;
    }
#if (CPSS_CPU_TYPE == CPSS_CPU_PPC)
    cpss_sbbr_write_data(0, ulArgs, (CHAR *) tRegSet.gpr[1], 10 * 1024);
#endif
#if (CPSS_CPU_TYPE == CPSS_CPU_I86)
    cpss_sbbr_write_data(0, ulArgs, (CHAR *) tRegSet.esp, 10 * 1024);
#endif
#if (CPSS_CPU_TYPE == CPSS_CPU_ARM)
    cpss_sbbr_write_data(0, ulArgs, (CHAR *) tRegSet.r[11], 10 * 1024);
#endif

    cpss_sbbr_write_text(0, ulArgs, "%s\n\r", "Task stack save end");
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_sbbr_save_task_stack
* 功    能: 存储任务的函数调用栈
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* lTid                       INT32                          IN               任务描述符
* 函数返回: 
* 说    明: 
*******************************************************************************/
VOID cpss_sbbr_save_task_stack(INT32 lTid)
{
    UINT16 usLoop;
    UINT32 ulArgs[4];

    if (taskTcb(lTid) == NULL)
        return;

    cpss_mem_memset(g_pcSbbrExcInfo, 0, CPSS_SBBR_EXC_BUF_LENGTH);

    cpss_sbbr_tt(lTid);
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, "  task %s Fun Stack  Info start\n\n", taskName(lTid));

    for (usLoop = 0; usLoop < g_lcurCallDep; usLoop++)
    {
        cpss_sbbr_sprintf(g_pcSbbrExcInfo, "%s\n\r", g_azCpssSbbrTempBuf[usLoop]);
    }
    cpss_sbbr_sprintf(g_pcSbbrExcInfo, " task %s  Fun Stack Info End\n\n", taskName(lTid));

    cpss_sbbr_write_text(0, ulArgs, "%s\n\n", g_pcSbbrExcInfo);
    /* 清本次的跟踪信息 */
    g_lcurCallDep = 0;
    cpss_mem_memset(g_pcSbbrExcInfo, 0, CPSS_SBBR_EXC_BUF_LENGTH);
    cpss_mem_memset(g_azCpssSbbrTempBuf, 0, 15 * 400);

}

/*******************************************************************************
* 函数名称: cpss_sbbr_save_Msched_task_stack
* 功    能: 存储主任务栈
* 函数类型: 
* 参    数:              类型                      IN/OUT          描述
* 函数返回: 
* 说    明: 
*******************************************************************************/
VOID cpss_sbbr_save_Msched_task_stack(VOID)
{
    UINT16 usLoop;

    /* 初始化任务用户区 */
    for (usLoop = 1; usLoop < VOS_MAX_TASK_NUM; usLoop++)
    {
        if (g_astCpssVosTaskDescTbl[usLoop].bInuse == TRUE)
        {

            if (!strncmp(g_astCpssVosTaskDescTbl[usLoop].acName, SWP_SCHED_M_TASKNAME, strlen(SWP_SCHED_M_TASKNAME)))
            {
                cpss_sbbr_write_task_stack_to_sbbr(g_astCpssVosTaskDescTbl[usLoop].taskId);
                break;
            }

        }
    }
}

#endif

#else
VOID cpss_kw_read_proc_class_info(UINT16 usGuid)
{
    return;
}

INT32 cpss_kw_ha_init()
{
    return CPSS_OK;
}

VOID cpss_kw_read_normal_task_info(UINT32 ulTaskDesc, UINT32 ulConfig)
{
    return;
}

VOID cpss_sbbr_wait_rsp_expire_proc(VOID)
{
    return;
}

BOOL cpss_sbbr_satisfy_auto_transfer(VOID)
{
    return TRUE;
}

VOID cpss_sbbr_trans_sbbr_data(VOID)
{
    return;
}

INT32 cpss_sbbr_rsp_msg_proc(CPSS_SBBR_RSP_MSG_T * pstRspMsg)
{
    return CPSS_OK;
}

VOID cpss_sbbr_transation_proc(UINT32 ulCpuNo)
{
    return;
}

VOID cpss_kw_set_task_run(VOID)
{
    return;
}

VOID cpss_kw_set_task_stop(VOID)
{
    return;
}

VOID cpss_kw_set_proc_run(UINT16 usGuid)
{
    return;
}

VOID cpss_kw_read_sched_task_info(UINT32 ulTaskDesc, CHAR * pName)
{
    return;
}
#endif
