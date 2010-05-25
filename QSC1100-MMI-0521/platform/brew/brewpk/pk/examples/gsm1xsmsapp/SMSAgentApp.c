/*===========================================================================
  FILE: SMSAgentApp.c

  SERVICES: SMS Agent Applet
 
  GENERAL DESCRIPTION:
            Application for handling the SMS messages

  PUBLIC CLASSES AND STATIC FUNCTIONS:
            class SMSApp

  INITIALIZATION AND SEQUENCING REQUIREMENTS:

  Copyright © 2002 QUALCOMM Incorporated. All Rights Reserved. 
  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                           INCLUDE FILES FOR MODULE
===========================================================================*/

#include "SMSAgentApp.h"           // Main Applet Header
#include "AEEStdLib.h"
//#include "GSM1xPhone.h"
#include "GSM1xSMSApp.bid"

/*===========================================================================

Function:  SMSAgentApp_New()

Description:
    This function allocates applet memory, and initializes the app data.

Parameters:

Return Value:
    SUCCESS or EFAILED.

==============================================================================*/
int SMSAgentApp_New(
    AEECLSID ClsId,
    IShell  *pIShell,
    IModule *pIModule,
    SMSAgentApp   **ppObj
    )
{
    
    if ((ClsId != AEECLSID_SMS_AGENT)
        || (pIShell == NULL)
        || (pIModule == NULL)
        || (ppObj == NULL))
    {
        return EFAILED;
    }

    if(AEEApplet_New(
       sizeof(SMSAgentApp),
       ClsId,
       pIShell,
       pIModule,
       (IApplet**)ppObj,
       (AEEHANDLER)SMSAgentApp_HandleEvent,
       (PFNFREEAPPDATA)SMSAgentApp_FreeAppData
       ) == FALSE)
    {
        return ENOMEMORY;
    }
    
    if (SMSAgentApp_InitAppData((IApplet*)*ppObj) != SUCCESS) 
    {
		DBGPRINTF("SMSAgentApp:In SMSAgentApp_New SMSAgentApp_InitAppData FAILED!");
      	IAPPLET_Release((IApplet*)*ppObj);
        *ppObj = NULL;
        return EFAILED;
    }

    return SUCCESS;
}

//-----------------------------------------------------------------------------
//                            Applet Method Definitions
//-----------------------------------------------------------------------------

/*=============================================================================
FUNCTION: GSM1xSMSApp_HandleEvent

DESCRIPTION:
    This is the EventHandler for this app. All events to this app are handled
    in this function.

PARAMETERS:
    po [in]: Pointer to the SMS Applet structure. This structure contains
             information specific to this applet.
    
    ecode [in]: Specifies the Event sent to this applet

    wParam, dwParam [in]: Event specific data.

RETURN VALUE:
    TRUE: If the app has processed the event.
    FALSE: If the app did not process the event.

COMMENTS:

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
boolean SMSAgentApp_HandleEvent(IApplet     *po,
                                   AEEEvent     eCode,
                                   uint16       wParam,
                                   uint32       dwParam)
{
   SMSAgentApp *pMe = (SMSAgentApp*)po;
   switch(eCode)
   {
      case EVT_APP_START:
		 return TRUE;

      case EVT_APP_STOP:  
		 return TRUE;

      case EVT_APP_SUSPEND:
		 return TRUE;

      case EVT_APP_RESUME:
		 return TRUE;

      case EVT_NOTIFY:
		 return SMSAgentApp_HandleNotifyEvent(pMe, (AEENotify*)dwParam);

      default:
		  break;
   }
   return FALSE;
} // SMSAgentApp_HandleEvent()

//------------------------------------------------------------------------------
// Public Helper Function Definitions (Used by all other modules of the SMS)
//------------------------------------------------------------------------------

/*=============================================================================
FUNCTION: GSM1xSMSApp_HandleNotifyEvent

DESCRIPTION:
    Handle the case of a notify event.

PARAMETERS:
    pMe [in]: Pointer to the SMS Applet structure.
    pN [in]: Pointer to a AEENotify object

RETURN VALUE:
    boolean

COMMENTS:
   Handles all the notifications from the Phone (status updates, new incoming
   page/message/voice mail messages). Also saves the message to the inbox
   and updates the temporary state (based on the storage status). Finally 
   checks to make sure that a state transition is required. If so, closes
   the current active dialog and resumes the state machine with the temporary 
   state.

SIDE EFFECTS:
    None

SEE ALSO:

=============================================================================*/
boolean SMSAgentApp_HandleNotifyEvent(SMSAgentApp *pMe, AEENotify *pN)
{
   AEECLSID              clsid = pN->cls;
   uint32                dwMask = pN->dwMask;
   
   #ifndef FEATURE_MULTIMODE_ARCH
   cm_call_info_s_type *pCallInfo;
   #else
   cm_mm_call_info_s_type *pCallInfo;
   #endif

   SMSApp * sms_app = (SMSApp *)pMe;	
   if(!pMe)
   {
	  DBGPRINTF("SMSAgentApp_HandleNotifyEvent: pMe doesn't exist");
      return FALSE;
   }

   if(clsid == AEECLSID_PHONE)
   {
	  // check to see that we're handling the call info status event
      if(dwMask & NMASK_PH_CALL_INFO)
      {
		 #ifndef FEATURE_MULTIMODE_ARCH
         pCallInfo = (cm_call_info_s_type*)pN->pData;
         #else
		 pCallInfo = (cm_mm_call_info_s_type*)pN->pData;
		 #endif

         if( (pCallInfo != NULL) &&
             (pCallInfo->call_type == CM_CALL_TYPE_SMS) )
         {
			switch(pCallInfo->call_state)
			{
               case CM_CALL_STATE_INCOM:
				  ISHELL_SendEvent(sms_app->a.m_pIShell,AEECLSID_APP_GSM1XSMS,
                                   EVT_CALL_STATE,CM_CALL_STATE_INCOM,0);
                  break;
               case CM_CALL_STATE_IDLE:
				  break;
               case CM_CALL_STATE_ORIG:
				  break;
               case CM_CALL_STATE_CONV:
               default:
                  // do nothing
				  break;
            }
         }
		 else if (pCallInfo == NULL)
			DBGPRINTF("SMSAgentApp_HandleNotifyEvent:pCallInfo=NULL");
		 else if (pCallInfo->call_type != CM_CALL_TYPE_SMS)
		 {
			 DBGPRINTF("CALL TYPE = %d", pCallInfo->call_type);
			 DBGPRINTF("CALL State = %d", pCallInfo->call_state);
		 }
      }

   }

   return TRUE;
}

/*=============================================================================
FUNCTION: GSM1xSMSApp_InitAppData

DESCRIPTION:
      Initializes all the SMS applet structure data to their default values.

PARAMETERS:
       pMe [in]: Pointer to the SMS Applet structure.

RETURN VALUE:
    EFAILED          : if failed to init the app
    SUCCESS          : if everything goes as expected

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int SMSAgentApp_InitAppData
(
   IApplet *po                                  // Application data
)

{
   SMSAgentApp *pMe = (SMSAgentApp *)po;        // Pointer to application data
   pMe->m_pIPhone = NULL;

   if(!pMe)
   {
      return EFAILED;
   }

   //Create an IPHONE Instance.
   if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_PHONE,
                            (void **)&pMe->m_pIPhone) != SUCCESS)
   {
      // Some thing went wrong and we are about to return. Do some cleanup
      // and release the resources, if they are allocated.
      DBGPRINTF("Failed to create AEECLSID_PHONE");
      SMSAgentApp_FreeAppData(po);
      return EFAILED;
   }

   return SUCCESS;
}

/*=============================================================================
FUNCTION: GSM1xSMSApp_FreeAppData

DESCRIPTION:

PARAMETERS:
   *pMe [in]: Pointer to the SMS Applet structure

RETURN VALUE:
    None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void SMSAgentApp_FreeAppData(IApplet *po)
{
   SMSAgentApp *pMe;

   pMe = (SMSAgentApp *)po;
   if(!pMe)
   {
      return;
   }

   if(pMe->m_pIPhone)
   {
      IPHONE_Release(pMe->m_pIPhone);
      pMe->m_pIPhone = NULL;
   }
}

