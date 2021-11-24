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
#ifndef CPSS_COM_LINK_H
#define CPSS_COM_LINK_H

/********************************* 头文件包含 ********************************/
#include "cpss_public.h"
#include "cpss_common.h"

#include "cpss_vk_proc.h"
#include "cpss_com_common.h"
#include "cpss_com_win.h"
#include "cpss_com_drv.h"

/******************************** 宏和常量定义 *******************************/

/* 链路管理纤程状态 */
#define CPSS_COM_LINK_PROC_STATE_INIT    CPSS_VK_PROC_USER_STATE_IDLE
#define CPSS_COM_LINK_PROC_STATE_NORMAL  ((UINT16)(CPSS_VK_PROC_USER_STATE_IDLE+1))

/* 链路状态 */
#define CPSS_COM_LINK_STATUS_UNKNOWN ((UINT16)(0)) /* 链路未知状态 */
#define CPSS_COM_LINK_STATUS_PENDING ((UINT16)(1)) /* 链路建立中状态 */
#define CPSS_COM_LINK_STATUS_NORMAL  ((UINT16)(2)) /* 链路正常状态 */
#define CPSS_COM_LINK_STATUS_FAULT   ((UINT16)(3)) /* 链路故障状态 */

#ifdef CPSS_HOST_CPU

/* 最大路由条数 */
#define CPSS_COM_MAX_ROUTE_NUM       (CPSS_COM_MAX_FRAME*CPSS_COM_MAX_SHELF* \
                                      CPSS_COM_MAX_SLOT*CPSS_COM_MAX_CPU)
/* 最大链路对象的个数 */ 
#define CPSS_COM_MAX_LINK_NUM        CPSS_COM_MAX_ROUTE_NUM

#else

/* 最大路由条数 */
#define CPSS_COM_MAX_ROUTE_NUM       1
/* 最大链路对象的个数 */ 
#define CPSS_COM_MAX_LINK_NUM        1

#endif

#define CPSS_COM_INVALID_ROUTE       (0xFFFFFFFF)
#define CPSS_COM_INVALID_LINK        (0xFFFFFFFF)

/* 地址字段，用于地址转换 */
#define CPSS_IP_HIGH8                ((UINT32)(0x0A))
#define CPSS_IP_MATE_HIGH8           ((UINT32)(0x0F))

/* 链路心跳定时器有效 */
#define CPSS_COM_LINK_HEART_ENABLE      TRUE
/* 链路心跳定时器消息 */
#define CPSS_COM_LINK_HEART_TM_MSG      CPSS_TIMER_01_MSG
/* 链路心跳定时器号 */
#define CPSS_COM_LINK_HEART_TM_NO       (CPSS_COM_LINK_HEART_TM_MSG&0xFF)
/* 链路心跳消息发送周期,单位:毫秒 */
#define CPSS_COM_LINK_HEART_TM_INTERVAL (1000)


/* 链路故障定时器号有效 */
#define CPSS_COM_LINK_FAULT_ENABLE      TRUE
/* 链路故障定时器消息 */
#define CPSS_COM_LINK_FAULT_TM_MSG      CPSS_TIMER_02_MSG
/* 链路故障定时器号 */
#define CPSS_COM_LINK_FAULT_TM_NO       (CPSS_COM_LINK_FAULT_TM_MSG&0xFF)
/* 链路故障定时器超时时间,单位:毫秒 */
#define CPSS_COM_LINK_FAULT_TM_INTERVAL (3*CPSS_COM_LINK_HEART_TM_INTERVAL)


/* 反向地址解析请求超时消息 */
#define CPSS_COM_LINK_RARP_TM_MSG       CPSS_TIMER_03_MSG
/* 反向地址解析请求消息定时器号 */
#define CPSS_COM_LINK_RARP_TM_NO        (CPSS_COM_LINK_RARP_TM_MSG&0xFF)
/* 反向地址解析请求发送周期,单位:毫秒 */
#define CPSS_COM_LINK_RARP_TM_INTERVAL  (2000)


/* 建立链路请求超时消息消息 */
#define CPSS_COM_LINK_REQ_TM_MSG        CPSS_TIMER_04_MSG
/* 建立链路请求超时消息号 */
#define CPSS_COM_LINK_REQ_TM_NO         (CPSS_COM_LINK_REQ_TM_MSG&0xFF)
/* 建立链路请求发送周期,单位:毫秒 */
#define CPSS_COM_LINK_REQ_TM_INTERVAL   (2000)

/* 建立链路请求超时消息消息 */
#define CPSS_COM_MATE_REQ_TM_MSG        CPSS_TIMER_05_MSG
/* 建立链路请求超时消息号 */
#define CPSS_COM_MATE_REQ_TM_NO         (CPSS_COM_MATE_REQ_TM_MSG&0xFF)
/* 建立链路请求发送周期,单位:毫秒 */
#define CPSS_COM_MATE_REQ_TM_INTERVAL   (200)

/*PCI的中断notify定时器消息*/
#define CPSS_COM_PCI_NOTIFY_TM_MSG         CPSS_TIMER_06_MSG
/*PCI的notify定时器号*/
#define CPSS_COM_PCI_NOTIFY_TM_NO          (CPSS_COM_PCI_NOTIFY_TM_MSG&0xFF)
/*PCI的notify定时时间间隔,单位:毫秒*/
#define CPSS_COM_PCI_NOTIFY_TM_INTERVAL    (2000)

/*主CPU的CPU号*/
#define CPSS_COM_PCI_MASTER_CPU_NO       1
/*从CPU的CPU号*/
#define CPSS_COM_PCI_SLAVE_CPU_NO        2
/*DSP的最小编号*/
#define CPSS_COM_PCI_DSP_CPU_NO_MIN      5
/*DSP的最大编号*/
#define CPSS_COM_PCI_DSP_CPU_NO_MAX      17

/*RDBS失败记录最大个数*/
#define CPSS_CPS__RDBS_INFO_RECORD  10 

/*主备切换统计最大条数*/
#define CPSS_COM_SWITCH_RECORD_NUM 10

/*记录的ARP记录最大条数*/
#define CPSS_COM_ARP_ITEM_NUM_MAX      10

#define CPSS_COM_ARP_STAT_REQ_ADD       1
#define CPSS_COM_ARP_STAT_RSP_ADD       2
/*PCI通信去中断号的偏移*/
#define CPSS_IRQ_RSV_HEARD 2

#define CPSS_COM_CTRL_CHANNEL     0
#define CPSS_COM_MATE_CHANNEL     1

#define CPSS_COM_MATE_CHANNEL_DISABLE  0
#define CPSS_COM_MATE_CHANNEL_ENABLE   1

#define CPSS_COM_NO_MASTER_TRANSMIT 0
#define CPSS_COM_MASTER_TRANSMIT    1

#define CPSS_COM_LINK_HEARTBEAT_MAX   5

#define CPSS_DEBUG_LOGIC_ADDR_GET_TM_MSG       CPSS_TIMER_03_MSG
#define CPSS_DEBUG_LOGIC_ADDR_GET_TM_NO        (CPSS_DEBUG_LOGIC_ADDR_GET_TM_MSG&0xFF)
#define CPSS_DEBUG_LOGIC_ADDR_GET_TM_INTERVAL  (1000)

#define CPSS_DEBUG_DIAG_COMM_TEST_TM_MSG       CPSS_TIMER_04_MSG
#define CPSS_DEBUG_DIAG_COMM_TEST_TM_NO        (CPSS_DEBUG_DIAG_COMM_TEST_TM_MSG&0xFF)
#define CPSS_DEBUG_DIAG_COMM_TEST_TM_INTERVAL  (1000)

/*链路连通测试状态宏定义*/
#define CPSS_COM_COMM_TEST_FREE   0
#define CPSS_COM_COMM_TEST_INIT   1
#define CPSS_COM_COMM_TEST_NORMAL 2

/*通信初始化标志*/
#define CPSS_COM_INIT_UNKNOWN     0
#define CPSS_COM_INIT_OK          1

/*ARP地址解析*/
#define CPSS_ARP_REQ_NOT_PROCESSED 0
#define CPSS_ARP_REQ_PROCESSED     1

/*IP路由增加删除标志*/
#define CPSS_COM_ROUTE_ADD 0
#define CPSS_COM_ROUTE_DEL 1

/*定义GCPA主备IP地址字符串*/
#define CPSS_GCPA_M_IP_STR "11.33.9.1"
#define CPSS_GCPA_S_IP_STR "11.33.105.1"

/* 逻辑地址相等 */
#define CPSS_COM_LOGIC_ADDR_SAME(addr1,addr2) (*(UINT32*)&addr1 == *(UINT32*)&addr2)

/* 判断是否是本板数据 */
#define CPSS_COM_LOCAL_BOARD_ADDR(addr1,addr2,addrFlag1,addrFlag2) \
    (*(UINT32*)&addr1 == *(UINT32*)&addr2) && (addrFlag1 == addrFlag2))

/* 逻辑地址为0 */
#define CPSS_COM_LOGIC_ADDR_ZERO(addr) (0 == *(UINT32*)&addr)

/* 本CPU逻辑地址 */
#define CPSS_COM_LOGIC_ADDR_THIS_CPU(addr)\
    CPSS_COM_LOGIC_ADDR_SAME(addr,g_pstComData->stAddrThis.stLogiAddr)

/* 本CPU物理地址 */
#define CPSS_COM_PHY_ADDR_THIS_CPU(addr)\
    CPSS_COM_PHY_ADDR_SAME(addr,g_pstComData->stAddrThis.stPhyAddr)


/* 物理地址相等 */
#define CPSS_COM_PHY_ADDR_SAME(addr1,addr2) (*(UINT32*)&addr1 == *(UINT32*)&addr2)

/* 物理地址为0 */
#define CPSS_COM_PHY_ADDR_ZERO(addr) (0 == *(UINT32*)&addr)
            
/******************************** 类型定义 ***********************************/


/* 链路统计信息 */
typedef struct tagCPSS_COM_LINK_STAT
{
    UINT32 ulSendDataNum;           /*发送数据个数统计*/
    UINT32 ulRecvDataNum;           /*接收数据个数统计*/
    UINT32 ulDeliverErrNum;         /*上交数据失败统计*/
    UINT32 ulLinkSetupNum;          /*链路建立次数统计*/
    UINT32 ulFaultNum;              /*链路故障次数统计*/
    UINT32 ulHeartBeatSendNum;      /*发送心跳次数统计*/ 
    UINT32 ulHeartBeatRecvNum;      /*接收心跳次数统计*/
    UINT32 ulPeerSwitchNum;         /*对端切换次数统计*/
}CPSS_COM_LINK_STAT_T;


/* 路由表项 */
typedef struct tagCPSS_COM_ROUTE
{
    CPSS_COM_PHY_ADDR_T stPhyAddr[2];      /* 物理地址: 0为主用,1为备用 */
    UINT16 ausArpFlag[2];                  /* ARP地址解析标志 */
    CPSS_COM_DATA_STORE_MAN_T *pstStoreMan; /*发送缓冲管理结构指针*/
}CPSS_COM_ROUTE_T;

/* 链路流量统计 */
typedef struct tagCPSS_COM_LINK_VOLUME_STAT
{
    UINT32 ulSendPkgNum ;
    UINT32 ulSendBytes ;
    UINT32 ulSendBytesPerSecond ;
    UINT32 ulSendBytesPerSecondPeek ;
    UINT32 ulSendBytesCur ;
}CPSS_COM_LINK_VOLUME_STAT_T;

/* 链路表项 */
typedef struct tagCPSS_COM_LINK
{   
    UINT32   ulLinkId;  /* 链路标识 */
    UINT16   usStatus;  /* 链路状态 COM_LINK_STATUS_NORMAL等 */
    UINT16   usReserved;
    
    /* 链路故障定时器描述符，参数为对端的物理地址，
     * 超时之后还没有收到对端的心跳消息，则设置链路为无效 
     */
    UINT32   ulLinkReqTd;               /* 链路建立请求定时器描述符 */
    UINT32   ulAddrFlag;                /* 链路对端主备状态 */
    UINT32   ulMateFlag;                /* 主备链路标志 */
    UINT32   ulHeartBeatNum ;           /* 链路心跳次数 */
    CPSS_COM_PHY_ADDR_T   stMPhyAddr;   /* 主用链路的物理地址 */
    CPSS_COM_PHY_ADDR_T   stDstPhyAddr; /* 链路对端的物理地址 */
    CPSS_COM_LOGIC_ADDR_T stDstLogAddr; /* 链路对端的逻辑地址 */
    CPSS_COM_DRV_T        stDrv;        /* 链路驱动 */
    CPSS_COM_SLID_WIN_T   stSlidWin; 
    CPSS_COM_LINK_STAT_T  stLinkStat;
    CPSS_COM_LINK_VOLUME_STAT_T stVolumeStat ; /*链路流量统计*/
}CPSS_COM_LINK_T;

/* 驱动读写,控制函数 */
typedef INT32 (*CPSS_COM_IO_RW)(CPSS_COM_LINK_T  *pstLink, INT8 *pucBuf, INT32 lMaxLen);
typedef INT32 (*CPSS_COM_IO_CTL)(CPSS_COM_LINK_T *pstLink, INT32 lCmd, INT32 lParam);


/* 反向地址解析请求消息 */
typedef struct tagCPSS_COM_RARP_REQ_MSG
{
    UINT32 ulSeqId;                    /* 请求端消息序列号 */
    CPSS_COM_PHY_ADDR_T stPhyAddr;     /* 请求端物理地址 */
    UINT8   ucBoardType;               /* 请求端功能板类型信息 */
    UINT8   ucReserved[3];    
}CPSS_COM_RARP_REQ_MSG_T;

/* 反向地址解析响应消息 */
typedef struct tagCPSS_COM_RARP_RSP_MSG
{
    UINT32 ulSeqId;                    /* 请求端消息序列号 */
    UINT32 ulResult;                   /* 反向地址解析结果:CPSS_OK 成功,CPSS_ERROR 失败 */
    CPSS_COM_PHY_ADDR_T stGcpaPhyAddr; /* 响应端主控板物理地址 */
    CPSS_COM_LOGIC_ADDR_T stLogicAddr; /* 请求端逻辑地址 */
    BOOL   bIsMateExist;               /* 请求端是否存在伙伴板:TRUE 存在,FAUSE 不存在 */
    CPSS_COM_PHY_ADDR_T stMatePhyAddr; /* 请求端伙伴板物理地址 */
}CPSS_COM_RARP_RSP_MSG_T;

/* 地址解析请求消息 */
typedef struct tagCPSS_COM_ARP_REQ_MSG
{
    UINT32 ulSeqId;                    /* 请求端消息序列号 */
    CPSS_COM_LOGIC_ADDR_T stLogicAddr; /* 需要解析的逻辑地址 */    
    UINT32 ulAddrFlag;                 /* 需要解析的地址标志 */
}CPSS_COM_ARP_REQ_MSG_T;

/* 地址解析响应消息 */
typedef struct tagCPSS_COM_ARP_RSP_MSG
{
    UINT32 ulSeqId;                    /* 请求端消息序列号 */
    UINT32 ulResult;                   /* 地址解析结果:CPSS_OK 成功,CPSS_ERROR 失败 */
    CPSS_COM_LOGIC_ADDR_T stLogicAddr; /* 需要解析的逻辑地址 */    
    UINT32 ulAddrFlag;                 /* 需要解析的地址标志 */
    CPSS_COM_PHY_ADDR_T stPhyAddr;     /* 解析得到的物理地址 */    
}CPSS_COM_ARP_RSP_MSG_T;

/* 建立连接请求消息 */
typedef struct tagCPSS_COM_LINK_REQ_MSG
{
    UINT32 ulSeqId;                    /* 请求端消息序列号 */
    CPSS_COM_LOGIC_ADDR_T stLogicAddr; /* 请求端逻辑地址 */    
    UINT32 ulAddrFlag;                 /* 请求端地址标志 */
    CPSS_COM_PHY_ADDR_T stPhyAddr;     /* 请求端物理地址 */
    CPSS_COM_PHY_ADDR_T stMatePhyAddr; /* 请求端伙伴板物理地址 */
    UINT32 ulMateFlag ;                /* 主备通道标志 */
    UINT32 ulMasterTransmitFlag ;      /* 主用板转发标志 */
    CPSS_COM_PHY_ADDR_T stGcpaPhyAddrM;/* 主用GCPA物理地址 */    
}CPSS_COM_LINK_REQ_MSG_T;

/* 建立连接响应消息 */
typedef struct tagCPSS_COM_LINK_RSP_MSG
{
    UINT32 ulSeqId;                    /* 请求端消息序列号 */
    UINT32 ulResult;                   /* 建立连接结果:CPSS_OK 成功,CPSS_ERROR 失败 */
    CPSS_COM_LOGIC_ADDR_T stLogicAddr; /* 响应端逻辑地址 */    
    UINT32 ulAddrFlag;                 /* 响应端地址标志 */
    CPSS_COM_PHY_ADDR_T stPhyAddr;     /* 响应端物理地址 */
    CPSS_COM_PHY_ADDR_T stMatePhyAddr; /* 响应端伙伴板物理地址 */
    UINT32 ulMasterTransmitFlag ;      /* 主用板转发标志 */
}CPSS_COM_LINK_RSP_MSG_T;

/* 链路心跳消息 */
typedef struct tagCPSS_COM_LINK_HEART_MSG
{   
    CPSS_COM_LOGIC_ADDR_T stLogicAddr; /* 发送端逻辑地址 */
    UINT32 ulAddrFlag;                 /* 发送端主备关系 */    
    CPSS_COM_PHY_ADDR_T stPhyAddr;     /* 发送端物理地址 */
}CPSS_COM_LINK_HEART_MSG_T;

/* 地址表 */
typedef struct tagCPSS_COM_ADDR
{    
    UINT32 ulAddrFlag;                 /* 地址标志 */     

    /* 备份类型
     * CPS__RDBS_DEV_11_BACKUP, 单板1+1备份
     * CPS__RDBS_DEV_NO_BACKUP, 单板无备份
    */
    UINT8  ucBackupType;
    UINT8  ucReserved[3];
    CPSS_COM_LOGIC_ADDR_T stLogiAddr;  /* 逻辑地址 */
    CPSS_COM_PHY_ADDR_T   stPhyAddr;   /* 物理地址 */
    UINT32 ulIpAddr;
}CPSS_COM_ADDR_T;

/* 通信模块全局变量 */
typedef struct tagCPSS_COM_DATA
{
    CPSS_COM_ADDR_T stAddrGcpaM;       /* 主控板主用地址 */
    CPSS_COM_ADDR_T stAddrGcpaS;       /* 主控板被用地址 */
    CPSS_COM_ADDR_T stAddrThis;        /* 本板地址 */
    CPSS_COM_ADDR_T stAddrMCup;        /* 主CPU的地址信息*/
    CPSS_COM_PHY_ADDR_T stMatePhyAddr; /* 伙伴板物理地址 */
    
    UINT32   ulRarpReqTNo;             /* 反向地址解析请求定时器号 */
    UINT32   ulMateReqTNo;             /* 主备通道建立请求定时器号 */
    UINT32   ulSeqID;                  /* 消息序列号,保证接收到的响应消息跟请求消息对应 */
}CPSS_COM_DATA_T;

/*PCI相关结构定义*/
typedef struct tagCPSS_COM_PCI_REG_REQ_MSG
{
    UINT32 ulMsgId ;
    UINT32 ulCpuNo ;
    UINT32 ulSumCheck ;
}CPSS_COM_PCI_REG_REQ_MSG_T;

typedef struct tagCPSS_COM_PCI_REG_RSP_MSG
{
    UINT32 ulMsgId ;
    CPSS_COM_PHY_ADDR_T    stMPhyAddr ;
    CPSS_COM_LOGIC_ADDR_T  stMLogAddr ;
    CPSS_COM_PHY_ADDR_T    stSPhyAddr ;
    CPSS_COM_LOGIC_ADDR_T  stSLogAddr ;
    UINT32 ulCheckSum ;
}CPSS_COM_PCI_REG_RSP_MSG_T ;

/*主备切换的统计结构*/
typedef struct
{
	UINT32 ulLogAddr;
	UINT32 ulSrcPhyAddr;
	UINT32 ulDesPhyAddr;
}CPSS_COM_SWITCH_INFO_T;

/*地址初始化信息结构*/
typedef struct tagCPSS_COM_ADDR_INIT_INFO
{
    UINT32 ulAddrInitFlag ;                /*0: 未走此流程 1:失败 2:成功*/
    CPSS_COM_PHY_ADDR_T stThisPhyAddr ;    /*本板物理地址*/
    CPSS_COM_PHY_ADDR_T stGcpaPhyAddrM ;   /*GCPA M 物理地址*/
    CPSS_COM_PHY_ADDR_T stGcpaPhyAddrS ;   /*GCPA S 物理地址*/
    CPSS_COM_LOGIC_ADDR_T stThisLogAddr ;  /*如果是主控板则此项不为空*/
    CPSS_COM_LOGIC_ADDR_T stGcpaLogAddr ;  /*主控板逻辑地址*/
    UINT32  ulThisIp  ;   /*本板IP地址*/
    UINT32  ulGcpaIpM ;   /*GCPA M IP地址*/
    UINT32  ulGcpaIpS ;   /*GCPA S IP地址*/
    CPSS_TIME_T stTime ;  /*执行时间*/
}CPSS_COM_ADDR_INIT_INFO_T ;

/*驱动初始化信息结构*/
typedef struct tagCPSS_COM_DRV_INIT_INFO
{
    UINT32 ulDrvInitFlag ;     /*0: 未走此流程 1:失败 2:成功*/
    UINT32 ulUdpBindStat ;
    UINT32 ulUdpIp ;
    UINT32 ulUdpPort ;
    UINT32 ulTcpBindStat ;
    UINT32 ulTcpIp ;
    UINT32 ulTcpPort ;
    UINT32 ulDcIp ;            /*网元直连IP*/
    UINT32 ulDcPort ;          /*网元直连端口*/
    UINT32 ulDcBindStat ;      /*网元直连绑定状态*/
    UINT32 ulPciInitFlag ;
    CPSS_TIME_T stTime ;  /*执行时间*/
}CPSS_COM_DRV_INIT_INFO_T ;

/*rarp请求信息结构*/
typedef struct tagCPSS_COM_RARP_REQ_INFO
{
    UINT32 ulRarpReqFlag ;
    UINT32 ulRarpReqNum ;
    CPSS_COM_PHY_ADDR_T stRarpPhyAddr ;
    CPSS_TIME_T stTime ;  /*执行时间*/    
}CPSS_COM_RARP_REQ_INFO_T ;

/*rarp响应信息结构*/
typedef struct tagCPSS_COM_RARP_RSP_INFO
{
    UINT32 ulRarpRspFlag ;
    UINT32 ulRarpRspRecvNum ;
    UINT32 ulRarpRspResult ;
    UINT32 ulMateFlag ;
    CPSS_COM_LOGIC_ADDR_T stRarpLogAddr ;
    CPSS_COM_PHY_ADDR_T stMatePhyAddr ;
    CPSS_COM_PHY_ADDR_T stGcpaPhyAddrM ;
    CPSS_TIME_T stTime ;  /*执行时间*/    
}CPSS_COM_RARP_RSP_INFO_T ;

/*power on信息结构*/
typedef struct tagCPSS_COM_POWER_ON_INFO
{
    UINT32 ulPowerOnFlag ;
    CPSS_COM_PHY_ADDR_T stPowerOnPhyAddr ;
    CPSS_TIME_T stTime ;  /*执行时间*/    
}CPSS_COM_POWER_ON_INFO_T ;

/*smss激活消息信息*/
typedef struct tagCPSS_COM_SMSS_ACT_INFO
{
    UINT32 ulSmssActFlag ;
    UINT32 ulAddrFlag ;
    CPSS_TIME_T stTime ;  /*执行时间*/    
}CPSS_COM_SMSS_ACT_INFO_T ;

/*smss激活响应信息*/
typedef struct tagCPSS_COM_SMSS_ACT_RSP_INFO
{
    UINT32 ulSmssActRspFlag ;
    UINT32 ulSmssActRspResult ;
    CPSS_TIME_T stTime ;  /*执行时间*/    
}CPSS_COM_SMSS_ACT_RSP_INFO_T ;

/*初始化流程统计结构定义*/
typedef struct tagCPSS_COM_INIT_STAT
{
    UINT32 ulRecvInitMsgFlag ;  /*是否接收到INIT消息标志*/
    CPSS_COM_ADDR_INIT_INFO_T stAddrInitInfo ;  /*地址初始化信息*/
    CPSS_COM_DRV_INIT_INFO_T stDrvInitInfo ;    /*驱动初始化信息*/
    UINT32 ulGcpaLinkSetup ;   /*去GCPA链路建立标志*/
    CPSS_COM_RARP_REQ_INFO_T stRarpReqInfo ;    /*rarp信息*/
    CPSS_COM_RARP_RSP_INFO_T stRarpRspInfo ;    /*rarp rsp信息*/
    CPSS_COM_POWER_ON_INFO_T stPowerOnInfo ;    /*poweron消息信息*/
    CPSS_COM_SMSS_ACT_INFO_T stSmssActInfo ;    /*smss激活信息*/
    CPSS_COM_SMSS_ACT_RSP_INFO_T stSmssActRspInfo ;  /*smss激活响应信息*/
}CPSS_COM_INIT_STAT_T ;

/*一条ARP记录*/
typedef struct tagCPSS_COM_ARP_ITEM
{
    UINT32 ulAddrFlag ;
    CPSS_COM_PHY_ADDR_T stPhyAddr ;
    CPSS_COM_LOGIC_ADDR_T stLogAddr ;
}CPSS_COM_ARP_ITEM_T ;

/*arp统计结构定义*/
typedef struct tagCPSS_COM_ARP_STAT
{
    UINT32 ulArpSendNum ;    /*发送的ARP请求数目*/
    UINT32 ulCurrResolveItem ;      /*当前记录的项指针*/
    CPSS_COM_ARP_ITEM_T astArpResolveItems[CPSS_COM_ARP_ITEM_NUM_MAX] ;
    UINT32 ulCurrLogAddr ;
    CPSS_COM_LOGIC_ADDR_T astArpLogAddr[CPSS_COM_ARP_ITEM_NUM_MAX] ;
    UINT32 aulAddrFlag[CPSS_COM_ARP_ITEM_NUM_MAX] ;    
}CPSS_COM_ARP_STAT_T ;

/*链路心跳结构*/
typedef struct tagCPSS_COM_HEARTBEAT_NODE_T
{
    CPSS_COM_PHY_ADDR_T stDstPhyAddr ;
    UINT16 usLinkState ;
    UINT16 usReserve ;
    struct tagCPSS_COM_HEARTBEAT_NODE_T* pstNextNode ;
}CPSS_COM_HEARTBEAT_NODE_T ;

/*链路心跳统计结构*/
typedef struct tagCPSS_COM_HEARTBEAT_STAT_T
{
    UINT32 ulHeartBeatTimerFlag ;
    UINT32 ulHeartBeatTimerNum ;
}CPSS_COM_HEARTBEAT_STAT_T ;

typedef struct tagCPSS_CTRLP_COMM_TEST_REQ_T
{
    CPSS_COM_PHY_ADDR_T stDstPhyAddr;   /* 目标物理地址 */
    UINT32 ulTestLen;                   /* 测试字符串长度 */
    UINT32 ulTestNum;                   /* 测试次数 */
}CPSS_CTRLP_COMM_TEST_REQ_T ;

/*链路连通测试的管理结构*/
typedef struct tagCPSS_COM_DIAG_COMM_TEST_MAN
{
    UINT32 ulUsedFlag ;
    UINT32 ulSeqId ;
    UINT32 ulProcId ;
    UINT32 ulTestId ;
    UINT32 ulSerialNo ;
    UINT32 ulAddrFlag ;
    CPSS_COM_LOGIC_ADDR_T stLogicAddr ;
    CPSS_CTRLP_COMM_TEST_REQ_T stOamsTestReq ;
}CPSS_COM_DIAG_COMM_TEST_MAN_T ;

/*得到单板逻辑地址/主备状态的请求消息*/
typedef struct tagCPSS_COM_GET_LOGIC_ADDR_REQ
{
    CPSS_COM_PHY_ADDR_T stPhyAddr ;
    CPSS_COM_PID_T  stPid ;
}CPSS_COM_GET_LOGIC_ADDR_REQ_T ;

/*得到单板逻辑地址/主备状态的响应消息*/
typedef struct tagCPSS_COM_GET_LOGIC_ADDR_RSP
{
    CPSS_COM_LOGIC_ADDR_T stLogAddr ;
    UINT32 ulAddrFlag ;
    UINT32 ulErrCode ;
}CPSS_COM_GET_LOGIC_ADDR_RSP_T ;

/*链路连通测试的请求消息*/
typedef struct tagCPSS_COM_COMM_TEST_REQ
{
    UINT32 ulSeqId ;
    UINT32 ulSerialId ;
    UINT32 ulTestId ;
    UINT32 ulMsgLen ;
    UINT32 ulTestNum ;
    UINT32 ulOamsProc ;
    CPSS_COM_PID_T stSrcPid ;
}CPSS_COM_COMM_TEST_REQ_T ;

/*链路连通测试的响应消息*/
typedef struct tagCPSS_COM_COMM_TEST_RSP
{
    UINT32 ulSeqId ;
    UINT32 ulSerialId ;
    UINT32 ulTestId ;
    UINT32 ulMsgLen ;
    UINT32 ulTestNum ;
    UINT32 ulOamsProc ;
    UINT32 ulErrCode ;
}CPSS_COM_COMM_TEST_RSP_T ;


typedef struct tagCPSS_COM_DSP_CPU_USAGE
{
    UINT32 ulDspNum ;
    UINT32 ulDspCpuUsage ;
}CPSS_COM_DSP_CPU_USAGE_T ;

/******************************** 全局变量声明 ********************************/

extern CPSS_COM_LOGIC_ADDR_T g_stComLogicAddrZero; /* 空逻辑地址 */
extern CPSS_COM_PHY_ADDR_T   g_stComPhyAddrZero;   /* 空物理地址 */

extern CPSS_COM_DATA_T *g_pstComData;  /* 通信常量地址 */

#ifndef CPSS_DSP_CPU
/*通信初始化流程统计变量*/
extern CPSS_COM_INIT_STAT_T g_stCpssComInitStat;

#endif

/********************************** 函数原形声明 ******************************/

/* IP地址转换成物理地址 */
extern INT32 cpss_com_ip2phy
(
    UINT32 ulIpAddr,
    CPSS_COM_PHY_ADDR_T *pstPhyAddr
);

/* 纤程标识字节序转换 */
extern VOID cpss_com_pid_ntoh
(
    CPSS_COM_PID_T *pstPid
);
extern VOID cpss_com_pid_hton
(
    CPSS_COM_PID_T *pstPid
);

/* 地址转换 */
extern UINT32 cpss_com_phy2index(CPSS_COM_PHY_ADDR_T stPhyAddr);
extern UINT32 cpss_com_phy2index(CPSS_COM_PHY_ADDR_T stPhyAddr);
extern INT32 cpss_com_phy2ip(CPSS_COM_PHY_ADDR_T stPhyAddr, UINT32 *pulIpAddr,BOOL bMateFlag);

/* 消息头部字节序转化函数 */
extern VOID cpss_com_ipc_hdr_ntoh(IPC_MSG_HDR_T *pHdr);
extern VOID cpss_com_ipc_hdr_hton(IPC_MSG_HDR_T *pHdr);

/* 初始化函数 */
extern INT32 cpss_com_route_table_init(VOID);
extern INT32 cpss_com_link_table_init(VOID);
extern INT32 cpss_com_init(VOID);

/* 链路操作函数 */
extern INT32 cpss_com_link_create(CPSS_COM_PHY_ADDR_T stPhyAddr,UINT32 ulDrvType);
extern INT32 cpss_com_link_delete(CPSS_COM_PHY_ADDR_T stPhyAddr);
extern CPSS_COM_LINK_T* cpss_com_link_find(CPSS_COM_PHY_ADDR_T stPhyAddr);
extern CPSS_COM_LINK_T* cpss_com_link_find_real(CPSS_COM_PHY_ADDR_T stPhyAddr);
extern CPSS_COM_LINK_T* cpss_com_link_byid_find(UINT32 ulLinkId);
extern INT32 cpss_com_link_status_set
(
    CPSS_COM_PHY_ADDR_T stPhyAddr,
    UINT16 usStatus
);

/* 路由操作函数 */
extern INT32 cpss_com_route_create
(
    CPSS_COM_LOGIC_ADDR_T stLogicAddr, 
    UINT32 ulAddrFlag,
    CPSS_COM_PHY_ADDR_T stPhyAddr,
    UINT32 ulDrvType
);
extern INT32 cpss_com_route_delete
(
    CPSS_COM_LOGIC_ADDR_T stLogicAddr,
    UINT32 ulAddrFlag
);

extern CPSS_COM_ROUTE_T* cpss_com_route_find(CPSS_COM_LOGIC_ADDR_T stLogiAddr);
extern CPSS_COM_ROUTE_T* cpss_com_route_find_real(CPSS_COM_LOGIC_ADDR_T stLogiAddr);

/* 构造消息包 */
extern CPSS_COM_TRUST_HEAD_T* cpss_com_packet_setup
(
    CPSS_COM_MSG_HEAD_T *pstMsg,
    CPSS_COM_LINK_T *pstLink
);
extern CPSS_COM_TRUST_HEAD_T* cpss_com_packet_setup_local
(
CPSS_COM_MSG_HEAD_T *pstMsg
);

/* CPU间消息发送 */
extern INT32 cpss_com_remote_msg_send
(
    CPSS_COM_MSG_HEAD_T *pstMsg,
    CPSS_COM_LINK_T *pstLink
);

/* 本地消息处理 */
extern INT32 cpss_com_local_msg_recv(CPSS_COM_TRUST_HEAD_T  *pstTrustHdr);

/* 反向地址解析 */
extern INT32 cpss_com_rarp_req_send(VOID); 
extern INT32 cpss_com_rarp_req_recv(CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern INT32 cpss_com_rarp_rsp_recv(CPSS_COM_MSG_HEAD_T *pstMsgHead);

/* 地址解析 */
extern INT32 cpss_com_arp_req_send(CPSS_COM_ARP_REQ_MSG_T *pstArpReq);
extern INT32 cpss_com_arp_req_recv(CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern INT32 cpss_com_arp_rsp_recv(CPSS_COM_MSG_HEAD_T *pstMsgHead);

extern INT32 cpss_com_app_arp_req_recv
(
    CPSS_COM_MSG_HEAD_T *pstMsgHead
);

extern INT32 cpss_com_app_arp_req_send
(
    CPSS_COM_LOGIC_ADDR_T stLogicAddr,
    UINT32                ulAddrFlag
);

/* 链路消息函数 */
extern INT32 cpss_com_link_req_send(CPSS_COM_PHY_ADDR_T stDstPhyAddr,UINT32 ulMateFlag);
extern INT32 cpss_com_link_req_recv(CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern INT32 cpss_com_link_rsp_recv(CPSS_COM_MSG_HEAD_T *pstMsgHead);

/* 主备通道建立函数 */
extern INT32 cpss_com_mate_channel_req_send(VOID);
extern INT32 cpss_com_mate_channel_req_recv(CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern INT32 cpss_com_mate_channel_rsp_recv(CPSS_COM_MSG_HEAD_T *pstMsgHead);


/* 链路心跳消息 */
extern INT32 cpss_com_link_heartbeat_send();
extern INT32 cpss_com_link_heartbeat_recv(CPSS_COM_MSG_HEAD_T *pstMsgHead);

/* 向SMSS发送上电消息 */
extern INT32 cpss_com_power_on_send(VOID);

/* 显示链路 */
extern VOID cpss_com_link_show(VOID);
extern VOID cpss_com_route_show(VOID);

/* 判断目标纤程中的地址是否是本地 */
BOOL cpss_com_is_local_address(const CPSS_COM_PID_T *pstDstPid);

INT32 cpss_com_board_switch_deal
(
    CPSS_COM_LOGIC_ADDR_T stLogAddr,
    CPSS_COM_PHY_ADDR_T   stPhyAddr,
    UINT32 ulAddrFlag
);

INT32 cpss_com_route_switch
(
 CPSS_COM_ROUTE_T* pstRoute,
 CPSS_COM_LOGIC_ADDR_T stLogAddr
);

/*arp统计函数*/
extern VOID cpss_com_arp_item_add
(
 CPSS_COM_PHY_ADDR_T *pstPhyAddr,
 CPSS_COM_LOGIC_ADDR_T* pstLogAddr,
 UINT32 ulAddrFlag,
 UINT32 ulAddFlag
);

extern VOID cpss_com_heartbeat_node_status_set
(
 CPSS_COM_PHY_ADDR_T stPhyAddr,
 UINT16 usStatus
) ;

extern INT32 cpss_com_heartbeat_link_add
(
 CPSS_COM_PHY_ADDR_T stDstPhyAddr
) ;

extern INT32 cpss_com_diag_comm_test_req_deal
(
 UINT8* pucRecvBuf,
 UINT32 ulSeqId,
 UINT32 ulOamsProc,
 UINT32 ulTestId
) ;

extern INT32 cpss_com_get_logic_addr_tm_recv() ;

extern INT32 cpss_com_logic_addr_get_rsp_deal(UINT8* pucRecvBuf) ;

extern INT32 cpss_com_logic_addr_get_req_deal(UINT8* pucRecvBuf) ;

extern INT32 cpss_com_comm_test_tm_msg_recv() ;

extern INT32 cpss_com_comm_test_req_deal(UINT8 *pucRecvBuf) ;

extern INT32 cpss_com_comm_test_rsp_deal(UINT8 *pucRecvBuf) ;

extern INT32 cpss_com_route_arp_flag_get(CPSS_COM_LOGIC_ADDR_T stLogicAddr,UINT32 ulAddrFlag);

extern INT32 cpss_com_swtich_route_deal(CPSS_COM_PHY_ADDR_T stMPhyAddr,CPSS_COM_PHY_ADDR_T stSPhyAddr,UINT32 ulDealFlag) ;

extern INT32 cpss_com_gcpa_route_deal(UINT32 ulDealFlag) ;
/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_COM_LINK_H */
/******************************** 头文件结束 *********************************/

