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
#ifndef CPSS_FS_FILE_H
#define CPSS_FS_FILE_H
/********************************* 头文件包含 ********************************/
#include "swp_config.h"
#include "cpss_type.h"
#include "cpss_tm.h"

#include <sys/stat.h>
/******************************** 宏和常量定义 *******************************/
#define CPSS_MAX_OPENED_FILES         35       /*同时打开的最大文件个数*/
#define CPSS_MAX_OPENED_DIR           35       /*同时打开的最大目录个数*/

#define CPSS_FLASH_FILESYS

/*判断文件属性的宏定义*/
#ifdef CPSS_VOS_VXWORKS 
#define    CPSS_FILE_ISDIR(mode)    ((mode & S_IFMT) == S_IFDIR)     /* directory */
#define    CPSS_FILE_ISCHR(mode)    ((mode & S_IFMT) == S_IFCHR)     /* character special */
#define    CPSS_FILE_ISREG(mode)    ((mode & S_IFMT) == S_IFREG)     /* regular file ,not device file*/
#define    CPSS_FILE_ISFIFO(mode)   ((mode & S_IFMT) == S_IFIFO)     /* fifo special */
#elif defined CPSS_VOS_WINDOWS
#define    CPSS_FILE_ISDIR(mode)    ((mode & _S_IFMT) == _S_IFDIR)    /* directory */
#define    CPSS_FILE_ISCHR(mode)    ((mode & _S_IFMT) == _S_IFCHR)    /* character special */
#define    CPSS_FILE_ISREG(mode)    ((mode & _S_IFMT) == _S_IFREG)    /* regular file ,not device file*/
#define    CPSS_FILE_ISFIFO(mode)   ((mode & _S_IFMT) == _S_IFIFO)    /* fifo special */
#elif defined CPSS_VOS_LINUX
#define    CPSS_FILE_ISDIR(mode)    ((mode & S_IFMT) == S_IFDIR)    /* directory */
#define    CPSS_FILE_ISCHR(mode)    ((mode & S_IFMT) == S_IFCHR)    /* character special */
#define    CPSS_FILE_ISREG(mode)    ((mode & S_IFMT) == S_IFREG)    /* regular file ,not device file*/
#define    CPSS_FILE_ISFIFO(mode)   ((mode & S_IFMT) == S_IFIFO)    /* fifo special */
#endif

/*ramdisk 的属性宏定义*/
#ifdef CPSS_VOS_VXWORKS_RAM_DISK
#define CPSS_RAM_DISK_BYTES_PER_BLK   512
#define CPSS_RAM_DISK_BLKS_PER_TRACK  8192
#endif
#define CPSS_FLASH_FILESYS_DESC  "F:"
#define CPSS_DISK_FILESYS_DESC   "H:"
#define CPSS_FILE_OPEN_OPERATE    1
#define CPSS_FILE_CLOSE_OPERATE   2

/*打包解包相关宏定义*/
#define CPSS_FS_PACK_SUFFIX              ".zip"
#define CPSS_FS_FILE_HDR_FLAG            0x04034b50
#define CPSS_FS_CENTRAL_DIR_FLAG         0x02014b50
#define CPSS_FS_CENTRAL_DIR_END_FLAG     0x06054b50
#define CPSS_FS_FILE_TAIL_FLAG           0x08074b50
#define CPSS_FS_GIZP_HDR_FLAG            0x8b1f
#define CPSS_FS_ZIP_DIR_FLAG             0xa
#define CPSS_FS_ZIP_FILE_FLAG            0x14
#define CPSS_FS_ZIP_VER                  0x14
#define CPSS_FS_ZIP_COMPRESS             0x8
#define CPSS_FS_ZIP_NOT_COMPRESS         0x0
#define CPSS_FS_TAIL_INFO_MODE           0x8
#define CPSS_FS_PACK_READ_BUF_LEN        4096
#define CPSS_FS_ZIP_HRD_LEN              30
#define CPSS_FS_ZIP_CENTRAL_DIR_LEN      46
#define CPSS_FS_ZIP_CENTRAL_DIR_END_LEN  22
#define CPSS_FS_ZIP_FILE_TAIL_LEN        16
#define CPSS_FS_EXTERNAL_FILE_ATTR_DIR   0x30
#define CPSS_FS_EXTERNAL_FILE_ATTR_FILE  0x20
#define CPSS_FS_GZIP_SUFFIX              ".gz"

#define CPSS_FS_GZIP_HRD_LEN      10
#define CPSS_FS_GZIP_RESERVE_LEN  1
#define CPSS_FS_GZIP_TAIL_LEN     8
#define CPSS_FS_GZIP_OS_WINDOWS   0xb
#define CPSS_FS_GZIP_OS_UNIX      3
#define CPSS_FS_UNZIP_END         2 

/*通知FTP纤程FD信息*/
#define CPSS_FS_FD_IND_MSG   0x1000
#define CPSS_FD_OPEN_FLAG    0 
#define CPSS_FD_CLOSE_FLAG   1

/*GZIP文件头是否增加文件名字符串标志*/
#define CPSS_FS_GZIP_ADD_FILENAME     (UINT8)0x8
#define CPSS_FS_GZIP_NOT_ADD_FILENAME (UINT8)0x0

#define CPSS_FS_GZIP_FORMAT_LEN   (CPSS_FS_GZIP_HRD_LEN + \
                               CPSS_FS_GZIP_RESERVE_LEN + CPSS_FS_GZIP_TAIL_LEN)

#define CPSS_FS_GZIP_DATA_LEN_GET(fileLen,nameLen) (fileLen \
                                  - nameLen - CPSS_FS_GZIP_FORMAT_LEN)

#define CPSS_FS_GZIP_HDR_LEN_GET(fileNameLen) (fileNameLen + CPSS_FS_GZIP_HRD_LEN +\
                                 CPSS_FS_GZIP_RESERVE_LEN)

/******************************** 类型定义 ***********************************/

#ifdef CPSS_VOS_WINDOWS
/*文件目录定义*/
typedef struct    tagCPSS_FILE_DIR_T    
{
    INT32        lHandle;        
    CHAR        acFileName [ CPSS_FS_FILE_ABSPATH_LEN]; 
} CPSS_FILE_DIR_T ;    

#endif

/*函数指针定义*/
typedef INT32  (*FS_FUNC)(const STRING szSrc, const STRING szDest);

#ifdef CPSS_VOS_VXWORKS
#define UINT64 unsigned long long
#endif

/*文件句柄链表节点定义*/
typedef struct tagCPSS_FS_HANDLE_NODE
{
    UINT32 ulFsHandle ;
    UINT8 aucFilePath[CPSS_FS_FILE_ABSPATH_LEN] ;    
    VOID*  pvNext ;
}CPSS_FS_HANDLE_NODE_T ;

/*文件句柄链表结构定义*/
typedef struct tagCPSS_FS_HANDLE_LINK
{
    CPSS_FS_HANDLE_NODE_T* pstNodeHdr ;
    UINT32 ulHandleNum ;
}CPSS_FS_HANDLE_LINK_T ;


/*文件打包解包结构定义*/

/*文件头结构*/
typedef struct tagCPSS_FS_FILE_HEAD
{
    UINT32 ulFileFlag ;       /*文件(目录)特征字0x04034b50*/
    UINT16 usVerExtract ;     /*解压版本号(文件:0x14 目录:0xa)*/
    UINT16 usBitFlag ;        /*位标志0*/
    UINT16 usCompressMethod ; /*压缩标志(0:不压缩，1:压缩)*/
    UINT16 usLastModTime ;    /*最后一次修改时间*/
    UINT16 usLastModDate ;    /*最后一次修改日期*/
    UINT32 ulCrc32 ;          /*crc校验(对压缩数据)*/
    UINT32 ulCompressSize ;   /*压缩后的文件长度*/
    UINT32 ulUnCompressSize ; /*未压缩的文件长度*/
    UINT16 usFileNameLen ;    /*文件名长度(包括目录)*/
    UINT16 usExtraLen ;       /*无用字段*/
}CPSS_FS_FILE_HEAD_T ;

/*zip格式中的central dir 结构*/
typedef struct tagCPSS_FS_CENTRAL_DIR_DESC
{
    UINT32 ulCentralDirFlag ;   /*文件(目录)描述项标志0x02014b50*/
    UINT16 usVerMade ;          /*版本信息(固定填14)*/
    UINT16 usVerExtract ;       /*解压版本号(文件:0x14 目录:0xa)*/
    UINT16 usBitFlag ;          /*位标志*/
    UINT16 usCompressMethod ;   /*压缩标志(0:不压缩，1:压缩)*/
    UINT16 usLastModTime ;      /*最后一次修改时间*/
    UINT16 usLastModData ;      /*最后一次修改日期*/
    UINT32 ulCrc32 ;            /*crc校验(对压缩数据)*/
    UINT32 ulCompressSize ;     /*压缩后的文件长度*/
    UINT32 ulUnCompressSize ;   /*未压缩的文件长度*/
    UINT16 usFileNameLen ;      /*文件名长度(包括目录)*/
    UINT16 usExtraLen ;         /*无用字段(固定填0)*/
    UINT16 usFileCommentLen ;   /*文件描述长度(固定填0)*/
    UINT16 usDiskNumStart ;     /*开始的分区编号(固定填0)*/
    UINT16 usInternalFileAttr ; /*文件内部属性(固定填0)*/
    UINT32 ulExternalFileAttr ; /*文件外部属性(文件30,目录20)*/
    UINT32 ulOffsetLocalHrd ;   /*相对于本地头的长度(从文件起始位置到对应的文件头)*/
    UINT8  pucFileName ;        /*文件(包括目录)名称字符串指针*/
}CPSS_FS_CENTRAL_DIR_DESC_T ;

/*zip格式中的central dir end 结构*/
typedef struct tagCPSS_FS_CENTRAL_DIR_END
{
    UINT32 ulCentralDirEndFlag ;   /*central dir end 标志*/
    UINT16 usDiskNum ;             /*分区数目(固定填0)*/
    UINT16 usCentralDirDiskNum ;   /*相对于central dir的分区数目(固定填0)*/
    UINT16 usTotalEntryOnDisk ;    /*本分区的所有文件数目(包括目录)*/
    UINT16 usTotalEntry ;          /*所有的文件数目*/
    UINT32 ulCetralEntrySize ;     /*central entry的长度*/
    UINT32 ulOffsetOfCentralEntry ;/*从zip文件头到central entry的长度*/
    UINT16 usFileCommentLen ;      /*文件描述长度*/
}CPSS_FS_CENTRAL_DIR_END_T ;

typedef struct tagCPSS_FS_GZIP_HDR
{
    UINT16 usGzipFlag ;     /*0x8b1F*/
    UINT8  ucDeflateFlag ;  /*0x8*/
    UINT8  ucFileFlag ;     /*0x8*/
    UINT32 ulTimeStamp ;    /*不填*/
    UINT8  ucExtralFlag ;   /*0*/
    UINT8  ucOsFlag ;       /*OS标志:0xB(windows)0x3(unix)*/
    UINT8  ucReserve ;      /*0*/
}CPSS_FS_GZIP_HDR_T ;

typedef struct tagCPSS_FS_GZIP_TAIL
{
    UINT32 ulCrcVal ;   /*crc值*/
    UINT32 ulFileLen ;  /*原始文件长度*/
}CPSS_FS_GZIP_TAIL_T ;

typedef struct tagCPSS_FS_FILE_TAIL_T
{
    UINT32 ulFileTailFlag ;
    UINT32 ulCrcValue ;
    UINT32 ulCompressSize ;
    UINT32 ulUnCompressSize ;
}CPSS_FS_FILE_TAIL_T ;

/*MS DOS的日期定义*/
typedef struct tagCPSS_MS_DOS_DATE
{
#ifdef CPSS_VOS_VXWORKS
#if _BYTE_ORDER == _LITTLE_ENDIAN        
    BITS16 btDay:5 ;
    BITS16 btMonth:4 ;
    BITS16 btYear:7 ;    
#endif
#if _BYTE_ORDER == _BIG_ENDIAN
    BITS16 btYear:7 ;
    BITS16 btMonth:4 ;
    BITS16 btDay:5 ;
#endif
#elif defined CPSS_VOS_WINDOWS
    BITS16 btDay:5 ;
    BITS16 btMonth:4 ;
    BITS16 btYear:7 ;    
#elif defined CPSS_VOS_LINUX
#if SWP_CPU_TYPE == SWP_CPU_LITTLE
    BITS16 btDay:5 ;
    BITS16 btMonth:4 ;
    BITS16 btYear:7 ;
#endif
#if SWP_CPU_TYPE == SWP_CPU_BIG
    BITS16 btYear:7 ;
    BITS16 btMonth:4 ;
    BITS16 btDay:5 ;
#endif
#endif
}CPSS_MS_DOS_DATE_T ;

/*MS DOS的时间定义*/
typedef struct tagCPSS_MS_DOS_TIME
{
#ifdef CPSS_VOS_VXWORKS
#if _BYTE_ORDER == _LITTLE_ENDIAN
    BITS16 btSencond:5 ;
    BITS16 btMinute:6 ;
    BITS16 btHour:5 ;
#endif
#if _BYTE_ORDER == _BIG_ENDIAN
    BITS16 btHour:5 ;
    BITS16 btMinute:6 ;
    BITS16 btSencond:5 ;
#endif
#elif defined CPSS_VOS_WINDOWS
    BITS16 btSencond:5 ;
    BITS16 btMinute:6 ;
    BITS16 btHour:5 ;
#elif defined CPSS_VOS_LINUX
#if SWP_CPU_TYPE == SWP_CPU_LITTLE
    BITS16 btSencond:5 ;
    BITS16 btMinute:6 ;
    BITS16 btHour:5 ;
#endif
#if SWP_CPU_TYPE == BIG
    BITS16 btHour:5 ;
    BITS16 btMinute:6 ;
    BITS16 btSencond:5 ;
#endif
#endif
}CPSS_MS_DOS_TIME_T ;

/*FD通知消息结构*/
typedef struct tagCPSS_FS_FD_IND_MSG 
{
    UINT32 ulOperateFlag ;
    UINT32 ulFd ;
    UINT8 aucFilePath[CPSS_FS_FILE_ABSPATH_LEN] ;    
}CPSS_FS_FD_IND_MSG_T;

/******************************** 全局变量声明 *******************************/



/******************************** 外部函数原形声明 ***************************/

extern INT32 gziplib_compressfile(INT32 OperType,char * inputname) ;

extern INT32 cpss_fs_suffix_add(UINT8* pucDirName,UINT8* pucSuffix) ;

extern INT32 cpss_ftp_progress_ind_send(UINT32 ulBeginTick,UINT32 ulCurrFileLen,UINT32* pulFileLenStep) ;

extern INT32 cpss_fs_xdelete(STRING szDirPath) ;

/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_FS_FILE_H */
/******************************** 头文件结束 *********************************/



