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
#include "swp_public.h"
#include "cpss_public.h"
//#include "om_public.h"

#ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL /* 全局板 */

#include "cpss_util_dl_list_lib.h"
#include "smss_public.h"
#include "smss_sysctl.h"
#include "smss_sysctl_gcpa.h"
#include "smss_sysctl_as.h"
#include "smss_verm.h"

/******************************* 局部宏定义 **********************************/

/******************************* 全局变量定义/初始化 *************************/

/******************************* 局部常数和类型定义 **************************/
SMSS_BOARD_INFO_T  ** apstBoardIndex = NULL;  /* 保存各外围板节点的指针数组 */
CPSS_UTIL_DL_LIST_T stHeartBeatList;   /* 需要进行心跳的单板所在的链表头 */
CPSS_UTIL_DL_LIST_T stSwitchList; /* 保存主备倒换的单板的链表 */
UINT32 g_ulSmssOnlineFlag = 0;  /* 收到不在线告警时, 是否处理开关. 0:不处理, 1:处理 */
extern BOOL   g_bRunGpHeartBeat;
/******************************* 局部函数原型声明 ****************************/
VOID smss_sysctl_recv_cps__oams_force_reset(CPS__OAMS_DYND_FORCE_RST_REQ_MSG_T *pstMsg);
VOID smss_sysctl_recv_cps__rdbs_mstatus_chg(CPS__RDBS_DEV_CPUMSTATUSCHG_IND_MSG_T *pstMsg);
VOID smss_sysctl_recv_oamsforceho(CPS__OAMS_DYND_FORCE_HO_REQ_MSG_T *pstMsg);
VOID smss_sysctl_gcpa_send_heartbeat(VOID);
/* API函数 */
INT32 smss_sysctl_phy2switch(CPSS_COM_PHY_ADDR_T *pstPhyAddr, CPSS_COM_LOGIC_ADDR_T *pstLogic);
BOOL smss_sysctl_seqid2pnode(UINT32 ulSeqId, SMSS_CPS__OAMS_FORCE_SWITCH_T **pstOut);
VOID smss_sysctl_gcpa_initboardtable(VOID);
VOID smss_sysctl_gcpa_recv_regreq(SMSS_SYSCTL_REG_REQ_MSG_T *pstMsg);
BOOL smss_sysctl_normal_gcpa_func(CPSS_COM_MSG_HEAD_T *pstMsgHead);
VOID smss_sysctl_gcpa_recv_gp_heartbeat(SMSS_SYSCTL_GP_HEARTBEAT_IND_T *pstMsg);
VOID smss_sysctl_gcpa_recv_bfault(SMSS_SYSCTL_BFAULT_IND_MSG_T *pstMsg);
UINT32 smss_sysctl_gcpa_recv_statuschgind(SMSS_SYSCTL_STATUS_CHG_IND_MSG_T *pstMsgHead);
BOOL smss_sysctl_as_bswitching(CPSS_COM_LOGIC_ADDR_T *pstLogic, 
                               SMSS_CPS__OAMS_FORCE_SWITCH_T **ppstOut);
VOID smss_sysctl_recv_bswitchind(CPSS_COM_MSG_HEAD_T *pstMsgHead);
VOID smss_sysctl_wait_bswitchind_timeout(CPSS_TIMER_MSG_T *pstMsg);
VOID smss_sysctl_idle_gcpa_init(VOID);
VOID smss_sysctl_gcpa_recv_brd_online_chg(SMSS_SYSCTL_BRD_OL_CHG_IND_MSG_T* pstMsg);
INT32 smss_show_board_info(UINT32 ulPhyAddr);
VOID smss_sysctl_shelf_force_reset_rsp(SWP_MO_ID_T *pstMid,UINT32 ulDelayTime,UINT32  ulSeqId);
VOID smss_sysctl_local_force_reset_rsp(UINT32 ulDelayTime,UINT32  ulSeqId);
VOID smss_sysctl_board_force_reset_rsp(SWP_MO_ID_T *pstMid,UINT32 ulDelayTime,UINT32  ulSeqId);
VOID smss_sysctl_gcpa_recv_force_rst_rsp(SMSS_SYSCTL_FORCE_RST_CMD_RSP_MSG_T *pstMsg);
VOID smss_sysctl_boardInfoRsp_Disp(SMSS_BOARD_INFO_QUERY_RSP_MSG_T* pstMsg);
VOID smss_sysctrl_cpustate_set_disp(SMSS_CPU_RSTATE_SET_IND_MSG_T* pstMsg);
UINT32 smss_sysctl_gcpa_set_slave_rstatus(CPSS_COM_PHY_ADDR_T stPhy, UINT8 ucValue);
VOID smss_sysctl_global_force_reset_rsp(SWP_MO_ID_T *pstMid,UINT32 ulDelayTime,UINT32 ulSeqId);
VOID smss_sysctl_cfg_gssa_gtsa(CPSS_COM_PHY_ADDR_T stPhyAddr);
VOID smss_sysctl_send_cfginfo_aboutgssagtsa(UINT32 *pulGcsaFlag,UINT32 *pulGtsaFlag);
VOID smss_sysctl_cfg_masa_about_gssa(CPSS_COM_PHY_ADDR_T stPhyAddr,UINT32 ulGcsaFlag,UINT32  ulGtsaFlag);
VOID smss_sysctl_gssa_adddel_ind_msg(VOID);
VOID smss_sysctl_gcpa_upperclk_req(SMSS_SYSCTL_UPPERCLK_QUERY_REQ_MSG_T *pReq);
VOID smss_sysctl_gcpa_board_status_ind(SMSS_SYSCTL_BOARD_STATUS_IND_MSG_T *pInd);
VOID smss_sysctl_gcpa_fanpara_req(SMSS_SYSCTL_FANPARA_REQ_MSG_T *pReq);
VOID smss_sysctl_gcpa_cpuloadinfo_req(CPSS_COM_MSG_HEAD_T * pstMsg);
VOID smss_sysctl_gcpa_boardloadinfo_req(CPSS_COM_MSG_HEAD_T * pstMsg);
VOID smss_sysctl_gcpa_boardlevel_ind(UINT8 *pucBuf);
VOID smss_sysctl_gcpa_boarddel_ind(UINT8 *pucBuf);
VOID smss_sysctl_gcpa_checkphyver_req(UINT8 *pucBuf);
VOID cps_send_om_alarm(UINT32 ulAlarmNO, UINT32 ulAlarmType, UINT8* pucbuf, UINT32 ulLen);
VOID cps_send_om_event(UINT32 ulEventNO, UINT8* pucbuf, UINT32 ulLen);
UINT8 g_brd_alarmstat[10] = {0};

/******************************* 函数实现 ************************************/
/***************************************************************************
* 函数名称: smss_sysctl_normal_gcpa_func
* 功    能: 处理GCPA特殊的函数。
* 函数类型  VOID
* 参    数: 
* 参数名称        类型              输入/输出       描述
* pstMsgHead    CPSS_COM_MSG_HEAD_T *  IN           消息头指针
* 函数返回: 
*           无           
* 说    明: 
***************************************************************************/
BOOL smss_sysctl_normal_gcpa_func(CPSS_COM_MSG_HEAD_T * pstMsg)
{
    switch(pstMsg->ulMsgId)
    {
    case SMSS_SYSCTL_REG_REQ_MSG:           /* 注册请求消息 */
        smss_sysctl_recv_regreq((SMSS_SYSCTL_REG_REQ_MSG_T*)pstMsg->pucBuf);
        break;
        
    case SMSS_SYSCTL_STATUS_CHG_IND_MSG:    /* 运行状态改变通知消息 */        
        smss_sysctl_recv_statuschgind(pstMsg);
        break;     
        
    case CPS__RDBS_DEV_CPUMSTATUSCHG_IND_MSG:   /* 单板管理状态改变消息 */        
       /* yanghuanjun 20070420 begin */
       if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
       {        
        smss_sysctl_recv_cps__rdbs_mstatus_chg((CPS__RDBS_DEV_CPUMSTATUSCHG_IND_MSG_T*)pstMsg->pucBuf);
       }
       /*  yanghuanjun 20070420 end */
        break;
        
    case CPS__OAMS_DYND_FORCE_RST_REQ_MSG:     /* OAMS的强制复位消息 */        
        smss_sysctl_recv_cps__oams_force_reset((CPS__OAMS_DYND_FORCE_RST_REQ_MSG_T *)pstMsg->pucBuf);
        break;       
        
    case SMSS_SYSCTL_GP_HEARTBEAT_IND_MSG: /* 心跳消息 */   
        smss_sysctl_recv_gp_heartbeat((SMSS_SYSCTL_GP_HEARTBEAT_IND_T*)pstMsg->pucBuf);
        break;
        
    case CPS__OAMS_DYND_FORCE_HO_REQ_MSG:      /* OAMS的强制倒换命令 */
        if (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)
        {
            smss_sysctl_recv_oamsforceho((CPS__OAMS_DYND_FORCE_HO_REQ_MSG_T *)(pstMsg->pucBuf));
        }        
        break;
        
    case SMSS_SYSCTL_BSWITCH_IND_MSG:   /* 主用全局板对外围板倒换的处理流程 */        
        smss_sysctl_recv_bswitchind(pstMsg);
        break;   
        
    case SMSS_TIMER_BSWITCH_CMD_MSG:    /* 向外围板发完倒换命令后等待超时 */       
        smss_sysctl_wait_bswitchind_timeout((CPSS_TIMER_MSG_T*)pstMsg->pucBuf);
        break;
#if 0        
    case SMSS_SHOW_BOARD_INFO_MSG:      /* 调试消息，用于输出指定单板的信息 */
        smss_show_board_info(*(SMSS_SHOW_BOARD_INFO_MSG_T *)(pstMsg->pucBuf));
        break;
#endif
    case SMSS_SYSCTL_BRD_OL_CHG_IND_MSG:
        smss_sysctl_gcpa_recv_brd_online_chg((SMSS_SYSCTL_BRD_OL_CHG_IND_MSG_T*)(pstMsg->pucBuf));
        break;
    case SMSS_SYSCTL_FORCE_RST_CMD_RSP_MSG:
        smss_sysctl_gcpa_recv_force_rst_rsp((SMSS_SYSCTL_FORCE_RST_CMD_RSP_MSG_T*)(pstMsg->pucBuf));
        break;        
    case SMSS_SYSCTL_LOST_HOSTHB_MSG:
        /*主控板丢失外围板的心跳处理 */
        smss_sysctl_gcpa_lost_cpuhb_handle((SMSS_SYSCTL_LOST_HOSTHB_T*)(pstMsg->pucBuf));
        break;
    case SMSS_CPU_RSTATE_SET_IND_MSG:
        {
            smss_sysctrl_cpustate_set_disp((SMSS_CPU_RSTATE_SET_IND_MSG_T*)(pstMsg->pucBuf));
        }
        break;
    case SMSS_BOARD_INFO_QUERY_RSP_MSG:
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: Receive SMSS_BOARD_INFO_QUERY_RSP_MSG!!!.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);         
        smss_sysctl_boardInfoRsp_Disp((SMSS_BOARD_INFO_QUERY_RSP_MSG_T*)(pstMsg->pucBuf));
        break;
    #ifdef SWP_FNBLK_BRDTYPE_MAIN_CTRL  
    case SMSS_SYSCTL_GCPA_UPGRADE_FAIL_IND_MSG:
         cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: Receive SMSS_SYSCTL_GCPA_UPGRADE_FAIL_IND_MSG!!!.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus); 

		 /* 清除整网元升级标记，接着复位主用全局板 */
		 /* 置状态为正常 */
	  	 smss_verm_set_upgstatus(SMSS_UPGSTATUS_NORMAL);
	  	 g_NeUpgTagSmss = SMSS_UPGSTATUS_NORMAL;

         smss_sysctl_enter_end(TRUE,
             SMSS_BRD_RST_CAUSE_ACT_PROC_FAILURE_SLAVE_BRD_STOA);
         break;
    case CPS__RDBS_DEV_GSSAADD_IND_MSG:
         cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: Receive CPS__RDBS_DEV_GSSAADD_IND_MSG.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus); 
         smss_sysctl_gssa_adddel_ind_msg();
         break;
    case CPS__RDBS_DEV_GSSADEL_IND_MSG:
         cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: Receive CPS__RDBS_DEV_GSSADEL_IND_MSG.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus); 
         smss_sysctl_gssa_adddel_ind_msg();
         break;
    #endif
   case SMSS_SYSCTL_UPPERCLK_QUERY_REQ_MSG:  /* 查询上级时钟请求 */         
        smss_sysctl_gcpa_upperclk_req((SMSS_SYSCTL_UPPERCLK_QUERY_REQ_MSG_T *)(pstMsg->pucBuf)); 
        break;
    case SMSS_SYSCTL_BOARD_STATUS_IND_MSG:   /* 用于一级交换板, 二级交换板特殊处理 */ 
        smss_sysctl_gcpa_board_status_ind((SMSS_SYSCTL_BOARD_STATUS_IND_MSG_T *)(pstMsg->pucBuf));
        break;  
    case SMSS_SYSCTL_FANPARA_REQ_MSG:  /* MASA设置风扇参数请求 */
       smss_sysctl_gcpa_fanpara_req((SMSS_SYSCTL_FANPARA_REQ_MSG_T *)(pstMsg->pucBuf)); 
       break;
    case SMSS_SYSCTL_CPULOADINFO_REQ_MSG:  /* CPU负荷查询请求 */
       smss_sysctl_gcpa_cpuloadinfo_req(pstMsg);
       break;
    case SMSS_SYSCTL_BOARDLOADINFO_REQ_MSG:  /* 单板负荷查询请求 */
      smss_sysctl_gcpa_boardloadinfo_req(pstMsg);
      break;
    case SMSS_SYSCTL_BOARDLEVELCHG_IND_MSG:  /* 单板级别改变消息 */
      smss_sysctl_gcpa_boardlevel_ind(pstMsg->pucBuf);
      break;
    case CPS__RDBS_DEV_BOARDDEL_IND_MSG:  /* 单板删除消息 */
      smss_sysctl_gcpa_boarddel_ind(pstMsg->pucBuf);
      break;       
    case SMSS_SYSCTL_CHECKPHYVER_REQ_MSG:  /* 查询RDBS配置的硬件版本信息 */
     smss_sysctl_gcpa_checkphyver_req(pstMsg->pucBuf);

    default:
        return FALSE;
    }
    return TRUE;
}

/***************************************************************************
* 函数名称: smss_sysctl_phy2switch
* 功    能: 根据单板的物理地址获得该单板所在的机框的主用交换板的逻辑地址。
* 函数类型  INT32
* 参    数: 
* 参数名称        类型              输入/输出       描述
* pstPhyAddr    CPSS_COM_PHY_ADDR_T *  IN          物理地址指针
* pstLogic      CPSS_COM_LOGIC_ADDR_T* OUT         逻辑地址
* 函数返回: 
*            SMSS_OK: 获取成功； SMSS_ERROR: 获取失败          
* 说    明: 
***************************************************************************/
INT32 smss_sysctl_phy2switch(CPSS_COM_PHY_ADDR_T *pstPhyAddr,
                             CPSS_COM_LOGIC_ADDR_T *pstLogic)
{
    CPSS_COM_PHY_ADDR_T stPhyAddr;    
    CPS__RDBS_DEV_GETSHELFPHYTYPE_MSG_REQ_T stShelfReq;
    CPS__RDBS_DEV_GETSHELFPHYTYPE_MSG_RSP_T stShelfRsp;
    CPS__RDBS_DEV_GETLOGICADDR_MSG_REQ_T stLogicReq;
    CPS__RDBS_DEV_GETLOGICADDR_MSG_RSP_T stLogicRsp;
    UINT8  ucSlot; 
    
    if ((NULL == pstPhyAddr)||(NULL == pstLogic))
    {
        return SMSS_ERROR;
    }
    
    stShelfReq.ucFrame = pstPhyAddr->ucFrame;
    stShelfReq.ucShelf = pstPhyAddr->ucShelf;
    
    /* 获得机框类型 */
    cps__rdbs_if_entry(CPS__RDBS_DEV_GETSHELFPHYTYPE_MSG,  CPS__RDBS_IF_FUNC, 
        (UINT8 *)&stShelfReq, sizeof(stShelfReq),
        (UINT8 *)&stShelfRsp, sizeof(stShelfRsp)); 
    
    if (stShelfRsp.ulResult != CPS__RDBS_RESULT_OK)
    {
        return SMSS_ERROR;   /* 获取失败，则返回错误 */
    }
    
    /* 根据机框类型获得交换板槽位号 */
    if (CPS__RDBS_DEV_ST_ATCA == stShelfRsp.ucShelfPhyType)  
    {
        ucSlot = 7;     
    }
    else
    {
        ucSlot = 10;   
    }
    stPhyAddr = *pstPhyAddr;
    stPhyAddr.ucSlot = ucSlot;
    stPhyAddr.ucCpu = 1;
    stLogicReq.stCpuPhyAddr = stPhyAddr;
    
    /* 根据物理地址获得逻辑地址 */
    cps__rdbs_if_entry(CPS__RDBS_DEV_GETLOGICADDR_MSG, CPS__RDBS_IF_FUNC, (UINT8 *)&stLogicReq, 
        sizeof(stLogicReq), (UINT8 *)&stLogicRsp, sizeof(stLogicRsp));
    if (stLogicRsp.ulResult != CPS__RDBS_RESULT_OK)
    {
        return SMSS_ERROR;
    }
    *pstLogic = stLogicRsp.stCpuLogicAddr;
    return SMSS_OK;    
}

/***************************************************************************
* 函数名称: smss_sysctl_seqid2pnode
* 功    能: 根据倒换序列号获得在倒换链表内的指针
* 函数类型  BOOL
* 参    数: 
* 参数名称              参数类型           输入/输出            描述
* ulTid                  UINT32               IN              定时器描述符
* ppstOut      SMSS_CPS__OAMS_FORCE_SWITCH_T **    OUT             保存所求的指针
* 函数返回: 
*           TRUE 表示倒换链表内有该节点, FALSE 表示链表内无相应节点。
* 说    明: 
***************************************************************************/
BOOL smss_sysctl_seqid2pnode(UINT32 ulSeqId, SMSS_CPS__OAMS_FORCE_SWITCH_T ** pstOut)
{
    SMSS_CPS__OAMS_FORCE_SWITCH_T * pstNode = 
        (SMSS_CPS__OAMS_FORCE_SWITCH_T *)(stSwitchList.pstHead);

    /* 查询倒换的节点 */
    while (NULL != pstNode)
    {
        if (ulSeqId == pstNode->ulSeqId+1)
        {
            break;
        }
        pstNode = pstNode->pstNext;
    }
    
    /* 获得有效时，输出该节点 */
    if (  (NULL != pstNode)
        &&(NULL != pstOut) )
    {
        *pstOut = pstNode;
        return TRUE;
    }
    return FALSE;
}


/***************************************************************************
* 函数名称: smss_sysctl_gcpa_get_board_state
* 功    能: 根据RDBS保存的单板状态转换为smss内部的单板状态.
* 函数类型: VOID
* 参    数: 无。
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
UINT32 smss_sysctl_gcpa_get_board_state(UINT16 state)
{
	UINT8 ucBoardState = SMSS_BRD_RSTATUS_FAULT;
  UINT8 ucStatus;
	
	ucStatus = state & 0x3;
	
	switch (ucStatus)
	{
	  case CPS__RDBS_DEV_VALUE_BOARD_OFFLINE:
	    ucBoardState = SMSS_BRD_RSTATUS_OFFLINE;
	    break;	
	  case CPS__RDBS_DEV_VALUE_BOARD_MISBRDTYPE:
	    ucBoardState = SMSS_BRD_RSTATUS_TYPERR;
	    break;
	  case CPS__RDBS_DEV_VALUE_BOARD_NORMAL:
	    ucBoardState = SMSS_BRD_RSTATUS_NORMAL;
	    break;   
	  case CPS__RDBS_DEV_VALUE_BOARD_FAULT:
	    ucBoardState = SMSS_BRD_RSTATUS_FAULT;
	    break;
	  default:
	    ucBoardState = SMSS_BRD_RSTATUS_FAULT;
	    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
            "<%s-%s>: get board state %d error selfphy (%d-%d-%d).",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            state, g_stSmssBaseInfo.stPhyAddr.ucFrame,
            g_stSmssBaseInfo.stPhyAddr.ucShelf,
            g_stSmssBaseInfo.stPhyAddr.ucSlot); 
	    break;  
	}
	return ucBoardState;	
}

/***************************************************************************
* 函数名称: smss_sysctl_gcpa_initboardtable
* 功    能: 备用全局板在升主完成后，进行系统内单板表的初始化。
* 函数类型: VOID
* 参    数: 无。
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_initboardtable(VOID)
{
    UINT32 ulIndex;
    SMSS_BOARD_INFO_T * pstBoardPointer = NULL;

    CPS__RDBS_DEV_GETALLBRDPHYINFO_MSG_RSP_T *pstRsp;
    
    pstRsp = (CPS__RDBS_DEV_GETALLBRDPHYINFO_MSG_RSP_T *)cpss_mem_malloc(sizeof(*pstRsp));
    if (NULL == pstRsp) /* 申请空间失败，则直接复位 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                   "<%s-%s>: smss_sysctl_gcpa_initboardtable: malloc (%d) failed.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   sizeof(*pstRsp));
        smss_sysctl_enter_end(TRUE,
                              SMSS_BRD_RST_CAUSE_ALLOC_MEM_FAILURE_GCPA_INIT);
        return;
    }

    cpss_mem_memset(pstRsp, 0, sizeof(CPS__RDBS_DEV_GETALLBRDPHYINFO_MSG_RSP_T));

    /* 查询本网元内所有单板信息 */
    cps__rdbs_if_entry(CPS__RDBS_DEV_GETALLBRDPHYINFO_MSG, CPS__RDBS_IF_FUNC,
                  NULL, 0, 
                  (UINT8 *)pstRsp, sizeof(*pstRsp));

    if (0 != pstRsp->ulResult)  /* 查询数据库失败 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: CPS__RDBS_DEV_GETALLBRDINFO failed, ulResult = 0x%08X.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   pstRsp->ulResult);
        cpss_mem_free(pstRsp);
        return;
    }

    /* 保存各单板信息 */
    for (ulIndex = 0; ulIndex < pstRsp->ulBrdNum; ulIndex++)
    {
        CPS__RDBS_DEV_GETLOGICADDR_MSG_REQ_T stRdbsReq;
        CPS__RDBS_DEV_GETLOGICADDR_MSG_RSP_T stRdbsRsp;
        UINT32 ulBoardIndex;
        
        ulBoardIndex = Phy2BoardIndex(pstRsp->astBrdInfo[ulIndex].stBrdPhyAddr);
        if (NULL == apstBoardIndex[ulBoardIndex])
        {
            /* 为此单板分配存储空间 */
        pstBoardPointer = (SMSS_BOARD_INFO_T *)cpss_mem_malloc(sizeof(SMSS_BOARD_INFO_T));
            if (NULL == pstBoardPointer) /* 申请内存失败，则直接复位 */
        {
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                       "<%s-%s>: malloc failed, size = %d.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                       sizeof(CPS__RDBS_DEV_GETALLBRDINFO_MSG_RSP_T));
            smss_sysctl_enter_end(TRUE,
                                  SMSS_BRD_RST_CAUSE_ALLOC_MEM_FAILURE_GCPA_INIT);
            return;
        }

        cpss_mem_memset(pstBoardPointer, 0, sizeof(SMSS_BOARD_INFO_T));
        apstBoardIndex[ulBoardIndex] = pstBoardPointer;

        /* 链表相关成员初始化 */
        pstBoardPointer->pstPrev = NULL;
        pstBoardPointer->pstNext = NULL;
        pstBoardPointer->bInList = FALSE;
        pstBoardPointer->ucBeatCount = 0;
        pstBoardPointer->usVerState = 0;

        /* 物理地址 */
        pstBoardPointer->stPhyAddr = pstRsp->astBrdInfo[ulIndex].stBrdPhyAddr;
        
        /* 逻辑地址 */
        stRdbsReq.stCpuPhyAddr = pstBoardPointer->stPhyAddr;
        cps__rdbs_if_entry(CPS__RDBS_DEV_GETLOGICADDR_MSG, CPS__RDBS_IF_FUNC,
                      (UINT8 *)&stRdbsReq, sizeof(stRdbsReq),
                      (UINT8 *)&stRdbsRsp, sizeof(stRdbsRsp));
        pstBoardPointer->stLogAddr = stRdbsRsp.stCpuLogicAddr;
        
        pstBoardPointer->ulPhyBrdType = pstRsp->astBrdInfo[ulIndex].ulBoardPhyType;
        pstBoardPointer->ucFunBrdType = pstRsp->astBrdInfo[ulIndex].ucCpuType;
        }
        else
        {
            /* 此单板信息的存储空间已分配 */
            pstBoardPointer = apstBoardIndex[ulBoardIndex];
        }

        /* 设置单板状态 */
        pstBoardPointer->ucBrdRStatus = 
            smss_sysctl_gcpa_get_board_state(pstRsp->astBrdInfo[ulIndex].usStatus);          
        smss_sysctl_query_cps__rdbs_status(pstBoardPointer->stPhyAddr,
                                      &pstBoardPointer->ucRStatus,
                                      &pstBoardPointer->ucMStatus,
                                      &pstBoardPointer->ucAStatus);
        /* 物理版本信息 */
        pstBoardPointer->stPhyBoardVer = pstRsp->astBrdInfo[ulIndex].stPhyBoardVer;
                                           
        /* 全局板不加入心跳列表 */
        if ((TRUE == SMSS_ADDR_IS_EQUAL(pstBoardPointer->stPhyAddr, CPSS_PHY_ADDR_GCPA_MASTER))
            || (TRUE== SMSS_ADDR_IS_EQUAL(pstBoardPointer->stPhyAddr, CPSS_PHY_ADDR_GCPA_SLAVE)))
        {
            continue;
        }

        if ((SMSS_MSTATUS_NORMAL == pstBoardPointer->ucMStatus) /* 未闭塞 */
            && (SMSS_RSTATUS_NORMAL == pstBoardPointer->ucRStatus) /* 正常 */
            && (FALSE == pstBoardPointer->bInList))
        {
            if(g_bRunGpHeartBeat == TRUE)
            {
                pstBoardPointer->ucBeatCount = 0;
                cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                       "<%s-%s>: Add board (%d-%d-%d) to heartbeat list.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                       pstBoardPointer->stPhyAddr.ucFrame, pstBoardPointer->stPhyAddr.ucShelf, 
                       pstBoardPointer->stPhyAddr.ucSlot);

                /* 将此板加入心跳列表 */
                cpss_util_dl_list_add(&stHeartBeatList, (CPSS_UTIL_DL_NODE_T*)pstBoardPointer);
                pstBoardPointer->bInList = TRUE;

                smss_show_hblist();
            }
        }
    }
                        
    /* 释放内存空间 */
    cpss_mem_free(pstRsp);
    
#ifdef SMSS_STUB
        smss_stub_cps__rdbs_dsync();
#endif
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_recv_oamsforceho
* 功    能: SMSS接收到OAMS的强制倒换请求时进行处理
* 参    数:  
* 参数名称      参数类型                  输入/输出        参数描述
* pstMsg    CPS__OAMS_DYND_FORCE_HO_REQ_MSG_T*    IN           OAMS的倒换命令信息
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_recv_oamsforceho( CPS__OAMS_DYND_FORCE_HO_REQ_MSG_T* pstMsg)
{
    CPSS_COM_LOGIC_ADDR_T stLogicAddr;
    CPS__OAMS_DYND_FORCE_HO_RSP_MSG_T stSwitchRsp;
    CPSS_COM_PID_T stPid;
    UINT32 ulTid;
    INT32 lRet;

    CPS__RDBS_DEV_GETLOGICADDR_MSG_REQ_T stGetLogAddReq;
    CPS__RDBS_DEV_GETLOGICADDR_MSG_RSP_T stGetLogAddRsp;
    CPS__OAMS_DYND_FORCE_HO_RSP_MSG_T stHoRsp;
    UINT32 ulBoardIndex;
    CPSS_COM_PHY_ADDR_T stPhyAddr;    
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "<%s-%s>: Received message %s.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
        "CPS__OAMS_DYND_FORCE_HO_REQ_MSG");
    
    /* 消息合法性判断 */
    if (NULL == pstMsg)
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "<%s-%s>: In Function smss_sysctl_recv_oamsforceho,"
        "Receive Null Message.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        return;
    }

    if (SMSS_STANDBY == g_stSmssAsInfo.ucSelfAStatus)
        /* 若本板为备用板,直接返回 */
    {
        return;
    }
    
    /* 根据模组信息获取逻辑地址 */
    pstMsg->stReqHeader.ulSeqId = cpss_ntohl(pstMsg->stReqHeader.ulSeqId);
    pstMsg->stTargetMoId.ulMOC = cpss_ntohl(pstMsg->stTargetMoId.ulMOC);
    
    cpss_mem_memset(&stPid, 0, sizeof(stPid));
    lRet = smss_sysctl_mo2logic((SWP_MO_ID_T *)&pstMsg->stTargetMoId, &stLogicAddr);

    if (SMSS_OK != lRet)   /* MO无效，则返回失败 */
    {
        stSwitchRsp.stRspHeader.ulSeqId = cpss_htonl(pstMsg->stReqHeader.ulSeqId);
        stSwitchRsp.stRspHeader.ulReturnCode = cpss_htonl(SMSS_SYSCTRL_ERR_INVALID_MO);    /* 标志失败原因：MO错误 */
        stSwitchRsp.stSrcPid = pstMsg->stSrcPid;
        stSwitchRsp.bIsMainCtrlBrd = FALSE;
        smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_HO_RSP_MSG, 
            (UINT8 *)&stSwitchRsp, sizeof(stSwitchRsp));
        return;
    }

    if (TRUE == SMSS_ADDR_IS_EQUAL(stLogicAddr, CPSS_LOGIC_ADDR_GCPA))
    {
        stPhyAddr = g_stSmssBaseInfo.stPhyAddr;
    }
    else
    {
        smss_sysctl_mo2phy((SWP_MO_ID_T *)&pstMsg->stTargetMoId, &stPhyAddr);
    }
    
    stHoRsp.stRspHeader.ulSeqId = cpss_htonl(pstMsg->stReqHeader.ulSeqId);
    stHoRsp.stSrcPid = pstMsg->stSrcPid;
    stHoRsp.bIsMainCtrlBrd = cpss_htonl(SMSS_ADDR_IS_EQUAL(stLogicAddr, CPSS_LOGIC_ADDR_GCPA));
    
    stGetLogAddReq.stCpuPhyAddr = stPhyAddr;
    cps__rdbs_if_entry(CPS__RDBS_DEV_GETLOGICADDR_MSG, CPS__RDBS_IF_FUNC,
        (UINT8 *)&stGetLogAddReq, sizeof(stGetLogAddReq),
        (UINT8 *)&stGetLogAddRsp, sizeof(stGetLogAddRsp));

    if (0 != stGetLogAddRsp.ulResult)  
    {   /* RDBS获取失败，直接返回RDBS的错误码 */
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "<%s-%s>: CPS__RDBS_DEV_GETLOGICADDR_MSG return error!.\n",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        stHoRsp.stRspHeader.ulReturnCode = cpss_htonl(stGetLogAddRsp.ulResult);
        smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_HO_RSP_MSG,
            (UINT8 *)&stHoRsp,  sizeof(stHoRsp));
        return;
    }
    else if (0 == stGetLogAddRsp.ucBackupType) 
    {/* 无伙伴单板 */

        stHoRsp.stRspHeader.ulReturnCode = cpss_htonl(SMSS_SYSCTRL_ERR_BSWITCH_STANDLONE);
        smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_HO_RSP_MSG,
            (UINT8 *)&stHoRsp,  sizeof(stHoRsp));
        return;
    }
    else
    {   /* 有伙伴板 */
        ulBoardIndex = Phy2BoardIndex(stGetLogAddRsp.stMateBrdPhyAddr);
        if ((NULL == apstBoardIndex[ulBoardIndex])
            ||(SMSS_RSTATUS_NORMAL != apstBoardIndex[ulBoardIndex]->ucRStatus))
        {   /* 伙伴板没有BOOT过或伙伴板当前状态不正常 */
            stHoRsp.stRspHeader.ulReturnCode = cpss_htonl(SMSS_SYSCTRL_ERR_BSWITCH_MATE_WRONG);
            smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_HO_RSP_MSG,
                (UINT8 *)&stHoRsp,  sizeof(stHoRsp));
            return;
        }        
    }



    {
        CPSS_COM_PHY_ADDR_T stPhyAddr;

        smss_sysctl_mo2phy((SWP_MO_ID_T *)&pstMsg->stTargetMoId, &stPhyAddr);
        
        ulBoardIndex = Phy2BoardIndex(stPhyAddr);

        if (  NULL == apstBoardIndex[ulBoardIndex]                          /* 单板不存在 */
            ||SMSS_ACTIVE != apstBoardIndex[ulBoardIndex]->ucAStatus         /* 不是主用板 */
            ||SMSS_RSTATUS_NORMAL != apstBoardIndex[ulBoardIndex]->ucRStatus)/* 运行状态不正常 */
            /* 当该单板不能倒换时，向OAMS 返回失败 */
        {
            stHoRsp.stRspHeader.ulSeqId = cpss_htonl(pstMsg->stReqHeader.ulSeqId);

            if (NULL == apstBoardIndex[ulBoardIndex])
            {
                stHoRsp.stRspHeader.ulReturnCode = cpss_htonl(SMSS_SYSCTRL_ERR_BSWITCH_NOT_ACTIVE);
            }
            else if (SMSS_ACTIVE != apstBoardIndex[ulBoardIndex]->ucAStatus)
            {
                stHoRsp.stRspHeader.ulReturnCode = cpss_htonl(SMSS_SYSCTRL_ERR_BSWITCH_NOT_ACTIVE);
            }
            else
            {
                stHoRsp.stRspHeader.ulReturnCode = cpss_htonl(SMSS_SYSCTRL_ERR_BSWITCH_STAGE_FORBID);
            }
            stHoRsp.stSrcPid = pstMsg->stSrcPid;
            stHoRsp.bIsMainCtrlBrd = cpss_htonl(FALSE);
            smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_HO_RSP_MSG,
                                   (UINT8 *)&stHoRsp,  sizeof(stHoRsp));
            return;
        }
    }

    if (TRUE == SMSS_ADDR_IS_EQUAL(stLogicAddr, CPSS_LOGIC_ADDR_GCPA))
        /* 若为全局板进行倒换则进行主降备动作 */
    {
        g_stSmssAsInfo.stSwitchInfo.ulSeqId = pstMsg->stReqHeader.ulSeqId;
        g_stSmssAsInfo.stSwitchInfo.ulReason = SMSS_BSWITCH_CPS__OAMS_FORCE;
        g_stSmssAsInfo.stSwitchInfo.stSrcPid = pstMsg->stSrcPid;
        g_stSmssAsInfo.stSwitchInfo.bIsMainCtrlBrd = TRUE;
        smss_sysctl_as_evoked_atos();
        return;
    }
    else  /* 若为外围板，判断该外围板是否是主且正常，若否，直接返回 */
    {
        
    }
    
    if (TRUE == smss_sysctl_as_bswitching(&stLogicAddr, NULL))  
        /* 该逻辑地址的外围板板正在倒换时返回失败 */
    {
        stSwitchRsp.stRspHeader.ulSeqId = cpss_htonl(pstMsg->stReqHeader.ulSeqId);
        stSwitchRsp.stRspHeader.ulReturnCode = cpss_htonl(SMSS_SYSCTRL_ERR_BSWITCH_RUNING);    /* 标志失败原因：正在倒换 */
        stSwitchRsp.bIsMainCtrlBrd = FALSE;
        stSwitchRsp.stSrcPid = pstMsg->stSrcPid;
        smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_HO_RSP_MSG, 
            (UINT8 *)&stSwitchRsp, sizeof(stSwitchRsp));
        return;
    }
    else /* 将倒换信息添入倒换链表 */
    {
        SMSS_CPS__OAMS_FORCE_SWITCH_T  *pstSwitchNode;
        
        pstSwitchNode = cpss_mem_malloc(sizeof(SMSS_CPS__OAMS_FORCE_SWITCH_T));
        if (NULL == pstSwitchNode)  /* 开辟节点失败时返回倒换失败 */
        {
            CPS__OAMS_DYND_FORCE_HO_RSP_MSG_T stSwitchRsp;
            
            stSwitchRsp.stRspHeader.ulSeqId = pstMsg->stReqHeader.ulSeqId;
            stSwitchRsp.stRspHeader.ulReturnCode = SMSS_SYSCTRL_ERR_BSWITCH_NO_MEMORY;            
            stSwitchRsp.stRspHeader.ulSeqId = cpss_htonl(stSwitchRsp.stRspHeader.ulSeqId);
            stSwitchRsp.stRspHeader.ulReturnCode = cpss_htonl(stSwitchRsp.stRspHeader.ulReturnCode);
            stSwitchRsp.bIsMainCtrlBrd = cpss_htonl(FALSE);
            stSwitchRsp.stSrcPid = pstMsg->stSrcPid;
            smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC,CPS__OAMS_DYND_FORCE_HO_RSP_MSG, 
                (UINT8 *)&stSwitchRsp, sizeof(stSwitchRsp));
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "<%s-%s>: smss_sysctl_recv_oamsforceho: Malloc failed, Alarm To Oams.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
            return;
        }
        else
        {
            cpss_mem_memset(pstSwitchNode, 0, sizeof(SMSS_CPS__OAMS_FORCE_SWITCH_T));

            /* 外围板进行倒换则向主用外围板发倒换命令 */
            stPid.stLogicAddr = stLogicAddr;
            stPid.ulPd = SMSS_SYSCTL_PROC;
            smss_com_send(&stPid, SMSS_SYSCTL_BSWITCH_CMD_MSG, NULL, 0);
            
            /* 插入倒换链表 */
            ulTid = cpss_timer_para_set(SMSS_TIMER_BSWITCH_CMD, SMSS_TIMER_BSWITCH_CMD_DELAY,1+pstMsg->stReqHeader.ulSeqId);
            pstSwitchNode->stLogicAddr = stLogicAddr;
            pstSwitchNode->ulSeqId = pstMsg->stReqHeader.ulSeqId;
            pstSwitchNode->ulTimerId = ulTid;
            pstSwitchNode->stSrcPid = pstMsg->stSrcPid;
            cpss_util_dl_list_add(&stSwitchList, (CPSS_UTIL_DL_NODE_T *)pstSwitchNode);
            
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                "<%s-%s>: Add switch node into switchlist.",
                g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
            
        }
    }
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_recv_cps__rdbs_mstatus_chg
* 功    能: 收到RDBS的管理状态改变消息时进行相应处理。
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                  输入/输出       描述
* pstMsg    CPS__RDBS_EDV_BMSTATUSCHG_MSG_T *   IN        管理状态改变信息
* 函数返回: 
*           无
* 说    明: 
***************************************************************************/
VOID smss_sysctl_recv_cps__rdbs_mstatus_chg(CPS__RDBS_DEV_CPUMSTATUSCHG_IND_MSG_T * pstMsg)
{      
    CPSS_COM_PID_T stDstPid;
    SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG_T stRst;
    SMSS_BOARD_INFO_T * pstBoardPointer;
    UINT32 ulBoardIndex;
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "<%s-%s>: Received message %s to block board (%d-%d-%d).",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
        "CPS__RDBS_DEV_CPUMSTATUSCHG_IND_MSG",
        pstMsg->stCpuPhyAddr.ucFrame, pstMsg->stCpuPhyAddr.ucShelf,
        pstMsg->stCpuPhyAddr.ucSlot);
    
    if (NULL == pstMsg) /* 消息合法性判断 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: In Function smss_sysctl_recv_cps__rdbs_mstatus_chg,"
                   "Receive Null Message.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        return;
    }

    /* 根据物理地址获得外围板的下标 */
    ulBoardIndex = Phy2BoardIndex(pstMsg->stCpuPhyAddr);
    pstBoardPointer = apstBoardIndex[ulBoardIndex];
    
    if (NULL == pstBoardPointer)
        /* 若该节点不存在，则申请内存 */
    {
        pstBoardPointer = cpss_mem_malloc(sizeof(SMSS_BOARD_INFO_T));
        
        if (NULL == pstBoardPointer) /* 开辟内存失败，直接复位 */
        {
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,            
            "<%s-%s>: smss_sysctl_recv_cps__rdbs_mstatus_chg: malloc failed, board reset.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus); 
         
            smss_sysctl_enter_end(FALSE,
                SMSS_BRD_RST_CAUSE_SAVE_CPS__RDBS_STAT_GCPA_ALLOC_MEM_FAILURE);        
            return;
        }
        else /* 内存开辟成功， 向该内存写入数据 */
        {
            cpss_mem_memset(pstBoardPointer,0,sizeof(SMSS_BOARD_INFO_T));
            pstBoardPointer->ucRStatus = SMSS_RSTATUS_IDLE;
            apstBoardIndex[ulBoardIndex] = pstBoardPointer;
            pstBoardPointer->ucBeatCount = 0;
            pstBoardPointer->bInList = FALSE;
        }
    }
     /* 设置单板的管理状态 */
     pstBoardPointer->ucMStatus = pstMsg->ucValue;
     pstBoardPointer->stPhyAddr = pstMsg->stCpuPhyAddr;

    /* 根据物理地址修改相应物理单板的管理状态值 */
    if (CPS__RDBS_OMC_MODE_BLOCK == pstMsg->ucMode)    
    {
        cpss_mem_memset(&stDstPid, 0, sizeof(stDstPid));        
        
        if (CPS__RDBS_VALUE_BLOCK == pstMsg->ucValue) /* 立即闭塞 */
        {            
             /* 如果单板在进行心跳，则将其从心跳链表内删除 */
            if (TRUE == pstBoardPointer->bInList)
            {
                    /* 向OAMS上报告警 */
                smss_send_alarm_brdnotresponded_to_oams(SMSS_ALMSUBNO_0, 1, pstBoardPointer->stPhyAddr);
                    cpss_util_dl_list_remove(&stHeartBeatList, (CPSS_UTIL_DL_NODE_T*)pstBoardPointer);
                    pstBoardPointer->pstPrev = NULL;
                    pstBoardPointer->pstNext = NULL;
                    pstBoardPointer->bInList = FALSE;
            }

            /* 设置单板的状态 */
            pstBoardPointer->ucMStatus = SMSS_MSTATUS_BLOCK;
#if 0
            pstBoardPointer->ucRStatus = SMSS_RSTATUS_END;
            pstBoardPointer->ucAStatus = SMSS_UNKNOWN_AS_STATUS;

            /* 设置RDBS中单板状态为故障且备 */
            smss_sysctl_set_cps__rdbs_rstatus(pstMsg->stCpuPhyAddr, SMSS_RSTATUS_END);
            smss_sysctl_set_cps__rdbs_astatus(pstMsg->stCpuPhyAddr, pstBoardPointer->ucAStatus);
            smss_sysctl_set_cps__rdbs_brdstatus(pstMsg->stCpuPhyAddr,CPS__RDBS_DEV_VALUE_BOARD_FAULT);
            smss_sysctrl_set_smss_boardstate(pstMsg->stCpuPhyAddr,SMSS_BRD_RSTATUS_FAULT);
#endif
            stRst.ulSeqId = 0;
            stRst.stPhyAddr = pstMsg->stCpuPhyAddr;
            stRst.ulDelayTime = 0;
            stRst.ulResetReason = 0;
            stRst.ulResetType = cpss_htonl(SMSS_FORCE_RST_SPC_SLOT);
            smss_com_send_phy(pstMsg->stCpuPhyAddr,SMSS_SYSCTL_PROC, SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG, 
                           (UINT8 *)&stRst, sizeof(stRst));   

            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,            
                "<%s-%s>: Send SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG to reset board (%d-%d-%d).",
                g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                pstBoardPointer->stPhyAddr.ucFrame,
                pstBoardPointer->stPhyAddr.ucShelf,
                pstBoardPointer->stPhyAddr.ucSlot);            
        }
    } 
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_recv_cps__oams_force_reset
* 功    能: 在全局板上接收OAMS的强制复位命令并进行处理
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                     输入/输出       描述
* pstMsg  CPS__OAMS_DYND_forCE_RST_REQ_MSG_T *     IN        复位信息
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_recv_cps__oams_force_reset(CPS__OAMS_DYND_FORCE_RST_REQ_MSG_T * pstMsg)
{   
    SWP_MO_ID_T * pstMid = NULL;
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Received message %s.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               "CPS__OAMS_DYND_FORCE_RST_REQ_MSG");
    
    pstMsg->stTargetMoId.ulMOC = cpss_ntohl(pstMsg->stTargetMoId.ulMOC); 
    pstMsg->ulDelayTime = cpss_ntohl(pstMsg->ulDelayTime); 
    pstMsg->stReqHeader.ulSeqId = cpss_ntohl(pstMsg->stReqHeader.ulSeqId);
    pstMid = (SWP_MO_ID_T *)&(pstMsg->stTargetMoId);
 
    if (SWP_MOC_DEV_SHELF == pstMid->ulMOC)
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: ResetType = %s , DelayTime = %d ",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            "SWP_MOC_DEV_SHELF",pstMsg->ulDelayTime);

        smss_sysctl_shelf_force_reset_rsp(pstMid,pstMsg->ulDelayTime,pstMsg->stReqHeader.ulSeqId);
    }
    else if (SWP_MOC_DEV_BOARD == pstMid->ulMOC)
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: ResetType = %s , DelayTime = %d ",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            "SWP_MOC_DEV_BOARD",pstMsg->ulDelayTime);
        
        /* 复位本板 */
        if ((g_stSmssBaseInfo.stPhyAddr.ucFrame == pstMid->unMOI.stBoard.ucFrameNo)
            &&(g_stSmssBaseInfo.stPhyAddr.ucShelf == pstMid->unMOI.stBoard.ucShelfNo)
            &&(g_stSmssBaseInfo.stPhyAddr.ucSlot == pstMid->unMOI.stBoard.ucSlotNo))
		{
			/* 设置单板版本状态为复位中态"SMSS_VERM_BOARD_VERSTATE_RESET"*/
            smss_verm_set_gcpa_cps__rdbs_verstate(g_stSmssBaseInfo.stPhyAddr, SMSS_VERM_BOARD_VERSTATE_RESET);
            smss_sysctl_local_force_reset_rsp(pstMsg->ulDelayTime,pstMsg->stReqHeader.ulSeqId);
		}
        /* 复位其他单板 */
        else
        {
            smss_sysctl_board_force_reset_rsp(pstMid,pstMsg->ulDelayTime,pstMsg->stReqHeader.ulSeqId);
        }
    }
    else if (SWP_MOC_GLOBAL_DATA == pstMid->ulMOC)  /* 网元复位 */
    {
       cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
          "<%s-%s>: ResetType = %s , DelayTime = %d ",
          g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
          "SWP_MOC_GLOBAL_DATA",pstMsg->ulDelayTime);
       smss_sysctl_global_force_reset_rsp(pstMid,pstMsg->ulDelayTime,pstMsg->stReqHeader.ulSeqId);    	
    }
    else
    {
        CPS__OAMS_DYND_FORCE_RST_RSP_MSG_T stRsp; 
        stRsp.ulSeqId = cpss_htonl(pstMsg->stReqHeader.ulSeqId);
        stRsp.ulReturnCode = cpss_htonl(SMSS_SYSCTRL_ERR_INVALID_MO);
        smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_RST_RSP_MSG, 
            (UINT8 *)&stRsp, sizeof(stRsp));
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "<%s-%s>: Reset Type Error!",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);

        return;
    }
}

/***************************************************************************
* 函数名称: smss_sysctl_gcpa_send_heartbeat
* 功    能: 在主用全局板上对各外围板进行心跳处理
* 函数类型  VOID
* 参    数: 无。
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_send_heartbeat(VOID)
{   
    SMSS_BOARD_INFO_T * pstNode;
    CPSS_COM_PHY_ADDR_T stMasterGcpaPhy = CPSS_PHY_ADDR_GCPA_MASTER;
    CPSS_COM_PHY_ADDR_T stSlaveGcpaPhy = CPSS_PHY_ADDR_GCPA_SLAVE;
    pstNode = (SMSS_BOARD_INFO_T *)stHeartBeatList.pstHead;
    
    while (NULL != pstNode)
    {
        if ((TRUE == SMSS_ADDR_IS_EQUAL(pstNode->stPhyAddr, stMasterGcpaPhy))
            ||(TRUE == SMSS_ADDR_IS_EQUAL(pstNode->stPhyAddr, stSlaveGcpaPhy)))
            /* 若为全局板，直接返回 */
        {
            pstNode = pstNode->pstNext;
            continue;
        }
        pstNode->ucBeatCount++;

        if (pstNode->ucBeatCount >= 3)
        {
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_WARN,
                       "<%s-%s>: Lost heartbeat of board (%d-%d-%d) %d times.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus, 
                       pstNode->stPhyAddr.ucFrame, pstNode->stPhyAddr.ucShelf,
                       pstNode->stPhyAddr.ucSlot, pstNode->ucBeatCount);
        }
        if (pstNode->ucBeatCount >= 5)
        {
            /* 心跳缺失超过5次，进行异常处理 */

            /* 设自己保存的状态 */
            pstNode->ucAStatus = SMSS_STANDBY;
            pstNode->ucRStatus = SMSS_RSTATUS_END;
            pstNode->ucBeatCount = 0;
            pstNode->ucOldRStatus = SMSS_RSTATUS_END;
            /* 设RDBS */
            smss_sysctl_set_cps__rdbs_rstatus(pstNode->stPhyAddr, SMSS_RSTATUS_END);
            smss_sysctl_set_cps__rdbs_astatus(pstNode->stPhyAddr, SMSS_STANDBY); 
            smss_sysctl_set_cps__rdbs_brdstatus(pstNode->stPhyAddr,CPS__RDBS_DEV_VALUE_BOARD_FAULT);
            smss_sysctrl_set_smss_boardstate(pstNode->stPhyAddr,SMSS_BRD_RSTATUS_FAULT);

            /* 向OAMS上报告警 */
            smss_send_alarm_brdnotresponded_to_oams(SMSS_ALMSUBNO_0, 1, pstNode->stPhyAddr);
            
            /* 将该节点从链表中删除 */
            if (TRUE == pstNode->bInList)
            {
                cpss_util_dl_list_remove(&stHeartBeatList, (CPSS_UTIL_DL_NODE_T *)pstNode);
                pstNode->pstNext = NULL;
                pstNode->pstPrev = NULL;
                pstNode->bInList = FALSE;
            }

            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,            
                       "<%s-%s>: Remove board (%d-%d-%d) from heartbeat list.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                       pstNode->stPhyAddr.ucFrame,
                       pstNode->stPhyAddr.ucShelf,
                       pstNode->stPhyAddr.ucSlot);
        }
        else  /* 心跳正常，向该外围板发心跳消息 */
        {
            SMSS_SYSCTL_GP_HEARTBEAT_IND_T stHeartBeat;
            /* 向OAMS上报告警 */
            if (pstNode->ucOldRStatus == SMSS_RSTATUS_END)
            {
                pstNode->ucOldRStatus = SMSS_RSTATUS_NORMAL;
            }
            stHeartBeat.ulSeqNum = cpss_htonl(pstNode->ulSndHeartSeq);
            smss_com_send_phy(pstNode->stPhyAddr, SMSS_SYSCTL_PROC,
                              SMSS_SYSCTL_GP_HEARTBEAT_IND_MSG, (UINT8*)&stHeartBeat, sizeof(stHeartBeat));
            smss_sysctl_record_heart(&(pstNode->stHeartInfo),SMSS_SEND_HEART,pstNode->ulSndHeartSeq);
            pstNode->ulSndHeartSeq += 1;   
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,            
                       "<%s-%s>: pstNode->ulSndHeartSeq = %d.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                       pstNode->ulSndHeartSeq);   
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,            
                       "<%s-%s>: Send heartbeat to board (%d-%d-%d): %d.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                       pstNode->stPhyAddr.ucFrame, pstNode->stPhyAddr.ucShelf,
                       pstNode->stPhyAddr.ucSlot, pstNode->ucBeatCount);
        }
        pstNode = pstNode->pstNext;
    }
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_gcpa_recv_regreq
* 功    能: 全局板受到注册请求时进行处理
* 函数类型  VOID
* 参    数: 
* 参数名称        类型               输入/输出       描述
* pstMsg  SMSS_SYSCTL_REG_REQ_MSG_T *   IN            注册信息
* 函数返回: 
*           无。
* 说    明: 20060512修改对RDBS的设置。
***************************************************************************/
VOID smss_sysctl_gcpa_recv_regreq(SMSS_SYSCTL_REG_REQ_MSG_T *pstMsg)
{
    SMSS_SYSCTL_REG_RSP_MSG_T stRegResult;
    UINT8 ucRStatus;
    UINT8 ucMStatus;
    UINT8 ucAStatus; 
    UINT8 ucQueryRdbsStatus = 0;
 
    UINT32 ulPhyBrdType= cpss_ntohl(pstMsg->ulPhyBrdType);
    UINT32 ulBoardIndex = Phy2BoardIndex(pstMsg->stPhyAddr);
    SMSS_BOARD_INFO_T * pstBoardIndex = apstBoardIndex[ulBoardIndex]; 
    stRegResult.ulResult = SMSS_ERROR;
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Received %s from board (%d-%d-%d).",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               "SMSS_SYSCTL_REG_REQ_MSG", pstMsg->stPhyAddr.ucFrame,
               pstMsg->stPhyAddr.ucShelf, pstMsg->stPhyAddr.ucSlot);

    if (NULL == pstBoardIndex) /* 第一次注册 */
    {
        pstBoardIndex = cpss_mem_malloc(sizeof(SMSS_BOARD_INFO_T));
        apstBoardIndex[ulBoardIndex] = pstBoardIndex;
        
        if (NULL == pstBoardIndex) /* 开辟内存失败 */
        {
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,            
                       "<%s-%s>: smss_sysctl_gcpa_recv_regreq() error: malloc failed.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus); 
            smss_sysctl_enter_end(FALSE,
                SMSS_BRD_RST_CAUSE_SAVE_CPS__RDBS_STAT_GCPA_ALLOC_MEM_FAILURE);
            return;
        }
        else  /* 内存申请成功，置相应状态 */
        {
            cpss_mem_memset(pstBoardIndex, 0, sizeof(SMSS_BOARD_INFO_T));
            pstBoardIndex->ucRStatus = SMSS_RSTATUS_IDLE;
            pstBoardIndex->stPhyAddr = pstMsg->stPhyAddr;
            pstBoardIndex->bInList = FALSE;
            pstBoardIndex->ulPhyBrdType = ulPhyBrdType;
        }
    }
    else   /* 如果该节点存在，判断该节点是否在心跳链表内 */
    {
        if (TRUE == pstBoardIndex->bInList) /* 应从心跳链表内摘除 */
        {
            cpss_util_dl_list_remove(&stHeartBeatList, (CPSS_UTIL_DL_NODE_T *)pstBoardIndex);
            pstBoardIndex->pstNext = NULL;
            pstBoardIndex->pstPrev = NULL;
            pstBoardIndex->bInList = FALSE;
        }
    }
            
    /* 查询数据库，获得该单板的管理状态 */
    ucQueryRdbsStatus = smss_sysctl_query_cps__rdbs_status(pstMsg->stPhyAddr,
                                                      &ucRStatus, &ucMStatus, &ucAStatus);
    if (SMSS_OK == ucQueryRdbsStatus )
        /* 查询成功 */
    {
        pstBoardIndex->ucMStatus = ucMStatus;
    }
    else  /* 查询失败 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: Board (%d-%d-%d) is not configured.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   pstMsg->stPhyAddr.ucFrame, pstMsg->stPhyAddr.ucShelf,
                   pstMsg->stPhyAddr.ucSlot);
        return;
    }
    
    /* 设置单板状态 */
    pstBoardIndex->ucRStatus = SMSS_RSTATUS_IDLE;
    pstBoardIndex->ucAStatus = SMSS_STANDBY;
    pstBoardIndex->stPhyAddr = pstMsg->stPhyAddr;
    pstBoardIndex->ulPhyBrdType = ulPhyBrdType;
    smss_sysctl_set_cps__rdbs_brdstatus(pstMsg->stPhyAddr,CPS__RDBS_DEV_VALUE_BOARD_FAULT);
    smss_sysctrl_set_smss_boardstate(pstMsg->stPhyAddr,SMSS_BRD_RSTATUS_FAULT);        

    /* 回复应答消息 */
    if ((SMSS_MSTATUS_NORMAL == pstBoardIndex->ucMStatus) || (SMSS_MSTATUS_DBLOCK == pstBoardIndex->ucMStatus))
        /* 管理状态正常，回复注册应答 */
    {
        smss_sysctl_set_cps__rdbs_astatus(pstMsg->stPhyAddr, SMSS_STANDBY);
        smss_sysctl_set_cps__rdbs_rstatus(pstMsg->stPhyAddr, SMSS_RSTATUS_END);

        if (TRUE == SMSS_ADDR_IS_EQUAL(pstMsg->stPhyAddr, CPSS_PHY_ADDR_GCPA_SLAVE))
            /* 备用全局板 */
        {
            g_stSmssAsInfo.ucMateRStatus = SMSS_RSTATUS_INIT;                            
        }
        stRegResult.ulResult = SMSS_OK;
        stRegResult.ulResult = cpss_htonl(stRegResult.ulResult);
        
        smss_com_send_phy(pstMsg->stPhyAddr, SMSS_SYSCTL_PROC, SMSS_SYSCTL_REG_RSP_MSG,
                          (UINT8 *)&stRegResult, sizeof(stRegResult)); 
        
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "<%s-%s>: Send %s to board (%d-%d-%d).",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   "SMSS_SYSCTL_REG_RSP_MSG", pstMsg->stPhyAddr.ucFrame,
                   pstMsg->stPhyAddr.ucShelf, pstMsg->stPhyAddr.ucSlot);
        smss_sysctl_cfg_gssa_gtsa(pstMsg->stPhyAddr);
    }
    return;
}
/***************************************************************************
* 函数名称: smss_sysctl_gcpa_recv_gp_heartbeat
* 功    能: 全局板收到心跳消息时进行处理
* 函数类型  VOID
* 参    数: 
* 参数名称        类型               输入/输出       描述
* pstMsg  SMSS_SYSCTL_GP_HEARTBEAT_IND_T *   IN            注册信息
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_recv_gp_heartbeat(SMSS_SYSCTL_GP_HEARTBEAT_IND_T*pstMsg)
{
    UINT32 ulBoardIndex = Phy2BoardIndex(pstMsg->stPhyAddr);
    SMSS_BOARD_INFO_T * pstBoardIndex = apstBoardIndex[ulBoardIndex];
    if (NULL != pstBoardIndex)  /* 如果单板存在，则设置其保存的全局板的心跳次数 */
    {
        pstBoardIndex->ucBeatCount = 0;
    } 
    else
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_WARN,
                   "<%s-%s>: Received heartbeat from unconfigured board (%d-%d-%d).",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   pstMsg->stPhyAddr.ucFrame, pstMsg->stPhyAddr.ucShelf,
                   pstMsg->stPhyAddr.ucSlot);
        return;
    }
    smss_sysctl_record_heart(&(pstBoardIndex->stHeartInfo),SMSS_RECV_HEART,cpss_htonl(pstMsg->ulSeqNum));
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,
               "<%s-%s>: Received heartbeat from board (%d-%d-%d), %d.",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               pstMsg->stPhyAddr.ucFrame, pstMsg->stPhyAddr.ucShelf,
               pstMsg->stPhyAddr.ucSlot, pstBoardIndex->ucBeatCount);
    return;
}
/***************************************************************************
* 函数名称: smss_sysctl_gcpa_recv_bfault
* 功    能: 全局板收到外围板故障消息时进行处理
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                  输入/输出       描述
* pstMsg  SMSS_SYSCTL_BFAULT_IND_MSG_T *   IN        外围板故障消息
* 函数返回: 
*           无。
* 说    明: 只修改RDBS和自己保存的信息。
***************************************************************************/
VOID smss_sysctl_gcpa_recv_bfault(SMSS_SYSCTL_BFAULT_IND_MSG_T *pstMsg)
{
    UINT32 ulBoardIndex = Phy2BoardIndex(pstMsg->stPhyAddr);
    SMSS_BOARD_INFO_T * pstBoardIndex = apstBoardIndex[ulBoardIndex]; 
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: GCPA Received %s from board (%d-%d-%d).",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               "SMSS_SYSCTL_BFAULT_IND_MSG",
        pstMsg->stPhyAddr.ucFrame, pstMsg->stPhyAddr.ucShelf, 
        pstMsg->stPhyAddr.ucSlot);
    
    pstMsg->ulReason = cpss_htonl(pstMsg->ulReason);
    pstMsg->ulType = cpss_htonl(pstMsg->ulType);
    
    /* 设置RDBS */
    smss_sysctl_set_cps__rdbs_rstatus(pstMsg->stPhyAddr, SMSS_RSTATUS_END);
    smss_sysctl_set_cps__rdbs_astatus(pstMsg->stPhyAddr, SMSS_STANDBY);
    smss_sysctl_set_cps__rdbs_brdstatus(pstMsg->stPhyAddr,CPS__RDBS_DEV_VALUE_BOARD_FAULT);
    smss_sysctrl_set_smss_boardstate(pstMsg->stPhyAddr,SMSS_BRD_RSTATUS_FAULT);
    /* 向OAMS上报告警 */
    /* stOamsEvent.ulEventSubNo = 1;
    stOamsEvent.ulEventNo = 2;
    smss_send_event_to_oams(&stOamsEvent); */
    
    /*  修改SMSS 自己保存的单板状态 */
    pstBoardIndex->ucRStatus = SMSS_RSTATUS_END;
    pstBoardIndex->ucAStatus = SMSS_STANDBY;

    /* 将该单板从心跳链表内删除 */
    if (TRUE == pstBoardIndex->bInList)
    {
        /* 向OAMS上报告警 */
        smss_send_alarm_brdnotresponded_to_oams(SMSS_ALMSUBNO_0, 1, pstMsg->stPhyAddr);
        cpss_util_dl_list_remove(&stHeartBeatList, (CPSS_UTIL_DL_NODE_T*)pstBoardIndex);
        pstBoardIndex->pstNext = NULL;
        pstBoardIndex->pstPrev = NULL;
        pstBoardIndex->bInList = FALSE;
    }

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_DETAIL,            
               "<%s-%s>: Remove board (%d-%d-%d) from heartbeat list.",
                g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                pstBoardIndex->stPhyAddr.ucFrame,
                pstBoardIndex->stPhyAddr.ucShelf,
                pstBoardIndex->stPhyAddr.ucSlot);
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_recv_bswitchind
* 功    能: 主用全局板收到外围板的主备倒换完成消息时进行相应处理
* 函数类型  VOID
* 参    数: 
* 参数名称        参数类型             输入/输出        参数描述
* pstMsg   SMSS_SYSCTL_BSWITCH_IND_MSG*   IN          倒换应答消息
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_recv_bswitchind(CPSS_COM_MSG_HEAD_T* pstMsgHead)
{
    CPS__OAMS_DYND_FORCE_HO_RSP_MSG_T stSwitchRsp; 
    /* CPS__OAMS_AM_ORIGIN_EVENT_T stOamsEvent; */
    CPSS_COM_PHY_ADDR_T stPhy;  
    BOOL bSwitching;
    CPSS_COM_LOGIC_ADDR_T stLogicAddr = pstMsgHead->stSrcProc.stLogicAddr;
    SMSS_SYSCTL_BSWITCH_IND_MSG_T * pstMsg =
        (SMSS_SYSCTL_BSWITCH_IND_MSG_T *)pstMsgHead->pucBuf;
    SMSS_CPS__OAMS_FORCE_SWITCH_T  *pstNode = NULL; 
    
    /* 获取单板的索引 */
    UINT32 ulBoardIndex = Phy2BoardIndex(pstMsg->stPhyAddr);
    SMSS_BOARD_INFO_T * pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
    
    pstMsg->ulReason = cpss_htonl(pstMsg->ulReason);
    pstMsg->ulResult = cpss_htonl(pstMsg->ulResult);    
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
               "<%s-%s>: Received %s from board (%d-%d-%d).",
               g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
               "SMSS_SYSCTL_BSWITCH_IND_MSG", pstMsg->stPhyAddr.ucFrame,
               pstMsg->stPhyAddr.ucShelf, pstMsg->stPhyAddr.ucSlot);
    
    if (SMSS_OK == pstMsg->ulResult)
        /* 若倒换成功，设置RDBS */
    {
        stPhy = pstMsg->stPhyAddr;
        smss_sysctl_set_cps__rdbs_astatus(pstMsg->stPhyAddr, SMSS_ACTIVE);
        pstBoardPointer->ucAStatus = SMSS_ACTIVE;
    }
   
    bSwitching = smss_sysctl_as_bswitching(&stLogicAddr, &pstNode);
    if (TRUE == bSwitching)
        /* 记录了倒换信息 */
    {
        /* 删除节点 */
        cpss_util_dl_list_remove(&stSwitchList, (CPSS_UTIL_DL_NODE_T *)pstNode);
        pstNode->pstNext = NULL;
        pstNode->pstPrev = NULL;
        /* 向OAMS回应答 */
        stSwitchRsp.stRspHeader.ulSeqId = pstNode->ulSeqId;
        stSwitchRsp.stRspHeader.ulReturnCode = pstMsg->ulResult;
        stSwitchRsp.stRspHeader.ulSeqId = cpss_htonl(stSwitchRsp.stRspHeader.ulSeqId);
        stSwitchRsp.stRspHeader.ulReturnCode = cpss_htonl(stSwitchRsp.stRspHeader.ulReturnCode);
        stSwitchRsp.bIsMainCtrlBrd = cpss_htonl(FALSE);
        stSwitchRsp.stSrcPid = pstNode->stSrcPid;
        smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_HO_RSP_MSG, 
                               (UINT8 *)&stSwitchRsp, sizeof(stSwitchRsp));
        
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "<%s-%s>: Send Switch Response to OAMS.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus); 
        
        cpss_timer_para_delete(pstNode->ulTimerId);  /* 删除定时器 */
        cpss_mem_free(pstNode);  /* 释放节点空间 */
        
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "<%s-%s>: Remove node from switch list.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
    }  

    /* 进行回复 */
    smss_com_send(&pstMsgHead->stSrcProc, SMSS_SYSCTL_BSWITCH_ACK_MSG, NULL, 0);
    return;
}
/***************************************************************************
* 函数名称: smss_sysctl_gcpa_recv_statuschgind
* 功    能: 主用全局板收到外围板的状态改变消息时进行
* 函数类型  VOID
* 参    数: 
* 参数名称        参数类型                 输入/输出        参数描述
* pstMsg   SMSS_SYSCTL_STATUS_CHG_IND_T*      IN             消息头
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
UINT32 smss_sysctl_gcpa_recv_statuschgind(SMSS_SYSCTL_STATUS_CHG_IND_MSG_T* pstMsg)
{    
    
    UINT32 ulBoardIndex = Phy2BoardIndex(pstMsg->stPhyAddr);
    SMSS_BOARD_INFO_T * pstBoardIndex = apstBoardIndex[ulBoardIndex];

    CPS_CPU_STAT_CHG_IND_MSG_T stCpuStat;

    CPSS_COM_PHY_ADDR_T stBrdPhyAddr;
    UINT8 ucAlarmClr = 0;

    if (NULL == pstBoardIndex)
    {
        return 1;
    }

    /* 收到的单板状态改变消息 */
    if(SMSS_BRD_RSTATUS_NORMAL == pstMsg->ucBoardState)
    {
    	stCpuStat.iState = cpss_htonl((INT32)(0));
    	switch(pstMsg->stPhyAddr.ucSlot)
    	{
    	case 1:
    		stCpuStat.stLogicAddr.ucModule = 16;
    		stCpuStat.stLogicAddr.usGroup = cpss_htons((UINT16)(1));
    		stCpuStat.stLogicAddr.ucSubGroup = 1;

    		stBrdPhyAddr.ucFrame = 1;
    		stBrdPhyAddr.ucShelf = 1;
    		stBrdPhyAddr.ucSlot = 1;
    		stBrdPhyAddr.ucCpu = 1;
    		if(g_brd_alarmstat[1] == 1)
    		{
    			ucAlarmClr = 1;
    			g_brd_alarmstat[1] = 0;
    		}
    		break;
    	case 2:
    		stCpuStat.stLogicAddr.ucModule = 16;
    		stCpuStat.stLogicAddr.usGroup = cpss_htons((UINT16)(2));
    		stCpuStat.stLogicAddr.ucSubGroup = 1;

    		stBrdPhyAddr.ucFrame = 1;
    		stBrdPhyAddr.ucShelf = 1;
    		stBrdPhyAddr.ucSlot = 2;
    		stBrdPhyAddr.ucCpu = 1;
    		if(g_brd_alarmstat[2] == 1)
    		{
    			ucAlarmClr = 1;
    			g_brd_alarmstat[2] = 0;
    		}
    		break;
    	case 3:
    		stCpuStat.stLogicAddr.ucModule = 16;
    		stCpuStat.stLogicAddr.usGroup = cpss_htons((UINT16)(2));
    		stCpuStat.stLogicAddr.ucSubGroup = 2;

    		stBrdPhyAddr.ucFrame = 1;
    		stBrdPhyAddr.ucShelf = 1;
    		stBrdPhyAddr.ucSlot = 2;
    		stBrdPhyAddr.ucCpu = 2;
    		if(g_brd_alarmstat[3] == 1)
    		{
    			ucAlarmClr = 1;
    			g_brd_alarmstat[3] = 0;
    		}
    		break;
    	case 4:
    		stCpuStat.stLogicAddr.ucModule = 16;
    		stCpuStat.stLogicAddr.usGroup = cpss_htons((UINT16)(2));
    		stCpuStat.stLogicAddr.ucSubGroup = 3;

    		stBrdPhyAddr.ucFrame = 1;
    		stBrdPhyAddr.ucShelf = 1;
    		stBrdPhyAddr.ucSlot = 2;
    		stBrdPhyAddr.ucCpu = 3;
    		if(g_brd_alarmstat[4] == 1)
    		{
    			ucAlarmClr = 1;
    			g_brd_alarmstat[4] = 0;
    		}
    		break;
    	}
#if 0 //dhwang added
    	if(ucAlarmClr == 1)
    	{

    		cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_BOARD_NOT_RESPONDED, OAMS_ALARM_CLEAR, (UINT8*)&stBrdPhyAddr, sizeof(stBrdPhyAddr));
    		cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
    		"cps send om lost heart beat alarm clear ! brd = %d-%d-%d-%d\n",stBrdPhyAddr.ucFrame, stBrdPhyAddr.ucShelf, stBrdPhyAddr.ucSlot, stBrdPhyAddr.ucCpu);
    	}
    	cps_send_om_event(OM_AM_CPS_EVENT_CODE_BOARD_STARTUP, (UINT8*)&stBrdPhyAddr, sizeof(stBrdPhyAddr));
    	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
    	"cps send om brd init finish ! brd = %d-%d-%d-%d\n",stBrdPhyAddr.ucFrame, stBrdPhyAddr.ucShelf, stBrdPhyAddr.ucSlot, stBrdPhyAddr.ucCpu);
#endif

  	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
		"\n****************************************\n* recieve PPU status change Msg !\n* PPU Logic Addr = <%d-%d-%d>, State = <%d>\n* now broadcast to All proc !\n****************************************",
		stCpuStat.stLogicAddr.ucModule, cpss_ntohs(stCpuStat.stLogicAddr.usGroup),
		stCpuStat.stLogicAddr.ucSubGroup, cpss_ntohl(stCpuStat.iState));

		cpss_com_send_broadcast(CPS_CPU_STAT_CHG_IND_MSG, (UINT8*)&stCpuStat, sizeof(CPS_CPU_STAT_CHG_IND_MSG_T));
         smss_sysctl_set_cps__rdbs_brdstatus(pstMsg->stPhyAddr,CPS__RDBS_DEV_VALUE_BOARD_NORMAL);
         smss_sysctrl_set_smss_boardstate(pstMsg->stPhyAddr,SMSS_BRD_RSTATUS_NORMAL);
    }
    else
    {
         smss_sysctl_set_cps__rdbs_brdstatus(pstMsg->stPhyAddr,CPS__RDBS_DEV_VALUE_BOARD_FAULT);
         smss_sysctrl_set_smss_boardstate(pstMsg->stPhyAddr,SMSS_BRD_RSTATUS_FAULT);
    }

    if ((SMSS_RSTATUS_NORMAL == pstBoardIndex->ucRStatus)
        &&(SMSS_RSTATUS_END == pstMsg->ucRStatus))
    {
        smss_send_alarm_brdnotresponded_to_oams(SMSS_ALMSUBNO_0, 1, pstMsg->stPhyAddr);
        smss_sysctl_sendbrdalarmind_tosysrt(pstMsg->stPhyAddr);
    }

    /* 设置该单板的状态 */
    pstBoardIndex->ucRStatus = pstMsg->ucRStatus;
    pstBoardIndex->ucAStatus = pstMsg->ucAStatus;
    
    /* 设置RDBS */
    smss_sysctl_set_cps__rdbs_rstatus(pstMsg->stPhyAddr, pstMsg->ucRStatus);
    smss_sysctl_set_cps__rdbs_astatus(pstMsg->stPhyAddr, pstMsg->ucAStatus);
    
    /* 如果外围板初始化完成，添入心跳列表 */
    if ((SMSS_RSTATUS_NORMAL == pstMsg->ucRStatus)
        && (TRUE != SMSS_ADDR_IS_EQUAL(pstMsg->stPhyAddr, CPSS_PHY_ADDR_GCPA_SLAVE))
        && FALSE == pstBoardIndex->bInList)
    {
        if( g_bRunGpHeartBeat== TRUE )
        {
            pstBoardIndex->ucBeatCount = 0;

            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                       "<%s-%s>: Add board (%d-%d-%d) to heartbeat list.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                       pstMsg->stPhyAddr.ucFrame, pstMsg->stPhyAddr.ucShelf, pstMsg->stPhyAddr.ucSlot);
            cpss_util_dl_list_add(&stHeartBeatList, 
                                    (CPSS_UTIL_DL_NODE_T *)pstBoardIndex);
            pstBoardIndex->bInList = TRUE;

            smss_show_hblist();
        }
    }
    else if (SMSS_RSTATUS_END == pstMsg->ucRStatus)
    /* 如果进行倒换，从心跳列表中删除 */
    {
        if (TRUE == pstBoardIndex->bInList)
        {
            /* 向OAMS上报告警 */
            smss_send_alarm_brdnotresponded_to_oams(SMSS_ALMSUBNO_0, 1, pstMsg->stPhyAddr);
            cpss_util_dl_list_remove(&stHeartBeatList, 
                (CPSS_UTIL_DL_NODE_T *)pstBoardIndex);
            pstBoardIndex->pstPrev = NULL;
            pstBoardIndex->pstNext = NULL;
            pstBoardIndex->bInList = FALSE;
        }

        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: Remove board (%d-%d-%d) from heartbeat list.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            pstBoardIndex->stPhyAddr.ucFrame, pstBoardIndex->stPhyAddr.ucShelf,
            pstBoardIndex->stPhyAddr.ucSlot);
    }

    return 0;
}

/***************************************************************************
* 函数名称: smss_sysctl_as_bswitching
* 功    能: 根据逻辑地址查某逻辑地址的单板是否正在进行OAMS发起的主备倒换。
* 函数类型  BOOL
* 参    数: 
* 参数名称      参数类型             输入/输出        参数描述
* pstLogic    CPSS_COM_LOGIC_ADDR_T*    IN            逻辑地址
* pstOut      SMSS_CPS__OAMS_FORCE_SWITCH_T**  OUT        在链表内的指针值
* 函数返回: 
*           TRUE:表示该逻辑地址的单板正在倒换 FALSE:表示未倒换
* 说    明: 无。
***************************************************************************/
BOOL smss_sysctl_as_bswitching(CPSS_COM_LOGIC_ADDR_T* pstLogic, 
                               SMSS_CPS__OAMS_FORCE_SWITCH_T ** pstOut)
{
    SMSS_CPS__OAMS_FORCE_SWITCH_T * pstNode = 
        (SMSS_CPS__OAMS_FORCE_SWITCH_T *)stSwitchList.pstHead;

    /* 获得倒换节点 */
    while (NULL != pstNode)
    { 
        if (TRUE == SMSS_ADDR_IS_EQUAL(*pstLogic, pstNode->stLogicAddr))   
        {
            break;
        }
        else
        {
            pstNode = pstNode->pstNext;
        }                
    }
    
    if (NULL == pstNode)  /* 若没查到该节点，则返回失败 */
    {
        return FALSE;
    }
    else
    {    
        if  (NULL != pstOut) /* 若需要输出，则进行输出 */ 
        {
            *pstOut = pstNode;             
        }        
        return TRUE;
    }
}
/***************************************************************************
* 函数名称: smss_sysctl_wait_bswitchind_timeout
* 功    能: 全局板向外围板发完倒换消息后等待其回应超时
* 函数类型  VOID
* 参    数: 
* 参数名称      参数类型       输入/输出        参数描述
* pstMsg    CPSS_TIMER_MSG_T *     IN           超时参数
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_wait_bswitchind_timeout(CPSS_TIMER_MSG_T * pstMsg)
{
    SMSS_CPS__OAMS_FORCE_SWITCH_T * pstSwitchNode = NULL;
    BOOL bNodeExist;
    CPS__OAMS_DYND_FORCE_HO_RSP_MSG_T stForceRsp;
    UINT32 ulSeqId = pstMsg->ulPara;
    
    bNodeExist = smss_sysctl_seqid2pnode(ulSeqId, &pstSwitchNode);
    if (TRUE == bNodeExist)  /* 有该节点存在 */
    {
        /* 向oams返回倒换失败 */
        stForceRsp.stRspHeader.ulSeqId = cpss_htonl(pstSwitchNode->ulSeqId);
        stForceRsp.stRspHeader.ulReturnCode = cpss_htonl(SMSS_SYSCTRL_ERR_BSWITCH_TIMEOUT);
        stForceRsp.stSrcPid = pstSwitchNode->stSrcPid;
        stForceRsp.bIsMainCtrlBrd = cpss_htonl(FALSE);
        smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC,
                              CPS__OAMS_DYND_FORCE_HO_RSP_MSG, 
                               (UINT8 *)&stForceRsp, sizeof(stForceRsp));
        
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: Board Switch timeout.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        
        /* 删除倒换节点并释放空间 */
        cpss_util_dl_list_remove(&stSwitchList, (CPSS_UTIL_DL_NODE_T *)pstSwitchNode);
        cpss_mem_free(pstSwitchNode);
        
    }
    else
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "<%s-%s>: Unknown SeqId %d Timeout.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            ulSeqId);
    }
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_idle_gpca_init
* 功    能: 实现GCPA特有的初始化
* 函数类型  VOID
* 参    数: 无。
* 函数返回: 
*           无。
* 说    明: 该函数在smss_sysctl_init()内被调用；
***************************************************************************/
VOID smss_sysctl_idle_gcpa_init(VOID)
{
    UINT32 ulBoardIndex;
    UINT32 ulSize = CPSS_COM_MAX_FRAME * CPSS_COM_MAX_SHELF
        * CPSS_COM_MAX_SLOT * sizeof(SMSS_BOARD_INFO_T *);
    
    /* 开辟指针空间 */
    apstBoardIndex = cpss_mem_malloc(ulSize);
    if (NULL == apstBoardIndex) /* 申请空间失败，则直接复位 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                   "<%s-%s>: malloc failed, size = %d.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   ulSize);
        smss_sysctl_enter_end(TRUE,
            SMSS_BRD_RST_CAUSE_ALLOC_MEM_FAILURE_GCPA_INIT);
        return;
    }
    cpss_mem_memset(apstBoardIndex, 0, ulSize);

    /* 初始化心跳链表 */
    cpss_util_dl_list_init(&stHeartBeatList);
    
    /* 初始化倒换链表 */
    cpss_util_dl_list_init(&stSwitchList);

    ulBoardIndex = Phy2BoardIndex(g_stSmssBaseInfo.stPhyAddr);
    if (NULL == apstBoardIndex[ulBoardIndex])
    {
        SMSS_BOARD_INFO_T * pstBoardPointer;

        /* 为主用全局板分配存储空间 */
        pstBoardPointer = (SMSS_BOARD_INFO_T *)cpss_mem_malloc(sizeof(SMSS_BOARD_INFO_T));
        if (NULL == pstBoardPointer) /* 申请内存失败，则直接复位 */
        {
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                       "<%s-%s>: malloc failed, size = %d.",
                       g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                       sizeof(CPS__RDBS_DEV_GETALLBRDINFO_MSG_RSP_T));
            smss_sysctl_enter_end(TRUE,
                                  SMSS_BRD_RST_CAUSE_ALLOC_MEM_FAILURE_GCPA_INIT);
            return;
        }

        cpss_mem_memset(pstBoardPointer, 0, sizeof(SMSS_BOARD_INFO_T));
        pstBoardPointer->ucFunBrdType = SWP_FUNBRD_TYPE;
        pstBoardPointer->ulPhyBrdType = SWP_PHYBRD_TYPE;
        apstBoardIndex[ulBoardIndex] = pstBoardPointer;
    }
    return;
}

/***************************************************************************
* 函数名称: smss_sysctrl_set_smss_boardstate
* 功    能: 设置全局单板上的smss存储的单板状态
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyaddr   CPSS_COM_PHY_ADDR_T             IN       设置单板的物理地址
* ucHardVerAlmFlag     UINT8                       IN       单板的硬件版本错误告警标志 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
INT32 smss_sysctrl_set_smss_boardstate(CPSS_COM_PHY_ADDR_T stPhyaddr, 
                                           UINT8 ucBoardState)
{
   UINT32 ulBoardIndex;
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
   ulBoardIndex = Phy2BoardIndex(stPhyaddr);
   pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
   if( pstBoardPointer == NULL)
   {
        apstBoardIndex[ulBoardIndex] = cpss_mem_malloc(sizeof(SMSS_BOARD_INFO_T));
        pstBoardPointer = apstBoardIndex[ulBoardIndex];
        if( pstBoardPointer == NULL )
        {
           return SMSS_ERROR;
        }
        cpss_mem_memset(pstBoardPointer,0,sizeof(SMSS_BOARD_INFO_T));

   }
   pstBoardPointer->ucBrdRStatus = ucBoardState;

   if(SMSS_BRD_RSTATUS_FAULT == ucBoardState)
   {
       if ((pstBoardPointer->ulPhyBrdType == SWP_PHYBRD_MEIA) || (pstBoardPointer->ulPhyBrdType == SWP_PHYBRD_MNPA))
       {
           CPSS_COM_PHY_ADDR_T stSlavePhyAdd;
           stSlavePhyAdd = stPhyaddr;
           stSlavePhyAdd.ucCpu = 2;
           smss_sysctl_gcpa_set_slave_rstatus(stSlavePhyAdd, SMSS_RSTATUS_END);
       }
   }
   return SMSS_OK;
}

/***************************************************************************
* 函数名称: smss_sysctl_set_cps__rdbs_brdstatus
* 功    能: 设置RDBS单板状态
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyaddr   CPSS_COM_PHY_ADDR_T             IN       设置单板的物理地址
* ucHardVerAlmFlag     UINT8                       IN       单板的硬件版本错误告警标志 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_set_cps__rdbs_brdstatus(CPSS_COM_PHY_ADDR_T stPhyAdd, UINT8 ucState)
{
    CPS__RDBS_DEV_SETBOARDSTATUS_MSG_REQ_T stSetBrdStateReq; 
    CPS__RDBS_DEV_SETBOARDSTATUS_MSG_RSP_T stSetBrdStateRsp;
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                   "<%s-%s>: SMSS Set RDBS Board(%d-%d-%d) State: %d",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   stPhyAdd.ucFrame,stPhyAdd.ucShelf,stPhyAdd.ucSlot,ucState);
    stSetBrdStateReq.stCpuPhyAddr = stPhyAdd;
    cpss_mem_memset(stSetBrdStateReq.aucRsv,0,3);
    stSetBrdStateReq.ucValue = ucState;
    cps__rdbs_if_entry(CPS__RDBS_DEV_SETBOARDSTATUS_MSG,  CPS__RDBS_IF_FUNC, 
        (UINT8 *)&stSetBrdStateReq, sizeof(stSetBrdStateReq),
        (UINT8 *)&stSetBrdStateRsp, sizeof(stSetBrdStateRsp)); 
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_gcpa_recv_brd_online_chg
* 功    能: 处理交换板上的机框管理功能上报的单板状态改变消息
* 函数类型  VOID 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* pstMsg   SMSS_SYSCTL_BRD_OL_CHG_IND_MSG_T*   IN       指向状态改变消息的指针
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_recv_brd_online_chg(SMSS_SYSCTL_BRD_OL_CHG_IND_MSG_T* pstMsg)
{
    if (NULL == pstMsg)    /* 消息合法性判断 */
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: smss_sysctl_gcpa_recv_brd_online_chg: Receive NULL Message.",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        return;
    }
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "<%s-%s>: Receive SMSS_SYSCTL_BRD_OL_CHG_IND_MSG: Frame = %d, Shelf = %d, Slot = %d, State = %d\n",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
        pstMsg->stPhyAddr.ucFrame,pstMsg->stPhyAddr.ucShelf,pstMsg->stPhyAddr.ucSlot,
        pstMsg->ucCurOLStatus);
    
    if (SMSS_BOARD_OFFLINE == pstMsg->ucCurOLStatus)
        /* 如果是由在线变为离线，则需要进行一些清除操作 */
    {
        UINT32 ulBrdIndex;
        SMSS_BOARD_INFO_T * pstBrdPointer = NULL;
     
        ulBrdIndex = Phy2BoardIndex(pstMsg->stPhyAddr);
        pstBrdPointer = apstBoardIndex[ulBrdIndex];
        if (NULL != pstBrdPointer)   /* 若该单板已经注册过 */
        {
        	  if (1 == g_ulSmssOnlineFlag)
        	  {
              /* 设置RDBS */
              smss_sysctl_set_cps__rdbs_astatus(pstMsg->stPhyAddr, SMSS_STANDBY);
              smss_sysctl_set_cps__rdbs_rstatus(pstMsg->stPhyAddr, SMSS_RSTATUS_END);
            }

            /* 避免重复告警 */
            if (pstBrdPointer->ucBrdRStatus != SMSS_BRD_RSTATUS_OFFLINE)
            {
               /* 向OAMS上报告警 */
               smss_send_alarm_brdnotinstalled_to_oams(SMSS_ALMSUBNO_0,pstMsg->ucCurOLStatus,pstMsg->stPhyAddr);	
            }

           if (1 == g_ulSmssOnlineFlag)
           {
            smss_sysctl_set_cps__rdbs_brdstatus(pstMsg->stPhyAddr,CPS__RDBS_DEV_VALUE_BOARD_OFFLINE);
            smss_sysctrl_set_smss_boardstate(pstMsg->stPhyAddr,SMSS_BRD_RSTATUS_OFFLINE);

            if (TRUE == pstBrdPointer->bInList)
                /* 将该单板从心跳列表内删除 */
            {
                /* 向OAMS上报告警 */
                smss_send_alarm_brdnotresponded_to_oams(SMSS_ALMSUBNO_0, 1, pstMsg->stPhyAddr);
                cpss_util_dl_list_remove(&stHeartBeatList, 
                    (CPSS_UTIL_DL_NODE_T*)pstBrdPointer);
                pstBrdPointer->pstNext = NULL;
                pstBrdPointer->pstPrev = NULL;
                pstBrdPointer->bInList = FALSE;
            }
            /* 设置单板的状态 */
            pstBrdPointer->ucRStatus = SMSS_RSTATUS_END;
            pstBrdPointer->ucAStatus = SMSS_STANDBY;
           } 

            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
               "GCPA (A) receive (%d-%d-%d) offline",
               pstMsg->stPhyAddr.ucFrame,pstMsg->stPhyAddr.ucShelf, 
               pstMsg->stPhyAddr.ucSlot);  
       }
     
    }
    else if (SMSS_BOARD_ONLINE == pstMsg->ucCurOLStatus) /* 在线, 上告警解除 */
    {
    	 smss_send_alarm_brdnotinstalled_to_oams(SMSS_ALMSUBNO_0,pstMsg->ucCurOLStatus,pstMsg->stPhyAddr);	 
    }

    return;
}

VOID smss_sysctl_gcpa_stoa_complete(VOID)
{
    UINT32 ulIndex = 0;
    
     if (smss_get_gcpa_upgstatus() == SMSS_UPGSTATUS_UPGRADING)
     {
      smss_sysctl_set_cps__rdbs_rstatus(g_stSmssAsInfo.stMatePhyAddr, SMSS_RSTATUS_NORMAL);              
     }
     else
     {
    smss_sysctl_set_cps__rdbs_rstatus(g_stSmssAsInfo.stMatePhyAddr, SMSS_RSTATUS_END);
     }
    
    smss_sysctl_set_cps__rdbs_astatus(g_stSmssAsInfo.stMatePhyAddr, SMSS_STANDBY);
    smss_sysctl_set_cps__rdbs_rstatus(g_stSmssBaseInfo.stPhyAddr, SMSS_RSTATUS_NORMAL);
    smss_sysctl_set_cps__rdbs_astatus(g_stSmssBaseInfo.stPhyAddr, SMSS_ACTIVE); 
    smss_sysctl_set_cps__rdbs_brdstatus(g_stSmssAsInfo.stMatePhyAddr,CPS__RDBS_DEV_VALUE_BOARD_FAULT);
    smss_sysctrl_set_smss_boardstate(g_stSmssAsInfo.stMatePhyAddr,SMSS_BRD_RSTATUS_FAULT);        
    smss_sysctl_set_cps__rdbs_brdstatus(g_stSmssBaseInfo.stPhyAddr,CPS__RDBS_DEV_VALUE_BOARD_NORMAL);
    smss_sysctrl_set_smss_boardstate(g_stSmssBaseInfo.stPhyAddr,SMSS_BRD_RSTATUS_NORMAL);        

    ulIndex = Phy2BoardIndex(g_stSmssAsInfo.stMatePhyAddr);
    if (NULL != apstBoardIndex[ulIndex])
    {
        apstBoardIndex[ulIndex]->ucRStatus = SMSS_RSTATUS_END;
        apstBoardIndex[ulIndex]->ucAStatus = SMSS_STANDBY;
    }
    ulIndex = Phy2BoardIndex(g_stSmssBaseInfo.stPhyAddr);
    if (NULL != apstBoardIndex[ulIndex])
    {
        apstBoardIndex[ulIndex]->ucRStatus = SMSS_RSTATUS_NORMAL;
        apstBoardIndex[ulIndex]->ucAStatus = SMSS_ACTIVE;
    }
}

/***************************************************************************
* 函数名称: smss_get_board_info
* 功    能: 获取指定单板的状态信息结构指针
* 函数类型: SMSS_BOARD_INFO_T *
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyAddr   CPSS_COM_PHY_ADDR_T              IN       指定单板的物理地址
* 函数返回: 物理地址非法时返回NULL，该单板未配置或未上电时也返回NULL。
* 说    明: 无。
***************************************************************************/
SMSS_BOARD_INFO_T * smss_get_board_info(CPSS_COM_PHY_ADDR_T stPhyAddr)
{
    if (0 == stPhyAddr.ucFrame || stPhyAddr.ucFrame > CPSS_COM_MAX_FRAME
        || 0 == stPhyAddr.ucShelf || stPhyAddr.ucShelf > CPSS_COM_MAX_SHELF
        || 0 == stPhyAddr.ucSlot  || stPhyAddr.ucSlot > CPSS_COM_MAX_SLOT)
    {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "<%s-%s>: smss_get_board_info: Invalid PhyAddr `%d-%d-%d-%d'.\n",
                   g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                   stPhyAddr.ucFrame, stPhyAddr.ucShelf,
                   stPhyAddr.ucSlot, stPhyAddr.ucCpu);
        return NULL;
    }
    return apstBoardIndex[(stPhyAddr.ucFrame - 1) * CPSS_COM_MAX_SHELF*CPSS_COM_MAX_SLOT
                          + (stPhyAddr.ucShelf - 1)*CPSS_COM_MAX_SLOT
                          + (stPhyAddr.ucSlot - 1)];
}

/***************************************************************************
* 函数名称: smss_sysctl_gcpa_show_board_info
* 功    能: 显示全局板上保存的单板的状态信息
* 函数类型  INT32 
* 参    数: 
* 参数名称   参数类型  输入/输出      参数描述
* ulPhyAddr   UINT32     IN        指定单板的物理地址，从最高位到最低位依次
*                                  为架框槽CPU号（CPU号不使用）。
* 函数返回: 
*           无。
* 说    明: 该函数用于调试
***************************************************************************/
INT32 smss_sysctl_gcpa_show_board_info(UINT32 ulPhyAddr)
{
    CPSS_COM_PHY_ADDR_T stPhy;
    UINT32 ulBoardIndex;
    SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
    ulPhyAddr = cpss_ntohl(ulPhyAddr);
    stPhy = *(CPSS_COM_PHY_ADDR_T *)&ulPhyAddr;
    stPhy.ucCpu = 1;
    
    ulBoardIndex = Phy2BoardIndex(stPhy);
    pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
    if (NULL == pstBoardPointer)
    {
        cps__oams_shcmd_printf("SMSS_SYSCTL: Board (%d-%d-%d) not exist.\n",
               stPhy.ucFrame, stPhy.ucShelf, stPhy.ucSlot);
        return CPS__OAMS_OK;
    }
 
    cps__oams_shcmd_printf("  PhyAddr: %d-%d-%d-%d\n",
           stPhy.ucFrame, stPhy.ucShelf, stPhy.ucSlot, stPhy.ucCpu);
 
    cps__oams_shcmd_printf("  MStatus: %d\n", pstBoardPointer->ucMStatus);
    cps__oams_shcmd_printf("  AStatus: %d\n", pstBoardPointer->ucAStatus);
    cps__oams_shcmd_printf("  RStatus: %d\n", pstBoardPointer->ucRStatus);
    return CPS__OAMS_OK;
}

INT32 smss_showbrd(void)
{
   UINT32 ulSptBrdMaxNum = CPSS_COM_MAX_FRAME * CPSS_COM_MAX_SHELF * CPSS_COM_MAX_SLOT;
   UINT32 ulBrdindex = 0;
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;

   cps__oams_shcmd_printf("\nFrame   Shelf    Slot     RStatus      AStatus    MStatus  PhyBrd  FunBrd  BrdRStatus\n");
   for (ulBrdindex = 0;ulBrdindex < ulSptBrdMaxNum ; ulBrdindex ++ )
   {
        pstBoardPointer = apstBoardIndex[ulBrdindex];
        if( pstBoardPointer == NULL )
          continue;
        cps__oams_shcmd_printf("%5d%8d%8d%12s%13s%10d%8d%8d%12d\n",
                    pstBoardPointer->stPhyAddr.ucFrame,
                    pstBoardPointer->stPhyAddr.ucShelf,
                    pstBoardPointer->stPhyAddr.ucSlot,
                    (SMSS_RSTATUS_NORMAL == pstBoardPointer->ucRStatus ? "NORMAL" : "FAULT"),
                    (SMSS_ACTIVE == pstBoardPointer->ucAStatus ? "ACTIVE" : "STANDBY"),
                    pstBoardPointer->ucMStatus,
                    pstBoardPointer->ulPhyBrdType,
                    pstBoardPointer->ucFunBrdType,
                    pstBoardPointer->ucBrdRStatus);
   }
 
   return CPS__OAMS_OK;
}

/***************************************************************************
* 函数名称: smss_sysctrl_get_brd_state_byphy
* 功    能: 根据物理地址获取全局单板上的smss存储的单板的运行状态，主备状态及管理状态
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyaddr   CPSS_COM_PHY_ADDR_T            IN       单板的物理地址
* pucAStatus     UINT8*                      OUT      单板的主备状态 
* pucRStatus     UINT8*                      OUT      单板的运行状态 
* pucMStatus     UINT8*                      OUT      单板的管理状态 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
INT32 smss_sysctrl_get_brd_state_byphy(CPSS_COM_PHY_ADDR_T  stPhyaddr,
                                      UINT8 *pucAStatus,
                                      UINT8 *pucRStatus,
                                      UINT8 *pucMStatus)
{
   UINT32 ulBoardIndex;
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
   ulBoardIndex = Phy2BoardIndex(stPhyaddr);
   pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
   if( pstBoardPointer != NULL )
   {
       *pucAStatus  = pstBoardPointer->ucAStatus;
       *pucRStatus  = pstBoardPointer->ucRStatus;
       *pucMStatus  = pstBoardPointer->ucMStatus;
       return SMSS_OK;
   }
   return SMSS_ERROR;
}
/***************************************************************************
* 函数名称: smss_sysctrl_set_smss_rstatus
* 功    能: 设置全局单板上的smss存储的单板的运行状态
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyaddr   CPSS_COM_PHY_ADDR_T             IN       设置单板的物理地址
* ucValue     UINT8                           IN       单板的运行状态 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
INT32 smss_sysctrl_set_smss_rstatus(CPSS_COM_PHY_ADDR_T stPhyaddr, UINT8 ucValue)
{
   UINT32 ulBoardIndex;
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
   ulBoardIndex = Phy2BoardIndex(stPhyaddr);
   pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
   if( pstBoardPointer == NULL)
   {
        apstBoardIndex[ulBoardIndex] = cpss_mem_malloc(sizeof(SMSS_BOARD_INFO_T));
        pstBoardPointer = apstBoardIndex[ulBoardIndex];
        if( pstBoardPointer == NULL )
        {
           return SMSS_ERROR;
        }
        cpss_mem_memset(pstBoardPointer,0,sizeof(SMSS_BOARD_INFO_T));

   }
   pstBoardPointer->stPhyAddr = stPhyaddr;
   pstBoardPointer->ucRStatus = ucValue;
   return SMSS_OK;
}
/***************************************************************************
* 函数名称: smss_sysctrl_set_smss_astatus
* 功    能: 设置全局单板上的smss存储的单板的主备状态
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyaddr   CPSS_COM_PHY_ADDR_T             IN       设置单板的物理地址
* ucValue     UINT8                           IN       单板的主备状态 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
INT32 smss_sysctrl_set_smss_astatus(CPSS_COM_PHY_ADDR_T stPhyaddr, UINT8 ucValue)
{
   UINT32 ulBoardIndex;
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
   ulBoardIndex = Phy2BoardIndex(stPhyaddr);
   pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
   if( pstBoardPointer == NULL)
   {
        apstBoardIndex[ulBoardIndex] = cpss_mem_malloc(sizeof(SMSS_BOARD_INFO_T));
        pstBoardPointer = apstBoardIndex[ulBoardIndex];
        if( pstBoardPointer == NULL )
        {
           return SMSS_ERROR;
        }
        cpss_mem_memset(pstBoardPointer,0,sizeof(SMSS_BOARD_INFO_T));
   }
   pstBoardPointer->stPhyAddr = stPhyaddr;
   pstBoardPointer->ucAStatus = ucValue;
   return SMSS_OK;
}
/***************************************************************************
* 函数名称: smss_sycstrl_set_smss_astatus
* 功    能: 设置全局单板上的smss存储的单板的管理状态
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyaddr   CPSS_COM_PHY_ADDR_T             IN       设置单板的物理地址
* ucValue     UINT8                           IN       单板的管理状态 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
INT32 smss_sysctrl_set_smss_mstatus(CPSS_COM_PHY_ADDR_T stPhyaddr, UINT8 ucValue)
{
   UINT32 ulBoardIndex;
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
   ulBoardIndex = Phy2BoardIndex(stPhyaddr);
   pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
   if( pstBoardPointer == NULL)
   {
        apstBoardIndex[ulBoardIndex] = cpss_mem_malloc(sizeof(SMSS_BOARD_INFO_T));
        pstBoardPointer = apstBoardIndex[ulBoardIndex];
        if( pstBoardPointer == NULL )
        {
           return SMSS_ERROR;
        }
        cpss_mem_memset(pstBoardPointer,0,sizeof(SMSS_BOARD_INFO_T));
   }
   pstBoardPointer->stPhyAddr = stPhyaddr;
   pstBoardPointer->ucMStatus = ucValue;
   return SMSS_OK;
}

/***************************************************************************
* 函数名称: smss_sysctl_get_phybrdtype
* 功    能: 获取全局单板上的smss存储的单板的物理类型
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyaddr   CPSS_COM_PHY_ADDR_T             IN       设置单板的物理地址
* *ulPhyBrdType     UINT32*                   out       单板的物理类型
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
INT32 smss_sysctl_get_phybrdtype(CPSS_COM_PHY_ADDR_T stPhyaddr,UINT32 *ulPhyBrdType)
{
   UINT32 ulBoardIndex;
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
    *ulPhyBrdType = 0;
   ulBoardIndex = Phy2BoardIndex(stPhyaddr);
   pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
   if( pstBoardPointer != NULL )
   {
       *ulPhyBrdType  = pstBoardPointer->ulPhyBrdType;
       return SMSS_OK;
   }
   return SMSS_ERROR;
}

/***************************************************************************
* 函数名称: smss_show_hblist
* 功    能: 用来显示全局单板上的心跳列表
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
INT32 smss_show_hblist(VOID)
{   
    SMSS_BOARD_INFO_T * pstNode;
    pstNode = (SMSS_BOARD_INFO_T *)stHeartBeatList.pstHead;
    cps__oams_shcmd_printf("\nFrame   Shelf    Slot     RStatus      AStatus    MStatus  BeatCount bInList\n");
    while (NULL != pstNode)
    {
        cps__oams_shcmd_printf("%5d%8d%8d%12s%13s%10d%10d%10s\n",
            pstNode->stPhyAddr.ucFrame,
            pstNode->stPhyAddr.ucShelf,
            pstNode->stPhyAddr.ucSlot,
            (SMSS_RSTATUS_NORMAL == pstNode->ucRStatus ? "NORMAL" : "FAULT"),
            (SMSS_ACTIVE == pstNode->ucAStatus ? "ACTIVE" : "STANDBY"),
            pstNode->ucMStatus,
            pstNode->ucBeatCount,
            (pstNode->bInList == FALSE ? "FALSE" : "TRUE")
            );
        pstNode = pstNode->pstNext;
    }
    return CPS__OAMS_OK;
}

/***************************************************************************
* 函数名称: smss_sysctl_rsp_after_brdrst
* 功    能: 单板复位后的后续操作
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                     输入/输出       描述
* stPhy       CPSS_COM_PHY_ADDR_T             IN       被复位对象的物理地址 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_rsp_after_brdrst(CPSS_COM_PHY_ADDR_T stPhy)
{
    UINT32 ulIndex;
    smss_sysctl_set_cps__rdbs_astatus(stPhy, SMSS_STANDBY);
    smss_sysctl_set_cps__rdbs_rstatus(stPhy, SMSS_RSTATUS_END);
    smss_sysctl_set_cps__rdbs_brdstatus(stPhy,CPS__RDBS_DEV_VALUE_BOARD_FAULT);
    smss_sysctrl_set_smss_boardstate(stPhy,SMSS_BRD_RSTATUS_FAULT);
    
    ulIndex = Phy2BoardIndex(stPhy);
    if (NULL != apstBoardIndex[ulIndex])
    {
        if (TRUE == apstBoardIndex[ulIndex]->bInList)                
        {
            /* 向OAMS上报告警 */
            smss_send_alarm_brdnotresponded_to_oams(SMSS_ALMSUBNO_0, 1, stPhy);

            cpss_util_dl_list_remove(&stHeartBeatList, 
                (CPSS_UTIL_DL_NODE_T *)apstBoardIndex[ulIndex]);
            apstBoardIndex[ulIndex]->pstNext = NULL;
            apstBoardIndex[ulIndex]->pstPrev = NULL;
            apstBoardIndex[ulIndex]->bInList = FALSE;
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
                "<%s-%s>: Remove board (%d-%d-%d) from heartbeat list.",
                g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                apstBoardIndex[ulIndex]->stPhyAddr.ucFrame, 
                apstBoardIndex[ulIndex]->stPhyAddr.ucShelf, 
                apstBoardIndex[ulIndex]->stPhyAddr.ucSlot );              
        }
        apstBoardIndex[ulIndex]->ucRStatus = SMSS_RSTATUS_END;
        apstBoardIndex[ulIndex]->ucAStatus = SMSS_STANDBY;
    }
}       

/***************************************************************************
* 函数名称: smss_sysctl_get_actnormaltcsa
* 功    能: 判断单板是否主用正常交换板
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                     输入/输出       描述
* pstPhy       CPSS_COM_PHY_ADDR_T            IN       需要判断的单板物理地址 
* 函数返回: 
*           SMSS_OK/SMSS_ERROR
* 说    明: 
***************************************************************************/
INT32 smss_sysctl_get_actnormaltcsa(CPSS_COM_PHY_ADDR_T *pstPhy)
{
    UINT32 ulBoardIndex;
    SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
    
    pstPhy->ucSlot = 7;
    ulBoardIndex = Phy2BoardIndex(*pstPhy);
    pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
    if( pstBoardPointer != NULL)
    {
        if ((pstBoardPointer->ucBrdRStatus == SMSS_BRD_RSTATUS_NORMAL) 
            && (SMSS_ACTIVE == pstBoardPointer->ucAStatus))
        {
            return SMSS_OK; 
        }
    }
    
    pstPhy->ucSlot = 8;
    ulBoardIndex = Phy2BoardIndex(*pstPhy);
    pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
    if( pstBoardPointer != NULL)
    {
        if ((pstBoardPointer->ucBrdRStatus == SMSS_BRD_RSTATUS_NORMAL) 
            && (SMSS_ACTIVE == pstBoardPointer->ucAStatus))
        {
            return SMSS_OK; 
        }
    }
    return SMSS_ERROR;
}

/***************************************************************************
* 函数名称: smss_sysctl_shelf_force_reset_rsp
* 功    能: 在全局板上的强制复位整框处理
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                     输入/输出       描述
* *pstMid       SWP_MO_ID_T                   IN       要求复位对象的MOID 
* ulDelayTime   UINT32                        IN                      
* ulSeqId       UINT32                        IN
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_shelf_force_reset_rsp(SWP_MO_ID_T *pstMid,UINT32 ulDelayTime,UINT32  ulSeqId)
{
    SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG_T stRst;           /* 向交换板发送的复位请求 */
    CPS__OAMS_DYND_FORCE_RST_RSP_MSG_T stRsp;                 /* 向OAMS发送的复位响应 */
    CPSS_COM_PHY_ADDR_T stPhy;
    INT32  lRet;    
    
    stPhy.ucFrame = pstMid->unMOI.stShelf.ucFrameNo;
    stPhy.ucShelf = pstMid->unMOI.stShelf.ucShelfNo;
    stPhy.ucCpu = 1;    
    
    /* 获取正常主用交换板失败，则向OAMS响应失败 */
    if (smss_sysctl_get_actnormaltcsa(&stPhy) != SMSS_OK) 
    {
        stRsp.ulSeqId = cpss_htonl(ulSeqId);
        stRsp.ulReturnCode = cpss_htonl(SMSS_SYSCTRL_ERR_INVALID_MO);
        smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_RST_RSP_MSG, 
            (UINT8 *)&stRsp, sizeof(stRsp));
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "<%s-%s>: smss_sysctl_get_actnormaltcsa failed.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        return;
    }
    
    stRst.ulSeqId = cpss_htonl(ulSeqId);
    stRst.stPhyAddr = stPhy; 
    stRst.ulResetType = cpss_htonl(SMSS_FORCE_RST_ALL_SHELF);
    stRst.ulDelayTime = cpss_htonl(ulDelayTime);
    stRst.ulResetReason = cpss_htonl(0);


    /* 向相应的交换板发强制复位消息，发送失败向OAMS回失败响应 */
    lRet = smss_com_send_phy(stPhy,SMSS_SYSCTL_PROC,
        SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG,(UINT8 *)&stRst, sizeof(stRst));

    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
        "<%s-%s>: Send reset shell to %d-%d-%d. lRet=%d",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
        stPhy.ucFrame, stPhy.ucShelf, stPhy.ucSlot, lRet);

    /* 在发送失败或者非延迟复位时，直接向OAMS回响应 */
 
    /*if ((SMSS_OK != lRet) || (0 == ulDelayTime))*/
    {
        /* 向OAMS回失败应答 */
        stRsp.ulSeqId = cpss_htonl(ulSeqId);
        stRsp.ulReturnCode = cpss_htonl(lRet);
        smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_RST_RSP_MSG, 
                               (UINT8 *)&stRsp, sizeof(stRsp));

        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "<%s-%s>: Send CPS__OAMS_DYND_FORCE_RST_RSP_MSG to OAMS.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);

        return;
    }
}

/***************************************************************************
* 函数名称: smss_sysctl_local_force_reset_rsp
* 功    能: 在全局板上的强制复位本板处理
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                     输入/输出       描述
* ulDelayTime   UINT32                        IN                      
* ulSeqId       UINT32                        IN
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_local_force_reset_rsp(UINT32 ulDelayTime,UINT32  ulSeqId)
{
    CPS__OAMS_DYND_FORCE_RST_RSP_MSG_T stRsp;                 /* 向OAMS发送的复位响应 */
    INT32  lRet;    

    if (0 == ulDelayTime)
    {
        lRet = SMSS_OK;
        /* 向OAMS回应答 */
        stRsp.ulSeqId = cpss_htonl(ulSeqId);
        stRsp.ulReturnCode = cpss_htonl(lRet);
        smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_RST_RSP_MSG, 
            (UINT8 *)&stRsp, sizeof(stRsp));    
        /* 延迟3秒后复位 */ 
        lRet = cpss_timer_set(SMSS_TIMER_BRD_RST_TIMER, 3 * 1000);        
        return;
    }

    lRet = cpss_timer_set(SMSS_TIMER_BRD_RST_TIMER, ulDelayTime*1000);        
     
    /* 向OAMS回应答 */
    stRsp.ulSeqId = cpss_htonl(ulSeqId);
    stRsp.ulReturnCode = cpss_htonl(lRet);
    smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_RST_RSP_MSG, 
        (UINT8 *)&stRsp, sizeof(stRsp));

    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_board_force_reset_rsp
* 功    能: 在全局板上的强制复位其他单板处理
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                     输入/输出       描述
* *pstMid       SWP_MO_ID_T                   IN       要求复位对象的MOID 
* ulDelayTime   UINT32                        IN                      
* ulSeqId       UINT32                        IN
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_board_force_reset_rsp(SWP_MO_ID_T *pstMid,UINT32 ulDelayTime,UINT32  ulSeqId)
{
    SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG_T stRst;           /* 向交换板发送的复位请求 */
    CPS__OAMS_DYND_FORCE_RST_RSP_MSG_T stRsp;                 /* 向OAMS发送的复位响应 */
    CPSS_COM_PHY_ADDR_T stPhy;
    UINT8  ucAStatus;
    UINT8  ucRStatus;
    UINT8  ucMStatus;
    INT32  lRet;    

    stPhy.ucFrame = pstMid->unMOI.stBoard.ucFrameNo;
    stPhy.ucShelf = pstMid->unMOI.stBoard.ucShelfNo;
    stPhy.ucSlot = pstMid->unMOI.stBoard.ucSlotNo;  
    stPhy.ucCpu = 1;    

     /* 设置单板版本状态为复位中态"SMSS_VERM_BOARD_VERSTATE_RESET"*/
     smss_verm_set_gcpa_cps__rdbs_verstate(stPhy, SMSS_VERM_BOARD_VERSTATE_RESET);

    /* 根据物理地址获取要复位的单板状态 */ 
    lRet = smss_sysctrl_get_brd_state_byphy(stPhy, &ucAStatus, &ucRStatus, &ucMStatus);
    /* 如果获取失败或单板状态为故障,直接向OAMS回响应 */
    if (SMSS_OK != lRet)
    {
        /* 向OAMS回失败应答 */
        stRsp.ulSeqId = cpss_htonl(ulSeqId);
        stRsp.ulReturnCode = cpss_htonl(SMSS_SYSCTRL_ERR_INVALID_MO);
        smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_RST_RSP_MSG, 
            (UINT8 *)&stRsp, sizeof(stRsp));
        return;
    }
    else if(SMSS_RSTATUS_NORMAL != ucRStatus)
    {
        /* 向OAMS回失败应答 */
        stRsp.ulSeqId = cpss_htonl(ulSeqId);
        stRsp.ulReturnCode = cpss_htonl(SMSS_SYSCTRL_ERR_BSWITCH_STAGE_FORBID);
        smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_RST_RSP_MSG, 
            (UINT8 *)&stRsp, sizeof(stRsp));
        return;
    }
    
    /* 填写向单板发送的复位请求消息 */
    stRst.ulSeqId = cpss_htonl(ulSeqId);
    stRst.stPhyAddr = stPhy; 
    stRst.ulResetType = cpss_htonl(SMSS_FORCE_RST_SPC_SLOT);
    stRst.ulDelayTime = cpss_htonl(ulDelayTime);
    stRst.ulResetReason = cpss_htonl(0);


    /* 向要复位的单板发强制复位消息，发送失败向OAMS回失败响应 */
    lRet = smss_com_send_phy(stPhy, SMSS_SYSCTL_PROC, SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG, (UINT8 *)&stRst, sizeof(stRst));
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
        "<%s-%s>: Send SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG to BOARD. lRet=%d",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,lRet);

    /* 在请求消息发送失败或非延迟复位时，向OAMS发送响应消息 */

    /*if ((SMSS_OK != lRet) || (0 == ulDelayTime))*/

    {
        /* 向OAMS回应答 */
        stRsp.ulSeqId = cpss_htonl(ulSeqId);
        stRsp.ulReturnCode = cpss_htonl(lRet);
        smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_RST_RSP_MSG, 
                               (UINT8 *)&stRsp, sizeof(stRsp));
        return;
    }
}

/***************************************************************************
* 函数名称: smss_sysctl_gcpa_recv_force_rst_rsp
* 功    能: 全局板收到外围板强制复位响应消息的处理
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                     输入/输出       描述
* *pstMsg  SMSS_SYSCTL_FORCE_RST_CMD_RSP_MSG_T  IN  收到的外围板强制复位响应 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_recv_force_rst_rsp(SMSS_SYSCTL_FORCE_RST_CMD_RSP_MSG_T *pstMsg)
{
    CPS__OAMS_DYND_FORCE_RST_RSP_MSG_T stRstRsp;

    stRstRsp.ulSeqId = pstMsg->ulSeqId;
    stRstRsp.ulReturnCode = pstMsg->ulReturnCode;
    smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_RST_RSP_MSG, 
        (UINT8 *)&stRstRsp, sizeof(stRstRsp));
}
/***************************************************************************
* 函数名称: smss_sysctrl_set_smss_phyver
* 功    能: 设置全局单板上的smss存储的单板的集成信息
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyaddr     CPSS_COM_PHY_ADDR_T             IN       设置单板的物理地址
* stPhyBoardVer SWP_PHYBRD_INTEGRATED_INFO_VER_T                           IN       单板的运行状态 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
INT32 smss_sysctrl_set_smss_phyver(CPSS_COM_PHY_ADDR_T stPhyaddr, 
                                   SWP_PHYBRD_INTEGRATED_INFO_VER_T stPhyBoardVer)
{
   UINT32 ulBoardIndex;
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
   ulBoardIndex = Phy2BoardIndex(stPhyaddr);
   pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
   if( pstBoardPointer == NULL)
   {
        apstBoardIndex[ulBoardIndex] = cpss_mem_malloc(sizeof(SMSS_BOARD_INFO_T));
        pstBoardPointer = apstBoardIndex[ulBoardIndex];
        if( pstBoardPointer == NULL )
        {
           return SMSS_ERROR;
        }
        cpss_mem_memset(pstBoardPointer,0,sizeof(SMSS_BOARD_INFO_T));

   }
   pstBoardPointer->stPhyAddr = stPhyaddr;
   pstBoardPointer->stPhyBoardVer = stPhyBoardVer;
   return SMSS_OK;
}
/***************************************************************************
* 函数名称: smss_sysctrl_set_smss_funbrd
* 功    能: 设置全局单板上的smss存储的单板功能类型
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyaddr   CPSS_COM_PHY_ADDR_T             IN       设置单板的物理地址
* ucFuncBoard     UINT8                       IN       单板的功能类型 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
INT32 smss_sysctrl_set_smss_funbrd(CPSS_COM_PHY_ADDR_T stPhyaddr, 
                                   UINT8 ucFuncBoard)
{
   UINT32 ulBoardIndex;
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
   ulBoardIndex = Phy2BoardIndex(stPhyaddr);
   pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
   if( pstBoardPointer == NULL)
   {
        apstBoardIndex[ulBoardIndex] = cpss_mem_malloc(sizeof(SMSS_BOARD_INFO_T));
        pstBoardPointer = apstBoardIndex[ulBoardIndex];
        if( pstBoardPointer == NULL )
        {
           return SMSS_ERROR;
        }
        cpss_mem_memset(pstBoardPointer,0,sizeof(SMSS_BOARD_INFO_T));

   }
   pstBoardPointer->stPhyAddr = stPhyaddr;
   pstBoardPointer->ucFunBrdType = ucFuncBoard;
   return SMSS_OK;
}

/***************************************************************************
* 函数名称: smss_sysctrl_get_smss_funbrd
* 功    能: 获取全局单板上的smss存储的单板的集成信息及功能单板
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyaddr   CPSS_COM_PHY_ADDR_T             IN       设置单板的物理地址
* pstBrdInfo  SWP_PHYBRD_INTEGRATED_INFO_VER_T   OUT       单板的集成信息
* 函数返回: 
*           功能单板类型
* 说    明: 
***************************************************************************/
UINT8 smss_sysctrl_get_smss_funbrd(CPSS_COM_PHY_ADDR_T stPhyaddr,
                                    SWP_PHYBRD_INTEGRATED_INFO_VER_T *pstBrdInfo)
{
   UINT32 ulBoardIndex;
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
   ulBoardIndex = Phy2BoardIndex(stPhyaddr);
   pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
   if( pstBoardPointer == NULL)
   {
           return SMSS_ERROR;
   }
   *pstBrdInfo = pstBoardPointer->stPhyBoardVer;
   return pstBoardPointer->ucFunBrdType;
}

/***************************************************************************
* 函数名称: smss_sysctrl_set_smss_hardveralmflag
* 功    能: 设置全局单板上的smss存储的单板硬件版本错误告警标志
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyaddr   CPSS_COM_PHY_ADDR_T             IN       设置单板的物理地址
* ucHardVerAlmFlag     UINT8                       IN       单板的硬件版本错误告警标志 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
INT32 smss_sysctrl_set_smss_hardveralmflag(CPSS_COM_PHY_ADDR_T stPhyaddr, 
                                           UINT8 ucHardVerAlmFlag)
{
   UINT32 ulBoardIndex;
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
   ulBoardIndex = Phy2BoardIndex(stPhyaddr);
   pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
   if( pstBoardPointer == NULL)
   {
        apstBoardIndex[ulBoardIndex] = cpss_mem_malloc(sizeof(SMSS_BOARD_INFO_T));
        pstBoardPointer = apstBoardIndex[ulBoardIndex];
        if( pstBoardPointer == NULL )
        {
           return SMSS_ERROR;
        }
        cpss_mem_memset(pstBoardPointer,0,sizeof(SMSS_BOARD_INFO_T));

   }
   pstBoardPointer->ucHardVerAlmFlag = ucHardVerAlmFlag;
   return SMSS_OK;
}

/***************************************************************************
* 函数名称: smss_sysctrl_get_smss_hardveralmflag
* 功    能: 获取全局单板上的smss存储的单板的硬件版本错误告警标志
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyaddr   CPSS_COM_PHY_ADDR_T             IN       设置单板的物理地址
* pucHardVerAlmFlag                        UINT8   OUT       单板硬件版本错误告警标志
* 函数返回: 
*           功能单板类型
* 说    明: 
***************************************************************************/
INT32 smss_sysctrl_get_smss_hardveralmflag(CPSS_COM_PHY_ADDR_T stPhyaddr,
                                           UINT8 *pucHardVerAlmFlag)
{
   UINT32 ulBoardIndex;
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
   ulBoardIndex = Phy2BoardIndex(stPhyaddr);
   pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
   if( pstBoardPointer == NULL)
   {
           return SMSS_ERROR;
   }
   *pucHardVerAlmFlag = pstBoardPointer->ucHardVerAlmFlag;
   return SMSS_OK;
}

/***************************************************************************
* 函数名称: smss_sysctl_show_heartinfo
* 功    能: 用于记录心跳定时器的相关信息，以便于定位心跳丢失的问题
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_show_gcpa_heartinfo(VOID)
{
    UINT32     ulCount;
    SMSS_BOARD_INFO_T * pstNode;
    UINT32 ulBrdindex = 0;
    UINT32 ulSptBrdMaxNum = CPSS_COM_MAX_FRAME * CPSS_COM_MAX_SHELF * CPSS_COM_MAX_SLOT;
    pstNode = (SMSS_BOARD_INFO_T *)stHeartBeatList.pstHead;
    cps__oams_shcmd_printf("\nFrame   Shelf    Slot      RecvDATE     recvSeq     recvTicks    SendDate    SendSeq    sendTick\n");

    for (ulBrdindex = 0;ulBrdindex < ulSptBrdMaxNum ; ulBrdindex ++ )
    {
        pstNode = apstBoardIndex[ulBrdindex];
        if( pstNode == NULL || 
            TRUE == SMSS_ADDR_IS_EQUAL(pstNode->stPhyAddr, CPSS_PHY_ADDR_GCPA_SLAVE)
            ||TRUE == SMSS_ADDR_IS_EQUAL(pstNode->stPhyAddr, CPSS_PHY_ADDR_GCPA_MASTER))
          continue;
        for( ulCount =0;ulCount<SMSS_HEART_RECORD_MAX_CNT;ulCount++)
        {
            cps__oams_shcmd_printf("%5d%8d%8d   (%2d:%2d:%2d)%10d%15d    (%2d:%2d:%2d)%10d%10d\n",
                pstNode->stPhyAddr.ucFrame,
                pstNode->stPhyAddr.ucShelf,
                pstNode->stPhyAddr.ucSlot,
                pstNode->stHeartInfo.stRecvHB[ulCount].stTimer.ucHour,
                pstNode->stHeartInfo.stRecvHB[ulCount].stTimer.ucMinute,
                pstNode->stHeartInfo.stRecvHB[ulCount].stTimer.ucSecond,
                pstNode->stHeartInfo.stRecvHB[ulCount].ulRecvSeq,
                pstNode->stHeartInfo.stRecvHB[ulCount].ultickcnt,
                pstNode->stHeartInfo.stSendHB[ulCount].stTimer.ucHour,
                pstNode->stHeartInfo.stSendHB[ulCount].stTimer.ucMinute,
                pstNode->stHeartInfo.stSendHB[ulCount].stTimer.ucSecond,
                pstNode->stHeartInfo.stSendHB[ulCount].ulSendSeq,
                pstNode->stHeartInfo.stSendHB[ulCount].ultickcnt
                );
        }
    }
}

/*主控板丢失外围板的心跳处理 */
/***************************************************************************
* 函数名称: smss_sysctl_gcpa_lost_cpuhb_handle
* 功    能: SYSCTL接收到SYSRT纤程的主CPU心跳丢失的处理函数
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* 无
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_lost_cpuhb_handle(SMSS_SYSCTL_LOST_HOSTHB_T *pstMsg)
{
    UINT32 ulBoardIndex;
    SMSS_BOARD_INFO_T * pstNode;
    CPSS_COM_PHY_ADDR_T stBrdPhyAddr;

    CPS_CPU_STAT_CHG_IND_MSG_T stCpuStat;

    /* 设自己保存的状态 */
    ulBoardIndex = Phy2BoardIndex(pstMsg->stPhyAddr);
    if (NULL == apstBoardIndex[ulBoardIndex])
    {
        return;
    }
    pstNode = apstBoardIndex[ulBoardIndex];
    pstNode->ucAStatus = SMSS_STANDBY;
    pstNode->ucRStatus = SMSS_RSTATUS_END;
    pstNode->ucBeatCount = 0;
    pstNode->ucOldRStatus = SMSS_RSTATUS_END;
    /* 设RDBS */

	stCpuStat.iState = cpss_htonl((INT32)(1));
	switch(pstMsg->stPhyAddr.ucSlot)
	{
	case 1:
		stCpuStat.stLogicAddr.ucModule = 16;
		stCpuStat.stLogicAddr.usGroup = cpss_htons((UINT16)(1));
		stCpuStat.stLogicAddr.ucSubGroup = 1;

		stBrdPhyAddr.ucFrame = 1;
		stBrdPhyAddr.ucShelf = 1;
		stBrdPhyAddr.ucSlot = 1;
		stBrdPhyAddr.ucCpu = 1;
		g_brd_alarmstat[1] = 1;
		break;
	case 2:
		stCpuStat.stLogicAddr.ucModule = 16;
		stCpuStat.stLogicAddr.usGroup = cpss_htons((UINT16)(2));
		stCpuStat.stLogicAddr.ucSubGroup = 1;

		stBrdPhyAddr.ucFrame = 1;
		stBrdPhyAddr.ucShelf = 1;
		stBrdPhyAddr.ucSlot = 2;
		stBrdPhyAddr.ucCpu = 1;
		g_brd_alarmstat[2] = 1;
		break;
	case 3:
		stCpuStat.stLogicAddr.ucModule = 16;
		stCpuStat.stLogicAddr.usGroup = cpss_htons((UINT16)(2));
		stCpuStat.stLogicAddr.ucSubGroup = 2;

		stBrdPhyAddr.ucFrame = 1;
		stBrdPhyAddr.ucShelf = 1;
		stBrdPhyAddr.ucSlot = 2;
		stBrdPhyAddr.ucCpu = 2;
		g_brd_alarmstat[3] = 1;
		break;
	case 4:
		stCpuStat.stLogicAddr.ucModule = 16;
		stCpuStat.stLogicAddr.usGroup = cpss_htons((UINT16)(2));
		stCpuStat.stLogicAddr.ucSubGroup = 3;

		stBrdPhyAddr.ucFrame = 1;
		stBrdPhyAddr.ucShelf = 1;
		stBrdPhyAddr.ucSlot = 2;
		stBrdPhyAddr.ucCpu = 3;
		g_brd_alarmstat[4] = 1;
		break;
	}
#if 0 //dhwang added
	cps_send_om_alarm(OM_AM_CPS_ALARM_CODE_BOARD_NOT_RESPONDED, OAMS_ALARM_OCCUR, (UINT8*)&stBrdPhyAddr, sizeof(stBrdPhyAddr));
#endif
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
	"cps send om lost heart beat alarm occur ! brd = %d-%d-%d-%d\n",stBrdPhyAddr.ucFrame, stBrdPhyAddr.ucShelf, stBrdPhyAddr.ucSlot, stBrdPhyAddr.ucCpu);

	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
	"\n****************************************\n* recieve PPU status change Msg !\n* PPU Logic Addr = <%d-%d-%d>, State = <%d>\n* now broadcast to All proc !\n****************************************",
	stCpuStat.stLogicAddr.ucModule, cpss_ntohs(stCpuStat.stLogicAddr.usGroup),
	stCpuStat.stLogicAddr.ucSubGroup, cpss_ntohl(stCpuStat.iState));
		
	cpss_com_send_broadcast(CPS_CPU_STAT_CHG_IND_MSG, (UINT8*)&stCpuStat, sizeof(CPS_CPU_STAT_CHG_IND_MSG_T));

    smss_sysctl_set_cps__rdbs_rstatus(pstNode->stPhyAddr, SMSS_RSTATUS_END);
    smss_sysctl_set_cps__rdbs_astatus(pstNode->stPhyAddr, SMSS_STANDBY); 
    
    smss_sysctl_set_cps__rdbs_brdstatus(pstNode->stPhyAddr,CPS__RDBS_DEV_VALUE_BOARD_FAULT);
    smss_sysctrl_set_smss_boardstate(pstNode->stPhyAddr,SMSS_BRD_RSTATUS_FAULT);

    smss_send_alarm_brdnotresponded_to_oams(SMSS_ALMSUBNO_0, 1, pstNode->stPhyAddr);
}

/***************************************************************************
* 函数名称: smss_sysctl_set_brdstatus
* 功    能: 设置单板状态
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* stPhyAddr       CPSS_COM_PHY_ADDR_T     IN
* ucState         UINT8                   IN
* 函数返回: 
*           无。
* 说    明: 设置SMSS中单板状态和RDBS中的单板状态
***************************************************************************/
VOID smss_sysctl_set_brdstatus(CPSS_COM_PHY_ADDR_T stPhyAdd, UINT8 ucState)
{
    UINT8 ucRdbsBrdState = CPS__RDBS_DEV_VALUE_BOARD_FAULT;
    switch(ucState) 
    {
    case SMSS_BRD_RSTATUS_OFFLINE:
        ucRdbsBrdState = CPS__RDBS_DEV_VALUE_BOARD_OFFLINE;
    	break;
    case SMSS_BRD_RSTATUS_TYPERR:
        ucRdbsBrdState = CPS__RDBS_DEV_VALUE_BOARD_MISBRDTYPE;
    	break;
    case SMSS_BRD_RSTATUS_NORMAL:
        ucRdbsBrdState = CPS__RDBS_DEV_VALUE_BOARD_NORMAL;
        break;
    case SMSS_BRD_RSTATUS_FAULT:
        ucRdbsBrdState = CPS__RDBS_DEV_VALUE_BOARD_FAULT;
        break;   
    default:
        break;
    }
    smss_sysctl_set_cps__rdbs_brdstatus(stPhyAdd,ucRdbsBrdState);
    smss_sysctrl_set_smss_boardstate(stPhyAdd,ucState);    
}    

/***************************************************************************
* 函数名称: smss_sysctl_boardInfoRsp_Disp
* 功    能: 处理单板的信息信息查询响应消息
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* pstMsg          SMSS_BOARD_INFO_QUERY_RSP_MSG_T     IN
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_boardInfoRsp_Disp(SMSS_BOARD_INFO_QUERY_RSP_MSG_T* pstMsg)
{
    UINT32 ulBoardIndex;
    UINT32 *pulTemp;    
    UINT32 i;
    SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
    
    ulBoardIndex = Phy2BoardIndex(pstMsg->stPhyAddr);
    pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
    if( pstBoardPointer == NULL)
    {
        apstBoardIndex[ulBoardIndex] = cpss_mem_malloc(sizeof(SMSS_BOARD_INFO_T));
        pstBoardPointer = apstBoardIndex[ulBoardIndex];
        if( pstBoardPointer == NULL )
        {
            return;
        }
        cpss_mem_memset(pstBoardPointer,0,sizeof(SMSS_BOARD_INFO_T));
        
    }
    pstBoardPointer->stPhyAddr = pstMsg->stPhyAddr;
    pstBoardPointer->ulPhyBrdType = cpss_ntohl(pstMsg->ulPhyBrdType);
    pstBoardPointer->ucFunBrdType = pstMsg->ucFunBrdType;
    smss_sysctl_set_cps__rdbs_astatus(pstMsg->stPhyAddr, pstMsg->ucAStatus); 
	smss_sysctl_set_cps__rdbs_rstatus(pstMsg->stPhyAddr,SMSS_RSTATUS_NORMAL);
    smss_sysctl_set_brdstatus(pstMsg->stPhyAddr,pstMsg->ucBrdRStatus);

    /* 转换为本机字节序 */
    pulTemp = (UINT32 *)&(pstMsg->stPhyBoardVer);
    for (i = 0; i < sizeof(pstMsg->stPhyBoardVer) / sizeof(UINT32); i++)
    {
        *pulTemp = cpss_ntohl(*pulTemp);
        pulTemp++;
    }
    
    cpss_mem_memcpy(&(pstBoardPointer->stPhyBoardVer),&(pstMsg->stPhyBoardVer),sizeof(SMSS_PHYBRD_VERINFO_T));
    
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_update_board_info
* 功    能: 更新单板信息
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* stPhyAddr       CPSS_COM_PHY_ADDR_T     IN
* 函数返回: 
*           无。
* 说    明: 向本CPU的系统主控纤程发送运行状态设置请求,
*           向单板上的SYSCTL发送单板信息查询请求 
***************************************************************************/
VOID smss_sysctl_update_board_info(CPSS_COM_PHY_ADDR_T stPhyAddr)
{
    int iIndex;
    SMSS_CPU_RSTATE_SET_IND_MSG_T stRStateInd;
    SMSS_BOARD_INFO_T * pstBrdInfo = NULL;
    
    iIndex = Phy2BoardIndex(stPhyAddr);
    pstBrdInfo = apstBoardIndex[iIndex];
    if ((pstBrdInfo!=NULL)&&(pstBrdInfo->ucRStatus == SMSS_RSTATUS_NORMAL)) 
    {
        return;
    }
    /* 向本CPU的系统主控纤程发送运行状态设置请求 */
    stRStateInd.stPhyAddr = stPhyAddr;
    stRStateInd.ucRStatus = SMSS_RSTATUS_NORMAL;
    smss_send_to_local(SMSS_SYSCTL_PROC, SMSS_CPU_RSTATE_SET_IND_MSG,
        &stRStateInd, sizeof(stRStateInd));
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Send SMSS_CPU_RSTATE_SET_IND_MSG to SYSCTL");
        
        
    /* 向单板上的SYSCTL发送单板信息查询请求 */
    smss_com_send_phy(stPhyAddr, SMSS_SYSCTL_PROC,
        SMSS_BOARD_INFO_QUERY_REQ_MSG, NULL, 0);        
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "Send SMSS_BOARD_INFO_QUERY_REQ_MSG to Cpu: %d-%d-%d-%d",
        stPhyAddr.ucFrame,stPhyAddr.ucShelf,stPhyAddr.ucSlot,stPhyAddr.ucCpu);
    return;
}

UINT32 smss_sysctl_gcpa_set_slave_rstatus(CPSS_COM_PHY_ADDR_T stPhy, UINT8 ucValue)
{
    CPS__RDBS_DEV_SETCPUSTATUS_MSG_REQ_T stMsg;
    CPS__RDBS_DEV_SETCPUSTATUS_MSG_RSP_T stRspMsg;
    
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "<%s-%s>: In RDBS, set SLAVE (%d-%d-%d-%d) RStatus to <%s>.",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
        stPhy.ucFrame, stPhy.ucShelf, stPhy.ucSlot, stPhy.ucCpu,
        SMSS_RSTATUS_NORMAL == ucValue ? "NORMAL" : "FAULT");
    
    stMsg.stCpuPhyAddr = stPhy;
    stMsg.ucMode = CPS__RDBS_DEV_MODE_CPU_HWFAULT;
    stRspMsg.ulResult = SMSS_ERROR;
    
    /* 进行SMSS与RDBS的转换 */
    if (SMSS_RSTATUS_NORMAL == ucValue)
    {
        stMsg.ucValue = 0;      /* 正常 */
    }
    else
    {
        stMsg.ucValue = 1;      /* 故障 */
    }
    cps__rdbs_if_entry(CPS__RDBS_DEV_SETCPUSTATUS_MSG, CPS__RDBS_IF_FUNC,
        (UINT8 *)&stMsg, sizeof(stMsg),
        (UINT8 *)&stRspMsg, sizeof(stRspMsg));
    
    return stRspMsg.ulResult;
}

/***************************************************************************
* 函数名称: smss_sysctrl_cpustate_set_disp
* 功    能: 处理CPU状态设置消息
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                    输入/输出       描述
* pstMsg          SMSS_CPU_RSTATE_SET_IND_MSG_T     IN
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctrl_cpustate_set_disp(SMSS_CPU_RSTATE_SET_IND_MSG_T* pstMsg)
{
    if(pstMsg->stPhyAddr.ucCpu == 1)
    {

#if 0
        smss_sysctl_set_cps__rdbs_rstatus(pstMsg->stPhyAddr, pstMsg->ucRStatus);
#endif

    }
    else
    {
        smss_sysctl_gcpa_set_slave_rstatus(pstMsg->stPhyAddr, pstMsg->ucRStatus);
    }
    return;
}

/***************************************************************************
* 函数名称: smss_sysctl_global_force_reset_rsp
* 功    能: 在全局板上的强制复位整个网元处理
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                     输入/输出       描述
* *pstMid       SWP_MO_ID_T                   IN       要求复位对象的MOID 
* ulDelayTime   UINT32                        IN                      
* ulSeqId       UINT32                        IN
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_global_force_reset_rsp(SWP_MO_ID_T *pstMid,UINT32 ulDelayTime,UINT32 ulSeqId)
{
    CPS__OAMS_DYND_FORCE_RST_RSP_MSG_T stRsp;           /* 向OAMS发送的复位响应 */ 
    SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG_T stRst;           /* 向交换板发送的复位请求 */    
    CPS__RDBS_DEV_GETALLBRDINFO_MSG_RSP_T *pstRsp;               
    CPS__RDBS_DEV_GETLOGICADDR_MSG_REQ_T stRdbsReq;
    CPS__RDBS_DEV_GETLOGICADDR_MSG_RSP_T stRdbsRsp;              
    UINT32 ulIndex;  
    CPSS_COM_PID_T stPid;  
    INT32 lRet; 

    
    
    /* 直接向OAMS回成功响应 */
    stRsp.ulSeqId = cpss_htonl(ulSeqId);
    stRsp.ulReturnCode = cpss_htonl(0);
    smss_sysctl_send_local(CPS__OAMS_PD_M_MSGDSP_PROC, CPS__OAMS_DYND_FORCE_RST_RSP_MSG, 
                               (UINT8 *)&stRsp, sizeof(stRsp));
    
    /* 复位信息 */
    stRst.ulSeqId = cpss_htonl(ulSeqId);    
    stRst.ulResetType = cpss_htonl(SMSS_FORCE_RST_ALL_SHELF);
    stRst.ulDelayTime = cpss_htonl(3);  
    stRst.ulResetReason = cpss_htonl(0);
    
    /* 循环向交换板发延迟整框复位消息 */
    pstRsp = (CPS__RDBS_DEV_GETALLBRDINFO_MSG_RSP_T *)cpss_mem_malloc(sizeof(*pstRsp));

    if (NULL == pstRsp)
    {
      cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "<%s-%s>: smss_sysctl_global_force_reset_rsp malloc failed.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
      return;	
    }

    cpss_mem_memset(pstRsp, 0, sizeof(CPS__RDBS_DEV_GETALLBRDINFO_MSG_RSP_T));
    
    cps__rdbs_if_entry(CPS__RDBS_DEV_GETALLBRDINFO_MSG, CPS__RDBS_IF_FUNC,
                  NULL, 0, 
                  (UINT8 *)pstRsp, sizeof(*pstRsp));

    if (0 == pstRsp->ulResult) /* 查询成功 */
    {
       /* 查询所有的主用交换板 */
       for (ulIndex = 0; ulIndex < pstRsp->ulBrdNum; ulIndex++)
       {
          if ((pstRsp->astBrdInfo[ulIndex].ulBoardPhyType == SWP_PHYBRD_MASA)
              && ((pstRsp->astBrdInfo[ulIndex].usStatus & 0x8) == 0))
          {
          	 /* 根据物理地址获取逻辑地址 */
          	 stRdbsReq.stCpuPhyAddr = pstRsp->astBrdInfo[ulIndex].stBrdPhyAddr;
          	 
          	 cps__rdbs_if_entry(CPS__RDBS_DEV_GETLOGICADDR_MSG,CPS__RDBS_IF_FUNC,
                        (UINT8*)&stRdbsReq,sizeof(stRdbsReq),
                        (UINT8*)&stRdbsRsp,sizeof(stRdbsRsp)); 
             
             if(stRdbsRsp.ulResult == 0)
             {
             stRst.stPhyAddr = pstRsp->astBrdInfo[ulIndex].stBrdPhyAddr;
               
               stPid.stLogicAddr = stRdbsRsp.stCpuLogicAddr;
               stPid.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER;
               stPid.ulPd = SMSS_SYSCTL_PROC;
               	
               lRet = smss_com_send(&stPid, SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG, 
                             (UINT8 *)&stRst, sizeof(stRst));	
                           
               cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_FATAL,
                "<%s-%s>: Send reset all shelf to %d-%d-%d, ret %d.",
                g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
                stRst.stPhyAddr.ucFrame, stRst.stPhyAddr.ucShelf,
                stRst.stPhyAddr.ucSlot, lRet);       
             }            
          }  	
       }	
    }
    
    cpss_mem_free(pstRsp);
    return;    
}

/***************************************************************************
* 函数名称: smss_sysctl_cfg_gssa_gtsa
* 功    能: 在全局板上的强制复位整个网元处理
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                     输入/输出       描述
* *pstMid       SWP_MO_ID_T                   IN       要求复位对象的MOID 
* ulDelayTime   UINT32                        IN                      
* ulSeqId       UINT32                        IN
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_cfg_gssa_gtsa(CPSS_COM_PHY_ADDR_T stPhyAddr)
{
   UINT32 ulPhyBrdType = 0;
   UINT32 ulGcsaFlag;
   UINT32 ulGtsaFlag;
   smss_sysctl_get_phybrdtype(stPhyAddr,&ulPhyBrdType);
   if (ulPhyBrdType == SWP_PHYBRD_MASA)
   {
       smss_sysctl_send_cfginfo_aboutgssagtsa(&ulGcsaFlag,&ulGtsaFlag);
       smss_sysctl_cfg_masa_about_gssa(stPhyAddr,ulGcsaFlag,ulGtsaFlag);
   }
   else
   {
       return;
   }
}
/***************************************************************************
* 函数名称: smss_sysctl_send_cfginfo_aboutgssagtsa
* 功    能: 在全局板上的查询是否配置了GSSA
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                     输入/输出       描述
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_send_cfginfo_aboutgssagtsa(UINT32 *pulGcsaFlag,UINT32 *pulGtsaFlag)
{
    CPS__RDBS_DEV_GETHOSTCPUINFOBYTYPE_MSG_REQ_T stCpuQuryReq;
    CPS__RDBS_DEV_GETHOSTCPUINFOBYTYPE_MSG_RSP_T stCpuQuryRsp;
    UINT32  ulBoardCfgSum;
    UINT32  ulGcsaFlag = 0;
    UINT32  ulGtsaFlag = 0;
    *pulGcsaFlag = 0;
    *pulGtsaFlag = 0;
    /* 查询系统中是否配置GTSA单板 */
    stCpuQuryReq.ucCpuType = SWP_FUNBRD_GTSA;
    cps__rdbs_if_entry(CPS__RDBS_DEV_GETHCPUINFOBYTYPE_MSG,  CPS__RDBS_IF_FUNC, 
        (UINT8 *)&stCpuQuryReq, sizeof(stCpuQuryReq),
        (UINT8 *)&stCpuQuryRsp, sizeof(stCpuQuryRsp)); 
    if( stCpuQuryRsp.ulResult != CPS__RDBS_RESULT_OK )
    {
       cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
        "<%s-%s>: In RDBS, smss query board func type(%#x) Failed Result (%#x) ",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
        stCpuQuryReq.ucCpuType,stCpuQuryRsp.ulResult);
    }
    else
    {
        /* modified by laixf for XAB118 */
        ulBoardCfgSum = stCpuQuryRsp.ulHostCpuNum;
        /* modified end */
       if(ulBoardCfgSum != 0)
       {
           ulGtsaFlag = 1;
       }
    }
    /* 查询系统中是否配置GCSA单板 */
    stCpuQuryReq.ucCpuType = SWP_FUNBRD_GCSA;
    cps__rdbs_if_entry(CPS__RDBS_DEV_GETHCPUINFOBYTYPE_MSG,  CPS__RDBS_IF_FUNC, 
        (UINT8 *)&stCpuQuryReq, sizeof(stCpuQuryReq),
        (UINT8 *)&stCpuQuryRsp, sizeof(stCpuQuryRsp)); 
    if( stCpuQuryRsp.ulResult != CPS__RDBS_RESULT_OK )
    {
       cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
        "<%s-%s>: In RDBS, smss query board func type(%#x) Failed Result (%#x)" ,
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
        stCpuQuryReq.ucCpuType,stCpuQuryRsp.ulResult);
    }
    else
    {
        ulBoardCfgSum = stCpuQuryRsp.ulHostCpuNum;

       if(ulBoardCfgSum != 0)
       {
           ulGcsaFlag = 1;
       }
    }
    *pulGcsaFlag = ulGcsaFlag;
    *pulGtsaFlag = ulGtsaFlag;
}
/***************************************************************************
* 函数名称: smss_sysctl_cfg_masa_about_gssa
* 功    能: 
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                     输入/输出       描述
* *pstMid       SWP_MO_ID_T                   IN       要求复位对象的MOID 
* ulDelayTime   UINT32                        IN                      
* ulSeqId       UINT32                        IN
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_cfg_masa_about_gssa(CPSS_COM_PHY_ADDR_T stPhyAddr,UINT32 ulGcsaFlag,UINT32  ulGtsaFlag)
{
    SMSS_CFG_MASA_ABOUT_GSSA_MSG_T stCfgReq;
    stCfgReq.ulGcsaFlag = cpss_htonl(ulGcsaFlag);
    stCfgReq.ulGtsaFlag = cpss_htonl(ulGtsaFlag);
    smss_com_send_phy(stPhyAddr,SMSS_DEVM_PROC, SMSS_CFG_MASA_ABOUT_GSSA_MSG, 
                           (UINT8*)&stCfgReq, sizeof(stCfgReq));
    cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
        "<%s-%s>: smss send(%s) to MASA(%d-%d-%d-%d),ulGcsaFlag(%d) ulGtsaFlag(%d)",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
        "SMSS_CFG_MASA_ABOUT_GSSA_MSG", stPhyAddr.ucFrame,
        stPhyAddr.ucShelf,stPhyAddr.ucSlot,stPhyAddr.ucCpu,
        ulGcsaFlag,ulGtsaFlag);
}
/***************************************************************************
* 函数名称: smss_sysctl_gssa_adddel_ind_msg
* 功    能: 
* 函数类型  VOID
* 参    数: 
* 参数名称        类型                     输入/输出       描述

* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gssa_adddel_ind_msg(VOID)
{
    CPS__RDBS_DEV_GETHOSTCPUINFOBYTYPE_MSG_REQ_T stCpuQuryReq;
    CPS__RDBS_DEV_GETHOSTCPUINFOBYTYPE_MSG_RSP_T stCpuQuryRsp;
    UINT32 ulCount;
    UINT32 ulGcsaFlag =0 ;
    UINT32 ulGtsaFlag =0 ;
    CPSS_COM_PHY_ADDR_T stPhyAddr;
    /* 查询系统中所有的TCSA单板 */
    stCpuQuryReq.ucCpuType = SWP_FUNBRD_TCSA;
    cps__rdbs_if_entry(CPS__RDBS_DEV_GETHCPUINFOBYTYPE_MSG,  CPS__RDBS_IF_FUNC, 
        (UINT8 *)&stCpuQuryReq, sizeof(stCpuQuryReq),
        (UINT8 *)&stCpuQuryRsp, sizeof(stCpuQuryRsp)); 
    if( stCpuQuryRsp.ulResult != CPS__RDBS_RESULT_OK )
    {
       cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
        "<%s-%s>: In RDBS, smss query board func type(%#x) Failed Result (%#x) ",
        g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
        stCpuQuryReq.ucCpuType,stCpuQuryRsp.ulResult);
    }
    else
    {
        stCpuQuryRsp.ulHostCpuNum = stCpuQuryRsp.ulHostCpuNum;
    }
    smss_sysctl_send_cfginfo_aboutgssagtsa(&ulGcsaFlag,&ulGtsaFlag);
    for( ulCount=0; ulCount <stCpuQuryRsp.ulHostCpuNum; ulCount++)
    {
        stPhyAddr.ucShelf = stCpuQuryRsp.astHostCpuInfo[ulCount].ucShelf;
        stPhyAddr.ucFrame = stCpuQuryRsp.astHostCpuInfo[ulCount].ucFrame;
        stPhyAddr.ucSlot = stCpuQuryRsp.astHostCpuInfo[ulCount].ucSlot;
        stPhyAddr.ucCpu = 1;
        smss_sysctl_cfg_masa_about_gssa(stPhyAddr,ulGcsaFlag,ulGtsaFlag);
    }
}

/***************************************************************************
* 函数名称: smss_sysctl_gcpa_upperclk_req
* 功    能: 主用全局板收到外围板上级时钟查询请求
* 函数类型  VOID
* 参    数: 
* 参数名称        参数类型             输入/输出        参数描述
* pReq   SMSS_SYSCTL_UPPERCLK_QUERY_REQ_MSG_T*   IN     查询请求
* 函数返回:
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_upperclk_req(SMSS_SYSCTL_UPPERCLK_QUERY_REQ_MSG_T *pReq)
{	 
	 UINT8  ucSlot;
	 UINT32 ulIndex;
	 UINT32 ulBoardIndex;
	 CPSS_COM_PHY_ADDR_T stPhyAddr;
	 SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
	 CPS__RDBS_DEV_GETALLBRDINFO_MSG_RSP_T *pstRsp;
	 SMSS_SYSCTL_UPPERCLK_QUERY_RSP_MSG_T  stRsp;
	 
	 cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: Receive from (%d-%d-%d %d) SMSS_SYSCTL_UPPERCLK_QUERY_REQ_MSG.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            pReq->stPhyAddr.ucFrame, pReq->stPhyAddr.ucShelf,
            pReq->stPhyAddr.ucSlot, pReq->stPhyAddr.ucCpu); 
            
	 /* 根据槽位号判断是MASA或其他板 */               
	 ucSlot = pReq->stPhyAddr.ucSlot;	 
	 if (ucSlot == 7 || ucSlot == 8) /* masa */
	 {	
	  	pstRsp = (CPS__RDBS_DEV_GETALLBRDINFO_MSG_RSP_T *)cpss_mem_malloc(sizeof(*pstRsp));

	  	if (NULL == pstRsp)
      {
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
            "<%s-%s>: smss_sysctl_gcpa_upperclk_req malloc failed.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);
        return;	
      }

	    cpss_mem_memset(pstRsp, 0, sizeof(CPS__RDBS_DEV_GETALLBRDINFO_MSG_RSP_T));
	
	    cps__rdbs_if_entry(CPS__RDBS_DEV_GETALLBRDINFO_MSG, CPS__RDBS_IF_FUNC,
	              NULL, 0, 
	              (UINT8 *)pstRsp, sizeof(*pstRsp));
	    if (0 == pstRsp->ulResult)
	    {
	  	   /* 查询主用时钟板 */ 	  
	       for (ulIndex = 0; ulIndex < pstRsp->ulBrdNum; ulIndex++)
	       {
	         if ((pstRsp->astBrdInfo[ulIndex].ulBoardPhyType == SWP_PHYBRD_NSCA)  /* 主用时钟板 */
	            && ((pstRsp->astBrdInfo[ulIndex].usStatus & 0x8) == 0))
	          {
	      	     stRsp.ucSlot = pstRsp->astBrdInfo[ulIndex].stBrdPhyAddr.ucSlot;          	
	             smss_com_send_phy(pReq->stPhyAddr, 
	                   SMSS_SYSCTL_PROC, SMSS_SYSCTL_UPPERCLK_QUERY_RSP_MSG, 
	                   (UINT8 *)&stRsp, sizeof(stRsp)); 
	             cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
	                "<%s-%s>: smss send %s to (%d-%d-%d-%d), active nsca %d",
	                g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
	                "SMSS_SYSCTL_UPPERCLK_QUERY_RSP_MSG", pReq->stPhyAddr.ucFrame,
	                pReq->stPhyAddr.ucShelf,pReq->stPhyAddr.ucSlot,pReq->stPhyAddr.ucCpu,
	                stRsp.ucSlot);            	
	            
	            break;    
	          }	
	       }
	    }
	    cpss_mem_free(pstRsp); 	
	 }
	 else /* 其他单板,查询本框主用MASA */
	 {
	 	   stPhyAddr =  pReq->stPhyAddr;
	 	   for (ulIndex = 7; ulIndex < 9; ulIndex++)
	 	   { 	 	   	 
	       stPhyAddr.ucSlot = ulIndex;
	       ulBoardIndex = Phy2BoardIndex(stPhyAddr);
	       pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
	       if (NULL != pstBoardPointer)
	       {
	     	   if (SMSS_ACTIVE == pstBoardPointer->ucAStatus)
	     	   {
	     	     stRsp.ucSlot = ulIndex;          	
	           smss_com_send_phy(pReq->stPhyAddr, 
	                   SMSS_SYSCTL_PROC, SMSS_SYSCTL_UPPERCLK_QUERY_RSP_MSG, 
	                   (UINT8 *)&stRsp, sizeof(stRsp)); 
	           cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
	             "<%s-%s>: smss send %s to (%d-%d-%d-%d), active masa %d",
	             g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
	             "SMSS_SYSCTL_UPPERCLK_QUERY_RSP_MSG", pReq->stPhyAddr.ucFrame,
	             pReq->stPhyAddr.ucShelf,pReq->stPhyAddr.ucSlot,pReq->stPhyAddr.ucCpu,
	             stRsp.ucSlot); 
	     	     break;
	     	   }
	       }
	 	   }	     	     
	 }	 
	 return;
}

/***************************************************************************
* 函数名称: smss_showphyver
* 功    能: 全局单板上show smss存储的单板的硬件信息
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
INT32 smss_showphyver(UINT8 ucFrame, UINT8 ucShelf, UINT8 ucSlot)
{
	 UINT32 ulIndex;
   UINT32 ulBoardIndex;   
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
   CPSS_COM_PHY_ADDR_T stPhyAddr;
   
   if (ucFrame == 0 || ucShelf == 0 || ucSlot == 0)
   {
   	 cps__oams_shcmd_printf("param error.\n");
   	 return CPS__OAMS_OK;
   }
      
   stPhyAddr.ucFrame = ucFrame;
   stPhyAddr.ucShelf = ucShelf;
   stPhyAddr.ucSlot = ucSlot;
   stPhyAddr.ucCpu = 1;
   
   ulBoardIndex = Phy2BoardIndex(stPhyAddr);
   pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
   if( pstBoardPointer == NULL)
   {
   	 cps__oams_shcmd_printf("Board (%d-%d-%d 1) not exit.\n", ucFrame, ucShelf, ucSlot);
   	 return CPS__OAMS_OK;
   }
   
   cps__oams_shcmd_printf("Board (%d-%d-%d 1) info:\n", ucFrame, ucShelf, ucSlot);
   cps__oams_shcmd_printf("Hardware Info: MotherBrd: Type(%3d), Ver(0x%08X)\n",
                             pstBoardPointer->stPhyBoardVer.stMotherBrd.ulBrdType,
                             pstBoardPointer->stPhyBoardVer.stMotherBrd.ulBrdVer);
   cps__oams_shcmd_printf("               BackBrd:   Type(%3d), Ver(0x%08X)\n",
                             pstBoardPointer->stPhyBoardVer.stBackBrd.ulBrdType,
                             pstBoardPointer->stPhyBoardVer.stBackBrd.ulBrdVer);
   for (ulIndex = 0; ulIndex < pstBoardPointer->stPhyBoardVer.ulChildBrdNum; ulIndex++)
   {
      cps__oams_shcmd_printf("           ChildBrd%d: Type(%3d), Ver(0x%08X)\n",
                       ulIndex + 1,
                       pstBoardPointer->stPhyBoardVer.astChildBrd[ulIndex].ulBrdType,
                       pstBoardPointer->stPhyBoardVer.astChildBrd[ulIndex].ulBrdVer);
   }   
   cps__oams_shcmd_printf("ucHardVerAlmFlag %d\n",pstBoardPointer->ucHardVerAlmFlag);
   cps__oams_shcmd_printf("usVerState %d\n",pstBoardPointer->usVerState);
   cps__oams_shcmd_printf("ulSndHeartSeq %d\n",pstBoardPointer->ulSndHeartSeq);

   return CPS__OAMS_OK;
}

/***************************************************************************
* 函数名称: smss_show_cfgphyver
* 功    能: 全局单板上show RDBS存储的单板的硬件信息
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
INT32 smss_show_cfgphyver(UINT8 ucFrame, UINT8 ucShelf, UINT8 ucSlot)
{	 
	 SMSS_SYSCTL_CHECKPHYVER_REQ_MSG_T  stReq;
	 
	 if (ucFrame == 0 || ucShelf == 0 || ucSlot == 0)
   {
   	 cps__oams_shcmd_printf("param error.\n");
   	 return -1;
   }
   
   stReq.ucFrame = ucFrame;
   stReq.ucShelf = ucShelf;
   stReq.ucSlot = ucSlot;
   
   smss_sysctl_send_local(SMSS_SYSCTL_PROC, SMSS_SYSCTL_CHECKPHYVER_REQ_MSG, 
            (UINT8 *)&stReq, sizeof(stReq));   
   return 0;
}

/***************************************************************************
* 函数名称: smss_sysctl_gcpa_cfgphyver
* 功    能: 全局单板上查询RDBS存储的单板的硬件信息
* 函数类型  INT32 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_checkphyver_req(UINT8 *pucBuf)
{
	 UINT32 i;
	 UINT32 ulIndex;	 
	 CPSS_COM_PHY_ADDR_T  stPhyAddr;
	 CPS__RDBS_DEV_GETALLBRDPHYINFO_MSG_RSP_T *pstRsp;
	 SMSS_SYSCTL_CHECKPHYVER_REQ_MSG_T *pReq;
	
	 pReq = (SMSS_SYSCTL_CHECKPHYVER_REQ_MSG_T *)pucBuf;
	 
   stPhyAddr.ucFrame = pReq->ucFrame;
   stPhyAddr.ucShelf = pReq->ucShelf;
   stPhyAddr.ucSlot = pReq->ucSlot;
   stPhyAddr.ucCpu = 1;
   	
	 pstRsp = (CPS__RDBS_DEV_GETALLBRDPHYINFO_MSG_RSP_T *)cpss_mem_malloc(sizeof(*pstRsp));
	 if (NULL == pstRsp) 
   {
      cps__oams_shcmd_printf("malloc (%d) failed.", sizeof(*pstRsp));
      return;
   }
	 cpss_mem_memset(pstRsp, 0, sizeof(CPS__RDBS_DEV_GETALLBRDPHYINFO_MSG_RSP_T));
	 cps__rdbs_if_entry(CPS__RDBS_DEV_GETALLBRDPHYINFO_MSG, CPS__RDBS_IF_FUNC,
                  NULL, 0, 
                  (UINT8 *)pstRsp, sizeof(*pstRsp));

   if (0 != pstRsp->ulResult)  /* 查询数据库失败 */
   {
      cps__oams_shcmd_printf("CPS__RDBS_DEV_GETALLBRDINFO failed, ulResult = 0x%08X.",
                          pstRsp->ulResult);
      cpss_mem_free(pstRsp);
      return;
   }
  
   for (ulIndex = 0; ulIndex < pstRsp->ulBrdNum; ulIndex++)
   {
      if ((*(UINT32 *)&stPhyAddr) == (*(UINT32 *)&(pstRsp->astBrdInfo[ulIndex].stBrdPhyAddr)))
      {
        cps__oams_shcmd_printf("Board (%d-%d-%d 1) rdbs data:\n", pReq->ucFrame, pReq->ucShelf, pReq->ucSlot);
        cps__oams_shcmd_printf("Hardware Info: MotherBrd: Type(%3d), Ver(0x%08X)\n",
                 pstRsp->astBrdInfo[ulIndex].stPhyBoardVer.stMotherBrd.ulBrdType,
                 pstRsp->astBrdInfo[ulIndex].stPhyBoardVer.stMotherBrd.ulBrdVer);
        cps__oams_shcmd_printf("               BackBrd:   Type(%3d), Ver(0x%08X)\n",
                 pstRsp->astBrdInfo[ulIndex].stPhyBoardVer.stBackBrd.ulBrdType,
                 pstRsp->astBrdInfo[ulIndex].stPhyBoardVer.stBackBrd.ulBrdVer);
        for (i = 0; i < pstRsp->astBrdInfo[ulIndex].stPhyBoardVer.ulChildBrdNum; i++)
        {
          cps__oams_shcmd_printf("           ChildBrd%d: Type(%3d), Ver(0x%08X)\n",
                       i + 1,
                       pstRsp->astBrdInfo[ulIndex].stPhyBoardVer.astChildBrd[i].ulBrdType,
                       pstRsp->astBrdInfo[ulIndex].stPhyBoardVer.astChildBrd[i].ulBrdVer);
        }    
        break; 	
      } 	
   }
   cpss_mem_free(pstRsp);
	 return;
}

/***************************************************************************
* 函数名称: smss_sysctl_gcpa_board_status_ind
* 功    能: 设置一级交换板、二级交换板临时状态
* 函数类型  void 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_board_status_ind(SMSS_SYSCTL_BOARD_STATUS_IND_MSG_T *pInd)
{
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: Receive %s from (%d-%d-%d %d).",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            "SMSS_SYSCTL_BOARD_STATUS_IND_MSG",pInd->stPhyAddr.ucFrame,
            pInd->stPhyAddr.ucShelf, pInd->stPhyAddr.ucSlot, 
            pInd->stPhyAddr.ucCpu); 
            
	smss_sysctl_set_cps__rdbs_brdstatus(pInd->stPhyAddr,CPS__RDBS_DEV_VALUE_BOARD_FAULT);
  smss_sysctrl_set_smss_boardstate(pInd->stPhyAddr,SMSS_BRD_RSTATUS_FAULT);    
    
  smss_sysctl_set_cps__rdbs_astatus(pInd->stPhyAddr, SMSS_STANDBY);
  smss_sysctl_set_cps__rdbs_rstatus(pInd->stPhyAddr, SMSS_RSTATUS_END);

  smss_com_send_phy(pInd->stPhyAddr,SMSS_VERM_PROC, SMSS_SYSCTL_BOARD_STATUS_ACK_MSG, 
                           NULL, 0);
                            
  cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: send %s to (%d-%d-%d %d).",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            "SMSS_SYSCTL_BOARD_STATUS_ACK_MSG",pInd->stPhyAddr.ucFrame,
            pInd->stPhyAddr.ucShelf, pInd->stPhyAddr.ucSlot, 
            pInd->stPhyAddr.ucCpu);                         
	return;
}

/***************************************************************************
* 函数名称: smss_sysctl_gcpa_record_info
* 功    能: 记录纤程激和失败信息, 等单板正常后,上报告警解除
* 函数类型  void 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyAddr    CPSS_COM_PHY_ADDR_T          IN             物理地址
* ulAlarmNo     UINT32                      IN             告警号
* ulAlarmSubNo  UINT32                      IN             告警子号
* ulObjId       UINT32                      IN             附加信息(网络序) 
* ulResult      UINT32                      IN             附加信息(网络序)
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_record_info(CPSS_COM_PHY_ADDR_T stPhyAddr, UINT32 ulAlarmNo,
          UINT32 ulAlarmSubNo,  UINT32 ulObjId, UINT32 ulResult)
{
	 UINT32 ulBoardIndex;
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
   ulBoardIndex = Phy2BoardIndex(stPhyAddr);
   pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
   
   if( pstBoardPointer != NULL)
   {
   	 pstBoardPointer->ulSndHeartSeq = 1;
   	 pstBoardPointer->stHeartInfo.ulRecvHBCnt = ulAlarmNo;
   	 pstBoardPointer->stHeartInfo.stRecvHB[0].ultickcnt = ulAlarmSubNo;
   	 pstBoardPointer->stHeartInfo.stRecvHB[0].ulSendSeq = ulObjId;
   	 pstBoardPointer->stHeartInfo.stRecvHB[0].ulRecvSeq = ulResult;
   }   
   return;	
}


/***************************************************************************
* 函数名称: smss_sysctl_send_alarm_clean_to_oams
* 功    能: 判断是否发送激和失败告警解除
* 函数类型  void 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* stPhyAddr    CPSS_COM_PHY_ADDR_T          IN             物理地址
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_send_alarm_clean_to_oams(CPSS_COM_PHY_ADDR_T  stPhyAddr)
{
   UINT32 ulBoardIndex;
   CPS__OAMS_AM_ORIGIN_ALARM_T stOamsAlarm;
   SMSS_BOARD_INFO_T * pstBoardPointer = NULL;
   ulBoardIndex = Phy2BoardIndex(stPhyAddr);
   pstBoardPointer = apstBoardIndex[ulBoardIndex]; 
   
   
   if( pstBoardPointer != NULL)
   {
   	 if (0 == pstBoardPointer->ulSndHeartSeq)  /* 无告警 */
   	 {
   	   return;	
   	 }
   	 
   	 /* 发告警解除 */
   	 stOamsAlarm.ulAlarmNo = pstBoardPointer->stHeartInfo.ulRecvHBCnt;
   	 stOamsAlarm.ulAlarmSubNo =  pstBoardPointer->stHeartInfo.stRecvHB[0].ultickcnt;
   	 stOamsAlarm.enAlarmType = CPS__OAMS_ALARM_CLEAR;
   	 smss_moid_get_moid_by_phy(CPS__OAMS_MOID_TYPE_BOARD,&stPhyAddr,&stOamsAlarm.stFaultMoID);
   	 *(UINT32*)stOamsAlarm.aucExtraInfo = pstBoardPointer->stHeartInfo.stRecvHB[0].ulSendSeq;
     *(UINT32*)(stOamsAlarm.aucExtraInfo+4) = pstBoardPointer->stHeartInfo.stRecvHB[0].ulRecvSeq;    
     smss_send_alarm_to_oams(&stOamsAlarm);
     
     /* 清0 */
     pstBoardPointer->ulSndHeartSeq = 0;
   }
   return;	
}



/***************************************************************************
* 函数名称: smss_sysctl_gcpa_fanpara_req
* 功    能: gcpa处理风扇参数设置请求
* 函数类型  void 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* pReq          SMSS_SYSCTL_FANPARA_REQ_MSG_T*          IN            
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_fanpara_req(SMSS_SYSCTL_FANPARA_REQ_MSG_T *pReq)
{
	 CPS__RDBS_DEV_GETFANCTRL_MSG_RSP_T stRsp;
	 CPS__RDBS_DEV_FANCTRL_CFG_T  stCfg;
	 
	 cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: recv %s from (%d-%d-%d %d).",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            "SMSS_SYSCTL_FANPARA_REQ_MSG",pReq->stPhyAddr.ucFrame,
            pReq->stPhyAddr.ucShelf, pReq->stPhyAddr.ucSlot, 
            pReq->stPhyAddr.ucCpu); 
            
   /* 查询RDBS, 本地序 */         
   cps__rdbs_if_entry(CPS__RDBS_DEV_GETFANCTRL_MSG, CPS__RDBS_IF_FUNC, 
         NULL, 0, (UINT8 *)&stRsp, sizeof(stRsp));
   
   if (0 != stRsp.ulResult) /* 查询失败, 不向masa发消息 */
   {
   	 cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: rdbs get fanpara fail 0x%x.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            stRsp.ulResult); 
     return;       
   }     
   
   /* 查询成功, 转化为网络序 */              
   stCfg = stRsp.stFanCtrlCfg;
   stCfg.usTfc = cpss_htons(stCfg.usTfc);
   smss_com_send_phy(pReq->stPhyAddr, SMSS_DEVM_PROC, SMSS_SYSCTL_FANPARA_RSP_MSG, 
                    (UINT8 *)&stCfg, sizeof(stCfg));
                    
   cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: send %s to (%d-%d-%d %d), ucTti %d, ucFanUpThreshold %d," 
            "ucFanDownThreshold %d, ucFanUpAlarm %d, ucNpwmUp %d, ucNpwmDown %d,"
            "usTfc %d, ucTpuDelay %d",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            "SMSS_SYSCTL_FANPARA_RSP_MSG",pReq->stPhyAddr.ucFrame,
            pReq->stPhyAddr.ucShelf, pReq->stPhyAddr.ucSlot, 
            pReq->stPhyAddr.ucCpu, stRsp.stFanCtrlCfg.ucTti, 
            stRsp.stFanCtrlCfg.ucFanUpThreshold, stRsp.stFanCtrlCfg.ucFanDownThreshold,
            stRsp.stFanCtrlCfg.ucFanUpAlarm, stRsp.stFanCtrlCfg.ucNpwmUp,
            stRsp.stFanCtrlCfg.ucNpwmDown, stRsp.stFanCtrlCfg.usTfc,
            stRsp.stFanCtrlCfg.ucTpudelay);                     
	return;
}

/***************************************************************************
* 函数名称: smss_sysctl_gcpa_cpuloadinfo_req
* 功    能: CPU负荷控制查询请求
* 函数类型  void 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* pstMsg          CPSS_COM_MSG_HEAD_T*          IN            
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_cpuloadinfo_req(CPSS_COM_MSG_HEAD_T * pstMsg)
{
	CPS__RDBS_DEV_GETCPULOADCTRL_MSG_REQ_T  *pReq;
  CPS__RDBS_DEV_GETCPULOADCTRL_MSG_RSP_T  stRsp;
  pReq = (CPS__RDBS_DEV_GETCPULOADCTRL_MSG_REQ_T *)(pstMsg->pucBuf);
  
  cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: recv SMSS_SYSCTL_CPULOADINFO_REQ_MSG, para (%d-%d-%d %d)",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            pReq->stCpuPhyAddr.ucFrame, pReq->stCpuPhyAddr.ucShelf,
            pReq->stCpuPhyAddr.ucSlot,pReq->stCpuPhyAddr.ucCpu); 
            
  cps__rdbs_if_entry(CPS__RDBS_DEV_GETCPULOADCTRL_MSG, CPS__RDBS_IF_FUNC, 
               (UINT8 *)pReq, sizeof(CPS__RDBS_DEV_GETCPULOADCTRL_MSG_REQ_T),
               (UINT8 *)&stRsp, sizeof(stRsp));
               
  /* 转换为网络序 */
  stRsp.ulResult = cpss_htonl(stRsp.ulResult);
  stRsp.usWatchTimer = cpss_htons(stRsp.usWatchTimer);
  stRsp.usAlarmTimer = cpss_htons(stRsp.usAlarmTimer);
  stRsp.aucRsv[0] = pReq->stCpuPhyAddr.ucCpu;   /* 使用保留字节传CPU号, 比较用 */
  
  /* 按源返回 */
  smss_com_send(&(pstMsg->stSrcProc), SMSS_SYSCTL_CPULOADINFO_RSP_MSG,
                (UINT8 *)&stRsp, sizeof(stRsp));
                
  cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: send SMSS_SYSCTL_CPULOADINFO_RSP_MSG.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);                            
	return;
}

/***************************************************************************
* 函数名称: smss_sysctl_gcpa_boardloadinfo_req
* 功    能: 单板负荷控制查询请求
* 函数类型  void 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* pstMsg          CPSS_COM_MSG_HEAD_T*          IN            
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_boardloadinfo_req(CPSS_COM_MSG_HEAD_T * pstMsg)
{
	CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG_REQ_T  *pReq;
  CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG_RSP_T  stRsp;
  pReq = (CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG_REQ_T *)(pstMsg->pucBuf);
  
  cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: recv SMSS_SYSCTL_BOARDLOADINFO_REQ_MSG, para (%d-%d-%d %d)",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            pReq->stCpuPhyAddr.ucFrame, pReq->stCpuPhyAddr.ucShelf,
            pReq->stCpuPhyAddr.ucSlot,pReq->stCpuPhyAddr.ucCpu); 
            
  cps__rdbs_if_entry(CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG, CPS__RDBS_IF_FUNC, 
               (UINT8 *)pReq, sizeof(CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG_REQ_T),
               (UINT8 *)&stRsp, sizeof(stRsp));
               
  /* 转换为网络序 */
  stRsp.ulResult = cpss_htonl(stRsp.ulResult);   
  /* 按源返回 */
  smss_com_send(&(pstMsg->stSrcProc), SMSS_SYSCTL_BOARDLOADINFO_RSP_MSG,
                (UINT8 *)&stRsp, sizeof(stRsp));
                
  cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: send SMSS_SYSCTL_BOARDLOADINFO_RSP_MSG.",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus);                                
  return;	
}

/***************************************************************************
* 函数名称: smss_sysctl_gcpa_boardlevel_ind
* 功    能: 单板级别设置消息
* 函数类型  void 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_boardlevel_ind(UINT8 *pucBuf)
{
	CPS__RDBS_DEV_SETOVERLOADSTATE_MSG_REQ_T *pReq;
	CPS__RDBS_DEV_SETOVERLOADSTATE_MSG_RSP_T  stRsp;
	pReq = (CPS__RDBS_DEV_SETOVERLOADSTATE_MSG_REQ_T *)pucBuf;
	
	cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: recv SMSS_SYSCTL_BOARDLEVELCHG_IND_MSG, (%d-%d-%d %d), level %d",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            pReq->stBoardPhyAddr.ucFrame, pReq->stBoardPhyAddr.ucShelf,
            pReq->stBoardPhyAddr.ucSlot,pReq->stBoardPhyAddr.ucCpu,
            pReq->ucOverloadState);
            
  cps__rdbs_if_entry(CPS__RDBS_DEV_SETOVERLOADSTATE_MSG, CPS__RDBS_IF_FUNC, 
        (UINT8 *)pReq, sizeof(*pReq),
        (UINT8 *)&stRsp, sizeof(stRsp));
                   
	return;
}

/***************************************************************************
* 函数名称: smss_sysctl_gcpa_boarddel_ind
* 功    能: 单板删除消息
* 函数类型  void 
* 参    数: 
* 参数名称      参数类型                    输入/输出      参数描述
* 
* 函数返回: 
*           无。
* 说    明: 
***************************************************************************/
VOID smss_sysctl_gcpa_boarddel_ind(UINT8 *pucBuf)
{
	INT32 lRet;
	CPSS_COM_PHY_ADDR_T stPhy;
	SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG_T stRst;
  CPS__RDBS_DEV_BOARDDEL_IND_MSG_T  *pInd;
  
  pInd = (CPS__RDBS_DEV_BOARDDEL_IND_MSG_T *)pucBuf;
  
  cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: recv CPS__RDBS_DEV_BOARDDEL_IND_MSG, (%d-%d-%d)",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            pInd->ucFrame, pInd->ucShelf, pInd->ucSlot);
  
  stPhy.ucFrame = pInd->ucFrame;
  stPhy.ucShelf = pInd->ucShelf;
  stPhy.ucSlot =  pInd->ucSlot;
  stPhy.ucCpu = 1;
  
  /* 直接复位单板 */
  stRst.ulSeqId = cpss_htonl(0);
  stRst.stPhyAddr = stPhy; 
  stRst.ulResetType = cpss_htonl(SMSS_FORCE_RST_SPC_SLOT);
  stRst.ulDelayTime = cpss_htonl(0);    
  stRst.ulResetReason = cpss_htonl(0);  
  
  lRet = smss_com_send_phy(stPhy, SMSS_SYSCTL_PROC, SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG, (UINT8 *)&stRst, sizeof(stRst));
  cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_INFO,
            "<%s-%s>: send SMSS_SYSCTL_FORCE_RST_CMD_REQ_MSG, lRet %d",
            g_stSmssAsInfo.szSelfAStatus, g_stSmssBaseInfo.szRStatus,
            pInd->ucFrame, pInd->ucShelf, pInd->ucSlot, lRet);
  return;	
}
#endif

/***************************************************************************
* 函数名称: cps_send_om_alarm
* 功    能: 向OM发送告警
* 函数类型  void
* 参    数:
* 参数名称      参数类型                    输入/输出      参数描述
* ulAlarmNO		UINT32		IN
* ucAlarmType	UINT8		IN
* pucbuf		UINT8*		IN
* ulLen			UINT32		IN
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
#if 0 //dhwang added
VOID cps_send_om_alarm(UINT32 ulAlarmNo, UINT32 ulAlarmType, UINT8* pucbuf, UINT32 ulLen)
{
	CPSS_COM_PID_T stPid;

	OAMS_AM_ORIGIN_ALARM_T alarm_msg;

	if(NULL == pucbuf)
	{
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "recv NULL msg in cps_send_om_alarm.");
        return;
	}

	cpss_com_logic_addr_get(&stPid.stLogicAddr, &stPid.ulAddrFlag);
	stPid.ulPd = OAMS_PD_A_AM_AGT_PROC;

	memset(&alarm_msg, 0, sizeof(alarm_msg));
	alarm_msg.ulAlarmNo = cpss_htonl(ulAlarmNo);
	alarm_msg.enAlarmType = cpss_htonl(ulAlarmType);
	alarm_msg.ulAlarmSubNo = cpss_htonl(ulAlarmNo + 1);
	memcpy(alarm_msg.aucExtraInfo, pucbuf, ulLen);
	cpss_com_send(&stPid, OM_ALARM_IND_MSG, (UINT8 *)&alarm_msg, sizeof(alarm_msg));
}
#endif
/***************************************************************************
* 函数名称: cps_send_om_event
* 功    能: 向OM发送事件
* 函数类型  void
* 参    数:
* 参数名称      参数类型                    输入/输出      参数描述
*ulEventNO		UINT32		IN
*pucbuf			UINT8*		IN
*ulLen			UINT32		IN
* 函数返回:
*           无。
* 说    明:
***************************************************************************/
#if 0 //dhwang added
VOID cps_send_om_event(UINT32 ulEventNO, UINT8* pucbuf, UINT32 ulLen)
{
	CPSS_COM_PID_T stPid;

	OAMS_AM_ORIGIN_EVENT_T event_msg;

	if(NULL == pucbuf)
	{
        cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_SYSCTL, CPSS_PRINT_ERROR,
                   "recv NULL msg in cps_send_om_event.");
        return;
	}

	cpss_com_logic_addr_get(&stPid.stLogicAddr, &stPid.ulAddrFlag);
	stPid.ulPd = OAMS_PD_A_AM_AGT_NOTIFY_PROC;

	memset(&event_msg, 0, sizeof(event_msg));
	event_msg.ulEventNo = cpss_htonl(ulEventNO);
	event_msg.ulExtraLen = cpss_htonl(ulLen);
	memcpy(event_msg.aucExtraInfo, pucbuf, ulLen);
	cpss_com_send(&stPid, OM_EVENT_IND_MSG, (UINT8 *)&event_msg, sizeof(event_msg));
}
#endif
/******************************源程序结束******************************************/
