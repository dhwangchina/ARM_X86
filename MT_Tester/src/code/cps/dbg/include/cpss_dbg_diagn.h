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
#ifndef CPSS_DBG_DIAGN_H
#define CPSS_DBG_DIAGN_H
#include "cpss_mm_mem.h"
#include "cps_replant.h"


/********************************* 头文件包含 ********************************/

/******************************** 宏和常量定义 *******************************/

/******************************** 类型定义 ***********************************/
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
    UINT8          aucRptInfo[sizeof(UINT32)+ CPSS_MAX_MEM_TAB_SUM * sizeof(CPSS_DBG_MEM_POOL_INFO_T)]; /* 报告信息 */
    
}CPSS_DBG_MEM_CPS__OAMS_O_DIAG_RSP_T;


/*内存池信息结构*/
typedef struct tagCPSS_MEM_RSP_INFO
{
    UINT32 ulMemPoolCnt;                    /*内存池数量*/
    CPSS_DBG_MEM_POOL_INFO_T astMemPools[CPSS_MAX_MEM_TAB_SUM];   /* 内存池信息*/
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

typedef struct tagCPSS_DISK_SPACE_INFO
{
  /*  CHAR acName[CPS__OAMS_DIAG_DIRNAME_MAX_LEN];*/
    UINT32 ulAllSizeHigh;
    UINT32 ulAllSizeLow;
    UINT32  ulUsedSizeHigh;
    UINT32  ulUsedSizeLow;
      
}CPSS_DISK_SPACE_INFO_T;


typedef struct tagCPSS_DISK_SAPCE_RSP_T
{
    CPS__OAMS_M_REQ_HDR_T tReqHeader;    /* 消息头*/ 
    UINT32        ulTestId;    /* 测试活动ID */
    UINT32        ulRptType;    /* 测试报告类型(中间、最终) */
    UINT32        ulTestResult; /* 测试结果(成功、失败) */ 
    UINT32        ulTestRptLen; /* 报告长度，最大为1024字节 */
    UINT8          aucRptInfo[sizeof(CPSS_DISK_SPACE_INFO_T)]; /* 报告信息 */
    
}CPSS_DBG_DISK_SPACE_RSP_INFO_T;
typedef struct tagCPSS_DIR_INFO
{
    CHAR acName[CPS__OAMS_DIAG_DIRNAME_MAX_LEN];
     UINT32 ulSizeHigh;
   UINT32 ulSizeLow;
   UINT32 ulFileNum; 
   
      
}CPSS_DIR_INFO_T;
typedef struct tagCPSS_DIR_RSP_T
{
    CPS__OAMS_M_REQ_HDR_T tReqHeader;    /* 消息头*/ 
    UINT32        ulTestId;    /* 测试活动ID */
    UINT32        ulRptType;    /* 测试报告类型(中间、最终) */
    UINT32        ulTestResult; /* 测试结果(成功、失败) */ 
    UINT32        ulTestRptLen; /* 报告长度，最大为1024字节 */
    UINT8          aucRptInfo[sizeof(CPSS_DIR_INFO_T)]; /* 报告信息 */
    
}CPSS_DBG_DIR_RSP_INFO_T;

typedef struct tagCPSS_MEM_OCCUPY_INFO
{
    
   UINT32 ulTotalMem;
   UINT32 ulFreeMem; 
   
      
}CPSS_MEM_OCCUPY_INFO_T;

typedef struct tagCPSS_MEM_OCCUPY_RSP_T
{
    CPS__OAMS_M_REQ_HDR_T tReqHeader;    /* 消息头*/ 
    UINT32        ulTestId;    /* 测试活动ID */
    UINT32        ulRptType;    /* 测试报告类型(中间、最终) */
    UINT32        ulTestResult; /* 测试结果(成功、失败) */ 
    UINT32        ulTestRptLen; /* 报告长度，最大为1024字节 */
    UINT8          aucRptInfo[sizeof(CPSS_MEM_OCCUPY_INFO_T)]; /* 报告信息 */
    
}CPSS_DBG_MEM_OCCUPY_RSP_INFO_T;
/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/

extern INT32 cpss_dbg_test_data_check(UINT8 *pucBuf, UINT32 ulLen);
extern VOID cpss_dbg_cps__oams_diag_req_msg_proc(CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern INT32 cpss_com_diag_comm_test_req_deal(UINT8* pucRecvBuf,UINT32 ulSeqId,UINT32 ulOamsProc,UINT32 ulTestId);
extern INT32 cpss_dbg_cps__oams_diag_stop_test_msg_deal(UINT8* pucRecvMsg) ;
extern VOID cpss_com_comm_stop_test_deal(UINT32 ulTestId) ;
/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_DBG_DIAGN_H */
/******************************** 头文件结束 *********************************/

