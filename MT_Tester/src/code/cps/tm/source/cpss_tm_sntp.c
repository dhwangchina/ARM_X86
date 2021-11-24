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
#include "cpss_common.h"
#include "cpss_err.h"
#include "cpss_tm_sntp.h"
#include <sys/socket.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/un.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
//#include "om_public.h"
#include <ctype.h>
#include <time.h>

#include "cpss_tm_sntp.h"
#include "cpss_tm_timer.h"

INT32 g_SntpSock;
INT32 g_SntpTimer;

#define TIME_PORT               37
#define NTPV1               "NTP/V1"
#define NTPV2               "NTP/V2"
#define NTPV3               "NTP/V3"
#define NTPV4               "NTP/V4"
#define TIME                "TIME/UDP"

#define NTP_PCK_LEN 48
#define LI 0
#define VN 3
#define MODE 3
#define STRATUM 0
#define POLL 4
#define PREC -6

#define JAN_1970    0x83aa7e80  /* 1900-1970 */
#define NTPFRAC(x)  (4299 * (x) + ((1981 * (x)) >> 11)) //4294
#define USEC(x)     (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))

#define SRC_IP "0.0.0.0"		/*	从 表中读取  */

#define SRC_PORT 6100  //modify by hd 123 2010-6-12
#define DEC_PORT 123

typedef struct _ntp_time
{
	unsigned int coarse;
	unsigned int fine;
} ntp_time;

struct ntp_packet
{
	unsigned char leap_ver_mode;
	unsigned char startum;
	char poll;
	char precision;
	int root_delay;
	int root_dispersion;
	int reference_identifier;
	ntp_time reference_timestamp;
	ntp_time originage_timestamp;
	ntp_time receive_timestamp;
	ntp_time transmit_timestamp;
};

char protocol[32];
char ntpsvrip[4];
int cpss_sntp_get_seconds = 0;

//#define SNTP_TEST

#ifdef SNTP_TEST
#define OM_AC_NTP_SERVER_CHG_IND_MSG 0x222222
//typedef struct
//{
// UINT8  aucServerIP[OM_IPV4_LENTH];/* Server IP*/
// UINT16 usServerPort;
// UINT8  aucRev[2];
// UINT32 ulNtpCycle;/*NTP同步周期*/
//}OM_SERVER_ADDR_T;

typedef struct _OM_AC_NTP_SERVER_CHG_IND_MSG_
{
 UINT8   ucOpType;       /*3-Modify*/
 UINT8  aucRev[3];
 OM_SERVER_ADDR_T stSvrAddr;
}OM_AC_NTP_SERVER_CHG_IND_MSG_T;
#endif

int construct_packet(char *packet);
void cps_sntp_set_app_clock();
extern INT32 cpss_timer_set_sys_clock(CPSS_TM_SYSSOFTCLOCK_T *pstNewClock);
void sntp_send_smss_activate_rsp(UINT32 ulResult);
int get_ntp_time(int sk, struct ntp_packet *ret_time);
int set_local_time(struct ntp_packet * pnew_time_packet);
int sntp_set_time();
int get_ntp_server_ip();
INT32 sntp_socket_init();
void sntp_send_smss_activate_rsp(UINT32 ulResult);

VOID cpss_tm_sntp_proc(UINT16 usState, VOID *pVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
VOID cpss_tm_sntp_init_func(CPSS_COM_MSG_HEAD_T* pstMsg);
VOID cpss_tm_sntp_normal_func(CPSS_COM_MSG_HEAD_T* pstMsg);

VOID cpss_tm_sntp_proc(UINT16 usState, VOID *pVar,
		CPSS_COM_MSG_HEAD_T *pstMsgHead)
{
	INT32 lRet;

	if(NULL == pstMsgHead)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_ERROR,
						"cpss_tm_sntp_proc receive NULL msg !\n");
		return;
	}

	switch (usState)
	{
	case SNTP_INIT_STATE:
		cpss_tm_sntp_init_func(pstMsgHead);
		break;
#if 0//dhwang added
	case SNTP_NORMAL_STATE:
		cpss_tm_sntp_normal_func(pstMsgHead);
		break;
#endif
	default:
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_WARN,
						"cpss_tm_sntp_proc receive unknown state, state = %x\n", pstMsgHead->ulMsgId);
		break;
	}
}

VOID cpss_tm_sntp_init_func(CPSS_COM_MSG_HEAD_T* pstMsg)
{
	INT32 lRet;

	if(NULL == pstMsg)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_ERROR,
						"cpss_tm_sntp_init_func receive NULL msg !\n");
		return;
	}

	switch (pstMsg->ulMsgId)
	{
	case SMSS_PROC_ACTIVATE_REQ_MSG:
		lRet = sntp_socket_init();
		//get_ntp_server_ip(); dhwang del
		sntp_send_smss_activate_rsp(lRet);
		break;
	case SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG:
		cpss_vk_proc_user_state_set(SNTP_NORMAL_STATE);
		cpss_timer_set(CPSS_SNTP_LOOP_TIMER, g_SntpTimer * 1000);
		break;
	default:
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_WARN,
						"cpss_tm_sntp_init_func receive unknown msg in INIT state, Msgid = %x\n", pstMsg->ulMsgId);
		break;
	}
}
#if 0 //dhwang added
VOID cpss_tm_sntp_normal_func(CPSS_COM_MSG_HEAD_T* pstMsg)
{
	OM_AC_NTP_SERVER_CHG_IND_MSG_T* stNtpMsg;

	if(NULL == pstMsg)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_ERROR,
						"cpss_tm_sntp_normal_func receive NULL msg !\n");
		return;
	}

	switch (pstMsg->ulMsgId)
	{
	case CPSS_SNTP_LOOP_TIMER_MSG:
		get_ntp_server_ip();

		cpss_timer_set(CPSS_SNTP_LOOP_TIMER, g_SntpTimer * 1000);

		sntp_set_time();
		break;
	case OM_AC_NTP_SERVER_CHG_IND_MSG:
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_INFO, "cpss_tm_sntp_normal_func rcv OM_AC_NTP_SERVER_CHG_IND_MSG msg.");

		if(NULL == pstMsg->pucBuf)
		{
			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_ERROR, "cpss_tm_sntp_normal_func pucBuf is NULL!\n");
		}

		stNtpMsg = (OM_AC_NTP_SERVER_CHG_IND_MSG_T*)(pstMsg->pucBuf);

//		cpss_timer_delete(CPSS_SNTP_LOOP_TIMER);
		if(0 >= stNtpMsg->stSvrAddr.ulNtpCycle)
		{
			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_ERROR, "NtpCycle Error Cycle = %d!Use Default!\n", stNtpMsg->stSvrAddr.ulNtpCycle);
			g_SntpTimer = 60;
		}
		else
		{
			g_SntpTimer = cpss_ntohl(stNtpMsg->stSvrAddr.ulNtpCycle);
		}

		ntpsvrip[0] = stNtpMsg->stSvrAddr.aucServerIP[0];
		ntpsvrip[1] = stNtpMsg->stSvrAddr.aucServerIP[1];
		ntpsvrip[2] = stNtpMsg->stSvrAddr.aucServerIP[2];
		ntpsvrip[3] = stNtpMsg->stSvrAddr.aucServerIP[3];

		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_INFO, "NtpServer = %d-%d-%d-%d Cycle = %d!\n", ntpsvrip[0], ntpsvrip[1], ntpsvrip[2], ntpsvrip[3], g_SntpTimer);

		sntp_set_time();

		cpss_timer_set(CPSS_SNTP_LOOP_TIMER, g_SntpTimer * 1000);
		break;
	default:
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_WARN,
						"cpss_tm_sntp_normal_func receive unknown msg in NORMAL state, Msgid = %x\n", pstMsg->ulMsgId);
		break;
	}
}
#endif
int construct_packet(char *packet)
{
	char version = 1;
	long tmp_wrd;
	int port;
	time_t timer;

	strcpy(protocol, NTPV3);

	if (!strcmp(protocol, NTPV1) || !strcmp(protocol, NTPV2) || !strcmp(
			protocol, NTPV3) || !strcmp(protocol, NTPV4))
	{
		memset(packet, 0, NTP_PCK_LEN);
		port = SRC_PORT;
		version = 3;
		tmp_wrd = htonl((LI << 30) | (version << 27) | (MODE << 24) | (STRATUM
				<< 16) | (POLL << 8) | (PREC & 0xff));
		memcpy(packet, &tmp_wrd, sizeof(tmp_wrd));

		/* Root Delay Root Dispersion Reference Indentifier */
		tmp_wrd = htonl(1 << 16);
		memcpy(&packet[4], &tmp_wrd, sizeof(tmp_wrd));
		memcpy(&packet[8], &tmp_wrd, sizeof(tmp_wrd));

		/* Timestamp */
		time(&timer);

		tmp_wrd = htonl(JAN_1970 + (long) timer);
		memcpy(&packet[40], &tmp_wrd, sizeof(tmp_wrd)); /*Transmit Timestamp coarse*/
		tmp_wrd = htonl((long) NTPFRAC(timer));
		memcpy(&packet[44], &tmp_wrd, sizeof(tmp_wrd)); /*Transmit Timestamp fine*/
		return NTP_PCK_LEN;
	}
	else if (!strcmp(protocol, TIME))/* "TIME/UDP" */
	{
		port = TIME_PORT;
		memset(packet, 0, 4);
		return 4;
	}

	return 0;
}

/* 获取NTP时间 */
int get_ntp_time(int sk, struct ntp_packet *ret_time)
{
	fd_set pending_data;
	struct timeval block_time;
	char data[NTP_PCK_LEN * 8];
	int packet_len, count = 0;
	int rc = 0;
	int len = 0;
	struct sockaddr_in sockadd =
	{ 0 };

	//int D,T,time_now; /* Delay and Additional */

	if (!(packet_len = construct_packet(data)))
	{
		return 0;
	}

	//  sockadd.sin_addr.s_addr = inet_addr(DEC_IP);
	sockadd.sin_addr.s_addr = *(UINT32 *) ntpsvrip;
	sockadd.sin_port = htons(DEC_PORT);
	sockadd.sin_family = AF_INET;
	rc = sendto(sk, data, packet_len, 0, (struct sockaddr*) &sockadd,
			sizeof(struct sockaddr));
	if (rc > 0)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_INFO,
				"send to NTP server Ok! sendlen = %d\n", rc);
	}
	else
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_ERROR,
				"send to NTP server sync request Error!\n");
	}

	FD_ZERO(&pending_data);
	FD_SET(sk, &pending_data);
	block_time.tv_sec = 2;
	block_time.tv_usec = 0;
	len = sizeof(struct sockaddr);
	if (select(sk + 1, &pending_data, NULL, NULL, &block_time) > 0)
	{

		rc = recvfrom(sk, data, NTP_PCK_LEN * 8, 0,
				(struct sockaddr*) &sockadd, &len);

		if (rc <= 0)
		{
			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_ERROR,
					"recive Nothing from NTP Server !\n");
			return 0;
		}
		else
		{
			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_INFO,
					"Recived message from sntp server \n");
		}

		if (data != 0)
		{
			memcpy(&ret_time->transmit_timestamp, data, 4);
			// return 1;
		}
		else if (count < NTP_PCK_LEN)
		{
			return 0;
		}

		ret_time->leap_ver_mode = ntohl(data[0]);
		ret_time->startum = ntohl(data[1]);
		ret_time->poll = ntohl(data[2]);
		ret_time->precision = ntohl(data[3]);
		ret_time->root_delay = ntohl(*(int*) &(data[4]));
		ret_time->root_dispersion = ntohl(*(int*) &(data[8]));
		ret_time->reference_identifier = ntohl(*(int*) &(data[12]));
		ret_time->reference_timestamp.coarse = ntohl(*(int*) &(data[16]));
		ret_time->reference_timestamp.fine = ntohl(*(int*) &(data[20]));
		ret_time->originage_timestamp.coarse = ntohl(*(int*) &(data[24]));
		ret_time->originage_timestamp.fine = ntohl(*(int*) &(data[28]));
		ret_time->receive_timestamp.coarse = ntohl(*(int*) &(data[32]));
		ret_time->receive_timestamp.fine = ntohl(*(int*) &(data[36]));
		ret_time->transmit_timestamp.coarse = ntohl(*(int*) &(data[40]));
		ret_time->transmit_timestamp.fine = ntohl(*(int*) &(data[44]));

		return 1;
	}
	return 0;
}

int set_local_time(struct ntp_packet * pnew_time_packet)
{
	//	int gmt_sec;
	struct timeval tv;
	tv.tv_sec = pnew_time_packet->transmit_timestamp.coarse - JAN_1970;
	tv.tv_usec = USEC(pnew_time_packet->transmit_timestamp.fine);
	cpss_sntp_get_seconds = tv.tv_sec; /* 获取秒数 */

	/* check sec of GMT */

	//	gmt_sec = cpss_gmt_get();
	//	    	printf("GMT secs Time of SYS:\t %d\n",gmt_sec);
	return settimeofday(&tv, NULL);
}
#if 0 //dhwang added
int get_ntp_server_ip()
{
	OM_GET_AC_SVRCFG_MSG_RSP_T stRsp;
	OM_GET_AC_SVRCFG_MSG_REQ_T stReq;

	stReq.ucType = 5;
	rdbs_ac_if_entry(OM_GET_AC_SVRCFG_MSG, RDBS_IF_FUNC,
			(UINT8*)&stReq,sizeof(OM_GET_AC_SVRCFG_MSG_REQ_T),
			(UINT8*)&stRsp, sizeof(OM_GET_AC_SVRCFG_MSG_RSP_T));
	if (RDBS_RESULT_OK != stRsp.ulResult)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_ERROR, "get_ntp_server_ip from rdbs failed\n!");
		g_SntpTimer = 60;
		return OAMS_ERROR;
	}
	else
	{
		ntpsvrip[0] = stRsp.stSvrAddr.aucServerIP[0];
		ntpsvrip[1] = stRsp.stSvrAddr.aucServerIP[1];
		ntpsvrip[2] = stRsp.stSvrAddr.aucServerIP[2];
		ntpsvrip[3] = stRsp.stSvrAddr.aucServerIP[3];

		if(1 > stRsp.stSvrAddr.ulNtpCycle)
		{
			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_ERROR, "get_ntp_server_ip get Bad Cycle\n!");
			g_SntpTimer = 60;
		}
		else
		{
			g_SntpTimer = stRsp.stSvrAddr.ulNtpCycle;
		}
	}
	return 0;
}

int sntp_set_time()
{
	struct ntp_packet new_time_packet;
	CPSS_TIME_T stTime;
	time_t timep;

	if (get_ntp_time(g_SntpSock, &new_time_packet))
	{

		if (!set_local_time(&new_time_packet))
		{
			cps_sntp_set_app_clock();

			//            printf("The NTP client Time Syn Success!\n");
		}

		/* check time and display */
		cpss_clock_get(&stTime);
		time(&timep);
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_INFO,
				"Time of SYSTEM:\t %s", asctime(localtime(&timep)));

		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_INFO,
				"\ncpss_sntp_get_seconds: %d\n", cpss_sntp_get_seconds);
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_INFO,
				"*******************************************\n\n");


	}

	return (cpss_sntp_get_seconds);
}
#endif
#if 0
VOID cpss_tm_sntp_proc(UINT16 usState, VOID *pVar,
		CPSS_COM_MSG_HEAD_T *pstMsgHead)
{
	INT32 lRet;
	switch (usState)
	{
	case SNTP_INIT_STATE:
	{
		switch (pstMsgHead->ulMsgId)
		{
		case SMSS_PROC_ACTIVATE_REQ_MSG:
		{
			lRet = sntp_socket_init();
			sntp_set_time();
			sntp_send_smss_activate_rsp(lRet);
			break;
		}
		case SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG:
		{
			cpss_vk_proc_user_state_set(SNTP_NORMAL_STATE);
			g_SntpTimer = 1;
			cpss_timer_set(CPSS_SNTP_LOOP_TIMER, g_SntpTimer * 1000);
			break;
		}
		default:
			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_WARN,
							"cpss_tm_sntp_proc receive unknown msg in INIT state, Msgid = %x\n", pstMsgHead->ulMsgId);
			break;
		}
		break;
	}
	case SNTP_NORMAL_STATE:
	{
		switch (pstMsgHead->ulMsgId)
		{
		case CPSS_SNTP_LOOP_TIMER_MSG:
			get_ntp_server_ip();
			cpss_timer_set(CPSS_SNTP_LOOP_TIMER, g_SntpTimer * 1000);
			sntp_set_time();
			break;
		default:
			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_WARN,
							"cpss_tm_sntp_proc receive unknown msg in NORMAL state, Msgid = %x\n", pstMsgHead->ulMsgId);
			break;
		}
		break;
	}
	default:
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_TM, CPSS_PRINT_WARN,
						"cpss_tm_sntp_proc receive unknown state, state = %x\n", pstMsgHead->ulMsgId);
		break;
	}
}
#endif

INT32 sntp_socket_init()
{
	struct sockaddr_in sockadd =
	{ 0 };
	int rc;
	/* getaddrinfo */

	g_SntpSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (-1 == g_SntpSock)
	{
		perror("sntp socket");
		return CPSS_ERROR;
	}

	sockadd.sin_addr.s_addr = inet_addr(SRC_IP);
	sockadd.sin_port = htons(SRC_PORT);
	sockadd.sin_family = AF_INET;
	rc = bind(g_SntpSock, (struct sockaddr *) &sockadd, sizeof(sockadd));
	if (-1 == bind)
	{
		return CPSS_ERROR;
	}

	return CPSS_OK;
}

void sntp_send_smss_activate_rsp(UINT32 ulResult)
{
	CPSS_COM_LOGIC_ADDR_T stLocalAddr;
	UINT32 ulLogicAddrFlag;
	CPSS_COM_PID_T stDstPid;
	SMSS_PROC_ACTIVATE_RSP_MSG_T stRspMsg;

	cpss_com_logic_addr_get(&stLocalAddr, &ulLogicAddrFlag);

	stDstPid.stLogicAddr = stLocalAddr;
	stDstPid.ulAddrFlag = ulLogicAddrFlag;
	stDstPid.ulPd = SMSS_SYSCTL_PROC;

	stRspMsg.ulResult = ulResult;
	cpss_com_send(&stDstPid, SMSS_PROC_ACTIVATE_RSP_MSG, (UINT8 *) &stRspMsg,
			sizeof(SMSS_PROC_ACTIVATE_RSP_MSG_T));
}

void cps_sntp_set_app_clock()
{
	time_t ulSeconds = 0;
	struct tm *pstBreakDownTime;
	CPSS_TM_SYSSOFTCLOCK_T stClock;

	memset(&stClock, 0, sizeof(CPSS_TM_SYSSOFTCLOCK_T));
	ulSeconds = time(NULL);
	pstBreakDownTime = localtime(&ulSeconds);

	stClock.usSysYear = (UINT16) (pstBreakDownTime->tm_year + 1900);
	stClock.ucSysMon = (UINT8) (pstBreakDownTime->tm_mon + 1);
	stClock.ucSysDay = (UINT8) pstBreakDownTime->tm_mday;
	stClock.ucSysHour = (UINT8) pstBreakDownTime->tm_hour;
	stClock.ucSysMin = (UINT8) pstBreakDownTime->tm_min;
	stClock.ucSysSec = (UINT8) pstBreakDownTime->tm_sec;

	cpss_timer_set_sys_clock(&stClock);
}
