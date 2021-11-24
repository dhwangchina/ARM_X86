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
#ifndef __INCcpss_util_q_pri_libh
#define __INCcpss_util_q_pri_libh

#ifdef __cplusplus
extern "C" {
#endif
/********************************* 头文件包含 ********************************/
  
/* includes */

#include "cpss_type.h"
#include "cpss_util_dl_list_lib.h"

/******************************** 宏和常量定义 *******************************/

/* defines */

/******************************** 类型定义 ***********************************/

/* typedefs */

typedef CPSS_UTIL_DL_LIST_T    UTIL_Q_PRI_HEAD; 

typedef struct util_q_pri_node
    {
    CPSS_UTIL_DL_NODE_T    node; /* priority doubly linked node */
    INT32    key; /* priority */
    } UTIL_Q_PRI_NODE;

/******************************** 外部函数原形声明 ***************************/

/* function declarations */

extern INT32    cpss_util_q_pri_init (UTIL_Q_PRI_HEAD *pstList);
extern void cpss_util_q_pri_put_from_head (UTIL_Q_PRI_HEAD *pstHead, 
                                           UTIL_Q_PRI_NODE *pstNode, INT32 lkey);
extern void cpss_util_q_pri_put_from_tail (UTIL_Q_PRI_HEAD *pstHead, 
                                           UTIL_Q_PRI_NODE *pstNode, INT32 lkey);
extern void cpss_util_q_pri_remove (UTIL_Q_PRI_HEAD *pstHead, 
                                    UTIL_Q_PRI_NODE *pstNode);
extern UTIL_Q_PRI_NODE *   cpss_util_q_pri_get (UTIL_Q_PRI_HEAD *pstHead);

#ifdef __cplusplus
}
#endif
/******************************** 头文件保护结尾 *****************************/

#endif /* __INCcpss_util_q_pri_libh */
/******************************** 头文件结束 *********************************/


