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
#ifndef CPSS_MM_H
#define CPSS_MM_H

/********************************* 头文件包含 ********************************/
#include "cpss_type.h"
#include "cpss_dbg.h"

/******************************** 宏和常量定义 *******************************/
/********************* 内存块规格定义开始***********************/
/** 不支持MMU情况下**/
#define CPSS_MM_SIZE_K 1024
#define CPSS_MM_SIZE_M (1 << 20)	/* (CPSS_MM_SIZE_K * CPSS_MM_SIZE_K) */
/** 支持MMU情况下**/
#define PAGE_SIZE    4096
/******************************** 类型定义 ***********************************/

/******************************** 全局变量声明 *******************************/
/******************************** 外部函数原形声明 ***************************/

extern VOID* cpss_mem_malloc_debug(UINT32 ulSize,UINT8 *pucFileName, UINT32 ulLines);
extern VOID  cpss_mem_free_debug(VOID *pvBuf,UINT8 *pucFileName, UINT32 ulLines);

#ifdef CPSS_MM_DISABLE

#define cpss_mem_malloc(ulSize) malloc(ulSize)
#define cpss_mem_free(pvBuf) free(pvBuf)

#else

#define cpss_mem_malloc(ulSize) cpss_mem_malloc_debug(ulSize,(UINT8*)__FILE__,__LINE__)
#define cpss_mem_free(pvBuf) cpss_mem_free_debug(pvBuf, (UINT8*)__FILE__,__LINE__)

#endif

#define cpss_mem_memcpy(pvBuf1,pvBuf2,ulLen) memcpy(pvBuf1,pvBuf2,ulLen)
#define cpss_mem_memset(pvBuf1,cByte,ulLen) memset(pvBuf1,cByte,ulLen)
#define cpss_mem_memmove(pvBuf1,pvBuf2,ulSize) memmove(pvBuf1,pvBuf2,ulSize)
#define cpss_mem_memcmp(pvBuf1,pvBuf2,ulSize) memcmp(pvBuf1,pvBuf2,ulSize)

/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_MM_H */
/******************************** 头文件结束 *********************************/

