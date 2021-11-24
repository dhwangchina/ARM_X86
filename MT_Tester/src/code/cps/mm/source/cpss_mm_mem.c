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
#include "cpss_public.h"
#include "cpss_mm_mem.h"
#include "cpss_dbg_diagn.h"
/******************************* 局部宏定义 **********************************/
CPSS_MEM_TABLE_T g_stCpssMemTable[CPSS_MAX_MEM_TAB_SUM];

#define CPSS_MEM_FAIL_STAT_TYPE_MALLOC "malloc"
#define CPSS_MEM_FAIL_STAT_TYPE_WRITE "write"
#define CPSS_MEM_FAIL_STAT_TYPE_FREE "free"

#define CPSS_MEM_FAIL_STAT_ERR_MAX_SUM 1024
#define CPSS_MEM_FAIL_STAT_ERR_FREENULL 	"memory is null"
#define CPSS_MEM_FAIL_STAT_ERR_FREEERROR 	"refree or not exist"
#define CPSS_MEM_FAIL_STAT_ERR_TOOLARGE 	"size is to large"
#define CPSS_MEM_FAIL_STAT_ERR_NOMEM 		"no system memory"
#define CPSS_MEM_FAIL_STAT_ERR_OVERFLOW 	"write overflow"

struct {
	UINT8 *pucFile;
	UINT32 ulLine;
	UINT8 *pucType;
	UINT8 *pucError;
} *g_pstCpssMemFailStat;

UINT32 g_ulCpssMemFailStatSum;
extern UINT32 g_ulMemTotalSize;
extern INT32 cps_devm_get_mem_size();

void cpss_mem_fail_stat(UINT8 *vpucFile, UINT32 vulLine, UINT8 *vpucType, UINT8 *vpucError)
{
	if(g_ulCpssMemFailStatSum >= CPSS_MEM_FAIL_STAT_ERR_MAX_SUM)
	{
		/* 记录越界，先解决这些问题再说 */
		return;
	}
	g_pstCpssMemFailStat[g_ulCpssMemFailStatSum].pucFile = vpucFile;
	g_pstCpssMemFailStat[g_ulCpssMemFailStatSum].ulLine = vulLine;
	g_pstCpssMemFailStat[g_ulCpssMemFailStatSum].pucType = vpucType;
	g_pstCpssMemFailStat[g_ulCpssMemFailStatSum].pucError = vpucError;
	g_ulCpssMemFailStatSum++;
}

/* 根据内存池号得到内存大小 */
UINT32 cpss_mem_pool2size(UINT32 vulPoolId)
{
	if(vulPoolId < CPSS_MON_MEM_BLK_POOL)
	{
		return (CPSS_MIN_MEM_BLK_SIZE << vulPoolId);
	}
	return (CPSS_MON_MEM_BLK_SIZE * (vulPoolId - CPSS_MON_MEM_BLK_POOL + 1));
}

/* 根据内存大小定位内存池号 */
UINT32 cpss_mem_size2pool(UINT32 vulSize)
{
	UINT32 ulSize;
	UINT32 ulPoolId;

	if(vulSize > CPSS_MON_MEM_BLK_SIZE)
	{
		ulPoolId = ((vulSize >> (CPSS_MON_MEM_BLK_POOL + CPSS_MIN_MEM_BLK_GENE)) + CPSS_MON_MEM_BLK_POOL);
		if((CPSS_MON_MEM_BLK_SIZE * (ulPoolId - CPSS_MON_MEM_BLK_POOL)) == vulSize)
		{
			ulPoolId--;
		}
		return ulPoolId;
	}
	ulPoolId = 0;
	ulSize = vulSize >> CPSS_MIN_MEM_BLK_GENE;
	while(0 != ulSize)
	{
		ulSize = ulSize >> 1;
		ulPoolId++;
	}
	if(vulSize == (CPSS_MIN_MEM_BLK_SIZE << (ulPoolId - 1)))
	{
		ulPoolId--;
	}
	return ulPoolId;
}

/* 将一个内存块加入到忙队列头，用户申请完内存需要进行此操作 */
INT32 cpss_mem_blk2busy(CPSS_MEM_TABLE_T *vpstTab, CPSS_MEM_BLK_HEAD_T *vpstBlk)
{
	cpss_vos_mutex_p(vpstTab->ulMutex, CPSS_VOS_MUTEX_WAIT_FOREVER);
	vpstBlk->pstNext = vpstTab->pstBusyHead;
	vpstBlk->pstPrev = NULL;
	if(NULL == vpstTab->pstBusyHead)
	{
		vpstTab->pstBusyTail = vpstBlk;
	}
	else
	{
		vpstTab->pstBusyHead->pstPrev = vpstBlk;
	}
	vpstTab->pstBusyHead = vpstBlk;
	vpstTab->ulUsedBlockNum++;
	cpss_vos_mutex_v(vpstTab->ulMutex);
    return CPSS_OK;
}

/* 将一个内存块加入到闲队列头，用户释放完内存需要进行此操作 */
INT32 cpss_mem_blk2idle(CPSS_MEM_TABLE_T *vpstTab, CPSS_MEM_BLK_HEAD_T *vpstBlk)
{
	cpss_vos_mutex_p(vpstTab->ulMutex, CPSS_VOS_MUTEX_WAIT_FOREVER);
	vpstBlk->pstNext = vpstTab->pstIdleHead;
	vpstBlk->pstPrev = NULL;
	if(NULL == vpstTab->pstIdleHead)
	{
		vpstTab->pstIdleTail = vpstBlk;
	}
	else
	{
		vpstTab->pstIdleHead->pstPrev = vpstBlk;
	}
	vpstTab->pstIdleHead = vpstBlk;
	cpss_vos_mutex_v(vpstTab->ulMutex);
    return CPSS_OK;
}

/* 将一个内存块从忙队列取出，用户释放内存时需要进行此操作 */
INT32 cpss_mem_blkfrombusy(CPSS_MEM_TABLE_T *vpstTab, CPSS_MEM_BLK_HEAD_T *vpstBlk)
{
	cpss_vos_mutex_p(vpstTab->ulMutex, CPSS_VOS_MUTEX_WAIT_FOREVER);
	if(NULL == vpstBlk->pstNext)
	{
		vpstTab->pstBusyTail = vpstBlk->pstPrev;
	}
	else
	{
		vpstBlk->pstNext->pstPrev = vpstBlk->pstPrev;
	}
	if(NULL == vpstBlk->pstPrev)
	{
		vpstTab->pstBusyHead = vpstBlk->pstNext;
	}
	else
	{
		vpstBlk->pstPrev->pstNext = vpstBlk->pstNext;
	}
	vpstTab->ulUsedBlockNum--;
	cpss_vos_mutex_v(vpstTab->ulMutex);
    return CPSS_OK;
}

/* 从闲队列尾拿出一个内存块，用户申请内存时需要进行此操作 */
CPSS_MEM_BLK_HEAD_T* cpss_mem_blkfromidle(CPSS_MEM_TABLE_T *vpstTab)
{
	CPSS_MEM_BLK_HEAD_T *pstBlk;
	cpss_vos_mutex_p(vpstTab->ulMutex, CPSS_VOS_MUTEX_WAIT_FOREVER);
	pstBlk = vpstTab->pstIdleTail;
	if(NULL != pstBlk)
	{
		vpstTab->pstIdleTail = pstBlk->pstPrev;
		if(NULL == pstBlk->pstPrev)
		{
			vpstTab->pstIdleHead = NULL;
		}
		else
		{
			pstBlk->pstPrev->pstNext = NULL;
		}
	}
	cpss_vos_mutex_v(vpstTab->ulMutex);
	return pstBlk;
}

/* 内存管理模块初始化 */
INT32 cpss_mem_init()
{
	UINT32 ulBlk;
	UINT32 ulLoop;
	UINT32 ulPoolId;
	UINT32 ulPoolSize;
	UINT8 *pucPoolAddr;
	UINT8 *pucManageAddr;

	/* 初始化失败统计 */
	g_ulCpssMemFailStatSum = 0;
	g_pstCpssMemFailStat = malloc(sizeof(*g_pstCpssMemFailStat) * CPSS_MEM_FAIL_STAT_ERR_MAX_SUM);
	if(NULL == g_pstCpssMemFailStat)
	{
        return CPSS_ERROR;
	}
	memset(g_stCpssMemTable, 0, sizeof(g_stCpssMemTable));
	for(ulLoop = 0; ulLoop < CPSS_MAX_MEM_TAB_SUM; ulLoop++)
	{
		/* 每一个内存池创建一个互斥信号量 */
		g_stCpssMemTable[ulLoop].ulMutex = cpss_vos_mutex_create();
		if(CPSS_VOS_MUTEXD_INVALID == g_stCpssMemTable[ulLoop].ulMutex)
		{
			/* 初始化时创建互斥信号量失败为致命错误，直接退出，此处不考虑已经申请的资源 */
	        cpss_output(CPSS_MODULE_MM,CPSS_PRINT_FATAL ,"!!!create mutex fail!!!!!\n\r");
	        return CPSS_ERROR;
		}
	}
	for(ulLoop = 0; ; ulLoop++)
	{
		if((stMemConfigTbl[ulLoop].ulBlkSize == 0) &&
				(stMemConfigTbl[ulLoop].ulBlkNum == 0))
		{
			break;
		}
		ulPoolId = cpss_mem_size2pool(stMemConfigTbl[ulLoop].ulBlkSize);
		ulPoolSize = cpss_mem_pool2size(ulPoolId);
		pucPoolAddr = malloc(stMemConfigTbl[ulLoop].ulBlkNum * ulPoolSize);
		if(NULL == pucPoolAddr)
		{
			/* 初始化时申请内存失败为致命错误，直接退出，此处不考虑已经申请的资源 */
	        cpss_output(CPSS_MODULE_MM,CPSS_PRINT_FATAL ,"init malloc fail!!!!!\n\r");
	        return CPSS_ERROR;
		}
		pucManageAddr = malloc(stMemConfigTbl[ulLoop].ulBlkNum * sizeof(CPSS_MEM_BLK_HEAD_T));
		if(NULL == pucManageAddr)
		{
			/* 初始化时申请内存失败为致命错误，直接退出，此处不考虑已经申请的资源 */
	        cpss_output(CPSS_MODULE_MM,CPSS_PRINT_FATAL ,"init malloc fail!!!!!\n\r");
	        return CPSS_ERROR;
		}
		for(ulBlk = 0; ulBlk < stMemConfigTbl[ulLoop].ulBlkNum; ulBlk++)
		{
			((CPSS_MEM_BLK_HEAD_T*)(pucManageAddr + (sizeof(CPSS_MEM_BLK_HEAD_T) * ulBlk)))->pstMemBlk
					= (CPSS_MEM_BLOCK_PREFIX_T*)(pucPoolAddr + (ulPoolSize * ulBlk));
			((CPSS_MEM_BLOCK_PREFIX_T*)(pucPoolAddr + (ulPoolSize * ulBlk)))->pstMemHead
					= (CPSS_MEM_BLK_HEAD_T*)(pucManageAddr + (sizeof(CPSS_MEM_BLK_HEAD_T) * ulBlk));
			((CPSS_MEM_BLOCK_PREFIX_T*)(pucPoolAddr + (ulPoolSize * ulBlk)))->ulPoolID = ulPoolId;
			cpss_mem_blk2idle(&g_stCpssMemTable[ulPoolId], (CPSS_MEM_BLK_HEAD_T*)(pucManageAddr + (sizeof(CPSS_MEM_BLK_HEAD_T) * ulBlk)));
		}
		g_stCpssMemTable[ulPoolId].ulMaxBlockNum += stMemConfigTbl[ulLoop].ulBlkNum;
		g_stCpssMemTable[ulPoolId].ulInitBlockNum += stMemConfigTbl[ulLoop].ulBlkNum;
	}
    return CPSS_OK;
}

VOID* cpss_mem_malloc_debug(UINT32 ulSize,UINT8 *pucFileName, UINT32 ulLines)
{
	UINT32 ulPooId;
	CPSS_MEM_BLK_HEAD_T *pstBlk;

	/* 以“字节对齐的大小+内存头+内存校验字节 ”的大小选择内存池 */
	ulPooId = cpss_mem_size2pool(CPSS_MEM_ALIGN(ulSize) + sizeof(CPSS_MEM_BLOCK_PREFIX_T) + sizeof(UINT32));
	if(ulPooId >= CPSS_MAX_MEM_TAB_SUM)
	{
		/* 申请的内存太大，超过了管理的最大内存大小 */
		cpss_mem_fail_stat(pucFileName, ulLines, CPSS_MEM_FAIL_STAT_TYPE_MALLOC, CPSS_MEM_FAIL_STAT_ERR_TOOLARGE);
		return NULL;
	}
	pstBlk = cpss_mem_blkfromidle(&g_stCpssMemTable[ulPooId]);
	if(NULL == pstBlk)
	{
		/* 空闲链表上已经没有内存，需要申请一块内存挂到链表上 */
		pstBlk = malloc(sizeof(CPSS_MEM_BLK_HEAD_T));
		if(NULL == pstBlk)
		{
			/* 内存申请失败，致命错误 */
			cpss_mem_fail_stat(pucFileName, ulLines, CPSS_MEM_FAIL_STAT_TYPE_MALLOC, CPSS_MEM_FAIL_STAT_ERR_NOMEM);
			return NULL;
		}
		pstBlk->pstMemBlk = malloc(cpss_mem_pool2size(ulPooId));
		if(NULL == pstBlk->pstMemBlk)
		{
			/* 内存申请失败，致命错误 */
			cpss_mem_fail_stat(pucFileName, ulLines, CPSS_MEM_FAIL_STAT_TYPE_MALLOC, CPSS_MEM_FAIL_STAT_ERR_NOMEM);
			free(pstBlk);
			return NULL;
		}
		pstBlk->pstMemBlk->pstMemHead = pstBlk;
		pstBlk->pstMemBlk->ulPoolID = ulPooId;
		cpss_vos_mutex_p(g_stCpssMemTable[ulPooId].ulMutex, CPSS_VOS_MUTEX_WAIT_FOREVER);
		g_stCpssMemTable[ulPooId].ulMaxBlockNum++;
		cpss_vos_mutex_v(g_stCpssMemTable[ulPooId].ulMutex);
	}
	/* 记录用户信息 */
	pstBlk->pstMemBlk->ulUserSize = ulSize;
	pstBlk->pstMemBlk->pFileName = pucFileName;
	pstBlk->pstMemBlk->ulLine = ulLines;
	pstBlk->pstMemBlk->ulCheckSum = CPSS_MEM_BLK_CHECKSUM(pstBlk->pstMemBlk);
	/* 增加校验字段，调试时可以查看应用是否越界写内存（不太可靠），该代码在ARM的CPU上可能有问题 */
	*(UINT32*)((UINT8*)(pstBlk->pstMemBlk + 1) + ulSize) = CPSS_MEM_CHECKWORD;
	cpss_mem_blk2busy(&g_stCpssMemTable[ulPooId], pstBlk);
	return (void*)(pstBlk->pstMemBlk + 1);
}

VOID cpss_mem_free_debug(VOID *pvBuf,UINT8 *pucFileName, UINT32 ulLines)
{
	CPSS_MEM_BLOCK_PREFIX_T *pstPrefix;

	if(NULL == pvBuf)
	{
		cpss_mem_fail_stat(pucFileName, ulLines, CPSS_MEM_FAIL_STAT_TYPE_FREE, CPSS_MEM_FAIL_STAT_ERR_FREENULL);
		return;
	}
	pstPrefix = (CPSS_MEM_BLOCK_PREFIX_T*)((UINT8*)pvBuf - sizeof(CPSS_MEM_BLOCK_PREFIX_T));
	if(!CPSS_MEM_BLK_ISVALID(pstPrefix))
	{
		/* 头校验失败：重复释放、释放非法内存 */
		cpss_mem_fail_stat(pucFileName, ulLines, CPSS_MEM_FAIL_STAT_TYPE_FREE, CPSS_MEM_FAIL_STAT_ERR_FREEERROR);
        cpss_output(CPSS_MODULE_MM,CPSS_PRINT_FATAL ,"Memory freed is invalid, addr is %#x\n", (UINT32)pvBuf);
		return;
	}
	if(*(UINT32*)((UINT8*)pvBuf + pstPrefix->ulUserSize) != CPSS_MEM_CHECKWORD)
	{
		/* 该块内存有越界情况，需要记录下来 */
		cpss_mem_fail_stat(pstPrefix->pFileName, pstPrefix->ulLine, CPSS_MEM_FAIL_STAT_TYPE_WRITE, CPSS_MEM_FAIL_STAT_ERR_OVERFLOW);
        cpss_output(CPSS_MODULE_MM,CPSS_PRINT_FATAL ,"Memory has detected write overflow, addr is %#x\n", (UINT32)pvBuf);
	}
	pstPrefix->ulCheckSum = ~pstPrefix->ulCheckSum;
	/* 从忙队列取出，重新放到闲队列中 */
	cpss_mem_blkfrombusy(&g_stCpssMemTable[pstPrefix->ulPoolID], pstPrefix->pstMemHead);
	cpss_mem_blk2idle(&g_stCpssMemTable[pstPrefix->ulPoolID], pstPrefix->pstMemHead);
}

void cpss_mem_show_fail()
{
	UINT32 ulLoop;

	if(0 == g_ulCpssMemFailStatSum)
	{
		cps__oams_shcmd_printf("no fail stat items\n");
	}
	else
	{
		cps__oams_shcmd_printf("%-10s%-20s%-10s%-10s\n", "S_TYPE", "S_ERROR", "N_LINE", "S_FILE");
	}

	for(ulLoop = 0; ulLoop < g_ulCpssMemFailStatSum; ulLoop++)
	{
		cps__oams_shcmd_printf("%-10s%-20s%-10d%-10s\n",
				g_pstCpssMemFailStat[ulLoop].pucType,
				g_pstCpssMemFailStat[ulLoop].pucError,
				g_pstCpssMemFailStat[ulLoop].ulLine,
				g_pstCpssMemFailStat[ulLoop].pucFile);
	}

}
void cpss_mem_show_summarize()
{
	UINT32 ulLoop;

	cps__oams_shcmd_printf("%-10s%-10s%-10s%-10s%-10s\n", "POOL", "SIZE", "N_MAX", "N_INIT", "N_USED");
	for(ulLoop = 0; ulLoop < CPSS_MAX_MEM_TAB_SUM; ulLoop++)
	{
		cps__oams_shcmd_printf("%-10d%-10d%-10d%-10d%-10d\n", ulLoop, cpss_mem_pool2size(ulLoop),
				g_stCpssMemTable[ulLoop].ulMaxBlockNum, g_stCpssMemTable[ulLoop].ulInitBlockNum,
				g_stCpssMemTable[ulLoop].ulUsedBlockNum);
	}
}

void cpss_mem_show_detail(UINT32 vulPoolId)
{
	UINT32 ulLoop;
	UINT32 ulStatSum = 0;
	CPSS_MEM_BLK_HEAD_T *pstBlk;
	static struct {
		UINT32 ulSum;
		UINT32 ulLine;
		char* pcFile;
	}* gs_pstCpssMemStat = NULL;
	static UINT32 gs_ulMaxStatSum = 4096;

	cps__oams_shcmd_printf("pool id:           %-10d\n", vulPoolId);
	cps__oams_shcmd_printf("pool size:         %-10d\n", cpss_mem_pool2size(vulPoolId));
	cps__oams_shcmd_printf("max block sum:     %-10d\n", g_stCpssMemTable[vulPoolId].ulMaxBlockNum);
	cps__oams_shcmd_printf("init block sum:    %-10d\n", g_stCpssMemTable[vulPoolId].ulInitBlockNum);
	cps__oams_shcmd_printf("used block sum:    %-10d\n", g_stCpssMemTable[vulPoolId].ulUsedBlockNum);
	if(NULL == gs_pstCpssMemStat)
	{
		gs_pstCpssMemStat = malloc(sizeof(*gs_pstCpssMemStat) * gs_ulMaxStatSum);
		if(NULL == gs_pstCpssMemStat)
		{
			return;
		}
	}
	memset(gs_pstCpssMemStat, 0, sizeof(*gs_pstCpssMemStat) * gs_ulMaxStatSum);

	pstBlk = g_stCpssMemTable[vulPoolId].pstBusyHead;
	while(NULL != pstBlk)
	{
		for(ulLoop = 0; ulLoop < ulStatSum; ulLoop++)
		{
			if((gs_pstCpssMemStat[ulLoop].pcFile == pstBlk->pstMemBlk->pFileName)
					&& (gs_pstCpssMemStat[ulLoop].ulLine == pstBlk->pstMemBlk->ulLine))
			{
				gs_pstCpssMemStat[ulLoop].ulSum++;
				break;
			}
		}
		if(ulLoop >= ulStatSum)
		{
			if(ulStatSum >= gs_ulMaxStatSum)
			{
				gs_pstCpssMemStat = realloc(gs_pstCpssMemStat, sizeof(*gs_pstCpssMemStat) * gs_ulMaxStatSum * 2);
				if(NULL == gs_pstCpssMemStat)
				{
					return;
				}
				gs_ulMaxStatSum = gs_ulMaxStatSum * 2;
			}
			gs_pstCpssMemStat[ulStatSum].pcFile = pstBlk->pstMemBlk->pFileName;
			gs_pstCpssMemStat[ulStatSum].ulLine = pstBlk->pstMemBlk->ulLine;
			gs_pstCpssMemStat[ulStatSum].ulSum = 1;
			ulStatSum++;
		}
		pstBlk = pstBlk->pstNext;
	}

	if(ulStatSum > 0)
	{
		cps__oams_shcmd_printf("%-10s%-10s%-10s\n", "N_SUM", "N_LINE", "S_FILE");
	}
	for(ulLoop = 0; ulLoop < ulStatSum; ulLoop++)
	{
		cps__oams_shcmd_printf("%-10d%-10d%-10s\n", gs_pstCpssMemStat[ulLoop].ulSum,
				gs_pstCpssMemStat[ulLoop].ulLine, gs_pstCpssMemStat[ulLoop].pcFile);
	}
}

/* 显示内存使用信息，vulMode = 0：打印使用说明
 * 					vulMode = 1：打印全部内存池的概况统计信息，vulPoolId无效
 * 					vulMode = 2：打印指定内存池的详细统计信息，vulPoolId为内存池号
 * 					vulMode = 3：打印全部内存池的详细统计信息，vulPoolId无效
 * 					vulMode = 4：打印检测到的内存内存申请、释放失败、写越界等信息，vulPoolId无效（只打印最近几次的统计）
 * */
void cpss_mem_show(UINT32 vulMode, UINT32 vulPoolId)
{
	UINT32 ulLoop;

	switch(vulMode)
	{
	case 1:
		cpss_mem_show_summarize();
		break;
	case 2:
		cpss_mem_show_detail(vulPoolId);
		break;
	case 3:
		for(ulLoop = 0; ulLoop < CPSS_MAX_MEM_TAB_SUM; ulLoop++)
		{
			cpss_mem_show_detail(ulLoop);
		}
		break;
	case 4:
		cpss_mem_show_fail();
		break;
	default:
		cps__oams_shcmd_printf("This function has two unsigned parameters, the first is vulMode, the second is vulPoolId\n");
		cps__oams_shcmd_printf("    vulMode = 0: print this help information\n");
		cps__oams_shcmd_printf("    vulMode = 1: print all pool's summarize\n");
		cps__oams_shcmd_printf("    vulMode = 2: print detail of the pool that pool id is vulPoolId\n");
		cps__oams_shcmd_printf("    vulMode = 3: print all pool's detail\n");
		cps__oams_shcmd_printf("    vulMode = 4: print detected error information\n");
		break;
	}
}

VOID cpss_mem_usage_get(UINT32 *pulCount, CPSS_DBG_MEM_POOL_INFO_T *pstInfo)
{
	return;
}

UINT32 cpss_mem_usage_take()
{
	UINT32 ulLoop;
	unsigned long long ulUsage = 0;
	
	unsigned long long ulTotal;

	ulTotal = g_ulMemTotalSize * 1024;
	if(0 == ulTotal)
	{
		ulTotal = 2 * 1024 * 1024 * 1024ull;
	}

	for(ulLoop = 0; ulLoop < CPSS_MAX_MEM_TAB_SUM; ulLoop++)
	{
		ulUsage += g_stCpssMemTable[ulLoop].ulUsedBlockNum * cpss_mem_pool2size(ulLoop);
	}

	if((ulUsage * 100 / ulTotal) >= 100)
	{
		return 100;
	}
	else
	{
		return (ulUsage * 100 / ulTotal);
	}
}
