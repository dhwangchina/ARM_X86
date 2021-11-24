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
#ifndef CPSS_FS_FTP_H
#define CPSS_FS_FTP_H
/********************************* 头文件包含 ********************************/
#include "cpss_type.h"

/******************************** 宏和常量定义 *******************************/

#define CPSS_FTP_RECV_SIZE      (0x100)
#define CPSS_TFTP_RECV_SIZE     (0x300)

#define CPSS_TFTP_WRITE_SIZE    (0x1000)

#define CPSS_FTP_TYPE_FTP       (0x0)
#define CPSS_FTP_TYPE_TFTP      (0x1)

#define CPSS_FTP_LOW_FLAG  0
#define CPSS_FTP_HIGH_FLAG 1

#define CPSS_FTP_QUERY_MSG  0x100000

#ifdef CPSS_VOS_WINDOWS
#define CPSS_FTP_GET_STR        "get"
#define CPSS_FTP_PUT_STR        "put"
#elif defined CPSS_VOS_VXWORKS
#define CPSS_FTP_GET_STR        "RETR %s"
#define CPSS_FTP_PUT_STR        "STOR %s"
#define CPSS_TFTP_GET_STR       "get"
#define CPSS_TFTP_PUT_STR       "put"
#define CPSS_TFTP_CMD_LEN       (0x4)
#endif

#define CPSS_FTP_TYPE_STR       "TYPE I"
#define CPSS_FTP_CWD_STR        "CWD"
#define CPSS_FTP_GET_CPS__TNBS_STR   "RETR"
#define CPSS_FTP_PUT_CPS__TNBS_STR   "STOR"
#define CPSS_FTP_RNFR_STR       "RNFR"
#define CPSS_FTP_RNTO_STR       "RNTO"
#define CPSS_FTP_QUIT_STR       "QUIT"
#define CPSS_FTP_DELE_STR       "DELE"

#define CPSS_FTP_CMD_LEN        (0x8)

#ifdef CPSS_VOS_WINDOWS
#define CPSS_FTP_AGENT_NAME      "cpss_ftp_agent"
#define CPSS_FTP_PROXY_NAME      "cpss_ftp_proxy"
#endif

#define CPSS_FTP_PROGRESS_SEND_FLAG      1
#define CPSS_FTP_PROGRESS_NOT_SEND_FLAG  0

#define CPSS_FTP_FILE_LIST_LEN_MAX    4096

#define CPS__TNBS_FTP_CMDREPLY_LEN     256
#define CPS__TNBS_FTP_IPADDR_LEN       16
#define CPS__TNBS_FTP_USERNAME_LEN     20
#define CPS__TNBS_FTP_PASSWORD_LEN     20
#define CPS__TNBS_FTP_ACCOUNT_LEN      20
#define CPS__TNBS_FTP_COMMAND_LEN      256
#define CPS__TNBS_FTP_FILE_LEN         256

#define CPSS_FTP_COMMAND_CWD      1
#define CPSS_FTP_INIT_PHASE       0
#define CPSS_FTP_XFER_PHASE       1
#define CPSS_FTP_CWD_PHASE        2
#define CPSS_FTP_TRANSFER_PHASE   3
#define CPSS_FTP_DELE_PHASE       4
#define CPSS_FTP_RNFR_PHASE       5
#define CPSS_FTP_RNTO_PHASE       6

#define CPSS_TFTP_MAX_REQ_NUM 8000

#define CPSS_TFTP_TRANS_REQ_MSG  0x100001
#define CPSS_TFTP_TRANS_DATA_MSG 0x100002
#define CPSS_TFTP_TRANS_ACK_MSG  0x100003
#define CPSS_TFTP_TRANS_DISCONNECT_MSG  0x100004

#define CPSS_TFTP_CLIENT_FLAG 0
#define CPSS_TFTP_SERVER_FLAG 1

#define CPSS_TFTP_DATA_BEGIN  0
#define CPSS_TFTP_DATA_MID    1
#define CPSS_TFTP_DATA_END    2

#define CPSS_TFTP_WRITE_MEDIUM_DISK    0
#define CPSS_TFTP_WRITE_MEDIUM_MEM     1

#define CPSS_TFTP_TIMEOUT_COUNT_MAX    20
#define CPSS_TFTP_RESEND_TIMEOUT_COUNT 2

#define CPSS_TFTP_RECV_SIDE  0
#define CPSS_TFTP_SEND_SIDE  1

#define CPSS_TFTP_TIMEOUT_TM_MSG      CPSS_TIMER_00_MSG
/* 链路心跳定时器号 */
#define CPSS_TFTP_TIMEOUT_TM_NO       (CPSS_TFTP_TIMEOUT_TM_MSG&0xFF)

#ifdef CPSS_FUNBRD_MC
  #define CPSS_TFTP_TIMEOUT_INTERVAL      12000
#else
  #define CPSS_TFTP_TIMEOUT_INTERVAL    2000
#endif

typedef struct {
    UINT32 ulCmdId;  /*命令索引*/    
    UINT8 ucHostIp[CPS__TNBS_FTP_IPADDR_LEN] ;       /*host ip addr */
    UINT8 ucUser[CPS__TNBS_FTP_USERNAME_LEN] ;       /* user name for host login */
    UINT8 ucPasswd[CPS__TNBS_FTP_PASSWORD_LEN] ;     /* password for host login */
    UINT8 ucAcct[CPS__TNBS_FTP_ACCOUNT_LEN] ;        /* account for host login */
    UINT8 ucCmd[CPS__TNBS_FTP_COMMAND_LEN] ;         /* command to send to host */
    INT32 (*pfStatusHook) (UINT32 ulTotal,UINT32 ulCurent,UINT32 ulVal);
    UINT32 ulVal; /*CPSS输入，TNBS不解释，直接传入hook中的ulVal参数*/
}CPS__TNBS_FTP_XFER_REQ_MSG_T;

typedef struct {
    UINT32  ulCmdId;  /*命令索引*/        
    INT32   lResult;      /* 命令响应码*/
    UINT32  ulCtrlSock;   /* where to return control socket fd */
    UINT32  ulDataSock;   /* where to return data socket fd, */
    /* (NULL == don't open data connection) */
    UINT32  ulReplyStringLength ; /*响应消息长度 */ 
    UINT8   ucReplyString[CPS__TNBS_FTP_CMDREPLY_LEN] ;      /* 服务器端响应应消息字符串*/
}CPS__TNBS_FTP_XFER_RSP_MSG_T;

typedef struct {
    UINT32   ulSocketId;  /* FTP控制SOCKET标识 */
    UINT32   ulCmdId;  /*命令索引*/
    UINT32   ulLen; /*数据长度*/
    UINT8    ucpFmt[CPS__TNBS_FTP_COMMAND_LEN] ;      /* format string of command to send */
    UINT32   ulArg1 ;                            /* first of six args to format string */
    UINT32   ulArg2 ;     
    UINT32   ulArg3 ;     
    UINT32   ulArg4 ;     
    UINT32   ulArg5 ;     
    UINT32   ulArg6 ;
    UINT8    ucClientFilePath[CPS__TNBS_FTP_FILE_LEN] ;
}CPS__TNBS_FTP_COMMAND_REQ_MSG_T;

typedef struct {
    INT32   lResult;  /* 命令响应码*/
    UINT32  ulCmdId;  /*命令索引 */
    UINT8   ucReplyString[CPS__TNBS_FTP_CMDREPLY_LEN] ;     /* 服务器端响应消息字符串*/
    UINT32  ulReplyStringLength ; /*响应消息长度 */
}CPS__TNBS_FTP_COMMAND_RSP_MSG_T;

/*FTP请求队列的节点*/
typedef struct tagCPSS_FTP_INFO_NODE_T
{
    UINT32 ulSerialId ;
    UINT32 ulUsrSeq ;
    UINT32 ulFtpCmd ;
    UINT32 ulCtrlSock ;
    UINT32 ulProgessStep ;
    UINT32 ulLen ;
    UINT32 ulPhase ;
    UINT32 ulServerIp ;

    UINT32 ulProStat ;
    UINT32 ulBeginTick ;
    UINT32 ulDataDealNum ;
    UINT32 ulStepVal ;

    UINT8 *pucMem ;
    UINT8 aucServerFilePathName[CPSS_FS_FILE_ABSPATH_LEN] ;
    UINT8 aucClientFilePathName[CPSS_FS_FILE_ABSPATH_LEN] ; 
    UINT8 aucPasswd[CPS__TNBS_FTP_PASSWORD_LEN] ;
    UINT8 aucUsrName[CPS__TNBS_FTP_USERNAME_LEN] ;
    CPSS_COM_PID_T stDstPid ;
    VOID* pvNext ;
}CPSS_FTP_INFO_NODE_T ;

/*FTP请求队列的管理结构*/
typedef struct tagCPSS_FTP_INFO_MAN_T
{
    CPSS_FTP_INFO_NODE_T* pstFtpHdr ;
    UINT8  *ucDataBuf ;
    UINT32 ulFtpInfoNum ;
    UINT32 ulFtpSerialId ;
    UINT32 ulFtpMutex ;
}CPSS_FTP_INFO_MAN_T ;


/******************************** 类型定义 ***********************************/

/*文件传输连接操作的结构(FTP,TFTP)*/
#ifdef CPSS_VOS_VXWORKS
typedef struct tagCPSS_FTP_CONNECT_T
{
    UINT32 ulCmd ;               /*FTP服务类型*/
    UINT32 ulLen ;               /*用户传入的内存大小*/
    UINT32 ulFileHandle ;        /*文件句柄*/
    INT32  lReadHandle ;         /*读句柄*/
    INT32  lWriteHandle ;        /*写句柄*/
    INT32  lDataSock ;           /*FTP的数据句柄*/
    INT32  lCtrlSock ;           /*FTP的控制句柄*/
    STRING szClientFileName ;    /*客户文件名称，包含绝对路径*/
    STRING szServerFileName ;    /*服务器文件名称，包含绝对路径*/
    STRING szServerIP ;          /*服务器IP地址*/
    STRING szUserName ;          /*FTP用户名称*/
    STRING szPasswd ;            /*FTP用户密码*/
    STRING szMemBuf ;            /*FTP用户传入的内存指针*/
}CPSS_FTP_CONNECT_T ;

#elif defined CPSS_VOS_WINDOWS
typedef struct tagCPSS_FTP_CONNECT_T
{
    UINT32 ulCmd ;                  /*FTP服务类型*/
    UINT32 ulLen ;                  /*用户传入的内存大小*/

    HINTERNET pvInternetSession ;   /*internet句柄*/
    HINTERNET pvConnectSession ;    /*connection句柄*/
    HINTERNET pvFTPSession ;        /*ftp句柄*/
    
    STRING szServerFileName ;       /*服务器文件名称，包含绝对路径*/
    STRING szServerIP ;             /*服务器IP地址*/

    STRING szUserName ;             /*FTP用户名称*/
    STRING szPasswd ;               /*FTP用户密码*/

    STRING szMemBuf ;               /*TFTP用户传入的内存指针*/
}CPSS_FTP_CONNECT_T ;
#endif

typedef struct tagCPSS_FTP_PROGRESS_INFO_T
{
    UINT32 ulSendFlag ;
    UINT32 ulPid ;
    UINT32 ulStep ;
    UINT32 ulSerialID ;
    UINT32 ulTotalSize ;
    UINT32 ulState ;
}CPSS_FTP_PROGRESS_INFO_T ;

typedef struct tagCPSS_TFTP_TRANS_NODE_T
{
    UINT32 ulTftpTransId ;
    UINT32 ulClientFlag ;
    UINT32 ulTimerId ;
    UINT32 ulFd ;
    UINT32 ulSendNo ;
    UINT32 ulSendNoOld ;
    UINT32 ulAckNo  ;
    UINT32 ulAckNoOld ;
    UINT32 ulSendDataLen ;
    UINT32 ulRecvDataLen ;
    UINT32 ulReadLen ;
    UINT32 ulDataEndFlag ;
    UINT32 ulSerialID ;
    UINT32 ulWriteMedium ;
    UINT32 ulWriteBuf ;
    UINT32 ulWriteBufLen ;
    UINT32 ulTransSideFlag ;
    UINT32 ulTimeVisit ;
    BOOL  bRelWriteBuf;
    CPSS_COM_PHY_ADDR_T stDestPhyAddr ;
    CPSS_COM_PID_T stDestPid ;
    UINT8 aucFileName[CPSS_FS_FILE_ABSPATH_LEN] ;
    VOID* pvNext ;
}CPSS_TFTP_TRANS_NODE_T;

typedef struct tagCPSS_TFTP_TRANS_REQ_MSG
{
    UINT32 ulTftpTransId ;
    UINT8  aucServerFilePath[CPSS_FS_FILE_ABSPATH_LEN] ;
    UINT32 ulTftpCmd ;
    CPSS_COM_PHY_ADDR_T stSrcPhyAddr ;
}CPSS_TFTP_TRANS_REQ_MSG_T;

typedef struct tagCPSS_TFTP_TRANS_ACK_MSG
{
    UINT32 ulTftpTransId ;
    UINT32 ulAckNo ;
}CPSS_TFTP_TRANS_ACK_MSG_T ;

typedef struct tagCPSS_TFTP_TRANS_DATA_MSG
{
    UINT32 ulTftpTransId ;
    UINT32 ulDataNo ;
    UINT32 ulDataLen ;
    UINT32 ulEndFlag ;
    UINT8  aucSendData[CPSS_TFTP_RECV_SIZE] ;
}CPSS_TFTP_TRANS_DATA_MSG_T ;

typedef struct tagCPSS_TFTP_TRANS_DISCONNCT_MSG
{
    UINT32 ulTftpTransId ;
    INT32  lResult ;
}CPSS_TFTP_TRANS_DISCONNCT_MSG_T ;

/******************************** 全局变量声明 *******************************/



/******************************** 外部函数原形声明 ***************************/
extern INT32 cpss_file_rename
(
STRING szOldName,
STRING szNewName
);

extern INT32 cpss_tftp_dir_add
(
CHAR *pacDirName
);
 

extern INT32 cpss_ftp_init_tnbs() ;

extern INT32 cpss_ftp_process_ind_hook(UINT32 ulTotal,UINT32 ulCurent,UINT32 ulVal) ;

extern CPSS_FTP_INFO_NODE_T* cpss_ftp_info_node_queue_find(UINT32 ulSerialId, CPSS_FTP_INFO_MAN_T* pstFtpMan) ;

extern VOID cpss_ftp_command_end_deal(CPSS_FTP_INFO_NODE_T *pstFtpInfoNode,INT32 lResult) ;

extern VOID cpss_ftp_link_show() ;

/******************************** 头文件保护结尾 *****************************/

#endif /* CPSS_FS_FTP_H */
/******************************** 头文件结束 *********************************/



