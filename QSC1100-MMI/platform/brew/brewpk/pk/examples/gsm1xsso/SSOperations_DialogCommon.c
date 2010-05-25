/*=============================================================================
FILE: SSOperations_DialogCommon.c

SERVICES: GSM1x SS Operations Dialog Common Functions

GENERAL DESCRIPTION:
      This file defines common functions for the SS Operations
      dialog handler functionality

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================== */
#include "AEEShell.h"         // AEE Shell Services
#include "AEEStdLib.h"        // AEE StdLib Services

#include "SSOperations_DialogCommon.h"
#include "SSOperations_DialogHandler.h"

#include "SSOperations_UserEvents.h"
#include "SSOperations_CommonMessageDefinitions.h"

// TBD!
// Temp HACK in order to avoid changing the phone & applet makefiles
#include "IStaticScrollingWrapper.c"

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/

/*===========================================================================
                    TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                    FUNCTION DECLARATIONS
===========================================================================*/

/*===========================================================================
                    LOCAL/STATIC DATA
===========================================================================*/

/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

/*=============================================================================
FUNCTION: SSOperationsDialog_ShowDialog

DESCRIPTION: 
    Display a new dialog. Failure in creation of the dialog is a severe
    error, could be due to memory shortage - so we quit.

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    dlgResId [in]: Dialog resource ID

RETURN VALUE:
    None

=============================================================================*/
void SSOperationsDialog_ShowDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    uint16                              dialogId
    )
{
   if (SUCCESS != ISHELL_CreateDialog(pSelf->m_pShell,
                                      SSOPERATIONS_RES_FILE,
                                      dialogId,
                                      NULL)) 
   {
      GSM1X_LOG_FATAL(pSelf->m_pSSUtilCls,"SSO:Unable to open dialog %d!", 
                        dialogId, 0, 0);
      ISHELL_CloseApplet(pSelf->m_pShell, FALSE); // applet quits.
   }
   else
   {
      GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls,"SSO:ShowDialog %d", dialogId, 0, 0);
   }
}

/*=============================================================================
FUNCTION: SSOperationsDialog_SetupNewDialog

DESCRIPTION: display a new dialog

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer           
    setupHandler [in]: dialog setup handler function

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialog_SetupNewDialog(
    struct _CSSOperationsDialogHandler *pSelf,
    DIALOG_SETUP_HANDLER                setupHandler
    )
{
    assert(setupHandler != NULL);

    pSelf->m_pDialogSetupHandler = setupHandler;

    // call the setup handler in order to setup the non display handler (move state)
    setupHandler(pSelf, FALSE);

    if ((pSelf->m_PopupStatus == SS_NO_POPUP) && (pSelf->m_bSuspended == FALSE))
    {
        CLOSE_ACTIVE_DIALOG;
    }
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_GetCallInfo

DESCRIPTION: Get a call's call info structure, using the call index to find 
             the call.

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
   callIndex [in]: call index

RETURN VALUE:
   pointer to a valid SSCall_Info_type.

=============================================================================*/
SSCall_Info_type* SSOperationsDialogHandler_GetCallInfo(
    struct _CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type                   callIndex
    )
{
    SSCall_Info_type *pCallInfo = NULL;
    boolean success;

    success = SSCallDatabase_GetCallInfo(pSelf->m_pCallDatabase,
                                         callIndex,
                                         &pCallInfo);
    if (!success)
    {
        GSM1X_LOG_FATAL(pSelf->m_pSSUtilCls, "can't get call info %d", callIndex, 0, 0);
        assert(!"can't get call info");
    }

    return pCallInfo;
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_GetCallTInfo

DESCRIPTION: Get a call's transaction info, using the call index to find 
             the call.

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    callIndex [in]: call index

RETURN VALUE:
   pointer to a valid SSCall_TInfo_type.

COMMENTS:
   call index must be that of a regular call and not of a MPTY call.

=============================================================================*/
SSCall_TInfo_type* SSOperationsDialogHandler_GetCallTInfo(
    struct _CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type                   callIndex
    )
{
    SSCall_TInfo_type *pCallTInfo = NULL;
    boolean            success;

    success = SSCallDatabase_GetCallTInfoByCallIndex(
                 pSelf->m_pCallDatabase,
                 callIndex,
                 &pCallTInfo
                 );

    if (!success)
    {
        GSM1X_LOG_FATAL(pSelf->m_pSSUtilCls, "GetCallTInfo Failed! %d", callIndex,0,0);
        assert(!"can't get call transaction info");
    }

    return pCallTInfo;
}


/*=============================================================================
FUNCTION: SSOperationsDialogHandler_GetCallTInfoIndex

DESCRIPTION: Get a call's transaction info index, using the call index to find
             the call.

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    callIndex [in]: call index

RETURN VALUE:
   call transaction info index.

COMMENTS:
   call index must be that of a regular call and not of a MPTY call.

=============================================================================*/
SSCall_TInfo_Index_type SSOperationsDialogHandler_GetCallTInfoIndex(
    struct _CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type                   callIndex
    )
{
    boolean                 success;
    SSCall_TInfo_Index_type tInfoIndex; 

    success = SSCallDatabase_GetTInfoIndexByCallIndex(
                 pSelf->m_pCallDatabase,
                 callIndex,
                 &tInfoIndex
                 );

    if (!success)
    {
        GSM1X_LOG_FATAL(pSelf->m_pSSUtilCls, "GetCallTInfoIndex Failed! %d", callIndex,0,0);
        assert(!"can't get call transaction info index");
    }

    return tInfoIndex;
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_GetCallStartTime

DESCRIPTION: Get a call's start time, using the call index to find 
             the call.

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    callIndex [in]: call index

RETURN VALUE:
   start time value.

=============================================================================*/
uint32 SSOperationsDialogHandler_GetCallStartTime(
    struct _CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type                   callIndex
    )
{
    SSCall_Info_type *pCallInfo =
        SSOperationsDialogHandler_GetCallInfo(pSelf, callIndex);

    if (pCallInfo->isMPTY) 
    {
        return pCallInfo->MPTYStartTime;
    }
    else
    {
        SSCall_TInfo_type *pCallTInfo =
            SSOperationsDialogHandler_GetCallTInfo(pSelf, callIndex);

        return pCallTInfo->callStartTime;
    }
}

/*=============================================================================
FUNCTION: SSOperationsDialogHandler_GetCallString

DESCRIPTION: Get a call's call string, using the call index to find 
             the call.

PARAMETERS:
   *pSelf [in]: CSSOperationsDialogHandler object pointer
    callIndex [in]: call index

RETURN VALUE:
   null terminated call string

=============================================================================*/
AECHAR* SSOperationsDialogHandler_GetCallString(
    struct _CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type                   callIndex
    )
{
    SSCall_Info_type *pCallInfo =
        SSOperationsDialogHandler_GetCallInfo(
            pSelf,
            callIndex
            );

    if (pCallInfo->isMPTY)
    {
        return pSelf->m_MPTYCallString;
    }
    else
    {
        SSCall_TInfo_type *pCallTInfo =
            SSOperationsDialogHandler_GetCallTInfo(pSelf, callIndex);

        return pCallTInfo->callString;
    }
}

/*===========================================================================
FUNCTION SSOperationsDialog_CreateTransactionWiseEventData

DESCRIPTION
   Creates a transaction wise event data and initialize it.

PARAMETERS:
    *pSelf [in]: Pointer to the dialog handler object
     callIndex [in]: call index
    *pMsg [in]: Pointer to a dynamically allocated message. can be NULL.
                its memory is the function's responsibility.

RETURN VALUE:
    Pointer to a dynamically allocated event data or NULL if a failure occured.
    event data's memory is the responsibility of the caller.

COMMENTS:
    Implemented in SSOperations_DialogIncoming.c

===========================================================================*/
SSEventData* SSOperationsDialog_CreateTransactionWiseEventData(
    struct _CSSOperationsDialogHandler *pSelf,
    SSCall_Index_type                   callIndex,
    SSMsg_int_msg_type                 *pMsg
    )
{
    boolean      success;
    SSEventData *pEventData = 
        SSOEvent_AllocateEventData(SS_EVENT_DATA_TRANSACTIONWISE_MSG);

    if (pEventData == NULL)
    {
        if (pMsg != NULL) 
        {
            SSOMessage_FreeMsg((SSMsg_int_msg_type*)pMsg);
        }
        return NULL;
    }

    pEventData->transactionwiseMsgData.msg = pMsg;
    pEventData->transactionwiseMsgData.callIndex = callIndex;

    success = SSCallDatabase_GetTInfoIndexByCallIndex(
                 pSelf->m_pCallDatabase,
                 callIndex,
                 &(pEventData->transactionwiseMsgData.tInfoIndex)
                 );
    if (success)
    {
        return pEventData;
    }
    else
    {
        assert(!"Bad call index or corrupted DB");
        return NULL;
    }
}

/*=============================================================================
FUNCTION:  SSOperationsDialog_GetFirstLineOfStr
 
DESCRIPTION: Checks if the specified string will fit in a single line of 
             the device display, which is assumed to be MAX_SIZE_DIAL_STR. 
             If the string is too long, we keep a truncated version in a 
             temporary buffer in the applet struct, and return its address.
             The original string will not be modified. 
 
PARAMETERS:
   *pSelf: CRefUIIdle object pointer 
      fnt: Which font to use
    *wStr: string to check
 
RETURN VALUE:
    AECHAR const *: a portion of the string that fits on one line.
            This is either the original, or the temp buffer mentioned above.
 
=============================================================================*/

AECHAR const* SSOperationsDialog_GetFirstLineOfStr(
    struct _CSSOperationsDialogHandler *pSelf,
    AEEFont                             fnt,
    AECHAR                             *wStr
    )
{
    int wStrLen;
    int wStrFits; /* how many AECHARs fit in a display line. */
    int wDisplayLineBufSize =  
        sizeof(pSelf->m_wDisplayLineBuf)/sizeof(AECHAR); /* size in AECHARs */
    AECHAR wEllipsis[4] = {'.', '.', '.' ,0};

    assert(wStr != NULL);

    wStrLen = WSTRLEN(wStr);

    // if IDISPLAY_MeasureTextEx failed, just return the entire string
    if (0 == IDISPLAY_MeasureTextEx(pSelf->m_pDisplay,fnt,wStr,wStrLen,
                                    pSelf->m_Rect.dx,&wStrFits))
    {
       return wStr;
    }

    if (wStrFits == wStrLen) {
       // Entire string wStrFits on the line, all is good
       return wStr;
    }
   
    // NOTE:  The following code assumes that we are using a monospaced font!
    wStrFits -= 3; // leave room for ellipsis
    WSTRNCOPYN(pSelf->m_wDisplayLineBuf, 
               wDisplayLineBufSize - 3, 
               wStr,
               wStrFits);

    WSTRCAT(pSelf->m_wDisplayLineBuf, wEllipsis);

    return pSelf->m_wDisplayLineBuf;
}


/*=============================================================================
FUNCTION: SSOperationsDialog_ProcessCallNumberAndName

DESCRIPTION:  sets the call transaction info entry's call name using the
              contacts database, and sets the call phone number to according
              to its PI (presentation indication) data.

PARAMETERS:
   *pSelf      [in]: Pointer to the dialog handler object
   *pCallTInfo [in]: pointer to call transaction info entry

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialog_ProcessCallNumberAndName(
    struct _CSSOperationsDialogHandler *pSelf,
    SSCall_TInfo_type                  *pCallTInfo
    )
{
    switch (pCallTInfo->callStringPI)
    {
        case SS_PI_ALLOWED:
        {
            SSOperationsDialog_DoNumberLookup(
                pSelf->m_pAddressBook,
                pCallTInfo->callString,
                pCallTInfo->callName,
                sizeof(pCallTInfo->callName)
                );
        }
        break;

        case SS_PI_RESTRICTED:
             pCallTInfo->callString[0] = (AECHAR)'\0';

            (void) ISHELL_LoadResString(pSelf->m_pShell,
                                        SSOPERATIONS_RES_FILE,
                                        IDS_NUMBER_RESTRICTED,
                                        pCallTInfo->callName,
                                        sizeof(pCallTInfo->callName));
            break;

        case SS_PI_UNAVAILABLE:
            pCallTInfo->callString[0] = (AECHAR)'\0';

            (void) ISHELL_LoadResString(pSelf->m_pShell,
                                        SSOPERATIONS_RES_FILE,
                                        IDS_NUMBER_UNAVAILABLE,
                                        pCallTInfo->callName,
                                        sizeof(pCallTInfo->callName));
            break;

    default:
        assert(!"Unknown call string PI value"); 
        return;
    }
}

/*=============================================================================
FUNCTION: SSOperationsDialog_DoNumberLookup

DESCRIPTION:  Searches the Contacts database for a name match for the
              specified phone number.

PARAMETERS:
   *pAddrBook [in]:  IAddrBook interface pointer
   *number    [in]:  Phone number to lookup
   *name:    [out]:  Destination buffer for the name
    size:     [in]:  Size of name buffer, in bytes

RETURN VALUE:
   None

=============================================================================*/
void SSOperationsDialog_DoNumberLookup(
    IAddrBook *pAddrBook, 
    AECHAR    *number, 
    AECHAR    *name, 
    uint32     size
    )
{
   IAddrRec *addrRec;
   int       wTotalFields;
   int       j;

   assert(NULL != name);
   assert(NULL != pAddrBook);

   name[0] = (AECHAR) '\0';

   //Start searching the address book with the search string
   if (SUCCESS == IADDRBOOK_EnumRecInit(pAddrBook,
                                        AEE_ADDR_CAT_NONE,
                                        AEE_ADDRFIELD_NONE,
                                        number,
                                        (uint16) WSTRSIZE(number))) 
   {
      for (;;) 
      {
         addrRec = IADDRBOOK_EnumNextRec(pAddrBook);

         if (NULL == addrRec) 
         {
            // No more matching records found, exit loop
            break;
         }

         //Get the name of the Contact from the record
         wTotalFields = IADDRREC_GetFieldCount(addrRec);

         for (j = 0; j < wTotalFields; j++) 
         {
            AEEAddrField *addrField = IADDRREC_GetField(addrRec,j);

            if (addrField && 
                (addrField->fID == AEE_ADDRFIELD_NAME) &&
                (addrField->pBuffer != NULL) &&
                (addrField->wDataLen > 0) ) 
            {
               WSTRNCOPYN(name,
                          (int)(size / sizeof(AECHAR)),
                          addrField->pBuffer,
                          -1);

               // Found one, bail out immediately
               //
               // NOTE:
               //    There could be more than one contact database match, 
               //    but for simplicity just take the first one that was 
               //    found.
               //
               IADDRREC_Release(addrRec);
               return;
            }
         }

         IADDRREC_Release(addrRec);
      }
   }
}

/*=============================================================================
FUNCTION: SSOperationsDialog_HandleDialogTimeout

DESCRIPTION: Generic timer used by several dialogs. 
             When this callback is invoked, it sends a EVT_USER_DIALOG_TIMEOUT 
             event to the applet.

PARAMETERS:
   pUserData: Assumed to be a pointer to a CSSOperationsDialogHandler.
              It is dictated by BREW type PFNNOTIFY.

RETURN VALUE:
    None

=============================================================================*/
void SSOperationsDialog_HandleDialogTimeout(void *pUserData)
{
   struct _CSSOperationsDialogHandler *pSelf = 
      (struct _CSSOperationsDialogHandler *)pUserData;

   assert(pSelf->m_pDialog != NULL);

   (void) ISHELL_SendEvent(
             pSelf->m_pShell,
             AEECLSID_SS_OPERATIONS,
             EVT_USER_DIALOG_TIMEOUT,
             0,
             0
             );
}

/*=============================================================================
FUNCTION: SSOperationsDialog_AVKType2ASCII

DESCRIPTION:
   Converts a {AVK_0..AVK_9,AVK_STAR,AVK_POUND} to the
   ASCII equivalent

PARAMETERS:
   key: BREW keycode

RETURN VALUE:
   char: ASCII value

COMMENTS:
   Returns the NULL character if the input value is out of range

=============================================================================*/
char SSOperationsDialog_AVKType2ASCII(AVKType key)
{

   if (AVK_STAR == key) {
      return '*';
   }

   if (AVK_POUND == key) {
      return '#';
   }

   if (key < AVK_0 || key > AVK_9) {
      return '\0';
   }

   return '0' + (char)(key - AVK_0); /*lint !e656*/
}

/*===========================================================================

FUNCTION: SSOperations_SetDefaultSoftkeyLook

DESCRIPTION: Sets a default softkey menu look 

PARAMETERS:
    *pIShell      [in]: Pointer to an IShell object
    *pSoftKey [in/out]: Pointer to an softkey menu object

RETURN VALUE:
   None

===========================================================================*/
void SSOperations_SetDefaultSoftkeyLook(
    IShell   *pIShell, 
    IMenuCtl *pSoftKey
    )
{
  AEEItemStyle normal, selected;

  ISHELL_GetItemStyle( pIShell, AEE_IT_MENU, &normal, &selected );

#ifdef FEATURE_COLOR_DISPLAY
  
  normal.ft = AEE_FT_NONE;
  normal.xOffset = normal.yOffset = 0;
  normal.roImage = AEE_RO_MASK;

  selected = normal;

#else

  normal.ft = AEE_FT_NONE;
  normal.xOffset = normal.yOffset = 0;
  normal.roImage = AEE_RO_COPY;

  selected = normal;
  selected.roImage = AEE_RO_XOR;

#endif

  IMENUCTL_SetStyle( pSoftKey, &normal, &selected );
}

#if (defined U_TEST && defined AEE_SIMULATOR)
/*=============================================================================
FUNCTION: SSOperations_TriggerCCEvent

DESCRIPTION: Testing Function used to send CC MNCC_DATA_INDICATION events 
             that simulate the reception of network messages

PARAMETERS:
   *pIShell [in]: Pointer to an IShell object
	msgType [in]: Message type to be created,
	tiFlag  [in]: TI flag (TRUE for 1 (MO), FALSE for 0 (MT))
	tiValue [in]: TI value (0-6)

RETURN VALUE:
    None

=============================================================================*/
void SSOperations_TriggerCCEvent(
    struct _CSSOperationsDialogHandler *pSelf,
	SSMsg_msg_type_type                 msgType,
	boolean                             tiFlag,
	uint8                               tiValue
	)
{
    SSEventData        *pEventData;
    SSMsg_int_msg_type *pIntMsg;
	
	pIntMsg = SSOMessage_AllocateMsg(msgType);
	assert (pIntMsg && "SSOMessage_AllocateMsg() failed");
    pIntMsg->gen_hdr.pd = SSMSG_PD_CC;
    pIntMsg->gen_hdr.tiflag = tiFlag;
    pIntMsg->gen_hdr.tivalue = tiValue;

    pEventData = SSOEvent_AllocateEventData(SS_EVENT_DATA_DATA_IND);
	assert (pEventData && "SSOEvent_AllocateEventData() failed");

    pEventData->dataIndData.msg = pIntMsg;

    GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls,
        "TriggerCCEvent: msgType:%d MO:%d TI:%d",
        msgType,
        tiFlag,
        tiValue
        );

    SS_PostEvent(
        pSelf->m_pShell, 
        AEECLSID_SS_OPERATIONS,
        MNCC_DATA_INDICATION,
        0, 
        (uint32)pEventData
        );
}
#endif //U_TEST && AEE_SIMULATOR

#if (defined U_TEST && defined AEE_SIMULATOR)
/*=============================================================================
FUNCTION: SSOperations_TriggerCCEventWithMsg

DESCRIPTION: Testing Function used to send CC MNCC_DATA_INDICATION events 
             that simulate the reception of network messages

PARAMETERS:
   *pSelf   [in]: CSSOperationsDialogHandler object pointer
	pMsg    [in]: Pointer to a Message to be sent. takes ownership of the 
                  message's memory.                  
	tiFlag  [in]: TI flag (TRUE for 1 (MO), FALSE for 0 (MT))
	tiValue [in]: TI value (0-6)

RETURN VALUE:
    None

=============================================================================*/
void SSOperations_TriggerCCEventWithMsg(
    struct _CSSOperationsDialogHandler *pSelf,
	SSMsg_int_msg_type                 *pMsg,
	boolean                             tiFlag,
	uint8                               tiValue
	)
{
    SSEventData *pEventData;

    assert(pMsg != NULL);

    pMsg->gen_hdr.pd = SSMSG_PD_CC;
    pMsg->gen_hdr.tiflag = tiFlag;
    pMsg->gen_hdr.tivalue = tiValue;

    pEventData = SSOEvent_AllocateEventData(SS_EVENT_DATA_DATA_IND);
	assert (pEventData && "SSOEvent_AllocateEventData() failed");

    pEventData->dataIndData.msg = pMsg;

    GSM1X_LOG_HIGH(pSelf->m_pSSUtilCls,
        "TriggerCCEvent: msgType:%d MO:%d TI:%d",
        pMsg->gen_hdr.msg_type,
        tiFlag,
        tiValue
        );

    SS_PostEvent(
        pSelf->m_pShell, 
        AEECLSID_SS_OPERATIONS,
        MNCC_DATA_INDICATION,
        0, 
        (uint32)pEventData
        );
}
#endif //U_TEST && AEE_SIMULATOR


