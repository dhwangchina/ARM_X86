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
#include "cpss_com_common.h"
#include "cpss_vk_proc.h"
#include "cpss_com_frag.h"


/******************************* 局部宏定义 **********************************/

/******************************* 全局变量定义/初始化 *************************/

/******************************* 局部常数和类型定义 **************************/

/******************************* 局部函数原型声明 ****************************/

/******************************* 函数实现 ************************************/

#ifndef CPSS_DSP_CPU
/*******************************************************************************
* 函数名称: cpss_com_frag_ipc_hdr_clear
* 功    能: 清零IPC包头数据
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
void cpss_com_frag_ipc_hdr_clear
(
CPSS_COM_TRUST_HEAD_T* pstTrustHdr
)
{
    IPC_MSG_HDR_T* pstIpcHdr ;

    pstIpcHdr = (IPC_MSG_HDR_T*)CPSS_TRUST_HDR_TO_IPC_HDR(pstTrustHdr);

    pstIpcHdr->ptNext = NULL ;
}


/*******************************************************************************
* 函数名称: cpss_com_frag_offset_inc
* 功    能: 在包头的相应位置将offset增加一
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
/**
VOID cpss_com_frag_offset_set
(
CPSS_COM_TRUST_HEAD_T*  pstTrustHdr,
UINT16 usOffset
)
{
    pstTrustHdr->fragOffsetEx = usOffset&0x00ff ;
    pstTrustHdr->fragOffset   = usOffset>>8 ;
}
**/
#define cpss_com_frag_offset_set(x,y) \
    (x)->fragOffsetEx = (y) & 0x00FF; \
    (x)->fragOffset   = ((y) >> 8) & 0x00FF;  \
/*******************************************************************************
* 函数名称: cpss_com_frag_offset_inc
* 功    能: 在包头的相应位置将offset增加一
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
INT32 cpss_com_frag_offset_cmp
(
CPSS_COM_TRUST_HEAD_T*  pstTrustHdr,
UINT16 usOffset 
)
{
    UINT32 usFragOffset = 0 ;

    usFragOffset = (pstTrustHdr->fragOffset)<<8;

    usFragOffset = usFragOffset + pstTrustHdr->fragOffsetEx ;

    if(usFragOffset==usOffset)
    {
        return 0 ;
    }
    else if(usFragOffset<usOffset)
    {
        return -1 ;
    }
    
    return 1 ;
}


/*******************************************************************************
* 函数名称: cpss_com_frag_next_slice_get
* 功    能: 拆包过程中申请小包内存失败及
*           拆完包后发送到消息队列失败时需要将拆包时申请的小包内存释放.
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* pucHead             UINT8*          输入              待释放的数据片链的首地址
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID* cpss_com_frag_next_slice_get
(
UINT8 *pucBuf
)
{
    IPC_MSG_HDR_T *pstIpcHdr ;

    if(pucBuf==NULL)
    {
        return NULL ;
    }

    pstIpcHdr   = (IPC_MSG_HDR_T*)CPSS_TRUST_HDR_TO_IPC_HDR(pucBuf);
    
    pstIpcHdr   = pstIpcHdr->ptNext;

    if(pstIpcHdr==NULL)
    {
        return NULL;
    }

    return (void*)CPSS_IPC_HDR_TO_TRUST_HDR(pstIpcHdr);
}

/*******************************************************************************
* 函数名称: cpss_com_frag_slice_unlink
* 功    能: 拆包过程中申请小包内存失败及
*           拆完包后发送到消息队列失败时需要将拆包时申请的小包内存释放.
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* pucHead             UINT8*          输入              待释放的数据片链的首地址
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID* cpss_com_frag_slice_unlink
(
UINT8 *pucBuf
)
{
    CPSS_COM_TRUST_HEAD_T *   pstTrustHdr;
    IPC_MSG_HDR_T *pstIpcHdr;
    IPC_MSG_HDR_T *pstIpcHdrTmp;

    if(pucBuf==NULL)
    {
        return NULL ;
    }

    pstIpcHdrTmp = (IPC_MSG_HDR_T*)CPSS_TRUST_HDR_TO_IPC_HDR(pucBuf);
    
    pstIpcHdr    = pstIpcHdrTmp->ptNext ;

    /*摘除链表中的元素*/
    pstIpcHdrTmp->ptNext = NULL;

    
    if(pstIpcHdr==NULL)
    {
        return NULL;
    }

    pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)CPSS_IPC_HDR_TO_TRUST_HDR(pstIpcHdr);
    
    return (VOID*)pstTrustHdr;
}



/*******************************************************************************
* 函数名称: cpss_com_frag_slice_put
* 功    能: 得到不需要拆包的片内存
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_frag_slice_put
(
UINT8 *pucMem1,
UINT8 *pucMem2
)
{
    CPSS_COM_TRUST_HEAD_T *   pstTrustHdr;
    IPC_MSG_HDR_T *pstIpcHdr = NULL ;

    if((pucMem1==NULL)||(pucMem2==NULL))
    {
        return;
    }

    pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)pucMem1 ;

    pstIpcHdr   = (IPC_MSG_HDR_T*)CPSS_TRUST_HDR_TO_IPC_HDR(pstTrustHdr) ;

    pstIpcHdr->ptNext = (IPC_MSG_HDR_T*)CPSS_TRUST_HDR_TO_IPC_HDR(pucMem2);
    
}



/*******************************************************************************
* 函数名称: cpss_com_frag_nosplit_slice_get
* 功    能: 得到不需要拆包的片内存
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述        
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
UINT8 * cpss_com_frag_nosplit_slice_get
(
UINT8 *pucBuf, 
UINT32 ulLen
)
{
    UINT8 * pucSliceBuf;
    CPSS_COM_TRUST_HEAD_T *   pstSliceHdr;
    
    /*申请发送的内存并拷贝数据*/
    pucSliceBuf = (UINT8 *) cpss_com_mem_alloc(ulLen);
    if(pucSliceBuf==NULL)
    {
        return NULL ; 
    }
    cpss_mem_memcpy(CPSS_TRUST_HDR_TO_USR_HDR(pucSliceBuf),pucBuf,ulLen);
    
    pstSliceHdr = (CPSS_COM_TRUST_HEAD_T*)pucSliceBuf;
    pstSliceHdr->fragFlag = COM_FRAG_NOFRAG;

    cpss_com_frag_offset_set(pstSliceHdr,1) ;

    pstSliceHdr->usSliceLen = CPSS_TRUST_HDR_TO_DATAEND_LEN_GET(ulLen);
    
    return pucSliceBuf;
}


/*******************************************************************************
* 函数名称: cpss_com_frag_slice_len_get
* 功    能: 得到数据片长度
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述      
* ulSum               UINT32          输入              数据片的个数
* ulCount             UINT32          输入              数据片的序号
* ulLen               UINT32          输入              数据包的长度
* 函数返回: 
*          数据片的长度                            
* 说   明:                                        
*******************************************************************************/
UINT16 cpss_com_frag_slice_len_get
(
 UINT32 ulSum, 
 UINT32 ulCount, 
 UINT32 ulLen 
 )
{
    UINT16 usSliceLen ;
    if(ulCount<ulSum-1)
    {
        usSliceLen = COM_FRAG_FRAME_SIZE ;
    }
    else
    {      
        usSliceLen = ulLen - ulCount*COM_FRAG_FRAME_SIZE ;
    }
    return usSliceLen ;
}


/*******************************************************************************
* 函数名称: cpss_com_frag_pkg_free
* 功    能: 拆包过程中申请小包内存失败及
*           拆完包后发送到消息队列失败时需要将拆包时申请的小包内存释放.
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述 
* pucHead             UINT8*          输入              待释放的数据片链的首地址
* 函数返回: 
*          无                            
* 说   明:                                        
*******************************************************************************/
VOID cpss_com_frag_pkg_free
(
UINT8 * pucHead
)
{    
    CPSS_COM_TRUST_HEAD_T * pstTrustHdr;
    CPSS_COM_TRUST_HEAD_T * pstHdrtmp;
    
    if(pucHead==NULL)
    {
        return;
    }
    
    pstTrustHdr = (CPSS_COM_TRUST_HEAD_T *)pucHead;
    
    while(pstTrustHdr != NULL)
    {
        pstHdrtmp   = pstTrustHdr;

        pstTrustHdr = (CPSS_COM_TRUST_HEAD_T*)cpss_com_frag_next_slice_get((CHAR*)pstHdrtmp);

        if(pstTrustHdr==NULL)
        {
            cpss_com_mem_free(pstHdrtmp);                
            return ;
        }

        cpss_com_mem_free(pstHdrtmp);               
    }
}



/*******************************************************************************
* 函数名称: cpss_com_frag_pkg_split
* 功    能: 拆包函数
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* pucBuf              UINT8*          输入              需要拆包的数据
* ulSize              UINT32          输入              数据的长度
* ppucTail            UINT8**         输出              数据片链表的尾指针
* 函数返回: 
*          成功：数据片链表的首地址
*          失败：NULL                    
* 说   明: 第一片数据有ipc和com头，之后的数据片只有ipc头                                       
*******************************************************************************/
UINT8* cpss_com_frag_pkg_split
(
 UINT8 * pucBuf,
 UINT32  ulSize,
 UINT8** ppucTail
 )
{
    UINT16  usSliceNumber ;     /*拆包数目*/
    UINT16  usSliceCount ;
    UINT16  usSliceLen ;        /*拆包后每一小包的纯数据长度*/
    
    CPSS_COM_TRUST_HEAD_T *   pstSliceHdr ;
    CPSS_COM_TRUST_HEAD_T *   pstSlicePreHdr;
    CPSS_COM_TRUST_HEAD_T *   pstSliceFirst;
    
    /*计算需要拆解的包数*/
    if(ulSize==0)
    {
        usSliceNumber = 1;
    }
    else
    {
        usSliceNumber = CPSS_DIV((ulSize-1),COM_FRAG_FRAME_SIZE) +1;     
    }
    
    /*判断是否需要拆包,不需拆包则直接返回包的头指针*/
    if((ulSize < COM_FRAG_FRAME_SIZE)||(usSliceNumber == 1))
    {        
        *ppucTail = cpss_com_frag_nosplit_slice_get(pucBuf,ulSize) ;
        return *ppucTail ;
    }
    
    pstSlicePreHdr = NULL;
    pstSliceFirst  = NULL;
    
    /*分配拆包需要的内存*/
    for(usSliceCount=0;usSliceCount<usSliceNumber;usSliceCount++)
    {    
        usSliceLen = cpss_com_frag_slice_len_get(usSliceNumber , usSliceCount , ulSize);
        if(usSliceCount==0)
        {
            pstSliceHdr = cpss_com_mem_alloc(usSliceLen);    
        }
        else
        {
            pstSliceHdr = cpss_com_nodisp_mem_alloc(usSliceLen);    
        }
        if(pstSliceHdr == NULL)
        {   
            cpss_com_frag_pkg_free((UINT8*)pstSliceFirst);        
            return NULL ;
        }
        
        /*已经拆过包了，继续拆包*/
        if(pstSliceFirst != NULL)
        {
            UINT16 usTmpOffset ;
            
            cpss_mem_memcpy(CPSS_TRUST_HDR_TO_DISP_HDR(pstSliceHdr),
                pucBuf+usSliceCount*COM_FRAG_FRAME_SIZE , usSliceLen);
            
            /*将数据挂在链表上*/
            cpss_com_frag_slice_put((CHAR*)pstSlicePreHdr,(CHAR*)pstSliceHdr) ;

            usTmpOffset = usSliceCount+1 ;
            
            cpss_com_frag_offset_set(pstSliceHdr,usTmpOffset) ;

            pstSliceHdr->usSliceLen = usSliceLen + CPSS_COM_TRUST_HEAD_LEN ;                                            
            if(usSliceCount != usSliceNumber-1)
            {
                pstSliceHdr->fragFlag = COM_FRAG_MIDDLE;   /*中间包*/
            }
            else
            {              
                pstSliceHdr->fragFlag = COM_FRAG_END;      /*尾包*/
            }
        }
        else
        {                  
            pstSliceFirst = pstSliceHdr;
            cpss_mem_memcpy(CPSS_TRUST_HDR_TO_USR_HDR(pstSliceHdr), pucBuf ,usSliceLen);
            pstSliceHdr->fragFlag = COM_FRAG_BEGIN;           
            
            cpss_com_frag_offset_set(pstSliceHdr,1) ;

            pstSliceHdr->usSliceLen = CPSS_TRUST_HDR_TO_DATAEND_LEN_GET(usSliceLen);
        }
        
        pstSlicePreHdr = pstSliceHdr;
    }   
    *ppucTail = (UINT8*)pstSliceHdr ;
    return (UINT8*)pstSliceFirst;    
}

/*******************************************************************************
* 函数名称: cpss_com_frag_pkg_com
* 功    能: 将小数据片组成完整的数据包
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* pstPkg       COM_FRAG_PACKAGE_T*    输入              组包的结构指针
* pucBuf              UINT8*          输入              输入的数据片指针
* pucComPkg           UINT8**         输出              输出的数据包指针
* 函数返回: 
*          成功：CPSS_OK
*          失败：CPSS_ERROR                    
* 说   明: 如果返回的数据包指针为NULL，说明数据包还未组好。
*******************************************************************************/
INT32 cpss_com_frag_pkg_com
(
 COM_FRAG_PACKAGE_T *pstPkg,
 UINT8 *pucBuf,
 UINT8 **pucComPkg
 )
{        
    UINT32 ulDataLen;
    UINT8 *pucRet ;
    
    CPSS_COM_TRUST_HEAD_T * pstHdr;
    CPSS_COM_MSG_HEAD_T *pstNetHdr ;
    
    *pucComPkg = NULL ;
    /* 将传入的指针转化为头结构 */
    if(pucBuf == NULL)
    {        
        return CPSS_ERROR;
    }
    pstHdr = (CPSS_COM_TRUST_HEAD_T *)(pucBuf);
    pstNetHdr = (CPSS_COM_MSG_HEAD_T *)CPSS_TRUST_HDR_TO_DISP_HDR(pucBuf) ;
    
    /*判断是否需要组包,不需组包则直接返回包地址*/
    if(pstHdr->fragFlag == COM_FRAG_NOFRAG)
    {       
        *pucComPkg = pucBuf ;
        return CPSS_OK;
    }
    
    /*如果接收到的是重复包*/
    if(0==cpss_com_frag_offset_cmp(pstHdr,pstPkg->usSliceOffset))
    {
        cpss_com_mem_free(pstHdr) ;
        return CPSS_OK ;
    }
    
    /*如果重复收到最后一包数据,则将前面已经组好的数据包上交*/
    if((pstHdr->fragFlag == COM_FRAG_END)&&(pstPkg->usSliceOffset == 0))
    {
        *pucComPkg = pstPkg->pucSlice ;
        cpss_com_mem_free(pstHdr) ;
        return CPSS_OK ;
    }    
    
    /* 小包的纯数据长度 */
    ulDataLen = pstHdr->usSliceLen ;
    /* 该小包为首包 */
    if(pstHdr->fragFlag == COM_FRAG_BEGIN)
    {
        /* 申请大包的内存,初始化包结构 */
        pstPkg->pucSlice = cpss_com_mem_alloc(pstNetHdr->ulLen);
        if(pstPkg->pucSlice==NULL)
        {
            return CPSS_ERROR ;
        }
               
        cpss_mem_memcpy(pstPkg->pucSlice,pucBuf,ulDataLen);
        pstPkg->ulSliceSum += ulDataLen;        
        pstPkg->usSliceOffset  = pstPkg->usSliceOffset + 1 ;
        pstPkg->pucSliceOffset = pstPkg->pucSlice + ulDataLen ;
        cpss_com_mem_free(pucBuf);
        
        return CPSS_OK ;
    }
    /* 为中间包或尾包 */
    else
    {
        cpss_mem_memcpy(pstPkg->pucSliceOffset,CPSS_TRUST_HDR_TO_DISP_HDR(pucBuf),
                        CPSS_TRUST_HDR_LEN_SUBTRACT(ulDataLen));
        if(pstHdr->fragFlag == COM_FRAG_MIDDLE)
        {            
            pstPkg->pucSliceOffset += CPSS_TRUST_HDR_LEN_SUBTRACT(ulDataLen);
            pstPkg->ulSliceSum     += CPSS_TRUST_HDR_LEN_SUBTRACT(ulDataLen);
            pstPkg->usSliceOffset  = pstPkg->usSliceOffset + 1 ;
            
            cpss_com_mem_free(pucBuf);
            
            *pucComPkg = NULL ;
            return CPSS_OK ;        
        }
        /* 尾包 */
        else
        {
            /* 返回尾指针 */
            pucRet = pstPkg->pucSlice ;
            pstPkg->ulSliceSum = 0;
            pstPkg->usSliceOffset = 0 ;
            pstPkg->pucSliceOffset = NULL ;
            
            cpss_com_mem_free(pucBuf);
            
            *pucComPkg = pucRet ;
            return CPSS_OK;
        }                  
    }
}



/*******************************************************************************
* 函数名称: cpss_com_frag_null_set
* 功    能:
*        
* 函数类型: 
* 参    数: 
* 参数名称            类型            输入/输出         描述
* 函数返回: 
* 说   明: 
*******************************************************************************/
VOID cpss_com_frag_null_set
(
    COM_FRAG_PACKAGE_T *pstPkg
)
{
    pstPkg->pucSlice = NULL ;
}
#endif
/*================  函数实现  <结束>  ================*/

