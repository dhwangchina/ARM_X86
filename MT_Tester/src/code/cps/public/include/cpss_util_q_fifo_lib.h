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
#ifndef __INCcpss_util_q_fifo_libh
#define __INCcpss_util_q_fifo_libh

#ifdef __cplusplus
extern "C" {
#endif
/********************************* 头文件包含 ********************************/
 

/* includes */

#include "cpss_type.h"
#include "cpss_util_dl_list_lib.h"

/******************************** 宏和常量定义 *******************************/

/* defines */

/* key */

#define    UTIL_Q_FIFO_KEY_HEAD    -1        /* put at head of queue */
#define    UTIL_Q_FIFO_KEY_TAIL    0        /* put at tail of q (any != -1) */

/******************************** 类型定义 ***********************************/

/* typedefs */

typedef CPSS_UTIL_DL_LIST_T    CPSS_UTIL_Q_FIFO_HEAD_T; 
typedef CPSS_UTIL_DL_NODE_T    CPSS_UTIL_Q_FIFO_NODE_T;

/******************************** 外部函数原形声明 ***************************/

/* function declarations */

extern INT32    cpss_util_q_fifo_init (CPSS_UTIL_Q_FIFO_HEAD_T *pstList);
/****
extern void cpss_util_q_fifo_put (CPSS_UTIL_Q_FIFO_HEAD_T *pstHead, 
                                  CPSS_UTIL_Q_FIFO_NODE_T *pstNode, INT32 lKey);
****/                                  

#define cpss_util_q_fifo_put(pstHead, pstNode, lKey) \
    cpss_util_dl_list_insert((pstHead), \
        ((lKey) == UTIL_Q_FIFO_KEY_HEAD) ? NULL : ((pstHead)->pstTail), \
        (pstNode))
/****                                  
extern void cpss_util_q_fifo_remove (CPSS_UTIL_Q_FIFO_HEAD_T *pstHead, 
                                     CPSS_UTIL_Q_FIFO_NODE_T *pstNode);
****/                                     
#define cpss_util_q_fifo_remove(pHead, pNode) \
    cpss_util_dl_list_remove((pHead), (pNode))

/****                                     
extern CPSS_UTIL_Q_FIFO_NODE_T *   cpss_util_q_fifo_get (CPSS_UTIL_Q_FIFO_HEAD_T *pstHead);
****/
#define cpss_util_q_fifo_get(pstHead) \
    ((CPSS_UTIL_Q_FIFO_NODE_T *)cpss_util_dl_list_get((pstHead)))

#ifdef __cplusplus
}
#endif
/******************************** 头文件保护结尾 *****************************/

#endif /* __INCcpss_util_q_fifo_libh */
/******************************** 头文件结束 *********************************/

