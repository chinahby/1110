#ifndef NETSETTINGS_H
#define NETSETTINGS_H
/*=============================================================================

FILE: NetSettings.h

SERVICES: QCT UI Network menu

GENERAL DESCRIPTION:
.h file for NetApp.

(c) COPYRIGHT 2006 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/NetSettings/NetSettings.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/01/06   jas     Complete the renaming of symbols formerly in CoreApp
06/18/06   joe     Initial revision.
===========================================================================*/

/*=============================================================================

                       INCLUDES

=============================================================================*/

/* Brew libraries */
#include "AEE.h"
#include "AEEShell.h"
#include "AEEAppGen.h"
#include "AEEMenu.h"
#include "AEE_OEM.h"
#include "AEEConfig.h"
#include "AppComFunc.h"
#include "OEMCFGI.h"
#if defined (FEATURE_WCDMA) || defined (FEATURE_GSM)
#include "AEESuppSvc.h"
#endif

#include "netsettings.brh"
#include "AEECM.h"

#include "AEESTK.h"

/*=============================================================================

                       NET APP USER EVENTS

=============================================================================*/

#define EVT_SYS_PREF_ERR            1153
#define EVT_MESSAGE_END             1154

/*=============================================================================

                       CONSTANTS

=============================================================================*/


/*=============================================================================

                       DATA STRUCTURE

=============================================================================*/

typedef struct
{
  /*First element of this structure must be AEEApplet. */
  AEEApplet a ;

  IConfig *    m_pIConfig;            
  ICM   *      m_pICM;              /* Call manager */

  AEECMPhError m_PhError;           /* Error in ICM_GetNetworks */
  AEECallback  m_nCallback;         /*GetNetworks callback.*/

  uint16       m_MessageID;
  AECHAR*      m_pszMsgString;      /* Message String to be displayed */

} CNetApp;

extern char *gpNetResourceFile;

/*=============================================================================

                       FUNCTION DECLARATIONS

=============================================================================*/

#endif /* NETSETTINGS_H */

