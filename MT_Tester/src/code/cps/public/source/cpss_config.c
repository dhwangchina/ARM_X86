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
#include "cpss_config.h"
/******************************* 局部宏定义 ***********************************/



/******************************* 全局变量定义/初始化 **************************/

/******************************* 文件管理相关配置信息 **************************/
/*  说明 : CPSS负责对硬盘和FLASH进行初始化，按照《统一平台公共约定》文档设置初始目录
*******************************************************************************/

/* 初始硬盘目录配置表 */
CPSS_DIR_CONFIG_T g_astCpssDirConfigH[] = 
{
    "smss" ,
    "smss/temp" ,
    "verlib" ,
    "rdbs" ,
    "rdbs/work" ,
    "rdbs/omc" ,
    "rdbs/temp" ,
    "rdbs/bak1",
    "rdbs/bak2",
    "oams" ,
    "oams/log" ,
    "oams/sbbr" ,
    "oams/pm" ,
    "oams/alarm" ,
    "oams/ftm" ,
    "oams/ftm/pm" ,
    "oams/ftm/am" ,
    "oams/ftm/cellho",
    "oams/ftm/incrdata" ,
    "oams/event" ,
    "oams/cdl" ,
    "oams/temp" ,
    "oams/oprt" ,
    "oams/alarmlog" ,
    "oams/dlm" ,
    "oams/dlm/oprtlog" ,
    "oams/dlm/amdata" ,
    "oams/dlm/pmdata" ,
    "oams/dlm/baklog" ,
    "oams/cellho",
    "cpss" ,
    "cpss/zip" ,
    "cpss/temp" ,
    "tnbs" ,
    "tnbs/route" ,
    "license"
    ""
};

/* 初始FLASH目录配置表 */
CPSS_DIR_CONFIG_T g_astCpssDirConfigF[] = 
{
    "smss" ,
    "smss/version" ,
    "smss/temp" ,
    "smss/bak" ,
    "sbbr" ,
    ""
};
/* 目录配置表项个数 */
UINT32 g_ulCpssDirNumH = NUM_ENTS (g_astCpssDirConfigH);
UINT32 g_ulCpssDirNumF = NUM_ENTS (g_astCpssDirConfigF);


/******************************* 文件管理相关配置信息结束 ********************/



/******************************* 局部常数和类型定义 **************************/

/******************************* 局部函数原型声明 ****************************/

/******************************* 函数实现 *************************************/
/*******************************************************************************
* 函数名称: 
* 功    能: 
* 函数类型: 
* 参    数: 无
* 函数返回: 0
* 说    明:
*******************************************************************************/


/******************************* 源文件结束 ***********************************/
