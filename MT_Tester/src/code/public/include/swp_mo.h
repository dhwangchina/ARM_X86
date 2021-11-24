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
#ifndef SWP_MO_H
#define SWP_MO_H

#ifdef __cplusplus
extern "C" {
#endif



/*******************************************************************************
*                               宏和常量定义                                   *
*******************************************************************************/
/* MOI字节长度 */
#define SWP_MOI_LEN                     (16)

/* MOC无效标记 */
#define SWP_MOC_NULL                    (0x00000000)
#define SWP_MOC_INVALID                 (SWP_MOC_NULL)

/* MOC定义(最高8bit为网元宏定义) */
#define SWP_MOC_DEF(ulMoc) \
    (((UINT32)(SWP_NE_TYPE) << 24) | ((UINT32)(ulMoc)  & 0x00ffffff))

#define PUBLIC_MAC_LENTH   6
#define PUBLIC_IPV4_LENTH  4
#define PUBLIC_NAME_LENTH  32
/* MOC类型 */
/* 全局数据层 */
#define SWP_MOC_GLOBAL                  SWP_MOC_DEF(0x00100000) /* 全局数据层 */
#define SWP_MOC_GLOBAL_DATA             SWP_MOC_DEF(0x00100001) /* 平台全局数据 */
    
/* 物理设备层 */
#define SWP_MOC_DEV                     SWP_MOC_DEF(0x00101000) /* 物理设备层 */
#define SWP_MOC_DEV_FRAME               SWP_MOC_DEF(0x00101001) /* 机架 */
#define SWP_MOC_DEV_SHELF               SWP_MOC_DEF(0x00101002) /* 机框 */
#define SWP_MOC_DEV_POWER               SWP_MOC_DEF(0x00101003) /* 电源 */
#define SWP_MOC_DEV_SHELF_MNG           SWP_MOC_DEF(0x00101004) /* 机框管理器 */
#define SWP_MOC_DEV_SHELF_FAN           SWP_MOC_DEF(0x00101005) /* 风扇 */
#define SWP_MOC_DEV_BOARD               SWP_MOC_DEF(0x00101006) /* 单板 */
#define SWP_MOC_DEV_CPU                 SWP_MOC_DEF(0x00101007) /* 处理器 */
#define SWP_MOC_DEV_DSP                 SWP_MOC_DEF(0x00101008) /* DSP */
#define SWP_MOC_DEV_E1_PORT             SWP_MOC_DEF(0x00101009) /* E1端口 */
#define SWP_MOC_DEV_EXT_CLOCK           SWP_MOC_DEF(0x0010100A) /* 外部时钟源 */
#define SWP_MOC_DEV_SHELF_FAN_BOX       SWP_MOC_DEF(0x0010100B) /* 风扇盒 */
#define SWP_MOC_DEV_HW_PORT             SWP_MOC_DEF(0x0010100C) /* HW端口 */
#define SWP_MOC_DEV_ETH_PORT            SWP_MOC_DEF(0x0010100D) /* 网络端口 */
#define SWP_MOC_DEV_ALARM_BOX           SWP_MOC_DEF(0x00101010) /* 告警箱 */
#define SWP_MOC_ATM_CLK_EXTRACT         SWP_MOC_DEF(0x00101011) /* 端口线路时钟配置 */

#define SWP_MOC_DEV_APS_GRP             SWP_MOC_DEF(0x00101020) /* APS光口组 */
#define SWP_MOC_DEV_ATM_PORT            SWP_MOC_DEF(0x00101021) /* ATM端口 */
#define SWP_MOC_DEV_ATM_SAR             SWP_MOC_DEF(0x00101022) /* ATM SAR配置 */
#define SWP_MOC_DEV_STM1_PORT           SWP_MOC_DEF(0x00101023) /* STM1光口 */
#define SWP_MOC_DEV_APS_OMAPE1          SWP_MOC_DEF(0x00101024) /* 通道化E1链路 */
#define SWP_MOC_DEV_MACCHIPCFG          SWP_MOC_DEF(0x00101030) /* MAC芯片配置 */
#define SWP_MOC_DEV_APS_PORT_INTFSYS    SWP_MOC_DEF(0x00101031) /* STM1参数INTFSYS */
#define SWP_MOC_DEV_APS_PORT_SOH        SWP_MOC_DEF(0x00101032) /* STM1参数SOH */
#define SWP_MOC_DEV_APS_PORT_LOH        SWP_MOC_DEF(0x00101033) /* STM1参数LOH */
#define SWP_MOC_DEV_APS_PORT_POH        SWP_MOC_DEF(0x00101034) /* STM1参数POH */
#define SWP_MOC_DEV_APS_PORT_PYLD       SWP_MOC_DEF(0x00101035) /* STM1参数PYLD */
#define SWP_MOC_DEV_APS_PORT_INTFLINE   SWP_MOC_DEF(0x00101036) /* STM1参数INTFLINE */
#define SWP_MOC_DEV_APS_PORT_J0S1       SWP_MOC_DEF(0x00101037) /* STM1参数J0S1 */
#define SWP_MOC_DEV_APS_PORT_J1C2       SWP_MOC_DEF(0x00101038) /* STM1参数J1C2 */
#define SWP_MOC_DEV_IMA_GRP             SWP_MOC_DEF(0x00101040) /* IMA组 */
#define SWP_MOC_DEV_IMA_LNK             SWP_MOC_DEF(0x00101041) /* IMA链路 */


/* 物理资源层 */
#define SWP_MOC_RES                     SWP_MOC_DEF(0x00102000) /* 物理资源层 */
#define SWP_MOC_RES_MODULE              SWP_MOC_DEF(0x00102001) /* 资源模块 */
#define SWP_MOC_RES_GROUP               SWP_MOC_DEF(0x00102002) /* 资源组 */
#define SWP_MOC_RES_SUBGRP_CPU          SWP_MOC_DEF(0x00102003) /* CPU资源子组 */
#define SWP_MOC_RES_SUBGRP_SLAVE_CPU    SWP_MOC_DEF(0x00102004) /* 从CPU资源子组 */
#define SWP_MOC_RES_SUBGRP_DSP          SWP_MOC_DEF(0x00102005) /* DSP资源子组 */
#define SWP_MOC_RES_SUBGRP_E1_PORT      SWP_MOC_DEF(0x00102006) /* E1端口资源子组 */
#define SWP_MOC_RES_SUBGRP_OPT_PORT     SWP_MOC_DEF(0x00102007) /* 光口资源子组 */
#define SWP_MOC_RES_SUBGRP_HW           SWP_MOC_DEF(0x00102008) /* HW资源子组 */
#define SWP_MOC_RES_SUBGRP_IP_PORT      SWP_MOC_DEF(0x00102009) /* 网络端口资源子组 */

    
/* 虚拟实体层 */
#define SWP_MOC_VRTL                    SWP_MOC_DEF(0x00102500) /* 虚拟实体层 */
#define SWP_MOC_VRTL_VER_LIB            SWP_MOC_DEF(0x00102501) /* 版本库 */
#define SWP_MOC_VRTL_VER_RUN            SWP_MOC_DEF(0x00102502) /* 运行版本 */
#define SWP_MOC_VRTL_VER_SPEC           SWP_MOC_DEF(0x00102503) /* 指定版本 */
#define SWP_MOC_VRTL_BRD_SET            SWP_MOC_DEF(0x00102504) /* 物理板组合 */
#define SWP_MOC_VRTL_BRD_SWPKT          SWP_MOC_DEF(0x00102505) /* 单板软件包 */
#define SWP_MOC_VRTL_BRD_SWPKTFILE      SWP_MOC_DEF(0x00102506) /* 单板软件包文件 */
#define SWP_MOC_VRTL_COUNTER_GRP        SWP_MOC_DEF(0x00102511) /* 计数器组 */
#define SWP_MOC_VRTL_MEAS_OBJ           SWP_MOC_DEF(0x00102512) /* 测量对象 */
#define SWP_MOC_VRTL_MEAS_FLTR          SWP_MOC_DEF(0x00102513) /* 测量过滤 */
#define SWP_MOC_VRTL_DRV_GRP            SWP_MOC_DEF(0x00102514) /* 派生计数器组 */
#define SWP_MOC_VRTL_DRV_METHOD         SWP_MOC_DEF(0x00102515) /* 派生计数器计算规则 */
#define SWP_MOC_VRTL_NSCA               SWP_MOC_DEF(0x00102521) /* 网同步配置 */
#define SWP_MOC_VRTL_NTP                SWP_MOC_DEF(0x00102522) /* NTP服务器配置 */
#define SWP_MOC_VRTL_ALARM_INFO         SWP_MOC_DEF(0x00102530) /* 告警信息配置 */
#define SWP_MOC_VRTL_PRIVATETIMER       SWP_MOC_DEF(0x00102531) /* 自定义定时器配置 */
#define SWP_MOC_VRTL_USER               SWP_MOC_DEF(0x00102532) /* MML用户信息配置  */
#define SWP_MOC_VRTL_LOG                SWP_MOC_DEF(0x00102533) /* 日志配额信息配置 */
#define SWP_MOC_VRTL_FAN                SWP_MOC_DEF(0x00102534) /* 风扇控制配置     */
#define SWP_MOC_VRTL_CPU_LOAD           SWP_MOC_DEF(0x00102535) /* 处理器过载门限配置 */
#define SWP_MOC_VRTL_BOARD_LOAD         SWP_MOC_DEF(0x00102536) /* 单板过载门限配置 */

/* ATM承载层 */
#define SWP_MOC_ATM                     SWP_MOC_DEF(0x00103000) /* ATM承载层 */
#define SWP_MOC_ATM_PVC                 SWP_MOC_DEF(0x00103001) /* PVC */
#define SWP_MOC_ATM_OFFICE              SWP_MOC_DEF(0x00103002) /* ATM邻接局 */
#define SWP_MOC_ATM_SIGLNK              SWP_MOC_DEF(0x00103003) /* ATM信令链路 */
#define SWP_MOC_ATM_AAL2_PATH           SWP_MOC_DEF(0x00103004) /* AAL2通道 */
#define SWP_MOC_ATM_AAL5_IPOA           SWP_MOC_DEF(0x00103005) /* IPOA链路 */
#define SWP_MOC_ATM_IU_SIGLNK           SWP_MOC_DEF(0x00103006) /* ATM IU信令链路 */
    
/* IP承载层(IP) */
#define SWP_MOC_IP                      SWP_MOC_DEF(0x00103100) /* IP承载层 */
#define SWP_MOC_IP_IPSTACK              SWP_MOC_DEF(0x00103101) /* IP协议栈子层 */
#define SWP_MOC_IP_IPSTACK_IPPORT       SWP_MOC_DEF(0x00103102) /* IP端口 */
#define SWP_MOC_IP_IPSTACK_L2TP         SWP_MOC_DEF(0x00103103) /* L2TP连接 */
#define SWP_MOC_IP_IPSTACK_SCTP         SWP_MOC_DEF(0x00103104) /* SCTP流 */
#define SWP_MOC_IP_IPSTACK_ACL          SWP_MOC_DEF(0x00103105) /* 访问控制列表 */
#define SWP_MOC_IP_IPSTACK_SAD          SWP_MOC_DEF(0x00103106) /* 安全关联配置 */
#define SWP_MOC_IP_IPSTACK_SRVREG       SWP_MOC_DEF(0x00103107) /* 业务注册 */
#define SWP_MOC_IP_IPSTACK_TUNNEL       SWP_MOC_DEF(0x00103108) /* 隧道 */
#define SWP_MOC_IP_IPSTACK_DSCPBA       SWP_MOC_DEF(0x00103109) /* DSCP BA分类 */
#define SWP_MOC_IP_IPSTACK_DSCPMF       SWP_MOC_DEF(0x0010310A) /* DSCP MF分类 */
#define SWP_MOC_IP_IPSTACK_DSCPSCHED    SWP_MOC_DEF(0x0010310B) /* DSCP端口与队列调度 */
#define SWP_MOC_IP_IPSTACK_DSCPWRED     SWP_MOC_DEF(0x0010310C) /* DSCP队列流控 */
#define SWP_MOC_IP_IPSTACK_IKECTRL      SWP_MOC_DEF(0x0010310D) /* IKE协议控制 */
#define SWP_MOC_IP_IPSTACK_IKECFG       SWP_MOC_DEF(0x0010310E) /* IKE协议配置 */
#define SWP_MOC_IP_IPSTACK_SERVER       SWP_MOC_DEF(0x0010310F) /* 外部服务器 */
#define SWP_MOC_IP_IPSTACK_DNSENTRY     SWP_MOC_DEF(0x00103110) /* 域名表项 */
#define SWP_MOC_IP_IPSTACK_NATP         SWP_MOC_DEF(0x00103111) /* NATP表项对 */
#define SWP_MOC_IP_IPSTACK_ARP          SWP_MOC_DEF(0x00103112) /* ARP表项 */
#define SWP_MOC_IP_IPSTACK_GTPUCFG      SWP_MOC_DEF(0x00103113) /* GTPU协议配置 */
#define SWP_MOC_IP_IPSTACK_PPPCFG       SWP_MOC_DEF(0x00103114) /* PPP协议配置 */
#define SWP_MOC_IP_IPSTACK_PPPPORT      SWP_MOC_DEF(0x00103115) /* PPP链路端口配置 */
#define SWP_MOC_IP_IPSTACK_NCP          SWP_MOC_DEF(0x00103116) /* NCP配置 */
#define SWP_MOC_IP_IPSTACK_OFF          SWP_MOC_DEF(0x00103117) /* IP局向配置 */
#define SWP_MOC_IUB_SIGTRAN_SCTP_ASSO   SWP_MOC_DEF(0x00103118) /* IUB局向SCTP偶联 */

#define SWP_MOC_IP_RTMNG                SWP_MOC_DEF(0x00103141) /* 路由管理子层 */
#define SWP_MOC_IP_RTMNG_SV4ROUTE       SWP_MOC_DEF(0x00103142) /* IPV4静态路由 */
#define SWP_MOC_IP_RTMNG_SV6ROUTE       SWP_MOC_DEF(0x00103143) /* IPV6静态路由 */
#define SWP_MOC_IP_RTMNG_OSPFV2PARA     SWP_MOC_DEF(0x00103144) /* OSPF V2协议配置 */
#define SWP_MOC_IP_RTMNG_OSPFV2AREA     SWP_MOC_DEF(0x00103145) /* OSPF V2路由区 */
#define SWP_MOC_IP_RTMNG_OSPFV2ITF      SWP_MOC_DEF(0x00103146) /* OSPF V2接口 */
#define SWP_MOC_IP_RTMNG_OSPFV2NBR      SWP_MOC_DEF(0x00103147) /* OSPF V2邻居 */
#define SWP_MOC_IP_RTMNG_OSPFV3PARA     SWP_MOC_DEF(0x00103148) /* OSPF V3协议配置 */
#define SWP_MOC_IP_RTMNG_OSPFV3AREA     SWP_MOC_DEF(0x00103149) /* OSPF V3路由区 */
#define SWP_MOC_IP_RTMNG_OSPFV3ITF      SWP_MOC_DEF(0x0010314A) /* OSPF V3接口 */
#define SWP_MOC_IP_RTMNG_OSPFV3NBR      SWP_MOC_DEF(0x0010314B) /* OSPF V3邻居 */
#define SWP_MOC_IP_RTMNG_RIPPARA        SWP_MOC_DEF(0x0010314C) /* RIP协议配置 */
#define SWP_MOC_IP_RTMNG_RIPITF         SWP_MOC_DEF(0x0010314D) /* RIP接口 */
#define SWP_MOC_IP_RTMNG_RIPNBR         SWP_MOC_DEF(0x0010314E) /* RIP邻居 */
#define SWP_MOC_IP_RTMNG_RIPNGPARA      SWP_MOC_DEF(0x0010314F) /* RIPNG协议配置 */
#define SWP_MOC_IP_RTMNG_RIPNGITF       SWP_MOC_DEF(0x00103150) /* RIPNG接口 */
#define SWP_MOC_IP_RTMNG_RIPNGNBR       SWP_MOC_DEF(0x00103151) /* RIPNG邻居 */

/* 7号信令承载层(N7) */
#define SWP_MOC_N7_MTP2_SIGLNK          SWP_MOC_DEF(0x00104001) /* MTP2信令链路 */
#define SWP_MOC_N7_LOC_OFFICE           SWP_MOC_DEF(0x00104002) /* 本局信令点   */
#define SWP_MOC_N7_ADJ_OFFICE           SWP_MOC_DEF(0x00104003) /* N7邻接局       */
#define SWP_MOC_N7_SIGLNK_SET           SWP_MOC_DEF(0x00104004) /* 信令链路组   */
#define SWP_MOC_N7_SIGLNK               SWP_MOC_DEF(0x00104005) /* 信令链路     */
#define SWP_MOC_N7_OFFICE_RT            SWP_MOC_DEF(0x00104006) /* 局向信令路由 */
#define SWP_MOC_N7_STATIC_RT            SWP_MOC_DEF(0x00104007) /* 信令静态路由 */
#define SWP_MOC_N7_SCCP_SSN             SWP_MOC_DEF(0x00104008) /* SCCP子系统   */
#define SWP_MOC_N7_GT_TRANSLATOR        SWP_MOC_DEF(0x00104009) /* GT翻译选择   */
#define SWP_MOC_N7_GT_NODE              SWP_MOC_DEF(0x0010400A) /* GT翻译号码   */
#define SWP_MOC_N7_SCCP_SHIELD          SWP_MOC_DEF(0x0010400B) /* SCCP屏蔽功能 */
#define SWP_MOC_N7_MTP_SHIELD           SWP_MOC_DEF(0x0010400C) /* MTP屏蔽功能  */
#define SWP_MOC_N7_TNSS_TIMER           SWP_MOC_DEF(0x0010400D) /* TNSS定时器 */
#define SWP_MOC_N7_LOC_SCCP_SSN         SWP_MOC_DEF(0x0010400E) /* 本局SCCP子系统 */
/* IP信令承载层 (SIGTRAN) */
#define SWP_MOC_SIGT                    SWP_MOC_DEF(0x00104100) /* SIGTRAN                */
#define SWP_MOC_SIGTRAN_AS              SWP_MOC_DEF(0x00104101) /* M3UA应用服务器         */
#define SWP_MOC_SIGTRAN_AS_LOC          SWP_MOC_DEF(0x00104102) /* M3UA应用服务器定位信息 */
#define SWP_MOC_SIGTRAN_ASP             SWP_MOC_DEF(0x00104103) /* M3UA应用服务器进程     */
#define SWP_MOC_SIGTRAN_SCTP_ASSO       SWP_MOC_DEF(0x00104104) /* SCTP偶联               */



/*******************************************************************************
*                               类型定义                                       *
*******************************************************************************/
/* MOI -- 平台全局数据 */
typedef struct
{
    UINT16   usNeElement;   /* 网元标识 */
    UINT16   usNeElementId; /* 与usNeElement取值相同(OMC内部处理需要) */
} SWP_MOI_GLOBAL_T;

/* MOI -- 机架 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
} SWP_MOI_DEV_FRAME_T;

/* MOI -- 机框 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
} SWP_MOI_DEV_SHELF_T;

/* MOI -- 电源 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucPowerNo;      /* 电源编号 (1..2)*/
} SWP_MOI_DEV_POWER_T;

/* MOI -- 机框管理器 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucShelfMngNo;   /* 机框管理器编号(1..2)*/
} SWP_MOI_DEV_SHELF_MNG_T;

/* MOI -- 风扇盒 */    
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucShelfFanBoxNo; /* 机框风扇盒编号 */
} SWP_MOI_DEV_SHELF_FAN_BOX_T;
    
/* MOI -- 风扇 */
typedef struct
{
    UINT16  usNeElement;     /* 网元标识 */
    UINT8   ucFrameNo;       /* 机架编号(1..5) */
    UINT8   ucShelfNo;       /* 机框编号(1..3) */
    UINT8   ucShelfFanBoxNo; /* 机框风扇盒编号 */
    UINT8   ucShelfFanNo;    /* 机框风扇编号 */
} SWP_MOI_DEV_SHELF_FAN_T;

/* MOI -- 单板 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
} SWP_MOI_DEV_BOARD_T;

/* MOI -- 处理器 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucCpuNo;        /* 处理器编号(CPU为1..4) */
} SWP_MOI_DEV_CPU_T;

/* MOI -- DSP */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucDspNo;        /* 处理器编号(DSP为5..31) */
} SWP_MOI_DEV_DSP_T;

/* MOI -- E1端口 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucE1Link;       /* E1物理链路编号(1..16) */
} SWP_MOI_DEV_E1_PORT_T;

/* MOI -- 外部时钟源 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucExtClockNo;   /* 外部时钟源编号 */
} SWP_MOI_DEV_EXT_CLOCK_T;

/* MOI -- HW端口 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucHwPortNo;     /* HW端口编号 */
} SWP_MOI_DEV_HW_PORT_T;

/* MOI -- 网络端口 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucEthPortNo;    /* 网络端口编号 */
} SWP_MOI_DEV_ETH_PORT_T;

/* MOI -- 告警箱 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucAlarmBoxId;   /* 告警箱编号（固定为1） */
} SWP_MOI_DEV_ALARM_BOX_T;

/* MOI -- 端口线路时钟配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucPort;         /* 端口号,NASA板、CASA板取值0..3;IAEA板0..15;MEPA板0..31 */
} SWP_MOI_ATM_CLK_EXTRACT_T;

/* MOI -- APS光口组 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucMasterPortNo; /* 主光口编号(0或2) */
} SWP_MOI_DEV_APS_GRP_T;

/* MOI -- ATM端口 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucApcPortNo;    /* APC芯片端口编号(0..31) */
} SWP_MOI_DEV_ATM_PORT_T;

/* MOI -- ATM SAR配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucApcPortNo;    /* APC芯片端口编号(固定取1) */
} SWP_MOI_DEV_ATM_SAR_T;

/* MOI -- STM1光口 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucPortNo;       /* 光口编号(0..3) */
} SWP_MOI_DEV_STM1_PORT_T;

/* MOI -- 通道化E1链路 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucMasterPortNo; /* 主光口编号(0或2) */
    UINT8   ucE1LinkId;     /* E1链路号(0..62) */
} SWP_MOI_DEV_APS_OMAPE1_T;

/* MOI -- MAC芯片配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucPortId;       /* Mac芯片端口号 */
} SWP_MOI_DEV_MAC_CHIP_CFG_T;

/* MOI -- STM1参数INTFSYS */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucMasterPortNo; /* 主光口编号 */
    UINT8   ucPortId;       /* 光口编号(0..3) */
} SWP_MOI_DEV_APS_PORT_INTFSYS_T;

/* MOI -- STM1参数SOH */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucMasterPortNo; /* 主光口编号 */
    UINT8   ucPortId;       /* 光口编号(0..3) */
} SWP_MOI_DEV_APS_PORT_SOH_T;

/* MOI -- STM1参数LOH */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucMasterPortNo; /* 主光口编号 */
    UINT8   ucPortId;       /* 光口编号(0..3) */
} SWP_MOI_DEV_APS_PORT_LOH_T;

/* MOI -- STM1参数POH */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucMasterPortNo; /* 主光口编号 */
    UINT8   ucPortId;       /* 光口编号(0..3) */
} SWP_MOI_DEV_APS_PORT_POH_T;

/* MOI -- STM1参数PYLD */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucMasterPortNo; /* 主光口编号 */
    UINT8   ucPortId;       /* 光口编号(0..3) */
} SWP_MOI_DEV_APS_PORT_PYLD_T;

/* MOI -- STM1参数INTFLINE */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucMasterPortNo; /* 主光口编号 */
    UINT8   ucPortId;       /* 光口编号(0..3) */
} SWP_MOI_DEV_APS_PORT_INTFLINE_T;

/* MOI -- STM1参数J0S1 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucMasterPortNo; /* 主光口编号 */
    UINT8   ucPortId;       /* 光口编号(0..3) */
} SWP_MOI_DEV_APS_PORT_J0S1_T;

/* MOI -- STM1参数J1C2 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucMasterPortNo; /* 主光口编号 */
    UINT8   ucPortId;       /* 光口编号(0..3) */
} SWP_MOI_DEV_APS_PORT_J1C2_T;

/* MOI -- IMA组 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucImaGrpId;     /* IMA组ID，范围:EAIB板0~15；CAIB板 0~83 */
} SWP_MOI_DEV_IMA_GRP_T;

/* MOI -- IMA链路 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucFrameNo;      /* 机架编号(1..5) */
    UINT8   ucShelfNo;      /* 机框编号(1..3) */
    UINT8   ucSlotNo;       /* 槽位编号(1..14) */
    UINT8   ucImaGrpId;     /* IMA组ID，范围:EAIB板0~15；CAIB板 0~83 */
    UINT8   ucImaLnkNo;     /* IMA链路号 0~63*/
} SWP_MOI_DEV_IMA_LNK_T;


/* MOI -- 资源模块 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usReserve;      /* 为usGroupNo字段保留（固定填0） */
    UINT8   ucModuleNo;     /* 模块编号(1..255) */
} SWP_MOI_RES_MODULE_T;

/* MOI -- 资源组 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usGroupNo;      /* 组编号(1..65535) */
    UINT8   ucModuleNo;     /* 模块编号(1..255) */
} SWP_MOI_RES_GROUP_T;

/* MOI -- 资源子组 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usGroupNo;      /* 组编号(1..65535) */
    UINT8   ucModuleNo;     /* 模块编号(1..255) */
    UINT8   ucSubGrpNo;     /* 子组编号(1..255) */
       /*  子组编号
       *    1..4        通用CPU, 其中主CPU固定为1;从CPU取值为2..4;
       *    5..31       DSP
       *    32..95      E1
       *    96..100     光口
       *    101..132    HW
       *    133..195    网络端口
       */
} SWP_MOI_RES_SUBGRP_T;

/* MOI -- 版本库 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usVerLibId;     /* 版本库标识，在创建对象时，从1开始递增顺序生成 */
} SWP_MOI_VRTL_VER_LIB_T;

/* MOI -- 运行版本配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usRunVerId;     /* 运行版本库标识，在创建对象时，从1开始递增顺序生成 */
} SWP_MOI_VRTL_VER_RUN_T;

/* MOI -- 指定版本配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usSpecVerId;    /* 指定版本id, 在创建对象时，从1开始递增顺序生成 */
} SWP_MOI_VRTL_VER_SPEC_T;

/* MOI -- 物理板组合 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usBrdSetId;     /* 物理版组合标识，在创建对象时，从1开始递增顺序生成 */
} SWP_MOI_VRTL_BRD_SET_T;

/* MOI -- 单板软件包 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usBrdSwPkgId;   /* 单板软件包标识，在创建对象时，从1开始递增顺序生成 */
} SWP_MOI_VRTL_BRD_SWPKG_T;

/* MOI -- 单板软件包文件 */
typedef struct
{
    UINT16  usNeElement;    /*网元标识 */
    UINT16  usBrdSwPkgFilesId;  /*单板软件包文件标识，在创建对象时，从1开始递增顺序生成*/
} SWP_MOI_VRTL_BRD_SWPKG_FILES_T;

/* MOI -- 计数器组 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usCounterGroup; /* 计数器组号 */
} SWP_MOI_VRTL_COUNTER_GRP_T;

/* MOI -- 测量对象 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usCounterGroup; /* 计数器组号 */
    UINT16  usMeasObjId;    /* 测量对象标识，在创建对象时，从1开始递增顺序生成 */
} SWP_MOI_VRTL_MEAS_OBJ_T;

/* MOI -- 测量过滤配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usCounterGroup; /* 计数器组号 */
    UINT16  usMeasFltrId;   /* 测量过滤标识，在创建对象时，从1开始递增顺序生成 */
} SWP_MOI_VRTL_MEAS_FLTR_T;

/* MOI -- 派生计数器组 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usDrvGrpNo;     /* 派生计数器组号 */
} SWP_MOI_VRTL_DRV_GRP_T;

/* MOI -- 派生计数器组计算规则 */   
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usDrvGrpNo;     /* 派生计数器组号 */
    UINT16  usDrvCntIdx;    /* 派生计数器索引号 */
} SWP_MOI_VRTL_DRV_METHOD_T;

/* MOI -- 网同步配置 */ 
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucNscaId;       /* 网同步表标识，在创建对象时，从1开始递增顺序生成 */
} SWP_MOI_VRTL_MEAS_NSCA_T;

/* MOI -- NTP服务器配置 */ 
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucNtpId;        /* 网同步表标识，在创建对象时，从1开始递增顺序生成 */
} SWP_MOI_VRTL_NTP_T;

/* MOI -- 告警信息 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usAlarmInfoId;  /* 告警信息标识，在创建对象时，从1开始递增顺序生成 */
} SWP_MOI_VRTL_ALARM_INFO_T;

/* MOI -- 自定义定时器配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucTimerType;    /* 定时器类型 */
} SWP_MOI_VRTL_PRIVATETIMER_T;


/* MOI -- MML用户信息配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucRecorderId;   /* 记录标识，在创建对象时，从1开始递增顺序生成 */
} SWP_MOI_VRTL_USER_T;

/* MOI -- 日志配额信息配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucRecorderId;   /* 记录标识，由OMC创建固定为1 */
} SWP_MOI_VRTL_LOG_T;

/* MOI -- 风扇配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucRecorderId;   /* 记录标识，由OMC创建固定为1 */
} SWP_MOI_VRTL_FAN_T;

/* MOI -- 处理器过载门限配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usRecorderId;   /* 记录标识，由OMC创建，从1开始递增顺序生成 */
} SWP_MOI_VRTL_CPU_LOAD_T;

/* MOI -- 单板过载门限配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucGroupType;    /* 功能单板类型 */
} SWP_MOI_VRTL_BOARD_LOAD_T;

/* MOI -- PVC */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usPvcId;        /* PVC标识(1..8192) */
} SWP_MOI_ATM_PVC_T;

/* MOI -- ATM邻接局 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usAni;          /* ATM邻接局号 */
} SWP_MOI_ATM_OFFICE_T;

/* MOI -- ATM信令链路 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usAni;          /* ATM邻接局号 */
    UINT16  usPvcId;        /* PVC标识(1..8192) */
} SWP_MOI_ATM_SIGLNK_T;

/* MOI -- AAL2通道 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usAni;          /* ATM邻接局号 */
    UINT32  ulPathId;       /* ALL2通道标识 */
} SWP_MOI_ATM_AAL2_PATH_T;

/* MOI -- IPOA链路 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usPvcId;        /* PVC标识(1..8192) */
} SWP_MOI_ATM_IPOA_T;

/* MOI -- ATM IU信令链路 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usPvcId;        /* PVC标识(1..8192) */
} SWP_MOI_ATM_IU_SIGLNK_T;

/* MOI -- IP端口 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usPortRecordId; /* 端口标识，OMC在创建对象时，从1开始递增顺序生成 */
} SWP_MOI_IP_IPSTACK_IPPORT_T;

/* MOI -- L2TP连接 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usPortRecordId; /* 端口标识 */
    UINT32  ulL2tpId;       /* L2TP连接标识, 来源于本表的ulL2tpId字段 */
} SWP_MOI_IP_IPSTACK_L2TP_T;

/* MOI -- SCTP流 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usPortRecordId; /* 端口标识 */
    UINT32  ulSctpId;       /* SCTP流标识，从1开始递增顺序生成, OMC在创建对象时自动生成 */
} SWP_MOI_IP_IPSTACK_SCTP_T;

/* MOI -- 访问控制列表 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usPortRecordId; /* 端口标识 */
    UINT16  usAclId;        /* 访问控制列表标识, 来源于本表的usAclId字段 */
} SWP_MOI_IP_IPSTACK_ACL_T;

/* MOI -- 安全关联配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usPortRecordId; /* 端口标识 */
    UINT16  usAclId;        /* 访问控制列表标识 */
    UINT8   ucSadId;        /* 安全关联对象标识(固定为1), OMC在创建对象时自动生成 */
} SWP_MOI_IP_IPSTACK_SAD_T;

/* MOI -- 业务注册对象 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usPortRecordId; /* 端口标识 */
    UINT16  usServiceId;    /* 业务标识, 来源于本表的usServiceId字段 */
} SWP_MOI_IP_IPSTACK_SERVICEREG_T;

/* MOI -- 隧道 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usPortRecordId; /* 端口标识 */
    UINT32  ulTunnelId;     /* 隧道标识(1..200), 来源于本表的ulTunnelId字段 */
} SWP_MOI_IP_IPSTACK_TUNNEL_T;

/* MOI -- DSCP BA分类 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usFlowId;       /* 流标识, 本表的usFlowId字段 */
} SWP_MOI_IP_IPSTACK_DSCPBA_T;

/* MOI -- DSCP MF分类 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usFlowId;       /* 流标识, 本表的usFlowId字段 */
} SWP_MOI_IP_IPSTACK_DSCPMF_T;

/* MOI -- DSCP 端口与队列调度 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usScheduleId;   /* 调度标识, 本表的usScheduleId字段 */
} SWP_MOI_IP_IPSTACK_DSCPSCHED_T;

/* MOI -- DSCP 队列流控 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usQueueId;      /* 队列标识, 本表的usQueueID字段 */
} SWP_MOI_IP_IPSTACK_DSCPWRED_T;

/* MOI -- IKE协议控制 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucIkeCtrlId;    /* IKE协议控制配置对象标识(固定为1), OMC在创建对象时自动生成 */
} SWP_MOI_IP_IPSTACK_IKECTRL_T;

/* MOI -- IKE协议配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucIkeCfgId;     /* IKE协议配置对象标识(固定为1), OMC在创建对象时自动生成 */
} SWP_MOI_IP_IPSTACK_IKECFG_T;

/* MOI -- 外部服务器 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usServerId;     /* 外部服务器标识, 本表的usServerId字段 */
} SWP_MOI_IP_IPSTACK_SERVER_T;

/* MOI -- 域名表项 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usReserve;      /* 为了4字节对齐，保留字段 */
    UINT32  ulDnsEntryId;   /* 域名表项标识（从1开始递增顺序生成）, OMC在创建对象时自动生成 */
} SWP_MOI_IP_IPSTACK_DNSENTRY_T;

/* MOI -- NATP表项 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usReserve;      /* 为了4字节对齐，保留字段 */
    UINT32  ulNatpId;       /* NATP标识(1..8000), 本表的ulNatpId字段 */
} SWP_MOI_IP_IPSTACK_NATP_T;

/* MOI -- ARP表项 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usReserve;      /* 为了4字节对齐，保留字段 */
    UINT32  ulArpId;        /* ARP标识（从1开始递增顺序生成）, OMC在创建对象时自动生成 */
} SWP_MOI_IP_IPSTACK_ARP_T;

/* MOI -- GTPU协议配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucGtpuCfgId;    /* GTPU协议配置对象标识(固定为1), OMC在创建对象时自动生成 */
} SWP_MOI_IP_IPSTACK_GTPUCFG_T;

/* MOI -- PPP协议配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usReserve;
    UINT32  ulIfIndex;      /* 接口索引 */
} SWP_MOI_IP_IPSTACK_PPPCFG_T;

/* MOI -- PPP链路端口配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usPPPPortId;    /* PPP链路端口配置对象标识, OMC在创建对象时从1开始递增顺序生成 */
    UINT32  ulIfIndex;      /* 接口索引 */
} SWP_MOI_IP_IPSTACK_PPPPORT_T;

/* MOI -- NCP配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usIpNcpCfgId;   /* NCP配置对象标识, OMC在创建对象时从1开始递增顺序生成 */
    UINT32  ulIfIndex;      /* 接口索引 */
} SWP_MOI_IP_IPSTACK_NCPCFG_T;

/* MOI -- IP局向 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usRecorderId;   /* 记录号,在创建对象时,从1开始递增顺序生成 */
} SWP_MOI_IP_IPSTACK_OFF_T;

/* MOI -- IUB局向SCTP偶联 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usSctpAssoId;   /* SCTP偶联标识 */
} SWP_MOI_IUB_SIGTRAN_SCTP_ASSO_T;

/* MOI -- IPV4静态路由 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usReserve;      /* 为了4字节对齐，保留字段 */
    UINT32  ulSRouteId;     /* 静态路由标识, 本表的ulSRouteId字段 */
} SWP_MOI_IP_RTMNG_SV4ROUTE_T;

/* MOI -- IPV6静态路由 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usReserve;      /* 为了4字节对齐，保留字段 */
    UINT32  ulSRouteId;     /* 静态路由标识, 本表的ulSRouteId字段 */
} SWP_MOI_IP_RTMNG_SV6ROUTE_T;

/* MOI -- OSPF V2协议配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucOspfV2ParaId; /* OSPF V2协议配置对象标识(固定为1), OMC在创建对象时自动生成 */
} SWP_MOI_IP_RTMNG_OSPFV2PARA_T;

/* MOI -- OSPF V2路由区 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usOspfAreaId;   /* 路由区标识, 本表的ulOspfAreaId字段 */
} SWP_MOI_IP_RTMNG_OSPFV2AREA_T;

/* MOI -- OSPF V2接口 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usOspfAreaId;   /* 路由区标识 */
    UINT16  usOspfIfId;     /* 接口标识, 本表的ulOspfIfId字段 */
} SWP_MOI_IP_RTMNG_OSPFV2ITF_T;

/* MOI -- OSPF V2邻居 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usOspfAreaId;   /* 路由区标识 */
    UINT16  usOspfIfId;     /* 接口标识 */
    UINT16  usOspfNbrId;    /* 邻居标识, 本表的ulOspfNbrId字段 */
} SWP_MOI_IP_RTMNG_OSPFV2NBR_T;

/* MOI -- OSPF V3协议配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucOspfV3ParaId; /* OSPF V3协议配置对象标识(固定为1), OMC在创建对象时自动生成 */
} SWP_MOI_IP_RTMNG_OSPFV3PARA_T;

/* MOI -- OSPF V3路由区 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usOspfAreaId;   /* 路由区标识, 本表的ulOspfAreaId字段 */
} SWP_MOI_IP_RTMNG_OSPFV3AREA_T;

/* MOI -- OSPF V3接口 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usOspfAreaId;   /* 路由区标识 */
    UINT16  usOspfIfId;     /* 接口标识, 本表的ulOspfIfId字段 */
} SWP_MOI_IP_RTMNG_OSPFV3ITF_T;

/* MOI -- OSPF V3邻居 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usOspfAreaId;   /* 路由区标识 */
    UINT16  usOspfIfId;     /* 接口标识 */
    UINT16  usOspfNbrId;    /* 邻居标识, 本表的ulOspfNbrId字段 */
} SWP_MOI_IP_RTMNG_OSPFV3NBR_T;

/* MOI -- RIP协议配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucRipParaId;    /* RIP协议配置对象标识(固定为1), OMC在创建对象时自动生成 */
} SWP_MOI_IP_RTMNG_RIPPARA_T;

/* MOI -- RIP接口 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usReserve;
    UINT32  ulIfIndex;      /* 接口索引 */
} SWP_MOI_IP_RTMNG_RIPITF_T;

/* MOI -- RIP邻居 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usRipNbrId;     /* RIP邻居标识, 本表的usRipNbrId字段 */
    UINT32  ulIfIndex;      /* 接口索引 */
} SWP_MOI_IP_RTMNG_RIPNBR_T;

/* MOI -- RIPNG协议配置 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucRipngParaId;  /* RIPNG协议配置对象标识(固定为1)，OMC在创建对象时自动生成 */
} SWP_MOI_IP_RTMNG_RIPNGPARA_T;

/* MOI -- RIPNG接口 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usReserve;
    UINT32  ulIfIndex;      /* 接口索引 */
} SWP_MOI_IP_RTMNG_RIPNGITF_T;

/* MOI -- RIPNG邻居 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usRipngNbrId;    /* RIPNG对象标识, OMC在创建对象时自动生成, OMC内部属性 */
    UINT32  ulIfIndex;      /* RIP接口索引 */
} SWP_MOI_IP_RTMNG_RIPNGNBR_T;

/* MOI -- MTP2信令链路 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT16  usPhyLinkId;    /* 物理链路号(1..65535) */
} SWP_MOI_N7_ATM_SIGLNK_T;

/* MOI -- 本局信令点 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucLocNetType;   /* 本局网络类型 */
} SWP_MOI_N7_LOC_OFFICE_T;

/* MOI -- N7邻接局 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucLocNetType;   /* 本局网络类型，在创建对象时根据父对象的取值设置*/
    UINT8   ucReserve;      /* 为了字节对齐引入保留字段（固定填0） */
    UINT16  usAdjOfficeId;  /* N7局向号(1..1024) */
} SWP_MOI_N7_ADJ_OFFICE_T;

/* MOI -- 信令链路组 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucLocNetType;   /* 本局网络类型，在创建对象时根据父对象的取值设置*/
    UINT8   ucReserve;      /* 为了字节对齐引入保留字段（固定填0） */
    UINT16  usLnkSetId;     /* 信令链路组号(1..255) */
} SWP_MOI_N7_SIGLNK_SET_T;

/* MOI -- 信令链路 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucLocNetType;   /* 本局网络类型，在创建对象时根据父对象的取值设置*/
    UINT8   ucReserve;      /* 为了字节对齐引入保留字段（固定填0） */
    UINT16  usLnkSetId;     /* 信令链路组号(1..255) */
    UINT16  usLnkId;        /* 信令链路ID(1..4080) */
} SWP_MOI_N7_SIGLNK_T;

/* MOI -- 局向信令路由 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucLocNetType;   /* 本局网络类型，在创建对象时根据父对象的取值设置*/
    UINT8   ucReserve;      /* 为了字节对齐引入保留字段（固定填0） */
    UINT16  usAdjOfficeId;  /* 邻局向编号(1..1024) */
    UINT8   ucOffRouteId;   /* 局向信令路由ID，创建对象时由OMC自动生成(1..1) */
} SWP_MOI_N7_OFFICE_RT_T;

/* MOI -- 信令静态路由 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucLocNetType;   /* 本局网络类型，在创建对象时根据父对象的取值设置*/
    UINT8   ucReserve;      /* 为了字节对齐引入保留字段（固定填0） */
    UINT16  usRouteId;      /* 路由编号(1..255) */
} SWP_MOI_N7_STATIC_RT_T;

/* MOI -- SCCP子系统 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucLocNetType;   /* 本局网络类型，在创建对象时根据父对象的取值设置*/
    UINT8   ucReserve;      /* 为了字节对齐引入保留字段（固定填0） */
    UINT16  usAdjOfficeId;  /* 邻局向编号(1..1024) */
    UINT8   ucSSN;          /* SCCP子系统编号 */
} SWP_MOI_N7_SCCP_SSN_T;

/* MOI -- GT翻译选择 */
typedef struct
{
    UINT16   usNeElement;       /* 网元标识 */
    UINT16   usGTTranslatorId;  /* GT翻译选择ID(1..1024)，由创建GT翻译选择对象时，
                                 * 从1开始递增顺序生成 
                                 */
} SWP_MOI_N7_GT_TRANSLATOR_T;

/* MOI -- GT翻译号码 */
typedef struct
{
    UINT16   usNeElement;    /* 网元标识 */
    UINT16   usGTNodeId;     /* GT翻译号码对象ID(1..1000)，由创建GT翻译号码对象时，
                              * 从1开始递增顺序生成 
                              */
} SWP_MOI_N7_GT_NODE_T;

/* MOI -- SCCP屏蔽功能 */
typedef struct
{
    UINT16   usNeElement;    /* 网元标识 */
    UINT16   usSccpShieldId; /* SCCP屏蔽功能ID，1..255，由创建SSCP屏蔽功能对象时，
                              * 从1开始递增顺序生成 
                              */
} SWP_MOI_N7_SCCP_SHIELD_T;

/* MOI -- MTP屏蔽功能 */
typedef struct
{
    UINT16   usNeElement;    /* 网元标识 */
    UINT16   usMtpShieldId;  /* MTP屏蔽功能ID，1..255，由创建MTP屏蔽功能对象时，
                              * 从1开始递增顺序生成 
                              */
} SWP_MOI_N7_MTP_SHIELD_T;

/* MOI -- TNSS定时器 */
typedef struct
{
    UINT16   usNeElement;    /* 网元标识 */
    UINT16   usReseverd;     /* 保留字段 */
    UINT32   ulTimerNo;      /* 定时器编号 */
} SWP_MOI_N7_TIMER_T;

/* MOI -- 本局SCCP子系统 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucLocNetType;   /* 本局网络类型,在创建对象时根据父对象的取值设置 */
    UINT8   ucSSN;          /* SCCP子系统编号 */
} SWP_MOI_N7_LOC_SCCP_SSN_T;

/* MOI -- M3UA应用服务器 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucLocNetType;   /* 本局网络类型，在创建对象时根据父对象的取值设置*/
    UINT8   ucReserve;      /* 为了字节对齐引入保留字段（固定填0） */
    UINT16  usAdjOfficeId;  /* 邻局向编号(1..1024) */
    UINT16  usAsId;         /* AS标识 */
} SWP_MOI_SIGTRAN_AS_T;

/* MOI -- M3UA应用服务器定位信息 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucLocNetType;   /* 本局网络类型，在创建对象时根据父对象的取值设置*/
    UINT8   ucAsLocId;      /* M3UA应用服务器定位信息ID(1..255)，从1开始顺序递增 */
    UINT16  usAdjOfficeId;  /* 邻局向编号(1..1024) */
    UINT16  usAsId;         /* AS标识 */
} SWP_MOI_SIGTRAN_AS_LOC_T;

/* MOI -- M3UA应用服务器进程 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucLocNetType;   /* 本局网络类型，在创建对象时根据父对象的取值设置*/
    UINT8   ucReserve;      /* 为了字节对齐引入保留字段（固定填0） */
    UINT16  usAdjOfficeId;  /* 邻局向编号(1..1024)，根据父对象的取值设置*/
    UINT16  usAspId;        /* ASP标识 */
} SWP_MOI_SIGTRAN_ASP_T;

/* MOI -- SCTP偶联 */
typedef struct
{
    UINT16  usNeElement;    /* 网元标识 */
    UINT8   ucLocNetType;   /* 本局网络类型，在创建对象时根据父对象的取值设置*/
    UINT8   ucReserve;      /* 为了字节对齐引入保留字段（固定填0） */
    UINT16  usAdjOfficeId;  /* 邻局向编号(1..1024)， 根据父对象的取值设置*/
    UINT16  usSctpAssoId;   /* SCTP偶联标识 */
} SWP_MOI_SIGTRAN_SCTP_ASSO_T;

#if 0
typedef struct
{
	UINT32  ulNeElement;    /* 网元标识 */
}SWP_MOI_AP_NEID_T;

typedef struct
{
	UINT32  ulNeElement;    /* 网元标识 */
	UINT8   ucRadioId;      /*Radio id*/
	UINT8   ucChannelId;
}SWP_MOI_AP_RADIO_T;
typedef struct
{
	UINT32  ulNeElement;    /* 网元标识 */
	UINT8   ucRadioId;      /*Radio id*/
//	UINT8   ucChannelId;
}SWP_MOI_AP_CHANNEL_T;

typedef struct
{
	UINT32  ulNeElement;    /* 网元标识 */
	UINT8   ucRadioId;      /*Radio id*/
	UINT8   stStaMac[PUBLIC_MAC_LENTH];
}SWP_MOI_AP_STA_T;

typedef struct
{
	UINT32  ulNeElement;    /* 网元标识 */
}SWP_MOI_AC_NEID_T;
typedef struct
{
	UINT32  ulNeElement;    /* 网元标识 */
	UINT8   stApMac[PUBLIC_MAC_LENTH];
	UINT8   stStaMac[PUBLIC_MAC_LENTH];
}SWP_MOI_STA_T;
typedef struct
{
	UINT8 stApMac[PUBLIC_MAC_LENTH];
}SWP_MOI_AP_MAC_T;
typedef struct
{
	UINT32  ulNeElement;    /* 网元标识 */
	UINT8   stApMac[PUBLIC_MAC_LENTH];
    UINT8   ucApStatus;     /*ap 状态*/
}SWP_MOI_AP_STATUS_T;

#endif

typedef struct
{
	UINT8 aucApMac[PUBLIC_MAC_LENTH];
}SWP_MOI_AP_T;

typedef struct
{
	UINT8   aucApMac[PUBLIC_MAC_LENTH];
	UINT8   ucRadioId;      /*Radio id*/
}SWP_MOI_AP_RADIO_T;

typedef struct
{
	UINT8 aucApMac[6];/*AP MAC*/
	UINT8   ucRadioId;
	UINT16 usWlanProfileId;
	UINT8 aucSSIDName[PUBLIC_NAME_LENTH];
	UINT8 aucBSSID [PUBLIC_MAC_LENTH];/*WlanMac*/
}SWP_MOI_AP_WLAN_T;

typedef struct
{
	UINT8   aucApMac[PUBLIC_MAC_LENTH];
	UINT8   ucRadioId;      /*Radio id*/
	UINT16   usChannelId;
}SWP_MOI_AP_CHANNEL_T;

typedef struct
{
	UINT8 aucApMac[PUBLIC_MAC_LENTH];/*AP MAC*/
	UINT8   ucRadioId;
	UINT16 usWlanProfileId;
	UINT8 aucSSIDName[PUBLIC_NAME_LENTH];
	UINT8 aucBSSID [PUBLIC_MAC_LENTH];
	UINT8 aucStaMac[PUBLIC_MAC_LENTH];/*终端Mac*/
}SWP_MOI_STA_T;

typedef struct
{
	UINT8   aucIp[PUBLIC_IPV4_LENTH];
	UINT16   usPort;
}SWP_MOI_RADIUS_T;

typedef struct
{
	UINT8 aucIpPoolName[PUBLIC_NAME_LENTH];
}SWP_MOI_IPPOOL_T;

#if 0

typedef struct
{
	UINT8   aucApMac[PUBLIC_MAC_LENTH];
	UINT16  usWlanProId;
	UINT8   ucRadioId;      /*Radio id*/
	UINT8   aucRev[3];
}SWP_MOI_WLAN_T;

typedef struct
{
	UINT8   ucFrameNo;      /* 机架编号(1..1) */
	UINT8   ucShelfNo;      /* 机框编号(1..1) */
    UINT8   ucPowerNo;      /* 电源编号 (1..2)*/
}SWP_MOI_AC_POWER_T ;
#endif

/*******************************************************************************
*                               MO_ID 结构                                     *
*******************************************************************************/
/* MOI */
typedef union
{
    SWP_MOI_GLOBAL_T                stGlobal;       /* 全局数据 */
    SWP_MOI_DEV_FRAME_T             stFrame;        /* 机架 */
    SWP_MOI_DEV_SHELF_T             stShelf;        /* 机框 */
    SWP_MOI_DEV_POWER_T             stPower;        /* 电源 */
    SWP_MOI_DEV_SHELF_MNG_T         stShelfMng;     /* 机框管理器 */
    SWP_MOI_DEV_SHELF_FAN_BOX_T     stShelfFanBox;  /* 风扇盒 */
    SWP_MOI_DEV_SHELF_FAN_T         stShelfFan;     /* 风扇 */
    SWP_MOI_DEV_BOARD_T             stBoard;        /* 单板 */
    SWP_MOI_DEV_CPU_T               stCpu;          /* 处理器 */
    SWP_MOI_DEV_DSP_T               stDsp;          /* DSP */
    SWP_MOI_DEV_E1_PORT_T           stE1Port;       /* E1端口 */ 
    SWP_MOI_DEV_EXT_CLOCK_T         stExtClock;     /* 外部时钟源 */
    SWP_MOI_DEV_HW_PORT_T           stHwPort;       /* HW端口 */
    SWP_MOI_DEV_ETH_PORT_T          stEthPort;      /* 网络端口 */
    SWP_MOI_DEV_ALARM_BOX_T         stAlarmBox;     /* 告警箱 */
    SWP_MOI_ATM_CLK_EXTRACT_T       stAtmClkExtract;/* 端口线路时钟配置 */
    SWP_MOI_DEV_APS_GRP_T           stApsGrp;       /* APS光口组 */
    SWP_MOI_DEV_APS_OMAPE1_T        stApsOmapE1;    /* 通道化E1链路 */
    SWP_MOI_DEV_ATM_PORT_T          stAtmPort;      /* ATM端口 */
    SWP_MOI_DEV_ATM_SAR_T           stAtmSar;       /* ATM SAR配置 */
    SWP_MOI_DEV_STM1_PORT_T         stStm1Port;     /* STM1光口 */
    SWP_MOI_DEV_MAC_CHIP_CFG_T      stMacChipCfg;   /* MAC芯片配置 */
    SWP_MOI_DEV_APS_PORT_INTFSYS_T  stApsPortIntfsys;/* STM1参数INTFSYS */
    SWP_MOI_DEV_APS_PORT_SOH_T      stApsPortSoh;    /* STM1参数SOH */
    SWP_MOI_DEV_APS_PORT_LOH_T      stApsPortLoh;    /* STM1参数LOH */
    SWP_MOI_DEV_APS_PORT_POH_T      stApsPortPoh;    /* STM1参数POH */
    SWP_MOI_DEV_APS_PORT_PYLD_T     stApsPortPyld;   /* STM1参数PYLD */
    SWP_MOI_DEV_APS_PORT_INTFLINE_T stApsPortIntfline;   /* STM1参数INTFLINE */
    SWP_MOI_DEV_APS_PORT_J0S1_T     stApsPortJ0s1;  /* STM1参数J0S1 */
    SWP_MOI_DEV_APS_PORT_J1C2_T     stApsPortJ1c2;  /* STM1参数J1C2 */
    SWP_MOI_DEV_IMA_GRP_T           stImaGrp;       /* IMA组 */
    SWP_MOI_DEV_IMA_LNK_T           stImaLnk;       /* IMA链路 */

    SWP_MOI_RES_MODULE_T            stResModule;    /* 资源模块 */
    SWP_MOI_RES_GROUP_T             stResGroup;     /* 资源组 */
    SWP_MOI_RES_SUBGRP_T            stResSubGrb;    /* 资源子组 */
    
    SWP_MOI_VRTL_VER_LIB_T          stVrtlVerLib;   /* 版本库 */
    SWP_MOI_VRTL_VER_RUN_T          stVrtlVerRun;   /* 运行版本配置 */
    SWP_MOI_VRTL_VER_SPEC_T         stVrtlVerSpec;  /* 指定版本配置 */  
    SWP_MOI_VRTL_BRD_SET_T          stVrtlBrdSet;   /* 物理板组合 */
    SWP_MOI_VRTL_BRD_SWPKG_T        stVrtlBrdSwPkg; /* 单板软件包 */
    SWP_MOI_VRTL_BRD_SWPKG_FILES_T  stVrtlBrdSwPkgFiles;    /* 单板软件包文件 */
    SWP_MOI_VRTL_COUNTER_GRP_T      stVrtlCounterGrp;   /* 计数器组 */   
    SWP_MOI_VRTL_MEAS_OBJ_T         stVrtlMeasObj;  /* 测量对象 */    
    SWP_MOI_VRTL_MEAS_FLTR_T        stVrtlMeasFltr; /* 测量过滤配置 */
    SWP_MOI_VRTL_DRV_GRP_T          stVrtlDrvGrp;   /* 派生计数器组 */
    SWP_MOI_VRTL_DRV_METHOD_T       stVrtlDrvMethod;    /* 派生计数器组计算规则 */
    SWP_MOI_VRTL_MEAS_NSCA_T        stVrtlMeasNsca; /* 网同步配置 */
    SWP_MOI_VRTL_NTP_T              stVrtlNtp;      /* NTP服务器配置 */
    SWP_MOI_VRTL_ALARM_INFO_T       stVrtlAlarmInfo;/* 告警信息 */
    SWP_MOI_VRTL_PRIVATETIMER_T     stVrtlPrivateTimer; /* 自定义定时器配置 */ 
    SWP_MOI_VRTL_USER_T             stVrtlUser;         /* MML用户信息配置 */
    SWP_MOI_VRTL_LOG_T              stVrtlLog;          /* 日志配额信息配置 */
    SWP_MOI_VRTL_FAN_T              stVrtlFan;          /* 风扇配置 */
    SWP_MOI_VRTL_CPU_LOAD_T         stVrtlCpuLoad;      /* 处理器过载门限配置 */
    SWP_MOI_VRTL_BOARD_LOAD_T       stVrtlBoardLoad;    /* 单板过载门限配置 */
    
    SWP_MOI_ATM_PVC_T               stAtmPvc;       /* PVC */
    SWP_MOI_ATM_OFFICE_T            stAtmOffice;    /* ATM邻接局 */
    SWP_MOI_ATM_SIGLNK_T            stAtmSigLnk;    /* ATM信令链路 */
    SWP_MOI_ATM_IPOA_T              stAtmIpoaLnk;   /* IPOA链路 */
    SWP_MOI_ATM_IU_SIGLNK_T         stAtmIuSigLnk;  /* ATM IU信令链路 */
    SWP_MOI_ATM_AAL2_PATH_T         stAtmAal2Path;  /* AAL2通道 */
    SWP_MOI_IP_IPSTACK_IPPORT_T     stIpStackIpPort;    /* IP端口表 */
    SWP_MOI_IP_IPSTACK_L2TP_T       stIpStackL2tp;      /* L2TP连接 */
    SWP_MOI_IP_IPSTACK_SCTP_T       stIpStackSctp;      /* SCTP流 */
    SWP_MOI_IP_IPSTACK_ACL_T        stIpStackAcl;       /* 访问控制列表 */
    SWP_MOI_IP_IPSTACK_SAD_T        stIpStackSad;       /* 安全关联配置 */ 
    SWP_MOI_IP_IPSTACK_SERVICEREG_T stIpStackServiceReg;/* 业务注册对象 */
    SWP_MOI_IP_IPSTACK_TUNNEL_T     stIpStackTunnel;    /* 隧道 */
    SWP_MOI_IP_IPSTACK_DSCPBA_T     stIpStackDscpBa;    /* DSCP BA分类 */
    SWP_MOI_IP_IPSTACK_DSCPMF_T     stIpStackDscpMf;    /* DSCP MF分类 */
    SWP_MOI_IP_IPSTACK_DSCPSCHED_T  stIpStackDscpSched; /* DSCP 端口与队列调度 */
    SWP_MOI_IP_IPSTACK_DSCPWRED_T   stIpStackDscpWred;  /* DSCP 队列流控 */
    SWP_MOI_IP_IPSTACK_IKECTRL_T    stIpStackIkeCtrl;   /* IKE协议控制 */
    SWP_MOI_IP_IPSTACK_IKECFG_T     stIpStackIkeCfg;    /* IKE协议配置 */
    SWP_MOI_IP_IPSTACK_SERVER_T     stIpStackServer;    /* 外部服务器 */
    SWP_MOI_IP_IPSTACK_DNSENTRY_T   stIpStackDnsentry;  /* 域名表项 */
    SWP_MOI_IP_IPSTACK_NATP_T       stIpStackNatp;      /* NATP表项 */
    SWP_MOI_IP_IPSTACK_ARP_T        stIpStackArp;       /* ARP表项 */ 
    SWP_MOI_IP_IPSTACK_GTPUCFG_T    stIpStackGtpuCfg;   /* GTPU协议配置 */
    SWP_MOI_IP_IPSTACK_PPPCFG_T     stIpStackPPPCfg;    /* PPP协议配置 */
    SWP_MOI_IP_IPSTACK_PPPPORT_T    stIpStackPPPPort;   /* PPP链路端口配置 */
    SWP_MOI_IP_IPSTACK_NCPCFG_T     stIpStackNcpCfg;    /* NCP配置 */
    SWP_MOI_IP_IPSTACK_OFF_T        stIpStackOff;       /* IP局向 */
    SWP_MOI_IUB_SIGTRAN_SCTP_ASSO_T stSctpAsso;         /* IUB局向SCTP偶联 */
        
    SWP_MOI_IP_RTMNG_SV4ROUTE_T     stIpRtmngSv4Route;  /* IPV4静态路由 */
    SWP_MOI_IP_RTMNG_SV6ROUTE_T     stIpRtmngSv6Route;  /* IPV6静态路由 */
    SWP_MOI_IP_RTMNG_OSPFV2PARA_T   stIpRtmngOspfv2Para;/* OSPF V2协议配置 */
    SWP_MOI_IP_RTMNG_OSPFV2AREA_T   stIpRtmngOspfv2Area;/* OSPF V2路由区 */
    SWP_MOI_IP_RTMNG_OSPFV2ITF_T    stIpRtmngOspfv2Itf; /* OSPF V2接口 */
    SWP_MOI_IP_RTMNG_OSPFV2NBR_T    stIpRtmngOspfv2Nbr; /* OSPF V2邻居 */
    SWP_MOI_IP_RTMNG_OSPFV3PARA_T   stIpRtmngOspfv3Para;/* OSPF V3协议配置 */
    SWP_MOI_IP_RTMNG_OSPFV3AREA_T   stIpRtmngOspfv3Area;/* OSPF V3路由区 */
    SWP_MOI_IP_RTMNG_OSPFV3ITF_T    stIpRtmngOspfv3Itf; /* OSPF V3接口 */
    SWP_MOI_IP_RTMNG_OSPFV3NBR_T    stIpRtmngOspfv3Nbr; /* OSPF V3邻居 */
    SWP_MOI_IP_RTMNG_RIPPARA_T      stIpRtmngRipPara;   /* RIP协议配置 */
    SWP_MOI_IP_RTMNG_RIPITF_T       stIpRtmngRipItf;    /* RIP接口 */
    SWP_MOI_IP_RTMNG_RIPNBR_T       stIpRtmngRipNbr;    /* RIP邻居 */
    SWP_MOI_IP_RTMNG_RIPNGPARA_T    stIpRtmngRipngPara; /* RIPNG协议配置 */
    SWP_MOI_IP_RTMNG_RIPNGITF_T     stIpRtmngRipngItf;  /* RIPNG接口 */
    SWP_MOI_IP_RTMNG_RIPNGNBR_T     stIpRtmngRipngNbr;  /* RIPNG邻居 */
    SWP_MOI_N7_ATM_SIGLNK_T         stN7AtmSigLnk;  /* MTP2信令链路 */
    SWP_MOI_N7_LOC_OFFICE_T         stN7LocOffice;  /* 本局信令点 */
    SWP_MOI_N7_ADJ_OFFICE_T         stN7AdjOffice;  /* N7邻接局 */
    SWP_MOI_N7_SIGLNK_SET_T         stN7SigLnkSet;  /* 信令链路组 */
    SWP_MOI_N7_SIGLNK_T             stN7SigLnk;     /* 信令链路 */
    SWP_MOI_N7_OFFICE_RT_T          stN7OfficeRt;   /* 局向信令路由 */
    SWP_MOI_N7_STATIC_RT_T          stN7StaticRt;   /* 信令静态路由 */
    SWP_MOI_N7_SCCP_SSN_T           stN7SccpSsn;    /* SCCP子系统 */
    SWP_MOI_N7_GT_TRANSLATOR_T      stN7GtTranslator;   /* GT翻译选择 */
    SWP_MOI_N7_GT_NODE_T            stN7GtNode;     /* GT翻译号码 */
    SWP_MOI_N7_SCCP_SHIELD_T        stN7SccpShield; /* SCCP屏蔽功能 */
    SWP_MOI_N7_MTP_SHIELD_T         stN7MtpShield;  /* MTP屏蔽功能 */ 
    SWP_MOI_N7_TIMER_T              stN7Timer;      /* TNSS定时器 */
    SWP_MOI_N7_LOC_SCCP_SSN_T       stN7Ssn;        /* 本局SCCP子系统 */
    SWP_MOI_SIGTRAN_AS_T            stSigTranAs;    /* M3UA应用服务器 */
    SWP_MOI_SIGTRAN_AS_LOC_T        stSigTranAsLoc; /* M3UA应用服务器定位信息 */
    SWP_MOI_SIGTRAN_ASP_T           stSigTranAsp;   /* M3UA应用服务器进程 */
    SWP_MOI_SIGTRAN_SCTP_ASSO_T     stSigTranSctpAsso;  /* SCTP偶联 */
    
#if 0
    SWP_MOI_AP_NEID_T              stApNeId;/*ap 网元id*/
    SWP_MOI_AP_RADIO_T             stApRadio;/*ap radio*/
    SWP_MOI_AC_NEID_T              stAcNeId;/*ac 网元id*/
    SWP_MOI_STA_T                    stSta;/*ap radio*/
    SWP_MOI_AP_MAC_T                stApMac;/*Ap Mac*/
    SWP_MOI_AP_STATUS_T             stApStatus;/*Ap Mac*/
    SWP_MOI_WLAN_T                  stWlan;/*Ap Mac*/
    SWP_MOI_PORTAL_T                stPortal;
    SWP_MOI_RADIUS_T                stRadius;
    SWP_MOI_AP_CHANNEL_T            stChannel;
    SWP_MOI_AP_STA_T                    stApSta;/*ap radio*/
    SWP_MOI_AC_POWER_T              stAcPower;/*Ac电源*/
#endif

    SWP_MOI_AP_T                    stAp;
    SWP_MOI_AP_RADIO_T              stRadio;
    SWP_MOI_AP_WLAN_T               stWlan;
    SWP_MOI_AP_CHANNEL_T            stChannel;
    SWP_MOI_STA_T                   stSta;
    SWP_MOI_RADIUS_T                stRadius;
    SWP_MOI_IPPOOL_T                stIpPool;

    UINT8   aucMoiMax[SWP_MOI_LEN];             /* 控制UNION大小的占位符 */
} SWP_MOI_U;

/* MO_ID */
typedef struct
{
    UINT32      ulMOC;
    SWP_MOI_U   unMOI;
} SWP_MO_ID_T;

/* 为了保持原来结构，增加一个通用MOID 结构 */
typedef  struct
{
    UINT32  ulMOC;
    UINT8   aucMOI[SWP_MOI_LEN];
} SWP_MO_ID_ORIGIN_T;



/*******************************************************************************
*                               函数声明                                       *
*******************************************************************************/



/*******************************************************************************
*                               头文件保护结尾                                 *
*******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* #ifdef SWP_MO_H */
/* 头文件结束, 末尾请保留一个空行 */

