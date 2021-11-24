/******************************************************************************
* COPYRIGHT @ Reserved
*******************************************************************************
* File         : ap_proc.h
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

#ifndef AP_PROC_H_
#define AP_PROC_H_
#include "mt_common.h"
#include "sta_proc.h"

/* CAPWAP Message Type Values */
#define CW_MSG_TYPE_VALUE_DISCOVERY_REQUEST                     1
#define CW_MSG_TYPE_VALUE_DISCOVERY_RESPONSE                    2
#define CW_MSG_TYPE_VALUE_JOIN_REQUEST                          3
#define CW_MSG_TYPE_VALUE_JOIN_RESPONSE                         4
#define CW_MSG_TYPE_VALUE_CONFIGURE_REQUEST                     5
#define CW_MSG_TYPE_VALUE_CONFIGURE_RESPONSE                    6
#define CW_MSG_TYPE_VALUE_CONFIGURE_UPDATE_REQUEST              7
#define CW_MSG_TYPE_VALUE_CONFIGURE_UPDATE_RESPONSE             8
#define CW_MSG_TYPE_VALUE_WTP_EVENT_REQUEST                     9
#define CW_MSG_TYPE_VALUE_WTP_EVENT_RESPONSE                    10
#define CW_MSG_TYPE_VALUE_CHANGE_STATE_EVENT_REQUEST            11
#define CW_MSG_TYPE_VALUE_CHANGE_STATE_EVENT_RESPONSE           12
#define CW_MSG_TYPE_VALUE_ECHO_REQUEST                          13
#define CW_MSG_TYPE_VALUE_ECHO_RESPONSE                         14
#define CW_MSG_TYPE_VALUE_IMAGE_DATA_REQUEST                    15
#define CW_MSG_TYPE_VALUE_IMAGE_DATA_RESPONSE                   16
#define CW_MSG_TYPE_VALUE_RESET_REQUEST                         17
#define CW_MSG_TYPE_VALUE_RESET_RESPONSE                        18
#define CW_MSG_TYPE_VALUE_PRIMARY_DISCOVERY_REQUEST             19
#define CW_MSG_TYPE_VALUE_PRIMARY_DISCOVERY_RESPONSE            20
#define CW_MSG_TYPE_VALUE_DATA_TRANSFER_REQUEST                 21
#define CW_MSG_TYPE_VALUE_DATA_TRANSFER_RESPONSE                22
#define CW_MSG_TYPE_VALUE_CLEAR_CONFIGURATION_REQUEST           23
#define CW_MSG_TYPE_VALUE_CLEAR_CONFIGURATION_RESPONSE          24
#define CW_MSG_TYPE_VALUE_STATION_CONFIGURATION_REQUEST         25
#define CW_MSG_TYPE_VALUE_STATION_CONFIGURATION_RESPONSE        26
#define CW_MSG_TYPE_VALUE_WLAN_CONFIGURATION_REQUEST            3398913
#define CW_MSG_TYPE_VALUE_WLAN_CONFIGURATION_RESPONSE           3398914
#define CW_MSG_TYPE_VALUE_WapiMK_ANNOUNCE_REQUEST               3398915
#define CW_MSG_TYPE_VALUE_WapiMK_ANNOUNCE_RESPONSE              3398916

/* CAPWAP Message Elements Type Values */
#define CW_MSG_ELEMENT_AC_DESCRIPTOR_CW_TYPE                   1
#define CW_MSG_ELEMENT_AC_IPV4_LIST_CW_TYPE                    2
#define CW_MSG_ELEMENT_AC_IPV6_LIST_CW_TYPE                    3
#define CW_MSG_ELEMENT_AC_NAME_CW_TYPE                         4
#define CW_MSG_ELEMENT_AC_NAME_INDEX_CW_TYPE                   5
#define CW_MSG_ELEMENT_TIMESTAMP_CW_TYPE                       6
#define CW_MSG_ELEMENT_ADD_MAC_ACL_CW_TYPE                     7
#define CW_MSG_ELEMENT_ADD_STATION_CW_TYPE                     8
#define CW_MSG_ELEMENT_ADD_STATIC_MAC_ACL_CW_TYPE              9
#define CW_MSG_ELEMENT_CW_CONTROL_IPV4_ADDRESS_CW_TYPE		      10
#define CW_MSG_ELEMENT_CW_CONTROL_IPV6_ADDRESS_CW_TYPE		      11
#define CW_MSG_ELEMENT_CW_TIMERS_CW_TYPE                       12
#define CW_MSG_ELEMENT_DATA_TRANSFER_DATA_CW_TYPE		            13
#define CW_MSG_ELEMENT_DATA_TRANSFER_MODE_CW_TYPE		            14
#define CW_MSG_ELEMENT_CW_DECRYPT_ER_REPORT_CW_TYPE		         15
#define CW_MSG_ELEMENT_CW_DECRYPT_ER_REPORT_PERIOD_CW_TYPE     16
#define CW_MSG_ELEMENT_DELETE_MAC_ACL_CW_TYPE                  17
#define CW_MSG_ELEMENT_DELETE_STATION_CW_TYPE                  18
#define CW_MSG_ELEMENT_DELETE_STATIC_MAC_ACL_CW_TYPE		         19
#define CW_MSG_ELEMENT_DISCOVERY_TYPE_CW_TYPE                  20
#define CW_MSG_ELEMENT_DUPLICATE_IPV4_ADDRESS_CW_TYPE          21
#define CW_MSG_ELEMENT_DUPLICATE_IPV6_ADDRESS_CW_TYPE          22
#define CW_MSG_ELEMENT_IDLE_TIMEOUT_CW_TYPE                    23
#define CW_MSG_ELEMENT_IMAGE_DATA_CW_TYPE			                  24
#define CW_MSG_ELEMENT_IMAGE_IDENTIFIER_CW_TYPE                25
#define CW_MSG_ELEMENT_IMAGE_INFO_CW_TYPE                      26
#define CW_MSG_ELEMENT_INITIATED_DOWNLOAD_CW_TYPE              27
#define CW_MSG_ELEMENT_LOCATION_DATA_CW_TYPE                   28
#define CW_MSG_ELEMENT_MAX_MSG_LEN_CW_TYPE                     29
#define CW_MSG_ELEMENT_LOCAL_IPV4_ADDRESS_CW_TYPE              30
#define CW_MSG_ELEMENT_RADIO_ADMIN_STATE_CW_TYPE               31
#define CW_MSG_ELEMENT_RADIO_OPERAT_STATE_CW_TYPE              32
#define CW_MSG_ELEMENT_RESULT_CODE_CW_TYPE                     33
#define CW_MSG_ELEMENT_RETURNED_MSG_ELEM_CW_TYPE               34
#define CW_MSG_ELEMENT_SESSION_ID_CW_TYPE                      35
#define CW_MSG_ELEMENT_STATISTICS_TIMER_CW_TYPE                36
#define CW_MSG_ELEMENT_VENDOR_SPEC_PAYLOAD_CW_TYPE             37
#define CW_MSG_ELEMENT_WTP_BOARD_DATA_CW_TYPE                  38
#define CW_MSG_ELEMENT_WTP_DESCRIPTOR_CW_TYPE                  39
#define CW_MSG_ELEMENT_WTP_FALLBACK_CW_TYPE                    40
#define CW_MSG_ELEMENT_WTP_FRAME_TUNNEL_MODE_CW_TYPE           41
#define CW_MSG_ELEMENT_WTP_IPV4_ADDRESS_CW_TYPE                30
#define CW_MSG_ELEMENT_WTP_MAC_TYPE_CW_TYPE                    44
#define CW_MSG_ELEMENT_WTP_NAME_CW_TYPE                        45
#define CW_MSG_ELEMENT_WTP_OPERAT_STATISTICS_CW_TYPE           46
#define CW_MSG_ELEMENT_WTP_RADIO_STATISTICS_CW_TYPE            47
#define CW_MSG_ELEMENT_WTP_REBOOT_STATISTICS_CW_TYPE           48
#define CW_MSG_ELEMENT_WTP_STATIC_IP_CW_TYPE                   49
#define CW_MSG_ELEMENT_LOCAL_IPV6_ADDRESS_CW_TYPE              50
#define CW_MSG_ELEMENT_CAPWAP_TRANSPORT_PROTOCOL_CW_TYPE	      51
#define CW_MSG_ELEMENT_MTU_DISCOVERY_PADDING_CW_TYPE           52
#define CW_MSG_ELEMENT_ECN_SUPPORT_CW_TYPE		        	         53

/********* 以下为CAPWAP扩充部分  ********/
#define CW_MSG_ELEMENT_RadioStationLimit_CW_TYPE               54
#define CW_MSG_ELEMENT_APStationLimit_CW_TYPE                  55
#define CW_MSG_ELEMENT_AP_ALARM_CW_TYPE                        56
#define CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE                  57
#define CW_MSG_ELEMENT_AP_WLAN_STA_STATIS_PACKRSSI_CW_TYPE     58
#define CW_MSG_ELEMENT_AP_WLAN_STA_STATIS_PACKLEN_CW_TYPE      59
#define CW_MSG_ELEMENT_AP_WLAN_STA_STATIS_PACKRate_CW_TYPE     60
#define CW_MSG_ELEMENT_STA_REDATTACK_CW_TYPE	        	         61
#define CW_MSG_ELEMENT_AP_WLAN_WAPI_STATIS_CW_TYPE	            62
#define CW_MSG_ELEMENT_AP_QOS_BASE_STATIS_CW_TYPE	      	      63
#define CW_MSG_ELEMENT_AP_QOS_BACK_STATIS_CW_TYPE	      	      64
#define CW_MSG_ELEMENT_AP_QOS_BEST_STATIS_CW_TYPE	      	      65
#define CW_MSG_ELEMENT_AP_QOS_VOICE_STATIS_CW_TYPE	            66
#define CW_MSG_ELEMENT_AP_QOS_VIDEO_STATIS_CW_TYPE	            67
#define CW_MSG_ELEMENT_AP_STATIS_CW_TYPE         	      	      68

/********* 以下为预备定义部分 ********/
#define CW_MSG_ELEMENT_UPDATE_MODE_SET_TYPE                    69
#define CW_MSG_ELEMENT_AP_NTPServer_SET_CW_TYPE	               70
#define CW_MSG_ELEMENT_AP_WLAN_STA_STATIS_PACKSNR_CW_TYPE      71
#define CW_MSG_ELEMENT_AP_WLAN_OFFSTA_STATIS_CW_TYPE           72
#define CW_MSG_ELEMENT_AP_SYSUPTIME_CW_TYPE                    73
#define CW_MSG_ELEMENT_AP_RT_COLLECT_CW_TYPE                   74
#define CW_MSG_ELEMENT_AP_SAMPLE_STARTIME_CW_TYPE              75
#define CW_MSG_ELEMENT_AP_ALARM_LIMIT_CW_TYPE                  76
#define CW_MSG_ELEMENT_AP_LAST_UP_GRADE_TIME                   77
#define CW_MSG_ELEMENT_AP_RADIO_STATIS_CMCC_CW_TYPE            78
#define CW_MSG_ELEMENT_AP_WLAN_STA_STATIS_CMCC_CW_TYPE         79
#define CW_MSG_ELEMENT_AP_WLAN_STATIS_VALUES_CMCC_CW_TYPE      80
#define CW_MSG_ELEMENT_AP_SYS_STATIS_CMCC_CW_TYPE              81
#define CW_MSG_ELEMENT_AP_ETHER_IFPM_PARA_CMCC_CW_TYPE         82
#define CW_MSG_ELEMENT_AP_TRANSFER_LINK_REPORT_CW_TYPE         83/* IE = 83 回传型AP上报的回传链路性能报告 */
#define BINDING_MSG_ELEMENT_TYPE_WTP_Radio_Info                85
#define BINDING_MSG_ELEMENT_TYPE_WTP_Radio_Status              86
#define BINDING_MSG_ELEMENT_TYPE_RF_ParaMgmt_CW_TYPE           87
#define CW_MSG_ELEMENT_AP_PM_PARA_CMCC_CW_TYPE                 88
#define BINDING_MSG_ELEMENT_TYPE_RF_MAC_Filter_Type            90
#define CW_MSG_EXT_ELEMENT_STATION_VLAN                        91
#define CW_MSG_EXT_ELEMENT_WLAN_CONFIG_EXINFO		               92
#define CW_MSG_EXT_ELEMENT_WAPI_STA_SESSION_KEY	               93
#define CW_MSG_EXT_ELEMENT_SSID_INTELLIGENT_BANDWIDTH_INFO     94     /*基于SSID的智能带宽保障*/
#define CW_MSG_EXT_ELEMENT_RADIO_INTELLIGENT_BANDWIDTH_INFO    95     /*基于Radio的智能带宽保障*/
#define CW_MSG_EXT_ELEMENT_AP_INTELLIGENT_BANDWIDTH_INFO       96     /*基于AP的智能带宽保障*/
#define CW_MSG_EXT_ELEMENT_AP_TRANSFER_ROUTER_INFO             97     /*AP回传类型的配置*/
#define CW_MSG_EXT_ELEMENT_AP_ARP_STATIS_INFO                  98     /*ARP报文限速功能配置*//*add by fangtao 20120725*/
#define CW_MSG_EXT_ELEMENT_AP_GET_IP_TYPE                      99     /*设备的IP地址获得方式*//*add by fangtao for MIB Interface 20121031*/

#define CW_MSG_EXT_ELEMENT_AC_PRIORITY                         128
#define CW_MSG_EXT_ELEMENT_11N_RADIO_CAPABILITY_TLV            129
#define CW_MSG_EXT_ELEMENT_802DOC11_RADIO_INFO                 130
#define CW_MSG_EXT_ELEMENT_SCAN_PARA_TLV                       131
#define CW_MSG_EXT_ELEMENT_SCAN_BIND_TLV                       132
#define CW_MSG_EXT_ELEMENT_CHANNEL_SCAN_REPORT                 133
#define CW_MSG_EXT_ELEMENT_NEIGHBOR_AP_REPORT                  134
#define CW_MSG_ELEMENT_STA_SCAN_INFO_CW_TYPE                   135
#define CW_MSG_EXT_ELEMENT_USER_ISOLATION_TLV                  137
#define CW_MSG_EXT_ELEMENT_ROGUE_AP_REPORT_TLV                 138
#define CW_MSG_EXT_ELEMENT_BANDWIDTH_CONF                      139
#define CW_ELEMENT_WLAN_ADMIN_STATE_CW_TYPE                    140
#define CW_MSG_EXT_ELEMENT_ENCRYPTION_EXT                      141
#define CW_MSG_EXT_ELEMENT_EXT_SUPPORTED_RATES                 142
#define CW_ELEMENT_WTP_DOS_ATTARK_CON_CW_TYPE                  143
#define CW_ELEMENT_WTP_CURRENT_STREAM_CW_TYPE                  144
#define CW_MSG_ELEMENT_ADD_STA_BHITE_LIST_CW_TYPE              145
#define CW_MSG_ELEMENT_DEL_STA_BHITE_LIST_CW_TYPE              146
#define CW_MSG_EXT_ELEMENT_802DOC11_RADIO_CONFIG               147
#define CW_ELEMENT_WTP_DOS_ATTARK_REPORT_CW_TYPE               150

/*57号消息上报告警、事件类型宏定义  */
#define AP_AC_AlarmTypeNum_APCoInterfere                  21 /* AP同频干扰告警上报    */
#define AP_AC_AlarmTypeNum_APCoInterfereClear             22 /* AP同频干扰告警清除    */
#define AP_AC_AlarmTypeNum_APNerborInterfere              23 /* AP邻频干扰告警上报   */
#define AP_AC_AlarmTypeNum_APNerborInterfereClear         24 /* AP邻频干扰告警清除    */
#define AP_AC_AlarmTypeNum_StaInterfere                   25 /* 终端干扰告警上报    */
#define AP_AC_AlarmTypeNum_StaInterfereClear              26 /* 终端干扰告警清除    */
#define AP_AC_AlarmTypeNum_OtherDeviceInterfere           27 /* 其他设备干扰告警上报    */
#define AP_AC_AlarmTypeNum_OtherDeviceInterfereClear      28 /* 其他设备干扰告警清除    */
#define AP_AC_AlarmTypeNum_RadioDown                      29 /* 无线链路中断告警上报(中断原因为“管理员关闭 */
#define AP_AC_AlarmTypeNum_RadioDownClear                 30 /* 无线链路中断告警清除(中断原因为“管理员关闭 */
#define AP_AC_AlarmTypeNum_APStaFull                      31 /* AP无法增加新的关联用户告警上报   */
#define AP_AC_AlarmTypeNum_APStaFullClear                 32 /* AP无法增加新的关联用户告警清除   */
#define AP_AC_AlarmTypeNum_ARP                            37/*ARP报文超速告警*/
#define AP_AC_AlarmTypeNum_ARPClear                       38/*ARP报文超速告警清除*/
#define AP_AC_EVentTypeNum_APMtRdoChanlChange             33 /* 无线信道变更通告 */
#define AP_AC_EVentTypeNum_DetectRogue                    34 /* 发现可疑设备通告 */
#define AP_AC_EVentTypeNum_StaAuthError                   35 /* 终端鉴权失败通告  */
#define AP_AC_EVentTypeNum_StAssociationFail              36 /* 终端关联失败通告 */
#define AP_AC_EVentTypeNum_UserWithInvalidCerfication     37 /* 非法证书用户侵入网络通告 */
#define AP_AC_EVentTypeNum_StationRepititiveAttack        38 /* 客户端重放攻击通告 */
#define AP_AC_EVentTypeNum_TamperAttack                   39 /* 篡改攻击通告 */
#define AP_AC_EVentTypeNum_LowSafeLevelAttack             40 /* 安全等级降低攻击通告  */
#define AP_AC_EVentTypeNum_AddressRedirectionAttack       41 /* 地址重定向攻击通告 */
#define AP_AC_EVentTypeNum_ACTurnToBackupDevice           43 /* AC发生主备切换告警 */

//DHCP Message Type
#define MT_DHCPV4_DISCOVER                                1 //DHCPv4 Discover
#define MT_DHCPV4_REQUEST                                 3 //DHCPv4 Request

//1	AC描述符（AC Descriptor）
typedef struct _AC_INFOR_SUB_ELEMENT_T_
{
	UINT32 ulACInforVndrId;//厂商企业号。
	UINT16 usACInforType;
//	         4 -   Hardware Version:AC的硬件版本,字符串格式,长度不超过32个字节。
//	         5 -   Software Version:AC的软件版本,字符串格式,长度不超过32个字节。
	UINT16 usACInforLen;//类型信息的长度,比如软件版本和硬件版本,长度都不超过32字节。
#define MT_AC_INFORMATION_DATA_LEN 32
	UINT8 aucACInforData[MT_AC_INFORMATION_DATA_LEN];//类型值,长度由前一个字段表示，不超过32字节。
}MT_AC_INFOR_SUB_ELEMENT;

typedef struct _T1_T_
{
	UINT16 usStations;//AC当前服务的station总个数
	UINT16 usLimit;//AC支持的能接入的station总个数
	UINT16 usActiveAPs;//AC当前接入的AP总个数
	UINT16 usMaxAPs;//AC支持的AP的最大个数
	UINT8 ucSecurity;//8-bit掩码表示AC支持的认证加密类型,该项可选支持，个数如下
	//         0 1 2 3 4 5 6 7
	//        +-+-+-+-+-+-+-+-+
	//        |Reserved |S|X|R|
	//        +-+-+-+-+-+-+-+-+
	//Reserved;//一些保留位为了将来应用,当前版本支持的实现必须填充为0。接收者收到这样的保留字段必须忽略。
	//      S:AC支持pre-shared 加密认证。
	//      X:AC支持X.509证书认证。
	//      R:保留为,发送者必须填充为0,接收者必须忽略这个字段。
	UINT8 ucR_MAC_Field;//AC在CAPWAP传输头支持的可选的Radio MAC地址字段,有以下枚举值：
	//      0 - 	Reserved
	//      1 -   支持
	//      2 -   不支持
	UINT8 ucResvd;//保留字段
	UINT8 ucDTLSPolicy;//AC通告它数据通道的DTLS策略.AC可能通告一个或者多个策略,下面的bit位来表示，AP必须遵循AC通告的策略选择,字段的个数如下：
	//         0 1 2 3 4 5 6 7
	//        +-+-+-+-+-+-+-+-+
	//        |Reserved |D|C|R|
	//        +-+-+-+-+-+-+-+-+
	//    Reserved;//保留字段，格式全0
	//     	 D;//DTLS-Enable数据通道支持，可选支持
	//    	 C;//明文数据信道支持，必须支持
	//       R;//保留字段,发送者必须设定为0,接收者必须忽略这些字段
	MT_AC_INFOR_SUB_ELEMENT stAcInforSubElmnt;
}__attribute__ ((packed)) MT_T1;

// 4 AC名字（AC Name）
typedef struct _T4_T_
{
	UINT8 aucAcName[MT_AC_NAME_LEN];//可变长UTF-8格式的AC name,最长不超过512字节。
}__attribute__ ((packed)) MT_T4;

//31 for Radio Administrative State
typedef struct _T31_T_
{
	UINT8 ucRadioID;//Radio id 范围是1-31.
	UINT8 ucAdminState;//8-bit为的字段,标识管理状态.有以下枚举值
	//      	1 -  Enabled 可用状态
	//      	2 -  Disabled 不可用状态
}__attribute__ ((packed)) MT_T31;

//32 for Radio Operational State
typedef struct _T32_T_
{
	UINT8 ucRadioID;//Radio id,范围1-31.
	UINT8 ucState;//8-bit为的字段,标识管理状态.有以下枚举值
//	      	1 -  Enabled 可用状态
//	      	0 -  Disabled 不可用状态
	UINT8 ucCause;//当radio是不可操作的时候,需要上报原因.引起radio不可操作的原因,枚举值如下.
//	      	0 -  正常
//	  	1- radio 失败
//	      	2 -  软件失败
//	      	3 -  管理员设定操作失败
}__attribute__ ((packed)) MT_T32;

//33 for Result Code
typedef struct _T33_T_
{
	UINT32 ulResultCode;//Result code的枚举值如下：
//	      0  成功.
//	      1  请求信息不完整失败.
//	      2  成功,但是检测到NAT.
//	  3  join失败,没有绑定802.11协议.
//	  4  join失败,资源耗尽.
//	  5  join失败,AP不识别.
//	  6  join失败,不正确的数据格式.
//	  7  Join Failurejoin失败,会话id已经使用.
//	  8  Join Failure join失败,不支持AP的硬件.
//	  9  join失败,绑定协议ac不支持.
//	  10 reset失败,不能reset.
//	  11 reset失败,固件写错误.
//	  12 configuration 失败,请求的配置设定失败.
//	  13 configuration 失败,不支持请求的配置.
//	  14 imagedate错误,校验和错误.
//	  15 image date错误,无效的数据长度.
//	  16 image date错误,未知的错误类型.
//	  17 image date错误,image已经发送.
//	      18 不是期望的消息,当前状态接收到非法消息.
//	      19不是期望消息,当前状态接收的消息无法识别.
//	      20 失败,缺少必要的消息类型标识.
//	      21 失败 不识别的消息类型.
//	      22 数据传输失败.
}__attribute__ ((packed)) MT_T33;

//36 for Statistics Timer
typedef struct _T36_T_
{
	UINT16 usStatisticsTimer;//统计结果上报周期，16-bit无符号整数定义的时间,单位是秒，默认值为120秒
}__attribute__ ((packed)) MT_T36;

//41 for WTP Frame Tunnel Mode
typedef struct _T41_T_
{
	UINT8 ucWtpFrmTunnlMod;
//	 0
//	      0 1 2 3 4 5 6 7
//	     +-+-+-+-+-+-+-+-+
//	     |Reservd|N|E|L|U|
//	     +-+-+-+-+-+-+-+-+
//	Type:   41 for WTP Frame Tunnel Mode
//	   Length:   1
//	   Reservd:保留位,发送者设定为0,接收者忽略这些保留字段.
//	   N:802.11隧道模式,集中转发,目前最常用的模式
//	   E:802.3 隧道模式
//	   L:Local tunnel,本地转发模式
//	   R（U）: 保留位，置0
}__attribute__ ((packed)) MT_T41;


//44 for WTP MAC Type
typedef struct _T44_T_
{
	 UINT8 ucMACType;//AP支持MAC模式（AC AP将封装格式），有如下枚举值：
//	      0 -   Local MAC:AP数据报文仅支持802.3frame封装数据。
//	      1 -   Split MAC:AP数据报文仅支持802.11frame封装数据。
//	      2 -   Both:AP数据报文既支持802.3frame封装数据，又支持802.11frame封装数据。
}__attribute__ ((packed)) MT_T44;

//48 for WTP Reboot Statistics
typedef struct _T48_T_
{
	UINT16 usRebootCount;//重启总次数.
	UINT16 usACInitiatedCount;//AC初始化引起重启的次数.
	UINT16 usLinkFailureCount;//链路失败引起的重启次数.
	UINT16 usSWFailureCount;//软件失败引起的重启次数.
	UINT16 usHWFailureCount;//硬件失败引起的重启次数.
	UINT16 usOtherFailureCount;//其它失败引起的AP重启总次数(非软件硬件和链路失败).
	UINT16 usUnknownFailureCount;//未知失败引起的重启次数.
	UINT8 ucLastFailureType;//最后一次失败类型.
	//      0 -  不支持的错误类型
	//  1- AC初始化引起错误
	//      2 - 链路失败
	//      3 -  软件失败
	//      4 -  硬件失败
	//      5 -  其它失败类型
	//      255 - 未知错误类型.
}__attribute__ ((packed)) MT_T48;

//1025 IEEE 802.11 Antenna
typedef struct _T1025_T_
{
	UINT8 ucRadioID;//Radio编号，1-31
	UINT8 ucDiversity;//天线分极。
	UINT8 ucCombiner;//天线组合。
	UINT8 ucAntennaCnt;//天线数
#define MT_ANTENNACNT_MAX 16
	UINT8 aucAntennaSelection[MT_ANTENNACNT_MAX];//1为内置，2为外置
}__attribute__ ((packed)) MT_T1025;

//1028 for IEEE 802.11 Direct Sequence Control
typedef struct _T1028_T_
{
	UINT8 ucRadioID;//8bit。范围1-31。
	UINT8 ucResvd;//8bit。保留字段。
	UINT8 ucCurrentChannel;//8bit。当前radio的信道。范围1-13（根据国家和地区而定）。
	UINT8 ucCurrentCCA;//(该字段内容暂不做要求) 当前的空闲信道评估模式设置。可参考IEEE 802.11 dot11CCAModeSupported MIB（见 [IEEE.802-11.2007]）。取值如下：
	//         1 - 能量检测(edonly)
	//         2 - 载波检测(csonly)
	//         4 - 载波检测和能量检测混合(edandcs)
	//         8 - 定时载波检测(cswithtimer)
	//        16 - 高速载波检测和能量检测(hrcsanded)
	UINT32 ulEnergyDetectThrd;//（该字段内容暂不做要求） 能量检测的能量阀值，接收端根据能量是否高于此阀值来判断。可参考IEEE 802.11 dot11EDThreshold MIB（见 [IEEE.802-11.2007]）。
}__attribute__ ((packed)) MT_T1028;

//1030 for IEEE 802.11 MAC Operation
typedef struct _T1030_T_
{
	UINT8 ucRadioID;//8bit。范围1-31。
	UINT8 ucResvd;//8bit。保留字段。发送方设置为0，接收方忽略此值。
	UINT16 usRTSThreshold;//16bit。设置启用RTS（发送请求）机制所要求的帧的长度门限值，取值范围是0～2346。缺省情况下，启用RTS机制所要求的帧长度门限值为2346字节。当帧的实际长度大于设定的门限值时，则系统启用RTS机制。RTS（Request To Send，要求发送）用于在无线局域网中避免数据发送冲突。RTS包的发送频率需要合理设置，设置RTS门限时需要进行权衡：如果将这个参数值设得较小，则会增加RTS包的发送频率，消耗更多的带宽。但RTS包发送得越频繁，系统从中断或冲突中恢复得就越快。
	UINT8 ucShortRetry;//8bit。帧长小于RTS门限值的帧的最大重传次数，取值范围1-15。缺省情况下，帧长小于RTS门限值的帧的最大重传次数为7。此值来源于消息元素IEEE 802.11 dot11ShortRetryLimit MIB（见 [IEEE.802-11.2007]）。
	UINT8 ucLongRetry;//8bit。帧长超过RTS门限值的帧的最大重传次数，取值范围1-15。缺省情况下，帧长超过RTS门限值的帧的最大重传次数为4。此值来源于消息元素IEEE 802.11 dot11LongRetryLimit MIB（见 [IEEE.802-11.2007]）。
	UINT16 usFragmtThrd;//16bit。帧的分片门限值，取值范围为256-2346，单位为字节。当数据包的实际大小超过指定的分片门限值时，该数据包被分片传输。缺省情况下，帧的分片门限值为2346字节，即小于2346字节的帧在传输时不分片。此值来源于消息元素IEEE 802.11 dot11FragmentationThreshold MIB（见 [IEEE.802-11.2007]）。
	UINT32 ulTxMSDULifetime;//传输一个Mac服务数据单元的最长周期。默认值为512。此值来源于消息元素IEEE 802.11 dot11MaxTransmitMSDULifetime MIB（见 [IEEE.802-11.2007]）。
	UINT32 ulRxMSDULifetime;//接收一个Mac服务数据单元的最长周期。默认值为512。此值来源于消息元素IEEE 802.11 dot11MaxReceiveLifetime MIB（见 [IEEE.802-11.2007]）。
}__attribute__ ((packed)) MT_T1030;

//1032 IEEE 802.11 Multi-Domain Capability
typedef struct _T1032_T_
{
	UINT8 ucRadioID;//Radio编号，1-31.
	UINT8 ucResvd;//保留字段
	UINT16 usFirstChannel;// #：频段内首信道
	UINT16 usNumberofChannls;//总可用信道
	UINT16 usMaxTxPwrLevl;//最大发射功率
}__attribute__ ((packed)) MT_T1032;

//1033	IEEE 802.11 OFDM Control
typedef struct _T1033_T_
{
	UINT8 ucRadioID;//数值1-31
	UINT8 ucResvd;//保留数值
	UINT8 ucCurrntChan;//当前信道
	UINT8 ucBandSupport;//支持的频带
	UINT32 ulTIThrld;//CCA规定的门限
}__attribute__ ((packed)) MT_T1033;

//1040	IEEE 802.11 Supported Rates
typedef struct _T1040_T_
{
	UINT8 ucRadioID;//Radio编号，1-31
#define MT_WTP_SUPPORT_RATE_SET_LEN 8
	UINT8 aucSupprtedRates[MT_WTP_SUPPORT_RATE_SET_LEN];//支持的速率集。
}__attribute__ ((packed)) MT_T1040;

//1041 for IEEE 802.11 Tx Power
typedef struct _T1041_T_
{
	UINT8 ucRadioID;//数值1-31
	UINT8 ucResvd;//保留数值
	UINT16 usCurrntTxPwr;//发射功率值。单位dbm。
}__attribute__ ((packed)) MT_T1041;

//IEEE802.11 Tx Power Level(1042)
typedef struct _T1042_T_
{
	UINT8 ucRadioID;//数值1-31
	UINT8 ucTxPwrLvlNum;//The number of power level attributes.  The value of this field comes from the IEEE 802.11
#define MT_WTP_TXPOWER_LEVEL_SET_LEN 8
	UINT16 ausPowerLevel[MT_WTP_TXPOWER_LEVEL_SET_LEN];//Each power level field contains a supported power level, in mW.  The value of this field comes from the corresponding IEEE 802.11 dot11TxPowerLevel[n] MIB element, see [IEEE.802-11.2007].
}__attribute__ ((packed)) MT_T1042;

//1046 for IEEE 802.11 WTP WLAN Radio Configuration
typedef struct _T1046_T_
{
	UINT8 ucRadioID;//8bit。范围1-31。
	UINT8 ucShortPreamble;//8bit。表明是否支持短前导码。取值范围：
	//     	 0 -   不支持短前导码
	//     	 1 -   支持短前导码
	UINT8 ucNumberOfBSSIDs;//这个字段表明AP支持的最大BSSID数，这个数约束了AP上支持的逻辑网络数量，范围是1-16。
	UINT8 ucDTIMPeriod;//DTIM间隔设定了信标帧间隔的数量，信标帧中的TIM元素中包含了DTIM字段，如果设置为1，则每个信标帧中都包含DTIM，如果设置为2，则每2个信标帧中包含1个DTIM，以此类推。这个字段来源于IEEE 802.11 dot11DTIMPeriod MIB element (见[IEEE.802-11.2007]）。
	UINT8 aucBSSID[MT_MAC_LENTH];//Radio的MAC地址。
	UINT16 usBeaconPeriod;//（信标帧间隔）,这个值设置了一个站点发送信标帧的时间间隔，在信标帧和探测帧回复（Probe Response）中都包含这个字段。这个信息来源于IEEE 802.11 dot11BeaconPeriod MIB元素（见 [IEEE.802-11.2007]）。
	UINT8 aucCountryStr[4];//（国家码）:
	//        CN:中国
	// 	      EU:欧洲
	//        US:美国
	//        JP:日本
	//        FR:法国
	//        ES:西班牙
	//        XX:未定义,目前支持这些国家码的设定
}__attribute__ ((packed)) MT_T1046;

//1048 for IEEE 802.11 WTP Radio Information
typedef struct _T1048_T_
{
	UINT8 ucRadioID;//The Radio Identifier, 范围1-31，是AP上的radio标识符。
	UINT32 ulRadioType;//在Radio Type的最后4位，可以组合,基本定义类型如下：
	//         8:   An IEEE 802.11n radio.
	//         4:   An IEEE 802.11g radio.
	//         2:   An IEEE 802.11a radio.
	//         1:   An IEEE 802.11b radio.
}__attribute__ ((packed)) MT_T1048;

//IE56_4~44,AP_ALARM（IE=56）,BGN
//4     AC与AP间系统时钟同步失败通告
typedef struct _T56_4_T_
{
;//无
}__attribute__ ((packed)) MT_IE56_4_T;

//21    CPU利用率过高告警
typedef struct _T56_21_T_
{
  UINT8 ucCpuUsage;/*当前利用率*/
}__attribute__ ((packed)) MT_IE56_21_T;

//22    CPU利用率过高告警清除
typedef struct _T56_22_T_
{
  UINT8 ucCpuUsage;/*当前利用率*/
}__attribute__ ((packed)) MT_IE56_22_T;

//23    内存利用率过高告警
typedef struct _T56_23_T_
{
  UINT8 ucMemUsage;/*当前利用率*/
}__attribute__ ((packed)) MT_IE56_23_T;

//24    内存利用率过高告警清除
typedef struct _T56_24_T_
{
  UINT8 ucMemUsage;/*当前利用率*/
}__attribute__ ((packed)) MT_IE56_24_T;

//27    AP无线监视工作模式变更通告
typedef struct _T56_27_T_
{
  UINT8 ucBeforeChgMode;/*变更前工作模式*/
  UINT8 ucAfterChgMode;/*变更后工作模式*/
}__attribute__ ((packed)) MT_IE56_27_T;

//28    AP软件升级失败通告
typedef struct _T56_28_T_
{
  UINT8 aucBeforeVer[32];/*之前版本*/
  UINT8 aucAfterVer[32];/*之后版本*/
  UINT8 aucCause[64];
}__attribute__ ((packed)) MT_IE56_28_T;

//29    SSID密钥冲突通告
typedef struct _T56_29_T_
{
  UINT8 ucRadioId;
  UINT8 ucWlanID1;
  UINT8 ucWlanID2;
  UINT8 ucKeyIndex;
}__attribute__ ((packed)) MT_IE56_29_T;

//30    AP冷启动告警
typedef struct _T56_30_T_
{
  UINT8 aucTime[32];/*冷启动时间，字符串表示*/
}__attribute__ ((packed)) MT_IE56_30_T;

//31    AP冷启动告警清除
typedef struct _T56_31_T_
{
  UINT8 aucTime[32];/*冷启动时间，字符串表示*/
}__attribute__ ((packed)) MT_IE56_31_T;

//32    AP热启动告警
typedef struct _T56_32_T_
{
  UINT8 aucTime[32];/*热启动时间，字符串表示*/
}__attribute__ ((packed)) MT_IE56_32_T;

//33    AP热启动告警清除
typedef struct _T56_33_T_
{
  UINT8 aucTime[32];/*热启动时间，字符串表示*/
}__attribute__ ((packed)) MT_IE56_33_T;

//34    加密机制改变通告
typedef struct _T56_34_T_
{
  UINT8 ucRadioId;
  UINT16 usWlanProfileId;
  UINT8 aucSSIDName[MT_SSID_LEN];
  UINT8 aucBSSID [MT_MAC_LENTH];/*WlanMac*/
  UINT8 ucAuthModeBefore;/*鉴权模式.1：WEP 2：WPA-PSK 3：WPA2-PSK 4：WPA-RADIUS 5：WPA2- RADIUS  6: WAPI-CER 7: WAPI-PSK*/
  UINT8 ucAuthModeAfter;/*改变后的加密机制*/
}__attribute__ ((packed)) MT_IE56_34_T;

//35    发现无线泛洪攻击通告
typedef struct _T56_35_T_
{
  UINT8 aucDeviceMac[MT_MAC_LENTH];/*可疑设备 MAC地址*/
  UINT8 aucResv[2];
  UINT32 ulAuthCount;/* 攻击的认证报文计数，增量上报*/
  UINT32 ulDeAuthCount; /* 攻击的去认证报文计数，增量上报*/
  UINT32 ulAossCount; /* 攻击的关联报文计数，增量上报*/
  UINT32 ulDeAossCount; /* 攻击的去关联报文计数，增量上报*/
}__attribute__ ((packed)) MT_IE56_35_T;

//36    发现仿冒攻击通告
typedef struct _T56_36_T_
{
  UINT8 aucDeviceMac[MT_MAC_LENTH];/*可疑设备 MAC地址*/
  UINT8 aucResv[2];
  UINT32 ulAuthCount;/* 攻击的认证报文计数，增量上报*/
  UINT32 ulDeAuthCount; /* 攻击的去认证报文计数，增量上报*/
  UINT32 ulAossCount; /* 攻击的关联报文计数，增量上报*/
  UINT32 ulDeAossCount; /* 攻击的去关联报文计数，增量上报*/
}__attribute__ ((packed)) MT_IE56_36_T;

//37    发现Dos攻击通告
typedef struct _T56_37_T_
{
  UINT8 aucDeviceMac[MT_MAC_LENTH];/*可疑设备 MAC地址*/
  UINT8 aucResv[2];
}__attribute__ ((packed)) MT_IE56_37_T;

//38    AP温度过高告警
typedef struct _T56_38_T_
{
  UINT8 ucApTemperatureTrap;/*当前温度*/
}__attribute__ ((packed)) MT_IE56_38_T;

//39    AP温度过高告警清除
typedef struct _T56_39_T_
{
  UINT8 ucApTemperatureClearTrap;/*当前温度*/
}__attribute__ ((packed)) MT_IE56_39_T;

//40    AP软件升级成功通告
typedef struct _T56_40_T_
{
  UINT8 aucBeforeVer[32];/*之前版本*/
  UINT8 aucAfterVer[32];/*之后版本*/
  UINT8 aucCause[64];
}__attribute__ ((packed)) MT_IE56_40_T;

//41    用户上线信息通告
typedef struct _T56_41_T_
{
  UINT32 ulUserOnlineTime;/*用户上线的时间,单位为秒(距1970年1月1日 00:00:00的秒数)*/
  INT16 sTxPower;/*信号强度,单位为dBm*/
  UINT16 usCurrentChannel;/*接入信道*/
  UINT8 aucStaMac[MT_MAC_LENTH];/*STA的MAC地址*/
  UINT8 aucSSID[32]; /*用户连接的SSID*/
}__attribute__ ((packed)) MT_IE56_41_T;

//42    用户下线信息通告
typedef struct _T56_42_T_
{
  UINT32 ulUserOfflineTime;/*用户下线的时间,单位为秒(距1970年1月1日 00:00:00的秒数)*/
  INT16 sTxPower;/*信号强度,单位为dBm*/
  UINT16 usCurrentChannel;/*接入信道*/
  UINT8 aucStaMac[6];/*STA的MAC地址*/
  UINT8 aucSSID[32]; /*用户连接的SSID*/
}__attribute__ ((packed)) MT_IE56_42_T;

//43    发现弱初始化向量攻击检测通告
typedef struct _T56_43_T_
{
  UINT8 aucDeviceMac[MT_MAC_LENTH];/*可疑设备 MAC地址*/
  UINT8 aucResv[2];
  UINT32 ulWeakIVDetecteCount;/* 弱IV检测数*/
}__attribute__ ((packed)) MT_IE56_43_T;

//44 AP缓存写失败通告
typedef struct _T56_44_T_
{
  UINT8 aucCause[64];/*失败原因*/
}__attribute__ ((packed)) MT_IE56_44_T;

//IE56_4~44,AP_ALARM（IE=56）,END

//IE57_21~38,AP_RADIO_ALARM BGN
//21 AP同频干扰告警
typedef struct _T57_21_T_
{
  UINT8 ucRadioId;
  UINT16 usCurrentChannel;/*AP当前信道*/
  UINT8 aucInterferBssid[MT_MAC_LENTH];/*干扰源BSSID信息*/
}__attribute__ ((packed)) MT_IE57_21_T;

//22 AP同频干扰告警清除
typedef struct _T57_22_T_
{
  UINT8 ucRadioId;
  UINT16 usCurrentChannel;/*AP当前信道*/
  UINT8 aucInterferBssid[MT_MAC_LENTH];/*干扰源BSSID信息*/
}__attribute__ ((packed)) MT_IE57_22_T;

//23 AP邻频干扰告警
typedef struct _T57_23_T_
{
  UINT8 ucRadioId;
  UINT16 usCurrentChannel;/*AP当前信道*/
  UINT8 aucInterferBssid[MT_MAC_LENTH];/*干扰源BSSID信息*/
  UINT16 usInterferChannel;/*干扰源信道*/
}__attribute__ ((packed)) MT_IE57_23_T;

//24 AP邻频干扰告警清除
typedef struct _T57_24_T_
{
  UINT8 ucRadioId;
  UINT16 usCurrentChannel;/*AP当前信道*/
  UINT8 aucInterferBssid[MT_MAC_LENTH];/*干扰源BSSID信息*/
  UINT16 usInterferChannel;/*干扰源信道*/
}__attribute__ ((packed)) MT_IE57_24_T;

//25    终端干扰告警
typedef struct _T57_25_T_
{
  UINT8 ucRadioId;
  UINT16 usCurrentChannel;/*AP当前信道*/
  UINT8 aucStaMac[MT_MAC_LENTH];/*终端Mac*/
}__attribute__ ((packed)) MT_IE57_25_T;

//26    终端干扰告警清除
typedef struct _T57_26_T_
{
  UINT8 ucRadioId;
  UINT16 usCurrentChannel;/*AP当前信道*/
  UINT8 aucStaMac[MT_MAC_LENTH];/*终端Mac*/
}__attribute__ ((packed)) MT_IE57_26_T;

//27    其他设备干扰告警
typedef struct _T57_27_T_
{
  UINT8 ucRadioId;
  UINT16 usCurrentChannel;/*AP当前信道*/
}__attribute__ ((packed)) MT_IE57_27_T;

//28    其他设备干扰告警清除
typedef struct _T57_28_T_
{
  UINT8 ucRadioId;
  UINT16 usCurrentChannel;/*AP当前信道*/
}__attribute__ ((packed)) MT_IE57_28_T;

//29    无线链路中断告警
typedef struct _T57_29_T_
{
  UINT8 ucRadioId;
  UINT8 ucReason;
}__attribute__ ((packed)) MT_IE57_29_T;

//30    无线链路中断告警清除
typedef struct _T57_30_T_
{
  UINT8 ucRadioId;
  UINT8 ucReason;
}__attribute__ ((packed)) MT_IE57_30_T;

//31 AP无法增加新的关联用户告警
typedef struct _T57_31_T_
{
  UINT32 ulApMaxStaNum;/*AP允许关联的最大用户数*/
  UINT8 aucCause[64];
}__attribute__ ((packed)) MT_IE57_31_T;

//32 AP无法增加新的关联用户告警清除
typedef struct _T57_32_T_
{
  UINT32 ulApMaxStaNum;/*AP允许关联的最大用户数*/
  UINT8 aucCause[64];
}__attribute__ ((packed)) MT_IE57_32_T;

//33    无线信道变更通告
typedef struct _T57_33_T_
{
  UINT8 ucRadioId;
  UINT16 usBeforeChgChannel;/*变更前信道*/
  UINT16 usAfterChgChannel;/*变更后信道*/
  UINT8 ucChgMode;/*1-手动，2-自动*/
//  UINT8 aucApSerial[64];//AP序列号
  UINT8 ucAlrmCnt;//累计告警次数
}__attribute__ ((packed)) MT_IE57_33_T;

//34    发现可疑设备通告
typedef struct _MT_DUBIOUS_AP_INFO_T_
{
  UINT8 aucApMac[MT_MAC_LENTH];/*AP MAC*/
  UINT8 aucResvd1[2];
  UINT8 aucBssid[MT_MAC_LENTH];/* BSSID信息*/
  UINT8 aucResvd2[2];
  UINT8 aucSSID[MT_SSID_LEN];
//  UINT8 aucUserStatAPReceivedStaSNR[16];/*信噪比*/
  UINT8 ucUserStatAPReceivedStaSNR;/*信噪比*/
  INT16 sTxPower;/* 信号强度*/
  UINT16 usCurrentChannel;/*信道*/
  UINT32 ulTime;/* 扫描到可疑AP的当前时间年、月、日、时、分、秒，字符串表示*/
}__attribute__ ((packed)) MT_DUBIOUS_AP_INFO_T;

typedef struct _T57_34_T_
{
  UINT8 ucApNum;
  UINT8 aucResv[3];
  MT_DUBIOUS_AP_INFO_T astApInfo[8];
}__attribute__ ((packed)) MT_IE57_34_T;

//35    终端鉴权失败通告
typedef struct _T57_35_T_
{
  UINT8 ucRadioId;
  UINT8 ucWlanId;
  UINT8 aucSSIDName[MT_SSID_LEN];
  UINT8 aucBSSID [MT_MAC_LENTH];
  UINT8 aucStaMac[MT_MAC_LENTH];/*终端Mac*/
  UINT8 ucAuthMode;/*鉴权模式.1：WEP 2：WPA-PSK 3：WPA2-PSK 4：WPA-RADIUS 5：WPA2- RADIUS  6: WAPI-CER 7: WAPI-PSK*/
  UINT8 aucCause[64];
}__attribute__ ((packed)) MT_IE57_35_T;

//36    终端关联失败通告
typedef struct _T57_36_T_
{
  UINT8 ucRadioId;
  UINT8 ucWlanId;
  UINT8 aucSSIDName[MT_SSID_LEN];
  UINT8 aucBSSID [MT_MAC_LENTH];
  UINT8 aucStaMac[MT_MAC_LENTH];/*终端Mac*/
  UINT8 aucCause[64];
}__attribute__ ((packed)) MT_IE57_36_T;

//37 ARP报文超速告警
typedef struct _T57_37_T_
{
  UINT8 ucRadioId;
  UINT8 ucWlanId;
  UINT8 aucStaMac[MT_MAC_LENTH];/*终端Mac*/
}__attribute__ ((packed)) MT_IE57_37_T;

//38 ARP报文超速告警清除
typedef struct _T57_38_T_
{
  UINT8 ucRadioId;
  UINT8 ucWlanId;
  UINT8 aucStaMac[MT_MAC_LENTH];/*终端Mac*/
}__attribute__ ((packed)) MT_IE57_38_T;

//AP射频单元启动工作通告
typedef struct _T57_39_T_
{
  UINT8 ucRadioId;
  UINT8 ucReason; /*上报原因: 1:AP上线时正常, 2:AP在线时正常*/
}__attribute__ ((packed)) MT_IE57_39_T;

//AP射频单元停止工作通告
typedef struct _T57_40_T_
{
  UINT8 ucRadioId;
  UINT8 ucReason; /*上报原因: 1: 射频模块异常, 2: 管理员关闭*/
  //UINT8 aucCause[64];
}__attribute__ ((packed)) MT_IE57_40_T;

//AP配置失败通告
typedef struct _T57_41_T_
{
  UINT8 ucChgType;// 配置类型 1: AP系统参数配置, 2: 射频参数配置, 3: WLAN配置*/
  UINT8 ucRadioId;
  UINT8 ucWlanId;
  UINT8 ucReason;// 失败原因
}__attribute__ ((packed)) MT_IE57_41_T;

//链路认证失败
typedef struct _T57_42_T_
{
  UINT8 ucRadioId;
  UINT8 ucWlanId;
  UINT8 aucSSIDName[32];
  UINT8 aucBSSID[MT_MAC_LENTH];
  UINT8 aucStaMac[MT_MAC_LENTH];/*终端Mac*/
  UINT8 ucAuthtype;/* 鉴权模式.1：WEP 2：WPA-PSK 3：WPA2-PSK 4：WPA-RADIUS 5：WPA2- RADIUS  6: WAPI-CER 7: WAPI-PSK */
  UINT8 aucCause[64];
}__attribute__ ((packed)) MT_IE57_42_T;

//AP配置成功通告
typedef struct _T57_43_T_
{
	UINT8 ucChgType;/*配置类型 1: AP系统参数配置, 2: 射频参数配置, 3: WLAN配置*/
	UINT8 ucRadioId;
	UINT8 ucWlanId;
	UINT8 ucReason;// 失败原因
}__attribute__ ((packed)) MT_IE57_43_T;

//IE57_21~42,AP_RADIO_ALARM END

//58  AP的终端统计（RSSI报文统计）
typedef struct _WLAN_STA_PARA_PACKRSSI_T_
{
	UINT16 usWlanID;
	UINT8 aucStaMacAddr[MT_MAC_LENTH];
	UINT32 ulUserStatApRxRSSIto90PktsLo;//低32位
	UINT32 ulUserStatApRxRSSIto90PktsHi;//高32位
	UINT32 ulUserStatApRxRSSI90to85PktsLo;//低32位
	UINT32 ulUserStatApRxRSSI90to85PktsHi;//高32位
	UINT32 ulUserStatApRxRSSI85to80PktsLo;//低32位
	UINT32 ulUserStatApRxRSSI85to80PktsHi;//高32位
	UINT32 ulUserStatApRxRSSI80to75PktsLo;//低32位
	UINT32 ulUserStatApRxRSSI80to75PktsHi;//高32位
	UINT32 ulUserStatApRxRSSI75to70PktsLo;//低32位
	UINT32 ulUserStatApRxRSSI75to70PktsHi;//高32位
	UINT32 ulUserStatApRxRSSI70to65PktsLo;//低32位
	UINT32 ulUserStatApRxRSSI70to65PktsHi;//高32位
	UINT32 ulUserStatApRxRSSI65to60PktsLo;//低32位
	UINT32 ulUserStatApRxRSSI65to60PktsHi;//高32位
	UINT32 ulUserStatApRxRSSI60to55PktsLo;//低32位
	UINT32 ulUserStatApRxRSSI60to55PktsHi;//高32位
	UINT32 ulUserStatApRxRSSI55to50PktsLo;//低32位
	UINT32 ulUserStatApRxRSSI55to50PktsHi;//高32位
	UINT32 ulUserStatApRxRSSI50to45PktsLo;//低32位
	UINT32 ulUserStatApRxRSSI50to45PktsHi;//高32位
	UINT32 ulUserStatApRxRSSI45toPktsLo;//低32位
	UINT32 ulUserStatApRxRSSI45toPktsHi;//高32位
	UINT32 ulUserStatApTxRSSIto90PktsLo;//低32位
	UINT32 ulUserStatApTxRSSIto90PktsHi;//高32位
	UINT32 ulUserStatApTxRSSI90to85PktsLo;//低32位
	UINT32 ulUserStatApTxRSSI90to85PktsHi;//高32位
	UINT32 ulUserStatApTxRSSI85to80PktsLo;//低32位
	UINT32 ulUserStatApTxRSSI85to80PktsHi;//高32位
	UINT32 ulUserStatApTxRSSI80to75PktsLo;//低32位
	UINT32 ulUserStatApTxRSSI80to75PktsHi;//高32位
	UINT32 ulUserStatApTxRSSI75to70PktsLo;//低32位
	UINT32 ulUserStatApTxRSSI75to70PktsHi;//高32位
	UINT32 ulUserStatApTxRSSI70to65PktsLo;//低32位
	UINT32 ulUserStatApTxRSSI70to65PktsHi;//高32位
	UINT32 ulUserStatApTxRSSI65to60PktsLo;//低32位
	UINT32 ulUserStatApTxRSSI65to60PktsHi;//高32位
	UINT32 ulUserStatApTxRSSI60to55PktsLo;//低32位
	UINT32 ulUserStatApTxRSSI60to55PktsHi;//高32位
	UINT32 ulUserStatApTxRSSI55to50PktsLo;//低32位
	UINT32 ulUserStatApTxRSSI55to50PktsHi;//高32位
	UINT32 ulUserStatApTxRSSI50to45PktsLo;//低32位
	UINT32 ulUserStatApTxRSSI50to45PktsHi;//高32位
	UINT32 ulUserStatApTxRSSI45toPktsLo;//低32位
	UINT32 ulUserStatApTxRSSI45toPktsHi;//高32位
}__attribute__ ((packed)) IE58_WLAN_STA_PARA_PACKRSSI_T;

typedef struct _T58_T_
{
	UINT8 ucRadioId;//Radio标识，1字节。
	UINT8 ucChannel;//本RADIO的当前信道，1字节
	UINT16 usStaNum;//一个Radio上的终端个数，最大为128。
#define MT_IE58_WLAN_STA_PARA_PACKRSSI_STANUM 128
	IE58_WLAN_STA_PARA_PACKRSSI_T astWlanStaParaPACKRSSI[MT_IE58_WLAN_STA_PARA_PACKRSSI_STANUM];
}__attribute__ ((packed)) MT_IE37_IE58_T;

//59  AP的终端统计（用户报文长度分布信息统计）
typedef struct _WLAN_STA_PATA_PACKLEN_T_
{
	UINT16 usWlanID;
	UINT8 aucStaMacAddr[MT_MAC_LENTH];
	UINT32 ulUserInlen0to128PktsLo;//低32位
	UINT32 ulUserInlen0to128PktsHi;//高32位
	UINT32 ulUserInlen128to256PktsLo;//低32位
	UINT32 ulUserInlen128to256PktsHi;//高32位
	UINT32 ulUserInlen256to512PktsLo;//低32位
	UINT32 ulUserInlen256to512PktsHi;//高32位
	UINT32 ulUserInlen512to768PktsLo;//低32位
	UINT32 ulUserInlen512to768PktsHi;//高32位
	UINT32 ulUserInlen768to1024PktsLo;//低32位
	UINT32 ulUserInlen768to1024PktsHi;//高32位
	UINT32 ulUserInlen1024to1536PktsLo;//低32位
	UINT32 ulUserInlen1024to1536PktsHi;//高32位
	UINT32 ulUserInlen1536to2048PktsLo;//低32位
	UINT32 ulUserInlen1536to2048PktsHi;//高32位
	UINT32 ulUserInlen2048to2560PktsLo;//低32位
	UINT32 ulUserInlen2048to2560PktsHi;//高32位
	UINT32 ulUserInlen2560to3072PktsLo;//低32位
	UINT32 ulUserInlen2560to3072PktsHi;//高32位
	UINT32 ulUserInlen3072to3584PktsLo;//低32位
	UINT32 ulUserInlen3072to3584PktsHi;//高32位
	UINT32 ulUserInlen3584toPktsLo;//低32位
	UINT32 ulUserInlen3584toPktsHi;//高32位
}__attribute__ ((packed)) IE59_WLAN_STA_PATA_PACKLEN_T;

typedef struct _T59_T_
{
	UINT8 ucRadioId;//Radio标识，1字节。
	UINT8 ucChannel;//本RADIO的当前信道，1字节
	UINT16 usStaNum;//一个Radio上的终端个数，最大为128。
#define MT_IE59_WLAN_STA_PATA_PACKLEN_STANUM 128
	IE59_WLAN_STA_PATA_PACKLEN_T astWlanStaParaPACKLEN[MT_IE59_WLAN_STA_PATA_PACKLEN_STANUM];
}__attribute__ ((packed)) MT_IE37_IE59_T;

//60  AP的终端统计（Rate报文统计）
typedef struct __T_
{
	UINT16 usWlanID;
	UINT8 aucStaMacAddr[MT_MAC_LENTH];
	UINT32 ulUserStatApTxRate1MbpsPktsLo;//低32位
	UINT32 ulUserStatApTxRate1MbpsPktsHi;//高32位
	UINT32 ulUserStatApTxRate2MbpsPktsLo;//低32位
	UINT32 ulUserStatApTxRate2MbpsPktsHi;//高32位
	UINT32 ulUserStatApTxRate55MbpsPktsLo;//低32位
	UINT32 ulUserStatApTxRate55MbpsPktsHi;//高32位
	UINT32 ulUserStatApTxRate6MbpsPktsLo;//低32位
	UINT32 ulUserStatApTxRate6MbpsPktsHi;//高32位
	UINT32 ulUserStatApTxRate9MbpsPktsLo;//低32位
	UINT32 ulUserStatApTxRate9MbpsPktsHi;//高32位
	UINT32 ulUserStatApTxRate11MbpsPktsLo;//低32位
	UINT32 ulUserStatApTxRate11MbpsPktsHi;//高32位
	UINT32 ulUserStatApTxRate12MbpsPktsLo;//低32位
	UINT32 ulUserStatApTxRate12MbpsPktsHi;//高32位
	UINT32 ulUserStatApTxRate18MbpsPktsLo;//低32位
	UINT32 ulUserStatApTxRate18MbpsPktsHi;//高32位
	UINT32 ulUserStatApTxRate24MbpsPktsLo;//低32位
	UINT32 ulUserStatApTxRate24MbpsPktsHi;//高32位
	UINT32 ulUserStatApTxRate36MbpsPktsLo;//低32位
	UINT32 ulUserStatApTxRate36MbpsPktsHi;//高32位
	UINT32 ulUserStatApTxRate48MbpsPktsLo;//低32位
	UINT32 ulUserStatApTxRate48MbpsPktsHi;//高32位
	UINT32 ulUserStatApTxRate54MbpsPktsLo;//低32位
	UINT32 ulUserStatApTxRate54MbpsPktsHi;//高32位
	UINT32 ulUserStatApRxRate1MbpsPktsLo;//低32位
	UINT32 ulUserStatApRxRate1MbpsPktsHi;//高32位
	UINT32 ulUserStatApRxRate2MbpsPktsLo;//低32位
	UINT32 ulUserStatApRxRate2MbpsPktsHi;//高32位
	UINT32 ulUserStatApRxRate55MbpsPktsLo;//低32位
	UINT32 ulUserStatApRxRate55MbpsPktsHi;//高32位
	UINT32 ulUserStatApRxRate6MbpsPktsLo;//低32位
	UINT32 ulUserStatApRxRate6MbpsPktsHi;//高32位
	UINT32 ulUserStatApRxRate9MbpsPktsLo;//低32位
	UINT32 ulUserStatApRxRate9MbpsPktsHi;//高32位
	UINT32 ulUserStatApRxRate11MbpsPktsLo;//低32位
	UINT32 ulUserStatApRxRate11MbpsPktsHi;//高32位
	UINT32 ulUserStatApRxRate12MbpsPktsLo;//低32位
	UINT32 ulUserStatApRxRate12MbpsPktsHi;//高32位
	UINT32 ulUserStatApRxRate18MbpsPktsLo;//低32位
	UINT32 ulUserStatApRxRate18MbpsPktsHi;//高32位
	UINT32 ulUserStatApRxRate24MbpsPktsLo;//低32位
	UINT32 ulUserStatApRxRate24MbpsPktsHi;//高32位
	UINT32 ulUserStatApRxRate36MbpsPktsLo;//低32位
	UINT32 ulUserStatApRxRate36MbpsPktsHi;//高32位
	UINT32 ulUserStatApRxRate48MbpsPktsLo;//低32位
	UINT32 ulUserStatApRxRate48MbpsPktsHi;//高32位
	UINT32 ulUserStatApRxRate54MbpsPktsLo;//低32位
	UINT32 ulUserStatApRxRate54MbpsPktsHi;//高32位
}__attribute__ ((packed)) IE60_WLAN_STA_PARA_PACKRATE_T;

typedef struct _T60_T_
{
	UINT8 ucRadioId;//Radio标识，1字节。
	UINT8 ucChannel;//本RADIO的当前信道，1字节
	UINT16 usStaNum;//一个Radio上的终端个数，最大为128。
#define MT_IE60_WLAN_STA_PARA_PACKRATE_STANUM 128
	IE60_WLAN_STA_PARA_PACKRATE_T astWlanStaParaPACKRate[MT_IE60_WLAN_STA_PARA_PACKRATE_STANUM];
}__attribute__ ((packed)) MT_IE37_IE60_T;

//61  地址重定向攻击通告
typedef struct _T61_T_
{
  UINT8 ucRadioId;
  UINT8 ucWlanId;
  UINT8 aucStaMac[MT_MAC_LENTH];
}__attribute__ ((packed)) MT_IE37_IE61_T;

//62  WAPI特性性能参数
typedef struct _AP_WAPI_PARA_T_
{
UINT8 aucStaMac[MT_MAC_LENTH];
UINT32 ulWPIReplayCntr;
UINT32 ulWPIDecryptableErrs;
UINT32 ulWPIMICErrs;
}__attribute__ ((packed)) MT_AP_WAPI_PARA_T;

typedef struct _T62_T_
{
  UINT8 ucRadioId;
  UINT8 ucWlanId;
  UINT32 ulStaNum;
#define MT_AP_WAPI_PARA_STA_CNT 64
  MT_AP_WAPI_PARA_T astApWapiPara[MT_AP_WAPI_PARA_STA_CNT];
}__attribute__ ((packed)) MT_IE37_IE62_T;

//63  AC/AP QoS基本性能参数
typedef struct _T63_T_
{
  UINT32 ulRadioId;
  UINT32 ulQueueAveLen;
  UINT32 ulLostPktsCnt;
  UINT32 ulAveRate;
}__attribute__ ((packed)) MT_IE37_IE63_T;

//64  AP BACKGROUD业务类QoS参数
typedef struct _T64_T_
{
  UINT32 ulRadioID;
  UINT32 ulAvgQueueLen;
  UINT32 ulAvgBurstPkts;
  UINT32 ulLostPkts;
  UINT32 ulApAvgRate;
  UINT32 ulLostTrafficNum;
}__attribute__ ((packed)) MT_IE37_IE64_T;

//65  AP BESTEFFORT业务类QoS参数
typedef struct _T65_T_
{
  UINT32 ulRadioID;
  UINT32 ulAvgQueueLen;
  UINT32 ulAvgBurstPkts;
  UINT32 ulApAvgRate;
  UINT32 ulLostPkts;
  UINT32 ulLostTrafficNum;
}__attribute__ ((packed)) MT_IE37_IE65_T;

//66  AP VOICE业务类QoS参数
typedef struct _T66_T_
{
  UINT32 ulRadioID;
  UINT32 ulQueueLen;
  UINT32 ulAvgBurstRate;
  UINT32 ulLostPktsRate;
  UINT32 ulAvgRate;
  UINT32 ulSucLineNum;
  UINT32 ulLostLineNum;
  UINT32 ulLostTrafficPkts;
  UINT32 ulRefusedNum;
}__attribute__ ((packed)) MT_IE37_IE66_T;

//67  AP Video 业务类QoS参数
typedef struct _T67_T_
{
  UINT32 ulRadioID;
  UINT32 ulQueueLen;
  UINT32 ulAvgBurstRate;
  UINT32 ulLostPktsRate;
  UINT32 ulAvgRate;
  UINT32 ulSucLineNum;
  UINT32 ulLostLineNum;
  UINT32 ulRefusedNum;
}__attribute__ ((packed)) MT_IE37_IE67_T;

//68  AP 用户关联统计
typedef struct _T68_T_
{
  UINT32 ulDot11bApUserAssoCnt;
  UINT32 ulDot11bApAssoFailCnt;
  UINT32 ulDot11bApReassoUserRate;
  UINT32 ulDot11gApUserAssoCnt;
  UINT32 ulDot11gApAssoFailCnt;
  UINT32 ulDot11gApReassoUserRate;
}__attribute__ ((packed)) MT_IE37_IE68_T;

//71  AP的终端统计（SNR报文统计）
typedef struct _IE71_AP_WLAN_STA_STATIS_PACKSNR_T_
{
	UINT16 usWlanID;
	UINT8 aucStaMacAddr[MT_MAC_LENTH];
	UINT32 ulUserStatApRxSNRto5PktsLo;//低32位
	UINT32 ulUserStatApRxSNRto5PktsHi;//高32位
	UINT32 ulUserStatApRxSNR5to15PktsLo;//低32位
	UINT32 ulUserStatApRxSNR5to15PktsHi;//高32位
	UINT32 ulUserStatApRxSNR15to20PktsLo;//低32位
	UINT32 ulUserStatApRxSNR15to20PktsHi;//高32位
	UINT32 ulUserStatApRxSNR20to25PktsLo;//低32位
	UINT32 ulUserStatApRxSNR20to25PktsHi;//高32位
	UINT32 ulUserStatApRxSNR25to30PktsLo;//低32位
	UINT32 ulUserStatApRxSNR25to30PktsHi;//高32位
	UINT32 ulUserStatApRxSNR30to35PktsLo;//低32位
	UINT32 ulUserStatApRxSNR30to35PktsHi;//高32位
	UINT32 ulUserStatApRxSNR30to40PktsLo;//低32位
	UINT32 ulUserStatApRxSNR30to40PktsHi;//高32位
	UINT32 ulUserStatApRxSNR40to45PktsLo;//低32位
	UINT32 ulUserStatApRxSNR40to45PktsHi;//高32位
	UINT32 ulUserStatApRxSNR45to50PktsLo;//低32位
	UINT32 ulUserStatApRxSNR45to50PktsHi;//高32位
	UINT32 ulUserStatApRxSNR50toPktsLo;//低32位
	UINT32 ulUserStatApRxSNR50toPktsHi;//高32位
	UINT32 ulUserStatApTxSNRto5PktsLo;//低32位
	UINT32 ulUserStatApTxSNRto5PktsHi;//高32位
	UINT32 ulUserStatApTxSNR5to15PktsLo;//低32位
	UINT32 ulUserStatApTxSNR5to15PktsHi;//高32位
	UINT32 ulUserStatApTxSNR15to20PktsLo;//低32位
	UINT32 ulUserStatApTxSNR15to20PktsHi;//高32位
	UINT32 ulUserStatApTxSNR20to25PktsLo;//低32位
	UINT32 ulUserStatApTxSNR20to25PktsHi;//高32位
	UINT32 ulUserStatApTxSNR25to30PktsLo;//低32位
	UINT32 ulUserStatApTxSNR25to30PktsHi;//高32位
	UINT32 ulUserStatApTxSNR30to35PktsLo;//低32位
	UINT32 ulUserStatApTxSNR30to35PktsHi;//高32位
	UINT32 ulUserStatApTxSNR30to40PktsLo;//低32位
	UINT32 ulUserStatApTxSNR30to40PktsHi;//高32位
	UINT32 ulUserStatApTxSNR40to45PktsLo;//低32位
	UINT32 ulUserStatApTxSNR40to45PktsHi;//高32位
	UINT32 ulUserStatApTxSNR45to50PktsLo;//低32位
	UINT32 ulUserStatApTxSNR45to50PktsHi;//高32位
	UINT32 ulUserStatApTxSNR50toPktsLo;//低32位
	UINT32 ulUserStatApTxSNR50toPktsHi;//高32位
}__attribute__ ((packed)) IE71_AP_WLAN_STA_STATIS_PACKSNR_T;

typedef struct _T71_T_
{
	UINT8 ucRadioId;//Radio标识，1字节。
	UINT8 ucChannel;//本RADIO的当前信道，1字节
	UINT16 usStaNum;//一个Radio上的终端个数，最大为128。
#define MT_IE71_AP_WLAN_STA_STATIS_PACKSNR_STANUM 128
	IE71_AP_WLAN_STA_STATIS_PACKSNR_T astWlanStaParaPACKSNR[MT_IE71_AP_WLAN_STA_STATIS_PACKSNR_STANUM];
}__attribute__ ((packed)) MT_IE37_IE71_T;

// 72  AP的终端统计（离线统计）
typedef struct _IE72_AP_WLAN_OFFSTA_STATIS_T_
{
	UINT16 usWlanID;
	UINT8 aucStaMacAddr[MT_MAC_LENTH];
	UINT32 ulUserStaAccessTimeDELAY;//接入时延
	UINT32 ulProbeRequestfirsttime;//接入请求探针发送时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
	UINT32 ulProbeRequestSum;//接入请求次数
	UINT32 ulProbeResponsetime;//接入请求探针响应时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
	UINT32 ulprobeFailReason;//接入失败原因
	UINT32 ulAssociationRequesttime;//关联请求时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
	UINT32 ulStatAssoReqSum;//关联请求次数
	UINT32 ulStatAssoRsptime;//关联响应时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
	UINT32 ulStatAssFailReason;//关联失败原因
	UINT32 ulUserStaReltime;//STA释放时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
	UINT32 ulUserStaRelReason;//STA释放原因
}__attribute__ ((packed)) IE72_AP_WLAN_OFFSTA_STATIS_T;

typedef struct _T72_T_
{
	UINT8 ucRadioId;//Radio标识，1字节。
	UINT8 ucChannel;//本RADIO的当前信道，1字节
	UINT16 usStaNum;//一个Radio上的终端个数，最大为128。
#define MT_IE72_AP_WLAN_OFFSTA_STATIS_STANUM 128
	IE72_AP_WLAN_OFFSTA_STATIS_T astWlanStaParaOFF[MT_IE72_AP_WLAN_OFFSTA_STATIS_STANUM];
}__attribute__ ((packed)) MT_IE37_IE72_T;

//78  AP无线接口性能统计，且基于每个Radio进行统计
typedef struct _T78_T_
{
  UINT8 ucRadioId;//Radio标识
  UINT8 ucRadioType;//Radio类型
  UINT8 ucCurrentChan;//当前信道
  UINT8 ucResv;//保留字
//  UINT32 ulRadioId;//Radio标识
  UINT32 ulApAvgSignalStrgth;//接收的信号平均强度，单位为dBm
  UINT32 ulApHighestRxStrgth;//接收的最高信号强度，单位为dBm
  UINT32 ulApLowestRxStrgth;//接收的最低信号强度，单位为dBm
  UINT32 ulWtpWIfUpdownTimes;//无线端口updown次数
  UINT32 ulWtpWIfTxDataPktsHi;//发送的数据包数高32位
  UINT32 ulWtpWIfTxDataPktsLo;//发送的数据包数低32位
  UINT32 ulWtpWIfRxDataPktsHi;//接收的数据包数高32位
  UINT32 ulWtpWIfRxDataPktsLo;//接收的数据包数低32位
  UINT32 ulWtpWIfUplinkDataOctetsHi;//接收的数据包字节数高32位，单位为Bytes
  UINT32 ulWtpWIfUplinkDataOctetsLo;//接收的数据包字节数低32位，单位为Bytes
  UINT32 ulWtpWIfDwlinkDataOctetsHi;//发送的数据包字节数高32位，单位为Bytes
  UINT32 ulWtpWIfDwlinkDataOctetsLo;//发送的数据包字节数低32位，单位为Bytes
  UINT32 ulWtpWIfChStatsDwlinkTotRetryPktsHi;//无线信道下行重传的数据包数高32位
  UINT32 ulWtpWIfChStatsDwlinkTotRetryPktsLo;//无线信道下行重传的数据包数低32位
  UINT32 ulWtpWIfRxMgmtFrameCntHi;//接收到的管理帧的数量高32位
  UINT32 ulWtpWIfRxMgmtFrameCntLo;//接收到的管理帧的数量低32位
  UINT32 ulWtpWIfRxCtrlFrameCntHi;//接收到的控制帧的数量高32位
  UINT32 ulWtpWIfRxCtrlFrameCntLo;//接收到的控制帧的数量低32位
  UINT32 ulWtpWIfRxDataFrameCntHi;//接收到的数据帧的数量高32位
  UINT32 ulWtpWIfRxDataFrameCntLo;//接收到的数据帧的数量低32位
  UINT32 ulWtpWIfTxMgmtFrameCntHi;//发送到的管理帧的数量高32位
  UINT32 ulWtpWIfTxMgmtFrameCntLo;//发送到的管理帧的数量低32位
  UINT32 ulWtpWIfTxCtrlFrameCntHi;//发送到的控制帧的数量高32位
  UINT32 ulWtpWIfTxCtrlFrameCntLo;//发送到的控制帧的数量低32位
  UINT32 ulWtpWIfTxDataFrameCntHi;//接收到的数据帧的数量高32位
  UINT32 ulWtpWIfTxDataFrameCnt;//接收到的数据帧的数量低32位
  UINT32 ulWtpWIfRetryCntHiLo;//无线信道下行重传的帧数高32位
  UINT32 ulWtpWIfRetryCntLo;//无线信道下行重传的帧数低32位
  UINT32 ulWtpWIfChStatsPhyErrPktsHi;//接收的错误数据包数高32位
  UINT32 ulWtpWIfChStatsPhyErrPktsLo;//接收的错误数据包数低32位
  UINT32 ulWtpWIfChStatsFrameErrorCntHi;//接收的错帧数高32位
  UINT32 ulWtpWIfChStatsFrameErrorCntLo;//接收的错帧数低32位
  UINT32 ulWtpWIfTxChStatsPhyErrPktsHi;//发送的错误数据包数高32位
  UINT32 ulWtpWIfTxChStatsPhyErrPktsLo;//发送的错误数据包数低32位
  UINT32 ulWtpWIfChStatsMacFcsErrPktsHi;//接收的FCS MAC错帧数高32位
  UINT32 ulWtpWIfChStatsMacFcsErrPktsLo;//接收的FCS MAC错帧数低32位
  UINT32 ulWtpWIfChStatsMacMicErrPktsHi;//接收的MIC MAC错帧数高32位
  UINT32 ulWtpWIfChStatsMacMicErrPktsLo;//接收的MIC MAC错帧数低32位
  UINT32 ulWtpWIfChStatsMacDecryptErrPktsHi;//接收的解密失败的MAC错帧数高32位
  UINT32 ulWtpWIfChStatsMacDecryptErrPktsLo;//接收的解密失败的MAC错帧数低32位
  UINT32 ulWtpWIfUplinkThroughout;//无线上行端口的流量，单位为Kbps
  UINT32 ulWtpWIfDownlinkThroughout;//无线下行端口的流量，单位为Kbps
  UINT32 ulWtpWIfUplinkFrameDiscardCntHi;//信道上行总的丢帧数高32位
  UINT32 ulWtpWIfUplinkFrameDiscardCntLo;//信道上行总的丢帧数低32位
  UINT32 ulWtpWIfUplinkFrameRetryCntHi;//信道上行总的重传帧数高32位
  UINT32 ulWtpWIfUplinkFrameRetryCntLo;//信道上行总的重传帧数低32位
  UINT32 ulWtpWIfUplinkFrameTotalCntHi;//信道上行总的帧数高32位
  UINT32 ulWtpWIfUplinkFrameTotalCntLo;//信道上行总的帧数低32位
  UINT32 ulWtpWIfDownlinkFrameTotalCntHi;//信道下行总的帧数高32位
  UINT32 ulWtpWIfDownlinkFrameTotalCntLo;//信道下行总的帧数低32位
  UINT32 ulWtpWIfDownlinkFrameErrorCntHi;//信道下行总的错帧数高32位
  UINT32 ulWtpWIfDownlinkFrameErrorCntLo;//信道下行总的错帧数低32位
  UINT32 ulWtpWIfDownlinkFrameDiscardCntHi;//信道下行总的丢帧数高32位
  UINT32 ulWtpWIfDownlinkFrameDiscardCntLo;//信道下行总的丢帧数低32位
  UINT32 ulWtpWIfDownlinkFrameRetryCntHi;//信道下行总的重传帧数高32位
  UINT32 ulWtpWIfDownlinkFrameRetryCntLo;//信道下行总的重传帧数低32位
  UINT32 ulWtpWIfRxDataFrameOctetsHi;//接收的数据帧字节数高32位，单位为Bytes
  UINT32 ulWtpWIfRxDataFrameOctetsLo;//接收的数据帧字节数低32位，单位为Bytes
  UINT32 ulWtpWIfTxDataFrameOctetsHi;//发送的数据帧字节数高32位，单位为Bytes
  UINT32 ulWtpWIfTxDataFrameOctetsLo;//发送的数据帧字节数低32位，单位为Bytes
  UINT32 ulWtpWIfRxAssocFrameOctetsHi;//发送的关联帧的数量高32位
  UINT32 ulWtpWIfRxAssocFrameOctetsLo;//发送的关联帧的数量低32位
  UINT32 ulWtpWIfTxAssocFrameOctetsHi;//接收到的关联帧的数量高32位
  UINT32 ulWtpWIfTxAssocFrameOctetsLo;//接收到的关联帧的数量低32位
  UINT32 ulWtpFhhopTimes;// 跳频次数
  UINT32 ulWtpWIfCurTxPwr;//当前AP的发射功率
  UINT32 ulSampleLenth;//抽样时长，单位为秒
//  UINT32 ulChStatsChannel;//AP当前占用的频点（1~13）
  UINT32 ulChStatsNumSta;//使用该信道的终端数
  UINT8 aucChStatsFrameErrRate[8];//信道的错帧率，单位为%，为8字节字符串
  UINT8 aucChStatsFrameRetryRate[8];//信道上帧的重传率，单位为%，为8字节字符串
  UINT32 ulMonitoredTime;//监控时长，单位为秒
  UINT32 ulchanUseRate;//AP工作频点空间占用率
  UINT16 usShtRetryThld;//无线接口帧长小于RTS门限时最大重试次数
  UINT16 usLongRetryThld;//无线接口帧长大于RTS门限时最大重试次数
  UINT8 ucWlanNum;//WLAN个数
  UINT8 aucResvd[3];//保留字，3字节
  UINT8 aucAPNeighborSSIDList[16 * MT_SSID_LEN];// AP周围的SSID列表，每组为32字节字符串，最多16组
}__attribute__ ((packed)) MT_IE37_IE78_T;

//79  AP的终端统计
typedef struct _WLAN_STA_T_
{
//  UINT8 ucWlanID;//Wlan标识
//  UINT8 aucResvd1[3];//保留字，3字节
  UINT16 usWlanId;//Wlan标识(2字节)
  UINT8 aucBssID[MT_MAC_LENTH];//BSSID：本WlanID的BSSID（6字节）
  UINT8 aucStaMac[MT_MAC_LENTH];//连接到AP的终端的MAC地址
  UINT8 aucResvd2[2];//保留字，2字节
#define MT_IE37_IE79_WMM_SUPPORT 1
#define MT_IE37_IE79_NONWMM_SUPPORT 2
  UINT8 ucStaWMMAttr;//1-WMMSta:支持QoS机制的终端，2-NonWMMSta:不支持QoS机制的终端
#define MT_IE37_IE79_PWR_ACTIVE 1
#define MT_IE37_IE79_PWR_SAVE 2
  UINT8 ucStaPowerSaveMode;//终端节电状态1：active, 2：powersave
  UINT8 ucStaRadiosType;//按位的最后4 位，可以组合,基本定义类型如下：
  //0 1 2 3 4 5 6 7
  //+-+-+-+-+-+-+-+-+
  //|Reservd|N|G|A|B|
  //+-+-+-+-+-+-+-+-+
  //8: An IEEE 802.11n radio.
  //4: An IEEE 802.11g radio.
  //2: An IEEE 802.11a radio.
  //1: An IEEE 802.11b radio.
  UINT8 ucResvd;//保留字，1字节
  UINT32 ulUserStatStaUpTime;//终端关联时长，单位为0.01秒
  UINT32 ulUserStatResourceUseRate;
  UINT32 ulChanRxUseTime;//上行信道有效占用时长
  UINT32 ulChanTxUseTime;//下行信道有效占用时长
  UINT32 ulChanTotalUseTime;//总空口有效占用时长
  UINT32 ulChanRxTotalUseTime;//上行信道占用总时长
  UINT32 ulChanTxTotalUseTime;//下行信道占用总时长
  UINT32 ulChanTxTotalRTTTime;//下行数据包传输总时长
  UINT32 ulChanRxTotalRTTTime;//上行数据包传输总时长
  UINT32 ulAPRecvdStaSignalStrthACKTime;//AP发出数据帧后收到STA回送的ACK控制帧的时间（单位：毫秒）。
  INT32 ilAPRecvdStaSignlStrthACK;//AP发出数据帧后收到STA回送的ACK控制帧的信号强度（有符号型，单位：dBm)。
  INT32 ilAPRecvdStaSignalStrth;//AP接收到的终端的当前信号强度，有符号型，单位：dBm
  UINT32 ulAPRecvdStaSNR;//AP接收到的终端的当前信噪比，单位：dB
  UINT32 ulStaTxPktsHi;//发到终端的包数高32位
  UINT32 ulStaTxPktsLo;//发到终端的包数低32位
  UINT32 ulStaTxBytesHi;//发到终端的字节数高32位，单位为Bytes
  UINT32 ulStaTxBytesLo;//发到终端的字节数低32位，单位为Bytes
  UINT32 ulStaRxPktsHi;//从终端收到的包数高32位
  UINT32 ulStaRxPktsLo;//从终端收到的包数低32位
  UINT32 ulStaRxBytesHi;//从终端收到的字节数高32位，单位为Bytes
  UINT32 ulStaRxBytesLo;//从终端收到的字节数低32位，单位为Bytes
  UINT32 ulAPTxRatesDown;//终端当前接入速率（下行），单位为Kbps
  UINT32 ulAPTxRatesUp;//终端当前接入速率（上行），单位为Kbps
  UINT32 ulAPTxRatesAvgDown;//终端平均速接入速率（下行），单位为Kbps
  UINT32 ulAPTxRatesAvgUp;//终端平均速接入速率（上行），单位为Kbps
  UINT32 ulStaTxRetryPktsHi;//终端重发的包数高32位
  UINT32 ulStaTxRetryPktsLo;//终端重发的包数低32位
  UINT32 ulStaRxRetryPktsHi;//终端重收的包数高32位
  UINT32 ulStaRxRetryPktsLo;//终端重收的包数低32位
  UINT32 ulStaReceiveErrPktsHi;//接收到的来自终端的错误包数高32位
  UINT32 ulStaReceiveErrPktsLo;//接收到的来自终端的错误包数低32位
  UINT32 ulUserStatApRxFrameCntHi;//AP接收到用户的数据帧数高32位
  UINT32 ulUserStatApRxFrameCntLo;//AP接收到用户的数据帧数低32位
  UINT32 ulUserStatApRxErrFrameCntHi;//AP接收到用户的错误帧数高32位
  UINT32 ulUserStatApRxErrFrameCntLo;//AP接收到用户的错误帧数低32位
  UINT32 ulUserStatApTxFrameCntHiLo;//AP发送给用户的数据帧数高32位
  UINT32 ulUserStatApTxFrameCntLo;//AP发送给用户的数据帧数低32位
  UINT32 ulUserStatApTxSuccFrameCntHi;//AP成功发送到用户的数据帧数高32位
  UINT32 ulUserStatApTxSuccFrameCntLo;//AP成功发送到用户的数据帧数低32位
  UINT32 ulUserStatApRxRetryBytesHi;//AP接收到用户的重传数据字节数高32位，单位为Bytes
  UINT32 ulUserStatApRxRetryBytesLo;//AP接收到用户的重传数据字节数低32位，单位为Bytes
  UINT32 ulUserStatApTxRetryBytesHi;//AP发送给用户的重传数据字节数高32位，单位为Bytes
  UINT32 ulUserStatApTxRetryBytesLo;//AP发送给用户的重传数据字节数低32位，单位为Bytes
  UINT32 ulUserRxCtrlFrameCntHi;//接收到的控制帧的数量高32位
  UINT32 ulUserRxCtrlFrameCntLo;//接收到的控制帧的数量低32位
  UINT32 ulUserRxMgmtFrameCntHi;//接收到的管理帧的数量高32位
  UINT32 ulUserRxMgmtFrameCntLo;//接收到的管理帧的数量低32位
  UINT32 ulUserRxDataFrameCntHi;//接收到的数据帧的数量高32位
  UINT32 ulUserRxDataFrameCntLo;//接收到的数据帧的数量低32位
  UINT32 ulUserTxCtrlFrameCntHi;//发送到的控制帧的数量高32位
  UINT32 ulUserTxCtrlFrameCntLo;//发送到的控制帧的数量低32位
  UINT32 ulUserTxMgmtFrameCntHi;//发送到的管理帧的数量高32位
  UINT32 ulUserTxMgmtFrameCntLo;//发送到的管理帧的数量低32位
  UINT32 ulUserTxDataFrameCntHi;//接收到的数据帧的数量高32位
  UINT32 ulUserTxDataFrameCntLo;//接收到的数据帧的数量低32位
  UINT32 ulUserInUcastPktsHi;//端口接收单播包数高32位
  UINT32 ulUserInUcastPktsLo;//端口接收单播包数低32位
  UINT32 ulUserInNUcastPktsHi;//端口接收非单播包数高32位
  UINT32 ulUserInNUcastPktsLo;//端口接收非单播包数低32位
  UINT32 ulUserStatApTxRetryFrameCntHi;//AP发送给用户的重传数据帧数高32位
  UINT32 ulUserStatApTxRetryFrameCntLo;//AP发送给用户的重传数据帧数低32位
  UINT32 ulUserStatApTxErrPktsHi;//AP发送给用户的错误包数高32位
  UINT32 ulUserStatApTxErrPktsLo;//AP发送给用户的错误包数低32位
  UINT32 ulUserStatTotalErrPktsHi;//总错误包数高32位
  UINT32 ulUserStatTotalErrPktsLo;//总错误包数低32位
  UINT32 ulUserStaAccessTime;//用户的接入时间点
  UINT32 ulUserStaAccessTimeDELAY;//接入时延 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
  UINT32 ulProbeRequestfirsttime;//接入请求探针发送时刻
  UINT32 ulProbeRequestSum;//接入请求次数
  UINT32 ulProbeResponsetime;//接入请求探针响应时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
  UINT32 ulAssociationRequesttime;//关联请求时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
  UINT32 ulStatAssoReqSum;//关联请求次数
  UINT32 ulStatAssoRspTime;//关联响应时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
  UINT32 ulStationOnlineSTART;//论证时刻
  UINT32 ulStationOnlineTOTALTIME;//在线总时长
  UINT32 ulUserifUplinkDataThroughput;//以太网口发送流量[MB]（ 以太网上行端口流量）单位为kbps
  UINT32 ulUserifDownlinkDataThroughput;//以太网口接收流量[MB] （以太网下行端口流量）单位为kbps
  UINT8 aucUserifMAC[MT_MAC_LENTH];//以太网口MAC地址
  UINT8 aucRecvd[2];
  UINT32 ulUserifMtu;//以太网口MTU值
}__attribute__ ((packed)) MT_IE37_IE79_WLAN_STA_PARA_T;

typedef struct _T79_T_
{
  UINT8 ucRadioID;//Radio标识
//  UINT8 aucResvd[3];//保留字，3字节
//  UINT32 ulStaNum;//一个Radio上的终端个数，最大为128
  UINT8 ucChannel;//本RADIO的当前信道，1字节
  UINT16 usStaNum;//一个Radio上的终端个数，最大为128
#define MT_IE37_IE79_STANUM 128
  MT_IE37_IE79_WLAN_STA_PARA_T astWlanStaPara[MT_IE37_IE79_STANUM];//终端统计信息列表，最多128个终端
}__attribute__ ((packed)) MT_IE37_IE79_T;

//80  AP的SSID性能统计，且基于AP中每个Radio的每个SSID进行统计
typedef struct _IE80_WLAN_STA_PARA_T_
{
  UINT8 ucWlanID;//Radio标识
  UINT8 aucResvd1[3];//保留字，3字节
  UINT8 aucWlanStatSSID[MT_SSID_LEN];//SSID，32字节字符串。
  UINT8 aucBSSID[MT_MAC_LENTH];//Wlan Mac
  UINT8 aucResvd2[2];//保留字，2字节
  UINT32 ulWlanStatTxDataPktsHi;// 发送的数据包数高32位
  UINT32 ulWlanStatTxDataPktsLo;// 发送的数据包数低32位
  UINT32 ulWlanStatRxDataPktsHi;// 接收的数据包数高32位
  UINT32 ulWlanStatRxDataPktsLo;// 接收的数据包数低32位
  UINT32 ulWlanStatUplinkDataOctetsHi;// 接收的数据包字节数高32位，单位为Bytes
  UINT32 ulWlanStatUplinkDataOctetsLo;// 接收的数据包字节数低32位，单位为Bytes
  UINT32 ulWlanStatDwlinkDataOctetsHi;// 发送的数据包字节数高32位，单位为Bytes
  UINT32 ulWlanStatDwlinkDataOctetsLo;// 发送的数据包字节数低32位，单位为Bytes
  UINT32 ulWlanStatChStatsDwlinkTotRetryPktsHi;// 下行重传的包数高32位
  UINT32 ulWlanStatChStatsDwlinkTotRetryPktsLo;// 下行重传的包数低32位
  UINT32 ulWlanStatApChStatsNumStations;// 当前连接到该SSID的终端数
  UINT32 ulWlanStatApStationOnlineSum;// 当前SSID下在线的终端数
  UINT32 ulWlanStatUplinkThroughout;//无线上行端口的流量，单位为Kbps
  UINT32 ulWlanStatDownlinkThroughout;//无线下行端口的流量，单位为Kbps
  UINT32 ulWlanStatUplinkFrameDiscardCntHi;//信道上行总的丢帧数高32位
  UINT32 ulWlanStatUplinkFrameDiscardCntLo;//信道上行总的丢帧数低32位
  UINT32 ulWlanStatUplinkFrameRetryCntHi;//信道上行总的重传帧数高32位
  UINT32 ulWlanStatUplinkFrameRetryCntLo;//信道上行总的重传帧数低32位
  UINT32 ulWlanStatUplinkFrameTotalCntHi;// 信道上行总的帧数高32位
  UINT32 ulWlanStatUplinkFrameTotalCntLo;// 信道上行总的帧数低32位
  UINT32 ulWlanStatDownlinkFrameTotalCntHi;// 信道下行总的帧数高32位
  UINT32 ulWlanStatDownlinkFrameTotalCntLo;// 信道下行总的帧数低32位
  UINT32 ulWlanStatDownlinkFrameErrorCntHi;// 信道下行总的错帧数高32位
  UINT32 ulWlanStatDownlinkFrameErrorCntLo;// 信道下行总的错帧数低32位
  UINT32 ulWlanStatDownlinkFrameDiscardCntHi;// 信道下行总的丢帧数高32位
  UINT32 ulWlanStatDownlinkFrameDiscardCntLo;// 信道下行总的丢帧数低32位
  UINT32 ulWlanStatDownlinkFrameRetryCntHi;//信道下行总的重传帧数高32位
  UINT32 ulWlanStatDownlinkFrameRetryCntLo;//信道下行总的重传帧数低32位
  UINT32 ulWlanStatRxDataFrameOctetsHi;//接收的数据帧字节数高32位，单位为Bytes
  UINT32 ulWlanStatRxDataFrameOctetsLo;//接收的数据帧字节数低32位，单位为Bytes
  UINT32 ulWlanStatTxDataFrameOctetsHi;//发送的数据帧字节数高32位，单位为Bytes
  UINT32 ulWlanStatTxDataFrameOctetsLo;//发送的数据帧字节数低32位，单位为Bytes
  UINT32 ulWlanStatRxAssocFrameOctetsHi;//发送的关联帧的数量高32位
  UINT32 ulWlanStatRxAssocFrameOctetsLo;//发送的关联帧的数量低32位
  UINT32 ulWlanStatTxAssocFrameOctetsHi;//接收到的关联帧的数量高32位
  UINT32 ulWlanStatTxAssocFrameOctetsLo;//接收到的关联帧的数量低32位
  UINT32 ulWlanStatResourceUseRate;//资源利用率, AP设备空中接口资源利用情况，以时间占用情况计算，单位为%
  UINT32 ulWlanStatAssoReqSum;//用户请求接入次数（计数器）
  UINT32 ulWlanStatAssoRspSum;//AP响应用户接入请求次数（计数器）
  UINT32 ulWlanStatAssoSuccSum;//用户成功接入次数（计数器）
  UINT32 ulWlanStatApChStatsNumStations_ever;//所有曾经关联到此SSID的用户总数.
}__attribute__ ((packed)) MT_IE37_IE80_WLAN_STA_PARA_T;

typedef struct _T80_T_
{
  UINT8 ucRadioID;//Radio标识
  UINT8 ucWlanNum;//携带的SSID性能统计信息个数
  UINT8 aucResvd[2];//保留字，2字节
#define MT_IE37_IE80_WLANNUM 16
  MT_IE37_IE80_WLAN_STA_PARA_T astWlanStaPara[MT_IE37_IE80_WLANNUM];//SSID性能统计信息列表，最多16个SSID
}__attribute__ ((packed)) MT_IE37_IE80_T;

//81  AP系统性能统计
typedef struct _T81_T_
{
  UINT8 aucWtpCPUType[32];//AP的CPU类型，32字节字符串
  UINT32 ulWtpCPURateUsage;//CPU实时利用率，单位为%
  UINT32 ulWtpCPUPeakUsage;//CPU峰值利用率，单位为%
  UINT32 ulWtpCPUAvgUsage;//CPU平均利用率，单位为%
  UINT8 aucWtpMemoryType[32];//AP的内存类型，32字节字符串
  UINT32 ulWtpMemorySize;//内存总数，单位为KB
  UINT32 ulWtpMemoryRemain;//内存可用数，单位为KB
  UINT32 ulWtpMemRateUsage;//内存实时利用率，单位为%
  UINT32 ulWtpMemPeakUsag;//内存峰值利用率，单位为%
  UINT32 ulWtpMemAvgUsagee;//内存平均利用率，单位为%
  UINT32 ulWtpFlashSize;//闪存总数，单位为KB
  UINT32 ulWtpFlashRemain;//闪存可用数，单位为KB
  UINT32 ulWtpFlashRateUsage;//闪存实时利用率，单位为%
  UINT32 ulWtpTemperature;//AP的当前温度
}__attribute__ ((packed)) MT_IE37_IE81_T;

//82  AP有线接口性能统计，基于每个AP设备的每个有线接口进行统计。每个有线接口包含一个本参数。从AP启动开始统计。且上报从AP启动开始统计以来的统计总量。
typedef struct _T82_T_
{
	 UINT16 usMtu;
	 UINT16 usInterfaceRate;
  UINT32 ulWtpEthifInUcastPktsHi;//端口接收单播包数高32位
  UINT32 ulWtpEthifInUcastPktsLo;//端口接收单播包数低32位
  UINT32 ulWtpEthifInNUcastPktsHi;//端口接收非单播包数高32位
  UINT32 ulWtpEthifInNUcastPktsLo;//端口接收非单播包数低32位
  UINT32 ulWtpEthifInOctetsHi;//端口接收的总字节数高32位，从AP启动开始统计，单位为Bytes
  UINT32 ulWtpEthifInOctetsLo;//端口接收的总字节数低32位，从AP启动开始统计，单位为Bytes
  UINT32 ulWtpEthifInDiscardPktsHi;//端口丢弃接收到的包数高32位
  UINT32 ulWtpEthifInDiscardPktsLo;//端口丢弃接收到的包数低32位
  UINT32 ulWtpEthifInErrorsHi;//端口接收到的错误包数高32位
  UINT32 ulWtpEthifInErrorsLo;//端口接收到的错误包数低32位
  UINT32 ulWtpEthifOutUcastPktsHi;//端口发送单播包数高32位
  UINT32 ulWtpEthifOutUcastPktsLo;//端口发送单播包数低32位
  UINT32 ulWtpEthifOutNUcastPktsHi;//端口发送非单播包数高32位
  UINT32 ulWtpEthifOutNUcastPktsLo;//端口发送非单播包数低32位
  UINT32 ulWtpEthifOutOctetsHi;//端口发送的总字节数高32位，从AP启动开始统计，单位为Bytes
  UINT32 ulWtpEthifOutOctetsLo;//端口发送的总字节数低32位，从AP启动开始统计，单位为Bytes
  UINT32 ulWtpEthifOutDiscardPktsHi;//端口丢弃要发送的包数高32位
  UINT32 ulWtpEthifOutDiscardPktsLo;//端口丢弃要发送的包数低32位
  UINT32 ulWtpEthifOutErrorsHi;//端口发送错误的包数高32位
  UINT32 ulWtpEthifOutErrorsLo;//端口发送错误的包数低32位
  UINT32 ulWtpEthifUpDwnTimes;//端口updown次数
  UINT32 ulWtpEthifDownlinkDataThroughput;//以太网下行端口流量，单位为kbps
  UINT32 ulWtpEthifUplinkDataThroughput;//以太网上行端口流量，单位为kbps
  UINT32 ulWtpEthifInBcastPktsHi;//端口接收广播包数高32位
  UINT32 ulWtpEthifInBcastPktsLo;//端口接收广播包数低32位
  UINT32 ulWtpEthifInMcastPktsHi;//端口接收组播包数高32位
  UINT32 ulWtpEthifInMcastPktsLo;//端口接收组播包数低32位
  UINT32 ulWtpEthifOutBcastPktsHi;//端口发送广播包数高32位
  UINT32 ulWtpEthifOutBcastPktsLo;//端口发送广播包数低32位
  UINT32 ulWtpEthifOutMcastPktsHi;//端口发送组播包数高32位
  UINT32 ulWtpEthifOutMcastPktsLo;//端口发送组播包数低32位
  UINT32 ulWtpEthifInFrameCntHi;//接收总的帧数高32位
  UINT32 ulWtpEthifInFrameCntLo;//接收总的帧数低32位
  UINT32 ulWtpEthifInFlow;//接收流量, 单位为kbps
  UINT32 ulWtpEthifInUcastFrameCntHi;//端口接收单播帧数高32位
  UINT32 ulWtpEthifInUcastFrameCntLo;//端口接收单播帧数低32位
  UINT32 ulWtpEthifInBcastFrameCntHi;//端口接收广播帧数高32位
  UINT32 ulWtpEthifInBcastFrameCntLo;//端口接收广播帧数低32位
  UINT32 ulWtpEthifInMcastFrameCntHi;//端口接收组播帧数高32位
  UINT32 ulWtpEthifInMcastFrameCntLo;//端口接收组播帧数低32位
  UINT32 ulWtpEthifOutFrameCntHi;//发送总的帧数高32位
  UINT32 ulWtpEthifOutFrameCntLo;//发送总的帧数低32位
  UINT32 ulWtpEthOutFlow;//发送流量, 单位为kbps
  UINT32 ulWtpEthifOutUcastFrameCntHi;//端口发送单播帧数高32位
  UINT32 ulWtpEthifOutUcastFrameCntLo;//端口发送单播帧数低32位
  UINT32 ulWtpEthifOutBcastFrameCntHi;//端口发送广播帧包数高32位
  UINT32 ulWtpEthifOutBcastFrameCntLo;//端口发送广播帧包数低32位
  UINT32 ulWtpEthifOutMcastFrameCntHi;//端口发送组播帧数高32位
  UINT32 ulWtpEthifOutMcastFrameCntLo;//端口发送组播帧数低32位
}__attribute__ ((packed)) MT_IE37_IE82_T;

//83  回传型AP上报的回传链路性能报告。
typedef struct _IE37_IE83_WLAN_SUB_ELEMNT_T_
{
  UINT8 ucWlanId;//回传网络标识编号
  UINT8 aucRecvd[2];
  UINT8 ucSSIDLen;//回传网络的服务集标识符长度，不超过32。
  UINT8 aucSSID[MT_SSID_LEN];//回传网络的服务集标识符，是ASCII形式的字符串，不超过32个字符。
}__attribute__ ((packed)) MT_IE37_IE83_WLAN_SUB_ELEMNT_T;

typedef struct _T83_T_
{
  UINT8 ucRadioID;//Radio ID。
  UINT8 ucCurrentChan;//信道号。
  UINT8 aucBSSID[MT_MAC_LENTH];//基本服务集识，这里标识本端Radio口的MAC地址。
  UINT8 aucNbrMac[MT_MAC_LENTH];//邻居Radio口的MAC地址（ peer-mac-address）。
#define MT_MESH_TYPE_NONE 0
#define MT_MESH_TYPE_NAP 1
#define MT_MESH_TYPE_SAP 2
  UINT8 ucAPTransfersType;//Mesh设备当前类型，0：NONE，1：NAP，2：SAP。
  INT8 icRSSI;//一字节。Signal Strength Indication (RSSI) 。单位：dBm。
  UINT8 ucSNR;//一字节。signal-to-noise ratio 。单位：dB。
#define MT_LINK_STATE_UP 0
#define MT_LINK_STATE_DOWN 1
  UINT8 ucLinkstate;//链路状态，0：up，1：down。
#define MT_LINK_MorS_STATE_MASTER 0
#define MT_LINK_MorS_STATE_SLAVE 1
  UINT8 ucLinkMorS;//链路主备状态，0：主用，1：备用。
  UINT8 ucRecvd;
  UINT32 ulUptime;//当前WDS链路UP时间，单位：秒。
  UINT8 ucWlanNum;//回传网络标识的个数, 最多16个。
  UINT8 aucRecvd[3];
#define MT_IE37_IE83_WLAN_NUM 16
  MT_IE37_IE83_WLAN_SUB_ELEMNT_T astWlanSubElemnt[MT_IE37_IE83_WLAN_NUM];//回传网络标识的描述子元素
}__attribute__ ((packed)) MT_IE37_IE83_T;

//84 同步两则
typedef struct _STA_MAC_LIST_T_
{
  UINT8 ucMacLen;//固定为6
  UINT8 aucStaMac[MT_MAC_LENTH];
}__attribute__ ((packed)) MT_STA_MAC_LIST_T;

typedef struct _T84_T_
{
  UINT8 ucRadioId;
  UINT8 ucWlanId;
  UINT8 aucBSSID[MT_MAC_LENTH];
#define MT_IE37_IE84_STA_ENTRIES_NUM 2
  UINT8 ucNumOfStaEntries;//用户实体数目,最大128
//  UINT8 ucMacLen;//固定为6
//  UINT8 aucStaMacList[128*MT_MAC_LENTH];//用户MAC地址列表
  MT_STA_MAC_LIST_T astStaMac[MT_IE37_IE84_STA_ENTRIES_NUM];//用户MAC地址列表
}__attribute__ ((packed)) MT_IE37_IE84_T;

//WTP Radio If (IE=85)AP Radio的信息
typedef struct _T85_T_
{
	UINT32 ulRadioId;//Radio标识
	UINT8 aucWtpWifDescr[16];//接口描述，为16字节字符串
	UINT32 ulIfType;//接口类型
	UINT32 ulWtpWifMtu;//MTU，最大传输单元
	UINT32 ulWtpWifSpeed;//接口带宽，单位为Mbps
	UINT8 aucWtpWifMacAddress[6];//接口Mac地址
	UINT8 aucResvd[2];
	UINT32 ulDot11CurrntCh;//AP当前频道ch1(1), ch2(2), ch3(3), ch4(4), ch5(5),ch6(6), ch7(7), ch8(8), ch9(9), ch10(10), ch11(11), ch12(12), ch13(13), ch149(149), ch153(153), ch157(157), ch161(161), ch165(165)。
	UINT32 ulRadioType;//Radio类型，0x01-11b，0x02-11a，0x04-11g，0x08-11n。 注释：Radio模式和CAPWAP协议标准定义不一致但和中移动网管定义的一致。
	UINT8 ucMaxTxPowerLevel[12];//最大发射功率，12字节字符串，单位为dBm。如：功率值为-5dBm，则表示为“-5”；功率值为5.5dBm，则表示为“5.5”。注: 放装型设备发射功率范围为(0-20), 分布型设备发射功率范围为(20-27)。
	UINT32 ulPwrAttRange;//功率衰减范围
	UINT32 ulPwrAttValue;//功率调整步长
	UINT32 ulDot11AntennaGain;//AP 802.11天线增益
	UINT32 ulWtpWifOperStatus;//接口当前状态，0:up 1:down  2:admindown
	UINT32 ulWifLastChange;//接口进入当前工作状态持续的时间，单位为0.01秒
}__attribute__ ((packed)) MT_IE37_IE85_T;


//88  AP连接信息统计，属于AP侧全局统计。从AP启动开始统计。且上报从AP启动开始统计以来的统计总量。
typedef struct _T88_T_
{
  UINT32 ulApStationAssocSum;//当前与AP关联的终端数
  UINT32 ulApStationOnlineSum;//当前AP下在线的终端数
  UINT32 ulApStationAssocSumMAX;//在线时段内AP最大附着用户数
  UINT32 ulAuthReqSum;
  UINT32 ulAuthSuccRespSum;
  UINT32 ulAuthFailRespSum;
  UINT32 ulAssocTimes;//关联总次数
  UINT32 ulAssocSucceedTimes;//关联成功总次数
  UINT32 ulAssocFailTimes;//关联失败总次数
  UINT32 ulReassocTimes;//重新关联总次数
  UINT32 ulPreAssCannotShiftDeassocFailSum;//由于之前的关联无法识别与转移而导致重新关联失败的总次数
  UINT32 ulApStatsDisassociated;//终端异常断开连接的总次数
  UINT32 ulAssocRejectSum;//由于接入点资源有限而拒绝关联的总次数
  UINT32 ulBSSNotSupportAssocFailSum;//因终端不支持基本速率集要求的速率而关联失败的总次数
  UINT32 ulReassocSuccessSum;//用户重关联成功次数
  UINT32 ulRejectReassocFailSum;//因拒绝而导致用户重关联失败的次数
  UINT32 ulAuthSuccessSum;//用户鉴别成功次数
  UINT32 ulPwdErrAuthFailSum;//因密码错误而导致用户鉴别失败的次数
  UINT32 ulInvalidAuthFailSum;//因无效而导致用户鉴别失败的次数
  UINT32 ulRejectAuthFailSum;//因拒绝而导致用户鉴别失败的次数
  UINT32 ulDeLinkAuthSum;//解链路验证的次数
  UINT32 ulStaLeaveDeLinkAuthSum;//因用户离开而导致解链路验证的次数
  UINT32 ulInabilityDeLinkAuthSum;//因AP能力不足而导致解链路验证的次数
  UINT32 ulStaAssocTimeSum;//所有曾经关联到此AP的用户的总累计时长，单位为分钟
  UINT32 ulRSSILowAssocFailSum;//因RSSI过低而关联失败的总次数(从AP启动开始统计)
  UINT32 ulConcurrentUsers;//在当前统计时长内有实际数据流量的用户个数
  UINT32 ulConcurrentUsersMAX;//在线时段内AP并发最大用户数
  UINT32 ulTimeoutUserLinkVeryFialedSum;//因超时而导致用户链路验证失败的次数
  UINT32 ulUserLinkVeryFailedSumOtherCause;//其它原因导致用户链路验证失败的次数
  UINT32 ulUserLinkReleaseFailedSum;//因超时而导致用户关联失败的次数
  UINT32 ulUserLinkReReleaseFailedSum;//因超时而导致用户重关联失败的次数
  UINT32 ulUserLinkOtherFailedSum;//其它原因导致用户重关联失败的次数
  UINT32 ulUserAuthTryTimesSum;//用户鉴别尝试次数
  UINT32 ulUserAuthTimeoutFailedSum;//因超时而导致用户鉴别失败的次数
  UINT32 ulUserAuthFailedOtherCauseSum;//其它原因导致用户鉴别失败的次数
  UINT32 ulDelinkVeryExceptionSum;//因异常情况而导致解链路验证的次数
  UINT32 ulExceptionDeAssocSum;//因异常情况而导致解关联的次数
  UINT32 ulOtherCauseDeAssocSum;//其它原因导致解关联的次数
  UINT32 ulInvalidAssocFailSum;//因无效而导致用户关联失败的次数
  UINT32 ulOtherAssocFialeSum;//其它原因导致用户关联失败的次数
  UINT32 ulInvalidReassocFailSum;//因无效而导致用户重关联失败的次数
  UINT32 ulLeaveDisassocSum;//因用户离开而导致解关联的次数
  UINT32 ulinabilityDisassocSum;//因AP能力不足而导致解关联的次数
  UINT32 ulInvalidUserLinkFailSum;//因无效而导致用户链路验证失败的次数
  UINT32 ulRejectUserLinkFailSum;//因拒绝而导致用户链路验证失败的次数
  UINT32 ulOtherDeauthSum;//其它原因导致解链路验证的次数
  UINT32 ulDeAssocSum;//解关联的次数（原名为 DisassocSum）
  UINT32 ulApStaAssocSum_ever;//所有曾经关联到此AP的用户总数
  UINT32 ulRejectassocSum;//因拒绝而导致用户关联失败的次数
  UINT32 ulReassocFailSum;//重关联失败总次数
  UINT32 ulAuthfailSum;//用户鉴别失败次数
  UINT32 ulUserLinkFailSum;//用户链路验证失败次数
}__attribute__ ((packed)) MT_IE37_IE88_T;
//IE37_IE78,79,80,81,82,83,84,88 end

//103  AP 地址信息(IPV4)
typedef struct _APIPADDR_INFO_IPV4_T_
{
	UINT8 aucGWAddr[MT_IPV4_ADDR_LEN];//AP 网关。
	UINT8 aucIpAddr[MT_IPV4_ADDR_LEN];//AP IP地址。
	UINT32 ulPreFix;//地址前缀。
}__attribute__ ((packed)) MT_IE103_APIPADDR_INFO_IPV4_T;

//104  AP 地址信息(IPV6)
typedef struct _APIPADDR_INFO_IPV6_T_
{
	UINT8 aucIpAddr[MT_IPV6_ADDR_LEN];//AP IP地址。
	UINT32 ulPreFix;//地址前缀。
}__attribute__ ((packed)) MT_IE104_APIPADDR_INFO_IPV6_T;

//107 APLastchgCONFIG Time
typedef struct _IE107_APLASTCHGCONFIGTIME_T_
{
	UINT32 ulAPLastChgConfigTime;
}__attribute__ ((packed)) MT_IE37_IE107_T;

//Channel Bind TLV（IE=132）
typedef struct _SCAN_CHANNEL_SET_T_
{
  UINT16 usChnnlId;//待扫描的信道编号
  UINT16 usFlag;//信道属性的标识位图，保留为将来使用。
}__attribute__ ((packed)) MT_IE132_SCAN_CHNNL_SET_T;

typedef struct _IE132_T_
{
  UINT8 ucRadioID;//Radio标识
  UINT8 aucFlag[3];//标识位图，保留为将来扩展使用。
  UINT16 usScanSrc;//扫描原因，即哪些事件触发的扫描，目前未定义，值始终为0。
  UINT16 usDeviceType;//扫描范围，目前未定义，值始终为0。
  UINT8 ucMaxCycles;//扫描循环次数。值255表示持续进行循环扫描。
  UINT16 usRecvd;//
  UINT8 ucChnnlCnt;//
  MT_IE132_SCAN_CHNNL_SET_T astScanChnnlSet[16];//参数携带了要扫描的信道列表
}__attribute__ ((packed)) MT_IE37_IE132_T;

//Channel Scan Report
typedef struct _CHNNL_SCAN_RPT_T_
{
  UINT16 usChannelNumber;//信道号
  UINT8 ucRadarStatistics;//该信道上是否检测到雷达，0x00 检测到雷达信号，0x01 未检测到雷达信号
  UINT16 usMeanTime;//信道测量时长，单位ms
  INT8 icMeanRSSI;//监听到的各个报文的平均信号强度，单位dbm
  UINT16 usScreenPacketCount;//总计在该信道上监听到的报文个数
  UINT8 ucNeighborCount;//该信道的邻居个数
  UINT8 ucMeanNoise;//该信道的平均噪声
  UINT8 ucInterference;//干扰情况，信道上所有邻居的Station interference + AP interference之和。
  UINT8 ucSelfTxOccp;//自身发送所占时长，自身报文空口占用时间/监测时间 * 255计算获得。
  UINT8 ucSelfStaOccp;//自身Station接收所占时长，连接本AP的Station，给本AP发送报文的空口占用时间/监测时间 * 255计算获得。
  UINT8 ucUnknownOccp;//未知的空口占用时长，未知空口报文的空口占用时间/监测时间 * 255计算获得。
  UINT8 ucCRCErrCnt;//错误报文个数/报文总个数 * 255 计算获得
  UINT8 ucDecryptErrCnt;//解密错误报文个数/报文总个数 * 255 计算获得
  UINT8 ucPhyErrCnt;//物理错误报文个数/报文总个数 * 255 计算获得
  UINT8 ucRetransCnt;//重传报文报文个数/报文总个数 * 255 计算获得
}__attribute__ ((packed)) MT_IE133_CHNNL_SCAN_RPT_T;

typedef struct _T133_T_
{
  UINT8 ucRadioID;
  UINT8 ucRptCnt;//Report Count：要报告的信道数目。
#define MT_CHANNEL_SCAN_REPORT_NUM 16
  MT_IE133_CHNNL_SCAN_RPT_T astChnnlScanRpt[MT_CHANNEL_SCAN_REPORT_NUM];
}__attribute__ ((packed)) MT_IE37_IE133_T;

//Neighbor Info
typedef struct _NeighborInfo_T_
{
  UINT8 aucBssID[MT_MAC_LENTH];//该邻居AP的BSSID
  UINT16 usChannelNumber;//检测到该邻居的信道号
  UINT8 uc11nChannelOffset;//该邻居（11n AP）使用的辅助信道偏移
  INT8 icMeanRssi;//检测到该邻居的平均信号强度，各个报文的平均信号强度
  UINT8 ucStaIntf;//邻居做为Station的占用权值，连接该AP的Station的空口占用时间/监测时间 * 255计算获得。
  UINT8 ucApIntf;//邻居做为AP的占用权值，该AP的空口占用时间/监测时间 * 255计算获得。
  UINT8 ucSSIDLen;
  UINT8 aucSsid[21];
}__attribute__ ((packed)) MT_IE134_NBR_INFO_T;

//Neighbor AP Report
typedef struct _T134_T_
{
  UINT8 ucRadioID;//Radio标识
  UINT8 ucResvd;//保留
  UINT16 usNumOfNghbrRprt;//邻居报告的数目
#define MT_NUMBER_NEIGHBOR_REPORT 128
  MT_IE134_NBR_INFO_T astNeighboeInfo[MT_NUMBER_NEIGHBOR_REPORT];
}__attribute__ ((packed)) MT_IE37_IE134_T;

//STA Entry
typedef struct _STA_ENTRY_T_
{
  UINT8 aucStaMac[MT_MAC_LENTH];//Sta MAC Address
  UINT8 aucBssid[MT_MAC_LENTH];//BSSID
  UINT16 usdataRate;//Date Rate
  UINT16 usChannelId;//Channel ID
  UINT32 ulLastTimeStamp;//Time Stamp of Last Seen
  INT8 icRssi;//RSSI
  UINT8 ucEncrypt;//Encrypt
}__attribute__ ((packed)) MT_IE138_STA_ENTRY_T;

//AP Entry
typedef struct _AP_ENTRY_T_
{
  UINT8 ucApEntryLen;//AP Entry Len:随后的内容长度。用于推算SSID的长度，即SSID长度 = AP Entry Len – 31字节。
#define MT_IE138_APENTRY_OFFSET 31
  UINT8 aucApSrcMac[MT_MAC_LENTH];//AP Src MAC Address
  UINT8 aucApBssidMac[MT_MAC_LENTH];//AP Bssid MAC Address
  INT8  icRssi;//RSSI
  UINT16 usDataRate;//Date Rate:被检测到的AP的发送速率。
  UINT16 usChannelId;//Channel ID:被检测到的AP所在信道。
  UINT32 ulLastTimeStampRecvdBeacon;//Time Stamp of Last Received Beacon
//  UINT16 usRecvdBeacon;//Received Beacon
  UINT32 ulLastSeenTimeStamp;//Time Stamp of Last Seen
  UINT16 usBeacomPeriod;//Beacon Period
#define MT_T138_SSID_LEN 21
  UINT8 aucSsid[MT_T138_SSID_LEN];//SSID...
  UINT8 ucType;//Type
//  Type:
//    0 1 2 3 4 5 6 7
//  +-+-+-+-+-+-+-+-+
//  |R|R|R|R|R|B|H|A|
//   +-+-+-+-+-+-+-+-+
//  B:表示该设备为Wlan Bridge。
//  H:表示该设备为Ad Hoc。
//  A:表示该设备为AP。
  UINT8 ucEncrypt;//Encrypt:是否加密。
  UINT8 ucSpoofType;//Spoof Type:被检测AP发起的攻击类型。值为17表示deauth攻击，18表示disassociation攻击
  INT8 icSnr;//信噪比，单位dB
}__attribute__ ((packed)) MT_IE138_AP_ENTRY_T;

//Rogue AP Report
typedef struct _T138_T_
{
  UINT8 ucRadioId;//Radio ID,1...31
  UINT8 ucResvd;//Reserved
  UINT16 usNumOfApEntry;//Num of AP Entries
#define MT_T138_AP_ENTRY_NUM 5
  MT_IE138_AP_ENTRY_T astApEntry[MT_T138_AP_ENTRY_NUM];
  UINT16 usNumOfStaEntry;//Num of Sta Entries
#define MT_T138_STA_ENTRY_NUM 2
  MT_IE138_STA_ENTRY_T astStaEntry[MT_T138_STA_ENTRY_NUM];
}__attribute__ ((packed)) MT_IE37_IE138_T;


extern MTBool gbAPOnlyFlag;

//CAPWAP Proc
extern MTBool ApMutiTestSendDiscoverReq(UINT32 vulApIndex);//DiscoverReq消息发送函数，AP->SUT(AC)
extern MTBool ApMutiTestSendJoinReq(UINT32 vulApIndex);
extern MTBool ApMutiTestSendConfigStatusReq(UINT32 vulApIndex);
extern MTBool ApMutiTestSendChangeStateReq(UINT32 vulApIndex);
extern MTBool ApMutiTestSendAddWlanRsp(UINT32 vulApIndex,UINT8 vucRadioId,UINT8 vucWlanId);
extern MTBool ApMutiTestSendConfigUpdateRsp(UINT32 vulApIndex);
extern MTBool ApMutiTestSendIamgeDataReq(UINT32 ulApIdx);
extern MTBool ApMutiTestSendIamgeDataRsp(UINT32 ulApIdx);
extern MTBool ApMutiTestSendStaConfigRsp(UINT16 vusStaConfigReqIeType,UINT32 vulApIdx,UINT32 vulStaIndex);
extern MTBool ApMutiTestSendWtpEventReq(UINT32 vulApIdx, UINT32 vulStaIndex);//WtpEventReq消息发送函数，AP->SUT（AC）
extern MTBool ApMutiTestSendResetRsp(UINT32 ulApIdx);//ResetRsp消息发送函数，AP->SUT(AC)
extern MTBool ApMutiTestSendKeepAlive(UINT32 vulApIndex);//KeepAlive,AP->AC
extern MTBool ApMutiTestSendEventReq_DelSta(UINT32 vulApIndex,UINT32 vulStaIndex);
extern MTBool ApMutiTestSendEchoReq(UINT32 vulApIndex);

extern MTBool ApMutiTestSendEventReq_IE37_IE56(UINT32 vulApIndex);
extern MTBool ApMutiTestSendEventReq_IE37_IE56_TYPE36(UINT32 vulApIndex);
extern MTBool ApMutiTestSendEventReq_IE37_IE57(UINT32 vulApIndex);
extern MTBool ApMutiTestSendEventReq_IE37_IE6X(UINT32 vulApIndex);
extern MTBool ApMutiTestSendEventReq_IE37_IE7X(UINT32 vulApIndex);
extern MTBool ApMutiTestSendEventReq_IE37_IE8X(UINT32 vulApIndex);
extern MTBool ApMutiTestSendEventReq_IE37_IE133_134_138(UINT32 vulApIndex);
extern MTBool ApMutiTestSendEventReq_IE37_IE5859(UINT32 vulApIndex);

//DHCPv4 Proc
extern MTBool ApMutiTestSendDhcpv4Discover(UINT32 vulApIndex);
extern MTBool ApMutiTestSendDhcpv4Req(UINT32 vulApIndex);
extern void MT_ApTmOvReset(UINT32 vulApIdx);
extern void MT_SndWtpEventMsg(UINT32 vulApIdx);
extern void MT_WtpEchoChk(void);
extern void MT_WtpEchoTimeChk(UINT32 vulApIdx);
extern void MT_WtpStatusChk(void);
extern void WtpRelease(UINT32 vulApIdx);
#endif /* AP_PROC_H_ */
