/******************************************************************************
* COPYRIGHT @ Reserved
*******************************************************************************
* File         : sta_proc.c
* Function     : 
* Statement    : 
* Creation Info：2011-11-2 Edward Wang
******************************************************************************/
/******************************************************************************
* Modification: 
* Time        ：
* Modifier    ：
* Statement   ：
******************************************************************************/

#include <time.h>
#include "sta_proc.h"

extern UINT32 gulStaHoldTime;
extern MTBool gbTestMod;
extern UINT16 gusSequenceNum;
extern UINT32 gulSysConfAPCnt;
extern UINT8  gaucStaGwIpV4Addr[MT_IPV4_ADDR_LEN];//SRV的IP地址
extern UINT8  gaucStaGwIpV6Addr[MT_IPV6_ADDR_LEN];//SRV的IP地址
extern UINT32 gulIpAddrFlag;//IP Addr Flag
extern MTBool ApMutiTestSendEventReq_IE37_IE56(UINT32 vulApIdx);
UINT32 gulDeAuthTestFlag = 0;
UINT32 gulAuthTestFlag = 0;
UINT32 gulDeAssoTestFlag = 0;
extern MTBool gbLogFlag;

#if 0
/*802.11认证请求消息*/
UINT8 gau80211AuthReq[]={
    0x00, 0x20, 0x43, 0x20, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xb0, 0x00, 0x75, 0x00, 0x00, 0x0b, 0x6b, 0xdf, 0xe4, 0xc2, 0x00, 0x1f, 0x3c, 0xdc, 0xe8, 0x62,
    0x00, 0x0b, 0x6b, 0xdf, 0xe4, 0xc2, 0x00, 0x04, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00
};

/*Association Req*/
UINT8 gauAssocReq[]={
    0x00, 0x20, 0x43, 0x20, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x75, 0x00, 0x00, 0x0b, 0x6b, 0xdf, 0xe4, 0xc2, 0x00, 0x1f, 0x3c, 0xdc, 0xe8, 0x62,
    0x00, 0x0b, 0x6b, 0xdf, 0xe4, 0xc2, 0x10, 0x04, 0x21, 0x05, 0x0a, 0x00, 0x00, 0x0c, 0x41, 0x43,
    0x31, 0x5f, 0x50, 0x4c, 0x41, 0x49, 0x4e, 0x5f, 0x70, 0x6d, 0x01, 0x08, 0x82, 0x84, 0x8b, 0x96,
    0x0c, 0x12, 0x18, 0x24, 0x21, 0x02, 0x00, 0x00, 0x24, 0x08, 0x95, 0x01, 0x99, 0x01, 0x9d, 0x01,
    0xa1, 0x01, 0x32, 0x04, 0x30, 0x48, 0x60, 0x6c, 0xdd, 0x07, 0x00, 0x50, 0xf2, 0x02, 0x00, 0x01,
    0x00
};

/*DeAuthReq*/
UINT8 gauDeAuthReq[]={
    0x00, 0x20, 0x43, 0x20, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0xc0, 0x00, 0x75, 0x00, 0x00, 0x0b, 0x6b, 0xd9, 0xa9, 0x6a, 0x00, 0x1f, 0x3c, 0xdc, 0xe8, 0x62,
    0x00, 0x0b, 0x6b, 0xd9, 0xa9, 0x6a, 0xc0, 0x0f, 0x01, 0x00
};
#endif


/*
        0                   1                   2                   3
        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |    Preamble   |  HLEN   |   RID   | WBID    |T|F|L|W|M|K|Flags|
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |          Fragment ID          |     Frag Offset         |Rsvd |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                 (optional) Radio MAC Address                  |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |            (optional) Wireless Specific Information           |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
       |                        Payload ....                           |
       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*/
MT_CAPWAPMSG_T gst80211AuthReq;
MT_CAPWAPMSG_T gst80211AuthHdOverReq;
MT_CAPWAPMSG_T gst80211AssocReq;
MT_CAPWAPMSG_T gst80211ReAssocReq;
MT_CAPWAPMSG_T gst80211DeAuthReq;
MT_CAPWAPMSG_T gst80211DisAssocReq;
MT_CAPWAPMSG_T gstPingReq;
MT_CAPWAPMSG_T gstPingReply;
MT_CAPWAPMSG_T gstArpReq;
MT_CAPWAPMSG_T gstArpReply;
MT_CAPWAPMSG_T gstPing6Req;
MT_CAPWAPMSG_T gstPing6Reply;
MT_CAPWAPMSG_T gstNeighborSolicit;//Neighbor Solicit
MT_CAPWAPMSG_T gstNeighborAdvertsmnt;//Neighbor Advertisement
MT_CAPWAPMSG_T gstDhcpDiscover;//DHCP Discover
MT_CAPWAPMSG_T gstDhcpReq;//DHCP Request
MT_CAPWAPMSG_T gstDhcpv6Solicit;//DHCPV6 Solicit
MT_CAPWAPMSG_T gstDhcpv6Req;//DHCPV6 Request
MT_CAPWAPMSG_T gst8021xAuthRsp;//IEEE 8021xAuthRsp
MT_CAPWAPMSG_T gstWapiAuthUncstRsp;//WAPI Auth Unicast key Rsp
MT_CAPWAPMSG_T gstWapiAuthMtcstRsp;//WAPI Auth Multicast key Rsp

//80211AuthReq消息发送函数,STA->SUT(AC)
MTBool ApMutiTestSend80211AuthReq(MT_STA_AUTH_INFO vstStaAuthInfo)
{
    MTBool bSendResult = MT_FALSE;
    UINT8 auc80211AuthReq[256] = {0};
    MT_80211AUTHENTICATION_T st80211AuthReq;
    MT_80211WLANMANAGMNTFRAME_FIXED_PARA_T st80211WlanMngmntFrmFixedPara;
    MT_80211WLANMANAGMNTFRAME_TAGGED_PARA_T stWlanMangmntFrmTaggedPara;
    UINT32 ulTaggedParaLen = 0;
    MT_80211WLANMANAGMNTFRAME_TAGGED_PARA_T stVendoeSpecific;
    UINT32 ulVendoeSpecificLen = 0;
    UINT8 aucOui[3] = {0};
    UINT8 ucVenOuiType = 0;
    UINT8 aucVenOuiVal[5] = {0};
    UINT8 aucBssId[MT_MAC_LENTH] = {0};
    UINT32 ulCapwapHdrFisrt32 = 0;
    UINT32 ulCapwapHdrSecond32 = 0;
    UINT8 ucWirelessLen = 0;
    UINT8 ucWDRssi = 0;//dBm
    UINT8 ucWDSnr = 0;//dB
    UINT16 usWDDataRate = 0;//Mbps
    UINT8 aucRscvd[3] = {0};
    UINT16 usFrameCtrl = 0;
    UINT16 usDuration = 0;
    UINT16 usSeqCtrl = 0;
    UINT32 ulApIdx = 0;
    UINT32 ulStaIndex = 0;
    UINT16 usAuthAlgrm = 0;
    UINT16 usAuthSeq = 0;
    UINT8 ucRadioId = 0;
    UINT8 ucWlanId = 0;
    UINT32 ulSuffix = 0;
//    UINT32 ulLoop = 0;

    cpss_mem_memset(&st80211AuthReq,0,sizeof(st80211AuthReq));
    cpss_mem_memset(&st80211WlanMngmntFrmFixedPara,0,sizeof(st80211WlanMngmntFrmFixedPara));
    cpss_mem_memset(&stWlanMangmntFrmTaggedPara,0,sizeof(stWlanMangmntFrmTaggedPara));
    cpss_mem_memset(&stVendoeSpecific,0,sizeof(stVendoeSpecific));

    ulApIdx = vstStaAuthInfo.ulApIndex;
    ulStaIndex = vstStaAuthInfo.ulStaIndex;
    usAuthAlgrm = vstStaAuthInfo.usAuthAlgrm;
    usAuthSeq = vstStaAuthInfo.usAuthSeq;

    ucRadioId = vstStaAuthInfo.ucRadioId;
    ucWlanId = vstStaAuthInfo.ucWlanId;
    MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);

    if(MT_FALSE == gbTestMod)//PARACONF
          {
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,1);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,1);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,1);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,1);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,0);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

      ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

      //Fragment ID
      MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
      //Fragment Offset
      MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
      //Reserved
      MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

      ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

      //Wireless Data
      ucWirelessLen = 4;
      ucWDRssi = 0;//dBm
      ucWDSnr = 0;//dB
      usWDDataRate = 0;//Mbps

      aucRscvd[0] = 0;
      aucRscvd[1] = 0;
      aucRscvd[2] = 0;

      st80211AuthReq.usFranCtrl = htons(0xB000);
      st80211AuthReq.usDuration = 314;
#if 0
      cpss_mem_memcpy(st80211AuthReq.aucDstMacAdd,gastAp[ulApIdx].auApMac,MT_MAC_LENTH);
#else
      cpss_mem_memcpy(st80211AuthReq.aucDstMacAdd,aucBssId,MT_MAC_LENTH);
#endif
      cpss_mem_memcpy(st80211AuthReq.aucSrcMAcAdd,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
      cpss_mem_memcpy(st80211AuthReq.aucBssid,aucBssId,MT_MAC_LENTH);

      switch(gulAuthTestFlag)
                {
      case 0://不变；
#if 0
    	  st80211AuthReq.aucDstMacAdd[0] = 0x ;
    	  st80211AuthReq.aucDstMacAdd[1] = 0x ;
    	  st80211AuthReq.aucDstMacAdd[2] = 0x ;
    	  st80211AuthReq.aucDstMacAdd[3] = 0x ;
    	  st80211AuthReq.aucDstMacAdd[4] = 0x ;
    	  st80211AuthReq.aucDstMacAdd[5] = 0x ;

    	  st80211AuthReq.aucSrcMAcAdd[0] = 0x ;
    	  st80211AuthReq.aucSrcMAcAdd[1] = 0x ;
    	  st80211AuthReq.aucSrcMAcAdd[2] = 0x ;
    	  st80211AuthReq.aucSrcMAcAdd[3] = 0x ;
    	  st80211AuthReq.aucSrcMAcAdd[4] = 0x ;
    	  st80211AuthReq.aucSrcMAcAdd[5] = 0x ;

    	  st80211AuthReq.aucBssid[0] = 0x ;
    	  st80211AuthReq.aucBssid[1] = 0x ;
    	  st80211AuthReq.aucBssid[2] = 0x ;
    	  st80211AuthReq.aucBssid[3] = 0x ;
    	  st80211AuthReq.aucBssid[4] = 0x ;
    	  st80211AuthReq.aucBssid[5] = 0x ;
#endif
    	  break;

      case 1://MAC1和MAC3相同，但是错误
    	  st80211AuthReq.aucDstMacAdd[0] = 0x99 ;
    	  st80211AuthReq.aucDstMacAdd[1] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[2] = 0x02 ;
    	  st80211AuthReq.aucDstMacAdd[3] = 0x03 ;
    	  st80211AuthReq.aucDstMacAdd[4] = 0x04 ;
    	  st80211AuthReq.aucDstMacAdd[5] = 0x05 ;
#if 0
    	  st80211AuthReq.aucSrcMAcAdd[0] = 0x ;
    	  st80211AuthReq.aucSrcMAcAdd[1] = 0x ;
    	  st80211AuthReq.aucSrcMAcAdd[2] = 0x ;
    	  st80211AuthReq.aucSrcMAcAdd[3] = 0x ;
    	  st80211AuthReq.aucSrcMAcAdd[4] = 0x ;
    	  st80211AuthReq.aucSrcMAcAdd[5] = 0x ;
#endif
    	  st80211AuthReq.aucBssid[0] = st80211AuthReq.aucDstMacAdd[0];
    	  st80211AuthReq.aucBssid[1] = st80211AuthReq.aucDstMacAdd[1];
    	  st80211AuthReq.aucBssid[2] = st80211AuthReq.aucDstMacAdd[2];
    	  st80211AuthReq.aucBssid[3] = st80211AuthReq.aucDstMacAdd[3];
    	  st80211AuthReq.aucBssid[4] = st80211AuthReq.aucDstMacAdd[4];
    	  st80211AuthReq.aucBssid[5] = st80211AuthReq.aucDstMacAdd[5];
    	  break;

      case 2://MAC1和MAC3不相同，但是值无所谓
    	  st80211AuthReq.aucDstMacAdd[0] = 0x77 ;
    	  st80211AuthReq.aucDstMacAdd[1] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[2] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[3] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[4] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[5] = 0x01 ;
#if 0
    	  st80211AuthReq.aucSrcMAcAdd[0] = 0x01 ;
    	  st80211AuthReq.aucSrcMAcAdd[1] = 0x01 ;
    	  st80211AuthReq.aucSrcMAcAdd[2] = 0x01 ;
    	  st80211AuthReq.aucSrcMAcAdd[3] = 0x01 ;
    	  st80211AuthReq.aucSrcMAcAdd[4] = 0x01 ;
    	  st80211AuthReq.aucSrcMAcAdd[5] = 0x01 ;
#endif
    	  st80211AuthReq.aucBssid[0] = 0x55 ;
    	  st80211AuthReq.aucBssid[1] = 0x01 ;
    	  st80211AuthReq.aucBssid[2] = 0x01 ;
    	  st80211AuthReq.aucBssid[3] = 0x01 ;
    	  st80211AuthReq.aucBssid[4] = 0x01 ;
    	  st80211AuthReq.aucBssid[5] = ucWlanId ;
    	  break;

      case 3:
#if 0
    	  st80211AuthReq.aucDstMacAdd[0] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[1] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[2] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[3] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[4] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[5] = 0x01 ;

    	  st80211AuthReq.aucSrcMAcAdd[0] = 0x01 ;
    	  st80211AuthReq.aucSrcMAcAdd[1] = 0x01 ;
    	  st80211AuthReq.aucSrcMAcAdd[2] = 0x01 ;
    	  st80211AuthReq.aucSrcMAcAdd[3] = 0x01 ;
    	  st80211AuthReq.aucSrcMAcAdd[4] = 0x01 ;
    	  st80211AuthReq.aucSrcMAcAdd[5] = 0x01 ;

    	  st80211AuthReq.aucBssid[0] = 0x01 ;
    	  st80211AuthReq.aucBssid[1] = 0x01 ;
    	  st80211AuthReq.aucBssid[2] = 0x01 ;
    	  st80211AuthReq.aucBssid[3] = 0x01 ;
    	  st80211AuthReq.aucBssid[4] = 0x01 ;
    	  st80211AuthReq.aucBssid[5] = 0x01 ;
#endif
    	  break;

      default:
    	  break;
                }

      MT_SetField16(st80211AuthReq.usFragSeqNum,0,4,0);//Fragment Number
      MT_SetField16(st80211AuthReq.usFragSeqNum,4,8,46);//Sequence Number
      MT_SetField16(st80211AuthReq.usFragSeqNum,12,4,0);//Resvd

      st80211WlanMngmntFrmFixedPara.usAuthAlgrm = usAuthAlgrm;
      if(1 == usAuthSeq)
                {
    	   st80211WlanMngmntFrmFixedPara.usAuthSeq = usAuthSeq;
                }
      else if(2 == usAuthSeq)
               {
    	   usAuthSeq = usAuthSeq + 1;
    	   st80211WlanMngmntFrmFixedPara.usAuthSeq = usAuthSeq;
                }
      st80211WlanMngmntFrmFixedPara.usStatusCode = 0;

      stWlanMangmntFrmTaggedPara.ucTagNum = 16;//Challenge Text
      stWlanMangmntFrmTaggedPara.ucTagLen = vstStaAuthInfo.ucChallengeTxtLen;
      cpss_mem_memcpy(stWlanMangmntFrmTaggedPara.aucTagValue,vstStaAuthInfo.aucChallengeTxt,stWlanMangmntFrmTaggedPara.ucTagLen);
      ulTaggedParaLen = sizeof(stWlanMangmntFrmTaggedPara.ucTagNum) + sizeof(stWlanMangmntFrmTaggedPara.ucTagLen) + stWlanMangmntFrmTaggedPara.ucTagLen;

      stVendoeSpecific.ucTagNum = 221;
      stVendoeSpecific.ucTagLen = 9;
      aucOui[0] = 0x00;
      aucOui[1] = 0x10;
      aucOui[2] = 0x18;
      ucVenOuiType = 2;
      aucVenOuiVal[0] = 0x00;
      aucVenOuiVal[1] = 0x10;
      aucVenOuiVal[2] = 0x00;
      aucVenOuiVal[3] = 0x00;
      aucVenOuiVal[4] = 0x00;
      cpss_mem_memcpy(&stVendoeSpecific.aucTagValue[0],aucOui,3);
      cpss_mem_memcpy(&stVendoeSpecific.aucTagValue[3],&ucVenOuiType,sizeof(ucVenOuiType));
      cpss_mem_memcpy(&stVendoeSpecific.aucTagValue[4],&aucVenOuiVal,5);
      ulVendoeSpecificLen = sizeof(stVendoeSpecific.ucTagNum) + sizeof(stVendoeSpecific.ucTagLen) + stVendoeSpecific.ucTagLen;

      ulSuffix = 0;
      cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
      ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
      cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
      ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
      cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&ucWirelessLen,sizeof(ucWirelessLen));
      ulSuffix = ulSuffix + sizeof(ucWirelessLen);
      cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&ucWDRssi,sizeof(ucWDRssi));
      ulSuffix = ulSuffix + sizeof(ucWDRssi);
      cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&ucWDSnr,sizeof(ucWDSnr));
      ulSuffix = ulSuffix + sizeof(ucWDSnr);
      cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&usWDDataRate,sizeof(usWDDataRate));
      ulSuffix = ulSuffix + sizeof(usWDDataRate);
      cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],aucRscvd,3);
      ulSuffix = ulSuffix + 3;
      cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&st80211AuthReq,sizeof(st80211AuthReq));
      ulSuffix = ulSuffix + sizeof(st80211AuthReq);
      cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&st80211WlanMngmntFrmFixedPara,sizeof(st80211WlanMngmntFrmFixedPara));
      ulSuffix = ulSuffix + sizeof(st80211WlanMngmntFrmFixedPara);

      if(MT_AUTH_POLICY_WEPSHAREDKEY == usAuthAlgrm)
                {
    	   if(3 == usAuthSeq)
    	            {
    		   cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&stWlanMangmntFrmTaggedPara,ulTaggedParaLen);
    		   ulSuffix = ulSuffix + ulTaggedParaLen;
    	            }
    	   cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&stVendoeSpecific,ulVendoeSpecificLen);
    	   ulSuffix = ulSuffix + ulVendoeSpecificLen;
                }

      gst80211AuthReq.ulBufLen = ulSuffix;

      cpss_mem_memcpy(gst80211AuthReq.aucBuff,auc80211AuthReq,gst80211AuthReq.ulBufLen);
          }
    else//MSG_POOL
          {
      usFrameCtrl = 45056;
      usDuration = 0;
      usSeqCtrl = 0;

      usFrameCtrl = htons(usFrameCtrl);
      usDuration = htons(usDuration);
      usSeqCtrl = htons(usSeqCtrl);
      memcpy(&gst80211AuthReq.aucBuff[16],&usFrameCtrl,2);
      memcpy(&gst80211AuthReq.aucBuff[18],&usDuration,2);
#if 0
      memcpy(&gst80211AuthReq.aucBuff[20],gastAp[ulApIdx].auApMac,MT_MAC_LENTH);//AP_MAC
#else
      memcpy(&gst80211AuthReq.aucBuff[20],aucBssId,MT_MAC_LENTH);
#endif
      memcpy(&gst80211AuthReq.aucBuff[26],gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);//STA_MAC
      memcpy(&gst80211AuthReq.aucBuff[32],aucBssId,MT_MAC_LENTH);//AP_MAC
      memcpy(&gst80211AuthReq.aucBuff[38],&usSeqCtrl,2);//Seq_Ctrl
          }

          /*发送STA的认证请求消息*/
    bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gst80211AuthReq.aucBuff, gst80211AuthReq.ulBufLen);

    if(MT_FALSE == bSendResult)
           {
    	printf("\nApMutiTestSendMsg send 80211AuthReq failure!\nFile:%s.Line:%d\n",__FILE__,__LINE__);
          }

    return bSendResult;
}

MTBool ApMutiTestSendAssocReq(MT_STA_WLAN_INFO_T vstStaWLanInfo)
{
  MTBool bSendResult = MT_FALSE;
  UINT16 usFrameCtrl = 0;
  UINT16 usDuration = 0;
  UINT16 usSeqCtrl = 0;
  UINT8 aucBssId[MT_MAC_LENTH] = {0};

  UINT8 auc80211AssoReq[256] = {0};
  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  UINT8 ucWirelessLen = 0;
  UINT8 ucWDRssi = 0;//dBm
  UINT8 ucWDSnr = 0;//dB
  UINT16 usWDDataRate = 0;//Mbps
  UINT8 aucRscvd[3] = {0};
  UINT8 aucDestMacAdd[MT_MAC_LENTH] = {0};
  UINT8 aucSrcMacAdd[MT_MAC_LENTH] = {0};
  UINT16 usSequenceNum = 0;
  //IEEE802.11Wireless LAN Info Frame
  UINT16 usCapabilitiesInfo;//按Bit计算
  UINT16 usListenIntval;
  UINT8 ucSsidParaTagNum = 0;
  UINT8 ucSsidParaTagLen = 0;
  char *pucSsidParaSsidName = NULL;
  UINT32 ulApIdx = 0;
  UINT32 ulStaIndex = 0;
  UINT8 ucRadioId = 0;
  UINT8 ucWlanId = 0;
  MT_VENDOR_SPECIFIC_INFO_T stVendorSpec;
  MT_WLAN_SUPPORT_RATES_T stSupportRates;
  MT_WLAN_EXTEND_SUPPORT_RATES_T stExtSupportRates;
  UINT32 ulSuffix = 0;

  cpss_mem_memset(&stVendorSpec,0,sizeof(stVendorSpec));
  cpss_mem_memset(&stSupportRates,0,sizeof(stSupportRates));
  cpss_mem_memset(&stExtSupportRates,0,sizeof(stExtSupportRates));

  ulApIdx = vstStaWLanInfo.ulApIndex;
  ulStaIndex = vstStaWLanInfo.ulStaIndex;
  ucRadioId = vstStaWLanInfo.ucRadioId;
  ucWlanId = vstStaWLanInfo.ucWlanId;
  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);

  if(MT_FALSE == gbTestMod)//PARACONF
     {
	  cpss_mem_memset(&gst80211AssocReq,0,sizeof(MT_CAPWAPMSG_T));
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,1);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,1);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,1);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,1);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,0);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

    ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

    //Fragment ID
    MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
    //Fragment Offset
    MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
    //Reserved
    MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

    ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

    //Wireless Data
    ucWirelessLen = 4;
    ucWDRssi = 0;//dBm
    ucWDSnr = 0;//dB
    usWDDataRate = 0;//Mbps

    aucRscvd[0] = 0;
    aucRscvd[1] = 0;
    aucRscvd[2] = 0;

    usFrameCtrl = 0x0000;
    usDuration = htons(0);
#if 0
    cpss_mem_memcpy(aucDestMacAdd,gastAp[ulApIdx].auApMac,MT_MAC_LENTH);//AP_MAC
#else
    cpss_mem_memcpy(aucDestMacAdd,aucBssId,MT_MAC_LENTH);
#endif
    if(ulApIdx == getApIDbyBssid(ulStaIndex))
    {
    	cpss_mem_memcpy(aucSrcMacAdd,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);//STA_MAC
    }
    //BssId
    usSequenceNum = htons(0);

    usCapabilitiesInfo = 1057;//按Bit计算
    //usCapabilitiesInfo = htons(usCapabilitiesInfo);
    usListenIntval = 10;
    //usListenIntval = htons(usListenIntval);

    //SSID
    ucSsidParaTagNum = 0;
    pucSsidParaSsidName = "CMCC_WLAN";
    ucSsidParaTagLen = strlen(pucSsidParaSsidName);

    //Support Rates 计算方法：128+rate*2
    stSupportRates.ucTagNum = 1;
    stSupportRates.ucTagLen = 8;
    stSupportRates.ucSupportRate[0] = 128 + 1 * 2;//1Mbps
    stSupportRates.ucSupportRate[1] = 128 + 2 * 2;//2Mbps
    stSupportRates.ucSupportRate[2] = 128 + 5.5 * 2;//5.5Mbps
    stSupportRates.ucSupportRate[3] = 128 + 11 * 2;//11Mbps
    stSupportRates.ucSupportRate[4] = 128 + 6 * 2;//6Mbps
    stSupportRates.ucSupportRate[5] = 128 + 9 * 2;//9Mbps
    stSupportRates.ucSupportRate[6] = 128 + 12 * 2;//12Mbps
    stSupportRates.ucSupportRate[7] = 128 + 18 * 2;//18Mbps

    //Extend Rates 计算方法：rate*2
    stExtSupportRates.ucTagNum = 50;
    stExtSupportRates.ucTagLen = 4;
    stExtSupportRates.ucExtSupportRate[0] = 24 * 2;//24
    stExtSupportRates.ucExtSupportRate[1] = 36 * 2;//36
    stExtSupportRates.ucExtSupportRate[2] = 48 * 2;//48
    stExtSupportRates.ucExtSupportRate[3] = 54 * 2;//54

    //Vendor Specific
    stVendorSpec.ucTageNum = 221;
    stVendorSpec.ucTagLen = 7;
    stVendorSpec.aucOui[0] = 0x00;
    stVendorSpec.aucOui[1] = 0x50;
    stVendorSpec.aucOui[2] = 0xf2;
    stVendorSpec.ucVendorOuiType = 2;
    stVendorSpec.ucWmeSubtype = 0;
    stVendorSpec.ucWmeVer = 1;
    stVendorSpec.ucWmeQoS = 0;

    ulSuffix = 0;
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
    ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);

    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
    ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);

    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&ucWirelessLen,sizeof(ucWirelessLen));
    ulSuffix = ulSuffix + sizeof(ucWirelessLen);
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&ucWDRssi,sizeof(ucWDRssi));
    ulSuffix = ulSuffix + sizeof(ucWDRssi);
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&ucWDSnr,sizeof(ucWDSnr));
    ulSuffix = ulSuffix + sizeof(ucWDSnr);
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&usWDDataRate,sizeof(usWDDataRate));
    ulSuffix = ulSuffix + sizeof(usWDDataRate);

    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],aucRscvd,3);
    ulSuffix = ulSuffix + 3;

    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&usFrameCtrl,sizeof(usFrameCtrl));
    ulSuffix = ulSuffix + sizeof(usFrameCtrl);
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&usDuration,sizeof(usDuration));
    ulSuffix = ulSuffix + sizeof(usDuration);
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],aucDestMacAdd,MT_MAC_LENTH);
    ulSuffix = ulSuffix + MT_MAC_LENTH;
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],aucSrcMacAdd,MT_MAC_LENTH);
    ulSuffix = ulSuffix + MT_MAC_LENTH;
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],aucBssId,MT_MAC_LENTH);
    ulSuffix = ulSuffix + MT_MAC_LENTH;
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&usSequenceNum,sizeof(usSequenceNum));
    ulSuffix = ulSuffix + sizeof(usSequenceNum);
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&usCapabilitiesInfo,sizeof(usCapabilitiesInfo));
    ulSuffix = ulSuffix + sizeof(usCapabilitiesInfo);
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&usListenIntval,sizeof(usListenIntval));
    ulSuffix = ulSuffix + sizeof(usListenIntval);
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&ucSsidParaTagNum,sizeof(ucSsidParaTagNum));
    ulSuffix = ulSuffix + sizeof(ucSsidParaTagNum);
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&ucSsidParaTagLen,sizeof(ucSsidParaTagLen));
    ulSuffix = ulSuffix + sizeof(ucSsidParaTagLen);
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&pucSsidParaSsidName,ucSsidParaTagLen);
    ulSuffix = ulSuffix + ucSsidParaTagLen;
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&stSupportRates,sizeof(stSupportRates));
    ulSuffix = ulSuffix + sizeof(stSupportRates);
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&stExtSupportRates,sizeof(stExtSupportRates));
    ulSuffix = ulSuffix + sizeof(stExtSupportRates);
    cpss_mem_memcpy(&auc80211AssoReq[ulSuffix],&stVendorSpec,sizeof(stVendorSpec));
    ulSuffix = ulSuffix + sizeof(stVendorSpec);

    gst80211AssocReq.ulBufLen = ulSuffix;
    cpss_mem_memcpy(gst80211AssocReq.aucBuff,auc80211AssoReq,gst80211AssocReq.ulBufLen);
     }
  else//MSG_POOL
     {
    usFrameCtrl = 0;
    usDuration = 0;
    usSeqCtrl = 0;

    usFrameCtrl = htons(usFrameCtrl);
    usDuration = htons(usDuration);
    usSeqCtrl = htons(usSeqCtrl);
    memcpy(&gst80211AssocReq.aucBuff[16],&usFrameCtrl,2);
    memcpy(&gst80211AssocReq.aucBuff[18],&usDuration,2);
#if 0
    memcpy(&gst80211AssocReq.aucBuff[20],gastAp[ulApIdx].auApMac,MT_MAC_LENTH);//AP_MAC
#else
    memcpy(&gst80211AssocReq.aucBuff[20],aucBssId,MT_MAC_LENTH);
#endif
    if(ulApIdx == getApIDbyBssid(ulStaIndex))
    {
    	memcpy(&gst80211AssocReq.aucBuff[26],gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);//STA_MAC
    }
    memcpy(&gst80211AssocReq.aucBuff[32],aucBssId,MT_MAC_LENTH);//AP_MAC
    memcpy(&gst80211AssocReq.aucBuff[38],&usSeqCtrl,2);//Seq_Ctrl
      }

    bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gst80211AssocReq.aucBuff,gst80211AssocReq.ulBufLen);
    if(MT_FALSE == bSendResult)
          {
        printf("ERROR.ApMutiTestSendAssocReq failure.\nFile:%s,Line:%d\n",__FILE__,__LINE__);
          }

  return bSendResult;
}

/*发送解除认证请求消息STA->AC*/
MTBool ApMutiTestSend80211DeAuthReq(MT_STA_AUTH_INFO vstStaAuthInfo)
{
  MTBool bSendResult = MT_FALSE;
  UINT16 usFrameCtrl = 0;
  UINT16 usDuration = 0;
  UINT16 usSeqCtrl = 0;
  UINT8 aucBssId[MT_MAC_LENTH] = {0};
  UINT8 aucDestMacAdd[MT_MAC_LENTH] = {0};
  UINT8 aucSrcMacAdd[MT_MAC_LENTH] = {0};

  UINT8 auc80211DeAuthReq[256] = {0};
  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  UINT8 ucWirelessLen = 4;
  UINT8 ucWDRssi = 0;//dBm
  UINT8 ucWDSnr = 0;//dB
  UINT16 usWDDataRate = 0;//Mbps
  UINT8 aucRscvd[3] = {0};
  UINT16 usSequenceNum = 0;
  UINT16 usReasnCode = 0;
  UINT32 ulApIdx = 0;
  UINT32 ulStaIndex = 0;
  UINT8 ucRadioId = 0;
  UINT8 ucWlanId = 0;
  UINT32 ulSuffix = 0;

  ulApIdx = vstStaAuthInfo.ulApIndex;
  ulStaIndex = vstStaAuthInfo.ulStaIndex;
  ucRadioId = vstStaAuthInfo.ucRadioId;
  ucWlanId = vstStaAuthInfo.ucWlanId;
  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);

  if(MT_FALSE == gbTestMod)//PARACONF
     {
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,1);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,1);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,1);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,1);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,0);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

    ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

    //Fragment ID
    MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
    //Fragment Offset
    MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
    //Reserved
    MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

    ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

    //Wireless Data
    ucWirelessLen = 4;
    ucWDRssi = 0;//dBm
    ucWDSnr = 0;//dB
    usWDDataRate = 0;//Mbps

    aucRscvd[0] = 0;
    aucRscvd[1] = 0;
    aucRscvd[2] = 0;

    usFrameCtrl = 0x00C0;

    usDuration = 0;
    //usDuration = htons(usDuration);
#if 0
    cpss_mem_memcpy(aucDestMacAdd,gastAp[ulApIdx].auApMac,MT_MAC_LENTH);//AP_MAC
#else
    cpss_mem_memcpy(aucDestMacAdd,aucBssId,MT_MAC_LENTH);
#endif
    cpss_mem_memcpy(aucSrcMacAdd,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);//STA_MAC
    //BssId
    usSequenceNum = htons(0);
    usReasnCode = 1;//htons(1);

    ulSuffix = 0;
    cpss_mem_memcpy(&auc80211DeAuthReq[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
    ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);

    cpss_mem_memcpy(&auc80211DeAuthReq[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
    ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);

    cpss_mem_memcpy(&auc80211DeAuthReq[ulSuffix],&ucWirelessLen,sizeof(ucWirelessLen));
    ulSuffix = ulSuffix + sizeof(ucWirelessLen);
    cpss_mem_memcpy(&auc80211DeAuthReq[ulSuffix],&ucWDRssi,sizeof(ucWDRssi));
    ulSuffix = ulSuffix + sizeof(ucWDRssi);
    cpss_mem_memcpy(&auc80211DeAuthReq[ulSuffix],&ucWDSnr,sizeof(ucWDSnr));
    ulSuffix = ulSuffix + sizeof(ucWDSnr);
    cpss_mem_memcpy(&auc80211DeAuthReq[ulSuffix],&usWDDataRate,sizeof(usWDDataRate));
    ulSuffix = ulSuffix + sizeof(usWDDataRate);

    cpss_mem_memcpy(&auc80211DeAuthReq[ulSuffix],aucRscvd,3);
    ulSuffix = ulSuffix + 3;

    cpss_mem_memcpy(&auc80211DeAuthReq[ulSuffix],&usFrameCtrl,sizeof(usFrameCtrl));
    ulSuffix = ulSuffix + sizeof(usFrameCtrl);
    cpss_mem_memcpy(&auc80211DeAuthReq[ulSuffix],&usDuration,sizeof(usDuration));
    ulSuffix = ulSuffix + sizeof(usDuration);
#if 1
    switch(gulDeAuthTestFlag)
          {
    case 0:
#if 0
    	aucDestMacAdd[0] = aucBssId[0];
    	aucDestMacAdd[1] = aucBssId[1];
    	aucDestMacAdd[2] = aucBssId[2];
    	aucDestMacAdd[3] = aucBssId[3];
    	aucDestMacAdd[4] = aucBssId[4];
    	aucDestMacAdd[5] = aucBssId[5];

    	aucSrcMacAdd[0] = ;
    	aucSrcMacAdd[1] = ;
    	aucSrcMacAdd[2] = ;
    	aucSrcMacAdd[3] = ;
    	aucSrcMacAdd[4] = ;
    	aucSrcMacAdd[5] = ;

    	aucBssId[0] = aucDestMacAdd[0];
    	aucBssId[1] = aucDestMacAdd[1];
    	aucBssId[2] = aucDestMacAdd[2];
    	aucBssId[3] = aucDestMacAdd[3];
    	aucBssId[4] = aucDestMacAdd[4];
    	aucBssId[5] = aucDestMacAdd[5];
#endif
    	break;

    case 1://1：MAC2取值正确，MAC1与MAC3值不同；
#if 0
    	aucDestMacAdd[0] = ;
    	aucDestMacAdd[1] = ;
    	aucDestMacAdd[2] = ;
    	aucDestMacAdd[3] = ;
    	aucDestMacAdd[4] = ;
    	aucDestMacAdd[5] = ;
#endif

#if 0
    	aucSrcMacAdd[0] = ;
    	aucSrcMacAdd[1] = ;
    	aucSrcMacAdd[2] = ;
    	aucSrcMacAdd[3] = ;
    	aucSrcMacAdd[4] = ;
    	aucSrcMacAdd[5] = ;
#endif
#if 0
    	aucBssId[0] = ;
    	aucBssId[1] = ;
    	aucBssId[2] = ;
    	aucBssId[3] = ;
    	aucBssId[4] = ;
    	aucBssId[5] = ;
#endif
    	break;

    case 2://2：MAC2取值正确，MAC1与MAC3相同，但值错误；
    	aucDestMacAdd[0] = 0x02;
    	aucDestMacAdd[1] = 0x02;
    	aucDestMacAdd[2] = 0x02;
    	aucDestMacAdd[3] = 0x02;
    	aucDestMacAdd[4] = 0x02;
    	aucDestMacAdd[5] = 0x02;

#if 0
    	aucSrcMacAdd[0] = ;
    	aucSrcMacAdd[1] = ;
    	aucSrcMacAdd[2] = ;
    	aucSrcMacAdd[3] = ;
    	aucSrcMacAdd[4] = ;
    	aucSrcMacAdd[5] = ;
#endif

    	aucBssId[0] = aucDestMacAdd[0];
    	aucBssId[1] = aucDestMacAdd[1];
    	aucBssId[2] = aucDestMacAdd[2];
    	aucBssId[3] = aucDestMacAdd[3];
    	aucBssId[4] = aucDestMacAdd[4];
    	aucBssId[5] = aucDestMacAdd[5];
    	break;

    case 3://3：MAC2取值错误，不是任何在线STA，MAC1与MAC3相同且正确；
#if 0
    	aucDestMacAdd[0] = ;
    	aucDestMacAdd[1] = ;
    	aucDestMacAdd[2] = ;
    	aucDestMacAdd[3] = ;
    	aucDestMacAdd[4] = ;
    	aucDestMacAdd[5] = ;
#endif

    	aucSrcMacAdd[0] = 0x03;
    	aucSrcMacAdd[1] = 0x03;
    	aucSrcMacAdd[2] = 0x03;
    	aucSrcMacAdd[3] = 0x03;
    	aucSrcMacAdd[4] = 0x03;
    	aucSrcMacAdd[5] = 0x03;

    	aucBssId[0] = aucDestMacAdd[0];
    	aucBssId[1] = aucDestMacAdd[1];
    	aucBssId[2] = aucDestMacAdd[2];
    	aucBssId[3] = aucDestMacAdd[3];
    	aucBssId[4] = aucDestMacAdd[4];
    	aucBssId[5] = aucDestMacAdd[5];
     break;

    case 4://4：MAC2取值错误，不是任何在线STA，MAC1与MAC3相同且错误；
    	aucDestMacAdd[0] = 0x07;
    	aucDestMacAdd[1] = 0x07;
    	aucDestMacAdd[2] = 0x07;
    	aucDestMacAdd[3] = 0x07;
    	aucDestMacAdd[4] = 0x07;
    	aucDestMacAdd[5] = 0x07;

    	aucSrcMacAdd[0] = 0x04;
    	aucSrcMacAdd[1] = 0x04;
    	aucSrcMacAdd[2] = 0x04;
    	aucSrcMacAdd[3] = 0x04;
    	aucSrcMacAdd[4] = 0x04;
    	aucSrcMacAdd[5] = 0x04;

    	aucBssId[0] = aucDestMacAdd[0];
    	aucBssId[1] = aucDestMacAdd[1];
    	aucBssId[2] = aucDestMacAdd[2];
    	aucBssId[3] = aucDestMacAdd[3];
    	aucBssId[4] = aucDestMacAdd[4];
    	aucBssId[5] = aucDestMacAdd[5];
    	break;

    case 5://5：MAC2取值正确，MAC1与MAC3相同且值正确；
#if 0
    	aucDestMacAdd[0] = ;
    	aucDestMacAdd[1] = ;
    	aucDestMacAdd[2] = ;
    	aucDestMacAdd[3] = ;
    	aucDestMacAdd[4] = ;
    	aucDestMacAdd[5] = ;
#endif

#if 0
    	aucSrcMacAdd[0] = ;
    	aucSrcMacAdd[1] = ;
    	aucSrcMacAdd[2] = ;
    	aucSrcMacAdd[3] = ;
    	aucSrcMacAdd[4] = ;
    	aucSrcMacAdd[5] = ;
#endif
    	aucBssId[0] = aucDestMacAdd[0];
    	aucBssId[1] = aucDestMacAdd[1];
    	aucBssId[2] = aucDestMacAdd[2];
    	aucBssId[3] = aucDestMacAdd[3];
    	aucBssId[4] = aucDestMacAdd[4];
    	aucBssId[5] = aucDestMacAdd[5];
    	break;

    default:
    	break;
          }
#endif

    cpss_mem_memcpy(&auc80211DeAuthReq[ulSuffix],aucDestMacAdd,MT_MAC_LENTH);
    ulSuffix = ulSuffix + MT_MAC_LENTH;
    cpss_mem_memcpy(&auc80211DeAuthReq[ulSuffix],aucSrcMacAdd,MT_MAC_LENTH);
    ulSuffix = ulSuffix + MT_MAC_LENTH;
    cpss_mem_memcpy(&auc80211DeAuthReq[ulSuffix],aucBssId,MT_MAC_LENTH);
    ulSuffix = ulSuffix + MT_MAC_LENTH;
    cpss_mem_memcpy(&auc80211DeAuthReq[ulSuffix],&usSequenceNum,sizeof(usSequenceNum));
    ulSuffix = ulSuffix + sizeof(usSequenceNum);
    cpss_mem_memcpy(&auc80211DeAuthReq[ulSuffix],&usReasnCode,sizeof(usReasnCode));
    ulSuffix = ulSuffix + sizeof(usReasnCode);

    gst80211DeAuthReq.ulBufLen = ulSuffix;
    cpss_mem_memcpy(gst80211DeAuthReq.aucBuff,auc80211DeAuthReq,gst80211DeAuthReq.ulBufLen);
     }
  else//MSG_POOL
     {
    usFrameCtrl = 49152;//0xc000
    usDuration = 0;
    usSeqCtrl = 0;

    usFrameCtrl = htons(usFrameCtrl);
    usDuration = htons(usDuration);
    usSeqCtrl = htons(usSeqCtrl);
    memcpy(&gst80211DeAuthReq.aucBuff[16],&usFrameCtrl,2);
    memcpy(&gst80211DeAuthReq.aucBuff[18],&usDuration,2);
#if 0
    memcpy(&gst80211DeAuthReq.aucBuff[20],gastAp[ulApIdx].auApMac,MT_MAC_LENTH);//AP_MAC
#else
    memcpy(&gst80211DeAuthReq.aucBuff[20],aucBssId,MT_MAC_LENTH);
#endif
    memcpy(&gst80211DeAuthReq.aucBuff[26],gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);//STA_MAC

    memcpy(&gst80211DeAuthReq.aucBuff[32],aucBssId,MT_MAC_LENTH);//AP_MAC
    memcpy(&gst80211DeAuthReq.aucBuff[38],&usSeqCtrl,2);//Seq_Ctrl
     }
        /*发送STA的认证请求消息*/
  bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gst80211DeAuthReq.aucBuff, gst80211DeAuthReq.ulBufLen);

  if(MT_FALSE == bSendResult)
    {
    printf("ERROR.ApMutiTestSend80211DeAuthReq failure for STA(%d).\nFile:%s.Line:%d.\n",ulStaIndex,__FILE__,__LINE__);
    }

  return bSendResult;
}

//Disassociation
MTBool ApMutiTestSend80211DisAssocReq(MT_STA_WLAN_INFO_T vstStaWLanInfo)
{
  MTBool bSendResult = MT_FALSE;
  UINT16 usFrameCtrl = 0;
  UINT16 usDuration = 0;
  UINT8 aucBssId[MT_MAC_LENTH] = {0};
  UINT8 aucDestMacAdd[MT_MAC_LENTH] = {0};
  UINT8 aucSrcMacAdd[MT_MAC_LENTH] = {0};
  UINT16 usSequenceNum = 0;
  UINT16 usReasnCode = 0;

  UINT8 auc80211DisassocReq[256] = {0};
  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  UINT8 ucWirelessLen = 0;
  UINT8 ucWDRssi = 0;//dBm
  UINT8 ucWDSnr = 0;//dB
  UINT16 usWDDataRate = 0;//Mbps
  UINT8 aucRscvd[3] = {0};
  UINT32 ulApIdx = 0;
  UINT32 ulStaIndex = 0;
  UINT8 ucRadioId = 0;
  UINT8 ucWlanId = 0;
  UINT32 ulSuffix = 0;

  ulApIdx = vstStaWLanInfo.ulApIndex;
  ulStaIndex = vstStaWLanInfo.ulStaIndex;
  ucRadioId = vstStaWLanInfo.ucRadioId;
  ucWlanId = vstStaWLanInfo.ucWlanId;

  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);

  //capwap Header
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,1);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,1);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,1);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,1);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,0);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

  //Fragment ID
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
  //Fragment Offset
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
  //Reserved
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

  //Wireless data
  ucWirelessLen = 4;
  ucWDRssi = 0;//dBm
  ucWDSnr = 0;//dB
  usWDDataRate = 0;//Mbps

  aucRscvd[0] = 0;
  aucRscvd[1] = 0;
  aucRscvd[2] = 0;

  usFrameCtrl = 0x00A0;
  usDuration = htons(0);
#if 0
  cpss_mem_memcpy(aucDestMacAdd,gastAp[ulApIdx].auApMac,MT_MAC_LENTH);//AP_MAC
#else
  cpss_mem_memcpy(aucDestMacAdd,aucBssId,MT_MAC_LENTH);
#endif
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(aucSrcMacAdd,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);//STA_MAC
  }

  usSequenceNum = htons(0);
  usReasnCode = 8;

  //80211DisassocReq
  ulSuffix = 0;
  cpss_mem_memcpy(&auc80211DisassocReq[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);

  cpss_mem_memcpy(&auc80211DisassocReq[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);

  cpss_mem_memcpy(&auc80211DisassocReq[ulSuffix],&ucWirelessLen,sizeof(ucWirelessLen));
  ulSuffix = ulSuffix + sizeof(ucWirelessLen);
  cpss_mem_memcpy(&auc80211DisassocReq[ulSuffix],&ucWDRssi,sizeof(ucWDRssi));
  ulSuffix = ulSuffix + sizeof(ucWDRssi);
  cpss_mem_memcpy(&auc80211DisassocReq[ulSuffix],&ucWDSnr,sizeof(ucWDSnr));
  ulSuffix = ulSuffix + sizeof(ucWDSnr);
  cpss_mem_memcpy(&auc80211DisassocReq[ulSuffix],&usWDDataRate,sizeof(usWDDataRate));
  ulSuffix = ulSuffix + sizeof(usWDDataRate);
  cpss_mem_memcpy(&auc80211DisassocReq[ulSuffix],aucRscvd,3);
  ulSuffix = ulSuffix + 3;
  cpss_mem_memcpy(&auc80211DisassocReq[ulSuffix],&usFrameCtrl,sizeof(usFrameCtrl));
  ulSuffix = ulSuffix + sizeof(usFrameCtrl);
  cpss_mem_memcpy(&auc80211DisassocReq[ulSuffix],&usDuration,sizeof(usDuration));
  ulSuffix = ulSuffix + sizeof(usDuration);

#if 1
    switch(gulDeAssoTestFlag)
          {
    case 0://正常
#if 0
    	aucDestMacAdd[0] = aucBssId[0];
    	aucDestMacAdd[1] = aucBssId[1];
    	aucDestMacAdd[2] = aucBssId[2];
    	aucDestMacAdd[3] = aucBssId[3];
    	aucDestMacAdd[4] = aucBssId[4];
    	aucDestMacAdd[5] = aucBssId[5];

    	aucSrcMacAdd[0] = ;
    	aucSrcMacAdd[1] = ;
    	aucSrcMacAdd[2] = ;
    	aucSrcMacAdd[3] = ;
    	aucSrcMacAdd[4] = ;
    	aucSrcMacAdd[5] = ;

    	aucBssId[0] = aucDestMacAdd[0];
    	aucBssId[1] = aucDestMacAdd[1];
    	aucBssId[2] = aucDestMacAdd[2];
    	aucBssId[3] = aucDestMacAdd[3];
    	aucBssId[4] = aucDestMacAdd[4];
    	aucBssId[5] = aucDestMacAdd[5];
#endif
    	break;

    case 1://1：MAC2取值正确，MAC1与MAC3值不同；
#if 0
    	aucDestMacAdd[0] = ;
    	aucDestMacAdd[1] = ;
    	aucDestMacAdd[2] = ;
    	aucDestMacAdd[3] = ;
    	aucDestMacAdd[4] = ;
    	aucDestMacAdd[5] = ;
#endif

#if 0
    	aucSrcMacAdd[0] = ;
    	aucSrcMacAdd[1] = ;
    	aucSrcMacAdd[2] = ;
    	aucSrcMacAdd[3] = ;
    	aucSrcMacAdd[4] = ;
    	aucSrcMacAdd[5] = ;
#endif
#if 0
    	aucBssId[0] = ;
    	aucBssId[1] = ;
    	aucBssId[2] = ;
    	aucBssId[3] = ;
    	aucBssId[4] = ;
    	aucBssId[5] = ;
#endif
    	break;

    case 2://2：MAC2取值正确，MAC1与MAC3相同，但值错误；
    	aucDestMacAdd[0] = 0x02;
    	aucDestMacAdd[1] = 0x02;
    	aucDestMacAdd[2] = 0x02;
    	aucDestMacAdd[3] = 0x02;
    	aucDestMacAdd[4] = 0x02;
    	aucDestMacAdd[5] = 0x02;

#if 0
    	aucSrcMacAdd[0] = ;
    	aucSrcMacAdd[1] = ;
    	aucSrcMacAdd[2] = ;
    	aucSrcMacAdd[3] = ;
    	aucSrcMacAdd[4] = ;
    	aucSrcMacAdd[5] = ;
#endif

    	aucBssId[0] = aucDestMacAdd[0];
    	aucBssId[1] = aucDestMacAdd[1];
    	aucBssId[2] = aucDestMacAdd[2];
    	aucBssId[3] = aucDestMacAdd[3];
    	aucBssId[4] = aucDestMacAdd[4];
    	aucBssId[5] = aucDestMacAdd[5];
    	break;

    case 3://3：MAC2取值错误，不是任何在线STA，MAC1与MAC3相同且正确；
#if 0
    	aucDestMacAdd[0] = ;
    	aucDestMacAdd[1] = ;
    	aucDestMacAdd[2] = ;
    	aucDestMacAdd[3] = ;
    	aucDestMacAdd[4] = ;
    	aucDestMacAdd[5] = ;
#endif

    	aucSrcMacAdd[0] = 0x03;
    	aucSrcMacAdd[1] = 0x03;
    	aucSrcMacAdd[2] = 0x03;
    	aucSrcMacAdd[3] = 0x03;
    	aucSrcMacAdd[4] = 0x03;
    	aucSrcMacAdd[5] = 0x03;

    	aucBssId[0] = aucDestMacAdd[0];
    	aucBssId[1] = aucDestMacAdd[1];
    	aucBssId[2] = aucDestMacAdd[2];
    	aucBssId[3] = aucDestMacAdd[3];
    	aucBssId[4] = aucDestMacAdd[4];
    	aucBssId[5] = aucDestMacAdd[5];
     break;

    case 4://4：MAC2取值错误，不是任何在线STA，MAC1与MAC3相同且错误；
    	aucDestMacAdd[0] = 0x07;
    	aucDestMacAdd[1] = 0x07;
    	aucDestMacAdd[2] = 0x07;
    	aucDestMacAdd[3] = 0x07;
    	aucDestMacAdd[4] = 0x07;
    	aucDestMacAdd[5] = 0x07;

    	aucSrcMacAdd[0] = 0x04;
    	aucSrcMacAdd[1] = 0x04;
    	aucSrcMacAdd[2] = 0x04;
    	aucSrcMacAdd[3] = 0x04;
    	aucSrcMacAdd[4] = 0x04;
    	aucSrcMacAdd[5] = 0x04;

    	aucBssId[0] = aucDestMacAdd[0];
    	aucBssId[1] = aucDestMacAdd[1];
    	aucBssId[2] = aucDestMacAdd[2];
    	aucBssId[3] = aucDestMacAdd[3];
    	aucBssId[4] = aucDestMacAdd[4];
    	aucBssId[5] = aucDestMacAdd[5];
    	break;

    case 5://5：MAC2取值正确，MAC1与MAC3相同且值正确；
#if 0
    	aucDestMacAdd[0] = ;
    	aucDestMacAdd[1] = ;
    	aucDestMacAdd[2] = ;
    	aucDestMacAdd[3] = ;
    	aucDestMacAdd[4] = ;
    	aucDestMacAdd[5] = ;
#endif

#if 0
    	aucSrcMacAdd[0] = ;
    	aucSrcMacAdd[1] = ;
    	aucSrcMacAdd[2] = ;
    	aucSrcMacAdd[3] = ;
    	aucSrcMacAdd[4] = ;
    	aucSrcMacAdd[5] = ;
#endif
    	aucBssId[0] = aucDestMacAdd[0];
    	aucBssId[1] = aucDestMacAdd[1];
    	aucBssId[2] = aucDestMacAdd[2];
    	aucBssId[3] = aucDestMacAdd[3];
    	aucBssId[4] = aucDestMacAdd[4];
    	aucBssId[5] = aucDestMacAdd[5];
    	break;

    default:
    	break;
          }
#endif

  cpss_mem_memcpy(&auc80211DisassocReq[ulSuffix],aucDestMacAdd,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  cpss_mem_memcpy(&auc80211DisassocReq[ulSuffix],aucSrcMacAdd,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  cpss_mem_memcpy(&auc80211DisassocReq[ulSuffix],aucBssId,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  cpss_mem_memcpy(&auc80211DisassocReq[ulSuffix],&usSequenceNum,sizeof(usSequenceNum));
  ulSuffix = ulSuffix + sizeof(usSequenceNum);
  cpss_mem_memcpy(&auc80211DisassocReq[ulSuffix],&usReasnCode,sizeof(usReasnCode));
  ulSuffix = ulSuffix + sizeof(usReasnCode);

  gst80211DisAssocReq.ulBufLen = ulSuffix;
  cpss_mem_memcpy(gst80211DisAssocReq.aucBuff,auc80211DisassocReq,gst80211DisAssocReq.ulBufLen);

  bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gst80211DisAssocReq.aucBuff, gst80211DisAssocReq.ulBufLen);

  if(MT_FALSE == bSendResult)
    {
    printf("ERROR.ApMutiTestSend80211DisAssocReq failure for STA(%d).\nFile:%s.Line:%d.\n",ulStaIndex,__FILE__,__LINE__);
    }

  return bSendResult;
}

//IEEE 8021xAuthRsp
MTBool ApMutiTestSend8021xAuthRsp(UINT32 vulApIdx, UINT32 vulStaIndex,MT_8021X_EAPOL_INFO_T vst8021xEapolInfo)
{
  MTBool bSendResult = MT_FALSE;
  UINT8 aucBssId[MT_MAC_LENTH] = {0};
  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  UINT8 ucWirelessLen = 0;
  UINT8 aucWirelessData[MT_MAC_LENTH] = {0};//BSSID
  UINT8 ucRcvd = 0;
  MT_ETH_HDR_T stEthHdr;
  IEEE8021XAUTH_REQ_T st801xAuthRsp;
  UINT32 ul801xAuthRspLen = 0;
  UINT8 ucRadioId = 0;
  UINT8 ucWlanId = 0;
  UINT8 ucWlanIndex = 0;
  UINT32 ulSuffix = 0;
  UINT32 ulLoop = 0;
  //usKeyInfo
  UINT8 ucEKFlag = 0;//Encrypted Key data Flag(0,4,0)
  UINT8 ucRFlag = 0;//Request Flag(4,1,0)
  UINT8 ucEFlag = 0;//Error Flag(5,1,0)
  UINT8 ucSFlag = 0;//Secure Flag(6,1,0)
  UINT8 ucKMFlag = 0;//Key MIC Flag(7,1,1)
  UINT8 ucKAFlag = 0;//Key Ack flag(8,1,0)
  UINT8 ucIFlag = 0;//Install Falg(9,1,0)
  UINT8 ucKIFlag = 0;//Key Index(10,2,0)
  UINT8 ucKTFlag = 0;//Key Type(12,1,1)
  UINT8 ucKDFlag = 0;//Key Descriptor Version(13,3,2)

  cpss_mem_memset(&stEthHdr,0,sizeof(stEthHdr));
  cpss_mem_memset(&st801xAuthRsp,0,sizeof(st801xAuthRsp));

  ucRadioId = gastApWlanInfo[vulApIdx].aucRadioList[0];
  if(0 == gastApWlanInfo[vulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
     {
	  printf("ERROR. No WLAN for STA.\nFile: %s,Line: %d\n",__FILE__,__LINE__);
	  return bSendResult;
     }
  else
     {
	  ucWlanIndex = vulStaIndex % gastApWlanInfo[vulApIdx].astRadioInfo[ucRadioId].ucWlanNum;
	  ucWlanId = gastApWlanInfo[vulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucWlanIndex];
     }

  //Get KeyInfo Flag
  vst8021xEapolInfo.usKeyInfo = htons(vst8021xEapolInfo.usKeyInfo);
  MT_GetField16(vst8021xEapolInfo.usKeyInfo,3,1,ucEKFlag);//Encrypted Key data Flag(3,1,0)
  MT_GetField16(vst8021xEapolInfo.usKeyInfo,4,1,ucRFlag);//Request Flag(4,1,0)
  MT_GetField16(vst8021xEapolInfo.usKeyInfo,5,1,ucEFlag);//Error Flag(5,1,0)
  MT_GetField16(vst8021xEapolInfo.usKeyInfo,6,1,ucSFlag);//Secure Flag(6,1,0)
  MT_GetField16(vst8021xEapolInfo.usKeyInfo,7,1,ucKMFlag);//Key MIC Flag(7,1,1)
  MT_GetField16(vst8021xEapolInfo.usKeyInfo,8,1,ucKAFlag);//Key Ack flag(8,1,0)
  MT_GetField16(vst8021xEapolInfo.usKeyInfo,9,1,ucIFlag);//Install Falg(9,1,0)
  MT_GetField16(vst8021xEapolInfo.usKeyInfo,10,2,ucKIFlag);//Key Index(10,2,0)
  MT_GetField16(vst8021xEapolInfo.usKeyInfo,12,1,ucKTFlag);//Key Type(12,1,1)
  MT_GetField16(vst8021xEapolInfo.usKeyInfo,13,3,ucKDFlag);//Key Descriptor Version(13,3,2)

  //capwap Header
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,1);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,1);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,0);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

  //Fragment ID
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
  //Fragment Offset
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
  //Reserved
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

  //Wireless data
  MT_GetBssID(vulApIdx,ucRadioId,ucWlanId,aucBssId);
  ucWirelessLen = 6;
  cpss_mem_memcpy(aucWirelessData,aucBssId,MT_MAC_LENTH);

  ucRcvd = 0;//Padding for 4 bytes alignment

  cpss_mem_memcpy(stEthHdr.aucDstMac,aucBssId,MT_MAC_LENTH);
  cpss_mem_memcpy(stEthHdr.aucSrcMac,gastStaPara[vulStaIndex].auStaMac,MT_MAC_LENTH);
  stEthHdr.usProtclType = htons(0x888e);

  st801xAuthRsp.ucVer = 1;
  st801xAuthRsp.ucType = 3;
  st801xAuthRsp.usLen = 0;
  st801xAuthRsp.ucDsrpType = 254;
  MT_SetField16(st801xAuthRsp.usKeyInfo,3,1,0);//Encrypted Key data Flag(3,1,0)
  MT_SetField16(st801xAuthRsp.usKeyInfo,4,1,0);//Request Flag(4,1,0)
  MT_SetField16(st801xAuthRsp.usKeyInfo,5,1,0);//Error Flag(5,1,0)
  MT_SetField16(st801xAuthRsp.usKeyInfo,6,1,0);//Secure Flag(6,1,0)
  MT_SetField16(st801xAuthRsp.usKeyInfo,7,1,1);//Key MIC Flag(7,1,1)
  MT_SetField16(st801xAuthRsp.usKeyInfo,8,1,0);//Key Ack flag(8,1,0)
  MT_SetField16(st801xAuthRsp.usKeyInfo,9,1,0);//Install Falg(9,1,0)
  MT_SetField16(st801xAuthRsp.usKeyInfo,10,2,0);//Key Index(10,2,0)
  MT_SetField16(st801xAuthRsp.usKeyInfo,12,1,1);//Key Type(12,1,1)
  MT_SetField16(st801xAuthRsp.usKeyInfo,13,3,2);//Key Descriptor Version(13,3,2)
  st801xAuthRsp.usKeyInfo = htons(st801xAuthRsp.usKeyInfo);
  st801xAuthRsp.usKeyLen = 0;
  st801xAuthRsp.ulRplCntr1 = vst8021xEapolInfo.ulReplCntr1;
  st801xAuthRsp.ulRplCntr2 = vst8021xEapolInfo.ulReplCntr2;

  //Nonce???
  cpss_mem_memcpy(st801xAuthRsp.aucNonce,vst8021xEapolInfo.aucNonce,IEEE8021XAUTH_NONCE_LEN);

  for(ulLoop = 0; ulLoop < IEEE8021XAUTH_KEY_IV_LEN; ulLoop++)
      {
	  st801xAuthRsp.aucKeyIV[ulLoop] = 0;
      }

  for(ulLoop = 0; ulLoop < IEEE8021XAUTH_WPA_KEY_RSC_LEN; ulLoop++)
      {
	  st801xAuthRsp.aucWpaKeyRsc[ulLoop] = 0;
      }

  for(ulLoop = 0; ulLoop < IEEE8021XAUTH_WPA_KEY_ID_LEN; ulLoop++)
      {
	  st801xAuthRsp.aucWpaKeyId[ulLoop] = 0;
      }

  for(ulLoop = 0; ulLoop < IEEE8021XAUTH_WPA_KEY_MIC_LEN; ulLoop++)
      {
	  st801xAuthRsp.aucWpaKeyMic[ulLoop] = 0;
      }

  if(0 == htonl(st801xAuthRsp.ulRplCntr1) && 1 == htonl(st801xAuthRsp.ulRplCntr2))
     {
	  st801xAuthRsp.usWapKeyLen = 16;
     }
  else if(0 == htonl(st801xAuthRsp.ulRplCntr1) && (2 == htonl(st801xAuthRsp.ulRplCntr2) || 3 == htonl(st801xAuthRsp.ulRplCntr2)))
     {
	  st801xAuthRsp.usWapKeyLen = 0;
     }

  cpss_mem_memcpy(st801xAuthRsp.aucWapKey,vst8021xEapolInfo.aucWapKey,st801xAuthRsp.usWapKeyLen);

  st801xAuthRsp.usLen = sizeof(st801xAuthRsp.ucDsrpType)
		  + sizeof(st801xAuthRsp.usKeyInfo)
		  + sizeof(st801xAuthRsp.usKeyLen)
		  + sizeof(st801xAuthRsp.ulRplCntr1)
		  + sizeof(st801xAuthRsp.ulRplCntr2)
		  + IEEE8021XAUTH_NONCE_LEN
		  + IEEE8021XAUTH_KEY_IV_LEN
		  + IEEE8021XAUTH_WPA_KEY_RSC_LEN
		  + IEEE8021XAUTH_WPA_KEY_ID_LEN
		  + IEEE8021XAUTH_WPA_KEY_MIC_LEN
		  + sizeof(st801xAuthRsp.usWapKeyLen)
		  + st801xAuthRsp.usWapKeyLen;

  ul801xAuthRspLen = sizeof(st801xAuthRsp.ucVer)
		  + sizeof(st801xAuthRsp.ucType)
		  + sizeof(st801xAuthRsp.usLen)
		  + st801xAuthRsp.usLen;
  st801xAuthRsp.usLen = htons(st801xAuthRsp.usLen);

  ulSuffix = 0;
  cpss_mem_memcpy(&gst8021xAuthRsp.aucBuff[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
  cpss_mem_memcpy(&gst8021xAuthRsp.aucBuff[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
  cpss_mem_memcpy(&gst8021xAuthRsp.aucBuff[ulSuffix],&ucWirelessLen,sizeof(ucWirelessLen));
  ulSuffix = ulSuffix + sizeof(ucWirelessLen);
  cpss_mem_memcpy(&gst8021xAuthRsp.aucBuff[ulSuffix],aucWirelessData,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  cpss_mem_memcpy(&gst8021xAuthRsp.aucBuff[ulSuffix],&ucRcvd,sizeof(ucRcvd));
  ulSuffix = ulSuffix + sizeof(ucRcvd);
  cpss_mem_memcpy(&gst8021xAuthRsp.aucBuff[ulSuffix],&stEthHdr,sizeof(stEthHdr));
  ulSuffix = ulSuffix + sizeof(stEthHdr);
  cpss_mem_memcpy(&gst8021xAuthRsp.aucBuff[ulSuffix],&st801xAuthRsp,ul801xAuthRspLen);
  ulSuffix = ulSuffix + ul801xAuthRspLen;

  gst8021xAuthRsp.ulBufLen = ulSuffix;

  bSendResult = MT_SendStaMsg2AC(vulApIdx,glUpmSockId, gst8021xAuthRsp.aucBuff, gst8021xAuthRsp.ulBufLen);

  if(MT_FALSE == bSendResult)
    {
    printf("ERROR.ApMutiTestSend8021xAuthRsp failure for STA(%d).\nFile:%s.Line:%d.\n",vulStaIndex,__FILE__,__LINE__);
    }

  return bSendResult;
}

//WAPI Auth Msg UcstKeyNegotiationRsp
MTBool ApMutiTestSendUcstKeyNegotiationRsp(UINT32 vulApIdx, UINT32 vulStaIndex,MT_WAI_AUTH_MSG_INFO_T vstWaiAuthInfo)
{
	MTBool bSendResult = MT_FALSE;
	UINT8 aucBssId[MT_MAC_LENTH] = {0};
	UINT32 ulCapwapHdrFisrt32 = 0;
	UINT32 ulCapwapHdrSecond32 = 0;
	UINT8 ucWirelessLen = 0;
	UINT8 aucWirelessData[MT_MAC_LENTH] = {0};//BSSID
	UINT8 ucRcvd = 0;
	MT_ETH_HDR_T stEthHdr;
	MT_WAI_AUTH_MSG_INFO_T stUcstKeyNgtnRsp;
	UINT32 ulUcstKeyNgtnRspLen = 0;
	UINT8 ucUncstFlag = 0;
	UINT8 aucBKID[WAI_BKID_LEN] = {0};
	UINT8 ucUSKID = 0;
	UINT8 aucAddId[WAI_ADDID_LEN] = {0};//AE_MAC+ASUE_MAC
	UINT8 aucASUE_Chllng[WAI_ASUE_CHALLENGE_LEN] = {0};
	UINT8 aucAE_Chllng[WAI_AE_CHALLENGE_LEN] = {0};
	UINT8 aucWIEfromASUE[WAI_WIE_FROM_ASUE_LEN] = {0};
	UINT8 aucMsgAuthCode[WAI_MSG_AUTH_CODE_LEN] = {0};
	UINT8 ucRadioId = 0;
	UINT8 ucWlanId = 0;
	UINT8 ucWlanIndex = 0;
	UINT32 ulSuffix = 0;
//	UINT32 ulLoop = 0;

	cpss_mem_memset(&stEthHdr,0,sizeof(stEthHdr));
	cpss_mem_memset(&stUcstKeyNgtnRsp,0,sizeof(stUcstKeyNgtnRsp));

	ucRadioId = gastApWlanInfo[vulApIdx].aucRadioList[0];

	if(0 == gastApWlanInfo[vulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
	{
		printf("ERROR. No WLAN for STA.\nFile: %s,Line: %d\n",__FILE__,__LINE__);
		return bSendResult;
	}
	else
	{
		ucWlanIndex = vulStaIndex % gastApWlanInfo[vulApIdx].astRadioInfo[ucRadioId].ucWlanNum;
		ucWlanId = gastApWlanInfo[vulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucWlanIndex];
	}

	//capwap Header
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,1);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,1);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,0);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

	ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

	//Fragment ID
	MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
	//Fragment Offset
	MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
	//Reserved
	MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

	ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

	//Wireless data
	MT_GetBssID(vulApIdx,ucRadioId,ucWlanId,aucBssId);
	ucWirelessLen = 6;
	cpss_mem_memcpy(aucWirelessData,aucBssId,MT_MAC_LENTH);

	ucRcvd = 0;//Padding for 4 bytes alignment

	cpss_mem_memcpy(stEthHdr.aucDstMac,aucBssId,MT_MAC_LENTH);
	if(vulApIdx == getApIDbyBssid(vulStaIndex))
	{
		cpss_mem_memcpy(stEthHdr.aucSrcMac,gastStaPara[vulStaIndex].auStaMac,MT_MAC_LENTH);
	}
	stEthHdr.usProtclType = htons(0x88b4);

	stUcstKeyNgtnRsp.usVer = htons(1);
	stUcstKeyNgtnRsp.ucType = WAI_TYPE_WAI_PROTOCOL_PACKAGE;//WAI protocol package
	stUcstKeyNgtnRsp.ucSubType = WAI_SUBTYPE_UNICASTKEY_NEGOTIATION_RSP;//Uinicast Key Negotiation Response
	stUcstKeyNgtnRsp.usRcvd = 0;
	stUcstKeyNgtnRsp.usLen = 0;
	stUcstKeyNgtnRsp.usSeqNum = htons(1);
	stUcstKeyNgtnRsp.ucFragSeqNum = 0;
	stUcstKeyNgtnRsp.ucFlag = 0;

	ulSuffix = 0;
	ucUncstFlag = 0;
//	aucBKID[16];//How todo
	ucUSKID = 0;
//	aucAddId[12];//AE_MAC+ASUE_MAC
	cpss_mem_memcpy(&aucAddId[0],aucBssId,MT_MAC_LENTH);
	if(vulApIdx == getApIDbyBssid(vulStaIndex))
	{
		cpss_mem_memcpy(&aucAddId[MT_MAC_LENTH],gastStaPara[vulStaIndex].auStaMac,MT_MAC_LENTH);
	}
//	aucASUE_Chllng[32];//How todo
//	aucAE_Chllng[32];//How todo
//	aucWIEfromASUE[22];//How todo
//	aucMsgAuthCode[20];//How todo

	cpss_mem_memcpy(&stUcstKeyNgtnRsp.aucBuff[ulSuffix],&ucUncstFlag,sizeof(ucUncstFlag));
	ulSuffix = ulSuffix + sizeof(ucUncstFlag);
	cpss_mem_memcpy(&stUcstKeyNgtnRsp.aucBuff[ulSuffix],aucBKID,WAI_BKID_LEN);
	ulSuffix = ulSuffix + WAI_BKID_LEN;
	cpss_mem_memcpy(&stUcstKeyNgtnRsp.aucBuff[ulSuffix],&ucUSKID,sizeof(ucUSKID));
	ulSuffix = ulSuffix + sizeof(ucUSKID);
	cpss_mem_memcpy(&stUcstKeyNgtnRsp.aucBuff[ulSuffix],aucAddId,WAI_ADDID_LEN);
	ulSuffix = ulSuffix + WAI_ADDID_LEN;
	cpss_mem_memcpy(&stUcstKeyNgtnRsp.aucBuff[ulSuffix],aucASUE_Chllng,WAI_ASUE_CHALLENGE_LEN);
	ulSuffix = ulSuffix + WAI_ASUE_CHALLENGE_LEN;
	cpss_mem_memcpy(&stUcstKeyNgtnRsp.aucBuff[ulSuffix],aucAE_Chllng,WAI_AE_CHALLENGE_LEN);
	ulSuffix = ulSuffix + WAI_AE_CHALLENGE_LEN;
	cpss_mem_memcpy(&stUcstKeyNgtnRsp.aucBuff[ulSuffix],aucWIEfromASUE,WAI_WIE_FROM_ASUE_LEN);
	ulSuffix = ulSuffix + WAI_WIE_FROM_ASUE_LEN;
	cpss_mem_memcpy(&stUcstKeyNgtnRsp.aucBuff[ulSuffix],aucMsgAuthCode,WAI_MSG_AUTH_CODE_LEN);
	ulSuffix = ulSuffix + WAI_MSG_AUTH_CODE_LEN;

	stUcstKeyNgtnRsp.usLen = sizeof(stUcstKeyNgtnRsp.usVer)
			+ sizeof(stUcstKeyNgtnRsp.ucType)
			+ sizeof(stUcstKeyNgtnRsp.ucSubType)
			+ sizeof(stUcstKeyNgtnRsp.usRcvd)
			+ sizeof(stUcstKeyNgtnRsp.usLen)
			+ sizeof(stUcstKeyNgtnRsp.usSeqNum)
			+ sizeof(stUcstKeyNgtnRsp.ucFragSeqNum)
			+ stUcstKeyNgtnRsp.ucFlag
			+ ulSuffix;
	ulUcstKeyNgtnRspLen = stUcstKeyNgtnRsp.usLen;
	stUcstKeyNgtnRsp.usLen = htons(stUcstKeyNgtnRsp.usLen);

	ulSuffix = 0;
	cpss_mem_memcpy(&gstWapiAuthUncstRsp.aucBuff[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
	ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
	cpss_mem_memcpy(&gstWapiAuthUncstRsp.aucBuff[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
	ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
	cpss_mem_memcpy(&gstWapiAuthUncstRsp.aucBuff[ulSuffix],&ucWirelessLen,sizeof(ucWirelessLen));
	ulSuffix = ulSuffix + sizeof(ucWirelessLen);
	cpss_mem_memcpy(&gstWapiAuthUncstRsp.aucBuff[ulSuffix],aucWirelessData,MT_MAC_LENTH);
	ulSuffix = ulSuffix + MT_MAC_LENTH;
	cpss_mem_memcpy(&gstWapiAuthUncstRsp.aucBuff[ulSuffix],&ucRcvd,sizeof(ucRcvd));
	ulSuffix = ulSuffix + sizeof(ucRcvd);
	cpss_mem_memcpy(&gstWapiAuthUncstRsp.aucBuff[ulSuffix],&stEthHdr,sizeof(stEthHdr));
	ulSuffix = ulSuffix + sizeof(stEthHdr);
	cpss_mem_memcpy(&gstWapiAuthUncstRsp.aucBuff[ulSuffix],&stUcstKeyNgtnRsp,ulUcstKeyNgtnRspLen);
	ulSuffix = ulSuffix + ulUcstKeyNgtnRspLen;

	gstWapiAuthUncstRsp.ulBufLen = ulSuffix;

	bSendResult = MT_SendStaMsg2AC(vulApIdx,glUpmSockId, gstWapiAuthUncstRsp.aucBuff, gstWapiAuthUncstRsp.ulBufLen);

	if(MT_FALSE == bSendResult)
	{
		printf("ERROR.ApMutiTestSendWapiAuthRsp failure for STA(%d).\nFile:%s.Line:%d.\n",vulStaIndex,__FILE__,__LINE__);
	}
	return bSendResult;
}

//McstKeyAnnouncmntRsp
MTBool ApMutiTestSendMcstKeyAnnouncmntRsp(UINT32 vulApIdx, UINT32 vulStaIndex,MT_WAI_AUTH_MSG_INFO_T vstWaiAuthInfo)
{
	MTBool bSendResult = MT_FALSE;
	UINT8 aucBssId[MT_MAC_LENTH] = {0};
	UINT32 ulCapwapHdrFisrt32 = 0;
	UINT32 ulCapwapHdrSecond32 = 0;
	UINT8 ucWirelessLen = 0;
	UINT8 aucWirelessData[MT_MAC_LENTH] = {0};//BSSID
	UINT8 ucRcvd = 0;
	MT_ETH_HDR_T stEthHdr;
	MT_WAI_AUTH_MSG_INFO_T stMcstKeyAnnmntRsp;
	UINT32 ulMcstKeyAnnmntRspLen = 0;
	UINT8 ucMtcstFlag = 0;
	UINT8 ucMSKID = 0;
	UINT8 ucUSKID = 0;
	UINT8 aucAddId[WAI_ADDID_LEN] = {0};//AE_MAC+ASUE_MAC
	UINT8 aucKeyAnncIdntfr[WAI_KEY_ANNOUNCEMENT_IDENTIFIER_LEN] = {0};
	UINT8 aucMsgAuthCode[WAI_MSG_AUTH_CODE_LEN] = {0};
	UINT8 ucRadioId = 0;
	UINT8 ucWlanIndex = 0;
	UINT8 ucWlanId = 0;
	UINT32 ulSuffix = 0;
//	UINT32 ulLoop = 0;

	cpss_mem_memset(&stEthHdr,0,sizeof(stEthHdr));
	cpss_mem_memset(&stMcstKeyAnnmntRsp,0,sizeof(stMcstKeyAnnmntRsp));

	ucRadioId = gastApWlanInfo[vulApIdx].aucRadioList[0];
	if(0 == gastApWlanInfo[vulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
	{
		printf("ERROR. No WLAN for STA.\nFile: %s,Line: %d\n",__FILE__,__LINE__);
		return bSendResult;
	}
	else
	{
		ucWlanIndex = vulStaIndex % gastApWlanInfo[vulApIdx].astRadioInfo[ucRadioId].ucWlanNum;
		ucWlanId = gastApWlanInfo[vulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucWlanIndex];
	}

	//capwap Header
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,1);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,1);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,0);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
	MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

	ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

	//Fragment ID
	MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
	//Fragment Offset
	MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
	//Reserved
	MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

	ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

	//Wireless data
	MT_GetBssID(vulApIdx,ucRadioId,ucWlanId,aucBssId);
	ucWirelessLen = 6;
	cpss_mem_memcpy(aucWirelessData,aucBssId,MT_MAC_LENTH);

	ucRcvd = 0;//Padding for 4 bytes alignment

	cpss_mem_memcpy(stEthHdr.aucDstMac,aucBssId,MT_MAC_LENTH);
	if(vulApIdx == getApIDbyBssid(vulStaIndex))
	{
		cpss_mem_memcpy(stEthHdr.aucSrcMac,gastStaPara[vulStaIndex].auStaMac,MT_MAC_LENTH);
	}
	stEthHdr.usProtclType = htons(0x88b4);

	stMcstKeyAnnmntRsp.usVer = htons(1);
	stMcstKeyAnnmntRsp.ucType = WAI_TYPE_WAI_PROTOCOL_PACKAGE;//WAI protocol package
	stMcstKeyAnnmntRsp.ucSubType = WAI_SUBTYPE_MULTICASTKEY_ANNOUNCEMENT_RSP;//MultiCast Key/STAKey announcement response
	stMcstKeyAnnmntRsp.usRcvd = 0;
	stMcstKeyAnnmntRsp.usLen = 0;
	stMcstKeyAnnmntRsp.usSeqNum = htons(2);
	stMcstKeyAnnmntRsp.ucFragSeqNum = 0;
	stMcstKeyAnnmntRsp.ucFlag = 0;

	ucMtcstFlag = 0;
	ucMSKID = 0;
	ucUSKID = 0;
	//ADDID:AE_MAC+ASUE_MAC
	cpss_mem_memcpy(&aucAddId[0],aucBssId,MT_MAC_LENTH);
	if(vulApIdx == getApIDbyBssid(vulStaIndex))
	{
		cpss_mem_memcpy(&aucAddId[MT_MAC_LENTH],gastStaPara[vulStaIndex].auStaMac,MT_MAC_LENTH);
	}

	//aucKeyAnncIdntfr[WAI_KEY_ANNOUNCEMENT_IDENTIFIER_LEN];//how todo
	//aucMsgAuthCode[WAI_MSG_AUTH_CODE_LEN];// How todo

	ulSuffix = 0;
	cpss_mem_memcpy(&stMcstKeyAnnmntRsp.aucBuff[ulSuffix],&ucMtcstFlag,sizeof(ucMtcstFlag));
	ulSuffix = ulSuffix + sizeof(ucMtcstFlag);
	cpss_mem_memcpy(&stMcstKeyAnnmntRsp.aucBuff[ulSuffix],&ucMSKID,sizeof(ucMSKID));
	ulSuffix = ulSuffix + sizeof(ucMSKID);
	cpss_mem_memcpy(&stMcstKeyAnnmntRsp.aucBuff[ulSuffix],&ucUSKID,sizeof(ucUSKID));
	ulSuffix = ulSuffix + sizeof(ucUSKID);
	cpss_mem_memcpy(&stMcstKeyAnnmntRsp.aucBuff[ulSuffix],aucAddId,WAI_ADDID_LEN);
	ulSuffix = ulSuffix + WAI_ADDID_LEN;
	cpss_mem_memcpy(&stMcstKeyAnnmntRsp.aucBuff[ulSuffix],aucKeyAnncIdntfr,WAI_KEY_ANNOUNCEMENT_IDENTIFIER_LEN);
	ulSuffix = ulSuffix + WAI_KEY_ANNOUNCEMENT_IDENTIFIER_LEN;
	cpss_mem_memcpy(&stMcstKeyAnnmntRsp.aucBuff[ulSuffix],aucMsgAuthCode,WAI_MSG_AUTH_CODE_LEN);
	ulSuffix = ulSuffix + WAI_MSG_AUTH_CODE_LEN;

	stMcstKeyAnnmntRsp.usLen = sizeof(stMcstKeyAnnmntRsp.usVer)
			+ sizeof(stMcstKeyAnnmntRsp.ucType)
			+ sizeof(stMcstKeyAnnmntRsp.ucSubType)
			+ sizeof(stMcstKeyAnnmntRsp.usRcvd)
			+ sizeof(stMcstKeyAnnmntRsp.usLen)
			+ sizeof(stMcstKeyAnnmntRsp.usSeqNum)
			+ sizeof(stMcstKeyAnnmntRsp.ucFragSeqNum)
			+ sizeof(stMcstKeyAnnmntRsp.ucFlag)
			+ ulSuffix;
	ulMcstKeyAnnmntRspLen = stMcstKeyAnnmntRsp.usLen;
	stMcstKeyAnnmntRsp.usLen = htons(stMcstKeyAnnmntRsp.usLen);

	ulSuffix = 0;
	cpss_mem_memcpy(&gstWapiAuthMtcstRsp.aucBuff[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
	ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
	cpss_mem_memcpy(&gstWapiAuthMtcstRsp.aucBuff[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
	ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
	cpss_mem_memcpy(&gstWapiAuthMtcstRsp.aucBuff[ulSuffix],&ucWirelessLen,sizeof(ucWirelessLen));
	ulSuffix = ulSuffix + sizeof(ucWirelessLen);
	cpss_mem_memcpy(&gstWapiAuthMtcstRsp.aucBuff[ulSuffix],aucWirelessData,MT_MAC_LENTH);
	ulSuffix = ulSuffix + MT_MAC_LENTH;
	cpss_mem_memcpy(&gstWapiAuthMtcstRsp.aucBuff[ulSuffix],&ucRcvd,sizeof(ucRcvd));
	ulSuffix = ulSuffix + sizeof(ucRcvd);
	cpss_mem_memcpy(&gstWapiAuthMtcstRsp.aucBuff[ulSuffix],&stEthHdr,sizeof(stEthHdr));
	ulSuffix = ulSuffix + sizeof(stEthHdr);
	cpss_mem_memcpy(&gstWapiAuthMtcstRsp.aucBuff[ulSuffix],&stMcstKeyAnnmntRsp,ulMcstKeyAnnmntRspLen);
	ulSuffix = ulSuffix + ulMcstKeyAnnmntRspLen;

	gstWapiAuthMtcstRsp.ulBufLen = ulSuffix;

	bSendResult = MT_SendStaMsg2AC(vulApIdx,glUpmSockId, gstWapiAuthMtcstRsp.aucBuff, gstWapiAuthMtcstRsp.ulBufLen);

	if(MT_FALSE == bSendResult)
	{
		printf("ERROR.ApMutiTestSendMcstKeyAnnouncmntRsp failure for STA(%d).\nFile:%s.Line:%d.\n",vulStaIndex,__FILE__,__LINE__);
	}
	return bSendResult;
}

//Ping Request IPv4<-->IPv4
MTBool ApMutiTestSendPingReq(MT_STA_WLAN_INFO_T vstStaWLanInfo)
{
  MTBool bSendResult = MT_FALSE;
  UINT8 aucIcmpData[] = {
  0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
  0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69
  };

  UINT8 aucPingReq[256] = {0};
  MT_STA_ICMPV4_T stStaIcmpV4;
  MT_IPV4_HEADER_T stStaIpV4Hdr;
  MT_VLAN_HEADER_T stStaVlanHdr;
  MT_ETH_HDR_T stStaEthHdr;

  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  UINT8 ucMacLen = 0;
  UINT8 aucBssId[MT_MAC_LENTH] = {0};
  UINT8 ucRcvd = 0;
  UINT32 ulApIdx = 0;
  UINT32 ulStaIndex = 0;
  UINT8 ucRadioId = 0;
  UINT8 ucWlanId = 0;
  UINT32 ulSuffix = 0;

  cpss_mem_memset(&stStaIcmpV4,0,sizeof(stStaIcmpV4));
  cpss_mem_memset(&stStaIpV4Hdr,0,sizeof(stStaIpV4Hdr));
  cpss_mem_memset(&stStaVlanHdr,0,sizeof(stStaVlanHdr));
  cpss_mem_memset(&stStaEthHdr,0,sizeof(stStaEthHdr));

  ulApIdx = vstStaWLanInfo.ulApIndex;
  ulStaIndex = vstStaWLanInfo.ulStaIndex;
  ucRadioId = vstStaWLanInfo.ucRadioId;
  ucWlanId = vstStaWLanInfo.ucWlanId;

  //ETHERNET HEADER
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stStaEthHdr.aucSrcMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
	  cpss_mem_memcpy(stStaEthHdr.aucDstMac,gastStaPara[ulStaIndex].auGwMac,MT_MAC_LENTH);
  }
#if 0
  GetMacFromStr(gstStaConfInfo.aucStaSrvMacStr,stStaEthHdr.aucDstMac);
#endif

  if(1 == gstStaConfInfo.ucVlanFlag)
    {
	  stStaEthHdr.usProtclType = htons(0x8100);//VLAN
    }
  else
    {
	  stStaEthHdr.usProtclType = htons(0x0800);//IP
     }

  //VLAN Info
  MT_SetField16(stStaVlanHdr.usPriority,0,3,0);//Priority: Best Effort (default)0
  MT_SetField16(stStaVlanHdr.usPriority,3,1,0);//CFI:Canonical(0)
  MT_SetField16(stStaVlanHdr.usPriority,4,12,gastApconf[ulApIdx].ulStaVlan);//VLAN ID
  stStaVlanHdr.usType = htons(0x0800);

  //IPv4 Header
  stStaIpV4Hdr.ucIpVerType = 0x45;
  if(0 == gstStaConfInfo.ucStaQoSFlag)
     {
    stStaIpV4Hdr.ucDiffSerFd = 0x00;//QoS:DSCP (0x00)
     }
  else
    {
    stStaIpV4Hdr.ucDiffSerFd = 0x38;//QoS:DSCP (0x38)
    }
  stStaIpV4Hdr.usTotalLen = htons(sizeof(stStaIpV4Hdr) + sizeof(stStaIcmpV4));//IPHDR (20) + ICMP (40)
  stStaIpV4Hdr.usIdentfier = htons(0x28d6);
  stStaIpV4Hdr.usFragOffset = 0x0000;
  stStaIpV4Hdr.ucTtl = 128;
  stStaIpV4Hdr.ucProtoType = 1;//ICMP
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stStaIpV4Hdr.aucSrcIpaddr,gastStaPara[ulStaIndex].aucStaIpV4Addr,MT_IPV4_ADDR_LEN);
	  cpss_mem_memcpy(stStaIpV4Hdr.aucDstIpAddr,gastStaPara[ulStaIndex].aucStaGwIpv4Addr,MT_IPV4_ADDR_LEN);
  }
#if 0
  GetIpFromStr(AF_INET,gstStaConfInfo.aucStaSrvIpv4AddrStr,stStaIpV4Hdr.aucDstIpAddr);
#endif

  stStaIpV4Hdr.usChkSum = MT_CalcIpChksm((UINT8*)&stStaIpV4Hdr,20);

  //ICMP Header and Data
  stStaIcmpV4.ucIpProtoType = 0x08;
  stStaIcmpV4.ucIcmpCode = 0x00;
  stStaIcmpV4.usBeIdentifier = htons(0x0A00);
  stStaIcmpV4.usSeqNum = MT_GetSequenceNum(gusSequenceNum);
  stStaIcmpV4.usSeqNum = htons(stStaIcmpV4.usSeqNum);
  cpss_mem_memcpy(stStaIcmpV4.aucData,aucIcmpData,sizeof(aucIcmpData));

  stStaIcmpV4.usIcmpChkSum = MT_CalcIpChksm((UINT8*)&stStaIcmpV4,40);

  //capwap Header
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,1);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,0);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,1);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

  //Fragment ID
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
  //Fragment Offset
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
  //Reserved
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

  ucMacLen = MT_MAC_LENTH;

  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);
  ucRcvd = 0;

  ulSuffix = 0;
  cpss_mem_memcpy(&aucPingReq[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
  cpss_mem_memcpy(&aucPingReq[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
  cpss_mem_memcpy(&aucPingReq[ulSuffix],&ucMacLen,sizeof(ucMacLen));
  ulSuffix = ulSuffix + sizeof(ucMacLen);
  cpss_mem_memcpy(&aucPingReq[ulSuffix],aucBssId,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  cpss_mem_memcpy(&aucPingReq[ulSuffix],&ucRcvd,sizeof(ucRcvd));
  ulSuffix = ulSuffix + sizeof(ucRcvd);
  cpss_mem_memcpy(&aucPingReq[ulSuffix],&stStaEthHdr,sizeof(stStaEthHdr));
  ulSuffix = ulSuffix + sizeof(stStaEthHdr);
  if(1 == gstStaConfInfo.ucVlanFlag)
     {
    cpss_mem_memcpy(&aucPingReq[ulSuffix],&stStaVlanHdr,sizeof(stStaVlanHdr));
    ulSuffix = ulSuffix + sizeof(stStaVlanHdr);
     }
  cpss_mem_memcpy(&aucPingReq[ulSuffix],&stStaIpV4Hdr,sizeof(stStaIpV4Hdr));
  ulSuffix = ulSuffix + sizeof(stStaIpV4Hdr);
  cpss_mem_memcpy(&aucPingReq[ulSuffix],&stStaIcmpV4,sizeof(stStaIcmpV4));
  ulSuffix = ulSuffix + sizeof(stStaIcmpV4);

  gstPingReq.ulBufLen = ulSuffix;

  cpss_mem_memcpy(gstPingReq.aucBuff,aucPingReq,gstPingReq.ulBufLen);

  bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gstPingReq.aucBuff, gstPingReq.ulBufLen);
  if(MT_FALSE == bSendResult)
     {
	  printf("ApMutiTestSendPingReq AP(%d) STA(%d) error.\nFile: %s, Line: %d\n",ulApIdx,ulStaIndex,__FILE__,__LINE__);
     }
  return bSendResult;
}

//Ping Reply
MTBool ApMutiTestSendPingReply(MT_STA_WLAN_INFO_T vstStaWLanInfo, MT_PING_REPLY_STA_INFO vstPingReplyStaInfo)
{
  MTBool bSendResult = MT_FALSE;
  UINT8 aucPingReply[256] = {0};
  UINT8 aucIcmpData[] = {
  0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
  0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69
  };

  MT_STA_ICMPV4_T stIcmpV4;
  MT_IPV4_HEADER_T stIpV4Hdr;
  MT_VLAN_HEADER_T stVlanHdr;
  MT_ETH_HDR_T stEthHdr;

  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  UINT8 ucMacLen = 0;
  UINT8 aucBssId[MT_MAC_LENTH] = {0};
  UINT8 ucRcvd = 0;
  UINT32 ulApIdx = 0;
  UINT32 ulStaIndex = 0;
  UINT8 ucRadioId = 0;
  UINT8 ucWlanId = 0;
  UINT32 ulSuffix = 0;

  cpss_mem_memset(&stEthHdr,0,sizeof(stEthHdr));
  cpss_mem_memset(&stVlanHdr,0,sizeof(stVlanHdr));
  cpss_mem_memset(&stIpV4Hdr,0,sizeof(stIpV4Hdr));
  cpss_mem_memset(&stIcmpV4,0,sizeof(stIcmpV4));
//  cpss_mem_memset(&,0,sizeof());
//  cpss_mem_memset(&,0,sizeof());
//  cpss_mem_memset(&,0,sizeof());
//  cpss_mem_memset(&,0,sizeof());
  ulApIdx = vstStaWLanInfo.ulApIndex;
  ulStaIndex = vstStaWLanInfo.ulStaIndex;
  ucRadioId = vstStaWLanInfo.ucRadioId;
  ucWlanId = vstStaWLanInfo.ucWlanId;

  //EHERNET Header
  //GetMacFromStr(gstStaConfInfo.aucStaSrvMacStr,stEthHdr.aucDstMac);
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stEthHdr.aucSrcMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }
  cpss_mem_memcpy(stEthHdr.aucDstMac,vstPingReplyStaInfo.aucDstMac,MT_MAC_LENTH);

  if(1 == gstStaConfInfo.ucVlanFlag)
    {
    stEthHdr.usProtclType = htons(0x8100);//VLAN
    }
  else
    {
	  stEthHdr.usProtclType = htons(0x0800);//IP
     }

  //VLAN Info
  MT_SetField16(stVlanHdr.usPriority,0,3,0);//Priority: Best Effort (default)0
  MT_SetField16(stVlanHdr.usPriority,3,1,0);//CFI:Canonical(0)
  MT_SetField16(stVlanHdr.usPriority,4,12,gastApconf[ulApIdx].ulStaVlan);//VLAN ID
  stVlanHdr.usType = htons(0x0800);

  //IPv4 Header
  stIpV4Hdr.ucIpVerType = 0x45;
  if(0 == gstStaConfInfo.ucStaQoSFlag)
     {
	  stIpV4Hdr.ucDiffSerFd = 0x00;//QoS:DSCP (0x00)
     }
  else
    {
	  stIpV4Hdr.ucDiffSerFd = 0x38;//QoS:DSCP (0x38)
    }
  stIpV4Hdr.usTotalLen = htons(sizeof(stIpV4Hdr) + sizeof(stIcmpV4));//IPHDR (20) + ICMP (40)
  stIpV4Hdr.usIdentfier = htons(0x28d6);
  stIpV4Hdr.usFragOffset = 0x0000;
  stIpV4Hdr.ucTtl = 64;
  stIpV4Hdr.ucProtoType = 1;//ICMP
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stIpV4Hdr.aucSrcIpaddr,gastStaPara[ulStaIndex].aucStaIpV4Addr,MT_IPV4_ADDR_LEN);
  }
  cpss_mem_memcpy(stIpV4Hdr.aucDstIpAddr,vstPingReplyStaInfo.aucDstIpv4Addr,MT_IPV4_ADDR_LEN);
#if 0
  GetIpFromStr(AF_INET,gstStaConfInfo.aucStaSrvIpv4AddrStr,stIpV4Hdr.aucDstIpAddr);
#endif
  stIpV4Hdr.usChkSum = MT_CalcIpChksm((UINT8*)&stIpV4Hdr,20);

  //ICMP Header and Data
  stIcmpV4.ucIpProtoType = 0x00;//Ping reply
  stIcmpV4.ucIcmpCode = 0x00;

  stIcmpV4.usBeIdentifier = vstPingReplyStaInfo.usIdentifier;
  stIcmpV4.usSeqNum = vstPingReplyStaInfo.usSeqNum;
  cpss_mem_memcpy(stIcmpV4.aucData,aucIcmpData,sizeof(aucIcmpData));

  stIcmpV4.usIcmpChkSum = MT_CalcIpChksm((UINT8*)&stIcmpV4,40);

  //capwap Header
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,1);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,0);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,1);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

  //Fragment ID
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
  //Fragment Offset
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
  //Reserved
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

  ucMacLen = MT_MAC_LENTH;

  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);
  ucRcvd = 0;

  ulSuffix = 0;
  cpss_mem_memcpy(&aucPingReply[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
  cpss_mem_memcpy(&aucPingReply[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
  cpss_mem_memcpy(&aucPingReply[ulSuffix],&ucMacLen,sizeof(ucMacLen));
  ulSuffix = ulSuffix + sizeof(ucMacLen);
  cpss_mem_memcpy(&aucPingReply[ulSuffix],aucBssId,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  cpss_mem_memcpy(&aucPingReply[ulSuffix],&ucRcvd,sizeof(ucRcvd));
  ulSuffix = ulSuffix + sizeof(ucRcvd);
  cpss_mem_memcpy(&aucPingReply[ulSuffix],&stEthHdr,sizeof(stEthHdr));
  ulSuffix = ulSuffix + sizeof(stEthHdr);
  if(1 == gstStaConfInfo.ucVlanFlag)
     {
    cpss_mem_memcpy(&aucPingReply[ulSuffix],&stVlanHdr,sizeof(stVlanHdr));
    ulSuffix = ulSuffix + sizeof(stVlanHdr);
     }
  cpss_mem_memcpy(&aucPingReply[ulSuffix],&stIpV4Hdr,sizeof(stIpV4Hdr));
  ulSuffix = ulSuffix + sizeof(stIpV4Hdr);
  cpss_mem_memcpy(&aucPingReply[ulSuffix],&stIcmpV4,sizeof(stIcmpV4));
  ulSuffix = ulSuffix + sizeof(stIcmpV4);

  gstPingReply.ulBufLen = ulSuffix;

  cpss_mem_memcpy(gstPingReply.aucBuff,aucPingReply,gstPingReply.ulBufLen);

  bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gstPingReply.aucBuff, gstPingReply.ulBufLen);

  return bSendResult;
}

//Ping6 Request(IPv6<-->IPv6)
MTBool ApMutiTestSendPing6Req(MT_STA_WLAN_INFO_T vstStaWLanInfo)
{
  MTBool bSendResult = MT_FALSE;
  UINT8 aucIcmpV6Data[] = {
  0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
  0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69
  };

  UINT8 aucPing6Req[256] = {0};
  UINT8 aucIcmpv6CkSumBuf[256] = {0};
  MT_ICMPV6_T stStaIcmpV6;
  MT_IPV6_HEADER_T stStaIpV6Hdr;
  MT_VLAN_HEADER_T stStaVlanHdr;
  MT_ETH_HDR_T stStaEthHdr;

  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  UINT8 ucMacLen = 0;
  UINT8 ucRcvd = 0;
  UINT8 aucBssId[MT_MAC_LENTH] = {0};
  UINT32 ulApIdx = 0;
  UINT32 ulStaIndex = 0;
  UINT8 ucRadioId = 0;
  UINT8 ucWlanId = 0;

  UINT32 ulSuffix = 0;
//  UINT32 ulLoop = 0;
  UINT8 ucZero = 0;
  UINT16 usRsvd = 0;
  UINT8 ucIcmpv6Pro = 0x3a;
  UINT16 usPayload = sizeof(stStaIcmpV6);

  cpss_mem_memset(&stStaIcmpV6,0,sizeof(stStaIcmpV6));
  cpss_mem_memset(&stStaIpV6Hdr,0,sizeof(stStaIpV6Hdr));
  cpss_mem_memset(&stStaVlanHdr,0,sizeof(stStaVlanHdr));
  cpss_mem_memset(&stStaEthHdr,0,sizeof(stStaEthHdr));

  ulApIdx = vstStaWLanInfo.ulApIndex;
  ulStaIndex = vstStaWLanInfo.ulStaIndex;
  ucRadioId = vstStaWLanInfo.ucRadioId;
  ucWlanId = vstStaWLanInfo.ucWlanId;

  //ETHERNET HEADER
  GetMacFromStr(gstStaConfInfo.aucStaSrvMacStr,stStaEthHdr.aucDstMac);
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stStaEthHdr.aucSrcMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }
  if(1 == gstStaConfInfo.ucVlanFlag)
    {
    stStaEthHdr.usProtclType = htons(0x8100);//VLAN
    }
  else
    {
    stStaEthHdr.usProtclType = htons(0x86dd);
     }

  //VLAN Info
  MT_SetField16(stStaVlanHdr.usPriority,0,3,0);//Priority: Best Effort (default)0
  MT_SetField16(stStaVlanHdr.usPriority,3,1,0);//CFI:Canonical(0)
  MT_SetField16(stStaVlanHdr.usPriority,4,12,gastApconf[ulApIdx].ulStaVlan);//VLAN ID
  stStaVlanHdr.usType = htons(0x86DD);

  //IPv6 Header
  MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,0,4,6);//IP Protocol Version

  if(0 == gstStaConfInfo.ucStaQoSFlag)
    {
    MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,4,6,0);//Traffic Class:DifferServField
    MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,10,1,0);//Traffic Class:ECN-Capable Transport
    MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,11,1,0);//Traffic Class:ECN-CE
    }
  else
    {
    MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,4,6,0x38);//Traffic Class:DifferServField
    MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,10,1,0);//Traffic Class:ECN-Capable Transport
    MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,11,1,0);//Traffic Class:ECN-CE
    }

  MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,12,20,0);//Flow Label
  stStaIpV6Hdr.ulIPVerTraffic = htonl(stStaIpV6Hdr.ulIPVerTraffic);
  stStaIpV6Hdr.usPayloadLen = sizeof(stStaIcmpV6);
  stStaIpV6Hdr.usPayloadLen = htons(stStaIpV6Hdr.usPayloadLen);
  stStaIpV6Hdr.ucNextHdr = 0x3a;//ICMPv6
  stStaIpV6Hdr.ucHopLmt = 64;
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stStaIpV6Hdr.aucSrcIpV6addr,gastStaPara[ulStaIndex].aucStaIpV6Addr,MT_IPV6_ADDR_LEN);
	  cpss_mem_memcpy(stStaIpV6Hdr.aucDstIpV6Addr,gastStaPara[ulStaIndex].aucStaGwIpv6Addr,MT_IPV6_ADDR_LEN);
  }
#if 0
  GetIpFromStr(AF_INET6,gstStaConfInfo.aucStaSrvIpv6AddrStr,stStaIpV6Hdr.aucDstIpV6Addr);
#endif

  //ICMPV6 Header and Data
  stStaIcmpV6.ucIpProtoType = 0x80;//ECHO (Ping) Request
  stStaIcmpV6.ucIcmpCode = 0;
  stStaIcmpV6.usIdentifier = htons(0x4041);
  stStaIcmpV6.usSeqNum = htons(MT_GetSequenceNum(gusSequenceNum));
  cpss_mem_memcpy(stStaIcmpV6.aucData,aucIcmpV6Data,sizeof(aucIcmpV6Data));

  //IPV6 CHECKSUM
  ulSuffix = 0;
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&stStaIcmpV6,sizeof(stStaIcmpV6));
  ulSuffix = ulSuffix + sizeof(stStaIcmpV6);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],stStaIpV6Hdr.aucSrcIpV6addr,MT_IPV6_ADDR_LEN);
  ulSuffix = ulSuffix + MT_IPV6_ADDR_LEN;
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],stStaIpV6Hdr.aucDstIpV6Addr,MT_IPV6_ADDR_LEN);
  ulSuffix = ulSuffix + MT_IPV6_ADDR_LEN;
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&usRsvd,sizeof(usRsvd));
  ulSuffix = ulSuffix + sizeof(usRsvd);
  usPayload = htons(usPayload);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&usPayload,sizeof(usPayload));
  ulSuffix = ulSuffix + sizeof(usPayload);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&usRsvd,sizeof(usRsvd));
  ulSuffix = ulSuffix + sizeof(usRsvd);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&ucZero,sizeof(ucZero));
  ulSuffix = ulSuffix + sizeof(ucZero);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&ucIcmpv6Pro,sizeof(ucIcmpv6Pro));
  ulSuffix = ulSuffix + sizeof(ucIcmpv6Pro);
  stStaIcmpV6.usIcmpChkSum = 0;
  stStaIcmpV6.usIcmpChkSum = MT_GetIcmpv6checksum(aucIcmpv6CkSumBuf,ulSuffix);

  //capwap Header
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,0);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,0);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,1);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

  //Fragment ID
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
  //Fragment Offset
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
  //Reserved
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

  ucMacLen = MT_MAC_LENTH;

  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);
  ucRcvd = 0;

  ulSuffix = 0;
  cpss_mem_memcpy(&aucPing6Req[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
  cpss_mem_memcpy(&aucPing6Req[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
  cpss_mem_memcpy(&aucPing6Req[ulSuffix],&ucMacLen,sizeof(ucMacLen));
  ulSuffix = ulSuffix + sizeof(ucMacLen);
  cpss_mem_memcpy(&aucPing6Req[ulSuffix],aucBssId,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  cpss_mem_memcpy(&aucPing6Req[ulSuffix],&ucRcvd,sizeof(ucRcvd));
  ulSuffix = ulSuffix + sizeof(ucRcvd);
  cpss_mem_memcpy(&aucPing6Req[ulSuffix],&stStaEthHdr,sizeof(stStaEthHdr));
  ulSuffix = ulSuffix + sizeof(stStaEthHdr);
  if(1 == gstStaConfInfo.ucVlanFlag)
     {
    cpss_mem_memcpy(&aucPing6Req[ulSuffix],&stStaVlanHdr,sizeof(stStaVlanHdr));
    ulSuffix = ulSuffix + sizeof(stStaVlanHdr);
     }
  cpss_mem_memcpy(&aucPing6Req[ulSuffix],&stStaIpV6Hdr,sizeof(stStaIpV6Hdr));
  ulSuffix = ulSuffix + sizeof(stStaIpV6Hdr);
  cpss_mem_memcpy(&aucPing6Req[ulSuffix],&stStaIcmpV6,sizeof(stStaIcmpV6));
  ulSuffix = ulSuffix + sizeof(stStaIcmpV6);

  gstPing6Req.ulBufLen = ulSuffix;

  cpss_mem_memcpy(gstPing6Req.aucBuff,aucPing6Req,gstPing6Req.ulBufLen);

  bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gstPing6Req.aucBuff, gstPing6Req.ulBufLen);
  return bSendResult;
}

//Ping6 Reply
MTBool ApMutiTestSendPing6Reply(MT_STA_WLAN_INFO_T vstStaWLanInfo, MT_PING6_REPLY_STA_INFO vstPing6ReplyStaInfo)
{
  MTBool bSendResult = MT_FALSE;
  UINT8 aucPing6Reply[256] = {0};
  UINT8 aucIcmpv6CkSumBuf[256] = {0};
  MT_ETH_HDR_T stEthHdr;
  MT_VLAN_HEADER_T stVlanHdr;
  MT_IPV6_HEADER_T stIpV6Hdr;
  MT_ICMPV6_T stIcmpv6;
  UINT8 aucStaSrvIpAddr[MT_IPV6_ADDR_LEN] = {0};
  UINT8 aucStaSrvMac[MT_MAC_LENTH] = {0};
  UINT8 aucIcmpV6Data[] = {
  0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, 0x70,
  0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69
  };
  UINT16 usRsvd = 0;
  UINT16 usPayload = sizeof(stIcmpv6);
  UINT8 ucZero = 0;
  UINT8 ucIcmpv6Pro = 0x3a;
  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  UINT8 ucMacLen = 0;
  UINT8 aucBssId[MT_MAC_LENTH] = {0};
  UINT32 ulApIdx = 0;
  UINT32 ulStaIndex = 0;
  UINT8 ucRadioId = 0;
  UINT8 ucWlanId = 0;
  UINT8 ucRcvd = 0;
  UINT32 ulSuffix = 0;

  cpss_mem_memset(&stEthHdr,0,sizeof(stEthHdr));
  cpss_mem_memset(&stVlanHdr,0,sizeof(stVlanHdr));
  cpss_mem_memset(&stIpV6Hdr,0,sizeof(stIpV6Hdr));
  cpss_mem_memset(&stIcmpv6,0,sizeof(stIcmpv6));
  GetIpFromStr(AF_INET6,gstStaConfInfo.aucStaSrvIpv6AddrStr,aucStaSrvIpAddr);
  GetMacFromStr(gstStaConfInfo.aucStaSrvMacStr,aucStaSrvMac);
  ulApIdx = vstStaWLanInfo.ulApIndex;
  ulStaIndex = vstStaWLanInfo.ulStaIndex;
  ucRadioId = vstStaWLanInfo.ucRadioId;
  ucWlanId = vstStaWLanInfo.ucWlanId;

  //Ethernet Header
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stEthHdr.aucSrcMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }
  cpss_mem_memcpy(stEthHdr.aucDstMac,vstPing6ReplyStaInfo.aucDstMac,MT_MAC_LENTH);
  //cpss_mem_memcpy(stEthHdr.aucDstMac,aucStaSrvMac,MT_MAC_LENTH);

  if(1 == gstStaConfInfo.ucVlanFlag)
     {
	  stEthHdr.usProtclType = htons(0x8100);//vlan
      }
  else
     {
	  stEthHdr.usProtclType = htons(0x86dd);//IPv6
     }

  //Vlan Header
  MT_SetField16(stVlanHdr.usPriority,0,3,0);//Priority: Best Effort (default)0
  MT_SetField16(stVlanHdr.usPriority,3,1,0);//CFI:Canonical(0)
  MT_SetField16(stVlanHdr.usPriority,4,12,gastApconf[ulApIdx].ulStaVlan);//VLAN ID
  stVlanHdr.usType = htons(0x86dd);

  //Ipv6 Header
  MT_SetField32(stIpV6Hdr.ulIPVerTraffic,0,4,6);//IP Protocol Version

  if(0 == gstStaConfInfo.ucStaQoSFlag)
    {
    MT_SetField32(stIpV6Hdr.ulIPVerTraffic,4,6,0);//Traffic Class:DifferServField
    MT_SetField32(stIpV6Hdr.ulIPVerTraffic,10,1,0);//Traffic Class:ECN-Capable Transport
    MT_SetField32(stIpV6Hdr.ulIPVerTraffic,11,1,0);//Traffic Class:ECN-CE
    }
  else
    {
    MT_SetField32(stIpV6Hdr.ulIPVerTraffic,4,6,0x38);//Traffic Class:DifferServField
    MT_SetField32(stIpV6Hdr.ulIPVerTraffic,10,1,0);//Traffic Class:ECN-Capable Transport
    MT_SetField32(stIpV6Hdr.ulIPVerTraffic,11,1,0);//Traffic Class:ECN-CE
    }

  MT_SetField32(stIpV6Hdr.ulIPVerTraffic,12,20,0);//Flow Label
  stIpV6Hdr.ulIPVerTraffic = htonl(stIpV6Hdr.ulIPVerTraffic);

  stIpV6Hdr.usPayloadLen = sizeof(stIcmpv6);
  stIpV6Hdr.usPayloadLen = htons(stIpV6Hdr.usPayloadLen);
  stIpV6Hdr.ucNextHdr = 0x3a;//ICMPv6
  stIpV6Hdr.ucHopLmt = 64;
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stIpV6Hdr.aucSrcIpV6addr,gastStaPara[ulStaIndex].aucStaIpV6Addr,MT_IPV6_ADDR_LEN);//SRC IP ADDRESS
  }
  cpss_mem_memcpy(stIpV6Hdr.aucDstIpV6Addr,vstPing6ReplyStaInfo.aucDstIpv6Addr,MT_IPV6_ADDR_LEN);

  //ICMPv6
  stIcmpv6.ucIpProtoType = 129;//Ping reply
  stIcmpv6.ucIcmpCode = 0;
  stIcmpv6.usIdentifier = vstPing6ReplyStaInfo.usIdentifier;
  stIcmpv6.usSeqNum = vstPing6ReplyStaInfo.usSeqNum;
  cpss_mem_memcpy(stIcmpv6.aucData,aucIcmpV6Data,sizeof(aucIcmpV6Data));
  //IPV6 CHECKSUM
  ulSuffix = 0;
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&stIcmpv6,sizeof(stIcmpv6));
  ulSuffix = ulSuffix + sizeof(stIcmpv6);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],stIpV6Hdr.aucSrcIpV6addr,MT_IPV6_ADDR_LEN);
  ulSuffix = ulSuffix + MT_IPV6_ADDR_LEN;
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],stIpV6Hdr.aucDstIpV6Addr,MT_IPV6_ADDR_LEN);
  ulSuffix = ulSuffix + MT_IPV6_ADDR_LEN;
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&usRsvd,sizeof(usRsvd));
  ulSuffix = ulSuffix + sizeof(usRsvd);
  usPayload = htons(usPayload);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&usPayload,sizeof(usPayload));
  ulSuffix = ulSuffix + sizeof(usPayload);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&usRsvd,sizeof(usRsvd));
  ulSuffix = ulSuffix + sizeof(usRsvd);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&ucZero,sizeof(ucZero));
  ulSuffix = ulSuffix + sizeof(ucZero);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&ucIcmpv6Pro,sizeof(ucIcmpv6Pro));
  ulSuffix = ulSuffix + sizeof(ucIcmpv6Pro);
  stIcmpv6.usIcmpChkSum = 0;
  stIcmpv6.usIcmpChkSum = MT_GetIcmpv6checksum(aucIcmpv6CkSumBuf,ulSuffix);

  //capwap Header
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,0);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,0);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,1);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

  //Fragment ID
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
  //Fragment Offset
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
  //Reserved
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

  ucMacLen = MT_MAC_LENTH;

  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);
  ucRcvd = 0;

  ulSuffix = 0;
  cpss_mem_memcpy(&aucPing6Reply[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
  cpss_mem_memcpy(&aucPing6Reply[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
  cpss_mem_memcpy(&aucPing6Reply[ulSuffix],&ucMacLen,sizeof(ucMacLen));
  ulSuffix = ulSuffix + sizeof(ucMacLen);
  cpss_mem_memcpy(&aucPing6Reply[ulSuffix],aucBssId,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  cpss_mem_memcpy(&aucPing6Reply[ulSuffix],&ucRcvd,sizeof(ucRcvd));
  ulSuffix = ulSuffix + sizeof(ucRcvd);

  cpss_mem_memcpy(&aucPing6Reply[ulSuffix],&stEthHdr,sizeof(stEthHdr));
  ulSuffix = ulSuffix + sizeof(stEthHdr);
  if(1 == gstStaConfInfo.ucVlanFlag)
     {
    cpss_mem_memcpy(&aucPing6Reply[ulSuffix],&stVlanHdr,sizeof(stVlanHdr));
    ulSuffix = ulSuffix + sizeof(stVlanHdr);
     }
  cpss_mem_memcpy(&aucPing6Reply[ulSuffix],&stIpV6Hdr,sizeof(stIpV6Hdr));
  ulSuffix = ulSuffix + sizeof(stIpV6Hdr);
  cpss_mem_memcpy(&aucPing6Reply[ulSuffix],&stIcmpv6,sizeof(stIcmpv6));
  ulSuffix = ulSuffix + sizeof(stIcmpv6);

  gstPing6Reply.ulBufLen = ulSuffix;

  cpss_mem_memcpy(gstPing6Reply.aucBuff,aucPing6Reply,gstPing6Reply.ulBufLen);

  bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gstPing6Reply.aucBuff, gstPing6Reply.ulBufLen);

  return bSendResult;
}

//ARP Request
MTBool ApMutiTestSendArpReq(MT_STA_WLAN_INFO_T vstStaWLanInfo)
{
	  MTBool bSendResult = MT_FALSE;
	  UINT8 aucArpReq[256] = {0};
	  MT_ETH_HDR_T stEthHdr;
	  MT_VLAN_HEADER_T stVlanHdr;
	  MT_ARP_REQ_T stArpReq;
	  UINT32 ulCapwapHdrFisrt32 = 0;
	  UINT32 ulCapwapHdrSecond32 = 0;
	  UINT8 aucBssId[MT_MAC_LENTH] = {0};
	  UINT32 ulApIdx = 0;
	  UINT32 ulStaIndex = 0;
	  UINT8 ucRadioId = 0;
	  UINT8 ucWlanId = 0;

	  UINT8 ucMacLen = 0;
	  UINT8 ucRcvd = 0;
	  UINT32 ulSuffix = 0;
	  UINT32 ulLoop = 0;

	  cpss_mem_memset(&stEthHdr,0,sizeof(stEthHdr));
	  cpss_mem_memset(&stVlanHdr,0,sizeof(stVlanHdr));
	  cpss_mem_memset(&stArpReq,0,sizeof(stArpReq));

	  ulApIdx = vstStaWLanInfo.ulApIndex;
	  ulStaIndex = vstStaWLanInfo.ulStaIndex;
	  ucRadioId = vstStaWLanInfo.ucRadioId;
	  ucWlanId = vstStaWLanInfo.ucWlanId;

	  //ETH Header
	  if(ulApIdx == getApIDbyBssid(ulStaIndex))
	  {
		  cpss_mem_memcpy(stEthHdr.aucSrcMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
	  }
	  for(ulLoop = 0;ulLoop < MT_MAC_LENTH; ulLoop++)
	     {
		  stEthHdr.aucDstMac[ulLoop] = 0xff;
	     }

	  if(1 == gstStaConfInfo.ucVlanFlag)
	    {
	    stEthHdr.usProtclType = htons(0x8100);//VLAN
	    }
	  else
	    {
		  stEthHdr.usProtclType = htons(0x0806);//ARP
	     }

	  //STA Valn info
	  MT_SetField16(stVlanHdr.usPriority,0,3,0);//Priority: Best Effort (default)0
	  MT_SetField16(stVlanHdr.usPriority,3,1,0);//CFI:Canonical(0)
	  MT_SetField16(stVlanHdr.usPriority,4,12,gastApconf[ulApIdx].ulStaVlan);//VLAN ID
	  stVlanHdr.usType = htons(0x0806);//ARP

	  //ARP Request
	  stArpReq.usHWType = 0x0001;//Ethernet
	  stArpReq.usProtclType = 0x0800;//IP
	  stArpReq.ucHWSize = 0x06;
	  stArpReq.ucProtclSize = 0x04;
	  stArpReq.usOpcode = 0x0001;
	  if(ulApIdx == getApIDbyBssid(ulStaIndex))
	  {
		  cpss_mem_memcpy(stArpReq.aucSrcMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);//Sender MAC
		  cpss_mem_memcpy(stArpReq.aucSrcIpv4Addr,gastStaPara[ulStaIndex].aucStaIpV4Addr,MT_IPV4_ADDR_LEN);//Sender IP
	  }
	  stArpReq.usHWType = htons(stArpReq.usHWType);
	  stArpReq.usProtclType = htons(stArpReq.usProtclType);
	  stArpReq.usOpcode = htons(stArpReq.usOpcode);

	  for(ulLoop = 0;ulLoop < MT_MAC_LENTH; ulLoop++)
		  {
		  stArpReq.aucDstMac[ulLoop] = 0x00;//Target MAC
		  }
	  if(ulApIdx == getApIDbyBssid(ulStaIndex))
	  {
		  cpss_mem_memcpy(stArpReq.aucDstIpv4Addr,gastStaPara[ulStaIndex].aucStaGwIpv4Addr,MT_IPV4_ADDR_LEN);//Target IP
	  }

	  //Capwap Header
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,0);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,0);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,1);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。
	  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

	  //Fragment ID
	  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
	  //Fragment Offset
	  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
	  //Reserved
	  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。
	  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

	  ucMacLen = MT_MAC_LENTH;
	  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);
	  ucRcvd = 0;

	  //Construct Arp Req Msg
	  //Capwap Header
	  ulSuffix = 0;
	  cpss_mem_memcpy(&aucArpReq[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
	  ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
	  cpss_mem_memcpy(&aucArpReq[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
	  ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
	  cpss_mem_memcpy(&aucArpReq[ulSuffix],&ucMacLen,sizeof(ucMacLen));
	  ulSuffix = ulSuffix + sizeof(ucMacLen);
	  cpss_mem_memcpy(&aucArpReq[ulSuffix],aucBssId,MT_MAC_LENTH);
	  ulSuffix = ulSuffix + MT_MAC_LENTH;
	  cpss_mem_memcpy(&aucArpReq[ulSuffix],&ucRcvd,sizeof(ucRcvd));
	  ulSuffix = ulSuffix + sizeof(ucRcvd);

	  //ETH Header
	  cpss_mem_memcpy(&aucArpReq[ulSuffix],&stEthHdr,sizeof(stEthHdr));
	  ulSuffix = ulSuffix + sizeof(stEthHdr);

	  //STA Vlan
	  if(1 == gstStaConfInfo.ucVlanFlag)
	     {
	    cpss_mem_memcpy(&aucArpReq[ulSuffix],&stVlanHdr,sizeof(stVlanHdr));
	    ulSuffix = ulSuffix + sizeof(stVlanHdr);
	     }

	  //ARP Req
	  cpss_mem_memcpy(&aucArpReq[ulSuffix],&stArpReq,sizeof(stArpReq));
	  ulSuffix = ulSuffix + sizeof(stArpReq);

	  gstArpReq.ulBufLen = ulSuffix;

	  cpss_mem_memcpy(gstArpReq.aucBuff,aucArpReq,gstArpReq.ulBufLen);

	  bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gstArpReq.aucBuff, gstArpReq.ulBufLen);

	  if(MT_FALSE == bSendResult)
	     {
	    printf("Error.MT_ApSendMsg2UPM for ArpReq is failure.\nFile: %s,Line: %d\n",__FILE__,__LINE__);
	     }

	  return bSendResult;
}

//ARP Reply
MTBool ApMutiTestSendArpReply(MT_STA_WLAN_INFO_T vstStaWLanInfo)
{
  MTBool bSendResult = MT_FALSE;
  UINT8 aucArpReply[256] = {0};
  MT_ETH_HDR_T stEthHdr;
  MT_VLAN_HEADER_T stStaVlanHdr;
  MT_ARP_REQ_T stArpReply;
  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  UINT8 aucBssId[MT_MAC_LENTH] = {0};
  UINT32 ulApIdx = 0;
  UINT32 ulStaIndex = 0;
  UINT8 ucRadioId = 0;
  UINT8 ucWlanId = 0;

  UINT8 ucMacLen = 0;
  UINT8 ucRcvd = 0;
  UINT32 ulSuffix = 0;

  cpss_mem_memset(&stEthHdr,0,sizeof(stEthHdr));
  cpss_mem_memset(&stStaVlanHdr,0,sizeof(stStaVlanHdr));
  cpss_mem_memset(&stArpReply,0,sizeof(stArpReply));

  ulApIdx = vstStaWLanInfo.ulApIndex;
  ulStaIndex = vstStaWLanInfo.ulStaIndex;
  ucRadioId = vstStaWLanInfo.ucRadioId;
  ucWlanId = vstStaWLanInfo.ucWlanId;

  //ETH Header
  GetMacFromStr(gstStaConfInfo.aucStaSrvMacStr,stEthHdr.aucDstMac);
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stEthHdr.aucSrcMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }
  if(1 == gstStaConfInfo.ucVlanFlag)
    {
    stEthHdr.usProtclType = htons(0x8100);//VLAN
    }
  else
    {
	  stEthHdr.usProtclType = htons(0x0806);//ARP
     }

  //STA Valn info
  MT_SetField16(stStaVlanHdr.usPriority,0,3,0);//Priority: Best Effort (default)0
  MT_SetField16(stStaVlanHdr.usPriority,3,1,0);//CFI:Canonical(0)
  MT_SetField16(stStaVlanHdr.usPriority,4,12,gastApconf[ulApIdx].ulStaVlan);//VLAN ID
  stStaVlanHdr.usType = htons(0x0806);//ARP

  //Arp reply
  stArpReply.usHWType = htons(0x0001);
  stArpReply.usProtclType = htons(0x0800);
  stArpReply.ucHWSize = 6;
  stArpReply.ucProtclSize = 4;
  stArpReply.usOpcode = htons(0x0002);
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stArpReply.aucSrcIpv4Addr,gastStaPara[ulStaIndex].aucStaIpV4Addr,MT_IPV4_ADDR_LEN);
	  cpss_mem_memcpy(stArpReply.aucSrcMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }
  GetIpFromStr(AF_INET,gstStaConfInfo.aucStaSrvIpv4AddrStr,stArpReply.aucDstIpv4Addr);
  GetMacFromStr(gstStaConfInfo.aucStaSrvMacStr,stArpReply.aucDstMac);

  //Capwap Header
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,0);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,0);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,1);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

  //Fragment ID
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
  //Fragment Offset
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
  //Reserved
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

  ucMacLen = MT_MAC_LENTH;

  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);
  ucRcvd = 0;

  //Construct Arp Reply Msg
  //Capwap Header
  ulSuffix = 0;
  cpss_mem_memcpy(&aucArpReply[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
  cpss_mem_memcpy(&aucArpReply[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
  cpss_mem_memcpy(&aucArpReply[ulSuffix],&ucMacLen,sizeof(ucMacLen));
  ulSuffix = ulSuffix + sizeof(ucMacLen);
  cpss_mem_memcpy(&aucArpReply[ulSuffix],aucBssId,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  cpss_mem_memcpy(&aucArpReply[ulSuffix],&ucRcvd,sizeof(ucRcvd));
  ulSuffix = ulSuffix + sizeof(ucRcvd);

  //ETH Header
  cpss_mem_memcpy(&aucArpReply[ulSuffix],&stEthHdr,sizeof(stEthHdr));
  ulSuffix = ulSuffix + sizeof(stEthHdr);

  //STA Vlan
  if(1 == gstStaConfInfo.ucVlanFlag)
     {
    cpss_mem_memcpy(&aucArpReply[ulSuffix],&stStaVlanHdr,sizeof(stStaVlanHdr));
    ulSuffix = ulSuffix + sizeof(stStaVlanHdr);
     }

  //ARP Reply
  cpss_mem_memcpy(&aucArpReply[ulSuffix],&stArpReply,sizeof(stArpReply));
  ulSuffix = ulSuffix + sizeof(stArpReply);

  gstArpReply.ulBufLen = ulSuffix;

  cpss_mem_memcpy(gstArpReply.aucBuff,aucArpReply,gstArpReply.ulBufLen);

  bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gstArpReply.aucBuff, gstArpReply.ulBufLen);

  if(MT_FALSE == bSendResult)
     {
    printf("Error.MT_ApSendMsg2UPM for ArpReply is failure.\nFile: %s,Line: %d\n",__FILE__,__LINE__);
     }
  //gstArpReply
  return bSendResult;
}

//IPv6 NeighborSolicitation
MTBool ApMutiTestSendNeighbrSolicit(MT_STA_WLAN_INFO_T vstStaWLanInfo)
{
	  MTBool bSendResult = MT_FALSE;
	  UINT8 aucNeighborSolicit[256] = {0};
	  UINT8 aucIcmpv6CkSumBuf[256] = {0};
	  MT_ETH_HDR_T stEthHdr;
	  MT_VLAN_HEADER_T stVlanHdr;
	  MT_IPV6_HEADER_T stIpV6Hdr;
	  MT_ICMPV6_NEIGHBOR_ADVERTISEMENT_T stNbSolicitICMPV6;

	  UINT32 ulCapwapHdrFisrt32 = 0;
	  UINT32 ulCapwapHdrSecond32 = 0;
	  UINT8 aucBssId[MT_MAC_LENTH] = {0};
	  UINT32 ulApIdx = 0;
	  UINT32 ulStaIndex = 0;
	  UINT8 ucRadioId = 0;
	  UINT8 ucWlanId = 0;
	  UINT8 ucMacLen = 0;
	  UINT8 ucRcvd = 0;
	  UINT32 ulSuffix = 0;
	  UINT16 usRsvd = 0;
	  UINT8 ucRsvd = 0;
	  UINT16 usPayload = sizeof(stNbSolicitICMPV6);
	  UINT8 ucIcmpv6Pro = 0x3a;
	  UINT8 aucStaSrvIpAddr[MT_IPV6_ADDR_LEN] = {0};
	  UINT8 aucStaSrvMac[MT_MAC_LENTH] = {0};

	  cpss_mem_memset(&stEthHdr,0,sizeof(stEthHdr));
	  cpss_mem_memset(&stVlanHdr,0,sizeof(stVlanHdr));
	  cpss_mem_memset(&stIpV6Hdr,0,sizeof(stIpV6Hdr));
	  cpss_mem_memset(&stNbSolicitICMPV6,0,sizeof(stNbSolicitICMPV6));

	  GetIpFromStr(AF_INET6,gstStaConfInfo.aucStaSrvIpv6AddrStr,aucStaSrvIpAddr);
	  GetMacFromStr(gstStaConfInfo.aucStaSrvMacStr,aucStaSrvMac);

	  ulApIdx = vstStaWLanInfo.ulApIndex;
	  ulStaIndex = vstStaWLanInfo.ulStaIndex;
	  ucRadioId = vstStaWLanInfo.ucRadioId;
	  ucWlanId = vstStaWLanInfo.ucWlanId;

	  //ETH Header
#if 0
	  GetMacFromStr(gstStaConfInfo.aucStaSrvMacStr,stEthHdr.aucDstMac);//Dest Mac
	  cpss_mem_memcpy(stEthHdr.aucSrcMac,gastAp[ulApIdx].astSta[ulStaIndex].auStaMac,MT_MAC_LENTH);//SRC MAC
#endif
	  if(ulApIdx == getApIDbyBssid(ulStaIndex))
	  {
		  cpss_mem_memcpy(stEthHdr.aucSrcMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
		  cpss_mem_memcpy(stEthHdr.aucDstMac,gastStaPara[ulStaIndex].auGwMac,MT_MAC_LENTH);
	  }

	  if(1 == gstStaConfInfo.ucVlanFlag)
	    {
	    stEthHdr.usProtclType = htons(0x8100);//VLAN
	    }
	  else
	    {
	    stEthHdr.usProtclType = htons(0x86dd);
	     }

	  //VLAN STRUCT
	  MT_SetField16(stVlanHdr.usPriority,0,3,0);//Priority: Best Effort (default)0
	  MT_SetField16(stVlanHdr.usPriority,3,1,0);//CFI:Canonical(0)
	  MT_SetField16(stVlanHdr.usPriority,4,12,gastApconf[ulApIdx].ulStaVlan);//VLAN ID
	  stVlanHdr.usType = htons(0x86dd);

	  //IPv6 Header
	  MT_SetField32(stIpV6Hdr.ulIPVerTraffic,0,4,6);//IP Protocol Version

	  if(0 == gstStaConfInfo.ucStaQoSFlag)
	    {
	    MT_SetField32(stIpV6Hdr.ulIPVerTraffic,4,6,0);//Traffic Class:DifferServField
	    MT_SetField32(stIpV6Hdr.ulIPVerTraffic,10,1,0);//Traffic Class:ECN-Capable Transport
	    MT_SetField32(stIpV6Hdr.ulIPVerTraffic,11,1,0);//Traffic Class:ECN-CE
	    }
	  else
	    {
	    MT_SetField32(stIpV6Hdr.ulIPVerTraffic,4,6,0x38);//Traffic Class:DifferServField
	    MT_SetField32(stIpV6Hdr.ulIPVerTraffic,10,1,0);//Traffic Class:ECN-Capable Transport
	    MT_SetField32(stIpV6Hdr.ulIPVerTraffic,11,1,0);//Traffic Class:ECN-CE
	    }

	  MT_SetField32(stIpV6Hdr.ulIPVerTraffic,12,20,0);//Flow Label
	  stIpV6Hdr.ulIPVerTraffic = htonl(stIpV6Hdr.ulIPVerTraffic);

	  stIpV6Hdr.usPayloadLen = sizeof(stNbSolicitICMPV6);
	  stIpV6Hdr.usPayloadLen = htons(stIpV6Hdr.usPayloadLen);
	  stIpV6Hdr.ucNextHdr = 0x3a;//ICMPv6
	  stIpV6Hdr.ucHopLmt = 255;
	  if(ulApIdx == getApIDbyBssid(ulStaIndex))
	  {
		  cpss_mem_memcpy(stIpV6Hdr.aucSrcIpV6addr,gastStaPara[ulStaIndex].aucStaIpV6Addr,MT_IPV6_ADDR_LEN);//SRC IP ADDRESS
	  }
	  cpss_mem_memcpy(stIpV6Hdr.aucDstIpV6Addr,aucStaSrvIpAddr,MT_IPV6_ADDR_LEN);//DST IP ADDRESS

	  //ICMPV6(Neighbor Solicit)
	  stNbSolicitICMPV6.ucProtclType = 135;
	  stNbSolicitICMPV6.ucCode = 0;

	  MT_SetField32(stNbSolicitICMPV6.ulFlag,0,1,0);//Router
	  MT_SetField32(stNbSolicitICMPV6.ulFlag,1,1,1);//Solicited
	  MT_SetField32(stNbSolicitICMPV6.ulFlag,2,1,1);//Override
	  MT_SetField32(stNbSolicitICMPV6.ulFlag,3,29,0);//Resvd
	  stNbSolicitICMPV6.ulFlag = htonl(stNbSolicitICMPV6.ulFlag);
	  if(ulApIdx == getApIDbyBssid(ulStaIndex))
	  {
		  cpss_mem_memcpy(stNbSolicitICMPV6.aucDstIpv6Addr,gastStaPara[ulStaIndex].aucStaIpV6Addr,MT_IPV6_ADDR_LEN);//Target IPv6 Address
	  }
	  stNbSolicitICMPV6.stIcmpv6Opt.ucType = 1;//Target Link-Lay Address
	  stNbSolicitICMPV6.stIcmpv6Opt.ucLen = 1;
	  if(ulApIdx == getApIDbyBssid(ulStaIndex))
	  {
		  cpss_mem_memcpy(stNbSolicitICMPV6.stIcmpv6Opt.aucLnkLayerAddr,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);//Target Link Address
	  }

	  //IPV6 CHECKSUM
	  ulSuffix = 0;
	  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&stNbSolicitICMPV6,sizeof(stNbSolicitICMPV6));
	  ulSuffix = ulSuffix + sizeof(stNbSolicitICMPV6);
	  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],stIpV6Hdr.aucSrcIpV6addr,MT_IPV6_ADDR_LEN);
	  ulSuffix = ulSuffix + MT_IPV6_ADDR_LEN;
	  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],stIpV6Hdr.aucDstIpV6Addr,MT_IPV6_ADDR_LEN);
	  ulSuffix = ulSuffix + MT_IPV6_ADDR_LEN;
	  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&usRsvd,sizeof(usRsvd));
	  ulSuffix = ulSuffix + sizeof(usRsvd);
	  usPayload = htons(usPayload);
	  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&usPayload,sizeof(usPayload));
	  ulSuffix = ulSuffix + sizeof(usPayload);
	  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&usRsvd,sizeof(usRsvd));
	  ulSuffix = ulSuffix + sizeof(usRsvd);
	  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&ucRsvd,sizeof(ucRsvd));
	  ulSuffix = ulSuffix + sizeof(ucRsvd);
	  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&ucIcmpv6Pro,sizeof(ucIcmpv6Pro));
	  ulSuffix = ulSuffix + sizeof(ucIcmpv6Pro);

	  stNbSolicitICMPV6.usChkSum = 0;
	  stNbSolicitICMPV6.usChkSum = MT_GetIcmpv6checksum(aucIcmpv6CkSumBuf,ulSuffix);

	  //CAPWAP HEADER
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,0);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,0);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,1);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

	  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

	  //Fragment ID
	  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
	  //Fragment Offset
	  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
	  //Reserved
	  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

	  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

	  ucMacLen = MT_MAC_LENTH;

	  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);
	  ucRcvd = 0;

	  ulSuffix = 0;
	  cpss_mem_memcpy(&aucNeighborSolicit[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
	  ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
	  cpss_mem_memcpy(&aucNeighborSolicit[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
	  ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
	  cpss_mem_memcpy(&aucNeighborSolicit[ulSuffix],&ucMacLen,sizeof(ucMacLen));
	  ulSuffix = ulSuffix + sizeof(ucMacLen);
	  cpss_mem_memcpy(&aucNeighborSolicit[ulSuffix],aucBssId,MT_MAC_LENTH);
	  ulSuffix = ulSuffix + MT_MAC_LENTH;
	  cpss_mem_memcpy(&aucNeighborSolicit[ulSuffix],&ucRcvd,sizeof(ucRcvd));
	  ulSuffix = ulSuffix + sizeof(ucRcvd);

	  cpss_mem_memcpy(&aucNeighborSolicit[ulSuffix],&stEthHdr,sizeof(stEthHdr));
	  ulSuffix = ulSuffix + sizeof(stEthHdr);
	  if(1 == gstStaConfInfo.ucVlanFlag)
	     {
	    cpss_mem_memcpy(&aucNeighborSolicit[ulSuffix],&stVlanHdr,sizeof(stVlanHdr));
	    ulSuffix = ulSuffix + sizeof(stVlanHdr);
	     }
	  cpss_mem_memcpy(&aucNeighborSolicit[ulSuffix],&stIpV6Hdr,sizeof(stIpV6Hdr));
	  ulSuffix = ulSuffix + sizeof(stIpV6Hdr);
	  cpss_mem_memcpy(&aucNeighborSolicit[ulSuffix],&stNbSolicitICMPV6,sizeof(stNbSolicitICMPV6));
	  ulSuffix = ulSuffix + sizeof(stNbSolicitICMPV6);

	  gstNeighborSolicit.ulBufLen = ulSuffix;

	  cpss_mem_memcpy(gstNeighborSolicit.aucBuff,aucNeighborSolicit,gstNeighborSolicit.ulBufLen);

	  bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gstNeighborSolicit.aucBuff, gstNeighborSolicit.ulBufLen);

	  if(MT_FALSE == bSendResult)
	     {
	    printf("Error.MT_ApSendMsg2UPM for NeighborSolicit is failure.\nFile: %s,Line: %d\n",__FILE__,__LINE__);
	     }

	  return bSendResult;
}

//IPv6 NeighborAdvertisement
MTBool ApMutiTestSendNeighbrAdvertsmnt(MT_STA_WLAN_INFO_T vstStaWLanInfo)
{
  MTBool bSendResult = MT_FALSE;
  UINT8 aucNeighborAdvertsmnt[256] = {0};
  UINT8 aucIcmpv6CkSumBuf[256] = {0};
  MT_ETH_HDR_T stEthHdr;
  MT_VLAN_HEADER_T stStaVlanHdr;
  MT_IPV6_HEADER_T stStaIpV6Hdr;
  MT_ICMPV6_NEIGHBOR_ADVERTISEMENT_T stNbAdmntICMPV6;

  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  UINT8 aucBssId[MT_MAC_LENTH] = {0};
  UINT32 ulApIdx = 0;
  UINT32 ulStaIndex = 0;
  UINT8 ucRadioId = 0;
  UINT8 ucWlanId = 0;
  UINT8 ucMacLen = 0;
  UINT8 ucRcvd = 0;
  UINT32 ulSuffix = 0;
  UINT16 usRsvd = 0;
  UINT8 ucRsvd = 0;
  UINT16 usPayload = sizeof(stNbAdmntICMPV6);
  UINT8 ucIcmpv6Pro = 0x3a;
  UINT8 aucStaSrvIpAddr[MT_IPV6_ADDR_LEN] = {0};
  UINT8 aucStaSrvMac[MT_MAC_LENTH] = {0};

  cpss_mem_memset(&stEthHdr,0,sizeof(stEthHdr));
  cpss_mem_memset(&stStaVlanHdr,0,sizeof(stStaVlanHdr));
  cpss_mem_memset(&stStaIpV6Hdr,0,sizeof(stStaIpV6Hdr));
  cpss_mem_memset(&stNbAdmntICMPV6,0,sizeof(stNbAdmntICMPV6));
  GetIpFromStr(AF_INET6,gstStaConfInfo.aucStaSrvIpv6AddrStr,aucStaSrvIpAddr);
  GetMacFromStr(gstStaConfInfo.aucStaSrvMacStr,aucStaSrvMac);

  ulApIdx = vstStaWLanInfo.ulApIndex;
  ulStaIndex = vstStaWLanInfo.ulStaIndex;
  ucRadioId = vstStaWLanInfo.ucRadioId;
  ucWlanId = vstStaWLanInfo.ucWlanId;

  //ETH Header
  GetMacFromStr(gstStaConfInfo.aucStaSrvMacStr,stEthHdr.aucDstMac);//Dest Mac
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stEthHdr.aucSrcMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);//SRC MAC
  }

  if(1 == gstStaConfInfo.ucVlanFlag)
    {
    stEthHdr.usProtclType = htons(0x8100);//VLAN
    }
  else
    {
    stEthHdr.usProtclType = htons(0x86dd);
     }

  //VLAN STRUCT
  MT_SetField16(stStaVlanHdr.usPriority,0,3,0);//Priority: Best Effort (default)0
  MT_SetField16(stStaVlanHdr.usPriority,3,1,0);//CFI:Canonical(0)
  MT_SetField16(stStaVlanHdr.usPriority,4,12,gastApconf[ulApIdx].ulStaVlan);//VLAN ID
  stStaVlanHdr.usType = htons(0x86dd);

  //IPv6 Header
  MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,0,4,6);//IP Protocol Version

  if(0 == gstStaConfInfo.ucStaQoSFlag)
    {
    MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,4,6,0);//Traffic Class:DifferServField
    MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,10,1,0);//Traffic Class:ECN-Capable Transport
    MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,11,1,0);//Traffic Class:ECN-CE
    }
  else
    {
    MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,4,6,0x38);//Traffic Class:DifferServField
    MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,10,1,0);//Traffic Class:ECN-Capable Transport
    MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,11,1,0);//Traffic Class:ECN-CE
    }

  MT_SetField32(stStaIpV6Hdr.ulIPVerTraffic,12,20,0);//Flow Label
  stStaIpV6Hdr.ulIPVerTraffic = htonl(stStaIpV6Hdr.ulIPVerTraffic);

  stStaIpV6Hdr.usPayloadLen = sizeof(stNbAdmntICMPV6);
  stStaIpV6Hdr.usPayloadLen = htons(stStaIpV6Hdr.usPayloadLen);
  stStaIpV6Hdr.ucNextHdr = 0x3a;//ICMPv6
  stStaIpV6Hdr.ucHopLmt = 255;
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stStaIpV6Hdr.aucSrcIpV6addr,gastStaPara[ulStaIndex].aucStaIpV6Addr,MT_IPV6_ADDR_LEN);//SRC IP ADDRESS
  }
  cpss_mem_memcpy(stStaIpV6Hdr.aucDstIpV6Addr,aucStaSrvIpAddr,MT_IPV6_ADDR_LEN);//DST IP ADDRESS

  //ICMPV6(Neighbor Advertisement)
  stNbAdmntICMPV6.ucProtclType = 136;
  stNbAdmntICMPV6.ucCode = 0;

  MT_SetField32(stNbAdmntICMPV6.ulFlag,0,1,0);//Router
  MT_SetField32(stNbAdmntICMPV6.ulFlag,1,1,1);//Solicited
  MT_SetField32(stNbAdmntICMPV6.ulFlag,2,1,1);//Override
  MT_SetField32(stNbAdmntICMPV6.ulFlag,3,29,0);//Resvd

  stNbAdmntICMPV6.ulFlag = htonl(stNbAdmntICMPV6.ulFlag);
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stNbAdmntICMPV6.aucDstIpv6Addr,gastStaPara[ulStaIndex].aucStaIpV6Addr,MT_IPV6_ADDR_LEN);//Target IPv6 Address
  }
  stNbAdmntICMPV6.stIcmpv6Opt.ucType = 2;//Target Link-Lay Address
  stNbAdmntICMPV6.stIcmpv6Opt.ucLen = 1;
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stNbAdmntICMPV6.stIcmpv6Opt.aucLnkLayerAddr,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);//Target Link Address
  }

  //IPV6 CHECKSUM
  ulSuffix = 0;
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&stNbAdmntICMPV6,sizeof(stNbAdmntICMPV6));
  ulSuffix = ulSuffix + sizeof(stNbAdmntICMPV6);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],stStaIpV6Hdr.aucSrcIpV6addr,MT_IPV6_ADDR_LEN);
  ulSuffix = ulSuffix + MT_IPV6_ADDR_LEN;
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],stStaIpV6Hdr.aucDstIpV6Addr,MT_IPV6_ADDR_LEN);
  ulSuffix = ulSuffix + MT_IPV6_ADDR_LEN;
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&usRsvd,sizeof(usRsvd));
  ulSuffix = ulSuffix + sizeof(usRsvd);
  usPayload = htons(usPayload);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&usPayload,sizeof(usPayload));
  ulSuffix = ulSuffix + sizeof(usPayload);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&usRsvd,sizeof(usRsvd));
  ulSuffix = ulSuffix + sizeof(usRsvd);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&ucRsvd,sizeof(ucRsvd));
  ulSuffix = ulSuffix + sizeof(ucRsvd);
  cpss_mem_memcpy(&aucIcmpv6CkSumBuf[ulSuffix],&ucIcmpv6Pro,sizeof(ucIcmpv6Pro));
  ulSuffix = ulSuffix + sizeof(ucIcmpv6Pro);

  stNbAdmntICMPV6.usChkSum = 0;
  stNbAdmntICMPV6.usChkSum = MT_GetIcmpv6checksum(aucIcmpv6CkSumBuf,ulSuffix);

  //CAPWAP HEADER
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,0);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,0);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,1);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

  //Fragment ID
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
  //Fragment Offset
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
  //Reserved
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

  ucMacLen = MT_MAC_LENTH;

  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);
  ucRcvd = 0;

  ulSuffix = 0;
  cpss_mem_memcpy(&aucNeighborAdvertsmnt[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
  cpss_mem_memcpy(&aucNeighborAdvertsmnt[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
  cpss_mem_memcpy(&aucNeighborAdvertsmnt[ulSuffix],&ucMacLen,sizeof(ucMacLen));
  ulSuffix = ulSuffix + sizeof(ucMacLen);
  cpss_mem_memcpy(&aucNeighborAdvertsmnt[ulSuffix],aucBssId,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  cpss_mem_memcpy(&aucNeighborAdvertsmnt[ulSuffix],&ucRcvd,sizeof(ucRcvd));
  ulSuffix = ulSuffix + sizeof(ucRcvd);

  cpss_mem_memcpy(&aucNeighborAdvertsmnt[ulSuffix],&stEthHdr,sizeof(stEthHdr));
  ulSuffix = ulSuffix + sizeof(stEthHdr);
  if(1 == gstStaConfInfo.ucVlanFlag)
     {
    cpss_mem_memcpy(&aucNeighborAdvertsmnt[ulSuffix],&stStaVlanHdr,sizeof(stStaVlanHdr));
    ulSuffix = ulSuffix + sizeof(stStaVlanHdr);
     }
  cpss_mem_memcpy(&aucNeighborAdvertsmnt[ulSuffix],&stStaIpV6Hdr,sizeof(stStaIpV6Hdr));
  ulSuffix = ulSuffix + sizeof(stStaIpV6Hdr);
  cpss_mem_memcpy(&aucNeighborAdvertsmnt[ulSuffix],&stNbAdmntICMPV6,sizeof(stNbAdmntICMPV6));
  ulSuffix = ulSuffix + sizeof(stNbAdmntICMPV6);

  gstNeighborAdvertsmnt.ulBufLen = ulSuffix;

  cpss_mem_memcpy(gstNeighborAdvertsmnt.aucBuff,aucNeighborAdvertsmnt,gstNeighborAdvertsmnt.ulBufLen);

  bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gstNeighborAdvertsmnt.aucBuff, gstNeighborAdvertsmnt.ulBufLen);

  if(MT_FALSE == bSendResult)
     {
    printf("Error.MT_ApSendMsg2UPM for NeighborAdvertisement is failure.\nFile: %s,Line: %d\n",__FILE__,__LINE__);
     }

  return bSendResult;
}

//DHCP Discover
MTBool ApMutiTestSendDhcpDiscover(MT_STA_WLAN_INFO_T vstStaWLanInfo)
{
  MTBool bSendResult = MT_FALSE;
  UINT8 aucDhcpDiscover[512] = {0};
  MT_ETH_HDR_T stEthHdr;
  MT_VLAN_HEADER_T stVlanHdr;
  MT_IPV4_HEADER_T stIpv4Hdr;
  MT_UDP_HDR_T stUdpHdr;
  MT_BOOTSTRAP_HDR_T stBootStrapHdr;
  UINT32 ulBtSrtapProtclLen = 0;
  MT_DHCP_OPTION_T stOption53;
  UINT32 ulOption53Len = 0;
  MT_DHCP_OPTION_T stOption116;
  UINT32 ulOption116Len = 0;
  MT_DHCP_OPTION_T stOption61;
  UINT32 ulOption61Len = 0;
  MT_DHCP_OPTION_T stOption12;
  UINT32 ulOption12Len = 0;
  MT_DHCP_OPTION_T stOption60;
  UINT32 ulOption60Len = 0;
  MT_DHCP_OPTION_T stOption55;
  UINT32 ulOption55Len = 0;
  MT_DHCP_OPTION_T stOption43;
  UINT32 ulOption43Len = 0;
  MT_DHCP_OPTION_T stEndOption;//0xFF
  UINT32 ulEndOptionLen = 0;
  UINT32 ulSuffix = 0;

  UINT32 ulLoop = 0;
  char *pHostname = "MT_Tester";
  char *pVendorClassId = "MSFT 5.0";

  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  UINT8 ucMacLen = 0;
  UINT8 aucBssId[MT_MAC_LENTH] = {0};
  UINT8 ucRadioId = 0;
  UINT8 ucWlanId = 0;
  UINT8 ucRcvd = 0;
  UINT32 ulDHCPTransacID = 0;
  UINT32 ulApIdx = 0;
  UINT32 ulStaIndex = 0;

  cpss_mem_memset(&stEthHdr,0,sizeof(stEthHdr));
  cpss_mem_memset(&stVlanHdr,0,sizeof(stVlanHdr));
  cpss_mem_memset(&stIpv4Hdr,0,sizeof(stIpv4Hdr));
  cpss_mem_memset(&stUdpHdr,0,sizeof(stUdpHdr));
  cpss_mem_memset(&stBootStrapHdr,0,sizeof(stBootStrapHdr));
  cpss_mem_memset(&stOption53,0,sizeof(stOption53));
  cpss_mem_memset(&stOption116,0,sizeof(stOption116));
  cpss_mem_memset(&stOption61,0,sizeof(stOption61));
  cpss_mem_memset(&stOption12,0,sizeof(stOption12));
  cpss_mem_memset(&stOption60,0,sizeof(stOption60));
  cpss_mem_memset(&stOption55,0,sizeof(stOption55));
  cpss_mem_memset(&stOption43,0,sizeof(stOption43));
  cpss_mem_memset(&stEndOption,0,sizeof(stEndOption));

  ulApIdx = vstStaWLanInfo.ulApIndex;
  ulStaIndex = vstStaWLanInfo.ulStaIndex;
  ucRadioId = vstStaWLanInfo.ucRadioId;
  ucWlanId = vstStaWLanInfo.ucWlanId;

//  ulDHCPTransacID = ulApIdx << 16 | ulStaIndex;
  ulDHCPTransacID = ulStaIndex;

  //BOOT STRAP PROTOCOL
  stBootStrapHdr.ucMsgType = 1;//Boot Request
  stBootStrapHdr.ucHWType = 1;//Ethernet
  stBootStrapHdr.ucHWAddrLen = 6;
  stBootStrapHdr.ucHops = 0;
  stBootStrapHdr.ulTransacID = htonl(ulDHCPTransacID);
  stBootStrapHdr.usSecondElaps = 0;
  MT_SetField16(stBootStrapHdr.usBootFlag,0,1,0);//BootCast Flag
  MT_SetField16(stBootStrapHdr.usBootFlag,1,15,0);//Rsvd Flag

  for(ulLoop = 0; ulLoop < MT_IPV4_ADDR_LEN; ulLoop++)
     {
    stBootStrapHdr.aucClientIpAddr[ulLoop] = 0;
    stBootStrapHdr.aucYourIpAddr[ulLoop] = 0;
    stBootStrapHdr.aucNxtSrvIpAddr[ulLoop] = 0;
    stBootStrapHdr.aucRelayAgntIpAddr[ulLoop] = 0;
     }
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stBootStrapHdr.aucClientMacAddr,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }

  for(ulLoop = 0; ulLoop < MT_HARDWAREPADDING_LEN; ulLoop++)
     {
    stBootStrapHdr.aucHwAddrPadding[ulLoop] = 0;
     }

  for(ulLoop = 0; ulLoop < MT_DHCP_SERVER_HOST_NAME_LEN; ulLoop++)
     {
    stBootStrapHdr.aucSrvName[ulLoop] = 0;
     }

  for(ulLoop = 0; ulLoop < MT_DHCP_BOOT_FILE_NAME_LEN; ulLoop++)
    {
    stBootStrapHdr.aucBtFileName[ulLoop] = 0;
    }

  stBootStrapHdr.ucMagicCookie[0] = 0x63;
  stBootStrapHdr.ucMagicCookie[1] = 0x82;
  stBootStrapHdr.ucMagicCookie[2] = 0x53;
  stBootStrapHdr.ucMagicCookie[3] = 0x63;

  //Option53:DHCP message Type
  stOption53.ucOption = 53;
  stOption53.ucOptionLen = 1;
  stOption53.aucOptnVal[0] = 1;
  ulOption53Len = sizeof(stOption53.ucOption) + sizeof(stOption53.ucOptionLen) + stOption53.ucOptionLen;

  //Option116:DHCP auto-configure
  stOption116.ucOption = 116;
  stOption116.ucOptionLen = 1;
  stOption116.aucOptnVal[0] = 1;
  ulOption116Len = sizeof(stOption116.ucOption) + sizeof(stOption116.ucOptionLen) + stOption116.ucOptionLen;

  //Option61:client identifier
  stOption61.ucOption = 61;
  stOption61.ucOptionLen = 7;
  stOption61.aucOptnVal[0] = 1;//Hardware Type:ETHERNET
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(&stOption61.aucOptnVal[1],gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }
  ulOption61Len = sizeof(stOption61.ucOption) + sizeof(stOption61.ucOptionLen) + stOption61.ucOptionLen;

  //Option12: host name
  stOption12.ucOption = 12;
  stOption12.ucOptionLen = strlen(pHostname);
  cpss_mem_memcpy(stOption12.aucOptnVal,pHostname,stOption12.ucOptionLen);
  ulOption12Len = sizeof(stOption12.ucOption) + sizeof(stOption12.ucOptionLen) + stOption12.ucOptionLen;

  //Option60:vendor class identifier
  stOption60.ucOption = 60;
  stOption60.ucOptionLen = strlen(pVendorClassId);
  cpss_mem_memcpy(stOption60.aucOptnVal,pVendorClassId,stOption60.ucOptionLen);
  ulOption60Len = sizeof(stOption60.ucOption) + sizeof(stOption60.ucOptionLen) + stOption60.ucOptionLen;

  //Option55:parameter request list
  stOption55.ucOption = 55;
  stOption55.ucOptionLen = 11;
  stOption55.aucOptnVal[0] = 1;//  subnet mask(1)
  stOption55.aucOptnVal[1] = 15;//  domain name(15)
  stOption55.aucOptnVal[2] = 3;//  router(3)
  stOption55.aucOptnVal[3] = 6;//  domain name server(6)
  stOption55.aucOptnVal[4] = 44;//  netbios over TCP/IP name server(44)
  stOption55.aucOptnVal[5] = 46;//  netbios over TCP/IP Node type(46)
  stOption55.aucOptnVal[6] = 47;//  netbios over TCP/IP scope(47)
  stOption55.aucOptnVal[7] = 31;//  perform router discover(31)
  stOption55.aucOptnVal[8] = 33;//  static route(33)
  stOption55.aucOptnVal[9] = 249;//  private/Classless static route(249)
  stOption55.aucOptnVal[10] = 43;//  vendor-specific infomation(43)
  ulOption55Len = sizeof(stOption55.ucOption) + sizeof(stOption55.ucOptionLen) + stOption55.ucOptionLen;

  //Option43:vendor-specific infomation
  stOption43.ucOption = 43;
  stOption43.ucOptionLen = 2;
  stOption43.aucOptnVal[0] = 0xdc;
  stOption43.aucOptnVal[1] = 0x00;
  ulOption43Len = sizeof(stOption43.ucOption) + sizeof(stOption43.ucOptionLen) + stOption43.ucOptionLen;

  //EndOption
  stEndOption.ucOption = 0xff;
  stEndOption.ucOptionLen = 0;
  ulEndOptionLen = sizeof(stEndOption.ucOption);

  ulBtSrtapProtclLen = sizeof(stBootStrapHdr)
      + ulOption53Len
      + ulOption116Len
      + ulOption61Len
      + ulOption12Len
      + ulOption60Len
      + ulOption55Len
      + ulOption43Len
      + ulEndOptionLen;

  //UDP HEADER
  stUdpHdr.usSrcPort = htons(68);
  stUdpHdr.usDstPort = htons(67);
  stUdpHdr.usPldLen = sizeof(stUdpHdr) + ulBtSrtapProtclLen;
  stUdpHdr.usPldLen = htons(stUdpHdr.usPldLen);
  stUdpHdr.usChkSum = 0x00;

  //IPv4 HEADER
  stIpv4Hdr.ucIpVerType = 0x45;
  MT_SetField16(stIpv4Hdr.ucDiffSerFd,0,6,0);//DSCP
  MT_SetField16(stIpv4Hdr.ucDiffSerFd,6,2,0);//ECN
  stIpv4Hdr.usTotalLen = htons(sizeof(stIpv4Hdr) + sizeof(stUdpHdr) + ulBtSrtapProtclLen);
  stIpv4Hdr.usIdentfier = htons(0x02b5);//693
  stIpv4Hdr.usFragOffset = 0x00;
  stIpv4Hdr.ucTtl = 64;
  stIpv4Hdr.ucProtoType = 17;//UDP

  for(ulLoop = 0; ulLoop < MT_IPV4_ADDR_LEN; ulLoop++)
     {
    stIpv4Hdr.aucSrcIpaddr[ulLoop] = 0;
    stIpv4Hdr.aucDstIpAddr[ulLoop] = 0xff;
     }

  stIpv4Hdr.usChkSum = 0;
  stIpv4Hdr.usChkSum = MT_CalcIpChksm((UINT8*)&stIpv4Hdr,sizeof(stIpv4Hdr));

  //VLAN HEADER
  MT_SetField16(stVlanHdr.usPriority,0,3,0);//Priority: Best Effort (default)0
  MT_SetField16(stVlanHdr.usPriority,3,1,0);//CFI:Canonical(0)
  MT_SetField16(stVlanHdr.usPriority,4,12,gastApconf[ulApIdx].ulStaVlan);//VLAN ID
  stVlanHdr.usType = htons(0x0800);//IPv4

  //ETH HEADER
  for(ulLoop = 0; ulLoop < MT_MAC_LENTH; ulLoop++)
     {
    stEthHdr.aucDstMac[ulLoop] = 0xff;
     }
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stEthHdr.aucSrcMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }

  if(1 == gstStaConfInfo.ucVlanFlag)
    {
    stEthHdr.usProtclType = htons(0x8100);//VLAN
    }
  else
    {
    stEthHdr.usProtclType = htons(0x0800);//IP
    }

  //CAPWAP HEADER
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,0);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,0);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,1);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

  //Fragment ID
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
  //Fragment Offset
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
  //Reserved
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

  ucMacLen = MT_MAC_LENTH;

  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);
  ucRcvd = 0;

  //construct capwap msg body
  ulSuffix = 0;
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&ucMacLen,sizeof(ucMacLen));
  ulSuffix = ulSuffix + sizeof(ucMacLen);
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],aucBssId,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&ucRcvd,sizeof(ucRcvd));
  ulSuffix = ulSuffix + sizeof(ucRcvd);

  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&stEthHdr,sizeof(stEthHdr));
  ulSuffix = ulSuffix + sizeof(stEthHdr);
  if(1 == gstStaConfInfo.ucVlanFlag)
     {
    cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&stVlanHdr,sizeof(stVlanHdr));
    ulSuffix = ulSuffix + sizeof(stVlanHdr);
     }

  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&stIpv4Hdr,sizeof(stIpv4Hdr));
  ulSuffix = ulSuffix + sizeof(stIpv4Hdr);
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&stUdpHdr,sizeof(stUdpHdr));
  ulSuffix = ulSuffix + sizeof(stUdpHdr);
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&stBootStrapHdr,sizeof(stBootStrapHdr));
  ulSuffix = ulSuffix + sizeof(stBootStrapHdr);
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&stOption53,ulOption53Len);
  ulSuffix = ulSuffix + ulOption53Len;
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&stOption116,ulOption116Len);
  ulSuffix = ulSuffix + ulOption116Len;
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&stOption61,ulOption61Len);
  ulSuffix = ulSuffix + ulOption61Len;
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&stOption12,ulOption12Len);
  ulSuffix = ulSuffix + ulOption12Len;
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&stOption60,ulOption60Len);
  ulSuffix = ulSuffix + ulOption60Len;
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&stOption55,ulOption55Len);
  ulSuffix = ulSuffix + ulOption55Len;
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&stOption43,ulOption43Len);
  ulSuffix = ulSuffix + ulOption43Len;
  cpss_mem_memcpy(&aucDhcpDiscover[ulSuffix],&stEndOption,ulEndOptionLen);
  ulSuffix = ulSuffix + ulEndOptionLen;

  gstDhcpDiscover.ulBufLen = ulSuffix;
  cpss_mem_memcpy(gstDhcpDiscover.aucBuff,aucDhcpDiscover,gstDhcpDiscover.ulBufLen);

  bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gstDhcpDiscover.aucBuff, gstDhcpDiscover.ulBufLen);

  if(MT_FALSE == bSendResult)
    {
   printf("Error.MT_ApSendMsg2UPM for ApMutiTestSendDhcpDiscover is failure.\nFile: %s,Line: %d\n",__FILE__,__LINE__);
    }

  return bSendResult;
}

//DHCP Request
MTBool ApMutiTestSendDhcpReq(MT_STA_WLAN_INFO_T vstStaWLanInfo)
{
	MTBool bSendResult = MT_FALSE;
  UINT8 aucDhcpReq[512] = {0};
  MT_BOOTSTRAP_HDR_T stBootStrapHdr;
  UINT32 ulBtSrtapProtclLen = 0;
  MT_DHCP_OPTION_T stOption53;
  UINT32 ulOption53Len = 0;
  MT_DHCP_OPTION_T stOption61;
  UINT32 ulOption61Len = 0;
  MT_DHCP_OPTION_T stOption50;
  UINT32 ulOption50Len = 0;
  MT_DHCP_OPTION_T stOption54;
  UINT32 ulOption54Len = 0;
  MT_DHCP_OPTION_T stOption12;
  UINT32 ulOption12Len = 0;
  MT_DHCP_OPTION_T stOption81;
  UINT32 ulOption81Len = 0;
  MT_DHCP_OPTION_T stOption60;
  UINT32 ulOption60Len = 0;
  MT_DHCP_OPTION_T stOption55;
  UINT32 ulOption55Len = 0;
  MT_DHCP_OPTION_T stOption43;
  UINT32 ulOption43Len = 0;
  MT_DHCP_OPTION_T stEndOption;//0xFF
  UINT32 ulEndOptionLen = 0;

  MT_ETH_HDR_T stEthHdr;
  MT_VLAN_HEADER_T stVlanHdr;
  MT_IPV4_HEADER_T stIpv4Hdr;
  MT_UDP_HDR_T stUdpHdr;
  UINT32 ulSuffix = 0;
  UINT32 ulLoop = 0;
  char *pHostname = "MT_Tester";
  char *pVendorClassId = "MSFT 5.0";
  UINT32 ulDHCPTransacID = 0;

  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  UINT8 ucMacLen = 0;
  UINT8 aucBssId[MT_MAC_LENTH] = {0};
  UINT8 aucDhcpv4ReqrdIpAddr[MT_IPV4_ADDR_LEN] = {0};
  UINT8 aucDhcpv4SrvIpAddr[MT_IPV4_ADDR_LEN] = {0};
  UINT32 ulApIdx = 0;
  UINT32 ulStaIndex = 0;
  UINT8 ucRadioId = 0;
  UINT8 ucWlanId = 0;
  UINT8 ucRcvd = 0;

  cpss_mem_memset(&stEthHdr,0,sizeof(stEthHdr));
  cpss_mem_memset(&stVlanHdr,0,sizeof(stVlanHdr));
  cpss_mem_memset(&stIpv4Hdr,0,sizeof(stIpv4Hdr));
  cpss_mem_memset(&stUdpHdr,0,sizeof(stUdpHdr));
  cpss_mem_memset(&stBootStrapHdr,0,sizeof(stBootStrapHdr));
  cpss_mem_memset(&stOption53,0,sizeof(stOption53));
  cpss_mem_memset(&stOption61,0,sizeof(stOption61));
  cpss_mem_memset(&stOption50,0,sizeof(stOption50));
  cpss_mem_memset(&stOption54,0,sizeof(stOption54));
  cpss_mem_memset(&stOption12,0,sizeof(stOption12));
  cpss_mem_memset(&stOption81,0,sizeof(stOption81));
  cpss_mem_memset(&stOption60,0,sizeof(stOption60));
  cpss_mem_memset(&stOption55,0,sizeof(stOption55));
  cpss_mem_memset(&stOption43,0,sizeof(stOption43));
  cpss_mem_memset(&stEndOption,0,sizeof(stEndOption));

  ulApIdx = vstStaWLanInfo.ulApIndex;
  ulStaIndex = vstStaWLanInfo.ulStaIndex;
  ucRadioId = vstStaWLanInfo.ucRadioId;
  ucWlanId = vstStaWLanInfo.ucWlanId;

//  ulDHCPTransacID = ulApIdx << 16 | ulStaIndex;
  ulDHCPTransacID = ulStaIndex;
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(aucDhcpv4ReqrdIpAddr,gastStaPara[ulStaIndex].aucStaIpV4Addr,MT_IPV4_ADDR_LEN);//STA IP Addr
	  cpss_mem_memcpy(aucDhcpv4SrvIpAddr,gastStaPara[ulStaIndex].aucStaIpV4Addr,MT_IPV4_ADDR_LEN);  //DHCP Srv IP Addr
  }

  //BOOT STRAP PROTOCOL
  stBootStrapHdr.ucMsgType = 1;//Boot Request
  stBootStrapHdr.ucHWType = 1;//Ethernet
  stBootStrapHdr.ucHWAddrLen = 6;
  stBootStrapHdr.ucHops = 0;
  stBootStrapHdr.ulTransacID = htonl(ulDHCPTransacID);
  stBootStrapHdr.usSecondElaps = 0;
  MT_SetField16(stBootStrapHdr.usBootFlag,0,1,0);//BootCast Flag
  MT_SetField16(stBootStrapHdr.usBootFlag,1,15,0);//Rsvd Flag

  for(ulLoop = 0; ulLoop < MT_IPV4_ADDR_LEN; ulLoop++)
     {
    stBootStrapHdr.aucClientIpAddr[ulLoop] = 0;
    stBootStrapHdr.aucYourIpAddr[ulLoop] = 0;
    stBootStrapHdr.aucNxtSrvIpAddr[ulLoop] = 0;
    stBootStrapHdr.aucRelayAgntIpAddr[ulLoop] = 0;
     }
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stBootStrapHdr.aucClientMacAddr,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }

  for(ulLoop = 0; ulLoop < MT_HARDWAREPADDING_LEN; ulLoop++)
     {
    stBootStrapHdr.aucHwAddrPadding[ulLoop] = 0;
     }

  for(ulLoop = 0; ulLoop < MT_DHCP_SERVER_HOST_NAME_LEN; ulLoop++)
     {
    stBootStrapHdr.aucSrvName[ulLoop] = 0;
     }

  for(ulLoop = 0; ulLoop < MT_DHCP_BOOT_FILE_NAME_LEN; ulLoop++)
    {
    stBootStrapHdr.aucBtFileName[ulLoop] = 0;
    }

  stBootStrapHdr.ucMagicCookie[0] = 0x63;
  stBootStrapHdr.ucMagicCookie[1] = 0x82;
  stBootStrapHdr.ucMagicCookie[2] = 0x53;
  stBootStrapHdr.ucMagicCookie[3] = 0x63;

  //Option53 DHCP message type = dhcp request
  stOption53.ucOption = 53;
  stOption53.ucOptionLen = 1;
  stOption53.aucOptnVal[0] = 3;
  ulOption53Len = sizeof(stOption53.ucOption) + sizeof(stOption53.ucOptionLen) + stOption53.ucOptionLen;

  //Option61 client identifier
  stOption61.ucOption = 61;
  stOption61.ucOptionLen = 7;
  stOption61.aucOptnVal[0] = 1;//Ethernet
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(&stOption61.aucOptnVal[1],gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }
  ulOption61Len = sizeof(stOption61.ucOption) + sizeof(stOption61.ucOptionLen) + stOption61.ucOptionLen;

  //Option50 required IP Address
  stOption50.ucOption = 50;
  stOption50.ucOptionLen = MT_IPV4_ADDR_LEN;
  cpss_mem_memcpy(stOption50.aucOptnVal,aucDhcpv4ReqrdIpAddr,MT_IPV4_ADDR_LEN);
  ulOption50Len = sizeof(stOption50.ucOption) + sizeof(stOption50.ucOptionLen) + stOption50.ucOptionLen;

  //Option54 DHCP Server Identifier
  stOption54.ucOption = 54;
  stOption54.ucOptionLen = MT_IPV4_ADDR_LEN;
  cpss_mem_memcpy(stOption54.aucOptnVal,aucDhcpv4SrvIpAddr,MT_IPV4_ADDR_LEN);
  ulOption54Len = sizeof(stOption54.ucOption) + sizeof(stOption54.ucOptionLen) + stOption54.ucOptionLen;

  //Option12 Host name
  stOption12.ucOption = 12;
  stOption12.ucOptionLen = strlen(pHostname);
  cpss_mem_memcpy(stOption12.aucOptnVal,pHostname,stOption12.ucOptionLen);
  ulOption12Len = sizeof(stOption12.ucOption) + sizeof(stOption12.ucOptionLen) + stOption12.ucOptionLen;

  //Option81 client fully qualified Domain name
  stOption81.ucOption = 81;
  stOption81.ucOptionLen = 4 + strlen(pHostname);
  stOption81.aucOptnVal[0] = 0;//Flags
  stOption81.aucOptnVal[1] = 0;//A-RR Result
  stOption81.aucOptnVal[2] = 0;//PTR-RR result
  cpss_mem_memcpy(&stOption81.aucOptnVal[3],pHostname,strlen(pHostname));
  cpss_mem_memcpy(&stOption81.aucOptnVal[3 + strlen(pHostname)],".",1);
  ulOption81Len = sizeof(stOption81.ucOption) + sizeof(stOption81.ucOptionLen) + stOption81.ucOptionLen;

 //Option60 Vendor class identifier = "MFT 5.0"
  stOption60.ucOption = 60;
  stOption60.ucOptionLen = strlen(pVendorClassId);
  cpss_mem_memcpy(stOption60.aucOptnVal,pVendorClassId,stOption60.ucOptionLen);
  ulOption60Len = sizeof(stOption60.ucOption) + sizeof(stOption60.ucOptionLen) + stOption60.ucOptionLen;

  //Option55 Para,eter Request List
  stOption55.ucOption = 55;
  stOption55.ucOptionLen = 11;
  stOption55.aucOptnVal[0] = 1;//  subnet mask(1)
  stOption55.aucOptnVal[1] = 15;//  domain name(15)
  stOption55.aucOptnVal[2] = 3;//  router(3)
  stOption55.aucOptnVal[3] = 6;//  domain name server(6)
  stOption55.aucOptnVal[4] = 44;//  netbios over TCP/IP name server(44)
  stOption55.aucOptnVal[5] = 46;//  netbios over TCP/IP Node type(46)
  stOption55.aucOptnVal[6] = 47;//  netbios over TCP/IP scope(47)
  stOption55.aucOptnVal[7] = 31;//  perform router discover(31)
  stOption55.aucOptnVal[8] = 33;//  static route(33)
  stOption55.aucOptnVal[9] = 249;//  private/Classless static route(249)
  stOption55.aucOptnVal[10] = 43;//  vendor-specific infomation(43)
  ulOption55Len = sizeof(stOption55.ucOption) + sizeof(stOption55.ucOptionLen) + stOption55.ucOptionLen;

  //Option43 Vendor_specfic nformation
  stOption43.ucOption = 43;
  stOption43.ucOptionLen = 3;
  stOption43.aucOptnVal[0] = 0xdc;
  stOption43.aucOptnVal[1] = 0x00;
  stOption43.aucOptnVal[2] = 0x00;
  ulOption43Len = sizeof(stOption43.ucOption) + sizeof(stOption43.ucOptionLen) + stOption43.ucOptionLen;

  //Option:end option
  stEndOption.ucOption = 0xff;
  stEndOption.ucOptionLen = 0;
  ulEndOptionLen = sizeof(stEndOption.ucOption);

  ulBtSrtapProtclLen = sizeof(stBootStrapHdr)
      + ulOption53Len
      + ulOption61Len
      + ulOption50Len
      + ulOption54Len
      + ulOption12Len
      + ulOption81Len
      + ulOption60Len
      + ulOption55Len
      + ulOption43Len
      + ulEndOptionLen;

  //UDP HEADER
  stUdpHdr.usSrcPort = htons(68);
  stUdpHdr.usDstPort = htons(67);
  stUdpHdr.usPldLen = sizeof(stUdpHdr) + ulBtSrtapProtclLen;
  stUdpHdr.usPldLen = htons(stUdpHdr.usPldLen);
  stUdpHdr.usChkSum = 0x00;

  //IPv4 HEADER
  stIpv4Hdr.ucIpVerType = 0x45;
  MT_SetField16(stIpv4Hdr.ucDiffSerFd,0,6,0);//DSCP
  MT_SetField16(stIpv4Hdr.ucDiffSerFd,6,2,0);//ECN
  stIpv4Hdr.usTotalLen = htons(sizeof(stIpv4Hdr) + sizeof(stUdpHdr) + ulBtSrtapProtclLen);
  stIpv4Hdr.usIdentfier = htons(0x02b6);//694
  stIpv4Hdr.usFragOffset = 0x00;
  stIpv4Hdr.ucTtl = 64;
  stIpv4Hdr.ucProtoType = 17;//UDP

  for(ulLoop = 0; ulLoop < MT_IPV4_ADDR_LEN; ulLoop++)
     {
    stIpv4Hdr.aucSrcIpaddr[ulLoop] = 0;
    stIpv4Hdr.aucDstIpAddr[ulLoop] = 0xff;
     }

  stIpv4Hdr.usChkSum = 0;
  stIpv4Hdr.usChkSum = MT_CalcIpChksm((UINT8*)&stIpv4Hdr,sizeof(stIpv4Hdr));

  //VLAN HEADER
  MT_SetField16(stVlanHdr.usPriority,0,3,0);//Priority: Best Effort (default)0
  MT_SetField16(stVlanHdr.usPriority,3,1,0);//CFI:Canonical(0)
  MT_SetField16(stVlanHdr.usPriority,4,12,gastApconf[ulApIdx].ulStaVlan);//VLAN ID
  stVlanHdr.usType = htons(0x0800);//IPv4

  //ETH HEADER
  for(ulLoop = 0; ulLoop < MT_MAC_LENTH; ulLoop++)
     {
    stEthHdr.aucDstMac[ulLoop] = 0xff;
     }
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stEthHdr.aucSrcMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }

  if(1 == gstStaConfInfo.ucVlanFlag)
    {
    stEthHdr.usProtclType = htons(0x8100);//VLAN
    }
  else
    {
    stEthHdr.usProtclType = htons(0x0800);//IP
    }

  //CAPWAP HEADER
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,0);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,0);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,1);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

  //Fragment ID
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
  //Fragment Offset
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
  //Reserved
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

  ucMacLen = MT_MAC_LENTH;

  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);
  ucRcvd = 0;

  //construct capwap msg body
  ulSuffix = 0;
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&ucMacLen,sizeof(ucMacLen));
  ulSuffix = ulSuffix + sizeof(ucMacLen);
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],aucBssId,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&ucRcvd,sizeof(ucRcvd));
  ulSuffix = ulSuffix + sizeof(ucRcvd);

  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stEthHdr,sizeof(stEthHdr));
  ulSuffix = ulSuffix + sizeof(stEthHdr);
  if(1 == gstStaConfInfo.ucVlanFlag)
     {
    cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stVlanHdr,sizeof(stVlanHdr));
    ulSuffix = ulSuffix + sizeof(stVlanHdr);
     }

  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stIpv4Hdr,sizeof(stIpv4Hdr));
  ulSuffix = ulSuffix + sizeof(stIpv4Hdr);
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stUdpHdr,sizeof(stUdpHdr));
  ulSuffix = ulSuffix + sizeof(stUdpHdr);
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stBootStrapHdr,sizeof(stBootStrapHdr));
  ulSuffix = ulSuffix + sizeof(stBootStrapHdr);
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stOption53,ulOption53Len);
  ulSuffix = ulSuffix + ulOption53Len;
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stOption61,ulOption61Len);
  ulSuffix = ulSuffix + ulOption61Len;
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stOption50,ulOption50Len);
  ulSuffix = ulSuffix + ulOption50Len;
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stOption54,ulOption54Len);
  ulSuffix = ulSuffix + ulOption54Len;
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stOption12,ulOption12Len);
  ulSuffix = ulSuffix + ulOption12Len;
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stOption81,ulOption81Len);
  ulSuffix = ulSuffix + ulOption81Len;
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stOption60,ulOption60Len);
  ulSuffix = ulSuffix + ulOption60Len;
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stOption55,ulOption55Len);
  ulSuffix = ulSuffix + ulOption55Len;
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stOption43,ulOption43Len);
  ulSuffix = ulSuffix + ulOption43Len;
  cpss_mem_memcpy(&aucDhcpReq[ulSuffix],&stEndOption,ulEndOptionLen);
  ulSuffix = ulSuffix + ulEndOptionLen;

  gstDhcpReq.ulBufLen = ulSuffix;
  cpss_mem_memcpy(gstDhcpReq.aucBuff,aucDhcpReq,gstDhcpReq.ulBufLen);

  bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gstDhcpReq.aucBuff, gstDhcpReq.ulBufLen);

  if(MT_FALSE == bSendResult)
    {
   printf("Error.MT_ApSendMsg2UPM for ApMutiTestSendDhcpReq is failure.\nFile: %s,Line: %d\n",__FILE__,__LINE__);
    }

  return bSendResult;
}

//DHCPv6
MTBool ApMutiTestSendDhcpv6Solicit(MT_STA_WLAN_INFO_T vstStaWLanInfo)
{
  MTBool bSendResult = MT_FALSE;
  UINT8 aucDhcpv6Solicit[512] = {0};
  MT_ETH_HDR_T stEthHdr;
  MT_VLAN_HEADER_T stVlanHdr;
  MT_IPV6_HEADER_T stIpv6Hdr;
  MT_UDP_HDR_T stUdpHdr;
  MT_DHCPV6_T stDhcpv6Solicit;
  UINT32 ulDhcpv6OptLen = 0;
  MT_DHCPV6_OLV_T stClientIdentifier;
  UINT32 ulClntIdntfLen = 0;
  UINT16 usDuidType = 0;
  UINT16 usHWType = 0;
  UINT32 ulTime = 0;
  UINT32 ulTmFalg = 0;
  UINT8 aucLLAddr[MT_MAC_LENTH] = {0};
  MT_DHCPV6_OLV_T stIdentityAssociation;
  UINT32 ulIdentityAssocLen = 0;
  UINT32 ulIAID = 0;
  UINT32 ulT1 = 0;
  UINT32 ulT2 = 0;
  MT_DHCPV6_OLV_T stIaAddress;
  UINT32 ulIaAddLen = 0;
  UINT8 aucIpv6Addr[MT_IPV6_ADDR_LEN] = {0};
  UINT32 ulPrefLifetime = 0;
  UINT32 ulValidLifetime = 0;
  MT_DHCPV6_OLV_T stElapedTime;
  UINT32 ulElapsdTmLen = 0;
  UINT16 usElapsedTime = 0;
  UINT8 aucIpv6SrcAdd[MT_IPV6_ADDR_LEN] = {0};
  UINT8 aucIpv6DstAdd[MT_IPV6_ADDR_LEN] = {0};
  UINT8 aucIpv6McastAddr[MT_MAC_LENTH] = {0};

  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  UINT32 ulSuffix = 0;
  UINT32 ulLoop = 0;
  UINT8 ucMacLen = 0;
  UINT8 ucRcvd = 0;
  UINT32 ulApIdx  = 0;
  UINT32 ulStaIndex = 0;
  UINT8 ucRadioId = 0;
  UINT8 ucWlanId = 0;
  UINT8 aucBssId[MT_MAC_LENTH] = {0};

  cpss_mem_memset(&stEthHdr,0,sizeof(stEthHdr));
  cpss_mem_memset(&stVlanHdr,0,sizeof(stVlanHdr));
  cpss_mem_memset(&stIpv6Hdr,0,sizeof(stIpv6Hdr));
  cpss_mem_memset(&stUdpHdr,0,sizeof(stUdpHdr));
  cpss_mem_memset(&stDhcpv6Solicit,0,sizeof(stDhcpv6Solicit));
  cpss_mem_memset(&stClientIdentifier,0,sizeof(stClientIdentifier));
  cpss_mem_memset(&stIdentityAssociation,0,sizeof(stIdentityAssociation));
  cpss_mem_memset(&stIaAddress,0,sizeof(stIaAddress));
  cpss_mem_memset(&stElapedTime,0,sizeof(stElapedTime));
//  cpss_mem_memset(&,0,sizeof());
//  cpss_mem_memset(&,0,sizeof());

  ulApIdx  = vstStaWLanInfo.ulApIndex;
  ulStaIndex = vstStaWLanInfo.ulStaIndex;
  ucRadioId = vstStaWLanInfo.ucRadioId;
  ucWlanId = vstStaWLanInfo.ucWlanId;

  aucIpv6McastAddr[0] = 0x33;
  aucIpv6McastAddr[1] = 0x33;
  aucIpv6McastAddr[2] = 0xff;
  aucIpv6McastAddr[3] = 0x00;
  aucIpv6McastAddr[4] = 0x00;
  aucIpv6McastAddr[5] = 0x01;
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  aucIpv6SrcAdd[0] = 0xfe;
	  aucIpv6SrcAdd[1] = 0x80;
	  aucIpv6SrcAdd[8] = 0x02;
	  aucIpv6SrcAdd[9] = gastStaPara[ulStaIndex].auStaMac[1];
	  aucIpv6SrcAdd[10] = gastStaPara[ulStaIndex].auStaMac[2];
	  aucIpv6SrcAdd[11] = 0xff;
	  aucIpv6SrcAdd[12] = 0xfe;
	  aucIpv6SrcAdd[13] = gastStaPara[ulStaIndex].auStaMac[3];
	  aucIpv6SrcAdd[14] = gastStaPara[ulStaIndex].auStaMac[4];
	  aucIpv6SrcAdd[15] = gastStaPara[ulStaIndex].auStaMac[5];
  }

  aucIpv6DstAdd[0] = 0xff;
  aucIpv6DstAdd[1] = 0x02;
  aucIpv6DstAdd[12] = 0x00;
  aucIpv6DstAdd[13] = 0x01;
  aucIpv6DstAdd[14] = 0x00;
  aucIpv6DstAdd[15] = 0x02;

  //DHCPV6
  MT_SetField32(stDhcpv6Solicit.ulMsgTypeTransId,0,8,1);//Message Type:Solicit(1)
  MT_SetField32(stDhcpv6Solicit.ulMsgTypeTransId,8,16,(UINT16)(ulApIdx + 1));//Transaction ID:High 2 Bytes
  MT_SetField32(stDhcpv6Solicit.ulMsgTypeTransId,24,8,(UINT8)(ulStaIndex + 1));//Transaction ID:Low 1 Bytes
  stDhcpv6Solicit.ulMsgTypeTransId = htonl(stDhcpv6Solicit.ulMsgTypeTransId);

  //Client Identifier
  stClientIdentifier.usOptionID = 1;
  usDuidType = 3;//Link-Layer address plus time(1);Link-Layer address(3)
  usHWType = 6;//IEEE 802(6)
  ulTime = cpss_gmt_get();

  if(1 == usDuidType)
     {
	  ulTmFalg = 1;//with Time
	  stClientIdentifier.usLen = sizeof(usDuidType) + sizeof(usHWType) + sizeof(ulTime) + MT_MAC_LENTH;
     }
  else
     {
	  ulTmFalg = 0;//without time
	  stClientIdentifier.usLen = sizeof(usDuidType) + sizeof(usHWType) + MT_MAC_LENTH;
     }

  stClientIdentifier.usOptionID = htons(stClientIdentifier.usOptionID);
  usDuidType = htons(usDuidType);
  usHWType = htons(usHWType);
  ulTime = htonl(ulTime);
//  time(&ulTime);
//  ulTime = htonl(ulTime);
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(aucLLAddr,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }

  ulSuffix = 0;
  cpss_mem_memcpy(&stClientIdentifier.aucVal[ulSuffix],&usDuidType,sizeof(usDuidType));
  ulSuffix = ulSuffix + sizeof(usDuidType);
  cpss_mem_memcpy(&stClientIdentifier.aucVal[ulSuffix],&usHWType,sizeof(usHWType));
  ulSuffix = ulSuffix + sizeof(usHWType);
  if(1 == ulTmFalg)
     {
	   cpss_mem_memcpy(&stClientIdentifier.aucVal[ulSuffix],&ulTime,sizeof(ulTime));
	   ulSuffix = ulSuffix + sizeof(ulTime);
      }
  cpss_mem_memcpy(&stClientIdentifier.aucVal[ulSuffix],aucLLAddr,MT_MAC_LENTH);
  ulClntIdntfLen = sizeof(stClientIdentifier.usOptionID) + sizeof(stClientIdentifier.usLen) + stClientIdentifier.usLen;
  stClientIdentifier.usLen = htons(stClientIdentifier.usLen);

  //Identity Association for non_tempory Address
  stIdentityAssociation.usOptionID = htons(3);
  ulIAID = htonl(0x01);
  ulT1 = htonl(0xffffffff);//infinity
  ulT2 = htonl(0xffffffff);//infinity
  stIaAddress.usOptionID = htons(5);//IA Address
  stIaAddress.usLen = MT_IPV6_ADDR_LEN + sizeof(ulPrefLifetime) + sizeof(ulValidLifetime);
  ulIaAddLen = sizeof(stIaAddress.usOptionID) + sizeof(stIaAddress.usLen) + stIaAddress.usLen;
  stIaAddress.usLen = htons(stIaAddress.usLen);
  for(ulLoop = 0; ulLoop < MT_IPV6_ADDR_LEN; ulLoop++)
     {
	  aucIpv6Addr[ulLoop] = 0;
     }
  ulPrefLifetime = 0xffffffff;
  ulValidLifetime = 0xffffffff;

  ulSuffix = 0;
  cpss_mem_memcpy(&stIaAddress.aucVal[ulSuffix],aucIpv6Addr,MT_IPV6_ADDR_LEN);
  ulSuffix = ulSuffix + MT_IPV6_ADDR_LEN;
  cpss_mem_memcpy(&stIaAddress.aucVal[ulSuffix],&ulPrefLifetime,sizeof(ulPrefLifetime));
  ulSuffix = ulSuffix + sizeof(ulPrefLifetime);
  cpss_mem_memcpy(&stIaAddress.aucVal[ulSuffix],&ulValidLifetime,sizeof(ulValidLifetime));

  stIdentityAssociation.usLen = sizeof(ulIAID) + sizeof(ulT1) + sizeof(ulT2) + ulIaAddLen;
  ulIdentityAssocLen = sizeof(stIdentityAssociation.usOptionID) + sizeof(stIdentityAssociation.usLen) + stIdentityAssociation.usLen;
  stIdentityAssociation.usLen = htons(stIdentityAssociation.usLen);

  ulSuffix = 0;
  cpss_mem_memcpy(&stIdentityAssociation.aucVal[ulSuffix],&ulIAID,sizeof(ulIAID));
  ulSuffix = ulSuffix + sizeof(ulIAID);
  cpss_mem_memcpy(&stIdentityAssociation.aucVal[ulSuffix],&ulT1,sizeof(ulT1));
  ulSuffix = ulSuffix + sizeof(ulT1);
  cpss_mem_memcpy(&stIdentityAssociation.aucVal[ulSuffix],&ulT2,sizeof(ulT2));
  ulSuffix = ulSuffix + sizeof(ulT2);
  cpss_mem_memcpy(&stIdentityAssociation.aucVal[ulSuffix],&stIaAddress,ulIaAddLen);

  //Elapsed Time
  stElapedTime.usOptionID = htons(8);
  usElapsedTime = htons(0x8ec4);
  stElapedTime.usLen = sizeof(usElapsedTime);
  cpss_mem_memcpy(&stElapedTime.aucVal[0],&usElapsedTime,stElapedTime.usLen);
  ulElapsdTmLen = sizeof(stElapedTime.usOptionID) + sizeof(stElapedTime.usLen) + stElapedTime.usLen;
  stElapedTime.usLen = htons(stElapedTime.usLen);

  //construct dhcpv6-solicit
  ulSuffix = 0;
  cpss_mem_memcpy(&stDhcpv6Solicit.aucDhcpv6OptionVal[ulSuffix],&stClientIdentifier,ulClntIdntfLen);
  ulSuffix = ulSuffix + ulClntIdntfLen;
  cpss_mem_memcpy(&stDhcpv6Solicit.aucDhcpv6OptionVal[ulSuffix],&stIdentityAssociation,ulIdentityAssocLen);
  ulSuffix = ulSuffix + ulIdentityAssocLen;
  cpss_mem_memcpy(&stDhcpv6Solicit.aucDhcpv6OptionVal[ulSuffix],&stElapedTime,ulElapsdTmLen);
  ulSuffix = ulSuffix + ulElapsdTmLen;
  ulDhcpv6OptLen = sizeof(stDhcpv6Solicit.ulMsgTypeTransId) + ulSuffix;

  //UDP
  stUdpHdr.usSrcPort = htons(MT_DHCPV6_SRC_PORT);//dhcpv6-client
  stUdpHdr.usDstPort = htons(MT_DHCPV6_DST_PORT);//dhcpv6-server
  stUdpHdr.usPldLen = sizeof(stUdpHdr) + ulDhcpv6OptLen;
  stUdpHdr.usPldLen = htons(stUdpHdr.usPldLen);
  stUdpHdr.usChkSum = 0;

  //IPv6
  MT_SetField32(stIpv6Hdr.ulIPVerTraffic,0,4,6);//Ip version
  MT_SetField32(stIpv6Hdr.ulIPVerTraffic,4,8,0);//Traffic Class
  MT_SetField32(stIpv6Hdr.ulIPVerTraffic,12,20,0);//Flow Label
  stIpv6Hdr.ulIPVerTraffic = htonl(stIpv6Hdr.ulIPVerTraffic);
  stIpv6Hdr.usPayloadLen = sizeof(stUdpHdr) + ulDhcpv6OptLen;//Payload
  stIpv6Hdr.usPayloadLen = htons(stIpv6Hdr.usPayloadLen);
  stIpv6Hdr.ucNextHdr = 0x11;//UDP
  stIpv6Hdr.ucHopLmt = 128;
  cpss_mem_memcpy(stIpv6Hdr.aucSrcIpV6addr,aucIpv6SrcAdd,MT_IPV6_ADDR_LEN);
  cpss_mem_memcpy(stIpv6Hdr.aucDstIpV6Addr,aucIpv6DstAdd,MT_IPV6_ADDR_LEN);

  //Vlan
  MT_SetField32(stVlanHdr.usPriority,0,3,0);//Priority
  MT_SetField32(stVlanHdr.usPriority,3,1,0);//CFI
  MT_SetField32(stVlanHdr.usPriority,4,12,gastApconf[ulApIdx].ulStaVlan);//Vlan Id
  stVlanHdr.usPriority = htons(stVlanHdr.usPriority);
  stVlanHdr.usType = htons(0x86dd);//IPv6

  //Ethernet
  cpss_mem_memcpy(stEthHdr.aucDstMac,aucIpv6McastAddr,MT_MAC_LENTH);
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stEthHdr.aucSrcMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }

  if(1 == gstStaConfInfo.ucVlanFlag)
    {
    stEthHdr.usProtclType = htons(0x8100);//VLAN
    }
  else
    {
    stEthHdr.usProtclType = htons(0x86dd);//IP
    }

  //CAPWAP HEADER
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,0);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,0);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,1);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

  //Fragment ID
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
  //Fragment Offset
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
  //Reserved
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

  ucMacLen = MT_MAC_LENTH;

  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);
  ucRcvd = 0;

  //construct capwap body
  ulSuffix = 0;
  cpss_mem_memcpy(&aucDhcpv6Solicit[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
  cpss_mem_memcpy(&aucDhcpv6Solicit[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
  cpss_mem_memcpy(&aucDhcpv6Solicit[ulSuffix],&ucMacLen,sizeof(ucMacLen));
  ulSuffix = ulSuffix + sizeof(ucMacLen);
  cpss_mem_memcpy(&aucDhcpv6Solicit[ulSuffix],aucBssId,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  cpss_mem_memcpy(&aucDhcpv6Solicit[ulSuffix],&ucRcvd,sizeof(ucRcvd));
  ulSuffix = ulSuffix + sizeof(ucRcvd);

  cpss_mem_memcpy(&aucDhcpv6Solicit[ulSuffix],&stEthHdr,sizeof(stEthHdr));
  ulSuffix = ulSuffix + sizeof(stEthHdr);
  if(1 == gstStaConfInfo.ucVlanFlag)
     {
	  cpss_mem_memcpy(&aucDhcpv6Solicit[ulSuffix],&stVlanHdr,sizeof(stVlanHdr));
	  ulSuffix = ulSuffix + sizeof(stVlanHdr);
     }

  cpss_mem_memcpy(&aucDhcpv6Solicit[ulSuffix],&stIpv6Hdr,sizeof(stIpv6Hdr));
  ulSuffix = ulSuffix + sizeof(stIpv6Hdr);

  cpss_mem_memcpy(&aucDhcpv6Solicit[ulSuffix],&stUdpHdr,sizeof(stUdpHdr));
  ulSuffix = ulSuffix + sizeof(stUdpHdr);

  cpss_mem_memcpy(&aucDhcpv6Solicit[ulSuffix],&stDhcpv6Solicit,ulDhcpv6OptLen);
  ulSuffix = ulSuffix + ulDhcpv6OptLen;

  gstDhcpv6Solicit.ulBufLen = ulSuffix;
  cpss_mem_memcpy(gstDhcpv6Solicit.aucBuff,aucDhcpv6Solicit,gstDhcpv6Solicit.ulBufLen);

  bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gstDhcpv6Solicit.aucBuff, gstDhcpv6Solicit.ulBufLen);

  if(MT_FALSE == bSendResult)
    {
   printf("Error.MT_ApSendMsg2UPM for ApMutiTestSendDhcpv6Solicit is failure.\nFile: %s,Line: %d\n",__FILE__,__LINE__);
    }

  return bSendResult;
}

//DHCPV6 Request
MTBool ApMutiTestSendDhcpv6Req(MT_STA_WLAN_INFO_T vstStaWLanInfo)
{
  MTBool bSendResult = MT_FALSE;
  UINT8 aucDhcpv6Req[512] = {0};
  MT_ETH_HDR_T stEthHdr;
  MT_VLAN_HEADER_T stVlanHdr;
  MT_IPV6_HEADER_T stIpv6Hdr;
  MT_UDP_HDR_T stUdpHdr;
  MT_DHCPV6_T stDhcpv6Req;
  UINT32 ulDhcpv6ReqLen = 0;
  MT_DHCPV6_OLV_T stClientIdentifier;
  UINT32 ulClntIdntfLen = 0;
  UINT16 usDuidType = 0;
  UINT16 usHWType = 0;
  UINT32 ulTime = 0;
  UINT32 ulTmFlag = 0;
  UINT8 aucLLAddr[MT_MAC_LENTH] = {0};
  MT_DHCPV6_OLV_T stIdentityAssociation;
  UINT32 ulIdentityAssocLen = 0;
  UINT32 ulIAID = 0;
  UINT32 ulT1 = 0;
  UINT32 ulT2 = 0;
  MT_DHCPV6_OLV_T stIaAddress;
  UINT32 ulIaAddLen = 0;
  UINT32 ulPrefLifetime = 0;
  UINT32 ulValidLifetime = 0;
  MT_DHCPV6_OLV_T stElapedTime;
  UINT32 ulElapsdTmLen = 0;
  UINT16 usElapsedTime = 0;
  UINT8 aucIpv6SrcAdd[MT_IPV6_ADDR_LEN] = {0};
  UINT8 aucIpv6DstAdd[MT_IPV6_ADDR_LEN] = {0};
  UINT8 aucIpv6McastAddr[MT_MAC_LENTH] = {0};
  MT_DHCPV6_OLV_T stSrvIndentifier;
  UINT32 ulSrvIdentLen = 0;

  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  UINT8 ucMacLen = 0;
  UINT8 aucBssId[MT_MAC_LENTH] = {0};
  UINT8 ucRcvd = 0;
  UINT32 ulSuffix = 0;
  UINT32 ulApIdx = 0;
  UINT32 ulStaIndex = 0;
  UINT8 ucRadioId = 0;
  UINT8 ucWlanId = 0;
//  UINT32 ulLoop = 0;

  cpss_mem_memset(&stEthHdr,0,sizeof(stEthHdr));
  cpss_mem_memset(&stVlanHdr,0,sizeof(stVlanHdr));
  cpss_mem_memset(&stIpv6Hdr,0,sizeof(stIpv6Hdr));
  cpss_mem_memset(&stUdpHdr,0,sizeof(stUdpHdr));
  cpss_mem_memset(&stDhcpv6Req,0,sizeof(stDhcpv6Req));
  cpss_mem_memset(&stClientIdentifier,0,sizeof(stClientIdentifier));
  cpss_mem_memset(&stIdentityAssociation,0,sizeof(stIdentityAssociation));
  cpss_mem_memset(&stIaAddress,0,sizeof(stIaAddress));
  cpss_mem_memset(&stElapedTime,0,sizeof(stElapedTime));
  cpss_mem_memset(&stSrvIndentifier,0,sizeof(stSrvIndentifier));

  ulApIdx = vstStaWLanInfo.ulApIndex;
  ulStaIndex = vstStaWLanInfo.ulStaIndex;
  ucRadioId = vstStaWLanInfo.ucRadioId;
  ucWlanId = vstStaWLanInfo.ucWlanId;

//  cpss_mem_memset(&,0,sizeof());
  aucIpv6McastAddr[0] = 0x33;
  aucIpv6McastAddr[1] = 0x33;
  aucIpv6McastAddr[2] = 0xff;
  aucIpv6McastAddr[3] = 0x00;
  aucIpv6McastAddr[4] = 0x00;
  aucIpv6McastAddr[5] = 0x01;

  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  aucIpv6SrcAdd[0] = 0xfe;
	  aucIpv6SrcAdd[1] = 0x80;
	  aucIpv6SrcAdd[8] = 0x02;
	  aucIpv6SrcAdd[9] = gastStaPara[ulStaIndex].auStaMac[1];
	  aucIpv6SrcAdd[10] = gastStaPara[ulStaIndex].auStaMac[2];
	  aucIpv6SrcAdd[11] = 0xff;
	  aucIpv6SrcAdd[12] = 0xfe;
	  aucIpv6SrcAdd[13] = gastStaPara[ulStaIndex].auStaMac[3];
	  aucIpv6SrcAdd[14] = gastStaPara[ulStaIndex].auStaMac[4];
	  aucIpv6SrcAdd[15] = gastStaPara[ulStaIndex].auStaMac[5];
  }
  aucIpv6DstAdd[0] = 0xff;
  aucIpv6DstAdd[1] = 0x02;
  aucIpv6DstAdd[12] = 0x00;
  aucIpv6DstAdd[13] = 0x01;
  aucIpv6DstAdd[14] = 0x00;
  aucIpv6DstAdd[15] = 0x02;

  //cpss_mem_memcpy(aucIpv6DstAdd,gaucStaGwIpV6Addr,MT_IPV6_ADDR_LEN);

  //DHCPV6
  MT_SetField32(stDhcpv6Req.ulMsgTypeTransId,0,8,3);//Message Type:Request(3)
  MT_SetField32(stDhcpv6Req.ulMsgTypeTransId,8,16,(UINT16)(ulApIdx + 1));//Transaction ID:High 2 Bytes
  MT_SetField32(stDhcpv6Req.ulMsgTypeTransId,24,8,(UINT8)(ulStaIndex + 1));//Transaction ID:Low 1 Bytes
  stDhcpv6Req.ulMsgTypeTransId = htonl(stDhcpv6Req.ulMsgTypeTransId);

  //Client Identifier
  usDuidType = 3;//Link Lay addr plus time(1);Link Lay addr(3)
  usHWType = 6;//IEEE 802(6)
  ulTime = cpss_gmt_get();

  stClientIdentifier.usOptionID = 1;

  if(1 == usDuidType)
     {
	  ulTmFlag = 1;
	  stClientIdentifier.usLen = sizeof(usDuidType) + sizeof(usHWType) + sizeof(ulTime) + MT_MAC_LENTH;
     }
  else if(3 == usDuidType)
     {
	  ulTmFlag = 0;
	  stClientIdentifier.usLen = sizeof(usDuidType) + sizeof(usHWType) + MT_MAC_LENTH;
     }

  stClientIdentifier.usOptionID = htons(stClientIdentifier.usOptionID);
  usDuidType = htons(usDuidType);//Link Lay addr plus time(1)
  usHWType = htons(usHWType);//IEEE 802(6)
  ulTime = htonl(ulTime);

  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(aucLLAddr,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }

  ulSuffix = 0;
  cpss_mem_memcpy(&stClientIdentifier.aucVal[ulSuffix],&usDuidType,sizeof(usDuidType));
  ulSuffix = ulSuffix + sizeof(usDuidType);
  cpss_mem_memcpy(&stClientIdentifier.aucVal[ulSuffix],&usHWType,sizeof(usHWType));
  ulSuffix = ulSuffix + sizeof(usHWType);
  if(1 == ulTmFlag)
     {
	   cpss_mem_memcpy(&stClientIdentifier.aucVal[ulSuffix],&ulTime,sizeof(ulTime));
	   ulSuffix = ulSuffix + sizeof(ulTime);
      }
  cpss_mem_memcpy(&stClientIdentifier.aucVal[ulSuffix],aucLLAddr,MT_MAC_LENTH);
  ulClntIdntfLen = sizeof(stClientIdentifier.usOptionID) + sizeof(stClientIdentifier.usLen) + stClientIdentifier.usLen;
  stClientIdentifier.usLen = htons(stClientIdentifier.usLen);

  //Identity Association for non_tempory Address
  stIdentityAssociation.usOptionID = htons(3);
  ulIAID = htonl(0x01);
  ulT1 = htonl(0xffffffff);//infinity
  ulT2 = htonl(0xffffffff);//infinity
  stIaAddress.usOptionID = htons(5);//IA Address
  stIaAddress.usLen = MT_IPV6_ADDR_LEN + sizeof(ulPrefLifetime) + sizeof(ulValidLifetime);
  ulIaAddLen = sizeof(stIaAddress.usOptionID) + sizeof(stIaAddress.usLen) + stIaAddress.usLen;
  stIaAddress.usLen = htons(stIaAddress.usLen);

  ulPrefLifetime = htonl(2250);
  ulValidLifetime = htonl(3600);

  ulSuffix = 0;
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(&stIaAddress.aucVal[ulSuffix],gastStaPara[ulStaIndex].aucStaIpV6Addr,MT_IPV6_ADDR_LEN);
  }
  ulSuffix = ulSuffix + MT_IPV6_ADDR_LEN;
  cpss_mem_memcpy(&stIaAddress.aucVal[ulSuffix],&ulPrefLifetime,sizeof(ulPrefLifetime));
  ulSuffix = ulSuffix + sizeof(ulPrefLifetime);
  cpss_mem_memcpy(&stIaAddress.aucVal[ulSuffix],&ulValidLifetime,sizeof(ulValidLifetime));

  stIdentityAssociation.usLen = sizeof(ulIAID) + sizeof(ulT1) + sizeof(ulT2) + ulIaAddLen;
  ulIdentityAssocLen = sizeof(stIdentityAssociation.usOptionID) + sizeof(stIdentityAssociation.usLen) + stIdentityAssociation.usLen;
  stIdentityAssociation.usLen = htons(stIdentityAssociation.usLen);

  ulSuffix = 0;
  cpss_mem_memcpy(&stIdentityAssociation.aucVal[ulSuffix],&ulIAID,sizeof(ulIAID));
  ulSuffix = ulSuffix + sizeof(ulIAID);
  cpss_mem_memcpy(&stIdentityAssociation.aucVal[ulSuffix],&ulT1,sizeof(ulT1));
  ulSuffix = ulSuffix + sizeof(ulT1);
  cpss_mem_memcpy(&stIdentityAssociation.aucVal[ulSuffix],&ulT2,sizeof(ulT2));
  ulSuffix = ulSuffix + sizeof(ulT2);
  cpss_mem_memcpy(&stIdentityAssociation.aucVal[ulSuffix],&stIaAddress,ulIaAddLen);

  //Elapsed Time
  stElapedTime.usOptionID = htons(8);
  usElapsedTime = htons(0x012c);
  stElapedTime.usLen = sizeof(usElapsedTime);
  cpss_mem_memcpy(&stElapedTime.aucVal[0],&usElapsedTime,stElapedTime.usLen);
  ulElapsdTmLen = sizeof(stElapedTime.usOptionID) + sizeof(stElapedTime.usLen) + stElapedTime.usLen;
  stElapedTime.usLen = htons(stElapedTime.usLen);

  //Server Identifier
  usDuidType = 1;//link-layer address plus time(1)
  usHWType = 1;//Ethernet(1)
  ulTime = cpss_gmt_get();

  stSrvIndentifier.usOptionID = 2;//Server Identifier

  if(1 == usDuidType)
     {
	  ulTmFlag = 1;
	  stSrvIndentifier.usLen = sizeof(usDuidType) + sizeof(usHWType) + sizeof(ulTime) + MT_MAC_LENTH;
      }
  else if(3 == usDuidType)
     {
	  ulTmFlag = 0;
	  stSrvIndentifier.usLen = sizeof(usDuidType) + sizeof(usHWType) + MT_MAC_LENTH;
     }
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(aucLLAddr,gastStaPara[ulStaIndex].auGwMac,MT_MAC_LENTH);//DHCPv6 Server Mac
  }

  usDuidType = htons(usDuidType);
  usHWType = htons(usHWType);
  ulTime = htonl(ulTime);

  stSrvIndentifier.usOptionID = htons(stSrvIndentifier.usOptionID);
  stSrvIndentifier.usLen = htons(stSrvIndentifier.usLen);

  ulSuffix = 0;
  cpss_mem_memcpy(&stSrvIndentifier.aucVal[ulSuffix],&usDuidType,sizeof(usDuidType));
  ulSuffix = ulSuffix + sizeof(usDuidType);
  cpss_mem_memcpy(&stSrvIndentifier.aucVal[ulSuffix],&usHWType,sizeof(usHWType));
  ulSuffix = ulSuffix + sizeof(usHWType);
  if(1 == ulTmFlag)
     {
	  cpss_mem_memcpy(&stSrvIndentifier.aucVal[ulSuffix],&ulTime,sizeof(ulTime));
	  ulSuffix = ulSuffix + sizeof(ulTime);
     }
  cpss_mem_memcpy(&stSrvIndentifier.aucVal[ulSuffix],aucLLAddr,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  ulSrvIdentLen = sizeof(stSrvIndentifier.usOptionID) + sizeof(stSrvIndentifier.usLen) + ulSuffix;

  //construct dhcpv6-Request
  ulSuffix = 0;
  cpss_mem_memcpy(&stDhcpv6Req.aucDhcpv6OptionVal[ulSuffix],&stClientIdentifier,ulClntIdntfLen);
  ulSuffix = ulSuffix + ulClntIdntfLen;
  cpss_mem_memcpy(&stDhcpv6Req.aucDhcpv6OptionVal[ulSuffix],&stIdentityAssociation,ulIdentityAssocLen);
  ulSuffix = ulSuffix + ulIdentityAssocLen;
  cpss_mem_memcpy(&stDhcpv6Req.aucDhcpv6OptionVal[ulSuffix],&stElapedTime,ulElapsdTmLen);
  ulSuffix = ulSuffix + ulElapsdTmLen;
  cpss_mem_memcpy(&stDhcpv6Req.aucDhcpv6OptionVal[ulSuffix],&stSrvIndentifier,ulSrvIdentLen);
  ulSuffix = ulSuffix + ulSrvIdentLen;
  ulDhcpv6ReqLen = sizeof(stDhcpv6Req.ulMsgTypeTransId) + ulSuffix;

  //UDP
  stUdpHdr.usSrcPort = htons(546);//dhcpv6-client
  stUdpHdr.usDstPort = htons(547);//dhcpv6-server
  stUdpHdr.usPldLen = sizeof(stUdpHdr) + ulDhcpv6ReqLen;
  stUdpHdr.usPldLen = htons(stUdpHdr.usPldLen);
  stUdpHdr.usChkSum = 0;

  //IPv6
  MT_SetField32(stIpv6Hdr.ulIPVerTraffic,0,4,6);//Ip version
  MT_SetField32(stIpv6Hdr.ulIPVerTraffic,4,8,0);//Traffic Class
  MT_SetField32(stIpv6Hdr.ulIPVerTraffic,12,20,0);//Flow Label
  stIpv6Hdr.ulIPVerTraffic = htonl(stIpv6Hdr.ulIPVerTraffic);
  stIpv6Hdr.usPayloadLen = sizeof(stUdpHdr) + ulDhcpv6ReqLen;//Payload
  stIpv6Hdr.usPayloadLen = htons(stIpv6Hdr.usPayloadLen);
  stIpv6Hdr.ucNextHdr = 0x11;//UDP
  stIpv6Hdr.ucHopLmt = 128;
  cpss_mem_memcpy(stIpv6Hdr.aucSrcIpV6addr,aucIpv6SrcAdd,MT_IPV6_ADDR_LEN);
  cpss_mem_memcpy(stIpv6Hdr.aucDstIpV6Addr,aucIpv6DstAdd,MT_IPV6_ADDR_LEN);

  //Vlan
  MT_SetField32(stVlanHdr.usPriority,0,3,0);//Priority
  MT_SetField32(stVlanHdr.usPriority,3,1,0);//CFI
  MT_SetField32(stVlanHdr.usPriority,4,12,gastApconf[ulApIdx].ulStaVlan);//Vlan Id
  stVlanHdr.usPriority = htons(stVlanHdr.usPriority);
  stVlanHdr.usType = htons(0x86dd);//IPv6

  //Ethernet
  cpss_mem_memcpy(stEthHdr.aucDstMac,aucIpv6McastAddr,MT_MAC_LENTH);
  if(ulApIdx == getApIDbyBssid(ulStaIndex))
  {
	  cpss_mem_memcpy(stEthHdr.aucSrcMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
  }

  if(1 == gstStaConfInfo.ucVlanFlag)
    {
    stEthHdr.usProtclType = htons(0x8100);//VLAN
    }
  else
    {
    stEthHdr.usProtclType = htons(0x86dd);//IP
    }

  //CAPWAP HEADER
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,0);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,0);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,0);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,1);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

  //Fragment ID
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
  //Fragment Offset
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
  //Reserved
  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

  ucMacLen = MT_MAC_LENTH;

  MT_GetBssID(ulApIdx,ucRadioId,ucWlanId,aucBssId);
  ucRcvd = 0;

  //construct capwap body
  ulSuffix = 0;
  cpss_mem_memcpy(&aucDhcpv6Req[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
  cpss_mem_memcpy(&aucDhcpv6Req[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
  ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
  cpss_mem_memcpy(&aucDhcpv6Req[ulSuffix],&ucMacLen,sizeof(ucMacLen));
  ulSuffix = ulSuffix + sizeof(ucMacLen);
  cpss_mem_memcpy(&aucDhcpv6Req[ulSuffix],aucBssId,MT_MAC_LENTH);
  ulSuffix = ulSuffix + MT_MAC_LENTH;
  cpss_mem_memcpy(&aucDhcpv6Req[ulSuffix],&ucRcvd,sizeof(ucRcvd));
  ulSuffix = ulSuffix + sizeof(ucRcvd);

  cpss_mem_memcpy(&aucDhcpv6Req[ulSuffix],&stEthHdr,sizeof(stEthHdr));
  ulSuffix = ulSuffix + sizeof(stEthHdr);
  if(1 == gstStaConfInfo.ucVlanFlag)
     {
	  cpss_mem_memcpy(&aucDhcpv6Req[ulSuffix],&stVlanHdr,sizeof(stVlanHdr));
	  ulSuffix = ulSuffix + sizeof(stVlanHdr);
     }

  cpss_mem_memcpy(&aucDhcpv6Req[ulSuffix],&stIpv6Hdr,sizeof(stIpv6Hdr));
  ulSuffix = ulSuffix + sizeof(stIpv6Hdr);

  cpss_mem_memcpy(&aucDhcpv6Req[ulSuffix],&stUdpHdr,sizeof(stUdpHdr));
  ulSuffix = ulSuffix + sizeof(stUdpHdr);

  cpss_mem_memcpy(&aucDhcpv6Req[ulSuffix],&stDhcpv6Req,ulDhcpv6ReqLen);
  ulSuffix = ulSuffix + ulDhcpv6ReqLen;

  gstDhcpv6Req.ulBufLen = ulSuffix;
  cpss_mem_memcpy(gstDhcpv6Req.aucBuff,aucDhcpv6Req,gstDhcpv6Req.ulBufLen);

  bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gstDhcpv6Req.aucBuff, gstDhcpv6Req.ulBufLen);

  if(MT_FALSE == bSendResult)
    {
   printf("Error.MT_ApSendMsg2UPM for ApMutiTestSendDhcpv6Req is failure.\nFile: %s,Line: %d\n",__FILE__,__LINE__);
    }

  return bSendResult;
}

//用Association Flood攻击AC
void assocflood(void)
{
	MTBool bSendResult = MT_FALSE;
	MT_STA_WLAN_INFO_T stStaWLanInfo;
 UINT32 ulApIndx = 0;
 UINT32 ulStaIndx = 0;
 UINT8 ucRadioId = 0;
 UINT8 ucWlanNum = 0;
 UINT8 ucStaWlanIndex = 0;
 UINT32 ulMsgCnt = 0;
#define ASSOCIATION_FLOOD_MSG_CNT_PERSTA 110
 UINT32 ulLoop = 0;

 cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));

 printf("\nJust for Association Flood Test.\n");
 for(ulApIndx = 0; ulApIndx < gulCmdStartApNum; ulApIndx++)
   {
   if(RUN_STATE == gastAp[ulApIndx].enState)
        {
     for(ulStaIndx = 0; ulStaIndx < MT_STA_MAX_NUM_PER_WTP; ulStaIndx++)
             {
       if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndx].enAuthState && ulApIndx == getApIDbyBssid(ulStaIndx))
                   {
         for(ulLoop = 0; ulLoop < ASSOCIATION_FLOOD_MSG_CNT_PERSTA; ulLoop++)
                        {
        	   stStaWLanInfo.ulApIndex = ulApIndx;
        	   stStaWLanInfo.ulStaIndex = ulStaIndx;
        	   ucRadioId = gastApWlanInfo[ulApIndx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
        	   stStaWLanInfo.ucRadioId = ucRadioId;
        	   ucWlanNum = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].ucWlanNum;
        	   ucStaWlanIndex = ulStaIndx % ucWlanNum;//用户需要接入的WLAN标识
        	   stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

        	   bSendResult = ApMutiTestSendAssocReq(stStaWLanInfo);
        	   if(MT_TRUE == bSendResult)
        	                    {
        		   gstMutiTest_Statics.ulSend80211AssocReq++;
        	                    }
        	   else
        	                    {
        		   gstErrMsgStatics.ulSend80211AssocReqErr++;
        	                    }
        	   ulMsgCnt++;
                        }
                   }
             }
       }
   }
 printf("Association MSG Count: %u\n",ulMsgCnt);
}

//用Disassociation Flood 攻击AC
void disassocflood(void)
{
	MTBool bSendResult = MT_FALSE;
	MT_STA_WLAN_INFO_T stStaWLanInfo;
 UINT32 ulApIndx = 0;
 UINT32 ulStaIndx = 0;
 UINT8 ucRadioId = 0;
 UINT8 ucWlanNum = 0;
 UINT8 ucStaWlanIndex = 0;
 UINT32 ulMsgCnt = 0;
#define DISASSOCIATION_FLOOD_MSG_CNT_PERSTA 110
 UINT32 ulLoop = 0;

 cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));

 printf("\nJust for Disassociation Flood Test.\n");
 for(ulApIndx = 0; ulApIndx < gulCmdStartApNum; ulApIndx++)
   {
   if(RUN_STATE == gastAp[ulApIndx].enState)
        {
     for(ulStaIndx = 0; ulStaIndx < MT_STA_MAX_NUM_PER_WTP; ulStaIndx++)
             {
#if 0
    	 for(ulLoop = 0; ulLoop < DISASSOCIATION_FLOOD_MSG_CNT_PERSTA; ulLoop++)
               {
    	   bSendResult = ApMutiTestSend80211DisAssocReq(ulApIndx, ulStaIndx);
    	   if(MT_TRUE == bSendResult)
    	            {
    		              ;
    	            }
    	   else
    	            {
    		   ;
  		             }
    	   ulMsgCnt++;
               }
#else
       if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndx].enAuthState && ulApIndx == getApIDbyBssid(ulStaIndx))
                   {
         for(ulLoop = 0; ulLoop < DISASSOCIATION_FLOOD_MSG_CNT_PERSTA; ulLoop++)
                        {
        	  stStaWLanInfo.ulApIndex = ulApIndx;
        	  stStaWLanInfo.ulStaIndex = ulStaIndx;
        	  ucRadioId = gastApWlanInfo[ulApIndx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
        	  stStaWLanInfo.ucRadioId = ucRadioId;
           ucWlanNum = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].ucWlanNum;

           if(0 == ucWlanNum)
                             {
            	break;
                             }

           ucStaWlanIndex = ulStaIndx % ucWlanNum;//用户需要接入的WLAN标识
           stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];;

           bSendResult = ApMutiTestSend80211DisAssocReq(stStaWLanInfo);
           if(MT_TRUE == bSendResult)
                             {
  			                          ;
  		                     }
           else
  		                     {
        	                       ;
  		                      }
           ulMsgCnt++;
                        }
                   }
#endif
             }
       }
   }
 printf("Disassociation MSG Count: %u\n",ulMsgCnt);
}

//去认证测试
void disassoctest(UINT32 vgulDeAssoTestFlag)
{
	MTBool bSendResult = MT_FALSE;
	MT_STA_WLAN_INFO_T stStaWLanInfo;
 UINT32 ulApIndx = 0;
 UINT32 ulStaIndx = 0;
 UINT8 ucRadioId = 0;
 UINT8 ucWlanNum = 0;
 UINT8 ucStaWlanIndex = 0;
 UINT32 ulMsgCnt = 0;
#define DISASSOCIATION_TEST_MSG_CNT_PERSTA 1
 UINT32 ulLoop = 0;

 cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
 gulDeAssoTestFlag = vgulDeAssoTestFlag;

 printf("\nJust for Disassociation test Test.\n");

 for(ulApIndx = 0; ulApIndx < gulCmdStartApNum; ulApIndx++)
   {
   if(RUN_STATE == gastAp[ulApIndx].enState)
        {
//     for(ulStaIndx = 0; ulStaIndx < MT_STA_MAX_NUM_PER_WTP; ulStaIndx++)
	   for(ulStaIndx = 0; ulStaIndx < 10; ulStaIndx++)
             {
//       if(ASSOCIATION_SUCESS_STATE == gastAp[ulApIndx].astSta[ulStaIndx].enAuthState)
//                   {
         for(ulLoop = 0; ulLoop < DISASSOCIATION_TEST_MSG_CNT_PERSTA; ulLoop++)
                        {
        	  stStaWLanInfo.ulApIndex = ulApIndx;
        	  stStaWLanInfo.ulStaIndex = ulStaIndx;
        	  ucRadioId = gastApWlanInfo[ulApIndx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
        	  stStaWLanInfo.ucRadioId = ucRadioId;
           ucWlanNum = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].ucWlanNum;

           if(0 == ucWlanNum)
                             {
            	break;
                             }

           ucStaWlanIndex = ulStaIndx % ucWlanNum;//用户需要接入的WLAN标识
           stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];;

           bSendResult = ApMutiTestSend80211DisAssocReq(stStaWLanInfo);
           if(MT_TRUE == bSendResult)
  		                     {
  			                         ;
  		                     }
           else
  		                     {
  			                         ;
  		                     }
           ulMsgCnt++;
                        }
//                   }
             }
       }
   }
 printf("Disassociation MSG Count: %u\n",ulMsgCnt);
}

//用De-authentication Flood攻击AC
void deauthflood(void)
{
	MTBool bSendResult = MT_FALSE;
 MT_STA_AUTH_INFO stStaAuthInfo;
 UINT32 ulApIndx = 0;
 UINT32 ulStaIndx = 0;
 UINT8 ucRadioId = 0;
 UINT8 ucWlanNum = 0;
 UINT8 ucStaWlanIndex = 0;
 UINT32 ulMsgCnt = 0;
#define DE_AUTHENTICATION_FLOOD_MSG_CNT_PERSTA 110
 UINT32 ulLoop = 0;

 cpss_mem_memset(&stStaAuthInfo,0,sizeof(stStaAuthInfo));

 printf("\nJust for De-authentication Flood Test.\n");
 for(ulApIndx = 0; ulApIndx < gulCmdStartApNum; ulApIndx++)
   {
   if(RUN_STATE == gastAp[ulApIndx].enState)
        {
     for(ulStaIndx = 0; ulStaIndx < MT_STA_MAX_NUM_PER_WTP; ulStaIndx++)
             {
#if 0
    	 for(ulLoop = 0; ulLoop < DE_AUTHENTICATION_FLOOD_MSG_CNT_PERSTA; ulLoop++)
               {
    	   bSendResult = ApMutiTestSend80211DeAuthReq(ulApIndx, ulStaIndx);
    	   if(MT_TRUE == bSendResult)
    	            {
    		   gstMutiTest_Statics.ulSendDeAuth++;
    	            }
    	   else
    	            {
    		   gstErrMsgStatics.ulSendDeAuthErr++;
    	            }
    	   staStateInit(ulStaIndx);
    	   ulMsgCnt++;
                   }
#else
       if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndx].enAuthState && ulApIndx == getApIDbyBssid(ulStaIndx))
                   {
         for(ulLoop = 0; ulLoop < DE_AUTHENTICATION_FLOOD_MSG_CNT_PERSTA; ulLoop++)
                        {
        	  ucRadioId = gastApWlanInfo[ulApIndx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况
        	  stStaAuthInfo.ulApIndex = ulApIndx;
        	  stStaAuthInfo.ulStaIndex = ulStaIndx;
        	  stStaAuthInfo.ucRadioId = ucRadioId;
        	  ucWlanNum = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].ucWlanNum;
        	  if(0 == ucWlanNum)
            	             {
             break;
                   	         }

        	  ucStaWlanIndex = ulStaIndx % ucWlanNum;
        	  stStaAuthInfo.ucWlanId = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
        	  stStaAuthInfo.usAuthAlgrm = (UINT16)gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].astWlanInfo[ucStaWlanIndex].ucAuthType;

        	  bSendResult = ApMutiTestSend80211DeAuthReq(stStaAuthInfo);
        	  if(MT_TRUE == bSendResult)
        	                 {
        		  gstMutiTest_Statics.ulSendDeAuth++;
        	                 }
        	  else
        	                 {
        		  gstErrMsgStatics.ulSendDeAuthErr++;
        	                 }
        	  staStateInit(ulStaIndx);
           ulMsgCnt++;
                        }
                   }
#endif
             }
       }
   }
 printf("De-authentication MSG Count: %u\n",ulMsgCnt);
}

//DeAuth测试
void deauthtest(UINT32 vgulDeAuthTestFlag)
{
	MTBool bSendResult = MT_FALSE;
 MT_STA_AUTH_INFO stStaAuthInfo;
 UINT32 ulApIndx = 0;
 UINT32 ulStaIndx = 0;
 UINT8 ucRadioId = 0;
 UINT8 ucWlanNum = 0;
 UINT8 ucStaWlanIndex = 0;
 UINT32 ulMsgCnt = 0;
#define DE_AUTHENTICATION_TEST_MSG_CNT_PERSTA 5

 cpss_mem_memset(&stStaAuthInfo,0,sizeof(stStaAuthInfo));

 printf("\nJust for De-authentication Test.\n");

 gulDeAuthTestFlag = vgulDeAuthTestFlag;

 for(ulApIndx = 0; ulApIndx < gulCmdStartApNum; ulApIndx++)
   {
   if(RUN_STATE == gastAp[ulApIndx].enState)
        {
	   for(ulStaIndx = 0; ulStaIndx < gulStaNumPerAp + DE_AUTHENTICATION_TEST_MSG_CNT_PERSTA; ulStaIndx++)
	        {
		   ucRadioId = gastApWlanInfo[ulApIndx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况
		   stStaAuthInfo.ulApIndex = ulApIndx;
		   stStaAuthInfo.ulStaIndex = ulStaIndx;
		   stStaAuthInfo.ucRadioId = ucRadioId;
		   ucWlanNum = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].ucWlanNum;

		   if(0 == ucWlanNum)
		        {
			   break;
		        }

		   ucStaWlanIndex = ulStaIndx % ucWlanNum;
		   stStaAuthInfo.ucWlanId = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
		   stStaAuthInfo.usAuthAlgrm = (UINT16)gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].astWlanInfo[ucStaWlanIndex].ucAuthType;

		   bSendResult = ApMutiTestSend80211DeAuthReq(stStaAuthInfo);
		   if(MT_TRUE == bSendResult)
		        {
			   gstMutiTest_Statics.ulSendDeAuth++;
		        }
		   else
		       {
			   gstErrMsgStatics.ulSendDeAuthErr++;
		       }
		   staStateInit(ulStaIndx);
		   ulMsgCnt++;
	        }
       }
   }
 printf("De-authentication MSG Count: %u\n",ulMsgCnt);
}

//向AC发送不存在的用户的Deauth消息
void deAuthForNonExistUser(UINT32 vulApIndx)
{
	MTBool bSendResult = MT_FALSE;
 MT_STA_AUTH_INFO stStaAuthInfo;
 UINT32 ulApIndx = 0;
 UINT32 ulStaIndx = 0;
 UINT8 ucRadioId = 0;
 UINT8 ucWlanNum = 0;
 UINT8 ucStaWlanIndex = 0;
 UINT32 ulMsgCnt = 0;
#define DE_AUTHENTICATION_TEST_MSG_CNT_PERSTA 5

 cpss_mem_memset(&stStaAuthInfo,0,sizeof(stStaAuthInfo));

 gulDeAuthTestFlag = 1;

 ulApIndx = vulApIndx;

 if(RUN_STATE == gastAp[ulApIndx].enState)
   {
	 for(ulStaIndx = gulStaNumPerAp; ulStaIndx < gulStaNumPerAp + DE_AUTHENTICATION_TEST_MSG_CNT_PERSTA; ulStaIndx++)
	   {
		 ucRadioId = gastApWlanInfo[ulApIndx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况
		 stStaAuthInfo.ulApIndex = ulApIndx;
		 stStaAuthInfo.ulStaIndex = ulStaIndx;
		 stStaAuthInfo.ucRadioId = ucRadioId;
		 ucWlanNum = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].ucWlanNum;

		 if(0 == ucWlanNum)
		   {
			 break;
		    }

		 ucStaWlanIndex = ulStaIndx % ucWlanNum;
		 stStaAuthInfo.ucWlanId = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
		 stStaAuthInfo.usAuthAlgrm = (UINT16)gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].astWlanInfo[ucStaWlanIndex].ucAuthType;

		 bSendResult = ApMutiTestSend80211DeAuthReq(stStaAuthInfo);

		 if(MT_TRUE == bSendResult)
		  {
			 gstMutiTest_Statics.ulSendDeAuth++;
			 staStateInit(ulStaIndx);
			 ulMsgCnt++;
		  }
		 else
		  {
			 gstErrMsgStatics.ulSendDeAuthErr++;
		  }
	   }
   }
}


//Auth测试
void authtest(UINT32 vgulAuthTestFlag)
{
	MTBool bSendResult = MT_FALSE;
 MT_STA_AUTH_INFO stStaAuthInfo;
 UINT32 ulApIndx = 0;
 UINT32 ulStaIndx = 0;
 UINT8 ucRadioId = 0;
 UINT8 ucWlanNum = 0;
 UINT8 ucStaWlanIndex = 0;
 UINT32 ulMsgCnt = 0;
#define AUTHENTICATION_TEST_MSG_CNT_PERSTA 1
 UINT32 ulLoop = 0;

 cpss_mem_memset(&stStaAuthInfo,0,sizeof(stStaAuthInfo));

 printf("\nJust for Authentication Test.\n");

 gulAuthTestFlag = vgulAuthTestFlag;

 for(ulApIndx = 0; ulApIndx < gulCmdStartApNum; ulApIndx++)
   {
   if(RUN_STATE == gastAp[ulApIndx].enState)
        {
	   for(ulStaIndx = 0; ulStaIndx < 1; ulStaIndx++)
             {
         for(ulLoop = 0; ulLoop < AUTHENTICATION_TEST_MSG_CNT_PERSTA; ulLoop++)
                        {
        	  ucRadioId = gastApWlanInfo[ulApIndx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况
        	  stStaAuthInfo.ulApIndex = ulApIndx;
        	  stStaAuthInfo.ulStaIndex = ulStaIndx;
        	  stStaAuthInfo.ucRadioId = ucRadioId;
        	  ucWlanNum = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].ucWlanNum;
        	  if(0 == ucWlanNum)
            	             {
             break;
                   	         }

        	  ucStaWlanIndex = ulStaIndx % ucWlanNum;
        	  stStaAuthInfo.ucWlanId = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
        	  stStaAuthInfo.usAuthAlgrm = (UINT16)gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].astWlanInfo[ucStaWlanIndex].ucAuthType;

        	  bSendResult = ApMutiTestSend80211AuthReq(stStaAuthInfo);
        	  if(MT_FALSE == bSendResult)
        	                 {
        		  gstMutiTest_Statics.ulSend80211AuthReq++;
        	                 }
        	  else
        	                 {
        		  gstErrMsgStatics.ulSend80211AuthReqErr++;
        	                 }
           ulMsgCnt++;
                        }
             }
       }
   }
 printf("Authentication MSG Count: %u\n",ulMsgCnt);
}

//用Authentication Flood攻击AC
void authflood(void)
{
	MTBool bSendResult = MT_FALSE;
 UINT32 ulApIndx = 0;
 UINT32 ulStaIndx = 0;
 UINT8 ucRadioId = 0;
 UINT8 ucWlanId = 0;
 UINT8 ucWlanNum = 0;
 UINT8 ucStaWlanIndex = 0;
 UINT32 ulMsgCnt = 0;
#define AUTHENTICATION_FLOOD_MSG_CNT_PERSTA 110
 UINT32 ulLoop = 0;
 MT_STA_AUTH_INFO stStaAuthInfo;

 cpss_mem_memset(&stStaAuthInfo,0,sizeof(stStaAuthInfo));

 printf("\nJust for Authentication Flood Test.\n");
 for(ulApIndx = 0; ulApIndx < gulCmdStartApNum; ulApIndx++)
   {
   if(RUN_STATE == gastAp[ulApIndx].enState)
        {
     for(ulStaIndx = 0; ulStaIndx < MT_STA_MAX_NUM_PER_WTP; ulStaIndx++)
             {
       if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndx].enAuthState && ulApIndx == getApIDbyBssid(ulStaIndx))
                   {
         for(ulLoop = 0; ulLoop < AUTHENTICATION_FLOOD_MSG_CNT_PERSTA; ulLoop++)
                        {
        	 stStaAuthInfo.ulApIndex = ulApIndx;
        	 stStaAuthInfo.ulStaIndex = ulStaIndx;
        	 ucRadioId = gastApWlanInfo[ulApIndx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况
     	    stStaAuthInfo.ucRadioId = ucRadioId;
        	 ucWlanNum = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].ucWlanNum;
        	 if(0 == ucWlanNum)
        	              {
        		 break;
        	              }
     	    ucStaWlanIndex = ulStaIndx % ucWlanNum;//用户需要接入的WLAN标识
     	    ucWlanId = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
     	    stStaAuthInfo.ucWlanId = ucWlanId;
     	    stStaAuthInfo.usAuthAlgrm = (UINT16)gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].astWlanInfo[ucWlanId].ucAuthType;
     	    stStaAuthInfo.ucChallengeTxtLen = 0;

     	    bSendResult = ApMutiTestSend80211AuthReq(stStaAuthInfo);
      	  if(MT_FALSE == bSendResult)
      	                 {
      		  gstMutiTest_Statics.ulSend80211AuthReq++;
      	                 }
      	  else
      	                 {
      		  gstErrMsgStatics.ulSend80211AuthReqErr++;
      	                 }
        	 ulMsgCnt++;
                        }
                   }
             }
       }
   }
 printf("Authentication MSG Count: %u\n",ulMsgCnt);
}

//DHCP Flood
void dhcpflood(void)
{
	MTBool bSendResult = MT_FALSE;
	MT_STA_WLAN_INFO_T stStaWLanInfo;
  UINT32 ulApIndx = 0;
  UINT32 ulStaIndx = 0;
  UINT8 ucRadioId = 0;
  UINT8 ucWlanNum = 0;
  UINT8 ucStaWlanIndex = 0;
  UINT32 ulMsgCnt = 0;
#define DHCP_FLOOD_MSG_CNT_PERSTA 110
  UINT32 ulLoop = 0;

  cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));

  printf("\nJust for DHCP Flood Test.\n");
  for(ulApIndx = 0; ulApIndx < gulCmdStartApNum; ulApIndx++)
    {
    if(RUN_STATE == gastAp[ulApIndx].enState)
         {
      for(ulStaIndx = 0; ulStaIndx < MT_STA_MAX_NUM_PER_WTP; ulStaIndx++)
              {
#if 1
    	  if(IP_ADDR_TYPE_V4 == gulIpAddrFlag)
    	         {
        for(ulLoop = 0; ulLoop < DHCP_FLOOD_MSG_CNT_PERSTA; ulLoop++)
                     {
        	stStaWLanInfo.ulApIndex = ulApIndx;
        	stStaWLanInfo.ulStaIndex = ulStaIndx;
          ucRadioId = gastApWlanInfo[ulApIndx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
          stStaWLanInfo.ucRadioId = ucRadioId;
          ucWlanNum = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].ucWlanNum;
          if(0 == ucWlanNum)
                           {
           	break;
                           }

          ucStaWlanIndex = ulStaIndx % ucWlanNum;//用户需要接入的WLAN标识
          stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

          bSendResult = ApMutiTestSendDhcpDiscover(stStaWLanInfo);
          if(MT_FALSE == bSendResult)
    	                  {
        	  gstMutiTest_Statics.ulStaSendDhcpReq++;
    	                 }
          else
    	                 {
        	  gstErrMsgStatics.ulSendDhcpReqErr++;
    	                 }
          ulMsgCnt++;
                     }
    	         }
    	  else
    	        {
    	    for(ulLoop = 0; ulLoop < DHCP_FLOOD_MSG_CNT_PERSTA; ulLoop++)
    	               {
    	    	   stStaWLanInfo.ulApIndex = ulApIndx;
    	    	   stStaWLanInfo.ulStaIndex = ulStaIndx;
    	        ucRadioId = gastApWlanInfo[ulApIndx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
    	        stStaWLanInfo.ucRadioId = ucRadioId;
    	        ucWlanNum = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].ucWlanNum;
    	        if(0 == ucWlanNum)
    	                         {
    	         	break;
    	                         }

    	        ucStaWlanIndex = ulStaIndx % ucWlanNum;//用户需要接入的WLAN标识
    	        stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIndx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
    	        bSendResult = ApMutiTestSendDhcpv6Solicit(stStaWLanInfo);
    	        if(MT_FALSE == bSendResult)
    	    	                 {
    	        	  gstMutiTest_Statics.ulStaSendDhcpv6Solicit++;
    	    	                 }
    	        else
    	    	                 {
    	        	  gstErrMsgStatics.ulSendDhcpv6SolicitErr++;
    	    	                 }
    	        ulMsgCnt++;
    	               }
    	        }
#else
      if(ASSOCIATION_SUCESS_STATE == gastAp[ulApIndx].astSta[ulStaIndx].enAuthState)
                {
      	  if(IP_ADDR_TYPE_V4 == gulIpAddrFlag)
      	                {
      		  for(ulLoop = 0; ulLoop < DHCP_FLOOD_MSG_CNT_PERSTA; ulLoop++)
      		                 {
      			  bSendResult = ApMutiTestSendDhcpDiscover(ulApIndx, ulStaIndx);
      			  if(MT_FALSE == bSendResult)
      			    	         {
      				  gstMutiTest_Statics.ulStaSendDhcpReq++;
      			    	         }
      			  else
      			                 {
      				  gstErrMsgStatics.ulSendDhcpReqErr++;
      			                 }
      			  ulMsgCnt++;
                             }
      	                 }
      	  else
      	                 {
      		  for(ulLoop = 0; ulLoop < DHCP_FLOOD_MSG_CNT_PERSTA; ulLoop++)
      	                     {
      	    	bSendResult = ApMutiTestSendDhcpv6Solicit(ulApIndx, ulStaIndx);
      	    	if(MT_FALSE == bSendResult)
      	    	                {
      	    		gstMutiTest_Statics.ulStaSendDhcpv6Solicit++;
      	    	                }
      	    	else
      	    	                {
      	    		gstErrMsgStatics.ulSendDhcpv6SolicitErr++;
      	    	                }
      	    	ulMsgCnt++;
      	                     }
                         }
                }
#endif
              }
        }
    }
  printf("DHCP Flood MSG Count: %u\n",ulMsgCnt);
}

/*STA Ping BGN*/
#if 0
INT32 datalen = 56;//data that goes witj ICMP echo request

//sig_alrm
void sig_alrm(INT32 signo)
{
  (*pr->fsend)();
  alarm(1);
  return;
}

//readloop函数
void readloop(void)
{
  INT32 size;
  UINT8 recvbuf[BUFSIZE];
  socklen_t len;
  ssize_t n;
  struct timeval tval;

  sockfd = Socket(pr->assend->sa_family,SOCK_RAW,pr->icmpproto);
  setuid(getuid());

  size = 60 * 1024;

  setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size));

  //sig_alrm(SIGALRM);

  for(;;)
     {
    len = pr->salen;
    n = recvfrom(sockfd,recvbuf,sizeof(recvbuf),0,pr->asrecv,&len);

    if(n < 0)
           {
       if(errno == EINTR)
                  {
         continue;
                  }
       else
                 {
        err_sys("recvfrom error");
                  }
            }
      Gettimeofday(&tval,NULL);
      (*pr->fproc)(recvbuf,n,&tval);
        }
}

//tv_sub函数用于将两timeval结构相减，并将结果存入第一个timeval结构中
void tv_sub(struct timeval *out, struct timeval *in)
{
  if((out->tv_usec -= in->tv_usec) < 0)
     {
    --out->tv_sec;
    out->tv_usec += 1000000;
     }

  out->tv_sec -= in->tv_sec;
}

/*in_cksum计算Internet校验和*/
UINT16 in_cksum(UINT16 *addr, UINT32 len)
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

/*函数sned_v4构造一个ICMPV4回射请求消息并写入原始套接口*/
VOID send_v4(VOID)
{
   INT32 ilLen = 0;
   struct icmp *icmp;

   icmp = (struct icmp *)sendbuf;
   icmp->icmp_type = ICMP_ECHO;
   icmp->icmp_code = 0;
   icmp->icmp_id = pid;
   icmp->icmp_seq = nsent++;

   Gettimeofday((struct timeval *)icmp->icmp_data,NULL);

   ilLen = 8 + datalen;
   icmp->icmp_cksum = 0;
   icmp->icmp_cksum = in_cksum((UINT16 *)icmp,ilLen);

   sendto(sockfd,sendbuf,ilLen,0,pr->assend,pr->salen);
}

/*Send_V6函数，构造并发送ICMPV6回射请求消息*/
VOID send_v6(VOID)
{
#ifdef IPv6
   INT32 ilLen = 0;
   struct icmp6_hdr *icmp6;

   icmp6 = (struct icmp6_hdr *)sendbuf;
   icmp6->icmp6_type = ICMP6_ECHO_REQUEST;
   icmp6->icmp6_code = 0;
   icmp6->id = pid;
   icmp6->icmp6_seq = nsent++;

   Gettimeofday((struct timeval *)(icmp + 1),NULL);

   ilLen = 8 + datalen;/* 8-Byte ICMP6 Header*/

   sento(sockfd,sendbuf,ilLen,0,pr->sasend,pr->salen);/*Kernel calculate and stores checksum for us*/
#endif /*IPv6*/
}

//ICMPv6消息由函数proc_v6处理
void proc_v6(UINT8 *ptr,ssize_t len,struct timeval * tvrecv)
{
#ifdef IPV6
   INT32 hlen1,icmp6len;
   double rtt;
   struct ip6_hdr *ip6;
   struct icmp6_hdr *icmp6;
   struct timeval *tvsend;

   ip6 = (struct ip6_hdr *)ptr;
   hlen1 = sizeof(struct ip6_hdr);

   if(ip6->ip6)next != IPPROTO_ICMPV6)
        {
     err_quit("Next header not IPPROTO_ICMPV6");
        }

   icmp6 = (struct icmp6_hdr *)(ptr + hlen1);

   if((icmp5len =len - hlen1) < 8)
        {
     err_quit("icmp5len (%d) < 8",icmp6len);
        }

   if(icmp6->icmp_type == ICMP6_ECHO_REPLY)
        {
     if(icmp6->icmp6_id != pid)
             {
       return;//not a response ro our ECHO_REQUEST
             }

     if(icmp6len < 16)
             {
       err_quit("icmp6len (%d) < 16",icmp6len);
             }

     tvsend = (struct timeval *)(icmp + 1);
     tv_sub(tvrecv,tvsend);
     rtt = tvrecv->tv_sec * 1000.0 + tvrecv->usec / 1000.0;

     printf("%d bytes from %s: seq = %u,hlim = %d,rtt= %.3f ms\n",
                       icmp6len,Sock_ntop_host(pr->sarecv,pr->salen),
                       icmp6->icmp6_seq,
                       ip6->ip6_hlim,
                       rtt);
        }
   else if(verbose)
        {
     printf("%d bytes from %s: type = %d, code = %d\n",
                        icmp6len,Sock_ntop_host(pr->sarecv,pr->salen),
                        icmp6->icmp6_type,
                        icmp6->icmp6_code);
        }
#endif //IPV6
}

//ICMPv4消息由函数proc_v4处理
void proc_v4(UINT8 *ptr,ssize_t len,struct timeval * tvrecv)
{
  INT32 hlen1,icmplen;
  double rtt;
  struct ip *ip;
  struct icmp *icmp;
  struct timeval *tvsend;

  ip = (struct ip *)ptr;// start of Ip header
  hlen1 = ip->ip_hl << 2;//length of IP header

  icmp = (struct icmp *)(ptr + hlen1);//start od ICMP header

  if((icmplen = len - hlen1) < 8)
     {
    err_quit("icmplen (%d) < 8",icmplen);
     }

  if(icmp->icmp_type == ICMP_ECHOREPLY)
     {
    if(icmp->icmp_id != pid)
           {
      return;//not a response ro our ECHO_REQUEST
           }

    if(icmplen < 16)
           {
      err_quit("icmplen (%d) < 16",icmplen);
           }

    tvsend = (struct timeval *) icmp->icmp_code;
    tv_sub(tvrecv,tvsend);
    rtt = tvrecv->tv_sec * 1000.0 + tvrecv->tv_usec / 1000.0;

    printf("%d bytes from %s: seq = %u,ttl = %d,rtt= %.3f ms\n",
                       icmplen,Sock_ntop_host(pr->asrecv,pr->salen),
                       icmp->icmp_seq,
                       ip->ip_ttl,
                       rtt);
       }
  else if(verbose)
     {
    printf("%d bytes from %s: type = %d, code = %d\n",
                       icmplen,Sock_ntop_host(pr->asrecv,pr->salen),
                       icmp->icmp_type,
                       icmp->icmp_code);
     }
}

struct proto proto_4 = {proc_v4,send_v4,NULL,NULL,0, IPPROTO_ICMP};
#ifdef IPV6
struct proto proto_6 = {proc_v6,send_v6,NULL,NULL,0, IPPROTO_ICMPV6};
#endif //IPV6

INT32 ping(int argc,UINT8 **argv)
{
  INT32 c;
  struct addrinfo *ai;

  opterr = 0;//don't want getopt() writing to stderr

  while((c = getopt(argc,argv,"v")) != -1)
     {
    switch(c)
           {
      case 'v':
        verbose++;
      break;

      case '?':
        err_quit("unrecognized option:$c",c);
                }
      }

  if(optind != argc - 1)
     {
    err_quit("usage: ping [-v]<hostname>");
     }

  host = argv[optind];

  pid = getpid();
  //Signal(SIGNALRM,sig_alrm);

  ai = Host_serv(host,NULL,0,0);

  printf("PING %s(%s): %d data bytes\n",
               ai->ai_canonname,
               Sock_ntop_host(ai->ai_addr,ai->addrlen),
               datalen);

  if(ai->ai_family == AF_INET)
     {
    pr = &proc_v4;
     }
#ifdef IPV6
  else if(ai->ai_family == AF_INET6)
     {
    pr = &proc_v6;

    if(IN6_IS_ADDR_V4MAPPED(&(((struct sockaddr_in6 *) ai->ai_addr)->sin6_addr)))
           {
      err_quit("cannot ping IPv4_mapped IPv6 address");
           }
     }
#endif
  else
     {
                err_quit("unknown address family %d",ai->ai_family);
     }

  pr->assend = ai->ai_addr;
  pr->asrecv = Calloc(1,ai->ai_addrlen);
  pr->salen = ai->ai_addrlen;

  readloop();

  exit(0);
}
/*STA Ping END*/
#endif

//切换过程
//Handover_Authenttication
MTBool ApMutiTestSend80211AuthReqForHandover(MT_STA_AUTH_INFO vstStaAuthInfo,UINT32 vulDstApIndex)
{
    MTBool bSendResult = MT_FALSE;
    UINT8 auc80211AuthReq[256] = {0};
    MT_80211AUTHENTICATION_T st80211AuthReq;
    MT_80211WLANMANAGMNTFRAME_FIXED_PARA_T st80211WlanMngmntFrmFixedPara;
    MT_80211WLANMANAGMNTFRAME_TAGGED_PARA_T stWlanMangmntFrmTaggedPara;
    UINT32 ulTaggedParaLen = 0;
    MT_80211WLANMANAGMNTFRAME_TAGGED_PARA_T stVendoeSpecific;
    UINT32 ulVendoeSpecificLen = 0;
    UINT8 aucOui[3] = {0};
    UINT8 ucVenOuiType = 0;
    UINT8 aucVenOuiVal[5] = {0};

    UINT8 aucBssId[MT_MAC_LENTH] = {0};
    UINT32 ulCapwapHdrFisrt32 = 0;
    UINT32 ulCapwapHdrSecond32 = 0;
    MT_CAPWAP_HDR_WD_T stCpHdrWdata;
    UINT16 usFrameCtrl = 0;
    UINT16 usDuration = 0;
    UINT16 usSeqCtrl = 0;

    UINT32 ulApIdx = 0;
    UINT8 ucRadioId = 0;
    UINT8 ucWlanId = 0;
    UINT32 ulStaIndex = 0;
    UINT16 usAuthAlgrm = 0;
    UINT16 usAuthSeq = 0;
    UINT32 ulSuffix = 0;
//    UINT32 ulLoop = 0;

    cpss_mem_memset(&st80211AuthReq,0,sizeof(st80211AuthReq));
    cpss_mem_memset(&st80211WlanMngmntFrmFixedPara,0,sizeof(st80211WlanMngmntFrmFixedPara));
    cpss_mem_memset(&stWlanMangmntFrmTaggedPara,0,sizeof(stWlanMangmntFrmTaggedPara));
    cpss_mem_memset(&stCpHdrWdata,0,sizeof(MT_CAPWAP_HDR_WD_T));
    cpss_mem_memset(&stVendoeSpecific,0,sizeof(stVendoeSpecific));

    ulApIdx = vstStaAuthInfo.ulApIndex;
    ulStaIndex = vstStaAuthInfo.ulStaIndex;
    usAuthAlgrm = vstStaAuthInfo.usAuthAlgrm;
    usAuthSeq = vstStaAuthInfo.usAuthSeq;

    ucRadioId = vstStaAuthInfo.ucRadioId;
    ucWlanId = vstStaAuthInfo.ucWlanId;
    MT_GetBssID(vulDstApIndex,ucRadioId,ucWlanId,aucBssId);

    if(MT_FALSE == gbTestMod)//PARACONF
          {
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,1);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,1);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,1);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,1);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,0);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
      MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

      ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

      //Fragment ID
      MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
      //Fragment Offset
      MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
      //Reserved
      MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

      ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

      //Wireless Data
      stCpHdrWdata.ucWirelessLen = 4;
      stCpHdrWdata.ucWDRssi = 0;//dBm
      stCpHdrWdata.ucWDSnr = 0;//dB
      stCpHdrWdata.usWDDataRate = 0;//Mbps

      st80211AuthReq.usFranCtrl = htons(0xB000);
      st80211AuthReq.usDuration = 314;
#if 0
      cpss_mem_memcpy(st80211AuthReq.aucDstMacAdd,gastAp[vulDstApIndex].auApMac,MT_MAC_LENTH);
#else
      cpss_mem_memcpy(st80211AuthReq.aucDstMacAdd,aucBssId,MT_MAC_LENTH);
#endif
      cpss_mem_memcpy(st80211AuthReq.aucSrcMAcAdd,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
      cpss_mem_memcpy(st80211AuthReq.aucBssid,aucBssId,MT_MAC_LENTH);

      switch(gulAuthTestFlag)
                {
      case 0://不变；
    	  break;

      case 1://MAC1和MAC3相同，但是错误
    	  st80211AuthReq.aucDstMacAdd[0] = 0x99 ;
    	  st80211AuthReq.aucDstMacAdd[1] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[2] = 0x02 ;
    	  st80211AuthReq.aucDstMacAdd[3] = 0x03 ;
    	  st80211AuthReq.aucDstMacAdd[4] = 0x04 ;
    	  st80211AuthReq.aucDstMacAdd[5] = 0x05 ;

    	  st80211AuthReq.aucBssid[0] = st80211AuthReq.aucDstMacAdd[0];
    	  st80211AuthReq.aucBssid[1] = st80211AuthReq.aucDstMacAdd[1];
    	  st80211AuthReq.aucBssid[2] = st80211AuthReq.aucDstMacAdd[2];
    	  st80211AuthReq.aucBssid[3] = st80211AuthReq.aucDstMacAdd[3];
    	  st80211AuthReq.aucBssid[4] = st80211AuthReq.aucDstMacAdd[4];
    	  st80211AuthReq.aucBssid[5] = st80211AuthReq.aucDstMacAdd[5];
    	  break;

      case 2://MAC1和MAC3不相同，但是值无所谓
    	  st80211AuthReq.aucDstMacAdd[0] = 0x77 ;
    	  st80211AuthReq.aucDstMacAdd[1] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[2] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[3] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[4] = 0x01 ;
    	  st80211AuthReq.aucDstMacAdd[5] = 0x01 ;

    	  st80211AuthReq.aucBssid[0] = 0x55 ;
    	  st80211AuthReq.aucBssid[1] = 0x01 ;
    	  st80211AuthReq.aucBssid[2] = 0x01 ;
    	  st80211AuthReq.aucBssid[3] = 0x01 ;
    	  st80211AuthReq.aucBssid[4] = 0x01 ;
    	  st80211AuthReq.aucBssid[5] = ucWlanId ;
    	  break;

      case 3:
    	  break;

      default:
    	  break;
                }

      MT_SetField16(st80211AuthReq.usFragSeqNum,0,4,0);//Fragment Number
      MT_SetField16(st80211AuthReq.usFragSeqNum,4,8,46);//Sequence Number
      MT_SetField16(st80211AuthReq.usFragSeqNum,12,4,0);//Resvd

      st80211WlanMngmntFrmFixedPara.usAuthAlgrm = usAuthAlgrm;
      if(1 == usAuthSeq)
                {
    	   st80211WlanMngmntFrmFixedPara.usAuthSeq = usAuthSeq;
                }
      else if(2 == usAuthSeq)
               {
    	   usAuthSeq = usAuthSeq + 1;
    	   st80211WlanMngmntFrmFixedPara.usAuthSeq = usAuthSeq;
                }
      st80211WlanMngmntFrmFixedPara.usStatusCode = 0;

      stWlanMangmntFrmTaggedPara.ucTagNum = 16;//Challenge Text
      stWlanMangmntFrmTaggedPara.ucTagLen = vstStaAuthInfo.ucChallengeTxtLen;
      cpss_mem_memcpy(stWlanMangmntFrmTaggedPara.aucTagValue,vstStaAuthInfo.aucChallengeTxt,stWlanMangmntFrmTaggedPara.ucTagLen);
      ulTaggedParaLen = sizeof(stWlanMangmntFrmTaggedPara.ucTagNum) + sizeof(stWlanMangmntFrmTaggedPara.ucTagLen) + stWlanMangmntFrmTaggedPara.ucTagLen;

      stVendoeSpecific.ucTagNum = 221;
      stVendoeSpecific.ucTagLen = 9;
      aucOui[0] = 0x00;
      aucOui[1] = 0x10;
      aucOui[2] = 0x18;
      ucVenOuiType = 2;
      aucVenOuiVal[0] = 0x00;
      aucVenOuiVal[1] = 0x10;
      aucVenOuiVal[2] = 0x00;
      aucVenOuiVal[3] = 0x00;
      aucVenOuiVal[4] = 0x00;
      cpss_mem_memcpy(&stVendoeSpecific.aucTagValue[0],aucOui,3);
      cpss_mem_memcpy(&stVendoeSpecific.aucTagValue[3],&ucVenOuiType,sizeof(ucVenOuiType));
      cpss_mem_memcpy(&stVendoeSpecific.aucTagValue[4],&aucVenOuiVal,5);
      ulVendoeSpecificLen = sizeof(stVendoeSpecific.ucTagNum) + sizeof(stVendoeSpecific.ucTagLen) + stVendoeSpecific.ucTagLen;

      ulSuffix = 0;
      cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
      ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);
      cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
      ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);
      cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&stCpHdrWdata,sizeof(MT_CAPWAP_HDR_WD_T));
      ulSuffix = ulSuffix + sizeof(MT_CAPWAP_HDR_WD_T);

      cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&st80211AuthReq,sizeof(st80211AuthReq));
      ulSuffix = ulSuffix + sizeof(st80211AuthReq);
      cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&st80211WlanMngmntFrmFixedPara,sizeof(st80211WlanMngmntFrmFixedPara));
      ulSuffix = ulSuffix + sizeof(st80211WlanMngmntFrmFixedPara);

      if(MT_AUTH_POLICY_WEPSHAREDKEY == usAuthAlgrm)
                {
    	   if(3 == usAuthSeq)
    	            {
    		   cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&stWlanMangmntFrmTaggedPara,ulTaggedParaLen);
    		   ulSuffix = ulSuffix + ulTaggedParaLen;
    	            }
    	   cpss_mem_memcpy(&auc80211AuthReq[ulSuffix],&stVendoeSpecific,ulVendoeSpecificLen);
    	   ulSuffix = ulSuffix + ulVendoeSpecificLen;
                }

      gst80211AuthHdOverReq.ulBufLen = ulSuffix;

      cpss_mem_memcpy(gst80211AuthHdOverReq.aucBuff,auc80211AuthReq,gst80211AuthHdOverReq.ulBufLen);
          }
    else//MSG_POOL
          {
      usFrameCtrl = 45056;
      usDuration = 0;
      usSeqCtrl = 0;

      usFrameCtrl = htons(usFrameCtrl);
      usDuration = htons(usDuration);
      usSeqCtrl = htons(usSeqCtrl);
      memcpy(&gst80211AuthHdOverReq.aucBuff[16],&usFrameCtrl,2);
      memcpy(&gst80211AuthHdOverReq.aucBuff[18],&usDuration,2);
#if 0
      memcpy(&gst80211AuthReq.aucBuff[20],gastAp[ulApIdx].auApMac,MT_MAC_LENTH);//AP_MAC
#else
      memcpy(&gst80211AuthHdOverReq.aucBuff[20],aucBssId,MT_MAC_LENTH);
#endif
      memcpy(&gst80211AuthHdOverReq.aucBuff[26],gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);//STA_MAC
      memcpy(&gst80211AuthHdOverReq.aucBuff[32],aucBssId,MT_MAC_LENTH);//AP_MAC
      memcpy(&gst80211AuthHdOverReq.aucBuff[38],&usSeqCtrl,2);//Seq_Ctrl
          }

          /*发送STA的认证请求消息*/
    bSendResult = MT_SendStaMsg2AC(vulDstApIndex,glUpmSockId, gst80211AuthHdOverReq.aucBuff, gst80211AuthHdOverReq.ulBufLen);
    if(MT_FALSE == bSendResult)
           {
    	printf("\nApMutiTestSendMsg send 80211AuthReqForHandover failure!\nFile:%s.Line:%d\n",__FILE__,__LINE__);
           }
  return bSendResult;
}

//Handover_ReAssociation
MTBool ApMutiTestSendReAssocReq(MT_STA_WLAN_INFO_T vstStaWLanInfo)
{
	  MTBool bSendResult = MT_FALSE;
	  UINT16 usFrameCtrl = 0;
	  UINT16 usDuration = 0;
	  UINT16 usSeqCtrl = 0;
	  UINT8 aucBssId[MT_MAC_LENTH] = {0};

	  UINT8 auc80211ReAssoReq[256] = {0};
	  UINT32 ulCapwapHdrFisrt32 = 0;
	  UINT32 ulCapwapHdrSecond32 = 0;
	  MT_CAPWAP_HDR_WD_T stCpHdrWdata;
	  UINT8 aucDestMacAdd[MT_MAC_LENTH] = {0};
	  UINT8 aucSrcMacAdd[MT_MAC_LENTH] = {0};
	  UINT16 usSequenceNum = 0;
	  //IEEE802.11Wireless LAN Info Frame
	  UINT16 usCapabilitiesInfo;//按Bit计算
	  UINT16 usListenIntval;
	  UINT8 aucCurrentApMAC[MT_MAC_LENTH] = {0};
	  UINT32 ulApIdx = 0;
	  UINT32 ulStaIndex = 0;
	  UINT8 ucRadioId = 0;
	  UINT8 ucWlanId = 0;
	  MT_SSID_PARA_T stTagSsidPara;
	  UINT8 ucTagSsidParalen = 0;
	  MT_WLAN_SUPPORT_RATES_T stSupportRates;
	  MT_POWER_CAPABILITY_T stPowerCapability;
	  MT_SUPPORT_CHANNELS_T stSupportChannel;
	  MT_WLAN_EXTEND_SUPPORT_RATES_T stExtSupportRates;
	  MT_HT_CAPABILITY_T stHTCapability;
	  MT_VENDOR_SPECIFIC_INFO_T stVendorSpec;
	  UINT32 ulSuffix = 0;

	  cpss_mem_memset(&stTagSsidPara,0,sizeof(MT_SSID_PARA_T));
	  cpss_mem_memset(&stCpHdrWdata,0,sizeof(MT_CAPWAP_HDR_WD_T));
	  cpss_mem_memset(&stSupportRates,0,sizeof(MT_WLAN_SUPPORT_RATES_T));
	  cpss_mem_memset(&stPowerCapability,0,sizeof(MT_POWER_CAPABILITY_T));
	  cpss_mem_memset(&stSupportChannel,0,sizeof(MT_SUPPORT_CHANNELS_T));
	  cpss_mem_memset(&stHTCapability,0,sizeof(MT_HT_CAPABILITY_T));
	  cpss_mem_memset(&stVendorSpec,0,sizeof(MT_VENDOR_SPECIFIC_INFO_T));
	  cpss_mem_memset(&stExtSupportRates,0,sizeof(MT_WLAN_EXTEND_SUPPORT_RATES_T));

	  ulApIdx = vstStaWLanInfo.ulApIndex;
	  ulStaIndex = vstStaWLanInfo.ulStaIndex;
	  ucRadioId = vstStaWLanInfo.ucRadioId;
	  ucWlanId = vstStaWLanInfo.ucWlanId;
	  //MT_GetBssID(vulDstApIndex,ucRadioId,ucWlanId,aucBssId);
	  cpss_mem_memcpy(aucBssId,gastStaPara[ulStaIndex].aucBssId,MT_MAC_LENTH);

	  if(MT_FALSE == gbTestMod)//PARACONF
	     {
	    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
	    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
	    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
	    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,1);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
	    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,1);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
	    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,1);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
	    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
	    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
	    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,1);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
	    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,0);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
	    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
	    MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

	    ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

	    //Fragment ID
	    MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
	    //Fragment Offset
	    MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
	    //Reserved
	    MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

	    ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);

	    //Wireless Data
	    stCpHdrWdata.ucWirelessLen = 4;
	    stCpHdrWdata.ucWDRssi = 0;//dBm
	    stCpHdrWdata.ucWDSnr = 0;//dB
	    stCpHdrWdata.usWDDataRate = htons(0);//Mbps

	    usFrameCtrl = htons(0x2000);//Reassociation
	    usDuration = htons(60);
	#if 0
	    cpss_mem_memcpy(aucDestMacAdd,gastAp[aucDestMacAdd].auApMac,MT_MAC_LENTH);//AP_MAC
	#else
	    cpss_mem_memcpy(aucDestMacAdd,aucBssId,MT_MAC_LENTH);
	#endif
	    cpss_mem_memcpy(aucSrcMacAdd,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);//STA_MAC
	    //BssId

	    //Sequence Number
	    usSequenceNum = htons(0);

	    //IEEE 802.11 wireless LAN Management frame
	    usCapabilitiesInfo = htons(0x0501);//按Bit计算
	    usListenIntval = htons(0x000a);
	    cpss_mem_memcpy(aucCurrentApMAC,gastStaPara[ulStaIndex].aucCrrntAp,MT_MAC_LENTH);
	  //  MT_GetBssID(ulCurrentApIdx,ucRadioId,ucWlanId,aucCurrentApMAC);


	    //SSID
	    stTagSsidPara.ucSsidParaSet = 0;
	    stTagSsidPara.ucSsidLen = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucWlanId].ucSsidLen;
	    cpss_mem_memcpy(stTagSsidPara.aucSsid,gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucWlanId].aucSsid,stTagSsidPara.ucSsidLen);
	    ucTagSsidParalen = sizeof(stTagSsidPara.ucSsidParaSet) + sizeof(stTagSsidPara.ucSsidLen) + stTagSsidPara.ucSsidLen;
//gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucWlanId]

	    //Support Rates 计算方法：128+rate*2
	    stSupportRates.ucTagNum = 1;
	    stSupportRates.ucTagLen = 8;
	    stSupportRates.ucSupportRate[0] = 128 + 1 * 2;//1Mbps
	    stSupportRates.ucSupportRate[1] = 128 + 2 * 2;//2Mbps
	    stSupportRates.ucSupportRate[2] = 128 + 5.5 * 2;//5.5Mbps
	    stSupportRates.ucSupportRate[3] = 128 + 11 * 2;//11Mbps
	    stSupportRates.ucSupportRate[4] = 128 + 6 * 2;//6Mbps
	    stSupportRates.ucSupportRate[5] = 128 + 9 * 2;//9Mbps
	    stSupportRates.ucSupportRate[6] = 128 + 12 * 2;//12Mbps
	    stSupportRates.ucSupportRate[7] = 128 + 18 * 2;//18Mbps

	    //Power Capability
	    stPowerCapability.ucPowerCapability = 33;
	    stPowerCapability.ucLen = 2;
	    stPowerCapability.ucMiniTransPower = 0;
	    stPowerCapability.ucMaxTransPower = 0;

	    //support channel
	    stSupportChannel.ucSupportChannels = 36;
	    stSupportChannel.ucLen = 48;
	    stSupportChannel.stChannel1.ucFirstSupportChannel = 36;
	    stSupportChannel.stChannel1.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel2.ucFirstSupportChannel = 40;
	    stSupportChannel.stChannel2.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel3.ucFirstSupportChannel = 44;
	    stSupportChannel.stChannel3.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel4.ucFirstSupportChannel = 48;
	    stSupportChannel.stChannel4.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel5.ucFirstSupportChannel = 52;
	    stSupportChannel.stChannel5.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel6.ucFirstSupportChannel = 56;
	    stSupportChannel.stChannel6.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel7.ucFirstSupportChannel = 60;
	    stSupportChannel.stChannel7.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel8.ucFirstSupportChannel = 64;
	    stSupportChannel.stChannel8.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel9.ucFirstSupportChannel = 100;
	    stSupportChannel.stChannel9.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel10.ucFirstSupportChannel = 104;
	    stSupportChannel.stChannel10.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel11.ucFirstSupportChannel = 108;
	    stSupportChannel.stChannel11.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel12.ucFirstSupportChannel = 112;
	    stSupportChannel.stChannel12.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel13.ucFirstSupportChannel = 116;
	    stSupportChannel.stChannel13.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel14.ucFirstSupportChannel = 120;
	    stSupportChannel.stChannel14.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel15.ucFirstSupportChannel = 124;
	    stSupportChannel.stChannel15.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel16.ucFirstSupportChannel = 128;
	    stSupportChannel.stChannel16.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel17.ucFirstSupportChannel = 132;
	    stSupportChannel.stChannel17.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel18.ucFirstSupportChannel = 136;
	    stSupportChannel.stChannel18.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel19.ucFirstSupportChannel = 140;
	    stSupportChannel.stChannel19.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel20.ucFirstSupportChannel = 149;
	    stSupportChannel.stChannel20.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel21.ucFirstSupportChannel = 153;
	    stSupportChannel.stChannel21.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel22.ucFirstSupportChannel = 157;
	    stSupportChannel.stChannel22.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel23.ucFirstSupportChannel = 161;
	    stSupportChannel.stChannel23.ucSupprotChannelRange = 1;
	    stSupportChannel.stChannel24.ucFirstSupportChannel = 165;
	    stSupportChannel.stChannel24.ucSupprotChannelRange = 1;

	    //Extend Rates 计算方法：rate*2
	    stExtSupportRates.ucTagNum = 50;
	    stExtSupportRates.ucTagLen = 4;
	    stExtSupportRates.ucExtSupportRate[0] = 24 * 2;//24
	    stExtSupportRates.ucExtSupportRate[1] = 36 * 2;//36
	    stExtSupportRates.ucExtSupportRate[2] = 48 * 2;//48
	    stExtSupportRates.ucExtSupportRate[3] = 54 * 2;//54

	    //HT Capability
	    stHTCapability.ucHtCapability = 45;
	    stHTCapability.ucLen = 26;
	    stHTCapability.usHtCapabilityInfo = htons(0x0834);
	    stHTCapability.ucAmpduPara = 0x17;
	    stHTCapability.stRxSupportMcsSet.ulFirst32Bit = htonl(0x0000ffff);
	    stHTCapability.stRxSupportMcsSet.ulSecond32Bit = htonl(0x00000000);
	    stHTCapability.stRxSupportMcsSet.usHighestSupportDataRate = htons(0x0000);
	    stHTCapability.stRxSupportMcsSet.ucTxSupportMcs = 0x00;
	    stHTCapability.usHtExtndCapability = htons(0x0000);
	    stHTCapability.ulTxBFCapability = htonl(0x00000000);
	    stHTCapability.ucASELCapability = 0x00;

	    //Vendor Specific
	    stVendorSpec.ucTageNum = 221;
	    stVendorSpec.ucTagLen = 7;
	    stVendorSpec.aucOui[0] = 0x00;
	    stVendorSpec.aucOui[1] = 0x50;
	    stVendorSpec.aucOui[2] = 0xf2;
	    stVendorSpec.ucVendorOuiType = 2;
	    stVendorSpec.ucWmeSubtype = 0;
	    stVendorSpec.ucWmeVer = 1;
	    stVendorSpec.ucWmeQoS = 0;

	    ulSuffix = 0;
	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&ulCapwapHdrFisrt32,sizeof(ulCapwapHdrFisrt32));
	    ulSuffix = ulSuffix + sizeof(ulCapwapHdrFisrt32);

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&ulCapwapHdrSecond32,sizeof(ulCapwapHdrSecond32));
	    ulSuffix = ulSuffix + sizeof(ulCapwapHdrSecond32);

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&stCpHdrWdata,sizeof(MT_CAPWAP_HDR_WD_T));
	    ulSuffix = ulSuffix + sizeof(MT_CAPWAP_HDR_WD_T);

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&usFrameCtrl,sizeof(usFrameCtrl));
	    ulSuffix = ulSuffix + sizeof(usFrameCtrl);

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&usDuration,sizeof(usDuration));
	    ulSuffix = ulSuffix + sizeof(usDuration);

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],aucDestMacAdd,MT_MAC_LENTH);
	    ulSuffix = ulSuffix + MT_MAC_LENTH;

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],aucSrcMacAdd,MT_MAC_LENTH);
	    ulSuffix = ulSuffix + MT_MAC_LENTH;

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],aucBssId,MT_MAC_LENTH);
	    ulSuffix = ulSuffix + MT_MAC_LENTH;

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&usSequenceNum,sizeof(usSequenceNum));
	    ulSuffix = ulSuffix + sizeof(usSequenceNum);

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&usCapabilitiesInfo,sizeof(usCapabilitiesInfo));
	    ulSuffix = ulSuffix + sizeof(usCapabilitiesInfo);

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&usListenIntval,sizeof(usListenIntval));
	    ulSuffix = ulSuffix + sizeof(usListenIntval);

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],aucCurrentApMAC,MT_MAC_LENTH);
	    ulSuffix = ulSuffix + MT_MAC_LENTH;

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&stTagSsidPara,ucTagSsidParalen);
	    ulSuffix = ulSuffix + ucTagSsidParalen;

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&stSupportRates,sizeof(MT_WLAN_SUPPORT_RATES_T));
	    ulSuffix = ulSuffix + sizeof(MT_WLAN_SUPPORT_RATES_T);

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&stPowerCapability,sizeof(MT_POWER_CAPABILITY_T));
	    ulSuffix = ulSuffix + sizeof(MT_POWER_CAPABILITY_T);

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&stSupportChannel,sizeof(MT_SUPPORT_CHANNELS_T));
	    ulSuffix = ulSuffix + sizeof(MT_SUPPORT_CHANNELS_T);

	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&stExtSupportRates,sizeof(MT_WLAN_EXTEND_SUPPORT_RATES_T));
	    ulSuffix = ulSuffix + sizeof(MT_WLAN_EXTEND_SUPPORT_RATES_T);
#if 0
	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&stHTCapability,sizeof(MT_HT_CAPABILITY_T));
	    ulSuffix = ulSuffix + sizeof(MT_HT_CAPABILITY_T);
#endif
	    cpss_mem_memcpy(&auc80211ReAssoReq[ulSuffix],&stVendorSpec,sizeof(MT_VENDOR_SPECIFIC_INFO_T));
	    ulSuffix = ulSuffix + sizeof(MT_VENDOR_SPECIFIC_INFO_T);

	    gst80211ReAssocReq.ulBufLen = ulSuffix;
	    cpss_mem_memcpy(gst80211ReAssocReq.aucBuff,auc80211ReAssoReq,gst80211ReAssocReq.ulBufLen);
	     }
	  else//MSG_POOL
	     {
	    usFrameCtrl = htons(0);
	    usDuration = htons(0);
	    usSeqCtrl = htons(0);
	    memcpy(&gst80211ReAssocReq.aucBuff[16],&usFrameCtrl,2);
	    memcpy(&gst80211ReAssocReq.aucBuff[18],&usDuration,2);
	#if 0
	    memcpy(&gst80211ReAssocReq.aucBuff[20],gastAp[ulApIdx].auApMac,MT_MAC_LENTH);//AP_MAC
	#else
	    memcpy(&gst80211ReAssocReq.aucBuff[20],aucBssId,MT_MAC_LENTH);
	#endif
	    memcpy(&gst80211ReAssocReq.aucBuff[26],gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);//STA_MAC
	    memcpy(&gst80211ReAssocReq.aucBuff[32],aucBssId,MT_MAC_LENTH);//AP_MAC
	    memcpy(&gst80211ReAssocReq.aucBuff[38],&usSeqCtrl,2);//Seq_Ctrl
	      }

	    bSendResult = MT_SendStaMsg2AC(ulApIdx,glUpmSockId, gst80211ReAssocReq.aucBuff,gst80211ReAssocReq.ulBufLen);
	    if(MT_FALSE == bSendResult)
	          {
	    	printf("ERROR.ApMutiTestSendAssocReq failure.\nFile:%s,Line:%d\n",__FILE__,__LINE__);
	          }

	  return bSendResult;
}

void staStateInit(UINT32 vulStaIndex)
{
	UINT32 ulStaIndx = vulStaIndex;

	gastStaPara[ulStaIndx].enAuthState = AUTH_INIT_STATE;
	gastStaPara[ulStaIndx].enHandOverState = HANDOVER_INIT_STATE;
	gastStaPara[ulStaIndx].enDhcpState = DHCPV4_INIT_STATE;
	gastStaPara[ulStaIndx].enDhcpv6State = DHCPV6_INIT_STATE;
	gastStaPara[ulStaIndx].stStaOnLineTm.ulBeginTime = 0;
	gastStaPara[ulStaIndx].stStaOnLineTm.ulEndTime = 0;
	cpss_mem_memset(gastStaPara[ulStaIndx].aucBssId,0,MT_MAC_LENTH);
	cpss_mem_memset(gastStaPara[ulStaIndx].aucCrrntAp,0,MT_MAC_LENTH);
	cpss_timer_para_delete(gastStaPara[ulStaIndx].ulStaHdOverTimerId);
	cpss_timer_para_delete(gastStaPara[ulStaIndx].ulStaHoldTimerId);
	gastStaPara[ulStaIndx].ulHomeApId = MT_UNDEFINED_OBJECT_IDENTIFIER;
	gastStaPara[ulStaIndx].ulForeignApId = MT_UNDEFINED_OBJECT_IDENTIFIER;
	gastStaPara[ulStaIndx].ulStaLocalApId = MT_UNDEFINED_OBJECT_IDENTIFIER;
	gastStaPara[ulStaIndx].ucWlanId = MT_UNDEFINED_WLANID_IDENTIFIER;
}

void MT_StaTmOv(UINT32 vulStaIndex)
{
	UINT32 ulApIdx = 0;
	MT_STA_AUTH_INFO stStaAuthInfo;
	UINT8 ucRadioId = 0;
	UINT8 ucWlanNum = 0;
	UINT8 ucStaWlanIndex = 0;
	MTBool bSendResult = MT_FALSE;

	cpss_mem_memset(&stStaAuthInfo,0,sizeof(MT_STA_AUTH_INFO));

	if(AUTH_INIT_STATE == gastStaPara[vulStaIndex].enAuthState)
	{
		return;
	}

	ulApIdx = getApIDbyBssid(vulStaIndex);

	if(MT_UNDEFINED_OBJECT_IDENTIFIER == ulApIdx)
	{
		return;
	}

	if(RUN_STATE != gastAp[ulApIdx].enState)
	{
		return;
	}


	if(HANDOVER_SUCCESS != gastStaPara[vulStaIndex].enHandOverState)
	{
		stStaAuthInfo.ulApIndex = gastStaPara[vulStaIndex].ulHomeApId;
	}

	stStaAuthInfo.ulStaIndex = vulStaIndex;
	ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况
	stStaAuthInfo.ucRadioId = ucRadioId;
	ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

	if(0 == ucWlanNum)
	{
		return;
	}

	ucStaWlanIndex = vulStaIndex % ucWlanNum;
	stStaAuthInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
	stStaAuthInfo.usAuthAlgrm = (UINT16)gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucStaWlanIndex].ucAuthType;

	bSendResult = ApMutiTestSend80211DeAuthReq(stStaAuthInfo);

	if(MT_FALSE == bSendResult)
	{
		gstErrMsgStatics.ulSendDeAuthErr++;
		MT_StaLog(vulStaIndex,"STA Send80211DeAuthReq Failure.");
		return;
	}

	gstMutiTest_Statics.ulSendDeAuth++;
	gbLogFlag = MT_TRUE;
	MT_StaLog(vulStaIndex,"STA Send80211DeAuthReq Success.");
	gbLogFlag = MT_FALSE;
	if(ASSOCIATION_SUCESS_STATE != gastStaPara[vulStaIndex].enAuthState)
	{
		gstMutiTest_Statics.ulAssoctnTmOut++;
		MT_StaLog(vulStaIndex,"STA offline for Auth TimeOut.");
	}
	else
	{
		if(MT_UNDEFINED_OBJECT_IDENTIFIER == gastStaPara[vulStaIndex].ulStaLocalApId)
		{
			gulOnlineStaCnt--;
			MT_StaLog(vulStaIndex,"STA offline without AP to locate.");
		}
	}
	staStateInit(vulStaIndex);
}

void MT_StaOnlineChk(void)
{
	UINT32 ulStaIndex = 0;
	UINT32 ulStaRunTime = 0;
	UINT8 aucTmOvInfo[512] = {0};

	for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
	{
		if(AUTH_INIT_STATE != gastStaPara[ulStaIndex].enAuthState)
		{
			ulStaRunTime = 0;

			gastStaPara[ulStaIndex].stStaOnLineTm.ulEndTime = cpss_tick_get();
			ulStaRunTime = (gastStaPara[ulStaIndex].stStaOnLineTm.ulEndTime - gastStaPara[ulStaIndex].stStaOnLineTm.ulBeginTime) / 1000;

			if(ulStaRunTime >= (gulStaHoldTime + 10))
		    {
				sprintf((char*)aucTmOvInfo,"No TimerMsg called for Off-line,so It has to off-line. Online TimeLen is :%ds. STA HoldOn TimeLen:%ds.\n\n",ulStaRunTime,gulStaHoldTime);

				MT_StaLog(ulStaIndex,(char*)aucTmOvInfo);

				MT_StaTmOv(ulStaIndex);
		    }
		}
	}
}

void staofflinebyforce(void)
{
	UINT32 ulStaIndex = MT_UNDEFINED_OBJECT_IDENTIFIER;
	UINT32 ulApIdx = MT_UNDEFINED_OBJECT_IDENTIFIER;
	MT_STA_AUTH_INFO stStaAuthInfo;
	UINT8 ucRadioId = 0;
	UINT8 ucWlanNum = 0;
	UINT8 ucStaWlanIndex = 0;
	MTBool bSendResult = MT_FALSE;

	for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
	{
		if(AUTH_INIT_STATE != gastStaPara[ulStaIndex].enAuthState)
		{
			ulApIdx = getApIDbyBssid(ulStaIndex);

			if(MT_UNDEFINED_OBJECT_IDENTIFIER == ulApIdx)
			{
				continue;
			}

			if(RUN_STATE != gastAp[ulApIdx].enState)
			{
				continue;
			}

			cpss_mem_memset(&stStaAuthInfo,0,sizeof(MT_STA_AUTH_INFO));

			if(HANDOVER_SUCCESS != gastStaPara[ulStaIndex].enHandOverState)
			{
				stStaAuthInfo.ulApIndex = gastStaPara[ulStaIndex].ulHomeApId;
			}

			stStaAuthInfo.ulStaIndex = ulStaIndex;
			ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况
			stStaAuthInfo.ucRadioId = ucRadioId;
			ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

			if(0 == ucWlanNum)
			{
				break;
			}

			ucStaWlanIndex = ulStaIndex % ucWlanNum;
			stStaAuthInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
			stStaAuthInfo.usAuthAlgrm = (UINT16)gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucStaWlanIndex].ucAuthType;

			bSendResult = ApMutiTestSend80211DeAuthReq(stStaAuthInfo);

			if(MT_FALSE == bSendResult)
			{
				gstErrMsgStatics.ulSendDeAuthErr++;
				MT_StaLog(ulStaIndex,"STA Send80211DeAuthReq Failure.");
				staStateInit(ulStaIndex);
				return;
			}

			gstMutiTest_Statics.ulSendDeAuth++;
			MT_StaLog(ulStaIndex,"STA Send80211DeAuthReq Success.");

			if(ASSOCIATION_SUCESS_STATE != gastStaPara[ulStaIndex].enAuthState)
			{
				gstMutiTest_Statics.ulAssoctnTmOut++;
				MT_StaLog(ulStaIndex,"STA offline for Auth TimeOut.");
				staStateInit(ulStaIndex);
			}
			else
			{
				if(MT_UNDEFINED_OBJECT_IDENTIFIER == gastStaPara[ulStaIndex].ulStaLocalApId)
				{
					gulOnlineStaCnt--;
					MT_StaLog(ulStaIndex,"STA offline without AP to locate.");
					staStateInit(ulStaIndex);
				}
			}
		}
	}




}
