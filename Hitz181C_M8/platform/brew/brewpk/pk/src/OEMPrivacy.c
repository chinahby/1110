/*===========================================================================
                              OEMPrivacy.c

  OEM REFERENCE APPLICATION CONTROL ROUTINES
  
  This file contains a reference implementation of a privacy management
  system to safeguard the device user.
    
        Copyright © 1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/


#include "OEMFeatures.h"

#include "OEMPrivacy.h"
#include "OEMMsgs_res.h"
#include "AEE_OEM.h"
#include "AEEComdef.h"
#include "AEEStdLib.h"

#ifdef FEATURE_BREW_PRIVACY_PROMPT
static boolean OEM_PrivacyDialogEvent(void *po, AEEEvent evt, uint16 wp, uint32 dwp);
static void    OEM_PrivacyDialogCancel(AEECallback *pCB);
#endif /* FEATURE_BREW_PRIVACY_PROMPT */

/*==================================================================
Function: OEM_CheckPrivacy

Description: Allows the OEM/Carrier to specify the correct action
for various outbound requests

Prototype:
   void OEM_CheckPrivacy(OEMPrivacyReqType t, AEECallback *pCB, int *pStatus)

Parameter(s):

Return Value:  None
Comments:      None
Side Effects:  None
See Also:      None

==================================================================*/
void OEM_CheckPrivacy(OEMPrivacyReqType t, AEECallback *pCB, int *pStatus)
{
   IShell *       pIShell = AEE_GetShell();
   int            nErr = SUCCESS;
#ifdef FEATURE_BREW_PRIVACY_PROMPT
   AEEPromptInfo  bi;
   uint16         wButtons[3];
   IDialog *      pd;

   nErr = ENOMEMORY;

   if(!pIShell)
      return;

   switch(t){
      case PRT_POSITION:
      case PRT_DIAL_VOICE:
         
         MEMSET(&bi, 0, sizeof(AEEPromptInfo));
         MEMSET(wButtons, 0, sizeof(wButtons));
         wButtons[0]    = OEM_IDC_YES;
         wButtons[1]    = OEM_IDC_NO;
         bi.pszRes      = OEMMSGS_RES_FILE;
         bi.pBtnIDs     = wButtons;
         bi.wTextID     = (uint16)(t == PRT_POSITION ? OEM_IDS_POS_REQUEST : OEM_IDS_DIAL_VOICE);
         bi.wTitleID    = OEM_IDS_PRIVACY;
         bi.wDefBtn     = OEM_IDC_NO;
         bi.dwProps     = CP_USE_DEFAULT;
         if(ISHELL_Prompt(pIShell, &bi)){
            pd = ISHELL_GetActiveDialog(pIShell);
            if(pd){
               pCB->pmc = (void *) pStatus;
               pCB->pfnCancel = (PFNCBCANCEL) OEM_PrivacyDialogCancel;
               pCB->pReserved = (void *) pd;
               IDIALOG_SetEventHandler(pd, (PFNAEEEVENT)OEM_PrivacyDialogEvent, (void *) pCB);
               IDIALOG_SetProperties(pd, DLG_HANDLE_ALL_EVENTS);
               ISHELL_SendEvent(pIShell, 0,EVT_DIALOG_START, IDIALOG_GetID(pd), (uint32)pd);
               return;
            }
         }

      default:
         nErr = EFAILED;
   } 
#endif /* FEATURE_BREW_PRIVACY_PROMPT */
   if(pStatus)
      *pStatus = nErr;
   ISHELL_Resume(pIShell, pCB);
}

#ifdef FEATURE_BREW_PRIVACY_PROMPT
static boolean OEM_PrivacyDialogEvent(void * po, AEEEvent evt, uint16 wp, uint32 dwp)
{
   AEECallback *pCB = (AEECallback *) po;
   IShell *pIShell  = AEE_GetShell();
   int    *pStatus;

   if(!pIShell)
      return FALSE;

   if(evt == EVT_KEY_PRESS || evt == EVT_KEY_RELEASE || evt == EVT_KEY) {

      return TRUE;
   }

   if(evt == EVT_COMMAND && pCB){
      ISHELL_SendEvent(pIShell, 0,EVT_DIALOG_END, 0, (uint32)wp);
      
      pStatus = (int *) pCB->pmc;
      
      if(pStatus)
         *pStatus = (int) (wp == OEM_IDC_YES ? 0 : EPRIVLEVEL);

      ISHELL_Resume(pIShell, pCB);
      return(TRUE);
   }
   return(FALSE);
}

static void OEM_PrivacyDialogCancel(AEECallback *pCB) {
   
   IShell *pIShell  = AEE_GetShell();

   ISHELL_EndDialog(pIShell);
}
#endif /* FEATURE_BREW_PRIVACY_PROMPT */
