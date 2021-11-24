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
#ifndef DRV_PUBLIC_H_
#define DRV_PUBLIC_H_

#include "cpss_public.h"

#define DRV_BSP_OK  0    /* 成功 */
#define DRV_BSP_ERROR  (-1) /* 失败 */

/* 输出结果重定向回调函数 */
typedef VOID (*DRV_SHCMD_OUTPUT_ACCEPT_PF)(STRING szText);

/*in smss_sysctl.h */
#define  DRV_NSM_CLK_INFO_SYNC_MSG_MAX_LEN    200
/*end in smss_sysctl.h*/

/*in smss_devm.h*/

#define DRV_BSP_OK  0    /* 成功 */
#define DRV_BSP_ERROR  (-1) /* 失败 */

/* for smss or tnbs query ,alarm query */
typedef struct
{
    /* UINT32 ulAlmSubNo;*/  /* 告警细节号 */
    UINT32 ulPortNo; /* 端口号或链路号 */
    UINT32 ulStatus; /* DRV_ALARM_OCCUR:告警产生； DRV_ALARM_CLEAR：告警解除 */
    UINT32 ulCount; /* 告警计数器 ,通过计数来反映告警存在变化*/
}DRV_ALM_ELM_T;

#define DRV_SHELF_FAN_ALM_NUM    15  /* 风机盒告警15个 */

#define DRV_SHELF_FAN_NUM        4   /* 机框中风机盒的个数 */

typedef struct{
   UINT8 ucPortId; /*媒体面上行端口ID */
   UINT8 ucState;  /* 媒体面上行端口状态，即正常
                    DRV_PORT_STATE_NORMAL；故障 DRV_PORT_STATE_DISABLE */
   UINT16 usResv;/* 保留，结构对齐字段*/
}DRV_IPHA_UL_PORT_STATE_INFO_MSG_T;

#define DRV_MASA_GE_MEDIAPORT_MAX_NUM 2
#define DRV_MASA_BASE_UPPORT_MAX_NUM 2

typedef struct{
    UINT32 ulPortNum; /*媒体面上行端口数，即最大值为 DRV_MASA_GE_MEDIAPORT_MAX_NUM?? 。*/
    DRV_IPHA_UL_PORT_STATE_INFO_MSG_T astPortInfo[DRV_MASA_GE_MEDIAPORT_MAX_NUM+DRV_MASA_BASE_UPPORT_MAX_NUM];/* 媒体面上行端口信息*/
} DRV_IPHA_UL_PORT_INFO_MSG_T;

#define DRV_BOARD_MEDIA_PORT_NUM 2 /* 板上媒体面端口数目*/

#define DRV_DSP_MEDIA_PORT_NUM   12 /* MDPA上的与DSP连接的媒体面端口数目*/

typedef struct
{
    UINT32 ulPortNo; /* 端口号或链路号 */
    UINT32 ulStatus; /* DRV_EVENT_OCCUR:事件产生； DRV_EVENT_CLEAR：无事件产生。*/
    UINT32 ulCount; /* 事件计数器 ,通过计数来反映事件存在变化。事件计数器DRV始终只进行累加操作。*/
}DRV_EVENT_ELM_T;

#define DRV_MAX_CHILD_BRD_NUM  4

/* 单板信息定义 */
/* 母板信息定义*/
typedef struct
{
   UINT32  ulBrdType;    /* 物理板类型 */
   UINT32  ulBrdVer;     /* 物理板版本 */
} DRV_PHYBRD_INFO_VER_T;

typedef struct
{
    DRV_PHYBRD_INFO_VER_T    stMotherBrd;  /* 母板（前插板） */
    DRV_PHYBRD_INFO_VER_T    stBackBrd;    /* 后插板 */
    UINT32  ulChildBrdNum;
    DRV_PHYBRD_INFO_VER_T  astChildBrd[DRV_MAX_CHILD_BRD_NUM];/*后插板*/
} DRV_PHYBRD_INTEGRATED_INFO_VER_T;

/*end in smss_devm.h*/

/*in smss_verm_fdl.c*/

/*以太端口类别号定义：*/

#define DRV_ETHER_IPV4_S   (0x0)
#define DRV_ETHER_CTRL_PORT_IPV4   (DRV_ETHER_IPV4_S  + 0x1) /*控制面IPV4的通信端口*/

/*LED灯的类别定义，与IPMI规范定义的LED类别ID值一致： */
#define DRV_LED_TYPE_BLUE_LED   0x00    /*ATCA单板热插拔H/S指示灯*/
#define DRV_LED_TYPE_OOS  0x01   /*ATCA单板Out Of Service状态指示灯*/
#define DRV_LED_TYPE_IS   0x02    /*ATCA单板服务IS灯*/
#define DRV_LED_TYPE_ATTN  0x03   /*ATCA单板告警ATTN灯*/
#define DRV_LED_TYPE_ACTIVE  0x04   /*ATCA单板主用状态灯*/

/* LED灯的状态定义： */
#define DRV_LED_STATU_OFF  0x0  /* 灯灭 */
#define DRV_LED_STATU_ON  0x1  /* 灯亮  */
#define DRV_LED_STATU_BLINK  0x2  /*闪烁模式，一个周期200ms亮200ms灭*/

/*end in smss_verm_fdl.c*/

/*in cpss_com_drv.h*/

/*配置的PCI内存窗口，相关信息查询数据结构*/
typedef struct
{
    UINT32 ulCpuId;/*本CPU编号，取值为：HOST为1，SLAVE为2，DSP为5～17。*/
    UINT32 ulStartAddr;/*PCI窗口的起始地址*/
    UINT32 ulLen; /*PCI窗口的大小*/
    UINT32 ulRamWinAttr;/*PCI内存窗口的属性,PCI内存窗口是在哪个CPU上,取值采用CpuID号标识,*/
}DRV_PCI_INFO_T;

/*end in cpss_com_drv.h*/

/*in smss_devm.c*/

#define DRV_ETHER_UPDATE_PORT_IPV4  (DRV_ETHER_IPV4_S  + 0x2) /*Update通道IPV4的通信端口*/

#define DRV_ALM_NOTIFY_SMSS_MSG     0x25000003   /* 告警通知消息 */

#define DRV_ETHERNET_PORT_SWITCH_MSG  0x25000006  /*以太端口切换通知消息*/

#define DRV_IPHA_PORT_0  0 /* 端口0，对应7槽masa*/
#define DRV_IPHA_PORT_1  1 /* 端口1，对应8槽masa*/

/*告警Status定义*/
#define  DRV_ALARM_OCCUR    0          /* 告警产生 */
#define  DRV_ALARM_CLEAR    1          /* 告警清除 */

typedef INT32 (*DRV_SHCMD_PF)(UINT32 ulPara0, UINT32 ulPara1,UINT32 ulPara2, UINT32 ulPara3);

/* ldt命令结构*/
typedef struct
{
    STRING szFuncName;             /* 函数名称 */
    STRING szFuncHelp;             /* 函数帮助信息 */
    STRING szArgFmt;               /*参数格式字符串,最多可带4个参数*/
    DRV_SHCMD_PF  pfShcmdFunc;    /* 函数地址 */
} DRV_SHCMD_INFO_T;

/*end in smss_devm.c*/

/*in cpss_com_common.c*/

#define DRV_ETHER_DEBUG_PORT_IPV4 (DRV_ETHER_IPV4_S  + 0x4) /*调试用 IPV4的通信端口*/

#define DRV_ETHER_MEDIA_PORT_IPV4   (DRV_ETHER_IPV4_S  + 0x3) /*媒体面通道IPV4的通信端口*/

#define DRV_ETHER_FAR_PORT_IPV4   (DRV_ETHER_IPV4_S  + 0x5) /*FAR IPV4的通信端口*/

/*end in cpss_com_common.c*/


#endif /* DRV_PUBLIC_H_ */
