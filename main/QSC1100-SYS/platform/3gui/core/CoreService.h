#ifndef CORESERVICE_H
#define CORESERVICE_H
 
/*=============================================================================

FILE: CoreService.h

GENERAL DESCRIPTION:
Header file for CoreService.c

PUBLIC CLASSES AND STATIC FUNCTIONS:
N/A

(c) COPYRIGHT 2006-2007 QUALCOMM Incorporated.
                   All Rights Reserved.

                   QUALCOMM Proprietary
=============================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/28/07   clm     Make CoreApp_DisplayFromCPHSInfo, CoreApp_DisplayONS, and 
                   CoreApp_DisplaySPN functions public.
08/14/07   cvs     Fix case where 1x targets would toggle RSSI when not in hybrid
06/13/07   cvs     Handle LIMITED_REGIONAL like LIMITED service
06/12/07   cvs     Don't update hybrid RSSI when LCD off
11/10/06   RT      Initial Revision - CoreApp Refactoring
===========================================================================*/

/*===========================================================================

                               INCLUDES

===========================================================================*/
#include "CoreApp.h"

#define SRV_STATUS_AVAIL(srv_status)  (((srv_status) == AEECM_SRV_STATUS_SRV) || \
                                       ((srv_status) == AEECM_SRV_STATUS_LIMITED) || \
                                       ((srv_status) == AEECM_SRV_STATUS_LIMITED_REGIONAL))

#define SRV_STATUS_LIMITED(srv_status)  (((srv_status) == AEECM_SRV_STATUS_LIMITED) || \
                                         ((srv_status) == AEECM_SRV_STATUS_LIMITED_REGIONAL))

#define SYS_MODE_GW_1X(sys_mode, srv_status) (((sys_mode) != AEECM_SYS_MODE_HDR)&&\
                                              (SRV_STATUS_AVAIL(srv_status)))

#define SYS_MODE_GW(sys_mode, srv_status) ((((sys_mode) == AEECM_SYS_MODE_GSM)||\
                                            ((sys_mode) == AEECM_SYS_MODE_WCDMA))&&\
                                           (SRV_STATUS_AVAIL(srv_status)))

#define SYS_MODE_HDR(sys_mode, hdr_srv_status) ((SRV_STATUS_AVAIL(hdr_srv_status)) \
                                || ((sys_mode) == AEECM_SYS_MODE_HDR))

#define HYBRID_MODE_AVAIL(phInfo) (((phInfo).hybr_pref == AEECM_HYBR_PREF_ON) && \
                                  ((phInfo).mode_pref != AEECM_MODE_PREF_CDMA_ONLY) && \
                                  ((phInfo).mode_pref != AEECM_MODE_PREF_HDR_ONLY))

#define HYBRID_SRV_AVAIL(phInfo, ssInfo)\
              ((HYBRID_MODE_AVAIL(phInfo)) &&\
              (SRV_STATUS_AVAIL((ssInfo).srv_status)) &&\
              (SRV_STATUS_AVAIL((ssInfo).hdr_srv_status)))

/* HDR is available if either is true
 1. hybrid preference is on AND the hdr_srv_status has AEECM_SRV_STATUS_SRV
 2. sys mode is HDR AND the main srv_status has AEECM_SRV_STATUS_SRV 
 */
#define HDR_SRV_AVAIL(phInfo, ssInfo)\
              ((((ssInfo).hdr_hybrid) &&\
                (SRV_STATUS_AVAIL((ssInfo).hdr_srv_status))) \
              || ((((ssInfo).sys_mode) == AEECM_SYS_MODE_HDR) && \
                 (SRV_STATUS_AVAIL((ssInfo).srv_status))))

/*-------------------------------------------------------------------

                  Exported function prototypes

-------------------------------------------------------------------*/
/*===========================================================================
FUNCTION InitAfterPhInfo

DESCRIPTION
  Executes startup sequence after phone information is available from ICM.
  If phone info is available at Core App startup, no need to wait for
  AEECM_EVENT_PH_INFO_AVAIL event.

DEPENDENCIES
  ICM interface should have been created previously.

SIDE EFFECTS
  None
===========================================================================*/
void InitAfterPhInfo(CCoreApp *pMe, AEECMOprtMode mode);


/*===========================================================================
FUNCTION DBToLevel

DESCRIPTION
  Converts RSSI absolute DB value to an annunciator level

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
int DBToLevel (int nDBVal);

/*===========================================================================
FUNCTION IsConfigCtrlReady

DESCRIPTION
  Check to see if config or control has invalid value

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean IsConfigCtrlReady(CCoreApp *pMe);

/*=============================================================================
FUNCTION: ProcessSubscriptionStatus

DESCRIPTION:
  Look at various parameters to see the subscription status and send it to CM

SIDE EFFECTS:
  None.

=============================================================================*/
void CoreApp_ProcessSubscriptionStatus (CCoreApp *pMe);


/*=============================================================================
FUNCTION: CoreApp_ProcessOffLine

DESCRIPTION:
  Display the corresponding message for offline mode

SIDE EFFECTS:
  None.

=============================================================================*/
boolean CoreApp_ProcessOffLine(CCoreApp *pMe);


/*=============================================================================
FUNCTION: CoreApp_ProcessICMEvents

DESCRIPTION:
  Process ICM Notifier events

SIDE EFFECTS:
  None.

=============================================================================*/
boolean CoreApp_ProcessICMEvents(CCoreApp *pMe, uint16 wParam, uint32 dwParam);

/*=============================================================================
FUNCTION: RefreshRSSIAnnunciator

DESCRIPTION:
  Refreshes the RSSI annunciator using current system service status

SIDE EFFECTS:
  None.

=============================================================================*/

void RefreshRSSIAnnunciator(CCoreApp *pMe, boolean serviceChange);

#ifdef FEATURE_CPHS_EONS
/*===========================================================================
FUNCTION CoreApp_DisplayFromCPHSInfo

DESCRIPTION
  Displays the Operator Name String from CPHS Info
DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
boolean CoreApp_DisplayFromCPHSInfo(CCoreApp *pMe, char* buf);

/*===========================================================================
FUNCTION CoreApp_DisplayONS

DESCRIPTION
  Displays the Operator Name String in the annunciators according to Algorithm.
  in EONS Feature Doc
DEPENDENCIES


SIDE EFFECTS
  None
===========================================================================*/
boolean CoreApp_DisplayONS(CCoreApp *pMe, char* buf,uint8 slot, uint8 *recNum);

/*===========================================================================
FUNCTION CoreApp_DisplaySPN

DESCRIPTION
  Displays the Service Provider Name in the annunciators according to Algorithm.
  in EONS Feature Doc
DEPENDENCIES
  Service #17/#19 is available and activatedin SST/UST respectively

SIDE EFFECTS
  None
===========================================================================*/
boolean CoreApp_DisplaySPN(CCoreApp *pMe, char* buf);
#endif /* FEATURE_CPHS_EONS */

#endif /* CORESERVICE_H */






