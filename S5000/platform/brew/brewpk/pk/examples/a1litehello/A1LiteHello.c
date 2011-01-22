/*===========================================================================

FILE: A1LiteHello.c
  
SERVICES: Sample applet using AEE
    
DESCRIPTION
   This file contains a very simple sample application that displays the
   "Hello World" on the display using A1Lite APIs.
      
PUBLIC CLASSES:  
   N/A
        
INITIALIZATION AND SEQUENCING REQUIREMENTS:
   The following explanation applies to this sample containing one applet which serves
   as a base for app developers to create their own applets using AEE Services:
   

   In the applet source file (like this one), include A1LiteAppGen.h.

   Mandatory Sections in Applet Source (this file):
   -----------------------------------------------
   Following Mandatory Sections are required for each applet source file. 
   (Search for "Mandatory" to identify these sections)

   Includes:     
      Copy this section as-is from the sample applet. It contains:
      A1LiteAppGen.h: For AEEApplet declaration

   Type Declarations:
		A data structure is usually defined to hold the app specific data. In this structure,
		the first element must be of type AEEApplet.  Note that this simple example does 
      not require any additional data, so this step has been removed.

   Functions: (For more details, see corresponding function description in this applet)

      App_HandleEvent(): This function is the Event Handler for the applet.
                         Copy the function outline from the sample applet and add app
                         specific code.

      A1LiteClsCreateInstance(): This function is called to create an instance of the applet.
                              It is called by A1LiteModGen when applet is being created.

   
   Important Notes:
   ---------------
   1. DO NOT use any "static data" in the applet. Always use the functions exported by
	   AEEStdlib or by IHeap services to dynamically allocate data and make it a member of 
		the applet structure. 
   2. DO NOT include and link "standard C library". Use AEE Memory Services (in AEEHeap.h) and Standard Library macros(in AEEStdLib.h).
      For example, use MALLOC() to allocate memory, WSTRCPY() to make a copy of Unicode (wide) string.
   3. BREW is Unicode(wide string) compliant ONLY (no ISOLATIN1/ANSI) except for file names which are ISOLATIN1/ANSI.
      ALWAYS USE AECHAR instead of "char". Use string services provided in AEEStdLib.h for string manipulation.
   4. It is always a good idea to DEFINE RESOURCES using BREW ResourceEditor. Make Strings, Bitmaps, Dialogs, etc. 
      as resources. ResourceEditor saves resources as .bri file, generates resource header file 
      and compiles .bri into a .bar binary file, which can be used by the applet.

   Miscellanoeus Notes:
   -------------------
   1. Make sure that the class ID used for the app is the same as that defined corresponding in .MIF file
   2. Always make sure that compiled resource (.bar) file and corresponding
      resource header (a) reside in app directory and (b) are included in the applet code.
      Define a constant APP_RES_FILE containing the name of the compiled resource file (with .bar extension).

   More than one applet:
   --------------------
   If more than one applet needs to be defined, then do the following
   (1) Follow the above description for each applet




   	   Copyright © 2000-2007 QUALCOMM Incorporated.
	                  All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/


/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "A1LiteAppGen.h"        // Applet helper file
#include "A1LiteHello.bid"		// Applet-specific header that contains class ID
#include "AEEStdLib.h"
#include "nmdef.h"
#include "AEEISignalCBFactory.h"
#include "AEESignalCBFactory.bid"
#include "AEEISignalBus.h"
#include "AEESignalBus.bid"
#include "AEEIEnv.h"
#include "AEEEnv.bid"
typedef struct SignalApplet SignalApplet;
struct SignalApplet {
   AEEApplet app;
   ISignalCBFactory *m_piSigCBFactory;
   ISignal *m_piSig1;
   ISignalCtl *m_piSig1Ctl;
   ISignal *m_piSig2;
   ISignalCtl *m_piSig2Ctl;
   ISignal *m_piSig3;
   ISignalCtl *m_piSig3Ctl;
   ISignalBus *m_piSignalBus;
   unsigned uState;
};


/*-------------------------------------------------------------------
Static function prototypes
-------------------------------------------------------------------*/
static boolean A1LiteHello_HandleEvent(AEEApplet * pme, AEEEvent eCode,uint16 wParam, uint32 dwParam);

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */
/*===========================================================================

FUNCTION: A1LiteClsCreateInstance

DESCRIPTION
	This function is invoked while the app is being loaded. All Modules must provide this 
	function. Ensure to retain the same name and parameters for this function.
	In here, the module must verify the ClassID and then invoke the A1LiteApplet_New() function
	that has been provided in A1LiteAppGen.c. 

   After invoking A1LiteApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to InitAppData(). This is done as follows:
   InitAppData() is called to initialize AppletData instance. It is app developers 
   responsibility to fill-in app data initialization code of InitAppData(). App developer
   is also responsible to release memory allocated for data contained in AppletData -- this can be
   done in FreeAppData().

PROTOTYPE:
	int AEEAppCreateInstance(AEECLSID clsID, IShell* pIShell, IModule* pIModule,IApplet** ppApplet)

PARAMETERS:
	clsID: [in]: Specifies the ClassID of the applet which is being loaded

	pIShell: [in]: Contains pointer to the IShell interface. 

	pIModule: pin]: Contains pointer to the IModule interface to the current module to which
	this app belongs

	ppApplet: [out]: On return, *ppApplet must point to a valid AEEApplet structure. Allocation
	of memory for this structure and initializing the base data members is done by A1LiteApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  SUCCESS: If the app needs to be loaded and if A1LiteApplet_New() invocation was successful
  EFAILED: If the app does not need to be loaded or if errors occurred in A1LiteApplet_New().
  If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/
int A1LiteClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * pMod,IEnv *piEnv, void ** ppObj)
{
   *ppObj = NULL;
		
   if(A1LiteApplet_New( sizeof(SignalApplet),               // Size of our private class
                     ClsId,                              // Our class ID
                     pIShell,                            // Shell interface
                     pMod,                               // Module instance
                     piEnv,                              // Env instance
                     (IApplet**)ppObj,                   // Return object
                     (AEEHANDLER)A1LiteHello_HandleEvent, // Our event handler
                     NULL))                              // No special "cleanup" function
         return(AEE_SUCCESS);

	return (EFAILED);
}

/*===========================================================================

FUNCTION A1LiteHello_HandleEvent
  
DESCRIPTION
   This is the EventHandler for this app. All events to this app are handled in this
   function. All APPs must supply an Event Handler.  

   Note - The switch statement in the routine is to demonstrate how event handlers are 
   generally structured, and to use ISignal and ISignalBus in a demonstrative way.
   This example admittedly uses ISignal and ISignalBus in a very contrived way,
   but with the intention of showing how the signal classes can be used.

   Also note that the use of "szText" below is provided only for demonstration purposes.  As 
   indicated in the documentation, a more practical approach is to load text resources
   from the applicaton's resource file.

   Finally, note that the ONLY event that an applet must process is EVT_APP_START.  Failure
   to return TRUE from this event will indicate that the app cannot be started and BREW
   will close the applet.
    
PROTOTYPE:
   boolean A1LiteHello_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)
      
PARAMETERS:
   pi: Pointer to the AEEApplet structure. This structure contains information specific
   to this applet. It was initialized during the AppCreateInstance() function.
        
   ecode: Specifies the Event sent to this applet
          
   wParam, dwParam: Event specific data.
            
DEPENDENCIES
   none
              
RETURN VALUE
   TRUE: If the app has processed the event
   FALSE: If the app did not process the event
                
SIDE EFFECTS
   none
===========================================================================*/

static void A1LiteHello_UpdateDisplay(SignalApplet *pApp)
{
   AECHAR aDone[] = {
      'T','e','s','t',' ',
      'P','a','s','s','e','d',
      0
   };
   AECHAR aText[6];

   aText[0] = 'S';
   aText[1] = 'i';
   aText[2] = 'g';
   aText[3] = '0'+(((pApp->uState)/10)%10);
   aText[4] = '0'+(((pApp->uState)/ 1)%10);
   aText[5] = 0;

   IDISPLAY_DrawText(pApp->app.m_pIDisplay,
                     AEE_FONT_BOLD,
                     pApp->uState ? aText : aDone,
                     -1,
                     0,
                     0,
                     NULL,
                     IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);

   IDISPLAY_Update(pApp->app.m_pIDisplay);
}

/*
||  Tests:
||   Create Sig1, Sig2, Sig3, SignalBus, add Sig2 to SignalBus
||    as OnEnable signal.
||
||   Sig01 -- set Sig1, see that Sig1 fires
||   Sig02 -- add Sig3 to SignalBus, see that Sig2 fires
||   Sig03 -- strobe SignalBus, see that Sig3 fires
||   Sig04 -- set Sig1 using Ctl, see that Sig1 fires
||   Sig05 -- re-enable Sig2 & Sig3, see that Sig2 fires
||   Sig06 -- strobe SignalBus, see that Sig3 fires
||   Sig07 -- remove Sig3 from SignalBus,
||            set SignalBus,
||            re-enable Sig1,
||            add Sig1 to SignalBus, see that Sig1 fires
||   Sig08 -- re-enable Sig3 & Sig2, see that Sig2 fires
||   Sig09 -- add Sig3 to SignalBus, see that Sig3 fires
||   Sig10 -- re-enable Sig1, see that Sig1 fires
||   Sig11 -- re-enable Sig2, see that Sig2 fires
||   Sig12 -- re-enable Sig3, see that Sig3 fires
||   Sig13 -- delete SignalBus, create a new one in its place;
||            release Sig2 Ctl and Sig2, create new ones;
||            attach Sig2 to SignalBus, enable Sig2 & Sig1;
||            add Sig1 to SignalBus as OnEnable signal,
||            see that Sig1 fires
||   Sig14 -- strobe SignalBus, see that Sig2 fires
||   Sig15 -- remove Sig2 from SignalBus, enable Sig3 and add Sig3 to
||            SignalBus, strobe SignalBus, see that Sig3 fires
||   Sig16 -- enable Sig1 and see that Sig1 fires
||
*/

static void A1LiteHello_Sig1Handler(void *pCx)
{
   SignalApplet *pApp = (SignalApplet *)pCx;

   if (pApp->uState == 0)
      return;

   A1LiteHello_UpdateDisplay(pApp);

   switch (pApp->uState) {
   case 1:
      pApp->uState = 2;
      ISignalBus_Add(pApp->m_piSignalBus, pApp->m_piSig3);
      break;
   case 4:
      pApp->uState = 5;
      ISignalCtl_Enable(pApp->m_piSig2Ctl);
      ISignalCtl_Enable(pApp->m_piSig3Ctl);
      break;
   case 7:
      pApp->uState = 8;
      ISignalCtl_Enable(pApp->m_piSig3Ctl);
      ISignalCtl_Enable(pApp->m_piSig2Ctl);
      break;
   case 10:
      pApp->uState = 11;
      ISignalCtl_Enable(pApp->m_piSig2Ctl);
      break;
   case 13:
      pApp->uState = 14;
      ISignalBus_Strobe(pApp->m_piSignalBus);
      break;
   case 16:
      pApp->uState = 0;
      A1LiteHello_UpdateDisplay(pApp);
      break;
   default:
      pApp->uState += 20;
      A1LiteHello_UpdateDisplay(pApp);
      pApp->uState = 0;
      break;
   }
}

static void A1LiteHello_Sig2Handler(void *pCx)
{
   SignalApplet *pApp = (SignalApplet *)pCx;

   if (pApp->uState == 0)
      return;

   A1LiteHello_UpdateDisplay(pApp);

   switch (pApp->uState) {
   case 2:
      pApp->uState = 3;
      ISignalBus_Strobe(pApp->m_piSignalBus);
      break;
   case 5:
      pApp->uState = 6;
      ISignalBus_Strobe(pApp->m_piSignalBus);
      break;
   case 8:
      pApp->uState = 9;
      ISignalBus_Add(pApp->m_piSignalBus, pApp->m_piSig3);
      break;
   case 11:
      pApp->uState = 12;
      ISignalCtl_Enable(pApp->m_piSig3Ctl);
      break;
   case 14:
      pApp->uState = 15;
      ISignalBus_Remove(pApp->m_piSignalBus, pApp->m_piSig2);
      ISignalCtl_Enable(pApp->m_piSig3Ctl);
      ISignalBus_Add(pApp->m_piSignalBus, pApp->m_piSig3);
      ISignalBus_Strobe(pApp->m_piSignalBus);
      break;
   default:
      pApp->uState += 40;
      A1LiteHello_UpdateDisplay(pApp);
      pApp->uState = 0;
      break;
   }
}

static void A1LiteHello_Sig3Handler(void *pCx)
{
   SignalApplet *pApp = (SignalApplet *)pCx;

   if (pApp->uState == 0)
      return;

   A1LiteHello_UpdateDisplay(pApp);

   switch (pApp->uState) {
   case 3:
      pApp->uState = 4;
      ISignalCtl_Set(pApp->m_piSig1Ctl);
      break;
   case 6:
      pApp->uState = 7;
      ISignalBus_Remove(pApp->m_piSignalBus, pApp->m_piSig3);
      ISignalBus_Set(pApp->m_piSignalBus);
      ISignalCtl_Enable(pApp->m_piSig1Ctl);
      ISignalBus_Add(pApp->m_piSignalBus, pApp->m_piSig1);
      break;
   case 9:
      pApp->uState = 10;
      ISignalCtl_Enable(pApp->m_piSig1Ctl);
      break;
   case 12:
      pApp->uState = 13;
      ISignalBus_Release(pApp->m_piSignalBus);
      pApp->m_piSignalBus = NULL;
      if (SUCCESS != IEnv_CreateInstance(pApp->app.m_piEnv,
                                         AEECLSID_SignalBus,
                                         (void **)&pApp->m_piSignalBus)) {
         pApp->uState = 81;
         A1LiteHello_UpdateDisplay(pApp);
         pApp->uState = 0;
         break;
      }
      ISignalCtl_Release(pApp->m_piSig2Ctl);
      pApp->m_piSig2Ctl = NULL;
      ISignal_Release(pApp->m_piSig2);
      pApp->m_piSig2 = NULL;
      if (SUCCESS != ISignalCBFactory_CreateSignal(pApp->m_piSigCBFactory,
                                                   A1LiteHello_Sig2Handler,
                                                   (void *)pApp,
                                                   &pApp->m_piSig2,
                                                   &pApp->m_piSig2Ctl)) {
         pApp->uState = 82;
         A1LiteHello_UpdateDisplay(pApp);
         pApp->uState = 0;
         break;
      }
      ISignalBus_Add(pApp->m_piSignalBus, pApp->m_piSig2);
      ISignalCtl_Enable(pApp->m_piSig2Ctl);
      ISignalCtl_Enable(pApp->m_piSig1Ctl);
      ISignalBus_OnEnable(pApp->m_piSignalBus, pApp->m_piSig1);
      break;
   case 15:
      pApp->uState = 16;
      ISignalCtl_Enable(pApp->m_piSig1Ctl);
      break;
   default:
      pApp->uState += 60;
      A1LiteHello_UpdateDisplay(pApp);
      pApp->uState = 0;
      break;
   }
}

static boolean A1LiteHello_HandleEvent(AEEApplet * pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
   SignalApplet *pApp = (SignalApplet *)pMe;

   switch (eCode){
      case EVT_APP_START:                        
         if (SUCCESS != IEnv_CreateInstance(pApp->app.m_piEnv,
                                            AEECLSID_SignalCBFactory,
                                            (void **)&pApp->m_piSigCBFactory)) {
            AECHAR szText[] = {'A','1','L','i','t','e',' ',
                               'n','o','t',' ',
                               'p','r','e','s','e','n','t','\0'};

            IDISPLAY_DrawText(pMe->m_pIDisplay,
                              AEE_FONT_BOLD,
                              szText,
                              -1,
                              0,
                              0,
                              NULL,
                              IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
            IDISPLAY_Update (pMe->m_pIDisplay);
            return TRUE;
         }

         /* Do some stuff just to exercise the Env */

         if (SUCCESS != ISignalCBFactory_CreateSignal(pApp->m_piSigCBFactory,
                                                      A1LiteHello_Sig1Handler,
                                                      (void *)pApp,
                                                      &pApp->m_piSig1,
                                                      &pApp->m_piSig1Ctl)) {
            ISignalCBFactory_Release(pApp->m_piSigCBFactory);
            return FALSE;
         }

         if (SUCCESS != ISignalCBFactory_CreateSignal(pApp->m_piSigCBFactory,
                                                      A1LiteHello_Sig2Handler,
                                                      (void *)pApp,
                                                      &pApp->m_piSig2,
                                                      &pApp->m_piSig2Ctl)) {
            ISignalCtl_Release(pApp->m_piSig1Ctl);
            ISignal_Release(pApp->m_piSig1);
            ISignalCBFactory_Release(pApp->m_piSigCBFactory);
            return FALSE;
         }

         if (SUCCESS != ISignalCBFactory_CreateSignal(pApp->m_piSigCBFactory,
                                                      A1LiteHello_Sig3Handler,
                                                      (void *)pApp,
                                                      &pApp->m_piSig3,
                                                      &pApp->m_piSig3Ctl)) {
            ISignalCtl_Release(pApp->m_piSig2Ctl);
            ISignal_Release(pApp->m_piSig2);
            ISignalCtl_Release(pApp->m_piSig1Ctl);
            ISignal_Release(pApp->m_piSig1);
            ISignalCBFactory_Release(pApp->m_piSigCBFactory);
            return FALSE;
         }

         if (SUCCESS != IEnv_CreateInstance(pApp->app.m_piEnv,
                                            AEECLSID_SignalBus,
                                            (void **)&pApp->m_piSignalBus)) {
            ISignalCtl_Release(pApp->m_piSig3Ctl);
            ISignal_Release(pApp->m_piSig3);
            ISignalCtl_Release(pApp->m_piSig2Ctl);
            ISignal_Release(pApp->m_piSig2);
            ISignalCtl_Release(pApp->m_piSig1Ctl);
            ISignal_Release(pApp->m_piSig1);
            ISignalCBFactory_Release(pApp->m_piSigCBFactory);
            return FALSE;
         }

         ISignalBus_OnEnable(pApp->m_piSignalBus, pApp->m_piSig2);

         pApp->uState = 1;

         ISignal_Set(pApp->m_piSig1);

			return(TRUE);

      case EVT_APP_STOP:
         if (pApp->m_piSignalBus) {
            ISignalBus_Release(pApp->m_piSignalBus);
         }
         if (pApp->m_piSig3Ctl) {
            ISignalCtl_Detach(pApp->m_piSig3Ctl);
            ISignalCtl_Release(pApp->m_piSig3Ctl);
         }
         if (pApp->m_piSig3)
            ISignal_Release(pApp->m_piSig3);
         if (pApp->m_piSig2Ctl) {
            ISignalCtl_Detach(pApp->m_piSig2Ctl);
            ISignalCtl_Release(pApp->m_piSig2Ctl);
         }
         if (pApp->m_piSig2)
            ISignal_Release(pApp->m_piSig2);
         if (pApp->m_piSig1Ctl) {
            ISignalCtl_Detach(pApp->m_piSig1Ctl);
            ISignalCtl_Release(pApp->m_piSig1Ctl);
         }
         if (pApp->m_piSig1)
            ISignal_Release(pApp->m_piSig1);
         if (pApp->m_piSigCBFactory)
            ISignalCBFactory_Release(pApp->m_piSigCBFactory);
         return(TRUE);


      default:
         break;
   }
   return(FALSE);
}


