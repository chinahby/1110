/*=====================================================================
FILE: ActivationApp.c

GENERAL DESCRIPTION:
   Implementation of BREW-required functions and GUI
   for the GSM1x Activation Applet.

IMPLEMENTATION-SPECIFIC COMMENTS:
   None. 
       

              (c) COPYRIGHT 2002, 2003 QUALCOMM Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
======================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEFile.h"            // File interface definitions
#include "AEEDB.h"              // Database interface definitions
#include "AEENet.h"             // Socket interface definitions
#include "AEESound.h"           // Sound Interface definitions
#include "AEETAPI.h"            // TAPI Interface definitions
#include "AEEMenu.h"            // Menu Services
#include "AEEStdLib.h"          // Helper Functions
#include "AEEDisp.h"


#if defined(BREW_STATIC_APP)
#include "OEMClassIDs.h"        // 
#else
#include "gsm1xactv.bid"        // ClassId Information For Static app included in OEMClassID.h
#endif /* BREW_STATIC_APP */


#include "gsm1xactv_res.h"  // Resource File

#include "AEEGSM1xControl.h"    // IGSM1x_Control Interface
#include "AEEGSM1xSig.h"        // IGSM1x_Control Interface

#include "gsm1xactvapi.h"
#include "gsm1xactvprl.h"



/*===============================================================================
GLOBAL DEFINITIONS
=============================================================================== */
#define GSM1XACTV_VERSION  1
#define GSM1XACTV_REVISION 0


#define GSM1XACTV_STATE_UNINITIALIZED    (-1)
#define GSM1XACTV_STATE_GSM1x            (0)
#define GSM1XACTV_STATE_1x               (1)

#define GSM1XACTV_ACTIVATE_GSM1X         (100)
#define GSM1XACTV_ACTIVATE_1X            (101)
#define GSM1XACTV_SHOW_ABOUT_INFO        (102)

/* Size of buffer used to hold text strings*/
#define TEXT_BUFFER_SIZE                 (100 * sizeof(AECHAR))

/* Time for delaying destruction of the applet.*/                                         
#define DESTRUCTION_DELAY_MILLISEC       (2500)



/*===============================================================================
STRUCTURE DEFINITIONS
=============================================================================== */
typedef struct _gsm1xactvStruct
{
     AEEApplet                          appletObj;
     
     IMenuCtl                          *m_pIMenu;
     IGSM1xControl                     *m_pIGSM1xControl;
     int                                m_iMobileState;
     AEEGSM1xControl_AppActionType      m_initialCommandFromDecisionMaker;

     /* The following variables are used internally.
        Putting them here takes care of their memory allocation.*/
     gsm1xactvprl_LargeDataBuffers  m_largeData;

} gsm1xactvStruct;


/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static boolean gsm1xactv_HandleEvent
(
    gsm1xactvStruct  *pMe, 
    AEEEvent          eCode, 
    uint16            wParam, 
    uint32            dwParam
);

static boolean gsm1xactv_InitAppData
(
    IApplet         *pi
);

static void    gsm1xactv_FreeAppData
(
    IApplet         *pi
);

static void        buildMenu
(
    gsm1xactvStruct                 *pMe,
    AEEGSM1xControl_modeBitMaskType  availModes
);

static void    setAnnunciator
( 
    gsm1xactvStruct  *appData,
    boolean           IsSuccess,
    boolean           IsGSM1x
);


static void    delayedDestructionCallback
(
    void            *userData 
);

static boolean ActivateGSM1x
(
    gsm1xactvStruct  *pMe
);

static void    showMessage
(
    gsm1xactvStruct  *pMe,
    uint8             message1,
    uint8             message2,
    uint8             message3
);


#if defined(BREW_STATIC_APP)

int         gsm1xactv_CreateInstance (AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj);
static int  gsm1xactv_ModLoad(IShell *ps, void * pHelpers, IModule ** pMod);


//----------------------------------------------------------
//   Local Data Declarations
//----------------------------------------------------------
static const AEEAppInfo gaiActvApp = 
{
   AEECLSID_GSM1XACTV,
   GSM1XACTV_RES_FILE,
   IDS_NAME,
   0,
   0,
   0,
   0,
   AFLAG_TOOL
};

//----------------------------------------------------------
//   Public function definitions
//----------------------------------------------------------
static void gsm1xactv_RegisterForEvents
(
    IShell *pIShell
)
{
    if(
         AEE_SUCCESS != ISHELL_RegisterNotify
                        (
                            pIShell, 
                            AEECLSID_GSM1XACTV,
                            AEECLSID_IGSM1XCONTROLNOTIFIER, 
                            NMASK_GSM1xCONTROL_ACTIVATION
                        )
      ){
        DBGPRINTF("Registered for notification.");
    }
    else{
        DBGPRINTF("Could not register for notification.");
    }
    return;
}


PFNMODENTRY gsm1xactv_GetModInfo
(
   IShell      *ps,
   AEECLSID   **ppClasses,
   AEEAppInfo **pApps,
   uint16      *pnApps,
   uint16      *pwMinPriv
)
{
   *ppClasses = NULL;
   *pApps  = (AEEAppInfo *)&gaiActvApp;
   *pnApps = 1;
   gsm1xactv_RegisterForEvents(ps);
   return((PFNMODENTRY)gsm1xactv_ModLoad);
}


static int gsm1xactv_ModLoad
(
   IShell   *ps, 
   void     *pHelpers, 
   IModule **pMod
)
{
   int retval;

   retval = AEEStaticMod_New
      ((int16)(sizeof(AEEMod)),
       ps,
       pHelpers,
       pMod,
       gsm1xactv_CreateInstance,
       NULL);
   return retval;
}
#endif  /*BREW_STATIC_APP*/



/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */



/*=============================================================================
FUNCTION: gsm1xactv_CreateInstance or  AEEClsCreateInstance

DESCRIPTION:
   Instantiate a new instance of GSM1x Activation Applet.    

RETURN VALUE:
   BREW competion status
=============================================================================*/
#if defined(BREW_STATIC_APP)
int gsm1xactv_CreateInstance
#else
int AEEClsCreateInstance
#endif /*BREW_STATIC_APP*/
(
    AEECLSID   ClsId,
    IShell    *pIShell,
    IModule   *po,
    void     **ppObj
)
{
   *ppObj = NULL;
          
   if(ClsId == AEECLSID_GSM1XACTV){
      
       if(
            TRUE == AEEApplet_New
                    (
                       sizeof(gsm1xactvStruct), 
                       ClsId, 
                       pIShell,
                       po,
                       (IApplet**)ppObj,
                       (AEEHANDLER)gsm1xactv_HandleEvent,
                       (PFNFREEAPPDATA)gsm1xactv_FreeAppData
                    )
         ){
            
           if (gsm1xactv_InitAppData((IApplet*)*ppObj)){
               return AEE_SUCCESS;
           }
       }
   
   }
     
   return EFAILED;
}


/*=============================================================================
FUNCTION: gsm1xactv_InitAppData 

DESCRIPTION:
   Initialize GSM1x Activation Applet class data.   

RETURN VALUE:
   boolean competion status
=============================================================================*/
static boolean gsm1xactv_InitAppData
(
    IApplet* pi
)
{
   gsm1xactvStruct    *pMe = (gsm1xactvStruct*)pi;
   IGSM1xControl     *gsm1xControlInstancePtr = NULL;
   int iResult;

   /* Make sure the pointers we'll be using are valid.*/
   if (pMe == NULL || pMe->appletObj.m_pIShell == NULL){
       return FALSE;
   }

   if(
        SUCCESS != ISHELL_CreateInstance
                   ( 
                      pMe->appletObj.m_pIShell, 
                      AEECLSID_MENUCTL, 
                      (void **)&pMe->m_pIMenu 
                   )
     ){
       return FALSE;
   }


   iResult = ISHELL_CreateInstance
             ( 
                 pMe->appletObj.m_pIShell, 
                 AEECLSID_IGSM1XCONTROL,
                 (void **)&gsm1xControlInstancePtr   
             );
                                     
   if(SUCCESS != iResult){
       DBGPRINTF("Unable to create GSM1XCONTROL instance, %d", iResult);
       return FALSE;
   }
   else{
       pMe->m_pIGSM1xControl = gsm1xControlInstancePtr;
       DBGPRINTF("Created GSM1XCONTROL instance");
   }

   // Set the mobile setat to un-initialized
   pMe->m_iMobileState = GSM1XACTV_STATE_UNINITIALIZED;

   pMe->m_initialCommandFromDecisionMaker = AEEGSM1XCONTROL_ACTION_RESERVED;

   return TRUE;
}


/*=============================================================================
FUNCTION: gsm1xactv_FreeAppData 

DESCRIPTION:
   Free GSM1x Activation Applet data.    

RETURN VALUE:
   boolean competion status
=============================================================================*/
static void gsm1xactv_FreeAppData
(
    IApplet* pi
)
{
    gsm1xactvStruct *pMyApp = (gsm1xactvStruct*) pi;

    if(NULL != pMyApp->m_pIMenu){
        IMENUCTL_Release(pMyApp->m_pIMenu);
    }

    if(NULL != pMyApp->m_pIGSM1xControl){
        IGSM1xControl_Release(pMyApp->m_pIGSM1xControl);
    }
}


/*=============================================================================
FUNCTION: delayedDestructionCallback 

DESCRIPTION:
   Close the applet after a certain period of time.    

RETURN VALUE:
   N/A
=============================================================================*/
static void  delayedDestructionCallback
( 
    void *userData
)
{
    gsm1xactvStruct *pMe = (gsm1xactvStruct*) userData;

    ISHELL_CloseApplet( pMe->appletObj.m_pIShell, FALSE);
}



/*=============================================================================
FUNCTION: ActivateGSM1x 

DESCRIPTION:
   Activate GSM1x mode, emit message/update annuciator, and 
   then set the timer to close the applet.    

RETURN VALUE:
   N/A
=============================================================================*/
static boolean  ActivateGSM1x
( 
    gsm1xactvStruct  *pMe
)
{
    uint32  status;

    status = gsm1xactvapi_ActivateGSM1xMode( pMe->m_pIGSM1xControl, &(pMe->m_largeData) );

    if(GSM1XACTVAPI_SUCCESS == status){
        showMessage(pMe, 0, IDS_GSM1X, IDS_ACTIVATED);
                         
        setAnnunciator( pMe, TRUE, TRUE );
        
    }
    else{
        showMessage(pMe, 0, IDS_GSM1X, IDS_ERROR);
                         
        setAnnunciator( pMe, FALSE, TRUE );
    }
    
    return (GSM1XACTVAPI_SUCCESS == status) ? TRUE : FALSE;
}


/*=============================================================================
FUNCTION: showMessage 

DESCRIPTION:
   Show the message specified by modeId and StatusId on the UI Display.    

RETURN VALUE:
   N/A
=============================================================================*/
static void  showMessage
(
    gsm1xactvStruct *pMe,
    uint8            message1,
    uint8            message2,
    uint8            message3 
)
{
#define MESSAGE_LINE_CNT     (3)
#define MESSAGE_BUF_MAX_LEN  (50)

    AECHAR wBuf [MESSAGE_BUF_MAX_LEN];
    AECHAR wBuf2[MESSAGE_BUF_MAX_LEN];
    AECHAR *curBuf;
    int    msgIndex;
    int16  msgIds[MESSAGE_LINE_CNT];

    msgIds[ 0 ] = message1;
    msgIds[ 1 ] = message2;
    msgIds[ 2 ] = message3;
   

    // Clear Screen
    IDISPLAY_ClearScreen(pMe->appletObj.m_pIDisplay);


    for( msgIndex = 0; msgIndex < MESSAGE_LINE_CNT; msgIndex++ )
    {
        if( 0 != msgIds[msgIndex] ){          
           if(
                ISHELL_LoadResString 
                (
                   pMe->appletObj.m_pIShell, 
                   GSM1XACTV_RES_FILE,
                   msgIds[msgIndex], 
                   wBuf, 
                   sizeof(wBuf)
                )
             ){  

                curBuf = wBuf;
                
                /* Get the version and revision.*/
                if(IDS_VERSION == msgIds[msgIndex]){
                    WSPRINTF(wBuf2, sizeof(wBuf2), wBuf, GSM1XACTV_VERSION, GSM1XACTV_REVISION);
                    curBuf = wBuf2;
                }
                
                IDISPLAY_DrawText
                (
                   pMe->appletObj.m_pIDisplay,
                   AEE_FONT_BOLD, 
                   curBuf, 
                   -1, 0, 20 * msgIndex, NULL,
                   IDF_ALIGN_CENTER 
                );
           }
        }
    }
    
    IDISPLAY_Update(pMe->appletObj.m_pIDisplay);
}
                        

/*=============================================================================
FUNCTION: gsm1xactv_HandleEvent 

DESCRIPTION:
   Process events sent by BREW.    

RETURN VALUE:
   boolean completion status
       TRUE: If the app has processed the event
       FALSE: If the app did not process the event
=============================================================================*/
static boolean gsm1xactv_HandleEvent
(
    gsm1xactvStruct *pMe, 
    AEEEvent         eCode, 
    uint16           wParam, 
    uint32           dwParam
)
{ 
    
    if(NULL != pMe->m_pIMenu){
       if (IMENUCTL_HandleEvent(pMe->m_pIMenu, eCode, wParam, dwParam)){
          return TRUE;
       }
    }
    
     
    switch (eCode){
      
       /*--------------------------------------------------------------------*/
       case EVT_APP_START: 
       /*--------------------------------------------------------------------*/

           {
               AEEGSM1xControl_modeBitMaskType  mode;
               boolean                          activationSuccess = FALSE;
                    
               if(NULL == pMe->m_pIGSM1xControl){
                   
                   DBGPRINTF("IGSM1xControl is not created!!");
                   setAnnunciator( pMe, activationSuccess, FALSE );

                   showMessage(pMe, 0, IDS_GSM1X, IDS_ERROR);
                   

                   /* Delay the destruction of the applet so the user has a chance
                      to read the message.*/
                   ISHELL_SetTimer( pMe->appletObj.m_pIShell, DESTRUCTION_DELAY_MILLISEC, delayedDestructionCallback, pMe);
                   return TRUE;
               }

               /* Get the available modes.  
                  If only one mode is available, do GSM1x provisioning and activation.
                  Otherwise, bring the app the menu.*/
               if( 
                   AEEGSM1XCONTROL_STATUS_SUCCESS !=  IGSM1xControl_GetAvailableModes
                                                      ( 
                                                          pMe->m_pIGSM1xControl, 
                                                         &mode 
                                                      ) 
                 ){
                   DBGPRINTF("Could not get available modes.");

                   showMessage(pMe, 0, IDS_GSM1X, IDS_ERROR);
                                   
                   setAnnunciator( pMe, activationSuccess, FALSE );
              
                   /* Delay the destruction of the applet so the user has a chance
                     to read the message.*/
                   ISHELL_SetTimer( pMe->appletObj.m_pIShell, DESTRUCTION_DELAY_MILLISEC, delayedDestructionCallback, pMe);
                   
                   return TRUE;
               }

               DBGPRINTF("Avail.modes=%d",(int)mode);
               

               switch(  pMe->m_initialCommandFromDecisionMaker ){
          
                   case  AEEGSM1XCONTROL_ACTION_PROVISION_GSM1X:
                       {

                           showMessage(pMe, 0, IDS_GSM1X, IDS_PROCESSING);
                           IDISPLAY_UpdateEx(pMe->appletObj.m_pIDisplay, FALSE );

                           if( ! ActivateGSM1x(pMe)){
                               DBGPRINTF("Failed to activate GSM1x mode");
                           }
                           else{
                               DBGPRINTF("Activated GSM1x mode");
                           }

                           ISHELL_SetTimer( pMe->appletObj.m_pIShell, DESTRUCTION_DELAY_MILLISEC, delayedDestructionCallback, pMe);
                           
                           return TRUE;

                           break;
                
                       }

             
                   case  AEEGSM1XCONTROL_ACTION_BRING_UP_GUI:
                   case  AEEGSM1XCONTROL_ACTION_RESERVED:
                   default:       
                       {
                           buildMenu(pMe, mode);
                           return TRUE;
                    
                           break;
                
                       }
               } /* switch */
               
               break;
        
           }


       /*--------------------------------------------------------------------*/
       case EVT_APP_STOP:
       /*--------------------------------------------------------------------*/
           {
               return TRUE;
               break;
           }

       /*--------------------------------------------------------------------*/
       case EVT_APP_SUSPEND:
       /*--------------------------------------------------------------------*/
           {
               // Set all controls to InActive
               if( NULL != pMe->m_pIMenu){
                  IMENUCTL_SetActive( pMe->m_pIMenu, FALSE );
               }
               return TRUE;
               break;
           }

        /*--------------------------------------------------------------------*/
       case EVT_APP_RESUME:
       /*--------------------------------------------------------------------*/
           {
               if( NULL != pMe->m_pIMenu){
                  IMENUCTL_SetActive( pMe->m_pIMenu, TRUE );
               }
               return TRUE;
               break;
           }

       /*--------------------------------------------------------------------*/
       case EVT_NOTIFY:
       /*--------------------------------------------------------------------*/    
           {
               AEENotify *notifyPtr;

               notifyPtr = (AEENotify *)dwParam;

               // If activation event received from notifier
               if(
                    (AEECLSID_IGSM1XCONTROLNOTIFIER == notifyPtr->cls) 
                    &&
                    (NMASK_GSM1xCONTROL_ACTIVATION == notifyPtr->dwMask)
                 ){
                   AEECLSID active;
                   int      temp32;

                   temp32 = (int)(notifyPtr->pData);

                   pMe->m_initialCommandFromDecisionMaker = (AEEGSM1xControl_AppActionType) temp32;

                   DBGPRINTF("DecisionMaker told %d",(int)(pMe->m_initialCommandFromDecisionMaker));
             
                   active = ISHELL_ActiveApplet(pMe->appletObj.m_pIShell); // Get the active app
                   if (AEECLSID_GSM1XACTV != active){      // If app has not been started
                   
                       int status;

                       status = ISHELL_StartApplet
                                (
                                    pMe->appletObj.m_pIShell, 
                                    AEECLSID_GSM1XACTV
                                );            // Start the app
               
                       if(SUCCESS != status){
                           DBGPRINTF("App start error");
                       }
                   }
                   return TRUE;
               }
          
               break;
        
           }

       /*--------------------------------------------------------------------*/
       case EVT_COMMAND:
       /*--------------------------------------------------------------------*/   
           {

              if(NULL != pMe->m_pIMenu){
                  IMENUCTL_Release(pMe->m_pIMenu);
                  pMe->m_pIMenu = NULL;
              }


              if( GSM1XACTV_ACTIVATE_GSM1X == wParam){
                 
                  if(NULL == pMe->m_pIGSM1xControl){
                      DBGPRINTF("IGSM1xControl is not created!!");
                      return TRUE;
                  }
  
                  if (GSM1XACTV_STATE_GSM1x == pMe->m_iMobileState){
                      return TRUE;
                  }
                  else{
                      pMe->m_iMobileState = GSM1XACTV_STATE_GSM1x;
                  }

                  showMessage(pMe, 0, IDS_GSM1X, IDS_PROCESSING);
                  IDISPLAY_UpdateEx(pMe->appletObj.m_pIDisplay, FALSE );


                  if( ! ActivateGSM1x(pMe)){
                               
                      DBGPRINTF("Failed to activate GSM1x mode");
                  }
                  else{
                      DBGPRINTF("Activated GSM1x mode");
                  }

                  ISHELL_SetTimer( pMe->appletObj.m_pIShell, DESTRUCTION_DELAY_MILLISEC, delayedDestructionCallback, pMe);
                                    
                  return TRUE;
              }
             
              else if(GSM1XACTV_ACTIVATE_1X == wParam){
              
                  boolean activationSuccess = FALSE;

                  if (pMe->m_pIGSM1xControl == NULL){
                      DBGPRINTF("IGSM1xControl is not created!!");
                      return TRUE;
                  }

                  if (GSM1XACTV_STATE_1x == pMe->m_iMobileState){
                      return TRUE;
                  }
                  else{
                      pMe->m_iMobileState = GSM1XACTV_STATE_1x;
                  }
                         
                  if(
                       AEEGSM1XCONTROL_STATUS_SUCCESS == IGSM1xControl_ActivateNonGSM1xMode
                                                         ( 
                                                            pMe->m_pIGSM1xControl, 
                                                            AEEGSM1XCONTROL_1X_RUIM_PROV_MASK 
                                                         )
                    ){
                      DBGPRINTF("Activated NONGSM1x mode (success).");
                      activationSuccess = TRUE;
                  }
                                                  
                  showMessage(pMe, 0, IDS_CDMA1X, IDS_ACTIVATED);

                  setAnnunciator( pMe, TRUE, FALSE );

                  ISHELL_SetTimer( pMe->appletObj.m_pIShell, DESTRUCTION_DELAY_MILLISEC, delayedDestructionCallback, pMe);

                  return TRUE;
                  
              } 

              else if(GSM1XACTV_SHOW_ABOUT_INFO == wParam){
                  
                  showMessage(pMe, IDS_NAME, IDS_VERSION, IDS_QUALCOMM);

                  ISHELL_SetTimer( pMe->appletObj.m_pIShell, DESTRUCTION_DELAY_MILLISEC, delayedDestructionCallback, pMe);
                                     
                  return TRUE;
              }

              break;
          }
        
      /*--------------------------------------------------------------------*/
      default:
      /*--------------------------------------------------------------------*/    
          {
              /* Do nothing.*/
              break;
          }
   
    } /* switch */
   
    /* If we got here, the event was not handled.*/
    return FALSE;
}


/*=============================================================================
FUNCTION: buildMenu 

DESCRIPTION:
   Build mode selection menu.    

RETURN VALUE:
   N/A
=============================================================================*/
static void     buildMenu
(
    gsm1xactvStruct                 *pMe,
    AEEGSM1xControl_modeBitMaskType  availModes
)
{
    AEERect          qrc;
    AEEDeviceInfo    di;

    AECHAR           wBuf[50];

     
    // Make sure the pointers we'll be using are valid
    if( 
         (NULL == pMe)
         || 
         (NULL == pMe->appletObj.m_pIShell)
         ||
         (NULL == pMe->m_pIMenu)
      ){
        return;
    }

    // Set Title
    if(
         0 == ISHELL_LoadResString 
              (
                 pMe->appletObj.m_pIShell, 
                 GSM1XACTV_RES_FILE,
                 IDS_MODE_SELECTION, 
                 wBuf, 
                 sizeof(wBuf)
              )
      ){
        DBGPRINTF("Could not load resource string for GSM1x");
        return;
    }
    IMENUCTL_SetTitle(pMe->m_pIMenu, NULL, 0, wBuf);

    // Set the Rectangle for the Menu
    ISHELL_GetDeviceInfo(pMe->appletObj.m_pIShell,&di);
    qrc.x = 0;
    qrc.y = 0;
    qrc.dx = di.cxScreen;
    qrc.dy = di.cyScreen;
    IMENUCTL_SetRect(pMe->m_pIMenu, &qrc);  

    // Add individual entries to the Menu
    if(0 != (availModes & AEEGSM1XCONTROL_GSM1X_PROV_MASK)){

       if(
            0 == ISHELL_LoadResString 
                 (
                    pMe->appletObj.m_pIShell, 
                    GSM1XACTV_RES_FILE,
                    IDS_GSM1X, 
                    wBuf, 
                    sizeof(wBuf)
                 )
         ){
           DBGPRINTF("Could not load resource string for GSM1x");
           return;
       }
       IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, GSM1XACTV_ACTIVATE_GSM1X, wBuf, 0);
    }

    if(0 != (availModes & AEEGSM1XCONTROL_1X_RUIM_PROV_MASK)){
    
        if(
             0 == ISHELL_LoadResString 
                  (
                     pMe->appletObj.m_pIShell, 
                     GSM1XACTV_RES_FILE,
                     IDS_CDMA1X, 
                     wBuf, 
                     sizeof(wBuf)
                  )
           ){
            DBGPRINTF("Could not load resource string for 1x");
            return;
        }
        IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, GSM1XACTV_ACTIVATE_1X, wBuf, 0);
    }

    /* Always add the about box.*/
    if(
          0 == ISHELL_LoadResString 
               (
                  pMe->appletObj.m_pIShell, 
                  GSM1XACTV_RES_FILE,
                  IDS_ABOUT, 
                  wBuf, 
                  sizeof(wBuf)
               )
      ){
            DBGPRINTF("Could not load resource string for about item");
            return;
    }
    IMENUCTL_AddItem(pMe->m_pIMenu, 0, 0, GSM1XACTV_SHOW_ABOUT_INFO, wBuf, 0);
    
    // Activate menu
    IMENUCTL_SetActive(pMe->m_pIMenu,TRUE);

}


/*=============================================================================
FUNCTION: setAnnunciator 

DESCRIPTION:
   Update the state of GSM1x annunciator given the newly attempted
   provisioning mode and the success/failure completion of mode change.
   
   Note that this annunciator is shared with "Voice Privacy" and, therefore,
   is an unreliable way to indicate GSM1x mode status.
   
RETURN VALUE:
   N/A
=============================================================================*/
static void    setAnnunciator
( 
    gsm1xactvStruct   *appData,
    boolean            IsSuccess,
    boolean            IsGSM1x
)
{
   uint16 AnnunciatorMask;
   uint16 annunciator;

#ifdef ANNUN_GSM1X
   // If GSM1x annunciator is available then use it
   annunciator = ANNUN_GSM1X;
#else
   // Otherwise, use some other annunciator
   annunciator = ANNUN_NET_LOCK;
#endif
   AnnunciatorMask = ( IsSuccess && IsGSM1x ) ? annunciator : 0;
    
    /* Update GSM1x annunciator state.*/
    IDISPLAY_SetAnnunciators
    (  
         appData->appletObj.m_pIDisplay, 
         annunciator, 
         AnnunciatorMask
    );
}
