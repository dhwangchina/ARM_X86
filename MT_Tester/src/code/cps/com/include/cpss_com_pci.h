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
#ifndef CPSS_COM_PCI_H
#define CPSS_COM_PCI_H

/********************************* 头文件包含 ********************************/
#include "cpss_type.h"

/******************************** 宏和常量定义 *******************************/
#ifdef CPSS_HOST_CPU
#define CPSS_COM_SLAVE_CPU_NUM       20
#else
#define CPSS_COM_SLAVE_CPU_NUM       1
#endif
#define CPSS_COM_PCI_RX_EBD_NUM      256
#define CPSS_COM_PCI_TX_EBD_NUM      256
#define CPSS_COM_PCI_RXTX_BUF_SIZE   256
#define CPSS_COM_PCI_USR_REG_SIZE    4096
#define CPSS_COM_PCI_BUF_NUM_MIN     4

#define CPSS_COM_PCI_ISR_COUNT_MIN   10

#define CPSS_COM_PCI_RECV_BUFSIZE   64*1024

#define CPSS_COM_PCI_TX_READY_MASK  0x00000000
#define CPSS_COM_PCI_RX_READY_MASK  0x80000000

#define CPSS_PCI_HAS_DATA 0x2
#define CPSS_PCI_NOT_HAS_DATA 0x3

#define CPSS_PCI_DMA_READ_MIN  0x10000
#define CPSS_PCI_DMS_WRITE_MIN 0x10000

#define CPSS_PCI_RESET_VALUE  0xffffffff

#define CPSS_PCI_HDR_EBD_LEN  (sizeof(CPSS_COM_EBD_POOL_HEAD_T)+sizeof(CPSS_COM_EBD_T)*CPSS_COM_PCI_RX_EBD_NUM+ \
                               sizeof(CPSS_COM_EBD_T)*CPSS_COM_PCI_TX_EBD_NUM)

#define CPSS_COM_PCI_MEM_WRITE_PERMIT      0x1
#define CPSS_COM_PCI_MEM_WRITE_NOT_PERMIT  0x2

#ifdef CPSS_HOST_CPU

#define CPSS_COM_PCI_POOL_HDR_TO_RX_EBD(addr) \
    ((UINT8*)(addr)+sizeof(CPSS_COM_EBD_POOL_HEAD_T))

#define CPSS_COM_PCI_POOL_HDR_TO_TX_EBD(addr) \
    (CPSS_COM_PCI_POOL_HDR_TO_RX_EBD(addr)+(sizeof(CPSS_COM_EBD_T)*CPSS_COM_PCI_RX_EBD_NUM))

#define CPSS_COM_PCI_POOL_HDR_TO_RXBUF(addr) \
    (CPSS_COM_PCI_POOL_HDR_TO_TX_EBD(addr)+(sizeof(CPSS_COM_EBD_T)*CPSS_COM_PCI_TX_EBD_NUM))

#define CPSS_COM_PCI_POOL_HDR_TO_TXBUF(addr,rxNum) \
    (CPSS_COM_PCI_POOL_HDR_TO_RXBUF(addr)+(CPSS_COM_PCI_RXTX_BUF_SIZE*rxNum))

#define CPSS_COM_PCI_POOL_HDR_TO_REGBUF(addr,rxNum,txNum) \
    (CPSS_COM_PCI_POOL_HDR_TO_TXBUF(addr,rxNum)+(CPSS_COM_PCI_RXTX_BUF_SIZE*txNum))

#else

#define CPSS_COM_PCI_POOL_HDR_TO_TX_EBD(addr) \
((UINT8*)(addr)+sizeof(CPSS_COM_EBD_POOL_HEAD_T))

#define CPSS_COM_PCI_POOL_HDR_TO_RX_EBD(addr) \
(CPSS_COM_PCI_POOL_HDR_TO_TX_EBD(addr)+(sizeof(CPSS_COM_EBD_T)*CPSS_COM_PCI_RX_EBD_NUM))

#define CPSS_COM_PCI_POOL_HDR_TO_TXBUF(addr) \
(CPSS_COM_PCI_POOL_HDR_TO_RX_EBD(addr)+(sizeof(CPSS_COM_EBD_T)*CPSS_COM_PCI_TX_EBD_NUM))

#define CPSS_COM_PCI_POOL_HDR_TO_RXBUF(addr,txNum) \
(CPSS_COM_PCI_POOL_HDR_TO_TXBUF(addr)+(CPSS_COM_PCI_RXTX_BUF_SIZE*txNum))

#define CPSS_COM_PCI_POOL_HDR_TO_REGBUF(addr,rxNum,txNum) \
(CPSS_COM_PCI_POOL_HDR_TO_RXBUF(addr,txNum)+(CPSS_COM_PCI_RXTX_BUF_SIZE*rxNum))

#endif


#define cpss_com_drv_pci_if_full_EX(ulTxBufWriteIndex, ulTxBufReadIndex, ulBufNumMax) \
    (CPSS_MOD(((ulTxBufWriteIndex)+1),(ulBufNumMax)) == (ulTxBufReadIndex)) 

#define CPSS_COM_PCI_TX_READY(status) \
    ((status)|CPSS_COM_PCI_TX_READY_MASK?FALSE:TRUE)

#define CPSS_COM_PCI_RX_READY(status) \
    ((status)&CPSS_COM_PCI_RX_READY_MASK?TRUE:FALSE)
#define CPSS_DSP_PCI_TEMP_ADDR     (0x83f00010+1024)
#define CPSS_DSP_PCI_TEMP_ADDR_LEN 1024*512

#define CPSS_PCI_EBD_STATUS_OFFSET    0
#define CPSS_PCI_EBD_LENGTH_OFFSET    4
#define CPSS_PCI_EBD_BUF_INDEX_OFFSET 8
#define CPSS_PCI_WIN_BASE_OFFSET  8
#define CPSS_PCI_WIN_SIZE_OFFSET  12

#ifdef CPSS_HOST_CPU
#define CPSS_PCI_RXEBD_READ_INDEX_OFFSET  20
#define CPSS_PCI_RXBUF_READ_INDEX_OFFSET  28
#define CPSS_PCI_TXEBD_WRITE_INDEX_OFFSET 40
#define CPSS_PCI_TXBUF_WRITE_INDEX_OFFSET 48
#define CPSS_PCI_TXBUF_READ_INDEX_OFFSET  44
#else
#define CPSS_PCI_RXEBD_READ_INDEX_OFFSET  36
#define CPSS_PCI_RXBUF_READ_INDEX_OFFSET  44
#define CPSS_PCI_TXEBD_WRITE_INDEX_OFFSET 24
#define CPSS_PCI_TXBUF_WRITE_INDEX_OFFSET 32
#define CPSS_PCI_TXBUF_READ_INDEX_OFFSET  28
#endif
/******************************** 类型定义 ***********************************/

/*CPSS_COM_DRV_EBD_T结构*/
typedef struct tagCPSS_COM_EBD_T
{
    UINT32 usStatus;      /*EBD状态字*/    
    UINT32 usLength;      /*数据包的长度(保证发送数据四字节对齐)*/
    UINT32 ulBufIndex;    /**/
}CPSS_COM_EBD_T;

/*共享PCI内存的存储区*/
#ifdef CPSS_HOST_CPU
typedef struct tagCPSS_COM_EBD_POOL_HEAD
{
/*********************************************
*  EBD池基本信息，共32字节
*/
    UINT8  aucFieldName[8];		/*EBD缓冲池描述字符串，利于直观调试*/
    UINT32 ulWinBase;	        /*本EBD缓冲池基地址*/
    UINT32 ulWinSize;           /*本EBD缓冲池大小*/
        
/*********************************************
*  RxEBD相关的数据成员，共48字节
*/
    UINT32 ulRxEbdReadIndex;
    UINT32 ulRxEbdWriteIndex;
    UINT32 ulRxBufReadIndex;
    UINT32 ulRxBufWriteIndex;
   
/*********************************************
*TxEBD相关的数据成员，共48字节
*/    
    UINT32 ulTxEbdReadIndex;
    UINT32 ulTxEbdWriteIndex;
    UINT32 ulTxBufReadIndex;
    UINT32 ulTxBufWriteIndex;
    
/*********************************************
*REGBUF相关的数据成员，共48字节
*/
    UINT32 ulRegBufStart;        /*用户主次缓冲区的起始大小*/
    UINT32 ulRegBufSize;         /*用户注册缓冲区大小*/
 
}CPSS_COM_EBD_POOL_HEAD_T;
#else
typedef struct tagCPSS_COM_EBD_POOL_HEAD
{
/*********************************************
*  EBD池基本信息，共32字节
*/
    UINT8  aucFieldName[8];		/*EBD缓冲池描述字符串，利于直观调试*/
    UINT32 ulWinBase;	        /*本EBD缓冲池基地址*/
    UINT32 ulWinSize;           /*本EBD缓冲池大小*/
        
/*********************************************
*  TxEBD相关的数据成员，共48字节
*/
    UINT32 ulTxEbdReadIndex;
    UINT32 ulTxEbdWriteIndex;
    UINT32 ulTxBufReadIndex;
    UINT32 ulTxBufWriteIndex;   
   
/*********************************************
*RxEBD相关的数据成员，共48字节
*/    
    UINT32 ulRxEbdReadIndex;
    UINT32 ulRxEbdWriteIndex;
    UINT32 ulRxBufReadIndex;
    UINT32 ulRxBufWriteIndex;    
    
/*********************************************
*REGBUF相关的数据成员，共48字节
*/
    UINT32 ulRegBufStart;        /*用户主次缓冲区的起始大小*/
    UINT32 ulRegBufSize;         /*用户注册缓冲区大小*/
 
}CPSS_COM_EBD_POOL_HEAD_T;
#endif

/**/
typedef struct tagCPSS_COM_PCI_DRV_LINK
{
    UINT32 ulRxEbdStart;      /*RxEBD区已使用BUF的起始地址*/
    UINT32 ulTxEbdStart;      /*TxEBD区已使用BUF的起始地址*/
    
    UINT32 ulRxBufBase;       /*RxEBD的基地址*/
    UINT32 ulRxBufNum;        /*RxEBD的个数*/

    UINT32 ulTxBufBase;       /*TxEBD的基地址*/
    UINT32 ulTxBufNum;        /*TxEBD的个数*/

    UINT32 ulRegBufStart;
    UINT32 ulRegBufSize;
    
}CPSS_COM_PCI_DRV_LINK_T;

/*PCI驱动链路表*/
typedef struct tagCPSS_COM_PCI_LINK_TABLE
{
    CPSS_COM_PCI_DRV_LINK_T astPciLink[CPSS_COM_SLAVE_CPU_NUM] ;
    UINT32 ulSlaveCpuNum ;
    UINT32 aulCpuNo[CPSS_COM_SLAVE_CPU_NUM] ;
    UINT32 aulMemPoolAddr[CPSS_COM_SLAVE_CPU_NUM] ;
    UINT32 aulMemPoolLen[CPSS_COM_SLAVE_CPU_NUM] ;
}CPSS_COM_PCI_LINK_TABLE_T;

/*PCI用户寄存区的管理结构*/
typedef struct tagCPSS_PCI_USER_REG_MANAGE
{
    UINT32 ulStat ;
}CPSS_PCI_USER_REG_MANAGE_T;


/******************************** 全局变量声明 *******************************/
extern CPSS_COM_PCI_LINK_TABLE_T g_stPciLinkTable;

/******************************** 外部函数原形声明 ***************************/
extern INT32 cpss_com_pci_read_len_get
(
 UINT32 ulLinkId,
 UINT32 *pulDataLen
);

extern INT32 cpss_com_pci_read
(
 UINT8*  pucDataAddr,
 UINT32  ulLinkId
);

extern INT32 cpss_com_pci_write
(
 UINT32  ulLinkId,
 UINT8   *pucBuf,
 UINT32  ulDataLen
);

extern INT32 cpss_pci_mem_init();

extern INT32 cpss_pci_mem_init_by_linkid
(
    UINT32 ulLinkId,
    UINT32 ulWriteFlag
);

extern VOID cpss_com_drv_info_init();

extern INT32 cpss_com_pci_special_write
(
    UINT32 ulLinkId,
    UINT32 ulOffSet,
    UINT8* pucBuf,
    UINT32 ulLen
);

extern INT32 cpss_com_pci_special_read
(
    UINT32 ulLinkId,
    UINT32 ulOffSet,
    UINT8 *pucBuf,
    UINT32 ulLen
);

extern VOID cpss_com_pci_special_zero(UINT32 ulLinkId);
/*
extern STATUS  drv_pci_info_get
(
    UINT32 *pulNum,
    DRV_PCI_INFO_T*pstInfo
);

extern STATUS  drv_pcibus_notify_reg
(
    VOID_FUNC_PTR pfPciNotifyFunc
);

extern STATUS drv_pci_notify_send
(
    UINT32 ulCpuId, 
    UINT8  ucReqId, 
    UINT32 ulMsgContext
);
*/
/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_COM_PCI_H */
/******************************** 头文件结束 *********************************/











































