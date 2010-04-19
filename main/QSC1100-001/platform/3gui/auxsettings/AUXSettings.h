#ifndef AUXSETTINGS_H
#define AUXSETTINGS_H
/*=============================================================================

FILE: AUXSettings.h

SERVICES: QCT UI AUX Configuration menu

GENERAL DESCRIPTION:
.h file for AUXApp.

(c) COPYRIGHT 2007-2008 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/08   cvs     Revert WLAN subscription changes
11/28/07   sg      Added new user event EVT_AUX_REGISTER_CM_EVENTS
10/29/07   sg      Support for setting the BREW Secure clock
02/23/07   jas     Refactoring for HDK builds
02/09/07   jas     Improve handling of CM events and sending WLAN subscription 
01/23/07   ypmw    Move WLAN application from Core App to AUX Settings
12/11/06   RT      Ported support for FEATURE_DATA_MULTI_PORTS
10/31/06   RT      Initial revision.
===========================================================================*/

/*=============================================================================

                       INCLUDES

=============================================================================*/
#include "customer.h"

/* Brew libraries */
#include "OEMFeatures.h"
#include "AEE.h"
#include "AEEShell.h"
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AEESysClock.h"

#ifdef FEATURE_WLAN
#include "AUXWLAN.h"
#endif

#include "AEE_OEM.h"
#include "AEEConfig.h"
#include "rdevmap.h"

#include "auxsettings.brh"

/*=============================================================================

                       AUX APP USER EVENTS

=============================================================================*/
/* Menu Events */
#define EVT_RDM_DONE                1052
#define EVT_RDM_NOT_ALLOWED         1053
#define EVT_RDM_DEVMAP_BUSY         1054
#define EVT_RDM_APP_BUSY            1055
#undef EVT_MESSAGE_END
#define EVT_MESSAGE_END             1154

#define AUXAPPTIMEOUT 60000  /* Time out value for bringing the program back to
                                the top level screen of the applet */

/*=============================================================================

                       DATA STRUCTURE

=============================================================================*/
typedef struct
{
  AEEApplet a;                                /* First element of this structure must be AEEApplet. */

  boolean      m_bAppIsRunning;               /* Whether app has been started. */

#ifdef FEATURE_RUNTIME_DEVMAP
  uint32       m_nCurrSrvc;                   /* Current service type */
  rdm_service_display_type *m_pRdmDeviceList; /*RDM Device List*/
#endif

  uint16       m_MessageID;
  AECHAR*      m_pszMsgString;                /* Message String to be displayed */
  uint16       m_RootDialogID;                /* Current dialog ID */

#ifdef FEATURE_WLAN
  AUXWLAN      m_WLAN;
#endif
  uint16       m_cxScreen;                    /* What is the screen size? */
  ISysClock    *m_pISecSysClock;              /* Pointer to BREW Secure Sys Clock*/
} AUXApp;

extern char *gpAUXResourceFile;

#define AUXCheckAEEReturnStatus(nReturnStatus)  ((nReturnStatus) == SUCCESS? TRUE: FALSE)

/*=============================================================================

                       FUNCTION DECLARATIONS

=============================================================================*/

#ifdef FEATURE_WLAN
/*=============================================================================
FUNCTION: AUXApp_ProcessICMEvents

DESCRIPTION:
  Process ICM Notifier events

SIDE EFFECTS:
  None.

=============================================================================*/
boolean AUXApp_ProcessICMEvents(AUXApp *pMe, uint16 wParam, uint32 dwParam);
#endif

#endif /* AUXSETTINGS_H */
