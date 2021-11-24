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
#ifndef CPSS_COM_COMMON_H
#define CPSS_COM_COMMON_H

/********************************* 头文件包含 ********************************/
#include "cpss_public.h"
#include "cpss_common.h"
#include "cpss_vk_proc.h"

/******************************** 宏和常量定义 *******************************/
/* 通信头部总长度,8=sizeof(ptNext)+sizeof(timeout) 56 */
#define CPSS_COM_LINK_HEAD_OFFSET       (8)

/* 通信可靠传输头长度 */
#define CPSS_COM_TRUST_HEAD_LEN         (sizeof(IPC_MSG_HDR_T)-CPSS_COM_LINK_HEAD_OFFSET)

/*数据分发头长度*/                                        
#define CPSS_COM_DISP_HEAD_LEN          (sizeof(CPSS_COM_MSG_HEAD_T))

/*通信头的长度=可靠传输头+数据分发头*/
#define CPSS_COM_LINK_HEAD_LEN          (CPSS_COM_TRUST_HEAD_LEN+CPSS_COM_DISP_HEAD_LEN)

/*得到从数据分发头到数据区结束的数据长度*/
#define CPSS_DISP_HDR_TO_DATAEND_LEN_GET(ulDataLen)   ((UINT32)ulDataLen+CPSS_COM_DISP_HEAD_LEN)

/*得到从可靠传输头到数据区结束的数据长度*/
#define CPSS_TRUST_HDR_TO_DATAEND_LEN_GET(ulDataLen)  ((UINT32)ulDataLen+CPSS_COM_LINK_HEAD_LEN)

/*减去可靠传输头的数据长度*/
#define CPSS_TRUST_HDR_LEN_SUBTRACT(ulDataLen)        ((UINT32)ulDataLen-CPSS_COM_TRUST_HEAD_LEN)

/*减去数据分发头的数据长度*/
#define CPSS_DISP_HDR_LEN_SUBTRACT(ulDataLen)        ((UINT32)ulDataLen-CPSS_COM_DISP_HEAD_LEN)

/*减去通信头的数据长度*/
#define CPSS_LINK_HDR_LEN_SUBTRACT(ulDataLen)         ((UINT32)ulDataLen-CPSS_COM_LINK_HEAD_LEN)

/*将包头长度右移2位*/
#define CPSS_COM_HDRLEN_PACK(hdrLen)    (hdrLen >> 2)

/*将包头长度左移2位*/
#define CPSS_COM_HDRLEN_UNPACK(hdrLen)  (hdrLen << 2)

/*将包指针从可靠传输头移动到数据分发头*/
#define CPSS_TRUST_HDR_TO_DISP_HDR(pNode)  (((INT8 *) pNode) + CPSS_COM_TRUST_HEAD_LEN)

/*将包指针从数据分发头移动到可靠传输头*/
#define CPSS_DISP_HDR_TO_TRUST_HDR(pNode)  (((INT8 *) pNode) - CPSS_COM_TRUST_HEAD_LEN)

/*将数据指针从数据数据分发头移动到用户数据区*/
#define CPSS_DISP_HDR_TO_USR_HDR(pNode)    (((INT8 *) pNode) + CPSS_COM_DISP_HEAD_LEN)

/*将数据指针从用户数据区移动到数据数据分发头*/
#define CPSS_USR_HDR_TO_DISP_HDR(pNode)    (((INT8 *) pNode) - CPSS_COM_DISP_HEAD_LEN)

/*将数据指针从可靠传输区移动到用户数据区*/
#define CPSS_TRUST_HDR_TO_USR_HDR(pNode)   (((INT8 *) pNode) + CPSS_COM_LINK_HEAD_LEN)

/*将数据指针从用户数据区移动到可靠传输区*/
#define CPSS_USR_HDR_TO_TRUST_HDR(pNode)   (((INT8 *) pNode) - CPSS_COM_LINK_HEAD_LEN)

/*将数据指针从IPC头移动到可靠传输头*/
#define CPSS_IPC_HDR_TO_TRUST_HDR(pNode)   (((INT8 *) pNode) + CPSS_COM_LINK_HEAD_OFFSET)

/*将数据指针从可靠传输头移动到IPC头*/
#define CPSS_TRUST_HDR_TO_IPC_HDR(pNode)   (((INT8 *) pNode) - CPSS_COM_LINK_HEAD_OFFSET)

/*发送缓存的最大数据个数*/
#define CPSS_COM_SEND_BUF_NUM_MAX     16

/*发送缓存的最大数据长度*/
#define CPSS_COM_SEND_BUF_LEN_MAX     (64*1024)

/*通信版本号*/
#define CPSS_COM_PROTOCAL_VER (1)

#define CPSS_TCP_MODULE_MIN           250        /* TCP链路的最小模组号定义*/

#define CPSS_TCP_LOGIC_ADDR_IF(module) ((module) >= 249)   /*判断逻辑地址是否为TCP的逻辑地址*/

#define CPSS_COM_SEND_PHY_MAX_SIZE    1024       /*send_phy发送的数据长度的最大长度*/

#define CPSS_COM_SEND_MATE_MAX_SIZE   2*1024*1024  /*send_mate发送的数据最大长度*/

#define CPSS_EVENTNO_TRACE_ROUTE_IND 0x20001

#define CPSS_ALARM_BOX_IP_ADDR   0x0a000001

/************************* 时间戳相关宏定义结束**************************/


/******************************** 类型定义 ***********************************/
#ifdef CPSS_DSP_CPU
/*
 * Internet address (old style... should be updated)
 */
struct in_addr {
        union {
                struct { UINT8 s_b1,s_b2,s_b3,s_b4; } S_un_b;
                struct { UINT16 s_w1,s_w2; } S_un_w;
                UINT32 S_addr;
        } S_un;
#define s_addr  S_un.S_addr
                                /* can be used for most tcp & ip code */
#define s_host  S_un.S_un_b.s_b2
                                /* host on imp */
#define s_net   S_un.S_un_b.s_b1
                                /* network */
#define s_imp   S_un.S_un_w.s_w2
                                /* imp */
#define s_impno S_un.S_un_b.s_b4
                                /* imp # */
#define s_lh    S_un.S_un_b.s_b3
                                /* logical host */
};

struct sockaddr_in {
        INT16   sin_family;
        UINT16 sin_port;
        struct  in_addr sin_addr;
        INT8    sin_zero[8];
};
#endif

/* SOCKET地址 */
typedef struct sockaddr_in CPSS_COM_SOCK_ADDR_T ;

/* 逻辑地址内部字段含义 */

typedef struct
{
#if (CPSS_BIT_ORDER == CPSS_BIT_LITTLE)
        BITS16 btSlot:4;
        BITS16 btShelf:4;
        BITS16 btFrame:4;
        BITS16 btRsv:3;
        BITS16 btModuleFlag:1;
#elif (CPSS_BIT_ORDER == CPSS_BIT_BIG)
        BITS16 btModuleFlag:1;
        BITS16 btRsv:3;
        BITS16 btFrame:4;
        BITS16 btShelf:4;
        BITS16 btSlot:4;
#endif
}CPSS_LOGIC_ADDR_GROUP_T;


/*通信可靠传输头定义*/
typedef struct tagCPSS_COM_TRUST_HEAD
{    
#if (CPSS_BIT_ORDER == CPSS_BIT_LITTLE)
    UINT8   hdrLen:5; /* header length: unit is 4 bytes */
    UINT8   ver:3; /* version */
#elif (CPSS_BIT_ORDER == CPSS_BIT_BIG)
    UINT8   ver:3; /* version */
    UINT8   hdrLen:5; /* header length: unit is 4 bytes */
#endif

    UINT8  ucPType; /* type of protocol */
    
#if (CPSS_BIT_ORDER == CPSS_BIT_LITTLE)
    UINT8   fragOffset:5 ;
    UINT8   fragFlag:3 ;    
#elif (CPSS_BIT_ORDER == CPSS_BIT_BIG)
    UINT8   fragFlag:3 ;
    UINT8   fragOffset:5 ;
#endif
    
    UINT8   fragOffsetEx ; /*数据片偏移量的低8位*/
    
    UINT16  usReserved;
    UINT16  usSliceLen; /* unknown? */
    UINT16  usSeq; /* sequence number */
    UINT16  usAck; /* acknowledgement number */
    UINT16  usFlowCtrl; /* flow control */ 
    UINT16  usCheckSum; /* header checksum */

#ifdef CPSS_COM_HDR_WITH_TIMESTAMP
    UINT32  ulTimeStamp[10];
#endif

#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
    CPSS_TRACE_INFO_T  stTimeStamp;
#endif
}CPSS_COM_TRUST_HEAD_T ;

/*动态路由缓存统计*/
typedef struct tagCPSS_STORE_DATA_STAT 
{
    UINT32 ulDataNum ;    /*发送缓冲数据个数*/
    UINT32 ulDataSize ;   /*发送数据长度*/
    UINT32 ulDiscardNum ; /*丢弃的数据个数*/
    UINT32 ulSwitchNum ;  /*主备切换的次数*/
    UINT32 ulTimeoutNum ; /*缓存定时器超时次数*/
    UINT32 ulBufOverFlowNum ; /*超出缓存范围导致数据丢弃次数*/
}CPSS_STORE_DATA_STAT_T;


/*发送缓存区管理节点*/
typedef struct tagCPSS_COM_STORE_NODE
{
    VOID* apvNodeHdr ;    /*发送缓冲头指针*/
    VOID* apvNodeTail ;   /*发送缓冲区尾指针*/
    UINT32 ulDataNum ;    /*发送缓冲数据个数*/
    UINT32 ulDataSize ;   /*发送数据长度*/
    UINT32 ulStoreTmr ;   /*消息缓存定时器*/
    CPSS_STORE_DATA_STAT_T stStoreStat ; /*发送缓存统计结构*/
}CPSS_COM_STORE_NODE_T;

/*发送缓冲管理结构0:主,1:备*/
typedef struct tagCPSS_COM_DATA_STORE_MAN
{
    CPSS_COM_STORE_NODE_T stStoreNode[2]; /*发送缓存区管理节点*/
}CPSS_COM_DATA_STORE_MAN_T;

/*发送数据缓存请求通知消息*/
typedef struct tagCPSS_COM_SEND_DATA_STOR_REQ
{
    UINT32 ulSendDataHdr;    /*缓存的数据链头指针*/
    UINT32 ulSendDataTail;   /*缓存的数据链尾指针*/ 
    CPSS_COM_LOGIC_ADDR_T stLogAddr; /*目的单板的逻辑地址*/
    UINT32 ulAddrFlag;       /*地址标志*/
}CPSS_COM_SEND_DATA_STOR_REQ_T ;

#define CPSS_COM_SPEC_MSG_NUM_MAX  10
#define CPSS_COM_SPEC_MSG_USED_FLAG_BUSY 1
#define CPSS_COM_SPEC_MSG_USED_FLAG_FREE 0

#define CPSS_COM_SPEC_MSG_STAT_OFF 0
#define CPSS_COM_SPEC_MSG_STAT_ON  1

#define COM_SEND_PHASE 0
#define DRV_PCI_SEND_PHASE 1
#define SEND_TO_SLID_PROC_PHASE 2
#define SEND_TO_SLID_PROC_FAIL_PHASE 3
#define DRV_PCI_RECV_PHASE 4
#define SLID_PROC_RECV_PHASE 5
#define IPC_RECV_PHASE 6
#define IPC_RECV_FAIL_PHASE 7
#define DEST_PROC_RECV_PHASE 8
#define DRV_PCI_SEND_FAIL_PHASE 9
/**/
typedef struct tagCPSS_COM_SPEC_MSG_STAT_T
{
    UINT32 ulMsgId ;
    UINT32 ulUsedFlag ;
    UINT32 ulComSendNum ;
    CPSS_TIME_T stComSendTime ;
    UINT32 ulDrvPciSendNum ;
    CPSS_TIME_T stDrvPciSendTime ;    
    UINT32 ulDrvPciSendFailNum ;
    CPSS_TIME_T stDrvPciSendFailTime ;        
    UINT32 ulSendToSlidProcNum ;
    CPSS_TIME_T stSendToSlidProcTime ;            
    UINT32 ulSendToSlidProcFailedNum ;
    CPSS_TIME_T stSendToSlidProcFailTime ;                
    UINT32 ulDrvPciRecvNum ;
    CPSS_TIME_T stDrvPciRecvTime ;                    
    UINT32 ulSlidProcRecvNum ;
    CPSS_TIME_T stSlidProcRecvTime ;                        
    UINT32 ulIpcRecvNum ;
    CPSS_TIME_T stIpcRecvTime ;                        
    UINT32 ulIpcRecvFailNum ;
    CPSS_TIME_T stIpcRecvFailTime ;                            
    UINT32 ulDestProcRecvNum ;
    CPSS_TIME_T stDestProcRecvTime ;
}CPSS_COM_SPEC_MSG_STAT_T ;

extern VOID cpss_com_spec_msg_stat(UINT32 ulMsgId,UINT32 ulPhaseFlag) ;

extern VOID cpss_com_spec_msg_stat_show() ;

extern VOID cpss_com_spec_msg_stat_clear() ;
/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/
extern UINT32 cpss_com_logic2index(CPSS_COM_LOGIC_ADDR_T stLogicAddr);

extern INT32 cpss_com_local_has_backup();

extern VOID cpss_com_slid_show();

extern VOID cpss_com_comm_hdr_hton(CPSS_COM_MSG_HEAD_T *pstMsg);

extern VOID *cpss_com_mem_alloc(INT32 lBytes);

extern VOID* cpss_com_nodisp_mem_alloc(INT32 lBytes);

extern VOID cpss_com_mem_free(VOID* pvBuf);

extern VOID cpss_com_trust_hrd_pub_info_fill(CPSS_COM_TRUST_HEAD_T* pstTrustHdr);

extern INT32 cpss_com_ipc_msg_send(CPSS_COM_TRUST_HEAD_T* pstTrustHdr,INT32 lType);
 
extern VOID cpss_com_hdr_hton(CPSS_COM_TRUST_HEAD_T *pstHdr);

extern VOID cpss_com_hdr_ntoh(CPSS_COM_TRUST_HEAD_T *pstHdr);
 
extern INT32 cpss_com_sock_error_get();

extern INT32 cpss_com_drv_pci_get_addr_by_cpuno
(
    UINT32 ulCpuNo,
    CPSS_COM_LOGIC_ADDR_T *pstLogAddr,
    CPSS_COM_PHY_ADDR_T   *pstPhyAddr
);

extern VOID cpss_com_drv_pci_addr_fill
(
    CPSS_COM_PHY_ADDR_T   stPhyAddr,
    CPSS_COM_LOGIC_ADDR_T stLogAddr,
    UINT32                ulCpuNo
);

extern INT32 cpss_pci_aux_clk_conn(VOID);

extern VOID cpss_com_drv_pci_notify_send();

extern INT32 cpss_com_os_socket_init();

extern BOOL cpss_com_link_if_normal
(
    CPSS_COM_PHY_ADDR_T stPhyAddr
);

extern BOOL cpss_com_link_if_pci_link
(
    CPSS_COM_LOGIC_ADDR_T stLogAddr
);

extern BOOL cpss_com_link_if_dsp_link
(
    UINT32 ulCpuNo
);

extern INT32 cpss_com_drv_pci_enable_read();

extern INT32 cpss_com_drv_pci_linkid_get
(
 UINT32 ulCpuNo,
 UINT32 *pulLinkId
);

extern INT32 cpss_com_drv_pci_notify_write();

VOID cpss_com_drv_pci_get_phyaddr
(
    CPSS_COM_PHY_ADDR_T   stMPhyAddr,
    UINT32                ulCpuNo,
    CPSS_COM_PHY_ADDR_T*  pstSPhyAddr
);

extern UINT32 cpss_com_get_drvtype_by_phyaddr(CPSS_COM_PHY_ADDR_T stPhyAddr);

INT32 cpss_com_send_to_store
(
    CPSS_COM_MSG_HEAD_T *pstSendBuf,
    CPSS_COM_LOGIC_ADDR_T stLogAddr,
    UINT32 ulAddrFlag
);

extern INT32 cpss_com_link_win_get_by_logaddr
(
    VOID **ppvSlidWin,
    CPSS_COM_PID_T *pstLogPid
);

VOID cpss_com_frag_slice_put
(
 UINT8 *pucMem1,
 UINT8 *pucMem2
);

INT32 cpss_com_link_data_store
(
 CPSS_COM_STORE_NODE_T *pstStoreNode,
 CPSS_COM_TRUST_HEAD_T *pstTrustHdr,
 CPSS_COM_TRUST_HEAD_T *pstTrustTail
);

extern INT32 cpss_com_idx2logaddr(UINT32 ulIdx, UINT16 *pusGroup);

extern INT32 cpss_com_phy_addr_mate_get(CPSS_COM_PHY_ADDR_T *pstPhyAddrMate);

extern BOOL cpss_com_mate_phyaddr(CPSS_COM_PHY_ADDR_T stPhyAddrDst);

extern INT32 cpss_com_send_broadcast_msg(CPSS_COM_MSG_HEAD_T *pstMsg);

extern INT32 cpss_com_get_self_ip_addr(UINT32 *pulIpAddr);

extern INT32 cpss_com_get_master_mc_ip_addr(UINT32 *pulIpAddr);

extern INT32 cpss_com_dsp_logic_addr_get(UINT8 ucCpuNo,CPSS_COM_LOGIC_ADDR_T* pstDspLogAddr);

/*消息跟踪相关函数声明*/
extern BOOL cpss_trace_route_it(CPSS_TRACE_HOOK_INFO_T *pstInfo, UINT32 *pulLoc, UINT32 ulCurLayer, UINT32 ulCurIfType);

extern VOID cpss_trace_route_error_hook(UINT8 *pucBuf, UINT32 ulErrno);

extern VOID cpss_trace_route_info_set(UINT32 ulNodeLoc, UINT32 ulLayer, UINT32 ulP1, UINT32 ulP2, UINT32 ulP3, UINT32 ulP4, UINT8 *pucBuf);

extern BOOL cpss_trace_route_it_send(UINT8 *pucBuf, UINT32 *pulIsSrc);

extern BOOL cpss_trace_route_it_recv(UINT8 *pucBuf, UINT32 *pulIsDst);

extern BOOL cpss_trace_route(CPSS_COM_TRUST_HEAD_T *pstTrustHdr);

extern VOID cpss_trace_route_mng_init();

extern VOID cpss_trace_route_event_send(UINT8 *pucBuf);

/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_COM_COMMON_H */
/******************************** 头文件结束 *********************************/


