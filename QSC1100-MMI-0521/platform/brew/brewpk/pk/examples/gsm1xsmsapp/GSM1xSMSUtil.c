/*===========================================================================

                           S M S U t i l . c
                

GENERAL DESCRIPTION
   This file contains utility procedures for common functions.

EXTERNALIZED FUNCTIONS
   GSM1xSMSUtil_ExtractTitle
   GSM1xSMSUtil_SecsToDateString
   GSM1xSMSUtil_InitMenuIcons
   GSM1xSMSUtil_SetMenuIcon
   GSM1xSMSUtil_ShowBusyIcon

INITIALIZATION AND SEQUENCING REQUIREMENTS
   None

Copyright (c) 2001,2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "GSM1xSMSApp.h"
#include "gsm1xsms_res.h"
#include "GSM1xSMSUtil.h"
#include "GSM1xSMSMessageStore.h"
#include "aeeext_res.h"
#include "AEEControls_res.h"

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

/*===========================================================================

                    TYPE DECLARATIONs

===========================================================================*/

/*===========================================================================

                    GLOBAL DATA 

===========================================================================*/

/*===========================================================================

                    LOCAL/STATIC DATA 

===========================================================================*/

/*===========================================================================

                       FUNCTION DECLARATIONS

===========================================================================*/

/*=============================================================================
FUNCTION: GSM1xSMSUtil_SecsToDateString

DESCRIPTION:
   Convert dwSecs to a string displaying the date.

PROTOTYPE:
   void GSM1xSMSUtil_SecsToDateString(SMSApp *pMe, uint32 dwTime,
                                       AECHAR *szBuf, int wBufLen);

PARAMETERS:
   pMe [in]: Pointer to the AEEApplet structure.

   dwTime [in]: Date in seconds form.

   szBuf [out]: String where date will be placed.

   wBufLen [in]: Length of szBuf string.

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   None
=============================================================================*/
void GSM1xSMSUtil_SecsToDateString(SMSApp *pMe, uint32 dwTime,
                              AECHAR *szBuf, int wBufLen) {

   JulianType         jt;
   AECHAR             szFormat[64];

   if (!pMe || !szBuf) {
      return;
   }

   // Load the date/time format string from resources.
   (void)ISHELL_LoadResString(pMe->a.m_pIShell, GSM1XSMS_RES_FILE,
                        IDS_DATE_FORMAT,
                        szFormat, sizeof(szFormat));

   // Get the date/time structure for the time represented by dwTime.
   GET_JULIANDATE(dwTime, &jt);

   // Fill in the string with the date/time information.
   WSPRINTF(szBuf, wBufLen, szFormat,
            jt.wMonth, jt.wDay, jt.wYear, jt.wHour, jt.wMinute, jt.wSecond);
}


/*=============================================================================
FUNCTION: GSM1xSMSUtil_AddBitmapMenuItem
  
DESCRIPTION:
   This function encapsulates the code needed to add a menu item with an icon
   to the current menu.

PROTOTYPE:
   boolean GSM1xSMSUtil_AddBitmapMenuItem(SMSApp *pMe,
                                IMenuCtl  * pMenu,
                             AECHAR    * pszMenuText,
                             uint16      wTextResID,
                             uint16      wImageResID,
                             AEEFont     wFont);

PARAMETERS:
   pMe [in]: Pointer to the AEEApplet structure.

   pMenu [in]: Pointer to menu to which to add the item.

   pszMenuText [in]: String containing menu text.  If this is a NULL pointer or
      an empty string the menu text will be obtained from the string resource
      with id wTextResID.

   wTextResID [in]: ID for command item.  If pszMenuText is NULL or an empty
      string this will also be used as the resource ID for the menu text.

   wImageResID [in]: Resource ID for the menu image.
   
   wFont [in]: Font type for the text of menu item

   dwData [in]: The 32bit parameter to be sent to the handler

RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:
   None
=============================================================================*/
boolean GSM1xSMSUtil_AddBitmapMenuItem(SMSApp *pMe,
                          IMenuCtl *pMenu,
                          AECHAR   *pszMenuText,
                          uint16    wTextResID,
                          uint16    wImageResID,
                          AEEFont   wFont,
                          uint32    dwData) {

   CtlAddItem       mai;

   if (!pMe || !pMenu) {
      return FALSE;
   }

   MEMSET(&mai, 0, sizeof(mai));

   // Fill in the menu structure.
  
   switch (wImageResID) {
   case IDB_RADIOBUTTON_ON:
      mai.pszResImage = AEEEXT_RES_FILE;
      mai.wImage = IDB_SELECT_ICON;
      break;
   case IDB_RADIOBUTTON_OFF:
      mai.pszResImage = AEEEXT_RES_FILE;
      mai.wImage = IDB_NONSELECT_ICON;
      break;
   default:
      mai.pszResImage = GSM1XSMS_RES_FILE;
      mai.wImage = wImageResID;
      break;
   }

   
   if (pszMenuText && (pszMenuText[0] != (AECHAR)'\0')) {
      mai.pText = pszMenuText;
   } else {
      mai.pszResText = GSM1XSMS_RES_FILE;
      mai.wText = wTextResID;
   }
   mai.wItemID = wTextResID;
   mai.wFont = wFont;
   mai.dwData = dwData;

   // Add the menu item.
   return IMENUCTL_AddItemEx(pMenu, &mai);
}

/*=============================================================================
FUNCTION: GSM1xSMSUtil_EncodeMBoxAndID
  
DESCRIPTION:
   Encode a mailbox value and a message ID into 32 bits. The mailbox value
   goes into the upper 16 bits and the message ID goes in the lower 16 bits.

PROTOTYPE:
   uint32 GSM1xSMSUtil_EncodeMBoxAndID (SMSMBoxType box, uint16 wID);

PARAMETERS:
    box [in]: Enumeration value of the mailbox
    
    wID [in]: Message ID

RETURN VALUE:
   32 bit value with encoded mailbox value and message ID

COMMENTS:
    This encoding scheme allows us to uniquely determine any message with 32
    bits. This is useful when identifying messages in a menu item.
  
SIDE EFFECTS:
   None
=============================================================================*/
uint32 GSM1xSMSUtil_EncodeMBoxAndID (SMSMBoxType eMBoxType, uint16 wID) {

   uint32 tmp;

   tmp = ((uint32)eMBoxType) << 16; // put box type in upper 16 bits
   tmp = tmp | wID;                // put ID in lower 16 bits
   return tmp;
}

/*=============================================================================
FUNCTION: GSM1xSMSUtil_DecodeMBoxAndID
  
DESCRIPTION:
    Extracts the message ID and mailbox value from an encoded 32 bit
    number.

PROTOTYPE:
   void GSM1xSMSUtil_DecodeMBoxAndID (uint32 dwCode,
             SMSMBoxType * peMBoxType, uint16 *pwID);

PARAMETERS:
    wdCode [in]: Value with encoded mailbox value and message ID
    
    peMBoxType [out]: Pointer to variable to store mailbox value
    
    pwID [out]: Pointer to variable to store message ID


RETURN VALUE:
   None

COMMENTS:
    The message ID is encoded in the lower 16 bits and the mailbox value is 
    in the upper 16 bits.
   
SIDE EFFECTS:
   None
=============================================================================*/
void GSM1xSMSUtil_DecodeMBoxAndID (uint32 dwCode, 
                              SMSMBoxType *peMBoxType, uint16 *pwID) {

   // the ID is the lower 16 bits 
   *pwID = (uint16)DECODE_MSG_ID(dwCode);   
   // the box type is the upper 16 bits
   *peMBoxType = (SMSMBoxType)((uint16)DECODE_MBOX(dwCode));  
}


/*===========================================================================

FUNCTION
   ExtractTitle

DESCRIPTION
   Updates the title field depending upon the mail box type.
   
   SIM Box and Inbox: This function extracts the title (to be displayed on the 
   screen) from the From field (if there is any).
   
   Outbox : Extracts the title based on the outgoing number (To field)

DEPENDENCIES
   None

PARAMETERS
   pMe [in]: Pointer to the SMS applet structure
   eMBoxType [in]: Type of mail box to indicate which mail box is making this
                   request to extract title.
   pInMsg [in/out]: Pointer to SMS message structure

RETURN VALUE
   None

SIDE EFFECTS
   Depending upon the mail box type, if there is any From/To field present in 
   the received SMS message, then the pInMsg Title field is updated.
===========================================================================*/

void GSM1xSMSUtil_ExtractTitle 
( 
   SMSApp* pMe, 
   SMSMBoxType eMBoxType,
   SMSMessageStruct* pInMsg 
)
{
   uint16               length;
   AECHAR searchStr[MAX_TITLE_LEN];

   // Some sanity checking on the pointers...
   if(!pMe || !pInMsg)
   {
      return;
   }

   // The title is determined is the following order:
   //  Contact name
   //  Phone
   //  Email 
   //  Message text

   pInMsg->m_szTitle[0] = 0;

   // Update the search string based on the mail box id.

   if((eMBoxType == SMS_MB_OUTBOX) || (eMBoxType == SMS_MB_SENT_OUTBOX))
   {
      // If the mailbox type is set to outbox (or sentbox) then we update the
      // title based on the "to" field.
      
      // Leave an extra space at the end of the search string so that BREW 
      // can add a NULL character.
      WSTRNCOPYN(searchStr,sizeof(searchStr)/sizeof(AECHAR),
                 pInMsg->m_szTo,(sizeof(searchStr)/sizeof(AECHAR)) - 1);
   }
   else
   {
      // If the mailbox type is set to anything other than outbox (or sentbox)
      // then we update the title based on the "from" field.

      // Leave an extra space at the end of the search string so that BREW 
      // can add a NULL character.
      WSTRNCOPYN(searchStr,sizeof(searchStr)/sizeof(AECHAR),
                 pInMsg->m_szFrom,(sizeof(searchStr)/sizeof(AECHAR)) - 1);
   }

   // First we check for the presence of From field in the message. If there is
   // none then the title is not set. If the field is present then we extract
   // the title from it.

   length = (uint16)WSTRSIZE(searchStr);
   if(length > 0)
   {
#if 0
      // At this point, pInMsg->m_szFrom points to the alphanumeric string that
      // is supposed to represent the phone number.  We can go two ways:
      // #1) The "From" field is blank, so a DB lookup is not possible...
      //     Hence the header is just the first MAX_TITLE_LEN characters of the 
      //     message.
      // #2) If there is no contact stored for this "from" field then we display
      //     the number as it is.

      // Perform lookup here
      AEEAddrField *addrField= NULL;
      IAddrRec     *addrRec = NULL;
      AECHAR       *pContactName = NULL;
      int          wTotalFields = 0;
      int          j =0;

      // Create the IAddrBook interface if this is the first time
      // this function has been called.
      if (NULL == pMe->m_pAddrBook) 
      {
         (void) ISHELL_CreateInstance(pMe->a.m_pIShell,
                                      AEECLSID_ADDRBOOK,
                                      (void **)&pMe->m_pAddrBook);
      }

      if (pMe->m_pAddrBook) 
      {
         if(IADDRBOOK_EnumRecInit(pMe->m_pAddrBook,
                                  AEE_ADDR_CAT_NONE,
                                  AEE_ADDRFIELD_SEARCH_EXACT_PHONE_NUM,
                                  searchStr,
                                  length) ==SUCCESS)
         {
            addrRec = IADDRBOOK_EnumNextRec(pMe->m_pAddrBook);
         }
   
         // If the number is not matched with the phone number then try to match
         // it with the another number.
         if(addrRec == NULL)
         {
            // Retrieve the Name based on the email address (use the new
            // BREW address book interface)
             if(IADDRBOOK_EnumRecInit(pMe->m_pAddrBook,
                                      AEE_ADDR_CAT_NONE,
                                      AEE_ADDRFIELD_EMAIL,
                                      searchStr,
                                      length) ==SUCCESS)
             {
                addrRec = IADDRBOOK_EnumNextRec(pMe->m_pAddrBook);
             }
         }
   
         if(addrRec != NULL)
         {
            // We are able to get a contact Name for our search string (phone
            // number or an email address).
            
            //Get the name of the Contact from the record
            wTotalFields = IADDRREC_GetFieldCount(addrRec);
   
            for(j=0;j< wTotalFields;j++)
            {
               addrField = IADDRREC_GetField(addrRec,j);
               if (addrField) 
               {
                  if (addrField->fID == AEE_ADDRFIELD_NAME) 
                  {
                     pContactName = WSTRDUP(addrField->pBuffer);
                     break;
                  }
               }
            }

            IADDRREC_Release(addrRec);
         }
      }
      else
      {
         // Some thing went wrong and we were failed to create an instance of
         // the address book. We just report an error and continue.
         DBGPRINTF("Failed to create AEECLSID_ADDRBOOK");
      }
      
      //if contact was found based on phonenumber or email
      if(pContactName!=NULL) 
      {
         WSTRNCOPYN(pInMsg->m_szTitle, MAX_TITLE_LEN, pContactName, -1);
      }
      else
      {
#endif
         // Use the phone number or email address (whatever the searchStr is
         // set to).
         WSTRNCOPYN(pInMsg->m_szTitle,
                    MAX_TITLE_LEN,
                    searchStr,
                    sizeof(searchStr)/sizeof(AECHAR));
#if 0
      }
#endif
   }
}

/*=============================================================================
FUNCTION:  GSM1xSMSUtil_InitMenuIcons
 
DESCRIPTION: Adds IDB_NONSELECT_ICON images to every menu item in the
             specified menu.
 
PARAMETERS:
   *pm: pointer to IMenuCtl instance
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSUtil_InitMenuIcons(IMenuCtl *pm)
{
   CtlAddItem cai;
   int        i;

   MEMSET(&cai, 0, sizeof(cai));
   cai.pszResImage = AEEEXT_RES_FILE;
   cai.wImage = IDB_NONSELECT_ICON;

   for (i = IMENUCTL_GetItemCount(pm); i >= 0; i--) {
      (void) IMENUCTL_SetItem(pm, 
                              IMENUCTL_GetItemID(pm, i), 
                              MSIF_IMAGE, 
                              &cai);
   }
}


/*=============================================================================
FUNCTION:  GSM1xSMSUtil_SetMenuIcon
 
DESCRIPTION:
 
PARAMETERS:
   *pm:    pointer to IMenuCtl instance
   listId: list ID of the item to set
   bSel:   indicates whether to select or unselect the icon
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSUtil_SetMenuIcon(IMenuCtl   *pm, 
                 uint16      listId, 
                 boolean     bSel)
{
   CtlAddItem cai;

   MEMSET(&cai, 0, sizeof(cai));

   cai.pszResImage = AEEEXT_RES_FILE;
   cai.wImage = bSel ? IDB_SELECT_ICON : IDB_NONSELECT_ICON;

   (void) IMENUCTL_SetItem(pm, listId, MSIF_IMAGE, &cai);
}

/*=============================================================================
FUNCTION:  GSM1xSMSUtil_ShowBusyIcon
 
DESCRIPTION: Sets or clears the busy icon
 
PARAMETERS:
   *ps: pointer to IShell instance
   pd: pointer to IDisplay instance
   rc: pointer to AEERect structure
   bClear: indicates whether to set or clear the busy icon
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void GSM1xSMSUtil_ShowBusyIcon(IShell *ps, IDisplay *pd, AEERect *rc, boolean bClear)
{
   IImage       *busy;
   AEEImageInfo  ii;
   AEEBitmapInfo info;

#if 0
   IBitmap *pIBitmap;
   int rv;

   rv = IDISPLAY_GetDeviceBitmap(pMe->a.m_pIDisplay, IBitmap &pIBitmap);
   if (rv == SUCCESS)
   {
      rv = IBITMAP_GetInfo(pIBitmap, &info, sizeof(AEEBitmapInfo));
      if (rv != SUCCESS)
      {
         info.nDepth = 1;
      }
   }
#endif

   info.nDepth = 1;

   busy = ISHELL_LoadResImage(ps,
                              AEECONTROLS_RES_FILE,
                              AEE_IDB_BUSY);
      
   if (bClear) {
      IDISPLAY_ClearScreen(pd);
   }

   if (busy != NULL) {
      IIMAGE_GetInfo(busy, &ii);

      // if color display
      if (info.nDepth > 1)
      {
         IIMAGE_SetParm(busy, IPARM_ROP, (int) AEE_RO_MASK, 0);
      }

      IIMAGE_Draw(busy,
                  rc->x + (rc->dx - ii.cx) / 2,
                  rc->y + (rc->dy - ii.cy) / 2);
      IIMAGE_Release(busy);
   }

   IDISPLAY_UpdateEx(pd, FALSE);
}

/*=============================================================================
FUNCTION:  GSM1xSMSUtil_GSMTime2Secs
 
DESCRIPTION: Convert from GSM time format to seconds
 
PARAMETERS:
   gsmtime: the GSM time to convert to seconds
 
RETURN VALUE:
   The time in seconds

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int GSM1xSMSUtil_GSMTime2Secs(GSMSMSTime *gsmtime)
{
   JulianType jt;
   
   jt.wYear   = gsmtime->year;
   jt.wMonth  = gsmtime->month;
   jt.wDay    = gsmtime->day;
   jt.wHour   = gsmtime->hour;
   jt.wMinute = gsmtime->minute;
   jt.wSecond = gsmtime->second;

   return (JULIANTOSECONDS(&jt) + (gsmtime->tzoffset * 60));
}

