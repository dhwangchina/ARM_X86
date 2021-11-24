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
#include "cpss_config.h"
#include "cpss_com_pci.h"
#include "cpss_com_common.h"
#include "cpss_com_drv.h"
#include "cpss_err.h"

#ifdef CPSS_PCI_INCLUDE
/******************************* 局部宏定义 **********************************/


/******************************* 全局变量定义/初始化 *************************/
CPSS_COM_PCI_LINK_TABLE_T g_stPciLinkTable = {{{0}}} ;
UINT32 g_ulPciInitFlag = 0 ;

/*cpss主动设置从CPU链路故障次数统计*/
UINT32 g_ulSlaveCpuResetNum = 0 ;

/******************************* 局部常数和类型定义 **************************/


/******************************* 局部函数原型声明 ****************************/
extern INT32 cpss_com_link_slave_cpu_reset(UINT32 ulCpuNo);

/******************************* 函数实现 ************************************/

/*******************************************************************************
* 函数名称: cpss_drv_pci_info_get                            
* 功    能: tcp驱动写函数
* 相关文档: 
* 函数类型:    
*                                
* 参    数:                                          
* 参数名称            类型            输入/输出         描述
* 返回值:
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:          
******************************************************************************/

INT32 cpss_drv_pci_info_get(UINT32* pulCpuNum,DRV_PCI_INFO_T* pstPciDrvInfo)
{
    DRV_PCI_INFO_T astDrvInfo[CPSS_DRV_PCI_INFO_ARRAY_MAX] ;
    DRV_PCI_INFO_T *pstDrvPciLink ;
    INT32 lRet ;
    UINT32 ulCpuNum = CPSS_DRV_PCI_INFO_ARRAY_MAX;
    UINT32 ulLoop = 0 ;
    UINT32 ulIndex = 0 ;
    
    pstDrvPciLink = astDrvInfo ;
    
    lRet = drv_pci_info_get(&ulCpuNum,pstDrvPciLink);
    if(CPSS_OK != lRet)
    {
        return lRet ;
    }
    
    if(ulCpuNum == 1)
    {
        pstPciDrvInfo[ulIndex].ulCpuId = astDrvInfo[ulLoop].ulCpuId ;
        pstPciDrvInfo[ulIndex].ulLen = astDrvInfo[ulLoop].ulLen ;
        pstPciDrvInfo[ulIndex].ulStartAddr = astDrvInfo[ulLoop].ulStartAddr ;
        pstPciDrvInfo[ulIndex].ulRamWinAttr = astDrvInfo[ulLoop].ulRamWinAttr ;
        *pulCpuNum = 1 ;
    }
    else
    {
        for(ulLoop=0;ulLoop<ulCpuNum;ulLoop++)
        {
            if(astDrvInfo[ulLoop].ulRamWinAttr == 1)
            {
                pstPciDrvInfo[ulIndex].ulCpuId = astDrvInfo[ulLoop].ulCpuId ;
                pstPciDrvInfo[ulIndex].ulLen = astDrvInfo[ulLoop].ulLen ;
                pstPciDrvInfo[ulIndex].ulStartAddr = astDrvInfo[ulLoop].ulStartAddr ;
                pstPciDrvInfo[ulIndex].ulRamWinAttr = astDrvInfo[ulLoop].ulRamWinAttr ;
                ulIndex = ulIndex + 1 ;
                *pulCpuNum = ulIndex ;
            }
        }     
    }
    

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_pci_memset
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
INT32 cpss_pci_memset(VOID* pvPciMem,UINT32 ulValue,UINT16 ulLen)
{
    #ifdef CPSS_DSP_CPU
    UINT8 *pucDspTemMem =(UINT8*)CPSS_DSP_PCI_TEMP_ADDR;

    CPSS_DSP_FILE_REC();

    memset(pucDspTemMem,ulValue,ulLen) ;

    CPSS_DSP_LINE_REC();

    cpss_dsp6x_master_write(ulLen,(UINT32)pucDspTemMem,(UINT32)pvPciMem) ;

    CPSS_DSP_LINE_REC();

    #else
    memset(pvPciMem,ulValue,ulLen) ;
    #endif

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_pci_memcpy2local
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
INT32 cpss_pci_memcpy2local(VOID* pvDspMem,VOID* pvPciMem,UINT32 ulLen)
{
    #ifdef CPSS_DSP_CPU

    UINT8* pucDspTempAddr =(UINT8*)CPSS_DSP_PCI_TEMP_ADDR ;

    CPSS_DSP_FILE_REC();

    CPSS_DSP_LINE_REC();

    cpss_dsp6x_master_read(ulLen,(UINT32)pucDspTempAddr,(UINT32)pvPciMem) ;

    CPSS_DSP_LINE_REC();

    cpss_mem_memcpy(pvDspMem,pucDspTempAddr, ulLen);
    
    #else
    memcpy(pvDspMem,pvPciMem,ulLen) ;
    #endif

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_pci_memcpy2pci
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
INT32 cpss_pci_memcpy2pci(VOID* pvPciMem,VOID* pvDspMem,UINT32 ulLen)
{
    #ifdef CPSS_DSP_CPU
    UINT8* pucDspTempAddr =(UINT8*)CPSS_DSP_PCI_TEMP_ADDR ;

    cpss_mem_memcpy(pucDspTempAddr,pvDspMem,ulLen) ;

    CPSS_DSP_FILE_REC();
    CPSS_DSP_LINE_REC();

    cpss_dsp6x_master_write(ulLen,(UINT32)pucDspTempAddr,(UINT32)pvPciMem) ;

    CPSS_DSP_LINE_REC();

    #else
    memcpy(pvPciMem,pvDspMem,ulLen) ;
    #endif

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_pci_memsize_check
* 功    能: 检查共享内存的大小是否合法
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
#ifdef CPSS_DSP_CPU
STATUS drv_pcibus_notify_reg (DRV_PCI_CALLBACKFUNCTION_PF pfPciNotifyFunc)
{
    return 0;
}
STATUS drv_pci_dma_area_read                    
(
    UINT32 ulBeginAddr, 
    UINT32 ulReadSize,
    UINT8 *pucBuf
)
{
    return 0;
}
STATUS drv_pci_dma_area_write                  /* DMA写函数*/
(
    UINT32 ulBeginAddr,
    UINT32 ulWriteSize,
    UINT8 *pucBuf
)
{
    return 0;
}
#endif
INT32 cpss_com_pci_memsize_check
(
UINT32 ulMemSize
)
{
    UINT32 ulSize = 0 ;

    ulSize = sizeof(CPSS_COM_EBD_POOL_HEAD_T) + 
             sizeof(CPSS_COM_EBD_T)*CPSS_COM_PCI_RX_EBD_NUM +
             sizeof(CPSS_COM_EBD_T)*CPSS_COM_PCI_TX_EBD_NUM +
             CPSS_COM_PCI_USR_REG_SIZE ;

    if(ulSize>=ulMemSize)
    {
        return CPSS_ERROR ;
    }

    ulSize = ulMemSize - ulSize ;

    if(CPSS_DIV(ulSize,CPSS_COM_PCI_RXTX_BUF_SIZE)<=CPSS_COM_PCI_BUF_NUM_MIN)
    {
        return CPSS_ERROR ;
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_pci_bufpara_cal
* 功    能: 计算共享内存的接收和发送参数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
INT32 cpss_com_pci_buf_para_cal
(
UINT32  ulPciShareMemSize,
UINT32* pulRxBufNum,
UINT32* pulTxBufNum,
UINT32* pulPciRegSize
)
{
    UINT32 ulBufSize = 0 ;
    UINT32 ulAdjustSize = 0 ;
    UINT32 ulBufNum = 0 ;

    /*得到TXBUF+RXBUF+REGBUF的size*/
    ulBufSize = ulPciShareMemSize-sizeof(CPSS_COM_EBD_POOL_HEAD_T)-
                sizeof(CPSS_COM_EBD_T)*CPSS_COM_PCI_RX_EBD_NUM-
                sizeof(CPSS_COM_EBD_T)*CPSS_COM_PCI_TX_EBD_NUM- 
                CPSS_COM_PCI_USR_REG_SIZE ;
    
    ulAdjustSize = CPSS_MOD(ulBufSize,CPSS_COM_PCI_RXTX_BUF_SIZE) ;
    
    /*调整REGBUF的大小*/
    if(ulAdjustSize!=0)
    {
        *pulPciRegSize = CPSS_COM_PCI_USR_REG_SIZE + ulAdjustSize ;
    }

    /*调整TXBUF+RXBUF的大小*/
    ulBufSize = ulBufSize - ulAdjustSize ;
    
    ulBufNum = CPSS_DIV(ulBufSize,CPSS_COM_PCI_RXTX_BUF_SIZE) ;

#ifdef  CPSS_HOST_CPU
    *pulRxBufNum = CPSS_DIV(ulBufNum,2) ;
    
    if(CPSS_MOD(ulBufNum,2)!=0)
    {
        *pulTxBufNum = CPSS_DIV(ulBufNum,2)+1 ;
    }
    else
    {
        *pulTxBufNum = CPSS_DIV(ulBufNum,2) ;
    }
#else
    *pulTxBufNum = CPSS_DIV(ulBufNum,2) ;
    
    if(CPSS_MOD(ulBufNum,2)!=0)
    {
        *pulRxBufNum = CPSS_DIV(ulBufNum,2)+1 ;
    }
    else
    {
        *pulRxBufNum = CPSS_DIV(ulBufNum,2) ;
    }
#endif
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_pci_mem_init
* 功    能: 对PCI进行初始化
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
INT32 cpss_com_pci_mem_init
(
UINT32 ulPciShareAddr,
UINT32 ulSize,
UINT32 ulLinkId,
UINT32 ulFlag
)
{
    UINT32 ulRxBufNum ;
    UINT32 ulTxBufNum ;
    UINT32 ulPciRegSize ;
    UINT32 ulPciShareAddrN ;
    UINT32 ulSizeN ;

    CPSS_COM_EBD_POOL_HEAD_T* pstPciPooHdr=NULL ;
    
    /*判断PCI共享缓冲区长度是否正确*/
    if(cpss_com_pci_memsize_check(ulSize)!=CPSS_OK)
    {
        return CPSS_ERROR ;
    }

    /*计算接收发送缓冲区参数*/
    cpss_com_pci_buf_para_cal(ulSize,&ulRxBufNum,&ulTxBufNum,&ulPciRegSize) ;    
    
    pstPciPooHdr = (CPSS_COM_EBD_POOL_HEAD_T*)ulPciShareAddr ;
    
    /*清空共享内存缓冲区的头部*/
    if(CPSS_COM_PCI_MEM_WRITE_PERMIT==ulFlag)
    {
        cpss_pci_memset((UINT8*)ulPciShareAddr,0,CPSS_PCI_HDR_EBD_LEN) ;
        
        /*填充EBD缓冲池的信息*/
        ulPciShareAddrN = cpss_htonl(ulPciShareAddr) ;
        ulSizeN = cpss_htonl(ulSize) ;
        
        cpss_pci_memcpy2pci(pstPciPooHdr,"^CPU^",5) ;       
        cpss_pci_memcpy2pci(pstPciPooHdr+CPSS_PCI_WIN_BASE_OFFSET,
            (UINT8*)&ulPciShareAddrN,sizeof(UINT32)) ;
        cpss_pci_memcpy2pci(pstPciPooHdr+CPSS_PCI_WIN_SIZE_OFFSET,
            (UINT8*)&ulSizeN,sizeof(UINT32)) ;
    }
    
#ifdef CPSS_HOST_CPU
    /*填充TXEBD&RXEBD的基址信息*/
    g_stPciLinkTable.astPciLink[ulLinkId-1].ulRxEbdStart = (UINT32)CPSS_COM_PCI_POOL_HDR_TO_RX_EBD(ulPciShareAddr) ;
    g_stPciLinkTable.astPciLink[ulLinkId-1].ulTxEbdStart = (UINT32)CPSS_COM_PCI_POOL_HDR_TO_TX_EBD(ulPciShareAddr) ;
    
    /*填充RXEBD的信息*/
    g_stPciLinkTable.astPciLink[ulLinkId-1].ulRxBufBase = (UINT32)CPSS_COM_PCI_POOL_HDR_TO_RXBUF(ulPciShareAddr) ;
    g_stPciLinkTable.astPciLink[ulLinkId-1].ulRxBufNum  = ulRxBufNum ;
    
    /*填充TXEBD的信息*/
    g_stPciLinkTable.astPciLink[ulLinkId-1].ulTxBufBase = (UINT32)CPSS_COM_PCI_POOL_HDR_TO_TXBUF(ulPciShareAddr,ulRxBufNum) ;
    g_stPciLinkTable.astPciLink[ulLinkId-1].ulTxBufNum  = ulTxBufNum ;
#else
    /*填充TXEBD&RXEBD的基址信息*/
    g_stPciLinkTable.astPciLink[ulLinkId-1].ulTxEbdStart = (UINT32)CPSS_COM_PCI_POOL_HDR_TO_TX_EBD(ulPciShareAddr) ;
    g_stPciLinkTable.astPciLink[ulLinkId-1].ulRxEbdStart = (UINT32)CPSS_COM_PCI_POOL_HDR_TO_RX_EBD(ulPciShareAddr) ;
    
    /*填充TXEBD的信息*/
    g_stPciLinkTable.astPciLink[ulLinkId-1].ulTxBufBase = (UINT32)CPSS_COM_PCI_POOL_HDR_TO_TXBUF(ulPciShareAddr) ;
    g_stPciLinkTable.astPciLink[ulLinkId-1].ulTxBufNum  = ulTxBufNum ;
    
    /*填充RXEBD的信息*/
    g_stPciLinkTable.astPciLink[ulLinkId-1].ulRxBufBase = (UINT32)CPSS_COM_PCI_POOL_HDR_TO_RXBUF(ulPciShareAddr,ulTxBufNum) ;
    g_stPciLinkTable.astPciLink[ulLinkId-1].ulRxBufNum  = ulRxBufNum ;
#endif
    /*填充REGBUF的信息*/
    g_stPciLinkTable.astPciLink[ulLinkId-1].ulRegBufSize  = ulPciRegSize ;
    g_stPciLinkTable.astPciLink[ulLinkId-1].ulRegBufStart = (UINT32)CPSS_COM_PCI_POOL_HDR_TO_REGBUF(ulPciShareAddr,ulRxBufNum,ulTxBufNum) ;
    
    /*清空用户缓冲区的内存*/
    if(CPSS_COM_PCI_MEM_WRITE_PERMIT==ulFlag)
    {
        memset((UINT8*)(g_stPciLinkTable.astPciLink[ulLinkId-1].ulRegBufStart),0,ulPciRegSize);
    }

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_pci_mem_init
* 功    能: 对PCI进行初始化
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;                           
* 说   明:                                            
*******************************************************************************/
INT32 cpss_pci_mem_init
(
    UINT32 ulWriteFlag
)
{
    UINT32 ulLoop ;
    UINT32 ulCpuNum =0 ;
    DRV_PCI_INFO_T astDrvPciInfo[CPSS_COM_SLAVE_CPU_NUM];
    DRV_PCI_INFO_T *pstPciInfo ;

    pstPciInfo = astDrvPciInfo ;

    if((CPSS_COM_PCI_MEM_WRITE_PERMIT!=ulWriteFlag)&&
        (CPSS_COM_PCI_MEM_WRITE_NOT_PERMIT!=ulWriteFlag))
    {
        return CPSS_ERROR ;
    }

    while(cpss_drv_pci_info_get(&ulCpuNum,pstPciInfo)!=CPSS_OK)
    {
#ifdef CPSS_VOS_VXWORKS
        taskDelay(1000) ;
#elif defined CPSS_VOS_WINDOWS
        Sleep(1000) ;

#elif defined CPSS_VOS_LINUX
        cpss_delay(1000) ;

#endif
        continue ;
    }
  
    /*得到CPU个数*/
    g_stPciLinkTable.ulSlaveCpuNum = ulCpuNum ;

    /*对本地变量进行赋值操作*/
    for(ulLoop=0;ulLoop<ulCpuNum;ulLoop++)
    {
        g_stPciLinkTable.aulMemPoolAddr[ulLoop] = astDrvPciInfo[ulLoop].ulStartAddr;
        g_stPciLinkTable.aulMemPoolLen[ulLoop]  = astDrvPciInfo[ulLoop].ulLen ;
        g_stPciLinkTable.aulCpuNo[ulLoop]       = astDrvPciInfo[ulLoop].ulCpuId ;
    }

    /*初始化PCI内存*/
    for(ulLoop=0;ulLoop<g_stPciLinkTable.ulSlaveCpuNum;ulLoop++)
    {
        if(cpss_com_pci_mem_init(g_stPciLinkTable.aulMemPoolAddr[ulLoop],
                                 g_stPciLinkTable.aulMemPoolLen[ulLoop],ulLoop+1,ulWriteFlag)==CPSS_ERROR)
        {
            return CPSS_ERROR ;
        }
    }

    g_ulPciInitFlag = 1 ;

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_pci_mem_init_by_linkId
* 功    能: 对PCI进行初始化
*        
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_pci_mem_init_by_linkid
(
 UINT32 ulLinkId,
 UINT32 ulWriteFlag
)
{
    UINT32 ulPciShareAddr;
    UINT32 ulSize;
    INT32  lRet ;

    if((ulLinkId>CPSS_COM_SLAVE_CPU_NUM)||(ulLinkId<0))
    {
        return CPSS_ERROR ;
    }

    ulPciShareAddr = g_stPciLinkTable.aulMemPoolAddr[ulLinkId-1] ;
    ulSize         = g_stPciLinkTable.aulMemPoolLen[ulLinkId-1] ;

    lRet = cpss_com_pci_mem_init(ulPciShareAddr,ulSize,ulLinkId,ulWriteFlag) ;

    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_pci_mem_init_by_linkId
* 功    能: 对PCI进行初始化
*        
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:
*******************************************************************************/
VOID cpss_com_drv_info_init()
{
    UINT32 ulLoop ;
    UINT32 ulCpuNum ;
    DRV_PCI_INFO_T astDrvPciInfo[CPSS_COM_SLAVE_CPU_NUM];
    DRV_PCI_INFO_T *pstPciInfo ;
    
    pstPciInfo = astDrvPciInfo ;
    
    while(cpss_drv_pci_info_get(&ulCpuNum,pstPciInfo)!=CPSS_OK)
    {
#ifdef CPSS_VOS_VXWORKS
        taskDelay(1000) ;
#elif defined CPSS_VOS_WINDOWS
        Sleep(1000) ;

#elif defined CPSS_VOS_LINUX
        cpss_delay(1000) ;

#endif
        continue ;
    }
    
    /*得到CPU个数*/
    g_stPciLinkTable.ulSlaveCpuNum = ulCpuNum ;
    
    /*对本地变量进行赋值操作*/
    for(ulLoop=0;ulLoop<ulCpuNum;ulLoop++)
    {
        g_stPciLinkTable.aulMemPoolAddr[ulLoop] = astDrvPciInfo[ulLoop].ulStartAddr;
        g_stPciLinkTable.aulMemPoolLen[ulLoop]  = astDrvPciInfo[ulLoop].ulLen ;
        g_stPciLinkTable.aulCpuNo[ulLoop]       = astDrvPciInfo[ulLoop].ulCpuId ;
    }
}


/*******************************************************************************
* 函数名称: cpss_com_pci_sendlen_fit
* 功    能: 判断发送缓冲区的长度是否满即将发送的数据的长度
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       满足：CPSS_OK;
*       不满足：CPSS_ERROR;
* 说   明:
*******************************************************************************/
INT32 cpss_com_pci_sendlen_fit
(
UINT32 ulSendLen,
UINT32 ulWriteIndex,
UINT32 ulReadIndex,
UINT32 ulBufNum
)
{
    UINT32 ulFreeBufNum = 0 ;
    
    if(ulWriteIndex<ulReadIndex)
    {
        ulFreeBufNum = ulReadIndex - ulWriteIndex;
    }
    else if(ulWriteIndex>=ulReadIndex)
    {
        ulFreeBufNum = ulBufNum - (ulWriteIndex - ulReadIndex);
    }

    if((ulFreeBufNum-1)*CPSS_COM_PCI_RXTX_BUF_SIZE>=ulSendLen)
    {
        return CPSS_OK ;
    }

    return CPSS_ERROR ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_mem_tx_copy
* 功    能: PCI的写函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                         
*******************************************************************************/
INT32 cpss_com_drv_pci_mem_tx_copy
(
UINT32 ulBeginAddr,
UINT32 ulTxLen,
UINT8* pucWriteBuf
)
{
    INT32 lRet = CPSS_OK ;

#if 0
    if(ulTxLen>CPSS_PCI_DMS_WRITE_MIN)
    {
        lRet = drv_pci_dma_area_write(ulBeginAddr,ulTxLen,pucWriteBuf) ;
        if(lRet != CPSS_OK)
        {
            return lRet ;
        }
    }
    else
#endif
    {
        cpss_pci_memcpy2pci((UINT8*)ulBeginAddr,pucWriteBuf,ulTxLen) ;
    }  
    
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_mem_write
* 功    能: PCI的写函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
VOID cpss_com_pci_mem_write
(
UINT8* pucSrcMem,
UINT8* pucTxBaseMem,
UINT32 ulTxIndex,
UINT32 ulTxLen,
UINT32 ulTxBufNum
)
{
    UINT32 ulTxBufCount ;
    UINT32 ulTxTmpLen ;
    UINT8* pucWriteBuf ;
    
    /*计算接收的数据使用的接收内存个数*/
    if(CPSS_MOD(ulTxLen,CPSS_COM_PCI_RXTX_BUF_SIZE)==0)
    {
        ulTxBufCount = CPSS_DIV(ulTxLen,CPSS_COM_PCI_RXTX_BUF_SIZE) ;
    }
    else
    {
        ulTxBufCount = CPSS_DIV(ulTxLen,CPSS_COM_PCI_RXTX_BUF_SIZE) + 1 ;
    }
    
    pucWriteBuf = pucTxBaseMem + ulTxIndex*CPSS_COM_PCI_RXTX_BUF_SIZE ;
    
    if((ulTxBufNum-ulTxIndex)>=ulTxBufCount)
    {
        cpss_com_drv_pci_mem_tx_copy((UINT32)pucWriteBuf,ulTxLen,pucSrcMem) ; 
    }
    else
    {
        /*第一次拷贝*/
        ulTxTmpLen = (ulTxBufNum-ulTxIndex)*CPSS_COM_PCI_RXTX_BUF_SIZE ;

        cpss_com_drv_pci_mem_tx_copy((UINT32)pucWriteBuf,ulTxTmpLen,pucSrcMem) ; 
        
        /*第二次拷贝*/
        pucSrcMem = pucSrcMem + ulTxTmpLen ;
        ulTxTmpLen = ulTxLen - ulTxTmpLen ;
        pucWriteBuf = pucTxBaseMem ;

        cpss_com_drv_pci_mem_tx_copy((UINT32)pucWriteBuf,ulTxTmpLen,pucSrcMem) ; 
    }
}

/*******************************************************************************
* 函数名称: cpss_com_pci_buf_index_get
* 功    能: 
*        
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*                                
* 说   明:                                            
*******************************************************************************/
INT32 cpss_com_pci_buf_index_get
(
UINT32 ulBufIndex,
UINT32 ulMsgLen,
UINT32 ulBufNumMax
)
{
    UINT32 ulRxBufNum ;

    if(CPSS_MOD(ulMsgLen,CPSS_COM_PCI_RXTX_BUF_SIZE)!=0)
    {
        ulRxBufNum = CPSS_DIV(ulMsgLen,CPSS_COM_PCI_RXTX_BUF_SIZE)+1 ;
    }
    else
    {
        ulRxBufNum = CPSS_DIV(ulMsgLen,CPSS_COM_PCI_RXTX_BUF_SIZE) ;
    }

    return CPSS_MOD((ulRxBufNum+ulBufIndex),ulBufNumMax) ;
}


/*******************************************************************************
* 函数名称: cpss_com_drv_pci_ebd_ready_rx_set
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
VOID cpss_com_drv_pci_ebd_ready_rx_set
(    
CPSS_COM_EBD_T*  pstEBD
)
{
    UINT32 usStatus ;
#ifdef CPSS_DSP_CPU
    cpss_pci_memcpy2local(&usStatus,
        (UINT8*)pstEBD+CPSS_PCI_EBD_STATUS_OFFSET,sizeof(UINT32)) ;
    
    usStatus = cpss_ntohl(usStatus) ;
    usStatus = usStatus|CPSS_COM_PCI_RX_READY_MASK ;

    usStatus = cpss_htonl(usStatus) ;
    cpss_pci_memcpy2pci((UINT8*)pstEBD+CPSS_PCI_EBD_STATUS_OFFSET,
        (UINT8*)&usStatus,sizeof(UINT32)) ;
#else
    *(UINT32*)((UINT8*)pstEBD+CPSS_PCI_EBD_STATUS_OFFSET) = cpss_htonl(cpss_ntohl(*(UINT32*)((UINT8*)pstEBD+CPSS_PCI_EBD_STATUS_OFFSET)) | CPSS_COM_PCI_RX_READY_MASK);
#endif
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_ebd_ready_tx_set
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;                            
* 说   明:                                            
*******************************************************************************/
VOID cpss_com_drv_pci_ebd_ready_tx_set
(
CPSS_COM_EBD_T*  pstEBD
)
{
    UINT32 usStatus = 0 ;

#ifdef CPSS_DSP_CPU
    cpss_pci_memcpy2pci((UINT8*)pstEBD+CPSS_PCI_EBD_STATUS_OFFSET,
        (UINT8*)&usStatus,sizeof(UINT32)) ;
#else
  *(UINT32*)((UINT8*)pstEBD+CPSS_PCI_EBD_STATUS_OFFSET) = usStatus;
#endif

}


/*******************************************************************************
* 函数名称: cpss_com_drv_pci_if_full
* 功    能: 判断发送缓冲区是否满
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR;
* 说   明:                                            
*******************************************************************************/
INT32 cpss_com_drv_pci_if_full
(
UINT32 ulTxBufWriteIndex,
UINT32 ulTxBufReadIndex,
UINT32 ulBufNumMax
)
{
    if(CPSS_MOD((ulTxBufWriteIndex+1),ulBufNumMax) == ulTxBufReadIndex)
    {
        return CPSS_OK ;
    }
    return CPSS_ERROR ;
}


/*******************************************************************************
* 函数名称: cpss_com_pci_write
* 功    能: PCI的写函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
INT32 cpss_com_pci_write
(
UINT32  ulLinkId,
UINT8  *pucBuf,
UINT32  ulDataLen
)
{
    CPSS_COM_EBD_POOL_HEAD_T* pstEbdPoolHdr ;
    CPSS_COM_PCI_DRV_LINK_T* pstPciLink ;
    CPSS_COM_EBD_T*      pstTxEBD ;
    UINT32 ulWriteIndex ;
    UINT32 ulReadIndex ;
    UINT32 ulEndWriteIndex ;
    UINT32 ulTxEbdWriteIndex ;
    UINT32 usStatus ;
        
    if(ulLinkId>g_stPciLinkTable.ulSlaveCpuNum)
    {
        return CPSS_ERROR ;
    }

    /*得到PCI共享数据交换缓冲区指针*/
    pstPciLink = &g_stPciLinkTable.astPciLink[ulLinkId-1] ;
    pstEbdPoolHdr = (CPSS_COM_EBD_POOL_HEAD_T*)g_stPciLinkTable.aulMemPoolAddr[ulLinkId-1] ;

    if(pstEbdPoolHdr==NULL)
    {
        return CPSS_ERROR ;
    }
    
    /*如果PCI内存还未初始化好，直接返回失败*/
#ifdef  CPSS_HOST_CPU
    if(0!=memcmp(pstEbdPoolHdr->aucFieldName,"^CPU^",5))
    {
        return CPSS_ERROR ;
    }
#endif
   
    /*找到对应的TXEBD结构*/
#ifdef CPSS_DSP_CPU
    cpss_pci_memcpy2local(&ulTxEbdWriteIndex,
        (UINT8*)pstEbdPoolHdr+CPSS_PCI_TXEBD_WRITE_INDEX_OFFSET,sizeof(UINT32)) ;       
    ulTxEbdWriteIndex = cpss_ntohl(ulTxEbdWriteIndex) ;
#else
    ulTxEbdWriteIndex = cpss_ntohl(*(UINT32*)((UINT8*)pstEbdPoolHdr+CPSS_PCI_TXEBD_WRITE_INDEX_OFFSET));
#endif

    /*判断从CPU是否已经复位*/
#ifdef  CPSS_HOST_CPU
    if(ulTxEbdWriteIndex > CPSS_COM_PCI_TX_EBD_NUM-1)
    {
        return CPSS_ERROR ;
    }
#endif

    pstTxEBD = (CPSS_COM_EBD_T*)(pstPciLink->ulTxEbdStart)+ulTxEbdWriteIndex ;

#ifdef CPSS_DSP_CPU
    cpss_pci_memcpy2local(&usStatus,
        ((UINT8*)pstTxEBD + CPSS_PCI_EBD_STATUS_OFFSET),sizeof(UINT32)) ;
    if(CPSS_COM_PCI_TX_READY(cpss_ntohl(usStatus))==TRUE)
#else
    if(CPSS_COM_PCI_TX_READY(cpss_ntohl(*(UINT32*)((UINT8*)pstTxEBD + CPSS_PCI_EBD_STATUS_OFFSET)))==TRUE)
#endif
    {
#ifdef CPSS_DSP_CPU
        cpss_pci_memcpy2local(&ulWriteIndex,
            ((UINT8*)pstEbdPoolHdr + CPSS_PCI_TXBUF_WRITE_INDEX_OFFSET),sizeof(UINT32)) ;
        cpss_pci_memcpy2local(&ulReadIndex,
            ((UINT8*)pstEbdPoolHdr + CPSS_PCI_TXBUF_READ_INDEX_OFFSET),sizeof(UINT32)) ;
        ulWriteIndex = cpss_ntohl(ulWriteIndex) ;
        ulReadIndex  = cpss_ntohl(ulReadIndex) ;
#else
        ulWriteIndex = cpss_ntohl(*(UINT32*)((UINT8*)pstEbdPoolHdr + CPSS_PCI_TXBUF_WRITE_INDEX_OFFSET));
        ulReadIndex = cpss_ntohl(*(UINT32*)((UINT8*)pstEbdPoolHdr + CPSS_PCI_TXBUF_READ_INDEX_OFFSET));
#endif
        /*判断发送缓冲区的长度是否满足需要的长度*/
        if(cpss_com_drv_pci_if_full(ulWriteIndex,  ulReadIndex, pstPciLink->ulTxBufNum) == CPSS_OK)
        {
            return CPSS_ERROR ;
        }

        if(cpss_com_pci_sendlen_fit(ulDataLen,ulWriteIndex,ulReadIndex,
                                    pstPciLink->ulTxBufNum) == CPSS_ERROR)
        {
            return CPSS_ERROR ;
        }
            
        /*向PCI共享缓冲区中写入数据*/
        cpss_com_pci_mem_write(pucBuf,(UINT8*)(pstPciLink->ulTxBufBase),ulWriteIndex,
                               ulDataLen,pstPciLink->ulTxBufNum);

        /*移动写入缓冲区的写指针*/
        ulEndWriteIndex =  cpss_com_pci_buf_index_get(ulWriteIndex,ulDataLen,
                                                      pstPciLink->ulTxBufNum) ;
        
        /*设置txEBD中的内容*/
#ifdef CPSS_DSP_CPU
        ulWriteIndex = cpss_htonl(ulWriteIndex) ;
        ulDataLen = cpss_htonl(ulDataLen) ;
        cpss_pci_memcpy2pci((UINT8*)pstTxEBD+CPSS_PCI_EBD_BUF_INDEX_OFFSET,
            (UINT8*)&ulWriteIndex,sizeof(UINT32)) ;
        cpss_pci_memcpy2pci((UINT8*)pstTxEBD+CPSS_PCI_EBD_LENGTH_OFFSET,
            (UINT8*)&ulDataLen,sizeof(UINT32)) ;
#else
        *(UINT32*)((UINT8*)pstTxEBD+CPSS_PCI_EBD_BUF_INDEX_OFFSET) = cpss_htonl(ulWriteIndex);
        *(UINT32*)((UINT8*)pstTxEBD+CPSS_PCI_EBD_LENGTH_OFFSET) = cpss_htonl(ulDataLen) ;
#endif
        /*设置PCI的EBD标志为可读*/
        cpss_com_drv_pci_ebd_ready_rx_set(pstTxEBD) ;
#ifdef CPSS_DSP_CPU
        ulEndWriteIndex = cpss_htonl(ulEndWriteIndex) ;
        cpss_pci_memcpy2pci((UINT8*)pstEbdPoolHdr+CPSS_PCI_TXBUF_WRITE_INDEX_OFFSET,
            (UINT8*)&ulEndWriteIndex,sizeof(UINT32)) ;

        /*移动TXEBD的写指针*/
        ulTxEbdWriteIndex = CPSS_MOD((ulTxEbdWriteIndex + 1),CPSS_COM_PCI_TX_EBD_NUM) ;
        ulTxEbdWriteIndex = cpss_htonl(ulTxEbdWriteIndex) ;
        cpss_pci_memcpy2pci((UINT8*)pstEbdPoolHdr+CPSS_PCI_TXEBD_WRITE_INDEX_OFFSET,
            (UINT8*)&ulTxEbdWriteIndex,sizeof(UINT32)) ;
#else
        *(UINT32*)((UINT8*)pstEbdPoolHdr+CPSS_PCI_TXBUF_WRITE_INDEX_OFFSET) = cpss_htonl(ulEndWriteIndex) ;
        *(UINT32*)((UINT8*)pstEbdPoolHdr+CPSS_PCI_TXEBD_WRITE_INDEX_OFFSET) = cpss_htonl(CPSS_MOD((ulTxEbdWriteIndex + 1),CPSS_COM_PCI_TX_EBD_NUM));
#endif  
        return CPSS_OK ;
    }

    return CPSS_ERROR ;
}

/*******************************************************************************
* 函数名称: cpss_com_drv_pci_mem_tx_copy
* 功    能: 
*        
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       有数据：  CPSS_OK;
*       没有数据：CPSS_ERROR;                           
* 说   明:                                          
*******************************************************************************/
INT32 cpss_com_drv_pci_mem_rx_copy
(
 UINT32 ulBeginAddr,
 UINT32 ulRxLen,
 UINT8* pucReadBuf
)
{
    INT32 lRet = CPSS_OK ;
    
#if 0
    if(ulRxLen>CPSS_PCI_DMA_READ_MIN)
    {
        lRet = drv_pci_dma_area_read(ulBeginAddr,ulRxLen,pucReadBuf) ;
        if(lRet != CPSS_OK)
        {
            return lRet ;
        }
    }
    else
#endif
    {
        cpss_pci_memcpy2local(pucReadBuf,(UINT8*)ulBeginAddr,ulRxLen) ;
    }  
    
    return CPSS_OK ;    
}
 

/*******************************************************************************
* 函数名称: cpss_com_pci_mem_read
* 功    能: 
*        
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       有数据：  CPSS_OK;
*       没有数据：CPSS_ERROR;                           
* 说   明:                                            
*******************************************************************************/
VOID cpss_com_pci_mem_read
(
UINT8* pucDestMem,
UINT8* pucRxBaseMem,
UINT32 ulRxIndex,
UINT32 ulRxLen,
UINT32 ulRxBufNum
)
{
    UINT32 ulRxBufCount ;
    UINT32 ulRxTmpLen ;
    UINT8* pucReadBuf ;

    /*计算接收的数据使用的接收内存个数*/
    if(CPSS_MOD(ulRxLen,CPSS_COM_PCI_RXTX_BUF_SIZE)==0)
    {
        ulRxBufCount = CPSS_DIV(ulRxLen,CPSS_COM_PCI_RXTX_BUF_SIZE) ;
    }
    else
    {
        ulRxBufCount = CPSS_DIV(ulRxLen,CPSS_COM_PCI_RXTX_BUF_SIZE) + 1 ;
    }

    pucReadBuf = pucRxBaseMem + ulRxIndex*CPSS_COM_PCI_RXTX_BUF_SIZE ;

    if((ulRxBufNum-ulRxIndex)>=ulRxBufCount)
    {
        cpss_com_drv_pci_mem_rx_copy((UINT32)pucReadBuf,ulRxLen,pucDestMem);
    }
    else
    {
        /*计算第一次拷贝的长度*/
        ulRxTmpLen = (ulRxBufNum-ulRxIndex)*CPSS_COM_PCI_RXTX_BUF_SIZE ;
        cpss_com_drv_pci_mem_rx_copy((UINT32)pucReadBuf,ulRxTmpLen,pucDestMem);

        /*第二次拷贝*/
        pucDestMem = pucDestMem + ulRxTmpLen ;
        ulRxTmpLen = ulRxLen - ulRxTmpLen ;
        pucReadBuf = pucRxBaseMem ;
        cpss_com_drv_pci_mem_rx_copy((UINT32)pucReadBuf,ulRxTmpLen,pucDestMem);
    }
}

/*******************************************************************************
* 函数名称: cpss_com_pci_read_len_get
* 功    能: 得到读取的数据的大小
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                       
*******************************************************************************/
INT32 cpss_com_pci_read_len_get
(
UINT32 ulLinkId,
UINT32 *pulDataLen
)
{
    CPSS_COM_EBD_POOL_HEAD_T* pstPciHdr ;
    CPSS_COM_EBD_T* pstRxEbd ;
    CPSS_COM_PCI_DRV_LINK_T* pstComPciLink ;
    UINT32 ulRxEbdReadIndex ;
    UINT32 ulCpuNo = 0 ;
    UINT32 usStatus = 0 ;
    UINT32 usLength = 0 ;
    
    ulCpuNo = 0 ;
    if((ulLinkId<=0)||(ulLinkId>g_stPciLinkTable.ulSlaveCpuNum))
    {
        return CPSS_ERR_COM_PARA_ILLEGAL ;
    }

    /*得到PCI共享内存的数据结构*/
    pstPciHdr = (CPSS_COM_EBD_POOL_HEAD_T*)(g_stPciLinkTable.aulMemPoolAddr[ulLinkId-1]) ;
    pstComPciLink = &g_stPciLinkTable.astPciLink[ulLinkId-1] ;
    
    /*判断从CPU是否已经复位*/
#ifdef CPSS_DSP_CPU
    cpss_pci_memcpy2local(&ulRxEbdReadIndex,
        ((UINT8*)pstPciHdr + CPSS_PCI_RXEBD_READ_INDEX_OFFSET),sizeof(UINT32)) ;
    ulRxEbdReadIndex = cpss_ntohl(ulRxEbdReadIndex) ;
#else
    ulRxEbdReadIndex = cpss_ntohl(*(UINT32*)((UINT8*)pstPciHdr + CPSS_PCI_RXEBD_READ_INDEX_OFFSET));
#endif

#ifdef  CPSS_HOST_CPU
    if(ulRxEbdReadIndex > CPSS_COM_PCI_RX_EBD_NUM-1)
    {
        /*设置本板连路为*/
        ulCpuNo = g_stPciLinkTable.aulCpuNo[ulLinkId-1] ;

        /*调用平台的接口函数，设置连路故障*/
        cpss_com_link_slave_cpu_reset(ulCpuNo);
        
        g_ulSlaveCpuResetNum = g_ulSlaveCpuResetNum + 1 ;
        
        return CPSS_ERROR ;
    }
#endif
    pstRxEbd = (CPSS_COM_EBD_T*)(pstComPciLink->ulRxEbdStart) + ulRxEbdReadIndex ;
    
    /*没有数据返回失败*/
#ifdef CPSS_DSP_CPU
    cpss_pci_memcpy2local(&usStatus,
        ((UINT8*)pstRxEbd + CPSS_PCI_EBD_STATUS_OFFSET),sizeof(UINT32)) ;
    
    if(CPSS_COM_PCI_RX_READY(cpss_ntohl(usStatus))==FALSE)
#else
    if(CPSS_COM_PCI_RX_READY(cpss_ntohl(*(UINT32*)((UINT8*)pstRxEbd + CPSS_PCI_EBD_STATUS_OFFSET)))==FALSE)
#endif
    {
        *pulDataLen = 0 ;
        return CPSS_OK ;
    }

    /*没有数据返回失败*/
#ifdef CPSS_DSP_CPU
    cpss_pci_memcpy2local(&usLength,
        ((UINT8*)pstRxEbd + CPSS_PCI_EBD_LENGTH_OFFSET),sizeof(UINT32)) ;

    *pulDataLen = cpss_ntohl(usLength) ;
#else
    *pulDataLen = cpss_ntohl(*(UINT32*)((UINT8*)pstRxEbd + CPSS_PCI_EBD_LENGTH_OFFSET));
#endif
    
    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_pci_read
* 功    能: 
*        
* 函数类型:
* 参    数:
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       有数据：  CPSS_OK;
*       没有数据：CPSS_ERROR;                           
* 说   明:                                            
*******************************************************************************/
INT32 cpss_com_pci_read
(
UINT8*  pucDataAddr,
UINT32  ulLinkId
)
{
    CPSS_COM_EBD_POOL_HEAD_T* pstPciHdr ;
    CPSS_COM_EBD_T* pstRxEbd ;
    CPSS_COM_PCI_DRV_LINK_T* pstComPciLink ;
    UINT8* ucpRxBufBase ;
    UINT32 ulMsgLen ;
    UINT32 ulRxReadIndex ;
    UINT32 ulRxEbdReadIndex ;
    UINT32 ulRxBufReadIndex ;
    UINT32 usStatus ;
    UINT32 usLength ;

    pstPciHdr = (CPSS_COM_EBD_POOL_HEAD_T*)(g_stPciLinkTable.aulMemPoolAddr[ulLinkId-1]) ;
    pstComPciLink = &g_stPciLinkTable.astPciLink[ulLinkId-1] ;
    
    /*判断从CPU是否已经复位*/
#ifdef CPSS_DSP_CPU
    cpss_pci_memcpy2local(&ulRxEbdReadIndex,
        ((UINT8*)pstPciHdr + CPSS_PCI_RXEBD_READ_INDEX_OFFSET),sizeof(UINT32)) ;
    ulRxEbdReadIndex = cpss_ntohl(ulRxEbdReadIndex) ;
#else
    ulRxEbdReadIndex = cpss_ntohl(*(UINT32*)((UINT8*)pstPciHdr + CPSS_PCI_RXEBD_READ_INDEX_OFFSET));
#endif

    /*判断从CPU是否已经复位*/
#ifdef  CPSS_HOST_CPU
    if(ulRxEbdReadIndex > CPSS_COM_PCI_RX_EBD_NUM-1)
    {
        return CPSS_ERROR ;
    }
#endif
    
    pstRxEbd = (CPSS_COM_EBD_T*)(pstComPciLink->ulRxEbdStart) + ulRxEbdReadIndex ;

    /*没有数据返回失败*/
#ifdef CPSS_DSP_CPU
    cpss_pci_memcpy2local(&usStatus,
        ((UINT8*)pstRxEbd + CPSS_PCI_EBD_STATUS_OFFSET),sizeof(UINT32)) ;    
    if(CPSS_COM_PCI_RX_READY(cpss_ntohl(usStatus))==FALSE)
#else
    if(CPSS_COM_PCI_RX_READY(cpss_ntohl(*(UINT32*)((UINT8*)pstRxEbd + CPSS_PCI_EBD_STATUS_OFFSET)))==FALSE)
#endif
    {
        return CPSS_ERROR ;
    }

    ucpRxBufBase = (UINT8*)(pstComPciLink->ulRxBufBase) ;
#ifdef CPSS_DSP_CPU
    cpss_pci_memcpy2local(&usLength,
        ((UINT8*)pstRxEbd + CPSS_PCI_EBD_LENGTH_OFFSET),sizeof(UINT32)) ;    
    ulMsgLen = cpss_ntohl(usLength) ;
#else
    ulMsgLen = cpss_ntohl(*(UINT32*)((UINT8*)pstRxEbd + CPSS_PCI_EBD_LENGTH_OFFSET));
#endif

#ifdef CPSS_DSP_CPU
    cpss_pci_memcpy2local(&ulRxBufReadIndex,
        ((UINT8*)pstPciHdr + CPSS_PCI_RXBUF_READ_INDEX_OFFSET),sizeof(UINT32)) ;    
    ulRxBufReadIndex = cpss_ntohl(ulRxBufReadIndex) ;
#else
    ulRxBufReadIndex = cpss_ntohl(*(UINT32*)((UINT8*)pstPciHdr + CPSS_PCI_RXBUF_READ_INDEX_OFFSET));
#endif

    /*判断从CPU是否已经复位*/
    if((ulMsgLen > pstComPciLink->ulRxBufNum*CPSS_COM_PCI_RXTX_BUF_SIZE)||
        (ulRxBufReadIndex > (pstComPciLink->ulRxBufNum-1)))
    {
        return CPSS_ERROR ;
    }

    /*拷贝数据到本地内存中*/
    cpss_com_pci_mem_read(pucDataAddr,ucpRxBufBase,
                          ulRxBufReadIndex,ulMsgLen,
                          pstComPciLink->ulRxBufNum) ;

    /*移动接收缓冲区的读指针*/
#ifdef CPSS_DSP_CPU
    ulRxReadIndex = cpss_htonl(cpss_com_pci_buf_index_get(ulRxBufReadIndex,ulMsgLen,pstComPciLink->ulRxBufNum));
    cpss_pci_memcpy2pci((UINT8*)pstPciHdr+CPSS_PCI_RXBUF_READ_INDEX_OFFSET,&ulRxReadIndex,sizeof(UINT32));
#else
    *(UINT32*)((UINT8*)pstPciHdr+CPSS_PCI_RXBUF_READ_INDEX_OFFSET) = 
         cpss_htonl(cpss_com_pci_buf_index_get(ulRxBufReadIndex,ulMsgLen,pstComPciLink->ulRxBufNum));
#endif

    /*设置EBD为可写*/
    cpss_com_drv_pci_ebd_ready_tx_set(pstRxEbd) ;

    /*移动接收EBD的读指针*/
#ifdef CPSS_DSP_CPU
    ulRxEbdReadIndex = cpss_htonl(CPSS_MOD((ulRxEbdReadIndex+1),CPSS_COM_PCI_RX_EBD_NUM)) ;
    cpss_pci_memcpy2pci((UINT8*)pstPciHdr+CPSS_PCI_RXEBD_READ_INDEX_OFFSET,&ulRxEbdReadIndex,sizeof(UINT32));
#else

    *(UINT32*)((UINT8*)pstPciHdr+CPSS_PCI_RXEBD_READ_INDEX_OFFSET) = cpss_htonl(CPSS_MOD((ulRxEbdReadIndex+1),CPSS_COM_PCI_RX_EBD_NUM)) ;
#endif

    return CPSS_OK ;
}


/*******************************************************************************
* 函数名称: cpss_com_pci_special_write
* 功    能: PCI驱动提供的交互区寄存器写接口函数：
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
INT32 cpss_com_pci_special_write
(
UINT32 ulLinkId,
UINT32 ulOffSet,
UINT8* pucBuf,
UINT32 ulLen
)
{
    UINT8* pucPciBuf ;
    CPSS_PCI_USER_REG_MANAGE_T* pstUsrRegMan ;

    /*判断交互区ID是否合法*/
    if((ulLinkId<=0)||(ulLinkId>g_stPciLinkTable.ulSlaveCpuNum))
    {
        return CPSS_ERR_COM_PARA_ILLEGAL;
    }

    pstUsrRegMan = (CPSS_PCI_USER_REG_MANAGE_T*)g_stPciLinkTable.astPciLink[ulLinkId-1].ulRegBufStart ;

#if 0
    /*缓冲区不空*/
    if(pstUsrRegMan->ulStat == 1)
    {
        printf("cpss_com_pci_special_write ulStat=%d\n",pstUsrRegMan->ulStat) ;
        return CPSS_ERROR ;
    }
#endif
    
    /*判断写入数据长度是否合法*/
    if((ulOffSet+ulLen)>(g_stPciLinkTable.astPciLink[ulLinkId-1].ulRegBufSize-sizeof(CPSS_PCI_USER_REG_MANAGE_T)))
    {
       printf("cpss_com_pci_special_write,ulLen=%d\n",ulOffSet+ulLen);
        return CPSS_ERROR ;
    }

    pucPciBuf = (UINT8*)(g_stPciLinkTable.astPciLink[ulLinkId-1].ulRegBufStart+sizeof(CPSS_PCI_USER_REG_MANAGE_T)) ;
    
    /*从PCI共享内存拷贝数据*/
    cpss_pci_memcpy2pci(pucPciBuf+ulOffSet,pucBuf,ulLen) ;

    /*pstUsrRegMan->ulStat = 1 ;*/

    /*printf("cpss_com_pci_special_write succ!\n") ;*/

    return CPSS_OK;
}


/*******************************************************************************
* 函数名称: cpss_com_pci_special_read
* 功    能: PCI驱动提供的交互区寄存器读取数据接口函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
INT32 cpss_com_pci_special_read
(
UINT32 ulLinkId,
UINT32 ulOffSet,
UINT8 *pucBuf,
UINT32 ulLen
)
{
    UINT8* pucPciBuf ;
    CPSS_PCI_USER_REG_MANAGE_T* pstUsrRegMan ;    
    
    /* 判断交互区ID是否合法 */
    if( ulLinkId >g_stPciLinkTable.ulSlaveCpuNum)
    {
        return CPSS_ERR_COM_PARA_ILLEGAL;
    }

    pstUsrRegMan = (CPSS_PCI_USER_REG_MANAGE_T*)g_stPciLinkTable.astPciLink[ulLinkId-1].ulRegBufStart ;

#if 0    
    /*缓冲区空*/
    if(pstUsrRegMan->ulStat == 0)
    {
        return CPSS_ERROR ;
    }
#endif

    /*判断读出数据长度是否合法*/
    if(ulOffSet+ulLen>g_stPciLinkTable.astPciLink[ulLinkId-1].ulRegBufSize-sizeof(CPSS_PCI_USER_REG_MANAGE_T))
    {
        return CPSS_ERROR ;
    }

    pucPciBuf = (UINT8*)(g_stPciLinkTable.astPciLink[ulLinkId-1].ulRegBufStart+sizeof(CPSS_PCI_USER_REG_MANAGE_T)) ;

    /*从PCI共享内存拷贝数据*/
    cpss_pci_memcpy2local(pucBuf,pucPciBuf+ulOffSet,ulLen) ;

    /*pstUsrRegMan->ulStat = 0 ;*/
    
    return CPSS_OK;
}

/*******************************************************************************
* 函数名称: cpss_com_pci_time_write
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
INT32 cpss_com_pci_time_write
(
UINT8 *pucBuf,
UINT32 ulSendLen
)
{
     
    UINT32 ulCpuNum ;
    UINT32 ulLoop ;

    ulCpuNum = g_stPciLinkTable.ulSlaveCpuNum ;

    for(ulLoop=0;ulLoop<ulCpuNum;ulLoop++)
    {
        cpss_com_pci_special_write(ulLoop+1,100,pucBuf,ulSendLen) ;   
    }
    
    return CPSS_OK ;

}

/*******************************************************************************
* 函数名称: cpss_com_pci_time_write
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
INT32 cpss_com_pci_time_read
(
UINT8 *pucBuf,
UINT32 ulReadLen
)
{
    cpss_com_pci_special_read(1,100,pucBuf,ulReadLen) ;

    /*检查校验和*/


    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_pci_special_zero
* 功    能: 清空special缓冲区
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
VOID cpss_com_pci_special_zero(UINT32 ulLinkId)
{
    cpss_pci_memset((UINT8*)(g_stPciLinkTable.astPciLink[ulLinkId-1].ulRegBufStart),
        0,g_stPciLinkTable.astPciLink[ulLinkId-1].ulRegBufSize);
}

/*******************************************************************************
* 函数名称: cpss_set_pci_link_fault_stat_show
* 功    能: 显示cpss主动设置pci链路状态为故障的次数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
* 说   明:                                                                                    
*******************************************************************************/
VOID cpss_set_pci_link_fault_stat_show()
{
    printf("---------cpss_set_pci_link_fault_stat----------\n") ;
    printf("g_ulSlaveCpuResetNum=%d\n",g_ulSlaveCpuResetNum) ;
}


/*******************************************************************************
* 函数名称: smss_drv_pci_write()
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
#ifdef CPSS_SLAVE_CPU
INT32 smss_drv_pci_write
(
    VOID* pvSendBuf,    /*发送的纯数据内存*/
    UINT32 ulBufLen,    /*发送的纯数据长度*/
    UINT32 ulMsgId,     /*发送的消息ID*/
    UINT32 ulDesPId     /*目的纤程ID*/
)
{
    INT32 lRet ;

    CPSS_COM_TRUST_HEAD_T* pstTrustHdr ;
    CPSS_COM_MSG_HEAD_T* pstDispHdr ;
    UINT8* pucUsrData ;

    pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)malloc(ulBufLen + CPSS_COM_LINK_HEAD_LEN) ;
    if(NULL == pstTrustHdr)
    {
        return CPSS_ERROR ;
    }

    pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr) ;
    pucUsrData = CPSS_TRUST_HDR_TO_USR_HDR(pstTrustHdr) ;
    

    /*清零头通信头*/
    memset((UINT8*)pstTrustHdr,0,CPSS_COM_TRUST_HEAD_LEN) ;
    memset((UINT8*)pstDispHdr,0,CPSS_COM_DISP_HEAD_LEN) ;
    
    pstDispHdr->stDstProc.ulPd = cpss_htonl(ulDesPId) ;
    pstDispHdr->ulMsgId = cpss_htonl(ulMsgId) ;

    /*拷贝数据*/
    memcpy(pucUsrData,(UINT8*)pvSendBuf,ulBufLen) ;
 
    lRet = cpss_com_pci_write(1,(UINT8*)pstTrustHdr,CPSS_COM_LINK_HEAD_LEN+ulBufLen) ;

    /*释放内存*/
    free((void*)pstTrustHdr) ;

    return lRet ;
}

/*******************************************************************************
* 函数名称: smss_drv_pci_read()
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
INT32 smss_drv_pci_read
(
 VOID* pvSendBuf,        /*接收的纯数据内存*/
 UINT32* pulRecvLen,     /*接收的纯数据长度*/
 UINT32* pulMsgId        /*接收的消息ID*/
)
{
    INT32 lRet ;
    UINT32 ulRecvLen ;
    CPSS_COM_TRUST_HEAD_T* pstTrustHdr ;
    CPSS_COM_MSG_HEAD_T* pstDispHdr ;
    UINT8* pucUsrData ;

    lRet = cpss_com_pci_read_len_get(1,&ulRecvLen) ;
    if(lRet != CPSS_OK)
    {
        return CPSS_ERROR ;
    }

    pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)malloc(ulRecvLen) ;
    if(NULL == pstTrustHdr)
    {
        return CPSS_ERROR ;
    }

    pstDispHdr = (CPSS_COM_MSG_HEAD_T*)CPSS_TRUST_HDR_TO_DISP_HDR(pstTrustHdr) ;
    pucUsrData = CPSS_TRUST_HDR_TO_USR_HDR(pstTrustHdr) ;

    lRet = cpss_com_pci_read((UINT8*)pstTrustHdr,1) ;
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }

    ulRecvLen = ulRecvLen - CPSS_COM_LINK_HEAD_LEN ;

    *pulRecvLen = ulRecvLen ;
    *pulMsgId = cpss_ntohl(pstDispHdr->ulMsgId) ;
    
    memcpy((UINT8*)pvSendBuf,pucUsrData,ulRecvLen) ;

    /*释放内存*/
    free((VOID*)pstTrustHdr) ;

    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: smss_drv_pci_init()
* 功    能: 
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；                            
* 说   明:                                            
*******************************************************************************/
INT32 smss_drv_pci_init()
{
    INT32 lRet ;
    
    /*初始化PCI内存*/
    lRet = cpss_pci_mem_init(CPSS_COM_PCI_MEM_WRITE_PERMIT);
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }

    /*向主CPU发送中断*/
    lRet = drv_pci_notify_send(1, 0x1, 0);   

    return lRet ;
}
#endif
#endif /* end ifdef CPSS_PCI_INCLUDE */
#ifdef CPSS_DSP_CPU

extern void mac_init (void);
extern void mac_call (void);
extern INT32 chip_poll (void);

INT8    macBuffer [IPC_MAX_MSG_LEN];

INT32 (*pTnbsCallBackFun)(VOID) = NULL;
void (*pTpssCallBackFun)(VOID)= NULL;

void mac_init (void)
{
    CPSS_VK_IPC_MSG_NODE_T *  ptNode;
    IPC_MSG_HDR_T *   ptHdr;
    CPSS_COM_MSG_HEAD_T *    ptData;
    
    /* Get a node. */
    
    ptNode = (CPSS_VK_IPC_MSG_NODE_T *) macBuffer;
    ptHdr = (IPC_MSG_HDR_T *) IPC_MSG_NODE_TO_HDR (ptNode);
    
    /* Clean message header. */
    
    memset ((void *) ptHdr, 0, sizeof (IPC_MSG_HDR_T));
    
    /* Set message header. */
    
    ptHdr->ptNext = NULL;
    
    /* Pointer to data. */
    
    ptData = (CPSS_COM_MSG_HEAD_T *) IPC_MSG_HDR_TO_DATA (ptHdr);
    
    /* Clean data. */
    
    memset ((void *) ptData, 0, sizeof (CPSS_COM_MSG_HEAD_T));
    
    ptData->ulMsgId = 0xFFFF0001;
    ptData->pucBuf = NULL;
}

void cpss_callbak_reg(UINT8 subSys, VOID *pfFun)
{
   if(subSys == SWP_SUBSYS_TPSS)
   {
       pTpssCallBackFun= pfFun;
   }
   else if(subSys == SWP_SUBSYS_UPM)
   {
       pTnbsCallBackFun = pfFun;
   }
}

extern UINT32 g_ulHeartBeartCntr;

void mac_call (void)
{
    if( g_ulHeartBeartCntr == 1 )
    {
       g_ulHeartBeartCntr = 0;                                                                                          
       smss_slave_dsp_hb_up_timeout();
    }
    if(pTpssCallBackFun != NULL)
    {
       pTpssCallBackFun();
    }
}

INT32 chip_poll (void)
{
    INT32 lRet = 0;
    if(pTnbsCallBackFun != NULL)
    {
       lRet = pTnbsCallBackFun();
    }
    return lRet;
}

#endif 
/*================  函数实现  <结束>  ================*/




