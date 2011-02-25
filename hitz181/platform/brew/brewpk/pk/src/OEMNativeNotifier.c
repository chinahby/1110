/*===========================================================================

FILE: OEMNativeNotifier.c

SERVICES: Sample nativenotifier class using AEE

DESCRIPTION
  This file contains a class that implements the INativeNotifier interface.
  INativeNotifier interface implements services that allows a user applet to 
  register to be notified when the deviec enters / exits the IDLE screen.
  See ISHELL_RegisterNotify function descriptions for more information on 
  registering events with the shell.
  
PUBLIC CLASSES:  
   N/A

   	   Copyright © 2004-2005 QUALCOMM Incorporated.
	                  All Rights Reserved.
                   QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
                     INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */

#include "OEMFeatures.h"
#include "AEE_OEMDispatch.h"

#include "AEEModGen.h"
#include "AEEAppGen.h"
#include "AEEShell.h"
#include "AEEDisp.h"
#include "AEEStdLib.h"
#include "AEENativeNotifier.h"

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

#define MAX_NATIVENOTIFY      5

#define FARF_NATIVENOTIFY     0

#if defined(AEE_SIMULATOR)
#define FARF_ALWAYS  1
#else
#define FARF_ALWAYS  0
#endif

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p

#undef  NOTIFY_MULTIPLE_PENDING_EVENTS

/*-------------------------------------------------------------------
            Type Declarations
-------------------------------------------------------------------*/

// NativeNotifier class strucutre.
typedef struct _OEMNativeNotifierClass
{
   DECLARE_VTBL(INotifier)
   AEECLSID     clsID;          // ClassID of nativenotifier class

   uint32       m_nRefs;        // Class reference counter
   IShell    *  m_pIShell;      // pointer to IShell

   uint32       m_dwNotifyMask;
   uint32       m_dwNotifyBit[MAX_NATIVENOTIFY];
   byte         m_byNotifyCountIn;
   byte         m_byNotifyCountOut;

   AEECallback  m_cbNotify;
} OEMNativeNotifierClass;

/*-------------------------------------------------------------------
            Function Prototypes
-------------------------------------------------------------------*/

// NativeNotifier function prototypes
uint32  OEMNativeNotifierClass_AddRef(INotifier* po);
uint32  OEMNativeNotifierClass_Release(INotifier* po);
void    OEMNativeNotifierClass_Notify(uint32 dwBit);
void    OEMNativeNotifierClass_NotifyCallback(INotifier * po);
void    OEMNativeNotifierClass_SetMask(INotifier* po, uint32 * dwMask);
int     OEMNativeNotifierClass_New(IShell * pIShell, AEECLSID cls, void **ppobj);

/*-------------------------------------------------------------------
            Global Constant Definitions
-------------------------------------------------------------------*/
 
/*-------------------------------------------------------------------
            Global Data Definitions
-------------------------------------------------------------------*/

OEMNativeNotifierClass *gpNativeNotifier = NULL;

/*-------------------------------------------------------------------
            Static variable Definitions
-------------------------------------------------------------------*/

/*===============================================================================
                     FUNCTION DEFINITIONS
=============================================================================== */

/*===========================================================================

Function:  OEMNativeNotifierClass_AddRef()

Description:
	This function increases the referecne count for the 
   OEMNativeNotifierClass object

Prototype:
	uint32  OEMNativeNotifierClass_AddRef(Notifier* po)

Parameters:
   po [in]: Pointer to NativeNotifier interface whose reference count 
            is to be incremented

Return Value:
	The updated reference count

Comments: None

Side Effects: None

==============================================================================*/
uint32  OEMNativeNotifierClass_AddRef(INotifier* po)
{
   return ++(((OEMNativeNotifierClass *)po)->m_nRefs);
}

/*===========================================================================

Function:  OEMNativeNotifierClass_Release()

Description:
   This function decreases the referecne count for the NativeNotifier object.
   If the reference count reaches zero, it does the required cleanup

Prototype:
	uint32  OEMNativeNotifierClass_Release(INotifier* po)

Parameters:
   po [in]: Pointer to the NativeNotifier interface whose reference count 
            needs to be decremented.

Return Value:
   The updated reference count

Comments: None

Side Effects: None

==============================================================================*/
uint32  OEMNativeNotifierClass_Release(INotifier* po)
{
   OEMNativeNotifierClass * pMe = (OEMNativeNotifierClass *)po;

   if (--pMe->m_nRefs != 0)
      return pMe->m_nRefs;

   ISHELL_Release(pMe->m_pIShell);   // Release the Shell

   // delete this OEMNativeNotifierClass object allocated in NotifierClass_New
   FREEOBJ(pMe);   
   
   gpNativeNotifier = NULL;

   return 0;
}

/*===========================================================================

Function:  OEMNativeNotifierClass_Notify()

Description:
   This function is called by the native when the Idle / Non Idle
   transition happens. The native need to pass the corresponding 
   mask releated to Idle / Non Idle transition

Prototype:
	uint32  OEMNativeNotifierClass_Notify(uint32 dwBit)

Parameters:
   dwBit [in]: Native Notifier mask that indicates the event that occurred

Return Value: None

Comments: If the Idle app is BREW based, then ISHELL_Notify() can be called
          directly to send the notification. 

Side Effects: None

===========================================================================*/
void OEMNativeNotifierClass_Notify(uint32 dwBit)
{
   OEMNativeNotifierClass * pMe = gpNativeNotifier;

   FARF(NATIVENOTIFY, ("Native Notify: 0x%x, 0x%x", pMe, dwBit));

   if (!pMe)
      return;

#ifdef  NOTIFY_MULTIPLE_PENDING_EVENTS
   pMe->m_dwNotifyBit[pMe->m_byNotifyCountIn] = dwBit;
   if (++pMe->m_byNotifyCountIn >= MAX_NATIVENOTIFY) {
      pMe->m_byNotifyCountIn = 0;
   }
#else
   pMe->m_dwNotifyBit[0] = dwBit;
#endif

   AEE_SYS_RESUME(&pMe->m_cbNotify);
}

/*===========================================================================

Function:  OEMNativeNotifierClass_NotifyCallback()

Description:
   This function calls the shell notifier to notify the events so as to
   send the events to the applications that have registered for these events.
   The Shell will send the EVT_NOTIFY event to the user applet which 
   requested the event notify.

Prototype:
	uint32  OEMNativeNotifierClass_NotifyCallback(INotifier *po)

Parameters:
   po [in]: Pointer to the NativeNotifier interface 

Return Value: None

Comments: None

Side Effects: None

===========================================================================*/
void OEMNativeNotifierClass_NotifyCallback(INotifier * po)
{
   OEMNativeNotifierClass * pMe = (OEMNativeNotifierClass *)po;
   byte                     byArr;

   FARF(NATIVENOTIFY, ("Notify Callback: 0x%x", pMe));

   if (!pMe)
      return;

#ifdef  NOTIFY_MULTIPLE_PENDING_EVENTS
   byArr = pMe->m_byNotifyCountOut++;
   if (pMe->m_byNotifyCountOut >= MAX_NATIVENOTIFY) {
      pMe->m_byNotifyCountOut = 0;
   }
#else
   byArr = 0;
#endif

   FARF(NATIVENOTIFY, ("Notify Bit: %d 0x%x", byArr, pMe->m_dwNotifyBit[byArr]));
   FARF(NATIVENOTIFY, ("Notify Mask: 0x%x", pMe->m_dwNotifyMask));

   if (pMe->m_dwNotifyBit[byArr] == NMASK_NATIVENOTIFIER_IDLE) {
      pMe->m_dwNotifyBit[byArr] = 0;
      if (pMe->m_dwNotifyMask & NMASK_NATIVENOTIFIER_IDLE) {
         ISHELL_Notify(pMe->m_pIShell, AEECLSID_NATIVENOTIFIER, NMASK_NATIVENOTIFIER_IDLE, NULL);
      }
   }

   if (pMe->m_dwNotifyBit[byArr] == NMASK_NATIVENOTIFIER_NONIDLE) {
      pMe->m_dwNotifyBit[byArr] = 0;
      if (pMe->m_dwNotifyMask & NMASK_NATIVENOTIFIER_NONIDLE) {
         ISHELL_Notify(pMe->m_pIShell, AEECLSID_NATIVENOTIFIER, NMASK_NATIVENOTIFIER_NONIDLE, NULL);
      }
   }
}

/*===========================================================================

Function:  OEMNativeNotifierClass_SetMask()

Description:
   This function gets called by the Shell when an user applet has requested 
   a notification be done when a event of type "dwMask" has occured on the
   notification service.

Prototype:
	void OEMNativeNotifierClass_SetMask(Notifier* po, uint32 * dwMask)

Parameters:
   po     [in]: pointer to NativeNotifier interface.
   dwMask [in]: contains a const pointer to the event mask

Return Value: None

Comments:  This function is a part of the NativeNotifier class which implements
           a basic notification service.

Side Effects: None

==============================================================================*/
void OEMNativeNotifierClass_SetMask(INotifier* po, uint32 * dwMask)
{
   OEMNativeNotifierClass * pMe = (OEMNativeNotifierClass *)po;

   // just check if the nativenotifier is the value sent by the user.
   if (0 == dwMask) {
      return;
   }

   FARF(NATIVENOTIFY, ("Native Notifier Mask: 0x%x", *dwMask));

   // Store the event that was requested for later use in the callback function.
   pMe->m_dwNotifyMask = GET_NOTIFIER_MASK(*dwMask);
}

/*===========================================================================

Function: OEMNativeNotifierClass_New

Description:
	This function is invoked when the nativenotifier class is being loaded. 
   It creates a new instance of the OEMNativeNotifierClass structure. 

Prototype:
   int OEMNativeNotifierClass_New(IShell * pIShell,
                                  AEECLSID clsID, 
                                  void   **ppobj)

Parameters:
	pIShell [in]: Contains a valid pointer to the AEE IShell interface
	clsID [in]:   Specifies the ClassID of this class
	ppobj [out]:  On return, *ppobj points to a valid OEMNativeNotifierClass 
                 structure. This allocation is done by this function

Dependancies: None

Return Value:
  SUCCESS: If successful
  EBADPARM, ENOMEMORY: If failed

Comments: None

Side Effects: None

===========================================================================*/
int OEMNativeNotifierClass_New(IShell   *pIShell,
                               AEECLSID  clsID,
                               void    **ppobj)
{
   OEMNativeNotifierClass   *pMe = NULL;
   VTBL(INotifier)          *appFuncs;
   int                       nSize;
		
   *ppobj = NULL;

   //Validate parameters
   if (!pIShell) 
      return EBADPARM;

   if (gpNativeNotifier) {
      ++gpNativeNotifier->m_nRefs;
   } else {
      nSize = sizeof(OEMNativeNotifierClass);
      // Allocate memory for the nativenotifier object and the INativeNotifierVtbl
      if (NULL == (pMe = (OEMNativeNotifierClass*)MALLOC(nSize + sizeof(INotifierVtbl))))
         return ENOMEMORY;

      appFuncs = (INotifierVtbl *)((byte *)pMe + nSize);

      //Initialize the individual entries in the VTBL
      appFuncs->AddRef  = OEMNativeNotifierClass_AddRef; 
      appFuncs->Release = OEMNativeNotifierClass_Release;
      appFuncs->SetMask = OEMNativeNotifierClass_SetMask;
   
      INIT_VTBL(pMe, INotifier, *appFuncs);   // Initialize the VTBL

      //Initialize the data members
      pMe->m_nRefs      = 1;
      pMe->m_pIShell    = pIShell;
      pMe->clsID        = clsID;

      ISHELL_AddRef(pIShell);

      // Init the callback...
      CALLBACK_Init(&pMe->m_cbNotify, (PFNNOTIFY)OEMNativeNotifierClass_NotifyCallback,
                    pMe);

      gpNativeNotifier = pMe;
   }

   *ppobj = (OEMNativeNotifierClass*)gpNativeNotifier;  

   return SUCCESS;
}

