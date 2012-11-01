/*==============================================================================
// 文件：
//        ContAppCommon.c
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
07-12-05                   初始版本
==============================================================================*/


/*==============================================================================
                                 本文件包含的外部文件
==============================================================================*/
#include "ContApp_priv.h"
#include "ContAppCommon.h"
#ifndef AEE_SIMULATOR
#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
#include "WMSApp.h"
#endif
#endif
#include "OEMHeap.h"
#include "OEMAddrBookRUIM.h"
/*==============================================================================
                                 宏定义和常数
==============================================================================*/
#ifdef FEATURE_SUPPORT_VCARD
#include "ContApp_vCard.h"
#endif

/*==============================================================================
                                 类型定义
==============================================================================*/

/*lint -e506 -e774 -e737 -e715*/
/*==============================================================================
                                 函数声明
==============================================================================*/
extern boolean AlphaNCMP(AECHAR *pStr1, AECHAR *pStr2, int nLen);
//Helper function
//////

/*=============================================================================

FUNCTION:CContApp_LoadSingleStoreCont

DESCRIPTION:
    将话机指定记录添加到 pMe->m_pAddList中
    
PARAMETERS:
    pMe:
    wCategory:
    wFieldID:
    pData:
    
RETURN VALUE:
    SUCCESS 
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_LoadSingleStoreCont(CContApp           *pMe,
                                 AEEAddrCat         wCategory,
                                 AEEAddrFieldID     wFieldID,
                                 AECHAR             *pData,
                                 Addr_Store_e_type  store)
{
    IAddrBook           *pAddrBook = NULL;
    AEEAddCacheInfo     *pCache = NULL;
    uint32              nCapacity;
    int                 nRet;
    uint16              wDataSize = 0;
    
    ASSERT(pMe != NULL); 
    
    if (pData)
    {
        wDataSize = (uint16)WSTRSIZE(pData);
    }
    
    IVector_RemoveAllElements(pMe->m_pAddList);
#ifdef FEATURE_RUIM_PHONEBOOK
    if (store == ADDR_STORE_RUIM)
    {
        pAddrBook = pMe->m_pAddrRUIM;
    }
    else
#endif        
    {
        pAddrBook = pMe->m_pAddrPhone;
    }
    nCapacity = (uint32)IADDRBOOK_GetNumRecs(pAddrBook);
    
    if (SUCCESS != IVector_EnsureCapacity(pMe->m_pAddList, nCapacity))
    {
        FARF(ADDR, ("Failed to IVector_EnsureCapacity"));
        return EFAILED;
    }
    
    // 从电话本Cache载入记录
    nRet = IADDRBOOK_EnumCacheInit(pAddrBook,
                                   wCategory,
                                   wFieldID,
                                   (void*)pData,
                                   wDataSize);
    if (SUCCESS != nRet)
    {
        FARF(ADDR, ("Failed to Enum init %d", nRet));
        return nRet;
    }
    if (SUCCESS != IADDRBOOK_EnumNextCache(pAddrBook, (void **)&pCache))
    {
        return SUCCESS;
    }
    
    while (NULL != pCache)
    {
        // add the node to Vector
        if (SUCCESS != IVector_AddElement(pMe->m_pAddList, pCache))
        {
            FARF(ADDR, ("Failed to add to IVector"));
            return EFAILED;
        }
        
        pCache = NULL;
        (void)IADDRBOOK_EnumNextCache(pAddrBook, (void **)&pCache);
    }
    
    // 注意: Cache 是已排好序的队列，这里不必排序
    return SUCCESS;
}// CContApp_LoadSingleStoreCont


/*=============================================================================

FUNCTION:CContApp_EnumContInit

DESCRIPTION:
    初始化枚举 Cont info。与 CContApp_EnumContNext 配合使用将循环的读取 
    m_pRUIMContIv 和 m_pPhoneContIv 中 Index 信息
    
PARAMETERS:
    pMe     :
    nContIdx: 第一个枚举的Index
    
RETURN VALUE:
    SUCCESS: 
    EFAILED: If the number of current cache <= MAX_NUM_MENUPOP
    
COMMENTS:
    
SIDE EFFECTS:
    pMe->m_nEnumIdx
    MAX_NUM_MENUPOP
    
SEE ALSO:

=============================================================================*/
int CContApp_EnumContInit(CContApp *pMe, uint32 nContIdx)
{
    ASSERT(pMe != NULL); 
    
    FARF(ADDR, ("Enum Cont info init"));
    
    if (IVector_Size(pMe->m_pAddList) <= MAX_NUM_MENUPOP)
    {
        FARF(ADDR, ("Record number under the MAX_NUM_MENUPOP %d ", IVector_Size(pMe->m_pAddList)));
        return EFAILED;
    }
    
    pMe->m_nEnumIdx = nContIdx;
    return SUCCESS;
}


/*=============================================================================

FUNCTION:CContApp_EnumContNext

DESCRIPTION:
    向列表菜单中添加项目
    
PARAMETERS:
    pMe       :
    pbCard[out]: 描述是否为 RUIM 卡记录
    
RETURN VALUE:
    Next cont info index
    
COMMENTS:
    
SIDE EFFECTS:
    pMe->m_nEnumIdx
    MAX_NUM_MENUPOP
    
SEE ALSO:

=============================================================================*/
uint32 CContApp_EnumContNext(CContApp *pMe)
{
    uint32 nContIdx;
    
    // Adjust the current index
    if (pMe->m_nEnumIdx >= IVector_Size(pMe->m_pAddList))
    {
        pMe->m_nEnumIdx -= IVector_Size(pMe->m_pAddList);
    }//else Remain the current value
    
    nContIdx = pMe->m_nEnumIdx;

    pMe->m_nEnumIdx++;
    return nContIdx;
}

/*=============================================================================

FUNCTION:CContApp_RawToContID

DESCRIPTION:
    将原始的Record id转化为 App ID
    
PARAMETERS:
    pMe   :
    nRawID: The ID suit of the OEM
    bCard : whether it is a RUIM record
    
RETURN VALUE:
    App ID
    
COMMENTS:
    
SIDE EFFECTS:
    App ID = RUIM ID + Phone recode number

SEE ALSO:

=============================================================================*/
uint16 CContApp_RawToContID( CContApp  *pMe,
                                    uint16     nRawID,
                                    boolean    bCard)
{
    uint16 nContID;
    
    ASSERT(pMe != NULL); 
    
    nContID = nRawID;
    
    if(!bCard)
    {
        if(nRawID >= IADDRBOOK_GetCapacity(pMe->m_pAddrPhone))
        {
            // Out of range
//            return CONTAPP_RECID_NULL;
        }
    }
#ifdef FEATURE_RUIM_PHONEBOOK
    else
    {
        if(nRawID > IADDRBOOK_GetCapacity(pMe->m_pAddrRUIM))
        {
            // Out of range
            return CONTAPP_RECID_NULL;
        }
        
        // RUIM ID start 1 convert to 0
        nContID += CONTAPP_RUIM_START_IDX -1;

    }
#endif// FEATURE_RUIM_PHONEBOOK

    return nContID;
}// CContApp_RawToContID

/*=============================================================================

FUNCTION:CContApp_ContToRawID

DESCRIPTION:
    将 App ID 转化为原始Phonebook ID
    
PARAMETERS:
    pMe   :
    nContID: The ID suit of the contact app
    
RETURN VALUE:
    原始phonebook ID
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint16 CContApp_ContToRawID( CContApp  *pMe,
                                    uint16     nContID)
{
    uint16 nRawID;
    
    ASSERT(pMe != NULL); 
    
    nRawID = nContID;
    if(nRawID >=  CONTAPP_RUIM_START_IDX)
    {
        // Is RUIM card id, RUIM ID start 1
        nRawID -= CONTAPP_RUIM_START_IDX;
        nRawID++;
    }
    
    return nRawID;
}// CContApp_ContToRawID

int CContApp_AddItem(CContApp           *pMe, 
                     IMenuCtl           *pMenuCtl,
                     AEEAddCacheInfo    *pCache)
{
    uint16     wRecID;
    CtlAddItem ai;

    ASSERT(pMe != NULL); 
    
    if (!pMenuCtl || !pCache)
    {
        return EBADPARM;
    }
    
    wRecID = CContApp_RawToContID(pMe, pCache->wRecID, (pCache->store == ADDR_STORE_RUIM));
    
    if (wRecID == CONTAPP_RECID_NULL)
    {
        FARF(ADDR, ("Phone Record id Out of range %d", wRecID));
        return EFAILED;
    }
    
    if (WSTRLEN(pCache->szName) == 0)
    {
        FARF(ADDR, ("Failed to Add No name item %d", wRecID));
        return EFAILED;
    }
    
    FARF(ADDR, ("Add phone item %d cat %d, %S", wRecID, pCache->nCat, pCache->szName));
    
    MEMSET(&ai, 0, sizeof(ai));
    ai.pText       = pCache->szName;
    ai.wItemID     = wRecID;
    ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
    ai.wImage      = (pCache->store == ADDR_STORE_RUIM ? IDB_RUIM : IDB_PHONE);
    if (FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
        return EFAILED;
    }
    
    return SUCCESS;
}// CContApp_AddItem

/*=============================================================================

FUNCTION: CContApp_SortByFldIDCB

DESCRIPTION:
    按字段类型排序
   
PARAMETERS:
    *fld1:
    *fld2:
   
RETURN VALUE:
    返回比较结果
    
COMMENTS:

SIDE EFFECTS:
    此函数用作IVector_Sort函数的IVectorSortCB参数
    
SEE ALSO:
=============================================================================*/
int CContApp_SortByFldIDCB(void *fld1, void *fld2)
{
    int re;
    int pri1;
    int pri2;
    
    if ( fld1 == NULL || fld2 == NULL ) 
    {
        return 0;
    }
    
    // Get priority
    pri1 = CContApp_GetFldPri(((AEEAddrField *)fld1)->fID);
    pri2 = CContApp_GetFldPri(((AEEAddrField *)fld2)->fID);
    
    re = pri1 - pri2;
    return re;
}// CContApp_SortByFldIDCB

/*=============================================================================

FUNCTION: CContApp_GetFldPri

DESCRIPTION:
    获取字段的优先级，优先级越小越高
   
PARAMETERS:
    wFldID : 字段类型
   
RETURN VALUE:
    该字段的优先级
    
COMMENTS:

SIDE EFFECTS:
    
SEE ALSO:
=============================================================================*/
int CContApp_GetFldPri(AEEAddrFieldID wFldID)
{
    int pri = 0;
    
    switch(wFldID)
    {
        case AEE_ADDRFIELD_NAME:
            pri = PRI_NAME;
            break;
            
        case AEE_ADDRFIELD_PHONE_PREF:
            pri = PRI_NUM_PREF;
            break;
            
        case AEE_ADDRFIELD_PHONE_HOME:
            pri = PRI_NUM_HOME;
            break;
            
        case AEE_ADDRFIELD_PHONE_WORK:
            pri = PRI_NUM_WORK;
            break;
            
        case AEE_ADDRFIELD_PHONE_FAX:
            pri = PRI_NUM_FAX;
            break;
            
        case AEE_ADDRFIELD_PHONE_CELL:
            pri = PRI_NUM_CELL;
            break;
            
        case AEE_ADDRFIELD_PHONE_GENERIC:
            pri = PRI_NUM_GENERIC;
            break;
        
        case AEE_ADDRFIELD_EMAIL:
            pri = PRI_EMAIL;
            break;
        
        case AEE_ADDRFIELD_URL:
            pri = PRI_URL;
            break;
            
        case AEE_ADDRFIELD_NOTES:
            pri = PRI_NOTES;
            break;
            
        case AEE_ADDRFIELD_PHOTO:
            pri = PRI_PHOTO;
            break;
        
        case AEE_ADDRFIELD_RINGTONE:
            pri = PRI_RING;
            break;
            
        case AEE_ADDRFIELD_BIRTHDAY:
            pri = PRI_BIRTHDAY;
            break;
            
        default:
            pri = PRI_DEFAULT;
            break;
    }
    
    return pri;
}// CContApp_SortByFldIDCB

/*=============================================================================

FUNCTION:CContApp_FindAddrRec

DESCRIPTION: 
    从数据库中载入记录到 m_pCurrRec 中

PARAMETERS:
    pMe:

RETURN VALUE:
    SUCCESS

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_FindAddrRec(CContApp *pMe, uint16 wContID)
{
    uint16 wRawID;
    
    ASSERT(pMe != NULL);
    
    // Check the parameter
    if(wContID == CONTAPP_RECID_NULL)
    {
        // Null id
        FARF(ADDR, ("Parameter is CONTAPP_RECID_NULL!"));
        return EFAILED;
    }
    
    // Convert the cont app id to raw id
    wRawID = CContApp_ContToRawID(pMe, wContID);
    
    if(wRawID == CONTAPP_RECID_NULL)
    {
        // ID out of range
        FARF(ADDR, ("Out of range CONTAPP_RECID_NULL!"));
        return EFAILED;
    }
    
    // Already loaded?
    if (pMe->m_pCurrRec != NULL) 
    {
        // 此时需要判断是否同为话机或同为RUIM卡记录
        // 如果不是则不能进行ID的比较
        if( (wRawID == wContID && pMe->m_bIsPhoneRec)
          ||(wRawID != wContID && !pMe->m_bIsPhoneRec))
        {
            if (IADDRREC_GetRecID(pMe->m_pCurrRec) == wRawID) 
            {
                FARF(ADDR, ("Same record ID %d", wRawID));
                return SUCCESS;
            }
        }
        
        IADDRREC_Release(pMe->m_pCurrRec);
        pMe->m_pCurrRec = NULL;
    }
    
    if(wRawID == wContID)
    {
        // Phone record
        FARF(ADDR, ("Get Phone Record %d",wRawID));
        pMe->m_pCurrRec = IADDRBOOK_GetRecByID(pMe->m_pAddrPhone, wRawID);
        pMe->m_bIsPhoneRec = TRUE;
    }
#ifdef FEATURE_RUIM_PHONEBOOK
    else
    {
        // RUIM record
        FARF(ADDR, ("Get RUIM Record %d",wRawID));
        pMe->m_pCurrRec = IADDRBOOK_GetRecByID(pMe->m_pAddrRUIM, wRawID);
        pMe->m_bIsPhoneRec = FALSE;
    }
#endif// FEATURE_RUIM_PHONEBOOK

    if (NULL == pMe->m_pCurrRec) 
    {
        FARF(ADDR, ("Failed to get record %d!",wRawID));
        return EFAILED;
    }
    
    return SUCCESS;
}// CContApp_FindAddrRec

/*=============================================================================

FUNCTION:CContApp_GetFldImgID

DESCRIPTION:
    获得对应 Field 的Image ID
    
PARAMETERS:
    nFldID  : 字段类型
    
RETURN VALUE:
    The resource ID of the image
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint16  CContApp_GetFldImgID(AEEAddrFieldID wFldID)
{
    switch(wFldID)
    {
        case AEE_ADDRFIELD_NAME:
            return 0;
            
        case AEE_ADDRFIELD_PHONE_PREF:
            return IDB_PHONE;
            
        case AEE_ADDRFIELD_PHONE_HOME:
            return IDB_HOMENUM;
            
        case AEE_ADDRFIELD_PHONE_WORK:
            return IDB_WORKNUM;
            
        case AEE_ADDRFIELD_PHONE_FAX:
            return IDB_FAXNUM;
            
        case AEE_ADDRFIELD_PHONE_GENERIC:
            return IDB_PHONE;
        
        case AEE_ADDRFIELD_EMAIL:
            return IDB_EMAIL;
        
        case AEE_ADDRFIELD_URL:
            return IDB_URL;
            
        case AEE_ADDRFIELD_NOTES:
            return IDB_NOTES;

        case AEE_ADDRFIELD_ADDRESS:
            return IDB_ADDRESS;

        default:
            return IDB_PHONE;
    }
}// CContApp_GetFldImgID


/*=============================================================================

FUNCTION:CContApp_SetViewStatic

DESCRIPTION:
    向View菜单中 Static 控件添加内容,同时Redraw控件
    
PARAMETERS:
    pMe     :
    pStatic : 指向Menu Control的指针
    idx     : 预览的内容index
        
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CContApp_SetViewStatic(CContApp *pMe, IStatic *pStatic, int idx)
{
    AEEAddrField *pFld;
    int32   lJulianDate;
    char    szBuf[20];
    int     pYear = 0;
    int     pMonth = 0;
    int     pDay = 0;
    char    Fmt[] = "%d.%d.%d";
    AECHAR  sz[20];
    
    
    ASSERT(pMe != NULL); 
    
    FARF(ADDR, ("Set view static"));
    
    if(pStatic == NULL)
    {
        return EBADPARM;
    }
    
    pFld = IVector_ElementAt( pMe->m_pFldIv, (uint32)idx );
    
    if(pFld == NULL)
    {
        return EFAILED;
    }

    ERR("pFld->fID = %d",pFld->fID,0,0);
    if(pFld->fID == AEE_ADDRFIELD_BIRTHDAY)
    {
        (void)WSTRTOSTR(pFld->pBuffer,szBuf,sizeof(szBuf));
        
        lJulianDate = (int32)STRTOUL(szBuf,NULL,10);
        
        (void)CContAppJDayToDate(lJulianDate, &pYear, &pMonth, &pDay);
        
        (void)SPRINTF(szBuf, Fmt, pYear, pMonth, pDay);

        (void)STRTOWSTR(szBuf, sz, sizeof(sz));
        // Set the view text
        if(FALSE == ISTATIC_SetText( pStatic,
                                     NULL,
                                     sz,
                                     AEE_FONT_NORMAL,
                                     AEE_FONT_NORMAL))
        {
            return EFAILED;
        }
    }
    else
    {
        // Set the view text
        if(FALSE == ISTATIC_SetText( pStatic,
                                     NULL,
                                     pFld->pBuffer,
                                     AEE_FONT_NORMAL,
                                     AEE_FONT_NORMAL))
        {
            return EFAILED;
        }
    }

    
    (void)ISTATIC_Redraw(pStatic);
    
    return SUCCESS;
}// CContApp_SetViewStatic


/*=============================================================================

FUNCTION:CContApp_GetFldTitleID

DESCRIPTION:
    获得对应 Field 的 title ID
    
PARAMETERS:
    nFldID  : 字段类型
    
RETURN VALUE:
    The resource ID of the string
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint16  CContApp_GetFldTitleID(AEEAddrFieldID wFldID)
{
    switch(wFldID)
    {
        case AEE_ADDRFIELD_NAME:
            return IDS_NAME;
            
        case AEE_ADDRFIELD_PHONE_PREF:
        case AEE_ADDRFIELD_PHONE_CELL:
        case AEE_ADDRFIELD_PHONE_GENERIC:
            return IDS_MOBILE;

        case AEE_ADDRFIELD_PHONE_HOME:
            return IDS_HOME;

        case AEE_ADDRFIELD_PHONE_WORK:
            return IDS_OFFICE;

        case AEE_ADDRFIELD_PHONE_FAX:
            return IDS_FAX;
        
        case AEE_ADDRFIELD_EMAIL:
            return IDS_EMAIL;
        
        case AEE_ADDRFIELD_URL:
            return IDS_URL;
            
        case AEE_ADDRFIELD_NOTES:
            return IDS_NOTES;

        case AEE_ADDRFIELD_ADDRESS:
            return IDS_ADDRESS;

        case AEE_ADDRFIELD_GROUP:
            return IDS_INPUT_GROUP;

        default:
            return 0;
    }
}// CContApp_GetFldTitleID

/*=============================================================================

FUNCTION:CContApp_GetFldInputMode

DESCRIPTION:
    获得对应 Field 的 输入法
    
PARAMETERS:
    nFldID  : 字段类型
    
RETURN VALUE:
    The input mode
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AEETextInputMode CContApp_GetFldInputMode(AEEAddrFieldID wFldID)
{
    switch(wFldID)
    {
        case AEE_ADDRFIELD_PHONE_PREF:
        case AEE_ADDRFIELD_PHONE_HOME:
        case AEE_ADDRFIELD_PHONE_WORK:
        case AEE_ADDRFIELD_PHONE_FAX:
        case AEE_ADDRFIELD_PHONE_CELL:
        case AEE_ADDRFIELD_PHONE_GENERIC:
        case AEE_ADDRFIELD_LOCATION:
        case AEE_ADDRFIELD_GROUP:
            return AEE_TM_NUMBERS;
            
        case AEE_ADDRFIELD_NAME:
        case AEE_ADDRFIELD_EMAIL:
        case AEE_ADDRFIELD_URL:
        case AEE_ADDRFIELD_NOTES:
        case AEE_ADDRFIELD_ADDRESS:
        {
        	#if defined(FEATURE_VERSION_HITZ181)
        	 boolean m_Issetmod = FALSE;
        	(void)OEM_GetConfig(CFGI_LANGUAGE_MOD,
	                          &m_Issetmod,
	                          sizeof(boolean));
	         
	         if(m_Issetmod)
	         {
	         	return AEE_TM_THAI_R;
	         }
	         else
	         {
				return AEE_TM_RAPID;
			 }
        	#else
        	    #if defined(FEATURE_LANG_ARABIC) ||defined(FEATURE_LANG_THAI)
        	    {
        	    	nv_language_enum_type language;
        	    	OEM_GetConfig( CFGI_LANGUAGE_SELECTION,&language,sizeof(language));
                    if(NV_LANGUAGE_ARABIC == language)
                    {
        	    		return AEE_TM_ARABIC;
        	    	}
        	    	else if(NV_LANGUAGE_THAI== language)
        	    	{
        	    		return AEE_TM_THAI;
        	    	}
        	    	else
        	    	{
        	    		return AEE_TM_LETTERS;
        	    	}
        	    }
        	    #elif defined(FEATURE_VERSION_MYANMAR)
        	    {
        	    	nv_language_enum_type language;
        	    	OEM_GetConfig( CFGI_LANGUAGE_SELECTION,&language,sizeof(language));
                    if(NV_LANGUAGE_MYANMAR == language)
                    {
        	    		return AEE_TM_MYANMAR;
        	    	}
        	    	else
        	    	{
        	    		return AEE_TM_LETTERS;
        	    	}
        	    }
        	    #elif  defined(FEATURE_VERSION_MTM)
        	    {
	         	nv_language_enum_type language;
    	    	OEM_GetConfig( CFGI_LANGUAGE_SELECTION,&language,sizeof(language));
                if(NV_LANGUAGE_THAI == language)
                {
	         		return AEE_TM_THAI;
	         	}
	         	else
	         	{
	         		return AEE_TM_LETTERS;
	         	}
	         	}
        	    #else
	            	#ifdef FEATURE_ALL_KEY_PAD
		    		return AEE_TM_CAPLOWER;
		    		#else
		    		return AEE_TM_LETTERS;
		    		#endif
            	#endif
        	#endif
        }
        default:
            return AEE_TM_CAPLOWER;
    }
}// CContApp_GetFldInputMode

/*=============================================================================

FUNCTION:CContApp_GetFldInputProp

DESCRIPTION:
    获得对应 Field 的输入法属性
    
PARAMETERS:
    nFldID  : 字段类型
    
RETURN VALUE:
    The property of the text control
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 CContApp_GetFldInputProp(CContApp *pMe, AEEAddrFieldID wFldID)
{
    switch(wFldID)
    {
        case AEE_ADDRFIELD_PHONE_PREF:
        case AEE_ADDRFIELD_PHONE_HOME:
        case AEE_ADDRFIELD_PHONE_WORK:
        case AEE_ADDRFIELD_PHONE_FAX:
        case AEE_ADDRFIELD_PHONE_CELL:
        case AEE_ADDRFIELD_PHONE_GENERIC:
        case AEE_ADDRFIELD_LOCATION:
        case AEE_ADDRFIELD_GROUP:
            return (TP_FIXSETRECT |TP_EDITNUMBER_PTSTRING |TP_FIXOEM | TP_USELESS_UPDOWN | TP_GRAPHIC_BG | TP_FOCUS_NOSEL);
            
        case AEE_ADDRFIELD_NAME:
            if(IS_RUIM_REC(pMe->m_wEditCont) && (ADDOREDIT_EDIT == pMe->m_nAddnewOrEdit) 
                || (ADDOREDIT_ADD == pMe->m_nAddnewOrEdit && pMe->m_nSaveType == CONTCFG_SAVETYPE_RUIM)) 

            {
                return (TP_MULTILINE | TP_FRAME | TP_FIXSETRECT | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_GRAPHIC_BG);
            }
            else
            {
                return (TP_MULTILINE | TP_FRAME | TP_FIXSETRECT | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT |  TP_GRAPHIC_BG);
            }            
            
        case AEE_ADDRFIELD_EMAIL:
        case AEE_ADDRFIELD_URL:
        case AEE_ADDRFIELD_NOTES:
        case AEE_ADDRFIELD_ADDRESS:
            if(IS_RUIM_REC(pMe->m_wEditCont) && (ADDOREDIT_EDIT == pMe->m_nAddnewOrEdit) 
                || (ADDOREDIT_ADD == pMe->m_nAddnewOrEdit && pMe->m_nSaveType == CONTCFG_SAVETYPE_RUIM)) 
            {
                return (TP_MULTILINE | TP_FRAME | TP_FIXSETRECT | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_DISPLAY_UNICODE_ASCII_COUNT | TP_GRAPHIC_BG);
            }
            else
            {
                return (TP_MULTILINE | TP_FRAME | TP_FIXSETRECT | TP_STARKEY_SWITCH | TP_DISPLAY_COUNT | TP_GRAPHIC_BG);
            }  
            
        default:
            return 0;
    }
}// CContApp_GetFldInputProp


/*=============================================================================

FUNCTION:CContApp_GetFldMaxSize

DESCRIPTION:
    获得对应 Field 的最大尺寸
    
PARAMETERS:
    nFldID  : 字段类型
    
RETURN VALUE:
    The size of the field
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint16 CContApp_SetFldMaxSize(CContApp *pMe,ITextCtl *pIText,AEEAddrFieldID wFldID)
{
    AECHAR *pwstrText;
    int nMaxchars = MAX_INPUT_NUM;
    //AEETextInputMode nInputMode;
    
    pwstrText = ITEXTCTL_GetTextPtr(pIText);

    switch(wFldID)
    {
        case AEE_ADDRFIELD_PHONE_PREF:
        case AEE_ADDRFIELD_PHONE_HOME:
        case AEE_ADDRFIELD_PHONE_WORK:
        case AEE_ADDRFIELD_PHONE_FAX:
        case AEE_ADDRFIELD_PHONE_CELL:
        case AEE_ADDRFIELD_PHONE_GENERIC:
        case AEE_ADDRFIELD_LOCATION:
        case AEE_ADDRFIELD_GROUP:
            if(IS_RUIM_REC(pMe->m_wEditCont) && (ADDOREDIT_EDIT == pMe->m_nAddnewOrEdit)
                ||(ADDOREDIT_ADD == pMe->m_nAddnewOrEdit && pMe->m_nSaveType == CONTCFG_SAVETYPE_RUIM)) 
            {
#ifndef WIN32
                nMaxchars = OEMRUIMAddr_GetRUIMMaxNumberSize();
#else
                nMaxchars = MAX_INPUT_NUM;
#endif
            }
            else
            {
                nMaxchars = MAX_INPUT_NUM;
            }          
            ITEXTCTL_SetMaxSize( pIText,nMaxchars);            
            return TRUE;
            
        case AEE_ADDRFIELD_NAME:
            if((IS_RUIM_REC(pMe->m_wEditCont) && ADDOREDIT_EDIT== pMe->m_nAddnewOrEdit)
                || (ADDOREDIT_ADD == pMe->m_nAddnewOrEdit && pMe->m_nSaveType == CONTCFG_SAVETYPE_RUIM))  
            {
#ifndef WIN32
                nMaxchars = OEMRUIMAddr_GetRUIMMaxNameSize();
#else
                nMaxchars = MAX_INPUT_NAME_EN;
#endif
            }
            else
            {
                nMaxchars = MAX_INPUT_NAME_EN;
            }
            break;
            
        case AEE_ADDRFIELD_EMAIL:
            nMaxchars = MAX_INPUT_EMAIL;
            break;
            
        case AEE_ADDRFIELD_URL:
            nMaxchars = MAX_INPUT_URL;
            break;
            
        case AEE_ADDRFIELD_NOTES:
            nMaxchars = MAX_INPUT_NOTES;
            break;

        case AEE_ADDRFIELD_ADDRESS:
            nMaxchars = MAX_INPUT_ADDRESS;
            break;
            
        default:
            ITEXTCTL_SetMaxSize( pIText,nMaxchars);            
            return FALSE;
    }

    if (((IS_RUIM_REC(pMe->m_wEditCont)&& ADDOREDIT_EDIT == pMe->m_nAddnewOrEdit) || 
        (ADDOREDIT_ADD == pMe->m_nAddnewOrEdit && pMe->m_nSaveType == CONTCFG_SAVETYPE_RUIM))
        && (HaveNoneASCIIChar(pwstrText, NULL)))
    {
        ITEXTCTL_SetMaxSize( pIText,(nMaxchars-1)/2);
    }
    else
    {

        ITEXTCTL_SetMaxSize( pIText,nMaxchars);
        
        //nInputMode = ITEXTCTL_GetInputMode (pIText, NULL);
        
        //if (IsDoubleBytesIME(nInputMode))
        //{
            //(void)ITEXTCTL_SetInputMode(pIText, AEE_TM_LETTERS);   //wangxiuqin modifly 20090304
        //}        
    }
    return TRUE;    
}// CContApp_GetFldMaxSize

/*=============================================================================

FUNCTION:CContApp_CheckNumber

DESCRIPTION:
    检查字符串是否为文本字符或可用的号码字符串
    
PARAMETERS:
    pStr  : 检查的字符串
            
RETURN VALUE:
    boolean : TRUE : Availd
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
boolean CContApp_CheckNumber(AECHAR *pStr)
{
    int i;
    if(!pStr)
    {
        return FALSE;
    }
    
    for(i=0; i<= MAX_INPUT_NUM; i++)
    {
        switch(pStr[i])
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
            case '+':
            case 'p':
            case 'P':
            //case 'w':
            //case 'W':
            case 't':
            case 'T':
            case '*':
            case '#':
                break;
                
            case '\0':
               return TRUE;
               
            default:
               return FALSE;
        }
    }
    
    return TRUE;
}// CContApp_CheckNumber

/*=============================================================================

FUNCTION: CContApp_UpdateAddrFields

DESCRIPTION:
    Update rec with the current fields in pMe->m_pFldIv
    Assumes that pMe->m_pCurrRec is valid
   
PARAMETERS:
    pMe     : pointer to CContApp object
    wContID : contact id of update
    
RETURN VALUE:
    SUCCESS
   
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
int CContApp_UpdateAddrFields(CContApp *pMe, uint16 wContID)
{
    AEEAddrField   *pFldArray;
    uint32          nFldNum;
    int             nRet;
    
    ASSERT(pMe != NULL);
    
    nFldNum = IVector_Size(pMe->m_pFldIv);
    if (0 == nFldNum)
    {
        // Can't update with no fields
        return EFAILED;
    }
    
    pFldArray = (AEEAddrField*)IVector_BuildBuffer( pMe->m_pFldIv,
                                                    sizeof(AEEAddrField));
        
    if (NULL == pFldArray)
    {
        return EFAILED;
    }
    
    ASSERT(pMe->m_pCurrRec != NULL);
    
    FARF(ADDR, ("Update all fields name len %d  %d",WSTRLEN(pFldArray[0].pBuffer), nFldNum));
    // Update all the fields of this record in address book
    nRet = IADDRREC_UpdateAllFields(pMe->m_pCurrRec, pFldArray, (int)nFldNum);
    
    FREE(pFldArray);
    pFldArray = NULL;
    
    if (SUCCESS == nRet)
    {
        // reload all the cont info
        switch(pMe->m_nViewType)
        {
            case CONTCFG_VIEWTYPE_ALL:
                (void)CContApp_LoadByCondition(pMe,(pMe->m_pFindData==NULL ? 0 : WSTRSIZE(pMe->m_pFindData)));
                break;
                
            case CONTCFG_VIEWTYPE_PHONE:
                (void) CContApp_LoadSingleStoreCont(pMe,
                                                    pMe->m_wFindCat,
                                                    pMe->m_wFindFldID,
                                                    pMe->m_pFindData,
                                                    ADDR_STORE_DBFILE);
                break;
                
#ifdef FEATURE_RUIM_PHONEBOOK
            case CONTCFG_VIEWTYPE_RUIM:
                (void) CContApp_LoadSingleStoreCont(pMe,
                                                    AEE_ADDR_CAT_NONE,
                                                    pMe->m_wFindFldID,
                                                    pMe->m_pFindData,
                                                    ADDR_STORE_RUIM);
                break;
#endif// FEATURE_RUIM_PHONEBOOK
    
            default:
                break;
        }
        
        return SUCCESS;
    }
    
    return EFAILED;
} // CContApp_UpdateAddrFields


/*=============================================================================

FUNCTION: CContApp_BuildAddrField

DESCRIPTION:
    Initializes a AEEAddrField structure with the contents of
    a FieldInfo structure.

    Set mallocData to true if the data buffer should be
    duplicated.
    
    Returns TRUE if successful.
   
PARAMETERS:
    wFldID[int] : Field type info
    pData  [in] : field data
    pFld  [out] : AEEAddrField Object pointer
    bMalloc[in] : boolean variable indicates if data should be allocated
   
RETURN VALUE:
    SUCCESS
   
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
int CContApp_BuildAddrField( AEEAddrFieldID  wFldID,
                                    void           *pData,
                                    AEEAddrField   *pFld,
                                    boolean         bMalloc)
{
	
    if ( pData == NULL || pFld == NULL) 
    {
        return EFAILED;
    }	
    
    if(wFldID == AEE_ADDRFIELD_NONE)
    {
        return EFAILED;
    }
    
    pFld->wDataLen = (uint16) WSTRSIZE(pData);	
	
    if ( bMalloc )
    {
        pFld->pBuffer = MALLOC(pFld->wDataLen);
        
        if (NULL == pFld->pBuffer)
        {
            return EFAILED;
        }
        MEMCPY(pFld->pBuffer, pData, pFld->wDataLen);
    }
    else
    {
        pFld->pBuffer = pData;
    }
    
    pFld->fID = wFldID;
    
    if(IS_NUM_FIELD(wFldID))
    {
        pFld->fType = AEEDB_FT_PHONE;
    }
    else
    {
        pFld->fType = AEEDB_FT_STRING;
    }
    
    return SUCCESS;
}// CContApp_BuildAddrField

/*=============================================================================

FUNCTION: CContApp_CheckSameFld

DESCRIPTION:
    Check whether the field have exist in current record pMe->m_pFldIv[idx]
   
PARAMETERS:
    pMe    : pointer to CContApp object
    wFldID : type of field
    pData  : data of field
    
RETURN VALUE:
    the index of same field
   
COMMENTS:

SIDE EFFECTS:
    pMe->m_pFldIv
    
SEE ALSO:
=============================================================================*/
uint16 CContApp_CheckSameFld( CContApp       *pMe,
                                     AEEAddrFieldID  wFldID,
                                     void           *pData)
{
    uint32         i;
    uint32         currNum;
    AEEAddrField  *pFld;
    
    ASSERT(pMe != NULL);
    
    if(pData == NULL) 
    {
        return CONTAPP_FLDID_NULL;
    }
    
    currNum = IVector_Size(pMe->m_pFldIv);
    
    // free the data first
    for (i = 0; i < currNum; i++)
    {
        pFld = (AEEAddrField*)IVector_ElementAt(pMe->m_pFldIv, i);
        
        if ( pFld == NULL ) 
        {
            continue;
        }
        
        if(pFld->fID == wFldID)
        {
            // 内容相同
            if(WSTRCMP((AECHAR *)pData, (AECHAR *)pFld->pBuffer) == 0)
            {
                // 同一个字段
                return((uint16)i);
            }
        }
    }
    
    return CONTAPP_FLDID_NULL;
} // CContApp_CheckSameFld

#if defined(FEATURE_WMS_APP) && !defined(FEATURE_WMSAPP_ONLYSUPPORTVMAIL)
/*=============================================================================

FUNCTION:CContApp_SendSMS

DESCRIPTION: 
    向指定的号码发送短消息

PARAMETERS:
    pMe:
    pNumber:

RETURN VALUE:
    SUCCESS
   
COMMENTS:

SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_SendSMS(CContApp *pMe, AECHAR *pNumber, AECHAR *pText )
{
    IWmsApp *pSMSApp;

    ASSERT(pMe != NULL);
    
    if ((pNumber == NULL) && (NULL == pText))
    {
        return EFAILED;
    }
    
    if (SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                          AEECLSID_WMSAPP,
                                          (void**)&pSMSApp))
    {
        return EFAILED;
    }

    if (SUCCESS != IWmsApp_SendTextMessageExt(pSMSApp, pNumber, pText))
    {
        (void)IWmsApp_Release(pSMSApp);
        pSMSApp = NULL;
        return EFAILED;
    }    
    
    (void)IWmsApp_Release(pSMSApp);
    pSMSApp = NULL;
    return SUCCESS;
}// CContApp_SendSMS
#endif

#ifdef FEATURE_RUIM_PHONEBOOK
/*=============================================================================

FUNCTION:CContApp_Copy2RUIM

DESCRIPTION: 
    复制字段到RUIM卡

PARAMETERS:
    pMe     :
    pNameFld: name field
    pNumFld : number field

RETURN VALUE:
    SUCCESS   : 
    EMAXRECORD: if the target device is full
    EEXIST    : if field have exist
    EBADPARM  : if parameter is bad
    
COMMENTS:

SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_Copy2RUIM( CContApp *pMe, 
                               AEEAddrField *pNameFld,
                               AEEAddrField *pNumFld)
{
    IAddrRec     *pAddrRec;
    AEEAddrField  addrFld[NUM_CREATE_FLDS] = {0};
    int nMaxchars = 0;
    
    
    ASSERT(pMe != NULL);
    
    // Check parameter
    if(pNameFld == NULL/* || pNumFld == NULL*/)
    {
        FARF(ADDR, ("Bad parameter at copy to RUIM"));
        return EBADPARM;
    }
    
#ifndef FEATURE_SAVE_ONLY_NAME
    if(pNumFld == NULL)
    {
        FARF(ADDR, ("Bad parameter at copy to RUIM"));
        return EBADPARM;
    }
#endif
    
    // Check the capcity
    if(IS_MAX_REC(pMe->m_pAddrRUIM))
    {
        return EMAXRECORD;
    }
/*    
    // Check the same record
    if(CContApp_CheckRUIMRec( pMe, 
                             (AECHAR *)pNameFld->pBuffer,
                             (AECHAR *)pNumFld->pBuffer))
    {
        return EEXIST;
    }
*/    
#ifndef WIN32
    nMaxchars = OEMRUIMAddr_GetRUIMMaxNameSize();
#endif
    if(WSTRLEN((AECHAR*)pNameFld->pBuffer) > nMaxchars)
    {
    	pMe->m_bNameLengthLonger = TRUE;		//Add By zzg 2012_10_31
        ((AECHAR*)pNameFld->pBuffer)[nMaxchars] = (AECHAR)'\0';
        pNameFld->wDataLen = (nMaxchars) * sizeof(AECHAR);
    }

    // Create the record
    addrFld[0] = *pNameFld;
#ifdef FEATURE_SAVE_ONLY_NAME
{
    byte itemcount = 1;

    if(pNumFld != NULL)
    {
        // Create the record
        itemcount++;
        addrFld[1] = *pNumFld;
        addrFld[1].fID = AEE_ADDRFIELD_PHONE_GENERIC;
    }

    pAddrRec = IADDRBOOK_CreateRec( pMe->m_pAddrRUIM,
                                    AEE_ADDR_CAT_NONE,
                                    addrFld,
                                    itemcount);
}
#else
    // Create the record
    addrFld[1] = *pNumFld;
    addrFld[1].fID = AEE_ADDRFIELD_PHONE_GENERIC;
    
    pAddrRec = IADDRBOOK_CreateRec( pMe->m_pAddrRUIM,
                                    AEE_ADDR_CAT_NONE,
                                    addrFld,
                                    NUM_CREATE_FLDS);
#endif

    if(NULL == pAddrRec)
    {
        FARF(ADDR, ("Failed to Create record at cpoy to RUIM"));
        return EFAILED;
    }
    
    IADDRREC_Release(pAddrRec);
    return SUCCESS;
}// CContApp_Copy2RUIM


/*=============================================================================

FUNCTION:CContApp_Merge2Phone

DESCRIPTION: 
    复制字段到话机

PARAMETERS:
    pMe     :
    pNameFld: name field
    pNumFld : number field

RETURN VALUE:
    SUCCESS   : 
    EMAXFIELD : if field type is full in current record
    EMAXRECORD: if the target device is full
    EEXIST    : if field have exist
    EBADPARM  : if parameter is bad
    
COMMENTS:

SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_Merge2Phone( CContApp *pMe, 
                                 AEEAddrField *pNameFld,
                                 AEEAddrField *pNumFld)
{
    IAddrRec     *pAddrRec;
    AEEAddrField  addrFld[NUM_CREATE_FLDS];
  //  int           nRet;
    byte          itemcount = 1;
    
    ASSERT(pMe != NULL);
    
    // Check parameter
    if(pNameFld == NULL)
    {
        return EBADPARM;
    }

#ifndef FEATURE_SAVE_ONLY_NAME
    if(pNumFld == NULL)
    {
        return EBADPARM;
    }
#endif

    // Check the capcity
    if(IS_MAX_REC(pMe->m_pAddrPhone))
    {
        return EMAXRECORD;
    }
    // Create the record
    addrFld[0] = *pNameFld;
    if(pNumFld != NULL)
    {
        addrFld[1] = *pNumFld;
        itemcount++;
    }

    pAddrRec = IADDRBOOK_CreateRec( pMe->m_pAddrPhone,
                                    AEE_ADDR_CAT_OTHER,
                                    addrFld,
                                    itemcount);
    if(NULL == pAddrRec)
    {
        return EFAILED;
    }
    
    IADDRREC_Release(pAddrRec);
    return SUCCESS;
}// CContApp_Merge2Phone

/*=============================================================================

FUNCTION:CContApp_CopyAllToRUIMCB

DESCRIPTION: 
    将当前话机的记录全部复制到 RUIM 卡时调用的Callback

PARAMETERS:
    pUser :
    
RETURN VALUE:
    NONE
    
COMMENTS:
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
void CContApp_CopyAllToRUIMCB(void *pUser)
{
    CContApp     *pMe = (CContApp *)pUser;
    uint16        i,j;
    uint16        wCurrNum;
    uint16        wFldNum;
    IAddrRec     *pAddrRec = NULL;
    AEEAddrField *pNameFld = NULL;
    AEEAddrField *pNumFld = NULL;
    
    ASSERT(pMe != NULL);
    
    if(FALSE == pMe->m_bCopyInited)
    {
        // not initilize
        return;
    }
    
    if(pMe->m_bForceComplete)
    {
        // 强制退出
        return;
    }
    
    wCurrNum = MIN(STEP_COPY, (pMe->m_wTotalNum - pMe->m_wCopyCurrID));
    
    FARF(ADDR, ("Current num %d all %d", wCurrNum, pMe->m_wTotalNum));
    
    for(i = 0; i< wCurrNum; i++)
    {
        pAddrRec = IADDRBOOK_GetRecByID( pMe->m_pAddrPhone, 
                                        (pMe->m_wCopyCurrID +i));
        if (pAddrRec == NULL)
        {
            FARF(ADDR, ("Fail to get rec %d", i));
            continue;
        }
        else
        {
            //get total fields
            wFldNum  = (uint16)IADDRREC_GetFieldCount(pAddrRec);
            
            if(wFldNum == 0)
            {
                (void)IADDRREC_Release(pAddrRec);
                pAddrRec = NULL;
                continue;
            }
            
            // Get name field
            pNameFld = IADDRREC_GetField(pAddrRec, IDX_NAME_FLD);
            
            for (j = 1; j < wFldNum; j++)
            {
                //Get number field
                pNumFld = IADDRREC_GetField(pAddrRec, j);
                if (IS_NUM_FIELD(pNumFld->fID))
                {
                    // Copy to RUIM card
                    if(SUCCESS == CContApp_Copy2RUIM(pMe, pNameFld, pNumFld))
                    {
                        // 增加记录计数
                        pMe->m_wCopyRecCnt++;
                    }
                }
            }//end for(j=0;;)
        }//end if(addrRec==NULL)
        
        IADDRREC_Release(pAddrRec);
        pAddrRec = NULL;

        if (MOVEALLTOUIM == pMe->m_nCopyMoveType)
        {
            CContApp_DeleteCont(pMe, (pMe->m_wCopyCurrID +i));
        }
        
    }
    
    if(IS_MAX_REC(pMe->m_pAddrRUIM))
    {
        // 如果达到最大容量结束复制
        wCurrNum = 0;
    }
    
    // 如果此次拷贝的数目已经小于step的值，则证明已经复制完成了
    if(wCurrNum < STEP_COPY)
    {
        pMe->m_bCopyInited = FALSE;
        
        // Have complete copy, Notify the applet
        (void)ISHELL_PostEvent( pMe->m_pShell,
                                AEECLSID_APP_CONTACT,
                                EVT_USER_COMPLETE,
                                0,
                                0);
        return;
    }
    else
    {
        // Update the information 
        (void)ISHELL_PostEvent( pMe->m_pShell,
                                AEECLSID_APP_CONTACT,
                                EVT_USER_REDRAW,
                                0,
                                0);
    }
    
    pMe->m_wCopyCurrID += STEP_COPY;
    CONTAPP_RESUMECB(&pMe->m_nCopyCB);
}// CContApp_CopyAllToRUIMCB

/*=============================================================================

FUNCTION:CContApp_CopyAllToPhoneCB

DESCRIPTION: 
    将当前话机的记录全部复制到话机时调用的Callback

PARAMETERS:
    pUser :
    
RETURN VALUE:
    NONE
    
COMMENTS:
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
void CContApp_CopyAllToPhoneCB(void *pUser)
{
    CContApp     *pMe = (CContApp *)pUser;
    uint16        i;
    uint16        wCurrNum;
    IAddrRec     *pAddrRec = NULL;
    AEEAddrField *pNameFld = NULL;
    AEEAddrField *pNumFld = NULL;
    
    ASSERT(pMe != NULL);
    
    if(FALSE == pMe->m_bCopyInited)
    {
        // not initilize
        return;
    }
    
    if(pMe->m_bForceComplete)
    {
        // 强制退出
        return;
    }
    
    wCurrNum = MIN(STEP_COPY, (pMe->m_wTotalNum - pMe->m_wCopyCurrID));
    
    FARF(ADDR, ("Current num %d all %d", wCurrNum, pMe->m_wTotalNum));
    
    // RUIM card ID start at 1
    for(i=1; i<= wCurrNum; i++)
    {
        pAddrRec = IADDRBOOK_GetRecByID( pMe->m_pAddrRUIM, 
                                        (pMe->m_wCopyCurrID +i));
        if (pAddrRec == NULL)
        {
            FARF(ADDR, ("Fail to get rec %d", i));
            continue;
        }
        else
        {
            // Get name field
            pNameFld = IADDRREC_GetField(pAddrRec, IDX_NAME_FLD);
            pNumFld  = IADDRREC_GetField(pAddrRec, IDX_NUM_RUIM_FLD);
            
            // Merge to RUIM card
            if(SUCCESS == CContApp_Merge2Phone(pMe, pNameFld, pNumFld))
            {
                // 增加记录计数
                pMe->m_wCopyRecCnt++;
            }
        }//end if(addrRec==NULL)
        
        IADDRREC_Release(pAddrRec);
        pAddrRec = NULL;
        
        if (MOVEALLTOPHONE == pMe->m_nCopyMoveType)
        {
            CContApp_DeleteCont(pMe, (pMe->m_wCopyCurrID + CONTAPP_RUIM_START_IDX -1 + i));
        }
        
    }
    
    if(IS_MAX_REC(pMe->m_pAddrPhone))
    {
        // 如果达到最大容量结束复制
        wCurrNum = 0;
    }
    
    if(wCurrNum < STEP_COPY)
    {
        pMe->m_bCopyInited = FALSE;
        
        // Have complete copy, Notify the applet
        (void)ISHELL_PostEvent( pMe->m_pShell,
                                AEECLSID_APP_CONTACT,
                                EVT_USER_COMPLETE,
                                0,
                                0);
        return;
    }
    else
    {
        // Update the information 
        (void)ISHELL_PostEvent( pMe->m_pShell,
                                AEECLSID_APP_CONTACT,
                                EVT_USER_REDRAW,
                                0,
                                0);
    }
    
    pMe->m_wCopyCurrID += STEP_COPY;
    CONTAPP_RESUMECB(&pMe->m_nCopyCB);
}// CContApp_CopyAllToPhoneCB

#endif// FEATURE_RUIM_PHONEBOOK

/*=============================================================================

FUNCTION: CContApp_CheckFldInRec

DESCRIPTION:
    获得对应 Field 的当前数量
    
PARAMETERS:
    pMe     :
    IAddrRec: 当前记录
    pAddrFld: 检查的数据
    
RETURN VALUE:
    SUCCESS   : if pass the check
    EMAXFIELD : if field type is full in current record
    EEXIST    : if field have exist
    EBADPARM  : if parameter is bad
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_CheckFldInRec( CContApp       *pMe,  
                                   IAddrRec       *pAddrRec,
                                   AEEAddrField   *pAddrFld)
{
    uint16        nCnt =0;
    int           i;
    int           nFldsNum;
    AEEAddrField *pFld;
    
    ASSERT(pMe != NULL); 
    
    if(NULL == pAddrRec || pAddrFld == NULL)
    {
        return EBADPARM;
    }
    
    nFldsNum = IADDRREC_GetFieldCount(pAddrRec);
    
    if(nFldsNum == 0)
    {
        FARF(ADDR, ("No fields to count"));
        return EFAILED;
    }
    
    for(i=0; i<nFldsNum; i++)
    {
        pFld = IADDRREC_GetField( pAddrRec, i );
        if(NULL == pFld)
        {
            continue;
        }
        
        if(IS_NUM_FIELD(pAddrFld->fID))
        {
            if(IS_NUM_FIELD(pFld->fID))
            {
                if(0 == WSTRCMP(pFld->pBuffer, pAddrFld->pBuffer))
                {
                    return EEXIST;
                }
                nCnt++;
            }
        }
        else
        {
            if(pFld->fID == pAddrFld->fID)
            {
                if(0 == WSTRCMP(pFld->pBuffer, pAddrFld->pBuffer))
                {
                    return EEXIST;
                }
                nCnt++;
            }
        }
    }
    
    if(nCnt >= CContApp_GetFldMaxNum(pAddrFld->fID))
    {
        FARF(ADDR, ("Arrive the max field limit"));
        return EMAXFIELD;
    }
    
    return SUCCESS;
}// CContApp_CheckFldInRec

/*=============================================================================

FUNCTION:CContApp_GetInputTitle

DESCRIPTION:
    获得对应状态的 input title ID
    
PARAMETERS:
    eState  : 所属状态
    
RETURN VALUE:
    The resource ID of the string
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
/*uint16  CContApp_GetInputTitle(FSMState eState)
{
    switch(eState)
    {
        case STATE_LOCKED:
            return IDS_INPUT_PASSWORD;

#ifdef FEATURE_RUIM_PHONEBOOK
        case STATE_COPYSTART:
            return IDS_INPUT_COPYSTART;
#endif// FEATURE_RUIM_PHONEBOOK

        default:
            return 0;
    }
}// CContApp_GetInputTitle

*/

/*=============================================================================

FUNCTION:CContApp_GetInputMode

DESCRIPTION:
    获得对应状态的输入法
    
PARAMETERS:
    eState  : 所属状态
    
RETURN VALUE:
    The input mode
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
AEETextInputMode CContApp_GetInputMode(FSMState eState)
{
    switch(eState)
    {
        case STATE_LOCKED:
            return AEE_TM_NUMBERS;
            
        default:
            return AEE_TM_NUMBERS;
    }
}// CContApp_GetInputMode

/*=============================================================================

FUNCTION:CContApp_GetInputProp

DESCRIPTION:
    获得对应状态的输入法属性
    
PARAMETERS:
    eState  : 所属状态
    
RETURN VALUE:
    The property of the text control
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint32 CContApp_GetInputProp(FSMState eState)
{
    switch(eState)
    {
        case STATE_LOCKED:
            return (TP_FRAME| TP_FIXSETRECT| TP_PASSWORD);

        default:
            return 0;
    }
}// CContApp_GetInputProp

/*=============================================================================

FUNCTION:CContApp_GetInputMaxSize

DESCRIPTION:
    获得对应状态的最大输入长度
    
PARAMETERS:
    eState  : 所属状态
    
RETURN VALUE:
    The size of the field
    
COMMENTS:
    
SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
uint16 CContApp_GetInputMaxSize(FSMState eState)
{
    switch(eState)
    {
        case STATE_LOCKED:
            return MAX_LEN_PASSWORD;

        default:
            return 0;
    }
}// CContApp_GetInputMaxSize


/*=============================================================================

FUNCTION:CContApp_SetCapacityStatic

DESCRIPTION:
    向Capacity dialog中 Static 控件添加内容
    
PARAMETERS:
    pMe      :
    pStatic  : 指向 static Control的指针
        
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CContApp_SetCapacityStatic(CContApp *pMe, IStatic *pStatic)
{
    AECHAR enter[] = {(AECHAR)'\n', (AECHAR)'\0'};
    AECHAR temp[MAX_LEN_COMMBUF];
    
    ASSERT(pMe != NULL); 
    
    //Load Phone capacity format
    (void)ISHELL_LoadResString ( pMe->m_pShell, 
                                 CONTAPP_RES_FILE_LANG, 
                                 IDS_CAPACITY_PHONE,
                                 pMe->m_szFmtBuf,
                                 sizeof(pMe->m_szFmtBuf));
    
    // 生成提示字符串
    WSPRINTF( temp,
              sizeof(temp),
              pMe->m_szFmtBuf,
              IADDRBOOK_GetNumRecs(pMe->m_pAddrPhone),
              IADDRBOOK_GetCapacity(pMe->m_pAddrPhone));
    
    // Copy to comm buffer
    WSTRCPY(pMe->m_szBuf, temp);
    
    // 换行
    WSTRCAT(pMe->m_szBuf, enter);
    
    //Load Phone capacity format
    (void)ISHELL_LoadResString ( pMe->m_pShell, 
                                 CONTAPP_RES_FILE_LANG, 
                                 IDS_CAPACITY_RUIM,
                                 pMe->m_szFmtBuf,
                                 sizeof(pMe->m_szFmtBuf));

#ifdef FEATURE_RUIM_PHONEBOOK
    if( IsRunAsUIMVersion() && pMe->m_bADNRUIMSupport )
    {
        // 生成提示字符串
        WSPRINTF( temp,
                  sizeof(temp),
                  pMe->m_szFmtBuf, 
                  IADDRBOOK_GetNumRecs(pMe->m_pAddrRUIM),
                  IADDRBOOK_GetCapacity(pMe->m_pAddrRUIM));
                  
        WSTRCAT(pMe->m_szBuf, temp);
    }
#endif// FEATURE_RUIM_PHONEBOOK

/*
    //Load title
    (void)ISHELL_LoadResString ( pMe->m_pShell, 
                                 CONTAPP_RES_FILE_LANG, 
                                 IDS_CAPACITY,
                                 pMe->m_pTitle,
                                 sizeof(AECHAR)*(MAX_LEN_TITLE));
*/
    if(FALSE == ISTATIC_SetText( pStatic,
                                 NULL,
                                 pMe->m_szBuf,
                                 AEE_FONT_NORMAL,
                                 AEE_FONT_NORMAL))
    {
        return EFAILED;
    }
    
    return SUCCESS;
}// CContApp_SetCapacityStatic

/*=============================================================================

FUNCTION:CContApp_SetDetailStatic

DESCRIPTION:
    向detail dialog中 Static 控件添加内容
    
PARAMETERS:
    pMe      :
    pStatic  : 指向 static Control的指针
        
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CContApp_SetDetailStatic(CContApp *pMe, IStatic *pStatic)
{
    AECHAR enter[] = {(AECHAR)'\n', (AECHAR)'\0'};
    AECHAR space[] = {(AECHAR)' ',(AECHAR)':',(AECHAR)' ',(AECHAR)'\0'};   
    int len = 0;
    AECHAR *m_sString = (AECHAR *)MALLOC(1024* sizeof(AECHAR));
    AECHAR *m_sMobile = (AECHAR *)MALLOC(AEE_ADDR_CAT_MAX_CHAR * sizeof(AECHAR));
    AECHAR *m_sHome = (AECHAR *)MALLOC(AEE_ADDR_CAT_MAX_CHAR * sizeof(AECHAR));
    AECHAR *m_sOffice = (AECHAR *)MALLOC(AEE_ADDR_CAT_MAX_CHAR * sizeof(AECHAR));
    AECHAR *m_sFax = (AECHAR *)MALLOC(AEE_ADDR_CAT_MAX_CHAR * sizeof(AECHAR));
    AECHAR *m_sAddress = (AECHAR *)MALLOC(AEE_ADDR_CAT_MAX_CHAR * sizeof(AECHAR));
    AECHAR *m_sURL = (AECHAR *)MALLOC(AEE_ADDR_CAT_MAX_CHAR * sizeof(AECHAR));
    AECHAR *m_sEMail = (AECHAR *)MALLOC(AEE_ADDR_CAT_MAX_CHAR * sizeof(AECHAR));
    AECHAR *m_sNotes = (AECHAR *)MALLOC(AEE_ADDR_CAT_MAX_CHAR * sizeof(AECHAR));
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    const AECHAR LRMarks[2] = {0x200E,0};
    AEEDeviceInfo DeviceInfo;
    
    ISHELL_GetDeviceInfo(pMe->m_pShell, &DeviceInfo);
#endif

    if ((NULL == m_sString) ||
        (NULL == m_sMobile) ||
        (NULL == m_sHome) ||
        (NULL == m_sOffice) ||
        (NULL == m_sFax) ||
        (NULL == m_sAddress) ||
        (NULL == m_sURL) ||
        (NULL == m_sEMail) ||
        (NULL == m_sNotes)) 
    {
        goto CContApp_SetDetailStatic_EXIT;
    }
    
    CContApp_GetRecByID(pMe, pMe->m_wSelectCont);

    len = ISHELL_LoadResString ( pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_MOBILE,  m_sMobile,
                        sizeof(AECHAR)*(AEE_ADDR_CAT_MAX_CHAR -1));
    if(len <= 0 )
    {
        return NULL;
    }                
    if(WSTRLEN(m_sString)<1024)
    {
        WSTRCPY(m_sString, m_sMobile);
        WSTRCAT(m_sString, space);
		
        if (pMe->m_pAddNewMobile)
        {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
            if((LNG_HEBREW == DeviceInfo.dwLang) || (LNG_ARABIC== DeviceInfo.dwLang))
            {
                WSTRCAT(m_sString, LRMarks);//LRM(U+200E) 左到右文字标记(英文)
                WSTRCAT(m_sString, pMe->m_pAddNewMobile);
                WSTRCAT(m_sString, LRMarks);//LRM(U+200E) 左到右文字标记(英文)
            }
            else
#endif
          WSTRCAT(m_sString, pMe->m_pAddNewMobile);
          FREEIF(pMe->m_pAddNewMobile);
        }
        WSTRCAT(m_sString, enter);

    }  
    if(!IS_RUIM_REC(pMe->m_wSelectCont))
    {

        len = ISHELL_LoadResString ( pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_HOME,  m_sHome,
                            sizeof(AECHAR)*(AEE_ADDR_CAT_MAX_CHAR -1));
        if(len <= 0 )
        {
            return NULL;
        }                

        len = ISHELL_LoadResString ( pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_OFFICE,  m_sOffice,
                            sizeof(AECHAR)*(AEE_ADDR_CAT_MAX_CHAR -1));
        if(len <= 0 )
        {
            return NULL;
        }                

        len = ISHELL_LoadResString ( pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_FAX,  m_sFax,
                            sizeof(AECHAR)*(AEE_ADDR_CAT_MAX_CHAR -1));
        if(len <= 0 )
        {
            return NULL;
        }                

        len = ISHELL_LoadResString ( pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_ADDRESS,  m_sAddress,
                            sizeof(AECHAR)*(AEE_ADDR_CAT_MAX_CHAR -1));
        if(len <= 0 )
        {
            return NULL;
        }                

        len = ISHELL_LoadResString ( pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_URL,  m_sURL,
                            sizeof(AECHAR)*(AEE_ADDR_CAT_MAX_CHAR -1));
        if(len <= 0 )
        {
            return NULL;
        }                

        len = ISHELL_LoadResString ( pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_EMAIL,  m_sEMail,
                            sizeof(AECHAR)*(AEE_ADDR_CAT_MAX_CHAR -1));
        if(len <= 0 )
        {
            return NULL;
        }                

        len = ISHELL_LoadResString ( pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_NOTES,  m_sNotes,
                            sizeof(AECHAR)*(AEE_ADDR_CAT_MAX_CHAR -1));
        if(len <= 0 )
        {
            return NULL;
        }                

        if(WSTRLEN(m_sString)<1024)
        {
            WSTRCAT(m_sString, m_sHome);
            WSTRCAT(m_sString, space);
            if (pMe->m_pAddNewHome)
            {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
                if((LNG_HEBREW == DeviceInfo.dwLang) || (LNG_ARABIC== DeviceInfo.dwLang))
                {
                    WSTRCAT(m_sString, LRMarks);//LRM(U+200E) 左到右文字标记(英文)
                    WSTRCAT(m_sString, pMe->m_pAddNewHome);
                    WSTRCAT(m_sString, LRMarks);//LRM(U+200E) 左到右文字标记(英文)
                }
                else
#endif
                WSTRCAT(m_sString, pMe->m_pAddNewHome);
                FREEIF(pMe->m_pAddNewHome);
            }
            WSTRCAT(m_sString, enter);
        }
    
        if(WSTRLEN(m_sString)<1024)
        {
            WSTRCAT(m_sString, m_sOffice);
            WSTRCAT(m_sString, space);
            if (pMe->m_pAddNewOffice)
            {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
                if((LNG_HEBREW == DeviceInfo.dwLang) || (LNG_ARABIC== DeviceInfo.dwLang))
                {
                    WSTRCAT(m_sString, LRMarks);//LRM(U+200E) 左到右文字标记(英文)
                    WSTRCAT(m_sString, pMe->m_pAddNewOffice);
                    WSTRCAT(m_sString, LRMarks);//LRM(U+200E) 左到右文字标记(英文)
                }
                else
#endif
                WSTRCAT(m_sString,  pMe->m_pAddNewOffice);
                FREEIF(pMe->m_pAddNewOffice);
            }
            WSTRCAT(m_sString, enter);
        }
         
        if(WSTRLEN(m_sString)<1024)
        {
            WSTRCAT(m_sString, m_sFax);
            WSTRCAT(m_sString, space);
            if (pMe->m_pAddNewFax)
            {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
                if((LNG_HEBREW == DeviceInfo.dwLang) || (LNG_ARABIC== DeviceInfo.dwLang))
                {
                    WSTRCAT(m_sString, LRMarks);//LRM(U+200E) 左到右文字标记(英文)
                    WSTRCAT(m_sString, pMe->m_pAddNewFax);
                    WSTRCAT(m_sString, LRMarks);//LRM(U+200E) 左到右文字标记(英文)
                }
                else
#endif
                WSTRCAT(m_sString, pMe->m_pAddNewFax);
                FREEIF(pMe->m_pAddNewFax);
            }
            WSTRCAT(m_sString, enter);
        }
    
        if(WSTRLEN(m_sString)<1024)
        {
            WSTRCAT(m_sString, m_sEMail);
            WSTRCAT(m_sString, space);
            if (pMe->m_pAddNewEMAIL)
            {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
                if((LNG_HEBREW == DeviceInfo.dwLang) || (LNG_ARABIC== DeviceInfo.dwLang))
                {
                    WSTRCAT(m_sString, LRMarks);//LRM(U+200E) 左到右文字标记(英文)
                    WSTRCAT(m_sString, pMe->m_pAddNewEMAIL);
                    WSTRCAT(m_sString, LRMarks);//LRM(U+200E) 左到右文字标记(英文)
                }
                else
#endif
               WSTRCAT(m_sString, pMe->m_pAddNewEMAIL);
               FREEIF(pMe->m_pAddNewEMAIL); 
            }
            WSTRCAT(m_sString, enter);
        }
    
        if(WSTRLEN(m_sString)<1024)
        {
            WSTRCAT(m_sString, m_sAddress);
            WSTRCAT(m_sString, space);
            if (pMe->m_pAddNewAddress)
            {
                WSTRCAT(m_sString, pMe->m_pAddNewAddress);
                FREEIF(pMe->m_pAddNewAddress); 
             }
            WSTRCAT(m_sString, enter);
        }
    
        if(WSTRLEN(m_sString)<1024)
        {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE

            if(LNG_ARABIC == DeviceInfo.dwLang)

            {

                const AECHAR RLMarks[2] = {0x200F,0};

                WSTRCAT(m_sString, RLMarks);//RLM(U+200F)  right to left 文字标记(Arabic)

                WSTRCAT(m_sString, m_sURL);

                    WSTRCAT(m_sString, space);

                WSTRCAT(m_sString, RLMarks);//RLM(U+200F) right to left文字标记(Arabic)

            }

            else

#endif

            {

                  WSTRCAT(m_sString, m_sURL);

                  WSTRCAT(m_sString, space);

              }


            if (pMe->m_pAddNewURL)
            {
#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
                if((LNG_HEBREW == DeviceInfo.dwLang) || (LNG_ARABIC== DeviceInfo.dwLang))
                {
                    WSTRCAT(m_sString, LRMarks);//LRM(U+200E) 左到右文字标记(英文)
                    WSTRCAT(m_sString, pMe->m_pAddNewURL);
                    WSTRCAT(m_sString, LRMarks);//LRM(U+200E) 左到右文字标记(英文)
                }
                else
#endif
                WSTRCAT(m_sString, pMe->m_pAddNewURL);
                FREEIF(pMe->m_pAddNewURL);      
            }
            WSTRCAT(m_sString, enter);
        }
    
        if(WSTRLEN(m_sString)<1024)
        {
            WSTRCAT(m_sString, m_sNotes);
            WSTRCAT(m_sString, space);
            if (pMe->m_pAddNewRemark)
            {
                WSTRCAT(m_sString, pMe->m_pAddNewRemark);
                FREEIF(pMe->m_pAddNewRemark);   
            }
            WSTRCAT(m_sString, enter);
        }
    }

#ifdef FEATURE_ARPHIC_LAYOUT_ENGINE
    //ISTATIC_SetProperties(pStatic, ISTATIC_GetProperties(pStatic) & (~(ST_DISPLAY_CONT_DETAIL)));
    if(LNG_ENGLISH == DeviceInfo.dwLang)
    {
        // 设置静态文本控件属性
        ISTATIC_SetProperties(pStatic, ST_DISPLAY_CONT_DETAIL | ISTATIC_GetProperties(pStatic));
    }
#endif
    // Set the view text
      if(FALSE == ISTATIC_SetText( pStatic,
                                 NULL,
                                 m_sString,
                                 AEE_FONT_NORMAL,
                                 AEE_FONT_NORMAL))
      {
          return EFAILED;
      }
   (void)ISTATIC_Redraw(pStatic);
   
CContApp_SetDetailStatic_EXIT:
     FREEIF(m_sMobile);
     FREEIF(m_sHome);
     FREEIF(m_sOffice);
     FREEIF(m_sFax);
     FREEIF(m_sEMail);
     FREEIF(m_sAddress);
     FREEIF(m_sURL);
     FREEIF(m_sNotes);
     FREEIF(m_sString);

    return SUCCESS;
}// CContApp_SetDetailStatic


/*=============================================================================

FUNCTION:CContApp_SetGroupMenuSel

DESCRIPTION:
    设置group dialog中 Menu 控件的 select item
    
PARAMETERS:
    pMe      :
    pMenuCtl : 指向Menu Control的指针
        
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CContApp_SetGroupMenuSel(CContApp *pMe, IMenuCtl *pMenuCtl)
{
    AEEAddrCat nCat;
    uint16     wSel;
    
    ASSERT(pMe != NULL);
    
    // Check parameter
    if(pMenuCtl == NULL)
    {
        return EBADPARM;
    }
    
    // Check current record is exist
    if(pMe->m_pCurrRec == NULL)
    {
        nCat = AEE_ADDR_CAT_NONE;
    }
    else
    {
        nCat = IADDRREC_GetCategory(pMe->m_pCurrRec);
    }
    
    // ensure the select item
    switch(nCat)
    {
        case AEE_ADDR_CAT_PERSONAL:
            wSel = IDS_GROUP_PERSONAL;
            pMe->m_wSelectGroup = CONTCFG_GROUP1;
            break;
            
        case AEE_ADDR_CAT_BUSINESS:
            wSel = IDS_GROUP_BUSINESS;
            pMe->m_wSelectGroup = CONTCFG_GROUP2;
            break;
            
        case AEE_ADDR_CAT_VIP:
            wSel = IDS_GROUP_VIP;
            pMe->m_wSelectGroup = CONTCFG_GROUP3;
            break;
            
        case AEE_ADDR_CAT_HOME:
            wSel = IDS_GROUP_HOME;
            pMe->m_wSelectGroup = CONTCFG_GROUP4;
            break;
            
        case AEE_ADDR_CAT_FRIEND:
            wSel = IDS_GROUP_FRIEND;
            pMe->m_wSelectGroup = CONTCFG_GROUP5;
            break;
            
        case AEE_ADDR_CAT_WORK:
            wSel = IDS_GROUP_WORK;
            pMe->m_wSelectGroup = CONTCFG_GROUP6;
            break;
            
        case AEE_ADDR_CAT_NONE:
        default:
            wSel = IDS_GROUP_NONE_STRING;
            pMe->m_wSelectGroup = 0;
            break;
    }
    
    IMENUCTL_SetSel(pMenuCtl, wSel);
    return SUCCESS;
}// CContApp_SetGroupMenuSel

/*==============================================================================

函数: CContApp_GetNameByNum
       
说明: 
    查找对应号码的姓名信息
       
参数: 
    pi       [in]    : IContApp接口对象指针。
    pNumber  [in]    : Number
    pContInfo[in/out]: 查询到的信息输出
返回值:
    SUCCESS: 如果存在该记录
       
备注:
       
==============================================================================*/
int CContApp_GetNameByNum( CContApp   *pMe,
                                  AECHAR     *pNumber,
                                  ContInfo   *pContInfo)
{
    IAddrBook  *pAddr = NULL;
    AEEAddCacheInfo info = {0};
    AEEAddCacheInfo *pUseinfo = NULL;
#ifdef FEATURE_RUIM_PHONEBOOK
    AEEAddCacheInfo uiminfo = {0};
    boolean    bNeedSearchUIM = FALSE;
#endif
    
    int nRet;
    MSG_FATAL("CContApp_GetNameByNum Start",0,0,0);
    ASSERT(pMe != NULL); 

    if ((pNumber == NULL) || (NULL == pContInfo))
    {
        return EFAILED;
    }
    
    if (WSTRLEN(pNumber) <= 1)
    {
        // 小于1位的号码不查找
        MSG_FATAL("CContApp_GetNameByNum WSTRLEN(pNumber) <= 1",0,0,0);
        return EFAILED;
    }

    // Create IAddrbokk instance for phone
    if (SUCCESS != ISHELL_CreateInstance((AEE_GetShell()),
                                         AEECLSID_ADDRBOOK,
                                         (void **)&pAddr))
    {
        MSG_FATAL("Create IAddrbokk instance Failed",0,0,0);
        return EFAILED;
    }
    
    nRet = IADDRBOOK_GetCacheinfoByNumber(pAddr, pNumber, &info, ContApp_NumberMatch);
    (void) IADDRBOOK_Release(pAddr);
    pAddr = NULL;
    
    if (nRet != SUCCESS)
    {
#ifdef FEATURE_RUIM_PHONEBOOK
        if (IsRunAsUIMVersion() && pMe->m_bADNRUIMSupport)
        {
            bNeedSearchUIM = TRUE;
        }
#endif
        
    }
#ifdef FEATURE_RUIM_PHONEBOOK
    else if ((info.szNumber != NULL) &&
             (ContApp_NumberMatch(info.szNumber, pNumber, NULL) != NUMBERMATCH_EQUAL) &&
             IsRunAsUIMVersion() && pMe->m_bADNRUIMSupport)
    {
        pUseinfo = &info;
        bNeedSearchUIM = TRUE;
    }
#endif    
    else
    {
        pUseinfo = &info;
    }
    
#ifdef FEATURE_RUIM_PHONEBOOK
    if (bNeedSearchUIM)
    {
        if (SUCCESS == ISHELL_CreateInstance((AEE_GetShell()),
                                             AEECLSID_ADDRBOOK_RUIM,
                                             (void **)&pAddr))
        {
            FREEIF(info.szName);
            FREEIF(info.szNumber);
           // return EFAILED;
        }
        
        nRet = IADDRBOOK_GetCacheinfoByNumber(pAddr, pNumber, &uiminfo, ContApp_NumberMatch);
        (void) IADDRBOOK_Release(pAddr);
        pAddr = NULL;
        
        if (nRet == SUCCESS)
        {
            int nLen_uim = 0;
            int nLen_Ph = 0;
            Numbermatch_e_Type eRet_uim = {0};
            Numbermatch_e_Type eRet_ph = {0};
            
            eRet_uim = ContApp_NumberMatch(uiminfo.szNumber, pNumber, &nLen_uim);
            if (pUseinfo != NULL)
            {
                eRet_ph = ContApp_NumberMatch(pUseinfo->szNumber, pNumber, &nLen_Ph);
            }
            
            switch (eRet_uim)
            {
                // 匹配度最高
                case NUMBERMATCH_EQUAL:
                    pUseinfo = &uiminfo;
                    break;
                    
                case NUMBERMATCH_WSTR1_ISTAIL_OF_WSTR2:
                    if (NULL == pUseinfo)
                    {
                        pUseinfo = &uiminfo;
                    }
                    else
                    {
                        switch (eRet_ph)
                        {
                            case NUMBERMATCH_WSTR1_ISTAIL_OF_WSTR2:
                                if (nLen_uim>nLen_Ph)
                                {// UIM 上的号码更匹配
                                    pUseinfo= &uiminfo;
                                }
                                break;
                                
                            case NUMBERMATCH_WSTR2_ISTAIL_OF_WSTR1:
                            default:
                                break;
                        }
                    }
                    break;

                case NUMBERMATCH_WSTR2_ISTAIL_OF_WSTR1:
                    if (NULL == pUseinfo)
                    {
                        pUseinfo = &uiminfo;
                    }
                    break;
                
                default:
                    break;
            }
        }
    }
#endif
    
    nRet = EFAILED;
    if (NULL != pUseinfo)
    {
        CContApp_GetRingtone(pAddr, pContInfo->ringName, pUseinfo->wRecID);
        // MSG_FATAL("pContInfo->ringName=%d",sizeof(pContInfo->ringName),0,0); 
         
        if (NULL != pUseinfo->szName)
        {
            
            if (pContInfo->wNameLen > WSTRLEN(pUseinfo->szName))
            {
                //MSG_FATAL("pContInfo->wNameLen=%d--pUseinfo->szName=%d",pContInfo->wNameLen,WSTRLEN(pUseinfo->szName),0); 
                (void)WSTRCPY(pContInfo->pName, pUseinfo->szName);
            }
        }
        nRet = SUCCESS;
    }
    FREEIF(info.szName);
    FREEIF(info.szNumber);
#ifdef FEATURE_RUIM_PHONEBOOK
    FREEIF(uiminfo.szName);
    FREEIF(uiminfo.szNumber);
#endif    
   MSG_FATAL("Create CContApp_GetNameByNum End",0,0,0); 
   return nRet;
} // CContApp_GetNameByNum

/*=============================================================================

FUNCTION: CContApp_DisplayRightTopStr

DESCRIPTION: 
    显示右上角的提示信息

PARAMETERS:
    pMe:
    pDispStr:
    
RETURN VALUE:
    NULL
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
void CContApp_DisplayRightTopStr(CContApp *pMe,IMenuCtl *pMenuCtl,uint16 wParam)
{
    AEERect rc;
    AECHAR wStrBuf[64] = {(AECHAR)'\0'};//use but find other err :key seem no active when second press
    RGBVAL  oldColor; 
    //static uint16 wField = MASK_ADDRFIELD_PHONE_GENERIC;
    
    ASSERT(pMe != NULL);

    // Check Parameter
    if(!pMenuCtl)
        return;
 /*   
    if(SMART_STATE_IDD_SELECT == pMe->m_nSmartStateType)
    {
        switch(wParam)
        {
            case AVK_LEFT:
            case AVK_RIGHT:
            case AVK_SELECT:  
                CContApp_DisplaySelectField(pMe, pMe->m_wFieldIndex);
                break;

            default:
                pMe->m_wSelectSmart = IMENUCTL_GetSel(pMenuCtl);            
                CContApp_GetRecByID(pMe, pMe->m_wSelectSmart);
                wField = MASK_ADDRFIELD_NAME;
                while(1)
                {
                    if (pMe->m_wFieldMask & (wField<<1))
                    {
                        wField = wField<<1;
                        pMe->m_wFieldIndex = wField;
                        break;
                    }
                    else
                    {
                        wField = wField<<1;
                    }
                }   
               CContApp_DisplaySelectField(pMe, pMe->m_wFieldIndex);            
               break;        
        }
    }      
 */
 //wuquan. tnag end
 
    if(pMe->m_bInsmartnofind)
    {
        if(pMenuCtl)
        {
            IMENUCTL_GetRect(pMenuCtl, &rc);
        }

        rc.y = 0;//pMe->m_nLineHeight * 2;
        /*
        if (SMART_STATE_IDD_SELECT != pMe->m_nSmartStateType)
        {
            rc.dy -= GetTitleBarHeight(pMe->m_pDisplay);//pMe->m_nLineHeight * 2; 
        }
        */
        rc.dy -= 0;
        //IDISPLAY_FillRect(pMe->m_pDisplay, &rc, RGB_BLACK);
#ifdef FEATURE_CARRIER_CHINA_VERTU
        {
            IImage *pImageBg = ISHELL_LoadResImage(pMe->m_pShell, AEE_APPSCOMMONRES_IMAGESFILE, IDI_CONTACT_BACKGROUND);
            
            Appscommon_ResetBackground(pMe->m_pDisplay, pImageBg, APPSCOMMON_BG_COLOR, &rc, 0, 0);
            if(pImageBg != NULL)
            {
                IImage_Release(pImageBg);
            }
        }
#else
        Appscommon_ResetBackgroundEx(pMe->m_pDisplay, &rc, TRUE);
#endif
        (void)ISHELL_LoadResString(pMe->m_pShell,
                                CONTAPP_RES_FILE_LANG,
                                IDS_MSG_FINDNULL,
                                wStrBuf,sizeof(wStrBuf));//no use 
        oldColor = IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, RGB_WHITE);
        (void)IDISPLAY_DrawText( pMe->m_pDisplay,
                                                            AEE_FONT_NORMAL, //AEE_FONT_BOLD,
                                                            wStrBuf,
                                                            -1,
                                                            rc.x,
                                                            rc.y,
                                                            &rc,
                                                            IDF_TEXT_TRANSPARENT);
        IDISPLAY_SetColor(pMe->m_pDisplay, CLR_USER_TEXT, oldColor);
    } 
    IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE);
}// CContApp_DisplayRightTopStr


/*=============================================================================

FUNCTION:CContApp_FilterSmartItems

DESCRIPTION:
    从现有列表中过滤出符合智能查找的记录
    
PARAMETERS:
    pMe   :
    pStr  : search data Alpha string '*' is wildcard
    
RETURN VALUE:
    SUCCESS 
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int CContApp_FilterSmartItems(CContApp *pMe, AECHAR *pStr)
{
    int             i;
    AEEAddCacheInfo *pCache=NULL;
    int32           nListItems = 0;
    boolean toNumber = FALSE;
    
#ifdef FEATURE_LANG_CHINESE
    AECHAR  alpha[MAX_INPUT_NAME_EN +1];
#endif

    ASSERT(pMe != NULL);
    
    if (!pStr)
    {
        return EBADPARM;
    }

    //pMe->m_nCurrSmartIdx = 0;
    // Free data
    FREEIF(pMe->m_pSmartFindBuf);
    pMe->m_pSmartBufLen = 0;
    nListItems = IVector_Size(pMe->m_pAddList);
    pMe->m_pSmartFindBuf = MALLOC(nListItems*sizeof(uint32));
    if(pMe->m_pSmartFindBuf == NULL)
    {
        FARF(ADDR, ("No memeory when load by smart"));
        return ENOMEMORY;
    }

#ifdef FEATURE_LANG_CHINESE
    if(CContApp_IsNumber(pStr))
    {
        int len = WSTRLEN(pStr);
        toNumber = TRUE;
        CContApp_Letter2Alpha(pStr, len);
    }
#endif
    // Search from Add vector
    for (i=0; i<nListItems; i++)
    {
        pCache = (AEEAddCacheInfo *)IVector_ElementAt(pMe->m_pAddList, (uint32)i);
        // Compare the string
        if (NULL != pCache)
        {
#ifdef FEATURE_LANG_CHINESE
{

            CContApp_CNWStr2Letter(alpha, MAX_INPUT_NAME_EN, pCache->szName, toNumber);
}
            if(TRUE == AlphaNCMP(alpha, pStr, (int)WSTRLEN(pStr)))
#else
            if(TRUE == AlphaNCMP(pCache->szName, pStr, (int)WSTRLEN(pStr)))
#endif
            {
                // find the data then save it
                pMe->m_pSmartFindBuf[pMe->m_pSmartBufLen] = (uint32)i;
                pMe->m_pSmartBufLen++;
            }
        }
    }

    FARF(ADDR, ("Load Phone book by smart find %d", pMe->m_pSmartBufLen));
    
    // No match the record
    if(pMe->m_pSmartBufLen == 0)
    {
        return EFAILED;
    }
    
    return SUCCESS;
}// CContApp_FilterSmartItems

/*=============================================================================

FUNCTION:CContApp_SaveBirthdayField

DESCRIPTION:
    保存生日字段
    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    
RETURN VALUE:
    SUCCESS: 
    EFAILED: If the number of current cache is 0
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CContApp_SaveBirthdayField(CContApp *pMe)
{
    int     nRet;
    //AECHAR *pData;
    uint16  wIdx;
    
    ASSERT(pMe != NULL);
    
    //pData = CContApp_GetBufbyFldID(pMe, AEE_ADDRFIELD_BIRTHDAY);
    wIdx = CContApp_GetIdxbyFldID(pMe, AEE_ADDRFIELD_BIRTHDAY);
    
    ERR("wIdx = %d",wIdx,0,0);
    if(CONTAPP_FLDID_NULL == wIdx)
    {
        // Add a new field
        nRet = CContApp_AddField( pMe,
                                  pMe->m_wEditCont,
                                  AEE_ADDRFIELD_BIRTHDAY,
                                  (void *)pMe->m_pFldInputBuf);
    }
    else
    {
        // Update the field
        nRet = CContApp_SaveField( pMe,
                                   pMe->m_wEditCont,
                                   wIdx,
                                   AEE_ADDRFIELD_BIRTHDAY,
                                   (void *)pMe->m_pFldInputBuf);
    }
    
    return nRet;
}// CContApp_SaveBirthdayField

/*=============================================================================

FUNCTION:CContApp_SaveEditField

DESCRIPTION:
保存编辑字段    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    
RETURN VALUE:
    SUCCESS: 
    EFAILED: If the number of current cache is 0
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CContApp_SaveEditField(CContApp *pMe,AEEAddrFieldID wFldID)
{
    int     nRet;
    //AECHAR *pData;
    uint16  wIdx;
    
    ASSERT(pMe != NULL);
    
    wIdx = CContApp_GetIdxbyFldID(pMe, wFldID);
    
    ERR("wIdx = %d",wIdx,0,0);
    if(CONTAPP_FLDID_NULL == wIdx)
    {
        // Add a new field
        nRet = CContApp_AddField( pMe,
                                  pMe->m_wEditCont,
                                  wFldID,
                                  (void *)pMe->m_pFldInputBuf);
    }
    else
    {
        if(ISNT_NAME_GENERIC_RING_FLD() && (pMe->m_pFldInputBuf == NULL))
        {
            nRet = CContApp_DeletePhoneFld( pMe, 
                                            pMe->m_wEditCont,
                                            wIdx);
        }
        else
        { 
        // Update the field
            nRet = CContApp_SaveField( pMe,
                                       pMe->m_wEditCont,
                                       wIdx,
                                       wFldID,
                                       (void *)pMe->m_pFldInputBuf);
        }
    }
    
    return nRet;
}// CContApp_SaveeditField



/*=============================================================================

FUNCTION:CContApp_SaveIconField

DESCRIPTION:
    保存头像字段
    
PARAMETERS:
    pMe     :
    
RETURN VALUE:
    SUCCESS: 
    EFAILED: If the number of current cache is 0
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CContApp_SaveIconField(CContApp *pMe, uint16 wIconID)
{
    int     nRet = SUCCESS;
    AECHAR  nData[AEE_MAX_FILE_NAME] = {0};
    uint16  wIdx;
    
    ASSERT(pMe != NULL);
    
    // if not select the use system setting
    if(wIconID != IDI_ICON_MENU_USESYSTEM)
    {
        WSTRCPY(nData, pMe->m_wFileList[wIconID]);
    }
    
    wIdx = CContApp_GetIdxbyFldID(pMe, AEE_ADDRFIELD_PHOTO);
    
    // 当前不存在该字段
    if(CONTAPP_FLDID_NULL == wIdx)
    {
        // 选择默认
        if(wIconID == IDI_ICON_MENU_USESYSTEM)
        {
            return nRet;
        }
        
        // Add a new field
        nRet = CContApp_AddField( pMe,
                                  pMe->m_wEditCont,
                                  AEE_ADDRFIELD_PHOTO,
                                  nData);
    }
    else
    {
        // 选择了默认，则删除字段
        if(wIconID == IDI_ICON_MENU_USESYSTEM)
        {
            nRet = CContApp_DeletePhoneFld( pMe, 
                                            pMe->m_wEditCont,
                                            wIdx);
        }
        else
        {
            // Update the field
            nRet = CContApp_SaveField( pMe,
                                       pMe->m_wEditCont,
                                       wIdx,
                                       AEE_ADDRFIELD_PHOTO,
                                       (void *)nData);
        }
    }
    
    return nRet;
}// CContApp_SaveIconField

/*=============================================================================

FUNCTION:CContApp_GetIdxbyFldID

DESCRIPTION:
    获取指定类型第一个字段的在Vector中的位置
    
PARAMETERS:
    pMe     :
    wFldID  : 选择字段的field ID
    
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
uint16 CContApp_GetIdxbyFldID(CContApp *pMe, AEEAddrFieldID wFldID)
{
    uint32        i;
    uint32        currNum;
    AEEAddrField *pFld;
    
    ASSERT(pMe != NULL);
    
    FARF(ADDR, ("Get Field index by field ID %d", wFldID));
    
    currNum = IVector_Size(pMe->m_pFldIv);
    
    // free the data first
    for (i = 0; i < currNum; i++)
    {
        pFld = (AEEAddrField*)IVector_ElementAt(pMe->m_pFldIv, (uint32)i);
        
        if ( pFld == NULL ) 
        {
            continue;
        }
        
        if(pFld->fID == wFldID)
        {
            return ((uint16)i);
        }
    }
    
    return CONTAPP_FLDID_NULL;
}// CContApp_GetIdxbyFldID

/*=============================================================================

FUNCTION:CContApp_GetIconImage

DESCRIPTION:
    载入头像图片用于预览
    
PARAMETERS:
    pMe     :
    
RETURN VALUE:
    Image pointer
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
IImage * CContApp_GetIconImage(CContApp *pMe, uint16 wIconID)
{
    char szName[AEE_MAX_FILE_NAME] = {0};
    char szPhoto[AEE_MAX_FILE_NAME] = {0};
    int  len;
    
    ASSERT(pMe != NULL);
    
    if(wIconID > pMe->m_nFileNum)
    {
        return NULL;
    }
    
    len = WSTRLEN(pMe->m_wFileList[wIconID]);
    
    if(len == 0)
    {
        return NULL;
    }
    
    // Generate the filename
    STRCPY(szPhoto, MP_CONTPIC_DIR);
    STRCAT(szPhoto, DIRECTORY_STR);
    (void)WSTRTOUTF8( (AECHAR *) pMe->m_wFileList[wIconID],
                      (int)      len, 
                      (byte *)   szName, 
                      (int)      sizeof(szName));;
    STRCAT(szPhoto, szName);
    
    return ISHELL_LoadImage(pMe->m_pShell, szPhoto);
}// CContApp_GetIconImage

/*=============================================================================

FUNCTION:CContApp_DrawIconImage

DESCRIPTION:
    在屏幕上画出图像
    
PARAMETERS:
    pMe     :
    
RETURN VALUE:
    Image pointer
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CContApp_DrawIconImage(CContApp *pMe, uint16 wIconID, AEERect *prc)
{
    IImage        *pImage;
    AEEImageInfo   sInfo;
    uint16         xPos = prc->x,yPos = prc->y;
    uint16         xOffSet = 0,yOffSet = 0;
    
    // Get image
    pImage = CContApp_GetIconImage(pMe, wIconID);
    
    // Erease the rectangle
    IDISPLAY_EraseRect(pMe->m_pDisplay, prc);
    
    if(pImage)
    {
        IIMAGE_GetInfo(pImage, &sInfo);
        
        // Caculate the Draw erea
        if((uint16)prc->dx > sInfo.cx)
        {
            xPos += (((uint16)prc->dx - sInfo.cx)/2);
        }
        else
        {
            xOffSet += ((sInfo.cx - (uint16)prc->dx)/2);
        }
        
        if((uint16)prc->dy > sInfo.cy)
        {
            yPos += (((uint16)prc->dy - sInfo.cy)/2);
        }
        else
        {
            yOffSet += ((sInfo.cy - (uint16)prc->dy)/2);
        }
        
        // Set draw size
        IIMAGE_SetDrawSize(pImage, (int)prc->dx, (int)prc->dy);
        
        // Set offset
        IIMAGE_SetOffset(pImage, (int)xOffSet, (int)yOffSet);
        
        // Draw image
        IIMAGE_Draw(pImage, (int)xPos, (int)yPos);
        
        // Release Image
        (void)IIMAGE_Release(pImage);
        pImage = NULL;
    }
    
    // Draw frame
    IDISPLAY_FrameRect(pMe->m_pDisplay, prc);
    IDISPLAY_Update(pMe->m_pDisplay);
    return SUCCESS;
}// CContApp_DrawIconImage

/*==================================================================

Local Method - This function is used to calculate date of a julian day

==================================================================*/
boolean CContAppJDayToDate(long jday, int *pYear, int *pMonth, int *pDay)
{
  long      lTemp;
  int       nCentury;
  int       nYear;
  int       nMonth;
  int       nDay;
  int       nDayOfYear;    // (1 <= nDayOfYear <= 366

  /*lint -e574*/
  if(jday < 0 || jday >= ((uint32)0xffffffff - (719465 * 4)) / 4)
      return(FALSE);

   /*lint -e539*/
   lTemp = (jday + 719465) * 4 - 1;
   nCentury = lTemp / 146097;

  // Calculate the year and day of year
   lTemp = ((lTemp % 146097) / 4) * 4 + 3;
   nYear = (nCentury * 100) + (lTemp / 1461);
   
   if(nYear > 9999)
      return(FALSE);

   nDayOfYear = (lTemp % 1461) / 4 + 1;
  
  // Calculate the month and day of month
   lTemp = nDayOfYear * 5 - 3;                   // shift to Mar based years
   nMonth = lTemp / 153;
   nDay = (lTemp % 153) / 5 + 1;
  
  // Convert back to Jan based years
   if (nMonth < 10)
      nMonth += 3;
   else {
      nMonth -= 9;
      nYear++;
   }
   *pYear = nYear;
   *pMonth = nMonth;
   *pDay = nDay;

   return(TRUE);
}


#ifdef FEATURE_RUIM_PHONEBOOK
/*=============================================================================

FUNCTION: CContApp_CheckRUIMRec

DESCRIPTION: 
    检查当前RUIM记录是否存在与输入的新记录重复
    
PARAMETERS:
    pMe:
    pName:
    pNumber:
    
RETURN VALUE:
    boolean: TRUE if exist ,otherwise FALSE.
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
boolean CContApp_CheckRUIMRec( CContApp *pMe, 
                                      AECHAR   *pName, 
                                      AECHAR   *pNumber)
{
    IAddrRec            *pAddrRec;
    AEEAddrField        *pAddrFld;
    
    ASSERT(pMe != NULL);
    
    if ( pName == NULL || pNumber == NULL ) 
    {
        return TRUE;
    }
    
    if (SUCCESS != IADDRBOOK_EnumRecInit( pMe->m_pAddrRUIM,
                                          AEE_ADDR_CAT_NONE,
                                          AEE_ADDRFIELD_NAME,
                                          pName,
                                          (uint16)WSTRSIZE(pName)))
    {
        return FALSE;
    }
    
    pAddrRec = IADDRBOOK_EnumNextRec(pMe->m_pAddrRUIM);
    
    for (;;)
    {
        if (pAddrRec == NULL) 
        {
            break;
        }
        
        //检查号码是否相同
        pAddrFld = IADDRREC_GetField( pAddrRec, IDX_NUM_RUIM_FLD );
        
        if (WSTRCMP((AECHAR *)pAddrFld->pBuffer, pNumber) == 0)
        {
            IADDRREC_Release(pAddrRec);
            return TRUE;
        }
        
        IADDRREC_Release(pAddrRec);
        pAddrRec = NULL;
        pAddrRec = IADDRBOOK_EnumNextRec(pMe->m_pAddrRUIM);
    }//end for
    
    return FALSE;
} // CContApp_CheckRUIMRec
#endif// FEATURE_RUIM_PHONEBOOK

/*=============================================================================

FUNCTION:CContApp_DeleteItem

DESCRIPTION:
    向列表菜单中添加话机项目
    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    pCache  : 指向添加内容的指针
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CContApp_DeleteItem(CContApp        *pMe, 
                        IMenuCtl        *pMenuCtl,
                        AEEAddCacheInfo *pCache)
{
    uint16     wRecID, rtn = TYPE_NO_MATCH;
    CtlAddItem ai;
    sSelectFieldInfo  curSelectFieldInfo;  
    
    ASSERT(pMe != NULL); 
    
    if (!pMenuCtl || !pCache)
    {
        return EBADPARM;
    }
    
    wRecID = CContApp_RawToContID(pMe, pCache->wRecID, (pCache->store == ADDR_STORE_RUIM));
    if (wRecID == CONTAPP_RECID_NULL)
    {
        FARF(ADDR, ("Phone Record id Out of range %d", wRecID));
        return EFAILED;
    }
    
    FARF(ADDR, ("Add phone item %d cat %d", wRecID, 0));
    
    if (WSTRLEN(pCache->szName) == 0)
    {
        FARF(ADDR, ("Failed to Add No name item %d", wRecID));
        return EFAILED;
    }

    MEMSET(&curSelectFieldInfo, 0, sizeof(curSelectFieldInfo));       
    MEMSET(&ai, 0, sizeof(ai));
    ai.pText = pCache->szName;

    ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
    if(IS_RUIM_REC(wRecID))
    {
        ai.wImage = IDB_RUIM;
    }
    else
    {
        ai.wImage = IDB_PHONE;
    }
    ai.wItemID = wRecID;
    if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
        return EFAILED;
    }

    curSelectFieldInfo.wIndex = wRecID;
    //curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;

    rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);

    if (TYPE_INDEX_MATCH == rtn)
    {
        //ai.wImage = IDB_SELECT_PHONE;
        //SetCheckBoxItem(pMenuCtl, wRecID, TRUE);
        //IMENUCTL_SetSelEx(pMenuCtl, wRecID, TRUE);
        IMENUCTL_SetLastItemSel(pMenuCtl, TRUE);
    }
    /*else
    {
        //ai.wImage = IDB_NOSELSECT_PHONE;
        SetCheckBoxItem(pMenuCtl, wRecID, FALSE);
    }
*/
/*
    curSelectFieldInfo.wIndex = wRecID;

    rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);

    ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
    if (TYPE_INDEX_MATCH == rtn)
    {
        ai.wImage = (pCache->store == ADDR_STORE_RUIM ? IDB_SELECT_UIM : IDB_SELECT_PHONE);
    }
    else
    {
        ai.wImage = (pCache->store == ADDR_STORE_RUIM ? IDB_NOSELECT_UIM : IDB_NOSELSECT_PHONE); 
    }

    ai.wItemID = wRecID;
    if (FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
        return EFAILED;
    }
*/
    return SUCCESS;
}// CContApp_DeleteItem


/*=============================================================================

FUNCTION:CContApp_CopyItem

DESCRIPTION:
    向列表菜单中添加话机项目
    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    pCache  : 指向添加内容的指针
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CContApp_CopyItem(CContApp  *pMe, 
                      IMenuCtl  *pMenuCtl,
                      AEEAddCacheInfo *pCache)
{
    uint16     wRecID, rtn = TYPE_NO_MATCH;
    CtlAddItem ai;
    sSelectFieldInfo  curSelectFieldInfo;  
    
    ASSERT(pMe != NULL); 
    
    if (!pMenuCtl || !pCache)
    {
        return EBADPARM;
    }
    
    wRecID = CContApp_RawToContID(pMe, pCache->wRecID, (pCache->store == ADDR_STORE_RUIM));
    
    if (wRecID == CONTAPP_RECID_NULL)
    {
        FARF(ADDR, ("Phone Record id Out of range %d", wRecID));
        return EFAILED;
    }
    
    if (WSTRLEN(pCache->szName) == 0)
    {
        FARF(ADDR, ("Failed to Add No name item %d", wRecID));
        return EFAILED;
    }
    
    FARF(ADDR, ("Add phone item %d cat %d", wRecID, 0));
    
    MEMSET(&curSelectFieldInfo, 0, sizeof(curSelectFieldInfo));       
    MEMSET(&ai, 0, sizeof(ai));
    ai.pText = pCache->szName;

    ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
    if(IS_RUIM_REC(wRecID))
    {
        ai.wImage = IDB_RUIM;
    }
    else
    {
        ai.wImage = IDB_PHONE;
    }
    ai.wItemID = wRecID;
    if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
        return EFAILED;
    }
    
    curSelectFieldInfo.wIndex = wRecID;
    //curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;
    
    rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
    
    if (TYPE_INDEX_MATCH == rtn)
    {
        //ai.wImage = IDB_SELECT_PHONE;
        //SetCheckBoxItem(pMenuCtl, wRecID, TRUE);
        //IMENUCTL_SetSelEx(pMenuCtl, wRecID, TRUE);
        IMENUCTL_SetLastItemSel(pMenuCtl, TRUE);
    }
   /* else
    {
        //ai.wImage = IDB_NOSELSECT_PHONE;
        SetCheckBoxItem(pMenuCtl, wRecID, FALSE);
    }
*/
    /*
    curSelectFieldInfo.wIndex = wRecID;

    rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);

    ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
    if (TYPE_INDEX_MATCH == rtn)
    {
        ai.wImage = (pCache->store == ADDR_STORE_RUIM ? IDB_SELECT_UIM : IDB_SELECT_PHONE);
    }
    else
    {
        ai.wImage = (pCache->store == ADDR_STORE_RUIM ? IDB_NOSELECT_UIM : IDB_NOSELSECT_PHONE);    
    }

    ai.wItemID = wRecID;
    if (FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
        return EFAILED;
    }*/
    
    return SUCCESS;
}// CContApp_CopyItem


/*=============================================================================

FUNCTION:CContApp_SelectItem

DESCRIPTION:
    向列表菜单中添加话机项目
    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    pCache  : 指向添加内容的指针
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CContApp_SelectItem(CContApp        *pMe, 
                        IMenuCtl        *pMenuCtl,
                        AEEAddCacheInfo *pCache)
{
    uint16      wRecID, wTempRecID, rtn = TYPE_NO_MATCH;
    CtlAddItem  ai;
    sSelectFieldInfo curSelectFieldInfo;
    
    ASSERT(pMe != NULL); 
    if (!pMenuCtl || !pCache)
    {
        return EBADPARM;
    }
        
    wRecID = CContApp_RawToContID(pMe, pCache->wRecID, (pCache->store == ADDR_STORE_RUIM));
    
    if (wRecID == CONTAPP_RECID_NULL)
    {
        FARF(ADDR, ("Phone Record id Out of range %d", wRecID));
        return EFAILED;
    }
    
    if (WSTRLEN(pCache->szName) == 0)
    {
        FARF(ADDR, ("Failed to Add No name item %d", wRecID));
        return EFAILED;
    }
    
    FARF(ADDR, ("Add phone item %d cat %d", wRecID, 0));
    
    MEMSET(&curSelectFieldInfo, 0, sizeof(curSelectFieldInfo));       
    MEMSET(&ai, 0, sizeof(ai));
    ai.pText = pCache->szName;
    ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;
    wTempRecID = wRecID;
    if(IS_RUIM_REC(wRecID))
    {
        ai.wImage = IDB_RUIM;
    }
    else
    {
        ai.wImage = IDB_PHONE;
    }
    ai.wItemID = wRecID;
    if(FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
        return EFAILED;
    }

    /*
    if (wTempRecID == pMe->m_wSelectCont)
    {
        curSelectFieldInfo.wIndex = wRecID;
        curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;

        rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);

        if (TYPE_FIELD_MATCH == rtn)
        {
            //ai.wImage = IDB_SELECT_PHONE;
            SetCheckBoxItem(pMenuCtl, wRecID, TRUE);
        }
        else
        {
            //ai.wImage = IDB_NOSELSECT_PHONE;  
            SetCheckBoxItem(pMenuCtl, wRecID, FALSE);
        }
    }
    else
    {*/
        //ai.wImage  = IDB_NOSELSECT_PHONE;  
        curSelectFieldInfo.wIndex = wRecID;
        //curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;

        rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);

        if (TYPE_INDEX_MATCH == rtn)
        {
            //ai.wImage = IDB_SELECT_PHONE;
            //SetCheckBoxItem(pMenuCtl, wRecID, TRUE);
            IMENUCTL_SetLastItemSel(pMenuCtl, TRUE);
        }
        /*else
        {
            //ai.wImage = IDB_NOSELSECT_PHONE;
            SetCheckBoxItem(pMenuCtl, wRecID, FALSE);
        }*/
    //}
    /*
    if (wTempRecID == pMe->m_wSelectCont)
    {
        curSelectFieldInfo.wIndex = wRecID;
        curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;
    }
    else
    {
        curSelectFieldInfo.wIndex = wRecID;
    }
    
    rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);

    if ((TYPE_FIELD_MATCH == rtn) || (TYPE_INDEX_MATCH == rtn))
    {
        ai.wImage = (pCache->store == ADDR_STORE_RUIM ? IDB_SELECT_UIM : IDB_SELECT_PHONE);
    }
    else
    {
        ai.wImage = (pCache->store == ADDR_STORE_RUIM ? IDB_NOSELECT_UIM : IDB_NOSELSECT_PHONE);
    }

    ai.wItemID = wRecID;
    if (FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
        return EFAILED;
    }
    */
    return SUCCESS;
}// CContApp_SelectItem

/*=============================================================================

FUNCTION:CContApp_SmartPhoneItem

DESCRIPTION:
    向列表菜单中添加话机项目
    
PARAMETERS:
    pMe     :
    pMenuCtl: 指向Menu Control的指针
    pCache  : 指向添加内容的指针
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
int CContApp_SmartItems(CContApp *pMe, IMenuCtl *pMenuCtl, AEEAddCacheInfo *pCache)
{
    AECHAR              *pName;
    uint16              wRecID, wTempRecID, rtn = TYPE_NO_MATCH;
    CtlAddItem          ai;
    sSelectFieldInfo    curSelectFieldInfo;  
    
    if (!pMenuCtl || !pCache || !pMe)
    {
        return EBADPARM;
    }
    
    pName = pCache->szName;
    wRecID = CContApp_RawToContID(pMe, pCache->wRecID, (pCache->store == ADDR_STORE_RUIM));
    
    if (wRecID == CONTAPP_RECID_NULL)
    {
        FARF(ADDR, ("Phone Record id Out of range %d", wRecID));
        return EFAILED;
    }
    
    FARF(ADDR, ("Add phone item %d cat %d", wRecID, pCache->nCat));
    
    MEMSET(&curSelectFieldInfo, 0, sizeof(curSelectFieldInfo));       
    MEMSET(&ai, 0, sizeof(ai));
    ai.pText = pName;
    ai.pszResImage = AEE_APPSCOMMONRES_IMAGESFILE;    
    wTempRecID = wRecID;
    ai.wItemID = wRecID;

    if (pCache->store == ADDR_STORE_RUIM)
    {
        ai.wImage = IDB_RUIM;
    }
    else
    {
        ai.wImage = IDB_PHONE;
    }
    
    if (FALSE == IMENUCTL_AddItemEx(pMenuCtl, &ai))
    {
        return EFAILED;
    }
    
    switch (pMe->m_nSmartStateType)
    {
        case SMART_STATE_IDD_LIST:
            break;
            
        case SMART_STATE_IDD_SELECT:
        case SMART_STATE_IDD_COPY:
        case SMART_STATE_IDD_DELETE:
        case SMART_STATE_IDD_SELECT_RECORD:
            {
                curSelectFieldInfo.wIndex = wRecID;
                
                rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
                if (TYPE_INDEX_MATCH == rtn)
                {
                    //ai.wImage  = IDB_SELECT_PHONE; 
                    //SetCheckBoxItem(pMenuCtl, wRecID, TRUE);
                    IMENUCTL_SetLastItemSel(pMenuCtl, TRUE);
                }
                /*else
                {
                    //ai.wImage  = IDB_NOSELSECT_PHONE;  
                    SetCheckBoxItem(pMenuCtl, wRecID, FALSE);
                }        */    
                break;       
            }
            /*
            if ((wTempRecID == pMe->m_wSelectSmart) &&
                (pMe->m_nSmartStateType != SMART_STATE_IDD_COPY) &&
                (pMe->m_nSmartStateType != SMART_STATE_IDD_DELETE))
            {
                curSelectFieldInfo.wIndex = wRecID;
                curSelectFieldInfo.wFieldMask = pMe->m_wFieldIndex;    
                rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);                            
            }
            else
            {
                curSelectFieldInfo.wIndex = wRecID;
                rtn = CContApp_FindSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
            }
            if (pCache->store == ADDR_STORE_RUIM)
            {
                if (TYPE_FIELD_MATCH == rtn)
                {
                    ai.wImage  = IDB_SELECT_UIM;  
                }
                else
                {
                    ai.wImage  = IDB_NOSELECT_UIM;  
                }
            }
            else
            {
                if (TYPE_FIELD_MATCH == rtn)
                {
                    ai.wImage  = IDB_SELECT_PHONE;  
                }
                else
                {
                    ai.wImage  = IDB_NOSELSECT_PHONE;  
                }
            }
            break;
*/
        default:
            return EFAILED;
    }     

    return SUCCESS;
}


#ifdef FEATURE_RUIM_PHONEBOOK
/*=============================================================================

FUNCTION:CContApp_CopyToRUIM

DESCRIPTION: 
    将当前话机的记录全部复制到 RUIM 卡

PARAMETERS:
    pMe    :
    wStartID: the Cont ID of Start copy
    
RETURN VALUE:
    SUCCESS   : 
    EMAXRECORD: if the target device is full
    ENORECORD : if no record to copy
    
COMMENTS:
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_CopyToRUIM(CContApp *pMe, uint16 wContID)
{
    uint16        j;
    uint16        wFldNum;
    IAddrRec     *pAddrRec = NULL;
    AEEAddrField *pNameFld = NULL;
    AEEAddrField *pNumFld = NULL;
    
    ASSERT(pMe != NULL);
    // Check the capcity
    if(IS_MAX_REC(pMe->m_pAddrRUIM))
    {
        return EMAXRECORD;
    }
    pAddrRec = IADDRBOOK_GetRecByID( pMe->m_pAddrPhone, wContID);
    if (pAddrRec == NULL)
    {
        FARF(ADDR, ("Fail to get rec %d", wContID));
        return EFAILED;
    }
    else
    {
     //get total fields
        wFldNum  = (uint16)IADDRREC_GetFieldCount(pAddrRec);
            
        if(wFldNum == 0)
        {
             (void)IADDRREC_Release(pAddrRec);
             pAddrRec = NULL;
             return EFAILED;
         }
            
      // Get name field
         pNameFld = IADDRREC_GetField(pAddrRec, IDX_NAME_FLD);

         if(wFldNum > 1)
         {
             for (j = 1; j < wFldNum; j++)
             {
          //Get number field
                pNumFld = IADDRREC_GetField(pAddrRec, j);
                if (IS_NUM_FIELD(pNumFld->fID))
                {
                // Copy to RUIM card
                    if(SUCCESS == CContApp_Copy2RUIM(pMe, pNameFld, pNumFld))
                    {
                            // 增加记录计数
                          pMe->m_wCopyRecCnt++;
                    }
                }
            }//end for(j=0;;)
         }
         else
         {
             if(SUCCESS == CContApp_Copy2RUIM(pMe, pNameFld, NULL))
             {
                     // 增加记录计数
                   pMe->m_wCopyRecCnt++;
             }
         }
    }//end if(addrRec==NULL)
        
    IADDRREC_Release(pAddrRec);
    pAddrRec = NULL; 
    
    if (SINGLEMOVE == pMe->m_nCopyMoveType)
    {
        CContApp_DeleteCont(pMe, wContID);
    }
//    (void)CContApp_LoadRUIMCont( pMe, pMe->m_wFindFldID, pMe->m_pFindData);
    return SUCCESS;
    
}// CContApp_CopyToRUIM
#endif// FEATURE_RUIM_PHONEBOOK

/*=============================================================================

FUNCTION:CContApp_CopyToPhone

DESCRIPTION: 
    将当前RUIM 的记录全部复制到 话机

PARAMETERS:
    pMe    :
    wStartID: the Cont ID of Start copy
    
RETURN VALUE:
    SUCCESS   : 
    EMAXRECORD: if the target device is full
    ENORECORD : if no record to copy
    
COMMENTS:
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_CopyToPhone(CContApp *pMe, uint16 wContID)
{
    uint16        j;
    uint16        wFldNum;
    IAddrRec     *pAddrRec = NULL;
    AEEAddrField *pNameFld = NULL;
    AEEAddrField *pNumFld = NULL;
    
    ASSERT(pMe != NULL);
    // Check the capcity
    if(IS_MAX_REC(pMe->m_pAddrPhone))
    {
        return EMAXRECORD;
    }
    pAddrRec = IADDRBOOK_GetRecByID( pMe->m_pAddrRUIM, wContID);
    if (pAddrRec == NULL)
    {
        FARF(ADDR, ("Fail to get rec %d", wContID));
        return EFAILED;
    }
    else
    {
     //get total fields
        wFldNum  = (uint16)IADDRREC_GetFieldCount(pAddrRec);
            
        if(wFldNum == 0)
        {
             (void)IADDRREC_Release(pAddrRec);
             pAddrRec = NULL;
             return EFAILED;
         }
            
      // Get name field
         pNameFld = IADDRREC_GetField(pAddrRec, IDX_NAME_FLD);

         if(wFldNum > 1)
         {
             for (j = 1; j < wFldNum; j++)
             {
          //Get number field
                pNumFld = IADDRREC_GetField(pAddrRec, j);
                if (IS_NUM_FIELD(pNumFld->fID))
                {
                // Copy to RUIM card
                    if(SUCCESS == CContApp_Merge2Phone(pMe, pNameFld, pNumFld))
                    {
                            // 增加记录计数
                            pMe->m_wCopyRecCnt++;
                    }
                }
              }//end for(j=0;;)
         }
         else
         {
            // Copy to RUIM card
            if(SUCCESS == CContApp_Merge2Phone(pMe, pNameFld, NULL))
            {
                    // 增加记录计数
                    pMe->m_wCopyRecCnt++;
            }
         }
    }//end if(addrRec==NULL)
        
    IADDRREC_Release(pAddrRec);
    pAddrRec = NULL; 
    
    if (SINGLEMOVE == pMe->m_nCopyMoveType)
    {
        CContApp_DeleteCont(pMe, wContID + CONTAPP_RUIM_START_IDX -1);
    }
    return SUCCESS;
}// CContApp_CopyToRUIM

boolean CContApp_GetRecByID(CContApp *pMe, uint16 wContID)
{
    uint16        j;
    uint16        wFldNum;
    IAddrRec     *pAddrRec = NULL;
    AEEAddrField *pNameFld = NULL;
    AEEAddrField *pNumFld = NULL;
    
    ASSERT(pMe != NULL);

    CContApp_FreeBuffer(pMe);


    if(IS_RUIM_REC(wContID))
    {
#ifdef FEATURE_RUIM_PHONEBOOK
        pAddrRec = IADDRBOOK_GetRecByID( pMe->m_pAddrRUIM, wContID -CONTAPP_RUIM_START_IDX +1);
#endif
        if (pAddrRec == NULL)
        {
            FARF(ADDR, ("Fail to get rec %d", wContID));
            return FALSE;
        }
        else
        {
         //get total fields
            wFldNum  = (uint16)IADDRREC_GetFieldCount(pAddrRec);
            pMe->m_wFieldCount = wFldNum;

                
            if(wFldNum == 0)
            {
                 (void)IADDRREC_Release(pAddrRec);
                 pAddrRec = NULL;
                  return FALSE;
             }
          // Get name field
             pNameFld = IADDRREC_GetField(pAddrRec, IDX_NAME_FLD);
          
             //FREEIF(pMe->m_pAddNewName);
             pMe->m_pAddNewName = WSTRDUP((AECHAR *)pNameFld->pBuffer);
             pMe->m_wFieldMask = MASK_ADDRFIELD_NAME;
                
             for (j = 1; j < wFldNum; j++)
             {
               //Get number field
                pNumFld = IADDRREC_GetField(pAddrRec, j);  

				CContApp_SetPlusNumber((AECHAR *)pNumFld->pBuffer);	//Add By zzg 2011_12_15				
          
                // Get mobile number
                //FREEIF(pMe->m_pAddNewMobile);
                pMe->m_pAddNewMobile = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                pMe->m_wFieldMask |= MASK_ADDRFIELD_PHONE_GENERIC;     
                
              }            
             //end for(j=0;;)
         }//end if(addrRec==NULL)
          
    IADDRREC_Release(pAddrRec);
    pAddrRec = NULL; 
    
    }
    else
    {
        pAddrRec = IADDRBOOK_GetRecByID( pMe->m_pAddrPhone, wContID);
        if (pAddrRec == NULL)
        {
            FARF(ADDR, ("Fail to get rec %d", wContID));
            return FALSE;
        }
        else
        {
         //get total fields
            wFldNum  = (uint16)IADDRREC_GetFieldCount(pAddrRec);
            pMe->m_wFieldCount = wFldNum;

                
            if(wFldNum == 0)
            {
                 (void)IADDRREC_Release(pAddrRec);
                 pAddrRec = NULL;
                 return FALSE;
             }
                
          // Get name field
             pNameFld = IADDRREC_GetField(pAddrRec, IDX_NAME_FLD);

             //FREEIF(pMe->m_pAddNewName);
             pMe->m_pAddNewName = WSTRDUP((AECHAR *)pNameFld->pBuffer);
             pMe->m_wFieldMask = MASK_ADDRFIELD_NAME;

            //Get category field
            pMe->m_nGroupCat = CContApp_GetCat(pMe, wContID);             
                
             for (j = 1; j < wFldNum; j++)
             {
                //Get number field
                pNumFld = IADDRREC_GetField(pAddrRec, j);
                switch (pNumFld->fID)
                {
                    case AEE_ADDRFIELD_PHONE_GENERIC:
                        // Get mobile number
                        //FREEIF(pMe->m_pAddNewMobile);						

						CContApp_SetPlusNumber((AECHAR *)pNumFld->pBuffer);	//Add By zzg 2011_12_15			
						
                        pMe->m_pAddNewMobile = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        pMe->m_wFieldMask |= MASK_ADDRFIELD_PHONE_GENERIC;      

                        break;
                        
                    case AEE_ADDRFIELD_PHONE_HOME:
                        //FREEIF(pMe->m_pAddNewHome);
                        pMe->m_pAddNewHome = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        pMe->m_wFieldMask |= MASK_ADDRFIELD_PHONE_HOME;                        
                        break;
                        
                    case AEE_ADDRFIELD_PHONE_WORK:
                        //FREEIF(pMe->m_pAddNewOffice);
                        pMe->m_pAddNewOffice = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        pMe->m_wFieldMask |= MASK_ADDRFIELD_PHONE_WORK;                        
                        break;
                        
                    case AEE_ADDRFIELD_PHONE_FAX:
                        //FREEIF(pMe->m_pAddNewFax);
                        pMe->m_pAddNewFax = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        pMe->m_wFieldMask |= MASK_ADDRFIELD_PHONE_FAX;                        
                        break;
                        
                    case AEE_ADDRFIELD_EMAIL:
                        //FREEIF(pMe->m_pAddNewEMAIL);
                        pMe->m_pAddNewEMAIL = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        pMe->m_wFieldMask |= MASK_ADDRFIELD_EMAIL;   
                        
                        if ((pMe->m_wSelFldType == MULTI_SELECT_NUMBER) ||(pMe->m_wSelFldType == SINGLE_SELECT_NUMBER))
                        {
                            pMe->m_wFieldMask &= (~MASK_ADDRFIELD_EMAIL);   
                            --pMe->m_wFieldCount;
                        }
                       break;
                        
                    case AEE_ADDRFIELD_ADDRESS:
                        //FREEIF(pMe->m_pAddNewAddress);
                        pMe->m_pAddNewAddress = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        pMe->m_wFieldMask |= MASK_ADDRFIELD_ADDRESS;
                        
                        if ((pMe->m_wSelFldType == MULTI_SELECT_NUMBER) ||(pMe->m_wSelFldType == SINGLE_SELECT_NUMBER))
                        {
                            pMe->m_wFieldMask &= (~MASK_ADDRFIELD_ADDRESS);
                            --pMe->m_wFieldCount;
                        }
                        break;

                    case AEE_ADDRFIELD_URL:
                        //FREEIF(pMe->m_pAddNewURL);
                        pMe->m_pAddNewURL = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        pMe->m_wFieldMask |= MASK_ADDRFIELD_URL;
                        
                        if ((pMe->m_wSelFldType == MULTI_SELECT_NUMBER) ||(pMe->m_wSelFldType == SINGLE_SELECT_NUMBER))
                        {
                            pMe->m_wFieldMask &= (~MASK_ADDRFIELD_URL);
                            --pMe->m_wFieldCount;
                        }
                        break;
                        
                    case AEE_ADDRFIELD_NOTES:
                        //FREEIF(pMe->m_pAddNewRemark);
                        pMe->m_pAddNewRemark = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        pMe->m_wFieldMask |= MASK_ADDRFIELD_NOTES;
                        
                        if ((pMe->m_wSelFldType == MULTI_SELECT_NUMBER) ||(pMe->m_wSelFldType == SINGLE_SELECT_NUMBER))
                        {
                            pMe->m_wFieldMask &= (~MASK_ADDRFIELD_NOTES);
                            --pMe->m_wFieldCount;
                        }                        
                        break;
                        
                    case AEE_ADDRFIELD_RINGTONE:
                        pMe->m_nRingToneID = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        if ((pMe->m_wSelFldType == MULTI_SELECT_NUMBER) ||(pMe->m_wSelFldType == SINGLE_SELECT_NUMBER))
                        {
                            --pMe->m_wFieldCount;
                        }                        
                        break;
                        
                    default:
                        break;
                }
              }//end for(j=0;;)
            }//end if(addrRec==NULL)
            
            IADDRREC_Release(pAddrRec);
            pAddrRec = NULL; 
        }
    return TRUE;
}

void CContApp_SetRecByFLDID(CContApp *pMe, uint16 wFLDID)
{
        switch(wFLDID)
        {
            case AEE_ADDRFIELD_NAME:
                 FREEIF(pMe->m_pAddNewName);
                 pMe->m_pAddNewName = pMe->m_pFldInputBuf;
                 break;
                 
            case AEE_ADDRFIELD_PHONE_GENERIC:
                 FREEIF(pMe->m_pAddNewMobile);
                 pMe->m_pAddNewMobile = pMe->m_pFldInputBuf;

				 
                 break;
                 
            case AEE_ADDRFIELD_PHONE_HOME:
                 FREEIF(pMe->m_pAddNewHome);
                 pMe->m_pAddNewHome = pMe->m_pFldInputBuf;
                 break;
                 
            case AEE_ADDRFIELD_PHONE_WORK:
                 FREEIF(pMe->m_pAddNewOffice);                    
                 pMe->m_pAddNewOffice = pMe->m_pFldInputBuf;
                 break;

            case AEE_ADDRFIELD_PHONE_FAX:
                 FREEIF(pMe->m_pAddNewFax);                    
                 pMe->m_pAddNewFax = pMe->m_pFldInputBuf;
                 break;
                 
            case AEE_ADDRFIELD_EMAIL:
                 FREEIF(pMe->m_pAddNewEMAIL);                    
                 pMe->m_pAddNewEMAIL = pMe->m_pFldInputBuf;
                 break;
                 
            case AEE_ADDRFIELD_ADDRESS:
                 FREEIF(pMe->m_pAddNewAddress);                    
                 pMe->m_pAddNewAddress = pMe->m_pFldInputBuf;
                 break;
                 
            case AEE_ADDRFIELD_URL:
                 FREEIF(pMe->m_pAddNewURL);                    
                 pMe->m_pAddNewURL = pMe->m_pFldInputBuf;
                 break;
                 
            case AEE_ADDRFIELD_NOTES:
                 FREEIF(pMe->m_pAddNewRemark);                    
                 pMe->m_pAddNewRemark = pMe->m_pFldInputBuf;
                 break;
                 
            default:
                break;
        }
        pMe->m_pFldInputBuf = NULL;
}

/*=============================================================================

FUNCTION: CContApp_CreateCont

DESCRIPTION: 
    添加新记录
    
PARAMETERS:
    pMe    :
    pName  :
    pNumber:
    bCard  :
    
RETURN VALUE:
    int: SUCCESS if add ,otherwise EFAILED.
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
    在调用此函数之前应该检查是否重名（话机）或记录重复（RUIM）
    
=============================================================================*/
int CContApp_CreateCont( CContApp *pMe,  boolean bCard)
{
    IAddrRec       *pAddrRec = NULL;
    AEEAddrField    addrFld[11];
    int index = 0;
    
    ASSERT(pMe != NULL);
    
    // Set the name field
    if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_NAME,
                                           pMe->m_pAddNewName,
                                           &addrFld[0],
                                           FALSE))
    {
        return EFAILED;
    }
    
    // Set the number field
    if (NULL !=pMe->m_pAddNewMobile)
    {    	

    	if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_PHONE_GENERIC,
                                               pMe->m_pAddNewMobile,
                                               &addrFld[1],
                                               FALSE))
        {
            return EFAILED;
        }
		
        index = 1;
    }
    else
    {
           index = 0;
    }
    if(!bCard)
    {
         // Set the home number field
        if (NULL !=pMe->m_pAddNewHome)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_PHONE_HOME,
                                                   pMe->m_pAddNewHome,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }

         // Set the office number field
        if (NULL !=pMe->m_pAddNewOffice)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_PHONE_WORK,
                                                   pMe->m_pAddNewOffice,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }

         // Set the Fax number field
        if (NULL !=pMe->m_pAddNewFax)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_PHONE_FAX,
                                                   pMe->m_pAddNewFax,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }

         // Set the EMAIL field
        if (NULL !=pMe->m_pAddNewEMAIL)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_EMAIL,
                                                   pMe->m_pAddNewEMAIL,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }

         // Set the Address field
        if (NULL !=pMe->m_pAddNewAddress)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_ADDRESS,
                                                   pMe->m_pAddNewAddress,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }
        
         // Set the URL field
        if (NULL !=pMe->m_pAddNewURL)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_URL,
                                                   pMe->m_pAddNewURL,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }

         // Set the Remark field
        if (NULL !=pMe->m_pAddNewRemark)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_NOTES,
                                                   pMe->m_pAddNewRemark,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }

         // Set the Ring Tone field
        if (NULL !=pMe->m_nRingToneID && (AECHAR)'\0' != pMe->m_nRingToneID[0])
        {
            //AECHAR  nData[2] = {0}; 
            //nData[0] = pMe->m_nRingToneID;
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_RINGTONE,
                                                   (void*)pMe->m_nRingToneID,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }

         // Set the Group field
        if (-1 !=pMe->m_nGroupCat)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_GROUP,
                                                   &pMe->m_nGroupCat,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }
    }
    
    if(!bCard)
    {
        pAddrRec = IADDRBOOK_CreateRec( pMe->m_pAddrPhone,
                                        pMe->m_nGroupCat,
                                        addrFld,
                                        /*--*/index);
    }
#ifdef FEATURE_RUIM_PHONEBOOK
    else
    {
        pAddrRec = IADDRBOOK_CreateRec( pMe->m_pAddrRUIM,
                                        AEE_ADDR_CAT_NONE,
                                        addrFld,
                                        2);
    }
#endif// FEATURE_RUIM_PHONEBOOK

    if(NULL == pAddrRec)
    {
        return EFAILED;
    }
    
    pMe->m_wEditCont = CContApp_RawToContID( pMe,
                                             IADDRREC_GetRecID(pAddrRec),
                                             bCard);
    
    IADDRREC_Release(pAddrRec);

    
    // reload all the cont info
    if(STATE_GROUPVIEW_LIST == CContApp_GetReturnState(pMe))
    {
        CContApp_GetGroupCat(pMe, pMe->m_wselGroupId);
        CContApp_LoadByCat(pMe, pMe->m_nGroupCat);
    }
    else
    {
        switch(pMe->m_nViewType)
            {
                case CONTCFG_VIEWTYPE_ALL:
                    (void)CContApp_LoadByCondition(pMe,(pMe->m_pFindData==NULL ? 0 : WSTRSIZE(pMe->m_pFindData)));
                    break;
                    
                case CONTCFG_VIEWTYPE_PHONE:
                    if(!bCard)
                    {
                        (void) CContApp_LoadSingleStoreCont(pMe,
                                                            pMe->m_wFindCat,
                                                            pMe->m_wFindFldID,
                                                            pMe->m_pFindData,
                                                            ADDR_STORE_DBFILE);
                    }
                    break;
                    
#ifdef FEATURE_RUIM_PHONEBOOK
                case CONTCFG_VIEWTYPE_RUIM:
                    if(bCard)
                    {
                        (void) CContApp_LoadSingleStoreCont(pMe,
                                                            AEE_ADDR_CAT_NONE,
                                                            pMe->m_wFindFldID,
                                                            pMe->m_pFindData,
                                                            ADDR_STORE_RUIM);
                    }
                    break;
#endif// FEATURE_RUIM_PHONEBOOK
        
                default:
                    break;
            }
    }
    
    return SUCCESS;
}// CContApp_CreateCont

boolean CContApp_DisplaySelectField( CContApp  *pMe, byte Field)
{
    AEERect  dlgrc;
    AECHAR  *DisplayString = NULL;

    //CContApp_GetRecByID(pMe, pMe->m_wSelectCont);

    switch(Field)
    {
        case MASK_ADDRFIELD_NAME:
            DisplayString = WSTRDUP((AECHAR *)pMe->m_pAddNewName);
            break;
        case MASK_ADDRFIELD_PHONE_GENERIC:
            DisplayString = WSTRDUP((AECHAR *)pMe->m_pAddNewMobile);			
            break;
        case MASK_ADDRFIELD_PHONE_HOME:
            DisplayString = WSTRDUP((AECHAR *)pMe->m_pAddNewHome);         
            break;
        case MASK_ADDRFIELD_PHONE_WORK:
            DisplayString = WSTRDUP((AECHAR *)pMe->m_pAddNewOffice);            
            break;
        case MASK_ADDRFIELD_PHONE_FAX:
            DisplayString = WSTRDUP((AECHAR *)pMe->m_pAddNewFax);            
            break;
        case MASK_ADDRFIELD_EMAIL:
            DisplayString = WSTRDUP((AECHAR *)pMe->m_pAddNewEMAIL);            
            break;
        case MASK_ADDRFIELD_ADDRESS:
            DisplayString = WSTRDUP((AECHAR *)pMe->m_pAddNewAddress);            
            break;
        case MASK_ADDRFIELD_URL:
            DisplayString = WSTRDUP((AECHAR *)pMe->m_pAddNewURL);            
            break;
        case MASK_ADDRFIELD_NOTES:
            DisplayString = WSTRDUP((AECHAR *)pMe->m_pAddNewRemark);            
            break;
        default:
            DisplayString = WSTRDUP((AECHAR *)pMe->m_pAddNewName);            
            break;
    }

    SETAEERECT(&dlgrc,
                        0,
//                        pMe->m_rc.dy -2*(BOTTOMBAR_HEIGHT +2),
                        pMe->m_rc.dy -3*pMe->m_nLineHeight-SEARCH_INPUTBOX_GAP,
                        pMe->m_rc.dx,
                        pMe->m_nLineHeight);
    IDISPLAY_EraseRect(pMe->m_pDisplay,&dlgrc);
    IDISPLAY_DrawHLine( pMe->m_pDisplay, 0 , pMe->m_rc.dy -2*pMe->m_nLineHeight-SEARCH_INPUTBOX_GAP -1, pMe->m_rc.dx);    

    (void)IDISPLAY_DrawText(pMe->m_pDisplay,
                        AEE_FONT_NORMAL, //AEE_FONT_BOLD,
                        DisplayString,
                        -1,
                        dlgrc.x,
                        dlgrc.y,
                        &dlgrc,
                        IDF_ALIGN_LEFT);

    FREEIF(DisplayString);

    // 统一更新界面
   // IDISPLAY_UpdateEx(pMe->m_pDisplay, FALSE); 

    return TRUE;
}

/*==============================================================================
函数:
       CContApp_AddSelectFieldListNode
说明:
       将指定的SelectField加入SelectField List, 并排列在指定
       Node的后面.

参数:
       ppNode [IO]: 指向sSelectFieldListNode结构指针的指针.
       pChInfo [In]: 指向sSelectFieldInfo结构的指针.

返回值:
       TRUE: 成功
       FALSE: 失败

备注:

==============================================================================*/
boolean CContApp_AddSelectFieldListNode(sSelectFieldListNode* pNode, sSelectFieldInfo* pSelectFieldInfo)
{
    sSelectFieldListNode* pSelectFieldNode = NULL;
    
    if (NULL == pSelectFieldInfo)
    {
        return FALSE;
    }
    
    pSelectFieldNode = (sSelectFieldListNode*)MALLOC(sizeof(sSelectFieldListNode));
    
    if(pSelectFieldNode == NULL)
    {
        return FALSE;
    }
    
    pCurSelectFieldNode = pNode;
    
    pSelectFieldNode->SelectFieldInfo.wIndex = pSelectFieldInfo->wIndex;
    pSelectFieldNode->SelectFieldInfo.wFieldCount = pSelectFieldInfo->wFieldCount;
    pSelectFieldNode->SelectFieldInfo.wFieldMask = pSelectFieldInfo->wFieldMask;
    pSelectFieldNode->SelectFieldInfo.wFieldMask |= MASK_ADDRFIELD_NAME;
    
    if(pCurSelectFieldNode == NULL)
    {
        //仅当传入的指针是pSelectFieldListRoot才允许其值为NULL
        pCurSelectFieldNode = pSelectFieldNode;
        pSelectFieldListRoot = pCurSelectFieldNode;
        pCurSelectFieldNode->pPreNode = NULL;
        pCurSelectFieldNode->pNextNode = NULL;      
    }
    else
    {   
        pSelectFieldNode->pNextNode = pCurSelectFieldNode->pNextNode;
        pSelectFieldNode->pPreNode = pCurSelectFieldNode;
        if(pCurSelectFieldNode->pNextNode)
        {
            pCurSelectFieldNode->pNextNode->pPreNode = pSelectFieldNode;
        }
        pCurSelectFieldNode->pNextNode = pSelectFieldNode;                 
    }
   
    return TRUE;
}

/*==============================================================================
函数:
       CContApp_DeleteSelectFieldListNode
说明:
       将指定的SelectField从SelectField List删除

参数:
       pNode [In]: 指向sSelectFieldListNode结构的指针.

返回值:
       TRUE: 成功
       FALSE: 失败

备注:

==============================================================================*/
boolean CContApp_DeleteSelectFieldListNode(sSelectFieldListNode* pNode, sSelectFieldInfo* pSelectFieldInfo)
{
    pCurSelectFieldNode = pNode;

    if(pCurSelectFieldNode == NULL)
    {
        return FALSE;
    }

    while (pCurSelectFieldNode != NULL)
    {
        if (pCurSelectFieldNode->SelectFieldInfo.wIndex == pSelectFieldInfo->wIndex)
        {
            /*这里主要考虑到删除的是不是第一个结点或者是最后一个结点并做特殊处理
            注意实际上我们所选上的第二个结点其实就是链表中的最后一个结点*/
            if (pCurSelectFieldNode == pSelectFieldListRoot)
            {
                pSelectFieldListRoot = pSelectFieldListRoot->pNextNode;
            }
            else
            {
                pCurSelectFieldNode->pPreNode->pNextNode = pCurSelectFieldNode->pNextNode;
            }
            if (pCurSelectFieldNode->pNextNode!=NULL)
            {
                pCurSelectFieldNode->pNextNode->pPreNode = pCurSelectFieldNode->pPreNode; 
            }
            break;
        }
        pCurSelectFieldNode = pCurSelectFieldNode->pNextNode;
    }    

    if (pCurSelectFieldNode != NULL)
    {
        FREE(pCurSelectFieldNode);    
        pCurSelectFieldNode = NULL;
    }
    
    return TRUE;
}

/*==============================================================================
函数:
       CContApp_FindSelectFieldListNode
说明:
       将指定的SelectField从SelectField List删除

参数:
       pNode [In]: 指向sSelectFieldListNode结构的指针.

返回值:
       TRUE: 成功
       FALSE: 失败

备注:

==============================================================================*/
uint16 CContApp_FindSelectFieldListNode(sSelectFieldListNode* pNode, sSelectFieldInfo* pSelectFieldInfo)
{
    uint16 rtn = TYPE_NO_MATCH;
    pCurSelectFieldNode = pNode;
    
    while (pCurSelectFieldNode != NULL)
    {
        if (pCurSelectFieldNode->SelectFieldInfo.wIndex ==  pSelectFieldInfo->wIndex)
        {   
            rtn = TYPE_INDEX_MATCH;
            if ((pCurSelectFieldNode->SelectFieldInfo.wFieldMask & pSelectFieldInfo->wFieldMask)  != 0)
            {
                rtn = TYPE_FIELD_MATCH;
            }
            break;
        }
        pCurSelectFieldNode = pCurSelectFieldNode->pNextNode;
    }      

    return rtn;
}

/*==============================================================================
函数:
       CContApp_FindSelectNodeFieldMark
说明:
       将指定的SelectField从SelectField List删除

参数:
       pNode [In]: 指向sSelectFieldListNode结构的指针.

返回值:
       TRUE: 成功
       FALSE: 失败

备注:

==============================================================================*/
boolean CContApp_SetSelectNodeFieldMark(sSelectFieldListNode* pNode, sSelectFieldInfo* pSelectFieldInfo)
{
    boolean rtn = FALSE;
    pCurSelectFieldNode = pNode;
    
    while (pCurSelectFieldNode != NULL)
    {
        if (pCurSelectFieldNode->SelectFieldInfo.wIndex ==  pSelectFieldInfo->wIndex)
        {   
            pCurSelectFieldNode->SelectFieldInfo.wFieldMask = pSelectFieldInfo->wFieldMask;
            rtn = TRUE;
            break;
        }
        pCurSelectFieldNode = pCurSelectFieldNode->pNextNode;
    }      

    return rtn;
}

/*==============================================================================
函数:
       CContApp_UpdateSelectFieldListNode
说明:
       将指定的SelectField加入SelectField List, 并排列在指定
       Node的后面.

参数:
       ppNode [IO]: 指向sSelectFieldListNode结构指针的指针.
       pChInfo [In]: 指向sSelectFieldInfo结构的指针.

返回值:
       TRUE: 成功
       FALSE: 失败

备注:

==============================================================================*/
boolean CContApp_UpdateSelectFieldListNode(sSelectFieldListNode* pNode, sSelectFieldInfo* pSelectFieldInfo)
{
    pCurSelectFieldNode = pNode;

    while (pCurSelectFieldNode != NULL)
    {
        if (pCurSelectFieldNode->SelectFieldInfo.wIndex ==  pSelectFieldInfo->wIndex)
        {
            if (pCurSelectFieldNode->SelectFieldInfo.wFieldMask & pSelectFieldInfo->wFieldMask)
            {
                pCurSelectFieldNode->SelectFieldInfo.wFieldMask = pCurSelectFieldNode->SelectFieldInfo.wFieldMask & (~pSelectFieldInfo->wFieldMask);
            }
            else
            {
                pCurSelectFieldNode->SelectFieldInfo.wFieldMask = pCurSelectFieldNode->SelectFieldInfo.wFieldMask |pSelectFieldInfo->wFieldMask;
            }
            return TRUE;
        }
        pCurSelectFieldNode = pCurSelectFieldNode->pNextNode;
    }   
   
    return TRUE;
}


boolean CContApp_GetSelectFieldByID(CContApp *pMe, sSelectFieldListNode* pNode, uint16 wContID)
{
    uint16        j;
    uint16        wFldNum;
    IAddrRec     *pAddrRec = NULL;
    AEEAddrField *pNameFld = NULL;
    AEEAddrField *pNumFld = NULL;

    ASSERT(pMe != NULL);

    if(IS_RUIM_REC(wContID))
    {
        pAddrRec = IADDRBOOK_GetRecByID( pMe->m_pAddrRUIM, wContID -CONTAPP_RUIM_START_IDX +1);
        if (pAddrRec == NULL)
        {
            FARF(ADDR, ("Fail to get rec %d", wContID));
            return FALSE;
        }
        else
        {
            //get total fields
            wFldNum  = (uint16)IADDRREC_GetFieldCount(pAddrRec);
            pMe->m_wFieldCount = wFldNum;

            if(wFldNum == 0)
            {
                (void)IADDRREC_Release(pAddrRec);
                pAddrRec = NULL;
                return FALSE;
            }
            // Get name field
            pNameFld = IADDRREC_GetField(pAddrRec, IDX_NAME_FLD);

            FREEIF(pNode->SelectFieldInfo.m_pAddNewName);
            pNode->SelectFieldInfo.m_pAddNewName = WSTRDUP((AECHAR *)pNameFld->pBuffer);

            for (j = 1; j < wFldNum; j++)
            {
                //Get number field
                pNumFld = IADDRREC_GetField(pAddrRec, j);

                // Get mobile number
                FREEIF(pNode->SelectFieldInfo.m_pAddNewMobile);
                pNode->SelectFieldInfo.m_pAddNewMobile = WSTRDUP((AECHAR *)pNumFld->pBuffer);				
            }            
            //end for(j=0;;)
        }//end if(addrRec==NULL)

        IADDRREC_Release(pAddrRec);
        pAddrRec = NULL; 

    }
    else
    {
        pAddrRec = IADDRBOOK_GetRecByID( pMe->m_pAddrPhone, wContID);
        if (pAddrRec == NULL)
        {
            FARF(ADDR, ("Fail to get rec %d", wContID));
            return FALSE;
        }
        else
        {
            //get total fields
            wFldNum  = (uint16)IADDRREC_GetFieldCount(pAddrRec);
            pMe->m_wFieldCount = wFldNum;

            if(wFldNum == 0)
            {
                (void)IADDRREC_Release(pAddrRec);
                pAddrRec = NULL;
                return FALSE;
            }

            // Get name field
            pNameFld = IADDRREC_GetField(pAddrRec, IDX_NAME_FLD);

            FREEIF(pNode->SelectFieldInfo.m_pAddNewName);
            pNode->SelectFieldInfo.m_pAddNewName = WSTRDUP((AECHAR *)pNameFld->pBuffer);

            for (j = 1; j < wFldNum; j++)
            {
                //Get number field
                pNumFld = IADDRREC_GetField(pAddrRec, j);
                switch (pNumFld->fID)
                {
                    case AEE_ADDRFIELD_PHONE_GENERIC:
                        // Get mobile number
                        FREEIF(pNode->SelectFieldInfo.m_pAddNewMobile);
                        pNode->SelectFieldInfo.m_pAddNewMobile = WSTRDUP((AECHAR *)pNumFld->pBuffer);						
                        break;

                    case AEE_ADDRFIELD_PHONE_HOME:
                        FREEIF(pNode->SelectFieldInfo.m_pAddNewHome);
                        pNode->SelectFieldInfo.m_pAddNewHome = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        break;

                    case AEE_ADDRFIELD_PHONE_WORK:
                        FREEIF(pNode->SelectFieldInfo.m_pAddNewOffice);
                        pNode->SelectFieldInfo.m_pAddNewOffice = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        break;

                    case AEE_ADDRFIELD_PHONE_FAX:
                        FREEIF(pNode->SelectFieldInfo.m_pAddNewFax);
                        pNode->SelectFieldInfo.m_pAddNewFax = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        break;

                    case AEE_ADDRFIELD_EMAIL:
                        // Get notes
                        FREEIF(pNode->SelectFieldInfo.m_pAddNewEMAIL);
                        pNode->SelectFieldInfo.m_pAddNewEMAIL = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        break;

                    case AEE_ADDRFIELD_ADDRESS:
                        FREEIF(pNode->SelectFieldInfo.m_pAddNewAddress);
                        pNode->SelectFieldInfo.m_pAddNewAddress = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        break;

                    case AEE_ADDRFIELD_URL:
                        FREEIF(pNode->SelectFieldInfo.m_pAddNewURL);
                        pNode->SelectFieldInfo.m_pAddNewURL = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        break;

                    case AEE_ADDRFIELD_NOTES:
                        // Set birthday field
                        FREEIF(pNode->SelectFieldInfo.m_pAddNewRemark);
                        pNode->SelectFieldInfo.m_pAddNewRemark = WSTRDUP((AECHAR *)pNumFld->pBuffer);
                        break;
                        
                    default:
                        break;
                }
            }//end for(j=0;;)
        }//end if(addrRec==NULL)

    IADDRREC_Release(pAddrRec);
    pAddrRec = NULL; 
    }
    return TRUE;
}

boolean CContApp_GetSelectFieldListNode(CContApp *pMe, sSelectFieldListNode* pNode)
{
    pCurSelectFieldNode = pNode;

    while (pCurSelectFieldNode != NULL)
    {
        CContApp_GetSelectFieldByID(pMe, pCurSelectFieldNode, pCurSelectFieldNode->SelectFieldInfo.wIndex);
     
        pCurSelectFieldNode = pCurSelectFieldNode->pNextNode;
    }
    return TRUE;
}

// 是CContApp_GetSelectFieldListNode的简化版，这个函数目的只要返回wFieldMask， pSelectFieldInfo->wIndex必须有效
int CContApp_GetSelectNode(sSelectFieldListNode* pNode, sSelectFieldInfo* sSelectFieldInfo)
{
    pCurSelectFieldNode = pNode;
    
    while (pCurSelectFieldNode != NULL)
    {
        if (pCurSelectFieldNode->SelectFieldInfo.wIndex ==  sSelectFieldInfo->wIndex)
        {   
            sSelectFieldInfo->wFieldMask = pCurSelectFieldNode->SelectFieldInfo.wFieldMask;
            return SUCCESS;
        }
        pCurSelectFieldNode = pCurSelectFieldNode->pNextNode;
    }      
    
    return EFAILED;
}

boolean CContApp_FormatSelectFieldContentListNode(CContApp *pMe, sSelectFieldListNode* pNode)
{
    int nLen = 0, nTotalLen = 0, MAX_LEN = 1024;
    boolean bRet = FALSE;
    sSelectFieldListNode *pTepNode = NULL;
    int32 dwSize = AEE_ADDR_CAT_MAX_CHAR * sizeof(AECHAR);
    
    AECHAR *m_sName = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sMobile = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sHome = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sOffice = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sFax = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sAddress = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sURL = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sEMail = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sNotes = (AECHAR *)MALLOC(dwSize);
    AECHAR *wstrFMTED;
    
    if (pSelectFieldListRoot == NULL)
    {
        goto CContApp_FormatSelectFieldContentListNode_EXIT;
    }
    
    if (NULL != pSelectFieldListRoot->SelectFieldInfo.m_pAddNewFormatContent)
    {
        FREEIF(pSelectFieldListRoot->SelectFieldInfo.m_pAddNewFormatContent);
    }
    pSelectFieldListRoot->SelectFieldInfo.m_pAddNewFormatContent = (AECHAR *)MALLOC(MAX_LEN * sizeof(AECHAR));
    
    if ((NULL == m_sName) ||
        (NULL == m_sMobile) ||
        (NULL == m_sHome) ||
        (NULL == m_sOffice) ||
        (NULL == m_sFax) ||
        (NULL == m_sAddress) ||
        (NULL == m_sURL) ||
        (NULL == m_sEMail) ||
        (NULL == m_sNotes) ||
        (NULL == pSelectFieldListRoot->SelectFieldInfo.m_pAddNewFormatContent))
    {
        goto CContApp_FormatSelectFieldContentListNode_EXIT;
    }
    
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_NAME, m_sName, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_MOBILE, m_sMobile, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_HOME, m_sHome, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_OFFICE, m_sOffice, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_FAX, m_sFax, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_ADDRESS, m_sAddress, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_URL, m_sURL, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_EMAIL, m_sEMail, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_NOTES, m_sNotes, dwSize);

    pTepNode = pNode;
    wstrFMTED = pSelectFieldListRoot->SelectFieldInfo.m_pAddNewFormatContent;
    while (pTepNode != NULL)
    {
        if (pTepNode->SelectFieldInfo.wFieldMask & MASK_ADDRFIELD_NAME)
        {
            nLen = WSTRLEN(m_sName);
            if ((nTotalLen+nLen+1) < MAX_LEN)
            {
                WSTRCAT(wstrFMTED, m_sName);
                nTotalLen += nLen;
                wstrFMTED[nTotalLen] = (AECHAR)':';
                nTotalLen++;
                nLen = WSTRLEN(pTepNode->SelectFieldInfo.m_pAddNewName);
                //if ((nTotalLen+nLen+1) < MAX_LEN_COMMBUF)
                {
                    WSTRCAT(wstrFMTED, pTepNode->SelectFieldInfo.m_pAddNewName);
                    nTotalLen += nLen;
                    wstrFMTED[nTotalLen] = (AECHAR)'\n';
                    nTotalLen++;
                }
            }
        }
        if (pTepNode->SelectFieldInfo.wFieldMask & MASK_ADDRFIELD_PHONE_GENERIC)
        {
            nLen = WSTRLEN(m_sMobile);
            if ((nTotalLen+nLen+1) < MAX_LEN)
            {
                WSTRCAT(wstrFMTED, m_sMobile);
                nTotalLen += nLen;
                wstrFMTED[nTotalLen] = (AECHAR)':';
                nTotalLen++;
                nLen = WSTRLEN(pTepNode->SelectFieldInfo.m_pAddNewMobile);
                //if ((nTotalLen+nLen+1) < MAX_LEN_COMMBUF)
                {
                    WSTRCAT(wstrFMTED, pTepNode->SelectFieldInfo.m_pAddNewMobile);
                    nTotalLen += nLen;
                    wstrFMTED[nTotalLen] = (AECHAR)'\n';
                    nTotalLen++;
                }				
            }
        }
        if (pTepNode->SelectFieldInfo.wFieldMask & MASK_ADDRFIELD_PHONE_HOME)
        {
            nLen = WSTRLEN(m_sHome);
            if ((nTotalLen+nLen+1) < MAX_LEN)
            {
                WSTRCAT(wstrFMTED, m_sHome);
                nTotalLen += nLen;
                wstrFMTED[nTotalLen] = (AECHAR)':';
                nTotalLen++;
                nLen = WSTRLEN(pTepNode->SelectFieldInfo.m_pAddNewHome);
                //if ((nTotalLen+nLen+1) < MAX_LEN_COMMBUF)
                {
                    WSTRCAT(wstrFMTED, pTepNode->SelectFieldInfo.m_pAddNewHome);
                    nTotalLen += nLen;
                    wstrFMTED[nTotalLen] = (AECHAR)'\n';
                    nTotalLen++;
                }
            }
        }
        if (pTepNode->SelectFieldInfo.wFieldMask & MASK_ADDRFIELD_PHONE_WORK)
        {
            nLen = WSTRLEN(m_sOffice);
            if ((nTotalLen+nLen+1) < MAX_LEN)
            {
                WSTRCAT(wstrFMTED, m_sOffice);
                nTotalLen += nLen;
                wstrFMTED[nTotalLen] = (AECHAR)':';
                nTotalLen++;
                nLen = WSTRLEN(pTepNode->SelectFieldInfo.m_pAddNewOffice);
                //if ((nTotalLen+nLen+1) < MAX_LEN_COMMBUF)
                {
                    WSTRCAT(wstrFMTED, pTepNode->SelectFieldInfo.m_pAddNewOffice);
                    nTotalLen += nLen;
                    wstrFMTED[nTotalLen] = (AECHAR)'\n';
                    nTotalLen++;
                }
            }
        }
        if (pTepNode->SelectFieldInfo.wFieldMask & MASK_ADDRFIELD_PHONE_FAX)
        {
            nLen = WSTRLEN(m_sFax);
            if ((nTotalLen+nLen+1) < MAX_LEN)
            {
                WSTRCAT(wstrFMTED, m_sFax);
                nTotalLen += nLen;
                wstrFMTED[nTotalLen] = (AECHAR)':';
                nTotalLen++;
                nLen = WSTRLEN(pTepNode->SelectFieldInfo.m_pAddNewFax);
                //if ((nTotalLen+nLen+1) < MAX_LEN_COMMBUF)
                {
                    WSTRCAT(wstrFMTED, pTepNode->SelectFieldInfo.m_pAddNewFax);
                    nTotalLen += nLen;
                    wstrFMTED[nTotalLen] = (AECHAR)'\n';
                    nTotalLen++;
                }
            }
        }
        if (pTepNode->SelectFieldInfo.wFieldMask & MASK_ADDRFIELD_EMAIL)
        {
            nLen = WSTRLEN(m_sEMail);
            if ((nTotalLen+nLen+1) < MAX_LEN)
            {
                WSTRCAT(wstrFMTED, m_sEMail);
                nTotalLen += nLen;
                wstrFMTED[nTotalLen] = (AECHAR)' ';
                nTotalLen++;
                nLen = WSTRLEN(pTepNode->SelectFieldInfo.m_pAddNewEMAIL);
                //if ((nTotalLen+nLen+1) < MAX_LEN_COMMBUF)
                {
                    WSTRCAT(wstrFMTED, pTepNode->SelectFieldInfo.m_pAddNewEMAIL);
                    nTotalLen += nLen;
                    wstrFMTED[nTotalLen] = (AECHAR)'\n';
                    nTotalLen++;
                }
            }
        }
        if (pTepNode->SelectFieldInfo.wFieldMask & MASK_ADDRFIELD_ADDRESS)
        {
            nLen = WSTRLEN(m_sAddress);
            if ((nTotalLen+nLen+1) < MAX_LEN)
            {
                WSTRCAT(wstrFMTED, m_sAddress);
                nTotalLen += nLen;
                wstrFMTED[nTotalLen] = (AECHAR)':';
                nTotalLen++;
                nLen = WSTRLEN(pTepNode->SelectFieldInfo.m_pAddNewAddress);
                //if ((nTotalLen+nLen+1) < MAX_LEN_COMMBUF)
                {
                    WSTRCAT(wstrFMTED, pTepNode->SelectFieldInfo.m_pAddNewAddress);
                    nTotalLen += nLen;
                    wstrFMTED[nTotalLen] = (AECHAR)'\n';
                    nTotalLen++;
                }
            }
        }
        if (pTepNode->SelectFieldInfo.wFieldMask & MASK_ADDRFIELD_URL)
        {
            nLen = WSTRLEN(m_sURL);
            if ((nTotalLen+nLen+1) < MAX_LEN)
            {
                WSTRCAT(wstrFMTED, m_sURL);
                nTotalLen += nLen;
                wstrFMTED[nTotalLen] = (AECHAR)' ';
                nTotalLen++;
                nLen = WSTRLEN(pTepNode->SelectFieldInfo.m_pAddNewURL);
                //if ((nTotalLen+nLen+1) < MAX_LEN_COMMBUF)
                {
                    WSTRCAT(wstrFMTED, pTepNode->SelectFieldInfo.m_pAddNewURL);
                    nTotalLen += nLen;
                    wstrFMTED[nTotalLen] = (AECHAR)'\n';
                    nTotalLen++;
                }
            }
        }
        if (pTepNode->SelectFieldInfo.wFieldMask & MASK_ADDRFIELD_NOTES)
        {
            nLen = WSTRLEN(m_sNotes);
            if ((nTotalLen+nLen+1) < MAX_LEN)
            {
                WSTRCAT(wstrFMTED, m_sNotes);
                nTotalLen += nLen;
                wstrFMTED[nTotalLen] = (AECHAR)' ';
                nTotalLen++;
                nLen = WSTRLEN(pTepNode->SelectFieldInfo.m_pAddNewRemark);
                //if ((nTotalLen+nLen+1) < MAX_LEN_COMMBUF)
                {
                    WSTRCAT(wstrFMTED, pTepNode->SelectFieldInfo.m_pAddNewRemark);
                    nTotalLen += nLen;
                    wstrFMTED[nTotalLen] = (AECHAR)'\n';
                    nTotalLen++;
                }
            
            }
        }
        
        pTepNode = pTepNode->pNextNode;
    }
    
    bRet = TRUE;
    
CContApp_FormatSelectFieldContentListNode_EXIT:
    FREEIF(m_sName);
    FREEIF(m_sMobile);
    FREEIF(m_sHome);
    FREEIF(m_sOffice);
    FREEIF(m_sFax);
    FREEIF(m_sAddress);
    FREEIF(m_sURL);
    FREEIF(m_sEMail);
    FREEIF(m_sNotes);
    
    return bRet;
}

boolean CContApp_FormatSendDirectoryContent(CContApp *pMe)
{
    int nLen = 0, nTotalLen = 0;
    boolean bRet = FALSE;
    int32 dwSize = sizeof(AECHAR) * AEE_ADDR_CAT_MAX_CHAR;
    
    AECHAR *m_sName = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sMobile = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sHome = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sOffice = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sFax = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sAddress = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sURL = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sEMail = (AECHAR *)MALLOC(dwSize);
    AECHAR *m_sNotes = (AECHAR *)MALLOC(dwSize);
    
    if ((NULL == m_sName) || 
        (NULL == m_sMobile) ||
        (NULL == m_sHome) ||
        (NULL == m_sOffice) ||
        (NULL == m_sFax) ||
        (NULL == m_sAddress) ||
        (NULL == m_sURL) ||
        (NULL == m_sEMail) ||
        (NULL == m_sNotes) ||
        (NULL == pMe->m_pBuf))
    {
        goto FormatSendDirectoryContent_EXIT;
    }

    MEMSET(pMe->m_pBuf, 0, (MAX_LEN_WMS_BUF +1)*sizeof(AECHAR));    

    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_NAME, m_sName, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_MOBILE, m_sMobile, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_HOME, m_sHome, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_OFFICE, m_sOffice, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_FAX, m_sFax, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_ADDRESS, m_sAddress, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_URL, m_sURL, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_EMAIL, m_sEMail, dwSize);
    (void)ISHELL_LoadResString(pMe->m_pShell, CONTAPP_RES_FILE_LANG, IDS_NOTES, m_sNotes, dwSize);

    // 以下每一部份需要加上长度不超过 pMe->m_pBuf 容量的检查
    if (pMe->m_pAddNewName)
    {
        nLen = WSTRLEN(m_sName) +  WSTRLEN(pMe->m_pAddNewName);
        if ((nTotalLen+nLen) < MAX_LEN_WMS_BUF)
        {
            WSTRCAT(pMe->m_pBuf, m_sName);
            nTotalLen += WSTRLEN(m_sName);
            pMe->m_pBuf[nTotalLen] = (AECHAR)':';
            nTotalLen++;
        
            WSTRCAT(pMe->m_pBuf, pMe->m_pAddNewName);
            nTotalLen += WSTRLEN(pMe->m_pAddNewName);
            pMe->m_pBuf[nTotalLen] = (AECHAR)'\n';
            nTotalLen++;
        }
            
        FREE(pMe->m_pAddNewName);
        pMe->m_pAddNewName = NULL;
    }

    if (pMe->m_pAddNewMobile)
    {	
        nLen = WSTRLEN(m_sMobile) + WSTRLEN(pMe->m_pAddNewMobile);
        if ((nTotalLen+nLen) < MAX_LEN_WMS_BUF)
        {
            WSTRCAT(pMe->m_pBuf, m_sMobile);
            nTotalLen += WSTRLEN(m_sMobile);
            pMe->m_pBuf[nTotalLen] = (AECHAR)':';
            nTotalLen++;
            WSTRCAT(pMe->m_pBuf, pMe->m_pAddNewMobile);
            nTotalLen += WSTRLEN(pMe->m_pAddNewMobile);
            pMe->m_pBuf[nTotalLen] = (AECHAR)'\n';
            nTotalLen++;
        }

        FREE(pMe->m_pAddNewMobile);
        pMe->m_pAddNewMobile = NULL;
    }
    if (pMe->m_pAddNewHome)
    {
        nLen = WSTRLEN(m_sHome) + WSTRLEN(pMe->m_pAddNewHome);
        if ((nTotalLen+nLen) < MAX_LEN_WMS_BUF)
        {
            WSTRCAT(pMe->m_pBuf, m_sHome);
            nTotalLen += WSTRLEN(m_sHome);
            pMe->m_pBuf[nTotalLen] = (AECHAR)':';
            nTotalLen++;
            WSTRCAT(pMe->m_pBuf, pMe->m_pAddNewHome);
            nTotalLen += WSTRLEN(pMe->m_pAddNewHome);
            pMe->m_pBuf[nTotalLen] = (AECHAR)'\n';
            nTotalLen++;
        }
        
        FREE(pMe->m_pAddNewHome);
        pMe->m_pAddNewHome = NULL;
    }
    if (pMe->m_pAddNewOffice)
    {
        nLen = WSTRLEN(m_sOffice) + WSTRLEN(pMe->m_pAddNewOffice);
        if ((nTotalLen+nLen) < MAX_LEN_WMS_BUF)
        {
            WSTRCAT(pMe->m_pBuf, m_sOffice);
            nTotalLen += WSTRLEN(m_sOffice);
            pMe->m_pBuf[nTotalLen] = (AECHAR)':';
            nTotalLen++;
            WSTRCAT(pMe->m_pBuf, pMe->m_pAddNewOffice);
            nTotalLen += WSTRLEN(pMe->m_pAddNewOffice);
            pMe->m_pBuf[nTotalLen] = (AECHAR)'\n';
            nTotalLen++;
        }
        
        FREE(pMe->m_pAddNewOffice);
        pMe->m_pAddNewOffice = NULL;
    }
    if (pMe->m_pAddNewFax)
    {
        nLen = WSTRLEN(m_sFax) + WSTRLEN(pMe->m_pAddNewFax);
        if ((nTotalLen+nLen) < MAX_LEN_WMS_BUF)
        {
            WSTRCAT(pMe->m_pBuf, m_sFax);
            nTotalLen += WSTRLEN(m_sFax);
            pMe->m_pBuf[nTotalLen] = (AECHAR)':';
            nTotalLen++;
            WSTRCAT(pMe->m_pBuf, pMe->m_pAddNewFax);
            nTotalLen += WSTRLEN(pMe->m_pAddNewFax);
            pMe->m_pBuf[nTotalLen] = (AECHAR)'\n';
            nTotalLen++;
        }
        
        FREE(pMe->m_pAddNewFax);
        pMe->m_pAddNewFax = NULL;
    }
    if (pMe->m_pAddNewEMAIL)
    {
        nLen = WSTRLEN(m_sEMail) + WSTRLEN(pMe->m_pAddNewEMAIL);
        if ((nTotalLen+nLen) < MAX_LEN_WMS_BUF)
        {
            WSTRCAT(pMe->m_pBuf, m_sEMail);
            nTotalLen += WSTRLEN(m_sEMail);
            pMe->m_pBuf[nTotalLen] = (AECHAR)' ';
            nTotalLen++;
            WSTRCAT(pMe->m_pBuf, pMe->m_pAddNewEMAIL);
            nTotalLen += WSTRLEN(pMe->m_pAddNewEMAIL);
            pMe->m_pBuf[nTotalLen] = (AECHAR)'\n';
            nTotalLen++;
        }
        
        FREE(pMe->m_pAddNewEMAIL); 
        pMe->m_pAddNewEMAIL = NULL;
    }
    if (pMe->m_pAddNewAddress)
    {
        nLen = WSTRLEN(m_sAddress) + WSTRLEN(pMe->m_pAddNewAddress);
        if ((nTotalLen+nLen) < MAX_LEN_WMS_BUF)
        {
            WSTRCAT(pMe->m_pBuf, m_sAddress);
            nTotalLen += WSTRLEN(m_sAddress);
            pMe->m_pBuf[nTotalLen] = (AECHAR)':';
            nTotalLen++;
            WSTRCAT(pMe->m_pBuf, pMe->m_pAddNewAddress);
            nTotalLen += WSTRLEN(pMe->m_pAddNewAddress);
            pMe->m_pBuf[nTotalLen] = (AECHAR)'\n';
            nTotalLen++;
        }
        
        FREE(pMe->m_pAddNewAddress); 
        pMe->m_pAddNewAddress = NULL;
    }
    if (pMe->m_pAddNewURL)
    {
        nLen = WSTRLEN(m_sURL) + WSTRLEN(pMe->m_pAddNewURL);
        if ((nTotalLen+nLen) < MAX_LEN_WMS_BUF)
        {
            WSTRCAT(pMe->m_pBuf, m_sURL);
            nTotalLen += WSTRLEN(m_sURL);
            pMe->m_pBuf[nTotalLen] = (AECHAR)' ';
            nTotalLen++;
            WSTRCAT(pMe->m_pBuf, pMe->m_pAddNewURL);
            nTotalLen += WSTRLEN(pMe->m_pAddNewURL);
            pMe->m_pBuf[nTotalLen] = (AECHAR)'\n';
            nTotalLen++;
        }
        
        FREE(pMe->m_pAddNewURL);  
        pMe->m_pAddNewURL = NULL;    
    }
    if (pMe->m_pAddNewRemark)
    {
        nLen = WSTRLEN(m_sNotes) + WSTRLEN(pMe->m_pAddNewRemark);
        if ((nTotalLen+nLen) < MAX_LEN_WMS_BUF)
        {
            WSTRCAT(pMe->m_pBuf, m_sNotes);
            nTotalLen += WSTRLEN(m_sNotes);
            pMe->m_pBuf[nTotalLen] = (AECHAR)':';
            nTotalLen++;
            WSTRCAT(pMe->m_pBuf, pMe->m_pAddNewRemark);
            nTotalLen += WSTRLEN(pMe->m_pAddNewRemark);
            pMe->m_pBuf[nTotalLen] = (AECHAR)'\n';
            nTotalLen++;
        }
        
        FREE(pMe->m_pAddNewRemark);  
        pMe->m_pAddNewRemark = NULL; 
    }
    
    bRet = TRUE;

FormatSendDirectoryContent_EXIT:
    FREEIF(m_sName);
    FREEIF(m_sMobile);
    FREEIF(m_sHome);
    FREEIF(m_sOffice);
    FREEIF(m_sFax);
    FREEIF(m_sAddress);
    FREEIF(m_sEMail);
    FREEIF(m_sURL);
    FREEIF(m_sNotes);   

    return bRet;
}


boolean CContApp_FreeSelectFieldListNode()
{
    sSelectFieldListNode *pNode=pSelectFieldListRoot;
    
    while (pNode != NULL)
    {
        FREEIF(pNode->SelectFieldInfo.m_pAddNewName);
        FREEIF(pNode->SelectFieldInfo.m_pAddNewMobile);
        FREEIF(pNode->SelectFieldInfo.m_pAddNewHome);
        FREEIF(pNode->SelectFieldInfo.m_pAddNewOffice);
        FREEIF(pNode->SelectFieldInfo.m_pAddNewFax);
        FREEIF(pNode->SelectFieldInfo.m_pAddNewEMAIL);
        FREEIF(pNode->SelectFieldInfo.m_pAddNewAddress);
        FREEIF(pNode->SelectFieldInfo.m_pAddNewURL);
        FREEIF(pNode->SelectFieldInfo.m_pAddNewRemark);
        
        pSelectFieldListRoot = pNode->pNextNode;
        
        if(NULL != pNode)
        {
            FREEIF(pNode);  
        }
        
        pNode=pSelectFieldListRoot;
    }
    
    return TRUE;
}

boolean CContApp_Phonebook_Load(CContApp *pMe)
{
    int nRet;
    
    nRet = CContApp_GetConfig(pMe, CONTCFG_VIEWTYPE, &pMe->m_nViewType, sizeof(pMe->m_nViewType));

    if(nRet != SUCCESS)
    {
        return FALSE;
    }

    // First load all the phonebook
    if(SUCCESS != CContApp_LoadPhoneBook(pMe, pMe->m_nViewType))
    {
        return FALSE;
    }
    return TRUE;
}

/*=============================================================================

FUNCTION:CContApp_GetNumberByField

DESCRIPTION:
    对指定的字段发起呼叫
    
PARAMETERS:
    pMe     :
    idx     : 选择字段的 index
        
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    This function only used in view dialog and.
    Assume the m_pFldIv is available.
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/
AECHAR* CContApp_GetNumberByField(CContApp *pMe, int idx)
{
    AEEAddrField *pFld;
    
    ASSERT(pMe != NULL); 
    
    pFld = IVector_ElementAt( pMe->m_pFldIv, (uint32)idx );
    if(pFld == NULL)
    {
        return NULL;
    }
    
    // Is number field ?
    if(!IS_NUM_FIELD(pFld->fID))
    {
        return NULL;
    }

    return(AECHAR *)pFld->pBuffer;
}// CContApp_MakeCall

/*=============================================================================

FUNCTION:CContApp_GetOneNumberFld

DESCRIPTION:
    对指定的字段发起呼叫
    
PARAMETERS:
    pMe     :
    idx     : 选择字段的 index
        
RETURN VALUE:
    SUCCESS: 
    
COMMENTS:
    This function only used in view dialog and.
    Assume the m_pFldIv is available.
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/

int CContApp_GetOneNumberFld(CContApp *pMe, int idx)
{
    AEEAddrField *pFld;
    int         fieldMask;
    
    ASSERT(pMe != NULL); 
    
    pFld = IVector_ElementAt( pMe->m_pFldIv, (uint32)idx );
    if(pFld == NULL)
    {
        return NULL;
    }
    
    // Is number field ?
    if(!IS_NUM_FIELD(pFld->fID))
    {
        return NULL;
    }

    switch(pFld->fID)
    {
        //phone
        case AEE_ADDRFIELD_PHONE_GENERIC :
            fieldMask = MASK_ADDRFIELD_PHONE_GENERIC;
            break;
            
        //home number
        case AEE_ADDRFIELD_PHONE_HOME :
            fieldMask = MASK_ADDRFIELD_PHONE_HOME;
            break;
    
        //work number
        case AEE_ADDRFIELD_PHONE_WORK :
            fieldMask = MASK_ADDRFIELD_PHONE_WORK;
            break;
    
        //fax number
        case AEE_ADDRFIELD_PHONE_FAX :
            fieldMask = MASK_ADDRFIELD_PHONE_FAX;
            break;
            
        default :
            return 0;
    }

    return fieldMask;
}// 

/*=============================================================================

FUNCTION:CContApp_GetPrimaryNumFld

DESCRIPTION:
    获取优先级最高的字段
    
PARAMETERS:
    pMe     :
        
RETURN VALUE:
    
COMMENTS:
    This function only used in view dialog and.
    Assume the m_pFldIv is available.
    
SIDE EFFECTS:
    
SEE ALSO:

=============================================================================*/

int CContApp_GetPrimaryNumFld(CContApp *pMe)
{
    AEEAddrField *pFld;
    int         fieldMask;
    int         tempFldMsk;
    int fldnum = 0;
    int i;
    
    ASSERT(pMe != NULL); 

    if(SUCCESS != CContApp_LoadAddrFlds( pMe, 
                                         pMe->m_wSelectCont,
                                         SELECT_ALL));
    
    fldnum = IVector_Size(pMe->m_pFldIv);
    if(1 == CContApp_GetCurrFldNum(pMe, AEE_ADDRFIELD_PHONE_GENERIC))
    {
        return CContApp_GetOneNumberFld(pMe, IDX_NUM_RUIM_FLD);
    }
    else
    {
        fieldMask = MASK_ADDRFIELD_PHONE_FAX;
        for(i=1; i<fldnum; i++)
        {
            //IVector_ElementAt( pMe->m_pFldIv, 0 ) is MASK_ADDRFIELD_NAME
            pFld = IVector_ElementAt( pMe->m_pFldIv, (uint32)i );
            if(pFld == NULL)
            {
                ERR("tamgwq pFld is NULL",0,0,0);
                return NULL;
            }
            
            // Is number field 
            if(!IS_NUM_FIELD(pFld->fID))
            {
                ERR("tamgwq pFld is not numfld",pFld->fID,0,0);
                continue;
            }

            switch(pFld->fID)
            {
                //phone
                case AEE_ADDRFIELD_PHONE_GENERIC :
                    tempFldMsk = MASK_ADDRFIELD_PHONE_GENERIC;
                    break;
                    
                //home number
                case AEE_ADDRFIELD_PHONE_HOME :
                    tempFldMsk = MASK_ADDRFIELD_PHONE_HOME;
                    break;
            
                //work number
                case AEE_ADDRFIELD_PHONE_WORK :
                    tempFldMsk = MASK_ADDRFIELD_PHONE_WORK;
                    break;
            
                //fax number
                case AEE_ADDRFIELD_PHONE_FAX :
                    tempFldMsk = MASK_ADDRFIELD_PHONE_FAX;
                    break;
                    
                default :
                    return 0;
            }
            if(tempFldMsk < fieldMask)
            {
                fieldMask = tempFldMsk;
            }
        }
    }

    return fieldMask;
}// 

/*==============================================================================
函数:
    ContApp_NumberMatch

说明:
    电话号码匹配规则函数。

参数:
    wstrNum1 [in]: 号码 1
    wstrNum2 [in]: 号码 2
       
返回值:
    TRUE : 两个号码匹配
    FALSE: 两个号码不匹配

备注:
    
       
==============================================================================*/
Numbermatch_e_Type ContApp_NumberMatch(const AECHAR * wstrNum1, const AECHAR * wstrNum2, int *pMatchChars)
{
    int nLen1, nLen2;
    AECHAR wstrNum1cmpbuff[MAX_INPUT_NUM + 1] = {(AECHAR)'\0'};
    AECHAR wstrNum2cmpbuff[MAX_INPUT_NUM + 1] = {(AECHAR)'\0'};
    AECHAR *pwstr1 = NULL;
    AECHAR *pwstr2 = NULL;
    Numbermatch_e_Type eRet = NUMBERMATCH_IRRELEVANCE;
    
    if (((NULL == wstrNum1)||(WSTRLEN(wstrNum1) > MAX_INPUT_NUM))\
        || ((NULL == wstrNum2)||(WSTRLEN(wstrNum2) > MAX_INPUT_NUM)))
    {
        return eRet;
    }

    (void)WSTRCPY(wstrNum1cmpbuff, wstrNum1);
    (void)WSTRCPY(wstrNum2cmpbuff, wstrNum2);
    (void)ContApp_ChangePhoneNumberForNameCompare(wstrNum1cmpbuff);
    (void)ContApp_ChangePhoneNumberForNameCompare(wstrNum2cmpbuff);
    
    nLen1 = WSTRLEN(wstrNum1cmpbuff);
    nLen2 = WSTRLEN(wstrNum2cmpbuff);
	//add by miaoxiaoming
	if (nLen1 == 0||nLen2==0)
	{
		return eRet;
	}
      
    if (((nLen1 < MATCH_DIGITS_MIN) || (nLen2 < MATCH_DIGITS_MIN)) &&
        (nLen1 != nLen2))
    {// 比较号码长度不等，要求两者长度均不低于设定比较门限长度
        return eRet;
    }
        
    if (nLen1 == nLen2)
    {// 比较号码长度相等，要求数字全匹配
        if (0 == WSTRCMP(wstrNum1cmpbuff, wstrNum2cmpbuff))
        {// 号码匹配成功
            if (NULL != pMatchChars)
            {
                *pMatchChars = nLen1;
            }
            eRet = NUMBERMATCH_EQUAL;
        }
    }
    else if (nLen1>nLen2)
    {
        pwstr1 = (AECHAR *)wstrNum2cmpbuff;
        pwstr2 = (AECHAR *)&(wstrNum1cmpbuff[nLen1-nLen2]);
#if defined FEATURE_CARRIER_MAROC_WANA
        if ((WSTRNCMP(wstrNum1cmpbuff, L"00212", 5) == 0)&&(WSTRNCMP(wstrNum2cmpbuff, L"0", 1) == 0))
        {
            pwstr1 = (AECHAR *)&wstrNum2cmpbuff[1];
            pwstr2 = (AECHAR *)&(wstrNum1cmpbuff[nLen1-nLen2+1]);
        }
#elif defined FEATURE_CARRIER_TAIWAN_APBW
        if ((WSTRNCMP(wstrNum1cmpbuff, L"886", 3) == 0)&&(WSTRNCMP(wstrNum2cmpbuff, L"0", 1) == 0))
        {
            pwstr1 = (AECHAR *)&wstrNum2cmpbuff[1];
            pwstr2 = (AECHAR *)&(wstrNum1cmpbuff[nLen1-nLen2+1]);
        }
#else
#endif
        if (0 == WSTRCMP(pwstr1, pwstr2))
        {// 号码匹配成功
            if (NULL != pMatchChars)
            {
                *pMatchChars = nLen2;
            }
            
            eRet = NUMBERMATCH_WSTR2_ISTAIL_OF_WSTR1;
        }
    }
    else
    {
        pwstr1 = (AECHAR *)wstrNum1cmpbuff;
        pwstr2 = (AECHAR *)&wstrNum2cmpbuff[nLen2-nLen1];
#if defined FEATURE_CARRIER_MAROC_WANA
         if ((WSTRNCMP(wstrNum2cmpbuff, L"00212", 5) == 0)&&(WSTRNCMP(wstrNum1cmpbuff, L"0", 1) == 0))
         {
             pwstr1 = (AECHAR *)&wstrNum1cmpbuff[1];
             pwstr2 = (AECHAR *)&(wstrNum2cmpbuff[nLen2-nLen1+1]);
         }
#elif defined FEATURE_CARRIER_CHINA_TELCOM
         if ((WSTRNCMP(wstrNum2cmpbuff, L"00", 2) == 0)&&(WSTRNCMP(wstrNum1cmpbuff, L"+", 1) == 0))
         {
             pwstr1 = (AECHAR *)&wstrNum1cmpbuff[1];
             pwstr2 = (AECHAR *)&(wstrNum2cmpbuff[nLen2-nLen1+1]);
         }
#elif ((defined(FEATURE_CARRIER_THAILAND_HUTCH))&&(!(defined(FEATURE_CARRIER_THAILAND_CAT_FACE))))
         if ((WSTRNCMP(wstrNum2cmpbuff, L"001", 3) == 0)&&(WSTRNCMP(wstrNum1cmpbuff, L"+", 1) == 0))
         {
             pwstr1 = (AECHAR *)&wstrNum1cmpbuff[1];
             pwstr2 = (AECHAR *)&(wstrNum2cmpbuff[nLen2-nLen1+1]);
         }
#else
#endif
        if (0 == WSTRCMP(pwstr1, pwstr2))
        {// 号码匹配成功
            if (NULL != pMatchChars)
            {
                *pMatchChars = nLen1;
            }
            
            eRet = NUMBERMATCH_WSTR1_ISTAIL_OF_WSTR2;
        }
    }
    
    return eRet;
} // ContApp_NumberMatch

/*==============================================================================
函数:
    ContApp_RegionMatch

说明:
    电话号码匹配规则函数。

参数:
    wstrNum1 [in]: 号码 1
    wstrNum2 [in]: 号码 2
       
返回值:
    TRUE : 两个号码匹配
    FALSE: 两个号码不匹配

备注:
    
       
==============================================================================*/
boolean ContApp_RegionMatch(const AECHAR * wstrNum1, const AECHAR * wstrNum2)
{
    int nLen1, nLen2;
    AECHAR *pwstr1 = NULL;
    AECHAR *pwstr2 = NULL;
#ifdef FEATURE_CARRIER_TAIWAN_APBW
    char strNum1[MAX_INPUT_NUM + 1];
    char strNum2[MAX_INPUT_NUM + 1];
#endif

    if ((NULL == wstrNum1) || (NULL == wstrNum2))
    {
        return FALSE;
    }
    
    nLen1 = WSTRLEN(wstrNum1);
    nLen2 = WSTRLEN(wstrNum2);
    
#ifdef FEATURE_CARRIER_CHINA_COSUN
        if(nLen2 != nLen1)
        {
            return FALSE;
        }
#endif

    if (((nLen1 < MATCH_DIGITS_MIN) || (nLen2 < MATCH_DIGITS_MIN)) &&
        (nLen1 != nLen2))
    {// 比较号码长度不等，要求两者长度均不低于设定比较门限长度
        return FALSE;
    }
        
    if((nLen2 < nLen1)||((nLen2 == nLen1)&&(nLen2 > MATCH_DIGITS_MIN)))
    {
        pwstr1 = (AECHAR *)wstrNum2;
        pwstr2 = (AECHAR *)&(wstrNum1[nLen1-nLen2]);
#ifdef FEATURE_CARRIER_TAIWAN_APBW
        (void)WSTRTOSTR( wstrNum1, strNum1, sizeof(nLen1));
        (void)WSTRTOSTR( wstrNum2, strNum2, sizeof(nLen2));

        if ((STRNCMP(strNum1, "886", 3) == 0)&&(STRNCMP(strNum2, "0", 1) == 0))
        {
            pwstr1 = (AECHAR *)&wstrNum2[1];
            pwstr2 = (AECHAR *)&(wstrNum1[nLen1-nLen2+1]);
        }
#endif
        if (0 == WSTRCMP(pwstr1, pwstr2))
        {// 号码匹配成功
            return TRUE;
        }
    }
    
    return FALSE;
} // ContApp_RegionMatch



/*=============================================================================

FUNCTION: CContApp_Create500Cont

DESCRIPTION: 
    添加新记录
    
PARAMETERS:
    pMe    :
    pName  :
    pNumber:
    bCard  :
    
RETURN VALUE:
    int: SUCCESS if add ,otherwise EFAILED.
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
    在调用此函数之前应该检查是否重名（话机）或记录重复（RUIM）
    
=============================================================================*/
int CContApp_Create500Cont( CContApp *pMe)
{
    IAddrRec       *pAddrRec = NULL;
    AEEAddrField    addrFld[11];
    int index = 0;
    int count = 0;
    AECHAR str[20];
    AECHAR Name[10] = {'a',0};
    AECHAR Mobile[32] = {'1','3','3','1','2','9','1','3',0};
    AECHAR Home[32] = {'2','8','0','6','8',0};
    AECHAR Office[32] = {'3','3','9','5','6',0};
    AECHAR Fax[32] = {'3','3','9','5','5',0};
    
    ASSERT(pMe != NULL);


    for (count = 1; count <=500; count++)
    {
        Name[1] = 0;
        Mobile[8] = 0;
        Home[5] = 0;
        Office[5] = 0;
        Fax[5] = 0;

        WWRITELONG(str, count);
        pMe->m_pAddNewName = WSTRCAT(Name, str);
        pMe->m_pAddNewMobile = WSTRCAT(Mobile, str);
        pMe->m_pAddNewHome = WSTRCAT(Home, str);
        pMe->m_pAddNewOffice = WSTRCAT(Office, str);
        pMe->m_pAddNewFax = WSTRCAT(Fax, str);
        
        // Set the name field
        if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_NAME,
                                               pMe->m_pAddNewName,
                                               &addrFld[0],
                                               FALSE))
        {
            return EFAILED;
        }


		CContApp_FixPlusNumber(pMe);	//Add By zzg 2011_12_15
		
        // Set the number field
        if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_PHONE_GENERIC,
                                               pMe->m_pAddNewMobile,
                                               &addrFld[1],
                                               FALSE))
        {
            return EFAILED;
        }
		
        index = 1;
         // Set the home number field
        if (NULL !=pMe->m_pAddNewHome)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_PHONE_HOME,
                                                   pMe->m_pAddNewHome,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }

         // Set the office number field
        if (NULL !=pMe->m_pAddNewOffice)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_PHONE_WORK,
                                                   pMe->m_pAddNewOffice,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }

         // Set the Fax number field
        if (NULL !=pMe->m_pAddNewFax)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_PHONE_FAX,
                                                   pMe->m_pAddNewFax,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }

         // Set the EMAIL field
        if (NULL !=pMe->m_pAddNewEMAIL)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_EMAIL,
                                                   pMe->m_pAddNewEMAIL,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }

         // Set the Address field
        if (NULL !=pMe->m_pAddNewAddress)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_ADDRESS,
                                                   pMe->m_pAddNewAddress,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }
            
         // Set the URL field
        if (NULL !=pMe->m_pAddNewURL)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_URL,
                                                   pMe->m_pAddNewURL,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }

         // Set the Remark field
        if (NULL !=pMe->m_pAddNewRemark)
        {
            if(SUCCESS != CContApp_BuildAddrField( AEE_ADDRFIELD_NOTES,
                                                   pMe->m_pAddNewRemark,
                                                   &addrFld[++index],
                                                   FALSE))
            {
                return EFAILED;
            }
        }
        
        pAddrRec = IADDRBOOK_CreateRec( pMe->m_pAddrPhone,
                                        AEE_ADDR_CAT_PERSONAL,
                                        addrFld,
                                        --index);

        if(NULL == pAddrRec)
        {
            return EFAILED;
        }    
        
        IADDRREC_Release(pAddrRec);    
        
    }
    return SUCCESS;
}

boolean CContApp_MarkAll(CContApp * pMe)
{
    sSelectFieldInfo curSelectFieldInfo;
    uint32   nContIdx;
    int i;
    uint16 wRecID;
    AEEAddCacheInfo   *pCache;
    int listNum;
    //boolean  bCard;
    //AECHAR    *pName;
    //AEEAddrCat nCat;
    boolean rnt = TRUE;
    
    pMe->m_eTotalSelectNum = 0;
    MEMSET(&curSelectFieldInfo, 0, sizeof(curSelectFieldInfo)); 
    CContApp_FreeSelectFieldListNode();
    
    // Adjust the current index
    if(pMe->m_nCurrIdx < 0)
    {
        // 倒数 Index
        pMe->m_nCurrIdx += (int)IVector_Size(pMe->m_pAddList);
    }
    else if(pMe->m_nCurrIdx >= (int)IVector_Size(pMe->m_pAddList))
    {
        pMe->m_nCurrIdx -= (int)IVector_Size(pMe->m_pAddList);
    }//else Remain the current value
    
    pMe->m_nEnumIdx = pMe->m_nCurrIdx;

    // 智能查找时的markall
    if(/*pMe->m_pSmartBufLen <= pMe->m_nleftNum && */pMe->m_pSmartBufLen != 0)
    {
        listNum = pMe->m_pSmartBufLen;
    }
    else
    {
        listNum = (int)IVector_Size(pMe->m_pAddList);
    }
    
    for(i=0; i<listNum; i++)
    {
        nContIdx = CContApp_EnumContNext(pMe);
        
        FARF(ADDR, ("Cont idx %d curr idx %d",nContIdx, pMe->m_nCurrIdx));
        
        //if(!bCard)
        //{
            // Read data from the phone
            //pCache = IVector_ElementAt(pMe->m_pAddList, nContIdx);
            if(/*pMe->m_pSmartBufLen <= pMe->m_nleftNum && */pMe->m_pSmartBufLen != 0)
            {
                pCache = (AEEAddCacheInfo *)IVector_ElementAt(pMe->m_pAddList, pMe->m_pSmartFindBuf[i]);
            }
            else
            {
                pCache = (AEEAddCacheInfo *)IVector_ElementAt(pMe->m_pAddList, (uint32)i);
            }
            wRecID = CContApp_RawToContID(pMe, pCache->wRecID, (pCache->store == ADDR_STORE_RUIM));
            // Extract the cache  获取记录的名字
            //wRecID = IADDRBOOK_ExtractCache(pMe->m_pAddrPhone, pCache, &pName, &nCat);
            
            //wRecID = CContApp_RawToContID(pMe, wRecID, FALSE);
            CContApp_LoadAddrFlds(pMe, wRecID, SELECT_ALL);

            if(wRecID == CONTAPP_RECID_NULL)
            {
                FARF(ADDR, ("Phone Record id Out of range %d", wRecID));
                return EFAILED;
            }
            curSelectFieldInfo.wIndex = wRecID;
            pMe->m_wSelectCont = wRecID;
            curSelectFieldInfo.wFieldMask = CContApp_GetPrimaryNumFld(pMe);
            pMe->m_eTotalSelectNum++;
            rnt = CContApp_AddSelectFieldListNode(pSelectFieldListRoot, &curSelectFieldInfo);
            //(void)CContApp_SelectPhoneItem(pMe, pMenuCtl, pCache);
    }
    return rnt;
}

void CContApp_FreeBuffer(CContApp * pMe)
{
    FREEIF(pMe->m_pAddNewName);
    FREEIF(pMe->m_pAddNewMobile);
    FREEIF(pMe->m_pAddNewHome);
    FREEIF(pMe->m_pAddNewOffice);
    FREEIF(pMe->m_pAddNewFax);
    FREEIF(pMe->m_pAddNewEMAIL);
    FREEIF(pMe->m_pAddNewAddress);
    FREEIF(pMe->m_pAddNewURL);
    FREEIF(pMe->m_pAddNewRemark);
    FREEIF(pMe->m_nRingToneID);

}

/*=============================================================================

FUNCTION:CContApp_DeleteMultipe_CB

DESCRIPTION: 
    将当前话机的记录全部delete调用的Callback

PARAMETERS:
    pUser :
    
RETURN VALUE:
    NONE
    
COMMENTS:
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
void CContApp_DeleteMultipe_CB(void *pUser)
{
    CContApp     *pMe = (CContApp *)pUser;
    uint16        deleteNum;
    int             wCurrNum;
    //AEEAddrField *pNumFld = NULL;
    //sSelectFieldListNode* pNode = NULL;
    
    ASSERT(pMe != NULL);
    
    if(FALSE == pMe->m_bDeleteInited)
    {
        // not initilize
        return;
    }
    
    if(pMe->m_bForceComplete)
    {
        // 强制退出
        pMe->m_bIsNotComplete = TRUE;
        return;
    }
    
    wCurrNum = MIN(STEP_COPY, (pMe->m_wTotalNum - pMe->m_wDeleteCurrID));
    
    FARF(ADDR, ("Current num %d all %d", wCurrNum, pMe->m_wTotalNum));

    for(deleteNum = 0; deleteNum < wCurrNum; deleteNum++)
    {        
        if (NULL != pCurSelectFieldNode)
        {
            pMe->m_wDeleteRecCnt++;
            CContApp_DeleteCont(pMe, pCurSelectFieldNode->SelectFieldInfo.wIndex);
        }
        else
        {
            break;
        }

        //pNode = pCurSelectFieldNode;
        pCurSelectFieldNode = pCurSelectFieldNode->pNextNode;
        /*if(NULL != pNode)
        {
            FREE(pNode);
            pNode = NULL;
        }*/

    }
    
    // 如果此次拷贝的数目已经小于step的值，则证明已经复制完成了
    if(deleteNum < wCurrNum)
    {
        (void)ISHELL_PostEvent( pMe->m_pShell,
                                AEECLSID_APP_CONTACT,
                                EVT_USER_ERR,
                                0,
                                0);
        return;
    }
    else if(deleteNum < STEP_COPY)
    {
        pMe->m_bDeleteInited = FALSE;
        
        // Have complete copy, Notify the applet
        (void)ISHELL_PostEvent( pMe->m_pShell,
                                AEECLSID_APP_CONTACT,
                                EVT_USER_COMPLETE,
                                0,
                                0);
        return;
    }
    else
    {
        // Update the information 
        /*(void)ISHELL_PostEvent( pMe->m_pShell,
                                AEECLSID_APP_CONTACT,
                                EVT_USER_REDRAW,
                                0,
                                0);*/
    }
    
    pMe->m_wDeleteCurrID += STEP_COPY;
    CONTAPP_RESUMECB(&pMe->m_nDeleteCB);
}// CContApp_DeleteMultipe_CB


/*=============================================================================

FUNCTION:CContApp_CopyMultipe_CB

DESCRIPTION: 
    将当前话机的记录全部复制到 RUIM 卡时调用的Callback

PARAMETERS:
    pUser :
    
RETURN VALUE:
    NONE
    
COMMENTS:
    
SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
void CContApp_CopyMultipe_CB(void *pUser)
{
    CContApp     *pMe = (CContApp *)pUser;
    uint16        i;
    uint16        wCurrNum;
    //sSelectFieldListNode* pNode = NULL;
    
    ASSERT(pMe != NULL);
    
    if(FALSE == pMe->m_bCopyInited)
    {
        // not initilize
        return;
    }
    
    if(pMe->m_bForceComplete)
    {
        // 强制退出
        pMe->m_bIsNotComplete = TRUE;
        return;
    }
    
    wCurrNum = MIN(STEP_COPY, (pMe->m_wTotalNum - pMe->m_wCopyCurrID));
    
    FARF(ADDR, ("Current num %d all %d  %d ", wCurrNum, pMe->m_wTotalNum, pMe->m_nCopyMoveType));
    
    for(i = 0; i< wCurrNum; i++)
    {
        if (NULL != pCurSelectFieldNode)
        {
            if( (COPYMULTIPE_TOUIM == pMe->m_nCopyMoveType || MOVEMULTIPE_TOUIM == pMe->m_nCopyMoveType)
                && !IS_RUIM_REC(pCurSelectFieldNode->SelectFieldInfo.wIndex) )
            {
                if(IS_MAX_REC(pMe->m_pAddrRUIM))
                {
                    // 如果达到最大容量结束复制
                    pMe->m_eErrType = UIMCARD_FULL;
                    break;
                }
                
                if(!CContApp_CheckNumLen(pMe, pCurSelectFieldNode->SelectFieldInfo.wIndex))
                {
                    pMe->m_nFailNum++;
                }
                else
                {
                    CContApp_CopyToRUIM(pMe, pCurSelectFieldNode->SelectFieldInfo.wIndex);
                    
                    if(MOVEMULTIPE_TOUIM == pMe->m_nCopyMoveType)
                    {
                        CContApp_DeleteCont(pMe, pCurSelectFieldNode->SelectFieldInfo.wIndex);
                    }
                }

            }
            else if( (COPYMULTIPE_TOPHONE == pMe->m_nCopyMoveType || MOVEMULTIPE_TOPHONE == pMe->m_nCopyMoveType)
                && IS_RUIM_REC(pCurSelectFieldNode->SelectFieldInfo.wIndex) )
            {
                if(IS_MAX_REC(pMe->m_pAddrPhone))
                {
                    // 如果达到最大容量结束复制
                    pMe->m_eErrType = PHONE_FULL;
                    break;
                }
                CContApp_CopyToPhone(pMe, pCurSelectFieldNode->SelectFieldInfo.wIndex - CONTAPP_RUIM_START_IDX + 1);

                if(MOVEMULTIPE_TOPHONE == pMe->m_nCopyMoveType)
                {
                    CContApp_DeleteCont(pMe, pCurSelectFieldNode->SelectFieldInfo.wIndex);
                }
            }
            else
            {
                pMe->m_eErrType = OTHER_ERR;
                break;
            }
            //pMe->m_wCopyRecCnt++;////save total num have copied
        }
        else
        {
            break;
        }
        pCurSelectFieldNode = pCurSelectFieldNode->pNextNode;
        /*if(NULL != pNode)
        {
            FREE(pNode);
            pNode = NULL;
        }*/

    }
    
    if(OTHER_ERR == pMe->m_eErrType)
    {
        (void)ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER_ERR,0,0);
        return ;
    }
    else if(PHONE_FULL == pMe->m_eErrType || UIMCARD_FULL == pMe->m_eErrType)
    {
        //pMe->m_wCopyRecCnt += i;//save total num have copied
        (void)ISHELL_PostEvent( pMe->m_pShell,AEECLSID_APP_CONTACT,EVT_USER_MAX,0,0);
        return ;
    }
    
    if(wCurrNum < STEP_COPY)
    {
        // 如果此次拷贝的数目已经小于step的值，则证明已经复制完成了
        pMe->m_bCopyInited = FALSE;

        //pMe->m_wCopyRecCnt += wCurrNum;////save total num have copied
        // Have complete copy, Notify the applet
        (void)ISHELL_PostEvent( pMe->m_pShell,
                                AEECLSID_APP_CONTACT,
                                EVT_USER_COMPLETE,
                                0,
                                0);
        return;
    }
    else
    {
        // Update the information 
        /*(void)ISHELL_PostEvent( pMe->m_pShell,
                                AEECLSID_APP_CONTACT,
                                EVT_USER_REDRAW,
                                0,
                                0);
                                */
    }
    
    pMe->m_wCopyCurrID += STEP_COPY;
    CONTAPP_RESUMECB(&pMe->m_nCopyCB);
}// CContApp_CopyMultipe_CB

void CContApp_SaveOneDial(CContApp * pMe)
{
    //遍历数组，修改单键拨号
    byte numFldIndx; 
    AECHAR    *tmp = NULL;

    if(pMe == NULL)
        return ;
    
    for(numFldIndx = 0; numFldIndx<CONTAPP_NUMBERFLDS; numFldIndx++)
    {
        if(TRUE == pMe->m_eOneDialFld[numFldIndx].bMask)
        {
            pMe->m_eOneDialFld[numFldIndx].bMask = FALSE;
            switch(pMe->m_eOneDialFld[numFldIndx].fldindx)
            {
                 case AEE_ADDRFIELD_PHONE_GENERIC:
                     tmp = pMe->m_pAddNewMobile;
                     break;
                     
                 case AEE_ADDRFIELD_PHONE_HOME:
                     tmp = pMe->m_pAddNewHome;
                     break;
  
                 case AEE_ADDRFIELD_PHONE_WORK:
                     tmp = pMe->m_pAddNewOffice;
                     break;
                     
                 case AEE_ADDRFIELD_PHONE_FAX:
                     tmp = pMe->m_pAddNewFax;
                     break;
                     
                 default:
                    break;
            }
			
            (void)CContApp_SetConfig( pMe,
                              (ContAppCFG)pMe->m_eOneDialFld[numFldIndx].oneDialIndx,
                              tmp,
                              sizeof(tmp));
        }
        else
        {
            break;
        }
    }
}

//获取返回状态
FSMState CContApp_GetReturnState(CContApp * pMe)
{
    FSMState rnt;

    if(pMe == NULL)
        return STATE_ERROR;
    
    CContApp_GetConfig(pMe, CONTCFG_SAVETYPE, &pMe->m_nSaveType, sizeof(pMe->m_nSaveType));
    switch(pMe->m_eStartMethod)
    {
        case STARTMETHOD_ADDFIELD: 
        {
            rnt = STATE_EXIT;
        }
        break;
        
        case STARTMETHOD_SAVEFIELD:
        {
            rnt = STATE_EXIT;
        }
        break;
        
        // idle 启动
        case STARTMETHOD_NORMAL:
        {
            // 明确位置的add, 或者edit
            rnt = STATE_MAINLIST;
        }
        break;

        // mainmenu 启动
        case STARTMETHOD_MAINMENU:
        {

            // 明确位置的add, 或者edit
            if(LISTFROMSEARCH == pMe->m_eListFrom)
            {
                // 从 search 结果列表的 opt 中edit, add
                rnt = STATE_FIND_LIST;
            }
            else if(LISTFROMGROUP == pMe->m_eListFrom)
            {
                // 从 groupView 结果列表的 opt 中edit, add
                rnt = STATE_GROUPVIEW_LIST;
            }
            else if(LISTFROMMAINMENU == pMe->m_eListFrom)
            {
                // 从 列表的 opt 中edit , add
                rnt = STATE_MAINLIST;
            }
            else if(LISTFROMNONE == pMe->m_eListFrom)
            {
                // 从 列表的 opt 中edit , add
                rnt = STATE_MAINMENU;
            }
            else
            {
                //错误
                pMe->m_wErrStrID = IDS_RETURN_ERR;
                rnt = STATE_ERROR;
            }                            
        }
        break;
        
        default: 
        {
            pMe->m_wErrStrID = IDS_RETURN_ERR;
            rnt = STATE_ERROR;
        }
        break;
        
    }    
    
    //MOVE_TO_STATE(rnt);

    return rnt;
}

/* 判断选择的项是否设置了onedial */
boolean CContApp_OnedialExit(CContApp * pMe)
{
    ContAppCFGCache *pCFGCache;
    boolean rnt = FALSE;
    
    if(NULL == pMe)
    {
        return FALSE;
    }   
            
    pCFGCache = &pMe->m_sCFGCache;  

    switch(pMe->m_wSelectOneDial)
    {
        case CONTCFG_ONEDIAL1:
            if(WSTRSIZE(pCFGCache->wOneDial1)) 
            {
                rnt = TRUE;
            }
            break;
            
        case CONTCFG_ONEDIAL2:
            if(WSTRSIZE(pCFGCache->wOneDial2)) 
            {
                rnt = TRUE;
            }
            break;
        
        case CONTCFG_ONEDIAL3:
            if(WSTRSIZE(pCFGCache->wOneDial3)) 
            {
                rnt = TRUE;
            }
            break;
        
        case CONTCFG_ONEDIAL4:
            if(WSTRSIZE(pCFGCache->wOneDial4)) 
            {
                rnt = TRUE;
            }
            break;
        
        case CONTCFG_ONEDIAL5:
            if(WSTRSIZE(pCFGCache->wOneDial5)) 
            {
                rnt = TRUE;
            }
            break;
        
        case CONTCFG_ONEDIAL6:
            if(WSTRSIZE(pCFGCache->wOneDial6)) 
            {
                rnt = TRUE;
            }
            break;
        
        case CONTCFG_ONEDIAL7:
            if(WSTRSIZE(pCFGCache->wOneDial7)) 
            {
                rnt = TRUE;
            }
            break;
        
        case CONTCFG_ONEDIAL8:
            if(WSTRSIZE(pCFGCache->wOneDial8)) 
            {
                rnt = TRUE;
            }
            break;
        
        case CONTCFG_ONEDIAL9:
            if(WSTRSIZE(pCFGCache->wOneDial9)) 
            {
                rnt = TRUE;
            }
            break;   
            
        default:
            rnt = FALSE;
            break;
    }

    return rnt;
}

boolean CContApp_CheckNumLen(CContApp * pMe, uint16 selContId)
{
#ifndef WIN32
    int CardNumLen = OEMRUIMAddr_GetRUIMMaxNumberSize();
#else
    int CardNumLen = 0;
#endif
    boolean rnt = TRUE;

    if(NULL == pMe)
    {
        return FALSE;
    }
    
    CContApp_FreeBuffer(pMe);
    CContApp_GetRecByID(pMe, selContId);    

    if(pMe->m_pAddNewMobile != NULL && WSTRLEN(pMe->m_pAddNewMobile) > CardNumLen
        || pMe->m_pAddNewHome != NULL && WSTRLEN(pMe->m_pAddNewHome) > CardNumLen
        || pMe->m_pAddNewOffice != NULL && WSTRLEN(pMe->m_pAddNewOffice) > CardNumLen
        || pMe->m_pAddNewFax != NULL && WSTRLEN(pMe->m_pAddNewFax) > CardNumLen )
    {
        rnt = FALSE;
    }

    CContApp_FreeBuffer(pMe);
    return rnt;
}


void CContApp_DrawScrollBar(CContApp * pMe, IMenuCtl   *pMenuCtl)
{
    if(pMe == NULL || pMenuCtl == NULL)
        return ;
    
    if((IVector_Size(pMe->m_pAddList) > MAX_NUM_MENUPOP && NULL == pMe->m_szAlpha) || (pMe->m_pSmartBufLen > MAX_NUM_MENUPOP && NULL != pMe->m_szAlpha) )
    {
        int currIdx = 0;
        int totalNum = 0;
        AEERect rect = {0};
        AEERect menuRect = {0};
        int recs = IVector_Size(pMe->m_pAddList);
        
        if(pMe->m_bInSmartFind)
        {
            currIdx = (MAX_NUM_MENUPOP + pMe->m_nCurrSmartIdx);
            totalNum = pMe->m_pSmartBufLen;
        }
        else
        {
            currIdx = (MAX_NUM_MENUPOP + pMe->m_nCurrIdx);
            totalNum = recs;
        }
        rect = pMe->m_rc;
        IMENUCTL_GetRect(pMenuCtl, &menuRect);
        menuRect.dx = rect.dx - SCROLLBAR_WIDTH;
        IMENUCTL_SetRect(pMenuCtl, &menuRect);
        MSG_FATAL("CContApp_DrawScrollBar",0,0,0);
        
        rect.x = rect.dx - SCROLLBAR_WIDTH;
        rect.dx = SCROLLBAR_WIDTH;
        rect.y = 0;
        rect.dy = rect.dy - 2 * BOTTOMBAR_HEIGHT;	
        pMe->m_ScrollBarrc=rect;
        
       if(pMe->m_wYPos > 0)
       {
         int pagenum = (pMe->m_wYPos-pMe->m_ScrollBarrc.y)/(pMe->m_ScrollBarrc.dy/(totalNum/MAX_NUM_MENUPOP +1));
         pMe->m_wYPos=0;
         currIdx=pagenum*MAX_NUM_MENUPOP+1;  
         MSG_FATAL("CContApp_DrawScrollBar---pagenum=%d",pagenum,0,0);
         MSG_FATAL("CContApp_DrawScrollBar---totalNum=%d----currIdx=%d---pMe->m_ScrollBarrc.dy=%d",totalNum,currIdx,pMe->m_ScrollBarrc.dy);
         Appscommon_DrawScrollBar(pMe->m_pDisplay, currIdx, totalNum, MAX_NUM_MENUPOP, &pMe->m_ScrollBarrc);
       }
       else
       {
         Appscommon_DrawScrollBar(pMe->m_pDisplay, currIdx, totalNum, MAX_NUM_MENUPOP, &rect);
       }
        //IDISPLAY_Update(pMe->m_pDisplay);  // 在这里update会和menu的redraw中的update不同步
    }

}

void CContApp_RestoreMenuRect(CContApp * pMe, IMenuCtl   *pMenuCtl)
{
    
    AEERect rect = {0};
    AEERect menuRect = {0};

    if(pMe == NULL || pMenuCtl == NULL)
        return ;
    
    rect = pMe->m_rc;
    IMENUCTL_GetRect(pMenuCtl, &menuRect);
    menuRect.dx = rect.dx ;
    IMENUCTL_SetRect(pMenuCtl, &menuRect);
    MSG_FATAL("CContApp_RestoreMenuRect",0,0,0);
}

// currItemIdx 是当前菜单选中的项，最大MAX_NUM_MENUPOP
uint16 CContApp_GetListReturnSel(CContApp * pMe, IMenuCtl   *pMenuCtl, int * currItemIdx)
{
    int itemIdx = 0;
    uint16 nextRecsId = 0;
    uint16 currRecsId = IMENUCTL_GetItemID(pMenuCtl, 0);
    int itemCounts = IMENUCTL_GetItemCount(pMenuCtl);
    uint16 rnt = MENU_SELECT_NULL;
    int recs = 0;
    int currIdx = 0;
    
    if(pMe == NULL || pMenuCtl == NULL)
        return MENU_SELECT_NULL;

    for(; itemIdx < itemCounts; itemIdx++)
    {
        //currRecsId = IMENUCTL_GetItemID(pMenuCtl, itemIdx);
        nextRecsId = IMENUCTL_GetItemID(pMenuCtl, itemIdx + 1);
        if(pMe->m_wEditCont == currRecsId)
        {
            *currItemIdx = itemIdx + 1;
            rnt = nextRecsId;
            break;
        }
        else
        {
            currRecsId = nextRecsId;
        }
    }

    if(NULL != pMe->m_szAlpha)
    {
        recs = pMe->m_pSmartBufLen;
        currIdx = pMe->m_nCurrSmartIdx;
    }
    else
    {
        recs = IVector_Size(pMe->m_pAddList);
        currIdx = pMe->m_nCurrIdx;
    }
    
    // 是否是最后一条
    if(*currItemIdx == MAX_NUM_MENUPOP && (currIdx + MAX_NUM_MENUPOP) == recs)
    {
        // 返回倒数第二条
        rnt = IMENUCTL_GetItemID(pMenuCtl, MAX_NUM_MENUPOP-2);
    }

    return rnt;
}


boolean CContApp_IsPopState(CContApp * pMe)
{
    switch(pMe->m_eCurState)
    {
        case STATE_SELECTOPT:
        /*case STATE_POPNUMFLD:*/
        case STATE_GROUPOPT:
        case STATE_FINDNUM_OPTS:
        case STATE_ONEDIAL_OPT:
        case STATE_GROUPVIEW_OPTS:
        case STATE_FIND_OPTS:
        case STATE_OPTS:
        {
            return TRUE;
        }

        default:
            break;
    }
    
    return FALSE;
}

#if defined(FEATURE_WMS_APP)
/*=============================================================================

FUNCTION:CContApp_GetSendListLeftNodeNum

DESCRIPTION: 
     获取发送列表还有多少可以填充

PARAMETERS:
    pMe:
    leftNum:

RETURN VALUE:
    SUCCESS
   
COMMENTS:

SIDE EFFECTS:
SEE ALSO:
=============================================================================*/
int CContApp_GetSendListLeftNodeNum(CContApp *pMe, int *leftNum )
{
    IWmsApp *pSMSApp;

    ASSERT(pMe != NULL);
    
    if (leftNum == NULL)
    {
        return EFAILED;
    }
    
    if (SUCCESS != ISHELL_CreateInstance( pMe->m_pShell,
                                          AEECLSID_WMSAPP,
                                          (void**)&pSMSApp))
    {
        return EFAILED;
    }

    if (SUCCESS != IWmsApp_GetLeftSendListNodeNum(pSMSApp, leftNum))
    {
        (void)IWmsApp_Release(pSMSApp);
        pSMSApp = NULL;
        return EFAILED;
    }    
    
    (void)IWmsApp_Release(pSMSApp);
    pSMSApp = NULL;
    
    return SUCCESS;
}// CContApp_SendSMS
#endif

int CContApp_GetSelNodeMarkNum(dword  mark)
{
    int rnt = 0;
    
    if( mark & MASK_ADDRFIELD_PHONE_GENERIC)
    {
        rnt++;
    }

    if( mark & MASK_ADDRFIELD_PHONE_HOME)
    {
        rnt++;
    }

    if( mark & MASK_ADDRFIELD_PHONE_WORK)
    {
        rnt++;
    }

    if( mark & MASK_ADDRFIELD_PHONE_FAX)
    {
        rnt++;
    }

    return rnt;
}

boolean CContApp_IsSelField(CContApp *pMe, dword mask)
{
    AEEAddrField *pFld = {0};
    boolean rnt;
    
    ASSERT(pMe);

    pFld = IVector_ElementAt( pMe->m_pFldIv, (uint32)pMe->m_wSelectFldOpts );

    switch(pFld->fID)
    {
        //phone
        case AEE_ADDRFIELD_PHONE_GENERIC :
            rnt = mask & MASK_ADDRFIELD_PHONE_GENERIC;
            break;
            
        //home number
        case AEE_ADDRFIELD_PHONE_HOME :
            rnt = mask & MASK_ADDRFIELD_PHONE_HOME;
            break;

        //work number
        case AEE_ADDRFIELD_PHONE_WORK :
            rnt = mask & MASK_ADDRFIELD_PHONE_WORK;
            break;

        //fax number
        case AEE_ADDRFIELD_PHONE_FAX :
            rnt = mask & MASK_ADDRFIELD_PHONE_FAX;
            break;
            
        default :
            rnt = FALSE;
    }

    return rnt;
}

#ifdef FEATURE_LANG_CHINESE
/*=============================================================================

FUNCTION: CContApp_CNWStr2Letter

DESCRIPTION: 
    将字符串转换为对应的按键字符串
    
PARAMETERS:
    pDest:
    nLen : 目的字符串长度 in AECHAR
    pSrc :
    
RETURN VALUE:
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
void CContApp_CNWStr2Letter(AECHAR *pDest, int nLen , AECHAR *pSrc, boolean Is2Number)
{
    int i;
    
    if(!pDest || !pSrc || nLen == 0)
    {
        return;
    }
    
    for(i=0; i<nLen; i++)
    {
        if(*pSrc == NULL)
        {
            break;
        }
        else
        {
            pDest[i] = CNUnicode2Letter(pSrc[i]);
            if(Is2Number)
            {
                pDest[i] = CNLetter2Alpha(pDest[i]);
            }
        }
    }
}// WStr2Alpha
//CNUni2Letter(UniCode) 

/*=============================================================================

FUNCTION: Letter2Alpha

DESCRIPTION: 
    将字符串转换为对应的按键字符串
    
PARAMETERS:
    pDest:
    nLen : 目的字符串长度 in AECHAR
    pSrc :
    
RETURN VALUE:
    
COMMENTS:

SIDE EFFECTS:

SEE ALSO:
=============================================================================*/
void CContApp_Letter2Alpha(AECHAR *pDest, int nLen)
{
    int i;
    
    if(!pDest || nLen == 0)
    {
        return;
    }
    
    for(i=0; i<nLen; i++)
    {
        if(*pDest == NULL)
        {
            break;
        }
        else
        {
            pDest[i] = CNLetter2Alpha(pDest[i]);
        }
    }
}// WStr2Alpha

#endif

boolean CContApp_IsNumber(AECHAR *str)
{
    int i, len;
    if(!str)
    {
        return FALSE;
    }

    len = WSTRLEN(str);
    
    for(i=0; i<= len; i++)
    {
        switch(str[i])
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
                break;
                
            case '\0':
               return TRUE;
               
            default:
               return FALSE;
        }
    }
    
    return TRUE;
}

#ifdef FEATURE_SUPPORT_VCARD
boolean ContApp_ReadVcfFile(CContApp *pMe, char* fileName, int * cnt)
{
    IFile* pIFile = NULL;
    IFileMgr *pIFileMgr = NULL;
    FileInfo pInfo = {0};
    char    *pdatabuf = NULL;
    //char    fullName[128] = {0};

    if (AEE_SUCCESS != ISHELL_CreateInstance(pMe->m_pShell,
                                            AEECLSID_FILEMGR,
                                            (void **)&pIFileMgr))
    {
          return FALSE;
    }

    //STRCAT(fullName, VCARD_PATH);    
    //STRCAT(fullName, fileName);
    if ( IFILEMGR_Test( pIFileMgr, fileName ) == SUCCESS )
    {
        if (IFILEMGR_GetInfo(pIFileMgr,fileName, &pInfo) == SUCCESS)
        {
            pIFile = IFILEMGR_OpenFile( pIFileMgr, fileName, 
                                      _OFM_READ );

            if ( pIFile != NULL )
            {
                pdatabuf = (char *)MALLOC(pInfo.dwSize);
                if(pdatabuf!=NULL)
                {
                    IFILE_Seek(pIFile, _SEEK_START, 0);
                    IFILE_Read( pIFile, pdatabuf, pInfo.dwSize);
                    
                    IFILE_Release( pIFile );
                    pIFile = NULL;
                    IFILEMGR_Remove(pIFileMgr, fileName);
                    IFILEMGR_Release(pIFileMgr);
                    pIFileMgr = NULL;

                    if(ParseVcfFile(pMe, pdatabuf, cnt) == VCARD_SUCCESS)
                    {
                        ERR("tanwq in ContApp_ReadVcfFile555", 0,0,0);
                        FREEIF(pdatabuf);
                        return TRUE;
                    }
                }
            }
        }
        
    }

    FREEIF(pdatabuf);
    if(NULL != pIFile)
    {
        IFILE_Release( pIFile );
        IFILEMGR_Remove(pIFileMgr, fileName);
    }

    if(NULL != pIFileMgr)
    {
        IFILEMGR_Release(pIFileMgr);
    }
    return FALSE;
}
#endif

boolean ContApp_DeleteAll(CContApp *pMe)
{
   IADDRBOOK_RemoveAllRecs(pMe->m_pAddrPhone);
/*
#ifdef FEATURE_RUIM_PHONEBOOK
   if(IsRunAsUIMVersion())
   {
       IADDRBOOK_RemoveAllRecs(pMe->m_pAddrRUIM);
   }
#endif

    IVector_RemoveAllElements(pMe->m_pAddList);
   */         
   return TRUE;
}

// 把edit保存的代码集成到这个函数里
int CContApp_SaveEditResult(CContApp* pMe)
{
    int nRet = SUCCESS;

    if(NULL ==pMe)
    {
        return EFAILED;
    }

    pMe->m_bInputNotComplete = FALSE;
    //if(pMe->m_nAddnewOrEdit == ADDOREDIT_EDIT)
    {
        CContApp_SaveOneDial(pMe);
       
        if(!IS_RUIM_REC(pMe->m_wEditCont))
        {
              FREEIF(pMe->m_pFldInputBuf);
              if(pMe->m_pAddNewHome != NULL)
              {
                  pMe->m_pFldInputBuf = WSTRDUP((AECHAR *)pMe->m_pAddNewHome);
              }
              nRet = CContApp_SaveEditField(pMe,AEE_ADDRFIELD_PHONE_HOME);

              FREEIF(pMe->m_pFldInputBuf);
              if(pMe->m_pAddNewOffice != NULL)
              {
                  pMe->m_pFldInputBuf = WSTRDUP((AECHAR *)pMe->m_pAddNewOffice);
              }
              nRet = CContApp_SaveEditField(pMe,AEE_ADDRFIELD_PHONE_WORK);

              FREEIF(pMe->m_pFldInputBuf);
              if(pMe->m_pAddNewFax != NULL)
              {
                  pMe->m_pFldInputBuf = WSTRDUP((AECHAR *)pMe->m_pAddNewFax);
              }
              nRet = CContApp_SaveEditField(pMe,AEE_ADDRFIELD_PHONE_FAX);

              FREEIF(pMe->m_pFldInputBuf);
              if(pMe->m_pAddNewEMAIL != NULL)
              {
                  pMe->m_pFldInputBuf = WSTRDUP((AECHAR *)pMe->m_pAddNewEMAIL);
              }
              nRet = CContApp_SaveEditField(pMe,AEE_ADDRFIELD_EMAIL);

              FREEIF(pMe->m_pFldInputBuf);
              if(pMe->m_pAddNewAddress != NULL)
              {
                pMe->m_pFldInputBuf = WSTRDUP((AECHAR *)pMe->m_pAddNewAddress);
              }
              nRet = CContApp_SaveEditField(pMe,AEE_ADDRFIELD_ADDRESS);

              FREEIF(pMe->m_pFldInputBuf);
              if(pMe->m_pAddNewURL != NULL)
              {
                  pMe->m_pFldInputBuf = WSTRDUP((AECHAR *)pMe->m_pAddNewURL);
              }
              nRet = CContApp_SaveEditField(pMe,AEE_ADDRFIELD_URL);

              FREEIF(pMe->m_pFldInputBuf);
              if(pMe->m_pAddNewRemark != NULL)
              {
                pMe->m_pFldInputBuf = WSTRDUP((AECHAR *)pMe->m_pAddNewRemark);
              }
              nRet = CContApp_SaveEditField(pMe,AEE_ADDRFIELD_NOTES);

              // 更新记录群组，只有话机记录可以走到这里
              if( SUCCESS != CContApp_SetCat( pMe,
                                              pMe->m_wEditCont,
                                              pMe->m_nGroupCat))
              {
                  pMe->m_wErrStrID = IDS_ERR_UPDATE;
                  MOVE_TO_STATE(STATE_ERROR);
                  return EFAILED;
              }

              //if(pMe->m_nFldInputID == AEE_ADDRFIELD_RINGTONE)
              {
                  //pMe->m_nFldInputID = AEE_ADDRFIELD_NONE;
                  nRet = CContApp_SaveRingField(pMe, pMe->m_nRingToneID);
              }
      }
      
       FREEIF(pMe->m_pFldInputBuf);
       pMe->m_pFldInputBuf = WSTRDUP((AECHAR *)pMe->m_pAddNewMobile);
       nRet = CContApp_SaveEditField(pMe,AEE_ADDRFIELD_PHONE_GENERIC);
       FREEIF(pMe->m_pFldInputBuf);
       pMe->m_pFldInputBuf = WSTRDUP((AECHAR *)pMe->m_pAddNewName);
       nRet = CContApp_SaveEditField(pMe,AEE_ADDRFIELD_NAME);

       return SUCCESS;
    }
}

int CContApp_GetGroupCat(CContApp* pMe, int16 groupId)
{
    switch(groupId)
    { 
        case IDS_GROUP_VIP:
            pMe->m_nGroupCat = AEE_ADDR_CAT_VIP;
            pMe->m_wSelectGroup = CONTCFG_GROUP1;
            break;
            
        case IDS_GROUP_HOME:
            pMe->m_nGroupCat = AEE_ADDR_CAT_HOME;
            pMe->m_wSelectGroup = CONTCFG_GROUP2;
            break;
        
        case IDS_GROUP_WORK:
            pMe->m_nGroupCat = AEE_ADDR_CAT_WORK;
            pMe->m_wSelectGroup = CONTCFG_GROUP3;
            break;
        
        case IDS_GROUP_FRIEND:
            pMe->m_nGroupCat = AEE_ADDR_CAT_FRIEND;
            pMe->m_wSelectGroup = CONTCFG_GROUP4;
            break;
        
        case IDS_GROUP_PERSONAL:
            pMe->m_nGroupCat = AEE_ADDR_CAT_PERSONAL;
            pMe->m_wSelectGroup = CONTCFG_GROUP5;
            break;
            
        case IDS_GROUP_BUSINESS:
            pMe->m_nGroupCat = AEE_ADDR_CAT_BUSINESS;
            pMe->m_wSelectGroup = CONTCFG_GROUP6;
            break;
            
        case IDS_GROUP_NONE_STRING:
            pMe->m_nGroupCat = AEE_ADDR_CAT_OTHER;
            pMe->m_wSelectGroup = CONTCFG_GROUP7;
            break;
                
        case IDS_GROUP_UIM:
            pMe->m_nGroupCat = AEE_ADDR_CAT_UIM;
            pMe->m_wSelectGroup = 0;
            break;   
            
        default:
            pMe->m_nGroupCat = AEE_ADDR_CAT_NONE;
            pMe->m_wSelectGroup = 0;
            break;
    }

    return SUCCESS;
}

/*==============================================================================

函数: CContApp_GetRingtone
       
说明: 
    获取相应电话(非RUIM卡)记录的ringtone
       
参数: 
    pAddr  : 
    ringName   [out]: ringName
    wNumLen   [in]: 记录ID
    
返回值:
    None
       
备注:
       
==============================================================================*/
int CContApp_GetRingtone( IAddrBook   *pAddr,
                               AECHAR     *ringName,
                               uint16      recId)
{
    int           i;
    int           nFldsNum;
    AEEAddrField *pFld;
    IAddrRec   *pAddrRec;

    if(!pAddr || !ringName)
    {
        return EFAILED;
    }
    
    pAddrRec = IADDRBOOK_GetRecByID(pAddr, recId);
    
    nFldsNum = IADDRREC_GetFieldCount(pAddrRec);
    for(i=0; i<nFldsNum; i++)
    {
        pFld = IADDRREC_GetField( pAddrRec, i );
        if(NULL == pFld)
        {
            continue;
        }

        if(pFld->fID == AEE_ADDRFIELD_RINGTONE)
        {
            WSTRCPY(ringName, (AECHAR*)pFld->pBuffer);
            return SUCCESS;
        }

    }

    ringName[0] = (AECHAR)'\0';
    return EFAILED;
}// CContApp_GetRingtone

/*=============================================================================
FUNCTION:  ContApp_ChangePhoneNumberForNameCompare
 
DESCRIPTION: Ensures that the specified number contains valid digits
 
PARAMETERS:
   *num: number to check
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void ContApp_ChangePhoneNumberForNameCompare(AECHAR *num)
{
    if ((NULL != num) && (WSTRLEN(num) < MAX_INPUT_NUM))
    {
        while (*num != '\0') 
        {
            if (((*num < (AECHAR)'0')&&(*num != (AECHAR)'+')\
                &&(*num != (AECHAR)'*')&&(*num != (AECHAR)'#'))||(*num > (AECHAR)'9'))
            {
                *num = '\0';
                return;
            }
            num++;
        }
    }
}

//Add By zzg 2011_12_15
void CContApp_FixPlusNumber(CContApp *pMe)
{	
	char tempstr[50];
	AECHAR  buff[50];
	WSTRTOSTR(pMe->m_pAddNewMobile, tempstr, sizeof(tempstr));
	DBGPRINTF("***zzg CContApp_FixPlusNumber m_pAddNewMobile = %s***", tempstr);

	if (WSTRNCMP(pMe->m_pAddNewMobile, L"+", 1) == 0) 
    {
    	WSTRCPY(buff, L"00");	
		WSTRCAT(buff, pMe->m_pAddNewMobile+1);
		WSTRCPY(pMe->m_pAddNewMobile, buff);
    }	        
		
}

void CContApp_SetPlusNumber(AECHAR *pBuffer)
{	
	char tempstr[50];
	AECHAR  buff[50];
	WSTRTOSTR(pBuffer, tempstr, sizeof(tempstr));
	DBGPRINTF("***zzg CContApp_SetPlusNumber m_pAddNewMobile = %s***", tempstr);

	if (WSTRNCMP(pBuffer, L"00", 2) == 0) 
	{
		WSTRCPY(buff, L"+");	
		WSTRCAT(buff, pBuffer+2);
		WSTRCPY(pBuffer, buff);
	}	              
		
}

//Add End


