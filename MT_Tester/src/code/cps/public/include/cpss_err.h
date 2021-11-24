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
#ifndef CPSS_ERR_H
#define CPSS_ERR_H

/********************************* 头文件包含 ********************************/

/******************************** 宏和常量定义 *******************************/
/*内存管理的错误码*/
#define CPSS_ERR_MEM_BASE                    (0x02080000)
/*无足够的系统内存*/
#define CPSS_ERR_MEM_NO_ENOUGH_SYS_MEM       (CPSS_ERR_MEM_BASE)
/*输入参数有误*/
#define CPSS_ERR_MEM_INPUT_INVALID_PARA      (CPSS_ERR_MEM_BASE+1)
/*内存块序号溢出*/
#define CPSS_ERR_MEM_BLOCKNO_OVERFLOW        (CPSS_ERR_MEM_BASE+2)
/*地址不在内存池管理范围内*/
#define CPSS_ERR_MEM_ADDR_OVERFLOW           (CPSS_ERR_MEM_BASE+3)
/*内存池中无足够的内存块*/
#define CPSS_ERR_MEM_NO_ENOUGH_BLOCK         (CPSS_ERR_MEM_BASE+4)
/*重复释放内存块*/
#define CPSS_ERR_MEM_REPEAT_RELEASE_BLOCK    (CPSS_ERR_MEM_BASE+5)
/*释放地址有误*/
#define CPSS_ERR_MEM_RELEASE_INVALID_ADDR    (CPSS_ERR_MEM_BASE+6)
/*内存操作越界*/
#define CPSS_ERR_MEM_OPERATION_BEYOND        (CPSS_ERR_MEM_BASE+7)

/*定时器管理的错误码*/
#define CPSS_ERR_TM_BASE                     (0x02050000)
/*无足够的系统内存*/
#define CPSS_ERR_TM_NO_ENOUGH_SYS_MEM	      (CPSS_ERR_TM_BASE)
/*输入参数有误*/
#define CPSS_ERR_TM_INPUT_INVALID_PARA	      (CPSS_ERR_TM_BASE+1)
/*无可用的定时器控制块*/
#define CPSS_ERR_TM_GET_TMCB_FAIL	          (CPSS_ERR_TM_BASE+2)
/*定时器不在队列中*/
#define CPSS_ERR_TM_NOT_IN_QUEUE	          (CPSS_ERR_TM_BASE+3)
/*定时器在队列中的位置有误*/
#define CPSS_ERR_TM_POSITION_ERROR	          (CPSS_ERR_TM_BASE+4)
/*纤程中无该定时器*/
#define CPSS_ERR_TM_NOT_IN_PCB	              (CPSS_ERR_TM_BASE+5)
/*定时器状态有误*/
#define CPSS_ERR_TM_STATUS_ERROR	          (CPSS_ERR_TM_BASE+6)
/*定时器不在二叉树中*/
#define CPSS_ERR_TM_NOT_IN_TREE	              (CPSS_ERR_TM_BASE+7)
/*设置辅助时钟失败*/
#define CPSS_ERR_TM_SET_AUX_CLOCK_FAIL	      (CPSS_ERR_TM_BASE+8)
/*vxWorks系统调用失败*/
#define CPSS_ERR_TM_SNTP_SYS_CALL_FAIL	      (CPSS_ERR_TM_BASE+9)
/*从rdbs读取sntp 配置信息失败*/
#define CPSS_ERR_TM_SNTP_READ_CFG_FAIL	      (CPSS_ERR_TM_BASE+10)

/*内核监控模块的错误码*/
#define CPSS_ERR_KW_BASE                     (0x02040000)
/*vxWorks系统调用失败*/
#define CPSS_ERR_KW_SYS_CALL_FAIL	         (CPSS_ERR_TM_BASE)

/*文件系统错误码*/
#define CPSS_ERR_FS_BASE                     (0x02030000)
/*文件名为空*/
#define CPSS_ERR_FS_FILENAME_NULL            (CPSS_ERR_FS_BASE)
/*文件名不合法*/
#define CPSS_ERR_FS_FILENAME_ILLEGAL         (CPSS_ERR_FS_BASE+1)
/*文件系统调用失败*/
#define CPSS_ERR_FS_SYSCALL_FAIL             (CPSS_ERR_FS_BASE+2)
/*文件打开个数超出最大允许打开个数*/
#define CPSS_ERR_FS_FILE_OPEN_OVERFLOW       (CPSS_ERR_FS_BASE+3)
/*文件接口函数入参不合法*/
#define CPSS_ERR_FS_PARA_ILLEGAL             (CPSS_ERR_FS_BASE+4)
/*文件已经存在*/
#define CPSS_ERR_FS_FILE_EXIST               (CPSS_ERR_FS_BASE+5)
/*文件句柄管理增加出错*/
#define CPSS_ERR_FS_HANDLE_ADD_FAIL          (CPSS_ERR_FS_BASE+6)
/*文件句柄管理删除出错*/
#define CPSS_ERR_FS_HANDLE_DEL_FAIL          (CPSS_ERR_FS_BASE+7)
/*文件目录句柄无效*/
#define CPSS_ERR_FS_DIR_HANDLE_INVALID       (CPSS_ERR_FS_BASE+8)
/*文件名长度大于最大长度*/
#define CPSS_ERR_FS_NAME_LEN_OVERFLOW        (CPSS_ERR_FS_BASE+9)
/*参数为空*/
#define CPSS_ERR_FS_PARA_NULL                (CPSS_ERR_FS_BASE+10)
/*文件不存在*/
#define CPSS_ERR_FS_FILE_NOT_EXIST           (CPSS_ERR_FS_BASE+11)
/*文件句柄不存在*/
#define CPSS_ERR_FS_HANDLE_NOT_EXIST         (CPSS_ERR_FS_BASE+12)
/*存在嵌套情况*/
#define CPSS_ERR_FS_RECURSION_EXIST          (CPSS_ERR_FS_BASE+13)
/*解压文件失败*/
#define CPSS_ERR_FS_UNCOMPRESS_FAIL          (CPSS_ERR_FS_BASE+14)
/*文件长度过大*/
#define CPSS_ERR_FS_SIZE_TOO_LARGE           (CPSS_ERR_FS_BASE+15)
/*文件后缀增加失败*/
#define CPSS_ERR_FS_SUFFIX_ADD_FAIL          (CPSS_ERR_FS_BASE+16)
/*拆分目录名称失败*/
#define CPSS_ERR_FS_UNLINK_NAME_FAIL         (CPSS_ERR_FS_BASE+17)
/*合成临时.tmp文件名失败*/
#define CPSS_ERR_FS_TMP_NAME_MAKE_FAIL       (CPSS_ERR_FS_BASE+18)
/*得到文件名称失败*/
#define CPSS_ERR_FS_FILENAME_GET_FAIL        (CPSS_ERR_FS_BASE+19)
/*组合文件名失败*/
#define CPSS_ERR_FS_LINK_NAME_FAIL           (CPSS_ERR_FS_BASE+20)
/*打开文件失败*/
#define CPSS_ERR_FS_OPEN_FILE_FAIL           (CPSS_ERR_FS_BASE+21)
/*内存申请失败*/
#define CPSS_ERR_FS_MALLOC_FAIL              (CPSS_ERR_FS_BASE+22)
/*服务器文件获取长度失败*/
#define CPSS_ERR_FS_SERVER_FILELEN_GET_FAIL  (CPSS_ERR_FS_BASE+23)
/**/
#define CPSS_ERR_FS_READ_LEN_NOT_ENOUGH      (CPSS_ERR_FS_BASE+24)
#define CPSS_ERR_FS_WRITE_LEN_NOT_ENOUGH     (CPSS_ERR_FS_BASE+25)

/*FTP请求链表加入节点失败*/
#define CPSS_ERR_FS_JOIN_NODE_FAIL           (CPSS_ERR_FS_BASE+26)
/*压缩文件失败*/
#define CPSS_ERR_FS_PACK_FILE_FAIL           (CPSS_ERR_FS_BASE+27)
/*FTP连接请求发送失败*/
#define CPSS_ERR_FS_CONN_REQ_SEND_FAIL       (CPSS_ERR_FS_BASE+28)
/*FTP连接响应结果失败*/
#define CPSS_ERR_FS_XFER_RSP_RESULT_ERR      (CPSS_ERR_FS_BASE+29)
/*FTP的CWD请求发送失败*/
#define CPSS_ERR_FS_CWD_REQ_SEND_FAIL        (CPSS_ERR_FS_BASE+30)
/*发送FTP命令失败*/
#define CPSS_ERR_FS_FTP_CMD_SEND_FAIL        (CPSS_ERR_FS_BASE+32)  
/*FTP命令不合法*/  
#define CPSS_ERR_FS_FTP_CMD_ILLEGAL          (CPSS_ERR_FS_BASE+33)
/*FTP传输命令响应失败*/
#define CPSS_ERR_FS_TRANS_RESULT_ERR         (CPSS_ERR_FS_BASE+34)
/*FTP用户内存写入失败*/
#define CPSS_ERR_FS_MEM_WRITE_FAIL           (CPSS_ERR_FS_BASE+35)
/*FTP过程处理失败*/
#define CPSS_ERR_FTP_PROCESS_FAIL            (CPSS_ERR_FS_BASE+36)
/*TFTP过程处理失败*/
#define CPSS_ERR_TFTP_PROCESS_FAIL           (CPSS_ERR_FS_BASE+37)
/*解压文件失败*/
#define CPSS_ERR_UNPACK_FILE_FAIL            (CPSS_ERR_FS_BASE+38)
/*TFTP连接失败*/
#define CPSS_ERR_TFTP_CONN_FAIL              (CPSS_ERR_FS_BASE+39)
/*TFTP传输失败*/
#define CPSS_ERR_TFTP_TRANS_FAIL             (CPSS_ERR_FS_BASE+40)
/*增加TFTP节点失败*/
#define CPSS_ERR_TFTP_NODE_ADD_FAIL          (CPSS_ERR_FS_BASE+41)
/*申请TFTP节点内存失败*/
#define CPSS_ERR_TFTP_MALLOC_NODE_MEM_FAIL   (CPSS_ERR_FS_BASE+42)
/*TFTP处理超时*/
#define CPSS_ERR_TFTP_TIMEOUT                (CPSS_ERR_FS_BASE+43)

/* 读文件失败*/
#define CPSS_ERR_FILE_READ_FAIL              (CPSS_ERR_FS_BASE + 44)
/* 写文件失败*/
#define CPSS_ERR_FILE_WRITE_FAIL              (CPSS_ERR_FS_BASE + 45)
/*COPY文件失败*/
#define CPSS_ERR_FILE_COPY_FAIL              (CPSS_ERR_FS_BASE + 46)
/*STAT 函数调用失败*/
#define CPSS_ERR_FILE_STAT_FAIL              (CPSS_ERR_FS_BASE + 47)
/*Seek  函数调用失败*/
#define CPSS_ERR_FILE_SEEK_FAIL              (CPSS_ERR_FS_BASE + 48)
/*创建文件失败*/
#define CPSS_ERR_FILE_CREATE_FAIL              (CPSS_ERR_FS_BASE + 49)
/* 非法文件描述符*/
#define CPSS_ERR_FS_INVALID_DESCRIPTOR   (CPSS_ERR_FS_BASE + 50)

/*通信错误码*/
#define CPSS_ERR_COM_BASE                    (0x02010000)
/*通信系统调用失败*/
#define CPSS_ERR_COM_SYSCALL_FAIL            (CPSS_ERR_COM_BASE)
/*没有找到对应的TCP链路*/
#define CPSS_ERR_COM_TCP_LINK_NOT_FIND       (CPSS_ERR_COM_BASE+1)
/*接口函数传入的参数不合法*/
#define CPSS_ERR_COM_PARA_ILLEGAL            (CPSS_ERR_COM_BASE+2)
/*路由不存在*/
#define CPSS_ERR_COM_ROUTE_NOT_EXIST         (CPSS_ERR_COM_BASE+3)
/*本板的备板不存在 */
#define CPSS_ERR_COM_BAKBRD_NOT_EXIST        (CPSS_ERR_COM_BASE+4)
/*链路不存在*/ 
#define CPSS_ERR_COM_LINK_NOT_EXIST          (CPSS_ERR_COM_BASE+5)
/*指定的驱动类型不存在*/
#define CPSS_ERR_COM_DRV_NOT_FIND            (CPSS_ERR_COM_BASE+6)
/*调用RDBS的接口函数失败*/
#define CPSS_ERR_COM_CPS__RDBS_ENTRY_FAIL         (CPSS_ERR_COM_BASE+7)
/*没有找到对应的PCI链路对象*/
#define CPSS_ERR_COM_PCI_NOT_FINDE           (CPSS_ERR_COM_BASE+8)
/*发送PCI中断失败*/
#define CPSS_ERR_COM_PCI_INTERRUPT_FAIL      (CPSS_ERR_COM_BASE+9)
/*存储的数据超过最大限制值*/
#define CPSS_ERR_COM_STORE_DATA_OVERFLOW     (CPSS_ERR_COM_BASE+10)
/*可靠传输组包失败*/
#define CPSS_ERR_COM_COMPKG_FAIL             (CPSS_ERR_COM_BASE+11)
/*不支持共享内存发送*/
#define CPSS_ERR_COM_SHARE_MEM_NOT_SUPPORT   (CPSS_ERR_COM_BASE+12)
/*构造消息报文失败*/
#define CPSS_ERR_COM_PACKET_SETUP_FAIL       (CPSS_ERR_COM_BASE+13)
/* 通信内存申请失败 */
#define CPSS_ERR_COM_MEM_ALLOC_FAIL          (CPSS_ERR_COM_BASE+14)
/* TCP发送能力超标 */
#define CPSS_ERR_COM_TCP_ABILITY_OVERFLOW    (CPSS_ERR_COM_BASE+15)
/*链路测试忙*/
#define CPSS_ERR_COM_COMM_TEST_BUSY          (CPSS_ERR_COM_BASE+16)
/*链路测试地址解析失败*/
#define CPSS_ERR_COM_COMM_TEST_ARP_FAIL      (CPSS_ERR_COM_BASE+17)

/*调试管理模块*/
#define CPSS_ERR_DBG_BASE                    (0x02020000)
/*打印级别、子系统号、模块号参数无效*/
#define CPSS_ERR_DBG_PRT_PARAM_INVA	         (CPSS_ERR_DBG_BASE)
/*打印数据字符串为空，加入缓存失败*/
#define CPSS_ERR_DBG_PRT_DATA_NULL           (CPSS_ERR_DBG_BASE+1)
/*打印数据字符串过长，加入缓存失败*/	
#define CPSS_ERR_DBG_PRT_DATA_LEN            (CPSS_ERR_DBG_BASE+2)
/*打印缓存不存在，数据加入缓存失败*/	 
#define CPSS_ERR_DBG_PRT_DATA_BUF            (CPSS_ERR_DBG_BASE+3)
/*打印数据发送失败*/	
#define CPSS_ERR_DBG_PRT_DATA_SEND	         (CPSS_ERR_DBG_BASE+4)
/*打印数据主机字节序转换为网络字节序失败*/
#define CPSS_ERR_DBG_PRT_DATA_HTON	         (CPSS_ERR_DBG_BASE+5)
/*打印数据网络字节序转换为主机字节序失败*/
#define CPSS_ERR_DBG_PRT_DATA_NTOH	         (CPSS_ERR_DBG_BASE+6)
/*打印定时设置失败*/
#define CPSS_ERR_DBG_PRT_TIMER_SET	         (CPSS_ERR_DBG_BASE+7)
/*打印定时删除失败*/
#define CPSS_ERR_DBG_PRT_TIMER_DEL	         (CPSS_ERR_DBG_BASE+8)
/*日志级别、子系统号、模块号参数无效*/
#define CPSS_ERR_DBG_LOG_PARAM_INVA	         (CPSS_ERR_DBG_BASE+9)
/*日志数据字符串为空，加入缓存失败*/
#define CPSS_ERR_DBG_LOG_DATA_NULL           (CPSS_ERR_DBG_BASE+10)
/*日志数据字符串过长，加入缓存失败*/	
#define CPSS_ERR_DBG_LOG_DATA_LEN            (CPSS_ERR_DBG_BASE+11)
/*日志缓存不存在，数据加入缓存失败*/	
#define CPSS_ERR_DBG_LOG_DATA_BUF            (CPSS_ERR_DBG_BASE+12)
/*日志数据发送失败*/	
#define CPSS_ERR_DBG_LOG_DATA_SEND	         (CPSS_ERR_DBG_BASE+13)
/*日志数据主机字节序转换为网络字节序失败*/
#define CPSS_ERR_DBG_LOG_DATA_HTON	         (CPSS_ERR_DBG_BASE+14)
/*日志数据网络字节序转换为主机字节序失败*/
#define CPSS_ERR_DBG_LOG_DATA_NTOH	         (CPSS_ERR_DBG_BASE+15)
/*日志定时设置失败*/
#define CPSS_ERR_DBG_LOG_TIMER_SET	         (CPSS_ERR_DBG_BASE+16)
/*日志定时删除失败*/
#define CPSS_ERR_DBG_LOG_TIMER_DEL	         (CPSS_ERR_DBG_BASE+17)

/*虚拟操作系统模块*/
#define CPSS_ERR_VOS_BASE                    (0x02070000)
/*信号量描述符不足*/
#define CPSS_ERR_VOS_INSUFFICIENT_SEM_DESC	    (CPSS_ERR_VOS_BASE)
/*信号量描述符无效*/
#define CPSS_ERR_VOS_INVALID_SEM_DESC	        (CPSS_ERR_VOS_BASE+1)
/*信号量类型无效*/
#define CPSS_ERR_VOS_INVALID_SEM_TYPE	        (CPSS_ERR_VOS_BASE+2)
/*任务描述符不足*/
#define CPSS_ERR_VOS_INSUFFICIENT_TASK_DESC	    (CPSS_ERR_VOS_BASE+3)
/*任务描述符无效*/
#define CPSS_ERR_VOS_INVALID_TASK_DESC	        (CPSS_ERR_VOS_BASE+4)
/*任务优先级非法*/
#define CPSS_ERR_VOS_ILLEGAL_TASK_PRIORITY	     (CPSS_ERR_VOS_BASE+5)
/*消息队列描述符不足*/
#define CPSS_ERR_VOS_INSUFFICIENT_MSG_Q_DESC	 (CPSS_ERR_VOS_BASE+6) 
/*消息队列描述符无效*/
#define CPSS_ERR_VOS_INVALID_MSG_Q_DESC	         (CPSS_ERR_VOS_BASE+7)
/*消息长度无效*/
#define CPSS_ERR_VOS_INVALID_MSG_LENGTH	         (CPSS_ERR_VOS_BASE+8)
/*消息队列不可用*/
#define CPSS_ERR_VOS_MSG_Q_UNAVAILABLE	         (CPSS_ERR_VOS_BASE+9)
/*参数没有指定*/
#define CPSS_ERR_VOS_NOT_SPECIFIED_ARGUMENTS	 (CPSS_ERR_VOS_BASE+10)

/*纤程调度模块*/
#define CPSS_ERR_VK_BASE                          (0x02060000)
/*调度器描述符不足*/
#define CPSS_ERR_VK_INSUFFICIENT_SCHED_DESC	      (CPSS_ERR_VK_BASE)
/*调度器描述符无效*/
#define CPSS_ERR_VK_INVALID_SCHED_DESC	          (CPSS_ERR_VK_BASE+1)
/*纤程类型无效*/
#define CPSS_ERR_VK_INVALID_GUID	              (CPSS_ERR_VK_BASE+2)
/*纤程实例不足*/
#define CPSS_ERR_VK_INSUFFICIENT_INSTANCE	      (CPSS_ERR_VK_BASE+3)
/*纤程实例无效*/
#define CPSS_ERR_VK_INVALID_INSTANCE	          (CPSS_ERR_VK_BASE+4)
/*纤程描述符无效*/
#define CPSS_ERR_VK_INVALID_PROC_DESC	          (CPSS_ERR_VK_BASE+5)
/*纤程优先级非法*/
#define CPSS_ERR_VOS_ILLEGAL_PROC_PRIORITY	      (CPSS_ERR_VK_BASE+6)
/*IPC消息池ID无效*/
#define CPSS_ERR_VK_INVALID_IPC_MSG_POOL_ID	      (CPSS_ERR_VK_BASE+7)
/*IPC消息不足*/
#define CPSS_ERR_VK_INSUFFICIENT_IPC_MSG	      (CPSS_ERR_VK_BASE+8)
/*参数没有指定*/
#define CPSS_ERR_VK_NOT_SPECIFIED_ARGUMENTS	      (CPSS_ERR_VK_BASE+9)

/******************************** 类型定义 ***********************************/


/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/

/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_ERR_H */
/******************************** 头文件结束 *********************************/

