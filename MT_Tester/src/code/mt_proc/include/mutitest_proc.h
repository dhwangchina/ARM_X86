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
#ifndef _MUTITEST_PROC_H_
#define _MUTITEST_PROC_H_

#include <stdlib.h>
#include "smss_public.h"
#include "cpss_public.h"
#include "mt_common.h"

//MT的状态
#define MT_POWERON          ((UINT32)0)
#define MT_WORKING          ((UINT32)2)
#define MT_ERROR            ((UINT32)0)
#define MT_SUCCESS          ((UINT32)1)

#define MT_START_CMD_MSG     0x96000001
#define MT_STOP_CMD_MSG      0x96000002

#define MT_STA_TUNNEL_MOD_4TO4    0
#define MT_STA_TUNNEL_MOD_4IN6    1
#define MT_STA_TUNNEL_MOD_6IN4    2
#define MT_STA_TUNNEL_MOD_6TO6    3

extern void MT_SysConfApIpOnEth0(void);
extern void MT_GetArpByPingAC(void);
extern UINT32 MT_GetApIndexFromApMac(char* vpApMacStr);
extern UINT32 MT_GetApIndexFromApIp(char* vpApIpStr);
extern UINT32 MT_GetStaIndexFromStaMac(char* vpApMacStr);
extern void showcapwapimgproc(UINT32 vulApIndex);
extern void showapinfo(void);                 //统计AP的状态信息
extern void showaplist(void);
extern void showstalist(void);
extern void count(void);                      //显示AP的统计信息
extern void MT_ShowStatistic(void);              //运行过程中的消息统计函数
extern void MT_ShowErrMsg(void);

extern void MT_GetApConf(UINT32 *vpulApCount);//获取AP的配置信息
extern void MT_GetAcInfo(void);               //AC配置信息
extern void MT_GetSysConf(void);              //获取系统配置信息
extern void MT_GetStaConf(void);              //获取STA配置信息
extern void MT_StaInstanceInit(UINT32 vulApIndex);//指定AP所属STA的初始化函数
extern void MT_ApInstanceInit(UINT32 vulApIndex);//AP的初始化函数
extern void MT_ApParaInit(void);//AP参数初始化
extern void MT_StaParaInit(void);//STA参数初始化

extern void MT_SysParaInit(void);             //系统初始化函数
extern void MT_ProcStart(UINT32 vulCmdStartApNum,UINT32 vulOnlineApNum);//测试启动函数
extern void MT_Task_Cpm(void);        //INIT SOCKET,INIT MSG RECEIVED TASK
extern void MT_Task_Upm(void);        //INIT SOCKET,INIT MSG RECEIVED TASK
extern void MT_Task_ApDhcp(void);     //INIT SOCKET,INIT MSG RECEIVED TASK
extern void MT_PowerOn(CPSS_COM_MSG_HEAD_T * vpstMsgHead);
extern void MT_Working(CPSS_COM_MSG_HEAD_T *pstMsgHead);

extern void start(UINT32 vulApNum,UINT32 vulStaNumPerAp, UINT32 vulSecond);
extern void stop(void);
extern void MT_StartParaCheck(UINT32 vulApNum,UINT32 vulStaNumPerAp,UINT32 vulSecond);//START参数校验
extern void MT_StartTest(void);
extern void showsysconfpara(void);
extern void showglobalpara(void);
extern void MT_ZeroPara(void);
extern void mainprocmsgcnt(void);

extern void test_5246_5247_msg_cnt(UINT32 vulApIdx);
extern void MT_ChangIfMsgQueueLen(void);
extern void startlog(void);
extern void stoplog(void);
//CAPWAP头信息分析
/*
                        0                                     1                                         2                                        3
0    1    2   3   4   5   6   7    8   9   0    1    2    3   4   5   6   7   8   9   0    1   2   3   4   5    6   7    8   9   0   1
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
First32含义说明:
MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,0);//Version
MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,0);//Type
MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,4);//HLEN：表示CAPWAP协议头的长度（包括可选项），类似IP头长度定义，HLEN乘以4即CAPWAP协议头真实长度（CAPWAP协议头是4字节对齐的）。
MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,1);//RID：5位数据，表示Radio ID，用于指出报文是与哪个Radio关联，便于AP预处理；0<=RID<=31
MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,1);//WBID：5位数据，表示Radio支持的无线技术，目前支持为如下值:  0->保留, 1->IEEE 802.11,2->保留,  3 ->EPCGlobal
MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,1);//T：为0表示payload是802.3帧，为1表示payload按照WBID指示。
MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,0);//F：为0表示报文未分片，为1表示报文是分片
MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,0);//L：当F位为1时，本标志位才有意义。为1表示报文为最后一个分片，反之则不是。
MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,1);//W：为1表示协议头中有“Wireless Specific Information”选项，反之则没有。
MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,0);//M：为1表示协议头中有“Radio MAC Address”选项，反之则没有。
MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,0);//K：为1表示当前报文为数据隧道的保活（keep-alive）报文，反之则不是。
MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,0);//Flags：保留，必须为0。
*/
#endif //_MUTITEST_PROC_H_
