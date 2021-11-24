/******************************************************************************
* COPYRIGHT @ Reserved
*******************************************************************************
* File         : sta_proc.h
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
#ifndef STA_PROC_H_
#define STA_PROC_H_

#include "mt_common.h"
#define WAI_BKID_LEN           16
#define WAI_ADDID_LEN          12
#define WAI_ASUE_CHALLENGE_LEN 32
#define WAI_AE_CHALLENGE_LEN   32
#define WAI_WIE_FROM_ASUE_LEN  22
#define WAI_MSG_AUTH_CODE_LEN  20
#define WAI_KEY_ANNOUNCEMENT_IDENTIFIER_LEN 16

typedef struct _CAPWAP_HDR_WD_T_
{
	UINT8 ucWirelessLen;
	UINT8 ucWDRssi;//dBm
	UINT8 ucWDSnr;//dB
	UINT16 usWDDataRate;//Mbps
	UINT8 aucPadding[3];
}__attribute__ ((packed)) MT_CAPWAP_HDR_WD_T;

typedef struct _SSID_PARA_T_
{
	UINT8 ucSsidParaSet;
	UINT8 ucSsidLen;
	UINT8 aucSsid[MT_SSID_LEN];
}__attribute__ ((packed)) MT_SSID_PARA_T;

typedef struct _SUPPORT_RATES_T_
{
	UINT8 ucTagNum;
	UINT8 ucTagLen;
	UINT8 ucSupportRate[8];
}__attribute__ ((packed)) MT_WLAN_SUPPORT_RATES_T;

typedef struct _EXTEND_SUPPORT_RATES_T_
{
	UINT8 ucTagNum;
	UINT8 ucTagLen;
	UINT8 ucExtSupportRate[4];
}__attribute__ ((packed)) MT_WLAN_EXTEND_SUPPORT_RATES_T;

typedef struct _POWER_CAPABILITY_T_
{
	UINT8 ucPowerCapability;
	UINT8 ucLen;
	UINT8 ucMiniTransPower;
	UINT8 ucMaxTransPower;
}MT_POWER_CAPABILITY_T;

typedef struct _VENDOR_SPECIFIC_INFO_T_
{
	UINT8 ucTageNum;
	UINT8 ucTagLen;
	UINT8 aucOui[3];
	UINT8 ucVendorOuiType;
	UINT8 ucWmeSubtype;
	UINT8 ucWmeVer;
	UINT8 ucWmeQoS;//按Bit计算
}__attribute__ ((packed)) MT_VENDOR_SPECIFIC_INFO_T;

typedef struct _SUPPORT_CHANNEL_SET_T_
{
	UINT8 ucFirstSupportChannel;
	UINT8 ucSupprotChannelRange;
}__attribute__ ((packed)) MT_SUPPORT_CHANNEL_SET_T;

typedef struct _SUPPORT_CHANNELS_T_
{
	UINT8 ucSupportChannels;
	UINT8 ucLen;
	MT_SUPPORT_CHANNEL_SET_T stChannel1;
	MT_SUPPORT_CHANNEL_SET_T stChannel2;
	MT_SUPPORT_CHANNEL_SET_T stChannel3;
	MT_SUPPORT_CHANNEL_SET_T stChannel4;
	MT_SUPPORT_CHANNEL_SET_T stChannel5;
	MT_SUPPORT_CHANNEL_SET_T stChannel6;
	MT_SUPPORT_CHANNEL_SET_T stChannel7;
	MT_SUPPORT_CHANNEL_SET_T stChannel8;
	MT_SUPPORT_CHANNEL_SET_T stChannel9;
	MT_SUPPORT_CHANNEL_SET_T stChannel10;
	MT_SUPPORT_CHANNEL_SET_T stChannel11;
	MT_SUPPORT_CHANNEL_SET_T stChannel12;
	MT_SUPPORT_CHANNEL_SET_T stChannel13;
	MT_SUPPORT_CHANNEL_SET_T stChannel14;
	MT_SUPPORT_CHANNEL_SET_T stChannel15;
	MT_SUPPORT_CHANNEL_SET_T stChannel16;
	MT_SUPPORT_CHANNEL_SET_T stChannel17;
	MT_SUPPORT_CHANNEL_SET_T stChannel18;
	MT_SUPPORT_CHANNEL_SET_T stChannel19;
	MT_SUPPORT_CHANNEL_SET_T stChannel20;
	MT_SUPPORT_CHANNEL_SET_T stChannel21;
	MT_SUPPORT_CHANNEL_SET_T stChannel22;
	MT_SUPPORT_CHANNEL_SET_T stChannel23;
	MT_SUPPORT_CHANNEL_SET_T stChannel24;
}__attribute__ ((packed)) MT_SUPPORT_CHANNELS_T;

typedef struct _GROUP_CIPHER_SUITE_T_
{
	UINT8 aucGroupCipherSuitOui[3];
	UINT8 aucGroupCipherSuitType;
}__attribute__ ((packed)) MT_GROUP_CIPHER_SUITE_T;

typedef struct _PAIRWISER_CIPHER_SUITELIST_T_
{
#define MT_PAIRWISER_CIPHER_SUITE_CNT 8
	MT_GROUP_CIPHER_SUITE_T astPairwiserCipherSuite[MT_PAIRWISER_CIPHER_SUITE_CNT];
}__attribute__ ((packed)) MT_PAIRWISER_CIPHER_SUITE_LIST_T;

typedef struct _AKM_SUITE_T_
{
	UINT8 aucAkmOui[3];
	UINT8 ucAkmType;
}__attribute__ ((packed)) MT_AKM_SUITE_T;

typedef struct _AKM_LIST_T_
{
#define MT_AKM_LIST_CNT 8
	MT_AKM_SUITE_T astAkmSuite[MT_AKM_LIST_CNT];
}__attribute__ ((packed)) MT_AKM_LIST_T;

typedef struct _RSN_INFO_T_
{
	UINT8 ucRsnInfo;
	UINT8 ucLen;
	UINT16 usRsnVer;
	MT_GROUP_CIPHER_SUITE_T stGroupCipherSuite;
	UINT16 usPairwiserCipherSuiteCnt;
	MT_PAIRWISER_CIPHER_SUITE_LIST_T stPairwiserCipherSuiteList;
	UINT16 usAKMSuiteCnt;
	MT_AKM_LIST_T stAkmSuiteList;
	UINT16 usRsnCapability;
	UINT16 usPmkidCnt;
	//PMKID LIST
}__attribute__ ((packed)) MT_RSN_INFO_T;

typedef struct _RX_SUPPORT_MCS_SET_T_
{
	UINT32 ulFirst32Bit;
	UINT32 ulSecond32Bit;
	UINT16 usLast16Bit;
	UINT16 usHighestSupportDataRate;
	UINT8 ucTxSupportMcs;
	UINT8 aucRsvd[3];
}__attribute__ ((packed)) MT_RX_SUPPORT_MCS_SET_T;

typedef struct _HT_CAPABILITY_T_
{
	UINT8 ucHtCapability;
	UINT8 ucLen;
	UINT16 usHtCapabilityInfo;
	UINT8 ucAmpduPara;
	MT_RX_SUPPORT_MCS_SET_T stRxSupportMcsSet;
	UINT16 usHtExtndCapability;
	UINT32 ulTxBFCapability;//Transmit Beam Forming capability
	UINT8 ucASELCapability;//Antenna Selection capability
}__attribute__ ((packed)) MT_HT_CAPABILITY_T;

extern MTBool ApMutiTestSend80211AuthReq(MT_STA_AUTH_INFO vstStaAuthInfo);//80211AuthReq消息发送函数,STA->SUT(AC)
extern MTBool ApMutiTestSend80211AuthReqForHandover(MT_STA_AUTH_INFO vstStaAuthInfo,UINT32 vulDstApIndex);
extern MTBool ApMutiTestSend80211DeAuthReq(MT_STA_AUTH_INFO vstStaAuthInfo);
extern MTBool ApMutiTestSendAssocReq(MT_STA_WLAN_INFO_T vstStaWLanInfo);
extern MTBool ApMutiTestSendReAssocReq(MT_STA_WLAN_INFO_T vstStaWLanInfo);
extern MTBool ApMutiTestSend80211DisAssocReq(MT_STA_WLAN_INFO_T vstStaWLanInfo);
extern MTBool ApMutiTestSend8021xAuthRsp(UINT32 vulApIdx, UINT32 vulStaIndex,MT_8021X_EAPOL_INFO_T vst8021xEapolInfo);
extern MTBool ApMutiTestSendWapiAuthRsp(UINT32 vulApIdx, UINT32 vulStaIndex,MT_WAI_AUTH_MSG_INFO_T vstWaiAuthInfo);
extern MTBool ApMutiTestSendDhcpDiscover(MT_STA_WLAN_INFO_T vstStaWLanInfo);
extern MTBool ApMutiTestSendDhcpReq(MT_STA_WLAN_INFO_T vstStaWLanInfo);
extern MTBool ApMutiTestSendArpReq(MT_STA_WLAN_INFO_T vstStaWLanInfo);
extern MTBool ApMutiTestSendArpReply(MT_STA_WLAN_INFO_T vstStaWLanInfo);
extern MTBool ApMutiTestSendPingReq(MT_STA_WLAN_INFO_T vstStaWLanInfo);
extern MTBool ApMutiTestSendPingReply(MT_STA_WLAN_INFO_T vstStaWLanInfo, MT_PING_REPLY_STA_INFO vstPingReplyStaInfo);
extern MTBool ApMutiTestSendNeighbrSolicit(MT_STA_WLAN_INFO_T vstStaWLanInfo);
extern MTBool ApMutiTestSendNeighbrAdvertsmnt(MT_STA_WLAN_INFO_T vstStaWLanInfo);
extern MTBool ApMutiTestSendPing6Req(MT_STA_WLAN_INFO_T vstStaWLanInfo);
extern MTBool ApMutiTestSendPing6Reply(MT_STA_WLAN_INFO_T vstStaWLanInfo, MT_PING6_REPLY_STA_INFO vstPing6ReplyStaInfo);
extern MTBool ApMutiTestSendDhcpv6Solicit(MT_STA_WLAN_INFO_T vstStaWLanInfo);
extern MTBool ApMutiTestSendDhcpv6Req(MT_STA_WLAN_INFO_T vstStaWLanInfo);

extern void assocflood(void);
extern void disassocflood(void);
extern void deauthflood(void);
extern void authflood(void);
extern void dhcpflood(void);
extern void deauthtest(UINT32 vgulDeAuthTestFlag);
extern void disassoctest(UINT32 vgulDeAssoTestFlag);
extern void authtest(UINT32 vgulAuthTestFlag);
extern void deAuthForNonExistUser(UINT32 vulApIndx);
extern void staStateInit(UINT32 vulStaIndex);
extern void MT_StaTmOv(UINT32 vulStaIndex);
extern void MT_StaOnlineChk(void);

#if 0
//PING BGN
#ifdef IPV6
#include "ipv6.h"
#include "icmp6.h"
#endif

#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>

#define BUFSIZE 1500
UINT8 recvbuf[BUFSIZE];
UINT8 sendbuf[BUFSIZE];

INT32 datalen;//bytes of data, following ICMP header
UINT8 *host;
INT32 nsent;//add 1 for each sendto
pid_t pid;
INT32 sockfd;
INT32 verbose;

void proc_v4(UINT8 *, ssize_t,struct timeval *);
void proc_v6(UINT8 *, ssize_t,struct timeval *);

void send_v4(void);
void send_v6(void);
void readloop(void);
void sig_alrm(INT32);
void tv_sub(struct timeval *,struct timeval *);

struct proto
{
        void (*fproc)(UINT8 *, ssize_t,struct timeval *);
        void (*fsend)(void);
        struct sockaddr *assend;//sockaddr() for send, from getaddrinfo
        struct sockaddr *asrecv;//sockaddr{} for receiving
        socklen_t salen;//length of sockaddr()s
        INT32 icmpproto;//IPPROTO_XXX value for ICMP
}*pr;

//PING END
#endif

#endif /* STA_PROC_H_ */
