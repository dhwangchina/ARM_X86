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
#ifndef CPSS_COM_DRV_H
#define CPSS_COM_DRV_H

/********************************* 头文件包含 ********************************/
#include "cpss_public.h"
#include "cpss_common.h"
#include "cpss_com_common.h"
#include "cps_replant.h"

/******************************** 宏和常量定义 *******************************/

#ifdef CPSS_PCI_INCLUDE
#define  CPSS_DRV_PCI_INFO_ARRAY_MAX   DRV_MAX_PCI_MEM_NUM
#endif
#ifdef CPSS_HOST_CPU
#define  CPSS_PCI_INFO_ARRAY_MAX   12
#define MPC8560_MEM_PCI_BASE_ADDR 0xa0000000
#else
#define  CPSS_PCI_INFO_ARRAY_MAX   1
#endif

#define  DRV_PCI_DSP_BASE_AADR    0x80000000
#define  CPSS_DSP_CPU_NUM          12
#define  CPSS_DSP_BASE_NUM         5


/* 最大驱动个数 */
#define CPSS_COM_MAX_DRV_NUM         (6)

/* 驱动类型 */
#define CPSS_COM_DRV_UDP             ((UINT32)(0))
#define CPSS_COM_DRV_TCP_SERVER      ((UINT32)(1))
#define CPSS_COM_DRV_TCP_CLIENT      ((UINT32)(2))
#define CPSS_COM_DRV_PCI             ((UINT32)(3))
#define CPSS_COM_DRV_PCI_SLAVE       ((UINT32)(4))
#define CPSS_COM_DRV_DC_SERVER       ((UINT32)(5))

/* UDP/TCP服务器端口号 */
#define CPSS_COM_UDP_SERVER_PORT (5050)
#define CPSS_COM_TCP_SERVER_PORT (5000) /* 参考O接口手册 */
#define CPSS_COM_DC_SERVER_PORT        (5050) /* 网元直连监听端口 */

/*GCPA作为server连接的TCP链路数据*/
#define CPSS_COM_DRV_TCP_NUM 20

/*GCPA作为server连接的网元直连链路最大个数*/
#define CPSS_COM_DRV_DC_TCP_NUM 20

/*GCPA作为client接入的TCP数据*/
#define CPSS_COM_DRV_CLIENT_TCP_NUM 2

#define CPSS_COM_TCP_SEND_MAX_LEN 64*1024
#define CPSS_COM_DC_RECV_MAX_LEN  16*1024

/* 以下参考G1/O接口手册 */
#define CPSS_COM_TCP_CPS__OAMS_IP_LOC (0x04)
#define CPSS_COM_TCP_CPS__OAMS_PORT_LOC (0x08)

/*PCI相关宏定义*/
/* must be modify */
#define CPSS_COM_PCI_V_IRQ_NO         0x1000
#define CPSS_COM_PCI_NOTIFY_IRQ_NO    0x1

#define CPSS_COM_DRV_PCI_READ_NUM_MAX    10

/*PCI内存可读可写标志*/
#define CPSS_COM_PCI_WR_ENABLE   0x1
#define CPSS_COM_PCI_WR_DISABLE  0x0

/*UDP收发统计相关宏定义*/
#define CPSS_UDP_STAT_LOGSHELF_MAX (CPSS_COM_MAX_FRAME*CPSS_COM_MAX_SHELF)
#define CPSS_UDP_STAT_SLOT_MAX (CPSS_COM_MAX_SLOT)

#define CPSS_UDP_DRV_STAT_SEND (0)
#define CPSS_UDP_DRV_STAT_RECV (1)
#define CPSS_UDP_DRV_STAT_ERR   (2)

/*PCI统计类型的宏定义*/
#define CPSS_COM_PCI_SEND_NUM_FLAG      (0)
#define CPSS_COM_PCI_SENDERROR_NUM_FLAG (1)
#define CPSS_COM_PCI_RECV_NUM_FLAG      (2)
#define CPSS_COM_PCI_FAULT_NUM_FLAG     (3)
#define CPSS_COM_PCI_RECV_DELIVER_ERR_NUM_FLAG (4)
#define  DRV_PCI_DSP_ADDR_OFFSET  (63*1024*1024)
#define CPSS_TCP_NO_BIND   0x1
#define CPSS_TCP_NO_ROUTE  0x2
#define CPSS_TCP_ROUTE     0x3
#define CPSS_COM_PCI_CHECK_VALUE 0x410127

/* TCP listen操作定时器消息 */
#define CPSS_COM_TCP_LISTEN_TM_MSG      CPSS_TIMER_00_MSG
/* TCP listen操作定时器号 */
#define CPSS_COM_TCP_LISTEN_TM_NO       (CPSS_COM_TCP_LISTEN_TM_MSG&0xFF)
/* TCP listen操作定时器时间间隔 */
#define CPSS_COM_TCP_LISTEN_TM_INTERVAL (2000)

/* TCP connect1操作定时器消息 */
#define CPSS_COM_TCP_CONNECT_TM0_MSG      CPSS_TIMER_01_MSG
/* TCP connect1操作定时器号 */
#define CPSS_COM_TCP_CONNECT_TM0_NO       (CPSS_COM_TCP_CONNECT_TM0_MSG&0xFF)

/* TCP connect2操作定时器消息 */
#define CPSS_COM_TCP_CONNECT_TM1_MSG      CPSS_TIMER_02_MSG
/* TCP connect2操作定时器号 */
#define CPSS_COM_TCP_CONNECT_TM1_NO       (CPSS_COM_TCP_CONNECT_TM1_MSG&0xFF)

/* TCP connect2操作定时器时间间隔 */
#define CPSS_COM_TCP_CONNECT_TM_INTERVAL (2000)

#define CPSS_COM_TCP_CONNECT_PORT        (5000)

/*serverflag*/
#define CPSS_COM_TCP_SERVER_FLAG         0
/*clientflag*/
#define CPSS_COM_TCP_CLIENT_FLAG         1
/*网元直连链路标志*/
#define CPSS_COM_TCP_DC_FLAG             2

#define CPSS_COM_TCP_LINK_BUFLEN_MAX     1024*100

#define CPSS_TCP_RECV_SEND_OK    0x2
#define CPSS_TCP_RECV_END        0x3

#define CPSS_TCP_PKG_NOT_FINISH  0x0
#define CPSS_TCP_PKG_FINISH      0x1

#define CPSS_COM_LDT_ADDR_PORT       5070
#define CPSS_COM_LDT_ADDR_IP         0x0ac20901

#define CPSS_COM_TCP_LISTEN_STAT_INIT    0
#define CPSS_COM_TCP_LISTEN_STAT_NORMAL  1
#define CPSS_COM_DC_LISTEN_STAT_INIT     0
#define CPSS_COM_DC_LISTEN_STAT_NORMAL   1
#define CPSS_COM_TCP_SERVER_USRID        1
#define CPSS_COM_DC_SERVER_USRID         2
/******************************** 类型定义 ***********************************/

/* 驱动适配层, 读函数指针 */
typedef INT32 (*CPSS_COM_DRV_READ_PTR)
(
    UINT32  ulDrvType,
    UINT32  ulLinkId, 
    UINT8  *pucBuf,
    UINT32  ulMaxSize
);

/* 驱动适配层, 写函数指针 */
typedef INT32 (*CPSS_COM_DRV_WRITE_PTR)
(
    UINT32  ulLinkId,
    UINT32  ulLinkInfo[3],
    UINT8  *pucBuf,
    UINT32  ulMaxSize,
    UINT32 *ulWriteSize    
);

/* 驱动适配层, 参数控制函数指针 */
typedef INT32 (*CPSS_COM_DRV_CTRL_PTR)
(
    UINT32 ulDrvType,
    UINT32 ulCmd,
    UINT32 ulParam
);

/* 驱动表项 */
typedef struct tagCPSS_COM_DRV
{
    UINT32 ulDrvType;                  /* CPSS_COM_DRV_UDP */
    UINT32 ulDrvD;                     /* IP ulSocket, PCI devID 驱动描述符 */
    CPSS_COM_SOCK_ADDR_T stSocketAddr; /* SOCKET 地址 */

    /* 读写/控制函数指针 */
    CPSS_COM_DRV_READ_PTR  pfRead;
    CPSS_COM_DRV_WRITE_PTR pfWrite;
    CPSS_COM_DRV_CTRL_PTR  pfIoCtl;
}CPSS_COM_DRV_T;

/*tcp链路收发数据统计*/
typedef struct tagCPSS_COM_TCP_STAT
{
    UINT32 ulSendNum ;          /*TCP数据发送包数*/
    UINT32 ulRecvNum ;          /*TCP数据接收包数*/
    UINT32 ulDeliverErrNum ;    /*TCP数据上交失败次数统计*/

    UINT32 ulTcpSendFailNum ;       /*申请IPC内存失败次数*/
    UINT32 ulTcpStackNotEnoughNum ;/*TCP协议栈缓存不足*/

}CPSS_COM_TCP_STAT_T;

/*tcp链路建立、拆除统计*/
typedef struct tagCPSS_COM_DRV_TCP_SETUP_STAT
{
    UINT32 ulLinkSetupNum ;            /*链路建立次数*/
    UINT32 ulLinkFaultPassiveNum ;     /*链路被动拆除次数*/
    UINT32 ulLinkFaultActiveNum  ;     /*链路主动拆除次数*/
}CPSS_COM_TCP_SETUP_STAT_T ;

typedef struct tagCPSS_TCP_RECV_BUF_M
{
    UINT8* pucDataBuf ;
    UINT32 ulRecvBufLen ;
    UINT32 ulDataFlag ;
    INT32   lDataDiffLen;	
}CPSS_TCP_RECV_BUF_M_T ;

/* tcp链路映射表项 */
typedef struct tagCPSS_COM_DRV_TCP
{
    UINT32 ulSClient;
    UINT32 ulRouteAllowFlag;
    UINT32 ulServerFlag ;
    CPSS_COM_TCP_LINK_T stTcpLink;
    CPSS_COM_LOGIC_ADDR_T stLogicAddr;
    CPSS_COM_TCP_STAT_T  stLinkDataStat;
    CPSS_COM_TCP_SETUP_STAT_T stLinkSetupDelStat; 
    CPSS_TCP_RECV_BUF_M_T stRecvBufM;
}CPSS_COM_DRV_TCP_T;

/*tcp链路管理结构*/
typedef struct tagCPSS_COM_TCP_LINK_MAN_T
{
    /*GCPA作为接入服务器接入的TCP链路*/
    CPSS_COM_DRV_TCP_T astServerTcpLink[CPSS_COM_DRV_TCP_NUM] ;
    /*GCPA作为网元直连的TCP链路*/
    CPSS_COM_DRV_TCP_T astServerDcLink[CPSS_COM_DRV_DC_TCP_NUM] ;
    /*GCPA作为客户端连接的TCP链路*/
    CPSS_COM_DRV_TCP_T astClientTcpLink[CPSS_COM_DRV_CLIENT_TCP_NUM] ;
    /*GCPA作为接入服务器接入的TCP链路条数*/
    UINT32 ulServerTcpNum ;
    /*GCPA作为客户端连接的TCP链路条数*/    
    UINT32 ulClientTcpNum ;
    /*GCPA作为客户端连接的TCP链路信息*/    
    CPSS_COM_TCP_LINK_T stTcpConnectInfo[CPSS_COM_DRV_CLIENT_TCP_NUM] ;
    CPSS_COM_PID_T stOamsPid ;
    UINT32 ulListenSocket ;
}CPSS_COM_TCP_LINK_MAN_T ;

/*TCP连接主动关闭统计*/
typedef struct tagCPSS_TCP_CLOSE_STAT
{
    UINT32 ulTcpRecvErrCloseNum ;
    UINT32 ulTcpSendFullCloseNum ;
}CPSS_TCP_CLOSE_STAT_T ;


/* 链路统计信息 */

#ifdef CPSS_PCI_INCLUDE
typedef struct tagCPSS_COM_PCI_STAT
{
    UINT32 ulSendDataNum;           /*发送数据个数统计*/
    UINT32 ulRecvDataNum;           /*接收数据个数统计*/
    UINT32 ulDeliverErrNum;         /*上交数据失败统计*/
    UINT32 ulLinkSetupNum;          /*链路建立次数统计*/
    UINT32 ulFaultNum;              /*链路故障次数统计*/
    UINT32 ulRecvDataDeliverErrNum; /* 收到数据上交失败统计 */
}CPSS_COM_PCI_STAT_T;

/*PCI链路连接结构*/
typedef struct tagCPSS_COM_PCI_LINK
{
    UINT32 ulCpuNo;
    UINT32 ulWriteMutex;
    UINT32 ulPciReadNum;
    UINT32 ulWRFlag;
    CPSS_COM_LOGIC_ADDR_T stDstLogAddr ;
    CPSS_COM_PHY_ADDR_T   stDstPhyAddr ;    
    CPSS_COM_PCI_STAT_T   stPciStat;
}CPSS_COM_PCI_LINK_T;

/*PCI连接表结构*/
typedef struct tagCPSS_COM_PCI_TABLE
{
    UINT32 ulCpuNum ;
    UINT32 ulNotifyTmr ;
    UINT32 ulPciReadMutex ;
    UINT32 ulIrqConnFlag ;
    CPSS_COM_PCI_LINK_T astPciLink[CPSS_DRV_PCI_INFO_ARRAY_MAX] ;
}CPSS_COM_PCI_TABLE_T ;

typedef struct tagCPSS_COM_PCI_NOTIFY
{
    UINT32 ulProcId ;
    UINT32 ulMsgId;
    UINT16 usMsglen;
    UINT16 usRsv;
    UINT32 ulIrqNo ;
    UINT32 ulCpuNo ;
}CPSS_COM_PCI_NOTIFY_T;
#endif

typedef struct
{
    UINT32 ulRecvNum;
    UINT32 ulSendNum;
    UINT32 ulSendErrNum;
}CPSS_UDP_DRV_STAT_T;

/*TCP接入错误统计*/
typedef struct tagCPSS_COM_TCP_ERR_STAT_T
{
    /*LDT通过交换板接入*/
    UINT32 ulAcceptErrNum ;
    UINT32 ulNoFreeSpaceNum ;
    UINT32 ulConnectSendFailNum ;
    UINT32 ulRecvErrNum ;

    /*LDT通过ONCA板接入*/
    UINT32 ulListenFailNum ;
    UINT32 ulLinkAddFailNum ;
    UINT32 ulExceptionNum ;
}CPSS_COM_TCP_ERR_STAT_T;

/*TCP的接收统计*/
typedef struct tagCPSS_COM_TCP_LISTEN_STAT_T
{
    UINT32 ulTcpProcInit ;
    UINT32 ulListenReqNum ;
    UINT32 ulDcListenReqNum ;
    UINT32 ulListenResult ;
    UINT32 ulDcListenResult ;
}CPSS_COM_TCP_LISTEN_STAT_T ;

extern INT32 cpss_com_drv_pci_wr_set
( 
 UINT32 ulCpuNo,
 UINT32 ulWRFlag
) ;

extern VOID cpss_com_drv_pci_wr_set_all
(
 UINT32 ulWRFlag
) ;

/******************************** 全局变量声明 *******************************/
extern UINT32 g_ulCpssComDrvTcpMutex;


/******************************** 外部函数原形声明 ***************************/

/* 驱动适配层初始化 */
extern INT32  cpss_com_drv_init(VOID);

/* 驱动读数据注册函数 */
extern INT32 cpss_com_drv_read_reg
(
    UINT32 ulDrvType,
    CPSS_COM_DRV_READ_PTR pfCallBackFunc /* 驱动回调函数,读取数据 */
);
    
/* 驱动适配层, 驱动表项查找 */
extern CPSS_COM_DRV_T* cpss_com_drv_find(UINT32 ulDrvType);

extern INT32 cpss_com_drv_read
(
    UINT32  ulDrvType,
    UINT32  ulLinkId, /* socekt,ip,port */
    UINT8  *pucBuf,
    UINT32  ulReadSize    
);

/* 驱动适配层, 写函数 */
extern INT32 cpss_com_drv_write
(
    VOID *pstLink,
    UINT8 *pucBuf,
    UINT32 ulMaxSize,
    UINT32 *pulWriteSize
);

/* 驱动适配层, 参数控制函数 */
extern INT32 cpss_com_drv_ioctl
(
    VOID *pvLink,
    UINT32 ulCmd, 
    UINT32 ulParam
);

/* UDP驱动, 写函数 */
extern INT32 cpss_com_drv_udp_write
(
    UINT32  ulLinkId,
    UINT32  ulLinkInfo[3], /* socekt,ip,port */
    UINT8  *pucBuf,
    UINT32  ulMaxSize,
    UINT32 *ulWriteSize
);

/* TCP驱动, 写函数 */
extern INT32 cpss_com_drv_tcp_write
(
    UINT32  ulLinkId,
    UINT32  ulLinkInfo[3], /* socekt,ip,port */
    UINT8  *pucBuf,
    UINT32  ulMaxSize,
    UINT32 *pulWriteSize    
);

extern INT32 cpss_com_drv_pci_write
(
    UINT32  ulLinkId,
    UINT32  ulLinkInfo[3],
    UINT8  *pucBuf,
    UINT32  ulMaxSize,
    UINT32 *pulWriteSize
);

/*  根据逻辑地址，查找链路，tcp链路使用 */
extern INT32 cpss_com_tcp_drv_find 
(    
    CPSS_COM_LOGIC_ADDR_T stLogicAddr,
    CPSS_COM_DRV_TCP_T *pstTcpDrv    
);

extern INT32 cpss_com_tcp_route_allow
(    
    CPSS_COM_TCP_LINK_T *pstTcpLink,
    CPSS_COM_LOGIC_ADDR_T *pstLogicAddr,
    UINT32* pulServerFlag
);

extern INT32 cpss_com_tcp_socket_exist
(
CPSS_COM_TCP_LINK_T *pstTcpLink
);

extern INT32 cpss_com_tcp_cps__oams_connect_info_send
(
UINT8* pucSendBuf,
UINT32 ulMsgId,
UINT32 ulServerFlag
);
 
extern INT32 cpss_com_drv_udp_read
(
UINT32  ulDrvType,
UINT32  ulLinkId,
UINT8  *pucBuf,
UINT32  ulReadSize
);
 
extern INT32 cpss_com_drv_tcp_read
(
UINT32  ulDrvType,
UINT32  ulLinkId,
UINT8  *pucBuf,
UINT32  ulReadSize
);

extern INT32 cpss_com_drv_pci_read
(
    UINT32  ulDrvType,
    UINT32  ulLinkId,
    UINT8   *pucBuf,
    UINT32  ulReadSize
);

extern INT32 cpss_com_drv_pci_init();
extern INT32  cpss_com_socket_init(UINT32 ulDrvType) ;
extern INT32 cpss_com_pci_link_stat(UINT32  ulLinkId ,UINT32 flag);
extern INT32 cpss_drv_pci_info_get(UINT32* pulCpuNum,DRV_PCI_INFO_T* pstPciDrvInfo);
extern INT32 cpss_com_tcp_del_socket_req_send(UINT32 ulSocketId) ;
extern INT32 cpss_com_tcp_data_send(UINT32 ulSocketId, UINT8* pucSendBuf, UINT32 ulSendLen) ;
extern CPSS_COM_DRV_TCP_T* cpss_com_tcp_link_find(CPSS_COM_TCP_LINK_T* pstTcpLink,UINT32 *pulServerFlag) ;
extern CPSS_COM_DRV_TCP_T* cpss_com_tcp_server_link_find_by_socket(UINT32 ulSocketId) ;
extern CPSS_COM_DRV_TCP_T* cpss_com_tcp_client_link_find_by_socket(UINT32 ulSocketId) ;
extern CPSS_COM_DRV_TCP_T* cpss_com_tcp_server_link_find(CPSS_COM_TCP_LINK_T* pstTcpLink) ;
extern CPSS_COM_DRV_TCP_T* cpss_com_tcp_client_link_find(CPSS_COM_TCP_LINK_T* pstTcpLink) ;
extern INT32 cpss_com_tcp_server_link_id_find(CPSS_COM_TCP_LINK_T* pstTcpLink,UINT32 *pulLinkId) ;
extern CPSS_COM_DRV_TCP_T* cpss_com_tcp_server_free_link_find() ;
extern INT32 cpss_route_add(UINT8* pucDstIP,UINT8* pucGateIP) ;
extern INT32 cpss_com_udp_task_reg();
extern INT32 cpss_com_pci_task_reg();
extern INT32 cpss_com_tcp_listen_task_reg();
extern INT32 cpss_com_dc_listen_task_reg();
extern INT32 cpss_com_tcp_recv_task_reg();
extern INT32 cpss_com_tcp_server_socket_free(UINT32 ulSocketId) ;
extern INT32 cpss_com_ldt_socket_init() ;
extern INT32 cpss_com_ldt_send(UINT8* pucSendBuf,UINT32 ulSendLen) ;

/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_COM_DRV_H */
/******************************** 头文件结束 *********************************/


