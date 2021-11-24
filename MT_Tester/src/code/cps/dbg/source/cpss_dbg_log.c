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
#include "cpss_err.h"
#include "cpss_vos_sem.h"
#include "cpss_dbg_log.h"

/******************************* 局部宏定义 ***********************************/

/******************************* 全局变量定义/初始化 **************************/
CPSS_LOG_MANAGE_T g_stCpssDbgLogManage;                /* 日志管理信息 */
BOOL g_bCpssDbgLogValid = FALSE;                       /* 日志是否可用标识 */
#ifndef CPSS_DSP_CPU
extern CPSS_COM_LOGIC_ADDR_T g_stLogicAddrGcpa;        /* 全局板逻辑地址 */
CPSS_COM_PHY_ADDR_T g_stCpssDbgLogPhyAddr;      /* 本板物理地址 */
extern  BOOL g_bCpssRecvProcEndInd ;                /* 是否收到激活完成*/
extern CPSS_COM_PHY_ADDR_T   g_stPhyAddrGcpaMaster;   /* 主控板物理地址*/
extern INT32 cpss_com_ldt_send(UINT8* pucSendBuf,UINT32 ulSendLen);  /* 通信发往ldt的函数*/

/******************************* 局部常数和类型定义 ***************************/

typedef struct tagCPSS_SAVE_LOG
{
   UINT32 ulLen;                /* 存储日志异常时的处理的长度*/
   UINT32 ulInfoLen;        /* 存储日志异常时实际的长度*/
   CHAR  acCpssLogInfo[1024];  /* 异常日志信息体*/
}CPSS_SAVE_LOG_T;

CPSS_SAVE_LOG_T g_tCpssSaveLog1 = {0};    /* 在外围板存储日志异常结构 */
CPSS_SAVE_LOG_T g_tCpssSaveLog2 = {0};   /* 在主控板 存储日志异常结构*/
CPSS_COM_PID_T g_tCpssLogRecord ={0,0,0};   /* 记录发送异常日志的纤程*/
/******************************* 局部函数原型声明 *****************************/

/******************************* 函数实现 *************************************/

/*******************************************************************************
* 函数名称: cpss_log_init
* 功    能: 日志部分初始化
* 函数类型: 
* 参    数: 无。
* 参数名称            类型        输入/输出         描述
* 
* 函数返回: 初始化成功返回CPSS_OK, 失败返回CPSS_ERROR.
* 说    明: 
*******************************************************************************/
INT32 cpss_log_init()
{
    INT32 lLogLevelSet = 0;
    INT32 lLogRateSet = 0;
    /* 初始化日志信息管理结构内容 */
    g_stCpssDbgLogManage.ulLogID = 0;          /* 日志流水号 */
    g_stCpssDbgLogManage.pucLogBufHead = NULL; /* 日志缓存 */
    g_stCpssDbgLogManage.pucLogDataAdd = NULL; 
    g_stCpssDbgLogManage.pucLogDataHead = NULL;
    g_stCpssDbgLogManage.pucLogDataTail = NULL;
    g_stCpssDbgLogManage.ulLogTimerPeriod = 1000;
    g_stCpssDbgLogManage.lLogTimerID = CPSS_ERROR;
    g_stCpssDbgLogManage.ulLogToken = 0;
    g_stCpssDbgLogManage.ulLogTokenValid = 0;
    g_stCpssDbgLogManage.ucLogLevel = CPSS_LOG_DETAIL;
    g_stCpssDbgLogManage.ulLogInfoLostNum = 0;
    g_stCpssDbgLogManage.ulSemForLogLost = VOS_SEM_DESC_INVALID;
    
    /* 初始化日志中用到的信号量 */
    g_stCpssDbgLogManage.ulSemForLogLost = cpss_vos_sem_m_create(VOS_SEM_Q_FIFO);
    if (VOS_SEM_DESC_INVALID == g_stCpssDbgLogManage.ulSemForLogLost)
    {
        return (CPSS_ERROR);
    }
    
    cpss_com_phy_addr_get(&g_stCpssDbgLogPhyAddr);
    /* 调试用 2006/4/5*/
    g_stCpssDbgLogManage.stLogDstPid.stLogicAddr = g_stLogicAddrGcpa;
    g_stCpssDbgLogManage.stLogDstPid.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER;
    g_stCpssDbgLogManage.stLogDstPid.ulPd = CPS__OAMS_PD_M_LOGM_PROC;
    /* 2006/4/5 ended */
    
    /* 初始化日志缓冲区 */
    if (g_stCpssDbgLogManage.pucLogBufHead != NULL)
    {
        cpss_mem_free(g_stCpssDbgLogManage.pucLogBufHead);
    }
    g_stCpssDbgLogManage.pucLogBufHead = (UINT8 *)cpss_mem_malloc(CPSS_LOG_BUF_LEN);    
    if (g_stCpssDbgLogManage.pucLogBufHead == NULL)
    {
        return (CPSS_ERROR);
    }
    
    g_stCpssDbgLogManage.pucLogDataHead = NULL;            /* 初始日志缓冲队列为空 */
    g_stCpssDbgLogManage.pucLogDataTail = g_stCpssDbgLogManage.pucLogBufHead; /* 日志数据尾节点指针 */
    g_stCpssDbgLogManage.pucLogDataAdd = g_stCpssDbgLogManage.pucLogDataTail; /* 日志数据加入处指针 */
    
    /* 初始化日志级别、日志上报速率 */
    lLogLevelSet = cpss_log_level_set(CPSS_LOG_WARN);
    lLogRateSet = cpss_log_rate_set(50);
    if ((CPSS_OK != lLogLevelSet)  || (CPSS_OK != lLogRateSet))
    {
        cpss_mem_free(g_stCpssDbgLogManage.pucLogBufHead);
        return (CPSS_ERROR);
    }
    g_stCpssDbgLogManage.bLogSendtoMate = FALSE; /* 打印重定向到本板表示不需要发送到伙伴板 */
    cpss_log_redirect_set();
    
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_dbg_log_init_send
* 功    能: 纤程初始化过程中发送日志
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出       描述
* pucBuf                       CHAR *          输入                  日志信息体
* ulLen                          UINT32          输入                   日志长度
* 。
* 函数返回: 成功:CPSS_OK;失败:CPSS_ERROR;
* 说    明: 其它模块调用该函数实现对日志的上报。
*******************************************************************************/    
INT32  cpss_dbg_log_init_send(CHAR *pucBuf,UINT32 ulLen)
{


   CPSS_LOG_HEAD_T stLogHead;
   CHAR acBuf[1024]={0};
   
    if (((CPSS_LOG_ENTRY_T *)pucBuf)->ulInfoLen > CPSS_MAX_LOG_LEN)
    {
        return (CPSS_ERROR);
    }

    /*  填充日志流水号*/
    ((CPSS_LOG_ENTRY_T *)pucBuf)->ulSerialID = g_stCpssDbgLogManage.ulLogID;
    
    g_stCpssDbgLogManage.ulLogID = g_stCpssDbgLogManage.ulLogID + 1;
    /* 填充日志源绝对时间 */    
    cpss_clock_get(&(((CPSS_LOG_ENTRY_T *)pucBuf)->stTime));
   /* 填充日志头*/

    stLogHead.ulSeqID = 0;        /* 固定为0 */

    cpss_com_phy_addr_get(&stLogHead.stPhyAddr);

    stLogHead.ucBoardType = SWP_FUNBRD_TYPE;    

     stLogHead.usInfoNum = 1;     
  /* 写入日志头*/
    cpss_mem_memcpy(acBuf,&stLogHead,  sizeof(CPSS_LOG_HEAD_T));
 /* 写入日志体*/
    cpss_mem_memcpy(acBuf +sizeof(CPSS_LOG_HEAD_T),pucBuf,ulLen);

        
    #ifdef CPSS_FUNBRD_MC
       return cpss_com_ldt_send(acBuf,sizeof(CPSS_LOG_HEAD_T) + ulLen);
        
    #else
      return cpss_com_send_phy(g_stPhyAddrGcpaMaster,CPSS_DBG_MNGR_PROC,CPSS_DBG_INIT_LOG_MSG,
                                          acBuf,sizeof(CPSS_LOG_HEAD_T) + ulLen);

    #endif 
}

/*******************************************************************************
* 函数名称: cpss_log
* 功    能: 日志信息的采集与上报
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出       描述
* ucSubSystemID       UINT8       输入            子系统号
* ucModuleID          UINT8       输入            模块号
* ucLogLevel          UINT8       输入            日志信息级别
* szLogInfo           STRING      输入            格式字符串，与printf相同
* ...                             输入            和szFormat配合的可变参数。
* 函数返回: 本次日志记录的字符个数,否则返回CPSS_ERROR。
* 说    明: 其它模块调用该函数实现对日志的上报。
*******************************************************************************/    

 INT32 cpss_log
(
 UINT8 ucSubSystemID,
 UINT8 ucModuleID,
 UINT8 ucLogLevel,
 STRING szLogInfo,
 ...
 )
{
    CPSS_LOG_INFO_T stLogInfo;
    va_list args;
    INT32 lComSendPhy = 0;
    INT32 lSemP = 0;
    UINT32 ulInfoLen;

  #if 0  
    if ((g_bCpssDbgLogValid == FALSE) || (ucLogLevel > g_stCpssDbgLogManage.ucLogLevel))
    {
        return (0);
    }
  #endif
    

    /* 判断进入的各个参数及合法性 */
    /* 判断日志级别、子系统号、模块号参数是否合法 */
    if ((ucLogLevel == CPSS_LOG_OFF) 
        || (ucSubSystemID >= SWP_SUBSYS_MAX_NUM)        
        || (ucModuleID >= SWP_MODULE_MAX_NUM))
    {
        return (CPSS_ERROR);
    }

#if 0 

    memset(&stLogInfo, 0, sizeof(CPSS_LOG_INFO_T));	

#endif 
    va_start(args, szLogInfo);
    vsnprintf(stLogInfo.cBuf, CPSS_LOG_PACKET_LEN, szLogInfo, args);
    va_end(args);
    
    /* 填充日志信息结构体 */
    /* stLogEntry.ulSerialID = g_ulLogID++; */                 /* 填充日志流水号，放到数据加入缓存时再做 */
    stLogInfo.stLogEntry.ulTick = cpss_tick_get();             /* 填充tick时间 */
    /*cpss_clock_get(&(stLogInfo.stLogEntry.stTime));*/        /* 填充日志源绝对时间 */
    stLogInfo.stLogEntry.ucSubSystemID = ucSubSystemID;        /* 填充子系统号 */
    stLogInfo.stLogEntry.ucModuleID = ucModuleID;              /* 填充模块号 */
    stLogInfo.stLogEntry.ucLogLevel = ucLogLevel;              /* 填充日志级别 */
    stLogInfo.stLogEntry.ucLogFormat = 0;                      /* 日志格式, 目前固定填0(日志内容为字符串) */
    stLogInfo.stLogEntry.ulInfoLen = strlen(stLogInfo.cBuf) + 1;    /* 日志信息体的长度 */

#ifndef CPSS_HOST_CPU
    if(0 != CPSS_MOD( stLogInfo.stLogEntry.ulInfoLen ,4))
	{
	   stLogInfo.stLogEntry.ulInfoLen += (4-CPSS_MOD( stLogInfo.stLogEntry.ulInfoLen ,4));
	}
#endif	

    /* 限制信息体的长度 */
    if (stLogInfo.stLogEntry.ulInfoLen > CPSS_MAX_LOG_LEN)
    {
        stLogInfo.stLogEntry.ulInfoLen = CPSS_MAX_LOG_LEN;
    }

 /* proc init state  */
   if(g_bCpssRecvProcEndInd == FALSE)
    {
         cpss_dbg_log_init_send((UINT8 *)(&stLogInfo),
        stLogInfo.stLogEntry.ulInfoLen + sizeof(CPSS_LOG_ENTRY_T));
    }

            if (g_bCpssDbgLogValid == FALSE) 
           {
               return 0;
           }

  ulInfoLen = 	stLogInfo.stLogEntry.ulInfoLen ;
  stLogInfo.stLogEntry.ulInfoLen = cpss_htonl(stLogInfo.stLogEntry.ulInfoLen);
   stLogInfo.stLogEntry.ulTick     = cpss_htonl( stLogInfo.stLogEntry.ulTick );
  
    /* 准备发送消息 */   
    /* 判断发送结果，如为失败则返回打印失败 */
#ifndef CPSS_DSP_CPU
    lComSendPhy = cpss_com_send_phy(g_stCpssDbgLogPhyAddr,
        CPSS_DBG_MNGR_PROC,
        CPSS_LOG_ADD_MSG,
        (UINT8 *)(&stLogInfo),
        ulInfoLen + sizeof(CPSS_LOG_ENTRY_T));
#else
{
 
 CPSS_COM_PID_T tDstPid;
 
 cpss_com_host_logic_addr_get(&tDstPid.stLogicAddr, &tDstPid.ulAddrFlag);
 
 tDstPid.ulPd = CPSS_DBG_MNGR_PROC;
 	
 lComSendPhy = cpss_com_send(&tDstPid,
        CPSS_LOG_ADD_MSG,
       (UINT8 *)(&stLogInfo),
          ulInfoLen+ sizeof(CPSS_LOG_ENTRY_T));
}
#endif

    if (CPSS_OK != lComSendPhy)
    {
        lSemP = cpss_vos_sem_p(g_stCpssDbgLogManage.ulSemForLogLost, NO_WAIT);
        if (lSemP == CPSS_OK)
        {
            g_stCpssDbgLogManage.ulLogInfoLostNum = g_stCpssDbgLogManage.ulLogInfoLostNum + 1;        
        }
        cpss_vos_sem_v(g_stCpssDbgLogManage.ulSemForLogLost);
        
        return (CPSS_ERROR);
    }
    
 
    return (ulInfoLen);
}


/*******************************************************************************
* 函数名称: cpss_extern_log
* 功    能: cpss_log的外部接口
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出       描述
* ucSubSystemID       UINT8       输入            子系统号
* ucModuleID          UINT8       输入            模块号
* ucLogLevel          UINT8       输入            日志信息级别
* szLogInfo           STRING      输入            格式字符串，与printf相同
* 函数返回: 本次日志记录的字符个数,否则返回CPSS_ERROR。
* 说    明: 2006/08/28李军增加
*******************************************************************************/    


INT32 cpss_extern_log
(
 UINT8 ucSubSystemID,
 UINT8 ucModuleID,
 UINT8 ucLogLevel,
 STRING szLogInfo
 )
{
   return (cpss_log(ucSubSystemID,ucModuleID,ucLogLevel,szLogInfo));

}


/*******************************************************************************
* 函数名称: cpss_log_data_add
* 功    能: 将日志数据加入缓冲  
* 函数类型: 
* 参    数: 
* 参数名称            类型        输入/输出         描述
* pucBuf                         UINT8 *     输入              缓冲区指针
* 函数返回: 成功执行返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 
*******************************************************************************/
INT32 cpss_log_data_add
(
 UINT8 *pucBuf
 )
{
    CPSS_LOG_HEAD_T stLogHead;
    UINT32 ulLogLen = 0;
    

    /* 判断到来的数据是否合法 */
    if (pucBuf == NULL)
    {
        return (CPSS_ERROR);
    }

    ((CPSS_LOG_ENTRY_T *)pucBuf)->ulInfoLen  = cpss_ntohl(((CPSS_LOG_ENTRY_T *)pucBuf)->ulInfoLen );
    ((CPSS_LOG_ENTRY_T *)pucBuf)->ulTick = cpss_ntohl(((CPSS_LOG_ENTRY_T *)pucBuf)->ulTick );
   	
    if (((CPSS_LOG_ENTRY_T *)pucBuf)->ulInfoLen > CPSS_MAX_LOG_LEN)
    {
        return (CPSS_ERROR);
    }

    
    /* 数据加入到缓存 */
    /* 判断缓存空间是否存在 */
    if (NULL == g_stCpssDbgLogManage.pucLogBufHead)
    {
        return (CPSS_ERROR);
    }
    /* 增加流水号计数 */
    ((CPSS_LOG_ENTRY_T *)pucBuf)->ulSerialID = g_stCpssDbgLogManage.ulLogID;
    g_stCpssDbgLogManage.ulLogID = g_stCpssDbgLogManage.ulLogID + 1;
    cpss_clock_get(&(((CPSS_LOG_ENTRY_T *)pucBuf)->stTime));/* 填充日志源绝对时间 */    
    ulLogLen = ((CPSS_LOG_ENTRY_T *)pucBuf)->ulInfoLen;    
    if (g_stCpssDbgLogManage.pucLogDataHead == NULL)
    {
        g_stCpssDbgLogManage.pucLogDataHead = g_stCpssDbgLogManage.pucLogDataTail;    /* 定位缓存头指针 */
        /* 加入第一条数据 */
        stLogHead.ulSeqID = 0;        /* 固定为0 */
        cpss_com_phy_addr_get(&stLogHead.stPhyAddr);
        stLogHead.ucBoardType = SWP_FUNBRD_TYPE;    /* 待定 */
        stLogHead.usInfoNum = 1;        /* 信息条计数为1 */        
        /* 加入包头 */
        cpss_mem_memcpy(
            g_stCpssDbgLogManage.pucLogDataTail,
            &stLogHead,
            sizeof(CPSS_LOG_HEAD_T));
        /* 加入每条日志数据头及消息内容 */
        cpss_mem_memcpy(
            g_stCpssDbgLogManage.pucLogDataTail + sizeof(CPSS_LOG_HEAD_T),
            pucBuf,
            sizeof(CPSS_LOG_ENTRY_T) + ulLogLen);    
        /* 移动数据写入指针的位置 */
        g_stCpssDbgLogManage.pucLogDataAdd = g_stCpssDbgLogManage.pucLogDataTail 
            + sizeof(CPSS_LOG_HEAD_T) 
            + sizeof(CPSS_LOG_ENTRY_T) 
            + ulLogLen;
    }
    else
    {
        /* 判断Tail处是否有足够的空间容纳新数据 */
        if (g_stCpssDbgLogManage.pucLogDataAdd + sizeof(CPSS_LOG_ENTRY_T) + ulLogLen 
            <= g_stCpssDbgLogManage.pucLogDataTail + CPSS_LOG_PACKET_LEN)
        {
            ((CPSS_LOG_HEAD_T *)g_stCpssDbgLogManage.pucLogDataTail)->usInfoNum = ((CPSS_LOG_HEAD_T *)g_stCpssDbgLogManage.pucLogDataTail)->usInfoNum + 1;
            /* 直接在g_pucLogDataAdd处加入数据 */
            cpss_mem_memcpy(
                g_stCpssDbgLogManage.pucLogDataAdd,
                pucBuf,
                sizeof(CPSS_LOG_ENTRY_T) + ulLogLen);    
            /* 移动数据写入指针的位置 */
            g_stCpssDbgLogManage.pucLogDataAdd += sizeof(CPSS_LOG_ENTRY_T) + ulLogLen;
        }
        else
        {
            /* 链尾节点后移 */
            g_stCpssDbgLogManage.pucLogDataTail += CPSS_LOG_PACKET_LEN;
            /* 链尾是否越界 */
            if (g_stCpssDbgLogManage.pucLogDataTail + CPSS_LOG_PACKET_LEN 
                > g_stCpssDbgLogManage.pucLogBufHead + CPSS_LOG_BUF_LEN)
            {
                g_stCpssDbgLogManage.pucLogDataTail = g_stCpssDbgLogManage.pucLogBufHead;/* 越界后返回缓存头处 */
            }
            /* 加入第一条数据 */
            stLogHead.ulSeqID = 0;        /* 固定为0 */
            cpss_com_phy_addr_get(&stLogHead.stPhyAddr);
            stLogHead.ucBoardType = SWP_FUNBRD_TYPE;    /* 待定 */
            stLogHead.usInfoNum = 1;      /* 信息条计数为1 */            
            /* 加入包头 */
            cpss_mem_memcpy(
                g_stCpssDbgLogManage.pucLogDataTail,
                &stLogHead,
                sizeof(CPSS_LOG_HEAD_T));
            /* 加入每条日志数据头及消息内容 */
            cpss_mem_memcpy(
                g_stCpssDbgLogManage.pucLogDataTail + sizeof(CPSS_LOG_HEAD_T),
                pucBuf,
                sizeof(CPSS_LOG_ENTRY_T) + ulLogLen);    
            /* 移动数据写入指针的位置 */
            g_stCpssDbgLogManage.pucLogDataAdd = g_stCpssDbgLogManage.pucLogDataTail 
                + sizeof(CPSS_LOG_HEAD_T) 
                + sizeof(CPSS_LOG_ENTRY_T) 
                + ulLogLen;
        }
    }
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_log_data_hton
* 功    能: 将整包日志数据由主机字节序转换为网络字节序
* 函数类型: 
* 参    数: 无。
* 参数名称            类型        输入/输出         描述
* pucBuf             UINT8 *      既是输入又是输出   存放转换为字节序的内容
* 函数返回: 成功转换数据返回该包数据的长度,否则返回0。
* 说    明: 
*******************************************************************************/
UINT32 cpss_log_data_hton(UINT8 *pucBuf)
{
    CPSS_LOG_HEAD_T *pstLogHead;
    CPSS_LOG_ENTRY_T *pstLogEntry;
    UINT16 usCount = 0;
    UINT32 ulLogLen = 0;

    if (pucBuf != NULL)
    {
        pstLogHead = (CPSS_LOG_HEAD_T *)pucBuf;
        usCount = pstLogHead->usInfoNum;        
        ulLogLen = sizeof(CPSS_LOG_HEAD_T);
        pstLogHead->ulSeqID = cpss_htonl(pstLogHead->ulSeqID);       
        pstLogHead->usInfoNum = cpss_htons(pstLogHead->usInfoNum);
        while (usCount > 0)
        {
            pstLogEntry = (CPSS_LOG_ENTRY_T *)(pucBuf + ulLogLen);                
            ulLogLen += pstLogEntry->ulInfoLen + sizeof(CPSS_LOG_ENTRY_T);
            if (ulLogLen > CPSS_LOG_PACKET_LEN)   /* 此时数据包出现异常 */
            {
                ulLogLen = 0;
                break;
            }
            /* 由主机字节序转换为网络字节序 */
            pstLogEntry->ulSerialID = cpss_htonl(pstLogEntry->ulSerialID);
            pstLogEntry->ulTick = cpss_htonl(pstLogEntry->ulTick);
            pstLogEntry->stTime.usYear = cpss_htons(pstLogEntry->stTime.usYear);
            pstLogEntry->ulInfoLen = cpss_htonl(pstLogEntry->ulInfoLen);
            usCount = usCount - 1;
        }
    }
    return (ulLogLen);
}
/*******************************************************************************
* 函数名称: cpss_log_data_ntoh
* 功    能: 将整包日志数据由网络字节序转换为主机字节序
* 函数类型: 
* 参    数: 无。
* 参数名称            类型        输入/输出         描述
* pucBuf                      UINT8 *           输入和输出   存放主机字节序
* 函数返回:  成功转换数据返回该包数据的长度,否则返回0。
* 说    明: 
*******************************************************************************/
UINT32 cpss_log_data_ntoh(UINT8 *pucBuf)
{
    CPSS_LOG_HEAD_T *pstLogHead;
    CPSS_LOG_ENTRY_T *pstLogEntry;
    UINT16 usCount = 0;
    UINT32 ulLogLen = 0;
    
    if (pucBuf != NULL)
    {
        pstLogHead = (CPSS_LOG_HEAD_T *)pucBuf;
        ulLogLen = sizeof(CPSS_LOG_HEAD_T);
        pstLogHead->ulSeqID = cpss_ntohl(pstLogHead->ulSeqID);        
        pstLogHead->usInfoNum = cpss_ntohs(pstLogHead->usInfoNum);
        usCount = pstLogHead->usInfoNum;   
        while (usCount > 0)
        {            
            pstLogEntry = (CPSS_LOG_ENTRY_T *)(pucBuf + ulLogLen); 
            /* 由网络字节序转换为主机字节序 */
            pstLogEntry->ulSerialID = cpss_ntohl(pstLogEntry->ulSerialID);
            pstLogEntry->ulTick = cpss_ntohl(pstLogEntry->ulTick);
            pstLogEntry->stTime.usYear = cpss_ntohs(pstLogEntry->stTime.usYear);
            pstLogEntry->ulInfoLen = cpss_ntohl(pstLogEntry->ulInfoLen);
            ulLogLen += pstLogEntry->ulInfoLen + sizeof(CPSS_LOG_ENTRY_T);  
            if (ulLogLen > CPSS_LOG_PACKET_LEN)   /* 此时数据包出现异常 */
            {
                ulLogLen = 0;
                break;
            }
            usCount = usCount - 1;
        }
    }
    return (ulLogLen);
}



/*******************************************************************************
* 函数名称: cpss_dbg_log_record
* 功    能: 记录异常日志
* 函数类型: 
* 参    数: 无。
* 参数名称            类型                                       输入/输出         描述
*      pstMsgHead           const CPSS_COM_MSG_HEAD_T *    输入                  日志消息      
* 函数返回: 无。
* 说    明: 
*******************************************************************************/


VOID cpss_dbg_log_record(const CPSS_COM_MSG_HEAD_T *pstMsgHead)
{
   if(pstMsgHead!=NULL)
      {
         g_tCpssLogRecord = pstMsgHead->stSrcProc;
      }
}

/*******************************************************************************
* 函数名称: cpss_dbg_log_record_show
* 功    能: 打印发送异常日志的纤程信息
* 函数类型: 
* 参    数: 无。
* 参数名称            类型                                       输入/输出         描述
*        
* 函数返回: 无。
* 说    明: 
*******************************************************************************/

void cpss_dbg_log_record_show()
{
   cps__oams_shcmd_printf(" Module %d\n\r", g_tCpssLogRecord.stLogicAddr.ucModule);
   cps__oams_shcmd_printf(" SubGroup %d\n\r", g_tCpssLogRecord.stLogicAddr.ucSubGroup);
   cps__oams_shcmd_printf(" Group 0x%x\n\r", g_tCpssLogRecord.stLogicAddr.usGroup); 
   cps__oams_shcmd_printf(" AddrFlag%d\n\r",     g_tCpssLogRecord.ulAddrFlag);
   cps__oams_shcmd_printf(" Pno%d\n\r",     g_tCpssLogRecord.ulPd); 
   
}

/*******************************************************************************
* 函数名称: cpss_log_show_save
* 功    能: 打印是否出现异常日志
* 函数类型: 
* 参    数: 无。
* 参数名称            类型                                       输入/输出         描述
*        
* 函数返回: 无。
* 说    明: 
*******************************************************************************/
VOID cpss_log_show_save(VOID)
{
    cps__oams_shcmd_printf(" ulLen = %d ulInfoLen = %d\n\r",g_tCpssSaveLog2.ulLen,g_tCpssSaveLog2.ulInfoLen);

}

/*******************************************************************************
* 函数名称: cpss_logm_runlog_debug
* 功    能: 检查是否出现异常日志
* 函数类型: 
* 参    数: 无。
* 参数名称            类型                                       输入/输出         描述
*  ptCpssSave              CPSS_SAVE_LOG_T *                    输出?                    存储异常日志的信息
* pucBuf                      UINT8*                                         输入                       异常日志消息体
* ulLen                        UINT32                                        输入                        异常日志消息体长度
* 函数返回: 日志异常返回cpss_error;异常正常返回cpss_ok;
* 说    明: 
*******************************************************************************/

INT32 cpss_logm_runlog_debug(CPSS_SAVE_LOG_T *ptCpssSave,UINT8* pucBuf, UINT32 ulLen)
{
    CPSS_LOG_HEAD_T*    pstLogHead = NULL;
    CPSS_LOG_ENTRY_T*   pstLogEntry = NULL;
    
    UINT16  usLogPacketNum = 0;
    UINT32  ulOffset = sizeof(CPSS_LOG_HEAD_T);
    UINT32  ulLoop = 0;
    UINT32  ulLogTotalLen = 0;
    UINT32  ulSingleLogLen = 0;

    pstLogHead = (CPSS_LOG_HEAD_T*)pucBuf;
    usLogPacketNum = pstLogHead->usInfoNum;

    usLogPacketNum = cpss_ntohs(usLogPacketNum);
	
   /* CPS__OAMS_CHGBS_UINT16(usLogPacketNum);*/
    
    ulLogTotalLen = sizeof(CPSS_LOG_HEAD_T);
    
    for (ulLoop = 0; ulLoop < usLogPacketNum; ulLoop++)
    {
        pstLogEntry = (CPSS_LOG_ENTRY_T*)(&pucBuf[ulOffset]);
        ulLogTotalLen += sizeof(CPSS_LOG_ENTRY_T);
        ulOffset += sizeof(CPSS_LOG_ENTRY_T);
        
        ulSingleLogLen = pstLogEntry->ulInfoLen;
       /* CPS__OAMS_CHGBS_UINT32(ulSingleLogLen);*/
	  ulSingleLogLen = cpss_ntohl(ulSingleLogLen);
        ulLogTotalLen += ulSingleLogLen;
        ulOffset += ulSingleLogLen;
    }
      
    if (ulLen != ulLogTotalLen)
    {
        UINT16 usMaxLen = (ulLen > ulLogTotalLen)? ulLen:ulLogTotalLen;
	  
        printf("Cpss Logm ERR, len error buflen = %d  CodeLen = %d\n", ulLen, ulLogTotalLen);
		
        ptCpssSave->ulLen  = ulLen;
	  ptCpssSave->ulInfoLen = ulLogTotalLen;
	 cpss_mem_memcpy(ptCpssSave->acCpssLogInfo, pucBuf,usMaxLen);
	 
	 return CPSS_ERROR;	
      /*  cpss_vos_task_suspend(cpss_vos_task_desc_self());*/
        
    }
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_log_data_send
* 功    能: 发送日志数据
* 函数类型: 
* 参    数: 无。
* 参数名称            类型                                       输入/输出         描述
*                       
* 函数返回: 成功:CPSS_OK;失败:CPSS_ERROR
* 说    明: 
*******************************************************************************/

INT32 cpss_log_data_send()
{
    UINT32 ulLogLen = 0;
    INT32 lComSendMate = 0;
    INT32 lComSend = 0;
    
    /* 判断缓存是否为空 */
    if (g_stCpssDbgLogManage.pucLogDataHead != NULL)
    {
        ulLogLen = cpss_log_data_hton(g_stCpssDbgLogManage.pucLogDataHead);
        if (ulLogLen > 0) /* 字节序转换成功，开始发送数据；如转换失败，自动丢弃该包数据 */
        {
            /* 发送数据，如发送失败 */
            if (g_stCpssDbgLogManage.bLogSendtoMate)
            {
                lComSendMate = cpss_com_send_mate(g_stCpssDbgLogManage.stLogDstPid.ulPd,
                                           CPSS_LOG_MSG,
                                           g_stCpssDbgLogManage.pucLogDataHead,
                                           ulLogLen);
	
                cpss_logm_runlog_debug(  &g_tCpssSaveLog1, g_stCpssDbgLogManage.pucLogDataHead,ulLogLen);
				
                   			
                if (CPSS_OK != lComSendMate)
                {                    
                    ulLogLen = cpss_log_data_ntoh(g_stCpssDbgLogManage.pucLogDataHead);
                    /* 发送失败，如成功转换回主机字节序，则返回失败，以便下一次发送，若转换失败，则丢弃该包数据 */
                    if (ulLogLen > 0)   
                    {
                        return (CPSS_ERROR);
                    }                                              
                }
            }
            else
            {
                lComSend = cpss_com_send(&(g_stCpssDbgLogManage.stLogDstPid),
                                      CPSS_LOG_MSG,
                                      g_stCpssDbgLogManage.pucLogDataHead,
                                      ulLogLen);
		
                  cpss_logm_runlog_debug(&g_tCpssSaveLog2,   g_stCpssDbgLogManage.pucLogDataHead,ulLogLen);	
                if (CPSS_OK != lComSend)
                {                    
                    ulLogLen = cpss_log_data_ntoh(g_stCpssDbgLogManage.pucLogDataHead);
                    /* 发送失败，如成功转换回主机字节序，则返回失败，以便下一次发送，若转换失败，则丢弃该包数据 */
                    if (ulLogLen > 0)
                    {
                        return (CPSS_ERROR);
                    }  
                }
            }        
        }   
        
        if (g_stCpssDbgLogManage.pucLogDataHead == g_stCpssDbgLogManage.pucLogDataTail)/* 缓存中只有一包数据 */
        {
            g_stCpssDbgLogManage.pucLogDataHead = NULL;/* 发完后缓存变空 */
        }
        else/* 缓存中的数据多于一包 */
        {
            /* 链表头后移 */     
            g_stCpssDbgLogManage.pucLogDataHead += CPSS_LOG_PACKET_LEN;
            if (g_stCpssDbgLogManage.pucLogDataHead + CPSS_LOG_PACKET_LEN 
                > g_stCpssDbgLogManage.pucLogBufHead + CPSS_LOG_BUF_LEN)
            {
                g_stCpssDbgLogManage.pucLogDataHead = g_stCpssDbgLogManage.pucLogBufHead;
            }
        }
    }
    return (CPSS_OK);    
}

/*******************************************************************************
* 函数名称: cpss_log_rate_set
* 功    能: 日志上报速率设置
* 函数类型: 
* 参    数: 
* 参数名称            类型        输入/输出         描述
* ulRate            UINT32        输入            日志上报速率（单位：条/秒）
* 函数返回: 成功执行返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 日志初始化时，系统会设置默认的上报速率（50条/秒）。
*******************************************************************************/
INT32 cpss_log_rate_set(UINT32 ulRate)
{
    /* 计算相应的定时器周期和令牌 */
    /* 分两种情况计算，即ulRate大于10和小于10两种情况 */
    if ((ulRate < 11) && (ulRate > 0))
    {
        g_stCpssDbgLogManage.ulLogTimerPeriod = 1000;
        g_stCpssDbgLogManage.ulLogToken = ulRate;
    }
    else if ((ulRate <= 256) && (ulRate > 10))
    {
        g_stCpssDbgLogManage.ulLogTimerPeriod = 100;
        /* 四舍五入计算每100ms令牌数 */
        if (CPSS_MOD(ulRate,10) > 4)
        {
            g_stCpssDbgLogManage.ulLogToken = CPSS_DIV(ulRate,10) + 1;
        }
        else
        {
            g_stCpssDbgLogManage.ulLogToken = CPSS_DIV(ulRate,10);
        }
    }
    else if (ulRate == 0)
    {
        g_stCpssDbgLogManage.ulLogTimerPeriod = 0;
        g_stCpssDbgLogManage.ulLogToken = 0;
    }
    /* ulRate超出范围 */
    else
    {
        return (CPSS_ERR_DBG_PRT_PARAM_INVA);
    }
    
    /* 准备发送数据 */
    return cpss_com_send_phy(g_stCpssDbgLogPhyAddr,
        CPSS_DBG_MNGR_PROC,
        CPSS_LOGTIMER_CREATE_MSG,
        NULL,
        0);
}

/*******************************************************************************
* 函数名称: cpss_log_level_set
* 功    能: 日志级别设置
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出         描述
* ucLogLevel          UINT8       输入              日志级别
* 函数返回: 成功设置返回CPSS_OK,否则返回CPSS_ERROR。
* 说    明: 设置日志上报的级别，只允许该级别的日志上报。
*******************************************************************************/
INT32 cpss_log_level_set
(
 UINT8 ucLogLevel
 )
{
    /* 判断日志级别参数是否合法 */
    if (ucLogLevel > CPSS_LOG_DETAIL)
    {
        return (CPSS_ERR_DBG_PRT_PARAM_INVA);
    }
    
    /* 设置单板日志级别 */
    g_stCpssDbgLogManage.ucLogLevel = ucLogLevel;
    
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_log_lost_info_get
* 功    能: 实现采集丢失的日志信息数    
* 函数类型: INT32
* 参    数: 
* 参数名称            类型        输入/输出       描述
* pulLogLostInfo      UINT32 *    输入            丢失的日志信息计数 
* 函数返回: 执行后返回CPSS_OK。
* 说    明: 
*******************************************************************************/
INT32 cpss_log_lost_info_get
(
 UINT32 *pulLogLostInfo
 )
{
    *pulLogLostInfo = g_stCpssDbgLogManage.ulLogInfoLostNum;
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_log_redirect_set    
* 功    能: 日志重定向设置 
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出     描述
* pstPid          CPSS_COM_PID_T *    输入          日志上报对应的PID，
* 函数返回: 成功设置后返回CPSS_OK，否则返回CPSS_ERROR.
* 说    明:
*******************************************************************************/
INT32 cpss_log_redirect_set(VOID)
{
    CPSS_COM_PID_T stLogPid;   /* 本板逻辑地址 */
    BOOL bFuncType;
//    bFuncType = cpss_local_funcbrd_type_is_mc();
		bFuncType =FALSE;
    cpss_com_logic_addr_get(&stLogPid.stLogicAddr, &stLogPid.ulAddrFlag);
    if(TRUE == bFuncType)
    {
        if (CPSS_COM_ADDRFLAG_MASTER != stLogPid.ulAddrFlag)
        {
            g_stCpssDbgLogManage.bLogSendtoMate = TRUE;   /* 需要发送到伙伴板 */
        }
        else
        {                
            g_stCpssDbgLogManage.bLogSendtoMate = FALSE;  /* 不需要发送到伙伴板 */
        }
    }
    else
    {
        g_stCpssDbgLogManage.bLogSendtoMate = FALSE;  /* 不需要发送到伙伴板 */
    }
    
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_vos_g_stCpssDbgLogManage_get
* 功    能: 获得g_stCpssDbgLogManage、地址
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出     描述
* 
* 函数返回: g_stCpssDbgLogManage、地址
* 说    明:
*******************************************************************************/
INT32 cpss_vos_g_stCpssDbgLogManage_get()
{
    return (INT32)(&g_stCpssDbgLogManage);
}

/*******************************************************************************
* 函数名称: cpss_vos_g_bCpssDbgLogValid_get
* 功    能: 获得g_bCpssDbgLogValid、地址
* 函数类型: 
* 参    数: 
* 参数名称        类型                输入/输出     描述
* 
* 函数返回: g_bCpssDbgLogValid、地址
* 说    明:
*******************************************************************************/

INT32 cpss_vos_g_bCpssDbgLogValid_get()
{
    return (INT32)(&g_bCpssDbgLogValid);
}
#else
 /**********************下面实现是为了避免dsp编译告警****************************/
VOID cpss_log_show_save(VOID)
{
    return;
}
INT32 cpss_extern_log
(
 UINT8 ucSubSystemID,
 UINT8 ucModuleID,
 UINT8 ucLogLevel,
 STRING szLogInfo
 )
 {
     return CPSS_OK;
 }
 INT32 cpss_log_rate_set(UINT32 ulRate)
 {
     return CPSS_OK;
 }
INT32 cpss_log_level_set
(
 UINT8 ucLogLevel
 )
 {
     return CPSS_OK;
 } 
INT32 cpss_log_data_send()
{
    return CPSS_OK;
}
INT32 cpss_log_data_add
(
 UINT8 *pucBuf
)
{
    return CPSS_OK;
}
INT32 cpss_log_init()
{
    return CPSS_OK;
}
INT32 cpss_log_redirect_set(VOID)
{
    return CPSS_OK;
}
#endif
/******************************* 源文件结束 ***********************************/
