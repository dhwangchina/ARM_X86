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
#include "cpss_config.h"
#include "swp_common.h"

#include "cpss_public.h"
#include "cpss_common.h"
#include "cpss_fs_file.h"
#include "cpss_fs_ftp.h"
#include "cpss_vos_sem.h"
#include "cpss_fs_crc.h"
#include "cpss_err.h"
#include "dirent.h"
#include "sys/types.h"
#include <unistd.h>

static UINT32 g_ulCpssFileOpened = 0 ;
static UINT32 g_ulCpssDirOpened = 0 ;
static UINT32 g_ulFileInit=FALSE;

static UINT32 g_ulCpssFsSem = 0 ;
CPSS_FS_HANDLE_LINK_T g_stFsHandleLink = {0};

UINT32 g_ulCpssFsPackSem = 0 ;
INT32 cpss_copy_file
(
	const STRING szSrcFile,
	const STRING szDstFile
);
/*******************************************************************************
* 函数名称: cpss_file_close
* 功    能: 关闭本地文件。
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* ulFd                UINT32          输入              待关闭的文件描述符
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:
*******************************************************************************/
INT32 cpss_file_close
(
    UINT32 ulFd
)
{
	INT32 lClose = 0;
	lClose = close(ulFd);
	if ( ERROR == lClose )
	{
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
			"close file handle (%d) failed!",ulFd) ;
		return CPSS_ERROR;
	}
	return CPSS_OK;
}
/*******************************************************************************
* 函数名称: cpss_file_seek
* 功    能: 对打开的本地文件设置读写位置。
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* ulFd                UINT32          输入              待设置的文件描述符
* ulOffset             UINT32          输入              设置偏移量
* lOrigin             INT32           输入              偏移类型
* pulSeekPos          UINT32          输出              距离文件起始位置的偏移量指针
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:
*           偏移类型：
*                CPSS_SEEK_SET，设置为lOffset
*                CPSS_SEEK_CUR，设置为 当前位置+ lOffset
*                CPSS_SEEK_END，设置为 文件尾位置+ lOffset
*                pulSeekPos:如果输入NULL则表示不需要输出该参数
*******************************************************************************/
INT32 cpss_file_seek
(
	UINT32 ulFd,
	UINT32 ulOffset,
	INT32 lOrigin,
	UINT32 *pulSeekPos
)
{

    INT32 lPos=CPSS_ERROR;
    /*
    调用操作系统的实现函数
    */

    lPos = lseek(ulFd, ulOffset, lOrigin);
    if ( ERROR == lPos )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "lseek file handle (%d) failed!",ulFd) ;
        return CPSS_ERROR;
    }

    if(pulSeekPos != NULL)
    {
        *pulSeekPos = (UINT32)lPos ;
    }

    return CPSS_OK;
}
/*******************************************************************************
* 函数名称: cpss_file_read
* 功    能: 读出已经打开文件的内容。
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* ulFd                UINT32           输入             待读文件的描述符
* pvBuf               VOID*            输出             文件内容读出之后，存放的缓存区域
* ulMaxLen             UINT32           输入             缓存区域的最大尺寸
* pulReadLen          UINT32*          输出             实际读出的字节长度
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:
*******************************************************************************/
INT32 cpss_file_read
(
    UINT32 ulFd,
    VOID *pvBuf,
    UINT32 lMaxLen,
    UINT32 *pulReadLen
)
{
	INT32 lRet = CPSS_OK;
    INT32  lCount ;
    UINT32 ulReadBufAddr ;
    UINT32 ul64MemEndBufAddr ;
    UINT32 ulReadPointer = 0 ;
    UINT32 ulOnceReadMax ;
    UINT32 ulReadLen = 0 ;
    UINT32 pucReadBuf;
    UINT32 ulBufLen = lMaxLen;
    ulReadBufAddr = (UINT32)pvBuf ;
    pucReadBuf = (UINT32)pvBuf;
    ul64MemEndBufAddr = (ulReadBufAddr|0x0000ffff) ;
    ulOnceReadMax = ul64MemEndBufAddr-ulReadBufAddr+1 ;

    while(1)
    {
        if(ulOnceReadMax >= ulBufLen)
        {

            lCount = read(ulFd,pucReadBuf+ulReadPointer, ulBufLen) ;

            if ( -1 == lCount )
			{
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
					"read file handle (%d) failed!",ulFd) ;
				return CPSS_FD_INVALID;
			}

            ulReadLen = ulReadLen + (UINT32)lCount ;

            break ;
        }
        else
        {

            lCount = read(ulFd,pucReadBuf+ulReadPointer, ulOnceReadMax) ;
            if ( -1 == lCount )
            {
            	cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
					"read file handle (%d) failed!",ulFd) ;
				return CPSS_FD_INVALID;
            }

            ulReadLen = ulReadLen + (UINT32)lCount ;

            if( 0 == lCount )
            {
                break ;
            }

            ulBufLen = ulBufLen - ulOnceReadMax ;
            ulReadPointer = ulReadPointer + ulOnceReadMax ;
            ulOnceReadMax = 0x10000 ;
            continue ;
        }
    }
    *pulReadLen = ulReadLen;
    return lRet;

}
/*******************************************************************************
* 函数名称: cpss_file_open
* 功    能: 打开本地文件。
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szFileName          const STRING    输入              待打开的文件名指针
* lFlag               INT32           输入              打开文件之后的读写类型(如FS_FILE_ORDONLY)
* 函数返回:
*       成功：文件描述符;
*       失败：CPSS_FD_INVALID；
* 说   明:如果存在打开标记CPSS_FILE_OCREAT,并且文件已经存在,则打开文件,并清除文件内容
*******************************************************************************/
UINT32 cpss_file_open
(
    const STRING szFileName,
    INT32 lFlag
)
{
    INT32 lFileHandle;
    INT32 lRet;
	lFileHandle=open(szFileName,lFlag);

    if(lFileHandle == CPSS_FD_INVALID)
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "open file handle (%d) failed!",lFileHandle) ;
        lRet = CPSS_FD_INVALID;
    }
    lRet = lFileHandle;
	return lRet;
}
/*******************************************************************************
* 函数名称: cpss_file_get_size
* 功    能: 获取文件的字节尺寸
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szFileName          const STRING    输入              被操作的文件名(含绝对路径名)指针
* pulSize             UINT32*         输出              文件字节大小
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:
*******************************************************************************/
INT32 cpss_file_get_size
(
    const STRING szFileName,
    UINT32 *pulSize
)
{
    struct stat stStat;
    /*
    调用操作系统函数
    */
    if ( ERROR == stat(szFileName, &stStat))
    {
		cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
			"get (%s) attr faile.",szFileName) ;

		return CPSS_ERROR;
    }
    *pulSize = stStat.st_size;
    return CPSS_OK;
}
/*******************************************************************************
* 函数名称: cpss_file_delete
* 功    能: 删除本地文件
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szFileName          const STRING    输入              待删除的文件名指针
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:
*******************************************************************************/
INT32 cpss_file_delete
(
		const STRING szFileName
)
{
    CHAR   acFileName[ CPSS_FS_FILE_ABSPATH_LEN] ;
    INT32 lFileNameChk;
    BOOL bFileExist;
    INT32 lUnlink = 0;

    if(szFileName == NULL)
    {
        return CPSS_ERR_FS_FILENAME_NULL ;
    }
    strncpy(acFileName,szFileName, CPSS_FS_FILE_ABSPATH_LEN) ;

    /*对文件名进行合法性检查*/
    lFileNameChk = cpss_filename_check(acFileName,TRUE);
    if( CPSS_OK != lFileNameChk )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "(%s) name is illegal.",acFileName) ;
        return CPSS_ERR_FS_FILENAME_ILLEGAL ;
    }

    /*判断文件是否存在*/
    bFileExist = cpss_file_exist(acFileName);
    if( FALSE == bFileExist )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "(%s) is not exist.",acFileName) ;
        return CPSS_ERR_FS_FILE_NOT_EXIST ;
    }

    /*unlink待删除的文件*/
#ifdef CPSS_VOS_WINDOWS                /* Win32-NT 操作系统部分 */
    /*转化文件名的*/
    cpss_fs_filename_change(acFileName) ;
    lUnlink = _unlink(acFileName);
    if ( -1 == lUnlink )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "_unlink file (%s) failed!",acFileName) ;
        return cpss_fs_errno_get() ;
    }
#elif defined CPSS_VOS_VXWORKS     /* VxWorks操作系统部分 */
    lUnlink = unlink(acFileName);
    if ( ERROR == lUnlink )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "unlink file (%s) failed!",acFileName) ;
        return cpss_fs_errno_get() ;
    }
#elif defined CPSS_VOS_LINUX

    lUnlink = unlink(acFileName);
    if ( ERROR == lUnlink )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "unlink file (%s) failed!",acFileName) ;
        return CPSS_ERROR;
    }
#endif

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_dir_create
* 功    能: 创建本地目录
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szDirName           const STRING    输入              待创建的目录名(含绝对路径名)
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:
*******************************************************************************/
INT32 cpss_dir_create
(
const STRING szDirName
)
{
    CHAR acDirName[ CPSS_FS_FILE_ABSPATH_LEN];
    INT32 lFileNameChk = 0;
    BOOL bFileExist = 0;
#ifdef CPSS_VOS_WINDOWS
    INT32 lMakeDir = 0;
#endif

    if(szDirName == NULL)
    {
        return CPSS_ERR_FS_FILENAME_NULL ;
    }
    strncpy(acDirName,szDirName, CPSS_FS_FILE_ABSPATH_LEN);

    /*
        检查文件名是否有效
     */
    lFileNameChk = cpss_filename_check(acDirName,TRUE);
    if( CPSS_OK != lFileNameChk )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "dirname(%s) is illegal", acDirName) ;
        return CPSS_ERR_FS_FILENAME_ILLEGAL;
    }
    bFileExist = cpss_file_exist(acDirName);
    if( TRUE == bFileExist)
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "dirname(%s) is exist", acDirName) ;
        return CPSS_OK ;
    }

    /*
    调用操作系统的实现函数
    */
#ifdef CPSS_VOS_WINDOWS            /* Win32-NT 操作系统部分 */
    /*转化文件名的*/
    cpss_fs_filename_change(acDirName) ;
    lMakeDir = _mkdir(acDirName);
    if ( -1 == lMakeDir )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "_mkdir execute error! filename (%s)", acDirName) ;
        return cpss_fs_errno_get() ;
    }
#elif defined CPSS_VOS_VXWORKS     /* VxWorks操作系统部分 */
    {
        INT32   lFd;

        lFd = open(acDirName, O_RDWR | O_CREAT, FSTAT_DIR | DEFAULT_DIR_PERM);
        if( lFd==ERROR )
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "mkdir(open) execute error! filename (%s), error=%d", acDirName, errnoGet()) ;
            return cpss_fs_errno_get();
        }
        else
        {
            close(lFd);
        }
    }
#elif defined CPSS_VOS_LINUX
    {
        INT32   lFd;

        lFd = mkdir(acDirName,77777);
        if( lFd==ERROR )
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "mkdir(open) execute error! filename (%s), error=%d", acDirName,CPSS_ERROR) ;
            return CPSS_ERROR;
        }
        else
        {
            close(lFd);
        }
    }

#endif                          /* #ifdef CPSS_VOS_LINUX 结束 */

    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_fs_dir_if_recursion
* 功    能: 判断目录拷贝过程中是否存在递归拷贝的情况
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szSrcDir            const STRING    输入              源目录(含绝对路径名)
* szDstDir            const STRING    输入              目的目录(含绝对路径名)
* 函数返回:
*       存在递归现象：  TRUE;
*       不存在递归现象：FALSE；
* 说   明:
*******************************************************************************/
BOOL cpss_fs_dir_if_recursion(STRING szSrcDir,STRING szDstDir)
{
    UINT32 ulCount = 0 ;

    /*得到最后的字符*/
    while(szSrcDir[ulCount] == szDstDir[ulCount])
    {
        if((szSrcDir[ulCount]=='0')||(szDstDir[ulCount]=='0'))
        {
            break ;
        }
        ulCount++ ;
    }

    /*去掉多余的判断*/

    /*判断是否存在循环嵌套*/
    if((szSrcDir[ulCount] == '\0')&&
        ((szDstDir[ulCount] == '\\')||
        (szDstDir[ulCount] == '/')))
    {
        return TRUE ;
    }

    return FALSE ;
}

/*******************************************************************************
* 函数名称: cpss_file_is_dir
* 功    能: 判断文件对象是否是目录
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szFileName          const STRING    输入              文件名(含绝对路径名)指针
* pbIsDir             BOOL*           输出              是否是目录：TRUE，是；FALSE，否
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:
*******************************************************************************/
INT32 cpss_file_is_dir
(
const STRING szFileName,
BOOL *pbIsDir
)
{
	struct stat stStat;
    INT32 lFileStat = 0;

    if((szFileName == NULL)||(pbIsDir == NULL))
    {
        return CPSS_ERR_FS_PARA_NULL ;
    }

    if ( ERROR == stat(szFileName, &stStat))
    {
        return CPSS_ERROR ;
    }

    if(S_ISDIR(stStat.st_mode))
    {
    	*pbIsDir = TRUE;
    }
    else
    {
    	*pbIsDir = FALSE;
    }

    return (CPSS_OK);
}


/*******************************************************************************
* 函数名称: cpss_dir_delete
* 功    能: 删除本地目录
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szDirName           const STRING    输入              待删除的目录名(含绝对路径名)
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:    包括嵌套的子目录和文件
*******************************************************************************/
INT32 cpss_dir_delete
(
const STRING szDirName
)
{
    INT32 lRet ;

    lRet = cpss_fs_xdelete(szDirName) ;

    if(CPSS_OK != lRet)
    {
        return lRet ;
    }

    return CPSS_OK ;

}

/*******************************************************************************
* 函数名称: cpss_fs_handle_add(UINT32 ulDirHandle)
* 功    能:
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       成功：目录描述符;
*       失败：CPSS_FD_INVALID；
* 说   明:
*******************************************************************************/
INT32 cpss_fs_handle_add
(
    UINT32 ulDirHandle
)
{
    CPSS_FS_HANDLE_NODE_T* pstFsHandleNode ;

    cpss_vos_mutex_p(g_ulCpssFsSem,WAIT_FOREVER) ;

    pstFsHandleNode = g_stFsHandleLink.pstNodeHdr ;

    g_stFsHandleLink.pstNodeHdr = cpss_mem_malloc(sizeof(CPSS_FS_HANDLE_NODE_T));

    if(NULL == g_stFsHandleLink.pstNodeHdr)
    {
    	/*恢复原来的指针值*/
        g_stFsHandleLink.pstNodeHdr = pstFsHandleNode ;

        cpss_vos_mutex_v(g_ulCpssFsSem) ;

        return CPSS_ERROR ;
    }

    g_stFsHandleLink.pstNodeHdr->ulFsHandle = ulDirHandle ;
    g_stFsHandleLink.pstNodeHdr->pvNext = (VOID*)pstFsHandleNode ;

    cpss_vos_mutex_v(g_ulCpssFsSem) ;

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_dir_open
* 功    能: 打开目录，然后可以读出目录下的内容，包括文件和目录
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szDirName           const STRING    输入              待打开的目录名(含绝对路径名)
* 函数返回:
*       成功：目录描述符;
*       失败：CPSS_FD_INVALID；
* 说   明:
*******************************************************************************/
UINT32 cpss_dir_open
(
const STRING szDirName
)
{
    CHAR acDirName[ CPSS_FS_FILE_ABSPATH_LEN];
    INT32 lFileNameChk = 0;

    if(szDirName == NULL)
    {
        return CPSS_FD_INVALID ;
    }
    strncpy(acDirName,szDirName, CPSS_FS_FILE_ABSPATH_LEN);

    /*
    检查文件名是否有效
    */
    lFileNameChk = cpss_filename_check(acDirName,TRUE);
    if( CPSS_OK != lFileNameChk)
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,"(%s) name is illegal") ;
        return CPSS_FD_INVALID;
    }

    /*
    调用操作系统的实现函数
    */
#ifdef CPSS_VOS_WINDOWS
    {
        CHAR  acPath[ CPSS_FS_FILE_ABSPATH_LEN];
        struct _finddata_t stFileInfo;
        CPSS_FILE_DIR_T *pstDir ;
        INT32 lHandle;
        STRING szCurrWorkDir;
        INT32 lChgDir = 0;

        szCurrWorkDir = _getcwd( acPath,  CPSS_FS_FILE_ABSPATH_LEN );
        if( NULL == szCurrWorkDir )
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "_getcwd get curr dir(%s) failed!",acPath) ;
            return CPSS_FD_INVALID;
        }
        cpss_fs_filename_change(acDirName) ;

        lChgDir = _chdir(acDirName);
        if ( -1 == lChgDir )
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "_chdir change dir(%s) failed!",acDirName) ;
            return CPSS_FD_INVALID;
        }



         lHandle = _findfirst("*", &stFileInfo);
        if ( lHandle == -1 )
        {
            _chdir(acPath) ;   //   modifed by hewei 20090428

            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "_findfirst (%s) failed!",acDirName) ;
            return CPSS_FD_INVALID ;
        }
        lChgDir = _chdir(acPath);
        if (-1 == lChgDir)
        {

            _findclose(lHandle) ;
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "_chdir to (%s) failed.",acDirName) ;
            return CPSS_FD_INVALID;
        }

        pstDir = malloc(sizeof(CPSS_FILE_DIR_T)) ;
        if(pstDir == NULL)
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "malloc memory failed.") ;
            return CPSS_FD_INVALID ;
        }

        pstDir->lHandle = lHandle ;
        memcpy(pstDir->acFileName,stFileInfo.name,strlen(stFileInfo.name)+1) ;

        if(CPSS_OK != cpss_fs_handle_add((UINT32)pstDir))
        {
            return CPSS_FD_INVALID ;
        }

        g_ulCpssDirOpened++ ;
        return (UINT32)pstDir ;
    }
#elif defined CPSS_VOS_VXWORKS
    {
        DIR * pstDir ;

        pstDir=opendir(acDirName);
        if( pstDir == NULL )
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "opendir (%s) failed!",acDirName) ;
            return CPSS_FD_INVALID ;
        }

        if(CPSS_OK != cpss_fs_handle_add((UINT32)pstDir))
        {
            return CPSS_FD_INVALID ;
        }

        g_ulCpssDirOpened++ ;
        return (UINT32)pstDir ;
    }
#elif defined CPSS_VOS_LINUX
    {
        DIR * pstDir ;

        /*打开目录*/
        pstDir=opendir(acDirName);
        if( pstDir == NULL )
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "opendir (%s) failed!",acDirName) ;
            return CPSS_FD_INVALID ;
        }
#if 	0 
        /*将目录句柄加入管理链表*/
        if(CPSS_OK != cpss_fs_handle_add((UINT32)pstDir))
        {
            return CPSS_FD_INVALID ;
        }

        g_ulCpssDirOpened++ ;
#endif 
        return (UINT32)pstDir ;
    }
#endif                          /* #ifdef CPSS_VOS_LINUX 结束 */
}


/*******************************************************************************
* 函数名称: cpss_fs_handle_delete(UINT32 ulDirHandle)
* 功    能:
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_fs_handle_delete
(
    UINT32 ulDirHandle
)
{

    CPSS_FS_HANDLE_NODE_T* pstFsHandleNode ;
    CPSS_FS_HANDLE_NODE_T* pstFsHandleNodePre ;

    cpss_vos_mutex_p(g_ulCpssFsSem,WAIT_FOREVER) ;

    pstFsHandleNode = g_stFsHandleLink.pstNodeHdr ;

    if(NULL == pstFsHandleNode)
    {
        cpss_vos_mutex_v(g_ulCpssFsSem) ;
        return CPSS_ERROR ;
    }

    pstFsHandleNodePre = g_stFsHandleLink.pstNodeHdr ;

    while(NULL != pstFsHandleNode)
    {
    	/*如果找到则删除此节点*/
        if(ulDirHandle == pstFsHandleNode->ulFsHandle)
        {
        	/*如果是链头的首节点*/
            if(pstFsHandleNode == pstFsHandleNodePre)
            {
                g_stFsHandleLink.pstNodeHdr = pstFsHandleNode->pvNext ;
                cpss_mem_free((VOID*)pstFsHandleNode);
            }
            else
            {
                pstFsHandleNodePre->pvNext = pstFsHandleNode->pvNext ;
                cpss_mem_free((VOID*)pstFsHandleNode);
            }
            cpss_vos_mutex_v(g_ulCpssFsSem) ;
            return CPSS_OK ;
        }
        pstFsHandleNodePre = pstFsHandleNode ;
        pstFsHandleNode = (CPSS_FS_HANDLE_NODE_T*)pstFsHandleNode->pvNext ;
    }

    cpss_vos_mutex_v(g_ulCpssFsSem) ;
    return CPSS_ERROR ;
}


/*******************************************************************************
* 函数名称: cpss_dir_close
* 功    能: 关闭文件目录
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* ulDd                UINT32          输入              待关闭的目录名(含绝对路径名)
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:
*******************************************************************************/
INT32 cpss_dir_close
(
UINT32 ulDd
)
{
	/*
	调用操作系统的实现函数
	*/
#ifdef CPSS_VOS_WINDOWS
    {
        CPSS_FILE_DIR_T *pstDir ;
        INT32 lCloseDir = 0;

        if(CPSS_OK != cpss_fs_handle_delete(ulDd))
        {
            return CPSS_ERR_FS_HANDLE_NOT_EXIST ;
        }

        pstDir = (CPSS_FILE_DIR_T*)ulDd ;

        lCloseDir = _findclose(pstDir->lHandle);
        if ( -1 == lCloseDir)
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,"_findclose failed!") ;
            free(pstDir) ;
            return cpss_fs_errno_get() ;
        }
        free(pstDir) ;
    }

#elif defined CPSS_VOS_VXWORKS
    {
        DIR *pstDir ;
        STATUS lCloseDir = 0;

        if(CPSS_OK != cpss_fs_handle_delete(ulDd))
        {
            return CPSS_ERR_FS_HANDLE_NOT_EXIST ;
        }

        pstDir = (DIR*)ulDd ;

        lCloseDir = closedir(pstDir);
        if ( ERROR == lCloseDir )
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,"closedir failed!") ;
            return cpss_fs_errno_get() ;
        }
    }
#elif defined CPSS_VOS_LINUX
    {
        DIR *pstDir ;
        STATUS lCloseDir = 0;

#if		0 
        if(CPSS_OK != cpss_fs_handle_delete(ulDd))
        {
            return CPSS_ERR_FS_HANDLE_NOT_EXIST ;
        }
#endif 
        pstDir = (DIR*)ulDd ;

        lCloseDir = closedir(pstDir);
        if ( ERROR == lCloseDir )
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,"closedir failed!") ;
            return CPSS_ERROR;
        }
    }
#endif

    g_ulCpssDirOpened-- ;
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_fs_dir_delete
* 功    能: 删除本地目录
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szDirName           const STRING    输入              待删除的目录名(含绝对路径名)
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:    包括嵌套的子目录和文件
*******************************************************************************/
INT32 cpss_fs_dir_delete
(
STRING szDirName
)
{
    CHAR acDirName[ CPSS_FS_FILE_ABSPATH_LEN];
    INT32 lFileNameChk = 0;
    BOOL bFileExist = 0;
    INT32 lRmDir = 0;

    if(szDirName == NULL)
    {
        return CPSS_ERR_FS_FILENAME_NULL ;
    }
    strncpy(acDirName,szDirName, CPSS_FS_FILE_ABSPATH_LEN);

    /*
    检查文件名是否有效
    */
    lFileNameChk = cpss_filename_check(acDirName,TRUE);
    if( CPSS_OK!= lFileNameChk)
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "(%s) is not exist.",acDirName) ;
        return CPSS_ERR_FS_FILENAME_ILLEGAL;
    }

    /*
    如果被删除的目录不存在，则返回ERROR
    */
    bFileExist = cpss_file_exist(acDirName);
    if( FALSE == bFileExist )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "(%s) name is illegal") ;
        return CPSS_ERR_FS_FILE_NOT_EXIST;
    }

    /*
    调用操作系统的实现函数
    */
#ifdef CPSS_VOS_WINDOWS
    {


        cpss_fs_filename_change(acDirName) ;
        lRmDir = _rmdir(acDirName);
        if( 0 != lRmDir )
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "system delete dir(%s) failed",acDirName) ;
            return cpss_fs_errno_get() ;
        }
    }

#elif defined CPSS_VOS_VXWORKS
    lRmDir = rmdir(acDirName);
    if (ERROR == lRmDir)
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "rmdir failed dirname (%s)",acDirName) ;
        return cpss_fs_errno_get() ;
    }
#elif defined CPSS_VOS_LINUX
    lRmDir = rmdir(acDirName);
    if (ERROR == lRmDir)
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "rmdir failed dirname (%s)",acDirName) ;
        return CPSS_ERROR ;
    }
#endif

    return (CPSS_OK);
}

/*******************************************************************************
* 函数名称: cpss_fs_dirpath_link
* 功    能: 将目录名和文件名组成新的目录路径
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szDirPath           STRING          输入(输出)        待操作的目录名指针
* szDirName           STRING          输入              文件名称
* ulPathLen           UINT32          输入              数组的长度
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:
*******************************************************************************/
INT32 cpss_fs_dirpath_link
(
STRING szDirPath ,
STRING szDirName ,
UINT32 ulPathLen
)
{
    UINT32 ulStrLen ;
    UINT32 ulSumStrLen;

    ulSumStrLen = strlen(szDirPath)+strlen(szDirName);
    if( ulSumStrLen > ulPathLen)
    {
        return CPSS_ERROR ;
    }

    ulStrLen = strlen(szDirPath) ;

#ifdef CPSS_VOS_WINDOWS
    if(szDirPath[ulStrLen-1] != '\\')
    {
        szDirPath[ulStrLen] = '\\' ;
        ulStrLen++ ;
    }
#elif defined CPSS_VOS_VXWORKS
    if(szDirPath[ulStrLen-1] != '/')
    {
        szDirPath[ulStrLen] = '/' ;
        ulStrLen++ ;
    }
#elif defined CPSS_VOS_LINUX
    if(szDirPath[ulStrLen-1] != '/')
    {
        szDirPath[ulStrLen] = '/' ;
        ulStrLen++ ;
    }
#endif

    strncpy(&szDirPath[ulStrLen],szDirName,strlen(szDirName)+1) ;

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_fs_dirpath_unlink
* 功    能: 得到上本级目录的上一级目录字符串
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szDirPath           STRING          输入(输出)        待操作的目录名指针
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:
*******************************************************************************/
INT32 cpss_fs_dirpath_unlink
(
STRING szDirPath
)
{
    INT32 ulStrLen ;

    ulStrLen = strlen(szDirPath) ;

    /*如果字符串中不是'\'或'/'则去掉此字符*/
    while((szDirPath[ulStrLen]!='\\')&&(szDirPath[ulStrLen]!='/'))
    {
        ulStrLen-- ;
        if(ulStrLen<0)
        {
            ulStrLen = 0 ;
            break ;
        }
    }

    szDirPath[ulStrLen] = '\0' ;

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_fs_handle_exist(UINT32 ulDirHandle)
* 功    能:
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       目录存在：TRUE;
*       目录不存在：FALSE;
* 说   明:
*******************************************************************************/
INT32 cpss_fs_handle_exist
(
    UINT32 ulDirHandle
)
{
    CPSS_FS_HANDLE_NODE_T* pstFsHandleNode ;

    cpss_vos_mutex_p(g_ulCpssFsSem,WAIT_FOREVER) ;

    pstFsHandleNode = g_stFsHandleLink.pstNodeHdr ;

    if(NULL == pstFsHandleNode)
    {
        cpss_vos_mutex_v(g_ulCpssFsSem) ;
        return FALSE ;
    }

    while(NULL != pstFsHandleNode)
    {
        if(pstFsHandleNode->ulFsHandle == ulDirHandle)
        {
            cpss_vos_mutex_v(g_ulCpssFsSem) ;
            return TRUE ;
        }
        pstFsHandleNode = (CPSS_FS_HANDLE_NODE_T*)pstFsHandleNode->pvNext ;
    }

    cpss_vos_mutex_v(g_ulCpssFsSem) ;
    return FALSE ;
}


/*******************************************************************************
* 函数名称: cpss_dir_read
* 功    能: 读取目录中的子项（文件或子目录）。
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* ulDd                UINT32          输入              待读取的目录描述符
* 函数返回:
*       成功：目录名或文件名;
*       失败：NULL；
* 说   明: 对于函数的返回值，应用模块不可以对其内容进行修改，
*          否则使后续的cpss_dir_read操作失败、甚至不可知后果
*           应用程序需将函数返回的字符串拷贝到自己的内存区中进行使用
*******************************************************************************/
STRING cpss_dir_read
(
UINT32 ulDd
)
{

#ifdef CPSS_VOS_WINDOWS
    {
        struct _finddata_t stFileInfo;
        CPSS_FILE_DIR_T *pstDir ;
        INT32 lFindNext = 0;
        INT32 lStrCmpCur = 0;
        INT32 lStrCmpPar = 0;

        if(FALSE == cpss_fs_handle_exist(ulDd))
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "dir_read|the dir handle(%d) is not exist",ulDd) ;
            return NULL ;
        }

        pstDir = (CPSS_FILE_DIR_T*)ulDd ;

        while (1)
        {
            lFindNext = _findnext(pstDir->lHandle,&stFileInfo);
            if (-1 == lFindNext)
            {
                return NULL;
            }
            else
            {
                lStrCmpCur = strcmp(stFileInfo.name, ".");
                lStrCmpPar = strcmp(stFileInfo.name, "..");
                if ((0 != lStrCmpCur) && (0 != lStrCmpPar))
                {
                    break;
                }
            }
        }

        memcpy(pstDir->acFileName,stFileInfo.name,strlen(stFileInfo.name)+1) ;
        return pstDir->acFileName ;
    }

#elif defined CPSS_VOS_VXWORKS
    {
        DIR *pstDir ;
        struct dirent *pstDirent ;
        INT32 lStrCmpCur = 0;
        INT32 lStrCmpPar = 0;

        if(FALSE == cpss_fs_handle_exist(ulDd))
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "dir_read|the dir handle(%d) is not exist",ulDd) ;
            return NULL ;
        }

        pstDir = (DIR *)ulDd ;

        while (1)
        {
            pstDirent = readdir(pstDir);
            if (NULL == pstDirent)
            {
                return NULL;
            }
            else
            {
                lStrCmpCur = strcmp(pstDirent->d_name, ".");
                lStrCmpPar = strcmp(pstDirent->d_name, "..");
                if ((0 != lStrCmpCur) && (0 != lStrCmpPar))
                {
                    break;
                }
            }
        }
        return pstDirent->d_name ;
    }
#elif defined CPSS_VOS_LINUX
    {
        DIR *pstDir ;
        struct dirent *pstDirent ;
        INT32 lStrCmpCur = 0;
        INT32 lStrCmpPar = 0;

#if 	0 
        if(FALSE == cpss_fs_handle_exist(ulDd))
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "dir_read|the dir handle(%d) is not exist",ulDd) ;
            return NULL ;
        }
#endif 
        pstDir = (DIR *)ulDd ;

        while (1)
        {
            pstDirent = readdir(pstDir);
            if (NULL == pstDirent)
            {
                return NULL;
            }
            else
            {
                lStrCmpCur = strcmp(pstDirent->d_name, ".");
                lStrCmpPar = strcmp(pstDirent->d_name, "..");
                if ((0 != lStrCmpCur) && (0 != lStrCmpPar))
                {
                    break;
                }
            }
        }
        return pstDirent->d_name ;
    }
#endif
}

/*******************************************************************************
* 函数名称: cpss_fs_xdelete
* 功    能: 循环删除本地目录
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szDirName           const STRING    输入              待删除的目录名(含绝对路径名)
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:    包括嵌套的子目录和文件
*******************************************************************************/
INT32 cpss_fs_xdelete(STRING szDirPath)
{
    UINT8 aucDirPath[CPSS_FS_FILE_ABSPATH_LEN+1] ;
    UINT8 aucFileName[CPSS_FS_FILENAME_LEN+1] ;
    UINT8 *pucFileName ;
    BOOL bIsExist ;
    BOOL bIsDir ;
    UINT32 ulDirFd ;
    INT32 lRet ;

    strncpy(aucDirPath,szDirPath,CPSS_FS_FILE_ABSPATH_LEN) ;

    if(CPSS_OK != cpss_filename_check(aucDirPath,TRUE))
    {
        return CPSS_ERR_FS_FILENAME_ILLEGAL ;
    }

    bIsExist = cpss_file_exist(aucDirPath) ;
    if(FALSE == bIsExist)
    {
        return CPSS_OK ;
    }
    else
    {
        cpss_file_is_dir(aucDirPath,&bIsDir) ;
        if(FALSE == bIsDir)
        {
            cpss_file_delete(aucDirPath) ;
            return CPSS_OK ;
        }
        else
        {
            ulDirFd = cpss_dir_open(aucDirPath);
            if(CPSS_FD_INVALID == ulDirFd)
            {
                return CPSS_ERR_FS_OPEN_FILE_FAIL ;
            }
            while(1)
            {
                pucFileName = cpss_dir_read(ulDirFd) ;
                if(NULL == pucFileName)
                {
                    cpss_dir_close(ulDirFd) ;

                    /*删除目录*/
                    cpss_fs_dir_delete(aucDirPath) ;

                    return CPSS_OK ;
                }
                else
                {
                    if(strlen(pucFileName) > CPSS_FS_FILENAME_LEN)
                    {
                        cpss_dir_close(ulDirFd) ;
                        return CPSS_ERR_FS_FILENAME_ILLEGAL ;
                    }
                    strncpy(aucFileName,pucFileName,CPSS_FS_FILENAME_LEN) ;

                    /*组织新目录名*/
                    lRet =cpss_fs_dirpath_link(aucDirPath,aucFileName,CPSS_FS_FILE_ABSPATH_LEN) ;
                    if(CPSS_OK != lRet)
                    {
                        cpss_dir_close(ulDirFd) ;
                        return CPSS_ERR_FS_LINK_NAME_FAIL ;
                    }

                    lRet = cpss_fs_xdelete(aucDirPath) ;
                    if(CPSS_OK != lRet)
                    {
                        cpss_dir_close(ulDirFd) ;
                        return lRet ;
                    }

                    cpss_fs_dirpath_unlink(aucDirPath) ;
                }
            }
        }
    }
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_file_move
* 功    能: 移动文件或目录到指定的文件或目录
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szSrcFile           STRING          输入              待移动文件或目录的路径
* szDestFile          STRING          输入              目的文件或目录路径
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:  如果目的文件存在,则覆盖目的文件
*******************************************************************************/
INT32 cpss_file_move
(
    const STRING szSrcFile,
    const STRING szDestFile
)
{
    BOOL bIsDir ;
    INT32 lFileNameChkSrc = 0;
    INT32 lFileNameChkDst = 0;
    BOOL bDirRecur = FALSE;
    INT32 lFileIsDir = 0;
    BOOL bFileExist = FALSE;
    INT32 lDirCreate = 0;
    INT32 lFsDirRecur = 0;
    INT32 lMoveEx = 0;

    INT8 acSrcFile[ CPSS_FS_FILE_ABSPATH_LEN];
    INT8 acDestFile[ CPSS_FS_FILE_ABSPATH_LEN];

    if((szSrcFile == NULL)||(szDestFile == NULL))
    {
        return CPSS_ERR_FS_FILENAME_NULL ;
    }

    strncpy(acSrcFile,szSrcFile, CPSS_FS_FILE_ABSPATH_LEN);
    strncpy(acDestFile,szDestFile, CPSS_FS_FILE_ABSPATH_LEN);

    /*
    检查文件名是否有效
    */
    lFileNameChkSrc = cpss_filename_check(acSrcFile,TRUE);
    lFileNameChkDst = cpss_filename_check(acDestFile,TRUE);
    if( lFileNameChkSrc !=CPSS_OK || lFileNameChkDst !=CPSS_OK )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
             "the file name is illegal srcFile=%s,destFile=%s",acSrcFile,acDestFile) ;
        return CPSS_ERR_FS_FILENAME_ILLEGAL;
    }

    /*判断目录是否存在嵌套拷贝的情况*/
    bDirRecur = cpss_fs_dir_if_recursion(acSrcFile,acDestFile);
    if( bDirRecur == TRUE )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
             "(%s) and (%s) dir move recursion.",acSrcFile,acDestFile) ;
        return CPSS_ERR_FS_RECURSION_EXIST ;
    }

    /*目的文件是否是目录?*/
    lFileIsDir = cpss_file_is_dir(acSrcFile,&bIsDir);
    if( lFileIsDir != CPSS_OK )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
             "judge (%s) is dir? failed.",acSrcFile) ;
        return lFileIsDir ;
    }

    if( bIsDir==TRUE )
    {
        bFileExist = cpss_file_exist(acDestFile);
        lDirCreate = cpss_dir_create(acDestFile);
        if ( !bFileExist  && (CPSS_OK != lDirCreate) )
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "CPSS_FS:(%s)is not exist or create failed.",acDestFile) ;
            return lDirCreate;
        }
        /*递归移动文件*/
        lFsDirRecur = cpss_fs_dir_recursive(acSrcFile, acDestFile,TRUE,cpss_file_move);
        if( lFsDirRecur != CPSS_OK)
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "CPSS_FS:move file (%s) to (%s) recusion failed.",acSrcFile,acDestFile) ;
            return lFsDirRecur ;
        }
    }
    else /*移动文件操作*/
#ifdef CPSS_VOS_WINDOWS                /* Win32-NT 操作系统部分 */
    {
        cpss_fs_filename_change(acSrcFile) ;
        cpss_fs_filename_change(acDestFile) ;

        lMoveEx = MoveFileEx(acSrcFile, acDestFile,MOVEFILE_REPLACE_EXISTING);
        if(!lMoveEx)
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "move file (%s) to (%s) failed.",acSrcFile,acDestFile) ;
            return CPSS_ERROR;
        }
    }
#elif defined CPSS_VOS_VXWORKS     /* VxWorks操作系统部分 */
    {
        lMoveEx = mv(acSrcFile,acDestFile);
        if (ERROR == lMoveEx)
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "move file (%s) to (%s) failed.",acSrcFile,acDestFile) ;
            return cpss_fs_errno_get() ;
        }
    }
#elif defined CPSS_VOS_LINUX     /* LINUX操作系统部分 */
    {
        lMoveEx = rename(acSrcFile,acDestFile);
        if (ERROR == lMoveEx)
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "move file (%s) to (%s) failed.",acSrcFile,acDestFile) ;
            return CPSS_ERROR ;
        }
    }
#endif                         /* #ifdef CPSS_VOS_LINUX */
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_fs_dir_recursive
* 功    能: 文件移动,目录拷贝的递归函数
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szSrcDir            const STRING    输入              源目录(含绝对路径名)
* szDstDir            const STRING    输入              目的目录(含绝对路径名)
* bReplace            BOOL            输入              是否覆盖标志
* pFunc               FS_FUNC         输入              拷贝函数
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:
*******************************************************************************/
extern VOID cpss_vk_dir_copy_stat(VOID);

INT32 cpss_fs_dir_recursive
(
const STRING szSrcDir,
const STRING szDestDir,
BOOL bReplace,
FS_FUNC pfFunc
)
{

    CHAR acDestName[CPSS_FS_FILE_ABSPATH_LEN] ;
    CHAR acSrcName[CPSS_FS_FILE_ABSPATH_LEN] ;
    CHAR acFileName[CPSS_FS_FILENAME_LEN] ;
    CHAR* pcFileName ;
    UINT32 ulHandle ;
    BOOL bIsDir ;
    INT32 lFsDirPathLink = 0;
    INT32 lFileIsDir = 0;
    BOOL bFileExist = 0;
    INT32 lDirCreate = 0;
    INT32 lFsDirRecur = 0;

    strncpy(acDestName,szDestDir, CPSS_FS_FILE_ABSPATH_LEN) ;
    strncpy(acSrcName,szSrcDir, CPSS_FS_FILE_ABSPATH_LEN) ;

    /*打开目录*/
    ulHandle=cpss_dir_open(acSrcName);
    if(CPSS_FD_INVALID == ulHandle)
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "open dir (%s) failed.",acSrcName) ;
        return CPSS_ERROR;
    }

    /*循环读取目录中的子项并进行递归*/
    while(NULL!=(pcFileName=cpss_dir_read(ulHandle)))
    {

        strncpy(acDestName,szDestDir, CPSS_FS_FILE_ABSPATH_LEN) ;
        strncpy(acSrcName,szSrcDir, CPSS_FS_FILE_ABSPATH_LEN) ;
        strncpy(acFileName,pcFileName, CPSS_FS_FILENAME_LEN) ;

        /*组织源目录的路径名称*/
        lFsDirPathLink= cpss_fs_dirpath_link(acSrcName,acFileName, CPSS_FS_FILE_ABSPATH_LEN);
        if( lFsDirPathLink != CPSS_OK )
        {
            cpss_dir_close(ulHandle);
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "make dir path (%s)(%s) failed.",acSrcName,acFileName) ;
            return CPSS_ERROR ;
        }

        /*判断源路径名是否是目录*/
        lFileIsDir = cpss_file_is_dir(acSrcName,&bIsDir);
        if(lFileIsDir != CPSS_OK)
        {
            cpss_dir_close(ulHandle);
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "get (%s) dir attr failed.",acSrcName) ;
            return CPSS_ERROR ;
        }

        /*组织目的目录的路径名称*/
        lFsDirPathLink = cpss_fs_dirpath_link(acDestName,acFileName, CPSS_FS_FILE_ABSPATH_LEN);
        if( lFsDirPathLink != CPSS_OK )
        {
            cpss_dir_close(ulHandle);
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "make dir path (%s)(%s) failed.",acDestName,acFileName) ;
            return CPSS_ERROR ;
        }

        if(bIsDir == TRUE)
        {
            bFileExist = cpss_file_exist(acDestName);
            if( bFileExist == FALSE)
            {
                lDirCreate = cpss_dir_create(acDestName);
                if( lDirCreate != CPSS_OK )
                {
                    cpss_dir_close(ulHandle);
                    cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                        "create dir (%s) failed.",acDestName) ;
                    return CPSS_ERROR ;
                }
            }
            lFsDirRecur = cpss_fs_dir_recursive(acSrcName, acDestName,TRUE,pfFunc) ;
            if( lFsDirRecur != CPSS_OK )
            {
                cpss_dir_close(ulHandle);
                cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                    "recusion (%s) to (%s) failed.",acSrcName,acDestName) ;
                return CPSS_ERROR ;
            }
        }
        else
        {
        	/*调用参数传入的函数操作源目的文件*/
            if((*pfFunc)(acSrcName, acDestName) != CPSS_OK)
            {
                /* cpss_dir_close(ulHandle);*/
                cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                    "recusion fun (%s) to (%s) failed.",acSrcName,acDestName) ;
              /*  return CPSS_ERROR ;*/
            }
	      else
	      	{
	      	    	cpss_vk_dir_copy_stat();
	      	}
        }
    }
    cpss_dir_close(ulHandle);

    /*如果需要进行文件删除则删除源文件*/
    if(pfFunc==cpss_file_move)
    {
        strncpy(acSrcName,szSrcDir, CPSS_FS_FILE_ABSPATH_LEN) ;
        cpss_dir_delete(acSrcName);
    }
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_file_write_64k
* 功    能: 按64K字节对齐写文件
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:
*******************************************************************************/
INT32 cpss_file_write_64k
(
 UINT32 ulFd,
 UINT8* pucWriteBuf,
 UINT32 ulBufLen
)
{
    INT32  lCount ;
    UINT32 ulWriteBufAddr ;
    UINT32 ul64MemEndBufAddr ;
    UINT32 ulWritePointer = 0 ;
    UINT32 ulOnceWriteMax ;

    ulWriteBufAddr = (UINT32)pucWriteBuf ;
    ul64MemEndBufAddr = (ulWriteBufAddr|0x0000ffff) ;
    ulOnceWriteMax = ul64MemEndBufAddr-ulWriteBufAddr+1 ;

    while(1)
    {
        if(ulOnceWriteMax >= ulBufLen)
        {
#ifdef CPSS_VOS_WINDOWS
            lCount = _write(ulFd,pucWriteBuf+ulWritePointer, ulBufLen) ;
#else
            lCount = write(ulFd,pucWriteBuf+ulWritePointer, ulBufLen) ;
#endif
            if ( -1 == lCount )
            {
                cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                    "_write file handle (%d) failed!",ulFd) ;
                return CPSS_ERROR;
}

            break ;
        }
        else
        {
#ifdef CPSS_VOS_WINDOWS
            lCount = _write(ulFd,pucWriteBuf+ulWritePointer, ulOnceWriteMax) ;
#else
            lCount = write(ulFd,pucWriteBuf+ulWritePointer, ulOnceWriteMax) ;
#endif
            if ( -1 == lCount )
            {
                cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                    "_write file handle (%d) failed!",ulFd) ;
                return CPSS_ERROR;
            }

            ulBufLen = ulBufLen - ulOnceWriteMax ;
            ulWritePointer = ulWritePointer + ulOnceWriteMax ;
            ulOnceWriteMax = 0x10000 ;
            continue ;
        }
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_file_write
* 功    能: 读出已经打开文件的内容。
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* ulFd                UINT32          输入              待写文件的描述符
* pvBuf               VOID*           输出              待写内容存放的缓存区域
* ulMaxLen             UINT32          输入              待写数据的缓存字节长度
* pulWriteLen         UINT32*         输出              实际写入的字节长度
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:如果实际写入的字节长度比lMaxLen小或返回值为负值，则说明文件写操作失败
*******************************************************************************/
INT32 cpss_file_write
(
UINT32 ulFd,
const VOID *pvBuf,
UINT32 ulMaxLen,
UINT32 *pulWriteLen
)
{
    INT32 lRet ;

    if((pvBuf == NULL)||(pulWriteLen == NULL))
    {
        return CPSS_ERR_FS_PARA_NULL ;
    }

    lRet = cpss_file_write_64k(ulFd,(UINT8*)pvBuf,ulMaxLen) ;
    *pulWriteLen = (UINT32)ulMaxLen;
    return lRet;
}


/*******************************************************************************
* 函数名称: cpss_dir_copy
* 功    能: 拷贝本地目录树。
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szSrcDir            const STRING    输入              源目录(含绝对路径名)
* szDstDir            const STRING    输入              目的目录(含绝对路径名)
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明:如果目标文件已经存在，则覆盖原有文件
*******************************************************************************/
INT32 cpss_dir_copy
(
const STRING szSrcDir,
const STRING szDstDir
)
{

    CHAR acSrcDir[ CPSS_FS_FILE_ABSPATH_LEN];
    CHAR acDestDir[ CPSS_FS_FILE_ABSPATH_LEN];
    BOOL bIsDir ;
    INT32 FileNameChkSrc = 0;
    INT32 FileNameChkDst = 0;
    BOOL bFileExist = FALSE;
    BOOL bDirRecur = FALSE;
    INT32 lFileIsDir = 0;
    INT32 lDirCreate = 0;
    INT32 lFsDirRecur = 0;
    INT32 lFileCpoy = 0;


    if((szSrcDir == NULL)||(szDstDir == NULL))
    {
        return CPSS_ERR_FS_FILENAME_NULL ;
    }

    strncpy(acSrcDir,szSrcDir, CPSS_FS_FILE_ABSPATH_LEN);
    strncpy(acDestDir,szDstDir, CPSS_FS_FILE_ABSPATH_LEN);

    /*
    检查文件名是否有效
    */
    FileNameChkSrc = cpss_filename_check(acSrcDir,TRUE);
    FileNameChkDst = cpss_filename_check(acDestDir,TRUE);
    if( CPSS_OK!= FileNameChkSrc || CPSS_OK!= FileNameChkDst )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "(%s) or (%s) is illegal.",acSrcDir,acDestDir) ;
        return CPSS_ERR_FS_FILENAME_ILLEGAL;
    }

    /*
    如果源目录不存在，则返回CPSS_ERROR
    */
    bFileExist = cpss_file_exist(acSrcDir);
    if( bFileExist != TRUE)
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "(%s) is not existed.",acSrcDir) ;
        return CPSS_ERR_FS_FILE_NOT_EXIST;
    }

    /*判断目录是否存在嵌套拷贝的情况*/
    bDirRecur= cpss_fs_dir_if_recursion(acSrcDir,acDestDir);
    if( bDirRecur == TRUE )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "(%s) and (%s) dir recursion.",acSrcDir,acDestDir) ;
        return CPSS_ERR_FS_RECURSION_EXIST ;
    }

    /*得到原文件是否为目录*/
    lFileIsDir = cpss_file_is_dir(acSrcDir,&bIsDir);
    if( lFileIsDir != CPSS_OK )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "get (%s) dir attr failed.",acSrcDir) ;
        return lFileIsDir ;
    }

    if(bIsDir == TRUE)
    {
    	/*如果目的文件不存在并且目录创建失败则返回CPSS_ERROR*/
        bFileExist = cpss_file_exist(acDestDir);
        if( bFileExist !=TRUE )
        {
            lDirCreate = cpss_dir_create(acDestDir);
            if( lDirCreate != CPSS_OK)
            {
                cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                    "create dir (%s) failed.",acDestDir) ;
                return lDirCreate ;
            }
        }

        /*通过递归进行文件拷贝*/
        lFsDirRecur = cpss_fs_dir_recursive(acSrcDir,acDestDir,TRUE,cpss_file_copy);
        if( lFsDirRecur != CPSS_OK)
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "dir copy (%s) to (%s) recrusion failed.",acSrcDir,acDestDir) ;
            return lFsDirRecur ;
        }
    }
    else
    {
        lFileCpoy = cpss_file_copy(acSrcDir,acDestDir);
        if( lFileCpoy != CPSS_OK )
        {
            cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
                "file copy (%s) to (%s) failed.",acSrcDir,acDestDir) ;
            return lFileCpoy ;
        }
    }
    return CPSS_OK ;
}
/*******************************************************************************
* 函数名称: cpss_fs_abs_path_check
* 功    能: 检查目录是否是绝对路径
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szFileName          STRING          输入              待操作的目录名指针
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_fs_abs_path_check(STRING szFileName)
{
#ifdef CPSS_VOS_WINDOWS
    if( ((szFileName[0] >= 'a' && szFileName[0] <= 'z')||
        (szFileName[0] >= 'A' && szFileName[0] <= 'Z'))&&
        (szFileName[1] == ':'))
    {
        return CPSS_OK ;
    }

    return CPSS_ERROR ;

#endif
#ifdef CPSS_VOS_LINUX
    if(szFileName[0] == '/')
    {
    	return CPSS_OK;
    }
    /*return CPSS_ERROR;*/
    return CPSS_OK;
#endif
}
/*******************************************************************************
* 函数名称: cpss_filename_check
* 功    能: 检查目录(文件)名是否合法
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* pcFileName          CHAR*           输入(输出)        待操作的目录名指针
* bIfAbsDir           BOOL            输入              是否需要判断绝对路径
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明: 检查文件名有效性，NULL指针，空字符串
检查文件名长度
路径节点分割符"\" "/"
文件名不能包含 ( :  *  ?  "  <  >  | )
*******************************************************************************/
INT32 cpss_filename_check(CHAR *pcFileName,BOOL bIfAbsDir)
{
    CHAR acPath[ CPSS_FS_FILE_ABSPATH_LEN+1] ;
    CHAR *pcPathOrFilePtr ;
    UINT32 ulFileNameLen = 0 ;
    UINT32 ulDirLen = 0 ;
    UINT32 ulStrLen = 0;
    INT32 lFsAbsPathChk = 0;

    acPath[CPSS_FS_FILE_ABSPATH_LEN] = 0 ;

    /*检查文件名*/
    if (NULL == pcFileName)
    {
        return CPSS_ERROR;
    }

    strncpy(acPath,pcFileName, CPSS_FS_FILE_ABSPATH_LEN) ;

    pcPathOrFilePtr = acPath ;

    /*如果目录长度大于最大长度返回失败*/
    ulStrLen = strlen(pcPathOrFilePtr);
    if( ulStrLen> CPSS_FS_FILE_ABSPATH_LEN )
    {
        return CPSS_ERROR;
    }

    /*检查目录名中是否有不合法的字符.*/
    while(*pcPathOrFilePtr)
    {
        if((*pcPathOrFilePtr == '*')||(*pcPathOrFilePtr == '?')||(*pcPathOrFilePtr == '\"')||(*pcPathOrFilePtr == '<')||
            (*pcPathOrFilePtr == '>')||(*pcPathOrFilePtr == '|'))
        {
            return CPSS_ERROR ;
        }
#ifdef CPSS_VOS_WINDOWS
        if('/'==*pcPathOrFilePtr)
        {
            *pcPathOrFilePtr='\\';
        }
#elif defined CPSS_VOS_VXWORKS
        if('\\'==*pcPathOrFilePtr)
        {
            *pcPathOrFilePtr='/';
        }
        /*
#elif defined CPSS_VOS_LINUX
        if('/'==*pcPathOrFilePtr)
        {
            *pcPathOrFilePtr='\\';
        }*/
#endif

        pcPathOrFilePtr++;
    }

    if(bIfAbsDir == TRUE)
    {
    	 /*检查是否是绝对路径*/
        lFsAbsPathChk = cpss_fs_abs_path_check(acPath);
        if( CPSS_OK != lFsAbsPathChk )
        {
            return CPSS_ERROR ;
        }
    }
    strncpy(pcFileName,acPath, CPSS_FS_FILE_ABSPATH_LEN) ;


    /*剔出文件名或目录名最后的'/'或'\'符号*/
    ulDirLen = strlen(pcFileName) ;
    while( (pcFileName[ulDirLen-1] == '\\')||(pcFileName[ulDirLen-1] == '/') )
    {
        pcFileName[ulDirLen-1] = '\0' ;
        ulDirLen-- ;
        if(ulDirLen == 0)
        {
            return CPSS_ERROR ;
        }
    }

    /*判断文件名长度是否大于规定的长度*/
    while( (pcFileName[ulDirLen-1]!='/')&&(pcFileName[ulDirLen-1]!='\\') )
    {
        ulFileNameLen++ ;
        ulDirLen-- ;
        if(ulFileNameLen > CPSS_FS_FILENAME_LEN)
        {
            return CPSS_ERROR ;
        }
        if(ulDirLen == 0)
        {
            return CPSS_OK ;
        }
    }

    return CPSS_OK;
}
/*******************************************************************************
* 函数名称: cpss_file_exist
* 功    能: 判断本地文件或目录是否已经存在
* 相关文档:
* 函数类型:
*
* 参    数:
* 参数名称            类型            输入/输出         描述
* pcFileName          const CHAR *         输入              文件名指针
* 函数返回:
*            布尔值（TRUE，FALSE）
* 说   明:
*******************************************************************************/
BOOL cpss_file_exist
(
const CHAR *pcFileName
)
{
	struct stat stStat;;
    BOOL bRet=TRUE;

    if ( ERROR == stat(pcFileName, &stStat))
    {
        return FALSE;
    }
    return bRet;
}
/*******************************************************************************
* 函数名称: cpss_file_copy
* 功    能: 本地文件拷贝
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szSrcFile           const STRING    输入              原始文件名(含绝对路径名)指针
* szDstFile           const STRING    输入              目的文件名(含绝对路径名)指针
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明: 如果目的文件存在则覆盖目的文件
*******************************************************************************/
INT32 cpss_file_copy
(
const STRING szSrcFile,
const STRING szDstFile
)
{
    INT8 acSrcFileName[ CPSS_FS_FILE_ABSPATH_LEN] ;
    INT8 acDstFileName[ CPSS_FS_FILE_ABSPATH_LEN] ;
    INT32 lFNameChkSrc = 0;
    INT32 lFNameChkDst = 0;
#ifdef CPSS_VOS_VXWORKS
    INT32 lCopy;
#endif
#ifdef CPSS_VOS_WINDOWS
    BOOL bCopyFile = FALSE;
#endif
#ifdef CPSS_VOS_LINUX
    INT32 lCopy;
#endif

    if((szSrcFile == NULL)||(szDstFile == NULL))
    {
        return CPSS_ERR_FS_FILENAME_NULL ;
    }
    strncpy(acSrcFileName,szSrcFile, CPSS_FS_FILE_ABSPATH_LEN) ;
    strncpy(acDstFileName,szDstFile, CPSS_FS_FILE_ABSPATH_LEN) ;

    lFNameChkSrc = cpss_filename_check(acSrcFileName,TRUE);
    lFNameChkDst = cpss_filename_check(acDstFileName,TRUE);
    /*对文件名进行合法性检查*/
    if( ( CPSS_OK != lFNameChkSrc)|| ( CPSS_OK != lFNameChkDst) )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "(%s)or(%s) name is illegal.",acSrcFileName,acDstFileName) ;
        return CPSS_ERR_FS_FILENAME_ILLEGAL ;
    }

    /*
    调用操作系统的实现函数
    */
#ifdef CPSS_VOS_WINDOWS

    cpss_fs_filename_change(acSrcFileName) ;
    cpss_fs_filename_change(acDstFileName) ;
    bCopyFile = CopyFile(acSrcFileName,acDstFileName,FALSE);
    if ( FALSE == bCopyFile )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "copy (%s)to(%s) is fail.",acSrcFileName,acDstFileName) ;
        return cpss_fs_errno_get() ;
    }
    cpss_output(CPSS_MODULE_FS,CPSS_PRINT_INFO,
            "copy (%s)to(%s) is success.",acSrcFileName,acDstFileName) ;

#elif defined CPSS_VOS_VXWORKS

    lCopy = copy(acSrcFileName, acDstFileName);
    if ( ERROR == lCopy )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "copy (%s)to(%s) is fail.",acSrcFileName,acDstFileName) ;
        return cpss_fs_errno_get() ;
    }

#endif

#ifdef  CPSS_VOS_LINUX

    lCopy = cpss_copy_file(acSrcFileName, acDstFileName);

    if ( CPSS_ERROR == lCopy )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "copy (%s)to(%s) is fail.",acSrcFileName,acDstFileName) ;
        return CPSS_ERROR;
    }
#endif

    return 0;
}

/*******************************************************************************
* 函数名称: cpss_copy_file
* 功    能: 本地文件拷贝
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szSrcFile           const STRING    输入              原始文件名(含绝对路径名)指针
* szDstFile           const STRING    输入              目的文件名(含绝对路径名)指针
* 函数返回:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说   明: 如果目的文件存在则覆盖目的文件
*******************************************************************************/
INT32 cpss_copy_file
(
	const STRING szSrcFile,
	const STRING szDstFile
)
{
	INT32 lRet = CPSS_OK;
	INT32 lFdSrc;
	INT32 lFdDst;
	INT32 lCountRead;
	INT32 lCountWrite;
	char readBuf[1025];
	UINT32 fileSize;
    UINT32 ulOnceReadMax = 1024;
    UINT32 ulReadLen = 0 ;
    UINT32 ulWriteLen = 0;
    UINT32 pucReadBuf;
    UINT32 ulBufLen;

    if((szSrcFile == NULL)||(szDstFile == NULL))
    {
        lRet = CPSS_ERROR;
    }

    lRet = cpss_file_get_size(szSrcFile,&fileSize);

    ulBufLen = fileSize;

    if(CPSS_OK == lRet)
    {
    	lFdSrc = cpss_file_open(szSrcFile,CPSS_FILE_OBINARY|CPSS_FILE_ORDONLY);

        if(CPSS_FD_INVALID == lFdSrc)
        {
            lRet = CPSS_ERROR;
        }
    }
    if(CPSS_OK == lRet)
    {
    	lFdDst = cpss_file_open(szDstFile,CPSS_FILE_ORDWR|CPSS_FILE_OCREATE);

		if(CPSS_FD_INVALID == lFdDst)
		{
			lRet = CPSS_ERROR ;
		}
    }

    while(1)
    {
        if(ulOnceReadMax >= ulBufLen)
        {

        	lCountRead = read(lFdSrc,readBuf, ulBufLen) ;

            if ( -1 == lCountRead )
			{
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
					"read file handle (%d) failed!",lFdSrc) ;
				lRet = CPSS_ERROR;

			}

            ulReadLen = ulReadLen + (UINT32)lCountRead ;

            lCountWrite = write(lFdDst,readBuf,ulBufLen);

            if(-1 == lCountWrite)
            {
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
					"write file handle (%d) failed!",lFdDst) ;
				lRet = CPSS_ERROR;
            }
            ulWriteLen = ulWriteLen + (UINT32)lCountWrite;
            break ;
        }
        else
        {

        	lCountRead = read(lFdSrc,readBuf, ulOnceReadMax) ;
            if ( -1 == lCountRead )
            {
            	cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
					"read file handle (%d) failed!",lFdSrc) ;
            	lRet = CPSS_ERROR;
            }

            ulReadLen = ulReadLen + (UINT32)lCountRead ;

            if( 0 == lCountRead )
            {
                break ;
            }

            lCountWrite = write(lFdDst,readBuf,ulOnceReadMax);

			if(-1 == lCountWrite)
			{
				cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
					"write file handle (%d) failed!",lFdDst) ;
				lRet = CPSS_ERROR;
				break;
			}
			ulWriteLen = ulWriteLen + (UINT32)lCountWrite;
            ulBufLen = ulBufLen - ulOnceReadMax ;
            continue ;
        }
    }

    if(ulReadLen != ulWriteLen)
    {
		lRet = CPSS_ERROR;
    }
    if(CPSS_OK == lRet)
    {
        cpss_file_close(lFdSrc) ;
        cpss_file_close(lFdDst);
    }

	return lRet;

}

/*******************************************************************************
* 函数名称: cpss_file_stat
* 功    能: 得到本地文件或目录的详细信息
*
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述
* szFileName          const STRING    输入              待打开的文件名指针
* lFlag               INT32           输入              打开文件之后的读写类型(如FS_FILE_ORDONLY)
* 函数返回:
*       成功：文件描述符;
*       失败：CPSS_FD_INVALID；
* 说   明:
*******************************************************************************/
INT32 cpss_file_stat
(
const CHAR *pcFileName,
CPSS_FILE_STAT_T *pstStat
)
{
    CHAR acFileName[CPSS_FS_FILE_ABSPATH_LEN];
    INT32 lFileNameChk = 0;

    /*对传入参数进行检查*/
    if (pstStat == NULL)
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,"input parameter illegal") ;
        return CPSS_ERR_FS_PARA_NULL ;
    }

    strncpy(acFileName,pcFileName, CPSS_FS_FILE_ABSPATH_LEN) ;

    /*
    检查文件名是否合法
    */
    lFileNameChk = cpss_filename_check(acFileName,TRUE);
    if( CPSS_OK != lFileNameChk )
    {
        cpss_output(CPSS_MODULE_FS,CPSS_PRINT_WARN,
            "input file name (%s) is not correctly!",acFileName) ;
        return CPSS_ERR_FS_FILENAME_ILLEGAL ;
    }

#ifdef CPSS_VOS_WINDOWS                /* Win32-NT 操作系统部分 */
    {
        struct _stat stStat ;
        INT32 lStat;

        /*转化文件名的*/
        cpss_fs_filename_change(acFileName) ;

        /*
        调用操作系统的stat函数
        */
        lStat = _stat(acFileName,&stStat);
        if ( -1 == lStat )
        {
            return cpss_fs_errno_get() ;
        }
        pstStat->usMode   = stStat.st_mode;
        pstStat->ulSize   = stStat.st_size;

        cpss_gmt2clock(stStat.st_atime,&pstStat->stAccessTime) ;
        cpss_gmt2clock(stStat.st_mtime,&pstStat->stModifyTime) ;
        cpss_gmt2clock(stStat.st_ctime,&pstStat->stChangTime) ;
    }

#elif defined CPSS_VOS_VXWORKS     /* VxWorks操作系统部分 */
    {
        struct stat stStat;
        /*
        调用操作系统的stat函数
        */
        if ( ERROR == stat(acFileName, &stStat))
        {
            return cpss_fs_errno_get() ;
        }
        pstStat->usMode  = stStat.st_mode;
        pstStat->ulSize  = stStat.st_size;

        cpss_gmt2clock(stStat.st_atime,&pstStat->stAccessTime) ;
        cpss_gmt2clock(stStat.st_mtime,&pstStat->stModifyTime) ;
        cpss_gmt2clock(stStat.st_ctime,&pstStat->stChangTime) ;
    }

#elif defined CPSS_VOS_LINUX     /* linux操作系统部分 */
    {
        struct stat stStat;
        /*
        调用操作系统的stat函数
        */
        if ( ERROR == stat(acFileName, &stStat))
        {
            return errno ;
        }
        pstStat->usMode  = stStat.st_mode;
        pstStat->ulSize  = stStat.st_size;

        cpss_gmt2clock(stStat.st_atime,&pstStat->stAccessTime) ;
        cpss_gmt2clock(stStat.st_mtime,&pstStat->stModifyTime) ;
        cpss_gmt2clock(stStat.st_ctime,&pstStat->stChangTime) ;
    }

#endif                         /* #ifdef CPSS_VOS_LINUX 结束 */
    return CPSS_OK;
}

/*******************************文件结束 **********************************/
