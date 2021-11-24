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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define SIM_SHELL_STR_LEN 256

#define SIM_SHELL_FIND_SYM_SUCCESS 0
#define SIM_SHELL_FIND_SYM_FAILURE (-1)
#define SIM_SHELL_FIND_SYM_IDLE (-2)

typedef enum {
	SIM_SHELL_SYM_TYPE_DATA,	/* "DATA" */
	SIM_SHELL_SYM_TYPE_CODE,	/* "CODE" */
	SIM_SHELL_SYM_TYPE_INVALID = -1,
}SIM_SHELL_SYM_TYPE_E;

typedef unsigned int (SIM_SHELL_FUN_T)(unsigned int P1, unsigned int P2,
		unsigned int P3, unsigned int P4,
		unsigned int P5, unsigned int P6,
		unsigned int P7, unsigned int P8,
		unsigned int P9, unsigned int P10);

typedef struct sim_shell_sym_tab_struct {
	struct sim_shell_sym_tab_struct *pstNext;
    void *pvSymPtr;
    SIM_SHELL_SYM_TYPE_E eSymType;
    char acSymName[SIM_SHELL_STR_LEN];
}SIM_SHELL_SYM_TAB_T;

static SIM_SHELL_SYM_TAB_T *gp_SimShellSymTabHead = NULL;

char g_RunningVer[256] = {0};

static const char *g_WelcomString[] = {
"",
"      /////   /////   /////   /////   /////       |",
"     /////   /////   /////   /////   /////        |",
"    /////   /////   /////   /////   /////         |",
"    /////   /////   /////   /////   /////         |",
"   //////  //////  //////  //////  //////         |",
"   //////  //////  //////  //////  //////         |",
"    /////   /////   /////   /////   /////         |",
"    /////   /////   /////   /////   /////         |",
"     /////   /////   /////   /////   /////        |   	Development  System",
"      ////    ////    ////    ////    ////        |",
"       ////    ////    ////    ////    ////       |",
"        ////    ////    ////    ////    ////      |   	Host  Based   Shell",
"         ////    ////    ////    ////    ////     |",
"          ////    ////    ////    ////    ////    |",
"           ///     ///     ///     ///     ///    |   	Version  2.0",
"           ///     ///     ///     ///     ///    |",
"            //      //      //      //      //    |",
"            //      //      //      //      //    |         Hello",
"            //      //      //      //      //    |",
"           //      //      //      //      //     |",
"",
"      Copyright 2011-2021 , Inc.",
"",
"C++ Constructors/Destructors Strategy is AUTOMATIC",
};

static void sim_shell_welcom()
{
    unsigned int Counter;
    unsigned int LineSum = sizeof(g_WelcomString) / sizeof(char*);

    for(Counter = 0;Counter < LineSum;Counter++)
    {
        printf("%s\n",g_WelcomString[Counter]);
    }
}

static int sim_shell_sym_insert(SIM_SHELL_SYM_TAB_T *vpstSym)
{
	vpstSym->pstNext = gp_SimShellSymTabHead;
	gp_SimShellSymTabHead = vpstSym;
	return 0;
}

static SIM_SHELL_SYM_TAB_T* sim_shell_sym_get(char *vpcSymName)
{
	SIM_SHELL_SYM_TAB_T *pstSym;

	pstSym = gp_SimShellSymTabHead;
	while(NULL != pstSym)
	{
		if(strcmp(pstSym->acSymName, vpcSymName) == 0)
		{
			return pstSym;
		}
		pstSym = pstSym->pstNext;
	}
	return NULL;
}

static int sim_shell_sym_init(char *vpcMapName)
{
    FILE *pFile;
    SIM_SHELL_SYM_TAB_T *pstSym;
    unsigned long ulSymAddr;
    char cSymType;
    char acSymName[SIM_SHELL_STR_LEN];
    char sStringLine[SIM_SHELL_STR_LEN];

    pFile = fopen(vpcMapName, "r");
    if(NULL == pFile)
    {
        return -1;
    }
    while(!feof(pFile))
    {
    	fgets(sStringLine, SIM_SHELL_STR_LEN, pFile);
		if(3 == sscanf(sStringLine, "%X %c %s\n", &ulSymAddr, &cSymType, acSymName))
    	{
    		pstSym = malloc(sizeof(SIM_SHELL_SYM_TAB_T));
    		if(NULL == pstSym)
    		{
    			return -1;
    		}
    		pstSym->pvSymPtr = (void*)ulSymAddr;
    		strncpy(pstSym->acSymName, acSymName, SIM_SHELL_STR_LEN);
    		if(('T' == cSymType) || ('t' == cSymType))
    		{
        		pstSym->eSymType = SIM_SHELL_SYM_TYPE_CODE;
    		}
    		else if(('B' == cSymType) || ('b' == cSymType))
    		{
        		pstSym->eSymType = SIM_SHELL_SYM_TYPE_DATA;
    		}
    		else
    		{
    			free(pstSym);
    			continue;
    		}
    		if(0 != sim_shell_sym_insert(pstSym))
    		{
    			free(pstSym);
    		}
    	}
    }
    return 0;
}

static unsigned long sim_shell_get_str_val(char *pvStr)
{
	if(pvStr[0] == '"')
	{
		pvStr[strlen(pvStr) - 1] = 0;
		return (unsigned long) (pvStr + 1);
	}
	return strtoul(pvStr, NULL, 0);
}

static int sim_shell_split_string(char *vpcString, int viMaxFieldSum,
		char vacFieldStrings[][SIM_SHELL_STR_LEN])
{
	char c;
	int iStatus = 0;
	int iFieldSum = 0;
	char *pcStart = vpcString;
	char *pcNext = vpcString;

	while((c = *pcNext++) != 0)
	{
		switch(iStatus)
		{
		case 0:
			if(isblank(c))
			{
				continue;
			}
			else if(c == '"')
			{
				pcStart = pcNext - 1;
				iStatus = 1; 
			}
			else
			{
				pcStart = pcNext - 1;
				iStatus = 2;
			}
			break;
		case 1: /* ��˫���ŵ��ַ������� */
			if(c == '"')
			{
				if(isblank(*pcNext) || (*pcNext == '\n'))    /* �ַ�������*/
				{
					memcpy(vacFieldStrings[iFieldSum], pcStart, pcNext
							- pcStart);
					vacFieldStrings[iFieldSum][pcNext - pcStart] = 0;
					if(++iFieldSum >= viMaxFieldSum)
					{
						return iFieldSum;
					}
					iStatus = 0;
				}
			}
			break;
		case 2:
			if(isblank(c) || (c == '\n')) 
			{
				memcpy(vacFieldStrings[iFieldSum], pcStart, pcNext - 1
						- pcStart);
				vacFieldStrings[iFieldSum][pcNext - 1 - pcStart] = 0;
				if(++iFieldSum >= viMaxFieldSum)
				{
					return iFieldSum;
				}
				iStatus = 0;
			}
			break;
		default:
			break;
		}
	}
	return iFieldSum;
}

static unsigned long sim_shell_pro_cmd(char *pString, int *pFlag)
{
    int lFieldSum;
	int lCounter;
	SIM_SHELL_SYM_TAB_T *pstSym;
	unsigned long ulFieldVal[10] = { 0 };
	char sFieldString[11][SIM_SHELL_STR_LEN];

	lFieldSum = sim_shell_split_string(pString, 11, sFieldString);
	if(0 >= lFieldSum)
	{
		*pFlag = SIM_SHELL_FIND_SYM_IDLE;
		return 0;
	}

	for(lCounter = 1; lCounter < lFieldSum; lCounter++)
	{
		ulFieldVal[lCounter - 1]
				= sim_shell_get_str_val(sFieldString[lCounter]);
	}
	pstSym = sim_shell_sym_get(sFieldString[0]);
	if(NULL == pstSym)
	{
		*pFlag = SIM_SHELL_FIND_SYM_FAILURE;
		return 0;
	}
	if(SIM_SHELL_SYM_TYPE_DATA == pstSym->eSymType)
	{
		if((lFieldSum > 1) && (0 == strcmp(sFieldString[1], "=")))
		{
			*(unsigned long*) pstSym->pvSymPtr = ulFieldVal[1];
		}
		*pFlag = SIM_SHELL_FIND_SYM_SUCCESS;
		return *(unsigned long*) pstSym->pvSymPtr;
	}
	*pFlag = SIM_SHELL_FIND_SYM_SUCCESS;
	return ((SIM_SHELL_FUN_T*) pstSym->pvSymPtr)(ulFieldVal[0], ulFieldVal[1],
			ulFieldVal[2], ulFieldVal[3], ulFieldVal[4], ulFieldVal[5],
			ulFieldVal[6], ulFieldVal[7], ulFieldVal[8], ulFieldVal[9]);
}

extern int cps_system(unsigned char* pucCmd);
static void sim_shell_entry()
{
	int Flag;
	unsigned long tReturn;
	FILE *pStdFile;
	char sInputString[SIM_SHELL_STR_LEN];

    pStdFile = fdopen(STDIN_FILENO, "r");
    if(NULL == pStdFile)
    {
    	printf("error open stdin, exit...\n");
    	return;
    }
    printf("->");
    while(1)
    {
        if(NULL == fgets(sInputString, SIM_SHELL_STR_LEN, pStdFile))
        {
            printf("->");
            continue;
        }
        tReturn = sim_shell_pro_cmd(sInputString,&Flag);
        if(SIM_SHELL_FIND_SYM_FAILURE == Flag)
        {
        	cps_system(sInputString);
            printf("->");
        }
        else if(SIM_SHELL_FIND_SYM_IDLE == Flag)
        {
            printf("->");
        }
        else
        {
            printf("->Value = %#x = %d\n->", tReturn, tReturn);
        }
    }
}

static void sim_shell_usr_app(int argc, char * argv[])
{
	wlan_init();
	/* Add user init functions here */
}

#ifdef SIM_SHELL_ENABLE
int main(int argc, char * argv[])
#else
int sim_shell_main_ex(int argc, char * argv[])
#endif
{
	char acMapFileName[SIM_SHELL_STR_LEN];

	strncpy(acMapFileName, argv[0], SIM_SHELL_STR_LEN);

	strncpy(g_RunningVer, argv[0], SIM_SHELL_STR_LEN);

	strcat(acMapFileName, ".map");

	if(0 != sim_shell_sym_init(acMapFileName))
	{
		printf("symbol table init failure!!!\n");
//		return -1;
	}
	sim_shell_welcom();
	sim_shell_usr_app(argc, argv);
	sim_shell_entry();
	return 0;
}

/*******************************************************************************
 * Tools start
 */
void symShow()
{
	SIM_SHELL_SYM_TAB_T *pstSym;

	pstSym = gp_SimShellSymTabHead;
	printf("%-8s\t%-8s\t%-8s\n", "TYPE", "ADDR", "NAME");
	while(NULL != pstSym)
	{
		if(SIM_SHELL_SYM_TYPE_DATA == pstSym->eSymType)
		{
			printf("%-8s\t%08X\t%s\n", "DATA", (unsigned int)pstSym->pvSymPtr, pstSym->acSymName);
		}
		else if(SIM_SHELL_SYM_TYPE_CODE == pstSym->eSymType)
		{
			printf("%-8s\t%08X\t%s\n", "CODE", (unsigned int)pstSym->pvSymPtr, pstSym->acSymName);
		}
		else
		{
			printf("%-8s\t%08X\t%s\n", "UNKNOWN", (unsigned int)pstSym->pvSymPtr, pstSym->acSymName);
		}
		pstSym = pstSym->pstNext;
	}
}

void reboot()
{
    exit(EXIT_SUCCESS);
}

void lkup(char *vpcStr)
{
	SIM_SHELL_SYM_TAB_T *pstSym;
	
	if(NULL == vpcStr)
	{
		return;
	}

	pstSym = gp_SimShellSymTabHead;
	printf("%-8s\t%-8s\t%-8s\n", "TYPE", "ADDR", "NAME");
	while(NULL != pstSym)
	{
		if(NULL != strstr(pstSym->acSymName, vpcStr))
		{
			if(SIM_SHELL_SYM_TYPE_DATA == pstSym->eSymType)
			{
				printf("%-8s\t%08X\t%s\n", "DATA", (unsigned int)pstSym->pvSymPtr, pstSym->acSymName);
			}
			else if(SIM_SHELL_SYM_TYPE_CODE == pstSym->eSymType)
			{
				printf("%-8s\t%08X\t%s\n", "CODE", (unsigned int)pstSym->pvSymPtr, pstSym->acSymName);
			}
			else
			{
				printf("%-8s\t%08X\t%s\n", "UNKNOWN", (unsigned int)pstSym->pvSymPtr, pstSym->acSymName);
			}
		}
		pstSym = pstSym->pstNext;
	}
}
