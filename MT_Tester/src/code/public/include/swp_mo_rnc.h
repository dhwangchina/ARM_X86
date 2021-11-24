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
#ifndef SWP_MO_RNC_H
#define SWP_MO_RNC_H



#ifdef __cplusplus
extern "C" {
#endif

#include "swp_mo.h"



/*******************************************************************************
*                               宏和常量定义                                   *
*******************************************************************************/
/* MOC类型 */
/* RNC网元对象 */
#define RNC_MOC_RNC                 SWP_MOC_DEF(0x02200001) /* RNC网元         */
#define RNC_MOC_RNC_PLMN            SWP_MOC_DEF(0x02200002) /* RNC网元PLMN信息 */
/* Iu口外部网元层 */
#define RNC_MOC_ADJ_MSC             SWP_MOC_DEF(0x02201001) /* 邻接MSC    */
#define RNC_MOC_ADJ_SGSN            SWP_MOC_DEF(0x02201002) /* 邻接SGSN   */
#define RNC_MOC_ADJ_MSC_IPPATH      SWP_MOC_DEF(0x02201003) /* 邻接MSC IP PATH */
#define RNC_MOC_ADJ_SGSN_IPPATH     SWP_MOC_DEF(0x02201004) /* 邻接SGSN IP PATH */

/* Iur口外部网元层 */
#define RNC_MOC_ADJ_RNC             SWP_MOC_DEF(0x02201101) /* 邻接RNC             */
#define RNC_MOC_ADJ_RNC_PLMN        SWP_MOC_DEF(0x02201102) /* 邻接RNC网元PLMN信息 */
#define RNC_MOC_ADJ_RNC_MSC         SWP_MOC_DEF(0x02201103) /* 邻接RNC网元MSC信息  */
#define RNC_MOC_ADJ_RNC_SGSN        SWP_MOC_DEF(0x02201104) /* 邻接RNC网元SGSN信息 */
/* 本地RAN层 */
#define RNC_MOC_NODEB               SWP_MOC_DEF(0x02202001) /* NodeB        */
#define RNC_MOC_CELL                SWP_MOC_DEF(0x02202002) /* 小区         */
#define RNC_MOC_CELL_PLAN_PARA      SWP_MOC_DEF(0x02202003) /* 小区规划数据 */
#define RNC_MOC_CELL_SIB            SWP_MOC_DEF(0x02202004) /* 系统消息     */
#define RNC_MOC_CELL_CARRIER        SWP_MOC_DEF(0x02202005) /* 载波         */
#define RNC_MOC_TIME_SLOT           SWP_MOC_DEF(0x02202006) /* 时隙         */
#define RNC_MOC_HSPDSCH             SWP_MOC_DEF(0x02202007) /* HS-PDSCH集   */
#define RNC_MOC_HSSCCH              SWP_MOC_DEF(0x02202008) /* HS-SCCH信息  */
#define RNC_MOC_PCCPCH              SWP_MOC_DEF(0x02202009) /* PCCPCH       */
#define RNC_MOC_DWPCH               SWP_MOC_DEF(0x0220200A) /* DwPCH        */
#define RNC_MOC_PRACH_SET           SWP_MOC_DEF(0x0220200B) /* PRACH集      */
#define RNC_MOC_PRACH               SWP_MOC_DEF(0x0220200C) /* PRACH信道    */
#define RNC_MOC_FPACH               SWP_MOC_DEF(0x0220200D) /* FPACH信道    */
#define RNC_MOC_RACH                SWP_MOC_DEF(0x0220200E) /* RACH信道     */
#define RNC_MOC_SCCPCH_SET          SWP_MOC_DEF(0x0220200F) /* SCCPCH集     */
#define RNC_MOC_SCCPCH              SWP_MOC_DEF(0x02202010) /* SCCPCH信道   */
#define RNC_MOC_PICH                SWP_MOC_DEF(0x02202011) /* PICH信道     */
#define RNC_MOC_PCH                 SWP_MOC_DEF(0x02202012) /* PCH信道      */
#define RNC_MOC_FACH                SWP_MOC_DEF(0x02202013) /* FACH信道     */
#define RNC_MOC_CTRCH_RLC           SWP_MOC_DEF(0x02202014) /* 信道RLC信息  */
#define RNC_MOC_ADJ_CELL            SWP_MOC_DEF(0x02202015) /* 邻小区       */
#define RNC_MOC_NODEB_AAL2PATH      SWP_MOC_DEF(0x02202016) /* NodeB AAL2通道 */
#define RNC_MOC_NODEB_EQM           SWP_MOC_DEF(0x02202017) /* NodeB设备配置 */
#define RNC_MOC_NODEB_IPPATH        SWP_MOC_DEF(0x02202020) /* NodeB IP通道 */
#define RNC_MOC_NODEB_CELLGROUP     SWP_MOC_DEF(0x02202021) /* NodeB小区组配置 */
#define RNC_MOC_HSUPA_EAGCH         SWP_MOC_DEF(0x02202022) /* E-AGCH信息 */
#define RNC_MOC_HSUPA_EHICH         SWP_MOC_DEF(0x02202023) /* E-HICH信息 */
#define RNC_MOC_HSUPA_EPUCH         SWP_MOC_DEF(0x02202024) /* E-PUCH信息 */

/* 外部RAN层 */
#define RNC_MOC_OUT                 SWP_MOC_DEF(0x02202100) /* <外部RAN层>    */
#define RNC_MOC_OUT_TDD_128_CELL    SWP_MOC_DEF(0x02202101) /* 外部TDD128小区 */
#define RNC_MOC_OUT_TDD_384_CELL    SWP_MOC_DEF(0x02202102) /* 外部TDD384小区 */
#define RNC_MOC_OUT_FDD_CELL        SWP_MOC_DEF(0x02202103) /* 外部FDD小区    */
#define RNC_MOC_OUT_IS2000_CELL     SWP_MOC_DEF(0x02202104) /* 外部IS2000小区 */
#define RNC_MOC_OUT_GERAN_CELL      SWP_MOC_DEF(0x02202105) /* 外部GERAN小区  */
/* 小区算法 */
#define RNC_MOC_CELL_RRM            SWP_MOC_DEF(0x02202201) /* <小区算法层>   */
#define RNC_MOC_RRM_CAC             SWP_MOC_DEF(0x02202202) /* CAC算法        */
#define RNC_MOC_RRM_SDCA            SWP_MOC_DEF(0x02202203) /* SDCA算法       */
#define RNC_MOC_RRM_FDCA            SWP_MOC_DEF(0x02202204) /* FDCA算法       */
#define RNC_MOC_RRM_PS              SWP_MOC_DEF(0x02202205) /* PS算法         */
#define RNC_MOC_RRM_AMRC            SWP_MOC_DEF(0x02202206) /* AMRC算法       */
#define RNC_MOC_RRM_RLS             SWP_MOC_DEF(0x02202207) /* RLS算法        */
#define RNC_MOC_RRM_PC              SWP_MOC_DEF(0x02202208) /* PC算法         */
#define RNC_MOC_RRM_HC              SWP_MOC_DEF(0x02202209) /* HC算法         */
#define RNC_MOC_RRM_LCC             SWP_MOC_DEF(0x0220220A) /* LCC算法        */
#define RNC_MOC_RRM_LCC_STRAT       SWP_MOC_DEF(0x0220220B) /* LCC算法策略    */
#define RNC_MOC_RRM_SYNC            SWP_MOC_DEF(0x0220220C) /* SYNC算法       */
#define RNC_MOC_RRM_LCS             SWP_MOC_DEF(0x0220220D) /* LCS算法        */
#define RNC_MOC_RRM_TIMER           SWP_MOC_DEF(0x0220220E) /* 算法定时器     */
#define RNC_MOC_RRM_CHOOSE          SWP_MOC_DEF(0x0220220F) /* 小区选择       */
#define RNC_MOC_RRM_HSDPA           SWP_MOC_DEF(0x02202210) /* HSDPA算法      */
#define RNC_MOC_RRM_UPPCH           SWP_MOC_DEF(0x02202211) /* UPPCH Shifting */
#define RNC_MOC_RRM_INTRAFREQ       SWP_MOC_DEF(0x02202212) /* 同频事件信息表 */
#define RNC_MOC_RRM_INTERFREQ       SWP_MOC_DEF(0x02202213) /* 异频事件信息表 */
#define RNC_MOC_RRM_INTERRATE       SWP_MOC_DEF(0x02202214) /* 系统间事件信息表 */
#define RNC_MOC_RRM_INTERNAL        SWP_MOC_DEF(0x02202215) /* 内部测量事件信息表 */
#define RNC_MOC_RRM_POSITION        SWP_MOC_DEF(0x02202216) /* 位置测量事件信息表 */
#define RNC_MOC_RRM_EVENTA          SWP_MOC_DEF(0x02202217) /* EventA准则表 */
#define RNC_MOC_RRM_EVENTB          SWP_MOC_DEF(0x02202218) /* EventB准则表 */
#define RNC_MOC_RRM_EVENTC          SWP_MOC_DEF(0x02202219) /* EventC准则表 */
#define RNC_MOC_RRM_EVENTD          SWP_MOC_DEF(0x0220221A) /* EventD准则表 */
#define RNC_MOC_RRM_EVENTE          SWP_MOC_DEF(0x0220221B) /* EventE准则表 */
#define RNC_MOC_RRM_EVENTF          SWP_MOC_DEF(0x0220221C) /* EventF准则表 */
#define RNC_MOC_RRM_TIDE            SWP_MOC_DEF(0x0220221D) /* Tide算法 */
#define RNC_MOC_RRM_HSUPA           SWP_MOC_DEF(0x0220221E) /* HSUPA信息 */

/* 小区测量层 */
#define RNC_MOC_CELL_MEAS           SWP_MOC_DEF(0x02202301) /* <小区测量层> */
#define RNC_MOC_NODEB_MEAS          SWP_MOC_DEF(0x02202302) /* NodeB测量                  */
#define RNC_MOC_NODEB_PERIODIC_CRT  SWP_MOC_DEF(0x02202303) /* NodeB测量周期准则          */
#define RNC_MOC_NODEB_EVENTAB_CRT   SWP_MOC_DEF(0x02202304) /* NodeB测量EventA/EventB准则 */
#define RNC_MOC_NODEB_EVENTCD_CRT   SWP_MOC_DEF(0x02202305) /* NodeB测量EventC/EventD准则 */
#define RNC_MOC_NODEB_EVENTEF_CRT   SWP_MOC_DEF(0x02202306) /* NodeB测量EventE/EventF准则 */
#define RNC_MOC_UE_INTRAFREQ        SWP_MOC_DEF(0x02202307) /* UE测量同频测量信息         */
#define RNC_MOC_UE_INTRAFREQ_CRT    SWP_MOC_DEF(0x02202308) /* UE测量同频测量准则         */
#define RNC_MOC_UE_INTERFREQ        SWP_MOC_DEF(0x02202309) /* UE测量异频测量信息         */
#define RNC_MOC_UE_INTERFREQ_CRT    SWP_MOC_DEF(0x0220230A) /* UE测量异频测量准则         */
#define RNC_MOC_UE_INTERRAT         SWP_MOC_DEF(0x0220230B) /* UE测量系统间测量信息       */
#define RNC_MOC_UE_INTERRAT_CRT     SWP_MOC_DEF(0x0220230C) /* UE测量系统间测量准则       */
#define RNC_MOC_UE_QUALITY          SWP_MOC_DEF(0x0220230D) /* UE测量质量测量信息         */
#define RNC_MOC_UE_TRAFFICVOL       SWP_MOC_DEF(0x0220230E) /* UE测量业务量测量信息       */
#define RNC_MOC_UE_POSITION         SWP_MOC_DEF(0x0220230F) /* UE测量位置测量信息         */
#define RNC_MOC_UE_POSITION_CRT     SWP_MOC_DEF(0x02202310) /* UE测量位置测量准则         */
#define RNC_MOC_UE_INTERNAL         SWP_MOC_DEF(0x02202311) /* UE测量内部测量信息         */
#define RNC_MOC_UE_INTERNAL_CRT     SWP_MOC_DEF(0x02202312) /* UE测量内部测量准则         */
#define RNC_MOC_UE_PERIODIC_CRT     SWP_MOC_DEF(0x02202313) /* UE测量周期准则             */
#define RNC_MOC_TPSS_TRAFFICVOL     SWP_MOC_DEF(0x02202314) /* TPSS下行业务量测量         */
#define RNC_MOC_TPSS_QUALITY        SWP_MOC_DEF(0x02202315) /* TPSS上行质量测量           */
/* 业务组合层 */
#define RNC_MOC_MUX                 SWP_MOC_DEF(0x02202401) /* <业务组合层> */
#define RNC_MOC_ULRAB               SWP_MOC_DEF(0x02202402) /* 上行RAB信息              */
#define RNC_MOC_ULRB                SWP_MOC_DEF(0x02202403) /* 上行RB信息               */
#define RNC_MOC_DLRAB               SWP_MOC_DEF(0x02202404) /* 下行RAB信息              */
#define RNC_MOC_DLRB                SWP_MOC_DEF(0x02202405) /* 下行RB信息               */
#define RNC_MOC_DLRAB_SYNC          SWP_MOC_DEF(0x02202406) /* 业务同步参数信息         */
#define RNC_MOC_RLC_SENDER          SWP_MOC_DEF(0x02202407) /* 发送端RLC                */
#define RNC_MOC_RLC_RECEIVER        SWP_MOC_DEF(0x02202408) /* 接受端RLC                */
#define RNC_MOC_TFS_SEMISTATIC      SWP_MOC_DEF(0x02202409) /* TFS半静态部分信息        */
#define RNC_MOC_TFS_DYMATIC         SWP_MOC_DEF(0x0220240A) /* TFS动态部分信息          */
#define RNC_MOC_IU                  SWP_MOC_DEF(0x0220240B) /* Iu信息                   */
#define RNC_MOC_IU_RFCI             SWP_MOC_DEF(0x0220240C) /* Iu-RFCI信息              */
#define RNC_MOC_PDCP                SWP_MOC_DEF(0x0220240D) /* PDCP                     */
#define RNC_MOC_PDCP_RFC2507        SWP_MOC_DEF(0x0220240E) /* PDCP-RFC2507信息         */
#define RNC_MOC_PDCP_RFC3095        SWP_MOC_DEF(0x0220240F) /* PDCP-RFC3095信息         */
#define RNC_MOC_ULTRAFFIC           SWP_MOC_DEF(0x02202410) /* 上行组合业务             */
#define RNC_MOC_ULTRAFFIC_MACC      SWP_MOC_DEF(0x02202411) /* 上行组合业务MAC-C配置    */
#define RNC_MOC_ULTRAFFIC_MACD      SWP_MOC_DEF(0x02202412) /* 上行组合业务MAC-D配置    */
#define RNC_MOC_ULTRAFFIC_TS        SWP_MOC_DEF(0x02202413) /* 上行组合业务时隙资源配置 */
#define RNC_MOC_ULTRAFFIC_L1        SWP_MOC_DEF(0x02202414) /* 上行组合业务L1配置       */
#define RNC_MOC_ULTRAFFIC_PC        SWP_MOC_DEF(0x02202415) /* 上行组合业务功控参数     */
#define RNC_MOC_DLTRAFFIC           SWP_MOC_DEF(0x02202416) /* 下行组合业务             */
#define RNC_MOC_DLTRAFFIC_MACC      SWP_MOC_DEF(0x02202417) /* 下行组合业务MAC-C配置    */
#define RNC_MOC_DLTRAFFIC_MACD      SWP_MOC_DEF(0x02202418) /* 下行组合业务MAC-D配置    */
#define RNC_MOC_DLTRAFFIC_TS        SWP_MOC_DEF(0x02202419) /* 下行组合业务时隙资源配置 */
#define RNC_MOC_DLTRAFFIC_L1        SWP_MOC_DEF(0x02202420) /* 下行组合业务L1配置       */
#define RNC_MOC_DLTRAFFIC_PC        SWP_MOC_DEF(0x02202421) /* 下行组合业务功控参数     */

/* 协议过程参数层 */
#define RNC_MOC_PROTOCAL            SWP_MOC_DEF(0x02202501) /* <协议过程参数层> */
#define RNC_MOC_NBAP                SWP_MOC_DEF(0x02202502) /* NBAP协议过程     */
#define RNC_MOC_RRC                 SWP_MOC_DEF(0x02202503) /* RRC协议过程      */
#define RNC_MOC_RRC_SETUP_CAUSE     SWP_MOC_DEF(0x02202504) /* RRC连接建立原因  */
#define RNC_MOC_RANAP               SWP_MOC_DEF(0x02202505) /* RANAP协议过程    */
#define RNC_MOC_IUUP                SWP_MOC_DEF(0x02202506) /* IUUP协议过程     */
#define RNC_MOC_IUB                 SWP_MOC_DEF(0x02202507) /* Iub同步定时器    */
#define RNC_MOC_UE_TIMER            SWP_MOC_DEF(0x02202508) /* UE定时器与计数器 */
#define RNC_MOC_PROTOCAL_HSUPA      SWP_MOC_DEF(0x02202509) /* HSUPA业务面控制参数 */

/* 业务规划参数层 */
#define RNC_MOC_TRAFFIC_QOS         SWP_MOC_DEF(0x02202601) /* 业务Qos配置      */
#define RNC_MOC_RRM_GLOBAL_PARA     SWP_MOC_DEF(0x02202220) /* 算法全局参数     */
#define RNC_MOC_MBMS_ARITH          SWP_MOC_DEF(0x02202602) /* MBMS算法表       */
#define RNC_MOC_MBMS_SFN            SWP_MOC_DEF(0x02202603) /* MBMS区域规划表   */
#define RNC_MOC_MBMS_SFN_CELL       SWP_MOC_DEF(0x02202604) /* SFN小区规划表    */
#define RNC_MOC_MBMS_SFN_TS         SWP_MOC_DEF(0x02202605) /* MBMS时隙资源表   */
#define RNC_MOC_MBMS_SFN_SCCPCHSET  SWP_MOC_DEF(0x02202606) /* MBMS SCCPCHSET表 */
#define RNC_MOC_MBMS_SFN_SCCPCH     SWP_MOC_DEF(0x02202607) /* MBMS SCCPCH表    */
#define RNC_MOC_MBMS_SFN_FACH       SWP_MOC_DEF(0x02202608) /* MBMS FACH表      */
#define RNC_MOC_PROHIBIT_SERVICE    SWP_MOC_DEF(0x02202609) /* 禁止接入业务     */
#define RNC_MOC_HSUPA_SERVICE       SWP_MOC_DEF(0x02202610) /* HSUPA接入业务     */
#define RNC_MOC_OVERLOAD_CNTRL      SWP_MOC_DEF(0x02202611) /* 负荷控制相关信息     */

 

/*******************************************************************************
*                               类型定义                                       *
*******************************************************************************/
/* MOI -- RNC网元 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
} RNC_MOI_RNC_T;

/* MOI -- RNC网元PLMN信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucRncPlmnInfoId;    /* Rnc的plmn信息标识，从1开始递增顺序生成*/
} RNC_MOI_RNC_PLMN_T;

/* MOI -- 邻接MSC */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usAdjMscId;         /* MSC标识 */
} RNC_MOI_ADJ_MSC_T;

/* MOI -- 邻接SGSN */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usAdjSgsnId;        /* SGSN标识 */
} RNC_MOI_ADJ_SGSN_T;

/* MOI -- 邻接MSC IP PATH */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usAdjMscId;         /* MSC标识 */
    UINT32  ulPathId;           /* PATH标识 */
} RNC_MOI_ADJ_MSC_IPPATH_T;

/* MOI -- 邻接SGSN IP PATH */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usAdjSgsnId;        /* SGSN标识 */
    UINT32  ulPathId;           /* PATH标识 */
} RNC_MOI_ADJ_SGSN_IPPATH_T;

/* MOI -- 邻接RNC */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usAdjRncId;         /* RNC标识 */
} RNC_MOI_ADJ_RNC_T;

/* MOI -- 邻接RNC网元PLMN信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usAdjRncId;         /* RNC标识 */
    UINT8   ucRncPlmnInfoId;    /* Rnc的plmn信息标识，从1开始递增顺序生成*/
} RNC_MOI_ADJ_RNC_PLMN_T;

/* MOI -- 邻接RNC网元MSC信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usAdjRncId;         /* RNC标识 */
    UINT16  usMscId;            /* MSC标识 */
} RNC_MOI_ADJ_RNC_MSC_T;

/* MOI -- 邻接RNC的SGSN信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usAdjRncId;         /* RNC标识 */
    UINT16  usSgsnId;           /* SGSN标识 */
} RNC_MOI_ADJ_RNC_SGSN_T;

/* MOI -- NodeB */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 */
} RNC_MOI_NODEB_T;

/* MOI -- NodeB AAL2通道 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识 */
    UINT16  usPvcId;        /* PVC ID */
} RNC_MOI_NODEB_AAL2PATH_T;

/* MOI -- NodeB IP通道 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识 */
    UINT32  ulPathId;       /* PATH ID */
} RNC_MOI_NODEB_IPPATH_T;

/* MOI -- NodeB设备配置 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识 */
    UINT16  usNodeBEqmId;   /* NodeBEqm标识,在创建对象时,从1开始递增顺序生成 */
} RNC_MOI_NODEB_EQM_T;

/* MOI -- NodeB小区组配置 */
typedef struct
{
    UINT16  usNeElement;            /* Rnc标识(1..4095) */
    UINT16  usNodeBId;              /* NodeB标识 */
    UINT32  ulLocalCellGroupId;     /* NodeB小区组标识*/
} RNC_MOI_NODEB_CELLGROUP_T;

/* MOI -- E-AGCH信息 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识,创建对象时从父对象获取 */
    UINT16  usCellId;       /* 小区标识 */
    UINT8   ucUARFCNId;     /* 载频标识, 创建对象时从父对象获取 */
    UINT8   ucEagchId;      /* E-AGCH标识(0~31) */
} RNC_MOI_HSUPA_EAGCH_T;

/* MOI -- E-HICH信息 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识，创建对象时从父对象获取 */
    UINT16  usCellId;       /* 小区标识 */
    UINT8   ucUARFCNId;     /* 载频标识, 创建对象时从父对象获取 */
    UINT8   ucEhichId;      /* E-HICH标识·(0..31) */
} RNC_MOI_HSUPA_EHICH_T;

/* MOI -- E-PUCH信息 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识，创建对象时从父对象获取 */
    UINT16  usCellId;       /* 小区标识 */
    UINT8   ucUARFCNId;     /* 载频标识, 创建对象时从父对象获取 */
    UINT8   ucTsNo;         /* 时隙 */
} RNC_MOI_HSUPA_EPUCH_T;

/* MOI -- 小区 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 */
    UINT16  usCellId;           /* 小区标识 */
} RNC_MOI_CELL_T;

/* MOI -- 小区规划参数 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识，创建对象时从父对象获取 */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucCellPlanParaId;   /* 小区规划参数标识，在创建对象时，从1开始递增顺序生成*/
} RNC_MOI_CELL_PLAN_PARA_T;

/* MOI -- 系统消息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识，创建对象时从父对象获取  */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucSibId;            /* 系统消息标识，在创建对象时，从1开始递增顺序生成*/
} RNC_MOI_CELL_SIB_T;

/* MOI -- 载频 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ，创建对象时从父对象获取 */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucUARFCNId;         /* 载频标识，在创建对象时，从1开始递增顺序生成 */
} RNC_MOI_CELL_CARRIER_T;

/* MOI -- 时隙 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识，创建对象时从父对象获取  */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucUARFCNId;         /* 载频标识，创建对象时从父对象获取*/
    UINT8   ucTimeSlotNo;       /* 时隙编号(0..6) */
} RNC_MOI_TIME_SLOT_T;

/* MOI -- HS-PDSCH集 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识，创建对象时从父对象获取  */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucUARFCNId;         /* 载频标识，创建对象时从父对象获取*/
    UINT8   ucTimeSlotNo;       /* 时隙编号(0..6) */
} RNC_MOI_HSPDSCH_SLOT_T;

/* MOI -- HS-SCCH集 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识，创建对象时从父对象获取  */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucUARFCNId;         /* 载频标识，创建对象时从父对象获取*/
    UINT8   ucHsscchId;         /* HS-SCCH信道ID(0..31) */
} RNC_MOI_HSSCCH_SLOT_T;

/* MOI -- PCCPCH */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识，创建对象时从父对象获取  */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucCommonPhyChId;    /* 公共物理信道标识(0..29) */
} RNC_MOI_PCCPCH_T;

/* MOI -- DwPCH */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识，创建对象时从父对象获取*/
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucCommonPhyChId;    /* 公共物理信道标识(0..29) */
} RNC_MOI_DWPCH_T;

/* MOI -- PRACH集 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ，创建对象时从父对象获取*/
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucPrachSetId;       /* PRACH集标识(1..2) */
} RNC_MOI_PRACH_SET_T;

/* MOI -- PRACH信道 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识，创建对象时从父对象获取*/
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucPrachSetId;       /* PRACH集标识(1..2) */
    UINT8   ucCommonPhyChId;    /* 公共物理信道标识(0..29) */
} RNC_MOI_PRACH_T;

/* MOI -- FPACH信道 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识，创建对象时从父对象获取*/
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucPrachSetId;       /* PRACH集标识(1..2) */
    UINT8   ucCommonPhyChId;    /* 公共物理信道标识(0..29) */
} RNC_MOI_FPACH_T;

/* MOI -- RACH信道 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ，创建对象时从父对象获取*/
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucPrachSetId;       /* PRACH集标识(1..2) */
    UINT8   ucCommonTranChId;   /* 公共传输信道标识(0..32) */
} RNC_MOI_RACH_T;

/* MOI -- SCCPCH集 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ，创建对象时从父对象获取*/
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucSccpchSetId;      /* SCCPCH集标识(1) */
} RNC_MOI_SCCPCH_SET_T;

/* MOI -- SCCPCH信道 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识，创建对象时从父对象获取*/
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucSccpchSetId;      /* SCCPCH集标识(1) */
    UINT8   ucCommonPhyChId;    /* 公共物理信道标识(0..29) */
} RNC_MOI_SCCPCH_T;

/* MOI -- PICH信道 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ，创建对象时从父对象获取*/
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucSccpchSetId;      /* SCCPCH集标识(1) */
    UINT8   ucCommonPhyChId;    /* 公共物理信道标识(0..29) */
} RNC_MOI_PICH_T;

/* MOI -- PCH信道 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识，创建对象时从父对象获取*/
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucSccpchSetId;      /* SCCPCH集标识(1) */
    UINT8   ucCommonTranChId;   /* 公共传输信道标识(0..32) */
} RNC_MOI_PCH_T;

/* MOI -- FACH信道 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识，创建对象时从父对象获取*/
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucSccpchSetId;      /* SCCPCH集标识(1) */
    UINT8   ucCommonTranChId;   /* 公共传输信道标识(0..32) */
} RNC_MOI_FACH_T;

/* MOI -- 信道RLC信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ，创建对象时从父对象获取*/
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucCommonTransChID;  /*公共传输信道ID*/
    UINT8   ucRbId;             /* RB id */
} RNC_MOI_CTRCH_RLC_T;

/* MOI -- 邻小区 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ，创建对象时从父对象获取*/
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucAdjCellId;        /* 邻小区记录标识(OMC设置)，从1开始递增顺序生成*/
} RNC_MOI_ADJ_CELL_T;

/* MOI -- 外部TDD128小区 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usReserve;          /* 为了字节对齐引入的保留字段（固定填0） */
    UINT32  ulCellRecordId;     /* 小区记录标识(OMC设置) */
} RNC_MOI_OUT_TDD_128_CELL_T;

/* MOI -- 外部TDD384小区 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usReserve;          /* 为了字节对齐引入的保留字段（固定填0） */
    UINT32  ulCellRecordId;     /* 小区记录标识(OMC设置) */
} RNC_MOI_OUT_TDD_384_CELL_T;

/* MOI -- 外部FDD小区 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usReserve;          /* 为了字节对齐引入的保留字段（固定填0） */
    UINT32  ulCellRecordId;     /* 小区记录标识(OMC设置) */
} RNC_MOI_OUT_FDD_CELL_T;

/* MOI -- 外部IS2000小区 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usReserve;          /* 为了字节对齐引入的保留字段（固定填0） */
    UINT32  ulCellRecordId;     /* 小区记录标识(OMC设置) */
} RNC_MOI_OUT_IS2000_CELL_T;

/* MOI -- 外部GERAN小区 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usReserve;          /* 为了字节对齐引入的保留字段（固定填0） */
    UINT32  ulCellRecordId;     /* 小区记录标识(OMC设置) */
} RNC_MOI_OUT_GERAN_CELL_T;

/* MOI -- CAC算法 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucCacId;            /* cac算法标识(固定为1) */
} RNC_MOI_CELL_RRM_CAC_T;

/* MOI -- SDCA算法 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucSdcaId;           /* Sdca算法标识,(固定为1) */
} RNC_MOI_CELL_RRM_SDCA_T;

/* MOI -- FDCA算法 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucFdcaId;           /* Fdca算法标识（固定1） */
} RNC_MOI_CELL_RRM_FDCA_T;

/* MOI -- PS算法 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucPsId;             /* Ps算法标识(固定为1) */
} RNC_MOI_CELL_RRM_PS_T;

/* MOI -- AMRC算法 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucAmrcId;           /* Amrc算法标识(固定为1) */
} RNC_MOI_CELL_RRM_AMRC_T;

/* MOI -- RLS算法 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucRlsId;            /* Rls算法标识(固定为1) */
} RNC_MOI_CELL_RRM_RLS_T;

/* MOI -- PC算法 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucPcId;             /* Pc算法标识（固定为1） */
} RNC_MOI_CELL_RRM_PC_T;

/* MOI -- HC算法 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucHcId;             /* Hc算法标识(固定为1) */
} RNC_MOI_CELL_RRM_HC_T;

/* MOI -- TIDE算法 */
typedef struct
{
    UINT16  usNeElement;     /* Rnc标识(1..4095) */
    UINT16  usNodeBId;       /* NodeB标识 ,在创建对象时根据父对象的取值设置*/
    UINT16  usCellId;        /* 小区标识*/
    UINT8   ucTideId;        /* Tide算法标识(由OMC创建，固定为1) */
} RNC_MOI_CELL_RRM_TIDE_T;

/* MOI -- HSUPA信息 */
typedef struct
{
    UINT16  usNeElement;     /* Rnc标识(1..4095) */
    UINT16  usNodeBId;       /* NodeB标识 ,在创建对象时根据父对象的取值设置*/
    UINT16  usCellId;        /* 小区标识*/
    UINT8   ucRrmHsupaId;    /* Hsupa算法标识（固定为1） */
} RNC_MOI_CELL_RRM_HSUPA_T;

/* MOI -- LCC算法 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucLccId;            /* Lcc算法标识(固定为1) */
} RNC_MOI_CELL_RRM_LCC_T;

/* MOI -- LCC算法策略 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;           /* 小区标识 */
    UINT16  usRecorderId;       /* Lcc算法策略标识,在创建对象时,从1开始递增顺序生成 */
} RNC_MOI_CELL_RRM_LCC_STRAT_T;

/* MOI -- 同频事件信息表 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;       /* 小区标识 */
    UINT16  usRecorderId;   /* 在创建对象时,从1开始递增顺序生成 */
} RNC_MOI_CELL_RRM_INTRAFREQ_EVENT_T;

/* MOI -- 异频事件信息表 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;       /* 小区标识 */
    UINT16  usRecorderId;   /* 在创建对象时,从1开始递增顺序生成 */
} RNC_MOI_CELL_RRM_INTERFREQ_EVENT_T;

/* MOI -- 系统间事件信息表 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;       /* 小区标识 */
    UINT16  usRecorderId;   /* 在创建对象时,从1开始递增顺序生成 */
} RNC_MOI_CELL_RRM_INTERRATE_EVENT_T;

/* MOI -- 内部测量事件信息表 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;       /* 小区标识 */
    UINT16  usRecorderId;   /* 在创建对象时,从1开始递增顺序生成 */
} RNC_MOI_CELL_RRM_INTERNAL_EVENT_T;

/* MOI -- 位置测量事件信息表 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;       /* 小区标识 */
    UINT16  usRecorderId;   /* 在创建对象时,从1开始递增顺序生成 */
} RNC_MOI_CELL_RRM_POSITION_EVENT_T;

/* MOI -- EventA准则表 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;       /* 小区标识 */
    UINT8   ucMeasQuantity; /* 测量量 */
} RNC_MOI_CELL_RRM_EVENTA_T;

/* MOI -- EventB准则表 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;       /* 小区标识 */
    UINT8   ucMeasQuantity; /* 测量量 */
} RNC_MOI_CELL_RRM_EVENTB_T;

/* MOI -- EventC准则表 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;       /* 小区标识 */
    UINT8   ucMeasQuantity; /* 测量量 */
} RNC_MOI_CELL_RRM_EVENTC_T;

/* MOI -- EventD准则表 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;       /* 小区标识 */
    UINT8   ucMeasQuantity; /* 测量量 */
} RNC_MOI_CELL_RRM_EVENTD_T;

/* MOI -- EventE准则表 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;       /* 小区标识 */
    UINT8   ucMeasQuantity; /* 测量量 */
} RNC_MOI_CELL_RRM_EVENTE_T;

/* MOI -- EventF准则表 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usNodeBId;      /* NodeB标识,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;       /* 小区标识 */
    UINT8   ucMeasQuantity; /* 测量量 */
} RNC_MOI_CELL_RRM_EVENTF_T;

/* MOI -- SYNC算法 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucSyncId;           /* Sync算法标识（固定为1） */
} RNC_MOI_CELL_RRM_SYNC_T;

/* MOI -- LCS算法 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucLcsId;            /* Lcs算法标识（固定为1） */
} RNC_MOI_CELL_RRM_LCS_T;

/* MOI -- 算法定时器 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置 */
    UINT16  usCellId;           /* 小区标识 */
    UINT8   ucRrmTimerId;       /* 算法定时器标识（固定为1） */
} RNC_MOI_CELL_RRM_TIMER_T;

/* MOI -- 小区选择和重选 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置*/
    UINT16  usCellId;           /* 小区标识*/
    UINT8   ucCellSelId;        /* 小区选择和重选标识, 在创建对象时，从1开始递增顺序生成*/
} RNC_MOI_CELL_RRM_SELECT_T;

/* MOI -- HSDPA信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置*/
    UINT16  usCellId;           /* 小区标识*/
    UINT8   ucRrmHsdpaId;       /* Hsdpa算法标识（固定为1） */
} RNC_MOI_CELL_RRM_HSDPA_T;

/* MOI -- UpPcH Shifting自适应算法 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usNodeBId;          /* NodeB标识 ,在创建对象时根据父对象的取值设置*/
    UINT16  usCellId;           /* 小区标识*/
    UINT8   ucUppchShiftId;     /* Uppch算法标识（固定为1） */
} RNC_MOI_CELL_RRM_UPPCH_T;

/* MOI -- NodeB测量 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
} RNC_MOI_CELL_MEAS_NODEB_T;

/* MOI -- NodeB测量周期准则 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
    UINT8   ucPeriCrtId;        /* 周期性测量准则（固定为1） */
} RNC_MOI_CELL_MEAS_NODEB_PERIODIC_CRITERIA_T;

/* MOI -- NodeB测量EventA/EventB准则 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
    UINT8   ucEventABCrtId;     /* EventAEventB测量准则（固定为1） */
} RNC_MOI_CELL_MEAS_NODEB_EVENTAB_CRITERIA_T;

/* MOI -- NodeB测量EventC/EventD准则 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
    UINT8   ucEventCDCrtId;     /* EventCEventD测量准则（固定为1） */
} RNC_MOI_CELL_MEAS_NODEB_EVENTCD_CRITERIA_T;

/* MOI -- NodeB测量EventE/EventF准则 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
    UINT8   ucEventEFCrtId;     /* EventEEventF测量准则（固定为1） */
} RNC_MOI_CELL_MEAS_NODEB_EVENTEF_CRITERIA_T;

/* MOI -- UE测量同频测量信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
} RNC_MOI_CELL_MEAS_UE_INTRAFREQ_T;

/* MOI -- UE测量同频测量准则 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT8   ucMeasId;       /* 测量标识 */
    UINT8   ucRecorderId;   /* UE同频测量准则，从1开始顺序编号 */
} RNC_MOI_CELL_MEAS_UE_INTRAFREQ_CRITERIA_T;

/* MOI -- UE测量异频测量信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
} RNC_MOI_CELL_MEAS_UE_INTERFREQ_T;

/* MOI -- UE测量异频测量准则 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
    UINT8   ucRecorderId;  /* UE异频测量准则，从1开始顺序编号 */
} RNC_MOI_CELL_MEAS_UE_INTERFREQ_CRITERIA_T;

/* MOI -- UE测量系统间测量信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
} RNC_MOI_CELL_MEAS_UE_INTERRAT_T;

/* MOI -- UE测量系统间测量准则 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
    UINT8   ucRecorderId;       /* UE系统间测量准则，从1开始顺序编号 */
} RNC_MOI_CELL_MEAS_UE_INTERRAT_CRITERIA_T;

/* MOI -- UE测量质量测量信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
} RNC_MOI_CELL_MEAS_UE_QUALITY_T;

/* MOI -- UE测量业务量测量信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
} RNC_MOI_CELL_MEAS_UE_TRAFFICVOLUME_T;

/* MOI -- UE测量位置测量信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
} RNC_MOI_CELL_MEAS_UE_POSITION_T;

/* MOI -- UE测量位置测量准则 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
    UINT8   ucRecorderId;       /* UE位置测量准则，从1开始顺序编号 */
} RNC_MOI_CELL_MEAS_UE_POSITION_CRT_T;

/* MOI -- UE测量内部测量信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
} RNC_MOI_CELL_MEAS_UE_INTERNAL_T;

/* MOI -- UE测量内部测量准则 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
    UINT8   ucUEInterCrtId;     /* UE内部测量准则，从1开始顺序编号 */
} RNC_MOI_CELL_MEAS_UE_INTERNAL_CRITERIA_T;

/* MOI -- UE测量周期准则 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
} RNC_MOI_CELL_MEAS_UE_PERIODIC_CRITERIA_T;

/* MOI -- TPSS下行业务量测量 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
} RNC_MOI_CELL_MEAS_TPSS_TRAFFICVOLUME_T;

/* MOI -- TPSS上行质量测量 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucMeasId;           /* 测量标识 */
} RNC_MOI_CELL_MEAS_TPSS_QUALITY_T;

/* MOI -- 上行RAB */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucUlRabIdx;         /* Rab类型索引号 */
} RNC_MOI_MUX_ULRAB_T;

/* MOI -- 上行RB信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucUlRabIdx;         /* Rab类型索引号 */
    UINT8   ucRBId;             /* RB标识*/
} RNC_MOI_MUX_ULRAB_RB_T;

/* MOI -- 下行RAB */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucDlRabIdx;         /* Rab类型索引号 */
} RNC_MOI_MUX_DLRAB_T;

/* MOI -- 下行RB信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucDlRabIdx;         /* Rab类型索引号 */
    UINT8   ucRBId;             /* RB标识*/
} RNC_MOI_MUX_DLRAB_RB_T;

/* MOI -- 业务同步参数信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucDlRabIdx;         /* Rab类型索引号 */
    UINT8   ucDlRabSynId;       /* 下行RAB同步配置对象ID（固定为1） */
} RNC_MOI_MUX_DLRAB_SYNC_T;

/* MOI -- 发送端RLC */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucSendRlcId;        /* 发送端RLC参数的索引 */
} RNC_MOI_MUX_RLC_SENDER_T;

/* MOI -- 接受端RLC */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucRecvRlcId;        /* 接受端RLC参数的索引 */
} RNC_MOI_MUX_RLC_RECEIVER_T;

/* MOI -- TFS半静态部分信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucTfsId;            /* TFS参数的索引号 */
} RNC_MOI_MUX_TFS_SEMISTATIC_T;

/* MOI -- TFS动态部分信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucTfsId;            /* TFS参数的索引号 */
    UINT8   ucReserve;          /* 保留字段 */
    UINT16  usRedorderId;       /* RLC大小 */
} RNC_MOI_MUX_TFS_DYNAMIC_T;

/* MOI -- Iu信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucIuInfoId;         /* Iu表索引 */
} RNC_MOI_MUX_IU_T;

/* MOI -- Iu-RFCI信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT8   ucIuInfoId;         /* Iu表索引 */
    UINT8   ucRfci;             /* RAB子流组合标识 */
} RNC_MOI_MUX_IU_RFCI_T;

/* MOI -- PDCP信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usResv;             /* 保留 */
    UINT32  ulRecordId;         /* PDCP配置记录ID（固定为1） */
} RNC_MOI_MUX_PDCP_T;

/* MOI -- PDCP-RFC2507信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usResv;             /* 保留 */
    UINT32  ulRecordId;         /* PDCP配置记录ID（固定为1） */
} RNC_MOI_MUX_PDCP_RFC2507_T;

/* MOI -- PDCP-RFC3095信息 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usResv;             /* 保留 */
    UINT32  ulRecordId;         /* PDCP配置记录ID（固定为1） */
} RNC_MOI_MUX_PDCP_RFC3095_T;

/* MOI -- 上行组合业务 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usUlMuxIdx;         /* 组合索引号 */
} RNC_MOI_MUX_ULTRAFFIC_T;

/* MOI -- MAC-C配置 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usUlMuxIdx;         /* 组合索引号 */
    UINT16  usTfcId;            /* TFC标识 */
} RNC_MOI_MUX_ULTRAFFIC_MACC_T;

/* MOI -- MAC-D配置 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usUlMuxIdx;         /* 组合索引号 */
    UINT16  usTfcId;            /* TFC标识 */
} RNC_MOI_MUX_ULTRAFFIC_MACD_T;

/* MOI -- 时隙资源配置 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usUlMuxIdx;         /* 组合索引号 */
    UINT8   ucTsIndex;          /* 时隙索引号 */
} RNC_MOI_MUX_ULTRAFFIC_TS_T;

/* MOI -- 上行组合业务L1配置 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usUlMuxIdx;         /* 组合索引号 */
    UINT8   ucUlMuxL1Id;        /* 上行业务组合L1ID（固定为1） */
} RNC_MOI_MUX_ULTRAFFIC_L1_T;

/* MOI -- 业务功控参数 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usUlMuxIdx;         /* 组合索引号 */
    UINT16  usCellTrafficPcType;/* 业务功控类型 */
} RNC_MOI_MUX_ULTRAFFIC_PC_T;

/* MOI -- 下行组合业务 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usDlMuxIdx;         /* 组合索引号 */
} RNC_MOI_MUX_DLTRAFFIC_T;

/* MOI -- MAC-C配置 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usDlMuxIdx;         /* 组合索引号 */
    UINT16  usTfcId;            /* TFC标识 */
} RNC_MOI_MUX_DLTRAFFIC_MACC_T;

/* MOI -- MAC-D配置 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usDlMuxIdx;         /* 组合索引号 */
    UINT16  usTfcId;            /* TFC标识 */
} RNC_MOI_MUX_DLTRAFFIC_MACD_T;

/* MOI -- 时隙资源配置 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usDlMuxIdx;         /* 组合索引号 */
    UINT8   ucTsIndex;          /* 时隙索引号 */
} RNC_MOI_MUX_DLTRAFFIC_TS_T;

/* MOI -- 下行组合业务L1配置 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usDlMuxIdx;         /* 组合索引号 */
    UINT8   ucDlMuxL1Id;        /* 下行业务组合L1ID（固定为1） */
} RNC_MOI_MUX_DLTRAFFIC_L1_T;

/* MOI -- 业务功控参数 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usDlMuxIdx;         /* 组合索引号 */
    UINT16  usCellTrafficPcType;/* 业务功控类型 */
} RNC_MOI_MUX_DLTRAFFIC_PC_T;

/* MOI -- NBAP协议过程 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usReserve;          /* 为了字节对齐引入的保留字段（固定填0） */
    UINT32  ulRecordId;         /* 记录号 */
} RNC_MOI_PROTOCOL_NBAP_T;

/* MOI -- RRC协议过程 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usReserve;          /* 为了字节对齐引入的保留字段（固定填0） */
    UINT32  ulRecordId;         /* 记录号 */
} RNC_MOI_PROTOCOL_RRC_T;

/* MOI -- RRC连接建立原因 */
typedef struct
{
    UINT16  usNeElement;            /* Rnc标识(1..4095) */
    UINT8   ucEstablishmentCause;   /*  RRC连接建立原因 */
} RNC_MOI_PROTOCOL_RRCSETUPCAUSE_T;

/* MOI -- RANAP协议过程 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usReserve;          /* 为了字节对齐引入的保留字段（固定填0） */
    UINT32  ulRecordId;         /* 记录号 */
} RNC_MOI_PROTOCOL_RANAP_T;

/* MOI -- IUUP协议过程 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usReserve;          /* 为了字节对齐引入的保留字段（固定填0） */
    UINT32  ulRecordId;         /* 记录号 */
} RNC_MOI_PROTOCOL_IUUP_T;

/* MOI -- Iub同步定时器 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usReserve;          /* 为了字节对齐引入的保留字段（固定填0） */
    UINT32  ulRecordId;         /* 记录号 */
} RNC_MOI_PROTOCOL_IUB_SYNCTIMER_T;

/* MOI -- UE定时器与计数器 */
typedef struct
{
    UINT16  usNeElement;        /* Rnc标识(1..4095) */
    UINT16  usReserve;          /* 为了字节对齐引入的保留字段（固定填0） */
    UINT32  ulRecordId;         /* 记录号 */
} RNC_MOI_UE_TIMER_T;

/* MOI -- HSUPA业务面控制参数 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT16  usReserve;      /* 为了字节对齐引入的保留字段（固定填0） */
    UINT32  ulRecordId;     /* 记录号 */
} RNC_MOI_PROTOCOL_HSUPA_T;

/* 业务规划参数层 */
/* MOI -- 业务Qos配置 */
typedef struct
{
    UINT16  usNeElement;      /* Rnc标识(1..4095) */
    UINT16  usReserve;        /* 为了字节对齐引入的保留字段（固定填0） */
    UINT32  ulRecordId;       /* 记录号 */
} RNC_MOI_TRAFFIC_QOS_T;

/* MOI -- 算法全局参数 */
typedef struct
{
    UINT16  usNeElement;      /* Rnc标识(1..4095) */
    UINT16  usReserve;        /* 为了字节对齐引入的保留字段（固定填0） */
    UINT32  ulRecordId;       /* 记录号 */
} RNC_MOI_RRM_GLOBAL_PARA_T;

/* MOI -- MBMS算法 */
typedef struct
{
    UINT16  usNeElement;      /* Rnc标识(1..4095) */
    UINT16  usMbSfnId;        /* SFN标识 */
} RNC_MOI_MBMS_ARITH_T;

/* MOI -- MBMS区域规划 */
typedef struct
{
    UINT16  usNeElement;      /* Rnc标识(1..4095) */
    UINT16  usMbSfnId;        /* SFN标识 */
    UINT16  usMbmsSac;        /* SFN所在区域 */
} RNC_MOI_MBMS_SFN_T;

/* MOI -- MBMS小区规划 */
typedef struct
{
    UINT16  usNeElement;      /* Rnc标识(1..4095) */
    UINT16  usMbSfnId;        /* SFN标识 */
    UINT16  usCellId;         /* 小区标识 */
} RNC_MOI_MBMS_SFN_CELL_T;

/* MOI -- MBMS时隙资源 */
typedef struct
{
    UINT16  usNeElement;      /* Rnc标识(1..4095) */
    UINT16  usMbSfnId;        /* SFN标识 */
    UINT16  usRecorderId;     /* 记录号 */
} RNC_MOI_MBMS_SFN_TS_T;

/* MOI -- MBMS SCCPCHSET */
typedef struct
{
    UINT16  usNeElement;      /* Rnc标识(1..4095) */
    UINT16  usMbSfnId;        /*  SFN标识 */
    UINT8   ucSccpchSetId;    /* SCCPCH信道集标识 */
} RNC_MOI_MBMS_SFN_SCCPCHSET_T;

/* MOI -- MBMS SCCPCH */
typedef struct
{
    UINT16  usNeElement;      /* Rnc标识(1..4095) */
    UINT16  usMbSfnId;        /* SFN标识 */
    UINT8   ucSccpchSetId;    /* SCCPCH信道集标识 */
    UINT8   ucChannelId;      /* FACH信道标识*/
} RNC_MOI_MBMS_SFN_SCCPCH_T;

/* MOI -- MBMS FACH */
typedef struct
{
    UINT16  usNeElement;      /* Rnc标识(1..4095) */
    UINT16  usMbSfnId;        /* SFN标识 */
    UINT8   ucSccpchSetId;    /* SCCPCH信道集标识 */
    UINT8   ucTransportChannelId;   /* 公共传输信道标识 */
} RNC_MOI_MBMS_SFN_FACH_T;

/* MOI -- 禁止接入业务 */
typedef struct
{
    UINT16  usNeElement;      /* Rnc标识(1..4095) */
    UINT8   ucServiceIdx;     /* 业务索引号 */
} RNC_MOI_PROHIBIT_SERVICE_T;

/* MOI -- HSUPA接入业务 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT8   ucServiceIdx;   /* 业务索引号 */
} RNC_MOI_HSUPA_SERVICE_T;

/* MOI -- 负荷控制相关信息 */
typedef struct
{
    UINT16  usNeElement;    /* Rnc标识(1..4095) */
    UINT8   ucRecorderId;   /* 记录号 */
} RNC_MOI_OVERLOAD_CTRL_T;


/* RNC扩展MO的MOI结构定义 */
typedef union
{
    RNC_MOI_RNC_T               stRnc;          /* RNC网元 */
    RNC_MOI_RNC_PLMN_T          stRncPlmn;      /* RNC网元PLMN信息 */
    RNC_MOI_ADJ_MSC_T           stAdjMsc;       /* 邻接MSC */
    RNC_MOI_ADJ_SGSN_T          stAdjSgsn;      /* 邻接SGSN */
    RNC_MOI_ADJ_MSC_IPPATH_T    stAdjMscIpPath; /* 邻接MSC IP PATH */
    RNC_MOI_ADJ_SGSN_IPPATH_T   stAdjSgsnIpPath;/* 邻接SGSN IP PATH */
    RNC_MOI_ADJ_RNC_T           stAdjRnc;       /* 邻接RNC */
    RNC_MOI_ADJ_RNC_PLMN_T      stAdjRncPlmn;   /* 邻接RNC网元PLMN信息 */
    RNC_MOI_ADJ_RNC_MSC_T       stAdjRncMsc;    /* 邻接RNC网元MSC信息 */
    RNC_MOI_ADJ_RNC_SGSN_T      stAdjRncSgsn;   /* 邻接RNC的SGSN信息 */
    RNC_MOI_NODEB_T             stNodeB;        /* NodeB */
    RNC_MOI_NODEB_AAL2PATH_T    stNodeBAal2Path;/* NodeB AAL2通道 */
    RNC_MOI_NODEB_IPPATH_T      stNodeBIpPath;  /* NodeB IP通道 */
    RNC_MOI_NODEB_EQM_T         stNodeBEqm;     /* NodeB设备配置 */
    RNC_MOI_NODEB_CELLGROUP_T   stNodebCellGroup;   /* NodeB小区组配置 */ 
    RNC_MOI_HSUPA_EAGCH_T       stHsupaEagch;   /* E-AGCH信息 */
    RNC_MOI_HSUPA_EHICH_T       stHsupaEhich;   /* E-HICH信息 */
    RNC_MOI_HSUPA_EPUCH_T       stHsupaEpuch;   /* E-PUCH信息 */
    RNC_MOI_CELL_T              stCell;         /* 小区 */
    RNC_MOI_CELL_PLAN_PARA_T    stCellPlanPara; /* 小区规划参数 */
    RNC_MOI_CELL_SIB_T          stCellSib;      /* 系统消息 */
    RNC_MOI_CELL_CARRIER_T      stCellCarrier;  /* 载频 */
    RNC_MOI_TIME_SLOT_T         stCellTimeSlot; /* 时隙 */
    RNC_MOI_HSPDSCH_SLOT_T      stHsPdschSlot;  /* HS-PDSCH集 */
    RNC_MOI_HSSCCH_SLOT_T       stHsScchSlot;   /* HS-SCCH集 */
    RNC_MOI_PCCPCH_T            stPccpch;       /* PCCPCH */
    RNC_MOI_DWPCH_T             stDwpch;        /* DwPCH */
    RNC_MOI_PRACH_SET_T         stPrachSet;     /* PRACH集 */
    RNC_MOI_PRACH_T             stPrach;        /* PRACH信道 */
    RNC_MOI_FPACH_T             stFpach;        /* FPACH信道 */
    RNC_MOI_RACH_T              stRach;         /* RACH信道 */
    RNC_MOI_SCCPCH_SET_T        stSccpchSet;    /* SCCPCH集 */    
    RNC_MOI_SCCPCH_T            stSccpch;       /* SCCPCH信道 */
    RNC_MOI_PICH_T              stPich;         /* PICH信道 */
    RNC_MOI_PCH_T               stPch;          /* PCH信道 */
    RNC_MOI_FACH_T              stFach;         /* FACH信道 */
    RNC_MOI_CTRCH_RLC_T         stCtrchRlc;     /* 信道RLC信息 */
    RNC_MOI_ADJ_CELL_T          stAdjCell;      /* 邻小区 */
    RNC_MOI_OUT_TDD_128_CELL_T  stOutTdd128Cell;/* 外部TDD128小区 */
    RNC_MOI_OUT_TDD_384_CELL_T  stOutTdd384Cell;/* 外部TDD384小区 */
    RNC_MOI_OUT_FDD_CELL_T      stOutFddCell;   /* 外部FDD小区 */
    RNC_MOI_OUT_IS2000_CELL_T   stOutIs2000Cell;/* 外部IS2000小区 */
    RNC_MOI_OUT_GERAN_CELL_T    stOutGeranCell; /* 外部GERAN小区 */
    RNC_MOI_CELL_RRM_CAC_T      stCellRrmCac;   /* CAC算法 */
    RNC_MOI_CELL_RRM_SDCA_T     stCellRrmSdca;  /* SDCA算法 */
    RNC_MOI_CELL_RRM_FDCA_T     stCellRrmFdca;  /* FDCA算法 */
    RNC_MOI_CELL_RRM_PS_T       stCellRrmPs;    /* PS算法 */
    RNC_MOI_CELL_RRM_AMRC_T     stCellRrmAmrc;  /* AMRC算法 */
    RNC_MOI_CELL_RRM_RLS_T      stCellRrmRls;   /* RLS算法 */
    RNC_MOI_CELL_RRM_PC_T       stCellRrmPc;    /* PC算法 */
    RNC_MOI_CELL_RRM_HC_T       stCellRrmHc;    /* HC算法 */
    RNC_MOI_CELL_RRM_TIDE_T     stCellRrmTide;  /* TIDE算法 */
    RNC_MOI_CELL_RRM_HSUPA_T    stCellRrmHsupa; /* HSUPA信息 */
    RNC_MOI_CELL_RRM_LCC_T      stCellRrmLcc;   /* LCC算法 */
    RNC_MOI_CELL_RRM_LCC_STRAT_T    stCellRrmLccStrat;/* LCC算法策略 */
    RNC_MOI_CELL_RRM_INTRAFREQ_EVENT_T  stCellRrmIntraFreqEvent;    /* 同频事件信息 */
    RNC_MOI_CELL_RRM_INTERFREQ_EVENT_T  stCellRrmInterFreqEvent;    /* 异频事件信息 */
    RNC_MOI_CELL_RRM_INTERRATE_EVENT_T  stCellRrmInterRateEvent;    /* 系统间事件信息 */
    RNC_MOI_CELL_RRM_INTERNAL_EVENT_T   stCellRrmInternalEvent;     /* 内部测量事件信息 */
    RNC_MOI_CELL_RRM_POSITION_EVENT_T   stCellRrmPositionEvent;     /* 位置测量事件信息 */
    RNC_MOI_CELL_RRM_EVENTA_T   stCellRrmEventA;    /* EVENTA准则表 */
    RNC_MOI_CELL_RRM_EVENTB_T   stCellRrmEventB;    /* EVENTB准则表 */
    RNC_MOI_CELL_RRM_EVENTC_T   stCellRrmEventC;    /* EVENTC准则表 */
    RNC_MOI_CELL_RRM_EVENTD_T   stCellRrmEventD;    /* EVENTD准则表 */
    RNC_MOI_CELL_RRM_EVENTE_T   stCellRrmEventE;    /* EVENTE准则表 */
    RNC_MOI_CELL_RRM_EVENTF_T   stCellRrmEventF;    /* EVENTF准则表 */
    
    RNC_MOI_CELL_RRM_SYNC_T     stCellRrmSync;  /* SYNC算法 */
    RNC_MOI_CELL_RRM_LCS_T      stCellRrmLcs;   /* LCS算法 */
    RNC_MOI_CELL_RRM_TIMER_T    stCellRrmTimer; /* 算法定时器 */
    RNC_MOI_CELL_RRM_SELECT_T   stCellRrmSelect;/* 小区选择和重选 */
    RNC_MOI_CELL_RRM_HSDPA_T    stCellRrmHsdpa; /* HSDPA信息 */
    RNC_MOI_CELL_RRM_UPPCH_T    stCellRrmUppch; /* UpPcH Shifting自适应算法 */
    RNC_MOI_CELL_MEAS_NODEB_T   stCellMeasNodeb;/* NodeB测量 */
    /* NodeB测量周期准则 */
    RNC_MOI_CELL_MEAS_NODEB_PERIODIC_CRITERIA_T stNbPerdCrit;
    /* NodeB测量EventA/EventB准则 */
    RNC_MOI_CELL_MEAS_NODEB_EVENTAB_CRITERIA_T  stNbEntABCrit;
    /* NodeB测量EventC/EventD准则表 */
    RNC_MOI_CELL_MEAS_NODEB_EVENTCD_CRITERIA_T  stNbEntCDCrit;
    /* NodeB测量EventE/EventF准则表 */
    RNC_MOI_CELL_MEAS_NODEB_EVENTEF_CRITERIA_T  stNbEntEFCrit;
    /* UE测量同频测量信息 */    
    RNC_MOI_CELL_MEAS_UE_INTRAFREQ_T            stUeIntraFreq;
    /* UE测量同频测量准则 */
    RNC_MOI_CELL_MEAS_UE_INTRAFREQ_CRITERIA_T   stUeIntraFreqCrit;
    /* UE测量异频测量信息 */
    RNC_MOI_CELL_MEAS_UE_INTERFREQ_T            stUeInterFreq;
    /* UE测量异频测量准则 */
    RNC_MOI_CELL_MEAS_UE_INTERFREQ_CRITERIA_T   stUeInterFreqCrit;
    /* UE测量系统间测量信息 */
    RNC_MOI_CELL_MEAS_UE_INTERRAT_T             stUeInterRat;
    /* UE测量系统间测量准则 */
    RNC_MOI_CELL_MEAS_UE_INTERRAT_CRITERIA_T    stUeInterRatCrit;
    /* UE测量质量测量信息 */
    RNC_MOI_CELL_MEAS_UE_QUALITY_T              stUeQuality;
    /* UE测量业务量测量信息 */
    RNC_MOI_CELL_MEAS_UE_TRAFFICVOLUME_T        stUeTrafficVolume;
    /* UE测量位置测量信息 */
    RNC_MOI_CELL_MEAS_UE_POSITION_T             stUePosition;
    /* UE测量位置测量准则 */
    RNC_MOI_CELL_MEAS_UE_POSITION_CRT_T         stUePositionCrt;
    /* UE测量内部测量信息 */
    RNC_MOI_CELL_MEAS_UE_INTERNAL_T             stUeInternal;
    /* UE测量内部测量准则 */
    RNC_MOI_CELL_MEAS_UE_INTERNAL_CRITERIA_T    stUeInterCrit;
    /* UE测量周期准则 */
    RNC_MOI_CELL_MEAS_UE_PERIODIC_CRITERIA_T    stUePerdCrit;
    /* TPSS下行业务量测量 */
    RNC_MOI_CELL_MEAS_TPSS_TRAFFICVOLUME_T      stTpssTrafVol;
    /* TPSS上行质量测量 */
    RNC_MOI_CELL_MEAS_TPSS_QUALITY_T            stTpssQuality;
    RNC_MOI_MUX_ULRAB_T                 stMuxUlRab;         /* 上行RAB */
    RNC_MOI_MUX_ULRAB_RB_T              stMuxUlRabRb;       /* 上行RAB RB信息 */
    RNC_MOI_MUX_DLRAB_T                 stMuxDlRab;         /* 下行RAB */
    RNC_MOI_MUX_DLRAB_RB_T              stMuxDlRabRb;       /* 下行RAB RB信息 */
    RNC_MOI_MUX_DLRAB_SYNC_T            stMuxDlRabSync;     /* 业务同步参数信息 */
    RNC_MOI_MUX_RLC_SENDER_T            stMuxRlcSender;     /* 发送端RLC */
    RNC_MOI_MUX_RLC_RECEIVER_T          stMuxRlcRecv;       /* 接受端RLC */
    RNC_MOI_MUX_TFS_SEMISTATIC_T        stMuxTfsSemistatic; /* TFS半静态部分信息 */
    RNC_MOI_MUX_TFS_DYNAMIC_T           stMuxTfsDynamic;    /* TFS动态部分信息 */   
    RNC_MOI_MUX_IU_T                    stMuxIu;            /* Iu信息 */
    RNC_MOI_MUX_IU_RFCI_T               stMuxIuRfci;        /* Iu-RFCI信息 */
    RNC_MOI_MUX_PDCP_T                  stMuxPdcp;          /* PDCP信息 */
    RNC_MOI_MUX_PDCP_RFC2507_T          stMuxPdcpRfc2507;   /* PDCP-RFC2507信息 */
    RNC_MOI_MUX_PDCP_RFC3095_T          stMuxPdcpRfc3095;   /* PDCP-RFC3095信息 */
    RNC_MOI_MUX_ULTRAFFIC_T             stMuxUlTraf;        /* 上行组合业务 */
    RNC_MOI_MUX_ULTRAFFIC_MACC_T        stMuxUlTrafMacc;    /* 上行组合业务MAC-C配置 */
    RNC_MOI_MUX_ULTRAFFIC_MACD_T        stMuxUlTrafMacd;    /* 上行组合业务MAC-D配置 */
    RNC_MOI_MUX_ULTRAFFIC_TS_T          stMuxUlTrafTs;      /* 上行组合业务时隙资源配置 */
    RNC_MOI_MUX_ULTRAFFIC_L1_T          stMuxUlTrafL1;      /* 上行组合业务L1配置 */
    RNC_MOI_MUX_ULTRAFFIC_PC_T          stMuxUlTrafPc;      /* 上行组合业务业务功控参数 */
    RNC_MOI_MUX_DLTRAFFIC_T             stMuxDlTraf;        /* 下行组合业务 */
    RNC_MOI_MUX_DLTRAFFIC_MACC_T        stMuxDlTrafMacc;    /* 下行组合业务MAC-C配置 */
    RNC_MOI_MUX_DLTRAFFIC_MACD_T        stMuxDlTrafMacd;    /* 下行组合业务MAC-D配置 */
    RNC_MOI_MUX_DLTRAFFIC_TS_T          stMuxDlTrafTs;      /* 下行组合业务时隙资源配置 */
    RNC_MOI_MUX_DLTRAFFIC_L1_T          stMuxDlTrafL1;      /* 下行组合业务L1配置 */
    RNC_MOI_MUX_DLTRAFFIC_PC_T          stMuxDlTrafPc;      /* 上行组合业务业务功控参数 */
    RNC_MOI_PROTOCOL_NBAP_T             stProtNbap;         /* NBAP协议过程 */
    RNC_MOI_PROTOCOL_RRC_T              stProtRrc;          /* RRC协议过程 */
    RNC_MOI_PROTOCOL_RRCSETUPCAUSE_T    stProtRrcSetupCause;/* RRC连接建立原因 */
    RNC_MOI_PROTOCOL_RANAP_T            stProtRanap;        /* RANAP协议过程 */
    RNC_MOI_PROTOCOL_IUUP_T             stProtIuup;         /* IUUP协议过程 */
    RNC_MOI_PROTOCOL_IUB_SYNCTIMER_T    stProtIubSyncTimer; /* Iub同步定时器 */
    RNC_MOI_UE_TIMER_T                  stUeTimer;          /* UE定时器与计数器 */
    RNC_MOI_PROTOCOL_HSUPA_T            stProtHsupa;        /* HSUPA业务面控制参数 */
    
    /* 业务规划参数层 */
    RNC_MOI_TRAFFIC_QOS_T               stTrafficQos;       /* 业务Qos配置 */
    RNC_MOI_RRM_GLOBAL_PARA_T           stRrmGlobalPara;    /* 算法全局参数 */
    RNC_MOI_MBMS_ARITH_T                stMbmsArith;        /* MBMS算法 */
    RNC_MOI_MBMS_SFN_T                  stMbmsSfn;          /* MBMS区域规划 */
    RNC_MOI_MBMS_SFN_CELL_T             stMbmsSfnCell;      /* MBMS小区规划 */
    RNC_MOI_MBMS_SFN_TS_T               stMbmsSfnTs;        /* MBMS时隙资源 */
    RNC_MOI_MBMS_SFN_SCCPCHSET_T        stMbmsSfnSccpchSet; /* MBMS SCCPCHSET */
    RNC_MOI_MBMS_SFN_SCCPCH_T           stMbmsSfnSccpch;    /* MBMS SCCPCH */
    RNC_MOI_MBMS_SFN_FACH_T             stMbmsSfnFach;      /* MBMS FACH */

    RNC_MOI_PROHIBIT_SERVICE_T          stProhibitService;  /* 禁止接入业务 */
    RNC_MOI_HSUPA_SERVICE_T             stHsupaService;     /* HSUPA接入业务 */
    RNC_MOI_OVERLOAD_CTRL_T             stOverloadCtrl;     /* 负荷控制相关信息 */
    
    UINT8   aucMoiMax[SWP_MOI_LEN];     /* 控制UNION大小的占位符 */
} RNC_MOI_U;

/* RNC扩展MO的MO_ID */
typedef struct
{
    UINT32      ulMOC;
    RNC_MOI_U   unMOI;
} RNC_MO_ID_T;



/*******************************************************************************
*                               函数声明                                       *
*******************************************************************************/



/*******************************************************************************
*                               头文件保护结尾                                 *
*******************************************************************************/
#ifdef __cplusplus
}
#endif

#endif /* #ifdef SWP_MO_RNC_H */
/* 头文件结束, 末尾请保留一个空行 */

