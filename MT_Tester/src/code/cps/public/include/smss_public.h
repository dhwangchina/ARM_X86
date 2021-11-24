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
#ifndef SMSS_PUBLIC_H
#define SMSS_PUBLIC_H

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_ABOX) 
#include "cps_drv_public.h"
#endif

/********************************* 头文件包含 ********************************/

/******************************** 宏和常量定义 *******************************/

/* 操作结果（用于消息参数或函数返回值，类型为INT32） */
#define SMSS_OK               0 /* 成功 */
#define SMSS_ERROR           -1 /* 失败 */

/* 单板主备状态宏定义 */
#define SMSS_ACTIVE        0x00 /* 主用 */
#define SMSS_STANDBY       0x01 /* 备用 */

/* 纤程定义 */

#define SMSS_BOOT_PROC          CPSS_VK_PD(24,0)  /* BOOT */
#define SMSS_TEST_PROC          CPSS_VK_PD(25,0)  /* 测试 */

/* 消息定义 */
#define SMSS_PROC_ACTIVATE_REQ_MSG              (0x23000000)
#define SMSS_PROC_ACTIVATE_RSP_MSG              (0x23000001)
#define SMSS_PROC_ACTIVATE_COMPLETE_IND_MSG     (0x23000002)
#define SMSS_STANDBY_TO_ACTIVE_REQ_MSG          (0x23000010)
#define SMSS_STANDBY_TO_ACTIVE_RSP_MSG          (0x23000011)
#define SMSS_STANDBY_TO_ACTIVE_COMPLETE_IND_MSG (0x23000012)
#define SMSS_ACTIVE_TO_STANDBY_IND_MSG          (0x23000022)

/* 该接口在G3接口中未添加，需要增加 */
#define SMSS_ACTIVE_PORT_CHG_IND_MSG            (0x23000027)
#define SMSS_ABOX_UPDATE_SWVER_COMMAND_IND_MSG   (0x23000024)
#define SMSS_ABOX_UPDATE_SWVER_RESULT_IND_MSG    (0x23000025)


/* 该接口在G3接口中未添加，需要增加 */
#define SMSS_CPU_FAULT_IND_MSG                    (0x23000026)

/* 子系统上报到SMSS子系统的本板故障通知消息 */
#define SMSS_BOARDSELF_FAULT_IND_MSG              (0x23000040)
/* SMSS子系统通知其他子系统备用板故障指示消息 */
#define SMSS_STANDBY_FAULT_IND_MSG                (0x23000023)

/* 在网元升级过程中，消息ID宏定义 */
#define SMSS_UPGRADE_DATA_REQ_MSG   (0x23000030)
#define SMSS_UPGRADE_DATA_RSP_MSG    (0x23000031)

#define CPS_CPU_STAT_CHG_ID_MSG     (0x23000051)

typedef struct
{
	CPSS_COM_LOGIC_ADDR_T stLogicAddr;
	UINT32 iState;/*0:正常 其它:故障*/
}CPS_CPU_STAT_CHG_IND_MSG;

/* 网元整体升级过程中，本板升级状态（仅用于全局板）的宏定义 */
#define SMSS_UPGSTATUS_NORMAL     0  /* 未升级，正常运行中 */
#define SMSS_UPGSTATUS_UPGRADING  1  /* 升级中 */

/* 事件编号宏定义 */
#define SMSS_EVENTNO_VER_DOWNLOAD_BEGIN   0x00030006    /* 版本文件下载开始 */
#define SMSS_EVENTNO_VER_DOWNLOAD_END     0x00030007    /* 版本文件下载结束 */

/******************************** 类型定义 ***********************************/
/* 纤程激活请求 */
typedef struct
{
    UINT8  ucAsStatus;          /* 单板的主备状态 (active/standby)
                                   SMSS_ACTIVE：  主用
                                   SMSS_STANDBY： 备用*/
} SMSS_PROC_ACTIVATE_REQ_MSG_T;

/* 纤程激活响应 */
typedef struct
{
    UINT32  ulResult;           /* 激活结果
                                   SMSS_OK：成功，
                                   其它：   失败 */
} SMSS_PROC_ACTIVATE_RSP_MSG_T;

/* 纤程备用转主用响应 */
typedef struct
{
    UINT32  ulResult;           /* 备用转主用结果
                                   SMSS_OK：成功，
                                   其它：   失败 */
} SMSS_STANDBY_TO_ACTIVE_RSP_MSG_T;

/* 单板倒换禁止/禁止解除函数（smss_board_switch_forbid）调用者 */
#define SMSS_BSF_CALLER_A       0
#define SMSS_BSF_CALLER_B       1
#define SMSS_BSF_CALLER_C       2
#define SMSS_BSF_CALLER_D       3
#define SMSS_BSF_CALLER_MAX     3

/* 纤程激活配置 */
typedef struct
{
    UINT32 ulProcDes;    /* 纤程描述符*/
    UINT32 ulWaitTime;   /* 等待纤程激活响应的时间，单位ms，
                            设为0，表示可立即向后续纤程发送激活请求消息，
                            不为0，表示需要等到该纤程的激活成功响应，才能
                            向后续纤程发送激活请求消息。
                            如超时，初始化失败。
                         */
} SMSS_PROC_TABLE_ITEM_T;

/* 版本文件头部 */
typedef struct
{
    CHAR        aucHeadWord[8];     /* 文件校验字, 固定内容"dtswpver" */
    UINT32      ulHeadLen;          /* 含本字段在内的头部字节长度, 目前固定为32 */
    UINT32      ulHeadVer;          /* 头部信息版本号, 目前固定为0 */
    UINT16      usNeType;           /* 网元类型 */
    UINT16      usPhyBrdType;       /* 物理单板类型(前插板) */
    UINT16      usFunBrdType;       /* 功能单板类型 */
    UINT16      usVerType;          /* 版本类型(主CPU/从CPU/FPGA/DSP/ME) */
    UINT32      ulVerNo;            /* 版本编号, 按字节为00.xx.yy.zz格式 */
    UINT16      usBuildYear;        /* 编译时间(年) */
    UINT8       ucBuildMonth;       /* 编译时间(月) */
    UINT8       ucBuildDay;         /* 编译时间(日) */
    UINT32      ulVerDataCheckSum;  /* 版本数据校验和 */
    UINT32      ulVerFileLen;       /* 版本文件长度(含头部, 单位:字节) */
} SWP_VER_FILE_HEAD_T;

typedef struct
{
    UINT32  ulActivePort;/* 激活结果
                    DRV_IPHA_PORT_0：端口0，对应7槽masa，
                    DRV_IPHA_PORT_1：端口1，对应8槽masa */
} SMSS_BOARD_MEDIA_ACTIVE_PORT_SET;

typedef struct
{
    UINT32  ulReason;                      /* 使用子系统定义的错误码 */
} SMSS_CPU_FAULT_IND_MSG_T;


#ifndef SMSS_ABOX_VER_FILENAME_LEN
#define SMSS_ABOX_VER_FILENAME_LEN 256
#endif

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_ABOX) 
typedef DRV_PHYBRD_INTEGRATED_INFO_MANU_T SMSS_ABOX_MANU_INFO_T;
#endif

/* 告警箱变更设计修改  oams查询告警箱路由信息结构体 */
typedef struct
{
	UINT32 ulIfRouteSwitch; /* 是否需要路由切换功能 0-不需要,1-需要 */
	UINT8 aucDesIp1[256]; /* 目的IP地址1,用字符串的形式表示IP地址 */
	UINT8 aucGateWay1[256];
	UINT8 aucDesIp2[256]; /* 目的IP地址2,用字符串的形式表示IP地址 */
	UINT8 aucGateWay2[256];
} SMSS_ABOX_ROUTE_QUERY_T;

typedef struct
{
    CHAR  acRunVerFileName[SMSS_ABOX_VER_FILENAME_LEN];  /* 版本文件名 */
} SMSS_ABOX_VER_INFO_T;

/* 全局板上的SMSS通知本板的RDBS升级数据文件请求消息体 */
typedef struct
{
    CHAR acDataFileName[CPSS_FS_FILEDIR_LEN]; /* 数据文件名，具体要求见请参考
	                                          《统一软件平台OAMS子系统接口手册(M接口)》 */
} SMSS_UPGRADE_DATA_REQ_MSG_T;

/* 全局板上的RDBS在数据文件升级完成后，向本板的SMSS发送升级数据文件响应消息体 */
typedef struct
{
    UINT32  ulResult; 	   /* 升级结果
                                   SMSS_OK：成功，
                                   其它：失败 */
} SMSS_UPGRADE_DATA_RSP_MSG_T;

typedef struct
{
    UINT32  ulCause; 	   /* 故障原因,各
                            * 子系统自己定义 
                            */
} SMSS_BOARDSELF_FAULT_IND_MSG_T;
/******************************** 全局变量声明 *******************************/
   
/* 补丁文件头结构定义 */
#define CPSS_FUNC_NAME_LEN  64         /* 函数名长度(单位:字节) */
#define CPSS_FUNC_NUM_MAX   32         /* 一个补丁文件中最多修复的函数个数 */
#define CPSS_PATCH_SAVE_BYTES 16  /* 保存原函数入口处的内容的字节数 */

typedef struct
{
    CHAR aucOld[CPSS_FUNC_NAME_LEN];     /* 待修复缺陷的旧函数名 */
    CHAR aucNew[CPSS_FUNC_NAME_LEN];     /* 已修复缺陷的新函数名 */
} SWP_PATCH_FUNC;

/* 补丁文件头部结构定义，在版本文件头部的基础上修改得到。
紫色表示新增，蓝色表示取值改变，黑色与版本文件头部完全一致。 */
typedef struct
{
    CHAR        aucHeadWord[8];     /* 文件校验字, 固定内容"swppatch" */
    UINT32      ulHeadLen;          /* 含本字段在内的头部字节长度，长度为 */
    UINT32      ulHeadVer;          /* 头部信息版本号, 目前固定为0 */
    UINT16      usNeType;           /* 网元类型 */
    UINT16      usPhyBrdType;       /* 物理单板类型(前插板) */
    UINT16      usFunBrdType;       /* 功能单板类型 */
    UINT16      usVerType;          /* 版本类型(主CPU/从CPU) */
    UINT32      ulVerNo;            /* 版本编号, 按字节为00.xx.yy.zz格式 */
    UINT16      usBuildYear;        /* 编译时间(年) */
    UINT8       ucBuildMonth;       /* 编译时间(月) */
    UINT8       ucBuildDay;         /* 编译时间(日) */
    UINT32      ulDataCheckSum;   /* 版本数据校验和 */
    UINT32      ulFileLen;         /* 版本文件长度(含头部, 单位:字节) */
	UINT32      ulPatchNo;          /* 补丁号 */
	CHAR        aucTaskName[32];   /* 补丁对应的任务名 */
    UINT32      ulFuncNum;         /* 修复的函数个数，不超过32个 */
    SWP_PATCH_FUNC astFunc[CPSS_FUNC_NUM_MAX];   /* 修复的函数名称信息 */
} SWP_PATCH_FILE_HEAD_T;

#define CPS_CPU_STAT_CHG_IND_MSG 0x23000051

typedef struct
{
CPSS_COM_LOGIC_ADDR_T stLogicAddr;
INT32 iState; 	/* 0：正常 其它：故障*/
}CPS_CPU_STAT_CHG_IND_MSG_T;

/******************************** 全局变量声明 *******************************/   
extern SMSS_PROC_TABLE_ITEM_T g_astSmssProcTable[];
extern SMSS_PROC_TABLE_ITEM_T g_astSmssProcTableForUser[];

/******************************** 外部函数原形声明 ***************************/
extern INT32 smss_board_switch_forbid(BOOL bForbidden);
extern BOOL  smss_mate_in_service(VOID);
extern UINT32 smss_devm_drv_init(VOID);

/* 纤程入口 */
extern VOID smss_sysctl_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID smss_verm_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID smss_devm_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);
extern VOID smss_test_proc(UINT16 usUserState, VOID *pvVar, CPSS_COM_MSG_HEAD_T *pstMsgHead);

/* 公共函数声明 */
BOOL smss_verm_buf_checksum(VOID *vpBuf, UINT32 ulBufSize, UINT32 *pulCheckSum);
BOOL smss_verm_file_verify(STRING szFileName, UINT32 ulFileLen, UINT32 ulCheckSum);
extern INT32 smss_send_me_status(UINT32 ulMeID,UINT32 ulMeState);

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_ABOX) 
/* 告警箱启用/禁用状态设置、查询，状态取值：0 - 禁用；1 - 启用 */
extern UINT32 smss_abox_mstate_set(UINT32 ulMState);
extern UINT32 smss_abox_mstate_get(UINT32 *pulMState);

/* 告警箱清音状态设置、查询，状态取值：0 - 清音；1 - 清音解除 */
extern UINT32 smss_abox_mute_set(UINT32 ulMute);
extern UINT32 smss_abox_mute_get(UINT32 *pulMute);

/* 告警箱生产制造信息查询 */
extern UINT32 smss_abox_manu_info_get(SMSS_ABOX_MANU_INFO_T *pstManuInfo);

/* 告警箱软件版本信息查询 */
extern UINT32 smss_abox_ver_info_get(SMSS_ABOX_VER_INFO_T *pstVerInfo);

/* 告警箱复位 */
extern VOID smss_abox_reset(VOID);

/* 告警箱检测 */
extern UINT32 smss_abox_test(VOID);  /* 阻塞方式 */

/* 告警箱声音告警 */
extern UINT32 smss_abox_sound_alarm(UINT32 ulSoundLevel); /* 参数取值：0-3 */

/* 告警箱灯光告警 */
extern UINT32 smss_abox_light_alarm(UINT32 ulCritLevel, UINT32 ulMajorLevel, UINT32 ulMinorLevel, UINT32 ulWarnLevel); /* 各参数取值：0-3 */

/* 告警箱通道灯设置 */
extern UINT32 smss_abox_linkstate_set(UINT32 ulLinkState);
extern INT32 smss_sysctl_abox_configure();
/* 告警箱ip设置 */
extern INT32 smss_abox_ip_set(UINT32 ulPortType,UINT8* pucIp1,UINT8* pucMask1,UINT8* pucMacAddr,UINT8* pucIp2,UINT8* pucMask2);
/* 告警箱路由设置 */
extern INT32 smss_abox_route_set(UINT32 ulIfRouteSwitch,UINT8 *pucGateWay1,UINT8 *pucGCPA1,
					   UINT8 *pucGateWay2,UINT8 *pucGCPA2);
extern VOID smss_abox_route_show();
/* 告警箱路由信息查询 */
extern INT32 smss_abox_route_query(SMSS_ABOX_ROUTE_QUERY_T *pstOamsRouteInfo);
#endif

#ifdef NO_RSN  /* 根据测试诊断手册，删除设置和重设置RSN功能 */
extern UINT32 smss_read_rsn(UINT8 aucRsn[16]);
#endif
extern UINT32 smss_write_sensitive_cmd(UINT32 ulStartNum,UINT32 ulTotalNum,UINT32 ulSensitiveCmdLength,UINT32 ulSensitiveInf[]);
extern UINT32 smss_read_sensitive_cmd(UINT32 ulStartNum,UINT32 ulTotalNum,UINT32 ulSensitiveCmdLength,UINT32 ulSensitiveInf[]);
/*  在网元整体升级过程中，全局板上提供用于查询本板当前的网元整体升级状态，本板上其它子系统可以调用，*/
extern UINT32 smss_get_gcpa_upgstatus(VOID);
extern INT32 smss_moid_get_local_moid(UINT32 ulMoIdType, SWP_MO_ID_T *pstOutMoId);
extern INT32 smss_moid_get_moid_by_phy(UINT32 ulMoIdType, 
                                CPSS_COM_PHY_ADDR_T *pstPhyAddr,    
                                SWP_MO_ID_T *pstOutMoId); 
extern BOOL  smss_get_proc_active_state(VOID); 
/* 供CPSS的调度任务中调用。*/
extern UINT32 smss_verm_patch_operation(CHAR *pucTaskName);
extern UINT32 smss_get_slave_status(VOID);

#define CPS_CPU_REBOOT_COLD 1
#define CPS_CPU_REBOOT_HOT 0
#define CPS_REBOOT_BY_OM 0
#define CPS_REBOOT_BY_KW 1
#define CPS_REBOOT_BY_ENTER_END 2
#define CPS_REBOOT_BY_VER_DOWNLOAD 3
#define CPS_REBOOT_BY_SEGFAULT 4
#define CPS_REBOOT_BY_CPM 5

typedef struct _CPS_CPU_REBOOT_
{
	UINT8 ucRebtType;
	UINT8 ucRebtRsn;
	STRING szExtra;
}CPS_REBOOT_T;

#define CPS_EXT_IF_STAT_CHG_IND_MSG (0x23000052)

typedef struct
{
	UINT32 ulIf;   /* 1~10，代表7220上的10个外部接口 */
	UINT32 ulState; /* 0：正常， 其它：故障 */
}CPS_EXT_IF_STAT_CHG_IND_MSG_T;

/*最大上报端口数*/
#define CPS_DEVM_PORT_MAX_NUM 10

/*端口状态结构*/
typedef struct _CPS_DEVM_PORT_STAT_
{
    UINT8 ucSlotId; /*槽号,对应AC设备*/
    UINT8 ucPortId; /*外部端口号 1~6对应sfp, 7~10对应sfp+*/
    UINT8 ucState; /*0-异常 1-正常*/
    UINT8 ucRsv; /*保留字段*/
}CPS_DEVM_PORT_STAT_T;

/*端口状态改变消息结构*/
typedef struct _CPS_DEVM_PORT_STATE_CHG_
{
    UINT32 ulPortNum; /*上报的端口数量*/
    CPS_DEVM_PORT_STAT_T astPortState[CPS_DEVM_PORT_MAX_NUM]; /*端口状态信息*/
}CPS_DEVM_PORT_STATE_CHG_MSG_T;

/*端口状态改变消息*/
#define CPS_DEVM_PORT_STATE_CHG_MSG 0x75071100

extern INT32 cps_verm_get_update_stat();

/******************************** 头文件保护结尾 *****************************/
#endif /* SMSS_PUBLIC_H */
/******************************** 头文件结束 *********************************/
