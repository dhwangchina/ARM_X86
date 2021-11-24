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
#ifndef __INCcpss_util_sl_list_libh
#define __INCcpss_util_sl_list_libh

#ifdef __cplusplus
extern "C" {
#endif

/********************************* 头文件包含 ********************************/
   
/* includes */

#include "cpss_type.h"

/******************************** 宏和常量定义 *******************************/

/* defines */

#define UTIL_SL_LIST_FIRST(pList) (((CPSS_UTIL_SL_LIST_T *) (pstList))->pstHead)
#define UTIL_SL_LIST_LAST(pList)  (((CPSS_UTIL_SL_LIST_T *) (pstList))->pstTail)
#define UTIL_SL_LIST_NEXT(pNode)  (((CPSS_UTIL_SL_NODE_T *) (pstNode))->pstNext)

/******************************** 类型定义 ***********************************/

/* typedefs */

/* node */

typedef struct util_sl_node
{
    struct util_sl_node *pstNext; /* next */
}CPSS_UTIL_SL_NODE_T;

/* singly-linked list */

typedef struct util_sl_list
{
    struct util_sl_node *pstHead; /* head */
    struct util_sl_node *pstTail; /* tail */
} CPSS_UTIL_SL_LIST_T;

/******************************** 外部函数原形声明 ***************************/

/* function declarations */

extern STATUS   cpss_util_sl_list_init (CPSS_UTIL_SL_LIST_T *pstList);
extern void cpss_util_sl_list_put_head 
(
    CPSS_UTIL_SL_LIST_T *pstList, 
    CPSS_UTIL_SL_NODE_T *pstNode
);

extern void cpss_util_sl_list_put_tail
(
    CPSS_UTIL_SL_LIST_T *pstList,
    CPSS_UTIL_SL_NODE_T *pstNode
);

extern void cpss_util_sl_list_remove
(
    CPSS_UTIL_SL_LIST_T *pstList,
    CPSS_UTIL_SL_NODE_T *pstDelNode, 
    CPSS_UTIL_SL_NODE_T *pstPrevNode
);

extern CPSS_UTIL_SL_NODE_T* cpss_util_sl_list_get(CPSS_UTIL_SL_LIST_T *pstList);
extern INT32 cpss_util_sl_list_count (CPSS_UTIL_SL_LIST_T *pstList);

#ifdef __cplusplus
}
#endif
/******************************** 头文件保护结尾 *****************************/

#endif /* __INCcpss_util_sl_list_libh */
/******************************** 头文件结束 *********************************/


