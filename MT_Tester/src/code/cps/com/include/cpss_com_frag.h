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
#ifndef CPSS_COM_FRAG_H
#define CPSS_COM_FRAG_H

/********************************* 头文件包含 ********************************/
#include "cpss_public.h"


/******************************** 宏和常量定义 *******************************/
#define COM_FRAG_FRAME_SIZE           1280          /*将数据拆包的最大包长*/


#define COM_FRAG_NOFRAG           0x0  /*不拆包*/
#define COM_FRAG_BEGIN            0x1  /*拆包的首帧*/
#define COM_FRAG_MIDDLE           0x3  /*拆包的中间帧*/
#define COM_FRAG_END              0x5  /*拆包的结束帧*/    

/******************************** 类型定义 ***********************************/

/*包管理结构*/
typedef struct tagCOM_FRAG_PACKAGE_T
{
    UINT8 * pucSlice;          /* 包指针 */
    UINT8 * pucSliceOffset ;   /* 包指针的偏移量 */
    UINT32  ulSliceSum;        /* 包总数 */
    UINT16  usSliceOffset;     /* 小包序号 */  
}COM_FRAG_PACKAGE_T ;




/******************************** 全局变量声明 *******************************/

/******************************** 外部函数原形声明 ***************************/


/*拆包函数*/
extern UINT8 * cpss_com_frag_pkg_split
(
UINT8 * pucBuf,
UINT32 ulSize,
UINT8** ppucTail
) ;

/*组包函数*/
extern INT32 cpss_com_frag_pkg_com
(
COM_FRAG_PACKAGE_T *pstPkg, 
UINT8 *pucBuf,
UINT8 **pucComPkg
) ;

/*释放包函数*/
extern VOID cpss_com_frag_pkg_free
(
UINT8 * pucHead
) ;
 
extern VOID cpss_com_frag_slice_put
(
UINT8 *pucMem1,
UINT8 *pucMem2
) ;

extern VOID* cpss_com_frag_slice_unlink
(
UINT8 *pucBuf
) ;

extern VOID* cpss_com_frag_next_slice_get
(
UINT8 *pucBuf
) ;

extern VOID cpss_com_frag_null_set
(
    COM_FRAG_PACKAGE_T *pstPkg
) ;
/******************************** 头文件保护结尾 *****************************/
#endif /* CPSS_COM_FRAG_H */
/******************************** 头文件结束 *********************************/
