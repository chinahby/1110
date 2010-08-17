/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

           C A L L   M A N A G E R   A D V I C E   O F   C H A R G E

GENERAL DESCRIPTION
  This module performs the Adivce of Charge functionality for the
  Call Manager.


EXTERNALIZED FUNCTIONS


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 2001 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmwaoc.c#3 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/04/09   am      fixing memset error.
02/04/09   rm      Initialising ACM filename correctly in MMGSDI
11/17/08   am      Replacing CM GSDI interface with CM MMGSDI interface
10/01/08   sv      Remove unnecessary string.h include to remove lint errors.
12/12/07   rm      Adding RAT-balancing feature support
                   Removed cm_refresh_aoc_init
08/07/07   ic      Lint clean up
04/03/07   rp      Adding use_e7 parameter to cmwaoc_set_ccm_timeout and calling 
                   it from cm_aoc_update_aoc
12/28/06   ka      Removing conditional return of AOC functionality with 
                   FALSE.
08/16/06   ka/rp   Moving aoc_ready flag to PH object.
03/24/06   ka      Cleaning up elapsed time cnt and call insertion into call
                   charge list. Restarting PH ccm timer for concurrent call
                   after first call is ended.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
10/13/05   ic      Replaced INTLOCK / INTFREE with rex_enter_crit_sect() /
                   rex_leave_crit_sect()  
12/06/04   ic      In cm_aoc_add_to_chargelist() renamed variable timer_ptr  
                   to timer_to_set_ptr to get rid of Lint error "declaration 
                   of symbol 'timer_ptr' hides symbol 'timer_ptr' (line 160, 
                   file ..\..\services\time\timer.h)"
08/04/04   ic      Removed unused argument from cm_aoc_save_acm().
                   CM Lint clean up - cmwaoc.c with Lint supression verified 
                   and free of Lint issues in all CUTE configurations. 
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added cmwaoc_dummy() to get rid of ADS 1.2 compiler warning
07/06/04   dk      Modified signature of cm_find_connection_id().
06/23/04   ic      Fixed ADS 120 compiler warnings (Saber build)
06/14/04   jqi     Made CM MMGSDI calls to be asynchronouse.
03/16/04   ka      Merged changes from 6250
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/19/04   ka      Changed cmaoc_send_call_end for Multiple PDP.
01/13/04   ka      Merged changes from MSM6200
05/30/03   tml     Added rec_num_accessed_pt to gsdi_sim_read synch call
05/01/03   jar     Added cm_refresh_aoc_init for REFRESH FCN Handling
04/28/03   AT      Lint cleanup.
04/11/03   prk     Removed references to FEATURE_AOC and FEATURE_MMGSDI.
02/25/03   AT      Fix 11.10 case to now drop calls on SIM error.
02/03/03   AT      Modified to make sure Max Charge hit is displayed when
                   ACM reaches ACMmax.  Set the end_status in the call object.
01/21/03   ws      Updated copyright information for 2003
11/22/02   ic      Removed unused variable from cm_aoc_save_acm()
11/14/02   vt      Merged changes from GW mainline.
10/30/02   AT      Fixed unnecessary error message when using Async function.
                   Added some F3 messages to help determine charges at warning
                   and at call being dropped.
10/25/02   AT      Utilize MMGSDI's Async function to help eliminate latency.
09/13/02   HQ      MM GSDI changes.
09/08/02   AT      Fixed problem AOC undercharging.
07/30/02   AT      Completed all GSM 11.10 tests.  Fixed minor charge issues.
06/21/02   AT      Fixed a problem where we were incorrectly rounding up CCM
04/26/02   AT      Fixed a problem where we double charge for the first segment.
04/24/02   AT      Fixed an error with multicall.
                   Added F3 messages for Charges for the call.
04/22/02   AT      Updated with changes for Advice of Charge.
04/16/02   AT      Merged Advice of Charge with GSDI and UI.
03/11/02   AT      Fixed the ending of the call when we are out of funds.
01/30/02   AT      Fixed comments from code reviews.
12/13/01   AT      Integrated this file with GSDI for both GSM SIM and USIM.
11/15/01   AT      Updated to allow for the response back to MN.
                   Also this file now lints cleanly.
11/01/01   AT      Added a function to reset the ACM.
09/01/01   AT      Initial release.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "customer.h"  /* Customer configuration file */

#if( defined (FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
#else /* FEATURE_WCDMA || FEATURE_GSM */

/*===========================================================================
FUNCTION cmwaoc_dummy

DESCRIPTION

  Dummy function, defined to get rid of
  'Warning: C3040E: no external declaration in translation unit'
  ADS 1.2 compiler warning

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void cmwaoc_dummy( void )
/*lint -esym(714,cmwaoc_dummy) */
/*lint -esym(765,cmwaoc_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}


#endif /* FEATURE_WCDMA || FEATURE_GSM */
