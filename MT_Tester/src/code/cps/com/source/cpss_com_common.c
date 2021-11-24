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
#include "cpss_vk_proc.h"
#include "cpss_com_common.h"
#include "cpss_com_link.h"
#include "cpss_tm_timer.h"
#include "cpss_err.h"

/******************************* 局部宏定义 **********************************/
/* WINDOWS环境下只需要使用一个高8位类型就可以了。控制面、主备通道使用同一个。
 * 放在此处得原因是修改后不需要整个工程全部重新编译，只需编译该文件即可。如果
 * 需要修改，正常途径是在编译宏定义中增加SWP_IP_HIGH8=XX就会覆盖该缺省值。
 */

#ifdef CPSS_VOS_WINDOWS

UINT8 g_ucCpssIpHighByte = 11 ;

#ifndef SWP_IP_HIGH8
#define SWP_IP_HIGH8 (g_ucCpssIpHighByte)
#endif

#endif

#ifdef CPSS_VOS_LINUX

UINT8 g_ucCpssIpHighByte = 11 ;

#ifndef SWP_IP_HIGH8
#define SWP_IP_HIGH8 (g_ucCpssIpHighByte)
#endif

#endif

/******************************* 全局变量定义/初始化 *************************/

/******************************* 局部常数和类型定义 **************************/

/******************************* 局部函数原型声明 ****************************/

/******************************* 函数实现 ************************************/


/*******************************************************************************
* 函数名称: cpss_com_pid_ntoh                            
* 功    能: 纤程标识,网络字节序转换成主机字节序
* 相关文档: 
* 函数类型:    
*                                
* 参    数:     
* 参数名称            类型            输入/输出         描述
* pstPid       CPSS_COM_PID_T*      输入、输出      纤程标识
* 返回值: 
*        无
* 说   明:
*                                                 
******************************************************************************/
VOID cpss_com_pid_ntoh
(
 CPSS_COM_PID_T *pstPid
 )
{
    pstPid->ulAddrFlag          = cpss_ntohl(pstPid->ulAddrFlag);
    pstPid->ulPd                = cpss_ntohl(pstPid->ulPd);
    
    if(pstPid->ulAddrFlag != CPSS_COM_ADDRFLAG_PHYSICAL)
    {
        pstPid->stLogicAddr.usGroup = cpss_ntohs(pstPid->stLogicAddr.usGroup);
    }
}    

/*******************************************************************************
* 函数名称: cpss_com_pid_hton                            
* 功    能: 纤程标识,网络字节序转换成主机字节序
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* pstPid     CPSS_COM_PID_T*        输入、输出      纤程标识
* 返回值: 
*         无
* 说   明:
*                                                 
******************************************************************************/
VOID cpss_com_pid_hton
(
 CPSS_COM_PID_T *pstPid
 )
{
    if(pstPid->ulAddrFlag != CPSS_COM_ADDRFLAG_PHYSICAL)
    {
        pstPid->stLogicAddr.usGroup = cpss_htons(pstPid->stLogicAddr.usGroup);
    }
    
    pstPid->ulAddrFlag          = cpss_htonl(pstPid->ulAddrFlag);
    pstPid->ulPd                = cpss_htonl(pstPid->ulPd);
}    

/*******************************************************************************
* 函数名称: cpss_com_comm_hdr_hton                            
* 功    能: 将数据分发头转化为网络字节序
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* pstPid     CPSS_COM_PID_T*        输入、输出      纤程标识
* 返回值: 
*         无
* 说   明:
*                                                 
******************************************************************************/
VOID cpss_com_comm_hdr_hton
(
CPSS_COM_MSG_HEAD_T *pstMsg
)
{
    pstMsg->ulLen = cpss_htonl(pstMsg->ulLen) ;
    pstMsg->ulMsgId = cpss_htonl(pstMsg->ulMsgId) ;
    cpss_com_pid_hton(&pstMsg->stDstProc) ;
    cpss_com_pid_hton(&pstMsg->stSrcProc) ;    
}

/*******************************************************************************
* 函数名称: cpss_com_comm_hdr_ntoh                            
* 功    能: 将数据分发头转化为主机字节序
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* pstPid     CPSS_COM_PID_T*        输入、输出      纤程标识
* 返回值: 
*         无
* 说   明:
*                                                 
******************************************************************************/
VOID cpss_com_comm_hdr_ntoh
(
CPSS_COM_MSG_HEAD_T *pstMsg
)
{
    pstMsg->ulLen = cpss_ntohl(pstMsg->ulLen) ;
    pstMsg->ulMsgId = cpss_ntohl(pstMsg->ulMsgId) ;
    cpss_com_pid_ntoh(&pstMsg->stDstProc) ;
    cpss_com_pid_ntoh(&pstMsg->stSrcProc) ;    
}


/*******************************************************************************
* 函数名称: cpss_com_ipc_hdr_ntoh                            
* 功    能: 消息头部报文,网络字节转化成主机字节顺序
* 相关文档: 
* 函数类型:    
*                                
* 参    数: 
* 参数名称            类型            输入/输出         描述
* pstHdr       IPC_MSG_HDR_T*         输入、输出      消息传输头部
* 返回值: 
*        无
* 说   明:
*                                                 
******************************************************************************/
VOID cpss_com_hdr_ntoh
(
CPSS_COM_TRUST_HEAD_T *pstHdr
)
{
    CPSS_COM_MSG_HEAD_T *pstMsg;
    
    /* 判断参数的有效性 */
    if(NULL==pstHdr)
    {
        return;
    }
    
    /* 处理字节序,传输层 */
    pstHdr->usSliceLen = cpss_ntohs(pstHdr->usSliceLen);
    pstHdr->usSeq      = cpss_ntohs(pstHdr->usSeq);
    pstHdr->usAck      = cpss_ntohs(pstHdr->usAck);
    pstHdr->usFlowCtrl = cpss_ntohs(pstHdr->usFlowCtrl);
    pstHdr->usCheckSum = cpss_ntohs(pstHdr->usCheckSum);
    
    /* 处理字节序,分发层 */
    pstMsg=(CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstHdr);
    
    cpss_com_pid_ntoh(&pstMsg->stDstProc);
    cpss_com_pid_ntoh(&pstMsg->stSrcProc);    
    
    pstMsg->ulMsgId = cpss_ntohl(pstMsg->ulMsgId);
    pstMsg->ulLen   = cpss_ntohl(pstMsg->ulLen);
    
}

/*******************************************************************************
* 函数名称: cpss_com_ipc_hdr_hton                            
* 功    能: 消息头部报文 主机字节到网络字节顺序转化
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* pstHdr       IPC_MSG_HDR_T*         输入、输出      消息传输头部
* 返回值:
*        无
* 说   明:
*                                                 
******************************************************************************/
VOID cpss_com_hdr_hton
(
CPSS_COM_TRUST_HEAD_T *pstHdr
)
{
    CPSS_COM_MSG_HEAD_T *pstMsg;
    
    /* 判断参数的有效性 */
    if(NULL==pstHdr)
    {
        return;
    }
    
    /* 处理字节序,传输层 */
    pstHdr->usSliceLen = cpss_htons(pstHdr->usSliceLen);
    pstHdr->usSeq      = cpss_htons(pstHdr->usSeq);
    pstHdr->usAck      = cpss_htons(pstHdr->usAck);
    pstHdr->usFlowCtrl = cpss_htons(pstHdr->usFlowCtrl);
    pstHdr->usCheckSum = cpss_htons(pstHdr->usCheckSum);
    
    /* 处理字节序,分发层 */
    pstMsg=(CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstHdr);
    
    cpss_com_pid_hton(&pstMsg->stDstProc);
    cpss_com_pid_hton(&pstMsg->stSrcProc);    
    
    pstMsg->ulMsgId = cpss_htonl(pstMsg->ulMsgId);
    pstMsg->ulLen   = cpss_htonl(pstMsg->ulLen);
}

BOOL cpss_com_phyAddr_valid(CPSS_COM_PHY_ADDR_T *pstPhyAddr)
{
    if ((pstPhyAddr->ucFrame < CPSS_COM_MAX_FRAME + 1) &&
        (pstPhyAddr->ucShelf < CPSS_COM_MAX_SHELF + 1) && 
        (pstPhyAddr->ucSlot < CPSS_COM_MAX_SLOT + 1) &&
        (pstPhyAddr->ucCpu < CPSS_COM_MAX_CPU + 1))
        {
            return (TRUE);
        }

    return (FALSE);
}

/*******************************************************************************
* 函数名称: cpss_com_phy2index                            
* 功    能: 根据物理地址得到链路表项的下标
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* stPhyAddr   CPSS_COM_PHY_ADDR_T   输入            物理地址       
* 返回值: 
*        链路表项的下标
* 说   明:
*                                                 
*******************************************************************************/
UINT32 cpss_com_phy2index(CPSS_COM_PHY_ADDR_T stPhyAddr)
{
#ifdef CPSS_HOST_CPU
    return ((stPhyAddr.ucFrame - 1)*CPSS_COM_MAX_SHELF*CPSS_COM_MAX_SLOT*CPSS_COM_MAX_CPU
        +(stPhyAddr.ucShelf - 1)*CPSS_COM_MAX_SLOT*CPSS_COM_MAX_CPU 
        +(stPhyAddr.ucSlot - 1)*CPSS_COM_MAX_CPU
        + stPhyAddr.ucCpu);
#else
    return 0 ;
#endif
}

/*******************************************************************************
* 函数名称: cpss_com_logic2index                            
* 功    能: 根据逻辑地址得到路由表项的下标
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型             输入/输出         描述
* stLogicAddr CPSS_COM_LOGIC_ADDR_T  输入           逻辑地址
* 返回值: 
*        路由表项的下标
* 说   明:
*                                                 
*******************************************************************************/
UINT32 cpss_com_logic2index(CPSS_COM_LOGIC_ADDR_T stLogicAddr)
{
#ifndef CPSS_HOST_CPU
    return 0;
#else
    UINT32 ulIdx;
    ulIdx = (UINT32)(((stLogicAddr.usGroup & 0x00000fff) >> 8) - 1)
        * CPSS_COM_MAX_SHELF * CPSS_COM_MAX_SLOT * CPSS_COM_MAX_CPU;
    ulIdx += (UINT32)((((stLogicAddr.usGroup & 0x000000ff) >> 4) - 1)
        * CPSS_COM_MAX_SLOT * CPSS_COM_MAX_CPU);
    ulIdx += (UINT32)(((stLogicAddr.usGroup & 0x0000000f) - 1)
        * CPSS_COM_MAX_CPU);
    ulIdx += (UINT32)stLogicAddr.ucSubGroup;
    return ulIdx;
#endif    
}

/*******************************************************************************
* 函数名称: cpss_com_idx2logaddr                            
* 功    能: 根据逻辑地址得到路由表项的下标
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型             输入/输出         描述
* ulIdx               UINT32           输入              ID
* pusGroup            UINT16*          输出              子组号
* 返回值: 
*        成功：CPSS_OK
*        失败：CPSS_ERROR
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_idx2logaddr(UINT32 ulIdx, UINT16 *pusGroup)
{
    CPSS_LOGIC_ADDR_GROUP_T *pstGroup = (CPSS_LOGIC_ADDR_GROUP_T*)pusGroup;

    if (ulIdx >= CPSS_COM_MAX_FRAME*CPSS_COM_MAX_SHELF*CPSS_COM_MAX_SLOT*CPSS_COM_MAX_CPU)
    {
        return (CPSS_ERROR);
    }
    
    pstGroup->btFrame = CPSS_DIV(ulIdx,(CPSS_COM_MAX_SHELF*CPSS_COM_MAX_SLOT*CPSS_COM_MAX_CPU)) + 1;
    pstGroup->btShelf = CPSS_DIV(ulIdx,(CPSS_COM_MAX_SLOT*CPSS_COM_MAX_CPU)) + 1;
    pstGroup->btSlot =  CPSS_DIV(ulIdx, CPSS_COM_MAX_CPU) + 1;
    pstGroup->btModuleFlag = 1;
    pstGroup->btRsv = 0;

    return (CPSS_OK);    
}

/*******************************************************************************
* 函数名称: cpss_com_phy2ip                            
* 功    能:  物理地址转化为IP地址
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* stPhyAddr    CPSS_COM_PHY_ADDR_T  输入            物理地址
* bMateFlag    BOOL                 输入            是否主备间物理地址
* 返回值:
*         IP地址
* 说   明:如果是主备之间通信使用的，则返回是15.x.x.x形式。
*         如果是普通UDP通信使用的IP，则返回是10.x.x.x形式。 
*******************************************************************************/
#ifndef CPSS_IP_OLD
INT32 cpss_com_phy2ip(CPSS_COM_PHY_ADDR_T stPhyAddr, UINT32 *pulIpAddr,BOOL bMateFlag)
{
#ifndef SWP_FNBLK_BRDTYPE_ABOX
#ifdef CPSS_VOS_VXWORKS
#ifdef CPSS_HOST_CPU
    UINT8  aucMacAddr[6] ;
    INT32  lRet ;    

    if(TRUE == bMateFlag)
    {
        lRet = drv_ether_macip_get(&stPhyAddr,
            DRV_ETHER_UPDATE_PORT_IPV4,0,aucMacAddr,pulIpAddr) ;
    }
    else
    {
        lRet = drv_ether_macip_get(&stPhyAddr,
            DRV_ETHER_CTRL_PORT_IPV4,0,aucMacAddr,pulIpAddr) ;
    }
    
    if(CPSS_OK != lRet)
    {
        return lRet ;
    }
#endif
    return CPSS_OK ;
#endif

#ifdef CPSS_VOS_WINDOWS
    UINT32 ulLogShelf;
    UINT32 ulIpHigh8;
    
    ulLogShelf=(stPhyAddr.ucFrame-1)*CPSS_COM_MAX_SHELF+stPhyAddr.ucShelf;
    
    ulIpHigh8 = SWP_IP_HIGH8;
    
    *pulIpAddr = ((ulIpHigh8 << 24)+(ulLogShelf << 16)+(stPhyAddr.ucSlot << 8)+
        stPhyAddr.ucCpu);
    
    *pulIpAddr = cpss_htonl(*pulIpAddr);
    
    return CPSS_OK ;
#endif  

#ifdef CPSS_VOS_LINUX
    UINT32 ulLogShelf;
    UINT32 ulIpHigh8;
    UINT8 ucPortNo = 1;   
    ulLogShelf=(stPhyAddr.ucFrame-1)*CPSS_COM_MAX_SHELF+stPhyAddr.ucShelf;
    
    ulIpHigh8 = SWP_IP_HIGH8;

    switch(stPhyAddr.ucSlot)
    {
    case 1:
    	break;
    case 2:
    	stPhyAddr.ucSlot = 2;
    	stPhyAddr.ucCpu = 1;
    	break;
    case 3:
    	stPhyAddr.ucSlot = 2;
    	stPhyAddr.ucCpu = 2;
    	break;
    case 4:
    	stPhyAddr.ucSlot = 2;
    	stPhyAddr.ucCpu = 3;
    	break;
    default:
    	break;
    }
    *pulIpAddr = ( (ulIpHigh8<<24)&0xff000000)
          | ((((0xe0&(stPhyAddr.ucFrame<<5))|(0x1f&stPhyAddr.ucShelf))<<16)&0x00ff0000)
          |((((0xf8&(stPhyAddr.ucSlot<<3))|(0x7&ucPortNo))<<8)&0x0000ff00)
          |(stPhyAddr.ucCpu&0x000000ff);
    
#if 0
    *pulIpAddr = ((ulIpHigh8 << 24)+(ulLogShelf << 16)+(stPhyAddr.ucSlot << 8)+
        stPhyAddr.ucCpu);
#endif
    
    *pulIpAddr = cpss_htonl(*pulIpAddr);
    
    return CPSS_OK ;
#endif  


#else
    *pulIpAddr = cpss_htonl(CPSS_ALARM_BOX_IP_ADDR);
    return CPSS_OK ;
#endif
}
#else
INT32 cpss_com_phy2ip(CPSS_COM_PHY_ADDR_T stPhyAddr, UINT32 *pulIpAddr,BOOL bMateFlag)
{

    UINT32 ulLogShelf;
    UINT32 ulIpHigh8;

    ulLogShelf=(stPhyAddr.ucFrame-1)*CPSS_COM_MAX_SHELF+stPhyAddr.ucShelf;
    
    ulIpHigh8 = SWP_IP_HIGH8;
    
    *pulIpAddr = ((ulIpHigh8 << 24)+(ulLogShelf << 16)+(stPhyAddr.ucSlot << 8)+
        stPhyAddr.ucCpu);
    
    *pulIpAddr = cpss_htonl(*pulIpAddr);

    return CPSS_OK ;

}
#endif
/*******************************************************************************
* 函数名称: cpss_com_ip2phy
* 功    能: IP地址转化为物理地址
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* stPhyAddr    CPSS_COM_PHY_ADDR_T  输入            物理地址
* bMateFlag    BOOL                 输入            是否主备间物理地址
* 返回值:
*         IP地址
* 说   明:如果是主备之间通信使用的，则返回是15.x.x.x形式。
*         如果是普通UDP通信使用的IP，则返回是10.x.x.x形式。 
*******************************************************************************/
#ifndef CPSS_IP_OLD
INT32 cpss_com_ip2phy(UINT32 ulIpAddr, CPSS_COM_PHY_ADDR_T *pstPhyAddr)
{
    UINT32 ulFrame;
    UINT32 ulShelf;
    UINT32 ulSlot;    
    
    ulFrame = (ulIpAddr & 0x00e00000) >> 21;
    ulShelf = (ulIpAddr & 0x001f0000) >> 16;
    ulSlot  = (ulIpAddr & 0x0000f800) >> 11;
    
    pstPhyAddr->ucFrame = (UINT8)ulFrame;
    pstPhyAddr->ucShelf = (UINT8)ulShelf;
    pstPhyAddr->ucSlot = (UINT8)ulSlot;
    pstPhyAddr->ucCpu = (UINT8)(ulIpAddr & 0x000000ff);
    
    if(pstPhyAddr->ucSlot == 2)
    {
		switch(pstPhyAddr->ucCpu)
		{
		case 1:
			pstPhyAddr->ucSlot = 2;
			pstPhyAddr->ucCpu = 1;
			break;
		case 2:
			pstPhyAddr->ucSlot = 3;
			pstPhyAddr->ucCpu = 1;
			break;
		case 3:
			pstPhyAddr->ucSlot = 4;
			pstPhyAddr->ucCpu = 1;
			break;
		}
    }
    return CPSS_OK;
    
}
#else
INT32 cpss_com_ip2phy(UINT32 ulIpAddr, CPSS_COM_PHY_ADDR_T *pstPhyAddr)
{
    UINT32 ulFrame;
    UINT32 ulShelf;
    UINT32 ulSlot;    
    
    ulShelf = (ulIpAddr & 0x00ff0000) >> 16;
    ulFrame = CPSS_DIV(ulShelf,(CPSS_COM_MAX_SHELF + 1)) + 1;
    ulShelf = ulShelf - (ulFrame - 1) * CPSS_COM_MAX_SHELF;
    ulSlot  = (ulIpAddr & 0x0000ff00) >> 8;
    
    pstPhyAddr->ucFrame = (UINT8)ulFrame;
    pstPhyAddr->ucShelf = (UINT8)ulShelf;
    pstPhyAddr->ucSlot = (UINT8)ulSlot;
    pstPhyAddr->ucCpu = (UINT8)(ulIpAddr & 0x000000ff);
    
    return CPSS_OK;
    
}
#endif

/*******************************************************************************
* 函数名称: cpss_com_log_addr_get                            
* 功    能: 根据Module和物理地址生成逻辑地址
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称     类型  输入/输出      描述
* ucModule    UINT8    输入    逻辑地址的模组号 
* ulPhyAddr   UINT32   输入    逻辑地址对应的物理地址
* pulLogAddr  UINT32*  输出    生成的逻辑地址
*
* 返回值:
*   成功：CPSS_OK；
*   失败：CPSS_ERROR;
*
* 说   明:
*                                                 
******************************************************************************/
INT32 cpss_com_log_addr_get(UINT8 ucModule, CPSS_COM_PHY_ADDR_T *pstPhyAddr, CPSS_COM_LOGIC_ADDR_T *pstLogAddr)
{
    UINT8 ucSubGroup;
    CPSS_LOGIC_ADDR_GROUP_T stGroup;
    
    BOOL bRet = cpss_com_phyAddr_valid(pstPhyAddr);
    
    if (bRet != TRUE)
    {
        return (CPSS_ERROR);
    }
    ucSubGroup = pstPhyAddr->ucCpu;
    stGroup.btFrame = pstPhyAddr->ucFrame;
    stGroup.btShelf = pstPhyAddr->ucShelf;
    stGroup.btSlot  = pstPhyAddr->ucSlot;
    stGroup.btRsv = 0;
    stGroup.btModuleFlag = 1;
    cpss_mem_memcpy((void*)&pstLogAddr->usGroup, (void*)&stGroup, sizeof(CPSS_LOGIC_ADDR_GROUP_T));
    pstLogAddr->ucModule = ucModule;
    pstLogAddr->ucSubGroup = ucSubGroup;
    return (CPSS_OK);
#if 0
    UINT8 ucSubGroup;
    CPSS_LOGIC_ADDR_GROUP_T stGroup;
    BOOL bRet = cpss_com_phyAddr_valid(pstPhyAddr);
    if (bRet != TRUE)
    {
        return (CPSS_ERROR);
    }
    pstLogAddr->ucModule = ucModule;
    switch(*(UINT32*)(pstPhyAddr))
    {
		case 0x01010101:
			pstLogAddr->usGroup = 1;
			pstLogAddr->ucSubGroup = 1;
			break;
		case 0x01010201:
			pstLogAddr->usGroup = 2;
			pstLogAddr->ucSubGroup = 1;
			break;
		case 0x01010301:
			pstLogAddr->usGroup = 2;
			pstLogAddr->ucSubGroup = 3;
			break;
		case 0x01010401:
			pstLogAddr->usGroup = 2;
			pstLogAddr->ucSubGroup = 3;
			break;
    }
    
#if 0
    ucSubGroup = pstPhyAddr->ucCpu;
    
    stGroup.btFrame = pstPhyAddr->ucFrame;
    stGroup.btShelf = pstPhyAddr->ucShelf;
    stGroup.btSlot  = pstPhyAddr->ucSlot;
    stGroup.btRsv = 0;
    stGroup.btModuleFlag = 1;
    cpss_mem_memcpy((void*)&pstLogAddr->usGroup, (void*)&stGroup, sizeof(CPSS_LOGIC_ADDR_GROUP_T));
    pstLogAddr->ucModule = ucModule;
    pstLogAddr->ucSubGroup = ucSubGroup;
#endif
    
    return (CPSS_OK);
#endif
}
/*******************************************************************************
* 函数名称: cpss_com_get_local_dsp_addr_info                            
* 功    能: 根据Module和物理地址生成逻辑地址
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称     类型  输入/输出      描述
* ucModule    UINT8    输入    逻辑地址的模组号 
* ulPhyAddr   UINT32   输入    逻辑地址对应的物理地址
* pulLogAddr  UINT32*  输出    生成的逻辑地址
*
* 返回值:
*   成功：CPSS_OK；
*   失败：CPSS_ERROR;
*
* 说   明:
******************************************************************************/
INT32 cpss_com_get_local_dsp_addr_info(UINT32* pulIp,UINT8 *puacMac,
                                       CPSS_COM_PHY_ADDR_T *pPhyAddr)
{
    CPSS_COM_PHY_ADDR_T stPhyAddr = {0} ;
    UINT32 ulIpAddr = 0 ;
    UINT32 ulPannel = 14 ;
    UINT32 ulFrame ;
    UINT32 ulShelf ;
    UINT32 ulSlot ;
    UINT32 ulCpuNo ;

    if((pulIp == NULL)||(puacMac == NULL)||(pPhyAddr == NULL))
    {
        return CPSS_ERR_COM_PARA_ILLEGAL ;
    }

    stPhyAddr = g_pstComData->stAddrThis.stPhyAddr ;

    ulFrame = stPhyAddr.ucFrame ;
    ulShelf = stPhyAddr.ucShelf ;
    ulSlot  = stPhyAddr.ucSlot ;
    ulCpuNo = stPhyAddr.ucCpu ;

    ulIpAddr = (((ulPannel<<24)&(0xff000000))|
               ((ulFrame<<21)&(0x00e00000))|
               ((ulShelf<<16)&(0x001f0000))|
               ((ulSlot<<11)&(0x0000f800))|
               (ulCpuNo&(0x000000ff))) ;
        
    ulIpAddr = (ulIpAddr|0x00000300) ;
    *pulIp = ulIpAddr ;
   
    *puacMac = 0x0;
    puacMac = puacMac+1;
    *puacMac = 0x14;
    puacMac = puacMac+1;
    *puacMac = 0xd5;
    puacMac = puacMac+1;
    *puacMac = (ulIpAddr&0x00ff0000) >>16;
    puacMac = puacMac+1;
    *puacMac = (ulIpAddr&0x0000ff00) >>8;
    puacMac = puacMac+1;
    *puacMac = ulIpAddr&0x000000ff;

    *pPhyAddr = stPhyAddr ;
    
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_com_get_drvtype_by_phyaddr                            
* 功    能:    根据目的物理地址判断通往下一跳的驱动类型
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* stPhyAddr  CPSS_COM_PHY_ADDR_T    输入           物理地址
* 返回值:
链路对象
* 说   明:
*  1、如果目的CPU号＝＝1，则目的节点是HOST处理器，如果本身是
          从CPU，则去目的节点必需经过PCI；如果本身不是从CPU，则去
          目的节点必需经过UDP。
    2、如果目的CPU号不等于1，则目的节点必然是从CPU，如果本身
           是带从CPU的HOST，则去目的节点必然经过PCI；如果本身不是
           带从CPU的处理器，则去目的节点必然经过UDP。
    3、目前不支持从CPU到从CPU的路由功能。
                                               
*******************************************************************************/
UINT32 cpss_com_get_drvtype_by_phyaddr(CPSS_COM_PHY_ADDR_T stPhyAddr)
{
#ifndef CPSS_PCI_SIM_BY_UDP    
    if (stPhyAddr.ucCpu == CPSS_COM_PCI_MASTER_CPU_NO)
    {
        #ifdef CPSS_SLAVE_CPU
        return (CPSS_COM_DRV_PCI);
        #else
        return (CPSS_COM_DRV_UDP);
        #endif
    }
    else
    {
        #ifdef CPSS_MULTI_CPU
        return (CPSS_COM_DRV_PCI);
        #else
        return (CPSS_COM_DRV_UDP);
        #endif
    }
#else
    return (CPSS_COM_DRV_UDP); 
#endif
}

/*******************************************************************************
* 函数名称: cpss_com_packet_setup                            
* 功    能: 根据消息头部信息生成包文
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* pstMsg    CPSS_COM_MSG_HEAD_T*    输入            分发层消息头 
* pstLink   CPSS_COM_LINK_T*             输入            链路对象
* 返回值:
*        传输层消息头
* 说   明:                                                
******************************************************************************/
CPSS_COM_TRUST_HEAD_T* cpss_com_packet_setup
(
CPSS_COM_MSG_HEAD_T *pstMsg,
CPSS_COM_LINK_T *pstLink
)
{
    CPSS_COM_TRUST_HEAD_T *pstTrustHdr;
    CPSS_COM_MSG_HEAD_T* pstAllocMsg ;
        
    /* 如果使用共享内存,则不需要填写信息 */
    if(CPSS_COM_MEM_SHARE == pstMsg->ucShareFlag)
    {        
        pstTrustHdr               = (CPSS_COM_TRUST_HEAD_T*)CPSS_DISP_HDR_TO_TRUST_HDR(pstMsg);
        return (pstTrustHdr);
    }
    
    /* 分配传输层消息头 */
    pstTrustHdr=(CPSS_COM_TRUST_HEAD_T*)cpss_com_mem_alloc(pstMsg->ulLen);
        
    if(NULL == pstTrustHdr)
    {
        return NULL;
    }

    pstTrustHdr->fragFlag = 0 ;
    pstTrustHdr->fragOffset = 0 ;
    pstTrustHdr->fragOffsetEx = 0 ;
    pstTrustHdr->usAck = 0 ;
    pstTrustHdr->usCheckSum = 0 ;
    pstTrustHdr->usFlowCtrl = 0 ;
    pstTrustHdr->usSeq = 0 ;
    pstTrustHdr->usSliceLen = 0 ;
    pstTrustHdr->usReserved = 0 ;
    
    /* 初始化传输层消息头 */     
    cpss_com_trust_hrd_pub_info_fill(pstTrustHdr) ;

    pstTrustHdr->ucPType = COM_SLID_PTYPE_INCREDIBLE ;
    
    /* 拷贝分发层消息头和消息体 */
    pstAllocMsg = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr);

    cpss_mem_memcpy(pstAllocMsg, pstMsg,CPSS_COM_DISP_HEAD_LEN);

    cpss_mem_memcpy(CPSS_TRUST_HDR_TO_USR_HDR(pstTrustHdr),pstMsg->pucBuf,pstMsg->ulLen);
    
    pstAllocMsg->ucShareFlag = CPSS_COM_MEM_SHARE ;

    /* 修正消息体内存指针位置 */
    pstMsg=(CPSS_COM_MSG_HEAD_T *)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr);
    pstMsg->pucBuf=CPSS_TRUST_HDR_TO_USR_HDR(pstTrustHdr);
    
    return pstTrustHdr;
}

/*******************************************************************************
* 函数名称: cpss_com_packet_setup_local                            
* 功    能: 根据消息头部信息生成包文
* 函数类型:    
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* pstMsg    CPSS_COM_MSG_HEAD_T*    输入            分发层消息头 
* 返回值:
*        传输层消息头
* 说   明:                                     
******************************************************************************/
CPSS_COM_TRUST_HEAD_T* cpss_com_packet_setup_local
(
CPSS_COM_MSG_HEAD_T *pstMsg
)
    {
    CPSS_COM_TRUST_HEAD_T *pstTrustHdr;
    CPSS_COM_MSG_HEAD_T* pstAllocMsg ;
    IPC_MSG_HDR_T* pstIpcHdr ;
    UINT8 *pucData;
    
    /* 如果使用共享内存,则不需要填写信息 */
    if(CPSS_COM_MEM_SHARE == pstMsg->ucShareFlag)
    {        
        pstTrustHdr               = (CPSS_COM_TRUST_HEAD_T*)CPSS_DISP_HDR_TO_TRUST_HDR(pstMsg);
        return (pstTrustHdr);
    }
     
    /* 分配传输层消息头 */
    pstIpcHdr = (IPC_MSG_HDR_T*)cpss_ipc_msg_alloc(pstMsg->ulLen + CPSS_COM_DISP_HEAD_LEN);
    if(NULL == pstIpcHdr)
    {
        return NULL;
    }
    
    pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)CPSS_IPC_HDR_TO_TRUST_HDR(pstIpcHdr);
    pstIpcHdr->lTimeout = NO_WAIT;
    pstIpcHdr->ptNext = NULL ;

   
    /* 拷贝分发层消息头和消息体 */
    pstAllocMsg = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr);

    memcpy(pstAllocMsg, pstMsg,CPSS_COM_DISP_HEAD_LEN);
    pucData = CPSS_TRUST_HDR_TO_USR_HDR(pstTrustHdr);
    memcpy(pucData,pstMsg->pucBuf,pstMsg->ulLen);
    
    pstAllocMsg->ucShareFlag = CPSS_COM_MEM_SHARE ;
    
    /* 修正消息体内存指针位置 */
    pstMsg=(CPSS_COM_MSG_HEAD_T *)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr);
    pstMsg->pucBuf=CPSS_TRUST_HDR_TO_USR_HDR(pstTrustHdr);
    
    return (pstTrustHdr);
}
/*******************************************************************************
* 函数名称: cpss_com_trust_hrd_pub_info_fill                            
* 功    能: 填充可靠传输头中的公共信息
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*        传输层消息头
* 说   明:
*                                                 
******************************************************************************/
VOID cpss_com_trust_hrd_pub_info_fill
(
CPSS_COM_TRUST_HEAD_T* pstTrustHdr
)
{
    pstTrustHdr->ver     = CPSS_COM_PROTOCAL_VER;
    if((pstTrustHdr->fragFlag==COM_FRAG_NOFRAG)||(pstTrustHdr->fragFlag==COM_FRAG_BEGIN))
    {
        pstTrustHdr->hdrLen = CPSS_COM_HDRLEN_PACK(CPSS_COM_LINK_HEAD_LEN) ;
    }
    else
    {
        pstTrustHdr->hdrLen  = CPSS_COM_HDRLEN_PACK(CPSS_COM_TRUST_HEAD_LEN);
    }
}


/*******************************************************************************
* 函数名称: cpss_com_ipc_hdr_clear                            
* 功    能: 初始化IPC的最前的8字节
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*        传输层消息头
* 说   明:
*                                                 
******************************************************************************/
VOID cpss_com_ipc_hdr_clear
(
CPSS_COM_TRUST_HEAD_T* pstTrustHdr
)
{
    IPC_MSG_HDR_T* pstIpcHdr ;

    pstIpcHdr = (IPC_MSG_HDR_T*)CPSS_TRUST_HDR_TO_IPC_HDR(pstTrustHdr);
    
#if (CPSS_TRACE_ROUTE_FLAG == TRUE)
    {
        cpss_mem_memset(&pstTrustHdr->stTimeStamp,0,sizeof(CPSS_TRACE_INFO_T));
    }
#endif
    
    pstIpcHdr->lTimeout = NO_WAIT;
    pstIpcHdr->ptNext = NULL ;
}


/*******************************************************************************
* 函数名称: cpss_com_mem_alloc                            
* 功    能: 申请通信内存的函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*        通信内存的指针
* 说   明:
*                                                 
******************************************************************************/
VOID* cpss_com_mem_alloc
(
INT32  lNBytes
)
{
    VOID *pvMsg;
    
    pvMsg = cpss_ipc_msg_alloc(lNBytes + CPSS_COM_DISP_HEAD_LEN);
    
    if (NULL != pvMsg)
    {
        pvMsg = CPSS_IPC_HDR_TO_TRUST_HDR(pvMsg);
        cpss_com_ipc_hdr_clear(pvMsg);    
    }

    return (pvMsg);
}

/*******************************************************************************
* 函数名称: cpss_com_nodisp_mem_alloc
* 功    能: 申请没有数据分发头的通信内存
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID* cpss_com_nodisp_mem_alloc(INT32 lBytes)
{
    VOID *pvMsg;
        
    pvMsg = cpss_ipc_msg_alloc(lBytes);
    
    if (NULL != pvMsg)
    {
        pvMsg = CPSS_IPC_HDR_TO_TRUST_HDR(pvMsg);
        cpss_com_ipc_hdr_clear(pvMsg);    
    }
    
    return (pvMsg);
}


/*******************************************************************************
* 函数名称: cpss_com_mem_free                            
* 功    能: 释放通信申请的内存
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*        通信内存的指针
* 说   明:
*                                                 
******************************************************************************/
VOID cpss_com_mem_free
(
VOID* pvBuf
)
{    
    cpss_com_ipc_hdr_clear(pvBuf);
    
    pvBuf = CPSS_TRUST_HDR_TO_IPC_HDR(pvBuf) ;

    cpss_ipc_msg_free(pvBuf) ;
}

/*******************************************************************************
* 函数名称: cpss_com_mem_free                            
* 功    能: 释放通信申请的内存
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*        通信内存的指针
* 说   明:
*                                                 
******************************************************************************/
INT32 cpss_com_ipc_msg_send 
(
CPSS_COM_TRUST_HEAD_T*   pstTrustHdr,
INT32   lType
)
{
    cpss_com_timestamp_add_hook_ipc(pstTrustHdr);
    
    return cpss_ipc_msg_send((IPC_MSG_HDR_T *)CPSS_TRUST_HDR_TO_IPC_HDR(pstTrustHdr),lType) ;
}
 
/*******************************************************************************
* 函数名称: cpss_com_mate_phyaddr                            
* 功    能: 判断物理地址否是备用单板
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*        通信内存的指针
* 说   明:
*                                                 
******************************************************************************/
BOOL cpss_com_mate_phyaddr
(
CPSS_COM_PHY_ADDR_T stPhyAddrDst
)
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_PHY_ADDR_T stPhyAddrMate ;
    
    if(CPSS_OK != cpss_com_phy_addr_mate_get(&stPhyAddrMate))
    {
        return FALSE ;
    }
    
    if(0 == CPSS_COM_PHY_ADDR_SAME(stPhyAddrDst,stPhyAddrMate))
    {
        return FALSE ;
    }
#endif
    return TRUE ;
}

/*******************************************************************************
* 函数名称: cpss_com_sock_error_get                            
* 功    能: 得到系统通信的错误码
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*        
* 说   明:                                     
******************************************************************************/
INT32 cpss_com_sock_error_get()
{
    #ifdef CPSS_VOS_WINDOWS
    return (WSAGetLastError());
    #endif

    #ifdef CPSS_VOS_VXWORKS
    return (errnoGet());
    #endif
	
    #ifdef CPSS_VOS_LINUX
    return (errno);
    #endif
}
#ifndef CPSS_DSP_CPU
/*******************************************************************************
* 函数名称: cpss_com_timestamp_add                            
* 功    能: 增加时间戳函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*        
* 说   明:                                     
******************************************************************************/
VOID cpss_com_timestamp_add(UINT32 ulLoc, UINT8* pucBuf)
{
#ifdef CPSS_COM_HDR_WITH_TIMESTAMP
    CPSS_COM_TRUST_HEAD_T *pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)pucBuf;
    if ((ulLoc < CPSS_COM_TIMESTAMP_LOC_MAX) && (ulLoc >= CPSS_COM_TIMESTAMP_LOC_APPI))
    {
        pstTrustHdr->ulTimeStamp[ulLoc] = cpss_timer_get_cur_seconds();
    }
    else
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
                   "cpss_com_timestamp_add: failed by ulLoc %d invalid ", ulLoc);
        return ;
    }
#endif
    return ;
    
}



CPSS_COM_TIMESTAMP_STAT_T *gp_stCpssComTSStat;
UINT32 g_ulCpssComTSCurNum = 0;

/*******************************************************************************
* 函数名称: cpss_com_timestamp_init                            
* 功    能: 时间戳初始化函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*        
* 说   明:                                     
******************************************************************************/
INT32 cpss_com_timestamp_init()
{
#ifdef CPSS_COM_HDR_WITH_TIMESTAMP
    gp_stCpssComTSStat = cpss_mem_malloc(
        CPSS_COM_TIMESTAMP_STAT_NUM_MAX *sizeof(CPSS_COM_TIMESTAMP_STAT_T));
    if (NULL == gp_stCpssComTSStat)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
                   "cpss_com_timestamp_init failed!");
        return (CPSS_ERROR);
    }
    g_ulCpssComTSCurNum = 0;

#endif
    return (CPSS_OK);
    
}

/*******************************************************************************
* 函数名称: cpss_com_timestamp_stat                            
* 功    能: 时间戳统计函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*        
* 说   明:                                     
******************************************************************************/
VOID cpss_com_timestamp_stat(CPSS_COM_TRUST_HEAD_T* pstTrustHdr)
{
#ifdef CPSS_COM_HDR_WITH_TIMESTAMP
    CPSS_COM_TIMESTAMP_STAT_T *pstTSStat;
    
    INT32 lMargin = pstTrustHdr->ulTimeStamp[CPSS_COM_TIMESTAMP_LOC_IPCI] - 
                           pstTrustHdr->ulTimeStamp[CPSS_COM_TIMESTAMP_LOC_APPI];

    if (g_ulCpssComTSCurNum < CPSS_COM_TIMESTAMP_STAT_NUM_MAX)
    {
        CPSS_COM_MSG_HEAD_T *pstMsg;
        
        pstTSStat = gp_stCpssComTSStat + g_ulCpssComTSCurNum;
        pstTSStat->lMargin = lMargin;
        pstMsg=(CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr);
        pstTSStat->ulMsgId = pstMsg->ulMsgId;
        cpss_mem_memcpy((void*)pstTSStat->ulTimeStamp, (void*)pstTrustHdr->ulTimeStamp, 
                                       CPSS_COM_TIMESTAMP_LOC_MAX * sizeof(UINT32));
        g_ulCpssComTSCurNum = g_ulCpssComTSCurNum + 1;
        if (CPSS_COM_TIMESTAMP_STAT_NUM_MAX <= g_ulCpssComTSCurNum)
        {
            g_ulCpssComTSCurNum = 0;
        }
    }
    else
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
                   "cpss_com_timestamp_stat appear Fatal Error!Stat Num(%d) Over flow!", g_ulCpssComTSCurNum);
        g_ulCpssComTSCurNum = 0;
    }
#endif
    return ;
    
}

#ifdef CPSS_VOS_WINDOWS
/*******************************************************************************
* 函数名称: cpss_com_timestamp_file_write                            
* 功    能: 写时间戳函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*        
* 说   明:                                     
******************************************************************************/
VOID cpss_com_timestamp_file_write(UINT8 *pucBuf, UINT32 ulLen)
{
#ifdef CPSS_COM_HDR_WITH_TIMESTAMP
    INT32 lRet;
    UINT32 ulFd;
    
    lRet = cpss_file_create(CPSS_COM_TIMESTAMP_FILE);
    if (CPSS_OK != lRet)
    {
       cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,"\n Create file failed! \n");
        return ;
    }
    
    ulFd = cpss_file_open (CPSS_COM_TIMESTAMP_FILE, (CPSS_FILE_ORDWR | 
                                         CPSS_FILE_OTEXT));

    if (CPSS_FD_INVALID == ulFd)
    {
      cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,"\n Open file failed! \n");
        return ;
    }
    else
    {
        /* Write file. */
        UINT32 ulBytesWrite;
        lRet = cpss_file_write (ulFd, pucBuf, ulLen, &ulBytesWrite);
        if (CPSS_OK != lRet)
        {
            cpss_file_close (ulFd);
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,"\n Write file failed! \n");
            return ;
        }
    }
#endif

    return ;

}

#endif
/*******************************************************************************
* 函数名称: cpss_com_timestamp_show
* 功    能: 时间戳显示函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*        
* 说   明:                                     
******************************************************************************/
VOID cpss_com_timestamp_show(UINT32 ulNum)
{
    /*ulNum是时间戳的类型*/
#ifdef CPSS_COM_HDR_WITH_TIMESTAMP
    CPSS_COM_TIMESTAMP_STAT_T *pstTSStat;
    
    if (0 == ulNum)
    {
        ulNum = g_ulCpssComTSCurNum;
    }
    else
    {
        if (CPSS_COM_TIMESTAMP_STAT_NUM_MAX < ulNum)
        {
            ulNum = g_ulCpssComTSCurNum;
        }
    }

    pstTSStat = gp_stCpssComTSStat;
#ifdef CPSS_VOS_VXWORKS
    {
        INT32 lCount;
        for (lCount = 0; lCount < ulNum; lCount++)
        {
            cps__oams_shcmd_printf ("\n The 0 record msg: msgId = 0x%x, margin: %d\n", pstTSStat->ulMsgId, pstTSStat->lMargin);
            cps__oams_shcmd_printf ("\n Send1:%d, Send2: %d; Recv1: %d, Recv2: %d, Recv3:%d \n",
                pstTSStat->ulTimeStamp[0],
                pstTSStat->ulTimeStamp[1],
                pstTSStat->ulTimeStamp[2],
                pstTSStat->ulTimeStamp[3],
                pstTSStat->ulTimeStamp[4]);
            pstTSStat = pstTSStat + 1;
        }
    }
#endif

#ifdef CPSS_VOS_WINDOWS
    cpss_com_timestamp_file_write((UINT8 *)gp_stCpssComTSStat, 
                   ulNum * sizeof(CPSS_COM_TIMESTAMP_STAT_T));
#endif

#ifdef CPSS_VOS_LINUX
    cpss_com_timestamp_file_write((UINT8 *)gp_stCpssComTSStat, 
                   ulNum * sizeof(CPSS_COM_TIMESTAMP_STAT_T));
#endif

#endif

    return ;

}
#else
INT32 cpss_com_timestamp_init()                                     
{
    return CPSS_OK;
}
VOID cpss_com_timestamp_add(UINT32 ulLoc, UINT8* pucBuf)            
{
    return;
}
VOID cpss_com_timestamp_stat(CPSS_COM_TRUST_HEAD_T* pstTrustHdr)    
{
    return;
}
VOID cpss_com_timestamp_show(UINT32 ulNum)                          
{
    return;
}
/* DMA读函数*/

UINT32 sysMemTop()
{
    return 2000 ;
}
#endif

/*******************************************************************************
* 函数名称: cpss_com_trafic_ip_get_by_logicaddr                            
* 功    能: 通过逻辑地址得到媒体面网口的IP地址
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*           成功: CPSS_OK ;
*           失败: OTHER ;
* 说   明: 此函数转换的逻辑地址不能存在主备关系。
******************************************************************************/
INT32 cpss_com_trafic_ip_get_by_logicaddr
(
CPSS_COM_LOGIC_ADDR_T    *pstDstLogicAddr,
UINT32                   ulPortId,
UINT32                   *pulIp
)
{
    CPSS_LOGIC_ADDR_GROUP_T* pstGroup ;
    CPSS_COM_PHY_ADDR_T stPhyAddr ;
    
    UINT32 ulFrame ;
    UINT32 ulShelf ;
    UINT32 ulSlot ;
    UINT32 ulCpuNo ;

    INT32 lRet ;
    UINT8 aucMacAddr[6] ;

    /*对传入的数据进行判断*/
    if((pstDstLogicAddr == NULL)||(pulIp == NULL))
    {
        return CPSS_ERROR ;
    }

    pstGroup = (CPSS_LOGIC_ADDR_GROUP_T*)&(pstDstLogicAddr->usGroup) ;

    /*得到架、框、槽、cpuNo*/
    ulFrame = pstGroup->btFrame ;
    ulShelf = pstGroup->btShelf ;
    ulSlot  = pstGroup->btSlot ;
    ulCpuNo = pstDstLogicAddr->ucSubGroup ;

    /*得到物理地址*/
    stPhyAddr.ucFrame = (UINT8)ulFrame ;
    stPhyAddr.ucShelf = (UINT8)ulShelf ;
    stPhyAddr.ucSlot  = (UINT8)ulSlot ;
    stPhyAddr.ucCpu   = (UINT8)ulCpuNo ;

    /*调用驱动函数得到网络字节序的IP地址*/
#ifdef CPSS_VOS_VXWORKS
#ifdef CPSS_HOST_CPU    
    lRet = drv_ether_macip_get(&stPhyAddr,
        DRV_ETHER_MEDIA_PORT_IPV4,3,aucMacAddr,pulIp) ;
#else
    return CPSS_OK ;
#endif
#else
    cpss_mem_memset(aucMacAddr,0,6) ;
    lRet = cpss_com_phy2ip(stPhyAddr,pulIp,FALSE) ;
#endif
    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_dsp_logic_addr_get                            
* 功    能: 
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*           成功: CPSS_OK ;
*           失败: OTHER ;
******************************************************************************/
INT32 cpss_com_dsp_logic_addr_get(UINT8 ucCpuNo,CPSS_COM_LOGIC_ADDR_T* pstDspLogAddr)
{
    UINT32 ulAddrFlag ;

    if(pstDspLogAddr == NULL)
    {
        return CPSS_ERROR ;
    }

    /*如果cpuno不再规定的范围内，返回失败*/
    if((ucCpuNo<CPSS_COM_PCI_DSP_CPU_NO_MIN)||(ucCpuNo>CPSS_COM_PCI_DSP_CPU_NO_MAX))
    {
        return CPSS_ERROR ;
    }

    cpss_com_logic_addr_get(pstDspLogAddr,&ulAddrFlag) ;

    pstDspLogAddr->ucSubGroup = ucCpuNo ;

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_m2s_log_addr_get                            
* 功    能: 
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*           成功: CPSS_OK ;
*           失败: OTHER ;
* 说明：
******************************************************************************/
INT32 cpss_com_m2s_log_addr_get
(
 UINT8 ucCpuNo,
 CPSS_COM_LOGIC_ADDR_T* pstMLogicAddr,
 CPSS_COM_LOGIC_ADDR_T* pstSLogicAddr
)
{
    if((pstMLogicAddr == NULL)||(pstSLogicAddr == NULL))
    {
        return CPSS_ERROR ;
    }

    *pstSLogicAddr = *pstMLogicAddr ;

    if((ucCpuNo != CPSS_COM_PCI_SLAVE_CPU_NO)&&
        ((ucCpuNo<CPSS_COM_PCI_DSP_CPU_NO_MIN)||
        (ucCpuNo>CPSS_COM_PCI_DSP_CPU_NO_MAX)))
    {
        return CPSS_ERROR ;
    }

    pstSLogicAddr->ucSubGroup = ucCpuNo ;

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_ip_info_get_by_phyaddr                            
* 功    能: 获取IP的地址
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*           成功: CPSS_OK ;
*           失败: OTHER ;
* 说明:
******************************************************************************/
INT32 cpss_com_ip_addr_gen
(
UINT8 ucPlaneNo, /* 平面编码 */
UINT8 ucSysNo, /* 机架号 */
UINT8 ucShelfNo, /* 机框号 */
UINT8 ucSlotNo, /* 槽位号 */
UINT8 ucPortNo, /* 端口号 */
UINT8 ucCpuNo,/* CPU编号 */
UINT32 *pulIpAddr /*Ip地址 */
)
{
    UINT32 ulIpAddr = 0;

    if(NULL == pulIpAddr) 
    {
       	return CPSS_ERROR;
    }

    ulIpAddr = ( (ucPlaneNo<<24)&0xff000000)
          | ((((0xe0&(ucSysNo<<5))|(0x1f&ucShelfNo))<<16)&0x00ff0000)
          |((((0xf8&(ucSlotNo<<3))|(0x7&ucPortNo))<<8)&0x0000ff00)
          |(ucCpuNo&0x000000ff);

    *pulIpAddr = ulIpAddr;

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_mac_addr_get                            
* 功    能: 获取MAC的地址
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*           成功: CPSS_OK ;
*           失败: OTHER ;
* 说明:
******************************************************************************/
INT32 cpss_com_mac_addr_get
(
 UINT8 ucSysNo,  /* 机架号 */
 UINT8 ucShelfNo,/* 机框号 */
 UINT8 ucSlotNo, /* 槽位号 */
 UINT8 ucPortNo, /* 端口号 */
 UINT8 ucCpuNo,  /* CPU编号 */
 UINT8 *pucMacAddr /* mac地址 */
)
{
    if(NULL == pucMacAddr)
    {
        return CPSS_ERROR;
    }

    pucMacAddr[0] = 0x0;
    pucMacAddr[1] = 0x14;
    pucMacAddr[2] = 0xD5;
    pucMacAddr[3] = (0xe0&(ucSysNo<<5))|(0x1f&ucShelfNo);
    pucMacAddr[4] = (0xf8&(ucSlotNo<<3))|(0x7&ucPortNo);
    pucMacAddr[5] = ucCpuNo;
    
    return CPSS_OK;
}


/*******************************************************************************
* 函数名称: cpss_ip_info_get_by_phyaddr                            
* 功    能: 
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*           成功: CPSS_OK ;
*           失败: OTHER ;
* 说明:
******************************************************************************/
INT32 cpss_ip_info_get_by_phyaddr
(
 CPSS_COM_PHY_ADDR_T stPhyAddr,
 UINT8 ucPortType,
 UINT32 *pulIpAddr,
 UINT8* pucMacAddr
 )
{
    UINT8 ucPlaneNo = 0 ;
    INT32 lRet = 0 ;
    
    /*判断参数是否正确*/
    if((pucMacAddr == NULL)||(pulIpAddr == NULL))
    {
        return CPSS_ERROR ;
    }

    /*得到面号*/
    switch (ucPortType )
    {
    case DRV_ETHER_CTRL_PORT_IPV4:   /* IPV4 控制面 */
        {
            ucPlaneNo = 11;
            break;
        }
    case DRV_ETHER_UPDATE_PORT_IPV4:  /* IPV4 主备通道 */
        {
            ucPlaneNo = 12;
            break;
        }
    case DRV_ETHER_DEBUG_PORT_IPV4:  /* IPV4 调试 */
        {
            ucPlaneNo = 13;
            break;
        }
    case DRV_ETHER_MEDIA_PORT_IPV4:  /* IPV4 媒体面 */
        {
            ucPlaneNo = 14;
            break;
        }
    case DRV_ETHER_FAR_PORT_IPV4:    /* IPV4 O接口 */
        {
            ucPlaneNo = 15;
            break;
        }
    default:
        {
            return CPSS_ERROR ;
        }
    }

    /*获得IP地址*/
    lRet = cpss_com_ip_addr_gen(ucPlaneNo,stPhyAddr.ucFrame,
        stPhyAddr.ucShelf,stPhyAddr.ucSlot,ucPortType,stPhyAddr.ucCpu,pulIpAddr) ;
    if(CPSS_ERROR == lRet)
    {
        return CPSS_ERROR ;
    }

    /*获得MAC地址*/
    lRet = cpss_com_mac_addr_get(stPhyAddr.ucFrame,stPhyAddr.ucShelf,
        stPhyAddr.ucSlot,ucPortType,stPhyAddr.ucCpu,pucMacAddr) ;
    if(CPSS_ERROR == lRet)
    {
        return CPSS_ERROR ;
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_local_brd_ip_get                            
* 功    能: 得到本CPU的MAC地址和IP地址
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*           成功: CPSS_OK ;
*           失败: OTHER ;
* 说明
******************************************************************************/
INT32 cpss_local_brd_ip_get
(
 UINT8 ucPortType,
 UINT32 *pulIpAddr,
 UINT8* pucMacAddr
)
{
    INT32 lRet = 0 ;
    CPSS_COM_PHY_ADDR_T stLocalPhyAddr ;

    /*得到本CPU的物理地址*/
    lRet = cpss_com_phy_addr_get(&stLocalPhyAddr) ;
    if(CPSS_ERROR == lRet)
    {
        return CPSS_ERROR ;
    }

    /*通过物理地址得到IP和MAC地址*/
    lRet = cpss_ip_info_get_by_phyaddr(stLocalPhyAddr,
        ucPortType,pulIpAddr,pucMacAddr) ;

    return lRet ;
}

#ifdef SWP_FNBLK_BRDTYPE_ABOX
VOID cps__oams_shcmd_printf(CHAR *pcFormat, ...)
{
#ifdef CPSS_DSP_CPU
    return;
#else
	va_list pParList;
	va_start(pParList,pcFormat);
	vprintf(pcFormat,pParList);
#endif
}
#endif


#if (CPSS_TRACE_ROUTE_FLAG == TRUE)

UINT32 g_ulCpssTraceRouteDebugSwitch = 1;
CPSS_TRACE_HOOK_INFO_T g_stCpssTraceTask[CPSS_TRACE_ROUTE_TASK_MAX];
UINT32 g_ulCpssTraceTaskCurNum = 0;

void cpss_msg_trace_clear(UINT32 ulMsgId)
{
    INT32 lCount = 0;
    
    for (lCount = 0; lCount < CPSS_TRACE_ROUTE_TASK_MAX; lCount = lCount + 1)
    {
        if (g_stCpssTraceTask[lCount].ulMsgId == ulMsgId)
        {
        
            if (g_ulCpssTraceRouteDebugSwitch)
            {
                cpss_output(CPSS_MODULE_COM, CPSS_PRINT_WARN,"cpss_msg_trace_clear: ulMsgId = 0x%x \n", ulMsgId);
            }
            cpss_mem_memset(&g_stCpssTraceTask[lCount], 0xff, sizeof(CPSS_TRACE_HOOK_INFO_T));
            g_stCpssTraceTask[lCount].ulUsedFlag = CPSS_TRACE_FREE;
            if (g_ulCpssTraceTaskCurNum > 0)
            {
                g_ulCpssTraceTaskCurNum = g_ulCpssTraceTaskCurNum - 1;
            }
            
            if (g_ulCpssTraceRouteDebugSwitch)
            {
                cpss_output(CPSS_MODULE_COM, CPSS_PRINT_WARN,"cpss_msg_trace_clear: g_ulCpssTraceTaskCurNum = %d \n",
                    g_ulCpssTraceTaskCurNum);
            }
        }
    }
}


CPSS_TRACE_HOOK_INFO_T *cpss_trace_config_info_get(UINT32 ulMsgId, UINT32 ulP2, UINT32 ulP3, UINT32 ulP4)
{
    INT32 lCount;
    
    for (lCount = 0; lCount < CPSS_TRACE_ROUTE_TASK_MAX; lCount = lCount + 1)
    {
        if (ulMsgId == g_stCpssTraceTask[lCount].ulMsgId)
        {
            return (&g_stCpssTraceTask[lCount]);
        }
    }
    
    return (NULL);
}

/*
pucBuf: point to link hdr.
*/
extern CPSS_COM_DATA_T *g_pstComData;
/*
main dealing functions. Record trace info to Hdr.
*/
VOID cpss_trace_route_info_set(UINT32 ulNodeLoc, UINT32 ulLayer, UINT32 ulP1, UINT32 ulP2, UINT32 ulP3, UINT32 ulP4, UINT8 *pucBuf)
{
    CPSS_TRACE_HOOK_INFO_T *pstInfo = cpss_trace_config_info_get(ulP1, ulP2, ulP3, ulP4);
    CPSS_COM_TRUST_HEAD_T *pstTrust = (CPSS_COM_TRUST_HEAD_T*)pucBuf;
    
    if (NULL == pstTrust)
    {
        return ;       
    }
    
    if (g_ulCpssTraceRouteDebugSwitch)
    {
        cpss_output(CPSS_MODULE_COM, CPSS_PRINT_WARN,"cpss_trace_route_info_set: ulNodeLoc = %d, ulLayer = %d, ulP1 = 0x%x \n",
            ulNodeLoc, ulLayer, ulP1);
    }
    switch (ulLayer)
    {
    case CPSS_TRACE_LAYER_APPI:
        if (g_pstComData != NULL)
        {
            pstTrust->stTimeStamp.ulSrcAddr = cpss_htonl(*(UINT32*)&g_pstComData->stAddrThis.stPhyAddr);
        }
        pstTrust->stTimeStamp.ulLdtLogAddr = cpss_htonl(pstInfo->ulLdtLogAddr);
        pstTrust->stTimeStamp.ulProcID = cpss_htonl(pstInfo->ulProcId);
        pstTrust->stTimeStamp.ulSrcComIfType = cpss_htonl(pstInfo->ulCurIfType);
        pstTrust->stTimeStamp.ulTsSrcComIf = cpss_htonl(cpss_gmt_get());
        pstTrust->stTimeStamp.ulTsSrcComIfTick = cpss_htonl(cpss_tick_get());
        pstTrust->stTimeStamp.ulDstAddr = cpss_htonl(pstInfo->ulAddr);
        pstTrust->stTimeStamp.ulMsgId = cpss_htonl(pstInfo->ulMsgId);
        if (pstInfo->ulTraceNum > 0)
        {
            pstInfo->ulTraceNum = pstInfo->ulTraceNum - 1;
            if (pstInfo->ulTraceNum == 0)
            {
                cpss_msg_trace_clear(pstInfo->ulMsgId);
            }
        }
        break;
    case CPSS_TRACE_LAYER_DRVSI:
        switch (ulNodeLoc)
        {
        case CPSS_TRACE_NODE_LOC_SRC:
            pstTrust->stTimeStamp.ulSrcDrvIfType = ulP1;
            pstTrust->stTimeStamp.ulTsSrcDrvIfTick = cpss_htonl(cpss_tick_get());
            break;
        case CPSS_TRACE_NODE_LOC_ROUTE:
            pstTrust->stTimeStamp.ulRSDrvIfType = ulP1;
            pstTrust->stTimeStamp.ulTsRSDrvIfTick = cpss_htonl(cpss_tick_get());
            break;
        }
        break;
        case CPSS_TRACE_LAYER_DRVRI:
            switch (ulNodeLoc)
            {
            case CPSS_TRACE_NODE_LOC_DST:
                pstTrust->stTimeStamp.ulDstDrvIfType = ulP1;
                pstTrust->stTimeStamp.ulTsDstRDrvIf = cpss_htonl(cpss_gmt_get());
                pstTrust->stTimeStamp.ulTsDstRDrvIfTick = cpss_htonl(cpss_tick_get());
                break;
            case CPSS_TRACE_NODE_LOC_ROUTE:
                pstTrust->stTimeStamp.ulRRDrvIfType = ulP1;
                pstTrust->stTimeStamp.ulTsRRDrvIf = cpss_htonl(cpss_gmt_get());
                pstTrust->stTimeStamp.ulTsRRDrvIfTick = cpss_htonl(cpss_tick_get());
                if (g_pstComData != NULL)
                {
                    pstTrust->stTimeStamp.ulRouteAddr = cpss_htonl(*(UINT32*)&g_pstComData->stAddrThis.stPhyAddr);
                }
                break;
            }
            break;
            case CPSS_TRACE_LAYER_TOSCHI:
                pstTrust->stTimeStamp.ulTsDstGiveSchTick = cpss_htonl(cpss_tick_get());
                pstTrust->stTimeStamp.ulRecvSchNum = ulP1;
                break;
            case CPSS_TRACE_LAYER_SCHI:
                pstTrust->stTimeStamp.ulTsDstSchTick = cpss_htonl(cpss_tick_get());
                pstTrust->stTimeStamp.ulDstRecvProcID = cpss_htonl(ulP1);
                break;
    }
}

VOID cpss_trace_route_set_mng_info(UINT32 ulLoc, UINT32 ulCurLayer, UINT32 ulCurIfType)
{
    if (ulLoc < CPSS_TRACE_ROUTE_TASK_MAX)
    {
        if (ulCurLayer != CPSS_TRACE_LAYER_INVALID)
        {
            g_stCpssTraceTask[ulLoc].ulCurLayer = ulCurLayer;
        }
        if (ulCurIfType != CPSS_TRACE_IF_INVALID)
        {
            g_stCpssTraceTask[ulLoc].ulCurIfType = ulCurIfType;
        }
    }
}

/*
used explain: used me at com_send etc.
*/
BOOL cpss_trace_route_it(CPSS_TRACE_HOOK_INFO_T *pstInfo, UINT32 *pulLoc, UINT32 ulCurLayer, UINT32 ulCurIfType)
{
    INT32 lCount;
    
    for (lCount = 0; lCount < CPSS_TRACE_ROUTE_TASK_MAX; lCount = lCount + 1)
    {
        if (pstInfo->ulMsgId == g_stCpssTraceTask[lCount].ulMsgId)
        {
            if (NULL != pulLoc)
            {
                *pulLoc = lCount;
            }
                
            if (g_ulCpssTraceRouteDebugSwitch)
            {
                cpss_output(CPSS_MODULE_COM, CPSS_PRINT_WARN,"cpss_trace_route_it: lCount = %d, ulCurLayer = %d, ulCurIfType = %d \n",
                    lCount, ulCurLayer, ulCurIfType);
            }
            cpss_trace_route_set_mng_info(lCount, ulCurLayer, ulCurIfType);
            return (TRUE);
        }
    }
    return (FALSE);
}


/* 
only dealing srcAddr and route addr. if it is srcAddr, then return true.
*/
BOOL cpss_trace_route_it_send(UINT8 *pucBuf, UINT32 *pulIsSrc)
{
    CPSS_COM_TRUST_HEAD_T *pstTrust = (CPSS_COM_TRUST_HEAD_T*)pucBuf;
    CPSS_COM_MSG_HEAD_T *pstMsg = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrust);
    UINT32 ulLocalAddr;
    
    if (NULL == pstTrust)
    {
        return (FALSE);
    }
    
    if (cpss_ntohl(pstTrust->stTimeStamp.ulTraceFlag) == CPSS_TRACE_VALID_FLAG)
    {
        CPSS_COM_LOGIC_ADDR_T stLogAddr ;
        cpss_mem_memcpy(&stLogAddr, &pstMsg->stSrcProc.stLogicAddr, sizeof(CPSS_COM_LOGIC_ADDR_T));
        stLogAddr.usGroup = cpss_ntohs(stLogAddr.usGroup);
        
        /*cpss_com_logic_addr_get((CPSS_COM_LOGIC_ADDR_T*)&ulLocalAddr, &ulAddrFlag);*/
        ulLocalAddr = (*(UINT32*)&g_pstComData->stAddrThis.stLogiAddr);
        if (*(UINT32*)&stLogAddr == ulLocalAddr)
        {
            if (NULL != pulIsSrc )
            {
                *pulIsSrc = TRUE;
            }
        }
        else
        {
            if (NULL != pulIsSrc )
            {
                *pulIsSrc = FALSE;
            }
        }
        
        if (g_ulCpssTraceRouteDebugSwitch)
        {
            cpss_output(CPSS_MODULE_COM, CPSS_PRINT_WARN,"cpss_trace_route_it_send: ulLocalAddr = 0x%x, pkgSrcLogAddr = 0x%x, ulMsgId = 0x%x \n",
                ulLocalAddr, *(UINT32*)&stLogAddr, cpss_ntohl(pstMsg->ulMsgId));
        }
        return (TRUE);
    }
    
    return (FALSE);
}

/*
used explain: used me at drv recv msg or route it or sendtosch or sch it.
*/
BOOL cpss_trace_route_it_recv(UINT8 *pucBuf, UINT32 *pulIsDst)
{
    CPSS_COM_TRUST_HEAD_T *pstTrust = (CPSS_COM_TRUST_HEAD_T*)pucBuf;
    CPSS_COM_MSG_HEAD_T *pstMsg = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrust);
    
    if (NULL == pstTrust)
    {
        return (FALSE);
    }
    
    if (cpss_ntohl(pstTrust->stTimeStamp.ulTraceFlag) == CPSS_TRACE_VALID_FLAG)
    {
        if (NULL != pulIsDst)
        {
            UINT32 ulAddrFlag;
            INT32 lRet;
            CPSS_COM_LOGIC_ADDR_T stLogicAddr;
            CPSS_COM_LOGIC_ADDR_T stLocalLogAddr;
            cpss_mem_memcpy(&stLogicAddr, &pstMsg->stDstProc.stLogicAddr, sizeof(CPSS_COM_LOGIC_ADDR_T));
            stLogicAddr.usGroup = cpss_ntohs(stLogicAddr.usGroup);
            cpss_com_logic_addr_get(&stLocalLogAddr, &ulAddrFlag);
            lRet = CPSS_COM_LOGIC_ADDR_THIS_CPU(stLogicAddr);
            if(TRUE == lRet)
            {
                *pulIsDst = TRUE;
            }
            else
            {
                *pulIsDst = FALSE;
            }
            
            if (g_ulCpssTraceRouteDebugSwitch)
            {
                cpss_output(CPSS_MODULE_COM, CPSS_PRINT_WARN,"cpss_trace_route_it_recv: ulLocalAddr = 0x%x, dstLogAddr = 0x%x, ulMsgId = 0x%x \n",
                    *(UINT32*)&stLocalLogAddr, *(UINT32*)&stLogicAddr, cpss_ntohl(pstMsg->ulMsgId));
            }
        }
        
        return (TRUE);
    }
    return (FALSE);
}


BOOL cpss_trace_route(CPSS_COM_TRUST_HEAD_T *pstTrustHdr)
{
    INT32 lCount;
    CPSS_COM_MSG_HEAD_T *pstMsg = (CPSS_COM_MSG_HEAD_T *)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr);
    
    for (lCount = 0; lCount < CPSS_TRACE_ROUTE_TASK_MAX; lCount = lCount + 1)
    {
        if (pstMsg->ulMsgId == g_stCpssTraceTask[lCount].ulMsgId)
        {
            if (pstTrustHdr != NULL)
            {
                pstTrustHdr->usReserved = 1;
                pstTrustHdr->stTimeStamp.ulTraceFlag = cpss_htonl(CPSS_TRACE_VALID_FLAG);
                return (TRUE);
            }
        }
    }
    
    return (FALSE);
}

#if 1

VOID cpss_trace_route_event_send(UINT8 *pucBuf)
{
    CPS__OAMS_AM_ORIGIN_EVENT_T stOamsEvent;
    CPSS_COM_TRUST_HEAD_T *pstTrustHdr = (CPSS_COM_TRUST_HEAD_T *)pucBuf;
    
    if (NULL == pstTrustHdr)
    {
        return ;
    }
        
    /* 向OAMS上报消息传说路径跟踪结果 */
    stOamsEvent.ulEventNo = cpss_htonl(CPSS_EVENTNO_TRACE_ROUTE_IND);
    stOamsEvent.ulEventSubNo = 0;
#ifndef SWP_FNBLK_BRDTYPE_ABOX
    cps__oams_moid_get_local_moid(CPS__OAMS_MOID_TYPE_BOARD, &stOamsEvent.stEventMoId);
    stOamsEvent.ulExtraLen = CPS__OAMS_AM_EVENT_EXTRA_INFO_LEN;
    {
        cpss_mem_memcpy(&stOamsEvent.aucExtraInfo[0], pucBuf, CPS__OAMS_AM_EVENT_EXTRA_INFO_LEN);
    }
#endif
    stOamsEvent.ulExtraLen = cpss_htonl(stOamsEvent.ulExtraLen);
    
    cpss_output(CPSS_MODULE_COM, CPSS_PRINT_WARN,"Trace event send: TraceFlag(0x%x), ulMsgId = 0x%x\n", pstTrustHdr->stTimeStamp.ulTraceFlag,
             cpss_ntohl(pstTrustHdr->stTimeStamp.ulMsgId));
#ifndef SWP_FNBLK_BRDTYPE_ABOX
    cps__oams_am_send_event(&stOamsEvent);
#endif
        
    return ;
}
#endif

VOID cpss_trace_route_error_hook(UINT8 *pucBuf, UINT32 ulErrno)
{
    CPSS_COM_TRUST_HEAD_T *pstTrust = (CPSS_COM_TRUST_HEAD_T*)pucBuf;
    
    if (NULL == pstTrust)
    {
        return ;
    }
    
    if (cpss_ntohl(pstTrust->stTimeStamp.ulTraceFlag) == CPSS_TRACE_VALID_FLAG)
    {
        pstTrust->stTimeStamp.ulErrno = cpss_htonl(ulErrno);
        
        if (g_ulCpssTraceRouteDebugSwitch)
        {
            cpss_output(CPSS_MODULE_COM, CPSS_PRINT_WARN,"cpss_trace_route_error_hook: ulErrno = %d, dstAddr = 0x%x, ulMsgId = 0x%x \n",
                ulErrno, cpss_ntohl(pstTrust->stTimeStamp.ulDstAddr), cpss_ntohl(pstTrust->stTimeStamp.ulMsgId));
        }
        cpss_trace_route_event_send(pucBuf);
    }
    return ;
}

VOID cpss_trace_route_mng_init()
{
    INT32 lCount;
    
    cpss_mem_memset(&g_stCpssTraceTask, 0xff, CPSS_TRACE_ROUTE_TASK_MAX*sizeof(CPSS_TRACE_HOOK_INFO_T));
    for (lCount = 0; lCount < CPSS_TRACE_ROUTE_TASK_MAX; lCount = lCount + 1)
    {
        g_stCpssTraceTask[lCount].ulUsedFlag = CPSS_TRACE_FREE;
    }
    g_ulCpssTraceTaskCurNum = 0;
}

/*
cpss_msg_trace_start 0x20002 0x81110101 1 0x10000 1
cpss_msg_trace_start 0x20003 0x81110101 0 0x10000 1
cpss_print_level_set 3 255 0
设置一个对方不存在的目的纤程，应该会报错 
cpss_msg_trace_start 0x20002 0x81110101 0 0x110000 0xfe000000 1
*/
INT32 cpss_msg_trace_start(
                           UINT32 ulMsgId,
                           UINT32 ulAddr,
                           UINT32 ulAddrFlag,
                           UINT32 ulTraceNum)
{
    INT32 lCount = 0;
    
    if (g_ulCpssTraceTaskCurNum >= CPSS_TRACE_ROUTE_TASK_MAX)
    {
        cps__oams_shcmd_printf("trace task has overflow!g_ulCpssTraceTaskCurNum=%d\n",g_ulCpssTraceTaskCurNum) ;
        return (CPSS_ERROR);
    }
    
    for (lCount = 0; lCount < CPSS_TRACE_ROUTE_TASK_MAX; lCount = lCount + 1)
    {
        if (g_stCpssTraceTask[lCount].ulUsedFlag == CPSS_TRACE_FREE)
        {
            g_stCpssTraceTask[lCount].ulUsedFlag = CPSS_TRACE_USED;
            g_stCpssTraceTask[lCount].ulAddrFlag = ulAddrFlag;
            g_stCpssTraceTask[lCount].ucAddrType = 0;
            g_stCpssTraceTask[lCount].ulAddr = ulAddr;
            g_stCpssTraceTask[lCount].ulMsgId = ulMsgId;
            g_stCpssTraceTask[lCount].ulTraceNum = ulTraceNum;
            g_ulCpssTraceTaskCurNum = g_ulCpssTraceTaskCurNum + 1;
            g_stCpssTraceTask[lCount].ulValidFlag = TRUE;
            
            if (g_ulCpssTraceRouteDebugSwitch)
            {
                cps__oams_shcmd_printf ("\n cpss_msg_trace_start: I have recv a trace route start req. param as below:  \n");
                cps__oams_shcmd_printf (" ulAddrFlag          = 0x%x\n", ulAddrFlag);
                cps__oams_shcmd_printf (" ulAddr              = 0x%x\n", ulAddr);
                cps__oams_shcmd_printf (" ulMsgId             = 0x%x\n", ulMsgId);
                cps__oams_shcmd_printf (" ulTraceNum           = 0x%x\n", ulTraceNum);
                cps__oams_shcmd_printf (" ulCurTotalTraceNum  = 0x%x\n", g_ulCpssTraceTaskCurNum);
            }
            return CPSS_OK ;
        }
    }
    return (CPSS_ERROR);
}

INT32 cpss_msg_trace_stop()
{
    INT32 lCount = 0;    
    
    g_ulCpssTraceTaskCurNum = 0 ;

    cpss_mem_memset(&g_stCpssTraceTask,0,CPSS_TRACE_ROUTE_TASK_MAX*sizeof(CPSS_TRACE_HOOK_INFO_T)) ;
    
    return CPSS_OK ;
}

VOID cpss_trace_route_show_cur()
{
    INT32 lCount;
    cps__oams_shcmd_printf ("\n NOW Curren trace route info :  \n");

    for (lCount = 0; lCount < CPSS_TRACE_ROUTE_TASK_MAX; lCount = lCount + 1)
    {
        if (g_stCpssTraceTask[lCount].ulUsedFlag == CPSS_TRACE_USED)
        {
            cps__oams_shcmd_printf (" ulAddrFlag          = 0x%x\n", g_stCpssTraceTask[lCount].ulAddrFlag);    
            cps__oams_shcmd_printf (" ulAddr              = 0x%x\n", g_stCpssTraceTask[lCount].ulAddr);        
            cps__oams_shcmd_printf (" ulLdtLogAddr        = 0x%x\n", g_stCpssTraceTask[lCount].ulLdtLogAddr);  
            cps__oams_shcmd_printf (" ulMsgId             = 0x%x\n", g_stCpssTraceTask[lCount].ulMsgId);       
            cps__oams_shcmd_printf (" ulProcId            = 0x%x\n", g_stCpssTraceTask[lCount].ulProcId);      
            cps__oams_shcmd_printf (" ulTraceNum          = 0x%x\n", g_stCpssTraceTask[lCount].ulTraceNum);    
            cps__oams_shcmd_printf (" ulCurTotalTraceNum  = 0x%x\n", g_ulCpssTraceTaskCurNum);                 
        }
    }

    return ;
}

#endif /* #if (CPSS_TRACE_ROUTE_FLAG == TRUE) */
/******************************* 源文件结束 **********************************/



