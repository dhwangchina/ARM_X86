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
#ifndef SMSS_BOOT_H
#define SMSS_BOOT_H
/********************************* 头文件包含 ********************************/

#define LOCAL	0
#define	HOST	1

#define BOOT_OK		0
#define BOOT_ERROR	1

#define SWP_CPU_BIG 0
#define SWP_CPU_LITTLE 1

#define DOWNLOAD_ERROR	1
#define VERCHECK_ERROR	2

#define SWP_FUNBRD_GCPA 0
#define SWP_FUNBRD_RSPA 1

#define	TRUE 0
#define	FALSE 1

typedef    char    INT8;
typedef    short   INT16;
typedef    int     INT32;

typedef    unsigned char   UINT8;
typedef    unsigned short  UINT16;
typedef	   unsigned int    UINT32;

typedef    void	   VOID;
typedef	   int     BOOL;

typedef    char*	STRING;
typedef    char		CHAR;

#define CPSS_VK_PD(usPType, usPNo) \
        (UINT32)((((usPType)&0xFFFF)<<16)|((usPNo)&0xFFFF))

#define CPSS_COM_PRIORITY_NORMAL   ((UINT32)(0x1)) /* 普通优先级 */

#define CPSS_COM_ADDRFLAG_PHYSICAL ((UINT32)(0x10)) /* 物理地址 */

#define CPSS_COM_NOT_ACK           ((UINT32)(0x0)) /* 要求不可靠发送（无确认）*/

#define SMSS_VERM_PROC          CPSS_VK_PD(22,0)  /* 版本管理 */

#define SMSS_SYSCTL_PROC        CPSS_VK_PD(21,0)

#define COM_TEST_PROC			CPSS_VK_PD(30,0)



#define CPSS_COM_UDP_SERVER_PORT (5050)

#define COM_TEST_BOOT			0x79100011

#if  (SWP_CPU_TYPE == SWP_CPU_BIG)

#define  boot_ntohs(x)   ((UINT16)(x))

#define  boot_ntohl(x)   ((UINT32)(x))

#define  boot_htons(x)   ((UINT16)(x))

#define  boot_htonl(x)   ((UINT32)(x))

#elif  (SWP_CPU_TYPE == SWP_CPU_LITTLE)

#define boot_ntohl(x)   (((((UINT32)(x)) & 0x000000ff) << 24) | \
                         ((((UINT32)(x)) & 0x0000ff00) <<  8) | \
                         ((((UINT32)(x)) & 0x00ff0000) >>  8) | \
                         ((((UINT32)(x)) & 0xff000000) >> 24))

#define boot_htonl(x)   (((((UINT32)(x)) & 0x000000ff) << 24) | \
                         ((((UINT32)(x)) & 0x0000ff00) <<  8) | \
                         ((((UINT32)(x)) & 0x00ff0000) >>  8) | \
                         ((((UINT32)(x)) & 0xff000000) >> 24))

#define boot_ntohs(x)   (((((UINT16)(x)) & 0x00ff) << 8) | \
                         ((((UINT16)(x)) & 0xff00) >> 8))


#define boot_htons(x)   (((((UINT16)(x)) & 0x00ff) << 8) | \
                         ((((UINT16)(x)) & 0xff00) >> 8))

#endif

typedef struct tagCPSS_COM_LOGIC_ADDR
{
    UINT8  ucModule;        /* 模块号 */
    UINT8  ucSubGroup;      /* 子组号 */
    UINT16 usGroup;         /* 组号 */
} CPSS_COM_LOGIC_ADDR_T;

/* 物理地址定义 */
typedef struct tagCPSS_COM_PHY_ADDR
{
    UINT8 ucFrame;         /* 物理机架号，[1..5] */
    UINT8 ucShelf;         /* 物理机框号， [1..4] */
    UINT8 ucSlot;          /* 物理槽号     [1..20] */
    UINT8 ucCpu;           /* CPU编号， 从1（HOST）开始， [1..20] */
} CPSS_COM_PHY_ADDR_T;

/*版本结构定义*/
typedef struct {
	CHAR        aucHeadWord[8];     /* 文件校验字, 固定内容"MyComp" */
    UINT16      usBuildYear;        /* 编译时间(年) */
    UINT8       ucBuildMonth;       /* 编译时间(月) */
    UINT8       ucBuildDay;         /* 编译时间(日) */
    UINT32      ulVerFileLen;       /* 版本文件长度(不含头部, 单位:字节) */
    UINT32      ulVerDataCheckSum;  /* 版本数据校验和 */
} VER_FILE_HEAD_T;

/*版本文件变更响应消息结构定义：*/
typedef struct {
	INT32		iResult;			/*结果，0：成功，其它：错误码*/
} CPS_VER_UPDATE_RSP_MSG_T;

/*版本文件变更请求消息结构定义：*/
typedef struct {
    CHAR        aucVerFile[256];     /* 版本文件名称 */
} CPS_VER_UPDATE_REQ_MSG_T;

/* 纤程ID，网元内唯一表示一个纤程 */
typedef struct tagCPSS_COM_PID
{
    CPSS_COM_LOGIC_ADDR_T stLogicAddr; /* 逻辑地址 */
    UINT32 ulAddrFlag;
    UINT32 ulPd;    /* 纤程描述符 */
} CPSS_COM_PID_T;

/* 通信头定义*/
typedef struct tagCPSS_COM_MSG_HEAD
{
    CPSS_COM_PID_T stDstProc;  /* 目的纤程 */
    CPSS_COM_PID_T stSrcProc;  /* 源纤程 */
    UINT32 ulMsgId; /* 用户的消息号 */
    UINT8 ucPriFlag;
    UINT8 ucAckFlag;
    UINT8 ucShareFlag;/* 共享内存标记：0 不共享，其它表示共享 */
    UINT8 usReserved;
    UINT8 *pucBuf;  /* 消息体指针 */
    UINT32 ulLen; /* 消息体长度 */
} CPSS_COM_MSG_HEAD_T;

/*通信可靠传输头定义*/
typedef struct tagCPSS_COM_TRUST_HEAD
{
#if (SWP_CPU_TYPE == SWP_CPU_LITTLE)
    UINT8   hdrLen:5; /* header length: unit is 4 bytes */
    UINT8   ver:3; /* version */
#elif (SWP_CPU_TYPE == SWP_CPU_BIG)
    UINT8   ver:3; /* version */
    UINT8   hdrLen:5; /* header length: unit is 4 bytes */
#endif

    UINT8  ucPType; /* type of protocol */

#if (SWP_CPU_TYPE == SWP_CPU_LITTLE)
    UINT8   fragOffset:5 ;
    UINT8   fragFlag:3 ;
#elif (SWP_CPU_TYPE == SWP_CPU_BIG)
    UINT8   fragFlag:3 ;
    UINT8   fragOffset:5 ;
#endif

    UINT8   fragOffsetEx ; /*数据片偏移量的低8位*/

    UINT16  usReserved;
    UINT16  usSliceLen; /* unknown? */
    UINT16  usSeq; /* sequence number */
    UINT16  usAck; /* acknowledgement number */
    UINT16  usFlowCtrl; /* flow control */
    UINT16  usCheckSum; /* header checksum */

}CPSS_COM_TRUST_HEAD_T ;

#define CPSS_COM_UDP_SERVER_PORT (5050)

#define CPS_VER_CFG_QUERY_REQ_MSG		(0x75025002)
#define CPS_VER_CFG_QUERY_RSP_MSG		(0x75025005)
#define CPS_VER_UPDATE_RSP_MSG			(0x75025004)

#define DOWNLOAD_ERROR	1
#define VERCHECK_ERROR	2

#endif /* BOOT_H_ */
