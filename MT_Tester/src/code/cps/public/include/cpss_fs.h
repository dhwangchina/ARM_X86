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
#ifndef CPSS_FS_H
#define CPSS_FS_H

/********************************* 头文件包含 ********************************/
#include "cpss_type.h"
#include "cpss_tm.h"
#include <fcntl.h>

/******************************** 宏和常量定义 *******************************/

/* 对外消息ID定义 */
#define CPSS_FTP_REQ_MSG             ((UINT32)(0x21000301)) /* FTP文件传输请求消息 */
#define CPSS_FTP_RSP_MSG             ((UINT32)(0x21000302)) /* FTP文件传输响应消息 */  
#define CPSS_TFTP_REQ_MSG            ((UINT32)(0x21000303)) /* TFTP文件传输请求消息 */ 
#define CPSS_TFTP_RSP_MSG            ((UINT32)(0x21000304)) /* TFTP文件传输响应消息 */ 
#define CPSS_FTP_PROGRESS_IND_MSG	 ((UINT32)(0x21000305))  /*FTP进度指示消息*/
#define CPSS_TFTP_DOWNLOAD_STAT_MSG  ((UINT32)(0x21000320))   /* tftp download end */

/*文件读请求消息*/
#define CPSS_FS_FILE_READ_REQ_MSG  0x21000306
#define CPSS_FS_FILE_READ_RSP_MSG  0x21000307
#define CPSS_FS_FILE_WRITE_REQ_MSG 0x21000308
#define CPSS_FS_FILE_WRITE_RSP_MSG 0x21000309
#define CPSS_FS_FILE_COPY_REQ_MSG  0x2100030a
#define CPSS_FS_FILE_COPY_RSP_MSG  0x2100030b
#define CPSS_FS_FILE_MOVE_REQ_MSG  0x2100030c
#define CPSS_FS_FILE_MOVE_RSP_MSG  0x2100030d
#define CPSS_FS_DIR_COPY_REQ_MSG   0x2100030e
#define CPSS_FS_DIR_COPY_RSP_MSG   0x2100030f
#define CPSS_FS_DIR_DEL_REQ_MSG    0x21000310
#define CPSS_FS_DIR_DEL_RSP_MSG    0x21000311
#define CPSS_FS_FILE_DEL_REQ_MSG   0x21000312
#define CPSS_FS_FILE_DEL_RSP_MSG   0x21000313
#define SMSS_DIR_COPY_REQ_MSG	   0x21000314

/* 无效文件描述符 */
#define CPSS_FD_INVALID ((UINT32)(0xFFFFFFFF))  

/* 无效目录描述符 */
#define CPSS_DD_INVALID ((UINT32)(0xFFFFFFFF))

#ifdef CPSS_VOS_WINDOWS
  
     /*
     file open flag
     */
    #define CPSS_FILE_ORDONLY        (_O_RDONLY)    /* 只读 */
    #define CPSS_FILE_OWRONLY        (_O_WRONLY)    /* 只写 */
    #define CPSS_FILE_ORDWR          (_O_RDWR)      /* 读写 */

    #define CPSS_FILE_OCREATE        (_O_CREAT)     /* 创建 */
    #define CPSS_FILE_OAPPEND        (_O_APPEND)    /* 追加 */
    #define CPSS_FILE_OTRUNC         (_O_TRUNC)     /* 清除原有内容 */
    #define CPSS_FILE_OTEXT          (_O_TEXT)      /* 文本方式 */
    #define CPSS_FILE_OBINARY        (_O_BINARY)    /* 二进制方式 */
    #define CPSS_SEEK_SET            (SEEK_SET)
    #define CPSS_SEEK_CUR            (SEEK_CUR)
    #define CPSS_SEEK_END            (SEEK_END)

#elif defined CPSS_VOS_LINUX
    #define CPSS_FILE_ORDONLY        (O_RDONLY)    /* 只读 */
    #define CPSS_FILE_OWRONLY        (O_WRONLY)    /* 只写 */
    #define CPSS_FILE_ORDWR          (O_RDWR)      /* 读写 */
    #define CPSS_FILE_OCREATE        (O_CREAT)     /* 创建 */
    #define CPSS_FILE_OAPPEND        (O_APPEND)    /* 追加 */
    #define CPSS_FILE_OTRUNC         (O_TRUNC)     /* 清除原有内容 */
    #define CPSS_FILE_OTEXT          (0)      /* 文本方式 */
    #define CPSS_FILE_OBINARY        (O_RDWR)    /* 二进制方式 */
    #define CPSS_SEEK_SET            (SEEK_SET)
    #define CPSS_SEEK_CUR            (SEEK_CUR)
    #define CPSS_SEEK_END            (SEEK_END)
   
#elif defined CPSS_VOS_VXWORKS   
    #define CPSS_FILE_ORDONLY         (O_RDONLY)      /* 只读 */
    #define CPSS_FILE_OWRONLY         (O_WRONLY)      /* 只写 */
    #define CPSS_FILE_ORDWR           (O_RDWR)        /* 读写 */
    #define CPSS_FILE_OCREATE         (O_CREAT)       /* 创建 */
    #define CPSS_FILE_OAPPEND         (O_APPEND)      /* 追加 */
    #define CPSS_FILE_OTRUNC          (O_TRUNC)       /* 清除原有内容 */
    #define CPSS_FILE_OTEXT           (0)             /* 文本方式 */
    #define CPSS_FILE_OBINARY         (O_RDWR)        /* 二进制方式 */
    #define CPSS_SEEK_SET             (SEEK_SET)
    #define CPSS_SEEK_CUR             (SEEK_CUR)
    #define CPSS_SEEK_END             (SEEK_END)    
#endif

/* 注：以下长度定义均包含末尾的结束符。
这些宏在O接口和M接口中被用到，不得擅自改动。
如果修改，必需同步更新接口手册。*/
/* 绝对路径名称长度*/
#define CPSS_FS_FILE_ABSPATH_LEN   (256+5)
/*文件相对路径名称长度*/
#define CPSS_FS_FILEDIR_LEN        (64)
/* FTP请求的用户名长度 */
#define CPSS_FTP_USERNAME_LEN      (32)
/* FTP请求的口令长度 */
#define CPSS_FTP_PASSWORD_LEN      (32)
/* 文件名长度（不含路径） */
#define CPSS_FS_FILENAME_LEN       (32+5)

/* 文件传输命令，用于CPSS_FTP_MSG_T、 CPSS_TFTP_MSG_T */
#define CPSS_FILE_PUT   ((UINT32)(0x0)) /* 上传 */
#define CPSS_FILE_GET   ((UINT32)(0x1)) /* 下载 */

/* FTP传输命令，用于CPSS_FTP_MSG_T、 CPSS_TFTP_MSG_T。TFTP只支持0x1和0x11 */
#define CPSS_FTP_CMD_PUT_FILE          ((UINT32)(0x01)) /* 上传 */
#define CPSS_FTP_CMD_PUT_DIR_PACK      ((UINT32)(0x02)) /* 上传并压缩*/
#define CPSS_FTP_CMD_GET_FILE          ((UINT32)(0x11)) /* 下载 */
#define CPSS_FTP_CMD_GET_DIR_PACK      ((UINT32)(0x12)) /* 下载压缩包并解压到目录 */
#define CPSS_FTP_CMD_GET_FILE_EXP      ((UINT32)(0x13))  /* 先下载,再写盘,版本文件专用 */

/* FTP处理状态 */
#define CPSS_FTP_STATE_PACKING         ((UINT32)(0x01)) /* 正在压缩 */
#define CPSS_FTP_STATE_TRANSFERRING    ((UINT32)(0x02)) /* 正在FTP传送 */

/******************************** 类型定义 ***********************************/

/*文件属性类型定义*/
typedef struct tagCPSS_FILE_STAT_T
{    
    UINT16        usMode;          /* 保留，暂不使用file mode,Bit mask for file-mode information. */       
    UINT16        usReserved;
    UINT32        ulSize;          /* size of file, in bytes */
    CPSS_TIME_T   stAccessTime;    /* time of last access */
    CPSS_TIME_T   stModifyTime;    /* time of last modification */
    CPSS_TIME_T   stChangTime;     /* time of last change of file status */
}CPSS_FILE_STAT_T;

/*FTP客户端传输请求消息*/
typedef struct tagCPSS_FTP_REQ_MSG
{
    UINT32  ulSerialID;        /* 用来识别同一个纤程的多个ftp传输 */
    UINT8   aucUserName[CPSS_FTP_USERNAME_LEN];   /* 用户名，最大长度20个字符，包括'\0'结束符*/
    UINT8   aucPasswd[CPSS_FTP_PASSWORD_LEN];     /* 密码，最大长度20个字符，包括'\0'结束符 */
    UINT32  ulServerIP;        /* ftp服务器端ip地址" */
    UINT8   aucServerFile[ CPSS_FS_FILE_ABSPATH_LEN];/* 服务器端文件名(相对路径) */
    UINT8   aucClientFile[ CPSS_FS_FILE_ABSPATH_LEN];/* 客户端文件名(绝对路径) */

    /* 传输命令：上传 CPSS_FILE_PUT，下载 CPSS_FILE_GET*/
    UINT32   ulFtpCmd;  
    UINT32   ulProgessStep;   /* 进度指示步进值(字节), 0表示不需要进度指示 */
    UINT32   ulLen;    /* 缓存空间字节尺寸，如果为0，则写入文件；否则写入缓存 */
    UINT8   *pucMem;  /* 则文件下载直接写入该缓存空间 */
} CPSS_FTP_REQ_MSG_T;

/*FTP客户端传输响应消息*/
typedef struct tagCPSS_FTP_RSP_MSG
{
    UINT32  ulSerialID; /* 来自请求消息中的ulSerialID值 */
    INT32    lErrorNo;  /* 文件传输结果：成功CPSS_OK, 失败CPSS_ERROR */
} CPSS_FTP_RSP_MSG_T;

/*TFTP客户端传输请求消息*/
typedef struct tagCPSS_TFTP_REQ_MSG
{
    UINT32 ulSerialID; /* 用来识别同一个纤程的多个tftp传输 */
    UINT32 ulServerIP; /* tftp服务器端ip地址 */
    UINT8 aucServerFile[ CPSS_FS_FILE_ABSPATH_LEN];/* 服务器端文件名(含相对路径) */
    UINT8 aucClientFile[ CPSS_FS_FILE_ABSPATH_LEN];/* 客户端文件名(含绝对路径) */

    /* 传输命令：上传 CPSS_FILE_PUT，下载 CPSS_FILE_GET*/
    UINT32    ulCmd;   
    UINT32    ulLen;    /* 缓存空间字节尺寸，如果为0，则写入文件；否则写入缓存 */
    UINT8    *pucMem;  /* 则文件下载直接写入该缓存空间 */
}CPSS_TFTP_REQ_MSG_T;

typedef CPSS_FTP_RSP_MSG_T CPSS_TFTP_RSP_MSG_T;

typedef CPSS_FTP_RSP_MSG_T CPSS_TFTP_DOWNLOAD_STAT_MSG_T;

typedef struct tagCPSS_DIR_CONFIG_T
{
    CHAR acDirName[ CPSS_FS_FILE_ABSPATH_LEN] ;
}CPSS_DIR_CONFIG_T;

/* FTP传输进度指示消息 */
typedef struct tagCPSS_FTP_PROGRESS_IND_MSG
{
    UINT32  ulSerialID;   /* 用来识别同一个纤程的多个ftp传输 */
    UINT32  ulState;      /* 处理状态: 压缩中/FTP中 */
    UINT32  ulUsedTime;   /* 已处理时间(毫秒) */
    UINT32  ulTotalSize;            /* 总文件长度(字节) */
    UINT32  ulDealingSize;           /* 已压缩的文件长度(字节)或已传送的文件长度(字节) */
} CPSS_FTP_PROGRESS_IND_MSG_T;

/*文件读操作请求消息*/
typedef struct tagCPSS_FS_FILE_READ_REQ
{
    UINT8     aucFileName[CPSS_FS_FILE_ABSPATH_LEN];  /* 文件名(含绝对路径) */ 
    VOID      *pvBuf;
    UINT32    ulMaxLen;  /*数据缓存的字节长度*/
    UINT32    ulOrigin;  /*读数据的起始位置*/
    INT32     lOffset;   /*相对于起始位置的偏移量*/
    UINT32    ulContext ;
}CPSS_FS_FILE_READ_REQ_T;

/*文件读操作响应消息*/
typedef struct tagCPSS_FS_FILE_READ_RSP
{
    UINT8     aucFileName[CPSS_FS_FILE_ABSPATH_LEN];  /* 文件名(含绝对路径) */ 
    VOID      *pvBuf;
    UINT32    ulMaxLen; /*数据缓存的字节长度*/
    UINT32    ulReadLen; /* 实际读出的数据字节长度*/
    UINT32    ulResult;
    UINT32    ulContext ;    
}CPSS_FS_FILE_READ_RSP_T;

/*文件写操作请求消息*/
typedef struct tagCPSS_FS_FILE_WRITE_REQ
{
    UINT8     aucFileName[CPSS_FS_FILE_ABSPATH_LEN];  /* 文件名(含绝对路径) */ 
    VOID      *pvBuf;
    UINT32    ulBufLen;  /* 待写入文件的缓冲区字节长度*/
    UINT32    ulOrigin;  /*写数据的起始位置*/
    INT32     lOffset;   /*相对于起始位置的偏移量*/
    INT32     lOpenFlag; /*文件打开方式*/
    UINT32    ulContext ;    
}CPSS_FS_FILE_WRITE_REQ_T;

/*文件写操作响应消息*/
typedef struct tagCPSS_FS_FILE_WRITE_RSP
{
    UINT8     aucFileName[CPSS_FS_FILE_ABSPATH_LEN];  /* 文件名(含绝对路径) */ 
    VOID      *pvBuf;
    UINT32    ulWriteLen; /* 实际写入的数据字节长度*/
    UINT32    ulResult;
    UINT32    ulContext ;    
}CPSS_FS_FILE_WRITE_RSP_T;

typedef struct tagCPSS_FS_FILE_COPY_REQ
{
    UINT8   aucSrcFileName[CPSS_FS_FILE_ABSPATH_LEN];  /* 源文件名(含绝对路径) */ 
    UINT8   aucDstFileName[CPSS_FS_FILE_ABSPATH_LEN];  /* 目的文件名(含绝对路径) */
    UINT32  ulContext ;    
}CPSS_FS_FILE_COPY_REQ_T;

typedef struct tagCPSS_FS_FILE_COPY_RSP
{
    UINT8   aucSrcFileName[CPSS_FS_FILE_ABSPATH_LEN];  /* 源文件名(含绝对路径) */ 
    UINT8   aucDstFileName[CPSS_FS_FILE_ABSPATH_LEN];  /* 目的文件名(含绝对路径) */ 
    UINT32  ulResult;
    UINT32  ulContext ;    
}CPSS_FS_FILE_COPY_RSP_T;

/*目录删除请求*/
typedef struct tagCPSS_FS_DIR_DEL_REQ
{
    UINT8  aucDirPathName[CPSS_FS_FILE_ABSPATH_LEN] ; /*删除目录的名称*/
    UINT32 ulContext ;
}CPSS_FS_DIR_DEL_REQ_T;

/*目录删除响应*/
typedef struct tagCPSS_FS_DIR_DEL_RSP
{
    UINT8  aucDirPathName[CPSS_FS_FILE_ABSPATH_LEN] ; /*删除目录的名称*/
    UINT32 ulResult ;  /*删除目录结果*/
    UINT32 ulContext ;
}CPSS_FS_DIR_DEL_RSP_T;

/*文件删除请求*/
typedef struct tagCPSS_FS_FILE_DEL_REQ
{
    UINT8  aucFilePathName[CPSS_FS_FILE_ABSPATH_LEN] ; /*删除文件的名称*/
    UINT32 ulContext ;
}CPSS_FS_FILE_DEL_REQ_T ;

/*文件删除响应*/
typedef struct tagCPSS_FS_FILE_DEL_RSP
{
    UINT8  aucFilePathName[CPSS_FS_FILE_ABSPATH_LEN] ; /*删除文件的名称*/
    UINT32 ulResult ;  /*删除文件结果*/
    UINT32 ulContext ;    
}CPSS_FS_FILE_DEL_RSP_T ;


typedef struct tagCPSS_FS_FILE_COPY_REQ CPSS_FS_FILE_MOVE_REQ_T;
typedef struct tagCPSS_FS_FILE_COPY_RSP CPSS_FS_FILE_MOVE_RSP_T;
typedef struct tagCPSS_FS_FILE_COPY_REQ CPSS_FS_DIR_COPY_REQ_T;
typedef struct tagCPSS_FS_FILE_COPY_RSP CPSS_FS_DIR_COPY_RSP_T;

/******************************** 全局变量声明 *******************************/
/*extern */

/******************************** 外部函数原形声明 ***************************/

extern INT32 cpss_fs_init(void) ;

/* 文件操作 *******************************************************************/

extern UINT32 cpss_file_open
(
    const STRING szFileName,
    INT32 lFlag
);

extern INT32 cpss_file_close
(
    UINT32 ulFd
);

extern INT32 cpss_file_read
(
    UINT32 ulFd, 
    VOID *pvBuf, 
    UINT32 lMaxLen, 
    UINT32 *pulReadLen
);

extern INT32 cpss_file_write
(
    UINT32 ulFd, 
    const VOID *pvBuf,
    UINT32 lMaxLen,
    UINT32 *pulWriteLen
);

extern INT32 cpss_file_seek
(
    UINT32  ulFd, 
    UINT32  lOffset, 
    INT32   lOrigin, 
    UINT32 *pulSeekPos
);

extern INT32 cpss_file_create
(
    const STRING szFileName
);

extern INT32 cpss_file_delete
(
    const STRING szFileName
);

extern INT32 cpss_file_copy
(
    const STRING szSrcFile,
    const STRING szDstFile
);

extern INT32 cpss_file_get_size
(
    const STRING szFileName, 
    UINT32 *pulSize
);

extern INT32 cpss_file_is_dir
(
    const STRING szFileName,
    BOOL *pbIsDir
);

extern INT32 cpss_file_pack
(
    const STRING szDirName,
    const STRING szDstFileName,
    BOOL bCompress
);

extern INT32 cpss_file_unpack
(
    const STRING szFileName,
    const STRING szDstDirName
);

extern INT32 cpss_file_stat
(
    const CHAR *pcFileName, 
    CPSS_FILE_STAT_T *pstStat
);

extern INT32 cpss_file_move
(
    const STRING szSrcFile, 
    const STRING szDestFile
);

extern BOOL cpss_file_exist
(
    const CHAR *pcFileName
);

/* 目录操作 *******************************************************************/

extern INT32 cpss_dir_create
(
    const STRING szDirName
);

extern INT32 cpss_dir_delete
(
    const STRING szDirName
);

extern UINT32 cpss_dir_open
(
    const STRING szDirName
);

extern INT32 cpss_dir_close
(
    UINT32 ulDd
);

extern STRING cpss_dir_read
(
    UINT32 ulDd
);

extern INT32 cpss_dir_copy
(
    const STRING szSrcDir,
    const STRING szDstDir
);

extern INT32 cpss_dir_used_space_get
(
    const STRING szDirName, 
    UINT32 *pulSizeHigh,
    UINT32 *pulSizeLow
);

extern INT32 cpss_disk_free_space_get
(
    const STRING szDiskName,
    UINT32 *pulSizeHigh,
    UINT32 *pulSizeLow
);

extern INT32 cpss_file_rename
(
STRING szOldName,
STRING szNewName
);

extern VOID cpss_ftp_proc
(
    UINT16  usUserState,
    VOID *  pvVar,
    CPSS_COM_MSG_HEAD_T *pstMsgHead
);

extern UINT32 cpss_fs_errno_get();

extern INT32 cpss_fs_ftp(CPSS_FTP_REQ_MSG_T* pstFTPReqMsg,UINT32 ulDstPid);  

#define CPS_FILE_TRANS_REQ_MSG 0x21000351
#define CPS_FILE_TRANS_RSP_MSG 0x21000352
#define CPS_FILE_TRANS_FTP 1
#define CPS_FILE_TRANS_TFTP 2
#define CPS_FILE_TRANS_UPLOAD 2
#define CPS_FILE_TRANS_DOWNLOAD 1

#define CPS_FS_FT_NO_ERROR 0
#define CPS_FS_FT_ERROR_HOST 1
#define CPS_FS_FT_ERROR_LOCAL_FILE 2
#define CPS_FS_FT_ERROR_REMOTE_FILE 3
#define CPS_FS_FT_ERROR_USER 4
#define CPS_FS_FT_ERROR_PERMISSION_DENIED 5
#define CPS_FS_FT_ERROR_PARAM 9
#define CPS_FS_FT_ERROR_UNKNOWN 10

typedef struct _CPS_FILE_TRANS_REQ_MSG_
{
   UINT32 ulCmd;         /* 传送类型包括：1:下载（从远程服务器到设备）2:上传（从设备到远程服务器）*/
   UINT32 ulProtocol;    /* 使用协议，1-FTP，2-TFTP */
   UINT8 aucServerFile[256];  /* 在服务器上的文件名称，包括路径 */
   UINT8 aucLocalFile[256];   /* 在本地的文件名称，包括路径 */
   UINT8 aucServerAddr[4];   /* 远程服务器地址 */
   UINT32 ulServerPort;      /* 远程服务器端口 */
   UINT8 aucUsername[32];    /* 远程服务器用户名 */
   UINT8 aucPasswd[32];      /* 远程服务器密码 */
}CPS_FILE_TRANS_REQ_MSG_T;

typedef struct _CPS_FILE_TRANS_RSP_MSG_
{
   INT32 lResult;          /* 传输结果：0成功，其它失败 */
   INT32 lErrCode;         /* 失败时存储错误码 */
}CPS_FILE_TRANS_RSP_MSG_T;

extern INT32 cps_file_trans_req_deal(CPS_FILE_TRANS_REQ_MSG_T* pstTransReq);

/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_FS_H */
/******************************** 头文件结束 *********************************/

