/******************************************************************************
FILE:  Common.c

SERVICES:

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

       Copyright © 2002 ???? Incorporated.  All Rights Reserved. ??? Proprietary/GTDR
******************************************************************************/
#include "Common.h"

/*===========================================================================
This function sets the menu attributes based on BREW Style Sheet for
menu control.
//set option of menu
===========================================================================*/
void Brew_SetMenuAttr(IMenuCtl * pMenu, AEECLSID clsMenu, AEERect * pRect, uint32 dwProps)
{
#ifdef AEE_STATIC
    if(pMenu == NULL) return;
    
    IMENUCTL_SetProperties(pMenu, MP_UNDERLINE_TITLE);
    if (pRect)
    {
        IMENUCTL_SetRect(pMenu, pRect);
    }
    
#else    
    
   AEEItemStyle      sel, normal;
   AEEMenuColors     col;

   if(pMenu == NULL) return;

   // Menu Style
   normal.ft = MENU8_FT;
   normal.xOffset = 0;
   normal.yOffset = 0;
   normal.roImage = MENU8_RO;

   sel.ft = MENU8_SELECT_FT;
   sel.xOffset = 0;
   sel.yOffset = 0;
   sel.roImage = MENU8_SELECT_RO;

   // Menu Colors
   col.cSelText =    MENU8_SELECT_TEXT;
   col.cText    =    MENU8_TITLE;
   col.wMask =       MENU8_COLOR_MASK;
   col.cFrame =      RGB_WHITE;   
   col.cScrollbar =  MENU_SCROOL_BAR;
   col.cScrollbarFill = MENU_BAR_FILLCOLOR;


   if (clsMenu == AEECLSID_MENUCTL)
   {
      col.cBack = MENU8_BACKGROUND;      
      col.cSelBack = MENU8_SELECT_BACKGROUND;            

      dwProps |= IMENUCTL_GetProperties(pMenu);
   }
   else if ( clsMenu == AEECLSID_ICONVIEWCTL)
   {
      col.cBack = TB8_BACKGROUND;
      col.cSelBack = TB8_SELECT_BACKGROUND;

      dwProps |= MP_ICON_TEXT_TOP|MP_NO_ARROWS;
   }
   else if(clsMenu == AEECLSID_SOFTKEYCTL)
   {
      col.cBack = SOTFKEY_BACKGROUND;
      col.cSelBack = SOTFKEY_SEL_BACKGROUND;
   }

   if (clsMenu == AEECLSID_MENUCTL || clsMenu == AEECLSID_SOFTKEYCTL || clsMenu == AEECLSID_ICONVIEWCTL)
   {
      dwProps |= MP_UNDERLINE_TITLE | MP_WRAPSCROLL;
      IMENUCTL_SetStyle(pMenu, &normal, &sel);
      IMENUCTL_SetColors(pMenu, &col);
      IMENUCTL_SetProperties(pMenu, dwProps);
      if (pRect)
         IMENUCTL_SetRect(pMenu, pRect);
   }
   
#endif  //AEE_STATIC   

}//Brew_SetMenuAttr()

/*===========================================================================
   This function adds one item to the specified IMenuCtl.
===========================================================================*/
boolean Brew_AddMenuItem(IMenuCtl * pMenu ,char *pszResFile,uint16 wTextID,
                         AECHAR * pText, uint16 wImageID,
                         uint16 wItemID, uint32 dwData)
{
   CtlAddItem  ai;

   if((pMenu == NULL) || (pszResFile == NULL)) return FALSE;

   // Fill in the CtlAddItem structure values
   ai.pText =       pText;
   ai.pImage =      NULL;
   ai.pszResImage = pszResFile;
   ai.pszResText  = pszResFile;
   ai.wText   =     wTextID;
   ai.wFont   =     AEE_FONT_NORMAL;
   ai.wImage  =     wImageID;
   ai.wItemID =     wItemID;
   ai.dwData  =     dwData;
   // Add the item to the menu control
   return IMENUCTL_AddItemEx( pMenu, &ai );
}//Brew_AddMenuItem()

/*===========================================================================
   This function adds softkey control
===========================================================================*/
void Brew_AddSoftItem(IMenuCtl *pMenu,char *pszResFile,AEERect *pRect, uint16 wResID, uint16 nItemID, uint16 wResID2, uint16 nItemID2)
{      
    if(IMENUCTL_IsActive(pMenu))
        IMENUCTL_SetActive(pMenu, FALSE);
    
    IMENUCTL_Reset(pMenu);
    
    (void)Brew_SetMenuAttr(pMenu,AEECLSID_SOFTKEYCTL,pRect,MP_MAXSOFTKEYITEMS);//set menu attible
    if(wResID != NULL && nItemID != NULL)
    {
        if (pszResFile)
            IMENUCTL_AddItem(pMenu, pszResFile, wResID, nItemID, NULL, 0);
    }
    
    if(wResID2 != NULL && nItemID2 != NULL)
    {
        if (pszResFile)
            IMENUCTL_AddItem(pMenu, pszResFile, wResID2, nItemID2, NULL, 0);
    }
    
    IMENUCTL_SetProperties(pMenu, MP_WRAPSCROLL);
    IMENUCTL_SetActive(pMenu, TRUE);
    
    IMENUCTL_Redraw(pMenu);
   
   
}//Brew_AddSoftItem()


/////////////////////////////////////////////////////////////////////////////////////////////
//deal menuctl up and down auto scroll event in the EVT_KEY event
//wParam
//    ps : shell interface
//     p : IMENUCTL interface

//   eCode [in] - Specifies the Event sent to this applet

//  wParam [in] - 16-bit event specific data.

/////////////////////////////////////////////////////////////////////////////////////////////
boolean UpAndDownScroll_HandleEvent(IShell* ps, void* p, AEEEvent eCode, uint16 wParam)
{
   uint16 nCurSel,nFirst,nLast;
   int nCount;

   if(!p)  return FALSE;
   if((eCode != EVT_KEY)||((wParam != AVK_DOWN) && (wParam != AVK_UP)))  return FALSE;   
   if(!GET_PVTBL(p,IMenuCtl)->IsActive(p))  return FALSE;

   nCurSel = GET_PVTBL(p,IMenuCtl)->GetSel(p);
   nCount  = GET_PVTBL(p,IMenuCtl)->GetItemCount(p);
   nFirst  = GET_PVTBL(p,IMenuCtl)->GetItemID(p, 0);
   nLast   = GET_PVTBL(p,IMenuCtl)->GetItemID(p, nCount - 1);

   if((nCurSel == nFirst) && ((AVKType)wParam == AVK_UP))
   {
      nCurSel = nLast;
      GET_PVTBL(p,IMenuCtl)->SetSel(p,nCurSel);
      return TRUE;
   }
   else if((nCurSel == nLast) && ((AVKType)wParam == AVK_DOWN))
   {
      nCurSel = nFirst;
      GET_PVTBL(p,IMenuCtl)->SetSel(p,nCurSel);
      return TRUE;
   }
   return FALSE;
}//PinTu_UpAndDownScroll_HandleEvent()

//////////////////////////////////////////////
//createInstance
//////////////////////////////////////////////
boolean Common_CreateObj(IShell *pIShell, AEECLSID clsID, void **ppObj)
{
   if(!(*ppObj))
   {
      if(ISHELL_CreateInstance(pIShell, clsID, (void **)&(*ppObj)) != SUCCESS)
         return FALSE;     // create instance failure
   }
   return TRUE;
}//Common_CreateObj()

//////////////////////////////////////////////
//release Instance
//////////////////////////////////////////////
void Common_ReleaseObj(void ** ppObj)
{
    if (*ppObj)
    {
      (void)IBASE_Release(((IBase *)*ppObj));
      *ppObj = NULL;
    }   
}//Common_ReleaseObj()

//////////////////////////////////////////////
//ÅÐ¶Ï¿í×Ö·û´®ÖÐÊÇ·ñº¬ÓÐ·ÇASCII×Ö·û¡£
//////////////////////////////////////////////
boolean JudgeHaveNoneASCIIChar ( AECHAR * pWstr, int * pPostion)
{
    int i ,len;
    AECHAR *pWstrTep = pWstr;

    if (NULL == pWstrTep)
    {
        return FALSE;
    }
    
    len = WSTRLEN(pWstrTep);
    if (len <= 0)
    {
        return FALSE;
    }
    for (i=0; ((i < len) && ((*pWstr) < 128)); i++,pWstr++)
    {
        ;
    }
    if (i < len)
    {
        if (NULL != pPostion)
        {
            *pPostion = i;
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


/*===========================================================================
   player sound data
===========================================================================*/
void PlayerSound(IRingerMgr* pRingerMgr,MediaType type)
{
    char fileName[64];     
    
    if (pRingerMgr == NULL) return ;
    
    MEMSET(fileName,0,sizeof(fileName));
    STRCPY(fileName,MEDIA_PATH);
    STRCAT(fileName,DIRECTORY_STR);
    
    switch(type)
    {
        case DROP_PIECE_MEDIA:
            STRCAT(fileName,"droppiece.wav");
            break;
        
        case DANGER_MEIDA:
            STRCAT(fileName,"clean.wav");
            break;
        
        case SUCCESS_MEIDA:
            STRCAT(fileName,"success.wav");
            break;
        
        case FAILURE_MEDIA:         
            STRCAT(fileName,"fail.wav");
            break;
        
        default:
            return ;
    }   
        
    (void)IRINGERMGR_Stop(pRingerMgr);
    (void)IRINGERMGR_PlayFile(pRingerMgr,fileName,0);         

}//PlayerSound()
