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
#ifndef CPS__TNBS_PUBLIC_H_
#define CPS__TNBS_PUBLIC_H_

#include "cpss_public.h"
#include "cps_rdbs_public.h"

/*in cpss_com_drv.c*/

#define  CPS__TNBS_OK                    ((INT32)0)
#define  CPS__TNBS_ERROR                 ((INT32)-1)

/* 纤程间传输数据的最大长度*/
#define  CPS__TNBS_MAX_DATA_LEN                  (60 * 1024)

#define	CPS__TNBS_SOCK_STREAM	1		/* stream socket */

#define	CPS__TNBS_IPPROTO_TCP    6		/* tcp协议 */

#define   CPS__TNBS_IPSTACK_SOCKET_REUSE_ENABLE              1

#define  CPS__TNBS_MSG_START             (0x22000000)

#define  CPS__TNBS_SOCKET_MSG_START      (CPS__TNBS_MSG_START + 0x7000)

#define  CPS__TNBS_SOCKET_DATAIND_MSG            (CPS__TNBS_SOCKET_MSG_START + 1)

#define  CPS__TNBS_SOCKET_SETUP_REQ_MSG          (CPS__TNBS_SOCKET_MSG_START + 3)

#define  CPS__TNBS_SOCKET_SETUP_RSP_MSG          (CPS__TNBS_SOCKET_MSG_START + 4)

#define  CPS__TNBS_SOCKET_SETUP_IND_MSG          (CPS__TNBS_SOCKET_MSG_START + 5)

#define  CPS__TNBS_SOCKET_LISTEN_REQ_MSG         (CPS__TNBS_SOCKET_MSG_START + 6)

#define  CPS__TNBS_SOCKET_LISTEN_RSP_MSG         (CPS__TNBS_SOCKET_MSG_START + 7)

#define  CPS__TNBS_SOCKET_EXCP_IND_MSG           (CPS__TNBS_SOCKET_MSG_START + 10)

typedef CPS__RDBS_IPADDR_T                   CPS__TNBS_IPADDR_T;

#define  CPS__TNBS_SCTP_MAX_IP_NUM               ((UINT8) 4)

/*
* HSPS的NBAP或者TNSS的M3UA发送消息到TNBS的IP协议栈纤程.
* 请求建立SOCKET.
*/
typedef struct {
    UINT32  ulUsrId;                /*用户ID，由用户自定义，以匹配Req和Rsp消息*/

            /* Socket类型:
                CPS__TNBS_SOCK_STREAM   1
                CPS__TNBS_SOCK_DGRAM    2
                CPS__TNBS_SOCK_RAW      3
                CPS__TNBS_SOCK_RDM       4
                CPS__TNBS_SOCK_SEQPACKET 5      */
    UINT8   ucSocketType;  /*使用宏进行赋值，参考本文件宏定义*/
    UINT8   ucProtoType;            /*协议类型，标准协议类型定义，参考上面宏定义*/
            /* Socket类型: #define	CPS__TNBS_IPPROTO_TCP    6		 tcp协议
                             #define	CPS__TNBS_IPPROTO_UDP	17	     udp协议
                             #define CPS__TNBS_IPPROTO_SCTP   132	    SCTP协议     */
    UINT16  usRxStreamNum;          /* 接收流最大数目，仅当SCTP时有效*/
    UINT16  usTxStreamNum;          /* 发送流最大数目，仅当SCTP时有效*/
    UINT16  usRsv;      /*对IP地址和端口号重用时，此字段生效
                          CPS__TNBS_IPSTACK_SOCKET_REUSE_DISABLE 不重用，
                          CPS__TNBS_IPSTACK_SOCKET_REUSE_ENABLE 重用*/

    UINT32       ulMaxRecvWin;           /* 最大接收窗口，仅当SCTP时有效*/

	UINT8        ucDestIpNum;    /*目的端IP地址数目*/
	UINT8        ucSrcIpNum;     /*本端IP地址数目*/
	UINT16       usSockTimeOut;  /*  用户希望的最大等待时间，单位为10ms,
	                               取值为0时为不限制连接建立时间	*/

    /* 标准用法，对端IP地址,仅当SCTP时可能存在多个 */
    CPS__TNBS_IPADDR_T   astDestIpAddr[CPS__TNBS_SCTP_MAX_IP_NUM];

    /* 标准用法，本端IP地址，仅当SCTP时可能存在多个 */
    CPS__TNBS_IPADDR_T   astSrcIpAddr[CPS__TNBS_SCTP_MAX_IP_NUM];

    UINT16  usPeerPort;             /* 对端端口，标准用法 */
    UINT16  usLocalPort;            /* 本端端口，标准用法 */
}CPS__TNBS_SOCKET_SETUP_REQ_MSG_T;

#define  CPS__TNBS_IPADDR_NUM                    ((UINT32)16)

typedef struct {
    UINT32  ulIpAddrNum;
    CPS__TNBS_IPADDR_T    astIpAddr[CPS__TNBS_IPADDR_NUM];
} CPS__TNBS_IPADDR_ARRAY_T;

/*
* HSPS的NBAP或者TNSS的M3UA发送消息到承载的IP协议栈纤程.
* 监听特定端口上对端发起的建立SOCEKT请求.
*/
typedef struct {
    UINT32  ulUsrId;                /*用户ID，由用户自定义，以匹配Req和Rsp消息*/
    UINT8   ucSocketType;           /* SOCKET类型 参考宏定义赋值*/
    UINT8   ucProtoType;            /* 协议类型参考宏定义赋值 */
    UINT16  usLocalPort;            /* 本端端口 */

	UINT8           ucSrcIpNum; /*本端IP地址数目*/
	UINT8           aucRsv[3];
    /*标准用法，本端IP地址，仅当SCTP时存在多个 */
    CPS__TNBS_IPADDR_T   astSrcIpAddr[CPS__TNBS_SCTP_MAX_IP_NUM];

    UINT16  usRxStreamNum;          /* 接收的最大流数目，仅当SCTP时有效*/
    UINT16  usTxStreamNum;          /* 发送的最大流数目，仅当SCTP时有效*/
    UINT32  ulMaxRecvWin;           /* 最大接收窗口，仅当SCTP时有效*/
}CPS__TNBS_SOCKET_LISTEN_REQ_MSG_T;

typedef struct {
    UINT32  ulUsrId; /*来自建立请求消息的用户ID，以匹配Req和Rsp消息*/
    UINT32  ulSocketId;  /* SOCKET标识 */
	UINT32  ulResult;    /* CPS__TNBS_OK ：建立成功；CPS__TNBS_ERROR,失败 */
} CPS__TNBS_SOCKET_LISTEN_RSP_MSG_T;

/*
* TNBS的IP协议栈纤程发送消息到HSPS的NBAP或者TNSS的M3UA.
* 返回建立SOCKET的响应.
*/
typedef struct {
    UINT32  ulResult;               /* CPS__TNBS_OK ：建立成功；CPS__TNBS_ERROR,失败   */

    /* 建立成功时，返回建立的SOCKET；建立失败时，此值没有意义 */
    UINT32  ulSocketId;

    UINT32  ulUsrId;                /*来自建立请求消息的用户ID，以匹配Req和Rsp消息*/
}CPS__TNBS_SOCKET_SETUP_RSP_MSG_T;

/*
* TNBS的IP协议栈纤程发送给HSPS的NBAP或TNSS的M3UA协议处理模块.
* 通知上层用户接收到新的SOCKET数据.
*/
typedef struct {
    UINT32   ulSocketId;            /* SOCKET标识 */
    CPS__TNBS_IPADDR_T   stSrcIpAddr;    /* 标准用法，源IP地址 */
    UINT16   usSrcPort;             /* 标准用法，源port */
    UINT16   usStreamId;            /*  流ID，仅当SCTP时有效 */
    UINT32   ulPayloadId;           /*  数据协议ID，仅当SCTP时有效 */
    UINT16   usLen;                 /* 数据长度 */
    UINT16   usRsv;/*保留字段*/
    UINT8    aucData[CPS__TNBS_MAX_DATA_LEN]; /* 数据内容 */
}CPS__TNBS_SOCKET_DATAIND_MSG_T;

/*
* TNBS的IP协议栈纤程发送消息到HSPS的NBAP或者TNSS的M3UA。
* 当本端作为服务器端时，在指定端口上监听对端的连接请求，
* 接收到对端建立请求，建立连接后，向上层用户发送该指示消息.
* 返回建立SOCKET的响应.
*/
typedef struct {
    UINT32   ulUsrId;                /*来自建立请求消息的用户ID，以匹配Req和Rsp消息*/
    UINT32   ulSocketId;             /* SOCKET标识 */
    UINT8    ucPeerIpNum;            /* 对端IP数目，仅当SCTP时有效 */
	UINT8    ucLocalIpNum;         /*本端IP地址数目*/
	UINT8           aucRsv[2];
    CPS__TNBS_IPADDR_T   astDestIpAddr[CPS__TNBS_SCTP_MAX_IP_NUM];    /* 标准用法 */
    CPS__TNBS_IPADDR_T   astSrcIpAddr[CPS__TNBS_SCTP_MAX_IP_NUM];     /* 标准用法 */
    UINT16   usPeerPort;             /* 对端端口，仅当SCTP时有效 */
    UINT16   usLocalPort;            /* 本端端口，仅当SCTP时有效 */
    UINT16   usTxStreamNum;          /* 发送流数目，仅当SCTP时有效 */
    UINT16   usRxStreamNum;          /* 接收流数目，仅当SCTP时有效 */
} CPS__TNBS_SOCKET_SETUP_IND_MSG_T;

/*
* 承载的IP协议栈纤程发送消息到HSPS的NBAP或者TNSS的M3UA.
* 指示当前SOCKET异常.
*/
typedef struct {
    UINT32  ulSocketId;             /* 发生异常的SOCKET */
    UINT32  ulCause;                /* 发生异常的类型，参考宏定义
	                                0：CPS__TNBS_SOCK_CLOSE 连接断开
	                                1：CPS__TNBS_DATA_ERROR 数据异常  */
    UINT32  ulDataRetriId;          /* ulCause = CPS__TNBS_DATA_ERROR时有效，
	                          数据ID，上层可用来获取没有发送成功的数据 */
}CPS__TNBS_SOCKET_EXCP_IND_MSG_T;

/*end in cpss_com_drv.c*/

#endif /* CPS__TNBS_PUBLIC_H_ */
