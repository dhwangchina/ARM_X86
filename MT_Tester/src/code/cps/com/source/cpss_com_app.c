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
#include <sys/msg.h>
#include <sys/ipc.h>
#include "cpss_public.h"
#include "cpss_common.h"

#include "cpss_com_common.h"
#include "cpss_com_link.h"
#include "cpss_com_drv.h"
#include "cpss_com_app.h"
#include "cpss_com_trace.h"
#include "cpss_err.h"

/******************************* 局部宏定义 **********************************/

/******************************* 全局变量定义/初始化 *************************/
CPSS_SEND_INTERFACE_STAT_T gstInterfaceStat;
/******************************* 局部常数和类型定义 **************************/
extern CPSS_VK_PROC_CLASS_T  g_astCpssVkProcClassTbl [VK_MAX_PROC_CLASS_NUM];
extern CPSS_VK_PROC_DESC_T  g_astCpssVkProcDescTbl [VK_MAX_PROC_INST_NUM];
extern UINT32 g_ulCpssComInitFlag ;
CPSS_COM_SPEC_MSG_STAT_T  g_astSpecMsgStat[CPSS_COM_SPEC_MSG_NUM_MAX] ;
UINT32 g_ulCpssSpecMsgStatFlag = CPSS_COM_SPEC_MSG_STAT_OFF ;
INT32 g_BootMsgId;

extern void cps_boot_reboot(CPS_REBOOT_T* stRbt);
void cps_com_boot_msg_dealing();
void cpss_message(const CHAR *pcFormat, ...);
/******************************* 局部函数原型声明 ****************************/

/******************************* 函数实现 ************************************/

/*******************************************************************************
* 函数名称: cpss_com_send
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
INT32 cpss_com_send
(
CPSS_COM_PID_T *pstDstPid,
UINT32 ulMsgId,
UINT8 *pucData,
UINT32 ulDataLen
)
{
    CPSS_COM_MSG_HEAD_T stMsg;

    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulComSendInCount);
/
    stMsg.stDstProc   = *pstDstPid;
    stMsg.ulMsgId      = ulMsgId;
    stMsg.pucBuf       = pucData;
    stMsg.ulLen         = ulDataLen;
    stMsg.ucPriFlag  = CPSS_COM_PRIORITY_NORMAL ;
    stMsg.ucShareFlag= CPSS_COM_MEM_NOT_SHARE ;
    stMsg.ucAckFlag  = CPSS_COM_ACK;
    stMsg.usReserved = 0;
	
    stMsg.stSrcProc.stLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
    stMsg.stSrcProc.ulAddrFlag   = g_pstComData->stAddrThis.ulAddrFlag;
    if(CPSS_DBG_DIAGN_MSGID == ulMsgId)
    {
    	stMsg.stSrcProc.ulPd = CPSS_DBG_MNGR_PROC;
    }
    else
    {
        stMsg.stSrcProc.ulPd = cpss_vk_pd_self_get();
    }

    #if (CPSS_TRACE_ROUTE_FLAG == TRUE)
    {
        CPSS_TRACE_HOOK_INFO_T stTraceHook;
        stTraceHook.ulMsgId = ulMsgId;
        cpss_trace_route_it(&stTraceHook, NULL, CPSS_TRACE_LAYER_APPI, CPSS_TRACE_IF_COM_SEND);
    }
    #endif

    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulComSendOutCount);

    return cpss_com_send_extend(&stMsg);
}

/*******************************************************************************
* 函数名称: cpss_com_send_dsp
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
INT32 cpss_com_send_dsp
(
CPSS_COM_PID_T *pstDstPid,
UINT32 ulMsgId,
UINT8 *pucData,
UINT32 ulDataLen
)
{
    UINT16  usGuid;
    UINT16  usInst;
    UINT32  ulIndex;
    VK_PROC_ID  tProcId;
    CPSS_COM_MSG_HEAD_T stMsg;

    stMsg.stDstProc   = *pstDstPid;
    stMsg.ulMsgId      = ulMsgId;
    stMsg.pucBuf       = pucData;
    stMsg.ulLen         = ulDataLen;
    stMsg.ucAckFlag  = CPSS_COM_ACK;



    usGuid = (UINT16) (((pstDstPid->ulPd) >> 16) & 0x0000ffff);
    if ((usGuid <= 0) || (usGuid >= VK_MAX_PROC_CLASS_NUM))
	{
	    return CPSS_ERROR;
	}
    usInst = (UINT16) ((pstDstPid->ulPd) & 0x0000ffff);
    if (usInst >= g_astCpssVkProcClassTbl[usGuid].usTotalInst)
    {
        return CPSS_ERROR;
    }
    ulIndex = g_astCpssVkProcClassTbl[usGuid].ulBegin + usInst;

    tProcId = g_astCpssVkProcDescTbl[ulIndex].tProcId;

    (tProcId->pfEntry) (tProcId->usState, tProcId->pcVar,
        &stMsg);

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_send_local
* 功    能: CPU内纤程间消息发送，只允许在纤程中调用，以调用者的身份发送消息。
*
* 函数类型:
* 参    数:
* 参数名称        类型            输入/输出         描述
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
INT32 cpss_com_send_local
(
CPSS_COM_MSG_HEAD_T *pstMsg
)
{
    CPSS_COM_TRUST_HEAD_T *pstTrustHdr;
    CPSS_COM_MSG_HEAD_T* pstComHdr ;
    INT32 lRet;

    /* 填写目的地址 */
    pstMsg->stDstProc.stLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
    pstMsg->stDstProc.ulAddrFlag  = g_pstComData->stAddrThis.ulAddrFlag;

     /* 如果源地址为空, 则CPSS负责填写 */
    if(TRUE == CPSS_COM_LOGIC_ADDR_ZERO(pstMsg->stSrcProc.stLogicAddr))
    {
        pstMsg->stSrcProc.stLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
        pstMsg->stSrcProc.ulAddrFlag  = g_pstComData->stAddrThis.ulAddrFlag;
        pstMsg->stSrcProc.ulPd        = cpss_vk_pd_self_get();
    }

    /* 构造消息报文格式 */
    pstTrustHdr = cpss_com_packet_setup_local(pstMsg);
    if(NULL == pstTrustHdr)
    {
        gstInterfaceStat.ulComSendLocalPcaketSetupFailNum = gstInterfaceStat.ulComSendLocalPcaketSetupFailNum +1;
        return CPSS_ERR_COM_PACKET_SETUP_FAIL;
    }

    pstComHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr) ;

    /*消息跟踪*/
    cpss_com_trace(pstComHdr) ;

    /* CPU内消息发送 */
    lRet = cpss_com_ipc_msg_send(pstTrustHdr, IPC_MSG_TYPE_LOCAL);
    if(CPSS_OK != lRet)
    {
        gstInterfaceStat.ulComSendLocalIpcSendFailNum = gstInterfaceStat.ulComSendLocalIpcSendFailNum + 1;
        cpss_output(CPSS_MODULE_COM, CPSS_PRINT_WARN,
            "cpss_com_send_local, inside msg failed!");
        cpss_com_mem_free (pstTrustHdr);
    }

    return (lRet);

}
/******************************************************************************
* 函数名称: cpss_com_tcp_send
* 功    能: TCP数据的发送函数
*
* 函数类型:
* 参    数:
* 参数名称        类型                输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:
*******************************************************************************/
INT32 cpss_com_tcp_send(CPSS_COM_MSG_HEAD_T* pstMsg)
{
    CPSS_COM_LINK_T stLink;
    CPSS_COM_DRV_TCP_T stTcpDrv;
    CPSS_COM_PID_T stDstPid={{0}};
    CPSS_COM_DRV_T *pstDrv;
    INT32 lRet;

    cpss_mem_memset(&stLink,0,sizeof(CPSS_COM_LINK_T));
    cpss_mem_memset(&stTcpDrv,0,sizeof(CPSS_COM_DRV_TCP_T));
    pstDrv=cpss_com_drv_find(CPSS_COM_DRV_TCP_SERVER);

   /* if(NULL == pstDrv) return CPSS_ERROR; */
    lRet = cpss_local_funcbrd_type_is_mc() ;

    if((TRUE == lRet) && (CPSS_COM_ADDRFLAG_MASTER == g_pstComData->stAddrThis.ulAddrFlag))
    {
        /* 根据逻辑地址,查找对应的tcp链路对象 */
 #ifdef CPSS_FUNBRD_MC
        /*tcp发送包的个数统计*/
        stTcpDrv.stLinkDataStat.ulSendNum = stTcpDrv.stLinkDataStat.ulSendNum + 1;
        lRet = cpss_com_tcp_drv_find(pstMsg->stDstProc.stLogicAddr,&stTcpDrv);
        if(CPSS_OK != lRet)
        {
           /*tcp发送包失败的个数统计*/
            stTcpDrv.stLinkDataStat.ulTcpSendFailNum = stTcpDrv.stLinkDataStat.ulTcpSendFailNum + 1;
            return CPSS_ERR_COM_TCP_LINK_NOT_FIND ;
        }

        /* TCP消息发送 */
        if(stTcpDrv.ulServerFlag != CPSS_COM_TCP_DC_FLAG)
        {
            if(NULL == pstDrv) return CPSS_ERROR;
            /* 填写链路参数 */
            cpss_mem_memcpy(&stLink.stDrv,pstDrv,sizeof(CPSS_COM_DRV_T));
            stLink.stDrv.ulDrvD                       = stTcpDrv.ulSClient;
            stLink.stDrv.stSocketAddr.sin_addr.s_addr = cpss_htonl(stTcpDrv.stTcpLink.ulIP);
            stLink.stDrv.stSocketAddr.sin_port        = cpss_htons(stTcpDrv.stTcpLink.usTcpPort);

            /* 消息发送 */
	        lRet = cpss_com_remote_msg_send(pstMsg, &stLink);
	        if(CPSS_OK != lRet)
	        {
                /*tcp发送包失败的个数统计*/
	            stTcpDrv.stLinkDataStat.ulTcpSendFailNum = stTcpDrv.stLinkDataStat.ulTcpSendFailNum + 1;
	        }
            return  lRet;
        }
        /* 网元直连消息发送 */
        else
        {
            lRet = cpss_com_tcp_data_send(stTcpDrv.ulSClient,pstMsg->pucBuf,pstMsg->ulLen) ;
            return lRet ;
        }
#else
        return (CPSS_ERROR);
#endif
    }

    /* 如果本板不是GCPA主用板,则发送到主控板进行转发 */
    cpss_mem_memset(&stDstPid,0,sizeof(stDstPid));
    stDstPid.stLogicAddr = g_pstComData->stAddrGcpaM.stLogiAddr;
    stDstPid.ulAddrFlag  = g_pstComData->stAddrGcpaM.ulAddrFlag;

    lRet = cpss_com_send_forward(pstMsg,&stDstPid);

    return lRet;

}


/*******************************************************************************
* 函数名称: cpss_com_send_extend
* 功    能: 多功能的消息发送。可以以其他纤程的身份进行发送，可以指定消息发送标志，
*           适合于特殊场合。
*
* 函数类型:
* 参    数:
* 参数名称        类型                输入/输出         描述
* pstDstPid   CPSS_COM_MSG_HEAD_T*     输入            消息头结构指针
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: O接口最大消息体长度：16KBYTE。
*           主备板之间最大消息体长度：2MBYTE。
*           CPU间最大消息体长度：（64K－256）BYTE。
*           CPU内最大消息体长度：无限制。
*******************************************************************************/
INT32 cpss_com_send_extend
(
 CPSS_COM_MSG_HEAD_T *pstMsg
 )
{
    CPSS_COM_TRUST_HEAD_T *pstTrustHdr;
    CPSS_COM_LOGIC_ADDR_T stLogAddr ;
    CPSS_COM_PID_T*   pstDstPid ;
    CPSS_COM_LINK_T*  pstLink;
    CPSS_COM_ROUTE_T* pstRoute ;
    UINT32 ulIdx ;
    UINT32 ulAddrFlag ;
    INT32 lRet;

    if(pstMsg->stDstProc.stLogicAddr.usGroup == 1)
    {
    	pstMsg->stDstProc.stLogicAddr.usGroup = 0x8111;
    	pstMsg->stDstProc.stLogicAddr.ucSubGroup = 1;
    }
    else if(pstMsg->stDstProc.stLogicAddr.usGroup == 2)
    {
    	switch(pstMsg->stDstProc.stLogicAddr.ucSubGroup)
    	{
    	case 1:
    		pstMsg->stDstProc.stLogicAddr.usGroup = 0x8112;
    		pstMsg->stDstProc.stLogicAddr.ucSubGroup = 2;
    		break;
    	case 2:
    		pstMsg->stDstProc.stLogicAddr.usGroup = 0x8113;
    		pstMsg->stDstProc.stLogicAddr.ucSubGroup = 2;
    		break;
    	case 3:
    		pstMsg->stDstProc.stLogicAddr.usGroup = 0x8114;
    		pstMsg->stDstProc.stLogicAddr.ucSubGroup = 2;
    		break;
    	}
    }

    if(pstMsg->stSrcProc.stLogicAddr.usGroup == 0x8111)
        {
        	pstMsg->stSrcProc.stLogicAddr.usGroup = 1;
        	pstMsg->stSrcProc.stLogicAddr.ucSubGroup = 1;
        }
        else if(pstMsg->stSrcProc.stLogicAddr.usGroup == 0x8112)
        {
        	pstMsg->stSrcProc.stLogicAddr.usGroup = 2;
        	pstMsg->stSrcProc.stLogicAddr.ucSubGroup = 1;
        }
        else if(pstMsg->stSrcProc.stLogicAddr.usGroup == 0x8113)
        {
        	pstMsg->stSrcProc.stLogicAddr.usGroup = 2;
        	pstMsg->stSrcProc.stLogicAddr.ucSubGroup = 2;
        }
        else if(pstMsg->stSrcProc.stLogicAddr.usGroup == 0x8114)
        {
        	pstMsg->stSrcProc.stLogicAddr.usGroup = 2;
        	pstMsg->stSrcProc.stLogicAddr.ucSubGroup = 3;
        }
    /*add end by zhao chuan for com send*/

    if(g_ulCpssComInitFlag == CPSS_COM_INIT_UNKNOWN)
    {
        cpss_output(CPSS_MODULE_COM, CPSS_PRINT_WARN,
            "cpss_com_send, com module not init!");
        return CPSS_ERROR ;
    }

    /* 检查函数入参有效性 */
    if(NULL == pstMsg)
    {
        gstInterfaceStat.ulComSendExMsgNullFailNum = gstInterfaceStat.ulComSendExMsgNullFailNum + 1;
        return CPSS_ERR_COM_PARA_ILLEGAL;
    }

    if(pstMsg->ucShareFlag != 0)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,"app cant usr share memory to send.srcPid=%x,srcLogaddr=%x,destProc=%x,msgId=%x\n",
            pstMsg->stSrcProc.ulPd,*(UINT32*)&pstMsg->stSrcProc.stLogicAddr,pstMsg->stDstProc.ulPd,pstMsg->ulMsgId) ;
        gstInterfaceStat.ulShareMemSendNum = gstInterfaceStat.ulShareMemSendNum + 1 ;
        return CPSS_ERR_COM_SHARE_MEM_NOT_SUPPORT ;
    }

    pstDstPid = &pstMsg->stDstProc;
    stLogAddr = pstDstPid->stLogicAddr ;
    ulAddrFlag = pstDstPid->ulAddrFlag ;

    /* 如果源地址为空, 则CPSS负责填写 */
    if(CPSS_COM_LOGIC_ADDR_ZERO(pstMsg->stSrcProc.stLogicAddr))
    {
        pstMsg->stSrcProc.stLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
        pstMsg->stSrcProc.ulAddrFlag  = g_pstComData->stAddrThis.ulAddrFlag;
        pstMsg->stSrcProc.ulPd        = cpss_vk_pd_self_get();
    }

    /*增加统计*/
    cpss_com_spec_msg_stat(pstMsg->ulMsgId,COM_SEND_PHASE) ;
    /*消息跟踪*/
    cpss_com_trace(pstMsg);
    #if (CPSS_TRACE_ROUTE_FLAG == TRUE)
    {
        CPSS_TRACE_HOOK_INFO_T stTraceHook;
        stTraceHook.ulMsgId = pstMsg->ulMsgId;
        cpss_trace_route_it(&stTraceHook, NULL, CPSS_TRACE_LAYER_APPI, CPSS_TRACE_IF_SEND_EX);
    }
    #endif
/* 处理CPU内消息 */
    if(CPSS_COM_LOGIC_ADDR_SAME(stLogAddr,g_pstComData->stAddrThis.stLogiAddr))
    {
        /* 构造消息包文 */
        pstTrustHdr = cpss_com_packet_setup_local(pstMsg);
        if(NULL == pstTrustHdr)
        {
	            gstInterfaceStat.ulComSendExPackSetupFailNum = gstInterfaceStat.ulComSendExPackSetupFailNum + 1;
            return CPSS_ERR_COM_PACKET_SETUP_FAIL;
        }

        /* CPU内消息分发 */
        lRet = cpss_com_ipc_msg_send(pstTrustHdr, IPC_MSG_TYPE_LOCAL);
        if(CPSS_OK != lRet)
        {
            cpss_com_mem_free (pstTrustHdr);
            gstInterfaceStat.ulComSendExIpcSendFailNum = gstInterfaceStat.ulComSendExIpcSendFailNum + 1;
            return lRet;
        }
        gstInterfaceStat.ulComSendExSuccNum = gstInterfaceStat.ulComSendExSuccNum + 1;
        return CPSS_OK;
    }
#ifdef CPSS_SLAVE_CPU
    else
    {
        return cpss_com_pci_write_slave(&(pstMsg->stDstProc),&(pstMsg->stSrcProc),pstMsg->ulMsgId,pstMsg->pucBuf,pstMsg->ulLen);
    }
#endif

    /* 如果目的逻辑地址是特殊地址(TCP)链路 */
    if(CPSS_TCP_LOGIC_ADDR_IF(stLogAddr.ucModule))
    {
        lRet = cpss_com_tcp_send(pstMsg);
        if(CPSS_OK != lRet)
        {
            gstInterfaceStat.ulComSendExTcpSendFailNum = gstInterfaceStat.ulComSendExTcpSendFailNum + 1;
            return lRet;
        }
        gstInterfaceStat.ulComSendExSuccNum = gstInterfaceStat.ulComSendExSuccNum + 1;
        return CPSS_OK ;
    }

    {
        ulIdx = (CPSS_COM_ADDRFLAG_MASTER == pstDstPid->ulAddrFlag)?0:1;
        pstRoute = cpss_com_route_find(pstDstPid->stLogicAddr);
#ifndef CPSS_PCI_SIM_BY_UDP
        if(cpss_com_link_if_pci_link(stLogAddr))
        {
            ulIdx = 0 ;
        }
        stLogAddr.ucSubGroup = 1 ;
#endif

        if((NULL == pstRoute) ||
            (CPSS_COM_PHY_ADDR_ZERO(pstRoute->stPhyAddr[ulIdx])))
        {
            /* 如果本板是主控板,说明目的单板还没有建立连接,返回失败 */
            if(cpss_local_funcbrd_type_is_mc())
            {
                gstInterfaceStat.ulComSendExDesNotExistFailNum = gstInterfaceStat.ulComSendExDesNotExistFailNum + 1;
                return CPSS_ERR_COM_ROUTE_NOT_EXIST;
            }
            else  /*如果是外围板*/
            {
                /*如果是去主控板的路由没有找到，直接返回失败*/
                if(CPSS_COM_LOGIC_ADDR_SAME(stLogAddr,g_pstComData->stAddrGcpaM.stLogiAddr))
                {
                    gstInterfaceStat.ulComSendExRouteNotExistFailNum = gstInterfaceStat.ulComSendExRouteNotExistFailNum + 1;
                    return CPSS_ERR_COM_ROUTE_NOT_EXIST ;
                }
#ifndef CPSS_PCI_SIM_BY_UDP
                /*如果去从CPU的路由没有建立，直接返回失败*/
                if(cpss_com_link_if_pci_link(pstDstPid->stLogicAddr))
                {
                    gstInterfaceStat.ulComSendExPciLinkFailNum = gstInterfaceStat.ulComSendExPciLinkFailNum + 1;
                    return CPSS_ERR_COM_PCI_NOT_FINDE ;
                }
#endif
                /*如果是可靠数据,则将数据发送到slid_proc纤程缓存*/
                if(CPSS_COM_ACK == pstMsg->ucAckFlag)
                {
                    /*发送到发送缓冲区存储*/
                    lRet = cpss_com_send_to_store(pstMsg,stLogAddr,ulAddrFlag);
                }

                /*如果已经处理过ARP消息则返回成功*/
                if(cpss_com_route_arp_flag_get(stLogAddr,ulAddrFlag) == CPSS_ARP_REQ_PROCESSED)
                {
                    return CPSS_OK ;
                }

                cpss_com_app_arp_req_send(stLogAddr,ulAddrFlag);

                if(CPSS_OK != lRet)
                {
                    gstInterfaceStat.ulComSendExSendStoreFailNum = gstInterfaceStat.ulComSendExSendStoreFailNum + 1;
                }
                else
                {
                   gstInterfaceStat.ulComSendExSuccNum = gstInterfaceStat.ulComSendExSuccNum + 1;
                }
                return lRet ;
            }
        }

        /* 查找物理地址对应的链路 */
        pstLink=cpss_com_link_find(pstRoute->stPhyAddr[ulIdx]);

        /* 发送CPU间消息 */
        lRet = cpss_com_remote_msg_send(pstMsg, pstLink);
        if(CPSS_OK != lRet)
        {
            gstInterfaceStat.ulComSendExRemoteSendFailNum = gstInterfaceStat.ulComSendExRemoteSendFailNum + 1;
        }
        else
        {
            gstInterfaceStat.ulComSendExSuccNum = gstInterfaceStat.ulComSendExSuccNum + 1;
        }
        return lRet;
}
    gstInterfaceStat.ulComSendExSuccNum = gstInterfaceStat.ulComSendExSuccNum + 1;
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_send_mate
* 功    能: 主备板纤程间消息发送，只允许在纤程中调用。
*
* 函数类型:
* 参    数:
* 参数名称        类型                输入/输出         描述
* pstDstPid   CPSS_COM_MSG_HEAD_T*     输入            消息头结构指针
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 发送标志默认为可靠、普通优先级。
*           消息体的最大长度：2M BYTE。
*******************************************************************************/
INT32 cpss_com_send_mate_extend
(
CPSS_COM_PID_T *pstSrcPid,
UINT32 ulDstPd,
UINT32 ulMsgId,
UINT8 *pucData,
UINT32 ulDataLen
)
{
    INT32 lRet;
    UINT32 ulMateAddrFlag ;
    CPSS_COM_PHY_ADDR_T stMatePhyAddr ;
    CPSS_COM_LOGIC_ADDR_T stMateLogAddr ;
    CPSS_COM_LINK_T* pstLink ;
    CPSS_COM_MSG_HEAD_T stComHdr ;

    /* 检查函数入参有效性 */
    if((ulDataLen>0) && (NULL == pucData))
    {
        return CPSS_ERR_COM_PARA_ILLEGAL;
    }

    /* 如果备板不存在, 返回失败*/
    lRet = cpss_com_local_has_backup();
    if(CPSS_OK != lRet)
    {
        return CPSS_ERR_COM_BAKBRD_NOT_EXIST;
    }

    stMatePhyAddr = g_pstComData->stMatePhyAddr ;

    pstLink = cpss_com_link_find(stMatePhyAddr) ;
    if((pstLink == NULL)||(pstLink->usStatus != CPSS_COM_LINK_STATUS_NORMAL))
    {
    /* 以物理地址方式, 发送消息 */
        lRet = cpss_com_send_phy_extend(stMatePhyAddr,ulDstPd,ulMsgId,pucData,ulDataLen,CPSS_COM_SEND_MATE_MAX_SIZE);
    }
    else
    {
        /*得到伙伴板的逻辑地址、主备状态*/
        stMateLogAddr  = g_pstComData->stAddrThis.stLogiAddr ;
        ulMateAddrFlag = !(g_pstComData->stAddrThis.ulAddrFlag) ;

        /*填写发送参数*/
        stComHdr.stDstProc.stLogicAddr = stMateLogAddr ;
        stComHdr.stDstProc.ulAddrFlag  = ulMateAddrFlag ;
        stComHdr.stDstProc.ulPd        = ulDstPd ;

        cpss_mem_memcpy(&stComHdr.stSrcProc,pstSrcPid,sizeof(CPSS_COM_PID_T));

        stComHdr.ucAckFlag             = CPSS_COM_ACK ;
        stComHdr.ucPriFlag             = CPSS_COM_PRIORITY_NORMAL ;
        stComHdr.ucShareFlag           = CPSS_COM_MEM_NOT_SHARE ;
        stComHdr.ulLen                 = ulDataLen ;
        stComHdr.ulMsgId               = ulMsgId ;
        stComHdr.pucBuf                = pucData ;

        /*消息跟踪*/
        cpss_com_trace(&stComHdr) ;

        /*发送数据*/
        lRet = cpss_com_slid_send(pucData,ulDataLen,&stComHdr,&(pstLink->stSlidWin));

        /*增加流量统计*/
        if(CPSS_OK == lRet)
        {
            pstLink->stVolumeStat.ulSendPkgNum++ ;
            pstLink->stVolumeStat.ulSendBytes += ulDataLen ;
            pstLink->stVolumeStat.ulSendBytesCur += ulDataLen ;
        }
    }

    return lRet;
}

/*******************************************************************************
* 函数名称: cpss_com_send_mate
* 功    能: 主备板纤程间消息发送，只允许在纤程中调用。
*
* 函数类型:
* 参    数:
* 参数名称        类型                输入/输出         描述
* pstDstPid   CPSS_COM_MSG_HEAD_T*     输入            消息头结构指针
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 发送标志默认为可靠、普通优先级。
*           消息体的最大长度：2M BYTE。
*******************************************************************************/
INT32 cpss_com_send_mate
(
UINT32 ulDstPd,
UINT32 ulMsgId,
UINT8 *pucData,
UINT32 ulDataLen
)
{
    INT32 lRet;
    CPSS_COM_PID_T stSrcPid;

    stSrcPid.stLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
    stSrcPid.ulAddrFlag   = g_pstComData->stAddrThis.ulAddrFlag;
    stSrcPid.ulPd           = cpss_vk_pd_self_get();

    lRet = cpss_com_send_mate_extend(&stSrcPid, ulDstPd, ulMsgId, pucData, ulDataLen);
    if(CPSS_OK != lRet)
    {
        gstInterfaceStat.ulSendMateFailNum = gstInterfaceStat.ulSendMateFailNum + 1;
    }
    else
    {
        gstInterfaceStat.ulSendMateSuccNum = gstInterfaceStat.ulSendMateSuccNum + 1;
    }
    return lRet;

}

/*******************************************************************************
* 函数名称: cpss_com_send_phy_extend
* 功    能: 使用物理地址发送，跟链路无关，直接使用UDP发送，无应答。
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* stDstPhyAddr  CPSS_COM_PHY_ADDR_T*  输入            目标物理地址
* ulDstPd       UINT32                输入          目标纤程的描述符
* ulMsgId       UINT32                输入          消息ID
* pucData       UINT8 *               输入          消息体指针
* ulDataLen     UINT32                输入          消息体长度*
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:
*******************************************************************************/
INT32 cpss_com_send_phy_extend
(
 CPSS_COM_PHY_ADDR_T  stDstPhyAddr,
 UINT32 ulDstPd,
 UINT32 ulMsgId,
 UINT8 *pucData,
 UINT32 ulDataLen,
 UINT32 ulSendLenMax
)
{
    CPSS_COM_TRUST_HEAD_T *pstTrustHdr;
    CPSS_COM_MSG_HEAD_T   stMsg;
    CPSS_COM_MSG_HEAD_T   *pstMsg;
    INT32 lRet;

    /* 检查函数入参有效性 */
    if(((ulDataLen>0) && (NULL==pucData)) || (ulDataLen>ulSendLenMax))
    {
        gstInterfaceStat.ulSendPhyParaNullFailNum = gstInterfaceStat.ulSendPhyParaNullFailNum + 1;
        return CPSS_ERR_COM_PARA_ILLEGAL;
    }

    /* 填写消息结构体 */
    stMsg.ulMsgId        = ulMsgId;
    stMsg.pucBuf         = pucData;
    stMsg.ulLen          = ulDataLen;
    stMsg.stDstProc.ulPd = ulDstPd;
    stMsg.usReserved = 0;
	
    /*设置消息优先级为高优先级*/
    stMsg.ucPriFlag = CPSS_COM_PRIORITY_NORMAL ;

    stMsg.ucShareFlag = 0 ;
    stMsg.ucAckFlag = CPSS_COM_NOT_ACK ;
    stMsg.stSrcProc.stLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
    stMsg.stSrcProc.ulAddrFlag  = g_pstComData->stAddrThis.ulAddrFlag;
    if(CPSS_DBG_DIAGN_MSGID == ulMsgId)
    {
    	stMsg.stSrcProc.ulPd  = CPSS_DBG_MNGR_PROC;
    }
    else
    {
        stMsg.stSrcProc.ulPd = cpss_vk_pd_self_get();
    }

    /* 构造消息报文格式 */
    pstTrustHdr = cpss_com_packet_setup(&stMsg,NULL);
    if(NULL == pstTrustHdr)
    {
        gstInterfaceStat.ulSendPhyPacketSetupFailNum = gstInterfaceStat.ulSendPhyPacketSetupFailNum + 1;
        return CPSS_ERR_COM_PACKET_SETUP_FAIL;
    }

    pstMsg = (CPSS_COM_MSG_HEAD_T *)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr);

    pstMsg->stDstProc.ulAddrFlag  =  CPSS_COM_ADDRFLAG_PHYSICAL;
    pstMsg->stDstProc.stLogicAddr = *(CPSS_COM_LOGIC_ADDR_T*)&stDstPhyAddr;

    pstMsg->stSrcProc.ulAddrFlag  =  CPSS_COM_ADDRFLAG_PHYSICAL;
    cpss_com_phy_addr_get((CPSS_COM_PHY_ADDR_T*)&pstMsg->stSrcProc.stLogicAddr);

    /*进行消息跟踪*/
    cpss_com_trace(pstMsg) ;

    /* 如果是CPU内消息,则调用二次调度进行消息分发 */
    if(CPSS_COM_PHY_ADDR_SAME(stDstPhyAddr, g_pstComData->stAddrThis.stPhyAddr))
    {
        /* CPU内消息发送 */
        lRet = cpss_com_ipc_msg_send(pstTrustHdr,IPC_MSG_TYPE_LOCAL);
        if(CPSS_OK != lRet)
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "cpss_com_send_phy, inside msg failed!");
            cpss_com_mem_free (pstTrustHdr);
        }
    }
    else/* 如果是CPU间消息,则调用CPSS驱动进行发送 */
    {
        UINT32 ulTotalLen;
        UINT32 ulWriteLen;
        UINT32 ulDrvType ;
        UINT32 ulIpAddr ;
        CPSS_COM_LINK_T *pstLink;
        CPSS_COM_LINK_T  stLink ;
        CPSS_COM_DRV_T*  pstDrv ;

        /* 计算消息长度 */
        ulTotalLen=CPSS_TRUST_HDR_TO_DATAEND_LEN_GET(stMsg.ulLen);

        pstLink = cpss_com_link_find(stDstPhyAddr);
        if(pstLink == NULL)
        {
            ulDrvType = cpss_com_get_drvtype_by_phyaddr(stDstPhyAddr);

            if(ulDrvType == CPSS_COM_DRV_UDP)
            {
                pstDrv=cpss_com_drv_find(CPSS_COM_DRV_UDP);
                cpss_mem_memcpy(&stLink.stDrv,pstDrv,sizeof(CPSS_COM_DRV_T));

                /*判断目的地址是否是伙伴板*/
                if(TRUE == cpss_com_mate_phyaddr(stDstPhyAddr))
                {
                    cpss_com_phy2ip(stDstPhyAddr,&ulIpAddr,TRUE) ;
                }
                else
                {
                    cpss_com_phy2ip(stDstPhyAddr,&ulIpAddr,FALSE) ;
                }

                stLink.stDrv.stSocketAddr.sin_addr.s_addr= ulIpAddr;
                pstLink = &stLink ;
            }
#if 0
#ifdef CPSS_PCI_INCLUDE
            else if(ulDrvType == CPSS_COM_DRV_PCI)
            {
                /*找到对应的链路，直接进行发送*/
                pstDrv = cpss_com_drv_find(CPSS_COM_DRV_PCI) ;
                cpss_mem_memcpy(&stLink.stDrv,pstDrv,sizeof(CPSS_COM_DRV_T));
                cpss_com_drv_pci_linkid_get(stDstPhyAddr.ucCpu,&stLink.ulLinkId) ;
            }
#endif
#endif
            else
            {
                cpss_com_mem_free(pstTrustHdr);
                gstInterfaceStat.ulSendPhyDrvNotFindFailNum = gstInterfaceStat.ulSendPhyDrvNotFindFailNum + 1;
                return CPSS_ERR_COM_DRV_NOT_FIND ;
            }
        }

        /* 转换成网络字节序 */
        cpss_com_hdr_hton(pstTrustHdr);
        ulWriteLen = 0;
		
        /* 使用驱动方式发送消息 */
        lRet = cpss_com_drv_write((VOID*)pstLink,(INT8*)pstTrustHdr,
                                  ulTotalLen,&ulWriteLen);
        cpss_com_mem_free (pstTrustHdr);
    }
    if(CPSS_OK != lRet)
    {
        gstInterfaceStat.ulSendPhyDrvWriteFailNum = gstInterfaceStat.ulSendPhyDrvWriteFailNum + 1;
    }
    else
    {
        gstInterfaceStat.ulSendPhySuccNum = gstInterfaceStat.ulSendPhySuccNum + 1;
    }
    return lRet;
}

/*******************************************************************************
* 函数名称: cpss_com_send_phy
* 功    能: 使用物理地址发送，跟链路无关，直接使用UDP发送，无应答。
*
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* stDstPhyAddr  CPSS_COM_PHY_ADDR_T*  输入            目标物理地址
* ulDstPd       UINT32                输入          目标纤程的描述符
* ulMsgId       UINT32                输入          消息ID
* pucData       UINT8 *               输入          消息体指针
* ulDataLen     UINT32                输入          消息体长度*
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 本函数主要由SMSS子系统使用，用于全局板和外围板BOOT软件进行通讯。
*******************************************************************************/
INT32 cpss_com_send_phy
(
 CPSS_COM_PHY_ADDR_T  stDstPhyAddr,
 UINT32 ulDstPd,
 UINT32 ulMsgId,
 UINT8 *pucData,
 UINT32 ulDataLen
 )
{
    INT32 lRet;

    lRet = cpss_com_send_phy_extend(stDstPhyAddr,ulDstPd,
        ulMsgId,pucData,ulDataLen,CPSS_COM_SEND_PHY_MAX_SIZE) ;

    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_send_broadcast
* 功    能: 向本CPU内所有纤程广播消息
*
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* ulMsgId       UINT32                输入          消息ID
* pucData       UINT8 *               输入          消息体指针
* ulDataLen     UINT32                输入          消息体长度*
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:
*******************************************************************************/
INT32 cpss_com_send_broadcast
(
 UINT32 ulMsgId,
 UINT8 *pucData,
 UINT32 ulDataLen
 )
{
	UINT32 ulProcNum = 0;
	CPSS_COM_PID_T stDstPid;
    /* 检查函数入参有效性 */
    if((ulDataLen>0) && (NULL == pucData))
    {
        return CPSS_ERROR;
    }
	cpss_com_logic_addr_get(&stDstPid.stLogicAddr, &stDstPid.ulAddrFlag);

    while(g_astSmssProcTable[ulProcNum].ulProcDes != CPSS_VK_PD_INVALID)
    {
    	stDstPid.ulPd = g_astSmssProcTable[ulProcNum].ulProcDes;
    	cpss_com_send(&stDstPid, ulMsgId, pucData, ulDataLen);
    	ulProcNum ++;
    }
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_com_send_forward
* 功    能: 消息转发
*
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* pstMsg      CPSS_COM_MSG_HEAD_T*    输入          消息内容
* pstDstPid   CPSS_COM_PID_T *        输入          next hop逻辑地址
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 只转发CPU间消息
*******************************************************************************/
INT32 cpss_com_send_forward
(
 CPSS_COM_MSG_HEAD_T *pstMsg,
 CPSS_COM_PID_T *pstDstPid
 )
{
    UINT32 ulIdx;
    INT32  lRet;
    CPSS_COM_LINK_T* pstLink;
    CPSS_COM_ROUTE_T* pstRoute;
    CPSS_COM_TRUST_HEAD_T* pstTrustHdr;

    /* 检查函数入参有效性 */
    if((NULL == pstMsg) || (NULL == pstDstPid))
    {
        gstInterfaceStat.ulSendForwParamNullFailNum = gstInterfaceStat.ulSendForwParamNullFailNum + 1;
        return CPSS_ERR_COM_PARA_ILLEGAL;
    }

    /* 如果是CPU内消息，则返回失败 */
    lRet = cpss_com_is_local_address(pstDstPid);
    if(TRUE == lRet)
    {
        /* 构造消息包文 */
        pstTrustHdr = cpss_com_packet_setup(pstMsg,NULL);
        if(NULL == pstTrustHdr)
        {
            gstInterfaceStat.ulSendForwPacketSetuplFailNum = gstInterfaceStat.ulSendForwPacketSetuplFailNum + 1;
            return CPSS_ERR_COM_PACKET_SETUP_FAIL;
        }

        /*增加统计*/
        cpss_com_spec_msg_stat(pstMsg->ulMsgId,IPC_RECV_PHASE) ;

        /* CPU内消息分发 */
        lRet = cpss_com_ipc_msg_send(pstTrustHdr, IPC_MSG_TYPE_LOCAL);
        if(CPSS_OK != lRet)
        {
            /*增加统计*/
            cpss_com_spec_msg_stat(pstMsg->ulMsgId,IPC_RECV_FAIL_PHASE) ;

            if(pstMsg->ucShareFlag != CPSS_COM_MEM_SHARE)
            {
                cpss_com_mem_free (pstTrustHdr);
            }
            gstInterfaceStat.ulSendForwIpcSendlFailNum = gstInterfaceStat.ulSendForwIpcSendlFailNum + 1;
            return lRet;
        }

        return CPSS_OK;
    }

    /*如果不是TCP链路*/
    lRet = CPSS_TCP_LOGIC_ADDR_IF(pstDstPid->stLogicAddr.ucModule);
    if(FALSE==lRet)
    {
        /*如果是物理地址，则调用物理发送接口发送数据*/
        if(pstDstPid->ulAddrFlag == CPSS_COM_ADDRFLAG_PHYSICAL)
        {
            lRet = cpss_com_send_phy(*(CPSS_COM_PHY_ADDR_T*)&pstDstPid->stLogicAddr,
                pstDstPid->ulPd,pstMsg->ulMsgId,pstMsg->pucBuf,pstMsg->ulLen) ;
            if(CPSS_OK != lRet)
            {
                gstInterfaceStat.ulSendForwPhySendlFailNum = gstInterfaceStat.ulSendForwPhySendlFailNum + 1;
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,"transit data failed.destaddr=%x,procid=%x,msgid=%x\n",
                    (UINT32*)&pstDstPid->stLogicAddr,pstDstPid->ulPd,pstMsg->ulMsgId) ;
                return lRet ;
            }
            /*发送成功则释放内存*/
            cpss_com_mem_free(CPSS_DISP_HDR_TO_TRUST_HDR(pstMsg)) ;
            return CPSS_OK ;
        }
        else
        {
            /* 如果是CPU间消息，则进行转发 */
            ulIdx=(CPSS_COM_ADDRFLAG_MASTER == pstDstPid->ulAddrFlag)?0:1;
            pstRoute=cpss_com_route_find(pstDstPid->stLogicAddr);
#ifndef CPSS_PCI_SIM_BY_UDP
            /*判断链路是否是PCI链路*/
            if(TRUE == cpss_com_link_if_pci_link(pstDstPid->stLogicAddr))
            {
                ulIdx = 0 ;
            }
#endif
            /* 如果没有找到路由,返回错误 */
            if((NULL == pstRoute) ||
                (CPSS_COM_PHY_ADDR_ZERO(pstRoute->stPhyAddr[ulIdx])))
            {
                /*如果是去主控板的路由没有找到，直接返回失败*/
                if(CPSS_COM_LOGIC_ADDR_SAME(pstDstPid->stLogicAddr,g_pstComData->stAddrGcpaM.stLogiAddr))
                {
                gstInterfaceStat.ulSendForwRouteFailNum = gstInterfaceStat.ulSendForwRouteFailNum + 1;
                return CPSS_ERR_COM_ROUTE_NOT_EXIST;
            }
#ifndef CPSS_PCI_SIM_BY_UDP
                /*如果去从CPU的路由没有建立，直接返回失败*/
                if(cpss_com_link_if_pci_link(pstDstPid->stLogicAddr))
                {
                    gstInterfaceStat.ulComSendExPciLinkFailNum = gstInterfaceStat.ulComSendExPciLinkFailNum + 1;
                    return CPSS_ERR_COM_PCI_NOT_FINDE ;
                }
#endif
                /*如果是可靠数据,则将数据发送到slid_proc纤程缓存*/
                if(CPSS_COM_ACK == pstMsg->ucAckFlag)
                {
                    /*发送到发送缓冲区存储*/
                    lRet = cpss_com_send_to_store(pstMsg,pstDstPid->stLogicAddr,pstDstPid->ulAddrFlag);
                }

                /*如果已经处理过ARP消息则返回成功*/
                if(cpss_com_route_arp_flag_get(pstDstPid->stLogicAddr,pstDstPid->ulAddrFlag) == CPSS_ARP_REQ_PROCESSED)
                {
                    return CPSS_OK ;
                }

                cpss_com_app_arp_req_send(pstDstPid->stLogicAddr,pstDstPid->ulAddrFlag);

                if(CPSS_OK != lRet)
                {
                    gstInterfaceStat.ulComSendForwardStoreFailNum = gstInterfaceStat.ulComSendForwardStoreFailNum + 1;
                }
                else
                {
                    gstInterfaceStat.ulSendForwSuccNum = gstInterfaceStat.ulSendForwSuccNum + 1;
                }
                return lRet ;
            }

            /* 查找链路 */
            pstLink=cpss_com_link_find(pstRoute->stPhyAddr[ulIdx]);

            /* 消息发送 */
            lRet = cpss_com_remote_msg_send(pstMsg, pstLink);
            return lRet;
        }
    }

    /*向LDT\OMC发送TCP数据*/
    lRet = cpss_com_tcp_send(pstMsg);
    if(CPSS_OK != lRet)
    {
        gstInterfaceStat.ulSendForwTcpSendFailNum = gstInterfaceStat.ulSendForwTcpSendFailNum + 1;
    }
    else
    {
        gstInterfaceStat.ulComSendExSuccNum = gstInterfaceStat.ulComSendExSuccNum + 1;
    }
    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_remote_msg_send
* 功    能: 发送CPU间消息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstMsg       CPSS_COM_MSG_HEAD_T*  输入         分发层消息头
* pstLink      CPSS_COM_LINK_T*           输入         链路对象
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:以逻辑地址方式发送的消息，调用该函数
******************************************************************************/
INT32 cpss_com_remote_msg_send
(
CPSS_COM_MSG_HEAD_T *pstMsg,
CPSS_COM_LINK_T *pstLink
)
{
    CPSS_COM_TRUST_HEAD_T*  pstTrustHdr;
    UINT32 ulTotalLen,ulWriteLen;
    INT32  lRet;

    /* 链路故障 */
    if(NULL == pstLink)
    {
        gstInterfaceStat.ulComRemoteMsgNullFailNum = gstInterfaceStat.ulComRemoteMsgNullFailNum + 1;
        return CPSS_ERR_COM_PARA_ILLEGAL;
    }

    if((pstMsg->ucAckFlag == CPSS_COM_ACK)&&(pstLink->stDrv.ulDrvType==CPSS_COM_DRV_UDP))
    {
        /*增加统计*/
        cpss_com_spec_msg_stat(pstMsg->ulMsgId,SEND_TO_SLID_PROC_PHASE);

        lRet = cpss_com_slid_send(pstMsg->pucBuf,pstMsg->ulLen,pstMsg,&(pstLink->stSlidWin));
        if(CPSS_OK != lRet)
        {
            /*增加统计*/
            cpss_com_spec_msg_stat(pstMsg->ulMsgId,SEND_TO_SLID_PROC_FAIL_PHASE);

            gstInterfaceStat.ulComRemoteSlidSendFailNum = gstInterfaceStat.ulComRemoteSlidSendFailNum + 1;
            return lRet ;
        }

        /*增加流量统计*/
        pstLink->stVolumeStat.ulSendPkgNum++ ;
        pstLink->stVolumeStat.ulSendBytes += pstMsg->ulLen ;
        pstLink->stVolumeStat.ulSendBytesCur += pstMsg->ulLen ;

        /*如果成功则释放内存*/
        if(CPSS_COM_MEM_SHARE == pstMsg->ucShareFlag)
        {
            cpss_com_mem_free (CPSS_DISP_HDR_TO_TRUST_HDR(pstMsg));
        }
    }
    else
    {
        ulTotalLen=CPSS_TRUST_HDR_TO_DATAEND_LEN_GET(pstMsg->ulLen);

        pstTrustHdr = cpss_com_packet_setup(pstMsg,pstLink);

        if(NULL == pstTrustHdr)
        {
            gstInterfaceStat.ulComRemotePacketSetupFailNum = gstInterfaceStat.ulComRemotePacketSetupFailNum + 1;
            return CPSS_ERR_COM_PACKET_SETUP_FAIL;
        }

        cpss_com_hdr_hton(pstTrustHdr) ;

        lRet = cpss_com_drv_write((VOID*)pstLink,(INT8*)pstTrustHdr,ulTotalLen,&ulWriteLen);

        if(CPSS_OK != lRet)
        {
            if(CPSS_COM_MEM_NOT_SHARE == pstMsg->ucShareFlag)
            {
                cpss_com_mem_free (pstTrustHdr);
            }
            gstInterfaceStat.ulComRemoteDrvWriteFailNum = gstInterfaceStat.ulComRemoteDrvWriteFailNum + 1;
            return lRet;
        }

        cpss_com_mem_free (pstTrustHdr);
    }
    gstInterfaceStat.ulComRemoteSuccNum = gstInterfaceStat.ulComRemoteSuccNum + 1;
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_local_msg_recv
* 功    能: 本CPU消息接收，处理
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstIpcHdr      IPC_MSG_HDR_T*       输入            传输层消息头
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
******************************************************************************/
INT32 cpss_com_local_msg_recv
(
CPSS_COM_TRUST_HEAD_T  *pstTrustHdr
)
{
    INT32 lRet;

    /* Added by jiangliming, 2006/4/24 */
    cpss_com_trace((CPSS_COM_MSG_HEAD_T *)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr));
    /* Add ended, 2006/4/24 */

    /*内部消息分发*/
    lRet = cpss_com_ipc_msg_send(pstTrustHdr,IPC_MSG_TYPE_REMOTE);

    if(CPSS_OK != lRet)
    {
        CPSS_COM_MSG_HEAD_T *pstMsg=(CPSS_COM_MSG_HEAD_T *)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr);
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "local message received error msgId 0x%08x src proc 0x%08x, dst proc 0x%08x srcAddr 0x%08x!",
            pstMsg->ulMsgId,pstMsg->stSrcProc.ulPd,pstMsg->stDstProc.ulPd,pstMsg->stSrcProc.stLogicAddr);
        return lRet;
    }
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_interface_send_show
* 功    能: 发送接口统计
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstIpcHdr      IPC_MSG_HDR_T*       输入            传输层消息头
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
******************************************************************************/

VOID cpss_com_interface_send_show()
{
    cps__oams_shcmd_printf("\n cpss_com_send_extend:\n");
    cps__oams_shcmd_printf("Succ:             %5d\n",gstInterfaceStat.ulComSendExSuccNum);
    cps__oams_shcmd_printf("DesNotExistFail:  %5d\n", gstInterfaceStat.ulComSendExDesNotExistFailNum);
    cps__oams_shcmd_printf("IpcSendFail:      %5d\n",gstInterfaceStat.ulComSendExIpcSendFailNum);
    cps__oams_shcmd_printf("MsgNullFail:      %5d\n",gstInterfaceStat.ulComSendExMsgNullFailNum);
    cps__oams_shcmd_printf("PackSetupFail:    %5d\n",gstInterfaceStat.ulComSendExPackSetupFailNum);
    cps__oams_shcmd_printf("PciLinkFail:      %5d\n",gstInterfaceStat.ulComSendExPciLinkFailNum);
    cps__oams_shcmd_printf("RemoteSendFail:   %5d\n",gstInterfaceStat.ulComSendExRemoteSendFailNum);
    cps__oams_shcmd_printf("RoutNotExistFail: %5d\n",gstInterfaceStat.ulComSendExRouteNotExistFailNum);
    cps__oams_shcmd_printf("SendStoreFail:    %5d\n",gstInterfaceStat.ulComSendExSendStoreFailNum);
    cps__oams_shcmd_printf("TcpSendFail:      %5d\n",gstInterfaceStat.ulComSendExTcpSendFailNum);
    cps__oams_shcmd_printf("\n cpss_com_send_local:\n");
    cps__oams_shcmd_printf("PcaketSetupFail:  %5d\n",gstInterfaceStat.ulComSendLocalPcaketSetupFailNum);
    cps__oams_shcmd_printf("IpcSendFail:      %5d\n",gstInterfaceStat.ulComSendLocalIpcSendFailNum);
    cps__oams_shcmd_printf("\n cpss_com_send_mate:\n");
    cps__oams_shcmd_printf("Succ:             %5d\n",gstInterfaceStat.ulSendMateSuccNum);
    cps__oams_shcmd_printf("MateFail:         %5d\n",gstInterfaceStat.ulSendMateFailNum);
    cps__oams_shcmd_printf("\n cpss_com_send_phy:\n");
    cps__oams_shcmd_printf("Succ:             %5d\n", gstInterfaceStat.ulSendPhySuccNum);
    cps__oams_shcmd_printf("DrvNotFindFail:   %5d\n",gstInterfaceStat.ulSendPhyDrvNotFindFailNum);
    cps__oams_shcmd_printf("DrvWriteFail:     %5d\n",gstInterfaceStat.ulSendPhyDrvWriteFailNum);
    cps__oams_shcmd_printf("PacketSetupFail:  %5d\n",gstInterfaceStat.ulSendPhyPacketSetupFailNum);
    cps__oams_shcmd_printf("ParaNullFail:     %5d\n",gstInterfaceStat.ulSendPhyParaNullFailNum);
    cps__oams_shcmd_printf("PhySendlFail:     %5d\n",gstInterfaceStat.ulSendForwPhySendlFailNum);
    cps__oams_shcmd_printf("RouteFail:        %5d\n",gstInterfaceStat.ulSendForwRouteFailNum);
    cps__oams_shcmd_printf("\n cpss_com_send_forward:\n");
    cps__oams_shcmd_printf("Succ:             %5d\n", gstInterfaceStat.ulSendForwSuccNum);
    cps__oams_shcmd_printf("IpcSendlFail:     %5d\n", gstInterfaceStat.ulSendForwIpcSendlFailNum);
    cps__oams_shcmd_printf("PacketSetuplFail: %5d\n",gstInterfaceStat.ulSendForwPacketSetuplFailNum);
    cps__oams_shcmd_printf("PacketSetuplFail: %5d\n",gstInterfaceStat.ulSendForwParamNullFailNum);
    cps__oams_shcmd_printf("ulComSendForwardStoreFailNum:%5d\n",gstInterfaceStat.ulComSendForwardStoreFailNum);
    cps__oams_shcmd_printf("\n cpss_com_remote_msg_send:\n");
    cps__oams_shcmd_printf("Succ:             %5d\n", gstInterfaceStat.ulComRemoteSuccNum);
    cps__oams_shcmd_printf("DrvWriteFail:     %5d\n",gstInterfaceStat.ulComRemoteDrvWriteFailNum);
    cps__oams_shcmd_printf("MsgNullFail:      %5d\n",gstInterfaceStat.ulComRemoteMsgNullFailNum);
    cps__oams_shcmd_printf("PacketSetupFail:  %5d\n",gstInterfaceStat.ulComRemotePacketSetupFailNum);
    cps__oams_shcmd_printf("SlidSendFail:     %5d\n",gstInterfaceStat.ulComRemoteSlidSendFailNum);
    cps__oams_shcmd_printf("ulShareMemSendNum:%5d\n",gstInterfaceStat.ulShareMemSendNum);

}

/*******************************************************************************
* 函数名称: cpss_com_send_broadcast_msg
* 功    能: 向所有的存在链路的单板发送广播消息
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
extern COM_SLID_LINK_T g_stSlidWinLink ;

INT32 cpss_com_send_broadcast_msg
(
CPSS_COM_MSG_HEAD_T *pstMsg
)
{
    INT32 lRet ;
    COM_SLID_LINK_NODE_T *pstSlidNode ;
    CPSS_COM_LINK_T* pstLink ;
    CPSS_COM_LOGIC_ADDR_T stSelfLogAddr ;
    UINT32 ulAddrFlag ;

    lRet = cpss_com_logic_addr_get(&stSelfLogAddr,&ulAddrFlag) ;
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }

    /*遍历滑窗链表*/
    pstSlidNode = g_stSlidWinLink.pstHead ;

    while(pstSlidNode!=NULL)
    {
        pstLink = (CPSS_COM_LINK_T*)pstSlidNode->ulData ;

        pstMsg->stDstProc.stLogicAddr = pstLink->stDstLogAddr ;
        pstMsg->stDstProc.ulAddrFlag  = pstLink->ulAddrFlag ;

        pstMsg->stSrcProc.stLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
        pstMsg->stSrcProc.ulAddrFlag  = g_pstComData->stAddrThis.ulAddrFlag;
        pstMsg->stSrcProc.ulPd        = cpss_vk_pd_self_get();

        /*如果是备板,使用send_mate接口*/
        if(1 == CPSS_COM_LOGIC_ADDR_SAME(pstMsg->stDstProc.stLogicAddr,stSelfLogAddr))
        {
            lRet =cpss_com_send_mate(pstMsg->stDstProc.ulPd,
                pstMsg->ulMsgId,pstMsg->pucBuf,pstMsg->ulLen) ;
        }
        else
        {
            lRet = cpss_com_send_extend(pstMsg) ;
        }
        if(CPSS_OK != lRet)
        {
            cpss_output(CPSS_MODULE_COM, CPSS_PRINT_WARN,
                "cpss_com_send_broadcast_msg, send msg logAddr=%x ,ulAddr=%d failed!",
                *(UINT32*)&pstMsg->stDstProc.stLogicAddr,pstMsg->stDstProc.ulAddrFlag);
        }

        pstSlidNode = pstSlidNode->pstNext ;
    }

    return CPSS_OK ;
}


INT32 cpss_com_spec_msg_stat_add(UINT32 ulMsgId)
{
    UINT32 ulLoop ;

    g_ulCpssSpecMsgStatFlag = CPSS_COM_SPEC_MSG_STAT_ON ;

    for(ulLoop=0;ulLoop<CPSS_COM_SPEC_MSG_NUM_MAX;ulLoop++)
    {
        if((g_astSpecMsgStat[ulLoop].ulMsgId == ulMsgId)
            &&(g_astSpecMsgStat[ulLoop].ulUsedFlag == CPSS_COM_SPEC_MSG_USED_FLAG_BUSY))
        {
            cps__oams_shcmd_printf("the special msg is busy!\n") ;

            return CPSS_OK ;
        }
    }

    for(ulLoop=0;ulLoop<CPSS_COM_SPEC_MSG_NUM_MAX;ulLoop++)
    {
        if(g_astSpecMsgStat[ulLoop].ulUsedFlag == CPSS_COM_SPEC_MSG_USED_FLAG_FREE)
        {
            cpss_mem_memset(&g_astSpecMsgStat[ulLoop],0,sizeof(CPSS_COM_SPEC_MSG_STAT_T)) ;

            g_astSpecMsgStat[ulLoop].ulMsgId = ulMsgId ;
            g_astSpecMsgStat[ulLoop].ulUsedFlag = CPSS_COM_SPEC_MSG_USED_FLAG_BUSY ;

            cps__oams_shcmd_printf("the special msg add success!\n") ;

            return CPSS_OK ;
        }
    }

    cps__oams_shcmd_printf("the special msg add failed!\n") ;

    return CPSS_ERROR ;
}


VOID cpss_com_spec_msg_stat_clear()
{
    UINT32 ulLoop ;

    g_ulCpssSpecMsgStatFlag = CPSS_COM_SPEC_MSG_STAT_OFF ;

    for(ulLoop=0;ulLoop<CPSS_COM_SPEC_MSG_NUM_MAX;ulLoop++)
    {
        cpss_mem_memset(&g_astSpecMsgStat[ulLoop],0,sizeof(CPSS_COM_SPEC_MSG_STAT_T)) ;
    }

//    cps__oams_shcmd_printf("call cpss_com_spec_msg_stat_clear succ!\n") ;
}

extern VOID cpss_com_time_print(UINT8* pucInfo,CPSS_TIME_T* pstTime) ;

VOID cpss_com_spec_msg_stat_show()
{
    UINT32 ulLoop ;

    cps__oams_shcmd_printf("\n") ;

    cps__oams_shcmd_printf("--------------------------------------------\n") ;

    for(ulLoop=0;ulLoop<CPSS_COM_SPEC_MSG_NUM_MAX;ulLoop++)
    {
        if(g_astSpecMsgStat[ulLoop].ulMsgId != 0)
        {
        cps__oams_shcmd_printf("ulMsgId=0x%x\n",g_astSpecMsgStat[ulLoop].ulMsgId) ;
        cps__oams_shcmd_printf("ulUsedFlag=%d\n",g_astSpecMsgStat[ulLoop].ulUsedFlag) ;

        cps__oams_shcmd_printf("ulComSendNum=%d\n",g_astSpecMsgStat[ulLoop].ulComSendNum) ;
        cpss_com_time_print("ComSendTime",&g_astSpecMsgStat[ulLoop].stComSendTime) ;

        cps__oams_shcmd_printf("ulDrvPciSendNum=%d\n",g_astSpecMsgStat[ulLoop].ulDrvPciSendNum) ;
        cpss_com_time_print("PciSendTime",&g_astSpecMsgStat[ulLoop].stDrvPciSendTime) ;

        cps__oams_shcmd_printf("ulSendToSlidProcNum=%d\n",g_astSpecMsgStat[ulLoop].ulSendToSlidProcNum) ;
        cpss_com_time_print("SendToSlidProcTime",&g_astSpecMsgStat[ulLoop].stSendToSlidProcTime) ;

        cps__oams_shcmd_printf("ulSendToSlidProcFailedNum=%d\n",g_astSpecMsgStat[ulLoop].ulSendToSlidProcFailedNum) ;
        cpss_com_time_print("SendToSlidProcFailTime",&g_astSpecMsgStat[ulLoop].stSendToSlidProcFailTime) ;

        cps__oams_shcmd_printf("ulDrvPciRecvNum=%d\n",g_astSpecMsgStat[ulLoop].ulDrvPciRecvNum) ;
        cpss_com_time_print("PciRecvTime",&g_astSpecMsgStat[ulLoop].stDrvPciRecvTime) ;

        cps__oams_shcmd_printf("ulSlidProcRecvNum=%d\n",g_astSpecMsgStat[ulLoop].ulSlidProcRecvNum) ;
        cpss_com_time_print("SlidProcRecvTime",&g_astSpecMsgStat[ulLoop].stSlidProcRecvTime) ;

        cps__oams_shcmd_printf("ulIpcRecvNum=%d\n",g_astSpecMsgStat[ulLoop].ulIpcRecvNum) ;
        cpss_com_time_print("IpcRecvTime",&g_astSpecMsgStat[ulLoop].stIpcRecvTime) ;

        cps__oams_shcmd_printf("ulIpcRecvFailNum=%d\n",g_astSpecMsgStat[ulLoop].ulIpcRecvFailNum) ;
        cpss_com_time_print("IpcRecvFailTime",&g_astSpecMsgStat[ulLoop].stIpcRecvFailTime) ;

        cps__oams_shcmd_printf("ulDestProcRecvNum=%d\n",g_astSpecMsgStat[ulLoop].ulDestProcRecvNum) ;
        cpss_com_time_print("DestProcRecvTime",&g_astSpecMsgStat[ulLoop].stDestProcRecvTime) ;

        cps__oams_shcmd_printf("--------------------------------------------\n") ;
    }
}
}

VOID cpss_com_spec_msg_stat(UINT32 ulMsgId,UINT32 ulPhaseFlag)
{
    UINT32 ulLoop ;

    if(g_ulCpssSpecMsgStatFlag == CPSS_COM_SPEC_MSG_STAT_OFF)
    {
        return ;
    }

    for(ulLoop=0;ulLoop<CPSS_COM_SPEC_MSG_NUM_MAX;ulLoop++)
    {
        if((g_astSpecMsgStat[ulLoop].ulMsgId == ulMsgId)
            &&(g_astSpecMsgStat[ulLoop].ulUsedFlag == CPSS_COM_SPEC_MSG_USED_FLAG_BUSY))
        {
            if(ulPhaseFlag == COM_SEND_PHASE)
            {
                g_astSpecMsgStat[ulLoop].ulComSendNum++ ;
                cpss_clock_get(&g_astSpecMsgStat[ulLoop].stComSendTime) ;
            }
            else if(ulPhaseFlag == SEND_TO_SLID_PROC_PHASE)
            {
                g_astSpecMsgStat[ulLoop].ulSendToSlidProcNum++ ;
                cpss_clock_get(&g_astSpecMsgStat[ulLoop].stSendToSlidProcTime) ;
            }
            else if(ulPhaseFlag == SEND_TO_SLID_PROC_FAIL_PHASE)
            {
                g_astSpecMsgStat[ulLoop].ulSendToSlidProcFailedNum++ ;
                cpss_clock_get(&g_astSpecMsgStat[ulLoop].stSendToSlidProcFailTime) ;
            }
            else if(ulPhaseFlag == DRV_PCI_SEND_PHASE)
            {
                g_astSpecMsgStat[ulLoop].ulDrvPciSendNum++ ;
                cpss_clock_get(&g_astSpecMsgStat[ulLoop].stDrvPciSendTime) ;
            }
            else if(ulPhaseFlag == DRV_PCI_RECV_PHASE)
            {
                g_astSpecMsgStat[ulLoop].ulDrvPciRecvNum++ ;
                cpss_clock_get(&g_astSpecMsgStat[ulLoop].stDrvPciRecvTime) ;
            }
            else if(ulPhaseFlag == SLID_PROC_RECV_PHASE)
            {
                g_astSpecMsgStat[ulLoop].ulSlidProcRecvNum++ ;
                cpss_clock_get(&g_astSpecMsgStat[ulLoop].stSlidProcRecvTime) ;
            }
            else if(ulPhaseFlag == IPC_RECV_PHASE)
            {
                g_astSpecMsgStat[ulLoop].ulIpcRecvNum++ ;
                cpss_clock_get(&g_astSpecMsgStat[ulLoop].stIpcRecvTime) ;
            }
            else if(ulPhaseFlag == IPC_RECV_FAIL_PHASE)
            {
                g_astSpecMsgStat[ulLoop].ulIpcRecvFailNum++ ;
                cpss_clock_get(&g_astSpecMsgStat[ulLoop].stIpcRecvFailTime) ;
            }
            else if(ulPhaseFlag == DEST_PROC_RECV_PHASE)
            {
                g_astSpecMsgStat[ulLoop].ulDestProcRecvNum++ ;
                cpss_clock_get(&g_astSpecMsgStat[ulLoop].stDestProcRecvTime) ;
            }
            else if(ulPhaseFlag == DRV_PCI_SEND_FAIL_PHASE)
            {
                g_astSpecMsgStat[ulLoop].ulDrvPciSendFailNum++ ;
                cpss_clock_get(&g_astSpecMsgStat[ulLoop].stDrvPciSendFailTime) ;
            }

            return ;
        }
    }
}

/*******************************************************************************
* 函数名称: cpss_vos_iproute_add
* 功    能: 添加静态路由
*
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* pcDest        UINT8*                输入          目的ip
* pcGateway     UINT8*                输入          网关ip
* ulMask        UINT32                输入          子网掩码
× ulTos         UINT32                输入          服务类型保留
× ulFlag        UINT32                输入          路由标识保留
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:
*******************************************************************************/
INT32 cpss_vos_iproute_add
(
 UINT8 *pcDest,
 UINT8 *pcGateway,
 UINT32 ulMask,
 UINT32 ulTos,
 UINT32 ulFlag
 )
{
    /* 检查函数入参有效性 */
    if((NULL == pcDest) || (NULL == pcGateway))
    {
        return CPSS_ERROR;
    }
    #ifdef CPSS_VOS_VXWORKS
    return (mRouteAdd(pcDest,pcGateway,ulMask,0,0));
    #else
    return CPSS_OK;
    #endif
}

/*******************************************************************************
* 函数名称: cpss_vos_iproute_delete
* 功    能: 删除静态路由
*
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* pcDest        UINT8*                输入          目的ip
* ulMask        UINT32                输入          子网掩码
× ulTos         UINT32                输入          服务类型保留
× ulFlag        UINT32                输入          路由标识保留
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:
*******************************************************************************/
INT32 cpss_vos_iproute_delete
(
 UINT8 *pcDest,
 UINT32 ulMask,
 UINT32 ulTos,
 UINT32 ulFlag
 )
{
    /* 检查函数入参有效性 */
    if(NULL == pcDest)
    {
        return CPSS_ERROR;
    }
    #ifdef CPSS_VOS_VXWORKS
    return (mRouteDelete(pcDest,ulMask,0,0));
    #else
    return CPSS_OK;
    #endif
}

/******************************* 源文件结束 **********************************/
void cps_com_boot_msg_init()
{
	g_BootMsgId = msgget(BOOT_KEY, IPC_CREAT);
	if(-1 == g_BootMsgId)
	{
		cpss_message("boot_msgget init fail!\n");
}
	else
{
		cpss_message("boot_msgget init ok! msgid = %d\n", g_BootMsgId);
	}
	cpss_vos_task_spawn("BootMsgDealing", 1, 0, 0, cps_com_boot_msg_dealing, 0);
	return;
}

void cps_com_boot_msg_dealing()
{
	CPS_REBOOT_T stRbt;
	BOOT_MSG_T stBootMsg;
	
	cpss_message("in cps_com_boot_msg_dealing\n");
	while(1)
	{
		cps_com_boot_rcv_msg((char*)&stBootMsg);
		switch(stBootMsg.ulMsgId)
		{
			case CPS_BOOT_DOWNLOAD_FAIL_ALARM:
				stRbt.ucRebtType = CPS_CPU_REBOOT_HOT;
				stRbt.ucRebtRsn = CPS_REBOOT_BY_VER_DOWNLOAD;
				stRbt.szExtra = "boot download error";

				cpss_message("receive boot download error alarm!\n");

				cps_boot_reboot(&stRbt);
				break;
			default:
				break;
		}
	}
}

void cps_com_boot_snd_msg(char* stBootMsg)
{
	BOOT_IPC_MSG_T stIpcMsg = {0};

	if(NULL == stBootMsg)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_COM, CPSS_PRINT_WARN, "stBootMsg is NULL in cps_com_boot_snd_msg, return!\n");
		return;
	}

	stIpcMsg.lMsgType = 1;
	memcpy(stIpcMsg.cIpcMsg, stBootMsg, sizeof(BOOT_MSG_T));
	msgsnd(g_BootMsgId, &stIpcMsg, 132, IPC_NOWAIT);
	return;
}

void cps_com_boot_rcv_msg(char* stBootMsg)
{
	BOOT_IPC_MSG_T stIpcMsg = {0};

	if(NULL == stBootMsg)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_COM, CPSS_PRINT_WARN, "stBootMsg is NULL in cps_com_boot_rcv_msg, return!\n");
		return;
	}

	if(-1 == msgrcv(g_BootMsgId, &stIpcMsg, 132, 2, 0))
	{
		perror("msgrcv");
		return;
	}

	memcpy(stBootMsg, stIpcMsg.cIpcMsg, sizeof(BOOT_MSG_T));
	return;
}
