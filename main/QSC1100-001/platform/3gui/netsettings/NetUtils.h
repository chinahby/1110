#ifndef NETUTILS_H
#define NETUTILS_H
/*===========================================================================

FILE: NetUtils.h

DESCRIPTION
   Header file for NetUtils.c

PUBLIC CLASSES:
   N/A

INITIALIZATION AND SEQUENCING REQUIREMENTS:


       Copyright © 2002 - 2007 QUALCOMM Incorporated.
                    All Rights Reserved.
                   QUALCOMM Proprietary
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/NetSettings/NetUtils.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/20/07   jas/ak  Adding support for persistent preference values.
09/01/06   jas     Complete the renaming of symbols formerly in CoreApp
09/01/06   jas     Resolving name collision with CoreApp
06/20/06   joe     Initial Revision

===========================================================================*/


/*=============================================================================

                INCLUDES AND VARIABLE DEFINITIONS

=============================================================================== */
#include "AEE.h"

#include "NetSettings.h"

/*=============================================================================

        MACRO DECLARATION

=============================================================================== */

/*===========================================================================
MACRO NET_FATAL

DESCRIPTION
  Call ERR_FATAL when fatal error needs to be informed

DEPENDENCIES
  ERR_FATAL

SIDE EFFECTS
  None
===========================================================================*/
#include "err.h"
#include "msg.h"
#include "oemnvint.h"
#define NET_FATAL(xx_exp)\
  if(!(xx_exp)){ \
    ERR_FATAL( "Fatal Assertion " #xx_exp " failed", 0, 0, 0 ); \
  }
#define NET_ERR(format, code1, code2, code3) MSG_ERROR(format, code1, code2, code3)
#define NET_MSG(format, code1, code2, code3) MSG_MED(format, code1, code2, code3)

/*=============================================================================

              CONSTANT DECLARATION

=============================================================================== */
#define NETSELECTIONVERIFICATIONTIME 2000

/*=============================================================================

              FUNCTION DECLARATION

=============================================================================== */
#ifdef FEATURE_JCDMA_1X
/*===========================================================================
FUNCTION NetGetM511Mode

DESCRIPTION
  Get M51 settings from NV, update static variables

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetM511Mode(CNetApp * pMe);

/*===========================================================================
FUNCTION NetSetM511Mode

DESCRIPTION
  Save M51 settings in NV, update static variables

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetM511Mode(CNetApp * pMe, uint64 value);

/*===========================================================================
FUNCTION NetGetM512Mode

DESCRIPTION
  Get M51 settings from NV, update static variables

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetM512Mode(CNetApp * pMe);

/*===========================================================================
FUNCTION NetSetM512Mode

DESCRIPTION
  Save M51 settings in NV, update static variables

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetM512Mode(CNetApp * pMe, uint64 value);

/*===========================================================================
FUNCTION NetGetM513Mode

DESCRIPTION
  Get M51 settings from NV, update static variables

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetM513Mode(CNetApp * pMe);

/*===========================================================================
FUNCTION NetSetM513Mode

DESCRIPTION
  Save M51 settings in NV, update static variables

DEPENDENCIES
  ui_get_nv

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetM513Mode(CNetApp * pMe, uint64 value);
#endif /* FEATURE_JCDMA_1X */

/*===========================================================================
FUNCTION NetGetForceModePref

DESCRIPTION
  Initialize force mode menu

DEPENDENCIES
  Various Get and Set method for the variables in NetUtils.c

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetForceModePref(CNetApp *pMe);


/*===========================================================================
FUNCTION NetSetForceModePref

DESCRIPTION

  Change force mode setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetForceModePref(CNetApp *pMe, uint64 nNewSetting);

#ifdef FEATURE_MMGSDI_DUAL_SLOT
/*===========================================================================
FUNCTION NetSetForceModePrefSlotCheck

DESCRIPTION

  Change force mode setting.
  Save it and return TRUE if successful, return FALSE otherwise.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint8 NetSetForceModePrefSlotCheck(CNetApp *pMe, uint64 nNewSetting);
#endif /*FEATURE_MMGSDI_DUAL_SLOT*/

/*===========================================================================
FUNCTION NetGetBandPref

DESCRIPTION
  Initialize band pref menu

DEPENDENCIES
  Various Get and Set method for the variables in NetUtils.c

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetBandPref(CNetApp *pMe);

/*===========================================================================
FUNCTION NetSetBandPref

DESCRIPTION

  Change band pref setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetBandPref(CNetApp *pMe, uint64 nNewSetting);

/*===========================================================================
FUNCTION NetGetDomainPref

DESCRIPTION
  Initialize domain pref menu

DEPENDENCIES
  Various Get and Set method for the variables in NetUtils.c

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetDomainPref(CNetApp *pMe);

/*===========================================================================
FUNCTION NetSetDomainPref

DESCRIPTION

  Change domain pref setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetDomainPref(CNetApp *pMe, uint64 nNewSetting);

/*===========================================================================
FUNCTION NetGetAcqOrderPref

DESCRIPTION
  Initialize acquistion order

DEPENDENCIES
  Various Get and Set method for the variables in NetUtils.c

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetAcqOrderPref(CNetApp *pMe);

/*===========================================================================
FUNCTION NetSetAcqOrderPref

DESCRIPTION

  Change acquistion order setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetAcqOrderPref(CNetApp *pMe, uint64 nNewSetting);

/*===========================================================================
FUNCTION NetGetRoamPref

DESCRIPTION
  Initialize roam pref

DEPENDENCIES
  Various Get and Set method for the variables in NetUtils.c

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetRoamPref(CNetApp *pMe);

/*===========================================================================
FUNCTION NetSetRoamPref

DESCRIPTION

  Change roam pref setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetRoamPref(CNetApp *pMe, uint64 nNewSetting);

/*===========================================================================
FUNCTION NetGetHybridPref

DESCRIPTION
  Initialize hybrid pref

DEPENDENCIES
  Various Get and Set method for the variables in NetUtils.c

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetHybridPref(CNetApp *pMe);

/*===========================================================================
FUNCTION NetSetHybridPref

DESCRIPTION

  Change hybrid pref setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetHybridPref(CNetApp *pMe, uint64 nNewSetting);

/*===========================================================================
FUNCTION NetGetAutoNAM

DESCRIPTION
  Save Network -> Auto NAM menu setting in NetApp

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetAutoNAM(CNetApp *pMe) ;

/*===========================================================================
FUNCTION NetSetAutoNAM

DESCRIPTION
  Save Network -> Auto NAM menu setting in NetApp

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetAutoNAM(CNetApp *pMe, uint64 nAutoNAMOn);

/*===========================================================================
FUNCTION NetGetNetworkDataFax

DESCRIPTION
  Get Network -> Data/Fax menu setting from SK UI.

DEPENDENCIES
  SK UI

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetNetworkDataFax(CNetApp *pMe);

/*===========================================================================
FUNCTION NetSetNetworkDataFax

DESCRIPTION
  Set Network -> Data/Fax menu setting in 3G UI.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetNetworkDataFax(CNetApp *pMe, uint64 nNewSetting);

/*===========================================================================
FUNCTION NetGetNetworkSelect

DESCRIPTION
  Get Network -> Select menu setting.

DEPENDENCIES
  SK UI

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetNetworkSelect(CNetApp *pMe);

/*===========================================================================
FUNCTION NetGetNetworkNAMSettings

DESCRIPTION
  Get Network -> Auto / Set NAM menu setting

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint64 NetGetNetworkNAMSettings(CNetApp *pMe);

/*===========================================================================
FUNCTION NetSetNetworkCurrentNAM

DESCRIPTION
  Save Network -> Set NAM selection in Net App

DEPENDENCIES
  SK UI

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetNetworkCurrentNAM(CNetApp *pMe, uint64 wNewSetting);

/*===========================================================================
FUNCTION NetSetStaticTextCtlSize

DESCRIPTION
  Set the rect for static control when there are one other control presented
  in the dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetStaticTextCtlSize(IStatic *pICurrentStatic, IControl *pIOtherCtrl);

/*===========================================================================
FUNCTION NetSetGeneralStaticTextStyle

DESCRIPTION
  Set the standard textctl dialog style

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetGeneralStaticTextStyle(IStatic *pIStaticText);

/*===========================================================================
FUNCTION NetAvoidCurrentSystem

DESCRIPTION
  This function will tell ICM to avoid the current serving system, for 
  the number of seconds specified in avoid_time

DEPENDENCIES
  None

SIDE EFFECTS
  Serving system is avoided
===========================================================================*/
boolean NetAvoidCurrentSystem(CNetApp * pMe, uint32 avoid_time);

/*===========================================================================
FUNCTION NetEndCurrentDlg

DESCRIPTION
  End the current dialog

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void NetEndCurrentDlg(CNetApp *pMe);

/*===========================================================================
FUNCTION NetSetNetworkAutoSelection

DESCRIPTION

  Change network select setting.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean NetSetNetworkAutoSelection(CNetApp *pMe,
                                   AEECMModePref mode_pref,
                                   AEECMPrefTerm term_pref,
                                   AEECMAcqOrderPref acq_order,
                                   AEECMBandPref band_pref,
                                   AEECMRoamPref roam_pref,
                                   AEECMHybrPref hyb_pref,
                                   AEECMServDomPref srv_domain_pref);

/*===========================================================================
FUNCTION NetDisplayMessageDlg

DESCRIPTION
  Create a message dialog based in resource id of string
  if nDisplaySeconds is zero, dialog will only clear on key input

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void NetDisplayMessageDlg(CNetApp *pMe, uint16 nStringID, int nDisplaySeconds);

/*===========================================================================
FUNCTION NetEndMessageDlg

DESCRIPTION
  End a message dialog

DEPENDENCIES
  None

SIDE EFFECTS
  Send an EVT_MESSAGE_END event
===========================================================================*/
void NetEndMessageDlg(CNetApp *pMe);

/*=============================================================================
FUNCTION: NetAppSelectNetwork

DESCRIPTION:  Process select network radio button dialogue input. If the choice
is automatic, handle it in NetApp. Otherwise, post an event to CoreApp instructing
it to handle the manual dialogues. CoreApp has all the info needed to handle them,
while NetApp does not.

PARAMETERS: dwParm is the currently selected dialog. This will be passed to
NetworkSelectSetting so that it can handle the event properly.

RETURN VALUE:
   SUCCESS - if successful

COMMENTS:
   This function assumes that CoreApp is the next running  application. When CoreApp
   resumes it will handle this event and bring up the correct dialogue. If CoreApp
   is not the next running application, the dialogue will launch whenever CoreApp
   resumes.

SIDE EFFECTS:
  None

=============================================================================*/
int NetAppSelectNetwork(CNetApp* pMe, uint32 dwParm);

#endif // NETUTILS_H

