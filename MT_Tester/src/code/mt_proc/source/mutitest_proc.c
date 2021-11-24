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

#include "mutitest_proc.h"
#include "ap_proc.h"
#include "sta_proc.h"
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>


#include <sys/socket.h>         //socket
#include <string.h>             //memset
#include <arpa/inet.h>          //inet_xxx
#include <sys/ioctl.h>          //ioctl
#include <errno.h>
#include <unistd.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/netdevice.h>  //net_device_stats
#include <net/if_arp.h>
#include <netinet/if_ether.h>
#include <netinet/ether.h>
#include <net/route.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <linux/if.h>
#include <sys/stat.h>

MTBool gbAPOnlyFlag = MT_FALSE;//AP在线不带用户标识
MTBool gbStopFlag = MT_FALSE;  //停止测试标识
MTBool gbApAlwaysOnline = MT_FALSE; //AP永远在线,0：AP自行下线，1：AP永远在线
MTBool gbTestMod = MT_FALSE;   //测试模式,是否采用消息模板，1：采用消息模板，0：程序自行构建消息，参数从AP配置信息中获取
MTBool gbStaDhcpFlag = MT_FALSE;
MTBool gbLogFlag = MT_FALSE;
UINT32 gulOnlineApNum = 0;     //在线AP数目统计
UINT32 gulApOnlnCnt = 0;     //在线AP数目统计
UINT32 gulOnlineStaCnt = 0;    //在线STA数目统计
UINT32 gulSysConfAPCnt = 0;    //AP配置信息中AP的总数
UINT16 gusSequenceNum = 0;
extern MTBool gbShowFlag;//周期性统计信息打印开关
UINT32 gulPeriodShowTime = 1000;//(ms)
UINT32 gulEchoInterval = 60000; //(ms)
UINT32 gulPeriodTriggerTime = 0;
UINT32 gulPeriodTriggerApNum = 0;

extern UINT32 gulCmdStartApNum;
UINT32 gulStaNumPerAp = 0; /*Station number per WTP*/
UINT32 gulStaHoldTime = 0; /*STA HOLD TIME*/
UINT32 gulApHoldTime = 0; /*AP HOLD TIME*/
UINT32 gulTestStopTime = 10;//Stop test time(s)
UINT32 gulStaHoTrgrPrd = 0;//STA切换触发周期
UINT32 gulStaMaxNum = 0;//MT_SYS_STA_CAPABILITY
extern UINT32 gulStaID;
UINT32 gulStaNumPerSecond = 0;
UINT32 gulApNumPerSecond = 0;

UINT32 gulIpAddrFlag = 0;//IP Addr Flag
UINT8  gaucAcIpAddrV4[MT_IPV4_ADDR_LEN];//AC的IP地址
UINT8  gaucAcIpAddrV6[MT_IPV6_ADDR_LEN];//AC的IP地址
UINT8  gaucStaGwIpV4Addr[MT_IPV4_ADDR_LEN];//SRV的IP地址
UINT8  gaucStaGwIpV6Addr[MT_IPV6_ADDR_LEN];//SRV的IP地址
UINT8  gaucStaGwMacAddr[MT_MAC_LENTH] = {0};//SRV的MAC地址
UINT8  gucStaTunnelMod;//STAs隧道模式

UINT8 gucStaAccessPlcy = 0xff;//用户接入策略标识

extern MT_AP_RADIO_WLAN_INFO_T gastApWlanInfo[MT_AP_MAX_NUM];//AP WLAN INFO RECORD
MT_RUNTIME_T gstRunTime;//持续运行时间

//MAC1，MAC2，MAC3
//设计下面几种测试用例：
//1：MAC2取值正确，MAC1与MAC3值不同；
//2：MAC2取值正确，MAC1与MAC3相同，但值错误；
//3：MAC2取值错误，不是任何在线STA，MAC1与MAC3相同且正确；
//4：MAC2取值错误，不是任何在线STA，MAC1与MAC3相同且错误；
//5：MAC2取值正确，MAC1与MAC3相同且值正确；
extern UINT8 gaucApUpdateFilename[100];
extern UINT16 gusApFileNameLen;
extern UINT8 gucWtpEventFlag;
extern MT_CAPWAPMSG_T gst80211AuthReq;
extern MT_CAPWAPMSG_T gst80211AssocReq;
extern MT_CAPWAPMSG_T gst80211DeAuthReq;

extern MT_CAPWAPMSG_T gstDiscoveryReq;
extern MT_CAPWAPMSG_T gstJoinReq;
extern MT_CAPWAPMSG_T gstConfigurationStatusReq;
extern MT_CAPWAPMSG_T gstChangeStateReq;
extern MT_CAPWAPMSG_T gstConfigurationUpdateRsp;
extern MT_CAPWAPMSG_T gstAddWlanRsp;
extern MT_CAPWAPMSG_T gstStationConfigurationRsp;
extern MT_CAPWAPMSG_T gstConfigUpdateRsp_RunStat;
extern MT_CAPWAPMSG_T gstCapwapImageReq;
extern MT_CAPWAPMSG_T gstCapwapImageRsp;
extern MT_CAPWAPMSG_T gstWtpEventReq_DelSta;
extern MT_CAPWAPMSG_T gstWtpEventReq;
extern MT_CAPWAPMSG_T gstResetRsp;
extern MT_CAPWAPMSG_T gstKeepAlive;
extern MT_CAPWAPMSG_T gstEchoReq;

void MT_ZeroPara(void)
{
  gulOnlineApNum = 0;
  gulOnlineStaCnt = 0;
  gusSequenceNum = 0;
  cpss_mem_memset(&gstMutiTest_Statics,0,sizeof(MUTITEST_STATICS_T));
  cpss_mem_memset(&gstErrMsgStatics,0,sizeof(MT_ERR_MSG_STATICS_T));
}

void showsysconfpara(void)
{
  printf("\n****System Para In /mt_conf/sysconf.conf.***\n");
  printf("TriggerTime:-----> %d\n",gstMtSysConf.ulTriggerTime);
  printf("ApTriggerNum:----> %d\n",gstMtSysConf.ulApTriggerNum);
  printf("StaTriggerNum:---> %d\n",gstMtSysConf.ulStaTriggerNum);
  printf("ApHoldTime:------> %d\n",gstMtSysConf.ulApHoldTime);
  printf("STA_Policy:------> %d\n",gstMtSysConf.ucStaPolicy);
  printf("NetDevName:------> %s\n",gstMtSysConf.aucDevName);
  printf("StaHoTrgrPrd:----> %d\n",gstMtSysConf.ulStaHoTrgrPeriod);//STA切换触发周期

  if(MT_FALSE == gstMtSysConf.ulApOnlineFlag)
     {
    printf("ApOnlineMode-----> %s(%d)\n","AutoOffline",gstMtSysConf.ulApOnlineFlag);//AP永远在线,0：AP自行下线，1：AP永远在线
     }
  else if(MT_TRUE == gstMtSysConf.ulApOnlineFlag)
     {
    printf("ApOnlineMode-----> %s(%d)\n","AlwaysOnline",gstMtSysConf.ulApOnlineFlag);//AP永远在线,0：AP自行下线，1：AP永远在线
     }
  else
     {
    printf("ApOnlineMode-----> %s(%d)\n","Unknown",gstMtSysConf.ulApOnlineFlag);//AP永远在线,0：AP自行下线，1：AP永远在线
     }

  if(0 == gstMtSysConf.ulTestMod)
    {
    printf("TestMod:---------> %s(%d)\n","PARA_CONF",gstMtSysConf.ulTestMod);//测试模式，0：程序自行构建消息，参数从AP配置信息中获取,1：采用消息模板，
    }
  else if(1 == gstMtSysConf.ulTestMod)
    {
    printf("TestMod:---------> %s(%d)\n","MSG_POOL",gstMtSysConf.ulTestMod);//测试模式，0：程序自行构建消息，参数从AP配置信息中获取,1：采用消息模板，
    }
  else
    {
    printf("TestMod:---------> %s(%d)\n","Unknown",gstMtSysConf.ulTestMod);//测试模式，0：程序自行构建消息，参数从AP配置信息中获取,1：采用消息模板，
    }

  printf("****System Para In /mt_conf/sysconf.conf.***\n");
}

void showglobalpara(void)
{
  printf("\n******Global Parameter in System******\n");
  printf("gbAPOnlyFlag:-------------> %d\n",gbAPOnlyFlag);//MTBool gbAPOnlyFlag = MT_FALSE;//AP在线不带用户标识
  printf("gbStopFlag:---------------> %d\n",gbStopFlag);//gbStopFlag = MT_FALSE;  //停止测试标识
  if(MT_FALSE == gbApAlwaysOnline)
     {
    printf("gbApAlwaysOnline:---------> %s\n","AutoOffline");//AP永远在线,0：AP自行下线，1：AP永远在线
     }
  else if(MT_TRUE == gbApAlwaysOnline)
    {
    printf("gbApAlwaysOnline:---------> %s\n","ApAlwaysOnline");//AP永远在线,0：AP自行下线，1：AP永远在线
    }
  else
     {
    printf("gbApAlwaysOnline:---------> %s\n","Unknown");//AP永远在线,0：AP自行下线，1：AP永远在线
     }

  if(MT_FALSE == gbTestMod)
    {
    printf("gbTestMod:----------------> %s\n","PARACONF");//测试模式，0：程序自行构建消息，参数从AP配置信息中获取
    }
  else if(MT_TRUE == gbTestMod)
    {
    printf("gbTestMod:----------------> %s\n","MSGPOOL");//测试模式，1：采用消息模板
    }
  else
    {
    printf("gbTestMod:----------------> %s\n","Unknown");
    }
  printf("gulOnlineApNum:-----------> %d\n",gulOnlineApNum);//gulOnlineApNum = 0;     //在线AP数目统计
  printf("gulOnlineStaCnt:----------> %d\n",gulOnlineStaCnt);//gulOnlineStaCnt;        //在线STA数目统计
  printf("gulSysConfAPCnt:----------> %d\n",gulSysConfAPCnt);//gulSysConfAPCnt = 0;    //AP配置信息中AP的总数
  printf("gulPeriodShowTime:--------> %d\n",gulPeriodShowTime);//gulPeriodShowTime = 1000;//(ms)
  printf("gulPeriodTriggerTime:-----> %d\n",gulPeriodTriggerTime);//gulPeriodTriggerTime = 0;
  printf("gulPeriodTriggerApNum:----> %d\n",gulPeriodTriggerApNum);//gulPeriodTriggerApNum = 0;
  printf("gulPeriodTriggerStaNum:---> %d\n",gulPeriodTriggerStaNum);//gulPeriodTriggerStaNum = 0;
  printf("gulStaHoTrgrPrd:----------> %d\n",gulStaHoTrgrPrd);//STA切换触发周期
  printf("Sta Access Policy---------> %d\n",gstMtSysConf.ucStaPolicy);
  printf("gulCmdStartApNum:---------> %d\n",gulCmdStartApNum);//gulCmdStartApNum = 0;/*default value*/
  printf("gulStaNumPerAp:-----------> %d\n",gulStaNumPerAp);//gulStaNumPerAp = 0; /*Station number per WTP*/
  printf("gulStaHoldTime:-----------> %d\n",gulStaHoldTime);//gulStaHoldTime = 0; /*STA HOLD TIME*/
  printf("gulApHoldTime:------------> %d\n",gulApHoldTime);//gulApHoldTime = 0; /*AP HOLD TIME*/
  printf("gulStaNumPerSecond:-------> %d\n",gulStaNumPerSecond);//gulStaNumPerSecond = 0;
  printf("gulApNumPerSecond:--------> %d\n",gulApNumPerSecond);//gulApNumPerSecond = 0;
  printf("******Global Parameter in System******\n");
}

UINT32 MT_GetCapwapMsgPool(void)
{
  UINT32 retCode = MT_SUCCESS;
  char *pMsgPathFileName = NULL;

  pMsgPathFileName = "/mt_conf/MT_AP_DiscoveryReq_MSG.txt";
  cpss_mem_memset(&gstDiscoveryReq,0,sizeof(gstDiscoveryReq));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstDiscoveryReq);

  pMsgPathFileName = "/mt_conf/MT_AP_JoinReq_MSG.txt";
  cpss_mem_memset(&gstJoinReq,0,sizeof(gstJoinReq));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstJoinReq);

  pMsgPathFileName = "/mt_conf/MT_AP_ConfigurationStatusReq_MSG.txt";
  cpss_mem_memset(&gstConfigurationStatusReq,0,sizeof(gstConfigurationStatusReq));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstConfigurationStatusReq);

  pMsgPathFileName = "/mt_conf/MT_AP_ChangeStateReq_MSG.txt";
  cpss_mem_memset(&gstChangeStateReq,0,sizeof(gstChangeStateReq));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstChangeStateReq);

  pMsgPathFileName = "/mt_conf/MT_AP_ConfigurationUpdateRsp_MSG.txt";
  cpss_mem_memset(&gstConfigurationUpdateRsp,0,sizeof(gstConfigurationUpdateRsp));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstConfigurationUpdateRsp);

  pMsgPathFileName = "/mt_conf/MT_AP_AddWlanRsp_MSG.txt";
  cpss_mem_memset(&gstAddWlanRsp,0,sizeof(gstAddWlanRsp));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstAddWlanRsp);

  pMsgPathFileName = "/mt_conf/MT_AP_StationConfigurationRsp_MSG.txt";
  cpss_mem_memset(&gstStationConfigurationRsp,0,sizeof(gstStationConfigurationRsp));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstStationConfigurationRsp);

  pMsgPathFileName = "/mt_conf/MT_AP_ConfigUpdateRsp_RunStat_MSG.txt";
  cpss_mem_memset(&gstConfigUpdateRsp_RunStat,0,sizeof(gstConfigUpdateRsp_RunStat));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstConfigUpdateRsp_RunStat);

  pMsgPathFileName = "/mt_conf/MT_AP_CapwapImageReq_MSG.txt";
  cpss_mem_memset(&gstCapwapImageReq,0,sizeof(gstCapwapImageReq));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstCapwapImageReq);

  pMsgPathFileName = "/mt_conf/MT_AP_CapwapImageRsp_MSG.txt";
  cpss_mem_memset(&gstCapwapImageRsp,0,sizeof(gstCapwapImageRsp));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstCapwapImageRsp);

  pMsgPathFileName = "/mt_conf/MT_AP_WtpEventReq_DelSta_MSG.txt";
  cpss_mem_memset(&gstWtpEventReq_DelSta,0,sizeof(gstWtpEventReq_DelSta));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstWtpEventReq_DelSta);

  pMsgPathFileName = "/mt_conf/MT_AP_WtpEventReq_MSG.txt";
  cpss_mem_memset(&gstWtpEventReq,0,sizeof(gstWtpEventReq));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstWtpEventReq);

  pMsgPathFileName = "/mt_conf/MT_AP_ResetRsp_MSG.txt";
  cpss_mem_memset(&gstResetRsp,0,sizeof(gstResetRsp));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstResetRsp);

  pMsgPathFileName = "/mt_conf/MT_AP_KeepAlive_MSG.txt";
  cpss_mem_memset(&gstKeepAlive,0,sizeof(gstKeepAlive));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstKeepAlive);

  pMsgPathFileName = "/mt_conf/MT_AP_EchoReq_MSG.txt";
  cpss_mem_memset(&gstEchoReq,0,sizeof(gstEchoReq));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gstEchoReq);

  pMsgPathFileName = "/mt_conf/MT_STA_80211AuthReq_MSG.txt";
  cpss_mem_memset(&gst80211AuthReq,0,sizeof(gst80211AuthReq));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gst80211AuthReq);

  pMsgPathFileName = "/mt_conf/MT_STA_AssocReq_MSG.txt";
  cpss_mem_memset(&gst80211AssocReq,0,sizeof(gst80211AssocReq));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gst80211AssocReq);

  pMsgPathFileName = "/mt_conf/MT_STA_DeAssocReq_MSG.txt";
  cpss_mem_memset(&gst80211DeAuthReq,0,sizeof(gst80211DeAuthReq));
  MT_GetCapwapMsg((UINT8*)pMsgPathFileName,&gst80211DeAuthReq);

  return retCode;
}

void showstalist(void)
{
  UINT32 ulStaIndex = 0;
  UINT32 ulStaOnlnCnt = 0;
  UINT8 aucStaStateStr[MT_STATE_BUF_LEN] = {'\0'};

  for(ulStaIndex = 0; ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
     {
	  if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState)
	     {
		  cpss_mem_memset(aucStaStateStr,0,MT_STATE_BUF_LEN);
		  strcpy((char*)aucStaStateStr,(char*)MT_GetStaHOStateStr(gastStaPara[ulStaIndex].enHandOverState));

		  printf("ApIndex: %d. StaIndex: %d. StaMac: %02x:%02x:%02x:%02x:%02x:%02x. STA_IPv4ADDR: %d.%d.%d.%d. STA_LOCAl_AP: %d, H_AP: %d. F_AP: %d. STA_HO_STATE: %s.\n\n",
				  getApIDbyBssid(ulStaIndex),ulStaIndex,
  gastStaPara[ulStaIndex].auStaMac[0],
  gastStaPara[ulStaIndex].auStaMac[1],
  gastStaPara[ulStaIndex].auStaMac[2],
  gastStaPara[ulStaIndex].auStaMac[3],
  gastStaPara[ulStaIndex].auStaMac[4],
  gastStaPara[ulStaIndex].auStaMac[5],
  gastStaPara[ulStaIndex].aucStaIpV4Addr[0],
  gastStaPara[ulStaIndex].aucStaIpV4Addr[1],
  gastStaPara[ulStaIndex].aucStaIpV4Addr[2],
  gastStaPara[ulStaIndex].aucStaIpV4Addr[3],
  gastStaPara[ulStaIndex].ulStaLocalApId,
  gastStaPara[ulStaIndex].ulHomeApId,
  gastStaPara[ulStaIndex].ulForeignApId,
  aucStaStateStr);
		  ulStaOnlnCnt++;
      }
     }
	printf("\n");
	printf("STA should Online Num: %d.\n",gulOnlineStaCnt);
	printf("Total STA Online Count:%d\n",ulStaOnlnCnt);
}

void showaplist(void)
{
  UINT32 ulApIndex = 0;
  UINT32 ulApOnLnCnt = 0;
  UINT32 ulStaOnlnCnt = 0;

  for(ulApIndex = 0; ulApIndex < gulSysConfAPCnt; ulApIndex++)
     {
    if(RUN_STATE == gastAp[ulApIndex].enState)
          {
      printf("AP_ID: %4d. AP_IPv4Addr: %d.%d.%d.%d. AP_MAC: %02x:%02x:%02x:%02x:%02x:%02x. EchoReq_Num: %d,STA_NUM: %d.\n",ulApIndex,
          gastAp[ulApIndex].aucApIpV4[0],
          gastAp[ulApIndex].aucApIpV4[1],
          gastAp[ulApIndex].aucApIpV4[2],
          gastAp[ulApIndex].aucApIpV4[3],
          gastAp[ulApIndex].auApMac[0],
          gastAp[ulApIndex].auApMac[1],
          gastAp[ulApIndex].auApMac[2],
          gastAp[ulApIndex].auApMac[3],
          gastAp[ulApIndex].auApMac[4],
          gastAp[ulApIndex].auApMac[5],
          gastAp[ulApIndex].ulEchoReqNum,
          gastAp[ulApIndex].ulStaCnt);
      ulApOnLnCnt++;
      ulStaOnlnCnt = ulStaOnlnCnt + gastAp[ulApIndex].ulStaCnt;
          }
    }
  printf("\n");
  printf("AP should Online Num: %d.STA should online Num:%d\n",gulOnlineApNum,gulOnlineStaCnt);
  printf("AP Online Count: %d.STA online Count:%d\n",ulApOnLnCnt,ulStaOnlnCnt);
}
//运行过程中的消息统计函数
void MT_ShowStatistic(void)
{
  UINT32 ulRunTime = 0;
  UINT32 ulStaOnLnCnt = 0;

  ulStaOnLnCnt = staOnlineCnt();
  gulApOnlnCnt = apOnlineCnt();

  gstRunTime.ulEndTime = cpss_tick_get();
  ulRunTime = (gstRunTime.ulEndTime - gstRunTime.ulBeginTime)/1000;

  printf("\n======================Statistic Count Begin========================\n");
  printf("AP STATISTIC:                             ");
  printf("RUNTIME:%4dD:%02dH:%02dM:%02dS\n",(ulRunTime / 3600) / 24,(ulRunTime / 3600) % 24,(ulRunTime / 60) % 60,ulRunTime % 60);

  printf("SendDiscoverReq--------> %-10d",gstMutiTest_Statics.ulSendDiscoverReq);
  printf("RecvDiscoverRsp--------> %-10d",gstMutiTest_Statics.ulRecvDiscoverRsp);
  printf("Delta:%d\n",gstMutiTest_Statics.ulSendDiscoverReq - gstMutiTest_Statics.ulRecvDiscoverRsp);

  printf("SendJoinReq------------> %-10d",gstMutiTest_Statics.ulSendJoinReq);
  printf("RecvJoinRsp------------> %-10d",gstMutiTest_Statics.ulRecvJoinRsp);
  printf("Delta:%d\n",gstMutiTest_Statics.ulSendJoinReq - gstMutiTest_Statics.ulRecvJoinRsp);

  printf("SendConfigStatusReq----> %-10d",gstMutiTest_Statics.ulSendConfigStatusReq);
  printf("RecvConfigStatusRsp----> %-10d",gstMutiTest_Statics.ulRecvConfigStatusRsp);
  printf("Delta:%d\n",gstMutiTest_Statics.ulSendConfigStatusReq - gstMutiTest_Statics.ulRecvConfigStatusRsp);

  printf("SendChangeStateReq-----> %-10d",gstMutiTest_Statics.ulSendChangeStateReq);
  printf("RecvChangeStateRsp-----> %-10d",gstMutiTest_Statics.ulRecvChangeStateRsp);
  printf("Delta:%d\n",gstMutiTest_Statics.ulSendChangeStateReq - gstMutiTest_Statics.ulRecvChangeStateRsp);

  printf("RecvConfigUpdateReq----> %-10d",gstMutiTest_Statics.ulRecvConfigUpdateReq);
  printf("SendConfigUpdateRsp----> %-10d",gstMutiTest_Statics.ulSendConfigUpdateRsp);
  printf("Delta:%d\n",gstMutiTest_Statics.ulRecvConfigUpdateReq - gstMutiTest_Statics.ulSendConfigUpdateRsp);

  printf("RecvAddWlanReq---------> %-10d",gstMutiTest_Statics.ulRecvAddWlanReq);
  printf("SendAddWlanRsp---------> %-10d",gstMutiTest_Statics.ulSendAddWlanRsp);
  printf("Delta:%d\n",gstMutiTest_Statics.ulRecvAddWlanReq - gstMutiTest_Statics.ulSendAddWlanRsp);

  printf("ApRecvResetReq---------> %-10d",gstMutiTest_Statics.ulApRecvResetReq);
  printf("APSendResetRsp---------> %-10d",gstMutiTest_Statics.ulAPSendResetRsp);
  printf("Delta:%d\n",gstMutiTest_Statics.ulApRecvResetReq - gstMutiTest_Statics.ulAPSendResetRsp);

  printf("SendWTPEventReq--------> %-10d",gstMutiTest_Statics.ulSendWTPEventReq);
  printf("RecvWtpEventRsp--------> %-10d",gstMutiTest_Statics.ulRecvWtpEventRsp);
  printf("Delta:%d\n",gstMutiTest_Statics.ulSendWTPEventReq - gstMutiTest_Statics.ulRecvWtpEventRsp);

  printf("ApSendEchoReq----------> %-10u",gstMutiTest_Statics.ulApSendEchoReq);
  printf("ApRecvEchoRsp----------> %-10u",gstMutiTest_Statics.ulApRecvEchoRsp);
  printf("Delta:%d\n",gstMutiTest_Statics.ulApSendEchoReq - gstMutiTest_Statics.ulApRecvEchoRsp);

  printf("ApSendKeepAlive--------> %-10u",gstMutiTest_Statics.ulApSendKeepAlive);
  printf("ApRecvKeepAlive--------> %-10u",gstMutiTest_Statics.ulApRecvKeepAlive);
  printf("Delta:%d\n",gstMutiTest_Statics.ulApSendKeepAlive - gstMutiTest_Statics.ulApRecvKeepAlive);

  printf("ApSendReset------------> %-10d",gstMutiTest_Statics.ulApSendReset);
  printf("EchoTmOvResetAp--------> %-10d\n",gstMutiTest_Statics.ulEchoTmOvResetAp);
  printf("ApOnlineFailCnt--------> %-10d",gstMutiTest_Statics.ulApUpFailureCount);
  printf("ApOnlineSuccessCnt-----> %-10d\n",gstMutiTest_Statics.ulApUpSuccessCount);
  //printf("OnlineApCnt------------> %-10d\n",gulOnlineApNum);
  printf("OnlineApCnt------------> %-10d\n",gulApOnlnCnt);//for a moment.

  /* Only Ap Up do NOT Need Sta Information! */
  if(MT_FALSE == gbAPOnlyFlag)
     {
    printf("\nSTA STATISTIC:\n");
    printf("Send80211AuthReq-------> %-10d",gstMutiTest_Statics.ulSend80211AuthReq);
    printf("Recv80211AuthRsp-------> %-10d",gstMutiTest_Statics.ulRecv80211AuthRsp);
    printf("Delta:%d\n",gstMutiTest_Statics.ulSend80211AuthReq - gstMutiTest_Statics.ulRecv80211AuthRsp);

    printf("Send80211AssocReq------> %-10d",gstMutiTest_Statics.ulSend80211AssocReq);
    printf("Recv80211AssocRsp------> %-10d",gstMutiTest_Statics.ulRecv80211AssocRsp);
    printf("Delta:%d\n",gstMutiTest_Statics.ulSend80211AssocReq - gstMutiTest_Statics.ulRecv80211AssocRsp);

    printf("RecvAddStaConfigReq----> %-10d",gstMutiTest_Statics.ulRecvAddStaConfigReq);
    printf("SendAddStaConfigRsp----> %-10d",gstMutiTest_Statics.ulSendAddStaConfigRsp);
    printf("Delta:%d\n",gstMutiTest_Statics.ulRecvAddStaConfigReq - gstMutiTest_Statics.ulSendAddStaConfigRsp);

    printf("Send80211AuthReqHO-----> %-10d",gstMutiTest_Statics.ulSend80211AuthReqHO);
    printf("Recv80211AuthRspHO-----> %-10d",gstMutiTest_Statics.ulRecv80211AuthRspHO);
    printf("Delta:%d\n",gstMutiTest_Statics.ulSend80211AuthReqHO - gstMutiTest_Statics.ulRecv80211AuthRspHO);

    printf("Send80211ReAssocReq----> %-10d",gstMutiTest_Statics.ulSend80211ReAssocReq);
    printf("Recv80211ReAssocRsp----> %-10d",gstMutiTest_Statics.ulRecv80211ReAssocRsp);
    printf("Delta:%d\n",gstMutiTest_Statics.ulSend80211ReAssocReq - gstMutiTest_Statics.ulRecv80211ReAssocRsp);

    printf("RecvAddStaConfigReqHO--> %-10d",gstMutiTest_Statics.ulRecvAddStaConfigReqHO);
    printf("SendAddStaConfigRspHO--> %-10d",gstMutiTest_Statics.ulSendAddStaConfigRspHO);
    printf("Delta:%d\n",gstMutiTest_Statics.ulRecvAddStaConfigReqHO - gstMutiTest_Statics.ulSendAddStaConfigRspHO);

    printf("RecvDelStaConfigReqHO--> %-10d",gstMutiTest_Statics.ulRecvDelStaConfigReqHO);
    printf("SendDelStaConfigRspHO--> %-10d",gstMutiTest_Statics.ulSendDelStaConfigRspHO);
    printf("Delta:%d\n",gstMutiTest_Statics.ulRecvDelStaConfigReqHO - gstMutiTest_Statics.ulSendDelStaConfigRspHO);

    if(IP_ADDR_TYPE_V4 == gulIpAddrFlag)
           {
      printf("SendDhcpDiscover-------> %-10d",gstMutiTest_Statics.ulStaSendDhcpDiscover);
      printf("RcvDhcpOffer-----------> %-10d",gstMutiTest_Statics.ulStaRcvDhcpOffer);
      printf("Delta:%d\n",gstMutiTest_Statics.ulStaSendDhcpDiscover - gstMutiTest_Statics.ulStaRcvDhcpOffer);

      printf("SendDhcpReq------------> %-10d",gstMutiTest_Statics.ulStaSendDhcpReq);
      printf("RcvDhcpAck-------------> %-10d",gstMutiTest_Statics.ulStaRcvDhcpAck);
      printf("Delta:%d\n",gstMutiTest_Statics.ulStaSendDhcpReq - gstMutiTest_Statics.ulStaRcvDhcpAck);

      printf("SendArpReq-------------> %-10d",gstMutiTest_Statics.ulStaSendArpReq);
      printf("RecvArpReply-----------> %-10d",gstMutiTest_Statics.ulStaRcvArpReply);
      printf("Delta:%d\n",gstMutiTest_Statics.ulStaSendArpReq - gstMutiTest_Statics.ulStaRcvArpReply);

      printf("RecvArpReq-------------> %-10d",gstMutiTest_Statics.ulStaRcvArpReq);
      printf("SendArpReply-----------> %-10d",gstMutiTest_Statics.ulStaSendArpReply);
      printf("Delta:%d\n",gstMutiTest_Statics.ulStaRcvArpReq - gstMutiTest_Statics.ulStaSendArpReply);

      printf("SendPingReq------------> %-10d",gstMutiTest_Statics.ulStaSendPingReq);
      printf("RecvPingReply----------> %-10d",gstMutiTest_Statics.ulStaRcvPingReply);
      printf("Delta:%d\n",gstMutiTest_Statics.ulStaSendPingReq - gstMutiTest_Statics.ulStaRcvPingReply);

      printf("RecvPingReq------------> %-10d",gstMutiTest_Statics.ulStaRecvPingReq);
      printf("SendPingReply----------> %-10d",gstMutiTest_Statics.ulStaSendPingReply);
      printf("Delta:%d\n",gstMutiTest_Statics.ulStaRecvPingReq - gstMutiTest_Statics.ulStaSendPingReply);
           }
    else
          {
      printf("SendDhcpv6Solicit------> %-10d",gstMutiTest_Statics.ulStaSendDhcpv6Solicit);
      printf("RcvDhcpv6Advertise-----> %-10d",gstMutiTest_Statics.ulStaRcvDhcpv6Advertise);
      printf("Delta:%d\n",gstMutiTest_Statics.ulStaSendDhcpv6Solicit - gstMutiTest_Statics.ulStaRcvDhcpv6Advertise);

      printf("SendDhcpv6Req----------> %-10d",gstMutiTest_Statics.ulStaSendDhcpv6Req);
      printf("RcvDhcpv6Reply---------> %-10d",gstMutiTest_Statics.ulStaRcvDhcpv6Reply);
      printf("Delta:%d\n",gstMutiTest_Statics.ulStaSendDhcpv6Req - gstMutiTest_Statics.ulStaRcvDhcpv6Reply);

      printf("SendPing6Req-----------> %-10d",gstMutiTest_Statics.ulStaSendPing6Req);
      printf("RecvPing6Reply---------> %-10d",gstMutiTest_Statics.ulStaRcvPing6Reply);
      printf("Delta:%d\n",gstMutiTest_Statics.ulStaSendPing6Req - gstMutiTest_Statics.ulStaRcvPing6Reply);

      printf("RcvPing6Req------------> %-10d",gstMutiTest_Statics.ulStaRcvPing6Req);
      printf("SendPing6Reply---------> %-10d",gstMutiTest_Statics.ulStaSendPing6Reply);
      printf("Delta:%d\n",gstMutiTest_Statics.ulStaRcvPing6Req - gstMutiTest_Statics.ulStaSendPing6Reply);
          }
    printf("SendDeAuthCnt----------> %-10d",gstMutiTest_Statics.ulSendDeAuth);
    printf("RecvDeAuthCnt----------> %-10d\n",gstMutiTest_Statics.ulRecvDeAuth);

    printf("RecvDelStaConfigReq----> %-10d",gstMutiTest_Statics.ulRecvDelStaConfigReq);
    printf("SendDelStaConfigRsp----> %-10d",gstMutiTest_Statics.ulSendDelStaConfigRsp);
    printf("Delta:%d\n",gstMutiTest_Statics.ulRecvDelStaConfigReq - gstMutiTest_Statics.ulSendDelStaConfigRsp);

    printf("AssocFailCnt-----------> %-10d",gstMutiTest_Statics.ulAssociationFail);
    printf("AssocTmOutCnt----------> %-10d\n",gstMutiTest_Statics.ulAssoctnTmOut);
    printf("ReAssocSucCnt----------> %-10d",gstMutiTest_Statics.ulReAssociationSuccess);
    printf("ReAssocFailCnt---------> %-10d\n",gstMutiTest_Statics.ulReAssociationFail);
    printf("AssocSucCnt------------> %-10d",gstMutiTest_Statics.ulAssociationSuccess);
    //printf("StaOnlineCnt-----------> %-10d\n",gulOnlineStaCnt);
    printf("StaOnlineCnt-----------> %-10d\n",ulStaOnLnCnt);
    }
  printf("======================Statistic Count End==========================\n");
}

//错误消息统计
void MT_ShowErrMsg(void)
{
	printf("ERROR Messages Statistics.\n");
	printf("SendDiscoverReqErr--------> %-10d",gstErrMsgStatics.ulSendDiscoverReqErr);
	printf("RecvDiscoverRspErr--------> %-10d\n",gstErrMsgStatics.ulRecvDiscoverRspErr);
	printf("SendJoinReqErr------------> %-10d",gstErrMsgStatics.ulSendJoinReqErr);
	printf("RecvJoinRspErr------------> %-10d\n",gstErrMsgStatics.ulRecvJoinRspErr);
	printf("SendConfigStatusReqErr----> %-10d",gstErrMsgStatics.ulSendConfigStatusReqErr);
	printf("RecvConfigStatusRspErr----> %-10d\n",gstErrMsgStatics.ulRecvConfigStatusRspErr);
	printf("SendChangeStateReqErr-----> %-10d",gstErrMsgStatics.ulSendChangeStateReqErr);
	printf("RecvChangeStateRspErr-----> %-10d\n",gstErrMsgStatics.ulRecvChangeStateRspErr);
	printf("RecvConfigUpdateReqErr----> %-10d",gstErrMsgStatics.ulRecvConfigUpdateReqErr);
	printf("SendConfigUpdateRspErr----> %-10d\n",gstErrMsgStatics.ulSendConfigUpdateRspErr);
	printf("RecvAddWlanReqErr---------> %-10d",gstErrMsgStatics.ulRecvAddWlanReqErr);
	printf("SendAddWlanRspErr---------> %-10d\n",gstErrMsgStatics.ulSendAddWlanRspErr);
	printf("SendIamgeDataReqErr-------> %-10d",gstErrMsgStatics.ulSendIamgeDataReqErr);
	printf("SendIamgeDataRspErr-------> %-10d\n",gstErrMsgStatics.ulSendIamgeDataRspErr);
	printf("ApRecvResetReqErr---------> %-10d",gstErrMsgStatics.ulApRecvResetReqErr);
	printf("APSendResetRspErr---------> %-10d\n",gstErrMsgStatics.ulAPSendResetRspErr);
	printf("SendWTPEventReqErr--------> %-10d",gstErrMsgStatics.ulSendWTPEventReqErr);
	printf("RecvWtpEventRspErr--------> %-10d\n",gstErrMsgStatics.ulRecvWtpEventRspErr);
	printf("ApSendEchoReqErr----------> %-10d",gstErrMsgStatics.ulApSendEchoReqErr);
	printf("ApRecvEchoRspErr----------> %-10d\n",gstErrMsgStatics.ulApRecvEchoRspErr);
	printf("ApSendKeepAliveErr--------> %-10d",gstErrMsgStatics.ulApSendKeepAliveErr);
	printf("ApRecvKeepAliveErr--------> %-10d\n",gstErrMsgStatics.ulApRecvKeepAliveErr);
	printf("ApUpFailureCountErr-------> %-10d",gstErrMsgStatics.ulApUpFailureCountErr);
	printf("ApSendResetErr------------> %-10d\n",gstErrMsgStatics.ulApSendResetErr);

	printf("SndEvntReq_IE37IE56Err----> %-10d",gstErrMsgStatics.ulSendEventReq_IE37IE56Err);
	printf("SndEvntReq_IE37IE57Err----> %-10d\n",gstErrMsgStatics.ulSendEventReq_IE37IE57Err);
	printf("SndEvntReq_IE37IE6XErr----> %-10d",gstErrMsgStatics.ulSendEventReq_IE37IE6XErr);
	printf("SndEvntReq_IE37IE7XErr----> %-10d\n",gstErrMsgStatics.ulSendEventReq_IE37IE7XErr);
	printf("SndEvntReq_IE37IE8XErr----> %-10d",gstErrMsgStatics.ulSendEventReq_IE37IE8XErr);
	printf("SndEvntReq_IE37IE13XErr---> %-10d\n",gstErrMsgStatics.ulSendEventReq_IE37IE133134138Err);
	printf("SndEvntReq_IE37IE5859Err--> %-10d\n",gstErrMsgStatics.ulSendEventReq_IE37IE5859Err);

	printf("Send80211AuthReqErr-------> %-10d",gstErrMsgStatics.ulSend80211AuthReqErr);
	printf("Recv80211AuthRspErr-------> %-10d\n",gstErrMsgStatics.ulRecv80211AuthRspErr);
	printf("Send80211AssocReqErr------> %-10d",gstErrMsgStatics.ulSend80211AssocReqErr);
	printf("Recv80211AssocRspErr------> %-10d\n",gstErrMsgStatics.ulRecv80211AssocRspErr);
	printf("RecvAddStaConfigReqErr----> %-10d",gstErrMsgStatics.ulRecvAddStaConfigReqErr);
	printf("SendAddStaConfigRspErr----> %-10d\n",gstErrMsgStatics.ulSendAddStaConfigRspErr);
	printf("Send80211AuthReqHOErr-----> %-10d",gstErrMsgStatics.ulSend80211AuthReqHOErr);
	printf("Recv80211AuthRspHOErr-----> %-10d\n",gstErrMsgStatics.ulRecv80211AuthRspHOErr);
	printf("Send80211ReAssocReqErr----> %-10d",gstErrMsgStatics.ulSend80211ReAssocReqErr);
	printf("Recv80211ReAssocRspErr----> %-10d\n",gstErrMsgStatics.ulRecv80211ReAssocRspErr);
	printf("RecvAddStaConfigReqHOErr--> %-10d",gstErrMsgStatics.ulRecvAddStaConfigReqHOErr);
	printf("SendAddStaConfigRspHOErr--> %-10d\n",gstErrMsgStatics.ulSendAddStaConfigRspHOErr);
	printf("RecvDelStaConfigReqHOErr--> %-10d",gstErrMsgStatics.ulRecvDelStaConfigReqHOErr);
	printf("SendDelStaConfigRspHOErr--> %-10d\n",gstErrMsgStatics.ulSendDelStaConfigRspHOErr);
	printf("RecvDelStaConfigReqErr----> %-10d",gstErrMsgStatics.ulRecvDelStaConfigReqErr);
	printf("SendDelStaConfigRspErr----> %-10d\n",gstErrMsgStatics.ulSendDelStaConfigRspErr);
	printf("SendDhcpDiscoverErr-------> %-10d",gstErrMsgStatics.ulSendDhcpDiscoverErr);
	printf("RcvDhcpOfferErr-----------> %-10d\n",gstErrMsgStatics.ulRcvDhcpOfferErr);
	printf("SendDhcpReqErr------------> %-10d",gstErrMsgStatics.ulSendDhcpReqErr);
	printf("RcvDhcpAckErr-------------> %-10d\n",gstErrMsgStatics.ulRcvDhcpAckErr);
	printf("SendArpReqErr-------------> %-10d",gstErrMsgStatics.ulSendArpReqErr);
	printf("RecvArpReplyErr-----------> %-10d\n",gstErrMsgStatics.ulRecvArpReplyErr);
	printf("RecvArpReqErr-------------> %-10d",gstErrMsgStatics.ulRecvArpReqErr);
	printf("SendArpReplyErr-----------> %-10d\n",gstErrMsgStatics.ulSendArpReplyErr);
	printf("SendPingReqErr------------> %-10d",gstErrMsgStatics.ulSendPingReqErr);
	printf("RecvPingReplyErr----------> %-10d\n",gstErrMsgStatics.ulRecvPingReplyErr);
	printf("RecvPingReqErr------------> %-10d",gstErrMsgStatics.ulRecvPingReqErr);
	printf("SendPingReplyErr----------> %-10d\n",gstErrMsgStatics.ulSendPingReplyErr);
	printf("SendDhcpv6SolicitErr------> %-10d",gstErrMsgStatics.ulSendDhcpv6SolicitErr);
	printf("RcvDhcpv6AdvertiseErr-----> %-10d\n",gstErrMsgStatics.ulRcvDhcpv6AdvertiseErr);
	printf("SendDhcpv6ReqErr----------> %-10d",gstErrMsgStatics.ulSendDhcpv6ReqErr);
	printf("RcvDhcpv6ReplyErr---------> %-10d\n",gstErrMsgStatics.ulRcvDhcpv6ReplyErr);
	printf("SendPing6ReqErr-----------> %-10d",gstErrMsgStatics.ulSendPing6ReqErr);
	printf("RecvPing6ReplyErr---------> %-10d\n",gstErrMsgStatics.ulRcvPing6ReplyErr);
	printf("RcvPing6ReqErr------------> %-10d",gstErrMsgStatics.ulRcvPing6ReqErr);
	printf("SendPing6ReplyErr---------> %-10d\n",gstErrMsgStatics.ulSendPing6ReplyErr);

	printf("RecvDeAuthErr-------------> %-10d",gstErrMsgStatics.ulRecvDeAuthErr);
	printf("SendDeAuthErr-------------> %-10d\n",gstErrMsgStatics.ulSendDeAuthErr);

	printf("Rcv5246UnKnwnMsg----------> %-10d",gstErrMsgStatics.ulRcv5246UnknwnMsg);
	printf("Rcv5247UnKnwnMsg----------> %-10d\n",gstErrMsgStatics.ulRcv5247UnknwnMsg);

	printf("Rcv5246MsgErr-------------> %-10d",gstErrMsgStatics.ulRcv5246MsgErr);
	printf("Rcv5247MsgErr-------------> %-10d\n",gstErrMsgStatics.ulRcv5247MsgErr);

	printf("Rc5246MsgHeadNULL---------> %-10d",gstErrMsgStatics.ul5246MsgHead_NULL);
	printf("Rc5247MsgHeadNULL---------> %-10d\n",gstErrMsgStatics.ul5247MsgHead_NULL);

	printf("Rcv5246MsgErrApId---------> %-10d",gstErrMsgStatics.ulRcv5246MsgErrApId);
	printf("Rcv5246MsgErrStaId--------> %-10d\n",gstErrMsgStatics.ulRcv5246MsgErrStaId);

	printf("Rcv5247MsgErrApId---------> %-10d",gstErrMsgStatics.ulRcv5247MsgErrApId);
	printf("Rcv5247MsgErrStaId--------> %-10d\n",gstErrMsgStatics.ulRcv5247MsgErrStaId);

	printf("RcvOtherMsgErr------------> %-10d\n",gstErrMsgStatics.ulRcvOtherMsgErr);
}

void count(void)
{
  MT_ShowStatistic();
  MT_ShowErrMsg();
  mainprocmsgcnt();
}

void showapinfo(void)
{
  UINT32 ulApIndex = 0;
  UINT32 ulOnlineApNum = 0;
  UINT32 ulNonInitApNum = 0;
  UINT8 aucApStateStr[MT_STATE_BUF_LEN] = {'\0'};

  printf("\n");
  for (ulApIndex = 0; ulApIndex < gulSysConfAPCnt; ulApIndex++)
    {
	  cpss_mem_memset(aucApStateStr,0,MT_STATE_BUF_LEN);
	  strcpy((char*)aucApStateStr,(char*)MT_GetApStateStr(gastAp[ulApIndex].enState));

    if (INIT_STATE == gastAp[ulApIndex].enState)
          {
    	printf("AP_ID: %d,AP_MAC: %02x:%02x:%02x:%02x:%02x:%02x. AP_STATE: %s\n",ulApIndex,
    			            gastAp[ulApIndex].auApMac[0], gastAp[ulApIndex].auApMac[1],
    			            gastAp[ulApIndex].auApMac[2], gastAp[ulApIndex].auApMac[3],
    			            gastAp[ulApIndex].auApMac[4], gastAp[ulApIndex].auApMac[5],
    			            aucApStateStr);
      ulNonInitApNum++;
           }

    if (WAIT_DISCOVER_RSP_STATE == gastAp[ulApIndex].enState)
          {
    	printf("AP_ID: %d,AP_MAC: %02x:%02x:%02x:%02x:%02x:%02x. AP_STATE: %s\n",ulApIndex,
    			            gastAp[ulApIndex].auApMac[0], gastAp[ulApIndex].auApMac[1],
    			            gastAp[ulApIndex].auApMac[2], gastAp[ulApIndex].auApMac[3],
    			            gastAp[ulApIndex].auApMac[4], gastAp[ulApIndex].auApMac[5],
    			            aucApStateStr);
      ulNonInitApNum++;
           }

    if (WAIT_JOIN_RSP_STATE == gastAp[ulApIndex].enState)
          {
    	printf("AP_ID: %d, AP_MAC: %02x:%02x:%02x:%02x:%02x:%02x. AP_STATE: %s\n",ulApIndex,
    			            gastAp[ulApIndex].auApMac[0], gastAp[ulApIndex].auApMac[1],
    			            gastAp[ulApIndex].auApMac[2], gastAp[ulApIndex].auApMac[3],
    			            gastAp[ulApIndex].auApMac[4], gastAp[ulApIndex].auApMac[5],
    			            aucApStateStr);
      ulNonInitApNum++;
           }

    if (WAIT_CONFIG_STATUS_RSP_STATE == gastAp[ulApIndex].enState)
          {
    	printf("AP_ID: %d, AP_MAC: %02x:%02x:%02x:%02x:%02x:%02x. AP_STATE: %s\n",ulApIndex,
    			            gastAp[ulApIndex].auApMac[0], gastAp[ulApIndex].auApMac[1],
    			            gastAp[ulApIndex].auApMac[2], gastAp[ulApIndex].auApMac[3],
    			            gastAp[ulApIndex].auApMac[4], gastAp[ulApIndex].auApMac[5],
    			            aucApStateStr);
      ulNonInitApNum++;
           }

    if (WAIT_CHANGE_STATE_RSP_STATE == gastAp[ulApIndex].enState)
          {
    	printf("AP_ID: %d, AP_MAC: %02x:%02x:%02x:%02x:%02x:%02x. AP_STATE: %s\n",ulApIndex,
    			            gastAp[ulApIndex].auApMac[0], gastAp[ulApIndex].auApMac[1],
    			            gastAp[ulApIndex].auApMac[2], gastAp[ulApIndex].auApMac[3],
    			            gastAp[ulApIndex].auApMac[4], gastAp[ulApIndex].auApMac[5],
    			            aucApStateStr);
      ulNonInitApNum++;
           }

    if (RUN_STATE == gastAp[ulApIndex].enState)
          {
    	printf("AP_ID: %d, AP_MAC: %02x:%02x:%02x:%02x:%02x:%02x. AP_STATE: %s\n",ulApIndex,
    			            gastAp[ulApIndex].auApMac[0], gastAp[ulApIndex].auApMac[1],
    			            gastAp[ulApIndex].auApMac[2], gastAp[ulApIndex].auApMac[3],
    			            gastAp[ulApIndex].auApMac[4], gastAp[ulApIndex].auApMac[5],
    			            aucApStateStr);
      ulOnlineApNum++;
           }
    }
  printf("Total Num of Ap = %d.\n",gulSysConfAPCnt);
  printf("NonInitApNum    = %d.\n",ulNonInitApNum);
  printf("OnlineApNum     = %d.\n",ulOnlineApNum);
}

void showcapwapimgproc(UINT32 vulApIndex)
{
   if(vulApIndex < 0 || vulApIndex > gulSysConfAPCnt)
        {
     printf("\nERROR! AP_ID is %d over 0~%d.\n",vulApIndex,gulSysConfAPCnt);
     return;
        }

   printf("\n******AP_%d CAPWAP Img Update Info******\n",vulApIndex);
   printf("ApRcvUpdateReqNum:-------->%d\n",gastCapImgUpdateStatic[vulApIndex].ulApRcvUpdateReqNum);
   printf("ApSndUpdateRspNum:-------->%d\n",gastCapImgUpdateStatic[vulApIndex].ulApSndUpdateRspNum);
   printf("ApSndImgdataReqNum:------->%d\n",gastCapImgUpdateStatic[vulApIndex].ulApSndImgdataReqNum);
   printf("APRcvImgdataRspNum:------->%d\n",gastCapImgUpdateStatic[vulApIndex].ulAPRcvImgdataRspNum);
   printf("ApRcvImgdataReqNum:------->%d\n",gastCapImgUpdateStatic[vulApIndex].ulApRcvImgdataReqNum);
   printf("ApSndImgdataRspNum:------->%d\n",gastCapImgUpdateStatic[vulApIndex].ulApSndImgdataRspNum);
   printf("ApRcvResetReqNum:--------->%d\n",gastCapImgUpdateStatic[vulApIndex].ulApRcvResetReqNum);
   printf("ApSndResetRspNum:--------->%d\n",gastCapImgUpdateStatic[vulApIndex].ulApSndResetRspNum);
   printf("ApRcvImgdataReqIE26Num:--->%d\n",gastCapImgUpdateStatic[vulApIndex].ulApRcvImgdataReqIE26Num);

   printf("\n*********Total AP CAPWAP Update Info*************\n");
   printf("ApRcvUpdateReqNum:-------->%d\n",gstCapImgUpdateStat.ulApRcvUpdateReqNum);
   printf("ApSndUpdateRspNum:-------->%d\n",gstCapImgUpdateStat.ulApSndUpdateRspNum);
   printf("ApSndImgdataReqNum:------->%d\n",gstCapImgUpdateStat.ulApSndImgdataReqNum);
   printf("APRcvImgdataRspNum:------->%d\n",gstCapImgUpdateStat.ulAPRcvImgdataRspNum);
   printf("ApRcvImgdataReqNum:------->%u\n",gstCapImgUpdateStat.ulApRcvImgdataReqNum);
   printf("ApSndImgdataRspNum:------->%u\n",gstCapImgUpdateStat.ulApSndImgdataRspNum);
   printf("ApRcvImgdataReqIE26Num:--->%d\n",gstCapImgUpdateStat.ulApRcvImgdataReqIE26Num);
   printf("ApRcvResetReqNum:--------->%d\n",gstCapImgUpdateStat.ulApRcvResetReqNum);
   printf("ApSndResetRspNum:--------->%d\n",gstCapImgUpdateStat.ulApSndResetRspNum);
   printf("Total Update AP NUM:------>%d\n",gstCapImgUpdateStat.ulAPCapwapImgSucNum);
}

//STA参数初始化
void MT_StaParaInit(void)
{
	UINT32 ulStaIndex = 0;

	for(ulStaIndex = 0;ulStaIndex < MT_SYS_STA_CAPABILITY; ulStaIndex++)
	{
		gastStaPara[ulStaIndex].enDhcpState = DHCPV4_INIT_STATE;
		gastStaPara[ulStaIndex].enDhcpv6State = DHCPV6_INIT_STATE;
		gastStaPara[ulStaIndex].enAuthState = AUTH_INIT_STATE;
		gastStaPara[ulStaIndex].enHandOverState = HANDOVER_INIT_STATE;

		gastStaPara[ulStaIndex].ulStaHoldTimerId = CPSS_TD_INVALID;
		gastStaPara[ulStaIndex].ulStaHdOverTimerId = CPSS_TD_INVALID;
		gastStaPara[ulStaIndex].ulHomeApId = MT_UNDEFINED_OBJECT_IDENTIFIER;
		gastStaPara[ulStaIndex].ulForeignApId = MT_UNDEFINED_OBJECT_IDENTIFIER;
		gastStaPara[ulStaIndex].ulStaLocalApId = MT_UNDEFINED_OBJECT_IDENTIFIER;
		gastStaPara[ulStaIndex].ucWlanId = MT_UNDEFINED_WLANID_IDENTIFIER;

		gastStaPara[ulStaIndex].auStaMac[0] = 0x08;
		gastStaPara[ulStaIndex].auStaMac[1] = 0x07;
		gastStaPara[ulStaIndex].auStaMac[2] = 0x08;
		gastStaPara[ulStaIndex].auStaMac[3] = ulStaIndex / 65536;
		gastStaPara[ulStaIndex].auStaMac[4] = ulStaIndex / 256;
		gastStaPara[ulStaIndex].auStaMac[5] = ulStaIndex % 256;

		  //STA IPv4 Addr
		gastStaPara[ulStaIndex].aucStaIpV4Addr[0] = gaucStaGwIpV4Addr[0];
		gastStaPara[ulStaIndex].aucStaIpV4Addr[1] = ulStaIndex / 65536;
		gastStaPara[ulStaIndex].aucStaIpV4Addr[2] = ulStaIndex / 256;
		gastStaPara[ulStaIndex].aucStaIpV4Addr[3] = ulStaIndex % 256;

		//STA IPv4 Gateway
		cpss_mem_memcpy(gastStaPara[ulStaIndex].aucStaGwIpv4Addr,gaucStaGwIpV4Addr,MT_IPV4_ADDR_LEN);

		//STA IPv6 Addr
		gastStaPara[ulStaIndex].aucStaIpV6Addr[0] = gaucStaGwIpV6Addr[0];
		gastStaPara[ulStaIndex].aucStaIpV6Addr[1] = gaucStaGwIpV6Addr[1];
		gastStaPara[ulStaIndex].aucStaIpV6Addr[2] = gaucStaGwIpV6Addr[2];
		gastStaPara[ulStaIndex].aucStaIpV6Addr[3] = gaucStaGwIpV6Addr[3];
		gastStaPara[ulStaIndex].aucStaIpV6Addr[4] = gaucStaGwIpV6Addr[4];
		gastStaPara[ulStaIndex].aucStaIpV6Addr[5] = gaucStaGwIpV6Addr[5];
		gastStaPara[ulStaIndex].aucStaIpV6Addr[6] = gaucStaGwIpV6Addr[6];
		gastStaPara[ulStaIndex].aucStaIpV6Addr[7] = gaucStaGwIpV6Addr[7];
		gastStaPara[ulStaIndex].aucStaIpV6Addr[8] = 0x00;
		gastStaPara[ulStaIndex].aucStaIpV6Addr[9] = 0x00;
		gastStaPara[ulStaIndex].aucStaIpV6Addr[10] = 0x00;
		gastStaPara[ulStaIndex].aucStaIpV6Addr[11] = 0x00;
		gastStaPara[ulStaIndex].aucStaIpV6Addr[12] = ulStaIndex / 65536;
		gastStaPara[ulStaIndex].aucStaIpV6Addr[13] = ulStaIndex / 256;
		gastStaPara[ulStaIndex].aucStaIpV6Addr[14] = 0x00;
		gastStaPara[ulStaIndex].aucStaIpV6Addr[15] = ulStaIndex % 255;

		//STA IPv6 Gateway
		cpss_mem_memcpy(gastStaPara[ulStaIndex].aucStaGwIpv6Addr,gaucStaGwIpV6Addr,MT_IPV6_ADDR_LEN);

		//STA GW MAC
		cpss_mem_memcpy(gastStaPara[ulStaIndex].auGwMac,gaucStaGwMacAddr,MT_MAC_LENTH);
		cpss_mem_memset(gastStaPara[ulStaIndex].aucBssId,0,MT_MAC_LENTH);
		cpss_mem_memset(gastStaPara[ulStaIndex].aucCrrntAp,0,MT_MAC_LENTH);

		gastStaPara[ulStaIndex].ulStaTrgPrdCnt = 0;//在全部STA的上线周期内，对于未上线的STA重新发起认证过程
		gastStaPara[ulStaIndex].stStaOnLineTm.ulBeginTime = 0;
		gastStaPara[ulStaIndex].stStaOnLineTm.ulEndTime = 0;
	}
}

//AP参数初始化
void MT_ApParaInit(void)
{
	INT32 lRetCode = 0;
	UINT32 ulApIndex = 0;

	for(ulApIndex = 0; ulApIndex < gulApMaxNum; ulApIndex++)
	{
	  //AP的初始状态
	  gastAp[ulApIndex].enState = INIT_STATE;
	  gastAp[ulApIndex].bKeepAliveFlag = MT_FALSE;
	  gastAp[ulApIndex].ulStaCnt = 0;
	  gastAp[ulApIndex].ulApHoldTimerId = CPSS_TD_INVALID;
	  gastAp[ulApIndex].ulEchoTimerId = CPSS_TD_INVALID;
	  gastAp[ulApIndex].ulEchoReqNum = 0;
	  gastAp[ulApIndex].ulPrdTrgCnt = 0;
	  gastAp[ulApIndex].ulDiscvCnt = 0;
	  gastAp[ulApIndex].stEchoTm.ulBeginTime = 0;
	  gastAp[ulApIndex].stEchoTm.ulEndTime = 0;

	  //AP的MAC地址
	  lRetCode = GetMacFromStr(gastApconf[ulApIndex].aucApMacStr,gastAp[ulApIndex].auApMac);

	  if(-1 == lRetCode)
	     {
	    printf("ERROR.GetMacFromStr() for AP_%d is error.\nFile:%s.Line:%d\n",ulApIndex,__FILE__,__LINE__);
	     }

	  //AP的IP地址
	  GetIpFromStr(AF_INET,gastApconf[ulApIndex].aucApIpV4AddrStr,gastAp[ulApIndex].aucApIpV4);
	  GetIpFromStr(AF_INET,gstAcConfInfo.aucAcIpv4AddStr,gastAp[ulApIndex].aucApGwAddr);
	  gastAp[ulApIndex].ulIpv4Prefix = 16;

	  GetIpFromStr(AF_INET6,gastApconf[ulApIndex].aucApIpV6AddrStr,gastAp[ulApIndex].aucApIpV6);
	  gastAp[ulApIndex].ulIpv6Prefix = 64;
	}
}

//获取AP配置信息
void MT_GetApConf(UINT32 *vpulApCount)
{
  INT32 lRet = 1;
  UINT32 ulApIndex = 0;

  FILE *stream = NULL;
  stream = fopen("/mt_conf/apconf.conf", "r");

  if (stream == NULL)
    {
    printf("The file /mt_conf/apconf.conf was not opened.\nFile:%s, Line:%d\n",__FILE__,__LINE__);
    return;
    }

  fseek(stream, 0L, SEEK_SET);

  while (EOF != lRet)
    {
    lRet = fscanf(stream, "%d %s %s %s %d %s %s %s %s %d %s %s %s %s %d %d %d %d %d\n",
          &gastApconf[ulApIndex].ulApIndex,
          gastApconf[ulApIndex].aucApMacStr,
          gastApconf[ulApIndex].aucApIpV4AddrStr,
          gastApconf[ulApIndex].aucApIpV6AddrStr,
          &gastApconf[ulApIndex].enDiscovType,
          gastApconf[ulApIndex].aucApmodule,
          gastApconf[ulApIndex].aucApImgFileName,
          gastApconf[ulApIndex].aucApImgVer,
          gastApconf[ulApIndex].acuSerialNo,
          &gastApconf[ulApIndex].ulNetCarrierInc,
          gastApconf[ulApIndex].aucBoardID,
          gastApconf[ulApIndex].aucBoardRevision,
          gastApconf[ulApIndex].aucHdWareVer,
          gastApconf[ulApIndex].aucActiveSftwareVer,
          &gastApconf[ulApIndex].ucRadioType,
          &gastApconf[ulApIndex].ucWtpMacType,
          &gastApconf[ulApIndex].ucWtpFrmTunnlMod,
          &gastApconf[ulApIndex].ulApVlan,
          &gastApconf[ulApIndex].ulStaVlan);

      if (EOF != lRet)
               {
        ulApIndex++;
               }
    }

  *vpulApCount = ulApIndex;//统计配置信息中AP的个数

  fclose(stream);
}

//AC配置信息
void MT_GetAcInfo(void)
{
  INT32 lRetCode = 0;
  FILE *pFileStream = NULL;

  pFileStream = fopen("/mt_conf/ac_info.conf", "r");

  if (NULL == pFileStream)
    {
    printf("The file /mt_conf/ac_info.conf was not opened.\nFile:%s, Line:%d\n",__FILE__,__LINE__);
    return;
    }

  fseek(pFileStream, 0L, SEEK_SET);

  while (EOF != lRetCode)
    {
    lRetCode = fscanf(pFileStream, "%s %d %d %s %s\n",
          gstAcConfInfo.aucAcName,
          &gstAcConfInfo.ulAcApCapabolity,
          &gstAcConfInfo.ulAcIpType,
          gstAcConfInfo.aucAcIpv4AddStr,
          gstAcConfInfo.aucAcIpv6AddStr);
    }

  fclose(pFileStream);
}

//获取系统配置信息
void MT_GetSysConf(void)
{
  INT32 lRetCode = 0;
  FILE *pFileStream = NULL;

  pFileStream = fopen("/mt_conf/sysconf.conf", "r");

  if (NULL == pFileStream)
    {
    printf("The file /mt_conf/sysconf.conf was not opened.\nFile:%s, Line:%d\n",__FILE__,__LINE__);
    return;
    }

  fseek(pFileStream, 0L, SEEK_SET);

  while (EOF != lRetCode)
    {
    lRetCode = fscanf(pFileStream, "%d %d %d %d %d %s %d %d %d\n",
          &gstMtSysConf.ulTriggerTime,
          &gstMtSysConf.ulApTriggerNum,
          &gstMtSysConf.ulStaTriggerNum,
          &gstMtSysConf.ulApHoldTime,
          &gstMtSysConf.ucStaPolicy,
          gstMtSysConf.aucDevName,
          &gstMtSysConf.ulApOnlineFlag,
          &gstMtSysConf.ulTestMod,
          &gstMtSysConf.ulStaHoTrgrPeriod);
    }

  fclose(pFileStream);
}

void MT_GetStaConf(void)
{
  INT32 lRetCode = 0;
  FILE *pFileStream = NULL;

  pFileStream = fopen("/mt_conf/staconf.conf", "r");

  if (NULL == pFileStream)
    {
    printf("The file /mt_conf/staconf.conf was not opened.\nFile:%s, Line:%d\n",__FILE__,__LINE__);
    return;
    }

  fseek(pFileStream, 0L, SEEK_SET);

  while (EOF != lRetCode)
    {
    lRetCode = fscanf(pFileStream, "%d %d %d %s %s %s %s %s\n",
          &gstStaConfInfo.ucVlanFlag,
          &gstStaConfInfo.ucStaQoSFlag,
          &gstStaConfInfo.ucStaTunnelMode,
          gstStaConfInfo.aucStaIpv4GwAddrStr,
          gstStaConfInfo.aucStaIpv6GwAddrStr,
          gstStaConfInfo.aucStaSrvIpv4AddrStr,
          gstStaConfInfo.aucStaSrvIpv6AddrStr,
          gstStaConfInfo.aucStaSrvMacStr);
    }

  fclose(pFileStream);
}

//在ETH0上配置AP的IP地址
void MT_SysConfApIpOnEth0(void)
{
  UINT32 ulLoop = 0;
#define MT_STR_CMD_ARR_LEN     64
  char aucStrCmd[MT_STR_CMD_ARR_LEN] = {0};
#define MT_ETH_NUM_STR_ARR_LEN 16
  char aucUlStr[MT_ETH_NUM_STR_ARR_LEN] = {0};

  //ifocnfig eth0:XXXX 30.0.0.1/16 up
  if(0 == gulSysConfAPCnt)
    {
    printf("ERROR. AP Num in /mt_conf/apconf.conf is %d.\nFile:%s.Line:%d\n",gulSysConfAPCnt,__FILE__,__LINE__);
    return;
    }

  for (ulLoop = 1; ulLoop <= gulSysConfAPCnt; ulLoop++)
    {
    cpss_mem_memset(aucStrCmd,0,MT_STR_CMD_ARR_LEN);
    strcpy(aucStrCmd, "ifconfig ");
    strcat(aucStrCmd, (char*)gstMtSysConf.aucDevName);

    if(IP_ADDR_TYPE_V4 == gulIpAddrFlag)
          {
      strcat(aucStrCmd, ":");
      sprintf(aucUlStr,"%d",ulLoop);
      strcat(aucStrCmd, aucUlStr);
      strcat(aucStrCmd, " ");
      strcat(aucStrCmd, (char*)gastApconf[ulLoop - 1].aucApIpV4AddrStr);
      strcat(aucStrCmd, "/16 up");
         }
   else//IPv6
        {
     strcat(aucStrCmd," add ");
     strcat(aucStrCmd, (char*)gastApconf[ulLoop - 1].aucApIpV6AddrStr);
     strcat(aucStrCmd, "/64 up");
        }
   system(aucStrCmd);
    }
}

void MT_GetArpByPingAC(void)
{
#define MT_PING_CMD_STR_ARR_LEN 256
	char aucPngCmd[MT_PING_CMD_STR_ARR_LEN] = {'\0'};

	if(IP_ADDR_TYPE_V4 == gstAcConfInfo.ulAcIpType)
	{
		strcpy(aucPngCmd,"ping ");
		strcat(aucPngCmd,gstAcConfInfo.aucAcIpv4AddStr);
		strcat(aucPngCmd," -c 10");
	}
	else
	{
		strcpy(aucPngCmd,"ping6 ");
		strcat(aucPngCmd,gstAcConfInfo.aucAcIpv6AddStr);
		strcat(aucPngCmd," -c 10");
	}

	system(aucPngCmd);
}
//系统初始化函数
void MT_SysParaInit(void)
{
	UINT32 ulLoop = 0;

    //初始化系统配置信息
	cpss_mem_memset(&gstMtSysConf,0,sizeof(MT_SYSCONF_PARA_T));

    //初始化AP配置信息以及AP_WLAN信息
	for(ulLoop = 0; ulLoop < MT_AP_MAX_NUM; ulLoop++)
    {
    cpss_mem_memset(&gastApconf[ulLoop],0,sizeof(MT_SYSCONF_PARA_T));
    cpss_mem_memset(&gastCapImgUpdateStatic[ulLoop],0,sizeof(MULTITEST_CAPWAP_IMG_UPDATE_T));
    cpss_mem_memset(&gastApWlanInfo[ulLoop],0,sizeof(MT_AP_RADIO_WLAN_INFO_T));
     }

  cpss_mem_memset(&gstCapImgUpdateStat,0,sizeof(CAPWAP_IMG_UPDATE_STATICS_T));
    //统计值初始化
  cpss_mem_memset(&gstMutiTest_Statics, 0, sizeof(MUTITEST_STATICS_T));

    //进程初始化
  cpss_mem_memset(&gstMutiTestPid, 0, sizeof(CPSS_COM_PID_T));

  cpss_mem_memset(&gstAcConfInfo,0,sizeof(MT_AC_CONF_INFO_T));

//    gstAcConfInfo.aucAcName[MT_AC_NAME_LEN];            //AC Name
//    gstAcConfInfo.ulAcApCapabolity;                    //AC的AP容量；1024、2048、4096
//    gstAcConfInfo.ulAcIpType;                          //AC的IP地址类型；0:IPv4,1:IPv6
//    gstAcConfInfo.aucAcIpv4AddStr[MT_IPV4ADDR_STR_LEN];//AC的IPv4地址;
//    gstAcConfInfo.aucAcIpv6AddStr[MT_IPV6ADDR_STR_LEN];//AC的IPv6地址;

    MT_GetAcInfo();
/*
 * 获取到系统配置参数：
 *  gstMtSysConf.aucAcIpAddrStr;
 *  gstMtSysConf.ulApTriggerNum;
 *  gstMtSysConf.ulStaTriggerNum;
 *  gstMtSysConf.ulTriggerTime;
 */

    //从AC配置参数中获取AC的IP地址
    GetIpFromStr(AF_INET,gstAcConfInfo.aucAcIpv4AddStr,gaucAcIpAddrV4);
    GetIpFromStr(AF_INET6,gstAcConfInfo.aucAcIpv6AddStr,gaucAcIpAddrV6);

    if(1024 >= gstAcConfInfo.ulAcApCapabolity)
          {
      gulApMaxNum = 1024;
      gulStaMaxNum = gulApMaxNum * 10;
          }
    else if(2048 >= gstAcConfInfo.ulAcApCapabolity && 1024 < gstAcConfInfo.ulAcApCapabolity)
          {
      gulApMaxNum = 2048;
      gulStaMaxNum = gulApMaxNum * 64;
          }
    else if(4096 >= gstAcConfInfo.ulAcApCapabolity && 2048 < gstAcConfInfo.ulAcApCapabolity)
          {
      gulApMaxNum = 4096;
      gulStaMaxNum = gulApMaxNum * 64;
          }
    else
          {
      gulApMaxNum = 0;
      gulStaMaxNum = gulApMaxNum * 64;
      printf("ERROR.MT_AP_MAX_NUM is %d in /mt_conf/ac_info.\nFile:%s,Line:%d\n",gstAcConfInfo.ulAcApCapabolity,__FILE__,__LINE__);
      return;
          }

    MT_GetSysConf();

    gulPeriodTriggerTime = gstMtSysConf.ulTriggerTime;
    gulPeriodTriggerApNum = gstMtSysConf.ulApTriggerNum;
    gulPeriodTriggerStaNum = gstMtSysConf.ulStaTriggerNum;
    gulApHoldTime = gstMtSysConf.ulApHoldTime;
    gulStaHoTrgrPrd = gstMtSysConf.ulStaHoTrgrPeriod;

    if(0 == gstMtSysConf.ucStaPolicy)
           {
    	 gucStaAccessPlcy = MT_STA_ACCESS_POLICY_AP_FIRST;;
           }
    else if(1 == gstMtSysConf.ucStaPolicy)
           {
    	 gucStaAccessPlcy = MT_STA_ACCESS_POLICY_STA_FIRST;
           }
    else
           {
    	 printf("ERROR. gstMtSysConf.ucStaPolicy(%d) is not 1 or 0.\nFile: %s. Line: %d\n",gstMtSysConf.ucStaPolicy,__FILE__,__LINE__);
           }

    if(1 == gstMtSysConf.ulApOnlineFlag)
          {
    	 gbApAlwaysOnline = MT_TRUE;
          }

    if(1 == gstMtSysConf.ulTestMod)
          {
      gbTestMod = MT_TRUE;//测试模式，1：采用消息模板，0：程序自行构建消息，参数从AP配置信息中获取.采用参数配置形式//
           }

    if(0 == gstAcConfInfo.ulAcIpType)
          {
      gulIpAddrFlag = IP_ADDR_TYPE_V4;
          }
    else
         {
      gulIpAddrFlag = IP_ADDR_TYPE_V6;
         }

    if(0 == gulPeriodTriggerTime)
          {
      printf("ERROR. PeriodTriggerTime is 0.File:%s.Line:%d\n",__FILE__,__LINE__);
      return;
           }

    if(0 == gulPeriodTriggerApNum)
          {
      printf("ERROR. PeriodTriggerApNum is 0.File:%s.Line:%d\n",__FILE__,__LINE__);
      return;
           }

    if(0 == gulPeriodTriggerStaNum)
           {
      printf("ERROR. PeriodTriggerStaNum is 0.File:%s.Line:%d\n",__FILE__,__LINE__);
      return;
            }

    if(0 == gulApHoldTime)
          {
      printf("ERROR. ApHoldTime is 0.File:%s.Line:%d\n",__FILE__,__LINE__);
      return;
          }

    if(0 == gulStaHoTrgrPrd)
          {
    	printf("ERROR. gulStaHoTrgrPrd is 0.File:%s. Line:%d\n",__FILE__,__LINE__);
    	return;
          }

    gulApNumPerSecond = (1000 * gulPeriodTriggerApNum) / gulPeriodTriggerTime;
    gulStaNumPerSecond = (1000 * gulPeriodTriggerStaNum) / gulPeriodTriggerTime;

    //获取AP的配置参数
    MT_GetApConf(&gulSysConfAPCnt);
#if 0 //Debug on PC
    gulSysConfAPCnt = 10;
#endif
    if (gulSysConfAPCnt < 1)
          {
      printf("No AP in /mt_conf/apconf.conf.\n File:%s.Line:%d\n", __FILE__, __LINE__);
      return;
           }
    else if (gulSysConfAPCnt > gulApMaxNum)
          {
      printf("The AP Num in /mt_conf/apconf.conf is %d, more than %d, so cut it into %d.\n",gulSysConfAPCnt,gulApMaxNum,gulApMaxNum);
      gulSysConfAPCnt = gulApMaxNum;
            }

    //获取STA配置信息
    MT_GetStaConf();

    GetIpFromStr(AF_INET,gstStaConfInfo.aucStaIpv4GwAddrStr,gaucStaGwIpV4Addr);
    GetIpFromStr(AF_INET6,gstStaConfInfo.aucStaIpv6GwAddrStr,gaucStaGwIpV6Addr);

    //STA Tunnel Mode
    switch(gstStaConfInfo.ucStaTunnelMode)
          {
    case 0:
      gucStaTunnelMod = MT_STA_TUNNEL_MOD_4TO4;
      break;
    case 1:
      gucStaTunnelMod = MT_STA_TUNNEL_MOD_4IN6;
      break;
    case 2:
      gucStaTunnelMod = MT_STA_TUNNEL_MOD_6IN4;
      break;
    case 3:
      gucStaTunnelMod = MT_STA_TUNNEL_MOD_6TO6;
      break;
    default:
      printf("ERROR. STA Tunnel Mode(%d) is not among (0~3).Check /mt_conf/staconf.conf.\nFile:%s,Line:%d.\n",gstStaConfInfo.ucStaTunnelMode,__FILE__,__LINE__);
      break;
          }

    //AP参数初始化
    MT_ApParaInit();
    //STA参数初始化
    MT_StaParaInit();
    //在ETH0上配置AP的IP地址
    system("service network restart");
    system("/etc/rc.local");

    MT_SysConfApIpOnEth0();
}

/*WHEN THE SECOND TIMER COME,CALL THIS FUN TO START MUTITEST AP*/
void MT_ProcStart(UINT32 vulCmdStartApNum,UINT32 vulOnlineApNum)
{
	UINT32 ulApIndex = 0;//AP索引
	UINT32 ulPrdTrgApCntr = 0;
	MTBool bSendResult = MT_FALSE;

	if(vulOnlineApNum >= vulCmdStartApNum)
	{
		return;
	}

	for(ulPrdTrgApCntr = 0; ulPrdTrgApCntr < gulPeriodTriggerApNum; ulPrdTrgApCntr++)
	{
		for(ulApIndex = 0; ulApIndex < gulCmdStartApNum; ulApIndex++)
		{
			if (INIT_STATE == gastAp[ulApIndex].enState)
			{
				break;
			}

			if(RUN_STATE != gastAp[ulApIndex].enState)
			{
				gastAp[ulApIndex].ulPrdTrgCnt++;

				if(MT_WTP_REDISCOVERY_INTERVAL_CNT == gastAp[ulApIndex].ulPrdTrgCnt)
				{
					MT_ApLog(ulApIndex, "AP Has to reboot for MT_WTP_REDISCOVERY_INTERVAL_CNT.");
					WtpRelease(ulApIndex);
				}
			}
		}

		if(ulApIndex < gulCmdStartApNum)
		{
			bSendResult = ApMutiTestSendDiscoverReq(ulApIndex);  //调用DiscoverReq消息发送函数，发起AP发现流程
			if(MT_TRUE == bSendResult)
		    {
				gstMutiTest_Statics.ulSendDiscoverReq++;
				gastAp[ulApIndex].enState = WAIT_DISCOVER_RSP_STATE;
				gastAp[ulApIndex].ulDiscvCnt++;
				gastAp[ulApIndex].ulApHoldTimerId = cpss_timer_para_set(MT_TEST_APHOLD_TMRID, gulApHoldTime * 1000, ulApIndex);
				MT_ApLog(ulApIndex, "ApMutiTestSendDiscoverReq Success");
		    }
			else
		    {
				gstErrMsgStatics.ulSendDiscoverReqErr++;
				MT_ApLog(ulApIndex, "ApMutiTestSendDiscoverReq Failure");
		    }
		}
	}
}
#if 0
/*INIT SOCKET,INIT MSG RECEIVED TASK*/
void MT_Task_Cpm(void)
{
	struct sockaddr_in stSrcAddr;
	UINT32 ulSrcAddrLen = 0;
	INT32 ilRcvLen = 0;
	CPSS_COM_PID_T stDstPid;
	UINT8 aucRcvBuf[MT_RECV_MSG_BUF_LEN] = {0};
	UINT32 ulMsgId = 0;
	INT32 retValue = 0;
	INT8 iRetCode = CPSS_ERROR;

	cpss_mem_memset(&stSrcAddr,0,sizeof(stSrcAddr));
	if(IP_ADDR_TYPE_V4 == gulIpAddrFlag)
    {
		retValue = 1;
		glCpmSockId = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
		setsockopt(glCpmSockId, IPPROTO_IP, IP_HDRINCL, &retValue, sizeof (retValue));
    }
	else
	{
		retValue = 1;
		//glCpmSockId = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
		glCpmSockId = socket(AF_INET6, SOCK_RAW, IPPROTO_UDP);
		setsockopt(glCpmSockId, IPPROTO_IPV6, IPV6_V6ONLY, &retValue, sizeof (retValue));
	}

	//获取逻辑地址
	cpss_com_logic_addr_get(&stDstPid.stLogicAddr, &stDstPid.ulAddrFlag);
	stDstPid.ulPd = MT_MAIN_PROC;

	while(1)
	{
		ilRcvLen = recvfrom(glCpmSockId, aucRcvBuf, MT_RECV_MSG_BUF_LEN, 0, (struct sockaddr*)&stSrcAddr, &ulSrcAddrLen);

		if(ilRcvLen <= 0)
		{
			gstRcvSndMsfCnt.ulRcvfrmforCpmMsgErrCnt++;
			printf("\nMT_Task_Cpm():recvfrom() failure!ilRcvLen <= 0\n,File: %s,Line: %d\n",__FILE__,__LINE__);
		}
		else
		{
			gstRcvSndMsfCnt.ulRcvfrmforCpmMsgCnt++;

			ulMsgId = MT_SOCKET_DATA_CPM_IND_MSG;
			iRetCode = cpss_com_send(&stDstPid, ulMsgId, aucRcvBuf, ilRcvLen);

			if(CPSS_OK == iRetCode)
			{
				gstRcvSndMsfCnt.ulComSndforCmpMsgCnt++;
			}
			else
			{
				gstRcvSndMsfCnt.ulComSndforCmpMsgErrCnt++;
			}
		}
	}
}

/*INIT SOCKET,INIT MSG RECEIVED TASK*/
void MT_Task_Upm(void)
{
	struct sockaddr_in stSrcAddr;
	UINT32 ulSrcAddrLen = 0;
	INT32 ilRcvLen = 0;
	CPSS_COM_PID_T stDstPid;
	UINT8 aucRcvBuf[MT_RECV_MSG_BUF_LEN] = {0};
	UINT32 ulMsgId = 0;
	INT32 retValue = 0;
	INT8 iRetCode = CPSS_ERROR;

	cpss_mem_memset(&stSrcAddr,0,sizeof(stSrcAddr));
	if(IP_ADDR_TYPE_V4 == gulIpAddrFlag)
	{
		retValue = 1;
		glUpmSockId = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
		setsockopt(glUpmSockId, IPPROTO_IP, IP_HDRINCL, &retValue, sizeof (retValue));
	}
	else
	{
		retValue = 1;
		glUpmSockId = socket(AF_INET6, SOCK_RAW, IPPROTO_UDP);
		//glUpmSockId = socket(AF_INET6, SOCK_DGRAM, IPPROTO_UDP);
		setsockopt(glUpmSockId, IPPROTO_IPV6, IPV6_V6ONLY, &retValue, sizeof (retValue));
	}

	cpss_com_logic_addr_get(&stDstPid.stLogicAddr, &stDstPid.ulAddrFlag);
	stDstPid.ulPd = MT_MAIN_PROC;

	while(1)
	{
		ilRcvLen = recvfrom(glUpmSockId, aucRcvBuf, MT_RECV_MSG_BUF_LEN, 0, (struct sockaddr*)&stSrcAddr, &ulSrcAddrLen);

		if(ilRcvLen <= 0)
		{
			gstRcvSndMsfCnt.ulRcvfrmforUpmMsgErrCnt++;
			printf("\nMT_Task_Upm():recvfrom() failure!ulRcvLen <= 0\n,File: %s,Line: %d\n",__FILE__,__LINE__);
		}
		else
		{
			gstRcvSndMsfCnt.ulRcvfrmforUpmMsgCnt++;
			/*RECEIVED MSG FROM SOCKET ,MSGID IS MT_SOCKET_DATA_UPM_IND_MSG*/
			ulMsgId = MT_SOCKET_DATA_UPM_IND_MSG;
			iRetCode = cpss_com_send(&stDstPid, ulMsgId, aucRcvBuf, ilRcvLen);

			if(CPSS_OK == iRetCode)
    		{
				gstRcvSndMsfCnt.ulComSndforUmpMsgCnt++;
    		}
			else
    		{
				gstRcvSndMsfCnt.ulComSndforUmpMsgErrCnt++;
    		}
		}
	}
}
#endif
#if 0
/*INIT Dhcp SOCKET,INIT MSG RECEIVED TASK*/
void MT_Task_ApDhcp(void)
{
	struct sockaddr_in stSrcAddr;
	UINT32 ulSrcAddrLen = 0;
	INT32 ilRcvLen = 0;
	CPSS_COM_PID_T stDstPid;
	UINT8 aucRcvBuf[MT_RECV_MSG_BUF_LEN] = {'\0'};
	UINT32 ulMsgId = 0;
	INT32 retValue = 0;

	cpss_mem_memset(&stSrcAddr,0,sizeof(stSrcAddr));
	cpss_mem_memset(&stDstPid,0,sizeof(stDstPid));

	if(IP_ADDR_TYPE_V4 == gulIpAddrFlag)
    {
		retValue = 1;
		glDhcpSockId = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
		setsockopt(glDhcpSockId, IPPROTO_IP, IP_HDRINCL, &retValue, sizeof (retValue));
    }
	else
	{
		retValue = 1;
		glDhcpSockId = socket(AF_INET6, SOCK_RAW, IPPROTO_UDP);
		setsockopt(glDhcpSockId, IPPROTO_IPV6, IPV6_V6ONLY, &retValue, sizeof (retValue));
	}

	//获取逻辑地址
	cpss_com_logic_addr_get(&stDstPid.stLogicAddr, &stDstPid.ulAddrFlag);
	stDstPid.ulPd = MT_MAIN_PROC;

	while(1)
    {
		ilRcvLen = recvfrom(glDhcpSockId, aucRcvBuf, MT_RECV_MSG_BUF_LEN, 0, (struct sockaddr*)&stSrcAddr, &ulSrcAddrLen);

		if(ilRcvLen <= 0)
        {
			printf("\nMT_Task_ApDhcp():recvfrom() failure!ilRcvLen <= 0\n,File: %s,Line: %d\n",__FILE__,__LINE__);
        }
		else
		{
			ulMsgId = MT_SOCKET_DATA_DHCP_IND_MSG;
			cpss_com_send(&stDstPid, ulMsgId, aucRcvBuf, ilRcvLen);
		}
    }
}
#endif

void MT_PowerOn(CPSS_COM_MSG_HEAD_T * vpstMsgHead)
{
  SMSS_PROC_ACTIVATE_RSP_MSG_T stSmssActiveRsp;
  INT32 lResult = 0;
#if 0
  char* pcProcNameCpm = "MT_Task_Cpm";
  char* pcProcNameUpm = "MT_Task_Upm";
  char* pcProcNameApDhcp = "MT_Task_ApDhcp";
#endif
  char* pcProcNameDispatcher = "MT_Task_MsgDispctcher";

  cpss_mem_memset(&stSmssActiveRsp,0,sizeof(SMSS_PROC_ACTIVATE_RSP_MSG_T));
  cpss_mem_memset(&gstWtpEventFlag,0,sizeof(MT_WTP_EVENT_FLAG_T));

  /*VALID CHECK*/
  if ((NULL == vpstMsgHead) || 0 == (vpstMsgHead->ulMsgId))
    {
    printf("\nMT_PowerOn:MsgHead INPUT_ERROR.\nFile:%s.Line:%d\n",__FILE__, __LINE__);
    return;
    }

  switch (vpstMsgHead->ulMsgId)
    {
  case SMSS_PROC_ACTIVATE_REQ_MSG:
    printf("MT_PowerOn received SMSS_PROC_ACTIVATE_REQ_MSG!\n");

    stSmssActiveRsp.ulResult = SMSS_OK;

    lResult = cpss_vk_msg_sender_get(&gstMutiTestPid);

    if (CPSS_OK != lResult)
          {
      printf("\nMT_PowerOn:cpss_vk_msg_sender_get() Error.\nFile:%s.Line:%d\n",__FILE__, __LINE__);
      return;
          }
    lResult = cpss_com_send(&gstMutiTestPid, SMSS_PROC_ACTIVATE_RSP_MSG,(UINT8 *) &stSmssActiveRsp, sizeof(stSmssActiveRsp));

    if (CPSS_OK != lResult)
          {
    	 printf("\nMT_PowerOn: <SMSS_PROC_ACTIVATE_RSP_MSG>(0x%x)[MSG_SENT_FAILED].\nFile:%s.Line:%d\n",SMSS_PROC_ACTIVATE_RSP_MSG, __FILE__, __LINE__);
    	 return;
          }

    break;

  case SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG:

    printf("MT_PowerOn received SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG!\n");
    /*CHANGE MUTITEST PROC STATE TO MT_WORKING*/
    lResult = cpss_vk_proc_user_state_set(MT_WORKING);

    if (CPSS_OK != lResult)
          {
      printf("\nMT_POWERON: <MT_PowerOn>(%d)[STATE_CHANGE_FAILED].\nFile:%s.Line:%d\n",MT_POWERON, __FILE__, __LINE__);
      return;
          }

    printf("MT_PowerOn: change MT_Main_Proc state to MT_Working\n");

    /*INIT MUTITEST PRO*/
    MT_SysParaInit();
    MT_GetCapwapMsgPool();
#if 0
    /*START THE TASK TO INIT SOCKET ,AND RECEIVED MSG*/
    cpss_vos_task_spawn(pcProcNameCpm, 0, 0, 0, (void*) MT_Task_Cpm, 0);

    /*START THE TASK TO INIT UPM SOCKET ,AND RECEIVED MSG*/
    cpss_vos_task_spawn(pcProcNameUpm, 0, 0, 0, (void*) MT_Task_Upm, 0);
    /*START THE TASK TO INIT ApDhcpv4 SOCKET ,AND RECEIVED MSG*/
//    cpss_vos_task_spawn(pcProcNameApDhcp, 0, 0, 0, (void*) MT_Task_ApDhcp, 0);
#endif
    /*START THE TASK TO INIT MT_Task_MsgDispctcher SOCKET ,AND RECEIVED MSG*/
    cpss_vos_task_spawn(pcProcNameDispatcher, 0, 0, 0, (void*) MT_Task_MsgDispctcher, 0);


    printf("\n*************SYSTEM CONF PARA*************\n");
    printf("PeriodTriggerTime-----> %-10d",gulPeriodTriggerTime);
    printf("PeriodTriggerApNum----> %-10d\n",gulPeriodTriggerApNum);
    printf("PeriodTriggerStaNum---> %-10d",gulPeriodTriggerStaNum);
    printf("ApHoldTime------------> %-10d\n",gstMtSysConf.ulApHoldTime);
    printf("AP Access Capability--> %-10d",gulApNumPerSecond);
    printf("STA Access Capability-> %-10d\n",gulStaNumPerSecond);
    printf("STA Policy------------> %-10d",gstMtSysConf.ucStaPolicy);
    printf("APCount_APConf--------> %-10d\n",gulSysConfAPCnt);

    if(MT_FALSE == gbTestMod)
          {
      printf("TestMod---------------> %s(%d)\n","PARACONF",gbTestMod);//测试模式，0：程序自行构建消息，参数从AP配置信息中获取
          }
    else if(MT_TRUE == gbTestMod)
          {
      printf("TestMod---------------> %s(%d)\n","MSGPOOL",gbTestMod);//测试模式，1：采用消息模板
          }
    else
          {
      printf("TestMod---------------> %s(%d)\n","Unknown",gbTestMod);
          }

    if(MT_FALSE == gbApAlwaysOnline)
          {
      printf("ApOnlineMode----------> %s(%d)\n","AutoOffline",gbApAlwaysOnline);//AP永远在线,0：AP自行下线，1：AP永远在线
          }
    else if(MT_TRUE == gbApAlwaysOnline)
          {
      printf("ApOnlineMode----------> %s(%d)\n","AlwaysOnline",gbApAlwaysOnline);//AP永远在线,0：AP自行下线，1：AP永远在线
          }
    else
          {
      printf("ApOnlineMode----------> %s(%d)\n","Unknown",gbApAlwaysOnline);//AP永远在线,0：AP自行下线，1：AP永远在线
          }

    printf("\n*************AC CONF INFO*****************\n");
    printf("AC Name---------------> %s\n",gstAcConfInfo.aucAcName);
    printf("MT_AP_MAX_NUM---------> %d\n",gulApMaxNum);
    printf("MT_STA_MAX_NUM--------> %d\n",gulStaMaxNum);
    if(0 == gstAcConfInfo.ulAcIpType)
          {
      printf("AcIpType--------------> %s.\n","IPv4");
      printf("AC_IPV4_ADDR----------> %s.\n",gstAcConfInfo.aucAcIpv4AddStr);
          }
    else//Ipv6
          {
      printf("AcIpType--------------> %s.\n","IPv6");
      printf("AC_IPV6_ADDR----------> %s.\n",gstAcConfInfo.aucAcIpv6AddStr);
          }

    printf("\n*************STA CONF INFO*****************\n");
    printf("STA_VlanTag-----------> %d\n",gstStaConfInfo.ucVlanFlag);
    printf("StaQoSFlag------------> %d\n",gstStaConfInfo.ucStaQoSFlag);

    if(0 == gstStaConfInfo.ucStaTunnelMode)
          {
      printf("STA_TUNNUL_MOD--------> %s(%d)\n","MT_STA_TUNNEL_MOD_4TO4",gstStaConfInfo.ucStaTunnelMode);
           }
    else if(1 == gstStaConfInfo.ucStaTunnelMode)
          {
      printf("STA_TUNNUL_MOD--------> %s(%d)\n","MT_STA_TUNNEL_MOD_4IN6",gstStaConfInfo.ucStaTunnelMode);
           }
    else if(2 == gstStaConfInfo.ucStaTunnelMode)
          {
      printf("STA_TUNNUL_MOD--------> %s(%d)\n","MT_STA_TUNNEL_MOD_6IN4",gstStaConfInfo.ucStaTunnelMode);
           }
    else if(3 == gstStaConfInfo.ucStaTunnelMode)
          {
      printf("STA_TUNNUL_MOD--------> %s(%d)\n","MT_STA_TUNNEL_MOD_6TO6",gstStaConfInfo.ucStaTunnelMode);
           }
    else
          {
      printf("STA_TUNNUL_MOD--------> %s(%d)\n","Unknown",gstStaConfInfo.ucStaTunnelMode);
           }

    printf("STA_IPV4_GW-----------> %s\n",gstStaConfInfo.aucStaIpv4GwAddrStr);
    printf("STA_IPV6_GW-----------> %s\n",gstStaConfInfo.aucStaIpv6GwAddrStr);
    printf("STA_SRV_IPV4----------> %s\n",gstStaConfInfo.aucStaSrvIpv4AddrStr);
    printf("STA_SRV_IPV6----------> %s\n",gstStaConfInfo.aucStaSrvIpv6AddrStr);
    printf("STA_SRV_MAC-----------> %s\n",gstStaConfInfo.aucStaSrvMacStr);
    printf("\n****MULTITEST TOOL help message!****\n");
    printf("Start Para: start $(ApNumber) $(StaNum-PerAp) $(STA HoldTime(s))\n");
    printf("Stop Para: stop\n");
    printf("****MULTITEST TOOL help message!****\n");

    break;

  default:
    printf("\nMT_PowerOn received unknown message = %d!\nFile:%s.Line:%d\n",vpstMsgHead->ulMsgId, __FILE__, __LINE__);
    break;
    }
  return;
}

//Process Message from port5246
void MT_MsgProcFrm5246(CPSS_COM_MSG_HEAD_T *vpstMsgHead)
{
	MT_DISPATCHER_SND_MSG_T stDisptchrMsg;
	UINT32 ulApIdx = MT_UNDEFINED_OBJECT_IDENTIFIER;
	UINT8 aucCapwapBuf[MT_CAPWAP_BUF_LEN] = {0};
	UINT32 ulCapwapSuffix = 0;
	UINT32 ulCWFirst32 = 0;
	UINT32 ulCWSecond32 = 0;
	UINT32 ulCWThird32 = 0;
	UINT8 aucApTmpInfo[128] = {0};

	UINT32 ulCWCpmMsgType = MT_UNDEFINED_OBJECT_IDENTIFIER;
	MTBool bSendResult = MT_FALSE;
	UINT8 aucStaMac[MT_MAC_LENTH] = { 0 };
	UINT32 ulLoop = 0;
	UINT32 ulStaIndex = 0;

	UINT16 usStaConfigReqIeType = 0xffff; /*Add Station(t=8) or Del Station(t=18)*/
	UINT16 usApUpdateReqIe24 = 0xffff;
	UINT16 usApUpdateReqIe24Len = 0;
	UINT16 usApUpdateReqIe25 = 0xffff;    /*AP Image Update(t=25)*/
	UINT16 usApUpdateReqIe25Len = 0;
	UINT16 usApUpdateReqIe26 = 0xffff;    /*AP Image Update(t=26)*/
	UINT16 usApUpdateReqIe37 = 0xffff;
	UINT16 usApUpdateReqIe37Len = 0;
	UINT32 ulTimerMsgPara = 0;

	MSG_ELEMNT_T stT1024;
	T1024_VALUE stT1024Value;
	UINT32 ulSsidLen = 0;
	UINT8 ucRadioId = 0;
	UINT8 ucWlanId = 0;
	UINT32 ulSuffix = 0;

	if(NULL == vpstMsgHead)
	{
		gstErrMsgStatics.ul5246MsgHead_NULL++;
		return;
	}

	cpss_mem_memset(&stDisptchrMsg,0,sizeof(MT_DISPATCHER_SND_MSG_T));
	cpss_mem_memset(&stT1024,0,sizeof(MSG_ELEMNT_T));
	cpss_mem_memset(&stT1024Value,0,sizeof(T1024_VALUE));

	cpss_mem_memcpy(&stDisptchrMsg,vpstMsgHead->pucBuf,vpstMsgHead->ulLen);
	ulApIdx = stDisptchrMsg.ulApIdx;

	if((ulApIdx >= gulCmdStartApNum)||(ulApIdx < 0))
	{
		gstErrMsgStatics.ulRcv5246MsgErrApId++;
		MT_ApLog(ulApIdx,"AP not online.");
		return;
	}

	cpss_mem_memcpy(aucCapwapBuf,stDisptchrMsg.aucCapwapBuf,stDisptchrMsg.ulCapwapBufLen);

	//CAPWAP头信息分析
	ulCapwapSuffix = 0;
	cpss_mem_memcpy(&ulCWFirst32,&aucCapwapBuf[ulCapwapSuffix],sizeof(ulCWFirst32));
	ulCapwapSuffix = ulCapwapSuffix + sizeof(ulCWFirst32);
	cpss_mem_memcpy(&ulCWSecond32,&aucCapwapBuf[ulCapwapSuffix],sizeof(ulCWSecond32));
	ulCapwapSuffix = ulCapwapSuffix + sizeof(ulCWSecond32);
	cpss_mem_memcpy(&ulCWThird32,&aucCapwapBuf[ulCapwapSuffix],sizeof(ulCWThird32));
	ulCapwapSuffix = ulCapwapSuffix + sizeof(ulCWThird32);

	ulCWFirst32 = htonl(ulCWFirst32);
	ulCWSecond32 = htonl(ulCWSecond32);
	ulCWThird32 = htonl(ulCWThird32);
	ulCWCpmMsgType = ulCWThird32;

	switch (ulCWCpmMsgType)
    {
	//AC->AP DISCOVERY_RESP
	case CW_MSG_TYPE_VALUE_DISCOVERY_RESPONSE:
	{
		if (WAIT_DISCOVER_RSP_STATE != gastAp[ulApIdx].enState)
        {
			gstErrMsgStatics.ulRecvDiscoverRspErr++;
			MT_ApLog(ulApIdx,"AP Receive DISCOVERY_RESPONSE Error.");
			break;
        }

		gstMutiTest_Statics.ulRecvDiscoverRsp++;
		MT_ApLog(ulApIdx,"AP Receive DISCOVERY_RESPONSE Success.");

		if(gulOnlineApNum == gulCmdStartApNum)
		{
			break;
		}

		bSendResult = ApMutiTestSendJoinReq(ulApIdx);

		if(MT_TRUE == bSendResult)
        {
			gstMutiTest_Statics.ulSendJoinReq++;
			gastAp[ulApIdx].enState = WAIT_JOIN_RSP_STATE;
			MT_ApLog(ulApIdx,"ApMutiTestSendJoinReq Success.");
        }
		else
		{
			gstErrMsgStatics.ulSendJoinReqErr++;
			MT_ApLog(ulApIdx,"ApMutiTestSendJoinReq Failure.");
		}
	}
	break;

	case CW_MSG_TYPE_VALUE_JOIN_RESPONSE:
	{
		if (WAIT_JOIN_RSP_STATE != gastAp[ulApIdx].enState)
        {
			gstErrMsgStatics.ulRecvJoinRspErr++;
			MT_ApLog(ulApIdx,"AP Receive JOIN_RESPONSE Error.");
			break;
        }

		gstMutiTest_Statics.ulRecvJoinRsp++;
		MT_ApLog(ulApIdx,"AP Receive JOIN_RESPONSE Success.");

		if(gulOnlineApNum == gulCmdStartApNum)
        {
			break;
        }

		bSendResult = ApMutiTestSendConfigStatusReq(ulApIdx);

		if(MT_TRUE == bSendResult)
        {
			gstMutiTest_Statics.ulSendConfigStatusReq++;
			gastAp[ulApIdx].enState = WAIT_CONFIG_STATUS_RSP_STATE;
			MT_ApLog(ulApIdx,"ApMutiTestSendConfigStatusReq Success.");
        }
		else
		{
			gstErrMsgStatics.ulSendConfigStatusReqErr++;
			MT_ApLog(ulApIdx,"ApMutiTestSendConfigStatusReq Failure.");
		}
	}
	break;

	case CW_MSG_TYPE_VALUE_CONFIGURE_RESPONSE:
	{
		if (WAIT_CONFIG_STATUS_RSP_STATE != gastAp[ulApIdx].enState)
    	{
			gstErrMsgStatics.ulRecvConfigStatusRspErr++;
			MT_ApLog(ulApIdx,"AP RECEIVE ConfigStatusRsp ERROR.");
			break;
    	}

		gstMutiTest_Statics.ulRecvConfigStatusRsp++;
		MT_ApLog(ulApIdx,"AP RECEIVE ConfigStatusRsp SUCCESS.");

		if(gulOnlineApNum == gulCmdStartApNum)
    	{
			break;
    	}

		bSendResult = ApMutiTestSendChangeStateReq(ulApIdx);

		if(MT_TRUE == bSendResult)
        {
			gstMutiTest_Statics.ulSendChangeStateReq++;
			gastAp[ulApIdx].enState = WAIT_CHANGE_STATE_RSP_STATE;
			MT_ApLog(ulApIdx,"ApMutiTestSendChangeStateReq SUCCESS.");
        }
		else
    	{
			gstErrMsgStatics.ulSendChangeStateReqErr++;
			MT_ApLog(ulApIdx,"ApMutiTestSendChangeStateReq Failure.");
    	}
	}
	break;

	case CW_MSG_TYPE_VALUE_CHANGE_STATE_EVENT_RESPONSE:
	{
		if (WAIT_CHANGE_STATE_RSP_STATE != gastAp[ulApIdx].enState)
        {
			gstErrMsgStatics.ulRecvChangeStateRspErr++;
			MT_ApLog(ulApIdx,"AP RECEIVE CHANGE_STATE_RESPONSE ERROR.");
			break;
        }

		gstMutiTest_Statics.ulRecvChangeStateRsp++;
		MT_ApLog(ulApIdx,"AP RECEIVE CHANGE_STATE_RESPONSE SUCCESS.");

		if(gulOnlineApNum == gulCmdStartApNum)
        {
			break;
        }

		gastAp[ulApIdx].enState = RUN_STATE;
		gstMutiTest_Statics.ulApUpSuccessCount++;
		gulOnlineApNum++;
		MT_ApLog(ulApIdx,"AP Online Success.");

		gstWtpEventFlag.ucFlagL2IeId_5630 = MT_WTP_EVENT_L2IE_ENABLE;
		gstWtpEventFlag.ucFlagL2IeId_5631 = MT_WTP_EVENT_L2IE_ENABLE;
		gstWtpEventFlag.ucFlagL2IeId_5632 = MT_WTP_EVENT_L2IE_ENABLE;
		gstWtpEventFlag.ucFlagL2IeId_5633 = MT_WTP_EVENT_L2IE_ENABLE;

		bSendResult = ApMutiTestSendEventReq_IE37_IE56(ulApIdx);

		if(MT_TRUE == bSendResult)
        {
			gstMutiTest_Statics.ulSendWTPEventReq++;
			MT_ApLog(ulApIdx,"ApMutiTestSendEventReq_IE37_IE56(30 31 32 33) SUCCESS.");
        }
		else
		{
			gstErrMsgStatics.ulSendWTPEventReqErr++;
			gstErrMsgStatics.ulSendEventReq_IE37IE56Err++;
			MT_ApLog(ulApIdx,"ApMutiTestSendEventReq_IE37_IE56(30 31 32 33) Failure.");
		}

		gstWtpEventFlag.ucFlagL2IeId_5630 = MT_WTP_EVENT_L2IE_DISABLE;
		gstWtpEventFlag.ucFlagL2IeId_5631 = MT_WTP_EVENT_L2IE_DISABLE;
		gstWtpEventFlag.ucFlagL2IeId_5632 = MT_WTP_EVENT_L2IE_DISABLE;
		gstWtpEventFlag.ucFlagL2IeId_5633 = MT_WTP_EVENT_L2IE_DISABLE;

		//Echo消息发送
		bSendResult = ApMutiTestSendEchoReq(ulApIdx);
		if(MT_FALSE == bSendResult)
		{
			gstErrMsgStatics.ulApSendEchoReqErr++;
			break;
		}
		else
        {
			gstMutiTest_Statics.ulApSendEchoReq++;
			gastAp[ulApIdx].ulEchoReqNum++;
			gastAp[ulApIdx].stEchoTm.ulBeginTime = cpss_tick_get();
			gastAp[ulApIdx].stEchoTm.ulEndTime = 0;
			gastAp[ulApIdx].ulEchoTimerId = cpss_timer_para_loop_set(MT_TEST_PERIOD_ECHO_TMRID, gulEchoInterval,ulApIdx);
        }
	}
	break;

	case CW_MSG_TYPE_VALUE_CONFIGURE_UPDATE_REQUEST:
	{
		if(RUN_STATE != gastAp[ulApIdx].enState)
    	{
			gstErrMsgStatics.ulRecvConfigUpdateReqErr++;
			MT_ApLog(ulApIdx,"AP RECEIVE CONFIGURE_UPDATE_REQUEST ERROR.");
			break;
    	}
		else
		{
			gstMutiTest_Statics.ulRecvConfigUpdateReq++;
			MT_ApLog(ulApIdx,"AP RECEIVE CONFIGURE_UPDATE_REQUEST SUCCESS.");
			bSendResult = ApMutiTestSendConfigUpdateRsp(ulApIdx);

			if(MT_FALSE == bSendResult)
            {
				gstErrMsgStatics.ulSendConfigUpdateRspErr++;
				MT_ApLog(ulApIdx,"ApMutiTestSendConfigUpdateRsp Failure.");
				break;
            }

			gstMutiTest_Statics.ulSendConfigUpdateRsp++;
			MT_ApLog(ulApIdx,"ApMutiTestSendConfigUpdateRsp SUCCESS.");

			gstWtpEventFlag.ucFlagL2IeId_5743 = MT_WTP_EVENT_L2IE_ENABLE;

			bSendResult = ApMutiTestSendEventReq_IE37_IE57(ulApIdx);

			if(MT_TRUE == bSendResult)
            {
				gstMutiTest_Statics.ulSendWTPEventReq++;
				MT_ApLog(ulApIdx,"ApMutiTestSendEventReq_IE37_IE57_43 SUCCESS.");
            }
			else
            {
				gstErrMsgStatics.ulSendWTPEventReqErr++;
				gstErrMsgStatics.ulSendEventReq_IE37IE57Err++;
				MT_ApLog(ulApIdx,"ApMutiTestSendEventReq_IE37_IE57_43 Failure.");
            }
			gstWtpEventFlag.ucFlagL2IeId_5743 = MT_WTP_EVENT_L2IE_DISABLE;
		}
	}
	break;
#if 0 //AP VER Update later
         else if(RUN_STATE == gastAp[ulApIdx].enState)
                        {
           gastCapImgUpdateStatic[ulApIdx].ulApRcvUpdateReqNum++;
           gstCapImgUpdateStat.ulApRcvUpdateReqNum += gastCapImgUpdateStatic[ulApIdx].ulApRcvUpdateReqNum;

           bSendResult = ApMutiTestSendConfigUpdateRsp(ulApIdx);

           if(MT_TRUE == bSendResult)
                             {
             gastCapImgUpdateStatic[ulApIdx].ulApSndUpdateRspNum++;
             gstCapImgUpdateStat.ulApSndUpdateRspNum += gastCapImgUpdateStatic[ulApIdx].ulApSndUpdateRspNum;
                              }
           else
                              {
        	   gstErrMsgStatics.ulSendConfigUpdateRspErr++;
        	   printf("AP[%d].enState:%d\n",ulApIdx,gastAp[ulApIdx].enState);
        	   printf("ERROR!ApMutiTestSendConfigUpdateRsp(AP_%d) fail.\n File:%s.Line:%d\n",ulApIdx,__FILE__,__LINE__);
        	   break;
                             }
//    1.1.1.1       Update Mode Setting（IE=69）
//              0                   1                   2                   3
//              0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//              +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//              |            Type=69            |            Length             |
//              +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//              |    UpdateType |           Reserved            |     IPType    |
//              +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//              |                           IP Address                          |
//              +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//              |                           IP Address                          |
//              +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//              |                           IP Address                          |
//              +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//              |                           IP Address                          |
//              +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//              |        FTP Data Type          |         FTP Data Length       |
//              +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//              |                        FTP Data Value...
//              +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//              Type:   69  AP升级方式设置。
//              Length:   > 24
//              UpdateType: AP升级方式。0:FTP，1:CAPWAP。
//              Reserved: 保留。
//              IPType：IP地址类型。0:IPv4，1:IPv6
//              IP Address: IP地址，如果是IPv4格式，只用前4个字节。
//              FTP Data Type:  FTP Date类型字段,我们定义如下枚举类型来表示，AC必须携带这些信息。
//                      0 -   FTP User Name，版本下载时FTP的用户名
//                      1 -   FTP Password：版本下载时FTP的密码
//              FTP Data Length:
//                              类型字段的长度值，16-bit。长度不超过32字节。
//              FTP Data Value:
//                              对应类型的数据值。
           cpss_mem_memcpy(&usApUpdateReqIe37,&aucCapwapBuf[16],2);
           cpss_mem_memcpy(&usApUpdateReqIe37Len,&aucCapwapBuf[18],2);
           usApUpdateReqIe37 = ntohs(usApUpdateReqIe37);
           usApUpdateReqIe37Len = ntohs(usApUpdateReqIe37Len);

           cpss_mem_memcpy(&usApUpdateReqIe25,&aucCapwapBuf[20 + usApUpdateReqIe37Len],2);
           cpss_mem_memcpy(&usApUpdateReqIe25Len,&aucCapwapBuf[20 + usApUpdateReqIe37Len + 2],2);
           usApUpdateReqIe25 = ntohs(usApUpdateReqIe25);
           usApUpdateReqIe25Len = ntohs(usApUpdateReqIe25Len);

           if(25 == usApUpdateReqIe25)/*CONFIGURE_UPDATE_REQUEST中如果带有25IE，则进行版本升级*/
                             {
             gusApFileNameLen = usApUpdateReqIe25Len;
             cpss_mem_memcpy(gaucApUpdateFilename,&aucCapwapBuf[20 + usApUpdateReqIe37Len + 4],gusApFileNameLen);//AP版本名称
             bSendResult = ApMutiTestSendIamgeDataReq(ulApIdx);/*Image Data Request*/

             if(MT_TRUE == bSendResult)
                                   {
               gastCapImgUpdateStatic[ulApIdx].ulApSndImgdataReqNum++;
               gstCapImgUpdateStat.ulApSndImgdataReqNum += gastCapImgUpdateStatic[ulApIdx].ulApSndImgdataReqNum;
                                   }
             else
                                   {
            	 gstErrMsgStatics.ulSendIamgeDataRspErr++;
            	 printf("ERROR!ApMutiTestSendIamgeDataReq fail.\n File: %s\n Line:%d\n",__FILE__,__LINE__);
            	 break;
                                   }
                             }
#if 0
           else/*Other IE*/
                             {
             printf("ERROR. AP_CONFIGURE_UPDATE_REQUEST IE is %d.\nFile: %s.Line:%d\n",usApUpdateReqIe25,__FILE__,__LINE__);
                             }
#endif
                       }
#endif

         //Add wlan
	case CW_MSG_TYPE_VALUE_WLAN_CONFIGURATION_REQUEST:
	{
		if(RUN_STATE != gastAp[ulApIdx].enState)
    	{
			MT_ApLog(ulApIdx,"AP RECEIVE WLAN_CONFIGURATION_REQUEST ERROR.");
			gstErrMsgStatics.ulRecvAddWlanReqErr++;
			break;
    	}

		cpss_mem_memcpy(&stT1024,&aucCapwapBuf[16],4);

		if(1024 != htons(stT1024.usMsgElemntType))
        {
			gstErrMsgStatics.ulRecvAddWlanReqErr++;
			MT_ApLog(ulApIdx,"AP RECEIVE WLAN_CONFIGURATION_REQUEST, but stT1024.usMsgElemntType is not 1024.");
			break;
        }

		gstMutiTest_Statics.ulRecvAddWlanReq++;
		MT_ApLog(ulApIdx,"AP RECEIVE WLAN_CONFIGURATION_REQUEST SUCCESS.");

		cpss_mem_memcpy(stT1024.aucMsgElemntValue,&aucCapwapBuf[20],htons(stT1024.usMsgElemntLen));
		ulSsidLen = htons(stT1024.usMsgElemntLen)
        		- sizeof(stT1024Value.ucRadioID)
        		- sizeof(stT1024Value.ucWlanID)
        		- sizeof(stT1024Value.usCapability)
        		- sizeof(stT1024Value.ucKeyIndex)
        		- sizeof(stT1024Value.ucKeyStatus)
        		- sizeof(stT1024Value.usKeyLen)
        		- htons(stT1024Value.usKeyLen)
        		- MT_GROUP_TSC_LEN//GrpTsc
        		- sizeof(stT1024Value.ucQoS)
        		- sizeof(stT1024Value.ucAuthType)
        		- sizeof(stT1024Value.ucMacMode)
        		- sizeof(stT1024Value.ucTunnelMod)
        		- sizeof(stT1024Value.ucSuppressSsid);

		ulSuffix = 0;
		cpss_mem_memcpy(&stT1024Value.ucRadioID,&stT1024.aucMsgElemntValue[ulSuffix],sizeof(stT1024Value.ucRadioID));
		ulSuffix = ulSuffix + sizeof(stT1024Value.ucRadioID);
		cpss_mem_memcpy(&stT1024Value.ucWlanID,&stT1024.aucMsgElemntValue[ulSuffix],sizeof(stT1024Value.ucWlanID));
		ulSuffix = ulSuffix + sizeof(stT1024Value.ucWlanID);
		cpss_mem_memcpy(&stT1024Value.usCapability,&stT1024.aucMsgElemntValue[ulSuffix],sizeof(stT1024Value.usCapability));
		ulSuffix = ulSuffix + sizeof(stT1024Value.usCapability);
		cpss_mem_memcpy(&stT1024Value.ucKeyIndex,&stT1024.aucMsgElemntValue[ulSuffix],sizeof(stT1024Value.ucKeyIndex));
		ulSuffix = ulSuffix + sizeof(stT1024Value.ucKeyIndex);
		cpss_mem_memcpy(&stT1024Value.ucKeyStatus,&stT1024.aucMsgElemntValue[ulSuffix],sizeof(stT1024Value.ucKeyStatus));
		ulSuffix = ulSuffix + sizeof(stT1024Value.ucKeyStatus);
		cpss_mem_memcpy(&stT1024Value.usKeyLen,&stT1024.aucMsgElemntValue[ulSuffix],sizeof(stT1024Value.usKeyLen));
		ulSuffix = ulSuffix + sizeof(stT1024Value.usKeyLen);
		stT1024Value.usKeyLen = htons(stT1024Value.usKeyLen);
		cpss_mem_memcpy(stT1024Value.aucKey,&stT1024.aucMsgElemntValue[ulSuffix],stT1024Value.usKeyLen);
		ulSuffix = ulSuffix + stT1024Value.usKeyLen;
		cpss_mem_memcpy(stT1024Value.aucGrpTsc,&stT1024.aucMsgElemntValue[ulSuffix],MT_GROUP_TSC_LEN);
		ulSuffix = ulSuffix + MT_GROUP_TSC_LEN;
		cpss_mem_memcpy(&stT1024Value.ucQoS,&stT1024.aucMsgElemntValue[ulSuffix],sizeof(stT1024Value.ucQoS));
		ulSuffix = ulSuffix + sizeof(stT1024Value.ucQoS);
		cpss_mem_memcpy(&stT1024Value.ucAuthType,&stT1024.aucMsgElemntValue[ulSuffix],sizeof(stT1024Value.ucAuthType));
		ulSuffix = ulSuffix + sizeof(stT1024Value.ucAuthType);
		cpss_mem_memcpy(&stT1024Value.ucMacMode,&stT1024.aucMsgElemntValue[ulSuffix],sizeof(stT1024Value.ucMacMode));
		ulSuffix = ulSuffix + sizeof(stT1024Value.ucMacMode);
		cpss_mem_memcpy(&stT1024Value.ucTunnelMod,&stT1024.aucMsgElemntValue[ulSuffix],sizeof(stT1024Value.ucTunnelMod));
		ulSuffix = ulSuffix + sizeof(stT1024Value.ucTunnelMod);
		cpss_mem_memcpy(&stT1024Value.ucSuppressSsid,&stT1024.aucMsgElemntValue[ulSuffix],sizeof(stT1024Value.ucSuppressSsid));
		ulSuffix = ulSuffix + sizeof(stT1024Value.ucSuppressSsid);
		cpss_mem_memcpy(stT1024Value.aucSsid,&stT1024.aucMsgElemntValue[ulSuffix],ulSsidLen);
		ulSuffix = ulSuffix + ulSsidLen;

		ucRadioId = stT1024Value.ucRadioID;//1~31
		ucWlanId = stT1024Value.ucWlanID;//1~16

		for(ulLoop = 0; ulLoop < MT_RADIO_MAX_NUM; ulLoop++)
        {
			if(0 == gastApWlanInfo[ulApIdx].aucRadioList[ulLoop])
			{
				gastApWlanInfo[ulApIdx].aucRadioList[ulLoop] = ucRadioId;
				gastApWlanInfo[ulApIdx].ucRadioNum++;
				break;
			}
			else if(ucRadioId == gastApWlanInfo[ulApIdx].aucRadioList[ulLoop])
			{
				break;
			}
			else
			{
				continue;
			}
        }

		for(ulLoop = 0; ulLoop < MT_WLAN_MAX_NUM; ulLoop++)
		{
			if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ulLoop])
			{
				gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ulLoop] = ucWlanId;
				gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum++;
				gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucWlanId].ucAuthType = stT1024Value.ucAuthType;
				gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucWlanId].ucMacMode = stT1024Value.ucMacMode;
				gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucWlanId].usKeyLen = stT1024Value.usKeyLen;
				cpss_mem_memcpy(gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucWlanId].aucKey,stT1024Value.aucKey,stT1024Value.usKeyLen);
				cpss_mem_memcpy(gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucWlanId].aucSsid,stT1024Value.aucSsid,ulSsidLen);
				gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucWlanId].ucSsidLen = ulSsidLen;
				gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucWlanId].ucTunnelMod = stT1024Value.ucTunnelMod;
				break;
			}
		}

		bSendResult = ApMutiTestSendAddWlanRsp(ulApIdx,ucRadioId,ucWlanId);

		if(MT_FALSE == bSendResult)
		{
			gstErrMsgStatics.ulSendAddWlanRspErr++;
			MT_ApLog(ulApIdx, "ApMutiTestSendAddWlanRsp Failure");
			break;
		}

		gstMutiTest_Statics.ulSendAddWlanRsp++;
		MT_ApLog(ulApIdx, "ApMutiTestSendAddWlanRsp SUCCESS");

		for(ulLoop = 0; ulLoop < 1; ulLoop++)
		{
			bSendResult = ApMutiTestSendKeepAlive(ulApIdx);
			if(MT_FALSE == bSendResult)
			{
				gstErrMsgStatics.ulApRecvKeepAliveErr++;
			}
			else
			{
				gstMutiTest_Statics.ulApSendKeepAlive++;
			}
		}
	}
	break;

	case CW_MSG_TYPE_VALUE_ECHO_RESPONSE:
	{
		gstMutiTest_Statics.ulApRecvEchoRsp++;
		gastAp[ulApIdx].ulEchoReqNum = 0;
		gastAp[ulApIdx].stEchoTm.ulEndTime = 0;
		//防止AP的ECHO定时器丢失而脱离检测
		gastAp[ulApIdx].stEchoTm.ulBeginTime = cpss_tick_get();
	}
	break;

	case CW_MSG_TYPE_VALUE_WTP_EVENT_RESPONSE:
	{
		gstMutiTest_Statics.ulRecvWtpEventRsp++;
	}
	break;

	//Add/Del Sta
	case CW_MSG_TYPE_VALUE_STATION_CONFIGURATION_REQUEST:
	{
		MT_ApLog(ulApIdx, "AP RECEIVE STATION_CONFIGURATION_REQUEST SUCCESS");
		cpss_mem_memcpy(&usStaConfigReqIeType,&aucCapwapBuf[16],2);
		usStaConfigReqIeType = ntohs(usStaConfigReqIeType);

		cpss_mem_memcpy(&aucStaMac[0],&aucCapwapBuf[22],MT_MAC_LENTH);
		ulStaIndex = getStaIndexByMac(aucStaMac);

		//非法用户
		if(MT_UNDEFINED_OBJECT_IDENTIFIER == ulStaIndex)
        {
			gstErrMsgStatics.ulRcv5246MsgErrStaId++;
			MT_ApLog(ulApIdx, "AP RECEIVE STATION_CONFIGURATION_REQUEST SUCCESSFULLY, but ulStaIndex is MT_UNDEFINED_OBJECT_IDENTIFIER.");
			MT_StaLog(ulStaIndex,"Sta not online.");
			break;
        }

		//Echo Check,只要AP上存在5246端口的消息交互，即认为该AP的状态正常
		gastAp[ulApIdx].ulEchoReqNum = 0;

		switch(usStaConfigReqIeType)
		{
		//增加STA
		case 8:
		{
			//初始接入状态
			if(AUTH_INIT_STATE == gastStaPara[ulStaIndex].enAuthState)
			{
				gstErrMsgStatics.ulRecvAddStaConfigReqErr++;
				MT_ApLog(ulApIdx, "AP RECEIVE STATION_CONFIGURATION_REQUEST SUCCESSFULLY, but ulStaIndex state is AUTH_INIT_STATE.");
				MT_StaLog(ulStaIndex,"AP RECEIVE STATION_CONFIGURATION_REQUEST SUCCESSFULLY, but ulStaIndex state is AUTH_INIT_STATE.");
				break;
			}

			//其他接入状态
			if(HANDOVER_INIT_STATE == gastStaPara[ulStaIndex].enHandOverState)
			{
				gstMutiTest_Statics.ulRecvAddStaConfigReq++;
				MT_ApLog(ulApIdx, "AP RECEIVE AddStaConfigReq SUCCESS, and STA state is HANDOVER_INIT_STATE (AUTH STATE is ASSOCIATION_SUCESS_STATE)");
				MT_StaLog(ulStaIndex,"AddStaConfigReq Arrived SUCCESS.");

				bSendResult = ApMutiTestSendStaConfigRsp(usStaConfigReqIeType,ulApIdx,ulStaIndex);

				if(MT_TRUE == bSendResult)
				{
					gstMutiTest_Statics.ulSendAddStaConfigRsp++;
					gastAp[ulApIdx].ulStaCnt++;
					gulOnlineStaCnt++;
					gastStaPara[ulStaIndex].ulStaLocalApId = ulApIdx;

					MT_ApLog(ulApIdx, "ApMutiTestSendStaConfigRsp SUCCESS, and sta state is HANDOVER_INIT_STATE (AUTH STATE is ASSOCIATION_SUCESS_STATE)");
					MT_StaLog(ulStaIndex,"AddStaConfigRsp Be sent SUCCESS.");

					gstWtpEventFlag.ucFlagL2IeId_5641 = MT_WTP_EVENT_L2IE_ENABLE;
					bSendResult = ApMutiTestSendEventReq_IE37_IE56(ulApIdx);

					if(MT_TRUE == bSendResult)
					{
						gstMutiTest_Statics.ulSendWTPEventReq++;
						MT_ApLog(ulApIdx, "ApMutiTestSendEventReq_IE37_IE56_41 SUCCESS.");
					}
					else
					{
						gstErrMsgStatics.ulSendWTPEventReqErr++;
						gstErrMsgStatics.ulSendEventReq_IE37IE56Err++;
						MT_ApLog(ulApIdx, "ApMutiTestSendEventReq_IE37_IE56_41 Failure.");
					}
					gstWtpEventFlag.ucFlagL2IeId_5641 = MT_WTP_EVENT_L2IE_DISABLE;
				}
				else
				{
					gstErrMsgStatics.ulSendAddStaConfigRspErr++;
					MT_ApLog(ulApIdx, "ApMutiTestSendStaConfigRsp Failure.");
					MT_StaLog(ulStaIndex,"AddStaConfigRsp Be sent Failure.");
				}
			}
			else
			{
				//切换状态
				gstMutiTest_Statics.ulRecvAddStaConfigReqHO++;
				MT_ApLog(ulApIdx, "AP RECEIVE AddStaConfigReqHO SUCCESS.");
				MT_StaLog(ulStaIndex,"AddStaConfigReqHO Arrived SUCCESS.");

				bSendResult = ApMutiTestSendStaConfigRsp(usStaConfigReqIeType,ulApIdx,ulStaIndex);

				if(MT_TRUE == bSendResult)
				{
					gstMutiTest_Statics.ulSendAddStaConfigRspHO++;
					gastAp[ulApIdx].ulStaCnt++;
					gulOnlineStaCnt++;
					gastStaPara[ulStaIndex].ulStaLocalApId = ulApIdx;
					MT_ApLog(ulApIdx, "ApMutiTestSendStaConfigRspHO SUCCESS.");
					MT_StaLog(ulStaIndex,"Add StaConfigRspHO Be sent SUCCESS.");
				}
				else
				{
					gstErrMsgStatics.ulSendAddStaConfigRspHOErr++;
					MT_ApLog(ulApIdx, "ApMutiTestSendStaConfigRspHO Failure.");
					MT_StaLog(ulStaIndex,"Add StaConfigRspHO Be sent Failure.");
				}
			}
		}
		break;

		//删除STA
		case 18:
		{
			if(AUTH_INIT_STATE == gastStaPara[ulStaIndex].enAuthState)
			{
				//DelSta请求所所要求的用户状态错误
				gstErrMsgStatics.ulRecvDelStaConfigReqErr++;
				MT_ApLog(ulApIdx, "AP RECEIVE DelStaConfigReq ERROR. STA State is AUTH_INIT_STATE.");
				gbLogFlag = MT_TRUE;
				MT_StaLog(ulStaIndex,"DelStaConfigReq ERROR. STA State is AUTH_INIT_STATE.");
				gbLogFlag = MT_FALSE;
				break;
			}

			if(ASSOCIATION_SUCESS_STATE != gastStaPara[ulStaIndex].enAuthState)
			{
				gstMutiTest_Statics.ulRecvDelStaConfigReq++;
				MT_ApLog(ulApIdx, "AP RECEIVE DelStaConfigReq SUCCESS. STA State is ASSOCIATION_FAIL");
				MT_StaLog(ulStaIndex,"Del StaConfigReq Arrived SUCCESS.");

				bSendResult = ApMutiTestSendStaConfigRsp(usStaConfigReqIeType,ulApIdx,ulStaIndex);

				if(MT_TRUE == bSendResult)
				{
					gstMutiTest_Statics.ulSendDelStaConfigRsp++;
					MT_ApLog(ulApIdx, "ApMutiTestSendStaConfigRsp for DelSTA SUCCESS.");
					MT_StaLog(ulStaIndex,"Del StaConfigRsp Be sent SUCCESS.");
					staStateInit(ulStaIndex);
				}
				else
				{
					gstErrMsgStatics.ulSendDelStaConfigRspErr++;
					MT_ApLog(ulApIdx, "ApMutiTestSendStaConfigRsp for DelSTA Failure.");
					MT_StaLog(ulStaIndex,"Del StaConfigRsp Be sent Failure.");
				}
			}
			else
			{
				//ASSOCIATION_SUCESS_STATE
				//HANDOVER_INIT_STATE
				if(HANDOVER_INIT_STATE == gastStaPara[ulStaIndex].enHandOverState)
				{
					gstMutiTest_Statics.ulRecvDelStaConfigReq++;
					MT_ApLog(ulApIdx, "AP RECEIVE DelStaConfigReq SUCCESS. STA State is HANDOVER_INIT_STATE(AUTH STATE is ASSOCIATION_SUCESS_STATE)");
					MT_StaLog(ulStaIndex,"Del StaConfigReq Arrived SUCCESS.");

					bSendResult = ApMutiTestSendStaConfigRsp(usStaConfigReqIeType,ulApIdx,ulStaIndex);

					if(MT_TRUE == bSendResult)
					{
						gstMutiTest_Statics.ulSendDelStaConfigRsp++;
						MT_ApLog(ulApIdx, "ApMutiTestSendStaConfigRsp for DelSta SUCCESS. and STA will Shutdown.");
						gastAp[ulApIdx].ulStaCnt--;
						gulOnlineStaCnt--;
						MT_StaLog(ulStaIndex,"Del StaConfigRsp Be sent SUCCESS.");
						staStateInit(ulStaIndex);
					}
					else
					{
						gstErrMsgStatics.ulSendDelStaConfigRspErr++;
						MT_ApLog(ulApIdx, "ApMutiTestSendStaConfigRsp for DelSta Failure.");
						MT_StaLog(ulStaIndex,"Del StaConfigRsp Be sent Failure.");
					}
				}
				else if(HANDOVER_SUCCESS == gastStaPara[ulStaIndex].enHandOverState)
				{
					//HANDOVER_SUCCESS
					gstMutiTest_Statics.ulRecvDelStaConfigReqHO++;
					MT_ApLog(ulApIdx, "AP RecvDelStaConfigReqHO Success.");
					MT_StaLog(ulStaIndex,"Del StaConfigRspHO Arrived Success.");

					bSendResult = ApMutiTestSendStaConfigRsp(usStaConfigReqIeType,ulApIdx,ulStaIndex);
					if(MT_TRUE == bSendResult)
					{
						gstMutiTest_Statics.ulSendDelStaConfigRspHO++;
						MT_ApLog(ulApIdx, "ApMutiTestSendStaConfigRspHO Success.");
						gastAp[ulApIdx].ulStaCnt--;
						gulOnlineStaCnt--;
						gastStaPara[ulStaIndex].ulStaLocalApId = MT_UNDEFINED_OBJECT_IDENTIFIER;
						MT_StaLog(ulStaIndex,"Del StaConfigRspHO Be sent Success.");

						gastStaPara[ulStaIndex].ulHomeApId = gastStaPara[ulStaIndex].ulForeignApId;
						gastStaPara[ulStaIndex].ulForeignApId = MT_UNDEFINED_OBJECT_IDENTIFIER;
						gastStaPara[ulStaIndex].enHandOverState = HANDOVER_INIT_STATE;
						ulTimerMsgPara = ulStaIndex + 1;
						gastStaPara[ulStaIndex].ulStaHdOverTimerId = cpss_timer_para_set(MT_TEST_STAHD_OVER_TMRID, gulStaHoTrgrPrd * 1000, ulTimerMsgPara);
					}
					else
					{
						gstErrMsgStatics.ulSendDelStaConfigRspHOErr++;
						MT_ApLog(ulApIdx, "ApMutiTestSendStaConfigRspHO Failure.");
						MT_StaLog(ulStaIndex,"Del StaConfigRspHO Be sent Failure.");
					}
				}
				else
				{
					gstMutiTest_Statics.ulRecvDelStaConfigReqHO++;
					MT_ApLog(ulApIdx, "AP RecvDelStaConfigReqHO SUCCESS. STA State is HANDOVER_FAIL.");
					MT_StaLog(ulStaIndex,"Del StaConfigReqHO Arrived SUCCESS.");

					bSendResult = ApMutiTestSendStaConfigRsp(usStaConfigReqIeType,ulApIdx,ulStaIndex);

					if(MT_TRUE == bSendResult)
					{
						MT_ApLog(ulApIdx, "ApMutiTestSendStaConfigRspHO SUCCESS.");
						gstMutiTest_Statics.ulSendDelStaConfigRspHO++;
						gastAp[ulApIdx].ulStaCnt--;
						gulOnlineStaCnt--;
						gastStaPara[ulStaIndex].ulStaLocalApId = MT_UNDEFINED_OBJECT_IDENTIFIER;
						MT_StaLog(ulStaIndex,"Del StaConfigRspHO Be sent SUCCESS.");
						staStateInit(ulStaIndex);
					}
					else
					{
						gstErrMsgStatics.ulSendDelStaConfigRspHOErr++;
						MT_ApLog(ulApIdx, "ApMutiTestSendStaConfigRspHO Failure.");
						MT_StaLog(ulStaIndex,"Del StaConfigRspHO Be sent Failure.");
					}
				}
			}
		}
		break;

		default:
		{
			gstErrMsgStatics.ulRecvAddStaConfigReqErr++;
			MT_ApLog(ulApIdx,"ERROR.STATION_CONFIGURATION_REQUEST usStaConfigReqIeType is not 8 or 18.");
			MT_StaLog(ulStaIndex,"ERROR.STATION_CONFIGURATION_REQUEST usStaConfigReqIeType is not 8 or 18.");
		}
		break;
		}
	}
	break;

	//capwap升级
	case CW_MSG_TYPE_VALUE_IMAGE_DATA_RESPONSE:
	{
		MT_ApLog(ulApIdx, "Ap IMAGE_DATA_RESPONSE Success.");
		gastCapImgUpdateStatic[ulApIdx].ulAPRcvImgdataRspNum++;
		gstCapImgUpdateStat.ulAPRcvImgdataRspNum += gastCapImgUpdateStatic[ulApIdx].ulAPRcvImgdataRspNum;
	}
	break;

	//capwap升级
	case CW_MSG_TYPE_VALUE_IMAGE_DATA_REQUEST:
	{
		if(INIT_STATE != gastAp[ulApIdx].enState)
		{
			MT_ApLog(ulApIdx, "AP Receive IMAGE_DATA_REQUEST Success.");
			gastCapImgUpdateStatic[ulApIdx].ulApRcvImgdataReqNum++;
			gstCapImgUpdateStat.ulApRcvImgdataReqNum += gastCapImgUpdateStatic[ulApIdx].ulApRcvImgdataReqNum;

			bSendResult = ApMutiTestSendIamgeDataRsp(ulApIdx);

			if(MT_TRUE == bSendResult)
			{
				MT_ApLog(ulApIdx, "ApMutiTestSendIamgeDataRsp Success.");
				gastCapImgUpdateStatic[ulApIdx].ulApSndImgdataRspNum++;
				gstCapImgUpdateStat.ulApSndImgdataRspNum += gastCapImgUpdateStatic[ulApIdx].ulApSndImgdataRspNum;
			}
			else
			{
				MT_ApLog(ulApIdx, "ApMutiTestSendIamgeDataRsp Failure.");
				gstErrMsgStatics.ulSendIamgeDataRspErr++;
				break;
			}

			cpss_mem_memcpy(&usApUpdateReqIe24,&aucCapwapBuf[16],2);
			cpss_mem_memcpy(&usApUpdateReqIe24Len,&aucCapwapBuf[18],2);
			usApUpdateReqIe24 = ntohs(usApUpdateReqIe24);
			usApUpdateReqIe24Len = ntohs(usApUpdateReqIe24Len);

			if((24 == usApUpdateReqIe24) && (usApUpdateReqIe24Len > 500))
			{
				break;
			}

			cpss_mem_memcpy(&usApUpdateReqIe26,&aucCapwapBuf[16 + 2 + usApUpdateReqIe24Len + 2],2);
			usApUpdateReqIe26 = ntohs(usApUpdateReqIe26);

			if(26 == usApUpdateReqIe26)
			{
				gastCapImgUpdateStatic[ulApIdx].ulApRcvImgdataReqIE26Num++;

				gstWtpEventFlag.ucFlagL2IeId_5640 = MT_WTP_EVENT_L2IE_ENABLE;
				bSendResult = ApMutiTestSendEventReq_IE37_IE56(ulApIdx);

				if(MT_TRUE == bSendResult)
				{
					gstMutiTest_Statics.ulSendWTPEventReq++;
					MT_ApLog(ulApIdx, "ApMutiTestSendEventReq_IE37_IE56_40 Success.");
				}
				else
				{
					gstErrMsgStatics.ulSendWTPEventReqErr++;
					gstErrMsgStatics.ulSendEventReq_IE37IE56Err++;
					MT_ApLog(ulApIdx, "ApMutiTestSendEventReq_IE37_IE56_40 Failure.");
				}
				gstWtpEventFlag.ucFlagL2IeId_5640 = MT_WTP_EVENT_L2IE_DISABLE;

				gstCapImgUpdateStat.ulApRcvImgdataReqIE26Num += gastCapImgUpdateStatic[ulApIdx].ulApRcvImgdataReqIE26Num;
				gstCapImgUpdateStat.ulAPCapwapImgSucNum++;
			}
		}
		else
		{
			MT_ApLog(ulApIdx, "AP Receive IMAGE_DATA_REQUEST Error.");
			gstErrMsgStatics.ulSendIamgeDataRspErr++;
		}
	}
	break;

	case CW_MSG_TYPE_VALUE_RESET_REQUEST:
	{
		gstMutiTest_Statics.ulApRecvResetReq++;
		gastCapImgUpdateStatic[ulApIdx].ulApRcvResetReqNum++;
		MT_ApLog(ulApIdx, "AP Receive RESET_REQUEST Success.");

		WtpRelease(ulApIdx);
	}
	break;

	default:
	{
		gstErrMsgStatics.ulRcv5246UnknwnMsg++;
		cpss_mem_memset(aucApTmpInfo,0,128);
		sprintf((char*)aucApTmpInfo,"In MT_Working state, MT_Tester Rcv MSG(%d).",ulCWCpmMsgType);
		MT_ApLog(ulApIdx, (char*)aucApTmpInfo);
	}
	break;
    }
}

//Process Message from port5247
void MT_MsgProcFrm5247(CPSS_COM_MSG_HEAD_T *vpstMsgHead)
{
	MT_DISPATCHER_SND_MSG_T stDisptchrMsg;
	UINT32 ulApIdx = MT_UNDEFINED_OBJECT_IDENTIFIER;
	UINT32 ulStaIndex = MT_UNDEFINED_OBJECT_IDENTIFIER;
	UINT8 aucStaTmpInfo[128] = {0};
	UINT8 aucCapwapBuf[MT_CAPWAP_BUF_LEN] = {0};
	UINT32 ulCapwapSuffix = 0;
	UINT32 ulCWFirst32 = 0;
	UINT32 ulCWSecond32 = 0;
	UINT8 ucVerFalg = 0;
	UINT8 ucTypeFalg = 0;
	UINT8 ucHdrLen = 0;
	UINT8 ucRId = 0;
	UINT8 ucWbId = 0;
	MT_BIT bTflag = 0;
	MT_BIT bFflag = 0;
	MT_BIT bLflag = 0;
	MT_BIT bWflag = 0;
	MT_BIT bMflag = 0;
	MT_BIT bKflag = 0;
	UINT8 ucFlags = 0;
	MTBool bSendResult = MT_FALSE;
	UINT32 ulTimerMsgPara = 0;
	UINT16 usProtclType = 0xFFFF;
	MT_ETH_HDR_T stEthHdr;
	MT_VLAN_HEADER_T stVlanHdr;
	MT_ARP_REQ_T stArp;
	MT_IPV6_HEADER_T stIpv6Hdr;
	MT_IPV4_HEADER_T stIpv4Hdr;
	MT_STA_ICMPV4_T stIcmpV4Hdr;
	MT_UDP_HDR_T stUdpHdr;
	MT_BOOTSTRAP_HDR_T stBootStrapHdr;
	MT_DHCP_OPTION_T stOption53;
	MT_ICMPV6_NEIGHBOR_ADVERTISEMENT_T stNeighbSolicit;
	MT_ICMPV6_T stStaIcmpv6;
	MT_DHCPV6_T stDhcpv6Req;
	UINT32 ulDhcpv6ReqValSuffix = 0;
	MT_DHCPV6_OLV_T stDhcpv6IdntAssoc4NonTmpAdd;
	UINT32 ulNonTmpAddLen = 0;
	MT_DHCPV6_OLV_T stDhcpv6ClntIdent;
	UINT32 ulClntIdentLen = 0;
	MT_DHCPV6_OLV_T stDhcpv6SrvIdent;
	UINT16 usDuidType = 0;
	UINT16 usHWType = 0;
	UINT32 ulTime = 0;
	MT_DHCPV6_OLV_T stIaAddress;
	MT_PING6_REPLY_STA_INFO stPing6ReplyStaInfo;
	MT_PING_REPLY_STA_INFO stPingReplyStaInfo;
	UINT8 ucRadioId = 0;
	UINT8 ucStaWlanIndex = 0;//用户需要接入的WLAN标识
	UINT8 ucWlanNum = 0;
	MT_80211AUTHENTICATION_T st80211AuthFlagInfo;
	MT_80211WLANMANAGMNTFRAME_FIXED_PARA_T st80211WlanMangmntFrameFixedPara;
	MT_80211WLANMANAGMNTFRAME_TAGGED_PARA_T st80211WlanMangmntFrameTaggedPara;
	MT_STA_AUTH_INFO stStaAuthInfo;
	MT_STA_WLAN_INFO_T stStaWLanInfo;
	UINT32 ulSuffix = 0;
	IEEE8021XAUTH_REQ_T stIeee8021xAuthReq;
	UINT32 ul8021xAuthReqLen = 0;
	MT_8021X_EAPOL_INFO_T st8021xEapolInfo;
	MT_WAI_AUTH_MSG_INFO_T stWaiAuthInfo;
	UINT32 ulWaiAuthInfoLen = 0;

	if(NULL == vpstMsgHead)
	{
		gstErrMsgStatics.ul5247MsgHead_NULL++;
		return;
	}

	cpss_mem_memset(&stEthHdr,0,sizeof(MT_ETH_HDR_T));
	cpss_mem_memset(&stArp,0,sizeof(MT_ARP_REQ_T));
	cpss_mem_memset(&stVlanHdr,0,sizeof(MT_VLAN_HEADER_T));
	cpss_mem_memset(&stIpv6Hdr,0,sizeof(MT_IPV6_HEADER_T));
	cpss_mem_memset(&stNeighbSolicit,0,sizeof(MT_ICMPV6_NEIGHBOR_ADVERTISEMENT_T));
	cpss_mem_memset(&stIpv4Hdr,0,sizeof(MT_IPV4_HEADER_T));
	cpss_mem_memset(&stIcmpV4Hdr,0,sizeof(MT_STA_ICMPV4_T));
	cpss_mem_memset(&stUdpHdr,0,sizeof(MT_UDP_HDR_T));
	cpss_mem_memset(&stBootStrapHdr,0,sizeof(MT_BOOTSTRAP_HDR_T));
	cpss_mem_memset(&stOption53,0,sizeof(MT_DHCP_OPTION_T));
	cpss_mem_memset(&stStaIcmpv6,0,sizeof(MT_ICMPV6_T));
	cpss_mem_memset(&stDhcpv6Req,0,sizeof(MT_DHCPV6_T));
	cpss_mem_memset(&stDhcpv6IdntAssoc4NonTmpAdd,0,sizeof(MT_DHCPV6_OLV_T));
	cpss_mem_memset(&stDhcpv6ClntIdent,0,sizeof(MT_DHCPV6_OLV_T));
	cpss_mem_memset(&stDhcpv6SrvIdent,0,sizeof(MT_DHCPV6_OLV_T));
	cpss_mem_memset(&stIaAddress,0,sizeof(MT_DHCPV6_OLV_T));
	cpss_mem_memset(&stPing6ReplyStaInfo,0,sizeof(MT_PING6_REPLY_STA_INFO));
	cpss_mem_memset(&stPingReplyStaInfo,0,sizeof(MT_PING_REPLY_STA_INFO));
	cpss_mem_memset(&st80211AuthFlagInfo,0,sizeof(MT_80211AUTHENTICATION_T));
	cpss_mem_memset(&st80211WlanMangmntFrameFixedPara,0,sizeof(MT_80211WLANMANAGMNTFRAME_FIXED_PARA_T));
	cpss_mem_memset(&st80211WlanMangmntFrameTaggedPara,0,sizeof(MT_80211WLANMANAGMNTFRAME_TAGGED_PARA_T));
	cpss_mem_memset(&stStaAuthInfo,0,sizeof(MT_STA_AUTH_INFO));
	cpss_mem_memset(&stStaWLanInfo,0,sizeof(MT_STA_WLAN_INFO_T));
	cpss_mem_memset(&stIeee8021xAuthReq,0,sizeof(IEEE8021XAUTH_REQ_T));
	cpss_mem_memset(&st8021xEapolInfo,0,sizeof(MT_8021X_EAPOL_INFO_T));
	cpss_mem_memset(&stWaiAuthInfo,0,sizeof(MT_WAI_AUTH_MSG_INFO_T));
	cpss_mem_memset(&stDisptchrMsg,0,sizeof(MT_DISPATCHER_SND_MSG_T));

	cpss_mem_memcpy(&stDisptchrMsg,vpstMsgHead->pucBuf,vpstMsgHead->ulLen);
	ulApIdx = stDisptchrMsg.ulApIdx;

	if((ulApIdx >= gulCmdStartApNum)||(ulApIdx < 0))
	{
		gstErrMsgStatics.ulRcv5247MsgErrApId++;
		MT_ApLog(ulApIdx,"AP not online.");
		return;
	}

	cpss_mem_memcpy(aucCapwapBuf,stDisptchrMsg.aucCapwapBuf,stDisptchrMsg.ulCapwapBufLen);

	ulCapwapSuffix = 0;
	cpss_mem_memcpy(&ulCWFirst32,&aucCapwapBuf[ulCapwapSuffix],sizeof(ulCWFirst32));
	ulCapwapSuffix = ulCapwapSuffix + sizeof(ulCWFirst32);
	cpss_mem_memcpy(&ulCWSecond32,&aucCapwapBuf[ulCapwapSuffix],sizeof(ulCWSecond32));
	ulCapwapSuffix = ulCapwapSuffix + sizeof(ulCWSecond32);

	ulCWFirst32 = htonl(ulCWFirst32);

	MT_GetField32(ulCWFirst32,MT_CAPWAP_HDR_VERSION_START,MT_CAPWAP_HDR_VERSION_LEN,ucVerFalg);
	MT_GetField32(ulCWFirst32,MT_CAPWAP_HDR_TYPE_START,MT_CAPWAP_HDR_TYPE_LEN,ucTypeFalg);
	MT_GetField32(ulCWFirst32,MT_CAPWAP_HDR_HLEN_START,MT_CAPWAP_HDR_HLEN_LEN,ucHdrLen);
	MT_GetField32(ulCWFirst32,MT_CAPWAP_HDR_RID_START,MT_CAPWAP_HDR_RID_LEN,ucRId);
	MT_GetField32(ulCWFirst32,MT_CAPWAP_HDR_WBID_START,MT_CAPWAP_HDR_WBID_LEN,ucWbId);
	MT_GetField32(ulCWFirst32,MT_CAPWAP_HDR_T_START,MT_CAPWAP_HDR_T_LEN,bTflag);
	MT_GetField32(ulCWFirst32,MT_CAPWAP_HDR_F_START,MT_CAPWAP_HDR_F_LEN,bFflag);
	MT_GetField32(ulCWFirst32,MT_CAPWAP_HDR_L_START,MT_CAPWAP_HDR_L_LEN,bLflag);
	MT_GetField32(ulCWFirst32,MT_CAPWAP_HDR_W_START,MT_CAPWAP_HDR_W_LEN,bWflag);
	MT_GetField32(ulCWFirst32,MT_CAPWAP_HDR_M_START,MT_CAPWAP_HDR_M_LEN,bMflag);
	MT_GetField32(ulCWFirst32,MT_CAPWAP_HDR_K_START,MT_CAPWAP_HDR_K_LEN,bKflag);
	MT_GetField32(ulCWFirst32,MT_CAPWAP_HDR_FLAGS_START,MT_CAPWAP_HDR_FLAGS_LEN,ucFlags);

	//Payload Type == IEEE 802.3 Frame
	if(0 == bTflag)
	{
		//KeepAlive
		if(1 == bKflag)
		{
			gastAp[ulApIdx].bKeepAliveFlag = MT_TRUE;
			gstMutiTest_Statics.ulApRecvKeepAlive++;
			return;
		}

		//ETH Header
		ulCapwapSuffix = 16;
		cpss_mem_memcpy(&stEthHdr,&aucCapwapBuf[ulCapwapSuffix],sizeof(MT_ETH_HDR_T));
		ulCapwapSuffix = ulCapwapSuffix + sizeof(MT_ETH_HDR_T);
		ulStaIndex = getStaIndexByMac(stEthHdr.aucDstMac);

		//Illegal StaIndex
		if(MT_UNDEFINED_OBJECT_IDENTIFIER == ulStaIndex)
		{
			gstErrMsgStatics.ulRcv5247MsgErrStaId++;
			MT_StaLog(ulStaIndex,"Sta not Online.");
			return;
		}

		//according to Ethernet Header中的protocol区分不同的协议过程
		usProtclType = htons(stEthHdr.usProtclType);

		switch(usProtclType)
		{
		//IPv4
		case 0x0800:
		{
			cpss_mem_memcpy(&stIpv4Hdr,&aucCapwapBuf[ulCapwapSuffix],sizeof(MT_IPV4_HEADER_T));
			ulCapwapSuffix = ulCapwapSuffix + sizeof(MT_IPV4_HEADER_T);

			switch(stIpv4Hdr.ucProtoType)
			{
			//ICMP
			case 0x01:
			{
				cpss_mem_memcpy(&stIcmpV4Hdr,&aucCapwapBuf[ulCapwapSuffix],sizeof(MT_STA_ICMPV4_T));

				//PING REPLY
				if(0x00 == stIcmpV4Hdr.ucIpProtoType)
				{
					gstMutiTest_Statics.ulStaRcvPingReply++;
					break;
				}

				if(0x08 == stIcmpV4Hdr.ucIpProtoType)
				{
					gstMutiTest_Statics.ulStaRecvPingReq++;

					if(ASSOCIATION_SUCESS_STATE != gastStaPara[ulStaIndex].enAuthState && DHCPV4_SUCCESS_STATE != gastStaPara[ulStaIndex].enDhcpState)
					{
						break;
					}

					cpss_mem_memset(&stPingReplyStaInfo,0,sizeof(stPingReplyStaInfo));
					stPingReplyStaInfo.usIdentifier = stIcmpV4Hdr.usBeIdentifier;
					stPingReplyStaInfo.usSeqNum = stIcmpV4Hdr.usSeqNum;
					cpss_mem_memcpy(stPingReplyStaInfo.aucDstMac,stEthHdr.aucSrcMac,MT_MAC_LENTH);
					cpss_mem_memcpy(stPingReplyStaInfo.aucDstIpv4Addr,stIpv4Hdr.aucSrcIpaddr,MT_IPV4_ADDR_LEN);
					cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
					stStaWLanInfo.ulApIndex = ulApIdx;
					stStaWLanInfo.ulStaIndex = ulStaIndex;
					ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
					stStaWLanInfo.ucRadioId = ucRadioId;
//					ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

					if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
					{
						gstErrMsgStatics.ulRcv5247MsgErr++;
						MT_StaLog(ulStaIndex,"Local AP has No WLAN Service.");
						break;
					}

					if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
					{
						break;
					}

					stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//					ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//					stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
					bSendResult = ApMutiTestSendPingReply(stStaWLanInfo, stPingReplyStaInfo);

					if(MT_TRUE == bSendResult)
					{
						gstMutiTest_Statics.ulStaSendPingReply++;
					}
					else
					{
						gstErrMsgStatics.ulSendPingReplyErr++;
					}
				}
			}
			break;

			//UDP DHCP
			case 0x11:
			{
				cpss_mem_memcpy(&stUdpHdr,&aucCapwapBuf[ulCapwapSuffix],sizeof(stUdpHdr));

				if(67 != htons(stUdpHdr.usSrcPort) && 68 != htons(stUdpHdr.usDstPort))
				{
					if(WAIT_DHCP_OFFER_STATE == gastStaPara[ulStaIndex].enDhcpState)
					{
						gstErrMsgStatics.ulRcvDhcpOfferErr++;
					}

					if(WAIT_DHCP_ACK_STATE == gastStaPara[ulStaIndex].enDhcpState)
					{
						gstErrMsgStatics.ulRcvDhcpAckErr++;
					}

					MT_StaLog(ulStaIndex,"UDP port not equal to 67 or 68.");
					break;
				}

				//overfly BootStrap header(240)
				ulCapwapSuffix = ulCapwapSuffix + sizeof(stUdpHdr) + sizeof(stBootStrapHdr);
				cpss_mem_memcpy(&stOption53,&aucCapwapBuf[ulCapwapSuffix],3);

				//DHCP Option53
				if(53 != stOption53.ucOption)
				{
					if(WAIT_DHCP_OFFER_STATE == gastStaPara[ulStaIndex].enDhcpState)
					{
						gstErrMsgStatics.ulRcvDhcpOfferErr++;
					}

					if(WAIT_DHCP_ACK_STATE == gastStaPara[ulStaIndex].enDhcpState)
					{
						gstErrMsgStatics.ulRcvDhcpAckErr++;
					}

					gstErrMsgStatics.ulRcv5247MsgErr++;
					MT_StaLog(ulStaIndex,"53 != stOption53.ucOption.");
					break;
				}
				else
				{
					switch(stOption53.aucOptnVal[0])
					{
					//Received DHCP Offer
					case 2:
					{
						if(WAIT_DHCP_OFFER_STATE != gastStaPara[ulStaIndex].enDhcpState)
						{
							gstErrMsgStatics.ulRcvDhcpOfferErr++;
							break;
						}

						gstMutiTest_Statics.ulStaRcvDhcpOffer++;

						//DHCP Request
						cpss_mem_memcpy(gastStaPara[ulStaIndex].aucStaIpV4Addr,stIpv4Hdr.aucDstIpAddr,MT_IPV4_ADDR_LEN);
						cpss_mem_memcpy(gastStaPara[ulStaIndex].aucStaGwIpv4Addr,stIpv4Hdr.aucSrcIpaddr,MT_IPV4_ADDR_LEN);
						cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
						stStaWLanInfo.ulApIndex = ulApIdx;
						stStaWLanInfo.ulStaIndex = ulStaIndex;
						ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
						stStaWLanInfo.ucRadioId = ucRadioId;
//						ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

						if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
						{
							gstErrMsgStatics.ulRcv5247MsgErr++;
							MT_StaLog(ulStaIndex,"Local AP has No WLAN Service.");
							break;
						}

						if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
						{
							break;
						}

						stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//						ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//						stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
						bSendResult = ApMutiTestSendDhcpReq(stStaWLanInfo);

						if(MT_TRUE == bSendResult)
						{
							gstMutiTest_Statics.ulStaSendDhcpReq++;
							gastStaPara[ulStaIndex].enDhcpState = WAIT_DHCP_ACK_STATE;
						}
						else
						{
							gstErrMsgStatics.ulSendDhcpReqErr++;
							gastStaPara[ulStaIndex].enDhcpState = DHCPV4_INIT_STATE;
						}
					}
					break;

					//Received DHCP ACK
					case 5:
					{
						if(WAIT_DHCP_ACK_STATE != gastStaPara[ulStaIndex].enDhcpState)
						{
							gstErrMsgStatics.ulRcvDhcpAckErr++;
							break;
						}

						gstMutiTest_Statics.ulStaRcvDhcpAck++;
						gastStaPara[ulStaIndex].enDhcpState = DHCPV4_SUCCESS_STATE;

						//Arp Req
						//不是splic Mac时才发送ping包
						if(1 != gastApconf[ulApIdx].ucWtpMacType)
						{
							//IPV4toIPv4
							if((IP_ADDR_TYPE_V4 == gulIpAddrFlag) && (MT_STA_TUNNEL_MOD_4TO4 == gucStaTunnelMod))
							{
								cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
								stStaWLanInfo.ulApIndex = ulApIdx;
								stStaWLanInfo.ulStaIndex = ulStaIndex;
								ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
								stStaWLanInfo.ucRadioId = ucRadioId;
//								ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

								if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
								{
									break;
								}

								if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
								{
								  break;
								}

								stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//								ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//								stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

								if(MT_AUTH_POLICY_WEPSHAREDKEY == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[stStaWLanInfo.ucWlanId].ucAuthType)
								{
									sleep(1);
								}

								if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState)
								{
									bSendResult = ApMutiTestSendArpReq(stStaWLanInfo);

									if(MT_TRUE == bSendResult)
									{
										gstMutiTest_Statics.ulStaSendArpReq++;
									}
									else
									{
										gstErrMsgStatics.ulSendArpReqErr++;
									}
								}
								break;
							}

							//IPv6InIpv4
							if((IP_ADDR_TYPE_V4 == gulIpAddrFlag) && (MT_STA_TUNNEL_MOD_6IN4 == gucStaTunnelMod))
							{
								cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
								stStaWLanInfo.ulApIndex = ulApIdx;
								stStaWLanInfo.ulStaIndex = ulStaIndex;
								ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
								stStaWLanInfo.ucRadioId = ucRadioId;
//								ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

								if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
								{
									break;
								}

								if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
								{
								  break;
								}

								stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//								ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//								stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

								if((ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState) && (DHCPV6_SUCCESS_STATE == gastStaPara[ulStaIndex].enDhcpv6State))
								{
									bSendResult = ApMutiTestSendPing6Req(stStaWLanInfo);

									if(MT_TRUE == bSendResult)
									{
										gstMutiTest_Statics.ulStaSendPing6Req++;
										gusSequenceNum++;
									}
									else
									{
										gstErrMsgStatics.ulSendDhcpv6ReqErr++;
									}
								}
								break;
							}

							//IPV4InIPv6
							if((IP_ADDR_TYPE_V6 == gulIpAddrFlag) && (MT_STA_TUNNEL_MOD_6IN4 == gucStaTunnelMod))
							{
								cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
								stStaWLanInfo.ulApIndex = ulApIdx;
								stStaWLanInfo.ulStaIndex = ulStaIndex;
								ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
								stStaWLanInfo.ucRadioId = ucRadioId;
//								ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

								if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
								{
									break;
								}

								if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
								{
								  break;
								}

								stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//								ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//								stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

								if(MT_AUTH_POLICY_WEPSHAREDKEY == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[stStaWLanInfo.ucWlanId].ucAuthType)
								{
									sleep(1);
								}

								if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState)
								{
									bSendResult = ApMutiTestSendArpReq(stStaWLanInfo);

									if(MT_TRUE == bSendResult)
									{
										gstMutiTest_Statics.ulStaSendArpReq++;
									}
									else
									{
										gstErrMsgStatics.ulSendArpReqErr++;
									}
								}
								break;
							}

							//IPv6toIpv6
							if((IP_ADDR_TYPE_V6 == gulIpAddrFlag) && (MT_STA_TUNNEL_MOD_6TO6 == gucStaTunnelMod))
							{
								cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
								stStaWLanInfo.ulApIndex = ulApIdx;
								stStaWLanInfo.ulStaIndex = ulStaIndex;
								ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
								stStaWLanInfo.ucRadioId = ucRadioId;
//								ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

								if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
								{
									break;
								}

								if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
								{
								  break;
								}

								stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//								ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//								stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];;
								if((ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState) && (DHCPV6_SUCCESS_STATE == gastStaPara[ulStaIndex].enDhcpv6State))
								{
									bSendResult = ApMutiTestSendPing6Req(stStaWLanInfo);
									if(MT_TRUE == bSendResult)
									{
										gstMutiTest_Statics.ulStaSendPing6Req++;
										gusSequenceNum++;
									}
									else
									{
										gstErrMsgStatics.ulSendDhcpv6ReqErr++;
									}
								}
								break;
							}
							else
							{
								cpss_mem_memset(aucStaTmpInfo,0,128);
								sprintf((char*)aucStaTmpInfo,"STA Tunnel Failure. IpAddrFlag: %d. StaTunnelMod: %d.",gulIpAddrFlag,gucStaTunnelMod);
								MT_StaLog(ulStaIndex,(char*)aucStaTmpInfo);
							}
						}
					}
						break;

					default:
					{
						gstErrMsgStatics.ulRcv5247MsgErr++;
						MT_StaLog(ulStaIndex,"Option53.aucOptnVal[0] not 2 or 5.");
					}
					break;
					}
				}
			}
			break;

			default:
			{
				gstErrMsgStatics.ulRcv5247MsgErr++;
				MT_StaLog(ulStaIndex,"stIpv4Hdr.ucProtoType is unknown.");
			}
			break;
			}
		}
		break;

		//ARP
		case 0x0806:
		{
			gstMutiTest_Statics.ulStaRcvArpReq++;
		}
		break;

		case 0x0000:
		{
			gstErrMsgStatics.ulRcv5247MsgErr++;
			MT_StaLog(ulStaIndex,"stEthHdr.usProtclType is unknown.");
		}
		break;

		case 0x0608:
		{
			gstErrMsgStatics.ulRcv5247MsgErr++;
			MT_StaLog(ulStaIndex,"stEthHdr.usProtclType is unknown.");
			break;

			cpss_mem_memcpy(&stArp,&aucCapwapBuf[ulCapwapSuffix],sizeof(stArp));
			//Opcode:ARP Request
			if(0x0001 == htons(stArp.usOpcode))
			{
				if(ASSOCIATION_SUCESS_STATE != gastStaPara[ulStaIndex].enAuthState)
				{
					break;
				}

				gstMutiTest_Statics.ulStaRcvArpReq++;
				ulStaIndex = stArp.aucDstIpv4Addr[3] - 0x01;

				cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));

				stStaWLanInfo.ulApIndex = ulApIdx;
				stStaWLanInfo.ulStaIndex = ulStaIndex;
				ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
				stStaWLanInfo.ucRadioId = ucRadioId;
//				ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

				if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
				{
					break;
				}

				if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
				{
				  break;
				}

				stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//				ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//				stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

				bSendResult = ApMutiTestSendArpReply(stStaWLanInfo);//Arp reply
				if(MT_TRUE == bSendResult)
				{
					gstMutiTest_Statics.ulStaSendArpReply++;
				}
				else
				{
					gstErrMsgStatics.ulSendArpReplyErr++;
				}
			}
			else if(0x0002 == htons(stArp.usOpcode))
			{
				//Opcode:ARP Reply
				gstMutiTest_Statics.ulStaRcvArpReply++;
				cpss_mem_memcpy(gastStaPara[ulStaIndex].auGwMac,stArp.aucSrcMac,MT_MAC_LENTH);

				cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));

				stStaWLanInfo.ulApIndex = ulApIdx;
				stStaWLanInfo.ulStaIndex = ulStaIndex;
				ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
				stStaWLanInfo.ucRadioId = ucRadioId;
//				ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

				if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
				{
					break;
				}

				if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
				{
				  break;
				}

				stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//				ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//				stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

				bSendResult = ApMutiTestSendPingReq(stStaWLanInfo);
				if(MT_TRUE == bSendResult)
				{
					gstMutiTest_Statics.ulStaSendPingReq++;
					gusSequenceNum++;
				}
				else
				{
					gstErrMsgStatics.ulSendPingReqErr++;
				}
			}
		}
		break;

		//IPv6
		case 0x86dd:
		{
			gstErrMsgStatics.ulRcv5247MsgErr++;
			MT_StaLog(ulStaIndex,"stEthHdr.usProtclType is unknown.");
			break;

			cpss_mem_memcpy(&stIpv6Hdr,&aucCapwapBuf[ulCapwapSuffix],sizeof(stIpv6Hdr));
			//ICMPv6
			if(0x3a == stIpv6Hdr.ucNextHdr)
			{
				ulCapwapSuffix = ulCapwapSuffix + sizeof(stIpv6Hdr);

				//NeighbrAdvertsmnt
				cpss_mem_memcpy(&stNeighbSolicit,&aucCapwapBuf[ulCapwapSuffix],sizeof(stNeighbSolicit));

				if(135 == stNeighbSolicit.ucProtclType)
				{
					cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
					stStaWLanInfo.ulApIndex = ulApIdx;
					stStaWLanInfo.ulStaIndex = ulStaIndex;
					ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
					stStaWLanInfo.ucRadioId = ucRadioId;
//					ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

					if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
					{
						break;
					}

					if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
					{
					  break;
					}

					stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//					ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//					stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

					bSendResult = ApMutiTestSendNeighbrAdvertsmnt(stStaWLanInfo);
					if(MT_TRUE == bSendResult)
					{
						;
					}
					else
					{
						;
					}
					break;
				}

				//Ping6Reply
				cpss_mem_memcpy(&stStaIcmpv6,&aucCapwapBuf[ulCapwapSuffix],sizeof(stStaIcmpv6));
				if(129 == stStaIcmpv6.ucIpProtoType)
				{
					gstMutiTest_Statics.ulStaRcvPing6Reply++;
					break;
				}
				else if(128 == stStaIcmpv6.ucIpProtoType)
				{
					gstMutiTest_Statics.ulStaRcvPing6Req++;
					cpss_mem_memset(&stPing6ReplyStaInfo,0,sizeof(stPing6ReplyStaInfo));
					stPing6ReplyStaInfo.usSeqNum = stStaIcmpv6.usSeqNum;
					stPing6ReplyStaInfo.usIdentifier = stStaIcmpv6.usIdentifier;
					cpss_mem_memcpy(&stPing6ReplyStaInfo.aucDstMac,stEthHdr.aucSrcMac,MT_MAC_LENTH);
					cpss_mem_memcpy(&stPing6ReplyStaInfo.aucDstIpv6Addr,stIpv6Hdr.aucSrcIpV6addr,MT_IPV6_ADDR_LEN);

					cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
					stStaWLanInfo.ulApIndex = ulApIdx;
					stStaWLanInfo.ulStaIndex = ulStaIndex;
					ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
					stStaWLanInfo.ucRadioId = ucRadioId;
//					ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

					if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
					{
						break;
					}

					if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
					{
					  break;
					}

					stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//					ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//					stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
					bSendResult = ApMutiTestSendPing6Reply(stStaWLanInfo, stPing6ReplyStaInfo);

					if(MT_TRUE == bSendResult)
					{
						gstMutiTest_Statics.ulStaSendPing6Reply++;
					}
					else
					{
						gstErrMsgStatics.ulSendPing6ReqErr++;
					}
					break;
				}
			}
			else if(0x11 == stIpv6Hdr.ucNextHdr)
			{
				//UDP
				ulCapwapSuffix = ulCapwapSuffix + sizeof(stIpv6Hdr);
				cpss_mem_memcpy(&stUdpHdr,&aucCapwapBuf[ulCapwapSuffix],sizeof(stUdpHdr));
				if(MT_DHCPV6_DST_PORT != htons(stUdpHdr.usSrcPort) && MT_DHCPV6_SRC_PORT != htons(stUdpHdr.usDstPort))
				{
					break;
				}

				//DHCPV6
				ulCapwapSuffix = ulCapwapSuffix + sizeof(stUdpHdr);
				cpss_mem_memcpy(&stDhcpv6Req,&aucCapwapBuf[ulCapwapSuffix],htons(stUdpHdr.usPldLen) - sizeof(stUdpHdr));

				//Advertise
				if(0x02 == (stDhcpv6Req.ulMsgTypeTransId >> 24) && WAIT_DHCPV6_ADVERTISE_STATE == gastStaPara[ulStaIndex].enDhcpv6State)
				{
					//Identify Association for non-temporary address
					ulDhcpv6ReqValSuffix = 0;
					cpss_mem_memcpy(&stDhcpv6IdntAssoc4NonTmpAdd,&stDhcpv6Req.aucDhcpv6OptionVal[ulDhcpv6ReqValSuffix],4);//usOptionID & usLen
					stDhcpv6IdntAssoc4NonTmpAdd.usOptionID = htons(stDhcpv6IdntAssoc4NonTmpAdd.usOptionID);
					stDhcpv6IdntAssoc4NonTmpAdd.usLen = htons(stDhcpv6IdntAssoc4NonTmpAdd.usLen);

					if(3 != stDhcpv6IdntAssoc4NonTmpAdd.usOptionID)
					{
						break;
					}

					cpss_mem_memcpy(stDhcpv6IdntAssoc4NonTmpAdd.aucVal,&stDhcpv6Req.aucDhcpv6OptionVal[ulDhcpv6ReqValSuffix + 4],stDhcpv6IdntAssoc4NonTmpAdd.usLen);
					ulNonTmpAddLen = sizeof(stDhcpv6IdntAssoc4NonTmpAdd.usOptionID) + sizeof(stDhcpv6IdntAssoc4NonTmpAdd.usLen) + stDhcpv6IdntAssoc4NonTmpAdd.usLen;

					//skip over IAID(4bytes) ,T1(4bytes) and T2(4bytes)
					cpss_mem_memcpy(&stIaAddress,&stDhcpv6IdntAssoc4NonTmpAdd.aucVal[12],stDhcpv6IdntAssoc4NonTmpAdd.usLen - 12);

					if(5 != htons(stIaAddress.usOptionID))
					{
						break;
					}

					gstMutiTest_Statics.ulStaRcvDhcpv6Advertise++;

					//IA Address
					//Get STA IPv6 Address
					cpss_mem_memcpy(gastStaPara[ulStaIndex].aucStaIpV6Addr,stIaAddress.aucVal,MT_IPV6_ADDR_LEN);

					//construct STA Ipv6 GW Address
					cpss_mem_memcpy(gastStaPara[ulStaIndex].aucStaGwIpv6Addr,gastStaPara[ulStaIndex].aucStaIpV6Addr,8);
					gastStaPara[ulStaIndex].aucStaGwIpv6Addr[8] = 0x00;
					gastStaPara[ulStaIndex].aucStaGwIpv6Addr[9] = 0x00;
					gastStaPara[ulStaIndex].aucStaGwIpv6Addr[10] = 0x00;
					gastStaPara[ulStaIndex].aucStaGwIpv6Addr[11] = 0x00;
					gastStaPara[ulStaIndex].aucStaGwIpv6Addr[12] = 0x00;
					gastStaPara[ulStaIndex].aucStaGwIpv6Addr[13] = 0x00;
					gastStaPara[ulStaIndex].aucStaGwIpv6Addr[14] = 0x50;
					gastStaPara[ulStaIndex].aucStaGwIpv6Addr[15] = 0x00;

					//Client Identifier
					ulDhcpv6ReqValSuffix = ulDhcpv6ReqValSuffix + ulNonTmpAddLen;
					cpss_mem_memcpy(&stDhcpv6ClntIdent,&stDhcpv6Req.aucDhcpv6OptionVal[ulDhcpv6ReqValSuffix],4);//usOptionID & usLen
					stDhcpv6ClntIdent.usOptionID = htons(stDhcpv6ClntIdent.usOptionID);
					stDhcpv6ClntIdent.usLen = htons(stDhcpv6ClntIdent.usLen);

					//Client Identifier
					if(1 == stDhcpv6ClntIdent.usOptionID)
					{
						cpss_mem_memcpy(stDhcpv6ClntIdent.aucVal,&stDhcpv6Req.aucDhcpv6OptionVal[ulDhcpv6ReqValSuffix + 4],stDhcpv6ClntIdent.usLen);
						ulClntIdentLen = sizeof(stDhcpv6ClntIdent.usOptionID) + sizeof(stDhcpv6ClntIdent.usLen) + stDhcpv6ClntIdent.usLen;

						//server Identifier
						ulDhcpv6ReqValSuffix = ulDhcpv6ReqValSuffix + ulClntIdentLen;
						cpss_mem_memcpy(&stDhcpv6SrvIdent,&stDhcpv6Req.aucDhcpv6OptionVal[ulDhcpv6ReqValSuffix],4);//usOptionID & usLen
						stDhcpv6SrvIdent.usOptionID = htons(stDhcpv6SrvIdent.usOptionID);
						stDhcpv6SrvIdent.usLen = htons(stDhcpv6SrvIdent.usLen);

						//server Identifier
						if(2 == stDhcpv6SrvIdent.usOptionID)
						{
							cpss_mem_memcpy(stDhcpv6SrvIdent.aucVal,&stDhcpv6Req.aucDhcpv6OptionVal[ulDhcpv6ReqValSuffix + 4],stDhcpv6SrvIdent.usLen);
							ulSuffix = 0;
							cpss_mem_memcpy(&usDuidType,&stDhcpv6SrvIdent.aucVal[ulSuffix],sizeof(usDuidType));
							ulSuffix = ulSuffix + sizeof(usDuidType);
							cpss_mem_memcpy(&usHWType,&stDhcpv6SrvIdent.aucVal[ulSuffix],sizeof(usHWType));
							ulSuffix = ulSuffix + sizeof(usHWType);

							usDuidType = htons(usDuidType);
							usHWType = htons(usHWType);

							//link-layer address plus time
							if(1 == usDuidType)
							{
								cpss_mem_memcpy(&ulTime,&stDhcpv6SrvIdent.aucVal[ulSuffix],sizeof(ulTime));
								ulSuffix = ulSuffix + sizeof(ulTime);
							}

							//get DHCPv6 Srv Mac
							cpss_mem_memcpy(gastStaPara[ulStaIndex].auGwMac,&stDhcpv6SrvIdent.aucVal[ulSuffix],MT_MAC_LENTH);
						}
					}

					//DHCPV6 Request
					cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
					stStaWLanInfo.ulApIndex = ulApIdx;
					stStaWLanInfo.ulStaIndex = ulStaIndex;
					ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
					stStaWLanInfo.ucRadioId = ucRadioId;
//					ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

					if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
					{
						break;
					}

					if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
					{
					  break;
					}

					stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//					ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//					stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

					bSendResult = ApMutiTestSendDhcpv6Req(stStaWLanInfo);

					if(MT_TRUE == bSendResult)
					{
						gstMutiTest_Statics.ulStaSendDhcpv6Req++;
						gastStaPara[ulStaIndex].enDhcpv6State = WAIT_DHCPV6_REPLY_STATE;
						break;
					}
					else
					{
						gstErrMsgStatics.ulSendDhcpv6ReqErr++;
						gastStaPara[ulStaIndex].enDhcpv6State = DHCPV6_INIT_STATE;
						break;
					}
				}
				else if(0x07 == (stDhcpv6Req.ulMsgTypeTransId >> 24) && WAIT_DHCPV6_REPLY_STATE == gastStaPara[ulStaIndex].enDhcpv6State)
				{
					//DHCPv6 Reply
					gstMutiTest_Statics.ulStaRcvDhcpv6Reply++;
					gastStaPara[ulStaIndex].enDhcpv6State = DHCPV6_SUCCESS_STATE;
				}
			}
		}
		break;

		//VLAN
		case 0x8100:
		{
			cpss_mem_memcpy(&stVlanHdr,&aucCapwapBuf[ulCapwapSuffix],sizeof(stVlanHdr));
			ulCapwapSuffix = ulCapwapSuffix + sizeof(stVlanHdr);

			//根据Vlan Header中的protocol区分不同的协议过程
			switch(htons(stVlanHdr.usType))
			{
			//IPv4
			case 0x0800:
			{
				cpss_mem_memcpy(&stIpv4Hdr,&aucCapwapBuf[ulCapwapSuffix],sizeof(MT_IPV4_HEADER_T));
				ulCapwapSuffix = ulCapwapSuffix + sizeof(MT_IPV4_HEADER_T);

				switch(stIpv4Hdr.ucProtoType)
				{
				//ICMP
				case 0x01:
				{
					cpss_mem_memcpy(&stIcmpV4Hdr,&aucCapwapBuf[ulCapwapSuffix],sizeof(MT_STA_ICMPV4_T));

					//PING REPLY
					if(0x00 == stIcmpV4Hdr.ucIpProtoType)
					{
						gstMutiTest_Statics.ulStaRcvPingReply++;
						break;
					}

					if(0x08 == stIcmpV4Hdr.ucIpProtoType)
					{
						gstMutiTest_Statics.ulStaRecvPingReq++;

						if(ASSOCIATION_SUCESS_STATE != gastStaPara[ulStaIndex].enAuthState && DHCPV4_SUCCESS_STATE != gastStaPara[ulStaIndex].enDhcpState)
						{
							break;
						}

						cpss_mem_memset(&stPingReplyStaInfo,0,sizeof(stPingReplyStaInfo));
						stPingReplyStaInfo.usIdentifier = stIcmpV4Hdr.usBeIdentifier;
						stPingReplyStaInfo.usSeqNum = stIcmpV4Hdr.usSeqNum;
						cpss_mem_memcpy(stPingReplyStaInfo.aucDstMac,stEthHdr.aucSrcMac,MT_MAC_LENTH);
						cpss_mem_memcpy(stPingReplyStaInfo.aucDstIpv4Addr,stIpv4Hdr.aucSrcIpaddr,MT_IPV4_ADDR_LEN);
						cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
						stStaWLanInfo.ulApIndex = ulApIdx;
						stStaWLanInfo.ulStaIndex = ulStaIndex;
						ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
						stStaWLanInfo.ucRadioId = ucRadioId;
//						ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

						if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
						{
							gstErrMsgStatics.ulRcv5247MsgErr++;
							MT_StaLog(ulStaIndex,"Local AP has No WLAN Service.");
							break;
						}

						if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
						{
						  break;
						}

						stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//						ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//						stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
						bSendResult = ApMutiTestSendPingReply(stStaWLanInfo, stPingReplyStaInfo);

						if(MT_TRUE == bSendResult)
						{
							gstMutiTest_Statics.ulStaSendPingReply++;
						}
						else
						{
							gstErrMsgStatics.ulSendPingReplyErr++;
						}
					}
				}
				break;

				//UDP DHCP
				case 0x11:
				{
					cpss_mem_memcpy(&stUdpHdr,&aucCapwapBuf[ulCapwapSuffix],sizeof(stUdpHdr));

					if(67 != htons(stUdpHdr.usSrcPort) && 68 != htons(stUdpHdr.usDstPort))
					{
						if(WAIT_DHCP_OFFER_STATE == gastStaPara[ulStaIndex].enDhcpState)
						{
							gstErrMsgStatics.ulRcvDhcpOfferErr++;
						}

						if(WAIT_DHCP_ACK_STATE == gastStaPara[ulStaIndex].enDhcpState)
						{
							gstErrMsgStatics.ulRcvDhcpAckErr++;
						}

						MT_StaLog(ulStaIndex,"UDP port not equal to 67 or 68.");
						break;
					}

					//overfly BootStrap header(240)
					ulCapwapSuffix = ulCapwapSuffix + sizeof(stUdpHdr) + sizeof(stBootStrapHdr);
					cpss_mem_memcpy(&stOption53,&aucCapwapBuf[ulCapwapSuffix],3);

					//DHCP Option53
					if(53 != stOption53.ucOption)
					{
						if(WAIT_DHCP_OFFER_STATE == gastStaPara[ulStaIndex].enDhcpState)
						{
							gstErrMsgStatics.ulRcvDhcpOfferErr++;
						}

						if(WAIT_DHCP_ACK_STATE == gastStaPara[ulStaIndex].enDhcpState)
						{
							gstErrMsgStatics.ulRcvDhcpAckErr++;
						}

						gstErrMsgStatics.ulRcv5247MsgErr++;
						MT_StaLog(ulStaIndex,"53 != stOption53.ucOption.");
						break;
					}
					else
					{
						switch(stOption53.aucOptnVal[0])
						{
						//Received DHCP Offer
						case 2:
						{
							if(WAIT_DHCP_OFFER_STATE != gastStaPara[ulStaIndex].enDhcpState)
							{
								gstErrMsgStatics.ulRcvDhcpOfferErr++;
								break;
							}

							gstMutiTest_Statics.ulStaRcvDhcpOffer++;

							//DHCP Request
							cpss_mem_memcpy(gastStaPara[ulStaIndex].aucStaIpV4Addr,stIpv4Hdr.aucDstIpAddr,MT_IPV4_ADDR_LEN);
							cpss_mem_memcpy(gastStaPara[ulStaIndex].aucStaGwIpv4Addr,stIpv4Hdr.aucSrcIpaddr,MT_IPV4_ADDR_LEN);
							cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
							stStaWLanInfo.ulApIndex = ulApIdx;
							stStaWLanInfo.ulStaIndex = ulStaIndex;
							ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
							stStaWLanInfo.ucRadioId = ucRadioId;
//							ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

							if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
							{
								gstErrMsgStatics.ulRcv5247MsgErr++;
								MT_StaLog(ulStaIndex,"Local AP has No WLAN Service.");
								break;
							}

							if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
							{
							  break;
							}

							stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//							ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//							stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

							bSendResult = ApMutiTestSendDhcpReq(stStaWLanInfo);

							if(MT_TRUE == bSendResult)
							{
								gstMutiTest_Statics.ulStaSendDhcpReq++;
								gastStaPara[ulStaIndex].enDhcpState = WAIT_DHCP_ACK_STATE;
							}
							else
							{
								gstErrMsgStatics.ulSendDhcpReqErr++;
								gastStaPara[ulStaIndex].enDhcpState = DHCPV4_INIT_STATE;
							}
						}
						break;

						//Received DHCP ACK
						case 5:
						{
							if(WAIT_DHCP_ACK_STATE != gastStaPara[ulStaIndex].enDhcpState)
							{
								gstErrMsgStatics.ulRcvDhcpAckErr++;
								break;
							}

							gstMutiTest_Statics.ulStaRcvDhcpAck++;
							gastStaPara[ulStaIndex].enDhcpState = DHCPV4_SUCCESS_STATE;

							//Arp Req
							//不是splic Mac时才发送ping包
							if(1 != gastApconf[ulApIdx].ucWtpMacType)
							{
								//IPV4toIPv4
								if((IP_ADDR_TYPE_V4 == gulIpAddrFlag) && (MT_STA_TUNNEL_MOD_4TO4 == gucStaTunnelMod))
								{
									cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
									stStaWLanInfo.ulApIndex = ulApIdx;
									stStaWLanInfo.ulStaIndex = ulStaIndex;
									ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
									stStaWLanInfo.ucRadioId = ucRadioId;
//									ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

									if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
									{
										break;
									}

									if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
									{
									  break;
									}

									stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//									ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//									stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

									if(MT_AUTH_POLICY_WEPSHAREDKEY == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[stStaWLanInfo.ucWlanId].ucAuthType)
									{
										sleep(1);
									}

									if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState)
									{
										bSendResult = ApMutiTestSendArpReq(stStaWLanInfo);

										if(MT_TRUE == bSendResult)
										{
											gstMutiTest_Statics.ulStaSendArpReq++;
										}
										else
										{
											gstErrMsgStatics.ulSendArpReqErr++;
										}
									}
									break;
								}

								//IPv6InIpv4
								if((IP_ADDR_TYPE_V4 == gulIpAddrFlag) && (MT_STA_TUNNEL_MOD_6IN4 == gucStaTunnelMod))
								{
									cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
									stStaWLanInfo.ulApIndex = ulApIdx;
									stStaWLanInfo.ulStaIndex = ulStaIndex;
									ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
									stStaWLanInfo.ucRadioId = ucRadioId;
//									ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

									if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
									{
										break;
									}

									if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
									{
									  break;
									}

									stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//									ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//									stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

									if((ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState) && (DHCPV6_SUCCESS_STATE == gastStaPara[ulStaIndex].enDhcpv6State))
									{
										bSendResult = ApMutiTestSendPing6Req(stStaWLanInfo);

										if(MT_TRUE == bSendResult)
										{
											gstMutiTest_Statics.ulStaSendPing6Req++;
											gusSequenceNum++;
										}
										else
										{
											gstErrMsgStatics.ulSendDhcpv6ReqErr++;
										}
									}
									break;
								}

								//IPV4InIPv6
								if((IP_ADDR_TYPE_V6 == gulIpAddrFlag) && (MT_STA_TUNNEL_MOD_6IN4 == gucStaTunnelMod))
								{
									cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
									stStaWLanInfo.ulApIndex = ulApIdx;
									stStaWLanInfo.ulStaIndex = ulStaIndex;
									ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
									stStaWLanInfo.ucRadioId = ucRadioId;
//									ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

									if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
									{
										break;
									}

									if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
									{
									  break;
									}

									stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//									ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//									stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

									if(MT_AUTH_POLICY_WEPSHAREDKEY == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[stStaWLanInfo.ucWlanId].ucAuthType)
									{
										sleep(1);
									}

									if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState)
									{
										bSendResult = ApMutiTestSendArpReq(stStaWLanInfo);

										if(MT_TRUE == bSendResult)
										{
											gstMutiTest_Statics.ulStaSendArpReq++;
										}
										else
										{
											gstErrMsgStatics.ulSendArpReqErr++;
										}
									}
									break;
								}

								//IPv6toIpv6
								if((IP_ADDR_TYPE_V6 == gulIpAddrFlag) && (MT_STA_TUNNEL_MOD_6TO6 == gucStaTunnelMod))
								{
									cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
									stStaWLanInfo.ulApIndex = ulApIdx;
									stStaWLanInfo.ulStaIndex = ulStaIndex;
									ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
									stStaWLanInfo.ucRadioId = ucRadioId;
//									ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

									if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
									{
										break;
									}

									if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
									{
									  break;
									}

									stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//									ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//									stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];;
									if((ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState) && (DHCPV6_SUCCESS_STATE == gastStaPara[ulStaIndex].enDhcpv6State))
									{
										bSendResult = ApMutiTestSendPing6Req(stStaWLanInfo);
										if(MT_TRUE == bSendResult)
										{
											gstMutiTest_Statics.ulStaSendPing6Req++;
											gusSequenceNum++;
										}
										else
										{
											gstErrMsgStatics.ulSendDhcpv6ReqErr++;
										}
									}
									break;
								}
								else
								{
									cpss_mem_memset(aucStaTmpInfo,0,128);
									sprintf((char*)aucStaTmpInfo,"STA Tunnel Failure. IpAddrFlag: %d. StaTunnelMod: %d.",gulIpAddrFlag,gucStaTunnelMod);
									MT_StaLog(ulStaIndex,(char*)aucStaTmpInfo);
								}
							}
						}
							break;

						default:
						{
							gstErrMsgStatics.ulRcv5247MsgErr++;
							MT_StaLog(ulStaIndex,"Option53.aucOptnVal[0] not 2 or 5.");
						}
						break;
						}
					}
				}
				break;

				default:
				{
					gstErrMsgStatics.ulRcv5247MsgErr++;
					MT_StaLog(ulStaIndex,"stIpv4Hdr.ucProtoType is unknown.");
				}
				break;
				}
			}
			break;

			//ARP Request
			case 0x0608:
			{
				gstMutiTest_Statics.ulStaRcvArpReq++;
				break;

				cpss_mem_memcpy(&stArp,&aucCapwapBuf[ulCapwapSuffix],sizeof(stArp));

				//Opcode:Request
				if(0x0001 == htons(stArp.usOpcode))
				{
					gstMutiTest_Statics.ulStaRcvArpReq++;
					cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));

					stStaWLanInfo.ulApIndex = ulApIdx;
					stStaWLanInfo.ulStaIndex = ulStaIndex;
					ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
					stStaWLanInfo.ucRadioId = ucRadioId;
//					ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

					if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
					{
						break;
					}

					if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
					{
					  break;
					}

					stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//					ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//					stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
					bSendResult = ApMutiTestSendArpReply(stStaWLanInfo);//Arp reply

					if(MT_TRUE == bSendResult)
					{
						gstMutiTest_Statics.ulStaSendArpReply++;
					}
					else
					{
						gstErrMsgStatics.ulSendArpReplyErr++;
					}
				}
				else if(0x0002 == htons(stArp.usOpcode))
				{
					//Opcode:ARP Reply
					gstMutiTest_Statics.ulStaRcvArpReply++;
					cpss_mem_memcpy(gastStaPara[ulStaIndex].auGwMac,stArp.aucSrcMac,MT_MAC_LENTH);
					//cpss_mem_memcpy(gastAp[ulApIdx].astSta[ulStaIndex].aucStaGwIpv4Addr,stArp.aucSrcIpv4Addr,MT_IPV4_ADDR_LEN);

					cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
					stStaWLanInfo.ulApIndex = ulApIdx;
					stStaWLanInfo.ulStaIndex = ulStaIndex;
					ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
					stStaWLanInfo.ucRadioId = ucRadioId;
//					ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

					if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
					{
						break;
					}

					if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
					{
					  break;
					}

					stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//					ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//					stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

					bSendResult = ApMutiTestSendPingReq(stStaWLanInfo);

					if(MT_TRUE == bSendResult)
					{
						gstMutiTest_Statics.ulStaSendPingReq++;
						gusSequenceNum++;
					}
					else
					{
						gstErrMsgStatics.ulSendPingReqErr++;
					}
				}
			}
			break;

			//IPv6
			case 0x86dd:
			{
				gstErrMsgStatics.ulRcv5247MsgErr++;
				MT_StaLog(ulStaIndex,"unknown 0x86dd.");

				cpss_mem_memcpy(&stIpv6Hdr,&aucCapwapBuf[ulCapwapSuffix],sizeof(stIpv6Hdr));

				if(0x3a == stIpv6Hdr.ucNextHdr)
				{
					ulCapwapSuffix = ulCapwapSuffix + sizeof(stIpv6Hdr);
					cpss_mem_memcpy(&stNeighbSolicit,&aucCapwapBuf[ulCapwapSuffix],sizeof(stNeighbSolicit));

					if(135 == stNeighbSolicit.ucProtclType)
					{
						//STA Index
						cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
						stStaWLanInfo.ulApIndex = ulApIdx;
						stStaWLanInfo.ulStaIndex = ulStaIndex;
						ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
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

						bSendResult = ApMutiTestSendNeighbrAdvertsmnt(stStaWLanInfo);

						if(MT_TRUE == bSendResult)
						{
							;
						}
						else
						{
							;
						}

						break;
					}

					//Ping6Reply
					cpss_mem_memcpy(&stStaIcmpv6,&aucCapwapBuf[ulCapwapSuffix],sizeof(stStaIcmpv6));

					if(129 == stStaIcmpv6.ucIpProtoType)
					{
						gstMutiTest_Statics.ulStaRcvPing6Reply++;
						break;
					}
					else if(128 == stStaIcmpv6.ucIpProtoType)
					{
						gstMutiTest_Statics.ulStaRcvPing6Req++;
						cpss_mem_memset(&stPing6ReplyStaInfo,0,sizeof(stPing6ReplyStaInfo));
						stPing6ReplyStaInfo.usSeqNum = stStaIcmpv6.usSeqNum;
						stPing6ReplyStaInfo.usIdentifier = stStaIcmpv6.usIdentifier;
						cpss_mem_memcpy(&stPing6ReplyStaInfo.aucDstMac,stEthHdr.aucSrcMac,MT_MAC_LENTH);
						cpss_mem_memcpy(&stPing6ReplyStaInfo.aucDstIpv6Addr,stIpv6Hdr.aucSrcIpV6addr,MT_IPV6_ADDR_LEN);
						cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));

						stStaWLanInfo.ulApIndex = ulApIdx;
						stStaWLanInfo.ulStaIndex = ulStaIndex;
						ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
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

						bSendResult = ApMutiTestSendPing6Reply(stStaWLanInfo, stPing6ReplyStaInfo);

						if(MT_TRUE == bSendResult)
						{
							gstMutiTest_Statics.ulStaSendPing6Reply++;
						}
						else
						{
							gstErrMsgStatics.ulSendPing6ReqErr++;
						}
						break;
					}
				}
				else if(0x11 == stIpv6Hdr.ucNextHdr)
				{
					//UDP
					ulCapwapSuffix = ulCapwapSuffix + sizeof(stIpv6Hdr);
					cpss_mem_memcpy(&stUdpHdr,&aucCapwapBuf[ulCapwapSuffix],sizeof(stUdpHdr));

					if(MT_DHCPV6_DST_PORT != htons(stUdpHdr.usSrcPort) && MT_DHCPV6_SRC_PORT != htons(stUdpHdr.usDstPort))
					{
						break;
					}

					//DHCPV6
					ulCapwapSuffix = ulCapwapSuffix + sizeof(stUdpHdr);
					cpss_mem_memcpy(&stDhcpv6Req,&aucCapwapBuf[ulCapwapSuffix],htons(stUdpHdr.usPldLen) - sizeof(stUdpHdr));

					//Identify Association for non-temporary address
					if(0x02 == (stDhcpv6Req.ulMsgTypeTransId >> 24))
					{
						ulDhcpv6ReqValSuffix = 0;
						cpss_mem_memcpy(&stDhcpv6IdntAssoc4NonTmpAdd,&stDhcpv6Req.aucDhcpv6OptionVal[ulDhcpv6ReqValSuffix],4);//usOptionID & usLen
						stDhcpv6IdntAssoc4NonTmpAdd.usOptionID = htons(stDhcpv6IdntAssoc4NonTmpAdd.usOptionID);
						stDhcpv6IdntAssoc4NonTmpAdd.usLen = htons(stDhcpv6IdntAssoc4NonTmpAdd.usLen);

						if(3 != stDhcpv6IdntAssoc4NonTmpAdd.usOptionID)
						{
							break;
						}

						cpss_mem_memcpy(stDhcpv6IdntAssoc4NonTmpAdd.aucVal,&stDhcpv6Req.aucDhcpv6OptionVal[ulDhcpv6ReqValSuffix + 4],stDhcpv6IdntAssoc4NonTmpAdd.usLen);
						ulNonTmpAddLen = sizeof(stDhcpv6IdntAssoc4NonTmpAdd.usOptionID) + sizeof(stDhcpv6IdntAssoc4NonTmpAdd.usLen) + stDhcpv6IdntAssoc4NonTmpAdd.usLen;
						//skip over IAID(4bytes) ,T1(4bytes) and T2(4bytes)
						cpss_mem_memcpy(&stIaAddress,&stDhcpv6IdntAssoc4NonTmpAdd.aucVal[12],stDhcpv6IdntAssoc4NonTmpAdd.usLen - 12);

						if(5 != htons(stIaAddress.usOptionID))
						{
							break;
						}

						//IA Address
						gstMutiTest_Statics.ulStaRcvDhcpv6Advertise++;

						//Get STA IPv6 Address
						cpss_mem_memcpy(gastStaPara[ulStaIndex].aucStaIpV6Addr,stIaAddress.aucVal,MT_IPV6_ADDR_LEN);

						//construct STA Ipv6 GW Address
						cpss_mem_memcpy(gastStaPara[ulStaIndex].aucStaGwIpv6Addr,gastStaPara[ulStaIndex].aucStaIpV6Addr,8);
						gastStaPara[ulStaIndex].aucStaGwIpv6Addr[8] = 0x00;
						gastStaPara[ulStaIndex].aucStaGwIpv6Addr[9] = 0x00;
						gastStaPara[ulStaIndex].aucStaGwIpv6Addr[10] = 0x00;
						gastStaPara[ulStaIndex].aucStaGwIpv6Addr[11] = 0x00;
						gastStaPara[ulStaIndex].aucStaGwIpv6Addr[12] = 0x00;
						gastStaPara[ulStaIndex].aucStaGwIpv6Addr[13] = 0x00;
						gastStaPara[ulStaIndex].aucStaGwIpv6Addr[14] = 0x50;
						gastStaPara[ulStaIndex].aucStaGwIpv6Addr[15] = 0x00;

						//Client Identifier
						ulDhcpv6ReqValSuffix = ulDhcpv6ReqValSuffix + ulNonTmpAddLen;
						cpss_mem_memcpy(&stDhcpv6ClntIdent,&stDhcpv6Req.aucDhcpv6OptionVal[ulDhcpv6ReqValSuffix],4);//usOptionID & usLen
						stDhcpv6ClntIdent.usOptionID = htons(stDhcpv6ClntIdent.usOptionID);
						stDhcpv6ClntIdent.usLen = htons(stDhcpv6ClntIdent.usLen);

						//Client Identifier
						if(1 == stDhcpv6ClntIdent.usOptionID)
						{
							cpss_mem_memcpy(stDhcpv6ClntIdent.aucVal,&stDhcpv6Req.aucDhcpv6OptionVal[ulDhcpv6ReqValSuffix + 4],stDhcpv6ClntIdent.usLen);
							ulClntIdentLen = sizeof(stDhcpv6ClntIdent.usOptionID) + sizeof(stDhcpv6ClntIdent.usLen) + stDhcpv6ClntIdent.usLen;

							//server Identifier
							ulDhcpv6ReqValSuffix = ulDhcpv6ReqValSuffix + ulClntIdentLen;
							cpss_mem_memcpy(&stDhcpv6SrvIdent,&stDhcpv6Req.aucDhcpv6OptionVal[ulDhcpv6ReqValSuffix],4);//usOptionID & usLen
							stDhcpv6SrvIdent.usOptionID = htons(stDhcpv6SrvIdent.usOptionID);
							stDhcpv6SrvIdent.usLen = htons(stDhcpv6SrvIdent.usLen);

							//server Identifier
							if(2 == stDhcpv6SrvIdent.usOptionID)
							{
								cpss_mem_memcpy(stDhcpv6SrvIdent.aucVal,&stDhcpv6Req.aucDhcpv6OptionVal[ulDhcpv6ReqValSuffix + 4],stDhcpv6SrvIdent.usLen);
								ulSuffix = 0;
								cpss_mem_memcpy(&usDuidType,&stDhcpv6SrvIdent.aucVal[ulSuffix],sizeof(usDuidType));
								ulSuffix = ulSuffix + sizeof(usDuidType);
								cpss_mem_memcpy(&usHWType,&stDhcpv6SrvIdent.aucVal[ulSuffix],sizeof(usHWType));
								ulSuffix = ulSuffix + sizeof(usHWType);

								usDuidType = htons(usDuidType);
								usHWType = htons(usHWType);

								//link-layer address plus time
								if(1 == usDuidType)
								{
									cpss_mem_memcpy(&ulTime,&stDhcpv6SrvIdent.aucVal[ulSuffix],sizeof(ulTime));
									ulSuffix = ulSuffix + sizeof(ulTime);
								}

								//get DHCPv6 Srv Mac
								cpss_mem_memcpy(gastStaPara[ulStaIndex].auGwMac,&stDhcpv6SrvIdent.aucVal[ulSuffix],MT_MAC_LENTH);
							}
						}

						//DHCPV6 Request
						cpss_mem_memset(&stStaWLanInfo,0,sizeof(stStaWLanInfo));
						stStaWLanInfo.ulApIndex = ulApIdx;
						stStaWLanInfo.ulStaIndex = ulStaIndex;
						ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
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

						bSendResult = ApMutiTestSendDhcpv6Req(stStaWLanInfo);

						if(MT_TRUE == bSendResult)
						{
							gstMutiTest_Statics.ulStaSendDhcpv6Req++;
							gastStaPara[ulStaIndex].enDhcpv6State = WAIT_DHCPV6_REPLY_STATE;
							break;
						}
						else
						{
							gstErrMsgStatics.ulSendDhcpv6ReqErr++;
							gastStaPara[ulStaIndex].enDhcpv6State = DHCPV6_INIT_STATE;
							break;
						}
					}
					else if(0x07 == (stDhcpv6Req.ulMsgTypeTransId >> 24))
					{
						//DHCPV6 Reply
						gstMutiTest_Statics.ulStaRcvDhcpv6Reply++;
						gastStaPara[ulStaIndex].enDhcpv6State = DHCPV6_SUCCESS_STATE;
					}
				}
			}
			break;

			default:
			{
				gstErrMsgStatics.ulRcv5247UnknwnMsg++;
				MT_StaLog(ulStaIndex,"Unknown stVlanHdr.usType.");
			}
			break;
		}
		}
		break;//VLAN

		//802.1x Authentication
		case 0x888E:
		{
			gstErrMsgStatics.ulRcv5247UnknwnMsg++;
			MT_StaLog(ulStaIndex,"Unknown 0x888E.");
			break;

			ul8021xAuthReqLen = htons(stUdpHdr.usPldLen) - sizeof(stUdpHdr) - ulCapwapSuffix;

			if(ul8021xAuthReqLen > sizeof(stIeee8021xAuthReq))
			{
				printf("ERROR. ul8021xAuthReqLen overflow.\n File: %s. Line: %d\n",__FILE__,__LINE__);
				break;
			}

			cpss_mem_memcpy(&stIeee8021xAuthReq,&aucCapwapBuf[ulCapwapSuffix],ul8021xAuthReqLen);

			st8021xEapolInfo.usKeyInfo = stIeee8021xAuthReq.usKeyInfo;
			st8021xEapolInfo.ulReplCntr1 = stIeee8021xAuthReq.ulRplCntr1;
			st8021xEapolInfo.ulReplCntr2 = stIeee8021xAuthReq.ulRplCntr2;
			cpss_mem_memcpy(st8021xEapolInfo.aucNonce,stIeee8021xAuthReq.aucNonce,IEEE8021XAUTH_NONCE_LEN);
			st8021xEapolInfo.ulWapKeyLen = htons(stIeee8021xAuthReq.usWapKeyLen);
			cpss_mem_memcpy(st8021xEapolInfo.aucWapKey,stIeee8021xAuthReq.aucWapKey,st8021xEapolInfo.ulWapKeyLen);

			bSendResult = ApMutiTestSend8021xAuthRsp(ulApIdx,ulStaIndex,st8021xEapolInfo);
			if(MT_TRUE == bSendResult)
			{
				;
			}
			else
			{
				;
			}
		}
		break;

		//WAPI Authentication
		case 0x88B4:
		{
			gstErrMsgStatics.ulRcv5247MsgErr++;
			MT_StaLog(ulStaIndex,"Unknown 0x88B4.");
			break;

			ulWaiAuthInfoLen = htons(stUdpHdr.usPldLen) - sizeof(stUdpHdr) - ulCapwapSuffix;
			if(ulWaiAuthInfoLen > sizeof(stWaiAuthInfo))
			{
				printf("ERROR. ulWaiAuthInfoLen overflow.\n File: %s. Line: %d\n",__FILE__,__LINE__);
				break;
			}

			cpss_mem_memcpy(&stWaiAuthInfo,&aucCapwapBuf[ulCapwapSuffix],ulWaiAuthInfoLen);
			if(WAI_TYPE_WAI_PROTOCOL_PACKAGE != stWaiAuthInfo.ucType)
			{
				printf("ERROR. MSG is not WAI_TYPE_WAI_PROTOCOL_PACKAGE.\nFile: %s, Line: %d\n",__FILE__,__LINE__);
				break;
			}

			if(WAI_SUBTYPE_UNICASTKEY_NEGOTIATION_REQ == stWaiAuthInfo.ucSubType)
			{
				bSendResult = ApMutiTestSendUcstKeyNegotiationRsp(ulApIdx, ulStaIndex,stWaiAuthInfo);
				break;
			}
			else if(WAI_SUBTYPE_UNICASTKEY_NEGOTIATION_CFM == stWaiAuthInfo.ucSubType)
			{
				break;
			}
			else if(WAI_SUBTYPE_MULTICASTKEY_ANNOUNCEMENT == stWaiAuthInfo.ucSubType)
			{
				bSendResult = ApMutiTestSendMcstKeyAnnouncmntRsp(ulApIdx, ulStaIndex,stWaiAuthInfo);
				break;
			}
			else
			{
				printf("ERROR. MSG Subtype is %d, not required.\nFile: %s,Line: %d\n",stWaiAuthInfo.ucSubType,__FILE__,__LINE__);
				break;
			}
		}
		break;

		default:
		{
			gstErrMsgStatics.ulRcv5247UnknwnMsg++;
			MT_StaLog(ulStaIndex,"stEthHdr.usProtclType is unknown.");
		}
		break;
		}
	}
	else //1 == bTflag
	{
		if(1 == ucWbId)//Payload Type == Native Frame Format 802.11
		{
			ulCapwapSuffix = 16;
			cpss_mem_memcpy(&st80211AuthFlagInfo,&aucCapwapBuf[ulCapwapSuffix],sizeof(st80211AuthFlagInfo));
			st80211AuthFlagInfo.usFranCtrl = htons(st80211AuthFlagInfo.usFranCtrl);
			ulStaIndex = getStaIndexByMac(st80211AuthFlagInfo.aucDstMacAdd);

			if(MT_UNDEFINED_OBJECT_IDENTIFIER == ulStaIndex)
			{
				gstErrMsgStatics.ulRcv5247MsgErrStaId++;
				MT_StaLog(ulStaIndex,"Sta not Online.");
				return;
			}

			switch(st80211AuthFlagInfo.usFranCtrl)
			{
	//		*0xB000：认证请求
	//		*0xB000：认证响应
			case 0xB000:
			{
				if(WAIT_80211_AUTH_RSP_STATE == gastStaPara[ulStaIndex].enAuthState)
				{
					gstMutiTest_Statics.ulRecv80211AuthRsp++;
					MT_StaLog(ulStaIndex,"STA Receive 80211Auth RSP Success.");
				}
				else if(ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState)
				{
					if(WAIT_HDOVER_AUTH_RSP_STATE == gastStaPara[ulStaIndex].enHandOverState)
					{
						gstMutiTest_Statics.ulRecv80211AuthRspHO++;
						MT_StaLog(ulStaIndex,"STA Receive 80211Auth RSP for Handover Success.");
					}
					else
					{
						gstErrMsgStatics.ulRecv80211AuthRspHOErr++;
						gstMutiTest_Statics.ulReAssociationFail++;
						MT_StaLog(ulStaIndex,"STA Receive 80211Auth RSP for Handover Error.");
						gastStaPara[ulStaIndex].enHandOverState = HANDOVER_FAIL;
						break;
					}
				}
				else
				{
					gstErrMsgStatics.ulRecv80211AuthRspErr++;
					gstMutiTest_Statics.ulAssociationFail++;
					gbLogFlag = MT_TRUE;
					MT_StaLog(ulStaIndex,"STA Receive 80211Auth RSP Error.");
					gbLogFlag = MT_FALSE;
					gastStaPara[ulStaIndex].enAuthState = ASSOCIATION_FAIL;
					break;
				}

				ulCapwapSuffix = ulCapwapSuffix + sizeof(st80211AuthFlagInfo);
				cpss_mem_memcpy(&st80211WlanMangmntFrameFixedPara,&aucCapwapBuf[ulCapwapSuffix],sizeof(st80211WlanMangmntFrameFixedPara));

				//WEP
				if(MT_AUTH_POLICY_WEPSHAREDKEY == st80211WlanMangmntFrameFixedPara.usAuthAlgrm && 2 == st80211WlanMangmntFrameFixedPara.usAuthSeq)
				{
					ulCapwapSuffix = ulCapwapSuffix + sizeof(st80211WlanMangmntFrameFixedPara);
					cpss_mem_memcpy(&st80211WlanMangmntFrameTaggedPara,&aucCapwapBuf[ulCapwapSuffix],sizeof(st80211WlanMangmntFrameTaggedPara.ucTagNum) + sizeof(st80211WlanMangmntFrameTaggedPara.ucTagLen));

					if(16 == st80211WlanMangmntFrameTaggedPara.ucTagNum)
					{
						stStaAuthInfo.ulApIndex = ulApIdx;
						stStaAuthInfo.ulStaIndex = ulStaIndex;
						ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
						stStaAuthInfo.ucRadioId = ucRadioId;
//						ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

						if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
						{
							gstErrMsgStatics.ulRcv5247MsgErr++;
							MT_StaLog(ulStaIndex,"Local AP has No WLAN Service.");
							break;
						}

						if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
						{
						  break;
						}

						stStaAuthInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//						ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//						stStaAuthInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
						stStaAuthInfo.usAuthAlgrm = MT_AUTH_POLICY_WEPSHAREDKEY;
						stStaAuthInfo.usAuthSeq = st80211WlanMangmntFrameFixedPara.usAuthSeq;
						stStaAuthInfo.ucChallengeTxtLen = st80211WlanMangmntFrameTaggedPara.ucTagLen;
						ulCapwapSuffix = ulCapwapSuffix + sizeof(st80211WlanMangmntFrameTaggedPara.ucTagNum) + sizeof(st80211WlanMangmntFrameTaggedPara.ucTagLen);
						cpss_mem_memcpy(stStaAuthInfo.aucChallengeTxt,&aucCapwapBuf[ulCapwapSuffix],stStaAuthInfo.ucChallengeTxtLen);

						bSendResult = ApMutiTestSend80211AuthReq(stStaAuthInfo);

						if(MT_TRUE == bSendResult)
						{
							gstMutiTest_Statics.ulSend80211AuthReq++;
							gastStaPara[ulStaIndex].enAuthState = WAIT_80211_AUTH_RSP_STATE;
							MT_StaLog(ulStaIndex,"STA Send80211AuthReq for WEP AUTH Success.");
						}
						else
						{
							gstErrMsgStatics.ulSend80211AuthReqErr++;
							MT_StaLog(ulStaIndex,"STA Send80211AuthReq for WEP AUTH Failure.");
							gastStaPara[ulStaIndex].enAuthState = ASSOCIATION_FAIL;
						}
					}
				}
				else if(MT_AUTH_POLICY_WEPSHAREDKEY == htons(st80211WlanMangmntFrameFixedPara.usAuthAlgrm) && 4 == htons(st80211WlanMangmntFrameFixedPara.usAuthSeq))
				{
					if ((RUN_STATE == gastAp[ulApIdx].enState) && (WAIT_80211_AUTH_RSP_STATE == gastStaPara[ulStaIndex].enAuthState))
					{
						stStaWLanInfo.ulApIndex = ulApIdx;
						stStaWLanInfo.ulStaIndex = ulStaIndex;
						ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
						stStaWLanInfo.ucRadioId = ucRadioId;
//						ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

						if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
						{
							gstErrMsgStatics.ulRcv5247MsgErr++;
							MT_StaLog(ulStaIndex,"Local AP has No WLAN Service.");
							break;
						}

						if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
						{
						  break;
						}

						stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//						ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//						stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

						bSendResult = ApMutiTestSendAssocReq(stStaWLanInfo);

						if(MT_TRUE == bSendResult)
						{
							gstMutiTest_Statics.ulSend80211AssocReq++;
							gastStaPara[ulStaIndex].enAuthState = WAIT_80211_ASSOC_RSP_STATE;
							MT_StaLog(ulStaIndex,"STA SendAssocReq with WEP Auth Success.");
						}
						else
						{
							gstErrMsgStatics.ulSend80211AssocReqErr++;
							MT_StaLog(ulStaIndex,"STA SendAssocReq with WEP Auth Failure.");
							gastStaPara[ulStaIndex].enAuthState = ASSOCIATION_FAIL;
							break;
						}
					}
				}
				else
				{
					//OPEN
					if ((RUN_STATE == gastAp[ulApIdx].enState) && (WAIT_80211_AUTH_RSP_STATE == gastStaPara[ulStaIndex].enAuthState))
					{
						stStaWLanInfo.ulApIndex = ulApIdx;
						stStaWLanInfo.ulStaIndex = ulStaIndex;
						ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
						stStaWLanInfo.ucRadioId = ucRadioId;
//						ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

						if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
						{
							gstErrMsgStatics.ulRcv5247MsgErr++;
							MT_StaLog(ulStaIndex,"Local AP has No WLAN Service.");
							break;
						}

						if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
						{
						  break;
						}

						stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//						ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//						stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

						bSendResult = ApMutiTestSendAssocReq(stStaWLanInfo);

						if(MT_TRUE == bSendResult)
						{
							gstMutiTest_Statics.ulSend80211AssocReq++;
							gastStaPara[ulStaIndex].enAuthState = WAIT_80211_ASSOC_RSP_STATE;
							MT_StaLog(ulStaIndex,"STA SendAssocReq with Open Auth Success.");
						}
						else
						{
							gstErrMsgStatics.ulSend80211AssocReqErr++;
							MT_StaLog(ulStaIndex,"STA SendAssocReq with Open Auth Failure.");
							gastStaPara[ulStaIndex].enAuthState = ASSOCIATION_FAIL;
						}
					}
					else if((WAIT_HDOVER_AUTH_RSP_STATE == gastStaPara[ulStaIndex].enHandOverState) && (ASSOCIATION_SUCESS_STATE == gastStaPara[ulStaIndex].enAuthState))
					{
						stStaWLanInfo.ulApIndex = ulApIdx;
						stStaWLanInfo.ulStaIndex = ulStaIndex;
						ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况;
						stStaWLanInfo.ucRadioId = ucRadioId;
//						ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

						if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
						{
							gstErrMsgStatics.ulRcv5247MsgErr++;
							MT_StaLog(ulStaIndex,"Local AP has No WLAN Service.");
							break;
						}

						if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
						{
						  break;
						}

						stStaWLanInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;

//						ucStaWlanIndex = ulStaIndex % ucWlanNum;//用户需要接入的WLAN标识
//						stStaWLanInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];

						bSendResult = ApMutiTestSendReAssocReq(stStaWLanInfo);

						if(MT_TRUE == bSendResult)
						{
							gstMutiTest_Statics.ulSend80211ReAssocReq++;
							gastStaPara[ulStaIndex].enHandOverState = WAIT_HDOVER_REASSOC_RSP_STATE;
							MT_StaLog(ulStaIndex,"STA SendReAssocReq Success.");
						}
						else
						{
							gstErrMsgStatics.ulSend80211ReAssocReqErr++;
							MT_StaLog(ulStaIndex,"STA SendReAssocReq Failure.");
							gastStaPara[ulStaIndex].enHandOverState = HANDOVER_FAIL;
						}
					}
				}
			}
			break;

	//		*0x0000：关联请求
	//		*0x1000：关联响应
			case 0x1000://关联响应
			{
				if(WAIT_80211_ASSOC_RSP_STATE == gastStaPara[ulStaIndex].enAuthState)
				{
					gstMutiTest_Statics.ulRecv80211AssocRsp++;
					gastStaPara[ulStaIndex].enAuthState = ASSOCIATION_SUCESS_STATE;
					gstMutiTest_Statics.ulAssociationSuccess++;
					gastStaPara[ulStaIndex].ulStaTrgPrdCnt = 0;
					MT_StaLog(ulStaIndex,"STA Receive AssocRsp Success.");

					gstWtpEventFlag.ucFlagL2IeId_5641 = MT_WTP_EVENT_L2IE_ENABLE;
					bSendResult = ApMutiTestSendEventReq_IE37_IE56(ulApIdx);
					if(MT_FALSE == bSendResult)
					{
						gstErrMsgStatics.ulSendWTPEventReqErr++;
						gstErrMsgStatics.ulSendEventReq_IE37IE56Err++;
						MT_ApLog(ulApIdx, "ApMutiTestSendEventReq_IE37_IE56_41 Failure");
					}
					else
					{
						gstMutiTest_Statics.ulSendWTPEventReq++;
						MT_ApLog(ulApIdx, "ApMutiTestSendEventReq_IE37_IE56_41 Success");
					}

					gstWtpEventFlag.ucFlagL2IeId_5641 = MT_WTP_EVENT_L2IE_DISABLE;

					//Just for切换过程,启动切换定时器，时长由sysconf.conf文件确定
					ulTimerMsgPara = ulStaIndex + 1;
					gastStaPara[ulStaIndex].ulStaHdOverTimerId = cpss_timer_para_set(MT_TEST_STAHD_OVER_TMRID, gulStaHoTrgrPrd * 1000, ulTimerMsgPara);
				}
				else
				{
					gstErrMsgStatics.ulRecv80211AssocRspErr++;
					gstMutiTest_Statics.ulAssociationFail++;
					gastStaPara[ulStaIndex].enAuthState = ASSOCIATION_FAIL;
				}
			}
			break;

	//		*0x2000：重关联请求
	//		*0x3000：重关联响应
			//重关联响应
			case 0x3000:
			{
				if(WAIT_HDOVER_REASSOC_RSP_STATE == gastStaPara[ulStaIndex].enHandOverState)
				{
					gstMutiTest_Statics.ulRecv80211ReAssocRsp++;
					gastStaPara[ulStaIndex].enHandOverState = HANDOVER_SUCCESS;
					gstMutiTest_Statics.ulReAssociationSuccess++;
					cpss_mem_memcpy(gastStaPara[ulStaIndex].aucCrrntAp,gastStaPara[ulStaIndex].aucBssId,MT_MAC_LENTH);
				}
				else
				{
					gstErrMsgStatics.ulRecv80211ReAssocRspErr++;
					gstMutiTest_Statics.ulReAssociationFail++;
					gastStaPara[ulStaIndex].enHandOverState = HANDOVER_FAIL;
				}
			}
			break;

			//DeAuth
			case 0xC000:
			{
				gstMutiTest_Statics.ulRecvDeAuth++;
				gbLogFlag = MT_TRUE;
				MT_StaLog(ulStaIndex,"STA Receive DeAuth Success.");
				gbLogFlag = MT_FALSE;
				if(AUTH_INIT_STATE == gastStaPara[ulStaIndex].enAuthState)
				{
					break;
				}

				if(ASSOCIATION_SUCESS_STATE != gastStaPara[ulStaIndex].enAuthState)
				{
					gstMutiTest_Statics.ulAssociationFail++;
					staStateInit(ulStaIndex);
				}
				else
				{
					if(MT_UNDEFINED_OBJECT_IDENTIFIER == gastStaPara[ulStaIndex].ulStaLocalApId)
					{
						gulOnlineStaCnt--;
						staStateInit(ulStaIndex);
					}
				}
			}
			break;

			default:
			{
				gstErrMsgStatics.ulRcv5247UnknwnMsg++;
				sprintf((char*)aucStaTmpInfo,"Sta Rcv MSG Type ERROR:0x%04x.",st80211AuthFlagInfo.usFranCtrl);
				MT_StaLog(ulStaIndex,(char*)aucStaTmpInfo);
			}
			break;
			}
		}
		else //0 == ucWbId
		{
			gstErrMsgStatics.ulRcv5247UnknwnMsg++;
		}
	}
	return;
}

/*MUTITEST PROC WORKING PROC*/
void MT_Working(CPSS_COM_MSG_HEAD_T *pstMsgHead)
{
	UINT32 ulApIdx = 0;
	UINT32 ulDestApIndex = 0;
	UINT32 ulStaIndex = 0;
	UINT32 ulMsgType = MT_UNDEFINED_OBJECT_IDENTIFIER;
	MTBool bSendResult = MT_FALSE;
	CPSS_TIMER_MSG_T *pstTimerMsg = NULL;
//	UINT8 ucStaWlanIndex = 0;//用户需要接入的WLAN标识
	UINT8 ucRadioId = 0;
//	UINT8 ucWlanNum = 0;
	UINT32 ulTimerMsgPara = 0;
	MT_STA_AUTH_INFO stStaAuthInfo;
	UINT32 ulLoop = 0;
	UINT32 ulTrgNum = 0;

	cpss_mem_memset(&stStaAuthInfo,0,sizeof(stStaAuthInfo));

	if (NULL == pstMsgHead)
	{
		printf("\nERROR. MT_Working():Input Para is NULL.\nFile:%s.Line:%d\n", __FILE__, __LINE__);
		return;
	}

	/*MSG PROC*/
	ulMsgType = pstMsgHead->ulMsgId;

	switch (ulMsgType)
	{
	//Start Test Proc
	case MT_START_CMD_MSG:
	{
		printf("MT_Working received MT_START_CMD_MSG message!\n\n");
		/*START PEROID TIMER,TO START MUTIAP TEST,UNIT:mS*/
		cpss_timer_loop_set(MT_TEST_PERIOD_TMRID, gulPeriodTriggerTime);

		/*START PEROID TIMER,TO PRINT STATICS INFO PERIOD,UNIT:mS*/
		cpss_timer_loop_set(MT_TEST_PERIOD_PRINT_TMRID, gulPeriodShowTime);
	}
	break;

	//Stop Test Proc
	case MT_STOP_CMD_MSG:
	{
		printf("\nMT_Working received MT_STOP_CMD_MSG message!");
		showglobalpara();
		printf("\nThis MutiTest Tools will OutPut Test Counters after (%d)s!\n",gulTestStopTime);

		cpss_timer_delete(MT_TEST_PERIOD_TMRID);
		cpss_timer_delete(MT_TEST_PERIOD_PRINT_TMRID);
		cpss_timer_delete(MT_TEST_APHOLD_TMRID);
		cpss_timer_delete(MT_TEST_STAHOLD_TMRID);
		cpss_timer_delete(MT_TEST_STAHD_OVER_TMRID);
		cpss_timer_delete(MT_TEST_PERIOD_ECHO_TMRID);

		/*START THE STOP TIMER*/
		cpss_timer_para_set(MT_TEST_STOP_TMRID, gulTestStopTime * 1000, 0);
	}
	break;

	//周期性触发测试,WHEN THE PERIOD TIME COME,CALL THE FUN TO START MUTI AP TEST；
	case MT_TEST_PERIOD_TIMOV_MSG:
	{
#if 0
#define MT_PACKET_SEND_TEST_NUM 250
		{
			for(ulTrgNum = 0; ulTrgNum < MT_PACKET_SEND_TEST_NUM; ulTrgNum++)
			{
				test_5246_5247_msg_cnt((ulLoop + ulTrgNum) % MT_AP_MAX_NUM);
			}
			ulLoop = ulLoop + MT_PACKET_SEND_TEST_NUM;
			break;
		}
#endif

		gulOnlineApNum = apOnlineCnt();//for a moment.

		if(gulOnlineApNum < gulCmdStartApNum)
		{
			MT_ProcStart(gulCmdStartApNum,gulOnlineApNum);
		}
		else
		{
			MT_WtpStatusChk();
		}

		//STA周期性检测Online状态，将超期STA主动下线。
		if(0 != gulCmdStartApNum * gulStaNumPerAp)
		{
			if(0 < (gstMutiTest_Statics.ulAssociationSuccess % (gulCmdStartApNum * gulStaNumPerAp)))
			{
				MT_StaOnlineChk();
			}
		}

		//AP上有用户
		if (MT_FALSE == gbAPOnlyFlag)
		{
			switch(gucStaAccessPlcy)
			{
			//用户接入策略：AP优先，即优先接入小序号AP，当小序号AP接满用户后，才接入大序号AP；
			case MT_STA_ACCESS_POLICY_AP_FIRST:
			{
				MT_StaAccessAPfirst();
			}
			break;

			case MT_STA_ACCESS_POLICY_STA_FIRST:
			{
				MT_StaAccessSTAfirst();
			}
			break;

			default:
			{
				printf("ERROR.STA Access Policy is %d.\nFile:%s,Line:%d\n",gucStaAccessPlcy,__FILE__,__LINE__);
			}
			break;
			}
		}
	}
	break;

	//周期性统计信息打印
	case MT_TEST_PERIOD_PRINT_TIMOV_MSG:
	{
		if(MT_TRUE == gbShowFlag)
		{
			MT_ShowStatistic();
		}

		//WTP周期性检测ECHO状态，将超期未回ECHO响应的WTP主动下线。
		MT_WtpEchoChk();
	}
	break;

	//ECHO定时处理,
	case MT_TEST_PERIOD_ECHO_TIMOV_MSG:
	{
		if(MT_TRUE == gbStopFlag)
		{
			break;
		}

		pstTimerMsg = (CPSS_TIMER_MSG_T *) pstMsgHead->pucBuf;
		ulApIdx = pstTimerMsg->ulPara;

		if(RUN_STATE != gastAp[ulApIdx].enState)
		{
			gstErrMsgStatics.ulApRecvEchoMsgErr++;
		}
		else
		{
			//Echo Req
			bSendResult = ApMutiTestSendEchoReq(ulApIdx);

			if(MT_TRUE == bSendResult)
    	    {
				gstMutiTest_Statics.ulApSendEchoReq++;
    		gastAp[ulApIdx].ulEchoReqNum++;
    		gastAp[ulApIdx].stEchoTm.ulBeginTime = cpss_tick_get();
    		gastAp[ulApIdx].stEchoTm.ulEndTime = 0;

    		//keep Alive
    		bSendResult = ApMutiTestSendKeepAlive(ulApIdx);
    		if(MT_FALSE == bSendResult)
    		    {
    			gstErrMsgStatics.ulApSendKeepAliveErr++;
    		    }
    		else
    		    {
    			gstMutiTest_Statics.ulApSendKeepAlive++;
    		    }

    		//WTP EVENT MSG
    		MT_SndWtpEventMsg(ulApIdx);
    	    }
    	else
    	    {
    		gstErrMsgStatics.ulApSendEchoReqErr++;
    	    }
		}
	}
	break;

	//停止测试活动
	case MT_TEST_STOP_TIMOV_MSG:
	{
		//AP释放
		for(ulApIdx = 0; ulApIdx < MT_AP_MAX_NUM; ulApIdx++)
		{
			WtpRelease(ulApIdx);
		}

		MT_ShowStatistic();
		MT_ShowErrMsg();
	}
	break;

	//超时后AP发送Reset Response
	case MT_TEST_APHOLD_TIMOV_MSG:
	{
		if(MT_TRUE == gbStopFlag)
		{
			break;
		}

		//AP自动上下线
   if(MT_FALSE == gbApAlwaysOnline)
	    {
	   pstTimerMsg = (CPSS_TIMER_MSG_T *) pstMsgHead->pucBuf;
	   ulApIdx = pstTimerMsg->ulPara;
	   MT_ApTmOvReset(ulApIdx);
	    }
	}
	break;

	case MT_TEST_STAHOLD_TIMOV_MSG:
	{
		if(MT_TRUE == gbStopFlag)
		{
			break;
		}

		pstTimerMsg = (CPSS_TIMER_MSG_T *) pstMsgHead->pucBuf;
		ulStaIndex = pstTimerMsg->ulPara;

		if(AUTH_INIT_STATE == gastStaPara[ulStaIndex].enAuthState)
		{
			break;
		}

		MT_StaTmOv(ulStaIndex);
	}
	break;

	//HandOver MSG
	case MT_TEST_STAHD_OVER_TIMOV_MSG:
	{
		if(MT_TRUE == gbStopFlag)
		{
			break;
		}

		if(MT_FALSE == gbHdOverFlag)
		{
			break;
		}

		pstTimerMsg = (CPSS_TIMER_MSG_T *) pstMsgHead->pucBuf;
		ulTimerMsgPara = pstTimerMsg->ulPara;
		ulStaIndex = ulTimerMsgPara - 1;
		ulApIdx = getApIDbyBssid(ulStaIndex);

		if(MT_UNDEFINED_OBJECT_IDENTIFIER == ulApIdx)
		{
			break;
		}

		stStaAuthInfo.ulApIndex = ulApIdx;
		stStaAuthInfo.ulStaIndex = ulStaIndex;
		ucRadioId = gastApWlanInfo[ulApIdx].aucRadioList[0];//默认RadioId为1，暂不考虑多Radio的情况
		stStaAuthInfo.ucRadioId = ucRadioId;
//		ucWlanNum = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum;

		if(0 == gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].ucWlanNum)
		{
			break;
		}

		if(MT_UNDEFINED_WLANID_IDENTIFIER == gastStaPara[ulStaIndex].ucWlanId)
		{
		  break;
		}

		stStaAuthInfo.ucWlanId = gastStaPara[ulStaIndex].ucWlanId;
		stStaAuthInfo.usAuthAlgrm = (UINT16)gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[stStaAuthInfo.ucWlanId].ucAuthType;
//		ucStaWlanIndex = ulStaIndex % ucWlanNum;
//		stStaAuthInfo.ucWlanId = gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].aucWlanList[ucStaWlanIndex];
//		stStaAuthInfo.usAuthAlgrm = (UINT16)gastApWlanInfo[ulApIdx].astRadioInfo[ucRadioId].astWlanInfo[ucStaWlanIndex].ucAuthType;

		stStaAuthInfo.usAuthSeq = 1;
		ulDestApIndex = findDestAp(ulApIdx);

		//    	 if(MT_UNDEFINED_OBJECT_IDENTIFIER == ulDestApIndex || MT_AP_MAX_NUM == ulDestApIndex || ulApIdx == ulDestApIndex)
		if(MT_UNDEFINED_OBJECT_IDENTIFIER == ulDestApIndex || gulCmdStartApNum == ulDestApIndex || ulApIdx == ulDestApIndex)
		{
			break;
		}

		bSendResult = ApMutiTestSend80211AuthReqForHandover(stStaAuthInfo,ulDestApIndex);

		if(MT_TRUE == bSendResult)
		{
			gstMutiTest_Statics.ulSend80211AuthReqHO++;
			gastStaPara[ulStaIndex].enHandOverState = WAIT_HDOVER_AUTH_RSP_STATE;
			MT_GetBssID(ulDestApIndex,stStaAuthInfo.ucRadioId,stStaAuthInfo.ucWlanId,gastStaPara[ulStaIndex].aucBssId);
			MT_GetBssID(stStaAuthInfo.ulApIndex,stStaAuthInfo.ucRadioId,stStaAuthInfo.ucWlanId,gastStaPara[ulStaIndex].aucCrrntAp);
			gastStaPara[ulStaIndex].ulHomeApId = ulApIdx;
			gastStaPara[ulStaIndex].ulForeignApId = ulDestApIndex;
			MT_StaLog(ulStaIndex,"STA Send80211AuthReqForHandover Success.");
		}
		else
		{
			gstErrMsgStatics.ulSend80211AuthReqHOErr++;
			MT_StaLog(ulStaIndex,"STA Send80211AuthReqForHandover Failure.");
		}
	}
	break;

	/*Msg From Dispatcher(5246)*/
	case MT_SOCKET_DATA_CPM_IND_MSG:
	{
		gstRcvSndMsfCnt.ul5246MsgFrmDispchrCnt++;
		MT_MsgProcFrm5246(pstMsgHead);
	}
	break;

	/*Msg From Dispatcher(5247)*/
	case MT_SOCKET_DATA_UPM_IND_MSG:
	{
		gstRcvSndMsfCnt.ul5247MsgFrmDispchrCnt++;
		MT_MsgProcFrm5247(pstMsgHead);
	}
	break;

	//AP DHCPv4
	case MT_SOCKET_DATA_DHCP_IND_MSG:
	{
		if(MT_TRUE == gbStopFlag)
		{
			break;
		}

     }
	break;

	default:
	{
		gstErrMsgStatics.ulRcvOtherMsgErr++;
		printf("\nMT_Working Msg Process enter default case, ulMsgType = 0x%x!\nFile: %s.Line: %d\n",ulMsgType,__FILE__,__LINE__);
	}
	break;
	}
}

void MT_Main_Proc(UINT16 usState, void* pVarr,CPSS_COM_MSG_HEAD_T *pstMsgHead)
{
  if (NULL == pstMsgHead)
    {
	  printf("ERROR. Input Para is NULL.\n.\nFile:%s.Line:%d\n",__FILE__,__LINE__);
	  return;
    }

  switch (usState)
    {
  case MT_POWERON:
    MT_PowerOn(pstMsgHead);
    break;

  case MT_WORKING:
    MT_Working(pstMsgHead);
    break;

  default:
    printf("\nMT_Main_Proc:State->(%d)[UNKNOW_STATE_ID]\n.File:%s.Line:%d\n",usState,__FILE__,__LINE__);
    break;
  }
  return;
}

/*start mutitest cmd*/
void MT_StartTest(void)
{
    char cRcvBuf[4] = {0};
    UINT32 ulMsgId = 0;
    CPSS_COM_PID_T stDstPid;

    /*init statistic counters*/
    cpss_mem_memset(&gstMutiTest_Statics,0,sizeof(MUTITEST_STATICS_T));
    cpss_mem_memset(&stDstPid,0,sizeof(CPSS_COM_PID_T));

    cpss_com_logic_addr_get(&stDstPid.stLogicAddr, &stDstPid.ulAddrFlag);
    stDstPid.ulPd = MT_MAIN_PROC;

    ulMsgId = MT_START_CMD_MSG;
    cpss_com_send(&stDstPid, ulMsgId, (UINT8*)&cRcvBuf, sizeof(cRcvBuf));
}

void MT_StartParaCheck(UINT32 vulApNum,UINT32 vulStaNumPerAp, UINT32 vulSecond)
{
  UINT32 ulStaNumPerAp = 0;
  UINT32 ulStaHoldTime = 0;
  UINT32 ulApHoldTime = 0;

  //First Para APNum
  if(vulApNum <= 0 || vulApNum > gulApMaxNum)
    {
    printf("ERROR.Input AP_Num %d error. It should be 1 ~ %d.\nFile:%s.Line:%d\n",vulApNum,gulApMaxNum,__FILE__,__LINE__);
    return;
    }

  gulCmdStartApNum = MT_MIN(vulApNum,gulSysConfAPCnt);
  printf("The %d APs will be online.\n",gulCmdStartApNum);

  //Second Para StaNumPerAp
  ulStaNumPerAp = (gulStaMaxNum / gulCmdStartApNum);//理论上每AP的最大接入用户数，但其最大值应为128
  if (ulStaNumPerAp > MT_STA_MAX_NUM_PER_WTP)
    {
    ulStaNumPerAp = MT_STA_MAX_NUM_PER_WTP;
    }

  if (vulStaNumPerAp < 0)
    {
    printf("ERROR!The second para StaNumPerAp %d should be 0~%d\n.File:%s,Line:%d\n",vulStaNumPerAp,ulStaNumPerAp, __FILE__, __LINE__);
    return;
     }
  else if (0 == vulStaNumPerAp)
    {
	  gbAPOnlyFlag = MT_TRUE;
	  gulStaNumPerAp = 0;
	  printf("No STA will be running during the test_procedure.\n");
    }
  else
    {
	  gulStaNumPerAp = MT_MIN(vulStaNumPerAp, MT_STA_MAX_NUM_PER_WTP);
    }

  printf("STA per AP is %d in practice, and less than %d in theory.\n",gulStaNumPerAp,ulStaNumPerAp);
    //The Third Para StaHodTime

  if(vulSecond < 0)
    {
    printf("ERROR.The Third Para is %d.\nFile:%s.Line:%d\n",vulSecond,__FILE__,__LINE__);
    return;
    }

  if(MT_FALSE == gbAPOnlyFlag)//AP带用户，按用户时间计算
    {
    if(0 != gulStaNumPerSecond)
          {
      ulStaHoldTime = (gulCmdStartApNum * gulStaNumPerAp) / gulStaNumPerSecond;//(s)
          }
    else
          {
      printf("ERROR.StaNumPerSecond is 0.\n");
      return;
           }
    }

  gulStaHoldTime = vulSecond > ulStaHoldTime ? vulSecond:ulStaHoldTime;

  ulApHoldTime = (gulCmdStartApNum / gulApNumPerSecond) + 1;//计算AP的理论上的最小保持时间（所有AP都能上线）

  gulApHoldTime = MT_MAX(gulApHoldTime,ulApHoldTime);

  printf("AP Hold time:---> %d(s)\n",gulApHoldTime);
  printf("STA Hold time:--> %d(s)\n",gulStaHoldTime);
}

//change interface MSG Queue length,BGN
void MT_ChangIfMsgQueueLen(void)
{
	struct ifreq stIfReq;
	INT32 fd = 0;
	INT32 ret = 0;

	cpss_mem_memset(&stIfReq,0,sizeof(stIfReq));

	fd = socket(AF_INET, SOCK_DGRAM, 0);//SOCK_RAW,SOCK_DGRAM

	strcpy(stIfReq.ifr_name, gstMtSysConf.aucDevName);

	//获取接口发送队长度
	ret = ioctl(fd, SIOCGIFTXQLEN, &stIfReq);

	if(ret < 0)
	{
		printf("IOCTL ERROR!\n");
	}
	else
	{
		printf("ifr_qlen: %d\n",stIfReq.ifr_qlen);
	}

	//更改接口发送队列长度，系统默认为1000
	stIfReq.ifr_qlen = 800;

	ret = ioctl(fd, SIOCSIFTXQLEN, &stIfReq);

	memset(&stIfReq,0,sizeof(stIfReq));
	strcpy(stIfReq.ifr_name,gstMtSysConf.aucDevName);

	ret = ioctl(fd, SIOCGIFTXQLEN, &stIfReq);
	if(ret < 0)
	{
		printf("IOCTL ERROR!\n");
	}
	else
	{
		printf("changed ifr_qlen: %d\n",stIfReq.ifr_qlen);
	}

	close(fd);
}
//通过shell命令触发MT测试活动的开始
void start(UINT32 vulApNum,UINT32 vulStaNumPerAp, UINT32 vulSecond)
{
	cpss_mem_memset(&gstRcvSndMsfCnt,0,sizeof(MT_RCV_SND_MSG_T));
	cpss_mem_memset(&gstRunTime,0,sizeof(gstRunTime));
	gulStaID = 0;
	cpss_mem_memset(&gstMutiTest_Statics,0,sizeof(MUTITEST_STATICS_T));
	cpss_mem_memset(&gstErrMsgStatics,0,sizeof(MT_ERR_MSG_STATICS_T));
	MT_ZeroPara();


	MT_StartParaCheck(vulApNum,vulStaNumPerAp,vulSecond);
	gbStopFlag = MT_FALSE;
	gbHdOverFlag = MT_FALSE;
	gbStaDhcpFlag = MT_FALSE;
	gbLogFlag = MT_FALSE;

	//AP参数初始化
	MT_ApParaInit();
	//STA参数初始化
	MT_StaParaInit();

	system("rm -rf /mt_conf/*log.lo*");

	//change interface Msg Queue Length
	MT_ChangIfMsgQueueLen();

	MT_GetArpByPingAC();

	gstRunTime.ulBeginTime = cpss_tick_get();

	MT_StartTest();
}

/*stop mutitest cmd*/
//通过shell命令结束测试活动
void stop(void)
{
	UINT8 aucRcvBuf[4] = {0};
	UINT32 ulMsgId = 0;
	CPSS_COM_PID_T stDstPid;
	gucWtpEventFlag = MT_WTP_EVENT_DISABLE;
	cpss_mem_memset(&gstWtpEventFlag,0,sizeof(MT_WTP_EVENT_FLAG_T));
	cpss_com_logic_addr_get(&stDstPid.stLogicAddr, &stDstPid.ulAddrFlag);
	stDstPid.ulPd = MT_MAIN_PROC;
	ulMsgId = MT_STOP_CMD_MSG;
	cpss_com_send(&stDstPid, ulMsgId, (UINT8*)&aucRcvBuf, sizeof(aucRcvBuf));
	gbStopFlag = MT_TRUE;
	gbHdOverFlag = MT_FALSE;
	gbStaDhcpFlag = MT_FALSE;
}

UINT32 MT_GetApIndexFromApMac(char* vpApMacStr)
{
	UINT32 ulApIndex = MT_UNDEFINED_OBJECT_IDENTIFIER;
	return ulApIndex;
}

UINT32 MT_GetApIndexFromApIp(char* vpApIpStr)
{
	UINT32 ulApIndex = MT_UNDEFINED_OBJECT_IDENTIFIER;
	return ulApIndex;
}

UINT32 MT_GetStaIndexFromStaMac(char* vpStaMacStr)
{
	UINT32 ulStaIndex = MT_UNDEFINED_OBJECT_IDENTIFIER;

	if(NULL == vpStaMacStr)
	{
		return ulStaIndex;
	}

	ulStaIndex = (*(vpStaMacStr + 3)) * 65535 + (*(vpStaMacStr + 4)) * 256 + *(vpStaMacStr + 5);

	if(ulStaIndex >= MT_SYS_STA_CAPABILITY)
	{
		ulStaIndex = MT_UNDEFINED_OBJECT_IDENTIFIER;
	}

	return ulStaIndex;
}

void mainprocmsgcnt(void)
{
	printf("MT_Tester_Rcvfrm_AC_MSGCNT:\n");
	printf("DispacherRcvfrmMsgCnt:-------> %u\n",gstRcvSndMsfCnt.ulRcvfrmMsgCnt);
	printf("DispacherRcvfrmMsgErrCnt:----> %u\n\n",gstRcvSndMsfCnt.ulRcvfrmMsgErrCnt);

	printf("DispacherComSend2APMsg:\n");
	printf("ComSnd2APMsgCnt:-------------> %u\n",gstRcvSndMsfCnt.ulComSnd2ApMsgCnt);
	printf("ComSnd2APMsgErrCnt:----------> %u\n",gstRcvSndMsfCnt.ulComSnd2ApMsgErrCnt);
	printf("DispacherComSend2STAMsg:\n");
	printf("ComSnd2STAMsgCnt:------------> %u\n",gstRcvSndMsfCnt.ulComSnd2StaMsgCnt);
	printf("ComSnd2STAMsgErrCnt:---------> %u\n",gstRcvSndMsfCnt.ulComSnd2StaMsgErrCnt);

	printf("\nMT_Tester Rcv MSG From Dispatcher:\n");
	printf("5246MsgFrmDispchrCnt:--------> %u\n",gstRcvSndMsfCnt.ul5246MsgFrmDispchrCnt);
	printf("5247MsgFrmDispchrCnt:--------> %u\n",gstRcvSndMsfCnt.ul5247MsgFrmDispchrCnt);
	printf("OtherMsgFrmDispchrCnt:-------> %u\n",gstRcvSndMsfCnt.ulOtherMsgFrmDispchrCnt);

	printf("\nMT_Tester_Sendto_AC_MSG:\n");
	printf("APSndtoAC_IPv4MsgCnt:--------> %u\n",gstRcvSndMsfCnt.ulAPSndtoAC_IPv4MsgCnt);
	printf("APSndtoAC_IPv4MsgErrCnt:-----> %u\n",gstRcvSndMsfCnt.ulAPSndtoAC_IPv4MsgErrCnt);
	printf("APSndtoAC_IPv6MsgCnt:--------> %u\n",gstRcvSndMsfCnt.ulAPSndtoAC_IPv6MsgCnt);
	printf("APSndtoAC_IPv6MsgErrCnt:-----> %u\n",gstRcvSndMsfCnt.ulAPSndtoAC_IPv6MsgErrCnt);

	printf("StaSndtoAC_IPv4MsgCnt:-------> %u\n",gstRcvSndMsfCnt.ulStaSndtoAC_IPv4MsgCnt);
	printf("StaSndtoAC_IPv4MsgErrCnt:----> %u\n",gstRcvSndMsfCnt.ulStaSndtoAC_IPv4MsgErrCnt);
	printf("StaSndtoAC_IPv6MsgCnt:-------> %u\n",gstRcvSndMsfCnt.ulStaSndtoAC_IPv6MsgCnt);
	printf("StaSndtoAC_IPv6MsgErrCnt:----> %u\n",gstRcvSndMsfCnt.ulStaSndtoAC_IPv6MsgErrCnt);
}

void test_5246_5247_msg_cnt(UINT32 vulApIdx)
{
	MTBool ret = MT_FALSE;
	ret = ApMutiTestSendEchoReq(vulApIdx);

	if(MT_FALSE == ret)
	{
		gstErrMsgStatics.ulApSendEchoReqErr++;
	}
	else
	{
		gstMutiTest_Statics.ulApSendEchoReq++;
	}

	ret = ApMutiTestSendKeepAlive(vulApIdx);
	if(MT_FALSE == ret)
	{
		gstErrMsgStatics.ulApSendKeepAliveErr++;
	}
	else
	{
		gstMutiTest_Statics.ulApSendKeepAlive++;
	}
}


void startlog(void)
{
	gbLogFlag = MT_TRUE;
}

void stoplog(void)
{
	gbLogFlag = MT_FALSE;
}
