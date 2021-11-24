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
/*
* 功    能:双向链表库
* 说    明: * 链表工作原理及过程如下：

  -----------
  |  pHead-----------------
  |         |             |
  |  pTail---------       |
  |         |     v       v
  |         |   -----   -----
  -----------    ---     ---
  -        -
  
    ---------             --------          --------
    | pHead-------------->| ptNext---------->| ptNext--------
    |       |             |      |          |      |      |
    |       |       ------- pPrev|<---------- pPrev|      |
    |       |       |     |      |          |      |      |
    | pTail-----    |     | ...  |    ----->| ...  |      |
    |-------|  |    v     |------|    |     |------|      v
    |  -----               |                 -----
    |   ---                |                  ---
    |    -                 |                   -
    ------------------------
* 创建信息：
******************************************************************************/
/******************************************************************************
* 修改历史: 
* 时间：
* 修改人：
* 修改说明：
*
******************************************************************************/

/******************************* 包含文件声明 *********************************/
#include "cpss_type.h"
#include "cpss_util_dl_list_lib.h"

/******************************* 局部宏定义 ***********************************/

/******************************* 全局变量定义/初始化 **************************/

/******************************* 局部常数和类型定义 ***************************/

/******************************* 局部函数原型声明 *****************************/

/******************************* 函数实现 *************************************/

/*******************************************************************************
* 函数名称: cpss_util_dl_list_init
* 功    能: 双向链表初始化
* 函数类型: 
* 参    数: 
* 参数名称       类型                    输入/输出         描述
* pstList        CPSS_UTIL_DL_LIST_T *   输入              链表指针
* 函数返回: 返回1.
* 说    明: 
*******************************************************************************/
INT32 cpss_util_dl_list_init
(
 CPSS_UTIL_DL_LIST_T *pstList
 )
{
    pstList->pstHead = NULL;   /* 对链表头初始化 */
    pstList->pstTail = NULL;   /* 对链表尾初始化 */
    
    return (1);
}

/*******************************************************************************
* 函数名称: cpss_util_dl_list_insert
* 功    能: 在双向链表中插入一个节点
* 函数类型: 
* 参    数: 
* 参数名称     类型                   输入/输出         描述
* pstList      CPSS_UTIL_DL_LIST_T *  输入              链表指针
* pstPrev      CPSS_UTIL_DL_NODE_T *  输入              在该节点前插入
* pstNode      CPSS_UTIL_DL_NODE_T *  输入              被插入节点
* 函数返回: 无。
* 说    明: 
*******************************************************************************/
VOID cpss_util_dl_list_insert
(
 CPSS_UTIL_DL_LIST_T *pstList,
 CPSS_UTIL_DL_NODE_T *pstPrev,
 CPSS_UTIL_DL_NODE_T *pstNode
 )
{
    CPSS_UTIL_DL_NODE_T *pstNext;
    /* 判断pre是否为空 */
    if (pstPrev == NULL)
    {
        pstNext = pstList->pstHead;
        pstList->pstHead = pstNode;
    }
    else
    {        
        pstNext = pstPrev->pstNext;
        pstPrev->pstNext = pstNode;
    }
    /* 判断next是否为空 */
    if (pstNext == NULL)
    {
        pstList->pstTail = pstNode;
    }
    else
    {
        pstNext->pstPrev = pstNode;
    }
    /* 执行链表插入操作 */
    pstNode->pstNext = pstNext; 
    pstNode->pstPrev = pstPrev;
}

/*******************************************************************************
* 函数名称: cpss_util_dl_list_add
* 功    能: 该函数在双向链表的尾部加一个节点。
* 函数类型: 
* 参    数: 
* 参数名称      类型                  输入/输出         描述
* pstList       CPSS_UTIL_DL_LIST_T * 输入              链表指针
* pstNode       CPSS_UTIL_DL_NODE_T * 输入              节点指针
* 函数返回:   无
* 说    明: 
*******************************************************************************/
/****
VOID cpss_util_dl_list_add
(
 CPSS_UTIL_DL_LIST_T *pstList,
 CPSS_UTIL_DL_NODE_T *pstNode
 )
{
    cpss_util_dl_list_insert (pstList, pstList->pstTail, pstNode);
}
****/

/*******************************************************************************
* 函数名称: cpss_util_dl_list_remove
* 功    能: 该函数删除双向链表的一个节点。
* 函数类型: 
* 参    数: 
* 参数名称        类型                    输入/输出         描述
* pstList         CPSS_UTIL_DL_LIST_T *   输入              链表指针
* pstNode         CPSS_UTIL_DL_NODE_T *   输入              节点指针
* 函数返回: 无。
* 说    明: 
*******************************************************************************/
VOID cpss_util_dl_list_remove
(
 CPSS_UTIL_DL_LIST_T *pstList,
 CPSS_UTIL_DL_NODE_T *pstNode
 )
{
    /* 删除双向链表中的一个节点 */
    if (pstNode->pstPrev == NULL)
    {
        pstList->pstHead = pstNode->pstNext;
    }
    else
    {
        pstNode->pstPrev->pstNext = pstNode->pstNext;
    }
    /* 判断next是否为空 */
    if (pstNode->pstNext == NULL)
    {
        pstList->pstTail = pstNode->pstPrev;
    }
    else
    {
        pstNode->pstNext->pstPrev = pstNode->pstPrev;
    }
}

/*******************************************************************************
* 函数名称: cpss_util_dl_list_get
* 功    能: 该函数得到双向链表的一个节点。
* 函数类型: 
* 参    数: 
* 参数名称       类型                    输入/输出         描述
* pstList        CPSS_UTIL_DL_LIST_T *   输入              链表指针
* 函数返回: 该节点的指针。
* 说    明: 
*******************************************************************************/
CPSS_UTIL_DL_NODE_T * cpss_util_dl_list_get
(
 CPSS_UTIL_DL_LIST_T *pstList
 )
{
    CPSS_UTIL_DL_NODE_T *pstNode;
    
    pstNode = pstList->pstHead;
    
    /* Is empty? */
    if (pstNode != NULL)                      
    {
        /* 头指向下一个 */
        pstList->pstHead = pstNode->pstNext;
        
        /* 是否存在下一个? */
        if (pstNode->pstNext == NULL)
        {
            pstList->pstTail = NULL; 
        }
        else
        {
            pstNode->pstNext->pstPrev = NULL;
        }
    }
    
    return (pstNode);
}

/*******************************************************************************
* 函数名称: cpss_util_dl_list_count
* 功    能: 该函数返回给定表的节点数。
* 函数类型: 
* 参    数: 
* 参数名称        类型                    输入/输出         描述
* pstList         CPSS_UTIL_DL_LIST_T *   输入              链表指针
* 函数返回: 返回节点数.
* 说    明: 
*******************************************************************************/
INT32 cpss_util_dl_list_count
(
 CPSS_UTIL_DL_LIST_T *pstList
 )
{
    CPSS_UTIL_DL_NODE_T *pstNode;
    INT32   lCount;
    
    pstNode = UTIL_DL_LIST_FIRST (pstList);
    lCount = 0;
    /* 遍历列表 */
    while (pstNode != NULL)
    {
        lCount = lCount + 1;
        pstNode = UTIL_DL_LIST_NEXT (pstNode);
    }
    /* 返回计数 */
    return (lCount);
}

