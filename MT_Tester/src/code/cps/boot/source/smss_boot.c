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
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <dirent.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <signal.h>

#include "smss_boot.h"

#if SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA
#define MSG_KEY 8964
#elif SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA
#define MSG_KEY 6489
#endif

#define BOOT_MSG_LENTH 128
#define BOOT_VER_LENTH 128

#define CCU_UDP_IP "11.33.9.1"
#define CCU_UDP_PORT 5050
#define PPU_DPB1_UDP_IP "11.33.17.2"
#define PPU_DPB2_UDP_IP "11.33.17.3"
#define PPU_UDP_PORT 5050

#define CPS_BOOT_CPU_REBOOT_IND_MSG 0x21080000
#define CPS_BOOT_RENEW_VERSION_IND_MSG 0x21080001
#define CPS_BOOT_DOWNLOAD_FAIL_ALARM 0x21080002

char SockTXBuf[512];	/*socket发送buf*/
char SockRXBuf[512];	/*socket接收buf*/
CPSS_COM_MSG_HEAD_T *SockTXMsg = (CPSS_COM_MSG_HEAD_T *)(SockTXBuf + sizeof(CPSS_COM_TRUST_HEAD_T));	/*发送cps消息头指针*/
CPSS_COM_MSG_HEAD_T *SockRXMsg = (CPSS_COM_MSG_HEAD_T *)(SockRXBuf + sizeof(CPSS_COM_TRUST_HEAD_T));	/*接收cps消息头指针*/
CPSS_COM_PHY_ADDR_T g_stPhyAddr;
CPSS_COM_PHY_ADDR_T g_stLocalPhyAddr;

typedef struct _BOOT_MSG_T_
{
	UINT32 ulMsgId;
	UINT8 ucMsg[128];
}BOOT_MSG_T;

typedef struct _BOOT_IPC_MSG_T_
{
	long lMsgType;
	char cIpcMsg[132];
}BOOT_IPC_MSG_T;

typedef struct _BOOT_INFO_T_
{
	char Local_ver[BOOT_VER_LENTH];
	char Running_ver[BOOT_VER_LENTH];
	char Update_ver[BOOT_VER_LENTH];
	pid_t RunPid;
	int MsgId;
	int SockId;
}BOOT_INFO_T;

BOOT_INFO_T g_BootInfo;

#ifdef SWP_LINUX
#define CCU_LOCAL_PATH "/root/workspace/wlan/project/release/ccu_release/Debug/"
#define PPU_LOCAL_PATH "/root/workspace/wlan/project/release/ppu_dpb1_release/Debug/"
#else
#define CCU_LOCAL_PATH "/usr/local/wlan/ACSubSystem/CPS/"
#define PPU_LOCAL_PATH "/usr/local/wlan/ACSubSystem/CPS/"
#endif

void boot_init();
void boot_ver_get_local();
void boot_ver_get_host();
void boot_run_app(char* ver);
void boot_running();
void boot_rsv_cps_msg(char* pstBootMsg);
void boot_snd_cps_msg(char* pstBootMsg);
void boot_cpu_reboot(UINT8 ucRebootType);
void boot_ver_update();
void boot_kw_app();
void boot_kwing();
void boot_ver_delete(char* ver);
int boot_ver_compare(char* ver1,char* ver2);
int boot_ver_download(char* ver);
void socket_send_msg_to_ccu(UINT32 msg,UINT32 ulLen,char* puc);
int recieve_ccu_rsp(void);
void boot_ver_renew();
void send_message_init();
void boot_download_alarm();

void boot_untmp(char* ver);
void boot_sig_quit();
int boot_get_cpuid();

int main(int argc,char* argv[])
{
	boot_init();

	boot_ver_get_local();

#if SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA

	boot_ver_update();

#endif

	boot_run_app(g_BootInfo.Local_ver);

//	boot_kwing();

	boot_kw_app();

	boot_running();

	return 0;
}

void boot_com_init()
{
	struct sockaddr_in stLocal = {0};
	int flags;
	int lDpbNo;

	lDpbNo = boot_get_cpuid();

	if(1 == lDpbNo)
	{
		stLocal.sin_addr.s_addr = inet_addr(PPU_DPB1_UDP_IP);
		g_stLocalPhyAddr.ucFrame = 1;
		g_stLocalPhyAddr.ucShelf = 1;
		g_stLocalPhyAddr.ucSlot = 3;
		g_stLocalPhyAddr.ucCpu = 1;
	}
	else if(2 == lDpbNo)
	{
		stLocal.sin_addr.s_addr = inet_addr(PPU_DPB2_UDP_IP);
		g_stLocalPhyAddr.ucFrame = 1;
		g_stLocalPhyAddr.ucShelf = 1;
		g_stLocalPhyAddr.ucSlot = 4;
		g_stLocalPhyAddr.ucCpu = 1;
	}

	stLocal.sin_family = AF_INET;
	stLocal.sin_port = boot_htons(PPU_UDP_PORT);

	while(1)
	{
		g_BootInfo.SockId = socket(AF_INET, SOCK_DGRAM, 0);

		if(g_BootInfo.SockId == -1)
		{
			perror("socket");
			close(g_BootInfo.SockId);
			sleep(1);
			continue;
		}
		else
		{
			printf("socket init ok ! sockId = %d\n", g_BootInfo.SockId);
		}

		if(-1 == bind(g_BootInfo.SockId, (struct sockaddr *)&stLocal, sizeof(stLocal)))
		{
			perror("bind");
			close(g_BootInfo.SockId);
			sleep(1);
			continue;
		}
		else
		{
			if(1 == lDpbNo)
			{
				printf("bind IP ok! IP = %s\n", PPU_DPB1_UDP_IP);
			}
			else if(2 == lDpbNo)
			{
				printf("bind IP ok! IP = %s\n", PPU_DPB2_UDP_IP);
			}
		}

		flags = fcntl(g_BootInfo.SockId, F_GETFL, 0);
		if(flags == -1)
		{
			perror("fcntl");
			close(g_BootInfo.SockId);
			sleep(1);
		}
	   if(-1 == fcntl(g_BootInfo.SockId, F_SETFL, flags|O_NONBLOCK))
		{
		   perror("fcntl");
			close(g_BootInfo.SockId);
		   sleep(1);
	    }
	   else
	   {
		   return;
	   }
	}

   return;
}

void boot_ipc_init()
{
	while(1)
	{
		g_BootInfo.MsgId = msgget(MSG_KEY, IPC_CREAT);
		msgctl(g_BootInfo.MsgId, IPC_RMID, NULL);
		g_BootInfo.MsgId = msgget(MSG_KEY, IPC_CREAT);
		if (g_BootInfo.MsgId == -1)
		{
			printf("msgget error! errno = %d\n", errno);
			sleep(1);
			continue;
		}
		else
		{
			printf("msg creat ok! msgId = %d\n", g_BootInfo.MsgId);
			return;
		}
	}
	return;
}

void boot_init()
{
	signal(SIGTERM, boot_sig_quit);
#if SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA
	boot_com_init();
	send_message_init();
#endif
	boot_ipc_init();
}

void boot_ver_get_local()
{
	DIR				*dp;
	struct dirent   *dirp;
	struct stat fst;
	char file_path[100] = {0};
	/*版本后缀名*/
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
	char local_dir[100] = CCU_LOCAL_PATH;
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
	char local_dir[100] = PPU_LOCAL_PATH;
#endif
//	char v[10] = ".ver";
    /*打开版本所在目录*/
	while(1)
	{
		if ((dp = opendir(local_dir)) == NULL)
		{
			printf("can't open local dir ! \n");
			sleep(1);
			continue;
		}
		/*读取目录中的文件*/
		while ((dirp = readdir(dp)) != NULL)
		{
			/*获得文件名*/
			char *s = dirp->d_name;
	//   	char *p;
	//		printf("file name = %s\n", dirp->d_name);
	//		printf("file type = %d\n", dirp->d_type);
	//		printf("file rlen = %d\n", dirp->d_reclen);

			if((0 == strcmp(s, "boot_ccu")) || (0 == strcmp(s, "boot_ppu")))
			{
				continue;
			}

			memset(file_path, 0, sizeof(file_path));
			strcpy(file_path, local_dir);
			strcat(file_path, dirp->d_name);

	 // 	printf("file_path = %s\n", file_path);

			if(-1 == stat(file_path, &fst))
			{
				printf("get file stat error!\n");
				printf("file name = %s\n",dirp->d_name);
				printf("errno = %d\n",errno);
			}

	//   	printf("file mode = %d\n", fst.st_mode);

			if((fst.st_mode & S_IXUSR) && (S_ISREG(fst.st_mode)))
			{
				printf("Local_ver = %s\n", dirp->d_name);
				strcpy(g_BootInfo.Local_ver,s);
				return;
			}
#if 0
			/*文件名长度<=4的不处理*/
			if(strlen(s)>4)
			{
				/*p指向文件名的后4个字节*/
				p = (s + strlen(s)-4);
				/*与版本后缀名比较*/
				if(strcmp(p,v)==0)
				{
					printf("find ver file is %s\n",s);
					/*关闭目录*/
					closedir(dp);
					/*返回文件名*/
					strcpy(g_BootInfo.Local_ver,s);
					printf("local_ver is %s\n", g_BootInfo.Local_ver);
					return;
				}
			}
#endif
		}
		/*未找到处理*/
#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
		printf("can not find any version!\n");
		closedir(dp);
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
		printf("can not find any version!\n ask ccu for help !\n");
		/*设local_ver为 0*/
	   memset(g_BootInfo.Local_ver,0,sizeof(g_BootInfo.Local_ver));
		/*关闭目录*/
	   closedir(dp);
	   return;
#endif
	   sleep(1);
	}
	/*返回空值*/
	return;
}

void boot_run_app(char* ver)
{
	char ver_bak[100] = {0};


	if(NULL == ver)
	{
		printf("ver = NULL at boot_run_app\n!exit!");
		exit(1);
	}

	strcpy(ver_bak, ver);

//	printf("before memset , g_BootInfo.Running_ver is %s\n",g_BootInfo.Running_ver);

	memset(g_BootInfo.Running_ver, 0, sizeof(g_BootInfo.Running_ver));

	strcpy(g_BootInfo.Running_ver,ver_bak);

	printf("now running ver is %s\n",g_BootInfo.Running_ver);

//	printf("now ver is %s\n", ver_bak);

	/*创建子进程*/
	if ((g_BootInfo.RunPid = fork())== 0)
	{
//		printf("in new process!\n");
		/*本地文件路径*/
		char path[100] = {0};
#if	(SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
		strcpy(path, CCU_LOCAL_PATH);
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
		strcpy(path, PPU_LOCAL_PATH);
#endif
		/*连接为文件绝对路径*/
		strcat(path,ver_bak);
		printf("now begin to run %s!\n",path);
		/*执行该版本*/
		execl(path, g_BootInfo.Running_ver, NULL);
	}
	else
	{
	printf("app's pid = %d\n", g_BootInfo.RunPid);
	}
	return;
}

void boot_running()
{
	BOOT_MSG_T stBootMsg = {0};
	printf("now in running mode !\n");

	while(1)
	{
		boot_rsv_cps_msg((char*)&stBootMsg);

		printf("Receive msgId = %x\n",stBootMsg.ulMsgId);

		switch(stBootMsg.ulMsgId)
		{
			case CPS_BOOT_CPU_REBOOT_IND_MSG:
				boot_cpu_reboot(stBootMsg.ucMsg[0]);
				break;
			case CPS_BOOT_RENEW_VERSION_IND_MSG:
				boot_ver_renew(stBootMsg.ucMsg);
				break;
			default:
				printf("receive error msg! msgId = %x\n",stBootMsg.ulMsgId);
				break;
		}
	}
}

void boot_kw_app()
{
	pthread_t stPtid;
	pthread_create(&stPtid, NULL, (void*)boot_kwing, NULL);
	return;
}

void boot_kwing()
{
	int wait_time = 3;

	int loop;

	printf("now begin to kw process!\n");

	while(1)
	{
		waitpid(g_BootInfo.RunPid, NULL, 0);

		for(loop = 0; loop < wait_time; loop++)
		{
			printf("process is over, restart in %d seconds!\n", wait_time - loop);

			sleep(1);
		}

		printf("in kwing Running_ver = %s\n", g_BootInfo.Running_ver);

		boot_run_app(g_BootInfo.Running_ver);
	}
	return;
}

void boot_rsv_cps_msg(char* pstBootMsg)
{
	BOOT_IPC_MSG_T stIpcMsg = {0};

	if(NULL == pstBootMsg)
	{
		printf("stBootMsg is NULL in boot_rsv_cps_msg, return!\n");
		return;
	}

	if(-1 == msgrcv(g_BootInfo.MsgId, &stIpcMsg, 132, 1, 0))
	{
		printf("msgrcv error! errno = %d\n", errno);
		perror("msgrcv");
		return;
	}

	memcpy(pstBootMsg, stIpcMsg.cIpcMsg, sizeof(BOOT_MSG_T));

	return;
}

void boot_cpu_reboot(UINT8 ucRebootType)
{
	char cmd[100] = {0};
	char *shutdowncmd = "shutdown -r 0";
	int loop;
	switch(ucRebootType)
	{
		case 0:
			for(loop = 0;loop < 1;loop++)
			{
				printf("boot will kill app in %d senconds!\n", 1 - loop);
				sleep(1);
			}

			if(kill(g_BootInfo.RunPid, SIGQUIT) == -1)
			{
				printf("kill process error!\n");
				return;
			}
			else
			{
				printf("process has been killed!\n");
			}

			break;
		case 1:
			strcpy(cmd, shutdowncmd);

			for(loop = 0;loop < 5;loop++)
			{
				printf("system will reboot in %d senconds!\n", 5 - loop);
				sleep(1);
			}

			system(cmd);

			break;
		default:
			break;
	}

	return;

#if 0
	printf("now begin to kill process!\n");
	if(kill(g_BootInfo.RunPid, SIGQUIT) == -1)
	{
		printf("kill process error!\n");
		return;
	}
	else
	{
		printf("process has been killed!\n");
	}
	return;
#endif
}

int boot_ver_download(char* ver)
{
	char* IP = " 11.33.9.1";

	struct stat fst;
	int sysret;

	char cmd[100] = {0};
	char cmd_map[100] = {0};

	char ver_map[100] = {0};

	char ver_tmp[100] = {0};
	char ver_map_tmp[100] = {0};

	char map[10] = ".map";
	char tmp[10] = ".tmp";

	strcpy(ver_map, ver);
	strcat(ver_map, map);

	strcpy(ver_tmp, ver);
	strcat(ver_tmp, tmp);

	strcpy(ver_map_tmp, ver_map);
	strcat(ver_map_tmp, tmp);

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)

	strcpy(cmd, "tftp -gr ");
	strcat(cmd, ver);
	strcat(cmd, " -l ");
	strcat(cmd, ver_tmp);
	strcat(cmd, IP);

	strcpy(cmd_map, "tftp -gr");
	strcat(cmd_map, ver_map);
	strcat(cmd_map, " -l ");
	strcat(cmd_map, ver_map_tmp);
	strcat(cmd_map, IP);

#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)

	strcpy(cmd, "tftp -v");
	strcat(cmd, IP);
	strcat(cmd, " -c get ");
	strcat(cmd, ver);
	strcat(cmd, " ");
	strcat(cmd, ver_tmp);

	strcpy(cmd_map, "tftp -v");
	strcat(cmd_map, IP);
	strcat(cmd_map, " -c get ");
	strcat(cmd_map, ver_map);
	strcat(cmd_map, " ");
	strcat(cmd_map, ver_map_tmp);

#endif
	printf("now cmd is %s\n",cmd);
	printf("now begin to download ver!\n");
	sysret = system(cmd);

	if(-1 == stat(ver_tmp, &fst))
	{
		printf("get file stat error\n");
		boot_ver_delete(ver_tmp);
		return BOOT_ERROR;
	}

	if(-1 == chmod(ver_tmp, fst.st_mode | S_IXUSR))
	{
		printf("chown error\n");
		boot_ver_delete(ver_tmp);
		return BOOT_ERROR;
	}

	if (fst.st_size != 0)
	{
		printf("download ver success!\n");
	}
	else
	{
		printf("download ver failed !\n");
		boot_ver_delete(ver_tmp);
		return BOOT_ERROR;
	}

	printf("now cmd_map is %s\n",cmd_map);
	printf("now begin to download ver!\n");

	system(cmd_map);

	if(-1 == stat(ver_map_tmp, &fst))
	{
		printf("get file stat error\n");
		boot_ver_delete(ver_tmp);
		boot_ver_delete(ver_map_tmp);
		return BOOT_ERROR;
	}

#if 0
	if(-1 == chmod(ver_map_tmp, fst.st_mode | S_IXUSR))
	{
		printf("chown error\n");
		return BOOT_ERROR;
	}
#endif

	if (fst.st_size != 0)
	{
		printf("download ver success!\n");
	}
	else
	{
		printf("download ver failed !\n");
		boot_ver_delete(ver_tmp);
		boot_ver_delete(ver_map_tmp);
		return BOOT_ERROR;
	}


	return BOOT_OK;
}

void boot_ver_get_host()
{
	UINT32 msglen;
	int looptime;
	msglen = sizeof(g_stPhyAddr);
	while(1)
	{
		looptime = 5;
		/*向ccu发送版本查询*/
		socket_send_msg_to_ccu(CPS_VER_CFG_QUERY_REQ_MSG, msglen, (char*)&g_stPhyAddr);
		while(looptime--)
		{
			if (recieve_ccu_rsp() != BOOT_OK)
			{
				usleep(200*1000);
				continue;
			}
			printf("recieve host_ver = %s\n",g_BootInfo.Update_ver);
			
			//close(g_BootInfo.SockId);
			
			return;
		}
	}
}

int boot_ver_compare(char* ver1,char* ver2)
{
	/*版本文件名比较*/
	if(strcmp(ver1,ver2) == 0)
	{
		return BOOT_OK;
	}
	else
	{
		return BOOT_ERROR;
	}
}

void boot_ver_update()
{
	char ver_map[100];
	char* map =".map";

	printf("now begin to update!\n");

	while(1)
	{
		/*服务端版本获得*/
		boot_ver_get_host();

		/*与服务器版本比较*/

		if (BOOT_OK == boot_ver_compare((char*)&g_BootInfo.Update_ver, (char*)&g_BootInfo.Local_ver))
//		if(0)
		{
			/*运行本地版本*/
			close(g_BootInfo.SockId);

			//boot_run_app(g_BootInfo.Local_ver);
			return;
		}
		else
		{
			/*新版本下载*/
			if(BOOT_OK != boot_ver_download(g_BootInfo.Update_ver))
			{
				UINT32 msglen;
				msglen = sizeof(g_stPhyAddr);
				printf("boot ver download failed!\n");
				printf("send verm download failed msg!\n");
				/*向CCU_VERM发送升级失败响应*/
				socket_send_msg_to_ccu(CPS_VER_UPDATE_RSP_MSG, msglen, (char*)&g_stPhyAddr);

				sleep(5);

				continue;
			}
			else
			{
				/*删除本地文件*/
				memset(ver_map, 0, sizeof(ver_map));
				strcpy(ver_map, g_BootInfo.Local_ver);
				strcat(ver_map, map);

				boot_ver_delete(g_BootInfo.Local_ver);
				boot_ver_delete(ver_map);

				/*将.tmp文件还原*/
				boot_untmp(g_BootInfo.Update_ver);

				strcpy(g_BootInfo.Local_ver, g_BootInfo.Update_ver);

				close(g_BootInfo.SockId);

//				boot_run_app(g_BootInfo.Update_ver);
				return;
			}
		}
	}
	return;
}

void boot_snd_cps_msg(char* pstBootMsg)
{
	BOOT_IPC_MSG_T stIpcMsg = {0};

	if(NULL == pstBootMsg)
	{
		printf("stBootMsg is NULL in boot_snd_cps_msg, return!\n");
		return;
	}

	stIpcMsg.lMsgType = 2;
	memcpy(stIpcMsg.cIpcMsg, pstBootMsg, sizeof(BOOT_MSG_T));
	if(-1 == msgsnd(g_BootInfo.MsgId, &stIpcMsg, 132, IPC_NOWAIT))
	{
		perror("msgsnd");
	}
	return;
}

/*ppu向ccu发送版本查询请求*/
void socket_send_msg_to_ccu(UINT32 msg,UINT32 ulLen,char* puc)
{
	struct sockaddr_in stSockAddr = {0};

	stSockAddr.sin_family = AF_INET;
	stSockAddr.sin_addr.s_addr = inet_addr(CCU_UDP_IP);
	stSockAddr.sin_port = boot_htons(CCU_UDP_PORT);

	memcpy(&SockTXMsg->stSrcProc.stLogicAddr, &g_stLocalPhyAddr, sizeof(CPSS_COM_PHY_ADDR_T));
	SockTXMsg->ulMsgId  = boot_htonl(msg);
	SockTXMsg->ulLen    = boot_htonl(ulLen);
	memcpy(SockTXMsg->pucBuf, puc, ulLen);

	/*通过socket发送信息*/
	if (sendto(g_BootInfo.SockId, SockTXBuf, sizeof(CPSS_COM_PHY_ADDR_T)+sizeof(CPSS_COM_TRUST_HEAD_T)+sizeof(CPSS_COM_MSG_HEAD_T), 0, (struct sockaddr *)&stSockAddr, sizeof(stSockAddr)) == -1)
	{
		printf("send to ccu error errno = %d\n",errno);
		perror("sendto");
	}
	else
	{
		printf("send ver request to ccu!\n");
	}
	return;
}

int recieve_ccu_rsp(void)
{
	struct sockaddr_in stSockAddr;
	UINT32 iFromLen = sizeof(stSockAddr);
	char* s;

	if(recvfrom(g_BootInfo.SockId, SockRXBuf, sizeof(SockRXBuf), 0, (struct sockaddr *)&stSockAddr, &iFromLen) == -1)
	{
		return BOOT_ERROR;
	}
	else
	{
		/*判断是否为版本查询响应*/
		if(CPS_VER_CFG_QUERY_RSP_MSG != boot_ntohl(SockRXMsg->ulMsgId))
		{
			printf("recieve error msg from ccu!\n");
			return BOOT_ERROR;
		}
		s = (char *)(SockRXBuf + sizeof(CPSS_COM_TRUST_HEAD_T) + sizeof(CPSS_COM_MSG_HEAD_T));

		if(0 == (UINT8)*s)
		{
			printf("recieve null version!\n");
			return BOOT_ERROR;
		}

		printf("recieve ver = %s\n", s);
		memset(g_BootInfo.Update_ver, 0, sizeof(g_BootInfo.Update_ver));
		strcpy(g_BootInfo.Update_ver, s);
		printf("now host_ver is %s\n",g_BootInfo.Update_ver);
	}
	return BOOT_OK;
}

void boot_ver_renew(char* ver)
{
	char ver_map[100];
	char* map = ".map";

	strcpy(g_BootInfo.Local_ver, g_BootInfo.Running_ver);
	strcpy(g_BootInfo.Update_ver, ver);

	if(BOOT_OK != boot_ver_download(g_BootInfo.Update_ver))
	{
		printf("ver renew error!\n");
		boot_download_alarm();
		return;
	}
	else
	{
		printf("ver renew ok! restart!\n");
		strcpy(g_BootInfo.Running_ver, ver);

		/*重启应用*/
		boot_cpu_reboot(0);

		/*删除本地版本*/
		memset(ver_map, 0, sizeof(ver_map));
		strcpy(ver_map, g_BootInfo.Local_ver);
		strcat(ver_map, map);

		boot_ver_delete(g_BootInfo.Local_ver);
		boot_ver_delete(ver_map);

		/*将.tmp文件还原*/
		boot_untmp(g_BootInfo.Update_ver);
	}
	return;
}

void send_message_init()
{
	/*可靠传输头初始化*/
	CPSS_COM_TRUST_HEAD_T *TrustHead = (CPSS_COM_TRUST_HEAD_T *)&SockTXBuf;
	memset(TrustHead, 0, sizeof(CPSS_COM_TRUST_HEAD_T));
	TrustHead->ucPType = 0;
	TrustHead->hdrLen = 14;
	TrustHead->ver = (1);

	/*数据头初始化*/
	memset(SockTXMsg, 0, sizeof(CPSS_COM_MSG_HEAD_T));
	SockTXMsg->stDstProc.ulAddrFlag = boot_htonl(CPSS_COM_ADDRFLAG_PHYSICAL);
	SockTXMsg->stDstProc.ulPd       = boot_htonl(SMSS_VERM_PROC);
	SockTXMsg->ucPriFlag            = CPSS_COM_PRIORITY_NORMAL;
	SockTXMsg->ucAckFlag            = CPSS_COM_NOT_ACK;
	SockTXMsg->pucBuf               = ((UINT8 *)SockTXMsg) + sizeof(CPSS_COM_MSG_HEAD_T);

	g_stPhyAddr.ucFrame = 1;
	g_stPhyAddr.ucShelf = 1;
	g_stPhyAddr.ucSlot = 1;
	g_stPhyAddr.ucCpu = 1;

	return;
}

void boot_ver_delete(char* ver)
{
	printf("delete old ver! ver = %s\n",ver);
	unlink(ver);
#if 0
	char cmdrm[100] = "rm -f ";
	strcat(cmdrm,ver);
	system(cmdrm);
#endif
}

void boot_download_alarm()
{
	UINT32 test;
	BOOT_MSG_T stBootMsg = {0};
	test = 0x1122;
	printf("send verm down load error\n");

	stBootMsg.ulMsgId = CPS_BOOT_DOWNLOAD_FAIL_ALARM;
	memcpy(stBootMsg.ucMsg, &test, sizeof(UINT32));
	boot_snd_cps_msg((char*)&stBootMsg);
	return;
}

int file_is_exe(char* path)
{
	struct stat fst;
	if(-1 == stat(path, &fst))
	{
		printf("get stat error!\n");
		return -1;
	}

	if((fst.st_mode & S_IXUSR) && (S_ISREG(fst.st_mode)))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

void boot_untmp(char* ver)
{
	char ver_map[100] = {0};

	char ver_tmp[100] = {0};
	char ver_map_tmp[100] = {0};

	char map[10] = ".map";
	char tmp[10] = ".tmp";

	strcpy(ver_map, ver);
	strcat(ver_map, map);

	strcpy(ver_tmp, ver);
	strcat(ver_tmp, tmp);

	strcpy(ver_map_tmp, ver_map);
	strcat(ver_map_tmp, tmp);

	rename(ver_tmp, ver);
	rename(ver_map_tmp, ver_map);

	return;
}

void boot_sig_quit()
{
	printf("receive SIGTERM ! kill boot and app! \n");
	kill(g_BootInfo.RunPid, SIGTERM);
	exit(1);
}

int boot_get_cpuid()
{
	FILE* fProc;
	char pcBuf[50];
	int lBayId;
	int lCpuId;

	lCpuId = 1;

	fProc = fopen("/proc/octeon_info", "r");
	if(NULL == fProc)
	{
		printf("open /proc/octeon_info fail!, use default\n");
		return lCpuId;
	}

	while(!feof(fProc))
	{
		fscanf(fProc, "%s", pcBuf);
		if(0 == strcmp("bay", pcBuf))
		{
			fscanf(fProc, "%s", pcBuf);
			if(0 == strcmp("id:", pcBuf))
			{
				fscanf(fProc, "%d", &lBayId);
				switch(lBayId)
				{
					case 1:
						printf("Get DpbId = 1\n");
						lCpuId = 1;
						break;
					case 2:
						printf("Get DpbId = 2\n");
						lCpuId = 2;
						break;
					default:
						printf("Get Error CpuId, use Default\n");
						break;
				}
				break;
			}
		}
	}

	fclose(fProc);

	return lCpuId;
}

