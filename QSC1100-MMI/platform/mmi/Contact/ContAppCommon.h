#ifndef CONTAPPCOMMON_H
#define CONTAPPCOMMON_H

/*==============================================================================
// 文件：
//        ContAppCommon.h
//        2007-12-05  草拟版本(Draft Version)
//        版权所有(c) 2007 Anylook
//        
// 文件说明：
//        
// 作者：2007-12-05
// 创建日期：Gemsea
// 当前版本：Draft
==============================================================================*/

/*==============================================================================
修改历史记录：
时    间      修   改   人     修改内容、位置及原因
--------      -----------     -----------------------------------------------
07-12-05                 初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/

/*==============================================================================
                                 宏定义和常数
==============================================================================*/
/*==============================================================================
                                 类型定义
==============================================================================*/
int CContApp_LoadSingleStoreCont(CContApp           *pMe,
                                 AEEAddrCat         wCategory,
                                 AEEAddrFieldID     wFieldID,
                                 AECHAR             *pData,
                                 Addr_Store_e_type  store);

int CContApp_EnumContInit(CContApp *pMe, uint32 nContIdx);
uint32 CContApp_EnumContNext(CContApp *pMe);
uint16 CContApp_RawToContID( CContApp *pMe, uint16 nRawID, boolean bCard);
uint16 CContApp_ContToRawID( CContApp *pMe, uint16 nContID);

int CContApp_AddItem(CContApp           *pMe, 
                     IMenuCtl           *pMenuCtl,
                     AEEAddCacheInfo    *pCache);
                     
int CContApp_AddSelectItem( CContApp *pMe, IMenuCtl *pMenuCtl, int flag);
int CContApp_SortByFldIDCB(void *fld1, void *fld2);
int CContApp_GetFldPri(AEEAddrFieldID wFldID);
int CContApp_FindAddrRec(CContApp *pMe, uint16 wContID);
uint16 CContApp_GetFldImgID(AEEAddrFieldID wFldID);
int CContApp_SetViewStatic( CContApp *pMe, IStatic *pStatic, int idx);
uint16 CContApp_GetFldTitleID(AEEAddrFieldID wFldID);
AEETextInputMode CContApp_GetFldInputMode(AEEAddrFieldID wFldID);
uint32 CContApp_GetFldInputProp(CContApp *pMe, AEEAddrFieldID wFldID);
uint16 CContApp_SetFldMaxSize(CContApp *pMe,ITextCtl *pIText,AEEAddrFieldID wFldID);
boolean CContApp_CheckNumber(AECHAR *pStr);
int CContApp_UpdateAddrFields(CContApp *pMe, uint16 wContID);
int CContApp_BuildAddrField( AEEAddrFieldID  wFldID, void *pData, AEEAddrField *pFld, boolean bMalloc);
uint16 CContApp_CheckSameFld( CContApp *pMe, AEEAddrFieldID wFldID, void *pData);
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
int CContApp_SendSMS(CContApp *pMe, AECHAR *pNumber, AECHAR *pText);
#endif

#if defined(FEATURE_RUIM_PHONEBOOK)
int CContApp_Copy2RUIM( CContApp *pMe, AEEAddrField *pNameFld, AEEAddrField *pNumFld);
int  CContApp_Merge2Phone( CContApp *pMe, AEEAddrField *pNameFld, AEEAddrField *pNumFld);
void CContApp_CopyAllToRUIMCB(void *pUser);
void CContApp_CopyAllToPhoneCB(void *pUser);
boolean CContApp_CheckRUIMRec( CContApp *pMe, AECHAR   *pName, AECHAR   *pNumber);
#endif /* FEATURE_RUIM_PHONEBOOK */

int CContApp_CheckFldInRec( CContApp *pMe, IAddrRec *pAddrRec, AEEAddrField *pAddrFld);
uint16 CContApp_GetInputTitle(FSMState eState);
AEETextInputMode CContApp_GetInputMode(FSMState eState);
uint32 CContApp_GetInputProp(FSMState eState);
uint16 CContApp_GetInputMaxSize(FSMState eState);
int CContApp_SetCapacityStatic(CContApp *pMe, IStatic *pStatic);
int CContApp_SetDetailStatic(CContApp *pMe, IStatic *pStatic);
int CContApp_SetGroupMenuSel(CContApp *pMe, IMenuCtl *pMenuCtl);
int CContApp_GetNameByNum( CContApp *pMe, AECHAR *pNumber, ContInfo *pContInfo);
void CContApp_DisplayRightTopStr(CContApp *pMe,IMenuCtl *pMenuCtl,uint16 wParam);
int CContApp_FilterSmartItems(CContApp *pMe, AECHAR *pStr);
int CContApp_SaveBirthdayField(CContApp *pMe);
int CContApp_SaveEditField(CContApp *pMe,AEEAddrFieldID wFldID);
int CContApp_SaveRingField(CContApp *pMe, AECHAR* ringName);
int CContApp_SaveIconField(CContApp *pMe, uint16 wIconID);
uint16 CContApp_GetIdxbyFldID(CContApp *pMe, AEEAddrFieldID wFldID);
IImage * CContApp_GetIconImage(CContApp *pMe, uint16 wIconID);
int CContApp_DrawIconImage(CContApp *pMe, uint16 wIconID, AEERect *prc);
boolean CContAppJDayToDate(long jday, int *pYear, int *pMonth, int *pDay);

int CContApp_DeleteItem(CContApp        *pMe, 
                        IMenuCtl        *pMenuCtl,
                        AEEAddCacheInfo *pCache);
                        
int CContApp_CopyItem(CContApp  *pMe, 
                      IMenuCtl  *pMenuCtl,
                      AEEAddCacheInfo *pCache);

int CContApp_SelectItem(CContApp        *pMe, 
                        IMenuCtl        *pMenuCtl,
                        AEEAddCacheInfo *pCache);

int CContApp_CopyToRUIM(CContApp *pMe, uint16 wContID);
int CContApp_CopyToPhone(CContApp *pMe, uint16 wContID);
boolean CContApp_GetRecByID(CContApp *pMe, uint16 wContID);
void CContApp_SetRecByFLDID(CContApp *pMe, uint16 wFLDID);
int CContApp_CreateCont( CContApp *pMe,  boolean bCard);
boolean CContApp_DisplaySelectField( CContApp  *pMe, byte Field);
boolean CContApp_AddSelectFieldListNode(sSelectFieldListNode* pNode, sSelectFieldInfo* pSelectFieldInfo);
boolean CContApp_AddSelectFieldListNode(sSelectFieldListNode* pNode, sSelectFieldInfo* pSelectFieldInfo);
boolean CContApp_DeleteSelectFieldListNode(sSelectFieldListNode* pNode, sSelectFieldInfo* pSelectFieldInfo);
uint16 CContApp_FindSelectFieldListNode(sSelectFieldListNode* pNode, sSelectFieldInfo* pSelectFieldInfo);
boolean CContApp_SetSelectNodeFieldMark(sSelectFieldListNode* pNode, sSelectFieldInfo* pSelectFieldInfo);
boolean CContApp_UpdateSelectFieldListNode(sSelectFieldListNode* pNode, sSelectFieldInfo* pSelectFieldInfo);
boolean CContApp_GetSelectFieldListNode(CContApp *pMe, sSelectFieldListNode* pNode);
boolean CContApp_FormatSelectFieldContentListNode(CContApp *pMe, sSelectFieldListNode* pNode);
boolean CContApp_FormatSendDirectoryContent(CContApp *pMe);
boolean CContApp_GetSelectFieldByID(CContApp *pMe, sSelectFieldListNode* pNode, uint16 wContID);
boolean CContApp_FreeSelectFieldListNode(void);
boolean CContApp_Phonebook_Load(CContApp *pMe);
AECHAR* CContApp_GetNumberByField(CContApp *pMe, int idx);
int CContApp_GetOneNumberFld(CContApp *pMe, int idx);
int CContApp_GetPrimaryNumFld(CContApp *pMe);
boolean CContApp_MarkAll(CContApp * pMe);
void CContApp_FreeBuffer(CContApp * pMe);
void CContApp_DeleteMultipe_CB(void *pUser);
void CContApp_CopyMultipe_CB(void *pUser);
void CContApp_SaveOneDial(CContApp * pMe);
FSMState CContApp_GetReturnState(CContApp * pMe);
boolean CContApp_OnedialExit(CContApp * pMe);
boolean CContApp_CheckNumLen(CContApp * pMe, uint16 selContId);
void CContApp_DrawScrollBar(CContApp * pMe, IMenuCtl   *pMenuCtl); 
void CContApp_RestoreMenuRect(CContApp * pMe, IMenuCtl   *pMenuCtl); 
uint16 CContApp_GetListReturnSel(CContApp * pMe, IMenuCtl   *pMenuCtl, int* currItemIdx);
boolean CContApp_IsPopState(CContApp * pMe);
int CContApp_GetSendListLeftNodeNum(CContApp *pMe, int *leftNum );
int CContApp_GetSelNodeMarkNum(dword mark);
#ifdef FEATURE_LANG_CHINESE
void CContApp_CNWStr2Letter(AECHAR *pDest, int nLen , AECHAR *pSrc, boolean Is2Number);
void CContApp_Letter2Alpha(AECHAR *pDest, int nLen);
#endif

int CContApp_GetSelectNode(sSelectFieldListNode* pNode, sSelectFieldInfo* sSelectFieldInfo);

int CContApp_SaveEditResult(CContApp* pMe);
int CContApp_GetRingtone( IAddrBook *pAddr,AECHAR *ringName, uint16 recId);
int CContApp_GetGroupCat(CContApp* pMe, int16 groupId);
#ifdef FEATURE_SUPPORT_VCARD
boolean ContApp_ReadVcfFile(CContApp *pMe, char* fineName, int *cnt);
#endif
boolean ContApp_DeleteAll(CContApp *pMe);

boolean CContApp_IsNumber(AECHAR *str);

boolean CContApp_IsSelField(CContApp *pMe, dword mask);

void ContApp_ChangePhoneNumberForNameCompare(AECHAR *num);

Numbermatch_e_Type ContApp_NumberMatch(const AECHAR * wstrNum1, const AECHAR * wstrNum2, int *pMatchChars);
boolean ContApp_RegionMatch(const AECHAR * wstrNum1, const AECHAR * wstrNum2);

int CContApp_Create500Cont( CContApp *pMe);

int CContApp_SmartItems(CContApp *pMe, IMenuCtl *pMenuCtl, AEEAddCacheInfo *pCache);

#endif //CONTAPPCOMMON_H

