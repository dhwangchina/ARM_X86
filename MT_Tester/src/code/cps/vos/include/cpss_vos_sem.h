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
#ifndef __INCcpss_vos_semh
#define __INCcpss_vos_semh

#ifdef __cplusplus
extern "C" {
#endif
/********************************* 头文件包含 ********************************/
 
/* includes */

#include "cpss_type.h"
#include "cpss_config.h"
/******************************** 宏和常量定义 *******************************/

/* defines */
/******************************** 类型定义 ***********************************/

/* typedef */

#ifdef CPSS_VOS_VXWORKS
#define VOS_SEM_TYPE_BINARY SEM_TYPE_BINARY /* binary semaphore */
#define VOS_SEM_TYPE_MUTEX  SEM_TYPE_MUTEX /* mutual exclusion semaphore */
#define VOS_SEM_TYPE_COUNTING   SEM_TYPE_COUNTING /* counting semaphore */
#endif

#ifdef CPSS_VOS_WINDOWS
#define VOS_SEM_TYPE_BINARY 0x0 /* binary semaphore */
#define VOS_SEM_TYPE_MUTEX	0x1 /* mutual exclusion semaphore */
#define VOS_SEM_TYPE_COUNTING   0x2 /* counting semaphore */
#endif

#ifdef CPSS_DSP_CPU
#define VOS_SEM_TYPE_BINARY 0x0 /* binary semaphore */
#define VOS_SEM_TYPE_MUTEX	0x1 /* mutual exclusion semaphore */
#define VOS_SEM_TYPE_COUNTING   0x2 /* counting semaphore */
#endif

#ifdef CPSS_VOS_LINUX
#define VOS_SEM_TYPE_BINARY 0x0 /* binary semaphore */
#define VOS_SEM_TYPE_MUTEX	0x1 /* mutual exclusion semaphore */
#define VOS_SEM_TYPE_COUNTING   0x2 /* counting semaphore */
#endif

/* options */

#ifdef CPSS_VOS_VXWORKS
#define VOS_SEM_Q_FIFO  SEM_Q_FIFO
#define VOS_SEM_Q_PRI   SEM_Q_PRIORITY
#endif

#ifdef CPSS_VOS_WINDOWS
#define VOS_SEM_Q_FIFO  0x00
#define VOS_SEM_Q_PRI   0x01
#endif

#ifdef CPSS_DSP_CPU
#define VOS_SEM_Q_FIFO  0x00
#define VOS_SEM_Q_PRI   0x01
#endif

#ifdef CPSS_VOS_LINUX
#define VOS_SEM_Q_FIFO  0x00
#define VOS_SEM_Q_PRI   0x01
#endif

/* semaphore descriptor */

#define VOS_SEM_DESC_INVALID    0xFFFFFFFF /* invalid */

/* typedefs */

/* semaphore state */

#ifdef CPSS_VOS_VXWORKS
typedef enum 
     {
     VOS_SEM_EMPTY = SEM_EMPTY,	/* 0: semaphore not available */
     VOS_SEM_FULL = SEM_FULL,/* 1: semaphore available */
     } CPSS_VOS_SEM_STATE_E;
#endif

#ifdef CPSS_VOS_WINDOWS 
typedef enum 
     {
     VOS_SEM_EMPTY,	/* 0: semaphore not available */
     VOS_SEM_FULL /* 1: semaphore available */
     } CPSS_VOS_SEM_STATE_E;
#endif

#ifdef CPSS_DSP_CPU
typedef enum 
     {
     VOS_SEM_EMPTY,	/* 0: semaphore not available */
     VOS_SEM_FULL /* 1: semaphore available */
     } CPSS_VOS_SEM_STATE_E;
#endif

#ifdef CPSS_VOS_LINUX
typedef enum 
     {
     VOS_SEM_EMPTY,	/* 0: semaphore not available */
     VOS_SEM_FULL /* 1: semaphore available */
     } CPSS_VOS_SEM_STATE_E;
#endif


typedef struct vos_sem_t
{
	UINT8	ucType; /* semaphore type */
	UINT8	ucOptions; /* semaphore options */
	void *pvSem;
} VOS_SEM_ID_T;

typedef VOS_SEM_ID_T* VOS_SEM_ID;


/* semaphore descriptor */

typedef struct vos_sem_desc_t
     {
     VOS_SEM_ID  semId; /* semaphore ID */
     BOOL    bInuse; /* inuse */
     } CPSS_VOS_SEM_DESC_T;

/* semaphore information */

typedef struct vos_sem_info_t
     {
     UINT32  ulSemDesc; /* semaphore descriptor */
     VOS_SEM_ID  semId; /* semaphore ID */
     UINT8 ucType; /* semaphore type */
     UINT8 ucOptions; /* semaphore options */
     } CPSS_VOS_SEM_INFO_T;
/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/
/* function declarations */

extern INT32    cpss_vos_sem_lib_init (VOID);
extern UINT32   cpss_vos_sem_b_create (INT32 lOptions, CPSS_VOS_SEM_STATE_E enInitState);
extern UINT32   cpss_vos_sem_c_create (INT32 lOptions, INT32 lInitCount,
                                       INT32 lMaxCount);
extern UINT32   cpss_vos_sem_m_create (INT32 lOptions);
extern INT32    cpss_vos_sem_delete (UINT32 ulSemDesc);
extern INT32    cpss_vos_sem_p (UINT32 ulSemDesc, INT32 lTimeout);
extern INT32    cpss_vos_sem_v (UINT32 ulSemDesc);
extern INT32    cpss_vos_sem_info_get (UINT32 ulSemDesc, 
                                       CPSS_VOS_SEM_INFO_T * pstSemInfo);
extern INT32    cpss_vos_sem_show (UINT32 ulSemDesc, INT32 lLevel);

#ifdef __cplusplus
}
#endif
/******************************** 头文件保护结尾 *****************************/

#endif /* __INCcpss_vos_semh */
/******************************** 头文件结束 *********************************/


