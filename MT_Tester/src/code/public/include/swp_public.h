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
#ifndef SWP_PUBLIC_H
#define SWP_PUBLIC_H

/********************************* 头文件包含 ********************************/
#include "swp_type.h"
#include "swp_common.h"
#include "swp_common_str.h"
#include "swp_mo.h"
#include "swp_config.h"
#if (SWP_NE_TYPE == SWP_NETYPE_RNC)
#include "swp_usr_common_rnc.h"
#include "swp_usr_common_ac.h"
#include "swp_mo_rnc.h"
#elif ((SWP_NE_TYPE == SWP_NETYPE_SGSN) \
     ||(SWP_NE_TYPE == SWP_NETYPE_MSCS) \
     ||(SWP_NE_TYPE == SWP_NETYPE_GGSN) \
     ||(SWP_NE_TYPE == SWP_NETYPE_HLR))
#include "swp_usr_common_ps.h"
#include "swp_mo_ps.h"
/****zengjian add for TAN app 20070228***/
#elif (SWP_NE_TYPE == SWP_NETYPE_TAN)
#include "swp_usr_common_rnc.h"
#include "swp_mo_rnc.h"
#include "swp_usr_common_ps.h"
/****chenyy  add for TAN app 20070321 Begin ***/
#include "swp_usr_common_tan.h"
/****chenyy  add for TAN app 20070321 End   ***/
#include "swp_mo_ps.h"
/***zengjian  add end*******/
#endif

/******************************** 宏和常量定义 *******************************/

/******************************** 类型定义 ***********************************/

/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/
extern INT32 swp_get_ha_time(INT32 lType);
extern INT32 swp_get_act_proc_time(INT32 lType);

/******************************** 头文件保护结尾 *****************************/
#endif /* SWP_PUBLIC_H */
/******************************** 头文件结束 *********************************/
