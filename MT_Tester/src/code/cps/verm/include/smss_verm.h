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
#ifndef SMSS_VERM_H
#define SMSS_VERM_H

/********************************* 头文件包含 ********************************/
#include "cpss_public.h"
#include "cps_replant.h"

/******************************** 宏和常量定义 *******************************/

#if (SWP_FUNBRD_TYPE == SWP_FUNBRD_GCPA)
#define KEY 8964
#elif (SWP_FUNBRD_TYPE == SWP_FUNBRD_RSPA)
#define KEY 6489
#endif

#define CPS_VER_UPDATE_REQ_MSG		0x75025003
#define CPS_VER_UPDATE_RSP_MSG		0x75025004
#define OM_AP_RENEW_VERSION_IND_MSG	0x24030001
#define OM_AC_RENEW_VERSION_IND_MSG	0x24030002

#define CPS_VER_CFG_QUERY_REQ_MSG	0x75025002
#define CPS_VER_CFG_QUERY_RSP_MSG	0x75025005

typedef struct {
	INT32		iResult;			/*结果，0：成功，其它：错误码*/
} CPS_VER_UPDATE_RSP_MSG_T;

#define SMSS_END_CAUSE_HOST_VER_CHG   0x123

/* VERM状态 */
#define SMSS_VERM_STATE_IDLE            CPSS_VK_PROC_USER_STATE_IDLE /* 0 */
#define SMSS_VERM_STATE_IDLE_1          1 /* 仅存在于全局板 */
#define SMSS_VERM_STATE_QUERY           2 /* 仅存在于备用全局板或外围板从CPU */
#define SMSS_VERM_STATE_VERIFY          3
#define SMSS_VERM_STATE_WAIT            4  /* 仅存在于带有从CPU/DSP的外围板主CPU上 */
#define SMSS_VERM_STATE_NORMAL          5
#define SMSS_VERM_STATE_END             6 /* CPU版本更新，需重启 */

/* 消息定义 */
#define SMSS_VERM_FILE_DOWNLOAD_REQ_MSG      (0x75020000)
#define SMSS_VERM_FILE_DOWNLOAD_RSP_MSG      (0x75020001)
#define SMSS_VERM_FILE_DOWNLOAD_END_IND_MSG  (0x75020002)
#define SMSS_VERM_SLAVE_START_IND_MSG        (0x75020008)
#define SMSS_VERM_DSP_START_IND_MSG          (0x75020009)
#define SMSS_VERM_VERINFO_REQ_MSG            (0x75020010)
#define SMSS_VERM_VERINFO_RSP_MSG            (0x75020011)
#define SMSS_VERM_VERINFO_IND_MSG            (0x75020012)
#define SMSS_VERM_VER_FDOWNLOAD_EVT_IND_MSG  (0x75020013)   
#define SMSS_VERM_FDL_DATA_MSG               (0x75020020)
#define SMSS_VERM_FDL_ACK_MSG                (0x75020021)
#define SMSS_VERM_BRD_VERSTATE_CHG_IND_MSG   (0x75020030)

/* 网络整体升级消息体ID宏定义 */
#define SMSS_VERM_GCPA_UPGRADE_IND_MSG       (0x75020031)

/* 用于主用全局板向备用全局板发RDBS下载数据请求，请求消息
 * 中携带单板升级状态(升级中，升级完成)和下载的文件名
 */
#define SMSS_VERM_UPGRADE_DATA_REQ_MSG       (0x75020032)
#define SMSS_VERM_UPGRADE_DATA_RSP_MSG       (0x75020033)
#define SMSS_VERM_VERLIB_CHECK_IND_MSG       (0x75020034)
/* 从CPU向主CPU发下载完成指示,无消息内容 */
#define SMSS_VERM_SLAVE_DOWNLOADEND_IND_MSG  (0x75020035)
/* 在IDLE态用于从CPU VERM向主CPU VERM发送逻辑芯片版本信息 */
#define SMSS_VERM_LOGCHIP_VERINFO_IND_MSG    (0x75020036)
/* 备用MASA初始态通知主用需要下载 */
#define SMSS_VERM_SBYMASA_NEEDDOWN_IND_MSG   (0x75020037) 
/* 备用MASA初始态通知主用下载完成 */
#define SMSS_VERM_SBYMASA_DOWNED_IND_MSG     (0x75020038)
/* 版本下载完成后延迟激活 */
#define SMSS_VERM_MASA_ACTIVEDELAYIND_MSG    (0x75020039)

/* 定时器相关宏定义 */
#define SMSS_VERM_RESEND_TIMER        CPSS_TIMER_00      /* 重发定时器号 */
#define SMSS_VERM_RESEND_TIMER_MSG    CPSS_TIMER_00_MSG  /* 重发定时器超时消息 */
#define SMSS_VERM_RESEND_TIMEOUT      2000               /* 重发定时器时长 */

#define SMSS_VERM_FDL_DELAY_TIMER     CPSS_TIMER_01      /* 下载延迟定时器号 */
#define SMSS_VERM_FDL_DELAY_TIMER_MSG CPSS_TIMER_01_MSG  /* 下载延迟定时器超时消息 */
#define SMSS_VERM_FDL_DELAY_TIMEOUT   60000              /* 下载延迟定时器时长 */
#define SMSS_VERM_DSP_LOAD_TIMER0      CPSS_TIMER_02      /* 分段下载DSP定时器号0 */
#define SMSS_VERM_DSP_LOAD_TIMER_MSG0  CPSS_TIMER_02_MSG  /* 下载延迟定时器超时消息 */
#define SMSS_VERM_DSP_LOAD_TIMEOUT     1000               /* 下载延迟定时器时长 */
#define SMSS_VERM_DSP_LOAD_TIMER1      CPSS_TIMER_03      /* 复位延时DSP定时器号0 */
#define SMSS_VERM_DSP_LOAD_TIMER_MSG1  CPSS_TIMER_03_MSG  /* 复位延迟定时器超时消息 */
#define SMSS_VERM_DSP_LOAD_TIMEOUT1    5000               /* 复位延迟定时器时长 */
#define SMSS_VERM_VERLIB_TIMER      CPSS_TIMER_04      /* 版本库下载延迟定时器号 */
#define SMSS_VERM_VERLIB_TIMER_MSG  CPSS_TIMER_04_MSG  /* 下载延迟定时器超时消息 */
#define SMSS_VERM_VERLIB_TIMEOUT    6000               /* 下载延迟定时器时长 */
#define SMSS_VERM_FDL_TIMER            CPSS_TIMER_05
#define SMSS_VERM_FDL_TIMER_MSG        CPSS_TIMER_05_MSG
#define SMSS_VERM_PATCH_TIMER      CPSS_TIMER_06      /* 补丁链表监测定时器号 */
#define SMSS_VERM_PATCH_TIMER_MSG  CPSS_TIMER_06_MSG  /* 补丁链表定时器超时消息 */
#define SMSS_VERM_PATCH_TIMEOUT    1000               /* 补丁链表定时器时长1s */
/* 在测试诊断中，重新下载版本文件到指定单板上，
或重新下载版本库版本文件到硬盘中的下载超时定时器 */
#define SMSS_VERM_TESTDIAG_FDL_TIMER      CPSS_TIMER_07      /* 测试诊断文件下载时器号 */
#define SMSS_VERM_TESTDIAG_FDL_TIMER_MSG  CPSS_TIMER_07_MSG  /* 测试诊断文件下载超时消息 */
#define SMSS_VERM_TESTDIAG_FDL_TIMEOUT    8000               /* 测试诊断文件下载定时器时长 */
#define SMSS_VERM_BACKUP_TIMER      CPSS_TIMER_08      /* 备份区定时器号 */
#define SMSS_VERM_BACKUP_TIMER_MSG  CPSS_TIMER_08_MSG  /* 备份区定时器超时消息 */
#define SMSS_VERM_BACKUP_TIMEOUT    (15 * 60 * 1000)   /* 备份区定时器时长15分钟 */
#define SMSS_VERM_STATUSIND_TIMER      CPSS_TIMER_09      /* 状态指示定时器号 */
#define SMSS_VERM_STATUSIND_TIMER_MSG  CPSS_TIMER_09_MSG  /* 状态指示定时器超时消息 */
#define SMSS_VERM_STATUSIND_TIMEOUT    (5000)             /* 状态指示定时器时长5s */
/* 错误码宏定义 */
#define ERR_SMSS_SLAVE_FDL_FAILED       1
#define ERR_SMSS_SLAVE_FDL_CHECK_FAILED 2
#define ERR_SMSS_SLAVE_FDL_LEN_FAILED 3

/* 版本比较的阶段 */
#define SMSS_VERM_STAGE_INIT    0  /* 在初始阶段的版本比较，*/
#define SMSS_VERM_STAGE_NORMAL  1  /* 在正常阶段的版本比较 */

/* 告警号定义 */
#define SMSS_VERM     (UINT32)1     /* 版本管理告警号 */

/* 管理对象类型宏定义 */
#define SWP_SMSS    (UINT32)1    /* 系统管理对象 */

#define smss_send_to_gcpa(ulDstPd, ulMsgId, pucData, ulDataLen) \
        { \
            CPSS_COM_PID_T stDstPid; \
            stDstPid.stLogicAddr = CPSS_LOGIC_ADDR_GCPA; \
            stDstPid.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER; \
            stDstPid.ulPd = ulDstPd; \
            smss_test_PrintMsgStream(ulMsgId,(UINT8 *)pucData,ulDataLen,TRUE);\
            cpss_com_send(&stDstPid, ulMsgId, (UINT8 *)pucData, ulDataLen); \
        }
#define smss_send_to_sgcpa(ulDstPd, ulMsgId, pucData, ulDataLen) \
        { \
            smss_test_PrintMsgStream(ulMsgId,(UINT8 *)pucData,ulDataLen,TRUE);\
            cpss_com_send_mate(ulDstPd, ulMsgId, (UINT8 *)pucData, ulDataLen); \
        }
#define smss_send_to_local(ulDstPd, ulMsgId, pucData, ulDataLen) \
        {\
            CPSS_COM_PID_T stDstPid; \
            stDstPid.stLogicAddr = g_stSmssBaseInfo.stLogAddr; \
            stDstPid.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER; \
            stDstPid.ulPd = ulDstPd; \
            smss_test_PrintMsgStream(ulMsgId,(UINT8 *)pucData,ulDataLen,TRUE);\
            cpss_com_send(&stDstPid, ulMsgId, (UINT8 *)pucData, ulDataLen); \
        }
#define smss_send_to_host(ulDstPd, ulMsgId, pucData, ulDataLen) \
        { \
            CPSS_COM_PHY_ADDR_T stPhy = g_stSmssBaseInfo.stPhyAddr; \
            stPhy.ucCpu = 1; \
            smss_test_PrintMsgStream(ulMsgId,(UINT8 *)pucData,ulDataLen,TRUE);\
            cpss_com_send_phy(stPhy, ulDstPd, \
                              ulMsgId, (UINT8 *)pucData, ulDataLen); \
        }
#define smss_send_to_slave(ulDstPd, ulMsgId, pucData, ulDataLen) \
        { \
            CPSS_COM_PID_T stDstPid; \
            stDstPid.stLogicAddr = g_stSmssBaseInfo.stLogAddr; \
            stDstPid.ulAddrFlag = CPSS_COM_ADDRFLAG_MASTER; \
            stDstPid.stLogicAddr.ucSubGroup = 2; \
            stDstPid.ulPd = ulDstPd; \
            smss_test_PrintMsgStream(ulMsgId,(UINT8 *)pucData,ulDataLen,TRUE);\
            cpss_com_send(&stDstPid, ulMsgId, (UINT8 *)pucData, ulDataLen); \
        }
#define smss_verm_state_set(usNewState) \
        { \
            cpss_print(SWP_SUBSYS_SMSS, SMSS_MODULE_VERM, CPSS_PRINT_INFO, \
                       " %8s: Transited to state %s.", \
                       g_szSmssVermStateStr, aszSmssVermState[usNewState]); \
            cpss_vk_proc_user_state_set(usNewState); \
            g_szSmssVermStateStr = aszSmssVermState[usNewState]; \
        }

#define smss_is_active() (SMSS_ACTIVE == g_stSmssAsInfo.ucSelfAStatus)

/******************************** 类型定义 ***********************************/
/* 版本信息请求消息
   在VERM纤程激活流程中，由备用全局板发给主用全局板或由外围板从CPU发给主CPU。*/
typedef CPS__RDBS_DEV_GETCURRVERINFO_MSG_REQ_T SMSS_VERM_VERINFO_REQ_MSG_T;
typedef CPS__RDBS_DEV_GETCURRVERINFO_MSG_REQ_T SMSS_BRD_INFO_T;

/* 版本信息响应消息 */
typedef struct 
{
    UINT32         ulVerInfoNum;        /* 版本信息列表成员个数 */
    CPS__RDBS_VERINFO_T astVerInfo[CPS__RDBS_DEV_VERITEM_MAX];   /* 版本信息列表 */
} SMSS_VERM_VERINFO_RSP_MSG_T;

/* 版本信息指示消息
   在配置版本变更流程中，由主CPU发给从CPU。 */
typedef SMSS_VERM_VERINFO_RSP_MSG_T SMSS_VERM_VERINFO_IND_MSG_T;
typedef SMSS_VERM_VERINFO_RSP_MSG_T SMSS_VERM_VERINFO_T;

/* 网络整体升级消息体结构 */
typedef SMSS_VERM_VERINFO_RSP_MSG_T SMSS_VERM_GCPA_UPGRADE_IND_MSG_T;

/* 以下定义用于CPS__RDBS_VERINFO_T结构中的ucRsv[0]成员。
       在配置版本信息g_stSmssCfgVerInfo中，所有版本文件的初始值设为UNCHANGED；
   需要下载的版本文件在开始下载时设为DOWNLOADING，下载完成后设为DOWNLOADED。
   程序检查是否所有版本文件都为UNCHANGED或DOWNLOADED判断版本下载结束。
       在当前版本信息g_stSmssCurVerInfo中，所有版本文件的初始值设为INVALID；
   无需下载的版本文件设为UNCHANGED；需要更新的版本文件删除后设为DELETED并开始
   下载。在版本比较（smss_verm_ver_compare()）的最后，将所有剩余的INVALID文件
   全部从FLASH文件系统中删除。版本文件下载成功后添加到本结构中，并设置为
   DOWNLOADED。
*/
#define SMSS_VERM_CFGVER_UNCHANGED      7   /* 配置版本文件初始或无需下载状态,即对应RDBS的正常态 */
#define SMSS_VERM_CFGVER_DOWNLOADED     0   /* 配置版本文件下载完成状态,即对应RDBS的正常态 */
#define SMSS_VERM_CFGVER_DOWNLOADING    5   /* 配置版本文件正在下载状态,即对应RDBS的下载中态 */
#define SMSS_VERM_CFGVER_QUEUING        4   /* 配置版本文件下载排队状态,即对应RDBS的下载排队中态 */
#define SMSS_VERM_CFGVER_DOWNLOADFAIL   2   /* 配置版本文件下载失败状态,即对应RDBS的下载失败态 */
#define SMSS_VERM_CFGVER_FDLB_CHECKFAIL 1   /* 配置版本文件下载前校验状态,即对应RDBS的校验失败态 */
#define SMSS_VERM_CFGVER_FDLS_CHECKFAIL 12  /* 配置版本文件下载后校验状态,即对应RDBS的校验失败态 */
#define SMSS_VERM_CURVER_UNCHANGED      3   /* 当前版本文件无需下载状态 */
#define SMSS_VERM_CURVER_DOWNLOADED     6   /* 当前版本文件下载完成状态 */
#define SMSS_VERM_CURVER_LOAD_FAILED    8   /* 当前版本文件下载错误状态 */
#define SMSS_VERM_CURVER_CHECK_FAILED   9   /* 当前版本文件校验错误状态 */
#define SMSS_VERM_CURVER_INVALID        10 /* 当前版本文件初始状态 */
#define SMSS_VERM_CURVER_DELETED        11 /* 当前版本文件删除状态 */
#define SMSS_VERM_RUNVER_NORMAL         0  /* 运行版本状态：正常 */
#define SMSS_VERM_RUNVER_CHANGED        1  /* 运行版本状态：已修改 */
/* 向RDBS置单板版本状态的宏定义开始 */

/* 故障态,即单板主处理器处于故障状态。此状态由全局板SMSS检测得到，无需单板上报 */
#define SMSS_VERM_BOARD_VERSTATE_FAULT   ((UINT32)8) 
/* BOOT中态,即单板主处理器处于BOOT状态。此状态由全局板SMSS根据单板主处理器上报的BOOT消息而得到 */ 
#define SMSS_VERM_BOARD_VERSTATE_BOOT   ((UINT32)5)  
 /* 正常态,即单板主处理器正常运行，所有版本文件的运行版本、Flash版本和配置版本一致 */
#define SMSS_VERM_BOARD_VERSTATE_NORMAL   ((UINT32)0) 
/* 下载中态,即单板主处理器正常运行，且当前有至少一个版本文件处于下载状态。 */
#define SMSS_VERM_BOARD_VERSTATE_DOWNLOADING   ((UINT32)3) 
/* 校验失败态,即单板主处理器正常运行，当前没有在下载版本文件，至少有一个版本文件下载后校验失败 */
#define SMSS_VERM_BOARD_VERSTATE_CHECKFAIL   ((UINT32)2) 
/* 待激活态(下载完成校验成功),即单板主处理器正常运行，所有版本文件均完成下载，至少有一个版本文件需要进行加载操作 */
#define SMSS_VERM_BOARD_VERSTATE_WAITACTIVE   ((UINT32)1) 
/* 复位中态,即单板主处理器已经被复位，正在重启中。此状态由全局板SMSS在完成对单板的复位操作（可以带较短的延迟时间）后立即进行设置 */
#define SMSS_VERM_BOARD_VERSTATE_RESET   ((UINT32)4) 
/* 补丁操作中, 单板主CPU或从CPU进行补丁操作 */ 
#define SMSS_VERM_BOARD_VERSTATE_PATCH_OPER  ((UINT32)6)
/* 烧写中 */
#define SMSS_VERM_BOARD_VERSTATE_WRITEFILE   ((UINT32)7)

/* 向RDBS置单板版本状态的宏定义结束 */

/* BOOT请求消息 */
typedef SMSS_VERM_VERINFO_REQ_MSG_T SMSS_BOOT_VER_REQ_MSG_T;

/* BOOT响应消息 */
typedef struct 
{ 
    UINT32 ulResult;      /* 返回结果，SMSS_OK表示成功，其他表示错误码 */
    CPS__RDBS_VERINFO_T stVerInfo;   /* 版本信息，参见《G4接口手册》 */
} SMSS_BOOT_VER_RSP_MSG_T;

typedef SMSS_BOOT_VER_RSP_MSG_T SMSS_BOOT_SLAVE_VER_RSP_MSG_T;

typedef struct
{
    UINT8  aucFileName[CPSS_FS_FILENAME_LEN]; /* 文件名 */
    UINT32 ulFileLen;           /* 文件长度 */
    UINT32 ulCheckSum;          /* 校验和 */
} SMSS_VERM_FILE_INFO_T;

/* 文件下载请求消息 */
typedef struct
{
    SMSS_VERM_FILE_INFO_T stInfo;       /* 文件信息 */
    UINT32 ulFdlId;                     /* 从CPU给出的文件传输ID */
} SMSS_VERM_FILE_DOWNLOAD_REQ_MSG_T;

/* 文件下载响应消息 */
typedef struct
{
    UINT32 ulResult;            /* 下载结果
                                   SMSS_OK：成功，
                                   其它：失败 */
    UINT32 ulFdlId;             /* REQ消息中所带的文件传输ID */
    UINT32 ulHostIndex;         /* 主CPU提供的索引号，供后续的主、从CPU间文件传输使用 */
} SMSS_VERM_FILE_DOWNLOAD_RSP_MSG_T;

/* 文件下载结束指示消息 */
typedef struct
{
    UINT32 ulResult;            /* 下载结果
                                   SMSS_OK：成功，
                                   其它：失败 */
    BOOL   bRestart;            /* 是否重启从CPU（立即激活） */
    UINT32 ulHostIndex;         /* 主CPU提供的索引号，
                                   参见SMSS_VERM_FILE_DOWNLOAD_RSP_MSG_T消息定义 */
} SMSS_VERM_FILE_DOWNLOAD_END_IND_MSG_T;


/* 用于重新下载文件测试诊断 */
typedef struct
{
  UINT32 ulSeqId;	 /* 序列号 */
  UINT32 ulTestId;    /* 测试ID  */
  UINT32 TimerId;     /* 定时器描述符 */
} SMSS_VEMR_TESTDIAG_FDL_MSG_T;


typedef struct _SMSS_VERM_FDL_NODE_T
{
    struct _SMSS_VERM_FDL_NODE_T *pstPrev; /* 前向指针，双向链表使用 */
    struct _SMSS_VERM_FDL_NODE_T *pstNext; /* 后向指针，双向链表使用 */
    UINT8   ucFdlType;          /* 文件下载类型 */
    UINT8   ucIndex;            /* g_stSmssCfgVerInfo的下标索引 */
    UINT8   ucFailed;           /* 下载失败次数 */
    UINT8   ucDelayed;          /* 已经过延迟 */
    SMSS_VERM_FILE_INFO_T stInfo; /* 文件信息 */
    CPS__RDBS_DEV_VERFILEDOWNLOAD_IND_MSG_T *pstInd; /* 仅用于主用全局板版本库版本文件添加 */
    
    /* 以下成员仅用于主、从CPU间的版本文件传输 */
    UINT8   *pucBuf;            /* 缓存区地址 */
    UINT16  usSeqNo;            /* 序列号 */
    UINT32  ulFdlId;            /* 文件下载ID，仅用于从CPU */
    UINT32  ulHostIndex;        /* 保存来自主CPU的ulIndex，
                                  参见SMSS_VERM_FILE_DOWNLOAD_RSP_MSG_T消息定义 */
    SMSS_VEMR_TESTDIAG_FDL_MSG_T stTestDg; /* 仅用于测试诊断，下载文件 */

} SMSS_VERM_FDL_NODE_T;

#if defined(SMSS_BOARD_WITH_SLAVE)
typedef struct
{
    SMSS_VERM_FILE_INFO_T stInfo;       /* 文件信息 */
    UINT32 ulFdlId;                     /* 从CPU给出的文件传输ID */
    UINT8 *pucBuf;
} SMSS_VERM_HOST_SLAVE_FILE;
#endif
#define SMSS_VERM_FDL_DATA_SIZE       (512*4) /* 数据包中数据区大小 */

/* DSP加载目前的所处的阶段 */
typedef enum
{
    enCodeLoadSeg    = 0,         /* 加载代码段 */
    enDataLoadSeg    = 1,         /* 加载数据段 */
    enResetDspSeg    = 2,         /* 复位DSP */
} EODspLoad;

#if defined(SMSS_BOARD_WITH_SLAVE) || (SWP_CPUROLE_SLAVE == SWP_CPUROLE_TYPE)

/* 以下数据结构用于主、从CPU间版本文件下载 */

#define SMSS_VERM_FDL_RESEND_MAX        5 /* 最大重发次数 */
/*
#define SMSS_VERM_FDL_HOST_TIMEOUT        5000
#define SMSS_VERM_FDL_SLAVE_TIMEOUT       1000
*/
#define SMSS_VERM_FDL_TIMEOUT       1000

/* 操作码 */
typedef enum
{
    enOpcodeRRQ    = 0,         /* Read request */
    enOpcodeWRQ    = 1,         /* Write request */
    enOpcodeDATA   = 2,         /* Data */
    enOpcodeACK    = 3,         /* Acknowledgment */
    enOpcodeERROR  = 4,         /* Error */
    enOpcodeUNKNOW = 5          /* UNKNOWN */
} EOpcode;

#define SMSS_VERM_FDL_OPCODE_DATA       2
#define SMSS_VERM_FDL_OPCODE_ACK        3

/* 数据包，由主CPU发给从CPU */
typedef struct
{
    UINT16 usOpcode;            /* 操作码，应为2 */
    UINT16 usSeqNo;             /* 序列号 */
    UINT32 ulFdlId;             /* 文件下载Id */
    UINT32 ulLen;               /* 数据长度 */
    UINT8  aucBuf[SMSS_VERM_FDL_DATA_SIZE]; /* 数据 */
} SMSS_VERM_FDL_DATA_MSG_T;

/* 确认包，由从CPU发给主CPU */
typedef struct
{
    UINT16 usOpcode;            /* 操作码，应为3 */
    UINT16 usSeqNo;             /* 序列号 */
    UINT32 ulFdlId;             /* 文件下载Id */
} SMSS_VERM_FDL_ACK_MSG_T;

/* 错误信息包，由主CPU发给从CPU */
typedef struct
{
    int i;
} SMSS_VERM_FDL_ERROR_MSG_T;
#endif

typedef struct
{
    CPSS_COM_PHY_ADDR_T stPhyAddr;
    UINT16 usVerState;
} SMSS_VERM_BRD_VERSTATE_CHG_IND_MSG_T;

#define SMSS_VERM_DSP_VERSEG_NUM    2
#define SMSS_VERM_DSP_LOAD_SEG_LEN  1024*1024
typedef struct  
{
    UINT8  *pucDspBuf;     /* 存储版本文件信息数据指针 */
    struct 
    {
        UINT32 ulBaseAddr;    /* 代码或者数据段的基地址 */
        UINT32 ulSegLength;   /* 代码或者数据段的长度 */    
        UINT32 ulSegOffset;   /* 代码或者数据段的偏移量 */
    }astCodeDataSegInfo[SMSS_VERM_DSP_VERSEG_NUM];
    UINT32 ulRefrenceDsp;  /* 未引用的DSP的个数 */
} SMSS_VERM_DSP_INFO_T;    /* 存储DSP版本文件的信息  */

/* 网元整体升级需要的结构体 */
typedef struct
{
    UINT32  ulSeqId;  /* 响应消息序列号 */ 
    UINT32  ulOpType;
    CPSS_COM_PID_T stDstPid; /* 响应消息纤程ID */
} SMSS_VERM_NE_UPGRADE_MSG_T;


/* 主用全局板上的VERM通知备用板的VERM升级数据文件请求消息体 */
/* SMSS_VERM_UPGRADE_DATA_REQ_MSG */
typedef struct
{ 
	 UINT32           ulOpType;
   char acDataFileName[CPSS_FS_FILEDIR_LEN]; /* 数据文件名，具体要求见请参考
	                                          《统一软件平台OAMS子系统接口手册(M接口)》 */
} SMSS_VERM_UPGRADE_DATA_REQ_MSG_T;

/* 备用全局板上的VERM向主用板的VERM发送升级数据文件响应消息体 */
typedef struct
{
    UINT32  ulResult; 	   /* 升级结果
                                   SMSS_OK：成功，
                                   其它：失败 */
} SMSS_VERM_UPGRADE_DATA_RSP_MSG_T;

typedef struct
{
  UINT32 ulIndex;	 /* 当前需要校验的数组项 */
} SMSS_VEMR_VERLIB_CHECK_IND_MSG_T;

/* verm文件开始下载或下载结束指示消息,用于外围板主CPU(包括备用全局板)向主用全局板发送,或外围板从CPU向外围板主CPU发送. */
#define CPS__OAMS_AM_EVENT_EXTRA_INFO_LEN 256
typedef struct
{
    CPSS_COM_PHY_ADDR_T stPhy;
    UINT32  ulEventNo;      /* 事件号 */
    UINT32  ulEventSubNo;   /* 事件细节号 */
    CPS__OAMS_MO_ID_T    stEventMoId;    /* 事件所对应对象的MO_ID */
    UINT32  ulExtraLen;     /* 事件额外信息长度 */
    UINT8   aucExtraInfo[CPS__OAMS_AM_EVENT_EXTRA_INFO_LEN]; /* 事件额外信息 */
} SMSS_VERM_VER_FDOWNLOAD_EVT_IND_MSG_T;

/* 补丁状态 */
#define SMSS_VEMR_PATCH_STATE_NEED_OPER  1     /* 应实施 */
#define SMSS_VEMR_PATCH_STATE_ALREADY_OPER  2  /* 已实施 */
#define SMSS_VEMR_PATCH_STATE_FAIL  3          /* 失败 */
#define SMSS_VEMR_PATCH_STATE_NEED_UNINSTALL  4 /* 应卸载 */
#define SMSS_VEMR_PATCH_STATE_ALREADY_UNINSTALL  5 /* 已卸载 */

/* 函数修复状态 */
#define SMSS_VEMR_PATCH_FUNC_STATE_NO_REPAIR  1    /* 未修复 */
#define SMSS_VEMR_PATCH_FUNC_STATE_ALREADY_REPAIR  2    /* 已修复 */
#define SMSS_VEMR_PATCH_FUNC_STATE_FIND_OLDFUNC_ERROR  3  /* 查找旧函数失败 */
#define SMSS_VEMR_PATCH_FUNC_STATE_FIND_NEWFUNC_ERROR  4  /* 查找新函数失败 */

/******************************** 全局变量声明 *******************************/

/* 补丁文件头结构定义 */
#define CPSS_FUNC_NAME_LEN  64         /* 函数名长度(单位:字节) */
#define CPSS_FUNC_NUM_MAX   32         /* 一个补丁文件中最多修复的函数个数 */
#define CPSS_PATCH_SAVE_BYTES 16  /* 保存原函数入口处的内容的字节数 */

/* 补丁链表结构 */
typedef struct
{
 UINT8 aucOldName[CPSS_FUNC_NAME_LEN];    /* 待修复缺陷的旧函数名 */
 VOID *pvOldAddr;                            /* 旧函数的入口地址 */
 UINT8 aucOldSave[CPSS_PATCH_SAVE_BYTES]; /* 保存旧函数入口处的内容 */
 UINT8 aucNewName[CPSS_FUNC_NAME_LEN];    /* 已修复缺陷的新函数名 */
 VOID *pvNewAddr;                            /* 新函数的入口地址 */
 UINT32 ulFixState;    /* 函数修复状态：
                         未修复、已修复、查找旧函数失败、查找新函数失败 */
} SMSS_PATCH_FUNC;

typedef struct _SMSS_PATCH_INFO_T
{
  struct _SMSS_PATCH_INFO_T *pstNext;      /* 用于构造链表 */
  UINT8  aucFileName[CPSS_FS_FILENAME_LEN]; /* 补丁文件名（不包含路径） */
  UINT32 ulPatchState;               /* 补丁状态：
                                       应实施、已实施、失败、应卸载、已卸载 */
  UINT32 ulErrorNo;                  /* 仅当补丁状态为失败时有效 */
  #if (SWP_OS_TYPE == SWP_OS_VXWORKS)
  MODULE_ID ulModuleId;                 /* 打补丁前和卸载补丁后，此值为0。
                                       打补丁后，此值为加载的目标模块ID。 */
  #else
  UINT32 ulModuleId;
  #endif                                     
  UINT32 ulPatchNo;                  /* 补丁号 */
  UINT8  aucTaskName[32];            /* 补丁对应的任务名 */
  UINT32  ulFuncNum;                   /* 修复的函数个数，不超过32个 */
  SMSS_PATCH_FUNC astFunc[CPSS_FUNC_NUM_MAX];   /* 修复的函数名称信息 */
  UINT32 ulSendResult;                /* 记录发消息是否成功 0: 成功，1：不成功*/
} SMSS_PATCH_INFO_T;

/* 在IDLE态用于从CPU VERM向主CPU VERM发送逻辑芯片版本信息 */
#define DRV_LOGIC_CHIP_MAX_NUM 16
typedef struct
{
    UINT32   aulLogChipVerInfo[DRV_LOGIC_CHIP_MAX_NUM]; /* 存储逻辑芯片版本信息 */
} SMSS_VERM_LOGCHIP_VERINFO_IND_MSG_T;
/******************************** 全局变量声明 *******************************/
extern SMSS_BRD_INFO_T g_stSmssBrdInfo;
extern SMSS_VERM_VERINFO_T g_stSmssCfgVerInfo;            /* 配置版本信息 */
extern UINT8 g_aucSmssCfgVerStatus[CPS__RDBS_DEV_VERITEM_MAX]; /* 配置版本文件状态 */
extern SMSS_VERM_VERINFO_T g_stSmssCurVerInfo;            /* 当前版本信息 */
extern UINT8 g_aucSmssCurVerStatus[CPS__RDBS_DEV_VERITEM_MAX]; /* 当前版本文件状态 */
extern STRING g_szSmssVermStateStr; /* VERM模块的状态字符串 */
extern SMSS_VERM_FDL_NODE_T g_stSmssSlaveFile;
extern const STRING aszSmssVermState[];
extern STRING szSmssVermFlashDir;
extern STRING szSmssVermVerlibDir;
extern STRING szVerInfoFile;
extern SMSS_VERM_DSP_INFO_T g_stSmssDspVerInfo;          /* 存储DSP的版本文件信息 */
extern UINT32 g_ulCount;
extern UINT32 g_ulReDlVerFile; /* 用于保护，同一时间只能进行一次下载版本文件到指定单板的测试诊断。0：未进行，1：进行中 */

/******************************** 外部函数原形声明 ***************************/
STRING smss_verm_full_name(UINT8 ucFdlType, STRING szFileName);
VOID smss_verm_update_curverinfo(UINT32 ulCfgIndex);
VOID smss_verm_verinfo_change(SMSS_VERM_VERINFO_IND_MSG_T *pstInd);
VOID smss_verm_load_all(VOID);
INT32 smss_verm_buffer_file_verify(CPS__RDBS_VERINFO_T *pstCfg);
VOID smss_verm_set_gcpa_cps__rdbs_verstate(CPSS_COM_PHY_ADDR_T stPhy, UINT16 usVerState);
/* Shell调试命令函数声明 */
extern INT32 smss_verm_print_cfgverinfo(VOID);
extern INT32 smss_verm_print_curverinfo(VOID);

/*向OAMS发送告警的函数*/
extern VOID  smss_verm_send_alarm_to_oams(UINT32 ulAlarmNo,UINT32 ulAlarmSubNo, UINT32 ulAlarmType, 
                                   UINT8* pucFileName, UINT32 ulFileNameLen,
                                   CPSS_COM_PHY_ADDR_T stPhyAddr);
extern VOID  smss_verm_send_alarm_harderror_to_oams(UINT32 ulAlarmSubNo, UINT32 ulAlarmType, 
                                             CPS__RDBS_DEV_GETCURRVERINFO_MSG_REQ_T *pstRdbsRsq,
                                             UINT32 ulResult);
extern VOID smss_verm_curverlib_state_get(STRING szFileName, UINT8 *pucState);
extern VOID smss_verm_curverlib_state_set(STRING szFileName, UINT8 ucState);

/* 下列函数定义于smss_verm_fdl.c */
VOID smss_verm_fdl_init(VOID);
#define SMSS_VERM_FDL_TYPE_SELF         1 /* 主/从CPU下载自已的版本文件 */
#define SMSS_VERM_FDL_TYPE_SLAVE        2 /* 主CPU下载从CPU的版本文件 */
#define SMSS_VERM_FDL_TYPE_VERLIB       3 /* 全局板下载版本库中的文件 */
#define SMSS_VERM_FDL_TYPE_VERLIB_ADD   4 /* 仅用于处理收到RDBS的全局板版本库版本文件添加消息 */
#define SMSS_VERM_FDL_TYPE_SELF_TESTDIAG         5 /* 测试诊断中主/从CPU下载自已的版本文件 */
#define SMSS_VERM_FDL_TYPE_VERLIB_ADD_TESTDIAG   6 /* 测试诊断中全局板下载版本库中的文件 */
extern VOID smss_verm_diagtest_redownload_verlib(CPS__OAMS_DIAG_START_TEST_REQ_MSG_T *pstdiagtest);
VOID smss_verm_fdl_add(UINT8 ucFdlType, UINT32 ulIndex, SMSS_VERM_FILE_INFO_T *pstInfo, SMSS_VEMR_TESTDIAG_FDL_MSG_T stTestDg);
VOID smss_verm_fdl_end(SMSS_VERM_FDL_NODE_T *pstNode, UINT32 bSuccess);
VOID smss_verm_fdl_next(VOID);

#if defined(SMSS_BOARD_WITH_SLAVE) /* 带有从CPU的主CPU */
VOID smss_verm_fdl_send_data(VOID);
VOID smss_verm_fdl_rcvd_ack(SMSS_VERM_FDL_ACK_MSG_T *pstAck);
#endif
#if (SWP_CPUROLE_SLAVE == SWP_CPUROLE_TYPE) /* 从CPU */
VOID smss_verm_fdl_rcvd_data(SMSS_VERM_FDL_DATA_MSG_T *pstData);
VOID smss_verm_fdl_resend_ack(VOID);
#endif

/* 下列函数定义于smss_verm_verlib.c */
/*设置版本库中相应当前的版本文件的版本文件状态.*/
VOID smss_verm_verlib_set_cur_state(STRING szFileName, UINT16 usState);
/*设置版本库中相应配置版本文件的版本文件状态.*/
VOID smss_verm_verlib_set_cfg_state(STRING szFileName, UINT16 usState);
/*向RDBS发送版本库版本文件状态改变请求。.*/
VOID smss_verm_verlib_to_cps__rdbs_send_fstate_ind(STRING szFileName, UINT16 usState);
/* 版本库审核.*/
VOID smss_verm_verlib_check(VOID);
/* 处理发来的版本库添加版本文件消息(CPS__RDBS_DEV_VERFILEDOWNLOAD_IND_MSG).*/
VOID smss_verm_verlib_cps__rdbs_file_add(CPS__RDBS_DEV_VERFILEDOWNLOAD_IND_MSG_T *pstInd);
/* 版本库版本文件删除.*/
VOID smss_verm_verlib_file_delete(CPS__RDBS_DEV_VERFILEDEL_IND_MSG_T *pstInd);
/* 处理发来版本库删除版本文件消息(CPS__RDBS_DEV_VERFILEDEL_IND_MSG).*/
VOID smss_verm_verlib_cps__rdbs_file_delete(CPS__RDBS_DEV_VERFILEDEL_IND_MSG_T *pstInd);

/* 下列函数定义于smss_verm_dev.c */
#if (SWP_CPUROLE_HOST == SWP_CPUROLE_TYPE) /* 主CPU */
#if defined(SMSS_BOARD_WITH_DSP)
VOID smss_verm_start_all_dsp(VOID);
VOID smss_verm_start_dsp(UINT8 ucDsp);
VOID smss_verm_reset_all_dsp(VOID);
VOID smss_verm_update_dsp_verinfo(VOID);
VOID smss_verm_load_all_dsp(VOID);
VOID smss_verm_dsp_load_seg(UINT8 ucDsp);
VOID smss_verm_init_load_dsp(UINT8 ucDsp);

#endif
VOID smss_verm_load_all_fpga(VOID);
#else  /* 从CPU */
#if (SWP_PHYBRD_MNPA == SWP_PHYBRD_TYPE) /* MNPA带有ME */
VOID smss_verm_load_all_me(VOID);
#endif
#endif

VOID smss_verm_flush_dir(STRING szDirName); /*删除配置版本信息中当前FLASH上多余文件 */
/* APP阶段版本文件下载开始,下载结束事件指示 */
VOID smss_verm_even_fdownload_ind(STRING szFileName, UINT32 usState);
UINT32 smss_verm_set_upgstatus(UINT32 ulStatus);
UINT32 g_NeUpgTagSmss; /* 正常中*/
extern UINT32 g_ulSmssDevmActComplete;
extern UINT32 g_ulBakFileCpyOk;
/******************************** 头文件保护结尾 *****************************/
#endif /* SMSS_VERM_H */
/******************************** 头文件结束 *********************************/
