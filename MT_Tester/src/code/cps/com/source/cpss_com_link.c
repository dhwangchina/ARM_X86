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
#include "smss_public.h"
#include "cpss_public.h"
#include "cpss_config.h"
#include "cpss_com_common.h"
#include "cpss_com_drv.h"
#include "cpss_com_trace.h"
#include "cpss_com_link.h"
#include "cpss_com_pci.h"
#include "cpss_config.h"
#include "cpss_err.h"

/******************************* 局部宏定义 **********************************/

#ifndef CPSS_MC_BRD_BACKUP_TYPE
#define CPSS_MC_BRD_BACKUP_TYPE (CPS__RDBS_DEV_11_BACKUP)
#endif

/******************************* 全局变量定义/初始化 *************************/

#ifdef CPSS_PCI_INCLUDE
extern CPSS_COM_PCI_TABLE_T g_stCpssComPciTbl;
#endif

extern UINT32 g_ulSlidSem ;
/* 常量逻辑地址 */
CPSS_COM_LOGIC_ADDR_T g_stLogicAddrGcpa;
/* 以下三个常量逻辑地址取值参考G1接口手册和O接口手册 */
CPSS_COM_LOGIC_ADDR_T g_stLogicAddrOmcSvr  ={250,0,0};
CPSS_COM_LOGIC_ADDR_T g_stLogicAddrOmcPmSvr={251,0,0};
CPSS_COM_LOGIC_ADDR_T g_stLogicAddrHlrSvr  ={252,1,0};

/* 常量物理地址 */
CPSS_COM_PHY_ADDR_T   g_stPhyAddrGcpaMaster={0,0,0,0};
CPSS_COM_PHY_ADDR_T   g_stPhyAddrGcpaSlave ={0,0,0,0};

/* 空地址 */
CPSS_COM_LOGIC_ADDR_T g_stComLogicAddrZero={0,0,0};
CPSS_COM_PHY_ADDR_T   g_stComPhyAddrZero  ={0,0,0,0};

/* 通信全局数据 */
CPSS_COM_DATA_T *g_pstComData;

#ifdef CPSS_FUNBRD_MC
/*RDBS失败统计的全局变量*/
UINT32 RdbsArpLinkInfo[CPSS_CPS__RDBS_INFO_RECORD];
UINT32 RdbsRarpLinkInfo[CPSS_CPS__RDBS_INFO_RECORD];
UINT32 g_ulArpNum = 0;
UINT32 g_ulRarpNum = 0;
#endif

/*主备切换统计变量*/
CPSS_COM_SWITCH_INFO_T switchinfo[CPSS_COM_SWITCH_RECORD_NUM];

/*通信初始化流程统计变量*/
#ifndef CPSS_DSP_CPU
CPSS_COM_INIT_STAT_T g_stCpssComInitStat = {0};
#endif


/*通信ARP记录统计变量*/
CPSS_COM_ARP_STAT_T g_stCpssComArpStat = {0};
UINT32 g_ulCpssMateChannelFlag = 0 ;
UINT32 g_ulCpssPoweronNum = 0 ;
UINT32 g_ulCpssDspSbbrMsgRecSwitch = 1;
CPSS_SBBR_DSP_RECORD_T *g_pstCpssSbbrRecord = (CPSS_SBBR_DSP_RECORD_T *)CPSS_DSP_SBBR_CPSS_START_ADDR;
CPSS_COM_HEARTBEAT_NODE_T* g_pstCpssHeartBeatLink = NULL ;
CPSS_COM_HEARTBEAT_STAT_T  g_stCpssHeartBeatStat = {0} ;

/*链路连通测试的管理全局变量*/
CPSS_COM_DIAG_COMM_TEST_MAN_T g_stCpssCommTestMan ;

/*CPSS通信初始化结束标志*/
UINT32 g_ulCpssComInitFlag = CPSS_COM_INIT_UNKNOWN ;

/*单板正常标志*/
UINT32 g_ulCpssBrdNormalFlag = FALSE ;

/*IP交换板第一次发送链路建立请求标志*/
UINT32 g_ulCpssMasaLinkReqFlag = FALSE ;

extern BOOL g_bCpssRecvProcEndInd;
extern CPSS_COM_PHY_ADDR_T g_LocalBrdPhyAddr;
extern CPSS_COM_LOGIC_ADDR_T gLocalBrdLogAddr;

/******************************* 局部常数和类型定义 **************************/

static CPSS_COM_ROUTE_T *g_pstComRouteTbl = NULL; /* 路由表 */
static UINT32           *g_aulComLinkTbl  = NULL;  /* 链路表 */

                                                   /*
                                                   * 以下配置表使用工具产生,不要手工配置,否则通信可能失败
*/
CPSS_COM_ADDR_T g_astComAddrGcpa[2]=
{
    /* ??? master */
    {
        CPSS_COM_ADDRFLAG_MASTER,     /* master */
            CPS__RDBS_DEV_11_BACKUP,
        {0,0,0},
        {0x1,0x1,0x8111},   /* logicAddr: module,subGroup,Group */
        {0x1,0x1,0x1,0x1}, /* phyAddr: frame, shelf, slot, cpu */
        0x00000000,       /* 自动根据物理地址生成 */
    },
    /* ??? slave */
    {
            CPSS_COM_ADDRFLAG_SLAVE,     /* slave */
                CPS__RDBS_DEV_11_BACKUP,
            {0,0,0},
            {0x1,0x1,0x8111},   /* logicAddr: module,subGroup,Group */
            {0x1,0x1,0x3,0x1}, /* phyAddr: frame, shelf, slot, cpu */
            0x00000000,       /* 自动根据物理地址生成 */
        },
};

/******************************* 局部函数原型声明 ****************************/
VOID cpss_com_link_gcpa_switch
(
    CPSS_COM_PHY_ADDR_T stThisPhyAddr,
    UINT8               ucStatus
);
/******************************* 函数实现 ************************************/

/*******************************************************************************
* 函数名称: cpss_com_route_table_init
* 功    能: 为路由表分配内存,并设置所有表项为无效状态
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
INT32 cpss_com_route_table_init(VOID)
{
    /* 为路由表申请内存 */
    g_pstComRouteTbl=(CPSS_COM_ROUTE_T*)cpss_mem_malloc
        (CPSS_COM_MAX_ROUTE_NUM*sizeof(CPSS_COM_ROUTE_T));   /* cpss type cast */

    if(NULL == g_pstComRouteTbl)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "route table initial failed!!");
        return CPSS_ERROR;
    }

    /* 所有表项，初始化为无效状态 */
    cpss_mem_memset(g_pstComRouteTbl,0,CPSS_COM_MAX_ROUTE_NUM*sizeof(CPSS_COM_ROUTE_T));

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_link_table_init
* 功    能:  为链路表分配内存,并设置所有的链路为无效状态
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
INT32 cpss_com_link_table_init(VOID)
{
    /* 为链路表申请内存 */
    g_aulComLinkTbl=(UINT32*)cpss_mem_malloc
        (CPSS_COM_MAX_LINK_NUM*sizeof(UINT32*));
    if(NULL == g_aulComLinkTbl)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "link table initial failed!!");
        return CPSS_ERROR;
    }

    /* 所有表项，初始化为无效状态 */
    cpss_mem_memset(g_aulComLinkTbl, 0, CPSS_COM_MAX_LINK_NUM*sizeof(CPSS_COM_LINK_T*));

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_udp_init
* 功    能: 初始化udp接收相关参数
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
#ifndef CPSS_DSP_CPU
INT32  cpss_com_udp_init(VOID)
{
    CPSS_COM_DRV_T* pstDrv=cpss_com_drv_find(CPSS_COM_DRV_UDP);
    CPSS_COM_SOCK_ADDR_T *pstSockAddr=&pstDrv->stSocketAddr;

    /* socket运行环境初始化 */
#ifdef CPSS_VOS_WINDOWS
    {
        WSADATA wsaData;
        if (WSAStartup(0x0202, &wsaData)!= 0)
        {
            return CPSS_ERROR ;
        }
    }
#endif

    /* 获取socket描述符 */
     pstDrv->ulDrvD=socket(AF_INET, SOCK_DGRAM,
        IPPROTO_UDP);
    if(INVALID_SOCKET ==  pstDrv->ulDrvD)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "socket initial failed!!");
        return CPSS_ERROR;
    }

    /* 填写socket地址参数 */
    pstSockAddr->sin_addr.s_addr=cpss_htonl(g_pstComData->stAddrThis.ulIpAddr);
    pstSockAddr->sin_port=cpss_htons(pstSockAddr->sin_port);

    /* 绑定本地IP地址 */
    if (bind(pstDrv->ulDrvD, (struct sockaddr*)pstSockAddr, sizeof(CPSS_COM_SOCK_ADDR_T))
        == SOCKET_ERROR)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "udp socket bind failed!!");
        closesocket(pstDrv->ulDrvD);
        pstDrv->ulDrvD=0;
        return CPSS_ERROR;
    }
    return CPSS_OK;
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_phyaddr_format_change
* 功    能: 转换IP格式
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
#ifndef CPSS_DSP_CPU

INT32 cpss_com_phyaddr_format_change
(
UINT32 ulPhyAddr,
CPSS_COM_PHY_ADDR_T* pstPhyAddr
)
{
    pstPhyAddr->ucFrame = (ulPhyAddr&0xff000000)>>24 ;
    pstPhyAddr->ucShelf = (ulPhyAddr&0x00ff0000)>>16  ;
    pstPhyAddr->ucSlot  = (ulPhyAddr&0x0000ff00)>>8  ;
    pstPhyAddr->ucCpu   = (ulPhyAddr&0x000000ff) ;
#ifndef SWP_FNBLK_BRDTYPE_ABOX
    if((pstPhyAddr->ucFrame > CPSS_COM_MAX_FRAME)||(pstPhyAddr->ucFrame < 1))
    {
        return CPSS_ERROR;
    }
    if((pstPhyAddr->ucShelf > CPSS_COM_MAX_SHELF)||(pstPhyAddr->ucShelf < 1))
    {
        return CPSS_ERROR;
    }
    if((pstPhyAddr->ucSlot > CPSS_COM_MAX_SLOT)||(pstPhyAddr->ucSlot < 1))
    {
        return CPSS_ERROR;
    }
#endif
    return CPSS_OK;
}

#endif
/*******************************************************************************
* 函数名称: cpss_com_addr_init
* 功    能: 初始化全局板和本版的地址信息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
*
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
******************************************************************************/
INT32 cpss_com_addr_init(VOID)
{
    UINT32 ulPhyAddr;
    UINT32 ucMcP1 = cpss_ntohl(SWP_MC_BOARD_PHY1);
    UINT32 ucMcP2 = cpss_ntohl(SWP_MC_BOARD_PHY2);
    INT32 lRet;

    ulPhyAddr = 0;
    /* 常量物理地址 */
    g_stPhyAddrGcpaMaster = *(CPSS_COM_PHY_ADDR_T*)&ucMcP1;
    g_stPhyAddrGcpaSlave  = *(CPSS_COM_PHY_ADDR_T*)&ucMcP2;

    /* 常量逻辑地址 */
    lRet = cpss_com_log_addr_get(CPSS_MC_MODULE, &g_stPhyAddrGcpaMaster, &g_stLogicAddrGcpa);
    if (lRet != CPSS_OK)
    {
        return (CPSS_ERROR);
    }

    /* 主控板地址1 */
    cpss_mem_memset(&g_pstComData->stAddrGcpaM, 0, sizeof(CPSS_COM_ADDR_T));
    cpss_mem_memcpy(&g_pstComData->stAddrGcpaM.stPhyAddr, &g_stPhyAddrGcpaMaster,
        sizeof(CPSS_COM_PHY_ADDR_T));
    g_pstComData->stAddrGcpaM.ucBackupType = CPSS_MC_BRD_BACKUP_TYPE;
    cpss_mem_memcpy(&g_pstComData->stAddrGcpaM.stLogiAddr, &g_stLogicAddrGcpa,
        sizeof(CPSS_COM_LOGIC_ADDR_T));

    /* 主控板地址2 */
    cpss_mem_memset(&g_pstComData->stAddrGcpaS, 0, sizeof(CPSS_COM_ADDR_T));
    cpss_mem_memcpy(&g_pstComData->stAddrGcpaS.stPhyAddr, &g_stPhyAddrGcpaSlave,
        sizeof(CPSS_COM_PHY_ADDR_T));
    g_pstComData->stAddrGcpaS.ucBackupType = CPSS_MC_BRD_BACKUP_TYPE;
    cpss_mem_memcpy(&g_pstComData->stAddrGcpaS.stLogiAddr, &g_stLogicAddrGcpa,
        sizeof(CPSS_COM_LOGIC_ADDR_T));

    /* 本板地址 */
#ifdef CPSS_HOST_CPU

#ifdef SWP_FNBLK_BRDTYPE_ABOX
    ulPhyAddr = CPSS_ALARM_BOX_IP_ADDR ;
#else
    drv_ipmi_addr_get(0x01, &ulPhyAddr);
#endif

    lRet = cpss_com_phyaddr_format_change(ulPhyAddr,&g_pstComData->stAddrThis.stPhyAddr) ;
    if(lRet != CPSS_OK)
    {
        return CPSS_ERROR;
    }

#if (SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST)
#ifndef SWP_FNBLK_BRDTYPE_ABOX
    g_pstComData->stAddrThis.stPhyAddr.ucCpu = 1;
#endif
#endif

#endif
    cpss_com_phy2ip(g_pstComData->stAddrThis.stPhyAddr, &g_pstComData->stAddrThis.ulIpAddr, FALSE);

    /* 获取主控板的IP地址 */
    cpss_com_phy2ip(g_stPhyAddrGcpaMaster, &g_pstComData->stAddrGcpaM.ulIpAddr, FALSE);
    cpss_com_phy2ip(g_stPhyAddrGcpaSlave, &g_pstComData->stAddrGcpaS.ulIpAddr, FALSE);

    /* 获取主控板的其它地址 */
    if(0 != CPSS_COM_PHY_ADDR_SAME(g_pstComData->stAddrThis.stPhyAddr,
      g_stPhyAddrGcpaMaster))
    {
        cpss_mem_memcpy(&g_pstComData->stAddrThis,
            &g_pstComData->stAddrGcpaM,
            sizeof(CPSS_COM_ADDR_T));
    }
    else if(0 != CPSS_COM_PHY_ADDR_SAME(g_pstComData->stAddrThis.stPhyAddr,
        g_stPhyAddrGcpaSlave))
    {
        cpss_mem_memcpy(&g_pstComData->stAddrThis,
            &g_pstComData->stAddrGcpaS,
            sizeof(CPSS_COM_ADDR_T));
    }  /* end if  CPSS_COM_PHY_ADDR_SAME(g_pstComData->stAddrThis.stPhyAddr*/

/*平台添加从本地配置文件读取单板ip*/
    {
    	int iFd;
    	unsigned char aucIpAddr[256] = {0};

    	iFd = open("/etc/baseip", O_RDONLY);
    	if(iFd < 0)
    	{
    		printf("error to load file /etc/baseip\n");
			return CPSS_ERROR;
    	}
    	if(0 == read(iFd, aucIpAddr, sizeof(aucIpAddr)))
    	{
    		printf("read file /etc/baseip failure\n");
			return CPSS_ERROR;
    	}
    	close(iFd);
        inet_aton(aucIpAddr, &g_pstComData->stAddrThis.ulIpAddr);
    }
/**/

    /*初始化流程统计*/
#ifndef CPSS_DSP_CPU
    g_stCpssComInitStat.stAddrInitInfo.ulAddrInitFlag = 1 ;
    g_stCpssComInitStat.stAddrInitInfo.stGcpaLogAddr = g_pstComData->stAddrGcpaM.stLogiAddr ;
    g_stCpssComInitStat.stAddrInitInfo.stGcpaPhyAddrM = g_pstComData->stAddrGcpaM.stPhyAddr ;
    g_stCpssComInitStat.stAddrInitInfo.stGcpaPhyAddrS = g_pstComData->stAddrGcpaS.stPhyAddr ;
    g_stCpssComInitStat.stAddrInitInfo.stThisPhyAddr = g_pstComData->stAddrThis.stPhyAddr ;
    g_stCpssComInitStat.stAddrInitInfo.stThisLogAddr = g_pstComData->stAddrThis.stLogiAddr ;
    g_stCpssComInitStat.stAddrInitInfo.ulAddrInitFlag= g_pstComData->stAddrThis.ulAddrFlag ;
    g_stCpssComInitStat.stAddrInitInfo.ulGcpaIpM = g_pstComData->stAddrGcpaM.ulIpAddr ;
    g_stCpssComInitStat.stAddrInitInfo.ulGcpaIpS = g_pstComData->stAddrGcpaS.ulIpAddr ;
    g_stCpssComInitStat.stAddrInitInfo.ulThisIp  = g_pstComData->stAddrThis.ulIpAddr ;
    cpss_clock_get(&g_stCpssComInitStat.stAddrInitInfo.stTime) ;
#endif

    return CPSS_OK;

}


/*******************************************************************************
* 函数名称:cpss_com_link_find_real
* 功    能:
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明: 保存 逻辑地址\物理地址\链路 之间的对应关系
******************************************************************************/
CPSS_COM_LINK_T* cpss_com_link_find_real
(
 CPSS_COM_PHY_ADDR_T stPhyAddr
)
{
    UINT32   ulIdx ;
    UINT32 *pulAddr ;

    ulIdx = cpss_com_phy2index(stPhyAddr);

    /*判断是否越界*/
    if(ulIdx > CPSS_COM_MAX_LINK_NUM)
    {
        return NULL ;
    }

    pulAddr = g_aulComLinkTbl+ulIdx;

    return (CPSS_COM_LINK_T*)*pulAddr;
}


/*******************************************************************************
* 函数名称:cpss_com_route_find_real
* 功    能: 创建路由
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明: 保存 逻辑地址\物理地址\链路 之间的对应关系
******************************************************************************/
CPSS_COM_ROUTE_T* cpss_com_route_find_real
(
 CPSS_COM_LOGIC_ADDR_T stLogAddr
)
{
    /* 逻辑地址 转换成 物理地址 数组下标 */
    UINT32 ulIdx ;
    CPSS_COM_LOGIC_ADDR_T stMCpuLogAddr = stLogAddr ;

    stMCpuLogAddr.ucSubGroup = 1 ;

    if(CPSS_COM_LOGIC_ADDR_THIS_CPU(stMCpuLogAddr))
    {
        stMCpuLogAddr.ucSubGroup = stLogAddr.ucSubGroup ;
    }

    ulIdx = cpss_com_logic2index(stMCpuLogAddr);

    /* 判断范围是否越界 */
    if( ulIdx >= CPSS_COM_MAX_ROUTE_NUM )
    {
        return NULL;
    }
    return (g_pstComRouteTbl+ulIdx);
}


/*******************************************************************************
* 函数名称: cpss_com_route_item_add
* 功    能: 在路由表中保存物理地址
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明: 保存 逻辑地址\物理地址\链路 之间的对应关系
******************************************************************************/
INT32 cpss_com_route_item_add
(
 CPSS_COM_ROUTE_T* pstRoute,
 CPSS_COM_PHY_ADDR_T stPhyAddr,
 UINT32 ulIndex
)
{
    UINT32 ulIndexLocal = ulIndex ;
    CPSS_COM_PHY_ADDR_T stPhyAddrTmp ;

    /*如果传入的物理地址及索引和路由表中对应的项相等,则成功返回*/
    if(1 != CPSS_COM_PHY_ADDR_SAME(pstRoute->stPhyAddr[ulIndexLocal],stPhyAddr))
    {
        /*修改index的值*/
        if(ulIndexLocal == 0)
        {
            ulIndexLocal = 1 ;
        }
        else
        {
            ulIndexLocal = 0 ;
        }

        /*如果路由表的两项和传入的物理地址均不等,则以传入的物理地址和索引为准*/
        if(1 != CPSS_COM_PHY_ADDR_SAME(pstRoute->stPhyAddr[ulIndexLocal],stPhyAddr))
        {
            pstRoute->stPhyAddr[ulIndex] = stPhyAddr ;
        }
        else
        {
            /*以重新接入的链路的主备状态为准修改路由表*/
            stPhyAddrTmp = pstRoute->stPhyAddr[0] ;
            pstRoute->stPhyAddr[0] = pstRoute->stPhyAddr[1] ;
            pstRoute->stPhyAddr[1] = stPhyAddrTmp ;
        }
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_route_create
* 功    能: 创建路由
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* stLogicAddr CPSS_COM_LOGIC_ADDR_T  输入           逻辑地址
* ulAddrFlag  UINT32                 输入           主备标识
* stPhyAddr   CPSS_COM_PHY_ADDR_T    输入           物理地址
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明: 保存 逻辑地址\物理地址\链路 之间的对应关系
******************************************************************************/
INT32 cpss_com_route_create
(
 CPSS_COM_LOGIC_ADDR_T stLogicAddr,
 UINT32 ulAddrFlag,
 CPSS_COM_PHY_ADDR_T stPhyAddr,
 UINT32 ulDrvType
 )
{
    CPSS_COM_ROUTE_T *pstRoute=cpss_com_route_find_real(stLogicAddr);
    UINT32 ulIdx=(CPSS_COM_ADDRFLAG_MASTER == ulAddrFlag)?0:1;
    CPSS_COM_LINK_T *pstLink = NULL ;

    /* 检查路由对象的有效性 */
    if(NULL == pstRoute)
    {
        return CPSS_ERR_COM_ROUTE_NOT_EXIST;
    }

    /* 创建链路 */
    if(cpss_com_link_create(stPhyAddr,ulDrvType)!=CPSS_OK)
    {
        return CPSS_ERROR;
    }

    /*初始化滑窗对象*/
    pstLink = cpss_com_link_find_real(stPhyAddr) ;

    /*在链路中保存对方单板的主备状态*/
    pstLink->ulAddrFlag   = ulAddrFlag ;
    pstLink->stDstLogAddr = stLogicAddr ;

    cpss_output(CPSS_MODULE_COM, CPSS_PRINT_DETAIL,
        "Route Create, peer logAddr = %#x. pstLink->ulLinkId = %d",
        *(UINT32*)&pstLink->stDstLogAddr, pstLink->ulLinkId);

    /*初始化滑窗对象*/
    cpss_com_slid_init(pstLink,&(pstLink->stSlidWin)) ;

    /* 保存路由物理地址 */
    cpss_com_route_item_add(pstRoute,stPhyAddr,ulIdx) ;

    /*设置GCPA全局变量中对应的值*/
    cpss_com_link_gcpa_switch(stPhyAddr,(UINT8)ulAddrFlag) ;

#ifdef CPSS_SWITCH_BRDTYPE
    /*如果是备板*/
    if((g_pstComData->stAddrThis.ulAddrFlag == CPSS_COM_ADDRFLAG_SLAVE)&&
        (g_ulCpssMateChannelFlag == CPSS_COM_MATE_CHANNEL_ENABLE))
    {
        /*如果对方是主用交换板则将路由表中都记录*/
        if(CPSS_COM_PHY_ADDR_SAME(stPhyAddr,g_pstComData->stMatePhyAddr)==1)
        {
            pstRoute->stPhyAddr[0]=stPhyAddr;
            pstRoute->stPhyAddr[1]=stPhyAddr;
        }
    }
#endif
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_route_delete
* 功    能:  删除路由，同时删除物理地址对应的链路
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
#ifndef CPSS_DSP_CPU
INT32 cpss_com_route_delete
(
 CPSS_COM_LOGIC_ADDR_T stLogicAddr,
 UINT32 ulAddrFlag
 )
{
    CPSS_COM_ROUTE_T *pstRoute=cpss_com_route_find_real(stLogicAddr);
    UINT32 ulIdx=(ulAddrFlag==CPSS_COM_ADDRFLAG_MASTER)?0:1;

    /* 检查路由对象的有效性 */
    if(NULL == pstRoute)
    {
        return CPSS_ERR_COM_ROUTE_NOT_EXIST;
    }

    /* 删除链路 */
    cpss_com_link_delete(pstRoute->stPhyAddr[ulIdx]);

    /* 删除路由 */
    cpss_mem_memset(&pstRoute->stPhyAddr[ulIdx],0,sizeof(CPSS_COM_PHY_ADDR_T));

    return CPSS_OK;
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_route_find
* 功    能: 根据逻辑地址查找路由表项
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* stLogiAddr  CPSS_COM_LOGIC_ADDR_T    输入         逻辑地址
* 返回值:
*        路由对象
* 说   明:
*******************************************************************************/
CPSS_COM_ROUTE_T* cpss_com_route_find(CPSS_COM_LOGIC_ADDR_T stLogiAddr)
{
    /* 逻辑地址 转换成 物理地址 数组下标 */
    UINT32 ulIdx ;
    CPSS_COM_LOGIC_ADDR_T stMCpuLogAddr = stLogiAddr ;

    stMCpuLogAddr.ucSubGroup = 1 ;

    if(CPSS_COM_LOGIC_ADDR_THIS_CPU(stMCpuLogAddr))
    {
        stMCpuLogAddr.ucSubGroup = stLogiAddr.ucSubGroup ;
    }
#ifdef CPSS_SWITCH_BRDTYPE
    else
    {
        if((g_pstComData->stAddrThis.ulAddrFlag == CPSS_COM_ADDRFLAG_SLAVE)&&
            (g_ulCpssMateChannelFlag == CPSS_COM_MATE_CHANNEL_ENABLE))
        {
            if(!cpss_com_link_if_pci_link(stLogiAddr))
            {
                stMCpuLogAddr = g_pstComData->stAddrThis.stLogiAddr ;
            }
        }
    }
#endif

    ulIdx = cpss_com_logic2index(stMCpuLogAddr) ;

    /* 判断范围是否越界 */
    if( ulIdx >= CPSS_COM_MAX_ROUTE_NUM )
    {
        return NULL;
    }

    return (g_pstComRouteTbl + ulIdx);
}

/*******************************************************************************
* 函数名称: cpss_com_link_create
* 功    能: 创建路由,保存 物理地址\链路 之间的对应关系
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* stPhyAddr  CPSS_COM_PHY_ADDR_T    输入           物理地址
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_link_create
(
    CPSS_COM_PHY_ADDR_T stPhyAddr,
    UINT32 ulDrvType
 )
{
    UINT32   ulIdx ;
    UINT32   *pulAddr ;
    UINT32   ulIp ;
    UINT32   ulCpuNo ;
    CPSS_COM_LINK_T *pstLink = NULL;
    CPSS_COM_DRV_T  *pstDrv = NULL ;

    ulIdx   = cpss_com_phy2index(stPhyAddr) ;

     if(ulIdx > CPSS_COM_MAX_LINK_NUM)
    {
        return CPSS_ERROR ;
    }
    pulAddr = g_aulComLinkTbl+ulIdx ;
    pstLink = (CPSS_COM_LINK_T*)*pulAddr ;

    /*判断目的地址是否是伙伴板*/
    if(TRUE == cpss_com_mate_phyaddr(stPhyAddr))
    {
        cpss_com_phy2ip(stPhyAddr,&ulIp,TRUE) ;
    }
    else
    {
        cpss_com_phy2ip(stPhyAddr,&ulIp,FALSE) ;
    }

    /* 如果链路对象无效,则重新申请内存 */
    if(NULL == pstLink)
    {
        if(NULL == (pstLink=cpss_mem_malloc(sizeof(CPSS_COM_LINK_T))))
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
                "link create failed!");
            return CPSS_ERROR;
        }
        cpss_mem_memset(pstLink,0,sizeof(CPSS_COM_LINK_T));
    }

    /* 保存链路参数 */
    pstLink->stDstPhyAddr = stPhyAddr;
    pstLink->usStatus     = CPSS_COM_LINK_STATUS_UNKNOWN;

    /* 得到驱动数据 */
    if(CPSS_COM_DRV_PCI==ulDrvType)
    {
        pstDrv  = cpss_com_drv_find(CPSS_COM_DRV_PCI);

        /*对PCI链路的linkid进行赋值*/
        ulCpuNo = stPhyAddr.ucCpu ;
#ifdef CPSS_PCI_INCLUDE
        cpss_com_drv_pci_linkid_get(ulCpuNo,&pstLink->ulLinkId) ;
#endif
    }
    else
    {
        pstDrv                = cpss_com_drv_find(CPSS_COM_DRV_UDP);
        pstLink->ulLinkId     = cpss_com_phy2index(stPhyAddr);
    }

    /* 保存链路对应的驱动 */
    pstDrv->stSocketAddr.sin_addr.s_addr = ulIp;
    cpss_mem_memcpy(&pstLink->stDrv, pstDrv, sizeof(CPSS_COM_DRV_T));
  /*  pstLink->stDrv.stSocketAddr.sin_addr.s_addr = ulIp;*/

    /* 把链路对象保存在链路表中 */
    *pulAddr = (UINT32)pstLink;

    /* 建立链路的统计 */
    pstLink->stLinkStat.ulLinkSetupNum = pstLink->stLinkStat.ulLinkSetupNum + 1;

    /*将链路信息增加到心跳链表中*/
    if(CPSS_OK != cpss_com_heartbeat_link_add(stPhyAddr))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,"add to heart beat link failed phyaddr=0x%x",*(UINT32*)&stPhyAddr) ;
        return CPSS_ERROR ;
    }

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_link_delete
* 功    能: 删除指定物理地址对应的链路
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* stPhyAddr  CPSS_COM_PHY_ADDR_T    输入           物理地址
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
******************************************************************************/
#ifndef CPSS_DSP_CPU
INT32 cpss_com_link_delete
(
 CPSS_COM_PHY_ADDR_T stPhyAddr
 )
{
    UINT32   ulIdx            = cpss_com_phy2index(stPhyAddr);
    UINT32 *pulAddr           = g_aulComLinkTbl+ulIdx;

    CPSS_COM_LINK_T  *pstLink = (CPSS_COM_LINK_T *)*pulAddr;
    UINT32          ulLinkIdx;

    ulLinkIdx = cpss_com_phy2index(stPhyAddr);

    /* 检查链路对象的有效性 */
    if(NULL == pstLink)
    {
        return CPSS_ERROR;
    }

    /* 删除链路 */
    cpss_mem_free(pstLink);
    *pulAddr = 0;

    return CPSS_OK;
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_link_find
* 功    能: 查找指定物理地址对应的链路
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* stPhyAddr  CPSS_COM_PHY_ADDR_T    输入           物理地址
* 返回值:
链路对象
* 说   明:
* 恢复原来的保护
*******************************************************************************/
CPSS_COM_LINK_T* cpss_com_link_find(CPSS_COM_PHY_ADDR_T stPhyAddr)
{

    UINT32   ulIdx ;
/****
    UINT32 *pulAddr ;
****/
    CPSS_COM_LINK_T* pstLink;
    CPSS_COM_PHY_ADDR_T stSendPhyAddr ;

    stSendPhyAddr = stPhyAddr ;

#ifdef CPSS_VOS_VXWORKS
    stSendPhyAddr.ucCpu = 1 ;
    if(CPSS_COM_PHY_ADDR_THIS_CPU(stSendPhyAddr))
    {
        stSendPhyAddr.ucCpu = stPhyAddr.ucCpu ;
    }
#endif

#if ((defined CPSS_SWITCH_BRDTYPE) && (defined CPSS_HOST_CPU))
    if((g_pstComData->stAddrThis.ulAddrFlag == CPSS_COM_ADDRFLAG_SLAVE)&&
        (g_ulCpssMateChannelFlag == CPSS_COM_MATE_CHANNEL_ENABLE))
    {
        if(stSendPhyAddr.ucCpu == 1)
        {
            stSendPhyAddr = g_pstComData->stMatePhyAddr ;
        }
    }
#endif

    ulIdx = cpss_com_phy2index(stSendPhyAddr);

    /*判断是否越界*/
    if(ulIdx > CPSS_COM_MAX_LINK_NUM)
    {
        return NULL ;
    }
    pstLink = (CPSS_COM_LINK_T*)(*(UINT32*)(g_aulComLinkTbl+ulIdx));

 #ifndef CPSS_SWITCH_BRDTYPE
    if(pstLink != NULL)
    {
        if(!CPSS_COM_PHY_ADDR_ZERO(pstLink->stMPhyAddr))
        {
            if(pstLink->ulAddrFlag == CPSS_COM_ADDRFLAG_SLAVE)
            {

                ulIdx = cpss_com_phy2index(pstLink->stMPhyAddr) ;

               /*判断是否越界*/
               if(ulIdx > CPSS_COM_MAX_LINK_NUM)
               {
                   return NULL ;
               }

                pstLink = (CPSS_COM_LINK_T*)(*(UINT32*)(g_aulComLinkTbl
                    + ulIdx));
            }
        }
    }
#endif

    return pstLink;   /* cpss type cast */
}


/*******************************************************************************
* 函数名称: cpss_com_link_byid_find
* 功    能: 通过链路的ID查找指定的链路对象指针
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* ulLinkId            UINT32          输入              链路ID
* 返回值:
链路对象指针
* 说   明:
*******************************************************************************/
CPSS_COM_LINK_T* cpss_com_link_byid_find(UINT32 ulLinkId)
{
    return (CPSS_COM_LINK_T*)(*(g_aulComLinkTbl+ulLinkId)) ; /* cpss type cast */
}


/*******************************************************************************
* 函数名称: cpss_com_link_status_set
* 功    能: 设置指定对端物理地址链路的状态
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* stPhyAddr  CPSS_COM_PHY_ADDR_T    输入            物理地址
* usStatus   UINT16                 输入            新的链路状态
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
*******************************************************************************/
INT32 cpss_com_link_status_set
(
 CPSS_COM_PHY_ADDR_T stPhyAddr,
 UINT16 usStatus
 )
{
    CPSS_COM_LINK_T *pstLink=cpss_com_link_find_real(stPhyAddr);

    /* 检查状态值是否有效 */
    if(usStatus>CPSS_COM_LINK_STATUS_FAULT)
    {
        return CPSS_ERROR;
    }

    /* 检查链路对象是否有效 */
    if(NULL == pstLink)
    {
        return CPSS_ERROR;
    }
    pstLink->usStatus=usStatus;

    /*设置heartbeat链表中相关节点的状态*/
    cpss_com_heartbeat_node_status_set(stPhyAddr,usStatus) ;

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_link_show
* 功    能: 显示所有存在的链路
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述

* 返回值:
*        无
* 说   明:
*
*******************************************************************************/
VOID cpss_com_link_show(VOID)
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_LINK_T *pstLink;
    CPSS_COM_DRV_T  *pstDrv;
    UINT32 ulIdx=0/*,ulLogicAddr*/;
    UINT32 ulLinkNum = 0;

    cps__oams_shcmd_printf("\n Local phyAddr: 0x%08x, logicAddr: 0x%08x:AddrFlag: 0x%08x:\n",
        *(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,
        *(UINT32*)&g_pstComData->stAddrThis.stLogiAddr,
        *(UINT32*)&g_pstComData->stAddrThis.ulAddrFlag);

    while(ulIdx<CPSS_COM_MAX_LINK_NUM)
    {
        pstLink=(CPSS_COM_LINK_T *)*(g_aulComLinkTbl+ulIdx);
        if(NULL == pstLink)
        {
            ulIdx = ulIdx + 1;
            continue;
        }

        {
            cps__oams_shcmd_printf("LinkId(%d)'s Info Begin:\n", ulIdx);

            pstDrv=&pstLink->stDrv;
            cps__oams_shcmd_printf("LinkId: %d, status: %d\n",ulIdx, pstLink->usStatus);
            cps__oams_shcmd_printf("Peer's phyAddr 0x%08x, LogAddr: 0x%08x, peer status: %d\n",
                *(UINT32*)&pstLink->stDstPhyAddr, *(UINT32*)&pstLink->stDstLogAddr, pstLink->ulAddrFlag);
            cps__oams_shcmd_printf("DrvType %d, DrvD %#x\n",pstDrv->ulDrvType,pstDrv->ulDrvD);
            cps__oams_shcmd_printf("IP addr 0x%08x\n",cpss_ntohl(pstDrv->stSocketAddr.sin_addr.s_addr));
            cps__oams_shcmd_printf("LinkSetupNum: %d, PeerSwitchNum: %d\n", pstLink->stLinkStat.ulLinkSetupNum,pstLink->stLinkStat.ulPeerSwitchNum);
            cps__oams_shcmd_printf("HeartBeatRecvNum: %d, HeartBeatSendNum: %d\n", pstLink->stLinkStat.ulHeartBeatRecvNum,pstLink->stLinkStat.ulHeartBeatSendNum);
            cps__oams_shcmd_printf("DeliverErrNum: %d, FaultNum: %d\n", pstLink->stLinkStat.ulDeliverErrNum,pstLink->stLinkStat.ulFaultNum);
            cps__oams_shcmd_printf("RecvDataNum: %d, SendDataNum: %d\n", pstLink->stLinkStat.ulRecvDataNum,pstLink->stLinkStat.ulSendDataNum);

            cps__oams_shcmd_printf("ulMateFlag: %d\n", pstLink->ulMateFlag);
            cps__oams_shcmd_printf("stMPhyAddr: 0x%x\n", *(UINT32*)&pstLink->stMPhyAddr);

            /*增加流量统计打印*/
            cps__oams_shcmd_printf("link volume stat:\n");
            cps__oams_shcmd_printf("ulSendPkgNum=%d\n",
                pstLink->stVolumeStat.ulSendPkgNum);
            cps__oams_shcmd_printf("ulSendBytes=%d\n",
                pstLink->stVolumeStat.ulSendBytes);
            cps__oams_shcmd_printf("ulSendBytesPerSecond=%d\n",
                pstLink->stVolumeStat.ulSendBytesPerSecond);
            cps__oams_shcmd_printf("ulSendBytesPerSecondPeek=%d\n",
                pstLink->stVolumeStat.ulSendBytesPerSecondPeek);
        }

        cpss_com_slid_show(&pstLink->stSlidWin);
        cps__oams_shcmd_printf("LinkId(%d)'s Info End\n\n", ulIdx);

        ulIdx = ulIdx + 1;
        ulLinkNum = ulLinkNum + 1;
    }

    cps__oams_shcmd_printf("Now Local Board has total LinkId: %d\n\n", ulLinkNum);
#else
    cps__oams_shcmd_printf("cpss_com_link_show can not support!\n\n");
#endif
}
VOID tidShow(UINT32 ulPhyAddr)
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_LINK_T *pstLink;
    CPSS_COM_DRV_T  *pstDrv;
    UINT32 ulIdx=0/*,ulLogicAddr*/;
    UINT32 ulLinkNum = 0;
    CPSS_COM_PHY_ADDR_T stPhyAddr;

    cps__oams_shcmd_printf("\n Local phyAddr: 0x%08x, logicAddr: 0x%08x:AddrFlag: 0x%08x:\n",
        *(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,
        *(UINT32*)&g_pstComData->stAddrThis.stLogiAddr,
        *(UINT32*)&g_pstComData->stAddrThis.ulAddrFlag);

    stPhyAddr.ucFrame = (UINT8)((ulPhyAddr & 0xff000000) >> 24);
    stPhyAddr.ucShelf = (UINT8)((ulPhyAddr & 0xff0000) >> 16);
    stPhyAddr.ucSlot = (UINT8)((ulPhyAddr & 0xff00) >> 8);
    stPhyAddr.ucCpu = (UINT8)((ulPhyAddr & 0xff));

    ulIdx = cpss_com_phy2index(stPhyAddr);
    {
        pstLink=(CPSS_COM_LINK_T *)*(g_aulComLinkTbl+ulIdx);
        if(NULL == pstLink)
        {
            printf ("\n Input phyAddr has no link Now! \n");
            return ;
        }

        {
            cps__oams_shcmd_printf("LinkId(%d)'s Info Begin:\n", ulIdx);

            pstDrv=&pstLink->stDrv;
            cps__oams_shcmd_printf("LinkId: %d, status: %d\n",ulIdx, pstLink->usStatus);
            cps__oams_shcmd_printf("Peer's phyAddr 0x%08x, LogAddr: 0x%08x, peer status: %d\n",
                *(UINT32*)&pstLink->stDstPhyAddr, *(UINT32*)&pstLink->stDstLogAddr, pstLink->ulAddrFlag);
            cps__oams_shcmd_printf("DrvType %d, DrvD %#x\n",pstDrv->ulDrvType,pstDrv->ulDrvD);
            cps__oams_shcmd_printf("IP addr 0x%08x\n",cpss_ntohl(pstDrv->stSocketAddr.sin_addr.s_addr));
            cps__oams_shcmd_printf("LinkSetupNum: %d, PeerSwitchNum: %d\n", pstLink->stLinkStat.ulLinkSetupNum,pstLink->stLinkStat.ulPeerSwitchNum);
            cps__oams_shcmd_printf("HeartBeatRecvNum: %d, HeartBeatSendNum: %d\n", pstLink->stLinkStat.ulHeartBeatRecvNum,pstLink->stLinkStat.ulHeartBeatSendNum);
            cps__oams_shcmd_printf("DeliverErrNum: %d, FaultNum: %d\n", pstLink->stLinkStat.ulDeliverErrNum,pstLink->stLinkStat.ulFaultNum);
            cps__oams_shcmd_printf("RecvDataNum: %d, SendDataNum: %d\n", pstLink->stLinkStat.ulRecvDataNum,pstLink->stLinkStat.ulSendDataNum);

            cps__oams_shcmd_printf("ulMateFlag: %d\n", pstLink->ulMateFlag);
            cps__oams_shcmd_printf("stMPhyAddr: 0x%x\n", *(UINT32*)&pstLink->stMPhyAddr);

            /*增加流量统计打印*/
            cps__oams_shcmd_printf("link volume stat:\n");
            cps__oams_shcmd_printf("ulSendPkgNum=%d\n",
                pstLink->stVolumeStat.ulSendPkgNum);
            cps__oams_shcmd_printf("ulSendBytes=%d\n",
                pstLink->stVolumeStat.ulSendBytes);
            cps__oams_shcmd_printf("ulSendBytesPerSecond=%d\n",
                pstLink->stVolumeStat.ulSendBytesPerSecond);
            cps__oams_shcmd_printf("ulSendBytesPerSecondPeek=%d\n",
                pstLink->stVolumeStat.ulSendBytesPerSecondPeek);
        }

        cpss_com_slid_show(&pstLink->stSlidWin);
        cps__oams_shcmd_printf("LinkId(%d)'s Info End\n\n", ulIdx);

    }

#endif
}

/*******************************************************************************
* 函数名称: cpss_com_route_show
* 功    能: 显示所有存在的路由
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述

* 返回值:
*        无
* 说   明:
*
*******************************************************************************/
VOID cpss_com_route_show(VOID)
{
#ifndef CPSS_DSP_CPU
    UINT32 ulIdx=0;
    CPSS_COM_LOGIC_ADDR_T stLogicAddr;
    CPSS_COM_ROUTE_T *pstRoute;
    UINT8 aucBuf[8] = {0,0,0,0,0,0,0,0};
    UINT32 ulRouteNum = 0;

    cps__oams_shcmd_printf("Local phyAddr: 0x%08x, logicAddr: 0x%08x: AddrFlag: 0x%08x:\n",
        *(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,
        *(UINT32*)&g_pstComData->stAddrThis.stLogiAddr,
        *(UINT32*)&g_pstComData->stAddrThis.ulAddrFlag);

    while(ulIdx<CPSS_COM_MAX_LINK_NUM)
    {
        INT32 lRet;
        pstRoute = g_pstComRouteTbl + ulIdx;
        lRet = cpss_mem_memcmp(pstRoute, aucBuf, 8);
        if(0 == lRet)
        {
            ulIdx = ulIdx + 1;
            continue;
        }
        cpss_com_idx2logaddr(ulIdx, &stLogicAddr.usGroup);
        {
            cps__oams_shcmd_printf("RouteIdx:%d: logAddr.Group:0x%08x,Master:0x%08x,Slave:0x%08x\n",
                        ulIdx,
                        *(UINT16*)&stLogicAddr.usGroup,
                        *(UINT32*)&pstRoute->stPhyAddr[0],
                        *(UINT32*)&pstRoute->stPhyAddr[1]);
        }
        ulIdx = ulIdx + 1;
        ulRouteNum = ulRouteNum + 1;
    }

    cps__oams_shcmd_printf("Now Local Board has total RouteTable: %d\n\n", ulRouteNum);
#else
    cps__oams_shcmd_printf("cpss_com_route_show can not support!\n\n");
#endif
}


/*******************************************************************************
* 函数名称: cpss_com_store_show
* 功    能: 显示特定的路由缓存发送的数据情况
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* ulLogAddr           UINT32          输入              输入的逻辑地址
* 返回值:
*        无
* 说   明:
*
*******************************************************************************/
INT32 cpss_com_store_show(UINT32 ulLogAddr)
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_LOGIC_ADDR_T stLogAddr ;
    CPSS_COM_ROUTE_T *pstRoute ;
    CPSS_COM_STORE_NODE_T *pstStoreNodeM ;
    CPSS_COM_STORE_NODE_T *pstStoreNodeS ;
    UINT32 ulLoop ;

    stLogAddr.ucModule = ulLogAddr>>24 ;
    stLogAddr.ucSubGroup = (ulLogAddr&0x00ff0000)>>16 ;
    stLogAddr.usGroup    = ulLogAddr&0x0000ffff ;

    pstRoute = cpss_com_route_find_real(stLogAddr) ;

    if(pstRoute == NULL)
    {
        cps__oams_shcmd_printf("not find the route(%x)\n\n", ulLogAddr);
        return CPSS_ERR_COM_ROUTE_NOT_EXIST ;
    }

    if(NULL == pstRoute->pstStoreMan)
    {
        cps__oams_shcmd_printf("not malloc the store stat memory(%x)\n",ulLogAddr);
        return CPSS_ERROR ;
    }

    pstStoreNodeM = &pstRoute->pstStoreMan->stStoreNode[0] ;
    pstStoreNodeS = &pstRoute->pstStoreMan->stStoreNode[1] ;

    cps__oams_shcmd_printf("\n------ store info(%x)begin------\n",ulLogAddr);

    for(ulLoop=0;ulLoop<2;ulLoop++)
    {
        cps__oams_shcmd_printf("(%d)ulDataNum=%d\n",ulLoop,pstStoreNodeM->stStoreStat.ulDataNum);
        cps__oams_shcmd_printf("(%d)ulDataSize=%d\n",ulLoop,pstStoreNodeM->stStoreStat.ulDataSize);
        cps__oams_shcmd_printf("(%d)ulDiscardNum=%d\n",ulLoop,pstStoreNodeM->stStoreStat.ulDiscardNum);
        cps__oams_shcmd_printf("(%d)ulSwitchNum=%d\n",ulLoop,pstStoreNodeM->stStoreStat.ulSwitchNum);
        cps__oams_shcmd_printf("(%d)ulTimeoutNum=%d\n",ulLoop,pstStoreNodeM->stStoreStat.ulTimeoutNum);
        cps__oams_shcmd_printf("(%d)ulBufOverFlowNum=%d\n",ulLoop,pstStoreNodeM->stStoreStat.ulBufOverFlowNum);
    }
    cps__oams_shcmd_printf("------ store info(%x)end------\n",ulLogAddr);
#else
    cps__oams_shcmd_printf("cpss_com_store_show can not support!\n\n");
#endif
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_rarp_req_send
* 功    能: 发送反向地址解析请求消息
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
INT32 cpss_com_rarp_req_send(VOID)
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_RARP_REQ_MSG_T stRarpReq;
    BOOL bMcFlag;

    /*通信初始化流程统计*/
    g_stCpssComInitStat.stRarpReqInfo.ulRarpReqFlag = 1 ;

    /* 全局板和从处理器，不需要进行反向地址解析 */
    bMcFlag = cpss_com_rarp_allow();
    if(FALSE == bMcFlag )
    {
        return CPSS_OK;
    }

    /* 外围单板, 发送地址解析消息 */
    cpss_mem_memset(&stRarpReq,0,sizeof(stRarpReq));
    stRarpReq.ulSeqId=cpss_htonl(g_pstComData->ulSeqID);
    stRarpReq.stPhyAddr=g_pstComData->stAddrThis.stPhyAddr;

    /* 发送到主控板主板 */
    if(CPSS_OK == cpss_com_send_phy(g_pstComData->stAddrGcpaM.stPhyAddr,
        CPSS_COM_LINK_MNGR_PROC,
        CPSS_COM_RARP_REQ_MSG,
        (UINT8 *)&stRarpReq,
        sizeof(stRarpReq)))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
            "rarp request message send to master.");
    }
    else
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "rarp request message send to master failed!,GcpaMaddr:0x%08x",
             *(UINT32*)&g_pstComData->stAddrGcpaM.stPhyAddr);
    } /* end if  CPSS_OK == cpss_com_send_phy... */

    /* 发送到主控板备板 */
    if(CPSS_OK == cpss_com_send_phy(g_pstComData->stAddrGcpaS.stPhyAddr,
        CPSS_COM_LINK_MNGR_PROC,
        CPSS_COM_RARP_REQ_MSG,
        (UINT8 *)&stRarpReq,
        sizeof(stRarpReq)))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
            "rarp request message send to slave.");
    }
    else
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "rarp request message send to slave failed!,GcpaSaddr:0x%08x",
            *(UINT32*)&g_pstComData->stAddrGcpaS.stPhyAddr);
    } /* end if(CPSS_OK == cpss_com_send_phy */

    /* 启动反向地址解析请求定时器 */
    if(0 == g_pstComData->ulRarpReqTNo)
    {
        cpss_timer_loop_set(CPSS_COM_LINK_RARP_TM_NO,CPSS_COM_LINK_RARP_TM_INTERVAL);
        g_pstComData->ulRarpReqTNo=CPSS_COM_LINK_RARP_TM_NO;
    }

    /*通信初始化流程统计*/
    g_stCpssComInitStat.stRarpReqInfo.ulRarpReqNum = g_stCpssComInitStat.stRarpReqInfo.ulRarpReqNum + 1 ;
    g_stCpssComInitStat.stRarpReqInfo.stRarpPhyAddr = stRarpReq.stPhyAddr ;
    cpss_clock_get(&g_stCpssComInitStat.stRarpReqInfo.stTime) ;
#endif
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_rarp_req_recv
* 功    能: 接收到反向地址解析请求消息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstMsgHead CPSS_COM_MSG_HEAD_T*   输入           分发层消息头
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
BOOL cpss_com_local_is_mmc()
{
    BOOL bFlag;

    bFlag = cpss_local_funcbrd_type_is_mc();
    if (TRUE == bFlag)
    {
        if (CPSS_COM_ADDRFLAG_MASTER == g_pstComData->stAddrThis.ulAddrFlag)
        {
            return (TRUE);
        }
    }

    return (FALSE);
}

#ifdef CPSS_FUNBRD_MC
INT32 cpss_com_rarp_req_recv
(
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
    CPSS_COM_RARP_REQ_MSG_T *pstRarpReq;
    CPSS_COM_RARP_RSP_MSG_T   stRarpRsp;
    CPS__RDBS_DEV_GETLOGICADDR_MSG_REQ_T stRdbsReq;
    CPS__RDBS_DEV_GETLOGICADDR_MSG_RSP_T stRdbsRsp;

    UINT32 ulIp;
    CPSS_COM_DRV_T  *pstDrv=cpss_com_drv_find(CPSS_COM_DRV_UDP);
    CPSS_COM_LINK_T *pstLink;
    BOOL bMcFlag;

    /*如果单板状态不正常则不发送RARP响应*/
    if(g_ulCpssBrdNormalFlag == FALSE)
    {
        return CPSS_OK ;
    }

    /* 外围单板和备用主控板, 不需要处理反向地址解析消息 */
    bMcFlag = cpss_com_local_is_mmc();
    if(FALSE == bMcFlag)
    {
        return CPSS_ERROR;
    }

    pstRarpReq=(CPSS_COM_RARP_REQ_MSG_T *)pstMsgHead->pucBuf;

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "rarp request received, resolve phyAddr 0x%08x.",
        *(UINT32*)&pstRarpReq->stPhyAddr);

    /* 调用RDBS接口函数,获得逻辑地址 */
    stRdbsReq.stCpuPhyAddr=pstRarpReq->stPhyAddr;
    {
        cps__rdbs_if_entry(CPS__RDBS_DEV_GETLOGICADDR_MSG,CPS__RDBS_IF_FUNC,
            (UINT8*)&stRdbsReq,sizeof(stRdbsReq),
            (UINT8*)&stRdbsRsp,sizeof(stRdbsRsp));
    }

    if(stRdbsRsp.ulResult != 0)
    {
        if(g_ulRarpNum >= CPSS_CPS__RDBS_INFO_RECORD)
        {
            g_ulRarpNum =0;
        }
        RdbsRarpLinkInfo[g_ulRarpNum] = *(UINT32*)&stRdbsReq.stCpuPhyAddr;
        g_ulRarpNum = g_ulRarpNum + 1;
        return CPSS_ERR_COM_CPS__RDBS_ENTRY_FAIL;
    }

    /* 找到物理地址对应的链路 */
    pstLink=cpss_com_link_find_real(pstRarpReq->stPhyAddr);
    if(NULL == pstLink)
    {
        if(CPSS_OK != cpss_com_link_create(pstRarpReq->stPhyAddr,CPSS_COM_DRV_UDP))
        {
            return CPSS_ERROR;
        }
        pstLink=cpss_com_link_find_real(pstRarpReq->stPhyAddr);
    }

    /* 保存链路参数 */
    cpss_com_phy2ip(pstRarpReq->stPhyAddr,&ulIp,FALSE);

    pstDrv->stSocketAddr.sin_addr.s_addr = ulIp;
    cpss_mem_memcpy(&pstLink->stDrv, pstDrv, sizeof(CPSS_COM_DRV_T));
  /*  pstLink->stDrv.stSocketAddr.sin_addr.s_addr = ulIp;*/
 
    pstLink->usStatus=CPSS_COM_LINK_STATUS_NORMAL;

    /* 发送反向地址解析响应消息 */
    stRarpRsp.ulSeqId       = pstRarpReq->ulSeqId;
    stRarpRsp.ulResult      = CPSS_OK;
    stRarpRsp.bIsMateExist  = (CPS__RDBS_DEV_11_BACKUP==stRdbsRsp.ucBackupType);
    stRarpRsp.stLogicAddr   = stRdbsRsp.stCpuLogicAddr;
    stRarpRsp.stMatePhyAddr = stRdbsRsp.stMateBrdPhyAddr;
    stRarpRsp.stGcpaPhyAddr = g_pstComData->stAddrThis.stPhyAddr;

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "rarp rsponse send, phyAddr 0x%08x -> logicAddr 0x%08x "
        "          matePhyAddr 0x%08x, GCPA phyAddr 0x%08x",
        *(UINT32*)&pstRarpReq->stPhyAddr,*(UINT32*)&stRarpRsp.stLogicAddr,
        *(UINT32*)&stRarpRsp.stGcpaPhyAddr,*(UINT32*)&stRarpRsp.stMatePhyAddr);

    stRarpRsp.stLogicAddr.usGroup=cpss_ntohs(stRarpRsp.stLogicAddr.usGroup);

    if(CPSS_OK != cpss_com_send_phy(pstRarpReq->stPhyAddr,
        CPSS_COM_LINK_MNGR_PROC,
        CPSS_COM_RARP_RSP_MSG,
        (UINT8*)&stRarpRsp,
        sizeof(stRarpRsp)))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "rarp rsponse message send failed!");
        return CPSS_ERROR;
    }

    return CPSS_OK;
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_rarp_rsp_recv
* 功    能: 接收到反向地址解析响应消息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstMsgHead CPSS_COM_MSG_HEAD_T*   输入           分发层消息头
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_rarp_rsp_recv
(
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_LINK_T *pstLink;
    CPSS_COM_RARP_RSP_MSG_T *pstRarpRsp;
    BOOL bMcFlag;

    /*通信初始化流程统计*/
    g_stCpssComInitStat.stRarpRspInfo.ulRarpRspFlag = 1 ;

    /* 全局板不处理反向地址解析响应消息 */
    bMcFlag = cpss_com_rarp_allow();
    if(FALSE == bMcFlag)
    {
        return CPSS_ERROR;
    }

    /* 得到本板的逻辑地址 */
    pstRarpRsp=(CPSS_COM_RARP_RSP_MSG_T *)pstMsgHead->pucBuf;
    pstRarpRsp->ulSeqId=cpss_htonl(pstRarpRsp->ulSeqId);
    pstRarpRsp->stLogicAddr.usGroup=cpss_ntohs(pstRarpRsp->stLogicAddr.usGroup);

    /*通信初始化流程统计*/
    g_stCpssComInitStat.stRarpRspInfo.ulRarpRspResult = pstRarpRsp->ulResult ;
    g_stCpssComInitStat.stRarpRspInfo.ulMateFlag = pstRarpRsp->bIsMateExist ;
    g_stCpssComInitStat.stRarpRspInfo.stGcpaPhyAddrM = pstRarpRsp->stGcpaPhyAddr ;
    g_stCpssComInitStat.stRarpRspInfo.stMatePhyAddr  = pstRarpRsp->stMatePhyAddr ;
    g_stCpssComInitStat.stRarpRspInfo.stRarpLogAddr = pstRarpRsp->stLogicAddr ;
    g_stCpssComInitStat.stRarpRspInfo.ulRarpRspRecvNum = g_stCpssComInitStat.stRarpRspInfo.ulRarpRspRecvNum + 1 ;

    /* 检查消息序列号 */
    if(pstRarpRsp->ulSeqId != g_pstComData->ulSeqID)
    {
        return CPSS_ERROR;
    }
    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "rarp rsponse message received.");

    /* 设置伙伴板不存在 */
    g_pstComData->stMatePhyAddr           = g_stComPhyAddrZero;
    g_pstComData->stAddrThis.ucBackupType = CPS__RDBS_DEV_NO_BACKUP;

    /* 保存反向地址解析消息内容 */
    if(CPSS_OK==pstRarpRsp->ulResult)
    {
        if(pstRarpRsp->bIsMateExist)
        {
            g_pstComData->stMatePhyAddr           = pstRarpRsp->stMatePhyAddr;
            g_pstComData->stAddrThis.ucBackupType = CPS__RDBS_DEV_11_BACKUP;
        }
        g_pstComData->stAddrThis.stLogiAddr=pstRarpRsp->stLogicAddr;

        /* 设置到全局板的链路有效 */
        pstLink=cpss_com_link_find_real(pstRarpRsp->stGcpaPhyAddr);
        pstLink->usStatus=CPSS_COM_LINK_STATUS_NORMAL;

        /* 如果响应消息的全局板跟本板全局板信息不一致,则更新 */
        {
            CPSS_COM_PHY_ADDR_T stPhyAddr;

           if(0 == CPSS_COM_LOGIC_ADDR_SAME(g_pstComData->stAddrGcpaM.stPhyAddr,
                pstRarpRsp->stGcpaPhyAddr))
            {
               UINT32 ulIpAddr ;

               cpss_mem_memcpy(&stPhyAddr,
                    &g_pstComData->stAddrGcpaM.stPhyAddr,
                    sizeof(CPSS_COM_PHY_ADDR_T));
                cpss_mem_memcpy(&g_pstComData->stAddrGcpaM.stPhyAddr,
                    &g_pstComData->stAddrGcpaS.stPhyAddr,
                    sizeof(CPSS_COM_PHY_ADDR_T));
                cpss_mem_memcpy(&g_pstComData->stAddrGcpaS.stPhyAddr,
                    &stPhyAddr,
                    sizeof(CPSS_COM_PHY_ADDR_T));

                /*更新IP地址信息*/
                ulIpAddr = g_pstComData->stAddrGcpaM.ulIpAddr ;
                g_pstComData->stAddrGcpaM.ulIpAddr = g_pstComData->stAddrGcpaS.ulIpAddr ;
                g_pstComData->stAddrGcpaS.ulIpAddr = ulIpAddr ;

                /*更新GCPA全局变量的地址*/
                cpss_mem_memcpy(&stPhyAddr,&g_stPhyAddrGcpaMaster,sizeof(CPSS_COM_PHY_ADDR_T)) ;
                cpss_mem_memcpy(&g_stPhyAddrGcpaMaster,&g_stPhyAddrGcpaSlave,sizeof(CPSS_COM_PHY_ADDR_T)) ;
                cpss_mem_memcpy(&g_stPhyAddrGcpaSlave,&stPhyAddr,sizeof(CPSS_COM_PHY_ADDR_T)) ;
            }
        }

        /* 删除反向地址解析定时器 */
        if(0 != g_pstComData->ulRarpReqTNo)
        {
            cpss_timer_delete(CPSS_COM_LINK_RARP_TM_NO);
            g_pstComData->ulRarpReqTNo=0;
        }

        {
            /* 向SMSS发送上电消息 */
            cpss_com_power_on_send();
        }
    }  /* end if  (CPSS_OK==pstRarpRsp->ulResult) */

    /* 收到正确的响应,序号递增 */
    g_pstComData->ulSeqID = g_pstComData->ulSeqID + 1;

    /*通信初始化流程统计*/
    g_stCpssComInitStat.stRarpRspInfo.ulRarpRspFlag = 2 ;
    cpss_clock_get(&g_stCpssComInitStat.stRarpRspInfo.stTime) ;

#endif
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_power_on_send
* 功    能: 向SMSS发送上电消息
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

INT32 cpss_com_power_on_send(VOID)
{
    CPSS_COM_POWER_ON_MSG_T stMsg;

    static BOOL bFirstSend=TRUE;

    CPSS_DSP_FILE_REC();
    CPSS_DSP_LINE_REC();
    CPSS_DSP_RUN_ADDR_REC(CPSS_DSP_POWER_ON_END);
    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulPowerOnInCount);

    /*通信初始化流程统计*/
#ifndef CPSS_DSP_CPU
    g_stCpssComInitStat.stPowerOnInfo.ulPowerOnFlag = 1 ;
    g_stCpssComInitStat.stPowerOnInfo.stPowerOnPhyAddr = g_pstComData->stAddrThis.stPhyAddr ;
#endif

    g_ulCpssComInitFlag = CPSS_COM_INIT_OK ;

    /*if(!bFirstSend)*/
    if(FALSE == bFirstSend)
    {
        return CPSS_ERROR;
    }

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "power on message send.");

    /* 填写上电消息参数 */
    stMsg.bMateExist=FALSE;
    stMsg.stMatePhyAddr=g_stComPhyAddrZero;

   if(0 == CPSS_COM_PHY_ADDR_ZERO(g_pstComData->stMatePhyAddr))
    {
        stMsg.bMateExist=TRUE;
        stMsg.stMatePhyAddr=g_pstComData->stMatePhyAddr;
    }

    /* 发送地址解析请求消息 */
    if(CPSS_OK == cpss_com_send_phy(g_pstComData->stAddrThis.stPhyAddr,
        SMSS_SYSCTL_PROC,
        CPSS_COM_POWER_ON_MSG,
        (UINT8*)&stMsg,
        sizeof(CPSS_COM_POWER_ON_MSG_T)))
    {
        bFirstSend = FALSE;

        /*通信初始化流程统计*/
#ifndef CPSS_DSP_CPU
        g_stCpssComInitStat.stPowerOnInfo.ulPowerOnFlag = 2 ;
        cpss_clock_get(&g_stCpssComInitStat.stPowerOnInfo.stTime) ;
#endif
        CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulPowerOnOutCount);
        return CPSS_OK;
    }

    CPSS_DSP_LINE_REC();
    CPSS_DSP_ERROR_WAIT;

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
        "\n Send Power on Message Fail!!!!!!!!.");

    return CPSS_ERROR;
}

/*******************************************************************************
* 函数名称: cpss_com_arp_req_send
* 功    能: 发送地址解析请求消息到主控板，期望得到某个逻辑地址的物理地址
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstArpReq    CPSS_COM_ARP_REQ_MSG_T*   输入            地址解析请求消息体
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
******************************************************************************/
INT32 cpss_com_arp_req_send
(
 CPSS_COM_ARP_REQ_MSG_T *pstArpReq
 )
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_PID_T stDstPid;

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "arp request send, resolve logicAddr 0x%08x %d.",
        *(UINT32*)&pstArpReq->stLogicAddr,
        pstArpReq->ulAddrFlag);

    /* 目的地址为主用全局板 */
/**
    cpss_mem_memset(&stDstPid,0,sizeof(stDstPid));
**/
    stDstPid.stLogicAddr = g_pstComData->stAddrGcpaM.stLogiAddr;
    stDstPid.ulAddrFlag  = CPSS_COM_ADDRFLAG_MASTER;
    stDstPid.ulPd        = CPSS_COM_LINK_MNGR_PROC;

    /*增加ARP统计*/
    cpss_com_arp_item_add(NULL,&pstArpReq->stLogicAddr,
        pstArpReq->ulAddrFlag,CPSS_COM_ARP_STAT_REQ_ADD);

    pstArpReq->ulSeqId             = cpss_htonl(g_pstComData->ulSeqID);
    pstArpReq->stLogicAddr.usGroup = cpss_htons(pstArpReq->stLogicAddr.usGroup);
    pstArpReq->ulAddrFlag          = cpss_htonl(pstArpReq->ulAddrFlag);

    /* 发送地址解析请求消息 */
    return cpss_com_send(&stDstPid,
        CPSS_COM_ARP_REQ_MSG,
        (UINT8*)pstArpReq,
        sizeof(CPSS_COM_ARP_REQ_MSG_T));
#else
    return CPSS_OK;
#endif
}

/*******************************************************************************
* 函数名称: cpss_com_arp_req_recv
* 功    能: 接收到地址解析请求消息，创建路由，初始化滑窗
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstMsgHead CPSS_COM_MSG_HEAD_T*   输入           分发层消息头
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
******************************************************************************/
#ifdef CPSS_FUNBRD_MC
INT32 cpss_com_arp_req_recv
(
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
    CPSS_COM_ARP_REQ_MSG_T *pstArpReq;
    CPSS_COM_ARP_RSP_MSG_T   stArpRsp;
    CPS__RDBS_DEV_GETPHYADDR_MSG_REQ_T stRdbsReq;
    CPS__RDBS_DEV_GETPHYADDR_MSG_RSP_T stRdbsRsp;
    BOOL bMcFlag;

    /* 主用主控板才处理地址解析消息 */
    bMcFlag = cpss_com_local_is_mmc();
    if(FALSE == bMcFlag)
    {
        return CPSS_ERROR;
    }
    pstArpReq                      = (CPSS_COM_ARP_REQ_MSG_T *)pstMsgHead->pucBuf;
    pstArpReq->stLogicAddr.usGroup = cpss_ntohs(pstArpReq->stLogicAddr.usGroup);
    pstArpReq->ulAddrFlag          = cpss_ntohl(pstArpReq->ulAddrFlag);

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "arp request received, resolve logicAddr 0x%08x %d from LogAddr = %#x.",
        *(UINT32*)&pstArpReq->stLogicAddr,
        pstArpReq->ulAddrFlag,
        *(UINT32*)&pstMsgHead->stSrcProc.stLogicAddr);

    /* 调用RDBS接口函数,获得物理地址 */
    stRdbsReq.stCpuLogicAddr=pstArpReq->stLogicAddr;
    stRdbsReq.ulMasterOrSlave= pstArpReq->ulAddrFlag;
    {
        cps__rdbs_if_entry(CPS__RDBS_DEV_GETPHYADDR_MSG, CPS__RDBS_IF_FUNC,
            (UINT8*)&stRdbsReq, sizeof(stRdbsReq),
            (UINT8*)&stRdbsRsp, sizeof(stRdbsRsp));
    }

    if(stRdbsRsp.ulResult != 0)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "arp resolved failed: logicAddr 0x%08x %d.",
        *(UINT32*)&pstArpReq->stLogicAddr,
        pstArpReq->ulAddrFlag);
        if(g_ulArpNum >= CPSS_CPS__RDBS_INFO_RECORD)
        {
            g_ulArpNum=0;
        }
        RdbsArpLinkInfo[g_ulArpNum] = *(UINT32*)&pstArpReq->stLogicAddr;
        g_ulArpNum = g_ulArpNum + 1;
        return CPSS_ERR_COM_CPS__RDBS_ENTRY_FAIL;
    }

    /* 填写地址解析响应消息 */
    cpss_mem_memset(&stArpRsp,0,sizeof(CPSS_COM_ARP_RSP_MSG_T));
    stArpRsp.ulSeqId     = pstArpReq->ulSeqId;
    stArpRsp.ulResult    = 0;
    stArpRsp.stPhyAddr   = stRdbsRsp.stCpuPhyAddr;
    stArpRsp.stLogicAddr = pstArpReq->stLogicAddr;
    stArpRsp.ulAddrFlag  = pstArpReq->ulAddrFlag;

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "arp response send, logicAddr 0x%08x %d -> phyAddr 0x%08x",
        *(UINT32*)&stArpRsp.stLogicAddr,
        stArpRsp.ulAddrFlag,
        *(UINT32*)&stArpRsp.stPhyAddr);

    stArpRsp.stLogicAddr.usGroup = cpss_htons(stArpRsp.stLogicAddr.usGroup);
    stArpRsp.ulAddrFlag          = cpss_htonl(stArpRsp.ulAddrFlag);
    stArpRsp.ulResult            = cpss_htonl(stArpRsp.ulResult);

    /* 发送地址解析响应消息 */
    return cpss_com_send(&pstMsgHead->stSrcProc,
        CPSS_COM_ARP_RSP_MSG,
        (UINT8*)&stArpRsp,
        sizeof(CPSS_COM_ARP_RSP_MSG_T));
}
#endif

/*******************************************************************************
* 函数名称: cpss_com_route_arp_flag_set
* 功    能: 设置路由表中的arp标志
* 相关文档:
* 函数类型:
* 参    数:
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_route_arp_flag_set
(
 CPSS_COM_LOGIC_ADDR_T stLogicAddr,
 UINT32 ulAddrFlag
)
{
    CPSS_COM_ROUTE_T* pstRoute = NULL ;

    /*参数校验*/
    if((ulAddrFlag != CPSS_COM_ADDRFLAG_MASTER)
        &&(ulAddrFlag != CPSS_COM_ADDRFLAG_SLAVE))
    {
        return CPSS_OK ;
    }

    /*查找路由*/
    pstRoute = cpss_com_route_find_real(stLogicAddr) ;
    if(pstRoute == NULL)
    {
        return CPSS_ERROR ;
    }

    /*设置ARP标志*/
    pstRoute->ausArpFlag[ulAddrFlag] = CPSS_ARP_REQ_PROCESSED ;

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_route_arp_flag_get
* 功    能: 得到路由表中的arp标志
* 相关文档:
* 函数类型:
* 参    数:
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_route_arp_flag_get
(
 CPSS_COM_LOGIC_ADDR_T stLogicAddr,
 UINT32 ulAddrFlag
)
{
    CPSS_COM_ROUTE_T* pstRoute = NULL ;

    /*参数校验*/
    if((ulAddrFlag != CPSS_COM_ADDRFLAG_MASTER)
        &&(ulAddrFlag != CPSS_COM_ADDRFLAG_SLAVE))
    {
        return CPSS_ERROR ;
    }

    /*查找路由*/
    pstRoute = cpss_com_route_find_real(stLogicAddr) ;
    if(pstRoute == NULL)
    {
        return CPSS_ERROR ;
    }

    /*得到ARP标志*/
    return (pstRoute->ausArpFlag[ulAddrFlag]) ;
}

/*******************************************************************************
* 函数名称: cpss_com_arp_rsp_recv
* 功    能: 接收到地址解析响应消息,创建路由
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstMsgHead CPSS_COM_MSG_HEAD_T*   输入           分发层消息头
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
******************************************************************************/
INT32 cpss_com_arp_rsp_recv
(
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_ARP_RSP_MSG_T *pstArpRsp;

    /* 获取消息内容,字节序转换 */
    pstArpRsp=(CPSS_COM_ARP_RSP_MSG_T *)pstMsgHead->pucBuf;

    pstArpRsp->ulSeqId             = cpss_ntohl(pstArpRsp->ulSeqId);
    pstArpRsp->stLogicAddr.usGroup = cpss_ntohs(pstArpRsp->stLogicAddr.usGroup);
    pstArpRsp->ulAddrFlag          = cpss_ntohl(pstArpRsp->ulAddrFlag);
    pstArpRsp->ulResult            = cpss_ntohl(pstArpRsp->ulResult);

    /*增加ARP统计*/
    cpss_com_arp_item_add(&pstArpRsp->stPhyAddr,&pstArpRsp->stLogicAddr,
        pstArpRsp->ulAddrFlag,CPSS_COM_ARP_STAT_RSP_ADD) ;

    /* 检查消息序列号 */

    /* 检查地址解析结果 */
    if(CPSS_OK != pstArpRsp->ulResult)
    {
        return CPSS_ERROR;
    }

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "arp response received, logicAddr 0x%08x %d -> 0x%08x.",
        *(UINT32*)&pstArpRsp->stLogicAddr,pstArpRsp->ulAddrFlag,
        *(UINT32*)&pstArpRsp->stPhyAddr);

    /* 收到正确的响应,序号递增 */
    g_pstComData->ulSeqID = g_pstComData->ulSeqID + 1;

    /*设置ARP过程标志*/
    cpss_com_route_arp_flag_set(pstArpRsp->stLogicAddr,pstArpRsp->ulAddrFlag) ;

    /* 收到正确的地址解析响应之后，进行建立链路过程 */
    cpss_com_link_req_send(pstArpRsp->stPhyAddr,CPSS_COM_CTRL_CHANNEL);
#endif
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_app_arp_req_recv
* 功    能: 链路管理纤程接收到应用接口发送的地址解析请求消息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstMsgHead CPSS_COM_MSG_HEAD_T*   输入           分发层消息头
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
******************************************************************************/
INT32 cpss_com_app_arp_req_recv
(
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_ARP_REQ_MSG_T *pstArpReq=(CPSS_COM_ARP_REQ_MSG_T *)pstMsgHead->pucBuf;

    /* 发送地址解析消息 */
    return cpss_com_arp_req_send(pstArpReq);
#else
   return CPSS_OK;
#endif
}

/*******************************************************************************
* 函数名称: cpss_com_app_arp_req_send
* 功    能: 向本CPU链路管理纤程发送地址解析请求消息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstMsgHead CPSS_COM_MSG_HEAD_T*   输入           分发层消息头
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
******************************************************************************/
INT32 cpss_com_app_arp_req_send
(
 CPSS_COM_LOGIC_ADDR_T stLogicAddr,
 UINT32                ulAddrFlag
 )
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_PID_T stDstPid;
    CPSS_COM_ARP_REQ_MSG_T stArpReq;

    /* 向链路管理纤程发送地址解析消息 */
    stArpReq.stLogicAddr = stLogicAddr;
    stArpReq.ulAddrFlag  = ulAddrFlag;
    stArpReq.ulSeqId = 0;

    stDstPid.stLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
    stDstPid.ulAddrFlag  = g_pstComData->stAddrThis.ulAddrFlag;
    stDstPid.ulPd        = CPSS_COM_LINK_MNGR_PROC;
    return cpss_com_send(&stDstPid,
        CPSS_COM_APP_ARP_REQ_MSG,
        (UINT8*)&stArpReq,
        sizeof(stArpReq));
#else
    return CPSS_OK;
#endif
}



/*******************************************************************************
* 函数名称: cpss_com_timer_heartbeat_start
* 功    能: 启动心跳定时器
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstLink          CPSS_COM_LINK_T *   输入           链路指针
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
******************************************************************************/
#if 0
INT32 cpss_com_timer_heartbeat_start(CPSS_COM_LINK_T *pstLink)
{
#if CPSS_COM_LINK_HEART_ENABLE

    /* 检查参数是否有效 */
    if((NULL == pstLink) || CPSS_COM_PHY_ADDR_ZERO(pstLink->stDstPhyAddr))
    {
        return CPSS_ERROR;
    }

    /* 删除旧的心跳定时器 */
    if((pstLink->ulHeartTd != 0) &&
        (pstLink->ulHeartTd != CPSS_TD_INVALID))
    {
        if(CPSS_OK != cpss_timer_para_delete(pstLink->ulHeartTd))
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "delete heartbeat td failed!");
            return CPSS_ERROR;
        }
    }

    /* 重新启动链路心跳定时器 */
    pstLink->ulHeartTd=
        cpss_timer_para_loop_set(CPSS_COM_LINK_HEART_TM_NO,
        CPSS_COM_LINK_HEART_TM_INTERVAL,
        *(UINT32*)&pstLink->stDstPhyAddr);

    if(pstLink->ulHeartTd == CPSS_TD_INVALID)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "create heartbeat td failed!");
        return CPSS_ERROR;
    }
#endif
    return CPSS_OK;
}
#endif

/*******************************************************************************
* 函数名称: cpss_com_timer_link_fault_start
* 功    能: 启动链路故障定时器
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstLink          CPSS_COM_LINK_T *   输入           链路指针
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
******************************************************************************/
#if 0
INT32 cpss_com_timer_link_fault_start(CPSS_COM_LINK_T *pstLink)
{
#if CPSS_COM_LINK_FAULT_ENABLE

    /* 检查参数是否有效 */
    if((NULL == pstLink) || CPSS_COM_PHY_ADDR_ZERO(pstLink->stDstPhyAddr))
    {
        return CPSS_ERROR;
    }

    /* 删除旧的心跳定时器 */
    if((pstLink->ulFaultTd != 0) &&
        (pstLink->ulFaultTd != CPSS_TD_INVALID))
    {
        if(CPSS_OK != cpss_timer_para_delete(pstLink->ulFaultTd))
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "delete link fault td failed!");
            return CPSS_ERROR;
        }
    }

    /* 重新启动链路故障定时器 */
    pstLink->ulFaultTd=
        cpss_timer_para_set(CPSS_COM_LINK_FAULT_TM_NO,
        CPSS_COM_LINK_FAULT_TM_INTERVAL,
        *(UINT32*)&pstLink->stDstPhyAddr);

    if(pstLink->ulFaultTd == CPSS_TD_INVALID)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "create link fault td failed!");
        return CPSS_ERROR;
    }
#endif
    return CPSS_OK;
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_link_req_send
* 功    能: 向指定逻辑地址，发送链路请求消息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型                输入/输出         描述
* stDstPhyAddr CPSS_COM_PHY_ADDR_T        输入            逻辑地址
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:在发送链路建立请求之前，路由表中应该存放了对端的逻辑地址、物理地址信息
******************************************************************************/
INT32 cpss_com_link_req_send
(
 CPSS_COM_PHY_ADDR_T stDstPhyAddr,
 UINT32 ulMateFlag
 )
{
    CPSS_COM_LINK_T *pstLink=cpss_com_link_find_real(stDstPhyAddr);
    CPSS_COM_LINK_REQ_MSG_T stReqMsg;
    UINT32 ulMsgId ;

    /* 判断链路的是否有效 */
    if(NULL == pstLink)
    {
        if(CPSS_OK != cpss_com_link_create(stDstPhyAddr,CPSS_COM_DRV_UDP))
        {
            return CPSS_ERROR;
        }
        pstLink=cpss_com_link_find_real(stDstPhyAddr);
    }

    pstLink->ulMateFlag = ulMateFlag ;

    cpss_mem_memset(&stReqMsg,0,sizeof(stReqMsg));

    /*设置链路请求的msgid*/
    if(ulMateFlag == CPSS_COM_CTRL_CHANNEL)
    {
        ulMsgId = CPSS_COM_LINK_REQ_MSG ;
    }
    else
    {
        ulMsgId = CPSS_COM_MATE_CHANNEL_REQ_MSG ;
        cpss_mem_memcpy(&stReqMsg.stGcpaPhyAddrM,
            &g_pstComData->stAddrGcpaM.stPhyAddr,sizeof(CPSS_COM_PHY_ADDR_T)) ;
    }

    /* 建链请求消息，带给自身的逻辑地址、物理地址、主备标记、伙伴板物理地址 */
    stReqMsg.stLogicAddr   = g_pstComData->stAddrThis.stLogiAddr;
    stReqMsg.stPhyAddr     = g_pstComData->stAddrThis.stPhyAddr;
    stReqMsg.ulAddrFlag    = g_pstComData->stAddrThis.ulAddrFlag;
    stReqMsg.stMatePhyAddr = g_pstComData->stMatePhyAddr;
    stReqMsg.ulMateFlag    = ulMateFlag;
    stReqMsg.ulMasterTransmitFlag= CPSS_COM_NO_MASTER_TRANSMIT ;

#ifdef CPSS_SWITCH_BRDTYPE
    stReqMsg.ulMasterTransmitFlag = CPSS_COM_MASTER_TRANSMIT ;
#endif

    /* 字节序转换 */
    stReqMsg.ulSeqId             = cpss_htonl(g_pstComData->ulSeqID);
    stReqMsg.stLogicAddr.usGroup = cpss_htons(stReqMsg.stLogicAddr.usGroup);
    stReqMsg.ulAddrFlag          = cpss_htonl(stReqMsg.ulAddrFlag);
    stReqMsg.ulMateFlag          = cpss_htonl(stReqMsg.ulMateFlag);
    stReqMsg.ulMasterTransmitFlag= cpss_htonl(stReqMsg.ulMasterTransmitFlag);

//#if 0
    /* 启动链路建立定时器 */
    if((0 == pstLink->ulLinkReqTd) ||
        (CPSS_TD_INVALID == pstLink->ulLinkReqTd))
    {
        pstLink->ulLinkReqTd=cpss_timer_para_loop_set(CPSS_COM_LINK_REQ_TM_NO,
            CPSS_COM_LINK_REQ_TM_INTERVAL,
            *(UINT32*)&stDstPhyAddr);
    }
//#endif

    /* 发送建立链接请求消息 */
    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_DETAIL,
        "link request send, phyAddr 0x%08x -> 0x%08x.",
        *(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,
        *(UINT32*)&stDstPhyAddr);

    return cpss_com_send_phy(stDstPhyAddr,
        CPSS_COM_LINK_MNGR_PROC,
        ulMsgId,
        (UINT8*)&stReqMsg,
        sizeof(CPSS_COM_LINK_REQ_MSG_T));
}

/*******************************************************************************
* 函数名称: cpss_com_swtich_route_add
* 功    能: 增加去交换板的路由
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
#ifndef CPSS_DSP_CPU
#ifdef CPSS_VOS_VXWORKS
INT32 cpss_com_swtich_route_deal
(
 CPSS_COM_PHY_ADDR_T stMPhyAddr,
 CPSS_COM_PHY_ADDR_T stSPhyAddr,
 UINT32 ulDealFlag
)
{
    UINT32 ulMIpAddrC ;
    UINT32 ulSIpAddrU ;
    UINT8 aucMIpAddrCStr[20] ;
    UINT8 aucSIpAddrUStr[20] ;
    struct in_addr stMAddr = {0} ;
    struct in_addr stSAddr = {0} ;

    /*得到主用的控制面IP*/
    cpss_com_phy2ip(stMPhyAddr,&ulMIpAddrC,FALSE) ;

    /*得到备用的UPDATE面IP*/
    cpss_com_phy2ip(stSPhyAddr,&ulSIpAddrU,TRUE) ;

    /*转化IP为字符串形式*/
    stSAddr.s_addr  = ulSIpAddrU ;
    stMAddr.s_addr  = ulMIpAddrC ;
    inet_ntoa_b(stSAddr,aucSIpAddrUStr) ;
    inet_ntoa_b(stMAddr,aucMIpAddrCStr) ;

    if(ulDealFlag == CPSS_COM_ROUTE_ADD)
    {
        /*增加路由*/
        routeAdd(aucSIpAddrUStr,aucMIpAddrCStr) ;

        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "gcpa add route dest=%s,gate=%s\n",
            aucSIpAddrUStr,aucMIpAddrCStr) ;
    }
    else if(ulDealFlag == CPSS_COM_ROUTE_DEL)
    {
        /*删除路由*/
        routeDelete(aucSIpAddrUStr,aucMIpAddrCStr) ;

        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "gcpa del route dest=%s,gate=%s\n",
            aucSIpAddrUStr,aucMIpAddrCStr) ;
    }
    else
    {
        return CPSS_ERROR ;
    }

    return CPSS_OK ;
}
#endif
#endif
/*******************************************************************************
* 函数名称: cpss_com_link_req_recv
* 功    能: 收到链路请求消息,建立到对方的链路,发送链路响应消息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstMsgHead CPSS_COM_MSG_HEAD_T*   输入           分发层消息头
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_link_req_recv
(
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
    CPSS_COM_LINK_RSP_MSG_T  stRspMsg;
    CPSS_COM_LINK_T         *pstLink;
    CPSS_COM_LINK_REQ_MSG_T *pstReqMsg=(CPSS_COM_LINK_REQ_MSG_T *)pstMsgHead->pucBuf;

    /* 字节序转换 */
    pstReqMsg->stLogicAddr.usGroup = cpss_ntohs(pstReqMsg->stLogicAddr.usGroup);
    pstReqMsg->ulAddrFlag          = cpss_ntohl(pstReqMsg->ulAddrFlag);
    pstReqMsg->ulMasterTransmitFlag= cpss_ntohl(pstReqMsg->ulMasterTransmitFlag) ;

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_DETAIL,
        "link request received, phyAddr 0x%08x <- 0x%08x.",
        *(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,
        *(UINT32*)&pstReqMsg->stPhyAddr);

    /* 建立到对方的链路 */
    if(CPSS_OK != cpss_com_route_create(pstReqMsg->stLogicAddr,
        pstReqMsg->ulAddrFlag,
        pstReqMsg->stPhyAddr,CPSS_COM_DRV_UDP))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "link request message create route failed!");
        return CPSS_ERROR;
    }
    pstLink=cpss_com_link_find_real(pstReqMsg->stPhyAddr);

    /* 建链响应消息，带给对方自身的逻辑地址、物理地址、主备标记 */
    cpss_mem_memset(&stRspMsg,0,sizeof(stRspMsg));
    stRspMsg.ulSeqId       = pstReqMsg->ulSeqId;
    stRspMsg.stLogicAddr   = g_pstComData->stAddrThis.stLogiAddr;
    stRspMsg.stPhyAddr     = g_pstComData->stAddrThis.stPhyAddr;
    stRspMsg.ulAddrFlag    = g_pstComData->stAddrThis.ulAddrFlag;
    stRspMsg.stMatePhyAddr = g_pstComData->stMatePhyAddr;
    stRspMsg.ulResult      = CPSS_OK;
    stRspMsg.ulMasterTransmitFlag = CPSS_COM_NO_MASTER_TRANSMIT ;

#ifdef CPSS_SWITCH_BRDTYPE
    stRspMsg.ulMasterTransmitFlag = CPSS_COM_MASTER_TRANSMIT ;
#endif

    /* 响应消息字节序转换 */
    stRspMsg.stLogicAddr.usGroup = cpss_htons(stRspMsg.stLogicAddr.usGroup);
    stRspMsg.ulAddrFlag          = cpss_htonl(stRspMsg.ulAddrFlag);
    stRspMsg.ulResult            = cpss_htonl(stRspMsg.ulResult);
    stRspMsg.ulMasterTransmitFlag= cpss_htonl(stRspMsg.ulMasterTransmitFlag) ;

    /*设置链路是否需要由主用转发的地址*/
    if(pstReqMsg->ulMasterTransmitFlag == CPSS_COM_MASTER_TRANSMIT)
    {
        pstLink->stMPhyAddr = pstReqMsg->stMatePhyAddr ;
#ifdef CPSS_FUNBRD_MC
#ifdef CPSS_VOS_VXWORKS
        if(pstLink->ulAddrFlag == CPSS_COM_ADDRFLAG_SLAVE)
        {
            /*删除原有路由*/
            cpss_com_swtich_route_deal(pstReqMsg->stPhyAddr,
                pstReqMsg->stMatePhyAddr,CPSS_COM_ROUTE_DEL) ;

            /*增加新路由*/
            cpss_com_swtich_route_deal(pstReqMsg->stMatePhyAddr,
                pstReqMsg->stPhyAddr,CPSS_COM_ROUTE_ADD) ;
        }
#endif
#endif
    }

    /* 设置链路状态为有效 */
    cpss_com_link_status_set(pstReqMsg->stPhyAddr,CPSS_COM_LINK_STATUS_NORMAL);

    /* 发送建立链接响应消息 */
    if(CPSS_OK != cpss_com_send_phy(pstReqMsg->stPhyAddr,
        CPSS_COM_LINK_MNGR_PROC,
        CPSS_COM_LINK_RSP_MSG,
        (UINT8*)&stRspMsg,
        sizeof(CPSS_COM_LINK_RSP_MSG_T)))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "link response message send failed!");
        return CPSS_ERROR;
    }
    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_DETAIL,
        "link response send, phyAddr 0x%08x -> 0x%08x.",
        *(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,
        *(UINT32*)&pstReqMsg->stPhyAddr);

    /* 启动心跳定时器和链路故障定时器*/
#if 0
    cpss_com_timer_heartbeat_start(pstLink);
    cpss_com_timer_link_fault_start(pstLink);
#endif

    /* 如果对方存在伙伴板,并且没有建立链路,则发送建链请求 */
    if(0 == (CPSS_COM_PHY_ADDR_ZERO(pstReqMsg->stMatePhyAddr)))
    {
        CPSS_COM_ROUTE_T *pstRoute=cpss_com_route_find_real(pstReqMsg->stLogicAddr);
        UINT32 ulIdx=(CPSS_COM_ADDRFLAG_MASTER==pstReqMsg->ulAddrFlag)?1:0;
        if (NULL == pstRoute)
        {
            return (CPSS_ERR_COM_ROUTE_NOT_EXIST);
        }
        if(0 != CPSS_COM_PHY_ADDR_ZERO(pstRoute->stPhyAddr[ulIdx]))
        {
            cpss_com_link_req_send(pstReqMsg->stMatePhyAddr,CPSS_COM_CTRL_CHANNEL);
        }
    }

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_link_rsp_recv
* 功    能: 收到链路响应消息,链路建立完成,向SMSS发送纤程激活响应消息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstMsgHead CPSS_COM_MSG_HEAD_T*   输入          分发层消息头
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_link_rsp_recv
(
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
    CPSS_COM_LINK_T *pstLink;
    CPSS_COM_LINK_RSP_MSG_T *pstRspMsg;

    /* 获取消息内容,返回失败 */
    pstRspMsg=(CPSS_COM_LINK_RSP_MSG_T *)pstMsgHead->pucBuf;

    /* 字节序转换 */
    pstRspMsg->ulSeqId             = cpss_ntohl(pstRspMsg->ulSeqId);
    pstRspMsg->stLogicAddr.usGroup = cpss_ntohs(pstRspMsg->stLogicAddr.usGroup);
    pstRspMsg->ulAddrFlag          = cpss_ntohl(pstRspMsg->ulAddrFlag);
    pstRspMsg->ulResult            = cpss_ntohl(pstRspMsg->ulResult);

    /* 检查消息序列号 */

    /* 检查链路建立结果 */
    if(pstRspMsg->ulResult!=CPSS_OK)
    {
        return CPSS_ERROR;
    }

    /* 得到物理地址对应的链路 */
    pstLink=cpss_com_link_find_real(pstRspMsg->stPhyAddr);
    if(NULL == pstLink)
    {
        cpss_output(CPSS_MODULE_COM, CPSS_PRINT_WARN,
               "\n Recv link rsp, but can not find linkobj.stPhyAddr:0x%08x ",
               *(UINT32*)&pstRspMsg->stPhyAddr);

        return (CPSS_ERROR);
    }

    /*设置链路是否需要由主用转发的地址*/
    if(pstRspMsg->ulMasterTransmitFlag == CPSS_COM_MASTER_TRANSMIT)
    {
        pstLink->stMPhyAddr = pstRspMsg->stMatePhyAddr ;
    }

    /* 删除链路请求定时器 */
    if((pstLink->ulLinkReqTd != 0) &&
        (pstLink->ulLinkReqTd != CPSS_TD_INVALID))
    {
        cpss_timer_para_delete(pstLink->ulLinkReqTd);
        pstLink->ulLinkReqTd=CPSS_TD_INVALID;
    }
    else
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_DETAIL,
                "\n Recv link rsp again from phyAddr = 0x%x. ",
                pstRspMsg->stPhyAddr);
        return (CPSS_ERROR);
    }

    cpss_output(CPSS_MODULE_COM, CPSS_PRINT_DETAIL,
        "link response received, phyAddr 0x%08x <- 0x%08x. Peer addrFlag = %#x, linkId = %d",
        *(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,
        *(UINT32*)&pstRspMsg->stPhyAddr, pstRspMsg->ulAddrFlag, pstLink->ulLinkId);


    /* 重新建立到对方的链路 */
    if(CPSS_OK != cpss_com_route_create(pstRspMsg->stLogicAddr,
        pstRspMsg->ulAddrFlag,
        pstRspMsg->stPhyAddr,CPSS_COM_DRV_UDP))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
            "link response message create route failed.");
        return CPSS_ERROR;
    }

    cpss_output(CPSS_MODULE_COM, CPSS_PRINT_DETAIL,
        "link response received, peer logAddr = %#x. link->stLogAddr = %#x, linkid = %d",
        *(UINT32*)&pstRspMsg->stLogicAddr, *(UINT32*)&pstLink->stDstLogAddr, pstLink->ulLinkId);

    cpss_com_link_status_set(pstRspMsg->stPhyAddr,CPSS_COM_LINK_STATUS_NORMAL) ;

#if 0
    cpss_com_timer_heartbeat_start(pstLink);
    cpss_com_timer_link_fault_start(pstLink);
#endif
    /* 收到正确的响应,序号递增 */
    g_pstComData->ulSeqID = g_pstComData->ulSeqID + 1;

    /* 如果对方存在伙伴板,并且没有建立链路,则发送建链请求 */
    if( 0 == (CPSS_COM_PHY_ADDR_ZERO(pstRspMsg->stMatePhyAddr)))
    {
        CPSS_COM_ROUTE_T *pstRoute=cpss_com_route_find_real(pstRspMsg->stLogicAddr);
        UINT32 ulMateIdx=(CPSS_COM_ADDRFLAG_MASTER==pstRspMsg->ulAddrFlag)?1:0;
        if (NULL == pstRoute)
        {
            return (CPSS_ERR_COM_ROUTE_NOT_EXIST);
        }

#ifdef CPSS_SWITCH_BRDTYPE
        if(g_ulCpssMasaLinkReqFlag == FALSE)
#else
        if(0 != CPSS_COM_PHY_ADDR_ZERO(pstRoute->stPhyAddr[ulMateIdx]))
#endif
        {
            cpss_com_link_req_send(pstRspMsg->stMatePhyAddr,CPSS_COM_CTRL_CHANNEL);
            g_ulCpssMasaLinkReqFlag = TRUE ;
        }
    }

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_heatbeat_node_status_set
* 功    能:
* 相关文档:
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
VOID cpss_com_heartbeat_node_status_set
(
 CPSS_COM_PHY_ADDR_T stPhyAddr,
 UINT16 usStatus
)
{
    CPSS_COM_HEARTBEAT_NODE_T *pstHeartBeatNode ;

    pstHeartBeatNode = g_pstCpssHeartBeatLink ;

    while(pstHeartBeatNode!=NULL)
    {
        if(1==CPSS_COM_PHY_ADDR_SAME(pstHeartBeatNode->stDstPhyAddr,stPhyAddr))
        {
            pstHeartBeatNode->usLinkState = usStatus ;
            break ;
        }
        pstHeartBeatNode = pstHeartBeatNode->pstNextNode ;
    }
}


/*******************************************************************************
* 函数名称: cpss_com_heartbeat_link_add
* 功    能:
* 相关文档:
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_heartbeat_link_add
(
 CPSS_COM_PHY_ADDR_T stDstPhyAddr
)
{
    UINT32 ulExistFlag = 0 ;
    CPSS_COM_HEARTBEAT_NODE_T *pstHeartBeatNode = g_pstCpssHeartBeatLink ;

    /*由于PCI链路没有心跳,所以不需要增加PCI链路节点*/
#ifndef CPSS_HOST_CPU
    return CPSS_OK ;
#else
    if(stDstPhyAddr.ucCpu != 1)
    {
        return CPSS_OK ;
    }
#endif

    /*查找对应的心跳节点是否存在*/
    while(pstHeartBeatNode != NULL)
    {
        if(1==CPSS_COM_PHY_ADDR_SAME(pstHeartBeatNode->stDstPhyAddr,stDstPhyAddr))
        {
            ulExistFlag = 1 ;
            break ;
        }

        pstHeartBeatNode = pstHeartBeatNode->pstNextNode ;
    }

    pstHeartBeatNode = NULL ;

    /*如果节点不存在,则增加节点*/
    if(ulExistFlag == 0)
    {
        pstHeartBeatNode = cpss_mem_malloc(sizeof(CPSS_COM_HEARTBEAT_NODE_T)) ;
        if(NULL == pstHeartBeatNode)
        {
            return CPSS_ERROR ;
        }

        cpss_mem_memset(pstHeartBeatNode,0,sizeof(CPSS_COM_HEARTBEAT_NODE_T)) ;

        pstHeartBeatNode->stDstPhyAddr = stDstPhyAddr ;
        pstHeartBeatNode->pstNextNode = g_pstCpssHeartBeatLink ;
        g_pstCpssHeartBeatLink = pstHeartBeatNode ;
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_heartbeat_link_send
* 功    能:
* 相关文档:
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_masa_link_fault_deal
(
 CPSS_COM_PHY_ADDR_T stMasaPhyAddr
)
{
    /*如果是全局板则设置MASA的链路为主用*/
#ifdef CPSS_FUNBRD_MC

    CPSS_COM_LINK_T* pstLink ;

    pstLink = cpss_com_link_find_real(stMasaPhyAddr) ;
    if(NULL == pstLink)
    {
        return CPSS_ERROR ;
    }

    /*是到MASA的链路,设置链路为主用*/
    if(CPSS_COM_PHY_ADDR_ZERO(pstLink->stMPhyAddr) == 0)
    {
        pstLink->ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER ;
    }

#endif
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_dsp_cpu_usage_send
* 功    能:
* 相关文档:
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
extern UINT32 g_ulCpssCpuBusyRate ;
UINT32 g_ulDspCpuCount = 0;
void cpss_com_dsp_cpu_usage_send()
{
#ifdef CPSS_DSP_CPU
     CPSS_COM_DSP_CPU_USAGE_T stMsg ;
     CPSS_COM_PHY_ADDR_T stPhy;
     stPhy = g_pstComData->stAddrThis.stPhyAddr;
     stPhy.ucCpu = 1;
     stMsg.ulDspNum = cpss_htonl(g_pstComData->stAddrThis.stPhyAddr.ucCpu - 5);
     stMsg.ulDspCpuUsage = cpss_htonl(g_ulCpssCpuBusyRate);

     g_ulDspCpuCount = g_ulDspCpuCount + 1 ;

     if(CPSS_MOD(g_ulDspCpuCount,5) == 0)
     {
          if(CPSS_OK !=cpss_com_send_phy(stPhy,
              CPSS_COM_LINK_MNGR_PROC,
              CPSS_COM_DSP_CPU_USAGE_MSG,
              (UINT8 *)&stMsg,
              sizeof(CPSS_COM_DSP_CPU_USAGE_T)))
          {
              cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                  "dsp cpu usage send failed, phyAddr 0x%08x -> 0x%08x!\n",
                  *(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,*(UINT32*)&stPhy);
          }
      }
      return ;
#endif
}
/*******************************************************************************
* 函数名称: cpss_com_heartbeat_link_send
* 功    能:
* 相关文档:
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_heartbeat_link_send
(
 CPSS_COM_LINK_HEART_MSG_T* pstMsg
)
{
    CPSS_COM_LINK_T* pstLink ;
    CPSS_COM_HEARTBEAT_NODE_T *pstHeartBeatNode = g_pstCpssHeartBeatLink ;

    /* 入参检查 */
    if(pstMsg == NULL)
    {
        return CPSS_ERR_COM_PARA_ILLEGAL ;
    }

    cpss_com_dsp_cpu_usage_send();

    /* 循环遍历心跳链表 */
    while(pstHeartBeatNode != NULL)
    {
        if(pstHeartBeatNode->usLinkState != CPSS_COM_LINK_STATUS_NORMAL)
        {
            pstHeartBeatNode = pstHeartBeatNode->pstNextNode ;
            continue ;
        }

        /*得到链路对象*/
        pstLink = cpss_com_link_find_real(pstHeartBeatNode->stDstPhyAddr) ;
        if(pstLink == NULL)
        {
            pstHeartBeatNode = pstHeartBeatNode->pstNextNode ;
            continue ;
        }

        /*统计心跳*/
        pstLink->ulHeartBeatNum = pstLink->ulHeartBeatNum + 1 ;
        pstLink->stLinkStat.ulHeartBeatSendNum = pstLink->stLinkStat.ulHeartBeatSendNum + 1;

        /*统计每条链路流量*/
        pstLink->stVolumeStat.ulSendBytesPerSecond = pstLink->stVolumeStat.ulSendBytesCur ;
        pstLink->stVolumeStat.ulSendBytesCur = 0 ;
        if(pstLink->stVolumeStat.ulSendBytesPerSecond > pstLink->stVolumeStat.ulSendBytesPerSecondPeek)
        {
             pstLink->stVolumeStat.ulSendBytesPerSecondPeek = pstLink->stVolumeStat.ulSendBytesPerSecond ;
        }

        /*设置链路状态*/
        if(pstLink->ulHeartBeatNum >= CPSS_COM_LINK_HEARTBEAT_MAX)
        {
            cpss_com_link_status_set(pstHeartBeatNode->stDstPhyAddr,CPSS_COM_LINK_STATUS_FAULT) ;
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,"detect link fault DstPhyAddr 0x%x\n",
                *(UINT32*)&pstHeartBeatNode->stDstPhyAddr) ;

            pstLink->ulHeartBeatNum = 0 ;

            /*发送建链请求*/
            cpss_com_link_req_send(pstHeartBeatNode->stDstPhyAddr,pstLink->ulMateFlag) ;

            /*如果链路对端是MASA单板,则设置链路为主用*/
            cpss_com_masa_link_fault_deal(pstHeartBeatNode->stDstPhyAddr) ;

            pstHeartBeatNode = pstHeartBeatNode->pstNextNode ;
            continue ;
        }

        /* 发送心跳消息 */
        if(CPSS_OK !=cpss_com_send_phy(pstHeartBeatNode->stDstPhyAddr,
            CPSS_COM_LINK_MNGR_PROC,
            CPSS_COM_LINK_HEARTBEAT_MSG,
            (UINT8 *)pstMsg,
            sizeof(CPSS_COM_LINK_HEART_MSG_T)))
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "link heartbeat send failed, phyAddr 0x%08x -> 0x%08x!\n",
                *(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,*(UINT32*)&pstMsg->stPhyAddr);
        }
        pstHeartBeatNode = pstHeartBeatNode->pstNextNode ;
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_link_heartbeat_send
* 功    能: 向指定物理地址的链路发送心跳消息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* stPhyAddr  CPSS_COM_PHY_ADDR_T    输入         物理地址
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_link_heartbeat_send()
{
    CPSS_COM_LINK_HEART_MSG_T stMsg;

    /* 填写心跳消息内容 */
    stMsg.stPhyAddr=g_pstComData->stAddrThis.stPhyAddr;
    stMsg.stLogicAddr=g_pstComData->stAddrThis.stLogiAddr;
    stMsg.ulAddrFlag=g_pstComData->stAddrThis.ulAddrFlag;

    /* 字节序转换 */
    stMsg.stLogicAddr.usGroup=cpss_htons(stMsg.stLogicAddr.usGroup);
    stMsg.ulAddrFlag=cpss_htonl(stMsg.ulAddrFlag);

    /*接收到链路定时器消息次数统计*/
    g_stCpssHeartBeatStat.ulHeartBeatTimerNum = g_stCpssHeartBeatStat.ulHeartBeatTimerNum + 1 ;

    /*遍历链路列表，发送心跳消息*/
    cpss_com_heartbeat_link_send(&stMsg) ;

#if 0
    /* 发送心跳消息 */
    if(CPSS_OK !=cpss_com_send_phy(stPhyAddr,
        CPSS_COM_LINK_MNGR_PROC,
        CPSS_COM_LINK_HEARTBEAT_MSG,
        (UINT8 *)&stMsg,
        sizeof(CPSS_COM_LINK_HEART_MSG_T)))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "link heartbeat send failed, phyAddr 0x%08x -> 0x%08x!",
            *(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,
            *(UINT32*)&stMsg.stPhyAddr);
        return CPSS_ERROR;
    }
#endif

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_link_heartbeat_recv
* 功    能: 收到链路心跳消息，把链路故障定时器删除，重新设定
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstMsgHead CPSS_COM_MSG_HEAD_T*   输入           分发层消息头
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_link_heartbeat_recv(CPSS_COM_MSG_HEAD_T *pstMsgHead)
{
    CPSS_COM_LINK_HEART_MSG_T *pstMsg = (CPSS_COM_LINK_HEART_MSG_T*)pstMsgHead->pucBuf;
    CPSS_COM_LINK_T *pstLink          = cpss_com_link_find_real(pstMsg->stPhyAddr);

    /* 字节序转换 */
    pstMsg->stLogicAddr.usGroup = cpss_ntohs(pstMsg->stLogicAddr.usGroup);
    pstMsg->ulAddrFlag          = cpss_ntohl(pstMsg->ulAddrFlag);

    if((NULL == pstLink) || (pstLink->usStatus != CPSS_COM_LINK_STATUS_NORMAL))
    {
        return CPSS_ERROR;
    }

    /*增加链路心跳收统计*/
    pstLink->stLinkStat.ulHeartBeatRecvNum = pstLink->stLinkStat.ulHeartBeatRecvNum + 1;

#if 0
    /* 重新启动链路故障定时器 */
    cpss_com_timer_link_fault_start(pstLink);
#endif

    /* 清零累计次数 */
    pstLink->ulHeartBeatNum = 0 ;

    /* 对端的备状态发生了变化,备升主 */
    if(pstLink->ulAddrFlag != pstMsg->ulAddrFlag)
    {
        /*切换单板的路由*/

        cpss_com_board_switch_deal(pstMsg->stLogicAddr,
            pstMsg->stPhyAddr,pstMsg->ulAddrFlag) ;

        /*重新设置链路的地址标志*/
        pstLink->ulAddrFlag=pstMsg->ulAddrFlag;

        cpss_com_link_gcpa_switch(pstMsg->stPhyAddr, (UINT8)pstMsg->ulAddrFlag);

        /*如果对端是交换板，则删除原有路由，增加新路由*/
#ifdef CPSS_VOS_VXWORKS
#ifdef CPSS_FUNBRD_MC
        if(pstLink->ulAddrFlag == CPSS_COM_ADDRFLAG_MASTER)
        {
            if(CPSS_COM_PHY_ADDR_ZERO(pstLink->stMPhyAddr) != 1)
            {
                /*删除原有路由*/
                cpss_com_swtich_route_deal(pstLink->stMPhyAddr,
                    pstLink->stDstPhyAddr,CPSS_COM_ROUTE_DEL) ;

                /*增加新路由*/
                cpss_com_swtich_route_deal(pstLink->stDstPhyAddr,
                    pstLink->stMPhyAddr,CPSS_COM_ROUTE_ADD) ;
            }
        }
#endif
#endif
    }  /* end  if(pstLink->ulAddrFlag != pstMsg->ulAddrFlag) */
    else
    {
	/* 测试线发现交换板链路的状态和路由表中的状态不一致,此处增加异常处理 */
         CPSS_COM_ROUTE_T *pstRoute ;

         pstRoute = cpss_com_route_find_real(pstMsg->stLogicAddr);

	  if(pstRoute ==NULL) return CPSS_ERROR;
    	    /* 如果自己记录的地址和心跳带过来的主控板地址不一致,说明发生了错误,启动修复*/
	  if((0 ==CPSS_COM_PHY_ADDR_SAME(pstRoute->stPhyAddr[pstLink->ulAddrFlag],pstMsg->stPhyAddr))&&\
             ((1 ==CPSS_COM_PHY_ADDR_SAME(pstMsg->stPhyAddr,g_stPhyAddrGcpaMaster))||\
             (1 ==CPSS_COM_PHY_ADDR_SAME(pstMsg->stPhyAddr,g_stPhyAddrGcpaSlave))))
	      {
	        #ifdef CPSS_SWITCH_BRDTYPE
	        /* 切换路由表*/
                cpss_com_route_switch(pstRoute,pstMsg->stLogicAddr) ;
		 /* 切换记录的gcpa 主备关系*/
		   cpss_com_link_gcpa_switch(pstMsg->stPhyAddr, (UINT8)pstMsg->ulAddrFlag);

		 /* 纤程激活完毕*/
		  if(g_bCpssRecvProcEndInd == TRUE)
		  {
		   UINT32 ulArgs[4];
		   cpss_sbbr_write_text(0, ulArgs, " routeTable occur error link ulAddrFlag = %d phyAddr = %d  \n\r",pstLink->ulAddrFlag,*(UINT32 *)&pstMsg->stPhyAddr);
		  }

		 #endif
	      }
    }
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_link_fault_recv
* 功    能: 收到链路故障消息，设置链路状态为故障态COM_LINK_STATUS_FAULT
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstMsgHead CPSS_COM_MSG_HEAD_T*   输入           分发层消息头
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
******************************************************************************/
#if 0
INT32 cpss_com_link_fault_recv(CPSS_COM_MSG_HEAD_T *pstMsgHead)
{
    CPSS_COM_PHY_ADDR_T *pstPhyAddr=(CPSS_COM_PHY_ADDR_T*)pstMsgHead->pucBuf;
    CPSS_COM_LINK_T *pstLink=cpss_com_link_find_real(*pstPhyAddr);

    if(NULL == pstLink)
    {
        return CPSS_ERROR;
    }

    /*链路故障统计*/
    pstLink->stLinkStat.ulFaultNum = pstLink->stLinkStat.ulFaultNum + 1;

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "link fault timer %d received.phyAddr=%x",pstLink->ulFaultTd,*(UINT32*)&pstLink->stDstPhyAddr);

    /* 链路故障定时器已经被自动删除 */
    pstLink->ulFaultTd=0;

    /* 删除链路心跳定时器 */
    if((pstLink->ulHeartTd != 0) &&
        (pstLink->ulHeartTd != CPSS_TD_INVALID))
    {
        cpss_timer_para_delete(pstLink->ulHeartTd);
        pstLink->ulHeartTd=0;
    }

    /* 如果对端是主控板，发送反向地址解析请求 */
    if( 0 != CPSS_COM_PHY_ADDR_SAME(g_stPhyAddrGcpaMaster,pstLink->stDstPhyAddr))
    {
        pstLink->usStatus=CPSS_COM_LINK_STATUS_FAULT;
        cpss_com_link_req_send(g_stPhyAddrGcpaMaster);
    }  /* if (CPSS_COM_PHY_ADDR_SAME(g_stPhyAddrGcpaSlave,pstLink->stDstPhyAddr)) */
    else if( 0 != CPSS_COM_PHY_ADDR_SAME(g_stPhyAddrGcpaSlave,pstLink->stDstPhyAddr))
    {
        pstLink->usStatus=CPSS_COM_LINK_STATUS_FAULT;
        cpss_com_link_req_send(g_stPhyAddrGcpaSlave);
    }
    else /* 设置链路状态为故障态 */
    {
        pstLink->usStatus=CPSS_COM_LINK_STATUS_FAULT;
        cpss_com_link_req_send(pstLink->stDstPhyAddr);
    }  /* end if  ( 0 != CPSS_COM_PHY_ADDR_SAME(...*/
    return CPSS_OK;
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_mate_channel_req_send
* 功    能: 发送主备通道建立请求消息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型                输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
******************************************************************************/
INT32 cpss_com_mate_channel_req_send(VOID)
{
#ifndef CPSS_DSP_CPU
    INT32 lRet ;
    CPSS_COM_PHY_ADDR_T stMatePhyAddr;

    /* 如果伙伴板的物理地址无效，则返回失败 */
    if( 0 != CPSS_COM_PHY_ADDR_ZERO(g_pstComData->stMatePhyAddr))
    {
        return CPSS_ERROR;
    }

	stMatePhyAddr = g_pstComData->stMatePhyAddr;

    /*发送主备通道建链请求消息*/
    lRet = cpss_com_link_req_send(stMatePhyAddr,CPSS_COM_MATE_CHANNEL) ;
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }
#endif
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_gcpa_route_deal
* 功    能: 增加去GCPA的路由
* 函数类型:
* 参    数:
* 参数名称            类型                输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
#ifndef CPSS_DSP_CPU
#ifdef CPSS_VOS_VXWORKS
INT32 cpss_com_gcpa_route_deal(UINT32 ulDealFlag)
{
    UINT8  aucGateWayIp[20] ;
    UINT32 ulMateIp ;
    CPSS_COM_PHY_ADDR_T stPhyAddrMate = {0} ;
    struct in_addr inetAddress = {0} ;

    cpss_com_phy_addr_mate_get(&stPhyAddrMate) ;

    cpss_com_phy2ip(stPhyAddrMate,&ulMateIp,TRUE) ;

    inetAddress.s_addr = ulMateIp ;

    inet_ntoa_b(inetAddress,aucGateWayIp) ;

    if(ulDealFlag == CPSS_COM_ROUTE_ADD)
    {
        routeAdd(CPSS_GCPA_M_IP_STR,aucGateWayIp) ;
        routeAdd(CPSS_GCPA_S_IP_STR,aucGateWayIp) ;

        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "switch brd add route gateway=%s\n",aucGateWayIp) ;
    }
    else if(ulDealFlag == CPSS_COM_ROUTE_DEL)
    {
        routeDelete(CPSS_GCPA_M_IP_STR,aucGateWayIp) ;
        routeDelete(CPSS_GCPA_S_IP_STR,aucGateWayIp) ;

        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "switch brd del route gateway=%s\n",aucGateWayIp) ;
    }
    else
    {
        return CPSS_ERROR ;
    }

    return CPSS_OK ;
}
#endif
#endif
/*******************************************************************************
* 函数名称: cpss_com_swtich_brd_recv_mate_req_deal
* 功    能: 处理交换板收到主备建链请求的处理
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型                输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_swtich_brd_recv_mate_req_deal
(
 CPSS_COM_MSG_HEAD_T* pstMsgHdr
)
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_LINK_REQ_MSG_T *pstLinkReq ;

    pstLinkReq = (CPSS_COM_LINK_REQ_MSG_T *)pstMsgHdr->pucBuf ;

    g_pstComData->stMatePhyAddr = pstLinkReq->stPhyAddr ;
    g_pstComData->stAddrThis.stLogiAddr = pstLinkReq->stLogicAddr ;

    if(pstLinkReq->ulAddrFlag == CPSS_COM_ADDRFLAG_MASTER)
    {
        g_pstComData->stAddrThis.ulAddrFlag = CPSS_COM_ADDRFLAG_SLAVE;
        g_ulCpssMateChannelFlag = CPSS_COM_MATE_CHANNEL_ENABLE ;

        /*如果本板是备板,则增加去GCPA的路由*/
#ifdef CPSS_VOS_VXWORKS
      /*  cpss_com_gcpa_route_deal(CPSS_COM_ROUTE_ADD) ; */
#endif
    }
    else
    {
        g_pstComData->stAddrThis.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER;
    }

    g_pstComData->stAddrThis.ucBackupType = CPS__RDBS_DEV_11_BACKUP;

    /*调整GCPA主备*/
    cpss_com_link_gcpa_switch(
        pstLinkReq->stGcpaPhyAddrM,CPSS_COM_ADDRFLAG_MASTER) ;

#endif
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_mate_channel_req_recv
* 功    能: 接收到主备通道建立请求消息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型                输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
******************************************************************************/
INT32 cpss_com_mate_channel_req_recv
(
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_LINK_T *pstLink;
    CPSS_COM_LINK_RSP_MSG_T   stLinkRsp;
    CPSS_COM_LINK_REQ_MSG_T *pstLinkReq=(CPSS_COM_LINK_REQ_MSG_T*)pstMsgHead->pucBuf;

    /* 转换成主机字节序 */
    pstLinkReq->stLogicAddr.usGroup = cpss_ntohs(pstLinkReq->stLogicAddr.usGroup);
    pstLinkReq->ulAddrFlag          = cpss_ntohl(pstLinkReq->ulAddrFlag);
    pstLinkReq->ulMateFlag          = cpss_ntohl(pstLinkReq->ulMateFlag);

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "mate channel request recv, phyAddr 0x%08x <- 0x%08x",
        *(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,
        *(UINT32*)&pstLinkReq->stPhyAddr);

    /* 如果伙伴板的物理地址无效，则返回失败 */
    if(0 != CPSS_COM_PHY_ADDR_ZERO(pstLinkReq->stPhyAddr))
    {
        return CPSS_ERROR;
    }
#ifndef CPSS_SWITCH_BRDTYPE
    /* 如果本板伙伴板物理地址跟对方物理地址不一致,返回失败 */
    if(0 == CPSS_COM_PHY_ADDR_SAME(pstLinkReq->stPhyAddr,g_pstComData->stMatePhyAddr))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "mate channel request recv, phyAddr not identical,"
            " local matePhyAddr 0x%08x, remote phyAddr 0x%08x!",
            *(UINT32*)&g_pstComData->stMatePhyAddr,
            *(UINT32*)&pstLinkReq->stPhyAddr);
        return CPSS_ERROR;
    }
#endif

    /* 建立主备通道 */
    if(CPSS_OK != cpss_com_route_create(pstLinkReq->stLogicAddr,
        pstLinkReq->ulAddrFlag,
        pstLinkReq->stPhyAddr,CPSS_COM_DRV_UDP))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "mate channel request recv, create route failed!");
        return CPSS_ERROR;
    }
    pstLink=cpss_com_link_find_real(pstLinkReq->stPhyAddr);
    cpss_com_link_status_set(pstLinkReq->stPhyAddr,CPSS_COM_LINK_STATUS_NORMAL);
    pstLink->ulMateFlag = CPSS_COM_MATE_CHANNEL ;
    /* 启动心跳定时器和链路故障定时器*/
#if 0
    cpss_com_timer_heartbeat_start(pstLink);
    cpss_com_timer_link_fault_start(pstLink);
#endif

    /* 发送主备通道建立响应消息 */
    cpss_mem_memset(&stLinkRsp,0,sizeof(stLinkRsp));
    stLinkRsp.stLogicAddr   = g_pstComData->stAddrThis.stLogiAddr;
    stLinkRsp.ulAddrFlag    = g_pstComData->stAddrThis.ulAddrFlag;
    stLinkRsp.stPhyAddr     = g_pstComData->stAddrThis.stPhyAddr;
    stLinkRsp.stMatePhyAddr = g_pstComData->stMatePhyAddr;

    /* 转换成网络字节序 */
    stLinkRsp.stLogicAddr.usGroup = cpss_htons(stLinkRsp.stLogicAddr.usGroup);
    stLinkRsp.ulAddrFlag          = cpss_htonl(stLinkRsp.ulAddrFlag);

    /* 发送主备通道建立响应消息到伙伴板,携带本板物理地址 */
    if(CPSS_OK != cpss_com_send_phy(pstLinkReq->stPhyAddr,
        CPSS_COM_LINK_MNGR_PROC,
        CPSS_COM_MATE_CHANNEL_RSP_MSG,
        (UINT8 *)&stLinkRsp,
        sizeof(stLinkRsp)))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "mate channel response send failed, phyAddr 0x%08x -> 0x%08x!",
            *(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,
            *(UINT32*)&pstLinkReq->stPhyAddr);
        return CPSS_ERROR;
    }

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "mate channel response send, phyAddr 0x%08x -> 0x%08x.",
        *(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,
        *(UINT32*)&pstLinkReq->stPhyAddr);
#endif
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_mate_channel_rsp_recv
* 功    能: 接收到主备通道建立响应消息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型                输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*
******************************************************************************/
INT32 cpss_com_mate_channel_rsp_recv
(
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_LINK_T *pstLink;
    CPSS_COM_LINK_RSP_MSG_T *pstLinkRsp=(CPSS_COM_LINK_RSP_MSG_T*)pstMsgHead->pucBuf;

    /* 转换成主机字节序 */
    pstLinkRsp->stLogicAddr.usGroup = cpss_ntohs(pstLinkRsp->stLogicAddr.usGroup);
    pstLinkRsp->ulAddrFlag          = cpss_ntohl(pstLinkRsp->ulAddrFlag);

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "mate channel response recv, phyAddr 0x%08x <- 0x%08x",
        *(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,
        *(UINT32*)&pstLinkRsp->stPhyAddr);

    /* 如果伙伴板的物理地址无效，则返回失败 */
    if( 0 != CPSS_COM_PHY_ADDR_ZERO(pstLinkRsp->stPhyAddr))
    {
        return CPSS_ERROR;
    }

    /* 如果本板伙伴板物理地址跟对方物理地址不一致,返回失败 */
   if( 0 == CPSS_COM_PHY_ADDR_SAME(pstLinkRsp->stPhyAddr,g_pstComData->stMatePhyAddr))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "mate channel response recv, phyAddr not identical,"
            " local matePhyAddr 0x%08x, remote phyAddr 0x%08x!",
            *(UINT32*)&g_pstComData->stMatePhyAddr,
            *(UINT32*)&pstLinkRsp->stPhyAddr);
        return CPSS_ERROR;
    }
    /* 建立主备通道 */
    if(CPSS_OK != cpss_com_route_create(pstLinkRsp->stLogicAddr,
        pstLinkRsp->ulAddrFlag,
        pstLinkRsp->stPhyAddr,CPSS_COM_DRV_UDP))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "mate channel response recv, create route failed!");
        return CPSS_ERROR;
    }
    pstLink=cpss_com_link_find_real(pstLinkRsp->stPhyAddr);
    cpss_com_link_status_set(pstLinkRsp->stPhyAddr,CPSS_COM_LINK_STATUS_NORMAL);

    /* 启动心跳定时器和链路故障定时器*/
#if 0
    cpss_com_timer_heartbeat_start(pstLink);
    cpss_com_timer_link_fault_start(pstLink);
#endif
    /* 删除主备通道建立请求定时器 */
    cpss_timer_delete(CPSS_COM_MATE_REQ_TM_NO);

    /*删除连路建立请求定时器*/
    cpss_timer_para_delete(pstLink->ulLinkReqTd);
    pstLink->ulLinkReqTd=CPSS_TD_INVALID;
#endif
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_link_gcpa_switch
* 功    能: 切换GCPA主备板
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明:
******************************************************************************/
VOID cpss_com_link_gcpa_switch
(
    CPSS_COM_PHY_ADDR_T stThisPhyAddr,
    UINT8               ucStatus
)
{
#ifndef CPSS_DSP_CPU
    BOOL bMPhySameFlag = FALSE ;
    BOOL bSPhySameFlag = FALSE ;
    UINT32 ulGcpaIpAddr ;

    bMPhySameFlag = CPSS_COM_PHY_ADDR_SAME(g_pstComData->stAddrGcpaM.stPhyAddr, stThisPhyAddr) ;
    bSPhySameFlag = CPSS_COM_PHY_ADDR_SAME(g_pstComData->stAddrGcpaS.stPhyAddr, stThisPhyAddr) ;


    if(TRUE == bMPhySameFlag)
    {
        if(SMSS_STANDBY == ucStatus)
        {
            g_pstComData->stAddrGcpaM.stPhyAddr = g_pstComData->stAddrGcpaS.stPhyAddr ;
            g_pstComData->stAddrGcpaS.stPhyAddr = stThisPhyAddr ;

            /*切换IP地址*/
            ulGcpaIpAddr = g_pstComData->stAddrGcpaM.ulIpAddr ;
            g_pstComData->stAddrGcpaM.ulIpAddr = g_pstComData->stAddrGcpaS.ulIpAddr ;
            g_pstComData->stAddrGcpaS.ulIpAddr = ulGcpaIpAddr ;
        }
    }

    if(TRUE == bSPhySameFlag)
    {
        if(SMSS_ACTIVE == ucStatus)
        {
            g_pstComData->stAddrGcpaS.stPhyAddr = g_pstComData->stAddrGcpaM.stPhyAddr;
            g_pstComData->stAddrGcpaM.stPhyAddr = stThisPhyAddr  ;

            /*切换IP地址*/
            ulGcpaIpAddr = g_pstComData->stAddrGcpaM.ulIpAddr ;
            g_pstComData->stAddrGcpaM.ulIpAddr = g_pstComData->stAddrGcpaS.ulIpAddr ;
            g_pstComData->stAddrGcpaS.ulIpAddr = ulGcpaIpAddr ;
        }
    }

    {
        cpss_mem_memcpy(&g_stPhyAddrGcpaMaster, &g_pstComData->stAddrGcpaM.stPhyAddr,
                sizeof(CPSS_COM_PHY_ADDR_T));
        cpss_mem_memcpy(&g_stPhyAddrGcpaSlave, &g_pstComData->stAddrGcpaS.stPhyAddr,
                sizeof(CPSS_COM_PHY_ADDR_T));
    }
#endif
}

/*******************************************************************************
* 函数名称: cpss_com_link_selfroute_switch
* 功    能: 切换本板的自己到自己的路由
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:
* 说   明:
*
******************************************************************************/
INT32 cpss_com_link_selfroute_switch()
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_ROUTE_T *pstRoute;
    CPSS_COM_LOGIC_ADDR_T stLogAddr ;

    stLogAddr = g_pstComData->stAddrThis.stLogiAddr ;
    pstRoute = cpss_com_route_find_real(stLogAddr);
    if(NULL==pstRoute)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
            "link self switch find route failed(logAddr=%x).",
            g_pstComData->stAddrThis.stLogiAddr);
        return CPSS_ERR_COM_ROUTE_NOT_EXIST ;
    }

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "route self switch (logAddr=%x).",*(UINT32*)&stLogAddr) ;

    cpss_com_route_switch(pstRoute,stLogAddr) ;
#endif
    return CPSS_OK ;
}



/*******************************************************************************
* 函数名称: cpss_com_active_proc_req_recv
* 功    能: 收到SMSS纤程激活请求消息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pstMsgHead  CPSS_COM_MSG_HEAD_T*  输入         分发层消息头
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:纤程激活请求消息中，应当携带本板的主备状态
******************************************************************************/
INT32 cpss_com_active_proc_req_recv
(
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
    /* 保存本板的主备状态 */
    SMSS_PROC_ACTIVATE_REQ_MSG_T *pstReqMsg = (SMSS_PROC_ACTIVATE_REQ_MSG_T *)pstMsgHead->pucBuf;

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "active proc request message received.");

    g_pstComData->stAddrGcpaM.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER;
    g_pstComData->stAddrGcpaS.ulAddrFlag = CPSS_COM_ADDRFLAG_SLAVE;

    g_pstComData->stAddrThis.ulAddrFlag=CPSS_COM_ADDRFLAG_MASTER;
    if(SMSS_STANDBY == pstReqMsg->ucAsStatus)
    {
        g_pstComData->stAddrThis.ulAddrFlag=CPSS_COM_ADDRFLAG_SLAVE;
    }

    /*切换本板GCPA的主备物理地址*/
    cpss_com_link_gcpa_switch( g_pstComData->stAddrThis.stPhyAddr,
                              pstReqMsg->ucAsStatus);

    /*通信初始化流程统计*/
#ifndef CPSS_DSP_CPU
    g_stCpssComInitStat.stSmssActInfo.ulSmssActFlag = 1 ;
    g_stCpssComInitStat.stSmssActInfo.ulAddrFlag = pstReqMsg->ucAsStatus ;
    cpss_clock_get(&g_stCpssComInitStat.stSmssActInfo.stTime) ;
#endif

    /*处理激活请求时设置交换板的全局变量*/
#ifdef CPSS_SWITCH_BRDTYPE
    if(g_pstComData->stAddrThis.ulAddrFlag == CPSS_COM_ADDRFLAG_SLAVE)
    {
        g_ulCpssMateChannelFlag = CPSS_COM_MATE_CHANNEL_ENABLE ;
    }
#endif

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_active_proc_rsp_send
* 功    能: 向SMSS发送纤程激活响应消息
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* ulResult        UINT32            输入            纤程激活结果
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:纤程激活结果: 成功CPSS_OK,失败CPSS_ERROR
*
******************************************************************************/
INT32 cpss_com_active_proc_rsp_send
(
 UINT32 ulResult
 )
{
    INT32 lResult;
    /* CPSS_COM_PID_T stDstPid; */
    SMSS_PROC_ACTIVATE_RSP_MSG_T stRspMsg;
    stRspMsg.ulResult=SMSS_OK;

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "active proc response message send.");

    if(CPSS_OK != ulResult)
    {
        stRspMsg.ulResult=SMSS_ERROR;
    }

    /* 发送消息纤程激活响应消息 */

    lResult=cpss_com_send_phy(g_pstComData->stAddrThis.stPhyAddr,
        SMSS_SYSCTL_PROC,
        SMSS_PROC_ACTIVATE_RSP_MSG,
        (UINT8*)&stRspMsg,
        sizeof(stRspMsg));

   if(CPSS_OK == lResult )
    {
         cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "Send to Smss for Active Rsp.");

        cpss_vk_proc_user_state_set(CPSS_COM_LINK_PROC_STATE_NORMAL);
    }

   /*通信初始化流程统计*/
#ifndef CPSS_DSP_CPU
   g_stCpssComInitStat.stSmssActRspInfo.ulSmssActRspFlag = 1 ;
   g_stCpssComInitStat.stSmssActRspInfo.ulSmssActRspResult = ulResult ;
   cpss_clock_get(&g_stCpssComInitStat.stSmssActRspInfo.stTime) ;
#endif

    return lResult;
}


/*******************************************************************************
* 函数名称: cpss_com_init
* 功    能: 通信模块初始化
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
INT32  cpss_com_init(VOID)
{
    BOOL bMcFlag;

    bMcFlag = FALSE;

    CPSS_DSP_FILE_REC();
    CPSS_DSP_LINE_REC();
    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulComInitInCount);

    g_pstComData=cpss_mem_malloc(sizeof(CPSS_COM_DATA_T));
    if(NULL == g_pstComData)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "global data malloc failed!");
        return CPSS_ERROR;
    }
    cpss_mem_memset(g_pstComData,0,sizeof(CPSS_COM_DATA_T));

    /* 地址初始化 */
    if(CPSS_OK != cpss_com_addr_init())
    {
        CPSS_DSP_LINE_REC();
        CPSS_DSP_ERROR_WAIT;
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
            "address initial failed!!");
        return CPSS_ERROR;
    }

    /* 路由表初始化 */
    if(CPSS_OK != cpss_com_route_table_init())
    {
        CPSS_DSP_LINE_REC();
        CPSS_DSP_ERROR_WAIT;
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
            "route table initial failed!!");
        return CPSS_ERROR;
    }

    /* 链路表初始化 */
    if(CPSS_OK != cpss_com_link_table_init())
    {
        CPSS_DSP_LINE_REC();
        CPSS_DSP_ERROR_WAIT;
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
            "link table initial failg_pstComDataed!!");
        return CPSS_ERROR;
    }

    /* 驱动初始化 */
    if(CPSS_OK != cpss_com_drv_init())
    {
        CPSS_DSP_LINE_REC();
        CPSS_DSP_ERROR_WAIT;
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
            "drv initial failed!!");
        return CPSS_ERROR;
    }

    /*测试功能(特定消息统计功能,变量清零)*/
    cpss_com_spec_msg_stat_clear() ;


#ifdef CPSS_HOST_CPU
#ifndef SWP_FNBLK_BRDTYPE_ABOX
    /*增加到全局板的链路*/
    bMcFlag = cpss_local_funcbrd_type_is_mc();
    if(FALSE == bMcFlag)
    {
        /*通信初始化流程统计*/
        g_stCpssComInitStat.ulGcpaLinkSetup = 1 ;

        if(cpss_com_link_create(g_pstComData->stAddrGcpaM.stPhyAddr,CPSS_COM_DRV_UDP)!=CPSS_OK)
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
                "add link to GCPA master failed!!");
            return CPSS_ERROR;
        }
        if(cpss_com_link_create(g_pstComData->stAddrGcpaS.stPhyAddr,CPSS_COM_DRV_UDP)!=CPSS_OK)
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
                "add link to GCPA slave failed!!");
            return CPSS_ERROR;
        }
    }
#endif
#endif
#ifdef SWP_FNBLK_BRDTYPE_ABOX
    /*增加去ONCA的路由*/
    cpss_route_add("0","10.0.0.10") ;
    cpss_route_add("0","10.0.0.11") ;
#endif

    /*创建链路心跳定时器*/
    if(CPSS_OK != cpss_timer_loop_set(CPSS_COM_LINK_HEART_TM_NO,CPSS_COM_LINK_HEART_TM_INTERVAL))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
            "create link heartbeart timer failed!!\n");
        g_stCpssHeartBeatStat.ulHeartBeatTimerFlag = 1 ;
        CPSS_DSP_LINE_REC();
        CPSS_DSP_ERROR_WAIT;
        return CPSS_ERROR ;
    }
    g_stCpssHeartBeatStat.ulHeartBeatTimerFlag = 2 ;
    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulComInitOutCount);

    return CPSS_OK;

}

#ifdef CPSS_PCI_INCLUDE
/*******************************************************************************
* 函数名称: cpss_com_pci_link_setup_stat
* 功    能:
* 函数类型:
* 参    数:
* 参数名称   类型     输入/输出         描述
* 函数返回:
* 说    明:
*******************************************************************************/
INT32 cpss_com_pci_link_setup_stat(CPSS_COM_PHY_ADDR_T stPhyAddr)
{
    UINT32 ulLinkId = 0;
    cpss_com_drv_pci_linkid_get(stPhyAddr.ucCpu,&ulLinkId) ;
    if(ulLinkId > CPSS_DRV_PCI_INFO_ARRAY_MAX )
    {
        return CPSS_ERROR;
    }
    g_stCpssComPciTbl.astPciLink[ulLinkId-1].stPciStat.ulLinkSetupNum =
    g_stCpssComPciTbl.astPciLink[ulLinkId-1].stPciStat.ulLinkSetupNum + 1;
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_pci_route_create
* 功    能:
* 函数类型:
* 参    数:
* 参数名称   类型     输入/输出         描述
* 函数返回:
* 说    明:
*******************************************************************************/
INT32 cpss_com_pci_route_create
(
    CPSS_COM_PHY_ADDR_T   stPhyAddr,
    CPSS_COM_LOGIC_ADDR_T stLogAddr
)
{
    CPSS_COM_ROUTE_T *pstRoute ;
    INT32 lRet;

    /*找到路由*/
    pstRoute=cpss_com_route_find_real(stLogAddr);
    if (NULL == pstRoute)
    {
        return (CPSS_ERR_COM_ROUTE_NOT_EXIST);
    }

    /*增加PCI的建链统计*/
    cpss_com_pci_link_setup_stat(stPhyAddr);

    /* 保存路由物理地址 */
    pstRoute->stPhyAddr[0]=stPhyAddr;

    /*如果此链路已经建立，返回*/
    if(NULL==cpss_com_link_find_real(stPhyAddr))
    {
        /*创建链路对象*/
        lRet = cpss_com_link_create(stPhyAddr,CPSS_COM_DRV_PCI) ;
        if(CPSS_OK != lRet)
        {
            return CPSS_ERROR ;
        }
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_pci_route_create_by_cpuno
* 功    能:
* 函数类型:
* 参    数:
* 参数名称   类型     输入/输出         描述
* 函数返回:
* 说    明:
*******************************************************************************/
INT32 cpss_com_pci_route_create_by_cpuno
(
CPSS_COM_PHY_ADDR_T   stPhyAddr,
CPSS_COM_LOGIC_ADDR_T stLogAddr,
UINT32                ulCpuNo
)
{
    INT32 lRet;
    CPSS_COM_PHY_ADDR_T   stPciPhyAddr ;
    CPSS_COM_LOGIC_ADDR_T stPciLogAddr ;

    /*填充注册CPU链路的逻辑、物理地址并填充*/
    cpss_com_drv_pci_addr_fill(stPhyAddr,stLogAddr,ulCpuNo);

    /*得到对应CPU的物理和逻辑地址*/
    cpss_com_drv_pci_get_addr_by_cpuno(ulCpuNo,&stPciLogAddr,&stPciPhyAddr);

    /*创建路由*/

    lRet = cpss_com_pci_route_create(stPciPhyAddr,stPciLogAddr);

    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_pci_reg_res_send
* 功    能:
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_com_pci_reg_res_send(UINT32 ulCpuNo)
{
    INT32  lRet ;
    UINT32 ulAddrFlag ;
    UINT32 ulLinkId ;

    CPSS_COM_PHY_ADDR_T   stMPhyAddr ;
    CPSS_COM_LOGIC_ADDR_T stMLogAddr ;
    CPSS_COM_PHY_ADDR_T   stSPhyAddr ;
    CPSS_COM_LOGIC_ADDR_T stSLogAddr ;

    CPSS_COM_PCI_REG_RSP_MSG_T stRegRspMsg ;

    /*得到本板物理、逻辑地址*/
    cpss_com_phy_addr_get(&stMPhyAddr);
    cpss_com_logic_addr_get(&stMLogAddr,&ulAddrFlag);

    /*计算SLAVE的物理、逻辑地址*/
    cpss_com_drv_pci_get_addr_by_cpuno(ulCpuNo,&stSLogAddr,&stSPhyAddr);

    /*填充消息体*/
    stRegRspMsg.ulMsgId = CPSS_COM_MCPU_REG_RSP_MSG ;
    stRegRspMsg.stMLogAddr = stMLogAddr ;
    stRegRspMsg.stMPhyAddr = stMPhyAddr ;
    stRegRspMsg.stSLogAddr = stSLogAddr ;
    stRegRspMsg.stSPhyAddr = stSPhyAddr ;

    /*计算校验和*/
    stRegRspMsg.ulCheckSum = CPSS_COM_PCI_CHECK_VALUE;

    lRet = cpss_com_drv_pci_linkid_get(ulCpuNo,&ulLinkId);
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }

    stRegRspMsg.ulMsgId = cpss_htonl(stRegRspMsg.ulMsgId) ;
    stRegRspMsg.ulCheckSum = cpss_htonl(stRegRspMsg.ulCheckSum) ;
    stRegRspMsg.stMLogAddr.usGroup = cpss_htons(stRegRspMsg.stMLogAddr.usGroup) ;
    stRegRspMsg.stSLogAddr.usGroup = cpss_htons(stRegRspMsg.stSLogAddr.usGroup) ;

    /*调用PCI特殊写函数发送到从CPU*/
    lRet = cpss_com_pci_special_write(ulLinkId,sizeof(CPSS_COM_PCI_REG_REQ_MSG_T),
        (UINT8*)&stRegRspMsg,sizeof(CPSS_COM_PCI_REG_RSP_MSG_T));
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_pci_reg_req_deal
* 功    能:
* 函数类型:
* 参    数:
* 参数名称   类型     输入/输出         描述
* 函数返回:
* 说    明:
*******************************************************************************/
INT32 cpss_com_pci_reg_req_deal
(
    CPSS_COM_PCI_REG_REQ_MSG_T *pstRegMsg
)
{
    UINT32 ulCpuNo ;
    UINT32 ulAddrFlag ;
    INT32  lRet ;
    CPSS_COM_PHY_ADDR_T   stPhyAddrL ;
    CPSS_COM_PHY_ADDR_T   stPciPhyAddr ;
    CPSS_COM_LOGIC_ADDR_T stLogAddrL ;

    /*得到CPU号*/
    ulCpuNo = pstRegMsg->ulCpuNo ;

    /*得到本板物理、逻辑地址*/
    cpss_com_phy_addr_get(&stPhyAddrL);
    cpss_com_logic_addr_get(&stLogAddrL,&ulAddrFlag);

    /*本板的地址不存在,返回失败*/
    if((TRUE == CPSS_COM_LOGIC_ADDR_ZERO(stLogAddrL))&&
        (TRUE == CPSS_COM_PHY_ADDR_ZERO(stPhyAddrL)))
    {
        return CPSS_ERROR ;
    }

    /*创建注册CPU相关链路和路由*/
    lRet = cpss_com_pci_route_create_by_cpuno(stPhyAddrL,stLogAddrL,ulCpuNo);
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }

    /**/
    cpss_com_drv_pci_get_phyaddr(stPhyAddrL,ulCpuNo,&stPciPhyAddr);

    /*设置链路状态正常*/
    cpss_com_link_status_set(stPciPhyAddr,CPSS_COM_LINK_STATUS_NORMAL);

    /*使能PCI读任务*/
    lRet = cpss_com_drv_pci_enable_read() ;
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }

    /*设置对应的PCI链路可读可写*/
    cpss_com_drv_pci_wr_set(ulCpuNo,CPSS_COM_PCI_WR_ENABLE) ;

    /*向从CPU发送注册响应*/
    cpss_com_pci_reg_res_send(ulCpuNo);

    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_pci_notify_tm_deal
* 功    能:
* 函数类型:
* 参    数:
* 参数名称   类型     输入/输出         描述
* 函数返回:
* 说    明: 此函数只有在从CPU或DSP上调用
*******************************************************************************/
VOID cpss_com_pci_notify_tm_deal
(
    CPSS_COM_MSG_HEAD_T* pstMsgHead
)
{
    INT32 lRet ;

     lRet = 0 ;
#ifdef CPSS_SLAVE_CPU

    /*向所有的从cpu发送notify中断消息*/
    cpss_com_drv_pci_notify_send() ;

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "cpss_com_drv_pci_notify_send done");

#elif defined CPSS_DSP_CPU

    /*向所有的PCI缓冲区写入notify消息*/
    CPSS_DSP_FILE_REC();
    CPSS_DSP_LINE_REC();
    CPSS_DSP_RUN_ADDR_REC(CPSS_DSP_CPSS_REG_REQ_END);
    lRet = cpss_com_drv_pci_notify_write();
    if(CPSS_OK == lRet)
    {
        /*停止写PCI用户注册区定时器*/
        cpss_timer_delete(CPSS_COM_PCI_NOTIFY_TM_NO);
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,"delete pci reg timer.");
        return ;
    }

    CPSS_DSP_LINE_REC();
    CPSS_DSP_ERROR_WAIT;

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
        "cpss_com_drv_pci_notify_write! failed");

#endif
}

/*******************************************************************************
* 函数名称: cpss_com_pci_reg_rsp_deal
* 功    能:
* 函数类型:
* 参    数:
* 参数名称   类型     输入/输出         描述
* 函数返回:
* 说    明: 
*******************************************************************************/

#ifdef CPSS_DSP_CPU
extern UINT32 g_ulCpssRegFlag;
#endif

INT32 cpss_com_pci_reg_rsp_deal
(
    CPSS_COM_MSG_HEAD_T* pstMsgHdr
)
{
    INT32  lRet;
    UINT32 ulLinkId ;
    CPSS_COM_PCI_REG_RSP_MSG_T *pstRegRsp ;

#ifdef CPSS_DSP_CPU
    if(g_ulCpssRegFlag == 0)
    {
        return CPSS_ERROR;
    }
#endif
    pstRegRsp = (CPSS_COM_PCI_REG_RSP_MSG_T*)pstMsgHdr->pucBuf ;

    CPSS_DSP_FILE_REC();
    CPSS_DSP_LINE_REC();
    CPSS_DSP_RUN_ADDR_REC(CPSS_DSP_CPSS_REG_RES_END);
    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulRegRspInCount);

    pstRegRsp->ulMsgId = cpss_ntohl(pstRegRsp->ulMsgId) ;
    pstRegRsp->ulCheckSum = cpss_ntohl(pstRegRsp->ulCheckSum) ;
    pstRegRsp->stMLogAddr.usGroup = cpss_ntohs(pstRegRsp->stMLogAddr.usGroup) ;
    pstRegRsp->stSLogAddr.usGroup = cpss_ntohs(pstRegRsp->stSLogAddr.usGroup) ;

    /*设置本从CPU的物理地址和逻辑地址*/
    g_pstComData->stAddrThis.stLogiAddr = pstRegRsp->stSLogAddr ;
    g_pstComData->stAddrThis.stPhyAddr  = pstRegRsp->stSPhyAddr ;
    g_pstComData->stAddrThis.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER ;

    /*填写主CPU的地址信息*/
    g_pstComData->stAddrMCup.stLogiAddr = pstRegRsp->stMLogAddr ;
    g_pstComData->stAddrMCup.stPhyAddr  = pstRegRsp->stMPhyAddr ;
    g_pstComData->stAddrMCup.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER ;

    /*创建链路和路由*/
    lRet = cpss_com_pci_route_create(pstRegRsp->stMPhyAddr,pstRegRsp->stMLogAddr);
    if(CPSS_OK != lRet)
    {
        CPSS_DSP_LINE_REC();
        CPSS_DSP_ERROR_WAIT;
        return CPSS_ERROR ;
    }

    /*设置所有的链路都已经正常*/
    cpss_com_link_status_set(pstRegRsp->stMPhyAddr,CPSS_COM_LINK_STATUS_NORMAL);

    /*删除notify重发定时器*/
#ifdef CPSS_SLAVE_CPU
    cpss_timer_delete(CPSS_COM_PCI_NOTIFY_TM_NO) ;
#endif

    /*得到链路ID*/
    cpss_com_drv_pci_linkid_get(CPSS_COM_PCI_MASTER_CPU_NO,&ulLinkId);

    g_stCpssComPciTbl.astPciLink[ulLinkId-1].stDstPhyAddr   = pstRegRsp->stMPhyAddr ;
    g_stCpssComPciTbl.astPciLink[ulLinkId-1].stDstLogAddr = pstRegRsp->stMLogAddr ;

    /*清空用户注册缓冲区为0*/
    cpss_com_pci_special_zero(ulLinkId);

    CPSS_DSP_SBBR_COUNTER_REC(g_pstCpssSbbrRecord->ulRegRspOutCount);
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称:  cpss_sbbr_get_dsp
* 功    能:
* 函数类型:
* 参    数:
* 参数名称   类型         输入/输出    描述
*            UINT32       输入         ulCpuNo：dsp的CPU号(0<=ulCpuNo<12)
             UINT8*       输入/输出    将得到的buf copy到该内存中
             UINT32*      输入/输出    输出黑匣子的长度
* 函数返回:  成功 ：CPSS_OK; 失败：CPSS_ERROR;
*
* 说    明: 黑匣子调用该函数得到黑匣子数据和长度
*******************************************************************************/
#ifdef CPSS_VOS_VXWORKS
INT32 cpss_sbbr_get_dsp(UINT32 ulCpuNo,UINT8 *pcAddr,UINT32 ulSize)
{
#ifdef CPSS_HOST_CPU_WITH_DSP
    UINT8 *pucBuf = NULL;
    UINT32 ulLen;
    UINT32 ulDspId;

    ulDspId = ulCpuNo - CPSS_DSP_BASE_NUM;

    if(ulDspId > CPSS_DSP_CPU_NUM)
    {
        return (CPSS_ERROR);
    }

    if(pcAddr == NULL ||ulSize == NULL )
    {
        return (CPSS_ERROR);
    }

    pucBuf = cpss_mem_malloc(CPSS_DSP_SBBR_LEN);
    if(pucBuf == NULL)
    {
        return (CPSS_ERROR);
    }

    cpss_mem_memset(pucBuf, 0 ,CPSS_DSP_SBBR_LEN);

    ulLen = CPSS_TEST_PCI_ADDR_BASE - DRV_PCI_DSP_BASE_AADR;

    drv_dsp_ext_mem_read(ulDspId,ulLen,CPSS_DSP_SBBR_LEN ,pucBuf);

    cpss_mem_memcpy((void*)pcAddr ,(void*)pucBuf,ulSize);


    cpss_mem_free(pucBuf);
#endif
    return (CPSS_OK);
}
/*******************************************************************************
* 函数名称:  cpss_com_dsp_sbbr_read
* 功    能:
* 函数类型:
* 参    数:
* 参数名称   类型         输入/输出    描述
*            UINT32       输入         ulCpuNo：dsp的CPU号(0<=ulCpuNo<12)
* 函数返回:  成功 ：CPSS_OK; 失败：CPSS_ERROR;
*
* 说    明: 读取DSO上的黑匣子数据，上传给GCPA
*******************************************************************************/
INT32 cpss_com_dsp_sbbr_read(UINT32 ulCpuNo)
{
#ifdef CPSS_HOST_CPU_WITH_DSP
    UINT8 *pucBuf = NULL;
    UINT32 ulLen;
    UINT32 ulDspId;

    ulDspId = ulCpuNo - CPSS_DSP_BASE_NUM;

    if(ulDspId > CPSS_DSP_CPU_NUM)
    {
        return (CPSS_ERROR);
    }

    pucBuf = cpss_mem_malloc(CPSS_DSP_SBBR_LEN);
    if(pucBuf == NULL)
    {
        return (CPSS_ERROR);
    }

    cpss_mem_memset(pucBuf, 0 ,CPSS_DSP_SBBR_LEN);

    ulLen = CPSS_TEST_PCI_ADDR_BASE - DRV_PCI_DSP_BASE_AADR;

    drv_dsp_ext_mem_read(ulDspId,ulLen,CPSS_DSP_SBBR_LEN ,pucBuf);

    cpss_sbbr_trans_dsp(ulCpuNo,pucBuf,CPSS_DSP_SBBR_LEN);

    cpss_mem_free(pucBuf);
#endif

    return (CPSS_OK);
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_link_slave_cpu_reset
* 功    能:
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* 函数返回:
*
* 说    明: 提供给SMSS调用的接口函数
*******************************************************************************/
INT32 cpss_com_link_slave_cpu_reset
(
    UINT32 ulCpuNo
)
{
    CPSS_COM_LOGIC_ADDR_T stSLogAddr={0} ;
    CPSS_COM_PHY_ADDR_T   stSPhyAddr={0} ;
    CPSS_COM_LINK_T*   pstLink ;
    INT32 lRet ;

#if 0
#ifdef CPSS_HOST_CPU_WITH_DSP
    /*调用驱动的函数初始化PCI的桥*/
    drv_dsp_init(ulCpuNo - CPSS_DSP_BASE_NUM);
    /* 调用SBBR函数上传黑匣子数据 */
    cpss_com_dsp_sbbr_read(ulCpuNo);

#endif
#endif

    /*找到对应物理地址*/
    lRet = cpss_com_drv_pci_get_addr_by_cpuno(ulCpuNo,&stSLogAddr,&stSPhyAddr);
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }

    /*找到对应的链路对象*/
    pstLink = cpss_com_link_find_real(stSPhyAddr) ;
    if(NULL == pstLink)
    {
        return CPSS_ERROR;
    }

    /*P信号量*/
    lRet = cpss_vos_mutex_p(g_stCpssComPciTbl.astPciLink[pstLink->ulLinkId-1].ulWriteMutex,
        WAIT_FOREVER);

    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR;
    }

    /*清空PCI注册缓冲区内存*/
    if( ulCpuNo>CPSS_COM_PCI_SLAVE_CPU_NO )
    cpss_com_pci_special_zero(pstLink->ulLinkId) ;

    /*置链路状态为故障*/
    pstLink->usStatus = CPSS_COM_LINK_STATUS_FAULT ;

    /*设置链路读写标志为不能读写*/
    g_stCpssComPciTbl.astPciLink[pstLink->ulLinkId-1].ulWRFlag = CPSS_COM_PCI_WR_DISABLE ;

    /*增加PCI 链路故障的统计*/
    cpss_com_pci_link_stat(pstLink->ulLinkId,CPSS_COM_PCI_FAULT_NUM_FLAG);

    /*v信号量*/
    cpss_vos_mutex_v(g_stCpssComPciTbl.astPciLink[pstLink->ulLinkId-1].ulWriteMutex) ;

    return CPSS_OK ;
}

#endif
/*******************************************************************************
* 函数名称: cpss_com_link_if_pci_link
* 功    能:
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL cpss_com_link_if_pci_link
(
    CPSS_COM_LOGIC_ADDR_T stLogAddr
)
{

#ifdef CPSS_HOST_CPU
    /*判断是否是本板的PCI链路*/
    if(stLogAddr.ucSubGroup != 1)
    {
        stLogAddr.ucSubGroup = 1 ;

        /*如果是本板PCI链路,返回TRUE*/
        return CPSS_COM_LOGIC_ADDR_THIS_CPU(stLogAddr);
    }
    else
    {
        return FALSE ;
    }
    /*对于从CPU和DSP永远返回TRUE*/
#else
    return TRUE ;
#endif
}
/*******************************************************************************
* 函数名称: cpss_com_irq_msg_deal
* 功    能:
* 函数类型:
* 参    数:
* 参数名称   类型     输入/输出         描述
* 函数返回:
* 说    明:
*******************************************************************************/
#ifndef CPSS_DSP_CPU
INT32 cpss_com_irq_msg_deal
(
    CPSS_COM_MSG_HEAD_T* pstMsgHead
)
{
    UINT8  *pucBuf ;
    UINT32 ulInterNo ;

    pucBuf    = pstMsgHead->pucBuf ;
    ulInterNo = *(UINT32*)(pucBuf+CPSS_IRQ_RSV_HEARD) ;

    switch(ulInterNo)
    {
#ifdef CPSS_PCI_INCLUDE
    case CPSS_COM_PCI_V_IRQ_NO:
        {
            INT32 lRet;
            CPSS_COM_PCI_REG_REQ_MSG_T stPciRegReq ;

            /*收到中断消息后回填相关信息*/
            stPciRegReq.ulMsgId = CPSS_COM_SCPU_REG_REQ_MSG ;
            stPciRegReq.ulCpuNo = CPSS_COM_PCI_SLAVE_CPU_NO ;
            stPciRegReq.ulSumCheck = CPSS_COM_SCPU_REG_REQ_MSG + CPSS_COM_PCI_SLAVE_CPU_NO ;
            lRet = cpss_com_pci_reg_req_deal(&stPciRegReq);
            if(CPSS_OK != lRet)
            {
                return CPSS_ERROR ;
            }
        }
        break;
#endif
    default:
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "recv unknow irq no(%d).",ulInterNo);
        break;
    }

    return CPSS_OK ;
}
#endif

/*******************************************************************************
* 函数名称: cpss_com_link_if_dsp_link
* 功    能:
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* 函数返回:
*
* 说    明:
*******************************************************************************/
BOOL cpss_com_link_if_dsp_link
(
    UINT32 ulCpuNo
)
{
    if((ulCpuNo>=CPSS_COM_PCI_DSP_CPU_NO_MIN)&&
        (ulCpuNo<=CPSS_COM_PCI_DSP_CPU_NO_MAX))
    {
        return TRUE ;
    }
    return FALSE ;
}

/*******************************************************************************
* 函数名称: cpss_com_switch_brd_standby2active
* 功    能:
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* 函数返回:
*
* 说    明:
*******************************************************************************/
INT32 cpss_com_switch_brd_standby2active()
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_LOGIC_ADDR_T stLogAddr ;
    CPSS_COM_ROUTE_T* pstRoute = NULL ;

    g_ulCpssMateChannelFlag = CPSS_COM_MATE_CHANNEL_DISABLE ;

    stLogAddr = g_pstComData->stAddrThis.stLogiAddr ;

    pstRoute = cpss_com_route_find_real(stLogAddr) ;
    if(NULL == pstRoute)
    {
        return CPSS_ERR_COM_ROUTE_NOT_EXIST ;
    }

    cpss_mem_memset(&pstRoute->stPhyAddr[1],0,sizeof(CPSS_COM_PHY_ADDR_T)) ;
#endif
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_switch_brd_active2standby
* 功    能:
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* 函数返回:
*
* 说    明:
*******************************************************************************/
INT32 cpss_com_switch_brd_active2standby()
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_LOGIC_ADDR_T stLogAddr ;
    CPSS_COM_PHY_ADDR_T stMatePhyAddr ;
    CPSS_COM_ROUTE_T* pstRoute = NULL ;

    g_ulCpssMateChannelFlag = CPSS_COM_MATE_CHANNEL_ENABLE ;

    stLogAddr = g_pstComData->stAddrThis.stLogiAddr ;
    stMatePhyAddr = g_pstComData->stMatePhyAddr ;

    pstRoute = cpss_com_route_find_real(stLogAddr) ;
    if(NULL == pstRoute)
    {
        return CPSS_ERR_COM_ROUTE_NOT_EXIST ;
    }

    pstRoute->stPhyAddr[0] = stMatePhyAddr ;
    pstRoute->stPhyAddr[1] = stMatePhyAddr ;
 #endif
    return CPSS_OK ;
}
/*******************************************************************************
* 函数名称: cpss_dsp_sbbr_record(UINT32 ulMsgId,UINT8 *pucBuf)
* 功    能:
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* 函数返回:
*
* 说    明: 
*******************************************************************************/
UINT32 g_ulCpssSubSystemMsgNum = 0;
UINT32 g_ulCpssCounter = 0;
void cpss_dsp_sbbr_record(UINT32 ulMsgId,UINT8 *pucMsgBuf,UINT32 ulMsgLen)
{
#ifdef CPSS_DSP_CPU
    UINT32 ulSec;
    UINT32 ulMs;
    UINT8 *pucBuf =(UINT8*)g_pstCpssSbbrRecord;

    if(g_ulCpssDspSbbrMsgRecSwitch)
    {
        cpss_timer_get_sec_and_ms(&ulSec,&ulMs);

        *(UINT32*)(pucBuf+g_ulCpssCounter) = cpss_htonl(g_ulCpssSubSystemMsgNum);
        g_ulCpssCounter = g_ulCpssCounter + 4;

        *(UINT32*)(pucBuf+g_ulCpssCounter) = cpss_htonl(ulMsgId);
        g_ulCpssCounter = g_ulCpssCounter + 4;

        *(UINT32*)(pucBuf+g_ulCpssCounter) = cpss_htonl(ulSec);
        g_ulCpssCounter = g_ulCpssCounter + 4;

        *(UINT32*)(pucBuf+g_ulCpssCounter) = cpss_htonl(ulMs);
        g_ulCpssCounter = g_ulCpssCounter + 4;

        *(UINT32*)(pucBuf+g_ulCpssCounter) = cpss_htonl(ulMsgLen);
        g_ulCpssCounter = g_ulCpssCounter + 4;

        if(ulMsgLen > CPSS_SBBR_MSG_FLOW_MAX)
        {
            ulMsgLen = CPSS_SBBR_MSG_FLOW_MAX;
        }

        cpss_mem_memcpy((pucBuf + g_ulCpssCounter),pucMsgBuf,ulMsgLen);

        g_ulCpssCounter = g_ulCpssCounter+ ulMsgLen;

        g_ulCpssSubSystemMsgNum = g_ulCpssSubSystemMsgNum + 1;

        if(g_ulCpssSubSystemMsgNum %CPSS_SBBR_MSG_RECORD_MAX == 0)
        {
            g_ulCpssCounter = 0;
        }
    }
#endif
}
/*******************************************************************************
* 函数名称: cpss_com_dsp_cpu_usage_get
* 功    能: 获取dsp的CPU占用率
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:
*        无
* 说   明:
*******************************************************************************/
extern void cpss_kw_dsp_cpu_usage_update(UINT32 ulDspNum, UINT32 ulDspCpuUsage) ;

void cpss_com_dsp_cpu_usage_get(CPSS_COM_MSG_HEAD_T* pstMsgHead)
{
#ifdef  CPSS_HOST_CPU_WITH_DSP
    CPSS_COM_DSP_CPU_USAGE_T *pstDspCpuUsage = NULL;

    pstDspCpuUsage = (CPSS_COM_DSP_CPU_USAGE_T *)(pstMsgHead->pucBuf);
    cpss_kw_dsp_cpu_usage_update(cpss_ntohl(pstDspCpuUsage->ulDspNum) , cpss_ntohl(pstDspCpuUsage->ulDspCpuUsage));

    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO," recv CPSS_COM_DSP_CPU_USAGE_MSG msg .DspNum :%d,DspCpuUsage:%d"
                ,cpss_ntohl(pstDspCpuUsage->ulDspNum),cpss_ntohl(pstDspCpuUsage->ulDspCpuUsage));
#endif
    return ;
}
/*******************************************************************************
* 函数名称: cpss_com_link_proc
* 功    能: 通信链路纤程
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* usUserState   UINT16                 输入         业务状态
* pvVar         VOID *                 输入         纤程私有数据区指针
* pstMsgHead    CPSS_COM_MSG_HEAD_T*   输入         消息头指针
* 返回值:
*        无
* 说   明:业务状态，除了初始状态之外CPSS_VK_PROC_USER_STATE_IDLE，其它状态由
*         用户自己定义。
*         纤程私有数据区指针，由用户使用，数据区的大小是静态配置的
*******************************************************************************/
#ifdef CPSS_DSP_CPU

VOID cpss_com_link_proc
(
 UINT16  usUserState,
 VOID *  pvVar,
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
    UINT32 ulMsgID = pstMsgHead->ulMsgId;

    CPSS_DSP_FILE_REC();
    CPSS_DSP_LINE_REC();
    CPSS_DSP_RUN_ADDR_REC(CPSS_DSP_CPSS_LINK_PROC_ENTRY);
    CPSS_DSP_MSGID_REC(pstMsgHead->ulMsgId);
    CPSS_DSP_PRCID_REC(CPSS_COM_LINK_MNGR_PROC);

    cpss_dsp_sbbr_record(pstMsgHead->ulMsgId,pstMsgHead->pucBuf,pstMsgHead->ulLen);

    /* 初始化状态需要处理的消息:反向地址解析、上电、纤程激活、链路建立 */
    if(CPSS_COM_LINK_PROC_STATE_INIT == usUserState)
    {
        switch(pstMsgHead->ulMsgId)
        {
            /* CPSS初始化消息 */
        case CPSS_COM_INIT_MSG:
            {
                /* 通信模块初始化 */

                CPSS_DSP_FILE_REC();
                CPSS_DSP_LINE_REC();
                CPSS_DSP_RUN_ADDR_REC(CPSS_DSP_COM_INIT_END);
                if(CPSS_OK != cpss_com_init())
                {
                    CPSS_DSP_LINE_REC();
                    CPSS_DSP_ERROR_WAIT;
                    return;
                }
                CPSS_DSP_LINE_REC();

                /*初始化流程统计*/

                break;
            }

           /* SMSS纤程激活消息 */
        case SMSS_PROC_ACTIVATE_REQ_MSG:
            {
                cpss_com_active_proc_rsp_send(CPSS_OK);
                break;
            }

            /* 链路建立响应消息 */
        case CPSS_COM_LINK_RSP_MSG:
            {
                UINT32 ulResult=cpss_com_link_rsp_recv(pstMsgHead);

                /* 向SMSS发送纤程激活响应消息 */
                if (ulResult == CPSS_OK)
                {
                    cpss_com_active_proc_rsp_send(ulResult);
                }
                break;
            }

            /* 链路建立请求定时器消息 */
        case CPSS_COM_LINK_REQ_TM_MSG:
            {
                CPSS_TIMER_MSG_T *pstMsg        = (CPSS_TIMER_MSG_T*)pstMsgHead->pucBuf;
                CPSS_COM_PHY_ADDR_T *pstPhyAddr = (CPSS_COM_PHY_ADDR_T *)&pstMsg->ulPara;
                CPSS_COM_LINK_T *pstLink;

                pstLink = cpss_com_link_find_real(*pstPhyAddr);

                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_DETAIL,
                    "link request timer received.");

                /* 重新发送建立链路请求消息 */
                cpss_com_link_req_send(*pstPhyAddr,pstLink->ulMateFlag);

                break;
            }

            /*处理PCI的注册响应消息*/
        case CPSS_COM_MCPU_REG_RSP_MSG:
            {
                UINT32 ulResult = cpss_com_pci_reg_rsp_deal(pstMsgHead);

                if(ulResult == CPSS_OK)
                {
                cpss_com_power_on_send() ;
                }

                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"cpss_com_pci_reg_rsp_deal done");

                break ;
            }

        /*处理PCI的NOTIFY定时消息*/
        case CPSS_COM_PCI_NOTIFY_TM_MSG:
            {
                cpss_com_pci_notify_tm_deal(pstMsgHead);

                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"cpss_com_pci_notify_tm_deal done");

                break ;
            }

        default:
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
                "link proc received message id 0x%08x,srcProcId=0x%x,dstProcId=0x%x,srcAddr=0x%x,dstAddr=0x%x,at initial state.\n",
                pstMsgHead->ulMsgId,pstMsgHead->stSrcProc.ulPd,pstMsgHead->stDstProc.ulPd,
                *(UINT32*)&pstMsgHead->stSrcProc.stLogicAddr,*(UINT32*)&pstMsgHead->stDstProc.stLogicAddr);
        }
    }
    /* 正常运行状态需要处理的消息:心跳、地址解析、链路备份、备升主 */
    else if(CPSS_COM_LINK_PROC_STATE_NORMAL == usUserState)
    {
        switch(pstMsgHead->ulMsgId)
        {
            /* 链路建立请求消息,主控板 */
        case CPSS_COM_LINK_REQ_MSG:
            {
                cpss_com_link_req_recv(pstMsgHead);
                break;
            }

            /* 链路建立响应消息,外围板 */
        case CPSS_COM_LINK_RSP_MSG:
            {
                cpss_com_link_rsp_recv(pstMsgHead);
                break;
            }

            /* 链路心跳消息 */
        case CPSS_COM_LINK_HEARTBEAT_MSG:
            {
                cpss_com_link_heartbeat_recv(pstMsgHead);
                break;
            }

            /* 链路心跳定时器消息 */
        case CPSS_COM_LINK_HEART_TM_MSG:
            {
                /* 发送心跳消息 */
                cpss_com_link_heartbeat_send();

                break;
            }

            /* 链路故障定时器消息 */
        case CPSS_COM_LINK_FAULT_TM_MSG:
            {
                break;
            }

            /* 链路建立请求定时器消息 */
        case CPSS_COM_LINK_REQ_TM_MSG:
            {
                CPSS_TIMER_MSG_T *pstMsg        = (CPSS_TIMER_MSG_T*)pstMsgHead->pucBuf;
                CPSS_COM_PHY_ADDR_T *pstPhyAddr = (CPSS_COM_PHY_ADDR_T *)&pstMsg->ulPara;
                CPSS_COM_LINK_T *pstLink;

                pstLink = cpss_com_link_find_real(*pstPhyAddr);

                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_DETAIL,
                    "link request timer received.");

               if( 0 != CPSS_COM_PHY_ADDR_ZERO(*pstPhyAddr))
                {
                    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                        "link request timer, phy address is NULL!");
                }

                /* 重新发送建立链路请求消息 */
                cpss_com_link_req_send(*pstPhyAddr,pstLink->ulMateFlag);

                break;
            }

           /*中断通知消息*/
#if 0
        case CPSS_COM_IRQ_IND_MSG:
            {
                /*处理接收到的中断通知消息*/
                cpss_com_irq_msg_deal(pstMsgHead);
                break ;
            }
#endif
            /*处理PCI的NOTIFY定时消息*/
           /*处理PCI的注册请求消息*/
        case CPSS_COM_SCPU_REG_REQ_MSG:
            {
                cpss_com_pci_reg_req_deal((CPSS_COM_PCI_REG_REQ_MSG_T *)(pstMsgHead->pucBuf));

                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"cpss_com_pci_reg_req_deal done");

                break ;
            }
        case 1234567:
            {
                int i ;
                printf("recv data from slave or master cpu!len=(%d)\n",pstMsgHead->ulLen) ;
                for(i=0;i<100;i++)
                {
                    printf("%c",*(pstMsgHead->pucBuf+i));
                }
                printf("\n");
            }

        default:
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
                "link proc received message id 0x%08x ,srcProcId=0x%x,dstProcId=0x%x,srcAddr=0x%x,dstAddr=0x%x, at normal state.\n",
                pstMsgHead->ulMsgId,pstMsgHead->stSrcProc.ulPd,pstMsgHead->stDstProc.ulPd,
                *(UINT32*)&pstMsgHead->stSrcProc.stLogicAddr,*(UINT32*)&pstMsgHead->stDstProc.stLogicAddr);
        }
    }   /* end if  (CPSS_COM_LINK_PROC_STATE_INIT == usUserState) */
}


#else

VOID cpss_com_link_proc
(
 UINT16  usUserState,
 VOID *  pvVar,
 CPSS_COM_MSG_HEAD_T *pstMsgHead
 )
{
    UINT32 ulMsgID = pstMsgHead->ulMsgId;

    CPSS_DSP_FILE_REC();
    CPSS_DSP_LINE_REC();
    CPSS_DSP_RUN_ADDR_REC(CPSS_DSP_CPSS_LINK_PROC_ENTRY);
    CPSS_DSP_MSGID_REC(pstMsgHead->ulMsgId);
    CPSS_DSP_PRCID_REC(CPSS_COM_LINK_MNGR_PROC);

/**
    cpss_dsp_sbbr_record(pstMsgHead->ulMsgId,pstMsgHead->pucBuf);
**/

#if 0
    if ((ulMsgID < CPSS_TIMER_00_MSG) || (ulMsgID > CPSS_TIMER_31_MSG))
    {
        cpss_output(CPSS_MODULE_COM, CPSS_PRINT_INFO,
                    "receive MsgID = %#x ", pstMsgHead->ulMsgId);
    }
#endif
    /* 初始化状态需要处理的消息:反向地址解析、上电、纤程激活、链路建立 */
    if(CPSS_COM_LINK_PROC_STATE_INIT == usUserState)
    {
        switch(pstMsgHead->ulMsgId)
        {
            /* CPSS初始化消息 */
        case CPSS_COM_INIT_MSG:
            {
                /* 通信模块初始化 */
                CPSS_DSP_FILE_REC();
                CPSS_DSP_LINE_REC();
                CPSS_DSP_RUN_ADDR_REC(CPSS_DSP_COM_INIT_END);
                if(CPSS_OK != cpss_com_init())
                {
                    CPSS_DSP_LINE_REC();
                    CPSS_DSP_ERROR_WAIT;
                    return;
                }

#if(SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
                if(CPSS_OK != cpss_com_init_mate())
                {
                    CPSS_DSP_LINE_REC();
//                	CPSS_DSP_ERROR_WAIT;
//                	return;
                }
#endif
                CPSS_DSP_LINE_REC();
                /*初始化流程统计*/
#ifndef CPSS_DSP_CPU
                g_stCpssComInitStat.ulRecvInitMsgFlag = 1 ;
#endif

#ifdef CPSS_HOST_CPU
                /* 如果本板是常量逻辑地址，则不需要反向地址解析 */
                if( FALSE == CPSS_COM_LOGIC_ADDR_ZERO(g_pstComData->stAddrThis.stLogiAddr))
                {
                    /* 如果是主控板，则总是存在备份板 */
                    BOOL bMcFlag;
                    bMcFlag = cpss_local_funcbrd_type_is_mc();
                    if (TRUE == bMcFlag)
                    {
                        if(CPS__RDBS_DEV_11_BACKUP == g_pstComData->stAddrThis.ucBackupType)
                        {
                            if(CPSS_COM_PHY_ADDR_SAME(g_pstComData->stAddrThis.stPhyAddr,
                                g_pstComData->stAddrGcpaM.stPhyAddr))
                            {
                                g_pstComData->stMatePhyAddr=g_pstComData->stAddrGcpaS.stPhyAddr;
                            }
                            else
                            {
                                g_pstComData->stMatePhyAddr=g_pstComData->stAddrGcpaM.stPhyAddr;
                            } /* end if (CPSS_COM_PHY_ADDR_SAME... */
                        }  /* end   if((SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)... */
                    }
                    /* 向SMSS发送上电消息 */
                    cpss_com_power_on_send();
                }
                else/* 发送反向地址解析请求 */
                {
#if (SWP_FUNBRD_ABOX != SWP_FUNBRD_TYPE)
                    cpss_com_rarp_req_send();
#endif
                } /* end if ( 0 == CPSS_COM_LOGIC_ADDR_ZERO... */
#endif

#if (SWP_FUNBRD_ABOX  == SWP_FUNBRD_TYPE)
                cpss_com_power_on_send();
#endif
                break;
            }

            /* 反向地址解析响应消息 */
        case CPSS_COM_RARP_RSP_MSG:
            {
                cpss_com_rarp_rsp_recv(pstMsgHead);
                break;
            }

            /* 主备通道建立响应消息 */
        case CPSS_COM_MATE_CHANNEL_RSP_MSG:
            {
                BOOL bMcFlag;
                cpss_com_mate_channel_rsp_recv(pstMsgHead);

                /* 向SMSS发送上电消息 */
                bMcFlag = cpss_local_funcbrd_type_is_mc();
                if(TRUE == bMcFlag)
                {
                    cpss_com_active_proc_rsp_send(CPSS_OK);
                }
                break;
            }

#ifdef CPSS_SWITCH_BRDTYPE
        case CPSS_COM_MATE_CHANNEL_REQ_MSG:
            {
                cpss_com_swtich_brd_recv_mate_req_deal(pstMsgHead);
                cpss_com_mate_channel_req_recv(pstMsgHead);

                if(g_ulCpssPoweronNum == 0)
                {
                    cpss_timer_delete(CPSS_COM_LINK_RARP_TM_NO);
                    cpss_com_power_on_send();
                }
                g_ulCpssPoweronNum = g_ulCpssPoweronNum + 1 ;
                break ;
            }
            /* 链路心跳消息 */
        case CPSS_COM_LINK_HEARTBEAT_MSG:
            {
                cpss_com_link_heartbeat_recv(pstMsgHead);
                break;
            }
            /* 链路心跳定时器消息 */
        case CPSS_COM_LINK_HEART_TM_MSG:
            {
                /* 发送心跳消息 */
                cpss_com_link_heartbeat_send() ;

                break;
            }
#endif
            /* SMSS纤程激活消息 */
        case SMSS_PROC_ACTIVATE_REQ_MSG:
            {
#ifdef CPSS_HOST_CPU

#if (SWP_FUNBRD_ABOX  == SWP_FUNBRD_TYPE)
                cpss_com_active_proc_rsp_send(CPSS_OK) ;
#else
                BOOL bMcFlag=FALSE;
                cpss_com_active_proc_req_recv(pstMsgHead);

                bMcFlag = cpss_local_funcbrd_type_is_mc();
                if(FALSE == bMcFlag)
                {
                    cpss_com_link_req_send(g_pstComData->stAddrGcpaM.stPhyAddr,CPSS_COM_CTRL_CHANNEL);
                }

                /*
                * 如果本板存在伙伴板，发送主备通道建立请求消息；
                * 否则，向smss发送纤程激活响应消息
                */
                if( 0 == CPSS_COM_PHY_ADDR_ZERO(g_pstComData->stMatePhyAddr))
                {
                    cpss_com_mate_channel_req_send();
                    cpss_timer_set(CPSS_COM_MATE_REQ_TM_NO, CPSS_COM_MATE_REQ_TM_INTERVAL);
                }
                else
                {
                    /* 如果是主控板,直接发送激活响应 */
                    if(TRUE == bMcFlag)
                    {
                        cpss_com_active_proc_rsp_send(CPSS_OK);
                    }
                }
#endif

#else
                cpss_com_active_proc_rsp_send(CPSS_OK);
#endif
                break;
            }

            /* 链路建立响应消息 */
        case CPSS_COM_LINK_RSP_MSG:
            {
                UINT32 ulResult=cpss_com_link_rsp_recv(pstMsgHead);

                /* 向SMSS发送纤程激活响应消息 */
                if (ulResult == CPSS_OK)
                {
                    cpss_com_active_proc_rsp_send(ulResult);
                }
                break;
            }

            /* 反向地址解析请求定时器消息 */
        case CPSS_COM_LINK_RARP_TM_MSG:
            {
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
                    "rarp timer received.");

                /* 发送反向地址解析请求 */
                cpss_com_rarp_req_send();
                break;
            }

            /* 链路建立请求定时器消息 */
        case CPSS_COM_LINK_REQ_TM_MSG:
            {
                CPSS_TIMER_MSG_T *pstMsg        = (CPSS_TIMER_MSG_T*)pstMsgHead->pucBuf;
                CPSS_COM_PHY_ADDR_T *pstPhyAddr = (CPSS_COM_PHY_ADDR_T *)&pstMsg->ulPara;
                CPSS_COM_LINK_T *pstLink;

                pstLink = cpss_com_link_find_real(*pstPhyAddr);

                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_DETAIL,
                    "link request timer received.");

                /* 重新发送建立链路请求消息 */
                cpss_com_link_req_send(*pstPhyAddr,pstLink->ulMateFlag);

                break;
            }

            /* 主备通道建立请求定时器消息 */
        case CPSS_COM_MATE_REQ_TM_MSG:
            {
                BOOL bMcFlag;
            /* 主备通道建立请求定时器超时,说明伙伴板还没有启动,
            * 向SMSS发送上电消息,本板先启动
                */
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
                    "mate req timer received.");
                bMcFlag = cpss_local_funcbrd_type_is_mc();
                if(TRUE == bMcFlag)
                {
                    cpss_com_active_proc_rsp_send(CPSS_OK);
                }

                break;
            }
            /*处理PCI的注册响应消息*/
#ifdef CPSS_PCI_INCLUDE
        case CPSS_COM_MCPU_REG_RSP_MSG:
            {
                cpss_com_pci_reg_rsp_deal(pstMsgHead);

                cpss_com_power_on_send() ;

                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"cpss_com_pci_reg_rsp_deal done");

                break ;
            }

        /*处理PCI的NOTIFY定时消息*/
        case CPSS_COM_PCI_NOTIFY_TM_MSG:
            {
                cpss_com_pci_notify_tm_deal(pstMsgHead);

                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"cpss_com_pci_notify_tm_deal done");

                break ;
            }
#endif

        default:
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
                "link proc received message id 0x%08x,srcProcId=0x%x,dstProcId=0x%x,srcAddr=0x%x,dstAddr=0x%x,at initial state.\n",
                pstMsgHead->ulMsgId,pstMsgHead->stSrcProc.ulPd,pstMsgHead->stDstProc.ulPd,
                *(UINT32*)&pstMsgHead->stSrcProc.stLogicAddr,*(UINT32*)&pstMsgHead->stDstProc.stLogicAddr);
        }
    }
    /* 正常运行状态需要处理的消息:心跳、地址解析、链路备份、备升主 */
    else if(CPSS_COM_LINK_PROC_STATE_NORMAL == usUserState)
    {
        switch(pstMsgHead->ulMsgId)
        {
            /* 反向地址解析请求消息,主控板 */
#ifdef CPSS_FUNBRD_MC
        case CPSS_COM_RARP_REQ_MSG:
            {
                cpss_com_rarp_req_recv(pstMsgHead);
                break;
            }

            /* 地址解析请求消息 */
        case CPSS_COM_ARP_REQ_MSG:
            {
                cpss_com_arp_req_recv(pstMsgHead);
                break;
            }
#endif
            /* 应用接口地址解析请求消息 */
        case CPSS_COM_APP_ARP_REQ_MSG:
            {
                cpss_com_app_arp_req_recv(pstMsgHead);
                break;
            }

            /* 地址解析响应消息 */
        case CPSS_COM_ARP_RSP_MSG:
            {
                cpss_com_arp_rsp_recv(pstMsgHead);
                break;
            }

            /* 链路建立请求消息,主控板 */
        case CPSS_COM_LINK_REQ_MSG:
            {
                cpss_com_link_req_recv(pstMsgHead);
                break;
            }

            /* 链路建立响应消息,外围板 */
        case CPSS_COM_LINK_RSP_MSG:
            {
                cpss_com_link_rsp_recv(pstMsgHead);
                break;
            }

            /* 链路心跳消息 */
        case CPSS_COM_LINK_HEARTBEAT_MSG:
            {
                cpss_com_link_heartbeat_recv(pstMsgHead);
                break;
            }

            /* 链路心跳定时器消息 */
        case CPSS_COM_LINK_HEART_TM_MSG:
            {
                /* 发送心跳消息 */
                cpss_com_link_heartbeat_send();

                break;
            }

            /* 链路故障定时器消息 */
        case CPSS_COM_LINK_FAULT_TM_MSG:
            {
#if 0
                cpss_com_link_fault_recv(pstMsgHead);
#endif
                break;
            }

            /* 链路建立请求定时器消息 */
        case CPSS_COM_LINK_REQ_TM_MSG:
            {
                CPSS_TIMER_MSG_T *pstMsg        = (CPSS_TIMER_MSG_T*)pstMsgHead->pucBuf;
                CPSS_COM_PHY_ADDR_T *pstPhyAddr = (CPSS_COM_PHY_ADDR_T *)&pstMsg->ulPara;
                CPSS_COM_LINK_T *pstLink;

                pstLink = cpss_com_link_find_real(*pstPhyAddr);

                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_DETAIL,
                    "link request timer received.");

               if( 0 != CPSS_COM_PHY_ADDR_ZERO(*pstPhyAddr))
                {
                    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                        "link request timer, phy address is NULL!");
                }

                /* 重新发送建立链路请求消息 */
                cpss_com_link_req_send(*pstPhyAddr,pstLink->ulMateFlag);

                break;
            }

        case SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG:
            /* CPSS仅在收到SMSS的"纤程激活完成指示"消息后,才允许TCP/DC连接接入 */
            g_ulCpssComDrvTcpMutex=cpss_vos_mutex_create();
            if(CPSS_VOS_MUTEXD_INVALID == g_ulCpssComDrvTcpMutex)
            {
                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                    "link proc, create tcp mutex failed!");
                return;
            }

            /*设置单板正常标志*/
            g_ulCpssBrdNormalFlag = TRUE ;

            break;

        case SMSS_STANDBY_TO_ACTIVE_REQ_MSG:    /* 备升主的请求消息 */
            {
                /* CPSS_COM_PID_T                   stDestPid; */
                SMSS_STANDBY_TO_ACTIVE_RSP_MSG_T stToActiveResult;

                /* 备升主响应消息体赋值 */
                stToActiveResult.ulResult = SMSS_OK;

                /* 发送消息 */
                if (CPSS_OK != cpss_com_send_phy(g_pstComData->stAddrThis.stPhyAddr,
                    pstMsgHead->stSrcProc.ulPd,
                    SMSS_STANDBY_TO_ACTIVE_RSP_MSG,
                    (UINT8 *)&stToActiveResult,
                    sizeof(SMSS_STANDBY_TO_ACTIVE_RSP_MSG_T)))
                {
                    cpss_output( CPSS_MODULE_COM, CPSS_PRINT_ERROR,
                        "link proc send SMSS_STANDBY_TO_ACTIVE_RSP_MSG failed!");
                }

                /* 更新本板主备状态 */
                g_pstComData->stAddrThis.ulAddrFlag=CPSS_COM_ADDRFLAG_MASTER;

#ifdef CPSS_SWITCH_BRDTYPE
                cpss_com_switch_brd_standby2active();
                /*删除路由*/
#ifdef CPSS_VOS_VXWORKS
             /*   cpss_com_gcpa_route_deal(CPSS_COM_ROUTE_DEL) ;*/
#endif
#endif
                /*切换本板GCPA的主备物理地址*/
                cpss_com_link_gcpa_switch(g_pstComData->stAddrThis.stPhyAddr,
                                          CPSS_COM_ADDRFLAG_MASTER);

                /*切换本板的路由*/
                cpss_com_link_selfroute_switch();

                /*发送切换指示心跳*/
                cpss_com_link_heartbeat_send() ;
                break;
            }
        case SMSS_ACTIVE_TO_STANDBY_IND_MSG:
            {
                /* 更新本板主备状态 */
                g_pstComData->stAddrThis.ulAddrFlag=CPSS_COM_ADDRFLAG_SLAVE;
#ifdef CPSS_SWITCH_BRDTYPE
                cpss_com_switch_brd_active2standby();
#ifdef CPSS_VOS_VXWORKS
             /*  cpss_com_gcpa_route_deal(CPSS_COM_ROUTE_DEL) ;*/
#endif
#endif

                /*切换本板GCPA的主备物理地址*/
                cpss_com_link_gcpa_switch(g_pstComData->stAddrThis.stPhyAddr,
                                          CPSS_COM_ADDRFLAG_SLAVE);
                /*切换本板的路由*/
                cpss_com_link_selfroute_switch();
                break;
            }
            /* 主备通道建立请求消息 */
        case CPSS_COM_MATE_CHANNEL_REQ_MSG:
            {
                cpss_com_mate_channel_req_recv(pstMsgHead);
                break;
            }
            /*处理主备通道建立响应消息*/
        case CPSS_COM_MATE_CHANNEL_RSP_MSG:
            {
                cpss_com_mate_channel_rsp_recv(pstMsgHead);
                break ;
            }
           /*中断通知消息*/
        case CPSS_COM_IRQ_IND_MSG:
            {
                /*处理接收到的中断通知消息*/
                cpss_com_irq_msg_deal(pstMsgHead);
                break ;
            }
            /*处理PCI的NOTIFY定时消息*/
#ifdef CPSS_PCI_INCLUDE
           /*处理PCI的注册请求消息*/
        case CPSS_COM_SCPU_REG_REQ_MSG:
            {
                cpss_com_pci_reg_req_deal((CPSS_COM_PCI_REG_REQ_MSG_T *)(pstMsgHead->pucBuf));

                cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,"cpss_com_pci_reg_req_deal done");

                break ;
            }
#endif
#ifdef CPSS_FUNBRD_MC
        case CPSS_COM_GET_LOGIC_ADDR_REQ_MSG:
            {
                /*处理接收到得到逻辑地址请求消息的处理*/
                cpss_com_logic_addr_get_req_deal(pstMsgHead->pucBuf) ;
                break;
            }
#endif
        case 1234567:
            {
                int i ;
                printf("recv data from slave or master cpu!len=(%d)\n",pstMsgHead->ulLen) ;
                for(i=0;i<100;i++)
                {
                    printf("%c",*(pstMsgHead->pucBuf+i));
                }
                printf("\n");
                break;
            }

        case CPSS_COM_DSP_CPU_USAGE_MSG:
            {
                cpss_com_dsp_cpu_usage_get(pstMsgHead);
                break;
            }

        default:
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
                "link proc received message id 0x%08x ,srcProcId=0x%x,dstProcId=0x%x,srcAddr=0x%x,dstAddr=0x%x, at normal state.\n",
                pstMsgHead->ulMsgId,pstMsgHead->stSrcProc.ulPd,pstMsgHead->stDstProc.ulPd,
                *(UINT32*)&pstMsgHead->stSrcProc.stLogicAddr,*(UINT32*)&pstMsgHead->stDstProc.stLogicAddr);
        }
    }   /* end if  (CPSS_COM_LINK_PROC_STATE_INIT == usUserState) */
}
#endif

/*******************************************************************************
* 函数名称: cpss_com_phy_addr_get
* 功    能: 获得自身的物理地址
*
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* pstPhyAddr    CPSS_COM_PHY_ADDR_T *    输出        指针，用于返回自身的物理地址
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:
*******************************************************************************/
INT32 cpss_com_phy_addr_get
(
 CPSS_COM_PHY_ADDR_T *pstPhyAddr
 )
{
    UINT32 ulPhyAddr;
    INT32 lRet;

    if(NULL == pstPhyAddr)
    {
        return CPSS_ERR_COM_PARA_ILLEGAL;
    }
    if(NULL == g_pstComData)
    {
#ifdef CPSS_HOST_CPU

#ifdef SWP_FNBLK_BRDTYPE_ABOX
       ulPhyAddr = CPSS_ALARM_BOX_IP_ADDR ;
#else
       drv_ipmi_addr_get(0x01, &ulPhyAddr);
#endif
       lRet = cpss_com_phyaddr_format_change(ulPhyAddr,pstPhyAddr) ;
       if(lRet != CPSS_OK)
       {
           return CPSS_ERROR;
       }
#ifndef SWP_FNBLK_BRDTYPE_ABOX
       pstPhyAddr->ucCpu = 1;
#endif
#endif
       return CPSS_OK;
    }
    *pstPhyAddr=g_pstComData->stAddrThis.stPhyAddr;

    return CPSS_OK;
}


/*******************************************************************************
* 函数名称: cpss_com_phy_addr_mate_get
* 功    能: 获得伙伴板的物理地址
*
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* pstPhyAddr    CPSS_COM_PHY_ADDR_T *    输出        指针，用于返回自身的物理地址
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:
*
*******************************************************************************/
#ifndef CPSS_DSP_CPU
INT32 cpss_com_phy_addr_mate_get
(
CPSS_COM_PHY_ADDR_T *pstPhyAddrMate
)
{
    if(1 == CPSS_COM_PHY_ADDR_ZERO(g_pstComData->stMatePhyAddr))
    {
        return CPSS_ERROR ;
    }

    *pstPhyAddrMate = g_pstComData->stMatePhyAddr ;
    return CPSS_OK ;
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_logic_addr_get
* 功    能: 获得自身的逻辑地址
*
* 函数类型:
* 参    数:
* 参数名称        类型                    输入/输出         描述
* pstLogicAddr  CPSS_COM_LOGIC_ADDR_T *    输出        指针，用于返回自身的逻辑地址
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:
*
*******************************************************************************/
INT32 cpss_com_logic_addr_get
(
 CPSS_COM_LOGIC_ADDR_T *pstLogicAddr,
 UINT32                *pulAddrFlag
 )
{
    if((NULL == pstLogicAddr) || (NULL == pulAddrFlag) || (NULL == g_pstComData))
    {
        return CPSS_ERR_COM_PARA_ILLEGAL;
    }
    *pstLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
    *pulAddrFlag  = g_pstComData->stAddrThis.ulAddrFlag;

    switch(pstLogicAddr->usGroup)
    {
    case 0x8111:
    	pstLogicAddr->usGroup = 1;
    	pstLogicAddr->ucSubGroup = 1;
    	break;
    case 0x8112:
    	pstLogicAddr->usGroup = 2;
    	pstLogicAddr->ucSubGroup = 1;
    	break;
    case 0x8113:
    	pstLogicAddr->usGroup = 2;
    	pstLogicAddr->ucSubGroup = 2;
    	break;
    case 0x8114:
    	pstLogicAddr->usGroup = 2;
    	pstLogicAddr->ucSubGroup = 3;
    	break;
    }
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_host_logic_addr_get
* 功    能: 获得主CPU的逻辑地址
*
* 函数类型:
* 参    数:
* 参数名称          类型                      输入/输出   描述
* pstHostLogicAddr  CPSS_COM_LOGIC_ADDR_T*    输出        指针，指定逻辑地址对应的主CPU逻辑地址
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:
*
*******************************************************************************/
INT32 cpss_com_host_logic_addr_get
(
 CPSS_COM_LOGIC_ADDR_T *pstHostLogicAddr,
 UINT32                *pulHostAddrFlag
 )
{
/**
    if((NULL == pstHostLogicAddr) || (NULL == pulHostAddrFlag))
    {
        return CPSS_ERR_COM_PARA_ILLEGAL;
    }
**/
    *pstHostLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
    *pulHostAddrFlag  = g_pstComData->stAddrThis.ulAddrFlag;

    pstHostLogicAddr->ucSubGroup=1;

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_send_forward
* 功    能:  cpss_com_is_local_address
*
* 函数类型:
* 参    数:
* 参数名称        类型                  输入/输出         描述
* pstDstPid   CPSS_COM_PID_T *          输入          目标纤程标识
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 只转发CPU间消息
*******************************************************************************/
BOOL cpss_com_is_local_address(const CPSS_COM_PID_T *pstDstPid)
{
    /* 本地特殊地址 */
    if(CPSS_COM_LOCAL_LOGIC_ADDR == (*(UINT32*)&pstDstPid->stLogicAddr))
    {
        return TRUE;
    }

    else if(CPSS_COM_PHY_ADDR_SAME(g_LocalBrdPhyAddr, pstDstPid->stLogicAddr))
    {
    	return TRUE;
    }
    else if(CPSS_COM_LOGIC_ADDR_SAME(pstDstPid->stLogicAddr, gLocalBrdLogAddr))
    {
    	return TRUE;
    }

    /* 物理地址跟本地相同 */
    else if(CPSS_COM_ADDRFLAG_PHYSICAL == pstDstPid->ulAddrFlag)
    {
        return CPSS_COM_PHY_ADDR_SAME(pstDstPid->stLogicAddr,
            g_pstComData->stAddrThis.stPhyAddr);
    }
    /* 逻辑地址跟本地相同，并且地址标记也相同 */
    else
    {
        return (CPSS_COM_LOGIC_ADDR_SAME(pstDstPid->stLogicAddr,g_pstComData->stAddrThis.stLogiAddr) &&
            (pstDstPid->ulAddrFlag == g_pstComData->stAddrThis.ulAddrFlag));
    }
    return FALSE;
}

/*******************************************************************************
* 函数名称: cpss_com_local_has_backup
* 功    能: 判断本CPU是否有备份关系。
*
* 函数类型:
* 参    数:
* 参数名称        类型       输入/输出         描述
* 函数返回:
*      有备份关系：CPSS_OK;
*      无备份关系：CPSS_ERROR；
* 说    明:
*
*******************************************************************************/
INT32 cpss_com_local_has_backup(VOID)
{
    if (g_pstComData->stAddrThis.ucBackupType != CPS__RDBS_DEV_11_BACKUP)
    {
        return (CPSS_ERROR);
    }
    else
    {
        return (CPSS_OK);
    }
}


/*******************************************************************************
* 函数名称: cpss_com_get_local_ip
* 功    能: 获取本板IP。
*
* 函数类型:
* 参    数:
* 参数名称   类型     输入/输出         描述
*
* 函数返回:
*       IP：成功－－有效IP；失败－－0；
* 说    明:
*        调用本函数前通信模块必需初始化完成。
*******************************************************************************/
UINT32 cpss_com_get_local_ip()
{
    return (g_pstComData->stAddrThis.ulIpAddr);
}

/*******************************************************************************
* 函数名称: cpss_com_link_if_normal
* 功    能: 判断链路状态是否正常
* 函数类型:
* 参    数:
* 参数名称   类型     输入/输出         描述
* 函数返回:
* 说    明:
*******************************************************************************/
BOOL cpss_com_link_if_normal
(
    CPSS_COM_PHY_ADDR_T stPhyAddr
)
{
    CPSS_COM_LINK_T* pstComLink ;

    if(TRUE == CPSS_COM_PHY_ADDR_ZERO(stPhyAddr))
    {
        return FALSE ;
    }

    pstComLink = cpss_com_link_find_real(stPhyAddr) ;
    if(NULL == pstComLink)
    {
        return FALSE ;
    }

    if(CPSS_COM_LINK_STATUS_NORMAL != pstComLink->usStatus)
    {
        return FALSE ;
    }

    return TRUE ;
}

/*******************************************************************************
* 函数名称: cpss_com_link_win_get_by_logaddr
* 功    能:
*
* 函数类型:
* 参    数:
* 参数名称   类型     输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:
*******************************************************************************/
INT32 cpss_com_link_win_get_by_logaddr
(
    VOID **ppvSlidWin,
    CPSS_COM_PID_T *pstLogPid
)
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_ROUTE_T *pstRoute ;
    CPSS_COM_PHY_ADDR_T stPhyAddr ;
    CPSS_COM_LINK_T *pstLink ;

    pstRoute  = cpss_com_route_find(pstLogPid->stLogicAddr) ;
    if (NULL == pstRoute)
    {
        return (CPSS_ERR_COM_ROUTE_NOT_EXIST);
    }
    stPhyAddr = pstRoute->stPhyAddr[pstLogPid->ulAddrFlag] ;

    if( 1 == CPSS_COM_PHY_ADDR_ZERO(stPhyAddr) )
    {
        *ppvSlidWin = NULL ;
        return CPSS_ERROR ;
    }

    pstLink = cpss_com_link_find(stPhyAddr) ;
    if(NULL == pstLink)
    {
        *ppvSlidWin = NULL ;
        return CPSS_ERROR ;
    }

    *ppvSlidWin = (VOID*)&(pstLink->stSlidWin) ;
#endif
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_link_win_get_by_logaddr_real
* 功    能:
*
* 函数类型:
* 参    数:
* 参数名称   类型     输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:
*******************************************************************************/
INT32 cpss_com_link_win_get_by_logaddr_real
(
    VOID **ppvSlidWin,
    CPSS_COM_PID_T *pstLogPid
)
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_ROUTE_T *pstRoute ;
    CPSS_COM_PHY_ADDR_T stPhyAddr ;
    CPSS_COM_LINK_T *pstLink ;

    pstRoute  = cpss_com_route_find_real(pstLogPid->stLogicAddr) ;
    if (NULL == pstRoute)
    {
        return (CPSS_ERR_COM_ROUTE_NOT_EXIST);
    }
    stPhyAddr = pstRoute->stPhyAddr[pstLogPid->ulAddrFlag] ;

    if( 1 == CPSS_COM_PHY_ADDR_ZERO(stPhyAddr) )
    {
        *ppvSlidWin = NULL ;
        return CPSS_ERROR ;
    }

    pstLink = cpss_com_link_find_real(stPhyAddr) ;
    if(NULL == pstLink)
    {
        *ppvSlidWin = NULL ;
        return CPSS_ERROR ;
    }

    *ppvSlidWin = (VOID*)&(pstLink->stSlidWin) ;
#endif
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_link_data_store
* 功    能:
*
* 函数类型:
* 参    数:
* 参数名称   类型     输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明:
*******************************************************************************/
INT32 cpss_com_link_data_store
(
 CPSS_COM_STORE_NODE_T *pstStoreNode,
 CPSS_COM_TRUST_HEAD_T *pstTrustHdr,
 CPSS_COM_TRUST_HEAD_T *pstTrustTail
)
{
#ifndef CPSS_DSP_CPU
    UINT32 ulDataLen ;
    CPSS_COM_MSG_HEAD_T *pstDispHdr ;

    pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr);

    ulDataLen = pstDispHdr->ulLen ;

    /*判断发送的内存是否已经超出缓存的范围*/
    if((pstStoreNode->ulDataNum+1>CPSS_COM_SEND_BUF_NUM_MAX)||
        (pstStoreNode->ulDataSize+ulDataLen>CPSS_COM_SEND_BUF_LEN_MAX))
    {
        /*增加统计*/
        pstStoreNode->stStoreStat.ulBufOverFlowNum = pstStoreNode->stStoreStat.ulBufOverFlowNum + 1 ;

        return CPSS_ERR_COM_STORE_DATA_OVERFLOW ;
    }
    else
    {
        pstStoreNode->ulDataNum = pstStoreNode->ulDataNum + 1 ;
        pstStoreNode->ulDataSize = pstStoreNode->ulDataSize + ulDataLen ;

        /*增加统计*/
        pstStoreNode->stStoreStat.ulDataNum = pstStoreNode->stStoreStat.ulDataNum + 1 ;
        pstStoreNode->stStoreStat.ulDataSize =
            pstStoreNode->stStoreStat.ulDataSize + ulDataLen ;
    }

    /*将数据缓存起来*/
    if(NULL == pstStoreNode->apvNodeHdr)
    {
        pstStoreNode->apvNodeHdr = pstTrustHdr  ;
        pstStoreNode->apvNodeTail = pstTrustTail ;
    }
    else
    {
        /*将数据挂接到缓存链尾*/
        cpss_com_frag_slice_put(pstStoreNode->apvNodeTail,
            (VOID*)pstTrustHdr);
        pstStoreNode->apvNodeTail = (VOID*)pstTrustTail ;
    }
#endif
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_route_switch_stat
* 功    能:
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:
*******************************************************************************/
INT32 cpss_com_route_switch_stat(CPSS_COM_ROUTE_T* pstRoute,CPSS_COM_LOGIC_ADDR_T stLogAddr)
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_LINK_T *pstLink = NULL;
    UINT32 ulTemp =0;

    if(0 == CPSS_COM_PHY_ADDR_ZERO(pstRoute->stPhyAddr[0]))
    {
        pstLink = cpss_com_link_find_real(pstRoute->stPhyAddr[0]);
        if(NULL != pstLink)
        {
             pstLink->stLinkStat.ulPeerSwitchNum = pstLink->stLinkStat.ulPeerSwitchNum + 1;
        }
    }
    if(0 == CPSS_COM_PHY_ADDR_ZERO(pstRoute->stPhyAddr[1]))
    {
        pstLink = cpss_com_link_find_real(pstRoute->stPhyAddr[1]);
        if(NULL != pstLink)
        {
             pstLink->stLinkStat.ulPeerSwitchNum = pstLink->stLinkStat.ulPeerSwitchNum + 1;
        }
    }
    if(NULL != pstLink)
    {
        ulTemp = CPSS_MOD(pstLink->stLinkStat.ulPeerSwitchNum,10);
        switchinfo[ulTemp].ulDesPhyAddr = *(UINT32*)&pstRoute->stPhyAddr[1];
        switchinfo[ulTemp].ulLogAddr = *(UINT32*)&stLogAddr;
        switchinfo[ulTemp].ulSrcPhyAddr = *(UINT32*)&pstRoute->stPhyAddr[0];
    }
#endif
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_board_switch_deal
* 功    能:
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:
*******************************************************************************/
INT32 cpss_com_route_switch
(
    CPSS_COM_ROUTE_T* pstRoute,
    CPSS_COM_LOGIC_ADDR_T stLogAddr
)
{
#ifndef CPSS_DSP_CPU
    INT32 lRet ;
    CPSS_COM_PID_T stLogPid ;
    CPSS_COM_PHY_ADDR_T stPhyAddrTmp ;
    CPSS_COM_SLID_WIN_T*  pstSlidWin ;

    CPSS_STORE_DATA_STAT_T stStoreStatM ;
    CPSS_STORE_DATA_STAT_T stStoreStatS ;

    cpss_com_route_switch_stat(pstRoute,stLogAddr);

    if(cpss_vos_sem_p(g_ulSlidSem, WAIT_FOREVER) != CPSS_OK)
    {
        /*增加统计*/
        return CPSS_ERROR ;
    }

    /*切换路由*/
    stPhyAddrTmp = pstRoute->stPhyAddr[0] ;
    pstRoute->stPhyAddr[0] = pstRoute->stPhyAddr[1] ;
    pstRoute->stPhyAddr[1] = stPhyAddrTmp ;

    if(pstRoute->pstStoreMan == NULL)
    {
        cpss_vos_sem_v(g_ulSlidSem) ;
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,"route switch but not normal!logAddr=0x%x,phyAddr1=0x%x,phyAddr2=0x%x",
            *(UINT32*)&stLogAddr,*(UINT32*)&pstRoute->stPhyAddr[0],*(UINT32*)&pstRoute->stPhyAddr[1]) ;
        return CPSS_OK ;
    }

    cpss_mem_memcpy(&stStoreStatM,
        &pstRoute->pstStoreMan->stStoreNode[0].stStoreStat,sizeof(CPSS_STORE_DATA_STAT_T)) ;
    cpss_mem_memcpy(&stStoreStatS,
        &pstRoute->pstStoreMan->stStoreNode[1].stStoreStat,sizeof(CPSS_STORE_DATA_STAT_T)) ;

    /*增加统计*/
    stStoreStatM.ulSwitchNum = stStoreStatM.ulSwitchNum + 1 ;
    stStoreStatS.ulSwitchNum = stStoreStatS.ulSwitchNum + 1 ;
    stStoreStatM.ulDiscardNum += pstRoute->pstStoreMan->stStoreNode[0].stStoreStat.ulDataNum ;
    stStoreStatS.ulDiscardNum += pstRoute->pstStoreMan->stStoreNode[1].stStoreStat.ulDataNum ;

    /*删除所有的缓存的数据*/
    cpss_com_frag_pkg_free(pstRoute->pstStoreMan->stStoreNode[0].apvNodeHdr) ;
    cpss_com_frag_pkg_free(pstRoute->pstStoreMan->stStoreNode[1].apvNodeHdr) ;

    /*复位主用单板链路的滑窗*/
    stLogPid.stLogicAddr = stLogAddr ;
    stLogPid.ulAddrFlag  = CPSS_COM_ADDRFLAG_MASTER ;

    lRet = cpss_com_link_win_get_by_logaddr_real((VOID**)&pstSlidWin,&stLogPid) ;
    if(CPSS_OK == lRet)
    {
        cpss_com_data_store_timer_delete_real(&stLogPid);
        cpss_com_slid_win_reset(pstSlidWin);
    }

    /*复位备用单板链路的滑窗*/
    stLogPid.ulAddrFlag  = CPSS_COM_ADDRFLAG_SLAVE ;

    lRet = cpss_com_link_win_get_by_logaddr_real((VOID**)&pstSlidWin,&stLogPid) ;
    if(CPSS_OK == lRet)
    {
        cpss_com_data_store_timer_delete_real(&stLogPid);
        cpss_com_slid_win_reset(pstSlidWin);
    }

    /*清空内存*/
    cpss_mem_memset(pstRoute->pstStoreMan,0,sizeof(CPSS_COM_DATA_STORE_MAN_T)) ;

    /*拷贝统计数据到原来的结构中*/
    cpss_mem_memcpy(&pstRoute->pstStoreMan->stStoreNode[0].stStoreStat,
        &stStoreStatM,sizeof(CPSS_STORE_DATA_STAT_T)) ;
    cpss_mem_memcpy(&pstRoute->pstStoreMan->stStoreNode[1].stStoreStat,
        &stStoreStatS,sizeof(CPSS_STORE_DATA_STAT_T)) ;

    cpss_vos_sem_v(g_ulSlidSem) ;
#endif
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_board_switch_deal
* 功    能:
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:
*******************************************************************************/
INT32 cpss_com_board_switch_deal
(
    CPSS_COM_LOGIC_ADDR_T stLogAddr,
    CPSS_COM_PHY_ADDR_T   stPhyAddr,
    UINT32 ulAddrFlag
)
{
#ifndef CPSS_DSP_CPU
    CPSS_COM_ROUTE_T *pstRoute ;

    pstRoute = cpss_com_route_find_real(stLogAddr);
    if (NULL == pstRoute)
    {
        return (CPSS_ERR_COM_ROUTE_NOT_EXIST);
    }
    /*不需要进行路由切换*/
    if(1==CPSS_COM_PHY_ADDR_SAME(pstRoute->stPhyAddr[ulAddrFlag],stPhyAddr))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "route have switched no need to switch again!stLogAddr(%x)stPhyAddr(%x)ulAddrFlag(%d)",
            *(UINT32*)&stLogAddr,*(UINT32*)&stPhyAddr,ulAddrFlag) ;
            return CPSS_OK ;
    }
    /*需要切换路由*/
    else
    {
        /*切换路由*/
        cpss_com_route_switch(pstRoute,stLogAddr) ;

        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "address flag changed %d, logic addr 0x%08x,phy addr 0x%08x!",
            ulAddrFlag,*(UINT32*)&stLogAddr,*(UINT32*)&stPhyAddr);
    }
#endif
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_slave_logic_addr_get
* 功    能:
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:   HOST节点获得从CPU的逻辑地址，只能在HOST处理器上调用。目前只有TNSS使用。
*******************************************************************************/
INT32 cpss_com_slave_logic_addr_get
(
    CPSS_COM_LOGIC_ADDR_T *pstSlaveLogicAddr,
    UINT32                *pulAddrFlag
)
{
#ifndef CPSS_DSP_CPU
    if ((NULL == pstSlaveLogicAddr) || (NULL == pulAddrFlag))
    {

        return (CPSS_ERR_COM_PARA_ILLEGAL);
    }

    cpss_mem_memcpy((void*)pstSlaveLogicAddr, (void*)&g_pstComData->stAddrThis.stLogiAddr,
                    sizeof(CPSS_COM_LOGIC_ADDR_T));
    #ifndef CPSS_NPEA_ONLY_XSCALE
    pstSlaveLogicAddr->ucSubGroup = 2;
    #endif
    *pulAddrFlag = g_pstComData->stAddrThis.ulAddrFlag;
  #endif
    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_com_slave_logic_addr_get
* 功    能:
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:   HOST节点获得从CPU的逻辑地址，只能在HOST处理器上调用。目前只有TNSS使用。
*******************************************************************************/
VOID cpss_com_pci_link_show()
{
#ifdef CPSS_PCI_INCLUDE
    UINT32 ulIndex;
    UINT32 ulNum = 0;
    for(ulIndex = 0; ulIndex < CPSS_DRV_PCI_INFO_ARRAY_MAX; ulIndex++)
    {
        if(0 != CPSS_COM_PHY_ADDR_ZERO(g_stCpssComPciTbl.astPciLink[ulIndex].stDstPhyAddr))
        {
            ulNum = ulNum + 1;
            continue;
        }
        cps__oams_shcmd_printf("DstLogAddr:            0x%5x\n",*(UINT32*)&(g_stCpssComPciTbl.astPciLink[ulIndex].stDstLogAddr));
        cps__oams_shcmd_printf("DstPhyAddr:            0x%5x\n",*(UINT32*)&(g_stCpssComPciTbl.astPciLink[ulIndex].stDstPhyAddr));
        cps__oams_shcmd_printf("CpuNo:                 %5d\n",g_stCpssComPciTbl.astPciLink[ulIndex].ulCpuNo);
        cps__oams_shcmd_printf("SendDataDeliverErrNum: %5d\n",g_stCpssComPciTbl.astPciLink[ulIndex].stPciStat.ulDeliverErrNum);
        cps__oams_shcmd_printf("FaultNum:              %5d\n",g_stCpssComPciTbl.astPciLink[ulIndex].stPciStat.ulFaultNum);
        cps__oams_shcmd_printf("LinkSetupNum:          %5d\n",g_stCpssComPciTbl.astPciLink[ulIndex].stPciStat.ulLinkSetupNum);
        cps__oams_shcmd_printf("RecvDataNum:           %5d\n",g_stCpssComPciTbl.astPciLink[ulIndex].stPciStat.ulRecvDataNum);
        cps__oams_shcmd_printf("SendDataNum:           %5d\n",g_stCpssComPciTbl.astPciLink[ulIndex].stPciStat.ulSendDataNum);
        cps__oams_shcmd_printf("RecvDataDeliverErrNum: %5d\n",g_stCpssComPciTbl.astPciLink[ulIndex].stPciStat.ulRecvDataDeliverErrNum);
    }
    if(ulNum == CPSS_DRV_PCI_INFO_ARRAY_MAX)
    {
        cps__oams_shcmd_printf("Now it has not pci link\n");
        }
#else
        cps__oams_shcmd_printf("Now it has not pci link\n");
#endif
}

/*******************************************************************************
* 函数名称: cpss_com_time_print
* 功    能: 显示CPSS的时间
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:
*******************************************************************************/
VOID cpss_com_time_print
(
 UINT8* pucInfo,
 CPSS_TIME_T* pstTime
)
{
#ifndef CPSS_DSP_CPU
    cps__oams_shcmd_printf("%s=%d/%d/%d %d:%d:%d\n",pucInfo,pstTime->usYear,
        pstTime->ucMonth,pstTime->ucDay,pstTime->ucHour,
        pstTime->ucMinute,pstTime->ucSecond) ;
#endif
}


/*******************************************************************************
* 函数名称: cpss_com_init_show
* 功    能: 显示CPSS的初始化流程
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:
*******************************************************************************/
#ifndef CPSS_DSP_CPU
VOID cpss_com_init_show()
{
    cps__oams_shcmd_printf(">>cpss com int procedure stat begin<<\n") ;

    cps__oams_shcmd_printf("g_stCpssComInitStat.ulRecvInitMsgFlag=%d\n",
        g_stCpssComInitStat.ulRecvInitMsgFlag) ;
    /*addr init 部分*/
    cps__oams_shcmd_printf("-------------------------------------------------\n");
    cps__oams_shcmd_printf("g_stCpssComInitStat.stAddrInitInfo.ulAddrInitFlag=%d\n",
        g_stCpssComInitStat.stAddrInitInfo.ulAddrInitFlag) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stAddrInitInfo.stThisPhyAddr=0x%x\n",
        *(UINT32*)&g_stCpssComInitStat.stAddrInitInfo.stThisPhyAddr) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stAddrInitInfo.stThisLogAddr=0x%x\n",
        *(UINT32*)&g_stCpssComInitStat.stAddrInitInfo.stThisLogAddr) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stAddrInitInfo.stGcpaPhyAddrM=0x%x\n",
        *(UINT32*)&g_stCpssComInitStat.stAddrInitInfo.stGcpaPhyAddrM) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stAddrInitInfo.stGcpaPhyAddrS=0x%x\n",
        *(UINT32*)&g_stCpssComInitStat.stAddrInitInfo.stGcpaPhyAddrS) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stAddrInitInfo.stGcpaLogAddr=0x%x\n",
        *(UINT32*)&g_stCpssComInitStat.stAddrInitInfo.stGcpaLogAddr) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stAddrInitInfo.ulAddrInitFlag=0x%x\n",
        g_stCpssComInitStat.stAddrInitInfo.ulAddrInitFlag) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stAddrInitInfo.ulThisIp=0x%x\n",
        g_stCpssComInitStat.stAddrInitInfo.ulThisIp) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stAddrInitInfo.ulGcpaIpM=0x%x\n",
        g_stCpssComInitStat.stAddrInitInfo.ulGcpaIpM) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stAddrInitInfo.ulGcpaIpS=0x%x\n",
        g_stCpssComInitStat.stAddrInitInfo.ulGcpaIpS) ;
    cpss_com_time_print("g_stCpssComInitStat.stAddrInitInfo.stTime=",
        &g_stCpssComInitStat.stAddrInitInfo.stTime) ;

    /*drv init 部分*/
    cps__oams_shcmd_printf("-------------------------------------------------\n");
    cps__oams_shcmd_printf("g_stCpssComInitStat.stDrvInitInfo.ulDrvInitFlag=%d\n",
        g_stCpssComInitStat.stDrvInitInfo.ulDrvInitFlag) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stDrvInitInfo.ulUdpIp=0x%x\n",
        g_stCpssComInitStat.stDrvInitInfo.ulUdpIp) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stDrvInitInfo.ulUdpPort=%d\n",
        g_stCpssComInitStat.stDrvInitInfo.ulUdpPort) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stDrvInitInfo.ulUdpBindStat=%d\n",
        g_stCpssComInitStat.stDrvInitInfo.ulUdpBindStat) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stDrvInitInfo.ulTcpIp=0x%x\n",
        g_stCpssComInitStat.stDrvInitInfo.ulTcpIp) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stDrvInitInfo.ulTcpPort=%d\n",
        g_stCpssComInitStat.stDrvInitInfo.ulTcpPort) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stDrvInitInfo.ulTcpBindStat=%d\n",
        g_stCpssComInitStat.stDrvInitInfo.ulTcpBindStat) ;

    cps__oams_shcmd_printf("g_stCpssComInitStat.stDrvInitInfo.ulDcIp=0x%x\n",
        g_stCpssComInitStat.stDrvInitInfo.ulDcIp) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stDrvInitInfo.ulDcPort=%d\n",
        g_stCpssComInitStat.stDrvInitInfo.ulDcPort) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stDrvInitInfo.ulDcBindStat=%d\n",
        g_stCpssComInitStat.stDrvInitInfo.ulDcBindStat) ;

    cps__oams_shcmd_printf("g_stCpssComInitStat.stDrvInitInfo.ulPciInitFlag=%d\n",
        g_stCpssComInitStat.stDrvInitInfo.ulPciInitFlag) ;
    cpss_com_time_print("g_stCpssComInitStat.stDrvInitInfo.stTime=",
        &g_stCpssComInitStat.stDrvInitInfo.stTime) ;

    /*建立上行去GCPA链路标志*/
    cps__oams_shcmd_printf("-------------------------------------------------\n");
    cps__oams_shcmd_printf("g_stCpssComInitStat.ulGcpaLinkSetup=%d\n",g_stCpssComInitStat.ulGcpaLinkSetup) ;

    /*rarp请求info*/
    cps__oams_shcmd_printf("-------------------------------------------------\n");
    cps__oams_shcmd_printf("g_stCpssComInitStat.stRarpReqInfo.ulRarpReqFlag=%d\n",
        *(UINT32*)&g_stCpssComInitStat.stRarpReqInfo.ulRarpReqFlag) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stRarpReqInfo.stRaraPhyAddr=0x%x\n",
        *(UINT32*)&g_stCpssComInitStat.stRarpReqInfo.stRarpPhyAddr) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stRarpReqInfo.ulRarpReqNum=%d\n",
        *(UINT32*)&g_stCpssComInitStat.stRarpReqInfo.ulRarpReqNum) ;
    cpss_com_time_print("g_stCpssComInitStat.stRarpReqInfo.stTime=",
        &g_stCpssComInitStat.stRarpReqInfo.stTime) ;
    /*rarp响应info*/
    cps__oams_shcmd_printf("-------------------------------------------------\n");
    cps__oams_shcmd_printf("g_stCpssComInitStat.stRarpRspInfo.ulRarpRspFlag=%d\n",
        g_stCpssComInitStat.stRarpRspInfo.ulRarpRspFlag) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stRarpRspInfo.ulRarpRspResult=%d\n",
        g_stCpssComInitStat.stRarpRspInfo.ulRarpRspResult) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stRarpRspInfo.ulRarpRspRecvNum=%d\n",
        g_stCpssComInitStat.stRarpRspInfo.ulRarpRspRecvNum) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stRarpRspInfo.ulMateFlag=%d\n",
        g_stCpssComInitStat.stRarpRspInfo.ulMateFlag) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stRarpRspInfo.stRarpLogAddr=0x%x\n",
        *(UINT32*)&g_stCpssComInitStat.stRarpRspInfo.stRarpLogAddr) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stRarpRspInfo.stMatePhyAddr=0x%x\n",
        *(UINT32*)&g_stCpssComInitStat.stRarpRspInfo.stMatePhyAddr) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stRarpRspInfo.stGcpaPhyAddrM=0x%x\n",
        *(UINT32*)&g_stCpssComInitStat.stRarpRspInfo.stGcpaPhyAddrM) ;
    cpss_com_time_print("g_stCpssComInitStat.stRarpRspInfo.stTime=",
        &g_stCpssComInitStat.stRarpRspInfo.stTime) ;
    /*poweron消息info*/
    cps__oams_shcmd_printf("-------------------------------------------------\n");
    cps__oams_shcmd_printf("g_stCpssComInitStat.stPowerOnInfo.ulPowerOnFlag=%d\n",
        g_stCpssComInitStat.stPowerOnInfo.ulPowerOnFlag) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stPowerOnInfo.stPowerOnPhyAddr=0x%x\n",
        *(UINT32*)&g_stCpssComInitStat.stPowerOnInfo.stPowerOnPhyAddr) ;
    cpss_com_time_print("g_stCpssComInitStat.stPowerOnInfo.stTime=",
        &g_stCpssComInitStat.stPowerOnInfo.stTime) ;
    /*smss激活请求消息info*/
    cps__oams_shcmd_printf("-------------------------------------------------\n");
    cps__oams_shcmd_printf("g_stCpssComInitStat.stSmssActInfo.ulSmssActFlag=%d\n",
        g_stCpssComInitStat.stSmssActInfo.ulSmssActFlag) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stSmssActInfo.ulAddrFlag=%d\n",
        g_stCpssComInitStat.stSmssActInfo.ulAddrFlag) ;
    cpss_com_time_print("g_stCpssComInitStat.stSmssActInfo.stTime=",
        &g_stCpssComInitStat.stSmssActInfo.stTime) ;
    /*smss激活请求消息info*/
    cps__oams_shcmd_printf("-------------------------------------------------\n");
    cps__oams_shcmd_printf("g_stCpssComInitStat.stSmssActRspInfo.ulSmssActRspFlag=%d\n",
        g_stCpssComInitStat.stSmssActRspInfo.ulSmssActRspFlag) ;
    cps__oams_shcmd_printf("g_stCpssComInitStat.stSmssActRspInfo.ulSmssActRspFlag=%d\n",
        g_stCpssComInitStat.stSmssActRspInfo.ulSmssActRspResult) ;
    cpss_com_time_print("g_stCpssComInitStat.stSmssActRspInfo.stTime=",
        &g_stCpssComInitStat.stSmssActRspInfo.stTime) ;

    cps__oams_shcmd_printf(">>cpss com int procedure stat end<<\n") ;
    cps__oams_shcmd_printf("\n") ;

}
#else
void cpss_com_init_show ()
{
    cps__oams_shcmd_printf("it has not init show func!\n") ;
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_arp_show
* 功    能: 显示arp统计
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:
*******************************************************************************/
VOID cpss_com_arp_item_add
(
 CPSS_COM_PHY_ADDR_T *pstPhyAddr,
 CPSS_COM_LOGIC_ADDR_T* pstLogAddr,
 UINT32 ulAddrFlag,
 UINT32 ulAddFlag
 )
{
#ifndef CPSS_DSP_CPU
    UINT32 ulItem ;

    if(CPSS_COM_ARP_STAT_REQ_ADD == ulAddFlag)
    {
        ulItem = g_stCpssComArpStat.ulCurrLogAddr ;

        g_stCpssComArpStat.ulArpSendNum = g_stCpssComArpStat.ulArpSendNum + 1 ;
        g_stCpssComArpStat.astArpLogAddr[ulItem] = *pstLogAddr;
        g_stCpssComArpStat.aulAddrFlag[ulItem] = ulAddrFlag ;
        g_stCpssComArpStat.ulCurrLogAddr = CPSS_MOD(ulItem,CPSS_COM_ARP_ITEM_NUM_MAX) ;
        ulItem = ulItem + 1;
    }
    else
    {
        ulItem = g_stCpssComArpStat.ulCurrResolveItem ;

        g_stCpssComArpStat.astArpResolveItems[ulItem].stLogAddr = *pstLogAddr ;
        g_stCpssComArpStat.astArpResolveItems[ulItem].stPhyAddr = *pstPhyAddr ;
        g_stCpssComArpStat.astArpResolveItems[ulItem].ulAddrFlag= ulAddrFlag ;
        g_stCpssComArpStat.ulCurrResolveItem = CPSS_MOD(ulItem,CPSS_COM_ARP_ITEM_NUM_MAX) ;
        ulItem = ulItem + 1;
    }
#endif
}

/*******************************************************************************
* 函数名称: cpss_com_arp_show
* 功    能: 显示arp统计
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:
*******************************************************************************/
#ifndef CPSS_DSP_CPU
VOID cpss_com_arp_show()
{
    UINT32 ulLoop ;

    cps__oams_shcmd_printf(">> cpss_arp_show begin<<\n") ;

    cps__oams_shcmd_printf("g_stCpssComArpStat.ulArpSendNum=%d\n",
        g_stCpssComArpStat.ulArpSendNum) ;
    cps__oams_shcmd_printf("g_stCpssComArpStat.ulCurrLogAddr=%d\n",
        g_stCpssComArpStat.ulCurrLogAddr) ;
    cps__oams_shcmd_printf("g_stCpssComArpStat.ulCurrResolveItem=%d\n",
        g_stCpssComArpStat.ulCurrResolveItem) ;

    for(ulLoop=0 ;ulLoop<CPSS_COM_ARP_ITEM_NUM_MAX ;ulLoop++)
    {
        cps__oams_shcmd_printf("g_stCpssComArpStat.astArpLogAddr[%d]=0x%x\n",ulLoop,
            *(UINT32*)&g_stCpssComArpStat.astArpLogAddr[ulLoop]) ;
        cps__oams_shcmd_printf("g_stCpssComArpStat.aulAddrFlag[%d]=0x%x\n",ulLoop,
            g_stCpssComArpStat.aulAddrFlag[ulLoop]) ;
        cps__oams_shcmd_printf("g_stCpssComArpStat.astArpResolveItems[%d].stLogAddr=0x%x\n",ulLoop,
            *(UINT32*)&g_stCpssComArpStat.astArpResolveItems[ulLoop].stLogAddr) ;
        cps__oams_shcmd_printf("g_stCpssComArpStat.astArpResolveItems[%d].stPhyAddr=0x%x\n",ulLoop,
            *(UINT32*)&g_stCpssComArpStat.astArpResolveItems[ulLoop].stPhyAddr) ;
        cps__oams_shcmd_printf("g_stCpssComArpStat.astArpResolveItems[%d].ulAddrFlag=0x%x\n",ulLoop,
            g_stCpssComArpStat.astArpResolveItems[ulLoop].ulAddrFlag) ;
    }
    cps__oams_shcmd_printf(">> cpss_arp_show end<<\n") ;
}
#else
VOID cpss_com_arp_show()
{
    cps__oams_shcmd_printf(">> cpss_arp_show not surrport !<<\n") ;
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_heartbeat_show
* 功    能: 显示心跳链表中的链路信息
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* 返回值:  无
* 说   明:
*******************************************************************************/
VOID cpss_com_heartbeat_show()
{
#ifndef CPSS_DSP_CPU
    UINT32 ulHeartBeatNum = 0 ;
    CPSS_COM_HEARTBEAT_NODE_T* pstNode ;

    cps__oams_shcmd_printf("g_stCpssHeartBeatStat.ulHeartBeatTimerFlag = %d\n",
        g_stCpssHeartBeatStat.ulHeartBeatTimerFlag) ;
    cps__oams_shcmd_printf("g_stCpssHeartBeatStat.ulHeartBeatTimerNum = %d\n",
        g_stCpssHeartBeatStat.ulHeartBeatTimerNum) ;

    pstNode = g_pstCpssHeartBeatLink ;

    while(pstNode != NULL)
    {
        cps__oams_shcmd_printf("-----------heart beat %d---------\n",ulHeartBeatNum) ;
        cps__oams_shcmd_printf("stDstPhyAddr = 0x%x\n",*(UINT32*)&pstNode->stDstPhyAddr) ;
        cps__oams_shcmd_printf("usLinkState = %d\n",pstNode->usLinkState) ;
        pstNode = pstNode->pstNextNode ;
        ulHeartBeatNum = ulHeartBeatNum + 1 ;
    }
#endif
}

#ifndef CPSS_DSP_CPU
/*****************************
可靠传输测试纤程
******************************/
typedef struct slidtest
{
    UINT32 timeInterval ;
    UINT32 pkgLen ;
}SLID_TEST_T ;

/*PCI的中断notify定时器消息*/
#define CPSS_COM_SLID_TEST_TM_MSG         CPSS_TIMER_07_MSG
/*PCI的notify定时器号*/
#define CPSS_COM_SLID_TEST_TM_NO          (CPSS_COM_SLID_TEST_TM_MSG&0xFF)



#define SLID_TEST_OPEN_SEND_TIMER  1234567
#define SLID_TEST_CLOSE_SEND_TIMER 345676

#define SLID_TEST_MSG               55555555
#define SLID_TEST_RSP_MSG           66666666



UINT8* g_pucSendBuf ;
UINT32 g_ulSendLen ;

INT32 cpss_com_slid_test
(
UINT16  usUserState,
VOID *  pvVar,
CPSS_COM_MSG_HEAD_T *pstMsgHead
)
{
    SLID_TEST_T* pstSlidTest ;
    CPSS_COM_MSG_HEAD_T stComHdr ;

    pstSlidTest = (SLID_TEST_T*)(pstMsgHead->pucBuf);

    switch(pstMsgHead->ulMsgId)
    {
    case SLID_TEST_OPEN_SEND_TIMER:
        {

            g_pucSendBuf = cpss_mem_malloc(pstSlidTest->pkgLen);
            if(NULL == g_pucSendBuf)
            {
                printf("------------------slid test malloc failed!\n") ;
                return CPSS_ERROR ;
            }

            if(CPSS_OK != cpss_timer_loop_set(CPSS_COM_SLID_TEST_TM_NO,pstSlidTest->timeInterval))
            {
                printf("------------------slid test timer set failed!\n") ;
                return CPSS_ERROR ;
            }

            g_ulSendLen = pstSlidTest->pkgLen ;

            cpss_mem_memset(g_pucSendBuf,'h',g_ulSendLen) ;

            printf("-----------recv SLID_TEST_OPEN_SEND_TIMER msg\n");
            break ;
        }
    case CPSS_COM_SLID_TEST_TM_MSG:
        {
            stComHdr.stDstProc.stLogicAddr.ucModule = 0x1 ;
            stComHdr.stDstProc.stLogicAddr.ucSubGroup = 0x1 ;
            stComHdr.stDstProc.stLogicAddr.usGroup    = 0x442 ;

            stComHdr.stDstProc.ulAddrFlag = 0 ;
            stComHdr.stDstProc.ulPd = 0x90000 ;

            stComHdr.pucBuf = g_pucSendBuf ;
            stComHdr.ucAckFlag = CPSS_COM_ACK ;
            stComHdr.ucPriFlag = CPSS_COM_PRIORITY_NORMAL ;
            stComHdr.ucShareFlag = 0 ;
            stComHdr.ulLen  = g_ulSendLen ;
            stComHdr.ulMsgId = SLID_TEST_MSG ;

            stComHdr.stSrcProc.stLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
            stComHdr.stSrcProc.ulAddrFlag  = g_pstComData->stAddrThis.ulAddrFlag;
            stComHdr.stSrcProc.ulPd        = cpss_vk_pd_self_get();

            if(CPSS_OK != cpss_com_send_extend(&stComHdr))
            {
                printf("------------------slid test send data failed!\n");
                return CPSS_ERROR ;
            }
            break ;
        }
    case SLID_TEST_MSG:
        {
            printf("--------------recv SLID_TEST_MSG msg!\n") ;
            stComHdr.stDstProc.stLogicAddr.ucModule = 0x1 ;
            stComHdr.stDstProc.stLogicAddr.ucSubGroup = 0x1 ;
            stComHdr.stDstProc.stLogicAddr.usGroup    = 0x441 ;

            stComHdr.stDstProc.ulAddrFlag = 0 ;
            stComHdr.stDstProc.ulPd = 0x90000 ;

            stComHdr.pucBuf = NULL ;
            stComHdr.ucAckFlag = CPSS_COM_ACK ;
            stComHdr.ucPriFlag = CPSS_COM_PRIORITY_NORMAL ;
            stComHdr.ucShareFlag = 0 ;
            stComHdr.ulLen  = 0 ;
            stComHdr.ulMsgId = SLID_TEST_RSP_MSG ;

            stComHdr.stSrcProc.stLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
            stComHdr.stSrcProc.ulAddrFlag  = g_pstComData->stAddrThis.ulAddrFlag;
            stComHdr.stSrcProc.ulPd        = cpss_vk_pd_self_get();

            if(CPSS_OK != cpss_com_send_extend(&stComHdr))
            {
                printf("------------------slid test send data failed!\n");
                return CPSS_ERROR ;
            }

            break ;
        }
    case SLID_TEST_RSP_MSG:
        {
            printf("--------------recv slid_test_rsp_msg\n") ;
            break ;
        }
    case SLID_TEST_CLOSE_SEND_TIMER:
        {
            cpss_timer_delete(CPSS_COM_SLID_TEST_TM_NO) ;
            cpss_mem_free(g_pucSendBuf);
            g_pucSendBuf = NULL ;
            g_ulSendLen = 0 ;
            printf("---------------recv SLID_TEST_CLOSE_SEND_TIMER ok!\n");
            break ;
        }
    default:
        {
            printf("----------------recv unknown msg msgid=%d\n",pstMsgHead->ulMsgId);
            break ;
        }
    }
    return CPSS_OK ;

}


INT32 slid_test_send_cmd
(
 UINT32 ulTimerInter,
 UINT32 ulSendLen
)
{
    SLID_TEST_T stSlidTest ;
    CPSS_COM_MSG_HEAD_T stComHdr ;

    stSlidTest.pkgLen = ulSendLen ;
    stSlidTest.timeInterval = ulTimerInter ;

    stComHdr.pucBuf = (UINT8*)&stSlidTest ;
    cpss_mem_memset((UINT8*)&(stComHdr.stDstProc.stLogicAddr),0,sizeof(CPSS_COM_LOGIC_ADDR_T)) ;

    stComHdr.stDstProc.ulPd = 0x90000 ;
    stComHdr.ucAckFlag = CPSS_COM_ACK ;
    stComHdr.ucPriFlag = CPSS_COM_PRIORITY_NORMAL ;
    stComHdr.ucShareFlag = 0 ;

    stComHdr.ulLen = sizeof(SLID_TEST_T);
    stComHdr.ulMsgId = SLID_TEST_OPEN_SEND_TIMER ;

    stComHdr.stSrcProc.stLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
    stComHdr.stSrcProc.ulAddrFlag  = g_pstComData->stAddrThis.ulAddrFlag;
    stComHdr.stSrcProc.ulPd        = cpss_vk_pd_self_get();

    if(CPSS_OK != cpss_com_send_extend(&stComHdr))
    {
        printf("---------send test send cmd failed!\n") ;
        return CPSS_ERROR ;
    }

    printf("---------send test send cmd succ!\n") ;
    return CPSS_OK ;
}


INT32 slid_test_stop_send_cmd()
{
    CPSS_COM_MSG_HEAD_T stComHdr ;


    stComHdr.pucBuf = NULL ;

    cpss_mem_memset((UINT8*)&(stComHdr.stDstProc.stLogicAddr),0,sizeof(CPSS_COM_LOGIC_ADDR_T)) ;

    stComHdr.stDstProc.ulPd = 0x90000 ;
    stComHdr.ucAckFlag = CPSS_COM_ACK ;
    stComHdr.ucPriFlag = CPSS_COM_PRIORITY_NORMAL ;
    stComHdr.ucShareFlag = 0 ;

    stComHdr.ulLen = 0;
    stComHdr.ulMsgId = SLID_TEST_CLOSE_SEND_TIMER ;

    stComHdr.stSrcProc.stLogicAddr = g_pstComData->stAddrThis.stLogiAddr;
    stComHdr.stSrcProc.ulAddrFlag  = g_pstComData->stAddrThis.ulAddrFlag;
    stComHdr.stSrcProc.ulPd        = cpss_vk_pd_self_get();

    if(CPSS_OK != cpss_com_send_extend(&stComHdr))
    {
        printf("---------send test send cmd failed!\n") ;
        return CPSS_ERROR ;
    }

    printf("---------send test send cmd succ!\n") ;
    return CPSS_OK ;
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_get_self_ip_addr(UINT32 *pulIpAddr)
* 功    能: 得到本板的网络字节序的IP地址
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:
*******************************************************************************/
INT32 cpss_com_get_self_ip_addr
(
UINT32 *pulIpAddr
)
{
    INT32 lRet ;
    CPSS_COM_PHY_ADDR_T stPhyAddrSelf = {0} ;

    /*得到本板的物理地址*/
    lRet = cpss_com_phy_addr_get(&stPhyAddrSelf) ;
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }

    /*得到本机字节序的IP地址*/
    cpss_com_phy2ip(stPhyAddrSelf,pulIpAddr, FALSE) ;

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_get_master_mc_ip_addr(UINT32 *pulIpAddr)
* 功    能: 得到主用主控板的网络字节序的IP地址
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:
*******************************************************************************/
INT32 cpss_com_get_master_mc_ip_addr
(
UINT32 *pulIpAddr
)
{
    CPSS_COM_PHY_ADDR_T stPhyAddrMGcpa ;

    /*得到主用GCPA物理地址(cpss维护此地址)*/
    stPhyAddrMGcpa = g_pstComData->stAddrGcpaM.stPhyAddr ;

    /*得到本机字节序的IP地址*/
    cpss_com_phy2ip(stPhyAddrMGcpa,pulIpAddr, FALSE) ;

    return CPSS_OK ;
}


INT32 cpss_com_get_mate_switch_ip_addr(UINT32 *pIpAddr)
{
#ifdef CPSS_VOS_VXWORKS
   CPSS_COM_PHY_ADDR_T tPhyAddrMate;

    if(pIpAddr ==NULL)  return CPSS_ERROR;

    if(cpss_com_phy_addr_mate_get(&tPhyAddrMate) == CPSS_ERROR)
    	{
    	   return CPSS_ERROR;
    	}

     if(cpss_com_phy2ip(tPhyAddrMate, pIpAddr, TRUE) != CPSS_OK)
     	{
     	   return CPSS_ERROR;
     	}
#endif
	return CPSS_OK;


}


 BOOL  cpss_com_direct_get_time_from_gcpa(VOID)
{

/* 如果是备用交换板,则从主用交换板获取时间信息*/
#ifdef CPSS_SWITCH_BRDTYPE

   return ( g_pstComData->stAddrThis.ulAddrFlag == CPSS_COM_ADDRFLAG_MASTER);
#else

  return TRUE;
#endif
}
/*******************************************************************************
* 函数名称: cpss_com_data_show
* 功    能: 通信全局变量的show函数
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:
*******************************************************************************/
VOID cpss_com_data_show()
{
    cps__oams_shcmd_printf("GcpaMlogiAddr: 0x%08x,GcpaMphyAddr: 0x%08x,GcpaMbackupType: %d,GcpaMaddrFlag: %d,GcpaMipAddr: 0x%08x\n",
        *(UINT32*)&g_pstComData->stAddrGcpaM.stLogiAddr,*(UINT32*)&g_pstComData->stAddrGcpaM.stPhyAddr,g_pstComData->stAddrGcpaM.ucBackupType,
        g_pstComData->stAddrGcpaM.ulAddrFlag,g_pstComData->stAddrGcpaM.ulIpAddr);
    cps__oams_shcmd_printf("GcpaSlogiAddr: 0x%08x,GcpaSphyAddr: 0x%08x,GcpaSbackupType: %d,GcpaSaddrFlag: %d,GcpaSipAddr: 0x%08x\n",
        *(UINT32*)&g_pstComData->stAddrGcpaS.stLogiAddr,*(UINT32*)&g_pstComData->stAddrGcpaS.stPhyAddr,g_pstComData->stAddrGcpaS.ucBackupType,
        g_pstComData->stAddrGcpaS.ulAddrFlag,g_pstComData->stAddrGcpaS.ulIpAddr);
    cps__oams_shcmd_printf("MCuplogiAddr: 0x%08x,MCupphyAddr: 0x%08x,MCupbackupType: %d,MCupaddrFlag: %d,MCupipAddr: 0x%08x\n",
        *(UINT32*)&g_pstComData->stAddrMCup.stLogiAddr,*(UINT32*)&g_pstComData->stAddrMCup.stPhyAddr,g_pstComData->stAddrMCup.ucBackupType,
        g_pstComData->stAddrMCup.ulAddrFlag,g_pstComData->stAddrMCup.ulIpAddr);
    cps__oams_shcmd_printf("ThislogiAddr: 0x%08x,ThisphyAddr: 0x%08x,ThisbackupType: %d,ThisaddrFlag: %d,ThisipAddr: 0x%08x\n",
        *(UINT32*)&g_pstComData->stAddrThis.stLogiAddr,*(UINT32*)&g_pstComData->stAddrThis.stPhyAddr,g_pstComData->stAddrThis.ucBackupType,
        g_pstComData->stAddrThis.ulAddrFlag,g_pstComData->stAddrThis.ulIpAddr);
    cps__oams_shcmd_printf("MatePhyAddr: 0x%08x,MateReqTNo: %d,RarpReqTNo: %d,ulSeqID: %d\n",
        *(UINT32*)&g_pstComData->stMatePhyAddr,g_pstComData->ulMateReqTNo,g_pstComData->ulRarpReqTNo,g_pstComData->ulSeqID);
}

/*******************************************************************************
* 函数名称: cpss_com_constant_addr_show
* 功    能: 通信全局变量地址的show函数
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:
*******************************************************************************/
VOID cpss_com_constant_addr_show()
{
    cps__oams_shcmd_printf("g_stLogicAddrGcpa:     {ucModule:%d,ucSubGroup:%d,ucGroup:0x%x}\n",
        g_stLogicAddrGcpa.ucModule,g_stLogicAddrGcpa.ucSubGroup,g_stLogicAddrGcpa.usGroup);
    cps__oams_shcmd_printf("g_stLogicAddrOmcSvr:   {ucModule:%d,ucSubGroup:%d,ucGroup:%d}\n",
        g_stLogicAddrOmcSvr.ucModule,g_stLogicAddrOmcSvr.ucSubGroup,g_stLogicAddrOmcSvr.usGroup);
    cps__oams_shcmd_printf("g_stLogicAddrOmcPmSvr: {ucModule:%d,ucSubGroup:%d,ucGroup:%d}\n",
        g_stLogicAddrOmcPmSvr.ucModule,g_stLogicAddrOmcPmSvr.ucSubGroup,g_stLogicAddrOmcPmSvr.usGroup);
    cps__oams_shcmd_printf("g_stLogicAddrHlrSvr:   {ucModule:%d,ucSubGroup:%d,ucGroup:%d}\n",
        g_stLogicAddrHlrSvr.ucModule,g_stLogicAddrHlrSvr.ucSubGroup,g_stLogicAddrHlrSvr.usGroup);
    cps__oams_shcmd_printf("g_stPhyAddrGcpaMaster: {ucFrame:%d,ucShelf:%d,ucSlot:%d,ucCpu:%d}\n",
        g_stPhyAddrGcpaMaster.ucFrame,g_stPhyAddrGcpaMaster.ucShelf,g_stPhyAddrGcpaMaster.ucSlot,g_stPhyAddrGcpaMaster.ucCpu);
    cps__oams_shcmd_printf("g_stPhyAddrGcpaSlave:  {ucFrame:%d,ucShelf:%d,ucSlot:%d,ucCpu:%d}\n",
        g_stPhyAddrGcpaSlave.ucFrame,g_stPhyAddrGcpaSlave.ucShelf,g_stPhyAddrGcpaSlave.ucSlot,g_stPhyAddrGcpaSlave.ucCpu);
}

/*******************************************************************************
* 函数名称: cpss_com_call_cps__rdbs_err_show
* 功    能: 调用RDBS接口函数的信息统计
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:
*******************************************************************************/
#ifdef CPSS_FUNBRD_MC
VOID cpss_com_call_cps__rdbs_err_show()
{
    UINT32 ulRarpIndex = 0;
    UINT32 ulArpIndex = 0;
    cps__oams_shcmd_printf("call cps__rdbs_if_entry failed in rarp static: %d\n",g_ulRarpNum);
    for(ulRarpIndex=0; ulRarpIndex < g_ulRarpNum;ulRarpIndex++)
    {
        cps__oams_shcmd_printf("Record[%d] = 0x%08x\n",ulRarpIndex,RdbsRarpLinkInfo[ulRarpIndex]);
    }
    cps__oams_shcmd_printf("call cps__rdbs_if_entry failed in arp static:  %d\n",g_ulArpNum);
    for(ulArpIndex=0; ulArpIndex < g_ulArpNum;ulArpIndex++)
    {
        cps__oams_shcmd_printf("Record[%d] = 0x%08x\n",ulArpIndex,RdbsArpLinkInfo[ulArpIndex]);
    }
}
#else
VOID cpss_com_call_cps__rdbs_err_show()
{
    cps__oams_shcmd_printf("rdbs err show can not support!!\n");
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_switch_info_show
* 功    能: 切换信息的show函数
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*          无
* 说   明:
*******************************************************************************/
VOID cpss_com_switch_info_show()
{
#ifndef CPSS_DSP_CPU
    UINT32 ulIndex =0;
    cps__oams_shcmd_printf("SWITCH INFO:\n");
    for(ulIndex=0; ulIndex< CPSS_COM_SWITCH_RECORD_NUM; ulIndex++)
    {
        cps__oams_shcmd_printf("record[%d]: logAddr:0x%08x,srcPhyAddr:0x%08x,DesPhyAddr:0x%08x\n",
            ulIndex,switchinfo[ulIndex].ulLogAddr,switchinfo[ulIndex].ulSrcPhyAddr,switchinfo[ulIndex].ulDesPhyAddr);
    }
#endif
}

#ifdef CPSS_VOS_WINDOWS
#ifdef CPSS_PCI_SIM_BY_UDP
void cpss_com_drv_pci_recv_task()
{
    ;
}

INT32 cpss_com_link_slave_cpu_reset
(
 UINT32 ulCpuNo
 )
{
    return CPSS_OK ;
}

INT32 cpss_com_pci_task_reg()
{
    return CPSS_OK;
}

#endif
#endif
#ifdef CPSS_PCI_INCLUDE
extern CPSS_COM_PCI_LINK_TABLE_T g_stPciLinkTable;
/*******************************************************************************
* 函数名称: cpss_pci_drv_info_show()
* 功    能: 打印PCI驱动信息
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:
*******************************************************************************/
VOID cpss_pci_drv_info_show()
{
    UINT32 ulCpuNum ;
    UINT32 ulLoop ;

    ulCpuNum = g_stPciLinkTable.ulSlaveCpuNum ;

    cps__oams_shcmd_printf(">>cpss pci driver info show begin<<\n") ;

    for(ulLoop=0 ;ulLoop<ulCpuNum ;ulLoop++)
    {
        cps__oams_shcmd_printf("g_stPciLinkTable.aulCpuNo[%d]=%d\n",
            ulLoop,g_stPciLinkTable.aulCpuNo[ulLoop]) ;

        cps__oams_shcmd_printf("g_stPciLinkTable.astPciLink[%d].ulRegBufStart=0x%x\n",
            ulLoop,g_stPciLinkTable.astPciLink[ulLoop].ulRegBufStart) ;
        cps__oams_shcmd_printf("g_stPciLinkTable.astPciLink[%d].ulRegBufSize=0x%x\n",
            ulLoop,g_stPciLinkTable.astPciLink[ulLoop].ulRegBufSize) ;
        cps__oams_shcmd_printf("g_stPciLinkTable.astPciLink[%d].ulRxEbdStart=0x%x\n",
            ulLoop,g_stPciLinkTable.astPciLink[ulLoop].ulRxEbdStart) ;
        cps__oams_shcmd_printf("g_stPciLinkTable.astPciLink[%d].ulTxEbdStart=0x%x\n",
            ulLoop,g_stPciLinkTable.astPciLink[ulLoop].ulTxEbdStart) ;
        cps__oams_shcmd_printf("g_stPciLinkTable.astPciLink[%d].ulRxBufBase=0x%x\n",
            ulLoop,g_stPciLinkTable.astPciLink[ulLoop].ulRxBufBase) ;
        cps__oams_shcmd_printf("g_stPciLinkTable.astPciLink[%d].ulRxBufNum=0x%x\n",
            ulLoop,g_stPciLinkTable.astPciLink[ulLoop].ulRxBufNum) ;
        cps__oams_shcmd_printf("g_stPciLinkTable.astPciLink[%d].ulTxBufBase=0x%x\n",
            ulLoop,g_stPciLinkTable.astPciLink[ulLoop].ulTxBufBase) ;
        cps__oams_shcmd_printf("g_stPciLinkTable.astPciLink[%d].ulTxBufNum=0x%x\n",
            ulLoop,g_stPciLinkTable.astPciLink[ulLoop].ulTxBufNum) ;

        cps__oams_shcmd_printf("g_stPciLinkTable.aulMemPoolAddr[%d]=0x%x\n",
            ulLoop,g_stPciLinkTable.aulMemPoolAddr[ulLoop]) ;
        cps__oams_shcmd_printf("g_stPciLinkTable.aulMemPoolLen[%d]=0x%x\n",
            ulLoop,g_stPciLinkTable.aulMemPoolLen[ulLoop]) ;

        cps__oams_shcmd_printf("----------------------------------------\n") ;
    }
    cps__oams_shcmd_printf(">>cpss pci driver info show end<<\n") ;
}
#else
VOID cpss_pci_drv_info_show()
{
    cps__oams_shcmd_printf(">>cpss pci driver info show can not support<<\n") ;
}
#endif
/*******************************************************************************
* 函数名称: cpss_dsp_sbbr_show(UINT32 ulDspId)
* 功    能: 打印PCI驱动信息
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       无
* 说   明: 
*******************************************************************************/
#ifdef CPSS_VOS_VXWORKS
void cpss_dsp_sbbr_show(UINT32 ulDspId)
{
#ifdef CPSS_HOST_CPU_WITH_DSP
    UINT32 ulIndex ,ulNextIndex;
    CPSS_SBBR_DSP_RECORD_T *pstSbbrData = NULL;
    UINT32 ulLen;

    pstSbbrData=cpss_mem_malloc(sizeof(CPSS_SBBR_DSP_RECORD_T));
    if(pstSbbrData == NULL)
    {
        return;
    }

    cpss_mem_memset(pstSbbrData, 0 ,sizeof(CPSS_SBBR_DSP_RECORD_T));

    ulLen = CPSS_DSP_SBBR_CPSS_START_ADDR - 0x80000000;

    drv_dsp_ext_mem_read(ulDspId,ulLen,sizeof(CPSS_SBBR_DSP_RECORD_T),(UINT8*)pstSbbrData);

    cps__oams_shcmd_printf(">>cpss dsp sbbr show begin<<\n") ;
    cps__oams_shcmd_printf("cpss_init:InCount:%d\n",pstSbbrData->ulCpssInitInCount);
    cps__oams_shcmd_printf("cpss_init:OutCount:%d\n",pstSbbrData->ulCpssInitOutCount);
    cps__oams_shcmd_printf("cpss_com_init:InCount:%d\n",pstSbbrData->ulComInitInCount);
    cps__oams_shcmd_printf("cpss_com_init:OutCount:%d\n",pstSbbrData->ulComInitOutCount);
    cps__oams_shcmd_printf("cpss_drv_pci_init:InCount:%d\n",pstSbbrData->ulDrvPciInitInCount);
    cps__oams_shcmd_printf("cpss_drv_pci_init:OutCount:%d\n",pstSbbrData->ulDrvPciInitOutCount);
    cps__oams_shcmd_printf("cpss_com_drv_pci_notify_write:InCount:%d\n",pstSbbrData->ulPciNotifyInCount);
    cps__oams_shcmd_printf("cpss_com_drv_pci_notify_write:OutCount:%d\n",pstSbbrData->ulPciNotifyOutCount);
    cps__oams_shcmd_printf("cpss_com_power_on_send:InCount:%d\n",pstSbbrData->ulPowerOnInCount);
    cps__oams_shcmd_printf("cpss_com_power_on_send:OutCount:%d\n",pstSbbrData->ulPowerOnOutCount);
    cps__oams_shcmd_printf("cpss_com_pci_reg_rsp_deal:InCount:%d\n",pstSbbrData->ulRegRspInCount);
    cps__oams_shcmd_printf("cpss_com_pci_reg_rsp_deal:OutCount:%d\n",pstSbbrData->ulRegRspOutCount);
    cps__oams_shcmd_printf("cpss_vk_proc_activate:InCount:%d\n",pstSbbrData->ulVkActivateInCount);
    cps__oams_shcmd_printf("cpss_vk_proc_activate:OutCount:%d\n",pstSbbrData->ulVkActivateOutCount);
    cpss_mem_free(pstSbbrData);
#endif
}
#else
void cpss_dsp_sbbr_show(UINT32 ulDspId)
{
    return;
}
#endif
/*******************************************************************************
* 函数名称: cpss_com_diag_comm_test_rsp_send()
* 功    能: 链路互联测试oams响应消息发送函数
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:
*******************************************************************************/
INT32 cpss_com_diag_comm_test_rsp_send
(
 UINT32 ulSeqId,
 UINT32 ulOamsProc,
 UINT32 ulTestId,
 UINT32 ulReturnCode
)
{
    INT32 lRet;
    CPSS_COM_PID_T stLocalPid ;
    CPS__OAMS_DIAG_START_TEST_RSP_MSG_T stTestRspMsg ;

    stTestRspMsg.stRspHeader.ulSeqId = ulSeqId ;
    stTestRspMsg.stRspHeader.ulReturnCode = ulReturnCode ;
    stTestRspMsg.ulTestId = ulTestId ;

    /*转化为网络字节序*/
    stTestRspMsg.stRspHeader.ulSeqId = cpss_htonl(stTestRspMsg.stRspHeader.ulSeqId) ;
    stTestRspMsg.stRspHeader.ulReturnCode = cpss_htonl(stTestRspMsg.stRspHeader.ulReturnCode) ;
    stTestRspMsg.ulTestId = cpss_htonl(stTestRspMsg.ulTestId) ;

    cpss_com_logic_addr_get(&stLocalPid.stLogicAddr,&stLocalPid.ulAddrFlag) ;
    stLocalPid.ulPd = ulOamsProc ;
    lRet = cpss_com_send(&stLocalPid,CPS__OAMS_DIAG_START_TEST_RSP_MSG,
        (UINT8*)&stTestRspMsg,sizeof(CPS__OAMS_DIAG_START_TEST_RSP_MSG_T)) ;
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
            "send CPS__OAMS_DIAG_START_TEST_RSP_MSG to oams failed!\n");
        return CPSS_ERROR ;
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_logic_addr_get_req_send()
* 功    能: 发送通过物理地址获取逻辑地址及主备状态的请求消息
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_com_logic_addr_get_req_send()
{
    CPSS_COM_PID_T stGcpaPid ;
    CPSS_COM_PID_T stLocalPid ;
    CPSS_COM_PHY_ADDR_T stPhyAddr ;
    CPSS_COM_GET_LOGIC_ADDR_REQ_T stGetLogicAddr ;
    INT32 lRet ;
    UINT32 ulMateFlag = 0 ;

    stPhyAddr = g_stCpssCommTestMan.stOamsTestReq.stDstPhyAddr ;

    /*设置全局板地址信息*/
    stGcpaPid.stLogicAddr = g_pstComData->stAddrGcpaM.stLogiAddr ;
    stGcpaPid.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER ;
    stGcpaPid.ulPd = CPSS_COM_LINK_MNGR_PROC ;

    /*得到本板的逻辑地址主备标志*/
    cpss_com_logic_addr_get(&stLocalPid.stLogicAddr,&stLocalPid.ulAddrFlag) ;
    stLocalPid.ulPd = CPSS_DBG_MNGR_PROC ;

    /*填充物理地址及本板地址信息*/
    stGetLogicAddr.stPhyAddr = stPhyAddr ;
    stGetLogicAddr.stPid = stLocalPid ;

    /*如果本板是全局板备板，则向主用全局板发送请求*/
    if(1==CPSS_COM_LOGIC_ADDR_SAME(stGcpaPid.stLogicAddr,stLocalPid.stLogicAddr))
    {
        if(stLocalPid.ulAddrFlag == CPSS_COM_ADDRFLAG_SLAVE)
        {
            ulMateFlag = 1 ;
        }
    }

    /*转化为网络字节序*/
    cpss_com_pid_hton(&stGetLogicAddr.stPid) ;

    if(ulMateFlag == 0)
    {
        lRet = cpss_com_send(&stGcpaPid,CPSS_COM_GET_LOGIC_ADDR_REQ_MSG,
            (UINT8*)&stGetLogicAddr,sizeof(CPSS_COM_GET_LOGIC_ADDR_REQ_T)) ;
    }
    else
    {
        lRet = cpss_com_send_mate(stGcpaPid.ulPd,CPSS_COM_GET_LOGIC_ADDR_REQ_MSG,
            (UINT8*)&stGetLogicAddr,sizeof(CPSS_COM_GET_LOGIC_ADDR_REQ_T)) ;
    }
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
            "send CPSS_COM_GET_LOGIC_ADDR_REQ_MSG to gcpa failed!\n");
        return CPSS_ERROR ;
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_logic_addr_get_req_deal()
* 功    能: 接收获取逻辑地址及主备状态的请求的消息的处理函数
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
#ifdef CPSS_FUNBRD_MC
INT32 cpss_com_logic_addr_get_req_deal
(
 UINT8* pucRecvBuf
)
{
    CPSS_COM_GET_LOGIC_ADDR_REQ_T* pstGetAddrReq ;
    CPSS_COM_GET_LOGIC_ADDR_RSP_T  stGetAddrRsp ;

    CPS__RDBS_DEV_GETLOGICADDR_MSG_REQ_T stRdbsReq;
    CPS__RDBS_DEV_GETLOGICADDR_MSG_RSP_T stRdbsRsp;

    CPSS_COM_PID_T stSendPid ;
    CPSS_COM_ROUTE_T *pstRoute ;
    CPSS_COM_LOGIC_ADDR_T stLogAddrM ;
    CPSS_COM_PHY_ADDR_T stPhyAddr ;

    CPSS_COM_LOGIC_ADDR_T stGcpaLogAddr ;
    CPSS_COM_PHY_ADDR_T stGcpaMPhyAddr ;
    UINT32 ulAddrFlag ;

    INT32 lRet ;
    UINT32 ulMateFlag = 0 ;

    pstGetAddrReq = (CPSS_COM_GET_LOGIC_ADDR_REQ_T*)pucRecvBuf ;

    stRdbsReq.stCpuPhyAddr = pstGetAddrReq->stPhyAddr ;
    stSendPid = pstGetAddrReq->stPid ;

    /*转化字节序*/
    stSendPid.stLogicAddr.usGroup = cpss_ntohs(stSendPid.stLogicAddr.usGroup) ;
    stSendPid.ulAddrFlag = cpss_ntohl(stSendPid.ulAddrFlag) ;
    stSendPid.ulPd = cpss_ntohl(stSendPid.ulPd) ;

    /*调用rdbs函数得到逻辑地址*/
    cps__rdbs_if_entry(CPS__RDBS_DEV_GETLOGICADDR_MSG,CPS__RDBS_IF_FUNC,
        (UINT8*)&stRdbsReq,sizeof(stRdbsReq),
        (UINT8*)&stRdbsRsp,sizeof(stRdbsRsp));

    if(stRdbsRsp.ulResult == CPSS_OK)
    {
        stGetAddrRsp.stLogAddr = stRdbsRsp.stCpuLogicAddr ;

        stLogAddrM = stGetAddrRsp.stLogAddr ;
        stLogAddrM.ucSubGroup = 1 ;
        stPhyAddr = pstGetAddrReq->stPhyAddr ;

        cpss_com_logic_addr_get(&stGcpaLogAddr,&ulAddrFlag) ;
        cpss_com_phy_addr_get(&stGcpaMPhyAddr) ;

        /*如果和GCPA逻辑地址相等*/
        if(CPSS_COM_LOGIC_ADDR_SAME(stLogAddrM,stGcpaLogAddr)==1)
        {
            if(CPSS_COM_PHY_ADDR_SAME(stPhyAddr,stGcpaMPhyAddr)==1)
            {
                stGetAddrRsp.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER ;
                stGetAddrRsp.ulErrCode = CPSS_OK ;
            }
            else
            {
                stGetAddrRsp.ulAddrFlag = CPSS_COM_ADDRFLAG_SLAVE ;
                stGetAddrRsp.ulErrCode = CPSS_OK ;
            }
        }
        else
        {
            stPhyAddr.ucCpu = 1 ;

            /*得到主备状态*/
            pstRoute = cpss_com_route_find_real(stLogAddrM) ;
            if(pstRoute != NULL)
            {
                if(CPSS_COM_PHY_ADDR_SAME(pstRoute->stPhyAddr[0],stPhyAddr)==1)
                {
                    stGetAddrRsp.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER ;
                    stGetAddrRsp.ulErrCode = CPSS_OK ;
                }
                else if(CPSS_COM_PHY_ADDR_SAME(pstRoute->stPhyAddr[1],stPhyAddr)==1)
                {
                    stGetAddrRsp.ulAddrFlag = CPSS_COM_ADDRFLAG_SLAVE ;
                    stGetAddrRsp.ulErrCode = CPSS_OK ;
                }
                else
                {
                    stGetAddrRsp.ulErrCode = CPSS_ERROR ;
                }
            }
            else
            {
                stGetAddrRsp.ulErrCode = CPSS_ERROR ;
            }
        }
    }
    else
    {
        stGetAddrRsp.ulErrCode = CPSS_ERROR ;
    }

    /*判断是否是备用单板(本板肯定是主用GCPA)*/
    if(1==CPSS_COM_LOGIC_ADDR_SAME(stSendPid.stLogicAddr,g_pstComData->stAddrGcpaM.stLogiAddr))
    {
        if(stSendPid.ulAddrFlag == CPSS_COM_ADDRFLAG_SLAVE)
        {
            ulMateFlag = 1 ;
        }
    }

    /*转化字节序*/
    stGetAddrRsp.stLogAddr.usGroup = cpss_htons(stGetAddrRsp.stLogAddr.usGroup) ;
    stGetAddrRsp.ulErrCode = cpss_htonl(stGetAddrRsp.ulErrCode) ;
    stGetAddrRsp.ulAddrFlag = cpss_htonl(stGetAddrRsp.ulAddrFlag) ;

    /*向外围单板发送响应消息*/
    if(ulMateFlag == 0)
    {
        lRet = cpss_com_send(&stSendPid,CPSS_COM_GET_LOGIC_ADDR_RSP_MSG,
            (UINT8*)&stGetAddrRsp,sizeof(CPSS_COM_GET_LOGIC_ADDR_RSP_T)) ;
    }
    else
    {
        lRet = cpss_com_send_mate(stSendPid.ulPd,CPSS_COM_GET_LOGIC_ADDR_RSP_MSG,
            (UINT8*)&stGetAddrRsp,sizeof(CPSS_COM_GET_LOGIC_ADDR_RSP_T)) ;
    }
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "send CPSS_COM_GET_LOGIC_ADDR_RSP_MSG failed!\n");
        return CPSS_ERROR ;
    }

    return CPSS_OK ;
}
#endif

/*******************************************************************************
* 函数名称: cpss_com_logic_addr_get_rsp_deal()
* 功    能: 发送通过物理地址获取逻辑地址及主备状态的响应消息
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_com_logic_addr_get_rsp_deal
(
 UINT8* pucRecvBuf
)
{
    CPSS_COM_GET_LOGIC_ADDR_RSP_T stRspMsg ;

    stRspMsg = *(CPSS_COM_GET_LOGIC_ADDR_RSP_T*)pucRecvBuf ;

    /*转化为本机字节序*/
    stRspMsg.stLogAddr.usGroup = cpss_ntohs(stRspMsg.stLogAddr.usGroup) ;
    stRspMsg.ulAddrFlag = cpss_ntohl(stRspMsg.ulAddrFlag) ;
    stRspMsg.ulErrCode  = cpss_ntohl(stRspMsg.ulErrCode) ;

    /*如果地址解析失败*/
    if(stRspMsg.ulErrCode != CPSS_OK)
    {
        /*删除定时器*/
        cpss_timer_delete(CPSS_DEBUG_LOGIC_ADDR_GET_TM_NO) ;

        /*向oams发送失败响应*/
        cpss_com_diag_comm_test_rsp_send(g_stCpssCommTestMan.ulSeqId,g_stCpssCommTestMan.ulProcId,g_stCpssCommTestMan.ulTestId,CPSS_ERR_COM_COMM_TEST_ARP_FAIL) ;

        /*清除全局变量*/
        cpss_mem_memset(&g_stCpssCommTestMan,0,sizeof(CPSS_COM_DIAG_COMM_TEST_MAN_T));
        return CPSS_ERROR ;
    }

    /*如果地址解析成功*/
    if(CPSS_COM_COMM_TEST_INIT == g_stCpssCommTestMan.ulUsedFlag)
    {
        g_stCpssCommTestMan.stLogicAddr = stRspMsg.stLogAddr ;
        g_stCpssCommTestMan.ulAddrFlag  = stRspMsg.ulAddrFlag ;

        /*向oams发送成功响应*/
        cpss_com_diag_comm_test_rsp_send(g_stCpssCommTestMan.ulSeqId,g_stCpssCommTestMan.ulProcId,g_stCpssCommTestMan.ulTestId,CPSS_OK) ;

        /*启动链路测试定时器*/
        if(CPSS_OK != cpss_timer_loop_set(CPSS_DEBUG_DIAG_COMM_TEST_TM_NO,
            CPSS_DEBUG_DIAG_COMM_TEST_TM_INTERVAL))
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
                "create CPSS_COM_DIAG_COMM_TEST_TM_NO timer failed.\n");
            return CPSS_ERROR ;
        }

        /*删除定时器*/
        cpss_timer_delete(CPSS_DEBUG_LOGIC_ADDR_GET_TM_NO) ;

        g_stCpssCommTestMan.ulUsedFlag = CPSS_COM_COMM_TEST_NORMAL ;
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_get_logic_addr_tm_recv()
* 功    能: 收到得到逻辑地址地定时器消息的处理函数
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_com_get_logic_addr_tm_recv()
{
    INT32 lRet ;

    lRet = cpss_com_logic_addr_get_req_send() ;
    if(CPSS_ERROR == lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
            "deal get logic addr timer msg failed.\n");
        return CPSS_ERROR ;
    }

    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_comm_test_tm_msg_recv()
* 功    能: 链路互联测试定时器消息的处理函数
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_com_comm_test_tm_msg_recv()
{
    UINT8 *pucBuf ;
    INT32 lRet ;
    UINT32 ulTestLen ;
    CPSS_COM_PID_T stDstPid ;
    CPSS_COM_COMM_TEST_REQ_T *pstTestReq ;
    UINT32 ulMateFlag = 0 ;

    ulTestLen = g_stCpssCommTestMan.stOamsTestReq.ulTestLen ;

    stDstPid.stLogicAddr = g_stCpssCommTestMan.stLogicAddr ;
    stDstPid.ulAddrFlag = g_stCpssCommTestMan.ulAddrFlag ;
    stDstPid.ulPd = CPSS_DBG_MNGR_PROC ;

    /*申请发送内存*/
    pucBuf = cpss_mem_malloc(ulTestLen + sizeof(CPSS_COM_COMM_TEST_REQ_T)) ;
    if(NULL == pucBuf)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
            "deal comm test timer msg failed,malloc failed size=%d.\n",ulTestLen);
        return CPSS_ERROR ;
    }

    pstTestReq = (CPSS_COM_COMM_TEST_REQ_T*)pucBuf ;

    /*测试包已经发送完毕*/
    if(g_stCpssCommTestMan.stOamsTestReq.ulTestNum == 0)
    {
        /*删除测试定时器*/
        cpss_timer_delete(CPSS_DEBUG_DIAG_COMM_TEST_TM_NO) ;

        /*清空全局变量*/
        cpss_mem_memset(&g_stCpssCommTestMan,0,sizeof(CPSS_COM_DIAG_COMM_TEST_MAN_T)) ;

        /*释放申请内存*/
        cpss_mem_free(pucBuf) ;

        return CPSS_OK ;
    }

    /*发送次数递增*/
    g_stCpssCommTestMan.ulSerialNo = g_stCpssCommTestMan.ulSerialNo + 1 ;

    /*发送次数递减*/
    g_stCpssCommTestMan.stOamsTestReq.ulTestNum = g_stCpssCommTestMan.stOamsTestReq.ulTestNum - 1 ;

    pstTestReq->ulSeqId = g_stCpssCommTestMan.ulSeqId ;
    pstTestReq->ulSerialId = g_stCpssCommTestMan.ulSerialNo ;
    pstTestReq->ulTestId = g_stCpssCommTestMan.ulTestId ;
    pstTestReq->ulMsgLen = g_stCpssCommTestMan.stOamsTestReq.ulTestLen ;
    pstTestReq->ulTestNum = g_stCpssCommTestMan.stOamsTestReq.ulTestNum ;
    pstTestReq->ulOamsProc= g_stCpssCommTestMan.ulProcId ;
    cpss_com_logic_addr_get(&pstTestReq->stSrcPid.stLogicAddr,&pstTestReq->stSrcPid.ulAddrFlag) ;
    pstTestReq->stSrcPid.ulPd = CPSS_DBG_MNGR_PROC ;

    /*判断是否主备通道间测试数据*/
    if(1 == CPSS_COM_LOGIC_ADDR_SAME(stDstPid.stLogicAddr,pstTestReq->stSrcPid.stLogicAddr))
    {
        if(stDstPid.ulAddrFlag != pstTestReq->stSrcPid.ulAddrFlag)
        {
            ulMateFlag = 1 ;
        }
    }

    /*转变字节序*/
    pstTestReq->ulSerialId = cpss_htonl(pstTestReq->ulSerialId) ;
    pstTestReq->ulSeqId = cpss_htonl(pstTestReq->ulSeqId) ;
    pstTestReq->ulTestId = cpss_htonl(pstTestReq->ulTestId) ;
    pstTestReq->ulMsgLen = cpss_htonl(pstTestReq->ulMsgLen) ;
    pstTestReq->ulTestNum = cpss_htonl(pstTestReq->ulTestNum) ;
    pstTestReq->ulOamsProc = cpss_htonl(pstTestReq->ulOamsProc) ;
    cpss_com_pid_hton(&pstTestReq->stSrcPid) ;

    /*发送测试数据*/
    if(ulMateFlag == 0)
    {
        lRet = cpss_com_send(&stDstPid,CPSS_COM_COMM_TEST_REQ_MSG,pucBuf,
            sizeof(CPSS_COM_COMM_TEST_REQ_T)+ulTestLen) ;
    }
    else
    {
        lRet = cpss_com_send_mate(stDstPid.ulPd,CPSS_COM_COMM_TEST_REQ_MSG,pucBuf,
            sizeof(CPSS_COM_COMM_TEST_REQ_T)+ulTestLen) ;
    }
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
            "send comm test req msg failed\n");
        cpss_mem_free(pucBuf) ;
        return CPSS_ERROR ;
    }

    cpss_mem_free(pucBuf) ;
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_comm_test_req_deal()
* 功    能: 链路互联测试请求消息的处理函数
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_com_comm_test_req_deal
(
 UINT8 *pucRecvBuf
)
{
    INT32 lRet ;
    CPSS_COM_COMM_TEST_REQ_T *pstTestReq ;
    CPSS_COM_COMM_TEST_RSP_T stTestRsp ;
    CPSS_COM_PID_T stDstPid ;
    CPSS_COM_PID_T stLocalPid ;
    UINT32 ulMateFlag = 0 ;

    pstTestReq = (CPSS_COM_COMM_TEST_REQ_T*)pucRecvBuf ;

    stDstPid = pstTestReq->stSrcPid ;

    stTestRsp.ulSerialId = pstTestReq->ulSerialId ;
    stTestRsp.ulSeqId    = pstTestReq->ulSeqId ;
    stTestRsp.ulTestId   = pstTestReq->ulTestId ;
    stTestRsp.ulMsgLen   = pstTestReq->ulMsgLen ;
    stTestRsp.ulTestNum  = pstTestReq->ulTestNum ;
    stTestRsp.ulOamsProc = pstTestReq->ulOamsProc ;
    stTestRsp.ulErrCode  = cpss_htonl(CPSS_OK) ;

    /*转换字节序*/
    cpss_com_pid_ntoh(&stDstPid) ;

    cpss_com_logic_addr_get(&stLocalPid.stLogicAddr,&stLocalPid.ulAddrFlag) ;

    /*判断是否主备通道*/
    if(1 == CPSS_COM_LOGIC_ADDR_SAME(stDstPid.stLogicAddr,stLocalPid.stLogicAddr))
    {
        if(stDstPid.ulAddrFlag != stLocalPid.ulAddrFlag)
        {
            ulMateFlag = 1 ;
        }
    }

    /*发送链路测试响应到对端的DEBUG纤程*/
    if(ulMateFlag == 0)
    {
        lRet = cpss_com_send(&stDstPid,CPSS_COM_COMM_TEST_RSP_MSG,
            (UINT8*)&stTestRsp,sizeof(CPSS_COM_COMM_TEST_REQ_T)) ;
    }
    else
    {
        lRet = cpss_com_send_mate(stDstPid.ulPd,CPSS_COM_COMM_TEST_RSP_MSG,
            (UINT8*)&stTestRsp,sizeof(CPSS_COM_COMM_TEST_REQ_T)) ;
    }
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
            "send comm test rsp msg failed\n");
        return CPSS_ERROR ;
    }

    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_comm_test_rsp_deal()
* 功    能: 链路互联测试响应消息的处理函数
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_com_comm_test_rsp_deal
(
 UINT8 *pucRecvBuf
)
{
    CPSS_COM_PID_T stOamsPid ;
    CPSS_COM_COMM_TEST_RSP_T* pstTestRsp ;
    CPS__OAMS_DIAG_TEST_APP_RPT_IND_MSG_T stTestIndMsg ;
    CPS__OAMS_DIAG_CPSS_CTRLP_COMM_TEST_RESULT_T stOamsRspInfo ;
    UINT32 ulTestNum ;
    INT32 lRet ;

    pstTestRsp = (CPSS_COM_COMM_TEST_RSP_T*)pucRecvBuf ;

    ulTestNum = cpss_ntohl(pstTestRsp->ulTestNum) ;

    /*网络字节序*/
    stTestIndMsg.stReqHeader.ulSeqId = pstTestRsp->ulSeqId ;
    stTestIndMsg.stRptInfo.ulTestId = pstTestRsp->ulTestId ;
    stTestIndMsg.stRptInfo.ulTestResult = pstTestRsp->ulErrCode ;
    stTestIndMsg.stRptInfo.ulTestRptLen =
        cpss_htonl(sizeof(CPS__OAMS_DIAG_CPSS_CTRLP_COMM_TEST_RESULT_T)) ;

    if(ulTestNum == 0)
    {
        stTestIndMsg.stRptInfo.ulRptType = cpss_htonl(CPS__OAMS_DIAG_RPT_TYPE_END) ;
    }
    else
    {
        stTestIndMsg.stRptInfo.ulRptType = cpss_htonl(CPS__OAMS_DIAG_RPT_TYPE_MIDDLE) ;
    }

    stOamsRspInfo.ulResult    = pstTestRsp->ulErrCode ;
    stOamsRspInfo.ulTestSeqId = pstTestRsp->ulSerialId ;
    stOamsRspInfo.ulTestLen   = pstTestRsp->ulMsgLen ;

    cpss_mem_memcpy(stTestIndMsg.stRptInfo.aucRptInfo,(UINT8*)&stOamsRspInfo,
        sizeof(CPS__OAMS_DIAG_CPSS_CTRLP_COMM_TEST_RESULT_T)) ;

    /*发送数据到本板的OAMS*/
    cpss_com_logic_addr_get(&stOamsPid.stLogicAddr,&stOamsPid.ulAddrFlag) ;
    stOamsPid.ulPd = cpss_ntohl(pstTestRsp->ulOamsProc) ;
    lRet = cpss_com_send(&stOamsPid,CPS__OAMS_DIAG_TEST_APP_RPT_IND_MSG,(UINT8*)&stTestIndMsg,
        sizeof(CPS__OAMS_DIAG_TEST_APP_RPT_IND_MSG_T)) ;

    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
            "send comm test ind msg failed\n");
        return CPSS_ERROR ;
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_diag_comm_test_req_deal()
* 功    能: 链路互联测试oams测试诊断请求消息的处理函数
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_com_diag_comm_test_req_deal
(
 UINT8* pucRecvBuf,
 UINT32 ulSeqId,
 UINT32 ulOamsProc,
 UINT32 ulTestId
)
{
    CPS__OAMS_DIAG_CPSS_CTRLP_COMM_TEST_REQ_T stOamsTestReq ;

    stOamsTestReq = *(CPS__OAMS_DIAG_CPSS_CTRLP_COMM_TEST_REQ_T*)pucRecvBuf ;

    /*变为本机字节序*/
    stOamsTestReq.ulTestLen = cpss_ntohl(stOamsTestReq.ulTestLen) ;
    stOamsTestReq.ulTestNum = cpss_ntohl(stOamsTestReq.ulTestNum) ;

    /*如果链路测试功能正在使用，则给oams返回失败，并指明错误原因*/
    if(g_stCpssCommTestMan.ulUsedFlag != CPSS_COM_COMM_TEST_FREE)
    {
        cpss_com_diag_comm_test_rsp_send(ulSeqId,ulOamsProc,ulTestId,CPSS_ERR_COM_COMM_TEST_BUSY) ;
        return CPSS_ERROR ;
    }

     /*如果测试字符超过2兆或者次数超过10次则给oams返回失败，并指明错误原因*/
   if((stOamsTestReq.ulTestLen>CPSS_COM_DIAG_COMM_TEST_MAXLEN)||(stOamsTestReq.ulTestNum>CPSS_COM_DIAG_COMM_TEST_MAXNUM))
    {
        cpss_com_diag_comm_test_rsp_send(ulSeqId,ulOamsProc,ulTestId,CPSS_ERR_COM_PARA_ILLEGAL ) ;
        return CPSS_ERROR ;
    }

    /*设置全局变量*/
    g_stCpssCommTestMan.stOamsTestReq.stDstPhyAddr = stOamsTestReq.stDstPhyAddr ;
    g_stCpssCommTestMan.stOamsTestReq.ulTestLen = stOamsTestReq.ulTestLen ;
    g_stCpssCommTestMan.stOamsTestReq.ulTestNum = stOamsTestReq.ulTestNum ;

    g_stCpssCommTestMan.ulSeqId = ulSeqId ;
    g_stCpssCommTestMan.ulTestId = ulTestId ;
    g_stCpssCommTestMan.ulSerialNo = 0 ;
    g_stCpssCommTestMan.ulProcId = ulOamsProc ;
    g_stCpssCommTestMan.ulUsedFlag = CPSS_COM_COMM_TEST_INIT ;

    /*向全局板的链路管理纤程发送解析物理地址消息*/
    cpss_com_logic_addr_get_req_send() ;

    /*开启重发定时器*/
    if(CPSS_OK != cpss_timer_loop_set(CPSS_DEBUG_LOGIC_ADDR_GET_TM_NO,
        CPSS_DEBUG_LOGIC_ADDR_GET_TM_INTERVAL))
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FATAL,
            "create get logic addr req failed!\n");
        return CPSS_ERROR ;
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_comm_stop_test_deal()
* 功    能: 链路互联测试诊断停止消息的处理函数
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
VOID cpss_com_comm_stop_test_deal
(
 UINT32 ulTestId
)
{
    if(ulTestId == g_stCpssCommTestMan.ulTestId)
    {
        /*停止链路测试定时器*/
        cpss_timer_delete(CPSS_DEBUG_DIAG_COMM_TEST_TM_NO) ;

        /*清空链路测试全局变量内存区*/
        cpss_mem_memset(&g_stCpssCommTestMan,0,sizeof(CPSS_COM_DIAG_COMM_TEST_MAN_T)) ;
    }
}

/*******************************************************************************
* 函数名称: cpss_dsp_sbbr_switch_set()
* 功    能: dsp上的黑匣子开关函数
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
＊　　　　　　　　　　INT32           flag              flag = 1:打开；flag = 0；关闭
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
void cpss_dsp_sbbr_switch_set(INT32 flag)
{
    g_ulCpssDspSbbrMsgRecSwitch = flag;
    cps__oams_shcmd_printf("Set dsp sbbr OK flag = %d!\n",flag);
}

/******************************* 源文件结束 **********************************/
