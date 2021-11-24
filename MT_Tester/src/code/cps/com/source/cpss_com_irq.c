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
#include "cpss_vos_msg_q.h"
#include "cpss_com_irq.h"
#include "cpss_common.h"
#include "cpss_err.h"

/*IRQ分发任务ID*/
#ifndef CPSS_DSP_CPU
UINT32  g_ulIrqMsgQueueId ;

/*IRQ链表结构*/
CPSS_COM_IRQ_REG_T *g_pstComIrgReg = NULL;

extern VOID cpss_kw_set_task_stop() ;
extern VOID cpss_kw_set_task_run() ;

#define VOS_MSG_Q_FIFO  0x00 /* 任务按FIFO算法调度  */
#define VOS_MSG_Q_PRI   0x01 /* 任务按优先级调度 */
#define VOS_MSG_PRI_NORMAL  0 /* 优先级为普通的消息*/
#define VOS_MSG_PRI_URGENT  1  /* 优先级为紧急的消息 */

/*******************************************************************************
* 函数名称: cpss_com_irq_dest_proc_add
* 功    能: 纤程注册存储函数
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型       输入/输出         描述
* ulIrqNo     UINT32       输入                 中断号
* ulPd        UINT32       输入           纤程描述符，该纤程响应软中断
* 函数返回: 
*         无
* 说    明: 
*
*******************************************************************************/
INT32 cpss_com_irq_dest_proc_add
(
    UINT32 ulIrqNo,
    UINT32 ulPdNo
)
{
   
    CPSS_COM_IRQ_REG_T *pstPreNode, *pstTempNode;
    
    if(NULL==g_pstComIrgReg)
    {
        pstTempNode = cpss_mem_malloc(sizeof(CPSS_COM_IRQ_REG_T));
        if(NULL == pstTempNode)
        {
            return CPSS_ERROR ;
        }
        pstTempNode->ulIrqNo = ulIrqNo ;
        pstTempNode->ulPdNo  = ulPdNo ;
        pstTempNode->pstNext = NULL ;

        g_pstComIrgReg = pstTempNode ;
    }
    else
    {
        pstTempNode = g_pstComIrgReg ;
        while(pstTempNode!=NULL)
        {
            /*中断号已经存在*/
            if(pstTempNode->ulIrqNo == ulIrqNo)
            {
                return CPSS_OK ;
            }

            pstPreNode = pstTempNode ;
            pstTempNode = pstTempNode->pstNext ;
        }

        pstTempNode = cpss_mem_malloc(sizeof(CPSS_COM_IRQ_REG_T));
        if(NULL == pstTempNode)
        {
            return CPSS_ERROR ;
        }
        pstTempNode->ulIrqNo = ulIrqNo ;
        pstTempNode->ulPdNo  = ulPdNo ;
        pstTempNode->pstNext = NULL ;

        pstPreNode->pstNext = pstTempNode ;
    }
    return CPSS_OK ;
}

/*******************************************************************************
* 函数名称: cpss_com_irq_register
* 功    能: 高层应用注册纤程，由该纤程响应软中断
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型       输入/输出         描述
* ulIrqNo     UINT32       输入                 中断号
* ulPd        UINT32       输入           纤程描述符，该纤程响应软中断
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*
*******************************************************************************/
INT32 cpss_com_irq_register
(
 UINT32 ulIrqNo,
 UINT32 ulPd
 )
{
    INT32 lRet ;
    
    lRet = cpss_com_irq_dest_proc_add(ulIrqNo,ulPd);

    return lRet ;
}

/*******************************************************************************
* 函数名称: cpss_com_irq_send
* 功    能: 由驱动调用的函数，目的是向CPSS中断任务发送中断信息。
*        
* 函数类型: 
* 参    数: 
* 参数名称   类型     输入/输出         描述
* ulLen      UINT32   输入        pucBuf的字节长度，最大长度128字节
* pucBuf     UINT8*   输出        前四个字节是中断号（主机字节序），后面是消息体
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*
*******************************************************************************/
INT32 cpss_com_irq_send
(
  UINT8 *pucBuf,
  UINT32 ulLen
 )
{
    INT32 lRet ;
    
    if(0 == g_ulIrqMsgQueueId)
    {
        return CPSS_ERROR ;
    }
    
    lRet = cpss_vos_msg_q_send(g_ulIrqMsgQueueId,pucBuf,ulLen,
        NO_WAIT,VOS_MSG_PRI_URGENT);
    
    if(CPSS_OK != lRet)
    {
        return CPSS_ERROR ;
    }
    
    return (CPSS_OK);
}
/*******************************************************************************
* 函数名称: cpss_com_irq_dest_proc_get
* 功    能: 根据中断号查找纤程ID
*        
* 函数类型: 
* 参    数: 
* 参数名称        类型       输入/输出         描述
* ulIrqNo         UINT32     输入              中断号
* ulPd            UINT32     输入              纤程描述符，该纤程响应软中断
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*
*******************************************************************************/
INT32 cpss_com_irq_dest_proc_get(UINT32 ulIrqNo ,UINT32 *ulPdNo)
{
    CPSS_COM_IRQ_REG_T *pstIrqNode;

    pstIrqNode = g_pstComIrgReg ;

    if(NULL == pstIrqNode)
    {
        return CPSS_ERROR ;
    }
    
    while(NULL != pstIrqNode)
    {
        if(pstIrqNode->ulIrqNo == ulIrqNo)
        {
            *ulPdNo = pstIrqNode->ulPdNo;
            return CPSS_OK ;
        }
        pstIrqNode = pstIrqNode->pstNext;
    }

    return CPSS_ERROR ;
}

/*******************************************************************************
* 函数名称: cpss_com_irq_dispatch_task
* 功    能: 分发irq中断的任务。
*        
* 函数类型: 
* 参    数: 
* 参数名称   类型     输入/输出         描述
* 函数返回: 
*       成功：CPSS_OK;
*       失败：CPSS_ERROR；
* 说    明: 
*
*******************************************************************************/
UINT32 g_ulCpssComIrqTaskPList[8];
INT32 cpss_com_irq_task_init()
{
    UINT32 ulMsgDescId ;
    UINT8  *pucRecvBuf ;

    /*创建消息队列*/
    ulMsgDescId = cpss_vos_msg_q_create (CPSS_COM_IRQ_DISP_MQ_NAME, 
        CPSS_COM_IRQ_DISP_MQ_MSG_NUM, 
        CPSS_COM_IRQ_DISP_MQ_MSG_LEN, VOS_MSG_Q_FIFO);
    if(VOS_MSG_Q_DESC_INVALID == ulMsgDescId)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "CPSS_COM: irq dispatch mq create failed.");
        return (CPSS_ERROR);
    }
    
    g_ulIrqMsgQueueId = ulMsgDescId ;
    g_ulCpssComIrqTaskPList[0] = ulMsgDescId;
    
    /*申请接收内存*/
    pucRecvBuf = cpss_mem_malloc(CPSS_COM_IRQ_DISP_MQ_MSG_LEN) ;
    if(NULL==pucRecvBuf)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "CPSS_COM: irq malloc recv buffer failed.");
        return (CPSS_ERROR);
    }
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
#if SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST
    drv_pub_reg_msg_send(cpss_com_irq_send);
#endif
#endif
    g_ulCpssComIrqTaskPList[1] = CPSS_COM_IRQ_DISP_MQ_MSG_LEN;
    g_ulCpssComIrqTaskPList[2] = (UINT32)pucRecvBuf;
    g_ulCpssComIrqTaskPList[3] = WAIT_FOREVER;

    return (CPSS_OK);
    
}

INT32 cpss_com_irq_task_block(UINT32 *pulParamList)
{
    UINT32 ulMsgDescId = pulParamList[0];
    UINT8  *pucRecvBuf = (UINT8*)pulParamList[2];
    INT32  lRecvLen ;
    
    lRecvLen = cpss_vos_msg_q_receive(ulMsgDescId, pucRecvBuf,
            pulParamList[1], pulParamList[3]);
    
    return (lRecvLen);
}
INT32 cpss_com_irq_task_dealing(INT32 lRecvLen, UINT32 *pulParamList)
{
    UINT8  *pucRecvBuf = (UINT8*)pulParamList[2];
    UINT32 ulProcId ;
    INT32  lRet ;
    UINT32 ulMsgId;
    UINT32 usMsgLen;
    CPSS_COM_MSG_HEAD_T stDispMsgHdr ;
    
    if(CPSS_ERROR == lRecvLen)
    {
        return (CPSS_ERROR) ;
    }
    
    /*取得中断号*/
    ulProcId = *(UINT32*)pucRecvBuf ;
    ulMsgId  = *(UINT32*)(pucRecvBuf+4) ;
    usMsgLen = *(UINT16*)(pucRecvBuf+8);
    /*去掉头部10个字节*/

    /*usMsgLen = usMsgLen- CPSS_DRV_IRQ_HEARD;*/

    /*通过中断号得到对应的目的纤程地址*/
#if 0
    lRet = cpss_com_irq_dest_proc_get(ulInterNo,&ulProcId) ;
    if(CPSS_OK != lRet)
    {
        return (CPSS_ERROR) ;
    }
#endif
    cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
        "irq task send to proc(0x%x),ulMsgId(0x%x),usMsgLen(%d).",ulProcId,ulMsgId,usMsgLen);
    /*向对应的中断任务发送中断通知消息*/
    stDispMsgHdr.stDstProc.ulPd        = ulProcId ;
    stDispMsgHdr.ucPriFlag             = CPSS_COM_PRIORITY_NORMAL;
    stDispMsgHdr.ucShareFlag           = CPSS_COM_MEM_NOT_SHARE ;
    stDispMsgHdr.ulLen                 = usMsgLen ;
    stDispMsgHdr.ulMsgId               = ulMsgId ;
    stDispMsgHdr.pucBuf                = pucRecvBuf + CPSS_DRV_IRQ_HEARD ;
    
    lRet = cpss_com_send_local(&stDispMsgHdr);
    if(CPSS_OK != lRet)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
            "CPSS_COM: irq task send to proc(%x) failed.",ulProcId);
        return (CPSS_ERROR) ;
    }

	return (CPSS_OK);
}
INT32 cpss_com_irq_task_reg()
{
    INT32 lRet;
    lRet = cpss_vos_ntask_register(
              CPSS_NORMAL_TASK_TYPE_MQ,
              cpss_com_irq_task_init, 
              cpss_com_irq_task_block,
			  cpss_com_irq_task_dealing,
              g_ulCpssComIrqTaskPList,
              CPSS_COM_IRQ_TASKNAME);
    if ( lRet != CPSS_OK)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_ERROR,
                "Irq task register failed! errCode = %#x \n", lRet);
        return (CPSS_ERROR);
    }

    return (CPSS_OK);
    
}

void cpss_com_irq_dispatch_task()
{
    UINT32 ulMsgDescId ;
    UINT8  *pucRecvBuf ;
    INT32  lRecvLen ;
    UINT32 ulProcId ;
    INT32  lRet ;
    UINT32 ulMsgId;
    UINT16 usMsgLen;
    CPSS_COM_MSG_HEAD_T stDispMsgHdr ;
    
    /*创建消息队列*/
    ulMsgDescId = cpss_vos_msg_q_create (CPSS_COM_IRQ_DISP_MQ_NAME, 
        CPSS_COM_IRQ_DISP_MQ_MSG_NUM, 
        CPSS_COM_IRQ_DISP_MQ_MSG_LEN, VOS_MSG_Q_FIFO);
    if(VOS_MSG_Q_DESC_INVALID == ulMsgDescId)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "CPSS_COM: irq dispatch mq create failed.");
        return ;
    }
    
    g_ulIrqMsgQueueId = ulMsgDescId ;
    
    /*申请接收内存*/
    pucRecvBuf = cpss_mem_malloc(CPSS_COM_IRQ_DISP_MQ_MSG_LEN) ;
    if(NULL==pucRecvBuf)
    {
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_FAIL,
            "CPSS_COM: irq malloc recv buffer failed.");
        return ;
    }
#if (SWP_OS_TYPE == SWP_OS_VXWORKS)
#if SWP_CPUROLE_TYPE == SWP_CPUROLE_HOST
    drv_pub_reg_msg_send(cpss_com_irq_send);
#endif
#endif

    /*接收数据*/
    while(TRUE)
    {
        cpss_kw_set_task_stop() ;
        
        lRecvLen = cpss_vos_msg_q_receive(ulMsgDescId,pucRecvBuf,
            CPSS_COM_IRQ_DISP_MQ_MSG_LEN,WAIT_FOREVER);
        
        cpss_kw_set_task_run() ;        
        
        if(CPSS_ERROR == lRecvLen)
        {
            continue ;
        }
        
        /*取得中断号*/
        ulProcId = *(UINT32*)pucRecvBuf ;
        ulMsgId  = *(UINT32*)(pucRecvBuf+4) ;
        usMsgLen = *(UINT16*)(pucRecvBuf+8);
        /*去掉头部10个字节*/
        /*usMsgLen = usMsgLen- CPSS_DRV_IRQ_HEARD;*/
        /*通过中断号得到对应的目的纤程地址*/
#if 0
        lRet = cpss_com_irq_dest_proc_get(ulInterNo,&ulProcId) ;
        if(CPSS_OK != lRet)
        {
            continue ;
        }
#endif
        cpss_output(CPSS_MODULE_COM,CPSS_PRINT_INFO,
            "irq task send to proc(0x%x),ulMsgId(0x%x),usMsgLen(%d).",ulProcId,ulMsgId,usMsgLen);
        /*向对应的中断任务发送中断通知消息*/
        stDispMsgHdr.stDstProc.ulPd        = ulProcId ;
        stDispMsgHdr.ucPriFlag             = CPSS_COM_PRIORITY_NORMAL;
        stDispMsgHdr.ucShareFlag           = CPSS_COM_MEM_NOT_SHARE ;
        stDispMsgHdr.ulLen                 = usMsgLen ;
        stDispMsgHdr.ulMsgId               = ulMsgId ;
        stDispMsgHdr.pucBuf                = pucRecvBuf + CPSS_DRV_IRQ_HEARD ;
        
        lRet = cpss_com_send_local(&stDispMsgHdr);
        if(CPSS_OK != lRet)
        {
            cpss_output(CPSS_MODULE_COM,CPSS_PRINT_WARN,
                "CPSS_COM: irq task send to proc(%x) failed.",ulProcId);
        }
    }
}
#else
INT32 cpss_com_irq_send
(
  UINT8 *pucBuf,
  UINT32 ulLen
 )
 {
     return CPSS_OK;
 }
#endif

