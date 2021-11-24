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
#ifndef cpss_type_H
#define cpss_type_H

/********************************* 头文件包含 ********************************/
#ifdef  __cplusplus
extern "C" {
#endif

/* includes */
#include "swp_public.h"

/******************************** 宏和常量定义 *******************************/

#undef CPSS_VOS_VXWORKS
#undef CPSS_VOS_WINDOWS

#undef CPSS_VOS_LINUX

#if (SWP_OS_TYPE == SWP_OS_WINDOWS)
#define CPSS_VOS_WINDOWS
#endif

#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
#define CPSS_VOS_VXWORKS
#endif

#if (SWP_OS_TYPE == SWP_OS_NONE)
#define CPSS_VOS_NONE
#endif

#if (SWP_OS_TYPE == SWP_OS_LINUX)
#define CPSS_VOS_LINUX
#endif

#define CPSS_NULL_STRING ""

/******************************** 类型定义 ***********************************/


#define NUM_ENTS(array) (sizeof (array) / sizeof ((array) [0]))


/******************************** 全局变量声明 *******************************/
/*extern */
/******************************** 外部函数原形声明 ***************************/
/*extern */
/******************************** 头文件保护结尾 *****************************/
#ifdef  __cplusplus
}
#endif /* End "#ifdef  __cplusplus" */

#endif /* cpss_type_H */
/******************************** 头文件结束 *********************************/

