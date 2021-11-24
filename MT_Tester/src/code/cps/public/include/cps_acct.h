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
#ifndef CPS_ACCT_H_
#define CPS_ACCT_H_

#include <cpss_public.h>

#define CPSS_MAX_USER_NAME_LENGTH 64
#define CPSS_MAX_USER_PASS_LENGTH 64

/*错误码*/
#define CPSS_DEFINE_ERR_BGN				(INT32)0x01000000
#define CPSS_ERROR_ACCOUNT_EXPIRE		CPSS_DEFINE_ERR_BGN + 1 /*帐户过期*/
#define CPSS_ERROR_ACCOUNT_LOCKED		CPSS_DEFINE_ERR_BGN + 2 /*帐户被锁定*/
#define CPSS_ERROR_NAMEPWD_SAME			CPSS_DEFINE_ERR_BGN + 3 /*帐户名密码相同*/
#define CPSS_ERROR_PWD_TOOSHORT			CPSS_DEFINE_ERR_BGN + 4 /*密码太短*/
#define CPSS_ERROR_PWD_TOOSIMPLE		CPSS_DEFINE_ERR_BGN + 5 /*密码太简单*/
#define CPSS_ERROR_USER_NOEXIST			CPSS_DEFINE_ERR_BGN + 6 /*用户不存在*/
#define CPSS_ERROR_PWD_INCORRECT		CPSS_DEFINE_ERR_BGN + 7 /*密码不正确*/
#define CPSS_ERROR_PWD_OLD				CPSS_DEFINE_ERR_BGN + 8 /*与旧密码相同*/
#define CPSS_ERROR_MAX_USER				CPSS_DEFINE_ERR_BGN + 9 /*达到最大用户*/
#define CPSS_ERROR_USER_EXIST			CPSS_DEFINE_ERR_BGN + 10 /*用户已存在*/
#define CPSS_ERROR_USER_NOLOGIN		CPSS_DEFINE_ERR_BGN + 11 /*用户不能远程登录*/
#define CPSS_ERROR_PARAM				CPSS_DEFINE_ERR_BGN + 20 /*错误的参数*/

#define CPSS_ERROR_UNKNOWN				CPSS_DEFINE_ERR_BGN + 40 /*未知错误*/

/*用户帐户信息*/
typedef struct CPS_OM_USER_INFO
{
	UINT8 chUserName[CPSS_MAX_USER_NAME_LENGTH];/* 用户名 */
	UINT8 chUserPass[CPSS_MAX_USER_PASS_LENGTH];/* 用户密码 只在添加用户时使用*/
	UINT8 byUserGrade;/*用户级别*/
	UINT8 byEnable;/*是否可以远程登录1-可以，2-不可以*/
	UINT8 ucLocked;/*账号锁定，0-不锁定，1-锁定*/
	UINT8 ucRev;
} CPS_OM_USER_INFO_T;

/*用户帐户操作请求*/
typedef struct
{
	UINT32 ucOpType;/*1-Add 2-update 3-del*/
	CPS_OM_USER_INFO_T stUsrInfo;
} CPS_OM_USEROPT_REQ_T;

/*全局密码信息*/
typedef struct CPS_OM_PASSWORD_ATTR
{
	UINT8 ucLeastPasswordLenth;/*口令最小长度*/
	UINT8 ucIsStrongPwd;/*口令是否必须为强口令，1-是 ，2- 不是*/
	UINT16 usPwdLife;/*口令生存期，单位：天*/
	UINT8 ucAuthFailureTimes;/*认证失败次数*/
	UINT8 ucCheckOldPassTimes;/*旧密码检查次数*/
} CPS_OM_PASSWORD_ATTR_T;

/*全局密码操作请求*/
typedef struct
{
	UINT32 ucOpType;/*1-Add 2-update 3-del*/
	CPS_OM_PASSWORD_ATTR_T stPwdInfo;
} CPS_OM_PWDOPT_REQ_T;

/*用户登录鉴定*/
INT32 cpss_om_user_login_check(UINT8* pucUser, UINT8* pucPass);
/*用户密码修改*/
INT32 cpss_om_user_passwd(UINT8* pucUser, UINT8* pucOldPass, UINT8* pucNewPass);
/*用户帐户操作*/
INT32 cpss_om_user_opt(CPS_OM_USEROPT_REQ_T* pstOptReq);
/*全局密码规则操作*/
INT32 cpss_om_pass_opt(CPS_OM_PWDOPT_REQ_T* pstOptReq);
INT32 cps_om_user_set_shell_tmout(UINT8 ucTmOut);

#endif /* CPSS_USER_H_ */
