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
#include "mt_common.h"
#include <time.h>

INT32 glSockId = -1;
INT32 glCpmSockId = -1;
INT32 glUpmSockId = -1;
INT32 glDhcpSockId = -1;

MTBool gbShowFlag = MT_TRUE;//周期性统计信息打印开关
extern UINT32 gulIpAddrFlag;//IP Addr Flag
UINT8 gucWtpEventFlag = MT_WTP_EVENT_DISABLE;
extern UINT8 gaucAcIpAddrV4[MT_IPV4_ADDR_LEN];//AC的IP地址
extern UINT8 gaucAcIpAddrV6[MT_IPV6_ADDR_LEN];//AC的IP地址
extern UINT8 gucStaAccessPlcy;
extern UINT32 gulStaHoldTime;
extern MTBool gbStaDhcpFlag;
extern MTBool gbLogFlag;

UINT8 gaucIpV4Header[] = {
  0x45, 0x00, 0x00, 0xeb, 0x00, 0x00, 0x40, 0x00, 0x40, 0x11, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xff, 0xff, 0xff, 0xff
};

UINT8 gaucUdpHeader[] = {0x04, 0x02, 0x14, 0x7e, 0x00, 0xd7, 0x00, 0x00};

UINT8 gaucIpV6Header[] = {
  0x60, 0x00, 0x00, 0x00, 0x01, 0x16, 0x11, 0x40, 0x20, 0x01, 0x00, 0x33, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x20, 0x01, 0x00, 0x33, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01
};
//IPv6数据报文头结构:
//60 00 00 00      IPv+ Traffic
//01 16            PayloadLen
//11               NextHeader
//40               Hotlimit
//20 01 00 33 00 00 00 00 00 00 00 00 00 00 01 00 Src
//20 01 00 33 00 00 00 00 00 00 00 00 00 00 00 01 Des

//GetSequenceNum
UINT32 MT_GetStaId(UINT32 vulStaId)
{
  if(MT_SYS_STA_CAPABILITY == vulStaId)
    {
    return 0;
    }
  else
    {
    return (vulStaId + 1) % MT_SYS_STA_CAPABILITY;
    }
}

//GetSequenceNum
UINT16 MT_GetSequenceNum(UINT16 vusSequence)
{
  UINT16 usSequenceNum = 0;

  if(0xFFFF == vusSequence)
    {
    usSequenceNum = 0;
    }
  else
    {
    usSequenceNum = (vusSequence + 1) % 0xFFFF;
    }

  return usSequenceNum;
}

//Get BSSID
void MT_GetBssID(UINT32 vulApIndex,UINT8 vucRadioId, UINT8 vucWlanId,UINT8 *vpBssId)
{
  UINT16 usApIndex = 0;
  UINT8 aucBssid[MT_MAC_LENTH] = {0};
  UINT32 ulLoop = 0;

  usApIndex = (UINT16)vulApIndex;
  usApIndex = htons(usApIndex);

  //0x00, 0x0b, 0x6b, 0xd9, 0xa9, 0x69
  aucBssid[0] = 0x00;
  aucBssid[1] = MT_WTP_BSSID_KEY_BYTE;// do not change it
  cpss_mem_memcpy(&aucBssid[2],&usApIndex,sizeof(usApIndex));
  aucBssid[4] = vucRadioId;
  aucBssid[5] = vucWlanId;

  for(ulLoop = 0;ulLoop < MT_MAC_LENTH;ulLoop++)
     {
    vpBssId[ulLoop] = aucBssid[ulLoop];
     }
}

UINT32 MT_GetApIndexFromCpmPort(UINT32 vulApCpmPort)
{
	UINT32 ulApIdx = MT_UNDEFINED_OBJECT_IDENTIFIER;

	if(vulApCpmPort >= MT_AP_CPM_PORT_START)
	{
		ulApIdx = vulApCpmPort - MT_AP_CPM_PORT_START;
	}

	return ulApIdx;
}

UINT32 MT_GetApIndexFromUpmPort(UINT32 vulApUpmPort)
{
	UINT32 ulApIdx = MT_UNDEFINED_OBJECT_IDENTIFIER;

	if(vulApUpmPort >= MT_AP_UPM_PORT_START)
	{
		ulApIdx = vulApUpmPort - MT_AP_UPM_PORT_START;
	}

	return ulApIdx;
}

UINT16 MT_GetApCpmPort(UINT32 vulApIndex)
{
  return MT_AP_CPM_PORT_START + vulApIndex;
}

UINT16 MT_GetApUpmPort(UINT32 vulApIndex)
{
  return MT_AP_UPM_PORT_START + vulApIndex;
}

//IPV4 checksum
UINT16 MT_CalcIpChksm(UINT8 *vaucBuf,UINT16 vusbufLen)
{
  unsigned int sum = 0;
  union
  {
      unsigned char *addr;
      unsigned short *i;
  } p;
  unsigned short nleft = vusbufLen;

  p.addr = vaucBuf;
  while (nleft > 1)
    {
      sum += *p.i++;
      nleft -= 2;
  }

  if (nleft == 1)
    {
      unsigned char a = 0;
      memcpy (&a, p.i, 1);
      sum += ntohs (a) << 8;
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);

  return ~sum;
}

//ICMPv6 Checksum
#if 0
void MT_CalcIcmpCksum(UINT8 *chksum, UINT8 *nptr, int nlen)
{
  INT32 x, news;
  UINT8 newlen = nlen;

  x = 0;

  while (newlen)
     {
    news = nptr[0] * 256 + nptr[1];
    nptr += 2;
    x += news & 0xffff;

    if (x & 0x10000)
           {
      x++;
      x&=0xffff;
            }

    newlen-=2;
        }

  x =~x & 0xFFFF;
  chksum[0]=x / 256;
  chksum[1]=x & 0xff;
}

UINT16 MT_CalcIPv6CheckSum(UINT16 *addr, UINT32 len)
{
  INT32 nLeft = len;
  UINT32 sum = 0;
  UINT16 *w = addr;
  UINT16 answer = 0;

  while(nLeft > 1)
    {
    sum += *w++;
    nLeft -= 2;
    }

  if(nLeft == 1)
     {
    *(UINT8 *)(&answer) = *(UINT8 *)w;
    sum += answer;
     }

  sum = (sum >> 16) + (sum & 0xFFFF);/* add hi 16 to low 16*/
  sum += (sum >> 16); /*add carry*/
  answer = ~sum; /*truncate to 16 bits*/
  return (answer);
}
#endif

unsigned short MT_GetIcmpv6checksum(unsigned char *buffer,int len)
{
  unsigned long cksum = 0;
  unsigned short *p=(UINT16*)buffer;
  int size=(len>>1)+(len&0x1);
  while (size > 0)
     {
      cksum += *p;
      p++;
      size--;
     }

  cksum = (cksum >> 16) + (cksum & 0xffff);
  cksum += (cksum >> 16);
  return (unsigned short) (~cksum);
}

//消息分发：区分5246端口和5247端口
void MT_Task_MsgDispctcher(void)
{
	MT_UDP_HDR_T stUdpHdr;
	MT_IPV4_HEADER_T stIpv4Hdr;
	struct sockaddr_in stSrcAddr;
	UINT32 ulSuffix = 0;
	UINT32 ulSrcAddrLen = 0;
	INT32 ilRcvLen = 0;
	CPSS_COM_PID_T stDstPid;
	UINT32 ulComSndMsgLen = 0;
	UINT8 aucRcvBuf[MT_RECV_MSG_BUF_LEN] = {0};
	MT_DISPATCHER_SND_MSG_T stDisptchrMsg;
	UINT32 ulUdpSrcPrt = 0;
	UINT32 ulUdpDstPrt = 0;
	UINT32 ulMsgId = 0;
	INT32 retValue = 0;
	INT8 iRetCode = CPSS_ERROR;

	cpss_mem_memset(&stUdpHdr,0,sizeof(MT_UDP_HDR_T));
	cpss_mem_memset(&stIpv4Hdr,0,sizeof(MT_IPV4_HEADER_T));
	cpss_mem_memset(&stSrcAddr,0,sizeof(stSrcAddr));
	cpss_mem_memset(&stDisptchrMsg,0,sizeof(MT_DISPATCHER_SND_MSG_T));
	cpss_mem_memset(&stDstPid,0,sizeof(CPSS_COM_PID_T));

	if(IP_ADDR_TYPE_V4 == gulIpAddrFlag)
    {
		retValue = 1;
		glSockId = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

		if(glSockId < 0)
		{
			printf("Get Socket failure. File %s; Line: %d",__FILE__,__LINE__);
			return;
		}

		iRetCode = setsockopt(glSockId, IPPROTO_IP, IP_HDRINCL, &retValue, sizeof (retValue));

		if(iRetCode < 0)
		{
			printf("SetSockopt Failure. File: %s, Line: %d\n",__FILE__,__LINE__);
			return;
		}

#if 0
		//send to 5246 socket
		glCpmSockId = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

		if(glCpmSockId < 0)
		{
			printf("Get Socket failure. File %s; Line: %d",__FILE__,__LINE__);
			return;
		}

		iRetCode = setsockopt(glCpmSockId, IPPROTO_IP, IP_HDRINCL, &retValue, sizeof (retValue));

		if(iRetCode < 0)
		{
			printf("SetSockopt Failure. File: %s, Line: %d\n",__FILE__,__LINE__);
			return;
		}

		//send to 5247 socket
		glUpmSockId = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);

		if(glUpmSockId < 0)
		{
			printf("Get Socket failure. File %s; Line: %d",__FILE__,__LINE__);
			return;
		}

		iRetCode = setsockopt(glUpmSockId, IPPROTO_IP, IP_HDRINCL, &retValue, sizeof (retValue));

		if(iRetCode < 0)
		{
			printf("SetSockopt Failure. File: %s, Line: %d\n",__FILE__,__LINE__);
			return;
		}
#endif
    }
	else
	{
		retValue = 1;
		glSockId = glSockId = socket(AF_INET6, SOCK_RAW, IPPROTO_UDP);

		if(glSockId < 0)
		{
			printf("Get Socket failure. File %s; Line: %d",__FILE__,__LINE__);
			return;
		}

		iRetCode = setsockopt(glSockId, IPPROTO_IPV6, IPV6_V6ONLY, &retValue, sizeof (retValue));

		if(iRetCode < 0)
		{
			printf("SetSockopt Failure. File: %s, Line: %d\n",__FILE__,__LINE__);
			return;
		}
	}

	glCpmSockId = glSockId;
	glUpmSockId = glSockId;

	//获取逻辑地址
	iRetCode = cpss_com_logic_addr_get(&stDstPid.stLogicAddr, &stDstPid.ulAddrFlag);

	if(CPSS_OK != iRetCode)
	{
		printf("cpss_com_logic_addr_get return ERROR. File: %s, Line: %d.\n",__FILE__,__LINE__);
		return;
	}

	stDstPid.ulPd = MT_MAIN_PROC;

	while(1)
	{
		ilRcvLen = recvfrom(glSockId, aucRcvBuf, MT_RECV_MSG_BUF_LEN, 0, (struct sockaddr*)&stSrcAddr, &ulSrcAddrLen);

		if(ilRcvLen <= 0)
		{
			gstRcvSndMsfCnt.ulRcvfrmMsgErrCnt++;
			printf("\nMT_MsgDispctcher():recvfrom() failure! ilRcvLen(%d) <= 0\n,File: %s,Line: %d.\n",ilRcvLen,__FILE__,__LINE__);
		}
		else
		{
			gstRcvSndMsfCnt.ulRcvfrmMsgCnt++;
			cpss_mem_memset(&stDisptchrMsg,0,sizeof(MT_DISPATCHER_SND_MSG_T));

			/*GET SocketMsgType*/
			if(IP_ADDR_TYPE_V4 == gulIpAddrFlag)
	        {
				ulSuffix = 0;
				cpss_mem_memcpy(&stIpv4Hdr,&aucRcvBuf[ulSuffix],MT_IPV4_HEADER_LEN);
				ulSuffix = ulSuffix + MT_IPV4_HEADER_LEN;
				cpss_mem_memcpy(&stUdpHdr,&aucRcvBuf[ulSuffix],MT_UDP_HEADER_LEN);
				ulSuffix = ulSuffix + MT_UDP_HEADER_LEN;

				if(ilRcvLen >= ulSuffix)
				{
					stDisptchrMsg.ulCapwapBufLen = ilRcvLen - ulSuffix;
					if(stDisptchrMsg.ulCapwapBufLen < MT_CAPWAP_BUF_LEN)
					{
						cpss_mem_memcpy(stDisptchrMsg.aucCapwapBuf,&aucRcvBuf[ulSuffix],stDisptchrMsg.ulCapwapBufLen);
					}
				}
	        }
			else//IPv6
	        {
				ulSuffix = 0;
				cpss_mem_memcpy(&stUdpHdr,&aucRcvBuf[ulSuffix],MT_UDP_HEADER_LEN);
				ulSuffix = ulSuffix + MT_UDP_HEADER_LEN;

				if(ilRcvLen >= ulSuffix)
				{
					stDisptchrMsg.ulCapwapBufLen = ilRcvLen - ulSuffix;

					if(stDisptchrMsg.ulCapwapBufLen < MT_CAPWAP_BUF_LEN)
					{
						cpss_mem_memcpy(stDisptchrMsg.aucCapwapBuf,&aucRcvBuf[ulSuffix],stDisptchrMsg.ulCapwapBufLen);
					}
				}
	        }

			ulUdpSrcPrt = htons(stUdpHdr.usSrcPort);
			ulUdpDstPrt = htons(stUdpHdr.usDstPort);

			switch(ulUdpSrcPrt)
			{
			case MT_CAPWAP_PORT_5246:
			{
				stDisptchrMsg.ulApIdx = MT_GetApIndexFromCpmPort(ulUdpDstPrt);

				if(MT_UNDEFINED_OBJECT_IDENTIFIER == stDisptchrMsg.ulApIdx)
				{
					break;
				}

				if(0 == stDisptchrMsg.ulCapwapBufLen)
				{
					break;
				}

				ulComSndMsgLen = sizeof(stDisptchrMsg.ulApIdx) + sizeof(stDisptchrMsg.ulCapwapBufLen) + stDisptchrMsg.ulCapwapBufLen;
				ulMsgId = MT_SOCKET_DATA_CPM_IND_MSG;

				iRetCode = cpss_com_send(&stDstPid, ulMsgId, (UINT8*)&stDisptchrMsg, ulComSndMsgLen);

				if(CPSS_OK == iRetCode)
				{
					gstRcvSndMsfCnt.ulComSnd2ApMsgCnt++;
				}
				else
				{
					gstRcvSndMsfCnt.ulComSnd2ApMsgErrCnt++;
				}
			}
			break;

			case MT_CAPWAP_PORT_5247:
			{
				stDisptchrMsg.ulApIdx = MT_GetApIndexFromUpmPort(ulUdpDstPrt);

				if(MT_UNDEFINED_OBJECT_IDENTIFIER == stDisptchrMsg.ulApIdx)
				{
					break;
				}

				if(0 == stDisptchrMsg.ulCapwapBufLen)
				{
					break;
				}

				ulComSndMsgLen = sizeof(stDisptchrMsg.ulApIdx) + sizeof(stDisptchrMsg.ulCapwapBufLen) + stDisptchrMsg.ulCapwapBufLen;
				ulMsgId = MT_SOCKET_DATA_UPM_IND_MSG;

				iRetCode = cpss_com_send(&stDstPid, ulMsgId, (UINT8*)&stDisptchrMsg, ulComSndMsgLen);

				if(CPSS_OK == iRetCode)
				{
					gstRcvSndMsfCnt.ulComSnd2StaMsgCnt++;
				}
				else
				{
					gstRcvSndMsfCnt.ulComSnd2StaMsgErrCnt++;
				}
			}
			break;

			default:
			{
				gstRcvSndMsfCnt.ulOtherMsgFrmDispchrCnt++;
				stDisptchrMsg.ulApIdx = MT_UNDEFINED_OBJECT_IDENTIFIER;
			}
			break;
			}
		}
	}
}


//AP给AC模块的消息发送函数
MTBool MT_SendApMsg2AC(UINT32 vulApIndex,MTSocket vCwSock, UINT8 *vaucCapwapBuf, UINT32 vulCapwaplen)
{
	INT32 ilRetCode = 0;
	UINT8 aucBuffer[MT_BUFF_LEN] = {0};
	UINT32 ulBufLen = 0;
	MT_IPV4_HEADER_T stIPv4Hdr;
	MT_UDP_HDR_T stUdpHdr;
	struct sockaddr_in stDestAddr;
	struct sockaddr_in6 stDestAddrv6;
	struct sockaddr_in6 stSrcAddrv6;
	UINT32 ulLoop = 0;
	UINT32 ulSuffix = 0;
	UINT16 usCpmSrcPort = 0;
	MTSocket ilIPv6Sock = 0;

	cpss_mem_memset(&stIPv4Hdr,0,sizeof(stIPv4Hdr));
	cpss_mem_memset(&stUdpHdr,0,sizeof(stUdpHdr));
	cpss_mem_memset(&stDestAddr,0,sizeof(stDestAddr));
	cpss_mem_memset(&stDestAddrv6,0,sizeof(stDestAddrv6));
	cpss_mem_memset(&stSrcAddrv6,0,sizeof(stSrcAddrv6));

	if(vulApIndex < 0 || vulApIndex >= gulCmdStartApNum)
	{
		return MT_FALSE;
	}

	if (NULL == vaucCapwapBuf)
	{
		printf("Error! The third Para buf is NULL.\n File: %s. Line: %d\n", __FILE__, __LINE__);
		return MT_FALSE;
	}

	if(IP_ADDR_TYPE_V4 == gulIpAddrFlag)
	{
		//UDP Header
		stUdpHdr.usSrcPort = MT_GetApCpmPort(vulApIndex);
		stUdpHdr.usDstPort = MT_CAPWAP_PORT_5246;
		stUdpHdr.usPldLen = sizeof(stUdpHdr) + vulCapwaplen;
		stUdpHdr.usChkSum = 0x0000;

		stUdpHdr.usSrcPort = htons(stUdpHdr.usSrcPort);
		stUdpHdr.usDstPort = htons(stUdpHdr.usDstPort);
		stUdpHdr.usPldLen = htons(stUdpHdr.usPldLen);
		stUdpHdr.usChkSum = htons(stUdpHdr.usChkSum);

		//IPv4Header
		stIPv4Hdr.ucIpVerType = 0x45;
		stIPv4Hdr.ucDiffSerFd = 0x00;
		stIPv4Hdr.usTotalLen = sizeof(stUdpHdr) + vulCapwaplen;
		stIPv4Hdr.usFragOffset = 0x0000;
		stIPv4Hdr.ucTtl = 64;
		stIPv4Hdr.ucProtoType = 0x11;
		stIPv4Hdr.usChkSum = 0x00;
		cpss_mem_memcpy(stIPv4Hdr.aucSrcIpaddr,gastAp[vulApIndex].aucApIpV4,MT_IPV4_ADDR_LEN);

		if(STATIC == gastApconf[vulApIndex].enDiscovType)
		{
			;
		}
		else if(DHCP == gastApconf[vulApIndex].enDiscovType)
		{
			cpss_mem_memcpy(stIPv4Hdr.aucDstIpAddr,gaucAcIpAddrV4,MT_IPV4_ADDR_LEN);
		}
		else if(DNS == gastApconf[vulApIndex].enDiscovType)
		{
			;
		}
		else if(BROADCAT == gastApconf[vulApIndex].enDiscovType)
		{
			for(ulLoop = 0; ulLoop < MT_IPV4_ADDR_LEN; ulLoop++)
			{
				stIPv4Hdr.aucDstIpAddr[ulLoop] = 0xFF;
			}
		}
		else if(RECNNCT == gastApconf[vulApIndex].enDiscovType)
		{
			cpss_mem_memcpy(stIPv4Hdr.aucDstIpAddr,gaucAcIpAddrV4,MT_IPV4_ADDR_LEN);
		}
		else
		{
			//printf("ERROR. AP(%d) Discover Type is unknown(%d).\nFile: %s,Line: %d.\n",vulApIndex,gastApconf[vulApIndex].enDiscovType,__FILE__,__LINE__);
			return MT_FALSE;
		}

		stIPv4Hdr.usChkSum = MT_CalcIpChksm((UINT8*)&stIPv4Hdr,sizeof(stIPv4Hdr));

		stIPv4Hdr.usTotalLen = htons(stIPv4Hdr.usTotalLen);
		stIPv4Hdr.usFragOffset = htons(stIPv4Hdr.usFragOffset);
		stIPv4Hdr.usChkSum = htons(stIPv4Hdr.usChkSum);

		//Buffer Data
		ulSuffix = 0;
		cpss_mem_memcpy(&aucBuffer[ulSuffix],&stIPv4Hdr,sizeof(stIPv4Hdr));
		ulSuffix = ulSuffix + sizeof(stIPv4Hdr);
		cpss_mem_memcpy(&aucBuffer[ulSuffix],&stUdpHdr,sizeof(stUdpHdr));
		ulSuffix = ulSuffix + sizeof(stUdpHdr);
		cpss_mem_memcpy(&aucBuffer[ulSuffix],vaucCapwapBuf,vulCapwaplen);
		ulSuffix = ulSuffix + vulCapwaplen;
		ulBufLen = ulSuffix;

		stDestAddr.sin_family = AF_INET;
		stDestAddr.sin_port = htons(MT_CAPWAP_PORT_5246);
		stDestAddr.sin_addr.s_addr = inet_addr((char*)gstAcConfInfo.aucAcIpv4AddStr);

		ilRetCode = sendto(vCwSock, aucBuffer, ulBufLen, 0, (const struct sockaddr *) &stDestAddr, sizeof(struct sockaddr));

		if(ilRetCode > 0)
		{
			gstRcvSndMsfCnt.ulAPSndtoAC_IPv4MsgCnt++;
		}
		else
		{
			gstRcvSndMsfCnt.ulAPSndtoAC_IPv4MsgErrCnt++;
		}

//		while(ilRetCode < 0)
//		{
//			perror("sendto");
//			if (errno == EINTR)
//			{
//				continue;
//			}
//			return MT_FALSE;
//		}
	}
	else
	{
		//IPv6
		ilIPv6Sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

		//AP Source IP Addr
		usCpmSrcPort = MT_GetApCpmPort(vulApIndex);

		stSrcAddrv6.sin6_family = AF_INET6;
		stSrcAddrv6.sin6_port = htons(usCpmSrcPort);
		cpss_mem_memcpy(&stSrcAddrv6.sin6_addr,gastAp[vulApIndex].aucApIpV6,MT_IPV6_ADDR_LEN);

		ilRetCode = bind(ilIPv6Sock, (struct sockaddr *) &stSrcAddrv6, sizeof(struct sockaddr_in6));

		if (ilRetCode < 0)
		{
			perror("bind");
			return MT_FALSE;
		}

		//Dest IP Addr
		stDestAddrv6.sin6_family = AF_INET6;
		stDestAddrv6.sin6_port = htons(MT_CAPWAP_PORT_5246);
		cpss_mem_memcpy(&stDestAddrv6.sin6_addr,gaucAcIpAddrV6,MT_IPV6_ADDR_LEN);

		ilRetCode = sendto(ilIPv6Sock, vaucCapwapBuf, vulCapwaplen, 0, (const struct sockaddr *) &stDestAddrv6, sizeof(struct sockaddr));
		//ilRetCode = sendto(vCwSock, ucBuffer, ulBufLen, 0, (const struct sockaddr *) &stDestAddrv6, sizeof(struct sockaddr));

		if(ilRetCode > 0)
		{
			gstRcvSndMsfCnt.ulAPSndtoAC_IPv6MsgCnt++;

		}
		else
		{
			gstRcvSndMsfCnt.ulAPSndtoAC_IPv6MsgErrCnt++;
		}

//		while(ilRetCode < 0)
//		{
//			perror("sendto");
//			if (errno == EINTR)
//			{
//				continue;
//			}
//			return MT_FALSE;
//		}
		close(ilIPv6Sock);
	}
	return MT_TRUE;
}

//STA给AC模块的消息发送函数
MTBool MT_SendStaMsg2AC(UINT32 vulApIndex,MTSocket vCwSock, UINT8 *vaucCapwapBuf, UINT32 vulCapwaplen)
{
	INT32 ilRetCode = 0;
	MT_IPV4_HEADER_T stIPv4Hdr;
	MT_UDP_HDR_T stUdpHdr;
	UINT8 aucBuffer[MT_BUFF_LEN] = {0};
	UINT32 ulBufLen = 0;
	struct sockaddr_in stDestAddr;
	struct sockaddr_in6 stDestAddrv6;
	struct sockaddr_in6 stSrcAddrv6;
	UINT32 ulLoop = 0;
	UINT32 ulSuffix = 0;
	UINT16 usUpmSrcPort = 0;
	MTSocket ilIPv6Sock = 0;
	cpss_mem_memset(&stIPv4Hdr,0,sizeof(stIPv4Hdr));
	cpss_mem_memset(&stUdpHdr,0,sizeof(stUdpHdr));
	cpss_mem_memset(&stDestAddr,0,sizeof(stDestAddr));
	cpss_mem_memset(&stDestAddrv6,0,sizeof(stDestAddrv6));
	cpss_mem_memset(&stSrcAddrv6,0,sizeof(stSrcAddrv6));

	if(vulApIndex < 0 || vulApIndex >= gulCmdStartApNum)
    {
		return MT_FALSE;
    }

	if (NULL == vaucCapwapBuf)
	{
		printf("Error! The forth Para buf is NULL.\nFile:%s.Line:%d\n", __FILE__, __LINE__);
		return MT_FALSE;
	}

	if(IP_ADDR_TYPE_V4 == gulIpAddrFlag)
	{
		//UDP Header
		stUdpHdr.usSrcPort = MT_GetApUpmPort(vulApIndex);
		stUdpHdr.usDstPort = MT_CAPWAP_PORT_5247;
		stUdpHdr.usPldLen = sizeof(stUdpHdr) + vulCapwaplen;
		stUdpHdr.usChkSum = 0x0000;

		stUdpHdr.usSrcPort = htons(stUdpHdr.usSrcPort);
		stUdpHdr.usDstPort = htons(stUdpHdr.usDstPort);
		stUdpHdr.usPldLen = htons(stUdpHdr.usPldLen);
		stUdpHdr.usChkSum = htons(stUdpHdr.usChkSum);

		//IPv4Header
		stIPv4Hdr.ucIpVerType = 0x45;
		stIPv4Hdr.ucDiffSerFd = 0x00;
		stIPv4Hdr.usTotalLen = sizeof(stUdpHdr) + vulCapwaplen;
		stIPv4Hdr.usFragOffset = 0x0000;
		stIPv4Hdr.ucTtl = 64;
		stIPv4Hdr.ucProtoType = 0x11;
		stIPv4Hdr.usChkSum = 0x00;

		cpss_mem_memcpy(stIPv4Hdr.aucSrcIpaddr,gastAp[vulApIndex].aucApIpV4,MT_IPV4_ADDR_LEN);

		if(STATIC == gastApconf[vulApIndex].enDiscovType)
		{
			;
		}
		else if(DHCP == gastApconf[vulApIndex].enDiscovType)
		{
			cpss_mem_memcpy(stIPv4Hdr.aucDstIpAddr,gaucAcIpAddrV4,MT_IPV4_ADDR_LEN);
		}
		else if(DNS == gastApconf[vulApIndex].enDiscovType)
		{
			;
		}
		else if(BROADCAT == gastApconf[vulApIndex].enDiscovType)
		{
			for(ulLoop = 0; ulLoop < MT_IPV4_ADDR_LEN; ulLoop++)
			{
				stIPv4Hdr.aucDstIpAddr[ulLoop] = 0xFF;
			}
		}
		else if(RECNNCT == gastApconf[vulApIndex].enDiscovType)
		{
			cpss_mem_memcpy(stIPv4Hdr.aucDstIpAddr,gaucAcIpAddrV4,MT_IPV4_ADDR_LEN);
		}
		else
		{
			return MT_FALSE;
		}

		stIPv4Hdr.usChkSum = MT_CalcIpChksm((UINT8*)&stIPv4Hdr,sizeof(stIPv4Hdr));

		stIPv4Hdr.usTotalLen = htons(stIPv4Hdr.usTotalLen);
		stIPv4Hdr.usFragOffset = htons(stIPv4Hdr.usFragOffset);
		stIPv4Hdr.usChkSum = htons(stIPv4Hdr.usChkSum);

		//Buffer Data
		ulSuffix = 0;
		cpss_mem_memcpy(&aucBuffer[ulSuffix],&stIPv4Hdr,sizeof(stIPv4Hdr));
		ulSuffix = ulSuffix + sizeof(stIPv4Hdr);
		cpss_mem_memcpy(&aucBuffer[ulSuffix],&stUdpHdr,sizeof(stUdpHdr));
		ulSuffix = ulSuffix + sizeof(stUdpHdr);
		cpss_mem_memcpy(&aucBuffer[ulSuffix],vaucCapwapBuf,vulCapwaplen);
		ulSuffix = ulSuffix + vulCapwaplen;
		ulBufLen = ulSuffix;

		stDestAddr.sin_family = AF_INET;
		stDestAddr.sin_port = htons(MT_CAPWAP_PORT_5247);
		stDestAddr.sin_addr.s_addr = inet_addr((char*)gstAcConfInfo.aucAcIpv4AddStr);

		ilRetCode = sendto(vCwSock, aucBuffer, ulBufLen, 0, (const struct sockaddr *) &stDestAddr, sizeof(struct sockaddr));

		if(ilRetCode > 0)
		{
			gstRcvSndMsfCnt.ulStaSndtoAC_IPv4MsgCnt++;
		}
		else
		{
			gstRcvSndMsfCnt.ulStaSndtoAC_IPv4MsgErrCnt++;
		}

//		while(ilRetCode < 0)
//		{
//			perror("sendto");
//
//			if (errno == EINTR)
//			{
//				continue;
//			}
//			return MT_FALSE;
//		}
	}
	else
	{
		//IPv6
		ilIPv6Sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

		//AP Source IP Addr
		usUpmSrcPort = MT_GetApUpmPort(vulApIndex);

		stSrcAddrv6.sin6_family = AF_INET6;
		stSrcAddrv6.sin6_port = htons(usUpmSrcPort);
		cpss_mem_memcpy(&stSrcAddrv6.sin6_addr,gastAp[vulApIndex].aucApIpV6,MT_IPV6_ADDR_LEN);

		ilRetCode = bind(ilIPv6Sock, (struct sockaddr *) &stSrcAddrv6, sizeof(struct sockaddr_in6));

		if (ilRetCode < 0)
		{
			perror("bind");
			return MT_FALSE;
		}

		//Dest ADDR
		stDestAddrv6.sin6_family = AF_INET6;
		stDestAddrv6.sin6_port = htons(MT_CAPWAP_PORT_5247);
		cpss_mem_memcpy(&stDestAddrv6.sin6_addr,gaucAcIpAddrV6,MT_IPV6_ADDR_LEN);

		ilRetCode = sendto(ilIPv6Sock, vaucCapwapBuf, vulCapwaplen, 0, (const struct sockaddr *) &stDestAddrv6, sizeof(struct sockaddr));

		if(ilRetCode > 0)
		{
			gstRcvSndMsfCnt.ulStaSndtoAC_IPv6MsgCnt++;
		}
		else
		{
			gstRcvSndMsfCnt.ulStaSndtoAC_IPv6MsgErrCnt++;
		}

//		while(ilRetCode < 0)
//		{
//			perror("sendto");
//			if (errno == EINTR)
//			{
//				continue;
//			}
//			return MT_FALSE;
//		}
		close(ilIPv6Sock);
	}
	return MT_TRUE;
}

//AP给UPM模块的消息发送函数
MTBool MT_ApSendMsg2DhcpSrv(UINT32 vulApIndex,MTSocket vCwSock, UINT8 *vucBtStrapBuf, UINT32 vulBtStrapBuflen)
{
  INT32 ilRetCode = 0;
  MT_IPV4_HEADER_T stIPv4Hdr;
  MT_UDP_HDR_T stUdpHdr;
  UINT8 aucBuffer[1024] = {0};
  UINT32 ulBufLen = 0;
  struct sockaddr_in stDestAddr;
  struct sockaddr_in6 stDestAddrv6;
  struct sockaddr_in6 stSrcAddrv6;
  UINT32 ulLoop = 0;
  UINT32 ulSuffix = 0;
  MTSocket ilIPv6Sock = 0;

  cpss_mem_memset(&stIPv4Hdr,0,sizeof(stIPv4Hdr));
  cpss_mem_memset(&stUdpHdr,0,sizeof(stUdpHdr));
  cpss_mem_memset(&stDestAddr,0,sizeof(stDestAddr));
  cpss_mem_memset(&stDestAddrv6,0,sizeof(stDestAddrv6));
  cpss_mem_memset(&stSrcAddrv6,0,sizeof(stSrcAddrv6));

  if(vulApIndex < 0 || vulApIndex >= gulCmdStartApNum)
     {
	  return MT_FALSE;
     }

  if (NULL == vucBtStrapBuf)
    {
	  printf("Error! The forth Para buf is NULL.\nFile:%s.Line:%d\n", __FILE__, __LINE__);
	  return MT_FALSE;
    }

  if(IP_ADDR_TYPE_V4 == gulIpAddrFlag)
    {
	  //UDP Header
	  stUdpHdr.usSrcPort = MT_DHCP_SRC_PORT;
	  stUdpHdr.usDstPort = MT_DHCP_DST_PORT;
	  stUdpHdr.usPldLen = sizeof(stUdpHdr) + vulBtStrapBuflen;
	  stUdpHdr.usChkSum = 0x0000;

	  stUdpHdr.usSrcPort = htons(stUdpHdr.usSrcPort);
	  stUdpHdr.usDstPort = htons(stUdpHdr.usDstPort);
	  stUdpHdr.usPldLen = htons(stUdpHdr.usPldLen);
	  stUdpHdr.usChkSum = htons(stUdpHdr.usChkSum);

	  //IPv4Header
	  stIPv4Hdr.ucIpVerType = 0x45;
	  stIPv4Hdr.ucDiffSerFd = 0x00;
	  stIPv4Hdr.usTotalLen = sizeof(stUdpHdr) + vulBtStrapBuflen;
	  stIPv4Hdr.usFragOffset = 0x0000;
	  stIPv4Hdr.ucTtl = 128;
	  stIPv4Hdr.ucProtoType = 0x11;
	  stIPv4Hdr.usChkSum = 0x00;

	  for(ulLoop = 0; ulLoop < MT_IPV4_ADDR_LEN; ulLoop++)
		 {
		  stIPv4Hdr.aucSrcIpaddr[ulLoop] = 0x00;
		  stIPv4Hdr.aucDstIpAddr[ulLoop] = 0xFF;
		 }

	  stIPv4Hdr.usChkSum = MT_CalcIpChksm((UINT8*)&stIPv4Hdr,sizeof(stIPv4Hdr));

	  stIPv4Hdr.usTotalLen = htons(stIPv4Hdr.usTotalLen);
	  stIPv4Hdr.usFragOffset = htons(stIPv4Hdr.usFragOffset);
	  stIPv4Hdr.usChkSum = htons(stIPv4Hdr.usChkSum);

	  //Buffer Data
	  ulSuffix = 0;
	  cpss_mem_memcpy(&aucBuffer[ulSuffix],&stIPv4Hdr,sizeof(stIPv4Hdr));
	  ulSuffix = ulSuffix + sizeof(stIPv4Hdr);
	  cpss_mem_memcpy(&aucBuffer[ulSuffix],&stUdpHdr,sizeof(stUdpHdr));
	  ulSuffix = ulSuffix + sizeof(stUdpHdr);
	  cpss_mem_memcpy(&aucBuffer[ulSuffix],vucBtStrapBuf,vulBtStrapBuflen);
	  ulSuffix = ulSuffix + vulBtStrapBuflen;
	  ulBufLen = ulSuffix;

   stDestAddr.sin_family = AF_INET;
   stDestAddr.sin_port = htons(MT_DHCP_DST_PORT);
   stDestAddr.sin_addr.s_addr = inet_addr((char*)gstAcConfInfo.aucAcIpv4AddStr);

   ilRetCode = sendto(vCwSock, aucBuffer, ulBufLen, 0, (const struct sockaddr *) &stDestAddr, sizeof(struct sockaddr));

   while(ilRetCode < 0)
        {
	   perror("sendto");

	   if (errno == EINTR)
            {
		   continue;
            }
	   return MT_FALSE;
        }
    }
  else//IPv6
    {
    ilIPv6Sock = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);

    stSrcAddrv6.sin6_family = AF_INET6;
    stSrcAddrv6.sin6_port = htons(MT_DHCPV6_DST_PORT);
    cpss_mem_memcpy(&stSrcAddrv6.sin6_addr,gastAp[vulApIndex].aucApIpV6,MT_IPV6_ADDR_LEN);

    ilRetCode = bind(ilIPv6Sock, (struct sockaddr *) &stSrcAddrv6, sizeof(struct sockaddr_in6));

    if (ilRetCode < 0)
          {
      perror("bind");
#if 0
      printf("SrcAddrv6:\n");
      for(ulLoop = 0; ulLoop < MT_IPV6_ADDR_LEN;ulLoop++)
               {
        printf("%02x ",stSrcAddrv6.sin6_addr.in6_u.u6_addr8[ulLoop]);
               }
#endif
      return MT_FALSE;
          }

  //Dest ADDR
    stDestAddrv6.sin6_family = AF_INET6;
    stDestAddrv6.sin6_port = htons(MT_DHCPV6_DST_PORT);
    cpss_mem_memcpy(&stDestAddrv6.sin6_addr,gaucAcIpAddrV6,MT_IPV6_ADDR_LEN);

    ilRetCode = sendto(ilIPv6Sock, vucBtStrapBuf, vulBtStrapBuflen, 0, (const struct sockaddr *) &stDestAddrv6, sizeof(struct sockaddr));

    while(ilRetCode < 0)
          {
       perror("sendto");
       if (errno == EINTR)
                  {
    	    continue;
                   }
       return MT_FALSE;
          }
    close(ilIPv6Sock);
    }

#if 0
  printf("\nMT Send STA_MSG:\n");
  for(ulLoop = 0; ulLoop < ulBufLen; ulLoop++)
    {
    printf("%02x ",aucBuffer[ulLoop]);
    if(0 == (ulLoop+1)%16)
      printf("\n");
    }
  printf("\n");
#endif
  return MT_TRUE;
}
//将字符串格式的IP地址转换成数组格式，vpIpStr为输入参数，aucIpAddr为输出参数

//inet_pton()
void GetIpFromStr(UINT32 vulAfInet,UINT8 *vpIpStr,UINT8 *vaucIpAddr)
{
  if (NULL == vpIpStr)
    {
      printf("ERROR! IP_STR is NULL.\nFile:%s Line:%d", __FILE__, __LINE__);
      return;
    }

  inet_pton(vulAfInet,(char*)vpIpStr,vaucIpAddr);
}

//将MAC数据转换成字符
void TransMacToStr(const unsigned char *vpMac, char *vpMacStr)
{
  if (NULL != vpMac)
    {
	  sprintf(vpMacStr, "%02x:%02x:%02x:%02x:%02x:%02x", (unsigned char) vpMac[0],
          (unsigned char) vpMac[1], (unsigned char) vpMac[2],
          (unsigned char) vpMac[3], (unsigned char) vpMac[4],
          (unsigned char) vpMac[5]);
    }
  else
    {
	  strcpy(vpMacStr, "00:00:00:00:00:00");
    }
}

UINT8 GetHexValFromChar(char vChar)
{
  if (vChar >= '0' && vChar <= '9')
    {
	  return vChar -= '0';
    }
  if (vChar >= 'a' && vChar <= 'f')
    {
	  return vChar -= 'a' - 10;
    }
  if (vChar >= 'A' && vChar <= 'F')
    {
	  return vChar -= 'A' - 10;
    }
  return 0;
}

/*将字符串转换成MAC地址*/
/*AA:BB:CC:DD:EE:FF*/

INT32 GetMacFromStr(const UINT8 *vpMacStr, UINT8 *vaucMac)
{
  UINT32 ulLoop = 0;

  if(NULL == vpMacStr)
    {
    printf("ERROR. The First Input Para is NULL.\nFile:%s.Line:%d\n",__FILE__,__LINE__);
    return -1;
    }

  if (strlen((char*)vpMacStr) < 17)/*AA:BB:CC:DD:EE:FF*/
    {
	  printf("Input parameter mac len is too short.\nFile:%s.Line:%d\n",__FILE__,__LINE__);
	  return -1;
    }

  for (ulLoop = 0; ulLoop < 17; ulLoop++)
    {
	  if(ulLoop == 2 || ulLoop == 5 || ulLoop == 8 || ulLoop == 11 || ulLoop == 14)
         {
		  if (vpMacStr[ulLoop] != ':')
             {
			  printf("Input parameter mac included invalid char.\nFile:%s.Line:%d\n",__FILE__,__LINE__);
			  return -1;
            }
        }
	  else
         {
		  if((vpMacStr[ulLoop] < '0' || vpMacStr[ulLoop] > '9') && (vpMacStr[ulLoop] < 'a' || vpMacStr[ulLoop] > 'f') && (vpMacStr[ulLoop] < 'A' || vpMacStr[ulLoop] > 'F'))
             {
			  printf("Input parameter mac included invalid char.\nFile:%s.Line:%d\n",__FILE__,__LINE__);
			  return -1;
            }
        }
    }

  vaucMac[0] = (GetHexValFromChar(vpMacStr[0]) << 4) + (GetHexValFromChar(vpMacStr[1]));
  vaucMac[1] = (GetHexValFromChar(vpMacStr[3]) << 4) + (GetHexValFromChar(vpMacStr[4]));
  vaucMac[2] = (GetHexValFromChar(vpMacStr[6]) << 4) + (GetHexValFromChar(vpMacStr[7]));
  vaucMac[3] = (GetHexValFromChar(vpMacStr[9]) << 4) + (GetHexValFromChar(vpMacStr[10]));
  vaucMac[4] = (GetHexValFromChar(vpMacStr[12]) << 4) + (GetHexValFromChar(vpMacStr[13]));
  vaucMac[5] = (GetHexValFromChar(vpMacStr[15]) << 4) + (GetHexValFromChar(vpMacStr[16]));
  return 0;
}

UINT8* MT_GetStaAuthStateStr(UINT32 vulStaAuthStateVal)
{
  UINT8 aucStateStr[32] = {'\0'};

  switch(vulStaAuthStateVal)
  {
  case AUTH_INIT_STATE:
    strcpy((char*)aucStateStr,"STA_INIT_STATE");
    break;

  case WAIT_80211_AUTH_RSP_STATE:
    strcpy((char*)aucStateStr,"WAIT_80211_AUTH_RSP_STATE");
    break;

  case WAIT_80211_ASSOC_RSP_STATE:
    strcpy((char*)aucStateStr,"WAIT_80211_ASSOC_RSP_STATE");
    break;

  case ASSOCIATION_SUCESS_STATE:
    strcpy((char*)aucStateStr,"ASSOCIATION_SUCESS_STATE");
    break;

  case ASSOCIATION_FAIL:
	  strcpy((char*)aucStateStr,"ASSOCIATION_FAIL");
	  break;

   default:
    strcpy((char*)aucStateStr,"UNKNOWN_AUTH_STATE");
    break;
  }
  return aucStateStr;
}

UINT8* MT_GetStaHOStateStr(UINT32 vulStaHoStateVal)
{
  UINT8 aucStateStr[MT_STATE_BUF_LEN] = {'\0'};

  switch(vulStaHoStateVal)
  {
  case HANDOVER_INIT_STATE:
	  strcpy((char*)aucStateStr,"HANDOVER_INIT_STATE");
	  break;

  case WAIT_HDOVER_AUTH_RSP_STATE:
	  strcpy((char*)aucStateStr,"WAIT_HDOVER_AUTH_RSP_STATE");
	  break;

  case WAIT_HDOVER_REASSOC_RSP_STATE:
	  strcpy((char*)aucStateStr,"WAIT_HDOVER_REASSOC_RSP_STATE");
	  break;

  case HANDOVER_SUCCESS:
	  strcpy((char*)aucStateStr,"HANDOVER_SUCCESS");
	  break;

  case HANDOVER_FAIL:
	  strcpy((char*)aucStateStr,"HANDOVER_FAIL");
	  break;

  default:
    strcpy((char*)aucStateStr,"UNKNOWN_HANDOVER_STATE");
    break;
  }
  return aucStateStr;
}

UINT8* MT_GetApStateStr(UINT32 vulApStateVal)
{
  UINT8 aucApteStr[32] = {'\0'};

  switch(vulApStateVal)
  {
  case INIT_STATE:
    strcpy((char*)aucApteStr,"INIT_STATE");
  break;

  case WAIT_DISCOVER_RSP_STATE:
    strcpy((char*)aucApteStr,"WAIT_DISCOVER_RSP_STATE");
  break;

  case WAIT_JOIN_RSP_STATE:
    strcpy((char*)aucApteStr,"WAIT_JOIN_RSP_STATE");
  break;

  case WAIT_CONFIG_STATUS_RSP_STATE:
    strcpy((char*)aucApteStr,"WAIT_CONFIG_STATUS_RSP_STATE");
  break;

  case WAIT_CHANGE_STATE_RSP_STATE:
    strcpy((char*)aucApteStr,"WAIT_CHANGE_STATE_RSP_STATE");
  break;

  case RUN_STATE:
    strcpy((char*)aucApteStr,"RUN_STATE");
  break;

  default://UNKNOWN_AP_STATE
    strcpy((char*)aucApteStr,"UNKNOWN_AP_STATE");
  break;
  }
  return aucApteStr;
}

void MT_GetCapwapMsg(UINT8 *vpMsgPathFileName, MT_CAPWAPMSG_T *vpstCapwapMsg)
{
 UINT32 retCode = 0;
 FILE *pFileStream = NULL;
 UINT32 ulLoop = 0;

 if(NULL ==vpMsgPathFileName)
  {
  printf("ERROR. The first Para is NULL.\nFile:%s,Line:%d.\n",__FILE__,__LINE__);
  return;
  }

 pFileStream = fopen((char*)vpMsgPathFileName,"r");

 if(pFileStream == NULL )
  {
  printf("%s was not opened\n",vpMsgPathFileName);
  return;
  }
 fseek(pFileStream, 0L, SEEK_SET );

 while(retCode != EOF )
  {

  retCode = fscanf(pFileStream, "%02x",&vpstCapwapMsg->aucBuff[ulLoop++]);
   }

 vpstCapwapMsg->ulBufLen = ulLoop - 1;

 fclose(pFileStream);
}

void showstart(void)
{
  gbShowFlag = MT_TRUE;//周期性统计信息打印开关
}

void showstop(void)
{
  gbShowFlag = MT_FALSE;//周期性统计信息打印开关
}

void wtpeventstart(UINT32 vulL2IeId)
{
	UINT8 ucIE56SubIEFlag = 0;
	UINT8 ucIE57SubIEFlag = 0;

  if(0 == vulL2IeId)
    {
    gucWtpEventFlag = MT_WTP_EVENT_DISABLE;

    gstWtpEventFlag.ucFlagL2IeId_5604 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5621 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5622 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5623 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5624 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5625 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5626 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5627 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5628 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5629 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5630 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5631 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5632 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5633 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5634 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5635 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5636 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5637 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5638 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5639 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5640 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5641 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5642 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5643 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5644 = MT_WTP_EVENT_L2IE_DISABLE;

    gstWtpEventFlag.ucFlagL2IeId_5721 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5722 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5723 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5724 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5725 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5726 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5727 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5728 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5729 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5730 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5731 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5732 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5733 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5734 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5735 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5736 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5737 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5738 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5739 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5740 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5741 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5742 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5743 = MT_WTP_EVENT_L2IE_DISABLE;

    gstWtpEventFlag.ucFlagL2IeId_58 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_59 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_60 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_61 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_62 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_63 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_64 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_65 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_66 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_67 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_68 = MT_WTP_EVENT_L2IE_DISABLE;
    //gstWtpEventFlag.ucFlagL2IeId_69 = MT_WTP_EVENT_L2IE_DISABLE;
    //gstWtpEventFlag.ucFlagL2IeId_70 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_71 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_72 = MT_WTP_EVENT_L2IE_DISABLE;
    //gstWtpEventFlag.ucFlagL2IeId_73 = MT_WTP_EVENT_L2IE_DISABLE;
    //gstWtpEventFlag.ucFlagL2IeId_74 = MT_WTP_EVENT_L2IE_DISABLE;
    //gstWtpEventFlag.ucFlagL2IeId_75 = MT_WTP_EVENT_L2IE_DISABLE;
    //gstWtpEventFlag.ucFlagL2IeId_76 = MT_WTP_EVENT_L2IE_DISABLE;
    //gstWtpEventFlag.ucFlagL2IeId_77 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_78 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_79 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_80 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_81 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_82 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_83 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_84 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_85 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_86 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_87 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_88 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_107 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_133 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_134 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_138 = MT_WTP_EVENT_L2IE_DISABLE;

    return;
    }
  else if(1 == vulL2IeId)
    {
    gucWtpEventFlag = MT_WTP_EVENT_ENABLE;

    //IE56
    ucIE56SubIEFlag = random() % 20;
    switch(ucIE56SubIEFlag)
    {
    	case 0:
    		gstWtpEventFlag.ucFlagL2IeId_5604 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 1:
    	gstWtpEventFlag.ucFlagL2IeId_5621 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 2:
    	gstWtpEventFlag.ucFlagL2IeId_5622 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 3:
    	gstWtpEventFlag.ucFlagL2IeId_5623 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 4:
    	gstWtpEventFlag.ucFlagL2IeId_5624 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 5:
    	gstWtpEventFlag.ucFlagL2IeId_5625 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 6:
    	gstWtpEventFlag.ucFlagL2IeId_5626 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 7:
    	gstWtpEventFlag.ucFlagL2IeId_5627 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 8:
    	gstWtpEventFlag.ucFlagL2IeId_5628 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 9:
    	gstWtpEventFlag.ucFlagL2IeId_5629 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 10:
    	//gstWtpEventFlag.ucFlagL2IeId_5630 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 11:
    	//gstWtpEventFlag.ucFlagL2IeId_5631 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 12:
    	//gstWtpEventFlag.ucFlagL2IeId_5632 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 13:
    	//gstWtpEventFlag.ucFlagL2IeId_5633 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 14:
    	gstWtpEventFlag.ucFlagL2IeId_5634 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 15:
    	gstWtpEventFlag.ucFlagL2IeId_5635 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 16:
    	gstWtpEventFlag.ucFlagL2IeId_5638 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 17:
    	gstWtpEventFlag.ucFlagL2IeId_5639 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 18:
    	gstWtpEventFlag.ucFlagL2IeId_5643 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 19:
    	gstWtpEventFlag.ucFlagL2IeId_5644 = MT_WTP_EVENT_L2IE_ENABLE;

    default:
    	break;
    }

//    gstWtpEventFlag.ucFlagL2IeId_5640 = MT_WTP_EVENT_L2IE_ENABLE;
//    gstWtpEventFlag.ucFlagL2IeId_5641 = MT_WTP_EVENT_L2IE_ENABLE;
//    gstWtpEventFlag.ucFlagL2IeId_5642 = MT_WTP_EVENT_L2IE_ENABLE;

    gstWtpEventFlag.ucFlagL2IeId_5636 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_5637 = MT_WTP_EVENT_L2IE_ENABLE;

    //IE57
    ucIE57SubIEFlag = random() % 22;
    switch(ucIE57SubIEFlag)
    {
    case 0:
    	gstWtpEventFlag.ucFlagL2IeId_5721 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 1:
    	gstWtpEventFlag.ucFlagL2IeId_5722 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 2:
    	gstWtpEventFlag.ucFlagL2IeId_5723 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 3:
    	gstWtpEventFlag.ucFlagL2IeId_5724 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 4:
    	gstWtpEventFlag.ucFlagL2IeId_5725 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 5:
    	gstWtpEventFlag.ucFlagL2IeId_5726 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 6:
    	gstWtpEventFlag.ucFlagL2IeId_5727 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 7:
    	gstWtpEventFlag.ucFlagL2IeId_5728 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 8:
    	gstWtpEventFlag.ucFlagL2IeId_5729 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 9:
    	gstWtpEventFlag.ucFlagL2IeId_5730 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 10:
    	gstWtpEventFlag.ucFlagL2IeId_5731 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 11:
    	gstWtpEventFlag.ucFlagL2IeId_5732 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 12:
    	gstWtpEventFlag.ucFlagL2IeId_5733 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 13:
    	gstWtpEventFlag.ucFlagL2IeId_5735 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 14:
    	gstWtpEventFlag.ucFlagL2IeId_5736 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 15:
    	gstWtpEventFlag.ucFlagL2IeId_5737 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 16:
    	gstWtpEventFlag.ucFlagL2IeId_5738 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 17:
    	gstWtpEventFlag.ucFlagL2IeId_5739 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 18:
    	gstWtpEventFlag.ucFlagL2IeId_5740 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 19:
    	gstWtpEventFlag.ucFlagL2IeId_5741 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 20:
    	gstWtpEventFlag.ucFlagL2IeId_5742 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    case 21:
    	gstWtpEventFlag.ucFlagL2IeId_5743 = MT_WTP_EVENT_L2IE_ENABLE;
    	break;

    default:
    	break;
    }

    gstWtpEventFlag.ucFlagL2IeId_5734 = MT_WTP_EVENT_L2IE_ENABLE;

    gstWtpEventFlag.ucFlagL2IeId_58 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_59 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_60 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_61 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_62 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_63 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_64 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_65 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_66 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_67 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_68 = MT_WTP_EVENT_L2IE_ENABLE;
    //gstWtpEventFlag.ucFlagL2IeId_69 = MT_WTP_EVENT_L2IE_ENABLE;
    //gstWtpEventFlag.ucFlagL2IeId_70 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_71 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_72 = MT_WTP_EVENT_L2IE_ENABLE;
    //gstWtpEventFlag.ucFlagL2IeId_73 = MT_WTP_EVENT_L2IE_ENABLE;
    //gstWtpEventFlag.ucFlagL2IeId_74 = MT_WTP_EVENT_L2IE_ENABLE;
    //gstWtpEventFlag.ucFlagL2IeId_75 = MT_WTP_EVENT_L2IE_ENABLE;
    //gstWtpEventFlag.ucFlagL2IeId_76 = MT_WTP_EVENT_L2IE_ENABLE;
    //gstWtpEventFlag.ucFlagL2IeId_77 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_78 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_79 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_80 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_81 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_82 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_83 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_84 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_85 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_86 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_87 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_88 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_107 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_133 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_134 = MT_WTP_EVENT_L2IE_ENABLE;
    gstWtpEventFlag.ucFlagL2IeId_138 = MT_WTP_EVENT_L2IE_ENABLE;

    return;
    }

  gucWtpEventFlag = MT_WTP_EVENT_ENABLE;

  switch(vulL2IeId)
     {
  case 5604:
    gstWtpEventFlag.ucFlagL2IeId_5604 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5621:
    gstWtpEventFlag.ucFlagL2IeId_5621 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5622:
    gstWtpEventFlag.ucFlagL2IeId_5622 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5623:
    gstWtpEventFlag.ucFlagL2IeId_5623 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5624:
    gstWtpEventFlag.ucFlagL2IeId_5624 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5625:
    gstWtpEventFlag.ucFlagL2IeId_5625 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5626:
    gstWtpEventFlag.ucFlagL2IeId_5626 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5627:
    gstWtpEventFlag.ucFlagL2IeId_5627 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5628:
    gstWtpEventFlag.ucFlagL2IeId_5628 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5629:
    gstWtpEventFlag.ucFlagL2IeId_5629 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5630:
    gstWtpEventFlag.ucFlagL2IeId_5630 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5631:
    gstWtpEventFlag.ucFlagL2IeId_5631 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5632:
    gstWtpEventFlag.ucFlagL2IeId_5632 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5633:
    gstWtpEventFlag.ucFlagL2IeId_5633 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5634:
    gstWtpEventFlag.ucFlagL2IeId_5634 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5635:
    gstWtpEventFlag.ucFlagL2IeId_5635 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5636:
    gstWtpEventFlag.ucFlagL2IeId_5636 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5637:
    gstWtpEventFlag.ucFlagL2IeId_5637 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5638:
    gstWtpEventFlag.ucFlagL2IeId_5638 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5639:
    gstWtpEventFlag.ucFlagL2IeId_5639 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5640:
//    gstWtpEventFlag.ucFlagL2IeId_5640 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5641:
//    gstWtpEventFlag.ucFlagL2IeId_5641 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5642:
//    gstWtpEventFlag.ucFlagL2IeId_5642 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5643:
    gstWtpEventFlag.ucFlagL2IeId_5643 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5644:
    gstWtpEventFlag.ucFlagL2IeId_5644 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5721:
    gstWtpEventFlag.ucFlagL2IeId_5721 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5722:
    gstWtpEventFlag.ucFlagL2IeId_5722 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5723:
    gstWtpEventFlag.ucFlagL2IeId_5723 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5724:
    gstWtpEventFlag.ucFlagL2IeId_5724 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5725:
    gstWtpEventFlag.ucFlagL2IeId_5725 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5726:
    gstWtpEventFlag.ucFlagL2IeId_5726 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5727:
    gstWtpEventFlag.ucFlagL2IeId_5727 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5728:
    gstWtpEventFlag.ucFlagL2IeId_5728 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5729:
    gstWtpEventFlag.ucFlagL2IeId_5729 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5730:
    gstWtpEventFlag.ucFlagL2IeId_5730 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5731:
    gstWtpEventFlag.ucFlagL2IeId_5731 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5732:
    gstWtpEventFlag.ucFlagL2IeId_5732 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5733:
    gstWtpEventFlag.ucFlagL2IeId_5733 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5734:
    gstWtpEventFlag.ucFlagL2IeId_5734 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5735:
    gstWtpEventFlag.ucFlagL2IeId_5735 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5736:
    gstWtpEventFlag.ucFlagL2IeId_5736 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5737:
    gstWtpEventFlag.ucFlagL2IeId_5737 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5738:
    gstWtpEventFlag.ucFlagL2IeId_5738 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5739:
    gstWtpEventFlag.ucFlagL2IeId_5739 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5740:
    gstWtpEventFlag.ucFlagL2IeId_5740 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5741:
    gstWtpEventFlag.ucFlagL2IeId_5741 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5742:
    gstWtpEventFlag.ucFlagL2IeId_5742 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 5743:
    //gstWtpEventFlag.ucFlagL2IeId_5743 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 58:
    gstWtpEventFlag.ucFlagL2IeId_58 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 59:
    gstWtpEventFlag.ucFlagL2IeId_59 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 60:
    gstWtpEventFlag.ucFlagL2IeId_60 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 61:
    gstWtpEventFlag.ucFlagL2IeId_61 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 62:
    gstWtpEventFlag.ucFlagL2IeId_62 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 63:
    gstWtpEventFlag.ucFlagL2IeId_63 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 64:
    gstWtpEventFlag.ucFlagL2IeId_64 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 65:
    gstWtpEventFlag.ucFlagL2IeId_65 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 66:
    gstWtpEventFlag.ucFlagL2IeId_66 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 67:
    gstWtpEventFlag.ucFlagL2IeId_67 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 68:
    gstWtpEventFlag.ucFlagL2IeId_68 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 69:
    //gstWtpEventFlag.ucFlagL2IeId_69 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 70:
    //gstWtpEventFlag.ucFlagL2IeId_70 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 71:
    gstWtpEventFlag.ucFlagL2IeId_71 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 72:
    gstWtpEventFlag.ucFlagL2IeId_72 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 73:
    //gstWtpEventFlag.ucFlagL2IeId_73 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 74:
    //gstWtpEventFlag.ucFlagL2IeId_74 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 75:
    //gstWtpEventFlag.ucFlagL2IeId_75 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 76:
    //gstWtpEventFlag.ucFlagL2IeId_76 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 77:
    //gstWtpEventFlag.ucFlagL2IeId_77 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 78:
    gstWtpEventFlag.ucFlagL2IeId_78 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 79:
    gstWtpEventFlag.ucFlagL2IeId_79 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 80:
    gstWtpEventFlag.ucFlagL2IeId_80 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 81:
    gstWtpEventFlag.ucFlagL2IeId_81 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 82:
    gstWtpEventFlag.ucFlagL2IeId_82 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 83:
    gstWtpEventFlag.ucFlagL2IeId_83 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 84:
    gstWtpEventFlag.ucFlagL2IeId_84 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 85:
    gstWtpEventFlag.ucFlagL2IeId_85 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 86:
    gstWtpEventFlag.ucFlagL2IeId_86 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 87:
    gstWtpEventFlag.ucFlagL2IeId_87 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 88:
    gstWtpEventFlag.ucFlagL2IeId_88 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 107:
    gstWtpEventFlag.ucFlagL2IeId_107 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 133:
    gstWtpEventFlag.ucFlagL2IeId_133 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 134:
    gstWtpEventFlag.ucFlagL2IeId_134 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  case 138:
    gstWtpEventFlag.ucFlagL2IeId_138 = MT_WTP_EVENT_L2IE_ENABLE;
    break;

  default:
    break;
     }
}

void wtpeventstop(UINT32 vulL2IeId)
{
  if(0 == vulL2IeId)
    {
    gucWtpEventFlag = MT_WTP_EVENT_DISABLE;

    gstWtpEventFlag.ucFlagL2IeId_5604 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5621 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5622 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5623 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5624 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5625 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5626 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5627 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5628 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5629 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5630 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5631 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5632 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5633 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5634 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5635 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5636 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5637 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5638 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5639 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5640 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5641 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5642 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5643 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5644 = MT_WTP_EVENT_L2IE_DISABLE;

    gstWtpEventFlag.ucFlagL2IeId_5721 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5722 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5723 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5724 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5725 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5726 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5727 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5728 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5729 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5730 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5731 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5732 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5733 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5734 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5735 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5736 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5737 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5738 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5739 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5740 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5741 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5742 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_5743 = MT_WTP_EVENT_L2IE_DISABLE;

    gstWtpEventFlag.ucFlagL2IeId_58 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_59 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_60 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_61 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_62 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_63 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_64 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_65 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_66 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_67 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_68 = MT_WTP_EVENT_L2IE_DISABLE;
    //gstWtpEventFlag.ucFlagL2IeId_69 = MT_WTP_EVENT_L2IE_DISABLE;
    //gstWtpEventFlag.ucFlagL2IeId_70 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_71 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_72 = MT_WTP_EVENT_L2IE_DISABLE;
    //gstWtpEventFlag.ucFlagL2IeId_73 = MT_WTP_EVENT_L2IE_DISABLE;
    //gstWtpEventFlag.ucFlagL2IeId_74 = MT_WTP_EVENT_L2IE_DISABLE;
    //gstWtpEventFlag.ucFlagL2IeId_75 = MT_WTP_EVENT_L2IE_DISABLE;
    //gstWtpEventFlag.ucFlagL2IeId_76 = MT_WTP_EVENT_L2IE_DISABLE;
    //gstWtpEventFlag.ucFlagL2IeId_77 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_78 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_79 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_80 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_81 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_82 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_83 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_84 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_85 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_86 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_87 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_88 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_107 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_133 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_134 = MT_WTP_EVENT_L2IE_DISABLE;
    gstWtpEventFlag.ucFlagL2IeId_138 = MT_WTP_EVENT_L2IE_DISABLE;
    return;
    }

  switch(vulL2IeId)
     {
  case 5604:
    gstWtpEventFlag.ucFlagL2IeId_5604 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5621:
    gstWtpEventFlag.ucFlagL2IeId_5621 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5622:
    gstWtpEventFlag.ucFlagL2IeId_5622 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5623:
    gstWtpEventFlag.ucFlagL2IeId_5623 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5624:
    gstWtpEventFlag.ucFlagL2IeId_5624 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5625:
    gstWtpEventFlag.ucFlagL2IeId_5625 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5626:
    gstWtpEventFlag.ucFlagL2IeId_5626 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5627:
    gstWtpEventFlag.ucFlagL2IeId_5627 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5628:
    gstWtpEventFlag.ucFlagL2IeId_5628 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5629:
    gstWtpEventFlag.ucFlagL2IeId_5629 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5630:
    gstWtpEventFlag.ucFlagL2IeId_5630 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5631:
    gstWtpEventFlag.ucFlagL2IeId_5631 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5632:
    gstWtpEventFlag.ucFlagL2IeId_5632 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5633:
    gstWtpEventFlag.ucFlagL2IeId_5633 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5634:
    gstWtpEventFlag.ucFlagL2IeId_5634 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5635:
    gstWtpEventFlag.ucFlagL2IeId_5635 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5636:
    gstWtpEventFlag.ucFlagL2IeId_5636 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5637:
    gstWtpEventFlag.ucFlagL2IeId_5637 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5638:
    gstWtpEventFlag.ucFlagL2IeId_5638 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5639:
    gstWtpEventFlag.ucFlagL2IeId_5639 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5640:
    gstWtpEventFlag.ucFlagL2IeId_5640 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5641:
    gstWtpEventFlag.ucFlagL2IeId_5641 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5642:
    gstWtpEventFlag.ucFlagL2IeId_5642 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5643:
    gstWtpEventFlag.ucFlagL2IeId_5643 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5644:
    gstWtpEventFlag.ucFlagL2IeId_5644 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5721:
    gstWtpEventFlag.ucFlagL2IeId_5721 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5722:
    gstWtpEventFlag.ucFlagL2IeId_5722 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5723:
    gstWtpEventFlag.ucFlagL2IeId_5723 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5724:
    gstWtpEventFlag.ucFlagL2IeId_5724 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5725:
    gstWtpEventFlag.ucFlagL2IeId_5725 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5726:
    gstWtpEventFlag.ucFlagL2IeId_5726 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5727:
    gstWtpEventFlag.ucFlagL2IeId_5727 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5728:
    gstWtpEventFlag.ucFlagL2IeId_5728 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5729:
    gstWtpEventFlag.ucFlagL2IeId_5729 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5730:
    gstWtpEventFlag.ucFlagL2IeId_5730 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5731:
    gstWtpEventFlag.ucFlagL2IeId_5731 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5732:
    gstWtpEventFlag.ucFlagL2IeId_5732 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5733:
    gstWtpEventFlag.ucFlagL2IeId_5733 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5734:
    gstWtpEventFlag.ucFlagL2IeId_5734 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5735:
    gstWtpEventFlag.ucFlagL2IeId_5735 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5736:
    gstWtpEventFlag.ucFlagL2IeId_5736 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5737:
    gstWtpEventFlag.ucFlagL2IeId_5737 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5738:
    gstWtpEventFlag.ucFlagL2IeId_5738 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5739:
    gstWtpEventFlag.ucFlagL2IeId_5739 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5740:
    gstWtpEventFlag.ucFlagL2IeId_5740 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5741:
    gstWtpEventFlag.ucFlagL2IeId_5741 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5742:
    gstWtpEventFlag.ucFlagL2IeId_5742 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 5743:
    gstWtpEventFlag.ucFlagL2IeId_5743 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 58:
    gstWtpEventFlag.ucFlagL2IeId_58 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 59:
    gstWtpEventFlag.ucFlagL2IeId_59 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 60:
    gstWtpEventFlag.ucFlagL2IeId_60 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 61:
    gstWtpEventFlag.ucFlagL2IeId_61 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 62:
    gstWtpEventFlag.ucFlagL2IeId_62 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 63:
    gstWtpEventFlag.ucFlagL2IeId_63 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 64:
    gstWtpEventFlag.ucFlagL2IeId_64 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 65:
    gstWtpEventFlag.ucFlagL2IeId_65 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 66:
    gstWtpEventFlag.ucFlagL2IeId_66 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 67:
    gstWtpEventFlag.ucFlagL2IeId_67 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 68:
    gstWtpEventFlag.ucFlagL2IeId_68 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 69:
    //gstWtpEventFlag.ucFlagL2IeId_69 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 70:
    //gstWtpEventFlag.ucFlagL2IeId_70 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 71:
    gstWtpEventFlag.ucFlagL2IeId_71 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 72:
    gstWtpEventFlag.ucFlagL2IeId_72 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 73:
    //gstWtpEventFlag.ucFlagL2IeId_73 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 74:
    //gstWtpEventFlag.ucFlagL2IeId_74 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 75:
    //gstWtpEventFlag.ucFlagL2IeId_75 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 76:
    //gstWtpEventFlag.ucFlagL2IeId_76 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 77:
    //gstWtpEventFlag.ucFlagL2IeId_77 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 78:
    gstWtpEventFlag.ucFlagL2IeId_78 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 79:
    gstWtpEventFlag.ucFlagL2IeId_79 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 80:
    gstWtpEventFlag.ucFlagL2IeId_80 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 81:
    gstWtpEventFlag.ucFlagL2IeId_81 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 82:
    gstWtpEventFlag.ucFlagL2IeId_82 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 83:
    gstWtpEventFlag.ucFlagL2IeId_83 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 84:
    gstWtpEventFlag.ucFlagL2IeId_84 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 85:
    gstWtpEventFlag.ucFlagL2IeId_85 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 86:
    gstWtpEventFlag.ucFlagL2IeId_86 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 87:
    gstWtpEventFlag.ucFlagL2IeId_87 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 88:
    gstWtpEventFlag.ucFlagL2IeId_88 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 107:
    gstWtpEventFlag.ucFlagL2IeId_107 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 133:
    gstWtpEventFlag.ucFlagL2IeId_133 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 134:
    gstWtpEventFlag.ucFlagL2IeId_134 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  case 138:
    gstWtpEventFlag.ucFlagL2IeId_138 = MT_WTP_EVENT_L2IE_DISABLE;
    break;

  default:
    break;
     }
}

//寻找STA切换所需要的next AP。
UINT32 findDestAp(UINT32 vulSrcApIndex)
{
	UINT32 ulApIndex = MT_UNDEFINED_OBJECT_IDENTIFIER;
	UINT8 ucFindNestAPFalg = 0;//0:cannot find;1:find it;

	if(vulSrcApIndex >= gulCmdStartApNum || vulSrcApIndex < 0)//if(vulSrcApIndex >= MT_AP_MAX_NUM || vulSrcApIndex < 0)
	{
		return ulApIndex;
	}
#if 0
	switch(gucStaAccessPlcy)
	{
	case MT_STA_ACCESS_POLICY_AP_FIRST:
	{
		for(ulApIndex = 0; ulApIndex < MT_AP_MAX_NUM; ulApIndex++)
		{
			if((RUN_STATE == gastAp[ulApIndex].enState) && (MT_TRUE == gastAp[ulApIndex].bKeepAliveFlag))
			{
				if(ulApIndex == vulSrcApIndex)
				{
					continue;
				}

				if(gastAp[ulApIndex].ulStaCnt < MT_STA_MAX_NUM_PER_WTP)
				{
					break;
				}
				else
				{
					continue;
				}
			}
		}
		break;
	}

	case MT_STA_ACCESS_POLICY_STA_FIRST:
	{
		for(ulApIndex = 0; ulApIndex < MT_AP_MAX_NUM; ulApIndex++)
		{
			if((RUN_STATE == gastAp[ulApIndex].enState) && (MT_TRUE == gastAp[ulApIndex].bKeepAliveFlag))
			{
				if(ulApIndex == vulSrcApIndex)
				{
					continue;
				}

				if(gastAp[ulApIndex].ulStaCnt < gastAp[(ulApIndex + 1) % MT_AP_MAX_NUM].ulStaCnt)
				{
					break;
				}
				else
				{
					continue;
				}
			}
		}
		break;
	}
	default:
		printf("Error! File:%s,Line:%d,gucStaAccessPlcy:%d\n",__FILE__,__LINE__,gucStaAccessPlcy);
		break;
	}
#endif

	//for(ulApIndex = vulSrcApIndex + 1; ulApIndex < MT_AP_MAX_NUM; ulApIndex++)
	for(ulApIndex = vulSrcApIndex + 1; ulApIndex < gulCmdStartApNum; ulApIndex++)
	{
		if((RUN_STATE == gastAp[ulApIndex].enState) && (MT_TRUE == gastAp[ulApIndex].bKeepAliveFlag))
		{
			if(gastAp[ulApIndex].ulStaCnt < MT_STA_MAX_NUM_PER_WTP)
			{
				ucFindNestAPFalg = 1;
				break;
			}
			else
			{
				continue;
			}
		}
	}

	if(0 == ucFindNestAPFalg)
	{
		for(ulApIndex = 0; ulApIndex < vulSrcApIndex; ulApIndex++)
		{
			if((RUN_STATE == gastAp[ulApIndex].enState) && (MT_TRUE == gastAp[ulApIndex].bKeepAliveFlag))
			{
				if(gastAp[ulApIndex].ulStaCnt < MT_STA_MAX_NUM_PER_WTP)
				{
					break;
				}
				else
				{
					continue;
				}
			}
		}
	}
	return ulApIndex;
}

UINT32 getApIDbyBssid(UINT32 vulStaIndex)
{
	UINT32 ulApIndex = MT_UNDEFINED_OBJECT_IDENTIFIER;
	//0x00,0x0b,ApIdx1,ApIdx2,RadioId,WlanId == BSSID

	if(MT_WTP_BSSID_KEY_BYTE != gastStaPara[vulStaIndex].aucBssId[1])
	{
		return ulApIndex;
	}

	ulApIndex = gastStaPara[vulStaIndex].aucBssId[2] * 256 + gastStaPara[vulStaIndex].aucBssId[3];//通过BSSID获取AP ID

	if(ulApIndex >= gulCmdStartApNum)
	{
		ulApIndex = MT_UNDEFINED_OBJECT_IDENTIFIER;
	}

	return ulApIndex;
}

UINT32 getStaIndexByMac(UINT8 *vpStaMac)
{
	UINT32 ulStaIndex = MT_UNDEFINED_OBJECT_IDENTIFIER;

	ulStaIndex = (*(vpStaMac + 3)) * 65536 + (*(vpStaMac + 4)) * 256 + *(vpStaMac + 5);

	if(ulStaIndex > MT_SYS_STA_CAPABILITY)
	{
		ulStaIndex = MT_UNDEFINED_OBJECT_IDENTIFIER;
	}

	return ulStaIndex;
}

void starthandover(void)
{
	gbHdOverFlag = MT_TRUE;
}

void stopthandover(void)
{
	gbHdOverFlag = MT_FALSE;
}

void MT_StaAccessSTAfirst(void)
{
	UINT32 ulStaPrdTrgCntr = 0;
	UINT32 ulApIdx = 0;
	UINT32 ulStaIndex = 0;
	UINT8 ucRadioId = 0;
	UINT8 ucWlanId = 0;
	UINT8 ucWlanNum = 0;
	UINT8 ucStaWlanIndex = 0;
	MT_STA_AUTH_INFO stStaAuthInfo;
	MT_STA_WLAN_INFO_T stStaWLanInfo;
	MTBool bSendResult = MT_FALSE;
	UINT32 ulTimeMsgPara = 0;

	cpss_mem_memset(&stStaAuthInfo,0,sizeof(MT_STA_AUTH_INFO));
	cpss_mem_memset(&stStaWLanInfo,0,sizeof(MT_STA_WLAN_INFO_T));

	if(0 == gulStaNumPerAp)
	{
		printf("ERROR! gulStaNumPerAp is %d.\nFile: %s Line: %d\n",gulStaNumPerAp,__FILE__,__LINE__);
		return;
	}

	for(ulStaPrdTrgCntr = 0; ulStaPrdTrgCntr < gulPeriodTriggerStaNum; ulStaPrdTrgCntr++)
	{
		ulApIdx = gulStaID % gulCmdStartApNum;//产生AP索引

		if(RUN_STATE != gastAp[ulApIdx].enState || gastAp[ulApIdx].ulStaCnt >= gulStaNumPerAp)
		{
			gulStaID = MT_GetStaId(gulStaID);
			continue;
		}

		for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
		{
			if(AUTH_INIT_STATE == gastStaPara[ulStaIndex].enAuthState)
			{
				break;
			}
			else
			{
				continue;
			}
		}

		if ((RUN_STATE == gastAp[ulApIdx].enState) && (MT_TRUE == gastAp[ulApIdx].bKeepAliveFlag))
		{
			ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况
			ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

			if(0 == ucWlanNum)
			{
				break;
			}

			stStaAuthInfo.ulApIndex = ulApIdx;
			stStaAuthInfo.ulStaIndex = ulStaIndex;
			stStaAuthInfo.ucRadioId = ucRadioId;

			ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
			ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

			if(MT_UNDEFINED_WLANID_IDENTIFIER == ucWlanId)
			{
				break;
			}

			stStaAuthInfo.ucWlanId = ucWlanId;
			stStaAuthInfo.usAuthAlgrm = (UINT16)gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucWlanId].ucAuthType;
			stStaAuthInfo.usAuthSeq = 1;
			stStaAuthInfo.ucChallengeTxtLen = 0;

			bSendResult = ApMutiTestSend80211AuthReq(stStaAuthInfo);
			if(MT_TRUE == bSendResult)
			{
				gstMutiTest_Statics.ulSend80211AuthReq++;
				gastStaPara[ulStaIndex].enAuthState = WAIT_80211_AUTH_RSP_STATE;
				MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,gastStaPara[ulStaIndex].aucBssId);
				MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,gastStaPara[ulStaIndex].aucCrrntAp);
				gastStaPara[ulStaIndex].ulHomeApId = ulApIdx;
				gastStaPara[ulStaIndex].ucWlanId = ucWlanId;

				ulTimeMsgPara = ulStaIndex;
				gastStaPara[ulStaIndex].ulStaHoldTimerId = cpss_timer_para_set(MT_TEST_STAHOLD_TMRID, gulStaHoldTime * 1000, ulTimeMsgPara);
				gastStaPara[ulStaIndex].stStaOnLineTm.ulBeginTime = cpss_tick_get();

				MT_StaLog(ulStaIndex,"STA Send80211AuthReq Success.");
				//DHCP
				stStaWLanInfo.ulApIndex = ulApIdx;
				stStaWLanInfo.ulStaIndex = ulStaIndex;
				stStaWLanInfo.ucRadioId = ucRadioId;
				stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

				if(gbStaDhcpFlag == MT_TRUE && IP_ADDR_TYPE_V4 == gulIpAddrFlag && DHCPV4_INIT_STATE == gastStaPara[ulStaIndex].enDhcpState)
				{
					bSendResult = ApMutiTestSendDhcpDiscover(stStaWLanInfo);
					if(MT_TRUE == bSendResult)
					{
						gstMutiTest_Statics.ulStaSendDhcpDiscover++;
						gastStaPara[ulStaIndex].enDhcpState = WAIT_DHCP_OFFER_STATE;
					}
					else
					{
						gstErrMsgStatics.ulSendDhcpDiscoverErr++;
					}
				}
				else if(gbStaDhcpFlag == MT_TRUE && IP_ADDR_TYPE_V6 == gulIpAddrFlag && DHCPV6_INIT_STATE == gastStaPara[ulStaIndex].enDhcpState)
				{
					bSendResult = ApMutiTestSendDhcpv6Solicit(stStaWLanInfo);
					if(MT_TRUE == bSendResult)
					{
						gstMutiTest_Statics.ulStaSendDhcpv6Solicit++;
						gastStaPara[ulStaIndex].enDhcpv6State = WAIT_DHCPV6_ADVERTISE_STATE;
					}
					else
					{
						gstErrMsgStatics.ulSendDhcpv6SolicitErr++;
					}
				}
			}
			else
			{
				gstErrMsgStatics.ulSend80211AuthReqErr++;
				MT_StaLog(ulStaIndex,"STA Send80211AuthReq Failure.");
			}
			gulStaID = MT_GetStaId(gulStaID);
		}
	}
}

UINT32 findNextAPforAccess(UINT32 vulApIdx)
{
	UINT32 ulNextApIdx = MT_UNDEFINED_OBJECT_IDENTIFIER;

	//if(vulApIdx >= MT_AP_MAX_NUM || vulApIdx < 0)
	if(vulApIdx >= gulCmdStartApNum || vulApIdx < 0)
	{
		return ulNextApIdx;
	}

	//for(ulNextApIdx = ((vulApIdx + 1) % MT_AP_MAX_NUM); ulNextApIdx < MT_AP_MAX_NUM; ulNextApIdx++)
	for(ulNextApIdx = ((vulApIdx + 1) % gulCmdStartApNum); ulNextApIdx < gulCmdStartApNum; ulNextApIdx++)
	{
		if(RUN_STATE == gastAp[ulNextApIdx].enState && gastAp[ulNextApIdx].ulStaCnt < gulStaNumPerAp)
		{
			break;
		}
		else
		{
			continue;
		}
	}
	return ulNextApIdx;
}

void MT_StaAccessAPfirst(void)
{
	UINT32 ulApIdx = 0;
	UINT32 ulStaIndex = 0;
	UINT32 ulPrdTrgStaCntr = 0;//在触发周期内，STA计数器
	UINT8 ucRadioId = 0;
	UINT8 ucWlanId = 0;
	UINT8 ucWlanNum = 0;
	UINT8 ucStaWlanIndex = 0;
	MT_STA_AUTH_INFO stStaAuthInfo;
	MT_STA_WLAN_INFO_T stStaWLanInfo;
	MTBool bSendResult = MT_FALSE;
	UINT32 ulTimeMsgPara = 0;
	UINT8 ucExitFlag = 0;

	cpss_mem_memset(&stStaAuthInfo,0,sizeof(MT_STA_AUTH_INFO));

	for(ulApIdx = 0; ulApIdx < gulApMaxNum; ulApIdx++)
	{
		if((RUN_STATE == gastAp[ulApIdx].enState) && (MT_TRUE == gastAp[ulApIdx].bKeepAliveFlag))
		{
			for(ulStaIndex = ulApIdx * MT_STA_MAX_NUM_PER_WTP; ulStaIndex < gulStaNumPerAp + ulApIdx * MT_STA_MAX_NUM_PER_WTP; ulStaIndex++)
			{
				if(ulPrdTrgStaCntr >= gulPeriodTriggerStaNum)
				{
					ucExitFlag = 1;
					break;
				}

				if(AUTH_INIT_STATE == gastStaPara[ulStaIndex].enAuthState)
				{
					stStaAuthInfo.ulApIndex = ulApIdx;
					stStaAuthInfo.ulStaIndex = ulStaIndex;
					ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况
					ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

					if(0 == ucWlanNum)
					{
						break;
					}

					ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
					stStaAuthInfo.ucRadioId = ucRadioId;
					ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

					if(MT_UNDEFINED_WLANID_IDENTIFIER == ucWlanId)
					{
						break;
					}

					stStaAuthInfo.ucWlanId = ucWlanId;
					stStaAuthInfo.usAuthAlgrm = (UINT16)gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucWlanId].ucAuthType;
					stStaAuthInfo.usAuthSeq = 1;
					stStaAuthInfo.ucChallengeTxtLen = 0;

					bSendResult = ApMutiTestSend80211AuthReq(stStaAuthInfo);

					if(MT_TRUE == bSendResult)
					{
						ulPrdTrgStaCntr++;
						gstMutiTest_Statics.ulSend80211AuthReq++;
						gastStaPara[ulStaIndex].enAuthState = WAIT_80211_AUTH_RSP_STATE;
						MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,gastStaPara[ulStaIndex].aucBssId);
						MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,gastStaPara[ulStaIndex].aucCrrntAp);
						gastStaPara[ulStaIndex].ulHomeApId = ulApIdx;
						gastStaPara[ulStaIndex].ucWlanId = ucWlanId;

						ulTimeMsgPara = ulStaIndex;
						gastStaPara[ulStaIndex].ulStaHoldTimerId = cpss_timer_para_set(MT_TEST_STAHOLD_TMRID, gulStaHoldTime * 1000, ulTimeMsgPara);
						gastStaPara[ulStaIndex].stStaOnLineTm.ulBeginTime = cpss_tick_get();

						MT_StaLog(ulStaIndex,"STA Send80211AuthReq Success.");

						//DHCP过程
						cpss_mem_memset(&stStaWLanInfo,0,sizeof(MT_STA_WLAN_INFO_T));
						stStaWLanInfo.ulApIndex = ulApIdx;
						stStaWLanInfo.ulStaIndex = ulStaIndex;
						ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况
						stStaWLanInfo.ucRadioId = ucRadioId;
//						ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

						if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
						{
							break;
						}

						if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
						{
						  break;
						}

						stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//						ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//						stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

						if(gbStaDhcpFlag == MT_TRUE && IP_ADDR_TYPE_V4 == gulIpAddrFlag && DHCPV4_INIT_STATE == gastStaPara[ulStaIndex].enDhcpState)
						{
							bSendResult = ApMutiTestSendDhcpDiscover(stStaWLanInfo);
							if(MT_TRUE == bSendResult)
							{
								gstMutiTest_Statics.ulStaSendDhcpDiscover++;
								gastStaPara[ulStaIndex].enDhcpState = WAIT_DHCP_OFFER_STATE;
							}
							else
							{
								gstErrMsgStatics.ulSendDhcpDiscoverErr++;
							}
						}
						else if(gbStaDhcpFlag == MT_TRUE && IP_ADDR_TYPE_V6 == gulIpAddrFlag && DHCPV6_INIT_STATE == gastStaPara[ulStaIndex].enDhcpState)
						{
							bSendResult = ApMutiTestSendDhcpv6Solicit(stStaWLanInfo);
							if(MT_TRUE == bSendResult)
							{
								gstMutiTest_Statics.ulStaSendDhcpv6Solicit++;
								gastStaPara[ulStaIndex].enDhcpv6State = WAIT_DHCPV6_ADVERTISE_STATE;
							}
							else
							{
								gstErrMsgStatics.ulSendDhcpv6SolicitErr++;
							}
						}
					}
					else
					{
						gstErrMsgStatics.ulSend80211AuthReqErr++;
						MT_StaLog(ulStaIndex,"STA Send80211AuthReq Failure.");
					}
				}
//				else if(AUTH_INIT_STATE != gastStaPara[ulStaIndex].enAuthState && ASSOCIATION_SUCESS_STATE != gastStaPara[ulStaIndex].enAuthState)
//				{
//					gastStaPara[ulStaIndex].ulStaTrgPrdCnt++;
//					if(MT_STA_REAUTH_INTERVAL_CNT == gastStaPara[ulStaIndex].ulStaTrgPrdCnt)
//					{
//						MT_StaTmOv(ulStaIndex);
//					}
//				}
			}
			if(ucExitFlag)
			{
				break;
			}
		}
		else
		{
			continue;
		}
	}
}

UINT32 staOnlineCnt(void)
{
	UINT32 ulStaIndex = 0;
	UINT32 ulStaCnt = 0;

	for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
	{
		if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState)
		{
			ulStaCnt++;
		}
	}
	return ulStaCnt;
}

UINT32 apOnlineCnt(void)
{
	UINT32 ulApIdx = 0;
	UINT32 ulApCnt = 0;

	for(ulApIdx = 0; ulApIdx < MT_AP_MAX_NUM; ulApIdx++)
	{
		if(RUN_STATE == gastAp[ulApIdx].enState)
		{
			ulApCnt++;
		}
	}
	return ulApCnt;
}

void MT_ApLog(UINT32 vulApIdx, char *vpLogInfo)
{
	FILE *pApLogFile = NULL;
#define MT_AP_LOG_BUF_LEN 1024
	UINT8 aucLogBuf[MT_AP_LOG_BUF_LEN] = {'\0'};
	time_t ulApTime = 0;
	UINT8 aucApteStr[32] = {'\0'};

	if(MT_FALSE == gbLogFlag)
	{
		return;
	}

	pApLogFile = fopen("/mt_conf/aplog.log", "aw+");

	if(NULL == pApLogFile)
	{
		return;
	}

	time(&ulApTime);
	strcpy((char*)aucApteStr,(char*)MT_GetApStateStr(gastAp[vulApIdx].enState));

	sprintf((char*)aucLogBuf,"%sAP_ID: %d, AP_MAC: %s, AP_IPADDR: %s, AP_STATE: %s, DiscoverCnt: %d, LOGINFO: %s\n\n",
			ctime(&ulApTime),
			vulApIdx,
			gastApconf[vulApIdx].aucApMacStr,
			gastApconf[vulApIdx].aucApIpV4AddrStr,
			aucApteStr,
			gastAp[vulApIdx].ulDiscvCnt,
			vpLogInfo);

	fputs((char*)aucLogBuf,pApLogFile);
	fclose(pApLogFile);
}

void MT_StaLog(UINT32 vulStaIndex, char* vpLogInfo)
{
	FILE *pStaLogFile = NULL;
#define MT_STA_LOG_BUF_LEN 1024
	UINT8 aucLogBuf[MT_AP_LOG_BUF_LEN] = {'\0'};
	time_t ulStaTime = 0;
	UINT32 ulApIdx = 0;
	UINT8 aucStaMacStr[17] = {'\0'};
	UINT8 aucStaBssidStr[17] = {'\0'};
	UINT8 aucStaAuthStatStr[32] = {'\0'};
	UINT8 aucStaHOStatStr[32] = {'\0'};
	UINT8 aucTemp[4] = {'\0'};

	if(MT_FALSE == gbLogFlag)
	{
		return;
	}

	if(MT_UNDEFINED_OBJECT_IDENTIFIER == vulStaIndex)
	{
		return;
	}

	ulApIdx = getApIDbyBssid(vulStaIndex);

	if(MT_UNDEFINED_OBJECT_IDENTIFIER == ulApIdx)
	{
		return;
	}

	pStaLogFile = fopen("/mt_conf/stalog.log", "aw+");

	if(NULL == pStaLogFile)
	{
		return;
	}

	time(&ulStaTime);
#if 0
	TransMacToStr(gastStaPara[vulStaIndex].auStaMac,aucStaMacStr);
	TransMacToStr(gastStaPara[vulStaIndex].aucBssId,aucStaBssidStr);
	strcpy(aucStaAuthStatStr,MT_GetStaAuthStateStr(gastStaPara[vulStaIndex].enAuthState));
	strcpy(aucStaHOStatStr,MT_GetStaHOStateStr(gastStaPara[vulStaIndex].enHandOverState));

	sprintf(aucLogBuf,"%sSTA_ID: %d, STA_MAC: %s, STA_BSSID: %s, STA_AUTH_STATE: %s, STA_HO_STATE: %s, LOCAL_APID: %d, AP_MAC: %s, H_APID: %d, F_APID: %d, LOGINFO: %s.\n\n",
			ctime(&ulStaTime),
			vulStaIndex,
			aucStaMacStr,
			aucStaBssidStr,
			aucStaAuthStatStr,
			aucStaHOStatStr,
			gastStaPara[vulStaIndex].ulStaLocalApId,
			gastApconf[ulApIdx].aucApMacStr,
			gastStaPara[vulStaIndex].ulHomeApId,
			gastStaPara[vulStaIndex].ulForeignApId,
			vpLogInfo);
#endif
	strcpy((char*)aucLogBuf,ctime(&ulStaTime));

	strcat((char*)aucLogBuf,"STA_ID: ");
	sprintf((char*)aucTemp,"%d",vulStaIndex);
	strcat((char*)aucLogBuf,(char*)aucTemp);

	strcat((char*)aucLogBuf,", STA_MAC: ");
	TransMacToStr((unsigned char*)gastStaPara[vulStaIndex].auStaMac,(char*)aucStaMacStr);
	strcat((char*)aucLogBuf,(char*)aucStaMacStr);

	strcat((char*)aucLogBuf,", STA_BSSID: ");
	TransMacToStr((unsigned char*)gastStaPara[vulStaIndex].aucBssId,(char*)aucStaBssidStr);
	strcat((char*)aucLogBuf,(char*)aucStaBssidStr);

	strcat((char*)aucLogBuf,", AUTH_STATE: ");
	strcat((char*)aucLogBuf,(char*)MT_GetStaAuthStateStr(gastStaPara[vulStaIndex].enAuthState));


	strcat((char*)aucLogBuf,", HO_STATE: ");
	strcat((char*)aucLogBuf,(char*)MT_GetStaHOStateStr(gastStaPara[vulStaIndex].enHandOverState));

	strcat((char*)aucLogBuf,", AP_MAC: ");
	strcat((char*)aucLogBuf,(char*)gastApconf[ulApIdx].aucApMacStr);

	strcat((char*)aucLogBuf,", LOCAL_APID: ");
	cpss_mem_memset(aucTemp,0,4);
	sprintf((char*)aucTemp,"%d",gastStaPara[vulStaIndex].ulStaLocalApId);
	strcat((char*)aucLogBuf,(char*)aucTemp);

	//homeApID
	strcat((char*)aucLogBuf,", H_APID: ");
	cpss_mem_memset(aucTemp,0,4);
	sprintf((char*)aucTemp,"%d",gastStaPara[vulStaIndex].ulHomeApId);
	strcat((char*)aucLogBuf,(char*)aucTemp);

	//ForgienApID
	strcat((char*)aucLogBuf,", F_APID: ");
	cpss_mem_memset(aucTemp,0,4);
	sprintf((char*)aucTemp,"%d",gastStaPara[vulStaIndex].ulForeignApId);
	strcat((char*)aucLogBuf,(char*)aucTemp);

	//LOGINFO
	strcat((char*)aucLogBuf,", LOGINFO: ");
	strcat((char*)aucLogBuf,vpLogInfo);
	strcat((char*)aucLogBuf,"\n\n");

	fputs((char*)aucLogBuf,pStaLogFile);
	fclose(pStaLogFile);
}

void MT_RunLog(UINT8* vpBuf, UINT32 vulBufLen, char *vpLogInfo)
{
	FILE *pRunLogFile = NULL;
#define MT_RUN_LOG_BUF_LEN 1024
	UINT8 aucLogBuf[MT_RUN_LOG_BUF_LEN] = {'\0'};
	time_t ulRunTime = 0;
	UINT32 ulLoop = 0;

	if(MT_FALSE == gbLogFlag)
	{
		return;
	}

	pRunLogFile = fopen("/mt_conf/runlog.log", "aw+");

	if(NULL == pRunLogFile)
	{
		return;
	}

	time(&ulRunTime);

	sprintf((char*)aucLogBuf,"%sLOGINFO: %s\nBuf:\n",ctime(&ulRunTime),vpLogInfo);
	fputs((char*)aucLogBuf,pRunLogFile);
	cpss_mem_memset(aucLogBuf,0,MT_RUN_LOG_BUF_LEN);
	cpss_mem_memcpy(aucLogBuf,vpBuf,vulBufLen);
	fputs((char*)aucLogBuf,pRunLogFile);
	fclose(pRunLogFile);
}

void stadhcpstart(void)
{
	gbStaDhcpFlag == MT_TRUE;
}

void stadhcpstop(void)
{
	gbStaDhcpFlag == MT_FALSE;
}
