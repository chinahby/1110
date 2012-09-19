#ifndef AEEVKEY_H
#define AEEVKEY_H
/*=====================================================================
FILE:  VKEY.h

SERVICES:  VIRTUAL KEY Control Interface

DESCRIPTION: Base level definitions, typedefs, etc. for AEE Date Control

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright ?2004-2006 TCL Incorporated.
               All Rights Reserved.
            TCL Proprietary/GTDR
=====================================================================*/

//---------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------

#include "AEE.h"
#include "AEEDisp.h"
#include "AEEText.h"
#include "AEEImage.h"
//---------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------
//#define key_number 12
#define RGB_NORMAL           (MAKE_RGB(193, 193, 193))
#define RGB_DOWN             (MAKE_RGB(253, 198, 137))
#define RGB_FRAME            (MAKE_RGB(255, 255, 0))
//---------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------
/*
#define AEE_VKEY_NUMBER               0x00000001
#define AEE_VKEY_CHAR                 0x00000002
#define AEE_VKEY_CHARANDNUM           0x00000004

#define VKEY_HANDLE_ALL_EVENTS        0x00000100
*/
typedef enum
{
   AEE_VKEY_NUMBER,                //数字键盘
   AEE_VKEY_BIGCHAR,               //大写字母键盘
   AEE_VKEY_LITTLECHAR,            //小写字母键盘
   AEE_VKEY_SYMBOL,                //符号键盘
   AEE_VKEY_NUMBER_LITTLECHAR,     //数字与小写字母键盘
   AEE_VKEY_NUMBER_BIGCHAR,        //数字与大写字母键盘
   AEE_VKEY_DIALNUMBER,            //拨号键盘
   AEE_VKEY_CALCULATOR,              //计算器键盘
   AEE_VKEY_ZHUYIN,                  // 注音键盘
   AEE_VKEY_USERKEYPAD,             //用户自己定义键盘
   AEE_VKEY_THAINUMBER,
   AEE_VKEY_MYANMRANUMBER
}VKeyCategory;

typedef enum
{
	State_DialNumber_Down,
	State_DialNumber_Up
}DialNumberState;
/*=================================================
用户自己定义的键盘所需要的数据结构

=================================================*/
typedef struct _AEEVKeyItem AEEVKeyItem;
struct _AEEVKeyItem
{
	AEEEvent        evt;
	uint16          wp;
	AECHAR          name[4];
};

typedef struct _AEEVKeyPad AEEVKeyPad;
struct _AEEVKeyPad
{
	int             row;        //行
	int             coloum;     //列
	AEEVKeyItem     *pItem;
};
/*====================================================
控件本身提供的数据结构

=====================================================*/
typedef struct _AEEVKeyItem_Own  AEEVKeyItem_Own;
struct _AEEVKeyItem_Own
{
	AEEEvent        evt;
	uint16          wp;
	AECHAR          name[1];
};

typedef struct _AEETHAIVKeyItem_Own  AEETHAIVKeyItem_Own;
struct _AEETHAIVKeyItem_Own
{
	AEEEvent        evt;
	uint16          wp;
	uint16          name[20];
    uint16          m_Itemmax;
};

typedef struct _AEEVKeyPad_Own  AEEVKeyPad_Own;
struct _AEEVKeyPad_Own
{
	int                 row;        //行
	int                 coloum;     //列

};

/////////////////////////////////////////
typedef struct _IVkeyCtl      IVkeyCtl;
/*this section declares one interface of IVkeyCtl through QINTERFACE,
  and add its member functions here*/
QINTERFACE(IVkeyCtl)
{
   DECLARE_IBASE(IVkeyCtl)
   
   DECLARE_ICONTROL(IVkeyCtl)  

   //below we can expand its member functions

   void    (*SetEventHandler)(IVkeyCtl * po, PFNAEEEVENT pfn, void * pUser);

   void    (*SetKeyPad)(IVkeyCtl * po, AEERect * prc, uint32  m_dwProps);
   void    (*SetColor)(IVkeyCtl * po, RGBVAL rgb , RGBVAL  rgbPress);
   int     (*SetUserKeyPad)(IVkeyCtl * po, AEERect * prc, AEEVKeyPad  *usr_pad);
   
};



/*Member functions are accessed through the following virtual table access macros;
GET_PVTBL():is used in the previous example to access the interface functions 
through virtual function table pointers.*/
#define IVKEYCTL_AddRef(p)                         GET_PVTBL(p,IVkeyCtl)->AddRef(p)
#define IVKEYCTL_Release(p)                        GET_PVTBL(p,IVkeyCtl)->Release(p)
#define IVKEYCTL_HandleEvent(p,ec,wp,dw)           GET_PVTBL(p,IVkeyCtl)->HandleEvent(p,ec,wp,dw)
#define IVKEYCTL_Redraw(p)                         GET_PVTBL(p,IVkeyCtl)->Redraw(p)
#define IVKEYCTL_SetActive(p,b)                    GET_PVTBL(p,IVkeyCtl)->SetActive(p,b)
#define IVKEYCTL_IsActive(p)                       GET_PVTBL(p,IVkeyCtl)->IsActive(p)
#define IVKEYCTL_SetRect(p,prc)                    GET_PVTBL(p,IVkeyCtl)->SetRect(p,prc)
#define IVKEYCTL_GetRect(p,prc)                    GET_PVTBL(p,IVkeyCtl)->GetRect(p,prc)
#define IVKEYCTL_SetProperties(p,props)            GET_PVTBL(p,IVkeyCtl)->SetProperties(p,props)
#define IVKEYCTL_GetProperties(p)                  GET_PVTBL(p,IVkeyCtl)->GetProperties(p)
#define IVKEYCTL_Reset(p)                          GET_PVTBL(p,IVkeyCtl)->Reset(p)

#define IVKEYCTL_SetEventHandler(p,pfn,puser)      GET_PVTBL(p,IVkeyCtl)->SetEventHandler(p,pfn,puser)
#define IVKEYCTL_SetKeyPad(p,prc,kind)             GET_PVTBL(p,IVkeyCtl)->SetKeyPad(p,prc,kind)
#define IVKEYCTL_SetColor(p,bg,pr)                 GET_PVTBL(p,IVkeyCtl)->SetColor(p,bg,pr)
#define IVKEYCTL_SetUserKeyPad(p,prc,usr_pad)      GET_PVTBL(p,IVkeyCtl)->SetUserKeyPad(p,prc,usr_pad)

#endif //AEEVKEY_H
