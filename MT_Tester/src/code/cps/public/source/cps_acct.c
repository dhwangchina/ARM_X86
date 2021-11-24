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
#include "cps_acct.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//#include <security/pam_appl.h>
//#include <security/pam_misc.h>
#include <shadow.h>
#include <ctype.h>
//#include <crack.h>
#include <pwd.h>
#include <time.h>
#include <crypt.h>
#include <sys/stat.h>
#include <unistd.h>

#define SH_TMPFILE              "/etc/nshadow"

//#undef D
//#define D(x, ...) cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, x)

#define MAX_USER_NUM 10
#define MAX_OPASS_NUM 10
#define MAX_PASS_DAY 90
#define LOGIN_DEF "/etc/login.defs"
#define LOGIN_DEF_BAK "/etc/login.defs.bak"
#define OPW_TMPFILE "/etc/toldpass"
#define OLD_PASSWORDS_FILE "/etc/oldpasswd"
#define PASSWD_TMPFILE "/etc/tpasswd"
#define PASSWD_FILE "/etc/passwd"
#define PAM_SERVER "system-auth"

typedef struct CPS_ACCT_USER_ERROR
{
	UINT8 aucUser[CPSS_MAX_USER_NAME_LENGTH];
	UINT8 ucErrorTime;
	UINT8 ucErrorMaxTime;
} CPS_ACCT_USER_ERROR_T;

typedef struct USER_OPASSWD
{
	UINT8 ucInUse;
	UINT8 ucPassNum;
	UINT8 aucUser[CPSS_MAX_USER_NAME_LENGTH];
	UINT8* paucPass[MAX_OPASS_NUM];
} USER_OPASSWD_T;

INT32 cpss_om_user_login_check(UINT8* pucUser, UINT8* pucPass);
INT32 cpss_om_user_passwd(UINT8* pucUser, UINT8* pucOldPass, UINT8* pucNewPass);
INT32 cpss_om_user_opt(CPS_OM_USEROPT_REQ_T* pstOptReq);
INT32 cpss_om_pass_opt(CPS_OM_PWDOPT_REQ_T* pstOptReq);

VOID cpss_om_set_current_user(UINT8* pucUserName);
INT32 cps_om_acct_error_deal(UINT8* pucUser);

INT32 cpss_om_user_opt_add(CPS_OM_USER_INFO_T* pstUserInfo);
INT32 cpss_om_user_opt_update(CPS_OM_USER_INFO_T* pstUserInfo);
INT32 cpss_om_user_opt_del(CPS_OM_USER_INFO_T* pstUserInfo);
INT32 cpss_om_user_opt_add_init(CPS_OM_USER_INFO_T* pstUserInfo);

INT32 cpss_om_pass_opt_add(CPS_OM_PASSWORD_ATTR_T* pstPwdInfo);
INT32 cpss_om_pass_opt_update(CPS_OM_PASSWORD_ATTR_T* pstPwdInfo);
INT32 cpss_om_pass_opt_del(CPS_OM_PASSWORD_ATTR_T* pstPwdInfo);

INT32 cpss_old_pass_user_add(UINT8*user, UINT8* pass);
INT32 cpss_old_pass_user_check(UINT8*user, UINT8* pass);
INT32 cpss_old_pass_user_pass_add(UINT8*user, UINT8* pass);
INT32 cpss_old_pass_user_del(UINT8*user);

INT32 cpss_om_pass_minlen_check(UINT8* pucPass);
INT32 cpss_om_pass_simple_check(UINT8* pucPass);
INT32 cpss_om_pass_cracklib_check(UINT8* pucPass);

INT32 cpss_om_pass_check_expire(UINT8* pucUser);
INT32 cpss_om_pass_check(UINT8* pucUser, UINT8* pucPass);
INT32 cpss_om_pass_update(UINT8 *pucUser, UINT8 *pucPass);

INT32 cps_om_save_old_password(UINT8* pucUser, UINT8* pucPass, INT32 lTimes);
INT32 cps_om_check_old_password(UINT8* pucUser, UINT8* pucPass);

INT32 cpss_om_user_loginable_set(UINT8 *pucUser, INT32 lState);
INT32 cpss_om_user_loginable_check(UINT8* pucUser);

#if 0
extern INT32 check_conv(INT32 num_msg, const struct pam_message **msg,
		struct pam_response **resp, void *app_data);

struct pam_conv conv =
{	check_conv, NULL};
#endif

CPS_ACCT_USER_ERROR_T g_stCpsAcctError;
UINT8 g_aucCurrentUser[CPSS_MAX_USER_NAME_LENGTH];
UINT8 g_aucPassWord[CPSS_MAX_USER_PASS_LENGTH];
USER_OPASSWD_T g_stUserOpasswd[MAX_USER_NUM];
CPS_OM_PASSWORD_ATTR_T g_PasswdAttr;

/*设置当前操作的用户名*/
VOID cpss_om_set_current_user(UINT8* pucUserName)
{
	if (NULL == pucUserName)
	{
		return;
	}
	/*将当前网页操作的用户名记录到全局变量中*/
	memset(g_aucCurrentUser, 0, sizeof(g_aucCurrentUser));
	strcpy((INT8*) g_aucCurrentUser, (INT8*) pucUserName);
}

/*用户登录鉴定*/
INT32 cpss_om_user_login_check(UINT8* pucUser, UINT8* pucPass)
{
	//	pam_handle_t* pamh;
	INT32 lRetval;
	struct spwd *stSpwd;
	struct passwd *stPwd;

	/*参数有效性检查*/
	/*用户名,密码,用户名,密码大于最大长度*/
	if ((NULL == pucUser) || (NULL == pucPass))
	{
		return CPSS_ERROR_PARAM;
	}

	if ((CPSS_MAX_USER_NAME_LENGTH < strlen((INT8*) pucUser))
			|| (CPSS_MAX_USER_PASS_LENGTH < strlen((INT8*) pucPass)))
	{
		return CPSS_ERROR_PARAM;
	}

	/*确定当前操作的用户名*/
	cpss_om_set_current_user(pucUser);

	/*检查用户是否存在于shadow*/
	stSpwd = getspnam((INT8*) pucUser);
	if (NULL == stSpwd)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_login_check login: no user = %s\n", pucUser);
		return CPSS_ERROR_USER_NOEXIST;
	}

	/*检查用户是否存在于passwd*/
	stPwd = getpwnam((INT8*) pucUser);
	if (NULL == stPwd)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_login_check login: no user = %s\n", pucUser);
		return CPSS_ERROR_USER_NOEXIST;
	}

	/*检查用户是否能远程登录*/
	lRetval = cpss_om_user_loginable_check(pucUser);
	if(CPSS_OK != lRetval)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "cpss_om_user_login_check login: user= %s can not login!\n", pucUser);
		return lRetval;
	}


#ifndef SWP_LINUX
	/*检查帐户是否属于AC可登录范围*/
	/*600为管理员组，700为用户组*/
	if (!((600 == stPwd->pw_gid) || (700 == stPwd->pw_gid)))
	{
		//		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "stPwd->stPwd->pw_gid = %d\n", stPwd->pw_gid);
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_login_check invalid user! gid = %d\n",
				stPwd->pw_gid);
		return CPSS_ERROR;
	}
#endif

	/*检查帐户是否被锁定*/
	if (0 == strncmp("!", stSpwd->sp_pwdp, 1))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_login_check login: user= %s locked!\n", pucUser);
		return CPSS_ERROR_ACCOUNT_LOCKED;
	}

	/*检查用户是否过期*/
	/*现阶段板子上时间不对，不检查用户过期*/
#if 0
	lRetval = cpss_om_pass_check_expire(pucUser);
	if(CPSS_OK != lRetval)
	{
		return lRetval;
	}
#endif

	/*用户密码检查*/
	lRetval = cpss_om_pass_check(pucUser, pucPass);
	if (CPSS_OK != lRetval)
	{
		/*用户密码出错处理*/
		lRetval = cps_om_acct_error_deal(pucUser);
		return lRetval;
	}
#if 0
	/*开始pam认证*/
	pam_start(PAM_SERVER, (INT8*) pucUser, &conv, &pamh);

	/*将密码传入conv用户鉴定用户*/
	memset(g_aucPassWord, 0, sizeof(g_aucPassWord));
	memcpy(g_aucPassWord, pucPass, strlen((INT8*) pucPass));

	/*用户认证*/
	lRetval = pam_authenticate(pamh, 0);
	printf("pam_authenticate lRetval = %d\n", lRetval);
	if (PAM_SUCCESS != lRetval)
	{
		/*密码失败处理*/
		/*如果失败，记录错误次数，与全局连续失败次数对比*/
		lRetval = cps_om_acct_error_deal(pucUser);

		pam_end(pamh, 0);
		return lRetval;
	}

	/*帐号鉴定*/
	lRetval = pam_acct_mgmt(pamh, 0);
	printf("pam_acct_mgmt lRetval = %d\n", lRetval);
	if (PAM_SUCCESS != lRetval)
	{
		pam_end(pamh, 0);
		return CPSS_ERROR_ACCOUNT_EXPIRE;
	}

	/*结束pam认证*/
	pam_end(pamh, 0);
#endif

	/*用户登录成功，清除失败记录*/
	if (0 == strcmp(&g_stCpsAcctError.aucUser, pucUser))
	{
		g_stCpsAcctError.ucErrorTime = 0;
	}

	return CPSS_OK;
}

/*修改用户密码*/
INT32 cpss_om_user_passwd(UINT8* pucUser, UINT8* pucOldPass, UINT8* pucNewPass)
{
	//	pam_handle_t *pamh;
	INT32 lRetval;

	/*判断参数有效性*/
	/*用户名，新旧密码是否为空，是否大于最大长度*/
	if ((NULL == pucUser) || (NULL == pucOldPass) || (NULL == pucNewPass))
	{
		return CPSS_ERROR_PARAM;
	}

	if ((CPSS_MAX_USER_NAME_LENGTH < strlen((INT8*) pucUser))
			|| (CPSS_MAX_USER_PASS_LENGTH < strlen((INT8*) pucOldPass))
			|| (CPSS_MAX_USER_PASS_LENGTH < strlen((INT8*) pucNewPass)))
	{
		return CPSS_ERROR_PARAM;
	}

	/*用户名密码相同*/
	if (0 == strcmp((INT8*) pucUser, (INT8*) pucNewPass))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_passwd user= [%s] same as passwd !\n", pucUser);
		return CPSS_ERROR_NAMEPWD_SAME;
	}

	/*检查是否为旧密码*/
	//	lRetval = cpss_old_pass_user_check(pucUser, pucNewPass);
	//	if (CPSS_OK != lRetval)
	//	{
	//		return lRetval;
	//	}

	/*检查是否为旧密码*/
	lRetval = cps_om_check_old_password(pucUser, pucNewPass);
	if (CPSS_OK != lRetval)
	{
		return lRetval;
	}

	/*检查长度*/
	lRetval = cpss_om_pass_minlen_check(pucNewPass);
	if (CPSS_OK != lRetval)
	{
		return lRetval;
	}

	/*检查是否太简单*/
	lRetval = cpss_om_pass_simple_check(pucNewPass);
	if (CPSS_OK != lRetval)
	{
		return lRetval;
	}

	//	/*是否查字典*/
	//	lRetval = cpss_om_pass_cracklib_check(pucNewPass);
	//	if(-1 == lRetval)
	//	{
	//		return CPSS_ERROR_PWD_TOOSIMPLE;
	//	}

	/*检查当前操作用户的密码*/
	lRetval = cpss_om_pass_check(g_aucCurrentUser, pucOldPass);
	if (lRetval != CPSS_OK)
	{
		return lRetval;
	}

	/*添加用户的密码*/
	lRetval = cpss_om_pass_update(pucUser, pucNewPass);
	if (lRetval != CPSS_OK)
	{
		return lRetval;
	}

#if 0
	/*pam认证开始*/
	pam_start(PAM_SERVER, (INT8*) g_aucCurrentUser, &conv, &pamh);

	/*将OldPass为当前登录的用户,传入conv中,用于鉴定当前用户*/
	memset(g_aucPassWord, 0, sizeof(g_aucPassWord));
	memcpy(g_aucPassWord, pucOldPass, strlen((INT8*) pucOldPass));

	/*当前登录用户认证*/
	lRetval = pam_authenticate(pamh, 0);
	printf("pam_authenticate lRetval = %d\n", lRetval);
	if (PAM_SUCCESS != lRetval)
	{
		pam_end(pamh, 0);
		return CPSS_ERROR_PWD_INCORRECT;
	}

	//	/*帐号鉴定*/
	//	lRetval = pam_acct_mgmt(pamh, 0);
	//	printf("pam_acct_mgmt lRetval = %d\n", lRetval);
	//	/*如果当前用户失败,返回*/
	//	if (PAM_SUCCESS != lRetval)
	//	{
	//		pam_end(pamh, 0);
	//		return lRetval;
	//	}

	/*将NewPass传入conv中,用于修改指定用户密码*/
	memset(g_aucPassWord, 0, sizeof(g_aucPassWord));
	memcpy(g_aucPassWord, pucNewPass, strlen((INT8*) pucNewPass));

	/*将用户改为欲修改的用户*/
	pam_set_item(pamh, PAM_USER, pucUser);

	/*更改用户的密码*/
	lRetval = pam_chauthtok(pamh, 0);
	printf("pam_chauthtok lRetval = %d\n", lRetval);
	if (PAM_SUCCESS != lRetval)
	{
		pam_end(pamh, 0);
		return CPSS_ERROR_PWD_INCORRECT;
	}
	pam_end(pamh, 0);
#endif

	/*修改成功后添加至用户的旧密码中*/
	//	lRetval = cpss_old_pass_user_pass_add(pucUser, pucNewPass);
	//	if (CPSS_OK != lRetval)
	//	{
	//		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "error in cpss_old_pass_user_pass_add\n");
	//		return lRetval;
	//	}

	//	lRetval = cps_om_save_old_password(pucUser, pucNewPass, g_PasswdAttr.ucCheckOldPassTimes);
	//	if (CPSS_OK != lRetval)
	//	{
	//		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "error in cps_om_save_old_password\n");
	//		return lRetval;
	//	}

	return CPSS_OK;
}

INT32 cpss_om_user_opt(CPS_OM_USEROPT_REQ_T* pstOptReq)
{
	INT32 lRet;

	/*判断参数是否正确*/
	if (NULL == pstOptReq)
	{
		return CPSS_ERROR_PARAM;
	}

	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
			"in cpss_om_user_opt !, op = %d\n", pstOptReq->ucOpType);

	/*用户操作区分*/
	switch (pstOptReq->ucOpType)
	{
	case 1:
		/*用户添加操作*/
		lRet = cpss_om_user_opt_add(&pstOptReq->stUsrInfo);
		break;
	case 2:
		/*用户属性修改操作*/
		lRet = cpss_om_user_opt_update(&pstOptReq->stUsrInfo);
		break;
	case 3:
		/*用户删除操作*/
		lRet = cpss_om_user_opt_del(&pstOptReq->stUsrInfo);
		break;
	case 4:
		/*初始化添加用户*/
		lRet = cpss_om_user_opt_add_init(&pstOptReq->stUsrInfo);
		break;
	default:
		/*参数错误*/
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"in cpss_om_user_opt error type = %d\n", pstOptReq->ucOpType);
		lRet = CPSS_ERROR_PARAM;
		break;
	}
	/*返回结果*/
	return lRet;
}

/*用户添加操作*/
INT32 cpss_om_user_opt_add(CPS_OM_USER_INFO_T* pstUserInfo)
{
	INT32 lRetval;
	//	UINT32 ulGrpId;
	INT8* pcGrp;
	//	pam_handle_t *pamh;
	UINT8 aucCmd[256];

	if (NULL == pstUserInfo)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_opt_add pstUserInfo is NULL!\n");
		return CPSS_ERROR_PARAM;
	}

	if ((NULL == pstUserInfo->chUserName) || (NULL == pstUserInfo->chUserPass))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_opt_add user or pass is NULL\n");
		return CPSS_ERROR_PARAM;
	}

	if ((CPSS_MAX_USER_NAME_LENGTH < strlen((INT8*) pstUserInfo->chUserName))
			|| (CPSS_MAX_USER_PASS_LENGTH < strlen(
					(INT8*) pstUserInfo->chUserPass)))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_opt_add strlen user or pass error!\n");
		return CPSS_ERROR_PARAM;
	}

	if((2 < pstUserInfo->byEnable) || (1 > pstUserInfo->byEnable))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_opt_add user loginEnable error!\n");
		return CPSS_ERROR_PARAM;
	}

	/*用户名密码相同*/
	if (0 == strcmp((INT8*) pstUserInfo->chUserName,
			(INT8*) pstUserInfo->chUserPass))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_opt_add user same as passwd!\n");
		return CPSS_ERROR_NAMEPWD_SAME;
	}

	//	/*检查是否为旧密码*/
	//	lRetval = cpss_old_pass_user_check(pstUserInfo->chUserName,
	//			pstUserInfo->chUserPass);
	//	if (-1 == lRetval)
	//	{
	//		return CPSS_ERROR_PWD_OLD;
	//	}

	/*检查长度*/
	lRetval = cpss_om_pass_minlen_check(pstUserInfo->chUserPass);
	if (CPSS_OK != lRetval)
	{
		return lRetval;
	}

	/*检查是否太简单*/
	lRetval = cpss_om_pass_simple_check(pstUserInfo->chUserPass);
	if (CPSS_OK != lRetval)
	{
		return lRetval;
	}

	/*确定用户所属组*/
	switch (pstUserInfo->byUserGrade)
	{
	/*管理员*/
	case 0:
		//		ulGrpId = 600;
		pcGrp = "admin";
		break;
		/*普通用户*/
	case 1:
		//		ulGrpId = 700;
		pcGrp = "user";
		break;
		/*参数错误*/
	default:
		return CPSS_ERROR_PARAM;
		break;
	}

	memset(aucCmd, 0, sizeof(aucCmd));

	//	sprintf((INT8*) aucCmd, "adduser -D -H -g %d %s", ulGrpId,
	//			pstUserInfo->chUserName);

#ifndef SWP_LINUX
	sprintf((INT8*) aucCmd, "adduser -D -H -G %s %s", pcGrp,
			pstUserInfo->chUserName);
#else
	sprintf((INT8*) aucCmd, "useradd -M -g %s %s", pcGrp,
			pstUserInfo->chUserName);
#endif

	lRetval = cps_system((INT8*) aucCmd);
	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
			"system() lRet adduser = %d\n", lRetval);
	if (lRetval != 0)
	{
		return CPSS_ERROR_USER_EXIST;
	}

	lRetval = cpss_om_pass_update(pstUserInfo->chUserName,
			pstUserInfo->chUserPass);
	if (CPSS_OK != lRetval)
	{
		return lRetval;
	}

#if 0
	/*pam流程开始*/
	pam_start(PAM_SERVER, (INT8*) pstUserInfo->chUserName, &conv, &pamh);

	/*将密码传入conv用于设定新密码*/
	memset(g_aucPassWord, 0, sizeof(g_aucPassWord));
	memcpy(g_aucPassWord, pstUserInfo->chUserPass, strlen(
					(INT8*) pstUserInfo->chUserPass));

	/*添加用户密码*/
	lRetval = pam_chauthtok(pamh, 0);
	/*如果不成功，删除已添加用户，返回失败*/
	if (PAM_SUCCESS != lRetval)
	{
		cpss_om_user_opt_del(pstUserInfo);
		pam_end(pamh, 0);
		return CPSS_ERROR_PARAM;
	}

	/*pam结束*/
	pam_end(pamh, 0);
#endif

	/*将用户信息加入oldpass中*/
	//	lRetval = cpss_old_pass_user_add(pstUserInfo->chUserName,
	//			pstUserInfo->chUserPass);
	//	if (CPSS_OK != lRetval)
	//	{
	//		return lRetval;
	//	}

	/*确定用户是否能远程登录*/
	cpss_om_user_loginable_set(pstUserInfo->chUserName, pstUserInfo->byEnable);

	memset(aucCmd, 0, sizeof(aucCmd));

	/*确定用户锁定状态*/
	switch (pstUserInfo->ucLocked)
	{
	case 0:
		sprintf((INT8*) aucCmd, "passwd -u %s",
				(UINT8*) pstUserInfo->chUserName);
		lRetval = cps_system((INT8*) aucCmd);
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
				"unlock user %s!\n", pstUserInfo->chUserName);
		break;
	case 1:
		sprintf((INT8*) aucCmd, "passwd -l %s",
				(UINT8*) pstUserInfo->chUserName);
		lRetval = cps_system((INT8*) aucCmd);
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
				"lock user s!\n", pstUserInfo->chUserName);
		break;
	default:
		return CPSS_ERROR_PARAM;
		break;
	}

	if (0 != lRetval)
	{
		return CPSS_ERROR_PARAM;
	}

	return CPSS_OK;
}

INT32 cpss_om_user_opt_update(CPS_OM_USER_INFO_T* pstUserInfo)
{
	INT32 lRet;
	//	UINT32 ulGrpId;
	INT8* pcGrpId;
	UINT8 aucCmd[256];

	if (NULL == pstUserInfo)
	{
		return CPSS_ERROR_PARAM;
	}

	if ((NULL == pstUserInfo->chUserName) || (CPSS_MAX_USER_NAME_LENGTH
			< strlen((INT8*) pstUserInfo->chUserName)))
	{
		return CPSS_ERROR_PARAM;
	}

	if((2 < pstUserInfo->byEnable) || (1 > pstUserInfo->byEnable))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_opt_add user loginEnable error!\n");
		return CPSS_ERROR_PARAM;
	}

	/*确定用户是否能远程登录*/
	cpss_om_user_loginable_set(pstUserInfo->chUserName, pstUserInfo->byEnable);

	/*确定用户所属组*/
	switch (pstUserInfo->byUserGrade)
	{
	/*管理员*/
	case 0:
		//		ulGrpId = 600;
		pcGrpId = "admin";
		break;
		/*普通用户*/
	case 1:
		//		ulGrpId = 700;
		pcGrpId = "user";
		break;
		/*参数错误*/
	default:
		return CPSS_ERROR_PARAM;
		break;
	}

	memset(aucCmd, 0, sizeof(aucCmd));
	sprintf((INT8*) aucCmd, "usermod -G %s %s", pcGrpId,
			pstUserInfo->chUserName);
	//	lRet = system((INT8*) aucCmd);

	memset(aucCmd, 0, sizeof(aucCmd));

	/*确定用户锁定状态*/
	switch (pstUserInfo->ucLocked)
	{
	case 0:
		sprintf((INT8*) aucCmd, "passwd -u %s", pstUserInfo->chUserName);
		lRet = cps_system((INT8*) aucCmd);
		break;
	case 1:
		sprintf((INT8*) aucCmd, "passwd -l %s", pstUserInfo->chUserName);
		lRet = cps_system((INT8*) aucCmd);
		break;
	default:
		return CPSS_ERROR_PARAM;
		break;
	}

	return CPSS_OK;
}

INT32 cpss_om_user_opt_del(CPS_OM_USER_INFO_T* pstUserInfo)
{
	INT32 lRet;
	UINT8 aucCmd[256];

	/*判断参数正确*/
	if (NULL == pstUserInfo)
	{
		return CPSS_ERROR_PARAM;
	}

	if ((NULL == pstUserInfo->chUserName) || (CPSS_MAX_USER_NAME_LENGTH
			< strlen((INT8*) pstUserInfo->chUserName)))
	{
		return CPSS_ERROR_PARAM;
	}

	memset(aucCmd, 0, sizeof(aucCmd));

#ifndef SWP_LINUX
	sprintf((INT8*) aucCmd, "deluser %s", pstUserInfo->chUserName);
#else
	sprintf((INT8*) aucCmd, "userdel %s", pstUserInfo->chUserName);
#endif
	lRet = cps_system((INT8*) aucCmd);
	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
			"system() lRet deluser = %d\n", lRet);

	if(0 == strcmp(pstUserInfo->chUserName, "user"))
	{
#ifndef SWP_LINUX
		cps_system("addgroup -g 700 user");
#else
		cps_system("groupadd -f -g 700 user");
#endif
	}

	if(0 == strcmp(pstUserInfo->chUserName, "admin"))
	{
#ifndef SWP_LINUX
		cps_system("addgroup -g 600 admin");
#else
		cps_system("groupadd -f -g 600 admin");
#endif
	}

	//	cpss_old_pass_user_del(pstUserInfo->chUserName);

	return lRet;
}

INT32 cpss_om_user_opt_add_init(CPS_OM_USER_INFO_T* pstUserInfo)
{
	INT32 lRetval;
	INT8* pcGrp;
	UINT8 aucCmd[256];

	if (NULL == pstUserInfo)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_opt_add pstUserInfo is NULL!\n");
		return CPSS_ERROR_PARAM;
	}

	if ((NULL == pstUserInfo->chUserName) || (NULL == pstUserInfo->chUserPass))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_opt_add user or pass is NULL\n");
		return CPSS_ERROR_PARAM;
	}

	if ((CPSS_MAX_USER_NAME_LENGTH < strlen((INT8*) pstUserInfo->chUserName))
			|| (CPSS_MAX_USER_PASS_LENGTH < strlen(
					(INT8*) pstUserInfo->chUserPass)))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_opt_add strlen user or pass error!\n");
		return CPSS_ERROR_PARAM;
	}

	if((2 < pstUserInfo->byEnable) || (1 > pstUserInfo->byEnable))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_om_user_opt_add user loginEnable error!\n");
		return CPSS_ERROR_PARAM;
	}

	/*确定用户所属组*/
	switch (pstUserInfo->byUserGrade)
	{
	/*管理员*/
	case 0:
		pcGrp = "admin";
		break;
		/*普通用户*/
	case 1:
		pcGrp = "user";
		break;
		/*参数错误*/
	default:
		return CPSS_ERROR_PARAM;
		break;
	}

	memset(aucCmd, 0, sizeof(aucCmd));

#ifndef SWP_LINUX
	sprintf((INT8*) aucCmd, "adduser -D -H -G %s %s", pcGrp,
			pstUserInfo->chUserName);
#else

	sprintf((INT8*) aucCmd, "useradd -M -g %s %s", pcGrp,
			pstUserInfo->chUserName);
#endif

	lRetval = cps_system((INT8*) aucCmd);
	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
			"system() lRet adduser = %d\n", lRetval);

	lRetval = cpss_om_pass_update(pstUserInfo->chUserName,
			pstUserInfo->chUserPass);
	if (CPSS_OK != lRetval)
	{
		return lRetval;
	}

	/*确定用户是否能远程登录*/
	cpss_om_user_loginable_set(pstUserInfo->chUserName, pstUserInfo->byEnable);

	memset(aucCmd, 0, sizeof(aucCmd));

	/*确定用户锁定状态*/
	switch (pstUserInfo->ucLocked)
	{
	case 0:
		sprintf((INT8*) aucCmd, "passwd -u %s",
				(UINT8*) pstUserInfo->chUserName);
		lRetval = cps_system((INT8*) aucCmd);
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
				"unlock user %s!\n", pstUserInfo->chUserName);
		break;
	case 1:
		sprintf((INT8*) aucCmd, "passwd -l %s",
				(UINT8*) pstUserInfo->chUserName);
		lRetval = cps_system((INT8*) aucCmd);
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
				"lock user s!\n", pstUserInfo->chUserName);
		break;
	default:
		return CPSS_ERROR_PARAM;
		break;
	}

	if (0 != lRetval)
	{
		return CPSS_ERROR_PARAM;
	}

	return CPSS_OK;
}

INT32 cpss_om_pass_opt(CPS_OM_PWDOPT_REQ_T* pstOptReq)
{
	INT32 lRet;

	if (NULL == pstOptReq)
	{
		return CPSS_ERROR_PARAM;
	}

	switch (pstOptReq->ucOpType)
	{
	case 1:
		lRet = cpss_om_pass_opt_add(&pstOptReq->stPwdInfo);
		break;
	case 2:
		lRet = cpss_om_pass_opt_update(&pstOptReq->stPwdInfo);
		break;
	case 3:
		lRet = cpss_om_pass_opt_del(&pstOptReq->stPwdInfo);
		break;
	default:
		return CPSS_ERROR_PARAM;
		break;
	}
	return CPSS_OK;
}

INT32 cpss_om_pass_opt_add(CPS_OM_PASSWORD_ATTR_T* pstPwdInfo)
{
	//    INT32 lRet;
	//	UINT8 aucRule[256];

	/*打开配置规则文件*/
	/*清空规则*/

	if (NULL == pstPwdInfo)
	{
		return CPSS_ERROR_PARAM;
	}

	if ((0 >= pstPwdInfo->ucLeastPasswordLenth) || (CPSS_MAX_USER_PASS_LENGTH
			< pstPwdInfo->ucLeastPasswordLenth) || (2
			< pstPwdInfo->ucIsStrongPwd) || (0
			>= pstPwdInfo->ucAuthFailureTimes) || (128
			< pstPwdInfo->ucAuthFailureTimes) || (10
			< pstPwdInfo->ucCheckOldPassTimes))
	{
		return CPSS_ERROR_PARAM;
	}

	//	memset(aucRule, 0, sizeof(aucRule));

	/*添加密码最小长度记录到全局变量中*/
	g_PasswdAttr.ucLeastPasswordLenth = pstPwdInfo->ucLeastPasswordLenth;

	/*密码强度检查开关记录到全局变量中*/
	g_PasswdAttr.ucIsStrongPwd = pstPwdInfo->ucIsStrongPwd;

	/*旧密码检查次数记录到全局变量中*/
	g_PasswdAttr.ucCheckOldPassTimes = pstPwdInfo->ucCheckOldPassTimes;

	/*密码生存期记录到全局变量中*/
	g_PasswdAttr.usPwdLife = pstPwdInfo->usPwdLife;

	/*连续认证失败次数记录到全局变量*/
	g_stCpsAcctError.ucErrorMaxTime = pstPwdInfo->ucAuthFailureTimes;

	//	sprintf((INT8*) aucRule, "password required pam_cracklib.so minlen＝%d ",
	//			pstPwdInfo->ucLeastPasswordLenth);

	//	switch (pstPwdInfo->ucIsStrongPwd)
	//	{
	//	case 0:
	//		break;
	//	case 1:
	//		sprintf((INT8*) aucRule + strlen((INT8*) aucRule),
	//				"dcredit=1 dcredit=1 ucredit=1 lcredit=1");
	//		break;
	//	default:
	//		return CPSS_ERROR;
	//		break;
	//	}

	/*将组织好的内容写入配置文件*/

	/*修改默认添加用户文件，设置生存期*/

	return CPSS_OK;
}

INT32 cpss_om_pass_opt_update(CPS_OM_PASSWORD_ATTR_T* pstPwdInfo)
{
	if (NULL == pstPwdInfo)
	{
		return CPSS_ERROR_PARAM;
	}

	if ((0 >= pstPwdInfo->ucLeastPasswordLenth) || (CPSS_MAX_USER_PASS_LENGTH
			< pstPwdInfo->ucLeastPasswordLenth) || (2
			< pstPwdInfo->ucIsStrongPwd) || (0
			>= pstPwdInfo->ucAuthFailureTimes) || (128
			< pstPwdInfo->ucAuthFailureTimes) || (10
			< pstPwdInfo->ucCheckOldPassTimes))
	{
		return CPSS_ERROR_PARAM;
	}

	/*添加密码最小长度记录到全局变量中*/
	g_PasswdAttr.ucLeastPasswordLenth = pstPwdInfo->ucLeastPasswordLenth;

	/*密码强度检查开关记录到全局变量中*/
	g_PasswdAttr.ucIsStrongPwd = pstPwdInfo->ucIsStrongPwd;

	/*旧密码检查次数记录到全局变量中*/
	g_PasswdAttr.ucCheckOldPassTimes = pstPwdInfo->ucCheckOldPassTimes;

	/*密码生存期记录到全局变量中*/
	g_PasswdAttr.usPwdLife = pstPwdInfo->usPwdLife;

	/*连续认证失败次数记录到全局变量*/
	g_stCpsAcctError.ucErrorMaxTime = pstPwdInfo->ucAuthFailureTimes;

	return CPSS_OK;
}

INT32 cpss_om_pass_opt_del(CPS_OM_PASSWORD_ATTR_T* pstPwdInfo)
{
	/*打开配置规则文件*/
	/*清空规则*/
	return CPSS_OK;
}

#if 0
INT32 check_conv(INT32 num_msg, const struct pam_message **msg,
		struct pam_response **resp, VOID *app_data)
{
	const struct pam_message *m = *msg;
	struct pam_response *r;
	INT32 i;
	INT8 *ct_passwd;

	printf("check_conv->num_msg = %d\n", num_msg);

	if ((num_msg <= 0) || (num_msg >= PAM_MAX_NUM_MSG))
	{
		fprintf(stderr, "Invalid number of messages\n");
		*resp = NULL;
		return (PAM_CONV_ERR);
	}

	if (NULL == (*resp = r = calloc(num_msg, sizeof(struct pam_response))))
	{
		return (PAM_BUF_ERR);
	}

	for (i = 0; i < num_msg; i++)
	{
		switch (m->msg_style)
		{
			case PAM_PROMPT_ECHO_OFF:
			printf("check_conv->PAM_PROMPT_ECHO_OFF\n");
			//            ct_passwd = getpass("Enter password: ");
			ct_passwd = (INT8*) g_aucPassWord;
			r->resp = strdup(ct_passwd);
			m++;
			r++;
			break;
			case PAM_PROMPT_ECHO_ON:
			printf("check_conv->PAM_PROMPT_ECHO_ON\n");
			if (m->msg)
			{
				fputs(m->msg, stdout);
			}
			r->resp = NULL;
			m++;
			r++;
			break;
			case PAM_ERROR_MSG:
			printf("check_conv->PAM_ERROR_MSG\n");
			if (m->msg)
			{
				fprintf(stderr, "%s\n", m->msg);
			}
			m++;
			r++;
			break;
			case PAM_TEXT_INFO:
			printf("check_conv->PAM_TEXT_INFO\n");
			if (m->msg)
			{
				printf("%s\n", m->msg);
			}
			m++;
			r++;
			break;
		}
		return (PAM_SUCCESS);
	}
	return (PAM_SUCCESS);
}
#endif

/*用户密码出错处理*/
/*用于判断用户连续错误*/
INT32 cps_om_acct_error_deal(UINT8* pucUser)
{
	UINT8 aucCmd[256];
	/*参数是否正确*/
	if ((NULL == pucUser) || (CPSS_MAX_USER_NAME_LENGTH < strlen(
			(INT8*) pucUser)))
	{
		return CPSS_ERROR_PARAM;
	}

	//	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "last user = %s\n", g_stCpsAcctError.aucUser);
	//	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "now user = %s\n", pucUser);
	//	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "max error = %u\n", g_stCpsAcctError.ucErrorMaxTime);

	/*判断当前用户与上次用户是否相同*/
	if (0 == strncmp(&g_stCpsAcctError.aucUser, pucUser, CPSS_MAX_USER_NAME_LENGTH))
	{
		/*相同，错误数+1*/
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"now error user = %s\n", pucUser);
		g_stCpsAcctError.ucErrorTime++;
	}
	else
	{
		/*不同，重新记录错误数*/
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"user %s login error time = 1\n", pucUser);
		g_stCpsAcctError.ucErrorTime = 1;
	}

	/*重新记录当前错误的用户名*/
	memset(&g_stCpsAcctError.aucUser, 0, sizeof(g_stCpsAcctError.aucUser));
	strncpy(&g_stCpsAcctError.aucUser, pucUser, CPSS_MAX_USER_NAME_LENGTH);

	/*判断用户连续失败次数大于等于最大失败次数*/
	if (g_stCpsAcctError.ucErrorMaxTime <= g_stCpsAcctError.ucErrorTime)
	{
		/*如果超过最大错误数*/
		/*锁定该用户帐户*/
		memset(aucCmd, 0, 256);
		sprintf((INT8*) aucCmd, "passwd -l %s", pucUser);
		/*调用系统函数锁定用户*/
		cps_system((INT8*) aucCmd);

		/*失败数清空*/
		g_stCpsAcctError.ucErrorTime = 0;

		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"user %s acct locked!\n", pucUser);

		/*返回用户帐户被锁定*/
		return CPSS_ERROR_ACCOUNT_LOCKED;
	}

	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
			"user %s passwd incorret!\n", pucUser);

	/*返回用户密码错误*/
	return CPSS_ERROR_PWD_INCORRECT;
}

INT32 cpss_old_pass_user_add(UINT8* pucUser, UINT8* pucPass)
{
	INT32 lUserloop;

	if ((NULL == pucUser) || (NULL == pucPass))
	{
		return CPSS_ERROR_PARAM;
	}

	if ((0 == strlen((INT8*) pucUser)) || (0 == strlen((INT8*) pucPass)))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_old_pass_user_add para error!\n");
		return CPSS_ERROR_PARAM;
	}

	/*查看全局opass表*/
	for (lUserloop = 0; lUserloop < MAX_USER_NUM; lUserloop++)
	{
		/*是否未用*/
		if (0 == g_stUserOpasswd[lUserloop].ucInUse)
		{
			/*清空当前内容*/
			memset(&g_stUserOpasswd[lUserloop], 0, sizeof(USER_OPASSWD_T));
			/*填入用户信息*/
			strcpy((INT8*) g_stUserOpasswd[lUserloop].aucUser, (INT8*) pucUser);
			g_stUserOpasswd[lUserloop].paucPass[0] = malloc(
					sizeof(MAX_OPASS_NUM));
			strcpy((INT8*) g_stUserOpasswd[lUserloop].paucPass[0],
					(INT8*) pucPass);

			g_stUserOpasswd[lUserloop].ucPassNum++;
			g_stUserOpasswd[lUserloop].ucInUse = 1;

			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
					"cpss_old_pass_user_add new user new pass!\n");
			return CPSS_OK;
		}
	}

	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
			"cpss_old_pass_user_add Max user!\n");
	return CPSS_ERROR_MAX_USER;
}

INT32 cpss_old_pass_user_check(UINT8* pucUser, UINT8* pucPass)
{
	INT32 lUserloop;
	INT32 lPassloop;

	if ((NULL == pucUser) || (NULL == pucPass))
	{
		return CPSS_ERROR_PARAM;

	}

	if ((0 == strlen((INT8*) pucUser)) || (0 == strlen((INT8*) pucPass)))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"cpss_old_pass_user_check para error!\n");
		return CPSS_ERROR_PARAM;
	}

	/*查看全局opass表*/
	for (lUserloop = 0; lUserloop < MAX_USER_NUM; lUserloop++)
	{
		/*是否已用*/
		if (1 == g_stUserOpasswd[lUserloop].ucInUse)
		{
			/*用户名是否相同*/
			if (0 == strcmp((INT8*) g_stUserOpasswd[lUserloop].aucUser,
					(INT8*) pucUser))
			{
				/*检查密码*/
				for (lPassloop = 0; lPassloop
						< ((g_PasswdAttr.ucCheckOldPassTimes
								< g_stUserOpasswd[lUserloop].ucPassNum) ? g_PasswdAttr.ucCheckOldPassTimes
								: g_stUserOpasswd[lUserloop].ucPassNum); lPassloop++)
				{
					if (0
							== strcmp(
									(INT8*) g_stUserOpasswd[lUserloop].paucPass[lPassloop],
									(INT8*) pucPass))
					{
						cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN,
								CPSS_PRINT_INFO, "old user old pass!\n");
						return CPSS_ERROR_PWD_OLD;
					}
				}
				cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
						"old user new pass!\n");
				return CPSS_OK;
			}
		}
	}

	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
			"user no find in old pass check\n");
	return CPSS_ERROR_USER_NOEXIST;
}

INT32 cpss_old_pass_user_pass_add(UINT8* pucUser, UINT8* pucPass)
{
	INT32 lUserloop;
	INT32 lNewpassloop;

	if ((NULL == pucUser) || (NULL == pucPass)
			|| (0 == strlen((INT8*) pucUser)) || (0 == strlen((INT8*) pucPass)))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"para error!\n");
		return CPSS_ERROR_PARAM;
	}

	/*查看全局opass表*/
	for (lUserloop = 0; lUserloop < MAX_USER_NUM; lUserloop++)
	{
		/*是否已用*/
		if (1 == g_stUserOpasswd[lUserloop].ucInUse)
		{
			/*用户名是否相同*/
			if (0 == strcmp((INT8*) g_stUserOpasswd[lUserloop].aucUser,
					(INT8*) pucUser))
			{
				/*如果为新密码（在原密码中找不到）*/
				/*如果密码区满，则释放最后一块的内存*/
				if (MAX_OPASS_NUM == g_stUserOpasswd[lUserloop].ucPassNum)
				{
					free(g_stUserOpasswd[lUserloop].paucPass[MAX_OPASS_NUM - 1]);
				}
				/*将新密码插入密码组头中*/
				/*将原密码后移1位*/
				for (lNewpassloop = MAX_OPASS_NUM - 1; lNewpassloop >= 0; lNewpassloop--)
				{
					g_stUserOpasswd[lUserloop].paucPass[lNewpassloop + 1]
							= g_stUserOpasswd[lUserloop].paucPass[lNewpassloop];
				}
				/*密码区首位为新密码*/
				g_stUserOpasswd[lUserloop].paucPass[0] = malloc(
						CPSS_MAX_USER_PASS_LENGTH);
				strcpy((INT8*) g_stUserOpasswd[lUserloop].paucPass[0],
						(INT8*) pucPass);
				if (MAX_OPASS_NUM != g_stUserOpasswd[lUserloop].ucPassNum)
				{
					g_stUserOpasswd[lUserloop].ucPassNum++;
				}
				cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
						"old user new pass!\n");
				return CPSS_OK;
			}
		}
	}

	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
			"user no find in cpss_old_pass_user_pass_add()\n");
	return CPSS_ERROR_USER_NOEXIST;
}

INT32 cpss_old_pass_user_del(UINT8* pucUser)
{
	INT32 lUserloop;
	INT32 lPassloop;

	if ((NULL == pucUser) || (0 == strlen((INT8*) pucUser)))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"para error!\n");
		return -1;
	}
	/*查找用户*/
	/*查看全局opass表*/
	for (lUserloop = 0; lUserloop < MAX_USER_NUM; lUserloop++)
	{
		/*用户名是否相同*/
		if (0 == strcmp((INT8*) g_stUserOpasswd[lUserloop].aucUser,
				(INT8*) pucUser))
		{
			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
					"pass num = %d\n", g_stUserOpasswd[lUserloop].ucPassNum);
			for (lPassloop = 0; lPassloop
					< g_stUserOpasswd[lUserloop].ucPassNum; lPassloop++)
			{
				free(g_stUserOpasswd[lUserloop].paucPass[lPassloop]);
			}
			memset(&g_stUserOpasswd[lUserloop], 0, sizeof(USER_OPASSWD_T));
			return CPSS_OK;
		}
	}
	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
			"no user in cpss_old_pass_user_del()!\n");
	return CPSS_ERROR_USER_NOEXIST;
}

/*密码长度检查*/
INT32 cpss_om_pass_minlen_check(UINT8* pucPass)
{
	if (NULL == pucPass)
	{
		return CPSS_ERROR;
	}

	if (strlen((INT8*) pucPass) < g_PasswdAttr.ucLeastPasswordLenth)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"too short!\n");
		return CPSS_ERROR_PWD_TOOSHORT;
	}
	else
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
				"minlin check ok!\n");
		return CPSS_OK;
	}
}

INT32 cpss_om_pass_set_max_day(UINT16 ulMaxDay)
{
	FILE* pfLogin;
	FILE* pfLoginBak;
	UINT8 aucBuf[256];
	UINT8 aucPut[256];

	if ((0 == MAX_PASS_DAY) || (ulMaxDay > MAX_PASS_DAY))
	{
		return CPSS_ERROR_PARAM;
	}

	pfLogin = fopen(LOGIN_DEF, "r");
	if (NULL == pfLogin)
	{
		return CPSS_ERROR;
	}

	pfLoginBak = fopen(LOGIN_DEF_BAK, "w");
	if (NULL == pfLoginBak)
	{
		fclose(pfLogin);
		return CPSS_ERROR;
	}

	memset(aucPut, 0, sizeof(aucPut));
	sprintf((INT8*) aucPut, "PASS_MAX_DAYS\t%d\n", ulMaxDay);

	while (fgets((INT8*) aucBuf, sizeof(aucBuf), pfLogin))
	{
		if (0 == strncmp((INT8*) aucBuf, "PASS_MAX_DAYS", strlen(
				"PASS_MAX_DAYS")))
		{
			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
					"find PASS_MAX_DAYS in cpss_om_pass_set_max_day()\n");
			fputs((INT8*) aucPut, pfLoginBak);
		}
		else
		{
			fputs((INT8*) aucBuf, pfLoginBak);
		}
	}

	fclose(pfLogin);
	fclose(pfLoginBak);

	unlink(LOGIN_DEF);
	rename(LOGIN_DEF_BAK, LOGIN_DEF);

	return CPSS_OK;
}

/*检查密码是否太简单*/
INT32 cpss_om_pass_simple_check(UINT8* pucPass)
{
	INT32 digits = 0;
	INT32 uppers = 0;
	INT32 lowers = 0;
	INT32 others = 0;
	INT32 i;

	if (NULL == pucPass)
	{
		return CPSS_ERROR;
	}

	/*如果配置为不检查用户密码强度*/
	if (2 == g_PasswdAttr.ucIsStrongPwd)
	{
		return CPSS_OK;
	}

	for (i = 0; pucPass[i]; i++)
	{
		if (isdigit(pucPass[i]))
		{
			digits = 1;
		}
		else if (isupper(pucPass[i]))
		{
			uppers = 1;
		}
		else if (islower(pucPass[i]))
		{
			lowers = 1;
		}
		else
		{
			others = 1;
		}
	}

	/*是否有大小写字母特殊字符数字之中的3类*/
	if ((digits + uppers + lowers + others) < 3)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"too simple!\n");
		return CPSS_ERROR_PWD_TOOSIMPLE;
	}
	else
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
				"check ok! in simple check\n");
		return CPSS_OK;
	}
}

#if 0
INT32 cpss_om_pass_cracklib_check(UINT8* pucPass)
{
	if (NULL == FascistCheck((INT8*) pucPass, NULL))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "good pass!\n");
		return CPSS_OK;
	}
	else
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "bad pass!\n");
		return -1;
	}
}
#endif

/*添加用户密码*/
INT32 cpss_om_pass_update(UINT8 *pucUser, UINT8 *pucPass)
{
	struct spwd *spwdent = NULL, *stmpent = NULL;
	struct stat st;
	FILE *pwfile, *opwfile;
	INT32 err = 1;
	INT32 oldmask;
	UINT8 aucNewPass[100];

	if ((NULL == pucUser) || (NULL == pucPass))
	{
		return CPSS_ERROR;
	}

	/*检查用户是否存在*/
	spwdent = getspnam((INT8*) pucUser);
	if (spwdent == NULL)
	{
		return CPSS_ERROR_USER_NOEXIST;
	}
	oldmask = umask(077);

	strcpy((INT8*) aucNewPass, crypt((INT8*) pucPass, "$1$"));

	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
			"oldpass = %s\n", spwdent->sp_pwdp);
	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
			"newpass = %s\n", aucNewPass);

	pwfile = fopen(SH_TMPFILE, "w");
	umask(oldmask);
	if (pwfile == NULL)
	{
		err = 1;
		goto done;
	}

	opwfile = fopen("/etc/shadow", "r");
	if (opwfile == NULL)
	{
		fclose(pwfile);
		err = 1;
		goto done;
	}

	if (fstat(fileno(opwfile), &st) == -1)
	{
		fclose(opwfile);
		fclose(pwfile);
		err = 1;
		goto done;
	}

	if (fchown(fileno(pwfile), st.st_uid, st.st_gid) == -1)
	{
		fclose(opwfile);
		fclose(pwfile);
		err = 1;
		goto done;
	}
	if (fchmod(fileno(pwfile), st.st_mode) == -1)
	{
		fclose(opwfile);
		fclose(pwfile);
		err = 1;
		goto done;
	}

	stmpent = fgetspent(opwfile);
	while (stmpent)
	{

		if (!strcmp(stmpent->sp_namp, (INT8*) pucUser))
		{
			stmpent->sp_pwdp = (INT8*) aucNewPass;
			stmpent->sp_lstchg = time(NULL) / (60 * 60 * 24);
			err = 0;
			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
					"Set password %s for %s\n", stmpent->sp_pwdp, pucUser);
			cps_om_save_old_password(pucUser, (UINT8*) stmpent->sp_pwdp,
					g_PasswdAttr.ucCheckOldPassTimes);
		}

		if (putspent(stmpent, pwfile))
		{
			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
					("error writing entry to shadow file: %m\n"));
			err = 1;
			break;
		}

		stmpent = fgetspent(opwfile);
	}
	fclose(opwfile);

	if (fclose(pwfile))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"error writing entries to shadow file: %m\n");
		err = 1;
	}

	done: if (!err)
	{
		if (!rename(SH_TMPFILE, "/etc/shadow"))
			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
					"password changed for %s\n", pucUser);
		else
			err = 1;
	}

	if (!err)
	{
		return CPSS_OK;
	}
	else
	{
		unlink(SH_TMPFILE);
		return CPSS_ERROR;
	}
}

/*用户密码检查*/
INT32 cpss_om_pass_check(UINT8* pucUser, UINT8* pucPass)
{
	struct spwd* stspw;
	/*获得用户的信息*/
	stspw = getspnam((INT8*) pucUser);
	if (NULL == stspw)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"no user! in cpss_om_pass_check()\n");
		return CPSS_ERROR_USER_NOEXIST;
	}

	/*检查用户的密码是否正确*/
	if (0 == (strcmp(stspw->sp_pwdp, crypt((INT8*) pucPass, stspw->sp_pwdp))))
	{
		//		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "pass_check user = %s\n", pucUser);
		//		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "check ok!\n");
		return CPSS_OK;
	}
	else
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"check user error! in cpss_om_pass_check()\n");
		return CPSS_ERROR_PWD_INCORRECT;
	}

	return CPSS_OK;
}

/*检查用户的密码是否过期*/
INT32 cpss_om_pass_check_expire(UINT8* pucUser)
{
	long int nowtime;
	struct spwd* stspw;

	/*获得当前用户的信息*/
	stspw = getspnam((INT8*) pucUser);
	if (NULL == stspw)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"no user! in cpss_om_pass_check_expire\n");
		return CPSS_ERROR_USER_NOEXIST;
	}

	/*获得当前的系统时间*/
	nowtime = (long int) time(NULL) / (60 * 60 * 24);

	//	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "nowtime = %ld\n", nowtime);
	//	cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "lasttime = %ld\n", stspw->sp_lstchg);

	/*判断用户上次更改密码时间和当前时间的差是否过期*/
	if (stspw->sp_lstchg < nowtime - g_PasswdAttr.usPwdLife)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,
				"user expire\n");
		return CPSS_ERROR_ACCOUNT_EXPIRE;
	}

	return CPSS_OK;
}

UINT8* cpss_crypt_md5_wrapper(UINT8* pucSrc)
{
	UINT8 aucBuf[16];
	UINT8* pucRt = aucBuf;

	memset(aucBuf, 0, 16);
	strcpy((INT8*) aucBuf, "$1$");

	crypt((INT8*) aucBuf, (INT8*) aucBuf);

	memset(aucBuf + 8, 0, 8);

	pucRt = (UINT8*) crypt((INT8*) pucSrc, (INT8*) aucBuf);

	return pucRt;
}

INT32 cps_om_save_old_password(UINT8* pucUser, UINT8* pucPass, INT32 lTimes)
{
	UINT8 aucBuf[16384];
	UINT8 aucnbuf[16384];
	UINT8 *s_luser, *s_uid, *s_npas, *s_pas, *pass;
	INT32 npas;
	FILE *pwfile, *opwfile;
	INT32 err = 0;
	INT32 oldmask;
	INT32 found = 0;
	struct passwd *pwd = NULL;
	struct stat st;

	if (lTimes < 0)
	{
		return CPSS_OK;
	}

	if (pucPass == NULL)
	{
		return CPSS_OK;
	}

	oldmask = umask(077);

	pwfile = fopen(OPW_TMPFILE, "w");
	umask(oldmask);
	if (pwfile == NULL)
	{
		err = 1;
		goto done;
	}

	opwfile = fopen(OLD_PASSWORDS_FILE, "r");
	if (opwfile == NULL)
	{
		fclose(pwfile);
		err = 1;
		goto done;
	}

	if (fstat(fileno(opwfile), &st) == -1)
	{
		fclose(opwfile);
		fclose(pwfile);
		err = 1;
		goto done;
	}

	if (fchown(fileno(pwfile), st.st_uid, st.st_gid) == -1)
	{
		fclose(opwfile);
		fclose(pwfile);
		err = 1;
		goto done;
	}
	if (fchmod(fileno(pwfile), st.st_mode) == -1)
	{
		fclose(opwfile);
		fclose(pwfile);
		err = 1;
		goto done;
	}

	while (fgets((INT8*) aucBuf, 16380, opwfile))
	{
		if (!strncmp((INT8*) aucBuf, (INT8*) pucUser, strlen((INT8*) pucUser)))
		{
			char *sptr = NULL;
			found = 1;
			if (lTimes == 0)
				continue;
			aucBuf[strlen((INT8*) aucBuf) - 1] = '\0';
			s_luser = (UINT8*) strtok_r((INT8*) aucBuf, ":", &sptr);
			if (0 != strcmp(s_luser, (INT8*) pucUser))
			{
				continue;
			}
			s_uid = (UINT8*) strtok_r(NULL, ":", &sptr);
			s_npas = (UINT8*) strtok_r(NULL, ":", &sptr);
			s_pas = (UINT8*) strtok_r(NULL, ":", &sptr);
			npas = strtol((INT8*) s_npas, NULL, 10) + 1;
			while (npas > lTimes)
			{
				s_pas = (UINT8*) strpbrk((INT8*) s_pas, ",");
				if (s_pas != NULL)
					s_pas++;
				npas--;
			}
			//			pass = cpss_crypt_md5_wrapper(pucPass);
			pass = pucPass;
			if (s_pas == NULL)
				snprintf((INT8*) aucnbuf, sizeof(aucnbuf), "%s:%s:%d:%s\n",
						s_luser, s_uid, npas, pass);
			else
				snprintf((INT8*) aucnbuf, sizeof(aucnbuf), "%s:%s:%d:%s,%s\n",
						s_luser, s_uid, npas, s_pas, pass);
			if (fputs((INT8*) aucnbuf, pwfile) < 0)
			{
				err = 1;
				break;
			}
		}
		else if (fputs((INT8*) aucBuf, pwfile) < 0)
		{
			err = 1;
			break;
		}
	}
	fclose(opwfile);

	if (!found)
	{
		pwd = getpwnam((INT8*) pucUser);
		if (pwd == NULL)
		{
			err = 1;
		}
		else
		{
			pass = cpss_crypt_md5_wrapper(pucPass);
			snprintf((INT8*) aucnbuf, sizeof(aucnbuf), "%s:%lu:1:%s\n",
					pucUser, (unsigned long) pwd->pw_uid, pass);
			if (fputs((INT8*) aucnbuf, pwfile) < 0)
			{
				err = 1;
			}
		}
	}

	if (fclose(pwfile))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				("error writing entries to old passwords file: %m\n"));
		err = 1;
	}

	done: if (!err)
	{
		if (rename(OPW_TMPFILE, OLD_PASSWORDS_FILE))
			err = 1;
	}
	if (!err)
	{
		return CPSS_OK;
	}
	else
	{
		unlink(OPW_TMPFILE);
		return CPSS_ERROR;
	}
}

/*检查用户是否为旧密码*/
INT32 cps_om_check_old_password(UINT8* pucUser, UINT8* pucPass)
{
	UINT8 buf[16384];
	UINT8 *s_luser, *s_uid, *s_npas, *s_pas;
	INT32 retval = CPSS_OK;
	FILE *opwfile;

	if ((NULL == pucUser) || (NULL == pucPass))
	{
		return CPSS_ERROR;
	}

	/*打开本地存储的旧密码文件*/
	opwfile = fopen(OLD_PASSWORDS_FILE, "r");
	if (opwfile == NULL)
	{
		return CPSS_ERROR;
	}

	/*从旧密码文件中获取用户信息*/
	while (fgets((INT8*) buf, 16380, opwfile))
	{
		/*比较用户名*/
		if (!strncmp((INT8*) buf, (INT8*) pucUser, strlen((INT8*) pucUser)))
		{
			char *sptr;
			buf[strlen((INT8*) buf) - 1] = '\0';
			s_luser = (UINT8*) strtok_r((INT8*) buf, ":,", &sptr);
			if (0 != strcmp(s_luser, (INT8*) pucUser))
			{
				continue;
			}
			s_uid = (UINT8*) strtok_r(NULL, ":,", &sptr);
			s_npas = (UINT8*) strtok_r(NULL, ":,", &sptr);
			s_pas = (UINT8*) strtok_r(NULL, ":,", &sptr);
			while (s_pas != NULL)
			{
				char *md5pass = crypt((INT8*) pucPass, (INT8*) s_pas);
				if (!strcmp((INT8*) md5pass, (INT8*) s_pas))
				{
					cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN,
							CPSS_PRINT_INFO, "checked now pass is old pass!\n");
					retval = CPSS_ERROR_PWD_OLD;
					break;
				}
				s_pas = (UINT8*) strtok_r(NULL, ":,", &sptr);
			}
			break;
		}
	}
	fclose(opwfile);

	return retval;
}

INT32 cpss_om_user_loginable_set(UINT8 *pucUser, INT32 lState)
{
	struct passwd *tmpent = NULL;
	struct stat st;
	FILE *pwfile, *opwfile;
	int err = 1;
	int oldmask;

	if (NULL == pucUser)
	{
		return CPSS_ERROR;
	}

	if((2 < lState) || (1 > lState))
	{
		return CPSS_ERROR;
	}

	oldmask = umask(077);

	pwfile = fopen(PASSWD_TMPFILE, "w");
	umask(oldmask);
	if (pwfile == NULL)
	{
		err = 1;
		goto done;
	}

	opwfile = fopen(PASSWD_FILE, "r");
	if (opwfile == NULL)
	{
		fclose(pwfile);
		err = 1;
		goto done;
	}

	if (fstat(fileno(opwfile), &st) == -1)
	{
		fclose(opwfile);
		fclose(pwfile);
		err = 1;
		goto done;
	}

	if (fchown(fileno(pwfile), st.st_uid, st.st_gid) == -1)
	{
		fclose(opwfile);
		fclose(pwfile);
		err = 1;
		goto done;
	}
	if (fchmod(fileno(pwfile), st.st_mode) == -1)
	{
		fclose(opwfile);
		fclose(pwfile);
		err = 1;
		goto done;
	}

	tmpent = fgetpwent(opwfile);
	while (tmpent)
	{
		if (!strcmp(tmpent->pw_name, (INT8*)pucUser))
		{
			if(1 == lState)
			{
				tmpent->pw_shell = "/bin/sh";
			}
			else if(2 == lState)
			{
				tmpent->pw_shell = "/sbin/nologin";
			}
			else
			{
				tmpent->pw_shell = "/bin/sh";
			}
			err = 0;
		}
		if (putpwent(tmpent, pwfile))
		{
			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,"error writing entry to password file: %m");
			err = 1;
			break;
		}
		tmpent = fgetpwent(opwfile);
	}
	fclose(opwfile);

	if (fclose(pwfile))
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "error writing entries to password file: %m");
		err = 1;
	}

	done: if (!err)
	{
		if (!rename(PASSWD_TMPFILE, PASSWD_FILE))
			cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_INFO,"loginmod changed for %s", pucUser);
		else
			err = 1;
	}

	if (!err)
	{
		return CPSS_OK;
	}
	else
	{
		unlink(PASSWD_TMPFILE);
		return CPSS_ERROR;
	}
}

INT32 cpss_om_user_loginable_check(UINT8* pucUser)
{
	struct passwd* stpwd;

	/*获得当前用户的信息*/
	stpwd = getpwnam((INT8*) pucUser);
	if (NULL == stpwd)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR,
				"no user! in cpss_om_user_loginable_check\n");
		return CPSS_ERROR_USER_NOEXIST;
	}

	if(0 == strncmp(stpwd->pw_shell, "/bin/sh", strlen("/bin/sh")))
	{
		return CPSS_OK;
	}

	if(0 == strncmp(stpwd->pw_shell, "/sbin/nologin", strlen("/sbin/nologin")))
	{
		return CPSS_ERROR_USER_NOLOGIN;
	}

	return CPSS_OK;
}

INT32 cps_om_user_set_shell_tmout(UINT8 ucTmOut)
{
	FILE* pfd;

	pfd = fopen("/etc/shtmout", "w+");
	if(NULL == pfd)
	{
		cpss_print(SWP_SUBSYS_CPSS, CPSS_MODULE_CMN, CPSS_PRINT_ERROR, "cps_om_user_set_shell_tmout can not open /etc/shtmout");
		return CPSS_ERROR;
	}

	fprintf(pfd, "%d", ucTmOut);

	fclose(pfd);

	return CPSS_OK;
}

/*初始化时添加用户组*/
VOID cps_user_group_init()
{
	memset(&g_stCpsAcctError, 0, sizeof(CPS_ACCT_USER_ERROR_T));
	g_stCpsAcctError.ucErrorMaxTime = 5;
#ifndef SWP_LINUX
	return;
#else
	cps_system("groupadd -f -g 600 admin");
	cps_system("groupadd -f -g 700 user");
#endif
}

	
