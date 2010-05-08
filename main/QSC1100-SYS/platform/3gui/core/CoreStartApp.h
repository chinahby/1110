#ifndef CORESTARTAPP_H
#define CORESTARTAPP_H
/*=============================================================================

FILE: CoreStartApp.h

SERVICES: QCT UI Core Startup BREW Application.

GENERAL DESCRIPTION:
.h file for CoreStartApp.

PUBLIC CLASSES AND STATIC FUNCTIONS:


INITIALIZATION AND SEQUENCING REQUIREMENTS:
This should be the startup application automatically launched by BREW.

(c) COPYRIGHT 2004 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/Core/CoreStartApp.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/09/04   jas     Created from CoreApp.h
===========================================================================*/

/*=============================================================================

                       INCLUDES

=============================================================================*/

#include "AEE.h"
#include "AEEShell.h"
#include "AEEAppGen.h"

#ifdef FEATURE_APP_TEST_AUTOMATION
#include "AEEDiag.h"
#endif



/*=============================================================================

                       CORE START APP USER EVENTS

=============================================================================*/


/*=============================================================================

                       CONSTANTS

=============================================================================*/

/*=============================================================================

                       DATA STRUCTURE

=============================================================================*/

#define CORE_NO_START_ARGS "NONE"
typedef struct
{
   char       *pszStartArgs;
} CCoreStartApp_AppStartArgs;

typedef struct
{
   AEEEvent    EventId;
   uint16      wParam;
   uint32      dwParam;
} CCoreStartApp_AppStartEvent;

typedef enum
{
   COREAPP_START_SIMPLE    = 0,   // ISHELL_StartApplet
   COREAPP_START_WITH_ARGS,       // ISHELL_StartAppletArgs
   COREAPP_POST_EVENT             // ISHELL_PostEvent
} CCoreStartApp_AppStartAction;

/* The type of the union member is determined by the start action.
 *    Start Action              Member Type
 *    -------------------------------------------------------
 *    COREAPP_START_SIMPLE      - unused -
 *    COREAPP_START_WITH_ARGS   CCoreApp_AppStartArgs
 *    COREAPP_POST_EVENT        CCoreApp_AppStartEvent
 */
typedef struct
{
   AEECLSID                         ClassId;
   CCoreStartApp_AppStartAction     StartAction;
   union
   {
      CCoreStartApp_AppStartArgs    sArgs;
      CCoreStartApp_AppStartEvent   sEvent;
   } StartData;
} CCoreStartApp_GenAppStartInfo;

/* The number of background apps that we can currently start.  If we
 * reach this limit, it can be increased.  We just need some size limit
 * for the array of apps to start.
 */
#define CORESTARTAPP_NUM_APP_STARTS    20

// Applet struct for the Core Start App Applet
typedef struct CCoreStartApp
{
   /* First element of this structure must be AEEApplet. */
   AEEApplet                  a;
   AEECallback                m_sStartCB;        // Callback used to start apps
   CCoreStartApp_GenAppStartInfo
                              m_AppStartInfo[CORESTARTAPP_NUM_APP_STARTS];
                                                 // Array of app start data
   uint8                      m_nNextAppToStart; // Index of the next app to
                                                 // start in the global array
   uint8                      m_nNumAppsToStart; // Number of apps in the
                                                 // global start array
} CCoreStartApp;

#endif /* CORESTARTAPP_H */
