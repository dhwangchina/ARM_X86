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
#ifndef MT_COMMON_H_
#define MT_COMMON_H_

#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <time.h>
#include "swp_type.h"
#include "cpss_mm.h"
#include "cpss_tm.h"

#if 1
#define MT_STA_HANDOVER 1
#endif

#define MT_SOCKET_DATA_CPM_IND_MSG     0x22000007
#define MT_SOCKET_DATA_UPM_IND_MSG     0x22000008
#define MT_SOCKET_DATA_DHCP_IND_MSG    0x22000009

#define MT_TEST_PERIOD_TIMER                      21
#define MT_TEST_PERIOD_TMRID                      CPSS_TIMER_21
#define MT_TEST_PERIOD_TIMOV_MSG                  CPSS_TIMER_21_MSG

#define MT_TEST_APHOLD_TIMER                      22
#define MT_TEST_APHOLD_TMRID                      CPSS_TIMER_22
#define MT_TEST_APHOLD_TIMOV_MSG                  CPSS_TIMER_22_MSG

#define MT_TEST_STOP_TIMER                        23
#define MT_TEST_STOP_TMRID                        CPSS_TIMER_23
#define MT_TEST_STOP_TIMOV_MSG                    CPSS_TIMER_23_MSG

#define MT_TEST_STAHOLD_TIMER                     24
#define MT_TEST_STAHOLD_TMRID                     CPSS_TIMER_24
#define MT_TEST_STAHOLD_TIMOV_MSG                 CPSS_TIMER_24_MSG

#define MT_TEST_STAHD_OVER_TIMER                  25
#define MT_TEST_STAHD_OVER_TMRID                  CPSS_TIMER_25
#define MT_TEST_STAHD_OVER_TIMOV_MSG              CPSS_TIMER_25_MSG

#define MT_TEST_PERIOD_PRINT_TIMER                27
#define MT_TEST_PERIOD_PRINT_TMRID                CPSS_TIMER_27
#define MT_TEST_PERIOD_PRINT_TIMOV_MSG            CPSS_TIMER_27_MSG

#define MT_TEST_PERIOD_ECHO_TIMER                 28
#define MT_TEST_PERIOD_ECHO_TMRID                 CPSS_TIMER_28
#define MT_TEST_PERIOD_ECHO_TIMOV_MSG             CPSS_TIMER_28_MSG

#define MT_AP_CPM_PORT_START    1001
#define MT_AP_UPM_PORT_START    8001
#define IP_ADDR_TYPE_V4         0//IPV4
#define IP_ADDR_TYPE_V6         1//IPV6
#define MT_MAC_LENTH            6
#define MT_IPV4_ADDR_LEN        4
#define MT_IPV6_ADDR_LEN        16
#define TRIGGER_BUFF_LEN        10
#define AP_MODULE_NAME_LEN      32
#define AP_IMG_FILE_NAME_LEN    32
#define AP_IMG_VER_NAME_LEN     32
#define AP_SERIAL_NO_LEN        32
#define AP_BOARD_ID_LEN         16
#define AP_BOARD_REVISON_LEN    16
#define MT_MAC_STR_LEN          18
#define MT_IPV4ADDR_STR_LEN     16
#define MT_IPV6ADDR_STR_LEN     40
#define MT_AC_NAME_LEN          512
#define MT_DEV_NAME_LEN         16
#define MT_SSID_LEN             32
#define MT_AP_MAX_NUM             4096
#define MT_WLAN_MAX_NUM           16//WLAN MAX NUM FOR ONE AP
#define MT_RADIO_MAX_NUM          31
#define MT_STA_MAX_NUM_PER_WTP    64
#define MT_SYS_STA_CAPABILITY     (MT_AP_MAX_NUM * MT_STA_MAX_NUM_PER_WTP)
#define MT_BUFF_LEN               2048
#define MT_IPV4_HEADER_LEN        20
#define MT_IPV6_HEADER_LEN        40
#define MT_UDP_HEADER_LEN         8
#define MT_CAPWAPMSG_BUFF_LEN     2048
#define MT_STATE_BUF_LEN          32
#define MSG_ELEMENT_VALUE_LEN     2048

#define MT_CAPWAP_PORT_5246        5246
#define MT_CAPWAP_PORT_5247        5247
#define MT_RECV_MSG_BUF_LEN       (2 * 1024)
#define MT_CAPWAP_BUF_LEN         (2 * 1024)
#define MT_DHCP_SRC_PORT          68
#define MT_DHCP_DST_PORT          67
#define MT_DHCPV6_SRC_PORT        546
#define MT_DHCPV6_DST_PORT        547

#define MT_STA_ACCESS_POLICY_AP_FIRST  0 //用户接入策略：AP优先，即优先接入小序号AP，当小序号AP接满用户后，才接入大序号AP；
#define MT_STA_ACCESS_POLICY_STA_FIRST 1 //用户接入策略：STA优先，及STA轮询接入每个AP；

/*80211 Capwap封装*/
#define MT_SetField32(src,start,len,val)  src |= ((~((UINT32)0xFFFFFFFF << len)) & val) << (32 - start - len)
#define MT_SetField16(src,start,len,val)  src |= ((~((UINT16)0xFFFF << len)) & val) << (16 - start - len)
#define MT_SetField8(src,start,len,val)   src |= ((~((UINT8)0xFF << len)) & val) << (8 - start - len)

//获取bit位信息
#define MT_GetField32(src,start,len,val)  val = (src & ((UINT32)0xFFFFFFFF >> start)) >> (32 - start - len)
#define MT_GetField16(src,start,len,val)  val = (src & ((UINT16)0xFFFF >> start)) >> (16 - start - len)
#define MT_GetField8(src,start,len,val)   val = (src & ((UINT8)0xFF >> start)) >> (8 - start - len)

#define MT_CAPWAP_HDR_VERSION_START   0
#define MT_CAPWAP_HDR_VERSION_LEN     4

#define MT_CAPWAP_HDR_TYPE_START      4
#define MT_CAPWAP_HDR_TYPE_LEN        4

#define MT_CAPWAP_HDR_HLEN_START      8
#define MT_CAPWAP_HDR_HLEN_LEN        5

#define MT_CAPWAP_HDR_RID_START       13
#define MT_CAPWAP_HDR_RID_LEN         5

#define MT_CAPWAP_HDR_WBID_START      18
#define MT_CAPWAP_HDR_WBID_LEN        5

#define MT_CAPWAP_HDR_T_START         23
#define MT_CAPWAP_HDR_T_LEN           1

#define MT_CAPWAP_HDR_F_START         24
#define MT_CAPWAP_HDR_F_LEN           1

#define MT_CAPWAP_HDR_L_START         25
#define MT_CAPWAP_HDR_L_LEN           1

#define MT_CAPWAP_HDR_W_START         26
#define MT_CAPWAP_HDR_W_LEN           1

#define MT_CAPWAP_HDR_M_START         27
#define MT_CAPWAP_HDR_M_LEN           1

#define MT_CAPWAP_HDR_K_START         28
#define MT_CAPWAP_HDR_K_LEN           1

#define MT_CAPWAP_HDR_FLAGS_START     29
#define MT_CAPWAP_HDR_FLAGS_LEN       3

#define MT_CAPWAP_HDR_FRAGMENT_ID_START 0
#define MT_CAPWAP_HDR_FRAGMENT_ID_LEN   16

#define MT_CAPWAP_HDR_FRAG_OFFSET_START 16
#define MT_CAPWAP_HDR_FRAG_OFFSET_LEN   13

#define MT_CAPWAP_HDR_RSVD_START        29
#define MT_CAPWAP_HDR_RSVD_LEN          3

#define MT_WTP_EVENT_L2IE_ENABLE        1
#define MT_WTP_EVENT_L2IE_DISABLE       0

#define MT_WTP_EVENT_ENABLE             1
#define MT_WTP_EVENT_DISABLE            0

#define WAI_TYPE_WAI_PROTOCOL_PACKAGE              1
#define WAI_SUBTYPE_UNICASTKEY_NEGOTIATION_REQ     8
#define WAI_SUBTYPE_UNICASTKEY_NEGOTIATION_RSP     9
#define WAI_SUBTYPE_UNICASTKEY_NEGOTIATION_CFM     10
#define WAI_SUBTYPE_MULTICASTKEY_ANNOUNCEMENT      11
#define WAI_SUBTYPE_MULTICASTKEY_ANNOUNCEMENT_RSP  12
#define MT_WTP_BSSID_KEY_BYTE                      0x0B

#define MT_UNDEFINED_OBJECT_IDENTIFIER             0xFFFFFFFF
#define MT_UNDEFINED_WLANID_IDENTIFIER             0xFF

typedef struct _DISPATCHER_SND_MSG_T_
{
	UINT32 ulApIdx;
	UINT32 ulCapwapBufLen;
	UINT8 aucCapwapBuf[MT_CAPWAP_BUF_LEN];
}MT_DISPATCHER_SND_MSG_T;

//RecvFrom 和ComSend消息统计
typedef struct _RCV_SND_MSG_T_
{
	unsigned long long ulRcvfrmMsgCnt;
	unsigned long long ulRcvfrmMsgErrCnt;

	unsigned long long ulComSnd2ApMsgCnt;
	unsigned long long ulComSnd2ApMsgErrCnt;
	unsigned long long ulComSnd2StaMsgCnt;
	unsigned long long ulComSnd2StaMsgErrCnt;

	unsigned long long ul5246MsgFrmDispchrCnt;
	unsigned long long ul5247MsgFrmDispchrCnt;
	unsigned long long ulOtherMsgFrmDispchrCnt;

	unsigned long long ulAPSndtoAC_IPv4MsgCnt;
	unsigned long long ulAPSndtoAC_IPv4MsgErrCnt;
	unsigned long long ulAPSndtoAC_IPv6MsgCnt;
	unsigned long long ulAPSndtoAC_IPv6MsgErrCnt;

	unsigned long long ulStaSndtoAC_IPv4MsgCnt;
	unsigned long long ulStaSndtoAC_IPv4MsgErrCnt;
	unsigned long long ulStaSndtoAC_IPv6MsgCnt;
	unsigned long long ulStaSndtoAC_IPv6MsgErrCnt;
}MT_RCV_SND_MSG_T;

MT_RCV_SND_MSG_T gstRcvSndMsfCnt;//RecvFrom 和ComSend消息统计

typedef struct _MT_RUNTIME_T_
{
  UINT32 ulBeginTime;
  UINT32 ulEndTime;
}MT_RUNTIME_T;

//CAPWAP协议有一个前导头，用于快速识别版本和报文分类。
//         0 1 2 3 4 5 6 7
//        +-+-+-+-+-+-+-+-+
//        |Version| Type  |
//        +-+-+-+-+-+-+-+-+
typedef struct _CAPWAP_MSG_PREAMBLE_T_
{
	/*0:CAPWAP Header.Version:4位数据，表示版本，本规范中固定为0。Type:4位数据，表示报文类型，支持的值如下:0 ->表示CAPWAP协议头为非加密;1 ->表示CAPWAP协议头为DTLS加密.*/
  UINT8 ucVersion;
}CAPWAP_MSG_PREAMBLE_T;

//CAPWAP协议头如下图所示，
/*
                  0                                       1                                       2                                                  3
    0   1    2   3   4   5   6   7    8   9   0   1    2    3   4   5    6   7   8   9   0   1    2    3   4   5    6   7   8   9   0   1
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
typedef struct _CAPWAP_MSG_HEADER_T_
{
  UINT8 ucHdLen;//表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
  UINT8 aucHdFlag[2];//保留，必须为0。
  UINT16 usFragID;//分片ID，用于分片报文重组。
  UINT16 usFragOffset;//当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
  //  UINT8 ucResv;
}CAPWAP_MSG_HEADER_T;

typedef struct _CAPWAP_MSG_CONTROL_HEADER_T_
{
  UINT8 aucMsgTypeEnterPrsNum[3];
  UINT8 aucMsgTypeEnterPrsSpecific;
  UINT8 ucSeqNum;
  UINT8 ucFlag;
  UINT16 usMsgElemnetLen;
}CAPWAP_MSG_CONTROL_HEADER_T;

//CAPWAP控制报文如下
//0                   1                   2                   3
//0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|                       Message Type                            |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|    Seq Num    |        Msg Element Length     |     Flags     |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//| Msg Element [0..N] ...
//+-+-+-+-+-+-+-+-+-+-+-+-+

typedef struct _MSG_ELEMNT_T_
{
  UINT16 usMsgElemntType;
  UINT16 usMsgElemntLen;
  UINT8 aucMsgElemntValue[MSG_ELEMENT_VALUE_LEN];//usMsgElemntLen的实际值
}MSG_ELEMNT_T;

typedef struct _CAPWAP_MSG_T_
{
  UINT8 ucPreamble;
  UINT8 ucHdLen;//表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
  UINT8 aucHdFlag[2];//保留，必须为0。
  UINT16 usFragID;//分片ID，用于分片报文重组。
  UINT16 usFragOffset;//当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
  UINT8 aucMsgTypeEnterPrsNum[3];
  UINT8 aucMsgTypeEnterPrsSpecific;
  UINT8 ucSeqNum;
  UINT16 usMsgElemnetLen;
  UINT8 ucFlag;
  //  CAPWAP_MSG_CONTROL_HEADER_T stCtrlHeader;
  UINT8 aucMsgElemntBuf[MSG_ELEMENT_VALUE_LEN];
}__attribute__ ((packed)) CAPWAP_MSG_T;

extern INT32 glSockId;
//和CPM交互的SOCKET标识
extern INT32 glCpmSockId;

//和UPM交互的 SOCKET标识
extern INT32 glUpmSockId;

extern UINT32 gulOnlineApNum;
extern UINT32 gulOnlineStaCnt;

//和DHCP交互的 SOCKET标识
extern INT32 glDhcpSockId;

//系统默认配置参数
extern UINT32 gulCmdStartApNum; /*default value*/
extern UINT32 gulStaNumPerAp; /*Station number per WTP*/

typedef enum
{
  MT_FALSE = 0,
  MT_TRUE = 1
}MTBool;

typedef enum _BIT_
{
  BIT_ZERO = 0,
  BIT_ONE = 1
}MT_BIT;

typedef enum
{
  PERIOD_EVENT = 0,
  DEL_STA_EVENT = 1
}WTP_EVENT_FLAG;

typedef int MTSocket;

//系统配置信息
typedef struct _SYSCONF_PARA_
{
  UINT32 ulTriggerTime;                       //触发时间(ms)
  UINT32 ulApTriggerNum;                      //触发AP的个数
  UINT32 ulStaTriggerNum;                     //触发STA的个数
  UINT32 ulApHoldTime;                        //AP HOLD TIME
  UINT8 ucStaPolicy;                          //STA介入策略：0－>AP优先；1－>STA优先
  UINT8 aucDevName[MT_DEV_NAME_LEN];          //网卡设备名称
  UINT32 ulApOnlineFlag;                      //AP Always Online标签,0：AP自行下线，1：AP永远在线
  UINT32 ulTestMod;                           //测试模式，0：程序自行构建消息，参数从AP配置信息中获取,1：采用消息模板，
  UINT32 ulStaHoTrgrPeriod;                   //STA切换触发周期
}MT_SYSCONF_PARA_T;

typedef struct _AC_CONF_INFO_T_
{
  UINT8 aucAcName[MT_AC_NAME_LEN];            //AC Name
  UINT32 ulAcApCapabolity;                    //AC的AP容量；1024、2048、4096
  UINT32 ulAcIpType;                          //AC的IP地址类型；0:IPv4,1:IPv6
  UINT8  aucAcIpv4AddStr[MT_IPV4ADDR_STR_LEN];//AC的IPv4地址;
  UINT8  aucAcIpv6AddStr[MT_IPV6ADDR_STR_LEN];//AC的IPv6地址;
}MT_AC_CONF_INFO_T;

typedef struct _STA_CONF_INFO_T_
{
  UINT8 ucVlanFlag;                               //是否使用VLAN
  UINT8 ucStaQoSFlag;                             //STA QoS Flag:0->No QoS;1->With QoS(0x38)
  UINT8 ucStaTunnelMode;                          //STA隧道模式:0->IPv4 to IPv4;1->IPv4inIpv6;2->IPv6inIPv4;3->IPv6toIPv6
  UINT8 aucStaIpv4GwAddrStr[MT_IPV4ADDR_STR_LEN]; //STA网关IPV4
  UINT8 aucStaIpv6GwAddrStr[MT_IPV6ADDR_STR_LEN]; //STA网关IPV6
  UINT8 aucStaSrvIpv4AddrStr[MT_IPV4ADDR_STR_LEN];//STA服务器IPV4
  UINT8 aucStaSrvIpv6AddrStr[MT_IPV6ADDR_STR_LEN];//STA服务器IPV6
  UINT8 aucStaSrvMacStr[MT_MAC_STR_LEN];          //SRV MAC
}MT_STA_CONF_INFO_T;

MT_SYSCONF_PARA_T gstMtSysConf;             //系统配置信息
MT_AC_CONF_INFO_T gstAcConfInfo;            //AC配置信息
MT_STA_CONF_INFO_T gstStaConfInfo;          //STA配置信息

//DHCPv4状态
typedef enum _DHCPV6_STATE_EN_
{
  DHCPV6_INIT_STATE = 0,
  WAIT_DHCPV6_ADVERTISE_STATE = 1,
  WAIT_DHCPV6_REPLY_STATE = 2,
  DHCPV6_SUCCESS_STATE = 3,
  UNKNOWN_DHCPV6_STATE = 4
} DHCPV6_STATE_ENUM;

//DHCPv4状态
typedef enum _DHCPV4_STATE_EN_
{
  DHCPV4_INIT_STATE = 0,
  WAIT_DHCP_OFFER_STATE = 1,
  WAIT_DHCP_ACK_STATE = 2,
  DHCPV4_SUCCESS_STATE = 3,
  UNKNOWN_DHCPV4_STATE = 4
} DHCPV4_STATE_ENUM;

typedef enum _HANDOVER_STATE_EUNM_
{
	HANDOVER_INIT_STATE = 10,
	WAIT_HDOVER_AUTH_RSP_STATE = 11,//Handover切换状态
	WAIT_HDOVER_REASSOC_RSP_STATE = 12,//Handover切换状态
	HANDOVER_SUCCESS = 13,
	HANDOVER_FAIL = 14,
	UNKNOWN_HANDOVER_STATE = 15
}HANDOVER_STATE_ENUM;

//STA认证状态
typedef enum _AUTH_STATE_EN_
{
  AUTH_INIT_STATE = 0,
  WAIT_80211_AUTH_RSP_STATE = 1,
  WAIT_80211_ASSOC_RSP_STATE = 2,
  ASSOCIATION_SUCESS_STATE = 3,
  ASSOCIATION_FAIL = 4,
  UNKNOWN_AUTH_STATE = 5
}AUTH_STATE_ENUM;

//STA的实例结构
typedef struct
{
	DHCPV4_STATE_ENUM enDhcpState;
	DHCPV6_STATE_ENUM enDhcpv6State;
	AUTH_STATE_ENUM enAuthState;
	HANDOVER_STATE_ENUM enHandOverState;
	UINT32 ulStaHoldTimerId;
	UINT8 auStaMac[MT_MAC_LENTH];
	UINT8 aucStaIpV4Addr[MT_IPV4_ADDR_LEN];
	UINT8 aucStaGwIpv4Addr[MT_IPV4_ADDR_LEN];
	UINT8 aucStaIpV6Addr[MT_IPV6_ADDR_LEN];
	UINT8 aucStaGwIpv6Addr[MT_IPV6_ADDR_LEN];
	UINT8 auGwMac[MT_MAC_LENTH];
	UINT32 ulStaHdOverTimerId;
	UINT8 aucBssId[MT_MAC_LENTH];
	UINT8 aucCrrntAp[MT_MAC_LENTH];
#define MT_STA_REAUTH_INTERVAL_CNT 20
	UINT32 ulStaTrgPrdCnt;//在全部STA的上线周期内，对于未上线的STA重新发起认证过程
	MT_RUNTIME_T stStaOnLineTm;
	UINT32 ulHomeApId;//STA接入的APIndex；
	UINT32 ulForeignApId;//STA切换的APIndex；
	UINT32 ulStaLocalApId;//AP在收到Add StaconfigReq时的APID；在收到Del StaconfigReq时为0xFFFFFFFF
	UINT8 ucWlanId;//用户接入的WLAN_ID，在用户的接入过程中唯一，且不变
} STA_INSTANCE_T;

//AP状态
typedef enum
{
  INIT_STATE = 0,
  WAIT_DISCOVER_RSP_STATE = 1,
  WAIT_JOIN_RSP_STATE = 2,
  WAIT_CONFIG_STATUS_RSP_STATE = 3,
  WAIT_CHANGE_STATE_RSP_STATE = 4,
  RUN_STATE = 5,
  UNKNOWN_AP_STATE = 6
} AP_STATE_ENUM;

//AP的实例结构
typedef struct
{
  AP_STATE_ENUM enState;
  UINT8 auApMac[MT_MAC_LENTH];
  UINT8 aucApIpV4[MT_IPV4_ADDR_LEN];
  UINT8 aucApGwAddr[MT_IPV4_ADDR_LEN];
  UINT32 ulIpv4Prefix;
  UINT8 aucApIpV6[MT_IPV6_ADDR_LEN];
  UINT32 ulIpv6Prefix;
  UINT32 ulStaCnt;
  MTBool bKeepAliveFlag;//KeepAlive标识
  UINT32 ulApHoldTimerId;
  UINT32 ulEchoTimerId;
#define MT_WTP_ECHO_DETECT_CNT 3
  UINT32 ulEchoReqNum;//统计ECHO REQ消息个数，约定，连续发送3次ECHO无响应，即认为AC已将该AP删除，当即对模拟呼叫侧的AP信息进行清零处理
#define MT_WTP_REDISCOVERY_INTERVAL_CNT 800
  UINT32 ulPrdTrgCnt;
  UINT32 ulDiscvCnt;//AP尝试上线的次数
  MT_RUNTIME_T stEchoTm;
} AP_INSTANCE_T;

AP_INSTANCE_T  gastAp[MT_AP_MAX_NUM];//
STA_INSTANCE_T gastStaPara[MT_SYS_STA_CAPABILITY];

/*消息统计值结构*/
typedef struct
{
  /*Ap Info Statistic*/
  UINT32 ulSendDiscoverReq;
  UINT32 ulRecvDiscoverRsp;
  UINT32 ulSendJoinReq;
  UINT32 ulRecvJoinRsp;
  UINT32 ulSendConfigStatusReq;
  UINT32 ulRecvConfigStatusRsp;
  UINT32 ulSendChangeStateReq;
  UINT32 ulRecvChangeStateRsp;
  UINT32 ulRecvConfigUpdateReq;
  UINT32 ulSendConfigUpdateRsp;
  UINT32 ulRecvAddWlanReq;
  UINT32 ulSendAddWlanRsp;
  UINT32 ulSendWTPEventReq;
  UINT32 ulRecvWtpEventRsp;
  UINT32 ulApUpFailureCount;
  UINT32 ulApUpSuccessCount;
  UINT32 ulApRecvResetReq;
  UINT32 ulAPSendResetRsp;
  UINT32 ulApSendKeepAlive;
  UINT32 ulApRecvKeepAlive;
  UINT32 ulApSendEchoReq;
  UINT32 ulApRecvEchoRsp;
  UINT32 ulApSendReset;
  UINT32 ulEchoTmOvResetAp;

  UINT32 ulSend80211AuthReq;
  UINT32 ulRecv80211AuthRsp;
  UINT32 ulSend80211AssocReq;
  UINT32 ulRecv80211AssocRsp;
  UINT32 ulRecvAddStaConfigReq;
  UINT32 ulSendAddStaConfigRsp;
  UINT32 ulRecvDelStaConfigReq;
  UINT32 ulSendDelStaConfigRsp;

  UINT32 ulSend80211AuthReqHO;
  UINT32 ulRecv80211AuthRspHO;
  UINT32 ulSend80211ReAssocReq;
  UINT32 ulRecv80211ReAssocRsp;
  UINT32 ulRecvAddStaConfigReqHO;
  UINT32 ulSendAddStaConfigRspHO;
  UINT32 ulRecvDelStaConfigReqHO;
  UINT32 ulSendDelStaConfigRspHO;

  UINT32 ulAssociationSuccess;
  UINT32 ulReAssociationSuccess;
  UINT32 ulAssociationFail;
  UINT32 ulAssoctnTmOut;
  UINT32 ulReAssociationFail;
  UINT32 ulRecvDeAuth;
  UINT32 ulSendDeAuth;
//  UINT32 ulRecvDelSta;
//  UINT32 ulSendDelSta;
//  UINT32 ulResetDelSta;
  UINT32 ulStaSendArpReq;
  UINT32 ulStaRcvArpReply;
  UINT32 ulStaRcvArpReq;
  UINT32 ulStaSendArpReply;
  UINT32 ulStaSendPingReq;
  UINT32 ulStaRcvPingReply;
  UINT32 ulStaRecvPingReq;
  UINT32 ulStaSendPingReply;
  UINT32 ulStaSendPing6Req;
  UINT32 ulStaRcvPing6Reply;
  UINT32 ulStaRcvPing6Req;
  UINT32 ulStaSendPing6Reply;
  UINT32 ulStaSendDhcpDiscover;
  UINT32 ulStaRcvDhcpOffer;
  UINT32 ulStaSendDhcpReq;
  UINT32 ulStaRcvDhcpAck;
  UINT32 ulStaSendDhcpv6Solicit;
  UINT32 ulStaRcvDhcpv6Advertise;
  UINT32 ulStaSendDhcpv6Req;
  UINT32 ulStaRcvDhcpv6Reply;
} MUTITEST_STATICS_T;

//错误消息统计
typedef struct _ERR_MSG_STATICS_T_
{
	UINT32 ulSendDiscoverReqErr;
	UINT32 ulRecvDiscoverRspErr;
	UINT32 ulSendJoinReqErr;
	UINT32 ulRecvJoinRspErr;
	UINT32 ulSendConfigStatusReqErr;
	UINT32 ulRecvConfigStatusRspErr;
	UINT32 ulSendChangeStateReqErr;
	UINT32 ulRecvChangeStateRspErr;
	UINT32 ulRecvConfigUpdateReqErr;
	UINT32 ulSendConfigUpdateRspErr;
	UINT32 ulRecvAddWlanReqErr;
	UINT32 ulSendAddWlanRspErr;
	UINT32 ulSendIamgeDataReqErr;
	UINT32 ulSendIamgeDataRspErr;
	UINT32 ulApUpFailureCountErr;
	UINT32 ulApRecvResetReqErr;
	UINT32 ulAPSendResetRspErr;
	UINT32 ulSendWTPEventReqErr;
	UINT32 ulRecvWtpEventRspErr;
	UINT32 ulApSendEchoReqErr;
	UINT32 ulApRecvEchoMsgErr;
	UINT32 ulApRecvEchoRspErr;
	UINT32 ulApSendKeepAliveErr;
	UINT32 ulApRecvKeepAliveErr;
	UINT32 ulApSendResetErr;

	UINT32 ulSendEventReq_IE37IE56Err;
	UINT32 ulSendEventReq_IE37IE57Err;
	UINT32 ulSendEventReq_IE37IE6XErr;
	UINT32 ulSendEventReq_IE37IE7XErr;
	UINT32 ulSendEventReq_IE37IE8XErr;
	UINT32 ulSendEventReq_IE37IE133134138Err;
	UINT32 ulSendEventReq_IE37IE5859Err;

	UINT32 ulSend80211AuthReqErr;
	UINT32 ulRecv80211AuthRspErr;
	UINT32 ulSend80211AssocReqErr;
	UINT32 ulRecv80211AssocRspErr;
	UINT32 ulRecvAddStaConfigReqErr;
	UINT32 ulSendAddStaConfigRspErr;
	UINT32 ulRecvDelStaConfigReqErr;
	UINT32 ulSendDelStaConfigRspErr;

	UINT32 ulSend80211AuthReqHOErr;
	UINT32 ulRecv80211AuthRspHOErr;
	UINT32 ulSend80211ReAssocReqErr;
	UINT32 ulRecv80211ReAssocRspErr;
	UINT32 ulRecvAddStaConfigReqHOErr;
	UINT32 ulSendAddStaConfigRspHOErr;
	UINT32 ulRecvDelStaConfigReqHOErr;
	UINT32 ulSendDelStaConfigRspHOErr;

	UINT32 ulSendDhcpDiscoverErr;
	UINT32 ulRcvDhcpOfferErr;
	UINT32 ulSendDhcpReqErr;
	UINT32 ulRcvDhcpAckErr;
	UINT32 ulSendArpReqErr;
	UINT32 ulRecvArpReplyErr;
	UINT32 ulRecvArpReqErr;
	UINT32 ulSendArpReplyErr;
	UINT32 ulSendPingReqErr;
	UINT32 ulRecvPingReplyErr;
	UINT32 ulRecvPingReqErr;
	UINT32 ulSendPingReplyErr;

	UINT32 ulSendDhcpv6SolicitErr;
	UINT32 ulRcvDhcpv6AdvertiseErr;
	UINT32 ulSendDhcpv6ReqErr;
	UINT32 ulRcvDhcpv6ReplyErr;
	UINT32 ulSendPing6ReqErr;
	UINT32 ulRcvPing6ReplyErr;
	UINT32 ulRcvPing6ReqErr;
	UINT32 ulSendPing6ReplyErr;

	UINT32 ulRecvDeAuthErr;
	UINT32 ulSendDeAuthErr;

	UINT32 ulRcv5246UnknwnMsg;
	UINT32 ulRcv5246MsgErr;
	UINT32 ul5246MsgHead_NULL;
	UINT32 ulRcv5246MsgErrApId;
	UINT32 ulRcv5246MsgErrStaId;

	UINT32 ulRcv5247UnknwnMsg;
	UINT32 ulRcv5247MsgErr;
	UINT32 ul5247MsgHead_NULL;
	UINT32 ulRcv5247MsgErrApId;
	UINT32 ulRcv5247MsgErrStaId;
	UINT32 ulRcvOtherMsgErr;
}MT_ERR_MSG_STATICS_T;

MUTITEST_STATICS_T gstMutiTest_Statics;
MT_ERR_MSG_STATICS_T gstErrMsgStatics;//错误消息统计

CPSS_COM_PID_T gstMutiTestPid ;

/*AP的CAPWAP Update PROC*/
typedef struct __ImageUpdate_Statics__
{
  UINT32 ulApRcvUpdateReqNum;
  UINT32 ulApSndUpdateRspNum;
  UINT32 ulApSndImgdataReqNum;
  UINT32 ulAPRcvImgdataRspNum;
  UINT32 ulApRcvImgdataReqNum;
  UINT32 ulApSndImgdataRspNum;
  UINT32 ulApRcvResetReqNum;
  UINT32 ulApSndResetRspNum;
  UINT32 ulApRcvImgdataReqIE26Num;
} MULTITEST_CAPWAP_IMG_UPDATE_T;

MULTITEST_CAPWAP_IMG_UPDATE_T gastCapImgUpdateStatic[MT_AP_MAX_NUM];

/* CAPWAP Img UPDATE全局统计信息 */
typedef struct __G_CAPWAP_IMG_UPDATE_STATICS_
{
	UINT32 ulApRcvUpdateReqNum;
	UINT32 ulApSndUpdateRspNum;
	UINT32 ulApSndImgdataReqNum;
	UINT32 ulAPRcvImgdataRspNum;
	long long ulApRcvImgdataReqNum;
	long long ulApSndImgdataRspNum;
	UINT32 ulApRcvResetReqNum;
	UINT32 ulApSndResetRspNum;
	UINT32 ulApRcvImgdataReqIE26Num;
	UINT32 ulAPCapwapImgSucNum;
}CAPWAP_IMG_UPDATE_STATICS_T;

CAPWAP_IMG_UPDATE_STATICS_T gstCapImgUpdateStat;

/*AP发现类型*/
typedef enum
{
  UNKNOWN_DISCV = 0,
  STATIC = 1,
  DHCP = 2,
  DNS = 3,
  BROADCAT = 4,
  RECNNCT = 5
}DISCOVERY_TYPE_E;

DISCOVERY_TYPE_E genApDiscovtype;

//AP配置信息
typedef struct __AP_CONF_PARA__
{
  UINT32 ulApIndex;                               //AP标识
  UINT8 aucApMacStr[MT_MAC_STR_LEN];              //MAC地址
  UINT8 aucApIpV4AddrStr[MT_IPV4ADDR_STR_LEN];      //IPV4地址
  UINT8 aucApIpV6AddrStr[MT_IPV6ADDR_STR_LEN];      //IPV6地址
  DISCOVERY_TYPE_E enDiscovType;                  //发现类型
  UINT8 aucApmodule[AP_MODULE_NAME_LEN];          //型号
  UINT8 aucApImgFileName[AP_IMG_FILE_NAME_LEN];   //Img文件名称
  UINT8 aucApImgVer[AP_IMG_VER_NAME_LEN];         //版本号
  UINT8 acuSerialNo[AP_SERIAL_NO_LEN];            //序列号
  UINT32 ulNetCarrierInc;                         //WTP Board data Vendor
  UINT8 aucBoardID[AP_BOARD_ID_LEN];              //WTP板卡标识
  UINT8 aucBoardRevision[AP_BOARD_REVISON_LEN];   //板卡的修订版本
  UINT8 aucHdWareVer[12];                         //硬件版本
  UINT8 aucActiveSftwareVer[12];                  //激活的软件版本
  UINT8 ucRadioType;                              //Radio 类型:bit3:11n;bit211g;bit1:11a;bit0:11g
  UINT8 ucWtpMacType;                             //WTP MAC类型:0->Local MAC,AP数据报文仅支持802.3frame封装数据;1 ->Split MAC,AP数据报文仅支持802.11frame封装数据;2 ->Both,AP数据报文既支持802.3frame封装数据，又支持802.11frame封装数据。
  UINT8 ucWtpFrmTunnlMod;                         //WtpFrameTunnelMod:bit3:Native Frame Tunnel Mode;bit2:802.3 Frame Tunnel Mode;bit1:local Bridging;bit0:Resvd
  UINT32 ulApVlan;                                //AP管理VLAN
  UINT32 ulStaVlan;                               //STA业务VLAN
}MT_AP_CONF_PARA_T;

MT_AP_CONF_PARA_T gastApconf[MT_AP_MAX_NUM];//AP配置信息

typedef struct _MT_CAPWAPMSG_T_
{
  UINT8 aucBuff[MT_CAPWAPMSG_BUFF_LEN];
  UINT32  ulBufLen;
}MT_CAPWAPMSG_T;

typedef struct _UDP_HEADER_T_
{

}MT_UDP_HEADER;

typedef struct _ETH_HDR_T_
{
  UINT8 aucDstMac[MT_MAC_LENTH];
  UINT8 aucSrcMac[MT_MAC_LENTH];
  UINT16 usProtclType;
}MT_ETH_HDR_T;

typedef struct _STA_VLAN_HEADER_T_
{
  UINT16 usPriority;//Best Effort(default 0),CFI,VLAN_ID
  UINT16 usType;//IPv6(0x86dd)
}MT_VLAN_HEADER_T;

typedef struct _STA_IPV4_HEADER_T_
{
  UINT8 ucIpVerType;//IP Version Type(IPV4 = 0x45)
  UINT8 ucDiffSerFd;//Different Service Field
  UINT16 usTotalLen;
  UINT16 usIdentfier;
  UINT16 usFragOffset;
  UINT8 ucTtl;//time to live
  UINT8 ucProtoType;//ICMP(0x01)
  UINT16 usChkSum;//CheckSum
  UINT8 aucSrcIpaddr[MT_IPV4_ADDR_LEN];//Source IP address
  UINT8 aucDstIpAddr[MT_IPV4_ADDR_LEN];//Destination Ip Address
}MT_IPV4_HEADER_T;

typedef struct _UDP_HDR_T_
{
  UINT16 usSrcPort;
  UINT16 usDstPort;
  UINT16 usPldLen;
  UINT16 usChkSum;
}MT_UDP_HDR_T;

typedef struct _STA_ICMP_T_
{
  UINT8 ucIpProtoType;//IP Protocol Type (ICMP = 8)
  UINT8 ucIcmpCode;
  UINT16 usIcmpChkSum;//CheckSum
  UINT16 usBeIdentifier;
  UINT16 usSeqNum;//Sequence Number
  UINT8  aucData[32];
}MT_STA_ICMPV4_T;

typedef struct _ICMPV6_T_
{
  UINT8 ucIpProtoType;//IP Protocol Type (ICMP = 8)
  UINT8 ucIcmpCode;
  UINT16 usIcmpChkSum;//CheckSum
  UINT16 usIdentifier;
  UINT16 usSeqNum;//Sequence Number
  UINT8  aucData[32];
}MT_ICMPV6_T;

typedef struct _ARP_REQ_T_
{
  UINT16 usHWType;
  UINT16 usProtclType;
  UINT8 ucHWSize;
  UINT8 ucProtclSize;
  UINT16 usOpcode;
  UINT8 aucSrcMac[MT_MAC_LENTH];
  UINT8 aucSrcIpv4Addr[MT_IPV4_ADDR_LEN];
  UINT8 aucDstMac[MT_MAC_LENTH];
  UINT8 aucDstIpv4Addr[MT_IPV4_ADDR_LEN];
}MT_ARP_REQ_T;

typedef struct _STA_IPV6_HEADER_T_
{
  UINT32 ulIPVerTraffic;//IP Version Type and Traffic class
  UINT16 usPayloadLen;//Different Service Field
  UINT8 ucNextHdr;
  UINT8 ucHopLmt;
  UINT8 aucSrcIpV6addr[MT_IPV6_ADDR_LEN];//Source IP address
  UINT8 aucDstIpV6Addr[MT_IPV6_ADDR_LEN];//Destination Ip Address
}MT_IPV6_HEADER_T;

typedef struct _ICMPV6_OPTIONT_T_
{
  UINT8 ucType;
  UINT8 ucLen;
  UINT8 aucLnkLayerAddr[MT_MAC_LENTH];
}MT_ICMPV6_OPTION;

typedef struct _ICMPV6_NEIGHBOR_ADVERTISEMENT_T_
{
  UINT8 ucProtclType;
  UINT8 ucCode;
  UINT16 usChkSum;
  UINT32 ulFlag;
  UINT8 aucDstIpv6Addr[MT_IPV6_ADDR_LEN];
  MT_ICMPV6_OPTION stIcmpv6Opt;
}MT_ICMPV6_NEIGHBOR_ADVERTISEMENT_T;

typedef struct _DHCP_OPTION_T_
{
  UINT8 ucOption;
  UINT8 ucOptionLen;
#define MT_DHCP_OPTION_VALUE_LEN 64
  UINT8 aucOptnVal[MT_DHCP_OPTION_VALUE_LEN];
}MT_DHCP_OPTION_T;

typedef struct _BOOTSTRAP_HDR_T_
{
  UINT8 ucMsgType;
  UINT8 ucHWType;
  UINT8 ucHWAddrLen;
  UINT8 ucHops;
  UINT32 ulTransacID;
  UINT16 usSecondElaps;
  UINT16 usBootFlag;
  UINT8 aucClientIpAddr[MT_IPV4_ADDR_LEN];
  UINT8 aucYourIpAddr[MT_IPV4_ADDR_LEN];
  UINT8 aucNxtSrvIpAddr[MT_IPV4_ADDR_LEN];
  UINT8 aucRelayAgntIpAddr[MT_IPV4_ADDR_LEN];
  UINT8 aucClientMacAddr[MT_MAC_LENTH];
#define MT_HARDWAREPADDING_LEN 10
  UINT8 aucHwAddrPadding[MT_HARDWAREPADDING_LEN];
#define MT_DHCP_SERVER_HOST_NAME_LEN 64
  UINT8 aucSrvName[MT_DHCP_SERVER_HOST_NAME_LEN];
#define MT_DHCP_BOOT_FILE_NAME_LEN 128
  UINT8 aucBtFileName[MT_DHCP_BOOT_FILE_NAME_LEN];
#define MT_MAGICCOOKIE_LEN 4
  UINT8 ucMagicCookie[MT_MAGICCOOKIE_LEN];//63 82 53 63
}MT_BOOTSTRAP_HDR_T;

//typedef struct _DHCPV4REQ_STA_INFO_T_
//{
//	UINT32 ulStaIndex;
//	UINT8 aucDhcpv4SrvIpAddr[MT_IPV4_ADDR_LEN];
//	UINT8 aucDhcpv4ReqrdIpAddr[MT_IPV4_ADDR_LEN];
//}MT_DHCPV4REQ_STA_INFO_T;

typedef struct _DHCPV6_OLV_T_
{
	UINT16 usOptionID;
	UINT16 usLen;
#define MT_DHCPV6_OPTION_VALUE_LEN 64
	UINT8 aucVal[MT_DHCPV6_OPTION_VALUE_LEN];
}MT_DHCPV6_OLV_T;

typedef struct _DHCPV6_T_
{
	UINT32 ulMsgTypeTransId;//|0~7MsgType|8~23ApID|24~31Staid|
#define MT_DHCPV6_VALUE_LEN 256
	UINT8 aucDhcpv6OptionVal[MT_DHCPV6_VALUE_LEN];
	}MT_DHCPV6_T;

//typedef struct _DHCPV6_STA_ADDR_INFO_
//{
//	UINT32 ulStaIndex;
//	UINT8 aucStaIpv6Addr[MT_IPV6_ADDR_LEN];
//}MT_DHCPV6_STA_ADDR_INFO;

typedef struct _PING6_REPLY_STA_INFO_
{
	//UINT32 ulStaIndex;
	UINT16 usIdentifier;
	UINT16 usSeqNum;
	UINT8 aucDstMac[MT_MAC_LENTH];
	UINT8 aucDstIpv6Addr[MT_IPV6_ADDR_LEN];//STA IPv6 Address
}MT_PING6_REPLY_STA_INFO;

typedef struct _PING_REPLY_STA_INFO_
{
	//UINT32 ulStaIndex;
	UINT16 usIdentifier;
	UINT16 usSeqNum;
	UINT8 aucDstMac[MT_MAC_LENTH];
	UINT8 aucDstIpv4Addr[MT_IPV4_ADDR_LEN];//STA IPv4 Address
}MT_PING_REPLY_STA_INFO;

typedef struct _T1024_VALUE_T_
{
	UINT8 ucRadioID;//范围1-31。
	UINT8 ucWlanID;//WLAN的标识符，范围1-16。
	UINT16 usCapability;
	/*
	包含了AP上WLAN的Capability信息，这些信息会携带在AP发出的Probe Response和信标帧中。16bit中每个bit都代表不同的AP的属性，详细描述见[IEEE.802-11.2007]。格式定义如下：
	        0                   1
	        0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5
	       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	       |E|I|C|F|P|S|B|A|M|Q|T|D|V|O|K|L|
	       +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
	      E (ESS):   扩展服务集。
	      I (IBSS):   独立的基本服务集。如Ad hoc模式。
	      C (CF-Pollable):   Contention Free Pollable，详细描述见[IEEE.802-11.2007]。
	      F (CF-Poll Request):   详细描述见[IEEE.802-11.2007]。
	      P (Privacy):   WLAN的私有要求，详细描述见[IEEE.802-11.2007]。
	      S (Short Preamble):   WLAN是否支持短前导码，详细描述见[IEEE.802-11.2007]。
	      B (PBCC):  WLAN是否支持PBCC，详细描述见[IEEE.802-11.2007]。
	      A (Channel Agility):   AP上的WLAN是否支持高速直接序列扩频（HR/DSSS），详细描述见[IEEE.802-11.2007]。
	      M (Spectrum Management):   频谱管理，详细描述见[IEEE.802-11.2007]。
	      Q (QoS):   服务质量，详细描述见[IEEE.802-11.2007]。
	      T (Short Slot Time):   短碰撞槽时间，详细描述见[IEEE.802-11.2007]。
	      D (APSD):   自动省电模式，详细描述见[IEEE.802-11.2007]。
	      V (Reserved):   保留字段，通常设置为0。
	      O (DSSS-OFDM):   正交频率多重分割的直接序列扩频，详细描述见[IEEE.802-11.2007]。
	      K (Delayed Block ACK):   拥塞延迟回复，详细描述见[IEEE.802-11.2007]。
	      L (Immediate Block ACK):   拥塞立即回复，详细描述见[IEEE.802-11.2007]。
	*/
	UINT8 ucKeyIndex;//分配的key的序列号。
	UINT8 ucKeyStatus;
	/*
	表示key的状态和用途，当Key Length为0时，Key Status无意义。取值有以下几种：
	      0 -  表示后面的Key字段是多播key。
	      1 -  表示WLAN采用shared WEP加密，是静态的key，用于所有用户数据（单播和多播）的加密。
	      2 -  表示AC将要开始更新AP和用户的GTK。
	      3 -  表示AC已经对GTK更新完成，并且发送的广播包不必再用GTK复制和传输。
	*/
	UINT16 usKeyLen;//代表后面的Key的长度。
#define MT_T1024_KEY_LEN 256
	UINT8 aucKey[MT_T1024_KEY_LEN];//用来对无线的用户数据进行加密。按照加密的配置，会设置单播密钥和多播密钥，这里的密钥只用于多播密钥，密钥组则包含在RSN信息元素中。在这种情况下，密钥信息是通过Add Station消息元素和IEEE 802.11 Station Session Key消息元素的协商中确定的。当使用WEP加密时，这个字段是广播密钥；当使用TKIP加密，这个字段表示256-bit的GTK。
#define MT_GROUP_TSC_LEN 6
	UINT8 aucGrpTsc[MT_GROUP_TSC_LEN];// 表示更新过的组密钥的序列计数器。
	UINT8 ucQoS;
	/*
	AP的默认的Qos策略，针对网络中发送来的给没有设置WMM的用户的数据。
	      支持以下几种类型：
	      0 -  Best Effort
	      1 -  Video
	      2 -  Voice
	      3 -  Background
	*/
#define MT_AUTH_POLICY_OPENSYSTEM   0
#define MT_AUTH_POLICY_WEPSHAREDKEY 1
	UINT8 ucAuthType;//WLAN支持的认证策略:0->Open System;1->WEP Shared Key
#define MT_MAC_MOD_LOCALMAC 0
#define MT_MAC_MOD_SPLITMAC 1
	UINT8 ucMacMode;//表示AP绑定的WLAN支持的Local MAC或Split MAC策略，取值含义如下：0->Local MAC:WLAN为Local MAC模式;1->Split MAC:WLAN为Split MAC模式。
#define MT_TUNNEL_MOD_LOCAL_BRDG 0
#define MT_TUNNEL_MOD_DOT3_TUNNEL 1
#define MT_TUNNEL_MOD_DOT11_TUNNEL 2
	UINT8 ucTunnelMod;
	/* 表示AP对于无线侧的802.11帧的隧道转发模式。有以下几种：
	      0 - Local Bridging:   所有用户数据本地转发。
	      1 - 802.3 Tunnel:   所有用户数据本地转换为802.3帧发给AC，由AC转发数据。
	      2 - 802.11 Tunnel:   所有用户数据以802.11帧格式转发AC，由AC进行转换有线帧转发。
	*/
	UINT8 ucSuppressSsid;// 是否隐藏SSID。如果设置为0，则WLAN的网络名（SSID）将不对外广播，信标帧和Probe Response帧中不包含SSID信息；1的话则包含网络名信息。
	UINT8 aucSsid[MT_SSID_LEN];//WLAN的服务集标识符，是ASCII形式的字符串，不超过32个字符
}T1024_VALUE;

typedef struct _WLAN_INFO_T_
{
	UINT8 ucAuthType;
	UINT8 ucMacMode;
	UINT16 usKeyLen;
	UINT8 aucKey[MT_T1024_KEY_LEN];
	UINT8 ucTunnelMod;
	UINT8 aucSsid[MT_SSID_LEN];
	UINT8 ucSsidLen;
}MT_WLAN_INFO;

typedef struct _RADIO_INFO_T_
{
  UINT8 ucWlanNum;
  UINT8 aucWlanList[MT_WLAN_MAX_NUM];
//  UINT8 ucWlanId;
  MT_WLAN_INFO astWlanInfo[MT_WLAN_MAX_NUM];
}MT_RADIO_INFO;

typedef struct _AP_RADIO_WLAN_INFO_T_
{
	UINT8 ucRadioNum;
	UINT8 aucRadioList[MT_RADIO_MAX_NUM];
//	UINT8 ucRadioId;
	MT_RADIO_INFO astRadioInfo[MT_RADIO_MAX_NUM];
}MT_AP_RADIO_WLAN_INFO_T;

typedef struct _80211AUTHENTICATION_T_
{
	UINT16 usFranCtrl;
	UINT16 usDuration;
	UINT8 aucDstMacAdd[MT_MAC_LENTH];
	UINT8 aucSrcMAcAdd[MT_MAC_LENTH];
	UINT8 aucBssid[MT_MAC_LENTH];
	UINT16 usFragSeqNum;
}MT_80211AUTHENTICATION_T;

typedef struct _80211WLANMANAGMNTFRAME_FIXED_PARA_T_
{
 UINT16 usAuthAlgrm;
 UINT16 usAuthSeq;
 UINT16 usStatusCode;
}__attribute__ ((packed)) MT_80211WLANMANAGMNTFRAME_FIXED_PARA_T;

typedef struct _80211WLANMANAGMNTFRAME_TAGGED_PARA_T_
{
 UINT8 ucTagNum;
 UINT8 ucTagLen;
 UINT8 aucTagValue[256];
}MT_80211WLANMANAGMNTFRAME_TAGGED_PARA_T;

typedef struct _STA_AUTH_INFO_T_
{
	UINT32 ulApIndex;
	UINT32 ulStaIndex;
	UINT8 ucRadioId;
	UINT8 ucWlanId;
	UINT16 usAuthAlgrm;
	UINT16 usAuthSeq;
#define MT_STA_AUTH_CHALLENGE_TXT_LEN 256
	UINT8 aucChallengeTxt[MT_STA_AUTH_CHALLENGE_TXT_LEN];
	UINT8 ucChallengeTxtLen;
}MT_STA_AUTH_INFO;

typedef struct _STA_WLAN_INFO_T_
{
	UINT32 ulApIndex;
	UINT32 ulStaIndex;
	UINT8 ucRadioId;
	UINT8 ucWlanId;
}MT_STA_WLAN_INFO_T;

//	8021xAuthenticationReq for WPA_PSK
typedef struct _IEEE8021XAUTH_REQ_T_
{
	UINT8 ucVer;
	UINT8 ucType;
	UINT16 usLen;
	UINT8 ucDsrpType;
	UINT16 usKeyInfo;
	UINT16 usKeyLen;
	UINT32 ulRplCntr1;
	UINT32 ulRplCntr2;
	#define IEEE8021XAUTH_NONCE_LEN 32
	UINT8 aucNonce[IEEE8021XAUTH_NONCE_LEN];
	#define IEEE8021XAUTH_KEY_IV_LEN 16
	UINT8 aucKeyIV[IEEE8021XAUTH_KEY_IV_LEN];
	#define IEEE8021XAUTH_WPA_KEY_RSC_LEN 8
	UINT8 aucWpaKeyRsc[IEEE8021XAUTH_WPA_KEY_RSC_LEN];
	#define IEEE8021XAUTH_WPA_KEY_ID_LEN 8
	UINT8 aucWpaKeyId[IEEE8021XAUTH_WPA_KEY_ID_LEN];
	#define IEEE8021XAUTH_WPA_KEY_MIC_LEN 16
	UINT8 aucWpaKeyMic[IEEE8021XAUTH_WPA_KEY_MIC_LEN];
	UINT16 usWapKeyLen;
	#define IEEE8021XAUTH_WPA_KEY_LEN 128
	UINT8 aucWapKey[IEEE8021XAUTH_WPA_KEY_LEN];
}__attribute__ ((packed)) IEEE8021XAUTH_REQ_T;

typedef struct _8021X_EAPOL_INFO_T_
{
	UINT16 usKeyInfo;
	UINT32 ulReplCntr1;
	UINT32 ulReplCntr2;
	UINT8 aucNonce[IEEE8021XAUTH_NONCE_LEN];
	UINT8 aucWpaKeyMic[IEEE8021XAUTH_WPA_KEY_MIC_LEN];
	UINT32 ulWapKeyLen;
	UINT8 aucWapKey[IEEE8021XAUTH_WPA_KEY_LEN];
}MT_8021X_EAPOL_INFO_T;

typedef struct _WAI_AUTH_MSG_INFO_T_
{
	UINT16 usVer;
	UINT8 ucType;
	UINT8 ucSubType;
	UINT16 usRcvd;
	UINT16 usLen;
	UINT16 usSeqNum;
	UINT8 ucFragSeqNum;
	UINT8 ucFlag;
	UINT8 aucBuff[256];
}MT_WAI_AUTH_MSG_INFO_T;

typedef struct _WTP_EVENT_FLAG_T_
{
  UINT8 ucFlagL2IeId_5604;
  UINT8 ucFlagL2IeId_5621;
  UINT8 ucFlagL2IeId_5622;
  UINT8 ucFlagL2IeId_5623;
  UINT8 ucFlagL2IeId_5624;
  UINT8 ucFlagL2IeId_5625;
  UINT8 ucFlagL2IeId_5626;
  UINT8 ucFlagL2IeId_5627;
  UINT8 ucFlagL2IeId_5628;
  UINT8 ucFlagL2IeId_5629;
  UINT8 ucFlagL2IeId_5630;
  UINT8 ucFlagL2IeId_5631;
  UINT8 ucFlagL2IeId_5632;
  UINT8 ucFlagL2IeId_5633;
  UINT8 ucFlagL2IeId_5634;
  UINT8 ucFlagL2IeId_5635;
  UINT8 ucFlagL2IeId_5636;
  UINT8 ucFlagL2IeId_5637;
  UINT8 ucFlagL2IeId_5638;
  UINT8 ucFlagL2IeId_5639;
  UINT8 ucFlagL2IeId_5640;
  UINT8 ucFlagL2IeId_5641;
  UINT8 ucFlagL2IeId_5642;
  UINT8 ucFlagL2IeId_5643;
  UINT8 ucFlagL2IeId_5644;

  UINT8 ucFlagL2IeId_5721;
  UINT8 ucFlagL2IeId_5722;
  UINT8 ucFlagL2IeId_5723;
  UINT8 ucFlagL2IeId_5724;
  UINT8 ucFlagL2IeId_5725;
  UINT8 ucFlagL2IeId_5726;
  UINT8 ucFlagL2IeId_5727;
  UINT8 ucFlagL2IeId_5728;
  UINT8 ucFlagL2IeId_5729;
  UINT8 ucFlagL2IeId_5730;
  UINT8 ucFlagL2IeId_5731;
  UINT8 ucFlagL2IeId_5732;
  UINT8 ucFlagL2IeId_5733;
  UINT8 ucFlagL2IeId_5734;
  UINT8 ucFlagL2IeId_5735;
  UINT8 ucFlagL2IeId_5736;
  UINT8 ucFlagL2IeId_5737;
  UINT8 ucFlagL2IeId_5738;
  UINT8 ucFlagL2IeId_5739;
  UINT8 ucFlagL2IeId_5740;
  UINT8 ucFlagL2IeId_5741;
  UINT8 ucFlagL2IeId_5742;
  UINT8 ucFlagL2IeId_5743;

  UINT8 ucFlagL2IeId_58;
  UINT8 ucFlagL2IeId_59;
  UINT8 ucFlagL2IeId_60;
  UINT8 ucFlagL2IeId_61;
  UINT8 ucFlagL2IeId_62;
  UINT8 ucFlagL2IeId_63;
  UINT8 ucFlagL2IeId_64;
  UINT8 ucFlagL2IeId_65;
  UINT8 ucFlagL2IeId_66;
  UINT8 ucFlagL2IeId_67;
  UINT8 ucFlagL2IeId_68;
  UINT8 ucFlagL2IeId_71;
  UINT8 ucFlagL2IeId_72;
  UINT8 ucFlagL2IeId_78;
  UINT8 ucFlagL2IeId_79;
  UINT8 ucFlagL2IeId_80;
  UINT8 ucFlagL2IeId_81;
  UINT8 ucFlagL2IeId_82;
  UINT8 ucFlagL2IeId_83;
  UINT8 ucFlagL2IeId_84;
  UINT8 ucFlagL2IeId_85;
  UINT8 ucFlagL2IeId_86;
  UINT8 ucFlagL2IeId_87;
  UINT8 ucFlagL2IeId_88;
  UINT8 ucFlagL2IeId_107;
  UINT8 ucFlagL2IeId_133;
  UINT8 ucFlagL2IeId_134;
  UINT8 ucFlagL2IeId_138;
}MT_WTP_EVENT_FLAG_T;

MT_WTP_EVENT_FLAG_T gstWtpEventFlag;
MTBool gbHdOverFlag;//切换开关
extern void MT_Task_MsgDispctcher(void);
extern void MT_StaAccessAPfirst(void);
extern void MT_StaAccessSTAfirst(void);
extern UINT16 MT_GetSequenceNum(UINT16 vusSequence);
extern UINT32 MT_GetStaId(UINT32 vulStaId);
extern UINT16 MT_GetApCpmPort(UINT32 vulApIndex);
extern UINT16 MT_GetApUpmPort(UINT32 vulApIndex);
extern UINT32 MT_GetApIndexFromCpmPort(UINT32 vulApCpmPort);
extern UINT32 MT_GetApIndexFromUpmPort(UINT32 vulApUpmPort);

extern UINT8 GetHexValFromChar(char vChar);

extern MTBool MT_SendApMsg2AC(UINT32 vulApIndex,MTSocket vCwSock, UINT8 *vucCapwapBuf, UINT32 vulCapwaplen);//AP给CPM模块的消息发送函数
extern MTBool MT_SendStaMsg2AC(UINT32 vulApIndex,MTSocket vCwSock, UINT8 *vucCapwapBuf, UINT32 vulCapwaplen);//AP给UPM模块的消息发送函数
extern MTBool MT_ApSendMsg2DhcpSrv(UINT32 vulApIndex,MTSocket vCwSock, UINT8 *vucBtStrapBuf, UINT32 vulBtStrapBuflen);
extern UINT8* MT_GetApStateStr(UINT32 vulApStateVal);
extern UINT8* MT_GetStaHOStateStr(UINT32 vulStaHoStateVal);
extern UINT8* MT_GetStaAuthStateStr(UINT32 vulStaAuthStateVal);
extern void MT_GetCapwapMsg(UINT8 *vpMsgPathFileName, MT_CAPWAPMSG_T *vpstCapwapMsg);
extern void MT_GetBssID(UINT32 vulApIndex,UINT8 vucRadioId, UINT8 vucWlanId,UINT8 *vpBssId);
extern UINT16 MT_CalcIpChksm(UINT8 *vaucBuf,UINT16 vusbufLen);
extern UINT16 MT_CalcIPv6CheckSum(UINT16 *addr, UINT32 len);
extern void MT_CalcIcmpCksum(UINT8 *chksum, UINT8 *nptr, int nlen);
extern UINT16 MT_GetIcmpv6checksum(UINT8 *buffer,INT32 len);
extern void showstart(void);
extern void showstop(void);
extern void wtpeventstart(UINT32 vulL2IeId);
extern void wtpeventstop(UINT32 vulL2IeId);
extern UINT32 findDestAp(UINT32 vulSrcApIndex);
extern UINT32 findNextAPforAccess(UINT32 vulApIdx);
extern UINT32 getApIDbyBssid(UINT32 vulStaIndex);
extern UINT32 getStaIndexByMac(UINT8 *vpStaMac);
extern void starthandover(void);
extern void stopthandover(void);
extern UINT32 staOnlineCnt(void);
extern UINT32 apOnlineCnt(void);
extern void MT_ApLog(UINT32 vulApIdx, char *vpLogInfo);
extern void MT_StaLog(UINT32 vulStaIndex, char *vpLogInfo);
extern void MT_RunLog(UINT8* vpBuf, UINT32 vulBufLen, char *vpLogInfo);
extern void TransMacToStr(const unsigned char *vpMac, char *vpMacStr);
extern INT32 GetMacFromStr(const UINT8 *vpMacStr, UINT8 *vaucMac);
extern void GetIpFromStr(UINT32 vulAfInet,UINT8 *vpIpStr,UINT8 *vaucIpAddr);
extern void stadhcpstart(void);
extern void stadhcpstop(void);
UINT32 gulPeriodTriggerStaNum;
UINT32 gulApMaxNum;//MT_AP_MAX_NUM
UINT32 gulCmdStartApNum;
UINT32 gulStaID;
MT_AP_RADIO_WLAN_INFO_T gastApWlanInfo[MT_AP_MAX_NUM];//AP WLAN INFO RECORD
#define MT_MAX(x, y)	(((x) < (y)) ? (y) : (x))
#define MT_MIN(x, y)	(((x) < (y)) ? (x) : (y))
#endif /* MT_COMMON_H_ */
