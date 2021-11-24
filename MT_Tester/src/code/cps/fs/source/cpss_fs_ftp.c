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
#ifndef CPSS_DSP_CPU
#include "cpss_public.h"
#include "smss_public.h"

#include "cpss_common.h"
#include "cpss_fs_ftp.h"
#include "cpss_fs_file.h"
#include "cpss_err.h"

#include "cpss_fs_ftplib.h"
#include "cpss_fs_tftpLib.h"

#include <arpa/inet.h>

#ifdef CPSS_VOS_WINDOWS
#define CPSS_SYS_SOCKET_FTP
#endif

#ifdef SWP_FNBLK_BRDTYPE_ABOX
#undef  CPSS_SYS_SOCKET_FTP
#define CPSS_SYS_SOCKET_FTP
#endif



/******************************* 局部宏定义 **********************************/

/******************************* 全局变量定义/初始化 *************************/

/******************************* 局部常数和类型定义 **************************/

/******************************* 局部函数原型声明 ****************************/
extern INT32 cpss_fs_get_file_name(STRING szDirName, STRING szFileName);
extern INT32 cpss_filename_check(CHAR *pcFileName, BOOL bIfAbsDir);
extern INT32 cpss_fs_dirpath_unlink(STRING szDirPath);
extern INT32 cpss_fs_dirpath_link(STRING szDirPath, STRING szDirName,
		UINT32 ulPathLen);
INT32 cpss_ftp_msg_send(CPSS_COM_PID_T* pstDstPid, UINT8 *pucSendData,
		UINT32 ulMsgId, UINT32 ulSendLen);

INT32 cps_file_trans_req_deal(CPS_FILE_TRANS_REQ_MSG_T* pstTransReq);
INT32 cps_file_trans_ftp_deal(CPS_FILE_TRANS_REQ_MSG_T* pstTransReq);
INT32 cps_file_trans_tftp_deal(CPS_FILE_TRANS_REQ_MSG_T* pstTransReq);

/*FS模块的打印开关*/
UINT32 g_ulCpssFsBytesPrint = 0;

/*FTP的互斥信号量*/
UINT32 g_ulCpssFtpMutex = CPSS_VOS_MUTEXD_INVALID;

/*TFTP的互斥信号量*/
UINT32 g_ulCpssTftpMutex = CPSS_VOS_MUTEXD_INVALID;

/*FTP传输标识符的基准*/
UINT32 g_ulCpssFtpSerialBase = 0;

CPSS_FS_HANDLE_NODE_T* g_pstCpssFdLink = NULL;
UINT32 g_ulCpssTftpSerialNo = 0;
UINT32 g_ulTftpTransLinkLow = 0;
UINT32 g_ulTftpTransLinkHigh = 0;

INT32 g_lCpsFsFtError;


/******************************* 函数实现 ************************************/
/*******************************************************************************
 * 函数名称: cpss_fs_htonl_xfer_req
 * 功    能: 将本机字节序转为网络字节许
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回: 无
 * 说   明:
 *
 *******************************************************************************/
VOID cpss_fs_htonl_xfer_req(CPS__TNBS_FTP_XFER_REQ_MSG_T *ptXferReq)
{

	if (ptXferReq == NULL)
		return;

	ptXferReq->ulCmdId = cpss_htonl( ptXferReq->ulCmdId );
	ptXferReq->ulVal = cpss_htonl(ptXferReq->ulVal );
	/*    (UINT32)ptXferReq->pfStatusHook = cpss_htonl(ptXferReq->pfStatusHook );*/

}

VOID cpss_fs_ntohl_xfer_rsp(CPS__TNBS_FTP_XFER_RSP_MSG_T *ptXferRsp)
{
	if (ptXferRsp == NULL)
		return;

	ptXferRsp->lResult = cpss_ntohl(ptXferRsp->lResult);
	ptXferRsp->ulCmdId = cpss_ntohl(ptXferRsp->ulCmdId );
	ptXferRsp->ulCtrlSock = cpss_ntohl(ptXferRsp->ulCtrlSock );
	ptXferRsp->ulDataSock = cpss_ntohl(ptXferRsp->ulDataSock);
	ptXferRsp->ulReplyStringLength = cpss_ntohl(ptXferRsp->ulReplyStringLength);

}

VOID cpss_fs_htonl_cmd_req(CPS__TNBS_FTP_COMMAND_REQ_MSG_T *ptCmdReq)
{
	if (ptCmdReq == NULL)
		return;

	ptCmdReq->ulCmdId = cpss_htonl(ptCmdReq->ulCmdId);
	ptCmdReq->ulLen = cpss_htonl( ptCmdReq->ulLen );
	ptCmdReq->ulSocketId = cpss_htonl( ptCmdReq->ulSocketId);
	ptCmdReq->ulArg1 = cpss_htonl( ptCmdReq->ulArg1);
	ptCmdReq->ulArg2 = cpss_htonl( ptCmdReq->ulArg2);
	ptCmdReq->ulArg3 = cpss_htonl( ptCmdReq->ulArg3);
	ptCmdReq->ulArg4 = cpss_htonl( ptCmdReq->ulArg4);
	ptCmdReq->ulArg5 = cpss_htonl( ptCmdReq->ulArg5);
	ptCmdReq->ulArg6 = cpss_htonl( ptCmdReq->ulArg6);

}

VOID cpss_fs_ntohl_cmd_rsp(CPS__TNBS_FTP_COMMAND_RSP_MSG_T *ptCmdRsp)
{
	if (ptCmdRsp == NULL)
		return;

	ptCmdRsp->lResult = cpss_ntohl(ptCmdRsp->lResult );
	ptCmdRsp->ulCmdId = cpss_ntohl(ptCmdRsp->ulCmdId);
	ptCmdRsp->ulReplyStringLength = cpss_ntohl(ptCmdRsp->ulReplyStringLength );

}

/*******************************************************************************
 * 函数名称: cpss_fs_byteflow_print
 * 功    能: 打印字符流函数
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回: 无
 * 说   明:
 *******************************************************************************/
VOID cpss_fs_byteflow_print(UINT8 *pucBuf, UINT32 ulLen)
{
	UINT32 ulLoop;

	if (g_ulCpssFsBytesPrint == 0)
	{
		return;
	}

	printf("<*_*> <*_*> <*_*> CPSS BYTE FLOW PRINT Begin: <*_*> <*_*> <*_*>\n");

	for (ulLoop = 0; ulLoop < ulLen; ulLoop++)
	{
		if (pucBuf[ulLoop] > 0xf)
		{
			printf("0x%x ", pucBuf[ulLoop]);
		}
		else
		{
			printf("0x0%x ", pucBuf[ulLoop]);
		}
		if ((ulLoop + 1) % 16 == 0)
		{
			printf("\n");
		}
	}
	printf("\n");
	printf("<*_*> <*_*> <*_*>CPSS BYTE FLOW PRINT End<*_*> <*_*> <*_*>\n");
}

/*******************************************************************************
 * 函数名称: cpss_fs_linux_filename_make
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 *******************************************************************************/
VOID cpss_fs_linux_filename_make(UINT8* pucFileName)
{

	UINT8* pucBufPtr;

	pucBufPtr = pucFileName;

	while (*pucBufPtr != '\0')
	{
		if (*pucBufPtr == '\\')
		{
			*pucBufPtr = '/';
		}
		pucBufPtr++;
	}
}

/*******************************************************************************
 * 函数名称: cpss_ftp_tmp_dirname_make
 * 功    能: 构造一个临时文件名
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * szDirName           STRING          输入              原始文件名
 * ulMaxDirLen         UINT32          输入              最大的目录名长度
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明: 
 *******************************************************************************/
INT32 cpss_ftp_tmp_dirname_make(STRING szDirName, UINT32 ulMaxDirLen)
{
	UINT32 ulStrLen = 0;
	/*目录名长度是否合法*/
	ulStrLen = strlen(szDirName) + 4;
	if (ulStrLen > ulMaxDirLen)
	{
		cpss_output(CPSS_MODULE_FS, CPSS_PRINT_WARN,
				"(%s) length is overflow.", szDirName);
		return CPSS_ERR_FS_NAME_LEN_OVERFLOW;
	}

	/*组成临时目录名*/
	strcpy(szDirName + strlen(szDirName), ".tmp");
	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_dirname_recover
 * 功    能: 恢复目录名
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * szDirName           STRING *        输入              目录指针
 * 函数返回:
 *           VOID
 * 说   明: 去掉目录名的后缀(.tmp)
 *******************************************************************************/
VOID cpss_ftp_dirname_recover(STRING szDirName)
{
	szDirName[strlen(szDirName) - 4] = '\0';
}

/*******************************************************************************
 * 函数名称: cpss_ftp_file_pack
 * 功    能: 压缩需要上传的文件
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_file_pack(UINT8* pucPackFileName, BOOL bCompress,
		UINT32 ulDstPid, UINT32 ulStepLen, UINT32 ulSerialId)
{
	INT32 lRet;
	printf("involking function which commited ");
#if 	0 
	UINT32 ulSizeHigh = 0;
	UINT32 ulSizeLow = 0;
	UINT8 *pucUsrBuf = NULL;
	UINT8 aucDstFilePath[CPSS_FS_FILE_ABSPATH_LEN];
	UINT8 aucFilePath[CPSS_FS_FILE_ABSPATH_LEN];
	CPSS_FTP_PROGRESS_INFO_T stFtpProgressInfo;

	strncpy(aucFilePath,pucPackFileName,CPSS_FS_FILE_ABSPATH_LEN);
	strncpy(aucDstFilePath,pucPackFileName,CPSS_FS_FILE_ABSPATH_LEN);

	stFtpProgressInfo.ulPid = ulDstPid;
	stFtpProgressInfo.ulStep = ulStepLen;
	stFtpProgressInfo.ulSerialID = ulSerialId;
	stFtpProgressInfo.ulState = CPSS_FTP_STATE_PACKING;

	if(ulStepLen == 0)
	{
		stFtpProgressInfo.ulSendFlag = CPSS_FTP_PROGRESS_NOT_SEND_FLAG;
	}
	else
	{
		stFtpProgressInfo.ulSendFlag = CPSS_FTP_PROGRESS_SEND_FLAG;
	}

	/*计算压缩目录的大小*/
	lRet = cpss_dir_used_space_get(aucFilePath,&ulSizeHigh,&ulSizeLow);
	if(CPSS_OK != lRet)
	{
		return lRet;
	}

	if(ulSizeHigh != 0)
	{
		return CPSS_ERR_FS_SIZE_TOO_LARGE;
	}

	stFtpProgressInfo.ulTotalSize = ulSizeLow;

	/*得到用户数据换存区*/
	pucUsrBuf = cpss_vk_proc_user_data_get();
	if(NULL != pucUsrBuf)
	{
		/*拷贝数据*/
		cpss_mem_memcpy(pucUsrBuf,&stFtpProgressInfo,sizeof(CPSS_FTP_PROGRESS_INFO_T));
	}

	/*生成压缩后的文件名称*/
	lRet = cpss_fs_suffix_add(aucDstFilePath,CPSS_FS_PACK_SUFFIX);
	if(CPSS_OK != lRet)
	{
		return CPSS_ERR_FS_SUFFIX_ADD_FAIL;
	}

	/*压缩文件*/
	lRet = cpss_file_pack(pucPackFileName,aucDstFilePath,bCompress);
	if(CPSS_OK != lRet)
	{
		return lRet;
	}

	/*修改传入的参数*/
	lRet = cpss_fs_suffix_add(pucPackFileName,CPSS_FS_PACK_SUFFIX);
	if(CPSS_OK != lRet)
	{
		return CPSS_ERR_FS_SUFFIX_ADD_FAIL;
	}
#endif
	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_file_unpack
 * 功    能: 解包下载的文件
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_file_unpack(UINT8* pucZipFilePath, UINT32 ulZipFileSize)
{
	printf(
			"involking function cpss_ftp_file_unpack which commited*********************************");
#if 	0
	INT32 lRet;
	UINT8 aucZipFilePath[CPSS_FS_FILE_ABSPATH_LEN];
	UINT8 aucDirPath[CPSS_FS_FILE_ABSPATH_LEN];
	CPSS_FTP_PROGRESS_INFO_T* pstFtpProgressInfo;

	strncpy(aucZipFilePath,pucZipFilePath,CPSS_FS_FILE_ABSPATH_LEN);
	strncpy(aucDirPath,pucZipFilePath,CPSS_FS_FILE_ABSPATH_LEN);

	/*取得目录路径*/
	lRet = cpss_fs_dirpath_unlink(aucDirPath);
	if(CPSS_OK != lRet)
	{
		return CPSS_ERR_FS_UNLINK_NAME_FAIL;
	}

	/*得到用户数据换存区*/
	pstFtpProgressInfo = (CPSS_FTP_PROGRESS_INFO_T*)cpss_vk_proc_user_data_get();
	if(NULL != pstFtpProgressInfo)
	{
		/*设置FTP进度指示信息*/
		pstFtpProgressInfo->ulState = CPSS_FTP_STATE_PACKING;
		pstFtpProgressInfo->ulTotalSize = ulZipFileSize;
	}

	/*调用解压函数进行解压*/
	lRet = cpss_file_unpack(aucZipFilePath,aucDirPath);
	if(CPSS_OK != lRet)
	{
		return lRet;
	}
#endif
	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_progress_ind_send
 * 功    能: FTP进度指示消息的发送函数
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_progress_ind_send(UINT32 ulBeginTick, UINT32 ulCurrFileLen,
		UINT32* pulFileLenStep)
{
	CPSS_FTP_PROGRESS_IND_MSG_T stFtpProgressIndMsg;
	CPSS_FTP_PROGRESS_INFO_T *pstFtpProgressInfo;
	CPSS_COM_PID_T stDstPid;
	UINT32 ulCurrTick = 0;
	UINT32 ulTickInterval = 0;
	UINT32 ulStep;
	INT32 lRet;

	pstFtpProgressInfo
			= (CPSS_FTP_PROGRESS_INFO_T*) cpss_vk_proc_user_data_get();
	if (NULL == pstFtpProgressInfo)
	{
		return CPSS_OK;
	}

	/*如果不需要发送进度指示直接返回成功*/
	if (pstFtpProgressInfo->ulSendFlag == CPSS_FTP_PROGRESS_NOT_SEND_FLAG)
	{
		return CPSS_OK;
	}

	ulStep = pstFtpProgressInfo->ulStep;

	/*不需要发送进度指示消息*/
	if (ulStep > *pulFileLenStep)
	{
		return CPSS_OK;
	}
	else
	{
		*pulFileLenStep = 0;
	}

	/*计算时间间隔*/
	ulCurrTick = cpss_tick_get();

	if (ulCurrTick < ulBeginTick)
	{
		ulTickInterval = 0xFFFFFFFF - ulCurrTick + ulBeginTick;
	}
	else
	{
		ulTickInterval = ulCurrTick - ulBeginTick;
	}

	cpss_com_logic_addr_get(&stDstPid.stLogicAddr, &stDstPid.ulAddrFlag);

	stFtpProgressIndMsg.ulUsedTime = ulTickInterval;
	stFtpProgressIndMsg.ulDealingSize = ulCurrFileLen;

	stFtpProgressIndMsg.ulSerialID = pstFtpProgressInfo->ulSerialID;
	stFtpProgressIndMsg.ulState = pstFtpProgressInfo->ulState;
	stFtpProgressIndMsg.ulTotalSize = pstFtpProgressInfo->ulTotalSize;

	stDstPid.ulPd = pstFtpProgressInfo->ulPid;

	/*向源纤程发送进度指示消息*/
	lRet = cpss_com_send(&stDstPid, CPSS_FTP_PROGRESS_IND_MSG,
			(UINT8*) &stFtpProgressIndMsg, sizeof(CPSS_FTP_PROGRESS_IND_MSG_T));
	if (lRet != CPSS_OK)
	{
		cpss_output(CPSS_MODULE_FS, CPSS_PRINT_WARN,
				"send ftp progress ind msg failed(%d)(%d)(%d).",
				stDstPid.stLogicAddr, stDstPid.ulPd, stDstPid.ulAddrFlag);
	}

	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_fs_ftp_analyze_file_len
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFTPReqMsg  CPSS_FTP_REQ_MSG_T*   输入              FTP服务请求消息结构指针
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_ftp_analyze_file_len(UINT8* pucBuf, UINT32 ulBufLen,
		UINT32* pulFileLen)
{
	UINT8 aucFileLen[10];
	UINT8 *pucBufPtr;
	UINT32 ulLoopI;
	UINT32 ulFileLen = 0;
	UINT32 ulTemSum = 0;
	UINT32 ulLoopJ;

	/*打印码流*/
	cpss_fs_byteflow_print(pucBuf, 100);

	pucBufPtr = pucBuf;

	/*如果是目录返回固定的长度*/
	if (*pucBufPtr == 'd')
	{
		*pulFileLen = 10000;
		return CPSS_ERROR;
	}

	/*找到标明文件长度开始的指针*/
	pucBufPtr = pucBufPtr + 14;

	/*过滤用户名前的空格*/
	while (*pucBufPtr == 0x20)
	{
		pucBufPtr++;
	}

	/*过滤用户名*/
	while (*pucBufPtr != 0x20)
	{
		pucBufPtr++;
	}

	/*过滤用户名和组名之间的空格*/
	while (*pucBufPtr == 0x20)
	{
		pucBufPtr++;
	}

	/*过滤组名*/
	while (*pucBufPtr != 0x20)
	{
		pucBufPtr++;
	}

	strncpy(aucFileLen, pucBufPtr, 10);

	for (ulLoopI = 0; ulLoopI < 9; ulLoopI++)
	{
		if (*(aucFileLen + ulLoopI) == 0x20)
		{
			continue;
		}
		else
		{
			ulTemSum = 1;
			for (ulLoopJ = 0; ulLoopJ < 8 - ulLoopI; ulLoopJ++)
			{
				ulTemSum = ulTemSum * 10;
			}

			ulFileLen = ulFileLen + (*(aucFileLen + ulLoopI) - 0x30) * ulTemSum;
		}
	}

	*pulFileLen = ulFileLen;

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_fs_ftp_ind_msg_set
 * 功    能: 设置文件传输过程中用户缓冲区中的数据
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFTPReqMsg  CPSS_FTP_REQ_MSG_T*   输入              FTP服务请求消息结构指针
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
#ifdef CPSS_VOS_VXWORKS
INT32 cpss_ftp_server_file_len_get
(
		UINT8* pucSerIp,
		UINT8* pucUserName,
		UINT8* pucPasswd,
		UINT8* pucSerDirName,
		UINT8* pucFileName,
		UINT32* pulFileLen
)
{
	INT32 lCtrlSock = 0;
	INT32 lDataSock = 0;
	INT32 lFtpXfer = 0;
	INT32 lStatus = CPSS_OK;

	INT32 lBytes = 0;
	INT32 lReadBytesSum = 0;
	INT32 lFtpReplyGet;
	UINT8 *pucRecvInfo;

	pucRecvInfo = cpss_mem_malloc(CPSS_FTP_FILE_LIST_LEN_MAX);
	if(NULL == pucRecvInfo)
	{
		return CPSS_ERROR;
	}

	/*启动FTP的连接过程*/
	lFtpXfer = ftpXfer(pucSerIp, pucUserName,pucPasswd,
			"","LIST %s",pucSerDirName,pucFileName, &lCtrlSock, &lDataSock);
	if ( ERROR== lFtpXfer)
	{
		cpss_mem_free(pucRecvInfo);
		return cpss_fs_errno_get();
	}

	while((lBytes=read(lDataSock,pucRecvInfo+lReadBytesSum,CPSS_FTP_RECV_SIZE))>0)
	{
		lReadBytesSum = lReadBytesSum + lBytes;
		if(lReadBytesSum >= (CPSS_FTP_FILE_LIST_LEN_MAX - 3*CPSS_FTP_RECV_SIZE))
		{
			cpss_mem_free(pucRecvInfo);
			lStatus = CPSS_ERROR;
			break;
		}
	}

	/*读过程是否有错*/
	if((lBytes == ERROR)&&(lStatus != CPSS_ERROR))
	{
		cpss_mem_free(pucRecvInfo);
		lStatus = CPSS_ERROR;
	}

	/*检查传输过程中是否有错*/
	lFtpReplyGet = ftpReplyGet(lCtrlSock, TRUE);
	if ((lFtpReplyGet != FTP_COMPLETE)&&(lStatus != CPSS_ERROR))
	{
		cpss_mem_free(pucRecvInfo);
		lStatus = CPSS_ERROR;
	}

	/*释放所有资源*/
	ftpCommand (lCtrlSock, "QUIT", 0, 0, 0, 0, 0, 0);

	close(lDataSock);
	close(lCtrlSock);

	if(CPSS_ERROR == lStatus)
	{
		*pulFileLen = 100000000; /*如果服务器不能返回文件的长度，则置文件为默认长度100000000*/
		return CPSS_ERROR;
	}

	/*对得到的数据进行处理*/
	cpss_fs_ftp_analyze_file_len(pucRecvInfo,lReadBytesSum,pulFileLen);

	cpss_mem_free(pucRecvInfo);

	return CPSS_OK;
}
#elif defined CPSS_VOS_WINDOWS
INT32 cpss_ftp_server_file_len_get
(
		INT32 lCtrlSock,
		INT32 lDataSock,
		UINT32* pulFileLen,
		UINT8* pulServerFile
)
{
	WIN32_FIND_DATA stFileInfo =
	{	0};
	UINT8 aucServerFilePath[CPSS_FS_FILE_ABSPATH_LEN];

	strncpy(aucServerFilePath,pulServerFile,CPSS_FS_FILE_ABSPATH_LEN);
#if 0
	if(NULL == FtpFindFirstFile((HINTERNET)lDataSock,aucServerFilePath,&stFileInfo,0,0))
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"call FtpFindFirstFile failed.%s.errno=%d\n",aucServerFilePath,GetLastError());
		return CPSS_ERROR;
	}

	if(stFileInfo.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
	{
		return CPSS_ERROR;
	}

	if(stFileInfo.nFileSizeHigh != 0)
	{
		return CPSS_ERROR;
	}

	*pulFileLen = stFileInfo.nFileSizeLow;
#endif

	*pulFileLen = 100000000; /*如果在windows环境下默认置文件长度为100000000*/

	return CPSS_OK;
}
#endif

/*******************************************************************************
 * 函数名称: cpss_fs_ftp_ind_msg_set
 * 功    能: 设置文件传输过程中用户缓冲区中的数据
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFTPReqMsg  CPSS_FTP_REQ_MSG_T*   输入              FTP服务请求消息结构指针
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_ftp_ind_msg_set(UINT32 ulSerialId, UINT32 ulProgressStep,
		UINT32 ulFtpCmd, UINT8* pucFilePath, UINT32 ulDstPid, UINT32 ulFileSize)
{
	INT32 lRet;
	CPSS_FTP_PROGRESS_INFO_T* pstFtpProgressInfo;
	UINT8 aucFilePathName[CPSS_FS_FILE_ABSPATH_LEN];
	UINT32 ulFileLen;

	pstFtpProgressInfo
			= (CPSS_FTP_PROGRESS_INFO_T*) cpss_vk_proc_user_data_get();
	if (NULL == pstFtpProgressInfo)
	{
		return CPSS_OK;
	}

	if (ulProgressStep == 0)
	{
		pstFtpProgressInfo->ulSendFlag = CPSS_FTP_PROGRESS_NOT_SEND_FLAG;
		return CPSS_OK;
	}
	else
	{
		pstFtpProgressInfo->ulSendFlag = CPSS_FTP_PROGRESS_SEND_FLAG;
	}

	/*如果是上传文件*/
	if ((CPSS_FTP_CMD_PUT_FILE == ulFtpCmd) || (CPSS_FTP_CMD_PUT_DIR_PACK
			== ulFtpCmd))
	{
		strncpy(aucFilePathName, pucFilePath, CPSS_FS_FILE_ABSPATH_LEN);

		/*得到文件的大小*/
		lRet = cpss_file_get_size(aucFilePathName, &ulFileLen);
		if (CPSS_OK != lRet)
		{
			return CPSS_ERROR;
		}
	}
	else
	{
		ulFileLen = ulFileSize;
	}

	pstFtpProgressInfo->ulPid = ulDstPid;
	pstFtpProgressInfo->ulSerialID = ulSerialId;
	pstFtpProgressInfo->ulStep = ulProgressStep;
	pstFtpProgressInfo->ulTotalSize = ulFileLen;
	pstFtpProgressInfo->ulState = CPSS_FTP_STATE_TRANSFERRING;

	return CPSS_OK;
}

#ifdef CPSS_VOS_VXWORKS

/*******************************************************************************
 * 函数名称: cpss_tftp_dir_add
 * 功    能: 增加TFTP服务器的服务目录
 * 函数类型:
 *
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pacDirName          CHAR*           输入              增加的服务目录
 * 返回值:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_tftp_dir_add
(
		CHAR *pacDirName
)
{
	CHAR acDirName[CPSS_FS_FILE_ABSPATH_LEN];
	BOOL bIsDir = FALSE;
	INT32 lFileNameChk = 0;
	INT32 lFileIsDir = 0;
	STATUS lTftpDirAdd = 0;

	strncpy(acDirName,pacDirName, CPSS_FS_FILE_ABSPATH_LEN);

	lFileNameChk = cpss_filename_check(acDirName,TRUE);
	if( lFileNameChk != CPSS_OK )
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"dir (%s) is illegal.",acDirName);
		return CPSS_ERR_FS_FILENAME_ILLEGAL;
	}

	lFileIsDir = cpss_file_is_dir(acDirName,&bIsDir);
	if( lFileIsDir != CPSS_OK)
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"CPSS_TFTP:(%s) is dir?failed.",acDirName);
		return lFileIsDir;
	}

	if( bIsDir == TRUE )
	{
		lTftpDirAdd = tftpdDirectoryAdd(acDirName);
		if( lTftpDirAdd != OK )
		{
			cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
					"CPSS_TFTP:add dir(%s)failed.",acDirName);
			return cpss_fs_errno_get();
		}
		return CPSS_OK;
	}
	else
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"CPSS_TFTP:(%s)is not dir.",acDirName);
		return CPSS_ERROR;
	}
}

/*******************************************************************************
 * 函数名称: cpss_tftp_dir_rmv
 * 功    能: 删除tftp服务器的服务目录
 * 函数类型:
 *
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pacDirName          CHAR*           输入              要删除的服务器目录
 * 返回值:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_tftp_dir_rmv
(
		CHAR *pacDirName
)
{
	CHAR acDirName[ CPSS_FS_FILE_ABSPATH_LEN];
	BOOL bIsDir = FALSE;
	INT32 lFileNameChk = 0;
	INT32 lFileIsDir = 0;
	STATUS lTftpDirRemov = 0;

	strncpy(acDirName,pacDirName, CPSS_FS_FILE_ABSPATH_LEN);

	lFileNameChk = cpss_filename_check(acDirName,TRUE);
	if( lFileNameChk != CPSS_OK )
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"dir (%s) is illegal.",acDirName);
		return CPSS_ERROR;
	}

	lFileIsDir = cpss_file_is_dir(acDirName,&bIsDir);
	if( lFileIsDir != CPSS_OK)
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"CPSS_TFTP:(%s) is dir?failed.",acDirName);
		return CPSS_ERROR;
	}

	if( bIsDir == TRUE )
	{
		lTftpDirRemov = tftpdDirectoryRemove(acDirName);
		if( lTftpDirRemov != OK )
		{
			cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
					"CPSS_TFTP:rmv dir(%s)failed.",acDirName);
			return CPSS_ERROR;
		}
		return CPSS_OK;
	}
	else
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"CPSS_TFTP:(%s)is not dir.",acDirName);
		return CPSS_ERROR;
	}
}

/*******************************************************************************
 * 函数名称: cpss_fs_tftpd
 * 功    能: 启动tftp服务器
 * 函数类型:
 *
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * lDirNum             INT32           输入              服务器文件目录的数目
 * ppacDirNames        CHAR**          输入              服务器文件目录的指针列表
 * 返回值:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_tftp_server_init
(
		INT32 lDirNum,
		CHAR** ppacDirNames
)
{
	INT32 lCount=lDirNum;
	BOOL bFileExist = FALSE;
	INT32 lDirCreate = 0;
	STATUS lTftpInit = 0;

	/*建立TFTP的目录*/
	while (--lCount >= 0)
	{
		bFileExist = cpss_file_exist(ppacDirNames[lCount]);
		if( bFileExist == TRUE)
		{
			printf("\n exist (%s)\n",ppacDirNames[lCount]);
			continue;
		}
		else
		{
			lDirCreate = cpss_dir_create(ppacDirNames[lCount]);
			if( lDirCreate != CPSS_OK )
			{
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
						"create directory failed.");
				return CPSS_ERROR;
			}
		}
	}

	lTftpInit = tftpdInit(0,lDirNum,ppacDirNames,TRUE,0);
	/*启动TFTP服务器任务*/
	if( ERROR== lTftpInit )
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"sprawn tftp server task failed.");
		return CPSS_ERROR;
	}

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_connect
 * 功    能: 连接FTP服务器
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFTPConnect  CPSS_FTP_CONNECT_T*  输入              FTP连接操作的结构指针
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_connect
(
		CPSS_FTP_CONNECT_T* pstFTPConnect,
		UINT32* pucFileLen
)
{
	INT8 acFileName[CPSS_FS_FILENAME_LEN];
	INT8 acFTPCmd[CPSS_FTP_CMD_LEN+1];
	INT8 acServerFileName[CPSS_FS_FILE_ABSPATH_LEN];
	INT32 lFileNameChkClt = 0;
	INT32 lFileNameChkSvr = 0;
	INT32 lTmpDirMake = 0;
	INT32 lGetFileName = 0;
	INT32 lDirPathUnlink = 0;
	INT32 lFtpTmpDirMake = 0;
	STATUS lFtpXfer = 0;
	BOOL bIsDir = FALSE;
	INT32 lRet;

	if(!((pstFTPConnect->ulLen > 0)&&(pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_FILE)))
	{
		lFileNameChkClt = cpss_filename_check(pstFTPConnect->szClientFileName,TRUE);
	}
	lFileNameChkSvr = cpss_filename_check(pstFTPConnect->szServerFileName,FALSE);
	if((lFileNameChkClt != CPSS_OK)|| (lFileNameChkSvr != CPSS_OK))
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"(%s) and (%s) is illegal.",pstFTPConnect->szClientFileName,pstFTPConnect->szServerFileName);
		return CPSS_ERR_FS_FILENAME_ILLEGAL;
	}

	if((pstFTPConnect->ulCmd == CPSS_FTP_CMD_PUT_FILE)||(pstFTPConnect->ulCmd == CPSS_FTP_CMD_PUT_DIR_PACK))
	{
		/*.tmp server name*/
		lTmpDirMake = cpss_ftp_tmp_dirname_make(pstFTPConnect->szServerFileName, CPSS_FS_FILE_ABSPATH_LEN);
		if( lTmpDirMake != CPSS_OK)
		{
			cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
					"make .tmp name failed(%s)",pstFTPConnect->szServerFileName);
			return CPSS_ERR_FS_TMP_NAME_MAKE_FAIL;
		}
	}
	strncpy(acServerFileName,pstFTPConnect->szServerFileName, CPSS_FS_FILE_ABSPATH_LEN);

	/*得到文件名*/
	lGetFileName = cpss_fs_get_file_name(acServerFileName,&acFileName[0]);
	if( lGetFileName != CPSS_OK)
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"get filename (%s) failed.",acServerFileName);
		return CPSS_ERR_FS_FILENAME_GET_FAIL;
	}

	/*得到文件的目录名*/
	lDirPathUnlink = cpss_fs_dirpath_unlink(acServerFileName);
	if( lDirPathUnlink != CPSS_OK)
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"get unlink dirname (%s) failed.",acServerFileName);
		return CPSS_ERR_FS_UNLINK_NAME_FAIL;
	}

	/*如果应用要求下载文件*/
	if((pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_FILE)||(pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_DIR_PACK))
	{
		/*得到文件的长度*/
		cpss_ftp_server_file_len_get(pstFTPConnect->szServerIP,pstFTPConnect->szUserName,
				pstFTPConnect->szPasswd,acServerFileName,acFileName,pucFileLen);

		strcpy(acFTPCmd,CPSS_FTP_GET_STR);
		if((pstFTPConnect->ulLen == 0)||(pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_DIR_PACK))
		{
			/*如果应用要求下载文件*/
			cpss_file_is_dir(pstFTPConnect->szClientFileName,&bIsDir);
			if(TRUE == bIsDir)
			{
				/*组织目录名+文件名*/
				lRet =cpss_fs_dirpath_link(pstFTPConnect->szClientFileName,acFileName,CPSS_FS_FILE_ABSPATH_LEN);
				if(CPSS_OK != lRet)
				{
					cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,"ftp make client path failed.(%s)(%s)\n",
							pstFTPConnect->szClientFileName,acFileName);
					return CPSS_ERR_FS_LINK_NAME_FAIL;
				}
			}

			/*在同样的目录下构造一个在源文件名后加上.tmp后缀的临时文件名*/
			lFtpTmpDirMake = cpss_ftp_tmp_dirname_make(pstFTPConnect->szClientFileName, CPSS_FS_FILE_ABSPATH_LEN);
			if( lFtpTmpDirMake != CPSS_OK )
			{
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
						"make .tmp name failed pstFTPConnect->szClientFileName(%s)",
						pstFTPConnect->szClientFileName);
				return CPSS_ERR_FS_TMP_NAME_MAKE_FAIL;
			}
			/*以写方式打开文件*/
			pstFTPConnect->ulFileHandle = cpss_file_open(pstFTPConnect->szClientFileName,
					CPSS_FILE_ORDWR|CPSS_FILE_OCREATE|CPSS_FILE_OTRUNC|CPSS_FILE_OBINARY);
			if(pstFTPConnect->ulFileHandle == CPSS_FD_INVALID)
			{
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
						"open (%s) failed,errno=(%d).",pstFTPConnect->szClientFileName,errno);
				return CPSS_ERR_FS_OPEN_FILE_FAIL;
			}
		}
	}
	/*如果上传文件*/
	else if((pstFTPConnect->ulCmd == CPSS_FTP_CMD_PUT_FILE)||(pstFTPConnect->ulCmd == CPSS_FTP_CMD_PUT_DIR_PACK))
	{
		strcpy(acFTPCmd,CPSS_FTP_PUT_STR);
		/*以读方式打开文件*/
		pstFTPConnect->ulFileHandle = cpss_file_open(pstFTPConnect->szClientFileName,
				CPSS_FILE_ORDONLY|CPSS_FILE_OBINARY);
		if(pstFTPConnect->ulFileHandle == CPSS_FD_INVALID)
		{
			cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
					"open (%s) failed,errno=(%d).",pstFTPConnect->szClientFileName,errno);
			return CPSS_ERR_FS_OPEN_FILE_FAIL;
		}
	}

	/*启动FTP的连接过程*/
	lFtpXfer = ftpXfer(pstFTPConnect->szServerIP, pstFTPConnect->szUserName,pstFTPConnect->szPasswd,
			"",acFTPCmd,acServerFileName,acFileName, &(pstFTPConnect->lCtrlSock),
			&(pstFTPConnect->lDataSock));
	if ( ERROR== lFtpXfer)
	{
		if( !((pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_FILE)&&(pstFTPConnect->ulLen > 0)) )
		{
			cpss_file_close(pstFTPConnect->ulFileHandle);
		}

		if(((pstFTPConnect->ulLen == 0)&&(pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_FILE))
				||(pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_DIR_PACK))
		{
			cpss_file_delete(pstFTPConnect->szClientFileName);
		}

		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"ftpXfer failed serverFileName=%s,clientFileName=%s,usrName=%s,passWd=%s,ip=%s,ulCmd=%d,ulLen=%d,errno=%d",
				pstFTPConnect->szServerFileName,pstFTPConnect->szClientFileName,pstFTPConnect->szUserName,
				pstFTPConnect->szPasswd,pstFTPConnect->szServerIP,pstFTPConnect->ulCmd,pstFTPConnect->ulLen,errno);

		return cpss_fs_errno_get();
	}

	/*根据不同的服务类型对读写描述符赋值*/
	if((pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_FILE)||
			(pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_DIR_PACK))
	{
		pstFTPConnect->lWriteHandle = pstFTPConnect->ulFileHandle;
		pstFTPConnect->lReadHandle = pstFTPConnect->lDataSock;
	}
	else if((pstFTPConnect->ulCmd == CPSS_FTP_CMD_PUT_FILE)||
			(pstFTPConnect->ulCmd == CPSS_FTP_CMD_PUT_DIR_PACK))
	{
		pstFTPConnect->lWriteHandle = pstFTPConnect->lDataSock;
		pstFTPConnect->lReadHandle = pstFTPConnect->ulFileHandle;
	}

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_connect
 * 功    能: 连接TFTP服务器
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFTPConnect  CPSS_FTP_CONNECT_T*  输入              文件传输连接操作的结构指针
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_tftp_connect
(
		CPSS_FTP_CONNECT_T* pstFTPConnect
)
{
	INT8 acTFTPCmd[CPSS_TFTP_CMD_LEN+1];
	INT32 lFileNameChkClt = 0;
	INT32 lFileNameChkSvr = 0;
	INT32 lFtpTmpDirMake = 0;
	STATUS lTftpXfer = 0;

	if(pstFTPConnect->szMemBuf == 0)
	{
		lFileNameChkClt = cpss_filename_check(pstFTPConnect->szClientFileName,TRUE);
	}
	lFileNameChkSvr = cpss_filename_check(pstFTPConnect->szServerFileName,FALSE);
	if(( lFileNameChkClt != CPSS_OK)||( lFileNameChkSvr != CPSS_OK))
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"(%s) and (%s) is illegal.",pstFTPConnect->szClientFileName,pstFTPConnect->szServerFileName);
		return CPSS_ERR_FS_FILENAME_ILLEGAL;
	}

	/*如果是文件下载过程*/
	if(pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_FILE)
	{
		strcpy(acTFTPCmd,CPSS_TFTP_GET_STR);
		if(pstFTPConnect->szMemBuf == NULL)
		{
			/*在同样的目录下构造一个在源文件名后加上.tmp后缀的临时文件名*/
			lFtpTmpDirMake = cpss_ftp_tmp_dirname_make(pstFTPConnect->szClientFileName, CPSS_FS_FILE_ABSPATH_LEN);
			if( lFtpTmpDirMake != CPSS_OK )
			{
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
						"make .tmp name failed(%s)",pstFTPConnect->szClientFileName);
				return CPSS_ERR_FS_TMP_NAME_MAKE_FAIL;
			}

			/*以写方式打开文件*/
			pstFTPConnect->ulFileHandle = cpss_file_open(pstFTPConnect->szClientFileName,
					CPSS_FILE_ORDWR|CPSS_FILE_OCREATE|CPSS_FILE_OTRUNC|CPSS_FILE_OBINARY);
			if(pstFTPConnect->ulFileHandle == CPSS_FD_INVALID)
			{
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
						"open (%s) failed,errno=%d",pstFTPConnect->szClientFileName,errno);
				return CPSS_ERR_FS_OPEN_FILE_FAIL;
			}
		}
	}
	/*如果上传文件*/
	else if(pstFTPConnect->ulCmd == CPSS_FTP_CMD_PUT_FILE)
	{
		strcpy(acTFTPCmd,CPSS_TFTP_PUT_STR);

		/*以读方式打开文件*/
		pstFTPConnect->ulFileHandle = cpss_file_open(pstFTPConnect->szClientFileName,
				CPSS_FILE_ORDONLY|CPSS_FILE_OBINARY);
		if(pstFTPConnect->ulFileHandle == CPSS_FD_INVALID)
		{
			cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
					"open (%s) failed,errno=%d",pstFTPConnect->szClientFileName,errno);
			return CPSS_ERR_FS_OPEN_FILE_FAIL;
		}
	}

	pstFTPConnect->lDataSock = 0;
	pstFTPConnect->lCtrlSock = 0;
	/*启动FTP的连接过程*/
	lTftpXfer = tftpXfer(pstFTPConnect->szServerIP, 0, pstFTPConnect->szServerFileName, acTFTPCmd,
			"binary", &pstFTPConnect->lDataSock, &pstFTPConnect->lCtrlSock);
	if ( ERROR == lTftpXfer)
	{
		if( !((pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_FILE)&&(pstFTPConnect->szMemBuf != NULL)) )
		{
			cpss_file_close(pstFTPConnect->ulFileHandle);
		}

		if((pstFTPConnect->szMemBuf == NULL)&&(pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_FILE))
		{
			cpss_file_delete(pstFTPConnect->szClientFileName);
		}

		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"tftpXfer failed serverFileName=%s,clientFileName=%s,ip=%s,ulCmd=%d,ulLen=%d,errno=%d",
				pstFTPConnect->szServerFileName,pstFTPConnect->szClientFileName,pstFTPConnect->szServerIP,
				pstFTPConnect->ulCmd,pstFTPConnect->ulLen,errno);
		return CPSS_ERR_TFTP_CONN_FAIL;
	}

	/*根据不同的服务类型对读写描述符赋值*/
	if(pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_FILE)
	{
		pstFTPConnect->lWriteHandle = pstFTPConnect->ulFileHandle;
		pstFTPConnect->lReadHandle = pstFTPConnect->lDataSock;
	}
	else if(pstFTPConnect->ulCmd == CPSS_FTP_CMD_PUT_FILE)
	{
		pstFTPConnect->lWriteHandle = pstFTPConnect->lDataSock;
		pstFTPConnect->lReadHandle = pstFTPConnect->ulFileHandle;
	}

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_fs_ftp
 * 功    能: 提供FTP服务
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFTPReqMsg  CPSS_FTP_REQ_MSG_T*   输入              FTP服务请求消息结构指针
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_ftp
(
		CPSS_FTP_REQ_MSG_T* pstFTPReqMsg,
		UINT32 ulDstPid
)
{
	INT32 lBytes = 0;
	INT32 lBlockRead = 0;
	INT8 acBuff[CPSS_FTP_RECV_SIZE+1];
	INT8 acClientFileName[ CPSS_FS_FILE_ABSPATH_LEN];
	INT8 acServerFileName[ CPSS_FS_FILE_ABSPATH_LEN];
	INT8 acServerIp[20];
	INT32 lStatus = CPSS_OK;
	CPSS_FTP_CONNECT_T stFTPConnect;
	struct in_addr stAddr;
	INT32 lFtpConn = 0;
	INT32 lFtpReplyGet = 0;
	INT32 lRet;
	UINT32 ulBeginTick = 0;
	UINT32 ulCurrFtpLen = 0;
	UINT32 ulFtpStep = 0;
	UINT32 ulFileLen = 0;

	/*判断ulCmd是否正确*/
	if((pstFTPReqMsg->ulFtpCmd != CPSS_FTP_CMD_GET_FILE)&&(pstFTPReqMsg->ulFtpCmd != CPSS_FTP_CMD_GET_DIR_PACK)&&
			(pstFTPReqMsg->ulFtpCmd != CPSS_FTP_CMD_PUT_FILE)&&(pstFTPReqMsg->ulFtpCmd != CPSS_FTP_CMD_PUT_DIR_PACK))
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,"ulCmd=%d,is not correct",pstFTPReqMsg->ulFtpCmd);
		return CPSS_ERR_FS_PARA_ILLEGAL;
	}

	if(!((pstFTPReqMsg->ulLen > 0)&&(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_FILE)))
	{
		strncpy(acClientFileName, pstFTPReqMsg->aucClientFile, CPSS_FS_FILE_ABSPATH_LEN);
	}
	strncpy(acServerFileName, pstFTPReqMsg->aucServerFile, CPSS_FS_FILE_ABSPATH_LEN);

	stAddr.s_addr = cpss_htonl(pstFTPReqMsg->ulServerIP);
	inet_ntoa_b (stAddr, acServerIp);

	stFTPConnect.ulLen = pstFTPReqMsg->ulLen;
	stFTPConnect.ulCmd = pstFTPReqMsg->ulFtpCmd;
	stFTPConnect.szClientFileName = acClientFileName;
	stFTPConnect.szServerFileName = acServerFileName;
	stFTPConnect.szServerIP = acServerIp;
	stFTPConnect.szUserName = pstFTPReqMsg->aucUserName;
	stFTPConnect.szPasswd = pstFTPReqMsg->aucPasswd;
	stFTPConnect.szMemBuf = pstFTPReqMsg->pucMem;

	/*如果上传压缩文件,则先压缩指定文件,并修改服务器文件名称*/
	if(stFTPConnect.ulCmd == CPSS_FTP_CMD_PUT_DIR_PACK)
	{
		lRet = cpss_ftp_file_pack(acClientFileName,TRUE,
				ulDstPid,pstFTPReqMsg->ulProgessStep,pstFTPReqMsg->ulSerialID);
		if(CPSS_OK != lRet)
		{
			cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
					"ftp file pack failed (%s) failed.",acClientFileName);
			return CPSS_ERR_FS_PACK_FILE_FAIL;
		}
	}

	/*连接FTP服务器并对输入参数进行检查*/
	lFtpConn = cpss_ftp_connect(&stFTPConnect,&ulFileLen);
	if( lFtpConn != CPSS_OK )
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"connet ftp (%s) failed.",stFTPConnect.szServerIP);
		return lFtpConn;
	}

	ulBeginTick = cpss_tick_get();

	/*设置FTP的进度指示消息信息*/
	if(CPSS_OK != cpss_fs_ftp_ind_msg_set(pstFTPReqMsg->ulSerialID,
					pstFTPReqMsg->ulProgessStep,pstFTPReqMsg->ulFtpCmd,
					acClientFileName,ulDstPid,ulFileLen))
	{
		lStatus = CPSS_ERROR;
	}

	/*从FTP的描述符循环读取数据*/
	while ( (lStatus!=CPSS_ERROR)&&((lBytes=read(stFTPConnect.lReadHandle,acBuff,CPSS_FTP_RECV_SIZE))>0) )
	{
		/*如果是下载过程，且用户内存大于0，则将读到的数据存在用户传进行来的内存中。《此需求由资源管理提》*/
		if( (pstFTPReqMsg->ulLen > 0)&&(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_FILE) )
		{
			lBlockRead = lBlockRead + lBytes;
			if(lBlockRead > pstFTPReqMsg->ulLen)
			{
				lStatus = CPSS_ERROR;
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
						"save memory(%d) too small lBlockRead=%d.",stFTPConnect.ulLen,lBlockRead);
				break;
			}
			memcpy(pstFTPReqMsg->pucMem+lBlockRead-lBytes,acBuff,lBytes);
		}
		else
		{
			/*将数据写到写文件描述符中*/
			if(write(stFTPConnect.lWriteHandle,acBuff,lBytes)==ERROR)
			{
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
						"write (%s) len(%d) failed.",stFTPConnect.szClientFileName,lBytes);
				lStatus = CPSS_ERROR;
			}
		}

		/*发送FTP进度指示*/
		ulCurrFtpLen = ulCurrFtpLen + lBytes;
		ulFtpStep = ulFtpStep + lBytes;

		cpss_ftp_progress_ind_send(ulBeginTick,ulCurrFtpLen,&ulFtpStep);
	}

	/*读的过程出现错误*/
	if(lBytes == ERROR)
	{
		lStatus = CPSS_ERROR;
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"read (%s) failed.errno=(%d)",stFTPConnect.szClientFileName,errno);
	}

	if( !((pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_FILE)&&(pstFTPReqMsg->ulLen > 0)) )
	{
		cpss_file_close(stFTPConnect.ulFileHandle);
	}

	close (stFTPConnect.lDataSock);

	/*读到文件末尾,检查传输过程中是否有错*/
	lFtpReplyGet = ftpReplyGet(stFTPConnect.lCtrlSock, TRUE);
	if ( lFtpReplyGet != FTP_COMPLETE)
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"ftpReplyGet return failed(%d).errno=(%d)",lFtpReplyGet,errno);
		lStatus = CPSS_ERROR;
	}

	/*rename server file name*/
	if( ((pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_PUT_FILE)||
					(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_PUT_DIR_PACK))&&(lStatus!=CPSS_ERROR) )
	{

		/*删除本地.zip文件*/
		if(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_PUT_DIR_PACK)
		{
			cpss_file_delete(acClientFileName);
		}

		cpss_fs_get_file_name(acServerFileName,acClientFileName);

		strncpy(acServerFileName, acClientFileName, CPSS_FS_FILENAME_LEN);

		/*去掉文件名称结尾所添加的.tmp*/
		cpss_ftp_dirname_recover(acClientFileName);

		ftpCommand(stFTPConnect.lCtrlSock,"DELE %s",(INT32)acClientFileName,0,0,0,0,0);

		if( (ftpCommand(stFTPConnect.lCtrlSock,"RNFR %s",(INT32)acServerFileName,0,0,0,0,0) == FTP_ERROR )||
				(ftpCommand(stFTPConnect.lCtrlSock,"RNTO %s",(INT32)acClientFileName,0,0,0,0,0) == FTP_ERROR ) )
		{
			cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
					"ftp rename from %s to %s failed",acServerFileName,acClientFileName);
			lStatus = CPSS_ERROR;
		}
	}

	if (ftpCommand (stFTPConnect.lCtrlSock, "QUIT", 0, 0, 0, 0, 0, 0) != FTP_COMPLETE)
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"ftp quit failed serverFile=%s,ip=%s",stFTPConnect.szServerFileName,stFTPConnect.szServerIP);
		lStatus = CPSS_ERROR;
	}

	/*关闭所有的资源描述符*/
	close (stFTPConnect.lCtrlSock);

	/*如果状态为ERROR进行错误处理*/
	if(lStatus == CPSS_ERROR)
	{
		if( ((pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_FILE)&&(pstFTPReqMsg->ulLen == 0))||
				(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_DIR_PACK) )
		{
			cpss_file_delete(acClientFileName);
		}

		return CPSS_ERR_FTP_PROCESS_FAIL;
	}

	/*如果是下载过程，且文件存在本地硬盘，则将存储的临时文件重命名为用户规定的文件名  《此需求由资源管理提》*/
	if( ((pstFTPReqMsg->ulLen == 0)&&(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_FILE))
			||(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_DIR_PACK) )
	{
		strncpy(acServerFileName,acClientFileName, CPSS_FS_FILE_ABSPATH_LEN);

		/*去掉文件名称结尾所添加的.tmp*/
		cpss_ftp_dirname_recover(acServerFileName);

		/*if file exist,delete file*/
		if(cpss_file_exist(acServerFileName)==TRUE)
		{
			cpss_file_delete(acServerFileName);
		}

		if( rename(acClientFileName,acServerFileName) == ERROR )
		{
			cpss_file_delete(acClientFileName);
			cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
					"ftp client rename from %s to %s failed",acClientFileName,acServerFileName);
			return cpss_fs_errno_get();
		}

		if(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_DIR_PACK)
		{
			lRet = cpss_ftp_file_unpack(acServerFileName,ulCurrFtpLen);
			cpss_file_delete(acServerFileName);
			if(CPSS_OK != lRet)
			{
				return CPSS_ERR_UNPACK_FILE_FAIL;
			}
		}
	}
	/*FTP过程正确完成*/
	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_fs_tftp
 * 功    能: 提供TFTP服务
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFTPReqMsg  CPSS_TFTP_REQ_MSG_T*  输入              TFTP服务请求消息结构指针
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_tftp
(
		CPSS_TFTP_REQ_MSG_T* pstTFTPReqMsg
)
{
	INT32 lBytes = 0;
	INT32 lBlockRead = 0;
	INT8 acBuff[CPSS_TFTP_RECV_SIZE+1];
	INT8 acClientFileName[ CPSS_FS_FILE_ABSPATH_LEN];
	INT8 acServerFileName[ CPSS_FS_FILE_ABSPATH_LEN];
	INT32 lStatus = CPSS_OK;
	CPSS_FTP_CONNECT_T stTFTPConnect;
	INT8 acServerIp[20];
	struct in_addr stAddr;
	INT32 lTftpConn = 0;
	STATUS lWrite = 0;
	BOOL bFileExist = FALSE;
	INT32 lRename = 0;
	UINT8* pucFileData = NULL;
	UINT32 ulWriteSum = 0;
	UINT32 ulWriteOnce = 0;

	if((pstTFTPReqMsg->ulLen == 0)&&(pstTFTPReqMsg->pucMem != NULL))
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"pstTFTPReqMsg->ulLen=%d,pstTFTPReqMsg->pucMem=%d",
				pstTFTPReqMsg->ulLen,pstTFTPReqMsg->pucMem);
		return CPSS_ERR_FS_PARA_ILLEGAL;
	}

	/*判断命令类型是否正确*/
	if((pstTFTPReqMsg->ulCmd != CPSS_FTP_CMD_GET_FILE )&&
			(pstTFTPReqMsg->ulCmd != CPSS_FTP_CMD_PUT_FILE))
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,"ulCmd=%d,not correct.",pstTFTPReqMsg->ulCmd);
		return CPSS_ERR_FS_PARA_ILLEGAL;
	}

	if(pstTFTPReqMsg->pucMem == NULL)
	{
		strncpy(acClientFileName, pstTFTPReqMsg->aucClientFile, CPSS_FS_FILE_ABSPATH_LEN);
	}
	strncpy(acServerFileName, pstTFTPReqMsg->aucServerFile, CPSS_FS_FILE_ABSPATH_LEN);

	stAddr.s_addr = cpss_htonl(pstTFTPReqMsg->ulServerIP);
	inet_ntoa_b (stAddr, acServerIp);

	stTFTPConnect.ulLen = pstTFTPReqMsg->ulLen;
	stTFTPConnect.ulCmd = pstTFTPReqMsg->ulCmd;
	stTFTPConnect.szClientFileName = acClientFileName;
	stTFTPConnect.szServerFileName = acServerFileName;
	stTFTPConnect.szServerIP = acServerIp;
	stTFTPConnect.szMemBuf = pstTFTPReqMsg->pucMem;

	/*申请文件内存*/
	if((pstTFTPReqMsg->pucMem == NULL)&&(pstTFTPReqMsg->ulCmd == CPSS_FTP_CMD_GET_FILE))
	{
		pucFileData = cpss_mem_malloc(pstTFTPReqMsg->ulLen);
		if(NULL == pucFileData)
		{
			cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
					"tftp get file failed(malloc fail).fileName=%s,fileLen=%d\n",
					acServerFileName,pstTFTPReqMsg->ulLen);
			return CPSS_ERR_FS_MALLOC_FAIL;
		}
	}

	/*连接TFTP服务器并对输入参数进行检查*/
	lTftpConn = cpss_tftp_connect(&stTFTPConnect);
	if( lTftpConn != CPSS_OK )
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"connet tftp (%s) failed.",stTFTPConnect.szServerIP);

		if((pstTFTPReqMsg->pucMem == NULL)&&(pstTFTPReqMsg->ulCmd == CPSS_FTP_CMD_GET_FILE))
		{
			cpss_mem_free(pucFileData);
		}

		return lTftpConn;
	}

	/*从TFTP的描述符循环读取数据*/
	while ( (lStatus!=CPSS_ERROR)&&((lBytes=read(stTFTPConnect.lReadHandle,acBuff,CPSS_TFTP_RECV_SIZE))>0) )
	{
		/*如果是下载过程，且用户内存大于0，则将读到的数据存在用户传进行来的内存中。《此需求由资源管理提》*/
		if(pstTFTPReqMsg->ulCmd == CPSS_FTP_CMD_GET_FILE)
		{
			lBlockRead = lBlockRead + lBytes;
			if(lBlockRead > pstTFTPReqMsg->ulLen)
			{
				lStatus = CPSS_ERROR;
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
						"save memory(%d) too small lBlockRead=%d.",stTFTPConnect.ulLen,lBlockRead);
				break;
			}
			if(pstTFTPReqMsg->pucMem != NULL)
			{
				memcpy(pstTFTPReqMsg->pucMem+lBlockRead-lBytes,acBuff,lBytes);
			}
			else
			{
				memcpy(pucFileData+lBlockRead-lBytes,acBuff,lBytes);
			}
		}

		/*如果是上传文件操作,直接向TFTP数据端口写入数据*/
		if(stTFTPConnect.ulCmd == CPSS_FTP_CMD_PUT_FILE)
		{
			lWrite = write(stTFTPConnect.lWriteHandle,acBuff,lBytes);
			if( ERROR== lWrite)
			{
				lStatus = CPSS_ERROR;
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
						"write (%s) len(%d) errno=%d failed.",stTFTPConnect.szClientFileName,lBytes,errno);
			}
		}
		continue;
	}

	if(lBytes==ERROR)
	{
		lStatus = CPSS_ERROR;
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"read (%s) failed errno=%d.",stTFTPConnect.szClientFileName,errno);
	}

	close (stTFTPConnect.lDataSock);

	lBytes = read (stTFTPConnect.lCtrlSock, acBuff, CPSS_TFTP_RECV_SIZE);
	if (lBytes > 0)
	{
		acBuff[lBytes] = '\0';
		lStatus = CPSS_ERROR;
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"tftp transfer failed(%s)",acBuff);
	}

	/*tftpQuit(stTFTPConnect.lCtrlSock) ;*/
	close (stTFTPConnect.lCtrlSock);

	/*TFTP过程结束后,将数据写到写文件描述符中*/
	if((CPSS_ERROR != lStatus)&&(pstTFTPReqMsg->ulCmd == CPSS_FTP_CMD_GET_FILE)&&
			(pstTFTPReqMsg->pucMem == NULL))
	{
		while(lStatus!=CPSS_ERROR)
		{
			if(lBlockRead > CPSS_TFTP_WRITE_SIZE)
			{
				ulWriteOnce = CPSS_TFTP_WRITE_SIZE;
			}
			else
			{
				ulWriteOnce = lBlockRead;
			}

			lWrite = write(stTFTPConnect.lWriteHandle,pucFileData+ulWriteSum,ulWriteOnce);
			if( ERROR== lWrite)
			{
				lStatus = CPSS_ERROR;
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
						"write (%s) failed,write(%d)bytes\n",stTFTPConnect.szClientFileName,lBlockRead);
			}
			ulWriteSum = ulWriteSum + lWrite;
			lBlockRead = lBlockRead - lWrite;
			if(0 == lBlockRead)
			{
				break;
			}
		}
	}

	if( !((pstTFTPReqMsg->ulCmd == CPSS_FTP_CMD_GET_FILE)&&(pstTFTPReqMsg->pucMem != NULL)) )
	{
		cpss_file_close(stTFTPConnect.ulFileHandle);
	}

	/*如果状态为ERROR进行错误处理*/
	if(lStatus == CPSS_ERROR)
	{
		if( (pstTFTPReqMsg->ulCmd == CPSS_FTP_CMD_GET_FILE)&&(pstTFTPReqMsg->pucMem == NULL) )
		{
			cpss_file_delete(acClientFileName);
			cpss_mem_free(pucFileData);
		}
		return CPSS_ERR_TFTP_PROCESS_FAIL;
	}

	/*如果是下载过程，且文件存在本地硬盘，则将存储的临时文件重命名为用户规定的文件名  《此需求由资源管理提》*/
	if( (pstTFTPReqMsg->pucMem == NULL)&&(pstTFTPReqMsg->ulCmd == CPSS_FTP_CMD_GET_FILE) )
	{
		/*释放内存*/
		cpss_mem_free(pucFileData);

		strncpy(acServerFileName,acClientFileName, CPSS_FS_FILE_ABSPATH_LEN);

		/*去掉文件名称结尾所添加的.tmp*/
		cpss_ftp_dirname_recover(acServerFileName);

		/*if file exist,delete file*/
		bFileExist = cpss_file_exist(acServerFileName);
		if(bFileExist == TRUE)
		{
			cpss_file_delete(acServerFileName);
		}

		lRename = rename(acClientFileName,acServerFileName);
		if( lRename == ERROR )
		{
			cpss_file_delete(acClientFileName);
			cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
					"tftp client rename from %s to %s failed",acClientFileName,acServerFileName);
			return cpss_fs_errno_get();
		}
	}
	/*TFTP过程正确完成*/
	return CPSS_OK;
}

#elif defined CPSS_VOS_WINDOWS        /*windows操作系统*/
/*******************************************************************************
 * 函数名称: cpss_ftp_connect
 * 功    能: 提供FTP服务
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFTPConnect  CPSS_FTP_CONNECT_T*  输入              FTP连接结构指针
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_connect
(
		CPSS_FTP_CONNECT_T *pstFTPConnect, /*FTP连接结构指针*/
		UINT32* pulDownFileLen
)
{
	UINT32 ulFileLen;
	INT32 lRet;
	UINT8 aucServerFile[CPSS_FS_FILE_ABSPATH_LEN];

	/*打开Internet会话*/
	pstFTPConnect->pvInternetSession = InternetOpen(CPSS_FTP_AGENT_NAME,LOCAL_INTERNET_ACCESS,NULL,0,0);
	if(pstFTPConnect->pvInternetSession == NULL)
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,"internetOpen fail errorno = (%d).",GetLastError());
		return cpss_fs_errno_get();
	}

	/*打开connectiong session会话*/
	pstFTPConnect->pvConnectSession = InternetConnect(pstFTPConnect->pvInternetSession,
			pstFTPConnect->szServerIP,INTERNET_DEFAULT_FTP_PORT,
			pstFTPConnect->szUserName,pstFTPConnect->szPasswd,
			INTERNET_SERVICE_FTP,INTERNET_FLAG_PASSIVE,0);
	if(pstFTPConnect->pvConnectSession == NULL)
	{
		InternetCloseHandle(pstFTPConnect->pvInternetSession);
		return cpss_fs_errno_get();
	}

	strncpy(aucServerFile,pstFTPConnect->szServerFileName,CPSS_FS_FILE_ABSPATH_LEN);
	cpss_fs_linux_filename_make(aucServerFile);

	if(NULL != pulDownFileLen)
	{
		lRet = cpss_ftp_server_file_len_get(0,(INT32)pstFTPConnect->pvConnectSession,
				&ulFileLen,aucServerFile);
		if(CPSS_OK != lRet)
		{
			InternetCloseHandle(pstFTPConnect->pvConnectSession);
			InternetCloseHandle(pstFTPConnect->pvInternetSession);
			return CPSS_ERR_FS_SERVER_FILELEN_GET_FAIL;
		}

		*pulDownFileLen = ulFileLen;
	}

	/*打开FTP会话*/
	if((pstFTPConnect->ulCmd == CPSS_FTP_CMD_PUT_FILE)||(pstFTPConnect->ulCmd == CPSS_FTP_CMD_PUT_DIR_PACK))
	{
		pstFTPConnect->pvFTPSession =
		FtpOpenFile(pstFTPConnect->pvConnectSession,aucServerFile,
				GENERIC_WRITE,FTP_TRANSFER_TYPE_BINARY|INTERNET_FLAG_DONT_CACHE,0);
	}
	else if((pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_FILE)||(pstFTPConnect->ulCmd == CPSS_FTP_CMD_GET_DIR_PACK))
	{
		pstFTPConnect->pvFTPSession = FtpOpenFile(pstFTPConnect->pvConnectSession,aucServerFile,
				GENERIC_READ,FTP_TRANSFER_TYPE_BINARY|INTERNET_FLAG_DONT_CACHE,0);
	}

	/*打开FTP会话失败*/
	if(pstFTPConnect->pvFTPSession == NULL)
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"ftpOpen fail errorno = (%d).",GetLastError());

		InternetCloseHandle(pstFTPConnect->pvConnectSession);
		InternetCloseHandle(pstFTPConnect->pvInternetSession);

		return cpss_fs_errno_get();
	}

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_fs_ftp_disconnect
 * 功    能: 断开和ftp服务器的连接，释放申请的资源
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFTPConnect  CPSS_FTP_CONNECT_T*  输入              FTP连接结构指针
 * 函数返回:
 * 说   明:
 *******************************************************************************/
VOID cpss_fs_ftp_disconnect
(
		CPSS_FTP_CONNECT_T* pstFTPConnect /*FTP连接结构指针*/
)
{
	/*关闭internet handle、connection handle、ftp handle*/
	InternetCloseHandle(pstFTPConnect->pvFTPSession);
	InternetCloseHandle(pstFTPConnect->pvConnectSession);
	InternetCloseHandle(pstFTPConnect->pvInternetSession);
}

/*******************************************************************************
 * 函数名称: cpss_fs_ftp_get
 * 功    能: 提供FTP下载服务
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFTPReqMsg  CPSS_FTP_REQ_MSG_T*   输入              FTP服务请求消息结构指针
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_ftp_get
(
		CPSS_FTP_REQ_MSG_T* pstFTPReqMsg, /*FTP服务请求消息结构指针*/
		UINT32 ulDstPid
)
{
	INT32 ulBytes = 0;
	UINT32 ulBlockRead = 0;
	UINT32 ulFileHandle= 0;
	UINT8 aucBuff[CPSS_FTP_RECV_SIZE+1];
	UINT8 aucClientFileName[ CPSS_FS_FILE_ABSPATH_LEN];
	UINT8 aucServerFileName[ CPSS_FS_FILE_ABSPATH_LEN];
	UINT8 aucFileName[CPSS_FS_FILENAME_LEN];
	UINT8 aucServerIp[20];
	struct in_addr stAddr;
	CPSS_FTP_CONNECT_T stFTPConnect;
	INT32 lStatus = CPSS_OK;
	INT32 lFileNameChkClt = 0;
	INT32 lFileNameChkSvr = 0;
	INT32 lFtpConn = 0;
	INT32 lFtpTmpDirMake = 0;
	BOOL bInternetReadFile = FALSE;
	INT32 lWrite = 0;
	BOOL bFileExist = FALSE;
	BOOL bIsDir;
	INT32 lRename = 0;
	INT32 lRet;
	UINT32 ulBeginTick = 0;
	UINT32 ulCurrFtpLen = 0;
	UINT32 ulFtpLenStep = 0;
	UINT32 ulDownFileLen = 0;

	if((pstFTPReqMsg->ulLen >0 )&&(pstFTPReqMsg->pucMem == NULL)&&
			(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_FILE))
	{
		return CPSS_ERR_FS_PARA_ILLEGAL;
	}

	if(pstFTPReqMsg->ulLen == 0)
	{
		strncpy(aucClientFileName, pstFTPReqMsg->aucClientFile, CPSS_FS_FILE_ABSPATH_LEN);
		lFileNameChkClt = cpss_filename_check(aucClientFileName,TRUE);
	}
	strncpy(aucServerFileName, pstFTPReqMsg->aucServerFile, CPSS_FS_FILE_ABSPATH_LEN);
	lFileNameChkSvr = cpss_filename_check(aucServerFileName,FALSE);

	if((lFileNameChkClt != CPSS_OK)|| (lFileNameChkSvr != CPSS_OK))
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"(%s) and (%s) is illegal.",aucClientFileName,aucServerFileName);
		return CPSS_ERR_FS_FILENAME_ILLEGAL;
	}

	/*得到字符串形式的IP地址*/
	stAddr.s_addr = cpss_htonl(pstFTPReqMsg->ulServerIP);
	strcpy(aucServerIp,inet_ntoa(stAddr));

	stFTPConnect.szServerIP = aucServerIp;
	stFTPConnect.szServerFileName = aucServerFileName;
	stFTPConnect.szUserName = pstFTPReqMsg->aucUserName;
	stFTPConnect.szPasswd = pstFTPReqMsg->aucPasswd;
	stFTPConnect.ulCmd = pstFTPReqMsg->ulFtpCmd;

	ulBeginTick = cpss_tick_get();

	/*连接FTP服务器*/
	lFtpConn = cpss_ftp_connect(&stFTPConnect,&ulDownFileLen);
	if( lFtpConn != CPSS_OK )
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"connect ftp ip(%s) failed.",aucServerIp);
		return lFtpConn;
	}

	/*设置FTP的进度指示消息信息*/
	if(CPSS_OK != cpss_fs_ftp_ind_msg_set(pstFTPReqMsg->ulSerialID,
					pstFTPReqMsg->ulProgessStep,pstFTPReqMsg->ulFtpCmd,
					aucClientFileName,ulDstPid,ulDownFileLen))
	{
		lStatus = CPSS_ERROR;
	}

	/*打开文件*/
	if((pstFTPReqMsg->ulLen == 0)||(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_DIR_PACK))
	{
		cpss_file_is_dir(aucClientFileName,&bIsDir);
		if(TRUE == bIsDir)
		{
			cpss_fs_get_file_name(aucServerFileName,aucFileName);

			/*组织目录名+文件名*/
			lRet =cpss_fs_dirpath_link(aucClientFileName,aucFileName,CPSS_FS_FILE_ABSPATH_LEN);
			if(CPSS_OK != lRet)
			{
				lStatus = CPSS_ERROR;
			}
		}

		/*在同样的目录下构造一个在源文件名后加上.tmp后缀的临时文件名*/
		lFtpTmpDirMake = cpss_ftp_tmp_dirname_make(aucClientFileName, CPSS_FS_FILE_ABSPATH_LEN);
		if( lFtpTmpDirMake != CPSS_OK )
		{
			lStatus = CPSS_ERROR;
		}

		/*以写方式打开文件*/
		ulFileHandle = cpss_file_open(aucClientFileName,
				CPSS_FILE_ORDWR|CPSS_FILE_OCREATE|CPSS_FILE_OTRUNC|CPSS_FILE_OBINARY);
		if(ulFileHandle == CPSS_FD_INVALID)
		{
			cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
					"open (%s) failed.",aucClientFileName);
			lStatus = CPSS_ERROR;
		}

		if(lStatus == CPSS_ERROR)
		{
			cpss_fs_ftp_disconnect(&stFTPConnect);
			return CPSS_ERROR;
		}
	}

	while(1)
	{
		/*从FTP读取数据*/
		bInternetReadFile = InternetReadFile(stFTPConnect.pvFTPSession,aucBuff,CPSS_FTP_RECV_SIZE,&ulBytes);
		if( bInternetReadFile == TRUE)
		{
			if(ulBytes==0)
			{
				break;
			}

			/*如果是下载过程，且用户内存大于0，则将读到的数据存在用户传进行来的内存中。《此需求由资源管理提》*/
			if((pstFTPReqMsg->ulLen > 0)&&(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_FILE))
			{
				ulBlockRead = ulBlockRead + ulBytes;
				if(ulBlockRead > pstFTPReqMsg->ulLen)
				{
					lStatus = CPSS_ERROR;
					cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
							"save memory(%d) too small.",pstFTPReqMsg->ulLen);
					break;
				}
				memcpy(pstFTPReqMsg->pucMem+ulBlockRead-ulBytes,aucBuff,ulBytes);
			}
			else
			{
				/*将数据写到写文件描述符中*/
				lWrite = _write(ulFileHandle,aucBuff,ulBytes);
				if( -1== lWrite)
				{
					lStatus = CPSS_ERROR;
					cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
							"write (%s) failed.",aucClientFileName);
					break;
				}
			}

			ulCurrFtpLen = ulCurrFtpLen + ulBytes;
			ulFtpLenStep = ulFtpLenStep + ulBytes;

			/*发送进度指示消息*/
			cpss_ftp_progress_ind_send(ulBeginTick,ulCurrFtpLen,&ulFtpLenStep);
		}
		else
		{
			lStatus = CPSS_ERROR;
			break;
		}
	}

	/*释放ftp的资源*/
	cpss_fs_ftp_disconnect(&stFTPConnect);

	/*如果状态为ERROR进行错误处理*/
	if(lStatus == CPSS_ERROR)
	{
		if((pstFTPReqMsg->ulLen == 0)||(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_DIR_PACK))
		{
			cpss_file_close(ulFileHandle);
			cpss_file_delete(aucClientFileName);
		}
		return CPSS_ERROR;
	}

	/*如果是下载过程，且文件存在本地硬盘，则将存储的临时文件重命名为用户规定的文件名  《此需求由资源管理提》*/
	if((pstFTPReqMsg->ulLen == 0)||(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_DIR_PACK))
	{
		cpss_file_close(ulFileHandle);

		strncpy(aucServerFileName,aucClientFileName, CPSS_FS_FILE_ABSPATH_LEN);

		cpss_ftp_dirname_recover(aucServerFileName);

		/*如果下载的本地文件名存在则返回失败*/
		bFileExist = cpss_file_exist(aucServerFileName);
		if( bFileExist == TRUE)
		{
			cpss_file_delete(aucServerFileName);
		}
		lRename = cpss_file_rename(aucClientFileName,aucServerFileName);
		if( lRename != CPSS_OK )
		{
			cpss_file_delete(aucClientFileName);
			return cpss_fs_errno_get();
		}

		if(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_DIR_PACK)
		{
			lRet = cpss_ftp_file_unpack(aucServerFileName,ulCurrFtpLen);
			cpss_file_delete(aucServerFileName);
			if(CPSS_OK != lRet)
			{
				return lRet;
			}
		}
	}
	/*FTP_GET过程正确完成*/
	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_fs_ftp_put
 * 功    能: 提供FTP上传服务
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFTPReqMsg  CPSS_FTP_REQ_MSG_T*   输入              FTP服务请求消息结构指针
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_ftp_put
(
		CPSS_FTP_REQ_MSG_T* pstFTPReqMsg, /*FTP服务请求消息结构指针*/
		UINT32 ulDstPid
)
{
	INT32 lBytes = 0; /*读文件的字节数*/
	UINT32 ulWriteByte = 0; /*写入FTP的字节数*/
	UINT32 ulFileHandle= 0; /*文件的描述符*/
	UINT8 aucBuff[CPSS_FTP_RECV_SIZE+1];
	UINT8 aucClientFileName[ CPSS_FS_FILE_ABSPATH_LEN];
	UINT8 aucServerFileName[ CPSS_FS_FILE_ABSPATH_LEN];
	UINT8 aucServerIp[20];
	INT32 lStatus = CPSS_OK;
	struct in_addr stAddr; /*系统的IP结构体*/
	CPSS_FTP_CONNECT_T stFTPConnect; /*FTP的连接结构*/

	INT32 lFileNameChkClt = 0;
	INT32 lFileNameChkSvr = 0;
	INT32 lFtpTmpDirMake = 0;
	INT32 lFtpConn = 0;
	BOOL bInternetReadFile = FALSE;
	BOOL bFtpRenameFile = FALSE;
	INT32 lRet;
	UINT32 ulBeginTick = 0;
	UINT32 ulCurrFtpLen = 0;
	UINT32 ulFtpLenStep = 0;

	strncpy(aucClientFileName, pstFTPReqMsg->aucClientFile, CPSS_FS_FILE_ABSPATH_LEN);
	strncpy(aucServerFileName, pstFTPReqMsg->aucServerFile, CPSS_FS_FILE_ABSPATH_LEN);
	lFileNameChkClt = cpss_filename_check(aucClientFileName,TRUE);
	lFileNameChkSvr = cpss_filename_check(aucServerFileName,FALSE);
	if(( lFileNameChkClt != CPSS_OK)|| ( lFileNameChkSvr != CPSS_OK))
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"(%s) and (%s) is illegal.",aucClientFileName,aucServerFileName);
		return CPSS_ERR_FS_FILENAME_ILLEGAL;
	}

	/*判断源文件是否存在*/
	if(FALSE == cpss_file_exist(aucClientFileName))
	{
		return CPSS_ERR_FS_FILE_NOT_EXIST;
	}

	/*在同样的目录下构造一个在源文件名后加上.tmp后缀的临时文件名*/
	lFtpTmpDirMake = cpss_ftp_tmp_dirname_make(aucServerFileName, CPSS_FS_FILE_ABSPATH_LEN);
	if( lFtpTmpDirMake != CPSS_OK )
	{
		return CPSS_ERR_FS_TMP_NAME_MAKE_FAIL;
	}

	/*得到服务器地址的字符串形式*/
	stAddr.s_addr = cpss_htonl(pstFTPReqMsg->ulServerIP);
	strcpy(aucServerIp,inet_ntoa(stAddr));

	/*对FTP连接结构变量赋值*/
	stFTPConnect.szServerIP = aucServerIp;
	stFTPConnect.szServerFileName = aucServerFileName;
	stFTPConnect.szUserName = pstFTPReqMsg->aucUserName;
	stFTPConnect.szPasswd = pstFTPReqMsg->aucPasswd;
	stFTPConnect.ulCmd = pstFTPReqMsg->ulFtpCmd;

	/*打包压缩文件*/
	if(stFTPConnect.ulCmd == CPSS_FTP_CMD_PUT_DIR_PACK)
	{
		lRet = cpss_ftp_file_pack(aucClientFileName,TRUE,ulDstPid,
				pstFTPReqMsg->ulProgessStep,pstFTPReqMsg->ulSerialID);
		if(CPSS_OK != lRet)
		{
			return lRet;
		}
	}

	ulBeginTick = cpss_tick_get();

	lFtpConn = cpss_ftp_connect(&stFTPConnect,NULL);
	if( lFtpConn != CPSS_OK)
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"connet ftp (%s) failed.",aucServerIp);
		return lFtpConn;
	}

	if(CPSS_OK != cpss_fs_ftp_ind_msg_set(pstFTPReqMsg->ulSerialID,
					pstFTPReqMsg->ulProgessStep,pstFTPReqMsg->ulFtpCmd,
					aucClientFileName,ulDstPid,0))
	{
		lStatus = CPSS_ERROR;
	}
	ulFileHandle = cpss_file_open(aucClientFileName,CPSS_FILE_ORDONLY|CPSS_FILE_OBINARY);
	if(ulFileHandle == CPSS_FD_INVALID)
	{
		lStatus = CPSS_ERROR;
	}

	while(lStatus != CPSS_ERROR)
	{
		lBytes = _read(ulFileHandle,aucBuff,CPSS_FTP_RECV_SIZE);

		if(lBytes<0)
		{
			lStatus = CPSS_ERROR;
			break;
		}

		if(lBytes==0)
		{
			break;
		}

		bInternetReadFile = InternetWriteFile(stFTPConnect.pvFTPSession,aucBuff,lBytes,&ulWriteByte);
		if( bInternetReadFile == FALSE )
		{
			lStatus = CPSS_ERROR;
			break;
		}

		ulCurrFtpLen = ulCurrFtpLen + lBytes;
		ulFtpLenStep = ulFtpLenStep + lBytes;

		cpss_ftp_progress_ind_send(ulBeginTick,ulCurrFtpLen,&ulFtpLenStep);
	}

	cpss_file_close(ulFileHandle);

	if(stFTPConnect.ulCmd == CPSS_FTP_CMD_PUT_DIR_PACK)
	{
		cpss_file_delete(aucClientFileName);
	}

	strcpy(aucClientFileName,aucServerFileName);
	cpss_ftp_dirname_recover(aucClientFileName);

	InternetCloseHandle(stFTPConnect.pvFTPSession);

	cpss_fs_linux_filename_make(aucClientFileName);
	cpss_fs_linux_filename_make(aucServerFileName);

	FtpDeleteFile(stFTPConnect.pvConnectSession,aucClientFileName);

	bFtpRenameFile = FtpRenameFile(stFTPConnect.pvConnectSession,aucServerFileName,aucClientFileName);
	if( bFtpRenameFile !=TRUE)
	{
		lStatus = CPSS_ERROR;
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"rename server filename(%s) failed",aucClientFileName);
	}
	InternetCloseHandle(stFTPConnect.pvConnectSession);
	InternetCloseHandle(stFTPConnect.pvInternetSession);

	return lStatus;
}

/*******************************************************************************
 * 函数名称: cpss_fs_ftp
 * 功    能: 提供FTP服务
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFTPReqMsg  CPSS_FTP_REQ_MSG_T*   输入              FTP服务请求消息结构指针
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_ftp
(
		CPSS_FTP_REQ_MSG_T* pstFTPReqMsg, /*FTP服务请求消息结构指针*/
		UINT32 ulDstPid
)
{
	INT32 lFtpPut = 0;
	INT32 lFtpGet = 0;
	/*如果需要上传服务*/
	if((pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_PUT_FILE)||
			(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_PUT_DIR_PACK))
	{
		lFtpPut = cpss_fs_ftp_put(pstFTPReqMsg,ulDstPid);
		if( lFtpPut != CPSS_OK )
		{
			return lFtpPut;
		}
	}
	/*如果需要上传服务*/
	else if((pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_FILE)||
			(pstFTPReqMsg->ulFtpCmd == CPSS_FTP_CMD_GET_DIR_PACK))
	{
		lFtpGet = cpss_fs_ftp_get(pstFTPReqMsg,ulDstPid);
		if( lFtpGet != CPSS_OK )
		{
			return lFtpGet;
		}
	}
	/*类型不识别*/
	else
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"service type(%d) is not support.",pstFTPReqMsg->ulFtpCmd);
		return CPSS_ERROR;
	}
	return CPSS_OK;
}

#endif

/*******************************************************************************
 * 函数名称: cpss_tftp_link_head_get
 * 功    能: 得到TFTP链表头指针
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
CPSS_TFTP_TRANS_NODE_T** cpss_tftp_link_head_get()
{
	return (CPSS_TFTP_TRANS_NODE_T**) ((UINT8*) cpss_vk_proc_user_data_get()
			+ sizeof(CPSS_FTP_INFO_MAN_T));
}

/*******************************************************************************
 * 函数名称: cpss_tftp_transid_get
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_tftp_transid_get(UINT32* pulTransId)
{
	CPSS_COM_PHY_ADDR_T stPhyAddr;
	UINT32 ulID;

	cpss_vos_mutex_p(g_ulCpssTftpMutex, CPSS_VOS_MUTEX_WAIT_FOREVER);

	cpss_com_phy_addr_get(&stPhyAddr);

	ulID = (stPhyAddr.ucFrame - 1) * CPSS_COM_MAX_SHELF * CPSS_COM_MAX_SLOT
			*CPSS_COM_MAX_CPU * CPSS_TFTP_MAX_REQ_NUM + (stPhyAddr.ucShelf - 1)
			* CPSS_COM_MAX_SLOT * CPSS_COM_MAX_CPU * CPSS_TFTP_MAX_REQ_NUM
			+ (stPhyAddr.ucSlot - 1) * CPSS_COM_MAX_CPU * CPSS_TFTP_MAX_REQ_NUM
			+ (stPhyAddr.ucCpu - 1) * CPSS_TFTP_MAX_REQ_NUM;

	*pulTransId = ulID + g_ulCpssTftpSerialNo;

	g_ulCpssTftpSerialNo++;

	cpss_vos_mutex_v(g_ulCpssTftpMutex);

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_transid_link_add
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: 
 *******************************************************************************/
INT32 cpss_tftp_transid_link_add(CPSS_TFTP_TRANS_NODE_T *pstTftpNode)
{
	CPSS_TFTP_TRANS_NODE_T *pstLinkNode;
	CPSS_TFTP_TRANS_NODE_T *pstNextNode;
	CPSS_TFTP_TRANS_NODE_T **ppstTftpHead;

	ppstTftpHead = cpss_tftp_link_head_get();

	pstLinkNode = cpss_mem_malloc(sizeof(CPSS_TFTP_TRANS_NODE_T));
	if (pstTftpNode == NULL)
	{
		cpss_output(CPSS_MODULE_FS, CPSS_PRINT_FAIL,
				"transid add failed (malloc fail)\n");
		return CPSS_ERR_TFTP_MALLOC_NODE_MEM_FAIL;
	}
	*pstLinkNode = *pstTftpNode;

	if (*ppstTftpHead == NULL)
	{
		*ppstTftpHead = pstLinkNode;
	}
	else
	{
		pstNextNode = *ppstTftpHead;
		*ppstTftpHead = pstLinkNode;
		pstLinkNode->pvNext = pstNextNode;
	}

	cpss_output(CPSS_MODULE_FS, CPSS_PRINT_DETAIL,
			"tftp transid id add ok transid=%d\n", pstTftpNode->ulTftpTransId);
	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_transid_link_find
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: 
 *******************************************************************************/
CPSS_TFTP_TRANS_NODE_T* cpss_tftp_transid_link_find(UINT32 ulTftpTransId)
{
	CPSS_TFTP_TRANS_NODE_T* pstTftpTransNode;
	CPSS_TFTP_TRANS_NODE_T** ppstTftpHead;

	ppstTftpHead = cpss_tftp_link_head_get();

	pstTftpTransNode = *ppstTftpHead;

	while (pstTftpTransNode != NULL)
	{
		if (pstTftpTransNode->ulTftpTransId == ulTftpTransId)
		{
			return pstTftpTransNode;
		}
		pstTftpTransNode = (CPSS_TFTP_TRANS_NODE_T*) pstTftpTransNode->pvNext;
	}

	cpss_output(CPSS_MODULE_FS, CPSS_PRINT_WARN,
			"find transid failed.transid=%d\n", ulTftpTransId);
	return NULL;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_transid_link_del
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: 
 *******************************************************************************/
int cpss_tftp_transid_link_del(UINT32 ulTftpTransId)
{
	CPSS_TFTP_TRANS_NODE_T* pstTftpNode;
	CPSS_TFTP_TRANS_NODE_T* pstPreTftpNode;
	CPSS_TFTP_TRANS_NODE_T** ppstTftpHead;

	ppstTftpHead = cpss_tftp_link_head_get();

	pstTftpNode = *ppstTftpHead;
	pstPreTftpNode = *ppstTftpHead;

	while (pstTftpNode != NULL)
	{
		if (pstTftpNode->ulTftpTransId == ulTftpTransId)
		{
			if (pstTftpNode == pstPreTftpNode)
			{
				*ppstTftpHead = pstTftpNode->pvNext;
			}
			else
			{
				pstPreTftpNode->pvNext = pstTftpNode->pvNext;
			}

			cpss_mem_free(pstTftpNode);

			cpss_output(CPSS_MODULE_FS, CPSS_PRINT_DETAIL,
					"tftp delete transid link success ulTftpTransId=%d\n",
					ulTftpTransId);

			return CPSS_OK;
		}

		pstPreTftpNode = pstTftpNode;
		pstTftpNode = (CPSS_TFTP_TRANS_NODE_T*) pstTftpNode->pvNext;
	}

	cpss_output(CPSS_MODULE_FS, CPSS_PRINT_DETAIL,
			"tftp delete transid link success ulTftpTransId=%d\n",
			ulTftpTransId);

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_rsp_msg_send
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: XA00016102 使用自研文件下载程序
 *******************************************************************************/
INT32 cpss_tftp_rsp_msg_send(CPSS_COM_PID_T* pstDstPid, INT32 lErrnoNo,
		INT32 lSerialId)
{
	CPSS_TFTP_RSP_MSG_T stTftpRspMsg;
	INT32 lRet;

	stTftpRspMsg.ulSerialID = lSerialId;
	stTftpRspMsg.lErrorNo = lErrnoNo;

	if (pstDstPid->ulAddrFlag == CPSS_COM_ADDRFLAG_PHYSICAL)
	{
		lRet = cpss_com_send_phy(
				*(CPSS_COM_PHY_ADDR_T*) &pstDstPid->stLogicAddr,
				pstDstPid->ulPd, CPSS_TFTP_RSP_MSG, (UINT8*) &stTftpRspMsg,
				sizeof(CPSS_TFTP_RSP_MSG_T));
	}
	else
	{
		lRet = cpss_com_send(pstDstPid, CPSS_TFTP_RSP_MSG,
				(UINT8*) &stTftpRspMsg, sizeof(CPSS_TFTP_RSP_MSG_T));
	}

	cpss_output(CPSS_MODULE_FS, CPSS_PRINT_DETAIL,
			"tftp rsp send lRet=%d,ulSerialID=%d,lErrorNo=%d\n", lRet,
			stTftpRspMsg.ulSerialID, stTftpRspMsg.lErrorNo);

	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_disconnect_msg_send
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: 
 *******************************************************************************/
INT32 cpss_tftp_disconnect_msg_send(CPSS_COM_PHY_ADDR_T stDestPhyAddr,
		UINT32 ulTftpTransId, INT32 lResult)
{
	CPSS_TFTP_TRANS_DISCONNCT_MSG_T stDisconnectMsg;
	INT32 lRet;

	stDisconnectMsg.ulTftpTransId = cpss_htonl(ulTftpTransId);
	stDisconnectMsg.lResult = cpss_htonl(lResult);

	lRet = cpss_com_send_phy(stDestPhyAddr, cpss_vk_pd_self_get(),
			CPSS_TFTP_TRANS_DISCONNECT_MSG, (UINT8*) &stDisconnectMsg,
			sizeof(CPSS_TFTP_TRANS_DISCONNCT_MSG_T));

	cpss_output(CPSS_MODULE_FS, CPSS_PRINT_WARN,
			"tftp disconect msg send.transid=%d,lResult=%d\n", ulTftpTransId,
			lResult);

	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_trans_req_send
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: 
 *******************************************************************************/
INT32 cpss_tftp_trans_req_send(CPSS_COM_PHY_ADDR_T stDestPhyAddr,
		UINT32 ulTftpCmd, UINT32 ulTransId, UINT8* pucServerFile)
{
	CPSS_TFTP_TRANS_REQ_MSG_T stTftpReq;
	CPSS_COM_PHY_ADDR_T stSrcPhyAddr;
	INT32 lRet;

	stTftpReq.ulTftpCmd = cpss_htonl(ulTftpCmd);
	stTftpReq.ulTftpTransId = cpss_htonl(ulTransId);
	strncpy(stTftpReq.aucServerFilePath, pucServerFile,
			CPSS_FS_FILE_ABSPATH_LEN);

	cpss_com_phy_addr_get(&stSrcPhyAddr);

	stTftpReq.stSrcPhyAddr = stSrcPhyAddr;

	lRet = cpss_com_send_phy(stDestPhyAddr, cpss_vk_pd_self_get(),
			CPSS_TFTP_TRANS_REQ_MSG, (UINT8*) &stTftpReq,
			sizeof(CPSS_TFTP_TRANS_REQ_MSG_T));

	cpss_output(CPSS_MODULE_FS, CPSS_PRINT_DETAIL,
			"tftp req send ulTransId=%d,ulTftpCmd=%d\n", ulTransId, ulTftpCmd);

	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_ack_msg_send
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: 
 *******************************************************************************/
INT32 cpss_tftp_ack_msg_send(CPSS_COM_PHY_ADDR_T stDestPhyAddr,
		UINT32 ulTftpTransId, UINT32 ulAckNo)
{
	CPSS_TFTP_TRANS_ACK_MSG_T stAckIndMsg;
	INT32 lRet;

	stAckIndMsg.ulAckNo = cpss_htonl(ulAckNo);
	stAckIndMsg.ulTftpTransId = cpss_htonl(ulTftpTransId);

	lRet = cpss_com_send_phy(stDestPhyAddr, cpss_vk_pd_self_get(),
			CPSS_TFTP_TRANS_ACK_MSG, (UINT8*) &stAckIndMsg,
			sizeof(CPSS_TFTP_TRANS_ACK_MSG_T));

	cpss_output(CPSS_MODULE_FS, CPSS_PRINT_DETAIL,
			"tftp send ack ackNo=%d transid=%d\n", ulAckNo, ulTftpTransId);
	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_trans_data_send
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 *                     ulFlag   0: 超时 1: ACK不一致  用于判断超时发送还是ACK不一致, ACK不一致不发
 * 函数返回:
 * 说   明: 
 *******************************************************************************/
INT32 cpss_tftp_trans_data_send(UINT32 ulTransId, UINT32 ulFlag)
{
	CPSS_TFTP_TRANS_NODE_T *pstTransNode;
	CPSS_TFTP_TRANS_DATA_MSG_T stSendData;
	UINT32 ulReadLen;
	UINT32 ulOffset;

	pstTransNode = cpss_tftp_transid_link_find(ulTransId);
	if (pstTransNode == NULL)
	{
		return CPSS_ERROR;
	}
	/*如果发送序号和ACK序号相等,则发送下一帧*/
	if ((pstTransNode->ulSendNo == pstTransNode->ulAckNo)
			|| (pstTransNode->ulAckNo == 0xFFFFFFFF))
	{
		if (0xFFFFFFFF == pstTransNode->ulAckNo)
		{
			cpss_file_seek(pstTransNode->ulFd, 0, CPSS_SEEK_SET, NULL);
			pstTransNode->ulReadLen = 0;
		}
		cpss_file_read(pstTransNode->ulFd, stSendData.aucSendData,
				CPSS_TFTP_RECV_SIZE, &ulReadLen);

		pstTransNode->ulReadLen = pstTransNode->ulReadLen + ulReadLen;
		pstTransNode->ulSendDataLen = ulReadLen;
		if (pstTransNode->ulAckNo == 0xFFFFFFFF)
		{
			pstTransNode->ulSendNo = 0;

		}
		else
		{
			pstTransNode->ulSendNo++;
		}
	}
	/*如果发送序号和ACK序号不等,则发送前一帧*/
	else
	{
		if (1 == ulFlag)
		{
			return CPSS_ERROR;
		}

		ulOffset = pstTransNode->ulReadLen - pstTransNode->ulSendDataLen;
		cpss_file_seek(pstTransNode->ulFd, ulOffset, CPSS_SEEK_SET, NULL);
		cpss_file_read(pstTransNode->ulFd, stSendData.aucSendData,
				CPSS_TFTP_RECV_SIZE, &ulReadLen);
	}

	/*组织发送的消息*/
	stSendData.ulTftpTransId = cpss_htonl(pstTransNode->ulTftpTransId);
	stSendData.ulDataLen = cpss_htonl(ulReadLen);
	stSendData.ulEndFlag = 0;
	stSendData.ulDataNo = cpss_htonl(pstTransNode->ulSendNo);

	/*如果文件已经读完,设置文件读完标志*/
	if (ulReadLen < CPSS_TFTP_RECV_SIZE)
	{
		stSendData.ulEndFlag = cpss_htonl(CPSS_TFTP_DATA_END);
		pstTransNode->ulDataEndFlag = CPSS_TFTP_DATA_END;
	}

	cpss_com_send_phy(pstTransNode->stDestPhyAddr, cpss_vk_pd_self_get(),
			CPSS_TFTP_TRANS_DATA_MSG, (UINT8*) &stSendData,
			sizeof(CPSS_TFTP_TRANS_DATA_MSG_T));

	cpss_output(
			CPSS_MODULE_FS,
			CPSS_PRINT_DETAIL,
			"tftp send data ulTftpTransId=%d,dataNo=%d,ulReadLen=%d,ulEndFlag=%d\n",
			pstTransNode->ulTftpTransId, pstTransNode->ulSendNo, ulReadLen,
			cpss_ntohl(stSendData.ulEndFlag));
	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_trans_ack_recv
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: 
 *******************************************************************************/
int cpss_tftp_trans_ack_recv(CPSS_TFTP_TRANS_ACK_MSG_T* pstAckMsg)
{
	INT32 lRet = CPSS_OK;
	UINT32 ulTransId;
	UINT32 ulAckNo;
	CPSS_TFTP_TRANS_NODE_T *pstTransNode;

	ulTransId = cpss_ntohl(pstAckMsg->ulTftpTransId);
	ulAckNo = cpss_ntohl(pstAckMsg->ulAckNo);

	cpss_output(CPSS_MODULE_FS, CPSS_PRINT_DETAIL,
			"tftp recv ack msg ulAckNo=%d,ulTransId=%d\n", ulAckNo, ulTransId);

	pstTransNode = cpss_tftp_transid_link_find(ulTransId);
	if (pstTransNode == NULL)
	{
		return CPSS_ERROR;
	}

	pstTransNode->ulAckNo = ulAckNo;

	/*最后一包的确认*/
	if ((pstTransNode->ulDataEndFlag == CPSS_TFTP_DATA_END)
			&& (pstTransNode->ulSendNo == pstTransNode->ulAckNo))
	{
		/*如果是客户端则向TFTP发送响应消息*/
		if (pstTransNode->ulClientFlag == CPSS_TFTP_CLIENT_FLAG)
		{
			/*发送响应消息*/
			cpss_tftp_rsp_msg_send(&pstTransNode->stDestPid, CPSS_OK,
					pstTransNode->ulSerialID);
		}

		/*关闭文件*/
		if (pstTransNode->ulWriteMedium == CPSS_TFTP_WRITE_MEDIUM_DISK)
		{
			cpss_file_close(pstTransNode->ulFd);
		}

		/*删除定时器资源*/
		cpss_timer_para_delete(pstTransNode->ulTimerId);

		/*删除node节点*/
		cpss_tftp_transid_link_del(ulTransId);
	}
	else
	{
		/*发送下一包数据*/
		lRet = cpss_tftp_trans_data_send(ulTransId, 1);
	}

	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_download_msg_send
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: 下载完后,写盘前发送消息给smss..
 *******************************************************************************/
INT32 cpss_tftp_download_msg_send(CPSS_COM_PID_T* pstDstPid, INT32 lErrnoNo,
		INT32 lSerialId)
{

	CPSS_TFTP_RSP_MSG_T stTftpRspMsg;
	INT32 lRet;

	stTftpRspMsg.ulSerialID = lSerialId;
	stTftpRspMsg.lErrorNo = lErrnoNo;

	if (pstDstPid->ulAddrFlag == CPSS_COM_ADDRFLAG_PHYSICAL)
	{
		lRet = cpss_com_send_phy(
				*(CPSS_COM_PHY_ADDR_T*) &pstDstPid->stLogicAddr,
				pstDstPid->ulPd, CPSS_TFTP_DOWNLOAD_STAT_MSG,
				(UINT8*) &stTftpRspMsg, sizeof(CPSS_TFTP_RSP_MSG_T));
	}
	else
	{
		lRet = cpss_com_send(pstDstPid, CPSS_TFTP_DOWNLOAD_STAT_MSG,
				(UINT8*) &stTftpRspMsg, sizeof(CPSS_TFTP_RSP_MSG_T));
	}

	cpss_output(CPSS_MODULE_FS, CPSS_PRINT_DETAIL,
			"tftp download send lRet=%d,ulSerialID=%d,lErrorNo=%d\n", lRet,
			stTftpRspMsg.ulSerialID, stTftpRspMsg.lErrorNo);

	return lRet;

}

/*******************************************************************************
 * 函数名称: cpss_tfpt_trans_data_recv
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: 
 *******************************************************************************/
int cpss_tfpt_trans_data_recv(CPSS_TFTP_TRANS_DATA_MSG_T* pstTftpDataMsg)
{
	INT32 lRet = CPSS_OK;
	printf(
			"involking function cpss_tfpt_trans_data_recv which omitted");
#if 	0 
	UINT32 ulTransId;
	UINT32 ulTransDataLen;
	UINT32 ulDataNo;
	UINT32 ulEndFlag;
	UINT32 ulWriteLen;
	UINT8* pucUsrMem;
	CPSS_COM_PHY_ADDR_T stDstPhyAddr;
	CPSS_TFTP_TRANS_NODE_T *pstTransNode;

	ulTransId = cpss_ntohl(pstTftpDataMsg->ulTftpTransId);
	ulTransDataLen = cpss_ntohl(pstTftpDataMsg->ulDataLen);
	ulDataNo = cpss_ntohl(pstTftpDataMsg->ulDataNo);
	ulEndFlag = cpss_ntohl(pstTftpDataMsg->ulEndFlag);

	cpss_output(CPSS_MODULE_FS,CPSS_PRINT_DETAIL,
			"tftp recv data transid=%d,dataNo=%d,dataLen=%d,endFlag=%d\n",
			ulTransId,ulDataNo,ulTransDataLen,ulEndFlag);

	pstTransNode = cpss_tftp_transid_link_find(ulTransId);
	if(pstTransNode == NULL)
	{
		return CPSS_ERROR;
	}

	stDstPhyAddr = pstTransNode->stDestPhyAddr;
	pucUsrMem = (UINT8*)pstTransNode->ulWriteBuf;

	if((ulDataNo == pstTransNode->ulAckNo)||(pstTransNode->ulAckNo == 0xFFFFFFFF))
	{
		if(pstTransNode->ulWriteMedium == CPSS_TFTP_WRITE_MEDIUM_DISK)
		{
			lRet = cpss_file_write(pstTransNode->ulFd,
					pstTftpDataMsg->aucSendData,ulTransDataLen,&ulWriteLen);
		}
		else
		{
			if(pstTransNode->ulWriteBufLen < (pstTransNode->ulRecvDataLen + ulTransDataLen))
			{
				lRet = CPSS_ERR_FS_MEM_WRITE_FAIL;
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
						"tftp usr mem is too small,memBufLen=%d ulRecvDataLen = %d ulTransDataLen = %d\n",pstTransNode->ulWriteBufLen,pstTransNode->ulRecvDataLen,ulTransDataLen);
				cpss_print(SWP_SUBSYS_CPSS,CPSS_MODULE_FS,CPSS_PRINT_IMPORTANT,
						"tftp usr mem is too small,memBufLen=%d ulRecvDataLen = %d ulTransDataLen = %d file %s \n",pstTransNode->ulWriteBufLen,pstTransNode->ulRecvDataLen,ulTransDataLen,pstTransNode->aucFileName);

			}
			else
			{
				cpss_mem_memcpy((pucUsrMem+pstTransNode->ulRecvDataLen),
						pstTftpDataMsg->aucSendData,ulTransDataLen);
			}
		}

		pstTransNode->ulRecvDataLen = pstTransNode->ulRecvDataLen + ulTransDataLen;
		pstTransNode->ulAckNo = ulDataNo+1;
	}

	if(lRet != CPSS_OK)
	{
		cpss_tftp_disconnect_msg_send(stDstPhyAddr,ulTransId,lRet);
	}
	else
	{
		cpss_tftp_ack_msg_send(stDstPhyAddr,ulTransId,pstTransNode->ulAckNo-1);
	}

	if((ulEndFlag == CPSS_TFTP_DATA_END)||(lRet != CPSS_OK))
	{
		if(pstTransNode->ulWriteMedium == CPSS_TFTP_WRITE_MEDIUM_DISK)
		{
			cpss_file_close(pstTransNode->ulFd);

			if((pstTransNode->ulTransSideFlag == CPSS_TFTP_RECV_SIDE)&&(lRet != CPSS_OK))
			{
				cpss_file_delete(pstTransNode->aucFileName);
			}
			else
			{
				INT8 acClientFileName[ CPSS_FS_FILE_ABSPATH_LEN]=
				{	0};
				strncpy(acClientFileName,pstTransNode->aucFileName,CPSS_FS_FILE_ABSPATH_LEN);
				cpss_ftp_dirname_recover(acClientFileName);
				cpss_file_delete(acClientFileName);
				cpss_file_rename(pstTransNode->aucFileName,acClientFileName);
			}
		}
		else
		{
			if(pstTransNode->bRelWriteBuf == TRUE)
			{

				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_DETAIL,
						"cpss exp tftp  ulSerialID %d recv buf  total len = %d",pstTransNode->ulSerialID,pstTransNode->ulRecvDataLen);

				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_DETAIL,
						"cpss exp tftp ulSerialID %d  start write disk @ %d len = %d",pstTransNode->ulSerialID,cpss_tick_get(),pstTransNode->ulWriteBufLen);

				if(lRet == CPSS_OK)
				{
					cpss_tftp_download_msg_send(&pstTransNode->stDestPid,CPSS_OK,pstTransNode->ulSerialID);

					lRet = cpss_file_write(pstTransNode->ulFd,
							pucUsrMem,pstTransNode->ulWriteBufLen,&ulWriteLen);
				}

				cpss_file_close(pstTransNode->ulFd);

				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_DETAIL,
						"cpss exp tftp ulSerialID %d end write disk @ %d len = %d write len = %d",pstTransNode->ulSerialID,cpss_tick_get(),pstTransNode->ulWriteBufLen,ulWriteLen);

				cpss_mem_free((VOID *)pstTransNode->ulWriteBuf);

				pstTransNode->bRelWriteBuf = FALSE;

			} 
		}
	}

	if((pstTransNode->ulClientFlag == CPSS_TFTP_CLIENT_FLAG)&&
			((ulEndFlag == CPSS_TFTP_DATA_END)||(lRet != CPSS_OK)))
	{
		cpss_tftp_rsp_msg_send(&pstTransNode->stDestPid,lRet,pstTransNode->ulSerialID);
	}

	if((ulEndFlag == CPSS_TFTP_DATA_END)||(lRet != CPSS_OK))
	{
		cpss_timer_para_delete(pstTransNode->ulTimerId);

		cpss_tftp_transid_link_del(ulTransId);
	}
#endif
	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_disconnect_msg_recv
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: 
 *******************************************************************************/
INT32 cpss_tftp_disconnect_msg_recv(
		CPSS_TFTP_TRANS_DISCONNCT_MSG_T *pstTftpDisconnect)
{
	UINT32 ulTransId;
	INT32 lResult;
	CPSS_TFTP_TRANS_NODE_T *pstTransNode;

	ulTransId = cpss_ntohl(pstTftpDisconnect->ulTftpTransId);
	lResult = cpss_ntohl(pstTftpDisconnect->lResult);

	cpss_output(CPSS_MODULE_FS, CPSS_PRINT_WARN,
			"tftp recv disconnect msg ulTransId=%d,lResult=%d\n", ulTransId,
			lResult);

	pstTransNode = cpss_tftp_transid_link_find(ulTransId);
	if (pstTransNode == NULL)
	{
		return CPSS_ERROR;
	}

	/*如果是客户端则向TFTP发送响应消息*/
	if (pstTransNode->ulClientFlag == CPSS_TFTP_CLIENT_FLAG)
	{
		/*发送响应消息*/
		cpss_tftp_rsp_msg_send(&pstTransNode->stDestPid, lResult,
				pstTransNode->ulSerialID);
	}

	/*关闭文件*/
	if (pstTransNode->ulWriteMedium == CPSS_TFTP_WRITE_MEDIUM_DISK)
	{
		cpss_file_close(pstTransNode->ulFd);

		/*删除文件*/
		if (pstTransNode->ulTransSideFlag == CPSS_TFTP_RECV_SIDE)
		{
			cpss_file_delete(pstTransNode->aucFileName);
		}
	}
	if (CPSS_TFTP_WRITE_MEDIUM_MEM == pstTransNode->ulWriteMedium)
	{ /* 如果是先下载到内存再写盘*/
		if (pstTransNode->bRelWriteBuf == TRUE)
		{
			pstTransNode->bRelWriteBuf = FALSE;
			/* 释放内存*/
			cpss_mem_free((VOID *)pstTransNode->ulWriteBuf );
			/* 关闭文件*/
			cpss_file_close(pstTransNode->ulFd);
			/* 删除文件*/
			cpss_file_delete(pstTransNode->aucFileName);
		}
	}

	/*删除定时器资源*/
	cpss_timer_para_delete(pstTransNode->ulTimerId);

	/*删除node节点*/
	cpss_tftp_transid_link_del(ulTransId);

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_send_side_timeout_deal
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: 
 *******************************************************************************/
INT32 cpss_tftp_send_side_timeout_deal(CPSS_TFTP_TRANS_NODE_T* pstTransNode)
{
	UINT32 ulTransId;

	ulTransId = pstTransNode->ulTftpTransId;

	if (pstTransNode->ulTimeVisit == 0)
	{
		pstTransNode->ulSendNoOld = pstTransNode->ulSendNo;
		pstTransNode->ulTimeVisit = 1;
		return CPSS_OK;
	}

	cpss_output(CPSS_MODULE_FS, CPSS_PRINT_DETAIL,
			"tftp recv send side timeout msg,transid=%d\n",
			pstTransNode->ulTftpTransId);

	if (pstTransNode->ulSendNo == pstTransNode->ulSendNoOld)
	{
		pstTransNode->ulTimeVisit++;

		if (pstTransNode->ulTimeVisit >= CPSS_TFTP_TIMEOUT_COUNT_MAX)
		{
			cpss_output(CPSS_MODULE_FS, CPSS_PRINT_FAIL,
					"tftp 10 times timeout delete tftp link.\n");

			if (pstTransNode->ulClientFlag == CPSS_TFTP_CLIENT_FLAG)
			{
				cpss_tftp_rsp_msg_send(&pstTransNode->stDestPid,
						CPSS_ERR_TFTP_TIMEOUT, pstTransNode->ulSerialID);
			}

			if (pstTransNode->ulWriteMedium == CPSS_TFTP_WRITE_MEDIUM_DISK)
			{
				cpss_file_close(pstTransNode->ulFd);

			}

			cpss_timer_para_delete(pstTransNode->ulTimerId);

			cpss_tftp_transid_link_del(ulTransId);
		}

		if ((pstTransNode->ulTimeVisit % CPSS_TFTP_RESEND_TIMEOUT_COUNT) == 0)
		{
			cpss_output(CPSS_MODULE_FS, CPSS_PRINT_WARN,
					"tftp 2 times timeout resend tftp data\n");

			cpss_tftp_trans_data_send(ulTransId, 0);
		}
	}
	else
	{
		pstTransNode->ulTimeVisit = 1;
		pstTransNode->ulSendNoOld = pstTransNode->ulSendNo;
	}

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_recv_side_timeout_deal
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: 
 *******************************************************************************/
INT32 cpss_tftp_recv_side_timeout_deal(CPSS_TFTP_TRANS_NODE_T* pstTransNode)
{
	UINT32 ulTransId;

	ulTransId = pstTransNode->ulTftpTransId;

	if (pstTransNode->ulTimeVisit == 0)
	{
		pstTransNode->ulAckNoOld = pstTransNode->ulAckNo;
		pstTransNode->ulTimeVisit = 1;

		return CPSS_OK;
	}

	cpss_output(CPSS_MODULE_FS, CPSS_PRINT_DETAIL,
			"tftp recv recv side timeout msg,transid=%d\n",
			pstTransNode->ulTftpTransId);

	if (pstTransNode->ulAckNo == pstTransNode->ulAckNoOld)
	{
		pstTransNode->ulTimeVisit++;
		if (pstTransNode->ulTimeVisit >= CPSS_TFTP_TIMEOUT_COUNT_MAX)
		{
			cpss_output(CPSS_MODULE_FS, CPSS_PRINT_FAIL,
					"tftp 10 times timeout delete tftp link.\n");

			if (pstTransNode->ulClientFlag == CPSS_TFTP_CLIENT_FLAG)
			{
				cpss_tftp_rsp_msg_send(&pstTransNode->stDestPid,
						CPSS_ERR_TFTP_TIMEOUT, pstTransNode->ulSerialID);
			}

			if (pstTransNode->ulWriteMedium == CPSS_TFTP_WRITE_MEDIUM_DISK)
			{
				cpss_file_close(pstTransNode->ulFd);

				if (pstTransNode->ulTransSideFlag == CPSS_TFTP_RECV_SIDE)
				{
					cpss_file_delete(pstTransNode->aucFileName);
				}
			}
			else if (CPSS_TFTP_WRITE_MEDIUM_MEM == pstTransNode->ulWriteMedium)
			{
				if (pstTransNode->bRelWriteBuf == TRUE)
				{
					pstTransNode->bRelWriteBuf = FALSE;

					cpss_mem_free((VOID *)pstTransNode->ulWriteBuf );

					cpss_file_close(pstTransNode->ulFd);

					cpss_file_delete(pstTransNode->aucFileName);
				}
			}

			cpss_timer_para_delete(pstTransNode->ulTimerId);

			cpss_tftp_transid_link_del(ulTransId);

		}
	}
	else
	{
		pstTransNode->ulTimeVisit = 1;
		pstTransNode->ulAckNoOld = pstTransNode->ulAckNo;
	}

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_timeout_msg_recv
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: 
 *******************************************************************************/
INT32 cpss_tftp_timeout_msg_recv(CPSS_TIMER_MSG_T *pstTimeOut)
{
	UINT32 ulTransId;
	INT32 lRet;
	CPSS_TFTP_TRANS_NODE_T *pstTransNode;

	ulTransId = pstTimeOut->ulPara;

	pstTransNode = cpss_tftp_transid_link_find(ulTransId);
	if (pstTransNode == NULL)
	{
		return CPSS_ERROR;
	}

	if (pstTransNode->ulTransSideFlag == CPSS_TFTP_SEND_SIDE)
	{
		lRet = cpss_tftp_send_side_timeout_deal(pstTransNode);
	}
	else
	{
		lRet = cpss_tftp_recv_side_timeout_deal(pstTransNode);
	}

	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_trans_req_recv
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明: 
 *******************************************************************************/
INT32 cpss_tftp_trans_req_recv(CPSS_TFTP_TRANS_REQ_MSG_T* pstTransReq)
{
	UINT32 ulTftpCmd;
	UINT32 ulTransId;
	UINT32 ulFd;
	INT32 lRet;
	CPSS_TFTP_TRANS_NODE_T stTranNode;
	CPSS_TFTP_TRANS_NODE_T *pstTransNode;

	ulTftpCmd = cpss_ntohl(pstTransReq->ulTftpCmd);
	ulTransId = cpss_ntohl(pstTransReq->ulTftpTransId);

	stTranNode.pvNext = NULL;
	stTranNode.stDestPhyAddr = pstTransReq->stSrcPhyAddr;
	stTranNode.ulAckNo = 0xFFFFFFFF;
	stTranNode.ulAckNoOld = 0;
	stTranNode.ulClientFlag = CPSS_TFTP_SERVER_FLAG;
	stTranNode.ulDataEndFlag = CPSS_TFTP_DATA_BEGIN;
	stTranNode.ulReadLen = 0;
	stTranNode.ulRecvDataLen = 0;
	stTranNode.ulSendDataLen = 0;
	stTranNode.ulSendNo = 0;
	stTranNode.ulSendNoOld = 0;
	stTranNode.ulTftpTransId = ulTransId;
	stTranNode.ulTimeVisit = 0;
	stTranNode.ulWriteBuf = 0;
	stTranNode.ulWriteBufLen = 0;
	stTranNode.ulWriteMedium = CPSS_TFTP_WRITE_MEDIUM_DISK;
	stTranNode.bRelWriteBuf = FALSE;

	if (ulTftpCmd == CPSS_FTP_CMD_PUT_FILE)
	{
		stTranNode.ulTransSideFlag = CPSS_TFTP_RECV_SIDE;

		strncpy(stTranNode.aucFileName, pstTransReq->aucServerFilePath,
				CPSS_FS_FILE_ABSPATH_LEN);

		ulFd = cpss_file_open(pstTransReq->aucServerFilePath, CPSS_FILE_ORDWR
				|CPSS_FILE_OCREATE | CPSS_FILE_OTRUNC | CPSS_FILE_OBINARY);

	}
	else
	{
		UINT32 ulLen = 0;

		stTranNode.ulTransSideFlag = CPSS_TFTP_SEND_SIDE;

		cpss_file_get_size(pstTransReq->aucServerFilePath, &ulLen);

		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_FS, CPSS_PRINT_IMPORTANT,
				"recv tftp trans req :server file %s length = %d  \n",
				pstTransReq->aucServerFilePath, ulLen);

		ulFd = cpss_file_open(pstTransReq->aucServerFilePath, CPSS_FILE_ORDONLY
				| CPSS_FILE_OBINARY);
	}

	if (ulFd == CPSS_FD_INVALID)
	{
		cpss_tftp_disconnect_msg_send(pstTransReq->stSrcPhyAddr, ulTransId,
				CPSS_ERR_FS_OPEN_FILE_FAIL);

		cpss_output(CPSS_MODULE_FS, CPSS_PRINT_FAIL,
				"tftp open server file failed.filename=%s\n",
				pstTransReq->aucServerFilePath);
		return CPSS_ERROR;
	}

	stTranNode.ulFd = ulFd;

	stTranNode.ulTimerId = cpss_timer_para_loop_set(CPSS_TFTP_TIMEOUT_TM_NO,
			CPSS_TFTP_TIMEOUT_INTERVAL, ulTransId);

	pstTransNode = cpss_tftp_transid_link_find(ulTransId);
	if (pstTransNode != NULL)
	{
		cpss_file_close(pstTransNode->ulFd);

		if (pstTransNode->ulTransSideFlag == CPSS_TFTP_RECV_SIDE)
		{
			cpss_file_delete(pstTransNode->aucFileName);
		}
		cpss_timer_para_delete(pstTransNode->ulTimerId);

		cpss_tftp_transid_link_del(ulTransId);
	}

	lRet = cpss_tftp_transid_link_add(&stTranNode);
	if (lRet != CPSS_OK)
	{

		cpss_tftp_disconnect_msg_send(pstTransReq->stSrcPhyAddr, ulTransId,
				lRet);

		cpss_timer_para_delete(stTranNode.ulTimerId);

		cpss_file_close(ulFd);

		if (stTranNode.ulTransSideFlag == CPSS_TFTP_RECV_SIDE)
		{
			cpss_file_delete(pstTransNode->aucFileName);
		}

		return CPSS_ERROR;
	}
	if (stTranNode.ulTransSideFlag == CPSS_TFTP_SEND_SIDE)
	{
		lRet = cpss_tftp_trans_data_send(ulTransId, 0);
	}

	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_tftp_trans_node_show
 * 功    能:
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
VOID cpss_tftp_trans_node_show()
{
	CPSS_TFTP_TRANS_NODE_T* pstTftpNode;

	pstTftpNode = (CPSS_TFTP_TRANS_NODE_T*) g_ulTftpTransLinkHigh;

	oams_shcmd_printf("high level tftp queue\n");

	oams_shcmd_printf("\n");

	while (pstTftpNode != NULL)
	{
		oams_shcmd_printf("\n****************************\n");
		oams_shcmd_printf("ulTftpTransId=%d\n", pstTftpNode->ulTftpTransId);
		oams_shcmd_printf("stDestPhyAddr=0x%x\n",
				*(UINT32*) &pstTftpNode->stDestPhyAddr);
		oams_shcmd_printf("ulAckNo=%d\n", pstTftpNode->ulAckNo);
		oams_shcmd_printf("ulAckNoOld=%d\n", pstTftpNode->ulAckNoOld);
		oams_shcmd_printf("ulClientFlag=%d\n", pstTftpNode->ulClientFlag);
		oams_shcmd_printf("ulDataEndFlag=%d\n", pstTftpNode->ulDataEndFlag);
		oams_shcmd_printf("ulFd=%d\n", pstTftpNode->ulFd);
		oams_shcmd_printf("ulReadLen=%d\n", pstTftpNode->ulReadLen);
		oams_shcmd_printf("ulRecvDataLen=%d\n", pstTftpNode->ulRecvDataLen);
		oams_shcmd_printf("ulSendDataLen=%d\n", pstTftpNode->ulSendDataLen);
		oams_shcmd_printf("ulSendNo=%d\n", pstTftpNode->ulSendNo);
		oams_shcmd_printf("ulSendNoOld=%d\n", pstTftpNode->ulSendNoOld);
		oams_shcmd_printf("ulSerialID=%d\n", pstTftpNode->ulSerialID);
		oams_shcmd_printf("ulTimerId=%d\n", pstTftpNode->ulTimerId);
		oams_shcmd_printf("ulTimeVisit=%d\n", pstTftpNode->ulTimeVisit);
		oams_shcmd_printf("ulTransSideFlag=%d\n", pstTftpNode->ulTransSideFlag);
		oams_shcmd_printf("ulWriteBuf=%d\n", pstTftpNode->ulWriteBuf);
		oams_shcmd_printf("ulWriteBufLen=%d\n", pstTftpNode->ulWriteBufLen);
		oams_shcmd_printf("ulWriteMedium=%d\n", pstTftpNode->ulWriteMedium);

		pstTftpNode = (CPSS_TFTP_TRANS_NODE_T*) pstTftpNode->pvNext;
	}

	oams_shcmd_printf("\nlow level tftp queue\n");

	oams_shcmd_printf("\n");

	pstTftpNode = (CPSS_TFTP_TRANS_NODE_T*) g_ulTftpTransLinkLow;

	while (pstTftpNode != NULL)
	{
		oams_shcmd_printf("\n****************************\n");
		oams_shcmd_printf("ulTftpTransId=%d\n", pstTftpNode->ulTftpTransId);
		oams_shcmd_printf("stDestPhyAddr=0x%x\n",
				*(UINT32*) &pstTftpNode->stDestPhyAddr);
		oams_shcmd_printf("ulAckNo=%d\n", pstTftpNode->ulAckNo);
		oams_shcmd_printf("ulAckNoOld=%d\n", pstTftpNode->ulAckNoOld);
		oams_shcmd_printf("ulClientFlag=%d\n", pstTftpNode->ulClientFlag);
		oams_shcmd_printf("ulDataEndFlag=%d\n", pstTftpNode->ulDataEndFlag);
		oams_shcmd_printf("ulFd=%d\n", pstTftpNode->ulFd);
		oams_shcmd_printf("ulReadLen=%d\n", pstTftpNode->ulReadLen);
		oams_shcmd_printf("ulRecvDataLen=%d\n", pstTftpNode->ulRecvDataLen);
		oams_shcmd_printf("ulSendDataLen=%d\n", pstTftpNode->ulSendDataLen);
		oams_shcmd_printf("ulSendNo=%d\n", pstTftpNode->ulSendNo);
		oams_shcmd_printf("ulSendNoOld=%d\n", pstTftpNode->ulSendNoOld);
		oams_shcmd_printf("ulSerialID=%d\n", pstTftpNode->ulSerialID);
		oams_shcmd_printf("ulTimerId=%d\n", pstTftpNode->ulTimerId);
		oams_shcmd_printf("ulTimeVisit=%d\n", pstTftpNode->ulTimeVisit);
		oams_shcmd_printf("ulTransSideFlag=%d\n", pstTftpNode->ulTransSideFlag);
		oams_shcmd_printf("ulWriteBuf=%d\n", pstTftpNode->ulWriteBuf);
		oams_shcmd_printf("ulWriteBufLen=%d\n", pstTftpNode->ulWriteBufLen);
		oams_shcmd_printf("ulWriteMedium=%d\n", pstTftpNode->ulWriteMedium);

		pstTftpNode = (CPSS_TFTP_TRANS_NODE_T*) pstTftpNode->pvNext;
	}
}

/*******************************************************************************
 * 函数名称: cpss_tftp_req_dealing
 * 功    能: 处理接收到的FTP传输请求消息
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstTFTPReqMsg         CPSS_TFTP_REQ_MSG_T*     输入              TFTP纤程的状态
 * pstDstPid               CPSS_COM_PID_T *           输入              存储的纤程数据
 * 函数返回:
 *******************************************************************************/
#ifdef CPSS_TFTP_BY_SYSTEM
void cpss_tftp_req_dealing
(
		CPSS_TFTP_REQ_MSG_T* pstTFTPReqMsg,
		CPSS_COM_PID_T *pstDstPid
)
{
	INT32 lErrorNo = 0;
	CPSS_TFTP_RSP_MSG_T stTFTPResMsg;
	INT32 lComSend = 0;

	/*调用TFTP上传下载过程*/
#ifdef CPSS_VOS_WINDOWS
	lErrorNo = CPSS_OK;
#else
	lErrorNo = cpss_fs_tftp(pstTFTPReqMsg);
#endif
	/*组织响应包*/
	stTFTPResMsg.lErrorNo = lErrorNo;
	stTFTPResMsg.ulSerialID = pstTFTPReqMsg->ulSerialID;

	/*向对端纤程发送响应消息*/
	if(pstDstPid->ulAddrFlag != CPSS_COM_ADDRFLAG_PHYSICAL)
	{
		lComSend = cpss_com_send(pstDstPid,CPSS_TFTP_RSP_MSG,(UINT8*)&stTFTPResMsg,sizeof(CPSS_TFTP_RSP_MSG_T));
	}
	else
	{
		lComSend = cpss_com_send_phy(*(CPSS_COM_PHY_ADDR_T*)&pstDstPid->stLogicAddr,
				pstDstPid->ulPd,CPSS_TFTP_RSP_MSG,(UINT8*)&stTFTPResMsg,sizeof(CPSS_TFTP_RSP_MSG_T));
	}

	if( lComSend != CPSS_OK)
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_FAIL,
				"comSend (%d)(%d)(%d) failed.",pstDstPid->stLogicAddr,pstDstPid->ulPd,pstDstPid->ulAddrFlag);
	}
}
#else
extern INT32 cpss_com_ip2phy(UINT32 ulIpAddr, CPSS_COM_PHY_ADDR_T *pstPhyAddr);
void cpss_tftp_req_dealing(CPSS_TFTP_REQ_MSG_T* pstTFTPReqMsg,
		CPSS_COM_PID_T *pstDstPid)
{
	INT32 lRet;
	UINT32 ulFd;
	CPSS_TFTP_TRANS_NODE_T stTftpTransNode;

	/*填写节点内容*/
	stTftpTransNode.pvNext = NULL;
	stTftpTransNode.stDestPid = *pstDstPid;
	stTftpTransNode.ulAckNo = 0xFFFFFFFF;
	stTftpTransNode.ulAckNoOld = 0x0;
	stTftpTransNode.ulClientFlag = CPSS_TFTP_CLIENT_FLAG;
	stTftpTransNode.ulDataEndFlag = CPSS_TFTP_DATA_BEGIN;
	stTftpTransNode.ulReadLen = 0;
	stTftpTransNode.ulRecvDataLen = 0;
	stTftpTransNode.ulSendDataLen = 0;
	stTftpTransNode.ulSendNo = 0;
	stTftpTransNode.ulSendNoOld = 0;
	stTftpTransNode.ulSerialID = pstTFTPReqMsg->ulSerialID;
	stTftpTransNode.ulTimeVisit = 0;
	stTftpTransNode.bRelWriteBuf = FALSE;
	stTftpTransNode.ulWriteBuf = 0;

	cpss_com_ip2phy(pstTFTPReqMsg->ulServerIP, &stTftpTransNode.stDestPhyAddr);

	cpss_tftp_transid_get(&stTftpTransNode.ulTftpTransId);

	if (pstTFTPReqMsg->ulCmd == CPSS_FTP_CMD_PUT_FILE)
	{
		stTftpTransNode.ulTransSideFlag = CPSS_TFTP_SEND_SIDE;
		ulFd = cpss_file_open(pstTFTPReqMsg->aucClientFile, CPSS_FILE_ORDONLY
				|CPSS_FILE_OBINARY);
	}
	else if (pstTFTPReqMsg->ulCmd == CPSS_FTP_CMD_GET_FILE)
	{
		stTftpTransNode.ulTransSideFlag = CPSS_TFTP_RECV_SIDE;
		if (pstTFTPReqMsg->pucMem == NULL)
		{
			stTftpTransNode.ulWriteMedium = CPSS_TFTP_WRITE_MEDIUM_DISK;

			cpss_ftp_tmp_dirname_make(pstTFTPReqMsg->aucClientFile,
					CPSS_FS_FILE_ABSPATH_LEN);

			strncpy(stTftpTransNode.aucFileName, pstTFTPReqMsg->aucClientFile,
					CPSS_FS_FILE_ABSPATH_LEN);

			cpss_file_delete(pstTFTPReqMsg->aucClientFile);

			ulFd = cpss_file_open(pstTFTPReqMsg->aucClientFile, CPSS_FILE_ORDWR
					| CPSS_FILE_OCREATE | CPSS_FILE_OTRUNC | CPSS_FILE_OBINARY);
		}
		else
		{
			ulFd = 0;
			stTftpTransNode.ulWriteMedium = CPSS_TFTP_WRITE_MEDIUM_MEM;
			stTftpTransNode.ulWriteBuf = (UINT32) pstTFTPReqMsg->pucMem;
			stTftpTransNode.ulWriteBufLen = pstTFTPReqMsg->ulLen;
		}
	}
	else if (pstTFTPReqMsg->ulCmd == CPSS_FTP_CMD_GET_FILE_EXP)
	{
		stTftpTransNode.ulTransSideFlag = CPSS_TFTP_RECV_SIDE;
		stTftpTransNode.ulWriteMedium = CPSS_TFTP_WRITE_MEDIUM_MEM;
		strncpy(stTftpTransNode.aucFileName, pstTFTPReqMsg->aucClientFile,
				CPSS_FS_FILE_ABSPATH_LEN);

		stTftpTransNode.ulWriteBuf
				= (UINT32) cpss_mem_malloc(pstTFTPReqMsg->ulLen );
		if (stTftpTransNode.ulWriteBuf == 0)
		{
			cpss_tftp_rsp_msg_send(&stTftpTransNode.stDestPid,
					CPSS_ERR_FS_MALLOC_FAIL, stTftpTransNode.ulSerialID);

			cpss_output(CPSS_MODULE_FS, CPSS_PRINT_FAIL,
					"tftp open clien file failed not enough memory\n",
					pstTFTPReqMsg->aucClientFile);
			return;
		}
		stTftpTransNode.ulWriteBufLen = pstTFTPReqMsg->ulLen;
		stTftpTransNode.bRelWriteBuf = TRUE;
		cpss_file_delete(pstTFTPReqMsg->aucClientFile);
		ulFd = cpss_file_open(pstTFTPReqMsg->aucClientFile, CPSS_FILE_ORDWR
				|CPSS_FILE_OCREATE | CPSS_FILE_OTRUNC | CPSS_FILE_OBINARY);

	}

	if (ulFd == CPSS_FD_INVALID)
	{
		if (pstTFTPReqMsg->ulCmd == CPSS_FTP_CMD_GET_FILE_EXP)
		{
			stTftpTransNode.bRelWriteBuf = FALSE;
			cpss_mem_free((VOID *)stTftpTransNode.ulWriteBuf );

		}
		cpss_tftp_rsp_msg_send(&stTftpTransNode.stDestPid,
				CPSS_ERR_FS_OPEN_FILE_FAIL, stTftpTransNode.ulSerialID);

		cpss_output(CPSS_MODULE_FS, CPSS_PRINT_FAIL,
				"tftp open clien file failed\n", pstTFTPReqMsg->aucClientFile);
		return;
	}

	stTftpTransNode.ulFd = ulFd;
	stTftpTransNode.ulTimerId = cpss_timer_para_loop_set(
			CPSS_TFTP_TIMEOUT_TM_NO, CPSS_TFTP_TIMEOUT_INTERVAL,
			stTftpTransNode.ulTftpTransId);

	lRet = cpss_tftp_transid_link_add(&stTftpTransNode);
	if (lRet != CPSS_OK)
	{

		if (pstTFTPReqMsg->ulCmd == CPSS_FTP_CMD_GET_FILE_EXP)
		{
			stTftpTransNode.bRelWriteBuf = FALSE;

			cpss_mem_free((VOID *)stTftpTransNode.ulWriteBuf );

			cpss_file_close(stTftpTransNode.ulFd);
		}
		cpss_timer_para_delete(stTftpTransNode.ulTimerId);

		if (stTftpTransNode.ulWriteMedium == CPSS_TFTP_WRITE_MEDIUM_DISK)
		{
			cpss_file_close(stTftpTransNode.ulFd);
		}

		cpss_tftp_rsp_msg_send(&stTftpTransNode.stDestPid,
				CPSS_ERR_TFTP_NODE_ADD_FAIL, stTftpTransNode.ulSerialID);

		return;
	}

	lRet = cpss_tftp_trans_req_send(stTftpTransNode.stDestPhyAddr,
			pstTFTPReqMsg->ulCmd, stTftpTransNode.ulTftpTransId,
			pstTFTPReqMsg->aucServerFile);
	if (lRet != CPSS_OK)
	{

		if (pstTFTPReqMsg->ulCmd == CPSS_FTP_CMD_GET_FILE_EXP)
		{
			stTftpTransNode.bRelWriteBuf = FALSE;

			cpss_mem_free((VOID *)stTftpTransNode.ulWriteBuf );

			cpss_file_close(stTftpTransNode.ulFd);
		}
		cpss_timer_para_delete(stTftpTransNode.ulTimerId);

		if (stTftpTransNode.ulWriteMedium == CPSS_TFTP_WRITE_MEDIUM_DISK)
		{
			cpss_file_close(stTftpTransNode.ulFd);
		}

		cpss_tftp_transid_link_del(stTftpTransNode.ulTftpTransId);

		cpss_tftp_rsp_msg_send(&stTftpTransNode.stDestPid,
				CPSS_ERR_TFTP_TRANS_FAIL, stTftpTransNode.ulSerialID);
	}
	else
	{
		cpss_print(
				SWP_SUBSYS_CPSS,
				CPSS_MODULE_FS,
				CPSS_PRINT_IMPORTANT,
				"recv tftp trans req :server file %s length = %d  client file %s \n",
				pstTFTPReqMsg->aucServerFile, pstTFTPReqMsg->ulLen,
				pstTFTPReqMsg->aucClientFile);

	}

	if (stTftpTransNode.ulTransSideFlag == CPSS_TFTP_SEND_SIDE)
	{
		lRet = cpss_tftp_trans_data_send(stTftpTransNode.ulTftpTransId, 0);
	}
}
#endif

/*******************************************************************************
 * 函数名称: cpss_ftp_req_dealing
 * 功    能: 处理接收到的FTP传输请求消息
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFTPReqMsg         CPSS_FTP_REQ_MSG_T*     输入              FTP纤程的状态
 * pstDstPid               CPSS_COM_PID_T           输入              存储的纤程数据
 * 函数返回:
 * 说   明:
 *******************************************************************************/
void cpss_ftp_req_dealing(CPSS_FTP_REQ_MSG_T* pstFTPReqMsg,
		CPSS_COM_PID_T *pstDstPid)
{
	printf("involking function cpss_ftp_req_dealing  which omitted");
#if 	0
	INT32 lErrorNo = 0;
	CPSS_FTP_RSP_MSG_T stFTPResMsg;
	INT32 lComSend = 0;

	lErrorNo = cpss_fs_ftp(pstFTPReqMsg,pstDstPid->ulPd);

	stFTPResMsg.lErrorNo = lErrorNo;
	stFTPResMsg.ulSerialID = pstFTPReqMsg->ulSerialID;

	if(pstDstPid->ulAddrFlag != CPSS_COM_ADDRFLAG_PHYSICAL)
	{
		lComSend = cpss_com_send(pstDstPid,CPSS_FTP_RSP_MSG,(UINT8*)&stFTPResMsg,sizeof(CPSS_FTP_RSP_MSG_T));
	}
	else
	{
		lComSend = cpss_com_send_phy(*(CPSS_COM_PHY_ADDR_T*)&pstDstPid->stLogicAddr,
				pstDstPid->ulPd,CPSS_FTP_RSP_MSG,(UINT8*)&stFTPResMsg,sizeof(CPSS_FTP_RSP_MSG_T));
	}

	if( lComSend != CPSS_OK)
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
				"comSend (%d)(%d)(%d) failed.",pstDstPid->stLogicAddr,pstDstPid->ulPd,pstDstPid->ulAddrFlag);
	}
#endif
}

/*******************************************************************************
 * 函数名称: cpss_fs_deal_rsp_send
 * 功    能: 发送响应消息函数
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_deal_rsp_send(UINT8* pucSendBuf, UINT32 ulSendLen,
		UINT32 ulMsgId, CPSS_COM_PID_T* pstDstPid)
{
	INT32 lComSend;

	if (pstDstPid->ulAddrFlag != CPSS_COM_ADDRFLAG_PHYSICAL)
	{
		lComSend = cpss_com_send(pstDstPid, ulMsgId, pucSendBuf, ulSendLen);
	}
	else
	{
		lComSend = cpss_com_send_phy(
				*(CPSS_COM_PHY_ADDR_T*) &pstDstPid->stLogicAddr,
				pstDstPid->ulPd, ulMsgId, pucSendBuf, ulSendLen);
	}

	if (CPSS_OK != lComSend)
	{
		cpss_output(
				CPSS_MODULE_FS,
				CPSS_PRINT_WARN,
				"comSend (logaddr=0x%x)(pid=%d)(addrflag=%d)(msgid=0x%x) failed.",
				pstDstPid->stLogicAddr, pstDstPid->ulPd, pstDstPid->ulAddrFlag,
				ulMsgId);
	}

	return lComSend;
}

/*******************************************************************************
 * 函数名称: cpss_fs_file_read_req_deal
 * 功    能: 处理接收到的文件读请求消息
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFileReadReq   CPSS_FS_FILE_READ_REQ_T*  输入       文件读请求消息
 * pstDstPid        CPSS_COM_PID_T*           输入       目的纤程的PID
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_file_read_req_deal(CPSS_FS_FILE_READ_REQ_T* pstFileReadReq,
		CPSS_COM_PID_T* pstDstPid)
{
	INT32 lRet = CPSS_OK;
	UINT32 ulFd;
	UINT32 ulSeekPos;
	UINT32 ulReadLen;
	CPSS_FS_FILE_READ_RSP_T stFileReadRsp;

	/*参数校验*/
	if ((pstFileReadReq == NULL) || (pstDstPid == NULL)
			|| (pstFileReadReq->pvBuf == NULL))
	{
		lRet = CPSS_ERR_FS_PARA_ILLEGAL;
	}

	/*打开文件*/
	if (CPSS_OK == lRet)
	{
		ulFd = cpss_file_open(pstFileReadReq->aucFileName, CPSS_FILE_OBINARY
				|CPSS_FILE_ORDONLY);
		if (CPSS_FD_INVALID == ulFd)
		{
			lRet = CPSS_FD_INVALID;
		}
	}

	/*seek文件*/
	if (CPSS_OK == lRet)
	{
		lRet = cpss_file_seek(ulFd, pstFileReadReq->lOffset,
				pstFileReadReq->ulOrigin, &ulSeekPos);
	}

	/*读文件*/
	if (CPSS_OK == lRet)
	{
		lRet = cpss_file_read(ulFd, pstFileReadReq->pvBuf,
				pstFileReadReq->ulMaxLen, &ulReadLen);
	}

	/*关闭文件*/
	if (CPSS_OK == lRet)
	{
		cpss_file_close(ulFd);
	}

	/*组织响应消息*/
	strncpy(stFileReadRsp.aucFileName, pstFileReadReq->aucFileName,
			CPSS_FS_FILE_ABSPATH_LEN);

	stFileReadRsp.pvBuf = pstFileReadReq->pvBuf;

	stFileReadRsp.ulContext = pstFileReadReq->ulContext;

	stFileReadRsp.ulMaxLen = pstFileReadReq->ulMaxLen;

	stFileReadRsp.ulReadLen = ulReadLen;

	if (lRet == CPSS_OK)
	{
		/*如果实际读取的数据长度小于请求读取的数据长度*/
		if (ulReadLen < pstFileReadReq->ulMaxLen)
		{
			stFileReadRsp.ulResult = CPSS_ERR_FS_READ_LEN_NOT_ENOUGH;
		}
		else
		{
			stFileReadRsp.ulResult = CPSS_OK;
		}
	}
	else
	{
		stFileReadRsp.ulResult = lRet;
	}

	/*发送响应消息*/
	lRet = cpss_fs_deal_rsp_send((UINT8*) &stFileReadRsp,
			sizeof(CPSS_FS_FILE_READ_RSP_T), CPSS_FS_FILE_READ_RSP_MSG,
			pstDstPid);

	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_fs_file_write_req_deal
 * 功    能: 处理接收到的文件写请求消息
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFileWriteReq   CPSS_FS_FILE_WRITE_REQ_T*  输入       文件写请求消息
 * pstDstPid        CPSS_COM_PID_T*             输入       目的纤程的PID
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_file_write_req_deal(CPSS_FS_FILE_WRITE_REQ_T* pstFileWriteReq,
		CPSS_COM_PID_T* pstDstPid)
{
	INT32 lRet = CPSS_OK;
	UINT32 ulFd;
	UINT32 ulSeekPos;
	UINT32 ulWriteLen;
	CPSS_FS_FILE_WRITE_RSP_T stFileWriteRsp;

	/*参数校验*/
	if ((pstFileWriteReq == NULL) || (pstDstPid == NULL)
			|| (pstFileWriteReq->pvBuf == NULL))
	{
		lRet = CPSS_ERR_FS_PARA_ILLEGAL;
	}

	/*打开文件(如果文件不存在则创建文件)*/
	if (CPSS_OK == lRet)
	{
		ulFd = cpss_file_open(pstFileWriteReq->aucFileName,
				pstFileWriteReq->lOpenFlag);
		if (CPSS_FD_INVALID == ulFd)
		{
			lRet = CPSS_FD_INVALID;
		}
	}

	/*seek文件*/
	if (CPSS_OK == lRet)
	{
		lRet = cpss_file_seek(ulFd, pstFileWriteReq->lOffset,
				pstFileWriteReq->ulOrigin, &ulSeekPos);
	}

	/*读文件*/
	if (CPSS_OK == lRet)
	{
		lRet = cpss_file_write(ulFd, pstFileWriteReq->pvBuf,
				pstFileWriteReq->ulBufLen, &ulWriteLen);
	}

	/*关闭文件*/
	if (CPSS_OK == lRet)
	{
		cpss_file_close(ulFd);
	}

	/*组织响应消息*/
	strncpy(stFileWriteRsp.aucFileName, pstFileWriteReq->aucFileName,
			CPSS_FS_FILE_ABSPATH_LEN);

	stFileWriteRsp.pvBuf = pstFileWriteReq->pvBuf;

	stFileWriteRsp.ulContext = pstFileWriteReq->ulContext;

	stFileWriteRsp.ulWriteLen = ulWriteLen;

	if (lRet == CPSS_OK)
	{
		/*如果实际读取的数据长度小于请求读取的数据长度*/
		if (ulWriteLen < pstFileWriteReq->ulBufLen)
		{
			stFileWriteRsp.ulResult = CPSS_ERR_FS_WRITE_LEN_NOT_ENOUGH;
		}
		else
		{
			stFileWriteRsp.ulResult = CPSS_OK;
		}
	}
	else
	{
		stFileWriteRsp.ulResult = lRet;
	}

	/*发送响应消息*/
	lRet = cpss_fs_deal_rsp_send((UINT8*) &stFileWriteRsp,
			sizeof(CPSS_FS_FILE_WRITE_RSP_T), CPSS_FS_FILE_WRITE_RSP_MSG,
			pstDstPid);

	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_fs_file_copy_req_deal
 * 功    能: 处理接收到的文件拷贝请求消息
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFileCopyReq   CPSS_FS_FILE_COPY_REQ_T*  输入       文件copy请求消息
 * pstDstPid        CPSS_COM_PID_T*           输入       目的纤程的PID
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_file_copy_req_deal(CPSS_FS_FILE_COPY_REQ_T* pstFileCopyReq,
		CPSS_COM_PID_T* pstDstPid)
{
	INT32 lRet = CPSS_OK;
	CPSS_FS_FILE_COPY_RSP_T stFileCopyRsp;

	/*参数合法性判断*/
	if ((pstFileCopyReq->aucDstFileName == NULL)
			|| (pstFileCopyReq->aucSrcFileName == NULL))
	{
		lRet = CPSS_ERR_FS_PARA_ILLEGAL;
	}

	/*拷贝文件*/
	if (CPSS_OK == lRet)
	{
		lRet = cpss_file_copy(pstFileCopyReq->aucSrcFileName,
				pstFileCopyReq->aucDstFileName);
	}

	/*组织响应消息*/
	strncpy(stFileCopyRsp.aucDstFileName, pstFileCopyReq->aucDstFileName,
			CPSS_FS_FILE_ABSPATH_LEN);
	strncpy(stFileCopyRsp.aucSrcFileName, pstFileCopyReq->aucSrcFileName,
			CPSS_FS_FILE_ABSPATH_LEN);

	stFileCopyRsp.ulContext = pstFileCopyReq->ulContext;
	stFileCopyRsp.ulResult = lRet;

	lRet = cpss_fs_deal_rsp_send((UINT8*) &stFileCopyRsp,
			sizeof(CPSS_FS_FILE_COPY_RSP_T), CPSS_FS_FILE_COPY_RSP_MSG,
			pstDstPid);

	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_fs_file_move_req_deal
 * 功    能: 处理接收到的文件移动请求消息
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFileMoveReq   CPSS_FS_FILE_MOVE_REQ_T*  输入       文件移动请求消息
 * pstDstPid        CPSS_COM_PID_T*           输入       目的纤程的PID
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_file_move_req_deal(CPSS_FS_FILE_MOVE_REQ_T* pstFileMoveReq,
		CPSS_COM_PID_T* pstDstPid)
{
	INT32 lRet = CPSS_OK;
	printf(
			"involking function cpss_fs_file_move_req_deal which omitted");
#if 	0
	CPSS_FS_FILE_MOVE_RSP_T stFileMoveRsp;

	/*参数合法性判断*/
	if((pstFileMoveReq->aucDstFileName == NULL)
			||(pstFileMoveReq->aucSrcFileName == NULL))
	{
		lRet = CPSS_ERR_FS_PARA_ILLEGAL;
	}

	if(CPSS_OK == lRet)
	{
		lRet = cpss_file_move(pstFileMoveReq->aucSrcFileName,
				pstFileMoveReq->aucDstFileName);
	}

	strncpy(stFileMoveRsp.aucDstFileName,
			pstFileMoveReq->aucDstFileName,CPSS_FS_FILE_ABSPATH_LEN);
	strncpy(stFileMoveRsp.aucSrcFileName,
			pstFileMoveReq->aucSrcFileName,CPSS_FS_FILE_ABSPATH_LEN);

	stFileMoveRsp.ulContext = pstFileMoveReq->ulContext;
	stFileMoveRsp.ulResult = lRet;

	lRet = cpss_fs_deal_rsp_send((UINT8*)&stFileMoveRsp,
			sizeof(CPSS_FS_FILE_MOVE_RSP_T),CPSS_FS_FILE_MOVE_RSP_MSG,pstDstPid);
#endif
	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_fs_dir_copy_req_deal
 * 功    能: 处理接收到的目录复制请求消息
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstDirCopyReq   CPSS_FS_DIR_COPY_REQ_T  输入       目录拷贝请求消息
 * pstDstPid        CPSS_COM_PID_T*        输入       目的纤程的PID
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_dir_copy_req_deal(CPSS_FS_DIR_COPY_REQ_T* pstDirCopyReq,
		CPSS_COM_PID_T* pstDstPid)
{
	INT32 lRet = CPSS_OK;
	//printf("involking function cpss_fs_dir_copy_req_deal which omitted");
	CPSS_FS_DIR_COPY_RSP_T stDirCopyRsp;

	if ((pstDirCopyReq->aucDstFileName == NULL)
			|| (pstDirCopyReq->aucSrcFileName == NULL))
	{
		lRet = CPSS_ERR_FS_PARA_ILLEGAL;
	}

	if (CPSS_OK == lRet)
	{
		lRet = cpss_dir_copy(pstDirCopyReq->aucSrcFileName,
				pstDirCopyReq->aucDstFileName);
	}

	strncpy(stDirCopyRsp.aucDstFileName, pstDirCopyReq->aucDstFileName,
			CPSS_FS_FILE_ABSPATH_LEN);
	strncpy(stDirCopyRsp.aucSrcFileName, pstDirCopyReq->aucSrcFileName,
			CPSS_FS_FILE_ABSPATH_LEN);

	stDirCopyRsp.ulContext = pstDirCopyReq->ulContext;
	stDirCopyRsp.ulResult = lRet;

	lRet
			= cpss_fs_deal_rsp_send((UINT8*) &stDirCopyRsp,
					sizeof(CPSS_FS_DIR_COPY_RSP_T), CPSS_FS_DIR_COPY_RSP_MSG,
					pstDstPid);

	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_fs_dir_del_req_deal
 * 功    能: 处理接收到的目录删除请求消息
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstDirDelReq   CPSS_FS_DIR_DEL_REQ_T  输入       目录删除请求消息
 * pstDstPid      CPSS_COM_PID_T*        输入       目的纤程的PID
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_dir_del_req_deal(CPSS_FS_DIR_DEL_REQ_T* pstDirDelReq,
		CPSS_COM_PID_T* pstDstPid)
{
	INT32 lRet = CPSS_OK;
	CPSS_FS_DIR_DEL_RSP_T stDirDelRsp;

	if (pstDirDelReq->aucDirPathName == NULL)
	{
		lRet = CPSS_ERR_FS_PARA_ILLEGAL;
	}

	if (CPSS_OK == lRet)
	{
		lRet = cpss_dir_delete(pstDirDelReq->aucDirPathName);
	}

	if (CPSS_OK == lRet)
	{
		lRet = cpss_dir_create(pstDirDelReq->aucDirPathName);
	}

	strncpy(stDirDelRsp.aucDirPathName, pstDirDelReq->aucDirPathName,
			CPSS_FS_FILE_ABSPATH_LEN);

	stDirDelRsp.ulContext = pstDirDelReq->ulContext;
	stDirDelRsp.ulResult = lRet;

	lRet = cpss_fs_deal_rsp_send((UINT8*) &stDirDelRsp,
			sizeof(CPSS_FS_DIR_DEL_RSP_T), CPSS_FS_DIR_DEL_RSP_MSG, pstDstPid);

	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_fs_file_del_req_deal
 * 功    能: 处理接收到的文件删除请求消息
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * pstFileDelReq   CPSS_FS_FILE_DEL_REQ_T  输入       文件删除请求消息
 * pstDstPid      CPSS_COM_PID_T*          输入       目的纤程的PID
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_file_del_req_deal(CPSS_FS_FILE_DEL_REQ_T* pstFileDelReq,
		CPSS_COM_PID_T* pstDstPid)
{
	INT32 lRet = CPSS_OK;
	CPSS_FS_FILE_DEL_RSP_T stFileDelRsp;

	if ((pstFileDelReq == NULL) || (pstDstPid == NULL))
	{
		lRet = CPSS_ERR_FS_PARA_ILLEGAL;
	}

	if (CPSS_OK == lRet)
	{
		lRet = cpss_file_delete(pstFileDelReq->aucFilePathName);
	}

	strncpy(stFileDelRsp.aucFilePathName, pstFileDelReq->aucFilePathName,
			CPSS_FS_FILE_ABSPATH_LEN);
	stFileDelRsp.ulContext = pstFileDelReq->ulContext;
	stFileDelRsp.ulResult = lRet;

	lRet
			= cpss_fs_deal_rsp_send((UINT8*) &stFileDelRsp,
					sizeof(CPSS_FS_FILE_DEL_RSP_T), CPSS_FS_FILE_DEL_RSP_MSG,
					pstDstPid);

	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_init_tnbs
 * 功    能: TNBS的初始化函数
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_init_tnbs()
{
	CPSS_FTP_INFO_MAN_T* pstFtpMan;

	pstFtpMan = (CPSS_FTP_INFO_MAN_T*) cpss_vk_proc_user_data_get();
	if (pstFtpMan == NULL)
	{
		return CPSS_ERROR;
	}

	cpss_mem_memset(pstFtpMan,0,sizeof(CPSS_FTP_INFO_MAN_T));

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_info_node_fill
 * 功    能: 将FTP的信息填入链表的节点
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
VOID cpss_ftp_info_node_fill(CPSS_FTP_INFO_NODE_T *pstFtpInfoNode,
		CPSS_FTP_REQ_MSG_T* pstFTPReqMsg, CPSS_COM_PID_T* pstDstPid,
		UINT8* pucServerFilePath, UINT8* pucClientFilePath, UINT32 ulSerialId)
{
	pstFtpInfoNode->ulSerialId = ulSerialId;

	pstFtpInfoNode->ulUsrSeq = pstFTPReqMsg->ulSerialID;
	pstFtpInfoNode->ulFtpCmd = pstFTPReqMsg->ulFtpCmd;
	pstFtpInfoNode->ulProgessStep = pstFTPReqMsg->ulProgessStep;
	pstFtpInfoNode->ulServerIp = pstFTPReqMsg->ulServerIP;

	strncpy(pstFtpInfoNode->aucServerFilePathName, pucServerFilePath,
			CPSS_FS_FILE_ABSPATH_LEN);

	strncpy(pstFtpInfoNode->aucClientFilePathName, pucClientFilePath,
			CPSS_FS_FILE_ABSPATH_LEN);

	pstFtpInfoNode->stDstPid = *pstDstPid;
	pstFtpInfoNode->pucMem = pstFTPReqMsg->pucMem;
	pstFtpInfoNode->ulLen = pstFTPReqMsg->ulLen;

	pstFtpInfoNode->ulDataDealNum = 0;
	pstFtpInfoNode->ulBeginTick = cpss_tick_get();
	pstFtpInfoNode->ulStepVal = pstFTPReqMsg->ulProgessStep;
	pstFtpInfoNode->ulProStat = CPSS_FTP_STATE_PACKING;

	//   strncpy(pstFtpInfoNode->aucPasswd,pstFTPReqMsg->aucPasswd,TNBS_FTP_PASSWORD_LEN) ;
	//   strncpy(pstFtpInfoNode->aucUsrName,pstFTPReqMsg->aucUserName,TNBS_FTP_USERNAME_LEN) ;

	pstFtpInfoNode->pvNext = NULL;
	pstFtpInfoNode->ulPhase = CPSS_FTP_INIT_PHASE;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_info_node_pre_deal
 * 功    能: 处理FTP链表的前一个节点
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
#if		0 
INT32 cpss_ftp_info_node_pre_deal
(
		CPSS_FTP_INFO_NODE_T *pstFtpInfoNode
)
{
	printf("involking function cpss_ftp_info_node_pre_deal which commited*********************************");

	INT32 lRet;
	UINT8 aucZipDstFileName[CPSS_FS_FILE_ABSPATH_LEN];

	cpss_mem_memcpy(aucZipDstFileName,pstFtpInfoNode->aucClientFilePathName,CPSS_FS_FILE_ABSPATH_LEN);

	lRet = cpss_ftp_tmp_dirname_make(aucZipDstFileName,CPSS_FS_FILE_ABSPATH_LEN);
	if(CPSS_OK != lRet)
	{
		return lRet;
	}

	if(pstFtpInfoNode->ulFtpCmd == CPSS_FTP_CMD_PUT_DIR_PACK)
	{
		lRet = cpss_file_pack(pstFtpInfoNode->aucClientFilePathName,
				aucZipDstFileName,TRUE);
		if(CPSS_OK != lRet)
		{
			return CPSS_ERR_FS_PACK_FILE_FAIL;
		}
	}

	return CPSS_OK;
}
#endif
/*******************************************************************************
 * 函数名称: cpss_ftp_connect_req_send
 * 功    能: FTP连接请求的发送函数
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
#if 	0 
INT32 cpss_ftp_connect_req_send
(
		CPSS_FTP_INFO_NODE_T* pstFtpInfoNode
)
{
	TNBS_FTP_XFER_REQ_MSG_T stFtpXfer;
	struct in_addr stAddr;
	CPSS_COM_PID_T stLocalPid;
	INT32 lRet;
	printf("involking function cpss_ftp_connect_req_send which commited*********************************");

	stAddr.s_addr = cpss_htonl(pstFtpInfoNode->ulServerIp);
#ifdef CPSS_VOS_WINDOWS
	strncpy(stFtpXfer.ucHostIp,inet_ntoa(stAddr),TNBS_FTP_IPADDR_LEN);
#else
	inet_ntoa_b(stAddr, stFtpXfer.ucHostIp);
#endif

	if(pstFtpInfoNode->ulStepVal != 0)
	{
		stFtpXfer.pfStatusHook = cpss_ftp_process_ind_hook;
	}
	else
	{
		stFtpXfer.pfStatusHook = NULL;
	}
	cpss_mem_memset(stFtpXfer.ucAcct,0,TNBS_FTP_ACCOUNT_LEN);

	strncpy(stFtpXfer.ucCmd,CPSS_FTP_TYPE_STR,strlen(CPSS_FTP_TYPE_STR)+1);

	strncpy(stFtpXfer.ucPasswd,pstFtpInfoNode->aucPasswd,TNBS_FTP_PASSWORD_LEN);
	strncpy(stFtpXfer.ucUser,pstFtpInfoNode->aucUsrName,TNBS_FTP_USERNAME_LEN);

	stFtpXfer.ulCmdId = pstFtpInfoNode->ulSerialId;
	stFtpXfer.ulVal = (UINT32)pstFtpInfoNode;

	cpss_com_logic_addr_get(&stLocalPid.stLogicAddr,&stLocalPid.ulAddrFlag);
	stLocalPid.ulPd = TNBS_FTP_PROC;

	cpss_fs_htonl_xfer_req(&stFtpXfer);

	lRet = cpss_com_send(&stLocalPid,TNBS_FTP_XFER_REQ_MSG,
			(UINT8*)&stFtpXfer,sizeof(TNBS_FTP_XFER_REQ_MSG_T));

	return lRet;
}
#endif
/*******************************************************************************
 * 函数名称: cpss_ftp_process_ind_hook
 * 功    能: FTP传输进度指示的回调函数
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_process_ind_hook(UINT32 ulTotal, UINT32 ulCurent, UINT32 ulVal)
{
	CPSS_FTP_INFO_NODE_T* pstFtpInfoNode = NULL;
	CPSS_FTP_PROGRESS_IND_MSG_T stFtpProgressInd;
	UINT32 ulTickInterval;
	UINT32 ulBeginTick;
	UINT32 ulCurrTick;
	UINT32 ulStepVal;
	UINT32 ulDataDealNum;

	pstFtpInfoNode = (CPSS_FTP_INFO_NODE_T*) ulVal;

#if 0
	/*查找对应的节点*/
	pstFtpInfoNode = cpss_ftp_info_node_queue_find(ulSerialId,pstFtpMan);
	if(CPSS_OK != pstFtpInfoNode)
	{
		return CPSS_ERROR;
	}
#endif

	ulDataDealNum = pstFtpInfoNode->ulDataDealNum;
	ulStepVal = pstFtpInfoNode->ulStepVal;

	/*是否需要发送指示消息*/
	if ((ulCurent - ulDataDealNum) < ulStepVal)
	{
		return CPSS_OK;
	}

	pstFtpInfoNode->ulDataDealNum = ulCurent;

	/*计算执行时间*/
	ulBeginTick = pstFtpInfoNode->ulBeginTick;
	ulCurrTick = cpss_tick_get();
	if (ulCurrTick < ulBeginTick)
	{
		ulTickInterval = 0xFFFFFFFF - ulCurrTick + ulBeginTick;
	}
	else
	{
		ulTickInterval = ulCurrTick - ulBeginTick;
	}

	/*填充消息体*/
	stFtpProgressInd.ulDealingSize = ulCurent;
	stFtpProgressInd.ulSerialID = pstFtpInfoNode->ulUsrSeq;
	stFtpProgressInd.ulTotalSize = ulTotal;
	stFtpProgressInd.ulUsedTime = ulTickInterval;
	stFtpProgressInd.ulState = pstFtpInfoNode->ulProStat;

	cpss_ftp_msg_send(&pstFtpInfoNode->stDstPid, (UINT8*) &stFtpProgressInd,
			CPSS_FTP_PROGRESS_IND_MSG, sizeof(CPSS_FTP_PROGRESS_IND_MSG_T));

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_info_index_alloc()
 * 功    能: FTP传输序号的申请，必须是唯一的
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_info_index_alloc(UINT32 *pulSerialId)
{
	/*生成FTP下载序号*/
	cpss_vos_mutex_p(g_ulCpssFtpMutex, CPSS_VOS_MUTEX_WAIT_FOREVER);
	g_ulCpssFtpSerialBase++;
	cpss_vos_mutex_v(g_ulCpssFtpMutex);

	*pulSerialId = g_ulCpssFtpSerialBase;
	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_info_node_queue_join
 * 功    能: 将FTP请求信息节点链入链表
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_info_node_queue_join(CPSS_FTP_INFO_NODE_T* pstFtpInfoNode)
{
	CPSS_FTP_INFO_MAN_T* pstFtpMan = NULL;
	CPSS_FTP_INFO_NODE_T* pstFtpNodeAlloc = NULL;
	CPSS_FTP_INFO_NODE_T* pstFtpNodeCur = NULL;
	CPSS_FTP_INFO_NODE_T* pstFtpNodePre = NULL;

	if (pstFtpInfoNode == NULL)
	{
		return CPSS_ERROR;
	}

	pstFtpMan = (CPSS_FTP_INFO_MAN_T*) cpss_vk_proc_user_data_get();
	if (pstFtpMan == NULL)
	{
		return CPSS_ERROR;
	}

	pstFtpNodeAlloc = cpss_mem_malloc(sizeof(CPSS_FTP_INFO_NODE_T));
	if (pstFtpNodeAlloc == NULL)
	{
		return CPSS_ERROR;
	}

	cpss_mem_memcpy(pstFtpNodeAlloc,pstFtpInfoNode,sizeof(CPSS_FTP_INFO_NODE_T));

	if (pstFtpMan->pstFtpHdr == NULL)
	{
		pstFtpMan->pstFtpHdr = pstFtpNodeAlloc;
		pstFtpNodeAlloc->pvNext = NULL;

		return CPSS_OK;
	}
	else
	{
		pstFtpNodeAlloc->pvNext = NULL;
	}

	pstFtpNodeCur = pstFtpMan->pstFtpHdr;

	while (pstFtpNodeCur != NULL)
	{
		pstFtpNodePre = pstFtpNodeCur;
		pstFtpNodeCur = (CPSS_FTP_INFO_NODE_T*) pstFtpNodeCur->pvNext;
	}

	pstFtpNodePre->pvNext = pstFtpNodeAlloc;

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_info_node_queue_delete
 * 功    能: 将FTP传输请求信息从链表中删除
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_info_node_queue_delete(UINT32 ulSerialId)
{
	CPSS_FTP_INFO_NODE_T* pstFtpNode = NULL;
	CPSS_FTP_INFO_NODE_T* pstFtpNodePre = NULL;
	CPSS_FTP_INFO_MAN_T* pstFtpMan = NULL;

	/*得到管理结构指针*/
	pstFtpMan = (CPSS_FTP_INFO_MAN_T*) cpss_vk_proc_user_data_get();
	if (pstFtpMan == NULL)
	{
		return CPSS_ERROR;
	}

	pstFtpNode = pstFtpMan->pstFtpHdr;

	/*查找对应的FTP节点*/
	while (pstFtpNode != NULL)
	{
		if (pstFtpNode->ulSerialId == ulSerialId)
		{
			if (pstFtpNode == pstFtpMan->pstFtpHdr)
			{
				pstFtpMan->pstFtpHdr = pstFtpNode->pvNext;
			}
			else
			{
				pstFtpNodePre->pvNext = pstFtpNode->pvNext;
			}

			cpss_mem_free(pstFtpNode);

			return CPSS_OK;
		}

		pstFtpNodePre = pstFtpNode;
		pstFtpNode = pstFtpNode->pvNext;
	}

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_info_node_queue_find
 * 功    能: 在链表中查找对应的FTP信息
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
CPSS_FTP_INFO_NODE_T* cpss_ftp_info_node_queue_find(UINT32 ulSerialId,
		CPSS_FTP_INFO_MAN_T* pstFtpMan)
{
	CPSS_FTP_INFO_NODE_T* pstFtpInfoNode;
	CPSS_FTP_INFO_MAN_T* pstFtpManLocal = NULL;

	/*得到管理结构指针*/
	if (pstFtpMan == NULL)
	{
		pstFtpManLocal = (CPSS_FTP_INFO_MAN_T*) cpss_vk_proc_user_data_get();
		if (pstFtpManLocal == NULL)
		{
			return NULL;
		}
	}
	else
	{
		pstFtpManLocal = pstFtpMan;
	}

	pstFtpInfoNode = pstFtpManLocal->pstFtpHdr;

	/*查找对应的FTP节点*/
	while (pstFtpInfoNode != NULL)
	{
		if (pstFtpInfoNode->ulSerialId == ulSerialId)
		{
			return pstFtpInfoNode;
		}

		pstFtpInfoNode = pstFtpInfoNode->pvNext;
	}

	cpss_output(CPSS_MODULE_FS, CPSS_PRINT_INFO,
			"cpss_ftp_info_node_queue_find queue null\n");

	return NULL;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_msg_send
 * 功    能: ftp相关消息的发送函数
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_msg_send(CPSS_COM_PID_T* pstDstPid, UINT8 *pucSendData,
		UINT32 ulMsgId, UINT32 ulSendLen)
{
	INT32 lRet = CPSS_OK;

	/*参数检查*/
	if ((pstDstPid == NULL) || ((ulSendLen > 0) && (pucSendData == NULL)))
	{
		return CPSS_ERROR;
	}

	/*发送ftp消息*/
	if (pstDstPid->ulAddrFlag != CPSS_COM_ADDRFLAG_PHYSICAL)
	{
		lRet = cpss_com_send(pstDstPid, ulMsgId, pucSendData, ulSendLen);
	}
	else
	{
		lRet = cpss_com_send_phy(
				*(CPSS_COM_PHY_ADDR_T*) &pstDstPid->stLogicAddr,
				pstDstPid->ulPd, ulMsgId, pucSendData, ulSendLen);
	}

	if (lRet != CPSS_OK)
	{
		cpss_output(CPSS_MODULE_FS, CPSS_PRINT_WARN,
				"ftp send (0x%x)(%d)(%d) failed!",
				*(UINT32*) &pstDstPid->stLogicAddr, pstDstPid->ulPd,
				pstDstPid->ulAddrFlag);
		return lRet;
	}

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_rsp_send
 * 功    能: ftp响应消息的发送函数
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_rsp_send(INT32 lResult, UINT32 ulUsrSeq,
		CPSS_COM_PID_T* pstDstPid)
{
	CPSS_FTP_RSP_MSG_T stFTPResMsg;
	INT32 lRet;

	/*填充响应消息*/
	stFTPResMsg.lErrorNo = lResult;
	stFTPResMsg.ulSerialID = ulUsrSeq;

	lRet = cpss_ftp_msg_send(pstDstPid, (UINT8*) &stFTPResMsg,
			CPSS_FTP_RSP_MSG, sizeof(CPSS_FTP_RSP_MSG_T));

	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_req_dealing_tnbs
 * 功    能: 通过TNBS进行FTP传输的FTP请求处理过程
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_req_dealing_tnbs(CPSS_FTP_REQ_MSG_T* pstFTPReqMsg,
		CPSS_COM_PID_T *pstDstPid)
{
	UINT32 ulDstIp = 0;
	UINT32 ulSerialId = 0;
	CPSS_FTP_INFO_NODE_T stFtpInfoNode;
	INT32 lRet;

	UINT8 aucServerFilePathName[CPSS_FS_FILE_ABSPATH_LEN];
	UINT8 aucClientFilePathName[CPSS_FS_FILE_ABSPATH_LEN];

	/*参数判断*/
	if ((pstFTPReqMsg == NULL) || (pstDstPid == NULL))
	{
		return CPSS_ERR_FS_PARA_ILLEGAL;
	}

	/*拷贝文件名*/
	strncpy(aucServerFilePathName, pstFTPReqMsg->aucServerFile,
			CPSS_FS_FILE_ABSPATH_LEN);
	strncpy(aucClientFilePathName, pstFTPReqMsg->aucClientFile,
			CPSS_FS_FILE_ABSPATH_LEN);

	/*检查文件名称*/
	if ((CPSS_OK != cpss_filename_check(aucServerFilePathName, FALSE))
			|| (CPSS_OK != cpss_filename_check(aucClientFilePathName, TRUE)))
	{
		return CPSS_ERR_FS_FILENAME_ILLEGAL;
	}

	/*得到FTP的序列号*/
	cpss_ftp_info_index_alloc(&ulSerialId);

	/*填充队列节点内容*/
	cpss_ftp_info_node_fill(&stFtpInfoNode, pstFTPReqMsg, pstDstPid,
			aucServerFilePathName, aucClientFilePathName, ulSerialId);

	/*将节点加入到请求队列中*/
	lRet = cpss_ftp_info_node_queue_join(&stFtpInfoNode);
	if (CPSS_OK != lRet)
	{
		cpss_ftp_rsp_send(CPSS_ERR_FS_JOIN_NODE_FAIL, stFtpInfoNode.ulUsrSeq,
				&stFtpInfoNode.stDstPid);

		cpss_output(CPSS_MODULE_FS, CPSS_PRINT_WARN,
				"cpss_ftp_req_dealing_tnbs queue join failed\n");

		return CPSS_ERR_FS_JOIN_NODE_FAIL;
	}

	//    lRet = cpss_ftp_info_node_pre_deal(&stFtpInfoNode) ;
	if (CPSS_OK != lRet)
	{
		cpss_ftp_rsp_send(lRet, stFtpInfoNode.ulUsrSeq, &stFtpInfoNode.stDstPid);

		cpss_ftp_info_node_queue_delete(ulSerialId);

		cpss_output(CPSS_MODULE_FS, CPSS_PRINT_WARN,
				"cpss_ftp_req_dealing_tnbs pre deal failed\n");

		return lRet;
	}

	//    lRet = cpss_ftp_connect_req_send(&stFtpInfoNode) ;
	if (CPSS_OK != lRet)
	{
		cpss_ftp_info_node_queue_delete(ulSerialId);

		cpss_ftp_rsp_send(CPSS_ERR_FS_CONN_REQ_SEND_FAIL,
				stFtpInfoNode.ulUsrSeq, &stFtpInfoNode.stDstPid);

		cpss_output(CPSS_MODULE_FS, CPSS_PRINT_WARN,
				"cpss_ftp_req_dealing_tnbs send connect req failed\n");

		return CPSS_ERR_FS_CONN_REQ_SEND_FAIL;
	}

	cpss_output(CPSS_MODULE_FS, CPSS_PRINT_INFO,
			"cpss_ftp_req_dealing_tnbs send connect req succ.seq=%d\n",
			stFtpInfoNode.ulSerialId);

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_ftpXfer_rsp_deal
 * 功    能: ftp连接响应的处理过程
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
#if 	0 
INT32 cpss_ftp_cwd_command_orgnize
(
		CPSS_FTP_INFO_NODE_T *pstFtpInfoNode,
		TNBS_FTP_COMMAND_REQ_MSG_T *pstFtpCommondReq
)
{
	printf("involking function cpss_ftp_cwd_command_orgnize which commited*********************************");

	UINT8 aucDirPathName[CPSS_FS_FILE_ABSPATH_LEN];
	UINT32 ulStrLen = 0;

	strncpy(aucDirPathName,pstFtpInfoNode->aucServerFilePathName,
			CPSS_FS_FILE_ABSPATH_LEN);

	cpss_fs_dirpath_unlink(aucDirPathName);

	if(strlen(aucDirPathName) == 0)
	{
		strcpy(aucDirPathName,".");
	}

	pstFtpCommondReq->ulSocketId = pstFtpInfoNode->ulCtrlSock;
	pstFtpCommondReq->ulCmdId = pstFtpInfoNode->ulSerialId;

	strncpy(pstFtpCommondReq->ucpFmt,CPSS_FTP_CWD_STR,strlen(CPSS_FTP_CWD_STR));
	ulStrLen = strlen(CPSS_FTP_CWD_STR);
	strncpy(pstFtpCommondReq->ucpFmt+ulStrLen," ",1);
	ulStrLen = ulStrLen + 1;
	strncpy(pstFtpCommondReq->ucpFmt+ulStrLen,aucDirPathName,strlen(aucDirPathName)+1);
	ulStrLen = ulStrLen + strlen(aucDirPathName);

	pstFtpCommondReq->ulLen = ulStrLen;

	return CPSS_OK;
}
#endif
#if		0 
/*******************************************************************************
 * 函数名称: cpss_ftp_cwd_req_send
 * 功    能: 发送CWD命令给TNBS
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_cwd_req_send
(
		CPSS_FTP_INFO_NODE_T *pstFtpInfoNode
)
{
	TNBS_FTP_COMMAND_REQ_MSG_T stFtpCommandReq;
	CPSS_COM_PID_T stLocalPid;
	INT32 lRet;

	//    cpss_ftp_cwd_command_orgnize(pstFtpInfoNode,&stFtpCommandReq) ;


	cpss_com_logic_addr_get(&stLocalPid.stLogicAddr,&stLocalPid.ulAddrFlag);
	stLocalPid.ulPd = TNBS_FTP_PROC;

	cpss_fs_htonl_cmd_req(&stFtpCommandReq);

	lRet = cpss_com_send(&stLocalPid,TNBS_FTP_CMD_REQ_MSG,
			(UINT8*)&stFtpCommandReq,sizeof(TNBS_FTP_COMMAND_REQ_MSG_T));

	cpss_output(CPSS_MODULE_FS,CPSS_PRINT_INFO,
			"cpss_ftp_cwd_req_send send cwd req.seq=%d.lRet=%d\n",stFtpCommandReq.ulCmdId,lRet);

	return lRet;
}
#endif
/*******************************************************************************
 * 函数名称: cpss_ftp_ftpXfer_rsp_deal
 * 功    能: FTP连接响应的处理过程
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
#if	0 
INT32 cpss_ftp_ftpXfer_rsp_deal(TNBS_FTP_XFER_RSP_MSG_T* pstFtpXferRsp)
{
	CPSS_FTP_INFO_NODE_T *pstFtpInfoNode = NULL;
	UINT32 ulUsrSerialId;
	UINT32 ulSerialId;
	INT32 lRet;

	ulSerialId = pstFtpXferRsp->ulCmdId;

	cpss_output(CPSS_MODULE_FS,CPSS_PRINT_INFO,
			"cpss_ftp_ftpXfer_rsp_deal recv seq=%d.lResult=%d.reply=%s",
			ulSerialId,pstFtpXferRsp->lResult,pstFtpXferRsp->ucReplyString);

	pstFtpInfoNode = cpss_ftp_info_node_queue_find(ulSerialId,NULL);
	if(NULL == pstFtpInfoNode)
	{
		return CPSS_ERROR;
	}

	ulUsrSerialId = pstFtpInfoNode->ulUsrSeq;

	if(pstFtpXferRsp->lResult != CPSS_OK)
	{

		cpss_ftp_rsp_send(pstFtpXferRsp->lResult,
				ulUsrSerialId,&pstFtpInfoNode->stDstPid);

		cpss_ftp_info_node_queue_delete(ulSerialId);

		return CPSS_ERROR;
	}

	pstFtpInfoNode->ulCtrlSock = pstFtpXferRsp->ulCtrlSock;
	pstFtpInfoNode->ulPhase = CPSS_FTP_CWD_PHASE;

	//    lRet = CPS__cpss_ftp_cwd_req_send(pstFtpInfoNode) ;
	if(CPSS_OK != lRet)
	{
		cpss_ftp_command_end_deal(pstFtpInfoNode,CPSS_ERR_FS_CWD_REQ_SEND_FAIL);
	}

	return lRet;
}
#endif 
/*******************************************************************************
 * 函数名称: cpss_ftp_trans_command_fill
 * 功    能: FTP发送命令的填充函数
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
#if 	0 
INT32 cpss_ftp_trans_command_fill
(
		CPSS_FTP_INFO_NODE_T* pstFtpInfoNode,
		TNBS_FTP_COMMAND_REQ_MSG_T *pstFtpCommandReq
)
{
	INT32 lRet = CPSS_OK;
	printf("involking function cpss_ftp_trans_command_fill which commited*********************************");
#if 	0 
	UINT8 aucServerFilePath[CPSS_FS_FILE_ABSPATH_LEN];
	UINT8 aucServerFileName[CPSS_FS_FILENAME_LEN];
	UINT32 ulFtpCmd;
	UINT32 ulStrLen;

	ulFtpCmd = pstFtpInfoNode->ulFtpCmd;
	pstFtpCommandReq->ulSocketId = pstFtpInfoNode->ulCtrlSock;
	pstFtpCommandReq->ulCmdId = pstFtpInfoNode->ulSerialId;

	strncpy(aucServerFilePath,
			pstFtpInfoNode->aucServerFilePathName,CPSS_FS_FILE_ABSPATH_LEN);

	if((ulFtpCmd == CPSS_FTP_CMD_PUT_FILE)||(ulFtpCmd == CPSS_FTP_CMD_PUT_DIR_PACK))
	{
		lRet = cpss_ftp_tmp_dirname_make(aucServerFilePath,CPSS_FS_FILE_ABSPATH_LEN);
		if(CPSS_OK != lRet)
		{
			return lRet;
		}
	}

	lRet = cpss_fs_get_file_name(aucServerFilePath,aucServerFileName);
	if(CPSS_OK != lRet)
	{
		return CPSS_ERROR;
	}

	strncpy(pstFtpCommandReq->ucClientFilePath,
			pstFtpInfoNode->aucClientFilePathName,CPSS_FS_FILE_ABSPATH_LEN - 5);

	if((ulFtpCmd == CPSS_FTP_CMD_PUT_FILE)||(ulFtpCmd == CPSS_FTP_CMD_PUT_DIR_PACK))
	{
		if(ulFtpCmd == CPSS_FTP_CMD_PUT_DIR_PACK)
		{
			lRet = cpss_ftp_tmp_dirname_make(
					pstFtpCommandReq->ucClientFilePath,CPSS_FS_FILE_ABSPATH_LEN - 5);
			if(CPSS_OK != lRet)
			{
				return CPSS_ERROR;
			}
		}

		strncpy(pstFtpCommandReq->ucpFmt,CPSS_FTP_PUT_TNBS_STR,strlen(CPSS_FTP_PUT_TNBS_STR));
		ulStrLen = strlen(CPSS_FTP_PUT_TNBS_STR);
	}
	else if((ulFtpCmd == CPSS_FTP_CMD_GET_FILE)||(ulFtpCmd == CPSS_FTP_CMD_GET_DIR_PACK))
	{
		lRet = cpss_ftp_tmp_dirname_make(
				pstFtpCommandReq->ucClientFilePath,CPSS_FS_FILE_ABSPATH_LEN);
		if(CPSS_OK != lRet)
		{
			return CPSS_ERROR;
		}

		strncpy(pstFtpCommandReq->ucpFmt,CPSS_FTP_GET_TNBS_STR,strlen(CPSS_FTP_GET_TNBS_STR));
		ulStrLen = strlen(CPSS_FTP_GET_TNBS_STR);
	}
	strncpy(pstFtpCommandReq->ucpFmt+ulStrLen," ",1);
	ulStrLen++;
	strncpy(pstFtpCommandReq->ucpFmt+ulStrLen,aucServerFileName,strlen(aucServerFileName)+1);
	ulStrLen = ulStrLen + strlen(aucServerFileName);

	pstFtpCommandReq->ulLen = ulStrLen;
#endif
	return lRet;
}
#endif
#if		0
/*******************************************************************************
 * 函数名称: cpss_ftp_cwd_rsp_deal
 * 功    能: FTP的CWD响应消息的处理
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_cwd_rsp_deal
(
		CPSS_FTP_INFO_NODE_T* pstFtpInfoNode
)
{
	TNBS_FTP_COMMAND_REQ_MSG_T stFtpCommandReq;
	CPSS_COM_PID_T stLocalPid;
	INT32 lRet;

	lRet = cpss_ftp_trans_command_fill(pstFtpInfoNode,&stFtpCommandReq);
	if(CPSS_OK != lRet)
	{

		cpss_ftp_command_end_deal(pstFtpInfoNode,lRet);
	}

	pstFtpInfoNode->ulPhase = CPSS_FTP_TRANSFER_PHASE;

	pstFtpInfoNode->ulProStat = CPSS_FTP_STATE_TRANSFERRING;
	pstFtpInfoNode->ulDataDealNum = 0;
	pstFtpInfoNode->ulBeginTick = cpss_tick_get();

	cpss_com_logic_addr_get(&stLocalPid.stLogicAddr,&stLocalPid.ulAddrFlag);
	stLocalPid.ulPd = TNBS_FTP_PROC;

	cpss_fs_htonl_cmd_req(&stFtpCommandReq);

	lRet = cpss_com_send(&stLocalPid,TNBS_FTP_CMD_REQ_MSG,
			(UINT8*)&stFtpCommandReq,sizeof(TNBS_FTP_COMMAND_REQ_MSG_T));
	if(CPSS_OK != lRet)
	{

		cpss_ftp_command_end_deal(pstFtpInfoNode,CPSS_ERR_FS_FTP_CMD_SEND_FAIL);
	}

	cpss_output(CPSS_MODULE_FS,CPSS_PRINT_INFO,
			"cpss_ftp_cwd_rsp_deal seq=%d.ftm=%s",
			stFtpCommandReq.ulCmdId,stFtpCommandReq.ucpFmt);

	return CPSS_OK;
}
#endif 
/*******************************************************************************
 * 函数名称: cpss_ftp_usr_mem_write
 * 功    能: ftp将数据写入到用户内存的函数
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_usr_mem_write(UINT8* pucFilePath, UINT8* pucWriteMem,
		UINT32 ulMemLen)
{
	UINT32 ulFd;
	INT32 lRet;
	UINT32 ulFileLen;
	UINT32 ulReadLen;

	lRet = cpss_file_get_size(pucFilePath, &ulFileLen);
	if (CPSS_OK != lRet)
	{
		return CPSS_ERROR;
	}

	if (ulFileLen > ulMemLen)
	{
		cpss_output(CPSS_MODULE_FS, CPSS_PRINT_WARN,
				"cpss_ftp_usr_mem_write,fileLen=%d,memLen=%d\n", ulFileLen,
				ulMemLen);
		return CPSS_ERROR;
	}

	ulFd = cpss_file_open(pucFilePath, CPSS_FILE_ORDONLY);
	if (ulFd == CPSS_FD_INVALID)
	{
		return CPSS_ERROR;
	}

	lRet = cpss_file_read(ulFd, pucWriteMem, ulFileLen, &ulReadLen);
	if (CPSS_OK != lRet)
	{
		cpss_file_close(ulFd);
		return CPSS_ERROR;
	}

	cpss_file_close(ulFd);
	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_get_file_deal
 * 功    能: 下载文件的处理流程
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_get_file_deal(CPSS_FTP_INFO_NODE_T* pstFtpInfoNode,
		CPS__TNBS_FTP_COMMAND_RSP_MSG_T* pstFtpCommandRsp)
{
	UINT32 ulFtpCommand;
	INT32 lRet;
	printf(
			"involking function cpss_ftp_get_file_deal which commited *********************************");
#if 	0 
	UINT8 aucClientFilePath[CPSS_FS_FILE_ABSPATH_LEN];

	if(pstFtpCommandRsp->lResult != CPSS_OK)
	{
		return pstFtpCommandRsp->lResult;
	}

	ulFtpCommand = pstFtpInfoNode->ulFtpCmd;

	strncpy(aucClientFilePath,
			pstFtpInfoNode->aucClientFilePathName,CPSS_FS_FILE_ABSPATH_LEN);
	lRet = cpss_ftp_tmp_dirname_make(aucClientFilePath,CPSS_FS_FILE_ABSPATH_LEN);
	if(CPSS_OK != lRet)
	{
		return lRet;
	}

	if(ulFtpCommand == CPSS_FTP_CMD_GET_FILE)
	{
		if((pstFtpInfoNode->ulLen > 0)&&(pstFtpInfoNode->pucMem != NULL))
		{
			lRet = cpss_ftp_usr_mem_write(aucClientFilePath,pstFtpInfoNode->pucMem,pstFtpInfoNode->ulLen);
			if(CPSS_OK != lRet)
			{
				return CPSS_ERR_FS_MEM_WRITE_FAIL;
			}
		}

		cpss_file_delete(pstFtpInfoNode->aucClientFilePathName);

		lRet = cpss_file_rename(aucClientFilePath,pstFtpInfoNode->aucClientFilePathName);
	}
	else if(ulFtpCommand == CPSS_FTP_CMD_GET_DIR_PACK)
	{
		pstFtpInfoNode->ulBeginTick = cpss_tick_get();
		pstFtpInfoNode->ulDataDealNum = 0;
		pstFtpInfoNode->ulProStat = CPSS_FTP_STATE_PACKING;

		lRet = cpss_file_unpack(aucClientFilePath,pstFtpInfoNode->aucClientFilePathName);

		cpss_file_delete(aucClientFilePath);
	}
	else
	{
		lRet = CPSS_ERROR;
	}
#endif
	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_put_file_deal
 * 功    能: 上传文件的处理流程
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_put_file_deal(CPSS_FTP_INFO_NODE_T* pstFtpInfoNode,
		CPS__TNBS_FTP_COMMAND_RSP_MSG_T* pstFtpCommandRsp)
{
	INT32 lRet;
	printf(
			"involking function cpss_ftp_put_file_deal which commited *********************************");
#if 	0 
	UINT32 ulStrLen;
	TNBS_FTP_COMMAND_REQ_MSG_T stFtpCommandReq;
	CPSS_COM_PID_T stLocalPid;
	UINT8 aucServerFileName[CPSS_FS_FILENAME_LEN];
	UINT8 aucServerFilePath[CPSS_FS_FILE_ABSPATH_LEN];
	UINT8 aucClientFilePath[CPSS_FS_FILE_ABSPATH_LEN];

	if(pstFtpInfoNode->ulFtpCmd == CPSS_FTP_CMD_PUT_DIR_PACK)
	{

		strncpy(aucClientFilePath,pstFtpInfoNode->aucClientFilePathName,CPSS_FS_FILE_ABSPATH_LEN);
		lRet = cpss_ftp_tmp_dirname_make(aucClientFilePath,CPSS_FS_FILE_ABSPATH_LEN);
		if(CPSS_OK != lRet)
		{
			return lRet;
		}

		cpss_file_delete(aucClientFilePath);
	}

	if(pstFtpCommandRsp->lResult != CPSS_OK)
	{
		return pstFtpCommandRsp->lResult;
	}

	strncpy(aucServerFilePath,
			pstFtpInfoNode->aucServerFilePathName,CPSS_FS_FILE_ABSPATH_LEN);
#if 0    
	lRet = cpss_ftp_tmp_dirname_make(aucServerFilePath,CPSS_FS_FILE_ABSPATH_LEN);
	if(CPSS_OK != lRet)
	{
		return lRet;
	}
#endif
	lRet = cpss_fs_get_file_name(aucServerFilePath,aucServerFileName);
	if(CPSS_OK != lRet)
	{
		return lRet;
	}

	stFtpCommandReq.ulCmdId = pstFtpInfoNode->ulSerialId;
	stFtpCommandReq.ulSocketId = pstFtpInfoNode->ulCtrlSock;

	strncpy(stFtpCommandReq.ucpFmt,CPSS_FTP_DELE_STR,strlen(CPSS_FTP_DELE_STR));
	ulStrLen = strlen(CPSS_FTP_DELE_STR);
	strncpy(stFtpCommandReq.ucpFmt+ulStrLen," ",1);
	ulStrLen = ulStrLen + 1;
	strncpy(stFtpCommandReq.ucpFmt+ulStrLen,aucServerFileName,strlen(aucServerFileName)+1);
	ulStrLen = ulStrLen + strlen(aucServerFileName);

	stFtpCommandReq.ulLen = ulStrLen;

	cpss_com_logic_addr_get(&stLocalPid.stLogicAddr,&stLocalPid.ulAddrFlag);
	stLocalPid.ulPd = TNBS_FTP_PROC;

	cpss_fs_htonl_cmd_req(&stFtpCommandReq);

	lRet = cpss_com_send(&stLocalPid,TNBS_FTP_CMD_REQ_MSG,
			(UINT8*)&stFtpCommandReq,sizeof(TNBS_FTP_COMMAND_REQ_MSG_T));

	pstFtpInfoNode->ulPhase = CPSS_FTP_DELE_PHASE;

	cpss_output(CPSS_MODULE_FS,CPSS_PRINT_INFO,
			"cpss_ftp_put_file_deal,seq=%d,fmt=%s\n",
			stFtpCommandReq.ulCmdId,stFtpCommandReq.ucpFmt);
#endif
	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_dele_rsp_deal
 * 功    能: 删除服务器文件的响应消息处理流程
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_dele_rsp_deal(CPSS_FTP_INFO_NODE_T* pstFtpInfoNode,
		CPS__TNBS_FTP_COMMAND_RSP_MSG_T* pstFtpCommandRsp)
{

	INT32 lRet;
#if 	0 
	UINT32 ulStrLen;
	TNBS_FTP_COMMAND_REQ_MSG_T stFtpCommandReq;
	CPSS_COM_PID_T stLocalPid;
	UINT8 aucServerFileName[CPSS_FS_FILENAME_LEN];
	UINT8 aucServerFilePath[CPSS_FS_FILE_ABSPATH_LEN];

	strncpy(aucServerFilePath,
			pstFtpInfoNode->aucServerFilePathName,CPSS_FS_FILE_ABSPATH_LEN);
#if 1  
	lRet = cpss_ftp_tmp_dirname_make(aucServerFilePath,CPSS_FS_FILE_ABSPATH_LEN);
	if(CPSS_OK != lRet)
	{
		return lRet;
	}
#endif
	lRet = cpss_fs_get_file_name(aucServerFilePath,aucServerFileName);
	if(CPSS_OK != lRet)
	{
		return lRet;
	}
	stFtpCommandReq.ulCmdId = pstFtpInfoNode->ulSerialId;
	stFtpCommandReq.ulSocketId = pstFtpInfoNode->ulCtrlSock;

	strncpy(stFtpCommandReq.ucpFmt,CPSS_FTP_RNFR_STR,strlen(CPSS_FTP_RNFR_STR));
	ulStrLen = strlen(CPSS_FTP_RNFR_STR);
	strncpy(stFtpCommandReq.ucpFmt+ulStrLen," ",1);
	ulStrLen = ulStrLen + 1;
	strncpy(stFtpCommandReq.ucpFmt+ulStrLen,aucServerFileName,strlen(aucServerFileName)+1);
	ulStrLen = ulStrLen + strlen(aucServerFileName);

	stFtpCommandReq.ulLen = ulStrLen;

	cpss_com_logic_addr_get(&stLocalPid.stLogicAddr,&stLocalPid.ulAddrFlag);
	stLocalPid.ulPd = TNBS_FTP_PROC;

	cpss_fs_htonl_cmd_req(&stFtpCommandReq);

	lRet = cpss_com_send(&stLocalPid,TNBS_FTP_CMD_REQ_MSG,
			(UINT8*)&stFtpCommandReq,sizeof(TNBS_FTP_COMMAND_REQ_MSG_T));

	pstFtpInfoNode->ulPhase = CPSS_FTP_RNFR_PHASE;

	cpss_output(CPSS_MODULE_FS,CPSS_PRINT_INFO,
			"cpss_ftp_dele_rsp_deal,seq=%d,fmt=%s\n",
			stFtpCommandReq.ulCmdId,stFtpCommandReq.ucpFmt);
#endif
	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_transfer_rsp_deal
 * 功    能: FTP传输过程的响应处理过程
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_transfer_rsp_deal(CPSS_FTP_INFO_NODE_T* pstFtpInfoNode,
		CPS__TNBS_FTP_COMMAND_RSP_MSG_T* pstFtpCommandRsp)
{
	UINT32 ulFtpCmd;
	INT32 lRet;

	ulFtpCmd = pstFtpInfoNode->ulFtpCmd;

	if ((ulFtpCmd == CPSS_FTP_CMD_GET_FILE) || (ulFtpCmd
			== CPSS_FTP_CMD_GET_DIR_PACK))
	{
		/*处理下载请求*/
		lRet = cpss_ftp_get_file_deal(pstFtpInfoNode, pstFtpCommandRsp);

		/*发送响应消息*/
		cpss_ftp_command_end_deal(pstFtpInfoNode, lRet);
	}
	else if ((ulFtpCmd == CPSS_FTP_CMD_PUT_FILE) || (ulFtpCmd
			== CPSS_FTP_CMD_PUT_DIR_PACK))
	{
		/*处理上传请求*/
		lRet = cpss_ftp_put_file_deal(pstFtpInfoNode, pstFtpCommandRsp);
		if (CPSS_OK != lRet)
		{
			/*发送失败响应消息*/
			cpss_ftp_command_end_deal(pstFtpInfoNode, lRet);
		}
	}
	else
	{
		/*发送失败响应消息*/
		cpss_ftp_command_end_deal(pstFtpInfoNode, CPSS_ERR_FS_FTP_CMD_ILLEGAL);
		lRet = CPSS_ERROR;
	}
	return lRet;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_rnfr_rsp_deal
 * 功    能: FTP的RNFR命令的响应处理流程
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_rnfr_rsp_deal(CPSS_FTP_INFO_NODE_T *pstFtpInfoNode,
		CPS__TNBS_FTP_COMMAND_RSP_MSG_T* pstFtpCommandRsp)
{
#if 	0
	INT32 lRet;
	TNBS_FTP_COMMAND_REQ_MSG_T stFtpCommandReq;
	CPSS_COM_PID_T stLocalPid;
	UINT8 aucServerFileName[CPSS_FS_FILENAME_LEN];
	UINT32 ulStrlen;

	if((pstFtpCommandRsp == NULL)||(pstFtpCommandRsp->lResult != CPSS_OK))
	{
		cpss_ftp_command_end_deal(pstFtpInfoNode,pstFtpCommandRsp->lResult);
		return CPSS_ERROR;
	}

	lRet = cpss_fs_get_file_name(pstFtpInfoNode->aucServerFilePathName,aucServerFileName);
	if(lRet != CPSS_OK)
	{
		cpss_ftp_command_end_deal(pstFtpInfoNode,lRet);
		return CPSS_ERROR;
	}

	stFtpCommandReq.ulSocketId = pstFtpInfoNode->ulCtrlSock;
	stFtpCommandReq.ulCmdId = pstFtpInfoNode->ulSerialId;

	strncpy(stFtpCommandReq.ucpFmt,CPSS_FTP_RNTO_STR,strlen(CPSS_FTP_RNTO_STR));
	ulStrlen = strlen(CPSS_FTP_RNTO_STR);
	strncpy(stFtpCommandReq.ucpFmt+ulStrlen," ",1);
	ulStrlen = ulStrlen + 1;
	strncpy(stFtpCommandReq.ucpFmt+ulStrlen,
			aucServerFileName,strlen(aucServerFileName)+1);
	ulStrlen = ulStrlen + strlen(aucServerFileName);

	stFtpCommandReq.ulLen = ulStrlen;

	cpss_com_logic_addr_get(&stLocalPid.stLogicAddr,&stLocalPid.ulAddrFlag);
	stLocalPid.ulPd = TNBS_FTP_PROC;

	cpss_fs_htonl_cmd_req(&stFtpCommandReq);

	lRet = cpss_com_send(&stLocalPid,TNBS_FTP_CMD_REQ_MSG,
			(UINT8*)&stFtpCommandReq,sizeof(TNBS_FTP_COMMAND_REQ_MSG_T));

	if(lRet != CPSS_OK)
	{
		cpss_ftp_command_end_deal(pstFtpInfoNode,lRet);
		return CPSS_ERROR;
	}

	pstFtpInfoNode->ulPhase = CPSS_FTP_RNTO_PHASE;

	cpss_output(CPSS_MODULE_FS,CPSS_PRINT_INFO,
			"cpss_ftp_rnfr_rsp_deal,seq=%d,fmt=%s\n",
			stFtpCommandReq.ulCmdId,stFtpCommandReq.ucpFmt);
#endif 
	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_rnto_rsp_deal
 * 功    能: FTP的RNTO名令的响应处理流程
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_rnto_rsp_deal(CPSS_FTP_INFO_NODE_T *pstFtpInfoNode,
		CPS__TNBS_FTP_COMMAND_RSP_MSG_T* pstFtpCommandRsp)
{

	if ((pstFtpCommandRsp == NULL) || (pstFtpCommandRsp->lResult != CPSS_OK))
	{
		cpss_ftp_command_end_deal(pstFtpInfoNode, pstFtpCommandRsp->lResult);

		return CPSS_ERROR;
	}

	cpss_ftp_command_end_deal(pstFtpInfoNode, CPSS_OK);

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_command_recv_deal
 * 功    能: FTP的接收命令的处理流程
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
#if	0	//liudng add begin
INT32 cpss_ftp_command_recv_deal
(
		TNBS_FTP_COMMAND_RSP_MSG_T* pstFtpCommandRsp
)
{
	INT32 lRet;
	CPSS_FTP_INFO_NODE_T *pstFtpInfoNode = NULL;

	if(pstFtpCommandRsp == NULL)
	{
		return CPSS_ERROR;
	}

	cpss_output(CPSS_MODULE_FS,CPSS_PRINT_INFO,
			"cpss_ftp_command_recv_deal,seq=%d,fmt=%s\n",
			pstFtpCommandRsp->ulCmdId,pstFtpCommandRsp->ucReplyString);

	pstFtpInfoNode = cpss_ftp_info_node_queue_find(pstFtpCommandRsp->ulCmdId,NULL);
	if(NULL == pstFtpInfoNode)
	{
		return CPSS_ERROR;
	}

	switch(pstFtpInfoNode->ulPhase)
	{
		case CPSS_FTP_CWD_PHASE:
		{
			//            lRet = cpss_ftp_cwd_rsp_deal(pstFtpInfoNode) ;
			break;
		}
		case CPSS_FTP_TRANSFER_PHASE:
		{
			lRet = cpss_ftp_transfer_rsp_deal(pstFtpInfoNode,pstFtpCommandRsp);
			break;
		}
		case CPSS_FTP_DELE_PHASE:
		{
			lRet = cpss_ftp_dele_rsp_deal(pstFtpInfoNode,pstFtpCommandRsp);
			break;
		}
		case CPSS_FTP_RNFR_PHASE:
		{
			lRet = cpss_ftp_rnfr_rsp_deal(pstFtpInfoNode,pstFtpCommandRsp);
			break;
		}
		case CPSS_FTP_RNTO_PHASE:
		{
			lRet = cpss_ftp_rnto_rsp_deal(pstFtpInfoNode,pstFtpCommandRsp);
			break;
		}
		default:
		{
			cpss_ftp_command_end_deal(pstFtpInfoNode,CPSS_ERROR);
			lRet = CPSS_ERROR;
			break;
		}
	}
	return lRet;
}
#endif 
#if		0 
/*******************************************************************************
 * 函数名称: cpss_ftp_quit_req_send
 * 功    能: FTP的QUIT命令发送函数
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_quit_req_send
(
		CPSS_FTP_INFO_NODE_T *pstFtpInfoNode
)
{
	TNBS_FTP_COMMAND_REQ_MSG_T stFtpQuitReq;
	CPSS_COM_PID_T stLocalPid;
	INT32 lRet;

	stFtpQuitReq.ulCmdId = pstFtpInfoNode->ulSerialId;
	stFtpQuitReq.ulSocketId = pstFtpInfoNode->ulCtrlSock;

	strncpy(stFtpQuitReq.ucpFmt,CPSS_FTP_QUIT_STR,strlen(CPSS_FTP_QUIT_STR)+1);

	stFtpQuitReq.ulLen = strlen(CPSS_FTP_QUIT_STR);

	cpss_com_logic_addr_get(&stLocalPid.stLogicAddr,&stLocalPid.ulAddrFlag);
	stLocalPid.ulPd = TNBS_FTP_PROC;

	cpss_fs_htonl_cmd_req(&stFtpQuitReq);

	lRet = cpss_com_send(&stLocalPid,TNBS_FTP_CMD_REQ_MSG,
			(UINT8*)&stFtpQuitReq,sizeof(TNBS_FTP_COMMAND_REQ_MSG_T));

	return lRet;
}
#endif 
/*******************************************************************************
 * 函数名称: cpss_ftp_command_end_deal
 * 功    能: FTP的结束处理流程
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
VOID cpss_ftp_command_end_deal(CPSS_FTP_INFO_NODE_T *pstFtpInfoNode,
		INT32 lResult)
{
	cpss_ftp_rsp_send(lResult, pstFtpInfoNode->ulUsrSeq,
			&pstFtpInfoNode->stDstPid);

	//    cpss_ftp_quit_req_send(pstFtpInfoNode) ;

	cpss_ftp_info_node_queue_delete(pstFtpInfoNode->ulSerialId);
}

/*******************************************************************************
 * 函数名称: cpss_fs_fd_insert
 * 功    能: 增加FD
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_fd_insert(UINT32 ulFd, UINT8* pucFileName)
{
	CPSS_FS_HANDLE_NODE_T *pstFsHandle;
	CPSS_FS_HANDLE_NODE_T *pstFsHandleNext;
	UINT32 ulExistFlag = 0;

	pstFsHandle = g_pstCpssFdLink;

	while (pstFsHandle != NULL)
	{
		if (pstFsHandle->ulFsHandle == ulFd)
		{
			ulExistFlag = 1;
			break;
		}
		pstFsHandle = pstFsHandle->pvNext;
	}

	if (ulExistFlag != 1)
	{
		pstFsHandle = cpss_mem_malloc(sizeof(CPSS_FS_HANDLE_NODE_T));
		if (NULL == pstFsHandle)
		{
			return CPSS_ERROR;
		}

		pstFsHandle->ulFsHandle = ulFd;
		strncpy(pstFsHandle->aucFilePath, pucFileName, CPSS_FS_FILE_ABSPATH_LEN);
		pstFsHandle->pvNext = NULL;

		if (g_pstCpssFdLink == NULL)
		{
			g_pstCpssFdLink = pstFsHandle;
		}
		else
		{
			pstFsHandleNext = g_pstCpssFdLink;
			g_pstCpssFdLink = pstFsHandle;
			g_pstCpssFdLink->pvNext = pstFsHandleNext;
		}
	}

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_fs_fd_delete
 * 功    能: 删除FD
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_fd_delete(UINT32 ulFd)
{
	CPSS_FS_HANDLE_NODE_T *pstFsHandle;
	CPSS_FS_HANDLE_NODE_T *pstFsHandlePre;

	pstFsHandle = g_pstCpssFdLink;
	pstFsHandlePre = pstFsHandle;

	while (pstFsHandle != NULL)
	{
		if (pstFsHandle->ulFsHandle == ulFd)
		{
			if (pstFsHandlePre == pstFsHandle)
			{
				g_pstCpssFdLink = pstFsHandle->pvNext;
			}
			else
			{
				pstFsHandlePre->pvNext = pstFsHandle->pvNext;
			}

			cpss_mem_free(pstFsHandle);

			break;
		}

		pstFsHandlePre = pstFsHandle;
		pstFsHandle = pstFsHandle->pvNext;
	}

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_fs_fd_close
 * 功    能: 删除FD链表，关闭打开的文件
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_fd_close()
{
	CPSS_FS_HANDLE_NODE_T *pstFsHandle;
	CPSS_FS_HANDLE_NODE_T *pstFsHandleNext;

	pstFsHandle = g_pstCpssFdLink;

	while (pstFsHandle != NULL)
	{

		cpss_file_close(pstFsHandle->ulFsHandle);

		pstFsHandleNext = pstFsHandle->pvNext;

		cpss_mem_free(pstFsHandle);

		pstFsHandle = pstFsHandleNext;
	}

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_fs_fd_info_deal
 * 功    能: 处理FD通知消息
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
INT32 cpss_fs_fd_info_deal(CPSS_FS_FD_IND_MSG_T* pstFdInd)
{
	UINT32 ulFd;
	UINT32 ulOperateFlag;

	ulFd = pstFdInd->ulFd;
	ulOperateFlag = pstFdInd->ulOperateFlag;

	if (ulOperateFlag == CPSS_FD_OPEN_FLAG)
	{
		cpss_fs_fd_insert(ulFd, pstFdInd->aucFilePath);
	}
	else if (ulOperateFlag == CPSS_FD_CLOSE_FLAG)
	{
		cpss_fs_fd_delete(ulFd);
	}

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_fs_fd_info_show
 * 功    能: 显示文件句柄信息
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:
 *******************************************************************************/
VOID cpss_fs_fd_info_show()
{
	CPSS_FS_HANDLE_NODE_T *pstFsHandle;
	UINT32 ulOpenFileNum = 0;

	pstFsHandle = g_pstCpssFdLink;

	oams_shcmd_printf("-----------open files------------\n");

	while (pstFsHandle != NULL)
	{
		oams_shcmd_printf("%d record filename=%s fd=%d\n", ulOpenFileNum,
				pstFsHandle->aucFilePath, pstFsHandle->ulFsHandle);

		ulOpenFileNum++;
		pstFsHandle = pstFsHandle->pvNext;
	}

	oams_shcmd_printf("\n%d files is opened!\n", ulOpenFileNum);

	oams_shcmd_printf("---------------------------------\n");
}

/*******************************************************************************
 * 函数名称: cpss_tftp_proc_init
 * 功    能: 初始化TFTP服务纤程
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * 函数返回:
 * 说   明:  在纤程用户内存中存储TFTP节点指针
 *******************************************************************************/
INT32 cpss_tftp_proc_init()
{
	UINT8* pucTftpLinkHead = NULL;

	/*得到FTP纤程的用户区内存指针*/
	pucTftpLinkHead = (UINT8*) cpss_vk_proc_user_data_get();
	if (pucTftpLinkHead == NULL)
	{
		return CPSS_ERROR;
	}

	/*偏移用户区内存指针*/
	pucTftpLinkHead = pucTftpLinkHead + sizeof(CPSS_FTP_INFO_MAN_T);

	/*清零内存*/
	cpss_mem_memset(pucTftpLinkHead,0,4);

	if (cpss_vk_pd_self_get() == CPSS_FS_FTP_LOW_PROC)
	{
		g_ulTftpTransLinkLow = *(UINT32*) pucTftpLinkHead;
	}
	else
	{
		g_ulTftpTransLinkHigh = *(UINT32*) pucTftpLinkHead;
	}

	return CPSS_OK;
}

/*******************************************************************************
 * 函数名称: cpss_ftp_proc
 * 功    能: 提供FTP服务的纤程
 *
 * 函数类型:
 * 参    数:
 * 参数名称            类型            输入/输出         描述
 * usUserState         usUserState     输入              FTP纤程的状态
 * pvVar               VOID*           输入              存储的纤程数据
 * pstMsgHead   CPSS_COM_MSG_HEAD_T*   输入              通信包头
 * 函数返回:
 *       成功：CPSS_OK;
 *       失败：CPSS_ERROR;
 * 说   明:
 *******************************************************************************/
extern cpss_dbg_disk_proc(CPSS_COM_MSG_HEAD_T *pstMsgHead);

extern UINT32 cpss_vk_get_dir_copy_stat(VOID);
extern VOID cpss_vk_clr_dir_copy_stat(VOID);
void cpss_ftp_proc(UINT16 usUserState, VOID * pvVar,
		CPSS_COM_MSG_HEAD_T *pstMsgHead)
{
	INT32 lStandbyToSend = 0;
	CPSS_COM_PID_T stDstPid;
	INT32 lRet = CPSS_OK;

	CPS_FILE_TRANS_RSP_MSG_T stTransRsp;

	/*得到源纤程的ProcId,logAddr.*/
	stDstPid.stLogicAddr = pstMsgHead->stSrcProc.stLogicAddr;
	stDstPid.ulAddrFlag = pstMsgHead->stSrcProc.ulAddrFlag;
	stDstPid.ulPd = pstMsgHead->stSrcProc.ulPd;

	switch (pstMsgHead->ulMsgId)
	{
#if 0
#ifdef CPSS_SYS_SOCKET_FTP
	/* FTP传输请求消息 */
	case CPSS_FTP_REQ_MSG:
	{
		CPSS_FTP_REQ_MSG_T *pstFTPReqMsg;

		/*得到通信帧头格式*/
		pstFTPReqMsg = (CPSS_FTP_REQ_MSG_T*)pstMsgHead->pucBuf;

		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_INFO,
				"recv ftp req ip=%x,serverfile=%s,clientfile=%s,ulCmd=%d,ulLen=%d",
				pstFTPReqMsg->ulServerIP,pstFTPReqMsg->aucServerFile,
				pstFTPReqMsg->aucClientFile,pstFTPReqMsg->ulFtpCmd,pstFTPReqMsg->ulLen);

		/*调用ftp处理*/
		cpss_ftp_req_dealing(pstFTPReqMsg,&stDstPid);

		break;
	}
#else
	case CPSS_FTP_REQ_MSG:
	{
		CPSS_FTP_REQ_MSG_T *pstFTPReqMsg;

		/*得到通信帧头格式*/
		pstFTPReqMsg = (CPSS_FTP_REQ_MSG_T*)pstMsgHead->pucBuf;

		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_INFO,
				"recv ftp req ip=%x,serverfile=%s,clientfile=%s,ulCmd=%d,ulLen=%d",
				pstFTPReqMsg->ulServerIP,pstFTPReqMsg->aucServerFile,
				pstFTPReqMsg->aucClientFile,pstFTPReqMsg->ulFtpCmd,pstFTPReqMsg->ulLen);

		/*处理ftp传输请求*/
		cpss_ftp_req_dealing_tnbs(pstFTPReqMsg,&stDstPid);
		break;
	}
	case TNBS_FTP_XFER_RSP_MSG:
	{
		cpss_fs_ntohl_xfer_rsp((TNBS_FTP_XFER_RSP_MSG_T*)pstMsgHead->pucBuf);
		/*处理ftpXfer响应*/
		//            cpss_ftp_ftpXfer_rsp_deal((TNBS_FTP_XFER_RSP_MSG_T*)pstMsgHead->pucBuf) ;
		break;
	}
	case TNBS_FTP_CMD_RSP_MSG:
	{
		cpss_fs_ntohl_cmd_rsp((TNBS_FTP_COMMAND_RSP_MSG_T*)pstMsgHead->pucBuf);

		//            cpss_ftp_command_recv_deal((TNBS_FTP_COMMAND_RSP_MSG_T*)pstMsgHead->pucBuf) ;
		break;
	}

#endif
	/* TFTP传输请求消息 */
	case CPSS_TFTP_REQ_MSG:
	{
		CPSS_TFTP_REQ_MSG_T *pstTFTPReqMsg;

		/*得到通信帧头格式*/
		pstTFTPReqMsg = (CPSS_TFTP_REQ_MSG_T*)pstMsgHead->pucBuf;

		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_INFO,
				"recv ftp req ip=%x,serverfile=%s,clientfile=%s,ulCmd=%d,ulLen=%d",
				pstTFTPReqMsg->ulServerIP,pstTFTPReqMsg->aucServerFile,
				pstTFTPReqMsg->aucClientFile,pstTFTPReqMsg->ulCmd,pstTFTPReqMsg->ulLen);

		/*调用tftp处理*/
		cpss_tftp_req_dealing(pstTFTPReqMsg,&stDstPid);
		break;
	}
	case CPSS_TFTP_TRANS_REQ_MSG:
	{
		cpss_tftp_trans_req_recv((CPSS_TFTP_TRANS_REQ_MSG_T*)pstMsgHead->pucBuf);
		break;
	}
	case CPSS_TFTP_TRANS_DATA_MSG:
	{
		cpss_tfpt_trans_data_recv((CPSS_TFTP_TRANS_DATA_MSG_T*)pstMsgHead->pucBuf);
		break;
	}
	case CPSS_TFTP_TRANS_ACK_MSG:
	{
		cpss_tftp_trans_ack_recv((CPSS_TFTP_TRANS_ACK_MSG_T*)pstMsgHead->pucBuf);
		break;
	}
	case CPSS_TFTP_TRANS_DISCONNECT_MSG:
	{
		cpss_tftp_disconnect_msg_recv((CPSS_TFTP_TRANS_DISCONNCT_MSG_T*)pstMsgHead->pucBuf);
		break;
	}
	case CPSS_TFTP_TIMEOUT_TM_MSG:
	{
		cpss_tftp_timeout_msg_recv((CPSS_TIMER_MSG_T*)pstMsgHead->pucBuf);
		break;
	}
#endif
	/*读文件请求*/
	case CPSS_FS_FILE_READ_REQ_MSG:
	{
		cpss_fs_file_read_req_deal(
				(CPSS_FS_FILE_READ_REQ_T*) pstMsgHead->pucBuf, &stDstPid);
		break;
	}
		/*写文件请求*/
	case CPSS_FS_FILE_WRITE_REQ_MSG:
	{
		cpss_fs_file_write_req_deal(
				(CPSS_FS_FILE_WRITE_REQ_T*) pstMsgHead->pucBuf, &stDstPid);
		break;
	}
		/*拷贝文件请求*/
	case CPSS_FS_FILE_COPY_REQ_MSG:
	{
		cpss_fs_file_copy_req_deal(
				(CPSS_FS_FILE_COPY_REQ_T*) pstMsgHead->pucBuf, &stDstPid);
		break;
	}
		/*移动文件请求*/
	case CPSS_FS_FILE_MOVE_REQ_MSG:
	{
		cpss_fs_file_move_req_deal(
				(CPSS_FS_FILE_MOVE_REQ_T*) pstMsgHead->pucBuf, &stDstPid);
		break;
	}
	case CPSS_FS_DIR_COPY_REQ_MSG:
	{
		cpss_vk_clr_dir_copy_stat();

		cpss_fs_dir_copy_req_deal((CPSS_FS_DIR_COPY_REQ_T*) pstMsgHead->pucBuf,
				&stDstPid);

		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_FS, CPSS_PRINT_IMPORTANT,
				" dir copy from %s to %s end , total files %d \n\r",
				((CPSS_FS_DIR_COPY_REQ_T*) pstMsgHead->pucBuf)->aucSrcFileName,
				((CPSS_FS_DIR_COPY_REQ_T*) pstMsgHead->pucBuf)->aucDstFileName,
				cpss_vk_get_dir_copy_stat());

		break;
	}
	case CPSS_FS_DIR_DEL_REQ_MSG:
	{
		cpss_fs_dir_del_req_deal((CPSS_FS_DIR_DEL_REQ_T*) pstMsgHead->pucBuf,
				&stDstPid);
		break;
	}
	case CPSS_FS_FILE_DEL_REQ_MSG:
	{
		cpss_fs_file_del_req_deal((CPSS_FS_FILE_DEL_REQ_T*) pstMsgHead->pucBuf,
				&stDstPid);
		break;
	}
	case CPSS_FS_FD_IND_MSG:
	{
		/*处理FD通知消息*/
		cpss_fs_fd_info_deal((CPSS_FS_FD_IND_MSG_T*) (pstMsgHead->pucBuf));
		break;
	}

	case CPS_FILE_TRANS_REQ_MSG:
	{
		g_lCpsFsFtError = CPS_FS_FT_ERROR_UNKNOWN;

		lRet = cps_file_trans_req_deal(
				(CPS_FILE_TRANS_REQ_MSG_T*) pstMsgHead->pucBuf);
		if (CPSS_OK == lRet)
		{
			g_lCpsFsFtError = CPS_FS_FT_NO_ERROR;
		}
		stTransRsp.lResult = cpss_htonl(lRet);
		stTransRsp.lErrCode = cpss_htonl(g_lCpsFsFtError);
		cpss_com_send(&pstMsgHead->stSrcProc, CPS_FILE_TRANS_RSP_MSG,
				&stTransRsp, sizeof(CPS_FILE_TRANS_RSP_MSG_T));
	}
		break;

		/* SMSS纤程激活消息 */
	case SMSS_PROC_ACTIVATE_REQ_MSG:
	{
		/*初始化tnbs的ftp功能*/
		lRet = CPSS_OK;

#if 0
#ifndef CPSS_SYS_SOCKET_FTP
		lRet = cpss_ftp_init_tnbs();
#endif            
		cpss_tftp_proc_init();
#endif

		cpss_active_proc_rsp_send(lRet);

		break;
	}
		/* 收到SMSS的“纤程激活完成指示”消息不处理事情 */
	case SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG:
	{
		break;
	}

#if 0
		case SMSS_STANDBY_TO_ACTIVE_REQ_MSG: /* 备升主的请求消息 */
		{
			/* 发送响应消息 */
			lStandbyToSend = cpss_standby_to_active_send(CPSS_OK);
			if (CPSS_OK != lStandbyToSend)
			{
				cpss_output(CPSS_MODULE_FS, CPSS_PRINT_ERROR,
						"CPSS_COM: cpss_fs_ftp proc send SMSS_STANDBY_TO_ACTIVE_RSP_MSG failed!");
			}
			break;
		}
#endif
#if 0
		case CPSS_FTP_QUERY_MSG:
		{
#ifndef CPSS_DSP_CPU
			cpss_ftp_link_show();
#endif
			break;
		}
#endif
		/* Add ended, 2006/4/24 */
		/*接收到未知消息*/
	default:
	{
		cpss_output(CPSS_MODULE_FS, CPSS_PRINT_WARN, "recv unknown msg.");
		break;
	}
	}
}

/*******************************************************************************
 * 函数名称: cpss_ftp_link_show
 * 功    能: FTP请求信息链的统计函数
 * 函数类型:
 * 参    数:
 * 函数返回: none
 * 说   明:
 *******************************************************************************/
VOID cpss_ftp_link_show()
{
	CPSS_FTP_INFO_NODE_T* pstFtpInfoNode;
	CPSS_FTP_INFO_MAN_T* pstFtpManLocal = NULL;
	UINT32 ulLoop = 0;

	/*得到FTP管理结构指针*/
	pstFtpManLocal = (CPSS_FTP_INFO_MAN_T*) cpss_vk_proc_user_data_get();
	if (pstFtpManLocal == NULL)
	{
		return;
	}

	/*得到FTP链表指针*/
	pstFtpInfoNode = pstFtpManLocal->pstFtpHdr;

	while (pstFtpInfoNode != NULL)
	{
		ulLoop++;
		oams_shcmd_printf("-------- the %d record begin--------\n", ulLoop);
		oams_shcmd_printf("aucClientFilePathName=%s\n",
				pstFtpInfoNode->aucClientFilePathName);
		oams_shcmd_printf("aucServerFilePathName=%s\n",
				pstFtpInfoNode->aucServerFilePathName);
		oams_shcmd_printf("ulSerialId=%d\n", pstFtpInfoNode->ulSerialId);
		oams_shcmd_printf("ulUsrSeq=%d\n", pstFtpInfoNode->ulUsrSeq);
		oams_shcmd_printf("aucUsrName=%s\n", pstFtpInfoNode->aucUsrName);
		oams_shcmd_printf("aucPasswd=%s\n", pstFtpInfoNode->aucPasswd);
		oams_shcmd_printf("ulFtpCmd=%d\n", pstFtpInfoNode->ulFtpCmd);
		oams_shcmd_printf("ulPhase=%d\n", pstFtpInfoNode->ulPhase);
		oams_shcmd_printf("ulServerIp=%d\n", pstFtpInfoNode->ulServerIp);
		oams_shcmd_printf("ulPd=0x%x\n", pstFtpInfoNode->stDstPid.ulPd);
		oams_shcmd_printf("-------- the %d record end--------\n", ulLoop);
		oams_shcmd_printf("\n");

		pstFtpInfoNode = pstFtpInfoNode->pvNext;
	}

	if (ulLoop == 1)
	{
		oams_shcmd_printf("--------^*^ there are no record ^*^--------\n");
	}
}

/*******************************************************************************
 * 函数名称: cpss_ftp_link_show
 * 功    能: FTP的显示函数
 * 函数类型:
 * 参    数:
 * 函数返回: none
 * 说   明:
 *******************************************************************************/
INT32 cpss_ftp_info_show(UINT32 ulType)
{
	CPSS_COM_PID_T stDstPid;
	INT32 lRet;

	cpss_com_logic_addr_get(&stDstPid.stLogicAddr, &stDstPid.ulAddrFlag);

	if (ulType == CPSS_FTP_LOW_FLAG)
	{
		stDstPid.ulPd = CPSS_FS_FTP_LOW_PROC;
	}
	else if (ulType == CPSS_FTP_HIGH_FLAG)
	{
		stDstPid.ulPd = CPSS_FS_FTP_HIGH_PROC;
	}
	else
	{
		cpss_output(CPSS_MODULE_FS, CPSS_PRINT_ERROR,
				"cpss_ftp_info_show parameter is not illegal.\n");

		return CPSS_ERROR;
	}

	lRet = cpss_com_send(&stDstPid, CPSS_FTP_QUERY_MSG, NULL, 0);
	if (CPSS_OK != lRet)
	{
		cpss_output(CPSS_MODULE_FS, CPSS_PRINT_ERROR,
				"cpss_ftp_info_show send msg failed.\n");
		return CPSS_ERROR;
	}

	return CPSS_OK;
}

#endif/*#ifndef CPSS_DSP_CPU*/

INT32 cps_file_trans_req_deal(CPS_FILE_TRANS_REQ_MSG_T* pstTransReq)
{
	INT32 lRet;

	if (NULL == pstTransReq)
	{
		g_lCpsFsFtError = CPS_FS_FT_ERROR_PARAM;
		return CPSS_ERROR;
	}

#if 0
	INT8 ServerAddr[32]=
	{	0};
	INT8* pServer;
	UINT32 ulPort;
	UINT32 ulType;
	UINT32 ulCmd;

	ulPort = pstTransReq->ulServerPort;
	ulPort = cpss_ntohl(ulPort);
	ulType = cpss_ntohl(pstTransReq->ulProtocol);
	ulCmd = cpss_ntohl(pstTransReq->ulCmd);

	struct in_addr stAddr;

	printf("cpss_ntohl(pstTransReq->ulServerPort) = %x\n", cpss_ntohl(pstTransReq->ulServerPort));
	printf("cpss_ntohl(pstTransReq->ulProtocol) = %x\n", cpss_ntohl(pstTransReq->ulProtocol));

	memcpy(&stAddr, pstTransReq->aucServerAddr, sizeof(struct in_addr));
	pServer = inet_ntoa(stAddr);
	sprintf(ServerAddr, "%s:%d", pServer, cpss_ntohl(pstTransReq->ulServerPort));

	switch(cpss_ntohl(pstTransReq->ulProtocol))
	{
		case CPS_FILE_TRANS_FTP:
		cps_file_trans_ftp_deal(cpss_ntohl(pstTransReq->ulCmd), &ServerAddr, &pstTransReq->aucUsername, &pstTransReq->aucPasswd, &pstTransReq->aucLocalFile, &pstTransReq->aucServerFile);
		break;
		case CPS_FILE_TRANS_TFTP:
		cps_file_trans_tftp_deal(cpss_ntohl(pstTransReq->ulCmd), pServer, ulPort,&pstTransReq->aucLocalFile, &pstTransReq->aucServerFile);
		break;
		default:
		break;
	}
#endif

	//	switch (cpss_ntohl(pstTransReq->ulProtocol))
	switch (pstTransReq->ulProtocol)

	{
	case CPS_FILE_TRANS_FTP:
		lRet = cps_file_trans_ftp_deal(pstTransReq);
		break;
	case CPS_FILE_TRANS_TFTP:
		lRet = cps_file_trans_tftp_deal(pstTransReq);
		break;
	default:
		break;
	}

	return lRet;
}

INT32 cps_file_trans_ftp_deal(CPS_FILE_TRANS_REQ_MSG_T* pstTransReq)
{
	INT8 ServerAddr[32] =
	{ 0 };
	struct in_addr stAddr;
	UINT32 ulPort;
	INT32 lRet;
	UINT8 aucLocalFileTmp[256];

	if ((NULL == pstTransReq->aucServerAddr) || (NULL
			== (pstTransReq->aucServerFile)) || (NULL
			== (pstTransReq->aucLocalFile)) || (NULL
			== (pstTransReq->aucUsername)))
	{
		g_lCpsFsFtError = CPS_FS_FT_ERROR_PARAM;
		return CPSS_ERROR;
	}

	if (sizeof(aucLocalFileTmp) < strlen((INT8*) pstTransReq->aucLocalFile)
			+ strlen(".tmp"))
	{
		g_lCpsFsFtError = CPS_FS_FT_ERROR_PARAM;
		return CPSS_ERROR;
	}

	//	ulPort = cpss_ntohl(pstTransReq->ulServerPort);
	ulPort = pstTransReq->ulServerPort;

	memcpy(&stAddr, pstTransReq->aucServerAddr, sizeof(struct in_addr));
	//	sprintf(ServerAddr, "%s:%d", inet_ntoa(stAddr), cpss_ntohl(pstTransReq->ulServerPort));
	sprintf(ServerAddr, "%s:%d", inet_ntoa(stAddr), pstTransReq->ulServerPort);

	ftpInit();
	lRet = ftpOpen(ServerAddr);
	if (0 == lRet)
	{
		g_lCpsFsFtError = CPS_FS_FT_ERROR_HOST;
		return CPSS_ERROR;
	}

	lRet
			= ftpLogin((INT8*)pstTransReq->aucUsername, (INT8*)pstTransReq->aucPasswd);
	if (0 == lRet)
	{
		g_lCpsFsFtError = CPS_FS_FT_ERROR_USER;
		return CPSS_ERROR;
	}

	//	switch (cpss_ntohl(pstTransReq->ulCmd))
	switch (pstTransReq->ulCmd)
	{
	case 1:
		memset(aucLocalFileTmp, 0, sizeof(aucLocalFileTmp));
		sprintf((INT8*) aucLocalFileTmp, "%s.tmp", pstTransReq->aucLocalFile);

		lRet
				= ftpGet((INT8*)aucLocalFileTmp, (INT8*)pstTransReq->aucServerFile, 'I');
		if (1 != lRet)
		{
			unlink((INT8*) aucLocalFileTmp);
			return CPSS_ERROR;
		}
		else
		{
			unlink((INT8*) pstTransReq->aucLocalFile);
			rename((INT8*) aucLocalFileTmp, (INT8*) pstTransReq->aucLocalFile);
			chmod((INT8*) pstTransReq->aucLocalFile , 0777);
		}
		break;
	case 2:
		lRet = ftpPut((INT8*)pstTransReq->aucLocalFile, (INT8*)pstTransReq->aucServerFile, 'I');
		if (1 != lRet)
		{
			return CPSS_ERROR;
		}
		break;
	default:
		break;
	}

	ftpQuit();
	return CPSS_OK;
}

INT32 cps_file_trans_tftp_deal(CPS_FILE_TRANS_REQ_MSG_T* pstTransReq)
{
	INT32 lFd;
	INT32 lRet;
	struct in_addr stAddr;
	UINT8 aucLocalFileTmp[256];

	if ((NULL == pstTransReq->aucServerAddr) || (NULL
			== (pstTransReq->aucServerFile)) || (NULL
			== (pstTransReq->aucLocalFile)))
	{
		g_lCpsFsFtError = CPS_FS_FT_ERROR_PARAM;
		return CPSS_ERROR;
	}

	if (sizeof(aucLocalFileTmp) < strlen((INT8*) pstTransReq->aucLocalFile)
			+ strlen(".tmp"))
	{
		g_lCpsFsFtError = CPS_FS_FT_ERROR_PARAM;
		return CPSS_ERROR;
	}

	memcpy(&stAddr, pstTransReq->aucServerAddr, sizeof(stAddr));

	//	switch (cpss_ntohl(pstTransReq->ulCmd))
	switch (pstTransReq->ulCmd)
	{
	case 1:
		memset(aucLocalFileTmp, 0, sizeof(aucLocalFileTmp));
		sprintf((INT8*) aucLocalFileTmp, "%s.tmp", pstTransReq->aucLocalFile);

		lFd = open((INT8*) aucLocalFileTmp, O_CREAT | O_RDWR);
		if (-1 == lFd)
		{
			return CPSS_ERROR;
		}

		//		lRet = tftpCopy(inet_ntoa(stAddr),
		//				cpss_ntohl(pstTransReq->ulServerPort),
		//						(INT8*)pstTransReq->aucServerFile, "get", "binary", lFd);
		lRet = tftpCopy(inet_ntoa(stAddr), pstTransReq->ulServerPort,
				(INT8*) pstTransReq->aucServerFile, "get", "binary", lFd);

		close(lFd);
		if (ERROR == lRet)
		{
			unlink((INT8*) aucLocalFileTmp);
			return CPSS_ERROR;
		}
		else
		{
			unlink((INT8*) pstTransReq->aucLocalFile);
			rename((INT8*) aucLocalFileTmp, (INT8*) pstTransReq->aucLocalFile);
			chmod((INT8*) pstTransReq->aucLocalFile , 0777);
			return CPSS_OK;
		}

		break;
	case 2:
		lFd = open((INT8*) pstTransReq->aucLocalFile, O_RDONLY);
		if (-1 == lFd)
		{
			g_lCpsFsFtError = CPS_FS_FT_ERROR_LOCAL_FILE;
			return CPSS_ERROR;
		}

		//		lRet = tftpCopy(inet_ntoa(stAddr),
		//				cpss_ntohl(pstTransReq->ulServerPort),
		//						(INT8*)pstTransReq->aucServerFile, "put", "binary", lFd);
		lRet = tftpCopy(inet_ntoa(stAddr), pstTransReq->ulServerPort,
				(INT8*) pstTransReq->aucServerFile, "put", "binary", lFd);
		close(lFd);
		if (ERROR == lRet)
		{
			return CPSS_ERROR;
		}
		else
		{
			return CPSS_OK;
		}

		break;
	default:
		return CPSS_ERROR;
		break;
	}

	return 0;
}

CPS_FILE_TRANS_REQ_MSG_T g_stTestTrans;
extern INT32 g_lCpsFsFtError;
void set_trans_fun(UINT32 cmd, UINT32 protocol, char* serverfile, char* localfile, char* IpAddr, UINT32 port, char* user, char* pass)
{
 INT32 lRet;
 g_stTestTrans.ulCmd = cmd;
 g_stTestTrans.ulProtocol = protocol;
 struct in_addr stAddr;
 inet_aton(IpAddr, &stAddr);
 memcpy(g_stTestTrans.aucServerFile, serverfile, sizeof(g_stTestTrans.aucServerFile));
 memcpy(g_stTestTrans.aucLocalFile, localfile, sizeof(g_stTestTrans.aucLocalFile));
 memcpy(g_stTestTrans.aucServerAddr, &stAddr, sizeof(UINT32));
 g_stTestTrans.ulServerPort = port;
 memcpy(g_stTestTrans.aucUsername, user, sizeof(g_stTestTrans.aucUsername));
 memcpy(g_stTestTrans.aucPasswd, pass, sizeof(g_stTestTrans.aucPasswd));
 lRet = cps_file_trans_req_deal(&g_stTestTrans);
 printf("file trans ret = %d\n", lRet);
 printf("g_lCpsFsFtError = %d\n", g_lCpsFsFtError);
}

/******************************* 源文件结束 **********************************/

