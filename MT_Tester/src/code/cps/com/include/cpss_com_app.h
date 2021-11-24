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
#ifndef CPSS_COM_APP_H
#define CPSS_COM_APP_H

/********************************* 头文件包含 ********************************/

/******************************** 宏和常量定义 *******************************/

/******************************** 类型定义 ***********************************/
typedef struct tagCPSS_SEND_INTERFACE_STAT
{
    UINT32 ulComSendExSuccNum ;   /*cpss_com_send_extend 接口发送成功次数*/
    UINT32 ulComSendExMsgNullFailNum ;   /*cpss_com_send_extend 接口发送失败次数*/
    UINT32 ulComSendExPackSetupFailNum ;   /*cpss_com_send_extend 接口发送失败次数*/
    UINT32 ulComSendExIpcSendFailNum ;   /*cpss_com_send_extend 接口发送失败次数*/
    UINT32 ulComSendExRouteNotExistFailNum ;   /*cpss_com_send_extend 接口发送失败次数*/
    UINT32  ulComSendExPciLinkFailNum ;   /*cpss_com_send_extend 接口发送失败次数*/
    UINT32  ulComSendExSendStoreFailNum ;   /*cpss_com_send_extend 接口发送失败次数*/
    UINT32  ulComSendExRemoteSendFailNum ;   /*cpss_com_send_extend 接口发送失败次数*/
    UINT32  ulComSendExTcpSendFailNum ;   /*cpss_com_send_extend 接口发送失败次数*/
    UINT32  ulComSendExDesNotExistFailNum ;   /*cpss_com_send_extend 接口发送失败次数*/
    UINT32  ulComSendLocalPcaketSetupFailNum ;   /*cpss_com_send_local 接口发送失败次数*/
    UINT32  ulComSendLocalIpcSendFailNum ;   /*cpss_com_send_local 接口发送失败次数*/
    UINT32  ulComRemoteMsgNullFailNum ;   /*cpss_com_remote_msg_send 接口发送失败次数*/
    UINT32  ulComRemoteSlidSendFailNum ;   /*cpss_com_remote_msg_send 接口发送失败次数*/
    UINT32  ulComRemotePacketSetupFailNum ;   /*cpss_com_remote_msg_send 接口发送失败次数*/
    UINT32  ulComRemoteDrvWriteFailNum;   /*cpss_com_remote_msg_send 接口发送失败次数*/

    UINT32 ulSendMateSuccNum ;    /*cpss_com_send_mate 接口发送成功次数*/
    UINT32 ulSendMateFailNum ;    /*cpss_com_send_mate 接口发送失败次数*/
    UINT32 ulSendPhySuccNum ;     /*cpss_com_send_phy 接口发送成功次数*/
    UINT32 ulSendPhyParaNullFailNum ;     /*cpss_com_send_phy 接口发送失败次数*/
    UINT32  ulSendPhyPacketSetupFailNum ;     /*cpss_com_send_phy 接口发送失败次数*/
    UINT32  ulSendPhyDrvNotFindFailNum ;     /*cpss_com_send_phy 接口发送失败次数*/
    UINT32  ulSendPhyDrvWriteFailNum ;     /*cpss_com_send_phy 接口发送失败次数*/
    UINT32  ulSendForwParamNullFailNum ;     /*cpss_com_send_forward 接口发送失败次数*/
    UINT32  ulSendForwPacketSetuplFailNum ;     /*cpss_com_send_forward 接口发送失败次数*/
    UINT32   ulSendForwIpcSendlFailNum ;     /*cpss_com_send_forward 接口发送失败次数*/
    UINT32  ulSendForwPhySendlFailNum ;     /*cpss_com_send_forward 接口发送失败次数*/
    UINT32  ulSendForwRouteFailNum ;     /*cpss_com_send_forward 接口发送失败次数*/
    UINT32  ulSendForwTcpSendFailNum ;     /*cpss_com_send_forward 接口发送失败次数*/
    UINT32 ulSendForwSuccNum;      /*cpss_com_send_forward 接口发送成功次数*/
    UINT32 ulComRemoteSuccNum;    /*cpss_com_remote_msg_send 接口发送成功次数*/
    UINT32 ulShareMemSendNum ;    /*cpss_com_send_extend 接口共享内存发送次数*/
    UINT32 ulComSendForwardStoreFailNum;
}CPSS_SEND_INTERFACE_STAT_T;

/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/
extern INT32 cpss_com_send_forward
(
    CPSS_COM_MSG_HEAD_T *pstMsg,
    CPSS_COM_PID_T *pstDstPid
);

extern INT32 cpss_com_send_phy_extend
(
 CPSS_COM_PHY_ADDR_T  stDstPhyAddr,
 UINT32 ulDstPd,
 UINT32 ulMsgId, 
 UINT8 *pucData, 
 UINT32 ulDataLen,
 UINT32 ulSendLenMax
);
/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_COM_APP_H */
/******************************** 头文件结束 *********************************/

