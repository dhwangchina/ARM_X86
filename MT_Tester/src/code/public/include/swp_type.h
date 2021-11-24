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
#ifndef SWP_TYPE_H
#define SWP_TYPE_H

/********************************* 头文件包含 ********************************/
#ifdef  __cplusplus
extern "C" {
#endif

/* includes */
#include "swp_common.h"

#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
#include "vxWorks.h"

/* #include "cbioLib.h" */
#include "dirent.h"
#include "dosFsLib.h" 
#include "iosLib.h"
#include "logLib.h"
#include "msgQLib.h"
#include "private/msgQLibP.h"
#include "private/semLibP.h"
/* #include "ramDiskCbio.h" */
#include "semLib.h"
#include "stat.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "sysLib.h"
#include "taskLib.h"
#include "tickLib.h"
#include "time.h" 
#include "timers.h"
#include "vmLib.h" 
#include "intLib.h"

#include "errno.h"
#include "errnoLib.h"
#include "ioLib.h"
#include "loadLib.h"
#include "moduleLib.h"
#include "ramDrv.h"
#include "symLib.h"
#include "unldLib.h"
    
#endif /* End "#if (SWP_OS_TYPE == SWP_OS_VXWORKS)" */

#if (SWP_OS_TYPE == SWP_OS_WINDOWS)
#include "WINDOWS.H"

#include "direct.h"
#include "errno.h"
#include "fcntl.h"
#include "io.h"
#include "process.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "time.h"
#include "wininet.h"

#endif /* End "#if (SWP_OS_TYPE == SWP_OS_WINDOWS)" */

#if (SWP_OS_TYPE == SWP_OS_LINUX)

#include "pthread.h"
#include "stdio.h"
#include "errno.h"
#include "fcntl.h"
#include "stdarg.h"
#include "stdlib.h"
#include "string.h"
#include "sys/stat.h"
#include "sys/types.h"
#include "time.h"
#include "strings.h"
#include "sys/times.h"
#include "stdint.h"
#include "semaphore.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "math.h"
#include "unistd.h"
#define cps__oams_shcmd_printf  printf
#define oams_shcmd_printf  printf

#endif /* End "#if (SWP_OS_TYPE == SWP_OS_LINUX)" */

#if (SWP_CPUROLE_TYPE==SWP_CPUROLE_DSP)
#undef CPSS_VOS_WINDOWS
#undef CPSS_VOS_VXWORKS

#undef CPSS_VOS_LINUX

#include "ctype.h"
#include "errno.h"
#include "stdarg.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "time.h"
#ifndef NULL
#ifdef __cplusplus
#define NULL    (0)
#else
#define NULL    ((void *) 0)
#endif
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

#ifndef CPSS_OK
#define CPSS_OK (0)
#endif

#ifndef CPSS_ERROR
#define CPSS_ERROR  (-1)
#endif

#ifndef NO_WAIT
#define NO_WAIT (0)
#endif
#ifndef WAIT_FOREVER
#define WAIT_FOREVER (0xffffffff)
#endif

#define VOID void

#define NUM_ENTS(array) (sizeof (array) / sizeof ((array) [0]))

/* typedefs */

typedef	char    INT8;
typedef	short   INT16;
typedef	int     INT32;

typedef	unsigned char   UINT8;
typedef	unsigned short  UINT16;
typedef	unsigned int    UINT32;

typedef	char	CHAR;
typedef	short   SHORT;
typedef	long    LONG;

typedef	unsigned char	UCHAR;
typedef unsigned short	USHORT;
typedef	unsigned int	UINT;
typedef unsigned long	ULONG;


typedef	int     BOOL;
typedef	int     STATUS;

/*typedef void	VOID;*/


#endif

/******************************** 宏和常量定义 *******************************/


#ifndef NULL
#ifdef __cplusplus
#define NULL    (0)
#else
#define NULL    ((void *) 0)
#endif
#endif

#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
#define SOCKET_ERROR ERROR
#define INVALID_SOCKET ERROR
#endif

#if (SWP_OS_TYPE == SWP_OS_LINUX)
#define SOCKET_ERROR ERROR
#define INVALID_SOCKET ERROR
#endif

#ifndef OK
#define OK	0
#endif

#ifndef ERROR
#define ERROR	-1
#endif

#ifndef FALSE
#define FALSE   (0)
#endif

#ifndef TRUE
#define TRUE    (1)
#endif

#ifndef CPSS_OK
#define CPSS_OK (0)
#endif

#ifndef CPSS_ERROR
#define CPSS_ERROR  (-1)
#endif

#if (SWP_OS_TYPE == SWP_OS_WINDOWS)
#ifndef NO_WAIT
#define NO_WAIT (0)
#endif
#ifndef WAIT_FOREVER
#define WAIT_FOREVER INFINITE
#endif
#endif

#if (SWP_OS_TYPE == SWP_OS_LINUX)
#ifndef NO_WAIT
#define NO_WAIT (0)
#endif
#ifndef WAIT_FOREVER
#define WAIT_FOREVER (0xffffffff)
#endif
#endif

/******************************** 类型定义 ***********************************/
/* WINDOWS下基本数据类型定义 */
#if (SWP_OS_TYPE == SWP_OS_WINDOWS)
typedef    char    INT8;
typedef    short   INT16;

typedef    unsigned char   UINT8;
typedef    unsigned short  UINT16;
typedef int STATUS;

#endif /* End "#if (SWP_OS_TYPE == SWP_OS_WINDOWS)" */

/* VXWORKS 下基本数据类型定义 */
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
typedef    char    CHAR;
typedef short   SHORT;
typedef    int INT;
typedef long    LONG;
#endif /* End "#if (SWP_OS_TYPE == SWP_OS_VXWORKS)" */

/* LINUX下基本数据类型定义 */
#if (SWP_OS_TYPE == SWP_OS_LINUX)
typedef    char    INT8;
typedef    short   INT16;
typedef    int     INT32;

typedef    unsigned char   UINT8;
typedef    unsigned short  UINT16;
typedef	   unsigned int    UINT32;
typedef    unsigned long long UINT64;

typedef    void	   VOID;
typedef	   int     BOOL;

typedef    char    CHAR;
typedef    short   SHORT;
typedef    int     INT;
typedef    long    LONG;

typedef	   unsigned char	UCHAR;
typedef    unsigned short	USHORT;
typedef    unsigned int	    UINT;
typedef    unsigned long	ULONG;

typedef int STATUS;

#endif /* End "#if (SWP_OS_TYPE == SWP_OS_LINUX)" */
/* add end by  for linux support */

/* 公共数据类型定义 */
typedef    char*  STRING;
typedef UINT8  BITS8;/* bt */
typedef UINT16 BITS16;/* bt */
typedef UINT32 BITS32;/* bt */

/* 函数指针类型定义 */
typedef void    (*VOID_FUNC_PTR) (); /* pointer to function returning void */

/* 通信地址定义 */
/* 逻辑地址定义 */
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

/* 纤程ID，网元内唯一表示一个纤程 */
typedef struct tagCPSS_COM_PID
{
    CPSS_COM_LOGIC_ADDR_T stLogicAddr; /* 逻辑地址 */

    /* 地址标志：
     * CPSS_COM_ADDRFLAG_MASTER，本逻辑地址的中的主用板
     * CPSS_COM_ADDRFLAG_SLAVE，本逻辑地址的中的备用板
     */
    UINT32 ulAddrFlag;
    UINT32 ulPd;    /* 纤程描述符 */
} CPSS_COM_PID_T;

/* 通信头定义*/
typedef struct tagCPSS_COM_MSG_HEAD
{
    CPSS_COM_PID_T stDstProc;  /* 目的纤程 */
    CPSS_COM_PID_T stSrcProc;  /* 源纤程 */
    UINT32 ulMsgId; /* 用户的消息号 */
    
    /* 优先级别：
     * CPSS_COM_PRIORITY_LOW，低优先级
     * CPSS_COM_PRIORITY_NORMAL，普通优先级
     */
    UINT8 ucPriFlag;
    
    /* 可靠标志：
     * CPSS_COM_ACK，要求可靠发送
     * CPSS_COM_NOT_ACK，要求不可靠发送
     */
    UINT8 ucAckFlag;    
    
    UINT8 ucShareFlag;/* 共享内存标记：0 不共享，其它表示共享 */
    
    UINT8 usReserved;
    UINT8 *pucBuf;  /* 消息体指针 */
    UINT32 ulLen; /* 消息体长度 */
} CPSS_COM_MSG_HEAD_T;

/******************************** 全局变量声明 *******************************/
/*extern */
/******************************** 外部函数原形声明 ***************************/
/*extern */
/******************************** 头文件保护结尾 *****************************/
#ifdef  __cplusplus
}
#endif /* End "#ifdef  __cplusplus" */

#endif /* SWP_TYPE_H */
/******************************** 头文件结束 *********************************/

