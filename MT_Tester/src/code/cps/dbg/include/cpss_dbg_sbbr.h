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
#ifndef CPSS_DBG_SBBR_H
#define CPSS_DBG_SBBR_H

/********************************* 头文件包含 ********************************/
#include "cpss_type.h"
/******************************** 宏和常量定义 *******************************/
#define CPSS_SBBR_POWER_ON_FIRST           0x12345678  /* 第一次上电后设置的标志 */
#define CPSS_SBBR_RUN_STATUS_NORMAL      0x15263748  /* 正常复位时设置的运行状态 */
#define CPSS_SBBR_RUN_STATUS_ABNORMAL  0x12345678  /* 运行状态的缺省值：异常状态 */
#define CPSS_SBBR_INVALID_USER_ID 0xff
#define SBBR_USER_MAX       32
#define CPSS_SBBR_TRANS_SIZE   800
/******************************** 类型定义 ***********************************/
/* 配置结构*/
typedef struct{
    char acName[32];       /* 用户名，最好是中断服务程序的函数名字 */
    UINT32 ulUserSN;       /* 用户编号，在同一类型用户中的编号，编号从0开始，必需连续 */
   UINT32 ulSize;         /* 用户区大小 */
} CPSS_SBB_INIT_CONFIG_T;

#if 0
typedef VOID (*CPSS_SBBR_HOOK_PF) (UINT32 ulType, UINT32 ulExecInfo);  /*黑匣子回调函数*/



typedef struct
{
    char acName[32];              /* user name */
    CPSS_SBBR_HOOK_PF Hook;             /* user exception hook */
    UINT8 *pucAddr;        /* user section address */
    UINT32 ulSize;                   /* user section size */
    UINT32 ulMin;                    /* offset of the start record */
    UINT32 ulNow;                    /* offset of the write position */
} LDT_SBBR_USER_T;
typedef struct
{
    UINT32  ulPowerOnFlag;   /* 上电标志位：0x12345678 */   
    UINT32  ulRunStatusFlag; /* 运行状态标志位：CPSS_SBBR_RUN_STATUS_NORMAL；CPSS_SBBR_RUN_STATUS_ABNORMAL */
    UINT8  *pucAddr;        /* address of `sbbr_sys' */
    UINT8   ucEndian;      /* 0 - big endian, 1 - little endian */
    UINT8   ucVersion;     /* SBBR version, now 2.0 (0x20) */
    UINT16  usSeqNumNow; /* sequence number */
    UINT8   ucTransfer;  /* configure automatic startup SBBR Data Transfer */
    UINT8   ucRecording; /* configure SBBR recording */
    UINT16  usUserNum;               /* total user number  */
    CPSS_SBBR_USER_T astUsers[SBBR_USER_MAX + 1]; /* 1 ~ SBBR_USER_MAX: User Data Section
                                           0 is Exception Data Section */
} LDT_SBBR_SYS_T;
typedef struct
{
    UINT16  usLength;              /* 记录长度 */
    UINT16  usSeqNum;             /* 记录序列号 */
    UINT32   ulTimestamp;           /* 时戳 */
    UINT32   ulArgs[4];             /* 用户参数 */
    UINT8   ucUser;                /* 用户 */
    UINT8   ucType;                /* 信息类型：文本/数据 */
    UINT8   aucInfo[1];             /* 用户信息 */
} LDT_SBBR_RECORD_T;

#endif

/* 管理区中用户管理部分的结构 */
typedef struct{
UINT8  acName[32];     /* user name */
CPSS_SBBR_HOOK_PF Hook;        /* user exception hook */
UINT8 *pucAddrStart;   /* user section address */
INT32 lSize;         /* user section size */
INT32 lMin;          /* offset of the start record */
INT32 lCurWriteLoc;  /* offset of the write position */
} CPSS_SBBR_USER_T;

/* 管理信息区结构 */
typedef struct{
UINT32  ulPowerOnFlag;   /* 上电标志位：0x12345678 */
UINT32  ulRunStatusFlag; /* 运行状态标志位：CPSS_SBBR_RUN_STATUS_NORMAL；CPSS_SBBR_RUN_STATUS_ABNORMAL */
UINT32  ulVersion;      /* 版本号，目前暂使用低8位，高24位清0，目前为 1.0 (0x10) */
UINT8  *pucSbbrStartAddr;  /* 黑匣子内存区起始地址 */
UINT32  ulSBBRTotalLen;     /* 黑匣子内存区长度 */
UINT8  *pucMngStartAddr;   /* 管理信息区起始地址 */
UINT32  ulMngSysLen;        /* 管理信息区长度 */
UINT8  *pucIsrStartAddr;   /* 中断信息区起始地址 */
UINT32  ulMngIsrLen;        /* 中断信息区长度 */
UINT8  *pucExecStartAddr;  /* 异常信息区起始地址 */
UINT32  ulExecLen;          /* 异常信息区长度 */
UINT8  *pucUserStartAddr;  /* 用户信息区起始地址 */
UINT32  ulUserLen;          /* 用户信息区长度 */
UINT8  *pucSpecAddr;       /* 特殊信息区起始地址 */
UINT32  ulSpecLen;          /* 特殊信息区长度 */
UINT8   ucEndian;           /* 本机字节序：0 - big endian, 1 - little endian */
UINT8   ucCheckSum;        /* 管理信息区校验和 */
UINT8   ucTransfer;         /* 是否上传黑匣子数据：由自动判断算法设置或人工设置，上传完成后清除该标志位。0――不上传；1－－上传 */
UINT8   ucWriteAble;        /* 是否允许写黑匣子标志：当上传过程不允许写或人工禁止写 */
UINT32  usSeqNumNow;       /* 记录序列号*/
UINT32  usUserNum;         /* 总的用户数目  */
CPSS_SBBR_USER_T astUser[SBBR_USER_MAX-1]; /* 1 ~ SBBR_USER_MAX: User Data Section 0 is Exception Data Section */
} CPSS_SBBR_SYS_T;

/* 管理对象结构*/
typedef struct
{
UINT8 *pucTransBuf; /* 指向临时上传黑匣子数据的缓冲区首地址指针 */
UINT32  ulTransSize;    /*需要传送的总长度*/
UINT32  ulCurOffset;    /* 已经传送的长度*/
UINT8 ucExecActFlag; /* 异常信息区是否已经记录过：0－没有；1－记录过。 */
UINT8 ucExecFlag; /* 是否向异常信息区写黑匣子数据：0－不向异常信息区写入；1－向异常信息区写入 */
UINT8 ucRsv[2];
CPSS_SBBR_SYS_T *pstSys; /* 指向管理信息区的指针 */
}CPSS_SBBR_LOCAL_MNG_T;

/* 用户信息区结构*/
typedef struct
{
UINT32  ulLength;              /* 记录长度 */
UINT32  usSeqNum;             /* 记录序列号 */
UINT32  usType;               /*信息类型：文本/数据*/
UINT32  ulTimeStamp;           /* 时戳 */
UINT32   ucUser;                /* 用户 */
UINT32   ulArgs[4];             /* 用户参数 */
UINT8   aucInfo[1];             /* 用户信息 */
} CPSS_SBBR_RECORD_T;

#define CPSS_SBBR_RECORD_HEAD_SIZE  (36)

typedef struct 
{
  CPSS_SBBR_REQ_HEAD_T stHead;
  UINT8 aucData[CPSS_SBBR_TRANS_SIZE];
}CPSS_SBBR_REQ_MSG_T;

typedef struct
{
    CHAR    acFileType[8];      /* 文件类型 */
    UINT32  ulFileVersion;      /* 文件版本 */
    UINT8   aucReserved[20];    /* 保留内容 */
}CPSS_LOGM_SBBLOG_FILE_HDR_T;
/******************************** 全局变量声明 *******************************/
/*extern */
/******************************** 外部函数原形声明 ***************************/

#endif

