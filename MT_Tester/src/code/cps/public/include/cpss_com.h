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
#ifndef CPSS_COM_H
#define CPSS_COM_H

/********************************* 头文件包含 ********************************/
#include "cpss_tm.h"
#include "cpss_config.h"
#include "cpss_fs.h"

/******************************** 宏和常量定义 *******************************/

/******************************** 字节序转化 **********************************/
/*
#define cpss_ntohs ntohs
#define cpss_ntohl ntohl
#define cpss_htons htons
#define cpss_htonl htonl
*/
#ifdef CPSS_VOS_VXWORKS                /* Win32-NT 操作系统部分 */
    #define closesocket close
#endif

#ifdef CPSS_VOS_LINUX                /* LINUX 操作系统部分 */
    #define closesocket close
#endif


#define CPSS_BIT_BIG		0
#define CPSS_BIT_LITTLE		1

#if (CPSS_CPU_TYPE == CPSS_CPU_BIG)
#define CPSS_BIT_ORDER CPSS_BIT_BIG
#elif (CPSS_CPU_TYPE == CPSS_CPU_LITTLE)
#define CPSS_BIT_ORDER CPSS_BIT_LITTLE
#endif

/******************************** 消息跟踪 **********************************/
/*#define CPSS_COM_UDP_PROC CPSS_VK_PD(2,0)*/

/******************************** 基本通信相关 ********************************/
/* 主备标志，用于CPSS_COM_PID_T */
#define CPSS_COM_ADDRFLAG_MASTER   ((UINT32)(0x00)) /* 逻辑地址的中的主用板 */
#define CPSS_COM_ADDRFLAG_SLAVE    ((UINT32)(0x01)) /* 逻辑地址的中的备用板 */
#define CPSS_COM_ADDRFLAG_PHYSICAL ((UINT32)(0x10)) /* 物理地址 */

/* 优先级别，用于CPSS_COM_MSG_HEAD_T */
#define CPSS_COM_PRIORITY_LOW      ((UINT32)(0x0)) /* 低优先级 */
#define CPSS_COM_PRIORITY_NORMAL   ((UINT32)(0x1)) /* 普通优先级 */
#define CPSS_COM_PRIORITY_URGENT   ((UINT32)(0x2)) /* 紧急优先级 */

/* 可靠标志，用于CPSS_COM_MSG_HEAD_T */
#define CPSS_COM_ACK               ((UINT32)(0x1)) /* 要求可靠发送（确认）*/
#define CPSS_COM_NOT_ACK           ((UINT32)(0x0)) /* 要求不可靠发送（无确认）*/

#define CPSS_COM_MEM_NOT_SHARE    ((UINT8)0)
#define CPSS_COM_MEM_SHARE            ((UINT8)1)

/* 无效逻辑地址 */
#define CPSS_COM_LOCAL_LOGIC_ADDR  ((UINT32)(0x00000000))

/* TCP连接上是否允许转发数据的标志，用于TCP连接绑定函数接口 */
#define	CPSS_COM_TCP_CONNECT_ROUTE_ALLOW		(0)
#define	CPSS_COM_TCP_CONNECT_ROUTE_NOT_ALLOW	(1)

/*TCP连接请求消息*/
#define CPSS_COM_TCP_CONNECT_REQ_MSG   0x21000310
/*TCP连接响应消息*/
#define CPSS_COM_TCP_CONNECT_RSP_MSG   0x21000311

/*网元直连部分消息*/
#define CPSS_COM_TCP_MML_USER_DATA_MSG 0x21000312

/*网元直连登录请求消息*/
#define CPSS_COM_TCP_USR_LOGIN_IND_MSG 0x21000313
/******************************** 基本通信结束 ********************************/

/******************************** 消息跟踪相关 ********************************/

/* 消息跟踪上报,一个数据包长度（包括控制头）,单位：字节 */
#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_DSP)
#define CPSS_TRACE_PACKET_LEN            ((UINT32)(1024))
#else
#define CPSS_TRACE_PACKET_LEN            ((UINT32)(60*1024))
#endif
#define CPSS_COM_TRACE_TASK_MAX_NUM        (4)        /* 最大消息跟踪任务数 */

/* 是否进行路径TRACE */
#define CPSS_MSG_TRACE_ROUTE             (1)
#define CPSS_MSG_NO_TRACE_ROUTE          (0)
#define CPSS_TRACE_BIT_SET(traceflag)    (traceflag | 1)
#define CPSS_TRACE_BIT_GET(traceflag)    (traceflag & 0x1)
#define CPSS_TRACE_BIT_CLEAR(traceflag)  (traceflag & 0)

typedef struct
{
    UINT32 ulUsedFlag; /* only for alloc */
    UINT32 ulValidFlag; /* when all info set completed, then set it True */
    UINT32 ulCurLayer; /* current dealing which layer */
    UINT32 ulCurIfType; /* current dealing used which interface to come in */
    UINT32 ulMsgId;
    UINT8 ucAddrType;
    UINT32 ulAddr;
    UINT8 ulAddrFlag;
    UINT32 ulProcId;
    UINT32 ulLdtLogAddr;
    UINT32 ulTraceNum;
}CPSS_TRACE_HOOK_INFO_T;
#define CPSS_TRACE_ROUTE_TASK_MAX (1)


#define CPSS_TRACE_FREE (0)
#define CPSS_TRACE_USED (1)

/* ERROR code */
#define CPSS_IPC_SEND_ERR    (1)
#define CPSS_PCI_WRITE_ERR   (2)

/******************************** 消息跟踪结束 ********************************/

/******************************** 类型定义 ***********************************/

/******************************** 基本通信相关 ********************************/

/* TCP连接相关接口 */
typedef struct tagCPSS_COM_TCP_LINK
{
    UINT32 ulIP;       /* OMC/LDT的IP地址，网络字节顺序 */
    UINT16 usTcpPort;  /* OMC/LDT接入时的TCP端口号，网络字节顺序 */
    UINT16 usReserved; /* 保留，填0 */
} CPSS_COM_TCP_LINK_T;

/*TCP的连接请求消息*/
typedef struct tagCPSS_COM_TCP_CONNECT_REQ_MSG_T
{
    UINT32 ulIP ;
    UINT16 usTcpPort ;
    UINT16 usReserved; /* 保留，填0 */
}CPSS_COM_TCP_CONNECT_REQ_MSG_T ;

typedef CPSS_COM_TCP_LINK_T CPSS_COM_TCP_CONN_IND_MSG_T;
typedef CPSS_COM_TCP_LINK_T CPSS_COM_TCP_DISCONN_IND_MSG_T;

/*TCP的连接响应消息*/
typedef struct tagCPSS_COM_TCP_CONNECT_RSP_MSG_T
{
    CPSS_COM_TCP_LINK_T stLinkInfo;
    UINT32 ulResult;
}CPSS_COM_TCP_CONNECT_RSP_MSG_T ;

/* 软件中断指示消息 */
typedef struct tagCPSS_COM_IRQ_IND_MSG
{
    UINT32 ulIrqNo;  /* 中断号 */
    UINT8 *pucBuff;  /* 中断信息 */
} CPSS_COM_IRQ_IND_MSG_T;


/* SMSS上电消息 */
typedef struct tagCPSS_COM_POWER_ON_MSG 
{
    BOOL bMateExist;  /* 是否存在伙伴板：TRUE 存在，FALSE 不存在 */
    CPSS_COM_PHY_ADDR_T  stMatePhyAddr; /* 伙伴板的物理地址 */
} CPSS_COM_POWER_ON_MSG_T;

/******************************** 基本通信结束 ********************************/

/******************************** 消息跟踪相关 ********************************/
/* 消息跟踪过滤条件: 相关点选择枚举类型 */
typedef enum
{
    CPSS_COM_TRACE_ASSPT_ALL_CPU,           /* 相关点为所有CPU */
    CPSS_COM_TRACE_ASSPT_LOCAL_CPU,         /* 相关点为本处理器 */
    CPSS_COM_TRACE_ASSPT_PHYADDR,           /* 相关点为指定物理地址 */
    CPSS_COM_TRACE_ASSPT_LOGICADDR_MASTER,  /* 相关点为逻辑地址主用方 */
    CPSS_COM_TRACE_ASSPT_LOGICADDR_SLAVE,   /* 相关点为逻辑地址备用方 */
    CPSS_COM_TRACE_ASSPT_LOGICADDR_BOTH     /* 相关点为逻辑地址主备双方 */
} CPSS_COM_TRACE_ASSPT_E;

/* 消息跟踪过滤条件: 消息方向选择枚举类型 */
typedef enum
{
    CPSS_COM_TRACE_MSGDIR_TRCPT_TO_ASSPT,   /*跟踪TrcPt -->AssPt的消息 */
    CPSS_COM_TRACE_MSGDIR_ASSPT_TO_TRCPT,   /*跟踪TrcPt<-- AssPt的消息 */
    CPSS_COM_TRACE_MSGDIR_DUAL              /*跟踪TrcPt<-->AssPt的消息(双向) */
} CPSS_COM_TRACE_MSGDIR_E;

typedef struct tagCPSS_COM_TRACE_FILTER_PID
{
    /* 逻辑地址过滤条件
    *  (1) 逻辑地址中的ucModule字段如果取值为0xff则表示所有Module, 否则表示特定的Module;
    *  (2) 逻辑地址中的usGroup字段如果取值为0xffff则表示所有Group, 否则表示特定的Group;
    *  (3) 逻辑地址中的ucSubGroup字段如果取值为0xff则表示所有SubGroup, 否则表示特定
    *      的SubGroup;
    */
    CPSS_COM_LOGIC_ADDR_T  stLogAddr ;

    /*物理地址*/
    CPSS_COM_PHY_ADDR_T    stPhyAddr ;

    UINT32 ulAddrFlag ;
    
    /* 逻辑地址主备标记过滤条件 */
    CPSS_COM_TRACE_ASSPT_E  enAddrFlagFilter;
    
    /* 纤程描述符过滤条件
    *  (1) 如果不需要根据纤程类型字段进行过滤, 则设置纤程类型范围为0~0xffff;
    *  (2) 如果不需要根据纤程号字段进行过滤, 则设置纤程号范围为0~0xffff.
    */
    UINT32 ulPnoMin;       /* 纤程号最小值 */
    UINT32 ulPnoMax;       /* 纤程号最大值 */
}CPSS_COM_TRACE_FILTER_PID_T;

/* 消息跟踪过滤条件
* 跟踪点(Trace Point, TrcPt) <--> 相关点(Associated Point, AssPt) */
typedef struct tagCPSS_COM_TRACE_FILTER_T
{
    /* 条件1: 跟踪点选择和过滤条件 */
    CPSS_COM_PHY_ADDR_T     stTrcPtPhyAddr;     /* 跟踪点物理地址 */
    UINT32  ulTrcPtPdMin;       /* 跟踪点纤程描述符最小值 */
    UINT32  ulTrcPtPdMax;       /* 跟踪点纤程描述符最大值 */
    
                                /* 条件2: 相关点选择和过滤条件
                                *  说明 :
                                *   1)  相关点选择为"本CPU"或"所有CPU", 则物理地址和逻辑地址字段无效. 
                                *   2)  相关点选择为"物理地址"时, 则物理地址字段有效, 逻辑地址字段
                                *       无效. 其中机框/机架/槽位/处理器编号字段为0xFF, 分别表示适配
                                *       所有的机框/机架/槽位/处理器.
                                *   3)  相关点选择为"逻辑地址"时, 则逻辑地址字段有效, 物理地址字段 
                                *       无效. 其中Module/Group/SubGroup字段为0xFF/0xFFFF/0xFF时, 分
                                *       别表示适配所有的Module/Group/SubGroup.
    */
    CPSS_COM_TRACE_ASSPT_E  enAssPtChoice;   /* 相关点选择 */
    CPSS_COM_PHY_ADDR_T     stAssPtPhyAddr;  /* 相关点物理地址 */
    CPSS_COM_LOGIC_ADDR_T   stAssLogicAddr;  /* 相关点逻辑地址 */
    UINT32  ulAssPtPdMin;       /* 相关点纤程描述符最小值 */
    UINT32  ulAssPtPdMax;       /* 相关点纤程描述符最大值 */
    
    /* 条件3: 消息方向和类型选择 */
    CPSS_COM_TRACE_MSGDIR_E enMsgDirFlag;       /* 消息方向选择 */
    UINT32 ulMsgIdMin;  /* 消息ID最小值 */
    UINT32 ulMsgIdMax;  /* 消息ID最大值 */
}CPSS_COM_TRACE_FILTER_T;


/* 每包跟踪消息头结构*/
/* 紧接着头结构的是usInfoNum个跟踪信息项, 
 * 每个信息项均以头部CPSS_COM_TRACE_ENTRY_T开始
 */
typedef struct tagCPSS_COM_TRACE_HEAD
{
    UINT32  ulSeqID;    /* O接口消息流水号（固定为0）*/
    UINT32  ulTraceID;    /* 消息跟踪任务号 */
    CPSS_COM_PHY_ADDR_T  stPhyAddr;  /* 上报端的物理地址 */
    UINT8 ucBoardType;  /* 功能板类型信息，以方便显示 */
    UINT8 ucReserved;
    UINT16 usInfoNum;  /* 消息跟踪信息项数目 */
} CPSS_COM_TRACE_HEAD_T;

/* 每一条被跟踪消息，控制头 */
typedef struct tagCPSS_COM_TRACE_ENTRY
{
    UINT32  ulSerialNo;  /* 消息跟踪序列号，每个跟踪任务维护各自的序列号 */
    UINT32  ulTick;      /* CPU的tick数 */
    CPSS_TIME_T  stTime; /* 绝对时间 */
    UINT32  ulInfoLen;   /* 本跟踪项信息体的长度（包括消息头和消息体） */
} CPSS_COM_TRACE_ENTRY_T;
  /* 紧接着的是被跟踪的消息，
   * 包括消息头（CPSS_COM_MSG_HEAD_T）以及长度为
   * （ulInfoLen－sizeof （CPSS_COM_MSG_HEAD_T））   字节的消息体。
   */

/******************************** 消息跟踪结束 ********************************/
#define CPSS_TRACE_VALID_FLAG (0xa7f7f6a7)
typedef struct
{
    UINT32 ulTraceFlag; /* valid = 0xa7f7f7a7 */

    UINT32 ulLdtLogAddr;
    UINT32 ulSrcAddr;
    UINT32 ulDstAddr;
    UINT32 ulProcID;
    UINT32 ulMsgId;
    UINT32 ulSrcComIfType;
    UINT32 ulTsSrcComIf;
    UINT32 ulTsSrcComIfTick;

    UINT32 ulSrcDrvIfType;
    UINT32 ulTsSrcDrvIfTick;

    UINT32 ulDstDrvIfType;
    UINT32 ulTsDstRDrvIf;
    UINT32 ulTsDstRDrvIfTick;

    UINT32 ulRecvSchNum;
    UINT32 ulTsDstGiveSchTick;

    UINT32 ulDstRecvProcID;
    UINT32 ulTsDstSchTick;

    UINT32 ulRRDrvIfType;
    UINT32 ulRouteAddr;
    UINT32 ulTsRRDrvIf;
    UINT32 ulTsRRDrvIfTick;

    UINT32 ulRSDrvIfType;
    UINT32 ulTsRSDrvIfTick;

    UINT32 ulErrno;
}CPSS_TRACE_INFO_T;

/*网元直连接口部分结构定义*/
typedef struct tagCPSS_COM_TCP_USR_LOGIN_IND_MSG_T
{
    CPSS_COM_TCP_LINK_T stTcpInfo; /*远端TCP信息*/
    UINT8 aucBuf[1024]; /*用户数据信息*/
}CPSS_COM_TCP_USR_LOGIN_IND_MSG_T ;

/* Node location: src/route/dst */
#define CPSS_TRACE_NODE_LOC_SRC        1
#define CPSS_TRACE_NODE_LOC_ROUTE    2
#define CPSS_TRACE_NODE_LOC_DST        3

/* msg transmit over com layer: AppIf/DrvIf/GiveSch or ToApp/Sch */
#define CPSS_TRACE_LAYER_APPI           (1)
#define CPSS_TRACE_LAYER_DRVSI          (2)
#define CPSS_TRACE_LAYER_DRVRI          (3)
#define CPSS_TRACE_LAYER_TOSCHI         (4)
#define CPSS_TRACE_LAYER_SCHI           (5)
#define CPSS_TRACE_LAYER_MAX            (6)
#define CPSS_TRACE_LAYER_INVALID        (0xffffffff)

#define CPSS_TRACE_IF_COM_SEND            (1)
#define CPSS_TRACE_IF_SEND_EX             (2)
#define CPSS_TRACE_IF_SEND_PHY            (3)
#define CPSS_TRACE_IF_SEND_MATE           (4)
#define CPSS_TRACE_IF_SEND_LOCAL          (5)
#define CPSS_TRACE_IF_INVALID             (0xffffffff)

#define CPSS_TRACE_IF_DRV_UDP 1
#define CPSS_TRACE_IF_DRV_PCI 2
/******************************** 全局变量声明 *******************************/
extern CPSS_COM_LOGIC_ADDR_T       g_stLogicAddrGcpa;
extern CPSS_COM_LOGIC_ADDR_T       g_stLogicAddrOmcSvr;
extern CPSS_COM_LOGIC_ADDR_T       g_stLogicAddrOmcPmSvr;
extern CPSS_COM_LOGIC_ADDR_T       g_stLogicAddrHlrSvr;

extern CPSS_COM_PHY_ADDR_T         g_stPhyAddrGcpaMaster;
extern CPSS_COM_PHY_ADDR_T         g_stPhyAddrGcpaSlave;

/* 常量逻辑地址 */
#define CPSS_LOGIC_ADDR_GCPA       g_stLogicAddrGcpa
#define CPSS_LOGIC_ADDR_OMC_SVR    g_stLogicAddrOmcSvr
#define CPSS_LOGIC_ADDR_OMC_PM_SVR g_stLogicAddrOmcPmSvr
#define CPSS_LOGIC_ADDR_HLR_SVR    g_stLogicAddrHlrSvr

/* 常量物理地址 */
#define CPSS_PHY_ADDR_GCPA_MASTER  g_stPhyAddrGcpaMaster
#define CPSS_PHY_ADDR_GCPA_SLAVE   g_stPhyAddrGcpaSlave
/*extern */
/******************************** 外部函数原形声明 ***************************/

/************************************* 消息发送 *******************************/

extern INT32 cpss_com_send 
(
    CPSS_COM_PID_T *pstDstPid,
    UINT32 ulMsgId, 
    UINT8 *pucData, 
    UINT32 ulDataLen
);

extern INT32 cpss_com_send 
(
    CPSS_COM_PID_T *pstDstPid,
    UINT32 ulMsgId, 
    UINT8 *pucData, 
    UINT32 ulDataLen
);

extern INT32 cpss_com_send_extend 
(
    CPSS_COM_MSG_HEAD_T *pstMsg
);

extern INT32 cpss_com_send_mate
(
    UINT32 ulDstPd,
    UINT32 ulMsgId, 
    UINT8 *pucData, 
    UINT32 ulDataLen
);

extern INT32 cpss_com_send_mate_extend
(
CPSS_COM_PID_T *pstSrcPid,
UINT32 ulDstPd,
UINT32 ulMsgId, 
UINT8 *pucData, 
UINT32 ulDataLen 
);

extern INT32 cpss_com_send_phy
(
    CPSS_COM_PHY_ADDR_T  stDstPhyAddr,
    UINT32 ulDstPd,
    UINT32 ulMsgId, 
    UINT8 *pucData, 
    UINT32 ulDataLen
);

extern INT32 cpss_com_tcp_link_send
(
    CPSS_COM_TCP_LINK_T *pstTcpLink,
    CPSS_COM_MSG_HEAD_T *pstMsg
);

extern INT32 cpss_com_tcp_link_bind 
(
    CPSS_COM_TCP_LINK_T *pstTcpLink, 
    CPSS_COM_LOGIC_ADDR_T *pstLogicAddr,
    UINT32 ulRouteAllowFlag    
);

extern INT32 cpss_com_tcp_link_disconnect 
(
    CPSS_COM_TCP_LINK_T *pstTcpLink
);

extern INT32 cpss_com_send_broadcast
(
    UINT32 ulMsgId, 
    UINT8 *pucData, 
    UINT32 ulDataLen
);


extern INT32 cpss_com_phy_addr_get
(
    CPSS_COM_PHY_ADDR_T *pstPhyAddr
);

/* 获得本处理器的逻辑地址和主备标记 */
extern INT32 cpss_com_logic_addr_get 
(
    CPSS_COM_LOGIC_ADDR_T *pstLogicAddr,
    UINT32                *pulAddrFlag
);   

/*获得本单板主CPU的逻辑地址和主备标记 */
extern INT32 cpss_com_host_logic_addr_get 
(
    CPSS_COM_LOGIC_ADDR_T *pstHostLogicAddr,
    UINT32                *pulHostAddrFlag
);

/******************************** 消息跟踪相关 ********************************/
extern INT32 cpss_com_trace_start
(
    UINT32 ulTraceID,
    CPSS_COM_TRACE_FILTER_T *pstTraceFilter,
    CPSS_COM_PID_T *pstLdtPid
);
extern INT32 cpss_com_trace_rate_set
( 
    UINT32 ulTraceID,
    UINT32 ulRate
);
extern INT32 cpss_com_trace_stop
(
    UINT32 ulTraceID 
);

extern INT32 cpss_com_irq_send
(
    UINT8 *pucBuf,
    UINT32 ulLen
);

extern INT32 cpss_com_link_slave_cpu_reset
(
 UINT32 ulCpuNo
);

extern INT32 cpss_com_irq_register
(
 UINT32 ulIrqNo,
 UINT32 ulPd
);

extern INT32 cpss_com_slave_logic_addr_get
(
    CPSS_COM_LOGIC_ADDR_T *pstSlaveLogicAddr,
    UINT32                *pulAddrFlag
);

UINT32 cpss_byte_order_change_ulong 
(
    UINT32 ulNet
);

UINT16 cpss_byte_order_change_ushort 
(
    UINT16 usNet
);

extern INT32 cpss_com_trafic_ip_get_by_logicaddr
(
 CPSS_COM_LOGIC_ADDR_T    *pstDstLogicAddr,
 UINT32                   ulPortId,
 UINT32                   *pulIp
);

extern INT32 cpss_com_trace_start_dsp
(
 UINT8 ucDspNo,
 UINT32 ulTraceID,
 CPSS_COM_TRACE_FILTER_T *pstTraceFilter,
 CPSS_COM_PID_T *pstLdtPid
) ;

INT32 cpss_com_trace_rate_set_dsp
(
 UINT8  ucDspNo,
 UINT32 ulTraceID,
 UINT32 ulRate
 );

INT32 cpss_com_trace_stop_dsp
(
 UINT8  ucDspNo,
 UINT32 ulTraceID
 );

INT32 cpss_com_m2s_log_addr_get
(
 UINT8 ucCpuNo,
 CPSS_COM_LOGIC_ADDR_T* pstMLogicAddr,
 CPSS_COM_LOGIC_ADDR_T* pstSLogicAddr
);

extern INT32 cpss_ip_info_get_by_phyaddr
(
 CPSS_COM_PHY_ADDR_T stPhyAddr,
 UINT8 ucPortType,
 UINT32 *pulIpAddr,
 UINT8* pucMacAddr
 );

extern INT32 cpss_local_brd_ip_get
(
 UINT8 ucPortType,
 UINT32 *pulIpAddr,
 UINT8* pucMacAddr
 );

 extern INT32 cpss_com_ldt_send(UINT8* pucSendBuf,UINT32 ulSendLen) ;

extern INT32 cpss_com_abox_ipinfo_get
(
CPSS_COM_TCP_LINK_T* pstDstAddr,
CPSS_COM_TCP_LINK_T* pstSrcAddr
);

extern INT32 cpss_vos_iproute_add
(
 UINT8 *pcDest,
 UINT8 *pcGateway,
 UINT32 ulMask,
 UINT32 ulTos, 
 UINT32 ulFlag
 );
 
 extern INT32 cpss_vos_iproute_delete
(
 UINT8 *pcDest,
 UINT32 ulMask,
 UINT32 ulTos, 
 UINT32 ulFlag
 );

#if  (CPSS_BIT_ORDER == CPSS_BIT_BIG)
#define  cpss_ntohs(x)    ((UINT16)(x))
#define  cpss_ntohl(x)    ((UINT32)(x))
#define  cpss_ntohll(x)   ((UINT64)(x))
#define  cpss_htons(x)    ((UINT16)(x))
#define  cpss_htonl(x)    ((UINT32)(x))
#define  cpss_htonll(x)   ((UINT64)(x))
#elif  (CPSS_BIT_ORDER == CPSS_BIT_LITTLE)

#define cpss_ntohl(x)   (((((UINT32)(x)) & 0x000000ff) << 24) | \
                         ((((UINT32)(x)) & 0x0000ff00) <<  8) | \
                         ((((UINT32)(x)) & 0x00ff0000) >>  8) | \
                         ((((UINT32)(x)) & 0xff000000) >> 24))

#define cpss_htonl(x)   (((((UINT32)(x)) & 0x000000ff) << 24) | \
                         ((((UINT32)(x)) & 0x0000ff00) <<  8) | \
                         ((((UINT32)(x)) & 0x00ff0000) >>  8) | \
                         ((((UINT32)(x)) & 0xff000000) >> 24))

#define cpss_ntohs(x)   (((((UINT16)(x)) & 0x00ff) << 8) | \
                         ((((UINT16)(x)) & 0xff00) >> 8))

 
#define cpss_htons(x)   (((((UINT16)(x)) & 0x00ff) << 8) | \
                         ((((UINT16)(x)) & 0xff00) >> 8))

#define	cpss_ntohll(x)	    (((((UINT64)(x)) & 0x00000000000000FF) << 56) | \
							 ((((UINT64)(x)) & 0x000000000000FF00) << 40) | \
							 ((((UINT64)(x)) & 0x0000000000FF0000) << 24) | \
							 ((((UINT64)(x)) & 0x00000000FF000000) <<  8) | \
							 ((((UINT64)(x)) & 0x000000FF00000000) >>  8) | \
							 ((((UINT64)(x)) & 0x0000FF0000000000) >> 24) | \
							 ((((UINT64)(x)) & 0x00FF000000000000) >> 40) | \
							 ((((UINT64)(x)) & 0xFF00000000000000) >> 56))

#define	cpss_htonll(x)	    (((((UINT64)(x)) & 0x00000000000000FF) << 56) | \
							 ((((UINT64)(x)) & 0x000000000000FF00) << 40) | \
							 ((((UINT64)(x)) & 0x0000000000FF0000) << 24) | \
							 ((((UINT64)(x)) & 0x00000000FF000000) <<  8) | \
							 ((((UINT64)(x)) & 0x000000FF00000000) >>  8) | \
							 ((((UINT64)(x)) & 0x0000FF0000000000) >> 24) | \
							 ((((UINT64)(x)) & 0x00FF000000000000) >> 40) | \
							 ((((UINT64)(x)) & 0xFF00000000000000) >> 56))

#endif


#if  0 
#define cpss_ntohs(x) (cpss_byte_order_change_ushort(x))
#define cpss_ntohl(x) (cpss_byte_order_change_ulong(x))
#define cpss_htons(x) (cpss_byte_order_change_ushort(x))
#define cpss_htonl(x) (cpss_byte_order_change_ulong(x))
#endif
******************************** 消息跟踪结束 ********************************/

#if SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA
#define BOOT_KEY 8964
#elif SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA
#define BOOT_KEY 6489
#endif

#define BOOT_MSG_LENTH 128
typedef struct _BOOT_MSG_T_
{
	UINT32 ulMsgId;
	UINT8 ucMsg[128];
}BOOT_MSG_T;
typedef struct _BOOT_IPC_MSG_T_
{
	long lMsgType;
	char cIpcMsg[132];
}BOOT_IPC_MSG_T;

void cps_com_boot_msg_init();
void cps_com_boot_snd_msg(char* stBootMsg);
void cps_com_boot_rcv_msg(char* stBootMsg);

#define CPS_BOOT_CPU_REBOOT_IND_MSG 0x21080000
#define CPS_BOOT_RENEW_VERSION_IND_MSG 0x21080001
#define CPS_BOOT_DOWNLOAD_FAIL_ALARM 0x21080002

INT32 cpss_dst_dev_logaddr_get(CPSS_COM_LOGIC_ADDR_T *pstLogicAddr, UINT32 *pulAddrFlag);
INT32 cpss_com_init_mate();
extern INT32 cpss_ipport_init();
extern UINT32 cpss_route_ipv4_init();

/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_COM_H */
/******************************** 头文件结束 *********************************/

