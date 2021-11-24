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

/******************************** 头文件保护开头 *****************************/
#ifndef SWP_COMMON_H
#define SWP_COMMON_H

#ifdef  __cplusplus
extern "C" {
#endif

/********************************* 头文件包含 ********************************/

/******************************** 宏和常量定义 *******************************/

/******************************** 类型定义 ***********************************/
/* 网元类型宏定义 */
/*                             0       保留 */
#define SWP_NETYPE_NODEB       1    /* NodeB（保留） */
#define SWP_NETYPE_RNC         2    /* RNC */
#define SWP_NETYPE_MSCS        3    /* MSC Server */
#define SWP_NETYPE_MGW         4    /* MGW */
#define SWP_NETYPE_SGSN        5    /* SGSN */
#define SWP_NETYPE_GGSN        6    /* GGSN */
#define SWP_NETYPE_HLR         7    /* HLR */
#define SWP_NETYPE_TAN         8    /* TAN：集群接入网 */

/* 物理单板类型宏定义 */
/*                             0       保留    <以下为前插板> */
#define SWP_PHYBRD_NO          0    /* 0:表示没有前插卡和后插卡 */
#define SWP_PHYBRD_GSSA        1    /* GSSA，GE交换板 */
#define SWP_PHYBRD_TSSA        2    /* TSSA，TDM交换板 */
#define SWP_PHYBRD_MASA        3    /* MASA，多功能ATCA交换板 */
#define SWP_PHYBRD_MMPA        4    /* MMPA，多功能主处理板 */
#define SWP_PHYBRD_MNPA        5    /* MNPA，多功能网络处理板 */
#define SWP_PHYBRD_MDPA        6    /* MDPA，多功能DSP处理板 */
#define SWP_PHYBRD_IWFA        7    /* IWFA，网络互通功能板 */
#define SWP_PHYBRD_MEIA        8    /* MEIA，多功能E1接口板 */
#define SWP_PHYBRD_MAPA        9    /* MAPA，多功能ATM处理板 */
#define SWP_PHYBRD_ERIA       10    /* ERIA，E1中继接口板 */
#define SWP_PHYBRD_ORIA       11    /* ORIA，光中继接口板 */
#define SWP_PHYBRD_NSCA       12    /* NSCA，网络同步时钟板 */
#define SWP_PHYBRD_MMPA_OEM   13    /* OEM的MMPA，多功能主处理板 */
#define SWP_PHYBRD_V2_805     14    /* MIPC805单板（V2单板，测试用） */ 
#define SWP_PHYBRD_V2_NPEA    15    /* NPEA单板（V2评估板，测试用） */
#define SWP_PHYBRD_ALBA       16    /* ALBA，告警箱ALBA板 */
#define SWP_PHYBRD_MEPA       17    /* MEPA，多业务E1接口处理板 */
#define SWP_PHYBRD_CSPA       18    /* CSPA，多功能ATM处理板 */
/*                          ～63       保留 */
/*                            64       保留    <以下为后插板> */
#define SWP_PHYBRD_GSSB       65    /* GSSB，GE交换后插板 */
#define SWP_PHYBRD_TSSB       66    /* TSSB，TDM交换后插板 */
#define SWP_PHYBRD_MASB       67    /* MASB，多功能ATCA交换后插板 */
#define SWP_PHYBRD_MEIB       68    /* MEIB，多功能E1接口后插板 */
#define SWP_PHYBRD_SOIB       69    /* SOIB，STM-1接口后插板 */
#define SWP_PHYBRD_FEIB       70    /* FEIB，FE接口后插板 */
#define SWP_PHYBRD_GEIB       71    /* GEIB，GE接口后插板 */
#define SWP_PHYBRD_ORIB       72    /* ORIB，光中继接口后插板 */
#define SWP_PHYBRD_NSCB       73    /* NSCB，网络同步时钟后插板 */
#define SWP_PHYBRD_MEPB      74    /* MEPA ,   多业务E1接口后插板*/
/*                          ～95       保留 */
/*                            96       保留    <以下为背板> */
#define SWP_PHYBRD_MABP       97    /* MABP，多功能ATCA背板 */
#define SWP_PHYBRD_MABP_OEM   98    /* OEM的MABP，多功能ATCA背板 */
/*                         ～111       保留 */
/*                           112       保留    <以下为子板> */
#define SWP_PHYBRD_CSIC      113    /* CSIC，信道化STM-1接口卡 */
#define SWP_PHYBRD_NSIC      114    /* NSIC，非信道化STM-1接口卡 */
#define SWP_PHYBRD_FEIC      115    /* FEIC，FE接口卡 */
#define SWP_PHYBRD_GEIC      116    /* GEIC，GE接口卡 */
#define SWP_PHYBRD_PMCFE_OEM      117    /* PMC_FE子卡，用于MMPA_OEM板 */
#define SWP_PHYBRD_SPMC      118    /* GMPA上的黑匣子内存卡 */
#define SWP_PHYBRD_NQIC      119    /* NQIC，Non-channelization 4xSTM-1/STM-4 Interface Card，非通道化STM-4接口子卡 */



/*                         ～127       保留 */
#define SWP_PHYBRD_FTUB      125    /* FTUB，机框管理器风扇盒 */

/* 功能单板类型宏定义 */
/*                             0       保留    以下为CN的功能单板 */
#define SWP_FUNBRD_SAIB        1    /* ATM接口板 */
#define SWP_FUNBRD_CAIB        2    /* 信道化ATM接口板 */
#define SWP_FUNBRD_EAIB        3    /* E1 ATM接口板 */
#define SWP_FUNBRD_IPIB        4    /* IP接口板 */
#define SWP_FUNBRD_ERIB        5    /* E1中继接口板 */
#define SWP_FUNBRD_SRIB        6    /* STM-1中继接口板 */
#define SWP_FUNBRD_NSIB        7    /* 窄带信令接口板 */
#define SWP_FUNBRD_CGIB        8    /* 计费接口板 */
#define SWP_FUNBRD_TSNB        9    /* 一级TDM交换网板 */
#define SWP_FUNBRD_MPNB       10    /* 一级媒体面分组交换板 */
#define SWP_FUNBRD_CPNB       11    /* 一级控制面分组交换板 */
#define SWP_FUNBRD_USNB       12    /* 二级通用交换网板 */
#define SWP_FUNBRD_CLKB       13    /* 时钟板 */
#define SWP_FUNBRD_OMCB       14    /* 操作维护控制板 */
#define SWP_FUNBRD_SSPB       15    /* SGSN业务理板 */
#define SWP_FUNBRD_GSPB       16    /* GGSN业务处理板 */
#define SWP_FUNBRD_MSPB       17    /* MSCS业务处理板 */
#define SWP_FUNBRD_MGPB       18    /* MGW业务处理板 */
#define SWP_FUNBRD_HSPB       19    /* HLR业务处理板 */
#define SWP_FUNBRD_TCPB       20    /* 码型变换处理板 */
#define SWP_FUNBRD_IWFB       21    /* 网络互通板 */
#define SWP_FUNBRD_MRPB       22    /* 媒体资源板 */
#define SWP_FUNBRD_GUPB       23    /* GTP-U处理板 */
/*                          ～63       保留 */
/*                            64       保留    以下为RNC的功能单板 */
#define SWP_FUNBRD_GCPA       65    /* 全局处理板 */
#define SWP_FUNBRD_RSPA       66    /* 无线网络信令处理板 */
#define SWP_FUNBRD_RTPA       67    /* 无线网络业务处理板 */
#define SWP_FUNBRD_PTPA       68    /* GTPU处理板 */
#define SWP_FUNBRD_ONCA       69    /* NODEB操作维护数据路由板 */
#define SWP_FUNBRD_CGDA       70    /* 时钟板 */
#define SWP_FUNBRD_GTSA       71    /* 一级业务交换板 */
#define SWP_FUNBRD_GCSA       72    /* 一级控制交换板 */
#define SWP_FUNBRD_TCSA       73    /* 二级通用交换板 */

#define SWP_FUNBRD_CASA       90    /* 通道化ATM接口板（STM-1） */
#define SWP_FUNBRD_NASA       91    /* 非通道化ATM接口板（STM-1） */
#define SWP_FUNBRD_IAEA       92    /* 通道化E1接口板 */

#define SWP_FUNBRD_IAPA       93    /* IMA ATM E1接口处理板 */
#define SWP_FUNBRD_CAPA       94    /* ChannelLized ATM 接口处理板 */
#define SWP_FUNBRD_IPEA       95    /* IP E1 接口板 */
#define SWP_FUNBRD_IPSA       96    /* IP STM 接口板 */
#define SWP_FUNBRD_IPUA       97    /* IP UTP接口板（100M、1000M自适应） */
#define SWP_FUNBRD_IPCA       98    /* IP cPos接口板 */



#define SWP_FUNBRD_CADA       74    /* Iu Cs ATM接口板（STM-1） */
#define SWP_FUNBRD_PADA       75    /* Iu Ps ATM接口板（STM-1） */
#define SWP_FUNBRD_RADA       76    /* Iur ATM接口板（STM-1） */
#define SWP_FUNBRD_NADA       77    /* Nodeb ATM接口板（STM-1） */
#define SWP_FUNBRD_CIDA       78    /* Iu Cs IP接口板(FE/GE/STM-1) */
#define SWP_FUNBRD_PIDA       79    /* Iu Ps IP接口板(FE/GE/STM-1) */
#define SWP_FUNBRD_RIDA       80    /* Iur IP接口板(FE/GE/STM-1) */
#define SWP_FUNBRD_NIDA       81    /* Nodeb IP接口板(FE/GE/STM-1) */
#define SWP_FUNBRD_CAEA       82    /* Iu Cs ATM接口板（E1） */
#define SWP_FUNBRD_PAEA       83    /* Iu Ps ATM接口板（E1） */
#define SWP_FUNBRD_RAEA       84    /* Iur ATM接口板（E1） */
#define SWP_FUNBRD_NAEA       85    /* Nodeb ATM接口板（E1） */
#define SWP_FUNBRD_CIEA       86    /* Iu Cs IP接口板(E1) */
#define SWP_FUNBRD_PIEA       87    /* Iu Ps IP接口板(E1) */
#define SWP_FUNBRD_RIEA       88    /* Iur IP接口板(E1) */
#define SWP_FUNBRD_NIEA       89           /* Nodeb IP接口板(E1) */
#define SWP_FUNBRD_ABOX       254    /* ABOX，告警箱ABOX板 */
/*                         ～255       保留 */
/* 以下为煤炭项目的功能单板 */
/*120 保留 added by panyr for TAN 2007-3-2 */
#if (SWP_NE_TYPE == SWP_NETYPE_TAN)
#define	SWP_FUNBRD_TAN_CNPB	SWP_FUNBRD_GCPA     /*  全局处理板/CN信令处理板 <65>*/
#define	SWP_FUNBRD_TAN_RNPB	SWP_FUNBRD_RSPA     /*  RNC信令处理板  <66>         */
#define	SWP_FUNBRD_TAN_UPPB_R	SWP_FUNBRD_RTPA     /*  RNC 业务处理板  <67>            */
#define	SWP_FUNBRD_TAN_UPPB_N	123     /*  CN 业务处理板              */       
#define	SWP_FUNBRD_TAN_SAIB	SWP_FUNBRD_NASA     /*  ATM接口板  <91>             */
#define	SWP_FUNBRD_TAN_IPIB	SWP_FUNBRD_ONCA     /*  IP接口板   <5>             */
#define	SWP_FUNBRD_TAN_TEIB	SWP_FUNBRD_IAEA     /*  窄带TDM接口板   <92>        */
#define	SWP_FUNBRD_TAN_CLKB	SWP_FUNBRD_CGDA     /*  时钟板  <70>                */
#define	SWP_FUNBRD_TAN_USNB	SWP_FUNBRD_TCSA     /*  二级通用交换网板   <73>     */
#else
#define	SWP_FUNBRD_TAN_CNPB	121     /*  全局处理板/CN信令处理板 <65>*/
#define	SWP_FUNBRD_TAN_RNPB	122     /*  RNC信令处理板  <66>         */
#define	SWP_FUNBRD_TAN_UPPB_R	124     /*  RNC 业务处理板  <67>            */
#define	SWP_FUNBRD_TAN_UPPB_N	123     /*  CN 业务处理板              */       
#define	SWP_FUNBRD_TAN_SAIB	125     /*  ATM接口板  <91>             */
#define	SWP_FUNBRD_TAN_IPIB	126     /*  IP接口板   <5>             */
#define	SWP_FUNBRD_TAN_TEIB	127     /*  窄带TDM接口板   <92>        */
#define	SWP_FUNBRD_TAN_CLKB	128     /*  时钟板  <70>                */
#define	SWP_FUNBRD_TAN_USNB	129     /*  二级通用交换网板   <73>     */
#endif

/* 抽象功能单板类型宏定义 */
#define SWP_ABS_FUNBRD_BASE          0x80    /* 抽象单板类型的基值 */
#define SWP_ABS_FUNBRD_IF_ATM        0x81    /* ATM接口板 */
#define SWP_ABS_FUNBRD_IF_IP         0x82    /* IP接口板 */
#define SWP_ABS_FUNBRD_IF_LTDM       0x83    /* 窄带TDM接口板 */
#define SWP_ABS_FUNBRD_CLK           0x84    /* 时钟板 */
#define SWP_ABS_FUNBRD_SW_LS         0x85    /* 二级交换板 */
#define SWP_ABS_FUNBRD_SW_IGS        0x86    /* IP一级交换板 */
#define SWP_ABS_FUNBRD_C_MAIN        0x87    /* 主控 */
#define SWP_ABS_FUNBRD_C_APP         0x88    /* 信令处理 */
#define SWP_ABS_FUNBRD_U_GTPU        0x89    /* GTPU处理 */
#define SWP_ABS_FUNBRD_U_APP         0x8a    /* 用户面处理 */
#define SWP_ABS_FUNBRD_TOTAL         (0x8a-0x81 + 1)    /* 总数 */

/*                                   ～0x9F <保留> */

/* 操作系统类型宏定义 */
/*                             0       保留 */
#define SWP_OS_WINDOWS         1    /* Windows */
#define SWP_OS_VXWORKS         2    /* VxWorks */
#define SWP_OS_LINUX           3    /* 嵌入式Linux */
#define SWP_OS_NONE            4    /* 无操作系统 */
/*                         ～255       保留 */

/* VxWorks版本宏定义 */
/*                             0       保留 */
#define SWP_VX_54              1    /* VxWorks5.4 */
#define SWP_VX_55              2    /* VxWorks5.5 */
#define SWP_VX_60              3    /* VxWorks6.0 */
/*                         ～255       保留 */

/* CPU角色类型宏定义 */
/*                             0       保留 */
#define SWP_CPUROLE_HOST       1    /* 主CPU */
#define SWP_CPUROLE_SLAVE      2    /* 从CPU */
#define SWP_CPUROLE_DSP        3    /* DSP */
/*                         ～255       保留 */

/* CPU类型宏定义 */
/* 不建议引入CPU类型相关的特殊处理，除非特别需要（例如：字节序、精确定时等）*/
/*                             0       保留    <以下为MPC系列通用处理器> */
#define SWP_CPU_MPC8247        1    /* MPC8247处理器（通常400MHz） */
#define SWP_CPU_MPC8560        2    /* MPC8560处理器（通常833MHz） */
#define SWP_CPU_MPC7447A       3    /* MPC87447A处理器（1.5GMHz） */
#define SWP_CPU_MPC7448        4    /* MPC87448处理器（2.0GMHz） */
#define SWP_CPU_MPC8260        5    /* MPC8260处理器（通常266MHz） */
#define SWP_CPU_MPC7410        6    /* RNCV2硬件平台中用到 */
#define SWP_CPU_MPC8266        7    /* RNCV2硬件平台中用到 */
/*                          ～31       保留 */
/*                            32       保留    <以下为X86系列通用处理器> */
#define SWP_CPU_PENTIUMM      33    /* PentiumM处理器（1.8G） */
/*                          ～63       保留 */
/*                            64       保留    <以下为MIPS系列通用处理器> */
/*                          ～95       保留 */
/*                            96       保留    <以下为ARM系列通用处理器> */
#define SWP_CPU_XSCALE        97    /* XSCALE处理器 */
/*                         ～127       保留 */
/*                           128       保留    <以下为网络处理器> */
#define SWP_CPU_ME2400       129    /* IXP2400的微引擎 */
/*                         ～159       保留 */
/*                           160       保留    <以下为DSP> */
#define SWP_CPU_C6415        161    /* TMS320C6415 */
#define SWP_CPU_C6455        162    /* TMS320C6455 */
#define SWP_CPU_C6416        163    /* TMS320C6416 */
/*                         ～191       保留 */
/*                         ～255       保留 */

#define SWP_CPU_BIG				41
#define SWP_CPU_LITTLE			42
                          
/* 子系统编号宏定义 */    
/*                             0       保留 */
#define SWP_SUBSYS_DRV         1    /* BSP & Driver，底层驱动子系统 */
#define SWP_SUBSYS_CPSS        2    /* CPSS，支撑平台子系统 */
#define SWP_SUBSYS_SMSS        3    /* SMSS，系统控制子系统 */
#define SWP_SUBSYS_RDBS        4    /* RDBS，资源和数据库子系统 */
#define SWP_SUBSYS_OAMS        5    /* OAMS，操作维护子系统 */
#define SWP_SUBSYS_CPM         6    /* TNBS，传输承载子系统 */
#define SWP_SUBSYS_UPM         7    /* TNSS，传输网络子系统 */
#define SWP_SUBSYS_HSPS        8    /* HSPS，RNC高层信令处理子系统 */
#define SWP_SUBSYS_TPSS        9    /* TPSS，RNC业务处理子系统 */
#define SWP_SUBSYS_PSSS       10    /* PSSS，PS业务子系统 */
#define SWP_SUBSYS_HSSS       11    /* HSSS，HLR业务子系统 */
#define SWP_SUBSYS_CSSS       12    /* CSSS，CS业务子系统 */
#define SWP_SUBSYS_CSUP       13    /* CSUP，CS用户面子系统 */
#define SWP_SUBSYS_RDBSR      14    /* RDBSR，RNC资源与数据库子系统 */

/*add for wlan*/
#define SWP_SUBSYS_AC		  15
/*add end for wlan*/

/*add for tan CSS 20070411 begin */
#define SWP_SUBSYS_TANCSS     19    /* TANCSS，TAN CS子系统 */
/*add for tan CSS 20070411 end */
/*                          ～20       保留 */
#define SWP_SUBSYS_ALL       255    /* 所有子系统 */
#define SWP_SUBSYS_MAX_NUM    20     /* 子系统最大个数 */

/* 18~27 煤炭产品预留 */

/* 各子系统名宏定义 */
#define SWP_SUBSYSNAME_MAX_LEN  8
#define SWP_SUBSYSNAME_DRV              " DRV"      /* BSP & Driver，底层驱动子系统 */
#define SWP_SUBSYSNAME_CPSS             "CPSS"      /* CPSS，支撑平台子系统 */
#define SWP_SUBSYSNAME_SMSS             "SMSS"      /* SMSS，系统控制子系统 */
#define SWP_SUBSYSNAME_RDBS             "RDBS"      /* RDBS，资源和数据库子系统 */
#define SWP_SUBSYSNAME_OAMS             "OAMS"      /* OAMS，操作维护子系统 */
#define SWP_SUBSYSNAME_CPM				"CPM"
#define SWP_SUBSYSNAME_TNBS             "TNBS"      /* TNBS，传输承载子系统 */
#define SWP_SUBSYSNAME_UPM              "UPM"      /* TNSS，传输网络子系统 */
#define SWP_SUBSYSNAME_HSPS             "HSPS"      /* HSPS，RNC高层信令处理子系统 */
#define SWP_SUBSYSNAME_TPSS             "TPSS"      /* TPSS，RNC业务处理子系统 */
#define SWP_SUBSYSNAME_PSSS             "PSSS"      /* PSSS，PS业务子系统 */
#define SWP_SUBSYSNAME_HSSS             "HSSS"      /* HSSS，HLR业务子系统 */
#define SWP_SUBSYSNAME_CSSS             "CSSS"      /* CSSS，CS业务子系统 */
#define SWP_SUBSYSNAME_CSUP             "CSUP"      /* CSUP，CS用户面子系统 */
#define SWP_SUBSYSNAME_RDBSR            "RDBSR"     /* RDBSR，RNC资源和数据库子系统 */

/* 物理板卡版本、子卡版本、后插卡版本类型宏定义：
   SWP_PHYBRD_VER
   SWP_PHYBRD_CHILDBRD_VER
   SWP_PHYBRD_BACKBRD_VER
   取值 */    
/*                                   0         保留 */
#define SWP_PHYBRD_VER_MNPA_V101     0x0501    /* 物理板MNPA版本V101 */
#define SWP_PHYBRD_VER_MNPA_V102     0x0502    /* 物理板MNPA版本V102 */
#define SWP_PHYBRD_VER_V101          0x0501    /* 物理板MNPA版本V101 */
#define SWP_PHYBRD_VER_V102          0x0502    /* 物理板MNPA版本V102 */
#define SWP_PHYBRD_VER_ANY           0xffff    /* 任意版本 */

/* VXWORKS版本类型宏定义：SWP_VER_TYPE取值 */    
/*                                   0       保留 */
#define SWP_VER_BOOT                 1    /* BOOT版本 */
#define SWP_VER_APP                  2    /* APP版本（完整功能版本） */

/* 字节序类型宏定义：SWP_ENDIAN_TYPE取值 */    
/* 说明：
      不建议直接使用SWP_ENDIAN_TYPE进行数据结构定义，最好是运行时
      根据判断本CPU类型为BIG_ENDIAN还是LITTLE_ENDIAN进行处理。
 */
/*                                   0       保留 */
#define SWP_ENDIAN_BIG               1    /* 大端字节序（例如PPC处理器） */
#define SWP_ENDIAN_LITTLE            2    /* 小端字节序（例如x86处理器） */

/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/

/******************************** 头文件保护结尾 *****************************/
#ifdef  __cplusplus
}
#endif

#endif /* SWP_COMMON_H */
/******************************** 头文件结束 *********************************/
