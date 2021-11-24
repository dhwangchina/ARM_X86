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
#ifndef CPSS_DBG_H
#define CPSS_DBG_H

/********************************* 头文件包含 ********************************/

#include "cpss_com.h"
#include "cpss_tm.h"

/******************************** 宏和常量定义 *******************************/
#if 0
/* 复位原因, 用于cpss_reset */
#define CPSS_RESET_CPS__OAMS_FORCE           ((UINT32)(1))   /* 网管下达强制复位 */
#define CPSS_RESET_SMSS_SWITCH_OVER     ((UINT32)(2))   /* 主备倒换复位 */
#define CPSS_RESET_SMSS_FAULT_DETECT    ((UINT32)(3))   /* SMSS检测到致命故障 */
#define CPSS_RESET_SMSS_INIT_FAILURE    ((UINT32)(4))   /* SMSS初始化失败 */
#define CPSS_RESET_CPS__RDBS_SYN_FAILURE     ((UINT32)(5))   /* RDBS数据同步失败 */
#endif

/* cpss_sbbr */
#define CPSS_SBBR_USERID_INVALID            0xFFFFFFFF  /* 无效的用户ID */
/* cpss_sbbr_start_transaction, 触发上传黑匣子的原因 */
#define CPSS_SBBR_TRANSACTION_FORCE            1  /* 手动下达的上传 */
#define CPSS_SBBR_TRANSACTION_AUTO             2  /* 自动触发的上传 */
/* 替换各个自研代码中的return语句的宏定义。这些宏定义可以通过编译开关确定是否记录黑匣子 */


#define CPSS_RETURN(value) {\
   return(value);\
  }

#define CPSS_RETURN_OK {\
    return (CPSS_OK);\
}

#define CPSS_RETURN_ERROR {\
    return (CPSS_ERROR);\
}
#define CPSS_RETURN_VOID {\
   return ;\
}

/* 黑匣子传输消息ID定义 */
#define CPSS_SBBR_REQ_MSG      ((UINT32)(0x21000203))
#define CPSS_SBBR_RSP_MSG      ((UINT32)(0x21000204))

/******************************** 打印部分开始 *******************************/

/* 打印信息上报,每个数据包长度,单位：字节 */
#define CPSS_PRINT_PACKET_LEN   ((UINT16)(1024))

/* 每条打印信息最大长度 */
#define CPSS_MAX_PRINT_LEN (1024-sizeof(CPSS_PRINT_ENTRY_T)-sizeof(CPSS_PRINT_HEAD_T))

#define CPSS_PRINT_OFF     ((UINT8)(0x00))   /* 打印级别：关闭打印 */
#define CPSS_PRINT_FATAL   ((UINT8)(0x01))   /* 打印级别：致命错误 */
#define CPSS_PRINT_FAIL    ((UINT8)(0x02))   /* 打印级别：业务失败 */
#define CPSS_PRINT_ERROR   ((UINT8)(0x03))   /* 打印级别：一般性错误 */
#define CPSS_PRINT_WARN    ((UINT8)(0x04))   /* 打印级别：警告信息 */
#define CPSS_PRINT_INFO    ((UINT8)(0x05))   /* 打印级别：一般信息*/
#define CPSS_PRINT_DETAIL  ((UINT8)(0x06))   /* 打印级别：详细信息*/
#define CPSS_PRINT_IMPORTANT  ((UINT8)(0x07))   /* 打印级别：只记日志不打印*/
/******************************** 打印部分结束 *******************************/

/******************************** 日志部分开始 *******************************/

/* 日志信息上报,一个数据包长度,单位：字节 */
#define CPSS_LOG_PACKET_LEN   ((UINT16)(1024))

/* 每条日志信息最大长度 */
#define CPSS_MAX_LOG_LEN (1024-sizeof(CPSS_LOG_ENTRY_T)-sizeof(CPSS_LOG_HEAD_T))

#define CPSS_LOG_OFF     ((UINT8)(0x00))    /* 日志级别：关闭日志 */
#define CPSS_LOG_FATAL   ((UINT8)(0x01))    /* 日志级别：致命错误 */
#define CPSS_LOG_FAIL    ((UINT8)(0x02))    /* 日志级别：业务失败 */
#define CPSS_LOG_ERROR   ((UINT8)(0x03))    /* 日志级别：一般性错误 */
#define CPSS_LOG_WARN    ((UINT8)(0x04))    /* 日志级别：警告信息 */
#define CPSS_LOG_INFO    ((UINT8)(0x05))    /* 日志级别：一般信息 */
#define CPSS_LOG_DETAIL  ((UINT8)(0x06))    /* 日志级别：详细信息 */

#define _FILE_LINE_(ucSubsystemID, ucModuleID, ucPrintLevel) \
        cpss_print(ucSubsystemID, ucModuleID, ucPrintLevel, "File: %s, Line:%d ",__FILE__, __LINE__)

/******************************** 日志部分结束 *******************************/

/******************************** 类型定义 ***********************************/

/******************************** 黑匣子部分开始 *****************************/

/* 黑匣子数据传输消息 */
typedef struct tagCPSS_SBBR_REQ_HEAD 
{
    UINT32 ulSeqID;      /* M接口消息流水号（固定为0）*/
    CPSS_COM_PHY_ADDR_T stPhyAddr;  /* 板卡物理地址 */
    UINT8 ucBoardType;  /* 功能板类型信息，以方便显示 */ 
    UINT8 ucReserved[3];
    UINT32 ulTotalLen;  /* 黑匣子数据总长度 */

    /* 本消息体内黑匣子数据相对于黑匣子数据头的偏移量,首次发送为0 */ 
    UINT32 ulOffset;    
    UINT32 ulInfoLen;  /* 本消息体内的黑匣子数据长度 */ 
}CPSS_SBBR_REQ_HEAD_T;
/* 紧接着是长度为ulInfoLen字节的黑匣子数据，为二进制格式 */


/* 黑匣子数据传输响应消息 */
typedef struct tagCPSS_SBBR_RSP_MSG
{
    UINT32 ulSeqID;      /* M接口消息流水号（固定为0）*/
    UINT32 ulTotalLen;         /* 黑匣子数据总长度 */

    /* 期望数据偏移量, OAMS期望CPSS发送的下一个数据包的数据偏移量, CPSS从此偏移
     * 量开始发送.如果ulExpectedOffset == ulTotalLen, 则黑匣子数据传送完成
     */
    UINT32 ulExpectedOffset;  
}CPSS_SBBR_RSP_MSG_T;

typedef VOID (*CPSS_SBBR_HOOK_PF) (UINT32 ulType, UINT32 ulExecInfo);  /*黑匣子回调函数*/
/******************************** 黑匣子部分结束 *****************************/

/******************************** 打印部分开始 *******************************/
typedef struct tagCPSS_PRINT_ENTRY
{
    UINT32 ulSerialID;    /* 流水号 */
    UINT32 ulTick;        /* tick数 */
    CPSS_TIME_T stTime;   /* 绝对时间 */
    UINT8   ucSubSystemID;    /* 子系统号 */
    UINT8   ucModuleID;       /* 模块号 */
    UINT8   ucPrintLevel;     /* 打印级别, 0~5 */
    UINT8   ucPrintFormat;    /* 打印格式, 目前固定填0(打印内容为字符串) */
    UINT32  ulInfoLen;        /* 打印信息体的长度 */
}CPSS_PRINT_ENTRY_T;
/* 紧接着是长度为ulInfoLen字节的打印内容，为文本格式 */
    
typedef struct tagCPSS_PRINT_HEAD
{
    UINT32 ulSeqID;       /* O接口消息流水号（固定为0）*/
    UINT32  ulPrintID;    /* 打印标识，由OAMS提供 */
    CPSS_COM_PHY_ADDR_T stPhyAddr;  /* 板卡物理地址 */
    UINT8 ucBoardType;    /* 功能板类型信息，以方便显示 */
    UINT8 ucReserved;
    UINT16 usInfoNum;     /* 打印信息项数目 */
}CPSS_PRINT_HEAD_T; 
/* 紧接着是usInfoNum个打印信息，打印信息以CPSS_PRINT_ENTRY_T开头 */


/******************************** 打印部分结束 *******************************/

/******************************** 日志部分开始 *******************************/
typedef struct tagCPSS_LOG_ENTRY
{
    UINT32 ulSerialID;    /* 流水号 */
    UINT32 ulTick;         /* tick数 */
    CPSS_TIME_T stTime;  /* 绝对时间 */
    UINT8  ucSubSystemID;   /* 子系统号 */
    UINT8  ucModuleID;       /* 模块号 */
    UINT8  ucLogLevel;       /* 日志级别, 0~5 */
    UINT8  ucLogFormat;      /* 日志格式, 目前固定填0(日志内容为字符串) */
    UINT32 ulInfoLen;        /* 日志信息体的长度 */
}CPSS_LOG_ENTRY_T;
/* 紧接着是长度为ulInfoLen字节的日志内容，为文本格式 */

typedef struct tagCPSS_LOG_HEAD
{
    UINT32 ulSeqID;    /* M接口消息流水号（固定为0）*/
    CPSS_COM_PHY_ADDR_T stPhyAddr;  /* 板卡物理地址 */
    UINT8 ucBoardType;      /* 功能板类型信息，以方便显示 */
    UINT8 ucReserved;
    UINT16 usInfoNum;  /* 日志信息项数目 */
}CPSS_LOG_HEAD_T;
/* 紧接着是usInfoNum个日志信息，日志信息以CPSS_LOG_ENTRY_T开头 */

/******************************** 日志部分结束 *******************************/

/******************************** 全局变量声明 *******************************/
/*extern */
/******************************** 外部函数原形声明 ***************************/

/******************************** 打印部分开始 *******************************/
extern INT32 cpss_print
(
    UINT8  ucSubSystemID,
    UINT8  ucModuleID,
    UINT8  ucPrintLevel,
    STRING  szFormat,
    ...
);
extern INT32 cpss_print_level_set
(
    UINT8 ucSubSystemID,
    UINT8 ucModuleID,
    UINT8 ucPrintLevel
);
extern INT32 cpss_print_redirect_set
(
    UINT32 ulPrintID, 
    CPSS_COM_PID_T *pstPid
);
extern INT32 cpss_print_rate_set
(     
    UINT32 ulRate 
);

/* 获得打印级别 */
extern INT32 cpss_print_level_get
(
    UINT8   ucSubSystemID,
    UINT8   ucModuleID,
    UINT8   *pucPrintLevel
);


extern INT32 cpss_print_lost_info_get
(
    UINT32 *pulPrtLostInfo
);
extern INT32 cpss_print_level_set_dsp
(
 UINT8 ucCpuNo,
 UINT8 ucSubSystemID,
 UINT8 ucModuleID,
 UINT8 ucPrintLevel
 );
extern INT32 cpss_print_redirect_set_dsp
(
UINT8 ucCpuNo,
UINT32 ulPrintID,
CPSS_COM_PID_T *pstPid
);
extern INT32 cpss_print_rate_set_dsp 
(
UINT8 ucCpuNo,
UINT32 ulRate
);

extern INT32 cpss_vprint
(
 UINT8   ucSubSystemID,
 UINT8   ucModuleID,
 UINT8   ucPrintLevel,
 STRING  szFormat,
 va_list argptr
 );

extern INT32 cpss_vsnprintf (char *buf, INT32 size, const char *format, va_list args);

/******************************** 打印部分结束 *******************************/

/******************************** 日志部分开始 *******************************/
#if 0
extern INT32 cpss_log
(
    UINT8  ucSubSystemID,
    UINT8  ucModuleID,
    UINT8  ucLogLevel,
    STRING  szLogInfo,
    ...
);
#endif 
extern INT32 cpss_log_rate_set
(
    UINT32 ulRate
);

extern INT32 cpss_log_level_set
(
    UINT8 ucLogLevel
);

extern INT32 cpss_log_lost_info_get
(
    UINT32 *pulLogLostInfo
);
#if 0
extern VOID cpss_dbg_print_module_register
(
 UINT32 ulSubSysNum, 
 STRING szModuleNameInfo[], 
 UINT32 ulModuleMax

);
#endif
extern INT32 cpss_log_rate_set_dsp 
(
UINT8 ucCpuNo,
UINT32 ulRate 
);
extern INT32 cpss_log_level_set_dsp 
(
UINT8 ucCpuNo,
UINT8 ucLogLevel
);
/******************************** 日志部分结束 *******************************/

/******************************** 黑匣子部分开始 *******************************/
extern UINT32 cpss_sbbr_register
(
    CPSS_SBBR_HOOK_PF pfWriteHook
);
extern INT32 cpss_sbbr_write_text
(
   UINT32 ulUserId, 
   UINT32 ulArgs[4], 
   STRING szFormat, 
   ...
);
extern INT32 cpss_sbbr_write_data
(
    UINT32 ulUserId, 
    UINT32 ulArgs[4], 
    UINT8 *pucData,
    UINT32 ulLen
);
extern INT32 cpss_sbbr_start_transaction
(
UINT32 ulCpuNo 
);
/********************************  黑匣子部分结束*******************************/

extern INT32 cpss_cpu_used_get
( 
    UINT32 *pulUsage
);

extern INT32 cpss_reset
( 
    UINT32 ulCause
);

typedef struct tagCPSS_LOG_INFO
{
    CPSS_LOG_ENTRY_T stLogEntry;  /* 每条日志信息结构 */
    INT8 cBuf[CPSS_LOG_PACKET_LEN];  /* 日志信息体 */
}CPSS_LOG_INFO_T;

/********************************  调试记录时间 *******************************/

extern void cpss_dbg_high_time_init();
extern UINT32 cpss_dbg_high_time_get();

#define CPSS_DBG_REC_TIME_ON

#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP) && (defined CPSS_DBG_REC_TIME_ON)
    #undef CPSS_DBG_REC_TIME_ON
#endif

#if defined CPSS_DBG_REC_TIME_ON

extern INT32 cpss_dbg_rec_time_delete(UINT32 ulRecId);
extern INT32 cpss_dbg_rec_time_delete_all();
extern INT32 cpss_dbg_rec_time_add(UINT32 ulRecId);
extern INT32 cpss_dbg_rec_time_add_all();
extern INT32 cpss_dbg_rec_time_task_show();
extern INT32 cpss_dbg_rec_time_show(UINT32 ulRecId, UINT32 ulMinTick, UINT32 ulMinStartTick, UINT32 ulMaxStartTick);

extern UINT32 cpss_dbg_rec_time_reg(UINT8 *pucRecName, UINT32 ulRecSum, UINT32 ulIsNeedLock);
extern INT32 cpss_dbg_rec_time_start(UINT32 ulRecId, UINT32 ulPara, UINT32 ulDataLen, UINT8 *pucDataBuf);
extern INT32 cpss_dbg_rec_time_end(UINT32 ulRecId, UINT32 ulResult, UINT32 ulPara);
extern INT32 cpss_dbg_rec_time_init();

#else

#define cpss_dbg_rec_time_reg(A, B, C) 0
#define cpss_dbg_rec_time_start(A, B, C, D) 0
#define cpss_dbg_rec_time_end(A, B, C) 0
#define cpss_dbg_rec_time_init() 0

#endif

/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_DBG_H */
/******************************** 头文件结束 *********************************/

