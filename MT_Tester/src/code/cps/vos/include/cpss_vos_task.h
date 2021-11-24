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
#ifndef __INCcpss_vos_task_H
#define __INCcpss_vos_task_H

/********************************* 头文件包含 ********************************/
#include "cpss_type.h"
#include "cpss_vos.h"
#include "cpss_config.h"
/******************************** 宏和常量定义 *******************************/
#define VOS_TASK_NAME_LEN   32
#ifdef CPSS_VOS_VXWORKS
#define VOS_TASK_VX_UNBREAKABLE VX_UNBREAKABLE /* 禁止断点 */
#define VOS_TASK_VX_FP_TASK VX_FP_TASK /* 支持浮点协处理 */
#define VOS_TASK_VX_PRIVATE_ENV VX_PRIVATE_ENV /* 私有环境变量 */
#define VOS_TASK_VX_NO_STACK_FILL   VX_NO_STACK_FILL /*  避免栈溢出过 0xee */
#endif
#ifdef CPSS_VOS_WINDOWS
#define VOS_TASK_VX_UNBREAKABLE 0x0002 /*  禁止断点 */
#define VOS_TASK_VX_FP_TASK 0x0008 /* 支持浮点协处理*/
#define VOS_TASK_VX_PRIVATE_ENV 0x0080 /* 私有环境变量 */
#define VOS_TASK_VX_NO_STACK_FILL   0x0100 /*  避免栈溢出过 0xee */
#endif

#ifdef CPSS_VOS_LINUX
#define VOS_TASK_VX_UNBREAKABLE 0x0002 /*  禁止断点 */
#define VOS_TASK_VX_FP_TASK 0x0008 /* 支持浮点协处理*/
#define VOS_TASK_VX_PRIVATE_ENV 0x0080 /* 私有环境变量 */
#define VOS_TASK_VX_NO_STACK_FILL   0x0100 /*  避免栈溢出过 0xee */
#endif

/* priority */



/******************************** 类型定义 ***********************************/
/* TCB(任务控制块) */
#ifdef CPSS_VOS_WINDOWS
typedef struct tagCPSS_VOS_TASK_TCB
    {
    INT8    acName [VOS_TASK_NAME_LEN]; /* 名称 */
    INT32   lOptions; /* 选项 */
    INT32   lPri; /* 优先级 */
    VOID_FUNC_PTR   pfEntry; /* 任务入口地址 */
    INT32   lStackSize; /* 栈大小 */
    ULONG   ulThreadId; /* 线程ID */
    HANDLE  hObj; /* 对象的句柄 */
    } CPSS_VOS_TASK_TCB_T;
#endif

#ifdef CPSS_DSP_CPU
typedef struct tagCPSS_VOS_TASK_TCB
    {
    INT8    acName [VOS_TASK_NAME_LEN]; /* 名称 */
    INT32   lOptions; /* 选项 */
    INT32   lPri; /* 优先级 */
    VOID_FUNC_PTR   pfEntry; /* 任务入口地址 */
    INT32   lStackSize; /* 栈大小 */
    } CPSS_VOS_TASK_TCB_T;
#endif

#ifdef CPSS_VOS_LINUX
typedef struct tagCPSS_VOS_TASK_TCB
    {
    INT8    acName [VOS_TASK_NAME_LEN]; /* 名称 */
    INT32   lOptions; /* 选项 */
    INT32   lPri; /* 优先级 */
    VOID_FUNC_PTR   pfEntry; /* 任务入口地址 */
    INT32   lStackSize; /* 栈大小 */
    pthread_t   ptThreadId; /* 线程ID */
    } CPSS_VOS_TASK_TCB_T;
#endif

/* 任务ID */
#ifdef CPSS_VOS_VXWORKS
typedef int VOS_TASK_ID;
#endif

#ifdef CPSS_VOS_WINDOWS
typedef struct tagCPSS_VOS_TASK_TCB * VOS_TASK_ID;
#endif

#ifdef CPSS_DSP_CPU
typedef struct tagCPSS_VOS_TASK_TCB * VOS_TASK_ID;
#endif

#ifdef CPSS_VOS_LINUX
typedef struct tagCPSS_VOS_TASK_TCB * VOS_TASK_ID;
#endif


/* 任务描述符 */
typedef struct tagCPSS_VOS_TASK_DESC
    {
    VOS_TASK_ID taskId; /* 任务ID */
    BOOL    bInuse; /* 已/未使用 */
    INT8 acName[VOS_TASK_NAME_LEN];
    UINT32 ulSbbrSize;
    INT32 lKwTmOutLimitType;	
    UINT32 ulKwId;
    INT32 lMicroTime;
    INT32 lMicroTimeStart;
    INT32 lMicroTimeEnd;
    UINT32 ulRunTicks;
    UINT32 ulSchedDesc;
    } CPSS_VOS_TASK_DESC_T;

/* task information */
typedef struct tagCPSS_VOS_TASK_INFO
    {
    UINT32  ulTaskDesc; /* 任务描述符 */
    VOS_TASK_ID taskId; /* 任务ID */
    INT8 *  pcName; /* 任务名 */
    INT32   lPri; /* 优先级 */
    INT32   lStatus; /* 任务状态 */
    } CPSS_VOS_TASK_INFO_T;

typedef struct
{
    CPSS_TASK_ENTRY_FUNCPTR pfTaskEntry;
    UINT32  ulSemDesc;
    UINT32 ulIntVal; /* 释放信号量的定时间隔 */
    UINT32 ulTaskIdx;
    INT32 lCurIntVal;
    UINT32 ulUseFlag;
    UINT32 ulCallCount;
}CPSS_TASK_REG_MNG_T;

/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/
extern INT32    cpss_vos_task_lib_init (void);
extern UINT32   cpss_vos_task_spawn (INT8 *pcName, INT32 lPri, INT32 lOptions,
                                     INT32 lStackSize, VOID_FUNC_PTR pfEntry,
                                     INT32 lArg);
extern INT32    cpss_vos_task_delete (UINT32 ulTaskDesc);
extern INT32    cpss_vos_task_suspend (UINT32 ulTaskDesc);
extern INT32    cpss_vos_task_resume (UINT32 ulTaskDesc);
extern INT32    cpss_vos_task_pri_set (UINT32 ulTaskDesc, INT32 lNewPri);
extern INT32    cpss_vos_task_pri_get (UINT32 ulTaskDesc, INT32 *plPri);
extern UINT32   cpss_vos_task_desc_self (void);
extern UINT32   cpss_vos_task_desc_get_by_name (INT8 *pcName);    
extern INT8 * cpss_vos_task_name_get_by_name (UINT32 ulTaskDesc );
extern INT32    cpss_vos_task_info_get (UINT32 ulTaskDesc, 
                                        CPSS_VOS_TASK_INFO_T *pstTaskInfo);
extern INT32   cpss_vos_task_show (UINT32 ulTaskDesc, INT32 lLevel);

/******************************** 头文件保护结尾 *****************************/

#endif /* __INCcpss_vos_task_H */

/******************************** 头文件结束 *********************************/

