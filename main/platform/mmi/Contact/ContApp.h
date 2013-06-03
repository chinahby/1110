#ifndef CONTAPP_H
#define CONTAPP_H

/*==============================================================================
// 文件：
//        ContApp.h
//        2004-06-08 jyh草拟版本(Draft Version)
//        版权所有(c) 2004 Anylook
//        
// 文件说明：
//        
// 作者：2004-06-08
// 创建日期：jyh
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时    间      修   改   人     修改内容、位置及原因
--------      -----------     -----------------------------------------------
04-06-08      jyh             初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "AEEAddrBook.h"
#include "AEEFile.h"
/*==============================================================================
                                 宏定义和常数
==============================================================================*/
#define MASK_ADDRFIELD_NAME                     (0x0001)
#define MASK_ADDRFIELD_PHONE_GENERIC    (0x0002) 
#define MASK_ADDRFIELD_PHONE_HOME       (0x0004)
#define MASK_ADDRFIELD_PHONE_WORK   (0x0008)
#define MASK_ADDRFIELD_PHONE_FAX        (0x0010)
#define MASK_ADDRFIELD_EMAIL                (0x0020)
#define MASK_ADDRFIELD_ADDRESS          (0x0040)
#define MASK_ADDRFIELD_URL                  (0x0080)
#define MASK_ADDRFIELD_NOTES            (0x0100)

#define TYPE_NO_MATCH (0x00)
#define TYPE_INDEX_MATCH   (0x1)
#define TYPE_FIELD_MATCH    (0x2)

//#define MATCH_DIGITS_MIN    7
#ifdef FEATURE_VERSION_LM126C
#define MATCH_DIGITS_MIN    11
#else
#define MATCH_DIGITS_MIN    5
#endif

#define MATCH_DIGITS_MAX    8   //Add By zzg 2013_02_22  后8位匹配

#define PARAM_SPEED_DIAL				 999		//Add By zzg 2012_10_31


/*==============================================================================
                                 类型定义
==============================================================================*/
typedef struct _sSelectFieldInfo
{
    uint16          wIndex;           
    uint16          wFieldCount;
    dword          wFieldMask; 
    AECHAR             *m_pAddNewName;
    AECHAR             *m_pAddNewMobile;
    AECHAR             *m_pAddNewHome;
    AECHAR             *m_pAddNewOffice;
    AECHAR             *m_pAddNewFax;    
    AECHAR             *m_pAddNewEMAIL;
    AECHAR             *m_pAddNewAddress;
    AECHAR             *m_pAddNewURL;
    AECHAR             *m_pAddNewRemark;
    AECHAR             *m_pAddNewFormatContent;
} sSelectFieldInfo;

typedef struct _sSelectFieldListNode sSelectFieldListNode ;

struct _sSelectFieldListNode
{
    sSelectFieldListNode* pPreNode;
    sSelectFieldListNode* pNextNode;    
    sSelectFieldInfo       SelectFieldInfo;
};


////
// Callback function type, used with IContactApp_SelectField()
//
// selRec  == the selected IAddrRec. Don't forget to use
//            IADDRREC_AddRef() if this pointer will be saved.
// selItem == the selected AEEAddrField from selRec.
//
// Returns TRUE if the contact app should exit
typedef boolean (*PFNSELECTEDCONTACTCB)(sSelectFieldListNode* pSelectFieldList);

////
// All the ways that somebody can start the contacts app
typedef enum _AppletStartMethod {
    STARTMETHOD_NORMAL=0x01,             // ISHELL_StartApplet()
    STARTMETHOD_SAVEFIELD,          // Save number with phone edit option
    STARTMETHOD_FINDNUMBER,         // find number called by other app
    STARTMETHOD_SELECTFIELD,        // Select Field called by other app
    STARTMETHOD_SMARTCALL,      // Select Field called by other app
    STARTMETHOD_GROUPLIST,
    STARTMETHOD_ADDFIELD,
    STARTMETHOD_ADDEMAIL,
    STARTMETHOD_ADDURL,
    STARTMETHOD_MAINMENU
}AppletStartMethod;

////
// Valid first characters for startup arguments. 
//
// See the CContactApp_DecodeStartArgs() 
// 
#define STARTARGPREFIX_SAVENUMBER       ((char) 'E')
#define STARTARGPREFIX_VIEWALL          ((char) 'V')
#define STARTARGPREFIX_FINDNUMBER       ((char) '?')
#define STARTARGPREFIX_SELECTFIELD      ((char) 'F')
#define STARTARGPREFIX_SMARTCALL        ((char) 'S')
#define STARTARGPREFIX_MAINMENU        ((char) 'M')
#define STARTARGPREFIX_GROUPLIST      ((char) 'G')

////
// Valid second characters for startup arguments in case of
// start method is STARTARGPREFIX_SAVENUMBER_NUMBEREDIT 
// or STARTARGPREFIX_SAVENUMBER_NONUMBEREDIT 
//
// See the CContactApp_DecodeStartArgs() 
// 
#define STARTARGPREFIX_SAVE_NUMBER     ((char) 'W')
#define STARTARGPREFIX_SAVE_EMAIL ((char) 'X')
#define STARTARGPREFIX_SAVE_URL  ((char) 'R')
#define STARTARGPREFIX_ADD_FLD  ((char) 'F')

typedef enum _PhoneSaveType{
    ADD_FIELD,
    SAVE_NUMBER,          //Add a new contact with this phone number on Phone(EFS)
    SAVE_EMAIL,  //Add this number to a existing contact
    SAVE_URL       //Add a new contact with this phone number on RUIM
}PhoneSaveType;

//These are different type of save option in STARTMETHOD_SAVENUMBER_NUMBER_EDIT
// and STARTMETHOD_SAVENUMBER_NO_NUMBER_EDIT.
typedef enum _PhoneNumberSaveType{
    SAVE_NEW,          //Add a new contact with this phone number on Phone(EFS)
    SAVE_TO_EXISTING,  //Add this number to a existing contact
    SAVE_TO_RUIM       //Add a new contact with this phone number on RUIM
}PhoneNumberSaveType;


// These are different type of option that select a type of field in 
// STARTMETHOD_SELECTFIELD.
// Note: The value is placed a mutually binary and the SELECT_ALL is 
// the value that lower 4 bits summation above all.
// The availble ASSIC value:'1','2','4','8','7','?'
typedef enum _SelectFldType{
    SINGLE_SELECT_NUMBER = '1',
    MULTI_SELECT_NUMBER = '2',
    MULTI_SELECT_CONTENT ='3',
    MULTI_SELECT_GROUPLIST ='4',
    SINGLE_SELECT_GROUPLIST ='5',
    SELECT_ALL    = '9'
}SelectFldType;

// Contact record information that used by Number lookup
typedef struct _ContInfo{
    AECHAR  *pName;         // Name string pointer
    uint16   wNameLen;      // Name buffer Len in AECHAR
    AECHAR   ringName[MAX_FILE_NAME];
    char     szPhoto[AEE_MAX_FILE_NAME];    // Photo picture
}ContInfo;

/*==============================================================================
                                 类接口定义
==============================================================================*/
typedef struct _IContApp  IContApp;

QINTERFACE(IContApp)
{
    INHERIT_IApplet(IContApp);
    int (*SaveNumber)( IContApp           *p, 
                        AECHAR             *number,
                        AECHAR             *E_Mail,  
                        AECHAR             *URL,                       
                        PhoneSaveType saveType);
                       
    int (*FindNumber)(IContApp *p, AECHAR *searchStr);
    
    int (*SelectField)( IContApp            *p, 
                        PFNSELECTEDCONTACTCB fnSelectedCallback, 
                        SelectFldType        FldType); 
    int (*NumberLookup)( IContApp   *pi,
                         AECHAR     *pNumber,
                         ContInfo   *pContInfo);
    int (*SpeedDialLookup)( IContApp   *pi,
                        AECHAR     *pSpeedDial,
                        AECHAR     *pNumber,
                        uint16      wNumLen);
    int (*SpeedDialCall)( IContApp   *pi,
                        AECHAR     *pSpeedDial);
    int (*MainMenu)( IContApp   *pi);

    int (*DeleteAll)( IContApp   *pi);
#ifdef FEATURE_SUPPORT_VCARD
    int (*ReadVcfFile)(IContApp *pi, char* fileName, int* cnt);

    int (*CreateVcfFile)(IContApp *pi, int id, char* fileName);

    int (*CreateVcfFileEx)(IContApp * pi, AEEAddrField * pFields, int nFldCnt, char* fn);

    int (*CheckCapacity)( IContApp   *pi, boolean* isFull);

#endif
};

/*==============================================================================
                                 类接口函数定义
==============================================================================*/
#define ICONTAPP_AddRef(p)              GET_PVTBL(p,IContApp)->AddRef(p)
#define ICONTAPP_Release(p)             GET_PVTBL(p,IContApp)->Release(p)
#define ICONTAPP_HandleEvent(p,ec,wp,dw)    \
                 GET_PVTBL(p,IContApp)->HandleEvent(p, ec, wp, dw)
                 
#define ICONTAPP_SaveNumber(p,n,e,u,t)          \
                 GET_PVTBL(p,IContApp)->SaveNumber(p,n,e,u,t)
                 
#define ICONTAPP_FindNumber(p,s)        GET_PVTBL(p,IContApp)->FindNumber(p,s)

#define ICONTAPP_SelectField(p,cb)          \
                 GET_PVTBL(p,IContApp)->SelectField(p, cb, SINGLE_SELECT_NUMBER)
                 
#define ICONTAPP_SelectFieldEx(p,cb,c)      \
                 GET_PVTBL(p,IContApp)->SelectField(p,cb,c)

#define ICONTAPP_NumberLookup(p,n,c)        \
                 GET_PVTBL(p,IContApp)->NumberLookup(p,n,c)
                 
#define ICONTAPP_SpeedDialLookup(p,s,n,L)   \
                 GET_PVTBL(p,IContApp)->SpeedDialLookup(p,s,n,L)
                 
#define ICONTAPP_SpeedDialCall(p,s)         \
                 GET_PVTBL(p,IContApp)->SpeedDialCall(p,s)

#define ICONTAPP_MainMenu(p) \
                 GET_PVTBL(p,IContApp)->MainMenu(p)

#define ICONTAPP_DeleteAll(p) \
                 GET_PVTBL(p,IContApp)->DeleteAll(p)
                 
#ifdef FEATURE_SUPPORT_VCARD
#define ICONTAPP_ReadVcfFile(p, fn, cnt) \
                 GET_PVTBL(p,IContApp)->ReadVcfFile(p, fn, cnt)

#define ICONTAPP_CreateVcfFile(p, id, filenane) \
                 GET_PVTBL(p,IContApp)->CreateVcfFile(p, id, filenane)
    
#define ICONTAPP_CreateVcfFileEx(p, af, fc, fn) \
                     GET_PVTBL(p,IContApp)->CreateVcfFileEx(p, af, fc,fn)

#define ICONTAPP_CheckCapacity(p, isFull) \
                 GET_PVTBL(p,IContApp)->CheckCapacity(p, isFull)

#endif

// 保持向后兼容性，重定义如下内容
typedef struct  _IContApp  IContactApp;

#define IContactApp_AddRef(p)       GET_PVTBL(p,IContactApp)->AddRef(p)
#define IContactApp_Release(p)      GET_PVTBL(p,IContactApp)->Release(p)
#define IContactApp_HandleEvent(p,ec,wp,dw) \
                 GET_PVTBL(p,IContactApp)->HandleEvent(p, ec, wp, dw)
                 
#define IContactApp_SaveNumber(p,n,m,s)     \
                 GET_PVTBL(p,IContactApp)->SaveNumber(p,n,s)
                 
#define IContactApp_FindNumber(p,s) GET_PVTBL(p,IContactApp)->FindNumber(p,s)

#define IContactApp_SelectField(p,cb)       \
                 GET_PVTBL(p,IContactApp)->SelectField(p,cb,SINGLE_SELECT_NUMBER)
                 
#define IContactApp_SelectFieldEx(p,cb,c)   \
                 GET_PVTBL(p,IContactApp)->SelectField(p,cb,c)
#endif //CONTAPP_H

