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
#ifndef CPS__OAMS_PUBLIC_H_
#define CPS__OAMS_PUBLIC_H_

#include "cpss_public.h"

/* 操作结果 */
#define CPS__OAMS_OK         (0)     /* 成功 */
#define CPS__OAMS_ERROR      (1)     /* 失败 */

/*in wlan_stub_ccu.c*/

typedef INT32 (*CPS__OAMS_SHCMD_PF)(UINT32 ulPara0, UINT32 ulPara1,UINT32 ulPara2, UINT32 ulPara3);

/* 管理对象标识, 同SWP_MO_ID_T
*  管理对象标识用于在网元内部对一个管理对象进行全局描述的作用。*/
typedef SWP_MO_ID_T CPS__OAMS_MO_ID_T;

/* 告警类型枚举 */
typedef enum
{
    CPS__OAMS_ALARM_OCCUR,           /* 告警产生 */
    CPS__OAMS_ALARM_CLEAR,           /* 告警自然清除 */
    CPS__OAMS_ALARM_CLEAR_OBJ_RST,   /* 故障对象复位导致的告警清除 */
    CPS__OAMS_ALARM_CLEAR_OBJ_DEL,   /* 故障对象被配置删除的告警清除 */
    CPS__OAMS_ALARM_CLEAR_SRC_RST,   /* 告警源复位导致的告警清除 */
    CPS__OAMS_ALARM_CLEAR_SRC_DEL,   /* 告警源被配置删除的告警清除 */
    CPS__OAMS_ALARM_CLEAR_MANUAL,    /* 告警手工清除 */
    CPS__OAMS_ALARM_CLEAR_ASSOC      /* 告警相关性导致的告警清除 */
}CPS__OAMS_AM_ALARM_TYPE_E;

#define CPS__OAMS_AM_ALARM_EXTRA_INFO_LEN    (64)      /* 告警额外信息 */

/* 原始告警信息
*  告警号, 告警细节号等字段的编号规则, 参见各<告警项手册>*/
typedef struct
{
    UINT32                  ulAlarmNo;      /* 告警号 */
    UINT32                  ulAlarmSubNo;   /* 告警细节号 */
    CPS__OAMS_AM_ALARM_TYPE_E    enAlarmType;    /* 告警类型 */
    CPS__OAMS_MO_ID_T            stFaultMoID;    /* 故障对象的MO_ID */
    UINT8  aucExtraInfo[CPS__OAMS_AM_ALARM_EXTRA_INFO_LEN];  /* 告警额外信息 */
} CPS__OAMS_AM_ORIGIN_ALARM_T;

#define CPS__OAMS_AM_EVENT_EXTRA_INFO_LEN    256     /* 事件额外信息 */

/* 原始事件信息
*  事件号, 事件细节号等字段的编号规则, 参见各<事件项手册>*/
typedef struct
{
    UINT32  ulEventNo;      /* 事件号 */
    UINT32  ulEventSubNo;   /* 事件细节号 */
    CPS__OAMS_MO_ID_T    stEventMoId;    /* 事件所对应对象的MO_ID */
    UINT32  ulExtraLen;     /* 事件额外信息长度 */
    UINT8   aucExtraInfo[CPS__OAMS_AM_EVENT_EXTRA_INFO_LEN]; /* 事件额外信息 */
} CPS__OAMS_AM_ORIGIN_EVENT_T;

/*end in wlan_stub_ccu.c*/

/*end in smss_verm.h*/

/* SMSS子系统 文件名称长度 */
#define CPS__OAMS_DIAG_FILENAME_MAX_LEN              (32)

/* 测试补充信息长度 */
#define   CPS__OAMS_DIAG_TEST_PARAM_MAX_LEN      (256)

/* M接口请求消息标准头部 */
typedef struct
{
    UINT32  ulSeqId;    /* 流水号，由请求消息的发送方填写，在相应的响应消息中 */
                        /* 需要回填该字段。其他情况下该字段无确定用途，由发送 */
                        /* 方自行使用，建议填0。*/
} CPS__OAMS_M_REQ_HDR_T, CPS__OAMS_O_REQ_HDR_T;

/* 启动测试活动请求消息结构 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T  stReqHeader;  /* 请求消息头部 */
    UINT32            ulTestId;     /* 测试活动ID */
    UINT32            ulTestCode;   /* 诊断测试码 */
    CPS__OAMS_MO_ID_T      stMortId;     /* MORT标识符 */
    UINT32            ulParamLen;   /* 参数区长度, 最大为64字节 */
    UINT8  aucTestParam[CPS__OAMS_DIAG_TEST_PARAM_MAX_LEN];  /* 参数区内容 */
} CPS__OAMS_DIAG_START_TEST_REQ_MSG_T;

/* 测试报告信息长度 */
#define   CPS__OAMS_DIAG_TEST_RPT_MAX_LEN        (32 * 1024)
#define   CPS__OAMS_DIAG_RPT_TYPE_MIDDLE_EX        (2)   /* 中间报告(不需要解析) */

/* 测试结果类型 */
#define CPS__OAMS_DIAG_RESULT_TYPE_OK            (0)   /* 测试成功 */
#define CPS__OAMS_DIAG_RESULT_TYPE_FAIL          (1)   /* 测试失败 */

#define CPS__OAMS_PD_A_DIAG_AGT_PROC         CPSS_VK_PD(84, 0)   /* 诊断测试管理代理 */

/* M接口消息ID, ID范围: 0x30050000～0x3005ffff */
#define CPS__OAMS_DIAG_START_TEST_REQ_MSG        (0x30050000)
#define CPS__OAMS_DIAG_START_TEST_RSP_MSG        (0x30050001)
#define CPS__OAMS_DIAG_STOP_TEST_IND_MSG         (0x30050002)
#define CPS__OAMS_DIAG_TEST_APP_RPT_IND_MSG      (0x30050004)

/* 原始测试报告  执行测试活动的子系统->测试代理 */
typedef struct
{
    UINT32  ulTestId;       /* 测试活动ID */
    UINT32  ulRptType;      /* 测试报告类型(中间、最终) */
    UINT32  ulTestResult;   /* 测试结果(成功、失败) */
    UINT32  ulTestRptLen;   /* 报告长度，最大为32k字节 */
    UINT8   aucRptInfo[CPS__OAMS_DIAG_TEST_RPT_MAX_LEN];  /* 报告信息 */
} CPS__OAMS_DIAG_APP_TEST_RPT_T;

/* 子系统上报测试报告指示消息结构 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T          stReqHeader;  /* 请求消息头部 */
    CPS__OAMS_DIAG_APP_TEST_RPT_T  stRptInfo;    /* 测试报告内容 */
} CPS__OAMS_DIAG_TEST_APP_RPT_IND_MSG_T;

/*end in smss_verm.h*/

/*in smss_sysctl_gcpa.h */
/* 网管对象强制复位 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T   stReqHeader;    /* 公共消息头部 */
    CPS__OAMS_MO_ID_T       stTargetMoId;   /* 目标MO标识  */

    /* 目前仅对单板支持延迟复位时间参数，用于多单板的版本激活控制。*/
    UINT32             ulDelayTime;    /* 延迟复位时间（单位：秒） */

} CPS__OAMS_DYND_FORCE_RST_REQ_MSG_T;

/* M接口响应消息标准头部 */
typedef struct
{
    UINT32  ulSeqId;        /*流水号，响应消息中需要回填请求消息的流水号. */
    UINT32  ulReturnCode;   /*返回码，0固定表示成功，其他为失败原因. */
} CPS__OAMS_M_RSP_HDR_T, CPS__OAMS_O_RSP_HDR_T;

/* 网管对象强制切换 */
/* 请求消息 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T  stReqHeader;  /* 公共消息头部 */
    CPS__OAMS_MO_ID_T      stTargetMoId; /* 目标MO标识  */
    CPSS_COM_PID_T    stSrcPid;     /* 源纤程描述符,SMSS保存，在响应消息中回填 */
}CPS__OAMS_DYND_FORCE_HO_REQ_MSG_T;

/* 响应消息 */
typedef struct
{
    CPS__OAMS_M_RSP_HDR_T  stRspHeader;   /* 公共消息头部 */
    CPSS_COM_PID_T    stSrcPid;      /* 源纤程描述符,SMSS保存，在响应消息中回填 */
    BOOL              bIsMainCtrlBrd;/* 是否为全局板 */
}CPS__OAMS_DYND_FORCE_HO_RSP_MSG_T;

/*end in smss_sysctl_gcpa.h */

/*in smss_verm.c*/

#define CPS__OAMS_DYND_NE_UPGRADE_RSP_MSG            (0x40040C01)
#define CPS__OAMS_DYND_NE_UPGRADE_TYPE_BGN       (0) /* 网元升级开始 */
#define CPS__OAMS_DYND_NE_UPGRADE_TYPE_END_AUTO  (1) /* 网元升级完成（自动复位） */

/* 网元整体升级 */
/* 请求消息 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T stReqHeader; /* 公共消息头部 */
    UINT32           ulOpType;    /* 网元整体升级操作类型 */

    /* 要下载的压缩文件名（含相对路径，并且含.zip文件后缀），
    *  文件服务器信息直接使用OMC设置的配置服务器；
    */
    CHAR acDataFileName[CPSS_FS_FILEDIR_LEN];
}CPS__OAMS_DYND_NE_UPGRADE_REQ_MSG_T;

typedef CPS__OAMS_M_RSP_HDR_T CPS__OAMS_DYND_NE_UPGRADE_RSP_MSG_T;

/* MO_ID类型 */
#define CPS__OAMS_MOID_TYPE_SHELF            (0)  /* 所在机框 */
#define CPS__OAMS_MOID_TYPE_BOARD            (1)  /* 所在单板 */

/* 重新下载文件到版本库 */
#define CPS__OAMS_DIAG_SMSS_REDOWNLOAD_VERFILE2VERLIB_TESTCODE   (0x00030001)

/* 重新下载文件到指定单板 */
#define CPS__OAMS_DIAG_SMSS_REDOWNLOAD_VERFILE2SPECBRD_TESTCODE  (0x00030002)

/* 查询指定单板正在运行的软件版本详细信息 */
#define CPS__OAMS_DIAG_SMSS_QRY_SPECBRD_SW_RUNVERINFO_TESTCODE   (0x00030003)

/* 查询单板硬件版本详细信息 */
#define CPS__OAMS_DIAG_SMSS_QRY_SPECCPU_HW_VERINFO_TESTCODE      (0x00030004)

/* 查询单板生产制造信息 */
#define CPS__OAMS_DIAG_SMSS_QRY_SPECBRD_MANUINFO_TESTCODE        (0x00030005)

typedef struct
{
    UINT8  ucVerType;       /* 版本类型 */
    UINT8  ucFuncType;      /* 功能类型 */
    UINT8  ucLogicUnitNo;   /* 逻辑单元号 */

    /*运行状态，0-正常，1-已修改（新的配置版本中该项已修改，但尚未激活） */
    UINT8  ucState;         /* 运行状态 */
    CHAR   acRunVerFileName[CPS__OAMS_DIAG_FILENAME_MAX_LEN];/* 版本文件名 */
} CPS__OAMS_DIAG_SMSS_RUNVERINFO_T;

/* 查询指定单板正在运行的软件版本详细信息 */
typedef struct
{
    UINT32  ulVerInfoNum;       /* 版本信息列表成员个数 */
    CPS__OAMS_DIAG_SMSS_RUNVERINFO_T  astVerInfo[1];  /* 版本信息列表 */
} CPS__OAMS_DIAG_SMSS_QRY_SPECBRD_SW_RUNVERINFO_RESULT_T;

/* 测试报告类型 */
#define CPS__OAMS_DIAG_RPT_TYPE_MIDDLE           (0)   /* 中间报告 */
#define CPS__OAMS_DIAG_RPT_TYPE_END              (1)   /* 最终报告 */
#define CPS__OAMS_DIAG_RPT_TYPE_MIDDLE_EX        (2)   /* 中间报告(不需要解析) */

/* 物理板信息 */
typedef struct
{
    UINT32  ulPhyBrdType;   /* 物理板类型 */
    UINT32  ulPhyBrdVer;    /* 物理板版本号，即EEPROM中的版本号 */
    UINT32  ulBrdPcbVer;    /* PCB版本号 */
} CPS__OAMS_DIAG_SMSS_PHYBRD_INFO_T;

/* SMSS子系统 CPU上逻辑版本数量 */
#define CPS__OAMS_DIAG_CPU_LOGICVER_CNT              (11)

/* 查询单板硬件版本详细信息上报消息中参数区结构 */
typedef struct
{
    /* 主CPU上逻辑版本信息列表,
    * 数组中0、1为FPGA版本;
    * 数组中2－6为CPLD版本;
    * 0xFFFFFFFF:表示不存在，其它：有效。
    */
    UINT32  aulHostBrdLogicVer[CPS__OAMS_DIAG_CPU_LOGICVER_CNT];

    /* 从CPU上逻辑版本信息列表,
    * 数组中0、1为FPGA版本;
    * 数组中2－6为CPLD版本;
    * 0xFFFFFFFF:表示不存在，其它：有效。
    */
    UINT32  aulSlaveBrdLogicVer[CPS__OAMS_DIAG_CPU_LOGICVER_CNT];

    UINT32  ulPhyBrdNum;                        /* 物理板个数 */
    CPS__OAMS_DIAG_SMSS_PHYBRD_INFO_T  astPhyBrd[1]; /* 物理板信息 */
} CPS__OAMS_DIAG_SMSS_QRY_SPECBRD_HW_VERINFO_RESULT_T;

/* 硬件子卡最大的数量 */
#define DRV_MAX_CHILD_BRD_NUM  4

/*硬件单板PCB版本信息由如下字段构成, 结构定义在驱动公共定义的头文件中。*/
typedef struct
{
    UINT32 ulPcbVer;      /* PCB版本 */
}DRV_PHYBRD_INFO_PCB_T;
/*板卡的PCB信息数据结构。*/
typedef struct
{
    DRV_PHYBRD_INFO_PCB_T  stMotherBrd;
    DRV_PHYBRD_INFO_PCB_T stBackBrd;
    UINT32 ulChildBrdNum;
    DRV_PHYBRD_INFO_PCB_T astChildBrd[DRV_MAX_CHILD_BRD_NUM];
}DRV_PHYBRD_INTEGRATED_INFO_PCB_T;

/* SMSS子系统 透传drv参数的buff大小 */
#define CPS__OAMS_DIAG_PHYBRD_MANUINFO_SIZE          (512)

/* SMSS子系统 子卡最大数量 */
#define CPS__OAMS_DIAG_MAX_CHILDBRD_NUM              (4)

/* 物理板信息 */
typedef struct
{
    /* 物理板生产制作信息，
    *  具体格式参见 Q.DTM.SJ.010-2007 EEPROM格式和定义规范(RNC)V1.1.0
    */
    UINT8 aucBuf[CPS__OAMS_DIAG_PHYBRD_MANUINFO_SIZE]; /* 物理板生产制作信息 */
} CPS__OAMS_DIAG_SMSS_PHYBRD_MANUINFO_T;

/* 查询单板生产制造信息上报消息中参数区结构 */
typedef struct
{
    /* 母板（前插板） */
    CPS__OAMS_DIAG_SMSS_PHYBRD_MANUINFO_T stMotherBrd;

    /* 后插板是否在线标志， 0：不在线，1：在线 */
    UINT32  ulBackCardPowerFlag;
    /* 后插板，当后插卡不在线时，内容无效 */
    CPS__OAMS_DIAG_SMSS_PHYBRD_MANUINFO_T stBackBrd;

    UINT32  ulChildBrdNum; /* 子卡数量，为0时后续参数无效 */
    /* 子卡信息 */
    CPS__OAMS_DIAG_SMSS_PHYBRD_MANUINFO_T astChildBrd[CPS__OAMS_DIAG_MAX_CHILDBRD_NUM];

} CPS__OAMS_DIAG_SMSS_QRY_SPECBRD_MANUINFO_RESULT_T;

/* 硬件子卡最大的数量 */
#define DRV_MAX_CHILD_BRD_NUM  4

#define DRV_PHYBRD_MANU_INFO_SIZE   512

/* 板卡的硬件板卡生产制造信息数据结构,仅适用于前插卡*/
typedef struct
{
#if 0
    UINT32  ulBatchSn;          /* 批次序列号 */
    UINT32  ulBarCode;          /* 批次内条码???*/
    UINT32  ulManufacturer;     /* 制造厂家编码 */
    UINT16  ulManuYear;         /* 制造时间_年 */
    UINT8   ulManuMonth;        /* 制造时间_月 */
    UINT8   ulManuDay;          /* 制造时间_日 */
    UINT16  usRepairYear;        /* 返修时间_年 */
    UINT8   ucRepairMonth;        /* 返修时间_月 */
    UINT8   ucRepairDay;          /* 返修时间_日 */
    UINT32  ulEquipmentManufacturer; /* 设备制造厂家编码 */
    UINT8   aucReserved[16];    /* 保留 */
#endif
    UINT8 aucBuf[DRV_PHYBRD_MANU_INFO_SIZE];
}DRV_PHYBRD_INFO_MANU_T;

typedef struct
{
    DRV_PHYBRD_INFO_MANU_T   stMotherBrd;  /* 母板（前插板） */
    UINT32  ulBackBrdPowerFlag;      /* 后插板是否在线标志
                DRV_BACKBRD_POWEROFF－不在线 DRV_BACKBRD_POWERON－在线 */
    DRV_PHYBRD_INFO_MANU_T   stBackBrd;    /* 后插板 */
    UINT32  ulChildBrdNum;
    DRV_PHYBRD_INFO_MANU_T   astChildBrd[DRV_MAX_CHILD_BRD_NUM];/*后插板*/
} DRV_PHYBRD_INTEGRATED_INFO_MANU_T;

/* 启动测试活动响应消息结构 */
typedef struct
{
    CPS__OAMS_M_RSP_HDR_T  stRspHeader;  /* 响应消息头部 */
    UINT32            ulTestId;     /* 测试活动ID */
} CPS__OAMS_DIAG_START_TEST_RSP_MSG_T;

/* 重新下载文件到指定单板请求消息中参数区结构 */
typedef struct
{
    CHAR         acFileName[CPS__OAMS_DIAG_FILENAME_MAX_LEN]; /* 版本文件名 */
    SWP_MO_ID_T  stMoId;                                 /* 版本文件MO_ID */
} CPS__OAMS_DIAG_SMSS_REDOWNLOAD_VERFILE2SPECBRD_REQ_T;

/*end in smss_verm.c*/

/*in smss_verm_fdl.c*/

/* (M接口)FTP服务器信息 */
typedef struct
{
    BOOL    bIsValid;                   /* 是否有效(OAMS内部使用) */
    CPSS_COM_LOGIC_ADDR_T stLogAddr;    /* 设置该FTP服务器信息的管理站逻辑地址 */
    UINT32  ulCryptType;                /* 加密类型，目前固定为0（不加密）*/
    UINT32  ulFtpSvrType;               /* FTP服务器的类型 */
    UINT32  ulFtpSvrIp;                 /* FTP服务器的IP地址 */
    CHAR    acUserName[CPSS_FTP_USERNAME_LEN];  /* 用户名 */
    CHAR    acPassWord[CPSS_FTP_PASSWORD_LEN];  /* 密码 */
    CHAR    acFileDir[CPSS_FS_FILEDIR_LEN];     /* 文件相对路径 */
}CPS__OAMS_COMM_FTP_SERVER_INFO_T;

/* FTP服务器的类型 */
#define CPS__OAMS_COMM_FTPSVR_TYPE_VER       (0)     /* 软件版本文件服务器 */

/*end in smss_verm_fdl.c*/

/*in smss_sysctl.c*/

#define CPS__OAMS_MOID_TYPE_CPU              (2)  /* 所在处理器(含DSP) */
#define CPS__OAMS_MOID_TYPE_SLAVE_CPU        (4)  /* 所在单板的从处理器 */

/*end in smss_sysctl.c*/

/*in smss_sysctl_gcpa.c*/

#define CPS__OAMS_DYND_FORCE_RST_REQ_MSG             (0x40040200)    /* 强制复位 */
#define CPS__OAMS_DYND_FORCE_RST_RSP_MSG             (0x40040201)
#define CPS__OAMS_DYND_FORCE_HO_REQ_MSG              (0x40040100)    /* 强制倒换 */
#define CPS__OAMS_DYND_FORCE_HO_RSP_MSG              (0x40040101)

typedef CPS__OAMS_M_RSP_HDR_T CPS__OAMS_DYND_FORCE_RST_RSP_MSG_T;

/*end in smss_sysctl_gcpa.c*/

/*in cpss_dbg_diagn.h*/

/* CPSS子系统 绝对路径名称长度 */
#define CPS__OAMS_DIAG_DIRNAME_MAX_LEN               (256)

/*end in cpss_dbg_diagn.h*/

/*in smss_devm.c*/

#define CPS__OAMS_DYND_CPLD_VERDLOAD_REQ_MSG             (0x40041400)/* 下载CPLD版本文件 */
#define CPS__OAMS_DYND_CPLD_VERDLOAD_RSP_MSG             (0x40041401)
#define CPS__OAMS_DYND_CPLD_UPDATE_REQ_MSG               (0x40041500)/* 升级CPLD版本文件 */
#define CPS__OAMS_DYND_CPLD_UPDATE_RSP_MSG               (0x40041501)
#define CPS__OAMS_DYND_CPLD_DELVERFILE_REQ_MSG           (0x40041600)/* 删除CPLD版本文件 */
#define CPS__OAMS_DYND_CPLD_DELVERFILE_RSP_MSG           (0x40041601)
#define CPS__OAMS_DYND_IPMC_QUERY_UPDATESTATE_REQ_MSG    (0x40041900)/* IPMC升级状态查询*/
#define CPS__OAMS_DYND_IPMC_CANCELUPDATE_REQ_MSG     (0x40041300)    /* IPMC软件升级,取消升级操作 */

/* 升级的IPMC最大数目 */
#define CPS__OAMS_DYND_IPMC_MAX_COUNT           (19)
#define CPS__OAMS_DYND_IPMC_UPDATE_RSP_MSG           (0x40041001)
#define CPS__OAMS_DYND_IPMC_UPDATE_IND_MSG           (0x40041106)    /* IPMC软件升级,升级结果指示 */
#define CPS__OAMS_DYND_IPMC_DELVERFILE_RSP_MSG       (0x40041201)
#define CPS__OAMS_DYND_IPMC_CANCELUPDATE_RSP_MSG     (0x40041301)
#define CPS__OAMS_DYND_IPMC_VERDLOAD_REQ_MSG         (0x40040F00)    /* IPMC软件升级,软件下载 */
#define CPS__OAMS_DYND_IPMC_VERDLOAD_RSP_MSG         (0x40040F01)
#define CPS__OAMS_DYND_IPMC_UPDATE_REQ_MSG           (0x40041000)    /* IPMC软件升级,升级操作 */
#define CPS__OAMS_DYND_IPMC_QUERY_UPDATESTATE_RSP_MSG    (0x40041901)
#define CPS__OAMS_DYND_IPMC_DELVERFILE_REQ_MSG       (0x40041200)    /* IPMC软件升级,删除文件操作 */
#define CPS__OAMS_DYND_CPLD_QUERY_UPDATESTATE_REQ_MSG    (0x40041700)/* 查询CPLD版本升级状态 */
#define CPS__OAMS_DYND_CPLD_QUERY_UPDATESTATE_RSP_MSG    (0x40041701)

/* IPMC版本文件下载 */
/* 请求消息 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T          stReqHeader;        /* 请求消息头部 */
    CPSS_COM_PHY_ADDR_T       stPhyAddr;          /* 提供升级的单板主CPU物理地址 */
    UINT32                    ulFtpSvrIp;         /* FTP服务器的IP地址 */
    CHAR    acUserName[CPSS_FTP_USERNAME_LEN];    /* 用户名 */
    CHAR    acPassWord[CPSS_FTP_PASSWORD_LEN];    /* 密码 */
    CHAR    acIPMCFileName[CPSS_FS_FILEDIR_LEN];  /* 远端，带相对路径的文件名 */
    UINT32  ulIpmcFileLen;                        /* IPMC版本文件大小，单位：字节 */
    UINT32  ulDataCheckSum;                       /* 数据校验和 */
}CPS__OAMS_DYND_IPMC_VERDLOAD_REQ_MSG_T;

/* 响应消息 */
typedef CPS__OAMS_M_RSP_HDR_T CPS__OAMS_DYND_IPMC_VERDLOAD_RSP_MSG_T;

/* IPMC版本文件升级 */
/* 请求消息 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T          stReqHeader;        /* 请求消息头部 */
    CPSS_COM_PID_T            stLdtPid;           /* LDT纤程 */
    CPSS_COM_PHY_ADDR_T       stPhyAddr;          /* 提供升级的单板主CPU物理地址 */

    /* 待升级的IPMC数目 */
    UINT8  ucIpmcCnt;

    /* 升级的目标IPMC编号列表，待升级的IPMC号列表从数组下标0开始依次填写,
    *  IPMC编号规则：
    *  对于单板，单板槽号(1~14);
    *  对于风机，最上边的风机从左到右依次为15、16、17，下面的风机为18
    */
    UINT8  aucIpmcId[CPS__OAMS_DYND_IPMC_MAX_COUNT];
}CPS__OAMS_DYND_IPMC_UPDATE_REQ_MSG_T;

/* 响应消息 */
typedef CPS__OAMS_M_RSP_HDR_T CPS__OAMS_DYND_IPMC_UPDATE_RSP_MSG_T;

/* 指示消息 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T    stReqHeader;  /* 请求消息头部 */
    CPSS_COM_PID_T      stLdtPid;     /* LDT纤程 */
    UINT8               ucIpmcId;     /* IPMC编号 */
    UINT8               aucRsv[3];    /* 保留 */
    UINT32              ulRetCode;    /* 升级结果返回码，0：成功，其他：错误码 */
}CPS__OAMS_DYND_IPMC_UPDATE_IND_MSG_T;

/* IPMC版本文件删除 */
/* 请求消息 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T          stReqHeader;        /* 请求消息头部 */
    CPSS_COM_PHY_ADDR_T       stPhyAddr;          /* 提供升级的单板主CPU物理地址 */
}CPS__OAMS_DYND_IPMC_DELVERFILE_REQ_MSG_T;

/* 响应消息 */
typedef CPS__OAMS_M_RSP_HDR_T CPS__OAMS_DYND_IPMC_DELVERFILE_RSP_MSG_T;

/* IPMC版本文件取消升级 */
/* 请求消息 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T          stReqHeader;        /* 请求消息头部 */
    CPSS_COM_PHY_ADDR_T       stPhyAddr;          /* 提供升级的单板主CPU物理地址 */
}CPS__OAMS_DYND_IPMC_CANCELUPDATE_REQ_MSG_T;

/* 响应消息 */
typedef CPS__OAMS_M_RSP_HDR_T CPS__OAMS_DYND_IPMC_CANCELUPDATE_RSP_MSG_T;

/* 下载CPLD版本文件  */
/* 请求消息 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T stReqHeader;               /* 请求消息头部 */
    CPS__OAMS_MO_ID_T     stTargetMoId;              /* 待升级单板主CPU的MOID */
    UINT32           ulFtpSvrIp;                        /* FTP服务器的IP地址 */
    CHAR             acUserName[CPSS_FTP_USERNAME_LEN]; /* 用户名 */
    CHAR             acPassWord[CPSS_FTP_PASSWORD_LEN]; /* 密码 */
    CHAR             acFileName[CPSS_FS_FILEDIR_LEN];   /* 带相对路径的文件名 */
} CPS__OAMS_DYND_CPLD_VERDLOAD_REQ_MSG_T;

/* 响应消息 */
typedef CPS__OAMS_M_RSP_HDR_T CPS__OAMS_DYND_CPLD_VERDLOAD_RSP_MSG_T;

/* 升级CPLD版本文件  */
/* 请求消息 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T stReqHeader;               /* 请求消息头部 */
    CPS__OAMS_MO_ID_T     stTargetMoId;              /* 待升级单板主CPU的MOID */
} CPS__OAMS_DYND_CPLD_UPDATE_REQ_MSG_T;

/* 响应消息 */
typedef CPS__OAMS_M_RSP_HDR_T CPS__OAMS_DYND_CPLD_UPDATE_RSP_MSG_T;

/* 删除CPLD版本文件  */
/* 请求消息 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T stReqHeader;               /* 请求消息头部 */
    CPS__OAMS_MO_ID_T     stTargetMoId;              /* 待升级单板主CPU的MOID */
    CHAR             acFileName[CPSS_FS_FILEDIR_LEN]; /* 64字节 */
} CPS__OAMS_DYND_CPLD_DELVERFILE_REQ_MSG_T;

/* 响应消息 */
typedef CPS__OAMS_M_RSP_HDR_T CPS__OAMS_DYND_CPLD_DELVERFILE_RSP_MSG_T;

/* 查询CPLD版本升级状态  */
/* 请求消息 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T stReqHeader;               /* 请求消息头部 */
    CPS__OAMS_MO_ID_T     stTargetMoId;              /* 待升级单板主CPU的MOID */
} CPS__OAMS_DYND_CPLD_QUERY_UPDATESTATE_REQ_MSG_T;

/* 响应消息 */
typedef struct
{
    CPS__OAMS_M_RSP_HDR_T stRspHeader; /* 响应消息头部 */
    UINT32 ulState; /* CPLD升级状态(0:空闲;1:下载升级文件中;2:升级文件下载完成;) */
} CPS__OAMS_DYND_CPLD_QUERY_UPDATESTATE_RSP_MSG_T;

/* IPMC版本文件升级状态查询 */
/* 请求消息 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T          stReqHeader;    /* 请求消息头部 */
    CPSS_COM_PHY_ADDR_T       stPhyAddr;      /* 提供升级的单板主CPU物理地址 */
} CPS__OAMS_DYND_IPMC_QUERY_UPDATESTATE_REQ_MSG_T;

/* 升级目标ID及升级结果 */
typedef struct
{
    UINT8  ucIpmcId;     /* IPMCID；255－表示无效 */
    UINT8  aucResv[3];   /* 保留 */
    UINT32 ulResult;     /* 升级结果，0－成功；0xffffffff－未升级；其他错误码 */
} CPS__OAMS_DYND_IPMC_UPDATE_STATE_T;

/* 响应消息 */
typedef struct
{
     CPS__OAMS_M_RSP_HDR_T stRspHeader;              /* 响应消息头部 */

     /* 升级状态：0-空闲; 1-下载升级文件中;
     * 2-升级就绪; 3-正在升级;
     */
     UINT8 ucUpdState;                           /* 升级状态 */
     UINT8 ucIpmcCount;                          /* 本次需要升级的IPMC总数目 */

     /* 正在升级的IPMCID；
     *  当ucIpmcCount＝0时,该参数取0xff,表示无意义。
     *  当升级状态取值为0－2时,该参数取0xff,表示无意义。
     */
     UINT8 ucCurrentIpmcId;                     /* 正在升级的IPMCID */
     UINT8 ucResv;                              /* 保留 */

     /* 升级IPMCID及升级结果，当ucIpmcCount=0时，该参数无意义。*/
     CPS__OAMS_DYND_IPMC_UPDATE_STATE_T astUpdResult[CPS__OAMS_DYND_IPMC_MAX_COUNT];
} CPS__OAMS_DYND_IPMC_QUERY_UPDATESTATE_RSP_MSG_T;

/*end in smss_devm.c*/

/*in cpss_dbg_diagn.c*/

/* 时变量到计数器组映射关系表项 */
typedef struct
{
    UINT32  ulCalcMethod;        /* 计算方法 */
    UINT32  ulCalcSourceIdx;     /* 计算源时变量的下标(首个时变量下标为0) */
} CPS__OAMS_PM_TMVAR_TO_CNTGRP_MAP_T;

#define CPS__OAMS_PM_TMVAR_TO_CNTGRP_CALC_AVG        (1) /* 取平均值 */
#define CPS__OAMS_PM_TMVAR_TO_CNTGRP_CALC_MAX        (2) /* 取最大值 */
#define CPS__OAMS_PM_TMVAR_TO_CNTGRP_CALC_INVALID    (0xff) /* 结束标记 */

/* DSP处理器占用率查询 */
#define CPS__OAMS_DIAG_CPSS_GET_DSP_USAGE_TESTCODE           (0x00020009)

/* DSP内存池信息查询 */
#define CPS__OAMS_DIAG_CPSS_GET_DSP_MEMPOOL_USAGE_TESTCODE   (0x0002000a)

/* 控制面板间连通性测试 */
#define CPS__OAMS_DIAG_CPSS_CTRLP_COMM_TEST_TESTCODE         (0x00020004)

/* 通用处理器内存使用查询 */
#define CPS__OAMS_DIAG_CPSS_GET_CPU_MEM_USAGE_TESTCODE       (0x00020007)

/* DSP内存使用查询 */
#define CPS__OAMS_DIAG_CPSS_GET_DSP_MEM_USAGE_TESTCODE       (0x00020008)

/* 停止测试活动指示消息结构 */
/* 测试代理 -> 子系统  或  子系统 -> 测试代理 */
typedef struct
{
    CPS__OAMS_M_REQ_HDR_T  stReqHeader;   /* 请求消息头部 */
    UINT32            ulTestId;      /* 测试活动ID */
    UINT32            ulStopReason;  /* 测试活动终止原因 */
} CPS__OAMS_DIAG_STOP_TEST_IND_MSG_T;

/*end in cpss_dbg_diagn.c*/

/*in cpss_com_link.c*/

/* 控制面板间连通性测试上报消息中参数区结构 */
typedef struct
{
    UINT32  ulTestSeqId;    /* 测试序号 */
    UINT32  ulTestLen;      /* 测试字符串长度 */
    UINT32  ulResult;       /* 测试结果，0：成功，非0不通 */
} CPS__OAMS_DIAG_CPSS_CTRLP_COMM_TEST_RESULT_T;

/*end in cpss_com_link.c*/
#endif /* CPS__OAMS_PUBLIC_H_ */
