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
#ifndef CPSS_MM_MEM_H
#define CPSS_MM_MEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cpss_public.h"

/******************************** 宏和常量定义 *******************************/
#define CPSS_MIN_MEM_BLK_GENE 6
#define CPSS_MIN_MEM_BLK_SIZE (1 << CPSS_MIN_MEM_BLK_GENE)	/* 对应内存池0的大小 */
#define CPSS_MON_MEM_BLK_POOL 14	/* 1 << 20 = 1M */
#define CPSS_MON_MEM_BLK_SIZE (CPSS_MIN_MEM_BLK_SIZE << CPSS_MON_MEM_BLK_POOL)
#define CPSS_MEM_ALIGN(x)    (((x) + 3) & 0xFFFFFFFC)   /* 将x进行4字节对齐 */
#define CPSS_MAX_MEM_TAB_SUM 128

#define CPSS_MAX_POOL_SUM 20
#define CPSS_MEM_BLK_CHECKSUM(blk) \
	( (UINT32)(((CPSS_MEM_BLOCK_PREFIX_T*)(blk))->pstMemHead) ^ \
	((CPSS_MEM_BLOCK_PREFIX_T*)(blk))->ulPoolID ^ \
	((CPSS_MEM_BLOCK_PREFIX_T*)(blk))->ulUserSize ^ \
	(UINT32)(((CPSS_MEM_BLOCK_PREFIX_T*)(blk))->pFileName) ^ \
	((CPSS_MEM_BLOCK_PREFIX_T*)(blk))->ulLine )

#define CPSS_MEM_BLK_ISVALID(blk) \
	( (NULL != ((CPSS_MEM_BLOCK_PREFIX_T*)(blk))) && \
	((((UINT32)(blk)) & 0x3) == 0) && \
	(((CPSS_MEM_BLOCK_PREFIX_T*)(blk))->ulPoolID < CPSS_MAX_MEM_TAB_SUM) && \
	(CPSS_MEM_BLK_CHECKSUM((blk)) == ((CPSS_MEM_BLOCK_PREFIX_T*)(blk))->ulCheckSum) )

#define CPSS_MEM_CHECKWORD 0x5A5AA5A5

/******************************** 类型定义 ***********************************/ 
typedef struct tagCPSS_MEM_BLOCK_PREFIX CPSS_MEM_BLOCK_PREFIX_T;
/*  内存块管理头结构 */
typedef struct tagCPSS_MEM_BLK_HEAD
{                          
	struct tagCPSS_MEM_BLK_HEAD *pstNext;     /* 指向下一个内存块管理头指针 */
	struct tagCPSS_MEM_BLK_HEAD *pstPrev;     /* 指向前一个内存块管理头指针 */
	CPSS_MEM_BLOCK_PREFIX_T *pstMemBlk;       /* 内存块的首地址，用户内存需要偏移一个CPSS_MEM_BLOCK_PREFIX_T */
}CPSS_MEM_BLK_HEAD_T;

/* 内存池管理表结构,一个内存池对应者一个这样的表 */
struct tagCPSS_MEM_BLOCK_PREFIX {
    CPSS_MEM_BLK_HEAD_T *pstMemHead;        /* 对应的管理头地址 */
    UINT32 ulPoolID;                        /* 内存池号 */
    UINT32 ulUserSize;                      /* 调用者申请的大小 */
	CHAR *pFileName;                      	/* 调用者的文件名*/
    UINT32 ulLine;                        	/* 调用者的行号 */
    UINT32 ulCheckSum;                      /* 前缀校验和 */
};

typedef struct tagCPSS_MEM_TABLE
{                              
    CPSS_MEM_BLK_HEAD_T *pstBusyHead;       /* 忙内存块管理链表头 */
    CPSS_MEM_BLK_HEAD_T *pstBusyTail;       /* 忙内存块管理链表尾 */
    CPSS_MEM_BLK_HEAD_T *pstIdleHead;       /* 闲内存块管理链表头 */
    CPSS_MEM_BLK_HEAD_T *pstIdleTail;       /* 闲内存块管理链表尾 */
    UINT32 ulMutex;                         /* 互斥锁 */
    UINT32 ulInitBlockNum;					/* 初始化时配置的内存块数量 */
    UINT32 ulMaxBlockNum;                   /* 最大内存块数量 */
    UINT32 ulUsedBlockNum;                  /* 已使用内存块数量 */
}CPSS_MEM_TABLE_T;

/******************************** 全局变量声明 *******************************/

extern CPSS_MEM_CONFIG_T stMemConfigTbl[];   /* 内存池信息*/

/******************************** 外部函数原形声明 ***************************/

extern INT32 cpss_mem_init();

#ifdef __cplusplus
}
#endif

#endif 

