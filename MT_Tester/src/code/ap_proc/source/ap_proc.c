/******************************************************************************
* COPYRIGHT @ Reserved
*******************************************************************************
* File         : ap_proc.c
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

#include "ap_proc.h"
#include "mt_common.h"
UINT8 gaucApUpdateFilename[128];
UINT16 gusApFileNameLen = 0;

extern UINT32 gulApHoldTime;
extern MTBool gbTestMod;
extern UINT32 gulSysConfAPCnt;
extern UINT8 gucWtpEventFlag;
extern UINT32 gulEchoInterval;
extern MTBool gbStopFlag;  //停止测试标识
//extern MT_AP_RADIO_WLAN_INFO_T gastApWlanInfo[MT_AP_MAX_NUM];//AP WLAN INFO RECORD

#if 0
/* 6.1.1        发现请求Discovery Request */
UINT8  gaucDiscoverReq[]={
    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x00, 0xc4, 0x00,
    0x00, 0x14, 0x00, 0x01, 0x02, 0x00, 0x26, 0x00, 0x46, 0x00, 0x00, 0x5b, 0xa0, 0x00, 0x00, 0x00,
    0x12, 0x50, 0x6f, 0x73, 0x74, 0x63, 0x6f, 0x6d, 0x5f, 0x41, 0x50, 0x32, 0x30, 0x30, 0x30, 0x46,
    0x49, 0x54, 0x50, 0x00, 0x01, 0x00, 0x06, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x00, 0x02, 0x00,
    0x06, 0x49, 0x58, 0x50, 0x34, 0x32, 0x35, 0x00, 0x03, 0x00, 0x0a, 0x4e, 0x50, 0x45, 0x37, 0x38,
    0x32, 0x31, 0x30, 0x34, 0x36, 0x00, 0x04, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x27, 0x00, 0x5b, 0x01, 0x01, 0x01, 0x01, 0x00, 0x0c, 0x00, 0x00, 0x5b, 0xa0, 0x00, 0x00, 0x00,
    0x04, 0x31, 0x2e, 0x30, 0x33, 0x00, 0x00, 0x5b, 0xa0, 0x00, 0x01, 0x00, 0x08, 0x56, 0x31, 0x2e,
    0x34, 0x2e, 0x30, 0x2e, 0x30, 0x00, 0x00, 0x5b, 0xa0, 0x00, 0x02, 0x00, 0x0e, 0x72, 0x65, 0x64,
    0x62, 0x6f, 0x6f, 0x74, 0x2d, 0x30, 0x38, 0x30, 0x31, 0x32, 0x39, 0x00, 0x00, 0x5b, 0xa0, 0x00,
    0x03, 0x00, 0x1b, 0x32, 0x30, 0x30, 0x30, 0x46, 0x49, 0x54, 0x50, 0x56, 0x31, 0x2e, 0x34, 0x2e,
    0x30, 0x2e, 0x30, 0x2e, 0x31, 0x31, 0x31, 0x31, 0x31, 0x36, 0x2e, 0x69, 0x6d, 0x67, 0x00, 0x29,
    0x00, 0x01, 0x04, 0x00, 0x2c, 0x00, 0x01, 0x00, 0x04, 0x18, 0x00, 0x05, 0x01, 0x00, 0x00, 0x00,
    0x04
};

/* 6.1.3Join请求Join Request */
UINT8  gaucJoinReq[]={
    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x01, 0x57, 0x00,
    0x00, 0x14, 0x00, 0x01, 0x02, 0x00, 0x1c, 0x00, 0x22, 0xce, 0xf7, 0xb0, 0xb2, 0xca, 0xd0, 0xb8,
    0xdf, 0xd0, 0xc2, 0xc7, 0xf8, 0xbd, 0xf5, 0xd2, 0xb5, 0xc2, 0xb7, 0xd1, 0xd0, 0xb7, 0xa2, 0xb4,
    0xb4, 0xd2, 0xb5, 0xd4, 0xb0, 0x41, 0xd7, 0xf9, 0x38, 0xc2, 0xa5, 0x00, 0x26, 0x00, 0x46, 0x00,
    0x00, 0x5b, 0xa0, 0x00, 0x00, 0x00, 0x12, 0x50, 0x6f, 0x73, 0x74, 0x63, 0x6f, 0x6d, 0x5f, 0x41,
    0x50, 0x32, 0x30, 0x30, 0x30, 0x46, 0x49, 0x54, 0x50, 0x00, 0x01, 0x00, 0x06, 0x31, 0x32, 0x33,
    0x34, 0x35, 0x36, 0x00, 0x02, 0x00, 0x06, 0x49, 0x58, 0x50, 0x34, 0x32, 0x35, 0x00, 0x03, 0x00,
    0x0a, 0x4e, 0x50, 0x45, 0x37, 0x38, 0x32, 0x31, 0x30, 0x34, 0x36, 0x00, 0x04, 0x00, 0x06, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x27, 0x00, 0x5b, 0x01, 0x01, 0x01, 0x01, 0x00, 0x0c, 0x00,
    0x00, 0x5b, 0xa0, 0x00, 0x00, 0x00, 0x04, 0x31, 0x2e, 0x30, 0x33, 0x00, 0x00, 0x5b, 0xa0, 0x00,
    0x01, 0x00, 0x08, 0x56, 0x31, 0x2e, 0x34, 0x2e, 0x30, 0x2e, 0x30, 0x00, 0x00, 0x5b, 0xa0, 0x00,
    0x02, 0x00, 0x0e, 0x72, 0x65, 0x64, 0x62, 0x6f, 0x6f, 0x74, 0x2d, 0x30, 0x38, 0x30, 0x31, 0x32,
    0x39, 0x00, 0x00, 0x5b, 0xa0, 0x00, 0x03, 0x00, 0x1b, 0x32, 0x30, 0x30, 0x30, 0x46, 0x49, 0x54,
    0x50, 0x56, 0x31, 0x2e, 0x34, 0x2e, 0x30, 0x2e, 0x30, 0x2e, 0x31, 0x31, 0x31, 0x31, 0x31, 0x36,
    0x2e, 0x69, 0x6d, 0x67, 0x00, 0x2d, 0x00, 0x18, 0xd0, 0xc2, 0xd3, 0xca, 0xcd, 0xa8, 0xd0, 0xc5,
    0xca, 0xdd, 0x41, 0x50, 0xb9, 0xdc, 0xc0, 0xed, 0xbd, 0xd3, 0xbf, 0xda, 0xc8, 0xed, 0xbc, 0xfe,
    0x00, 0x23, 0x00, 0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03,
    0x00, 0x00, 0x00, 0x04, 0x00, 0x29, 0x00, 0x01, 0x04, 0x00, 0x2c, 0x00, 0x01, 0x00, 0x04, 0x18,
    0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x04, 0x00, 0x1e, 0x00, 0x04, 0x32, 0x00, 0x00, 0x02, 0x00,
    0x25, 0x00, 0x14, 0x00, 0x00, 0x5b, 0xa0, 0x00, 0x4d, 0x00, 0x04, 0x00, 0x00, 0x18, 0x17, 0x00,
    0x49, 0x00, 0x04, 0x00, 0x00, 0x00, 0x2b, 0x00, 0x33, 0x00, 0x01, 0x02, 0x00, 0x30, 0x00, 0x0f,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x35, 0x00, 0x01, 0x00
};

/* 6.1.5        状态配置请求Configuration Status Request */
UINT8  gaucConfigStatusReq[]={
    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x04, 0x01, 0x02, 0x00,
    0x00, 0x04, 0x00, 0x00, 0x00, 0x1f, 0x00, 0x02, 0x01, 0x00, 0x00, 0x24, 0x00, 0x02, 0x00, 0x0a,
    0x00, 0x30, 0x00, 0x0f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x25, 0x00, 0x58, 0x00, 0x00, 0x5b, 0xa0, 0x00, 0x55, 0x00, 0x50, 0x00,
    0x00, 0x00, 0x01, 0x77, 0x69, 0x66, 0x69, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x05, 0xdc, 0x00, 0x00, 0x00, 0x64, 0x00,
    0x22, 0x59, 0x00, 0x00, 0x26, 0x30, 0x30, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x04, 0x32,
    0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00,
    0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x04,
    0x01, 0x00, 0x06, 0x01, 0x01, 0x02, 0x02, 0x02, 0x02, 0x04, 0x04, 0x00, 0x08, 0x01, 0x00, 0x01,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x04, 0x06, 0x00, 0x10, 0x01, 0x00, 0x00, 0x00, 0x07, 0x04, 0x00,
    0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x02, 0x00, 0x04, 0x08, 0x00, 0x08, 0x01, 0x00, 0x00,
    0x01, 0x00, 0x20, 0x00, 0x00, 0x04, 0x09, 0x00, 0x08, 0x01, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x04, 0x10, 0x00, 0x01, 0x01, 0x04, 0x11, 0x00, 0x04, 0x01, 0x00, 0x00, 0x0a, 0x04, 0x12,
    0x00, 0x10, 0x01, 0x07, 0x00, 0x01, 0x00, 0x02, 0x00, 0x03, 0x00, 0x05, 0x00, 0x06, 0x00, 0x07,
    0x00, 0x0a, 0x04, 0x16, 0x00, 0x10, 0x01, 0x00, 0x08, 0x01, 0x00, 0x0b, 0x6b, 0xd9, 0xbc, 0x7d,
    0x01, 0xf4, 0x43, 0x4e, 0x4f, 0x00, 0x04, 0x18, 0x00, 0x05, 0x01, 0x00, 0x00, 0x00, 0x04
};

/* 6.1.11 状态变更事件请求Change State Event Request */
UINT8  gaucChangeStateReq[]={
    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0b, 0x05, 0x00, 0x12, 0x00,
    0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x06, 0x00, 0x20, 0x00, 0x03, 0x01, 0x01, 0x00
};

/* 6.1.8        配置更新响应Configuration Update Response */
UINT8  gaucConfigUpdateRsp[]={
    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0, 0x00, 0x08, 0x0f, 0x00, 0x12, 0x00,
    0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x2, 0x00, 0x03, 0x01, 0x01, 0x00
};

/* Add wlan响应消息Discovery Request:t1024 */
UINT8  gaucAddWlanRsp[]={
    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x33, 0xdd, 0x02, 0x10, 0x00, 0x17, 0x00,
    0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x04, 0x02, 0x00, 0x08, 0x01, 0x01, 0x00, 0x0b,
    0x6b, 0xd9, 0xa9, 0x69
};

/* 6.1.9        WTP Event Request */
//UINT8  gaucWtpEventReq[]={
//    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x06, 0x00, 0x93, 0x00,
//    0x00, 0x25, 0x00, 0x8c, 0x00, 0x00, 0x5b, 0xa0, 0x00, 0x39, 0x00, 0x84, 0x00, 0x00, 0x00, 0x1e,
//    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
//    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
//};

/* 6.1.20STA配置响应消息Station Configuration Response */
UINT8 gaucStaConfigRsp[]={
    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1a, 0x03, 0x00, 0x0f, 0x00,
    0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00
};

UINT8  gaucConfigUpdateRsp_RunStat[]={
    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x19, 0x00, 0x0f, 0x00,
    0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00
};

UINT8 gauCapwapImageReq[] = {
    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x34, 0x00, 0x55, 0x00,
    0x00, 0x19, 0x00, 0x46, 0x00, 0x00, 0x5b, 0xa0, 0x00, 0x00, 0x00, 0x12, 0x50, 0x6f, 0x73, 0x74,
    0x63, 0x6f, 0x6d, 0x5f, 0x41, 0x50, 0x32, 0x30, 0x30, 0x30, 0x46, 0x49, 0x54, 0x00, 0x00, 0x01,
    0x00, 0x09, 0x56, 0x31, 0x2e, 0x34, 0x2e, 0x30, 0x2e, 0x30, 0x00, 0x00, 0x02, 0x00, 0x1b, 0x32,
    0x30, 0x30, 0x30, 0x46, 0x49, 0x54, 0x56, 0x31, 0x2e, 0x34, 0x2e, 0x30, 0x2e, 0x30, 0x2e, 0x31,
    0x31, 0x31, 0x30, 0x31, 0x38, 0x2e, 0x69, 0x6d, 0x67, 0x00, 0x00, 0x1b, 0x00, 0x00
};

/*capwap升级响应消息：CW_MSG_TYPE_VALUE_IMAGE_DATA_RESPONSE  AP->AC*/
UINT8 gauCapwapImageRsp[] = {
    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x35, 0x00, 0x59, 0x00,
    0x00, 0x19, 0x00, 0x46, 0x00, 0x00, 0x5b, 0xa0, 0x00, 0x00, 0x00, 0x12, 0x50, 0x6f, 0x73, 0x74,
    0x63, 0x6f, 0x6d, 0x5f, 0x41, 0x50, 0x32, 0x30, 0x30, 0x30, 0x46, 0x49, 0x54, 0x00, 0x00, 0x01,
    0x00, 0x09, 0x56, 0x31, 0x2e, 0x34, 0x2e, 0x30, 0x2e, 0x30, 0x00, 0x00, 0x02, 0x00, 0x1b, 0x32,
    0x30, 0x30, 0x30, 0x46, 0x49, 0x54, 0x56, 0x31, 0x2e, 0x34, 0x2e, 0x30, 0x2e, 0x30, 0x2e, 0x31,
    0x31, 0x31, 0x30, 0x31, 0x38, 0x2e, 0x69, 0x6d, 0x67, 0x00, 0x00, 0x21, 0x00, 0x04, 0x00, 0x00,
    0x00, 0x00
};

/* */
UINT8  gaucWtpEventReqDelSta[] = {
    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x61, 0x00, 0x10, 0x00,
    0x00, 0x12, 0x00, 0x09, 0x01, 0x00, 0x06, 0x00, 0x1f, 0x3c, 0xdc, 0xe8, 0x62
};

/* 6.1.9        WTP Event Request */
UINT8 gaucWtpEventReq[] = {
    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x09, 0x07, 0x00, 0x93, 0x00,
    0x00, 0x25, 0x00, 0x8c, 0x00, 0x00, 0x5b, 0xa0, 0x00, 0x38, 0x00, 0x84, 0x00, 0x00, 0x00, 0x20,
    0x32, 0x30, 0x31, 0x31, 0x2d, 0x31, 0x32, 0x2d, 0x31, 0x32, 0x20, 0x31, 0x39, 0x3a, 0x32, 0x37,
    0x3a, 0x32, 0x37, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

/* 6.1.18复位响应消息Reset Response */
UINT8  gaucResetRsp[]={
    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x12, 0x03, 0x00, 0x0b, 0x00,
    0x00, 0x21, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00
};

//Keep_Alive
UINT8 gaucKeepAlive2Upm[] = {
    0x00, 0x10, 0x40, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x16, 0x00, 0x23, 0x00, 0x10, 0x00, 0x00,
    0x00, 0x01, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x04
};

UINT8 gaucEchoReq[] = {
    0x00, 0x10, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0d, 0x08, 0x00, 0x03, 0x00
};
#endif

MT_CAPWAPMSG_T gstDiscoveryReq;
MT_CAPWAPMSG_T gstJoinReq;
MT_CAPWAPMSG_T gstConfigurationStatusReq;
MT_CAPWAPMSG_T gstChangeStateReq;
MT_CAPWAPMSG_T gstConfigurationUpdateRsp;
MT_CAPWAPMSG_T gstAddWlanRsp;
MT_CAPWAPMSG_T gstStationConfigurationRsp;
MT_CAPWAPMSG_T gstConfigUpdateRsp_RunStat;
MT_CAPWAPMSG_T gstCapwapImageReq;
MT_CAPWAPMSG_T gstCapwapImageRsp;
MT_CAPWAPMSG_T gstWtpEventReq_DelSta;
MT_CAPWAPMSG_T gstWtpEventReq;
MT_CAPWAPMSG_T gstResetRsp;
MT_CAPWAPMSG_T gstKeepAlive;
MT_CAPWAPMSG_T gstEchoReq;


//CAPWAP Proc
//DiscoverReq消息发送函数，AP->SUT(AC)
MTBool ApMutiTestSendDiscoverReq(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;
  CAPWAP_MSG_T stDiscoverReq;
  UINT32 ulDiscoverReqLen = 0;
  MSG_ELEMNT_T stMsgElemntT20;
  UINT32 ulT20Len = 0;
  MSG_ELEMNT_T stMsgElemntT38;
  UINT32 ulT38Len = 0;
  MSG_ELEMNT_T stMsgElemntT39;
  UINT32 ulT39Len = 0;
  MSG_ELEMNT_T stMsgElemntT41;
  UINT32 ulT41Len = 0;
  MT_T41 stT41;
  MSG_ELEMNT_T stMsgElemntT44;
  UINT32 ulT44Len = 0;
  MT_T44 stT44;
  MSG_ELEMNT_T stMsgElemntT1048;
  UINT32 ulT1048Len = 0;
  MT_T1048 stT1048;


  MSG_ELEMNT_T stWTPModeNumber;  //型号
  UINT32 ulModLen = 0;
  MSG_ELEMNT_T stWTPSerialNumber;//序列号
  UINT32 ulSnLen = 0;
  MSG_ELEMNT_T stBoardID;        //板卡标识
  UINT32 ulBdIDLen = 0;
  MSG_ELEMNT_T stBoardRevision;  //板卡修订版本
  UINT32 ulBdRevLen = 0;
  MSG_ELEMNT_T stBaseMACAddress; //板卡的以太网卡MAC地址
  UINT32 ulBasMacAddLen = 0;

  MSG_ELEMNT_T stWTPHdwareVer;       //硬件版本；
  UINT32 ulHwVerLen = 0;
  MSG_ELEMNT_T stWTPActiveSftwareVer;//激活的软件版本；
  UINT32 ulActSwVerLen = 0;
  MSG_ELEMNT_T stWTPBootVer;         //启动版本；
  UINT32 ulBtVerLen = 0;
  MSG_ELEMNT_T stWTPOtherSftwareVer; //其他软件版本；
  UINT32 ulOthSwVerLen = 0;
  UINT16 usEncryptionCapabilities = 0;
  UINT16 usSuffix = 0;
  UINT16 usMsgElemntSuffix = 0;
  UINT32 ulNetCarrierInc = 0;

  if(MT_FALSE == gbTestMod)
     {
    cpss_mem_memset(&stDiscoverReq,0,sizeof(stDiscoverReq));
    cpss_mem_memset(&stMsgElemntT20,0,sizeof(stMsgElemntT20));
    cpss_mem_memset(&stMsgElemntT38,0,sizeof(stMsgElemntT38));
    cpss_mem_memset(&stMsgElemntT39,0,sizeof(stMsgElemntT39));
    cpss_mem_memset(&stMsgElemntT41,0,sizeof(stMsgElemntT41));
    cpss_mem_memset(&stMsgElemntT44,0,sizeof(stMsgElemntT44));
    cpss_mem_memset(&stMsgElemntT1048,0,sizeof(stMsgElemntT1048));
    cpss_mem_memset(&stT41,0,sizeof(stT41));
    cpss_mem_memset(&stT44,0,sizeof(stT44));
    cpss_mem_memset(&stT1048,0,sizeof(stT1048));


    cpss_mem_memset(&stWTPModeNumber,0,sizeof(stWTPModeNumber));  //型号
    cpss_mem_memset(&stWTPSerialNumber,0,sizeof(stWTPSerialNumber));//序列号
    cpss_mem_memset(&stBoardID,0,sizeof(stBoardID));        //板卡标识
    cpss_mem_memset(&stBoardRevision,0,sizeof(stBoardRevision));  //板卡修订版本
    cpss_mem_memset(&stBaseMACAddress,0,sizeof(stBaseMACAddress)); //板卡的以太网卡MAC地址

    cpss_mem_memset(&stWTPHdwareVer,0,sizeof(stWTPHdwareVer));       //硬件版本；
    cpss_mem_memset(&stWTPActiveSftwareVer,0,sizeof(stWTPActiveSftwareVer));//激活的软件版本；
    cpss_mem_memset(&stWTPBootVer,0,sizeof(stWTPBootVer));         //启动版本；
    cpss_mem_memset(&stWTPOtherSftwareVer,0,sizeof(stWTPOtherSftwareVer)); //其他软件版本；

    ulNetCarrierInc = htonl(gastApconf[vulApIndex].ulNetCarrierInc);

    //Discovery Type(20)
    stMsgElemntT20.usMsgElemntType = htons(20);
    stMsgElemntT20.usMsgElemntLen = 1;
    stMsgElemntT20.aucMsgElemntValue[0] = gastApconf[vulApIndex].enDiscovType;

    ulT20Len = stMsgElemntT20.usMsgElemntLen + sizeof(stMsgElemntT20.usMsgElemntType) + sizeof(stMsgElemntT20.usMsgElemntLen);
    stMsgElemntT20.usMsgElemntLen = htons(stMsgElemntT20.usMsgElemntLen);

    //WTP BOARD DATA(38)
    //38- 型号
    stWTPModeNumber.usMsgElemntType = htons(0);
    stWTPModeNumber.usMsgElemntLen = strlen((char*)gastApconf[vulApIndex].aucApmodule);
    cpss_mem_memcpy(&stWTPModeNumber.aucMsgElemntValue, &gastApconf[vulApIndex].aucApmodule,stWTPModeNumber.usMsgElemntLen);

    ulModLen = stWTPModeNumber.usMsgElemntLen + sizeof(stWTPModeNumber.usMsgElemntType) + sizeof(stWTPModeNumber.usMsgElemntLen);
    stWTPModeNumber.usMsgElemntLen = htons(stWTPModeNumber.usMsgElemntLen);

    //38-序列号
    stWTPSerialNumber.usMsgElemntType = htons(1);
    stWTPSerialNumber.usMsgElemntLen = strlen((char*)gastApconf[vulApIndex].acuSerialNo);
    cpss_mem_memcpy(&stWTPSerialNumber.aucMsgElemntValue,&gastApconf[vulApIndex].acuSerialNo,stWTPSerialNumber.usMsgElemntLen);

    ulSnLen = stWTPSerialNumber.usMsgElemntLen + sizeof(stWTPSerialNumber.usMsgElemntType) + sizeof(stWTPSerialNumber.usMsgElemntLen);
    stWTPSerialNumber.usMsgElemntLen = htons(stWTPSerialNumber.usMsgElemntLen);

    //38-板卡标识
    stBoardID.usMsgElemntType = htons(2);
//    stBoardID.usMsgElemntLen = strlen(gastApconf[vulApIndex].aucBoardID);
//    cpss_mem_memcpy(&stBoardID.aucMsgElemntValue,&gastApconf[vulApIndex].aucBoardID,stBoardID.usMsgElemntLen);
    stBoardID.usMsgElemntLen = 1;

    switch(gastApconf[vulApIndex].enDiscovType)
          {
    case 1:
    	stBoardID.aucMsgElemntValue[0] = 1;//1：静态分配；2：DHCP；3：PPPoE；4：DHCPv6；5：无状态方式（SLACC）
    	break;

    case 2:
    	stBoardID.aucMsgElemntValue[0] = 2;//1：静态分配；2：DHCP；3：PPPoE；4：DHCPv6；5：无状态方式（SLACC）
    	break;

    case 3:
    	stBoardID.aucMsgElemntValue[0] = 3;//1：静态分配；2：DHCP；3：PPPoE；4：DHCPv6；5：无状态方式（SLACC）
    	break;

    case 4:
    	stBoardID.aucMsgElemntValue[0] = 4;//1：静态分配；2：DHCP；3：PPPoE；4：DHCPv6；5：无状态方式（SLACC）
    	break;

    case 5:
    	stBoardID.aucMsgElemntValue[0] = 5;//1：静态分配；2：DHCP；3：PPPoE；4：DHCPv6；5：无状态方式（SLACC）
    	break;

    default:
    	break;
          }

    ulBdIDLen = stBoardID.usMsgElemntLen + sizeof(stBoardID.usMsgElemntType) + sizeof(stBoardID.usMsgElemntLen);
    stBoardID.usMsgElemntLen = htons(stBoardID.usMsgElemntLen);

    //38-板卡的修订版本
    stBoardRevision.usMsgElemntType = htons(3);
    stBoardRevision.usMsgElemntLen = strlen((char*)gastApconf[vulApIndex].aucBoardRevision);
    cpss_mem_memcpy(&stBoardRevision.aucMsgElemntValue,&gastApconf[vulApIndex].aucBoardRevision,stBoardRevision.usMsgElemntLen);

    ulBdRevLen = stBoardRevision.usMsgElemntLen + sizeof(stBoardRevision.usMsgElemntType) + sizeof(stBoardRevision.usMsgElemntLen);
    stBoardRevision.usMsgElemntLen = htons(stBoardRevision.usMsgElemntLen);

    //38-板卡的以太网卡MAC地址
    stBaseMACAddress.usMsgElemntType = htons(4);
    stBaseMACAddress.usMsgElemntLen = MT_MAC_LENTH;
    GetMacFromStr(gastApconf[vulApIndex].aucApMacStr,stBaseMACAddress.aucMsgElemntValue);

    ulBasMacAddLen = stBaseMACAddress.usMsgElemntLen + sizeof(stBaseMACAddress.usMsgElemntType) + sizeof(stBaseMACAddress.usMsgElemntLen);
    stBaseMACAddress.usMsgElemntLen = htons(stBaseMACAddress.usMsgElemntLen);

  //construct IE38
    usSuffix = 0;
    stMsgElemntT38.usMsgElemntType = htons(38);

    cpss_mem_memcpy(&stMsgElemntT38.aucMsgElemntValue[usSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
    usSuffix = usSuffix + sizeof(ulNetCarrierInc);
    cpss_mem_memcpy(&stMsgElemntT38.aucMsgElemntValue[usSuffix],&stWTPModeNumber,ulModLen);
    usSuffix = usSuffix + ulModLen;
    cpss_mem_memcpy(&stMsgElemntT38.aucMsgElemntValue[usSuffix],&stWTPSerialNumber,ulSnLen);
    usSuffix = usSuffix + ulSnLen;
    cpss_mem_memcpy(&stMsgElemntT38.aucMsgElemntValue[usSuffix],&stBoardID,ulBdIDLen);
    usSuffix = usSuffix + ulBdIDLen;
    cpss_mem_memcpy(&stMsgElemntT38.aucMsgElemntValue[usSuffix],&stBoardRevision,ulBdRevLen);
    usSuffix = usSuffix + ulBdRevLen;
    cpss_mem_memcpy(&stMsgElemntT38.aucMsgElemntValue[usSuffix],&stBaseMACAddress,ulBasMacAddLen);
    usSuffix = usSuffix + ulBasMacAddLen;
    stMsgElemntT38.usMsgElemntLen = usSuffix;

    ulT38Len = stMsgElemntT38.usMsgElemntLen + sizeof(stMsgElemntT38.usMsgElemntType) + sizeof(stMsgElemntT38.usMsgElemntLen);
    stMsgElemntT38.usMsgElemntLen = htons(stMsgElemntT38.usMsgElemntLen);

  //WTP Discriptor(39)
    //39-WTPHdwareVer硬件版本；
    stWTPHdwareVer.usMsgElemntType = htons(0);
    stWTPHdwareVer.usMsgElemntLen = strlen((char*)gastApconf[vulApIndex].aucHdWareVer);
    cpss_mem_memcpy(&stWTPHdwareVer.aucMsgElemntValue[0],&gastApconf[vulApIndex].aucHdWareVer,stWTPHdwareVer.usMsgElemntLen);

    ulHwVerLen = sizeof(stWTPHdwareVer.usMsgElemntType) + sizeof(stWTPHdwareVer.usMsgElemntLen)+ stWTPHdwareVer.usMsgElemntLen;
    stWTPHdwareVer.usMsgElemntLen = htons(stWTPHdwareVer.usMsgElemntLen);

    //39-WTPActiveSftwareVer激活的软件版本；
    stWTPActiveSftwareVer.usMsgElemntType = htons(1);
    stWTPActiveSftwareVer.usMsgElemntLen = strlen((char*)gastApconf[vulApIndex].aucActiveSftwareVer);
    cpss_mem_memcpy(&stWTPActiveSftwareVer.aucMsgElemntValue[0],&gastApconf[vulApIndex].aucActiveSftwareVer,stWTPActiveSftwareVer.usMsgElemntLen);

    ulActSwVerLen = sizeof(stWTPActiveSftwareVer.usMsgElemntType) + sizeof(stWTPActiveSftwareVer.usMsgElemntLen) + stWTPActiveSftwareVer.usMsgElemntLen;
    stWTPActiveSftwareVer.usMsgElemntLen = htons(stWTPActiveSftwareVer.usMsgElemntLen);

    //39-WTPBootVer启动版本；
    stWTPBootVer.usMsgElemntType = htons(2);
    stWTPBootVer.usMsgElemntLen = strlen("redboot-080129");
    cpss_mem_memcpy(&stWTPBootVer.aucMsgElemntValue[0],"redboot-080129",stWTPBootVer.usMsgElemntLen);

    ulBtVerLen = sizeof(stWTPBootVer.usMsgElemntType) + sizeof(stWTPBootVer.usMsgElemntLen) + stWTPBootVer.usMsgElemntLen;
    stWTPBootVer.usMsgElemntLen = htons(stWTPBootVer.usMsgElemntLen);

    //39-WTPOtherSftwareVer其他软件版本；
    stWTPOtherSftwareVer.usMsgElemntType = htons(3);
    stWTPOtherSftwareVer.usMsgElemntLen = strlen((char*)gastApconf[vulApIndex].aucApImgFileName);
    cpss_mem_memcpy(&stWTPOtherSftwareVer.aucMsgElemntValue[0],&gastApconf[vulApIndex].aucApImgFileName,stWTPOtherSftwareVer.usMsgElemntLen);
    ulOthSwVerLen = sizeof(stWTPOtherSftwareVer.usMsgElemntType) + sizeof(stWTPOtherSftwareVer.usMsgElemntLen) + stWTPOtherSftwareVer.usMsgElemntLen;
    stWTPOtherSftwareVer.usMsgElemntLen = htons(stWTPOtherSftwareVer.usMsgElemntLen);

    //construct IE39
    stMsgElemntT39.usMsgElemntType = htons(39);

    stMsgElemntT39.aucMsgElemntValue[0] = 1; //MAX Radios
    stMsgElemntT39.aucMsgElemntValue[1] = 1; //Radio in use
    stMsgElemntT39.aucMsgElemntValue[2] = 1;//ucEncryptCapabilityNum = 1;
    stMsgElemntT39.aucMsgElemntValue[3] = 1;//ucEncryptWBID = 1;//802.11
    usEncryptionCapabilities = 12;
    usEncryptionCapabilities = htons(usEncryptionCapabilities);
    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[4],&usEncryptionCapabilities,sizeof(usEncryptionCapabilities));

    usSuffix = 6;
    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
    usSuffix = usSuffix  + sizeof(ulNetCarrierInc);
    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&stWTPHdwareVer,ulHwVerLen);
    usSuffix = usSuffix  + ulHwVerLen;

    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
    usSuffix = usSuffix  + sizeof(ulNetCarrierInc);
    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&stWTPActiveSftwareVer,ulActSwVerLen);
    usSuffix = usSuffix  + ulActSwVerLen;

    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
    usSuffix = usSuffix  + sizeof(ulNetCarrierInc);
    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&stWTPBootVer,ulBtVerLen);
    usSuffix = usSuffix  + ulBtVerLen;

    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
    usSuffix = usSuffix  + sizeof(ulNetCarrierInc);
    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&stWTPOtherSftwareVer,ulOthSwVerLen);
    usSuffix = usSuffix  + ulOthSwVerLen;

    stMsgElemntT39.usMsgElemntLen = usSuffix;
    ulT39Len = stMsgElemntT39.usMsgElemntLen + sizeof(stMsgElemntT39.usMsgElemntType) + sizeof(stMsgElemntT39.usMsgElemntLen);
    stMsgElemntT39.usMsgElemntLen = htons(stMsgElemntT39.usMsgElemntLen);

  //WTP Frame Tunnel Mode(41)
    stT41.ucWtpFrmTunnlMod = gastApconf[vulApIndex].ucWtpFrmTunnlMod;

    usSuffix = 0;
    stMsgElemntT41.usMsgElemntType = htons(41);
    stMsgElemntT41.usMsgElemntLen = 1;
    cpss_mem_memcpy(&stMsgElemntT41.aucMsgElemntValue[usSuffix],&stT41,stMsgElemntT41.usMsgElemntLen);
    ulT41Len = stMsgElemntT41.usMsgElemntLen + sizeof(stMsgElemntT41.usMsgElemntType) + sizeof(stMsgElemntT41.usMsgElemntLen);
    stMsgElemntT41.usMsgElemntLen = htons(stMsgElemntT41.usMsgElemntLen);

  //WTP MAC Type(44)
    stT44.ucMACType = gastApconf[vulApIndex].ucWtpMacType;

    usSuffix = 0;
    stMsgElemntT44.usMsgElemntType = htons(44);
    stMsgElemntT44.usMsgElemntLen = 1;
    cpss_mem_memcpy(&stMsgElemntT44.aucMsgElemntValue[usSuffix],&stT44,stMsgElemntT44.usMsgElemntLen);
    ulT44Len = stMsgElemntT44.usMsgElemntLen + sizeof(stMsgElemntT44.usMsgElemntType) + sizeof(stMsgElemntT44.usMsgElemntLen);
    stMsgElemntT44.usMsgElemntLen = htons(stMsgElemntT44.usMsgElemntLen);

  //IEEE 802.11 WTP Radio Information(1048)
    stT1048.ucRadioID = 1;
    stT1048.ulRadioType = htonl(gastApconf[vulApIndex].ucRadioType);

    usSuffix = 0;
    stMsgElemntT1048.usMsgElemntType = htons(1048);
    stMsgElemntT1048.usMsgElemntLen = 5;
    cpss_mem_memcpy(&stMsgElemntT1048.aucMsgElemntValue[usSuffix],&stT1048,stMsgElemntT1048.usMsgElemntLen);
    ulT1048Len = stMsgElemntT1048.usMsgElemntLen + sizeof(stMsgElemntT1048.usMsgElemntType) + sizeof(stMsgElemntT1048.usMsgElemntLen);
    stMsgElemntT1048.usMsgElemntLen = htons(stMsgElemntT1048.usMsgElemntLen);

    //construct Discovery MSG
    usMsgElemntSuffix = 0;
    cpss_mem_memcpy(&stDiscoverReq.aucMsgElemntBuf[usMsgElemntSuffix],&stMsgElemntT20,ulT20Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulT20Len;
    cpss_mem_memcpy(&stDiscoverReq.aucMsgElemntBuf[usMsgElemntSuffix],&stMsgElemntT38,ulT38Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulT38Len;
    cpss_mem_memcpy(&stDiscoverReq.aucMsgElemntBuf[usMsgElemntSuffix],&stMsgElemntT39,ulT39Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulT39Len;
    cpss_mem_memcpy(&stDiscoverReq.aucMsgElemntBuf[usMsgElemntSuffix],&stMsgElemntT41,ulT41Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulT41Len;
    cpss_mem_memcpy(&stDiscoverReq.aucMsgElemntBuf[usMsgElemntSuffix],&stMsgElemntT44,ulT44Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulT44Len;
    cpss_mem_memcpy(&stDiscoverReq.aucMsgElemntBuf[usMsgElemntSuffix],&stMsgElemntT1048,ulT1048Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulT1048Len;

    stDiscoverReq.ucPreamble = (UINT8)0;
    stDiscoverReq.ucHdLen = 16;
    stDiscoverReq.aucHdFlag[0] = 3;
    stDiscoverReq.aucHdFlag[1] = 0;
    stDiscoverReq.usFragID = 0;
    stDiscoverReq.usFragOffset = 0;
    stDiscoverReq.aucMsgTypeEnterPrsNum[0] = (UINT8)0;
    stDiscoverReq.aucMsgTypeEnterPrsNum[1] = (UINT8)0;
    stDiscoverReq.aucMsgTypeEnterPrsNum[2] = (UINT8)0;
    stDiscoverReq.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_DISCOVERY_REQUEST;
    stDiscoverReq.ucSeqNum = (UINT8)0;
    stDiscoverReq.usMsgElemnetLen = sizeof(stDiscoverReq.usMsgElemnetLen) + sizeof(stDiscoverReq.ucFlag) + usMsgElemntSuffix;
    stDiscoverReq.ucFlag = (UINT8)0;
    ulDiscoverReqLen = 13 + stDiscoverReq.usMsgElemnetLen;
    stDiscoverReq.usMsgElemnetLen = htons(stDiscoverReq.usMsgElemnetLen);

    gstDiscoveryReq.ulBufLen = ulDiscoverReqLen;
    cpss_mem_memcpy(&gstDiscoveryReq.aucBuff,(UINT8*)&stDiscoverReq,ulDiscoverReqLen);
      }
  else
    {
   cpss_mem_memcpy(&(gstDiscoveryReq.aucBuff[89]), &(gastAp[vulApIndex].auApMac),MT_MAC_LENTH);
     }

  bSendResult = MT_SendApMsg2AC(vulApIndex,glCpmSockId,gstDiscoveryReq.aucBuff, gstDiscoveryReq.ulBufLen);

  return bSendResult;
}

/*Join Req消息AP->AC*/
MTBool ApMutiTestSendJoinReq(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;

  CAPWAP_MSG_T stJoinReq;
  UINT32 ulJoinReqLen = 0;
  MSG_ELEMNT_T stMsgElemntT20;//T20,DicoveryType
  UINT32 ulT20Len = 0;
  MSG_ELEMNT_T stMsgElemntT28;//T28,Location Data
  UINT32 ulT28Len = 0;
  char *pucApLocation = "Floor_803";
  MSG_ELEMNT_T stMsgElemntT38;//T38,WTP Board Data
  UINT32 ulT38Len = 0;
  UINT16 usEncryptionCapabilities = 0;
  MSG_ELEMNT_T stMsgElemntT39;//T39,WTP Descriptor
  UINT32 ulT39Len = 0;
  MSG_ELEMNT_T stMsgElemntT45;//T45,WTP Name
  UINT32 ulT45Len = 0;
  UINT8 aucWtpName[32] = {0};
  UINT8 aucIntChar[8] = {0};
  MSG_ELEMNT_T stMsgElemntT35;//T35,session ID
  UINT32 ulT35Len = 0;
  UINT32 ulSessionId0 = 0;
  UINT32 ulSessionId1 = 0;
  UINT32 ulSessionId2 = 0;
  UINT32 ulSessionId3 = 0;
  MSG_ELEMNT_T stMsgElemntT41;//T41,WTP Frame Tunnel Mode
  UINT32 ulT41Len = 0;
  MSG_ELEMNT_T stMsgElemntT44;//T44,WTP MAC Type
  UINT32 ulT44Len = 0;
  MSG_ELEMNT_T stMsgElemntT1048;//T1048,IEEE 802.11 QTP Radio Information
  UINT32 ulT1048Len = 0;
  MSG_ELEMNT_T stMsgElemntT30;//T30,CAPWAP Local IPv4 Address
  UINT32 ulT30Len = 0;
  MSG_ELEMNT_T stMsgElemntT37;//T37,Vendor Specific Payload
  UINT32 ulT37Len = 0;
  MSG_ELEMNT_T stMsgElemntT51;//T51,CAPWAP Transport Protocol
  UINT32 ulT51Len = 0;
  MSG_ELEMNT_T stMsgElemntT48;//T48,WTP Reboot Statistics
  UINT32 ulT48Len = 0;
  MT_T48 stT48;
  MSG_ELEMNT_T stMsgElemntT53;//T53,ECN Support
  UINT32 ulT53Len = 0;
  MSG_ELEMNT_T stMsgElemntT77;//AP_LAST_UP_GRADE_TIME；
  UINT32 ulT77Len = 0;
  MSG_ELEMNT_T stMsgElemntT73;//AP_SYSUPTIME；
  UINT32 ulT73Len = 0;
  MSG_ELEMNT_T stMsgElemntT103;//AP IPv4 Addr；
  UINT32 ulT103Len = 0;
  MSG_ELEMNT_T stMsgElemntT104;//AP IPv6 Addr；
  UINT32 ulT104Len = 0;
  MT_IE103_APIPADDR_INFO_IPV4_T stApIpv4Addr;
  MT_IE104_APIPADDR_INFO_IPV6_T stApIpv6Addr;

  MSG_ELEMNT_T stWTPModeNumber;  //型号
  UINT32 ulModLen = 0;
  MSG_ELEMNT_T stWTPSerialNumber;//序列号
  UINT32 ulSnLen = 0;
  MSG_ELEMNT_T stBoardID;        //板卡标识
  UINT32 ulBdIDLen = 0;
  MSG_ELEMNT_T stBoardRevision;  //板卡修订版本
  UINT32 ulBdRevLen = 0;
  MSG_ELEMNT_T stBaseMACAddress; //板卡的以太网卡MAC地址
  UINT32 ulBasMacAddLen = 0;

  MSG_ELEMNT_T stWTPHdwareVer;       //硬件版本；
  UINT32 ulHwVerLen = 0;
  MSG_ELEMNT_T stWTPActiveSftwareVer;//激活的软件版本；
  UINT32 ulActSwVerLen = 0;
  MSG_ELEMNT_T stWTPBootVer;         //启动版本；
  UINT32 ulBtVerLen = 0;
  MSG_ELEMNT_T stWTPOtherSftwareVer; //其他软件版本；
  UINT32 ulOthSwVerLen = 0;

//  UINT8 aucIpAddr[MT_IPV4_ADDR_LEN] = {0};
  UINT16 usSuffix = 0;
//  UINT16 usMsgElemntSuffix = 0;
  UINT32 ulNetCarrierInc = 0;
  UINT32 ulApUpgradeTime = 0;
  UINT32 ulApSysUpTime = 0;

  if(MT_FALSE == gbTestMod)
     {
    cpss_mem_memset(&stJoinReq,0,sizeof(stJoinReq));
    cpss_mem_memset(&stMsgElemntT20,0,sizeof(stMsgElemntT20));
    cpss_mem_memset(&stMsgElemntT28,0,sizeof(stMsgElemntT28));
    cpss_mem_memset(&stMsgElemntT38,0,sizeof(stMsgElemntT38));
    cpss_mem_memset(&stMsgElemntT39,0,sizeof(stMsgElemntT39));
    cpss_mem_memset(&stMsgElemntT45,0,sizeof(stMsgElemntT45));
    cpss_mem_memset(&stMsgElemntT35,0,sizeof(stMsgElemntT35));
    cpss_mem_memset(&stMsgElemntT41,0,sizeof(stMsgElemntT41));
    cpss_mem_memset(&stMsgElemntT44,0,sizeof(stMsgElemntT44));
    cpss_mem_memset(&stMsgElemntT1048,0,sizeof(stMsgElemntT1048));
    cpss_mem_memset(&stMsgElemntT30,0,sizeof(stMsgElemntT30));
    cpss_mem_memset(&stMsgElemntT37,0,sizeof(stMsgElemntT37));
    cpss_mem_memset(&stMsgElemntT51,0,sizeof(stMsgElemntT51));
    cpss_mem_memset(&stMsgElemntT48,0,sizeof(stMsgElemntT48));
    cpss_mem_memset(&stMsgElemntT53,0,sizeof(stMsgElemntT53));
    cpss_mem_memset(&stMsgElemntT103,0,sizeof(stMsgElemntT103));
    cpss_mem_memset(&stMsgElemntT104,0,sizeof(stMsgElemntT104));

    cpss_mem_memset(&stT48,0,sizeof(stT48));

    cpss_mem_memset(&stApIpv4Addr,0,sizeof(stApIpv4Addr));
    cpss_mem_memset(&stApIpv6Addr,0,sizeof(stApIpv6Addr));

    cpss_mem_memset(&stWTPModeNumber,0,sizeof(stWTPModeNumber));
    cpss_mem_memset(&stWTPSerialNumber,0,sizeof(stWTPSerialNumber));
    cpss_mem_memset(&stBoardID,0,sizeof(stBoardID));
    cpss_mem_memset(&stBoardRevision,0,sizeof(stBoardRevision));
    cpss_mem_memset(&stBaseMACAddress,0,sizeof(stBaseMACAddress));
    cpss_mem_memset(&stWTPHdwareVer,0,sizeof(stWTPHdwareVer));
    cpss_mem_memset(&stWTPActiveSftwareVer,0,sizeof(stWTPActiveSftwareVer));
    cpss_mem_memset(&stWTPBootVer,0,sizeof(stWTPBootVer));
    cpss_mem_memset(&stWTPOtherSftwareVer,0,sizeof(stWTPOtherSftwareVer));

    ulNetCarrierInc = htonl(gastApconf[vulApIndex].ulNetCarrierInc);

  //  T20,DicoveryType
    stMsgElemntT20.usMsgElemntType = htons(20);
    stMsgElemntT20.usMsgElemntLen = 1;
    stMsgElemntT20.aucMsgElemntValue[0] = gastApconf[vulApIndex].enDiscovType;

    ulT20Len = stMsgElemntT20.usMsgElemntLen + sizeof(stMsgElemntT20.usMsgElemntType) + sizeof(stMsgElemntT20.usMsgElemntLen);
    stMsgElemntT20.usMsgElemntLen = htons(stMsgElemntT20.usMsgElemntLen);

    //  T28,Location Data
    stMsgElemntT28.usMsgElemntType = htons(28);
    stMsgElemntT28.usMsgElemntLen = strlen((char*)pucApLocation);
    stMsgElemntT28.aucMsgElemntValue[0] = gastApconf[vulApIndex].enDiscovType;
    cpss_mem_memcpy(&stMsgElemntT28.aucMsgElemntValue[0],pucApLocation,stMsgElemntT28.usMsgElemntLen);

    ulT28Len = stMsgElemntT28.usMsgElemntLen + sizeof(stMsgElemntT28.usMsgElemntType) + sizeof(stMsgElemntT28.usMsgElemntLen);
    stMsgElemntT28.usMsgElemntLen = htons(stMsgElemntT28.usMsgElemntLen);

    //T38,WTP Board Data
         //38-型号
    stWTPModeNumber.usMsgElemntType = htons(0);
    stWTPModeNumber.usMsgElemntLen = strlen((char*)gastApconf[vulApIndex].aucApmodule);
    cpss_mem_memcpy(&stWTPModeNumber.aucMsgElemntValue, &gastApconf[vulApIndex].aucApmodule,stWTPModeNumber.usMsgElemntLen);

    ulModLen = stWTPModeNumber.usMsgElemntLen + sizeof(stWTPModeNumber.usMsgElemntType) + sizeof(stWTPModeNumber.usMsgElemntLen);
    stWTPModeNumber.usMsgElemntLen = htons(stWTPModeNumber.usMsgElemntLen);

         //38-序列号
    stWTPSerialNumber.usMsgElemntType = htons(1);
    stWTPSerialNumber.usMsgElemntLen = strlen((char*)gastApconf[vulApIndex].acuSerialNo);
    cpss_mem_memcpy(&stWTPSerialNumber.aucMsgElemntValue,&gastApconf[vulApIndex].acuSerialNo,stWTPSerialNumber.usMsgElemntLen);

    ulSnLen = stWTPSerialNumber.usMsgElemntLen + sizeof(stWTPSerialNumber.usMsgElemntType) + sizeof(stWTPSerialNumber.usMsgElemntLen);
    stWTPSerialNumber.usMsgElemntLen = htons(stWTPSerialNumber.usMsgElemntLen);

        //38-板卡标识
    stBoardID.usMsgElemntType = htons(2);
//    stBoardID.usMsgElemntLen = strlen(gastApconf[vulApIndex].aucBoardID);
//    cpss_mem_memcpy(&stBoardID.aucMsgElemntValue,&gastApconf[vulApIndex].aucBoardID,stBoardID.usMsgElemntLen);

    stBoardID.usMsgElemntLen = 1;

    switch(gastApconf[vulApIndex].enDiscovType)
          {
    case 1:
    	stBoardID.aucMsgElemntValue[0] = 1;//1：静态分配；2：DHCP；3：PPPoE；4：DHCPv6；5：无状态方式（SLACC）
    	break;

    case 2:
    	stBoardID.aucMsgElemntValue[0] = 2;//1：静态分配；2：DHCP；3：PPPoE；4：DHCPv6；5：无状态方式（SLACC）
    	break;

    case 3:
    	stBoardID.aucMsgElemntValue[0] = 3;//1：静态分配；2：DHCP；3：PPPoE；4：DHCPv6；5：无状态方式（SLACC）
    	break;

    case 4:
    	stBoardID.aucMsgElemntValue[0] = 4;//1：静态分配；2：DHCP；3：PPPoE；4：DHCPv6；5：无状态方式（SLACC）
    	break;

    case 5:
    	stBoardID.aucMsgElemntValue[0] = 5;//1：静态分配；2：DHCP；3：PPPoE；4：DHCPv6；5：无状态方式（SLACC）
    	break;

    default:
    	break;
          }

    ulBdIDLen = stBoardID.usMsgElemntLen + sizeof(stBoardID.usMsgElemntType) + sizeof(stBoardID.usMsgElemntLen);
    stBoardID.usMsgElemntLen = htons(stBoardID.usMsgElemntLen);

        //38-板卡的修订版本
    stBoardRevision.usMsgElemntType = htons(3);
    stBoardRevision.usMsgElemntLen = strlen((char*)gastApconf[vulApIndex].aucBoardRevision);
    cpss_mem_memcpy(&stBoardRevision.aucMsgElemntValue,&gastApconf[vulApIndex].aucBoardRevision,stBoardRevision.usMsgElemntLen);

    ulBdRevLen = stBoardRevision.usMsgElemntLen + sizeof(stBoardRevision.usMsgElemntType) + sizeof(stBoardRevision.usMsgElemntLen);
    stBoardRevision.usMsgElemntLen = htons(stBoardRevision.usMsgElemntLen);

      //38-板卡的以太网卡MAC地址
    stBaseMACAddress.usMsgElemntType = htons(4);
    stBaseMACAddress.usMsgElemntLen = MT_MAC_LENTH;
    GetMacFromStr(gastApconf[vulApIndex].aucApMacStr,stBaseMACAddress.aucMsgElemntValue);

    ulBasMacAddLen = stBaseMACAddress.usMsgElemntLen + sizeof(stBaseMACAddress.usMsgElemntType) + sizeof(stBaseMACAddress.usMsgElemntLen);
    stBaseMACAddress.usMsgElemntLen = htons(stBaseMACAddress.usMsgElemntLen);

    //Construct IE38
    usSuffix = 0;
    stMsgElemntT38.usMsgElemntType = htons(38);

    cpss_mem_memcpy(&stMsgElemntT38.aucMsgElemntValue[usSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
    usSuffix = usSuffix + sizeof(ulNetCarrierInc);
    cpss_mem_memcpy(&stMsgElemntT38.aucMsgElemntValue[usSuffix],&stWTPModeNumber,ulModLen);
    usSuffix = usSuffix + ulModLen;
    cpss_mem_memcpy(&stMsgElemntT38.aucMsgElemntValue[usSuffix],&stWTPSerialNumber,ulSnLen);
    usSuffix = usSuffix + ulSnLen;
    cpss_mem_memcpy(&stMsgElemntT38.aucMsgElemntValue[usSuffix],&stBoardID,ulBdIDLen);
    usSuffix = usSuffix + ulBdIDLen;
    cpss_mem_memcpy(&stMsgElemntT38.aucMsgElemntValue[usSuffix],&stBoardRevision,ulBdRevLen);
    usSuffix = usSuffix + ulBdRevLen;
    cpss_mem_memcpy(&stMsgElemntT38.aucMsgElemntValue[usSuffix],&stBaseMACAddress,ulBasMacAddLen);
    usSuffix = usSuffix + ulBasMacAddLen;

    stMsgElemntT38.usMsgElemntLen = usSuffix;

    ulT38Len = stMsgElemntT38.usMsgElemntLen + sizeof(stMsgElemntT38.usMsgElemntType) + sizeof(stMsgElemntT38.usMsgElemntLen);
    stMsgElemntT38.usMsgElemntLen = htons(stMsgElemntT38.usMsgElemntLen);

    //T39,WTP Descriptor
    //39-WTPHdwareVer硬件版本；
    stWTPHdwareVer.usMsgElemntType = htons(0);
    stWTPHdwareVer.usMsgElemntLen = strlen((char*)gastApconf[vulApIndex].aucHdWareVer);
    cpss_mem_memcpy(&stWTPHdwareVer.aucMsgElemntValue[0],&gastApconf[vulApIndex].aucHdWareVer,stWTPHdwareVer.usMsgElemntLen);

    ulHwVerLen = sizeof(stWTPHdwareVer.usMsgElemntType) + sizeof(stWTPHdwareVer.usMsgElemntLen)+ stWTPHdwareVer.usMsgElemntLen;
    stWTPHdwareVer.usMsgElemntLen = htons(stWTPHdwareVer.usMsgElemntLen);

    //39-WTPActiveSftwareVer激活的软件版本；
    stWTPActiveSftwareVer.usMsgElemntType = htons(1);
    stWTPActiveSftwareVer.usMsgElemntLen = strlen((char*)gastApconf[vulApIndex].aucActiveSftwareVer);
    cpss_mem_memcpy(&stWTPActiveSftwareVer.aucMsgElemntValue[0],&gastApconf[vulApIndex].aucActiveSftwareVer,stWTPActiveSftwareVer.usMsgElemntLen);

    ulActSwVerLen = sizeof(stWTPActiveSftwareVer.usMsgElemntType) + sizeof(stWTPActiveSftwareVer.usMsgElemntLen) + stWTPActiveSftwareVer.usMsgElemntLen;
    stWTPActiveSftwareVer.usMsgElemntLen = htons(stWTPActiveSftwareVer.usMsgElemntLen);

    //39-WTPBootVer启动版本；
    stWTPBootVer.usMsgElemntType = htons(2);
    stWTPBootVer.usMsgElemntLen = strlen("redboot-080129");
    cpss_mem_memcpy(&stWTPBootVer.aucMsgElemntValue[0],"redboot-080129",stWTPBootVer.usMsgElemntLen);

    ulBtVerLen = sizeof(stWTPBootVer.usMsgElemntType) + sizeof(stWTPBootVer.usMsgElemntLen) + stWTPBootVer.usMsgElemntLen;
    stWTPBootVer.usMsgElemntLen = htons(stWTPBootVer.usMsgElemntLen);

    //39-WTPOtherSftwareVer其他软件版本；
    stWTPOtherSftwareVer.usMsgElemntType = htons(3);
    stWTPOtherSftwareVer.usMsgElemntLen = strlen((char*)gastApconf[vulApIndex].aucApImgFileName);
    cpss_mem_memcpy(&stWTPOtherSftwareVer.aucMsgElemntValue[0],&gastApconf[vulApIndex].aucApImgFileName,stWTPOtherSftwareVer.usMsgElemntLen);
    ulOthSwVerLen = sizeof(stWTPOtherSftwareVer.usMsgElemntType) + sizeof(stWTPOtherSftwareVer.usMsgElemntLen) + stWTPOtherSftwareVer.usMsgElemntLen;
    stWTPOtherSftwareVer.usMsgElemntLen = htons(stWTPOtherSftwareVer.usMsgElemntLen);

    //condtruct IE39
    stMsgElemntT39.usMsgElemntType = htons(39);
    stMsgElemntT39.aucMsgElemntValue[0] = 1; //MAX Radios
    stMsgElemntT39.aucMsgElemntValue[1] = 1; //Radio in use
    stMsgElemntT39.aucMsgElemntValue[2] = 1;//ucEncryptCapabilityNum = 1;
    stMsgElemntT39.aucMsgElemntValue[3] = 1;//ucEncryptWBID = 1;//802.11
    usEncryptionCapabilities = 12;
    usEncryptionCapabilities = htons(usEncryptionCapabilities);
    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[4],&usEncryptionCapabilities,sizeof(usEncryptionCapabilities));

    usSuffix = 6;
    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
    usSuffix = usSuffix  + sizeof(ulNetCarrierInc);
    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&stWTPHdwareVer,ulHwVerLen);
    usSuffix = usSuffix  + ulHwVerLen;

    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
    usSuffix = usSuffix  + sizeof(ulNetCarrierInc);
    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&stWTPActiveSftwareVer,ulActSwVerLen);
    usSuffix = usSuffix  + ulActSwVerLen;

    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
    usSuffix = usSuffix  + sizeof(ulNetCarrierInc);
    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&stWTPBootVer,ulBtVerLen);
    usSuffix = usSuffix  + ulBtVerLen;

    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
    usSuffix = usSuffix  + sizeof(ulNetCarrierInc);
    cpss_mem_memcpy(&stMsgElemntT39.aucMsgElemntValue[usSuffix],&stWTPOtherSftwareVer,ulOthSwVerLen);
    usSuffix = usSuffix  + ulOthSwVerLen;

    stMsgElemntT39.usMsgElemntLen = usSuffix;

    ulT39Len = stMsgElemntT39.usMsgElemntLen + sizeof(stMsgElemntT39.usMsgElemntType) + sizeof(stMsgElemntT39.usMsgElemntLen);
    stMsgElemntT39.usMsgElemntLen = htons(stMsgElemntT39.usMsgElemntLen);

    //  T45,WTP Name
    stMsgElemntT45.usMsgElemntType = htons(45);
#if 1
    sprintf((char*)aucWtpName,"AP_%04d",vulApIndex);
#else
    strcpy(&aucWtpName,"AP_");
    sprintf(&aucIntChar,"%d",vulApIndex);
    strcat(&aucWtpName,&aucIntChar);
#endif
    stMsgElemntT45.usMsgElemntLen = strlen((char*)aucWtpName);
    cpss_mem_memcpy(&stMsgElemntT45.aucMsgElemntValue,&aucWtpName,stMsgElemntT45.usMsgElemntLen);

    ulT45Len = stMsgElemntT45.usMsgElemntLen + sizeof(stMsgElemntT45.usMsgElemntType) + sizeof(stMsgElemntT45.usMsgElemntLen);
    stMsgElemntT45.usMsgElemntLen = htons(stMsgElemntT45.usMsgElemntLen);

    //  T35,session ID
    stMsgElemntT35.usMsgElemntType = htons(35);
    stMsgElemntT35.usMsgElemntLen = 16;

    ulSessionId0 = htonl(1);
    ulSessionId1 = htonl(2);
    ulSessionId2 = htonl(3);
    ulSessionId3 = htonl(4);

    usSuffix = 0;
    cpss_mem_memcpy(&stMsgElemntT35.aucMsgElemntValue[usSuffix],&ulSessionId0,sizeof(ulSessionId0));
    usSuffix = usSuffix + sizeof(ulSessionId0);
    cpss_mem_memcpy(&stMsgElemntT35.aucMsgElemntValue[usSuffix],&ulSessionId1,sizeof(ulSessionId1));
    usSuffix = usSuffix + sizeof(ulSessionId1);
    cpss_mem_memcpy(&stMsgElemntT35.aucMsgElemntValue[usSuffix],&ulSessionId2,sizeof(ulSessionId2));
    usSuffix = usSuffix + sizeof(ulSessionId2);
    cpss_mem_memcpy(&stMsgElemntT35.aucMsgElemntValue[usSuffix],&ulSessionId3,sizeof(ulSessionId3));

    ulT35Len = stMsgElemntT35.usMsgElemntLen + sizeof(stMsgElemntT35.usMsgElemntType) + sizeof(stMsgElemntT35.usMsgElemntLen);
    stMsgElemntT35.usMsgElemntLen = htons(stMsgElemntT35.usMsgElemntLen);

    //  T41,WTP Frame Tunnel Mode
    stMsgElemntT41.usMsgElemntType = htons(41);
    stMsgElemntT41.usMsgElemntLen = 1;
    stMsgElemntT41.aucMsgElemntValue[0] = gastApconf[vulApIndex].ucWtpFrmTunnlMod;

    ulT41Len = stMsgElemntT41.usMsgElemntLen + sizeof(stMsgElemntT41.usMsgElemntType) + sizeof(stMsgElemntT41.usMsgElemntLen);
    stMsgElemntT41.usMsgElemntLen = htons(stMsgElemntT41.usMsgElemntLen);

    //  T44,WTP MAC Type
    stMsgElemntT44.usMsgElemntType = htons(44);
    stMsgElemntT44.usMsgElemntLen = 1;
    stMsgElemntT44.aucMsgElemntValue[0] = gastApconf[vulApIndex].ucWtpMacType;

    ulT44Len = stMsgElemntT44.usMsgElemntLen + sizeof(stMsgElemntT44.usMsgElemntType) + sizeof(stMsgElemntT44.usMsgElemntLen);
    stMsgElemntT44.usMsgElemntLen = htons(stMsgElemntT44.usMsgElemntLen);

    //  T1048,IEEE 802.11 WTP Radio Information
    stMsgElemntT1048.usMsgElemntType = htons(1048);
    stMsgElemntT1048.usMsgElemntLen = 5;
    stMsgElemntT1048.aucMsgElemntValue[0] = 1;//Radio ID
    stMsgElemntT1048.aucMsgElemntValue[1] = 0;//Resvd
    stMsgElemntT1048.aucMsgElemntValue[2] = 0;//Resvd
    stMsgElemntT1048.aucMsgElemntValue[3] = 0;//Resvd
    stMsgElemntT1048.aucMsgElemntValue[4] = gastApconf[vulApIndex].ucRadioType;//Radio Type
    ulT1048Len = stMsgElemntT1048.usMsgElemntLen + sizeof(stMsgElemntT1048.usMsgElemntType) + sizeof(stMsgElemntT1048.usMsgElemntLen);
    stMsgElemntT1048.usMsgElemntLen = htons(stMsgElemntT1048.usMsgElemntLen);

    //  T30,CAPWAP Local IPv4 Address
    stMsgElemntT30.usMsgElemntType = htons(30);
    stMsgElemntT30.usMsgElemntLen = MT_IPV4_ADDR_LEN;
    //GetIpFromStr(AF_INET,gastApconf[vulApIndex].aucApIpV4AddrStr,aucIpAddr);
//    stMsgElemntT30.aucMsgElemntValue[0] = aucIpAddr[0];
//    stMsgElemntT30.aucMsgElemntValue[1] = aucIpAddr[1];
//    stMsgElemntT30.aucMsgElemntValue[2] = aucIpAddr[2];
//    stMsgElemntT30.aucMsgElemntValue[3] = aucIpAddr[3];
    GetIpFromStr(AF_INET,gastApconf[vulApIndex].aucApIpV4AddrStr,stMsgElemntT30.aucMsgElemntValue);

    ulT30Len = stMsgElemntT30.usMsgElemntLen + sizeof(stMsgElemntT30.usMsgElemntType) + sizeof(stMsgElemntT30.usMsgElemntLen);
    stMsgElemntT30.usMsgElemntLen = htons(stMsgElemntT30.usMsgElemntLen);

    //  T37,Vendor Specific Payload
    //37-77 AP_LAST_UP_GRADE_TIM
    time((time_t*)&ulApUpgradeTime);//AP版本更新时间
    ulApUpgradeTime = htonl(ulApUpgradeTime);

    stMsgElemntT77.usMsgElemntType = htons(77);//AP_LAST_UP_GRADE_TIME；
    stMsgElemntT77.usMsgElemntLen = 4;
    cpss_mem_memcpy(&stMsgElemntT77.aucMsgElemntValue,&ulApUpgradeTime,stMsgElemntT77.usMsgElemntLen);

    ulT77Len = stMsgElemntT77.usMsgElemntLen + sizeof(stMsgElemntT77.usMsgElemntLen) + sizeof(stMsgElemntT77.usMsgElemntType);
    stMsgElemntT77.usMsgElemntLen = htons(stMsgElemntT77.usMsgElemntLen);

    //37-73 AP_SYSUPTIME
//    time(&ulApSysUpTime); //AP系统更新时间
//    ulApSysUpTime = htonl(ulApSysUpTime);
    ulApSysUpTime = htonl(3000);

    stMsgElemntT73.usMsgElemntType = htons(73);//AP_SYSUPTIME；
    stMsgElemntT73.usMsgElemntLen = 4;
    cpss_mem_memcpy(&stMsgElemntT73.aucMsgElemntValue,&ulApSysUpTime,stMsgElemntT73.usMsgElemntLen);

    ulT73Len = stMsgElemntT73.usMsgElemntLen + sizeof(stMsgElemntT73.usMsgElemntLen) + sizeof(stMsgElemntT73.usMsgElemntType);
    stMsgElemntT73.usMsgElemntLen = htons(stMsgElemntT73.usMsgElemntLen);

    //37-103  AP 地址信息(IPV4)
    cpss_mem_memcpy(stApIpv4Addr.aucGWAddr,gastAp[vulApIndex].aucApGwAddr,MT_IPV4_ADDR_LEN);
    cpss_mem_memcpy(stApIpv4Addr.aucIpAddr,gastAp[vulApIndex].aucApIpV4,MT_IPV4_ADDR_LEN);
    stApIpv4Addr.ulPreFix = htonl(gastAp[vulApIndex].ulIpv4Prefix);

    stMsgElemntT103.usMsgElemntType  = htons(103);
    stMsgElemntT103.usMsgElemntLen = MT_IPV4_ADDR_LEN + MT_IPV4_ADDR_LEN + sizeof(stApIpv4Addr.ulPreFix);
    cpss_mem_memcpy(stMsgElemntT103.aucMsgElemntValue,&stApIpv4Addr,stMsgElemntT103.usMsgElemntLen);

    ulT103Len = stMsgElemntT103.usMsgElemntLen + sizeof(stMsgElemntT103.usMsgElemntLen) + sizeof(stMsgElemntT103.usMsgElemntType);
    stMsgElemntT103.usMsgElemntLen = htons(stMsgElemntT103.usMsgElemntLen);

    //37-104  AP 地址信息(IPV6)
    cpss_mem_memcpy(stApIpv6Addr.aucIpAddr,gastAp[vulApIndex].aucApIpV6,MT_IPV6_ADDR_LEN);
    stApIpv6Addr.ulPreFix = htonl(gastAp[vulApIndex].ulIpv6Prefix);

    stMsgElemntT104.usMsgElemntType = htons(104);
    stMsgElemntT104.usMsgElemntLen = MT_IPV6_ADDR_LEN + sizeof(stApIpv6Addr.ulPreFix);
    cpss_mem_memcpy(stMsgElemntT104.aucMsgElemntValue,&stApIpv6Addr,stMsgElemntT104.usMsgElemntLen);

    ulT104Len = stMsgElemntT104.usMsgElemntLen + sizeof(stMsgElemntT104.usMsgElemntLen) + sizeof(stMsgElemntT104.usMsgElemntType);
    stMsgElemntT104.usMsgElemntLen = htons(stMsgElemntT104.usMsgElemntLen);

    //construct IE37
    stMsgElemntT37.usMsgElemntType = htons(37);
    usSuffix = 0;
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
    usSuffix = usSuffix + sizeof(ulNetCarrierInc);
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usSuffix],&stMsgElemntT77,ulT77Len);
    usSuffix = usSuffix + ulT77Len;
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usSuffix],&stMsgElemntT73,ulT73Len);
    usSuffix = usSuffix + ulT73Len;

    if(0 == gstAcConfInfo.ulAcIpType)//AC GW IPv4
          {
        cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usSuffix],&stMsgElemntT103,ulT103Len);
        usSuffix = usSuffix + ulT103Len;
          }
    else if(1 == gstAcConfInfo.ulAcIpType)//AC GW IPv6
          {
        cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usSuffix],&stMsgElemntT104,ulT104Len);
        usSuffix = usSuffix + ulT104Len;
          }

    stMsgElemntT37.usMsgElemntLen = usSuffix;
    ulT37Len = stMsgElemntT37.usMsgElemntLen + sizeof(stMsgElemntT37.usMsgElemntType) + sizeof(stMsgElemntT37.usMsgElemntLen);
    stMsgElemntT37.usMsgElemntLen = htons(stMsgElemntT37.usMsgElemntLen);

  //  T51,CAPWAP Transport Protocol
    stMsgElemntT51.usMsgElemntType = htons(51);
    stMsgElemntT51.usMsgElemntLen = 1;
    stMsgElemntT51.aucMsgElemntValue[0] = 2;

    ulT51Len = stMsgElemntT51.usMsgElemntLen + sizeof(stMsgElemntT51.usMsgElemntType) + sizeof(stMsgElemntT51.usMsgElemntLen);
    stMsgElemntT51.usMsgElemntLen = htons(stMsgElemntT51.usMsgElemntLen);

    //  T48,WTP Reboot Statistics
    stT48.usRebootCount = htons(0);//重启总次数.
    stT48.usACInitiatedCount = htons(0);//AC初始化引起重启的次数.
    stT48.usLinkFailureCount = htons(0);//链路失败引起的重启次数.
    stT48.usSWFailureCount = htons(0);//软件失败引起的重启次数.
    stT48.usHWFailureCount = htons(0);//硬件失败引起的重启次数.
    stT48.usOtherFailureCount = htons(0);//其它失败引起的AP重启总次数(非软件硬件和链路失败).
    stT48.usUnknownFailureCount = htons(0);//未知失败引起的重启次数.
    stT48.ucLastFailureType = 0;//最后一次失败类型.

    usSuffix = 0;
    stMsgElemntT48.usMsgElemntType = htons(48);
    stMsgElemntT48.usMsgElemntLen = 15;
    cpss_mem_memcpy(&stMsgElemntT48.aucMsgElemntValue[usSuffix],&stT48,stMsgElemntT48.usMsgElemntLen);
    ulT48Len = stMsgElemntT48.usMsgElemntLen + sizeof(stMsgElemntT48.usMsgElemntType) + sizeof(stMsgElemntT48.usMsgElemntLen);
    stMsgElemntT48.usMsgElemntLen = htons(stMsgElemntT48.usMsgElemntLen);

    //  T53,ECN Support
    stMsgElemntT53.usMsgElemntType = htons(53);
    stMsgElemntT53.usMsgElemntLen = 1;
    stMsgElemntT53.aucMsgElemntValue[0] = 0;

    ulT53Len = stMsgElemntT53.usMsgElemntLen + sizeof(stMsgElemntT53.usMsgElemntType) + sizeof(stMsgElemntT53.usMsgElemntLen);
    stMsgElemntT53.usMsgElemntLen = htons(stMsgElemntT53.usMsgElemntLen);

    //Join Request Message
    usSuffix = 0;
    cpss_mem_memcpy(&stJoinReq.aucMsgElemntBuf[usSuffix],&stMsgElemntT20,ulT20Len);
    usSuffix = usSuffix + ulT20Len;
    cpss_mem_memcpy(&stJoinReq.aucMsgElemntBuf[usSuffix],&stMsgElemntT28,ulT28Len);
    usSuffix = usSuffix + ulT28Len;
    cpss_mem_memcpy(&stJoinReq.aucMsgElemntBuf[usSuffix],&stMsgElemntT38,ulT38Len);
    usSuffix = usSuffix + ulT38Len;
    cpss_mem_memcpy(&stJoinReq.aucMsgElemntBuf[usSuffix],&stMsgElemntT39,ulT39Len);
    usSuffix = usSuffix + ulT39Len;
    cpss_mem_memcpy(&stJoinReq.aucMsgElemntBuf[usSuffix],&stMsgElemntT45,ulT45Len);
    usSuffix = usSuffix + ulT45Len;
    cpss_mem_memcpy(&stJoinReq.aucMsgElemntBuf[usSuffix],&stMsgElemntT35,ulT35Len);
    usSuffix = usSuffix + ulT35Len;
    cpss_mem_memcpy(&stJoinReq.aucMsgElemntBuf[usSuffix],&stMsgElemntT41,ulT41Len);
    usSuffix = usSuffix + ulT41Len;
    cpss_mem_memcpy(&stJoinReq.aucMsgElemntBuf[usSuffix],&stMsgElemntT44,ulT44Len);
    usSuffix = usSuffix + ulT44Len;
    cpss_mem_memcpy(&stJoinReq.aucMsgElemntBuf[usSuffix],&stMsgElemntT1048,ulT1048Len);
    usSuffix = usSuffix + ulT1048Len;
    cpss_mem_memcpy(&stJoinReq.aucMsgElemntBuf[usSuffix],&stMsgElemntT30,ulT30Len);
    usSuffix = usSuffix + ulT30Len;
    cpss_mem_memcpy(&stJoinReq.aucMsgElemntBuf[usSuffix],&stMsgElemntT37,ulT37Len);
    usSuffix = usSuffix + ulT37Len;
    cpss_mem_memcpy(&stJoinReq.aucMsgElemntBuf[usSuffix],&stMsgElemntT51,ulT51Len);
    usSuffix = usSuffix + ulT51Len;
    cpss_mem_memcpy(&stJoinReq.aucMsgElemntBuf[usSuffix],&stMsgElemntT48,ulT48Len);
    usSuffix = usSuffix + ulT48Len;
    cpss_mem_memcpy(&stJoinReq.aucMsgElemntBuf[usSuffix],&stMsgElemntT53,ulT53Len);
    usSuffix = usSuffix + ulT53Len;

    //capwap message
    stJoinReq.ucPreamble = (UINT8)0;
    stJoinReq.ucHdLen = 16;
    stJoinReq.aucHdFlag[0] = 3;
    stJoinReq.aucHdFlag[1] = 0;
    stJoinReq.usFragID = 0;
    stJoinReq.usFragOffset = 0;
    stJoinReq.aucMsgTypeEnterPrsNum[0] = (UINT8)0;
    stJoinReq.aucMsgTypeEnterPrsNum[1] = (UINT8)0;
    stJoinReq.aucMsgTypeEnterPrsNum[2] = (UINT8)0;
    stJoinReq.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_JOIN_REQUEST;
    stJoinReq.ucSeqNum = (UINT8)0;
    stJoinReq.usMsgElemnetLen = sizeof(stJoinReq.usMsgElemnetLen) + (UINT16)(sizeof(stJoinReq.ucFlag)) + usSuffix;
    stJoinReq.ucFlag = (UINT8)0;
    ulJoinReqLen = 13 + stJoinReq.usMsgElemnetLen;
    stJoinReq.usMsgElemnetLen = htons(stJoinReq.usMsgElemnetLen);

    gstJoinReq.ulBufLen = ulJoinReqLen;
    cpss_mem_memcpy(&gstJoinReq.aucBuff,(UINT8*)&stJoinReq,gstJoinReq.ulBufLen);
     }
  else
    {
    cpss_mem_memcpy(&gstJoinReq.aucBuff[127], &(gastAp[vulApIndex].auApMac),MT_MAC_LENTH);
    }

  bSendResult = MT_SendApMsg2AC(vulApIndex,glCpmSockId, gstJoinReq.aucBuff,gstJoinReq.ulBufLen);

  if (MT_FALSE == bSendResult)
    {
    printf("\nERROR.MT_ApSendMsg2CPM send AP(%d) JoinReq failure!\nFile:%s.Line:%d\n",vulApIndex,__FILE__,__LINE__);
    }
  /*CHANGE AP STATE*/
  return bSendResult;
}

//ConfigStatusReq.AP->AC
MTBool ApMutiTestSendConfigStatusReq(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;
  CAPWAP_MSG_T stConfigStatusReq;
  UINT32 ulConfigStatusReqLen = 0;
  MSG_ELEMNT_T stMsgElemntT4;//AC NAME
  UINT32 ulT4Len = 0;
  MT_T4 stT4;
  MSG_ELEMNT_T stMsgElemntT31;//Radio Administrative State
  UINT32 ulT31Len = 0;
  MT_T31 stT31;
  MSG_ELEMNT_T stMsgElemntT36;//Statistic Timer
  UINT32 ulT36Len = 0;
  MT_T36 stT36;
  MSG_ELEMNT_T stMsgElemntT48;//WTP Reboot Statistics
  UINT32 ulT48Len = 0;
  MT_T48 stT48;
  MSG_ELEMNT_T stMsgElemntT37;//Vendor Specific Payload
  UINT32 ulT37Len = 0;
  UINT32 ulNetCarrierInc = 0;
  MSG_ELEMNT_T stMsgElemntT1025;//IEEE 802.11 Antenna
  UINT32 ulT1025Len = 0;
  MT_T1025 stT1025;
  MSG_ELEMNT_T stMsgElemntT1028;//IEEE 802.11 Direct Swquence Control
  UINT32 ulT1028Len = 0;
  MT_T1028 stT1028;
  MSG_ELEMNT_T stMsgElemntT1030;//IEEE802.11 MAC Operation
  UINT32 ulT1030Len = 0;
  MT_T1030 stT1030;
  MSG_ELEMNT_T stMsgElemntT1032;//IEEE802.11 Multi-Domain Capability
  UINT32 ulT1032Len = 0;
  MT_T1032 stT1032;
  MSG_ELEMNT_T stMsgElemntT1033;//IEEE802.11 OFDM Control
  UINT32 ulT1033Len = 0;
  MT_T1033 stT1033;
  MSG_ELEMNT_T stMsgElemntT1040;//IEEE802.11 Supported Rates
  UINT32 ulT1040Len = 0;
  MT_T1040 stT1040;
  MSG_ELEMNT_T stMsgElemntT1041;//IEEE802.11 Tx Power
  UINT32 ulT1041Len = 0;
  MT_T1041 stT1041;
  MSG_ELEMNT_T stMsgElemntT1042;//IEEE802.11 Tx Power Level
  UINT32 ulT1042Len = 0;
  MT_T1042 stT1042;
  MSG_ELEMNT_T stMsgElemntT1046;//IEEE802.11 WTP Radio Configuration
  UINT32 ulT1046Len = 0;
  MT_T1046 stT1046;
  MSG_ELEMNT_T stMsgElemntT1048;//IEEE802.11 WTP Radio Information
  UINT32 ulT1048Len = 0;
  MT_T1048 stT1048;
  MSG_ELEMNT_T stMsgElemntT85;//WTP Radio Info
  UINT32 ulT85Len = 0;
  MT_IE37_IE85_T stIE85;

  UINT32 ulSuffix = 0;
  UINT32 ulLoop = 0;

  if(MT_FALSE == gbTestMod)
     {
    cpss_mem_memset(&stConfigStatusReq,0,sizeof(stConfigStatusReq));

    cpss_mem_memset(&stMsgElemntT4,0,sizeof(stMsgElemntT4));
    cpss_mem_memset(&stMsgElemntT31,0,sizeof(stMsgElemntT31));
    cpss_mem_memset(&stMsgElemntT36,0,sizeof(stMsgElemntT36));
    cpss_mem_memset(&stMsgElemntT48,0,sizeof(stMsgElemntT48));
    cpss_mem_memset(&stMsgElemntT37,0,sizeof(stMsgElemntT37));
    cpss_mem_memset(&stMsgElemntT1025,0,sizeof(stMsgElemntT1025));
    cpss_mem_memset(&stMsgElemntT1028,0,sizeof(stMsgElemntT1028));
    cpss_mem_memset(&stMsgElemntT1030,0,sizeof(stMsgElemntT1030));
    cpss_mem_memset(&stMsgElemntT1032,0,sizeof(stMsgElemntT1032));
    cpss_mem_memset(&stMsgElemntT1033,0,sizeof(stMsgElemntT1033));
    cpss_mem_memset(&stMsgElemntT1040,0,sizeof(stMsgElemntT1040));
    cpss_mem_memset(&stMsgElemntT1041,0,sizeof(stMsgElemntT1041));
    cpss_mem_memset(&stMsgElemntT1042,0,sizeof(stMsgElemntT1042));
    cpss_mem_memset(&stMsgElemntT1046,0,sizeof(stMsgElemntT1046));
    cpss_mem_memset(&stMsgElemntT1048,0,sizeof(stMsgElemntT1048));
    cpss_mem_memset(&stMsgElemntT85,0,sizeof(stMsgElemntT85));
    cpss_mem_memset(&stT4,0,sizeof(stT4));
    cpss_mem_memset(&stT31,0,sizeof(stT31));
    cpss_mem_memset(&stT36,0,sizeof(stT36));
    cpss_mem_memset(&stT48,0,sizeof(stT48));
    cpss_mem_memset(&stIE85,0,sizeof(stIE85));
    cpss_mem_memset(&stT1025,0,sizeof(stT1025));
    cpss_mem_memset(&stT1028,0,sizeof(stT1028));
    cpss_mem_memset(&stT1030,0,sizeof(stT1030));
    cpss_mem_memset(&stT1032,0,sizeof(stT1032));
    cpss_mem_memset(&stT1033,0,sizeof(stT1033));
    cpss_mem_memset(&stT1040,0,sizeof(stT1040));
    cpss_mem_memset(&stT1041,0,sizeof(stT1041));
    cpss_mem_memset(&stT1042,0,sizeof(stT1042));
    cpss_mem_memset(&stT1046,0,sizeof(stT1046));
    cpss_mem_memset(&stT1048,0,sizeof(stT1048));

    ulNetCarrierInc = htonl(gastApconf[vulApIndex].ulNetCarrierInc);

    //AC NAME(4)
    if(strlen((char*)gstAcConfInfo.aucAcName) < MT_AC_NAME_LEN)
          {
    	strcpy((char*)stT4.aucAcName,(char*)gstAcConfInfo.aucAcName);
          }

    ulSuffix = 0;
    stMsgElemntT4.usMsgElemntType = htons(4);
    stMsgElemntT4.usMsgElemntLen = strlen((char*)gstAcConfInfo.aucAcName);
    cpss_mem_memcpy(&stMsgElemntT4.aucMsgElemntValue[ulSuffix],&stT4,stMsgElemntT4.usMsgElemntLen);
    ulT4Len = sizeof(stMsgElemntT4.usMsgElemntType) + sizeof(stMsgElemntT4.usMsgElemntLen) + stMsgElemntT4.usMsgElemntLen;
    stMsgElemntT4.usMsgElemntLen = htons(stMsgElemntT4.usMsgElemntLen);

    //Radio Administrative State(31)
    stT31.ucRadioID = 1;
    stT31.ucAdminState = 0;

    ulSuffix = 0;
    stMsgElemntT31.usMsgElemntType = htons(31);
    stMsgElemntT31.usMsgElemntLen = 2;
    cpss_mem_memcpy(&stMsgElemntT31.aucMsgElemntValue[ulSuffix],&stT31,stMsgElemntT31.usMsgElemntLen);

    ulT31Len = sizeof(stMsgElemntT31.usMsgElemntType) + sizeof(stMsgElemntT31.usMsgElemntLen) + stMsgElemntT31.usMsgElemntLen;
    stMsgElemntT31.usMsgElemntLen = htons(stMsgElemntT31.usMsgElemntLen);

    //Statistic Timer(36)
    stT36.usStatisticsTimer = htons(10);

    ulSuffix = 0;
    stMsgElemntT36.usMsgElemntType = htons(36);
    stMsgElemntT36.usMsgElemntLen = 2;

    cpss_mem_memcpy(&stMsgElemntT36.aucMsgElemntValue[ulSuffix],&stT36,stMsgElemntT36.usMsgElemntLen);
    ulT36Len = sizeof(stMsgElemntT36.usMsgElemntType) + sizeof(stMsgElemntT36.usMsgElemntLen) + stMsgElemntT36.usMsgElemntLen;
    stMsgElemntT36.usMsgElemntLen = htons(stMsgElemntT36.usMsgElemntLen);

    //WTP Reboot Statistics(48)
    stT48.usRebootCount = htons(0);//重启总次数.
    stT48.usACInitiatedCount = htons(0);//AC初始化引起重启的次数.
    stT48.usLinkFailureCount = htons(0);//链路失败引起的重启次数.
    stT48.usSWFailureCount = htons(0);//软件失败引起的重启次数.
    stT48.usHWFailureCount = htons(0);//硬件失败引起的重启次数.
    stT48.usOtherFailureCount = htons(0);//其它失败引起的AP重启总次数(非软件硬件和链路失败).
    stT48.usUnknownFailureCount = htons(0);//未知失败引起的重启次数.
    stT48.ucLastFailureType = 0;//最后一次失败类型.

    ulSuffix = 0;
    stMsgElemntT48.usMsgElemntType = htons(48);
    stMsgElemntT48.usMsgElemntLen = 15;
    cpss_mem_memcpy(&stMsgElemntT48.aucMsgElemntValue[ulSuffix],&stT48,stMsgElemntT48.usMsgElemntLen);
    ulT48Len = sizeof(stMsgElemntT48.usMsgElemntType) + sizeof(stMsgElemntT48.usMsgElemntLen) + stMsgElemntT48.usMsgElemntLen;
    stMsgElemntT48.usMsgElemntLen = htons(stMsgElemntT48.usMsgElemntLen);

    //Vendor Specific Payload(37)
    //IE85
    stIE85.ulRadioId = htonl(1);//Radio标识
    strcpy((char*)stIE85.aucWtpWifDescr,"WAPI");//接口描述，为16字节字符串
    stIE85.ulIfType = htonl(1);//接口类型
    stIE85.ulWtpWifMtu = htonl(1500);//MTU，最大传输单元
    stIE85.ulWtpWifSpeed = htonl(54);//接口带宽，单位为Mbps
    cpss_mem_memcpy(stIE85.aucWtpWifMacAddress,gastAp[vulApIndex].auApMac,MT_MAC_LENTH);//接口Mac地址
    stIE85.aucWtpWifMacAddress[0] = 0x85;
    stIE85.ulDot11CurrntCh = htonl(1);//AP当前频道ch1(1), ch2(2), ch3(3), ch4(4), ch5(5),ch6(6), ch7(7), ch8(8), ch9(9), ch10(10), ch11(11), ch12(12), ch13(13), ch149(149), ch153(153), ch157(157), ch161(161), ch165(165)。
    stIE85.ulRadioType = htonl(1);//Radio类型，0x01-11b，0x02-11a，0x04-11g，0x08-11n。 注释：Radio模式和CAPWAP协议标准定义不一致但和中移动网管定义的一致。
    strcpy((char*)stIE85.ucMaxTxPowerLevel,"-10");//最大发射功率，12字节字符串，单位为dBm。如：功率值为-5dBm，则表示为“-5”；功率值为5.5dBm，则表示为“5.5”。注: 放装型设备发射功率范围为(0-20), 分布型设备发射功率范围为(20-27)。
    stIE85.ulPwrAttRange = htonl(10);//功率衰减范围
    stIE85.ulPwrAttValue = htonl(10);//功率调整步长
    stIE85.ulDot11AntennaGain = htonl(10);//AP 802.11天线增益
    stIE85.ulWtpWifOperStatus = htonl(0);//接口当前状态，0:up 1:down  2:admindown
    stIE85.ulWifLastChange = htonl(1000);//接口进入当前工作状态持续的时间，单位为0.01秒

    stMsgElemntT85.usMsgElemntType = htons(85);
    stMsgElemntT85.usMsgElemntLen = sizeof(stIE85);
    cpss_mem_memcpy(&stMsgElemntT85.aucMsgElemntValue[0],&stIE85,stMsgElemntT85.usMsgElemntLen);
    ulT85Len = sizeof(stMsgElemntT85.usMsgElemntType) + sizeof(stMsgElemntT85.usMsgElemntLen) + stMsgElemntT85.usMsgElemntLen;
    stMsgElemntT85.usMsgElemntLen = htons(stMsgElemntT85.usMsgElemntLen);

    ulSuffix = 0;
    stMsgElemntT37.usMsgElemntType = htons(37);
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
    ulSuffix = ulSuffix + sizeof(ulNetCarrierInc);

    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT85,ulT85Len);
    ulSuffix = ulSuffix + ulT85Len;
    stMsgElemntT37.usMsgElemntLen = ulSuffix;

    ulT37Len = sizeof(stMsgElemntT37.usMsgElemntType) + sizeof(stMsgElemntT37.usMsgElemntLen) + stMsgElemntT37.usMsgElemntLen;
    stMsgElemntT37.usMsgElemntLen = htons(stMsgElemntT37.usMsgElemntLen);

    //IEEE 802.11 Antenna(1025)
    stT1025.ucRadioID = 1;//Radio编号，1-31
    stT1025.ucDiversity = 1;//天线分极。
    stT1025.ucCombiner = 2;//天线组合。
    stT1025.ucAntennaCnt = 2;//天线数
    for(ulLoop = 0; ulLoop < stT1025.ucAntennaCnt; ulLoop++)
           {
    	  stT1025.aucAntennaSelection[ulLoop] = 1;//1为内置，2为外置
           }

    stMsgElemntT1025.usMsgElemntType = htons(1025);
    stMsgElemntT1025.usMsgElemntLen = 4 + stT1025.ucAntennaCnt;

    ulSuffix = 0;
    cpss_mem_memcpy(&stMsgElemntT1025.aucMsgElemntValue[ulSuffix],&stT1025,stMsgElemntT1025.usMsgElemntLen);
    ulT1025Len  = sizeof(stMsgElemntT1025.usMsgElemntType) + sizeof(stMsgElemntT1025.usMsgElemntLen) + stMsgElemntT1025.usMsgElemntLen;
    stMsgElemntT1025.usMsgElemntLen = htons(stMsgElemntT1025.usMsgElemntLen);

    //IEEE 802.11 Direct Swquence Control(1028)
    stT1028.ucRadioID = 1;//8bit。范围1-31。
    stT1028.ucCurrentChannel = 3;//8bit。当前radio的信道。范围1-13（根据国家和地区而定）。
    stT1028.ucCurrentCCA = 2;//(该字段内容暂不做要求) 当前的空闲信道评估模式设置。可参考IEEE 802.11 dot11CCAModeSupported MIB（见 [IEEE.802-11.2007]）。取值如下：
	//         1 - 能量检测(edonly)
	//         2 - 载波检测(csonly)
	//         4 - 载波检测和能量检测混合(edandcs)
	//         8 - 定时载波检测(cswithtimer)
	//        16 - 高速载波检测和能量检测(hrcsanded)
    stT1028.ulEnergyDetectThrd = htonl(20);//（该字段内容暂不做要求） 能量检测的能量阀值，接收端根据能量是否高于此阀值来判断。可参考IEEE 802.11 dot11EDThreshold MIB（见 [IEEE.802-11.2007]）。

    stMsgElemntT1028.usMsgElemntType = htons(1028);
    stMsgElemntT1028.usMsgElemntLen = 8;

    ulSuffix = 0;
    cpss_mem_memcpy(&stMsgElemntT1028.aucMsgElemntValue[ulSuffix],&stT1028,stMsgElemntT1028.usMsgElemntLen);
    ulT1028Len = sizeof(stMsgElemntT1028.usMsgElemntType) + sizeof(stMsgElemntT1028.usMsgElemntLen) + stMsgElemntT1028.usMsgElemntLen;
    stMsgElemntT1028.usMsgElemntLen = htons(stMsgElemntT1028.usMsgElemntLen);

    //IEEE802.11 MAC Operation(1030)
    stT1030.ucRadioID = 1;//8bit。范围1-31。
    stT1030.usRTSThreshold = htons(2346);//16bit。设置启用RTS（发送请求）机制所要求的帧的长度门限值，取值范围是0～2346。缺省情况下，启用RTS机制所要求的帧长度门限值为2346字节。当帧的实际长度大于设定的门限值时，则系统启用RTS机制。RTS（Request To Send，要求发送）用于在无线局域网中避免数据发送冲突。RTS包的发送频率需要合理设置，设置RTS门限时需要进行权衡：如果将这个参数值设得较小，则会增加RTS包的发送频率，消耗更多的带宽。但RTS包发送得越频繁，系统从中断或冲突中恢复得就越快。
    stT1030.ucShortRetry = 10;//8bit。帧长小于RTS门限值的帧的最大重传次数，取值范围1-15。缺省情况下，帧长小于RTS门限值的帧的最大重传次数为7。此值来源于消息元素IEEE 802.11 dot11ShortRetryLimit MIB（见 [IEEE.802-11.2007]）。
    stT1030.ucLongRetry = 10;//8bit。帧长超过RTS门限值的帧的最大重传次数，取值范围1-15。缺省情况下，帧长超过RTS门限值的帧的最大重传次数为4。此值来源于消息元素IEEE 802.11 dot11LongRetryLimit MIB（见 [IEEE.802-11.2007]）。
    stT1030.usFragmtThrd = htons(2346);//8bit。帧的分片门限值，取值范围为256-2346，单位为字节。当数据包的实际大小超过指定的分片门限值时，该数据包被分片传输。缺省情况下，帧的分片门限值为2346字节，即小于2346字节的帧在传输时不分片。此值来源于消息元素IEEE 802.11 dot11FragmentationThreshold MIB（见 [IEEE.802-11.2007]）。
    stT1030.ulTxMSDULifetime = htonl(512);//传输一个Mac服务数据单元的最长周期。默认值为512。此值来源于消息元素IEEE 802.11 dot11MaxTransmitMSDULifetime MIB（见 [IEEE.802-11.2007]）。
    stT1030.ulRxMSDULifetime = htonl(512);//接收一个Mac服务数据单元的最长周期。默认值为512。此值来源于消息元素IEEE 802.11 dot11MaxReceiveLifetime MIB（见 [IEEE.802-11.2007]）。

    ulSuffix = 0;
    stMsgElemntT1030.usMsgElemntType = htons(1030);
    stMsgElemntT1030.usMsgElemntLen = 16;
    cpss_mem_memcpy(&stMsgElemntT1030.aucMsgElemntValue[ulSuffix],&stT1030,stMsgElemntT1030.usMsgElemntLen);
    ulT1030Len = sizeof(stMsgElemntT1030.usMsgElemntType) + sizeof(stMsgElemntT1030.usMsgElemntLen) + stMsgElemntT1030.usMsgElemntLen;
    stMsgElemntT1030.usMsgElemntLen = htons(stMsgElemntT1030.usMsgElemntLen);

    //IEEE802.11 Multi-Domain Capability(1032)
    stT1032.ucRadioID = 1;//Radio编号，1-31.
    stT1032.usFirstChannel = htons(1);// #：频段内首信道
    stT1032.usNumberofChannls = htons(13);//总可用信道
    stT1032.usMaxTxPwrLevl = htons(27);//最大发射功率

    ulSuffix = 0;
    stMsgElemntT1032.usMsgElemntType = htons(1032);
    stMsgElemntT1032.usMsgElemntLen = 8;
    cpss_mem_memcpy(&stMsgElemntT1032.aucMsgElemntValue[ulSuffix],&stT1032, stMsgElemntT1032.usMsgElemntLen);
    ulT1032Len = sizeof(stMsgElemntT1032.usMsgElemntType) + sizeof(stMsgElemntT1032.usMsgElemntLen) + stMsgElemntT1032.usMsgElemntLen;
    stMsgElemntT1032.usMsgElemntLen = htons(stMsgElemntT1032.usMsgElemntLen);

    //IEEE802.11 OFDM Control(1033)
    stT1033.ucRadioID = 1;//数值1-31
    stT1033.ucCurrntChan = 2;//当前信道
    stT1033.ucBandSupport = 2;//支持的频带
    stT1033.ulTIThrld = htonl(10);//CCA规定的门限

    ulSuffix = 0;
    stMsgElemntT1033.usMsgElemntType = htons(1033);
    stMsgElemntT1033.usMsgElemntLen = 8;
    cpss_mem_memcpy(&stMsgElemntT1033.aucMsgElemntValue[ulSuffix],&stT1033,stMsgElemntT1033.usMsgElemntLen);
    ulT1033Len = sizeof(stMsgElemntT1033.usMsgElemntType) + sizeof(stMsgElemntT1033.usMsgElemntLen) + stMsgElemntT1033.usMsgElemntLen;
    stMsgElemntT1033.usMsgElemntLen = htons(stMsgElemntT1033.usMsgElemntLen);

    //IEEE802.11 Supported Rates(1040)
    stT1040.ucRadioID = 1;
//    Selects among rates:(802.11-2007.pdf P413)
//    02 = 1 Mb/s 03 = 1.5 Mb/s 04 = 2 Mb/s 05 = 2.5 Mb/s
//    06 = 3 Mb/s 09 = 4.5 Mb/s 11 = 5.5 Mb/s 12 = 6 Mb/s
//    18 = 9 Mb/s 22 = 11 Mb/s 24 = 12 Mb/s 27 = 13.5 Mb/s
//    36 = 18 Mb/s 44 = 22 Mb/s 48 = 24 Mb/s 54 = 27 Mb/s
//    66 = 33 Mb/s 72 = 36 Mb/s 96 = 48 Mb/s 108 = 54 Mb/s
//           1, 2, 5.5, 6, 9, 11, 12, 18,
//           24, 36, 48, 54,
#if 0
    stT1040.aucSupprtedRates[0] = 2;//02 = 1 Mb/s
    stT1040.aucSupprtedRates[1] = 4;//04 = 2 Mb/s
    stT1040.aucSupprtedRates[2] = 11;//11 = 5.5 Mb/s
    stT1040.aucSupprtedRates[3] = 12;//12 = 6 Mb/s
    stT1040.aucSupprtedRates[4] = 18;//18 = 9 Mb/s
    stT1040.aucSupprtedRates[5] = 22;//22 = 11 Mb/s
    stT1040.aucSupprtedRates[6] = 24;//24 = 12 Mb/s
    stT1040.aucSupprtedRates[7] = 36;//36 = 18 Mb/s
#endif
    stT1040.aucSupprtedRates[0] = 128 + 1 * 2;//02 = 1 Mb/s
    stT1040.aucSupprtedRates[1] = 128 + 2 * 2;//04 = 2 Mb/s
    stT1040.aucSupprtedRates[2] = 128 + 5.5 * 2;//11 = 5.5 Mb/s
    stT1040.aucSupprtedRates[3] = 128 + 6 * 2;//12 = 6 Mb/s
    stT1040.aucSupprtedRates[4] = 128 + 9 * 2;//18 = 9 Mb/s
    stT1040.aucSupprtedRates[5] = 128 + 11 * 2;//22 = 11 Mb/s
    stT1040.aucSupprtedRates[6] = 128 + 12 * 2;//24 = 12 Mb/s
    stT1040.aucSupprtedRates[7] = 128 + 18 * 2;//36 = 18 Mb/s

    ulSuffix = 0;
    stMsgElemntT1040.usMsgElemntType = htons(1040);
    stMsgElemntT1040.usMsgElemntLen = 9;
    cpss_mem_memcpy(&stMsgElemntT1040.aucMsgElemntValue[ulSuffix],&stT1040,stMsgElemntT1040.usMsgElemntLen);
    ulT1040Len = sizeof(stMsgElemntT1040.usMsgElemntType) + sizeof(stMsgElemntT1040.usMsgElemntLen) + stMsgElemntT1040.usMsgElemntLen;
    stMsgElemntT1040.usMsgElemntLen = htons(stMsgElemntT1040.usMsgElemntLen);

    //IEEE802.11 Tx Power(1041)
    stT1041.ucRadioID = 1;//Radio ID:8bit。范围1-31。
    stT1041.usCurrntTxPwr = htons(20);//Current Tx Power:发射功率值。单位dbm。

    ulSuffix = 0;
    stMsgElemntT1041.usMsgElemntType = htons(1041);
    stMsgElemntT1041.usMsgElemntLen = 4;
    cpss_mem_memcpy(&stMsgElemntT1041.aucMsgElemntValue[ulSuffix],&stT1041,stMsgElemntT1041.usMsgElemntLen);
    ulT1041Len = sizeof(stMsgElemntT1041.usMsgElemntType) + sizeof(stMsgElemntT1041.usMsgElemntLen) + stMsgElemntT1041.usMsgElemntLen;
    stMsgElemntT1041.usMsgElemntLen = htons(stMsgElemntT1041.usMsgElemntLen);

    //IEEE802.11 Tx Power Level(1042)
    stT1042.ucRadioID = 1;//Radio ID:8bit。范围1-31。
    stT1042.ucTxPwrLvlNum = 8;//Num Levels:   The number of power level attributes.  The value of this field comes from the IEEE 802.11 dot11NumberSupportedPowerLevels MIB element (see[IEEE.802-11.2007]).

    for(ulLoop = 0; ulLoop < stT1042.ucTxPwrLvlNum; ulLoop++)
          {
    	stT1042.ausPowerLevel[ulLoop] = htons(ulLoop);
          }

    ulSuffix = 0;
    stMsgElemntT1042.usMsgElemntType = htons(1042);
    stMsgElemntT1042.usMsgElemntLen = 18;
    cpss_mem_memcpy(&stMsgElemntT1042.aucMsgElemntValue[ulSuffix],&stT1042,stMsgElemntT1042.usMsgElemntLen);
    ulT1042Len = sizeof(stMsgElemntT1042.usMsgElemntType) + sizeof(stMsgElemntT1042.usMsgElemntLen) + stMsgElemntT1042.usMsgElemntLen;
    stMsgElemntT1042.usMsgElemntLen = htons(stMsgElemntT1042.usMsgElemntLen);

    //IEEE802.11 WTP Radio Configuration(1046)
    stT1046.ucRadioID = 1;//8bit。范围1-31。
    stT1046.ucShortPreamble = 0;//8bit。表明是否支持短前导码。取值范围：
    //     	 0 -   不支持短前导码
    //     	 1 -   支持短前导码
    stT1046.ucNumberOfBSSIDs = 1;//这个字段表明AP支持的最大BSSID数，这个数约束了AP上支持的逻辑网络数量，范围是1-16。
    stT1046.ucDTIMPeriod = 1;//DTIM间隔设定了信标帧间隔的数量，信标帧中的TIM元素中包含了DTIM字段，如果设置为1，则每个信标帧中都包含DTIM，如果设置为2，则每2个信标帧中包含1个DTIM，以此类推。这个字段来源于IEEE 802.11 dot11DTIMPeriod MIB element (见[IEEE.802-11.2007]）。
    MT_GetBssID(vulApIndex,stT1046.ucRadioID,1,stT1046.aucBSSID);//Radio的MAC地址。
    stT1046.usBeaconPeriod = htons(10);//（信标帧间隔）,这个值设置了一个站点发送信标帧的时间间隔，在信标帧和探测帧回复（Probe Response）中都包含这个字段。这个信息来源于IEEE 802.11 dot11BeaconPeriod MIB元素（见 [IEEE.802-11.2007]）。
    strcpy((char*)stT1046.aucCountryStr,"CN");//（国家码）:
    //        CN:中国
    // 	      EU:欧洲
    //        US:美国
    //        JP:日本
    //        FR:法国
    //        ES:西班牙
    //        XX:未定义,目前支持这些国家码的设定

    ulSuffix = 0;
    stMsgElemntT1046.usMsgElemntType = htons(1046);
    stMsgElemntT1046.usMsgElemntLen = 16;
    cpss_mem_memcpy(&stMsgElemntT1046.aucMsgElemntValue[ulSuffix],&stT1046,stMsgElemntT1046.usMsgElemntLen);
    ulT1046Len = sizeof(stMsgElemntT1046.usMsgElemntType) + sizeof(stMsgElemntT1046.usMsgElemntLen) + stMsgElemntT1046.usMsgElemntLen;
    stMsgElemntT1046.usMsgElemntLen = htons(stMsgElemntT1046.usMsgElemntLen);

    //IEEE802.11 WTP Radio Information(1048)
    stT1048.ucRadioID = 1;//Radio ID
    stT1048.ulRadioType = htonl(gastApconf[vulApIndex].ucRadioType);//Radio Type

    ulSuffix = 0;
    stMsgElemntT1048.usMsgElemntType = htons(1048);
    stMsgElemntT1048.usMsgElemntLen = 5;
    cpss_mem_memcpy(&stMsgElemntT1048.aucMsgElemntValue[ulSuffix],&stT1048,stMsgElemntT1048.usMsgElemntLen);
    ulT1048Len = sizeof(stMsgElemntT1048.usMsgElemntType) + sizeof(stMsgElemntT1048.usMsgElemntLen) + stMsgElemntT1048.usMsgElemntLen;
    stMsgElemntT1048.usMsgElemntLen = htons(stMsgElemntT1048.usMsgElemntLen);

    //Construct ConfigStatusReq Buffer
    ulSuffix = 0;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT4,ulT4Len);
    ulSuffix = ulSuffix + ulT4Len;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT31,ulT31Len);
    ulSuffix = ulSuffix + ulT31Len;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT36,ulT36Len);
    ulSuffix = ulSuffix + ulT36Len;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT48,ulT48Len);
    ulSuffix = ulSuffix + ulT48Len;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT37,ulT37Len);
    ulSuffix = ulSuffix + ulT37Len;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT1025,ulT1025Len);
    ulSuffix = ulSuffix + ulT1025Len;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT1028,ulT1028Len);
    ulSuffix = ulSuffix + ulT1028Len;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT1030,ulT1030Len);
    ulSuffix = ulSuffix + ulT1030Len;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT1032,ulT1032Len);
    ulSuffix = ulSuffix + ulT1032Len;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT1033,ulT1033Len);
    ulSuffix = ulSuffix + ulT1033Len;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT1040,ulT1040Len);
    ulSuffix = ulSuffix + ulT1040Len;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT1041,ulT1041Len);
    ulSuffix = ulSuffix + ulT1041Len;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT1042,ulT1042Len);
    ulSuffix = ulSuffix + ulT1042Len;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT1046,ulT1046Len);
    ulSuffix = ulSuffix + ulT1046Len;
    cpss_mem_memcpy(&stConfigStatusReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT1048,ulT1046Len);
    ulSuffix = ulSuffix + ulT1048Len;

    //construct ConfigStatusReq MSG
    stConfigStatusReq.ucPreamble = (UINT8)0;
    stConfigStatusReq.ucHdLen = 16;
    stConfigStatusReq.aucHdFlag[0] = 3;
    stConfigStatusReq.aucHdFlag[1] = 0;
    stConfigStatusReq.usFragID = 0;
    stConfigStatusReq.usFragOffset = 0;
    stConfigStatusReq.aucMsgTypeEnterPrsNum[0] = (UINT8)0;
    stConfigStatusReq.aucMsgTypeEnterPrsNum[1] = (UINT8)0;
    stConfigStatusReq.aucMsgTypeEnterPrsNum[2] = (UINT8)0;
    stConfigStatusReq.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_CONFIGURE_REQUEST;
    stConfigStatusReq.ucSeqNum = (UINT8)0;
    stConfigStatusReq.usMsgElemnetLen = sizeof(stConfigStatusReq.usMsgElemnetLen) + (UINT16)(sizeof(stConfigStatusReq.ucFlag)) + ulSuffix;
    stConfigStatusReq.ucFlag = (UINT8)0;
    ulConfigStatusReqLen = 13 + stConfigStatusReq.usMsgElemnetLen;
    stConfigStatusReq.usMsgElemnetLen = htons(stConfigStatusReq.usMsgElemnetLen);

    gstConfigurationStatusReq.ulBufLen = ulConfigStatusReqLen;
    cpss_mem_memcpy(&gstConfigurationStatusReq.aucBuff,(UINT8*)&stConfigStatusReq,gstConfigurationStatusReq.ulBufLen);
     }
  else
    {
    cpss_mem_memcpy(&gstConfigurationStatusReq.aucBuff[95],&(gastAp[vulApIndex].auApMac), MT_MAC_LENTH);
    }

  bSendResult = MT_SendApMsg2AC(vulApIndex,glCpmSockId, gstConfigurationStatusReq.aucBuff, gstConfigurationStatusReq.ulBufLen);

  if (MT_FALSE == bSendResult)
     {
    printf("\nMT_ApSendMsg2CPM send AP(%d) ConfigStatusReq failure!\nFile:%s.Line:%d\n",vulApIndex,__FILE__,__LINE__);
     }
  return bSendResult;
}

/*ChangeStateReq消息AP->AC*/
MTBool ApMutiTestSendChangeStateReq(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;
  CAPWAP_MSG_T stChangeStateReq;
  UINT32 ulChangeStateReqLen = 0;
  MSG_ELEMNT_T stMsgElemntT33;//Result Code(33)
  UINT32 ulT33Len = 0;
  MT_T33 stT33;
  MSG_ELEMNT_T stMsgElemntT32;//Radio Operational State(32)
  UINT32 ulT32Len = 0;
  MT_T32 stT32;

  UINT32 ulSuffix = 0;
//  UINT32 ulMsgElemntSuffix = 0;

  if(MT_FALSE == gbTestMod)
     {
    cpss_mem_memset(&stChangeStateReq,0,sizeof(stChangeStateReq));
    cpss_mem_memset(&stMsgElemntT33,0,sizeof(stMsgElemntT33));
    cpss_mem_memset(&stMsgElemntT32,0,sizeof(stMsgElemntT32));
    cpss_mem_memset(&stT33,0,sizeof(stT33));
    cpss_mem_memset(&stT32,0,sizeof(stT32));

    //Result Code(33)
    stT33.ulResultCode = htonl(0);

    ulSuffix = 0;
    stMsgElemntT33.usMsgElemntType = htons(33);
    stMsgElemntT33.usMsgElemntLen = 4;
    cpss_mem_memcpy(&stMsgElemntT33.aucMsgElemntValue[ulSuffix],&stT33,stMsgElemntT33.usMsgElemntLen);
    ulT33Len = sizeof(stMsgElemntT33.usMsgElemntType) + sizeof(stMsgElemntT33.usMsgElemntLen) + stMsgElemntT33.usMsgElemntLen;
    stMsgElemntT33.usMsgElemntLen = htons(stMsgElemntT33.usMsgElemntLen);

    //Radio Operational State(32)
    stT32.ucRadioID = 1;
    stT32.ucState = 1;
    stT32.ucCause = 0;

    ulSuffix = 0;
    stMsgElemntT32.usMsgElemntType = htons(32);
    stMsgElemntT32.usMsgElemntLen = 3;

    cpss_mem_memcpy(&stMsgElemntT32.aucMsgElemntValue[ulSuffix],&stT32,stMsgElemntT32.usMsgElemntLen);
    ulT32Len = sizeof(stMsgElemntT32.usMsgElemntType) + sizeof(stMsgElemntT32.usMsgElemntLen) + stMsgElemntT32.usMsgElemntLen;
    stMsgElemntT32.usMsgElemntLen = htons(stMsgElemntT32.usMsgElemntLen);

//Construct ChangeStateReq Buffer
    ulSuffix = 0;
    cpss_mem_memcpy(&stChangeStateReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT33,ulT33Len);
    ulSuffix = ulSuffix + ulT33Len;
    cpss_mem_memcpy(&stChangeStateReq.aucMsgElemntBuf[ulSuffix],&stMsgElemntT32,ulT32Len);
    ulSuffix = ulSuffix + ulT32Len;

    stChangeStateReq.ucPreamble = (UINT8)0;
    stChangeStateReq.ucHdLen = 16;
    stChangeStateReq.aucHdFlag[0] = 3;
    stChangeStateReq.aucHdFlag[1] = 0;
    stChangeStateReq.usFragID = 0;
    stChangeStateReq.usFragOffset = 0;
    stChangeStateReq.aucMsgTypeEnterPrsNum[0] = (UINT8)0;
    stChangeStateReq.aucMsgTypeEnterPrsNum[1] = (UINT8)0;
    stChangeStateReq.aucMsgTypeEnterPrsNum[2] = (UINT8)0;
    stChangeStateReq.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_CHANGE_STATE_EVENT_REQUEST;
    stChangeStateReq.ucSeqNum = (UINT8)0;
    stChangeStateReq.usMsgElemnetLen = sizeof(stChangeStateReq.usMsgElemnetLen)
                                + (UINT16)(sizeof(stChangeStateReq.ucFlag))
                                + (UINT16)ulT33Len
                                + (UINT16)ulT32Len;

    stChangeStateReq.ucFlag = (UINT8)0;
    ulChangeStateReqLen = 13 + stChangeStateReq.usMsgElemnetLen;
    stChangeStateReq.usMsgElemnetLen = htons(stChangeStateReq.usMsgElemnetLen);

    gstChangeStateReq.ulBufLen = ulChangeStateReqLen;
    cpss_mem_memcpy(&gstChangeStateReq.aucBuff,(UINT8*)&stChangeStateReq,gstChangeStateReq.ulBufLen);
     }

  bSendResult = MT_SendApMsg2AC(vulApIndex,glCpmSockId, gstChangeStateReq.aucBuff,gstChangeStateReq.ulBufLen);

  if (MT_FALSE == bSendResult)
     {
    printf("\nMT_ApSendMsg2CPM send AP(%d) ChangeStateReq failure!\nFile:%s.Line:%d\n",vulApIndex,__FILE__,__LINE__);
     }
  return bSendResult;
}

/*ConfigUpdateRsp消息，AP->AC*/
MTBool ApMutiTestSendConfigUpdateRsp(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;
  CAPWAP_MSG_T stConfigUpdateRsp;
  UINT32 ulConfigUpdateRspLen = 0;
  MSG_ELEMNT_T stMsgElemntT33;//Result Code(33)
  UINT32 ulT33Len = 0;
  UINT32 ulResCode = 0;
  MSG_ELEMNT_T stMsgElemntT32;//Radio Operational State(32)
  UINT32 ulT32Len = 0;
  UINT8 ucRadioId = 0;//Radio ID: Radio id,范围1-31.
  UINT8 ucState = 0;//State: 8-bit为的字段,标识管理状态.有以下枚举值:1 -  Enabled 可用状态; 0 -  Disabled 不可用状态
  UINT8 ucCause = 0;//Cause: 当radio是不可操作的时候,需要上报原因.引起radio不可操作的原因,枚举值如下. 0 -  正常;1 -  radio 失败;2 -  软件失败;3 -  管理员设定操作失败

  UINT32 ulSuffix = 0;
  UINT32 ulMsgElemntSuffix = 0;

  if(MT_FALSE == gbTestMod)
     {
	  cpss_mem_memset(&stConfigUpdateRsp,0,sizeof(stConfigUpdateRsp));
	  cpss_mem_memset(&stMsgElemntT33,0,sizeof(stMsgElemntT33));
	  cpss_mem_memset(&stMsgElemntT32,0,sizeof(stMsgElemntT32));
	  //Result Code(33)
	  stMsgElemntT33.usMsgElemntType = htons(33);
	  stMsgElemntT33.usMsgElemntLen = 4;
	  //Result code的枚举值如下：
	     // 0 成功.
	     //    1 请求信息不完整失败.
             //    2 成功,但是检测到NAT.
      // 3 join失败,没有绑定802.11协议.
      // 4 join失败,资源耗尽.
      // 5 join失败,AP不识别.
      // 6 join失败,不正确的数据格式.
      // 7 Join Failurejoin失败,会话id已经使用.
      // 8 Join Failure join失败,不支持AP的硬件.
      // 9 join失败,绑定协议ac不支持.
      // 10 reset失败,不能reset.
      // 11 reset失败,固件写错误.
      // 12 configuration 失败,请求的配置设定失败.
      // 13 configuration 失败,不支持请求的配置.
      // 14 imagedate错误,校验和错误.
      // 15 image date错误,无效的数据长度.
      // 16 image date错误,未知的错误类型.
      // 17 image date错误,image已经发送.
             //   18 不是期望的消息,当前状态接收到非法消息.
            //    19不是期望消息,当前状态接收的消息无法识别.
            //    20 失败,缺少必要的消息类型标识.
           //    21 失败 不识别的消息类型.
           //    22 数据传输失败.
	  ulResCode = 0;
	  ulResCode = htonl(ulResCode);
	  stMsgElemntT33.aucMsgElemntValue[0] = 1;//Radio ID
	  ulSuffix = 0;
	  cpss_mem_memcpy(&stMsgElemntT33.aucMsgElemntValue[ulSuffix],&ulResCode,sizeof(ulResCode));
	  ulT33Len = sizeof(stMsgElemntT33.usMsgElemntType) + sizeof(stMsgElemntT33.usMsgElemntLen) + stMsgElemntT33.usMsgElemntLen;
	  stMsgElemntT33.usMsgElemntLen = htons(stMsgElemntT33.usMsgElemntLen);

	  cpss_mem_memcpy(&stConfigUpdateRsp.aucMsgElemntBuf[ulMsgElemntSuffix],&stMsgElemntT33,ulT33Len);
	  ulMsgElemntSuffix = ulMsgElemntSuffix + ulT33Len;

	  //Radio Operational State(32)
	  stMsgElemntT32.usMsgElemntType = htons(32);
	  stMsgElemntT32.usMsgElemntLen = 3;
	  ucRadioId = 1;//Radio ID: Radio id,范围1-31.
	  ucState = 1;//State: 8-bit为的字段,标识管理状态.有以下枚举值:1 -  Enabled 可用状态; 0 -  Disabled 不可用状态
	  ucCause = 0;//Cause: 当radio是不可操作的时候,需要上报原因.引起radio不可操作的原因,枚举值如下. 0 -  正常;1 -  radio 失败;2 -  软件失败;3 -  管理员设定操作失败

	  ulSuffix = 0;
	  cpss_mem_memcpy(&stMsgElemntT32.aucMsgElemntValue[ulSuffix],&ucRadioId,sizeof(ucRadioId));
	  ulSuffix = ulSuffix + sizeof(ucRadioId);
	  cpss_mem_memcpy(&stMsgElemntT32.aucMsgElemntValue[ulSuffix],&ucState,sizeof(ucState));
	  ulSuffix = ulSuffix + sizeof(ucState);
	  cpss_mem_memcpy(&stMsgElemntT32.aucMsgElemntValue[ulSuffix],&ucCause,sizeof(ucCause));
	  ulT32Len = sizeof(stMsgElemntT32.usMsgElemntType) + sizeof(stMsgElemntT32.usMsgElemntLen) + stMsgElemntT32.usMsgElemntLen;
	  stMsgElemntT32.usMsgElemntLen = htons(stMsgElemntT32.usMsgElemntLen);

	  cpss_mem_memcpy(&stConfigUpdateRsp.aucMsgElemntBuf[ulMsgElemntSuffix],&stMsgElemntT32,ulT32Len);
	  stConfigUpdateRsp.ucPreamble = (UINT8)0;
	  stConfigUpdateRsp.ucHdLen = 16;
	  stConfigUpdateRsp.aucHdFlag[0] = 3;
	  stConfigUpdateRsp.aucHdFlag[1] = 0;
	  stConfigUpdateRsp.usFragID = 0;
	  stConfigUpdateRsp.usFragOffset = 0;
	  stConfigUpdateRsp.aucMsgTypeEnterPrsNum[0] = (UINT8)0;
	  stConfigUpdateRsp.aucMsgTypeEnterPrsNum[1] = (UINT8)0;
	  stConfigUpdateRsp.aucMsgTypeEnterPrsNum[2] = (UINT8)0;
	  stConfigUpdateRsp.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_CONFIGURE_UPDATE_RESPONSE;
	  stConfigUpdateRsp.ucSeqNum = (UINT8)0;
	  stConfigUpdateRsp.usMsgElemnetLen = sizeof(stConfigUpdateRsp.usMsgElemnetLen)
                                  + (UINT16)(sizeof(stConfigUpdateRsp.ucFlag))
                                  + (UINT16)ulT33Len
                                  + (UINT16)ulT32Len;
	  stConfigUpdateRsp.ucFlag = (UINT8)0;
	  ulConfigUpdateRspLen = 13 + stConfigUpdateRsp.usMsgElemnetLen;
	  stConfigUpdateRsp.usMsgElemnetLen = htons(stConfigUpdateRsp.usMsgElemnetLen);
	  gstConfigurationUpdateRsp.ulBufLen = ulConfigUpdateRspLen;
	  cpss_mem_memcpy(&gstConfigurationUpdateRsp.aucBuff,(UINT8*)&stConfigUpdateRsp,gstConfigurationUpdateRsp.ulBufLen);
     }

  bSendResult = MT_SendApMsg2AC(vulApIndex,glCpmSockId,gstConfigurationUpdateRsp.aucBuff, gstConfigurationUpdateRsp.ulBufLen);

  if (MT_FALSE == bSendResult)
     {
	  printf("\nApMutiTestSendMsg send AP_%d ConfigUpdateRsp failure!\nFile:%s.Line:%d\n",vulApIndex,__FILE__,__LINE__);
	  return bSendResult;
     }
//  else if (RUN_STATE == gastAp[vulApIndex].enState)
//    {
//    if(MT_FALSE == gbTestMod)
//          {
//      cpss_mem_memset(&stConfigUpdateRsp,0,sizeof(stConfigUpdateRsp));
//      cpss_mem_memset(&stMsgElemntT33,0,sizeof(stMsgElemntT33));
//
//      //Result Code(33)
//      stMsgElemntT33.usMsgElemntType = htons(33);
//      stMsgElemntT33.usMsgElemntLen = 4;
////Result code的枚举值如下：
//// 0 成功.
////1 请求信息不完整失败.
////2 成功,但是检测到NAT.
////3 join失败,没有绑定802.11协议.
////4 join失败,资源耗尽.
////5 join失败,AP不识别.
////6 join失败,不正确的数据格式.
////7 Join Failurejoin失败,会话id已经使用.
////8 Join Failure join失败,不支持AP的硬件.
////9 join失败,绑定协议ac不支持.
////10 reset失败,不能reset.
////11 reset失败,固件写错误.
////12 configuration 失败,请求的配置设定失败.
////13 configuration 失败,不支持请求的配置.
////14 imagedate错误,校验和错误.
////15 image date错误,无效的数据长度.
////16 image date错误,未知的错误类型.
////17 image date错误,image已经发送.
////18 不是期望的消息,当前状态接收到非法消息.
////19不是期望消息,当前状态接收的消息无法识别.
////20 失败,缺少必要的消息类型标识.
////21 失败 不识别的消息类型.
////22 数据传输失败.
//      ulResCode = 0;
//      ulResCode = htonl(ulResCode);
//      stMsgElemntT33.aucMsgElemntValue[0] = 1;//Radio ID
//      ulSuffix = 0;
//      cpss_mem_memcpy(&stMsgElemntT33.aucMsgElemntValue[ulSuffix],&ulResCode,sizeof(ulResCode));
//
//      ulT33Len = sizeof(stMsgElemntT33.usMsgElemntType) + sizeof(stMsgElemntT33.usMsgElemntLen) + stMsgElemntT33.usMsgElemntLen;
//      stMsgElemntT33.usMsgElemntLen = htons(stMsgElemntT33.usMsgElemntLen);
//
//      cpss_mem_memcpy(&stConfigUpdateRsp.aucMsgElemntBuf[ulMsgElemntSuffix],&stMsgElemntT33,ulT33Len);
//
//
//      stConfigUpdateRsp.ucPreamble = (UINT8)0;
//      stConfigUpdateRsp.ucHdLen = 16;
//      stConfigUpdateRsp.aucHdFlag[0] = 3;
//      stConfigUpdateRsp.aucHdFlag[1] = 0;
//      stConfigUpdateRsp.usFragID = 0;
//      stConfigUpdateRsp.usFragOffset = 0;
//      stConfigUpdateRsp.aucMsgTypeEnterPrsNum[0] = (UINT8)0;
//      stConfigUpdateRsp.aucMsgTypeEnterPrsNum[1] = (UINT8)0;
//      stConfigUpdateRsp.aucMsgTypeEnterPrsNum[2] = (UINT8)0;
//      stConfigUpdateRsp.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_CONFIGURE_UPDATE_RESPONSE;
//      stConfigUpdateRsp.ucSeqNum = (UINT8)0;
//      stConfigUpdateRsp.usMsgElemnetLen = sizeof(stConfigUpdateRsp.usMsgElemnetLen)
//                                  + (UINT16)(sizeof(stConfigUpdateRsp.ucFlag))
//                                  + (UINT16)ulT33Len;
//
//      stConfigUpdateRsp.ucFlag = (UINT8)0;
//      ulConfigUpdateRspLen = 13 + stConfigUpdateRsp.usMsgElemnetLen;
//      stConfigUpdateRsp.usMsgElemnetLen = htons(stConfigUpdateRsp.usMsgElemnetLen);
//
//      gstConfigUpdateRsp_RunStat.ulBufLen = ulConfigUpdateRspLen;
//      cpss_mem_memcpy(&gstConfigUpdateRsp_RunStat.aucBuff,(UINT8*)&stConfigUpdateRsp,gstConfigUpdateRsp_RunStat.ulBufLen);
//           }
//
//    bSendResult = MT_SendApMsg2AC(vulApIndex,glCpmSockId,gstConfigUpdateRsp_RunStat.aucBuff,gstConfigUpdateRsp_RunStat.ulBufLen);
//
//   if (MT_FALSE == bSendResult)
//        {
//      printf("\nApMutiTestSendMsg send AP_%d ConfigUpdateRsp failure!\nFile:%s.Line:%d\n",vulApIndex,__FILE__,__LINE__);
//      return bSendResult;
//        }
//    }
  return bSendResult;
}

//Add WLAN Rsp AP->AC
MTBool ApMutiTestSendAddWlanRsp(UINT32 vulApIndex,UINT8 vucRadioId,UINT8 vucWlanId)
{
  MTBool bSendResult = MT_FALSE;
  CAPWAP_MSG_T stAddWlanRsp;
  UINT32 ulAddWlanRspLen = 0;
  MSG_ELEMNT_T stMsgElemntT33;//Result Code(33)
  UINT32 ulT33Len = 0;
  MSG_ELEMNT_T stMsgElemntT1026;//IEEE802.11 WTP Assigned BSSID(1026)
  UINT32 ulT1026Len = 0;
  UINT32 ulResCode = 0;
  UINT8 aucBssID[MT_MAC_LENTH] = {0};
  UINT32 ulSuffix = 0;
  UINT32 ulMsgElemntSuffix = 0;

  if (RUN_STATE != gastAp[vulApIndex].enState)
    {
    printf("ERROR.AP State is not WAIT_ADD_WLAN_REQ_STATE.\nFile:%s.Line%d\n",__FILE__, __LINE__);
    return bSendResult;
    }

  MT_GetBssID(vulApIndex,vucRadioId,vucWlanId,aucBssID);

  if(MT_FALSE == gbTestMod)
     {
    cpss_mem_memset(&stAddWlanRsp,0,sizeof(stAddWlanRsp));
    cpss_mem_memset(&stMsgElemntT33,0,sizeof(stMsgElemntT33));

    //Result Code(33)
    stMsgElemntT33.usMsgElemntType = htons(33);
    stMsgElemntT33.usMsgElemntLen = 4;
//Result code的枚举值如下：
// 0 成功.
//1 请求信息不完整失败.
//2 成功,但是检测到NAT.
//3 join失败,没有绑定802.11协议.
//4 join失败,资源耗尽.
//5 join失败,AP不识别.
//6 join失败,不正确的数据格式.
//7 Join Failurejoin失败,会话id已经使用.
//8 Join Failure join失败,不支持AP的硬件.
//9 join失败,绑定协议ac不支持.
//10 reset失败,不能reset.
//11 reset失败,固件写错误.
//12 configuration 失败,请求的配置设定失败.
//13 configuration 失败,不支持请求的配置.
//14 imagedate错误,校验和错误.
//15 image date错误,无效的数据长度.
//16 image date错误,未知的错误类型.
//17 image date错误,image已经发送.
//18 不是期望的消息,当前状态接收到非法消息.
//19不是期望消息,当前状态接收的消息无法识别.
//20 失败,缺少必要的消息类型标识.
//21 失败 不识别的消息类型.
//22 数据传输失败.
    ulResCode = 0;
    ulResCode = htonl(ulResCode);
    stMsgElemntT33.aucMsgElemntValue[0] = 1;//Radio ID
    ulSuffix = 0;
    cpss_mem_memcpy(&stMsgElemntT33.aucMsgElemntValue[ulSuffix],&ulResCode,sizeof(ulResCode));

    ulT33Len = sizeof(stMsgElemntT33.usMsgElemntType) + sizeof(stMsgElemntT33.usMsgElemntLen) + stMsgElemntT33.usMsgElemntLen;
    stMsgElemntT33.usMsgElemntLen = htons(stMsgElemntT33.usMsgElemntLen);

    cpss_mem_memcpy(&stAddWlanRsp.aucMsgElemntBuf[ulMsgElemntSuffix],&stMsgElemntT33,ulT33Len);

    ulMsgElemntSuffix = ulMsgElemntSuffix + ulT33Len;

    //IEEE802.11 WTP Assigned BSSID(1026)
    stMsgElemntT1026.usMsgElemntType = htons(1026);
    stMsgElemntT1026.usMsgElemntLen = 8;
    //BSSID:   AP在收到AC的创建WLAN报文后，创建的WLAN的Mac地址。
    ulSuffix = 0;
    cpss_mem_memcpy(&stMsgElemntT1026.aucMsgElemntValue[ulSuffix],&vucRadioId,sizeof(vucRadioId));
    ulSuffix = ulSuffix + sizeof(vucRadioId);
    cpss_mem_memcpy(&stMsgElemntT1026.aucMsgElemntValue[ulSuffix],&vucWlanId,sizeof(vucWlanId));
    ulSuffix = ulSuffix + sizeof(vucWlanId);
    cpss_mem_memcpy(&stMsgElemntT1026.aucMsgElemntValue[ulSuffix],aucBssID,MT_MAC_LENTH);

    ulT1026Len = sizeof(stMsgElemntT1026.usMsgElemntType) + sizeof(stMsgElemntT1026.usMsgElemntLen) + stMsgElemntT1026.usMsgElemntLen;
    stMsgElemntT1026.usMsgElemntLen = htons(stMsgElemntT1026.usMsgElemntLen);
    cpss_mem_memcpy(&stAddWlanRsp.aucMsgElemntBuf[ulMsgElemntSuffix],&stMsgElemntT1026,ulT1026Len);

    stAddWlanRsp.ucPreamble = (UINT8)0;
    stAddWlanRsp.ucHdLen = 16;
    stAddWlanRsp.aucHdFlag[0] = 3;
    stAddWlanRsp.aucHdFlag[1] = 0;
    stAddWlanRsp.usFragID = 0;
    stAddWlanRsp.usFragOffset = 0;
    stAddWlanRsp.aucMsgTypeEnterPrsNum[0] = 0;
    stAddWlanRsp.aucMsgTypeEnterPrsNum[1] = 0x33;
    stAddWlanRsp.aucMsgTypeEnterPrsNum[2] = 0xdd;
    stAddWlanRsp.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_DISCOVERY_RESPONSE;
    stAddWlanRsp.ucSeqNum = (UINT8)0;
    stAddWlanRsp.usMsgElemnetLen = sizeof(stAddWlanRsp.usMsgElemnetLen)
                                + (UINT16)(sizeof(stAddWlanRsp.ucFlag))
                                + (UINT16)ulT33Len
                                + (UINT16)ulT1026Len;

    stAddWlanRsp.ucFlag = (UINT8)0;
    ulAddWlanRspLen = 13 + stAddWlanRsp.usMsgElemnetLen;
    stAddWlanRsp.usMsgElemnetLen = htons(stAddWlanRsp.usMsgElemnetLen);

    gstAddWlanRsp.ulBufLen = ulAddWlanRspLen;
    cpss_mem_memcpy(&gstAddWlanRsp.aucBuff,(UINT8*)&stAddWlanRsp,gstAddWlanRsp.ulBufLen);
     }
  else
     {
    cpss_mem_memcpy(&gstAddWlanRsp.aucBuff[30],aucBssID,MT_MAC_LENTH);
      }

  bSendResult = MT_SendApMsg2AC(vulApIndex,glCpmSockId,gstAddWlanRsp.aucBuff, gstAddWlanRsp.ulBufLen);

 if (MT_FALSE == bSendResult)
    {
   printf("\nApMutiTestSendMsg send gaucAddWlanRsp failure!\nFile:%s.Line:%d\n",__FILE__,__LINE__);
    }

  return bSendResult;
}

/*CAPWAP升级请求AP->AC*/
MTBool ApMutiTestSendIamgeDataReq(UINT32 ulApIdx)
{
    MTBool bSendResult = MT_FALSE;
    UINT16 usImagDataReqLen = 0;
    UINT16 usImgDataFirstIEType = 0;
    UINT16 usImgDataFirstIELen = 0;
    UINT16 usImgDataSecondIEType = 0;
    UINT16 usImgDataSecondIELen = 0;

    CAPWAP_MSG_T stIamgeDataReq;
    UINT32 ulIamgeDataReqLen = 0;
    MSG_ELEMNT_T stMsgElemntT25;//Image Identifier(25)
    UINT32 ulT25Len = 0;
    MSG_ELEMNT_T stMsgElemntT27;//Initiated Download(27)
    UINT32 ulT27Len = 0;
//    UINT32 ulSuffix = 0;
    UINT32 ulMsgElemntSuffix = 0;

    if(MT_FALSE == gbTestMod)
          {
      cpss_mem_memset(&stIamgeDataReq,0,sizeof(stIamgeDataReq));
      cpss_mem_memset(&stMsgElemntT25,0,sizeof(stMsgElemntT25));
      cpss_mem_memset(&stMsgElemntT27,0,sizeof(stMsgElemntT27));
      //Image Identifier(25)
      stMsgElemntT25.usMsgElemntType = htons(25);
      stMsgElemntT25.usMsgElemntLen = gusApFileNameLen;
      cpss_mem_memcpy(&stMsgElemntT25.aucMsgElemntValue,gaucApUpdateFilename,stMsgElemntT25.usMsgElemntLen);
      ulT25Len = sizeof(stMsgElemntT25.usMsgElemntType) + sizeof(stMsgElemntT25.usMsgElemntLen) + stMsgElemntT25.usMsgElemntLen;
      stMsgElemntT25.usMsgElemntLen = htons(stMsgElemntT25.usMsgElemntLen);

      cpss_mem_memcpy(&stIamgeDataReq.aucMsgElemntBuf[ulMsgElemntSuffix],&stMsgElemntT25,ulT25Len);

      ulMsgElemntSuffix = ulMsgElemntSuffix + ulT25Len;
      //Initiated Download(27)
      stMsgElemntT27.usMsgElemntType = htons(27);
      stMsgElemntT27.usMsgElemntLen = 0;
      cpss_mem_memset(stMsgElemntT27.aucMsgElemntValue,0,stMsgElemntT27.usMsgElemntLen);
      ulT27Len = sizeof(stMsgElemntT27.usMsgElemntType) + sizeof(stMsgElemntT27.usMsgElemntLen) + stMsgElemntT27.usMsgElemntLen;
      stMsgElemntT27.usMsgElemntLen = htons(stMsgElemntT27.usMsgElemntLen);
      cpss_mem_memcpy(&stIamgeDataReq.aucMsgElemntBuf[ulMsgElemntSuffix],&stMsgElemntT27,ulT27Len);

      stIamgeDataReq.ucPreamble = (UINT8)0;
      stIamgeDataReq.ucHdLen = 16;
      stIamgeDataReq.aucHdFlag[0] = 3;
      stIamgeDataReq.aucHdFlag[1] = 0;
      stIamgeDataReq.usFragID = 0;
      stIamgeDataReq.usFragOffset = 0;
      stIamgeDataReq.aucMsgTypeEnterPrsNum[0] = 0;
      stIamgeDataReq.aucMsgTypeEnterPrsNum[1] = 0;
      stIamgeDataReq.aucMsgTypeEnterPrsNum[2] = 0;
      stIamgeDataReq.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_IMAGE_DATA_REQUEST;
      stIamgeDataReq.ucSeqNum = (UINT8)0;
      stIamgeDataReq.usMsgElemnetLen = sizeof(stIamgeDataReq.usMsgElemnetLen)
                                  + (UINT16)(sizeof(stIamgeDataReq.ucFlag))
                                  + (UINT16)ulT25Len
                                  + (UINT16)ulT27Len;

      stIamgeDataReq.ucFlag = (UINT8)0;
      ulIamgeDataReqLen = 13 + stIamgeDataReq.usMsgElemnetLen;
      stIamgeDataReq.usMsgElemnetLen = htons(stIamgeDataReq.usMsgElemnetLen);

      gstCapwapImageReq.ulBufLen = ulIamgeDataReqLen;
      cpss_mem_memcpy(&gstCapwapImageReq.aucBuff,(UINT8*)&stIamgeDataReq,gstCapwapImageReq.ulBufLen);
          }
    else
         {
      usImgDataFirstIEType = 25;
      usImgDataFirstIELen = gusApFileNameLen;
      usImgDataSecondIEType = 27;
      usImgDataSecondIELen = 0;
      usImagDataReqLen = sizeof(usImgDataFirstIEType) + sizeof(usImgDataFirstIELen) + usImgDataFirstIELen
                       + sizeof(usImgDataSecondIEType) + sizeof(usImgDataSecondIELen) + usImgDataSecondIELen
                       + sizeof(usImagDataReqLen) + 1;

      usImgDataFirstIEType = htons(usImgDataFirstIEType);
      usImgDataFirstIELen = htons(usImgDataFirstIELen);
      usImgDataSecondIEType = htons(usImgDataSecondIEType);
      usImgDataSecondIELen = htons(usImgDataSecondIELen);
      usImagDataReqLen = htons(usImagDataReqLen);

      memcpy(&gstCapwapImageReq.aucBuff[13],&usImagDataReqLen,2);
      memcpy(&gstCapwapImageReq.aucBuff[16],&usImgDataFirstIEType,2);//IE25
      memcpy(&gstCapwapImageReq.aucBuff[18],&usImgDataFirstIELen,2);//LEN_IE25
      memcpy(&gstCapwapImageReq.aucBuff[20],gaucApUpdateFilename,gusApFileNameLen);//AP_FileName
      memcpy(&gstCapwapImageReq.aucBuff[20 + gusApFileNameLen],&usImgDataSecondIEType,2);//IE27
      memcpy(&gstCapwapImageReq.aucBuff[20 + gusApFileNameLen + 2],&usImgDataSecondIELen,2);
         }

    bSendResult = MT_SendApMsg2AC(ulApIdx,glCpmSockId, gstCapwapImageReq.aucBuff,gstCapwapImageReq.ulBufLen);

    if (MT_FALSE == bSendResult)
        {
     printf("\nMT_ApSendMsg2CPM send CapwapImageReq failure!File:%s.Line:%d\n",__FILE__,__LINE__);
        }
    return bSendResult;
}

/*CAPWAP升级响应消息AP->AC*/
MTBool ApMutiTestSendIamgeDataRsp(UINT32 ulApIdx)
{
  MTBool bSendResult = MT_FALSE;
  UINT16 usImgdataLen = 0;
  UINT16 usImgdataRspFirstIe = 0;
  UINT16 usImgdataRspFirstIeLen = 0;
  UINT8 aucImgdataRspFirstIeVal[128] = {0};
  UINT16 usImgdataRspSecondIe = 0;
  UINT16 usImgdataRspSecondIeLen = 0;
  UINT8 aucImgdataRspSecondIeVal[16] = {0};
  UINT32 ulLoop = 0;

  CAPWAP_MSG_T stIamgeDataRsp;
  UINT32 ulIamgeDataRspLen = 0;
  MSG_ELEMNT_T stMsgElemntT25;//Image Identifier(25)
  UINT32 ulT25Len = 0;
  MSG_ELEMNT_T stMsgElemntT33;//Result Code(33)
  UINT32 ulT33Len = 0;
  UINT32 ulResCode = 0;
  UINT32 ulSuffix = 0;
  UINT32 ulMsgElemntSuffix = 0;

  if(MT_FALSE == gbTestMod)
    {
    cpss_mem_memset(&stIamgeDataRsp,0,sizeof(stIamgeDataRsp));
    cpss_mem_memset(&stMsgElemntT25,0,sizeof(stMsgElemntT25));
    cpss_mem_memset(&stMsgElemntT33,0,sizeof(stMsgElemntT33));

    //Image Identifier(25)
    stMsgElemntT25.usMsgElemntType = htons(25);
    stMsgElemntT25.usMsgElemntLen = gusApFileNameLen;
    cpss_mem_memcpy(&stMsgElemntT25.aucMsgElemntValue,gaucApUpdateFilename,stMsgElemntT25.usMsgElemntLen);
    ulT25Len = sizeof(stMsgElemntT25.usMsgElemntType) + sizeof(stMsgElemntT25.usMsgElemntLen) + stMsgElemntT25.usMsgElemntLen;
    stMsgElemntT25.usMsgElemntLen = htons(stMsgElemntT25.usMsgElemntLen);

    cpss_mem_memcpy(&stIamgeDataRsp.aucMsgElemntBuf[ulMsgElemntSuffix],&stMsgElemntT25,ulT25Len);

    ulMsgElemntSuffix = ulMsgElemntSuffix + ulT25Len;
    //Result Code(33)
    stMsgElemntT33.usMsgElemntType = htons(33);
    stMsgElemntT33.usMsgElemntLen = 4;
//Result code的枚举值如下：
// 0 成功.
//1 请求信息不完整失败.
//2 成功,但是检测到NAT.
//3 join失败,没有绑定802.11协议.
//4 join失败,资源耗尽.
//5 join失败,AP不识别.
//6 join失败,不正确的数据格式.
//7 Join Failurejoin失败,会话id已经使用.
//8 Join Failure join失败,不支持AP的硬件.
//9 join失败,绑定协议ac不支持.
//10 reset失败,不能reset.
//11 reset失败,固件写错误.
//12 configuration 失败,请求的配置设定失败.
//13 configuration 失败,不支持请求的配置.
//14 imagedate错误,校验和错误.
//15 image date错误,无效的数据长度.
//16 image date错误,未知的错误类型.
//17 image date错误,image已经发送.
//18 不是期望的消息,当前状态接收到非法消息.
//19不是期望消息,当前状态接收的消息无法识别.
//20 失败,缺少必要的消息类型标识.
//21 失败 不识别的消息类型.
//22 数据传输失败.
    ulResCode = 0;
    ulResCode = htonl(ulResCode);
    stMsgElemntT33.aucMsgElemntValue[0] = 1;//Radio ID
    ulSuffix = 0;
    cpss_mem_memcpy(&stMsgElemntT33.aucMsgElemntValue[ulSuffix],&ulResCode,sizeof(ulResCode));

    ulT33Len = sizeof(stMsgElemntT33.usMsgElemntType) + sizeof(stMsgElemntT33.usMsgElemntLen) + stMsgElemntT33.usMsgElemntLen;
    stMsgElemntT33.usMsgElemntLen = htons(stMsgElemntT33.usMsgElemntLen);

    cpss_mem_memcpy(&stIamgeDataRsp.aucMsgElemntBuf[ulMsgElemntSuffix],&stMsgElemntT33,ulT33Len);

    stIamgeDataRsp.ucPreamble = (UINT8)0;
    stIamgeDataRsp.ucHdLen = 16;
    stIamgeDataRsp.aucHdFlag[0] = 3;
    stIamgeDataRsp.aucHdFlag[1] = 0;
    stIamgeDataRsp.usFragID = 0;
    stIamgeDataRsp.usFragOffset = 0;
    stIamgeDataRsp.aucMsgTypeEnterPrsNum[0] = 0;
    stIamgeDataRsp.aucMsgTypeEnterPrsNum[1] = 0;
    stIamgeDataRsp.aucMsgTypeEnterPrsNum[2] = 0;
    stIamgeDataRsp.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_IMAGE_DATA_RESPONSE;
    stIamgeDataRsp.ucSeqNum = (UINT8)0;
    stIamgeDataRsp.usMsgElemnetLen = sizeof(stIamgeDataRsp.usMsgElemnetLen)
                                + (UINT16)(sizeof(stIamgeDataRsp.ucFlag))
                                + (UINT16)ulT25Len
                                + (UINT16)ulT33Len;

    stIamgeDataRsp.ucFlag = (UINT8)0;
    ulIamgeDataRspLen = 13 + stIamgeDataRsp.usMsgElemnetLen;
    stIamgeDataRsp.usMsgElemnetLen = htons(stIamgeDataRsp.usMsgElemnetLen);

    gstCapwapImageRsp.ulBufLen = ulIamgeDataRspLen;
    cpss_mem_memcpy(&gstCapwapImageRsp.aucBuff,(UINT8*)&stIamgeDataRsp,gstCapwapImageRsp.ulBufLen);
    }
  else
    {
    usImgdataRspFirstIe = 25;
    memcpy(&aucImgdataRspFirstIeVal[0],&gaucApUpdateFilename[0],gusApFileNameLen);
    usImgdataRspFirstIeLen = gusApFileNameLen;

    usImgdataRspSecondIe = 33;
    usImgdataRspSecondIeLen = 4;
    for(ulLoop = 0; ulLoop < usImgdataRspSecondIeLen; ulLoop++)
          {
      aucImgdataRspSecondIeVal[ulLoop] = 0;
           }

    usImgdataLen = sizeof(usImgdataRspFirstIe) + sizeof(usImgdataRspFirstIeLen) + gusApFileNameLen
               + sizeof(usImgdataRspSecondIe) + sizeof(usImgdataRspSecondIeLen) + usImgdataRspSecondIeLen
               + sizeof(usImgdataLen)  + 1;

    usImgdataRspFirstIe = htons(usImgdataRspFirstIe);
    usImgdataRspFirstIeLen = htons(usImgdataRspFirstIeLen);
    usImgdataRspSecondIe = htons(usImgdataRspSecondIe);
    usImgdataRspSecondIeLen = htons(usImgdataRspSecondIeLen);
    usImgdataLen = htons(usImgdataLen);


    memcpy(&gstCapwapImageRsp.aucBuff[13],&usImgdataLen,2);
    memcpy(&gstCapwapImageRsp.aucBuff[16],&usImgdataRspFirstIe,2);
    memcpy(&gstCapwapImageRsp.aucBuff[18],&usImgdataRspFirstIeLen,2);
    memcpy(&gstCapwapImageRsp.aucBuff[20],&aucImgdataRspFirstIeVal[0],gusApFileNameLen);
    memcpy(&gstCapwapImageRsp.aucBuff[20 + gusApFileNameLen],&usImgdataRspSecondIe,2);
    memcpy(&gstCapwapImageRsp.aucBuff[20 + gusApFileNameLen + 2],&usImgdataRspSecondIeLen,2);
    memcpy(&gstCapwapImageRsp.aucBuff[20 + gusApFileNameLen + 2 + 2],&aucImgdataRspSecondIeVal[0],4);
    }

  bSendResult = MT_SendApMsg2AC(ulApIdx,glCpmSockId, gstCapwapImageRsp.aucBuff,gstCapwapImageRsp.ulBufLen);

  if (MT_FALSE == bSendResult)
       {
     printf("\nMT_ApSendMsg2CPM send gauCapwapImageRsp failure!\n");
      }
  return bSendResult;
}


MTBool ApMutiTestSendStaConfigRsp(UINT16 vusStaConfigReqIeType,UINT32 vulApIdx,UINT32 vulStaIndex)
{
  MTBool bSendResult = MT_FALSE;
  CAPWAP_MSG_T stStaConfigRsp;
  UINT32 ulStaConfigRspLen = 0;
  MSG_ELEMNT_T stMsgElemntT33;//Result Code(33)
  UINT32 ulT33Len = 0;

  UINT32 ulResCode = 0;
  UINT32 ulSuffix = 0;
  UINT32 ulMsgElemntSuffix = 0;

  switch(vusStaConfigReqIeType)
  {
  case 8://Add STA
  if(MT_FALSE == gbTestMod)
    {
    cpss_mem_memset(&stStaConfigRsp,0,sizeof(stStaConfigRsp));
    cpss_mem_memset(&stMsgElemntT33,0,sizeof(stMsgElemntT33));

    //Result Code(33)
    stMsgElemntT33.usMsgElemntType = htons(33);
    stMsgElemntT33.usMsgElemntLen = 4;
//Result code的枚举值如下：
// 0 成功.
//1 请求信息不完整失败.
//2 成功,但是检测到NAT.
//3 join失败,没有绑定802.11协议.
//4 join失败,资源耗尽.
//5 join失败,AP不识别.
//6 join失败,不正确的数据格式.
//7 Join Failurejoin失败,会话id已经使用.
//8 Join Failure join失败,不支持AP的硬件.
//9 join失败,绑定协议ac不支持.
//10 reset失败,不能reset.
//11 reset失败,固件写错误.
//12 configuration 失败,请求的配置设定失败.
//13 configuration 失败,不支持请求的配置.
//14 imagedate错误,校验和错误.
//15 image date错误,无效的数据长度.
//16 image date错误,未知的错误类型.
//17 image date错误,image已经发送.
//18 不是期望的消息,当前状态接收到非法消息.
//19不是期望消息,当前状态接收的消息无法识别.
//20 失败,缺少必要的消息类型标识.
//21 失败 不识别的消息类型.
//22 数据传输失败.
    ulResCode = 0;
    ulResCode = htonl(ulResCode);
    stMsgElemntT33.aucMsgElemntValue[0] = 1;//Radio ID
    ulSuffix = 0;
    cpss_mem_memcpy(&stMsgElemntT33.aucMsgElemntValue[ulSuffix],&ulResCode,sizeof(ulResCode));

    ulT33Len = sizeof(stMsgElemntT33.usMsgElemntType) + sizeof(stMsgElemntT33.usMsgElemntLen) + stMsgElemntT33.usMsgElemntLen;
    stMsgElemntT33.usMsgElemntLen = htons(stMsgElemntT33.usMsgElemntLen);

    cpss_mem_memcpy(&stStaConfigRsp.aucMsgElemntBuf[ulMsgElemntSuffix],&stMsgElemntT33,ulT33Len);

    stStaConfigRsp.ucPreamble = (UINT8)0;
    stStaConfigRsp.ucHdLen = 16;
    stStaConfigRsp.aucHdFlag[0] = 3;
    stStaConfigRsp.aucHdFlag[1] = 0;
    stStaConfigRsp.usFragID = 0;
    stStaConfigRsp.usFragOffset = 0;
    stStaConfigRsp.aucMsgTypeEnterPrsNum[0] = 0;
    stStaConfigRsp.aucMsgTypeEnterPrsNum[1] = 0;
    stStaConfigRsp.aucMsgTypeEnterPrsNum[2] = 0;
    stStaConfigRsp.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_STATION_CONFIGURATION_RESPONSE;
    stStaConfigRsp.ucSeqNum = (UINT8)0;
    stStaConfigRsp.usMsgElemnetLen = sizeof(stStaConfigRsp.usMsgElemnetLen)
                                + (UINT16)(sizeof(stStaConfigRsp.ucFlag))
                                + (UINT16)ulT33Len;

    stStaConfigRsp.ucFlag = (UINT8)0;
    ulStaConfigRspLen = 13 + stStaConfigRsp.usMsgElemnetLen;
    stStaConfigRsp.usMsgElemnetLen = htons(stStaConfigRsp.usMsgElemnetLen);

    gstStationConfigurationRsp.ulBufLen = ulStaConfigRspLen;
    cpss_mem_memcpy(&gstStationConfigurationRsp.aucBuff,(UINT8*)&stStaConfigRsp,gstStationConfigurationRsp.ulBufLen);
    }

  bSendResult = MT_SendApMsg2AC(vulApIdx,glCpmSockId, gstStationConfigurationRsp.aucBuff, gstStationConfigurationRsp.ulBufLen);

  if (MT_FALSE == bSendResult)
     {
    printf("\nApMutiTestSendMsg send  gaucStaConfigRsp failure!\nFile:%s.Line:%d\n",__FILE__,__LINE__);
     }
   break;

  case 18://Del STA
	  if(MT_FALSE == gbTestMod)
         {
		  cpss_mem_memset(&stStaConfigRsp,0,sizeof(stStaConfigRsp));
		  cpss_mem_memset(&stMsgElemntT33,0,sizeof(stMsgElemntT33));

		  //Result Code(33)
		  stMsgElemntT33.usMsgElemntType = htons(33);
		  stMsgElemntT33.usMsgElemntLen = 4;
    //Result code的枚举值如下：
    // 0 成功.
    //1 请求信息不完整失败.
    //2 成功,但是检测到NAT.
    //3 join失败,没有绑定802.11协议.
    //4 join失败,资源耗尽.
    //5 join失败,AP不识别.
    //6 join失败,不正确的数据格式.
    //7 Join Failurejoin失败,会话id已经使用.
    //8 Join Failure join失败,不支持AP的硬件.
    //9 join失败,绑定协议ac不支持.
    //10 reset失败,不能reset.
    //11 reset失败,固件写错误.
    //12 configuration 失败,请求的配置设定失败.
    //13 configuration 失败,不支持请求的配置.
    //14 imagedate错误,校验和错误.
    //15 image date错误,无效的数据长度.
    //16 image date错误,未知的错误类型.
    //17 image date错误,image已经发送.
    //18 不是期望的消息,当前状态接收到非法消息.
    //19不是期望消息,当前状态接收的消息无法识别.
    //20 失败,缺少必要的消息类型标识.
    //21 失败 不识别的消息类型.
    //22 数据传输失败.
		  ulResCode = 0;
		  ulResCode = htonl(ulResCode);
		  stMsgElemntT33.aucMsgElemntValue[0] = 1;//Radio ID
		  ulSuffix = 0;
		  cpss_mem_memcpy(&stMsgElemntT33.aucMsgElemntValue[ulSuffix],&ulResCode,sizeof(ulResCode));

		  ulT33Len = sizeof(stMsgElemntT33.usMsgElemntType) + sizeof(stMsgElemntT33.usMsgElemntLen) + stMsgElemntT33.usMsgElemntLen;
		  stMsgElemntT33.usMsgElemntLen = htons(stMsgElemntT33.usMsgElemntLen);

		  cpss_mem_memcpy(&stStaConfigRsp.aucMsgElemntBuf[ulMsgElemntSuffix],&stMsgElemntT33,ulT33Len);

		  stStaConfigRsp.ucPreamble = (UINT8)0;
		  stStaConfigRsp.ucHdLen = 16;
		  stStaConfigRsp.aucHdFlag[0] = 3;
		  stStaConfigRsp.aucHdFlag[1] = 0;
		  stStaConfigRsp.usFragID = 0;
		  stStaConfigRsp.usFragOffset = 0;
		  stStaConfigRsp.aucMsgTypeEnterPrsNum[0] = 0;
		  stStaConfigRsp.aucMsgTypeEnterPrsNum[1] = 0;
		  stStaConfigRsp.aucMsgTypeEnterPrsNum[2] = 0;
		  stStaConfigRsp.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_STATION_CONFIGURATION_RESPONSE;
		  stStaConfigRsp.ucSeqNum = (UINT8)0;
		  stStaConfigRsp.usMsgElemnetLen = sizeof(stStaConfigRsp.usMsgElemnetLen)
                                    + (UINT16)(sizeof(stStaConfigRsp.ucFlag))
                                    + (UINT16)ulT33Len;

		  stStaConfigRsp.ucFlag = (UINT8)0;
		  ulStaConfigRspLen = 13 + stStaConfigRsp.usMsgElemnetLen;
		  stStaConfigRsp.usMsgElemnetLen = htons(stStaConfigRsp.usMsgElemnetLen);

		  gstStationConfigurationRsp.ulBufLen = ulStaConfigRspLen;
		  cpss_mem_memcpy(&gstStationConfigurationRsp.aucBuff,(UINT8*)&stStaConfigRsp,gstStationConfigurationRsp.ulBufLen);
         }

	  bSendResult = MT_SendApMsg2AC(vulApIdx,glCpmSockId, gstStationConfigurationRsp.aucBuff, gstStationConfigurationRsp.ulBufLen);

	  if (MT_FALSE == bSendResult)
         {
		  printf("\nApMutiTestSendMsg send  gaucStaConfigRsp failure!\nFile:%s.Line:%d\n",__FILE__,__LINE__);
         }

	  break;

  default:
    printf("\n Recive CW_MSG_TYPE_VALUE_STATION_CONFIGURATION_REQUEST,but unkown IE Type[%d]\n",vusStaConfigReqIeType);
    break;
  }
  return bSendResult;
}

/*Reset响应消息*/
MTBool ApMutiTestSendResetRsp(UINT32 vulApIdx)
{
  MTBool bSendResult = MT_FALSE;
  CAPWAP_MSG_T stResetRsp;
  UINT32 ulResetRspLen = 0;

  if(MT_FALSE == gbTestMod)
     {
    cpss_mem_memset(&stResetRsp,0,sizeof(stResetRsp));

    stResetRsp.ucPreamble = (UINT8)0;
    stResetRsp.ucHdLen = 16;
    stResetRsp.aucHdFlag[0] = 3;
    stResetRsp.aucHdFlag[1] = 0;
    stResetRsp.usFragID = 0;
    stResetRsp.usFragOffset = 0;
    stResetRsp.aucMsgTypeEnterPrsNum[0] = 0;
    stResetRsp.aucMsgTypeEnterPrsNum[1] = 0;
    stResetRsp.aucMsgTypeEnterPrsNum[2] = 0;
    stResetRsp.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_RESET_RESPONSE;
    stResetRsp.ucSeqNum = (UINT8)0;
    stResetRsp.usMsgElemnetLen = sizeof(stResetRsp.usMsgElemnetLen)
                                + (UINT16)(sizeof(stResetRsp.ucFlag));

    stResetRsp.ucFlag = (UINT8)0;
    ulResetRspLen = 13 + stResetRsp.usMsgElemnetLen;
    stResetRsp.usMsgElemnetLen = htons(stResetRsp.usMsgElemnetLen);

    gstResetRsp.ulBufLen = ulResetRspLen;
    cpss_mem_memcpy(&gstResetRsp.aucBuff,(UINT8*)&stResetRsp,gstResetRsp.ulBufLen);
     }

  bSendResult = MT_SendApMsg2AC(vulApIdx,glCpmSockId, gstResetRsp.aucBuff, gstResetRsp.ulBufLen);

  if(MT_FALSE == bSendResult)
    {
    printf("ERROR.ApMutiTestSendResetRsp failure.\nFile:%s.Line:%d\n",__FILE__,__LINE__);
    }
  return bSendResult;
}

MTBool ApMutiTestSendKeepAlive(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;
//  UINT8 aucKpAlive[64] = {0};
  UINT32 ulCapwapHdrFisrt32 = 0;
  UINT32 ulCapwapHdrSecond32 = 0;
  MT_ETH_HDR_T stEthHdr;
  UINT8 aucKeepAlive2Upm[] = {
      0x00, 0x10, 0x00, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x14, 0x00, 0x23, 0x00, 0x10, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
  };

  cpss_mem_memset(&stEthHdr,0,sizeof(stEthHdr));

  if(MT_FALSE == gbTestMod)
     {
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,2);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,0);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,1);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,0);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,0);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,0);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,1);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
	  MT_SetField32(ulCapwapHdrFisrt32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。

	  ulCapwapHdrFisrt32 = htonl(ulCapwapHdrFisrt32);

	  //Fragment ID
	  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAGMENT_ID_START,MT_CAPWAP_HDR_FRAGMENT_ID_LEN,0);//Fragment ID：分片ID，用于分片报文重组。
	  //Fragment Offset
	  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_FRAG_OFFSET_START,MT_CAPWAP_HDR_FRAG_OFFSET_LEN,0);//Fragment Offset：当F位为1是本字段有意义，表示分片报文偏移，类似IP报文分片偏移，本字段乘以8为真实偏移。
	  //Reserved
	  MT_SetField32(ulCapwapHdrSecond32,MT_CAPWAP_HDR_RSVD_START,MT_CAPWAP_HDR_RSVD_LEN,0);//Reserved：保留，必须为0。

	  ulCapwapHdrSecond32 = htonl(ulCapwapHdrSecond32);


    bSendResult = MT_SendStaMsg2AC(vulApIndex,glUpmSockId, aucKeepAlive2Upm, sizeof(aucKeepAlive2Upm));
      }
  else
    {
    bSendResult = MT_SendStaMsg2AC(vulApIndex,glUpmSockId, gstKeepAlive.aucBuff, gstKeepAlive.ulBufLen);
     }


  if(MT_FALSE == bSendResult)
    {
    printf("ERROR.MT_ApSendMsg2UPM for KeepAlive Failure.\nFile:%s.Line:%d\n",__FILE__,__LINE__);
    }
  return bSendResult;
}

MTBool ApMutiTestSendEventReq_DelSta(UINT32 vulApIndex,UINT32 vulStaIndex)
{
  MTBool bSendResult = MT_FALSE;
//  CAPWAP_MSG_T stWtpEventReq;
//  UINT32 ulWtpEventReqLen = 0;
//  MSG_ELEMNT_T stMsgElemntT37;
//  UINT32 ulT37Len = 0;
//  MSG_ELEMNT_T stMsgElemntT135;
//  UINT32 ulT135Len = 0;

  if(MT_FALSE == gbTestMod)
      {
    bSendResult = MT_SendApMsg2AC(vulApIndex, glCpmSockId, gstWtpEventReq_DelSta.aucBuff, gstWtpEventReq_DelSta.ulBufLen);
       }
  else
      {
	  memcpy(&gstWtpEventReq_DelSta.aucBuff[23],gastStaPara[vulStaIndex].auStaMac,MT_MAC_LENTH);
	  bSendResult = MT_SendApMsg2AC(vulApIndex, glCpmSockId, gstWtpEventReq_DelSta.aucBuff, gstWtpEventReq_DelSta.ulBufLen);
      }

  if(MT_FALSE == bSendResult)
     {
    printf("ERROR.MT_ApSendMsg2CPM for WtpEventReq Failure.\nFile:%s.Line:%d\n",__FILE__,__LINE__);
     }

  return bSendResult;
}

UINT8 gucApSrcMac4 = 0;
UINT8 gucApSrcMac5 = 0;

//T37_IE56
MTBool ApMutiTestSendEventReq_IE37_IE56(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;
//  UINT32 ulLoop = 0;
  CAPWAP_MSG_T stWtpEventReq;
  UINT32 ulWtpEventReqLen = 0;
  MSG_ELEMNT_T stMsgElemntT37;
  UINT32 ulMsgElemntT37Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_4;
  UINT32 ulMsgElemntT56_4Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_21;
  UINT32 ulMsgElemntT56_21Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_22;
  UINT32 ulMsgElemntT56_22Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_23;
  UINT32 ulMsgElemntT56_23Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_24;
  UINT32 ulMsgElemntT56_24Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_27;
  UINT32 ulMsgElemntT56_27Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_28;
  UINT32 ulMsgElemntT56_28Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_29;
  UINT32 ulMsgElemntT56_29Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_30;
  UINT32 ulMsgElemntT56_30Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_31;
  UINT32 ulMsgElemntT56_31Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_32;
  UINT32 ulMsgElemntT56_32Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_33;
  UINT32 ulMsgElemntT56_33Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_34;
  UINT32 ulMsgElemntT56_34Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_35;
  UINT32 ulMsgElemntT56_35Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_36;
  UINT32 ulMsgElemntT56_36Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_37;
  UINT32 ulMsgElemntT56_37Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_38;
  UINT32 ulMsgElemntT56_38Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_39;
  UINT32 ulMsgElemntT56_39Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_40;
  UINT32 ulMsgElemntT56_40Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_41;
  UINT32 ulMsgElemntT56_41Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_42;
  UINT32 ulMsgElemntT56_42Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_43;
  UINT32 ulMsgElemntT56_43Len = 0;
  MSG_ELEMNT_T stMsgElemntT56_44;
  UINT32 ulMsgElemntT56_44Len = 0;

  UINT32 ulAlarmOrEventType = 0;

  MT_IE56_21_T stT21;
  MT_IE56_22_T stT22;
  MT_IE56_23_T stT23;
  MT_IE56_24_T stT24;
  MT_IE56_27_T stT27;
  MT_IE56_28_T stT28;
  MT_IE56_29_T stT29;
  MT_IE56_30_T stT30;
  MT_IE56_31_T stT31;
  MT_IE56_32_T stT32;
  MT_IE56_33_T stT33;
  MT_IE56_34_T stT34;
  MT_IE56_35_T stT35;
  MT_IE56_36_T stT36;
  MT_IE56_37_T stT37;
  MT_IE56_38_T stT38;
  MT_IE56_39_T stT39;
  MT_IE56_40_T stT40;
  MT_IE56_41_T stT41;
  MT_IE56_42_T stT42;
  MT_IE56_43_T stT43;
  MT_IE56_44_T stT44;
  UINT32 ulSuffix = 0;
  UINT32 ulNetCarrierInc = htonl(gastApconf[vulApIndex].ulNetCarrierInc);
//  UINT8 *pSsid = "PC_PLAIN_LOCAL";
  UINT32 ulStaIndex = 0;

  cpss_mem_memset(&stWtpEventReq,0,sizeof(stWtpEventReq));
  cpss_mem_memset(&stMsgElemntT37,0,sizeof(stMsgElemntT37));
  cpss_mem_memset(&stMsgElemntT56_4,0,sizeof(stMsgElemntT56_4));
  cpss_mem_memset(&stMsgElemntT56_21,0,sizeof(stMsgElemntT56_21));
  cpss_mem_memset(&stMsgElemntT56_22,0,sizeof(stMsgElemntT56_22));
  cpss_mem_memset(&stMsgElemntT56_23,0,sizeof(stMsgElemntT56_23));
  cpss_mem_memset(&stMsgElemntT56_24,0,sizeof(stMsgElemntT56_24));
  cpss_mem_memset(&stMsgElemntT56_27,0,sizeof(stMsgElemntT56_27));
  cpss_mem_memset(&stMsgElemntT56_28,0,sizeof(stMsgElemntT56_28));
  cpss_mem_memset(&stMsgElemntT56_29,0,sizeof(stMsgElemntT56_29));
  cpss_mem_memset(&stMsgElemntT56_30,0,sizeof(stMsgElemntT56_30));
  cpss_mem_memset(&stMsgElemntT56_31,0,sizeof(stMsgElemntT56_31));
  cpss_mem_memset(&stMsgElemntT56_32,0,sizeof(stMsgElemntT56_32));
  cpss_mem_memset(&stMsgElemntT56_33,0,sizeof(stMsgElemntT56_33));
  cpss_mem_memset(&stMsgElemntT56_34,0,sizeof(stMsgElemntT56_34));
  cpss_mem_memset(&stMsgElemntT56_35,0,sizeof(stMsgElemntT56_35));
  cpss_mem_memset(&stMsgElemntT56_36,0,sizeof(stMsgElemntT56_36));
  cpss_mem_memset(&stMsgElemntT56_37,0,sizeof(stMsgElemntT56_37));
  cpss_mem_memset(&stMsgElemntT56_39,0,sizeof(stMsgElemntT56_38));
  cpss_mem_memset(&stMsgElemntT56_39,0,sizeof(stMsgElemntT56_39));
  cpss_mem_memset(&stMsgElemntT56_40,0,sizeof(stMsgElemntT56_40));
  cpss_mem_memset(&stMsgElemntT56_41,0,sizeof(stMsgElemntT56_41));
  cpss_mem_memset(&stMsgElemntT56_42,0,sizeof(stMsgElemntT56_42));
  cpss_mem_memset(&stMsgElemntT56_43,0,sizeof(stMsgElemntT56_43));
  cpss_mem_memset(&stMsgElemntT56_44,0,sizeof(stMsgElemntT56_44));

  cpss_mem_memset(&stT21,0,sizeof(stT21));
  cpss_mem_memset(&stT22,0,sizeof(stT22));
  cpss_mem_memset(&stT23,0,sizeof(stT23));
  cpss_mem_memset(&stT24,0,sizeof(stT24));
  cpss_mem_memset(&stT27,0,sizeof(stT27));
  cpss_mem_memset(&stT28,0,sizeof(stT28));
  cpss_mem_memset(&stT29,0,sizeof(stT29));
  cpss_mem_memset(&stT30,0,sizeof(stT30));
  cpss_mem_memset(&stT31,0,sizeof(stT31));
  cpss_mem_memset(&stT32,0,sizeof(stT32));
  cpss_mem_memset(&stT33,0,sizeof(stT33));
  cpss_mem_memset(&stT34,0,sizeof(stT34));
  cpss_mem_memset(&stT35,0,sizeof(stT35));
  cpss_mem_memset(&stT36,0,sizeof(stT36));
  cpss_mem_memset(&stT37,0,sizeof(stT37));
  cpss_mem_memset(&stT38,0,sizeof(stT38));
  cpss_mem_memset(&stT39,0,sizeof(stT39));
  cpss_mem_memset(&stT40,0,sizeof(stT40));
  cpss_mem_memset(&stT41,0,sizeof(stT41));
  cpss_mem_memset(&stT42,0,sizeof(stT42));
  cpss_mem_memset(&stT43,0,sizeof(stT43));
  cpss_mem_memset(&stT44,0,sizeof(stT44));

  //4   AC与AP间系统时钟同步失败通告

  //21    CPU利用率过高告警
  stT21.ucCpuUsage = 80;

  //22    CPU利用率过高告警清除
  stT22.ucCpuUsage = 80;

      //23    内存利用率过高告警
  stT23.ucMemUsage = 70;

  //24    内存利用率过高告警清除
  stT24.ucMemUsage = 70;

  //27    AP无线监视工作模式变更通告
  stT27.ucBeforeChgMode = 1;
  stT27.ucAfterChgMode = 2;

  //28    AP软件升级失败通告
  strcpy((char*)stT28.aucBeforeVer,"20130326_1.4.1.1");
  strcpy((char*)stT28.aucAfterVer,"20130401_1.5.1.1");
  strcpy((char*)stT28.aucCause,"not find the file for bug2818");

  //SSID密钥冲突通告29
  stT29.ucRadioId = 1;
  stT29.ucWlanID1 = 1;
  stT29.ucWlanID2 = 2;
  stT29.ucKeyIndex = 2;

  //30  AP冷启动告警
  strcpy((char*)stT30.aucTime,"2013-03-26");

  //31    AP冷启动告警清除
  strcpy((char*)stT31.aucTime,"2013-03-26");

  //32    AP热启动告警
  strcpy((char*)stT32.aucTime,"2013-03-26");

  //33    AP热启动告警清除
  strcpy((char*)stT33.aucTime,"2013-03-26");

  //加密机制改变通告34
  stT34.ucRadioId = 1;
  stT34.usWlanProfileId = htons(2);
  //cpss_mem_memcpy(stT34.aucSSIDName,pSsid,strlen(pSsid));
  strcpy((char*)stT34.aucSSIDName,"IE034_PC_PLAIN_LOCAL");
  MT_GetBssID(vulApIndex,stT34.ucRadioId,1,stT34.aucBSSID);
  stT34.ucAuthModeBefore = rand()%8;
  stT34.ucAuthModeAfter = (rand() + 1)%8;

   //发现无线泛洪攻击通告35
  stT35.aucDeviceMac[0] = 0x56;
  stT35.aucDeviceMac[1] = 0x35;
  stT35.aucDeviceMac[2] = 0x02;
  stT35.aucDeviceMac[3] = 0x03;
  stT35.aucDeviceMac[4] = rand()%255;
  stT35.aucDeviceMac[5] = rand()%255;

#if 1
  for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
     {
    if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState && vulApIndex == getApIDbyBssid(ulStaIndex))
           {
      cpss_mem_memcpy(stT35.aucDeviceMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
      break;
           }
     }
#endif

  stT35.ulAossCount = htonl(rand()%100);
  stT35.ulAuthCount = htonl(rand()%200);
  stT35.ulDeAossCount = htonl(rand()%200);
  stT35.ulDeAuthCount = htonl(rand()%200);

  //发现仿冒攻击通告36
  stT36.aucDeviceMac[0] = 0x56;
  stT36.aucDeviceMac[1] = 0x36;
  stT36.aucDeviceMac[2] = 0x02;
  stT36.aucDeviceMac[3] = 0x03;
  stT36.aucDeviceMac[4] = 0x04;
  stT36.aucDeviceMac[5] = rand()%255;

#if 1
  for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
     {
    if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState && vulApIndex == getApIDbyBssid(ulStaIndex))
           {
      cpss_mem_memcpy(stT36.aucDeviceMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
      break;
           }
     }
#endif

  stT36.ulAossCount = htonl(110);
  stT36.ulAuthCount = htonl(119);
  stT36.ulDeAossCount = htonl(120);
  stT36.ulDeAuthCount = htonl(122);

  //发现Dos攻击通告37
  stT37.aucDeviceMac[0] = 0x56;
  stT37.aucDeviceMac[1] = 0x37;
  stT37.aucDeviceMac[2] = 0x02;
  stT37.aucDeviceMac[3] = 0x03;
  stT37.aucDeviceMac[4] = rand()%255;
  stT37.aucDeviceMac[5] = rand()%255;

  //38 AP温度过高告警
  stT38.ucApTemperatureTrap = 88;

  //39 AP温度过高告警清除
  stT39.ucApTemperatureClearTrap = 88;

  //40 AP软件升级成功通告
  strcpy((char*)stT40.aucBeforeVer,"20130326_1.4.1.1");
  strcpy((char*)stT40.aucAfterVer,"20130401_1.5.1.1");
  strcpy((char*)stT40.aucCause,"Bug Modified for 2818");

  //41  用户上线信息通告
  time((time_t*)&stT41.ulUserOnlineTime);
  stT41.ulUserOnlineTime = htonl(stT41.ulUserOnlineTime);
  stT41.sTxPower = htons(20);
  stT41.usCurrentChannel = htons(11);

  for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
     {
	  if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState && vulApIndex == getApIDbyBssid(ulStaIndex))
	     {
		  cpss_mem_memcpy(stT41.aucStaMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
		  break;
	     }
     }

  strcpy((char*)stT41.aucSSID,"IE056_41_PC_PLAIN_LOCAL");

  //42 用户下线信息通告
  time((time_t*)&stT42.ulUserOfflineTime);
  stT42.ulUserOfflineTime = htonl(stT42.ulUserOfflineTime);
  stT42.sTxPower = htons(30);
  stT42.usCurrentChannel = htons(11);

  for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
     {
	  if(AUTH_INIT_STATE == gastStaPara[ulStaIndex].enAuthState && vulApIndex == getApIDbyBssid(ulStaIndex))
	     {
		  cpss_mem_memcpy(stT42.aucStaMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
		  break;
	     }
     }

  strcpy((char*)stT42.aucSSID,"IE056_42_PC_PLAIN_LOCAL");

  //发现弱初始化向量攻击检测通告43
  stT43.aucDeviceMac[0] = 0x56;
  stT43.aucDeviceMac[1] = 0x43;
  stT43.aucDeviceMac[2] = 0x02;
  stT43.aucDeviceMac[3] = 0x03;
  stT43.aucDeviceMac[4] = rand()%255;
  stT43.aucDeviceMac[5] = rand()%255;
  stT43.ulWeakIVDetecteCount = htonl(rand());

  //AP缓存写失败通告
  strcpy((char*)stT44.aucCause,"IE56_44AP缓存写失败通告");

  //AP_ALARM（IE=56）,TYPE = 4
  ulSuffix = 0;
  stMsgElemntT56_4.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(4);
  cpss_mem_memcpy(&stMsgElemntT56_4.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
//  cpss_mem_memcpy(&stMsgElemntT56_4.aucMsgElemntValue[ulSuffix],&stTxx,sizeof(stTxx));
//  ulSuffix = ulSuffix + sizeof(stTxx);
  stMsgElemntT56_4.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_4Len = sizeof(stMsgElemntT56_4.usMsgElemntType) + sizeof(stMsgElemntT56_4.usMsgElemntLen) + stMsgElemntT56_4.usMsgElemntLen;
  stMsgElemntT56_4.usMsgElemntLen = htons(stMsgElemntT56_4.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 21
  ulSuffix = 0;
  stMsgElemntT56_21.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(21);
  cpss_mem_memcpy(&stMsgElemntT56_21.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_21.aucMsgElemntValue[ulSuffix],&stT21,sizeof(stT21));
  ulSuffix = ulSuffix + sizeof(stT21);
  stMsgElemntT56_21.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_21Len = sizeof(stMsgElemntT56_21.usMsgElemntType) + sizeof(stMsgElemntT56_21.usMsgElemntLen) + stMsgElemntT56_21.usMsgElemntLen;
  stMsgElemntT56_21.usMsgElemntLen = htons(stMsgElemntT56_21.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 22
  ulSuffix = 0;
  stMsgElemntT56_22.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(22);
  cpss_mem_memcpy(&stMsgElemntT56_22.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_22.aucMsgElemntValue[ulSuffix],&stT22,sizeof(stT22));
  ulSuffix = ulSuffix + sizeof(stT22);
  stMsgElemntT56_22.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_22Len = sizeof(stMsgElemntT56_22.usMsgElemntType) + sizeof(stMsgElemntT56_22.usMsgElemntLen) + stMsgElemntT56_22.usMsgElemntLen;
  stMsgElemntT56_22.usMsgElemntLen = htons(stMsgElemntT56_22.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 23
  ulSuffix = 0;
  stMsgElemntT56_23.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(23);
  cpss_mem_memcpy(&stMsgElemntT56_23.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_23.aucMsgElemntValue[ulSuffix],&stT23,sizeof(stT23));
  ulSuffix = ulSuffix + sizeof(stT23);
  stMsgElemntT56_23.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_23Len = sizeof(stMsgElemntT56_23.usMsgElemntType) + sizeof(stMsgElemntT56_23.usMsgElemntLen) + stMsgElemntT56_23.usMsgElemntLen;
  stMsgElemntT56_23.usMsgElemntLen = htons(stMsgElemntT56_23.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 24
  ulSuffix = 0;
  stMsgElemntT56_24.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(24);
  cpss_mem_memcpy(&stMsgElemntT56_24.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_24.aucMsgElemntValue[ulSuffix],&stT24,sizeof(stT24));
  ulSuffix = ulSuffix + sizeof(stT24);
  stMsgElemntT56_24.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_24Len = sizeof(stMsgElemntT56_24.usMsgElemntType) + sizeof(stMsgElemntT56_24.usMsgElemntLen) + stMsgElemntT56_24.usMsgElemntLen;
  stMsgElemntT56_24.usMsgElemntLen = htons(stMsgElemntT56_24.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 27
  ulSuffix = 0;
  stMsgElemntT56_27.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(27);
  cpss_mem_memcpy(&stMsgElemntT56_27.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_27.aucMsgElemntValue[ulSuffix],&stT27,sizeof(stT27));
  ulSuffix = ulSuffix + sizeof(stT27);
  stMsgElemntT56_27.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_27Len = sizeof(stMsgElemntT56_27.usMsgElemntType) + sizeof(stMsgElemntT56_27.usMsgElemntLen) + stMsgElemntT56_27.usMsgElemntLen;
  stMsgElemntT56_27.usMsgElemntLen = htons(stMsgElemntT56_27.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 28
  ulSuffix = 0;
  stMsgElemntT56_28.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(28);
  cpss_mem_memcpy(&stMsgElemntT56_28.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_28.aucMsgElemntValue[ulSuffix],&stT28,sizeof(stT28));
  ulSuffix = ulSuffix + sizeof(stT28);
  stMsgElemntT56_28.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_28Len = sizeof(stMsgElemntT56_28.usMsgElemntType) + sizeof(stMsgElemntT56_28.usMsgElemntLen) + stMsgElemntT56_28.usMsgElemntLen;
  stMsgElemntT56_28.usMsgElemntLen = htons(stMsgElemntT56_28.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 29
  ulSuffix = 0;
  stMsgElemntT56_29.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(29);
  cpss_mem_memcpy(&stMsgElemntT56_29.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_29.aucMsgElemntValue[ulSuffix],&stT29,sizeof(stT29));
  ulSuffix = ulSuffix + sizeof(stT29);
  stMsgElemntT56_29.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_29Len = sizeof(stMsgElemntT56_29.usMsgElemntType) + sizeof(stMsgElemntT56_29.usMsgElemntLen) + stMsgElemntT56_29.usMsgElemntLen;
  stMsgElemntT56_29.usMsgElemntLen = htons(stMsgElemntT56_29.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 30
  ulSuffix = 0;
  stMsgElemntT56_30.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(30);
  cpss_mem_memcpy(&stMsgElemntT56_30.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_30.aucMsgElemntValue[ulSuffix],&stT30,sizeof(stT30));
  ulSuffix = ulSuffix + sizeof(stT30);
  stMsgElemntT56_30.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_30Len = sizeof(stMsgElemntT56_30.usMsgElemntType) + sizeof(stMsgElemntT56_30.usMsgElemntLen) + stMsgElemntT56_30.usMsgElemntLen;
  stMsgElemntT56_30.usMsgElemntLen = htons(stMsgElemntT56_30.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 31
  ulSuffix = 0;
  stMsgElemntT56_31.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(31);
  cpss_mem_memcpy(&stMsgElemntT56_31.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_31.aucMsgElemntValue[ulSuffix],&stT31,sizeof(stT31));
  ulSuffix = ulSuffix + sizeof(stT31);
  stMsgElemntT56_31.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_31Len = sizeof(stMsgElemntT56_31.usMsgElemntType) + sizeof(stMsgElemntT56_31.usMsgElemntLen) + stMsgElemntT56_31.usMsgElemntLen;
  stMsgElemntT56_31.usMsgElemntLen = htons(stMsgElemntT56_31.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 32
  ulSuffix = 0;
  stMsgElemntT56_32.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(32);
  cpss_mem_memcpy(&stMsgElemntT56_32.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_32.aucMsgElemntValue[ulSuffix],&stT32,sizeof(stT32));
  ulSuffix = ulSuffix + sizeof(stT32);
  stMsgElemntT56_32.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_32Len = sizeof(stMsgElemntT56_32.usMsgElemntType) + sizeof(stMsgElemntT56_32.usMsgElemntLen) + stMsgElemntT56_32.usMsgElemntLen;
  stMsgElemntT56_32.usMsgElemntLen = htons(stMsgElemntT56_32.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 33
  ulSuffix = 0;
  stMsgElemntT56_33.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(33);
  cpss_mem_memcpy(&stMsgElemntT56_33.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_33.aucMsgElemntValue[ulSuffix],&stT33,sizeof(stT33));
  ulSuffix = ulSuffix + sizeof(stT33);
  stMsgElemntT56_33.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_33Len = sizeof(stMsgElemntT56_33.usMsgElemntType) + sizeof(stMsgElemntT56_33.usMsgElemntLen) + stMsgElemntT56_33.usMsgElemntLen;
  stMsgElemntT56_33.usMsgElemntLen = htons(stMsgElemntT56_33.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 34
  ulSuffix = 0;
  stMsgElemntT56_34.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(34);
  cpss_mem_memcpy(&stMsgElemntT56_34.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_34.aucMsgElemntValue[ulSuffix],&stT34,sizeof(stT34));
  ulSuffix = ulSuffix + sizeof(stT34);
  stMsgElemntT56_34.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_34Len = sizeof(stMsgElemntT56_34.usMsgElemntType) + sizeof(stMsgElemntT56_34.usMsgElemntLen) + stMsgElemntT56_34.usMsgElemntLen;
  stMsgElemntT56_34.usMsgElemntLen = htons(stMsgElemntT56_34.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 35
  ulSuffix = 0;
  stMsgElemntT56_35.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(35);
  cpss_mem_memcpy(&stMsgElemntT56_35.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_35.aucMsgElemntValue[ulSuffix],&stT35,sizeof(stT35));
  ulSuffix = ulSuffix + sizeof(stT35);
  stMsgElemntT56_35.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_35Len = sizeof(stMsgElemntT56_35.usMsgElemntType) + sizeof(stMsgElemntT56_35.usMsgElemntLen) + stMsgElemntT56_35.usMsgElemntLen;
  stMsgElemntT56_35.usMsgElemntLen = htons(stMsgElemntT56_35.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 36
  ulSuffix = 0;
  stMsgElemntT56_36.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(36);
  cpss_mem_memcpy(&stMsgElemntT56_36.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_36.aucMsgElemntValue[ulSuffix],&stT36,sizeof(stT36));
  ulSuffix = ulSuffix + sizeof(stT36);
  stMsgElemntT56_36.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_36Len = sizeof(stMsgElemntT56_36.usMsgElemntType) + sizeof(stMsgElemntT56_36.usMsgElemntLen) + stMsgElemntT56_36.usMsgElemntLen;
  stMsgElemntT56_36.usMsgElemntLen = htons(stMsgElemntT56_36.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 37
  ulSuffix = 0;
  stMsgElemntT56_37.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(37);
  cpss_mem_memcpy(&stMsgElemntT56_37.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_37.aucMsgElemntValue[ulSuffix],&stT37,sizeof(stT37));
  ulSuffix = ulSuffix + sizeof(stT37);
  stMsgElemntT56_37.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_37Len = sizeof(stMsgElemntT56_37.usMsgElemntType) + sizeof(stMsgElemntT56_37.usMsgElemntLen) + stMsgElemntT56_37.usMsgElemntLen;
  stMsgElemntT56_37.usMsgElemntLen = htons(stMsgElemntT56_37.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 38
  ulSuffix = 0;
  stMsgElemntT56_38.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(38);
  cpss_mem_memcpy(&stMsgElemntT56_38.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_38.aucMsgElemntValue[ulSuffix],&stT38,sizeof(stT38));
  ulSuffix = ulSuffix + sizeof(stT38);
  stMsgElemntT56_38.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_38Len = sizeof(stMsgElemntT56_38.usMsgElemntType) + sizeof(stMsgElemntT56_38.usMsgElemntLen) + stMsgElemntT56_38.usMsgElemntLen;
  stMsgElemntT56_38.usMsgElemntLen = htons(stMsgElemntT56_38.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 39
  ulSuffix = 0;
  stMsgElemntT56_39.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(39);
  cpss_mem_memcpy(&stMsgElemntT56_39.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_39.aucMsgElemntValue[ulSuffix],&stT39,sizeof(stT39));
  ulSuffix = ulSuffix + sizeof(stT39);
  stMsgElemntT56_39.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_39Len = sizeof(stMsgElemntT56_39.usMsgElemntType) + sizeof(stMsgElemntT56_39.usMsgElemntLen) + stMsgElemntT56_39.usMsgElemntLen;
  stMsgElemntT56_39.usMsgElemntLen = htons(stMsgElemntT56_39.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 40
  ulSuffix = 0;
  stMsgElemntT56_40.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(40);
  cpss_mem_memcpy(&stMsgElemntT56_40.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_40.aucMsgElemntValue[ulSuffix],&stT40,sizeof(stT40));
  ulSuffix = ulSuffix + sizeof(stT40);
  stMsgElemntT56_40.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_40Len = sizeof(stMsgElemntT56_40.usMsgElemntType) + sizeof(stMsgElemntT56_40.usMsgElemntLen) + stMsgElemntT56_40.usMsgElemntLen;
  stMsgElemntT56_40.usMsgElemntLen = htons(stMsgElemntT56_40.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 41
  ulSuffix = 0;
  stMsgElemntT56_41.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(41);
  cpss_mem_memcpy(&stMsgElemntT56_41.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_41.aucMsgElemntValue[ulSuffix],&stT41,sizeof(stT41));
  ulSuffix = ulSuffix + sizeof(stT41);
  stMsgElemntT56_41.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_41Len = sizeof(stMsgElemntT56_41.usMsgElemntType) + sizeof(stMsgElemntT56_41.usMsgElemntLen) + stMsgElemntT56_41.usMsgElemntLen;
  stMsgElemntT56_41.usMsgElemntLen = htons(stMsgElemntT56_41.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 42
  ulSuffix = 0;
  stMsgElemntT56_42.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(42);
  cpss_mem_memcpy(&stMsgElemntT56_42.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_42.aucMsgElemntValue[ulSuffix],&stT42,sizeof(stT42));
  ulSuffix = ulSuffix + sizeof(stT42);
  stMsgElemntT56_42.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_42Len = sizeof(stMsgElemntT56_42.usMsgElemntType) + sizeof(stMsgElemntT56_42.usMsgElemntLen) + stMsgElemntT56_42.usMsgElemntLen;
  stMsgElemntT56_42.usMsgElemntLen = htons(stMsgElemntT56_42.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 43
  ulSuffix = 0;
  stMsgElemntT56_43.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(43);
  cpss_mem_memcpy(&stMsgElemntT56_43.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56_43.aucMsgElemntValue[ulSuffix],&stT43,sizeof(stT43));
  ulSuffix = ulSuffix + sizeof(stT43);
  stMsgElemntT56_43.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_43Len = sizeof(stMsgElemntT56_43.usMsgElemntType) + sizeof(stMsgElemntT56_43.usMsgElemntLen) + stMsgElemntT56_43.usMsgElemntLen;
  stMsgElemntT56_43.usMsgElemntLen = htons(stMsgElemntT56_43.usMsgElemntLen);

  //AP_ALARM（IE=56）,TYPE = 44
  ulSuffix = 0;
  stMsgElemntT56_44.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(44);
  cpss_mem_memcpy(&stMsgElemntT56_44.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT56_44.aucMsgElemntValue[ulSuffix],&stT44,sizeof(stT44));
  ulSuffix = ulSuffix + sizeof(stT44);
  stMsgElemntT56_44.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56_44Len = sizeof(stMsgElemntT56_44.usMsgElemntType) + sizeof(stMsgElemntT56_44.usMsgElemntLen) + stMsgElemntT56_44.usMsgElemntLen;
  stMsgElemntT56_44.usMsgElemntLen = htons(stMsgElemntT56_44.usMsgElemntLen);

  //MsgElemntT37
  stMsgElemntT37.usMsgElemntType = htons(CW_MSG_ELEMENT_VENDOR_SPEC_PAYLOAD_CW_TYPE);
  ulSuffix = 0;
  cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
  ulSuffix = ulSuffix + sizeof(ulNetCarrierInc);

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5604)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_4,ulMsgElemntT56_4Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_4Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5621)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_21,ulMsgElemntT56_21Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_21Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5622)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_22,ulMsgElemntT56_22Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_22Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5623)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_23,ulMsgElemntT56_23Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_23Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5624)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_24,ulMsgElemntT56_24Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_24Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5627)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_27,ulMsgElemntT56_27Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_27Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5628)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_28,ulMsgElemntT56_28Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_28Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5629)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_29,ulMsgElemntT56_29Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_29Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5630)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_30,ulMsgElemntT56_30Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_30Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5631)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_31,ulMsgElemntT56_31Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_31Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5632)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_32,ulMsgElemntT56_32Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_32Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5633)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_33,ulMsgElemntT56_33Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_33Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5634)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_34,ulMsgElemntT56_34Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_34Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5635)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_35,ulMsgElemntT56_35Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_35Len;
     }


  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5636)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_36,ulMsgElemntT56_36Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_36Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5637)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_37,ulMsgElemntT56_37Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_37Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5638)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_38,ulMsgElemntT56_38Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_38Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5639)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_39,ulMsgElemntT56_39Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_39Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5640)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_40,ulMsgElemntT56_40Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_40Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5641)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_41,ulMsgElemntT56_41Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_41Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5642)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_42,ulMsgElemntT56_42Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_42Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5643)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_43,ulMsgElemntT56_43Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_43Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5644)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56_44,ulMsgElemntT56_44Len);
    ulSuffix = ulSuffix + ulMsgElemntT56_44Len;
     }

  stMsgElemntT37.usMsgElemntLen = ulSuffix;
  ulMsgElemntT37Len = sizeof(stMsgElemntT37.usMsgElemntType) + sizeof(stMsgElemntT37.usMsgElemntLen) + stMsgElemntT37.usMsgElemntLen;
  stMsgElemntT37.usMsgElemntLen = htons(stMsgElemntT37.usMsgElemntLen);

  //WtpEventReq
  stWtpEventReq.ucPreamble = (UINT8)0;
  stWtpEventReq.ucHdLen = 16;
  stWtpEventReq.aucHdFlag[0] = 3;
  stWtpEventReq.aucHdFlag[1] = 0;
  stWtpEventReq.usFragID = 0;
  stWtpEventReq.usFragOffset = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[0] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[1] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[2] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_WTP_EVENT_REQUEST;
  stWtpEventReq.ucSeqNum = (UINT8)0;
  stWtpEventReq.usMsgElemnetLen = sizeof(stWtpEventReq.usMsgElemnetLen)
                              + (UINT16)(sizeof(stWtpEventReq.ucFlag))
                              + ulMsgElemntT37Len;

  stWtpEventReq.ucFlag = (UINT8)0;

  cpss_mem_memcpy(stWtpEventReq.aucMsgElemntBuf,&stMsgElemntT37,ulMsgElemntT37Len);

  ulWtpEventReqLen = 13 + stWtpEventReq.usMsgElemnetLen;
  stWtpEventReq.usMsgElemnetLen = htons(stWtpEventReq.usMsgElemnetLen);

  gstWtpEventReq.ulBufLen = ulWtpEventReqLen;
  cpss_mem_memcpy(gstWtpEventReq.aucBuff,(UINT8*)&stWtpEventReq,gstWtpEventReq.ulBufLen);

  bSendResult = MT_SendApMsg2AC(vulApIndex, glCpmSockId, gstWtpEventReq.aucBuff, gstWtpEventReq.ulBufLen);

  return bSendResult;
}

//T37_IE56_TYPE36
MTBool ApMutiTestSendEventReq_IE37_IE56_TYPE36(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;
//  UINT32 ulLoop = 0;
  CAPWAP_MSG_T stWtpEventReq;
  UINT32 ulWtpEventReqLen = 0;
  MSG_ELEMNT_T stMsgElemntT37;
  UINT32 ulMsgElemntT37Len = 0;
  MSG_ELEMNT_T stMsgElemntT56;
  UINT32 ulMsgElemntT56Len = 0;
  UINT32 ulAlarmOrEventType = 0;
  MT_IE56_36_T stT36;
  UINT32 ulSuffix = 0;
  UINT32 ulNetCarrierInc = htonl(gastApconf[vulApIndex].ulNetCarrierInc);

  cpss_mem_memset(&stWtpEventReq,0,sizeof(stWtpEventReq));
  cpss_mem_memset(&stMsgElemntT37,0,sizeof(stMsgElemntT37));
  cpss_mem_memset(&stMsgElemntT56,0,sizeof(stMsgElemntT56));
  cpss_mem_memset(&stT36,0,sizeof(stT36));


  //MT_T36_T
  stT36.aucDeviceMac[0] = 0x30;
  stT36.aucDeviceMac[1] = 0x31;
  stT36.aucDeviceMac[2] = 0x32;
  stT36.aucDeviceMac[3] = 0x33;
  stT36.aucDeviceMac[4] = 0x34;
  stT36.aucDeviceMac[5] = rand()%255;

  stT36.ulAossCount = htonl(110);
  stT36.ulAuthCount = htonl(119);
  stT36.ulDeAossCount = htonl(120);
  stT36.ulDeAuthCount = htonl(122);

  //AP_ALARM（IE=56）
  ulSuffix = 0;
  stMsgElemntT56.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(36);//发现仿冒攻击通告
  cpss_mem_memcpy(&stMsgElemntT56.aucMsgElemntValue[ulSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  ulSuffix = ulSuffix + sizeof(ulAlarmOrEventType);
  cpss_mem_memcpy(&stMsgElemntT56.aucMsgElemntValue[ulSuffix],&stT36,sizeof(stT36));
  ulSuffix = ulSuffix + sizeof(stT36);

  stMsgElemntT56.usMsgElemntLen = ulSuffix;
  ulMsgElemntT56Len = sizeof(stMsgElemntT56.usMsgElemntType) + sizeof(stMsgElemntT56.usMsgElemntLen) + stMsgElemntT56.usMsgElemntLen;
  stMsgElemntT56.usMsgElemntLen = htons(stMsgElemntT56.usMsgElemntLen);

  //MsgElemntT37
  stMsgElemntT37.usMsgElemntType = htons(CW_MSG_ELEMENT_VENDOR_SPEC_PAYLOAD_CW_TYPE);
  ulSuffix = 0;
  cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
  ulSuffix = ulSuffix + sizeof(ulNetCarrierInc);
  cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[ulSuffix],&stMsgElemntT56,ulMsgElemntT56Len);
  ulSuffix = ulSuffix + ulMsgElemntT56Len;
  stMsgElemntT37.usMsgElemntLen = ulSuffix;
  ulMsgElemntT37Len = sizeof(stMsgElemntT37.usMsgElemntType) + sizeof(stMsgElemntT37.usMsgElemntLen) + stMsgElemntT37.usMsgElemntLen;
  stMsgElemntT37.usMsgElemntLen = htons(stMsgElemntT37.usMsgElemntLen);

  //WtpEventReq
  stWtpEventReq.ucPreamble = (UINT8)0;
  stWtpEventReq.ucHdLen = 16;
  stWtpEventReq.aucHdFlag[0] = 3;
  stWtpEventReq.aucHdFlag[1] = 0;
  stWtpEventReq.usFragID = 0;
  stWtpEventReq.usFragOffset = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[0] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[1] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[2] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_WTP_EVENT_REQUEST;
  stWtpEventReq.ucSeqNum = (UINT8)0;
  stWtpEventReq.usMsgElemnetLen = sizeof(stWtpEventReq.usMsgElemnetLen)
                              + (UINT16)(sizeof(stWtpEventReq.ucFlag))
                              + ulMsgElemntT37Len;

  stWtpEventReq.ucFlag = (UINT8)0;

  cpss_mem_memcpy(stWtpEventReq.aucMsgElemntBuf,&stMsgElemntT37,ulMsgElemntT37Len);

  ulWtpEventReqLen = 13 + stWtpEventReq.usMsgElemnetLen;
  stWtpEventReq.usMsgElemnetLen = htons(stWtpEventReq.usMsgElemnetLen);

  gstWtpEventReq.ulBufLen = ulWtpEventReqLen;
  cpss_mem_memcpy(gstWtpEventReq.aucBuff,(UINT8*)&stWtpEventReq,gstWtpEventReq.ulBufLen);

  bSendResult = MT_SendApMsg2AC(vulApIndex, glCpmSockId, gstWtpEventReq.aucBuff, gstWtpEventReq.ulBufLen);

  return bSendResult;
}

//信道扫描结果
MTBool ApMutiTestSendEventReq_IE37_IE57(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;
  UINT32 ulLoop = 0;
  CAPWAP_MSG_T stWtpEventReq;
  UINT32 ulWtpEventReqLen = 0;
  MSG_ELEMNT_T stMsgElemntT37;
  UINT32 ulMsgElemntT37Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_21;
  UINT32 ulMsgElemntT57_21Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_22;
  UINT32 ulMsgElemntT57_22Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_23;
  UINT32 ulMsgElemntT57_23Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_24;
  UINT32 ulMsgElemntT57_24Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_25;
  UINT32 ulMsgElemntT57_25Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_26;
  UINT32 ulMsgElemntT57_26Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_27;
  UINT32 ulMsgElemntT57_27Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_28;
  UINT32 ulMsgElemntT57_28Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_29;
  UINT32 ulMsgElemntT57_29Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_30;
  UINT32 ulMsgElemntT57_30Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_31;
  UINT32 ulMsgElemntT57_31Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_32;
  UINT32 ulMsgElemntT57_32Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_33;
  UINT32 ulMsgElemntT57_33Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_34;
  UINT32 ulMsgElemntT57_34Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_35;
  UINT32 ulMsgElemntT57_35Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_36;
  UINT32 ulMsgElemntT57_36Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_37;
  UINT32 ulMsgElemntT57_37Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_38;
  UINT32 ulMsgElemntT57_38Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_39;
  UINT32 ulMsgElemntT57_39Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_40;
  UINT32 ulMsgElemntT57_40Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_41;
  UINT32 ulMsgElemntT57_41Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_42;
  UINT32 ulMsgElemntT57_42Len = 0;
  MSG_ELEMNT_T stMsgElemntT57_43;
  UINT32 ulMsgElemntT57_43Len = 0;

  MT_IE57_21_T stT21;
  MT_IE57_22_T stT22;
  MT_IE57_23_T stT23;
  MT_IE57_24_T stT24;
  MT_IE57_25_T stT25;
  MT_IE57_26_T stT26;
  MT_IE57_27_T stT27;
  MT_IE57_28_T stT28;
  MT_IE57_29_T stT29;
  MT_IE57_30_T stT30;
  MT_IE57_31_T stT31;
  MT_IE57_32_T stT32;
  MT_IE57_33_T stT33;
  MT_IE57_34_T stT34;
  UINT32 ulST34LEN = 0;
  MT_IE57_35_T stT35;
  MT_IE57_36_T stT36;
  MT_IE57_37_T stT37;
  MT_IE57_38_T stT38;
  MT_IE57_39_T stT39;
  MT_IE57_40_T stT40;
  MT_IE57_41_T stT41;
  MT_IE57_42_T stT42;
  MT_IE57_43_T stT43;
  UINT32 ulStaIndex = 0;
  UINT16 usMsgElemntSuffix = 0;
//  UINT8 *pSsid = "IE057_PC_PLAIN_LOCAL";
//  UINT8 *pTime = "2013-02-26";
  UINT32 ulAlarmOrEventType = 0;
  UINT32 ulNetCarrierInc = htonl(gastApconf[vulApIndex].ulNetCarrierInc);

  cpss_mem_memset(&stWtpEventReq,0,sizeof(stWtpEventReq));
  cpss_mem_memset(&stMsgElemntT37,0,sizeof(stMsgElemntT37));

  cpss_mem_memset(&stMsgElemntT57_21,0,sizeof(stMsgElemntT57_21));
  cpss_mem_memset(&stMsgElemntT57_22,0,sizeof(stMsgElemntT57_22));
  cpss_mem_memset(&stMsgElemntT57_23,0,sizeof(stMsgElemntT57_23));
  cpss_mem_memset(&stMsgElemntT57_24,0,sizeof(stMsgElemntT57_24));
  cpss_mem_memset(&stMsgElemntT57_25,0,sizeof(stMsgElemntT57_25));
  cpss_mem_memset(&stMsgElemntT57_26,0,sizeof(stMsgElemntT57_26));
  cpss_mem_memset(&stMsgElemntT57_27,0,sizeof(stMsgElemntT57_27));
  cpss_mem_memset(&stMsgElemntT57_28,0,sizeof(stMsgElemntT57_28));
  cpss_mem_memset(&stMsgElemntT57_29,0,sizeof(stMsgElemntT57_29));
  cpss_mem_memset(&stMsgElemntT57_30,0,sizeof(stMsgElemntT57_30));
  cpss_mem_memset(&stMsgElemntT57_31,0,sizeof(stMsgElemntT57_31));
  cpss_mem_memset(&stMsgElemntT57_32,0,sizeof(stMsgElemntT57_32));
  cpss_mem_memset(&stMsgElemntT57_33,0,sizeof(stMsgElemntT57_33));
  cpss_mem_memset(&stMsgElemntT57_34,0,sizeof(stMsgElemntT57_34));
  cpss_mem_memset(&stMsgElemntT57_35,0,sizeof(stMsgElemntT57_35));
  cpss_mem_memset(&stMsgElemntT57_36,0,sizeof(stMsgElemntT57_36));
  cpss_mem_memset(&stMsgElemntT57_37,0,sizeof(stMsgElemntT57_37));
  cpss_mem_memset(&stMsgElemntT57_38,0,sizeof(stMsgElemntT57_38));
  cpss_mem_memset(&stMsgElemntT57_39,0,sizeof(stMsgElemntT57_39));
  cpss_mem_memset(&stMsgElemntT57_40,0,sizeof(stMsgElemntT57_40));
  cpss_mem_memset(&stMsgElemntT57_41,0,sizeof(stMsgElemntT57_41));
  cpss_mem_memset(&stMsgElemntT57_42,0,sizeof(stMsgElemntT57_42));
  cpss_mem_memset(&stMsgElemntT57_43,0,sizeof(stMsgElemntT57_43));

  cpss_mem_memset(&stT21,0,sizeof(stT21));
  cpss_mem_memset(&stT22,0,sizeof(stT22));
  cpss_mem_memset(&stT23,0,sizeof(stT23));
  cpss_mem_memset(&stT24,0,sizeof(stT24));
  cpss_mem_memset(&stT25,0,sizeof(stT25));
  cpss_mem_memset(&stT26,0,sizeof(stT26));
  cpss_mem_memset(&stT27,0,sizeof(stT27));
  cpss_mem_memset(&stT28,0,sizeof(stT28));
  cpss_mem_memset(&stT29,0,sizeof(stT29));
  cpss_mem_memset(&stT30,0,sizeof(stT30));
  cpss_mem_memset(&stT31,0,sizeof(stT31));
  cpss_mem_memset(&stT32,0,sizeof(stT32));
  cpss_mem_memset(&stT33,0,sizeof(stT33));
  cpss_mem_memset(&stT34,0,sizeof(stT34));
  cpss_mem_memset(&stT35,0,sizeof(stT35));
  cpss_mem_memset(&stT36,0,sizeof(stT36));
  cpss_mem_memset(&stT37,0,sizeof(stT37));
  cpss_mem_memset(&stT38,0,sizeof(stT38));
  cpss_mem_memset(&stT39,0,sizeof(stT39));
  cpss_mem_memset(&stT40,0,sizeof(stT40));
  cpss_mem_memset(&stT41,0,sizeof(stT41));
  cpss_mem_memset(&stT42,0,sizeof(stT42));
  cpss_mem_memset(&stT43,0,sizeof(stT43));

  //21  AP同频干扰告警
  stT21.ucRadioId = 1;
  stT21.usCurrentChannel = htons(13);
  MT_GetBssID(vulApIndex,stT21.ucRadioId,1,stT21.aucInterferBssid);

  //22    AP同频干扰告警清除
  stT22.ucRadioId = 1;
  stT22.usCurrentChannel = htons(11);
  MT_GetBssID(vulApIndex,stT22.ucRadioId,1,stT22.aucInterferBssid);

  //23    AP邻频干扰告警
  stT23.ucRadioId = 1;
  stT23.usCurrentChannel = htons(11);
  stT23.usInterferChannel = htons(8);
  MT_GetBssID(vulApIndex,stT23.ucRadioId,1,stT23.aucInterferBssid);

  //24    AP邻频干扰告警清除
  stT24.ucRadioId = 1;
  stT24.usCurrentChannel = htons(11);
  stT24.usInterferChannel = htons(10);
  MT_GetBssID(vulApIndex,stT24.ucRadioId,1,stT24.aucInterferBssid);

  //25    终端干扰告警
  stT25.ucRadioId = 1;
  stT25.usCurrentChannel = htons(11);
  stT25.aucStaMac[0] = 0x57;
  stT25.aucStaMac[1] = 0x25;
  stT25.aucStaMac[2] = 0x52;
  stT25.aucStaMac[3] = 0x53;
  stT25.aucStaMac[4] = 0x54;
  stT25.aucStaMac[5] = 0x55;

#if 1
  for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
         {
    if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState && vulApIndex == getApIDbyBssid(ulStaIndex))
           {
      cpss_mem_memcpy(stT25.aucStaMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
      break;
           }
         }

#endif

  //26    终端干扰告警清除
  stT26.ucRadioId = 1;
  stT26.usCurrentChannel = htons(11);
  stT26.aucStaMac[0] = 0x57;
  stT26.aucStaMac[1] = 0x26;
  stT26.aucStaMac[2] = 0x52;
  stT26.aucStaMac[3] = 0x53;
  stT26.aucStaMac[4] = 0x54;
  stT26.aucStaMac[5] = 0x55;

#if 1
  for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
     {
    if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState && vulApIndex == getApIDbyBssid(ulStaIndex))
           {
      cpss_mem_memcpy(stT26.aucStaMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
      break;
           }
     }
#endif

  //27    其他设备干扰告警
  stT27.ucRadioId = 1;
  stT27.usCurrentChannel = htons(11);

  //28    其他设备干扰告警清除
  stT28.ucRadioId = 1;
  stT28.usCurrentChannel = htons(9);

  //29    无线链路中断告警
  stT29.ucRadioId = 1;
  stT29.ucReason = 1 + rand() % 5;

  //30    无线链路中断告警清除
  stT30.ucRadioId = 1;
  stT30.ucReason = 1 + rand() % 5;

  //31    AP无法增加新的关联用户告警
  stT31.ulApMaxStaNum = htonl(64);
  strcpy((char*)stT31.aucCause,"Current users 65 overflow.");

  //32    AP无法增加新的关联用户告警清除
  stT32.ulApMaxStaNum = htonl(64);
  strcpy((char*)stT32.aucCause,"Current users 65 overflow.");

  //33    无线信道变更通告
  stT33.ucRadioId = 1;
  stT33.usBeforeChgChannel = htons(11);
  stT33.usAfterChgChannel = htons(6);
  stT33.ucChgMode = 1 + rand() % 2;
//  strcpy(stT33.aucApSerial,gastApconf[vulApIndex].acuSerialNo);
  stT33.ucAlrmCnt = 33;

      //34    发现可疑设备通告
  stT34.ucApNum = 5;
  ulST34LEN = 4 + stT34.ucApNum * sizeof(MT_DUBIOUS_AP_INFO_T);

  for(ulLoop = 0; ulLoop < stT34.ucApNum; ulLoop++)
     {
    stT34.astApInfo[ulLoop].aucApMac[0] = 0x20;
    stT34.astApInfo[ulLoop].aucApMac[1] = 0x21;
    stT34.astApInfo[ulLoop].aucApMac[2] = 0x22;
    stT34.astApInfo[ulLoop].aucApMac[3] = 0x23;
    stT34.astApInfo[ulLoop].aucApMac[4] = 0x24;
    stT34.astApInfo[ulLoop].aucApMac[5] = 0x25;
#if 1
    if(254 == gucApSrcMac5)
           {
      gucApSrcMac4++;
      gucApSrcMac5 = 0;
           }
    stT34.astApInfo[ulLoop].aucApMac[4] = gucApSrcMac4;
    stT34.astApInfo[ulLoop].aucApMac[5] = gucApSrcMac5;
#endif
#if 1
    if(254 == gucApSrcMac5)
           {
      gucApSrcMac4++;
      gucApSrcMac5 = 0;
           }
#endif
    stT34.astApInfo[ulLoop].aucBssid[0] = 0x11;
    stT34.astApInfo[ulLoop].aucBssid[1] = 0x22;
    stT34.astApInfo[ulLoop].aucBssid[2] = 0x33;
    stT34.astApInfo[ulLoop].aucBssid[3] = 0x44;
    stT34.astApInfo[ulLoop].aucBssid[4] = gucApSrcMac4++;
    stT34.astApInfo[ulLoop].aucBssid[5] = gucApSrcMac5++;

    //cpss_mem_memcpy(stT34.astApInfo[ulLoop].aucSSID,pSsid,strlen(pSsid));
    strcpy((char*)stT34.astApInfo[ulLoop].aucSSID,"IE05734_PC_PLAIN_LOCAL");
//    stT34.astApInfo[ulLoop].aucUserStatAPReceivedStaSNR[0] = 0x31;
//    stT34.astApInfo[ulLoop].aucUserStatAPReceivedStaSNR[1] = 0x32 + ulLoop;
    stT34.astApInfo[ulLoop].ucUserStatAPReceivedStaSNR = 57;
    stT34.astApInfo[ulLoop].sTxPower = htons(20);
    stT34.astApInfo[ulLoop].usCurrentChannel = htons(rand()%200);
//    cpss_mem_memcpy(stT34.astApInfo[ulLoop].aucTime,pTime,strlen(pTime));
//    stT34.astApInfo[ulLoop].ulTime = htonl(20130503);
    time((time_t*)&stT34.astApInfo[ulLoop].ulTime);
    stT34.astApInfo[ulLoop].ulTime = htonl(stT34.astApInfo[ulLoop].ulTime);
     }

  //35    终端鉴权失败通告
  stT35.ucRadioId = 1;
  stT35.ucWlanId = 1;
  stT35.ucAuthMode = 3;
  stT35.aucStaMac[0] = 0x35;
  stT35.aucStaMac[1] = 0x11;
  stT35.aucStaMac[2] = 0x12;
  stT35.aucStaMac[3] = 0x13;
  stT35.aucStaMac[4] = 0x14;
  stT35.aucStaMac[5] = 0x15;
#if 1
  for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
         {
    if(ASSOCIATION_SUCESS_STATE != gastStaPara[ulStaIndex].enAuthState && vulApIndex == getApIDbyBssid(ulStaIndex))
           {
      cpss_mem_memcpy(stT35.aucStaMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
      break;
           }
         }
#endif

  MT_GetBssID(vulApIndex,stT35.ucRadioId,stT35.ucWlanId,stT35.aucBSSID);
  strcpy((char*)stT35.aucCause,"终端鉴权失败");
  strcpy((char*)stT35.aucSSIDName,"IE05735_PC_OPEN_LOCAL");

  //36    终端关联失败通告
  stT36.ucRadioId = 1;
  stT36.ucWlanId = 1;
  stT36.aucStaMac[0] = 0x36;
  stT36.aucStaMac[1] = 0x11;
  stT36.aucStaMac[2] = 0x12;
  stT36.aucStaMac[3] = 0x13;
  stT36.aucStaMac[4] = 0x14;
  stT36.aucStaMac[5] = 0x15;
#if 1
  for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
     {
    if(ASSOCIATION_SUCESS_STATE != gastStaPara[ulStaIndex].enAuthState && vulApIndex == getApIDbyBssid(ulStaIndex))
           {
      cpss_mem_memcpy(stT36.aucStaMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
      break;
           }
     }
#endif

  MT_GetBssID(vulApIndex,stT36.ucRadioId,stT36.ucWlanId,stT36.aucBSSID);
  strcpy((char*)stT36.aucCause,"终端关联失败");
  strcpy((char*)stT36.aucSSIDName,"IE05736_PC_OPEN_LOCAL");

  //37    ARP报文超速告警
  stT37.ucRadioId = 1;
  stT37.ucWlanId = 1;

  stT37.aucStaMac[0] = 0x37;
  stT37.aucStaMac[1] = 0x11;
  stT37.aucStaMac[2] = 0x12;
  stT37.aucStaMac[3] = 0x13;
  stT37.aucStaMac[4] = 0x14;
  stT37.aucStaMac[5] = 0x15;
#if 1
  for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
     {
    if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState && vulApIndex == getApIDbyBssid(ulStaIndex))
           {
      cpss_mem_memcpy(stT37.aucStaMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
      break;
           }
     }
#endif

  //38    ARP报文超速告警清除
  stT38.ucRadioId = 1;
  stT38.ucWlanId = 1;

  stT38.aucStaMac[0] = 0x38;
  stT38.aucStaMac[1] = 0x11;
  stT38.aucStaMac[2] = 0x12;
  stT38.aucStaMac[3] = 0x13;
  stT38.aucStaMac[4] = 0x14;
  stT38.aucStaMac[5] = 0x15;
#if 1
  for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
     {
    if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState && vulApIndex == getApIDbyBssid(ulStaIndex))
           {
      cpss_mem_memcpy(stT38.aucStaMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
      break;
           }
     }
#endif

  //39 AP射频单元启动工作通告
  stT39.ucRadioId = 1;
  stT39.ucReason = 1 + rand() % 2;

  //40 AP射频单元停止工作通告
  stT40.ucRadioId = 1;
  stT40.ucReason = 1 + rand() % 2;
  //strcpy(stT40.aucCause,"IE57_40AP射频单元停止工作通告");

  //41 AP配置失败通告
  stT41.ucChgType = 1 + rand() % 3;
  stT41.ucRadioId = 1;
  stT41.ucWlanId = 1;
  stT41.ucReason = 1 + rand() % 5;

      //42 链路认证失败
  stT42.ucRadioId = 1;
  stT42.ucWlanId = 1;
  strcpy((char*)stT42.aucSSIDName,"IE05742_PC_PLAIN_LOCAL");
  MT_GetBssID(vulApIndex,stT42.ucRadioId,stT42.ucWlanId,stT42.aucBSSID);

  stT42.aucStaMac[0] = 0x57;
  stT42.aucStaMac[1] = 0x42;
  stT42.aucStaMac[2] = 0x02;
  stT42.aucStaMac[3] = 0x03;
  stT42.aucStaMac[4] = 0x04;
  stT42.aucStaMac[5] = 0x05;

#if 1
  for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
     {
    if(ASSOCIATION_SUCESS_STATE != gastStaPara[ulStaIndex].enAuthState && vulApIndex == getApIDbyBssid(ulStaIndex))
           {
      cpss_mem_memcpy(stT42.aucStaMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
      break;
           }
     }
#endif

  stT42.ucAuthtype = 1 + rand() % 7;
  strcpy((char*)stT42.aucCause,"Auth type does not match.");

  //43 AP配置成功通告
  stT43.ucChgType = 1 + rand() % 3;
  stT43.ucRadioId = 1;
  stT43.ucWlanId = 1;
  stT43.ucReason = 0;

  //IE57_21
  usMsgElemntSuffix = 0;
  stMsgElemntT57_21.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(21);
  cpss_mem_memcpy(&stMsgElemntT57_21.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_21.aucMsgElemntValue[usMsgElemntSuffix],&stT21,sizeof(MT_IE57_21_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_21_T);
  stMsgElemntT57_21.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_21Len = sizeof(stMsgElemntT57_21.usMsgElemntType) + sizeof(stMsgElemntT57_21.usMsgElemntLen) + stMsgElemntT57_21.usMsgElemntLen;
  stMsgElemntT57_21.usMsgElemntLen = htons(stMsgElemntT57_21.usMsgElemntLen);

  //IE57_22
  usMsgElemntSuffix = 0;
  stMsgElemntT57_22.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(22);
  cpss_mem_memcpy(&stMsgElemntT57_22.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_22.aucMsgElemntValue[usMsgElemntSuffix],&stT22,sizeof(MT_IE57_22_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_22_T);
  stMsgElemntT57_22.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_22Len = sizeof(stMsgElemntT57_22.usMsgElemntType) + sizeof(stMsgElemntT57_22.usMsgElemntLen) + stMsgElemntT57_22.usMsgElemntLen;
  stMsgElemntT57_22.usMsgElemntLen = htons(stMsgElemntT57_22.usMsgElemntLen);

  //IE57_23
  usMsgElemntSuffix = 0;
  stMsgElemntT57_23.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(23);
  cpss_mem_memcpy(&stMsgElemntT57_23.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_23.aucMsgElemntValue[usMsgElemntSuffix],&stT23,sizeof(MT_IE57_23_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_23_T);
  stMsgElemntT57_23.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_23Len = sizeof(stMsgElemntT57_23.usMsgElemntType) + sizeof(stMsgElemntT57_23.usMsgElemntLen) + stMsgElemntT57_23.usMsgElemntLen;
  stMsgElemntT57_23.usMsgElemntLen = htons(stMsgElemntT57_23.usMsgElemntLen);

  //IE57_24
  usMsgElemntSuffix = 0;
  stMsgElemntT57_24.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(24);
  cpss_mem_memcpy(&stMsgElemntT57_24.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_24.aucMsgElemntValue[usMsgElemntSuffix],&stT24,sizeof(MT_IE57_24_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_24_T);
  stMsgElemntT57_24.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_24Len = sizeof(stMsgElemntT57_24.usMsgElemntType) + sizeof(stMsgElemntT57_24.usMsgElemntLen) + stMsgElemntT57_24.usMsgElemntLen;
  stMsgElemntT57_24.usMsgElemntLen = htons(stMsgElemntT57_24.usMsgElemntLen);

  //IE57_25
  usMsgElemntSuffix = 0;
  stMsgElemntT57_25.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(25);
  cpss_mem_memcpy(&stMsgElemntT57_25.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_25.aucMsgElemntValue[usMsgElemntSuffix],&stT25,sizeof(MT_IE57_25_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_25_T);
  stMsgElemntT57_25.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_25Len = sizeof(stMsgElemntT57_25.usMsgElemntType) + sizeof(stMsgElemntT57_25.usMsgElemntLen) + stMsgElemntT57_25.usMsgElemntLen;
  stMsgElemntT57_25.usMsgElemntLen = htons(stMsgElemntT57_25.usMsgElemntLen);

  //IE57_26
  usMsgElemntSuffix = 0;
  stMsgElemntT57_26.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(26);
  cpss_mem_memcpy(&stMsgElemntT57_26.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_26.aucMsgElemntValue[usMsgElemntSuffix],&stT26,sizeof(MT_IE57_26_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_26_T);
  stMsgElemntT57_26.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_26Len = sizeof(stMsgElemntT57_26.usMsgElemntType) + sizeof(stMsgElemntT57_26.usMsgElemntLen) + stMsgElemntT57_26.usMsgElemntLen;
  stMsgElemntT57_26.usMsgElemntLen = htons(stMsgElemntT57_26.usMsgElemntLen);

  //IE57_27
  usMsgElemntSuffix = 0;
  stMsgElemntT57_27.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(27);
  cpss_mem_memcpy(&stMsgElemntT57_27.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_27.aucMsgElemntValue[usMsgElemntSuffix],&stT27,sizeof(MT_IE57_27_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_27_T);
  stMsgElemntT57_27.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_27Len = sizeof(stMsgElemntT57_27.usMsgElemntType) + sizeof(stMsgElemntT57_27.usMsgElemntLen) + stMsgElemntT57_27.usMsgElemntLen;
  stMsgElemntT57_27.usMsgElemntLen = htons(stMsgElemntT57_27.usMsgElemntLen);

  //IE57_28
  usMsgElemntSuffix = 0;
  stMsgElemntT57_28.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(28);
  cpss_mem_memcpy(&stMsgElemntT57_28.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_28.aucMsgElemntValue[usMsgElemntSuffix],&stT28,sizeof(MT_IE57_28_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_28_T);
  stMsgElemntT57_28.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_28Len = sizeof(stMsgElemntT57_28.usMsgElemntType) + sizeof(stMsgElemntT57_28.usMsgElemntLen) + stMsgElemntT57_28.usMsgElemntLen;
  stMsgElemntT57_28.usMsgElemntLen = htons(stMsgElemntT57_28.usMsgElemntLen);

  //IE57_29
  usMsgElemntSuffix = 0;
  stMsgElemntT57_29.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(29);
  cpss_mem_memcpy(&stMsgElemntT57_29.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_29.aucMsgElemntValue[usMsgElemntSuffix],&stT29,sizeof(MT_IE57_29_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_29_T);
  stMsgElemntT57_29.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_29Len = sizeof(stMsgElemntT57_29.usMsgElemntType) + sizeof(stMsgElemntT57_29.usMsgElemntLen) + stMsgElemntT57_29.usMsgElemntLen;
  stMsgElemntT57_29.usMsgElemntLen = htons(stMsgElemntT57_29.usMsgElemntLen);

  //IE57_30
  usMsgElemntSuffix = 0;
  stMsgElemntT57_30.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(30);
  cpss_mem_memcpy(&stMsgElemntT57_30.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_30.aucMsgElemntValue[usMsgElemntSuffix],&stT30,sizeof(MT_IE57_30_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_30_T);
  stMsgElemntT57_30.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_30Len = sizeof(stMsgElemntT57_30.usMsgElemntType) + sizeof(stMsgElemntT57_30.usMsgElemntLen) + stMsgElemntT57_30.usMsgElemntLen;
  stMsgElemntT57_30.usMsgElemntLen = htons(stMsgElemntT57_30.usMsgElemntLen);

  //IE57_31
  usMsgElemntSuffix = 0;
  stMsgElemntT57_31.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(31);
  cpss_mem_memcpy(&stMsgElemntT57_31.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_31.aucMsgElemntValue[usMsgElemntSuffix],&stT31,sizeof(MT_IE57_31_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_31_T);
  stMsgElemntT57_31.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_31Len = sizeof(stMsgElemntT57_31.usMsgElemntType) + sizeof(stMsgElemntT57_31.usMsgElemntLen) + stMsgElemntT57_31.usMsgElemntLen;
  stMsgElemntT57_31.usMsgElemntLen = htons(stMsgElemntT57_31.usMsgElemntLen);

  //IE57_32
  usMsgElemntSuffix = 0;
  stMsgElemntT57_32.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(32);
  cpss_mem_memcpy(&stMsgElemntT57_32.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_32.aucMsgElemntValue[usMsgElemntSuffix],&stT32,sizeof(MT_IE57_32_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_32_T);
  stMsgElemntT57_32.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_32Len = sizeof(stMsgElemntT57_32.usMsgElemntType) + sizeof(stMsgElemntT57_32.usMsgElemntLen) + stMsgElemntT57_32.usMsgElemntLen;
  stMsgElemntT57_32.usMsgElemntLen = htons(stMsgElemntT57_32.usMsgElemntLen);

  //IE57_33
  usMsgElemntSuffix = 0;
  stMsgElemntT57_33.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(33);
  cpss_mem_memcpy(&stMsgElemntT57_33.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_33.aucMsgElemntValue[usMsgElemntSuffix],&stT33,sizeof(MT_IE57_33_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_33_T);
  stMsgElemntT57_33.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_33Len = sizeof(stMsgElemntT57_33.usMsgElemntType) + sizeof(stMsgElemntT57_33.usMsgElemntLen) + stMsgElemntT57_33.usMsgElemntLen;
  stMsgElemntT57_33.usMsgElemntLen = htons(stMsgElemntT57_33.usMsgElemntLen);

  //IE57_34
  usMsgElemntSuffix = 0;
  stMsgElemntT57_34.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(34);
  cpss_mem_memcpy(&stMsgElemntT57_34.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_34.aucMsgElemntValue[usMsgElemntSuffix],&stT34,ulST34LEN);
  usMsgElemntSuffix = usMsgElemntSuffix + ulST34LEN;
  stMsgElemntT57_34.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_34Len = sizeof(stMsgElemntT57_34.usMsgElemntType) + sizeof(stMsgElemntT57_34.usMsgElemntLen) + stMsgElemntT57_34.usMsgElemntLen;
  stMsgElemntT57_34.usMsgElemntLen = htons(stMsgElemntT57_34.usMsgElemntLen);

  //IE57_35
  usMsgElemntSuffix = 0;
  stMsgElemntT57_35.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(35);
  cpss_mem_memcpy(&stMsgElemntT57_35.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_35.aucMsgElemntValue[usMsgElemntSuffix],&stT35,sizeof(MT_IE57_35_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_35_T);
  stMsgElemntT57_35.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_35Len = sizeof(stMsgElemntT57_35.usMsgElemntType) + sizeof(stMsgElemntT57_35.usMsgElemntLen) + stMsgElemntT57_35.usMsgElemntLen;
  stMsgElemntT57_35.usMsgElemntLen = htons(stMsgElemntT57_35.usMsgElemntLen);

  //IE57_36
  usMsgElemntSuffix = 0;
  stMsgElemntT57_36.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(36);
  cpss_mem_memcpy(&stMsgElemntT57_36.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_36.aucMsgElemntValue[usMsgElemntSuffix],&stT36,sizeof(MT_IE57_36_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_36_T);
  stMsgElemntT57_36.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_36Len = sizeof(stMsgElemntT57_36.usMsgElemntType) + sizeof(stMsgElemntT57_36.usMsgElemntLen) + stMsgElemntT57_36.usMsgElemntLen;
  stMsgElemntT57_36.usMsgElemntLen = htons(stMsgElemntT57_36.usMsgElemntLen);

  //IE57_37
  usMsgElemntSuffix = 0;
  stMsgElemntT57_37.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(37);
  cpss_mem_memcpy(&stMsgElemntT57_37.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_37.aucMsgElemntValue[usMsgElemntSuffix],&stT37,sizeof(MT_IE57_37_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_37_T);
  stMsgElemntT57_37.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_37Len = sizeof(stMsgElemntT57_37.usMsgElemntType) + sizeof(stMsgElemntT57_37.usMsgElemntLen) + stMsgElemntT57_37.usMsgElemntLen;
  stMsgElemntT57_37.usMsgElemntLen = htons(stMsgElemntT57_37.usMsgElemntLen);

  //IE57_38
  usMsgElemntSuffix = 0;
  stMsgElemntT57_38.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(38);
  cpss_mem_memcpy(&stMsgElemntT57_38.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_38.aucMsgElemntValue[usMsgElemntSuffix],&stT38,sizeof(MT_IE57_38_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_38_T);
  stMsgElemntT57_38.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_38Len = sizeof(stMsgElemntT57_38.usMsgElemntType) + sizeof(stMsgElemntT57_38.usMsgElemntLen) + stMsgElemntT57_38.usMsgElemntLen;
  stMsgElemntT57_38.usMsgElemntLen = htons(stMsgElemntT57_38.usMsgElemntLen);

  //IE57_39
  usMsgElemntSuffix = 0;
  stMsgElemntT57_39.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(39);
  cpss_mem_memcpy(&stMsgElemntT57_39.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_39.aucMsgElemntValue[usMsgElemntSuffix],&stT39,sizeof(MT_IE57_39_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_39_T);
  stMsgElemntT57_39.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_39Len = sizeof(stMsgElemntT57_39.usMsgElemntType) + sizeof(stMsgElemntT57_39.usMsgElemntLen) + stMsgElemntT57_39.usMsgElemntLen;
  stMsgElemntT57_39.usMsgElemntLen = htons(stMsgElemntT57_39.usMsgElemntLen);

  //IE57_40
  usMsgElemntSuffix = 0;
  stMsgElemntT57_40.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(40);
  cpss_mem_memcpy(&stMsgElemntT57_40.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_40.aucMsgElemntValue[usMsgElemntSuffix],&stT40,sizeof(MT_IE57_40_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_40_T);
  stMsgElemntT57_40.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_40Len = sizeof(stMsgElemntT57_40.usMsgElemntType) + sizeof(stMsgElemntT57_40.usMsgElemntLen) + stMsgElemntT57_40.usMsgElemntLen;
  stMsgElemntT57_40.usMsgElemntLen = htons(stMsgElemntT57_40.usMsgElemntLen);

  //IE57_41
  usMsgElemntSuffix = 0;
  stMsgElemntT57_41.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(41);
  cpss_mem_memcpy(&stMsgElemntT57_41.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_41.aucMsgElemntValue[usMsgElemntSuffix],&stT41,sizeof(MT_IE57_41_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_41_T);
  stMsgElemntT57_41.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_41Len = sizeof(stMsgElemntT57_41.usMsgElemntType) + sizeof(stMsgElemntT57_41.usMsgElemntLen) + stMsgElemntT57_41.usMsgElemntLen;
  stMsgElemntT57_41.usMsgElemntLen = htons(stMsgElemntT57_41.usMsgElemntLen);

  //IE57_42
  usMsgElemntSuffix = 0;
  stMsgElemntT57_42.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(42);
  cpss_mem_memcpy(&stMsgElemntT57_42.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_42.aucMsgElemntValue[usMsgElemntSuffix],&stT42,sizeof(MT_IE57_42_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_42_T);
  stMsgElemntT57_42.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_42Len = sizeof(stMsgElemntT57_42.usMsgElemntType) + sizeof(stMsgElemntT57_42.usMsgElemntLen) + stMsgElemntT57_42.usMsgElemntLen;
  stMsgElemntT57_42.usMsgElemntLen = htons(stMsgElemntT57_42.usMsgElemntLen);

  //IE57_43
  usMsgElemntSuffix = 0;
  stMsgElemntT57_43.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_ALARM_CW_TYPE);

  ulAlarmOrEventType = htonl(43);
  cpss_mem_memcpy(&stMsgElemntT57_43.aucMsgElemntValue[usMsgElemntSuffix],&ulAlarmOrEventType,sizeof(ulAlarmOrEventType));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulAlarmOrEventType);

  cpss_mem_memcpy(&stMsgElemntT57_43.aucMsgElemntValue[usMsgElemntSuffix],&stT43,sizeof(MT_IE57_43_T));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(MT_IE57_43_T);
  stMsgElemntT57_43.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT57_43Len = sizeof(stMsgElemntT57_43.usMsgElemntType) + sizeof(stMsgElemntT57_43.usMsgElemntLen) + stMsgElemntT57_43.usMsgElemntLen;
  stMsgElemntT57_43.usMsgElemntLen = htons(stMsgElemntT57_43.usMsgElemntLen);

  //37-57
  usMsgElemntSuffix = 0;
  stMsgElemntT37.usMsgElemntType = htons(CW_MSG_ELEMENT_VENDOR_SPEC_PAYLOAD_CW_TYPE);

  cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulNetCarrierInc);

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5721)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_21,ulMsgElemntT57_21Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_21Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5722)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_22,ulMsgElemntT57_22Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_22Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5723)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_23,ulMsgElemntT57_23Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_23Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5724)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_24,ulMsgElemntT57_24Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_24Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5725)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_25,ulMsgElemntT57_25Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_25Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5726)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_26,ulMsgElemntT57_26Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_26Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5727)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_27,ulMsgElemntT57_27Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_27Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5728)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_28,ulMsgElemntT57_28Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_28Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5729)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_29,ulMsgElemntT57_29Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_29Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5730)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_30,ulMsgElemntT57_30Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_30Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5731)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_31,ulMsgElemntT57_31Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_31Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5732)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_32,ulMsgElemntT57_32Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_32Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5733)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_33,ulMsgElemntT57_33Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_33Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5734)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_34,ulMsgElemntT57_34Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_34Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5735)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_35,ulMsgElemntT57_35Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_35Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5736)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_36,ulMsgElemntT57_36Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_36Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5737)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_37,ulMsgElemntT57_37Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_37Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5738)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_38,ulMsgElemntT57_38Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_38Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5739)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_39,ulMsgElemntT57_39Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_39Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5740)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_40,ulMsgElemntT57_40Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_40Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5741)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_41,ulMsgElemntT57_41Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_41Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5742)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_42,ulMsgElemntT57_42Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_42Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_5743)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT57_43,ulMsgElemntT57_43Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT57_43Len;
     }

  stMsgElemntT37.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT37Len = sizeof(stMsgElemntT37.usMsgElemntType) + sizeof(stMsgElemntT37.usMsgElemntLen) + stMsgElemntT37.usMsgElemntLen;
  stMsgElemntT37.usMsgElemntLen = htons(stMsgElemntT37.usMsgElemntLen);

  stWtpEventReq.ucPreamble = (UINT8)0;
  stWtpEventReq.ucHdLen = 16;
  stWtpEventReq.aucHdFlag[0] = 3;
  stWtpEventReq.aucHdFlag[1] = 0;
  stWtpEventReq.usFragID = 0;
  stWtpEventReq.usFragOffset = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[0] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[1] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[2] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_WTP_EVENT_REQUEST;
  stWtpEventReq.ucSeqNum = (UINT8)0;
  stWtpEventReq.usMsgElemnetLen = sizeof(stWtpEventReq.usMsgElemnetLen)
                              + (UINT16)(sizeof(stWtpEventReq.ucFlag))
                              + ulMsgElemntT37Len;

  stWtpEventReq.ucFlag = (UINT8)0;

  cpss_mem_memcpy(stWtpEventReq.aucMsgElemntBuf,&stMsgElemntT37,ulMsgElemntT37Len);

  ulWtpEventReqLen = 13 + stWtpEventReq.usMsgElemnetLen;
  stWtpEventReq.usMsgElemnetLen = htons(stWtpEventReq.usMsgElemnetLen);

  gstWtpEventReq.ulBufLen = ulWtpEventReqLen;
  cpss_mem_memcpy(gstWtpEventReq.aucBuff,(UINT8*)&stWtpEventReq,gstWtpEventReq.ulBufLen);

  bSendResult = MT_SendApMsg2AC(vulApIndex, glCpmSockId, gstWtpEventReq.aucBuff, gstWtpEventReq.ulBufLen);

  return bSendResult;
}

//IE37_IE60,61,62,63,64,65,66,67,68
MTBool ApMutiTestSendEventReq_IE37_IE6X(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;
//  UINT32 ulLoop = 0;
  CAPWAP_MSG_T stWtpEventReq;
  UINT32 ulWtpEventReqLen = 0;
  MSG_ELEMNT_T stMsgElemntT37;
  UINT32 ulMsgElemntT37Len = 0;
  MSG_ELEMNT_T stMsgElemntT60;  //60  AP的终端统计（Rate报文统计）
  UINT32 ulMsgElemntT60Len = 0;
  MT_IE37_IE60_T stIE60;
  MSG_ELEMNT_T stMsgElemntT61;  //61  地址重定向攻击通告
  UINT32 ulMsgElemntT61Len = 0;
  MT_IE37_IE61_T stIE61;
  MSG_ELEMNT_T stMsgElemntT62;  //62  WAPI特性性能参数
  UINT32 ulMsgElemntT62Len = 0;
  MT_IE37_IE62_T stIE62;
  UINT32 ulStIE62LEN = 0;
  MSG_ELEMNT_T stMsgElemntT63;  //63  AC/AP QoS基本性能参数
  UINT32 ulMsgElemntT63Len = 0;
  MT_IE37_IE63_T stIE63;
  MSG_ELEMNT_T stMsgElemntT64;  //64  AP BACKGROUD业务类QoS参数
  UINT32 ulMsgElemntT64Len = 0;
  MT_IE37_IE64_T stIE64;
  MSG_ELEMNT_T stMsgElemntT65;  //65  AP BESTEFFORT业务类QoS参数
  UINT32 ulMsgElemntT65Len = 0;
  MT_IE37_IE65_T stIE65;
  MSG_ELEMNT_T stMsgElemntT66;  //66  AP VOICE业务类QoS参数
  UINT32 ulMsgElemntT66Len = 0;
  MT_IE37_IE66_T stIE66;
  MSG_ELEMNT_T stMsgElemntT67;  //67  AP Video 业务类QoS参数
  UINT32 ulMsgElemntT67Len = 0;
  MT_IE37_IE67_T stIE67;
  MSG_ELEMNT_T stMsgElemntT68;  //68  AP 用户关联统计
  UINT32 ulMsgElemntT68Len = 0;
  MT_IE37_IE68_T stIE68;

  UINT8 aucWapiStaMAC0[MT_MAC_LENTH] = {0x00,0x0B,0xC0,0x02,0x9E,0x51};
  UINT8 aucWapiStaMAC1[MT_MAC_LENTH] = {0x00,0x0B,0xC0,0x02,0x9E,0x6D};
  UINT8 aucWapiStaMAC2[MT_MAC_LENTH] = {0x00,0x0B,0xC0,0x02,0x9E,0x53};
  UINT8 aucWapiStaMAC3[MT_MAC_LENTH] = {0x00,0x0B,0xC0,0x02,0x9E,0x52};

  UINT32 ulStaIndex = 0;
  UINT16 usMsgElemntSuffix = 0;
  UINT32 ulNetCarrierInc = htonl(gastApconf[vulApIndex].ulNetCarrierInc);

  cpss_mem_memset(&stMsgElemntT60,0,sizeof(stMsgElemntT60));
  cpss_mem_memset(&stMsgElemntT61,0,sizeof(stMsgElemntT61));
  cpss_mem_memset(&stMsgElemntT62,0,sizeof(stMsgElemntT62));
  cpss_mem_memset(&stMsgElemntT63,0,sizeof(stMsgElemntT63));
  cpss_mem_memset(&stMsgElemntT64,0,sizeof(stMsgElemntT64));
  cpss_mem_memset(&stMsgElemntT65,0,sizeof(stMsgElemntT65));
  cpss_mem_memset(&stMsgElemntT66,0,sizeof(stMsgElemntT66));
  cpss_mem_memset(&stMsgElemntT67,0,sizeof(stMsgElemntT67));
  cpss_mem_memset(&stMsgElemntT68,0,sizeof(stMsgElemntT68));
  cpss_mem_memset(&stIE60,0,sizeof(MT_IE37_IE60_T));
  cpss_mem_memset(&stIE61,0,sizeof(MT_IE37_IE61_T));
  cpss_mem_memset(&stIE62,0,sizeof(MT_IE37_IE62_T));
  cpss_mem_memset(&stIE63,0,sizeof(MT_IE37_IE63_T));
  cpss_mem_memset(&stIE64,0,sizeof(MT_IE37_IE64_T));
  cpss_mem_memset(&stIE65,0,sizeof(MT_IE37_IE65_T));
  cpss_mem_memset(&stIE66,0,sizeof(MT_IE37_IE66_T));
  cpss_mem_memset(&stIE67,0,sizeof(MT_IE37_IE67_T));
  cpss_mem_memset(&stIE68,0,sizeof(MT_IE37_IE68_T));

  //60  AP的终端统计（Rate报文统计）
  stIE60.ucRadioId = 1;//Radio标识，1字节。
  stIE60.ucChannel = 1;//本RADIO的当前信道，1字节
  stIE60.usStaNum = 5;//一个Radio上的终端个数，最大为128。

  for(ulStaIndex = 0; ulStaIndex < stIE60.usStaNum; ulStaIndex++)
     {
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].usWlanID = htons(1);
	  if(vulApIndex != getApIDbyBssid(ulStaIndex))
	     {
		  break;
	     }
	  cpss_mem_memcpy(stIE60.astWlanStaParaPACKRate[ulStaIndex].aucStaMacAddr,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate1MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate1MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate2MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate2MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate55MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate55MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate6MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate6MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate9MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate9MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate11MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate11MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate12MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate12MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate18MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate18MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate24MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate24MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate36MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate36MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate48MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate48MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate54MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApTxRate54MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate1MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate1MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate2MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate2MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate55MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate55MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate6MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate6MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate9MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate9MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate11MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate11MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate12MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate12MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate18MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate18MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate24MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate24MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate36MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate36MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate48MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate48MbpsPktsHi = htonl(60);//高32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate54MbpsPktsLo = htonl(60);//低32位
	  stIE60.astWlanStaParaPACKRate[ulStaIndex].ulUserStatApRxRate54MbpsPktsHi = htonl(60);//高32位
     }

  stMsgElemntT60.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_WLAN_STA_STATIS_PACKRate_CW_TYPE);
  stMsgElemntT60.usMsgElemntLen = stIE60.usStaNum * sizeof(IE60_WLAN_STA_PARA_PACKRATE_T) + 4;
  stIE60.usStaNum = htons(stIE60.usStaNum);
  cpss_mem_memcpy(stMsgElemntT60.aucMsgElemntValue,&stIE60,stMsgElemntT60.usMsgElemntLen);
  ulMsgElemntT60Len = sizeof(stMsgElemntT60.usMsgElemntType) + sizeof(stMsgElemntT60.usMsgElemntLen) + stMsgElemntT60.usMsgElemntLen;
  stMsgElemntT60.usMsgElemntLen = htons(stMsgElemntT60.usMsgElemntLen);

      //61  地址重定向攻击通告
  stIE61.ucRadioId = 1;
  stIE61.ucWlanId = 2;
  stIE61.aucStaMac[0] = 0x61;
  stIE61.aucStaMac[1] = 0x01;
  stIE61.aucStaMac[2] = 0x02;
  stIE61.aucStaMac[3] = 0x03;
  stIE61.aucStaMac[4] = 0x04;
  stIE61.aucStaMac[5] = 0x05;

#if 1
  for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
     {
    if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState && vulApIndex == getApIDbyBssid(ulStaIndex))
           {
      cpss_mem_memcpy(stIE61.aucStaMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
      break;
           }
     }
#endif

  stMsgElemntT61.usMsgElemntType = htons(CW_MSG_ELEMENT_STA_REDATTACK_CW_TYPE);
  stMsgElemntT61.usMsgElemntLen = sizeof(MT_IE37_IE61_T);
  cpss_mem_memcpy(stMsgElemntT61.aucMsgElemntValue,&stIE61,sizeof(MT_IE37_IE61_T));
  ulMsgElemntT61Len = sizeof(stMsgElemntT61.usMsgElemntType) + sizeof(stMsgElemntT61.usMsgElemntLen) + stMsgElemntT61.usMsgElemntLen;
  stMsgElemntT61.usMsgElemntLen = htons(stMsgElemntT61.usMsgElemntLen);

  //62  WAPI特性性能参数
  ulStIE62LEN = 0;
  stIE62.ucRadioId = 1;
  stIE62.ucWlanId = 1;
  stIE62.ulStaNum = 4;//gastAp[vulApIndex].ulStaCnt;

  cpss_mem_memcpy(stIE62.astApWapiPara[0].aucStaMac,aucWapiStaMAC0,MT_MAC_LENTH);
  stIE62.astApWapiPara[0].ulWPIDecryptableErrs = htonl(100);
  stIE62.astApWapiPara[0].ulWPIMICErrs = htonl(200);
  stIE62.astApWapiPara[0].ulWPIReplayCntr = htonl(300);

  cpss_mem_memcpy(stIE62.astApWapiPara[1].aucStaMac,aucWapiStaMAC1,MT_MAC_LENTH);
  stIE62.astApWapiPara[1].ulWPIDecryptableErrs = htonl(100);
  stIE62.astApWapiPara[1].ulWPIMICErrs = htonl(200);
  stIE62.astApWapiPara[1].ulWPIReplayCntr = htonl(300);

  cpss_mem_memcpy(stIE62.astApWapiPara[2].aucStaMac,aucWapiStaMAC2,MT_MAC_LENTH);
  stIE62.astApWapiPara[2].ulWPIDecryptableErrs = htonl(100);
  stIE62.astApWapiPara[2].ulWPIMICErrs = htonl(200);
  stIE62.astApWapiPara[2].ulWPIReplayCntr = htonl(300);

  cpss_mem_memcpy(stIE62.astApWapiPara[3].aucStaMac,aucWapiStaMAC3,MT_MAC_LENTH);
  stIE62.astApWapiPara[3].ulWPIDecryptableErrs = htonl(100);
  stIE62.astApWapiPara[3].ulWPIMICErrs = htonl(200);
  stIE62.astApWapiPara[3].ulWPIReplayCntr = htonl(300);

  ulStIE62LEN = stIE62.ulStaNum * sizeof(MT_AP_WAPI_PARA_T);

#if 0
  for(ulStaIndex = 0; ulStaIndex < stIE62.ulStaNum; ulStaIndex++)
      {
    if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState && vulApIndex == getApIDbyBssid(ulStaIndex))
           {
      cpss_mem_memcpy(stIE62.astApWapiPara[ulStaIndex].aucStaMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
      stIE62.astApWapiPara[ulStaIndex].ulWPIDecryptableErrs = htonl(100);
      stIE62.astApWapiPara[ulStaIndex].ulWPIMICErrs = htonl(200);
      stIE62.astApWapiPara[ulStaIndex].ulWPIReplayCntr = htonl(300);
      ulStIE62LEN = ulStIE62LEN + sizeof(MT_AP_WAPI_PARA_T);
           }
     }
#endif

  ulStIE62LEN = ulStIE62LEN + sizeof(stIE62.ucRadioId) + sizeof(stIE62.ucWlanId) + sizeof(stIE62.ulStaNum);

  stIE62.ulStaNum = htonl(stIE62.ulStaNum);

  stMsgElemntT62.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_WLAN_WAPI_STATIS_CW_TYPE);
  stMsgElemntT62.usMsgElemntLen = ulStIE62LEN;
  cpss_mem_memcpy(stMsgElemntT62.aucMsgElemntValue,&stIE62,stMsgElemntT62.usMsgElemntLen);
  ulMsgElemntT62Len = sizeof(stMsgElemntT62.usMsgElemntType) + sizeof(stMsgElemntT62.usMsgElemntLen) + stMsgElemntT62.usMsgElemntLen;
  stMsgElemntT62.usMsgElemntLen = htons(stMsgElemntT62.usMsgElemntLen);

  //63  AC/AP QoS基本性能参数
  stIE63.ulRadioId = htonl(1);
  stIE63.ulQueueAveLen = htonl(100);
  stIE63.ulLostPktsCnt = htonl(200);
  stIE63.ulAveRate = htonl(120);

  stMsgElemntT63.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_QOS_BASE_STATIS_CW_TYPE);
  stMsgElemntT63.usMsgElemntLen = sizeof(MT_IE37_IE63_T);
  cpss_mem_memcpy(stMsgElemntT63.aucMsgElemntValue,&stIE63,sizeof(MT_IE37_IE63_T));
  ulMsgElemntT63Len = sizeof(stMsgElemntT63.usMsgElemntType) + sizeof(stMsgElemntT63.usMsgElemntLen) + stMsgElemntT63.usMsgElemntLen;
  stMsgElemntT63.usMsgElemntLen = htons(stMsgElemntT63.usMsgElemntLen);

  //64  AP BACKGROUD业务类QoS参数
  stIE64.ulRadioID = htonl(1);
  stIE64.ulLostTrafficNum = htonl(50);
  stIE64.ulLostPkts = htonl(60);
  stIE64.ulAvgQueueLen = htonl(40);
  stIE64.ulAvgBurstPkts = htonl(40);
  stIE64.ulApAvgRate = htonl(40);

  stMsgElemntT64.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_QOS_BACK_STATIS_CW_TYPE);
  stMsgElemntT64.usMsgElemntLen = sizeof(MT_IE37_IE64_T);
  cpss_mem_memcpy(stMsgElemntT64.aucMsgElemntValue,&stIE64,sizeof(MT_IE37_IE64_T));
  ulMsgElemntT64Len = sizeof(stMsgElemntT64.usMsgElemntType) + sizeof(stMsgElemntT64.usMsgElemntLen) + stMsgElemntT64.usMsgElemntLen;
  stMsgElemntT64.usMsgElemntLen = htons(stMsgElemntT64.usMsgElemntLen);

  //65  AP BESTEFFORT业务类QoS参数
  stIE65.ulRadioID = htonl(1);
  stIE65.ulLostTrafficNum = htonl(100);
  stIE65.ulLostPkts = htonl(120);
  stIE65.ulAvgQueueLen = htonl(180);
  stIE65.ulAvgBurstPkts = htonl(150);
  stIE65.ulApAvgRate = htonl(130);

  stMsgElemntT65.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_QOS_BEST_STATIS_CW_TYPE);
  stMsgElemntT65.usMsgElemntLen = sizeof(MT_IE37_IE65_T);
  cpss_mem_memcpy(stMsgElemntT65.aucMsgElemntValue,&stIE65,sizeof(MT_IE37_IE65_T));
  ulMsgElemntT65Len = sizeof(stMsgElemntT65.usMsgElemntType) + sizeof(stMsgElemntT65.usMsgElemntLen) + stMsgElemntT65.usMsgElemntLen;
  stMsgElemntT65.usMsgElemntLen = htons(stMsgElemntT65.usMsgElemntLen);

  //66  AP VOICE业务类QoS参数
  stIE66.ulRadioID = htonl(1);
  stIE66.ulQueueLen = htonl(120);
  stIE66.ulAvgBurstRate = htonl(110);
  stIE66.ulLostPktsRate = htonl(100);
  stIE66.ulAvgRate = htonl(150);
  stIE66.ulSucLineNum = htonl(160);
  stIE66.ulLostLineNum = htonl(100);
  stIE66.ulLostTrafficPkts = htonl(130);
  stIE66.ulRefusedNum = htonl(160);

  stMsgElemntT66.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_QOS_VOICE_STATIS_CW_TYPE);
  stMsgElemntT66.usMsgElemntLen = sizeof(MT_IE37_IE66_T);
  cpss_mem_memcpy(stMsgElemntT66.aucMsgElemntValue,&stIE66,sizeof(MT_IE37_IE66_T));
  ulMsgElemntT66Len = sizeof(stMsgElemntT66.usMsgElemntType) + sizeof(stMsgElemntT66.usMsgElemntLen) + stMsgElemntT66.usMsgElemntLen;
  stMsgElemntT66.usMsgElemntLen = htons(stMsgElemntT66.usMsgElemntLen);

  //67  AP Video 业务类QoS参数
  stIE67.ulRadioID = htonl(1);
  stIE67.ulQueueLen = htonl(100);
  stIE67.ulAvgBurstRate = htonl(110);
  stIE67.ulLostPktsRate = htonl(120);
  stIE67.ulAvgRate = htonl(110);
  stIE67.ulSucLineNum = htonl(100);
  stIE67.ulLostLineNum = htonl(130);
  stIE67.ulRefusedNum = htonl(150);

  stMsgElemntT67.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_QOS_VIDEO_STATIS_CW_TYPE);
  stMsgElemntT67.usMsgElemntLen = sizeof(MT_IE37_IE67_T);
  cpss_mem_memcpy(stMsgElemntT67.aucMsgElemntValue,&stIE67,sizeof(MT_IE37_IE67_T));
  ulMsgElemntT67Len = sizeof(stMsgElemntT67.usMsgElemntType) + sizeof(stMsgElemntT67.usMsgElemntLen) + stMsgElemntT67.usMsgElemntLen;
  stMsgElemntT67.usMsgElemntLen = htons(stMsgElemntT67.usMsgElemntLen);

  //68  AP 用户关联统计
  stIE68.ulDot11bApUserAssoCnt = htonl(200);
  stIE68.ulDot11bApAssoFailCnt = htonl(10);
  stIE68.ulDot11bApReassoUserRate = htonl(100);
  stIE68.ulDot11gApUserAssoCnt = htonl(220);
  stIE68.ulDot11gApAssoFailCnt = htonl(10);
  stIE68.ulDot11gApReassoUserRate = htonl(10);

  stMsgElemntT68.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_STATIS_CW_TYPE);
  stMsgElemntT68.usMsgElemntLen = sizeof(MT_IE37_IE68_T);
  cpss_mem_memcpy(stMsgElemntT68.aucMsgElemntValue,&stIE68,sizeof(MT_IE37_IE68_T));
  ulMsgElemntT68Len = sizeof(stMsgElemntT68.usMsgElemntType) + sizeof(stMsgElemntT68.usMsgElemntLen) + stMsgElemntT68.usMsgElemntLen;
  stMsgElemntT68.usMsgElemntLen = htons(stMsgElemntT68.usMsgElemntLen);

  //37-60,61,62,63,64,65,66,67,68
  usMsgElemntSuffix = 0;
  stMsgElemntT37.usMsgElemntType = htons(CW_MSG_ELEMENT_VENDOR_SPEC_PAYLOAD_CW_TYPE);

  //ulNetCarrierInc
  cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulNetCarrierInc);

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_60)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT60,ulMsgElemntT60Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT60Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_61)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT61,ulMsgElemntT61Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT61Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_62)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT62,ulMsgElemntT62Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT62Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_63)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT63,ulMsgElemntT63Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT63Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_64)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT64,ulMsgElemntT64Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT64Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_65)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT65,ulMsgElemntT65Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT65Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_66)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT66,ulMsgElemntT66Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT66Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_67)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT67,ulMsgElemntT67Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT67Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_68)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT68,ulMsgElemntT68Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT68Len;
     }

  stMsgElemntT37.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT37Len = sizeof(stMsgElemntT37.usMsgElemntType) + sizeof(stMsgElemntT37.usMsgElemntLen) + stMsgElemntT37.usMsgElemntLen;
  stMsgElemntT37.usMsgElemntLen = htons(stMsgElemntT37.usMsgElemntLen);

  stWtpEventReq.ucPreamble = (UINT8)0;
  stWtpEventReq.ucHdLen = 16;
  stWtpEventReq.aucHdFlag[0] = 3;
  stWtpEventReq.aucHdFlag[1] = 0;
  stWtpEventReq.usFragID = 0;
  stWtpEventReq.usFragOffset = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[0] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[1] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[2] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_WTP_EVENT_REQUEST;
  stWtpEventReq.ucSeqNum = (UINT8)0;
  stWtpEventReq.usMsgElemnetLen = sizeof(stWtpEventReq.usMsgElemnetLen)
                              + (UINT16)(sizeof(stWtpEventReq.ucFlag))
                              + ulMsgElemntT37Len;

  stWtpEventReq.ucFlag = (UINT8)0;

  cpss_mem_memcpy(stWtpEventReq.aucMsgElemntBuf,&stMsgElemntT37,ulMsgElemntT37Len);

  ulWtpEventReqLen = 13 + stWtpEventReq.usMsgElemnetLen;
  stWtpEventReq.usMsgElemnetLen = htons(stWtpEventReq.usMsgElemnetLen);

  gstWtpEventReq.ulBufLen = ulWtpEventReqLen;
  cpss_mem_memcpy(gstWtpEventReq.aucBuff,(UINT8*)&stWtpEventReq,gstWtpEventReq.ulBufLen);

  bSendResult = MT_SendApMsg2AC(vulApIndex, glCpmSockId, gstWtpEventReq.aucBuff, gstWtpEventReq.ulBufLen);

  return bSendResult;
}

//IE37_IE58,59,60,71,72
MTBool ApMutiTestSendEventReq_IE37_IE5859(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;
  UINT32 ulLoop = 0;
  CAPWAP_MSG_T stWtpEventReq;
  UINT32 ulWtpEventReqLen = 0;
  MSG_ELEMNT_T stMsgElemntT37;
  UINT32 ulMsgElemntT37Len = 0;

  MSG_ELEMNT_T stMsgElemntT58;  //58  AP的终端统计（RSSI报文统计）
  UINT32 ulMsgElemntT58Len = 0;
  MT_IE37_IE58_T stIE58;

  MSG_ELEMNT_T stMsgElemntT59;  //59  AP的终端统计（用户报文长度分布信息统计）
  UINT32 ulMsgElemntT59Len = 0;
  MT_IE37_IE59_T stIE59;

  UINT16 usMsgElemntSuffix = 0;
  UINT32 ulNetCarrierInc = htonl(gastApconf[vulApIndex].ulNetCarrierInc);

  cpss_mem_memset(&stWtpEventReq,0,sizeof(CAPWAP_MSG_T));
  cpss_mem_memset(&stMsgElemntT37,0,sizeof(MSG_ELEMNT_T));
  cpss_mem_memset(&stMsgElemntT58,0,sizeof(MSG_ELEMNT_T));
  cpss_mem_memset(&stMsgElemntT59,0,sizeof(MSG_ELEMNT_T));
  cpss_mem_memset(&stIE58,0,sizeof(MT_IE37_IE58_T));
  cpss_mem_memset(&stIE59,0,sizeof(MT_IE37_IE59_T));

  //cpss_mem_memset(&,0,sizeof());

  //58  AP的终端统计（RSSI报文统计）
  stIE58.ucRadioId = 1;//Radio标识，1字节。
  stIE58.ucChannel = 1;//本RADIO的当前信道，1字节
  stIE58.usStaNum = 5;//一个Radio上的终端个数，最大为128。

  for(ulLoop = 0; ulLoop < stIE58.usStaNum; ulLoop++)
     {
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].usWlanID = htons(1);
	  if(getApIDbyBssid(ulLoop) == vulApIndex)
	     {
		  cpss_mem_memcpy(stIE58.astWlanStaParaPACKRSSI[ulLoop].aucStaMacAddr,gastStaPara[ulLoop].auStaMac,MT_MAC_LENTH);
	     }
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSIto90PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSIto90PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI90to85PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI90to85PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI85to80PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI85to80PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI80to75PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI80to75PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI75to70PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI75to70PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI70to65PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI70to65PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI65to60PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI65to60PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI60to55PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI60to55PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI55to50PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI55to50PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI50to45PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI50to45PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI45toPktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApRxRSSI45toPktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSIto90PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSIto90PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI90to85PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI90to85PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI85to80PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI85to80PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI80to75PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI80to75PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI75to70PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI75to70PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI70to65PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI70to65PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI65to60PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI65to60PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI60to55PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI60to55PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI55to50PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI55to50PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI50to45PktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI50to45PktsHi = htonl(58);//高32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI45toPktsLo = htonl(58);//低32位
	  stIE58.astWlanStaParaPACKRSSI[ulLoop].ulUserStatApTxRSSI45toPktsHi = htonl(58);//高32位
     }

  stMsgElemntT58.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_WLAN_STA_STATIS_PACKRSSI_CW_TYPE);
  stMsgElemntT58.usMsgElemntLen = stIE58.usStaNum * sizeof(IE58_WLAN_STA_PARA_PACKRSSI_T) + 4;
  stIE58.usStaNum = htons(stIE58.usStaNum);
  cpss_mem_memcpy(stMsgElemntT58.aucMsgElemntValue,&stIE58,stMsgElemntT58.usMsgElemntLen);
  ulMsgElemntT58Len = sizeof(stMsgElemntT58.usMsgElemntType) + sizeof(stMsgElemntT58.usMsgElemntLen) + stMsgElemntT58.usMsgElemntLen;
  stMsgElemntT58.usMsgElemntLen = htons(stMsgElemntT58.usMsgElemntLen);

  //59  AP的终端统计（用户报文长度分布信息统计）
  stIE59.ucRadioId = 1;
  stIE59.ucChannel = 1;
  stIE59.usStaNum = 5;

  for(ulLoop = 0; ulLoop < stIE59.usStaNum; ulLoop++)
     {
	  stIE59.astWlanStaParaPACKLEN[ulLoop].usWlanID = htons(1);
	  if(getApIDbyBssid(ulLoop) == vulApIndex)
	     {
		  cpss_mem_memcpy(stIE59.astWlanStaParaPACKLEN[ulLoop].aucStaMacAddr,gastStaPara[ulLoop].auStaMac,MT_MAC_LENTH);
	     }
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen0to128PktsLo = htonl(59);//低32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen0to128PktsHi = htonl(59);//高32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen128to256PktsLo = htonl(59);//低32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen128to256PktsHi = htonl(59);//高32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen256to512PktsLo = htonl(59);//低32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen256to512PktsHi = htonl(59);//高32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen512to768PktsLo = htonl(59);//低32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen512to768PktsHi = htonl(59);//高32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen768to1024PktsLo = htonl(59);//低32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen768to1024PktsHi = htonl(59);//高32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen1024to1536PktsLo = htonl(59);//低32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen1024to1536PktsHi = htonl(59);//高32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen1536to2048PktsLo = htonl(59);//低32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen1536to2048PktsHi = htonl(59);//高32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen2048to2560PktsLo = htonl(59);//低32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen2048to2560PktsHi = htonl(59);//高32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen2560to3072PktsLo = htonl(59);//低32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen2560to3072PktsHi = htonl(59);//高32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen3072to3584PktsLo = htonl(59);//低32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen3072to3584PktsHi = htonl(59);//高32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen3584toPktsLo = htonl(59);//低32位
	  stIE59.astWlanStaParaPACKLEN[ulLoop].ulUserInlen3584toPktsHi = htonl(59);//高32位
     }

  stMsgElemntT59.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_WLAN_STA_STATIS_PACKLEN_CW_TYPE);
  stMsgElemntT59.usMsgElemntLen = stIE59.usStaNum * sizeof(IE59_WLAN_STA_PATA_PACKLEN_T) + 4;
  stIE59.usStaNum = htons(stIE59.usStaNum);
  cpss_mem_memcpy(stMsgElemntT59.aucMsgElemntValue,&stIE59,stMsgElemntT59.usMsgElemntLen);
  ulMsgElemntT59Len = sizeof(stMsgElemntT59.usMsgElemntType) + sizeof(stMsgElemntT59.usMsgElemntLen) + stMsgElemntT59.usMsgElemntLen;
  stMsgElemntT59.usMsgElemntLen = htons(stMsgElemntT59.usMsgElemntLen);

  //37.58,59,
  usMsgElemntSuffix = 0;
  stMsgElemntT37.usMsgElemntType = htons(CW_MSG_ELEMENT_VENDOR_SPEC_PAYLOAD_CW_TYPE);

  //ulNetCarrierInc
  cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulNetCarrierInc);

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_58)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT58,ulMsgElemntT58Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT58Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_59)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT59,ulMsgElemntT59Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT59Len;
     }

  stMsgElemntT37.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT37Len = sizeof(stMsgElemntT37.usMsgElemntType) + sizeof(stMsgElemntT37.usMsgElemntLen) + stMsgElemntT37.usMsgElemntLen;
  stMsgElemntT37.usMsgElemntLen = htons(stMsgElemntT37.usMsgElemntLen);

  stWtpEventReq.ucPreamble = (UINT8)0;
  stWtpEventReq.ucHdLen = 16;
  stWtpEventReq.aucHdFlag[0] = 3;
  stWtpEventReq.aucHdFlag[1] = 0;
  stWtpEventReq.usFragID = 0;
  stWtpEventReq.usFragOffset = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[0] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[1] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[2] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_WTP_EVENT_REQUEST;
  stWtpEventReq.ucSeqNum = (UINT8)0;
  stWtpEventReq.usMsgElemnetLen = sizeof(stWtpEventReq.usMsgElemnetLen)
                              + (UINT16)(sizeof(stWtpEventReq.ucFlag))
                              + ulMsgElemntT37Len;

  stWtpEventReq.ucFlag = (UINT8)0;

  cpss_mem_memcpy(stWtpEventReq.aucMsgElemntBuf,&stMsgElemntT37,ulMsgElemntT37Len);

  ulWtpEventReqLen = 13 + stWtpEventReq.usMsgElemnetLen;
  stWtpEventReq.usMsgElemnetLen = htons(stWtpEventReq.usMsgElemnetLen);

  if(ulWtpEventReqLen > MT_CAPWAPMSG_BUFF_LEN)
    {
    ulWtpEventReqLen = MT_CAPWAPMSG_BUFF_LEN;
    }

  gstWtpEventReq.ulBufLen = ulWtpEventReqLen;
  cpss_mem_memcpy(gstWtpEventReq.aucBuff,(UINT8*)&stWtpEventReq,gstWtpEventReq.ulBufLen);

  bSendResult = MT_SendApMsg2AC(vulApIndex, glCpmSockId, gstWtpEventReq.aucBuff, gstWtpEventReq.ulBufLen);

  return bSendResult;
}

//IE37-IE71-72-78-79
MTBool ApMutiTestSendEventReq_IE37_IE7X(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;
  UINT32 ulLoop = 0;
  CAPWAP_MSG_T stWtpEventReq;
  UINT32 ulWtpEventReqLen = 0;
  MSG_ELEMNT_T stMsgElemntT37;
  UINT32 ulMsgElemntT37Len = 0;

  MSG_ELEMNT_T stMsgElemntT71;  //71  AP的终端统计（SNR报文统计）
  UINT32 ulMsgElemntT71Len = 0;
  MT_IE37_IE71_T stIE71;

  MSG_ELEMNT_T stMsgElemntT72;  //72  AP的终端统计（离线统计）
  UINT32 ulMsgElemntT72Len = 0;
  MT_IE37_IE72_T stIE72;

  MSG_ELEMNT_T stMsgElemntT78;
  UINT32 ulMsgElemntT78Len = 0;
  MT_IE37_IE78_T stIE78;

  MSG_ELEMNT_T stMsgElemntT79;
  UINT32 ulMsgElemntT79Len = 0;
  MT_IE37_IE79_T stIE79;

  UINT32 ulStaIndex = 0;
  UINT16 usMsgElemntSuffix = 0;
//  UINT8 aucStrTmp[4] = {0};
  UINT32 ulNetCarrierInc = htonl(gastApconf[vulApIndex].ulNetCarrierInc);

  cpss_mem_memset(&stMsgElemntT71,0,sizeof(MSG_ELEMNT_T));
  cpss_mem_memset(&stMsgElemntT72,0,sizeof(MSG_ELEMNT_T));
  cpss_mem_memset(&stMsgElemntT78,0,sizeof(MSG_ELEMNT_T));
  cpss_mem_memset(&stMsgElemntT79,0,sizeof(MSG_ELEMNT_T));

  cpss_mem_memset(&stIE71,0,sizeof(MT_IE37_IE71_T));
  cpss_mem_memset(&stIE72,0,sizeof(MT_IE37_IE72_T));
  cpss_mem_memset(&stIE78,0,sizeof(MT_IE37_IE78_T));
  cpss_mem_memset(&stIE79,0,sizeof(MT_IE37_IE79_T));

  //71  AP的终端统计（SNR报文统计）
  stIE71.ucRadioId = 1;//Radio标识，1字节。
  stIE71.ucChannel = 1;//本RADIO的当前信道，1字节
  stIE71.usStaNum = 1;//一个Radio上的终端个数，最大为128。

  for(ulLoop = 0; ulLoop < stIE71.usStaNum; ulLoop++)
     {
	  stIE71.astWlanStaParaPACKSNR[ulLoop].usWlanID = htons(1);
	  if(getApIDbyBssid(ulLoop) == vulApIndex)
	     {
		  cpss_mem_memcpy(stIE71.astWlanStaParaPACKSNR[ulLoop].aucStaMacAddr,gastStaPara[ulLoop].auStaMac,MT_MAC_LENTH);
	     }
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNRto5PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNRto5PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR5to15PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR5to15PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR15to20PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR15to20PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR20to25PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR20to25PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR25to30PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR25to30PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR30to35PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR30to35PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR30to40PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR30to40PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR40to45PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR40to45PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR45to50PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR45to50PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR50toPktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApRxSNR50toPktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNRto5PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNRto5PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR5to15PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR5to15PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR15to20PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR15to20PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR20to25PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR20to25PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR25to30PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR25to30PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR30to35PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR30to35PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR30to40PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR30to40PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR40to45PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR40to45PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR45to50PktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR45to50PktsHi = htonl(71);//高32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR50toPktsLo = htonl(71);//低32位
	  stIE71.astWlanStaParaPACKSNR[ulLoop].ulUserStatApTxSNR50toPktsHi = htonl(71);//高32位
     }

  stMsgElemntT71.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_WLAN_STA_STATIS_PACKSNR_CW_TYPE);
  stMsgElemntT71.usMsgElemntLen = stIE71.usStaNum * sizeof(IE71_AP_WLAN_STA_STATIS_PACKSNR_T) + 4;
  stIE71.usStaNum = htons(stIE71.usStaNum);
  cpss_mem_memcpy(stMsgElemntT71.aucMsgElemntValue,&stIE71,stMsgElemntT71.usMsgElemntLen);
  ulMsgElemntT71Len = sizeof(stMsgElemntT71.usMsgElemntType) + sizeof(stMsgElemntT71.usMsgElemntLen) + stMsgElemntT71.usMsgElemntLen;
  stMsgElemntT71.usMsgElemntLen = htons(stMsgElemntT71.usMsgElemntLen);

  //72  AP的终端统计（离线统计）
  stIE72.ucRadioId = 1;//Radio标识，1字节。
  stIE72.ucChannel = 1;//本RADIO的当前信道，1字节
  stIE72.usStaNum = 1;//一个Radio上的终端个数，最大为128。

  for(ulLoop = 0; ulLoop < stIE72.usStaNum;ulLoop++)
     {
	  stIE72.astWlanStaParaOFF[ulLoop].usWlanID = htons(1);
	  if(getApIDbyBssid(ulLoop) == vulApIndex)
	  {
		  cpss_mem_memcpy(stIE72.astWlanStaParaOFF[ulLoop].aucStaMacAddr,gastStaPara[ulLoop].auStaMac,MT_MAC_LENTH);
	  }
	  stIE72.astWlanStaParaOFF[ulLoop].ulUserStaAccessTimeDELAY = htonl(72);//接入时延
	  time((time_t*)&stIE72.astWlanStaParaOFF[ulLoop].ulProbeRequestfirsttime);
	  stIE72.astWlanStaParaOFF[ulLoop].ulProbeRequestfirsttime = htonl(stIE72.astWlanStaParaOFF[ulLoop].ulProbeRequestfirsttime);//接入请求探针发送时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
	  stIE72.astWlanStaParaOFF[ulLoop].ulProbeRequestSum = htonl(72);//接入请求次数
	  time((time_t*)&stIE72.astWlanStaParaOFF[ulLoop].ulProbeResponsetime);
	  stIE72.astWlanStaParaOFF[ulLoop].ulProbeResponsetime = htonl(stIE72.astWlanStaParaOFF[ulLoop].ulProbeResponsetime);//接入请求探针响应时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
	  stIE72.astWlanStaParaOFF[ulLoop].ulprobeFailReason = htonl(2);//接入失败原因
	  time((time_t*)&stIE72.astWlanStaParaOFF[ulLoop].ulAssociationRequesttime);
	  stIE72.astWlanStaParaOFF[ulLoop].ulAssociationRequesttime = htonl(stIE72.astWlanStaParaOFF[ulLoop].ulAssociationRequesttime);//关联请求时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
	  stIE72.astWlanStaParaOFF[ulLoop].ulStatAssoReqSum = htonl(72);//关联请求次数
	  time((time_t*)&stIE72.astWlanStaParaOFF[ulLoop].ulStatAssoRsptime);
	  stIE72.astWlanStaParaOFF[ulLoop].ulStatAssoRsptime = htonl(stIE72.astWlanStaParaOFF[ulLoop].ulStatAssoRsptime);//关联响应时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
	  stIE72.astWlanStaParaOFF[ulLoop].ulStatAssFailReason = htonl(3);//关联失败原因
	  time((time_t*)&stIE72.astWlanStaParaOFF[ulLoop].ulUserStaReltime);
	  stIE72.astWlanStaParaOFF[ulLoop].ulUserStaReltime = htonl(stIE72.astWlanStaParaOFF[ulLoop].ulUserStaReltime);//STA释放时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
	  stIE72.astWlanStaParaOFF[ulLoop].ulUserStaRelReason = htonl(1);//STA释放原因
     }

  stMsgElemntT72.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_WLAN_OFFSTA_STATIS_CW_TYPE);
  stMsgElemntT72.usMsgElemntLen = stIE72.usStaNum * sizeof(IE72_AP_WLAN_OFFSTA_STATIS_T) + 4;
  stIE72.usStaNum = htons(stIE72.usStaNum);
  cpss_mem_memcpy(stMsgElemntT72.aucMsgElemntValue,&stIE72,stMsgElemntT72.usMsgElemntLen);
  ulMsgElemntT72Len = sizeof(stMsgElemntT72.usMsgElemntType) + sizeof(stMsgElemntT72.usMsgElemntLen) + stMsgElemntT72.usMsgElemntLen;
  stMsgElemntT72.usMsgElemntLen = htons(stMsgElemntT72.usMsgElemntLen);

  //78  AP无线接口性能统计，且基于每个Radio进行统计
  stIE78.ucRadioId = 1;//Radio标识
  stIE78.ucRadioType = 1;//Radio类型
  stIE78.ucCurrentChan = 11;//当前信道
//  stIE78.ulRadioId = htonl(1);//Radio标识
  stIE78.ulApAvgSignalStrgth = htonl(78);//接收的信号平均强度，单位为dBm
  stIE78.ulApHighestRxStrgth = htonl(78);//接收的最高信号强度，单位为dBm
  stIE78.ulApLowestRxStrgth = htonl(78);//接收的最低信号强度，单位为dBm
  stIE78.ulWtpWIfUpdownTimes = htonl(78);//无线端口updown次数
  stIE78.ulWtpWIfTxDataPktsHi = htonl(78);//发送的数据包数高32位
  stIE78.ulWtpWIfTxDataPktsLo = htonl(78);//发送的数据包数低32位
  stIE78.ulWtpWIfRxDataPktsHi = htonl(78);//接收的数据包数高32位
  stIE78.ulWtpWIfRxDataPktsLo = htonl(78);//接收的数据包数低32位
  stIE78.ulWtpWIfUplinkDataOctetsHi = htonl(78);//接收的数据包字节数高32位，单位为Bytes
  stIE78.ulWtpWIfUplinkDataOctetsLo = htonl(78);//接收的数据包字节数低32位，单位为Bytes
  stIE78.ulWtpWIfDwlinkDataOctetsHi = htonl(78);//发送的数据包字节数高32位，单位为Bytes
  stIE78.ulWtpWIfDwlinkDataOctetsLo = htonl(78);//发送的数据包字节数低32位，单位为Bytes
  stIE78.ulWtpWIfChStatsDwlinkTotRetryPktsHi = htonl(78);//无线信道下行重传的数据包数高32位
  stIE78.ulWtpWIfChStatsDwlinkTotRetryPktsLo = htonl(78);//无线信道下行重传的数据包数低32位
  stIE78.ulWtpWIfRxMgmtFrameCntHi = htonl(78);//接收到的管理帧的数量高32位
  stIE78.ulWtpWIfRxMgmtFrameCntLo = htonl(78);//接收到的管理帧的数量低32位
  stIE78.ulWtpWIfRxCtrlFrameCntHi = htonl(78);//接收到的控制帧的数量高32位
  stIE78.ulWtpWIfRxCtrlFrameCntLo = htonl(78);//接收到的控制帧的数量低32位
  stIE78.ulWtpWIfRxDataFrameCntHi = htonl(78);//接收到的数据帧的数量高32位
  stIE78.ulWtpWIfRxDataFrameCntLo = htonl(78);//接收到的数据帧的数量低32位
  stIE78.ulWtpWIfTxMgmtFrameCntHi = htonl(78);//发送到的管理帧的数量高32位
  stIE78.ulWtpWIfTxMgmtFrameCntLo = htonl(78);//发送到的管理帧的数量低32位
  stIE78.ulWtpWIfTxCtrlFrameCntHi = htonl(78);//发送到的控制帧的数量高32位
  stIE78.ulWtpWIfTxCtrlFrameCntLo = htonl(78);//发送到的控制帧的数量低32位
  stIE78.ulWtpWIfTxDataFrameCntHi = htonl(78);//接收到的数据帧的数量高32位
  stIE78.ulWtpWIfTxDataFrameCnt = htonl(78);//接收到的数据帧的数量低32位
  stIE78.ulWtpWIfRetryCntHiLo = htonl(78);//无线信道下行重传的帧数高32位
  stIE78.ulWtpWIfRetryCntLo = htonl(78);//无线信道下行重传的帧数低32位
  stIE78.ulWtpWIfChStatsPhyErrPktsHi = htonl(78);//接收的错误数据包数高32位
  stIE78.ulWtpWIfChStatsPhyErrPktsLo = htonl(78);//接收的错误数据包数低32位
  stIE78.ulWtpWIfChStatsFrameErrorCntHi = htonl(78);//接收的错帧数高32位
  stIE78.ulWtpWIfChStatsFrameErrorCntLo = htonl(78);//接收的错帧数低32位
  stIE78.ulWtpWIfTxChStatsPhyErrPktsHi = htonl(78);//发送的错误数据包数高32位
  stIE78.ulWtpWIfTxChStatsPhyErrPktsLo = htonl(78);//发送的错误数据包数低32位
  stIE78.ulWtpWIfChStatsMacFcsErrPktsHi = htonl(78);//接收的FCS MAC错帧数高32位
  stIE78.ulWtpWIfChStatsMacFcsErrPktsLo = htonl(78);//接收的FCS MAC错帧数低32位
  stIE78.ulWtpWIfChStatsMacMicErrPktsHi = htonl(78);//接收的MIC MAC错帧数高32位
  stIE78.ulWtpWIfChStatsMacMicErrPktsLo = htonl(78);//接收的MIC MAC错帧数低32位
  stIE78.ulWtpWIfChStatsMacDecryptErrPktsHi = htonl(78);//接收的解密失败的MAC错帧数高32位
  stIE78.ulWtpWIfChStatsMacDecryptErrPktsLo = htonl(78);//接收的解密失败的MAC错帧数低32位
  stIE78.ulWtpWIfUplinkThroughout = htonl(78);//无线上行端口的流量，单位为Kbps
  stIE78.ulWtpWIfDownlinkThroughout = htonl(78);//无线下行端口的流量，单位为Kbps
  stIE78.ulWtpWIfUplinkFrameDiscardCntHi = htonl(78);//信道上行总的丢帧数高32位
  stIE78.ulWtpWIfUplinkFrameDiscardCntLo = htonl(78);//信道上行总的丢帧数低32位
  stIE78.ulWtpWIfUplinkFrameRetryCntHi = htonl(78);//信道上行总的重传帧数高32位
  stIE78.ulWtpWIfUplinkFrameRetryCntLo = htonl(78);//信道上行总的重传帧数低32位
  stIE78.ulWtpWIfUplinkFrameTotalCntHi = htonl(78);//信道上行总的帧数高32位
  stIE78.ulWtpWIfUplinkFrameTotalCntLo = htonl(78);//信道上行总的帧数低32位
  stIE78.ulWtpWIfDownlinkFrameTotalCntHi = htonl(78);//信道下行总的帧数高32位
  stIE78.ulWtpWIfDownlinkFrameTotalCntLo = htonl(78);//信道下行总的帧数低32位
  stIE78.ulWtpWIfDownlinkFrameErrorCntHi = htonl(78);//信道下行总的错帧数高32位
  stIE78.ulWtpWIfDownlinkFrameErrorCntLo = htonl(78);//信道下行总的错帧数低32位
  stIE78.ulWtpWIfDownlinkFrameDiscardCntHi = htonl(78);//信道下行总的丢帧数高32位
  stIE78.ulWtpWIfDownlinkFrameDiscardCntLo = htonl(78);//信道下行总的丢帧数低32位
  stIE78.ulWtpWIfDownlinkFrameRetryCntHi = htonl(78);//信道下行总的重传帧数高32位
  stIE78.ulWtpWIfDownlinkFrameRetryCntLo = htonl(78);//信道下行总的重传帧数低32位
  stIE78.ulWtpWIfRxDataFrameOctetsHi = htonl(78);//接收的数据帧字节数高32位，单位为Bytes
  stIE78.ulWtpWIfRxDataFrameOctetsLo = htonl(78);//接收的数据帧字节数低32位，单位为Bytes
  stIE78.ulWtpWIfTxDataFrameOctetsHi = htonl(78);//发送的数据帧字节数高32位，单位为Bytes
  stIE78.ulWtpWIfTxDataFrameOctetsLo = htonl(78);//发送的数据帧字节数低32位，单位为Bytes
  stIE78.ulWtpWIfRxAssocFrameOctetsHi = htonl(78);//发送的关联帧的数量高32位
  stIE78.ulWtpWIfRxAssocFrameOctetsLo = htonl(78);//发送的关联帧的数量低32位
  stIE78.ulWtpWIfTxAssocFrameOctetsHi = htonl(78);//接收到的关联帧的数量高32位
  stIE78.ulWtpWIfTxAssocFrameOctetsLo = htonl(78);//接收到的关联帧的数量低32位
  stIE78.ulWtpFhhopTimes = htonl(78);// 跳频次数
  stIE78.ulWtpWIfCurTxPwr = htonl(15);//当前AP的发射功率
  stIE78.ulSampleLenth = htonl(78);//抽样时长，单位为秒
//  stIE78.ulChStatsChannel = htonl(13);//AP当前占用的频点（1~13）
  stIE78.ulChStatsNumSta = htonl(64);//使用该信道的终端数
  strcpy((char*)stIE78.aucChStatsFrameErrRate,"78%");//信道的错帧率，单位为%，为8字节字符串
  strcpy((char*)stIE78.aucChStatsFrameRetryRate,"78%");//信道上帧的重传率，单位为%，为8字节字符串
  stIE78.ulMonitoredTime = htonl(78);//监控时长，单位为秒
  stIE78.ulchanUseRate = htonl(78);//AP工作频点空间占用率
  stIE78.usShtRetryThld = htons(78);//无线接口帧长小于RTS门限时最大重试次数
  stIE78.usLongRetryThld = htons(78);//无线接口帧长大于RTS门限时最大重试次数

  stIE78.ucWlanNum = 1;//WLAN个数

//  for(ulLoop = 0; ulLoop < stIE78.ucWlanNum;ulLoop++)
//    {
//    sprintf(aucStrTmp,"%d",ulLoop);
//    strcpy(stIE78.aucAPNeighborSSIDList,"PC_PLAIN_LOCAL");
//    strcat(stIE78.aucAPNeighborSSIDList,aucStrTmp);
//    }
  strcpy((char*)stIE78.aucAPNeighborSSIDList,"IE078_PC_PLAIN_LOCAL");// AP周围的SSID列表，每组为32字节字符串，最多16组
//  strcat(stIE78.aucAPNeighborSSIDList,"PC_WEP_LOCAL");
//  strcat(stIE78.aucAPNeighborSSIDList,"PC_WPA_PSK_LOCAL");

  stMsgElemntT78.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_RADIO_STATIS_CMCC_CW_TYPE);
  stMsgElemntT78.usMsgElemntLen = sizeof(MT_IE37_IE78_T) - (16 - stIE78.ucWlanNum) * 32;
  cpss_mem_memcpy(stMsgElemntT78.aucMsgElemntValue,&stIE78,sizeof(MT_IE37_IE78_T));
  ulMsgElemntT78Len = sizeof(stMsgElemntT78.usMsgElemntType) + sizeof(stMsgElemntT78.usMsgElemntLen) + stMsgElemntT78.usMsgElemntLen;
  stMsgElemntT78.usMsgElemntLen = htons(stMsgElemntT78.usMsgElemntLen);

  //79  AP的终端统计
  stIE79.ucRadioID = 1;
  stIE79.ucChannel = 1;
  stIE79.usStaNum = 1;

  for(ulLoop = 0; ulLoop < stIE79.usStaNum; ulLoop++)
    {
    stIE79.astWlanStaPara[ulLoop].usWlanId = htons(1);//Wlan标识
    MT_GetBssID(vulApIndex,stIE79.ucRadioID,stIE79.astWlanStaPara[ulLoop].usWlanId,stIE79.astWlanStaPara[ulLoop].aucBssID);
#if 0
    cpss_mem_memcpy(stIE79.astWlanStaPara[ulLoop].aucStaMac,gastAp[vulApIndex].astSta[ulLoop].auStaMac,MT_MAC_LENTH);//连接到AP的终端的MAC地址
#else
    for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
           {
      if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState  && vulApIndex == getApIDbyBssid(ulStaIndex))
             {
        cpss_mem_memcpy(stIE79.astWlanStaPara[ulLoop].aucStaMac,gastStaPara[ulStaIndex].auStaMac,MT_MAC_LENTH);
        break;
             }
           }
#endif

    stIE79.astWlanStaPara[ulLoop].ucStaWMMAttr = MT_IE37_IE79_WMM_SUPPORT;//1-WMMSta:支持QoS机制的终端，2-NonWMMSta:不支持QoS机制的终端
    stIE79.astWlanStaPara[ulLoop].ucStaPowerSaveMode = MT_IE37_IE79_PWR_SAVE;//终端节电状态1：active, 2：powersave
    stIE79.astWlanStaPara[ulLoop].ucStaRadiosType = 1;//按位的最后4 位，可以组合,基本定义类型如下：
    stIE79.astWlanStaPara[ulLoop].ulUserStatStaUpTime = htonl(79);//终端关联时长，单位为0.01秒
    stIE79.astWlanStaPara[ulLoop].ulUserStatResourceUseRate = htonl(79);
    stIE79.astWlanStaPara[ulLoop].ulChanRxUseTime = htonl(79);//上行信道有效占用时长
    stIE79.astWlanStaPara[ulLoop].ulChanTxUseTime = htonl(79);//下行信道有效占用时长
    stIE79.astWlanStaPara[ulLoop].ulChanTotalUseTime = htonl(79);//总空口有效占用时长
    stIE79.astWlanStaPara[ulLoop].ulChanRxTotalUseTime = htonl(79);//上行信道占用总时长
    stIE79.astWlanStaPara[ulLoop].ulChanTxTotalUseTime = htonl(79);//下行信道占用总时长
    stIE79.astWlanStaPara[ulLoop].ulChanTxTotalRTTTime = htonl(79);//下行数据包传输总时长
    stIE79.astWlanStaPara[ulLoop].ulChanRxTotalRTTTime = htonl(79);//上行数据包传输总时长
    stIE79.astWlanStaPara[ulLoop].ulAPRecvdStaSignalStrthACKTime = htonl(79);
    stIE79.astWlanStaPara[ulLoop].ilAPRecvdStaSignlStrthACK = htonl(-79);//AP发出数据帧后收到STA回送的ACK控制帧的信号强度（有符号型，单位：dBm)。
    stIE79.astWlanStaPara[ulLoop].ilAPRecvdStaSignalStrth = htonl(79);//AP接收到的终端的当前信号强度，有符号型，单位：dBm
    stIE79.astWlanStaPara[ulLoop].ulAPRecvdStaSNR = htonl(79);//AP接收到的终端的当前信噪比，单位：dB
//  cpss_mem_memcpy(stIE79.astWlanStaPara[ulLoop].aucStaMac,gastAp[vulApIndex].astSta[ulLoop].auStaMac,MT_MAC_LENTH);//连接到AP的终端的MAC地址
//  strcpy(stIE79.astWlanStaPara[ulLoop].aucAPRecvdStaSignlStrthACK,"-66dBm");
//    stIE79.astWlanStaPara[ulLoop].ilAPRecvdStaSignlStrthACK = htonl(-66);
//  strcpy(stIE79.astWlanStaPara[ulLoop].aucUserStatAPRecvdStaSignalStrth,"-57dBm");//AP接收到的终端的当前信号强度
//    stIE79.astWlanStaPara[ulLoop].ilAPRecvdStaSignalStrth = htonl(57);
//  strcpy(stIE79.astWlanStaPara[ulLoop].aucUserStatAPReceivedStaSNR,"67dB");//AP接收到的终端的当前信噪比
//    stIE79.astWlanStaPara[ulLoop].ulAPRecvdStaSNR = htonl(67);
    stIE79.astWlanStaPara[ulLoop].ulStaTxPktsHi = htonl(79);//发到终端的包数高32位
    stIE79.astWlanStaPara[ulLoop].ulStaTxPktsLo = htonl(79);//发到终端的包数低32位
    stIE79.astWlanStaPara[ulLoop].ulStaTxBytesHi = htonl(79);//发到终端的字节数高32位，单位为Bytes
    stIE79.astWlanStaPara[ulLoop].ulStaTxBytesLo = htonl(79);//发到终端的字节数低32位，单位为Bytes
    stIE79.astWlanStaPara[ulLoop].ulStaRxPktsHi = htonl(79);//从终端收到的包数高32位
    stIE79.astWlanStaPara[ulLoop].ulStaRxPktsLo = htonl(79);//从终端收到的包数低32位
    stIE79.astWlanStaPara[ulLoop].ulStaRxBytesHi = htonl(79);//从终端收到的字节数高32位，单位为Bytes
    stIE79.astWlanStaPara[ulLoop].ulStaRxBytesLo = htonl(79);//从终端收到的字节数低32位，单位为Bytes
    stIE79.astWlanStaPara[ulLoop].ulAPTxRatesDown = htonl(79);//终端当前接入速率（下行），单位为Kbps
    stIE79.astWlanStaPara[ulLoop].ulAPTxRatesUp = htonl(79);//终端当前接入速率（上行），单位为Kbps
    stIE79.astWlanStaPara[ulLoop].ulAPTxRatesAvgDown = htonl(79);//终端平均速接入速率（下行），单位为Kbps
    stIE79.astWlanStaPara[ulLoop].ulAPTxRatesAvgUp = htonl(79);//终端平均速接入速率（上行），单位为Kbps
    stIE79.astWlanStaPara[ulLoop].ulStaTxRetryPktsHi = htonl(79);//终端重发的包数高32位
    stIE79.astWlanStaPara[ulLoop].ulStaTxRetryPktsLo = htonl(79);//终端重发的包数低32位
    stIE79.astWlanStaPara[ulLoop].ulStaRxRetryPktsHi = htonl(79);//终端重收的包数高32位
    stIE79.astWlanStaPara[ulLoop].ulStaRxRetryPktsLo = htonl(79);//终端重收的包数低32位
    stIE79.astWlanStaPara[ulLoop].ulStaReceiveErrPktsHi = htonl(79);//接收到的来自终端的错误包数高32位
    stIE79.astWlanStaPara[ulLoop].ulStaReceiveErrPktsLo = htonl(79);//接收到的来自终端的错误包数低32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatApRxFrameCntHi = htonl(79);//AP接收到用户的数据帧数高32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatApRxFrameCntLo = htonl(79);//AP接收到用户的数据帧数低32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatApRxErrFrameCntHi = htonl(79);//AP接收到用户的错误帧数高32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatApRxErrFrameCntLo = htonl(79);//AP接收到用户的错误帧数低32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatApTxFrameCntHiLo = htonl(79);//AP发送给用户的数据帧数高32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatApTxFrameCntLo = htonl(79);//AP发送给用户的数据帧数低32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatApTxSuccFrameCntHi = htonl(79);//AP成功发送到用户的数据帧数高32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatApTxSuccFrameCntLo = htonl(79);//AP成功发送到用户的数据帧数低32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatApRxRetryBytesHi = htonl(79);//AP接收到用户的重传数据字节数高32位，单位为Bytes
    stIE79.astWlanStaPara[ulLoop].ulUserStatApRxRetryBytesLo = htonl(79);//AP接收到用户的重传数据字节数低32位，单位为Bytes
    stIE79.astWlanStaPara[ulLoop].ulUserStatApTxRetryBytesHi = htonl(79);//AP发送给用户的重传数据字节数高32位，单位为Bytes
    stIE79.astWlanStaPara[ulLoop].ulUserStatApTxRetryBytesLo = htonl(79);//AP发送给用户的重传数据字节数低32位，单位为Bytes
    stIE79.astWlanStaPara[ulLoop].ulUserRxCtrlFrameCntHi = htonl(79);//接收到的控制帧的数量高32位
    stIE79.astWlanStaPara[ulLoop].ulUserRxCtrlFrameCntLo = htonl(79);//接收到的控制帧的数量低32位
    stIE79.astWlanStaPara[ulLoop].ulUserRxMgmtFrameCntHi = htonl(79);//接收到的管理帧的数量高32位
    stIE79.astWlanStaPara[ulLoop].ulUserRxMgmtFrameCntLo = htonl(79);//接收到的管理帧的数量低32位
    stIE79.astWlanStaPara[ulLoop].ulUserRxDataFrameCntHi = htonl(79);//接收到的数据帧的数量高32位
    stIE79.astWlanStaPara[ulLoop].ulUserRxDataFrameCntLo = htonl(79);//接收到的数据帧的数量低32位
    stIE79.astWlanStaPara[ulLoop].ulUserTxCtrlFrameCntHi = htonl(79);//发送到的控制帧的数量高32位
    stIE79.astWlanStaPara[ulLoop].ulUserTxCtrlFrameCntLo = htonl(79);//发送到的控制帧的数量低32位
    stIE79.astWlanStaPara[ulLoop].ulUserTxMgmtFrameCntHi = htonl(79);//发送到的管理帧的数量高32位
    stIE79.astWlanStaPara[ulLoop].ulUserTxMgmtFrameCntLo = htonl(79);//发送到的管理帧的数量低32位
    stIE79.astWlanStaPara[ulLoop].ulUserTxDataFrameCntHi = htonl(79);//接收到的数据帧的数量高32位
    stIE79.astWlanStaPara[ulLoop].ulUserTxDataFrameCntLo = htonl(79);//接收到的数据帧的数量低32位
    stIE79.astWlanStaPara[ulLoop].ulUserInUcastPktsHi = htonl(79);//端口接收单播包数高32位
    stIE79.astWlanStaPara[ulLoop].ulUserInUcastPktsLo = htonl(79);//端口接收单播包数低32位
    stIE79.astWlanStaPara[ulLoop].ulUserInNUcastPktsHi = htonl(79);//端口接收非单播包数高32位
    stIE79.astWlanStaPara[ulLoop].ulUserInNUcastPktsLo = htonl(79);//端口接收非单播包数低32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatApTxRetryFrameCntHi = htonl(79);//AP发送给用户的重传数据帧数高32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatApTxRetryFrameCntLo = htonl(79);//AP发送给用户的重传数据帧数低32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatApTxErrPktsHi = htonl(79);//AP发送给用户的错误包数高32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatApTxErrPktsLo = htonl(79);//AP发送给用户的错误包数低32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatTotalErrPktsHi = htonl(79);//总错误包数高32位
    stIE79.astWlanStaPara[ulLoop].ulUserStatTotalErrPktsLo = htonl(79);//总错误包数低32位
    //time(&stIE79.astWlanStaPara[ulLoop].ulUserStaAccessTime);
    stIE79.astWlanStaPara[ulLoop].ulUserStaAccessTime = htonl(7900);//用户的接入时间点
    time((time_t*)&stIE79.astWlanStaPara[ulLoop].ulUserStaAccessTimeDELAY);
    stIE79.astWlanStaPara[ulLoop].ulUserStaAccessTimeDELAY = htonl(stIE79.astWlanStaPara[ulLoop].ulUserStaAccessTimeDELAY);//接入时延 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
    stIE79.astWlanStaPara[ulLoop].ulProbeRequestfirsttime = htonl(79);//接入请求探针发送时刻
    stIE79.astWlanStaPara[ulLoop].ulProbeRequestSum = htonl(79);//接入请求次数
    time((time_t*)&stIE79.astWlanStaPara[ulLoop].ulProbeResponsetime);
    stIE79.astWlanStaPara[ulLoop].ulProbeResponsetime = htonl(stIE79.astWlanStaPara[ulLoop].ulProbeResponsetime);//接入请求探针响应时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
    time((time_t*)&stIE79.astWlanStaPara[ulLoop].ulAssociationRequesttime);
    stIE79.astWlanStaPara[ulLoop].ulAssociationRequesttime = htonl(stIE79.astWlanStaPara[ulLoop].ulAssociationRequesttime);//关联请求时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
    stIE79.astWlanStaPara[ulLoop].ulStatAssoReqSum = htonl(79);//关联请求次数
    time((time_t*)&stIE79.astWlanStaPara[ulLoop].ulStatAssoRspTime);
    stIE79.astWlanStaPara[ulLoop].ulStatAssoRspTime = htonl(stIE79.astWlanStaPara[ulLoop].ulStatAssoRspTime);//关联响应时刻 单位为秒，1970年1月1日0时0分0秒起经历的秒数。
    stIE79.astWlanStaPara[ulLoop].ulStationOnlineSTART = htonl(79);//论证时刻
    stIE79.astWlanStaPara[ulLoop].ulStationOnlineTOTALTIME = htonl(79);//在线总时长
    stIE79.astWlanStaPara[ulLoop].ulUserifUplinkDataThroughput = htonl(79);//以太网口发送流量[MB]（ 以太网上行端口流量）单位为kbps
    stIE79.astWlanStaPara[ulLoop].ulUserifDownlinkDataThroughput = htonl(79);//以太网口接收流量[MB] （以太网下行端口流量）单位为kbps
    stIE79.astWlanStaPara[ulLoop].aucUserifMAC[0] = 0x79;//以太网口MAC地址
    stIE79.astWlanStaPara[ulLoop].aucUserifMAC[1] = 0x01;
    stIE79.astWlanStaPara[ulLoop].aucUserifMAC[2] = 0x02;
    stIE79.astWlanStaPara[ulLoop].aucUserifMAC[3] = 0x03;
    stIE79.astWlanStaPara[ulLoop].aucUserifMAC[4] = 0x04;
    stIE79.astWlanStaPara[ulLoop].aucUserifMAC[5] = ulLoop;
    stIE79.astWlanStaPara[ulLoop].ulUserifMtu = htonl(1400);//以太网口MTU值
    }

  stMsgElemntT79.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_WLAN_STA_STATIS_CMCC_CW_TYPE);
  stMsgElemntT79.usMsgElemntLen = 4 + stIE79.usStaNum * sizeof(MT_IE37_IE79_WLAN_STA_PARA_T);
  stIE79.usStaNum = htons(stIE79.usStaNum);
  cpss_mem_memcpy(stMsgElemntT79.aucMsgElemntValue,&stIE79,stMsgElemntT79.usMsgElemntLen);
  ulMsgElemntT79Len = sizeof(stMsgElemntT79.usMsgElemntType) + sizeof(stMsgElemntT79.usMsgElemntLen) + stMsgElemntT79.usMsgElemntLen;
  stMsgElemntT79.usMsgElemntLen = htons(stMsgElemntT79.usMsgElemntLen);


  //37-71,72,78,79
  usMsgElemntSuffix = 0;
  stMsgElemntT37.usMsgElemntType = htons(CW_MSG_ELEMENT_VENDOR_SPEC_PAYLOAD_CW_TYPE);

  //ulNetCarrierInc
  cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulNetCarrierInc);

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_71)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT71,ulMsgElemntT71Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT71Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_72)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT72,ulMsgElemntT72Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT72Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_78)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT78,ulMsgElemntT78Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT78Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_79)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT79,ulMsgElemntT79Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT79Len;
     }

  stMsgElemntT37.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT37Len = sizeof(stMsgElemntT37.usMsgElemntType) + sizeof(stMsgElemntT37.usMsgElemntLen) + stMsgElemntT37.usMsgElemntLen;
  stMsgElemntT37.usMsgElemntLen = htons(stMsgElemntT37.usMsgElemntLen);

  stWtpEventReq.ucPreamble = (UINT8)0;
  stWtpEventReq.ucHdLen = 16;
  stWtpEventReq.aucHdFlag[0] = 3;
  stWtpEventReq.aucHdFlag[1] = 0;
  stWtpEventReq.usFragID = 0;
  stWtpEventReq.usFragOffset = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[0] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[1] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[2] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_WTP_EVENT_REQUEST;
  stWtpEventReq.ucSeqNum = (UINT8)0;
  stWtpEventReq.usMsgElemnetLen = sizeof(stWtpEventReq.usMsgElemnetLen)
                              + (UINT16)(sizeof(stWtpEventReq.ucFlag))
                              + ulMsgElemntT37Len;

  stWtpEventReq.ucFlag = (UINT8)0;

  cpss_mem_memcpy(stWtpEventReq.aucMsgElemntBuf,&stMsgElemntT37,ulMsgElemntT37Len);

  ulWtpEventReqLen = 13 + stWtpEventReq.usMsgElemnetLen;
  stWtpEventReq.usMsgElemnetLen = htons(stWtpEventReq.usMsgElemnetLen);

  if(ulWtpEventReqLen > MT_CAPWAPMSG_BUFF_LEN)
    {
    ulWtpEventReqLen = MT_CAPWAPMSG_BUFF_LEN;
    }

  gstWtpEventReq.ulBufLen = ulWtpEventReqLen;
  cpss_mem_memcpy(gstWtpEventReq.aucBuff,(UINT8*)&stWtpEventReq,gstWtpEventReq.ulBufLen);

  bSendResult = MT_SendApMsg2AC(vulApIndex, glCpmSockId, gstWtpEventReq.aucBuff, gstWtpEventReq.ulBufLen);

  return bSendResult;
}

//37-IE80,
//IE37-IE80-81-82-83-84-88
MTBool ApMutiTestSendEventReq_IE37_IE8X(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;
  UINT32 ulLoop = 0;
  CAPWAP_MSG_T stWtpEventReq;
  UINT32 ulWtpEventReqLen = 0;
  MSG_ELEMNT_T stMsgElemntT37;
  UINT32 ulMsgElemntT37Len = 0;

  MSG_ELEMNT_T stMsgElemntT80;
  UINT32 ulMsgElemntT80Len = 0;
  MT_IE37_IE80_T stIE80;

  MSG_ELEMNT_T stMsgElemntT81;
  UINT32 ulMsgElemntT81Len = 0;
  MT_IE37_IE81_T stIE81;

  MSG_ELEMNT_T stMsgElemntT82;
  UINT32 ulMsgElemntT82Len = 0;
  MT_IE37_IE82_T stIE82;

  MSG_ELEMNT_T stMsgElemntT83;
  UINT32 ulMsgElemntT83Len = 0;
  MT_IE37_IE83_T stIE83;

  MSG_ELEMNT_T stMsgElemntT84;
  UINT32 ulMsgElemntT84Len = 0;
  MT_IE37_IE84_T stIE84;

  MSG_ELEMNT_T stMsgElemntT88;
  UINT32 ulMsgElemntT88Len = 0;
  MT_IE37_IE88_T stIE88;

  UINT32 ulStaIndex = 0;
  UINT16 usMsgElemntSuffix = 0;
  UINT8 aucStrTmp[4] = {0};
  UINT32 ulNetCarrierInc = htonl(gastApconf[vulApIndex].ulNetCarrierInc);

  cpss_mem_memset(&stMsgElemntT80,0,sizeof(MSG_ELEMNT_T));
  cpss_mem_memset(&stMsgElemntT81,0,sizeof(MSG_ELEMNT_T));
  cpss_mem_memset(&stMsgElemntT82,0,sizeof(MSG_ELEMNT_T));
  cpss_mem_memset(&stMsgElemntT83,0,sizeof(MSG_ELEMNT_T));
  cpss_mem_memset(&stMsgElemntT84,0,sizeof(MSG_ELEMNT_T));
  cpss_mem_memset(&stMsgElemntT88,0,sizeof(MSG_ELEMNT_T));

  cpss_mem_memset(&stIE80,0,sizeof(MT_IE37_IE80_T));
  cpss_mem_memset(&stIE81,0,sizeof(MT_IE37_IE81_T));
  cpss_mem_memset(&stIE82,0,sizeof(MT_IE37_IE82_T));
  cpss_mem_memset(&stIE83,0,sizeof(MT_IE37_IE83_T));
  cpss_mem_memset(&stIE84,0,sizeof(MT_IE37_IE84_T));
  cpss_mem_memset(&stIE88,0,sizeof(MT_IE37_IE88_T));

  //80  AP的SSID性能统计，且基于AP中每个Radio的每个SSID进行统计
  stIE80.ucRadioID = 1;
  stIE80.ucWlanNum = 1;

  for(ulLoop = 0; ulLoop < stIE80.ucWlanNum; ulLoop++)
    {
    //cpss_mem_memcpy(aucStrTmp,0,4);
    stIE80.astWlanStaPara[ulLoop].ucWlanID = ulLoop + 1;//Radio标识
    sprintf((char*)aucStrTmp,"%d",ulLoop);
    strcpy((char*)stIE80.astWlanStaPara[ulLoop].aucWlanStatSSID,"IE080_PC_PLAIN_LOCAL");//SSID，32字节字符串。
    strcat((char*)stIE80.astWlanStaPara[ulLoop].aucWlanStatSSID,(char*)aucStrTmp);

    MT_GetBssID(vulApIndex,stIE80.ucRadioID,stIE80.astWlanStaPara[ulLoop].ucWlanID,stIE80.astWlanStaPara[ulLoop].aucBSSID);//Wlan Mac

    stIE80.astWlanStaPara[ulLoop].ulWlanStatTxDataPktsHi = htonl(80);// 发送的数据包数高32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatTxDataPktsLo = htonl(80);// 发送的数据包数低32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatRxDataPktsHi = htonl(80);// 接收的数据包数高32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatRxDataPktsLo = htonl(80);// 接收的数据包数低32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatUplinkDataOctetsHi = htonl(80);// 接收的数据包字节数高32位，单位为Bytes
    stIE80.astWlanStaPara[ulLoop].ulWlanStatUplinkDataOctetsLo = htonl(80);// 接收的数据包字节数低32位，单位为Bytes
    stIE80.astWlanStaPara[ulLoop].ulWlanStatDwlinkDataOctetsHi = htonl(80);// 发送的数据包字节数高32位，单位为Bytes
    stIE80.astWlanStaPara[ulLoop].ulWlanStatDwlinkDataOctetsLo = htonl(80);// 发送的数据包字节数低32位，单位为Bytes
    stIE80.astWlanStaPara[ulLoop].ulWlanStatChStatsDwlinkTotRetryPktsHi = htonl(80);// 下行重传的包数高32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatChStatsDwlinkTotRetryPktsLo = htonl(80);// 下行重传的包数低32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatApChStatsNumStations = htonl(64);// 当前连接到该SSID的终端数
    stIE80.astWlanStaPara[ulLoop].ulWlanStatApStationOnlineSum = htonl(64);// 当前SSID下在线的终端数
    stIE80.astWlanStaPara[ulLoop].ulWlanStatUplinkThroughout = htonl(80);//无线上行端口的流量，单位为Kbps
    stIE80.astWlanStaPara[ulLoop].ulWlanStatDownlinkThroughout = htonl(80);//无线下行端口的流量，单位为Kbps
    stIE80.astWlanStaPara[ulLoop].ulWlanStatUplinkFrameDiscardCntHi = htonl(80);//信道上行总的丢帧数高32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatUplinkFrameDiscardCntLo = htonl(80);//信道上行总的丢帧数低32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatUplinkFrameRetryCntHi = htonl(80);//信道上行总的重传帧数高32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatUplinkFrameRetryCntLo = htonl(80);//信道上行总的重传帧数低32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatUplinkFrameTotalCntHi = htonl(80);// 信道上行总的帧数高32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatUplinkFrameTotalCntLo = htonl(80);// 信道上行总的帧数低32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatDownlinkFrameTotalCntHi = htonl(80);// 信道下行总的帧数高32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatDownlinkFrameTotalCntLo = htonl(80);// 信道下行总的帧数低32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatDownlinkFrameErrorCntHi = htonl(80);// 信道下行总的错帧数高32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatDownlinkFrameErrorCntLo = htonl(80);// 信道下行总的错帧数低32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatDownlinkFrameDiscardCntHi = htonl(80);// 信道下行总的丢帧数高32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatDownlinkFrameDiscardCntLo = htonl(80);// 信道下行总的丢帧数低32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatDownlinkFrameRetryCntHi = htonl(80);//信道下行总的重传帧数高32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatDownlinkFrameRetryCntLo = htonl(80);//信道下行总的重传帧数低32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatRxDataFrameOctetsHi = htonl(80);//接收的数据帧字节数高32位，单位为Bytes
    stIE80.astWlanStaPara[ulLoop].ulWlanStatRxDataFrameOctetsLo = htonl(80);//接收的数据帧字节数低32位，单位为Bytes
    stIE80.astWlanStaPara[ulLoop].ulWlanStatTxDataFrameOctetsHi = htonl(80);//发送的数据帧字节数高32位，单位为Bytes
    stIE80.astWlanStaPara[ulLoop].ulWlanStatTxDataFrameOctetsLo = htonl(80);//发送的数据帧字节数低32位，单位为Bytes
    stIE80.astWlanStaPara[ulLoop].ulWlanStatRxAssocFrameOctetsHi = htonl(80);//发送的关联帧的数量高32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatRxAssocFrameOctetsLo = htonl(80);//发送的关联帧的数量低32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatTxAssocFrameOctetsHi = htonl(80);//接收到的关联帧的数量高32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatTxAssocFrameOctetsLo = htonl(80);//接收到的关联帧的数量低32位
    stIE80.astWlanStaPara[ulLoop].ulWlanStatResourceUseRate = htonl(80);//资源利用率, AP设备空中接口资源利用情况，以时间占用情况计算，单位为%
    stIE80.astWlanStaPara[ulLoop].ulWlanStatAssoReqSum = htonl(80);//用户请求接入次数（计数器）
    stIE80.astWlanStaPara[ulLoop].ulWlanStatAssoRspSum = htonl(80);//AP响应用户接入请求次数（计数器）
    stIE80.astWlanStaPara[ulLoop].ulWlanStatAssoSuccSum = htonl(80);//用户成功接入次数（计数器）
    stIE80.astWlanStaPara[ulLoop].ulWlanStatApChStatsNumStations_ever = htonl(61);//所有曾经关联到此SSID的用户总数.
    }

  stMsgElemntT80.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_WLAN_STATIS_VALUES_CMCC_CW_TYPE);
  stMsgElemntT80.usMsgElemntLen = 4 + stIE80.ucWlanNum * sizeof(MT_IE37_IE80_WLAN_STA_PARA_T);
  cpss_mem_memcpy(stMsgElemntT80.aucMsgElemntValue,&stIE80,stMsgElemntT80.usMsgElemntLen);
  ulMsgElemntT80Len = sizeof(stMsgElemntT80.usMsgElemntType) + sizeof(stMsgElemntT80.usMsgElemntLen) + stMsgElemntT80.usMsgElemntLen;
  stMsgElemntT80.usMsgElemntLen = htons(stMsgElemntT80.usMsgElemntLen);

  //81  AP系统性能统计
  strcpy((char*)stIE81.aucWtpCPUType,"MT_TESTER_CPU");//AP的CPU类型，32字节字符串
  stIE81.ulWtpCPURateUsage = htonl(81);//CPU实时利用率，单位为%
  stIE81.ulWtpCPUPeakUsage = htonl(81);//CPU峰值利用率，单位为%
  stIE81.ulWtpCPUAvgUsage = htonl(81);//CPU平均利用率，单位为%
  strcpy((char*)stIE81.aucWtpMemoryType,"MT_TESTER_MEM");//AP的内存类型，32字节字符串
  stIE81.ulWtpMemorySize = htonl(2048000);//内存总数，单位为KB
  stIE81.ulWtpMemoryRemain = htonl(1024000);//内存可用数，单位为KB
  stIE81.ulWtpMemRateUsage = htonl(81);//内存实时利用率，单位为%
  stIE81.ulWtpMemPeakUsag = htonl(81);//内存峰值利用率，单位为%
  stIE81.ulWtpMemAvgUsagee = htonl(81);//内存平均利用率，单位为%
  stIE81.ulWtpFlashSize = htonl(4096000);//闪存总数，单位为KB
  stIE81.ulWtpFlashRemain = htonl(2048000);//闪存可用数，单位为KB
  stIE81.ulWtpFlashRateUsage = htonl(81);//闪存实时利用率，单位为%
  stIE81.ulWtpTemperature = htonl(81);//AP的当前温度

  stMsgElemntT81.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_SYS_STATIS_CMCC_CW_TYPE);
  stMsgElemntT81.usMsgElemntLen = sizeof(MT_IE37_IE81_T);
  cpss_mem_memcpy(stMsgElemntT81.aucMsgElemntValue,&stIE81,stMsgElemntT81.usMsgElemntLen);
  ulMsgElemntT81Len = sizeof(stMsgElemntT81.usMsgElemntType) + sizeof(stMsgElemntT81.usMsgElemntLen) + stMsgElemntT81.usMsgElemntLen;
  stMsgElemntT81.usMsgElemntLen = htons(stMsgElemntT81.usMsgElemntLen);

  //82  AP有线接口性能统计，基于每个AP设备的每个有线接口进行统计。每个有线接口包含一个本参数。从AP启动开始统计。且上报从AP启动开始统计以来的统计总量
  stIE82.usMtu = htons(1500);
  stIE82.usInterfaceRate = htons(1000);
  stIE82.ulWtpEthifInUcastPktsHi = htonl(82);//端口接收单播包数高32位
  stIE82.ulWtpEthifInUcastPktsLo = htonl(82);//端口接收单播包数低32位
  stIE82.ulWtpEthifInNUcastPktsHi = htonl(82);//端口接收非单播包数高32位
  stIE82.ulWtpEthifInNUcastPktsLo = htonl(82);//端口接收非单播包数低32位
  stIE82.ulWtpEthifInOctetsHi = htonl(82);//端口接收的总字节数高32位，从AP启动开始统计，单位为Bytes
  stIE82.ulWtpEthifInOctetsLo = htonl(82);//端口接收的总字节数低32位，从AP启动开始统计，单位为Bytes
  stIE82.ulWtpEthifInDiscardPktsHi = htonl(82);//端口丢弃接收到的包数高32位
  stIE82.ulWtpEthifInDiscardPktsLo = htonl(82);//端口丢弃接收到的包数低32位
  stIE82.ulWtpEthifInErrorsHi = htonl(82);//端口接收到的错误包数高32位
  stIE82.ulWtpEthifInErrorsLo = htonl(82);//端口接收到的错误包数低32位
  stIE82.ulWtpEthifOutUcastPktsHi = htonl(82);//端口发送单播包数高32位
  stIE82.ulWtpEthifOutUcastPktsLo = htonl(82);//端口发送单播包数低32位
  stIE82.ulWtpEthifOutNUcastPktsHi = htonl(82);//端口发送非单播包数高32位
  stIE82.ulWtpEthifOutNUcastPktsLo = htonl(82);//端口发送非单播包数低32位
  stIE82.ulWtpEthifOutOctetsHi = htonl(82);//端口发送的总字节数高32位，从AP启动开始统计，单位为Bytes
  stIE82.ulWtpEthifOutOctetsLo = htonl(82);//端口发送的总字节数低32位，从AP启动开始统计，单位为Bytes
  stIE82.ulWtpEthifOutDiscardPktsHi = htonl(82);//端口丢弃要发送的包数高32位
  stIE82.ulWtpEthifOutDiscardPktsLo = htonl(82);//端口丢弃要发送的包数低32位
  stIE82.ulWtpEthifOutErrorsHi = htonl(82);//端口发送错误的包数高32位
  stIE82.ulWtpEthifOutErrorsLo = htonl(82);//端口发送错误的包数低32位
  stIE82.ulWtpEthifUpDwnTimes = htonl(82);//端口updown次数
  stIE82.ulWtpEthifDownlinkDataThroughput = htonl(82);//以太网下行端口流量，单位为kbps
  stIE82.ulWtpEthifUplinkDataThroughput = htonl(82);//以太网上行端口流量，单位为kbps
  stIE82.ulWtpEthifInBcastPktsHi = htonl(82);//端口接收广播包数高32位
  stIE82.ulWtpEthifInBcastPktsLo = htonl(82);//端口接收广播包数低32位
  stIE82.ulWtpEthifInMcastPktsHi = htonl(82);//端口接收组播包数高32位
  stIE82.ulWtpEthifInMcastPktsLo = htonl(82);//端口接收组播包数低32位
  stIE82.ulWtpEthifOutBcastPktsHi = htonl(82);//端口发送广播包数高32位
  stIE82.ulWtpEthifOutBcastPktsLo = htonl(82);//端口发送广播包数低32位
  stIE82.ulWtpEthifOutMcastPktsHi = htonl(82);//端口发送组播包数高32位
  stIE82.ulWtpEthifOutMcastPktsLo = htonl(82);//端口发送组播包数低32位
  stIE82.ulWtpEthifInFrameCntHi = htonl(82);//接收总的帧数高32位
  stIE82.ulWtpEthifInFrameCntLo = htonl(82);//接收总的帧数低32位
  stIE82.ulWtpEthifInFlow = htonl(82);//接收流量, 单位为kbps
  stIE82.ulWtpEthifInUcastFrameCntHi = htonl(82);//端口接收单播帧数高32位
  stIE82.ulWtpEthifInUcastFrameCntLo = htonl(82);//端口接收单播帧数低32位
  stIE82.ulWtpEthifInBcastFrameCntHi = htonl(82);//端口接收广播帧数高32位
  stIE82.ulWtpEthifInBcastFrameCntLo = htonl(82);//端口接收广播帧数低32位
  stIE82.ulWtpEthifInMcastFrameCntHi = htonl(82);//端口接收组播帧数高32位
  stIE82.ulWtpEthifInMcastFrameCntLo = htonl(82);//端口接收组播帧数低32位
  stIE82.ulWtpEthifOutFrameCntHi = htonl(82);//发送总的帧数高32位
  stIE82.ulWtpEthifOutFrameCntLo = htonl(82);//发送总的帧数低32位
  stIE82.ulWtpEthOutFlow = htonl(82);//发送流量, 单位为kbps
  stIE82.ulWtpEthifOutUcastFrameCntHi = htonl(82);//端口发送单播帧数高32位
  stIE82.ulWtpEthifOutUcastFrameCntLo = htonl(82);//端口发送单播帧数低32位
  stIE82.ulWtpEthifOutBcastFrameCntHi = htonl(82);//端口发送广播帧包数高32位
  stIE82.ulWtpEthifOutBcastFrameCntLo = htonl(82);//端口发送广播帧包数低32位
  stIE82.ulWtpEthifOutMcastFrameCntHi = htonl(82);//端口发送组播帧数高32位
  stIE82.ulWtpEthifOutMcastFrameCntLo = htonl(82);//端口发送组播帧数低32位

  stMsgElemntT82.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_ETHER_IFPM_PARA_CMCC_CW_TYPE);
  stMsgElemntT82.usMsgElemntLen = sizeof(MT_IE37_IE82_T);
  cpss_mem_memcpy(stMsgElemntT82.aucMsgElemntValue,&stIE82,stMsgElemntT82.usMsgElemntLen);
  ulMsgElemntT82Len = sizeof(stMsgElemntT82.usMsgElemntType) + sizeof(stMsgElemntT82.usMsgElemntLen) + stMsgElemntT82.usMsgElemntLen;
  stMsgElemntT82.usMsgElemntLen = htons(stMsgElemntT82.usMsgElemntLen);

     //83  回传型AP上报的回传链路性能报告。
  stIE83.ucRadioID = 1;//Radio ID。
  stIE83.ucCurrentChan = 13;//信道号。

  MT_GetBssID(vulApIndex,stIE83.ucRadioID,1,stIE83.aucBSSID);//基本服务集识，这里标识本端Radio口的MAC地址。

  //邻居Radio口的MAC地址（ peer-mac-address）。
  cpss_mem_memcpy(stIE83.aucNbrMac,gastAp[vulApIndex].auApMac,MT_MAC_LENTH);
//  stIE83.aucNbrMac[0] = 0x83;
//  stIE83.aucNbrMac[1] = 0x01;
//  stIE83.aucNbrMac[2] = 0x02;
//  stIE83.aucNbrMac[3] = 0x03;
//  stIE83.aucNbrMac[4] = 0x04;
//  stIE83.aucNbrMac[5] = 0x05;

  stIE83.ucAPTransfersType = MT_MESH_TYPE_SAP;//Mesh设备当前类型，0：NONE，1：NAP，2：SAP。
  stIE83.icRSSI = -83;//一字节。Signal Strength Indication (RSSI) 。单位：dBm。
  stIE83.ucSNR = 83;//一字节。signal-to-noise ratio 。单位：dB。
  stIE83.ucLinkstate = MT_LINK_STATE_UP;//链路状态，0：up，1：down。
  stIE83.ucLinkMorS = MT_LINK_MorS_STATE_MASTER;//链路主备状态，0：主用，1：备用。
  stIE83.ulUptime = htonl(83);//当前WDS链路UP时间，单位：秒。
  stIE83.ucWlanNum = 2;//回传网络标识的个数, 最多16个。

  for(ulLoop = 0; ulLoop < stIE83.ucWlanNum; ulLoop++)
    {
    //cpss_mem_memcpy(aucStrTmp,0,4);
    stIE83.astWlanSubElemnt[ulLoop].ucWlanId = ulLoop;//回传网络标识编号
    sprintf((char*)aucStrTmp,"%d",ulLoop);
    stIE83.astWlanSubElemnt[ulLoop].ucSSIDLen = 32;//回传网络的服务集标识符长度，不超过32。
    strcpy((char*)stIE83.astWlanSubElemnt[ulLoop].aucSSID,"IE083_PC_PLAIN_LOCAL");//回传网络的服务集标识符，是ASCII形式的字符串，不超过32个字符。
    strcat((char*)stIE83.astWlanSubElemnt[ulLoop].aucSSID,(char*)aucStrTmp);
    }

  stMsgElemntT83.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_TRANSFER_LINK_REPORT_CW_TYPE);
  stMsgElemntT83.usMsgElemntLen = 28 + stIE83.ucWlanNum * sizeof(MT_IE37_IE83_WLAN_SUB_ELEMNT_T);
  cpss_mem_memcpy(stMsgElemntT83.aucMsgElemntValue,&stIE83,stMsgElemntT83.usMsgElemntLen);
  ulMsgElemntT83Len = sizeof(stMsgElemntT83.usMsgElemntType) + sizeof(stMsgElemntT83.usMsgElemntLen) + stMsgElemntT83.usMsgElemntLen;
  stMsgElemntT83.usMsgElemntLen = htons(stMsgElemntT83.usMsgElemntLen);

     //84同步两则
  stIE84.ucRadioId = 1;
  stIE84.ucWlanId = 1;

  MT_GetBssID(vulApIndex,stIE84.ucRadioId,stIE84.ucWlanId,stIE84.aucBSSID);
  stIE84.ucNumOfStaEntries = MT_IE37_IE84_STA_ENTRIES_NUM;//用户实体数目,最大128

  for(ulLoop = 0; ulLoop < stIE84.ucNumOfStaEntries; ulLoop++)
    {
    stIE84.astStaMac[ulLoop].ucMacLen = MT_MAC_LENTH;
    if(vulApIndex == getApIDbyBssid(ulStaIndex))
    {
    	cpss_mem_memcpy(stIE84.astStaMac[ulLoop].aucStaMac,gastStaPara[ulLoop].auStaMac,MT_MAC_LENTH);
    }
    stIE84.astStaMac[ulLoop].aucStaMac[0] = 0x84;
    stIE84.astStaMac[ulLoop].aucStaMac[1] = ulLoop;
    }

  stMsgElemntT84.usMsgElemntType = htons(84);
  stMsgElemntT84.usMsgElemntLen = 9 + stIE84.ucNumOfStaEntries * sizeof(MT_STA_MAC_LIST_T);
  cpss_mem_memcpy(stMsgElemntT84.aucMsgElemntValue,&stIE84,stMsgElemntT84.usMsgElemntLen);
  ulMsgElemntT84Len = sizeof(stMsgElemntT84.usMsgElemntType) + sizeof(stMsgElemntT84.usMsgElemntLen) + stMsgElemntT84.usMsgElemntLen;
  stMsgElemntT84.usMsgElemntLen = htons(stMsgElemntT84.usMsgElemntLen);

  //88  AP连接信息统计，属于AP侧全局统计。从AP启动开始统计。且上报从AP启动开始统计以来的统计总量。
  stIE88.ulApStationAssocSum = htonl(gastAp[vulApIndex].ulStaCnt);//当前与AP关联的终端数
  stIE88.ulApStationOnlineSum = htonl(gastAp[vulApIndex].ulStaCnt);//当前AP下在线的终端数
  stIE88.ulApStationAssocSumMAX = htonl(MT_STA_MAX_NUM_PER_WTP);//在线时段内AP最大附着用户数
  stIE88.ulAuthReqSum = htonl(88);
  stIE88.ulAuthSuccRespSum = htonl(88);
  stIE88.ulAuthFailRespSum = htonl(88);
  stIE88.ulAssocTimes = htonl(88);//关联总次数
  stIE88.ulAssocSucceedTimes = htonl(88);//关联成功总次数
  stIE88.ulAssocFailTimes = htonl(88);//关联失败总次数
  stIE88.ulReassocTimes = htonl(88);//重新关联总次数
  stIE88.ulPreAssCannotShiftDeassocFailSum = htonl(88);//由于之前的关联无法识别与转移而导致重新关联失败的总次数
  stIE88.ulApStatsDisassociated = htonl(88);//终端异常断开连接的总次数
  stIE88.ulAssocRejectSum = htonl(88);//由于接入点资源有限而拒绝关联的总次数
  stIE88.ulBSSNotSupportAssocFailSum = htonl(88);//因终端不支持基本速率集要求的速率而关联失败的总次数
  stIE88.ulReassocSuccessSum = htonl(88);//用户重关联成功次数
  stIE88.ulRejectReassocFailSum = htonl(88);//因拒绝而导致用户重关联失败的次数
  stIE88.ulAuthSuccessSum = htonl(88);//用户鉴别成功次数
  stIE88.ulPwdErrAuthFailSum = htonl(88);//因密码错误而导致用户鉴别失败的次数
  stIE88.ulInvalidAuthFailSum = htonl(88);//因无效而导致用户鉴别失败的次数
  stIE88.ulRejectAuthFailSum = htonl(88);//因拒绝而导致用户鉴别失败的次数
  stIE88.ulDeLinkAuthSum = htonl(88);//解链路验证的次数
  stIE88.ulStaLeaveDeLinkAuthSum = htonl(88);//因用户离开而导致解链路验证的次数
  stIE88.ulInabilityDeLinkAuthSum = htonl(88);//因AP能力不足而导致解链路验证的次数
  stIE88.ulStaAssocTimeSum = htonl(88);//所有曾经关联到此AP的用户的总累计时长，单位为分钟
  stIE88.ulRSSILowAssocFailSum = htonl(88);//因RSSI过低而关联失败的总次数(从AP启动开始统计)
  stIE88.ulConcurrentUsers = htonl(88);//在当前统计时长内有实际数据流量的用户个数
  stIE88.ulConcurrentUsersMAX = htonl(88);//在线时段内AP并发最大用户数
  stIE88.ulTimeoutUserLinkVeryFialedSum = htonl(88);//因超时而导致用户链路验证失败的次数
  stIE88.ulUserLinkVeryFailedSumOtherCause = htonl(88);//其它原因导致用户链路验证失败的次数
  stIE88.ulUserLinkReleaseFailedSum = htonl(88);//因超时而导致用户关联失败的次数
  stIE88.ulUserLinkReReleaseFailedSum = htonl(88);//因超时而导致用户重关联失败的次数
  stIE88.ulUserLinkOtherFailedSum = htonl(88);//其它原因导致用户重关联失败的次数
  stIE88.ulUserAuthTryTimesSum = htonl(88);//用户鉴别尝试次数
  stIE88.ulUserAuthTimeoutFailedSum = htonl(88);//因超时而导致用户鉴别失败的次数
  stIE88.ulUserAuthFailedOtherCauseSum = htonl(88);//其它原因导致用户鉴别失败的次数
  stIE88.ulDelinkVeryExceptionSum = htonl(88);//因异常情况而导致解链路验证的次数
  stIE88.ulExceptionDeAssocSum = htonl(88);//因异常情况而导致解关联的次数
  stIE88.ulOtherCauseDeAssocSum = htonl(88);//其它原因导致解关联的次数
  stIE88.ulInvalidAssocFailSum = htonl(88);//因无效而导致用户关联失败的次数
  stIE88.ulOtherAssocFialeSum = htonl(88);//其它原因导致用户关联失败的次数
  stIE88.ulInvalidReassocFailSum = htonl(88);//因无效而导致用户重关联失败的次数
  stIE88.ulLeaveDisassocSum = htonl(88);//因用户离开而导致解关联的次数
  stIE88.ulinabilityDisassocSum = htonl(88);//因AP能力不足而导致解关联的次数
  stIE88.ulInvalidUserLinkFailSum = htonl(88);//因无效而导致用户链路验证失败的次数
  stIE88.ulRejectUserLinkFailSum = htonl(88);//因拒绝而导致用户链路验证失败的次数
  stIE88.ulOtherDeauthSum = htonl(88);//其它原因导致解链路验证的次数
  stIE88.ulDeAssocSum = htonl(88);//解关联的次数（原名为 DisassocSum）
  stIE88.ulApStaAssocSum_ever = htonl(88);//所有曾经关联到此AP的用户总数
  stIE88.ulRejectassocSum = htonl(88);//因拒绝而导致用户关联失败的次数
  stIE88.ulReassocFailSum = htonl(88);//重关联失败总次数
  stIE88.ulAuthfailSum = htonl(88);//用户鉴别失败次数
  stIE88.ulUserLinkFailSum = htonl(88);//用户链路验证失败次数

  stMsgElemntT88.usMsgElemntType = htons(CW_MSG_ELEMENT_AP_PM_PARA_CMCC_CW_TYPE);
  stMsgElemntT88.usMsgElemntLen = sizeof(MT_IE37_IE88_T);
  cpss_mem_memcpy(stMsgElemntT88.aucMsgElemntValue,&stIE88,stMsgElemntT88.usMsgElemntLen);
  ulMsgElemntT88Len = sizeof(stMsgElemntT88.usMsgElemntType) + sizeof(stMsgElemntT88.usMsgElemntLen) + stMsgElemntT88.usMsgElemntLen;
  stMsgElemntT88.usMsgElemntLen = htons(stMsgElemntT88.usMsgElemntLen);

  //37-80,81,82,83,84,88
  usMsgElemntSuffix = 0;
  stMsgElemntT37.usMsgElemntType = htons(CW_MSG_ELEMENT_VENDOR_SPEC_PAYLOAD_CW_TYPE);

  //ulNetCarrierInc
  cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulNetCarrierInc);

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_80)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT80,ulMsgElemntT80Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT80Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_81)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT81,ulMsgElemntT81Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT81Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_82)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT82,ulMsgElemntT82Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT82Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_83)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT83,ulMsgElemntT83Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT83Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_84)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT84,ulMsgElemntT84Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT84Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_88)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT88,ulMsgElemntT88Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT88Len;
     }

  stMsgElemntT37.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT37Len = sizeof(stMsgElemntT37.usMsgElemntType) + sizeof(stMsgElemntT37.usMsgElemntLen) + stMsgElemntT37.usMsgElemntLen;
  stMsgElemntT37.usMsgElemntLen = htons(stMsgElemntT37.usMsgElemntLen);

  stWtpEventReq.ucPreamble = (UINT8)0;
  stWtpEventReq.ucHdLen = 16;
  stWtpEventReq.aucHdFlag[0] = 3;
  stWtpEventReq.aucHdFlag[1] = 0;
  stWtpEventReq.usFragID = 0;
  stWtpEventReq.usFragOffset = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[0] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[1] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[2] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_WTP_EVENT_REQUEST;
  stWtpEventReq.ucSeqNum = (UINT8)0;
  stWtpEventReq.usMsgElemnetLen = sizeof(stWtpEventReq.usMsgElemnetLen)
                              + (UINT16)(sizeof(stWtpEventReq.ucFlag))
                              + ulMsgElemntT37Len;

  stWtpEventReq.ucFlag = (UINT8)0;

  cpss_mem_memcpy(stWtpEventReq.aucMsgElemntBuf,&stMsgElemntT37,ulMsgElemntT37Len);

  ulWtpEventReqLen = 13 + stWtpEventReq.usMsgElemnetLen;
  stWtpEventReq.usMsgElemnetLen = htons(stWtpEventReq.usMsgElemnetLen);

  if(ulWtpEventReqLen > MT_CAPWAPMSG_BUFF_LEN)
    {
    ulWtpEventReqLen = MT_CAPWAPMSG_BUFF_LEN;
    }

  gstWtpEventReq.ulBufLen = ulWtpEventReqLen;
  cpss_mem_memcpy(gstWtpEventReq.aucBuff,(UINT8*)&stWtpEventReq,gstWtpEventReq.ulBufLen);

  bSendResult = MT_SendApMsg2AC(vulApIndex, glCpmSockId, gstWtpEventReq.aucBuff, gstWtpEventReq.ulBufLen);

  return bSendResult;
}

//WTP EVENT REQ for Bogus AP,,VIT=133-134-138
MTBool ApMutiTestSendEventReq_IE37_IE133_134_138(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;
  UINT32 ulLoop = 0;
  UINT32 ultime = 0;
  CAPWAP_MSG_T stWtpEventReq;
  UINT32 ulWtpEventReqLen = 0;
  MSG_ELEMNT_T stMsgElemntT37;
  UINT32 ulMsgElemntT37Len = 0;

  MSG_ELEMNT_T stMsgElemntT107;
  UINT32 ulMsgElemntT107Len = 0;
  MT_IE37_IE107_T stT107;

  MSG_ELEMNT_T stMsgElemntT132;
  UINT32 ulMsgElemntT132Len = 0;
  MT_IE37_IE132_T stT132;

  MSG_ELEMNT_T stMsgElemntT133;
  UINT32 ulMsgElemntT133Len = 0;
  MT_IE37_IE133_T stT133;

//  UINT8 ucRadioID = 0;
//  UINT8 ucReportCount = 0;//要报告的信道数目
  MSG_ELEMNT_T stMsgElemntT134;
  UINT32 ulMsgElemntT134Len = 0;
  MT_IE37_IE134_T stT134;

  MSG_ELEMNT_T stMsgElemntT138;
  UINT32 ulMsgElemntT138Len = 0;
  MT_IE37_IE138_T stT138;
  UINT32 ulstT138Len = 0;

  UINT16 usMsgElemntSuffix = 0;
  UINT32 ulNetCarrierInc = htonl(gastApconf[vulApIndex].ulNetCarrierInc);
//  UINT8 *pSsid0 = "ESSID_PC_PLAIN_LOCAL0";
//  UINT8 *pSsid1 = "ESSID_PC_PLAIN_LOCAL1";
//  UINT8 *pSsid2 = "ESSID_PC_PLAIN_LOCAL2";
//  UINT8 *pSsid3 = "ESSID_PC_PLAIN_LOCAL3";
//  UINT8 *pSsid4 = "ESSID_PC_PLAIN_LOCAL4";
//  UINT8 *pSsid5 = "ESSID_PC_PLAIN_LOCAL5";
//  UINT8 *pSsid6 = "ESSID_PC_PLAIN_LOCAL6";
//  UINT8 *pSsid7 = "ESSID_PC_PLAIN_LOCAL7";
//  UINT8 *pSsid8 = "ESSID_PC_PLAIN_LOCAL8";
//  UINT8 aucBuf[1024] = {0};
  UINT8 aucTmpp[4] = {0};

  cpss_mem_memset(&stWtpEventReq,0,sizeof(stWtpEventReq));
  cpss_mem_memset(&stMsgElemntT37,0,sizeof(stMsgElemntT37));
  cpss_mem_memset(&stMsgElemntT107,0,sizeof(stMsgElemntT107));
  cpss_mem_memset(&stMsgElemntT132,0,sizeof(stMsgElemntT132));
  cpss_mem_memset(&stMsgElemntT133,0,sizeof(stMsgElemntT133));
  cpss_mem_memset(&stMsgElemntT134,0,sizeof(stMsgElemntT134));
  cpss_mem_memset(&stMsgElemntT138,0,sizeof(stMsgElemntT138));
  cpss_mem_memset(&stT107,0,sizeof(stT107));
  cpss_mem_memset(&stT132,0,sizeof(stT132));
  cpss_mem_memset(&stT133,0,sizeof(stT133));
  cpss_mem_memset(&stT134,0,sizeof(stT134));
  cpss_mem_memset(&stT138,0,sizeof(stT138));

  //107 AP_Last_chgCONFIG_Time
  time((time_t*)&ultime);
  //printf("Current time = %s\n", asctime(localtime(&ultime)));
  stT107.ulAPLastChgConfigTime = htonl(ultime);

  stMsgElemntT107.usMsgElemntType = htons(107);
  stMsgElemntT107.usMsgElemntLen = 4;
  cpss_mem_memcpy(stMsgElemntT107.aucMsgElemntValue,&stT107,stMsgElemntT107.usMsgElemntLen);

  ulMsgElemntT107Len = stMsgElemntT107.usMsgElemntLen + sizeof(stMsgElemntT107.usMsgElemntLen) + sizeof(stMsgElemntT107.usMsgElemntType);
  stMsgElemntT107.usMsgElemntLen = htons(stMsgElemntT107.usMsgElemntLen);

  //Channel Bind TLV（IE=132）
  stT132.ucRadioID = 1;//Radio标识
  stT132.usScanSrc = htons(0);//扫描原因，即哪些事件触发的扫描，目前未定义，值始终为0。
  stT132.usDeviceType = htons(0);//扫描范围，目前未定义，值始终为0。
  stT132.ucMaxCycles = 255;//扫描循环次数。值255表示持续进行循环扫描。
  stT132.ucChnnlCnt = 13;//

  for(ulLoop = 0; ulLoop < stT132.ucChnnlCnt; ulLoop++)
     {
    stT132.astScanChnnlSet[ulLoop].usChnnlId = htons((UINT16)ulLoop);//参数携带了要扫描的信道列表
    stT132.astScanChnnlSet[ulLoop].usFlag = htons(0);
     }

  stMsgElemntT132.usMsgElemntType = htons(CW_MSG_EXT_ELEMENT_SCAN_BIND_TLV);
  stMsgElemntT132.usMsgElemntLen = 12 + stT132.ucChnnlCnt * sizeof(MT_IE132_SCAN_CHNNL_SET_T);
  cpss_mem_memcpy(stMsgElemntT132.aucMsgElemntValue,&stT132,stMsgElemntT132.usMsgElemntLen);

  ulMsgElemntT132Len = sizeof(stMsgElemntT132.usMsgElemntType) + sizeof(stMsgElemntT132.usMsgElemntLen) + stMsgElemntT132.usMsgElemntLen;
  stMsgElemntT132.usMsgElemntLen = htons(stMsgElemntT132.usMsgElemntLen);

  //Vendor Identifier Type:133,Channel Scan Report
  stT133.ucRadioID = 1;
  stT133.ucRptCnt = 10;//要报告的信道数目

  for(ulLoop = 0; ulLoop < stT133.ucRptCnt; ulLoop++)
     {
    stT133.astChnnlScanRpt[ulLoop].usChannelNumber = htons((UINT16) ulLoop);//信道号
    stT133.astChnnlScanRpt[ulLoop].ucRadarStatistics = 1;//该信道上是否检测到雷达，0x00 检测到雷达信号，0x01 未检测到雷达信号
    stT133.astChnnlScanRpt[ulLoop].usMeanTime = htons(133);//信道测量时长，单位ms
    stT133.astChnnlScanRpt[ulLoop].icMeanRSSI = -100;//监听到的各个报文的平均信号强度，单位dbm
    stT133.astChnnlScanRpt[ulLoop].usScreenPacketCount = htons(133);//总计在该信道上监听到的报文个数
    stT133.astChnnlScanRpt[ulLoop].ucNeighborCount = 133;//该信道的邻居个数
    stT133.astChnnlScanRpt[ulLoop].ucMeanNoise = 133;//该信道的平均噪声
    stT133.astChnnlScanRpt[ulLoop].ucInterference = 133;//干扰情况，信道上所有邻居的Station interference + AP interference之和。
    stT133.astChnnlScanRpt[ulLoop].ucSelfTxOccp = 133;//自身发送所占时长，自身报文空口占用时间/监测时间 * 255计算获得。
    stT133.astChnnlScanRpt[ulLoop].ucSelfStaOccp = 133;//自身Station接收所占时长，连接本AP的Station，给本AP发送报文的空口占用时间/监测时间 * 255计算获得。
    stT133.astChnnlScanRpt[ulLoop].ucUnknownOccp = 133;//未知的空口占用时长，未知空口报文的空口占用时间/监测时间 * 255计算获得。
    stT133.astChnnlScanRpt[ulLoop].ucCRCErrCnt = 133;//错误报文个数/报文总个数 * 255 计算获得
    stT133.astChnnlScanRpt[ulLoop].ucDecryptErrCnt = 133;//解密错误报文个数/报文总个数 * 255 计算获得
    stT133.astChnnlScanRpt[ulLoop].ucPhyErrCnt = 133;//物理错误报文个数/报文总个数 * 255 计算获得
    stT133.astChnnlScanRpt[ulLoop].ucRetransCnt = 133;//重传报文报文个数/报文总个数 * 255 计算获得
     }


  stMsgElemntT133.usMsgElemntType = htons(CW_MSG_EXT_ELEMENT_CHANNEL_SCAN_REPORT);
  stMsgElemntT133.usMsgElemntLen = 2 + stT133.ucRptCnt * sizeof(MT_IE133_CHNNL_SCAN_RPT_T);
  cpss_mem_memcpy(stMsgElemntT133.aucMsgElemntValue,&stT133,stMsgElemntT133.usMsgElemntLen);

  ulMsgElemntT133Len = sizeof(stMsgElemntT133.usMsgElemntType) + sizeof(stMsgElemntT133.usMsgElemntLen) + stMsgElemntT133.usMsgElemntLen;
  stMsgElemntT133.usMsgElemntLen = htons(stMsgElemntT133.usMsgElemntLen);

  //Vendor Identifier Type:134,Neighbor AP Report
  stT134.ucRadioID = 1;//Radio标识
  stT134.ucResvd = 0;//保留
  stT134.usNumOfNghbrRprt = 9;//邻居报告的数目

#if 1
  for(ulLoop = 0; ulLoop < stT134.usNumOfNghbrRprt; ulLoop++)
    {
    MT_GetBssID(vulApIndex,stT134.ucRadioID,ulLoop,stT134.astNeighboeInfo[ulLoop].aucBssID);
    stT134.astNeighboeInfo[ulLoop].usChannelNumber = htons(134);
    stT134.astNeighboeInfo[ulLoop].uc11nChannelOffset = 134;
    stT134.astNeighboeInfo[ulLoop].icMeanRssi = -34;
    stT134.astNeighboeInfo[ulLoop].ucStaIntf = 134;
    stT134.astNeighboeInfo[ulLoop].ucApIntf = 134;
    sprintf((char*)aucTmpp,"%d",ulLoop);
    strcpy((char*)stT134.astNeighboeInfo[ulLoop].aucSsid,"IE134_PC_PLAIN_LOCAL");
    strcat((char*)stT134.astNeighboeInfo[ulLoop].aucSsid,(char*)aucTmpp);
    stT134.astNeighboeInfo[ulLoop].ucSSIDLen = strlen((char*)stT134.astNeighboeInfo[ulLoop].aucSsid);
    //cpss_mem_memcpy(stT134.astNeighboeInfo[ulLoop].aucSsid,pSsid0,strlen(pSsid0));
    }
#else
  stT134.astNeighboeInfo[0].aucBssID[0] = 0x01;
  stT134.astNeighboeInfo[0].aucBssID[1] = 0x02;
  stT134.astNeighboeInfo[0].aucBssID[2] = 0x03;
  stT134.astNeighboeInfo[0].aucBssID[3] = 0x04;
  stT134.astNeighboeInfo[0].aucBssID[4] = 0x05;
  stT134.astNeighboeInfo[0].aucBssID[5] = 0x20;
  stT134.astNeighboeInfo[0].usChannelNumber = htons(1);
  stT134.astNeighboeInfo[0].uc11nChannelOffset = 0;
  stT134.astNeighboeInfo[0].icMeanRssi = -10;
  stT134.astNeighboeInfo[0].ucStaIntf = 0;
  stT134.astNeighboeInfo[0].ucApIntf = 0;
  stT134.astNeighboeInfo[0].ucSSIDLen = 21;
  cpss_mem_memcpy(stT134.astNeighboeInfo[0].aucSsid,pSsid0,strlen(pSsid0));

  stT134.astNeighboeInfo[1].aucBssID[0] = 0x01;
  stT134.astNeighboeInfo[1].aucBssID[1] = 0x02;
  stT134.astNeighboeInfo[1].aucBssID[2] = 0x03;
  stT134.astNeighboeInfo[1].aucBssID[3] = 0x04;
  stT134.astNeighboeInfo[1].aucBssID[4] = 0x05;
  stT134.astNeighboeInfo[1].aucBssID[5] = 0x21;
  stT134.astNeighboeInfo[1].usChannelNumber = htons(1);
  stT134.astNeighboeInfo[1].uc11nChannelOffset = 0;
  stT134.astNeighboeInfo[1].icMeanRssi = -9;
  stT134.astNeighboeInfo[1].ucStaIntf = 0;
  stT134.astNeighboeInfo[1].ucApIntf = 0;
  stT134.astNeighboeInfo[1].ucSSIDLen = 21;
  cpss_mem_memcpy(stT134.astNeighboeInfo[1].aucSsid,pSsid1,strlen(pSsid1));

  stT134.astNeighboeInfo[2].aucBssID[0] = 0x01;
  stT134.astNeighboeInfo[2].aucBssID[1] = 0x02;
  stT134.astNeighboeInfo[2].aucBssID[2] = 0x03;
  stT134.astNeighboeInfo[2].aucBssID[3] = 0x04;
  stT134.astNeighboeInfo[2].aucBssID[4] = 0x05;
  stT134.astNeighboeInfo[2].aucBssID[5] = 0x022;
  stT134.astNeighboeInfo[2].usChannelNumber = htons(1);
  stT134.astNeighboeInfo[2].uc11nChannelOffset = 0;
  stT134.astNeighboeInfo[2].icMeanRssi = -6;
  stT134.astNeighboeInfo[2].ucStaIntf = 0;
  stT134.astNeighboeInfo[2].ucApIntf = 0;
  stT134.astNeighboeInfo[2].ucSSIDLen = 21;
  cpss_mem_memcpy(stT134.astNeighboeInfo[2].aucSsid,pSsid2,strlen(pSsid2));

  stT134.astNeighboeInfo[3].aucBssID[0] = 0x1;
  stT134.astNeighboeInfo[3].aucBssID[1] = 0x2;
  stT134.astNeighboeInfo[3].aucBssID[2] = 0x3;
  stT134.astNeighboeInfo[3].aucBssID[3] = 0x4;
  stT134.astNeighboeInfo[3].aucBssID[4] = 0x5;
  stT134.astNeighboeInfo[3].aucBssID[5] = 0x23;
  stT134.astNeighboeInfo[3].usChannelNumber = htons(1);
  stT134.astNeighboeInfo[3].uc11nChannelOffset = 0;
  stT134.astNeighboeInfo[3].icMeanRssi = -25;
  stT134.astNeighboeInfo[3].ucStaIntf = 0;
  stT134.astNeighboeInfo[3].ucApIntf = 0;
  stT134.astNeighboeInfo[3].ucSSIDLen = 21;
  cpss_mem_memcpy(stT134.astNeighboeInfo[3].aucSsid,pSsid3,strlen(pSsid3));

  stT134.astNeighboeInfo[4].aucBssID[0] = 0x01;
  stT134.astNeighboeInfo[4].aucBssID[1] = 0x02;
  stT134.astNeighboeInfo[4].aucBssID[2] = 0x03;
  stT134.astNeighboeInfo[4].aucBssID[3] = 0x04;
  stT134.astNeighboeInfo[4].aucBssID[4] = 0x05;
  stT134.astNeighboeInfo[4].aucBssID[5] = 0x24;
  stT134.astNeighboeInfo[4].usChannelNumber = htons(1);
  stT134.astNeighboeInfo[4].uc11nChannelOffset = 0;
  stT134.astNeighboeInfo[4].icMeanRssi = -7;
  stT134.astNeighboeInfo[4].ucStaIntf = 0;
  stT134.astNeighboeInfo[4].ucApIntf = 0;
  stT134.astNeighboeInfo[4].ucSSIDLen = 21;
  cpss_mem_memcpy(stT134.astNeighboeInfo[4].aucSsid,pSsid4,strlen(pSsid4));

  stT134.astNeighboeInfo[5].aucBssID[0] = 0x01;
  stT134.astNeighboeInfo[5].aucBssID[1] = 0x02;
  stT134.astNeighboeInfo[5].aucBssID[2] = 0x03;
  stT134.astNeighboeInfo[5].aucBssID[3] = 0x04;
  stT134.astNeighboeInfo[5].aucBssID[4] = 0x05;
  stT134.astNeighboeInfo[5].aucBssID[5] = 0x25;
  stT134.astNeighboeInfo[5].usChannelNumber = htons(1);
  stT134.astNeighboeInfo[5].uc11nChannelOffset = 0;
  stT134.astNeighboeInfo[5].icMeanRssi = -2;
  stT134.astNeighboeInfo[5].ucStaIntf = 0;
  stT134.astNeighboeInfo[5].ucApIntf = 0;
  stT134.astNeighboeInfo[5].ucSSIDLen = 21;
  cpss_mem_memcpy(stT134.astNeighboeInfo[5].aucSsid,pSsid5,strlen(pSsid5));

  stT134.astNeighboeInfo[6].aucBssID[0] = 0x01;
  stT134.astNeighboeInfo[6].aucBssID[1] = 0x02;
  stT134.astNeighboeInfo[6].aucBssID[2] = 0x03;
  stT134.astNeighboeInfo[6].aucBssID[3] = 0x04;
  stT134.astNeighboeInfo[6].aucBssID[4] = 0x05;
  stT134.astNeighboeInfo[6].aucBssID[5] = 0x26;
  stT134.astNeighboeInfo[6].usChannelNumber = htons(1);
  stT134.astNeighboeInfo[6].uc11nChannelOffset = 0;
  stT134.astNeighboeInfo[6].icMeanRssi = -2;
  stT134.astNeighboeInfo[6].ucStaIntf = 0;
  stT134.astNeighboeInfo[6].ucApIntf = 0;
  stT134.astNeighboeInfo[6].ucSSIDLen = 21;
  cpss_mem_memcpy(stT134.astNeighboeInfo[6].aucSsid,pSsid6,strlen(pSsid6));

  stT134.astNeighboeInfo[7].aucBssID[0] = 0x01;
  stT134.astNeighboeInfo[7].aucBssID[1] = 0x02;
  stT134.astNeighboeInfo[7].aucBssID[2] = 0x03;
  stT134.astNeighboeInfo[7].aucBssID[3] = 0x04;
  stT134.astNeighboeInfo[7].aucBssID[4] = 0x05;
  stT134.astNeighboeInfo[7].aucBssID[5] = 0x27;
  stT134.astNeighboeInfo[7].usChannelNumber = htons(1);
  stT134.astNeighboeInfo[7].uc11nChannelOffset = 0;
  stT134.astNeighboeInfo[7].icMeanRssi = 0xff;
  stT134.astNeighboeInfo[7].ucStaIntf = 0;
  stT134.astNeighboeInfo[7].ucApIntf = 0;
  stT134.astNeighboeInfo[7].ucSSIDLen = 21;
  cpss_mem_memcpy(stT134.astNeighboeInfo[7].aucSsid,pSsid7,strlen(pSsid7));

  stT134.astNeighboeInfo[8].aucBssID[0] = 0x01;
  stT134.astNeighboeInfo[8].aucBssID[1] = 0x02;
  stT134.astNeighboeInfo[8].aucBssID[2] = 0x03;
  stT134.astNeighboeInfo[8].aucBssID[3] = 0x04;
  stT134.astNeighboeInfo[8].aucBssID[4] = 0x05;
  stT134.astNeighboeInfo[8].aucBssID[5] = 0x28;
  stT134.astNeighboeInfo[8].usChannelNumber = htons(1);
  stT134.astNeighboeInfo[8].uc11nChannelOffset = 0;
  stT134.astNeighboeInfo[8].icMeanRssi = -3;
  stT134.astNeighboeInfo[8].ucStaIntf = 0;
  stT134.astNeighboeInfo[8].ucApIntf = 0;
  stT134.astNeighboeInfo[8].ucSSIDLen = 21;
  cpss_mem_memcpy(stT134.astNeighboeInfo[8].aucSsid,pSsid8,strlen(pSsid8));
#endif

  stMsgElemntT134.usMsgElemntType = htons(CW_MSG_EXT_ELEMENT_NEIGHBOR_AP_REPORT);
  stMsgElemntT134.usMsgElemntLen = sizeof(stT134.ucRadioID) + sizeof(stT134.ucResvd) + sizeof(stT134.usNumOfNghbrRprt) + stT134.usNumOfNghbrRprt * sizeof(MT_IE134_NBR_INFO_T);
  stT134.usNumOfNghbrRprt = htons(stT134.usNumOfNghbrRprt);
  cpss_mem_memcpy(stMsgElemntT134.aucMsgElemntValue,&stT134,stMsgElemntT134.usMsgElemntLen);

  ulMsgElemntT134Len = sizeof(stMsgElemntT134.usMsgElemntType) + sizeof(stMsgElemntT134.usMsgElemntLen) + stMsgElemntT134.usMsgElemntLen;
  stMsgElemntT134.usMsgElemntLen = htons(stMsgElemntT134.usMsgElemntLen);

  //Vendor Identifier Type:138,Rogue AP Report
  stT138.ucRadioId = 1;
  stT138.usNumOfApEntry = MT_T138_AP_ENTRY_NUM;

  ulstT138Len = sizeof(stT138.ucRadioId) + sizeof(stT138.ucResvd) + sizeof(stT138.usNumOfApEntry);

  for(ulLoop = 0; ulLoop < stT138.usNumOfApEntry; ulLoop++)
    {
    stT138.astApEntry[ulLoop].aucApSrcMac[0] = 0x01;
    stT138.astApEntry[ulLoop].aucApSrcMac[1] = 0x38;
    stT138.astApEntry[ulLoop].aucApSrcMac[2] = 0x22;
    stT138.astApEntry[ulLoop].aucApSrcMac[3] = 0x24;
    stT138.astApEntry[ulLoop].aucApSrcMac[4] = rand() % MT_T138_AP_ENTRY_NUM;
    stT138.astApEntry[ulLoop].aucApSrcMac[5] = 1 + (UINT8)ulLoop;

#if 0
    if(254 == gucApSrcMac5)
           {
      gucApSrcMac4++;
      gucApSrcMac5 = 0;
           }
    stT138.astApEntry[ulLoop].aucApSrcMac[4] = gucApSrcMac4;
    stT138.astApEntry[ulLoop].aucApSrcMac[5] = gucApSrcMac5++;
#endif
    stT138.astApEntry[ulLoop].aucApBssidMac[0] = 0x01;
    stT138.astApEntry[ulLoop].aucApBssidMac[1] = 0x38;
    stT138.astApEntry[ulLoop].aucApBssidMac[2] = 0x12;
    stT138.astApEntry[ulLoop].aucApBssidMac[3] = 0x13;
    stT138.astApEntry[ulLoop].aucApBssidMac[4] = 0x14;
    stT138.astApEntry[ulLoop].aucApBssidMac[5] = 1 + (UINT8)ulLoop;
#if 0
    stT138.astApEntry[ulLoop].aucApBssidMac[4] = gucApSrcMac4;
    stT138.astApEntry[ulLoop].aucApBssidMac[5] = gucApSrcMac5;
#endif
    stT138.astApEntry[ulLoop].icRssi = -9;//RSSI
    stT138.astApEntry[ulLoop].usDataRate = htons(12);//Date Rate:被检测到的AP的发送速率。
    stT138.astApEntry[ulLoop].usChannelId = htons(rand()%200);//Channel ID:被检测到的AP所在信道。
    time((time_t*)&ultime);
    stT138.astApEntry[ulLoop].ulLastTimeStampRecvdBeacon = htonl(ultime);//Time Stamp of Last Received Beacon
//    stT138.astApEntry[ulLoop].usRecvdBeacon = htons(2134);//Received Beacon
    time((time_t*)&ultime);
    stT138.astApEntry[ulLoop].ulLastSeenTimeStamp = htonl(ultime);//Time Stamp of Last Seen
    stT138.astApEntry[ulLoop].usBeacomPeriod = htons(100);//Beacon Period

#if 1
    sprintf((char*)aucTmpp,"%d",ulLoop);
    strcpy((char*)stT138.astApEntry[ulLoop].aucSsid,"IE138_PC_PLAIN_LOCAL");
    strcat((char*)stT138.astApEntry[ulLoop].aucSsid,(char*)aucTmpp);

#else
    switch(ulLoop)
           {
    case 0:
      cpss_mem_memcpy(stT138.astApEntry[ulLoop].aucSsid,pSsid0,strlen(pSsid0));//SSID...
      break;

    case 1:
      cpss_mem_memcpy(stT138.astApEntry[ulLoop].aucSsid,pSsid1,strlen(pSsid1));//SSID...
      break;

    case 2:
      cpss_mem_memcpy(stT138.astApEntry[ulLoop].aucSsid,pSsid2,strlen(pSsid2));//SSID...
      break;

    case 3:
      cpss_mem_memcpy(stT138.astApEntry[ulLoop].aucSsid,pSsid3,strlen(pSsid3));//SSID...
      break;

    case 4:
      cpss_mem_memcpy(stT138.astApEntry[ulLoop].aucSsid,pSsid4,strlen(pSsid4));//SSID...
      break;

    default:
      //strcpy(stT138.astApEntry[ulLoop].aucSsid,"UNKNOWN");
      break;
           }
#endif

    stT138.astApEntry[ulLoop].ucType = 1 + rand() % 2;//Type
    stT138.astApEntry[ulLoop].ucEncrypt = 0;//Encrypt:是否加密。
    stT138.astApEntry[ulLoop].ucSpoofType = 17;//Spoof Type:被检测AP发起的攻击类型。值为17表示deauth攻击，18表示disassociation攻击
    stT138.astApEntry[ulLoop].icSnr = -10;
    //stT138.astApEntry[ulLoop].ucApEntryLen = MT_IE138_APENTRY_OFFSET + strlen(stT138.astApEntry[ulLoop].aucSsid);

    stT138.astApEntry[ulLoop].ucApEntryLen = 52;

    //ulstT138Len = ulstT138Len + stT138.astApEntry[ulLoop].ucApEntryLen + sizeof(stT138.astApEntry[ulLoop].ucApEntryLen);
    }

  ulstT138Len = ulstT138Len + stT138.usNumOfApEntry * sizeof(MT_IE138_AP_ENTRY_T);

  stT138.usNumOfStaEntry = MT_T138_STA_ENTRY_NUM;
  //stT138.usNumOfStaEntry = 0;

  for(ulLoop = 0; ulLoop < stT138.usNumOfStaEntry; ulLoop++)
    {
    stT138.astStaEntry[ulLoop].aucStaMac[0] = 0x30;
    stT138.astStaEntry[ulLoop].aucStaMac[1] = 0x31;
    stT138.astStaEntry[ulLoop].aucStaMac[2] = 0x32;
    stT138.astStaEntry[ulLoop].aucStaMac[3] = 0x33;
    stT138.astStaEntry[ulLoop].aucStaMac[4] = rand() % 255;
    stT138.astStaEntry[ulLoop].aucStaMac[5] = 1 + (UINT8)ulLoop;

    stT138.astStaEntry[ulLoop].aucBssid[0] = 0x21;
    stT138.astStaEntry[ulLoop].aucBssid[1] = 0x22;
    stT138.astStaEntry[ulLoop].aucBssid[2] = 0x24;
    stT138.astStaEntry[ulLoop].aucBssid[3] = 0x23;
    stT138.astStaEntry[ulLoop].aucBssid[4] = rand() % 255;
    stT138.astStaEntry[ulLoop].aucBssid[5] = 1 + (UINT8)ulLoop;

    stT138.astStaEntry[ulLoop].usdataRate = htons(10);
    stT138.astStaEntry[ulLoop].usChannelId = htons(ulLoop % 13);
    time((time_t*)&ultime);
    stT138.astStaEntry[ulLoop].ulLastTimeStamp = htonl(ultime);
    stT138.astStaEntry[ulLoop].icRssi = -10;
    stT138.astStaEntry[ulLoop].ucEncrypt = 1;
    }

  ulstT138Len = ulstT138Len + sizeof(stT138.usNumOfStaEntry) + stT138.usNumOfStaEntry * sizeof(MT_IE138_STA_ENTRY_T);

  stMsgElemntT138.usMsgElemntType = htons(CW_MSG_EXT_ELEMENT_ROGUE_AP_REPORT_TLV);
  stMsgElemntT138.usMsgElemntLen = ulstT138Len;

  stT138.usNumOfApEntry = htons(stT138.usNumOfApEntry);
  stT138.usNumOfStaEntry = htons(stT138.usNumOfStaEntry);

  cpss_mem_memcpy(stMsgElemntT138.aucMsgElemntValue,&stT138,stMsgElemntT138.usMsgElemntLen);
  ulMsgElemntT138Len = sizeof(stMsgElemntT138.usMsgElemntType) + sizeof(stMsgElemntT138.usMsgElemntLen) + stMsgElemntT138.usMsgElemntLen;
  stMsgElemntT138.usMsgElemntLen = htons(stMsgElemntT138.usMsgElemntLen);

  //打印AP_MAC，AP_BSSID,AP_SSID
#if 0
  printf("WTP EVENT REQ IE37_IE138 Info:\nAP_ID: %d,AP_MAC: %02x-%02x-%02x-%02x-%02x-%02x\n",vulApIndex,gastAp[vulApIndex].auApMac[0],gastAp[vulApIndex].auApMac[1],
      gastAp[vulApIndex].auApMac[2],gastAp[vulApIndex].auApMac[3],
      gastAp[vulApIndex].auApMac[4],gastAp[vulApIndex].auApMac[5]);
  for(ulLoop = 0; ulLoop < htons(stT138.usNumOfApEntry); ulLoop++)
     {
    printf("AP_ENTRY(%d) SSID: %s\n",ulLoop,stT138.astApEntry[ulLoop].aucSsid);
    printf("AP_ENTRY(%d) BSSID: %02x-%02x-%02x-%02x-%02x-%02x\n\n",ulLoop,stT138.astApEntry[ulLoop].aucApBssidMac[0],stT138.astApEntry[ulLoop].aucApBssidMac[1],
        stT138.astApEntry[ulLoop].aucApBssidMac[2],stT138.astApEntry[ulLoop].aucApBssidMac[3],
        stT138.astApEntry[ulLoop].aucApBssidMac[4],stT138.astApEntry[ulLoop].aucApBssidMac[5]);
     }
  printf("\n");

//  memcpy(aucBuf,&stT138,ulstT138Len);
//
//  for(ulLoop = 0; ulLoop < ulstT138Len; ulLoop++)
//    {
//    printf("%02x ",aucBuf[ulLoop]);
//    if(0 == (ulLoop + 1) % 16)
//          {
//      printf("\n");
//          }
//    }
//  printf("\n");
#endif

  usMsgElemntSuffix = 0;
  stMsgElemntT37.usMsgElemntType = htons(CW_MSG_ELEMENT_VENDOR_SPEC_PAYLOAD_CW_TYPE);

  cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&ulNetCarrierInc,sizeof(ulNetCarrierInc));
  usMsgElemntSuffix = usMsgElemntSuffix + sizeof(ulNetCarrierInc);
#if 0
  cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT132,ulMsgElemntT132Len);
  usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT132Len;
#endif
  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_107)
     {
	    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT107,ulMsgElemntT107Len);
	    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT107Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_133)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT133,ulMsgElemntT133Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT133Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_134)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT134,ulMsgElemntT134Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT134Len;
     }

  if(MT_WTP_EVENT_L2IE_ENABLE == gstWtpEventFlag.ucFlagL2IeId_138)
     {
    cpss_mem_memcpy(&stMsgElemntT37.aucMsgElemntValue[usMsgElemntSuffix],&stMsgElemntT138,ulMsgElemntT138Len);
    usMsgElemntSuffix = usMsgElemntSuffix + ulMsgElemntT138Len;
     }

  stMsgElemntT37.usMsgElemntLen = usMsgElemntSuffix;

  ulMsgElemntT37Len = sizeof(stMsgElemntT37.usMsgElemntType) + sizeof(stMsgElemntT37.usMsgElemntLen) + stMsgElemntT37.usMsgElemntLen;
  stMsgElemntT37.usMsgElemntLen = htons(stMsgElemntT37.usMsgElemntLen);

  stWtpEventReq.ucPreamble = (UINT8)0;
  stWtpEventReq.ucHdLen = 16;
  stWtpEventReq.aucHdFlag[0] = 3;
  stWtpEventReq.aucHdFlag[1] = 0;
  stWtpEventReq.usFragID = 0;
  stWtpEventReq.usFragOffset = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[0] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[1] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsNum[2] = 0;
  stWtpEventReq.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_WTP_EVENT_REQUEST;
  stWtpEventReq.ucSeqNum = (UINT8)0;
  stWtpEventReq.usMsgElemnetLen = sizeof(stWtpEventReq.usMsgElemnetLen)
                              + (UINT16)(sizeof(stWtpEventReq.ucFlag))
                              + ulMsgElemntT37Len;

  stWtpEventReq.ucFlag = (UINT8)0;

  cpss_mem_memcpy(stWtpEventReq.aucMsgElemntBuf,&stMsgElemntT37,ulMsgElemntT37Len);

  ulWtpEventReqLen = 13 + stWtpEventReq.usMsgElemnetLen;
  stWtpEventReq.usMsgElemnetLen = htons(stWtpEventReq.usMsgElemnetLen);

  gstWtpEventReq.ulBufLen = ulWtpEventReqLen;
  cpss_mem_memcpy(gstWtpEventReq.aucBuff,(UINT8*)&stWtpEventReq,gstWtpEventReq.ulBufLen);

  bSendResult = MT_SendApMsg2AC(vulApIndex, glCpmSockId, gstWtpEventReq.aucBuff, gstWtpEventReq.ulBufLen);

  return bSendResult;
}

MTBool ApMutiTestSendEchoReq(UINT32 vulApIndex)
{
  MTBool bSendResult = MT_FALSE;
  CAPWAP_MSG_T stEchoReq;
  UINT32 ulEchoReqLen = 0;

  if(MT_FALSE == gbTestMod)
     {
    cpss_mem_memset(&stEchoReq,0,sizeof(stEchoReq));

    stEchoReq.ucPreamble = (UINT8)0;
    stEchoReq.ucHdLen = 16;
    stEchoReq.aucHdFlag[0] = 3;
    stEchoReq.aucHdFlag[1] = 0;
    stEchoReq.usFragID = 0;
    stEchoReq.usFragOffset = 0;
    stEchoReq.aucMsgTypeEnterPrsNum[0] = 0;
    stEchoReq.aucMsgTypeEnterPrsNum[1] = 0;
    stEchoReq.aucMsgTypeEnterPrsNum[2] = 0;
    stEchoReq.aucMsgTypeEnterPrsSpecific = CW_MSG_TYPE_VALUE_ECHO_REQUEST;
    stEchoReq.ucSeqNum = (UINT8)0;
    stEchoReq.usMsgElemnetLen = sizeof(stEchoReq.usMsgElemnetLen)
                                + (UINT16)(sizeof(stEchoReq.ucFlag));

    stEchoReq.ucFlag = (UINT8)0;
    ulEchoReqLen = 13 + stEchoReq.usMsgElemnetLen;
    stEchoReq.usMsgElemnetLen = htons(stEchoReq.usMsgElemnetLen);

    gstEchoReq.ulBufLen = ulEchoReqLen;
    cpss_mem_memcpy(&gstEchoReq.aucBuff,(UINT8*)&stEchoReq,gstEchoReq.ulBufLen);
      }

  bSendResult = MT_SendApMsg2AC(vulApIndex, glCpmSockId, gstEchoReq.aucBuff,gstEchoReq.ulBufLen);

  if(MT_FALSE == bSendResult)
    {
    printf("ERROR.MT_ApSendMsg2CPM for EchoReq Failure.\nFile:%s.Line:%d\n",__FILE__,__LINE__);
    }

  return bSendResult;
}

//AP DHCPV4 PROC
//Dhcpv4Discover
MTBool ApMutiTestSendDhcpv4Discover(UINT32 vulApIndex)
{
	MTBool bSendResult = MT_FALSE;
//	INT32 lRetCode = 0;
//	UINT8 aucBuff[512] = {0};
//	UINT32 ulBufLen = 0;
	MT_IPV4_HEADER_T stIpv4Hdr;
	MT_UDP_HDR_T stUdpHdr;
	UINT8 aucBtStrap[512] = {0};
	UINT32 ulBtStrapLen = 0;
	MT_BOOTSTRAP_HDR_T stBtStrapHdr;
	MT_DHCP_OPTION_T stOption53;
	UINT32 ulOption53Len = 0;
	MT_DHCP_OPTION_T stOption55;
	UINT32 ulOption55Len = 0;
	MT_DHCP_OPTION_T stEndOption;
	UINT32 ulEndOptionLen = 0;
	UINT8 aucPadding[46] = {0};
	UINT8 ucSubNetMask = 0;
	UINT8 ucBrdCastAddr = 0;
	UINT8 ucTimeOffset = 0;
	UINT8 ucRouter = 0;
	UINT8 ucDomainName = 0;
	UINT8 ucDomainNameSrv = 0;
	UINT8 ucHostName = 0;
	UINT8 ucVendrSpecInfo = 0;
	UINT8 aucSrvHostName[MT_DHCP_SERVER_HOST_NAME_LEN] = {0};
	UINT8 aucBtFileName[MT_DHCP_BOOT_FILE_NAME_LEN] = {0};
	UINT32 ulLoop = 0;
	UINT32 ulSuffix = 0;
	struct sockaddr_in stDestAddr;

	cpss_mem_memset(&stIpv4Hdr,0,sizeof(stIpv4Hdr));
	cpss_mem_memset(&stUdpHdr,0,sizeof(stUdpHdr));
	cpss_mem_memset(&stBtStrapHdr,0,sizeof(stBtStrapHdr));
	cpss_mem_memset(&stOption53,0,sizeof(stOption53));
	cpss_mem_memset(&stOption55,0,sizeof(stOption55));
	cpss_mem_memset(&stEndOption,0,sizeof(stEndOption));
	cpss_mem_memset(&stDestAddr,0,sizeof(stDestAddr));

	stBtStrapHdr.ucMsgType = 1;//Boot Request(1)
	stBtStrapHdr.ucHWType = 1;//Ethernet(1)
	stBtStrapHdr.ucHWAddrLen = 6;//
	stBtStrapHdr.ucHops = 0;
	stBtStrapHdr.ulTransacID = vulApIndex;
	stBtStrapHdr.ulTransacID = htonl(stBtStrapHdr.ulTransacID);
	MT_SetField16(stBtStrapHdr.usBootFlag,0,1,0);//BroadCast Flag:Unicast(0)
	MT_SetField16(stBtStrapHdr.usBootFlag,1,15,0);//Reserved Flag:0
	stBtStrapHdr.usBootFlag = htons(stBtStrapHdr.usBootFlag);

	for(ulLoop = 0; ulLoop < MT_IPV4_ADDR_LEN; ulLoop++)
	{
		stBtStrapHdr.aucClientIpAddr[ulLoop] = 0;
		stBtStrapHdr.aucYourIpAddr[ulLoop] = 0;
		stBtStrapHdr.aucNxtSrvIpAddr[ulLoop] = 0;
		stBtStrapHdr.aucRelayAgntIpAddr[ulLoop] = 0;
	}

	cpss_mem_memcpy(stBtStrapHdr.aucClientMacAddr,gastAp[vulApIndex].auApMac,MT_MAC_LENTH);

	for(ulLoop = 0; ulLoop < MT_HARDWAREPADDING_LEN; ulLoop++)
	{
		stBtStrapHdr.aucHwAddrPadding[ulLoop] = 0x00;
	}

	cpss_mem_memcpy(stBtStrapHdr.aucSrvName,aucSrvHostName,MT_DHCP_SERVER_HOST_NAME_LEN);
	cpss_mem_memcpy(stBtStrapHdr.aucBtFileName,aucBtFileName,MT_DHCP_BOOT_FILE_NAME_LEN);
	stBtStrapHdr.ucMagicCookie[0] = 0x63;
	stBtStrapHdr.ucMagicCookie[1] = 0x82;
	stBtStrapHdr.ucMagicCookie[2] = 0x53;
	stBtStrapHdr.ucMagicCookie[3] = 0x63;

	stOption53.ucOption = 53;//DHCP Message type
	stOption53.ucOptionLen = 1;
	stOption53.aucOptnVal[0] = 1;
	ulOption53Len = sizeof(stOption53.ucOption) + sizeof(stOption53.ucOptionLen) + stOption53.ucOptionLen;

	stOption55.ucOption = 55;
	stOption55.ucOptionLen = 8;

	ucSubNetMask = 1;
	ucBrdCastAddr = 28;
	ucTimeOffset = 2;
	ucRouter = 3;
	ucDomainName = 15;
	ucDomainNameSrv = 6;
	ucHostName = 12;
	ucVendrSpecInfo = 43;

	ulSuffix = 0;
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucSubNetMask,sizeof(ucSubNetMask));
	ulSuffix = ulSuffix + sizeof(ucSubNetMask);
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucBrdCastAddr,sizeof(ucBrdCastAddr));
	ulSuffix = ulSuffix + sizeof(ucBrdCastAddr);
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucTimeOffset,sizeof(ucTimeOffset));
	ulSuffix = ulSuffix + sizeof(ucTimeOffset);
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucRouter,sizeof(ucRouter));
	ulSuffix = ulSuffix + sizeof(ucRouter);
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucDomainName,sizeof(ucDomainName));
	ulSuffix = ulSuffix + sizeof(ucDomainName);
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucDomainNameSrv,sizeof(ucDomainNameSrv));
	ulSuffix = ulSuffix + sizeof(ucDomainNameSrv);
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucHostName,sizeof(ucHostName));
	ulSuffix = ulSuffix + sizeof(ucHostName);
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucVendrSpecInfo,sizeof(ucVendrSpecInfo));
	ulSuffix = ulSuffix + sizeof(ucVendrSpecInfo);

	ulOption55Len = sizeof(stOption55.ucOption) + sizeof(stOption55.ucOptionLen) + stOption55.ucOptionLen;

	stEndOption.ucOption = 0xff;
	stEndOption.ucOptionLen = 0;

	if(0 == stEndOption.ucOptionLen)
	{
		ulEndOptionLen = sizeof(stEndOption.ucOption);
	}
	else
	{
		stEndOption.aucOptnVal[0] = 0;
		ulEndOptionLen = sizeof(stEndOption.ucOption) + sizeof(stEndOption.ucOptionLen) + stEndOption.ucOptionLen;
	}

	ulSuffix = 0;
	cpss_mem_memcpy(&aucBtStrap[ulSuffix],&stBtStrapHdr,sizeof(stBtStrapHdr));
	ulSuffix = ulSuffix + sizeof(stBtStrapHdr);
	cpss_mem_memcpy(&aucBtStrap[ulSuffix],&stOption53,ulOption53Len);
	ulSuffix = ulSuffix + ulOption53Len;
	cpss_mem_memcpy(&aucBtStrap[ulSuffix],&stOption55,ulOption55Len);
	ulSuffix = ulSuffix + ulOption55Len;
	cpss_mem_memcpy(&aucBtStrap[ulSuffix],&stEndOption,ulEndOptionLen);
	ulSuffix = ulSuffix + ulEndOptionLen;
	cpss_mem_memcpy(&aucBtStrap[ulSuffix],aucPadding,sizeof(aucPadding));
	ulSuffix = ulSuffix + sizeof(aucPadding);
	ulBtStrapLen = ulSuffix;

	bSendResult = MT_ApSendMsg2DhcpSrv(vulApIndex, glDhcpSockId, (UINT8*) &aucBtStrap,ulBtStrapLen);

	return bSendResult;
}

//Dhcpv4Req
MTBool ApMutiTestSendDhcpv4Req(UINT32 vulApIndex)
{
	MTBool bSendResult = MT_FALSE;
//	INT32 lRetCode = 0;
//	UINT8 aucBuff[512] = {0};
//	UINT32 ulBufLen = 0;
	MT_IPV4_HEADER_T stIpv4Hdr;
	MT_UDP_HDR_T stUdpHdr;
	UINT8 aucBtStrap[512] = {0};
	UINT32 ulBtStrapLen = 0;
	MT_BOOTSTRAP_HDR_T stBtStrapHdr;
	MT_DHCP_OPTION_T stOption53;
	UINT32 ulOption53Len = 0;
	MT_DHCP_OPTION_T stOption54;
	UINT32 ulOption54Len = 0;
	MT_DHCP_OPTION_T stOption50;
	UINT32 ulOption50Len = 0;
	MT_DHCP_OPTION_T stOption55;
	UINT32 ulOption55Len = 0;
	MT_DHCP_OPTION_T stEndOption;
	UINT32 ulEndOptionLen = 0;
	UINT8 aucPadding[34] = {0};
	UINT8 ucSubNetMask = 0;
	UINT8 ucBrdCastAddr = 0;
	UINT8 ucTimeOffset = 0;
	UINT8 ucRouter = 0;
	UINT8 ucDomainName = 0;
	UINT8 ucDomainNameSrv = 0;
	UINT8 ucHostName = 0;
	UINT8 ucVendrSpecInfo = 0;
	UINT8 aucSrvHostName[MT_DHCP_SERVER_HOST_NAME_LEN] = {0};
	UINT8 aucBtFileName[MT_DHCP_BOOT_FILE_NAME_LEN] = {0};
	UINT32 ulLoop = 0;
	UINT32 ulSuffix = 0;
	struct sockaddr_in stDestAddr;

	cpss_mem_memset(&stIpv4Hdr,0,sizeof(stIpv4Hdr));
	cpss_mem_memset(&stUdpHdr,0,sizeof(stUdpHdr));
	cpss_mem_memset(&stBtStrapHdr,0,sizeof(stBtStrapHdr));
	cpss_mem_memset(&stOption53,0,sizeof(stOption53));
	cpss_mem_memset(&stOption54,0,sizeof(stOption54));
	cpss_mem_memset(&stOption50,0,sizeof(stOption50));
	cpss_mem_memset(&stOption55,0,sizeof(stOption55));
	cpss_mem_memset(&stEndOption,0,sizeof(stEndOption));
	cpss_mem_memset(&stDestAddr,0,sizeof(stDestAddr));

	stBtStrapHdr.ucMsgType = 1;//Boot Request(1)
	stBtStrapHdr.ucHWType = 1;//Ethernet(1)
	stBtStrapHdr.ucHWAddrLen = 6;//
	stBtStrapHdr.ucHops = 0;
	stBtStrapHdr.ulTransacID = vulApIndex;
	stBtStrapHdr.ulTransacID = htonl(stBtStrapHdr.ulTransacID);
	MT_SetField16(stBtStrapHdr.usBootFlag,0,1,0);//BroadCast Flag:Unicast(0)
	MT_SetField16(stBtStrapHdr.usBootFlag,1,15,0);//Reserved Flag:0
	stBtStrapHdr.usBootFlag = htons(stBtStrapHdr.usBootFlag);

	for(ulLoop = 0; ulLoop < MT_IPV4_ADDR_LEN; ulLoop++)
	{
		stBtStrapHdr.aucClientIpAddr[ulLoop] = 0;
		stBtStrapHdr.aucYourIpAddr[ulLoop] = 0;
		stBtStrapHdr.aucNxtSrvIpAddr[ulLoop] = 0;
		stBtStrapHdr.aucRelayAgntIpAddr[ulLoop] = 0;
	}

	cpss_mem_memcpy(stBtStrapHdr.aucClientMacAddr,gastAp[vulApIndex].auApMac,MT_MAC_LENTH);

	for(ulLoop = 0; ulLoop < MT_HARDWAREPADDING_LEN; ulLoop++)
	{
		stBtStrapHdr.aucHwAddrPadding[ulLoop] = 0x00;
	}

	cpss_mem_memcpy(stBtStrapHdr.aucSrvName,aucSrvHostName,MT_DHCP_SERVER_HOST_NAME_LEN);
	cpss_mem_memcpy(stBtStrapHdr.aucBtFileName,aucBtFileName,MT_DHCP_BOOT_FILE_NAME_LEN);
	stBtStrapHdr.ucMagicCookie[0] = 0x63;
	stBtStrapHdr.ucMagicCookie[1] = 0x82;
	stBtStrapHdr.ucMagicCookie[2] = 0x53;
	stBtStrapHdr.ucMagicCookie[3] = 0x63;

	//Option53
	stOption53.ucOption = 53;//DHCP Message type
	stOption53.ucOptionLen = 1;
	stOption53.aucOptnVal[0] = 1;
	ulOption53Len = sizeof(stOption53.ucOption) + sizeof(stOption53.ucOptionLen) + stOption53.ucOptionLen;

	//Option54
	stOption54.ucOption = 54;//DHCP Server Identifier
	stOption54.ucOptionLen = 4;
	//stOption54.aucOptnVal
	ulOption54Len = sizeof(stOption54.ucOption) + sizeof(stOption54.ucOptionLen) + stOption54.ucOptionLen;

	//Option50
	stOption50.ucOption = 50;//Requested IP Address
	stOption50.ucOptionLen = 4;
	//stOption50.aucOptnVal
	ulOption50Len = sizeof(stOption50.ucOption) + sizeof(stOption50.ucOptionLen) + stOption50.ucOptionLen;

	//Option55
	stOption55.ucOption = 55;//Parameter Request List
	stOption55.ucOptionLen = 8;

	ucSubNetMask = 1;
	ucBrdCastAddr = 28;
	ucTimeOffset = 2;
	ucRouter = 3;
	ucDomainName = 15;
	ucDomainNameSrv = 6;
	ucHostName = 12;
	ucVendrSpecInfo = 43;

	ulSuffix = 0;
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucSubNetMask,sizeof(ucSubNetMask));
	ulSuffix = ulSuffix + sizeof(ucSubNetMask);
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucBrdCastAddr,sizeof(ucBrdCastAddr));
	ulSuffix = ulSuffix + sizeof(ucBrdCastAddr);
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucTimeOffset,sizeof(ucTimeOffset));
	ulSuffix = ulSuffix + sizeof(ucTimeOffset);
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucRouter,sizeof(ucRouter));
	ulSuffix = ulSuffix + sizeof(ucRouter);
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucDomainName,sizeof(ucDomainName));
	ulSuffix = ulSuffix + sizeof(ucDomainName);
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucDomainNameSrv,sizeof(ucDomainNameSrv));
	ulSuffix = ulSuffix + sizeof(ucDomainNameSrv);
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucHostName,sizeof(ucHostName));
	ulSuffix = ulSuffix + sizeof(ucHostName);
	cpss_mem_memcpy(&stOption55.aucOptnVal[ulSuffix],&ucVendrSpecInfo,sizeof(ucVendrSpecInfo));
	ulSuffix = ulSuffix + sizeof(ucVendrSpecInfo);

	ulOption55Len = sizeof(stOption55.ucOption) + sizeof(stOption55.ucOptionLen) + stOption55.ucOptionLen;

	stEndOption.ucOption = 0xff;
	stEndOption.ucOptionLen = 0;

	if(0 == stEndOption.ucOptionLen)
	{
		ulEndOptionLen = sizeof(stEndOption.ucOption);
	}
	else
	{
		stEndOption.aucOptnVal[0] = 0;
		ulEndOptionLen = sizeof(stEndOption.ucOption) + sizeof(stEndOption.ucOptionLen) + stEndOption.ucOptionLen;
	}

	ulSuffix = 0;
	cpss_mem_memcpy(&aucBtStrap[ulSuffix],&stBtStrapHdr,sizeof(stBtStrapHdr));
	ulSuffix = ulSuffix + sizeof(stBtStrapHdr);
	cpss_mem_memcpy(&aucBtStrap[ulSuffix],&stOption53,ulOption53Len);
	ulSuffix = ulSuffix + ulOption53Len;
	cpss_mem_memcpy(&aucBtStrap[ulSuffix],&stOption54,ulOption54Len);
	ulSuffix = ulSuffix + ulOption54Len;
	cpss_mem_memcpy(&aucBtStrap[ulSuffix],&stOption50,ulOption50Len);
	ulSuffix = ulSuffix + ulOption50Len;
	cpss_mem_memcpy(&aucBtStrap[ulSuffix],&stOption55,ulOption55Len);
	ulSuffix = ulSuffix + ulOption55Len;
	cpss_mem_memcpy(&aucBtStrap[ulSuffix],&stEndOption,ulEndOptionLen);
	ulSuffix = ulSuffix + ulEndOptionLen;
	cpss_mem_memcpy(&aucBtStrap[ulSuffix],aucPadding,sizeof(aucPadding));
	ulSuffix = ulSuffix + sizeof(aucPadding);
	ulBtStrapLen = ulSuffix;

	bSendResult = MT_ApSendMsg2DhcpSrv(vulApIndex, glDhcpSockId, (UINT8*) &aucBtStrap,ulBtStrapLen);

	return bSendResult;
}

void MT_SndWtpEventMsg(UINT32 vulApIdx)
{
	UINT8 ucWtpEventSelect = 0;
	MTBool bSendResult = MT_FALSE;

	if(vulApIdx < 0 || vulApIdx >= gulCmdStartApNum)//	if(vulApIdx < 0 || vulApIdx >= MT_AP_MAX_NUM)
	{
		return;
	}

	if(MT_WTP_EVENT_ENABLE != gucWtpEventFlag)
	{
		return;
	}
	else
	{
		ucWtpEventSelect = (UINT8)(rand() % 7) + 1;
		switch(ucWtpEventSelect)
		{
		case 1:
			bSendResult = ApMutiTestSendEventReq_IE37_IE56(vulApIdx);
			if(MT_FALSE == bSendResult)
			{
				gstErrMsgStatics.ulSendWTPEventReqErr++;
				gstErrMsgStatics.ulSendEventReq_IE37IE56Err++;
				MT_ApLog(vulApIdx, "ApMutiTestSendEventReq_IE37_IE56 Failure.");
			}
			else
			{
				gstMutiTest_Statics.ulSendWTPEventReq++;
				MT_ApLog(vulApIdx, "ApMutiTestSendEventReq_IE37_IE56 Success.");
			}
			break;

		case 2:
			bSendResult = ApMutiTestSendEventReq_IE37_IE57(vulApIdx);
			if(MT_FALSE == bSendResult)
			{
				gstErrMsgStatics.ulSendWTPEventReqErr++;
				gstErrMsgStatics.ulSendEventReq_IE37IE57Err++;
				MT_ApLog(vulApIdx, "ApMutiTestSendEventReq_IE37_IE57 Failure.");
			}
			else
			{
				gstMutiTest_Statics.ulSendWTPEventReq++;
				MT_ApLog(vulApIdx, "ApMutiTestSendEventReq_IE37_IE57 Success.");
			}
			break;

		case 3:
			bSendResult = ApMutiTestSendEventReq_IE37_IE6X(vulApIdx);
			if(MT_FALSE == bSendResult)
			{
				gstErrMsgStatics.ulSendWTPEventReqErr++;
				gstErrMsgStatics.ulSendEventReq_IE37IE6XErr++ ;
				MT_ApLog(vulApIdx, "ApMutiTestSendEventReq_IE37_IE6X Failure.");
			}
			else
			{
				gstMutiTest_Statics.ulSendWTPEventReq++;
				MT_ApLog(vulApIdx, "ApMutiTestSendEventReq_IE37_IE6X Success.");
			}
			break;

		case 4:
			bSendResult = ApMutiTestSendEventReq_IE37_IE7X(vulApIdx);
			if(MT_FALSE == bSendResult)
			{
				gstErrMsgStatics.ulSendWTPEventReqErr++;
				gstErrMsgStatics.ulSendEventReq_IE37IE7XErr++ ;
				MT_ApLog(vulApIdx, "ApMutiTestSendEventReq_IE37_IE7X Failure.");
			}
			else
			{
				gstMutiTest_Statics.ulSendWTPEventReq++;
				MT_ApLog(vulApIdx, "ApMutiTestSendEventReq_IE37_IE7X Success.");
			}
			break;

		case 5:
			bSendResult = ApMutiTestSendEventReq_IE37_IE8X(vulApIdx);
			if(MT_FALSE == bSendResult)
			{
				gstErrMsgStatics.ulSendWTPEventReqErr++;
				gstErrMsgStatics.ulSendEventReq_IE37IE8XErr++ ;
				MT_ApLog(vulApIdx, "ApMutiTestSendEventReq_IE37_IE8X Failure.");
			}
			else
			{
				gstMutiTest_Statics.ulSendWTPEventReq++;
				MT_ApLog(vulApIdx, "ApMutiTestSendEventReq_IE37_IE8X Success.");
			}
			break;

		case 6:
			bSendResult = ApMutiTestSendEventReq_IE37_IE133_134_138(vulApIdx);
			if(MT_FALSE == bSendResult)
			{
				gstErrMsgStatics.ulSendWTPEventReqErr++;
				gstErrMsgStatics.ulSendEventReq_IE37IE133134138Err++ ;
				MT_ApLog(vulApIdx, "ApMutiTestSendEventReq_IE37_IE133_134_138 Failure.");
			}
			else
			{
				gstMutiTest_Statics.ulSendWTPEventReq++;
				MT_ApLog(vulApIdx, "ApMutiTestSendEventReq_IE37_IE133_134_138 Success.");
			}
			break;

		case 7:
			bSendResult = ApMutiTestSendEventReq_IE37_IE5859(vulApIdx);
			if(MT_FALSE == bSendResult)
			{
				gstErrMsgStatics.ulSendWTPEventReqErr++;
				gstErrMsgStatics.ulSendEventReq_IE37IE5859Err++;
				MT_ApLog(vulApIdx, "ApMutiTestSendEventReq_IE37_IE5859 Failure.");
			}
			else
			{
				gstMutiTest_Statics.ulSendWTPEventReq++;
				MT_ApLog(vulApIdx, "ApMutiTestSendEventReq_IE37_IE5859 Success.");
			}
			break;

		default:
			break;
		}
	}
}


void WtpRelease(UINT32 vulApIdx)
{
	UINT32 ulStaIndex = 0;
	MTBool bSendResult = MT_FALSE;

	if(INIT_STATE == gastAp[vulApIdx].enState)
	{
		return;
	}

	if(MT_TRUE != gbStopFlag)
	{
		bSendResult = ApMutiTestSendResetRsp(vulApIdx);

		if(MT_FALSE == bSendResult)
		{
			gstErrMsgStatics.ulApSendResetErr++;
			MT_ApLog(vulApIdx,"ApMutiTestSendResetRsp Failure.");
			return;
		}

		if(gastAp[vulApIdx].ulEchoReqNum >= MT_WTP_ECHO_DETECT_CNT)
		{
			gstMutiTest_Statics.ulEchoTmOvResetAp++;
			MT_ApLog(vulApIdx, "AP going to shutdown without EchoRsp for MT_WTP_ECHO_DETECT_CNT.");
		}
		else
		{
			gstMutiTest_Statics.ulApSendReset++;
			MT_ApLog(vulApIdx,"ApMutiTestSendResetRsp SUCCESS, AP shutdown.");
		}
	}
	else
	{
		MT_ApLog(vulApIdx,"AP going to shutdown for test-stop.");
	}

	if(RUN_STATE == gastAp[vulApIdx].enState)
	{
		gulOnlineApNum--;

		if(MT_FALSE == gbAPOnlyFlag)
		{
			for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
	    	{
				if(AUTH_INIT_STATE != gastStaPara[ulStaIndex].enAuthState)
	    		{
					if(getApIDbyBssid(ulStaIndex) != vulApIdx)
					{
						continue;
					}

					if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState)
		    		{
						gulOnlineStaCnt--;
		    		}
					else
		    		{
						gstMutiTest_Statics.ulAssociationFail++;
		    		}

					MT_StaLog(ulStaIndex,"STA is going to shutdown.");
					staStateInit(ulStaIndex);
	    		}
	    	}
		}
	}
	else
	{
		gstMutiTest_Statics.ulApUpFailureCount++;
	}

	gastAp[vulApIdx].enState = INIT_STATE;
	gastAp[vulApIdx].bKeepAliveFlag = MT_FALSE;
	gastAp[vulApIdx].ulStaCnt = 0;
	gastAp[vulApIdx].ulEchoReqNum = 0;
	gastAp[vulApIdx].ulPrdTrgCnt = 0;
	gastAp[vulApIdx].ulDiscvCnt = 0;
	gastAp[vulApIdx].stEchoTm.ulBeginTime = 0;
	gastAp[vulApIdx].stEchoTm.ulEndTime = 0;
	cpss_timer_para_delete(gastAp[vulApIdx].ulApHoldTimerId);
	cpss_timer_para_delete(gastAp[vulApIdx].ulEchoTimerId);
	cpss_mem_memset(&gastApWlanInfo[vulApIdx],0,sizeof(MT_AP_RADIO_WLAN_INFO_T));
}

//MT_WTP_ECHO_DETECT_CNT次的ECHO无响应，认为AC已将AP释放，则自动清理本AP下的用户
void MT_WtpEchoChk(void)
{
	UINT32 ulApIdx = 0;

	for(ulApIdx = 0; ulApIdx < MT_AP_MAX_NUM; ulApIdx++)
	{
		if(RUN_STATE == gastAp[ulApIdx].enState)
		{
			if(gastAp[ulApIdx].ulEchoReqNum > MT_WTP_ECHO_DETECT_CNT)
			{
				WtpRelease(ulApIdx);
			}
		}
	}
}

void MT_WtpEchoTimeChk(UINT32 vulApIdx)
{
	UINT32 ulEchoTmLen = 0;
	UINT8 aucTmOvInfo[512] = {0};
	UINT8 aucTmp[4] = {0};

	MTBool bSendResult = MT_FALSE;

	if(RUN_STATE != gastAp[vulApIdx].enState)
	{
		return;
	}

	gastAp[vulApIdx].stEchoTm.ulEndTime = cpss_tick_get();
	ulEchoTmLen = gastAp[vulApIdx].stEchoTm.ulEndTime - gastAp[vulApIdx].stEchoTm.ulBeginTime;

	if(ulEchoTmLen >= (gulEchoInterval * 2))
	{
#if 1
		sprintf((char*)aucTmOvInfo,"In EchoTimeChk, No EchoRsp Be received during %dms.",ulEchoTmLen);
#else
		strcpy((char*)aucTmOvInfo,"In EchoTimeChk, No EchoRsp Be received during (");
		sprintf((char*)aucTmp,"%d",ulEchoTmLen);
		strcat((char*)aucTmOvInfo,(char*)aucTmp);
		strcat((char*)aucTmOvInfo," ms).");
#endif
		MT_ApLog(vulApIdx,(char*)aucTmOvInfo);

		bSendResult = ApMutiTestSendEchoReq(vulApIdx);

		if(MT_TRUE == bSendResult)
		{
			gstMutiTest_Statics.ulApSendEchoReq++;
			gastAp[vulApIdx].ulEchoReqNum++;
			gastAp[vulApIdx].stEchoTm.ulBeginTime = cpss_tick_get();
			gastAp[vulApIdx].stEchoTm.ulEndTime = 0;
		}
		else
		{
			gstErrMsgStatics.ulApSendEchoReqErr++;
		}
	}
}

void MT_ApTmOvReset(UINT32 vulApIdx)
{
	//AP处于初始状态
	if(INIT_STATE == gastAp[vulApIdx].enState)
	{
		return;
	}

	//AP处于其他状态
	MT_ApLog(vulApIdx, "AP Online Time is out.");

	WtpRelease(vulApIdx);
}

void MT_WtpStatusChk(void)
{
	UINT32 ulApIdx = 0;

	for(ulApIdx = 0; ulApIdx < MT_AP_MAX_NUM; ulApIdx++)
	{
		if(INIT_STATE != gastAp[ulApIdx].enState)
		{
			if(RUN_STATE == gastAp[ulApIdx].enState)
		    {
				MT_WtpEchoTimeChk(ulApIdx);
		    }
			else
		    {
				gstMutiTest_Statics.ulApUpFailureCount++;
				MT_ApLog(ulApIdx,"AP Online Failure, and Be set to INIT_STATE.");
				WtpRelease(ulApIdx);
		    }
		}
	}
}
