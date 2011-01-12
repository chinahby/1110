/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                 C A L L   M A N A G E R   GSDI   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager GSDI Interface block, which groups
  all the functionality that is a associated with CM<->GSDI interfacing.




EXTERNALIZED FUNCTIONS



INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmnv_init() must be call to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2002 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmgsdi.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/09   am      fixing memset error.
11/20/08   rm      RAT balancing req - Accept UIM_USIM_RAT Refresh request
                   during CS/PS call. 
11/21/08   am      Defining cmgsdi_dummy() to get rid of compiler warnings
11/17/08   am      Adding GSDI support also for targets other than GW.
11/07/08   rm      RAT balancing - Honor EF_RAT file changes while power up
10/01/08   am      Making changes such that if FEATURE_DATA_REFRESH_VOTING
                   defined CM will vote for only non-PS calls
09/18/08   rm      RAT Balancning enhancement- Block user from changing mode pref
                   only if ENS flag is ON and UIM_USIM_RAT file is present 
12/12/07   rm      Adding RAT-balancing feature support
10/01/07   rm      Handling of GSDI_CARD_ERR_POLL_ERROR in CM
08/07/07   ic      Lint clean up
06/07/07   rp      Fixing memory leak
09/06/06   ic      Unnecessary comments clean up
07/18/06   ka/ic   Adding check for CM specific SIM files before voting for
                   file notify.
04/03/06   ka      Setting default value of ACT field in PPLMN with Access 
                   tech SIM file to 0.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
09/02/05   pk      Modified cm_gsdi_refresh_vote_cb(), added the else part to
                   if ( cm_state == CM_OVERALL_CALL_STATE_IDLE)
03/30/05   dk      Initialize file_length to 0 in PLMN list write func.
03/21/05   dk      Fixed plmn_write function so that if a entry is
                   deleted, it is reflected correctly in the SIM files.
01/17/05   dk      Added support to read unpacked User PLMN Lists.
11/15/04   ic      Lint cleanup with CM_DEBUG on
11/12/04   ic      Fixed RVCT 2.1 compiler warnings / Lint clean up
11/11/04   ic      Lint cleanup
08/09/04   ic      Lint clean up for SNAR configuration
08/02/04   ic      CM Lint clean up - cmgsdi.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
07/12/04   ic      Added support for FEATURE_ALS
06/14/04   jqi     Made CM MMGSDI calls to be asynchronouse.
06/04/04   ka      Fixed Lint errors.
04/05/04   ka      Modified code to support change in gsdi_get_ef_length()
                   API.  Change from byte* to int*.
04/02/04   ic      Merged over to CM mainline the delta between
                   MSMSHARED_CM.01.00.10.00.23 and MSMSHARED_CM.01.00.10.00.28
03/15/04   ka      Merged changes from 6250. Fixed Lint errors.
02/26/04   jar     Modified MSM6500 Builds.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/10/04   ws      Added new wrapper cm_gsdi_in_traffic_channel_w_mode
02/06/04   ws      Initial jaguar/mainline merge.
01/29/04   jar     Added new wrapper cm_gsdi_in_traffic_channel_w_mode
01/24/04   tml     Refresh for dual slot
09/17/03   ws      Added events for slot 2.
06/02/03   jar     Added rec num accessed dummy for gsdi_sim_read
05/01/03   jar     Added support for REFRESH File Change Notifications
                   Changed ERR_FATAL to CM_ERR_FATAL
04/28/03   AT      Lint cleanup.
04/11/03   prk     Removed references to FEATURE_MMGSDI.
02/03/03   prk     Removed removed references to SYS_ACT_UNKNOWN_MASK.
           cs      Added more GSDI event reporting.
01/29/03   jar     Changed FEATURE_MMGSDI_GSM && FEATURE_MMGSDI_UMTS to ||
01/29/03   jar     Changed NULL_PTR NULL.
01/24/03   jar     Do not attempt client registration with MMGSDI in 1X.
01/21/03   ws      Updated copyright information for 2003.
11/26/02   vt      Fixed a compile proble when FEATURE_MMGSDI is not defined.
11/22/02   PRK     Added support for accessing user preferred PLMN list.
10/04/02   CS      Initial release.
===========================================================================*/



/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#if !((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI))
/*lint -esym(766,comdef.h) */
#include "comdef.h"
#include "cmi.h"
#include "cmgsdi.h"
#include <string.h>
#include "cmtask.h"
#include "cmdbg.h"
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif

#if (defined FEATURE_WCDMA) || (defined FEATURE_GSM)
#error code not present
#endif



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#if (defined (FEATURE_WCDMA) || defined (FEATURE_GSM) )
#error code not present
#endif /* FEATURE_WCDMA || FEATURE_GSM */

#ifdef FEATURE_MMGSDI


/*===========================================================================
FUNCTION CM_GSDI_IN_TRAFFIC

DESCRIPTION
  Inform GSDI that call is connected/disconnected, so GSDI can start/stop
  in call polling.

RETURN VALUE
  GSDI status.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the CM task.
===========================================================================*/
void cm_gsdi_in_traffic(boolean in_traffic)
{
  gsdi_cm_in_traffic_channel(in_traffic);
}


/*===========================================================================
FUNCTION CM_GSDI_IN_TRAFFIC_CHANNEL_W_MODE

DESCRIPTION
  Inform GSDI that call is connected/disconnected, so GSDI can start/stop
  in call polling.

  in_traffic is a boolean which indicates whether we are in traffic or not.
  mode is a indicates whether the call is GSM / UMTS or 1X.

RETURN VALUE
  GSDI status.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the CM task.
===========================================================================*/
void cm_gsdi_in_traffic_channel_w_mode(
  boolean in_traffic,
  sys_sys_mode_e_type mode)
{
  gsdi_in_tc_mode_enum_type gsdi_mode = GSDI_NOT_IN_TC;

  switch ( mode )
  {
    case SYS_SYS_MODE_AMPS:
    case SYS_SYS_MODE_CDMA:
      gsdi_mode = GSDI_IN_TC_1X;
      break;

    case SYS_SYS_MODE_GSM:
    case SYS_SYS_MODE_WCDMA:
      gsdi_mode = GSDI_IN_TC_GW;
      break;

    default:
      MSG_ERROR ("Unhandled Mode: %x",mode,0,0);
      return;
  }

  gsdi_cm_in_traffic_channel_w_mode(in_traffic, gsdi_mode);
}
#if (defined FEATURE_ALS) && (defined FEATURE_GSM)
#error code not present
#endif /* #if (defined FEATURE_ALS) && (defined FEATURE_GSM) */

#endif /* FEATURE_MMGSDI*/

/*===========================================================================
FUNCTION CMGSDI_REFRESH_INIT

DESCRIPTION
  This function initialises the AOC and sets the new RAT mode.
  The rat_mode is for RAT-balanccing feature.
  If UIM_USIM_RAT file is present, CM reads the contents of the file,
  and also updates the rat_mode.

  Once the file read is done CM, send refresh_complete command to GSDI.

RETURN VALUE

  TRUE  - Command successfully queued.
  FALSE - Command was not queued.

DEPENDENCIES

===========================================================================*/

#if (defined FEATURE_WCDMA) || (defined FEATURE_GSM)
#error code not present
#endif /* #if (defined FEATURE_WCDMA) || (defined FEATURE_GSM) */
#else /* ((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI)) */

/*===========================================================================
FUNCTION cmgsdi_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmgsdi_dummy( void )
/*lint -esym(714,cmgsdi_dummy) */
/*lint -esym(765,cmgsdi_dummy)
** Can't be static as it would result in compiler warnings
*/
{
}

#endif /* ((defined FEATURE_MMGSDI) && (defined FEATURE_CM_MMGSDI)) */


