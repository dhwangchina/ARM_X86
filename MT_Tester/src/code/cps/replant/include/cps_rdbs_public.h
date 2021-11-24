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
#ifndef CPS__RDBS_PUBLIC_H_
#define CPS__RDBS_PUBLIC_H_

#include "cpss_public.h"

/* 返回码 */
#define  CPS__RDBS_RESULT_OK          ((UINT32)(0))     /* 成功 */
#define  CPS__RDBS_RESULT_FAIL        ((UINT32)(1))     /* 失败 */

/*in wlan_stub_ccu.c*/
#define  CPS__RDBS_IF_ASYNMSG         ((UINT8)(1))     /* 异步消息调用 */
#define  CPS__RDBS_HTONS(A,B)    ( (CPS__RDBS_IF_ASYNMSG == (B)) ? (cpss_htons(A)) : (A))
#define  CPS__RDBS_HTONL(A,B)    ( (CPS__RDBS_IF_ASYNMSG == (B)) ? (cpss_htonl(A)) : (A))

#define CPS__RDBS_MSGID_BASE            (UINT32)0x24000000
#define CPS__RDBS_TRIGMSG_BASE          ((UINT32)0x00000C00)  /* 非标准消息的起始编号 */

#define CPS__RDBS_MODULE_DEV   		   ((UINT8)(0x0A))

#define CPS__RDBS_DEV_MSGID_BASE        CPS__RDBS_MSGID_BASE + (UINT32)(CPS__RDBS_MODULE_DEV<<16)
/* CPSS根据物理地址获取逻辑地址 */
#define CPS__RDBS_DEV_GETLOGICADDR_MSG             (CPS__RDBS_DEV_MSGID_BASE + (UINT32)1)

/* CPSS根据物理地址获取逻辑地址 */
/* 输入参数结构 */
typedef struct
{
    CPSS_COM_PHY_ADDR_T    stCpuPhyAddr;  /* 物理地址 */
} CPS__RDBS_DEV_GETLOGICADDR_MSG_REQ_T;

/* 输出参数结构 */
typedef struct
{
    UINT32                 ulResult;          /* 返回结果，0表示成功，其他表示错误码 */
    CPSS_COM_LOGIC_ADDR_T  stCpuLogicAddr;    /* 逻辑地址 */
    CPSS_COM_PHY_ADDR_T    stMateBrdPhyAddr;  /* 物理地址 */
    UINT8                  ucBackupType;      /* 备份类型 */
    UINT8                  aucRsv[3];
} CPS__RDBS_DEV_GETLOGICADDR_MSG_RSP_T;
/*end in wlan_stub_ccu.c*/


/*in smss_verm.h*/

/* RDBS的CPU管理状态变更通知 */

/* RDBS版本库版本文件下载指示 */
#define CPS__RDBS_DEV_VERFILEDOWNLOAD_IND_MSG     (CPS__RDBS_DEV_MSGID_BASE + CPS__RDBS_TRIGMSG_BASE + (UINT32)1)

/* RDBS版本库版本文件删除指示 */
#define CPS__RDBS_DEV_VERFILEDEL_IND_MSG          (CPS__RDBS_DEV_MSGID_BASE + CPS__RDBS_TRIGMSG_BASE + (UINT32)2)

/* 特定物理板配置版本变更通知 */
#define CPS__RDBS_DEV_SPECVERCHG_IND_MSG          (CPS__RDBS_DEV_MSGID_BASE + CPS__RDBS_TRIGMSG_BASE + (UINT32)38)

/* 共用配置版本变更通知 */
#define CPS__RDBS_DEV_RUNVERCHG_IND_MSG           (CPS__RDBS_DEV_MSGID_BASE + CPS__RDBS_TRIGMSG_BASE + (UINT32)3)

#define CPS__OAMS_DYND_NE_UPGRADE_REQ_MSG            (0x40040C00)    /* 网元整体升级 */

/* 错误码构造宏定义(m:模块编号, r:失败原因) */
#define CPS__RDBS_RETCODE_GEN(m, r)  \
    (((UINT32)(SWP_SUBSYS_RDBS) << 24) | ((UINT32)(m) << 16) | (UINT32)(r))
#define CPS__RDBS_RET_DEV_BGN                    CPS__RDBS_RETCODE_GEN(CPS__RDBS_MODULE_DEV, 0)

typedef SWP_MO_ID_ORIGIN_T CPS__RDBS_MO_ID_T;

#define SWP_MAX_CHILD_BRD_NUM  4

typedef struct
{
   UINT32  ulBrdType;    /* 物理板类型 */
   UINT32  ulBrdVer;     /* 物理板版本 */
} SWP_PHYBRD_INFO_VER_T;

typedef struct
{
    SWP_PHYBRD_INFO_VER_T    stMotherBrd;  /* 母板（前插板） */
    SWP_PHYBRD_INFO_VER_T    stBackBrd;    /* 后插板 */
    UINT32  ulChildBrdNum;
    SWP_PHYBRD_INFO_VER_T  astChildBrd[SWP_MAX_CHILD_BRD_NUM];/*后插板*/
} SWP_PHYBRD_INTEGRATED_INFO_VER_T;

/* SMSS查询指定单板上当前运行的所有版本信息 */
/* 输入参数结构 */
#define CPS__RDBS_DEV_VERITEM_MAX             ((UINT16)(16))
typedef struct
{
    CPSS_COM_PHY_ADDR_T              stBrdPhyAddr;   /* 物理地址，其中ucCpu：1-主CPU，2-从CPU */
    SWP_PHYBRD_INTEGRATED_INFO_VER_T stPhyBoardVer;  /* 单板的硬件版本信息 */
} CPS__RDBS_DEV_GETCURRVERINFO_MSG_REQ_T;

/** 微码功能类型定义 **/
#define CPS__RDBS_DEV_FUNCTYPE_ME_DEFAULT      ((UINT8)1)      /* ME缺省类型 */
typedef struct
{
    UINT8     ucVerType;      /* 版本类型：CPU、FPGA、DSP、ME */
    UINT8     ucFuncType;     /* 功能类型，用于区分同一类逻辑器件的不同功能 */
    UINT8     ucLogicUnitNo;  /* 逻辑单元号 */
    UINT8     aucRsv[1];      /* 保留，固定填0 */
    UINT32    ulVerNo;        /* 版本号 */
    UINT32    ulVerDate;      /* 版本日期 */
    UINT32    ulFileLen;      /* 版本文件长度 */
    UINT32    ulCheckSum;     /* 版本文件校验和 */
    UINT8     aucFileName[CPSS_FS_FILENAME_LEN]; /* 版本文件名 */
} CPS__RDBS_VERINFO_T;

/* RDBS版本库版本文件下载指示 */
/* 消息体结构 */
typedef struct
{
    UINT32    ulVerDate;     /* 版本日期 */
    UINT32    ulFileLen;     /* 版本文件长度 */
    UINT32    ulCheckSum;    /* 版本文件校验和 */
    UINT8     aucVerFileName[CPSS_FS_FILENAME_LEN]; /* 版本文件名 */
    CPS__RDBS_MO_ID_T  stRdbsMoId;    /* 管理对象MO标识 */
}  CPS__RDBS_DEV_VERFILEDOWNLOAD_IND_MSG_T;

/* RDBS版本库版本文件删除指示 */
/* 消息体结构 */
typedef struct
{
    UINT8     aucVerFileName[CPSS_FS_FILENAME_LEN]; /* 版本文件名 */
    CPS__RDBS_MO_ID_T  stRdbsMoId;    /* 管理对象MO标识 */
} CPS__RDBS_DEV_VERFILEDEL_IND_MSG_T;

/* 共用配置版本变更通知 */
/* 消息体结构 */
typedef struct
{
    SWP_PHYBRD_INTEGRATED_INFO_VER_T   stPhyBoardVer;  /* 单板的硬件版本信息 */
    UINT8                              ucFuncBrdType;  /*功能单板类型*/
    UINT8                              aucRsv[3];
} CPS__RDBS_DEV_RUNVERCHG_IND_MSG_T;

typedef struct
{
    UINT32    ulResult;         /* 返回结果，0表示成功，其他表示错误码 */
    UINT8     ucBoardLogicType; /* 单板逻辑类型 */
    UINT8     aucRsv[3];
    UINT32    ulVerInfoNum;     /* 版本列表中条目个数 */
    CPS__RDBS_VERINFO_T astVerInfo[CPS__RDBS_DEV_VERITEM_MAX];  /* 版本列表 */
} CPS__RDBS_DEV_GETCURRVERINFO_MSG_RSP_T;

/* SMSS查询指定单板上当前运行的所有版本信息 */
#define CPS__RDBS_DEV_GETCURRVERINFO_MSG           (CPS__RDBS_DEV_MSGID_BASE + (UINT32)3)

#define  CPS__RDBS_IF_FUNC            ((UINT8)(0))     /* 函数调用 */

/* SMSS设置指定单板的运行状态 */
/* 运行状态取值宏定义 */
#define CPS__RDBS_DEV_VALUE_BOARD_NORMAL       ((UINT8)(0))   /* 正常 */
#define CPS__RDBS_DEV_VALUE_BOARD_FAULT        ((UINT8)(1))   /* 故障 */
#define CPS__RDBS_DEV_VALUE_BOARD_OFFLINE      ((UINT8)(2))   /* 不在位 */
#define CPS__RDBS_DEV_VALUE_BOARD_MISBRDTYPE   ((UINT8)(3))   /* 单板类型不一致 */

#define CPS__RDBS_DEV_VT_HOSTCPU               ((UINT8)1)      /* CPU */
#define CPS__RDBS_DEV_VT_SLAVECPU              ((UINT8)2)      /* CPU */
#define CPS__RDBS_DEV_VT_DSP                   ((UINT8)3)      /* DSP */
#define CPS__RDBS_DEV_VT_SLAVEMNG_ME           ((UINT8)4)      /* 从处理器管理的ME */
#define CPS__RDBS_DEV_VT_FPGA                  ((UINT8)5)      /* FPGA */
#define CPS__RDBS_DEV_VT_SCDFPGA               ((UINT8)6)      /* SCARD_FPGA */
#define CPS__RDBS_DEV_VT_HOSTMNG_ME            ((UINT8)7)      /* 主处理器管理的ME */

#define CPS__RDBS_DEV_FUNCTYPE_CPU_INGRESS     ((UINT8)2)      /* 主CPU功能 */
#define CPS__RDBS_DEV_FUNCTYPE_CPU_EGRESS      ((UINT8)3)      /* 从CPU功能 */


/* 特定物理板配置版本变更通知 */
typedef struct
{
    CPSS_COM_PHY_ADDR_T    stBrdPhyAddr;  /* 物理地址 */
} CPS__RDBS_DEV_SPECVERCHG_IND_MSG_T;

/* SMSS修改单板的版本状态 */
#define CPS__RDBS_DEV_SETBOARDVERSTATE_MSG         (CPS__RDBS_DEV_MSGID_BASE + (UINT32)32)

/* SMSS修改单板的版本状态 */
typedef struct
{
    CPSS_COM_PHY_ADDR_T stPhyAddr;    /* 单板物理地址 */
    UINT16    usVerState;    /* 单板版本状态 */
    UINT8    aucRsv[2];
} CPS__RDBS_DEV_SETBOARDVERSTATE_MSG_REQ_T;

typedef struct
{
    UINT32      ulResult;        /* 返回结果，0表示成功，其他表示错误码 */
} CPS__RDBS_DEV_SETBOARDVERSTATE_MSG_RSP_T;

/*end in smss_verm.h*/

/*in smss_sysctl_slave.h */

/* RDBS的CPU管理状态变更通知 */
/* 消息体结构 */
typedef struct
{
    CPSS_COM_PHY_ADDR_T    stCpuPhyAddr;  /* 物理地址 */
    UINT8      ucMode;        /* 管理操作类型 */
    UINT8      ucValue;       /* 管理操作值 */
    UINT8      aucRsv[2];
} CPS__RDBS_DEV_CPUMSTATUSCHG_IND_MSG_T;

/*end in smss_sysctl_slave.h */

/*in smss_verm_verlib.c*/

/* verlib表 */
#define CPS__RDBS_DEV_VERLIB_MAX               ((UINT32)1000)
/* 关系表允许最大记录数 */
#define CPS__RDBS_DEV_TBL_VERLIB_MAX ((UINT32)CPS__RDBS_DEV_VERLIB_MAX)

/*     OAMS查询版本库中所有版本状态 */
#define CPS__RDBS_DEV_VERNUM_MAX     CPS__RDBS_DEV_TBL_VERLIB_MAX  /* 和版本库的表容量保持一致 */
typedef struct
{
    UINT8    aucFileName[CPSS_FS_FILENAME_LEN];    /* 文件名 */
    UINT16    usState;        /* 文件状态 */
    UINT8    aucRsv[2];    /* 填充字节 */
    UINT32    ulFileLen;    /* 文件长度 */
    UINT32    ulCheckSum;    /* 文件校验和 */
    CPS__RDBS_MO_ID_T  stRdbsMoId;    /* 管理对象MO标识 */
} CPS__RDBS_DEV_VER_T;
typedef struct
{
    UINT32      ulResult;        /* 返回结果，0表示成功，其他表示错误码 */
    UINT32      ulFileNum;        /* 版本文件数目 */
    CPS__RDBS_DEV_VER_T    astVer[CPS__RDBS_DEV_VERNUM_MAX];
} CPS__RDBS_DEV_GETALLVERSTATE_MSG_RSP_T;

/* OAMS查询版本库中所有版本状态 */
#define CPS__RDBS_DEV_GETALLVERSTATE_MSG           (CPS__RDBS_DEV_MSGID_BASE + (UINT32)34)

/* SMSS修改版本库中的单个版本文件状态 */
typedef struct
{
    UINT8    aucFileName[CPSS_FS_FILENAME_LEN];    /* 文件名 */
    UINT16    usState;        /* 文件状态 */
    UINT8    aucRsv[2];
} CPS__RDBS_DEV_SETVERFILESTATE_MSG_REQ_T;

typedef struct
{
    UINT32      ulResult;        /* 返回结果，0表示成功，其他表示错误码 */
} CPS__RDBS_DEV_SETVERFILESTATE_MSG_RSP_T;

/* SMSS修改版本库中的单个版本文件状态 */
#define CPS__RDBS_DEV_SETVERFILESTATE_MSG          (CPS__RDBS_DEV_MSGID_BASE + (UINT32)33)

/* SMSS子系统 文件名称长度 */
#define CPS__OAMS_DIAG_FILENAME_MAX_LEN              (32)

/* 重新下载文件到版本库请求消息中参数区结构 */
typedef struct
{
    CHAR         acFileName[CPS__OAMS_DIAG_FILENAME_MAX_LEN]; /* 版本文件名 */
    SWP_MO_ID_T  stMoId;                                 /* 版本文件MO_ID */
} CPS__OAMS_DIAG_SMSS_REDOWNLOAD_VERFILE_REQ_T;

/*end in smss_verm_verlib.c*/

/*in smss_sysctl.c*/

#define CPS__RDBS_DEV_LOADLEVEL_MAX    ((UINT32)10)

/*smss查询指定CPU的负荷控制参数*/
typedef struct
{
    CPSS_COM_PHY_ADDR_T  stCpuPhyAddr;  /* 物理地址 */
}CPS__RDBS_DEV_GETCPULOADCTRL_MSG_REQ_T;
typedef struct
{
    UINT8  ucLoadThreshold;      /*本级别CPU的过载门限*/
    UINT8  aucRsv[1];
    UINT16 usLogRate;            /*日志发送速率， 单位条/秒*/
}CPS__RDBS_DEV_CPULOADINFO_T;
typedef struct
{
    UINT32   ulResult;          /*返回结果，0表示成功，其他表示错误码*/
    UINT16   usWatchTimer;      /* 主Cpu功能类型，即单板功能类型 */
    UINT16   usAlarmTimer;
    UINT8    ucLoadInfoNum;
    UINT8    aucRsv[3];
    CPS__RDBS_DEV_CPULOADINFO_T astLoadInfo[CPS__RDBS_DEV_LOADLEVEL_MAX];
}CPS__RDBS_DEV_GETCPULOADCTRL_MSG_RSP_T;

typedef struct
{
    UINT32   ulResult;          /*返回结果，0表示成功，其他表示错误码*/
    UINT8    ucLoadNum;
    UINT8    ucSlaveCpuCoeff;
    UINT8    aucLoadThreshold[CPS__RDBS_DEV_LOADLEVEL_MAX];  /*过载门限*/
}CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG_RSP_T;

/*SMSS查询指定CPU的负荷控制信息 */
#define CPS__RDBS_DEV_GETCPULOADCTRL_MSG            (CPS__RDBS_DEV_MSGID_BASE + (UINT32)60)

/*smss设置指定单板的过载状态*/
typedef struct
{
    CPSS_COM_PHY_ADDR_T  stBoardPhyAddr;    /* 物理地址,主CPU物理地址*/
    UINT8                ucOverloadState;     /*单板过载状态*/
    UINT8                aucRsv[3];
}CPS__RDBS_DEV_SETOVERLOADSTATE_MSG_REQ_T;
typedef struct
{
    UINT32              ulResult;          /*返回结果，0表示成功，其他表示错误码*/
}CPS__RDBS_DEV_SETOVERLOADSTATE_MSG_RSP_T;

/*SMSS设置指定单板的过载状态*/
#define CPS__RDBS_DEV_SETOVERLOADSTATE_MSG          (CPS__RDBS_DEV_MSGID_BASE + (UINT32)62)

/*SMSS查询指定单板的负荷控制信息*/
#define CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG          (CPS__RDBS_DEV_MSGID_BASE + (UINT32)61)

/*CPU的负荷控制状态变更通知SMSS*/
#define CPS__RDBS_DEV_CPULOADCTRLCHG_IND_MSG      (CPS__RDBS_DEV_MSGID_BASE + CPS__RDBS_TRIGMSG_BASE + (UINT32)42)

/*单板的负荷控制状态变更通知SMSS*/
#define CPS__RDBS_DEV_BOARDLOADCTRLCHG_IND_MSG    (CPS__RDBS_DEV_MSGID_BASE + CPS__RDBS_TRIGMSG_BASE + (UINT32)43)

/* SMSS设置指定CPU的运行状态 */
#define CPS__RDBS_DEV_SETCPUSTATUS_MSG             (CPS__RDBS_DEV_MSGID_BASE + (UINT32)5)

/* SMSS设置指定CPU的运行状态 */
/* 输入参数结构 */
/* 运行状态操作类型宏 */
#define CPS__RDBS_DEV_MODE_CPU_HWFAULT        ((UINT8)(0))   /* 故障/正常 */
#define CPS__RDBS_DEV_MODE_CPU_MASTERORSLAVE  ((UINT8)(3))   /* 主/备 */
typedef struct
{
    CPSS_COM_PHY_ADDR_T stCpuPhyAddr;  /* 物理地址 */
    UINT8               ucMode;        /* 运行状态操作类型 */
    UINT8               ucValue;       /* 运行状态值 */
    UINT8               aucRsv[2];
} CPS__RDBS_DEV_SETCPUSTATUS_MSG_REQ_T;

/* 输出参数结构 */
typedef struct
{
    UINT32    ulResult;      /* 返回结果，0表示成功，其他表示错误码 */
} CPS__RDBS_DEV_SETCPUSTATUS_MSG_RSP_T;

/*CPU负荷控制参数改变，通知SMSS*/
typedef struct
{
    UINT8    ucCpu;
    UINT8    ucLoadInfoNum;
    UINT8    aucRsv[2];
    UINT16   usWatchTimer;
    UINT16   usAlarmTimer;
    CPS__RDBS_DEV_CPULOADINFO_T astLoadInfo[CPS__RDBS_DEV_LOADLEVEL_MAX];
}CPS__RDBS_DEV_CPULOADCTRLCHG_IND_MSG_T;

/*smss查询指定单板的负荷控制参数*/
typedef struct
{
    CPSS_COM_PHY_ADDR_T  stCpuPhyAddr;  /* 物理地址,主CPU物理地址*/
}CPS__RDBS_DEV_GETBOARDLOADCTRL_MSG_REQ_T;

typedef struct
{
    UINT8    ucSlaveCpuCoeff;                            /* 从CPU或DSP的折算系数*/
    UINT8    ucLoadNum;                                 /* 过载门限有效值数目*/
    UINT8    aucLoadThreshold[CPS__RDBS_DEV_LOADLEVEL_MAX];  /* 过载门限*/
}CPS__RDBS_DEV_BOARDLOADCTRLCHG_IND_MSG_T;

/* SMSS查询指定CPU的状态 */
/* 输入参数结构 */
typedef struct
{
    CPSS_COM_PHY_ADDR_T    stCpuPhyAddr;  /* 物理地址 */
} CPS__RDBS_DEV_GETCPUSTATUS_MSG_REQ_T;

/* 输出参数结构 */
typedef struct
{
    UINT32   ulResult;      /* 返回结果，0表示成功，其他表示错误码 */

    /* 管理状态：bit0=0/1, 未闭塞/闭塞；
    bit1=0/1，未延时闭塞/延时闭塞；
    bit2～bit6:保留，取0；bit15=0/1：监控/关闭监控 */
    UINT16   usMStatus;

   /*  运行状态字
    *   Bit0 : 0-正常, 1-故障
    *   Bit3 : 0-主用, 1-备用
    *   其他 : 保留（取值0）
    */
    UINT16   usStatus;
} CPS__RDBS_DEV_GETCPUSTATUS_MSG_RSP_T;

/* SMSS查询指定CPU的状态 */
#define CPS__RDBS_DEV_GETCPUSTATUS_MSG             (CPS__RDBS_DEV_MSGID_BASE + (UINT32)4)

/*end in smss_sysctl.c*/

/*in smss_sysctl_gcpa.c*/

/* RDBS的CPU管理状态变更通知 */
#define CPS__RDBS_DEV_CPUMSTATUSCHG_IND_MSG       (CPS__RDBS_DEV_MSGID_BASE + CPS__RDBS_TRIGMSG_BASE + (UINT32)0)

/* 一级交换板增加通知 */
#define CPS__RDBS_DEV_GSSAADD_IND_MSG             (CPS__RDBS_DEV_MSGID_BASE + CPS__RDBS_TRIGMSG_BASE + (UINT32)36)

/* 一级交换板删除通知 */
#define CPS__RDBS_DEV_GSSADEL_IND_MSG             (CPS__RDBS_DEV_MSGID_BASE + CPS__RDBS_TRIGMSG_BASE + (UINT32)37)

/* RDBS 单板删除通知 */
#define CPS__RDBS_DEV_BOARDDEL_IND_MSG            (CPS__RDBS_DEV_MSGID_BASE + CPS__RDBS_TRIGMSG_BASE + (UINT32)4)

/* SMSS查询机框物理类型 */
#define CPS__RDBS_DEV_GETSHELFPHYTYPE_MSG          (CPS__RDBS_DEV_MSGID_BASE + (UINT32)2)

#define CPS__RDBS_DEV_ST_ATCA                 ((UINT8)14)     /* ATCA标准机框 */

/* SMSS查询本网元内所有单板的物理信息 */
#define CPS__RDBS_DEV_GETALLBRDPHYINFO_MSG          (CPS__RDBS_DEV_MSGID_BASE + (UINT32)59)

/* SMSS修改单板的运行状态 */
#define CPS__RDBS_DEV_SETBOARDSTATUS_MSG             (CPS__RDBS_DEV_MSGID_BASE + (UINT32)45)

/* 查询本网元内所有单板信息 */
#define CPS__RDBS_DEV_GETALLBRDINFO_MSG            (CPS__RDBS_DEV_MSGID_BASE + (UINT32)22)

/* 根据主CPU功能类型查询主CPU配置信息 */
#define CPS__RDBS_DEV_GETHCPUINFOBYTYPE_MSG          (CPS__RDBS_DEV_MSGID_BASE + (UINT32)54)

/* 查询本网元内所有单板信息 */
#define CPS__RDBS_DEV_GETALLBRDINFO_MSG            (CPS__RDBS_DEV_MSGID_BASE + (UINT32)22)

/* SMSS查询风机控制参数 */
#define CPS__RDBS_DEV_GETFANCTRL_MSG                (CPS__RDBS_DEV_MSGID_BASE + (UINT32)58)

/* 管理操作类型宏 */
#define CPS__RDBS_OMC_MODE_BLOCK      ((UINT8)(0))      /* 立即闭塞 */
#define CPS__RDBS_OMC_MODE_DELAY      ((UINT8)(1))      /* 延时闭塞 */
#define CPS__RDBS_OMC_MODE_OBSERVE    ((UINT8)(15))     /* 动态数据监测 */

/* 状态操作值 */
#define CPS__RDBS_VALUE_UNBLOCK       ((UINT8)(0))      /* 解闭塞操作 */
#define CPS__RDBS_VALUE_BLOCK         ((UINT8)(1))      /* 闭塞操作 */
#define CPS__RDBS_VALUE_DELAY         ((UINT8)(2))      /* 延迟闭塞 */

/* SMSS查询机框物理类型 */
/* 输入参数结构 */
typedef struct
{
    UINT8    ucFrame;       /* 机架号 */
    UINT8    ucShelf;       /* 机框号 */
    UINT8    aucRsv[2];
} CPS__RDBS_DEV_GETSHELFPHYTYPE_MSG_REQ_T;
/* 输出参数结构 */
/* 机框类型 */
#define CPS__RDBS_DEV_ST_UNKNOWN              ((UINT8)0)      /* 类型不明 */
#define CPS__RDBS_DEV_ST_ATCA                 ((UINT8)14)     /* ATCA标准机框 */
#define CPS__RDBS_DEV_ST_DT                   ((UINT8)15)     /* 自研机框 */
typedef struct
{
    UINT32  ulResult;        /* 返回结果，0表示成功，其他表示错误码 */
    UINT8   ucShelfPhyType;  /* 机框类型 */
    UINT8   aucRsv[3];
} CPS__RDBS_DEV_GETSHELFPHYTYPE_MSG_RSP_T;

#define CPS__RDBS_DEV_FRAME_MAX        ((UINT8)5)   /* 机架号最大值 */
#define CPS__RDBS_DEV_SHELF_MAX        ((UINT8)3)      /* 机框号最大值 */
#define CPS__RDBS_DEV_SLOT_MAX         ((UINT8)14)     /* 槽位号最大值 */
#define CPS__RDBS_DEV_TBL_BOARD_MAX   ((UINT32)((CPS__RDBS_DEV_FRAME_MAX) * (CPS__RDBS_DEV_SHELF_MAX) * (CPS__RDBS_DEV_SLOT_MAX)))
#define CPS__RDBS_DEV_MAX_BRD_NUM             ((UINT8)(CPS__RDBS_DEV_TBL_BOARD_MAX)) /* 网元内单板最大数 */

/* SMSS查询本网元内所有单板的物理信息 */
typedef struct
{
    CPSS_COM_PHY_ADDR_T  stBrdPhyAddr;  /* 物理地址 */
    SWP_PHYBRD_INTEGRATED_INFO_VER_T stPhyBoardVer;  /* 单板的硬件版本信息 */
    CPS__RDBS_MO_ID_T  stRdbsMoId;    /* 管理对象MO标识 */
    UINT32   ulBoardPhyType;  /* 物理类型 */
    UINT8    ucCpuType;      /* 主Cpu功能类型，即单板功能类型 */
    UINT8    aucRsv[3];
    UINT16   usMStatus;
    UINT16   usStatus;
}CPS__RDBS_DEV_BRDPHYINFO_T;
typedef struct
{
    UINT32   ulResult;       /* 返回结果，0表示成功，其他表示错误码 */
    UINT32   ulBrdNum;      /* 单板数目*/
    CPS__RDBS_DEV_BRDPHYINFO_T  astBrdInfo[CPS__RDBS_DEV_MAX_BRD_NUM];
}CPS__RDBS_DEV_GETALLBRDPHYINFO_MSG_RSP_T;

/* OAMS查询本网元内所有单板信息 */
/* 输入参数结构 */

/* 输出参数结构 */
#define CPS__RDBS_DEV_MAX_BRD_NUM   ((UINT8)(CPS__RDBS_DEV_TBL_BOARD_MAX))
typedef struct
{
    CPSS_COM_PHY_ADDR_T  stBrdPhyAddr;  /* 物理地址 */
    CPS__RDBS_MO_ID_T  stRdbsMoId;    /* 管理对象MO标识 */
    UINT32   ulBoardPhyType;  /* 物理类型 */
    UINT8    ucCpuType;       /* 主Cpu功能类型，即单板功能类型 */
    UINT8    aucRsv[3];
    UINT16   usMStatus;
    UINT16   usStatus;
}CPS__RDBS_DEV_BRDINFO_T;

typedef struct
{
    UINT32   ulResult;       /* 返回结果，0表示成功，其他表示错误码 */
    UINT32   ulBrdNum;       /* 单板数目*/
    CPS__RDBS_DEV_BRDINFO_T  astBrdInfo[CPS__RDBS_DEV_MAX_BRD_NUM];
} CPS__RDBS_DEV_GETALLBRDINFO_MSG_RSP_T;

/* SMSS设置指定单板的运行状态 */
/* 运行状态取值宏定义 */
#define CPS__RDBS_DEV_VALUE_BOARD_NORMAL       ((UINT8)(0))   /* 正常 */
#define CPS__RDBS_DEV_VALUE_BOARD_FAULT        ((UINT8)(1))   /* 故障 */
#define CPS__RDBS_DEV_VALUE_BOARD_OFFLINE      ((UINT8)(2))   /* 不在位 */
#define CPS__RDBS_DEV_VALUE_BOARD_MISBRDTYPE   ((UINT8)(3))   /* 单板类型不一致 */

typedef struct
{
    CPSS_COM_PHY_ADDR_T     stCpuPhyAddr;  /* 物理地址 */
    UINT8                   ucValue;       /* 运行状态值*/
    UINT8                   aucRsv[3];
} CPS__RDBS_DEV_SETBOARDSTATUS_MSG_REQ_T;

typedef struct
{
    UINT32    ulResult;      /* 返回结果，0表示成功，其他表示错误码 */
} CPS__RDBS_DEV_SETBOARDSTATUS_MSG_RSP_T;

/* 根据主CPU功能类型查询主CPU配置信息 */
/* 输入参数结构 */
typedef struct
{
    UINT8    ucCpuType;      /* 主CPU类型与单板功能类型取值一致，具体取值参见统一软件平台公共约定设计文档 */
    UINT8    aucRsv[3];
} CPS__RDBS_DEV_GETHOSTCPUINFOBYTYPE_MSG_REQ_T;

/* 输出参数结构 */
typedef struct
{
    UINT8  ucFrame;  /* 机架号 */
    UINT8  ucShelf;  /* 机框号 */
    UINT8  ucSlot;   /* 槽位号 */
    UINT8  ucHostCpu;/* 主CPU号 */
    UINT8  ucCpuType;  /* CPU功能类型，参见统一软件平台公共约定中功能板类型相关宏定义*/
    UINT8  aucRsv[3];
    UINT8  ucModule; /* 模块号 */
    UINT8  ucSGroup; /* 子组号 */
    UINT16 usGroup;  /* 组号 */
    UINT16 usMStatus;/* 管理状态，具体取值参见《统一软件平台RDBS关系表设计报告》 4.1.1.4节 */
    UINT16 usStatus; /* 运行状态，具体取值参见《统一软件平台RDBS关系表设计报告》4.1.1.4节*/
} CPS__RDBS_DEV_HOSTCPUINFO_T;

typedef struct
{
    UINT32   ulResult;          /* 返回结果，0表示成功，其他表示错误码 */
    UINT32   ulHostCpuNum;      /* 主CPU数目*/
    CPS__RDBS_DEV_HOSTCPUINFO_T  astHostCpuInfo[CPS__RDBS_DEV_MAX_BRD_NUM];
} CPS__RDBS_DEV_GETHOSTCPUINFOBYTYPE_MSG_RSP_T;

typedef struct
{
    UINT8    ucTti;              /*  机框启动散热时间 */
    UINT8    ucFanUpThreshold;   /*  风机转速上限阈值 */
    UINT8    ucFanDownThreshold; /*  风机转速下限阈值 */
    UINT8    ucFanUpAlarm;       /* 风扇转速上限告警门限 */
    UINT8    ucNpwmUp;         /*  风扇转速上调粒度 */
    UINT8    ucNpwmDown;      /*  风扇转速下调粒度 */
    UINT16   usTfc;              /* 风扇转速调整时间间隔 */
    UINT8    ucTpudelay;         /* 单板在线扫描启动延时*/
    UINT8    aucRsv[3];
}CPS__RDBS_DEV_FANCTRL_CFG_T;
typedef struct
{
    UINT32   ulResult;       /* 返回结果，0表示成功，其他表示错误码 */
    CPS__RDBS_DEV_FANCTRL_CFG_T stFanCtrlCfg;
} CPS__RDBS_DEV_GETFANCTRL_MSG_RSP_T;

/* RDBS 单板删除通知 */
/* 消息体结构 */
typedef struct
{
    UINT8    ucFrame;       /* 机架号 */
    UINT8    ucShelf;       /* 机框号 */
    UINT8    ucSlot;        /* 槽位号 */
    UINT8    aucRsv[1];
} CPS__RDBS_DEV_BOARDDEL_IND_MSG_T;

/*end in smss_sysctl_gcpa.c*/

/*in smss_devm.c*/

/* 风扇控制参数修改后通知全局板SMSS的设备监视纤程 */
#define CPS__RDBS_DEV_FANCTRLCHG_IND_MSG          (CPS__RDBS_DEV_MSGID_BASE + CPS__RDBS_TRIGMSG_BASE + (UINT32)41)

#define  CPS__RDBS_MODULE_ATM        ((UINT8)(0x0B))

#define CPS__RDBS_ATM_MSGID_BASE        CPS__RDBS_MSGID_BASE + (UINT32)(CPS__RDBS_MODULE_ATM<<16)

#define CPS__RDBS_ATM_CLKEXTRACTADD_IND_MSG          CPS__RDBS_ATM_MSGID_BASE + CPS__RDBS_TRIGMSG_BASE + (UINT32)45 /*	本板线路时钟提取端口增加通知 */

#define CPS__RDBS_ATM_CLKEXTRACTDEL_IND_MSG          CPS__RDBS_ATM_MSGID_BASE + CPS__RDBS_TRIGMSG_BASE + (UINT32)46 /* 本板线路时钟提取端口删除通知 */

/*end in smss_devm.c*/

#define  CPS__RDBS_IPV6_ADDR_LEN      ((UINT8)(16))

/* IP地址定义 */
typedef struct
{
    UINT8    ucType;         /* 0,IPV4; 1,IPV6 */
    UINT8    ucValid;         /* 0, invalid; 1, valid*/
    UINT8    ucIpPrefix;     /* IP前缀，如果没有，填0,  */
    UINT8    ucRvs;
    UINT8    aucIp[CPS__RDBS_IPV6_ADDR_LEN];
}CPS__RDBS_IPADDR_T;

/*in cpss_com_link.c*/

/* 单板备份类型 */
#define CPS__RDBS_DEV_11_BACKUP       ((UINT8)(1))      /* 单板1+1备份 */
#define CPS__RDBS_DEV_NO_BACKUP       ((UINT8)(0))      /* 单板无备份 */

/* CPSS根据逻辑地址获取物理地址 */
/* 输入参数结构 */
typedef struct
{
    CPSS_COM_LOGIC_ADDR_T    stCpuLogicAddr;    /* 逻辑地址 */
    UINT32                   ulMasterOrSlave;   /* 主备板位标识 */
} CPS__RDBS_DEV_GETPHYADDR_MSG_REQ_T;
/* 输出参数结构 */
typedef struct
{
    UINT32                ulResult;       /* 返回结果，0表示成功，其他表示错误码 */
    CPSS_COM_PHY_ADDR_T   stCpuPhyAddr;   /* 物理地址 */
} CPS__RDBS_DEV_GETPHYADDR_MSG_RSP_T;

/* 控制面板间连通性测试请求消息中参数区结构 */
typedef struct
{
    CPSS_COM_PHY_ADDR_T stDstPhyAddr;   /* 目标物理地址 */
    UINT32 ulTestLen;                   /* 测试字符串长度 */
    UINT32 ulTestNum;                   /* 测试次数 */
} CPS__OAMS_DIAG_CPSS_CTRLP_COMM_TEST_REQ_T;

/* CPSS根据逻辑地址获取物理地址 */
#define CPS__RDBS_DEV_GETPHYADDR_MSG               (CPS__RDBS_DEV_MSGID_BASE + (UINT32)0)

/*end in cpss_com_link.c*/

#endif /* CPS__RDBS_PUBLIC_H_ */
