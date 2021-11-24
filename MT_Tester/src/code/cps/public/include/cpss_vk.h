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
#ifndef CPSS_VK_H
#define CPSS_VK_H

/********************************* 头文件包含 ********************************/
#include "cpss_type.h"
#include "cpss_com.h"
/******************************** 宏和常量定义 *******************************/
/* scheduler descriptor */

#define VK_SCHED_DESC_INVALID   0xFFFFFFFF /* invalid */

#define CPSS_VK_PNO_INVALID    ((UINT16)(0xFFFF))     /* 无效纤程号 */
#define CPSS_VK_PNO_DEFAULT    ((UINT16)(0x0))        /* 默认纤程号 */
#define CPSS_VK_PTYPE_INVALID  ((UINT16)(0xFFFF))     /* 无效纤程类型 */
#define CPSS_VK_PD_INVALID     ((UINT32)(0xFFFFFFFF)) /* 无效纤程描述符 */

#define CPSS_VK_PTYPE(ulPD)  (UINT16)((ulPD)>>16) /* 从纤程描述符得到纤程类型 */
#define CPSS_VK_PNO(ulPD)    (UINT16)((ulPD)&0xFFFF)/* 从纤程描述符得到纤程号 */

/* 从纤程类型和纤程号得到纤程描述符 */
#define CPSS_VK_PD(usPType, usPNo) \
        (UINT32)((((usPType)&0xFFFF)<<16)|((usPNo)&0xFFFF)) 

/* 纤程业务状态初始值 */
#define CPSS_VK_PROC_USER_STATE_IDLE    ((UINT16)(0x00))

/******************************** 类型定义 ***********************************/


/******************************** 全局变量声明 *******************************/
/*extern */
/******************************** 外部函数原形声明 ***************************/

extern void cpss_vk_sched (INT32 slArg);
extern UINT32   cpss_vk_pd_self_get (void);
extern void *   cpss_vk_proc_user_data_get (void);
extern INT32    cpss_vk_proc_user_state_set (UINT16 ulNewState);
extern INT32    cpss_vk_proc_user_state_get (UINT16 *pusState);
extern INT32    cpss_vk_msg_sender_get (CPSS_COM_PID_T *ptSender);
extern INT32    cpss_vk_msg_id_get (UINT32 *pulId);
extern UINT32   cpss_vk_sched_desc_self (void);
extern UINT32   cpss_vk_sched_desc_get_by_name (INT8 *pcName);
extern STRING   cpss_vk_proc_name_get_by_pid(UINT32 ulPID);
#ifdef CPSS_VOS_VXWORKS
extern MODULE_ID    cpss_load_module (INT8 *pucPatchFileName, UINT32 ulOffset, 
                                      UINT32 ulSize);
extern INT32    cpss_unload_module (MODULE_ID moduleId);
#else
extern   UINT32 cpss_load_module (INT8 *pucPatchFileName, UINT32 ulOffset, 
                                      UINT32 ulSize);
extern INT32    cpss_unload_module (UINT32 moduleId);
#endif

extern void *   cpss_lookup_func (INT8 *pucFuncName);
extern void cpss_apply_patch (void *pucOldFunc, void *pucNewFunc, UINT8 *aucSaveBuf);
extern void cpss_undo_patch (void *pucOldFunc, UINT8 *aucSaveBuf);
extern INT32    cpss_need_patch_operation (INT8 *pucTaskName);
/*extern */
/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_VK_H */
/******************************** 头文件结束 *********************************/

