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
#include "cpss_com_common.h"
#include "cpss_com_link.h"
//#include "om_public.h"

#include <errno.h>
#include <arpa/inet.h>
#include <stdio.h>

#define MAIN_BRD

#ifdef MAIN_BRD

#define LOCAL_BRD_PHY_ADDR 0x03010101
#define MATE_BRD_PHY_ADDR 0x03010201

#else

#define LOCAL_BRD_PHY_ADDR 0x03010201
#define MATE_BRD_PHY_ADDR 0x03010101

#endif

CPSS_COM_PHY_ADDR_T g_LocalBrdPhyAddr;
CPSS_COM_PHY_ADDR_T g_MateBrdPhyAddr;
UINT32 g_MateBrdIpAddr;
UINT32 g_LocalBrdIpAddr;
INT32 g_MateSockId;
CPSS_COM_LOGIC_ADDR_T gLocalBrdLogAddr;
CPSS_COM_LOGIC_ADDR_T gMateBrdLogAddr;

extern VOID cpss_kw_set_task_stop() ;
extern VOID cpss_kw_set_task_run() ;
extern INT32 cpss_com_version_check(CPSS_COM_TRUST_HEAD_T* pstTrustHdr, UINT32 ulRecvLen);
extern VOID cpss_udp_drv_stat(UINT32 ulStatFlag, UINT32 ulIp);
extern INT32 cpss_com_drv_linkid_get(UINT32 ulDrvType, UINT32 ulLinkInfo[3],UINT32 *pulLinkId);
INT32 cpss_dst_dev_logaddr_get(CPSS_COM_LOGIC_ADDR_T *pstLogicAddr, UINT32 *pulAddrFlag);

INT32 cpss_com_addr_init_mate();
INT32 cpss_com_socket_init_mate();
void cpss_com_drv_udp_task_mate();
INT32 cpss_com_route_create_mate();
INT32 cpss_com_get_mate_phyaddr();

//extern INT32 upm_ccm_if_cfg(UINT8 op,  OM_UPM_AC_INTERFACE_T *pstif);

INT32 cpss_com_init_mate()
{
	INT32 lRet;
	INT8* pcName = "CpssUdpMate";

	return CPSS_ERROR;//dhwang added 20120105

	lRet = cpss_com_addr_init_mate();
	if(CPSS_OK != lRet)
	{
//		printf("cpss_com_addr_init_mate error! Ret = %d\n", lRet);
		return CPSS_ERROR;
	}

	lRet = cpss_com_socket_init_mate();
	if(CPSS_ERROR == lRet)
	{
		printf("cpss_com_socket_init_mate error!\n");
		return CPSS_ERROR;
	}

	lRet = cpss_vos_task_spawn(pcName, 1, 0, 0, cpss_com_drv_udp_task_mate, 0);
	if(CPSS_ERROR == lRet)
	{
		printf("Task CpssUdpMate creat error\n");
		return CPSS_ERROR;
	}

	lRet = cpss_com_route_create_mate();
	if(CPSS_ERROR == lRet)
	{
		printf("cpss_com_route_create_mate error\n");
		return CPSS_ERROR;
	}

	return CPSS_OK;
}

INT32 cpss_com_addr_init_mate()
{
#if 0
	UINT32 ulLocalPhyAddr;
	UINT32 ulMatePhyAddr;

	ulLocalPhyAddr = cpss_htonl(LOCAL_BRD_PHY_ADDR);
	ulMatePhyAddr = cpss_htonl(MATE_BRD_PHY_ADDR);

	g_LocalBrdPhyAddr = *(CPSS_COM_PHY_ADDR_T*)(&ulLocalPhyAddr);
	g_MateBrdPhyAddr = *(CPSS_COM_PHY_ADDR_T*)(&ulMatePhyAddr);
#endif
	INT32 lRet;

	lRet = cpss_com_get_mate_phyaddr();
	if(CPSS_OK != lRet)
	{
		return lRet;
	}

	lRet = cpss_com_log_addr_get(CPSS_MC_MODULE, &g_LocalBrdPhyAddr, &gLocalBrdLogAddr);
	if(CPSS_OK != lRet)
	{
		return lRet;
	}

	lRet = cpss_com_log_addr_get(CPSS_MC_MODULE, &g_MateBrdPhyAddr, &gMateBrdLogAddr);
	if(CPSS_OK != lRet)
	{
		return lRet;
	}

	cpss_com_phy2ip(g_LocalBrdPhyAddr, &g_LocalBrdIpAddr, FALSE);

	cpss_com_phy2ip(g_MateBrdPhyAddr, &g_MateBrdIpAddr, FALSE);

	return CPSS_OK;
}

INT32 cpss_com_socket_init_mate()
{
	CPSS_COM_SOCK_ADDR_T *pstSockAddr;
	INT32 lSockId;
	INT32 lRet;
	INT8 acIpBuf[4];
	INT8 acCmdBuf[256];

	memcpy(acIpBuf, &g_LocalBrdIpAddr, sizeof(acIpBuf));

//	printf("in cpss_com_socket_init_mate ,ip = %d.%d.%d.%d", acIpBuf[0], acIpBuf[1], acIpBuf[2], acIpBuf[3]);

	memset(acCmdBuf, 0, sizeof(acCmdBuf));
	sprintf(acCmdBuf, "ifconfig spi0:100 %d.%d.%d.%d netmask 255.255.0.0 up", acIpBuf[0], acIpBuf[1], acIpBuf[2], acIpBuf[3]);
//	printf("cmd = %s\n", acCmdBuf);

	cps_system(acCmdBuf);

#if 0
	OM_UPM_AC_INTERFACE_T stInterFace;
	memset(&stInterFace, 0, sizeof(stInterFace));

	strcpy(&stInterFace.aucIfName, "spi0:100");
	memcpy(&stInterFace.astIfIP, &g_LocalBrdIpAddr, 4);
	stInterFace.ucAdminState = 2;
	stInterFace.ucIfIpNum = 1;
	stInterFace.usVlanId = 0;

	if(-1 == upm_ccm_if_cfg(1, &stInterFace))
	{
		printf("upm_ccm_if_cfg error in cpss_com_socket_init_mate");
	}
#endif

	pstSockAddr = cpss_mem_malloc(sizeof(CPSS_COM_SOCK_ADDR_T));

	lSockId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(-1 == lSockId)
	{
		perror("socket");
		return CPSS_ERROR;
	}
	else
	{
		g_MateSockId = lSockId;
	}

	pstSockAddr->sin_family = AF_INET;
	pstSockAddr->sin_addr.s_addr = g_LocalBrdIpAddr;
	pstSockAddr->sin_port = cpss_htons(CPSS_COM_UDP_SERVER_PORT);

    printf("socket bind ipaddr = %s, port = %d\n", inet_ntoa(pstSockAddr->sin_addr), cpss_ntohs(pstSockAddr->sin_port));

	lRet = bind(g_MateSockId, (struct sockaddr*)pstSockAddr, sizeof(CPSS_COM_SOCK_ADDR_T));
	if(-1 == lRet)
	{
		perror("bind");

		close(g_MateSockId);

		cpss_mem_free(pstSockAddr);

		return CPSS_ERROR;
	}

	cpss_mem_free(pstSockAddr);

	return CPSS_OK;
}

void cpss_com_drv_udp_task_mate()
{
    CPSS_COM_SOCK_ADDR_T stSockAddr;
    CPSS_COM_TRUST_HEAD_T *pstTrustHdr = NULL;
    CPSS_COM_DRV_T  *pstDrv  = cpss_com_drv_find(CPSS_COM_DRV_UDP);

    /* 消息大小定义个极限值64k */
    INT32 lRecvLen, lMaxRecvLen = 64*1024;
    UINT32 ulLinkId,ulLinkInfo[3];

    INT32 lRecvSize   = sizeof(CPSS_COM_SOCK_ADDR_T);

    /* 申请接收缓冲区内存 */
    UINT8 *pucRecvBuf = cpss_mem_malloc(lMaxRecvLen);

    if(NULL == pucRecvBuf)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                   "udp task, malloc receive buff failed!");
        return;
    }

    /* 初始化缓冲区内存 */
    cpss_mem_memset(pucRecvBuf,0,lMaxRecvLen);

    /* 等待通信模块初始化完成 */
    while(0 == g_MateSockId)
    {
        cpss_vos_task_delay(100);
    }

    while(TRUE)
    {

        cpss_kw_set_task_stop() ;

        /* 接收UDP消息 */
        lRecvLen = recvfrom(g_MateSockId, pucRecvBuf, lMaxRecvLen,
                            0, (struct sockaddr *)&stSockAddr, &lRecvSize);

        if( SOCKET_ERROR == lRecvLen )
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
        if(CPSS_OK != cpss_com_version_check(pstTrustHdr, lRecvLen))
        {
            continue ;
        }

        /* 申请通信消息头,然后拷贝消息 */
        pstTrustHdr = cpss_com_nodisp_mem_alloc(CPSS_TRUST_HDR_LEN_SUBTRACT(lRecvLen));

        if(NULL == pstTrustHdr)
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                      "udp task, TRUST message head alloc failed!");
            continue;
        }

        cpss_mem_memcpy((UINT8*)pstTrustHdr, pucRecvBuf, lRecvLen);

        /* 根据链路参数, 获取linkId */
        ulLinkInfo[0] = g_MateSockId;
        ulLinkInfo[1] = cpss_ntohl(stSockAddr.sin_addr.s_addr);
        ulLinkInfo[2] = cpss_ntohs(stSockAddr.sin_port);

        if(CPSS_OK != cpss_com_drv_linkid_get(CPSS_COM_DRV_UDP, ulLinkInfo, &ulLinkId))
        {
            cpss_com_mem_free(pstTrustHdr);
            continue;
        }

        /* 把收到的报文交给驱动适配层来处理 */
        if((pstDrv->pfRead != NULL) &&
          (CPSS_OK == pstDrv->pfRead(CPSS_COM_DRV_UDP, ulLinkId, (UINT8 *)pstTrustHdr, lRecvSize)))
        {
            continue;
        }

    }/*end while(TRUE)*/
}

INT32 cpss_com_route_create_mate()
{
#if 0
	CPSS_COM_LOGIC_ADDR_T stLogicAddr;
	CPSS_COM_PHY_ADDR_T stPhyAddr;
	UINT32 ulMatePhyAddr;

#endif

	UINT32 ulAddrFlag;
	UINT32 ulDrvType;
	INT32 lRet;

#if 0
	ulMatePhyAddr = cpss_htonl(MATE_BRD_PHY_ADDR);

	stPhyAddr = *(CPSS_COM_PHY_ADDR_T*)(&ulMatePhyAddr);

	lRet = cpss_com_log_addr_get(CPSS_MC_MODULE, &stPhyAddr, &stLogicAddr);
	if(CPSS_ERROR == lRet)
	{
		return CPSS_ERROR;
	}
#endif

	ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER;

	ulDrvType = CPSS_COM_DRV_UDP;

	lRet = cpss_com_route_create(gMateBrdLogAddr, ulAddrFlag, g_MateBrdPhyAddr, ulDrvType);
	if(CPSS_ERROR == lRet)
	{
		return CPSS_ERROR;
	}

	return CPSS_OK;
}

INT32 cpss_com_get_mate_phyaddr()
{
	FILE* fConf;
	char cBuf[100];
	char* pcStr;
	unsigned char ucNum;

	fConf = fopen("MateAddr.conf", "r");
	if(NULL == fConf)
	{
		return 1;
	}

	while(!feof(fConf))
	{
		fgets(cBuf, 100, fConf);
		pcStr = strchr(cBuf, '=');
		if(NULL == pcStr)
		{
			continue;
		}

		if(0 == strncmp("Shelf", cBuf, strlen("Shelf")))
		{
			sscanf(pcStr + 1, "%hhd", &ucNum);
			if(4 < ucNum)
			{
				fclose(fConf);

				return 2;
			}

			g_LocalBrdPhyAddr.ucShelf = ucNum;
			g_MateBrdPhyAddr.ucShelf = ucNum;
		}
		else if(0 == strncmp("Frame", cBuf, strlen("Frame")))
		{
			sscanf(pcStr + 1, "%hhd", &ucNum);
			if(5 < ucNum)
			{
				fclose(fConf);

				return 3;
			}

			g_LocalBrdPhyAddr.ucFrame = ucNum;
			g_MateBrdPhyAddr.ucFrame = ucNum;
		}
		else if(0 == strncmp("BrdType", cBuf, strlen("BrdType")))
		{
			sscanf(pcStr + 1, "%hhd", &ucNum);
			if(0 == ucNum)
			{
				g_LocalBrdPhyAddr.ucSlot = 1;
				g_MateBrdPhyAddr.ucSlot = 2;
			}
			else if(1 == ucNum)
			{
				g_LocalBrdPhyAddr.ucSlot = 2;
				g_MateBrdPhyAddr.ucSlot = 1;
			}
			else
			{
				fclose(fConf);

				return 4;
			}
		}
	}
	g_LocalBrdPhyAddr.ucCpu = 1;
	g_MateBrdPhyAddr.ucCpu = 1;

	fclose(fConf);

	return CPSS_OK;
}

INT32 cpss_dst_dev_logaddr_get(CPSS_COM_LOGIC_ADDR_T *pstLogicAddr, UINT32 *pulAddrFlag)
{
    if((NULL == pstLogicAddr) || (NULL == pulAddrFlag) || (NULL == &gMateBrdLogAddr))
    {
        return CPSS_ERROR;
    }

    *pstLogicAddr = gMateBrdLogAddr;
    *pulAddrFlag = CPSS_COM_ADDRFLAG_MASTER;

    return CPSS_OK;
}
