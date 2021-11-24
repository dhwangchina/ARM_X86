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
#include "cpss_vos_task.h"
#include "cpss_com_common.h"
#include "cpss_com_link.h"
#include "cpss_com_app.h"
#include "cpss_com_drv.h"
#include "cpss_com_trace.h"
#include "cpss_com_pci.h"
#include "cpss_err.h"
#include <arpa/inet.h>

/******************************* 局部宏定义 **********************************/

/******************************* 局部常数和类型定义 **************************/

/* 驱动参数 */
#ifdef CPSS_VOS_WINDOWS
    static CPSS_COM_DRV_T g_stCpssComDrvTbl[CPSS_COM_MAX_DRV_NUM]=
    {
        #ifdef CPSS_UDP_INCLUDE
        {
            CPSS_COM_DRV_UDP, 0, {AF_INET,CPSS_COM_UDP_SERVER_PORT},
            NULL,cpss_com_drv_udp_write,NULL,
        },
        #endif
        #ifdef CPSS_TCP_INCLUDE
        {
            CPSS_COM_DRV_TCP_SERVER, 0, {AF_INET,CPSS_COM_TCP_SERVER_PORT},
            NULL,cpss_com_drv_tcp_write,NULL,
        },
        {
            CPSS_COM_DRV_DC_SERVER, 0, {AF_INET,CPSS_COM_DC_SERVER_PORT},
                NULL,cpss_com_drv_tcp_write,NULL,
        },
        #endif
#ifdef CPSS_PCI_INCLUDE
        {
            CPSS_COM_DRV_PCI, 0, {0},
            NULL,cpss_com_drv_pci_write,NULL,
        },
#endif
    };


#elif defined (CPSS_VOS_LINUX)
    static CPSS_COM_DRV_T g_stCpssComDrvTbl[CPSS_COM_MAX_DRV_NUM]=
    {
        #ifdef CPSS_UDP_INCLUDE
        {
            CPSS_COM_DRV_UDP, 0, {AF_INET,CPSS_COM_UDP_SERVER_PORT},
            NULL,cpss_com_drv_udp_write,NULL,
        },
        #endif
        #ifdef CPSS_TCP_INCLUDE
        {
            CPSS_COM_DRV_TCP_SERVER, 0, {AF_INET,CPSS_COM_TCP_SERVER_PORT},
            NULL,cpss_com_drv_tcp_write,NULL,
        },
        {
            CPSS_COM_DRV_DC_SERVER, 0, {AF_INET,CPSS_COM_DC_SERVER_PORT},
                NULL,cpss_com_drv_tcp_write,NULL,
        },
        #endif
#ifdef CPSS_PCI_INCLUDE
        {
            CPSS_COM_DRV_PCI, 0, {0},
            NULL,cpss_com_drv_pci_write,NULL,
        },
#endif
    };
#elif defined (CPSS_VOS_VXWORKS)
    static CPSS_COM_DRV_T g_stCpssComDrvTbl[CPSS_COM_MAX_DRV_NUM]=
    {
        #ifdef CPSS_UDP_INCLUDE
        {
            CPSS_COM_DRV_UDP, 0, {sizeof(CPSS_COM_SOCK_ADDR_T),AF_INET,CPSS_COM_UDP_SERVER_PORT},
            NULL,cpss_com_drv_udp_write,NULL,
        },
        #endif
        #ifdef CPSS_TCP_INCLUDE
        {
            CPSS_COM_DRV_TCP_SERVER, 0, {sizeof(CPSS_COM_SOCK_ADDR_T),AF_INET,CPSS_COM_TCP_SERVER_PORT},
            NULL,cpss_com_drv_tcp_write,NULL,
        },
        {
            CPSS_COM_DRV_DC_SERVER, 0, {sizeof(CPSS_COM_SOCK_ADDR_T),AF_INET,CPSS_COM_DC_SERVER_PORT},
            NULL,cpss_com_drv_tcp_write,NULL,
        },
        #endif
#ifdef CPSS_PCI_INCLUDE
        {
            CPSS_COM_DRV_PCI, 0, {0},
            NULL,cpss_com_drv_pci_write,NULL,
        },
#endif
    };
#else 
    static CPSS_COM_DRV_T g_stCpssComDrvTbl[CPSS_COM_MAX_DRV_NUM]=
    {
        #ifdef CPSS_UDP_INCLUDE
        {
            CPSS_COM_DRV_UDP, 0, {sizeof(CPSS_COM_SOCK_ADDR_T),AF_INET,CPSS_COM_UDP_SERVER_PORT},
            NULL,cpss_com_drv_udp_write,NULL,
        },
        #endif
        #ifdef CPSS_TCP_INCLUDE
        {
            CPSS_COM_DRV_TCP_SERVER, 0, {sizeof(CPSS_COM_SOCK_ADDR_T),AF_INET,CPSS_COM_TCP_SERVER_PORT},
            NULL,cpss_com_drv_tcp_write,NULL,
        },
        {
            CPSS_COM_DRV_TCP_SERVER, 0, {sizeof(CPSS_COM_SOCK_ADDR_T),AF_INET,CPSS_COM_DC_SERVER_PORT},
            NULL,cpss_com_drv_tcp_write,NULL,
        },
        #endif
#ifdef CPSS_PCI_INCLUDE
        {
            CPSS_COM_DRV_PCI, 0, {0},
            NULL,cpss_com_drv_pci_write,NULL,
        },
#endif

    };
#endif

/* 驱动个数 */
UINT32  g_ulCpssComDrvNum = NUM_ENTS(g_stCpssComDrvTbl);

/* tcp,逻辑地址映射表 */
CPSS_COM_TCP_LINK_MAN_T g_stCpssTcpMan ;

/* UDP 收到的包的总数 */
UINT32 g_ulUdpRecvNum = 0;

/*PCI表结构*/
#ifdef CPSS_PCI_INCLUDE
CPSS_COM_PCI_TABLE_T g_stCpssComPciTbl={0};
#endif

UINT32 g_ulCpssComDrvTcpMutex=CPSS_VOS_MUTEXD_INVALID;

/*tcp_lind_send函数使用的发送内存的指针，动态申请*/
#ifdef CPSS_TCP_INCLUDE
static STRING g_szCpssTcpSendBufPtr = NULL ;        
#endif

/*UDP统计使用的全局变量*/
CPSS_UDP_DRV_STAT_T *gp_stCpssComUdpStat;
#ifdef CPSS_DSP_CPU
DRV_PCI_INFO_T g_stCpssDspPciInfo = {0} ;
#endif

CPSS_TCP_CLOSE_STAT_T g_stTcpCloseStat = {0} ;

/*TCP通过交换板接入错误统计*/
CPSS_COM_TCP_ERR_STAT_T g_stCpssTcpErrStat = {0} ;

/*TCP的listen过程统计*/
CPSS_COM_TCP_LISTEN_STAT_T g_stTcpListenStat = {0} ;

/*连接LDT的UDP连接socket*/
UINT32 g_ulCpssLdtSocket = 0 ;
#ifndef CPSS_DSP_CPU
fd_set g_stCpssReadFds;
#endif

UINT32 g_ulCpssTcpListenFlag = 0 ;  /*TCP监听标志*/
UINT32 g_ulCpssDcListenFlag  = 0 ;  /*网元直连监听标志*/

/*使用交换板进行TCP通信*/
UINT32 g_ulCpssComTcpListenTaskPList[8] ;
UINT32 g_ulCpssComDcListenTaskPList[16] ;
UINT32 g_ulCpssComTcpRecvTaskPList[1];
extern g_MateSockId;
extern g_MateBrdIpAddr;

/******************************* 局部函数原型声明 ****************************/
extern VOID cpss_kw_set_task_stop() ;
extern VOID cpss_kw_set_task_run() ;
extern VOID cpss_tm_sntp_pci_read(UINT8 ucReqId, UINT32 ulMsgContext);
extern INT32 cpss_com_slave_pci_drv_deliver(UINT32 ulDrvType,UINT32 ulLinkId,UINT8 *pstBuf,UINT32 ulReadSize);
/********************************* 函数定义 ***********************************/
#ifdef CPSS_DSP_CPU
STATUS drv_pci_info_get
(
 UINT32 *pulNum, 
 DRV_PCI_INFO_T *pstInfo 
)
{    
    *pulNum = 1 ;

    pstInfo->ulCpuId = g_stCpssDspPciInfo.ulCpuId ;
    pstInfo->ulLen   = g_stCpssDspPciInfo.ulLen ;
    pstInfo->ulRamWinAttr = g_stCpssDspPciInfo.ulRamWinAttr ;
    pstInfo->ulStartAddr  = g_stCpssDspPciInfo.ulStartAddr ;

    return CPSS_OK ;
}
#endif

/*******************************************************************************
* 函数名称: cpss_com_drv_init                            
* 功    能: 驱动初始化
* 相关文档: 
* 函数类型:    *                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明:
*******************************************************************************/
INT32  cpss_com_drv_init(VOID)
{ 
    INT32 lRet;

    /*增加初始化流程统计*/
#ifndef CPSS_DSP_CPU
    g_stCpssComInitStat.stDrvInitInfo.ulDrvInitFlag = 1 ;
#endif

#ifdef CPSS_UDP_INCLUDE
#ifndef SWP_FNBLK_BRDTYPE_ABOX
    /* 初始化UDP驱动的接收统计 */
    gp_stCpssComUdpStat = cpss_mem_malloc(
          CPSS_UDP_STAT_LOGSHELF_MAX*CPSS_UDP_STAT_SLOT_MAX*sizeof(CPSS_UDP_DRV_STAT_T));
    if (NULL == gp_stCpssComUdpStat)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,"gp_stCpssComUdpStat initial failed!!\n");
        return CPSS_ERROR;
    }
    cpss_mem_memset((void*)gp_stCpssComUdpStat, 0, 
        CPSS_UDP_STAT_LOGSHELF_MAX*CPSS_UDP_STAT_SLOT_MAX*sizeof(CPSS_UDP_DRV_STAT_T));

    /* udp 驱动初始化 */
    lRet = cpss_com_socket_init(CPSS_COM_DRV_UDP);

    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
                   "udp initial failed!!");
        return CPSS_ERROR;
    }
    cpss_com_drv_read_reg(CPSS_COM_DRV_UDP,cpss_com_drv_udp_read);
#endif
#endif
#ifdef CPSS_FUNBRD_MC
    /*如果是全局板则初始化去LDT的特殊的socket*/
    if(CPSS_OK != cpss_com_ldt_socket_init())
    {
        return CPSS_ERROR ;
    }
#endif

#ifdef CPSS_TCP_INCLUDE

    /* tcp 驱动初始化 */
    lRet = cpss_com_socket_init(CPSS_COM_DRV_TCP_SERVER);
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
                   "tcp initial failed!!");
        return CPSS_ERROR;
    }
    cpss_com_drv_read_reg(CPSS_COM_DRV_TCP_SERVER,cpss_com_drv_tcp_read);
    
    /*申请TCP发送需要的内存*/
    g_szCpssTcpSendBufPtr = cpss_mem_malloc(CPSS_COM_TCP_SEND_MAX_LEN) ;
    if(g_szCpssTcpSendBufPtr==NULL)
    {
        return CPSS_ERROR ;
    }
    
    /*清空TCP管理全局变量内存*/
    cpss_mem_memset(&g_stCpssTcpMan,0,sizeof(CPSS_COM_TCP_LINK_MAN_T)) ;
    
    /* dc 驱动初始化 */
    lRet = cpss_com_socket_init(CPSS_COM_DRV_DC_SERVER);
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "dc initial failed!!");
        return CPSS_ERROR;
    }

    cpss_com_drv_read_reg(CPSS_COM_DRV_DC_SERVER,cpss_com_drv_tcp_read);
    
#endif

    /*PCI挂接读处理函数*/
#ifdef CPSS_PCI_INCLUDE
#ifndef CPSS_DSP_CPU    
    /*通信初始化过程统计*/
    g_stCpssComInitStat.stDrvInitInfo.ulPciInitFlag = 1 ;
#endif    
    /*PCI初始化*/
    lRet = cpss_com_drv_pci_init();
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }
#ifndef CPSS_DSP_CPU
    /*通信初始化过程统计*/
    g_stCpssComInitStat.stDrvInitInfo.ulPciInitFlag = 2 ;
#endif

#ifndef CPSS_SLAVE_CPU 
    cpss_com_drv_read_reg(CPSS_COM_DRV_PCI, cpss_com_drv_pci_read);
#else
    cpss_com_drv_read_reg(CPSS_COM_DRV_PCI, cpss_com_slave_pci_drv_deliver);
#endif

#endif    

#ifndef CPSS_DSP_CPU
    cpss_clock_get(&g_stCpssComInitStat.stDrvInitInfo.stTime) ;
#endif

    return CPSS_OK;
    
}

/*******************************************************************************
* 函数名称: cpss_com_drv_find                            
* 功    能: 根据驱动类型查找驱动信息
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* ulDrvType           UINT32          输入              驱动类型，如CPSS_COM_DRV_UDP

* 返回值:
*       成功：驱动表项指针;
*       失败：NULL;
* 说   明:
*                                                 
*******************************************************************************/
CPSS_COM_DRV_T* cpss_com_drv_find(UINT32 ulDrvType)
{
    UINT32 ulIdx;

    for(ulIdx=0; ulIdx<g_ulCpssComDrvNum; ulIdx++)
    {
        if(g_stCpssComDrvTbl[ulIdx].ulDrvType==ulDrvType)
        {
            return &g_stCpssComDrvTbl[ulIdx];
        }
    }
    return NULL;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_deliver                            
* 功    能: 驱动适配层读函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* ulDrvType           UINT32          输入              驱动类型，如CPSS_COM_DRV_UDP
* ulLinkId            UINT32          输入              链路标识
* pucBuf              UINT8*          输入              消息缓冲区,含完整头部结构(IPC)
* ulReadSize          UINT32          输入              缓冲区字节尺寸
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:链路标识对于不同的驱动类型,具有不同的含义,
*         TCP\UDP   链路表数组下标索引
*         PCI       内存交互区ID
*         ATM       atm链路标识
******************************************************************************/
#ifdef CPSS_SLAVE_CPU
/* chgx 20070417 for only slave cpu's pci recv */
#include "cpss_vos_msg_q.h"
#include "cpss_vk_sched.h"
extern  CPSS_VOS_MSG_Q_DESC_T   g_astCpssVosMsgQDescTbl [VOS_MAX_MSG_Q_NUM];
extern VOS_MSG_Q_ID  cpss_vk_get_msg_q_id(UINT32 ulPd);
INT32 cpss_com_slave_pci_drv_deliver
(
    UINT32  ulDrvType,
    UINT32  ulLinkId,
    UINT8  *pstBuf,  /* point to trust hdr. */
    UINT32  ulReadSize
)
{
    CPSS_COM_MSG_HEAD_T* pstMsg;  
    IPC_MSG_HDR_T *pstIPcHdr ;
    VOS_MSG_Q_ID    tMsgQId;
    INT32 lRet;

    /* 设置消息体的指针,指向正确的位置 */
    cpss_com_hdr_ntoh((CPSS_COM_TRUST_HEAD_T*)pstBuf) ;
    
    pstMsg=(CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstBuf);

    cpss_com_trace(pstMsg);
    
    pstMsg->pucBuf=(UINT8*)CPSS_DISP_HDR_TO_USR_HDR(pstMsg);

    pstIPcHdr = (IPC_MSG_HDR_T*)CPSS_TRUST_HDR_TO_IPC_HDR(pstBuf);
	   
    tMsgQId = cpss_vk_get_msg_q_id(pstMsg->stDstProc.ulPd);
    if ((UINT32)tMsgQId != CPSS_ERROR)
    {
         if (msgQSend (tMsgQId,  (INT8 *) (& pstIPcHdr), 4, 0, 0) == CPSS_ERROR)
         {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                               "msgQSend failed!,tMsgQId:%d,pid:%d",tMsgQId,pstMsg->stDstProc.ulPd);
            cpss_com_mem_free((VOID*)pstBuf);

            cpss_com_pci_link_stat(ulLinkId,CPSS_COM_PCI_RECV_DELIVER_ERR_NUM_FLAG); 

            return (CPSS_ERROR);
         }
    }
    else
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                       "msgQSend failed!,tMsgQId:%d,pid:%d",tMsgQId,pstMsg->stDstProc.ulPd);
        cpss_com_mem_free((VOID*)pstBuf);

        cpss_com_pci_link_stat(ulLinkId,CPSS_COM_PCI_RECV_DELIVER_ERR_NUM_FLAG); 

        return (CPSS_ERROR);

    }
     
     return (CPSS_OK);
}
#endif
INT32 cpss_com_drv_deliver
(
    UINT32  ulLinkId,
    CPSS_COM_TRUST_HEAD_T  *pstBuf
)
{
    CPSS_COM_MSG_HEAD_T* pstMsg;  
    CPSS_COM_TRUST_HEAD_T *pstTrustHdr;
    CPSS_COM_PID_T stDstPid;
    INT32 lRet;

    pstTrustHdr = pstBuf ;

    /* 时间戳相关HOOK */
    cpss_com_timestamp_add_hook_drvr(pstBuf);

    /* 设置消息体的指针,指向正确的位置 */
    pstMsg=(CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr);
    
    if(pstTrustHdr->ucPType == COM_SLID_PTYPE_INCREDIBLE)
    {
        pstMsg->pucBuf=(UINT8*)CPSS_DISP_HDR_TO_USR_HDR(pstMsg);

        /* 如果目标地址是本CPU */
        lRet = cpss_com_is_local_address(&pstMsg->stDstProc);
        if(TRUE == lRet)
        {  
            /* 处理本地消息 */
            lRet = cpss_com_local_msg_recv(pstTrustHdr);
            if(CPSS_ERROR == lRet)
            {
                cpss_com_mem_free((VOID*)pstTrustHdr);
            }
            return lRet; 
        }
        else/* 如果不是本地消息,则考虑转发 */
        {
            stDstPid=pstMsg->stDstProc;

                lRet = cpss_com_send_forward(pstMsg,&stDstPid);
                if(CPSS_OK != lRet)
                {
                    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                               "forward packet failed!");
                    if(pstMsg->ucShareFlag == CPSS_COM_MEM_SHARE)
                    {
                        cpss_com_mem_free((VOID*)pstTrustHdr);
                    }
                    return CPSS_ERROR;
                }
            else
            {
                return CPSS_OK;        
            }
        }
    }
    else
    {
        CPSS_COM_LINK_T* pstLink ;
        /*如果连路尚未建好，则不处理可靠控制数据*/
        if(ulLinkId == 0xffffffff)
        {
            cpss_com_mem_free((VOID*)pstTrustHdr) ;
            return CPSS_ERROR ;
        }
        pstLink = cpss_com_link_byid_find(ulLinkId) ;
            
        /*调用滑窗的接收函数*/
        cpss_com_slid_recv(&(pstLink->stSlidWin),pstTrustHdr);
    }
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_write                            
* 功    能: 驱动适配层写函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* pstLink       CPSS_COM_LINK_T*      输入              链路对象
* pucBuf        UINT8 *               输入              消息缓冲区
* ulMaxSize     UINT32                输入              消息缓冲区字节大小
* pulWriteSize  UINT32*               输入              实际输出的字节大小
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:                                            
******************************************************************************/
INT32 cpss_com_drv_write
(
VOID *pstLinkObj,
UINT8  *pucBuf,
UINT32 ulMaxSize,
UINT32 *pulWriteSize
)
{   
    CPSS_COM_LINK_T *pstLink = (CPSS_COM_LINK_T*)pstLinkObj;
    
    /* 校验入参 */
    if(pstLink == NULL)
    {
        return CPSS_ERROR ;
    }
    
    if(NULL != pstLink->stDrv.pfWrite)
    {
        UINT32  ulLinkInfo[3];
        
        ulLinkInfo[0]=pstLink->stDrv.ulDrvD;
        ulLinkInfo[1]=pstLink->stDrv.stSocketAddr.sin_addr.s_addr;
        ulLinkInfo[2]=pstLink->stDrv.stSocketAddr.sin_port;
        
        cpss_com_timestamp_add_hook_drvs(pucBuf);
        return pstLink->stDrv.pfWrite(pstLink->ulLinkId,ulLinkInfo,pucBuf,
                                      ulMaxSize,pulWriteSize);
    }
    
    return CPSS_ERROR;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_ioctl                            
* 功    能: 驱动适配层IO属性操作函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* pstLink       CPSS_COM_LINK_T*      输入              链路对象
* ulCmd         UINT32                输入              命令
* ulParam       UINT32                输入              命令参数
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*                                                 
******************************************************************************/
INT32 cpss_com_drv_ioctl
(
    VOID *pvLink,
    UINT32 ulCmd,
    UINT32 ulParam
)
{
    CPSS_COM_LINK_T *pstLink = (CPSS_COM_LINK_T*)pvLink;
    
     if(NULL != pstLink->stDrv.pfIoCtl)
    {
        return pstLink->stDrv.pfIoCtl(pstLink->stDrv.ulDrvType,ulCmd,ulParam);
    }   

    return CPSS_ERROR;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_linkid_get
* 功    能: 根据指定链路参数,获得linkId
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* ulDrvType           UINT32          输入              驱动类型
* ulLinkInfo[3]       UINT32          输入              链路参数
* pulLinkId           UINT8*          输出              链路标识
* 函数返回: 
*          成功：CPSS_OK;
*          失败：CPSS_ERROR;
* 说    明: 
*******************************************************************************/
INT32 cpss_com_drv_linkid_get
(
    UINT32  ulDrvType,
    UINT32  ulLinkInfo[3], /* socekt,ip,port */
    UINT32  *pulLinkId
)
{
    INT32 lRet ;

    /* 如果是UDP,根据IP地址得到物理地址,然后得到链路对象 */
    if(CPSS_COM_DRV_UDP == ulDrvType)
    {
        CPSS_COM_PHY_ADDR_T stPhyAddr;
        CPSS_COM_LINK_T *pstLink;

        /* 把IP地址转换成物理地址 */
        lRet = cpss_com_ip2phy(ulLinkInfo[1],&stPhyAddr);
        if(CPSS_OK != lRet)
        {
            return CPSS_ERROR;
        }

        /* 根据物理地址查找链路，获取linkId */
        pstLink=cpss_com_link_find_real(stPhyAddr);
        if(NULL != pstLink)
        {
             *pulLinkId=pstLink->ulLinkId;
             return CPSS_OK;
        } 
		*pulLinkId = 0xFFFFFFFF;
		return CPSS_OK;
    }
    /* 如果是TCP,直接查找tcp地址映射表,得到逻辑地址,再找到链路对象 */
    else if(CPSS_COM_DRV_TCP_SERVER== ulDrvType)
    {
        CPSS_COM_DRV_TCP_T *pstDrvTcp=NULL;
        CPSS_COM_TCP_LINK_T stTcpLink ;

        stTcpLink.ulIP = ulLinkInfo[1] ;
        stTcpLink.usTcpPort = (UINT16)ulLinkInfo[2] ;

        /* 查找tcp链路映射表,获取逻辑地址*/
        lRet = cpss_com_tcp_server_link_id_find(&stTcpLink,pulLinkId) ;
        if(CPSS_OK != lRet)
        {
            return CPSS_ERROR ;
        }

        return CPSS_OK;
        
    }
    /* 如果是PCI,???? */
    else if(CPSS_COM_DRV_PCI == ulDrvType)
    {

    }

    return CPSS_ERROR;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_read_reg
* 功    能: 驱动读函数注册
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型                  输入/输出         描述
* ulDrvType          UINT32                 输入              驱动类型
* pfCallBackFunc     CPSS_COM_DRV_READ_PTR  输入              驱动读回调函数
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*                                                 
******************************************************************************/
INT32 cpss_com_drv_read_reg
(
    UINT32 ulDrvType,
    CPSS_COM_DRV_READ_PTR pfCallBackFunc /* 驱动回调函数,读取数据 */
)
{
    CPSS_COM_DRV_T *pstDrv=cpss_com_drv_find(ulDrvType);

    if(NULL == pstDrv)
    {
        return CPSS_ERR_COM_PARA_ILLEGAL;
    } 
    pstDrv->pfRead=pfCallBackFunc;
    
    return CPSS_OK;
}


#ifdef CPSS_UDP_INCLUDE

/*******************************************************************************
* 函数名称: cpss_com_socket_attr_set                            
* 功    能:        设置SOCKET支持的最大发送缓冲区长度。
* 相关文档: G1接口手册
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述

* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*                                                 
*******************************************************************************/
INT32 cpss_com_socket_attr_set(UINT32 ulSocketId, UINT32 ulDrvType)
{
    INT32 lOptValTcp = 65535;
    INT32 lOptValUdp = 65535;
    INT32 lOptVal;

    if (CPSS_COM_DRV_UDP == ulDrvType)
    {
        lOptVal = lOptValUdp;
    }
    else if (CPSS_COM_DRV_TCP_SERVER == ulDrvType)
    {
        lOptVal = lOptValTcp;
    }
    else if(CPSS_COM_DRV_DC_SERVER == ulDrvType)
    {
        lOptVal = lOptValTcp;
    }
    else
    {
        return (CPSS_ERROR);
    }
    
    setsockopt(ulSocketId, SOL_SOCKET, SO_SNDBUF, (UINT8 *)(&lOptVal), sizeof(lOptVal));
    setsockopt(ulSocketId, SOL_SOCKET, SO_RCVBUF, (UINT8 *)(&lOptVal), sizeof(lOptVal));

    return (CPSS_OK);
    
}

/*******************************************************************************
* 函数名称: cpss_com_socket_init                            
* 功    能: 初始化socket相关参数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述

* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*                                                 
*******************************************************************************/
INT32  cpss_com_socket_init(UINT32 ulDrvType)
{
    CPSS_COM_DRV_T* pstDrv;
    CPSS_COM_SOCK_ADDR_T *pstSockAddr;
    INT32 lProtocol;
    INT32 lSockType;
    INT32 lRet;
    UINT32 ulSocketID;

    /* 检查参数的合法性, 支持UDP和TCP初始化过程 */
    if(CPSS_COM_DRV_UDP== ulDrvType)
    {
        lProtocol=IPPROTO_IP;
        lSockType=SOCK_DGRAM;
    }
    else if(CPSS_COM_DRV_TCP_SERVER == ulDrvType)
    {
        lProtocol=IPPROTO_IP;
        lSockType=SOCK_STREAM;
    }
    else if(CPSS_COM_DRV_DC_SERVER == ulDrvType)
    {
        lProtocol=IPPROTO_IP;
        lSockType=SOCK_STREAM;
    }
    else
    {
        return CPSS_ERROR;
    }
        
    pstDrv=cpss_com_drv_find(ulDrvType);
    pstSockAddr=&pstDrv->stSocketAddr;
    
    /* 获取socket描述符 */
    ulSocketID = socket(AF_INET, lSockType, lProtocol);
    if(INVALID_SOCKET == ulSocketID)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
                   "socket initial failed!!");
        return CPSS_ERROR;
    }

    /* 设置SOCKET属性 */
    lRet = cpss_com_socket_attr_set(ulSocketID, ulDrvType);
    if(SOCKET_ERROR == lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
                   "socket Attr set failed!!, Input DrvType(%d) now is not support!", ulDrvType);
        closesocket(ulSocketID);
        return CPSS_ERROR;
    }

    
    /* 填写socket地址参数 */
#ifdef CPSS_VOS_WINDOWS
    pstSockAddr->sin_addr.s_addr= g_pstComData->stAddrThis.ulIpAddr;

#elif defined (CPSS_VOS_LINUX)
    pstSockAddr->sin_addr.s_addr= g_pstComData->stAddrThis.ulIpAddr;

#else
    pstSockAddr->sin_addr.s_addr=0 ;
#endif

    pstSockAddr->sin_port=cpss_htons(pstSockAddr->sin_port);
    
    /*通信初始化流程统计*/
    if(CPSS_COM_DRV_UDP== ulDrvType)
    {
        g_stCpssComInitStat.stDrvInitInfo.ulUdpIp = pstSockAddr->sin_addr.s_addr ;
        g_stCpssComInitStat.stDrvInitInfo.ulUdpPort = pstSockAddr->sin_port ;
    }
    else if(CPSS_COM_DRV_TCP_SERVER == ulDrvType)
    {
        g_stCpssComInitStat.stDrvInitInfo.ulTcpIp = pstSockAddr->sin_addr.s_addr ;
        g_stCpssComInitStat.stDrvInitInfo.ulTcpPort = pstSockAddr->sin_port ;
    }
    else
    {
        g_stCpssComInitStat.stDrvInitInfo.ulDcIp = pstSockAddr->sin_addr.s_addr ;
        g_stCpssComInitStat.stDrvInitInfo.ulDcPort = pstSockAddr->sin_port ;
    }
    
    /* 绑定本地IP地址 */
    printf("socket bind ipaddr = %s, port = %d\n", inet_ntoa(pstSockAddr->sin_addr), cpss_ntohs(pstSockAddr->sin_port));
    lRet = bind(ulSocketID,(struct sockaddr*)pstSockAddr,sizeof(CPSS_COM_SOCK_ADDR_T));
    if(SOCKET_ERROR==lRet)
    {
    	perror("bind");

        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
                   "socket bind failed!!");
        closesocket(ulSocketID);

        /*通信初始化流程统计*/
        if(CPSS_COM_DRV_UDP== ulDrvType)
        {
            g_stCpssComInitStat.stDrvInitInfo.ulUdpBindStat = 1 ;
        }
        else if(CPSS_COM_DRV_TCP_SERVER == ulDrvType)
        {
            g_stCpssComInitStat.stDrvInitInfo.ulTcpBindStat = 1 ;
        }
        else
        {
            g_stCpssComInitStat.stDrvInitInfo.ulDcBindStat = 1 ;
        }

        return CPSS_ERROR;
    }

    pstDrv->ulDrvD = ulSocketID;

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_ldt_socket_init                            
* 功    能: 初始化LDT的socket初始化函数
* 函数类型:                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_com_ldt_socket_init()
{
    INT32 ulSocketID ;
    INT32 lRet ;
    UINT32 ulOptVal = 65535 ;
#ifdef CPSS_VOS_WINDOWS
    CPSS_COM_SOCK_ADDR_T stSockAddr = {AF_INET,CPSS_COM_LDT_ADDR_PORT} ;
#endif

#ifdef CPSS_VOS_VXWORKS
    CPSS_COM_SOCK_ADDR_T stSockAddr = {sizeof(CPSS_COM_SOCK_ADDR_T),AF_INET,CPSS_COM_LDT_ADDR_PORT} ;
#endif

#ifdef CPSS_VOS_LINUX
    CPSS_COM_SOCK_ADDR_T stSockAddr = {AF_INET,CPSS_COM_LDT_ADDR_PORT} ;
#endif
    
    ulSocketID = socket(AF_INET, SOCK_DGRAM, IPPROTO_IP);
    if(INVALID_SOCKET == ulSocketID)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "ldt socket initial failed!!");
        return CPSS_ERROR;
    }

    /*设置socket属性*/
    setsockopt(ulSocketID, SOL_SOCKET, SO_SNDBUF, (UINT8 *)(&ulOptVal), sizeof(ulOptVal));
    setsockopt(ulSocketID, SOL_SOCKET, SO_RCVBUF, (UINT8 *)(&ulOptVal), sizeof(ulOptVal));    
    
    /* 填写socket地址参数 */
#ifdef CPSS_VOS_WINDOWS
    stSockAddr.sin_addr.s_addr= g_pstComData->stAddrThis.ulIpAddr;
#elif defined CPSS_VOS_LINUX
    stSockAddr.sin_addr.s_addr= g_pstComData->stAddrThis.ulIpAddr;
#else
    stSockAddr.sin_addr.s_addr=0 ;
#endif
    
    stSockAddr.sin_port=cpss_htons(CPSS_COM_LDT_ADDR_PORT);
    
    /* 绑定本地IP地址 */
    lRet = bind(ulSocketID,(struct sockaddr*)&stSockAddr,sizeof(CPSS_COM_SOCK_ADDR_T));
    if(SOCKET_ERROR==lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "ldt socket bind failed!!errno=%d",cpss_com_sock_error_get());
        closesocket(ulSocketID);
        
        return CPSS_OK;
    }
    
    g_ulCpssLdtSocket = ulSocketID;
    
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_os_socket_init                            
* 功    能: 操作系统级的SOCKET初始化
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述

* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*                                                 
******************************************************************************/
INT32 cpss_com_os_socket_init()
{
    #ifdef CPSS_VOS_WINDOWS
    /* windows操作系统, socket运行环境初始化 */    
    WSADATA wsaData;
    if (WSAStartup(0x0202, &wsaData)!= 0)
    {     
        return CPSS_ERROR ;
    }    
    #endif
    
    return (CPSS_OK);
}
/*******************************************************************************
* 函数名称: cpss_com_drv_init                            
* 功    能: 通信模块驱动初始化
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述

* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
VOID cpss_udp_drv_stat(UINT32 ulStatFlag, UINT32 ulIp)
{
    UINT32 ulLogShelf ;
    UINT32 ulSlot ;
    UINT32 ulLoc ;
    CPSS_UDP_DRV_STAT_T *pulStat;
    
    ulIp = cpss_ntohl(ulIp) ;

#ifdef CPSS_VOS_WINDOWS
    ulLogShelf = (ulIp & 0x00ff0000) >> 16;
    ulSlot = (ulIp & 0x0000ff00) >> 8;
#else
    ulLogShelf = (ulIp & 0x001f0000) >> 16;
    ulSlot = (ulIp & 0x0000f800) >> 11;
#endif

    ulLoc = (ulLogShelf - 1) * CPSS_UDP_STAT_SLOT_MAX + ulSlot;
    
    if ((ulLogShelf > CPSS_UDP_STAT_LOGSHELF_MAX) || (ulSlot > (CPSS_UDP_STAT_SLOT_MAX - 4)))
    {
        return ;
    }
    
    pulStat = gp_stCpssComUdpStat + ulLoc;
    if (CPSS_UDP_DRV_STAT_SEND == ulStatFlag)
    {
        pulStat->ulSendNum = pulStat->ulSendNum + 1;
    }
    else if(CPSS_UDP_DRV_STAT_RECV == ulStatFlag)
    {
        pulStat->ulRecvNum = pulStat->ulRecvNum + 1;
    }
    else if (CPSS_UDP_DRV_STAT_ERR == ulStatFlag)
    {
        pulStat->ulSendErrNum = pulStat->ulSendErrNum + 1;
    }

    return ;
    
}

VOID cpss_udp_stat_show_all()
{
    UINT32 ulIpMax = CPSS_UDP_STAT_LOGSHELF_MAX*CPSS_UDP_STAT_SLOT_MAX;
    UINT32 ulCount;
    for (ulCount = 0; ulCount < ulIpMax; ulCount++)
    {
        ;
    }
}

/*******************************************************************************
* 函数名称: cpss_udp_stat_loc_get                            
* 功    能: UDP驱动统计－－根据输入IP获取该IP在统计全局变量数组中的下标位置。
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*        封装该函数的目的是为了移植性：IP算法目前有2种，WINDOWS/VXWORKS短期内
*        可能仍会采用不同的方式。通过封装可以在stat函数内屏蔽这种变化。其实真正
*        的封装还是由ip2phy函数完成。
******************************************************************************/
INT32 cpss_udp_stat_loc_get(UINT32 ulIp)
{
    CPSS_COM_PHY_ADDR_T   stPhyAddr;
    BOOL bRet;
    UINT32 ulLogShelf;
    INT32 lLoc;

    cpss_com_ip2phy(ulIp, &stPhyAddr);
    bRet = cpss_com_phyAddr_valid(&stPhyAddr);
    if (TRUE != bRet)
    {
        return (CPSS_ERROR);
    }

    ulLogShelf = (stPhyAddr.ucFrame - 1)*CPSS_COM_MAX_SHELF + stPhyAddr.ucShelf;
    lLoc = (ulLogShelf - 1) * CPSS_UDP_STAT_SLOT_MAX + stPhyAddr.ucSlot;

    return (lLoc);
    
}

/*******************************************************************************
* 函数名称: cpss_com_drv_udp_read                            
* 功    能: UDP驱动适配层读函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_drv_udp_read
(
UINT32  ulDrvType,
UINT32  ulLinkId,
UINT8  *pucBuf,
UINT32  ulReadSize
)
{
    CPSS_COM_TRUST_HEAD_T* pstTrustHdr;
    CPSS_COM_MSG_HEAD_T* pstDispHdr ;
    INT32 lRet;
    
    pstTrustHdr=(CPSS_COM_TRUST_HEAD_T*)pucBuf ;

    if(COM_SLID_PTYPE_INCREDIBLE==pstTrustHdr->ucPType)
    {
        /*如果是不可靠数据则转化字节序*/
        cpss_com_hdr_ntoh(pstTrustHdr);

        /*设置数据内存为共享内存*/
        pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr) ;
        pstDispHdr->ucShareFlag = CPSS_COM_MEM_SHARE ;
    }

    /*将接收到的数据交给高层*/
    lRet = cpss_com_drv_deliver(ulLinkId,pstTrustHdr);
    if (lRet != CPSS_OK)
    {
#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
        cpss_trace_route_error_hook(pucBuf, CPSS_PCI_WRITE_ERR);
#endif
    }
    return lRet;
}



/*******************************************************************************
* 函数名称: cpss_com_drv_udp_write                            
* 功    能: UDP驱动写函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* ulLinkId            UINT32          输入              链路标识
* ulLinkInfo[3]       UINT32          输入              链路参数
* pucBuf              UINT8*          输入              消息缓冲区
* ulMaxSize           UINT32          输入              消息缓冲区字节大小
* pulWriteSize        UINT32*         输入              实际输出的字节大小
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*                                                 
******************************************************************************/
INT32 cpss_com_drv_udp_write
(
    UINT32  ulLinkId,
    UINT32  ulLinkInfo[3], /* socekt,ip,port */
    UINT8  *pucBuf,
    UINT32  ulMaxSize,
    UINT32 *pulWriteSize
)
{
    CPSS_COM_SOCK_ADDR_T stSockAddr;
    INT32 lSockId;

    /* 填写socket参数 */
/**    
    cpss_mem_memset(&stSockAddr,0,sizeof(stSockAddr));
**/
    stSockAddr.sin_family=AF_INET;
    stSockAddr.sin_addr.s_addr=ulLinkInfo[1];
    stSockAddr.sin_port=(UINT16)ulLinkInfo[2];

    if(g_MateBrdIpAddr == stSockAddr.sin_addr.s_addr)
    {
    	lSockId = g_MateSockId;
    }
    else
    {
    	lSockId = ulLinkInfo[0];
    }

#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
    {
        UINT32 bIsSrc;
        if (cpss_trace_route_it_send(pucBuf, &bIsSrc))
        {
            UINT32 ulNodeLoc;
            if (bIsSrc)
                ulNodeLoc = CPSS_TRACE_NODE_LOC_SRC;
            else
                ulNodeLoc = CPSS_TRACE_NODE_LOC_ROUTE;
            cpss_trace_route_info_set(ulNodeLoc, CPSS_TRACE_LAYER_DRVSI, CPSS_TRACE_IF_DRV_UDP, 0,0,0, pucBuf);
        }
    }
#endif

    /* 数据发送 */
#ifdef ZC
    printf("sendto Ipaddr = %s, port = %d\n", inet_ntoa(stSockAddr.sin_addr), cpss_ntohs(stSockAddr.sin_port));
#endif

    *pulWriteSize = sendto(lSockId, pucBuf, ulMaxSize, 0,
                    (struct sockaddr*)&stSockAddr,sizeof(CPSS_COM_SOCK_ADDR_T));
    if(SOCKET_ERROR == (*pulWriteSize)) 
    {
        cpss_udp_drv_stat(CPSS_UDP_DRV_STAT_ERR, stSockAddr.sin_addr.s_addr);
        return CPSS_ERR_COM_SYSCALL_FAIL;            
    }

    cpss_udp_drv_stat(CPSS_UDP_DRV_STAT_SEND, stSockAddr.sin_addr.s_addr);
    
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_version_check
* 功    能: 通信头版本校验函数
* 函数类型: 
* 参    数: 
* 参数名称                类型                    输入/输出         描述
* CPSS_COM_TRUST_HEAD_T   CPSS_COM_TRUST_HEAD_T   输入              接收到的包头数据
* ulRecvLen               UINT32                  输入              接收到的数据长度
* 函数返回: 
*           CPSS_OK:    版本校验通过
*         　CPSS_ERROR: 版本校验不通过
* 说    明: 校验接收的控制面数据的包头版本是否正确
*******************************************************************************/
INT32 cpss_com_version_check
(
 CPSS_COM_TRUST_HEAD_T* pstTrustHdr,
 UINT32 ulRecvLen
)
{
    CPSS_COM_MSG_HEAD_T*  pstDispHdr ;
    UINT32 ulDataLen ;
    UINT32 usSliceLen ;
    
    /*校验数据指针*/
    if(pstTrustHdr == NULL)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,"recv data is null") ;
        return CPSS_ERROR ;
    }

    /*校验数据长度*/
    if(ulRecvLen < (UINT32)CPSS_COM_HDRLEN_UNPACK(pstTrustHdr->hdrLen))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
            "check data len is illegal ulRecvLen=%d,dataLen=%d",
            ulRecvLen,CPSS_COM_HDRLEN_UNPACK(pstTrustHdr->hdrLen));
        return CPSS_ERROR ;
    }
    
    /*通信头版本校验*/
    if(pstTrustHdr->ver != CPSS_COM_PROTOCAL_VER)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
            "check version is not compatible version=%d",pstTrustHdr->ver);
        return CPSS_ERROR ;
    }

    /*校验不可靠数据长度*/
    if(pstTrustHdr->ucPType == COM_SLID_PTYPE_INCREDIBLE)
    {
        pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr) ;
        ulDataLen = cpss_ntohl(pstDispHdr->ulLen) ;

        if(ulDataLen != CPSS_LINK_HDR_LEN_SUBTRACT(ulRecvLen))
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                "incredible recv len is not equal datalen recvLen=%d,dataLen=%d",
                CPSS_LINK_HDR_LEN_SUBTRACT(ulRecvLen),ulDataLen);
            return CPSS_ERROR ;
        }
    }
    /*校验可靠数据长度*/
    else if(pstTrustHdr->ucPType == COM_SLID_PTYPE_CREDIBLE)
    {
        usSliceLen = cpss_ntohs(pstTrustHdr->usSliceLen) ;

        if(usSliceLen != ulRecvLen)
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                "credible recv len is not equal datalen recvLen=%d,dataLen=%d",
                ulRecvLen,usSliceLen);
            return CPSS_ERROR ;
        }
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_udp_task
* 功    能: udp消息接收任务
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述

* 函数返回: 
*          无
* 说    明: 负责接收UDP驱动方式的消息，然后进行处理，或者进行转发
*******************************************************************************/

UINT32 g_ulCpssComUdpTaskPList[8];

INT32 cpss_com_udp_task_init()
{
    UINT8 *pucBuf;
    CPSS_COM_DRV_T  *pstDrv  = cpss_com_drv_find(CPSS_COM_DRV_UDP);
    
    g_ulCpssComUdpTaskPList[1] = 64*1024;
    pucBuf = malloc(g_ulCpssComUdpTaskPList[1]);
    if (NULL == pucBuf)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                "Udp :malloc receive buff(%d) failed!\n", g_ulCpssComUdpTaskPList[1]);
        return (CPSS_ERROR);
    }
    cpss_mem_memset(pucBuf, 0, g_ulCpssComUdpTaskPList[1]);
    g_ulCpssComUdpTaskPList[2] = (UINT32)pucBuf;
    g_ulCpssComUdpTaskPList[3] = 0;
    /* 等待通信模块初始化完成 */
    while(0 == pstDrv->ulDrvD)
    {
        cpss_vos_task_delay(1000);
    }
    g_ulCpssComUdpTaskPList[0] = pstDrv->ulDrvD;

    return (CPSS_OK);
    
}
/*
    0:socketId;
    1:ulRecvBufLen;
    2:ulRecvBufPtr;
    3:ulOption;
    4:stSockAddr;
*/
INT32 cpss_com_udp_task_block(UINT32 *pulParamList)
{
    CPSS_COM_SOCK_ADDR_T stSockAddr;
    INT32 lRecvLen, lMaxRecvLen = pulParamList[1];    
    INT32 lRecvSize   = sizeof(CPSS_COM_SOCK_ADDR_T);    
    UINT8 *pucRecvBuf = (UINT8*)pulParamList[2];

    lRecvLen = recvfrom(pulParamList[0], pucRecvBuf, lMaxRecvLen,
                            0,(struct sockaddr *)&stSockAddr, &lRecvSize);

    memcpy(&pulParamList[4], &stSockAddr, lRecvSize);
    
    return (lRecvLen);
}
INT32 cpss_com_udp_task_dealing(INT32 lRecvLen, UINT32 *pulParamList)
{
    CPSS_COM_SOCK_ADDR_T *pstSockAddr = (CPSS_COM_SOCK_ADDR_T*)&pulParamList[4];
    INT32 lMaxRecvLen = pulParamList[1];    
    INT32 lRecvSize   = sizeof(CPSS_COM_SOCK_ADDR_T);    
    UINT8 *pucRecvBuf = (UINT8*)pulParamList[2];

    CPSS_COM_TRUST_HEAD_T *pstTrustHdr = NULL;
    CPSS_COM_DRV_T  *pstDrv  = cpss_com_drv_find(CPSS_COM_DRV_UDP);
    
    UINT32 ulLinkId,ulLinkInfo[3];

    if ( CPSS_ERROR == lRecvLen )
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                       "udp task received error(%d)!", 
                       cpss_com_sock_error_get());
        return (CPSS_ERROR);
    }
    
    g_ulUdpRecvNum++;
    
    cpss_udp_drv_stat(CPSS_UDP_DRV_STAT_RECV, pstSockAddr->sin_addr.s_addr);
        
    cpss_com_byteflow_print(CPSS_MODULE_DRV_UDP, pucRecvBuf, lRecvLen);
    /* 把消息头部转化成主机字节序 */
    pstTrustHdr = (CPSS_COM_TRUST_HEAD_T *)(pucRecvBuf);

    /*版本校验*/
    if(CPSS_OK != cpss_com_version_check(pstTrustHdr,lRecvLen))
    {
        return (CPSS_ERROR);
    }
#if 0
    /* 判断接收到的消息是否完整 */
    if(lRecvLen < CPSS_COM_HDRLEN_UNPACK(pstTrustHdr->hdrLen))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                   "udp task received bad packet!");
        return (CPSS_ERROR);
    }
#endif
#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
        {
            UINT32 bIsDst;
            BOOL bTraceIt = cpss_trace_route_it_recv(pucRecvBuf, &bIsDst);
            if (TRUE == bTraceIt)
            {
                UINT32 ulNodeLoc;
                if (TRUE == bIsDst)
                    ulNodeLoc = CPSS_TRACE_NODE_LOC_DST;
                else
                    ulNodeLoc = CPSS_TRACE_NODE_LOC_ROUTE;
                cpss_trace_route_info_set(ulNodeLoc, CPSS_TRACE_LAYER_DRVRI, CPSS_TRACE_IF_DRV_UDP, 0,0,0, pucRecvBuf);
            }
        }
#endif

    /* 申请通信消息头,然后拷贝消息 */        
    pstTrustHdr=cpss_com_nodisp_mem_alloc(CPSS_TRUST_HDR_LEN_SUBTRACT(lRecvLen));
    
    if(NULL == pstTrustHdr)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                  "udp task, TRUST message head alloc failed!");
        return (CPSS_ERROR);
    }

   
    cpss_mem_memcpy((UINT8*)pstTrustHdr, pucRecvBuf, lRecvLen);

    /* 根据链路参数, 获取linkId */
    ulLinkInfo[0] = pstDrv->ulDrvD;
    ulLinkInfo[1] = cpss_ntohl(pstSockAddr->sin_addr.s_addr);
    ulLinkInfo[2] = cpss_ntohs(pstSockAddr->sin_port);
    
    if(CPSS_OK != cpss_com_drv_linkid_get(CPSS_COM_DRV_UDP,ulLinkInfo,&ulLinkId))
    {
        cpss_com_mem_free(pstTrustHdr);
        return (CPSS_ERROR);
    }

    /* 把收到的报文交给驱动适配层来处理 */
    if((pstDrv->pfRead != NULL) && 
      (CPSS_OK == pstDrv->pfRead(CPSS_COM_DRV_UDP,ulLinkId,(UINT8 *)pstTrustHdr,lRecvSize)))
    {
        return (CPSS_OK);
    }
        
    return (CPSS_ERROR);
    
}

INT32 cpss_com_udp_task_reg()
{
    INT32 lRet;
    lRet = cpss_vos_ntask_register(
              CPSS_NORMAL_TASK_TYPE_UDP,
              cpss_com_udp_task_init, 
              cpss_com_udp_task_block,
              cpss_com_udp_task_dealing,
              g_ulCpssComUdpTaskPList,
              CPSS_UDPRECV_TASKNAME);
    if ( lRet != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                "UDP task register failed! errCode = %#x \n", lRet);
        return (CPSS_ERROR);
    }

    return (CPSS_OK);
    
}

VOID cpss_com_drv_udp_task(VOID)
{
    CPSS_COM_SOCK_ADDR_T stSockAddr;
    CPSS_COM_TRUST_HEAD_T *pstTrustHdr = NULL;    
    CPSS_COM_DRV_T  *pstDrv  = cpss_com_drv_find(CPSS_COM_DRV_UDP);
    
    /* 消息大小定义个极限值64k */
    INT32 lRecvLen, lMaxRecvLen = 64*1024;
    UINT32 ulLinkId,ulLinkInfo[3];
    
    INT32 lRecvSize   = sizeof(CPSS_COM_SOCK_ADDR_T);    
    UINT8 *pucRecvBuf = cpss_mem_malloc(lMaxRecvLen);

    /* 申请接收缓冲区内存 */
    if(NULL == pucRecvBuf)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                   "udp task, malloc receive buff failed!");
        return;
    }
    cpss_mem_memset(pucRecvBuf,0,lMaxRecvLen);
            
    /* 等待通信模块初始化完成 */
    while(0 == pstDrv->ulDrvD)
    {
        cpss_vos_task_delay(100);
    }

    while(TRUE)
    {
        
        cpss_kw_set_task_stop() ;
        
        /* 接收UDP消息 */
        lRecvLen = recvfrom(pstDrv->ulDrvD,pucRecvBuf,lMaxRecvLen,
                            0,(struct sockaddr *)&stSockAddr,&lRecvSize);
        if( SOCKET_ERROR ==lRecvLen )
        {           
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                       "udp task received error(%d)!", 
                       cpss_com_sock_error_get());
            continue;
        }

        cpss_kw_set_task_run() ;

        cpss_udp_drv_stat(CPSS_UDP_DRV_STAT_RECV, stSockAddr.sin_addr.s_addr);
        
        cpss_com_byteflow_print(CPSS_MODULE_DRV_UDP, pucRecvBuf, lRecvLen);
        /* 把消息头部转化成主机字节序 */
        pstTrustHdr = (CPSS_COM_TRUST_HEAD_T *)(pucRecvBuf);

        /*版本校验*/
        if(CPSS_OK != cpss_com_version_check(pstTrustHdr,lRecvLen))
        {
            continue ;
        }
#if 0
        /* 判断接收到的消息是否完整 */
        if(lRecvLen < CPSS_COM_HDRLEN_UNPACK(pstTrustHdr->hdrLen))
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                       "udp task received bad packet!");
            continue;
        }
#endif

#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
            {
                UINT32 bIsDst;
                BOOL bTraceIt = cpss_trace_route_it_recv(pucRecvBuf, &bIsDst);
                if (TRUE == bTraceIt)
                {
                    UINT32 ulNodeLoc;
                    if (TRUE == bIsDst)
                        ulNodeLoc = CPSS_TRACE_NODE_LOC_DST;
                    else
                        ulNodeLoc = CPSS_TRACE_NODE_LOC_ROUTE;
                    cpss_trace_route_info_set(ulNodeLoc, CPSS_TRACE_LAYER_DRVRI, CPSS_TRACE_IF_DRV_UDP, 0,0,0, pucRecvBuf);
                }
            }
#endif

        /* 申请通信消息头,然后拷贝消息 */        
        pstTrustHdr=cpss_com_nodisp_mem_alloc(CPSS_TRUST_HDR_LEN_SUBTRACT(lRecvLen));
        
        if(NULL == pstTrustHdr)
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                      "udp task, TRUST message head alloc failed!");
            continue;
        }
        
        cpss_mem_memcpy((UINT8*)pstTrustHdr,pucRecvBuf,lRecvLen);

        /* 根据链路参数, 获取linkId */
        ulLinkInfo[0] = pstDrv->ulDrvD;
        ulLinkInfo[1] = cpss_ntohl(stSockAddr.sin_addr.s_addr);
        ulLinkInfo[2] = cpss_ntohs(stSockAddr.sin_port);
        
        if(CPSS_OK != cpss_com_drv_linkid_get(CPSS_COM_DRV_UDP,ulLinkInfo,&ulLinkId))
        {
            cpss_com_mem_free(pstTrustHdr);
            continue;
        }

        /* 把收到的报文交给驱动适配层来处理 */
        if((pstDrv->pfRead != NULL) && 
          (CPSS_OK == pstDrv->pfRead(CPSS_COM_DRV_UDP,ulLinkId,(UINT8 *)pstTrustHdr,lRecvSize)))
        {
            continue;
        }
        
    }/*end while(TRUE)*/
}
#endif

#ifdef CPSS_TCP_INCLUDE

/*******************************************************************************
* 函数名称: cpss_com_dc_login_msg_compose                            
* 功    能: 组织网络直连登录消息
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
CPSS_COM_TRUST_HEAD_T* cpss_com_dc_login_msg_compose
(
 CPSS_COM_TCP_LINK_T* pstTcpLink,
 CPSS_COM_MSG_HEAD_T* pstRecvData
)
{
    CPSS_COM_TCP_USR_LOGIN_IND_MSG_T stTcpLoginMsg ;
    CPSS_COM_TRUST_HEAD_T* pstTrustHdr ;
    CPSS_COM_MSG_HEAD_T* pstSendData;

    stTcpLoginMsg.stTcpInfo.ulIP = pstTcpLink->ulIP ;
    stTcpLoginMsg.stTcpInfo.usTcpPort = pstTcpLink->usTcpPort ;

    cpss_mem_memcpy(stTcpLoginMsg.aucBuf,pstRecvData->pucBuf,pstRecvData->ulLen) ;

    /*由于消息体进行重构,所以重新申请IPC内存*/
    pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)cpss_com_mem_alloc(sizeof(CPSS_COM_TCP_USR_LOGIN_IND_MSG_T)) ;
    if(pstTrustHdr == NULL)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_dc_login_msg_compose malloc failed size=%d\n",
            sizeof(CPSS_COM_TCP_USR_LOGIN_IND_MSG_T));
        return NULL ;
    }

    /*设置TRUST头*/
    pstTrustHdr->ucPType  = COM_SLID_PTYPE_INCREDIBLE ;    

    pstSendData = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr) ; 
    pstSendData->pucBuf = (UINT8*)CPSS_DISP_HDR_TO_USR_HDR(pstSendData) ;

    /*拷贝数据内容*/
    cpss_mem_memcpy(pstSendData->pucBuf,
        (UINT8*)&stTcpLoginMsg,pstRecvData->ulLen+sizeof(CPSS_COM_TCP_LINK_T)) ;  

    /*设置消息为共享内存*/
    pstSendData->ucShareFlag = CPSS_COM_MEM_SHARE ;
    pstSendData->ucAckFlag = CPSS_COM_NOT_ACK ;
    pstSendData->ucPriFlag = CPSS_COM_PRIORITY_NORMAL ;

    pstSendData->ulLen = pstRecvData->ulLen + sizeof(CPSS_COM_TCP_LINK_T) ;
    pstSendData->ulMsgId = CPSS_COM_TCP_USR_LOGIN_IND_MSG ;
    pstSendData->stDstProc.ulPd = CPS__OAMS_PD_M_DLM_MSGDSP_PROC ;

    cpss_com_logic_addr_get(
        &pstSendData->stDstProc.stLogicAddr,&pstSendData->stDstProc.ulAddrFlag) ;
    
    return pstTrustHdr ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_tcp_read                            
* 功    能: TCP驱动适配层读函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_drv_tcp_read
(
 UINT32  ulDrvType,
 UINT32  ulLinkId,
 UINT8  *pucBuf,
 UINT32  ulReadSize
 )
{
    CPSS_COM_TRUST_HEAD_T* pstTrustHdr;
    CPSS_COM_TRUST_HEAD_T* pstSendTrustHdr ;
    CPSS_COM_DRV_TCP_T* pstTcpDrv;
    CPSS_COM_LOGIC_ADDR_T stLogicAddr = {0};
    CPSS_COM_MSG_HEAD_T* pstMsg;
    UINT32 ulServerFlag ;
    INT32 lRet; 
    
    pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)pucBuf;
    pstTcpDrv   = (CPSS_COM_DRV_TCP_T*)ulLinkId;
    /*TCP接收到的数据统计*/
    pstTcpDrv->stLinkDataStat.ulRecvNum = pstTcpDrv->stLinkDataStat.ulRecvNum + 1;

    /*转化字节序为本机字节序*/
    if(pstTcpDrv->ulServerFlag != CPSS_COM_TCP_DC_FLAG)
    {
        cpss_com_hdr_ntoh(pstTrustHdr) ;
    }
    
    /*发送数据到希望的纤程*/
    pstMsg    = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr) ;
    
    pstTrustHdr->ucPType  = COM_SLID_PTYPE_INCREDIBLE ;
    
#ifndef SWP_FNBLK_BRDTYPE_ABOX
    /* 如果该链路没有绑定逻辑地址或路由属性不允许转发，则所有的消息发送到oams通信纤程 */
    lRet = cpss_com_tcp_route_allow(&pstTcpDrv->stTcpLink,&stLogicAddr,&ulServerFlag);

    if((CPSS_TCP_NO_ROUTE == lRet)||(CPSS_TCP_NO_BIND == lRet))
    {
        cpss_com_logic_addr_get(&pstMsg->stDstProc.stLogicAddr,&pstMsg->stDstProc.ulAddrFlag) ;        

        if(ulServerFlag == CPSS_COM_TCP_SERVER_FLAG)
        {
            pstMsg->stDstProc.ulPd        = CPS__OAMS_PD_M_COMM_PROC;
        
            if(CPSS_TCP_NO_BIND == lRet)
            {
                /* 填写鉴权消息的IP和port，网络字节序 */
                pstMsg->pucBuf = (UINT8*)CPSS_DISP_HDR_TO_USR_HDR(pstMsg);
                *(UINT32*)&pstMsg->pucBuf[4] = pstTcpDrv->stTcpLink.ulIP;
                *(UINT16*)&pstMsg->pucBuf[8] = pstTcpDrv->stTcpLink.usTcpPort;
            }
        }
        else if(ulServerFlag == CPSS_COM_TCP_DC_FLAG)
        {
            if(CPSS_TCP_NO_BIND == lRet)
            {                                
                /*组织登录请求消息*/
                pstSendTrustHdr = cpss_com_dc_login_msg_compose(&pstTcpDrv->stTcpLink,pstMsg) ;
                if(pstSendTrustHdr == NULL)
                {
                    cpss_com_mem_free(pstTrustHdr) ;
                    return CPSS_ERROR ;
                }

                /*释放内存*/
                cpss_com_mem_free(pstTrustHdr) ;
                
                pstTrustHdr = pstSendTrustHdr ;
            }
        }
    }
#else
        if(pstMsg->stDstProc.ulPd == 0)
        {
            *(UINT32*)&(pstMsg->stDstProc.stLogicAddr) = CPSS_COM_LOCAL_LOGIC_ADDR ;
            pstMsg->stDstProc.ulPd        = CPS__OAMS_PD_M_COMM_PROC;                
        }
#endif
    
    /*将接收到的数据交给高层*/
    lRet = cpss_com_drv_deliver(0,pstTrustHdr);
    /* 交给高层失败的数据统计*/
    if(CPSS_OK != lRet)
    {
       pstTcpDrv->stLinkDataStat.ulDeliverErrNum = pstTcpDrv->stLinkDataStat.ulDeliverErrNum + 1;
    }
    return lRet;
}


/*******************************************************************************
* 函数名称: cpss_com_drv_tcp_write                            
* 功    能: tcp驱动写函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* ulLinkId            UINT32          输入              链路标识
* ulLinkInfo[3]       UINT32          输入              链路参数
* pucBuf              UINT8*          输入              消息缓冲区
* ulMaxSize           UINT32          输入              消息缓冲区字节大小
* pulWriteSize        UINT32*         输入              实际输出的字节大小
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:                                           
******************************************************************************/
INT32 cpss_com_drv_tcp_write
(
    UINT32  ulLinkId,
    UINT32  ulLinkInfo[3], /* socekt,ip,port */
    UINT8  *pucBuf,
    UINT32  ulMaxSize,
    UINT32 *pulWriteSize    
)
{
    INT32 lSendLen;
    UINT32 ulSocket = ulLinkInfo[0];
    CPSS_COM_TCP_LINK_T stTcpLink = {0} ;

    /* 数据发送 */
    lSendLen = cpss_com_tcp_data_send(ulSocket,
        CPSS_TRUST_HDR_TO_DISP_HDR(pucBuf),CPSS_TRUST_HDR_LEN_SUBTRACT(ulMaxSize)) ;

    /*判断TCP链路承载能力是否超标*/
#if 0
    if(lSendLen != (INT32)(CPSS_TRUST_HDR_LEN_SUBTRACT(ulMaxSize)))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "tcp drv send failed.lSendBuf=0x%x,errno=%d\n",lSendLen,cpss_com_sock_error_get());

        /*tcp断开统计*/
        g_stTcpCloseStat.ulTcpSendFullCloseNum = g_stTcpCloseStat.ulTcpSendFullCloseNum + 1 ;
        
        return CPSS_ERROR ;
    }
#endif
    if(SOCKET_ERROR == lSendLen) 
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                 "CPSS_DRV: com tcp write failed! length 0x%08x",lSendLen);
        return CPSS_ERR_COM_SYSCALL_FAIL;            
    }  
    *pulWriteSize=lSendLen;
    
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_msg_body_recv                            
* 功    能: 
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_tcp_msg_body_recv
(
CPSS_COM_DRV_TCP_T *pstTcpDrv,
STRING             szHdrBuf,
INT32              lMsgLen
)
{
    UINT32              ulTcpSocket;
    INT32               lRecvLen;
    INT32               lTotalLen;
    INT32               lMsgBodyLen;
    CPSS_COM_TCP_LINK_T stTcpLink; 
    CPSS_COM_TCP_DISCONN_IND_MSG_T stDisconnMsg ;
    INT32 lRet ;
    
    ulTcpSocket = pstTcpDrv->ulSClient ;
    lTotalLen   = 0 ;
    lMsgBodyLen = lMsgLen ;
    
    if(lMsgLen==0)
    {
        return CPSS_OK ;
    }
    else if(lMsgLen<0)
    {
        return CPSS_ERROR ;
    }

    while(TRUE)
    {
        lRecvLen = recv(ulTcpSocket,szHdrBuf,lMsgBodyLen,0) ;
        if(lRecvLen <= 0)
        {
            g_stCpssTcpErrStat.ulRecvErrNum++ ;

            stTcpLink.ulIP      = pstTcpDrv->stTcpLink.ulIP ;
            stTcpLink.usTcpPort = pstTcpDrv->stTcpLink.usTcpPort ;
            
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                "tcp task received error.IP(%x),port(%d)",
                stTcpLink.ulIP,stTcpLink.usTcpPort);
            
            /*断开socket连接,清除socket表示*/
            cpss_com_tcp_server_socket_free(ulTcpSocket) ;

            /*向OAMS发送disconnect消息*/
            stDisconnMsg.ulIP      = pstTcpDrv->stTcpLink.ulIP ;
            stDisconnMsg.usTcpPort = pstTcpDrv->stTcpLink.usTcpPort ;
            
            lRet = cpss_com_tcp_cps__oams_connect_info_send(
                (UINT8*)&stDisconnMsg,CPSS_COM_TCP_DISCONN_IND_MSG,pstTcpDrv->ulServerFlag);
            if(CPSS_OK != lRet)
            {
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                    "send tcp disconnect indicate message failed!");
                cpss_com_tcp_link_disconnect(&stTcpLink) ;
            }

            return CPSS_ERROR;
        }
        
        lTotalLen    = lTotalLen + lRecvLen ;
        szHdrBuf     = szHdrBuf  + lRecvLen ;
        lMsgBodyLen  = lMsgBodyLen  - lRecvLen ;
        
        if(lTotalLen == lMsgLen)
        {
            return CPSS_OK ;
        }
    }        
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_msg_read_to_null                            
* 功    能: 将TCP连接的数据读空
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:                                             
******************************************************************************/
INT32 cpss_com_tcp_msg_read_to_null
(
CPSS_COM_DRV_TCP_T *pstTcpDrv,
INT32              lMsgLen
)
{
#define CPSS_COM_TCP_ONCE_READ_SIZE   1024
    UINT32              ulTcpSocket;
    INT32               lRecvLen;
    INT32               lTotalLen;
    INT32               lMsgBodyLen;
    INT32               lReadNum ;
    INT32               lLoop ;
    INT32               lRecvLenRecord ;
    UINT8               aucRecvBuf[CPSS_COM_TCP_ONCE_READ_SIZE] ;
    CPSS_COM_TCP_LINK_T stTcpLink; 
    CPSS_COM_TCP_DISCONN_IND_MSG_T stDisconnMsg ;
    INT32 lRet;
    
    
    ulTcpSocket = pstTcpDrv->ulSClient ;
    lTotalLen   = 0 ;

    /*计算读取的次数*/
    if(CPSS_MOD(lMsgLen,CPSS_COM_TCP_ONCE_READ_SIZE) == 0)
    {
        lReadNum = CPSS_DIV(lMsgLen,CPSS_COM_TCP_ONCE_READ_SIZE) ;
    }
    else
    {
        lReadNum = CPSS_DIV(lMsgLen,CPSS_COM_TCP_ONCE_READ_SIZE) + 1 ;
    }

    for(lLoop=0;lLoop<lReadNum;lLoop++)
    {
        /*得到TCP读取的长度*/
        if(lLoop<lReadNum-1)
        {
            lMsgBodyLen = CPSS_COM_TCP_ONCE_READ_SIZE ;
        }
        else
        {
            lMsgBodyLen = lMsgLen - CPSS_COM_TCP_ONCE_READ_SIZE*(lReadNum-1) ;
        }

        lRecvLenRecord = lMsgBodyLen ;
        lTotalLen      = 0 ;

        while(TRUE)
        {
            lRecvLen = recv(ulTcpSocket,aucRecvBuf,lMsgBodyLen,0) ;
            if(lRecvLen <= 0)
            {
                g_stCpssTcpErrStat.ulRecvErrNum++ ;
                
                stTcpLink.ulIP      = pstTcpDrv->stTcpLink.ulIP ;
                stTcpLink.usTcpPort = pstTcpDrv->stTcpLink.usTcpPort ;
                
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                    "tcp task received error.IP(%x),port(%d)",
                    stTcpLink.ulIP,stTcpLink.usTcpPort);

                /*断开socket连接,清除socket表示*/
                cpss_com_tcp_server_socket_free(ulTcpSocket) ;                

                /*向OAMS发送disconnect消息*/
                stDisconnMsg.ulIP      = pstTcpDrv->stTcpLink.ulIP ;
                stDisconnMsg.usTcpPort = pstTcpDrv->stTcpLink.usTcpPort ;
                
                lRet = cpss_com_tcp_cps__oams_connect_info_send(
                    (UINT8*)&stDisconnMsg,CPSS_COM_TCP_DISCONN_IND_MSG,pstTcpDrv->ulServerFlag);
                if(CPSS_OK != lRet)
                {
                    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                        "send tcp disconnect indicate message failed!");
                    cpss_com_tcp_link_disconnect(&stTcpLink) ;
                }                
                return CPSS_ERROR;
            }
            
            lTotalLen    = lTotalLen + lRecvLen ;
            lMsgBodyLen  = lMsgBodyLen  - lRecvLen ;
            
            if(lTotalLen == lRecvLenRecord)
            {
                break ;
            }
        }
    }  
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_tcp_close_show                            
* 功    能: cpss主动断开TCP连接统计 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明:
******************************************************************************/
VOID cpss_tcp_close_show()
{
    cps__oams_shcmd_printf("ulTcpRecvErrCloseNum=%d\n",g_stTcpCloseStat.ulTcpRecvErrCloseNum) ;
    cps__oams_shcmd_printf("ulTcpSendFullCloseNum=%d\n",g_stTcpCloseStat.ulTcpSendFullCloseNum) ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_tcp_msg_recv                            
* 功    能: 接收完整的TCP消息 
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*                                                 
******************************************************************************/
INT32 cpss_com_drv_tcp_msg_recv
(
 CPSS_COM_DRV_TCP_T *pstTcpDrv,
 UINT8** ppucTcpRecvBuf
)
{
    INT32  lMsgBodyLen ;
    UINT8  aucComHdr[CPSS_COM_DISP_HEAD_LEN] ;
    STRING szMsgBodyPtr ;
    INT32  lRet;
    
    /*接收消息体的包头*/
    lRet = cpss_com_tcp_msg_body_recv(pstTcpDrv,aucComHdr,sizeof(CPSS_COM_MSG_HEAD_T));
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }
    
    /*得到包长度*/
    lMsgBodyLen   = cpss_ntohl(((CPSS_COM_MSG_HEAD_T *)aucComHdr)->ulLen) ;
    
    /*申请包内存*/
    szMsgBodyPtr = cpss_com_mem_alloc(lMsgBodyLen) ;
    
    if(szMsgBodyPtr == NULL)
    {
        /*错误处理,读空TCP数据*/
        lRet = cpss_com_tcp_msg_read_to_null(pstTcpDrv,lMsgBodyLen);
        if(CPSS_OK != lRet)
        {
            return CPSS_ERROR ;
        }
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
            "tcp recv task malloc failed.IP(%x).port(%x)",
            pstTcpDrv->stTcpLink.ulIP,pstTcpDrv->stTcpLink.usTcpPort);
        return CPSS_ERROR ;        
    }
    else
    {
        /*拷贝包头到包内存中*/
        cpss_mem_memcpy(CPSS_TRUST_HDR_TO_DISP_HDR(szMsgBodyPtr),aucComHdr,CPSS_COM_DISP_HEAD_LEN) ;
        
        lRet = cpss_com_tcp_msg_body_recv(pstTcpDrv,CPSS_TRUST_HDR_TO_USR_HDR(szMsgBodyPtr),lMsgBodyLen);
        if(CPSS_OK != lRet)
        {
            cpss_com_mem_free(szMsgBodyPtr);
            return CPSS_ERROR ;
        }
    }
    
    *ppucTcpRecvBuf = szMsgBodyPtr ;
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_listen_task_init
* 功    能: tcp监听任务初始化
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述

* 函数返回: 
*          无
* 说    明:                                               
*******************************************************************************/
INT32 cpss_com_tcp_listen_task_init()
{
    CPSS_COM_DRV_T  *pstDrv=cpss_com_drv_find(CPSS_COM_DRV_TCP_SERVER);

    /* 等待创建互斥量 */
    while(CPSS_VOS_MUTEXD_INVALID == g_ulCpssComDrvTcpMutex)
    {
        cpss_vos_task_delay(200);
    }
    
    /* 对端口进行监听 */
    listen(pstDrv->ulDrvD,8);
    g_ulCpssComTcpListenTaskPList[0] = pstDrv->ulDrvD;
    
    return (CPSS_OK);
}
    
/*******************************************************************************
* 函数名称: cpss_com_tcp_listen_task_block
* 功    能: tcp监听任务阻塞函数
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* 函数返回: 
*          无
* 说    明:                                               
*******************************************************************************/
INT32 cpss_com_tcp_listen_task_block(UINT32 *pulParamList)
{
    CPSS_COM_SOCK_ADDR_T stClient = {0};
    INT32  lAddrSize ;
    INT32  lSClient;
    lAddrSize = sizeof(stClient);
    lSClient = accept(pulParamList[0], (struct sockaddr *)&stClient,
                           &lAddrSize);
    memcpy(&pulParamList[1], &stClient, lAddrSize);
    return (lSClient);
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_listen_task_dealing
* 功    能: tcp监听任务处理过程
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述

* 函数返回: 
*          无
* 说    明:                                               
*******************************************************************************/
INT32 cpss_com_tcp_listen_task_dealing(INT32 lSClient, UINT32 *pulParamList)
{
    UINT32 ulSockAttr = 1; /* TRUE表示NONE BLOCK */    
    CPSS_COM_TCP_CONN_IND_MSG_T stTcpMsg = {0};
    CPSS_COM_DRV_TCP_T* pstTcpDrvInfo = NULL ;
    INT32 lRet;
    CPSS_COM_TCP_LINK_T stTcpLink ;
    CPSS_COM_SOCK_ADDR_T *pstClient = (CPSS_COM_SOCK_ADDR_T*)&pulParamList[1];
    CPSS_COM_DRV_T  *pstDrv=cpss_com_drv_find(CPSS_COM_DRV_TCP_SERVER);
    
    
    if(INVALID_SOCKET == lSClient)
    {
        g_stCpssTcpErrStat.ulAcceptErrNum++ ;

        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                  "tcp task, accept failed!");
        return CPSS_ERROR;
    }

        /*设置socket为非阻塞socket*/
#ifdef CPSS_VOS_WINDOWS
    ioctlsocket(lSClient, FIONBIO, &ulSockAttr);
#elif defined CPSS_VOS_LINUX
    fcntl(lSClient, F_SETFL, 0);
#else
    ioctl(lSClient,FIONBIO,(INT32)&ulSockAttr);
#endif
    stTcpLink.ulIP = cpss_ntohl(pstClient->sin_addr.s_addr);
    stTcpLink.usTcpPort = cpss_ntohs(pstClient->sin_port);

    if(NULL != cpss_com_tcp_server_link_find(&stTcpLink))
    {
        closesocket(lSClient) ;
        return CPSS_ERROR; ;
    }

        /*寻找空的链路位置*/
    pstTcpDrvInfo = cpss_com_tcp_server_free_link_find(CPSS_COM_TCP_SERVER_FLAG) ;
    if(pstTcpDrvInfo == NULL)
    {                
        g_stCpssTcpErrStat.ulNoFreeSpaceNum++ ;

        closesocket(lSClient) ; 

        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
            "have no space for new connect!");  
        return CPSS_ERROR;
    }

    /*保存客户端连接信息*/
    pstTcpDrvInfo->ulSClient=lSClient;
    pstTcpDrvInfo->stTcpLink.ulIP=stTcpLink.ulIP;
    pstTcpDrvInfo->stTcpLink.usTcpPort=stTcpLink.usTcpPort;
    pstTcpDrvInfo->ulServerFlag = CPSS_COM_TCP_SERVER_FLAG ;
        
        /* 向oams发送消息CPSS_COM_TCP_CONN_IND_MSG */
    stTcpMsg.ulIP      = pstTcpDrvInfo->stTcpLink.ulIP;
    stTcpMsg.usTcpPort = pstTcpDrvInfo->stTcpLink.usTcpPort;
        
    /*发送失败则断开和客户端的连接*/
    lRet = cpss_com_tcp_cps__oams_connect_info_send(
        (UINT8*)&stTcpMsg,CPSS_COM_TCP_CONN_IND_MSG,CPSS_COM_TCP_SERVER_FLAG);
    if(CPSS_OK != lRet)
    {
        g_stCpssTcpErrStat.ulConnectSendFailNum++ ;
            
        closesocket(lSClient) ; 

        cpss_mem_memset(pstTcpDrvInfo,0,sizeof(CPSS_COM_DRV_TCP_T)) ;

        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
            "send tcp connect notify message failed!");
        
        return CPSS_ERROR;
    } 
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_listen_task_reg
* 功    能: DC监听任务注册函数
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述

* 函数返回: 
*          无
* 说    明:                                            
*******************************************************************************/
INT32 cpss_com_tcp_listen_task_reg()
{
    INT32 lRet;
    lRet = cpss_vos_ntask_register(
              CPSS_NORMAL_TASK_TYPE_ACCEPT,
              cpss_com_tcp_listen_task_init, 
              cpss_com_tcp_listen_task_block,
              cpss_com_tcp_listen_task_dealing,
              g_ulCpssComTcpListenTaskPList,
              CPSS_TCPLISTEN_TASKNAME);
    if ( lRet != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                "Tcp listen task register failed! errCode = %#x \n", lRet);
        return (CPSS_ERROR);
    }
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_com_dc_listen_task_init
* 功    能: DC监听任务初始化
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述

* 函数返回: 
*          无
* 说    明:                                            
*******************************************************************************/
INT32 cpss_com_dc_listen_task_init()
{
    CPSS_COM_DRV_T  *pstDrv=cpss_com_drv_find(CPSS_COM_DRV_DC_SERVER);
    
    /* 等待创建互斥量 */
    while(CPSS_VOS_MUTEXD_INVALID == g_ulCpssComDrvTcpMutex)
    {
        cpss_vos_task_delay(200);
    }
    
    /* 对端口进行监听 */
    listen(pstDrv->ulDrvD,8);
    g_ulCpssComDcListenTaskPList[0] = pstDrv->ulDrvD;
    
    return (CPSS_OK); 
}

/*******************************************************************************
* 函数名称: cpss_com_dc_listen_task_block
* 功    能: DC监听任务阻塞函数
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述

* 函数返回: 
*          无
* 说    明:                                            
*******************************************************************************/
INT32 cpss_com_dc_listen_task_block(UINT32 *pulParamList)
{
    CPSS_COM_SOCK_ADDR_T stClient = {0};
    INT32  lAddrSize ;
    INT32  lSClient;
    lAddrSize = sizeof(stClient);
    lSClient = accept(pulParamList[0], (struct sockaddr *)&stClient,
        &lAddrSize);
    memcpy(&pulParamList[1], &stClient, lAddrSize);
    return (lSClient);
}

/*******************************************************************************
* 函数名称: cpss_com_dc_listen_task_dealing
* 功    能: DC监听任务处理过程
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述

* 函数返回: 
*          无
* 说    明:                                            
*******************************************************************************/
INT32 cpss_com_dc_listen_task_dealing(INT32 lSClient, UINT32 *pulParamList)
{
    UINT32 ulSockAttr = 1; /* TRUE表示NONE BLOCK */    
    CPSS_COM_TCP_CONN_IND_MSG_T stTcpMsg = {0};
    CPSS_COM_DRV_TCP_T* pstTcpDrvInfo = NULL ;
    INT32 lRet;
    CPSS_COM_TCP_LINK_T stTcpLink ;
    CPSS_COM_SOCK_ADDR_T *pstClient = (CPSS_COM_SOCK_ADDR_T*)&pulParamList[1];
    CPSS_COM_DRV_T  *pstDrv=cpss_com_drv_find(CPSS_COM_DRV_DC_SERVER);
    
    
    if(INVALID_SOCKET == lSClient)
    {
        g_stCpssTcpErrStat.ulAcceptErrNum++ ;
        
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
            "dc task, accept failed!");
        return CPSS_ERROR;
    }
    
    /*设置socket为非阻塞socket*/
#ifdef CPSS_VOS_WINDOWS
    ioctlsocket(lSClient, FIONBIO, &ulSockAttr);
#elif defined CPSS_VOS_LINUX
    fcntl(lSClient, F_SETFL, 0);
#else
    ioctl(lSClient,FIONBIO,(INT32)&ulSockAttr);
#endif
    stTcpLink.ulIP = cpss_ntohl(pstClient->sin_addr.s_addr);
    stTcpLink.usTcpPort = cpss_ntohs(pstClient->sin_port);
    
    if(NULL != cpss_com_tcp_server_link_find(&stTcpLink))
    {
        closesocket(lSClient) ;
        return CPSS_ERROR;
    }
    
    /*寻找空的链路位置*/
    pstTcpDrvInfo = cpss_com_tcp_server_free_link_find(CPSS_COM_TCP_DC_FLAG) ;
    if(pstTcpDrvInfo == NULL)
    {
        g_stCpssTcpErrStat.ulNoFreeSpaceNum++ ;
        
        closesocket(lSClient) ;
        
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
            "have no space for new dc connect!");  
        return CPSS_ERROR;
    }
    
    /*保存客户端连接信息*/
    pstTcpDrvInfo->ulSClient=lSClient;
    pstTcpDrvInfo->stTcpLink.ulIP=stTcpLink.ulIP;
    pstTcpDrvInfo->stTcpLink.usTcpPort=stTcpLink.usTcpPort;
    pstTcpDrvInfo->ulServerFlag = CPSS_COM_TCP_DC_FLAG ;
    
    /*由于网元直连的接收可能需要多次，所以申请接收内存*/
    pstTcpDrvInfo->stRecvBufM.ulRecvBufLen = 0 ;
    pstTcpDrvInfo->stRecvBufM.ulDataFlag   = CPSS_TCP_RECV_SEND_OK ;
    
    pstTcpDrvInfo->stRecvBufM.pucDataBuf   = cpss_mem_malloc(CPSS_COM_DC_RECV_MAX_LEN) ;
    if(pstTcpDrvInfo->stRecvBufM.pucDataBuf == NULL)
    {
        /*释放TCP链路*/
        cpss_com_tcp_link_disconnect(&pstTcpDrvInfo->stTcpLink) ;
        
        /*返回失败*/
        return CPSS_ERROR ;
    }

    /* 向oams发送消息CPSS_COM_TCP_CONN_IND_MSG */
    stTcpMsg.ulIP      = pstTcpDrvInfo->stTcpLink.ulIP;
    stTcpMsg.usTcpPort = pstTcpDrvInfo->stTcpLink.usTcpPort;
    
    /*发送失败则断开和客户端的连接*/
    lRet = cpss_com_tcp_cps__oams_connect_info_send(
        (UINT8*)&stTcpMsg,CPSS_COM_TCP_CONN_IND_MSG,CPSS_COM_TCP_DC_FLAG);
    if(CPSS_OK != lRet)
    {
        g_stCpssTcpErrStat.ulConnectSendFailNum++ ;
        
        closesocket(lSClient) ; 
        
        cpss_mem_memset(pstTcpDrvInfo,0,sizeof(CPSS_COM_DRV_TCP_T)) ;
        
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
            "send dc connect notify message failed!");
        
        return CPSS_ERROR;
    } 
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_listen_task_reg
* 功    能: DC监听任务注册函数
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述

* 函数返回: 
*          无
* 说    明:                                            
*******************************************************************************/
INT32 cpss_com_dc_listen_task_reg()
{
    INT32 lRet;
    lRet = cpss_vos_ntask_register(
        CPSS_NORMAL_TASK_TYPE_ACCEPT,
        cpss_com_dc_listen_task_init, 
        cpss_com_dc_listen_task_block,
        cpss_com_dc_listen_task_dealing,
        g_ulCpssComDcListenTaskPList,
        CPSS_DC_LISTEN_TASKNAME);
    if ( lRet != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
            "Dc listen task register failed! errCode = %#x \n", lRet);
        return (CPSS_ERROR);
    }
    return (CPSS_OK);
}

VOID cpss_com_drv_tcp_listen_task(VOID)
{
    INT32  lAddrSize ;
    UINT32 ulSClient;
    UINT32 ulRunTrack = 0 ;
    CPSS_COM_SOCK_ADDR_T stClient = {0};
    CPSS_COM_TCP_CONN_IND_MSG_T stTcpMsg = {0};
    INT32 lRet;
    UINT32 ulSockAttr = 1; /* TRUE表示NONE BLOCK */    
    CPSS_COM_DRV_TCP_T* pstTcpDrvInfo = NULL ;
    CPSS_COM_TCP_LINK_T stTcpLink ;

    CPSS_COM_DRV_T  *pstDrv=cpss_com_drv_find(CPSS_COM_DRV_TCP_SERVER);

    /* 等待创建互斥量 */
    while(CPSS_VOS_MUTEXD_INVALID == g_ulCpssComDrvTcpMutex)
    {
        cpss_vos_task_delay(200);
    }
    
    /* 对端口进行监听 */
    listen(pstDrv->ulDrvD,8);

    while(TRUE)
    {
        cpss_kw_set_task_stop() ;
        
        lAddrSize = sizeof(stClient);
        ulSClient = accept(pstDrv->ulDrvD, (struct sockaddr *)&stClient,
                           &lAddrSize);
        
        cpss_kw_set_task_run() ;
              
        if(INVALID_SOCKET == ulSClient)
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                      "tcp task, accept failed!");
            continue;
        }

        /*设置socket为非阻塞socket*/
#ifdef CPSS_VOS_WINDOWS
        ioctlsocket(ulSClient, FIONBIO, &ulSockAttr);
#elif defined CPSS_VOS_LINUX
        fcntl(ulSClient, F_SETFL, 0);
#else
        ioctl(ulSClient,FIONBIO,(INT32)&ulSockAttr);
#endif
        stTcpLink.ulIP = cpss_ntohl(stClient.sin_addr.s_addr);
        stTcpLink.usTcpPort = cpss_ntohs(stClient.sin_port);

        if(NULL != cpss_com_tcp_server_link_find(&stTcpLink))
        {
            closesocket(ulSClient) ;
            continue ;
        }
        
        /*寻找空的链路位置*/
        pstTcpDrvInfo = cpss_com_tcp_server_free_link_find(CPSS_COM_TCP_SERVER_FLAG) ;
        if(pstTcpDrvInfo == NULL)
            {                
            closesocket(ulSClient) ; 

            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                "have no space for new connect!");  
                continue;
            }

        /*保存客户端连接信息*/
        pstTcpDrvInfo->ulSClient=ulSClient;
        pstTcpDrvInfo->stTcpLink.ulIP=stTcpLink.ulIP;
        pstTcpDrvInfo->stTcpLink.usTcpPort=stTcpLink.usTcpPort;
            
            /* 向oams发送消息CPSS_COM_TCP_CONN_IND_MSG */
        stTcpMsg.ulIP      = pstTcpDrvInfo->stTcpLink.ulIP;
        stTcpMsg.usTcpPort = pstTcpDrvInfo->stTcpLink.usTcpPort;
            
        /*发送失败则断开和客户端的连接*/
        lRet = cpss_com_tcp_cps__oams_connect_info_send((UINT8*)&stTcpMsg,CPSS_COM_TCP_CONN_IND_MSG,CPSS_COM_TCP_SERVER_FLAG);
        if(CPSS_OK != lRet)
        {
            closesocket(ulSClient) ; 

            cpss_mem_memset(pstTcpDrvInfo,0,sizeof(CPSS_COM_DRV_TCP_T)) ;

            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                "send tcp connect notify message failed!");
            
            continue ;
        }
    }
}   

/*******************************************************************************
* 函数名称: cpss_com_tcp_recv_task_init
* 功    能: tcp接收任务初始化
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述

* 函数返回: 
*          无
* 说    明: 
*******************************************************************************/
#define CPSS_TCP_RECV_MAX_LEN  (64*1024) 

INT32 cpss_com_tcp_recv_task_init()
{
    /* 等待创建互斥量 */
    while(CPSS_VOS_MUTEXD_INVALID == g_ulCpssComDrvTcpMutex)
    {
        cpss_vos_task_delay(200);
    }

    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_recv_task_block
* 功    能: tcp接收任务阻塞功能
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述

* 函数返回: 
*          无
* 说    明: 
*******************************************************************************/
INT32 cpss_com_tcp_recv_task_block(UINT32 *pulParamList)
{
    UINT32 ulIdx;

    struct timeval stSelTimeout ;
    UINT32 ulFdMax ;
        
    INT32 lNum;
       
    ulFdMax = 0;
    stSelTimeout.tv_sec  = 0 ;
    stSelTimeout.tv_usec = 100 ;

    /* 文件描述符集合清零 */
    FD_ZERO(&g_stCpssReadFds);   

    /* 查找已经建立TCP连接的客户 */
    for(ulIdx=0; ulIdx<CPSS_COM_DRV_TCP_NUM; ulIdx++)
    {
        if(g_stCpssTcpMan.astServerTcpLink[ulIdx].ulSClient != 0)
        {
            FD_SET(g_stCpssTcpMan.astServerTcpLink[ulIdx].ulSClient,&g_stCpssReadFds);

            /*计算最大的socket的值*/
            if(ulFdMax < g_stCpssTcpMan.astServerTcpLink[ulIdx].ulSClient)
            {
                ulFdMax = g_stCpssTcpMan.astServerTcpLink[ulIdx].ulSClient ;
            }
        }
    }

    /* 查找已经建立DC连接的客户 */
    for(ulIdx=0; ulIdx<CPSS_COM_DRV_DC_TCP_NUM; ulIdx++)
    {
        if(g_stCpssTcpMan.astServerDcLink[ulIdx].ulSClient != 0)
        {
            FD_SET(g_stCpssTcpMan.astServerDcLink[ulIdx].ulSClient,&g_stCpssReadFds);
            
            /*计算最大的socket的值*/
            if(ulFdMax < g_stCpssTcpMan.astServerDcLink[ulIdx].ulSClient)
            {
                ulFdMax = g_stCpssTcpMan.astServerDcLink[ulIdx].ulSClient ;
            }
        }
    }

    /* 查看有哪些连接接收到数据 */
    lNum=select(ulFdMax+1,&g_stCpssReadFds,(fd_set *)NULL,(fd_set *)NULL,&stSelTimeout);       

    return (lNum);
}


/*******************************************************************************
* 函数名称: cpss_com_dc_recv_data_check
* 功    能: 查找接收数据中的空格
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述

* 函数返回: 
*          无
* 说    明: 
*******************************************************************************/
#define CPSS_DC_ENTER_KEY_DOWN     0
#define CPSS_DC_ENTER_KEY_UNTOUCH  1

INT32 cpss_com_dc_recv_data_check
(
 UINT8 *pucRecvBuf,
 UINT32* pulRealLen,
 UINT32* pulEnterKeyNum,        /*回车符的次数*/
 UINT32 ulRecvBytes
)
{
    UINT32 ulNum  = 0 ;
    UINT32 ulLoop = 0 ;
    UINT32 ulEnterKeyNum = 0 ;

    /*查找空格字符*/
    for(ulLoop=0;ulLoop<ulRecvBytes;ulLoop++)
    {
        if((pucRecvBuf[ulLoop] == '\n')||(pucRecvBuf[ulLoop] == '\r'))
        {
            break ;
        }
        ulNum++ ;
    }

    /*如果没有找到空格*/
    if(ulNum == ulRecvBytes)
    {
        *pulRealLen = ulNum ;
        return CPSS_DC_ENTER_KEY_UNTOUCH ;
    }
    else
    {
        /*统计回车符的个数*/
        for(ulLoop=ulNum;ulLoop<ulRecvBytes;ulLoop++)
        {
            if((pucRecvBuf[ulLoop] == '\n')||(pucRecvBuf[ulLoop] == '\r'))
            {
                ulEnterKeyNum++ ;
            }
            else
            {
                break ;
            }
        }
        
        *pulRealLen = ulNum ;
        *pulEnterKeyNum = ulEnterKeyNum ;

        return CPSS_DC_ENTER_KEY_DOWN ;
    }
}

/*******************************************************************************
* 函数名称: cpss_com_drv_dc_data_recv
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述

* 函数返回: 
*          无
* 说    明: 
*******************************************************************************/
CPSS_COM_TRUST_HEAD_T* cpss_com_drv_dc_data_recv
(
CPSS_COM_DRV_TCP_T *pstDrvTcp
)
{
    INT32 lRecvLen ;
    INT32 lRet ;
    CPSS_COM_TCP_LINK_T stTcpLink ;
    CPSS_COM_TCP_DISCONN_IND_MSG_T stDisconnMsg ;
    CPSS_COM_TRUST_HEAD_T* pstTrustHdr = NULL ;
    CPSS_COM_MSG_HEAD_T* pstDispHdr ;
    UINT32 ulDcRecvOnce = 0 ;
    UINT32 ulEnterKeyNum = 0 ;
    INT32  lErrno ;
    UINT8  aucDcRecvBuf[CPSS_COM_DC_RECV_MAX_LEN] ;

    /*设置接收结构体*/
    if(pstDrvTcp->stRecvBufM.ulDataFlag == CPSS_TCP_PKG_FINISH)
    {
        pstDrvTcp->stRecvBufM.ulDataFlag   = CPSS_TCP_PKG_NOT_FINISH ;
        pstDrvTcp->stRecvBufM.ulRecvBufLen = 0 ;
    }

    /*循环读取数据*/
    while(1)
    {
        /*接收网元直连数据*/
        lRecvLen = recv(pstDrvTcp->ulSClient,aucDcRecvBuf,CPSS_TCP_RECV_MAX_LEN,0) ;
    
        if(lRecvLen < 0)
        {
            lRecvLen = 0 ;
        }
    
        /*如果接收数据失败则断开连接*/
        if(((lRecvLen <= 0)&&(pstDrvTcp->stRecvBufM.ulRecvBufLen == 0))
        ||((lRecvLen <= 0)&&(pstDrvTcp->stRecvBufM.ulRecvBufLen == ulDcRecvOnce)))
        {
            /*判断是否结收数据完成*/
#ifdef CPSS_VOS_WINDOWS
            lErrno = WSAGetLastError() ;
#else
            lErrno == 10035 ;
#endif
            if(lErrno == 10035)
            {
                return NULL ;
            }
                        
            g_stCpssTcpErrStat.ulRecvErrNum++ ;
        
            stTcpLink.ulIP      = pstDrvTcp->stTcpLink.ulIP ;
            stTcpLink.usTcpPort = pstDrvTcp->stTcpLink.usTcpPort ;
        
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                "dc task received error.IP(%x),port(%d)",
                stTcpLink.ulIP,stTcpLink.usTcpPort);
        
            /*断开socket连接,清除socket表示*/
            cpss_com_tcp_server_socket_free(pstDrvTcp->ulSClient) ;
        
            /*向OAMS发送disconnect消息*/
            stDisconnMsg.ulIP      = pstDrvTcp->stTcpLink.ulIP ;
            stDisconnMsg.usTcpPort = pstDrvTcp->stTcpLink.usTcpPort ;
        
            lRet = cpss_com_tcp_cps__oams_connect_info_send(
                (UINT8*)&stDisconnMsg,CPSS_COM_TCP_DISCONN_IND_MSG,CPSS_COM_TCP_DC_FLAG);
            if(CPSS_OK != lRet)
            {
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                    "send dc disconnect indicate message failed!");
                cpss_com_tcp_link_disconnect(&stTcpLink) ;
            }
        
            return NULL;
        }
        else
        {
            if(pstDrvTcp->stRecvBufM.ulRecvBufLen == 0)
            {
                /*对接收到的网元直连数据进行检测*/
                lRet = cpss_com_dc_recv_data_check(aucDcRecvBuf,&ulDcRecvOnce,&ulEnterKeyNum,(UINT32)lRecvLen) ;                
            
                if(ulDcRecvOnce == 0)
                {
                    if(ulEnterKeyNum == (UINT32)lRecvLen)
                    {
                        continue ;
                    }
                }

                if(ulDcRecvOnce == 0)
                {
                    cpss_mem_memcpy(pstDrvTcp->stRecvBufM.pucDataBuf,aucDcRecvBuf+ulEnterKeyNum,((UINT32)lRecvLen - ulEnterKeyNum)) ;
                        pstDrvTcp->stRecvBufM.ulRecvBufLen = (UINT32)lRecvLen - ulEnterKeyNum ;
                        
                    /*对接网元直连数据进行检测*/
                    lRet = cpss_com_dc_recv_data_check(pstDrvTcp->stRecvBufM.pucDataBuf,
                        &ulDcRecvOnce,&ulEnterKeyNum,pstDrvTcp->stRecvBufM.ulRecvBufLen) ;    
                }
                else
                {
                    cpss_mem_memcpy(pstDrvTcp->stRecvBufM.pucDataBuf,aucDcRecvBuf,((UINT32)lRecvLen)) ;
                        pstDrvTcp->stRecvBufM.ulRecvBufLen = (UINT32)lRecvLen ;

                }
            
            }
            else
            {
                cpss_mem_memcpy(pstDrvTcp->stRecvBufM.pucDataBuf+pstDrvTcp->stRecvBufM.ulRecvBufLen,aucDcRecvBuf,(UINT32)lRecvLen) ;
                pstDrvTcp->stRecvBufM.ulRecvBufLen = pstDrvTcp->stRecvBufM.ulRecvBufLen + (UINT32)lRecvLen ;

                /*对接网元直连数据进行检测*/
                lRet = cpss_com_dc_recv_data_check(pstDrvTcp->stRecvBufM.pucDataBuf,
                    &ulDcRecvOnce,&ulEnterKeyNum,pstDrvTcp->stRecvBufM.ulRecvBufLen) ;                
            }

            /*如果不是完整的命令行直接返回*/
            if(lRet == CPSS_DC_ENTER_KEY_UNTOUCH)
            {
                continue ;
            }
            
            /*组织网元直连数据进行发送*/
            pstTrustHdr = cpss_com_mem_alloc(ulDcRecvOnce+1) ;
            if(pstTrustHdr == NULL)
            {
                return NULL ;
            }
            else
            {
                pstTrustHdr->ucPType = COM_SLID_PTYPE_INCREDIBLE ;
            
                pstDispHdr         = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr) ;
                pstDispHdr->pucBuf = (UINT8*)CPSS_DISP_HDR_TO_USR_HDR(pstDispHdr) ;
         
                /*拷贝用户数据*/
                cpss_mem_memcpy(pstDispHdr->pucBuf,
                    pstDrvTcp->stRecvBufM.pucDataBuf,ulDcRecvOnce) ;

                /*设置通信分发头*/
                pstDispHdr->ucAckFlag = CPSS_COM_NOT_ACK ;
                pstDispHdr->ucPriFlag = CPSS_COM_PRIORITY_NORMAL ;
                pstDispHdr->ucShareFlag = CPSS_COM_MEM_SHARE ;
                pstDispHdr->ulLen       = ulDcRecvOnce + 1 ;
                pstDispHdr->ulMsgId   = CPSS_COM_TCP_MML_USER_DATA_MSG ;

                /*填充目的地址和纤程号*/
                cpss_com_logic_addr_get(
                    &pstDispHdr->stDstProc.stLogicAddr,&pstDispHdr->stDstProc.ulAddrFlag) ;
                pstDispHdr->stDstProc.ulPd = CPS__OAMS_PD_M_DLM_MSGDSP_PROC ;

                /*填充源地址*/
                pstDispHdr->stSrcProc.stLogicAddr = pstDrvTcp->stLogicAddr ;

                /*判断是否还有数据*/
                if(pstDrvTcp->stRecvBufM.ulRecvBufLen > (ulDcRecvOnce + ulEnterKeyNum))
                {
                    /*重新拷贝数据*/
                   cpss_mem_memcpy(pstDrvTcp->stRecvBufM.pucDataBuf,
                       (pstDrvTcp->stRecvBufM.pucDataBuf+ulDcRecvOnce+ulEnterKeyNum),
                       (pstDrvTcp->stRecvBufM.ulRecvBufLen-ulDcRecvOnce-ulEnterKeyNum)) ;
                   pstDrvTcp->stRecvBufM.ulRecvBufLen = pstDrvTcp->stRecvBufM.ulRecvBufLen-ulDcRecvOnce-ulEnterKeyNum ;
                }
                else
                {
                    pstDrvTcp->stRecvBufM.ulDataFlag = CPSS_TCP_PKG_FINISH ;
                }
                
                /*在接收的字符串最后增加行结束符*/
                *(pstDispHdr->pucBuf + ulDcRecvOnce) = '\0' ;
                              
                return pstTrustHdr ;
            }
        }
    }
    return pstTrustHdr ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_recv_task_dealing
* 功    能: tcp接收任务处理功能
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* 函数返回: 
*          无
* 说    明: 负责接收tcp驱动方式的消息，然后进行处理，或者进行转发
*******************************************************************************/
INT32 cpss_com_tcp_recv_task_dealing(INT32 lNum, UINT32 *pulParamList)
{
    UINT32 ulIdx;
    CPSS_COM_MSG_HEAD_T* pstDispHdr ;
    CPSS_COM_DRV_TCP_T *pstDrvTcp;
    CPSS_COM_DRV_T     *pstDrv=cpss_com_drv_find(CPSS_COM_DRV_TCP_SERVER);
    CPSS_COM_TRUST_HEAD_T* pstTrustHdr ;
        
    UINT32  ulLinkId;
    UINT8 *pucRecvBuf = NULL ;

    if(lNum <= 0)
    {                  
        cpss_vos_task_delay(200);
        return CPSS_ERROR;
    }

    /*接收普通TCP连接的数据*/
    for(ulIdx=0; ulIdx<CPSS_COM_DRV_TCP_NUM; ulIdx++)
    {
        pstDrvTcp = &g_stCpssTcpMan.astServerTcpLink[ulIdx];

        if(0 == pstDrvTcp->ulSClient)
        {
            continue;
        }
        
        if(!FD_ISSET(pstDrvTcp->ulSClient,&g_stCpssReadFds))
        {
            continue;
        }

        /*接收完整的一包数据*/
        if(cpss_com_drv_tcp_msg_recv(pstDrvTcp,&pucRecvBuf)!= CPSS_OK)
        {
            continue;
        }

        ulLinkId = (UINT32)pstDrvTcp;

        /*设置内存标志为共享内存*/
        pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pucRecvBuf) ;
        pstDispHdr->ucShareFlag = CPSS_COM_MEM_SHARE ;

        /* 把收到的报文交给驱动适配层来处理 */
        if((pstDrv->pfRead != NULL) && 
            (CPSS_OK == pstDrv->pfRead(CPSS_COM_DRV_TCP_SERVER,ulLinkId,pucRecvBuf,0)))
        {
            continue;
        }
    }

    /* 接收一个网元直连接口的数据 */
    for(ulIdx=0; ulIdx<CPSS_COM_DRV_DC_TCP_NUM; ulIdx++)
    {
        pstDrvTcp = &g_stCpssTcpMan.astServerDcLink[ulIdx];

        if(0 == pstDrvTcp->ulSClient)
        {
            continue;
        }
        
        if(!FD_ISSET(pstDrvTcp->ulSClient,&g_stCpssReadFds))
        {
            continue;
        }

        /*网元直连接口不进行数据的连续接收*/
        while(1)
        {            
            pstTrustHdr = cpss_com_drv_dc_data_recv(pstDrvTcp) ;
            if(pstTrustHdr == NULL)
            {
                break ;
            }

            ulLinkId = (UINT32)pstDrvTcp;

            /* 把收到的报文交给驱动适配层来处理 */
            if((pstDrv->pfRead != NULL) && 
            (CPSS_OK == pstDrv->pfRead(CPSS_COM_DRV_DC_SERVER,ulLinkId,(UINT8*)pstTrustHdr,0)))
            {
                continue;
            }
        }
    }
    
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_recv_task_reg
* 功    能: tcp接收任务注册功能
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述

* 函数返回: 
*          无
* 说    明: 
*******************************************************************************/
INT32 cpss_com_tcp_recv_task_reg()
{
    INT32 lRet;
    lRet = cpss_vos_ntask_register(
              CPSS_NORMAL_TASK_TYPE_SELECT,
              cpss_com_tcp_recv_task_init, 
              cpss_com_tcp_recv_task_block,
	      cpss_com_tcp_recv_task_dealing,
              g_ulCpssComTcpRecvTaskPList,
              CPSS_TCPRECV_TASKNAME);
    if ( lRet != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                "Tcp recv task register failed! errCode = %#x \n", lRet);
        return (CPSS_ERROR);
    }

    return (CPSS_OK);
    
}

VOID cpss_com_drv_tcp_recv_task(VOID)
{    
#define CPSS_TCP_RECV_MAX_LEN  (64*1024)

    fd_set stReadFds;
    UINT32 ulIdx;

    struct timeval stSelTimeout ;
    UINT32 ulFdMax ;
    
    CPSS_COM_MSG_HEAD_T* pstDispHdr ;
    
    CPSS_COM_DRV_TCP_T *pstDrvTcp;
    CPSS_COM_DRV_T     *pstDrv=cpss_com_drv_find(CPSS_COM_DRV_TCP_SERVER);
        
    INT32 lResult,lNum;
    UINT32  ulLinkId;
    UINT8 *pucRecvBuf = NULL ;

    stSelTimeout.tv_sec  = 0 ;
    stSelTimeout.tv_usec = 100 ;

    /* 等待创建互斥量 */
    while(CPSS_VOS_MUTEXD_INVALID == g_ulCpssComDrvTcpMutex)
    {
        cpss_vos_task_delay(200);
    }
    
    while(TRUE)
    {
        lResult   = CPSS_OK;
        ulFdMax = 0;
        
        /* 文件描述符集合清零 */
        FD_ZERO(&stReadFds);   

        /* 查找已经建立连接的客户 */
        for(ulIdx=0; ulIdx<CPSS_COM_DRV_TCP_NUM; ulIdx++)
        {
            if(g_stCpssTcpMan.astServerTcpLink[ulIdx].ulSClient != 0)
            {
                FD_SET(g_stCpssTcpMan.astServerTcpLink[ulIdx].ulSClient,&stReadFds);

                /*计算最大的socket的值*/
                if(ulFdMax < g_stCpssTcpMan.astServerTcpLink[ulIdx].ulSClient)
                {
                    ulFdMax = g_stCpssTcpMan.astServerTcpLink[ulIdx].ulSClient ;
                }
            }
        }

        cpss_kw_set_task_stop() ;

        /* 查看有哪些连接接收到数据 */
        lNum=select(ulFdMax+1,&stReadFds,(fd_set *)NULL,(fd_set *)NULL,&stSelTimeout);       

        cpss_kw_set_task_run() ;
        /* 如果没有接收到任何数据,则把控制权移交给其它任务 */
        if(lNum <= 0)
        {                  
            cpss_vos_task_delay(200);
            continue;
        }

        /* 接收一个连接的数据 */
        for(ulIdx=0; ulIdx<CPSS_COM_DRV_TCP_NUM; ulIdx++)
        {
            pstDrvTcp = &g_stCpssTcpMan.astServerTcpLink[ulIdx];

            if(0 == pstDrvTcp->ulSClient)
            {
                continue;
            }
            
            if(!FD_ISSET(pstDrvTcp->ulSClient,&stReadFds))
            {
                continue;
            }

            /*接收完整的一包数据*/
            if(cpss_com_drv_tcp_msg_recv(pstDrvTcp,&pucRecvBuf)!= CPSS_OK)
            {
                continue ;
            }

            ulLinkId = (UINT32)pstDrvTcp;

            /*设置内存标志为共享内存*/
            pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pucRecvBuf) ;
            pstDispHdr->ucShareFlag = CPSS_COM_MEM_SHARE ;

            /* 把收到的报文交给驱动适配层来处理 */
            if((pstDrv->pfRead != NULL) && (CPSS_OK == pstDrv->pfRead(CPSS_COM_DRV_TCP_SERVER,ulLinkId,pucRecvBuf,0)))
            {
                continue;
            }
        }        
    }        
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_link_bind
* 功    能: 提供给OAMS使用，用于通知CPSS某个MC/LDT的TCP连接已经过了鉴权，绑定
*           一个逻辑地址到该TCP链路上，该链路可以用于正常的消息收发了。
*           在绑定之前，该TCP连接仅用于接收鉴权请求消息。
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* pstTcpLink    CPSS_COM_TCP_LINK_T*     输入        TCP连接描述信息，包括IP+PORT
* pstLogicAddr  CPSS_COM_LOGIC_ADDR_T*   输入       在该连接上绑定的逻辑地址
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*******************************************************************************/
INT32 cpss_com_tcp_link_bind 
(
    CPSS_COM_TCP_LINK_T *pstTcpLink, 
    CPSS_COM_LOGIC_ADDR_T *pstLogicAddr,
    UINT32 ulRouteAllowFlag
)
{
    CPSS_COM_DRV_TCP_T *pstTcpDrv;
    CPSS_COM_DRV_T *pstDrv;
        
    pstDrv = cpss_com_drv_find(CPSS_COM_DRV_TCP_SERVER);

    /*查找server表*/
    pstTcpDrv = cpss_com_tcp_server_link_find(pstTcpLink) ;
    if(pstTcpDrv == NULL)
    {
        return CPSS_ERR_COM_TCP_LINK_NOT_FIND ;
    }
        
    /* 保存链路映射关系 */
    pstTcpDrv->stLogicAddr=*pstLogicAddr;
    pstTcpDrv->ulRouteAllowFlag = ulRouteAllowFlag ;

    return CPSS_OK ;
        }

/*******************************************************************************
* 函数名称: cpss_com_tcp_link_bind
* 功    能: 
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* pstTcpLink    CPSS_COM_TCP_LINK_T*     输入        TCP连接描述信息，包括IP+PORT
* pstLogicAddr  CPSS_COM_LOGIC_ADDR_T*   输入       在该连接上绑定的逻辑地址
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*******************************************************************************/
INT32 cpss_com_tcp_link_sim_bind
(
 CPSS_COM_TCP_LINK_T *pstTcpLink, 
 CPSS_COM_LOGIC_ADDR_T *pstLogicAddr,
 UINT32 ulRouteAllowFlag
 )
        {
    CPSS_COM_DRV_TCP_T *pstTcpDrv;
    CPSS_COM_DRV_T *pstDrv;
    
    pstDrv = cpss_com_drv_find(CPSS_COM_DRV_TCP_SERVER);
    
    /*查找server表*/
    pstTcpDrv = cpss_com_tcp_client_link_find(pstTcpLink) ;
    if(pstTcpDrv == NULL)
    {
        return CPSS_ERR_COM_TCP_LINK_NOT_FIND ;
        }
        
        /* 保存链路映射关系 */
        pstTcpDrv->stLogicAddr=*pstLogicAddr;
        pstTcpDrv->ulRouteAllowFlag = ulRouteAllowFlag ;

        return CPSS_OK;
    }


/*******************************************************************************
* 函数名称: cpss_com_tcp_link_disconnect
* 功    能: 提供给OAMS使用，用于断开和OMC/LDT的TCP连接。
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* pstTcpLink    CPSS_COM_TCP_LINK_T*     输入        TCP连接描述信息，包括IP+PORT
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*******************************************************************************/
INT32 cpss_com_tcp_link_disconnect 
(
    CPSS_COM_TCP_LINK_T *pstTcpLink
)
{
    UINT32 ulIndex;
    UINT32 ulLoop ;
    UINT32 ulSocket ;
        
    /*合法性检查*/
    if(NULL == pstTcpLink)
    {
        return CPSS_ERROR ;
    }

    /*查询tcp的server表*/
    for(ulIndex=0;ulIndex<CPSS_COM_DRV_TCP_NUM;ulIndex++)
    {
        if((g_stCpssTcpMan.astServerTcpLink[ulIndex].stTcpLink.ulIP == pstTcpLink->ulIP)&&
            (g_stCpssTcpMan.astServerTcpLink[ulIndex].stTcpLink.usTcpPort == pstTcpLink->usTcpPort))
        {
            ulSocket = g_stCpssTcpMan.astServerTcpLink[ulIndex].ulSClient ;

            /*释放tcp链路接收内存*/
            if(g_stCpssTcpMan.astServerTcpLink[ulIndex].stRecvBufM.pucDataBuf != NULL)
            {
                cpss_mem_free(g_stCpssTcpMan.astServerTcpLink[ulIndex].stRecvBufM.pucDataBuf) ;
                g_stCpssTcpMan.astServerTcpLink[ulIndex].stRecvBufM.pucDataBuf = NULL ;
            }

            /*清零TCP链路信息*/
            cpss_mem_memset(&g_stCpssTcpMan.astServerTcpLink[ulIndex],
                0,sizeof(CPSS_COM_DRV_TCP_T)) ;
            g_stCpssTcpMan.ulServerTcpNum-- ;
            
            if(ulSocket != 0)
            {
#ifdef CPSS_FUNBRD_MC
                cpss_com_tcp_del_socket_req_send(ulSocket) ;
#else
                closesocket(ulSocket) ;
#endif

            }
            
            return CPSS_OK ;
        }
    }

    /*查询网元直连表*/
    for(ulIndex=0;ulIndex<CPSS_COM_DRV_DC_TCP_NUM;ulIndex++)
    {
        if((g_stCpssTcpMan.astServerDcLink[ulIndex].stTcpLink.ulIP == pstTcpLink->ulIP)&&
            (g_stCpssTcpMan.astServerDcLink[ulIndex].stTcpLink.usTcpPort == pstTcpLink->usTcpPort))
        {
            if(g_stCpssTcpMan.astServerDcLink[ulIndex].ulSClient != 0)
            {
#ifdef CPSS_FUNBRD_MC
                cpss_com_tcp_del_socket_req_send(g_stCpssTcpMan.astServerDcLink[ulIndex].ulSClient) ;
#else
                closesocket(g_stCpssTcpMan.astServerDcLink[ulIndex].ulSClient) ;
#endif
            }
            
            /*释放tcp链路接收内存*/
            if(g_stCpssTcpMan.astServerDcLink[ulIndex].stRecvBufM.pucDataBuf != NULL)
            {
                cpss_mem_free(g_stCpssTcpMan.astServerDcLink[ulIndex].stRecvBufM.pucDataBuf) ;
                g_stCpssTcpMan.astServerDcLink[ulIndex].stRecvBufM.pucDataBuf = NULL ;
            }
            
            /*清零TCP链路信息*/
            cpss_mem_memset(&g_stCpssTcpMan.astServerDcLink[ulIndex],
                0,sizeof(CPSS_COM_DRV_TCP_T)) ;
            g_stCpssTcpMan.ulServerTcpNum-- ;
            return CPSS_OK ;
        }
    }
    
    /*查询tcp的client表*/
    for(ulIndex=0; ulIndex<CPSS_COM_DRV_CLIENT_TCP_NUM; ulIndex++)
    {
        if((g_stCpssTcpMan.astClientTcpLink[ulIndex].stTcpLink.ulIP == pstTcpLink->ulIP)&&
            (g_stCpssTcpMan.astClientTcpLink[ulIndex].stTcpLink.usTcpPort == pstTcpLink->usTcpPort))
        {
            if(g_stCpssTcpMan.astClientTcpLink[ulIndex].ulSClient != 0)
            {
#ifdef CPSS_FUNBRD_MC
                cpss_com_tcp_del_socket_req_send(g_stCpssTcpMan.astClientTcpLink[ulIndex].ulSClient) ;
#else
                closesocket(g_stCpssTcpMan.astClientTcpLink[ulIndex].ulSClient) ;
#endif
            }

            /*释放tcp链路接收内存*/
            if(g_stCpssTcpMan.astClientTcpLink[ulIndex].stRecvBufM.pucDataBuf != NULL)
            {
                cpss_mem_free(g_stCpssTcpMan.astClientTcpLink[ulIndex].stRecvBufM.pucDataBuf) ;
                g_stCpssTcpMan.astClientTcpLink[ulIndex].stRecvBufM.pucDataBuf = NULL ;
            }

            /*清零TCP链路信息*/
            cpss_mem_memset(&g_stCpssTcpMan.astClientTcpLink[ulIndex],
                0,sizeof(CPSS_COM_DRV_TCP_T)) ;
            g_stCpssTcpMan.ulClientTcpNum-- ;

            break;

        }
    }

    /*设置对应的connect info为空*/
    for(ulLoop=0;ulLoop<CPSS_COM_DRV_CLIENT_TCP_NUM;ulLoop++)
    {
        if((g_stCpssTcpMan.stTcpConnectInfo[ulLoop].ulIP == pstTcpLink->ulIP)&&
            (g_stCpssTcpMan.stTcpConnectInfo[ulLoop].usTcpPort == pstTcpLink->usTcpPort))
        {
            cpss_mem_memset(&(g_stCpssTcpMan.stTcpConnectInfo[ulLoop]),0,sizeof(CPSS_COM_TCP_LINK_T)) ;

            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "cpss_com_tcp_link_disconnect clear connectinfo ip=0x%x,port=0x%x\n",
                pstTcpLink->ulIP,pstTcpLink->usTcpPort);  
            return CPSS_OK;     
        }
    }

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_tcp_link_disconnect not find link ip=0x%x,port=0x%x\n",
        pstTcpLink->ulIP,pstTcpLink->usTcpPort);
    return CPSS_ERROR;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_link_send
* 功    能: 在OMC/LDT接入鉴权失败时，OAMS需要发送失败的响应消息，可以通过TCP链路
*           的IP地址、Port信息，来发送消息。
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* pstTcpLink  CPSS_COM_TCP_LINK_T *     输入              目标TCP信息
* pstMsg      CPSS_COM_MSG_HEAD_T *     输入              消息头结构指针
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说    明: 
*******************************************************************************/
INT32 cpss_com_tcp_link_send
(
    CPSS_COM_TCP_LINK_T *pstTcpLink,
    CPSS_COM_MSG_HEAD_T *pstMsg
)
{
    INT32 lSendLen ;
    UINT32 ulDataLen ;
    UINT8* pucSendBuf ;
    CPSS_COM_DRV_TCP_T *pstTcpDrv;
    CPSS_COM_TCP_LINK_T stTcpLink = {0} ;    
    UINT32 ulServerFlag ;

    /* 检查参数有效性 */
    if((NULL == pstTcpLink) || (NULL == pstMsg))
    {
        return (CPSS_ERR_COM_PARA_ILLEGAL);
    }

    /*如果原地址不存在，则填充原地址*/
    if(TRUE == CPSS_COM_LOGIC_ADDR_ZERO(pstMsg->stSrcProc.stLogicAddr))
    {
        pstMsg->stSrcProc.stLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
        pstMsg->stSrcProc.ulAddrFlag  = g_pstComData->stAddrThis.ulAddrFlag;
        pstMsg->stSrcProc.ulPd        = cpss_vk_pd_self_get();
    }

    /* 2006/5/24, jlm added */
    cpss_com_trace(pstMsg);
    /* 2006/5/24, jlm added */        
    
    /*查找TCP链路*/
    pstTcpDrv = cpss_com_tcp_link_find(pstTcpLink,&ulServerFlag) ;
    if(pstTcpDrv == NULL)
    {
        return CPSS_ERROR ;
    }

    /* 数据发送 */
    ulDataLen = pstMsg->ulLen ;        
    pucSendBuf = pstMsg->pucBuf ;
        
    if(ulServerFlag != CPSS_COM_TCP_DC_FLAG)
    {
    /*将数据分发头转化为网络字节序*/
    cpss_com_comm_hdr_hton(pstMsg) ;
        
    cpss_mem_memcpy(g_szCpssTcpSendBufPtr,(UINT8*)pstMsg,CPSS_COM_DISP_HEAD_LEN) ;
    cpss_mem_memcpy(g_szCpssTcpSendBufPtr+CPSS_COM_DISP_HEAD_LEN,pstMsg->pucBuf,ulDataLen) ;

        ulDataLen = ulDataLen + CPSS_COM_DISP_HEAD_LEN ;
        pucSendBuf = g_szCpssTcpSendBufPtr ;
    }

#ifdef CPSS_CPS__TNBS_TCP_SIM
    lSendLen=send(pstTcpDrv->ulSClient,pucSendBuf,ulDataLen,0) ;
#else
#ifdef CPSS_FUNBRD_MC
    /*如果是GCPA则调用TNBS的协议栈发送*/        
    lSendLen = cpss_com_tcp_data_send(pstTcpDrv->ulSClient,pucSendBuf,ulDataLen) ;
#else
    lSendLen = send(pstTcpDrv->ulSClient,pucSendBuf,ulDataLen,0) ;
#endif
#endif
        
    if(lSendLen == SOCKET_ERROR)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "CPSS_DRV: com tcp write failed! length 0x%08x",lSendLen);
        return CPSS_ERR_COM_SYSCALL_FAIL;            
    }
    return CPSS_OK ;
}
    

/*******************************************************************************
* 函数名称: cpss_com_tcp_drv_find
* 功    能: 查找逻辑地址对应链路
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* pstLogicAddr  CPSS_COM_LOGIC_ADDR_T*   输入       在该连接上绑定的逻辑地址
* 函数返回: 
*       成功：链路对象;
*       失败：NULL；
* 说    明: 
*******************************************************************************/
INT32 cpss_com_tcp_drv_find 
(    
    CPSS_COM_LOGIC_ADDR_T stLogicAddr,
    CPSS_COM_DRV_TCP_T *pstTcpDrv    
)
{
    UINT32 ulIdx;
    CPSS_COM_DRV_TCP_T *pstTcpDrvT;    

    /* 检查参数有效性 */
    if(NULL == pstTcpDrv)
    {
        return CPSS_ERR_COM_PARA_ILLEGAL;
    }

    /* 查找逻辑地址对应的tcp链路 */    
    for(ulIdx=0; ulIdx<CPSS_COM_DRV_TCP_NUM; ulIdx++)
    {
        pstTcpDrvT=&g_stCpssTcpMan.astServerTcpLink[ulIdx];
        
        if(!CPSS_COM_LOGIC_ADDR_SAME(pstTcpDrvT->stLogicAddr,stLogicAddr))            
        {   
            continue;
        }  
        cpss_mem_memcpy(pstTcpDrv,pstTcpDrvT,sizeof(CPSS_COM_DRV_TCP_T));                        
        return CPSS_OK;
    }    
    
    /* 查找网元直连对应的tcp链路 */
    for(ulIdx=0; ulIdx<CPSS_COM_DRV_DC_TCP_NUM; ulIdx++)
    {
        pstTcpDrvT=&g_stCpssTcpMan.astServerDcLink[ulIdx];
        
        if(!CPSS_COM_LOGIC_ADDR_SAME(pstTcpDrvT->stLogicAddr,stLogicAddr))            
        {   
            continue;
        }  
        cpss_mem_memcpy(pstTcpDrv,pstTcpDrvT,sizeof(CPSS_COM_DRV_TCP_T));                        
        return CPSS_OK;
    }
    
    return CPSS_ERROR;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_route_allow
* 功    能: 查找tcp链路对应的逻辑地址
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* pstLogicAddr  CPSS_COM_LOGIC_ADDR_T*   输入       在该连接上绑定的逻辑地址
* 函数返回: 
*            CPSS_TCP_NO_BIND  没有绑定逻辑地址
*            CPSS_TCP_NO_ROUTE 绑定了逻辑地址但不能路由
*            CPSS_TCP_ROUTE    可以路由
*            CPSS_ERROR        失败
* 说    明: 
*******************************************************************************/
INT32 cpss_com_tcp_route_allow 
(    
    CPSS_COM_TCP_LINK_T *pstTcpLink,
    CPSS_COM_LOGIC_ADDR_T *pstLogicAddr,
    UINT32* pulServerFlag
)
{
    CPSS_COM_DRV_TCP_T *pstTcpDrv;    

    /* 检查参数有效性 */
    if((NULL == pstTcpLink) || (NULL == pstLogicAddr))
    {
        return CPSS_ERR_COM_PARA_ILLEGAL;
    }
    
    /* 查找链路对应的逻辑地址 */           
    pstTcpDrv = cpss_com_tcp_link_find(pstTcpLink,pulServerFlag) ;
    if(pstTcpLink == NULL)
    {
        return CPSS_ERROR ;
    }

    /*如果是client数据，则直接返回*/
    if(*pulServerFlag == CPSS_COM_TCP_CLIENT_FLAG)
    {
        return CPSS_TCP_ROUTE ;
    }       

    /*逻辑地址存在*/
    if(TRUE == CPSS_COM_LOGIC_ADDR_ZERO(pstTcpDrv->stLogicAddr))
    {
        return CPSS_TCP_NO_BIND;
    }

    /*路由标志：不允许路由*/
    if(pstTcpDrv->ulRouteAllowFlag == CPSS_COM_TCP_CONNECT_ROUTE_NOT_ALLOW)
    {
        return CPSS_TCP_NO_ROUTE ;
    }

    return CPSS_TCP_ROUTE;
}    


/*******************************************************************************
* 函数名称: cpss_com_tcp_socket_exist
* 功    能: 查找tcp对应的socket是否存在
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* pstLogicAddr  CPSS_COM_LOGIC_ADDR_T*   输入       在该连接上绑定的逻辑地址
* 函数返回: 
*       存在：CPSS_OK;
*       失败：CPSS_ERROR;
* 说    明: 
*******************************************************************************/
INT32 cpss_com_tcp_socket_exist
(
CPSS_COM_TCP_LINK_T *pstTcpLink
)
{
    UINT32 ulIdx = 0;
    UINT32 ulServerFlag ;
    CPSS_COM_DRV_TCP_T *pstTcpDrv;

    pstTcpDrv = cpss_com_tcp_link_find(pstTcpLink,&ulServerFlag) ;
    if(pstTcpDrv == NULL)
    {
        return CPSS_ERROR ;
    }

    if(pstTcpDrv->ulSClient == 0)
    {
        return CPSS_ERROR ;
    }

    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_tcp_cps__oams_connect_info_send
* 功    能: 发送链路连接，断开消息给OAMS纤程
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* ulMsgId         UINT32                输入              连接（断开）消息
* pucSendBuf      UINT8                 输出              发送的消息体
* 函数返回: 
*       存在：CPSS_OK;
*       失败：CPSS_ERROR;
* 说    明: 
*******************************************************************************/
INT32 cpss_com_tcp_cps__oams_connect_info_send
(
UINT8* pucSendBuf,
UINT32 ulMsgId,
UINT32 ulServerFlag
)
{
    CPSS_COM_MSG_HEAD_T stComMsg;
    INT32 lRet;

    cpss_mem_memset(&stComMsg,0,sizeof(CPSS_COM_MSG_HEAD_T));
    
    if((ulMsgId!=CPSS_COM_TCP_CONN_IND_MSG)&&
       (ulMsgId!=CPSS_COM_TCP_DISCONN_IND_MSG))
    {
        return CPSS_ERROR ;
    }

    /*根据不同的链路类型选择不同的目的纤程*/
    if(ulServerFlag == CPSS_COM_TCP_SERVER_FLAG || ulServerFlag == CPSS_COM_TCP_CLIENT_FLAG)
    {
        stComMsg.stDstProc.ulPd = CPS__OAMS_PD_M_COMM_PROC ;
    }
    else if(ulServerFlag == CPSS_COM_TCP_DC_FLAG)
    {
        stComMsg.stDstProc.ulPd = CPS__OAMS_PD_M_DLM_MSGDSP_PROC ;
    }

    stComMsg.stDstProc.stLogicAddr = g_pstComData->stAddrThis.stLogiAddr ;
    stComMsg.pucBuf  = pucSendBuf ;

    if(ulMsgId==CPSS_COM_TCP_CONN_IND_MSG)
    {
        stComMsg.ulMsgId = CPSS_COM_TCP_CONN_IND_MSG ;
        stComMsg.ulLen   = sizeof(CPSS_COM_TCP_CONN_IND_MSG_T) ;
    }
    else
    {
        stComMsg.ulMsgId = CPSS_COM_TCP_DISCONN_IND_MSG ;
        stComMsg.ulLen   = sizeof(CPSS_COM_TCP_DISCONN_IND_MSG_T) ;
    }
    
    lRet = cpss_com_send_extend(&stComMsg);
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
            "send tcp connect notify message failed!");
        return CPSS_ERROR ;
    }
    
    return CPSS_OK ;
}
#endif

#ifdef CPSS_PCI_INCLUDE

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_read                            
* 功    能: TCP驱动适配层读函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_drv_pci_read
(
    UINT32  ulDrvType,
    UINT32  ulLinkId,
    UINT8   *pucBuf,
    UINT32  ulReadSize
)
{
    CPSS_COM_TRUST_HEAD_T* pstTrustHdr;
    INT32 lRet; 
    
    pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)pucBuf;
    
    /*转化字节序为本机字节序*/
    cpss_com_hdr_ntoh(pstTrustHdr) ;
    
    /**/
    pstTrustHdr->ucPType  = COM_SLID_PTYPE_INCREDIBLE ;
    
#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
    {
        UINT32 bIsDst;
        BOOL bTraceIt = cpss_trace_route_it_recv(pucBuf, &bIsDst);
        if (TRUE == bTraceIt)
        {
            UINT32 ulNodeLoc;
            if (TRUE == bIsDst)
                ulNodeLoc = CPSS_TRACE_NODE_LOC_DST;
            else
                ulNodeLoc = CPSS_TRACE_NODE_LOC_ROUTE;
            cpss_trace_route_info_set(ulNodeLoc, CPSS_TRACE_LAYER_DRVRI, CPSS_TRACE_IF_DRV_PCI, 0,0,0, pucBuf);
        }
    }
#endif

    /*将接收到的数据交给高层*/
    lRet = cpss_com_drv_deliver(0,pstTrustHdr);
    if(lRet != CPSS_OK)
    {
      cpss_com_pci_link_stat(ulLinkId,CPSS_COM_PCI_RECV_DELIVER_ERR_NUM_FLAG); 
    }

    return lRet;
}
/*******************************************************************************
* 函数名称: cpss_com_pci_link_stat                            
* 功    能: tcp驱动写函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*                                                 
******************************************************************************/

INT32 cpss_com_pci_link_stat(UINT32  ulLinkId ,UINT32 flag)
{
    if(ulLinkId > CPSS_DRV_PCI_INFO_ARRAY_MAX)
    {
       return CPSS_ERROR;
    }
    if(CPSS_COM_PCI_SEND_NUM_FLAG == flag)
    {
       g_stCpssComPciTbl.astPciLink[ulLinkId-1].stPciStat.ulSendDataNum = 
       g_stCpssComPciTbl.astPciLink[ulLinkId-1].stPciStat.ulSendDataNum + 1;
    }
    else if(CPSS_COM_PCI_SENDERROR_NUM_FLAG == flag)
    {
       g_stCpssComPciTbl.astPciLink[ulLinkId-1].stPciStat.ulDeliverErrNum = 
       g_stCpssComPciTbl.astPciLink[ulLinkId-1].stPciStat.ulDeliverErrNum + 1;
    }
    else if(CPSS_COM_PCI_RECV_NUM_FLAG == flag)
    {
       g_stCpssComPciTbl.astPciLink[ulLinkId-1].stPciStat.ulRecvDataNum = 
       g_stCpssComPciTbl.astPciLink[ulLinkId-1].stPciStat.ulRecvDataNum + 1;
    }
    else if(CPSS_COM_PCI_FAULT_NUM_FLAG == flag)
    {
        g_stCpssComPciTbl.astPciLink[ulLinkId-1].stPciStat.ulFaultNum = 
        g_stCpssComPciTbl.astPciLink[ulLinkId-1].stPciStat.ulFaultNum + 1;
    }
    else if(CPSS_COM_PCI_RECV_DELIVER_ERR_NUM_FLAG == flag)
    {
        g_stCpssComPciTbl.astPciLink[ulLinkId-1].stPciStat.ulRecvDataDeliverErrNum = 
        g_stCpssComPciTbl.astPciLink[ulLinkId-1].stPciStat.ulRecvDataDeliverErrNum + 1;
    }
    return CPSS_OK;   
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_write                            
* 功    能: tcp驱动写函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_drv_pci_write
(
    UINT32  ulLinkId,
    UINT32  ulLinkInfo[3],
    UINT8  *pucBuf,
    UINT32  ulMaxSize,
    UINT32 *pulWriteSize
)
{
    INT32  lRet ;
    CPSS_COM_MSG_HEAD_T *pstMsg ;
    
    /*P信号量*/
    if(CPSS_OK != cpss_vos_mutex_p(g_stCpssComPciTbl.astPciLink[ulLinkId-1].ulWriteMutex,WAIT_FOREVER))
    {
        return CPSS_ERROR ;
    }

    /*判断链路读写标志*/
    if(g_stCpssComPciTbl.astPciLink[ulLinkId-1].ulWRFlag == CPSS_COM_PCI_WR_DISABLE)
    {
        cpss_vos_mutex_v(g_stCpssComPciTbl.astPciLink[ulLinkId-1].ulWriteMutex);
        return CPSS_ERROR ;
    }


    /* pci 发送数据统计flag = 0 */
    cpss_com_pci_link_stat(ulLinkId,CPSS_COM_PCI_SEND_NUM_FLAG);

#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
    {
        UINT32 bIsSrc;
        BOOL bTraceIt = cpss_trace_route_it_send(pucBuf, &bIsSrc);
        if (TRUE == bTraceIt)
        {
            UINT32 ulNodeLoc;
            if (TRUE == bIsSrc)
                ulNodeLoc = CPSS_TRACE_NODE_LOC_SRC;
            else
                ulNodeLoc = CPSS_TRACE_NODE_LOC_ROUTE;
            cpss_trace_route_info_set(ulNodeLoc, CPSS_TRACE_LAYER_DRVSI, CPSS_TRACE_IF_DRV_PCI, 0,0,0, pucBuf);
        }
    }
#endif

    pstMsg = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pucBuf) ;

    /*增加统计*/
    cpss_com_spec_msg_stat(pstMsg->ulMsgId,DRV_PCI_SEND_PHASE);
    
    lRet = cpss_com_pci_write(ulLinkId,pucBuf,ulMaxSize) ;
    if(CPSS_OK != lRet)
    {
        /*增加统计*/
        cpss_com_spec_msg_stat(pstMsg->ulMsgId,DRV_PCI_SEND_FAIL_PHASE);
        
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "CPSS_DRV: com pci write failed! length 0x%08x",ulMaxSize);
#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
        cpss_trace_route_error_hook(pucBuf, CPSS_PCI_WRITE_ERR);
#endif

       /*pci发送数据失败统计 flag = 1*/
       cpss_com_pci_link_stat(ulLinkId,CPSS_COM_PCI_SENDERROR_NUM_FLAG);

        cpss_vos_mutex_v(g_stCpssComPciTbl.astPciLink[ulLinkId-1].ulWriteMutex);

        return CPSS_ERROR ;
    }

    cpss_vos_mutex_v(g_stCpssComPciTbl.astPciLink[ulLinkId-1].ulWriteMutex);

    *pulWriteSize = ulMaxSize ;

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_info_set
* 功    能:
* 函数类型:
* 参    数:                                          
* 参数名称            类型            输入/输出         描述

* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_com_drv_pci_info_set
(
    CPSS_COM_PCI_TABLE_T *pstPciTable
)
{
    UINT32 ulCpuNum = 0 ;
    UINT32 ulLoop ;
    INT32  lRet ;
    DRV_PCI_INFO_T astDrvPciInfo[CPSS_PCI_INFO_ARRAY_MAX] ;
    DRV_PCI_INFO_T *pstDrvPciLink ;

    pstDrvPciLink = astDrvPciInfo ;

    /*得到PCI驱动的信息*/
    lRet = cpss_drv_pci_info_get(&ulCpuNum,pstDrvPciLink);
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }
    
    /*得到CPU的数目*/
    pstPciTable->ulCpuNum = ulCpuNum ;
    
    for(ulLoop=0;ulLoop<ulCpuNum;ulLoop++)
    {
        pstPciTable->astPciLink[ulLoop].ulCpuNo = astDrvPciInfo[ulLoop].ulCpuId ;
    }
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_dsp_pci_info_get
* 功    能:
* 函数类型:
* 参    数:                                          
* 参数名称            类型            输入/输出         描述

* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
#ifdef CPSS_DSP_CPU
INT32 cpss_com_drv_dsp_pci_info_get()
{
    DRV_PCI_INFO_T *pstDspPciInfo ;

    pstDspPciInfo = (DRV_PCI_INFO_T*)(DRV_PCI_DSP_BASE_AADR + DRV_PCI_DSP_ADDR_OFFSET) ;

    pstDspPciInfo->ulCpuId       = cpss_ntohl(pstDspPciInfo->ulCpuId);
    pstDspPciInfo->ulLen         = cpss_ntohl(pstDspPciInfo->ulLen);
    pstDspPciInfo->ulRamWinAttr  = cpss_ntohl(pstDspPciInfo->ulRamWinAttr);
    pstDspPciInfo->ulStartAddr   = cpss_ntohl(pstDspPciInfo->ulStartAddr);
    /*读取PCI驱动信息数据*/
    g_stCpssDspPciInfo = *pstDspPciInfo ;
    
    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_DETAIL,
        "dsp pci drv info &g_stCpssDspPciInfo=0x%x, g_stCpssDspPciInfo.ulCpuId=%d,g_stCpssDspPciInfo.ulLen=%d,g_stCpssDspPciInfo.ulStartAddr=0x%x\n",
        &g_stCpssDspPciInfo,g_stCpssDspPciInfo.ulCpuId,g_stCpssDspPciInfo.ulLen,g_stCpssDspPciInfo.ulStartAddr);
    
    return CPSS_OK ;
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_pci_callback_fun
* 功    能:
* 相关文档:
* 函数类型:
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
VOID cpss_com_pci_callback_fun(UINT8 ucReqId, UINT32 ulMsgContext)
{
#ifndef CPSS_DSP_CPU
    INT32 lRet ;
    CPSS_COM_PCI_NOTIFY_T stPciNotify ;

    stPciNotify.ulProcId = CPSS_COM_LINK_MNGR_PROC ;
    stPciNotify.ulCpuNo = 2 ;
    stPciNotify.usRsv  = 0;
    stPciNotify.ulMsgId = CPSS_COM_IRQ_IND_MSG;
    stPciNotify.usMsglen = sizeof(CPSS_COM_PCI_NOTIFY_T);
    stPciNotify.ulIrqNo = CPSS_COM_PCI_V_IRQ_NO ;

    lRet = cpss_com_irq_send((UINT8*)&stPciNotify,sizeof(CPSS_COM_PCI_NOTIFY_T));
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "CPSS_DRV: send pci notify failed.");
    }
#endif
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_get_phyaddr
* 功    能:
* 相关文档:
* 函数类型:
* 参    数:                                          
* 参数名称            类型            输入/输出         描述

* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
VOID cpss_com_drv_pci_get_phyaddr
(
    CPSS_COM_PHY_ADDR_T   stMPhyAddr,
    UINT32                ulCpuNo,
    CPSS_COM_PHY_ADDR_T*  pstSPhyAddr
)
{
    pstSPhyAddr->ucFrame = stMPhyAddr.ucFrame ;
    pstSPhyAddr->ucShelf = stMPhyAddr.ucShelf ;
    pstSPhyAddr->ucSlot  = stMPhyAddr.ucSlot ;
    pstSPhyAddr->ucCpu   = ulCpuNo ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_get_logaddr
* 功    能:
* 相关文档:
* 函数类型:
* 参    数:                                          
* 参数名称            类型            输入/输出         描述

* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
VOID cpss_com_drv_pci_get_logaddr
(
    CPSS_COM_LOGIC_ADDR_T   stMLogAddr,
    UINT32                  ulCpuNo,
    CPSS_COM_LOGIC_ADDR_T*  pstSLogAddr
)
{
    pstSLogAddr->ucModule   = stMLogAddr.ucModule ;
    pstSLogAddr->usGroup    = stMLogAddr.usGroup ;
    pstSLogAddr->ucSubGroup = ulCpuNo ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_linkid_get
* 功    能:
* 相关文档:
* 函数类型:
* 参    数:                                          
* 参数名称            类型            输入/输出         描述

* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_com_drv_pci_linkid_get
(
    UINT32 ulCpuNo,
    UINT32 *pulLinkId
)
{
    UINT32 ulCpuNum ;
    UINT32 ulLoop ;

    ulCpuNum = g_stCpssComPciTbl.ulCpuNum ;

    for(ulLoop=0;ulLoop<ulCpuNum;ulLoop++)
    {
        if(ulCpuNo == g_stCpssComPciTbl.astPciLink[ulLoop].ulCpuNo)
        {
            *pulLinkId = ulLoop + 1 ;
            return CPSS_OK ;
        }
    }
    return CPSS_ERROR ;
}


/*******************************************************************************
* 函数名称: cpss_com_drv_pci_get_addr_by_cpuno
* 功    能:
* 相关文档:
* 函数类型:
* 参    数:                                          
* 参数名称            类型            输入/输出         描述

* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_com_drv_pci_get_addr_by_cpuno
(
    UINT32 ulCpuNo,
    CPSS_COM_LOGIC_ADDR_T *pstLogAddr,
    CPSS_COM_PHY_ADDR_T   *pstPhyAddr
)
{
    UINT32 ulLinkId ;
    INT32  lRet ;

    lRet = cpss_com_drv_pci_linkid_get(ulCpuNo,&ulLinkId) ;
    if(CPSS_OK != lRet)
    {
        return CPSS_ERR_COM_PCI_NOT_FINDE ;
    }

    *pstLogAddr = g_stCpssComPciTbl.astPciLink[ulLinkId-1].stDstLogAddr;
    *pstPhyAddr = g_stCpssComPciTbl.astPciLink[ulLinkId-1].stDstPhyAddr; 

    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_drv_pci_addr_fill
* 功    能:
* 相关文档:
* 函数类型:
* 参    数:                                          
* 参数名称            类型            输入/输出         描述

* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
VOID cpss_com_drv_pci_addr_fill
(
    CPSS_COM_PHY_ADDR_T   stPhyAddr,
    CPSS_COM_LOGIC_ADDR_T stLogAddr,
    UINT32                ulCpuNo
)
{
    UINT32 ulLinkId ;
    CPSS_COM_PHY_ADDR_T   stPciPhyAddr ;
    CPSS_COM_LOGIC_ADDR_T stPciLogAddr ;

    /*找到对应CPU的链路号*/
    cpss_com_drv_pci_linkid_get(ulCpuNo,&ulLinkId) ;

    /*计算对应CPU的逻辑地址和物理地址*/
    cpss_com_drv_pci_get_phyaddr(stPhyAddr,ulCpuNo,&stPciPhyAddr) ;
    cpss_com_drv_pci_get_logaddr(stLogAddr,ulCpuNo,&stPciLogAddr) ;

    /*填充PCI的逻辑地址和物理地址*/
    g_stCpssComPciTbl.astPciLink[ulLinkId-1].stDstPhyAddr = stPciPhyAddr ;
    g_stCpssComPciTbl.astPciLink[ulLinkId-1].stDstLogAddr = stPciLogAddr ;
}


/*******************************************************************************
* 函数名称: cpss_com_drv_pci_notify_send
* 功    能: 向所有的从CPU发送notify中断消息
* 相关文档:
* 函数类型:
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
#ifdef CPSS_SLAVE_CPU
VOID cpss_com_drv_pci_notify_send()
{
    UINT32 ulCpuNum ;
    UINT32 ulCpuNo ;
    UINT32 ulLoop ;

    ulCpuNum = g_stCpssComPciTbl.ulCpuNum ;

    /*循环发送CPSS_COM_PCI_NOTIFY_IRQ_NO消息*/
    for(ulLoop=0;ulLoop<ulCpuNum;ulLoop++)
    {
        ulCpuNo = g_stCpssComPciTbl.astPciLink[ulLoop].ulCpuNo ;
        drv_pci_notify_send(ulCpuNo, CPSS_COM_PCI_NOTIFY_IRQ_NO,0) ;
    }
}
/*******************************************************************************
* 函数名称: cpss_com_pci_write_slave
* 功    能: 纤程间消息发送，只允许在纤程中调用，以调用者的身份发送消息。
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型            输入/输出         描述
* pstDstPid   CPSS_COM_PID_T*     输入            目标纤程的PID
* ulMsgId     UINT32            输入            消息ID
* pucData     UINT8 *           输入            消息体指针
* ulDataLen   UINT32            输入            消息体长度
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: CPU内最大消息体长度：没有限制，受限于最大内存块。
*           CPU间最大消息体长度：（64K－256）BYTE
*           发送标志默认为可靠、普通优先级。
*******************************************************************************/
UINT32 g_ulPciPrint=0 ;
UINT32 g_usVpi = 0;
INT32 cpss_com_pci_write_slave
(
CPSS_COM_PID_T *pstDstPid,
CPSS_COM_PID_T *pstSrcPid,
UINT32 ulMsgId,
UINT8 *pucData,
UINT32 ulDataLen
)
{
    CPSS_COM_EBD_POOL_HEAD_T* pstEbdPoolHdr ;
    CPSS_COM_PCI_DRV_LINK_T* pstPciLink ;
    CPSS_COM_EBD_T*      pstTxEBD ;
    UINT32 ulWriteIndex ;
    UINT32 ulReadIndex ;
    UINT32 ulEndWriteIndex ;
    UINT32 ulTxEbdWriteIndex ;
    UINT32 usStatus ;
    UINT32 ulTxBufCount ;
    UINT32 ulTxTmpLen ;
    UINT8* pucWriteBuf ;
    CPSS_COM_MSG_HEAD_T* pstMsg;  
    CPSS_COM_TRUST_HEAD_T *pstTrustHdr;
    UINT32 ulTmp;
    UINT32 ulSendLen ;
    UINT8* pucPciBuf ;
    UINT8 *pucTempBuf;
    ulSendLen = ulDataLen + 16 + 40 ;
        
    /*得到PCI共享数据交换缓冲区指针*/
    cpss_com_pci_link_stat(1,CPSS_COM_PCI_SEND_NUM_FLAG);

    if(CPSS_OK != cpss_vos_mutex_p(g_stCpssComPciTbl.astPciLink[0].ulWriteMutex,WAIT_FOREVER))
    {
        return CPSS_ERROR ;
    }
    pstPciLink = &g_stPciLinkTable.astPciLink[0] ;
    pstEbdPoolHdr = (CPSS_COM_EBD_POOL_HEAD_T*)g_stPciLinkTable.aulMemPoolAddr[0] ;

    if(pstEbdPoolHdr==NULL)
    {
        cpss_vos_mutex_v(g_stCpssComPciTbl.astPciLink[0].ulWriteMutex);
        return CPSS_ERROR ;
    }

    /*找到对应的TXEBD结构*/
    ulTxEbdWriteIndex = cpss_ntohl(*(UINT32*)((UINT8*)pstEbdPoolHdr+CPSS_PCI_TXEBD_WRITE_INDEX_OFFSET));


    pstTxEBD = (CPSS_COM_EBD_T*)(pstPciLink->ulTxEbdStart)+ulTxEbdWriteIndex ;

    if(CPSS_COM_PCI_TX_READY(cpss_ntohl(*(UINT32*)((UINT8*)pstTxEBD + CPSS_PCI_EBD_STATUS_OFFSET))))
    {
        ulWriteIndex = cpss_ntohl(*(UINT32*)((UINT8*)pstEbdPoolHdr + CPSS_PCI_TXBUF_WRITE_INDEX_OFFSET));
        ulReadIndex = cpss_ntohl(*(UINT32*)((UINT8*)pstEbdPoolHdr + CPSS_PCI_TXBUF_READ_INDEX_OFFSET));
        
        if(cpss_com_drv_pci_if_full_EX(ulWriteIndex,  ulReadIndex, pstPciLink->ulTxBufNum))
        {
            cpss_vos_mutex_v(g_stCpssComPciTbl.astPciLink[0].ulWriteMutex);
            return CPSS_ERROR ;
        }
        /*判断发送缓冲区的长度是否满足需要的长度*/

        if(cpss_com_pci_sendlen_fit(ulSendLen,ulWriteIndex,ulReadIndex,
                                    pstPciLink->ulTxBufNum) == CPSS_ERROR)
        {
            cpss_vos_mutex_v(g_stCpssComPciTbl.astPciLink[0].ulWriteMutex);
            return CPSS_ERROR ;
        }
             
	    /*计算接收的数据使用的接收内存个数*/
        {  
	    pucWriteBuf = (UINT8*)(pstPciLink->ulTxBufBase) + ulWriteIndex*CPSS_COM_PCI_RXTX_BUF_SIZE ;

	    pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)pucWriteBuf;
	    pstMsg = CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr);

	    /* dispatch hdr fill */
	    *(UINT32*)&pstMsg->stDstProc.stLogicAddr = *(UINT32*)&pstDstPid->stLogicAddr;
	    pstMsg->stDstProc.ulAddrFlag = pstDstPid->ulAddrFlag;
	    pstMsg->stDstProc.ulPd = pstDstPid->ulPd;
	    pstMsg->stSrcProc.ulAddrFlag = pstSrcPid->ulAddrFlag;
	    pstMsg->stSrcProc.ulPd = pstSrcPid->ulPd;
	    pstMsg->stSrcProc.stLogicAddr = pstSrcPid->stLogicAddr;
	    pstMsg->ulMsgId      = ulMsgId;
	    pstMsg->pucBuf       = pucData;
	    pstMsg->ulLen         = ulDataLen;
	    pstMsg->ucPriFlag  = CPSS_COM_PRIORITY_NORMAL ;
	    pstMsg->ucShareFlag= CPSS_COM_MEM_SHARE ;
	    pstMsg->ucAckFlag  = CPSS_COM_ACK;

	    /* trust hdr fill */
	    #if 1
	    ulTmp = (CPSS_COM_PROTOCAL_VER << 29) | (CPSS_COM_HDRLEN_PACK(CPSS_COM_LINK_HEAD_LEN) << 24) | (0 << 16) |\
	                 (0 << 13) | (0 << 8) | (0);
	    
	    *(UINT32*)pstTrustHdr     = ulTmp;
	    (UINT32*)pstTrustHdr += 1;
	    *(UINT32*)pstTrustHdr     = ulSendLen;
	    (UINT32*)pstTrustHdr += 1;
	    *(UINT32*)pstTrustHdr     = 0;
	    (UINT32*)pstTrustHdr += 1;
	    *(UINT32*)pstTrustHdr     = 0;
            cpss_com_hdr_hton(pstTrustHdr) ;
          #else
          
           cpss_com_trust_hrd_pub_info_fill(pstTrustHdr);
          
            cpss_com_hdr_hton(pstTrustHdr) ;
            #endif
	    
	    pucPciBuf = (UINT8*)pstMsg ;
	    if(ulDataLen <=200)
	    {
	    #if 0
	        pucTempBuf = (UINT8*)malloc(ulDataLen);
	        memcpy(pucTempBuf, pucData, ulDataLen);
	    #endif		
	        memcpy(pucPciBuf+40, pucData, ulDataLen);
	    #if 0	
	        if(g_ulPciPrint == 1)
            {
               
                if(ulMsgId == 0x22022001)
    	        {
    	           UINT32 ulLoop ;
    	           printf("pucData= %p,",pucData);
    	           for(ulLoop=0;ulLoop<ulDataLen;ulLoop++)
    	           {
    	               printf("%x ",*(pucData+ulLoop));	        
    	           }
    	           printf("\n");
    	           printf("pucTempBuf= %p,",pucTempBuf);
    	           for(ulLoop=0;ulLoop<ulDataLen;ulLoop++)
    	           {
    	               printf("%x ",*(pucTempBuf+ulLoop));	        
    	           }
    	           printf("\n");
    	           printf("pucPciBuf= %p,",pucPciBuf);
    	           for(ulLoop=0;ulLoop<ulDataLen;ulLoop++)
    	           {
    	               printf("%x ",*(pucPciBuf+40+ulLoop));	        
    	           }
    	           printf("\n");
    	        }
            }
            else if(g_ulPciPrint == 2)
            {
                if (ulMsgId == 0x22022001)                   
                 {                
                    if(memcmp((pucPciBuf+40),pucData,ulDataLen))
    	            {
    	               UINT32 ulLoop ;
    	               printf("pucData= %p,",pucData);
    	               for(ulLoop=0;ulLoop<ulDataLen;ulLoop++)
    	               {
    	                   printf("%x ",*(pucData+ulLoop));	        
    	               }
    	               printf("\n");
    	               printf("pucTempBuf= %p,",pucTempBuf);
    	               for(ulLoop=0;ulLoop<ulDataLen;ulLoop++)
    	               {
    	                   printf("%x ",*(pucTempBuf+ulLoop));	        
    	               }
    	               printf("\n");
    	               printf("pucPciBuf= %p,",pucPciBuf);
    	               for(ulLoop=0;ulLoop<ulDataLen;ulLoop++)
    	               {
    	                   printf("%x ",*(pucPciBuf+40+ulLoop));	        
    	               }
    	               printf("\n");
    	            }
                 }
            }
            else if(g_ulPciPrint == 3)
            {
                if (ulMsgId == 0x22022001)                   
                 {                
                    /*if(memcmp((pucPciBuf+40),pucData,ulDataLen))*/
                    if(*(UINT16*)(pucData+10) == (UINT16)g_usVpi)
    	            {
    	               UINT32 ulLoop ;
    	               printf("pucData= %p,",pucData);
    	               for(ulLoop=0;ulLoop<ulDataLen;ulLoop++)
    	               {
    	                   printf("%x ",*(pucData+ulLoop));	        
    	               }
    	               printf("\n");
    	               printf("pucTempBuf= %p,",pucTempBuf);
    	               for(ulLoop=0;ulLoop<ulDataLen;ulLoop++)
    	               {
    	                   printf("%x ",*(pucTempBuf+ulLoop));	        
    	               }
    	               printf("\n");
    	               printf("pucPciBuf= %p,",pucPciBuf);
    	               for(ulLoop=0;ulLoop<ulDataLen;ulLoop++)
    	               {
    	                   printf("%x ",*(pucPciBuf+40+ulLoop));	        
    	               }
    	               printf("\n");
    	            }
                 }
            }
	 #endif		
	 #if 0		
            free(pucTempBuf);
	 #endif 
	    }
	    else
	    {
            memcpy(pucPciBuf+40, pucData, 200);
	        cpss_com_pci_mem_write(pucData+200,(UINT8*)(pstPciLink->ulTxBufBase),ulWriteIndex+1,
                               ulDataLen-200,pstPciLink->ulTxBufNum);
	    }
	}

        /*移动写入缓冲区的写指针*/
        ulEndWriteIndex =  cpss_com_pci_buf_index_get(ulWriteIndex,ulSendLen,
                                                      pstPciLink->ulTxBufNum) ;
        
        /*设置txEBD中的内容*/
        *(UINT32*)((UINT8*)pstTxEBD+CPSS_PCI_EBD_BUF_INDEX_OFFSET) = cpss_htonl(ulWriteIndex);
        *(UINT32*)((UINT8*)pstTxEBD+CPSS_PCI_EBD_LENGTH_OFFSET) = cpss_htonl(ulSendLen) ;

        /*设置PCI的EBD标志为可读*/
        cpss_com_drv_pci_ebd_ready_rx_set(pstTxEBD) ;

        *(UINT32*)((UINT8*)pstEbdPoolHdr+CPSS_PCI_TXBUF_WRITE_INDEX_OFFSET) = cpss_htonl(ulEndWriteIndex) ;
        *(UINT32*)((UINT8*)pstEbdPoolHdr+CPSS_PCI_TXEBD_WRITE_INDEX_OFFSET) = cpss_htonl(CPSS_MOD((ulTxEbdWriteIndex + 1),CPSS_COM_PCI_TX_EBD_NUM));
        cpss_vos_mutex_v(g_stCpssComPciTbl.astPciLink[0].ulWriteMutex);
        return CPSS_OK ;
    }
    cpss_vos_mutex_v(g_stCpssComPciTbl.astPciLink[0].ulWriteMutex);
    return CPSS_ERROR ;
}
#endif

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_notify_write
* 功    能: 
* 相关文档:
* 函数类型:
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
*******************************************************************************/
#ifdef CPSS_DSP_CPU
UINT32 g_ulCpssRegFlag = 0;
#endif
INT32 cpss_com_drv_pci_notify_write()
{
    UINT32 ulCpuNum ;
    UINT32 ulLoop ;
    INT32  lRet ;
    CPSS_COM_PCI_REG_REQ_MSG_T stPciDspInd ;

    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulPciNotifyInCount);
    stPciDspInd.ulMsgId = cpss_htonl(CPSS_COM_SCPU_REG_REQ_MSG) ;

    /*得到CPU的个数*/
    ulCpuNum = g_stCpssComPciTbl.ulCpuNum ;

    /*循环发送CPSS_COM_PCI_NOTIFY_IRQ_NO消息*/
    for(ulLoop=0;ulLoop<ulCpuNum;ulLoop++)
    {   
        stPciDspInd.ulCpuNo = cpss_htonl(g_stCpssComPciTbl.astPciLink[ulLoop].ulCpuNo) ;
        stPciDspInd.ulSumCheck = cpss_htonl(CPSS_COM_PCI_CHECK_VALUE);
        

        lRet = cpss_com_pci_special_write(ulLoop+1,0,(UINT8*)&stPciDspInd,
            sizeof(CPSS_COM_PCI_REG_REQ_MSG_T));
        if(CPSS_OK != lRet)
        {
            return CPSS_ERROR ;
        }
    }
    #ifdef CPSS_DSP_CPU
    g_ulCpssRegFlag = 1;
    #endif
    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulPciNotifyOutCount);
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_drv_pci_write_sem_create
* 功    能: 初始化所有的PCI发送信号量
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* 函数返回: 
*          
* 说    明: 
*******************************************************************************/
INT32 cpss_com_drv_pci_write_sem_create()
{
    UINT32  ulCpuNum ;
    UINT32 ulLoop ;

    ulCpuNum = g_stCpssComPciTbl.ulCpuNum ;
    
    /*创建每个PCI内存缓冲区的写互斥量*/
    for(ulLoop=0;ulLoop<ulCpuNum;ulLoop++)
    {
        g_stCpssComPciTbl.astPciLink[ulLoop].ulWriteMutex = cpss_vos_mutex_create();
        if(VOS_SEM_DESC_INVALID == g_stCpssComPciTbl.astPciLink[ulLoop].ulWriteMutex)
        {
            return CPSS_ERROR ;
        }
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_enable_read
* 功    能:
* 相关文档:
* 函数类型:
* 参    数:                                          
* 参数名称            类型            输入/输出         描述

* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_com_drv_pci_enable_read()
{
    
    /*开启PCI读任务的信号量*/
    if(0==g_stCpssComPciTbl.ulPciReadMutex)
    {
        g_stCpssComPciTbl.ulPciReadMutex =  cpss_vos_sem_b_create(VOS_SEM_Q_FIFO,VOS_SEM_EMPTY);
        if(VOS_SEM_DESC_INVALID == g_stCpssComPciTbl.ulPciReadMutex)
        {
            return CPSS_ERROR ;
        }
    }
#ifdef CPSS_DSP_CPU    
    /*判断是否挂接了辅助时钟,如果没有挂接则挂接辅助时钟*/
#if 0
    if(0==g_stCpssComPciTbl.ulIrqConnFlag)
    {
        lRet = cpss_pci_aux_clk_conn();
        if(CPSS_OK != lRet)
        {
            return CPSS_ERROR ;
        }
        g_stCpssComPciTbl.ulIrqConnFlag = 1;
    }
#endif
#endif
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_drv_pci_init
* 功    能:
* 相关文档:
* 函数类型:
* 参    数:                                          
* 参数名称            类型            输入/输出         描述

* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/

INT32 cpss_com_drv_pci_init()
{
    INT32  lRet ;

#ifdef CPSS_DSP_CPU
    /*读取PCI的驱动信息*/
    CPSS_DSP_FILE_REC();
    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulDrvPciInitInCount);
    cpss_com_drv_dsp_pci_info_get() ;
#endif

    /*得到PCI链路的相关数据*/
    lRet = cpss_com_drv_pci_info_set(&g_stCpssComPciTbl);
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "pci init(pci info set) failed.");       
        return CPSS_ERROR ;
    }

    /*创建PCI写互斥量*/
    lRet = cpss_com_drv_pci_write_sem_create();
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,"pci init(create sem) failed.");
        return CPSS_ERROR ;
    }

#ifdef CPSS_HOST_CPU

    /*向irq任务注册中断号*/
    /*cpss_com_irq_register(CPSS_COM_PCI_V_IRQ_NO,CPSS_COM_LINK_MNGR_PROC);*/
    
    /*PCI驱动信息初始化*/
#ifdef CPSS_HOST_CPU_WITH_DSP
    cpss_pci_mem_init(CPSS_COM_PCI_MEM_WRITE_PERMIT);
#else
    cpss_pci_mem_init(CPSS_COM_PCI_MEM_WRITE_NOT_PERMIT);
#endif
    
    /*挂接驱动处理程序*/
    drv_pcibus_notify_reg(cpss_com_pci_callback_fun);

    /*如果对端CPU是DSP，则使能PCI读功能*/
    if(TRUE == cpss_com_link_if_dsp_link(g_stCpssComPciTbl.astPciLink[0].ulCpuNo))
    {
        lRet = cpss_com_drv_pci_enable_read() ;
        if(CPSS_OK != lRet)
        {
            return CPSS_ERROR ;
        }
    }
#else
    /*初始化PCI内存*/
    lRet = cpss_pci_mem_init(CPSS_COM_PCI_MEM_WRITE_PERMIT);
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,"cpss_pci_mem_init failed") ;
            
        return CPSS_ERROR ;
    }

    /*启动notify通知消息周期定时器*/
    lRet = cpss_timer_loop_set
        (CPSS_COM_PCI_NOTIFY_TM_NO,CPSS_COM_PCI_NOTIFY_TM_INTERVAL);
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,"pci notify timer set failed") ;
	CPSS_DSP_LINE_REC();
	CPSS_DSP_ERROR_WAIT;
        return CPSS_ERROR ;
    }
    
    /*使能PCI读功能*/
    lRet = cpss_com_drv_pci_enable_read() ;
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }
    
    /*从CPU,DSP上设置所有的链路可读可写*/
    cpss_com_drv_pci_wr_set_all(CPSS_COM_PCI_WR_ENABLE) ;

   #ifdef CPSS_SLAVE_CPU
   #ifdef CPSS_VOS_VXWORKS
       drv_pcibus_notify_reg(cpss_tm_sntp_pci_read);
   #endif
       cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"cpss_tm_sntp_pci_read reg") ;
   #endif
 
#endif

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,"cpss_com_drv_pci_init succ!");

    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulDrvPciInitOutCount);
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_wr_set
* 功    能: 
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* 函数返回: 
*          
* 说    明: 
*******************************************************************************/
INT32 cpss_com_drv_pci_wr_set
(
 UINT32 ulCpuNo,
 UINT32 ulWRFlag
)
{
    INT32 lRet ;
    UINT32 ulLinkId ;

    lRet = cpss_com_drv_pci_linkid_get(ulCpuNo,&ulLinkId) ;
    if(CPSS_OK != lRet)
    {
        return CPSS_ERR_COM_PCI_NOT_FINDE ;
    }

    /*设置读写标志*/
    g_stCpssComPciTbl.astPciLink[ulLinkId-1].ulWRFlag = ulWRFlag ;

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_wr_set_all
* 功    能: 
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* 函数返回: 
*          
* 说    明: 
*******************************************************************************/
VOID cpss_com_drv_pci_wr_set_all
(
 UINT32 ulWRFlag
)
{
    UINT32 ulLoop ;

    /*设置全部的读写标志*/    
    for(ulLoop=0;ulLoop<g_stCpssComPciTbl.ulCpuNum;ulLoop++)
    {
        g_stCpssComPciTbl.astPciLink[ulLoop].ulWRFlag = ulWRFlag ;
    }

}

/*******************************************************************************
* 函数名称: cpss_drv_pci_sem_release
* 功    能: 
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* 函数返回: 
*          
* 说    明: 
*******************************************************************************/
VOID cpss_drv_pci_sem_release()
{
    cpss_vos_mutex_v(g_stCpssComPciTbl.ulPciReadMutex) ;
}

/*******************************************************************************
* 函数名称: cpss_pci_aux_clk_conn
* 功    能: 
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* 函数返回: 
*          
* 说    明: 只有在VX使用
*******************************************************************************/
INT32 cpss_pci_aux_clk_conn(VOID)
{
#if 0
#ifdef CPSS_VOS_VXWORKS
    INT32 lRet ;
    lRet = sysAuxClkConnect(cpss_drv_pci_sem_release, 0);
    if (lRet != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,"cpss_pci_irq_conn fail!");
        return CPSS_ERROR;
    }
#endif
#endif
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_sum_check
* 功    能: 
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* 函数返回: 
*          
* 说    明: 
*******************************************************************************/
INT32 cpss_com_drv_pci_sum_check
(
    UINT32* pulBuf,
    UINT32 ulSum
)
{
    UINT32 ulLoop;
    UINT32 ulSumCheck ;

    ulSumCheck =0 ;

    for(ulLoop=0;ulLoop<ulSum -1;ulLoop++)
    {
        ulSumCheck = ulSumCheck + *(pulBuf+ulLoop);
    }

    if(ulSumCheck == *(pulBuf+ulSum-1))
    {
        return CPSS_OK ;
    }
    return CPSS_ERROR ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_special_recv_deal
* 功    能: PCI接收任务
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* 函数返回: 
*          
* 说    明: 
*******************************************************************************/
INT32 cpss_com_drv_pci_special_recv_deal(UINT32 ulLinkId)
{
    UINT32 ulCpuNo ;
    INT32  lRet ;
    CPSS_COM_PCI_REG_REQ_MSG_T stRegReq = {0} ;
    CPSS_COM_PCI_REG_RSP_MSG_T stRegRes = {0} ;
    CPSS_COM_MSG_HEAD_T stDispHdr;

    cpss_mem_memset(&stRegReq,0,sizeof(CPSS_COM_PCI_REG_REQ_MSG_T));
    cpss_mem_memset(&stDispHdr,0,sizeof(CPSS_COM_MSG_HEAD_T));
    
    ulCpuNo = g_stCpssComPciTbl.astPciLink[ulLinkId-1].ulCpuNo ;

#ifdef CPSS_HOST_CPU
    
    /*如果对方单板为DSP则读取特殊缓冲区*/
    if(TRUE == cpss_com_link_if_dsp_link(ulCpuNo))
    {
        /*从特殊内存中读取数据*/
        lRet = cpss_com_pci_special_read(ulLinkId,0,(UINT8*)&stRegReq,
            sizeof(CPSS_COM_PCI_REG_REQ_MSG_T));
        
        stRegReq.ulMsgId = cpss_ntohl(stRegReq.ulMsgId) ;
        stRegReq.ulCpuNo = cpss_ntohl(stRegReq.ulCpuNo) ;
        stRegReq.ulSumCheck = cpss_ntohl(stRegReq.ulSumCheck) ;

        if((0==stRegReq.ulMsgId)||
            (CPSS_COM_SCPU_REG_REQ_MSG!=stRegReq.ulMsgId))
        {
            return CPSS_ERROR;
        }

        /*对读到的数据进行校验和*/
        if(stRegReq.ulSumCheck != CPSS_COM_PCI_CHECK_VALUE)
        {
            return CPSS_ERROR;
        }

        /*回填CPU号*/
        stRegReq.ulCpuNo = ulCpuNo ;
        
        /*填充发送的参数*/
        stDispHdr.pucBuf = (UINT8*)&stRegReq ;
        stDispHdr.ulMsgId = stRegReq.ulMsgId ;
        stDispHdr.ulLen   = sizeof(CPSS_COM_PCI_REG_REQ_MSG_T) ;
        
    }
#else
    
    /*读取SPECIAL区中的响应数据*/

    /*从特殊内存中读取数据*/
    lRet = cpss_com_pci_special_read(ulLinkId,
        sizeof(CPSS_COM_PCI_REG_REQ_MSG_T),(UINT8*)&stRegRes,
        sizeof( CPSS_COM_PCI_REG_RSP_MSG_T));
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }
    
    
    if(cpss_ntohl(stRegRes.ulMsgId)!=CPSS_COM_MCPU_REG_RSP_MSG)
    {
        return CPSS_ERROR ;
    }

    /*对读到的数据进行校验和*/
    if(cpss_ntohl(stRegRes.ulCheckSum) != CPSS_COM_PCI_CHECK_VALUE)
    {
        return CPSS_ERROR;
    }

    stDispHdr.pucBuf = (UINT8*)&stRegRes ;
    stDispHdr.ulMsgId = cpss_ntohl(stRegRes.ulMsgId) ;
    stDispHdr.ulLen   = sizeof(CPSS_COM_PCI_REG_RSP_MSG_T) ;

#endif   
    
    stDispHdr.stDstProc.ulPd = CPSS_COM_LINK_MNGR_PROC;
    stDispHdr.ucAckFlag = CPSS_COM_NOT_ACK ;
    stDispHdr.ucShareFlag = CPSS_COM_MEM_NOT_SHARE ;
    stDispHdr.ucPriFlag = CPSS_COM_PRIORITY_NORMAL ; 
    
    /*发送到链路管理纤程*/
    
    lRet = cpss_com_send_local(&stDispHdr);
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_recv_task
* 功    能: PCI接收任务
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* 函数返回: 
*          
* 说    明: 
*******************************************************************************/
#ifndef CPSS_DSP_CPU
UINT32 g_ulCpssComPciTaskPList[1];
INT32 cpss_com_pci_task_init()
{
    /*等待PCI接收信号量正常*/
    while(0 == g_stCpssComPciTbl.ulPciReadMutex)
    {
        cpss_vos_task_delay(1000) ;
    }
    g_ulCpssComPciTaskPList[0] = 1;
    
    return (CPSS_OK);
}

INT32 cpss_com_pci_task_block(UINT32 *pulParamList)
{
    cpss_vos_task_delay(pulParamList[0]);
    
    return (CPSS_OK);
}

INT32 cpss_com_pci_task_dealing(INT32 lParam, UINT32 *pulParamList)
{
    INT32 lRet;
    UINT32 ulLoop ;
    UINT32 ulDataLen ;
    UINT8* pucRecvBuf ;
    UINT32 ulCpuNum;
    UINT32 ulLinkId;
    UINT32 ulRecvLen;
    CPSS_COM_DRV_T     *pstPciDrv ;
    CPSS_COM_MSG_HEAD_T* pstDispHdr ;    

/*得到从CPU或主CPU的个数*/
    ulCpuNum = g_stCpssComPciTbl.ulCpuNum ;
    
    /*循环扫描所有的从CPU*/
    for(ulLoop=0;ulLoop<ulCpuNum;ulLoop++)
    {
        ulLinkId = ulLoop+1 ;
        
        /*判断链路状态是否正常*/
        lRet = cpss_com_link_if_normal(g_stCpssComPciTbl.astPciLink[ulLoop].stDstPhyAddr);
        if(FALSE == lRet)
        {
#if 1    
            /*对读PCI特殊缓冲区的速度进行控制*/
            g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum = g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum + 1 ;
            if(g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum<CPSS_COM_DRV_PCI_READ_NUM_MAX)
            {
                continue ;
            }
            g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum = 0 ;
#endif
            /*处理接收特殊缓冲区中的数据*/
            cpss_com_drv_pci_special_recv_deal(ulLoop+1) ;
            
            continue ;
        }
        /*循环读取PCI内存中所有的数据*/
        while(1)
        {
            /*得到PCI数据的长度*/
            lRet = cpss_com_pci_read_len_get(ulLinkId,&ulRecvLen);
            
            if((CPSS_OK !=lRet)||(0 == ulRecvLen))
            {
                break ;
            }
            
            /*得到实际数据的长度ulRecvLen=trustHdr+dispHdr+dataLen*/
            ulDataLen =  CPSS_LINK_HDR_LEN_SUBTRACT(ulRecvLen) ;
            
            /*申请接收内存*/
            pucRecvBuf = cpss_com_mem_alloc(ulDataLen) ;
            if(NULL==pucRecvBuf)
            {
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                    "PCI recv task malloc failed(%d)",ulDataLen);
                break ;
            }
            
            pstPciDrv=cpss_com_drv_find(CPSS_COM_DRV_PCI);
            
            /*pci读取数据的统计flag = 2*/                
            cpss_com_pci_link_stat(ulLinkId,CPSS_COM_PCI_RECV_NUM_FLAG);

            /*调用PCI驱动读取PCI数据*/
            lRet = cpss_com_pci_read(pucRecvBuf,ulLinkId) ;
            if(CPSS_OK != lRet)
            {
                cpss_com_mem_free(pucRecvBuf) ;
                continue;
            }
            
            /*增加统计*/
            pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pucRecvBuf) ;
            cpss_com_spec_msg_stat(pstDispHdr->ulMsgId,DRV_PCI_RECV_PHASE) ;            
            
            /* 把收到的报文交给驱动适配层来处理 */
            if((pstPciDrv->pfRead != NULL) &&
                (CPSS_OK == pstPciDrv->pfRead(CPSS_COM_DRV_PCI,ulLinkId,pucRecvBuf,ulRecvLen)))
            {
                continue;
            }
        }
    }

    return (CPSS_OK);
    
}
INT32 cpss_com_pci_task_reg()
{
    INT32 lRet;
    lRet = cpss_vos_ntask_register(
              CPSS_NORMAL_TASK_TYPE_DELAY,
              cpss_com_pci_task_init, 
              cpss_com_pci_task_block,
              cpss_com_pci_task_dealing,
              g_ulCpssComPciTaskPList,
              CPSS_COM_PCI_TASKNAME);
    if ( lRet != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                "Pci listen task register failed! errCode = %#x \n", lRet);
        return (CPSS_ERROR);
    }

    return (CPSS_OK);
    
}

VOID cpss_com_drv_pci_recv_task(VOID)
{    
    INT32  lRet;
    UINT32 ulLoop ;
    UINT32 ulDataLen ;
    UINT8* pucRecvBuf ;
    UINT32 ulCpuNum;
    UINT32 ulLinkId;
    UINT32 ulRecvLen;
    CPSS_COM_DRV_T     *pstPciDrv ;
    CPSS_COM_MSG_HEAD_T* pstDispHdr ;
    
    /*等待PCI接收信号量正常*/
#if (defined CPSS_VOS_WINDOWS)||(defined CPSS_VOS_VXWORKS)
    while(0 == g_stCpssComPciTbl.ulPciReadMutex)
    {
        cpss_vos_task_delay(1000) ;
    }
#endif
#if (defined CPSS_VOS_WINDOWS)||(defined CPSS_VOS_VXWORKS)
    while (TRUE)
#endif
    {
#if 0      
#ifdef CPSS_DSP_CPU
        lRet = cpss_vos_sem_p(g_stCpssComPciTbl.ulPciReadMutex, WAIT_FOREVER);
        if (lRet != CPSS_OK)
        {
            continue;
        }
#endif
#endif
        cpss_kw_set_task_run() ;

        /*得到从CPU或主CPU的个数*/
        ulCpuNum = g_stCpssComPciTbl.ulCpuNum ;
        
        /*循环扫描所有的从CPU*/
        for(ulLoop=0;ulLoop<ulCpuNum;ulLoop++)
        {
            ulLinkId = ulLoop+1 ;
            
            /*判断链路状态是否正常*/
            lRet = cpss_com_link_if_normal(g_stCpssComPciTbl.astPciLink[ulLoop].stDstPhyAddr);
            if(FALSE == lRet)
            {
#if 1    
                /*对读PCI特殊缓冲区的速度进行控制*/
                g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum = g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum + 1 ;
                if(g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum<CPSS_COM_DRV_PCI_READ_NUM_MAX)
                {
                    continue ;
                }
                g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum = 0 ;
#endif
                /*处理接收特殊缓冲区中的数据*/
                cpss_com_drv_pci_special_recv_deal(ulLoop+1) ;
                
                continue ;
            }
            /*循环读取PCI内存中所有的数据*/
            while(1)
            {
                /*得到PCI数据的长度*/
                lRet = cpss_com_pci_read_len_get(ulLinkId,&ulRecvLen);
                
                if((CPSS_OK != lRet)||(0 == ulRecvLen))
                {
                    break ;
                }
                
                /*得到实际数据的长度ulRecvLen=trustHdr+dispHdr+dataLen*/
                ulDataLen =  CPSS_LINK_HDR_LEN_SUBTRACT(ulRecvLen) ;
                
                /*申请接收内存*/
                pucRecvBuf = cpss_com_mem_alloc(ulDataLen) ;
                if(NULL==pucRecvBuf)
                {
                    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                        "PCI recv task malloc failed(%d)",ulDataLen);
                    break ;
                }
                
                pstPciDrv=cpss_com_drv_find(CPSS_COM_DRV_PCI);
                
                /*pci读取数据的统计flag = 2*/                
                cpss_com_pci_link_stat(ulLinkId,CPSS_COM_PCI_RECV_NUM_FLAG);

                /*调用PCI驱动读取PCI数据*/
                lRet = cpss_com_pci_read(pucRecvBuf,ulLinkId) ;
                if(CPSS_OK != lRet)
                {
                    cpss_com_mem_free(pucRecvBuf) ;
                    continue ;
                }
                
                /*增加统计*/
                pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pucRecvBuf) ;
                cpss_com_spec_msg_stat(pstDispHdr->ulMsgId,DRV_PCI_RECV_PHASE) ;               
                
                /* 把收到的报文交给驱动适配层来处理 */
                if((pstPciDrv->pfRead != NULL) &&
                    (CPSS_OK == pstPciDrv->pfRead(CPSS_COM_DRV_PCI,ulLinkId,pucRecvBuf,ulRecvLen)))
                {
/*                    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                        "PCI recv task deliver succ\n");*/
                    continue;
                }
            }
        }
#if (defined CPSS_VOS_WINDOWS)||(defined CPSS_VOS_VXWORKS)
        cpss_kw_set_task_stop() ;
        cpss_vos_task_delay(1) ;
#endif

    }       
}
#else
INT32 cpss_com_drv_pci_recv_task(VOID)
{    
    INT32  lRet;
    UINT32 ulLoop ;
    UINT32 ulDataLen ;
    UINT8* pucRecvBuf ;
    UINT32 ulCpuNum;
    UINT32 ulLinkId;
    UINT32 ulRecvLen;
    CPSS_COM_DRV_T     *pstPciDrv ;
    CPSS_COM_MSG_HEAD_T* pstDispHdr ;
    
        /*得到从CPU或主CPU的个数*/
    ulCpuNum = g_stCpssComPciTbl.ulCpuNum ;
    /*循环扫描所有的从CPU*/
    for(ulLoop=0;ulLoop<ulCpuNum;ulLoop++)
    {
        ulLinkId = ulLoop+1 ;

        /*判断链路状态是否正常*/
        lRet = cpss_com_link_if_normal(g_stCpssComPciTbl.astPciLink[ulLoop].stDstPhyAddr);
        if(FALSE == lRet)
        {

            /*对读PCI特殊缓冲区的速度进行控制*/
            g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum++ ;
            if(g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum<CPSS_COM_DRV_PCI_READ_NUM_MAX)
            {
                return CPSS_ERROR ;
            }
            g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum = 0 ;

            /*处理接收特殊缓冲区中的数据*/
            cpss_com_drv_pci_special_recv_deal(ulLoop+1) ;
            
            return CPSS_ERROR ;
        }
        
        
            /*得到PCI数据的长度*/
        lRet = cpss_com_pci_read_len_get(ulLinkId,&ulRecvLen);
        
        if((CPSS_OK != lRet)||(0 == ulRecvLen))
        {
            return CPSS_ERROR ;
        }
        
        /*得到实际数据的长度ulRecvLen=trustHdr+dispHdr+dataLen*/
        ulDataLen =  CPSS_LINK_HDR_LEN_SUBTRACT(ulRecvLen) ;
        
        /*申请接收内存*/
        pucRecvBuf = cpss_com_mem_alloc(ulDataLen) ;
        if(NULL==pucRecvBuf)
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "PCI recv task malloc failed(%d)",ulDataLen);
            return CPSS_ERROR ;
        }
        
        pstPciDrv=cpss_com_drv_find(CPSS_COM_DRV_PCI);
        
        /*pci读取数据的统计flag = 2*/                
        cpss_com_pci_link_stat(ulLinkId,CPSS_COM_PCI_RECV_NUM_FLAG);

        /*调用PCI驱动读取PCI数据*/
        lRet = cpss_com_pci_read(pucRecvBuf,ulLinkId) ;
        if(CPSS_OK != lRet)
        {
            cpss_com_mem_free(pucRecvBuf) ;
            return CPSS_ERROR ;
        }
        
        /*增加统计*/
        pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pucRecvBuf) ;
        cpss_com_spec_msg_stat(pstDispHdr->ulMsgId,DRV_PCI_RECV_PHASE) ;
        
        /* 把收到的报文交给驱动适配层来处理 */
        if((pstPciDrv->pfRead != NULL) &&
            (CPSS_OK == pstPciDrv->pfRead(CPSS_COM_DRV_PCI,ulLinkId,pucRecvBuf,ulRecvLen)))
        {
/*            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "PCI recv task deliver succ\n");*/
             return CPSS_OK;
        }    
        return CPSS_ERROR;
    }    
}
#endif

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_recv_dealing
* 功    能: PCI消息处理
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* 函数返回: 
*          
* 说    明:     
*******************************************************************************/
VOID cpss_com_drv_pci_recv_dealing(VOID)
{    
    INT32  lRet;
    UINT32 ulLoop ;
    UINT32 ulDataLen ;
    UINT8* pucRecvBuf ;
    UINT32 ulCpuNum;
    UINT32 ulLinkId;
    UINT32 ulRecvLen;
    CPSS_COM_DRV_T     *pstPciDrv ;
    
    
    /*得到从CPU或主CPU的个数*/
    ulCpuNum = g_stCpssComPciTbl.ulCpuNum ;
        
        /*循环扫描所有的从CPU*/
    for(ulLoop=0;ulLoop<ulCpuNum;ulLoop++)
    {
        ulLinkId = ulLoop+1 ;
        
        /*判断链路状态是否正常*/
        lRet = cpss_com_link_if_normal(g_stCpssComPciTbl.astPciLink[ulLoop].stDstPhyAddr);
        if(FALSE == lRet)
        {
            /*对读PCI特殊缓冲区的速度进行控制*/
            g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum = g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum + 1 ;
            if(g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum<CPSS_COM_DRV_PCI_READ_NUM_MAX)
            {
                continue ;
            }
            g_stCpssComPciTbl.astPciLink[ulLoop].ulPciReadNum = 0 ;
            /*处理接收特殊缓冲区中的数据*/
            cpss_com_drv_pci_special_recv_deal(ulLoop+1) ;
            
            continue ;
        }
        /*循环读取PCI内存中所有的数据*/
        while(1)
        {
            /*得到PCI数据的长度*/
            lRet = cpss_com_pci_read_len_get(ulLinkId,&ulRecvLen);
            
            if((CPSS_OK != lRet)||(0 == ulRecvLen))
            {
                break ;
            }
            
            /*得到实际数据的长度ulRecvLen=trustHdr+dispHdr+dataLen*/
            ulDataLen =  CPSS_LINK_HDR_LEN_SUBTRACT(ulRecvLen) ;
            
            /*申请接收内存*/
            pucRecvBuf = cpss_com_mem_alloc(ulDataLen) ;
            if(NULL==pucRecvBuf)
            {
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                    "PCI recv task malloc failed(%d)",ulDataLen);
                break ;
            }
            
            pstPciDrv=cpss_com_drv_find(CPSS_COM_DRV_PCI);
            
            /*调用PCI驱动读取PCI数据*/
            cpss_com_pci_read(pucRecvBuf,ulLinkId) ;
            
            /* 把收到的报文交给驱动适配层来处理 */
            if((pstPciDrv->pfRead != NULL) &&
                (CPSS_OK == pstPciDrv->pfRead(CPSS_COM_DRV_PCI,ulLinkId,pucRecvBuf,ulRecvLen)))
            {
                continue;
            }
            /*cpss_com_mem_free(pucRecvBuf);*/
        }
    }
}

/*******************************************************************************
* 函数名称: cpss_pci_info_write                            
* 功    能: 提供给SMSS写DSP的外存。
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
*   ulIp              UINT32          输入              IP
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*        该函数是模拟SHELL类函数。
******************************************************************************/
INT32 cpss_pci_info_write(UINT32 vuiCpuID)
{
#ifdef CPSS_HOST_CPU_WITH_DSP
    UINT32 ulLoop = 0 ;
    UINT32 ulCpuNum = 0 ;
    INT32  lRet ;
    
    DRV_PCI_INFO_T astDrvPciInfo[CPSS_DRV_PCI_INFO_ARRAY_MAX] ;
    DRV_PCI_INFO_T *pstDrvPciLink ;
    UINT32 ulCpuId = 0;
    
    pstDrvPciLink = astDrvPciInfo ;
    
    /*得到PCI驱动的信息*/
    lRet = cpss_drv_pci_info_get(&ulCpuNum,pstDrvPciLink);
    if(CPSS_ERROR == lRet)
    {
        return CPSS_ERROR ;
    }

    for(ulLoop = 0; ulLoop < ulCpuNum; ulLoop++)
    {
        ulCpuId = astDrvPciInfo[ulLoop].ulCpuId - 5;
        if(ulCpuId == vuiCpuID)
        {
            if(TRUE != cpss_com_link_if_dsp_link(astDrvPciInfo[ulLoop].ulCpuId))
            {
                return CPSS_ERROR ;
            }
            astDrvPciInfo[ulLoop].ulStartAddr = astDrvPciInfo[ulLoop].ulStartAddr + MPC8560_MEM_PCI_BASE_ADDR;
            astDrvPciInfo[ulLoop].ulStartAddr = cpss_htonl(astDrvPciInfo[ulLoop].ulStartAddr) ;
            astDrvPciInfo[ulLoop].ulLen       = cpss_htonl(astDrvPciInfo[ulLoop].ulLen) ;
            astDrvPciInfo[ulLoop].ulCpuId     = cpss_htonl(1) ;
            astDrvPciInfo[ulLoop].ulRamWinAttr= cpss_htonl(astDrvPciInfo[ulLoop].ulRamWinAttr) ;

            lRet = drv_dsp_ext_mem_write(ulCpuId, DRV_PCI_DSP_ADDR_OFFSET, sizeof(DRV_PCI_INFO_T), (UINT8 *)&astDrvPciInfo[ulLoop]) ;
            if(CPSS_ERROR == lRet)
            {
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,"drv_dsp_ext_mem_write failed, ulCpuId = %d.\n", ulCpuId) ;
            }
            return CPSS_OK;
        }
    }

#endif
    return CPSS_ERROR ;
}

#endif /* end "#ifdef CPSS_PCI_INCLUDE" */

/*******************************************************************************
* 函数名称: cpss_pci_info_write                            
* 功    能: 提供给SMSS写DSP的外存。
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
*   ulIp              UINT32          输入              IP
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*        在windows下定义此空函数解决编译问题。
******************************************************************************/
#ifdef CPSS_VOS_WINDOWS
INT32 cpss_pci_info_write(UINT32 vuiCpuID)
{
    return CPSS_OK ;
}
#endif

/*******************************************************************************
* 函数名称: cpss_com_tcp_link_show
* 功    能: tcp链路统计函数
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* 函数返回: 
*          
* 说    明: 
*******************************************************************************/
VOID cpss_com_tcp_link_show()
{
    UINT32 ulIdx;
    UINT32 ulTcpLinkNum = 0 ;
    
    cps__oams_shcmd_printf("---- server tcp link show begin ----\n") ;
    
    for(ulIdx=0; ulIdx<CPSS_COM_DRV_TCP_NUM; ulIdx++)
    {
        if(g_stCpssTcpMan.astServerTcpLink[ulIdx].ulSClient == 0)
        {                
             continue;
        }
        cps__oams_shcmd_printf("\n") ;        
        
         cps__oams_shcmd_printf("IP addr 0x%08x\n",
             g_stCpssTcpMan.astServerTcpLink[ulIdx].stTcpLink.ulIP,
             g_stCpssTcpMan.astServerTcpLink[ulIdx].stTcpLink.usTcpPort);
         cps__oams_shcmd_printf("DeliverErrNum:%d,TcpSendFailNum:%d\n",
             g_stCpssTcpMan.astServerTcpLink[ulIdx].stLinkDataStat.ulDeliverErrNum,
             g_stCpssTcpMan.astServerTcpLink[ulIdx].stLinkDataStat.ulTcpSendFailNum);
         cps__oams_shcmd_printf("RecvNum:%d,SendNum:%d\n",
             g_stCpssTcpMan.astServerTcpLink[ulIdx].stLinkDataStat.ulRecvNum,
             g_stCpssTcpMan.astServerTcpLink[ulIdx].stLinkDataStat.ulSendNum);
         cps__oams_shcmd_printf("TcpStackNotEnoughNum:%d,lLinkSetupNum:%d\n",
             g_stCpssTcpMan.astServerTcpLink[ulIdx].stLinkDataStat.ulTcpStackNotEnoughNum,
             g_stCpssTcpMan.astServerTcpLink[ulIdx].stLinkSetupDelStat.ulLinkSetupNum);
         cps__oams_shcmd_printf("LinkSetupDelStat:%d,LinkFaultPassiveNum:%d\n",
             g_stCpssTcpMan.astServerTcpLink[ulIdx].stLinkSetupDelStat.ulLinkFaultActiveNum,
             g_stCpssTcpMan.astServerTcpLink[ulIdx].stLinkSetupDelStat.ulLinkFaultPassiveNum);
         ulTcpLinkNum++ ;
        cps__oams_shcmd_printf("\n") ;
    }
    cps__oams_shcmd_printf("---- there are %d server TCP connect! ----\n",ulTcpLinkNum);

    cps__oams_shcmd_printf("\n") ;

    cps__oams_shcmd_printf("---- server dc link show begin ----\n") ;
    ulTcpLinkNum = 0 ;
    
    for(ulIdx=0; ulIdx<CPSS_COM_DRV_DC_TCP_NUM; ulIdx++)
    {
        if(g_stCpssTcpMan.astServerDcLink[ulIdx].ulSClient == 0)
        {                
            continue;
        }

        cps__oams_shcmd_printf("\n") ;        
        
        cps__oams_shcmd_printf("IP addr 0x%08x\n",
            g_stCpssTcpMan.astServerDcLink[ulIdx].stTcpLink.ulIP,
            g_stCpssTcpMan.astServerDcLink[ulIdx].stTcpLink.usTcpPort);
        cps__oams_shcmd_printf("DeliverErrNum:%d,TcpSendFailNum:%d\n",
            g_stCpssTcpMan.astServerDcLink[ulIdx].stLinkDataStat.ulDeliverErrNum,
            g_stCpssTcpMan.astServerDcLink[ulIdx].stLinkDataStat.ulTcpSendFailNum);
        cps__oams_shcmd_printf("RecvNum:%d,SendNum:%d\n",
            g_stCpssTcpMan.astServerDcLink[ulIdx].stLinkDataStat.ulRecvNum,
            g_stCpssTcpMan.astServerDcLink[ulIdx].stLinkDataStat.ulSendNum);
        cps__oams_shcmd_printf("TcpStackNotEnoughNum:%d,lLinkSetupNum:%d\n",
            g_stCpssTcpMan.astServerDcLink[ulIdx].stLinkDataStat.ulTcpStackNotEnoughNum,
            g_stCpssTcpMan.astServerDcLink[ulIdx].stLinkSetupDelStat.ulLinkSetupNum);
        cps__oams_shcmd_printf("LinkSetupDelStat:%d,LinkFaultPassiveNum:%d\n",
            g_stCpssTcpMan.astServerDcLink[ulIdx].stLinkSetupDelStat.ulLinkFaultActiveNum,
            g_stCpssTcpMan.astServerDcLink[ulIdx].stLinkSetupDelStat.ulLinkFaultPassiveNum);
        ulTcpLinkNum++ ;
        cps__oams_shcmd_printf("\n") ;        
    }
    cps__oams_shcmd_printf("---- there are %d server dc connect! ----\n",ulTcpLinkNum);     
       
    cps__oams_shcmd_printf("\n") ;    

    cps__oams_shcmd_printf("---- client tcp link show begin ----\n") ;
    ulTcpLinkNum = 0 ;
    
    for(ulIdx=0; ulIdx<CPSS_COM_DRV_CLIENT_TCP_NUM; ulIdx++)
    {
        if(g_stCpssTcpMan.astClientTcpLink[ulIdx].ulSClient == 0)
        {                
            continue;
    }

        cps__oams_shcmd_printf("\n") ;        
        
        cps__oams_shcmd_printf("IP addr 0x%08x\n",
            g_stCpssTcpMan.astClientTcpLink[ulIdx].stTcpLink.ulIP,
            g_stCpssTcpMan.astClientTcpLink[ulIdx].stTcpLink.usTcpPort);
        cps__oams_shcmd_printf("DeliverErrNum:%d,TcpSendFailNum:%d\n",
            g_stCpssTcpMan.astClientTcpLink[ulIdx].stLinkDataStat.ulDeliverErrNum,
            g_stCpssTcpMan.astClientTcpLink[ulIdx].stLinkDataStat.ulTcpSendFailNum);
        cps__oams_shcmd_printf("RecvNum:%d,SendNum:%d\n",
            g_stCpssTcpMan.astClientTcpLink[ulIdx].stLinkDataStat.ulRecvNum,
            g_stCpssTcpMan.astClientTcpLink[ulIdx].stLinkDataStat.ulSendNum);
        cps__oams_shcmd_printf("TcpStackNotEnoughNum:%d,lLinkSetupNum:%d\n",
            g_stCpssTcpMan.astClientTcpLink[ulIdx].stLinkDataStat.ulTcpStackNotEnoughNum,
            g_stCpssTcpMan.astClientTcpLink[ulIdx].stLinkSetupDelStat.ulLinkSetupNum);
        cps__oams_shcmd_printf("LinkSetupDelStat:%d,LinkFaultPassiveNum:%d\n",
            g_stCpssTcpMan.astClientTcpLink[ulIdx].stLinkSetupDelStat.ulLinkFaultActiveNum,
            g_stCpssTcpMan.astClientTcpLink[ulIdx].stLinkSetupDelStat.ulLinkFaultPassiveNum);
        ulTcpLinkNum++ ;
        cps__oams_shcmd_printf("\n") ;        
    }
    cps__oams_shcmd_printf("---- there are %d client tcp connect! ----\n",ulTcpLinkNum);  
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_link_show
* 功    能: tcp链路统计函数
* 函数类型: 
* 参    数: 
* 参数名称        类型                  输入/输出         描述
* 函数返回: 
*          
* 说    明: 
*******************************************************************************/
VOID cpss_com_tcp_err_show()
{
    cps__oams_shcmd_printf("---- tcp switch connect errshow ------\n") ;

    cps__oams_shcmd_printf("    ulAcceptErrNum        = %d\n",g_stCpssTcpErrStat.ulAcceptErrNum) ;
    cps__oams_shcmd_printf("    ulNoFreeSpaceNum      = %d\n",g_stCpssTcpErrStat.ulNoFreeSpaceNum) ;
    cps__oams_shcmd_printf("    ulConnectSendFailNum  = %d\n",g_stCpssTcpErrStat.ulConnectSendFailNum) ;
    cps__oams_shcmd_printf("    ulRecvErrNum          = %d\n",g_stCpssTcpErrStat.ulRecvErrNum) ;

    cps__oams_shcmd_printf("---- tcp onca connect errshow ------\n") ;
    cps__oams_shcmd_printf("    ulRecvErrNum          = %d\n",g_stCpssTcpErrStat.ulListenFailNum) ;
    cps__oams_shcmd_printf("    ulLinkAddFailNum      = %d\n",g_stCpssTcpErrStat.ulLinkAddFailNum) ;
    cps__oams_shcmd_printf("    ulExceptionNum        = %d\n",g_stCpssTcpErrStat.ulExceptionNum) ;    
}

#ifdef CPSS_UDP_INCLUDE
/*******************************************************************************
* 函数名称: cpss_udp_drv_stat_show                            
* 功    能: UDP驱动统计显示函数。
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
*   ulIp              UINT32          输入              IP
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*        该函数是模拟SHELL类函数。
******************************************************************************/
VOID cpss_udp_drv_stat_show(UINT32 ulIp)
{
    INT32 lLoc;
    CPSS_UDP_DRV_STAT_T *pulStat;

    lLoc = cpss_udp_stat_loc_get(ulIp);
    if (CPSS_ERROR == lLoc)
    {
        cps__oams_shcmd_printf ("\n Invalid Ip(%#x) to stat. \n", ulIp);
        return ;
    }
    
    pulStat = gp_stCpssComUdpStat + lLoc;

    cps__oams_shcmd_printf ("\n Recv from Ip(0x%x)'s package num = %d, send : %d, and send error: %d \n", 
        ulIp, pulStat->ulRecvNum, pulStat->ulSendNum, pulStat->ulSendErrNum);

    return ;
    
}
#else
VOID cpss_udp_drv_stat_show(UINT32 ulIp)
{
    cps__oams_shcmd_printf("udp stat show can not support!!!\n");    
}
#endif

void cpss_drv_udp_recv_show()
{
    cps__oams_shcmd_printf("The board udp recv data num :%d \n",g_ulUdpRecvNum);    
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_server_link_find
* 功    能: 
* 函数类型: 
* 参    数: 
* 函数返回: 
* 说    明: 
*******************************************************************************/
#ifdef CPSS_TCP_INCLUDE
CPSS_COM_DRV_TCP_T* cpss_com_tcp_server_link_find
(
 CPSS_COM_TCP_LINK_T* pstTcpLink
)
{
    UINT32 ulIndex ;

    /*查找server tcp 链路*/
    for(ulIndex=0;ulIndex<CPSS_COM_DRV_TCP_NUM;ulIndex++)
    {
        if((pstTcpLink->ulIP == g_stCpssTcpMan.astServerTcpLink[ulIndex].stTcpLink.ulIP)&&
            (pstTcpLink->usTcpPort == g_stCpssTcpMan.astServerTcpLink[ulIndex].stTcpLink.usTcpPort))
        {
            return &g_stCpssTcpMan.astServerTcpLink[ulIndex] ;
        }
    }

    /*查找网元直连链路*/
    for(ulIndex=0;ulIndex<CPSS_COM_DRV_DC_TCP_NUM;ulIndex++)
    {
        if((pstTcpLink->ulIP == g_stCpssTcpMan.astServerDcLink[ulIndex].stTcpLink.ulIP)&&
            (pstTcpLink->usTcpPort == g_stCpssTcpMan.astServerDcLink[ulIndex].stTcpLink.usTcpPort))
        {
            return &g_stCpssTcpMan.astServerDcLink[ulIndex] ;
        }
    }
    
    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_tcp_server_link_find failed ip=0x%x,port=0x%x\n",
        pstTcpLink->ulIP,pstTcpLink->usTcpPort) ;    
    return NULL ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_client_link_find
* 功    能: 
* 函数类型: 
* 参    数: 
* 函数返回: 
* 说    明: 
*******************************************************************************/
CPSS_COM_DRV_TCP_T* cpss_com_tcp_client_link_find
(
 CPSS_COM_TCP_LINK_T* pstTcpLink
)
{
    UINT32 ulIndex ;
    
    /*查找server tcp 链路*/
    for(ulIndex=0;ulIndex<CPSS_COM_DRV_CLIENT_TCP_NUM;ulIndex++)
    {
        if((pstTcpLink->ulIP == g_stCpssTcpMan.astClientTcpLink[ulIndex].stTcpLink.ulIP)&&
            (pstTcpLink->usTcpPort == g_stCpssTcpMan.astClientTcpLink[ulIndex].stTcpLink.usTcpPort))
        {
            return &g_stCpssTcpMan.astClientTcpLink[ulIndex] ;
        }
    }
    
    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_tcp_client_link_find failed ip=0x%x,port=0x%x\n",
        pstTcpLink->ulIP,pstTcpLink->usTcpPort) ;
    return NULL ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_link_find
* 功    能: 查找tcp链路在链路表中的空间
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
CPSS_COM_DRV_TCP_T* cpss_com_tcp_link_find
(
 CPSS_COM_TCP_LINK_T* pstTcpLink,
 UINT32 *pulServerFlag
)
{
    CPSS_COM_DRV_TCP_T* pstTcpDrv = NULL ;

    pstTcpDrv = cpss_com_tcp_server_link_find(pstTcpLink) ;
    if(pstTcpDrv != NULL)
    {
        *pulServerFlag = pstTcpDrv->ulServerFlag ;
        return pstTcpDrv ;
    }

    pstTcpDrv = cpss_com_tcp_client_link_find(pstTcpLink) ;
    if(pstTcpDrv != NULL)
    {
        *pulServerFlag = CPSS_COM_TCP_CLIENT_FLAG ;
        return pstTcpDrv ;
    }

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_tcp_link_find failed ip=0x%x,port=0x%x\n",
        pstTcpLink->ulIP,pstTcpLink->usTcpPort) ;    
    return NULL ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_server_link_id_find
* 功    能: 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_server_link_id_find
(
 CPSS_COM_TCP_LINK_T* pstTcpLink,
 UINT32 *pulLinkId
)
{
    UINT32 ulIndex ;

    for(ulIndex=0 ;ulIndex<CPSS_COM_DRV_TCP_NUM ;ulIndex++)
    {
        if((pstTcpLink->ulIP == g_stCpssTcpMan.astServerTcpLink[ulIndex].stTcpLink.ulIP)&&
            (pstTcpLink->usTcpPort == g_stCpssTcpMan.astServerTcpLink[ulIndex].stTcpLink.usTcpPort))
        {
            *pulLinkId = ulIndex ;
            return CPSS_OK ;
        }
    }

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_tcp_server_link_id_find failed ip=0x%x,port=0x%x\n",
        pstTcpLink->ulIP,pstTcpLink->usTcpPort) ;
    return CPSS_ERROR ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_server_free_link_find
* 功    能: 查找tcp链路在链路表中的空间
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
CPSS_COM_DRV_TCP_T* cpss_com_tcp_server_free_link_find
(
 UINT32 ulServerFlag
)
{
    UINT32 ulIndex ;
    
    /*查找server tcp 链路*/
    if(ulServerFlag == CPSS_COM_TCP_SERVER_FLAG)
    {
    for(ulIndex=0;ulIndex<CPSS_COM_DRV_TCP_NUM;ulIndex++)
    {
        if(g_stCpssTcpMan.astServerTcpLink[ulIndex].ulSClient == 0)
        {
            return &g_stCpssTcpMan.astServerTcpLink[ulIndex] ;
        }
    }
    }
    /*查找直连链路*/
    else if(ulServerFlag == CPSS_COM_TCP_DC_FLAG)
    {
        for(ulIndex=0;ulIndex<CPSS_COM_DRV_DC_TCP_NUM;ulIndex++)
        {
            if(g_stCpssTcpMan.astServerDcLink[ulIndex].ulSClient == 0)
            {
                return &g_stCpssTcpMan.astServerDcLink[ulIndex] ;
            }
        }
    }

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_tcp_server_free_link_find there is no free position\n") ;        
    return NULL ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_client_free_link_find
* 功    能: 查找tcp链路在链路表中的空间
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
CPSS_COM_DRV_TCP_T* cpss_com_tcp_client_free_link_find()
{
    UINT32 ulIndex ;
    
    /*查找server tcp 链路*/
    for(ulIndex=0;ulIndex<CPSS_COM_DRV_CLIENT_TCP_NUM;ulIndex++)
    {
        if(g_stCpssTcpMan.astClientTcpLink[ulIndex].ulSClient == 0)
        {
            return &g_stCpssTcpMan.astClientTcpLink[ulIndex] ;
        }
    }

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_tcp_client_free_link_find not find\n") ;    
    return NULL ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_server_link_find_by_socket
* 功    能: 通过socketId找到TCP的链路对象
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
CPSS_COM_DRV_TCP_T* cpss_com_tcp_server_link_find_by_socket
(
 UINT32 ulSocketId
)
{
    UINT32 ulLoop ;
    
    /*循环查找*/

    for(ulLoop = 0 ; ulLoop < CPSS_COM_DRV_TCP_NUM ;ulLoop++)
    {
        /*如果socket相等*/       
        if(g_stCpssTcpMan.astServerTcpLink[ulLoop].ulSClient == ulSocketId)
        {
            return &g_stCpssTcpMan.astServerTcpLink[ulLoop] ;
        }
    }
    
    /*查找直连链路*/
    for(ulLoop = 0 ; ulLoop < CPSS_COM_DRV_DC_TCP_NUM ;ulLoop++)
    {
        /*如果socket相等*/
        if(g_stCpssTcpMan.astServerDcLink[ulLoop].ulSClient == ulSocketId)
        {
            return &g_stCpssTcpMan.astServerDcLink[ulLoop] ;
        }
    }

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_tcp_server_link_find_by_socket not find socket=%d\n",ulSocketId) ;    
    return NULL ; 
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_server_link_find_by_socket
* 功    能: 通过socketId找到TCP的链路对象
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
CPSS_COM_DRV_TCP_T* cpss_com_tcp_client_link_find_by_socket
(
 UINT32 ulSocketId
)
{
    UINT32 ulLoop ;
    
    /*循环查找*/
    for(ulLoop = 0 ; ulLoop < CPSS_COM_DRV_CLIENT_TCP_NUM ;ulLoop++)
    {
        /*如果socket相等*/       
        if(g_stCpssTcpMan.astClientTcpLink[ulLoop].ulSClient == ulSocketId)
        {
            return &g_stCpssTcpMan.astClientTcpLink[ulLoop] ;
        }
    }
    
    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_tcp_client_link_find_by_socket not find ulSocketId=%d\n",ulSocketId) ;    
    return NULL ; 
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_server_link_find_by_socket
* 功    能: 通过socketId找到TCP的链路对象
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
CPSS_COM_DRV_TCP_T* cpss_com_tcp_link_find_by_socket
(
 UINT32 ulSocketId,
 UINT32 *pulServerFlag
)
{
    CPSS_COM_DRV_TCP_T* pstTcpDrv = NULL ;

    /*查找server tcp link*/
    pstTcpDrv = cpss_com_tcp_server_link_find_by_socket(ulSocketId) ;
    if(pstTcpDrv != NULL)
    {
        *pulServerFlag = pstTcpDrv->ulServerFlag ;
        return pstTcpDrv ;
    }

    /*查找client tcp link*/
    pstTcpDrv = cpss_com_tcp_client_link_find_by_socket(ulSocketId) ;
    if(pstTcpDrv != NULL)
    {
        *pulServerFlag = CPSS_COM_TCP_CLIENT_FLAG ;
        return pstTcpDrv ;
    }

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_tcp_link_find_by_socket not find ulSocketId=%d\n",ulSocketId) ;    
    return NULL ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_server_socket_free
* 功    能: 释放server表中的socket
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_server_socket_free
(
 UINT32 ulSocketId
)
{
    CPSS_COM_DRV_TCP_T* pstDrvTcp = NULL ;
    
    pstDrvTcp = cpss_com_tcp_server_link_find_by_socket(ulSocketId) ;
    if(pstDrvTcp == NULL)
    {
        return CPSS_ERROR ;
    }

    closesocket(ulSocketId) ;

    pstDrvTcp->ulSClient = 0 ;

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_link_free
* 功    能: 释放tcp链路在链路表中的空间
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_link_free
(
 UINT32 ulSocketId
)
{
    INT32 lRet ;
    UINT8* pucFreeBuf ;
    CPSS_COM_DRV_TCP_T* pstTcpDrv = NULL;

    /*查询tcp的server表*/
    pstTcpDrv = cpss_com_tcp_server_link_find_by_socket(ulSocketId) ;
    if(pstTcpDrv != NULL)
    {
        pucFreeBuf = pstTcpDrv->stRecvBufM.pucDataBuf ;
        cpss_mem_memset(pstTcpDrv,0,sizeof(CPSS_COM_DRV_TCP_T)) ;
        g_stCpssTcpMan.ulServerTcpNum-- ;
    }

    /*查询tcp的client表*/
    pstTcpDrv = cpss_com_tcp_client_link_find_by_socket(ulSocketId) ;
    if(pstTcpDrv != NULL)
    {
        pucFreeBuf = pstTcpDrv->stRecvBufM.pucDataBuf ;
        cpss_mem_memset(pstTcpDrv,0,sizeof(CPSS_COM_DRV_TCP_T)) ;
        g_stCpssTcpMan.ulClientTcpNum-- ;
    }
    
    /*释放TCP的发送内存*/
    if(pucFreeBuf != NULL)
    {
        cpss_mem_free(pucFreeBuf) ;
    }
    
#ifdef CPSS_FUNBRD_MC
    lRet = cpss_com_tcp_del_socket_req_send(ulSocketId) ;
#else
    lRet = closesocket(ulSocketId) ;
#endif

    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_server_link_add
* 功    能: 将接入的TCP连接记录到CPSS的TCP表中
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_server_link_add
( 
UINT32 ulSocketId,
UINT32 ulIpAddr,
UINT16 usPort,
UINT32 ulServerFlag
)
{
    INT32 lRet ;
    CPSS_COM_TCP_CONN_IND_MSG_T stTcpMsg = {0} ;
    CPSS_COM_DRV_TCP_T* pstTcpDrv = NULL ;

    /*是否已经增加过此TCP链路*/
    pstTcpDrv = cpss_com_tcp_server_link_find_by_socket(ulSocketId) ;
    if(pstTcpDrv != NULL)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_server_link_add already have the socket=%d,ulIpAddr=0x%x,usport=0x%x\n",
            ulSocketId,ulIpAddr,usPort) ;
            return CPSS_OK ;
    }

    /*查找空闲的TCP链路*/
    pstTcpDrv = cpss_com_tcp_server_free_link_find(ulServerFlag) ;
    if(NULL == pstTcpDrv)
    {
        /*释放tcp链路*/
        cpss_com_tcp_link_free(ulSocketId) ;

        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_server_link_add have no space for add socket=%d,ulIpAddr=0x%x,usport=0x%x\n",
            ulSocketId,ulIpAddr,usPort) ;
        
        return CPSS_ERROR ;
    }

    /*增加链路信息*/
    pstTcpDrv->ulSClient = ulSocketId ;
    pstTcpDrv->stTcpLink.ulIP = ulIpAddr ;
    pstTcpDrv->stTcpLink.usTcpPort = usPort ;        
    pstTcpDrv->ulServerFlag = ulServerFlag ;
        
    /*向OAMS发送链路接入指示消息*/        
    stTcpMsg.ulIP = ulIpAddr ;
    stTcpMsg.usTcpPort = usPort ;
    stTcpMsg.usReserved = 0 ;
        
    /*发送和客户端的连接*/
    lRet = cpss_com_tcp_cps__oams_connect_info_send(
        (UINT8*)&stTcpMsg,CPSS_COM_TCP_CONN_IND_MSG,ulServerFlag);
    if(lRet == CPSS_ERROR)
    {
        /*释放tcp链路*/
        cpss_com_tcp_link_free(ulSocketId) ;

        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_server_link_add send ind to oams failed. socket=%d,ulIpAddr=0x%x,usport=0x%x\n",
            ulSocketId,ulIpAddr,usPort) ;
    }
    
    /*申请链路的接收内存*/
    if(lRet == CPSS_OK)
    {
        if(pstTcpDrv->stRecvBufM.pucDataBuf == NULL)
        {
            pstTcpDrv->stRecvBufM.pucDataBuf = cpss_mem_malloc(CPSS_COM_TCP_LINK_BUFLEN_MAX) ;
            if(pstTcpDrv->stRecvBufM.pucDataBuf == NULL)
            {
                /*释放tcp链路*/
                cpss_com_tcp_link_free(ulSocketId) ;
                lRet = CPSS_ERROR ;
                
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                    "cpss_com_tcp_server_link_add malloc failed. socket=%d,ulIpAddr=0x%x,usport=0x%x\n",
                    ulSocketId,ulIpAddr,usPort) ;
            }
        }
    }
    
    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_client_link_add
* 功    能: 将接入的TCP连接记录到CPSS的TCP表中
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_client_link_add
( 
 UINT32 ulSocketId,
 UINT32 ulIpAddr,
 UINT16 usPort
 )
{
    CPSS_COM_DRV_TCP_T* pstTcpDrv = NULL ;

    /*是否已经增加过此TCP链路*/
    pstTcpDrv = cpss_com_tcp_client_link_find_by_socket(ulSocketId) ;
    if(pstTcpDrv != NULL)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_client_link_add already have the socket=%d\n",ulSocketId) ;
        
        return CPSS_OK ;
    }

    /*查找空闲的TCP链路*/
    pstTcpDrv = cpss_com_tcp_client_free_link_find() ;
    if(NULL == pstTcpDrv)
    {
        /*释放tcp链路*/
        cpss_com_tcp_link_free(ulSocketId) ;

        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_client_link_add have no space for add socket=%d\n",ulSocketId) ;
        
        return CPSS_ERROR ;
    }

    pstTcpDrv->ulSClient = ulSocketId ;
    pstTcpDrv->stTcpLink.ulIP = ulIpAddr ;
    pstTcpDrv->stTcpLink.usTcpPort = usPort ;
    pstTcpDrv->ulServerFlag = CPSS_COM_TCP_CLIENT_FLAG ;
        
    /*申请链路的接收内存*/
    if(pstTcpDrv->stRecvBufM.pucDataBuf == NULL)
    {
        pstTcpDrv->stRecvBufM.pucDataBuf = cpss_mem_malloc(CPSS_COM_TCP_LINK_BUFLEN_MAX) ;
        if(pstTcpDrv->stRecvBufM.pucDataBuf == NULL)
        {
            /*释放tcp链路*/
            cpss_com_tcp_link_free(ulSocketId) ;
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "cpss_com_tcp_client_link_add malloc failed. socket=%d,ulIpAddr=0x%x,usport=0x%x\n",
                ulSocketId,ulIpAddr,usPort) ;
            return CPSS_ERROR ;
        }
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_link_add
* 功    能: 将接入的TCP连接记录到CPSS的TCP表中
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: LDT接入的链路
******************************************************************************/
INT32 cpss_com_tcp_link_add
(
 UINT32 ulSocketId,
 UINT32 ulIpAddr,
 UINT16 usPort,
 UINT32 ulServerFlag
)
{
    INT32 lRet ;

    /*根据不同的链路类型套用不同的处理函数*/
    if((ulServerFlag == CPSS_COM_TCP_SERVER_FLAG)||(ulServerFlag == CPSS_COM_TCP_DC_FLAG))
    {
        lRet = cpss_com_tcp_server_link_add(ulSocketId,ulIpAddr,usPort,ulServerFlag) ;
    }
    else if(ulServerFlag == CPSS_COM_TCP_CLIENT_FLAG)
    {
        lRet = cpss_com_tcp_client_link_add(ulSocketId,ulIpAddr,usPort) ;
    }
    else
    {
        return CPSS_ERROR ;
    }

    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_link_add failed.socketid=%d,ipaddr=0x%x,portid=%d,serverflag=%d\n",
            ulSocketId,ulIpAddr,usPort,ulServerFlag) ;        
    }
    
    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_route_add()                            
* 功    能: 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明:
******************************************************************************/
INT32 cpss_route_add
(
 UINT8* pucDstIP,
 UINT8* pucGateIP
)
{
    INT32 lRet = CPSS_OK ;

#ifdef CPSS_FUNBRD_MC
    lRet = CPSS_OK ;
#else
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
    lRet = routeAdd(pucDstIP,pucGateIP) ;
#endif
#endif

    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_route_delete()                            
* 功    能: 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明:
******************************************************************************/
INT32 cpss_route_delete
( 
UINT8* pucDstIP,
UINT8* pucGateIP
)
{
    INT32 lRet = CPSS_OK ;
    
#ifdef CPSS_FUNBRD_MC
    lRet = CPSS_OK ;
#else
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
    lRet = routeDelete(pucDstIP,pucGateIP) ;
#endif
#endif
    
    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_ipstr_2_ipint                            
* 功    能: 将字符串形式的IP转化为本机字节序的整型
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明:
******************************************************************************/
INT32 cpss_com_tcp_ipstr_2_ipint
(
 UINT8 *pucIpAddr,
 UINT32 *pulIpAddr
)
{
    /*参数检查*/
    if((pucIpAddr == NULL)||(pulIpAddr == NULL))
    {
        return CPSS_ERROR ;
    }

    *pulIpAddr = 0 ;

    *pulIpAddr = (*pulIpAddr | ((*pucIpAddr<<24) & 0xff000000)) ;
    *pulIpAddr = (*pulIpAddr | ((*(pucIpAddr+1)<<16) & 0x00ff0000)) ;
    *pulIpAddr = (*pulIpAddr | ((*(pucIpAddr+2)<<8) & 0x0000ff00)) ;
    *pulIpAddr = (*pulIpAddr | (*(pucIpAddr+3) & 0x000000ff)) ;

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_ipstr_2_ipint                            
* 功    能: 将本机字节序的IP转化为字符串形式的整型
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明:
******************************************************************************/
INT32 cpss_com_tcp_ipint_2_ipstr
(
 UINT8 *pucIpAddr,
 UINT32 ulIpAddr
)
{
    /*参数校验*/
    if(pucIpAddr == NULL)
    {
        return CPSS_ERROR ;
    }

    /*转换IP地址形式*/
    *pucIpAddr     = (UINT8)((ulIpAddr & 0xff000000)>>24) ;
    *(pucIpAddr+1) = (UINT8)((ulIpAddr & 0x00ff0000)>>16) ;
    *(pucIpAddr+2) = (UINT8)((ulIpAddr & 0x0000ff00)>>8)  ;
    *(pucIpAddr+3) = (UINT8)(ulIpAddr & 0x000000ff) ;
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_cps__rdbs_local_ip_fill                            
* 功    能: 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明:
******************************************************************************/
INT32 cpss_com_tcp_cps__rdbs_local_ip_fill
(
 CPS__RDBS_IPADDR_T* pstRdbsIp
)
{
    UINT32 ulIpAddr ;
    UINT8  aucMacAddr[6] ;
    
    cpss_local_brd_ip_get(DRV_ETHER_CTRL_PORT_IPV4,&ulIpAddr,aucMacAddr) ;
    ulIpAddr = cpss_ntohl(ulIpAddr) ;

    pstRdbsIp->ucType     = 0 ;
    pstRdbsIp->ucValid    = 1 ;
    pstRdbsIp->ucIpPrefix = 0 ;
    pstRdbsIp->ucRvs      = 0 ;
    pstRdbsIp->aucIp[0]   = (UINT8)(ulIpAddr&0xff000000)>>24 ;
    pstRdbsIp->aucIp[1]   = (UINT8)(ulIpAddr&0x00ff0000)>>16 ;
    pstRdbsIp->aucIp[2]   = (UINT8)(ulIpAddr&0x0000ff00)>>8  ;
    pstRdbsIp->aucIp[3]   = (UINT8)(ulIpAddr&0x000000ff)     ;

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_connect_req_send                            
* 功    能: 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_connect_req_send
(
 CPSS_COM_TCP_LINK_T* pstTcpLink,
 UINT32 ulTcpIndex
)
{
    CPSS_COM_PID_T stTnbsPid ;
    CPS__TNBS_SOCKET_SETUP_REQ_MSG_T stConnectReq;
    CPSS_COM_LOGIC_ADDR_T stLocalAddr ;
    CPS__TNBS_IPADDR_ARRAY_T stIpAddr = {0};
    UINT32 ulAddrFlag ;
    UINT32 ulIpAddr = 0 ;
    INT32  lRet ;

    /*得到本地逻辑地址*/
    cpss_com_logic_addr_get(&stTnbsPid.stLogicAddr,&stTnbsPid.ulAddrFlag) ;
    stTnbsPid.ulPd = CPS__TNBS_IPSTACK_PROC ;

    /*填写请求消息*/
    stConnectReq.ulUsrId = cpss_htonl(ulTcpIndex) ;
    stConnectReq.ucSocketType = CPS__TNBS_SOCK_STREAM ;
    stConnectReq.ucProtoType  = CPS__TNBS_IPPROTO_TCP ;
    stConnectReq.usRsv = cpss_htons(CPS__TNBS_IPSTACK_SOCKET_REUSE_ENABLE) ;
      
    /*填写目的端的ip,port信息*/
    stConnectReq.ucDestIpNum  = 1 ;
    stConnectReq.astDestIpAddr[0].ucType     = 0 ;
    stConnectReq.astDestIpAddr[0].ucValid    = 1 ;
    stConnectReq.astDestIpAddr[0].ucIpPrefix = 0 ;
    cpss_com_tcp_ipint_2_ipstr(stConnectReq.astDestIpAddr[0].aucIp,pstTcpLink->ulIP) ;
    stConnectReq.usPeerPort = cpss_htons(pstTcpLink->usTcpPort) ;

    /*填写源端的ip,port信息*/
    stConnectReq.ucSrcIpNum = 1 ;
    stConnectReq.astSrcIpAddr[0].ucType     = 0 ;
    stConnectReq.astSrcIpAddr[0].ucValid    = 1 ;
    stConnectReq.astSrcIpAddr[0].ucIpPrefix = 0 ;
    stConnectReq.usSockTimeOut  =  0;
    cpss_mem_memset(stConnectReq.astSrcIpAddr[0].aucIp,0,4) ;
    
    cpss_com_logic_addr_get(&stLocalAddr,&ulAddrFlag) ;
    
#if 0
#ifdef CPSS_FUNBRD_MC
#if 1    
    lRet = cps__tnbs_ipstack_find_outerport_ipaddr(&stLocalAddr,&stIpAddr);
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }
#else
    stIpAddr.astIpAddr[0].aucIp[0] = 10 ;
    stIpAddr.astIpAddr[0].aucIp[1] = 0 ;
    stIpAddr.astIpAddr[0].aucIp[2] = 0 ;
    stIpAddr.astIpAddr[0].aucIp[3] = 100 ;
#endif
#endif
    cpss_mem_memcpy(stConnectReq.astSrcIpAddr[0].aucIp,stIpAddr.astIpAddr[0].aucIp,4) ;
#endif  
  
    if(ulTcpIndex == 0)
    {
        stConnectReq.usLocalPort = cpss_htons(CPSS_COM_TCP_CONNECT_PORT + 10) ;
    }
    else if(ulTcpIndex == 1)
    {
        stConnectReq.usLocalPort = cpss_htons(CPSS_COM_TCP_CONNECT_PORT + 11) ;
    }

    /*发送请求消息到TNBS*/
    lRet = cpss_com_send(&stTnbsPid,CPS__TNBS_SOCKET_SETUP_REQ_MSG,
        (UINT8*)&stConnectReq,sizeof(CPS__TNBS_SOCKET_SETUP_REQ_MSG_T)) ;

    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_connect_req_send failed!srcip=0x%x,dstip=0x%x\n",
            ulIpAddr,pstTcpLink->ulIP) ;
    }
    else
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
            "cpss_com_tcp_connect_req_send succ dstip=0x%x,dstport=0x%x\n",
            pstTcpLink->ulIP,pstTcpLink->usTcpPort) ;        
    }

    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_listen_req_send                            
* 功    能: 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_del_socket_req_send
(
 UINT32 ulSocketId
)
{
#ifndef CPSS_CPS__TNBS_TCP_SIM
    INT32 lRet ;
    CPS__TNBS_SOCKET_DEL_REQ_MSG_T stDelSocketReq ;
    CPSS_COM_MSG_HEAD_T stDispMsg ;

    /*填写消息体*/
    stDelSocketReq.ulSocketId = cpss_htonl(ulSocketId) ;
    stDelSocketReq.ulDelType = cpss_htonl(CPS__TNBS_SOCK_ABORT) ;

    /*填写地址信息*/
    cpss_com_logic_addr_get(&stDispMsg.stDstProc.stLogicAddr,&stDispMsg.stDstProc.ulAddrFlag) ;
    stDispMsg.stDstProc.ulPd = CPS__TNBS_IPSTACK_PROC ;

    /*填写源端地址*/
    stDispMsg.stSrcProc.stLogicAddr = stDispMsg.stDstProc.stLogicAddr ;
    stDispMsg.stSrcProc.ulAddrFlag  = stDispMsg.stDstProc.ulAddrFlag ;
    stDispMsg.stSrcProc.ulPd        = CPSS_COM_TCP_PROC ;

    /*填写其它信息*/
    stDispMsg.ucAckFlag = CPSS_COM_NOT_ACK ;
    stDispMsg.ucPriFlag = CPSS_COM_PRIORITY_NORMAL ;
    stDispMsg.ucShareFlag = 0 ;
    stDispMsg.ulLen = sizeof(CPS__TNBS_SOCKET_DEL_REQ_MSG_T) ;
    stDispMsg.ulMsgId = CPS__TNBS_SOCKET_DEL_REQ_MSG ;
    stDispMsg.pucBuf  = (UINT8*)&stDelSocketReq ;

    lRet = cpss_com_send_extend(&stDispMsg) ;
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_del_socket_req_send failed.socketid=%d\n",ulSocketId) ;
    }
    else
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
            "cpss_com_tcp_del_socket_req_send succ.socketid=%d\n",ulSocketId) ;        
    }

    return lRet ;

#else

    closesocket(ulSocketId) ;
    return CPSS_OK ;

#endif
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_listen_req_send                            
* 功    能: 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_listen_req_send()
{
    INT32 lRet = CPSS_OK ;
    CPS__TNBS_SOCKET_LISTEN_REQ_MSG_T  stListenReq = {0} ;
    CPSS_COM_PID_T stSelfPid = {0};
    CPS__TNBS_IPADDR_ARRAY_T stIpAddr = {0} ;    
    
#ifdef CPSS_FUNBRD_MC
    
    /*填充listen请求消息*/
    stListenReq.ucSocketType = CPS__TNBS_SOCK_STREAM ;
    stListenReq.ucSrcIpNum = 1 ;
    
    /*设置listen功能为listen所有的接入*/
    stListenReq.astSrcIpAddr[0].ucType  = 0 ;
    stListenReq.astSrcIpAddr[0].ucValid = 1 ;
    stListenReq.astSrcIpAddr[0].ucIpPrefix = 0 ;
        
    /*得到本板的地址信息*/
    cpss_com_logic_addr_get(&stSelfPid.stLogicAddr,&stSelfPid.ulAddrFlag) ;
    stSelfPid.ulPd = CPS__TNBS_IPSTACK_PROC ;

    cpss_mem_memset(stListenReq.astSrcIpAddr[0].aucIp,0,4) ;

    if(g_ulCpssTcpListenFlag == CPSS_COM_TCP_LISTEN_STAT_INIT)
    {
        stListenReq.ulUsrId = cpss_htonl(CPSS_COM_TCP_SERVER_USRID) ;        
        stListenReq.usLocalPort = cpss_htons(CPSS_COM_TCP_SERVER_PORT) ; 
        
        /*向TNBS发送listen*/
        lRet = cpss_com_send(&stSelfPid,CPS__TNBS_SOCKET_LISTEN_REQ_MSG,
            (UINT8*)&stListenReq,sizeof(CPS__TNBS_SOCKET_LISTEN_REQ_MSG_T)) ;  
        
        g_stTcpListenStat.ulListenReqNum++ ;  
        
        cpss_output(CPSS_MODULE_COM, CPSS_PRINT_INFO,
            "cpss_com_tcp_listen_req_send tcp connect lRet=%d\n",lRet);        
    }
    
    if(g_ulCpssDcListenFlag == CPSS_COM_DC_LISTEN_STAT_INIT)
    {
        stListenReq.ulUsrId = cpss_htonl(CPSS_COM_DC_SERVER_USRID) ;
        stListenReq.usLocalPort = cpss_htons(CPSS_COM_DC_SERVER_PORT) ;
    
        /*向TNBS发送listen*/
        lRet = cpss_com_send(&stSelfPid,CPS__TNBS_SOCKET_LISTEN_REQ_MSG,
        (UINT8*)&stListenReq,sizeof(CPS__TNBS_SOCKET_LISTEN_REQ_MSG_T)) ; 

        g_stTcpListenStat.ulDcListenReqNum++ ; 
    
        cpss_output(CPSS_MODULE_COM, CPSS_PRINT_INFO,
            "cpss_com_tcp_listen_req_send dc connect lRet=%d\n",lRet);                
    }
    
#endif

    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_proc_init                            
* 功    能: 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_proc_init
(
SMSS_PROC_ACTIVATE_REQ_MSG_T* pstActReq
)
{
    INT32 lRet ;

    g_stTcpListenStat.ulTcpProcInit = 1 ;

    /*启动listen定时器*/
    lRet = cpss_timer_loop_set(CPSS_COM_TCP_LISTEN_TM_NO,CPSS_COM_TCP_LISTEN_TM_INTERVAL) ;    
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_timer_loop_set CPSS_COM_TCP_LISTEN_TM_NO failed.\n") ;
    }

    cpss_com_tcp_listen_req_send() ;

    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_listen_timeout_deal                            
* 功    能: 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: tcp纤程初始化函数
******************************************************************************/
INT32 cpss_com_tcp_listen_timeout_deal()
{
    INT32 lRet ;
    
    /*发送listen消息给TNBS的IP协议栈纤程*/
    lRet = cpss_com_tcp_listen_req_send() ;
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_listen_req_send failed.\n");        
    }
    
    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_listen_rsp_deal                            
* 功    能:接收到tnbs的tcp listen请求的响应消息的处理
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*          成功：CPSS_OK 
*          失败：CPSS_ERROR
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_listen_rsp_deal
(
 CPS__TNBS_SOCKET_LISTEN_RSP_MSG_T* pstListenRsp
)
{
    UINT32 ulUsrId = cpss_ntohl(pstListenRsp->ulUsrId) ;

    if(ulUsrId == CPSS_COM_TCP_SERVER_USRID)
    {
        g_stTcpListenStat.ulListenResult = cpss_ntohl(pstListenRsp->ulResult) ;
    }
    else
    {
        g_stTcpListenStat.ulDcListenResult = cpss_ntohl(pstListenRsp->ulResult) ;
    }    

    /*如果响应结果失败则不停止listen定时器*/
    if(cpss_ntohl(pstListenRsp->ulResult) != CPS__TNBS_OK)
    {
        g_stCpssTcpErrStat.ulListenFailNum++ ;
        
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_listen_rsp_deal failed result=%d,socketid=%d\n",
            cpss_ntohl(pstListenRsp->ulResult),cpss_ntohl(pstListenRsp->ulSocketId));        
        return CPSS_ERROR ;
    }
    else
    {
        /*设置listen过程的状态*/
        if(ulUsrId == CPSS_COM_TCP_SERVER_USRID)
        {
            g_ulCpssTcpListenFlag = CPSS_COM_TCP_LISTEN_STAT_NORMAL ;
        }
        else if(ulUsrId == CPSS_COM_DC_SERVER_USRID)
        {
            g_ulCpssDcListenFlag = CPSS_COM_DC_LISTEN_STAT_NORMAL ;
        }
        else
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "listen response usrid is unexpected usrid=%d!\n",ulUsrId) ;
        }

        if((g_ulCpssTcpListenFlag == CPSS_COM_TCP_LISTEN_STAT_NORMAL)
            &&(g_ulCpssDcListenFlag == CPSS_COM_DC_LISTEN_STAT_NORMAL))
        {
            /*停止listen消息定时器*/
            cpss_timer_delete(CPSS_COM_TCP_LISTEN_TM_NO) ;
        }

        /*存储sockID在TCP管理结构中*/
        g_stCpssTcpMan.ulListenSocket = cpss_ntohl(pstListenRsp->ulSocketId) ;
    }

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "cpss_com_tcp_listen_rsp_deal success.socketid=%d\n",cpss_ntohl(pstListenRsp->ulSocketId));        
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_connect_rsp_deal
* 功    能:收到tcp连接响应的处理过程
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*          成功：CPSS_OK 
*          失败：CPSS_ERROR
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_connect_rsp_deal
(
 CPS__TNBS_SOCKET_SETUP_RSP_MSG_T* pstConnectRsp
)
{
    INT32 lRet ;
    CPSS_COM_TCP_LINK_T   stTcpLink ;
    CPSS_COM_LOGIC_ADDR_T stSimLogicAddr ;
    CPSS_COM_TCP_CONNECT_RSP_MSG_T stConnectRsp ;
    UINT32 ulTcpIndex ;

    /*参数校验*/
    if(NULL == pstConnectRsp)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_connect_rsp_deal para null.\n");
        return CPSS_ERROR ;
    }

    /*tnbs建链失败*/
    if(cpss_ntohl(pstConnectRsp->ulResult) != CPS__TNBS_OK)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_connect_rsp_deal result = 0x%x\n",cpss_ntohl(pstConnectRsp->ulResult));
        /*  return CPSS_ERROR ; */
    }
    
    /*得到TCP链路的序号*/
    ulTcpIndex = cpss_ntohl(pstConnectRsp->ulUsrId) ;
    
    stConnectRsp.stLinkInfo.ulIP = g_stCpssTcpMan.stTcpConnectInfo[ulTcpIndex].ulIP ;
    stConnectRsp.stLinkInfo.usTcpPort = g_stCpssTcpMan.stTcpConnectInfo[ulTcpIndex].usTcpPort ;
    stConnectRsp.ulResult = cpss_ntohl(pstConnectRsp->ulResult);

    /*停止connect连接定时器*/
    if(ulTcpIndex == 0)
    {
      /*  cpss_timer_delete(CPSS_COM_TCP_CONNECT_TM0_NO) ;  */
    }
    else if(ulTcpIndex == 1)
    {
      /*  cpss_timer_delete(CPSS_COM_TCP_CONNECT_TM1_NO) ;  */
    }
    
    if(stConnectRsp.ulResult == CPS__TNBS_OK)
   {
    /*记录tcp连接链路信息*/
    lRet = cpss_com_tcp_link_add(cpss_ntohl(pstConnectRsp->ulSocketId),stConnectRsp.stLinkInfo.ulIP,
        stConnectRsp.stLinkInfo.usTcpPort,CPSS_COM_TCP_CLIENT_FLAG) ;
    if(lRet != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_connect_rsp_deal add link failed. ip=0x%x,port=0x%x\n",
            stConnectRsp.stLinkInfo.ulIP,stConnectRsp.stLinkInfo.usTcpPort);
        return CPSS_ERROR ;
    }

    /*模拟OAMS对链路进行绑定,便于数据接收和发送*/
    stTcpLink.ulIP      = stConnectRsp.stLinkInfo.ulIP ;
    stTcpLink.usTcpPort = stConnectRsp.stLinkInfo.usTcpPort ;

    stSimLogicAddr.ucModule = 251 ;
    stSimLogicAddr.ucSubGroup = 1 ;
    stSimLogicAddr.usGroup = 1 ;

    cpss_com_tcp_link_sim_bind(&stTcpLink,
        &stSimLogicAddr,CPSS_COM_TCP_CONNECT_ROUTE_ALLOW) ;
    }
    /*清空TCP链路信息*/
#if 0
    cpss_mem_memset(&g_stCpssTcpMan.stTcpConnectInfo[ulTcpIndex],
        0, sizeof(CPSS_COM_TCP_LINK_T)) ;
#endif

    /*向OAMS发送连接响应消息*/
    lRet = cpss_com_send(&g_stCpssTcpMan.stOamsPid,CPSS_COM_TCP_CONNECT_RSP_MSG,
        (UINT8*)&stConnectRsp,sizeof(CPSS_COM_TCP_CONNECT_RSP_MSG_T)) ;

    /*清空地址信息内存*/

  /*  cpss_mem_memset(&g_stCpssTcpMan.stOamsPid,0,sizeof(CPSS_COM_PID_T)) ; */

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "cpss_com_tcp_connect_rsp_deal succ. ip=0x%x,port=0x%x ulResult = %d \n",
        stConnectRsp.stLinkInfo.ulIP,stConnectRsp.stLinkInfo.usTcpPort,stConnectRsp.ulResult);

    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_data_send
* 功    能: 发送TCP数据到tnbs纤程
* 函数类型:    
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:
*          成功：CPSS_OK 
*          失败：CPSS_ERROR
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_data_send
(
 UINT32 ulSocketId, 
 UINT8* pucSendBuf,
 UINT32 ulSendLen
)
{
    INT32 lSendLen = 0 ;
    CPSS_COM_PID_T stTnbsPid = {0} ;
    
    /*参数判断*/
    if(pucSendBuf == NULL)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_data_send para null\n");
        return CPSS_ERROR ;
    }

    /*参数判断*/
    if(ulSendLen > CPS__TNBS_MAX_DATA_LEN)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_data_send data len overflow.sendlen=%d\n",ulSendLen);
        return CPSS_ERROR ;
    }
#ifdef CPSS_CPS__TNBS_TCP_SIM
    lSendLen=send(ulSocketId,pucSendBuf,ulSendLen,0) ;
   if(lSendLen != (INT32)(ulSendLen))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "tcp drv send failed.lSendBuf=0x%x,errno=%d\n",lSendLen,cpss_com_sock_error_get());

        /*tcp断开统计*/
        g_stTcpCloseStat.ulTcpSendFullCloseNum = g_stTcpCloseStat.ulTcpSendFullCloseNum + 1 ;
        
        return CPSS_ERROR ;
    }
#else
#ifdef CPSS_FUNBRD_MC
    {
        CPS__TNBS_SOCKET_DATASEND_REQ_MSG_T stSendDataReq ;
        CPSS_COM_MSG_HEAD_T stDispMsg ;
        
        /*填充地址信息*/
        cpss_com_logic_addr_get(&stTnbsPid.stLogicAddr,&stTnbsPid.ulAddrFlag) ;
        stTnbsPid.ulPd = CPS__TNBS_IPSTACK_PROC ;
        
        /*填充发送消息体*/
        stSendDataReq.ulSocketId = cpss_htonl(ulSocketId) ;
        stSendDataReq.usLen = cpss_htons(ulSendLen) ;
        cpss_mem_memcpy(stSendDataReq.aucData,pucSendBuf,ulSendLen) ;
        stSendDataReq.ulLifeTime = cpss_htonl(0) ;
        stSendDataReq.ulContext = cpss_htonl(0) ;
        
        /*填充发送数据头信息*/
        stDispMsg.pucBuf    = (UINT8*)&stSendDataReq ;
        stDispMsg.stDstProc = stTnbsPid ;
        stDispMsg.ucAckFlag = CPSS_COM_NOT_ACK ;
        stDispMsg.ucPriFlag = CPSS_COM_PRIORITY_NORMAL ;
        stDispMsg.ucShareFlag = CPSS_COM_MEM_NOT_SHARE ;
        stDispMsg.ulLen       = sizeof(CPS__TNBS_SOCKET_DATASEND_REQ_MSG_T) ;
        stDispMsg.ulMsgId     = CPS__TNBS_SOCKET_DATASEND_REQ_MSG ;
        stDispMsg.stSrcProc.stLogicAddr = stTnbsPid.stLogicAddr ;
        stDispMsg.stSrcProc.ulAddrFlag  = stTnbsPid.ulAddrFlag ;
        stDispMsg.stSrcProc.ulPd        = CPSS_COM_TCP_PROC ;

        /*发送数据到TNBS的UDP处理纤程*/
        lSendLen = cpss_com_send_extend(&stDispMsg) ;
        if(CPSS_ERROR == lSendLen)
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "cpss_com_tcp_data_send failed socketid=0x%x msgLen=%d\n",ulSocketId,ulSendLen);        
            lSendLen = 0 ;
        }
        else
        {
            lSendLen = ulSendLen ;
        }
    }
#else
    lSendLen=send(ulSocketId,pucSendBuf,ulSendLen,0) ;
    if(lSendLen != (INT32)(ulSendLen))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "tcp drv send failed.lSendBuf=0x%x,errno=%d\n",lSendLen,cpss_com_sock_error_get());

        /*tcp断开统计*/
        g_stTcpCloseStat.ulTcpSendFullCloseNum = g_stTcpCloseStat.ulTcpSendFullCloseNum + 1 ;
        
        return CPSS_ERROR ;
    }
#endif
#endif

    if(lSendLen>0)
    {
        return CPSS_OK ;
    }
    else
    {
        return CPSS_ERROR ;
    }
}


/*******************************************************************************
* 函数名称: cpss_com_dc_pkg_building
* 功    能: 网元直连数据组包 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*          成功：CPSS_OK 
*          失败：CPSS_ERROR
* 说   明: 
******************************************************************************/
INT32 cpss_com_dc_pkg_building
( 
 UINT8  **ppucRecvData,
 UINT32 *pulRecvLen, 
 CPSS_TCP_RECV_BUF_M_T *pstRecvBufM,
 CPSS_COM_TRUST_HEAD_T **ppstTrustHdr
)
{
    UINT32 ulDcRecvOnce = 0 ;
    UINT32 ulEnterKeyNum = 0 ;
    INT32  lRet = 0 ;
    CPSS_COM_MSG_HEAD_T* pstDispHdr ;
    CPSS_COM_TRUST_HEAD_T* pstTrustHdr ;

    if(*pulRecvLen == 0)
    {
        /*对包组织结构进行清零*/
        if(pstRecvBufM->ulDataFlag == CPSS_TCP_PKG_FINISH)
        {
            return CPSS_TCP_RECV_END ;
        }
        else
        {
            /*对接网元直连数据进行检测*/
            lRet = cpss_com_dc_recv_data_check(pstRecvBufM->pucDataBuf,
                &ulDcRecvOnce,&ulEnterKeyNum,pstRecvBufM->ulRecvBufLen) ;
        }
        }
    else
    {
        if(pstRecvBufM->ulRecvBufLen == 0)
        {
    /*对接收到的网元直连数据进行检测*/
            lRet = cpss_com_dc_recv_data_check(*ppucRecvData,&ulDcRecvOnce,&ulEnterKeyNum,*pulRecvLen) ;                
            
            if(ulDcRecvOnce == 0)
            {
                if(ulEnterKeyNum == *pulRecvLen)
                {
                    pstRecvBufM->ulDataFlag = CPSS_TCP_PKG_FINISH ;
                    return CPSS_TCP_RECV_END;
                }
            }
            
            if(ulDcRecvOnce == 0)
            {
                cpss_mem_memcpy(pstRecvBufM->pucDataBuf,*ppucRecvData+ulEnterKeyNum,(*pulRecvLen - ulEnterKeyNum)) ;
                    pstRecvBufM->ulRecvBufLen = *pulRecvLen - ulEnterKeyNum ;    
                      
                /*对接网元直连数据进行检测*/
                lRet = cpss_com_dc_recv_data_check(pstRecvBufM->pucDataBuf,
                    &ulDcRecvOnce,&ulEnterKeyNum,pstRecvBufM->ulRecvBufLen) ;              
            }
            else
            {
                cpss_mem_memcpy(pstRecvBufM->pucDataBuf,*ppucRecvData,(*pulRecvLen)) ;
                    pstRecvBufM->ulRecvBufLen = *pulRecvLen ;
    
            /*对接网元直连数据进行检测*/
            lRet = cpss_com_dc_recv_data_check(pstRecvBufM->pucDataBuf,
                &ulDcRecvOnce,&ulEnterKeyNum,pstRecvBufM->ulRecvBufLen) ;
        }
    }
    else
    {
	    cpss_mem_memcpy(pstRecvBufM->pucDataBuf+pstRecvBufM->ulRecvBufLen,*ppucRecvData,(*pulRecvLen)) ;
                    pstRecvBufM->ulRecvBufLen = pstRecvBufM->ulRecvBufLen + *pulRecvLen ;
        lRet = cpss_com_dc_recv_data_check(pstRecvBufM->pucDataBuf,
            &ulDcRecvOnce,&ulEnterKeyNum,pstRecvBufM->ulRecvBufLen) ;
    }
    }
    
    /*如果不是完整的命令行直接返回*/
    if(lRet == CPSS_DC_ENTER_KEY_UNTOUCH)
    {
        return CPSS_TCP_RECV_END ;
    }
    
    /*组织网元直连数据进行发送*/
    *ppstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)cpss_com_mem_alloc(ulDcRecvOnce + 1) ;
    if(*ppstTrustHdr == NULL)
    {
        pstRecvBufM->ulDataFlag = CPSS_TCP_PKG_FINISH ;
        
        return CPSS_TCP_RECV_END ;
    }
    else
    {
        pstTrustHdr = *ppstTrustHdr ;
        pstTrustHdr->ucPType = COM_SLID_PTYPE_INCREDIBLE ;        
        
        pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(*ppstTrustHdr) ;
        pstDispHdr->pucBuf = (UINT8*)CPSS_DISP_HDR_TO_USR_HDR(pstDispHdr) ;
        
        cpss_mem_memcpy(pstDispHdr->pucBuf,pstRecvBufM->pucDataBuf,ulDcRecvOnce) ;

        /*设置通信分发头*/
        pstDispHdr->ucAckFlag = CPSS_COM_NOT_ACK ;
        pstDispHdr->ucPriFlag = CPSS_COM_PRIORITY_NORMAL ;
        pstDispHdr->ucShareFlag = CPSS_COM_MEM_SHARE ;
        pstDispHdr->ulLen       = ulDcRecvOnce ;
        pstDispHdr->ulMsgId     = CPSS_COM_TCP_MML_USER_DATA_MSG ;
        
        /*填充目的地址和纤程号*/
        cpss_com_logic_addr_get(
            &pstDispHdr->stDstProc.stLogicAddr,&pstDispHdr->stDstProc.ulAddrFlag) ;
        pstDispHdr->stDstProc.ulPd = CPS__OAMS_PD_M_DLM_MSGDSP_PROC ;
        
        /*在上交的数据末尾增加行结束符*/
        pstDispHdr->ulLen  = ulDcRecvOnce + 1 ;
        *(pstDispHdr->pucBuf + ulDcRecvOnce) = '\0' ;

    }
    
    /*判断是否还有数据*/
    if(pstRecvBufM->ulRecvBufLen > (ulDcRecvOnce + ulEnterKeyNum))
    {
        /*重新拷贝数据*/
        cpss_mem_memmove(pstRecvBufM->pucDataBuf,
            (pstRecvBufM->pucDataBuf+ulDcRecvOnce+ulEnterKeyNum),
            (pstRecvBufM->ulRecvBufLen-ulDcRecvOnce-ulEnterKeyNum)) ;
        pstRecvBufM->ulRecvBufLen = pstRecvBufM->ulRecvBufLen-ulDcRecvOnce-ulEnterKeyNum ;
    }
    else
    {
        pstRecvBufM->ulDataFlag   = CPSS_TCP_PKG_FINISH ;
        pstRecvBufM->ulRecvBufLen = 0 ;
    }

    return CPSS_TCP_RECV_SEND_OK ;
}



/*******************************************************************************
* 函数名称: cpss_com_tcp_pkg_building
* 功    能: TCP组包函数 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*          成功：CPSS_OK 
*          失败：CPSS_ERROR
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_pkg_building
(
 UINT8  **ppucRecvData,
 UINT32 *pulRecvLen, 
 CPSS_TCP_RECV_BUF_M_T *pstRecvBufM,
 UINT32 *pulSendLen,
 UINT8  **ppucSendBuf
)
{
    UINT8* pucKeepRecvBuf ;
    UINT32 ulKeepDataLen ;
    UINT32 ulNeedDataLen ;
    UINT32 ulRecvDataLen ;
    CPSS_COM_MSG_HEAD_T* pstDispHdr ;

    /* 入参检查 */
    if((*ppucRecvData == NULL)||(pstRecvBufM == NULL)||(ppucSendBuf == NULL)||
        (pulRecvLen == NULL)||(pulSendLen == NULL)||(pstRecvBufM->pucDataBuf == NULL))
    {
        return CPSS_ERROR ;
    }

    /*如果接收的管理结构中有完整数据包*/
    if(pstRecvBufM->ulDataFlag == CPSS_TCP_PKG_FINISH)
    {
        pstRecvBufM->ulDataFlag   = CPSS_TCP_PKG_NOT_FINISH ;
        pstRecvBufM->ulRecvBufLen = 0 ;
    }

    /*如果接收数据长度等于零*/
    if((*pulRecvLen == 0)||(pstRecvBufM->lDataDiffLen<=0))
    {
        pstRecvBufM->ulDataFlag   = CPSS_TCP_PKG_NOT_FINISH ;
        pstRecvBufM->ulRecvBufLen = 0 ;
        return CPSS_TCP_RECV_END ;
    }

    /*赋值*/
    pucKeepRecvBuf = pstRecvBufM->pucDataBuf + pstRecvBufM->ulRecvBufLen ;
    ulKeepDataLen  = pstRecvBufM->ulRecvBufLen ;

    if((ulKeepDataLen + (*pulRecvLen)) < CPSS_COM_DISP_HEAD_LEN)
    {
        cpss_mem_memcpy(pucKeepRecvBuf,*ppucRecvData,*pulRecvLen) ;           
        pstRecvBufM->ulRecvBufLen = pstRecvBufM->ulRecvBufLen + *pulRecvLen ;
        return CPSS_TCP_RECV_END ;
    }
    else
    {
        /*如果缓存中的数据小于分发头的长度,则拷贝分发头*/
        if(ulKeepDataLen < CPSS_COM_DISP_HEAD_LEN)
        {
            cpss_mem_memcpy(pucKeepRecvBuf,
                *ppucRecvData,CPSS_COM_DISP_HEAD_LEN-ulKeepDataLen) ;
        }
        
        pstDispHdr = (CPSS_COM_MSG_HEAD_T*)pstRecvBufM->pucDataBuf ;
        ulRecvDataLen = CPSS_DISP_HDR_LEN_SUBTRACT(ulKeepDataLen + *pulRecvLen) ;

        if(cpss_ntohl(pstDispHdr->ulLen) > ulRecvDataLen)
        {
            cpss_mem_memcpy(pucKeepRecvBuf,*ppucRecvData,*pulRecvLen) ;
            pstRecvBufM->ulRecvBufLen = pstRecvBufM->ulRecvBufLen + *pulRecvLen ;
            return CPSS_TCP_RECV_END ;
        }
        else
        {
            ulNeedDataLen = CPSS_DISP_HDR_TO_DATAEND_LEN_GET(cpss_ntohl(pstDispHdr->ulLen)) - ulKeepDataLen ;
            cpss_mem_memcpy(pucKeepRecvBuf,*ppucRecvData,ulNeedDataLen) ;
            
             pstRecvBufM->lDataDiffLen = pstRecvBufM->lDataDiffLen  - ulNeedDataLen;
			
            *pulRecvLen = *pulRecvLen - ulNeedDataLen ;
            *ppucRecvData = *ppucRecvData + ulNeedDataLen ;
            pstRecvBufM->ulDataFlag = CPSS_TCP_PKG_FINISH ;
            *ppucSendBuf = pstRecvBufM->pucDataBuf ;
            *pulSendLen = CPSS_DISP_HDR_TO_DATAEND_LEN_GET(cpss_ntohl(pstDispHdr->ulLen)) ;
            
            return CPSS_TCP_RECV_SEND_OK ;     
        }
    }
    return CPSS_TCP_RECV_END ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_recv_data_deal
* 功    能: TCP接收到数据后的处理 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*          成功：CPSS_OK 
*          失败：CPSS_ERROR
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_recv_data_deal
(
 CPS__TNBS_SOCKET_DATAIND_MSG_T* pstIndMsg
)
{
    CPSS_COM_DRV_TCP_T    *pstDrvTcp;
    CPSS_COM_DRV_T        *pstDrv ; 
    CPSS_COM_MSG_HEAD_T   *pstDispHdr ;
    CPSS_COM_TRUST_HEAD_T *pstTrustHdr ;
    
    INT32   lRet ;
    UINT32  ulServerFlag ;
    UINT32  ulRecvLen ;
    UINT32  ulSendDataLen ;
    UINT8   *pucRecvData ;
    UINT8   *pucSendBuf ;
    UINT32  ulDcFlag = 0 ;
    
     ulRecvLen = 0;
	 
    /*参数检查*/
    if(pstIndMsg == NULL)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_recv_data_deal para null\n");        
        return CPSS_ERROR ;
    }    
    
    /*找到对应的TCP链路信息*/
    pstDrvTcp = cpss_com_tcp_link_find_by_socket(cpss_ntohl(pstIndMsg->ulSocketId),&ulServerFlag) ;
    if(NULL == pstDrvTcp)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_recv_data_deal not find link socket=%d\n",cpss_ntohl(pstIndMsg->ulSocketId));
        return CPSS_ERROR ;
    }

    /*得到接收数据的长度和指针*/
    ulRecvLen   = cpss_ntohs(pstIndMsg->usLen) ;
    pucRecvData = pstIndMsg->aucData ;

    pstDrvTcp->stRecvBufM.lDataDiffLen = ulRecvLen;
    /*找到TCP驱动*/
    pstDrv = cpss_com_drv_find(CPSS_COM_DRV_TCP_SERVER);    

    while(1)
    {
        /*如果为网元直连*/
        if(ulServerFlag == CPSS_COM_TCP_DC_FLAG)
        {
            /*组织网元直连消息*/
            if(ulDcFlag == 1)
            {
                ulRecvLen = 0 ;
            }

            lRet = cpss_com_dc_pkg_building(&pucRecvData,&ulRecvLen,
                &pstDrvTcp->stRecvBufM,&pstTrustHdr) ;
            if(lRet == CPSS_TCP_RECV_END)
            {
                return CPSS_OK ;
            }
            
            ulDcFlag = 1 ;
        }

        else if((ulServerFlag == CPSS_COM_TCP_SERVER_FLAG)||(ulServerFlag == CPSS_COM_TCP_CLIENT_FLAG))
	
        {
            
            lRet = cpss_com_tcp_pkg_building(&pucRecvData,&ulRecvLen,
                &pstDrvTcp->stRecvBufM,&ulSendDataLen,&pucSendBuf) ;
            if((lRet == CPSS_TCP_RECV_END)||(lRet == CPSS_ERROR))
            {
                return CPSS_OK ;
            }
        }

        if(ulServerFlag != CPSS_COM_TCP_DC_FLAG)
        {
            /*申请IPC内存*/
            pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)cpss_com_nodisp_mem_alloc(ulSendDataLen) ;
            if(pstTrustHdr == NULL)
            {
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                    "cpss_com_tcp_recv_data_deal malloc failed size=%d\n",ulSendDataLen);        
                return CPSS_ERROR ;
            }

            pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr) ;        
            pstDispHdr->pucBuf = (UINT8*)CPSS_DISP_HDR_TO_USR_HDR(pstDispHdr) ;
        
            /*设置消息为共享内存*/
            pstDispHdr->ucShareFlag = CPSS_COM_MEM_SHARE ;
        
            /*拷贝数据内容*/
            cpss_mem_memcpy(pstDispHdr,pucSendBuf,ulSendDataLen) ;
       
            /*如果是客户端的链路,则将收到的数据直接发送给本板的纤程*/
            if(ulServerFlag == CPSS_COM_TCP_CLIENT_FLAG)
            {
                cpss_com_logic_addr_get(
                    &pstDispHdr->stDstProc.stLogicAddr,&pstDispHdr->stDstProc.ulAddrFlag) ;
            
                pstDispHdr->stDstProc.stLogicAddr.usGroup = 
                    cpss_htons(pstDispHdr->stDstProc.stLogicAddr.usGroup) ;
                pstDispHdr->stDstProc.ulAddrFlag = cpss_htonl(pstDispHdr->stDstProc.ulAddrFlag) ;
            }
        }
        else
        {
            pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr) ;
            pstDispHdr->stSrcProc.stLogicAddr = pstDrvTcp->stLogicAddr ; 
        }
        
        /*调用TCP驱动接收数据*/
        lRet = pstDrv->pfRead(CPSS_COM_DRV_TCP_SERVER,(UINT32)pstDrvTcp,(UINT8*)pstTrustHdr,0) ;
        if(lRet != CPSS_OK)
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "cpss_com_tcp_recv_data_deal tcp read failed msgid=%x,srcPid=%x,dstPid=%x",
                pstDispHdr->ulMsgId,pstDispHdr->stSrcProc.ulPd,pstDispHdr->stDstProc.ulPd) ;

		 /*   return CPSS_ERROR ;*/
	 
        }
        else
        {
        if(ulServerFlag != CPSS_COM_TCP_DC_FLAG)
        {
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "cpss_com_tcp_recv_data_deal tcp read OK msgid=%x,srcPid=%x,dstPid=%x",
                pstDispHdr->ulMsgId,pstDispHdr->stSrcProc.ulPd,pstDispHdr->stDstProc.ulPd) ;

		  /*return CPSS_OK ;*/
		 
            }
	      else
	      	{
	      	    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "cpss_com_tcp_dc_recv_data_deal tcp read OK msgid=%x,srcPid=%x,dstPid=%x",
                pstDispHdr->ulMsgId,pstDispHdr->stSrcProc.ulPd,pstDispHdr->stDstProc.ulPd) ;
	      	}
        }
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_connect_req_deal
* 功    能: tcp纤程接收连接请求消息的处理流程
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*          成功：CPSS_OK 
*          失败：CPSS_ERROR
* 说   明: LDT接入的链路
******************************************************************************/
INT32 cpss_com_tcp_connect_req_deal
(
 CPS__TNBS_SOCKET_SETUP_IND_MSG_T* pstConnectReq
)
{
    UINT32 ulIpAddr = 0 ;
    INT32 lRet ;
    UINT16 usPeerPort = 0;
    
    /*参数校验*/
    if(pstConnectReq == NULL)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_connect_req_deal parameter null\n");
        return CPSS_ERROR ;
    }

    /*转化IP字符串为IP整型*/
    cpss_com_tcp_ipstr_2_ipint(pstConnectReq->astDestIpAddr[0].aucIp,&ulIpAddr) ;
    usPeerPort = cpss_ntohs(pstConnectReq->usPeerPort);

    /*记录TCP连接的IP,port,sockId*/
    if(cpss_ntohl(pstConnectReq->ulUsrId) == CPSS_COM_TCP_SERVER_USRID)
    {
    lRet = cpss_com_tcp_link_add(cpss_ntohl(pstConnectReq->ulSocketId),
        ulIpAddr,usPeerPort,CPSS_COM_TCP_SERVER_FLAG) ;
    }
    else if(cpss_ntohl(pstConnectReq->ulUsrId) == CPSS_COM_DC_SERVER_USRID)
    {
        lRet = cpss_com_tcp_link_add(cpss_ntohl(pstConnectReq->ulSocketId),
            ulIpAddr,usPeerPort,CPSS_COM_TCP_DC_FLAG) ;
    }
    else
    {
        lRet = CPSS_ERROR ;
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "tnbs's tcp connect indicate msg usrid=%d,is not expected\n",
            cpss_ntohl(pstConnectReq->ulUsrId)) ;
    }
        
    if(CPSS_OK != lRet)
    {
        g_stCpssTcpErrStat.ulLinkAddFailNum++ ;
        
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_connect_req_deal failed ip=0x%x,port=0x%x!\n",
            ulIpAddr,cpss_ntohs(pstConnectReq->usPeerPort)); 
        return lRet ;
    }
    
    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
            "cpss_com_tcp_connect_req_deal ip=0x%x,port=0x%x!\n",
            ulIpAddr,cpss_ntohs(pstConnectReq->usPeerPort)); 

    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_exception_deal
* 功    能: tcp连接故障的处理
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*          成功：CPSS_OK 
*          失败：CPSS_ERROR
* 说   明: 
******************************************************************************/
INT32 cpss_com_tcp_exception_deal
(
 CPS__TNBS_SOCKET_EXCP_IND_MSG_T* pstExcpInd
)
{
    INT32 lRet ;
    CPSS_COM_DRV_TCP_T *pstTcpLinkInfo ;
    CPSS_COM_TCP_DISCONN_IND_MSG_T stDisconnMsg ;
    CPSS_COM_TCP_LINK_T stTcpLink ;
    UINT32 ulServerFlag ;

    /*查找socket对应的TCP链路*/
    pstTcpLinkInfo = cpss_com_tcp_link_find_by_socket(cpss_ntohl(pstExcpInd->ulSocketId),&ulServerFlag) ;
    if(NULL == pstTcpLinkInfo)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_exception_deal not find link socket=0x%x\n",
            cpss_ntohl(pstExcpInd->ulSocketId)) ;      
        return CPSS_OK ;
    }

    g_stCpssTcpErrStat.ulExceptionNum++ ;

    /*向OAMS发送disconnect消息*/
    stDisconnMsg.ulIP      = pstTcpLinkInfo->stTcpLink.ulIP ;
    stDisconnMsg.usTcpPort = pstTcpLinkInfo->stTcpLink.usTcpPort ;
    
    /*增加主动断开连接的统计*/
    pstTcpLinkInfo->stLinkSetupDelStat.ulLinkFaultActiveNum++;

    /*向OAMS发送链路断开指示消息*/
    lRet = cpss_com_tcp_cps__oams_connect_info_send(
        (UINT8*)&stDisconnMsg,CPSS_COM_TCP_DISCONN_IND_MSG,ulServerFlag);
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_exception_deal send tcp disconnect indicate message failed!");
    }

    /*如果发送指示消息失败,则主动删除CPSS自己保存的TCP链路信息*/
    if(CPSS_OK != lRet)
    {
        stTcpLink.ulIP      = pstTcpLinkInfo->stTcpLink.ulIP ;
        stTcpLink.usTcpPort = pstTcpLinkInfo->stTcpLink.usTcpPort ;
        
        /*删除TCP链路*/
        cpss_com_tcp_link_disconnect (&stTcpLink) ;
    }

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "cpss_com_tcp_exception_deal deal result=%d\n",lRet);

    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_tcp_link_info_record                            
* 功    能: 将去告警箱的TCP链路的IP,port信息记录在全局管理结构中
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*          成功：CPSS_OK 
*          失败：CPSS_ERROR
* 说   明:
******************************************************************************/
INT32 cpss_com_tcp_link_info_record
(
CPSS_COM_TCP_LINK_T* pstTcpLink,
UINT32 *pulTcpIndex
)
{
    UINT32 ulLoop = 0 ;
    UINT32 ulRecordFlag = FALSE ;
    
    /*参数校验*/
    if((NULL == pstTcpLink)||(NULL == pulTcpIndex))
    {
        return CPSS_ERROR ;
    }

    /*循环查找*/
    for(ulLoop=0;ulLoop<CPSS_COM_DRV_CLIENT_TCP_NUM;ulLoop++)
    {
        if((g_stCpssTcpMan.stTcpConnectInfo[ulLoop].ulIP == pstTcpLink->ulIP)&&
            (g_stCpssTcpMan.stTcpConnectInfo[ulLoop].usTcpPort == pstTcpLink->usTcpPort))
        {
            ulRecordFlag  = TRUE ;
            break ;
        }
    }

    /*如果TCP信息记录存在*/
    if(ulRecordFlag == TRUE)
    {
        *pulTcpIndex = ulLoop ;
        return CPSS_OK ;
    }

    /*如果TCP信息记录不存在*/
    for(ulLoop=0;ulLoop<CPSS_COM_DRV_CLIENT_TCP_NUM;ulLoop++)
    {
        if(g_stCpssTcpMan.stTcpConnectInfo[ulLoop].ulIP == 0)
        {
            g_stCpssTcpMan.stTcpConnectInfo[ulLoop].ulIP = pstTcpLink->ulIP ;
            g_stCpssTcpMan.stTcpConnectInfo[ulLoop].usTcpPort = pstTcpLink->usTcpPort ;
            break ;
        }
    }

    /*如果没有空余位置*/
    if(ulLoop == CPSS_COM_DRV_CLIENT_TCP_NUM)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_tcp_link_info_record has no free space.ip=0x%x.port=%d\n",
            pstTcpLink->ulIP,pstTcpLink->usTcpPort) ;        
        return CPSS_ERROR ;
    }

    if(ulLoop == 0)
    {

        /*启动连接定时器0*/
      /*  cpss_timer_loop_set(CPSS_COM_TCP_CONNECT_TM0_NO,CPSS_COM_TCP_CONNECT_TM_INTERVAL) ;  */
    }
    else if(ulLoop == 1)
    {
        /*启动连接定时器1*/
      /*  cpss_timer_loop_set(CPSS_COM_TCP_CONNECT_TM1_NO,CPSS_COM_TCP_CONNECT_TM_INTERVAL) ;  */
    }

    *pulTcpIndex = ulLoop ;    
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_cps__oams_connect_req_deal                            
* 功    能:cpss接收到操作维护的连接请求消息的处理过程
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*          成功：CPSS_OK 
*          失败：CPSS_ERROR
* 说   明:
******************************************************************************/
INT32 cpss_com_cps__oams_connect_req_deal
(
 CPSS_COM_TCP_CONNECT_REQ_MSG_T* pstConnectReq,
 CPSS_COM_PID_T* pstDstPid
)
{
    CPSS_COM_TCP_LINK_T stTcpLink ;
    UINT32 ulTcpIndex ;    
    INT32  lRet = CPSS_ERROR ;

    /*参数校验*/
    if((pstConnectReq == NULL)||(pstDstPid == NULL))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "cpss_com_cps__oams_connect_req_deal para null.\n") ;
        return CPSS_ERROR ;
    }

    stTcpLink.ulIP       = pstConnectReq->ulIP ;
    stTcpLink.usTcpPort  = pstConnectReq->usTcpPort ;
    stTcpLink.usReserved = 0 ;

    /*将TCP链路信息记录在全局结构中*/
    lRet = cpss_com_tcp_link_info_record(&stTcpLink,&ulTcpIndex) ;
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "cpss_com_tcp_link_info_record ok.ip=0x%x,port=%d\n",
        stTcpLink.ulIP,stTcpLink.usTcpPort) ;

    /*将发送纤程的地址信息进行保存*/
    g_stCpssTcpMan.stOamsPid = *pstDstPid ;

    /*发送连接请求*/
    cpss_com_tcp_connect_req_send(&stTcpLink,ulTcpIndex) ;

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_cps__oams_connect_req_deal success.ip=0x%x,port=0x%x\n",
        stTcpLink.ulIP,stTcpLink.usTcpPort) ;
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_connect_timer_msg_deal                            
* 功    能: 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明:
******************************************************************************/
VOID cpss_com_connect_timer_msg_deal
(
    UINT32 ulTimerIndex
)
{
    CPSS_COM_TCP_LINK_T stTcpLink ;

    /*填充连接的TCP信息*/
    stTcpLink.ulIP      = g_stCpssTcpMan.stTcpConnectInfo[ulTimerIndex].ulIP ;
    stTcpLink.usTcpPort = g_stCpssTcpMan.stTcpConnectInfo[ulTimerIndex].usTcpPort ;  
    
    /*重新发送TCP链路connect消息*/
    cpss_com_tcp_connect_req_send(&stTcpLink,ulTimerIndex) ;
}


/*******************************************************************************
* 函数名称: cpss_com_tcp_proc                            
* 功    能: 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明:
******************************************************************************/
VOID cpss_com_tcp_proc
(
 UINT16  usUserState,
 VOID *  pvVar,
 CPSS_COM_MSG_HEAD_T *pstMsgHead
)
{
    INT32 lRet ;
    
    switch(pstMsgHead->ulMsgId)
    {
    case SMSS_PROC_ACTIVATE_REQ_MSG:
        {
            cpss_output(CPSS_MODULE_COM, CPSS_PRINT_INFO,
                "CPSS_COM: cpss_com_tcp_proc recv SMSS_PROC_ACTIVATE_REQ_MSG\n");
            
            cpss_active_proc_rsp_send(CPSS_OK);
            break ;
        }
    case SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG:
        {
            cpss_output(CPSS_MODULE_COM, CPSS_PRINT_INFO,
                "CPSS_COM: cpss_com_tcp_proc recv SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG\n");
            
            cpss_com_tcp_proc_init(
                (SMSS_PROC_ACTIVATE_REQ_MSG_T*)pstMsgHead->pucBuf) ;
            break;
        }
    case CPSS_COM_TCP_LISTEN_TM_MSG:
        {
            cpss_output(CPSS_MODULE_COM, CPSS_PRINT_INFO,
                "CPSS_COM: cpss_com_tcp_proc recv CPSS_COM_TCP_LISTEN_TM_MSG\n");            

            cpss_com_tcp_listen_timeout_deal() ;
            break ;
        }
    case CPS__TNBS_SOCKET_LISTEN_RSP_MSG:
        {
            cpss_output(CPSS_MODULE_COM, CPSS_PRINT_INFO,
                "CPSS_COM: cpss_com_tcp_proc recv CPS__TNBS_SOCKET_LISTEN_RSP_MSG\n");            
            
            cpss_com_tcp_listen_rsp_deal(
                (CPS__TNBS_SOCKET_LISTEN_RSP_MSG_T*)pstMsgHead->pucBuf) ;
            break ;
        }
    case CPS__TNBS_SOCKET_SETUP_RSP_MSG:
        {
            cpss_com_tcp_connect_rsp_deal(
                (CPS__TNBS_SOCKET_SETUP_RSP_MSG_T*)pstMsgHead->pucBuf) ;
            break ;
        }
    case CPS__TNBS_SOCKET_DATAIND_MSG:
        {
            cpss_com_tcp_recv_data_deal(
                (CPS__TNBS_SOCKET_DATAIND_MSG_T*)pstMsgHead->pucBuf) ;
            break ;
        }
    case CPS__TNBS_SOCKET_SETUP_IND_MSG:
        {
            cpss_com_tcp_connect_req_deal(
                (CPS__TNBS_SOCKET_SETUP_IND_MSG_T*)pstMsgHead->pucBuf) ;
            break ;
        }
#if 0
    case CPS__TNBS_SOCKET_DATASEND_RSP_MSG:
        {
            cpss_com_tcp_data_send_rsp_deal(
                (CPS__TNBS_SOCKET_DATASEND_RSP_MSG_T*)pstMsgHead->pucBuf) ;
            break;
        }
#endif
    case CPS__TNBS_SOCKET_EXCP_IND_MSG:
        {
            cpss_com_tcp_exception_deal(
                (CPS__TNBS_SOCKET_EXCP_IND_MSG_T*)pstMsgHead->pucBuf) ;
            break ;
        }
    case CPSS_COM_TCP_CONNECT_REQ_MSG:
        {
            cpss_com_cps__oams_connect_req_deal(
                (CPSS_COM_TCP_CONNECT_REQ_MSG_T*)pstMsgHead->pucBuf,&pstMsgHead->stSrcProc) ;
            break ;
        }
    case CPSS_COM_TCP_CONNECT_TM0_MSG:
        {
            cpss_com_connect_timer_msg_deal(0) ;
            break ;
        }
    case CPSS_COM_TCP_CONNECT_TM1_MSG:
        {
            cpss_com_connect_timer_msg_deal(1) ;
            break ;
        }
    case SMSS_STANDBY_TO_ACTIVE_REQ_MSG:    /* 备升主的请求消息 */
        {
            /* 发送响应消息 */
            lRet = cpss_standby_to_active_send(CPSS_OK);
            if (CPSS_OK != lRet)
            {                
                cpss_output(CPSS_MODULE_COM, CPSS_PRINT_ERROR,
                    "CPSS_COM: cpss_com_tcp_proc send SMSS_STANDBY_TO_ACTIVE_RSP_MSG failed!");    
            }
            break;
        }
    default:
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "tcp proc recv unknown msgid=0x%x\n.",pstMsgHead->ulMsgId) ;
            break ;
        }
    }
}
#else
INT32 cpss_com_tcp_server_link_id_find
(
 CPSS_COM_TCP_LINK_T* pstTcpLink,
 UINT32 *pulLinkId
)
{
	return CPSS_OK ;
}

#endif

/*******************************************************************************
* 函数名称: cpss_com_tcp_listen_timeout_deal                            
* 功    能: 
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: tcp纤程初始化函数
******************************************************************************/
VOID cpss_com_tcp_listen_show()
{
    cps__oams_shcmd_printf("ulTcpProcInit=%d\n",g_stTcpListenStat.ulTcpProcInit) ;
    
    cps__oams_shcmd_printf("---- tcp listen stat ----\n") ;
    cps__oams_shcmd_printf("ulListenReqNum=%d\n",g_stTcpListenStat.ulListenReqNum) ;
    cps__oams_shcmd_printf("ulListenResult=%d\n",g_stTcpListenStat.ulListenResult) ;

    cps__oams_shcmd_printf("---- dc listen stat ----\n") ;
    cps__oams_shcmd_printf("ulDcListenReqNum=%d\n",g_stTcpListenStat.ulDcListenReqNum) ;
    cps__oams_shcmd_printf("ulDcListenResult=%d\n",g_stTcpListenStat.ulDcListenResult) ;
}

/*******************************************************************************
* 函数名称: cpss_com_abox_ipinfo_get                            
* 功    能: 通过目的地址得到源地址
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明: 
******************************************************************************/
INT32 cpss_com_abox_ipinfo_get
(
CPSS_COM_TCP_LINK_T* pstDstAddr,
CPSS_COM_TCP_LINK_T* pstSrcAddr
)
{
    #ifdef SWP_FNBLK_BRDTYPE_ABOX  
#ifdef CPSS_VOS_VXWORKS    
 
    UINT32 ulServerFlag ;
    CPSS_COM_DRV_TCP_T *pstTcpDrv; 
    CPSS_COM_SOCK_ADDR_T stSockAddr ;
    UINT32 ulSockLen ;  

    if((pstDstAddr == NULL)||(pstSrcAddr == NULL))
    {
        return CPSS_ERROR ;
    }
    
    pstTcpDrv = cpss_com_tcp_link_find(pstDstAddr,&ulServerFlag);
    if(pstTcpDrv == NULL)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_abox_ipinfo_get not find.ip=0x%x,port=0x%x\n",
        pstDstAddr->ulIP,pstDstAddr->usTcpPort) ;
        return CPSS_ERROR ;
    }
    
    ulSockLen = sizeof(stSockAddr) ;
    if(getsockname(pstTcpDrv->ulSClient,(struct sockaddr*)&stSockAddr,&ulSockLen)!=OK)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_abox_ipinfo_get getsockname failed.errno=%d\n",errno) ;                
        return CPSS_ERROR ;
    }
    
    pstSrcAddr->ulIP = cpss_ntohl(stSockAddr.sin_addr.s_addr) ;
    pstSrcAddr->usTcpPort = cpss_ntohs(stSockAddr.sin_port) ;
    
    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_abox_ipinfo_get success ip=0x%x.port=%d\n",pstSrcAddr->ulIP,pstSrcAddr->usTcpPort) ;  
#endif
    #endif
    return CPSS_OK ;  
}

/*******************************************************************************
* 函数名称: cpss_com_ldt_send                            
* 功    能: 直接调用socket向LDT发送数据的函数
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：其它;
* 说   明: 只能在主控板上调用                                          
******************************************************************************/  

INT32 cpss_com_ldt_send(UINT8* pucSendBuf,UINT32 ulSendLen)
{
#if 0 /*def CPSS_FUNBRD_MC */
    CPSS_COM_SOCK_ADDR_T stSockAddr;
    INT32 lRet ;
    
    /*参数校验*/
    if(pucSendBuf == NULL)
    {
        return CPSS_ERROR ;
    }

    /* 填写socket参数 */
    cpss_mem_memset(&stSockAddr,0,sizeof(stSockAddr));
    stSockAddr.sin_family=AF_INET;
    stSockAddr.sin_addr.s_addr=cpss_htonl(CPSS_COM_LDT_ADDR_IP);
    stSockAddr.sin_port=cpss_htons(CPSS_COM_LDT_ADDR_PORT);

    /* 数据发送 */
    lRet=sendto(g_ulCpssLdtSocket,pucSendBuf,ulSendLen, 0, 
        (struct sockaddr*)&stSockAddr,sizeof(CPSS_COM_SOCK_ADDR_T));
    if(SOCKET_ERROR == lRet)
    {
        cpss_output(CPSS_MODULE_COM, CPSS_PRINT_ERROR,
            "CPSS_COM: cpss_com_ldt_send send failed!errno=%d\n",cpss_com_sock_error_get());    
        return CPSS_ERROR ;
    }
#endif
    return CPSS_OK ;
}

/******************************* 源文件结束 **********************************/

