/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                  C A L L   M A N A G E R


          S U P P L E M E N T A R Y   S E R V I C E   M O D U L E

GENERAL DESCRIPTION
  This module contains the Call Manager Sups Object functions for WCDMA/GSM.

  The Sups Object is responsible for:
  1. Processing clients' call independent sups commands.
  2. Processing Lower Layer replies for clients' sups commands.
  3. Processing Lower Layer call independent sups related notifications.
  4. Notifying the client list of sups events.


EXTERNALIZED FUNCTIONS

  cmsups_init
    Initializing the sups object.

  cmsups_client_cmd_proc
    Process call independent sups commands from the clients.

  cmsups_rpt_proc
    Process Lower Layer sups notifications/command replies.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  cmsups_init() must be called to initialize this module before any other
  function declared in this module is being called.


Copyright (c) 2001 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmwsups.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/26/09   mp      Updated SUPS event notification in SUSPEND state
12/28/08   ks      Copying L3 data to sups_info ptr for more sups cnf
09/05/08   rn      FDN checking option in call control
08/07/07   ic      Lint clean up
05/16/07   cl      Fix typo in include file AEEstd.h
05/02/07   rp      Adding FEATURE_CM_LCS
04/15/07   rp      Removing FEATURE_MMGPS
02/23/07   pk      Banned API lint fixes
02/08/07   rp      Copying L3 data to sups_info ptr for interrogate cnf.
01/19/07   pk      Reverting previous lint fixes
01/12/07   pk      Lint fixes
12/19/06   ka      Adding support to process UCS2 in CM_SUPS_CMD_PROCESS_USS.
11/09/06   pk      Added support for HYBR_GW and UMTS -> 1X handover cont...
09/29/06   rp/ka   Added code to initialize all the fields in sups object.
09/13/06   rp      Added code to copy Layer 3 message from MN to sups object
                   and sups client object and featurized under 
                   FEATURE_SS_L3_DATA 
08/21/08   ka      Fixing cmsups_ussd_phase1_req() by adding a call to
                   cm_mn_send_cmd().
07/04/06   ka      Changes to support call control on PS calls.
01/03/06   ic      Fixed compile error in cmsups_ussd_phase1_req()
                   <argument 2 to 'strncpy'>: implicit cast of pointer to 
                   non-equal pointer
                   Fixed compile error in cmsups_ussd_phase1_req() 
                   <argument 1 to 'cm_util_gsm_alphabet_to_ascii'>: implicit 
                   cast of pointer to non-equal pointer
                   Correcting fields being sent for ussd phase 1 request.
01/12/06   ic      For specific calls / walks Lint warnings, replaced global 
                   -fsp comment with -e{error#} comments where needed
01/06/06   ic      Lint cleanup  
12/13/05   ic      Fixed compiler warnings 
11/08/05   sk      Separated CM reports and commands.
11/03/05   ic      Added CM_ASSERT() for ptr function parameters and lint 
                   comments for 613 Lint error 
08/02/05   ic      Print value that caused error in default switch case
07/25/05   sk      Replaced (for fixing CR 67893) 
                   cm_util_ascii_to_def_alphabet() with 
                   cm_util_ascii_to_gsm_alphabet().
06/24/05   ic      Adding CCBS feature (merged in P4 change list 204600)
04/06/05   ka      Change #define name
02/09/05   ic      Lint cleanup
01/21/05   ka      Changing encoding scheme for CM_USSD_DCS_UNSPECIFIED to
                   be the same as 7bit encoding.
01/12/05   ka      Support for 7bit/8bit/unspec USSD data coding scheme.
11/15/04   ic      Lint cleanup with CM_DEBUG on
09/13/04   ka      Handling CM_CC_RESULT_NOT_ALLOWED in sups call control
                   call back.
08/26/04   sbs     Added externalAddress and nameString IEs to MOLR_Invoke /
                   LocationNotification SS messages
08/17/04   ic      Undoing P4 change list #121638
08/04/04   ic      CM Lint clean up - cmwsups.c with Lint supression verified
                   and free of Lint issues in all CUTE configurations.
07/27/04   ws      Add Call Control support for SUPS.
07/26/04   ic      Changed include file order to customer.h, then cm.h then
                   cmi.h
                   Added cmwsups_dummy() to get rid of ADS 1.2 compiler warning
07/07/04   ka      Fixed Lint errors.
05/27/04   ic      In cmsups_client_cmd_proc() call cmsups_client_cmd_err()
                   irrespective of whether error occured or not (client is
                   supposed to be always notified about command processing
                   status)
05/14/04   ka      Fixed Lint errors.
03/16/04   ka      Merged changes from 6250. Fixed Lint errors.
02/25/04   ic      Changes to make the code pass unit test cases and at the
                   same time make calls in Saber build
02/06/04   ws      Initial jaguar/mainline merge.
01/13/04   ka      Merged changes from MSM6200
12/19/03   RI      Added setting of USS data present field when a Phase 1
                   response is sent to clients in a Phase 2 structure.
11/19/03   sbs     Added LCS MOLR Completed API function
07/08/03   sbs/lt  Added support for Location Services related SS messages
06/20/03   prk     Featured out copy_fwd_to_number() to remove compiler warning.
                   Currently not used but may be used in the future.
06/04/03   RI      Modified the processing of forwarded to number information
                   in the SS confirmations.
04/28/03   AT      Lint cleanup.
04/11/03   prk     Removed references to FEATURE_SUPS.
03/13/03   RI      Updated the processing of USS indication in
                   cmsups_rpt_proc().
01/21/03   ws      Updated copyright information for 2003
11/14/02   vt      Merged changes from GW mainline.
                   [RI]Updated the processing of USSD confirmations.
05/17/02   PRK     Removed comment.
05/16/02   RI      Corrected USS problems.
05/07/02   PRK     Renamed to cmwsups.c.
04/25/02   RI      Added processing of CM_PROCESS_UNSTRUCTURED_SS_DATA_CONF.
03/11/02   RI      Modifications as per new interface definitions for
                   call independent ss messages.
                   - confirmation messages are processed based on boolean
                   flag 'success'
02/04/02   RI      Removed mn_cm_exp_t.h inlcude file.
                   - removed the IEI processing in the messages received
                   from the lower layer (MN) and setting of IEI fields
                   in the messages sent to the lower layer (MN).
01/14/02   PRK     Checked service status flag instead of service state flag
                   in function cmsups_client_cmd_check.
                   Replaced prefix cmutil_ with cm_util_.
11/28/01   RI      Commented out the password fields in activate and deactivate
                   SS requests.
11/21/01   RI      Fixed ADS compiler errors.
11/05/01   RI      Modified handling of IEI fields in SUPS commands
                   and events.
                   Added copy_ss_info_fields, copy_ss_error,
                   copy_bsg_list, copy_ff_list, copy_fwd_to_number.
10/18/01   PRK     Added changes for WCDMA/GSM mode.
                   Made modification due to srv_state, moving from
                   phone group to serving system group (FEATURE_NEWSS).
09/25/01   RI      Initial release for WCDMA/GSM.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
/*lint -esym(766,customer.h) */
#include "customer.h"

#if ( defined(FEATURE_WCDMA) || defined(FEATURE_GSM) )
#error code not present
#else /* FEATURE_WCDMA || FEATURE_GSM */

/*===========================================================================
FUNCTION cmwsups_dummy

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
void cmwsups_dummy( void )
/*lint -esym(714,cmwsups_dummy) */
/*lint -esym(765,cmwsups_dummy)
** Can't be static as it would result in compiler warnings 
*/ 
{
}


#endif /* FEATURE_GSM || FEATURE_WCDMA */
