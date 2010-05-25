#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#define CP_GB       936
#include "gbinput.h"
#include "AEEStdLib.h"

#ifdef DIFF_CAPITAL//区分字母大小写输入
#endif

/*---------------------------------------------------------------------------*/
/*                         TYPE AND CONSTANT                                 */
/*---------------------------------------------------------------------------*/
#if LATIN_ENCODE_LENGTH == 2
	const unsigned short okToBeginMakeWordStr[] 
	={0x6309,'O','K',0x5f00,0x59cb,0x81ea,0x9020,0x8bcd,0x00,0x00};
	const unsigned short okToAddMakeWordStr[] 
	={0x6309,'O','K',0x52a0,0x5165,0x81ea,0x9020,0x8bcd,0x00,0x00};
#else
	#define okToBeginMakeWordStr "按OK开始自造词"
	#define okToAddMakeWordStr "按OK加入自造词"	
#endif

/********************************Macro Declaration*************************************/
#define CN_WORD_MAKING	2
#define CN_WORD_FINISH	3

#define ELEM_CHARWORD	1
#define ELEM_SELFWORD	2

#define ZHCN_INDEX		0
#define EN_INDEX		1
//#define IMLIST_INDEX	2
#define SM_INDEX		2
#define CHAR_INDEX		3
#define NUM_INDEX		4
#define CAPITAL_CHAR_INDEX		5
#define LOWERCASE_CHAR_INDEX	6

#define DONT_ADD_SPACE              0
#define ADD_SPACE_ALWAYS            1
#define ADD_SPACE_AFTER_WORD        2

#define ASCII_INTERPUNCTION         (" ,.?!@_-\'\"():;+*/<>~#$%^&\\|`=")
#define CHARACTER_IM_CANDIDATES_MAX		9	/**< 字母输入法最大候选数,added by xch 20060807*/

/******************************	ENUM ****************************************/
typedef enum MAKE_WORD_STATUS
{
	MW_POWERON ,	//刚刚切换到自造词状态，未选择任何字母的状态
	MW_SELECTTING ,	//至少选择了一个字母以上
	MW_FINISH 	//全部选择完，等待用户按OK键
}MAKE_WORD_STATUS;

/*********************************Struct Declaration************************************/
typedef struct CHARInputStruct
{
	//	KeyCandidates *CharCandidateArray;//位置不能调换
	KeyCandidates *CharCandidateArray;
	
	char inputString[64];
	int inputLth;
	int curSeledCandIndex;
	
}CHARInputStruct;

typedef struct SMInputStruct
{
	char inputString[64];
	char numberString[64];
	int inputLth;
	int curSeledCandIndex;
	char *pchar;//当前正在选择的字符，对应的候选在CharCandigdateArray中定义
	MAKE_WORD_STATUS status;//自造词状态
	ENInputStruct *penis;//正在自造的smart english
	
	//char *pend;
}SMInputStruct;

/****************************Static Defininion*******************************/
static KeyCandidates CharCandidateArray[] = 
{
	{'0',0 },
    {'1',0 },
    {'2',0 },
    {'3',0 },
    {'4',0 },
    {'5',0 },
    {'6',0 },
    {'7',0 },
    {'8',0 },
    {'9',0 },	
    {'*', 0},
    {'#', 0},
	{0, 0}
};

/*******************************Function Declaration************************************/
static void PYOnReturn(GBIM *p,void *param);
static void PYOnSharp(GBIM *p,void *param);
static void PYOnStar(GBIM *p,void *param);
static void PYReset(GBIM *p,void *param);
static void PYSwitchIM(GBIM *p,void *param);
static void PYOnLeft(GBIM *p,void *param);
static void PYOnRight(GBIM *p,void *param);
static void PYOnUp(GBIM *p,void *param);
static void PYOnDown(GBIM *p,void *param);
static void PYOnBlank(GBIM *p,void *param);
static void PYOnBack(GBIM *p,void *param);
static void PYOnNumS(GBIM *p,void *param,int realkey);
static void PYOnNumL(GBIM *p,void *param,int realkey);
static void PYPrepareShowMessage(GBIM *p,void* param);
int PYCanPageDown(GBIM *p,void* param);
int PYCanPageUp(GBIM *p,void* param);
static int PYCanPrevHeader(GBIM *p,void *param);
static int PYCanNextHeader(GBIM *p,void *param);
static void PYDisplay(GBIM *p,void* param);

static void PYSetSyllableIndex(GBIM *p,unsigned char syllableIndex);




static void SmartEnOnReturn(GBIM *p,void *param);
static void SmartEnOnSharp(GBIM *p,void *param);
static void SmartEnOnStar(GBIM *p,void *param);
static int SmartEnCanPageUp(GBIM *p,void* param);
static int SmartEnCanPageDown(GBIM *p,void* param);
static void SmartEnReset(GBIM *p,void *param);
static void SmartEnSwitchIM(GBIM *p,void *param);
static void SmartEnOnLeft(GBIM *p,void *param);
static void SmartEnOnRight(GBIM *p,void *param);
static void SmartEnOnUp(GBIM *p,void *param);
static void SmartEnOnDown(GBIM *p,void *param);
static void SmartEnOnBlank(GBIM *p,void *param);
static void SmartEnOnBack(GBIM *p,void *param);
static void SmartEnOnNumS(GBIM *p,void *param,int realkey);
static void SmartEnOnNumL(GBIM *p,void *param,int realkey);
static void SmartEnPrepareShowMessage(GBIM *p,void* param);
static int SmartEnCanPrevHeader(GBIM *p,void *param);
static int SmartEnCanNextHeader(GBIM *p,void *param);
static void SmartEnDisplay(GBIM *p,void *param);

static void SelIMOnReturn(GBIM *p,void *param);
static void SelIMOnSharp(GBIM *p,void *param);
static void SelIMOnStar(GBIM *p,void *param);
static int SelIMCanPageUp(GBIM *p,void* param);
static int SelIMCanPageDown(GBIM *p,void* param);
static void SelIMReset(GBIM *p,void *param);
static void SelIMSwitchIM(GBIM *p,void *param);
static void SelIMOnLeft(GBIM *p,void *param);
static void SelIMOnRight(GBIM *p,void *param);
static void SelIMOnUp(GBIM *p,void *param);
static void SelIMOnDown(GBIM *p,void *param);
static void SelIMOnBlank(GBIM *p,void *param);
static void SelIMOnBack(GBIM *p,void *param);
static void SelIMOnNumS(GBIM *p,void *param,int realkey);
static void SelIMOnNumL(GBIM *p,void *param,int realkey);
static void SelIMPrepareShowMessage(GBIM *p,void* param);
static int SelIMCanPrevHeader(GBIM *p,void *param);
static int SelIMCanNextHeader(GBIM *p,void *param);

#ifdef _SHOW_DEBUG
static void SelIMDisplay(GBIM *p,void *param);
#endif

static void SMOnReturn(GBIM *p,void *param);
static void SMOnSharp(GBIM *p,void *param);
static void SMOnStar(GBIM *p,void *param);
static int SMCanPageUp(GBIM *p,void* param);
static int SMCanPageDown(GBIM *p,void* param);
static void SMReset(GBIM *p,void *param);
static void SMSwitchIM(GBIM *p,void *param);
static void SMOnLeft(GBIM *p,void *param);
static void SMOnRight(GBIM *p,void *param);
static void SMOnUp(GBIM *p,void *param);
static void SMOnDown(GBIM *p,void *param);
static void SMOnBlank(GBIM *p,void *param);
static void SMOnBack(GBIM *p,void *param);
static void SMOnNumS(GBIM *p,void *param,int realkey);
static void SMOnNumL(GBIM *p,void *param,int realkey);
static void SMPrepareShowMessage(GBIM *p,void* param);
static int SMCanPrevHeader(GBIM *p,void *param);
static int SMCanNextHeader(GBIM *p,void *param);

#ifdef _SHOW_DEBUG
static void SMDisplay(GBIM *p,void *param);
#endif

//字母输入

static void CharOnReturn(GBIM *p,void *param);
static void CharOnSharp(GBIM *p,void *param);
static void CharOnStar(GBIM *p,void *param);
static int CharCanPageUp(GBIM *p,void* param);
static int CharCanPageDown(GBIM *p,void* param);
static void CharReset(GBIM *p,void *param);
static void CharSwitchIM(GBIM *p,void *param);
static void CharOnLeft(GBIM *p,void *param);
static void CharOnRight(GBIM *p,void *param);
static void CharOnUp(GBIM *p,void *param);
static void CharOnDown(GBIM *p,void *param);
static void CharOnBlank(GBIM *p,void *param);
static void CharOnBack(GBIM *p,void *param);
static void CharOnNumS(GBIM *p,void *param,int realkey);
static void CharOnNumL(GBIM *p,void *param,int realkey);
static void CharPrepareShowMessage(GBIM *p,void* param);
static int CharCanPrevHeader(GBIM *p,void *param);
static int CharCanNextHeader(GBIM *p,void *param);

#ifdef _SHOW_DEBUG
static void CharDisplay(GBIM *p,void *param);
#endif

//数字输入
static void NumberOnReturn(GBIM *p,void *param);
static void NumberOnSharp(GBIM *p,void *param);
static void NumberOnStar(GBIM *p,void *param);
static int NumberCanPageUp(GBIM *p,void* param);
static int NumberCanPageDown(GBIM *p,void* param);
static void NumberReset(GBIM *p,void *param);
static void NumberSwitchIM(GBIM *p,void *param);
static void NumberOnLeft(GBIM *p,void *param);
static void NumberOnRight(GBIM *p,void *param);
static void NumberOnUp(GBIM *p,void *param);
static void NumberOnDown(GBIM *p,void *param);
static void NumberOnBlank(GBIM *p,void *param);
static void NumberOnBack(GBIM *p,void *param);
static void NumberOnNumS(GBIM *p,void *param,int realkey);
static void NumberOnNumL(GBIM *p,void *param,int realkey);
static void NumberPrepareShowMessage(GBIM *p,void* param);
static int NumberCanPrevHeader(GBIM *p,void *param);
static int NumberCanNextHeader(GBIM *p,void *param);


static void SMFirstPage(GBIM *p,void*param);
static int c2u(unsigned short *pu,char *pc,int lth);

static void GBElementsInit(CHARInputStruct *pcharis,int option,int elemtype);
static void GBEngineChangeIM(void);

static int SelectCandidate(EBInputStruct * is, int index);
static int SendHZtoClient(unsigned short *upchar,int lth);

/**********************************GLOBAL DEFINITION ***********************************/
EBInputStruct EBIS;
ENInputStruct ENIS;
CHARInputStruct CHARIS ={CharCandidateArray,0};

#ifdef DIFF_CAPITAL//区分字母大小写输入
CHARInputStruct capital_CHARIS ={CharCandidateArray,0};
CHARInputStruct lowercase_CHARIS ={CharCandidateArray,0};
#endif

//CList	IMLIST;

SMInputStruct SMIS={0,8,0};

//void GBElementsInit();
GBIM GBIMS[]=
{
	{0,&EBIS,0,0,PYOnUp,PYOnDown,PYOnLeft,PYOnRight,
		PYOnNumS,PYOnNumL,PYOnBlank,PYOnBack,
		PYSwitchIM,PYReset,0,0,PYPrepareShowMessage,
		PYCanPageUp,PYCanPageDown,
		PYCanPrevHeader,PYCanNextHeader,
		PYOnStar,
		PYOnSharp,
		PYOnReturn
	},
	{1,&ENIS,0,0,SmartEnOnUp,SmartEnOnDown,SmartEnOnLeft,SmartEnOnRight,
	SmartEnOnNumS,SmartEnOnNumL,SmartEnOnBlank,SmartEnOnBack,
	SmartEnSwitchIM,SmartEnReset,0,0,SmartEnPrepareShowMessage,
	SmartEnCanPageUp,SmartEnCanPageDown,
	SmartEnCanPrevHeader,SmartEnCanNextHeader,
	SmartEnOnStar,
	SmartEnOnSharp,
	SmartEnOnReturn
	},
	{2,&CHARIS,0,0,CharOnUp,CharOnDown,CharOnLeft,CharOnRight,
	CharOnNumS,CharOnNumL,CharOnBlank,CharOnBack,
	CharSwitchIM,CharReset,0,0,CharPrepareShowMessage,
	CharCanPageUp,CharCanPageDown,
	CharCanPrevHeader,CharCanNextHeader,
	CharOnStar,
	CharOnSharp,
	CharOnReturn
	},
	{3,0,0,0,NumberOnUp,NumberOnDown,NumberOnLeft,NumberOnRight,
	NumberOnNumS,NumberOnNumL,NumberOnBlank,NumberOnBack,
	NumberSwitchIM,NumberReset,0,0,NumberPrepareShowMessage,
	NumberCanPageUp,NumberCanPageDown,
	NumberCanPrevHeader,NumberCanNextHeader,
	NumberOnStar,
	NumberOnSharp,
	NumberOnReturn
	},
	/*	{4,&IMLIST,0,0,SelIMOnUp,SelIMOnDown,SelIMOnLeft,SelIMOnRight,
	SelIMOnNumS,SelIMOnNumL,SelIMOnBlank,SelIMOnBack,
	SelIMSwitchIM,SelIMReset,0,0,SelIMPrepareShowMessage,
	SelIMCanPageUp,SelIMCanPageDown,
	SelIMCanPrevHeader,SelIMCanNextHeader,
	SelIMOnStar,
	//SelIMDisplay,	
	SelIMOnSharp,
	SelIMOnReturn
},*/
	{4,&SMIS,0,0,SMOnUp,SMOnDown,SMOnLeft,SMOnRight,
	SMOnNumS,SMOnNumL,SMOnBlank,SMOnBack,
	SMSwitchIM,SMReset,0,0,SMPrepareShowMessage,
	SMCanPageUp,SMCanPageDown,
	SMCanPrevHeader,SMCanNextHeader,
	SMOnStar,
	SMOnSharp,
	SMOnReturn
	}
#ifdef DIFF_CAPITAL//区分字母大小写输入
	,
	{5,&CHARIS,0,0,CharOnUp,CharOnDown,CharOnLeft,CharOnRight,
	CharOnNumS,CharOnNumL,CharOnBlank,CharOnBack,
	CharSwitchIM,CharReset,0,0,CharPrepareShowMessage,
	CharCanPageUp,CharCanPageDown,
	CharCanPrevHeader,CharCanNextHeader,
	CharOnStar,
	CharOnSharp,
	CharOnReturn
	},
	{6,&CHARIS,0,0,CharOnUp,CharOnDown,CharOnLeft,CharOnRight,
	CharOnNumS,CharOnNumL,CharOnBlank,CharOnBack,
	CharSwitchIM,CharReset,0,0,CharPrepareShowMessage,
	CharCanPageUp,CharCanPageDown,
	CharCanPrevHeader,CharCanNextHeader,
	CharOnStar,
	CharOnSharp,
	CharOnReturn
	}
#endif
	
};

GBIMWT GBIMW={GBIMS,0,0,0,0};

/**********************************FUNCTION DEFINITION*********************************/
static int SmartEnSelectCandidate(ENInputStruct * enis, int index, int addSpaceAfterWord);

static void PYSetSyllableIndex(GBIM *p,unsigned char syllableIndex)
{
	EBInputStruct *pis =(EBInputStruct *)(p->pIS);
	if(syllableIndex >= 0 && syllableIndex <= pis->syllableLen)
	{
		pis->syllableIndex = syllableIndex;
		EBFindFirstPageC(pis);					
		GBIMW.isSelectedCand =0;
    GBIMW.isShowCandWin = 1;
		GBIMW.noHandleKeyEvent =0;
	}
}

//static void GetAssociate(unsigned short *str)
//{
//	int len = gb_strlen(str);
//	EBInputStruct * is = (EBInputStruct *)GBIMW.GBIMS->pIS;	
//	
//	if(len > 0)
//		GBIMW.isShowCandWin = 1;
//
//	GBIMW.isSelectedCand = 0;
//
//	//historyISNLen += len;		
//	//用户每输入一个汉字都应该调用EBAssocAddHistoryISN, 以更新联想词
//	EBAssocAddHistoryISN(is, str, len);
//	if(is->selCount > 1)
//	{
//		//加入自造词，is->fullInputString为对应的输入串，可以为数字串，也可以为字母串
//		EBShAddPhrase(is->fullOutputString, is->fullInputString);
//	}
//	//表示开始重新输入词语
//	EBHelperEBISRestart(is);
//	//获得联想词
//	EBAssocFirstPageC(is);	
//
//	GBIMW.selectIndex =0;	
//	GBIMW.noHandleKeyEvent = 0;
//}
static void GetAssociate(unsigned short *str)
{
	int len = WSTRLEN(str);
	EBInputStruct * is = (EBInputStruct *)GBIMW.GBIMS->pIS;	

	if(GBIMW.activeIM != ZHCN_INDEX)
	{
		return;
	}
	
	if(len > 0)
	{
		GBIMW.isShowCandWin = 1;
	}
	else
	{
		GBIMW.isShowCandWin = 0;
	}

	//表示开始重新输入词语
	EBHelperEBISRestart(is);
	
	GBIMW.isSelectedCand = 0;	
	//historyISNLen += len;		
	//用户每输入一个汉字都应该调用EBAssocAddHistoryISN, 以更新联想词
	EBAssocAddHistoryISN(is, str, len);
	//获得联想词
	EBAssocFirstPageC(is);	
	
	GBIMW.selectIndex =0;	
	GBIMW.noHandleKeyEvent = 0;
}

static void PYOnReturn(GBIM *p,void *param)
{
	EBInputStruct *pis =0;
	if(p->pSwitchItem)
	{
		((GBIM*)(p->pSwitchItem))->GBIMOnReturn(p->pSwitchItem,param);
		return;
	}
	pis =(EBInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.isSelectedCand =0;
		GBIMW.noHandleKeyEvent =1;
		return;
	}
	
	WSTRCPY((wchar_t*)GBIMW.upscreenStr,(const wchar_t*)pis->fullOutputString);
	GBIMW.upscreenLen =WSTRLEN((const wchar_t*)GBIMW.upscreenStr);
	
	GBIMW.shouldUpScreen =1;
	GBIMW.noHandleKeyEvent =1;
	//GBIMW.isShowCandWin =1;
}

static void PYOnSharp(GBIM *p,void *param)
{
	EBInputStruct *pis =0;
	if(p->pSwitchItem)
	{
		((GBIM*)(p->pSwitchItem))->GBIMOnSharp(p->pSwitchItem,param);
		return;
	}
	pis =(EBInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.isSelectedCand =0;
		GBIMW.noHandleKeyEvent =0;
		GBIMW.isShowCandWin =1;
		//return;
	}
	
	if(pis->inputStringLen ==0 && EBIsInterpunctionMode(pis) !=EB_OK)//开始进入符号模式
	{
		GBIMW.selectIndex =0;
		pis->componentLen =0;
		EBInterpunctionFirstPageC(pis);
	}
	else if(EBIsInterpunctionMode(pis) ==EB_OK)
	{
		if(GBIMW.selectIndex <pis->candidateLen-1)
		{
			GBIMW.selectIndex++;
		}
		else if(EBCanPageDown(pis) ==EB_OK)
		{
			GBIMW.selectIndex =0;
			EBNextPageC(pis);
		}
		else 
		{
			GBIMW.selectIndex =0;
			EBInterpunctionFirstPageC(pis);	
		}
		

		//if(GBIMW.candsNum > 0) 
		{ 
			GBIMW.isSelectedCand =1;
		}
	}
	else 
	{	
		
		EBHelperAddInputChar(pis,'#');
		
		GBIMW.selectIndex =0;
		EBFindFirstPageC(pis);	
	}		
}

static void PYOnStar(GBIM *p,void *param)
{
	EBInputStruct *pis =0;
	if(p->pSwitchItem)
	{
		((GBIM*)(p->pSwitchItem))->GBIMOnStar(p->pSwitchItem,param);
		return;
	}
	
	pis =(EBInputStruct *)(p->pIS);
	if(EBHelperAddInputChar(pis, L'*') == EB_OK)
	{
		EBFindFirstPageC(pis);
	} 	
}

static void SmartEnOnReturn(GBIM *p,void *param)
{
	ENInputStruct *pis =0;
	if(p->pSwitchItem)
	{
		((GBIM*)(p->pSwitchItem))->GBIMOnReturn(p->pSwitchItem,param);
		return;
	}
	pis =(ENInputStruct *)(p->pIS);
	
	//gb_strcpy((wchar_t*)GBIMW.upscreenStr,(const wchar_t*)pis->inputString);
	c2u((unsigned short*)GBIMW.upscreenStr,(char*)pis->inputString,pis->inputLen);
	GBIMW.upscreenLen =WSTRLEN((const wchar_t*)GBIMW.upscreenStr);
	
	GBIMW.shouldUpScreen =1;
	GBIMW.noHandleKeyEvent = 0; //Modified by hcy 08.04.03 1
}

static void SmartEnOnSharp(GBIM *p,void *param)
{
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	
	//added by xch 20060803
	if(pis->inputLen > 0 && pis->candidateCount <= 0)
		return;
	
	//这个是前面输入了英文，现在按‘#’应该词语上屏，然后继续输标点
	if(pis->inputLen > 0 && pis->inputString[0] != '#')
	{
        SmartEnSelectCandidate(pis, GBIMW.selectIndex, DONT_ADD_SPACE);
        /*
		SendHZtoClient (pis->candidates[GBIMW.selectIndex],
		gb_strlen((const wchar_t*)pis->candidates[GBIMW.selectIndex]));*/
		
		GBLtReset(pis);
	}
	
	//SMART:
	if(pis->inputLen ==0)
	{
		GBIMW.selectIndex =0;
		pis->inputString[pis->inputLen++]=L'#';	
		
#ifdef VIETNAM
		if(GBLtFirstPage(pis) == EBEN_OK);
#else
		if(GBLtFirstHeader(pis) == EBEN_OK)
		{
			GBLtFirstPage(pis);	
		}
#endif		
        GBIMW.isShowCandWin = 1;
		GBIMW.noHandleKeyEvent =0;
		GBIMW.isSelectedCand =0;
	}
	else if(pis->inputString[0]=='#')
	{
        
		if(GBIMW.selectIndex <pis->candidateCount -1)
		{
			GBIMW.selectIndex++;
		}
		else if(GBLtCanPageDown(pis) == EBEN_OK)
		{
			GBIMW.selectIndex =0;
			GBLtNextPage(pis);
		}
		else 
		{
			GBIMW.selectIndex =0;
			GBLtFirstPage(pis);	
		}
		
		GBIMW.isSelectedCand = 1;
		GBIMW.noHandleKeyEvent =0;
	}
	/*
	else if(pis->inputLen >0)
	{
	SmartEnSelectCandidate(pis, GBIMW.selectIndex, DONT_ADD_SPACE);
	SendHZtoClient (pis->candidates[GBIMW.selectIndex],
	gb_strlen((const wchar_t*)pis->candidates[GBIMW.selectIndex]));
	
	  GBLtReset(pis);
	  goto SMART;//智能标点
}*/
	
	return;
}

static void SmartEnOnStar(GBIM *p,void *param)
{	
	return;
}

static void PYReset(GBIM *p,void *param)
{
	EBInputStruct *pis =(EBInputStruct *)(p->pIS);
	EBInputMethodInit(pis);	
	//	EBInputReset(pis);
	p->pSwitchItem =0;
	
	if(GBIMW.activeMode ==1)
	{
		GBIMW.syllableNum =0;
	}
	else//笔画 | 组合
	{
		GBIMW.stroke[0] =0;	
		
		//GBIMW.componentsLth =0;
		
		//modified by xch 2006/08/03
		memset(GBIMW.components, 0, sizeof(GBIMW.components));
		
		if(pis->componentLen > 0)
		{
			GBIMW.componentsLth = pis->componentLen;
			memcpy(GBIMW.components, pis->components, sizeof(short) * pis->componentLen);
		}
		else
			GBIMW.componentsLth =0;
	}
	
}

static void PYSwitchIM(GBIM *p,void *param)
{
	EBInputStruct *pis =(EBInputStruct *)(p->pIS);
	if(!GBIMW.isShowCandWin || !(pis->inputStringLen>0))
	{
		GBEngineChangeIM();
		GBIMW.noHandleKeyEvent =0;
		GBIMW.isShowCandWin =0;
		return;	
	}
	
	if(p->pSwitchItem ==0 && GBIMW.isCanChangeIM && !GBIMW.isChineseOnly)
	{
		GBIMW.selectIndex =0;
		p->pSwitchItem =&GBIMS[1];
		{
			int i =0;
			ENInputStruct *penis=(ENInputStruct*)(((GBIM*)(p->pSwitchItem))->pIS);	
			
#if (LATIN_ENCODE_LENGTH == 2)
			for(i=0;i<pis->inputStringLen;i++)
			{
				penis->inputString[i] =pis->inputString[i];
			}
#else
			memcpy(penis->inputString,pis->inputString,pis->inputStringLen);
#endif
			penis->inputLen =pis->inputStringLen;
			penis->inputString[penis->inputLen] =0;
#ifdef VIETNAM
			if(i = (GBLtFirstPage(penis)) != EBEN_OK)
#else
				if(i = (GBLtFirstHeader(penis)) != EBEN_OK)
#endif
				{
					;//##printf("The number string doesn't match any word!\r\n");
					//p->pSwitchItem =0;//不切换
#ifdef __GBSHDATA
					//give a chance to user make word
					GBIMW.candsNum =1;
					gb_strcpy((GBCHAR*)GBIMW.cands[0],okToBeginMakeWordStr);

					
					GBIMW.syllableNum =1;			
					memset(GBIMW.fullOutput,0,sizeof(GBIMW.fullOutput));	
					
					//TODO
					
					gb_strncpy((GBCHAR*)GBIMW.fullOutput, MAX_INPUTLTH,(GBCHAR*)penis->inputString,penis->inputLen);//				
					
					GBIMW.isCandHelpTips =1;

#else
					GBIMW.candsNum = 0;
#endif
					GBIMW.noHandleKeyEvent =0;
					GBIMW.shouldUpScreen =0;
					GBIMW.isShowCandWin =1;		
				}
				else
				{
					GBLtFirstPage(penis);
					if(!penis->headerIsAWord && penis->candidateCount>0
						&& penis->inputLen >1)
						GBIMW.selectIndex =1;
					else
						GBIMW.selectIndex =0;
				}
		}
	}
	else if(p->pSwitchItem)
	{
		//surport change smarten output formart
		GBIMW.GBIMS[1].GBIMSwitchIM((GBIM*)p->pSwitchItem,0);
	}		
}

static int PYCanPrevHeader(GBIM *p,void *param)
{
	EBInputStruct * pis =(EBInputStruct *)(p->pIS);
	
	if(p->pSwitchItem)
	{
		return ((GBIM*)(p->pSwitchItem))->GBIMCanPrevHeader(p->pSwitchItem,param);
	}
	
	return GBIMW.selectIndex > 0 || EBCanPageUp(pis)==EB_OK;
}

static int PYCanNextHeader(GBIM *p,void *param)
{
	EBInputStruct * pis =(EBInputStruct *)(p->pIS);
	if(p->pSwitchItem)
	{
		return ((GBIM*)(p->pSwitchItem))->GBIMCanNextHeader(p->pSwitchItem,param);
	}
	
	return GBIMW.selectIndex < pis->candidateLen -1 || EBCanPageDown(pis)==EB_OK;
}

static void PYOnLeft(GBIM *p,void *param)
{
	EBInputStruct *pis =0;
	
	if(p->pSwitchItem)
	{
		((GBIM*)(p->pSwitchItem))->GBIMOnLeft(p->pSwitchItem,param);
		return;
	} 
	
	pis =(EBInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.isSelectedCand =0;
		GBIMW.noHandleKeyEvent =1;
		GBIMW.isShowCandWin =0;
		GBIMW.shouldUpScreen =0;
		return;
	}
	
	if(!GBIMW.isSelectedCand)//没有选中候选
	{
		if(p->pSwitchItem)
		{
			((GBIM*)(p->pSwitchItem))->GBIMOnLeft(p->pSwitchItem,param);
			return;
		} 
		
		pis =(EBInputStruct *)(p->pIS);
		
		if(pis->syllableLen > 0)
		{
			pis->syllableIndex = (pis->syllableIndex + pis->syllableLen) 
				% (pis->syllableLen + 1);
			EBFindFirstPageC(pis);		
		}
		else 
		{
			GBIMW.noHandleKeyEvent =0;
			GBIMW.shouldUpScreen =0;
		}	
	}
	else
	{
		if(EBIsAssociateMode(pis) !=EB_OK//非联想
			&& EBIsInterpunctionMode(pis) !=EB_OK//非符号 
			&& pis->inputStringLen<=0
			&& pis->candidateLen <=0)//非高频字
		{
			GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],param);
			GBIMW.noHandleKeyEvent =1;
			GBIMW.shouldUpScreen =0;
			return;
		}
		
		if(GBIMW.selectIndex >0)
		{
			GBIMW.selectIndex--;
		}
		else if(EBCanPageUp(pis) ==EB_OK)
		{
			EBPrevPageC(pis);
			GBIMW.selectIndex = pis->candidateLen-1;
		}
	}
	
}

static void PYOnRight(GBIM *p,void *param)
{
	EBInputStruct *pis =0;
	if(p->pSwitchItem)
	{
		((GBIM*)(p->pSwitchItem))->GBIMOnRight(p->pSwitchItem,param);
		return;
	} 
	
	pis =(EBInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.isSelectedCand =0;
		GBIMW.noHandleKeyEvent =1;
		GBIMW.isShowCandWin =0;
		GBIMW.shouldUpScreen =0;
		return;
	}
	
	if(!GBIMW.isSelectedCand)//没有选中候选
	{
		if(p->pSwitchItem)
		{
			((GBIM*)(p->pSwitchItem))->GBIMOnRight(p->pSwitchItem,param);
			return;
		} 
		
		pis =(EBInputStruct *)(p->pIS);
		if(pis->syllableLen > 0)
		{
			pis->syllableIndex = (pis->syllableIndex + 1) 
				% (pis->syllableLen + 1);
			EBFindFirstPageC(pis);
			//validInput = 1;
		}
		else if(pis->candidateLen > 1)//符号,笔画或组合模式下
		{
			GBIMW.isSelectedCand =1;

			GBIMW.selectIndex =1;
			GBIMW.noHandleKeyEvent =0;	
		}
	}
	else
	{
		if(EBIsAssociateMode(pis) !=EB_OK
			&& EBIsInterpunctionMode(pis) !=EB_OK 
			&& pis->inputStringLen <=0
			&& pis->candidateLen <=0)//非高频字
		{
			GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],param);
			GBIMW.noHandleKeyEvent =1;
			return;
		}
		
		if(GBIMW.selectIndex <pis->candidateLen-1)
		{
			GBIMW.selectIndex++;
		}
		else if(EBCanPageDown(pis) ==EB_OK)
		{
			GBIMW.selectIndex =0;
			EBNextPageC(pis);
		}
	}	
}

static void PYOnBlank(GBIM *p,void *param)
{
	EBInputStruct *pis =0;
	
	if(p->pSwitchItem)
	{
		((GBIM*)(p->pSwitchItem))->GBIMOnBlank(p->pSwitchItem,param);
		return;
	} 
	
	pis =(EBInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.isSelectedCand =0;
		GBIMW.noHandleKeyEvent =1;
		GBIMW.isShowCandWin =0;
		GBIMW.shouldUpScreen =0;
		return;
	}
	
	if(GBIMW.isSelectedCand)
	{
		SelectCandidate(pis,GBIMW.selectIndex);
		GBIMW.selectIndex =0;
	}
	else//没选中状态,默认上0
	{		
		int ret=0;
		if((ret=SelectCandidate(pis,0)) ==CN_WORD_MAKING)
		{
			GBIMW.isSelectedCand =1;
			GBIMW.isShowCandWin =1;
			GBIMW.shouldUpScreen =0;
		}
		else if(ret ==CN_WORD_FINISH)
		{
			GBIMW.isSelectedCand =0;
			GBIMW.isShowCandWin =1;
			GBIMW.shouldUpScreen =1;
		}
		else
		{
			GBIMW.isSelectedCand =0;
			GBIMW.isShowCandWin =1;
			GBIMW.shouldUpScreen =0;
		}
	}
}

static void PYOnNumS(GBIM *p,void *param,int realkey)
{
	EBInputStruct *pis =0;
	GBIMW.shouldUpScreen =0;
	
	if(p->pSwitchItem)
	{
		((GBIM*)(p->pSwitchItem))->GBIMOnNumS(p->pSwitchItem,param,realkey);
		return;
	} 
	
	pis =(EBInputStruct *)(p->pIS);

  if(GBIMW.gbImeMode == PT_PINYIN_MODE && EBIsAssociateMode(pis) == EB_OK)
	{
		GBIMW.isShowCandWin =0;
	}


	//##添加以下为了输入1出来符号,并可连续输入电话号码
	if(pis->inputMethod ==1 
		&& pis->inputStringLen ==0
		&& realkey ==GB_VK1
		&& GBIMW.isSelectedCand ==0)
	{
		GBIMW.isSelectedCand =0;
		PYOnSharp(p,0);
		
		if(pis->inputStringLen ==2
			&& pis->inputString[1] =='#')
		{
			pis->inputStringLen =1;
			pis->inputString[1] =0;
		}
		else if(pis->inputStringLen ==0)
		{
			pis->fullInputString[pis->inputStringLen] ='1';
			pis->inputString[pis->inputStringLen++] ='1';
		}

		return;
	}
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.isShowCandWin =1;
		GBIMW.isSelectedCand =0;
	}
	else
	{
		if(!GBIMW.isSelectedCand)
		{
			if(EBIsInterpunctionMode(pis) ==EB_OK)
			{
				if(pis->inputMethod ==1)
				{
					if(GBIMW.isSelectedCand)
						SelectCandidate(pis,0);
					else
					{
						pis->fullInputString[pis->inputStringLen] =realkey;
						pis->inputString[pis->inputStringLen++]	 =realkey;
						return;
					}
				}
				else
					SelectCandidate(pis,0);
			}
			GBIMW.selectIndex =0;
		}
		else
		{
			int retval =0;
			if((retval =SelectCandidate(pis,realkey-'1')) ==CN_WORD_MAKING)
			{
				GBIMW.isSelectedCand =1;
				GBIMW.isShowCandWin =1;
			}
			else if(retval ==CN_WORD_FINISH)
			{
				GBIMW.isSelectedCand =0;
				GBIMW.selectIndex =0;
				GBIMW.shouldUpScreen =1;
			}
			else
			{
				if(realkey-'1' >= pis->candidateLen || realkey =='0')
				{					
					GBIMW.isSelectedCand =1;
				}
				else
				{
					GBIMW.isSelectedCand =0;
				}
				
				GBIMW.noHandleKeyEvent =0;
				GBIMW.shouldUpScreen =0;				
			}
			
			
			return;
		}
	}
	
	pis->syllableIndex =0;
	if(EBHelperAddInputChar(pis, realkey) == EB_OK)
	{
		EBFindFirstPageC(pis);
		GBIMW.selectIndex =0;	
	}	
	
	GBIMW.noHandleKeyEvent =0;
	
}

static void PYOnNumL(GBIM *p,void *param,int realkey)
{
	EBInputStruct *pis =0;
	if(p->pSwitchItem)
	{
		((GBIM*)(p->pSwitchItem))->GBIMOnNumL(p->pSwitchItem,param,realkey);
		return;
	} 
	
	pis =(EBInputStruct *)(p->pIS);
	SelectCandidate(pis,realkey-GB_VK1L);
	
}

int PYCanPageUp(GBIM *p,void *param)
{
	EBInputStruct *pis =0;
	
	if(p->pSwitchItem)
	{
		return ((GBIM*)(p->pSwitchItem))->GBIMCanPageUp(p->pSwitchItem,param);
	}
	pis =(EBInputStruct *)(p->pIS);
	return EBCanPageUp(pis) == EB_OK;
}

extern boolean IsGBAssociateMode(void);

//对PT_PINYIN_MODE特殊处理,需要考虑拼音输入法UP上屏功能不能使用
static void PYOnUp(GBIM *p,void *param)
{
	EBInputStruct *pis =0;
	if(p->pSwitchItem)
	{
		((GBIM*)(p->pSwitchItem))->GBIMOnUp(p->pSwitchItem,param);
		return;
	}
	
	pis =(EBInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.noHandleKeyEvent =1;
		GBIMW.isShowCandWin =0;
		GBIMW.isSelectedCand =0;
		GBIMW.shouldUpScreen =0;
		return;
	}
	
	if(EBCanPageUp(pis) == EB_OK)
	{
		//根据状态获得上一页候选字
		EBPrevPageC(pis);
	}
	else if(GBIMW.isSelectedCand)
	{
		GBIMW.isSelectedCand =0;
		GBIMW.isShowCandWin =1;
		GBIMW.noHandleKeyEvent =0;
		GBIMW.selectIndex =0;
	}
	else if(pis->inputStringLen>0)
	{
	  //added by list.//使得向上翻页到第一页时,不执行上屏操作.
	  if(GBIMW.gbImeMode == PT_PINYIN_MODE)
	  {
	      return;
	  }
		if(pis->syllableIndex !=0)
			return;
		if(pis->selCount > 0)//已经选了一次以上汉字
		{
			unsigned char *pend = (unsigned char *)pis->fullOutputString;
			
			while(*pend < '0' || *pend > '9')
			{
				//*pend =0;
				pend++;
			}
			SendHZtoClient(pis->fullOutputString, (pend -(unsigned char *)pis->fullOutputString) / 2);
		}
		else
		{
			memset(GBIMW.upscreenStr, 0, sizeof(GBIMW.upscreenStr));
			GBIMW.upscreenLen = strlen(pis->fullInputString);
#if LATIN_ENCODE_LENGTH == 2
			c2u(GBIMW.upscreenStr, pis->fullInputString,GBIMW.upscreenLen);
#else
			strcpy((char*)GBIMW.upscreenStr, pis->fullInputString);
#endif					
			GBIMW.shouldUpScreen = 1;
			GBIMW.isShowCandWin =0;
			memset(GBIMW.fullOutput,0,sizeof(GBIMW.fullOutput));
		}
		
		EBInputMethodInit(pis);		
		GBIMW.isSelectedCand =0;
		GBIMW.noHandleKeyEvent =0;
		
	}
	else if(EBIsInterpunctionMode(pis) ==EB_OK)
	{
		
	}
	else
	{
	  if(IsGBAssociateMode())
	  {
	     return;
	  }
		EBInputReset(pis);
		GBIMW.noHandleKeyEvent =0;	
		GBIMW.isShowCandWin =0;
	}
}

int PYCanPageDown(GBIM *p,void *param)
{
	EBInputStruct *pis =(EBInputStruct *)(p->pIS);
	if(p->pSwitchItem)
	{
		return ((GBIM*)(p->pSwitchItem))->GBIMCanPageDown(p->pSwitchItem,param);
	}
	return EBCanPageDown(pis) == EB_OK;
}

static void PYOnDown(GBIM *p,void *param)
{
	EBInputStruct *pis =0;
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.noHandleKeyEvent =1;
		GBIMW.isShowCandWin =0;
		GBIMW.isSelectedCand =0;
		GBIMW.shouldUpScreen =0;
		return;
	}
	
	if(p->pSwitchItem)
	{
		((GBIM*)(p->pSwitchItem))->GBIMOnDown(p->pSwitchItem,param);
		return;
	}
 
	if(GBIMW.isSelectedCand == 0 && 
	   (GBIMW.gbImeMode == PT_PINYIN_MODE||
	    GBIMW.gbImeMode == HW_ASSOC_MODE) //&& 
	   /*IsGBAssociateMode() == FALSE*/ )
	{
	   GBIMW.isSelectedCand = 1;
	}

	if(GBIMW.isSelectedCand)
	{
		
		
		pis =(EBInputStruct *)(p->pIS);
		if(EBCanPageDown(pis) == EB_OK)
		{
			//根据状态获得下一页候选字
			EBNextPageC(pis);
			if(GBIMW.selectIndex > pis->candidateLen-1)
				GBIMW.selectIndex =pis->candidateLen-1;
		}
		else
		{
			if(EBCanPageUp(pis) !=EB_OK)
			{
			  if(GBIMW.gbImeMode == PT_PINYIN_MODE || GBIMW.gbImeMode == HW_ASSOC_MODE)
			  {
			     return;
			  }
				EBInputReset(pis);
				//GBIMW.noHandleKeyEvent =1;	
				GBIMW.noHandleKeyEvent =0;	
				GBIMW.isShowCandWin =0;	
				GBIMW.isSelectedCand =0;
				GBIMW.selectIndex =0;
			}
			//printf("已经是最后一页\r\n");
		}	
	}
	else
	{
		GBIMW.isSelectedCand =1;
		GBIMW.selectIndex =0;
	}
}

static void PYOnBack(GBIM *p,void *param)
{
	EBInputStruct *pis =0;
	
	if(p->pSwitchItem)
	{
		((GBIM*)(p->pSwitchItem))->GBIMOnBack(p->pSwitchItem,param);
		return;
	}
	
	pis =(EBInputStruct *)(p->pIS);
	
    //if(GBIMW.isShowCandWin == 0) //Modified by hcy 08.04.28 减少一次c键消除面板
	if(GBIMW.isShowCandWin == 0 || pis->inputStringLen == 0)
    {
		GBIMW.isSelectedCand = 0;
		GBIMW.noHandleKeyEvent = 1;
		GBIMW.shouldUpScreen = 0;
    }
	else if(GBIMW.isSelectedCand)
	{
		GBIMW.isSelectedCand =0;
		GBIMW.noHandleKeyEvent =0;
		GBIMW.isShowCandWin =1;
		GBIMW.shouldUpScreen =0;
	}
	else
	{

		if(pis->inputMethod ==1 
				&& pis->inputString[0] =='1'
				&& pis->inputStringLen >0)
		{
			pis->inputString[--pis->inputStringLen] =0;
			pis->fullInputString[pis->inputStringLen] =0;
			return;
		}

		
		if(EBHelperCancelSelect(pis) == EB_OK)
		{
			EBFindFirstPageC(pis);
			GBIMW.selectIndex =0;
		}
		else if(EBHelperDeleteInputChar(pis) == EB_OK)
		{
			if(pis->inputStringLen > 0)
			{
				pis->syllableIndex =0;
				GBIMW.syllableSelIndex =pis->syllableIndex;
				EBFindFirstPageC(pis);			
			}
			else
			{
				//如果当前输入为空,则可以重置输入法来获得高频字
				//here, modified by list.2007-1-23//不作普遍联想
				if(GBIMW.gbImeMode != KEY_PINYIN_MODE)
				{
				  EBAssocFirstPageC(pis);				
                }
			  else
			  {
    			 EBInputReset(pis);
    			 GBIMW.noHandleKeyEvent =0;
    			 GBIMW.shouldUpScreen =0;
    			 GBIMW.isShowCandWin = 0;//关闭候选
			  }
			}
			GBIMW.selectIndex =0;
		}
		else
		{
			EBInputReset(pis);
			GBIMW.noHandleKeyEvent =0;
			GBIMW.shouldUpScreen =0;
			GBIMW.isShowCandWin = 0;//关闭候选
		}
	}
}

/*
modify by abing : 处理一个音节都放不下的情况
*/
static  void GetHeaderIndex(int *pb,int *pe,int beginpos,int split,void *p) 
{
	EBInputStruct *pis =(EBInputStruct *)(((GBIM*)p)->pIS);
	int getrange =0;
	int realroom =GBIMW.syllableAreaWth;
	int width =0;
	int totalwidth =0;
	int pos =beginpos;
	int i=0;
	
	*pb =*pe =0;
	for(i =0;i <pis->syllableLen+1;)
	{	
		int sizecx =(i ==0 )? 
			GBIMW.pfGetWStrLth(GBIMW.fullOutput)
			: GBIMW.pfGetStrLth(EBIS_GET_SYLLABLE(pis,i-1));		
		
		if(width+sizecx+split >realroom)
		{
			if(sizecx+split >=realroom)
			{
				if(pis->syllableIndex <=i)
				{
					*pb = pis->syllableIndex;
                    *pe = pis->syllableIndex + 1;
					break;//header too long ,simple return
				}
				else 
				{
					i++;
					goto AGAIN;
				}
			}
			if(i > 0 && !getrange)	
			{
				*pe =i-1;
				if(pis->syllableIndex <=*pe) getrange =1;								
			}
AGAIN:		if(!getrange) *pb=i;
			width =0;
			pos =beginpos;//
		}
		else
		{
			totalwidth +=sizecx+split;
			width +=sizecx+split;
			
			pos+=sizecx+split;
			i++;
			if(i ==pis->syllableLen+1 && *pe<*pb) *pe =i-1;
		}
	}
	if(totalwidth <=realroom && i>0) *pe =i-1;
	
	//store syllablle
	for(i =*pb ; i <=*pe; i++)
	{
		if( i ==0)
			GBIMW.syllable[0] =GBIMW.fullOutput;
		else
			GBIMW.syllable[i-*pb] =EBIS_GET_SYLLABLE(pis,i-1);
	}
	
	if(pis->inputStringLen > 0)
	{
		GBIMW.syllableNum =*pe -*pb +1;
		GBIMW.syllableSelIndex =pis->syllableIndex-*pb;
	}
	else
	{
		GBIMW.syllableNum =0;
	}
	
	return ;
}

static void PYPrepareShowMessage(GBIM *p,void* param)
{
	
	EBInputStruct *pis =0;
	
	if(GBIMW.isShowCandWin ==0)
		return;
	
	pis =(EBInputStruct *)(p->pIS);
	
	
	if(p->pSwitchItem)
	{
		
		if(pis->inputMethod !=EB_INPUT_METHOD_PINYIN)
		{
			memset(GBIMW.components,0,sizeof(GBIMW.components));
			memset(GBIMW.stroke,0,sizeof(GBIMW.stroke));
		}
		((GBIM*)(p->pSwitchItem))->GBIMPrepareShowMessage(p->pSwitchItem,param);
		return;
	} 
	
	//pis =(EBInputStruct *)(p->pIS);
	
	for(GBIMW.candsNum=0;
	GBIMW.candsNum<pis->candidateLen;
	GBIMW.candsNum++)
	{
		memset(
			GBIMW.cands[GBIMW.candsNum],
			0,
			sizeof(GBIMW.cands[GBIMW.candsNum])
			);
		memcpy( 
			GBIMW.cands[GBIMW.candsNum],
			pis->candidates+pis->cis[GBIMW.candsNum].start,
			pis->cis[GBIMW.candsNum].len*sizeof(unsigned short)
			);		
	}
	
	memset(GBIMW.cands[GBIMW.candsNum],0,
		sizeof(GBIMW.cands[GBIMW.candsNum]));
	
	GBIMW.syllableNum =0;	
	
	//memset(GBIMW.messageUp[GBIMW.syllableNum],0,sizeof(GBIMW.messageUp[GBIMW.syllableNum]));

	if(pis->inputMethod ==1 
			&& pis->inputString[0] =='1')
	{
		//STRNCPY (GBIMW.fullOutput,(const char *)pis->inputString,pis->inputStringLen);
		//gb_strncpy((GBCHAR*)GBIMW.fullOutput, MAX_INPUTLTH, (GBCHAR*)pis->inputString,pis->inputStringLen);
		  STRTOWSTR((char*)pis->inputString, (AECHAR *)GBIMW.fullOutput, MAX_INPUTLTH);
	}
	else if(pis->fullOutputString[0])
	{
		memset(GBIMW.fullOutput,0,sizeof(GBIMW.fullOutput));
		gb_strcpy((wchar_t*)GBIMW.fullOutput,
			(const wchar_t*)pis->fullOutputString);			
	}	
	
	if(pis->inputMethod ==1)//##pinyin
	{
		int begin,end;
		GetHeaderIndex(&begin,&end, 0,GBIMW.syllableSplit,&(GBIMW.GBIMS[GBIMW.activeIM]));	
	}
	else if(pis->inputMethod ==2 || pis->inputMethod ==3 )//##stroke
	{
		int err=0;
		GBIMW.syllableNum =0;		
		err =EBStringToStroke(pis,(char*)GBIMW.stroke,
			sizeof(GBIMW.stroke));
        GBIMW.componentsLth = pis->componentLen;
        memset(GBIMW.components, 0, sizeof(GBIMW.components));
        memcpy(GBIMW.components, pis->components, sizeof(short) * pis->componentLen);
	}
	
	if(pis->candidateLen >0 || pis->inputStringLen >0)
	{
		GBIMW.isShowCandWin =1;
	}
	else
	{
		GBIMW.isShowCandWin =0;
	}
}

static void SmartEnReset(GBIM *p,void *param)
{
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	pis->inputLen =1;
	GBLtReset(pis);	
}

static void SmartEnSwitchIM(GBIM *p,void *param)
{
	//EN_OUTPUT_FCC               首字母大写
	//EN_OUTPUT_CAPITAL           全部字母大写
	//EN_OUTPUT_LOWER             全部字母小写
	
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBEngineChangeIM();
		GBIMW.isSelectedCand =0;
		GBIMW.noHandleKeyEvent =0;
		GBIMW.shouldUpScreen =0;
		return;
	}
	
	if(GBIMW.option == EN_OUTPUT_NORMAL
		|| GBIMW.option == EN_OUTPUT_LOWER)
	{
	/*	
	modifed by xch 2006/06/03 
	note:单个按键的时候,小写大写注意只切换一次
		*/
		if(pis->inputLen == 1)
			GBIMW.option = EN_OUTPUT_CAPITAL;
		else
			GBIMW.option = EN_OUTPUT_TOGGLE_FCC;
		
		GBLtGetDesireOutput(pis,GBIMW.option);
	}
	else if(GBIMW.option == EN_OUTPUT_TOGGLE_FCC)
	{
		GBIMW.option = EN_OUTPUT_CAPITAL;
		GBLtGetDesireOutput(pis,GBIMW.option);
	}
	else if(GBIMW.option == EN_OUTPUT_CAPITAL)
	{
		GBIMW.option = EN_OUTPUT_LOWER;
		GBLtGetDesireOutput(pis,GBIMW.option);
		
		if(GBIMW.activeIM ==0)
		{
			int i=0;
			EBInputStruct *pcnis =(EBInputStruct*)(GBIMW.GBIMS[GBIMW.activeIM].pIS);
			EBInputReset(pcnis);
			
			for(;i<pis->inputLen;i++)
			{
				EBHelperAddInputChar(pcnis,pis->inputString[i]);	
			}
			pcnis->inputStringLen = pis->inputLen;
			pcnis->inputString[pcnis->inputStringLen] = 0;
			EBFindFirstPageC(pcnis);
			GBIMW.GBIMS[GBIMW.activeIM].pSwitchItem = 0;
		}
	}
	
	GBIMW.noHandleKeyEvent =0;
	GBIMW.shouldUpScreen =0;
	return;
}

/*
int SmartEnCanPageUp(GBIM *p,void *param)
{
return GBLtCanPrevHeader((ENInputStruct *)(p->pIS)) == EBEN_OK || GBLtCanPageUp((ENInputStruct *)(p->pIS)) == EBEN_OK;
}
*/
static int SmartEnCanPageUp(GBIM *p,void *param)
{
	return GBLtCanPageUp((ENInputStruct *)(p->pIS)) == EBEN_OK;
}

#ifdef VIETNAM
static void SmartEnOnUp(GBIM *p,void *param)
{ 
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	int ret =0;
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.noHandleKeyEvent =1;
		GBIMW.shouldUpScreen =0;
		GBIMW.isSelectedCand =0;
		return;
	}
	
	if(GBIMW.isSelectedCand)
	{
		///	if(ENIS.inputLen ==1 && ENIS.inputString[0] =='#')
		if(ENIS.inputLen >0)
		{
			if(GBLtCanPageUp(pis) == EBEN_OK)
				GBLtPrevPage(&ENIS);
			if(pis->candidateCount >0)
				GBIMW.selectIndex =pis->candidateCount-1;
			else
				GBIMW.selectIndex =0;
			GBIMW.isShowCandWin =1;
			GBIMW.noHandleKeyEvent =0;
		}	
		else
			GBIMW.isSelectedCand =0;
		
		GBIMW.isShowCandWin =1;
		GBIMW.shouldUpScreen =0;
		GBIMW.noHandleKeyEvent =0;
	}
	else
	{
		//if(ENIS.inputLen ==1 && ENIS.inputString[0] =='#')
		if(ENIS.inputLen >0)
		{
			if(GBLtCanPageUp(pis) == EBEN_OK)
				GBLtPrevPage(&ENIS);
			if(pis->candidateCount >0)
				GBIMW.selectIndex =pis->candidateCount-1;
			else
				GBIMW.selectIndex =0;
			GBIMW.isShowCandWin =1;
			GBIMW.noHandleKeyEvent =0;
		}
		else
		{
			memset(GBIMW.upscreenStr, 0, sizeof(GBIMW.upscreenStr));
			if(pis->inputLen >0)
			{
				assert(pis->inputLen <= EN_INPUT_MAX_LEN);
				//TODO
				//gb_strncpy((char *)GBIMW.upscreenStr, pis->inputString, pis->inputLen);
				
				/*modified*/
				gb_strncpy(GBIMW.upscreenStr, MAX_UPSCREEN, pis->inputString,pis->inputLen);				
				
				GBIMW.upscreenLen = pis->inputLen;
				GBIMW.shouldUpScreen =1;
				memset(GBIMW.fullOutput,0,sizeof(GBIMW.fullOutput));
				if(GBIMW.activeIM ==0)//back to hanzi input
				{
					GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],param);				
				}
			}
			else
				GBIMW.shouldUpScreen =0;
			GBIMW.isShowCandWin =0;		
			GBIMW.noHandleKeyEvent =0;
			SmartEnReset(p, param);
		}
		
	}
}
#else

static void SmartEnOnUp(GBIM *p,void *param)
{
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	int ret =0;
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.noHandleKeyEvent =1;
		GBIMW.shouldUpScreen =0;
		GBIMW.isSelectedCand =0;
		return;
	}
	
	if(GBIMW.isSelectedCand)
	{
		if(ENIS.inputLen >0/*ENIS.inputLen ==1 && ENIS.inputString[0] =='#'*/)
		{
			if(GBLtCanPageUp(pis) == EBEN_OK)
				GBLtPrevPage(&ENIS);
			if(pis->candidateCount >0)
				GBIMW.selectIndex =0/*pis->candidateCount-1*/;// Modified by hcy 08.04.28
			else
				GBIMW.selectIndex =0;
			GBIMW.isShowCandWin =1;
			GBIMW.noHandleKeyEvent =0;
		}
		else 
			if(GBLtCanPrevHeader(&ENIS) == EBEN_OK)
			{
				ret = GBLtPrevHeader(&ENIS);
				if(ret >= 0)
				{
					GBLtFirstPage(&ENIS);
				}
			}
			else
				GBIMW.isSelectedCand =0;
			
			GBIMW.isShowCandWin =1;
			GBIMW.shouldUpScreen =0;
			GBIMW.noHandleKeyEvent =0;
	}
	else
	{
		if(ENIS.inputLen >0/*ENIS.inputLen ==1 && ENIS.inputString[0] =='#'*/)
		{
			if(GBLtCanPageUp(pis) == EBEN_OK)
				GBLtPrevPage(&ENIS);
			if(pis->candidateCount >0)
				GBIMW.selectIndex =pis->candidateCount-1;
			else
				GBIMW.selectIndex =0;
			GBIMW.isShowCandWin =1;
			GBIMW.noHandleKeyEvent =0;
		}
		else 	
			if(GBLtCanPrevHeader(&ENIS) == EBEN_OK)
			{
				ret = GBLtPrevHeader(&ENIS);
				if(ret >= 0)
				{
					GBLtFirstPage(&ENIS);
				}
				GBIMW.isSelectedCand =1;
				GBIMW.isShowCandWin =1;		
				GBIMW.noHandleKeyEvent =0;
				GBIMW.shouldUpScreen =0;
			}
			else
			{
				memset(GBIMW.upscreenStr, 0, sizeof(GBIMW.upscreenStr));
				if(pis->inputLen >0)
				{
					assert(pis->inputLen <= EN_INPUT_MAX_LEN);
					//TODO
					//gb_strncpy((char *)GBIMW.upscreenStr, MAX_UPSCREEN, pis->inputString, pis->inputLen);
					
					/*modified*/
					gb_strncpy(GBIMW.upscreenStr, MAX_UPSCREEN, pis->inputString,pis->inputLen);				
					
					GBIMW.upscreenLen = pis->inputLen;
					GBIMW.shouldUpScreen =1;
					memset(GBIMW.fullOutput,0,sizeof(GBIMW.fullOutput));
					if(GBIMW.activeIM ==0)//back to hanzi input
					{
						GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],param);				
					}
				}
				else
					GBIMW.shouldUpScreen =0;
				GBIMW.isShowCandWin =0;		
				GBIMW.noHandleKeyEvent =0;
				SmartEnReset(p, param);
			}
			
	}
}
#endif

/*
int SmartEnCanPageDown(GBIM *p,void *param)
{
return GBLtCanNextHeader((ENInputStruct *)(p->pIS)) == EBEN_OK || GBLtCanPageDown((ENInputStruct *)(p->pIS)) == EBEN_OK;
}
*/

static int SmartEnCanPageDown(GBIM *p,void *param)
{
	return GBLtCanPageDown((ENInputStruct *)(p->pIS)) == EBEN_OK;
}

#ifdef VIETNAM
static void SmartEnOnDown(GBIM *p,void *param)
{
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.isSelectedCand =0;
		GBIMW.noHandleKeyEvent =1;
		GBIMW.shouldUpScreen =0;
		return;
	}
	
	if(GBIMW.isSelectedCand)
	{
		if(pis->inputLen <=0)
		{
			GBLtReset(pis);
			GBIMW.noHandleKeyEvent =0;
			return;
		}
		
		GBIMW.noHandleKeyEvent =0;
		
		//??
		//if(ENIS.inputLen ==1 && ENIS.inputString[0] =='#')
		if(ENIS.inputLen >0)
		{
			if(GBLtCanPageDown(pis) == EBEN_OK)
				GBLtNextPage(pis);

			GBIMW.selectIndex =0;			
			GBIMW.isShowCandWin =1;
			GBIMW.noHandleKeyEvent =0;
		}
		
		if(!GBIMW.isSelectedCand)
			GBIMW.selectIndex =0;
		else if(GBIMW.selectIndex >pis->candidateCount-1)
			GBIMW.selectIndex =pis->candidateCount-1;
	}
	else
	{
		ENInputStruct *pis=(ENInputStruct*)(p->pIS);
		int ret=0;
		//if(ENIS.inputLen ==1 && ENIS.inputString[0] =='#')
		if(ENIS.inputLen >0)
		{
			if(GBLtCanPageDown(pis) == EBEN_OK)
			{
				GBLtNextPage(pis);
				GBIMW.selectIndex =0;
				GBIMW.isSelectedCand =1;
			}

			GBIMW.isShowCandWin =1;
			GBIMW.noHandleKeyEvent =0;
		}
		
		
		if(!GBIMW.isSelectedCand)
		{
			GBIMW.selectIndex =0;
		}
		else if(GBIMW.selectIndex >pis->candidateCount-1)
		{
			GBIMW.selectIndex =pis->candidateCount-1;
		}
		
		GBIMW.isShowCandWin =1;
		GBIMW.noHandleKeyEvent =0;
	}
}
#else
static void SmartEnOnDown(GBIM *p,void *param)
{
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.isSelectedCand =0;
		GBIMW.noHandleKeyEvent =1;
		GBIMW.shouldUpScreen =0;
		return;
	}
	
	if(GBIMW.isSelectedCand)
	{
		if(pis->inputLen <=0)
		{
			GBLtReset(pis);
			GBIMW.noHandleKeyEvent =0;
			return;
		}
		
		GBIMW.noHandleKeyEvent =0;
		
		//??
		if(ENIS.inputLen >0/*ENIS.inputLen ==1 && ENIS.inputString[0] =='#'*/)
		{
			if(GBLtCanPageDown(pis) == EBEN_OK)
				GBLtNextPage(pis);
			GBIMW.selectIndex =0;			

			if(pis->candidateCount > 0) 
			{
//				GBIMW.isSelectedCand =1;hcy
			}
			else
			{
				GBIMW.isSelectedCand =0;
			}

			GBIMW.isShowCandWin =1;
			GBIMW.noHandleKeyEvent =0;
		}
		else 
			if(GBLtCanNextHeader(pis) == EBEN_OK)
			{
				int ret = GBLtNextHeader(pis);
				if(ret >= 0)
				{
					GBLtFirstPage(pis);
				}
			}
			if(!GBIMW.isSelectedCand)
				GBIMW.selectIndex =0;
			else if(GBIMW.selectIndex >pis->candidateCount-1)
				GBIMW.selectIndex =pis->candidateCount-1;
	}
	else
	{
		ENInputStruct *pis=(ENInputStruct*)(p->pIS);
		int ret=0;
		if(ENIS.inputLen >0/*ENIS.inputLen ==1 && ENIS.inputString[0] =='#'*/)
		{
			if(GBLtCanPageDown(pis) == EBEN_OK)
				GBLtNextPage(pis);
			GBIMW.selectIndex =0;	
			

			if(pis->candidateCount > 0) 
			{
				//GBIMW.isSelectedCand =1;//hcy
			}
			else
			{
				GBIMW.isSelectedCand =0;
 				pis->candidateCount = 1; //hcy
			}

			GBIMW.isShowCandWin =1;
			GBIMW.noHandleKeyEvent =0;
		}
		else 
			if(GBLtCanNextHeader(pis) == EBEN_OK)
			{
				ret = GBLtNextHeader(pis);
				if(ret >= 0)
				{
					GBLtFirstPage(pis);
				}
			}
			
			if(!GBIMW.isSelectedCand)
			{
				if(!pis->headerIsAWord && pis->candidateCount>0
					&& pis->inputLen >1)
					GBIMW.selectIndex =0;//1 wyl
				else
					GBIMW.selectIndex =0;
			}
			else if(GBIMW.selectIndex >pis->candidateCount-1)
			{
				GBIMW.selectIndex =pis->candidateCount-1;
			}
			

			if(pis->candidateCount > 0) 
			{
//				GBIMW.isSelectedCand =1;//hcy
			}
			else
			{
				GBIMW.isSelectedCand =0;
				pis->candidateCount = 1; //hcy
			}

			
			GBIMW.isShowCandWin =1;
			GBIMW.noHandleKeyEvent =0;
	}
}
#endif
static void SmartEnOnLeft(GBIM *p,void *param)
{
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	int ret =0;
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.isSelectedCand =0;
		GBIMW.noHandleKeyEvent =1;
		GBIMW.shouldUpScreen =0;
		return;
	}
	
	if(GBIMW.isSelectedCand)
	{
		if( GBIMW.selectIndex > 0)
		{
			GBIMW.selectIndex--;
		}
		else if(GBLtCanPageUp(pis) == EBEN_OK)
		{
			ret = GBLtPrevPage(pis);
			if(pis->candidateCount >0)
				GBIMW.selectIndex =pis->candidateCount-1;
			else
				GBIMW.selectIndex =0;
			GBIMW.isShowCandWin =1;
			GBIMW.noHandleKeyEvent =0;
		}
		else
		{			
			GBIMW.noHandleKeyEvent =0;
		}		
	}
	else
	{        
		if(pis->candidateCount > 1 && GBIMW.selectIndex >0)
		{
			GBIMW.selectIndex--;
			GBIMW.isSelectedCand =1;
		}
		else
		{
			GBIMW.isSelectedCand =0;
			GBIMW.selectIndex =0;
		}
		
		GBIMW.noHandleKeyEvent =0;
		GBIMW.isShowCandWin =1;
	}
}

static int SmartEnCanPrevHeader(GBIM *p,void *param)
{
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	return GBIMW.selectIndex > 0 || GBLtCanPageUp(pis)==EBEN_OK;
	//return 	GBLtCanPrevHeader((ENInputStruct*)(p->pIS)) == EBEN_OK;
}

static int SmartEnCanNextHeader(GBIM *p,void *param)
{
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	return GBIMW.selectIndex < pis->candidateCount-1 || GBLtCanPageDown(pis)==EBEN_OK;
	//return 	GBLtCanNextHeader((ENInputStruct*)(p->pIS)) == EBEN_OK;
}

static void SmartEnOnRight(GBIM *p,void *param)
{
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	int ret =0;
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.isSelectedCand =0;
		GBIMW.noHandleKeyEvent =1;
		GBIMW.shouldUpScreen =0;
		return;
	}
	
	if(GBIMW.isSelectedCand)
	{
		if( GBIMW.selectIndex < pis->candidateCount-1)
		{
			GBIMW.selectIndex++;
		}
		else if(GBLtCanPageDown(pis) == EBEN_OK)
		{
			ret = GBLtNextPage(pis);
			GBIMW.selectIndex =0;
		}
		
		GBIMW.isShowCandWin =1;
		GBIMW.noHandleKeyEvent =0;
	}
	else
	{
		if(pis->candidateCount > 1)
		{
			GBIMW.selectIndex =1;
			GBIMW.isSelectedCand =1;
		}
		else
			GBIMW.selectIndex =0;

		GBIMW.isShowCandWin =1;
		GBIMW.noHandleKeyEvent =0;
	}
	
}

static void SmartEnOnNumS(GBIM *p,void *param,int realkey)
{
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.isShowCandWin =1;
		GBIMW.isSelectedCand =0;
		GBIMW.shouldUpScreen =0;
	}
	
	if(!GBIMW.isSelectedCand)
	{
        if(pis->inputString[0] == '#')
        {
            SmartEnSelectCandidate(pis, 0, DONT_ADD_SPACE);
			pis->inputString[0]=realkey;	
            pis->inputLen = 1;
        }
        else
        {
			if(pis->inputLen <EN_INPUT_MAX_LEN)
			{
				pis->inputString[pis->inputLen++]=realkey;	
			}
        }
#ifdef VIETNAM
		if(GBLtFirstPage(pis) != EBEN_OK)
#else
			if(GBLtFirstHeader(pis) != EBEN_OK)//##keep for make word
#endif
			{
				;//printf("The number string doesn't match any word!\r\n");
#ifdef __GBSHDATA
				//give a chance to user make word
				GBIMW.candsNum = 0;// 1 modified by hcy new
//del by hcy	gb_strcpy((GBCHAR*)GBIMW.cands[0],okToBeginMakeWordStr);

				
				GBIMW.syllableNum =1;			
				memset(GBIMW.fullOutput,0,sizeof(GBIMW.fullOutput));	
				
				pis->inputLen = 0;//hcy
				memset(pis->inputString, 0, sizeof(pis->inputString));//hcy

				//TODO
				////如果外部依赖GBIMW.fullOutput==GBIMW.syllable[0]判断当前
				//输出是用GBIMW.syllable[0]
				gb_strncpy((GBCHAR*)GBIMW.fullOutput, MAX_INPUTLTH, pis->inputString,pis->inputLen);
				GBIMW.syllable[0] =GBIMW.fullOutput;
				//gb_strncpy((GBCHAR*)GBIMW.syllable[0],pis->inputString,pis->inputLen);
				
				GBIMW.syllableNum =1;
				GBIMW.isCandHelpTips =1;
#else
				GBIMW.candsNum = 0;
#endif				
				GBIMW.isSelectedCand =0;
				GBIMW.noHandleKeyEvent =0;
				GBIMW.shouldUpScreen =0;
				GBIMW.isShowCandWin =1;		
			}
			else 
			{
				GBLtFirstPage(pis);	
				if(!pis->headerIsAWord && pis->candidateCount>0
					&& pis->inputLen >1)
					GBIMW.selectIndex =0;//1 wyl
				else
					GBIMW.selectIndex =0;
					
				GBIMW.isCandHelpTips =0;
			}			
	}
	else
	{
		//if(pis->candidateCount > 0)
		{
			if('0' < realkey && realkey <= ('0' + pis->candidateCount))
			{
				SendHZtoClient((unsigned short*)pis->candidates[realkey-'1'],
					strlen((char*)pis->candidates[realkey-'1']));
				
				SmartEnSelectCandidate(pis, realkey-'1', DONT_ADD_SPACE);
				
				//重置输入法
				SmartEnReset(p, param);
				if(GBIMW.activeIM ==0)//back to hanzi input
				{
					GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],param);	
				}
				GBIMW.isShowCandWin =0;
				GBIMW.selectIndex =0;
			}
			
			GBIMW.isCandHelpTips =0;
		}	
	}	
}

static void SmartEnOnNumL(GBIM *p,void *param,int realkey)
{
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	if(realkey-GB_VK1L+1 <=pis->candidateCount)
	{
        SmartEnSelectCandidate(pis, realkey-GB_VK1L, DONT_ADD_SPACE);
	}
	GBLtReset(pis);
	if(GBIMW.activeIM ==0)//back to hanzi input
	{
		GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],param);	
	}
}

static void SmartEnOnBlank(GBIM *p,void *param)
{
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.noHandleKeyEvent =1;
		GBIMW.shouldUpScreen =0;
		GBIMW.isSelectedCand =0;
		//if(GBIMW.activeIM ==0)//back to hanzi input
		if(GBIMW.activeMode !=GBSmartLatin)//back to hanzi input
		{
			GBIMW.activeIM =0;
			GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],param);	
		}
		return;
	}
	
	if(pis->inputLen>0)
	{
		if(pis->candidateCount >0)
		{
			SmartEnSelectCandidate(pis, GBIMW.selectIndex, ADD_SPACE_ALWAYS);
			GBIMW.noHandleKeyEvent =0;
			GBIMW.option = EN_OUTPUT_LOWER;
			memset(GBIMW.fullOutput,0,sizeof(GBIMW.fullOutput));
		}
		else//switch 2 english make word
		{
#ifdef __GBSHDATA 
			GBIMW.prevIM =GBIMW.activeIM;
			GBIMW.activeIM =4;
			GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],param);	
			
			//设置自造词元素
			GBElementsInit((CHARInputStruct*)(GBIMW.GBIMS[2].pIS),EN_OPTION_DONT_ADJUST_CHAR_ORDER,ELEM_SELFWORD);//初始化大小写混合的字母输入法
			
			((SMInputStruct*)(GBIMW.GBIMS[GBIMW.activeIM].pIS))->status =MW_POWERON;
			((SMInputStruct*)(GBIMW.GBIMS[GBIMW.activeIM].pIS))->penis =pis;//将smart english的pis传入
			//SMFirstPage(&GBIMW.GBIMS[GBIMW.activeIM],pis);
			SMFirstPage(&GBIMW.GBIMS[GBIMW.activeIM],0);
			GBIMW.candsNum = 0; //hcy
#endif
			return;
		}
	}
	
	
	GBLtReset(pis);	
    GBIMW.isShowCandWin = 0;
	
	if(GBIMW.activeMode !=GBSmartLatin)//back to hanzi input
	{
		GBIMW.activeIM =0;
		GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],param);	
	}
}

static void SmartEnOnBack(GBIM *p,void *param)
{
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	
    if(!GBIMW.isShowCandWin)
    {
		GBIMW.isSelectedCand = 0;
		GBIMW.shouldUpScreen = 0;
		GBIMW.noHandleKeyEvent = 1;
		if(GBIMW.activeIM ==0)//back to hanzi input
		{
			GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],param);	
		}
		return;
    }
	
	if(!GBIMW.isSelectedCand)
	{
		if(pis->inputLen>0)
		{
			pis->inputString[--pis->inputLen]=0;
			if(pis->inputLen==0)
			{				
				if(GBIMW.activeIM ==0)//back to hanzi input
				{
					GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],param);	
					//GBIMW.option = EN_OUTPUT_NORMAL;//reset smarten output format
					return;
				}
                else
                {
                    SmartEnReset(p, param);
					GBIMW.noHandleKeyEvent =0;
                    GBIMW.isShowCandWin = 0;
					return;
                }
			}
#ifdef VIETNAM
			if(GBLtFirstPage(pis) != EBEN_OK)
#else
				if(GBLtFirstHeader(pis) != EBEN_OK)
#endif
				{
					GBIMW.syllableNum =1;			
					memset(GBIMW.fullOutput,0,sizeof(GBIMW.fullOutput));
					gb_strncpy((GBCHAR*)GBIMW.fullOutput,MAX_INPUTLTH, (GBCHAR*)pis->inputString,pis->inputLen);			
			
				}
				else
				{
					//if(((SMInputStruct *)GBIMW.GBIMS[4].pIS)->status ==MW_FINISH)
#ifdef __GBSHDATA
					if(((SMInputStruct *)GBIMW.GBIMS[4].pIS)->status == MW_FINISH
						|| ((SMInputStruct *)GBIMW.GBIMS[4].pIS)->status ==MW_POWERON)
					{
						GBIMW.isCandHelpTips =0;
						//((SMInputStruct *)GBIMW.GBIMS[4].pIS)->status = MW_SELECTTING;
					}
#else
					GBIMW.isCandHelpTips =0;
					//((SMInputStruct *)GBIMW.GBIMS[4].pIS)->status = MW_SELECTTING;
#endif
					GBLtFirstPage(pis);
					GBIMW.selectIndex =0;
				}
		}
		else
		{
			GBLtReset(pis);
			GBIMW.noHandleKeyEvent =1;
			GBIMW.isShowCandWin =0;
			return;
		}
		GBIMW.noHandleKeyEvent =0;
		GBIMW.shouldUpScreen =0;
		GBIMW.isSelectedCand =0;
	}
	else
	{
		GBIMW.isSelectedCand =0;
		GBIMW.isShowCandWin =1;
		GBIMW.shouldUpScreen =0;
		GBIMW.noHandleKeyEvent =0;
	}
}

static void SmartEnPrepareShowMessage(GBIM *p,void *param)
{
	ENInputStruct *pis=(ENInputStruct*)(p->pIS);
	unsigned short index=0;
	
	if(GBIMW.isCandHelpTips)
	{
		//GBIMW.syllableNum =0;
		return;
	}
	for(GBIMW.candsNum=0;
	GBIMW.candsNum<pis->candidateCount;
	GBIMW.candsNum++)
	{			
		memset(GBIMW.cands[GBIMW.candsNum],0,
			sizeof(GBIMW.cands[GBIMW.candsNum]));
		index=GBIMW.candsNum;
		//TODO
		
		gb_strcpy((GBCHAR*)(GBIMW.cands[GBIMW.candsNum]),(GBCHAR*)(pis->candidates[GBIMW.candsNum]));							
	}
	
	memset(GBIMW.cands[GBIMW.candsNum],0,
		sizeof(GBIMW.cands[GBIMW.candsNum]));
	
	
	assert(pis->inputLen <= EN_INPUT_MAX_LEN);
	GBIMW.syllableNum =0;	
	memset(GBIMW.fullOutput,0,sizeof(GBIMW.fullOutput));
	
	//TODO
	if(pis->inputLen > 0)
	{
		/*modified*/
		gb_strncpy((GBCHAR*)(GBIMW.fullOutput), MAX_INPUTLTH, pis->inputString,pis->inputLen);
		if(GBIMW.activeMode == GBPinYin)
		{
			//gb_strncpy((GBCHAR*)GBIMW.syllable[0],pis->inputString,pis->inputLen);
			gb_strncpy((GBCHAR*)GBIMW.fullOutput,MAX_INPUTLTH, pis->inputString,pis->inputLen);
			GBIMW.syllable[0] =GBIMW.fullOutput;
			GBIMW.syllableSelIndex =0;
			GBIMW.syllableNum =1;				
		}
	}
	*((GBCHAR*)GBIMW.fullOutput + pis->inputLen) =0;
}

//static int SelectCandidate(EBInputStruct * is, int index,IMForwardEventStruct *call_data)
static  int SelectCandidate(EBInputStruct * is, int index)
{
    int len ;
    int ret;
	unsigned short upchar[256];
	
    if(index < 0 || index >= is->candidateLen)
    {
        return 0;
    }
    if((ret = EBHelperSelect(is, (unsigned short)index)) < 0)
    {
        return 0;
    }
    else if(ret == 0)
    {		
       	len = WSTRLEN(is->fullOutputString);
		
		memset(upchar,0,sizeof(upchar));
        memcpy(upchar, is->fullOutputString, 
            len * sizeof(unsigned short));
		
		SendHZtoClient(upchar,len);
        GBIMW.isSelectedCand = 0;
		
        //historyISNLen += len;		
        //用户每输入一个汉字都应该调用EBAssocAddHistoryISN, 以更新联想词
        EBAssocAddHistoryISN(is, is->fullOutputString, len);
		if(is->selCount > 1)
		{
			//加入自造词，is->fullInputString为对应的输入串，可以为数字串，也可以为字母串
			EBShAddPhrase(is->fullOutputString, is->fullInputString);
		}
        //表示开始重新输入词语
        EBHelperEBISRestart(is);
        //获得联想词
        EBAssocFirstPageC(is);	
		
		return CN_WORD_FINISH;
		
    }
    else
    {
        //上次输入的字符串还没有选择完
        EBFindFirstPageC(is);
		return CN_WORD_MAKING;
    }    
	
    return 1;
}

static void GBEngineInit(void* param)
{
	int iret = -1;
	GBIME_INIT *pInit =(GBIME_INIT *)param;
	CHARInputStruct* pcharis =0;
	
	memcpy(&(GBIMW.gbSet),&(pInit->gbSetting),sizeof(GBSetting));
	
	
	iret = EBStartup(pInit->gbChnEngineInit.pEngineData);
	assert(iret == EB_OK);
	memset(GBIMW.GBIMS[0].pIS,0,sizeof(EBInputStruct));
	
	EBIS.inputMethod =pInit->gbChnEngineInit.nInputMethod;
	EBIS.candidateDesireLen =pInit->gbChnEngineInit.nCandDesireLenInWord;
	EBIS.interpunctionDesireLen =pInit->gbChnEngineInit.nInterpunctionDesireLen;
	EBInputMethodInit(&EBIS);
	
	EBShDataInit(pInit->gbChnEngineInit.pSelfMadeData, pInit->gbChnEngineInit.nSelfMadeDataLength, 
        2, 2, pInit->gbChnEngineInit.fpOnSelfMadeDataChanged);
	
	memset(&ENIS,0,sizeof(ENInputStruct));
	ENIS.candidateDesireLen = pInit->gbLtEngineInit.nCandDesireLenInWord;
	ENIS.candidateDesireCount = pInit->gbLtEngineInit.nCandDesireCountInWord;
	GBLtStartup(pInit->gbLtEngineInit.pEngineData);
	
	GBLtSelfHelpWordsInit(pInit->gbLtEngineInit.pSelfMadeData, 
		pInit->gbLtEngineInit.nSelfMadeDataLength, 
		pInit->gbLtEngineInit.fpOnSelfMadeDataChanged,
		0);
	
#ifdef DIFF_CAPITAL
	pcharis =(CHARInputStruct*)(GBIMW.GBIMS[5].pIS);
	GBElementsInit(pcharis,EN_OPTION_DONT_ADJUST_CHAR_ORDER | EN_OPTION_OUTPUT_CAPITAL_CHAR,ELEM_CHARWORD);//初始化大写的字母输入法
#else
	pcharis =(CHARInputStruct*)(GBIMW.GBIMS[2].pIS);
	GBElementsInit(pcharis,EN_OPTION_DONT_ADJUST_CHAR_ORDER,ELEM_CHARWORD);//初始化大小写混合的字母输入法
#endif
	
	
	GBIMW.isShowCandWin =0;
	GBIMW.noHandleKeyEvent =1;
	GBIMW.selectIndex =0;
}

static void GBSetActiveIM(GBInputMode mode);

int HandleKeyEvent(int realkey,unsigned long param)
{
	GBIMW.shouldUpScreen =0;
	switch(realkey)
	{			
	case GB_VK_BACK:
		GBIMW.GBIMS[GBIMW.activeIM].GBIMOnBack(
			&(GBIMW.GBIMS[GBIMW.activeIM]),0);
		if(GBIMW.noHandleKeyEvent)
		{					
			GBIMW.noHandleKeyEvent =0;
			return 0;
		}
		break;
	case GB_VK_RETURN:
		GBIMW.GBIMS[GBIMW.activeIM].GBIMOnReturn(
			&(GBIMW.GBIMS[GBIMW.activeIM]),0);
		GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(
			&(GBIMW.GBIMS[GBIMW.activeIM]),0);
		if(GBIMW.noHandleKeyEvent)
		{					
			GBIMW.noHandleKeyEvent =0;
			return 0;
		}	
	case GB_VK_OK:
		GBIMW.GBIMS[GBIMW.activeIM].GBIMOnBlank(
			&(GBIMW.GBIMS[GBIMW.activeIM]),0);	
		if(GBIMW.noHandleKeyEvent)
		{					
			GBIMW.noHandleKeyEvent =0;
			return 0;
		}	
		break;
	case GB_VK_UP:
		GBIMW.GBIMS[GBIMW.activeIM].GBIMOnUp(
			&(GBIMW.GBIMS[GBIMW.activeIM]),0);	
		if(GBIMW.noHandleKeyEvent)
		{					
			GBIMW.noHandleKeyEvent =0;
			return 0;
		}	
		break;
	case GB_VK_DOWN:			
		GBIMW.GBIMS[GBIMW.activeIM].GBIMOnDown(
			&(GBIMW.GBIMS[GBIMW.activeIM]),0);
		if(GBIMW.noHandleKeyEvent)
		{					
			GBIMW.noHandleKeyEvent =0;
			return 0;
		}				
		break;
	case GB_VK_LEFT:
		GBIMW.GBIMS[GBIMW.activeIM].GBIMOnLeft(
			&(GBIMW.GBIMS[GBIMW.activeIM]),0);
		if(GBIMW.noHandleKeyEvent)
		{					
			GBIMW.noHandleKeyEvent =0;
			return 0;
		}			
		break;
	case GB_VK_RIGHT:
		GBIMW.GBIMS[GBIMW.activeIM].GBIMOnRight(
			&(GBIMW.GBIMS[GBIMW.activeIM]),0);
		if(GBIMW.noHandleKeyEvent)
		{					
			GBIMW.noHandleKeyEvent =0;
			return 0;
		}			
		break;
	case '*'://##无缝切换
		GBIMW.GBIMS[GBIMW.activeIM].GBIMSwitchIM(
			&(GBIMW.GBIMS[GBIMW.activeIM]),0);
		if(GBIMW.noHandleKeyEvent)
		{					
			GBIMW.noHandleKeyEvent =0;
			return 0;
		}
		break;			
	case '#':
		GBIMW.GBIMS[GBIMW.activeIM].GBIMOnSharp(
			&(GBIMW.GBIMS[GBIMW.activeIM]),0);
		if(GBIMW.noHandleKeyEvent)
		{					
			GBIMW.noHandleKeyEvent =0;
			return 0;
		}
		break;		
	case CFG_SYLLABLE_WTH:
		GBIMW.syllableAreaWth =param;
		break;
	case CFG_GETSTREXTENT_FUNC:
		GBIMW.pfGetStrLth =(pfGetStrLth_T)param;
		break;
	case CFG_GETWSTREXTENT_FUNC:
		GBIMW.pfGetWStrLth =(pfGetStrLth_T)param;
		break;
	case CFG_INITENGINE:
		GBEngineInit((void*)param);
		break;
	case CFG_SETACTIVEIM:
		GBSetActiveIM(param);
		break;
	case CFG_RESETACTIVEIM:
		GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(
			&(GBIMW.GBIMS[GBIMW.activeIM]),0);
		break;
	case CFG_SYLLABLE_SPLIT:
		GBIMW.syllableSplit =param;
		break;
	case CFG_SETCHINESE_LIMIT:
		GBIMW.isChineseOnly =param;
		break;
	case CFG_ISCANCHANGEIM:
		GBIMW.isCanChangeIM =param;
		break;
	case CFG_SETENGLISH_LIMIT:
		GBIMW.isEnglishOnly =param;
		break;
	case CFG_SELECT_SYLLABLE:
		PYSetSyllableIndex(&GBIMW.GBIMS[GBIMW.activeIM],param);
		break;
  //新增项用于输入输入汉字，得到相关联想词
  case CFG_GET_ASSOCIATE:
		GetAssociate((unsigned short *)param);
		break;
		
	default:
		if( (GB_VK0<=realkey && realkey<=GB_VK9)
			||('A'<=realkey && realkey<='Z')
			||('a'<=realkey && realkey<='z'))
		{
			GBIMW.GBIMS[GBIMW.activeIM].GBIMOnNumS(
				&(GBIMW.GBIMS[GBIMW.activeIM]),0,realkey);	
		}
		else if(GB_VK1L<=realkey && realkey<=GB_VK9L)
		{
			GBIMW.GBIMS[GBIMW.activeIM].GBIMOnNumL(
				&(GBIMW.GBIMS[GBIMW.activeIM]),0,realkey);
		}
		else 
		{							
			return 0;
		}
		break;
		
	}		
	
	GBIMW.GBIMS[GBIMW.activeIM].GBIMPrepareShowMessage(
		&(GBIMW.GBIMS[GBIMW.activeIM]),0);	
	
	return 1;
}

static int SendHZtoClient(unsigned short *upchar,int lth)
{
	int i =0;
	for(i=0;i<lth;i++)
	{
		GBIMW.upscreenStr[i] =upchar[i];
	}
	
	GBIMW.upscreenStr[i] =0;
	GBIMW.upscreenLen =lth;
	GBIMW.shouldUpScreen =1;
	
	return 1;
}

/*
\fn int SmartEnSelectCandidate(ENInputStruct * enis, int index, int addSpaceAfterWord);
\brief 英文选字上屏
\param enis 英文交换区
\param index 候选下标
\param addSpaceAfterWord 在词语后面加入空格
DONT_ADD_SPACE          : 不要增加空格
ADD_SPACE_ALWAYS        : 对于所有的候选都增加空格
ADD_SPACE_AFTER_WORD    : 只对单词增加空格，对字母不增加空格
\return    0 : failed 
1 : succeed

*/
//#define DONT_ADD_SPACE              0
//#define ADD_SPACE_ALWAYS            1
//#define ADD_SPACE_AFTER_WORD        2
static int SmartEnSelectCandidate(ENInputStruct * enis, int index, int addSpaceAfterWord)
{
    const GBCHAR * pSource;
    
    GBCHAR * pTarget;
    if(index < 0 || index >= enis->candidateCount)
    {
        return 0;
    }
    pSource = enis->candidates[index];
    memset(GBIMW.upscreenStr, 0, sizeof(GBIMW.upscreenStr));
    pTarget = (GBCHAR *)GBIMW.upscreenStr;
    while(*pSource)
    {
        *pTarget++ = *pSource++;
    }
    //自动加空格
    if(addSpaceAfterWord == ADD_SPACE_ALWAYS || 
        (pSource - enis->candidates[index] > 1 && addSpaceAfterWord == ADD_SPACE_AFTER_WORD))
    {
        //标点模式下面，只有规定的标点后面才自动加空格
        if(enis->inputString[0] == '#')
        {
			//			if(gb_strstr(".!?:", enis->candidates[index]) != NULL)
			if(gb_strchr(enis->candidates[index],(GBCHAR)'.') != NULL
				|| gb_strchr(enis->candidates[index],(GBCHAR)':') != NULL
				|| gb_strchr(enis->candidates[index],(GBCHAR)'!') != NULL
				|| gb_strchr(enis->candidates[index],(GBCHAR)'?') != NULL)
			{
				*pTarget++ = ' ';
			}
			
        }
        else //一般的候选词语自动加空格
        {
            *pTarget++ = ' ';
        }
    }
    *pTarget = 0;
    GBIMW.upscreenLen = pTarget - ((GBCHAR *)GBIMW.upscreenStr);
    GBIMW.shouldUpScreen = 1;
    return 1;
}

#ifdef _SHOW_DEBUG

static void PYDisplay(GBIM *p,void* param)
{
	char buf[2048];
	EBInputStruct *pis =0;	
	int candsNum =0;
	
	if(p->pSwitchItem)
	{
		((GBIM*)(p->pSwitchItem))->GBIMDisplay(p->pSwitchItem,param);
		return;
	}
	
	pis =(EBInputStruct *)(p->pIS);
	
	for(candsNum=0;
	candsNum<pis->candidateLen;
	candsNum++)
	{
		memset(buf,0,sizeof(buf));
		
		/*WideCharToMultiByte(CP_GB, 0, 
		GBIMW.cands[candsNum], 
		gb_strlen(GBIMW.cands[candsNum]) + 1,
		buf, sizeof(buf) - 1, NULL, NULL);*/
		
		printf("%s ",GBIMW.cands[candsNum]);
		//printf("%s ",buf);	
	}
	
	printf("\n");
	return;
}

static void SmartEnDisplay(GBIM *p,void *param)
{
	char buf[2048];
	int candsNum =0;
	ENInputStruct *pis =0;
	
	pis =(ENInputStruct *)(p->pIS);
	
	for(candsNum=0;
	candsNum<pis->candidateCount;
	candsNum++)
	{
		memset(buf,0,sizeof(buf));
		printf("%s ",GBIMW.cands[candsNum]);	
	}
	printf("\n");
}

#endif

#ifndef DIFF_CAPITAL
static void GBSetActiveIM(GBInputMode mode)
{
	
	GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&(GBIMW.GBIMS[GBIMW.activeIM]),0);
	
	switch(mode)
	{
	case GBIMNull:
		GBIMW.activeIM =3;
		GBIMW.activeMode =GBIMNull;
		GBIMW.isShowCandWin =0;
		break;
		
	case GBPinYin:
		GBIMW.activeIM =0;
		GBIMW.activeMode =GBPinYin;
		( (EBInputStruct*) GBIMW.GBIMS[GBIMW.activeIM].pIS )->inputMethod =1;
		GBIMW.isSelectedCand =0;
		GBIMW.isCandHelpTips =0;
		GBIMW.isShowCandWin =0;
		break;
		
	case GBStroke:
		GBIMW.activeIM =0;
		GBIMW.activeMode =GBStroke;
		( (EBInputStruct*) GBIMW.GBIMS[GBIMW.activeIM].pIS )->inputMethod =2;
		GBIMW.isSelectedCand =0;
		GBIMW.isShowCandWin =0;
		break;
		
	case GBYinStroke:
		GBIMW.activeIM =0;
		GBIMW.activeMode =GBYinStroke;
		( (EBInputStruct*) GBIMW.GBIMS[GBIMW.activeIM].pIS )->inputMethod =3;
		GBIMW.isSelectedCand =0;
		GBIMW.isShowCandWin =0;
		break;
		
	case GBSmartLatin:
		GBIMW.activeIM =1;
		GBIMW.activeMode =GBSmartLatin;
		GBIMW.isSelectedCand =0;
		GBIMW.isShowCandWin =0;
		break;
		
	case GBCharacter:
		GBIMW.activeIM =2;
		GBIMW.activeMode =GBCharacter;
		GBIMW.isSelectedCand =1;
		GBIMW.isShowCandWin =0;
		break;
		
	case GBNumber:
		GBIMW.activeIM =3;
		GBIMW.activeMode =GBNumber;
		GBIMW.isSelectedCand =0;
		GBIMW.isShowCandWin =0;
		break;
		
	default:
		break;
	}
	
	GBIMW.isCandHelpTips =0;
	GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&(GBIMW.GBIMS[GBIMW.activeIM]),0);
}

static void GBEngineChangeIM()
{
	if(!GBIMW.isCanChangeIM)
		return;
	GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],0);
	switch(GBIMW.activeMode)
	{
	case GBIMNull:
		GBIMW.activeIM =0;//change to pinyin
		GBIMW.activeMode =GBPinYin;//1
		((EBInputStruct*)GBIMW.GBIMS[GBIMW.activeIM].pIS)->inputMethod =1;
		GBIMW.isShowCandWin =1;
		break;
	case GBPinYin:
		GBIMW.activeIM =0;//change to stroke
		GBIMW.activeMode =GBStroke;//2
		((EBInputStruct*)GBIMW.GBIMS[GBIMW.activeIM].pIS)->inputMethod =2;
		GBIMW.isShowCandWin =1;
		break;
	case GBStroke:
		GBIMW.activeIM =0;//change to yin stroke
		GBIMW.activeMode =GBYinStroke;//3
		((EBInputStruct*)GBIMW.GBIMS[GBIMW.activeIM].pIS)->inputMethod =3;
		GBIMW.isShowCandWin =1;
		break;
	case GBYinStroke:
		if(GBIMW.isChineseOnly)
		{
			GBIMW.activeIM =0;//change to pinyin
			GBIMW.activeMode =GBPinYin;//1
			((EBInputStruct*)GBIMW.GBIMS[GBIMW.activeIM].pIS)->inputMethod =1;
			GBIMW.isShowCandWin =1;
		}
		else
		{
			GBIMW.activeIM =1;//change to smart english
			GBIMW.activeMode =GBSmartLatin;
			GBIMW.isShowCandWin =0;
		}
		break;
	case GBSmartLatin:
		GBIMW.activeIM =2;//change to char
		GBIMW.activeMode =GBCharacter;
		GBIMW.isShowCandWin =0;
		break;
	case GBCharacter:
		if(GBIMW.isEnglishOnly)
		{
			GBIMW.activeIM =1;//change to smart english
			GBIMW.activeMode =GBSmartLatin;
		}
		else
		{
			GBIMW.activeIM =3;//change to number
			GBIMW.activeMode =GBNumber;
		}
		GBIMW.isShowCandWin =0;
		break;
	case GBNumber:
		GBIMW.activeIM =0;//change to pinyin
		GBIMW.activeMode =GBPinYin;//1
		((EBInputStruct*)GBIMW.GBIMS[GBIMW.activeIM].pIS)->inputMethod =1;
		GBIMW.isShowCandWin =1;
		break;   
	}
	
	GBIMW.isSelectedCand =0;
	GBIMW.isCandHelpTips =0;
	GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],0);
}
#else

static void GBSetActiveIM(GBInputMode mode)
{
	
	GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&(GBIMW.GBIMS[GBIMW.activeIM]),0);
	
	switch(mode)
	{
	case GBIMNull:
		GBIMW.activeIM =3;
		GBIMW.activeMode =GBIMNull;
		GBIMW.isShowCandWin =0;
		break;
		
	case GBPinYin:
		GBIMW.activeIM =0;
		GBIMW.activeMode =GBPinYin;
		( (EBInputStruct*) GBIMW.GBIMS[GBIMW.activeIM].pIS )->inputMethod =1;
		GBIMW.isSelectedCand =0;
		GBIMW.isCandHelpTips =0;
		GBIMW.isShowCandWin =0;
		break;
		
	case GBStroke:
		GBIMW.activeIM =0;
		GBIMW.activeMode =GBStroke;
		( (EBInputStruct*) GBIMW.GBIMS[GBIMW.activeIM].pIS )->inputMethod =2;
		GBIMW.isSelectedCand =0;
		GBIMW.isShowCandWin =0;
		break;
		
	case GBYinStroke:
		GBIMW.activeIM =0;
		GBIMW.activeMode =GBYinStroke;
		( (EBInputStruct*) GBIMW.GBIMS[GBIMW.activeIM].pIS )->inputMethod =3;
		GBIMW.isSelectedCand =0;
		GBIMW.isShowCandWin =0;
		break;
		
	case GBSmartLatin:
		GBIMW.activeIM =1;
		GBIMW.activeMode =GBSmartLatin;
		GBIMW.isSelectedCand =0;
		GBIMW.isShowCandWin =0;
		break;
		
	case GBCAPITALCharacter:
		GBIMW.activeIM =5;//change to lower char
		GBIMW.activeMode =GBCAPITALCharacter;
		GBIMW.isShowCandWin =0;
		
		GBElementsInit((CHARInputStruct*)(GBIMW.GBIMS[2].pIS),
			EN_OPTION_DONT_ADJUST_CHAR_ORDER | EN_OPTION_OUTPUT_CAPITAL_CHAR,ELEM_CHARWORD);//初始化小写的字母输入法
		break;
	case GBLOWERCASECharacter:
		GBIMW.activeIM =6;//change to lower char
		GBIMW.activeMode =GBLOWERCASECharacter;
		GBIMW.isShowCandWin =0;
		
		GBElementsInit((CHARInputStruct*)(GBIMW.GBIMS[2].pIS),
			EN_OPTION_DONT_ADJUST_CHAR_ORDER | EN_OPTION_OUTPUT_LOWER_CHAR,ELEM_CHARWORD);//初始化小写的字母输入法
		break;
		
	case GBNumber:
		GBIMW.activeIM =3;
		GBIMW.activeMode =GBNumber;
		GBIMW.isSelectedCand =0;
		GBIMW.isShowCandWin =0;
		break;
		
	default:
		break;
	}
	
	GBIMW.isCandHelpTips =0;
	GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&(GBIMW.GBIMS[GBIMW.activeIM]),0);
}

static void GBEngineChangeIM()
{
	if(!GBIMW.isCanChangeIM)
		return;
	GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],0);
	switch(GBIMW.activeMode)
	{
	case GBIMNull:
		GBIMW.activeIM =0;//change to pinyin
		GBIMW.activeMode =GBPinYin;//1
		((EBInputStruct*)GBIMW.GBIMS[GBIMW.activeIM].pIS)->inputMethod =1;
		GBIMW.isShowCandWin =1;
		break;
	case GBPinYin:
		GBIMW.activeIM =0;//change to stroke
		GBIMW.activeMode =GBStroke;//2
		((EBInputStruct*)GBIMW.GBIMS[GBIMW.activeIM].pIS)->inputMethod =2;
		GBIMW.isShowCandWin =1;
		break;
	case GBStroke:
		GBIMW.activeIM =0;//change to yin stroke
		GBIMW.activeMode =GBYinStroke;//3
		((EBInputStruct*)GBIMW.GBIMS[GBIMW.activeIM].pIS)->inputMethod =3;
		GBIMW.isShowCandWin =1;
		break;
	case GBYinStroke:
		if(GBIMW.isChineseOnly)
		{
			GBIMW.activeIM =0;//change to pinyin
			GBIMW.activeMode =GBPinYin;//1
			((EBInputStruct*)GBIMW.GBIMS[GBIMW.activeIM].pIS)->inputMethod =1;
			GBIMW.isShowCandWin =1;
		}
		else
		{
			GBIMW.activeIM =1;//change to smart english
			GBIMW.activeMode =GBSmartLatin;
			GBIMW.isShowCandWin =0;
		}
		break;
	case GBSmartLatin:
		GBIMW.activeIM =5;//change to capital char
		GBIMW.activeMode =GBCAPITALCharacter;
		GBIMW.isShowCandWin =0;
		
		GBElementsInit((CHARInputStruct*)(GBIMW.GBIMS[2].pIS),
			EN_OPTION_DONT_ADJUST_CHAR_ORDER | EN_OPTION_OUTPUT_CAPITAL_CHAR,ELEM_CHARWORD);//初始化大写的字母输入法
		break;
	case GBCAPITALCharacter:
		GBIMW.activeIM =6;//change to lower char
		GBIMW.activeMode =GBLOWERCASECharacter;
		GBIMW.isShowCandWin =0;
		
		GBElementsInit((CHARInputStruct*)(GBIMW.GBIMS[2].pIS),
			EN_OPTION_DONT_ADJUST_CHAR_ORDER | EN_OPTION_OUTPUT_LOWER_CHAR,ELEM_CHARWORD);//初始化小写的字母输入法
		break;
	case GBLOWERCASECharacter:
		if(GBIMW.isEnglishOnly)
		{
			GBIMW.activeIM =1;//change to smart english
			GBIMW.activeMode =GBSmartLatin;
		}
		else
		{
			GBIMW.activeIM =3;//change to number
			GBIMW.activeMode =GBNumber;
		}
		GBIMW.isShowCandWin =0;
		break;
	case GBNumber:
		GBIMW.activeIM =0;//change to pinyin
		GBIMW.activeMode =GBPinYin;//1
		((EBInputStruct*)GBIMW.GBIMS[GBIMW.activeIM].pIS)->inputMethod =1;
		GBIMW.isShowCandWin =1;
		break;   
	}
	
	GBIMW.isSelectedCand =0;
	GBIMW.isCandHelpTips =0;
	GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],0);
}
#endif
//字母输入法

static void CharOnStar(GBIM *p,void *param)
{	
	//	GBEngineChangeIM();	
}

static int CharCanPageUp(GBIM *p,void* param)
{
	CHARInputStruct *pcharis =(CHARInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		//GBIMW.noHandleKeyEvent =1;
		return 0;
	}
	
	return pcharis->curSeledCandIndex / CHARACTER_IM_CANDIDATES_MAX - 0;
}

static int GetCharSHCandIndex(CHARInputStruct *pcharis,int realkey)
{
	int index =0;
	KeyCandidates *p =pcharis->CharCandidateArray;
	
	while(p->key && p->key != realkey)
	{
		p++;
	}
	
	if(p->key)
	{
		index =p -pcharis->CharCandidateArray;
	}
	else 
	{
		index =0;
	}
	return index;
}

static int GetCharCandIndex(CHARInputStruct *pcharis,int realkey)
{
	int index =0;
	KeyCandidates *p =pcharis->CharCandidateArray;
	
	while(p->key && p->key != realkey)
	{
		p++;
	}
	
	if(p->key)
	{
		index =p -pcharis->CharCandidateArray;
	}
	else 
	{
		index =0;
	}
	return index;
}

static int CharCanPageDown(GBIM *p,void* param)
{
	CHARInputStruct *pcharis =(CHARInputStruct *)(p->pIS);
	int index =0;
	//int rs = 0;
	int lth =0;

	if(!GBIMW.isShowCandWin)
	{
		return 0;
	}
	
	index =GetCharCandIndex(pcharis,pcharis->inputString[0]);

	//return !(WSTRLEN(pcharis->CharCandidateArray[index].candidates) <= CHARACTER_IM_CANDIDATES_MAX));
	
	lth =WSTRLEN(pcharis->CharCandidateArray[index].candidates);
	
	return (lth/CHARACTER_IM_CANDIDATES_MAX -pcharis->curSeledCandIndex/CHARACTER_IM_CANDIDATES_MAX-1 >0);
}

static void CharReset(GBIM *p,void *param)
{
	CHARInputStruct *pcharis =(CHARInputStruct *)(p->pIS);
	
	KeyCandidates *keep_CharCandidateArray =pcharis->CharCandidateArray;
	
	memset(pcharis,0,sizeof(CHARInputStruct));
	
	pcharis->CharCandidateArray =keep_CharCandidateArray;
		
	GBIMW.isSelectedCand =1;
	GBIMW.isShowCandWin =0;
	GBIMW.noHandleKeyEvent =1;
	GBIMW.shouldUpScreen =0;
}

static void CharSwitchIM(GBIM *p,void *param)
{
	CHARInputStruct *pcharis =(CHARInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin || !(pcharis->inputLth>0))
	{
		GBEngineChangeIM();
	
		GBIMW.isSelectedCand =1;
		GBIMW.noHandleKeyEvent =0;
		return;	
	}
}

static void CharOnLeft(GBIM *p,void *param)
{
	CHARInputStruct *pcharis =(CHARInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.noHandleKeyEvent =1;	
		GBIMW.isShowCandWin =0;
		GBIMW.shouldUpScreen =0;
		return;
	}
	
	if(pcharis->curSeledCandIndex > 0)
	{
		int j =0;
		int index =GetCharCandIndex(pcharis,pcharis->inputString[0]);
		pcharis->curSeledCandIndex--;
		
		if((pcharis->curSeledCandIndex+1)/CHARACTER_IM_CANDIDATES_MAX != pcharis->curSeledCandIndex/CHARACTER_IM_CANDIDATES_MAX)//上一页
		{
			GBIMW.candsNum =CHARACTER_IM_CANDIDATES_MAX;
		}
		
		GBIMW.selectIndex =pcharis->curSeledCandIndex % CHARACTER_IM_CANDIDATES_MAX;
		
		
		for(j=0;j<GBIMW.candsNum;j++)
		{
			GBIMW.cands[j][0] =pcharis->CharCandidateArray[index].candidates[j];
			GBIMW.cands[j][1] =0;
		}
	}
	else
	{		
		GBIMW.noHandleKeyEvent =0;		
		GBIMW.isShowCandWin =1;		
	}

	GBIMW.shouldUpScreen =0;	
}

static void CharOnRight(GBIM *p,void *param)
{
	CHARInputStruct *pcharis =(CHARInputStruct *)(p->pIS);
	
	int index =GetCharCandIndex(pcharis,pcharis->inputString[0]);
	//??int CandNum =gb_strlen(pcharis->CharCandidateArray[index].candidates);
	int CandNum =WSTRLEN(pcharis->CharCandidateArray[index].candidates);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.noHandleKeyEvent =1;	
		GBIMW.isShowCandWin =0;
		GBIMW.shouldUpScreen =0;
		return;
	}
	
	if(pcharis->curSeledCandIndex < CandNum-1)
	{
		int j =0;
		pcharis->curSeledCandIndex++;
		
		if((pcharis->curSeledCandIndex-1) / CHARACTER_IM_CANDIDATES_MAX != pcharis->curSeledCandIndex / CHARACTER_IM_CANDIDATES_MAX)//下一页
		{
			GBIMW.candsNum =CandNum-pcharis->curSeledCandIndex > CHARACTER_IM_CANDIDATES_MAX 
				? CHARACTER_IM_CANDIDATES_MAX: (CandNum-pcharis->curSeledCandIndex);
			
			for(j=0;j<GBIMW.candsNum;j++)
			{
				GBIMW.cands[j][0] =pcharis->CharCandidateArray[index].candidates[pcharis->curSeledCandIndex+j];
				GBIMW.cands[j][1] =0;
			}
		}
		
		GBIMW.selectIndex =pcharis->curSeledCandIndex % CHARACTER_IM_CANDIDATES_MAX;		
	}
	else
	{
		GBIMW.selectIndex =pcharis->curSeledCandIndex =0;
		GBIMW.noHandleKeyEvent =0;
		GBIMW.isShowCandWin =1;
	}

	GBIMW.shouldUpScreen =0;
}

static void CharOnUp(GBIM *p,void *param)
{
	CHARInputStruct *pcharis =(CHARInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.noHandleKeyEvent =1;
		GBIMW.isSelectedCand =0;
		GBIMW.shouldUpScreen =0;
		return;
	}
	
	if( CharCanPageUp(p,0) )
	{
		int j =0;
		int begin =0;
		int index  = GetCharCandIndex(pcharis,pcharis->inputString[0]);
		pcharis->curSeledCandIndex -= CHARACTER_IM_CANDIDATES_MAX;	
		
		GBIMW.selectIndex =pcharis->curSeledCandIndex % CHARACTER_IM_CANDIDATES_MAX;		
		
		begin =pcharis->curSeledCandIndex -GBIMW.selectIndex;//得到当前页第一候选字在buffer中的索引
		
		for(j =0; j < CHARACTER_IM_CANDIDATES_MAX; j++)
		{
			GBIMW.cands[j][0] 
				= pcharis->CharCandidateArray[index].candidates[begin+j];
			
		}
		
		GBIMW.candsNum =j;
	}
	else
	{
		if(pcharis->inputLth >= 0)
		{
			GBIMW.upscreenStr[0] =pcharis->inputString[0];
			GBIMW.upscreenLen =1;
			memset(GBIMW.fullOutput,0,sizeof(GBIMW.fullOutput));
			GBIMW.noHandleKeyEvent =0;
			GBIMW.isShowCandWin =0;
			GBIMW.shouldUpScreen =1;
		}
		else
		{
			GBIMW.shouldUpScreen =0;
			GBIMW.noHandleKeyEvent =0;
			GBIMW.isShowCandWin =1;
		}
	}
}

static void CharOnDown(GBIM *p,void *param)
{	
	CHARInputStruct *pcharis =(CHARInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.noHandleKeyEvent =1;
		GBIMW.isSelectedCand =0;
		return;
	}
	
	if(CharCanPageDown(p,0))
	{
		int j=0;
		int start;
		int currentPageNum =pcharis->curSeledCandIndex / CHARACTER_IM_CANDIDATES_MAX;
		int currentPos =pcharis->curSeledCandIndex % CHARACTER_IM_CANDIDATES_MAX;
		int index =GetCharCandIndex(pcharis,pcharis->inputString[0]);
		unsigned short *pcandStr =pcharis->CharCandidateArray[index].candidates;
		int CandNum =WSTRLEN(pcandStr);
		
		
		if( currentPageNum * CHARACTER_IM_CANDIDATES_MAX + CHARACTER_IM_CANDIDATES_MAX + currentPos <CandNum )
		{
			pcharis->curSeledCandIndex +=CHARACTER_IM_CANDIDATES_MAX;//下页的位置
		}
		else
		{
			pcharis->curSeledCandIndex = CandNum - 1;//下页的最后一个位置
		}
		currentPageNum++;
		
		start = currentPageNum * CHARACTER_IM_CANDIDATES_MAX;
		
		GBIMW.selectIndex =pcharis->curSeledCandIndex % CHARACTER_IM_CANDIDATES_MAX;
		
		
		for(j = 0; j < CHARACTER_IM_CANDIDATES_MAX && j + start < CandNum; j++)
		{
			GBIMW.cands[j][0] = pcandStr[start +j];
			GBIMW.cands[j][1] = 0;
		}
		
		GBIMW.candsNum = j;		
	}
	
	/*modified by xch 20060807*/
	GBIMW.noHandleKeyEvent =0;
	GBIMW.isShowCandWin =1;
	
	GBIMW.shouldUpScreen =0;
}

static void CharOnBlank(GBIM *p,void *param)
{
	CHARInputStruct *pcharis =(CHARInputStruct *)(p->pIS);
	
	if(pcharis->inputString[0])
	{
		int index =GetCharCandIndex(pcharis,pcharis->inputString[0]);
		SendHZtoClient(&(pcharis->CharCandidateArray[index].candidates[pcharis->curSeledCandIndex]),1);
		pcharis->inputString[0] =0;
		pcharis->inputLth =0;
		GBIMW.noHandleKeyEvent =0;
	}
	else
	{
		GBIMW.noHandleKeyEvent =1;
		GBIMW.shouldUpScreen =0;
	}
	GBIMW.isShowCandWin =0;
	
}

static void CharOnBack(GBIM *p,void *param)
{
	CHARInputStruct *pcharis =(CHARInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.noHandleKeyEvent =1;
		return;
	}
	
	if(pcharis->inputLth > 0)
	{
		pcharis->inputString[--pcharis->inputLth] =0;
	}
	
	GBIMW.isShowCandWin =0;
	GBIMW.noHandleKeyEvent =0;
}

static void CharFirstPage(CHARInputStruct * pcharis, int realkey)
{
    int i;
	int index =0;
	
	
	GBIMW.isShowCandWin =1;
	GBIMW.isSelectedCand =1;
	GBIMW.selectIndex =0;
	pcharis->curSeledCandIndex =0;
	
    pcharis->inputLth = 1;
	pcharis->inputString[0] =realkey;//设置数字区
	
	GBIMW.candsNum =0;
	
	
	index =GetCharCandIndex(pcharis,realkey);
	
	for(i = 0; i < CHARACTER_IM_CANDIDATES_MAX; i++)//显示第一页
	{
		if(pcharis->CharCandidateArray[index].candidates[i])
		{
			GBIMW.cands[GBIMW.candsNum][0] 
				=pcharis->CharCandidateArray[index].candidates[i];
			GBIMW.cands[GBIMW.candsNum++][1] = 0;
		}
	}	
}

static void CharOnNumS(GBIM *p,void *param,int realkey)
{
	CHARInputStruct *pcharis =(CHARInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin || pcharis->inputLth != 1)
	{
		GBIMW.shouldUpScreen =0;
		CharFirstPage(pcharis, realkey);
	}
	else
	{
		switch(realkey)
		{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			{
                if(realkey != pcharis->inputString[0])
                {
					int index =GetCharCandIndex(pcharis,pcharis->inputString[0]);
					GBIMW.upscreenStr[0] 
						=pcharis->CharCandidateArray[index].candidates[pcharis->curSeledCandIndex];
					GBIMW.upscreenLen = 1;
					GBIMW.upscreenStr[1] =0;
					GBIMW.shouldUpScreen =1;
                    CharFirstPage(pcharis, realkey);
                }
                else
                {
                    CharOnRight(p, param);
                }
				GBIMW.noHandleKeyEvent = 0;
				GBIMW.isShowCandWin = 1;
				break;
			}
		}
	}
	
	
	GBIMW.noHandleKeyEvent =0;
}

static void CharOnNumL(GBIM *p,void *param,int realkey)
{
	GBIMW.noHandleKeyEvent =1;
}

static void CharPrepareShowMessage(GBIM *p,void* param)
{
	CHARInputStruct *pcharis =(CHARInputStruct *)(p->pIS);
	if(GBIMW.isShowCandWin)
	{		
		GBIMW.fullOutput[0] = pcharis->inputString[0];
		GBIMW.fullOutput[1] = 0;
	}
}

static int CharCanPrevHeader(GBIM *p,void *param){return 0;}
static int CharCanNextHeader(GBIM *p,void *param){return 0;}
static void CharDisplay(GBIM *p,void *param){}
static void CharOnSharp(GBIM *p,void *param)
{
	CHARInputStruct *pcharis =(CHARInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin || pcharis->inputLth != 1)
	{
		CharFirstPage(pcharis,'#');
	}
	else
	{
		int index =GetCharCandIndex(pcharis,pcharis->inputString[0]);
		
		if(index ==-1) return;
		
        if('#' != pcharis->inputString[0])
        {
			GBIMW.upscreenStr[0] 
				=pcharis->CharCandidateArray[index].candidates[pcharis->curSeledCandIndex];
			GBIMW.upscreenLen = 1;
			GBIMW.upscreenStr[1] =0;
			GBIMW.shouldUpScreen =1;
            CharFirstPage(pcharis, '#');
        }
        else
        {
            CharOnRight(p, param);
        }
		GBIMW.noHandleKeyEvent = 0;
		GBIMW.isShowCandWin = 1;
		
	}
}

static void CharOnReturn(GBIM *p,void *param){}	

//数字输入法
static void NumberOnStar(GBIM *p,void *param)
{
	//GBEngineChangeIM();
}

static int NumberCanPageUp(GBIM *p,void* param){return 1;}
static int NumberCanPageDown(GBIM *p,void* param){return 1;}
static void NumberReset(GBIM *p,void *param)
{
	GBIMW.isShowCandWin =0;
	GBIMW.noHandleKeyEvent =1;
	GBIMW.shouldUpScreen =0;
}

static void NumberSwitchIM(GBIM *p,void *param)
{
	if(!GBIMW.isShowCandWin)
	{
		GBEngineChangeIM();
		GBIMW.noHandleKeyEvent =0;
		GBIMW.isShowCandWin =0;
		return;	
	}
}

static void NumberOnLeft(GBIM *p,void *param)
{
	GBIMW.noHandleKeyEvent =1;
}

static void NumberOnRight(GBIM *p,void *param)
{
	GBIMW.noHandleKeyEvent =1;
}

static void NumberOnUp(GBIM *p,void *param)
{
	GBIMW.noHandleKeyEvent =1;
}

static void NumberOnDown(GBIM *p,void *param)
{
	GBIMW.noHandleKeyEvent =1;
}

static void NumberOnBlank(GBIM *p,void *param)
{
	GBIMW.noHandleKeyEvent =1;
}

static void NumberOnBack(GBIM *p,void *param)
{
	GBIMW.noHandleKeyEvent =1;
}

static void NumberOnNumS(GBIM *p,void *param,int realkey)
{
	switch(realkey)
	{
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '0':
		{
			GBIMW.upscreenStr[0] =realkey;
			GBIMW.upscreenStr[1] =0;
			GBIMW.upscreenLen =1;
			GBIMW.shouldUpScreen =1;
			GBIMW.noHandleKeyEvent =0;
			break;
		}
		
	default:
		GBIMW.noHandleKeyEvent =1;
		break;
	}
	
}

static void NumberOnNumL(GBIM *p,void *param,int realkey)
{
	
}

static void NumberPrepareShowMessage(GBIM *p,void* param)
{
	
}

static int NumberCanPrevHeader(GBIM *p,void *param){return 0;}
static int NumberCanNextHeader(GBIM *p,void *param){return 0;}
static void NumberDisplay(GBIM *p,void *param){}
static void NumberOnSharp(GBIM *p,void *param)
{
	GBIMW.noHandleKeyEvent =1;
}

static void NumberOnReturn(GBIM *p,void *param){}

static void GBElementsInit(CHARInputStruct *pcharis,int option,int elemtype)
{
	int i=0;
	int lth =0;
	int j =0;
	
	unsigned char keepinputString0 =ENIS.inputString[0];
	int keepinputLen =ENIS.inputLen;
	
	//ENIS.option = EN_OPTION_DONT_ADJUST_CHAR_ORDER;
	ENIS.option =option;
	while(pcharis->CharCandidateArray[i].key)
	{
		ENIS.inputString[0] =(char)(pcharis->CharCandidateArray[i].key);
		ENIS.inputLen =1;
		
		
#ifdef VIETNAM
		if(GBLtFirstPage(&ENIS) == EBEN_OK)
#else
			if(GBLtFirstHeader(&ENIS) == EBEN_OK)
#endif
			{
				GBLtFirstPage(&ENIS);
			}
			
			lth =0;
NEXT_PAGE:	
			for(j =0;j <ENIS.candidateCount;j++)
			{
				pcharis->CharCandidateArray[i].candidates[lth+j]=
					pcharis->CharCandidateArray[i].candidates[lth+j] 
					=ENIS.candidates[j][0];
			}
			
			pcharis->CharCandidateArray[i].candidates[lth+j] =
				pcharis->CharCandidateArray[i].candidates[lth+j] =0;//清空
			
			while(GBLtCanPageDown(&ENIS) == EBEN_OK)
			{
				GBLtNextPage(&ENIS);
				lth +=j;
				goto NEXT_PAGE;
			}
			
			i++;
	}
	
	if(elemtype ==ELEM_SELFWORD)
	{
		WSTRNCOPYN(pcharis->CharCandidateArray[0].candidates,32,
			pcharis->CharCandidateArray[11].candidates,7);
		pcharis->CharCandidateArray[0].candidates[7] =L'0';
		WSTRCPY(pcharis->CharCandidateArray[0].candidates+CHARACTER_IM_CANDIDATES_MAX,
			pcharis->CharCandidateArray[11].candidates+CHARACTER_IM_CANDIDATES_MAX);
	}
	
	ENIS.inputString[0] =keepinputString0;
	ENIS.inputLen =keepinputLen;
    ENIS.option = 0;
	
}

static int u2c(char *pc,unsigned short *pu,int lth)
{
	unsigned short *pend =pu + lth;
	
	if(!pu || !pc) return -1;
	
	while(pu < pend)
	{
		*pc++ =(char)*pu++;
	}
	return 0;
}

static int c2u(unsigned short *pu,char *pc,int lth)
{
	char *pend =pc + lth;
	
	if(!pu || !pc) return -1;
	
	while(pc < pend)
	{
		*pu++ =(unsigned short)*pc++;
	}
	return 0;
}

static int c2u_1to0(unsigned short *pu,char *pc,int lth)
{
	char *pend =pc + lth;
	
	if(!pu || !pc) return -1;
	
	while(pc < pend)
	{
		
		*pu++ =(unsigned short)(*pc =='1'?'0':*pc);
		pc++;
	}
	return 0;
}

//英文自造词输入
static void SMFirstPage(GBIM *p,void*param)
{
	SMInputStruct *psmis =(SMInputStruct *)(p->pIS);
	int i;
	int index =0;
	CHARInputStruct *pcharis =0;
	
	assert(psmis->penis ==GBIMW.GBIMS[1].pIS);
	psmis->curSeledCandIndex =0;
	
	if(psmis->inputLth <=0 )
	{
		psmis->inputLth =psmis->penis->inputLen;
#if LATIN_ENCODE_LENGTH == 2
		u2c(psmis->inputString,psmis->penis->inputString,psmis->penis->inputLen);
		memcpy(psmis->numberString,psmis->inputString,psmis->inputLth);
		psmis->numberString[psmis->inputLth] =0;
#else
		memcpy(psmis->inputString,psmis->penis->inputString,psmis->penis->inputLen);
		memcpy(psmis->numberString,psmis->inputString,psmis->inputLth);
		psmis->numberString[psmis->inputLth] =0;
#endif
		psmis->pchar =psmis->inputString;//保存正在选择的字符指针
		//	psmis->pend =psmis->inputString +psmis->inputLth;
	}
	else
	{
		
	}
	
	GBIMW.isShowCandWin =1;
	GBIMW.isSelectedCand =1;
	GBIMW.selectIndex =0;
	GBIMW.candsNum =0;
	
	pcharis =(CHARInputStruct*)(GBIMW.GBIMS[2].pIS);
	
	index =GetCharSHCandIndex(pcharis,*psmis->pchar);
	
	for(i = 0; i < CHARACTER_IM_CANDIDATES_MAX; i++)//显示第一页
	{
		if(pcharis->CharCandidateArray[index].candidates[i])
		{
			GBIMW.cands[GBIMW.candsNum][0] 
				=pcharis->CharCandidateArray[index].candidates[i];
			GBIMW.cands[GBIMW.candsNum++][1] = 0;
		}
	}
	assert(psmis->inputLth <sizeof(GBIMW.fullOutput));
#if LATIN_ENCODE_LENGTH == 2 
	c2u((unsigned short*)GBIMW.fullOutput,(char*)psmis->inputString,psmis->inputLth);
#else	
	memcpy(GBIMW.fullOutput,psmis->inputString,psmis->inputLth);
#endif
	GBIMW.fullOutput[psmis->inputLth*sizeof(GBCHAR)] =0;
	GBIMW.syllableNum =1;
}

static void SMOnStar(GBIM *p,void *param)
{
	
}

static int SMCanPageUp(GBIM *p,void* param)
{
	SMInputStruct *psmis =(SMInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		return 0;
	}
	
	return psmis->curSeledCandIndex / CHARACTER_IM_CANDIDATES_MAX - 0;
}

static int SMCanPageDown(GBIM *p,void* param)
{
	SMInputStruct *psmis =(SMInputStruct *)(p->pIS);
	int index =0;
	CHARInputStruct *pcharis =0;
	
	if(!GBIMW.isShowCandWin)
	{
		return 0;
	}
	
	pcharis =(CHARInputStruct*)(GBIMW.GBIMS[2].pIS);
	index =GetCharSHCandIndex(pcharis,*(psmis->pchar));
	return WSTRLEN(pcharis->CharCandidateArray[index].candidates) / CHARACTER_IM_CANDIDATES_MAX 
		-  psmis->curSeledCandIndex / CHARACTER_IM_CANDIDATES_MAX ;
}

static void SMReset(GBIM *p,void *param)
{
	SMInputStruct *psmis =(SMInputStruct *)(p->pIS);
	memset(psmis,0,sizeof(SMInputStruct));
	GBIMW.selectIndex =0;
	GBIMW.syllableNum =0;
	GBIMW.candsNum =0;
	psmis->pchar =psmis->inputString;
}

static void SMSwitchIM(GBIM *p,void *param)
{
}

static void SMOnLeft(GBIM *p,void *param)
{
	SMInputStruct *psmis =(SMInputStruct *)(p->pIS);
	CHARInputStruct *pcharis =(CHARInputStruct*)(GBIMW.GBIMS[2].pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.noHandleKeyEvent =1;	
		GBIMW.isShowCandWin =0;
		return;
	}
	
	if(psmis->curSeledCandIndex > 0)
	{
		int j =0;
		int index =GetCharSHCandIndex(pcharis,*psmis->pchar);
		psmis->curSeledCandIndex--;
		
		if((psmis->curSeledCandIndex+1)/CHARACTER_IM_CANDIDATES_MAX 
			!= psmis->curSeledCandIndex/CHARACTER_IM_CANDIDATES_MAX)//上一页
		{
			GBIMW.candsNum =CHARACTER_IM_CANDIDATES_MAX;
		}
		
		GBIMW.selectIndex =psmis->curSeledCandIndex % CHARACTER_IM_CANDIDATES_MAX;
		
		
		for(j=0;j<GBIMW.candsNum;j++)
		{
			GBIMW.cands[j][0] =pcharis->CharCandidateArray[index].candidates[j];
			GBIMW.cands[j][1] =0;
		}
	}
	else
	{		
		GBIMW.noHandleKeyEvent =0;		
		GBIMW.isShowCandWin =1;
	}
}

static void SMOnRight(GBIM *p,void *param)
{
	SMInputStruct *psmis =(SMInputStruct *)(p->pIS);
	CHARInputStruct *pcharis =(CHARInputStruct*)(GBIMW.GBIMS[2].pIS);
	
	
	int index =GetCharSHCandIndex(pcharis,*psmis->pchar);
	//??int CandNum =gb_strlen(pcharis->CharCandidateArray[index].candidates);
	int CandNum =WSTRLEN(pcharis->CharCandidateArray[index].candidates);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.noHandleKeyEvent =1;	
		GBIMW.isShowCandWin =0;
		return;
	}
	
	if(psmis->curSeledCandIndex < CandNum-1)
	{
		int j =0;
		psmis->curSeledCandIndex++;
		
		if((psmis->curSeledCandIndex-1)/CHARACTER_IM_CANDIDATES_MAX != psmis->curSeledCandIndex/CHARACTER_IM_CANDIDATES_MAX)//下一页
		{
			GBIMW.candsNum =CandNum-psmis->curSeledCandIndex >CHARACTER_IM_CANDIDATES_MAX 
				? CHARACTER_IM_CANDIDATES_MAX: (CandNum-psmis->curSeledCandIndex);
			
			for(j=0;j<GBIMW.candsNum;j++)
			{
				GBIMW.cands[j][0] =pcharis->CharCandidateArray[index].candidates[psmis->curSeledCandIndex+j];
				GBIMW.cands[j][1] =0;
			}
		}
		
		GBIMW.selectIndex =psmis->curSeledCandIndex % CHARACTER_IM_CANDIDATES_MAX;
		
	}
	else
	{
		GBIMW.noHandleKeyEvent =0;
		GBIMW.isShowCandWin =1;
	}
}

static void SMOnUp(GBIM *p,void *param)
{
	SMInputStruct *psmis =(SMInputStruct *)(p->pIS);
	CHARInputStruct *pcharis =(CHARInputStruct*)(GBIMW.GBIMS[2].pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.isShowCandWin =0;
		GBIMW.noHandleKeyEvent =1;
		GBIMW.isSelectedCand =0;
		return;
	}
	
	if( SMCanPageUp(p,0) )
	{
		int j =0;
		int begin =0;
		int index  =GetCharSHCandIndex(pcharis,*psmis->pchar);
		psmis->curSeledCandIndex -=CHARACTER_IM_CANDIDATES_MAX;	
		
		GBIMW.selectIndex =psmis->curSeledCandIndex % CHARACTER_IM_CANDIDATES_MAX;		
		
		begin =psmis->curSeledCandIndex -GBIMW.selectIndex;//得到当前页第一候选字在buffer中的索引
		
		for(j =0; j <CHARACTER_IM_CANDIDATES_MAX; j++)
		{
			GBIMW.cands[j][0] 
				= pcharis->CharCandidateArray[index].candidates[begin+j];
			
		}
		
		GBIMW.candsNum =j;
	}
	else
	{
		
		GBIMW.shouldUpScreen =0;
		GBIMW.noHandleKeyEvent =0;
		GBIMW.isShowCandWin =1;
		
	}
}

static void SMOnDown(GBIM *p,void *param)
{
	SMInputStruct *psmis =(SMInputStruct *)(p->pIS);
	CHARInputStruct *pcharis =(CHARInputStruct*)(GBIMW.GBIMS[2].pIS);
	
	if(!GBIMW.isShowCandWin)
	{
		GBIMW.isShowCandWin =0;
		GBIMW.noHandleKeyEvent =1;
		GBIMW.isSelectedCand =0;
	}
	
	if( SMCanPageDown(p,0) )
	{
		int j=0;
		int start;
		int currentPageNum =psmis->curSeledCandIndex / CHARACTER_IM_CANDIDATES_MAX;
		int currentPos =psmis->curSeledCandIndex % CHARACTER_IM_CANDIDATES_MAX;
		int index =GetCharSHCandIndex(pcharis,*(psmis->pchar));
		
		
		unsigned short *pcandStr =pcharis->CharCandidateArray[index].candidates;
		int CandNum =WSTRLEN(pcandStr);
		
		if( currentPageNum * CHARACTER_IM_CANDIDATES_MAX + CHARACTER_IM_CANDIDATES_MAX + currentPos <CandNum )
		{
			psmis->curSeledCandIndex +=CHARACTER_IM_CANDIDATES_MAX;//下页的位置
		}
		else
		{
			psmis->curSeledCandIndex = CandNum - 1;//下页的最后一个位置
		}
		currentPageNum++;
		
		start = currentPageNum * CHARACTER_IM_CANDIDATES_MAX;
		
		GBIMW.selectIndex =psmis->curSeledCandIndex % CHARACTER_IM_CANDIDATES_MAX;
		
		
		for(j = 0; j < CHARACTER_IM_CANDIDATES_MAX && j + start < CandNum; j++)
		{
			GBIMW.cands[j][0] = pcandStr[start +j];
			GBIMW.cands[j][1] = 0;
		}
		
		GBIMW.candsNum = j;
		
		GBIMW.noHandleKeyEvent =0;
		GBIMW.isShowCandWin =1;
	}
	else
	{
		GBIMW.noHandleKeyEvent =0;
		GBIMW.isShowCandWin =1;
	}
}

static void SMOnBlank(GBIM *p,void *param)
{
	SMInputStruct *psmis =(SMInputStruct *)(p->pIS);
	
	assert(psmis->penis ==GBIMW.GBIMS[1].pIS);
	
	if(psmis->status ==MW_FINISH)//选完加入自造词
	{
		if(psmis->inputLth > 0 && psmis->inputString[0] >0)
		{
			//TODO
			int err;
			GBIMW.isCandHelpTips =0;
#if LATIN_ENCODE_LENGTH == 2			
			c2u(psmis->penis->inputString,psmis->inputString,psmis->inputLth);
#else
			strcpy(psmis->penis->inputString,psmis->inputString);
#endif
			psmis->penis->inputLen =psmis->inputLth;
			err =GBLtSelfHelpWordsAdd(psmis->penis);
			
			/*for(i =0;i<psmis->inputLth;i++)
			{
			KeyCandidates *p =CharCandidateArray;
			while(p->key)
			{
			if(gb_strchr(p->candidates,psmis->inputString[i]))
			{
			psmis->penis->inputString[i] =p->key;
			break;
			}
			p++;
			}
		}*/
			
			//c2u(psmis->penis->inputString,psmis->numberString,psmis->inputLth);
			//c2u_1to0(psmis->penis->inputString,psmis->numberString,psmis->inputLth);
			EnChar2NumStr(psmis->penis->inputString, psmis->penis->inputString, psmis->inputLth);
			
			psmis->penis->inputLen =psmis->inputLth;
			
#ifdef VIETNAM
			if((err =GBLtFirstPage(psmis->penis)) ==EBEN_OK)
#else
				if((err =GBLtFirstHeader(psmis->penis)) == EBEN_OK)
#endif
				{
					GBLtFirstPage(psmis->penis);	
				}
				
				GBIMW.activeIM =GBIMW.prevIM;
				//gb_strcpy(psmis->penis->inputString,psmis->inputString);
				
				
				
				/*switch(GBIMW.activeMode)
				{
				case GBPinYin:
				case GBStroke:
				case GBYinStroke:
				{
				GBIMW.activeIM =0;
				GBIMW.isCandHelpTips =0;
				GBIMW.candsNum =0;
				GBIMW.syllableNum =0;
				GBIMW.noHandleKeyEvent =0;
				GBIMW.isShowCandWin =0;				
				break;
				}
				case GBSmartLatin:
				{
				GBIMW.activeIM =1;
				GBIMW.isCandHelpTips =0;
				GBIMW.candsNum =0;
				GBIMW.syllableNum =0;
				GBIMW.noHandleKeyEvent =0;
				GBIMW.isShowCandWin =0;
				break;
				}
				case GBCharacter:
				break;
				case GBNumber:
				break;
				break;
				}
				GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],param);*/
		}
	}
	else if(psmis->status ==MW_SELECTTING || 
		psmis->status ==MW_POWERON)//正在选，加入选中候选字母
	{
		//if(psmis->inputLth < EN_INPUT_MAX_LEN)
		
		if(psmis->pchar-psmis->inputString < EN_INPUT_MAX_LEN)
		{
			psmis->inputString[psmis->pchar-psmis->inputString] = (GBIMW.cands[GBIMW.selectIndex][0]);
			psmis->pchar++;
			SMFirstPage(p,0);
		}
		
		if(psmis->pchar -psmis->inputString  <psmis->inputLth)
		{
			psmis->status =MW_SELECTTING;			
		}
		else
		{
			psmis->status =MW_FINISH;
			GBIMW.isCandHelpTips =1;
			GBIMW.isSelectedCand =0;
			GBIMW.candsNum =1;
			GBIMW.syllableNum =1;
			
			gb_strcpy((GBCHAR*)GBIMW.cands[0],okToAddMakeWordStr);				
		}
	}
	else if(psmis->status ==MW_FINISH)
	{
		psmis->status =-1;		
	}
}

static void SMOnBack(GBIM *p,void *param)
{
	SMInputStruct *psmis =(SMInputStruct *)(p->pIS);
	
	if(!GBIMW.isShowCandWin)	
	{
		GBIMW.isSelectedCand =0;
		GBIMW.noHandleKeyEvent =1;
		GBIMW.shouldUpScreen =0;
		return;
	}	
	
	if(psmis->pchar !=psmis->inputString)//pchar--
	{
		psmis->pchar--;
		GBIMW.fullOutput[psmis->pchar -psmis->inputString] 
			=*(psmis->pchar) 
			=psmis->penis->inputString[psmis->pchar -psmis->inputString];
		GBIMW.isShowCandWin =1;
		GBIMW.noHandleKeyEvent =0;
		SMFirstPage(p,0);
		return;
	}
	else//切换回自造词前模式
	{
		
		switch(GBIMW.activeMode)
		{
		case GBPinYin:
		case GBStroke:
		case GBYinStroke:
			{
				GBIMW.activeIM =0;
				//if(GBIMW.GBIMS[GBIMW.activeIM]->pSwitchItem ==0)//不是无缝切换
				{
					
				}
				
#ifdef __GBSHDATA
				//give a chance to user make word
				GBIMW.candsNum =1;
				gb_strcpy((GBCHAR*)GBIMW.cands[0],okToBeginMakeWordStr);

				
				GBIMW.syllableNum =1;			
				memset(GBIMW.fullOutput,0,sizeof(GBIMW.fullOutput));	
				
				//TODO
				c2u((unsigned short*)GBIMW.fullOutput,psmis->inputString,
					psmis->inputLth);//
				
				GBIMW.isCandHelpTips =1;
#else
				GBIMW.candsNum = 0;
#endif				
				GBIMW.noHandleKeyEvent =0;
				GBIMW.shouldUpScreen =0;
				GBIMW.isShowCandWin =1;
				GBIMW.isSelectedCand =0;
				
				break;
			}
		case GBSmartLatin:
			{
				GBIMW.activeIM =1;
				
#ifdef __GBSHDATA
				//give a chance to user make word
				GBIMW.candsNum =1;
				gb_strcpy((GBCHAR*)GBIMW.cands[0],okToBeginMakeWordStr);

				
				GBIMW.syllableNum =1;			
				memset(GBIMW.fullOutput,0,sizeof(GBIMW.fullOutput));	
				
				//TODO
				c2u((unsigned short*)GBIMW.fullOutput,psmis->inputString,
					psmis->inputLth);//
				
				GBIMW.isCandHelpTips =1;
#else
				GBIMW.candsNum = 0;
#endif				
				GBIMW.noHandleKeyEvent =0;
				GBIMW.shouldUpScreen =0;
				GBIMW.isShowCandWin =1;
				GBIMW.isSelectedCand =0;
				/*GBIMW.isCandHelpTips =0;
				GBIMW.noHandleKeyEvent =0;
				GBIMW.isShowCandWin =1;
				GBIMW.isSelectedCand =0;*/
				break;
			}
		case GBCharacter:
			break;
		case GBNumber:
			break;
			break;
		}
		//GBIMW.GBIMS[GBIMW.activeIM].GBIMReset(&GBIMW.GBIMS[GBIMW.activeIM],param);
	}	
}

static void SMOnNumS(GBIM *p,void *param,int realkey)
{
	SMInputStruct *psmis =(SMInputStruct *)(p->pIS);
	
	if('0' < realkey && realkey < GBIMW.candsNum +'1')
	{
		if(psmis->pchar-psmis->inputString < EN_INPUT_MAX_LEN)
		{
			psmis->inputString[psmis->pchar-psmis->inputString] =(GBIMW.cands[realkey -'1'][0]);
			psmis->pchar++;
			SMFirstPage(p,0);
		}
		
		if(psmis->pchar -psmis->inputString  <psmis->inputLth)
		{
			psmis->status =MW_SELECTTING;			
		}
		else
		{
			psmis->status =MW_FINISH;
			GBIMW.isCandHelpTips =1;
			GBIMW.isSelectedCand =0;
			GBIMW.candsNum =1;
			GBIMW.syllableNum =1;
			
			/*modified by */
			gb_strcpy((GBCHAR*)GBIMW.cands[0],okToAddMakeWordStr);
		}
	}
	
	GBIMW.noHandleKeyEvent =0;
}

static void SMOnNumL(GBIM *p,void *param,int realkey)
{
}

static void SMPrepareShowMessage(GBIM *p,void* param)
{
	return;
}

static int SMCanPrevHeader(GBIM *p,void *param){return 0;}
static int SMCanNextHeader(GBIM *p,void *param){return 0;}
static void SMDisplay(GBIM *p,void *param){}
static void SMOnSharp(GBIM *p,void *param)
{
}
static void SMOnReturn(GBIM *p,void *param){}
