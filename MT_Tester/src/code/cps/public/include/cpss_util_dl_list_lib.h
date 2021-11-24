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
#ifndef __INCcpss_util_dl_list_libh
#define __INCcpss_util_dl_list_libh

#ifdef __cplusplus
extern "C" {
#endif

/********************************* 头文件包含 ********************************/
 
/* includes */

#include "cpss_type.h"

/******************************** 宏和常量定义 *******************************/

/* defines */

#define UTIL_DL_LIST_FIRST(pList) (((CPSS_UTIL_DL_LIST_T *) (pList))->pstHead)
#define UTIL_DL_LIST_LAST(pList)  (((CPSS_UTIL_DL_LIST_T *) (pList))->pstTail)
#define UTIL_DL_LIST_PREV(pNode)  (((CPSS_UTIL_DL_NODE_T *) (pNode))->pstPrev)
#define UTIL_DL_LIST_NEXT(pNode)  (((CPSS_UTIL_DL_NODE_T *) (pNode))->pstNext)

/******************************** 类型定义 ***********************************/

/* typedefs */

/* node */

typedef struct util_dl_node
    {
    struct util_dl_node *pstPrev; /* previous */
    struct util_dl_node *pstNext; /* next */
    } CPSS_UTIL_DL_NODE_T;

/* double-linked list */

typedef struct util_dl_list
    {
    struct util_dl_node *pstHead; /* head */
    struct util_dl_node *pstTail; /* tail */
    } CPSS_UTIL_DL_LIST_T;

/******************************** 外部函数原形声明 ***************************/

/* function declarations */

extern INT32    cpss_util_dl_list_init (CPSS_UTIL_DL_LIST_T *pstList);
extern void cpss_util_dl_list_insert (CPSS_UTIL_DL_LIST_T *pstList, CPSS_UTIL_DL_NODE_T *pstPrev,
                                      CPSS_UTIL_DL_NODE_T *pstNode);
/****
extern void cpss_util_dl_list_add (CPSS_UTIL_DL_LIST_T *pstList, CPSS_UTIL_DL_NODE_T *pstNode);
****/
#define cpss_util_dl_list_add(pstList, pstNode) \
    cpss_util_dl_list_insert ((pstList), ((pstList)->pstTail), (pstNode))

extern void cpss_util_dl_list_remove (CPSS_UTIL_DL_LIST_T *pstList, CPSS_UTIL_DL_NODE_T *pstNode);
extern CPSS_UTIL_DL_NODE_T *   cpss_util_dl_list_get (CPSS_UTIL_DL_LIST_T *pstList);
extern INT32    cpss_util_dl_list_count (CPSS_UTIL_DL_LIST_T *pstList);

#ifdef __cplusplus
}
#endif
/******************************** 头文件保护结尾 *****************************/

#endif /* __INCcpss_util_dl_list_libh */
/******************************** 头文件结束 *********************************/



