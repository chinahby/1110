/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


              U I M   G E N E R I C   S T A T E   M A C H I N E

GENERAL DESCRIPTION
  This module contains the state machine for the generic protocol of the UIM
  interface. Generic module processes all the commands that are common to
  R-UIM, SIM and USIM.

EXTERNALIZED FUNCTIONS

  uim_process_generic_command
    Processes a generic UIM server command

  uim_process_generic_response
    Processes the responses for a generic UIM server command

  uim_send_generic_err_rpt
    Processes an error condition for a generic UIM server command

  uim_generic_command
    This function contains the state machine that processes APDUs
    for generic commands.

  uim_generic_command_response
    This function processes the UIM responses from APDUs.

Copyright (c) 2001,2007-2010 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/uimgen.c_v   1.30   14 Oct 2002 16:37:22   jruvalca  $
$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3.65/uim/uimdrv/src/uimgen.c#2 $ $DateTime: 2010/02/17 07:40:30 $ $Author: rmandala $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/10   rm      Workaround to send card error when 
                   FEATURE_UIM_INTERFACE_NOT_USED is defined
08/03/09   yb      Fix to do get response for seek command if a GCF test case
                   is running or NV_ENS_ENABLED_I NV item is enabled.
07/16/09   vs      Added handling for ICCID read failure with MFLO CAS card
07/01/09   yb      1. Fix to do get response for envelope command when card 
                   responds with warnings.
                   2. Fix to report warnings to GSTK on receiving warnings in
                   Envelope response.
06/17/09   sk      Fixed Issue with change_chv functionality
06/12/09   yk      Featurized the fixes to support cards that don't have CDMA
                   DF for only 1x builds.
06/10/09   yb      Fix to send status word as received in get response to AT COP
                   even if card returns error for envelope sent.
06/02/09   yb      Fixed handling of class C cards with FEATURE_UIM_USB_UICC on
06/04/09   rm      Fixes to handle technical problem reported by the card
06/01/09   ssr     Fixed compiler warnings
05/08/09   ps      Fixed 8200 compiler warnings
05/06/09   vs      Fixed state processing of UIM_POWER_DOWN_F
04/20/09   vs      Re-introducing changes made for DI computation, power down
04/17/09   js      Backing out changes made for - Updating DI computation, faking
                   power down if card is already powered down, forcing T=0 protocol
                   with feature, handling absent DF CDMA
04/17/09   js      Fixed misconfigured featurization
04/17/09   vs      Updated DI computation, faking power down if card is already
                   powered down, forcing T=0 protocol with feature, handling
                   absent DF CDMA
04/06/09   js      Support for accessing EF-by-path with proprietary DFs
03/25/09   sun     Added support for Virtuim
03/13/09   vs      Fixed power up issue for MFLO cards without ICCID
02/19/09   sun     Removed MBMS Featurization
02/18/09   vs      Updated IC-USB logic to execute only if IC-USB is selected
12/03/08   js      Support for accessing GSM Cingular DF by path
11/05/08   vs      Update to allow UIM clock stop only after sending the first
                   instruction to the card in passive mode
10/08/08   rm      Cached select support for T=1 protocol
10/07/08   vs      Fix to update record data when proessing UIM_WRITE_REC_CMD_F
10/06/08   jk      Fixed featurization compile error
09/19/08   rm      Fix to set non-cached select for T=1 protocol temporarily
09/17/08   vs      Updated UIM USB ICCD poll delay timer logic
09/17/08   js      Implemented UIM Card interface selection based on first
                   instruction class NV
09/15/08   vs      Removed featurization FEATURE_UIM_USB_UICC_SUSPEND_RESUME
09/12/08   nb      Added support to handle warnings returned by Envelope Command
09/06/08   kk      Fixed Lint Error
09/05/08   vs      Moved fallback timer and signal clearance to UIM USB module
09/05/08   kk      Fixed Featurization Issue
08/26/08   kk      Few comments from uim directory management review
08/20/08   vs      Added support for ATR poll request by USB UICC, ICCD power
                   off when power down cmd is received and ISO fallback
08/14/08   kk      Added handling for SELECT EFs with non-generic file ids
08/08/08   vs      Removed featurization UIM_CLOCK_STOP_ALLOWED
08/08/08   ssr     Changed the order of FEATURE_UIM_RUIM for successful authentication
08/05/08   vs      Renamed start and stop clock functions appropriately
07/25/08   rm      Fix for ICCID read failure
07/17/08   vs      Added support for UIM USB UICC ICCD interface
07/17/08   kk      Added support for PKCS 15
05/28/08   rm      TA1=0x97 feature support.
05/06/08   sk      Do not retry command when "toolkit busy" response is
                   received for an envelope.
04/01/08   vs      Fixed compiler warning
03/25/08   rm      Removed changes which did not allow clock stop to happen
                   on LCU
03/18/08   vs      Fixed Klocwork errors
03/13/08   rm/jar  Disabling the clock stop temporarily in LCU
02/23/08   jar     Changed byte type to unit16 to avoid an infinite loop when
                   a 256 byte non spec conforming Select response is received.
12/17/07   rm      Fix to correct the instruction case in some situations
07/18/07   sp      Featurize uim_free_reset_auth_info()
07/02/07   sun     Added support for ODD INS Class
06/19/03   sp      Don't reselect MF for a select failure in streaming APDU mode
05/03/07   jk      Fixed compile warning
03/29/07   sp      Return no data for a select resp on App termination
03/07/07   sun     Added support for Disable with Replacement
02/13/07   pv      Use uim_set_poll_timer instead of rex_set_timer
01/11/07   tml     Fix compilation issue with 1x
12/11/06   tml     Clear the uim_last_sel_dir for the specific channel and slot
                   upon select request for app termination
11/13/06   pv      Store the protocol to be used for polling from the
                   streamed APDU header if in passive mode and start polling
                   only after the protocol is known.
10/09/06   pv      Allocate memory and assign values to uim_rd_wr_info
                   though not the first attempt.
                   Assign the offset properly for UIM_READ_BIN_CMD_F.
09/28/06   pv      Consider an external reset as a initial power-up rather
                   than internal reset which might perform TP-DL.
                   Do not send a link_established for external reset
                   since an internal power-up would send it out any way.
09/25/06   pv      Use uim_clk_busy_wait instead of clk_busy_wait.
08/18/06   pv      Consider ORANGE DF as level 1 df if df1 type is not an ADF.
08/16/06   sp      Added a check for protocol in pin operation commands
08/02/06   sun     Added case statement for UIM_RESET_SWITCH_UIM_PASSIVE_F in
                   uim_send_generic_err_rpt
07/13/06   pv      Compilation error fixes when FEATURE_UIM_SUPPORT_CSIM IS
                   not defined.
07/07/06   pv      Allow streaming of USIM commands on the default channel.
06/12/06   pv      Get rid of FEATURE_UIM_USIM_INTERNAL_VERIFY_PIN.
                   Notify internal reset to GSDI.
                   Add more debug messages.
                   Do not compare directory structure with stale date when poll
                   fails.
                   Internal reset would not to a TP DN any more.
06/07/06   jar     Terminate the UIM Paths to be selected with UIM_NO_FILE_SEL
                   in UIM_RESET_ST due to the possibility with some cards to
                   have a non 0xFFFF item as the second element of the array.
04/20/06   pv      Featurize out code using cmd in uim_update_last_sel_dir
                   based on FEATURE_UIM_UICC to compile when the feature is
                   not defined
04/03/06   pv      Lint Fixes
03/29/06   pv      Take down the interface and notify gsdi if bad/unknown
                   status words are received while polling if
                   FEATURE_UIM_RPT_BAD_SW_ON_POLL is defined.
                   Add message to print status words when un-known status
                   words are received.
                   Do not consider a select as an intermediate select while
                   in check-char state.
03/09/06   pv      Added additonal NULL check to avoid possible error messages.
                   Also, avoid re-execution of the clear cache code
                   on the item for which the cache was already cleared.
                   initializing select_rsp_cache_item_ptr
03/09/06   pv      Added new command to clear the response cache
                   and Load the status words to the report buffer even if we
                   errored out because of un-known status words
03/01/06   pv      Clear the get response cache on receiving an external RESET
                   command
02/15/06   pv      Select by path from MF if the path starts with 7FFF.
                   Reduce the additional buffer for CWT  and BWT.
                   time out values with additional checks in UIM task to
                   ignore spurious timeout signal.
02/14/06   nk      Merge for: Use heap_small when the size of get response
                   allocation is less than 64 bytes; Remove uim rows from usim
                   path table for enums removed; and Added support to cache the
                   get response data and not do a get response if the item is
                   already cached.  Also, expose a new API to not send down the
                   select for the sake of getting the get response information
02/03/06   pv      Revert the earlier change.
01/30/06   pv      Reduce the additional buffer for CWT  and BWT
                   time out values
01/30/06   pv      Do not perform internal fetch while in passive mode.
                   Notify GSDI about any reason for power down.
01/25/06   pv      Allow select by path from MF during the power-up procedure
                   by moving the setting of the flag from
                   uim_process_generic_command to uim_generic_command.
                   Featurized power down function call for the previous
                   check-in based on DUAL SLOT definition.
01/18/06   pv      Added code to do a cold reset instead of a warm reset
                   when a reset command is received to change the instruction
                   class to GSM.
11/16/05   wli     Changes to support no-iccid card.
11/14/05   pv      Lint Fixes
11/01/05   tml     Combine last sel EF with last sel Dir
11/01/05   jk      Corrected compile error on 7500 (sticter compliler)
10/21/05   pv      Delayed the decision to skip pps from RESET_ST to
                   UIM_DELAY_AFTER_ATR_ST
10/20/05   jk      Added changes to ensure that implicit status words are 0
                   before streaming 7816 apdu is sent
10/19/05   pv      Added code to introduce delay between the reception of the
                   ATR and sending the first byte of the PPS request.
10/06/05   pv      Code to allow select by path from MF for external select
09/13/05   jk      Added changes to keep interface powered down during voltage
                   switch for minimum time specified in ISO7816
09/14/05   wli     Fixed compilation error for feature
                   FEATURE_UIM_USIM_INTERNAL_VERIFY_PIN
08/25/05   pv      Code to notify GSDI upon pdown based on the option.
                   Notify client upon pdown failure.
                   Reset command sets uim_status to un_initialized so that the
                   response could be properly processed
08/15/05   tml     Treated UIM_INCORRECT_PARAM_IN_DATA_FIELD as an indication
                   of wrong instruction class in Check Characteristics state
07/28/05   pv      replaced UIM_MAX_NUM_CDD_ATTEMPTS with UIM_MAX_NUM_ATTEMPTS
                   where not necessary.
07/27/05   jk      Made work waiting time array for dual slots
07/26/05   pv      Check to see if command expects return data in poll
                   rather than checking the vote for traffic channel.
07/22/05   pv      Expect no data in return for poll while not in traffic.
07/18/05   sun     Added support for USIM files in ENS
07/13/05   pv      Fixes for STATUS with NO Data return p2 parameter
06/22/05   pv      changes to handle poll when in passive mode
06/13/05   pv      Code to support UIM commands with allocated memory.
06/08/05   wli     Replaced UIM_MAX_NUM_ATTEMPTS with UIM_MAX_NUM_CMD_ATTEMPTS
                   for command retry.
06/02/05   pv      Code for FEATURE_UIM_ISIM & FEATURE_UIM_SESSION_MANAGEMENT
05/10/05   pv      BT Sim access changes - Handle RST passive mode,
                   PUP and PDOWN
05/10/05   sst     Lint fixes
04/29/05   jk      Changes to set report data len=0 and sw1 and sw2 = 0 when
                   error report being sent for stream 7816 APDU
03/18/05   pv      Changes for Jaguar dual-slot multiplevoltage support
02/22/05   pv      changed uim_first_inst_class based on the protocol
                   supplied when a reset command is queued.
02/03/05   jk      Changed FEATURE_UIM_RUN_TIME_ENABLE to FEATURE_UIM_RUIM
                   around CHECK_FOR_CDMA_DF to allow CDMA to function okay
                   in 1x builds without RUN_TIME_ENABLE
01/28/05   pv      Changed the location where we set the atr_received flag
                   so that we will not set the flag if the ATR is corrupted
01/21/05   wli     Extend work waiting timer value for an out of spec RUIM card.
12/28/04   pv      computed uim_work_waiting_time_sleep_ticks in order to compare
12/27/04   jk      Fixed Compiler warnings.
12/16/04   jk      Merged changes for Orange support from mainline
11/21/04   wli     Dual slot merge and debug.
11/15/04   pv      Added computation for block guard time for T=1 protocol
11/01/04   pv      Change + 20 to CLK_MS_PER_TICK while computing
                   work_waiting time to be able to turn off the interface
                   within 960 ETUs
10/22/04   tml     Select MF immediately after a select failed
10/12/04   jar     Fixed Compile Error
10/11/04   pv      Added code to parse the ATR for voltage class information
09/25/04   jar     Added fix to zero out get resp status words for a
                   UIM_STREAM_ISO7816_APDU_F cmd.
09/23/04   jar     Added fix to keep UIM from an infinite loop due to hack.
09/09/04   ck      Added a hack to update last selected directory structure with
                   second level ORANGE DF which has an ID as first level DF.
09/01/04   ck      Added support for CSIM.
08/17/04   ck      Moved the setting of protocol information to the reset state.
08/16/04   pv      Saved protocol information irrespective of the last
                   selected DF when the protocol is GSM.  Without the change,
                   the protocol is un-known when reporting link_established
08/11/04   ck      Lint changes.
07/14/04   ck      Brought back resetting just EFs when SELECT failed to work
                   with cards that do not maintain their state upon failure
                   to select an EF.
06/16/04   ck      Removed resetting of directories upon a failed SELECT as it
                   could cause polling errors.
05/17/04   ck      Added support for ISO 7816 streaming API.
04/28/04   ts      Added support for > 256 byte writes to transparent files.
04/14/04   ts      Added an extra 2etu delay when turning off the UIM clock.
                   This is needed to compensate for getting the byte before
                   the stop bits.
04/08/04   wli     Featurize for non-gsdi targets.
03/18/04   ts      Back out the previous change because it caused the
                   instruction class switch to fail.
03/15/04   ts      Make sure instruction class is set to USIM on first power
                   up.  This is needed for the second slot if instruction class
                   is changed to GSM in the first slot bring up.
03/13/04   ck      Removed the featurization around gsdi_link_est in response
                   to a successful UIM_RESET_F
                   Merged in hack to toggle the instruction class to GSM for a
                   SIM card that accepted a UICC command but did not return FCP
                   template for the MF select that resulted in Voltage mismatch
02/20/04   ts      Fixed lint errors.
02/18/04   ck      Fixed Lint errors.
02/09/04   ck      Added checks for rec_len before dividing the record length
                   to arrive at the number of records for non transparent file
                   structures.
01/27/04   ck      Added support to send the link est command to GSDI upon
                   completing the UIM_RESET_F command for dual slots.
01/15/04   ck      Added support to reset the elements of the last selected dir
                   structure based on the file ID that was attempted.
01/13/04   ts      Fixed the multi-voltage code because of the multi-slot
                   changes.
01/12/04   ck      Removed duplicate case statements for UTIL commands in
                   the Stream APDU command processing.
12/29/03   ck      Fixed an issue with POLLING_OFF that was not returning the
                   correct value to GSTK in the report.
12/09/03   ck      Deleted resetting of the directories upon receiving an error
                   for Select
                   Set the internal timer after subtracting the Poll offset to
                   meet the deadline.
10/31/03   ts      Set P3 to maximum size for accesses greater than the maximum
                   transaction size.
10/23/03   ck      Merged in changes to support 18etus for certain cards that
                   require higher turnaround delay.
10/28/03   ck      Removed duplicate case for RUN Cave in APDU Stream processing
10/17/03   ck      Added support to stream APDU.
10/13/03   ts      Added processing to set guardtime to minimum for T=1.
10/10/03   ck      Fixed a compile issue with use of the flag reset_with_tp in
                   single slot builds.
10/08/03   ck      Fixed an issue where the toolkit was not switching when
                   the serving system (CDMA /GSM ) changed.
                   - Changed the order of CHECK_FOR_CDMA_DF and TERMINAL_
                     PROFILE_ST in internal wake up state.
09/19/03   ck      Merged in the fixes to pass GCF test cases
                   - Check for invalid Start character TS in ATR
                   - Make sure that PPS is resent twice following a time-out
                     with the third attempt using the default PPS.
09/11/03   ck      Fixed an issue with polling in UICC protocol whent the last
                   selected directory was MF. Also cleaned up polling to parse
                   the information in the response rather than relying on the
                   index for specific information.
09/02/03   ck      Added support to set the CDMA directory present in the nvruim
                   structure for dual slots after changing the preferred slots.
08/22/03   ck      Added a USIM error condition to work with a non-standard GSM
                   card for toggling the instruction class.
08/15/03   ck      Set the toolkit slot in the TP download state since a
                   FETCH could be in response to the download command.
08/15/03   ck      Code review comments checked in.
08/14/03   ck      Added support for dual slots
07/22/03   ts      Change the TA(1) parameters for the PPS to values the MSM
                   can support (due to a slow UART clock).
06/13/03   ck      Added featurization for message that displays the operating
                   protocol.
06/05/03   ck      Fixed an issue where a select by path reference from MF was
                   attempted when the ADF was part of the path causing failure.
06/03/03   ck      Added support to not count the convention change and
                   instruction change as attempts at power up.
05/23/03   ts      Added message to indicate transmission protocol for QXDM
                   messages.
05/01/03   ck      Set the protocol for the FETCH command based on the last
                   successful command.
04/28/03   ts      Added support for multi-Voltage classes.
04/28/03   ts      Added support for T=1 protocol.
04/22/03   wli     Rearranged GSTK/UTK feature.
04/22/03   jk      Added UTIL and WIM support
02/27/03   wli     Store ICCID if command is UIM_RESET_F.
02/19/03   ts      Fixed lint issues from GSTK support code.
02/14/03   ck      Fixed the ATR parsing error that resulted in incorrect
                   work waiting time.
02/13/03   wli     Added support to GSTK.
02/03/03   ck      Set the command count to MAX_NUM_ATTEMPTS to prevent the cmd
                   from being re-tried when a memory failure is recd for SMS
                   writes ( Update record state ) - to satisfy a GCF test case!
01/29/03   ck      Added additional checks for incorrect p1 and p2 before
                   toggling the instruction class in CHECK_CHAR_ST
12/20/02   ts      Fixed Lint issues
11/26/02   ts      Use only max UIM frequency supported.  No change during PPS.
11/21/02   ts      Added a new state to check for the presents of the CDMA DF
                   for RTRE.
11/21/02   ts      Use default PPS for TA1 values that are invalid.
10/29/02   ck      Removed setting of count to UIM_MAX_NUM_ATTEMPTS in the
                   CHECK_CHAR_ST. Added a flag to indicate that ATR has been
                   recd that can be used in the task to set the count.
                   Set the uim_atr_flag to TRUE upon receiving an ATR.
10/25/02   jar     Don't set cmd_ptr->hdr.cmd_count=UIM_MAX_NUM_ATTEMPTS
                   in UIM_CHECK_CHARACTERISTICS_ST to allow toggling - SIM/USIM
10/24/02   ts      Added support for work waiting time
10/24/02   ts      Only allow one attempt for power up after ATR.
10/24/02   ts      Do not set or clear response timeout.  Timeout ishandled in
                   the driver.
10/24/02   ts
10/14/02  ck/jar   Fixed Verify PIN Issue for USIMs.
10/11/02   ck      Added support to make use of the NV item first instn class
10/02/02   ts      Reject SIMs that request a guardtime between 0 and 255.
09/20/02   ts      Use a default max UIM clock frequency of 4MHz since we are
                   operating at 3V.  per spec.
09/19/02   ck      Added checks for UIM_UNKNOWN_INS before switching instrn
                   class
09/13/02   wli     Added feature to verify CHV2 after wake up.
08/12/02   ck      Added support for File selection by path reference.
07/28/02   sb      Merged changes made in rev1.20.1.0 made by pmarsh:
05/22/02   plm     Replaced reference to T_KOVU, T_MSM5200 and T_MSM6200 with
                   T_UIM_CLK_MAX_FREQ defined in targetXXXX.h.
07/18/02   wl      Verify pin after a ADF has been selected.
07/09/02   ck      Added support for AID selection by file ID.
05/15/02   ts      Fixed #ifdef's for using tcxo/4 for MSM5200, Kovu and
                   MSM6200.
05/09/02   ts      Included MSM5200, Kovu and MSM6200 to targets that can handle
                   a UIM clock of tcxo/4.
04/30/01   ck      Modified the Enhanced search to use the search offset in the
                   command while building the APDU.
04/09/02   ck      Modified the featuization around clock stop functions to
                   include FEATURE_UIM_USIM.
03/27/02   ck      Modified the Power up procedure to detect the protocol of the
                   card based on the protocol class in the first command sent to
                   the card.
                   and removed detection based on the historical characters.
02/01/02   ts      Process the file characteristics from CDMA or GSM to determine
                   if the card supports 3V technology.
01/22/02   ts      Added processing of file characteristics from CDMA or GSM DF
                   or MF. This is used to determine clock stop preferences from
                   the card.
01/08/02   ck      Fixed an issue where the num bytes in an access command was
                   compared with only the low word of the EF file size that
                   resulted in an incorrect PRL being read when the PRL was
                   greater that 255 bytes.
12/07/01   ck      Added caching of Terminal Profile data to send down the
                   TERMINAL PROFILE command after a RESET.
                   Added setting of instruction class for Verify CHV1 command.
                   Added RESET command processing.
12/04/01   ts      Added support for UIM toolkit.
11/19/01   ts      Fixed USIM detection for USIM/GSM code.
10/19/01   ck      Fixed the problem where VERIFY_CHV report was being sent for
                   a CHANGE CHV command.
10/11/01   ck      Added support to change the intruction class of SELECT command
                   while attempting to read the ICCID, when the historical chars
                   do not comply with ISO specs ( carrier specific hist chars in
                   a SIM Card ).
07/27/01   ts      Changed the operational parameters to use the parameters sent in
                   the ATR or PPS.
07/26/01   wli     Changed several "switch" statements to direct assignment. This
                   is to simplify the code and save rom space.
06/26/01   ts      Changed clock control feature switch names to have "FEATURE_"
                   prepended.  Changed clock control names to indicate the tcxo
                   relationship as opposed to the actual clock frequency.
06/12/01   ts      Fixed the PPS determination.  Code will now skip the PPS if
                   TA1 is not received.
05/28/01   ck      Added support for multi mode server.
01/16/01   ts      Created module.  This module contains the generic state
                   machine.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "intconv.h"
#include "target.h"
#include "tmc.h"

#ifdef FEATURE_UIM
#include "customer.h"
#include "msg.h"
#include "err.h"
#include "task.h"
#include "uimdrv.h"
#include "uimi.h"
#include "uimgen.h"
#include <memory.h>

#if defined( FEATURE_UIM_T_1_SUPPORT )
#include "uimt1.h"  /* T=1 protocol support */
#endif /* FEATURE_UIM_T_1_SUPPORT */

#if defined( FEATURE_UIM_RUIM )
#include "nvruimi.h"
#endif /* FEATURE_UIM_RUIM */

#include "clk.h"

#include "gsdi_exp.h"

#ifdef FEATURE_GSTK
#include "gstki.h"
#endif

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#ifdef UIM_DRIVER_TIMESTAMP
  #define SLEEP_COUNTER_FREQ     32800
#endif

/* This defines the amount of time to delay after receiving the ATR and
 * before sending out a PPS request to the CARD
 */
#define UIM_DELAY_TIME_AFTER_ATR    1

/* This defines the delay after powering the UIM and after PPS. */
#define UIM_CARD_DELAY_TIMEOUT 20
#define BGT_22_ETU             22
/* Buffer to store the historical characters */
uim_hist_char_buf_type uim_hist_char_buf[UIM_NUM_DRV_SLOTS];

#ifdef FEATURE_UIM_FLEXIBLE_COMPLIANCE
  /* This defines the size of the historical characters for cards that need
     extra delay before the response */
  #define UIM_HIST_CHAR_CMP_SIZE 9
  LOCAL byte const UIM_MORE_DELAY_HIST_CHARS[] =
  {0x4D, 0x4D, 0x41, 0x52, 0xF2, 0x2B, 0x56, 0x32, 0x2E};
  #define UIM_MIN_MINOR_REV_MORE_DELAY '3'
  #define UIM_MAX_MINOR_REV_MORE_DELAY '4'
#endif

#ifdef FEATURE_UIM_UICC
/* Bits set indicating SFI is used */
#define SFI_USED_IN_P1         0x80
#define SFI_USED_IN_P2         0xF8

/* Bits indicating Select parent or child or by path from current DF */
#define UN_SUPPORTED_SEL       0x01
#endif
#ifdef FEATURE_UIM_TOOLKIT
/* Type to store the terminal profile data */
typedef struct {
  byte           num_bytes;         /* Number of bytes in profile */
  byte data[UIM_TERMINAL_PROFILE_DATA_LENGTH]; /* Terminal Profile */
}uim_int_terminal_profile_data_type;

/* Terminal profile data buffer */
LOCAL uim_int_terminal_profile_data_type uim_int_term_profile_data = {0,"\0"};
#endif /* FEATURE_UIM_TOOLKIT */

/* Clock stop control */
LOCAL uim_clock_stop_cntrl_type uim_clock_stop_cntrl[UIM_NUM_DRV_SLOTS];

/* This defines the delay between turning on and off the UIM clock and
   using the UIM interface. */
//#define UIM_STOP_START_TIMEOUT 1

#ifdef FEATURE_UIM_HANDLE_TA1_0X97_ATR
/* This is a variable to keep a count on the warm resets. */
unsigned char warm_reset_count = 0;
#endif /* FEATURE_UIM_HANDLE_TA1_0X97_ATR */

/* State of the UIM */
LOCAL const uim_generic_state_type *uim_generic_state_ptr;

/* Op params buffer */
uim_op_params_type uim_op_params_buf;

/* Pointer to the above buffer */
LOCAL uim_op_params_type *uim_op_params_ptr = &uim_op_params_buf;

/* Indicates use minimum guardtime from ATR */
LOCAL boolean uim_use_minimum_guardtime_size;

/* Pointer to the Global UIM Request buffer */
LOCAL uim_req_buf_type *uim_req_buf_ptr = &uim_req_buf;

/* Variable to indicate the mode in which UIM is operating */
uim_mode_type uim_mode      = UIM_NORMAL_MODE;

/* Variable to hold the previous mode */
uim_mode_type uim_save_mode = UIM_NORMAL_MODE;

/* State configuration for the commands. */

/*  UIM_INTERNAL_ME_PUP_F    = 0x0100, POWER UP UIM due to task start up */
LOCAL const uim_generic_state_type UIM_INTERNAL_PUP_STATES[] =
{ UIM_POWER_UP_ST, UIM_RESET_ST,
  UIM_DELAY_AFTER_ATR_ST, UIM_PPS_ST, UIM_UPDATE_OP_PARAMS_ST,
#if defined( FEATURE_UIM_T_1_SUPPORT )
  UIM_IFS_NEG_ST,
#endif /* FEATURE_UIM_T_1_SUPPORT */
  UIM_CHECK_CHARACTERISTICS_ST,
  UIM_SELECT_ST, UIM_READ_ICCID_ST,
#if defined( FEATURE_UIM_RUIM ) || \
    defined (FEATURE_UIM_SUPPORT_DUAL_SLOTS)
  UIM_CHECK_FOR_CDMA_DF,
#endif /* FEATURE_UIM_RUIM */
  UIM_DONE_ST };

/*  UIM_INTERNAL_WAKE_UP_F,         Power up due to Power management */
LOCAL const uim_generic_state_type UIM_INTERNAL_WAKE_UP_STATES[] =
{ UIM_POWER_UP_ST, UIM_RESET_ST,
  UIM_DELAY_AFTER_ATR_ST, UIM_PPS_ST, UIM_UPDATE_OP_PARAMS_ST,
#if defined( FEATURE_UIM_T_1_SUPPORT )
  UIM_IFS_NEG_ST,
#endif /* FEATURE_UIM_T_1_SUPPORT */
  UIM_CHECK_CHARACTERISTICS_ST,
  UIM_SELECT_ST, UIM_READ_ICCID_ST, UIM_VERIFY_CHV1_ST,
#if defined( FEATURE_UIM_RUIM ) || \
    defined (FEATURE_UIM_SUPPORT_DUAL_SLOTS)
  UIM_CHECK_FOR_CDMA_DF,
#endif /* FEATURE_UIM_RUIM */

  UIM_DONE_ST
};

/*  UIM_RESET_SWITCH_UIM_PASSIVE_F,         Power up making UIM passive */
LOCAL const uim_generic_state_type UIM_RST_SWITCH_UIM_PASSIVE_STATES[] =
{ UIM_POWER_UP_ST, UIM_RESET_ST,
  UIM_DELAY_AFTER_ATR_ST, UIM_PPS_ST, UIM_UPDATE_OP_PARAMS_ST,
#if defined( FEATURE_UIM_T_1_SUPPORT )
  UIM_IFS_NEG_ST,
#endif /* FEATURE_UIM_T_1_SUPPORT */
  UIM_DONE_ST
};

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

/* additional info for read and write and inc commands*/
LOCAL uim_rd_wr_info_type uim_rd_wr_info;

LOCAL uim_generic_state_type uim_generic_states[UIM_MAX_NUMBER_GEN_STATES];

/* Counter for SELECT sub state machine */
LOCAL byte select_state_ctr = 0;

/* PPS Request Type */
LOCAL uim_pps_req_buf_type uim_pps_req_buf;
LOCAL uim_pps_req_buf_type *uim_pps_req_buf_ptr = &uim_pps_req_buf;

/* Buffer to hold the ATR */
uim_atr_buf_type uim_atr_buf[UIM_NUM_DRV_SLOTS];

/* Buffer to hold the PPS characters */
LOCAL byte uim_pps_char_buf[UIM_MAX_PPS_CHARS];

/* Buffer to hold the ICCID */
LOCAL byte uim_iccid[UIM_NUM_DRV_SLOTS][UIM_ICCID_SIZE];

/* Buffer to hold ICCID used in GCF test cases */
LOCAL byte const UIM_TEST_ICCID[UIM_ICCID_SIZE] = {0x98, 0x94, 0x00, 0x20, 0x20, 0x41, 0x00, 0x00, 0x40, 0xF5 };

/* This structure contains info about the CHV1 */
LOCAL uim_chv_struct_type uim_chv1[UIM_NUM_DRV_SLOTS];
LOCAL uim_chv_struct_type uim_chv2[UIM_NUM_DRV_SLOTS];

/* Variable to indicate if a re-selection to the MF has to be done
 * owing to the failure of the last select command
 */
boolean uim_reselect_mf = FALSE;

#ifdef FEATURE_UIM_UICC
/* Flag to indicate if the instruction class needs to be changed */
boolean uim_toggle_instrn_class = FALSE;

/* Instruction toggle counter */
byte uim_instrn_toggle_count = 1;

/* Flag to indicate if the file selection method is selection by path ref */
LOCAL boolean uim_sel_by_path_ref_from_mf = FALSE;
#endif /* FEATURE_UIM_UICC */

/* Byte to hold the file characteristics byte */
LOCAL byte file_char_byte;

/* Type to store the protocols supported by the card */
typedef struct {
  boolean t_0;  /* T = 0 support */
#if defined( FEATURE_UIM_T_1_SUPPORT )
  boolean t_1;  /* T = 1 support */
#endif /* FEATURE_UIM_T_1_SUPPORT */
} uim_prot_sup_by_card_type;

/* Protocol supported by the card */
LOCAL uim_prot_sup_by_card_type uim_prot_sup_by_card;

#ifdef FEATURE_UIM_SUPPORT_INVERSE_CONVENTION
/* Number of times the convention (inverse/direct) is changed */
byte uim_convention_change_count = 1;
#endif

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
/* Slot where terminal profile is downloaded to */
LOCAL uim_slot_type uim_toolkit_slot;
#endif

#ifdef FEATURE_UIM_SUPPORT_NO_ICCID
LOCAL boolean uim_iccid_select_failed = FALSE;
#endif

/* Status Word 1 used for Get Resp */
uim_sw1_type uim_get_resp_sw1;

/* Status word 2 used for Get Resp */
uim_sw2_type uim_get_resp_sw2;

/* Holds the current channel being used */
uim_channel_type uim_cur_channel = UIM_DEFAULT_CHANNEL;

/*===========================================================================
              F U N C T I O N   P R O T O T Y P E S
===========================================================================*/
LOCAL boolean uim_alloc_set_rd_wr_info(word num_bytes_to_alloc);

#ifdef FEATURE_UIM_UICC
LOCAL void uim_set_uicc_pin_param_p2
(
  uim_chv_type chv,
  byte *p2
);

LOCAL void uim_parse_uicc_select_response
(
  byte const *data,
  word num_bytes,
  boolean ef_rsp,
  uim_last_sel_ef_type *last_sel_ef,
  byte *file_char
);

LOCAL void uim_parse_uicc_status_response
(
  byte const *data,
  word num_bytes,
  uim_dir_type *file_id,
  uim_aid_type *aid
);
#endif /* FEATURE_UIM_UICC */

LOCAL void uim_reset_last_sel_ef_data( void );

LOCAL uim_cmd_status_type uim_update_last_sel_dir
(
  uim_rsp_buf_type const *rsp_ptr,
#ifdef FEATURE_UIM_UICC
  uim_cmd_type     const *cmd,
#endif /* FEATURE_UIM_UICC */
  word             path
);

LOCAL void uim_free_reset_rd_wr_info( void );

/*===========================================================================*/

/* Releases the memory if allocated to alloc_mem member and resets all
 * other variables holding additional info about read/write
 */
LOCAL void uim_free_reset_rd_wr_info( void )
{
  if ( uim_rd_wr_info.alloc_mem != NULL )
  {
    mem_free(&tmc_heap, uim_rd_wr_info.alloc_mem);
    uim_rd_wr_info.alloc_mem = NULL;
  }

  uim_rd_wr_info.data_ptr      = NULL;
  uim_rd_wr_info.num_bytes_rsp = 0;
  uim_rd_wr_info.num_bytes     = 0;
  uim_rd_wr_info.offset        = 0;
}

/* Resets all the static variables holding additional read/write information,
 * allocates memory, assigns the allocated memory to alloc_mem and
 * data_ptr
 */
LOCAL boolean uim_alloc_set_rd_wr_info(word num_bytes_to_alloc)
{
  uim_free_reset_rd_wr_info();

  if (num_bytes_to_alloc == 0 )
  {
    return FALSE;
  }

  uim_rd_wr_info.alloc_mem =
    mem_malloc(&tmc_heap, num_bytes_to_alloc);

  uim_rd_wr_info.data_ptr =
    uim_rd_wr_info.alloc_mem;

  uim_rd_wr_info.num_bytes =
    num_bytes_to_alloc;

  return ( uim_rd_wr_info.alloc_mem != NULL );
}

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_START_POWER_SAVE

DESCRIPTION
  For USB cards this function informs the USB driver that the USB interface is
  not currently needed for any ICCD transaction and hence it can be suspended.
  For legacy ISO cards this function processes a request to turn the UIM clock
  off. It uses a global control variable to determine how to control the clock.

DEPENDENCIES
  For legacy ISO cards this function uses a global variable to determine
  how to control the UIM clock.

RETURN VALUE
  TRUE/FALSE.

SIDE EFFECTS
  For USB cards if all functional interfaces over the USB interface have voted
  for a suspend, the USB interface to the card will be suspended.
  For legacy ISO cards the UIM clock will either be left high or low or will be
   left on, depending on the preferences of the UIM card.

===========================================================================*/
boolean uim_start_power_save
(
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  uim_slot_type slot
#else
  void
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
)
{
  uim_drv_slot_type drv_slot;
  dword guardtime_wait_value;

  /* Keep a local buffer for controlling the clock. */
  uim_op_params_type local_uim_op_params_buf;

  /* Return status */
  boolean status = TRUE;

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  if (slot == UIM_SLOT_1)
  {
    drv_slot = UIM_DRV_SLOT1;
  }
  else
  {
    drv_slot = UIM_DRV_SLOT2;
  }
#else
#ifdef FEATURE_UIM1
  drv_slot = UIM_DRV_SLOT1;
#else
  drv_slot = UIM_DRV_SLOT2;
#endif /* FEATURE_UIM1 */
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

  /* First, wait for the guardtime to pass.  This is 2 etus */
  if((uim_op_params_ptr->FI < UIM_CRCF_SIZE) &&
     (uim_op_params_ptr->DI < UIM_BRAF_SIZE))
  {
    guardtime_wait_value = ((crcf_values[uim_op_params_ptr->FI]*1000000)/
                          (uim_clk_freq[drv_slot] *
                           braf_values[uim_op_params_ptr->DI]));
  }
  else
  {
    MSG_ERROR("Attempt to access array out of bounds",0,0,0);
    return FALSE;
  }
#ifndef TEST_FRAMEWORK
  uim_clk_busy_wait (guardtime_wait_value);
  uim_clk_busy_wait (guardtime_wait_value);
  /* Make sure to wait long enough after the last transaction to
  turn off the clock. */
  uim_clk_busy_wait ((1860*1000000)/uim_clk_freq[drv_slot]);
#endif  /*!TEST_FRAMEWORK*/

  if((uim_mode == UIM_PASSIVE_MODE) &&
     (uim_passive_clk_stop_allowed == FALSE))
  {
    uim_clock_stop_cntrl[drv_slot] = UIM_CLOCK_STOP_NOT_ALLOWED;
  }

  /* Determine what to do about stopping the UIM clock. */
  switch (uim_clock_stop_cntrl[drv_slot])
  {
    case UIM_CLOCK_STOP_HIGH:
      {
        /* Stop the clock at a high state */
        local_uim_op_params_buf.change_baud_rate = FALSE;
        local_uim_op_params_buf.change_clk_freq = TRUE;
        local_uim_op_params_buf.change_guardtime = FALSE;
        /* Get the last clock frequency used for the UIM. */
        local_uim_op_params_buf.clock_frequency = UIM_CLK_STOP_HIGH;
        /* Program the clock frequency */
        uim_update_op_params( &local_uim_op_params_buf, drv_slot );
      } /* end case - stop the clock at a high state */
      break;

    case UIM_CLOCK_STOP_LOW:
      {
        /* Stop the clock at a low state */
        local_uim_op_params_buf.change_baud_rate = FALSE;
        local_uim_op_params_buf.change_clk_freq = TRUE;
        local_uim_op_params_buf.change_guardtime = FALSE;
        /* Get the last clock frequency used for the UIM. */
        local_uim_op_params_buf.clock_frequency = UIM_CLK_STOP_LOW;
        /* Program the clock frequency */
        uim_update_op_params( &local_uim_op_params_buf, drv_slot );
      } /* end case - stop the clock at a low state */
      break;

    default:
      {
        /* Do not stop the clock as the card has given a preference to keep it
           on or the control variable is out of range. */
        status = FALSE;
      } /* end case - do not stop the clock */
      break;

  } /* end switch - check clock control */

  return ( status );
} /* uim_start_power_save */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_STOP_POWER_SAVE

DESCRIPTION
  For USB cards this function sends a request to the USB driver to resume
  the USB interface as we need to communicate with the card over the ICCD
  functional interface.
  For legacy ISO cards this function processes a request to turn the UIM clock
  back on. It uses a global control variable to determine how to control the
  clock. It also uses a global variable to determine what frequency to set
  the clock.

DEPENDENCIES
  For legacy ISO cards this function uses a global variable to determine how
  to control the UIM clock and another global variable to determine the
  clock frequency.

RETURN VALUE
  None

SIDE EFFECTS
  For USB cards the USB driver will be requested to resume the interface.
  For ISO cards the UIM clock will be turned on and set to a frequency
  previously determined.

===========================================================================*/
void uim_stop_power_save
(
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  uim_slot_type slot
#else
  void
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
)
{
  uim_drv_slot_type drv_slot;

  /* Keep a local buffer for controlling the clock. */
  uim_op_params_type local_uim_op_params_buf;

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  if (slot == UIM_SLOT_1)
  {
    drv_slot = UIM_DRV_SLOT1;
  }
  else
  {
    drv_slot = UIM_DRV_SLOT2;
  }
#else
#ifdef FEATURE_UIM1
  drv_slot = UIM_DRV_SLOT1;
#else
  drv_slot = UIM_DRV_SLOT2;
#endif /* FEATURE_UIM1 */
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

  /* Determine what to do about stopping the UIM clock. */
  switch (uim_clock_stop_cntrl[drv_slot])
  {
    case UIM_CLOCK_STOP_HIGH:
    case UIM_CLOCK_STOP_LOW:
      {
        /* Turn the clock back on */
        local_uim_op_params_buf.change_baud_rate = FALSE;
        local_uim_op_params_buf.change_clk_freq = TRUE;
        local_uim_op_params_buf.change_guardtime = FALSE;
        /* Get the last clock frequency used for the UIM. */
        local_uim_op_params_buf.clock_frequency =
          uim_op_params_ptr->clock_frequency;
        /* Program the clock frequency */
        uim_update_op_params( &local_uim_op_params_buf, drv_slot );

#ifndef TEST_FRAMEWORK
        /* Make sure to wait long enough after turning the clock back on
           before processing the next transaction. */
        uim_clk_busy_wait ((700*1000000)/uim_clk_freq[drv_slot]);
#endif /*!TEST_FRAMEWORK*/

      } /* end case - stop the clock at a low state */
      break;

    default:
      {
        /* Do not turn the clock back on as the clock was not turned off
           from before. */
      } /* end case - do not stop the clock */
      break;

  } /* end switch - check clock control */
} /* uim_stop_power_save */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_FORMAT_CHV

DESCRIPTION
  This procedure formats the chv recd from NV in the commands NEW_CHV and
  UNBLOCK_CHV.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes the value of the CHV passed in as a parameter.

===========================================================================*/

void uim_format_chv
(
  byte *chv   /* This points to the CHV buffer that holds the CHV digits.
                 The digits in this buffer are formatted  */
)
{
  /* Index for processing the CHV buffer */
  byte i = 0;
  /* This holds the index of the last CHV digit */
  byte j = UIM_MAX_CHV_DIGITS;

  if(chv == NULL)
  {
    MSG_ERROR("uim_format_chv chv is NULL",0,0,0);
    return;
  }

  /* Look for the end of the CHV digits. */
  for (i = 0; i < UIM_MAX_CHV_DIGITS; i++)
  {
     if (chv[i] == '\0')
     {
       /* This is past the last byte of the CHV digits. */
       /* Start adding pad characters after the CHV digits. */
       j = i;
       break;
     }
  }

  /* Fill the buffer with pad characters */
  for (i = j; i < UIM_MAX_CHV_DIGITS; i++)
      chv[i] = UIM_CHV_PAD_CHAR;

} /* uim_format_chv */

#ifdef FEATURE_UIM_UICC
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SET_UICC_PIN_PARAM_P2

DESCRIPTION
  This procedure sets the parameter p2 in the apdu header based on the USIM
  PIN.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if a valid PIN type is recd. FALSE otherwise.

SIDE EFFECTS
  Fills the p2 in the apdu header.
===========================================================================*/

LOCAL void uim_set_uicc_pin_param_p2
(
  uim_chv_type chv,
  byte *p2
)
{
  if (p2 == NULL)
  {
    MSG_ERROR("uim_set_uicc_pin_param_p2 p2 is null",0,0,0);
    return;
  }
  switch (chv)
  {
    case UIM_PIN1_APP1:
       *p2 = UIM_PIN1_APP1_VALUE;
    break;

    case UIM_PIN1_APP2:
       *p2 = UIM_PIN1_APP2_VALUE;
    break;

    case UIM_PIN1_APP3:
       *p2 = UIM_PIN1_APP3_VALUE;
    break;

    case UIM_PIN1_APP4:
       *p2 = UIM_PIN1_APP4_VALUE;
    break;

    case UIM_PIN1_APP5:
       *p2 = UIM_PIN1_APP5_VALUE;
    break;

    case UIM_PIN1_APP6:
       *p2 = UIM_PIN1_APP6_VALUE;
    break;

    case UIM_PIN1_APP7:
       *p2 = UIM_PIN1_APP7_VALUE;
    break;

    case UIM_PIN1_APP8:
       *p2 = UIM_PIN1_APP8_VALUE;
    break;

    case UIM_PIN2_APP1:
       *p2 = UIM_PIN2_APP1_VALUE;
    break;

    case UIM_PIN2_APP2:
       *p2 = UIM_PIN2_APP2_VALUE;
    break;

    case UIM_PIN2_APP3:
       *p2 = UIM_PIN2_APP3_VALUE;
    break;

    case UIM_PIN2_APP4:
       *p2 = UIM_PIN2_APP4_VALUE;
    break;

    case UIM_PIN2_APP5:
       *p2 = UIM_PIN2_APP5_VALUE;
    break;

    case UIM_PIN2_APP6:
       *p2 = UIM_PIN2_APP6_VALUE;
    break;

    case UIM_PIN2_APP7:
       *p2 = UIM_PIN2_APP7_VALUE;
    break;

    case UIM_PIN2_APP8:
       *p2 = UIM_PIN2_APP8_VALUE;
    break;

    case UIM_UNIVERSAL_PIN:
       *p2 = UIM_UNIVERSAL_PIN_VALUE;
    break;

    default:
       *p2 = 0;
       MSG_ERROR ("Incorrect chv_type %d",chv,0,0);
    break;
  }
} /* uim_set_uicc_pin_param_p2 */
#endif /* FEATURE_UIM_UICC */


/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SEND_GENERIC_ERR_RPT

DESCRIPTION
  This function sends an error report for a generic command that is being
  requested.

DEPENDENCIES
  This function queues an error report onto a client queue.

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/
void uim_send_generic_err_rpt
(
  uim_cmd_type *cmd_ptr                    /* the command to queue up */
)
{
  q_type *save_done_q_ptr;
  q_link_type *save_link_ptr;

  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_send_generic_err_rpt cmd_ptr is NULL",0,0,0);
  }
  save_done_q_ptr = cmd_ptr->hdr.cmd_hdr.done_q_ptr;
  save_link_ptr   = &cmd_ptr->hdr.cmd_hdr.link;

  /* Copy user data from received command to report buffer */
  uim_rpt_buf.user_data = cmd_ptr->hdr.user_data;

  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Report back the status as UIM_FAIL indicating failure in the R-UIM */
  uim_rpt_buf.rpt_status = UIM_FAIL;

  /* Check if the client task has supplied a report back function. */
  if (cmd_ptr->hdr.rpt_function != NULL)
  {
    /* Process the error report based on the command that failed. */
    switch(cmd_ptr->hdr.command)
    {
      case UIM_READ_REC_CMD_F:
      case UIM_READ_BIN_CMD_F:
        {
          if (cmd_ptr->hdr.command == UIM_READ_REC_CMD_F)
          {
            uim_rpt_buf.rpt_type = UIM_READ_REC_CMD_R;
          }
          else
          {
            uim_rpt_buf.rpt_type = UIM_READ_BIN_CMD_R;
          }

          uim_rpt_buf.rpt.read.num_bytes_read =
            uim_rd_wr_info.num_bytes_rsp;

          uim_rpt_buf.rpt.read.data =
            uim_rd_wr_info.alloc_mem;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for UIM Read", 0, 0, 0 );

          uim_free_reset_rd_wr_info();
        }
        break;

      case UIM_WRITE_BIN_CMD_F:
      case UIM_WRITE_REC_CMD_F:
        {
          if (cmd_ptr->hdr.command == UIM_WRITE_BIN_CMD_F)
          {
            uim_rpt_buf.rpt_type = UIM_WRITE_BIN_CMD_R;
          }
          else
          {
            uim_rpt_buf.rpt_type = UIM_WRITE_REC_CMD_R;
          }

          uim_rpt_buf.rpt.write.num_bytes_written =
            uim_rd_wr_info.num_bytes_rsp;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for UIM Write", 0, 0, 0 );

          /* Clear all the static variables */
          uim_free_reset_rd_wr_info();
        }
        break;

      case UIM_INC_CMD_F:
        {
          uim_rpt_buf.rpt_type = UIM_INC_CMD_R;

          uim_rpt_buf.rpt.inc.inc_rsp_len =
            uim_rd_wr_info.num_bytes_rsp;

          uim_rpt_buf.rpt.inc.data =
            uim_rd_wr_info.alloc_mem;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for UIM Inc", 0, 0, 0 );

          uim_free_reset_rd_wr_info();
        }
        break;

      case UIM_ACCESS_F:
        {
          uim_rpt_buf.rpt_type = UIM_ACCESS_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for UIM Access", 0, 0, 0 );
        } /* end case - UIM_ACCESS_F */
        break;

      case UIM_UNBLOCK_CHV_F:
        {
          uim_rpt_buf.rpt_type = UIM_UNBLOCK_CHV_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for CHV Unblock", 0, 0, 0 );
        } /* end case - UIM_UNBLOCK_CHV_F */
        break;

      case UIM_STATUS_F:
        {
          uim_rpt_buf.rpt_type = UIM_STATUS_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Status ", 0, 0, 0 );
        } /* end case - UIM_STATUS_F */
        break;

      case UIM_INTERNAL_ME_PUP_F:
      case UIM_INTERNAL_POLL_F:
          /* do nothing */
        break;
      case UIM_INTERNAL_WAKE_UP_F:        /* Power up due to Power management */
        break;


      case UIM_INTERNAL_SELECT_F:
#ifdef FEATURE_UIM_UICC
          /* Reset the flag */
          uim_sel_by_path_ref_from_mf = FALSE;
#endif
        break;

      case UIM_CACHED_SEL_F:
      case UIM_SELECT_F:
        {
#ifdef FEATURE_UIM_UICC
          /* Reset the flag */
          uim_sel_by_path_ref_from_mf = FALSE;
#endif
          if ( cmd_ptr->hdr.command == UIM_SELECT_F)
          {
            uim_rpt_buf.rpt_type = UIM_SELECT_R;
          }
          else
          {
            uim_rpt_buf.rpt_type = UIM_CACHED_SEL_R;
          }

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for SELECT", 0, 0, 0 );
        } /* end case - UIM_SELECT_F */
        break;

      case UIM_INVALIDATE_F:
        {
          uim_rpt_buf.rpt_type = UIM_INVALIDATE_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Invalidate", 0, 0, 0 );
        } /* end case - UIM_INVALIDATE_F */
        break;

      case UIM_REHABILITATE_F:
        {
          uim_rpt_buf.rpt_type = UIM_REHABILITATE_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Rehabilitate", 0, 0, 0 );
        } /* end case - UIM_REHABILITATE_F */
        break;

      case UIM_VERIFY_CHV_F:
        {
          uim_rpt_buf.rpt_type = UIM_VERIFY_CHV_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Verify CHV ", 0, 0, 0 );
        } /* end case - UIM_VERIFY_CHV_F */
        break;

      case UIM_CHANGE_CHV_F:
        {
          uim_rpt_buf.rpt_type = UIM_CHANGE_CHV_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Change CHV ", 0, 0, 0 );
        } /* end case - UIM_CHANGE_CHV_F */
        break;

      case UIM_DISABLE_CHV_F:
        {
          uim_rpt_buf.rpt_type = UIM_DISABLE_CHV_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Disable CHV ", 0, 0, 0 );
        } /* end case - UIM_DISABLE_CHV_F */
        break;

      case UIM_ENABLE_CHV_F:
        {
          uim_rpt_buf.rpt_type = UIM_ENABLE_CHV_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Enable CHV ", 0, 0, 0 );
        } /* end case - UIM_ENABLE_CHV_F */
        break;

      case UIM_SEEK_F:
        {
          uim_rpt_buf.rpt_type = UIM_SEEK_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Seek access ", 0, 0, 0 );
        } /* end case - UIM_SEEK_F */
        break;

#ifdef FEATURE_UIM_TOOLKIT

    case UIM_TERMINAL_PROFILE_F:        /* Send a terminal profile */
      {
        uim_rpt_buf.rpt_type = UIM_TERMINAL_PROFILE_R;

        /* Fill in necessary fields of command block */
        uim_rpt_buf.rpt_hdr.task_ptr = NULL;

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
          /* Use the slot for toolkit as the polling slot */
        uim_toolkit_slot = UIM_SLOT_NONE;
        uim_polling_slot = UIM_SLOT_NONE;
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

        /* Report to requesting task */
        (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
        MSG_MED( "Reporting error for Terminal Profile", 0, 0, 0 );
      } /* end case - UIM_TERMINAL_PROFILE_F */
      break;

    case UIM_ENVELOPE_F:                /* Send an envelope command */
      {
        uim_rpt_buf.rpt_type = UIM_ENVELOPE_R;

        /* Fill in necessary fields of command block */
        uim_rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Report to requesting task */
        (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
        MSG_MED( "Reporting error for Envelope", 0, 0, 0 );
      } /* end case - UIM_ENVELOPE_F */
      break;

    case UIM_INTERNAL_FETCH_F:          /* Fetch a proactive command */
      {
        /* If there is an error with fetching a proactive command, it
           indicates there is a bug in the code or in the card.  Do nothing */

      } /* end case - UIM_INTERNAL_FETCH_F */
      break;

    case UIM_TERMINAL_RESPONSE_F:       /* Send a proactive cmd response */
      {
        uim_rpt_buf.rpt_type = UIM_TERMINAL_RESPONSE_R;

        /* Fill in necessary fields of command block */
        uim_rpt_buf.rpt_hdr.task_ptr = NULL;

        /* Report to requesting task */
        (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
        MSG_MED( "Reporting error for Terminal Response", 0, 0, 0 );
      } /* end case - UIM_TERMINAL_RESPONSE_F */
      break;

      case UIM_POLLING_INTERVAL_F:        /* Polling interval or polling off */
        {
          uim_rpt_buf.rpt_type = UIM_POLLING_INTERVAL_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for TK polling", 0, 0, 0 );
        } /* end case - UIM_POLLING_INTERVAL_F */
        break;

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS

      case UIM_RESET_WITH_TP_F:         /* Reset the card and optionally
                                           send a terminal profile. */
        {
          uim_rpt_buf.rpt_type = UIM_RESET_WITH_TP_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Use the slot for toolkit as the polling slot */
          uim_toolkit_slot = UIM_SLOT_NONE;
          uim_polling_slot = UIM_SLOT_NONE;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Reset with Term Prof", 0, 0, 0 );
        } /* end case - UIM_RESET_WITH_TP_F */
        break;

#endif /* FEATURE_UIM_DUAL_SLOTS */
#endif /* FEATURE_UIM_TOOLKIT */

      case UIM_RESET_F:                 /* Perform a warm reset and perform
                                           power up */
        {
          uim_rpt_buf.rpt_type = UIM_RESET_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Reset", 0, 0, 0 );
        } /* end case - UIM_RESET_F */
        break;

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS

      case UIM_PREF_SLOT_CHANGE_F:
        {
          uim_rpt_buf.rpt_type = UIM_PREF_SLOT_CHANGE_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Pref Slot change", 0, 0, 0 );
        }
      break;

#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

      case UIM_STREAM_APDU_F:            /* Stream APDU to the card */
        {
          uim_rpt_buf.rpt_type = UIM_STREAM_APDU_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Stream APDU", 0, 0, 0 );
        } /* end case - UIM_STREAM_APDU_F */
        break;


      case UIM_STREAM_ISO7816_APDU_F: /* Stream an ISO7816 APDU to the card */
        {
          uim_rpt_buf.rpt_type = UIM_STREAM_ISO7816_APDU_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;
          uim_rpt_buf.rpt.stream_iso7816_apdu.num_data_bytes = 0;
          uim_rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw1 = (uim_sw1_type) 0;
          uim_rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw2 = (uim_sw2_type) 0;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Ext Stream APDU", 0, 0, 0 );
        } /* end case - UIM_STREAM_ISO7816_APDU_F */
        break;
      case UIM_POWER_DOWN_F:
        {
          uim_rpt_buf.rpt_type = UIM_POWER_DOWN_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Power Down", 0, 0, 0 );
        }
        break;

        case UIM_RESET_SWITCH_UIM_PASSIVE_F:
            {
          uim_rpt_buf.rpt_type = UIM_RESET_SWITCH_UIM_PASSIVE_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting error for Reset Switch UIM Passive", 0, 0, 0 );
        }
        break;

      default:
        {
          /* Send this request to the custom generic error processing since
             this module does not handle this command */
          uim_custom_send_generic_err_rpt (cmd_ptr);
        } /* end case - default */
        break;

    }  /* switch */
  } /* end if - the client has supplied a report back function. */

  /* return buffer to free queue */
  if (save_done_q_ptr != NULL)
  {
      /* place command on requested queue */
      q_put( save_done_q_ptr, save_link_ptr );
  }

} /* uim_send_generic_err_rpt */

#ifdef FEATURE_UIM_UICC
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PARSE_UICC_SELECT_RESPONSE

DESCRIPTION
  This procedure parses the UICC select response for an EF and fills the
  required fields of last selected EF structure if required and fils the
  file characteristic byte.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Fills the file characteristic byte and elements of the last selected EF
  structure when the file selected is an EF that is under an ADF.
===========================================================================*/

LOCAL void uim_parse_uicc_select_response
(
  byte const *data,
  word num_bytes,
  boolean ef_rsp,
  uim_last_sel_ef_type *last_sel_ef,
  byte *file_char
)
{
  uint16 i = 0; /* index variable */
  uint16 j = 0; /* Temp storage for length */

  if (last_sel_ef == NULL)
  {
    ERR_FATAL("uim_parse_uicc_select_response last_sel_ef is NULL",0,0,0);
  }

  /* Process all the bytes in the receive FIFO */
  while (i < num_bytes)
  {
     if (data == NULL)
     {
       MSG_ERROR("uim_parse_uicc_select_response data is NULL",0,0,0);
       return;
     }
     switch (data[i++])
     {
        case UIM_FCP_TEMPLATE_TAG:
            i++;  /* Move past the length field */

        break;

        case UIM_FILE_DESCRIPTOR_TAG:
            /* If EF response is being parsed */
            if (ef_rsp)
            {
              /* Move past the length field */
              i++;

              /* Parse the file descriptor byte */
              switch (data[i++] & UIM_UICC_EF_STRUCT_MASK)
              {
                case UIM_UICC_TRANSPARENT_EF:
                  last_sel_ef->file_struct = UIM_TRANSPARENT;

                  /* Move past the data coding byte */
                  i+=1;

                  break;

                case UIM_UICC_LINEAR_EF:
                  last_sel_ef->file_struct = UIM_LINEAR_FIXED;

                  /* Move past the data coding byte */
                  i+=1;

                  /* Get the record length */
                  last_sel_ef->rec_len = (word)FLIPW(data+i);

                  /* Increment the counter by 3 to go past the number of recs */
                  i+=3;
                  break;

                case UIM_UICC_CYCLIC_EF:
                  last_sel_ef->file_struct = UIM_CYCLIC;

                  /* Move past the data coding byte */
                  i+=1;

                  /* Get the record length */
                  last_sel_ef->rec_len = (word)FLIPW(data+i);

                  /* Increment the counter by 3 to go past the number of recs */
                  i+=3;
                  break;

                default:
                  MSG_ERROR("Incorrect file type",0,0,0);
                  break;
              }
            }
            else
            {
              /* Get the length */
              j = data[i++];

              /* Skip j bytes */
              i+=j;
            }

        break;

        case UIM_FILE_ID_TAG:
            /* If EF response is being parsed */
            if (ef_rsp)
            {
              /* Move past the length field */
              i++;

              /* Get the file id */
              last_sel_ef->file_id = (word)FLIPW(data+i);

              /* Move past the file id */
              i+=2;

            }
            else
            {
              /* Get the length */
              j = data[i++];

              /* Skip j bytes */
              i+=j;
            }

        break;

        case UIM_PROPRIETARY_TAG:

            /* Get the file characteristics byte */
            if(file_char != NULL)
            {
              *file_char = data[i+3];
            }
            else
            {
              MSG_ERROR("uim_parse_uicc_select_response data is NULL",0,0,0);
              return;
            }


            /* Get the length */
            j = data[i++];

            /* Skip j bytes */
            i+=j;

        break;

        case UIM_FILE_SIZE_TAG:

            /* If EF response is being parsed */
            if (ef_rsp)
            {

              /* Move past the length field */
              i++;

              /* Get the file id */
              last_sel_ef->file_size = (word)FLIPW(data+i);

              /* Move past the file id */
              i+=2;
            }
            else
            {
              /* Get the length */
              j = data[i++];

              /* Skip j bytes */
              i+=j;
            }
        break;

        case UIM_DF_NAME_TAG:
        case UIM_SECURITY_ATTR_TAG1:
        case UIM_SECURITY_ATTR_TAG2:
        case UIM_SECURITY_ATTR_TAG3:
        case UIM_SECURITY_ATTR_TAG4:
        case UIM_LIFE_CYCLE_STATUS_TAG:
        case UIM_TOTAL_FILE_SIZE_TAG:
        case UIM_SFI_TAG:
        /* Ignore the tags that are not recognized */
        default:

            /* Get the length */
            j = data[i++];

            /* Skip j bytes */
            i+=j;

        break;
     }
  }
} /* uim_parse_uicc_select_response */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PARSE_STATUS_RESPONSE

DESCRIPTION
  This procedure parses the UICC response to a STATUS command

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Fills the data into the parameters provided.
===========================================================================*/

LOCAL void uim_parse_uicc_status_response
(
  byte const *data,
  word num_bytes,
  uim_dir_type *file_id,
  uim_aid_type *aid
)
{
  byte i = 0; /* index variable */
  byte j = 0; /* Temp storage for length */


  if(file_id == NULL)
  {
    ERR_FATAL("uim_parse_uicc_status_response file_id is NULL",0,0,0);
  }

  if(aid == NULL)
  {
    ERR_FATAL("uim_parse_uicc_status_response aid is NULL",0,0,0);
  }
  /* Process all the bytes in the receive FIFO */
  while (i < num_bytes)
  {
     if (data == NULL)
     {
       MSG_ERROR("uim_parse_uicc_status_response data is NULL",0,0,0);
       return;
     }
     switch (data[i++])
     {
        case UIM_FCP_TEMPLATE_TAG:
          i++;  /* Move past the length field */

        break;

        case UIM_DF_NAME_TAG:
        {
          /* Store the length of the aid data */
          aid->aid_length = data[i++];

          memcpy( aid->data, data+i, aid->aid_length);

          i+= aid->aid_length;

        }
        break;

        case UIM_FILE_ID_TAG:
        {
          /* Move past the length field */
          j = data[i++];

          /* Get the file id */
          *file_id = (word)FLIPW(data+i);

          /* Move past the file id */
          i+=j;

        }
        break;

        /* Ignore the remaining tags */
        default:
        {
          /* Get the length */
          j = data[i++];

          /* Skip j bytes */
          i+=j;
        }
        break;
     } /* switch */
  }  /* while loop */
} /* uim_parse_status_response */

#endif /* FEATURE_UIM_UICC */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_RESET_LAST_SEL_EF_DATA

DESCRIPTION
  This procedure resets the data in the last selected EF structure

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/

LOCAL void uim_reset_last_sel_ef_data( void )
{
   /* Clear the EF particulars */
   uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_id =
                                                           UIM_NO_FILE_SEL;
   uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size = 0;
   uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_struct =
                                                        UIM_NO_FILE_STRUCT;
   uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len = 0;
} /* uim_reset_last_sel_ef_data */

/* <EJECT> */
/*===========================================================================
FUNCTION UIM_UPDATE_LAST_SEL_DIR

DESCRIPTION
  This procedure processes the response to a Select command and updates the
  last selected directory structure.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of uim_cmd_status_type

SIDE EFFECTS
  Changes the UIM last selected directory structure.

===========================================================================*/
LOCAL uim_cmd_status_type uim_update_last_sel_dir
(
  uim_rsp_buf_type const *rsp_ptr,
#ifdef FEATURE_UIM_UICC
  uim_cmd_type     const *cmd,
#endif /* FEATURE_UIM_UICC */
  word             path
)
{
  uim_cmd_status_type status = UIM_CMD_SUCCESS;
#ifdef FEATURE_UIM_UICC
  if(cmd == NULL)
  {
    MSG_ERROR("uim_update_last_sel_dir cmd is NULL",0,0,0);
    return UIM_CMD_ERR;
  }
#endif /* FEATURE_UIM_UICC */
  if (rsp_ptr == NULL)
  {
    ERR_FATAL("uim_update_last_sel_dir rsp_ptr is NULL",0,0,0);
  }


  switch ((path & UIM_FILE_ID_LEVEL_MASK) >> UIM_FILE_ID_LEVEL_SHIFT)
  {
    case UIM_MF_LEVEL:
    {
      /* Clear out the last select file fields. */
#ifdef FEATURE_UIM_UICC
      memset( uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf.data, '0',
              UIM_AID_MAX_NAME_LENGTH);
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf.aid_length = 0;
#endif /* FEATURE_UIM_UICC */
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df = UIM_NO_FILE_SEL;
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type = UIM_NO_SUCH_FILE;
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 = UIM_NO_FILE_SEL;
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef  = UIM_NO_FILE_SEL;
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = FALSE;

      /* Reset the data in the last selected EF structure */
      uim_reset_last_sel_ef_data();
    } /* end case - UIM_MF_LEVEL */
    break;

    case UIM_DF_LEVEL_2:
    {
      /* Save the last selected 2nd level DF */
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 = path;
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef  = UIM_NO_FILE_SEL;
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = FALSE;

      /* Reset the data in the last selected EF structure */
      uim_reset_last_sel_ef_data();

    } /* end case - UIM_DF_LEVEL_2 */
    break;

    case UIM_EF_UNDER_MF:
    case UIM_EF_UNDER_2ND_DF:
    case UIM_EF_UNDER_1ST_DF:
    {
      /* Save the last selected EF type */
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef = path;
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = TRUE;

#ifdef FEATURE_UIM_UICC
      if (cmd->hdr.protocol == UIM_UICC)
      {
        /* call the function to parse the response from the UICC
           select for an EF select */
        uim_parse_uicc_select_response(
            rsp_ptr->rsp.data,
            rsp_ptr->cmd_rsp_size,
            TRUE,
            &uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info,
            &file_char_byte );
      }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
      else
#endif /* FEATURE_UIM_UICC */
      {
        /* Save the EF particulars */
        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_id =
                                        (word)FLIPW (rsp_ptr->rsp.ef.file_id);
        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size =
                                        (word)FLIPW (rsp_ptr->rsp.ef.file_size);
        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_struct =
                                                 rsp_ptr->rsp.ef.file_struct;
        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len =
                                                    rsp_ptr->rsp.ef.rec_len;
      }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

    } /* end case - UIM_EF_UNDER_1_DF */
    break;

    case UIM_DF_LEVEL_1:
    {
#ifdef FEATURE_UIM_UICC
      if (path == UIM_SEL_AID_BY_FILE_ID)
      {
        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type = UIM_ADF;

        if (cmd->hdr.command != UIM_STREAM_ISO7816_APDU_F)
        {
          if (cmd->hdr.command == UIM_INTERNAL_SELECT_F)
          {
            /* Update the dir structure with AID info  */
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf = uim_adf_to_be_sel;
          }
          else
          {
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf = cmd->select.aid;
          }

          /* Save the AID as the application whose session is open */
          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].curr_app =
                                          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf;
        } /*7816 apdu*/
        else
        {
          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf =
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].curr_app;
        }
      }
      else
#endif
      {
        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type = UIM_DF;
        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df = path;
      }

      /* Save the last selected 1st level DF */
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 = UIM_NO_FILE_SEL;
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df3 = UIM_NO_FILE_SEL; /* we will manually update last_sel_dir for DF3 case ? */
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef  = UIM_NO_FILE_SEL;
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = FALSE;

      /* Reset the data in the last selected EF structure */
      uim_reset_last_sel_ef_data();

    } /* end case - UIM_DF_LEVEL_1 */
    break;

#ifdef FEATURE_UIM_UICC
    case UIM_ADF_LEVEL:
    {
      /* Save the last selected 1st level DF as ADF type*/
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type = UIM_ADF;
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = FALSE;
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 = UIM_NO_FILE_SEL;
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df3 = UIM_NO_FILE_SEL; /* we will manually update last_sel_dir for DF3 case */
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef  = UIM_NO_FILE_SEL;

      if (cmd->hdr.command == UIM_INTERNAL_SELECT_F)
      {
         /* Update the dir structure with AID info  */
         uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf = uim_adf_to_be_sel;
      }
      else
      {
         uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf = cmd->select.aid;
      }

      /* Save the AID as the application whose session is open */
      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].curr_app =
                                        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf;

      /* Reset the data in the last selected EF structure */
      uim_reset_last_sel_ef_data();

      /* If we came to this state because of an external select command
         with session termination indication for an ADF */
      if ((cmd->hdr.command == UIM_SELECT_F ||
           cmd->hdr.command == UIM_CACHED_SEL_F) &&
          (cmd->select.session == UIM_TERMINATION ))
      {
        /* Clear out the last selected ADF */
        memset( uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf.data, '0',
                UIM_AID_MAX_NAME_LENGTH);
        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf.aid_length = 0;

        /* Also clear out the last selected app */
        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].curr_app =
                                        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf;
      }
    }
    break;
#endif /* FEATURE_UIM_UICC */

    default:
    {
      /* Return command error since we have gotten a garbage file
         type and we cannot handle this */
      status = UIM_CMD_ERR;
    } /* end case - default */
    break;
  } /* end switch - get the level of file */

  return ( status );

} /* uim_update_last_sel_dir */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_GENERIC_COMMAND

DESCRIPTION
  This procedure processes a command sent to the UIM task. It sets the UIM
  state variable and calls the UIM state machine to process the command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  It sets the RUIM state variable amongst others.

===========================================================================*/

boolean uim_process_generic_command
(
  uim_cmd_type *cmd_ptr  /* Pointer to received command */
)
{

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  uim_slot_type slot;
#endif

#ifdef FEATURE_UIM_SUPPORT_CSIM
  boolean select_reqd = TRUE;
  byte j = 0;
  word path;
#endif /* FEATURE_UIM_SUPPORT_CSIM */

  /* This indicates that the command is processed by the state machine */
  boolean status = TRUE;

  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_process_generic_command cmd_ptr is NULL",0,0,0);
  }

  /* Copy user data from received command to report buffer */
  uim_rpt_buf.user_data = cmd_ptr->hdr.user_data;

  if (cmd_ptr->hdr.protocol == UIM_WCDMA)
  {
    cmd_ptr->hdr.protocol = UIM_UICC;
  }

  /* Process the generic command. */
  switch (cmd_ptr->hdr.command)
  {
    case UIM_INTERNAL_ME_PUP_F:         /* POWER UP UIM due to task start up */
    {
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
      {
        MSG_MED( "Received internal Powerup command ", 0, 0, 0 );
        /* Traverse through the states to power up the UIM from bootup */
        uim_generic_state_ptr = UIM_INTERNAL_PUP_STATES;
      }
      /* Start processing select files from the first in the list. */
      select_state_ctr = 0;
    } /* end case - UIM_INTERNAL_PUP_F */
      break;

    case UIM_INTERNAL_WAKE_UP_F:        /* Power up due to Power management */
    {
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
      {
        MSG_MED( "Received internal Wakeup command ", 0, 0, 0 );
        /* Traverse through the states to power up the UIM from sleep. */
        uim_generic_state_ptr = UIM_INTERNAL_WAKE_UP_STATES;
      }
      /* Start processing select files from the first in the list. */
      select_state_ctr = 0;
    } /* end case - UIM_INTERNAL_WAKE_UP_F */
      break;

    case UIM_INTERNAL_POLL_F:           /* Internal command to Poll */
      {
        MSG_MED( "Received internal Poll command ", 0, 0, 0 );
        /* Build the Internal Poll list of states. */
        uim_generic_states[0] = UIM_STATUS_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;
      } /* end case - UIM_INTERNAL_POLL_F */
      break;

    case UIM_INTERNAL_SELECT_F:         /* Internal Select command */
      {
        MSG_MED( "Received internal Select command ", 0, 0, 0 );

        /* If the first entry is not a valid one, then reject the command */
        if (uim_path_to_be_sel[0] != UIM_NO_FILE_SEL)
        {
          /* Build the Select list of states. */
          uim_generic_states[0] = UIM_SELECT_ST;
          uim_generic_states[1] =  UIM_DONE_ST;

           uim_generic_state_ptr = uim_generic_states;
          /* Start processing select files from the first in the list. */
          select_state_ctr = 0;
        }
        else
        {
          status = FALSE;
        }
      } /* end case - UIM_INTERNAL_SELECT_F */
      break;

    case UIM_READ_BIN_CMD_F:
      {
        word num_bytes_to_alloc = 0;

        MSG_MED( "Received UIM READ command ", 0, 0, 0 );

        /* Determine the structure of the file */
        if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_struct
                != UIM_TRANSPARENT)
        {
          MSG_ERROR("UIM_READ_BIN received on a non transparent EF",
                    0,0,0);
          status = FALSE;
          break;
        }

        /* This is a read of a transparent file */
        uim_generic_states[0] = UIM_READ_BIN_MEM_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;

        /* Initialize the offset */
        if (cmd_ptr->read_bin.offset >=
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size )
        {
          cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
          status = FALSE;
          MSG_ERROR("Offset too high file length is is %x",
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size,0,0);
          break;
        }

        /* Start with the supplied length */
        num_bytes_to_alloc = cmd_ptr->read_bin.len;

        /* If the number of bytes to be read is 0 change it to the
         * the length of file...For this command, 0 means rest of the file
         */
        if (num_bytes_to_alloc == 0)
        {
          /* offset >= file_size is ruled out above */
          num_bytes_to_alloc =
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size -
              cmd_ptr->read_bin.offset;
        }

        if ( (cmd_ptr->read_bin.offset + num_bytes_to_alloc) >
                 uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size)
        {
          MSG_ERROR("READ_BIN Truncating len", cmd_ptr->read_bin.len,
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size,0);
          num_bytes_to_alloc =
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size
          - cmd_ptr->read_bin.offset;
        }

        /* Try to allocate mem and reset all the variables */
        if ( !uim_alloc_set_rd_wr_info(num_bytes_to_alloc) )
        {
          cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
          status = FALSE;
          MSG_ERROR("Unable to allocate memory for data %d",
                    uim_rd_wr_info.num_bytes,0,0);
          break;
        }

        uim_rd_wr_info.offset  = cmd_ptr->read_bin.offset;
      }
      break;

    case UIM_READ_REC_CMD_F:
      {
        if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_struct
                == UIM_TRANSPARENT)
        {
          MSG_ERROR("UIM_READ_REC received on a transparent EF",
                    0,0,0);
          status = FALSE;
          break;
        }

        if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len != 0)
        {
          /* This is a read of a record file */
          uim_generic_states[0] = UIM_READ_REC_MEM_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;

          /* We will always return the entire record length */
          if ( !uim_alloc_set_rd_wr_info(uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len) )
          {
            cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
            status = FALSE;
            MSG_ERROR("Unable to allocate memory for data %d",
                      uim_rd_wr_info.num_bytes,0,0);
            break;
          }
        }
        else
        {
          status = FALSE;
          MSG_ERROR("Rec_len is 0 and ef is %x",
                                   uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef,0,0);
        }
      }
      break;

    case UIM_WRITE_BIN_CMD_F:
      {
        MSG_MED( "Received UIM WRITE command ", 0, 0, 0 );

        /* Determine the structure of the file */
        if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_struct
                != UIM_TRANSPARENT)
        {
          MSG_ERROR("UIM_WRITE_BIN received on a non transparent EF",
                    0,0,0);
          status = FALSE;
          break;
        }

        /* This is an update of a transparent file. */
        uim_generic_states[0] = UIM_UPDATE_BIN_MEM_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;
        /* Initialize the offset */
        if (cmd_ptr->write_bin.offset <
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size )
        {
          uim_rd_wr_info.offset  = cmd_ptr->write_bin.offset;
        }
        else
        {
          cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
          status = FALSE;
          MSG_ERROR("Offset too high file length is is %x",
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size,0,0);
          break;
        }

        /* Initialize the data pointer to read from to write to UIM */
        uim_rd_wr_info.data_ptr = cmd_ptr->write_bin.data;

        /* If the number of bytes to be read is 0, do not continue */
        if (cmd_ptr->write_bin.len  == 0 ||
            cmd_ptr->write_bin.data == NULL)
        {
          cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
          status = FALSE;
          MSG_ERROR("Cannot Write 0 bytes",0,0,0);
          break;
        }
        else if ( (cmd_ptr->write_bin.len + uim_rd_wr_info.offset) <=
                 uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size)
        {
          /* offset being equal to file length has
           * already been ruled out above -- so len cannot be 0
           */
          uim_rd_wr_info.num_bytes = cmd_ptr->write_bin.len;
        }
        else
        {
          MSG_ERROR("WRITE_BIN (offset + len) exceeds file length",
            cmd_ptr->read_bin.len,
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size,0);
          status = FALSE;
          MSG_ERROR("Offset too high file length is is %x",
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size,0,0);
          break;
        }
      }
      break;

    case UIM_WRITE_REC_CMD_F:
      {
        if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_struct
                == UIM_TRANSPARENT)
        {
          MSG_ERROR("UIM_READ_REC received on a transparent EF",
                    0,0,0);
          status = FALSE;
          break;
        }

        if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len != 0)
        {
          /* This is an update of a record file. */
          uim_generic_states[0] = UIM_UPDATE_REC_MEM_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;

          uim_rd_wr_info.data_ptr      = cmd_ptr->write_rec.data;
          uim_rd_wr_info.num_bytes_rsp = 0;

          if (cmd_ptr->write_rec.len !=
              uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len)
          {
            MSG_ERROR("WRITE_REC mismatch record length ",
                      cmd_ptr->write_bin.len,
                      uim_rd_wr_info.num_bytes,0);
            status = FALSE;
            break;
          }
          else
          {
            uim_rd_wr_info.num_bytes =
              cmd_ptr->write_rec.len;
          }
        }
        else
        {
          status = FALSE;
          MSG_ERROR("Rec_len is 0 and ef is %x",
                                   uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef,0,0);
        }
      }
      break;

    case UIM_INC_CMD_F:
      {
        /* number of bytes expected for this inc command */
        word temp_num_expected_bytes;

        MSG_MED( "Received UIM INC command ", 0, 0, 0 );

        if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_struct
                == UIM_TRANSPARENT)
        {
          MSG_ERROR("UIM_INC received on a transparent EF",
                    0,0,0);
          status = FALSE;
          break;
        }

        /* This is an increment command. */
        uim_generic_states[0] = UIM_INC_MEM_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;

        uim_rd_wr_info.num_bytes = cmd_ptr->inc.len;
        uim_rd_wr_info.data_ptr  = cmd_ptr->inc.data;
        uim_rd_wr_info.num_bytes_rsp = 0;

#ifdef FEATURE_UIM_UICC
        if ( cmd_ptr->hdr.protocol == UIM_UICC)
        {
          temp_num_expected_bytes = uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len;
        }
        else
        {
          /* p3 should always be 3 for GSM */
          temp_num_expected_bytes = 3;
        }
#else
        /* p3 should always be 3 for GSM */
        temp_num_expected_bytes = 3;
#endif
        if ( uim_rd_wr_info.num_bytes !=
             temp_num_expected_bytes)
        {
          MSG_ERROR("INC_REC length mismatch",
                    cmd_ptr->inc.len,
                    temp_num_expected_bytes,0);
          status = FALSE;
          break;
        }
      }
      break;

    case UIM_ACCESS_F:                  /* Read or Write from UIM */
      {
        MSG_MED( "Received UIM Access command ", 0, 0, 0 );

        /* Determine what type of access this is */
        if (cmd_ptr->access_uim.access == UIM_READ_F)
        {
          /* Determine the structure of the file */
          if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_struct
                  == UIM_TRANSPARENT)
          {
            /* This is a read of a transparent file */
            uim_generic_states[0] = UIM_READ_BINARY_ST;
            uim_generic_states[1] = UIM_DONE_ST;
            uim_generic_state_ptr = uim_generic_states;
          }
          else
          {

            if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len != 0)
            {
              /* This is a read of a record file */
              uim_generic_states[0] = UIM_READ_RECORD_ST;
              uim_generic_states[1] = UIM_DONE_ST;
              uim_generic_state_ptr = uim_generic_states;

              /* pick the length of the record from the response */
              cmd_ptr->access_uim.num_bytes =
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len;

              /* Determine the number of records in the EF */
              cmd_ptr->access_uim.num_records_rsp =
                (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size /
                 uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len );
            }
            else
            {
              status = FALSE;
              MSG_ERROR("Rec_len is 0 and ef is %x",
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len,0,0);
            }
          } /* end if - determine the file structure */
        }
        else if (cmd_ptr->access_uim.access == UIM_WRITE_F)
        {
          /* Determine the structure of the file */
          if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_struct
                  == UIM_TRANSPARENT)
          {
            /* This is an update of a transparent file. */
            uim_generic_states[0] = UIM_UPDATE_BINARY_ST;
            uim_generic_states[1] = UIM_DONE_ST;
            uim_generic_state_ptr = uim_generic_states;
          }
          else
          {
            if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len != 0)
            {
              /* This is an update of a record file. */
              uim_generic_states[0] = UIM_UPDATE_RECORD_ST;
              uim_generic_states[1] = UIM_DONE_ST;
              uim_generic_state_ptr = uim_generic_states;

              /* pick the length of the record from the response */
              cmd_ptr->access_uim.num_bytes =
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len;

              /* Determine the number of records in the EF */
              cmd_ptr->access_uim.num_records_rsp =
                (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size /
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len );
            }
            else
            {
              status = FALSE;
              MSG_ERROR("Rec_len is 0 and ef is %x",
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len,0,0);
            }
          } /* end if - determine the file structure */
        }
        else if (cmd_ptr->access_uim.access == UIM_INC_F)
        {
          /* This is an increment command. */
          uim_generic_states[0] = UIM_INCREMENT_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
      } /* end case - UIM_ACCESS_F */
      break;

    case UIM_CACHED_SEL_F:
    case UIM_SELECT_F:           /* Select a file/directory from the UIM */
      {
        if (cmd_ptr->hdr.command == UIM_SELECT_F)
        {
          MSG_MED( "Received UIM SELECT command ", 0, 0, 0 );
        }
        else
        {
          MSG_MED( "Received UIM_CACHED_SEL_F command ", 0, 0, 0 );
        }

        /* Check to see if we are trying to terminate a non
           current app */
#ifdef FEATURE_UIM_UICC
        if ((cmd_ptr->select.session == UIM_TERMINATION) &&
             (memcmp(cmd_ptr->select.aid.data,
                   uim_last_sel_dir[uim_cur_channel][uim_drv_slot].curr_app.data,
                   cmd_ptr->select.aid.aid_length) != 0))
        {
          status = FALSE;
        }
        else
#endif /* #ifdef FEATURE_UIM_UICC */
        {
          /* Build the Select list of states. */
          uim_generic_states[0] = UIM_SELECT_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;

          /* Start processing select files from the first in the list. */
          select_state_ctr = 0;
        }
      } /* end case - UIM_SELECT_F */
      break;

    case UIM_SEEK_F:                    /* Seek from the UIM */
      {
        MSG_MED( "Received UIM Seek command ", 0, 0, 0 );

        /* Build the Seek list of states. */
        uim_generic_states[0] = UIM_SEEK_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;
      } /* end case - UIM_SEEK_F */
      break;

    case UIM_INVALIDATE_F:              /* Invalidate an EF */
      {
        MSG_MED( "Received UIM Invalidate command ", 0, 0, 0 );

        /* Build the Invalidate list of states. */
        uim_generic_states[0] = UIM_INVALIDATE_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;
      } /* end case - UIM_INVALIDATE_F */
      break;

    case UIM_REHABILITATE_F:            /* Rehabilitate an EF */
      {
        MSG_MED( "Received UIM Rehabilitate command ", 0, 0, 0 );

        /* Build the Invalidate list of states. */
        uim_generic_states[0] = UIM_REHABILITATE_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;
      } /* end case - UIM_REHABILITATE_F */
      break;

    case UIM_VERIFY_CHV_F:              /* Verify CHV */
      {
        MSG_MED( "Received Verify CHV command ", 0, 0, 0 );

#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        /* Determine which CHV is to be verified. */
        if ((cmd_ptr->verify_chv.chv == UIM_CHV1) &&
            ((cmd_ptr->hdr.protocol == UIM_GSM) ||
             (cmd_ptr->hdr.protocol == UIM_CDMA)))
        {
          /* Copy the CHV to the cache buffer. The command processing will
             retrieve the CHV digits from the cache buffer. */
          memcpy( uim_chv1[uim_drv_slot].chv, cmd_ptr->verify_chv.chv_digits,
                  UIM_MAX_CHV_DIGITS);
          uim_chv1[uim_drv_slot].use_chv = TRUE;

          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_chv1[uim_drv_slot].chv);

          /* Build the Verify CHV list of states. */
          uim_generic_states[0] = UIM_VERIFY_CHV1_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
        else if ((cmd_ptr->verify_chv.chv == UIM_CHV2) &&
                 ((cmd_ptr->hdr.protocol == UIM_GSM) ||
                  (cmd_ptr->hdr.protocol == UIM_CDMA)))
        {
          /* Copy the CHV to the cache buffer. The command processing will
             retrieve the CHV digits from the cache buffer. */
          memcpy( uim_chv2[uim_drv_slot].chv, cmd_ptr->verify_chv.chv_digits,
                  UIM_MAX_CHV_DIGITS);
          uim_chv2[uim_drv_slot].use_chv = TRUE;

          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_chv2[uim_drv_slot].chv);

          /* Build the Verify CHV list of states.  */
          uim_generic_states[0] = UIM_VERIFY_CHV2_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
#ifdef FEATURE_UIM_UICC
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
        if ((cmd_ptr->verify_chv.chv > UIM_CHV2) &&
            (cmd_ptr->verify_chv.chv < UIM_CHV_MAX) &&
            (cmd_ptr->hdr.protocol == UIM_UICC))
        {
          memcpy( uim_chv2[uim_drv_slot].chv, cmd_ptr->verify_chv.chv_digits,
                  UIM_MAX_CHV_DIGITS);

          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_chv2[uim_drv_slot].chv);

          /* Verify CHV2 state is used for Verifying PIN */
          uim_generic_states[0] = UIM_VERIFY_CHV2_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif		
        {
          MSG_HIGH(" Wrong CHV/PIN %d recd for %d protocol",
                   cmd_ptr->verify_chv.chv,cmd_ptr->hdr.protocol,0);
          status = FALSE;
        }
      } /* end case - UIM_VERIFY_CHV_F */
      break;

    case UIM_CHANGE_CHV_F:              /* Change CHV */
      {
        MSG_MED( "Received Change CHV command ", 0, 0, 0 );
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        /* Determine which CHV is to be changed. */
        if ((cmd_ptr->change_chv.chv == UIM_CHV1) &&
            ((cmd_ptr->hdr.protocol == UIM_GSM) ||
             (cmd_ptr->hdr.protocol == UIM_CDMA)))
        {
          /* Build the Change CHV list of states. The new digits are NOT
             copied to the cache buffer unless the change completes
             successfully. */
          uim_generic_states[0] = UIM_CHANGE_CHV1_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
        else if ((cmd_ptr->change_chv.chv == UIM_CHV2) &&
                 ((cmd_ptr->hdr.protocol == UIM_GSM) ||
                  (cmd_ptr->hdr.protocol == UIM_CDMA)))
        {
          /* Build the Verify CHV list of states. CHV2 is not cached since
             it is NOT automatically verified on wake up. */
          uim_generic_states[0] = UIM_CHANGE_CHV2_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
#ifdef FEATURE_UIM_UICC
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
        if ((cmd_ptr->change_chv.chv > UIM_CHV2) &&
            (cmd_ptr->change_chv.chv < UIM_CHV_MAX) &&
            (cmd_ptr->hdr.protocol == UIM_UICC))
        {
          /* Verify CHV2 state is used for Verifying PIN */
          uim_generic_states[0] = UIM_CHANGE_CHV2_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif		
        {
          MSG_HIGH(" Wrong CHV/PIN %d recd for %d protocol",
                   cmd_ptr->verify_chv.chv,cmd_ptr->hdr.protocol,0);
          status = FALSE;
        }
      } /* end case - UIM_CHANGE_CHV_F */
      break;

    case UIM_DISABLE_CHV_F:             /* Disable CHV */
      {
        MSG_MED( "Received Disable CHV command ", 0, 0, 0 );
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        /* Determine which CHV is to be disabled. */
        if ((cmd_ptr->disable_chv.chv == UIM_CHV1) &&
            ((cmd_ptr->hdr.protocol == UIM_GSM) ||
             (cmd_ptr->hdr.protocol == UIM_CDMA)))
        {
          /* Build the Change CHV list of states. */
          uim_generic_states[0] = UIM_DISABLE_CHV1_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
#ifdef FEATURE_UIM_UICC
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
        if ((cmd_ptr->disable_chv.chv > UIM_CHV2) &&
            (cmd_ptr->disable_chv.chv < UIM_CHV_MAX) &&
            (cmd_ptr->hdr.protocol == UIM_UICC))
        {
          /* Verify CHV2 state is used for Verifying PIN */
          uim_generic_states[0] = UIM_DISABLE_CHV1_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
#endif /* FEATURE_UIM_UICC */
#if (defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)) && \
     defined ( UIM_FEATURE_ALLOW_CHV2_DISABLE)
        else if ((cmd_ptr->disable_chv.chv == UIM_CHV2) &&
                 ((cmd_ptr->hdr.protocol == UIM_GSM) ||
                  (cmd_ptr->hdr.protocol == UIM_CDMA)))
        {
          /* Build the Change CHV list of states. */
          uim_generic_states[0] = UIM_DISABLE_CHV2_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
#endif /* UIM_FEATURE_ALLOW_CHV2_DISABLE */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif            
        {
          MSG_HIGH(" Wrong CHV/PIN %d recd for %d protocol",
                   cmd_ptr->verify_chv.chv,cmd_ptr->hdr.protocol,0);
          status = FALSE;
        }
      } /* end case - UIM_DISABLE_CHV_F */
      break;

    case UIM_ENABLE_CHV_F:              /* Enable CHV */
      {
        MSG_MED( "Received Enable CHV command ", 0, 0, 0 );
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        /* Determine which CHV is to be changed. */
        if ((cmd_ptr->enable_chv.chv == UIM_CHV1) &&
            ((cmd_ptr->hdr.protocol == UIM_GSM) ||
             (cmd_ptr->hdr.protocol == UIM_CDMA)))
        {
          /* Build the Change CHV list of states. The enable digits are
             NOT copied to the cache buffer unless the comand completes
             successfully. */
          uim_generic_states[0] = UIM_ENABLE_CHV1_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
#ifdef FEATURE_UIM_UICC
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
        if ((cmd_ptr->enable_chv.chv > UIM_CHV_NONE) &&
            (cmd_ptr->enable_chv.chv < UIM_CHV_MAX) &&
            (cmd_ptr->hdr.protocol == UIM_UICC))
        {
          /* Verify CHV2 state is used for Verifying PIN */
          uim_generic_states[0] = UIM_ENABLE_CHV1_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
#endif /* FEATURE_UIM_UICC */
#if (defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)) && \
     defined (UIM_FEATURE_ALLOW_CHV2_DISABLE)
        else if ((cmd_ptr->enable_chv.chv == UIM_CHV2) &&
                 ((cmd_ptr->hdr.protocol == UIM_GSM) ||
                  (cmd_ptr->hdr.protocol == UIM_CDMA)))
        {
          /* Build the Change CHV list of states. */
          uim_generic_states[0] = UIM_ENABLE_CHV2_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
#endif /* UIM_FEATURE_ALLOW_CHV2_DISABLE */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif            
        {
          MSG_HIGH(" Wrong CHV/PIN %d recd for %d protocol",
                   cmd_ptr->verify_chv.chv,cmd_ptr->hdr.protocol,0);
          status = FALSE;
        }
      } /* end case - UIM_ENABLE_CHV_F */
      break;

    case UIM_UNBLOCK_CHV_F:             /* Unblock CHV */
      {
        MSG_MED( "Received Unblock CHV command ", 0, 0, 0 );
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        /* Determine which CHV is to be unblocked. */
        if ((cmd_ptr->unblk_chv.chv == UIM_CHV1) &&
            ((cmd_ptr->hdr.protocol == UIM_GSM) ||
             (cmd_ptr->hdr.protocol == UIM_CDMA)))
        {
          /* Build the Unblock CHV list of states. */
          uim_generic_states[0] = UIM_UNBLOCK_CHV1_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
        else if ((cmd_ptr->unblk_chv.chv == UIM_CHV2)&&
                 ((cmd_ptr->hdr.protocol == UIM_GSM) ||
                  (cmd_ptr->hdr.protocol == UIM_CDMA)))
        {
          /* Build the Unblock CHV list of states. */
          uim_generic_states[0] = UIM_UNBLOCK_CHV2_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
#ifdef FEATURE_UIM_UICC
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
        if ((cmd_ptr->unblk_chv.chv > UIM_CHV_NONE) &&
            (cmd_ptr->unblk_chv.chv < UIM_CHV_MAX) &&
            (cmd_ptr->hdr.protocol == UIM_UICC))
        {
          /* Verify CHV2 state is used for Verifying PIN */
          uim_generic_states[0] = UIM_UNBLOCK_CHV2_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif		
        {
          MSG_HIGH(" Wrong CHV/PIN %d recd for %d protocol",
                   cmd_ptr->verify_chv.chv,cmd_ptr->hdr.protocol,0);
          status = FALSE;
        }
      } /* end case - UIM_UNBLOCK_CHV_F */
      break;

    case UIM_STATUS_F:                  /* Status of the Card */
      {
        MSG_MED( "Received UIM Status command ", 0, 0, 0 );

        /* Build the Status list of states. */
        uim_generic_states[0] = UIM_STATUS_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;
      } /* end case - UIM_STATUS_F */
      break;

#ifdef FEATURE_UIM_TOOLKIT
    case UIM_TERMINAL_PROFILE_F:        /* Send a terminal profile */
      {
        MSG_MED( "Received Terminal Profile command ", 0, 0, 0 );

        /* Build the Terminal Profile list of states. */
        uim_generic_states[0] = UIM_TERMINAL_PROFILE_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;
      } /* end case - UIM_TERMINAL_PROFILE_F */
      break;

    case UIM_ENVELOPE_F:                /* Send an envelope command */
      {
        MSG_MED( "Received Envelope command ", 0, 0, 0 );

        /* Build the Envelope list of states. */
        uim_generic_states[0] = UIM_ENVELOPE_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;

      } /* end case - UIM_ENVELOPE_F */
      break;

    case UIM_INTERNAL_FETCH_F:          /* Fetch a proactive command */
      {
        MSG_MED( "Received Internal Fetch command ", 0, 0, 0 );

        /* Build the Fetch list of states. */
        uim_generic_states[0] = UIM_FETCH_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;

      } /* end case - UIM_INTERNAL_FETCH_F */
      break;

    case UIM_TERMINAL_RESPONSE_F:       /* Send a proactive cmd response */
      {
        MSG_MED( "Received Terminal Response command ", 0, 0, 0 );

        /* Build the Terminal Response list of states. */
        uim_generic_states[0] = UIM_TERMINAL_RESPONSE_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;

        /* Initialize the flag */
        uim_rpt_buf.rpt.terminal_response.proactive_cmd_pending = FALSE;
      } /* end case - UIM_TERMINAL_RESPONSE_F */
      break;

    case UIM_POLLING_INTERVAL_F:
      {
        MSG_MED( "Received TK polling command ", 0, 0, 0 );

        /* Build the Terminal Response list of states. */
        uim_generic_states[0] = UIM_POLLING_INTERVAL_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;
      } /* end case - UIM_POLLING_INTERVAL_F */
      break;
#endif /* FEATURE_UIM_TOOLKIT */

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS

    case UIM_PREF_SLOT_CHANGE_F:
      {
        MSG_MED("Received slot change command ", 0, 0, 0 );

        if (cmd_ptr->slot_chg.slot == UIM_SLOT_AUTOMATIC)
        {
          MSG_HIGH(" Wrong slot indicated in slot change cmd ",0,0,0);
          status = FALSE;
        }
        else
        {
          /* Update the preferred slot and the operational slot */
          if (cmd_ptr->slot_chg.app == UIM_GSM_APP)
          {
            /* Use the slot provided in the cmd as pref GSM slot */
            uim_pref_gsm_slot = cmd_ptr->slot_chg.slot;
            uim_set_gsm_slot(FALSE);
          }
          else if (cmd_ptr->slot_chg.app == UIM_CDMA_APP)
          {
            /* Use the slot provided in the cmd as pref CDMA slot */
            uim_pref_cdma_slot = cmd_ptr->slot_chg.slot;
            /* Set the operational slot for CDMA */

            slot = uim_set_cdma_slot();
            /* Let NVRUIM know of the CDMA DF present for the new slot */

            if (((slot == UIM_SLOT_1) && (uim_dfs_1.cdma_df_present)) ||
                ((slot == UIM_SLOT_2) && (uim_dfs_2.cdma_df_present)))
            {
              /* Set the directory present bit for CDMA */
              nvruim_dir_present |= NVRUIM_CDMA_DF_PRESENT;
            }
            else
            {
              /* Clear the bit */
              nvruim_dir_present = (nvruim_dir_present ^ (nvruim_dir_present &
                                                  NVRUIM_CDMA_DF_PRESENT));
            }
          }
          else if (cmd_ptr->slot_chg.app == UIM_TELECOM_APP)
          {
            /* Use the slot provided in the cmd as Telecom slot */
            uim_telecom_slot = cmd_ptr->slot_chg.slot;
          }
          else
          {
            MSG_HIGH(" Invalid App indicated in slot change cmd ",0,0,0);
            status = FALSE;
          }

          /* Set the state as DONE to indicate that there is no further
             processing */
          uim_generic_states[0] = UIM_DONE_ST;
        }
      }
      break;

#ifdef FEATURE_UIM_TOOLKIT

    case UIM_RESET_WITH_TP_F:    /* Switch cards for the toolkit session. */
      {
        MSG_MED( "Received toolkit switch command", 0, 0, 0 );

        /* Reset the directory */
        uim_reset_dir_for_slot(cmd_ptr->hdr.slot);

        /* Traverse through the same states as Internal Wake Up. */
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
        {
          uim_generic_state_ptr = UIM_INTERNAL_WAKE_UP_STATES;
        }
        uim_generic_state_ptr = UIM_INTERNAL_WAKE_UP_STATES;
        /* Start processing select files from the first in the list. */
        select_state_ctr = 0;
      } /* end case - UIM_RESET_WITH_TP_F */
      break;

#endif /* FEATURE_UIM_TOOLKIT */

#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

    case UIM_RESET_F:   /* Perform a warm reset and perform power up */
      {
        MSG_MED( "Received Reset command ", 0, 0, 0 );

        /* Clear out all the cache that UIM has been maintaining and start fresh */
        uim_clear_get_response_cache();

        /* The reset command shall have to bring - back uim to life
         * so let us change the uim_status accordingly
         */
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
        /* The status of UIM is set to Uninitialized since reset the UIM */
        uim_status = UIM_UNINITIALIZED_S;
        /* Clean out the last selected directory as we will reset the card. */
        uim_reset_dir();
        /* !!!!! Do not clear the CSIM directory structure !!!!! */
#else
        /* The status of UIM is set to Uninitialized */
        uim_set_uim_status_for_slot (UIM_UNINITIALIZED_S, cmd_ptr->hdr.slot);
        /* Clear out the directory structure */
        uim_reset_dir_for_slot (cmd_ptr->hdr.slot);
#endif

          /* Reset directory structure */
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
        uim_reset_dir();
#else
        uim_reset_dir_for_slot(cmd_ptr->hdr.slot);
#endif

#if defined (FEATURE_UIM_UICC)
        /* We are going to start fresh with the new instruction
           class. Set the uim_first_instn_class accordingly. Assumes
           that the caller would set the protocol appropriately.
        */
        switch(cmd_ptr->hdr.protocol)
        {
          case UIM_GSM:
          case UIM_CDMA:
            {
              /* Perform a cold reset instead of a warm reset when
               * the reset is issued to change the instruction
               */
              if ( uim_first_inst_class != NV_UIM_FIRST_INST_CLASS_GSM_SIM )
              {
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
                uim_power_down();
#else
                uim_power_down(cmd_ptr->hdr.slot);
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
                rex_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY);
              }
              uim_first_inst_class = NV_UIM_FIRST_INST_CLASS_GSM_SIM;
            }
            break;
          case UIM_UICC:
          case UIM_WCDMA:
            {
              #ifdef FEATURE_UIM_USB_UICC
                /* If we are performing a RESET from USB interface then set
                   inst class to USB UICC else set the inst class to UMTS SIM */
                if(uim_usb_is_usb_interface_selected())
                {
                  uim_first_inst_class = NV_UIM_FIRST_INST_CLASS_USB_UICC;
                }
                else
                {
                  uim_first_inst_class = NV_UIM_FIRST_INST_CLASS_UMTS_SIM;
                }
              #else
                uim_first_inst_class = NV_UIM_FIRST_INST_CLASS_UMTS_SIM;
              #endif /* FEATURE_UIM_USB_UICC */
            }
            break;
          default:
            break;
        }
#endif

        /* Traverse through the states to reset the UIM.
           Perform the same as a power-up.  This processing assumes
           the calling task will perform any other UIM commands after
           resetting the UIM. */
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
        {
          uim_generic_state_ptr = UIM_INTERNAL_PUP_STATES;
        }
        /* Start processing select files from the first in the list. */
        select_state_ctr = 0;

        /* Consider an external reset as a initial power-up */
        uim_me_powerup = TRUE;
      } /* end case - UIM_RESET_F */
      break;

  case UIM_RESET_SWITCH_UIM_PASSIVE_F:
  case UIM_POWER_UP_UIM_PASSIVE_F:
  {
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
    {
      MSG_MED( "Received Reset to make UIM passive ", 0, 0, 0 );
      /* Traverse through the atr and pps states only */
      uim_generic_state_ptr = UIM_RST_SWITCH_UIM_PASSIVE_STATES;
    }
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
    /* The status of UIM is set to Uninitialized since reset the UIM */
    uim_status = UIM_UNINITIALIZED_S;
    /* Clean out the last selected directory as we will reset the card. */
    uim_reset_dir();
    /* !!!!! Do not clear the CSIM directory structure !!!!! */
#else
    /* The status of UIM is set to Uninitialized */
    uim_set_uim_status_for_slot (UIM_UNINITIALIZED_S, cmd_ptr->hdr.slot);
    /* Clear out the directory structure */
    uim_reset_dir_for_slot (cmd_ptr->hdr.slot);
    MSG_HIGH(" Resetting slot %d ", cmd_ptr->hdr.slot,0,0);
#endif
  }
    break;

  case UIM_POWER_DOWN_F:
      {
        MSG_HIGH( "Received power down command", 0, 0, 0 );
        if (uim_powerdown)
        {
          /* Since the UIM is already powered down,fake a response */
          uim_generic_states[0] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
          /* Set the response command status as done */
          uim_rsp_buf_ptr->cmd_status = UIM_DONE;
          /* Ensure that the command response signal is processed */
          (void) rex_set_timer(&uim_delay_timer,0);
          /* Since we don't want to process any more states we return */
          return (status);
        }
        else
        {
          uim_generic_states[0] = UIM_POWER_DOWN_ST;
          uim_generic_states[1] = UIM_DONE_ST;
          uim_generic_state_ptr = uim_generic_states;
        }
      }
      break;
    case UIM_STREAM_APDU_F:       /* Send an APDU to the card */
      {
        MSG_MED( "Received Stream APDU command ", 0, 0, 0 );

        /* Build the Stream APDU list of states. */
        uim_generic_states[0] = UIM_STREAM_APDU_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;

        /* Check to make sure that the instruction does not match with
           commands for which we do directory maintenance */
        switch ((uim_instrns_enum_type)(cmd_ptr->stream_apdu.instrn))
        {
          case SELECT:
          case STATUS:
          case READ_BINARY:
          case UPDATE_BINARY:
          case READ_RECORD:
          case UPDATE_RECORD:
          case SEEK:
          case INCREASE:
          case INVALIDATE:
          case REHABILITATE:
#ifdef  FEATURE_UIM_GSM
#error code not present
#elif defined (FEATURE_UIM_USIM)
#error code not present
#elif (defined (FEATURE_UIM_RUIM) && defined (FEATURE_AUTH))
          case RUN_CAVE:
#endif
          case SLEEP:
          case GET_RESPONSE:
          case TERMINAL_PROFILE:
          case ENVELOPE:
          case FETCH:
          case TERMINAL_RESPONSE:

#ifdef FEATURE_UIM_RUIM
#ifdef FEATURE_AUTH
          case UPDATE_SSD:
          case BS_CHALLENGE:
          case CONFIRM_SSD:
          case GEN_KEY_VPM:
#endif /* FEATURE_AUTH */

#ifdef FEATURE_OTASP
          case MS_KEY_REQUEST:
          case KEY_GEN_REQUEST:
          case CONFIG_REQUEST:
          case DOWNLOAD_REQUEST:
          case SSPR_CONFIG_REQUEST:
          case SSPR_DOWNLOAD_REQUEST:
          case OTAPA_REQUEST:
          case COMMIT:
          case VALIDATE:
#endif /* FEATURE_OTASP */

          case STORE_ESN_ME:
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_UIM_UTIL
          case MANAGE_CHANNEL:
#ifdef FEATURE_UIM_WIM
          case MSE:
          case PSO:
#ifndef FEATURE_AUTH
          case ASK_RANDOM:
#endif
#endif /* FEATURE_UIM_WIM */
#endif /* FEATURE_UIM_UTIL */

            status = FALSE;

          break;

          /* Allow the processing of commands only for PIN related commands and
             those that we do not recognize */
          default:
            status = TRUE;
          break;

        } /* end of switch */
      } /* end case - UIM_STREAM_APDU_F */
    break;

    case UIM_STREAM_ISO7816_APDU_F:     /* Stream an APDU - extended version */
      {
        /* Variables to hold the class (including channel) byte of the supplied
         * streaming APDU and the instruction supplied respectively.
         */
        byte class_byte,instrn;
        MSG_MED( "Received Stream 7816 APDU command ", 0, 0, 0 );



        /* Build the Stream 7816 APDU list of states. */
        uim_generic_states[0] = UIM_STREAM_APDU_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;

        MSG_MED("APDU params class,ins,p1 %x %x %x",
             cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_CLASS_OFFSET],
             cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_INSTRN_OFFSET],
             cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_P1_OFFSET]);

        MSG_MED("APDU params p2,p3,d1 %x %x %x",
             cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_P2_OFFSET],
             cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE3_4_7816_LC_OFFSET],
             cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE2_7816_APDU_SIZE]);

        MSG_MED("APDU parameters d2 %x",
             cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE2_7816_APDU_SIZE+1],
             0,0);

        /* Extract the class byte out of the APDU */
        class_byte =
          cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_CLASS_OFFSET];

        /* Extract the instruction byte out of the APDU*/
        instrn =
           cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_INSTRN_OFFSET];

        /* Let the commands be streamed in the default channel as well if in
         * passive mode
         */
        if ( uim_mode == UIM_PASSIVE_MODE )
        {
          /* When streaming in passive mode, let us store the protocol
           * to be used for internal poll from the header of the streamed
           * APDU on the default channel.  Ignore a manage channel
           * with UICC instruction class since it is acceptable
           * on a java card though an ICC.
           */
          if ( ((class_byte & UIM_LOGICAL_CHANNEL_MASK) == 0 )&&
              !((class_byte == UIM_UICC_INSTRUCTION_CLASS1)
                && (instrn == MANAGE_CHANNEL)) )
          {
          
#ifndef UIM_INSTRUCTION_CLASS
#define UIM_INSTRUCTION_CLASS 0xA0
#endif
          
            if (class_byte == UIM_INSTRUCTION_CLASS)
            {
              uim_dfs_1.protocol = UIM_GSM;
            }
            else
            {
              uim_dfs_1.protocol = UIM_UICC;
            }

            /* Start polling for presence detection once the
             * instruction class is known.  And reset the poll
             * timer if this is the first time.
             */
            if ( ( (int)uim_ctl & (int)UIM_PASSIVE ) != ((int)UIM_PASSIVE) )
            {
              /* Indicate that we can start the polling using the vote */
              uim_ctl = (uim_voter_type) ((int)uim_ctl | (int)UIM_PASSIVE);

              /* Just entered the passive mode.  Set the time to poll to the
               * default value and restart the timer to start counting
               * from the time we know the instruction class.
               */
              uim_set_poll_timer( UIM_POLL_TIME );
            }
          }
          break;
        }

        /* Check if the command is streamed in default channel */
        if (!(class_byte & UIM_LOGICAL_CHANNEL_MASK))
        {
#ifndef FEATURE_UIM_SUPPORT_CSIM
          /* Disallow the command */
          status = FALSE;
          MSG_HIGH ("Streaming not permitted in default channel",0,0,0);
#else
          if ( uim_cur_channel != UIM_DEFAULT_CHANNEL )
          {
            MSG_ERROR("Stream on default with hdr->channel != 0 %d",
                      uim_cur_channel, 0, 0);
            /* Set the cur channel to the default channel just in case */
            uim_cur_channel = UIM_DEFAULT_CHANNEL;
          }
          /* Reset the select state counter */
          select_state_ctr = 0;

#ifdef FEATURE_UIM_UICC
          /* Check if USIM command being streamed on the default channel */
          if ( (class_byte == UIM_UICC_INSTRUCTION_CLASS1) ||
               (class_byte == UIM_UICC_INSTRUCTION_CLASS2))
          {
            /* Var to store P1,P2 from the streamed APDU */
            byte apdu_p1, apdu_p2;

            /* Vars to store the csim dir and current dir */
            uim_last_sel_dir_type *csim_dir, *curr_dir;

            csim_dir = &uim_csim_last_sel_dir[uim_drv_slot];
            curr_dir = &uim_last_sel_dir[uim_cur_channel][uim_drv_slot];

            /* This is a usim command */
            MSG_HIGH("Streaming UICC on default channel",0,0,0);

            /* Set the protocol for the command */
            cmd_ptr->hdr.protocol = UIM_UICC;

            /* Initialize select required to FALSE */
            select_reqd = FALSE;

            /* Get the value of P1 & P2 from the streamed APDU */
            apdu_p1 =
              cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_P1_OFFSET];

            apdu_p2 =
              cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_P2_OFFSET];

            /* If select command being streamed */
            if ( (uim_instrns_enum_type)(instrn) == SELECT )
            {
              /* Select by DF name i.e SELECT ADF*/
              if ( apdu_p1 & UIM_USIM_SELECT_BY_DF_NAME )
              {
                byte *stream_aid, *curr_aid;
                byte stream_aid_len, curr_aid_len, min_len;

                stream_aid =
                  &(cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE3_4_7816_LC_OFFSET+1]);

                stream_aid_len =
                  cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE3_4_7816_LC_OFFSET];


                curr_aid =
                  uim_last_sel_dir[uim_cur_channel][uim_drv_slot].curr_app.data;

                curr_aid_len =
                  uim_last_sel_dir[uim_cur_channel][uim_drv_slot].curr_app.aid_length;

                /* Get the minimum of the supplied and the curr_app length */
                min_len = (curr_aid_len > stream_aid_len ) ?
                  stream_aid_len : curr_aid_len;

                /* Compare the data with the curr app */
                if ( memcmp((void *)stream_aid, curr_aid, min_len) == 0 )
                {
                  /*The aid supplied and the current aid are same to the minimum length */
                  MSG_HIGH("APP active sel 7FFF cur app len %d, apdu len %d",
                           curr_aid_len, stream_aid_len, 0);

                  /* We would simply force a select on 7fff and return
                   * the result.  The response would be similar to that of
                   * selection by AID.
                   */
                  uim_generic_states[0] = UIM_SELECT_ST;
                  uim_generic_states[2] = UIM_DONE_ST;
                  uim_generic_state_ptr = uim_generic_states;

                  /* Build the path to be selected as 3F00, 7FFF, NULL */
                  uim_path_to_be_sel[0] = UIM_MF_ID;
                  uim_path_to_be_sel[1] = UIM_SEL_AID_BY_FILE_ID;
                  uim_path_to_be_sel[2] = UIM_NO_FILE_SEL;
                }
                else
                {
                  MSG_ERROR("Not allowed to active other app on default channel",
                          0, 0, 0);
                  /* Fail the command */
                  status = FALSE;

                  break;
                }
              } /* select by df name */
              else if ( apdu_p1 & UN_SUPPORTED_SEL )
              {
                /* This is a select child or parent or by path from current DF
                 * which are not supported
                 */
                MSG_ERROR("Select child, parent or from current not supported",
                          0, 0, 0);
                status = FALSE;

                break;
              } /* sel child, parent or path from current df */
              else if ( apdu_p1 & UIM_USIM_SELECT_BY_PATH_FROM_MF )
              {
                /* If the select that we received is a select by
                 * path from MF, we do not have to select the last
                 * selected CSIM since this selection starts with
                 * MF any way */
                MSG_HIGH("No Select required for Sel by path", 0, 0, 0);
              }
              else if ( apdu_p1 == UIM_USIM_SELECT_BY_FILE_ID )
              {
                if ( memcmp(csim_dir, curr_dir,
                            sizeof(uim_last_sel_dir_type))
                   )
                {
                  /* Select by file-id and mismatch with last selection */
                  select_reqd = TRUE;
                }
              }
              else
              {
                MSG_ERROR("UN-Known P1 for select.. Failing", 0, 0, 0);

                /* Fail the command */
                status = FALSE;

                break;
              }
            } /* if the instruction is select */
            else if ( ( ((uim_instrns_enum_type)(instrn) == READ_BINARY  ) ||
                        ((uim_instrns_enum_type)(instrn) == UPDATE_BINARY) ||
                        ((uim_instrns_enum_type)(instrn) == INCREASE ) ) &&
                      ( apdu_p1 & SFI_USED_IN_P1 )
                    )
            {
              /* The instruction is one of read bin, update bin and increase,
               * with usage of SFI which is not supported
               */
              MSG_ERROR("SFI selection is not supported", 0, 0, 0);
              status = FALSE;

              break;
            }/* Read Bin, Update Bin or Increase*/
            else if ( ( ((uim_instrns_enum_type)(instrn) == READ_RECORD   ) ||
                        ((uim_instrns_enum_type)(instrn) == UPDATE_RECORD ) ||
                        ((uim_instrns_enum_type)(instrn) == SEEK ) ) &&
                      ( apdu_p2 & SFI_USED_IN_P2 )
                    )
            {
              /* The instruction is one of read rec, update rec or seek,
               * with usage of SFI which is not supported
               */
              MSG_ERROR("SFI selection is not supported", 0, 0, 0);
              status = FALSE;

              break;
            } /* Read Rec, Update Rec or Search Rec */
            else if ( memcmp(csim_dir, curr_dir,
                             sizeof(uim_last_sel_dir_type))
                    )
            {
              /* Any command other than select with the last selection
               * not the same as CSIM last selection and SFI not used
               */
              MSG_HIGH("CSIM Internal selection required", 0, 0, 0);
              /* The CSIM selection is not where it left off do an
               * internal select
               */
              select_reqd = TRUE;
            }

            /* It has been identified that an internal select is needed */
            if ( select_reqd )
            {
              int i = 0;

              /* last selection by CSIM is different from the
               * current selection.  Blindly  do a select on what was
               * last selected
               */

              /* Initialize path to be selected to no file */
              for ( i = 0; i< UIM_MAX_PATH_ELEMENTS; i++ )
              {
                uim_path_to_be_sel[i] = UIM_NO_FILE_SEL;
              }


              /* Re-initialize the counter */
              i = 0;

              /* Always start with MF so that if nothing is selected,
               * MF would be selected and also, select by path from
               * MF bit would be set to simplify this select.
               */

              uim_path_to_be_sel[i++] = UIM_MF_ID;

              if ( csim_dir->df1_type == UIM_DF )
              {
                /* DF1 is something other than ADF. select it */
                uim_path_to_be_sel[i++] = csim_dir->df1.df;
              }
              else if ( csim_dir->df1_type == UIM_ADF )
              {
                /* select USIM adf... actually the active app */
                uim_path_to_be_sel[i++] = UIM_SEL_AID_BY_FILE_ID;
              }

              if ( csim_dir->df2 != UIM_NO_FILE_SEL )
              {
                /* DF2 has some value. select it */
                uim_path_to_be_sel[i++] = csim_dir->df2;
              }

              if ( csim_dir->ef_sel )
              {
                /* Ef was selected before.  Select it */
                uim_path_to_be_sel[i++] = csim_dir->ef;
              }
              /* Build the Stream 7816 APDU list of states */
              uim_generic_states[0] = UIM_SELECT_ST;
              uim_generic_states[1] = UIM_STREAM_APDU_ST;
              uim_generic_states[2] = UIM_DONE_ST;
              uim_generic_state_ptr = uim_generic_states;

              MSG_HIGH("Internal select required for CSIM", 0, 0, 0);

              MSG_HIGH("Last DF1,DF2,EF %x %x %x",
                             uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df,
                             uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2,
                             uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef);

              MSG_HIGH("CSIM DF1,DF2,EF %x %x %x",
                             uim_csim_last_sel_dir[uim_drv_slot].df1.df,
                             uim_csim_last_sel_dir[uim_drv_slot].df2,
                             uim_csim_last_sel_dir[uim_drv_slot].ef);

              MSG_HIGH("New path %x %x %x",
                             uim_path_to_be_sel[UIM_PATH_DF1_INDEX],
                             uim_path_to_be_sel[UIM_PATH_DF2_INDEX],
                             uim_path_to_be_sel[UIM_PATH_EF_INDEX]);
              MSG_HIGH("and  %x", uim_path_to_be_sel[UIM_PATH_EF_INDEX+1], 0, 0);
            } /* select_reqd */
            else
            {
              MSG_LOW("No internal select required for CSIM", 0, 0, 0);
            }
          } /* UICC instruction class */
#else /* FEATURE_UIM_UICC */
          /* Reject the command if the class in not GSM */
          if ( (class_byte != UIM_INSTRUCTION_CLASS) &&
               (instrn != MANAGE_CHANNEL)
             )
          {
            status = FALSE;
            MSG_HIGH ("Non A0 commands disallowed in default channel",0,0,0);
          }
#endif /* FEATURE_UIM_UICC */
          else
          {
            MSG_MED("Recd 7816 CSIM APDU command", 0, 0, 0 );

            /* Set the protocol for the command */
            cmd_ptr->hdr.protocol = UIM_GSM;

            /* Initialize select required to TRUE */
            select_reqd = TRUE;

            /* Very minimal checking is done here */
            switch ((uim_instrns_enum_type)(instrn))
            {
              case SELECT:
              {
                path = (word)FLIPW (cmd_ptr->stream_iso7816_apdu.cmd_data +
                                       UIM_CASE3_4_7816_LC_OFFSET + 1);

                /* Set the path counter to zero */
                j = 0;

                switch ((path & UIM_FILE_ID_LEVEL_MASK) >> UIM_FILE_ID_LEVEL_SHIFT )
                {
                  case UIM_MF_LEVEL:
                  case UIM_DF_LEVEL_1:
                      /* MF and first level DF can be selected from anywhere */
                    select_reqd = FALSE;
                  break;

                  case UIM_DF_LEVEL_2:
                  case UIM_EF_UNDER_1ST_DF:
                    /* Check to see if DF1 is the same between CSIM structure
                       and last sel dir structure */
                    if ((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df !=
                             uim_csim_last_sel_dir[uim_drv_slot].df1.df) ||
                        (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type != UIM_DF))
                    {
                      /* Select MF and DF1 from CSIM structure first */
                      uim_path_to_be_sel[j++] = UIM_MF_ID;

                      if (uim_csim_last_sel_dir[uim_drv_slot].df1.df !=
                                                              UIM_NO_FILE_SEL)
                      {
                        uim_path_to_be_sel[j++] =
                                    uim_csim_last_sel_dir[uim_drv_slot].df1.df;
                      }
                      else
                      {
                        /* Send it to the card anyway */
                        select_reqd = FALSE;
                        MSG_HIGH ("DF1 is not selected by CSIM",0,0,0);
                      }
                    }
                    else
                    {
                      select_reqd = FALSE;
                    }
                  break;

                  case UIM_EF_UNDER_MF:
                    /* Always select the MF before selecting the EF */
                    uim_path_to_be_sel[j++] = UIM_MF_ID;
                  break;

                  case UIM_EF_UNDER_2ND_DF:
                    if ((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type != UIM_DF) ||
                        (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df !=
                           uim_csim_last_sel_dir[uim_drv_slot].df1.df) ||
                        (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 !=
                           uim_csim_last_sel_dir[uim_drv_slot].df2))
                    {
                       /* Select MF and DF1 from CSIM structure first */
                      uim_path_to_be_sel[j++] = UIM_MF_ID;

                      if (uim_csim_last_sel_dir[uim_drv_slot].df1.df !=
                                                               UIM_NO_FILE_SEL)
                      {
                        uim_path_to_be_sel[j++] =
                                    uim_csim_last_sel_dir[uim_drv_slot].df1.df;
                      }
                      else
                      {
                        MSG_HIGH ("DF1 is not selected by CSIM",0,0,0);
                        status = FALSE;
                      }
                      if (uim_csim_last_sel_dir[uim_drv_slot].df1.df !=
                                                               UIM_NO_FILE_SEL)
                      {
                        uim_path_to_be_sel[j++] =
                                       uim_csim_last_sel_dir[uim_drv_slot].df2;
                      }
                      else
                      {
                        /* Send it to the card anyway */
                        select_reqd = FALSE;
                        MSG_HIGH ("DF2 is not selected by CSIM",0,0,0);
                      }
                    }
                    else
                    {
                      select_reqd = FALSE;
                    }
                  break;

                  default:
                    status = FALSE;
                    MSG_HIGH ("Unrecognized path type %x", path, 0, 0);
                  break;
                }
              }
              break;

#ifdef  FEATURE_UIM_GSM
#error code not present
#endif

              /* commands involving an EF */
              case READ_BINARY:
              case UPDATE_BINARY:
              case READ_RECORD:
              case UPDATE_RECORD:
              case SEEK:
              case INCREASE:
              case INVALIDATE:
              case REHABILITATE:
              {
                /* If no EF was selected */
                if (!uim_csim_last_sel_dir[uim_drv_slot].ef_sel)
                {
                  /* Send the command anyway to the card */
                  select_reqd = FALSE;
                  MSG_HIGH ("No CSIM EF was selected for this operation",0,0,0);
                }
                /* If the last selected structure does not match with the csim
                   structure, then select the entire csim path */
                else if ((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type != UIM_DF) ||
                         (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df !=
                                uim_csim_last_sel_dir[uim_drv_slot].df1.df))
                {
                  uim_path_to_be_sel[j++] = UIM_MF_ID;

                  if (uim_csim_last_sel_dir[uim_drv_slot].df1.df !=
                                                               UIM_NO_FILE_SEL)
                  {
                    uim_path_to_be_sel[j++] =
                                    uim_csim_last_sel_dir[uim_drv_slot].df1.df;
                  }

                  if (uim_csim_last_sel_dir[uim_drv_slot].df2 !=
                                                               UIM_NO_FILE_SEL)
                  {
                    uim_path_to_be_sel[j++] =
                                    uim_csim_last_sel_dir[uim_drv_slot].df2;
                  }

                  uim_path_to_be_sel[j++] =
                                        uim_csim_last_sel_dir[uim_drv_slot].ef;
                }
                else if ((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 !=
                                 uim_csim_last_sel_dir[uim_drv_slot].df2) &&
                         (uim_csim_last_sel_dir[uim_drv_slot].df2 !=
                                 UIM_NO_FILE_SEL))
                {
                  uim_path_to_be_sel[j++] =
                                    uim_csim_last_sel_dir[uim_drv_slot].df2;

                  uim_path_to_be_sel[j++] =
                                    uim_csim_last_sel_dir[uim_drv_slot].ef;
                }
                else if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef !=
                                    uim_csim_last_sel_dir[uim_drv_slot].ef)
                {
                  uim_path_to_be_sel[j++] =
                                    uim_csim_last_sel_dir[uim_drv_slot].ef;
                }
                else
                {
                  select_reqd = FALSE;
                }
              }
              break;

              /* commands that do not require any particular selection */
              case VERIFY_CHV:
              case CHANGE_CHV:
              case DISABLE_CHV:
              case ENABLE_CHV:
              case UNBLOCK_CHV:
              case TERMINAL_PROFILE:
              case ENVELOPE:
              case TERMINAL_RESPONSE:
              case MANAGE_CHANNEL:
                select_reqd = FALSE;
              break;

              case GET_RESPONSE:
              case FETCH:
                /* Driver internally sends these commands, so disallow them */
                status = FALSE;
                MSG_HIGH ("Get Resp/Fetch not allowed %x",instrn,0,0);
              break;

              default:
                /* No internal action is performed for these commands */
                select_reqd = FALSE;
                MSG_HIGH ("No internal action for command %x",instrn,0,0);
              break;
            }

            if (select_reqd && status)
            {
              /* Build the Stream 7816 APDU list of states */
              uim_generic_states[0] = UIM_SELECT_ST;
              uim_generic_states[1] = UIM_STREAM_APDU_ST;
              uim_generic_states[2] = UIM_DONE_ST;
              uim_generic_state_ptr = uim_generic_states;

              MSG_HIGH("Internal select required for CSIM", 0, 0, 0);

              MSG_MED("Last DF1,DF2,EF %x %x %x",
                             uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df,
                             uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2,
                             uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef);

              MSG_MED("CSIM DF1,DF2,EF %x %x %x",
                             uim_csim_last_sel_dir[uim_drv_slot].df1.df,
                             uim_csim_last_sel_dir[uim_drv_slot].df2,
                             uim_csim_last_sel_dir[uim_drv_slot].ef);

              MSG_MED("New path %x %x %x",
                             uim_path_to_be_sel[UIM_PATH_DF1_INDEX],
                             uim_path_to_be_sel[UIM_PATH_DF2_INDEX],
                             uim_path_to_be_sel[UIM_PATH_EF_INDEX]);

              /* !!!!!! CSIM last selected directory structure will not be
                 updated for internal selects as the goal is to select the files
                 in the csim structure !!!!!!! */
            }
          }
#endif /* FEATURE_UIM_SUPPORT_CSIM */
        } /* Streamed on default channel */

#ifdef FEATURE_UIM_SUPPORT_CSIM
        if(cmd_ptr->stream_iso7816_apdu.num_cmd_bytes < UIM_CSIM_APDU_MIN_SIZE)
#else
        if (cmd_ptr->stream_iso7816_apdu.num_cmd_bytes < UIM_7816_APDU_MIN_SIZE)
#endif
        {
          /* There are not enough bytes in the command */
          MSG_HIGH ("Insufficient number of bytes in the command %d",
                            cmd_ptr->stream_iso7816_apdu.num_cmd_bytes,0,0);
          status = FALSE;
        }
      } /* end case - UIM_STREAM_ISO7816_APDU_F */
    break;

    case UIM_CLEAR_CACHE_F:     /* Clear uim cache */
      {
        MSG_HIGH( "Received Clear Cache command", 0, 0, 0 );
        uim_generic_states[0] = UIM_CLEAR_CACHE_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;
      }
      break;

    case UIM_RECOVERY_COMPLETE_F: /* switch to normal mode */
      {
        MSG_HIGH("Received recovery complete", 0, 0, 0);
                        uim_generic_states[0] = UIM_RECOVERY_COMPLETE_ST;
        uim_generic_states[1] = UIM_DONE_ST;
        uim_generic_state_ptr = uim_generic_states;
      }
      break;

    default:
      /* Whatever is not processed above may be a custom command. */
      uim_custom_generic_cmd_processor(cmd_ptr, uim_req_buf_ptr,
                                       uim_rsp_buf_ptr,
                                       uim_command_response_callback);
      break;

  } /* end switch (command_type) */

  if (status)
  {
    /* Call the state machine. */
    uim_generic_command (cmd_ptr);
  }
  else
  {
    /* Send an error report to the client and release the buffer */
    uim_rpt_buf.sw1 = (uim_sw1_type) 0;
    uim_rpt_buf.sw2 = (uim_sw2_type) 0;

    uim_send_generic_err_rpt(cmd_ptr);
  }

  return(status);

} /* uim_process_generic_command() */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_GENERIC_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the UIM
  at the completion of the command processing and releases the command
  buffer for a generic command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer.

===========================================================================*/

void uim_process_generic_response
(
  uim_rsp_buf_type const *rsp_ptr,
  uim_cmd_type *cmd_ptr
)
{
  /* save the following info to place the buffer back on the requested queue */
  q_type *save_done_q_ptr;
  q_link_type *save_link_ptr;

#if defined(FEATURE_UIM_UICC) && defined(FEATURE_UIM_SUPPORT_CSIM)
  /* Path counter for use in selection by path ref */
  byte path_ctr = 0 ;
#endif

#ifdef FEATURE_UIM_SUPPORT_CSIM
  word path;
#endif /* FEATURE_UIM_SUPPORT_CSIM */

  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_process_generic_response cmd_ptr is NULL",0,0,0);
  }

  if (rsp_ptr == NULL)
  {
    ERR_FATAL("uim_process_generic_response rsp_ptr is NULL",0,0,0);
  }

  save_done_q_ptr = cmd_ptr->hdr.cmd_hdr.done_q_ptr;
  save_link_ptr   = &cmd_ptr->hdr.cmd_hdr.link;

  /* We will reset the report options flag in the command to NONE for
     backward compatibility purposes */

  cmd_ptr->hdr.options = UIM_OPTION_NONE;

  /* Set the status to UIM_PASS unless something failed */
  uim_rpt_buf.rpt_status = UIM_PASS;

  switch (cmd_ptr->hdr.command)
  {
    case UIM_INTERNAL_ME_PUP_F:         /* POWER UP UIM due to task start up */
    case UIM_INTERNAL_POLL_F:           /* Internal command to Poll */
        /* do nothing */
      break;
    case UIM_INTERNAL_WAKE_UP_F:        /* Power up due to Power management */
      gsdi_uim_notify_reset(UIM_RESET_COMPLETE_S);

      /* Save the last mode other than recovery mode */
      if ( uim_mode != UIM_RECOVERY_MODE )
      {
        MSG_HIGH("Saving current mode %d and switching to recovery mode",
                 uim_mode, 0, 0);
        /* Free any read write info it would be re-allocated as needed */
        uim_free_reset_rd_wr_info();
#if defined (FEATURE_UIM_USIM)
#error code not present
#endif /*defined (FEATURE_UIM_USIM)*/
        uim_save_mode = uim_mode;
      }

      /* Put the UIM to check for recovery commands only */
      uim_mode = UIM_RECOVERY_MODE;
      break;


    case UIM_INTERNAL_SELECT_F:         /* Internal Select command */
#ifdef FEATURE_UIM_UICC
        /* Reset the flag */
        uim_sel_by_path_ref_from_mf = FALSE;
#endif
      break;

    case UIM_READ_REC_CMD_F:
    case UIM_READ_BIN_CMD_F:
      {
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          if (cmd_ptr->hdr.command == UIM_READ_REC_CMD_F)
          {
            uim_rpt_buf.rpt_type = UIM_READ_REC_CMD_R;
          }
          else
          {
            uim_rpt_buf.rpt_type = UIM_READ_BIN_CMD_R;
          }

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          uim_rpt_buf.rpt.read.num_bytes_read =
            uim_rd_wr_info.num_bytes_rsp;

          uim_rpt_buf.rpt.read.data =
            uim_rd_wr_info.alloc_mem;

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting UIM READ", 0, 0, 0 );

          /* Release the allocated memory for reads/inc commands */
          uim_free_reset_rd_wr_info();

        } /* (cmd_ptr->hdr.rpt_function != NULL) */
      }
      break;

    case UIM_WRITE_BIN_CMD_F:
    case UIM_WRITE_REC_CMD_F:
      {
        if (cmd_ptr->hdr.command == UIM_WRITE_BIN_CMD_F)
        {
          uim_rpt_buf.rpt_type = UIM_WRITE_BIN_CMD_R;
        }
        else
        {
          uim_rpt_buf.rpt_type = UIM_WRITE_REC_CMD_R;
        }

        /* Fill in necessary fields of command block */
        uim_rpt_buf.rpt_hdr.task_ptr = NULL;

        uim_rpt_buf.rpt.write.num_bytes_written =
          uim_rd_wr_info.num_bytes_rsp;

        /* Report to the Requesting task */
        (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );

        /* Clear all the static variables */
        uim_free_reset_rd_wr_info();

        MSG_MED( "Reporting UIM WRITE", 0, 0, 0 );
      }
      break;

    case UIM_INC_CMD_F:
      {
        uim_rpt_buf.rpt_type = UIM_INC_CMD_R;

        /* Fill in necessary fields of command block */
        uim_rpt_buf.rpt_hdr.task_ptr = NULL;

        if ( !uim_alloc_set_rd_wr_info(rsp_ptr->cmd_rsp_size) )
        {
          uim_rpt_buf.rpt_status = UIM_FAIL;

          uim_rpt_buf.rpt.inc.inc_rsp_len = 0;

          uim_rpt_buf.rpt.inc.data        = NULL;
        }
        else
        {

          uim_rpt_buf.rpt.inc.inc_rsp_len =
            rsp_ptr->cmd_rsp_size;

          memcpy(uim_rd_wr_info.alloc_mem, rsp_ptr->rsp.data,
                 rsp_ptr->cmd_rsp_size);


          uim_rpt_buf.rpt.inc.data =
            uim_rd_wr_info.alloc_mem;
        }

        /* Report to the Requesting task */
        (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
        MSG_MED( "Reporting UIM INC", 0, 0, 0 );

        /* Release the allocated memory for reads/inc commands */
        uim_free_reset_rd_wr_info();
      }
      break;

    case UIM_ACCESS_F:                  /* Read or Write from UIM */
      {
        /* Report back that Access was completed, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_ACCESS_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* copy the data into the buffer from NV */
          if ( cmd_ptr->access_uim.num_bytes >= rsp_ptr->cmd_rsp_size )
          {
             /* Fill in the number of bytes in the response that we got
             from UIM into the NV command */

             cmd_ptr->access_uim.num_bytes_rsp = rsp_ptr->cmd_rsp_size;

             memcpy(cmd_ptr->access_uim.data_ptr, rsp_ptr->rsp.data,
                 rsp_ptr->cmd_rsp_size);
          }
          else
          {
             memcpy(cmd_ptr->access_uim.data_ptr, rsp_ptr->rsp.data,
                 cmd_ptr->access_uim.num_bytes);

             cmd_ptr->access_uim.num_bytes_rsp = cmd_ptr->access_uim.num_bytes;
          }

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting UIM Access", 0, 0, 0 );

        } /* (cmd_ptr->hdr.rpt_function != NULL) */
      } /* end case - UIM_ACCESS_F */
      break;

    case UIM_CACHED_SEL_F:
    case UIM_SELECT_F:            /* Select a file/directory from the UIM */
      {
#ifdef FEATURE_UIM_UICC
        /* Reset the flag */
        uim_sel_by_path_ref_from_mf = FALSE;
#endif
        /* Report back that Select was completed, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          if ( cmd_ptr->hdr.command == UIM_SELECT_F )
          {
            uim_rpt_buf.rpt_type = UIM_SELECT_R;
          }
          else
          {
            uim_rpt_buf.rpt_type = UIM_CACHED_SEL_R;
          }

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Fill in the number of bytes in the report */
          uim_rpt_buf.rpt.select.num_data_bytes = rsp_ptr->cmd_rsp_size;

          /* copy the data into the report buffer  */
          memcpy(uim_rpt_buf.rpt.select.data, rsp_ptr->rsp.data,
                 rsp_ptr->cmd_rsp_size);

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting UIM Select", 0, 0, 0 );

        } /* (cmd_ptr->hdr.rpt_function != NULL) */
      } /* end case - UIM_SELECT_F */
      break;

    case UIM_SEEK_F:                    /* Seek from the UIM */
      {
        /* Report back that Seek was completed, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_SEEK_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Fill in the number of bytes in the response that we got
            from UIM into the NV command */
          uim_rpt_buf.rpt.seek.num_records = rsp_ptr->cmd_rsp_size;

          /* copy the data into the report buffer */
          memcpy(uim_rpt_buf.rpt.seek.data, rsp_ptr->rsp.data,
                 rsp_ptr->cmd_rsp_size);

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting UIM Seek", 0, 0, 0 );

        } /* (cmd_ptr->hdr.rpt_function != NULL) */
      } /* end case - UIM_SEEK_F */
      break;

    case UIM_INVALIDATE_F:              /* Invalidate an EF */
      {
        /* The item has been invalidated if it's get response has been
         * cached.... remove it from the cache so that the next select
         * on the item is forced to do a get response
         */

        /* If we have a pointer to the item */
        if ( select_rsp_cache_item_ptr != NULL)
        {
          /* Indicate that the file attributes are no longer cached */
          select_rsp_cache_item_ptr->is_cached = FALSE;

          /* If response cache has been allocated */
          if (select_rsp_cache_item_ptr->data != NULL)
          {
            /* Free the cache */
            uim_free(select_rsp_cache_item_ptr->data);
            select_rsp_cache_item_ptr->data = NULL;
          }
        }

        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_INVALIDATE_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting UIM Invalidate", 0, 0, 0 );
        }
      } /* end case - UIM_INVALIDATE_F */
      break;

    case UIM_REHABILITATE_F:            /* Rehabilitate an EF */
      {
        /* The item has been rehabilitated if it's get response has been
         * cached.... remove it from the cache so that the next select
         * on the item is forced to do a get response
         */

        /* If we have a pointer to the item */
        if ( select_rsp_cache_item_ptr != NULL)
        {
          /* Indicate that the file attributes are no longer cached */
          select_rsp_cache_item_ptr->is_cached = FALSE;

          /* If response cache has been allocated */
          if (select_rsp_cache_item_ptr->data != NULL)
          {
            /* Free the cache */
            uim_free(select_rsp_cache_item_ptr->data);
            select_rsp_cache_item_ptr->data = NULL;
          }
        }

        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_REHABILITATE_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting UIM Rehabilitate", 0, 0, 0 );
        }
      } /* end case - UIM_REHABILITATE_F */
      break;

    case UIM_VERIFY_CHV_F:              /* Verify CHV */
      {
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_VERIFY_CHV_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          if (rsp_ptr->cmd_status != UIM_DONE)
          {
             /* Set the status to UIM_FAIL since something failed */
             uim_rpt_buf.rpt_status = UIM_FAIL;
          }

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting UIM Verify CHV/PIN", 0, 0, 0 );
        }
      } /* end case - UIM_VERIFY_CHV_F */
      break;

    case UIM_CHANGE_CHV_F:              /* Change CHV */
      {
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_CHANGE_CHV_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          if (rsp_ptr->cmd_status != UIM_DONE)
          {
             /* Set the status to UIM_FAIL since something failed */
             uim_rpt_buf.rpt_status = UIM_FAIL;
          }

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting UIM Change CHV/PIN", 0, 0, 0 );
        }
      } /* end case - UIM_CHANGE_CHV_F */
      break;

    case UIM_DISABLE_CHV_F:             /* Disable CHV */
      {
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_DISABLE_CHV_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          if (rsp_ptr->cmd_status != UIM_DONE)
          {
             /* Set the status to UIM_FAIL since something failed */
             uim_rpt_buf.rpt_status = UIM_FAIL;
          }

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting UIM Disable CHV/PIN", 0, 0, 0 );
        }
      } /* end case - UIM_DISABLE_CHV_F */
      break;

    case UIM_ENABLE_CHV_F:              /* Enable CHV */
      {
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_ENABLE_CHV_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          if (rsp_ptr->cmd_status != UIM_DONE)
          {
             /* Set the status to UIM_FAIL since something failed */
             uim_rpt_buf.rpt_status = UIM_FAIL;
          }

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting UIM Enable CHV/PIN", 0, 0, 0 );
        }
      } /* end case - UIM_ENABLE_CHV_F */
      break;

    case UIM_UNBLOCK_CHV_F:             /* Unblock CHV */
      {
        /* Report back the CHV unblock result, if requested */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_UNBLOCK_CHV_R;

          if (rsp_ptr->cmd_status != UIM_DONE)
          {
            /* Set the status as FAIL if the command has not succeeded */
            uim_rpt_buf.rpt_status = UIM_FAIL;
          }

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to the Requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting CHV Unblock", 0, 0, 0 );
        } /* end if - is the report function defined */
      } /* end case - UIM_UNBLOCK_CHV_F */
      break;

    case UIM_STATUS_F:                  /* Status of the Card */
      {
        /* Report back the status as success */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_STATUS_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Fill in the number of bytes in the response that we got
            from UIM into the NV command */
          uim_rpt_buf.rpt.status.num_bytes = rsp_ptr->cmd_rsp_size;

          /* copy the data into the buffer from NV */
          memcpy(uim_rpt_buf.rpt.status.data, rsp_ptr->rsp.data,
                 rsp_ptr->cmd_rsp_size);

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting result for Status ", 0, 0, 0 );
        } /* end if - is the report function defined. */
      } /* end case - UIM_STATUS_F */
      break;

#ifdef FEATURE_UIM_TOOLKIT

    case UIM_TERMINAL_PROFILE_F:        /* Send a terminal profile */
      {
        /* Report back the status as success */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_TERMINAL_PROFILE_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting result for Terminal Profile", 0, 0, 0 );
        } /* end if - is the report function defined. */
      } /* end case - UIM_TERMINAL_PROFILE_F */
      break;

    case UIM_ENVELOPE_F:                /* Send an envelope command */
      {
        /* Report back the status as success */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_ENVELOPE_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Fill in the number of bytes in the response that we got
            from UIM into the NV command */
          uim_rpt_buf.rpt.envelope.data_length = rsp_ptr->cmd_rsp_size;

          /* copy the data into the buffer from NV */
          memcpy(uim_rpt_buf.rpt.envelope.data, rsp_ptr->rsp.data,
                 rsp_ptr->cmd_rsp_size);

          /* copy the intermediate sw1 and sw2 */
          uim_rpt_buf.rpt.envelope.get_resp_sw1 = uim_get_resp_sw1;
          uim_rpt_buf.rpt.envelope.get_resp_sw2 = uim_get_resp_sw2;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting result for Envelope", 0, 0, 0 );
        } /* end if - is the report function defined. */
      } /* end case - UIM_ENVELOPE_F */
      break;

    case UIM_INTERNAL_FETCH_F:          /* Fetch a proactive command */
      {
#ifdef FEATURE_GSTK
        /* Pass raw data to GSTK */
        (void) gstk_send_proactive_cmd(rsp_ptr->cmd_rsp_size,rsp_ptr->rsp.data);
#elif defined (FEATURE_UIM_TOOLKIT_UTK)
        /* The Fetch command is handled differently than other commands.
           The response to a Fetch command is a proactive command.  */
        uim_tk_process_proactive_command( rsp_ptr );
#endif /* FEATURE_UIM_TOOLKIT_UTK */

      } /* end case - UIM_INTERNAL_FETCH_F */
      break;

    case UIM_TERMINAL_RESPONSE_F:       /* Send a proactive cmd response */
      {
        /* Report back the status as success */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_TERMINAL_RESPONSE_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting result for Terminal Response", 0, 0, 0 );
        } /* end if - is the report function defined. */
      } /* end case - UIM_TERMINAL_RESPONSE_F */
      break;

    case UIM_POLLING_INTERVAL_F:
      {
        /* Report back the status as success */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_POLLING_INTERVAL_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting result for TK Polling", 0, 0, 0 );
        } /* end if - is the report function defined. */
      } /* end case - UIM_POLLING_INTERVAL_F */
      break;

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
    case UIM_RESET_WITH_TP_F:    /* Switch cards for the toolkit session. */
      {
        /* If a TP was not downloaded, then reset the toolkit slot */
        if (!cmd_ptr->reset_with_tp.terminal_profile)
        {
          uim_toolkit_slot = UIM_SLOT_NONE;
          uim_polling_slot = UIM_SLOT_NONE;
        }

        /* Report back the status as success */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_RESET_WITH_TP_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          if ( uim_mode == UIM_PASSIVE_MODE )
          {
            /* Flush all the existing commands to empty the queue
            */
            uim_flush_command(UIM_FLUSH_ALL);

            /* Make sure that we have switched the UIM mode to normal */
            uim_mode = UIM_NORMAL_MODE;

            /* Remove the requirement for presence detection in passive mode */
            uim_ctl = (uim_voter_type) ( (int)uim_ctl & ~((int)UIM_PASSIVE) );
          }

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting result for Reset with Terminal Profile",0, 0, 0 );
        } /* end if - is the report function defined. */
      } /* end case - UIM_RESET_WITH_TP_F */
      break;
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
#endif /* FEATURE_UIM_TOOLKIT */

    case UIM_RESET_F:
      {
        /* Report back the status as success */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_RESET_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
          /* Fill in the slot information for the reset */
          uim_rpt_buf.rpt.reset.reset_slot = cmd_ptr->hdr.slot;
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

          if ( uim_mode == UIM_PASSIVE_MODE )
          {
            /* Flush all the existing commands to empty the queue
            */
            uim_flush_command(UIM_FLUSH_ALL);

            /* Make sure that we have switched the UIM mode to normal mode */
            uim_mode = UIM_NORMAL_MODE;

            /* Remove the requirement for presence detection in passive mode */
            uim_ctl = (uim_voter_type) ( ((int)uim_ctl) & ~((int)UIM_PASSIVE) );
          }

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting result for Reset", 0, 0, 0 );
        } /* end if - is the report function defined. */
      } /* end case - UIM_RESET_F */
      break;
  case UIM_POWER_UP_UIM_PASSIVE_F:
  case UIM_RESET_SWITCH_UIM_PASSIVE_F:
    {
      /* Report back the status as success */
      if (cmd_ptr->hdr.rpt_function != NULL)
      {
        if (cmd_ptr->hdr.command == UIM_POWER_UP_UIM_PASSIVE_F)
        {
          uim_rpt_buf.rpt_type = UIM_POWER_UP_UIM_PASSIVE_R;
        }
        else
        {
          uim_rpt_buf.rpt_type = UIM_RESET_SWITCH_UIM_PASSIVE_R;
        }

        /* Fill in necessary fields of command block */
        uim_rpt_buf.rpt_hdr.task_ptr = NULL;

        uim_rpt_buf.rpt.rst_uim_passive.num_bytes = uim_atr_buf[uim_drv_slot].num_bytes;
        memcpy(uim_rpt_buf.rpt.rst_uim_passive.data, uim_atr_buf[uim_drv_slot].data,
               uim_atr_buf[uim_drv_slot].num_bytes);

        /* Make sure that we have switched the UIM mode to passive */
        uim_mode = UIM_PASSIVE_MODE;

        /* Set clock stop allowed in passive mode flag to FALSE */
        uim_passive_clk_stop_allowed = FALSE;

        /* Flush all the existing commands to empty the queue
        */
        uim_flush_command(UIM_FLUSH_ALL);

        /* Report to requesting task */
        (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
        MSG_MED( "Reporting result for Reset to make UIM passive",0, 0, 0 );
      } /* end if - is the report function defined. */
    }
    break;
  case UIM_POWER_DOWN_F:
    {
      uim_rpt_buf.rpt_type = UIM_POWER_DOWN_R;
      uim_rpt_buf.rpt_hdr.task_ptr = NULL;

      /* Switch the mode back to the saved previous mode */
      uim_mode = uim_save_mode;

      /* Flush all the existing commands to empty the queue
      */
      uim_flush_command(UIM_FLUSH_ALL);

      /* Report to requesting task */
      (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
        uim_reset_dir();
        /* set the status to ERROR */
        uim_status = UIM_ERR_S;
#else
        if (cmd_ptr->hdr.slot == UIM_SLOT_1)
        {
          uim_status = UIM_ERR_S;
        }
        else if (cmd_ptr->hdr.slot == UIM_SLOT_2)
        {
          uim_status_2 = UIM_ERR_S;
        }
        uim_reset_dir_for_slot(cmd_ptr->hdr.slot);
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

        /* Notify GSDI about the card removal if the optionm is set */
        if (cmd_ptr->pdown.pdown_option == UIM_OPTION_NOTIFY_PDOWN)
        {
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
          uim_notify_error(UIM_POWER_DOWN_CMD_NOTIFICATION_S);
#else
          uim_notify_error(UIM_ERR_S, cmd_ptr->hdr.slot);
#endif
        }

      MSG_ERROR( "Reporting result for Power Down",0, 0, 0 );
    }
    break;

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS

    case UIM_PREF_SLOT_CHANGE_F:
      {
        /* Report back the status as success */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_PREF_SLOT_CHANGE_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting result for Pref Slot change", 0, 0, 0 );
        }
      }
      break;

#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

    case UIM_STREAM_APDU_F:            /* Stream APDU to the card */
      {
        /* Report back the status as success */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_STREAM_APDU_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting result for Stream APDU", 0, 0, 0 );
        } /* end if - is the report function defined. */
      } /* end case - UIM_STREAM_APDU_F */
      break;

    case UIM_STREAM_ISO7816_APDU_F:            /* Stream APDU to the card */
      {
        /* Report back the status as success */
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_STREAM_ISO7816_APDU_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          uim_rpt_buf.rpt.stream_iso7816_apdu.num_data_bytes = 0;

#ifndef FEATURE_UIM_SUPPORT_CSIM
          /* Copy the response back for Case 2 */
          if ((uim_req_buf.instrn_case == UIM_INSTRN_CASE_2) ||
              (uim_req_buf.instrn_case == UIM_INSTRN_CASE_4))
#else
          if (rsp_ptr->cmd_rsp_size > 0)
#endif /* FEATURE_UIM_SUPPORT_CSIM */
          {
            /* copy the data into report buffer */
            uim_rpt_buf.rpt.stream_iso7816_apdu.num_data_bytes =
                                                   rsp_ptr->cmd_rsp_size;

            memcpy(uim_rpt_buf.rpt.stream_iso7816_apdu.data, rsp_ptr->rsp.data,
                   rsp_ptr->cmd_rsp_size);

            uim_rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw1 = uim_get_resp_sw1;
            uim_rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw2 = uim_get_resp_sw2;
          }
          else
          {
            uim_rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw1 = (uim_sw1_type) 0;
            uim_rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw2 = (uim_sw2_type) 0;
          }
#ifdef FEATURE_UIM_SUPPORT_CSIM
          if ( rsp_ptr->cmd_status == UIM_DONE )
          {
            /* Update the directory structures when the command is select cmd
               in default channel (CSIM command) */
            if ((uim_req_buf.apdu_hdr.instrn == SELECT) &&
                (uim_req_buf.apdu_hdr.uim_class == UIM_INSTRUCTION_CLASS) &&
                !(uim_mode == UIM_PASSIVE_MODE))
            {
              path = (word)FLIPW(uim_req_buf.req.data);

              (void)uim_update_last_sel_dir(rsp_ptr,
#ifdef FEATURE_UIM_UICC
                                            cmd_ptr,
#endif /* FEATURE_UIM_UICC */
                                              path);

              /* Copy the contents into CSIM structure */
              uim_csim_last_sel_dir[uim_drv_slot] =
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot];
            }
#ifdef FEATURE_UIM_UICC
            else if ( (uim_req_buf.apdu_hdr.instrn    == SELECT) &&
                      (uim_req_buf.apdu_hdr.uim_class ==
                       UIM_UICC_INSTRUCTION_CLASS1) &&
                      !(uim_mode == UIM_PASSIVE_MODE)
                    )
            {
              /* Get the path out from the apdu here */
              if ( uim_req_buf.apdu_hdr.p1 & UIM_USIM_SELECT_BY_PATH_FROM_MF )
              {
                /* First update with MF */
                (void) uim_update_last_sel_dir( rsp_ptr,
                                                cmd_ptr,
                                                UIM_MF_ID);

                /* This is a select by path from MF */
                path_ctr = UIM_PATH_TO_SEL_MF_INDEX;

                /* This is a select by path */
                while ( path_ctr < (uim_req_buf.apdu_hdr.p3/2) && (path_ctr < 4) )
                {
                  /* The path item is */
                  path = (word)FLIPW ( uim_req_buf.req.data + (2 * path_ctr) );

                  /* This is a hack since the numbering of the Orange DF is not
                     compliant with spec. Without this it completely breaks the
                     directory maintenance. Second level DF should not start with
                     7FXX */
                  if (path == UIM_ORANGE_DF)
                  {
                    /* Do not go through the update function and instead set the
                       structure here as ADF info would be overwritten. Save the last
                       selected 2nd level DF */
                    uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 = UIM_ORANGE_DF;
                    uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef  = UIM_NO_FILE_SEL;
                    uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = FALSE;

                    /* Reset the data in the last selected EF structure */
                    uim_reset_last_sel_ef_data();

                    path_ctr++;
                  }
#ifdef FEATURE_ENHANCED_NW_SELECTION
                  else if (path == UIM_CINGULAR_DF)
                  {
                    /* Do not go through the update function and instead set the
                       structure here as ADF info would be overwritten. Save the last
                       selected 2nd level DF */
                    uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 = UIM_CINGULAR_DF;
                    uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef  = UIM_NO_FILE_SEL;
                    uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = FALSE;

                    /* Reset the data in the last selected EF structure */
                    uim_reset_last_sel_ef_data();

                    path_ctr++;
                  }
#endif
                  else
                  {
                    (void) uim_update_last_sel_dir( rsp_ptr,
                                                    cmd_ptr,
                                                    path);
                    /* Increment to the next path item */
                    path_ctr++;
                  }
                } /*while*/
              }
              else
              {
                /* This is a select by file-id */
                path = (word)FLIPW (uim_req_buf.req.data);
                /* Update the last selected dir structure */
                (void) uim_update_last_sel_dir( rsp_ptr,
                                                cmd_ptr,
                                                path);
              }

              /* Copy the contents into CSIM structure */
              uim_csim_last_sel_dir[uim_drv_slot] =
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot];

            } /* UICC instructoin class - select */
#endif /*FEATURE_UIM_UICC*/
          } /* Result was done */
#endif /* FEATURE_UIM_SUPPORT_CSIM */

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
          MSG_MED( "Reporting result for Extended Stream APDU", 0, 0, 0 );
        } /* end if - is the report function defined. */
      } /* end case - UIM_STREAM_ISO7816_APDU_F */
    break;

    case UIM_CLEAR_CACHE_F:
      {

        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_CLEAR_CACHE_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          if ( rsp_ptr->cmd_status != UIM_DONE )
          {
            /* Set the status to UIM_PASS unless something failed */
            uim_rpt_buf.rpt_status = UIM_FAIL;
            MSG_MED( "Reporting failure for Clear Cache", 0, 0, 0);
          }
          else
          {
            MSG_MED( "Reporting success for CLEAR CACHE", 0, 0, 0);
          }

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
        } /* if report function is not null */
      } /* case UIM_CLEAR_CACHE_F */
    break;

    case UIM_RECOVERY_COMPLETE_F:
      {
        MSG_HIGH("Reporting response for RECOVERY_COMPLETE_F", 0, 0, 0);
        if (cmd_ptr->hdr.rpt_function != NULL)
        {
          uim_rpt_buf.rpt_type = UIM_RECOVER_COMPLETE_R;

          /* Fill in necessary fields of command block */
          uim_rpt_buf.rpt_hdr.task_ptr = NULL;

          if ( rsp_ptr->cmd_status != UIM_DONE )
          {
            /* Set the status to UIM_PASS unless something failed */
            uim_rpt_buf.rpt_status = UIM_FAIL;
            MSG_MED( "Reporting failure for RECOVERY_COMPLETE ", 0, 0, 0);
          }
          else
          {
            MSG_MED( "Reporting success for RECOVERY_COMPLETE ", 0, 0, 0);
          }

          /* Report to requesting task */
          (*(cmd_ptr->hdr.rpt_function))( &uim_rpt_buf );
        } /* if report function is not null */
      } /* case UIM_CLEAR_CACHE_F */
    break;
    default:
      {
        uim_custom_process_generic_response(rsp_ptr,cmd_ptr);
      } /* end case - default */
      break;

  } /* end switch - cmd_ptr->hdr.command */

  /* return buffer to free queue */
  if (save_done_q_ptr != NULL)
  {
     /* place command on requested queue */
     q_put( save_done_q_ptr, save_link_ptr );
  } /* end if - save_done_q_ptr != NULL */
} /* uim_process_generic_response */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GENERIC_COMMAND

DESCRIPTION
  This procedure is the main state machine for Generic UIM command processing.

DEPENDENCIES
  This function uses the UIM generic state pointer to determine what to process.

RETURN VALUE
  None

SIDE EFFECTS
  Sends a command to the UIM.

===========================================================================*/

void uim_generic_command
(
  uim_cmd_type *cmd_ptr  /* Pointer to the received command */
)
{
  word path;  /* File path */
  byte FI, DI; /* Transmission factors from ATR */
  word item_ctr;
#ifdef FEATURE_UIM_UICC
  byte i,j = 0;
#endif

  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_generic_command cmd_ptr is NULL",0,0,0);
  }
  /* Set the protocol from the command to the UIM request buffer */
  uim_req_buf_ptr->protocol = cmd_ptr->hdr.protocol;

  /* Build an APDU based on the UIM generic state */
  switch(*uim_generic_state_ptr)
  {
    case UIM_POWER_DOWN_ST:
      {
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
        {
          MSG_ERROR("Turning off UIM because of POWER down cmd",0,0,0);

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
          uim_power_down();
#else
          uim_power_down(cmd_ptr->hdr.slot);
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
          /* Nothing is to be done set the response signal */
          (void) rex_set_timer(&uim_delay_timer,0);
        }
          /* Set the response command status as done */
          uim_rsp_buf_ptr->cmd_status = UIM_DONE;
      }
      break;

    case UIM_POWER_UP_ST:                     /* Power Up state */
      {
#ifdef FEATURE_UIM_HANDLE_TA1_0X97_ATR
        /* Initialize the warm reset count to zero for each powerup */
        warm_reset_count = 0;
#endif /* FEATURE_UIM_HANDLE_TA1_0X97_ATR */
        uim_power_up(
#ifndef FEATURE_UIM_IGNORE_UNSOLICITED_BYTES
          *(uim_unsolicited_rsp_callback)
#endif /* FEATURE_UIM_IGNORE_UNSOLICITED_BYTES */
        );
#if defined( FEATURE_UIM_T_1_SUPPORT )
        /* Set the current protocol to T=0 during power up */
        uim_current_protocol = UIM_T_0_PROTOCOL;
#endif /* FEATURE_UIM_T_1_SUPPORT */
        /* Signal is set internally for proper operation of state machine */

        /* This intentional wait is to delay the reset by 20ms. The delay
           timer is associated with the UIM_CMD_RSP_SIG. */
#ifndef TEST_FRAMEWORK
#ifndef FEATURE_UIM_INTERFACE_NOT_USED
        (void)rex_timed_wait(UIM_CMD_RSP_SIG, &uim_delay_timer,
                       UIM_CARD_DELAY_TIMEOUT);
#endif /* FEATURE_UIM_INTERFACE_NOT_USED */
#endif/* !TEST_FRAMEWORK */
#ifdef FEATURE_UIM_UICC
        /* If this is the first time we are executing this command and if the
           uim_first_instn_class is set to GSM, then toggle the flag */
        if (cmd_ptr->hdr.cmd_count == 1)
        {
          if (uim_first_inst_class == NV_UIM_FIRST_INST_CLASS_GSM_SIM)
          {
            uim_toggle_instrn_class = TRUE;
          }
        }
#endif  /* FEATURE_UIM_UICC */

      } /* end case - UIM_POWER_UP_ST */
      break;

    case UIM_RESET_ST:                        /* Reset State */
      {
        /* Indicate that we have not yet received an ATR */
        uim_atr_received = FALSE;
        /* Set the initial work waiting time */
        /* Compute a new work waiting time */
        uim_work_waiting_time[uim_drv_slot] = ((960 * 10) *
                              crcf_values[UIM_CRCF_372_1]) /
                             (uim_clk_freq[uim_drv_slot] / 1000) + CLK_MS_PER_TICK;
#ifdef UIM_DRIVER_TIMESTAMP
        uim_work_waiting_time_sleep_ticks[uim_drv_slot] =
          ( ((uim_work_waiting_time[uim_drv_slot] - CLK_MS_PER_TICK) *
             SLEEP_COUNTER_FREQ) / 1000 );
#endif

#if defined(FEATURE_UIM_T_1_SUPPORT)
        /* for T=1 protocol, Block Guard Time is 22 Etu's */
        uim_t_1_bgt = (BGT_22_ETU * crcf_values[UIM_CRCF_372_1]) /
                       (uim_clk_freq[uim_drv_slot] / 1000) + CLK_MS_PER_TICK;
#endif
        /* Set the flag to FALSE */
        uim_atr_pps_done= FALSE;
        /* Do not use the minimum guardtime.  This is determined in the
           ATR */
        uim_use_minimum_guardtime_size = FALSE;

        /* Reset the UIM card */
        uim_reset( uim_rsp_buf_ptr, *(uim_command_response_callback));

        /* Fill out the path to select the directory for determining clock
           stop preferences.  This is used by the UIM_SELECT_ST after reset,
           UIM_PPS_ST and UIM_UPDATE_OP_PARAMS_ST.  */

#ifdef FEATURE_UIM_UICC
        if ( !uim_toggle_instrn_class )
        {
          /* Set the protocol to UICC */
          cmd_ptr->hdr.protocol = UIM_UICC;
          uim_path_to_be_sel[0] = UIM_MF_ID;
          uim_path_to_be_sel[1] = UIM_NO_FILE_SEL;

#if defined (FEATURE_UIM_SUPPORT_DUAL_SLOTS)
          /* !!!!!!!!!! THIS IS UNTESTED CASE !!!!!!!!!!*/

          if (cmd_ptr->hdr.slot == UIM_SLOT_1)
          {
            /* Save the protocol as UICC */
            uim_dfs_1.protocol = UIM_UICC;
          }
          else if (cmd_ptr->hdr.slot == UIM_SLOT_2)
          {
            /* Save the protocol as UICC */
            uim_dfs_2.protocol = UIM_UICC;
          }
#else
          /* Save the protocol as UICC */
          uim_dfs_1.protocol = UIM_UICC;
#endif
        }
        else
#endif
        {
          /* Set the protocol to GSM 11.11 */
          cmd_ptr->hdr.protocol = UIM_GSM;

#if defined (FEATURE_UIM_SUPPORT_DUAL_SLOTS)
          if (cmd_ptr->hdr.slot == UIM_SLOT_1)
          {
            /* Save the protocol as GSM */
            uim_dfs_1.protocol = UIM_GSM;
          }
          else if (cmd_ptr->hdr.slot == UIM_SLOT_2)
          {
            /* Save the protocol as GSM */
            uim_dfs_2.protocol = UIM_GSM;
          }
#else
          /* Save the protocol as GSM */
          uim_dfs_1.protocol = UIM_GSM;
#endif

          /* Reset the select state counter */
          select_state_ctr = 0;
#if defined ( FEATURE_UIM_RUIM ) && defined(FEATURE_UIM_AUTH_CDMA_DF_FIRST)
          uim_path_to_be_sel[select_state_ctr++] = UIM_CDMA_DF;
#endif /* FEATURE_UIM_RUIM  AND FEATURE_UIM_AUTH_CDMA_DF_FIRST */
#if defined ( FEATURE_UIM_GSM )
#error code not present
#endif /* FEATURE_UIM_GSM */
#if defined ( FEATURE_UIM_GSM_DCS_1800 )
#error code not present
#endif /* FEATURE_UIM_GSM_DCS_1800 */
#if defined ( FEATURE_UIM_RUIM ) && !defined(FEATURE_UIM_AUTH_CDMA_DF_FIRST)
          uim_path_to_be_sel[select_state_ctr++] = UIM_CDMA_DF;
#endif /* FEATURE_UIM_RUIM AND NOT FEATURE_UIM_AUTH_CDMA_DF_FIRST */
#if !defined ( FEATURE_UIM_GSM ) && !defined ( FEATURE_UIM_GSM_DCS_1800 ) && \
    !defined ( FEATURE_UIM_RUIM )
          /* SIM technology is not defined.  Use MF as default */
          uim_path_to_be_sel[select_state_ctr++] = UIM_MF_ID;
#endif /* Pick the DF to check for file characteristics. */
          /* Indicate the end of the list of DFs to search. */
          uim_path_to_be_sel[select_state_ctr] = UIM_NO_FILE_SEL;
          /* Reset the select state control to start selecting from the first
           DF */
          select_state_ctr = 0;
        }
      } /* end case - UIM_RESET_ST */
      break;
    case UIM_DELAY_AFTER_ATR_ST:              /* Introduce delay after ATR */
      {
        /* This intentional wait is to delay sending the PPS request after
         * the reception of the ATR.  To avoid sending the first PPS byte
         * within 12 ETU of the last ATR byte received.  This could happen
         * based on when the last byte is received.. if the byte is already
         * present in the FIFO by the time the ISR is reading bytes out
         */
#ifndef TEST_FRAMEWORK
        (void)rex_timed_wait(UIM_CMD_RSP_SIG, &uim_delay_timer,
                       UIM_DELAY_TIME_AFTER_ATR);
#else
         (void)rex_set_sigs(&uim_tcb,UIM_CMD_RSP_SIG);
#endif/* !TEST_FRAMEWORK */
      }
      break;
    case UIM_PPS_ST:                          /* PPS State */
      {
        /* Fill out the PPS request buffer. */
        uim_pps_req_buf_ptr->pps_resp_callback = uim_command_response_callback;

        /* This uses TA(1) from the ATR to negotiate the PPS. */
        uim_pps_char_buf[0] = UIM_PPS_DEFAULT_PPSS;
        /* Separate FI from DI */
        FI = uim_rsp_buf_ptr->rsp.data[UIM_TA1_INDEX] >> UIM_FI_SHIFT_OF_TA1;
        DI = uim_rsp_buf_ptr->rsp.data[UIM_TA1_INDEX] & UIM_DI_MASK_OF_TA1;
        /* Check if TA1 is valid */
        if ((braf_values[DI] == 0) || (crcf_values[FI] == 0))
        {
          uim_pps_req_buf_ptr->size_of_pps_request = UIM_PPS_DEFAULT_REQ_SIZE;
          uim_pps_char_buf[1] = UIM_PPS_DEFAULT_PPS0;
          uim_pps_char_buf[2] = UIM_PPS_DEFAULT_PCK;
        }
        else
        {
          /* If we are in the third reset attempt */
          if (cmd_ptr->hdr.cmd_count > (UIM_MAX_NUM_ATTEMPTS - 1))
          {
            uim_pps_req_buf_ptr->size_of_pps_request = UIM_PPS_DEFAULT_REQ_SIZE;
            uim_pps_char_buf[1] = UIM_PPS_DEFAULT_PPS0;
            uim_pps_char_buf[2] = UIM_PPS_DEFAULT_PCK;
          }
          else
          {
            uim_pps_req_buf_ptr->size_of_pps_request = UIM_PPS_REQ_SIZE;
            uim_pps_char_buf[1] = (int)UIM_PPS_PPS1_PPS0
#if defined( FEATURE_UIM_T_1_SUPPORT )
               | (int)uim_operating_protocol
#endif /* FEATURE_UIM_T_1_SUPPORT */
              ;
            uim_pps_char_buf[2] = uim_rsp_buf_ptr->rsp.data[UIM_TA1_INDEX];
            uim_pps_char_buf[3] =
              (uim_pps_char_buf[0] ^ uim_pps_char_buf[1] ^ uim_pps_char_buf[2]);
          }
        }
        uim_pps_req_buf_ptr->request_buf = uim_pps_char_buf;
        uim_pps_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;
        uim_send_pps(uim_pps_req_buf_ptr);

      } /* end case - UIM_PPS_ST */
      break;

    case UIM_UPDATE_OP_PARAMS_ST:  /* Update Operational Parameters State */
      {
        /* Fill out the operational parameters as default values */
        uim_op_params_ptr->change_baud_rate = TRUE;
        uim_op_params_ptr->change_clk_freq = TRUE;
        uim_op_params_ptr->change_guardtime = TRUE;
        uim_op_params_ptr->FI = UIM_CRCF_372_1;
        uim_op_params_ptr->DI = UIM_BRAF_1;
        /* Use the maximum frequency allowed by default for 3V UIMs */
        uim_op_params_ptr->clock_frequency =
          uim_max_freq_supported[uim_drv_slot];
        /* Determine if there is an interface byte to process.  This code
           operates on the ATR as well as the PPS response. */
        if (uim_rsp_buf_ptr->rsp.data[UIM_ATR_T0_BYTE] & (UIM_ATR_TA_PRESENT))
        {
          /* Set the op parameters from the ATR/PPS response */
          uim_op_params_ptr->FI = (uim_clock_rate_conversion_factor_type)
            (uim_rsp_buf_ptr->rsp.data[UIM_TA1_INDEX] >> UIM_FI_SHIFT_OF_TA1);
          uim_op_params_ptr->DI = (uim_baud_rate_adjustment_factor_type)
            (uim_rsp_buf_ptr->rsp.data[UIM_TA1_INDEX] & UIM_DI_MASK_OF_TA1);
        } /* end if - TA(1) does exist in the ATR/PPS. */
        /* Use the default guardtime size. */
        uim_op_params_ptr->guardtime_bits = UIM_DEFLT_GUARDTIME_BITS;
#if defined( FEATURE_UIM_T_1_SUPPORT )
        /* Set the waiting times for T=1 protocol */
        uim_t_1_bwt = ((11 + ((1 << uim_t_1_bwi) * 960)) *
                       crcf_values[UIM_CRCF_372_1]) /
                       (uim_clk_freq[uim_drv_slot] / 1000) + CLK_MS_PER_TICK;
        uim_t_1_cwt = ((11 + (1 << uim_t_1_cwi)) *
                       crcf_values[uim_op_params_ptr->FI]) /
                       ((uim_clk_freq[uim_drv_slot] / 1000) *
                        braf_values[uim_op_params_ptr->DI]) + CLK_MS_PER_TICK;
        /* for T=1 protocol, Block Guard Time is 22 Etu's */
        uim_t_1_bgt = (BGT_22_ETU * crcf_values[uim_op_params_ptr->FI]) /
                       ((uim_clk_freq[uim_drv_slot] / 1000) *
                        braf_values[uim_op_params_ptr->DI]) + CLK_MS_PER_TICK;
        /* Determine if the guard bit time in the minimum for T=1 */
        if ((UIM_T_1_PROTOCOL == uim_operating_protocol) &&
            (uim_use_minimum_guardtime_size))
        {
          /* Use the default guardtime size. */
          uim_op_params_ptr->guardtime_bits = UIM_MINIMUM_T_1_GUARDTIME;
        }
#endif /* FEATURE_UIM_T_1_SUPPORT */
        /* Send the operational parameters */
        uim_update_op_params( uim_op_params_ptr, uim_drv_slot );

        /* Compute a new work waiting time */
        uim_work_waiting_time[uim_drv_slot] = ((960 * uim_WI[uim_drv_slot]) *
                              crcf_values[uim_op_params_ptr->FI]) /
                             (uim_clk_freq[uim_drv_slot] / 1000) + CLK_MS_PER_TICK;

        /* Extend work waiting time for some out of spec RUIM cards */
        uim_original_work_waiting_time[uim_drv_slot] =
                                        uim_work_waiting_time[uim_drv_slot];
        uim_extended_work_waiting_time[uim_drv_slot] =
                                    2 * uim_work_waiting_time[uim_drv_slot];

#ifdef UIM_DRIVER_TIMESTAMP
        uim_work_waiting_time_sleep_ticks[uim_drv_slot] =
                ( ((uim_work_waiting_time[uim_drv_slot] - CLK_MS_PER_TICK) *
                                               SLEEP_COUNTER_FREQ) / 1000 );
#endif

        /* Signal is set internally for proper operation of state machine */
#ifndef TEST_FRAMEWORK
        (void)rex_timed_wait(UIM_CMD_RSP_SIG, &uim_delay_timer,
                       UIM_CARD_DELAY_TIMEOUT);
#else
         (void)rex_set_sigs(&uim_tcb,UIM_CMD_RSP_SIG);
#endif/* !TEST_FRAMEWORK */

      } /* end case - UIM_UPDATE_OP_PARAMS_ST */
      break;

#if defined( FEATURE_UIM_T_1_SUPPORT )
    case UIM_IFS_NEG_ST:
      {
        /* This command does not use these parameters.  Set them to a benign
           command, just in case */
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_IFS_BLOCK;

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        /* This is the IFS parameter that indicates the maximum number of bytes
           the MSM can accept in a block */
        uim_req_buf_ptr->req.data[0] =
          int32touint8(UIM_MAX_TRANSACTION_SIZE -
                       (UIM_T_1_NUM_BYTES_PROLOGUE + uim_epilogue_field_size));

        uim_send_command(uim_req_buf_ptr );
      } /* end case - UIM_IFS_NEG_ST */
      break;
#endif /* FEATURE_UIM_T_1_SUPPORT */

     /* For now, we are assuming that reads and writes to the UIM are
       not going to exceed 255 bytes */

    case UIM_READ_ICCID_ST:
      {
#ifdef FEATURE_UIM_SUPPORT_NO_ICCID
        /* Skip read iccid if select failed. */
        if (uim_iccid_select_failed)
        {
          uim_iccid_select_failed = FALSE;

          uim_generic_state_ptr++;
  #ifdef FEATURE_UIM_UICC
          if ((cmd_ptr->hdr.command == UIM_INTERNAL_WAKE_UP_F) &&
              (cmd_ptr->hdr.protocol == UIM_UICC))
          {
              /* Proceed past the VERIFY_CHV1_ST. */
              uim_generic_state_ptr++;
          }
  #endif
          /* go to process next command */
          uim_generic_command (cmd_ptr);
          break;
        }
#endif /* FEATURE_UIM_SUPPORT_NO_ICCID */


        uim_req_buf_ptr->apdu_hdr.instrn = READ_BINARY;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_2;
        uim_req_buf_ptr->apdu_hdr.p1 = UIM_ICCID_OFFSET;
        uim_req_buf_ptr->apdu_hdr.p2 = UIM_ICCID_OFFSET;
        uim_req_buf_ptr->apdu_hdr.p3 = UIM_ICCID_SIZE;

        /* Specifically select the ICCID EF. */
#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr );

      } /* end case - UIM_READ_ICCID_ST */
      break;

#if defined( FEATURE_UIM_RUIM ) || \
    defined (FEATURE_UIM_SUPPORT_DUAL_SLOTS)
    /* Try to select the CDMA DF.  Use this for RTRE */
    case UIM_CHECK_FOR_CDMA_DF:
      {
        /* Reset the select state counter */
        select_state_ctr = 0;
        uim_path_to_be_sel[select_state_ctr++] = UIM_CDMA_DF;
        /* Indicate the end of the list of DFs to search. */
        uim_path_to_be_sel[select_state_ctr] = UIM_NO_FILE_SEL;
        /* Reset the select state control to start selecting from the first
         DF */
        select_state_ctr = 0;
      } /* Fall through the the select processing */
      //lint -fallthrough
#endif /* FEATURE_UIM_RUIM || DUAL SLOTS */

    /* Select a DF to determine characteristics. The processing for this state
       is the same as UIM_SELECT_ST since the characteristics are in the
       response of selecting a DF. */
    case UIM_CHECK_CHARACTERISTICS_ST:
    case UIM_SELECT_ST:                       /* Select State */
      {

        /* If the command is a UIM_CACHED_SEL_F and we already have the item
         * cached, do not send the command to the driver, instead, fake
         * a response.  The cache would be retrieved when the
         * faked response is received in the generic_command_response func
         */
        if ( cmd_ptr->hdr.command == UIM_CACHED_SEL_F &&
             uim_item_sel_rsp_cached && select_rsp_cache_item_ptr != NULL )
        {
          uim_req_buf_ptr->rsp_ptr->sw1        = (byte) SW1_NORMAL_END;
          uim_req_buf_ptr->rsp_ptr->sw2        = (byte) SW2_NORMAL_END;
          uim_req_buf_ptr->rsp_ptr->cmd_status = UIM_DONE;

#if defined( FEATURE_UIM_T_1_SUPPORT )
          uim_t_1_protocol_state = UIM_T_1_CACHED_SEL_RSP_ST;
#endif /* FEATURE_UIM_T_1_SUPPORT */

          /* Set the response signal */
#ifndef TEST_FRAMEWORK
          (void)rex_timed_wait(UIM_CMD_RSP_SIG, &uim_delay_timer, 0);
#else
          (void)rex_set_sigs(&uim_tcb,UIM_CMD_RSP_SIG);
#endif/* !TEST_FRAMEWORK */

          break;
        }

        uim_intermediate_sel = FALSE;

#ifdef FEATURE_UIM_UICC
        if ( uim_sel_by_path_ref_from_mf )
        {
          /* This is a select by path so no worries of intermediate
           * selects and their get responses
           */
          uim_intermediate_sel = FALSE;
        }
        else
#endif
        if (select_state_ctr == UIM_MAX_PATH_ELEMENTS-1)
        {
          /* The following select whould be the last part of the select */
          uim_intermediate_sel = FALSE;
        }
        else if ( uim_path_to_be_sel[select_state_ctr+1] == UIM_NO_FILE_SEL )
        {
          /* The following select whould be the last part of the select */
          uim_intermediate_sel = FALSE;
        }
        else
        {
          /* This must be an intermediate select for which we do not
           * care about the get response data... ignore it
           * In check-char state, the path is not build traditionally like
           * x,y,no-file but is set as GSM, DCS, RUIM etc and a failure would
           * trigger the next item to be selected so do not consider it as
           * an intermediate during check-char state
           */
          if ( *uim_generic_state_ptr != UIM_CHECK_CHARACTERISTICS_ST )
          {
            uim_intermediate_sel = TRUE;
          }
        }

        /* Get the next DF/EF to select */
        path = uim_path_to_be_sel[select_state_ctr];

        uim_req_buf_ptr->apdu_hdr.instrn = SELECT;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;

          if (cmd_ptr->hdr.command == UIM_INTERNAL_WAKE_UP_F)
          {
            /* No select by path when internally power up */
            uim_sel_by_path_ref_from_mf = FALSE;
          }
          else if ((uim_path_to_be_sel[UIM_PATH_TO_SEL_MF_INDEX] ==
                    UIM_MF_ID ||
                    uim_path_to_be_sel[UIM_PATH_TO_SEL_MF_INDEX] ==
                    UIM_SEL_AID_BY_FILE_ID) &&
                   (uim_path_to_be_sel[UIM_PATH_TO_SEL_MF_INDEX + 1] !=
                      UIM_NO_FILE_SEL ) &&
                   (uim_path_to_be_sel[UIM_PATH_TO_SEL_MF_INDEX + 1] !=
                      UIM_SEL_ADF) &&
                   (select_state_ctr == 0) )
          {
            /* If MF is the first entry in the path followed by some other entry
             * that is not a selection by AID DF name , then we can use selection
             * by path reference
             */
           uim_sel_by_path_ref_from_mf = TRUE;
          }
          else
          {
            uim_sel_by_path_ref_from_mf = FALSE;
          }

          if ( uim_sel_by_path_ref_from_mf )
          {
            /* This is a select by path so no worries of intermediate
             * selects and their get responses
             */
            uim_intermediate_sel = FALSE;
          }

          if ( uim_intermediate_sel  ||
               ( uim_item_sel_rsp_cached &&
                 ( select_rsp_cache_item_ptr != NULL) ) ||
               uim_reselect_mf )
          {
            /* This select is either an intermediate select to get to the
             * final EF or could be the final select but for which the response
             * has already been cached.  We need not request the FCP template
             */
            uim_req_buf_ptr->apdu_hdr.p2 = UIM_USIM_RETURN_NO_DATA;
            uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
          }
          else
          {
            uim_req_buf_ptr->apdu_hdr.p2 = UIM_USIM_RETURN_FCP_TEMPLATE;
          }

          /* Check to see if Select by path referenceing is set */
          if (uim_sel_by_path_ref_from_mf)
          {
            /* Temporary variable to hold where in the path_to_be_sel should
             * we start extracting for path that needs to be sent out to the
             * card
             */
            byte start_index = 0;

            uim_req_buf_ptr->apdu_hdr.p1 = UIM_USIM_SELECT_BY_PATH_FROM_MF;

            /* conditionally initialize the index to 1 or 0 as we do not want
             * to include MF if the path starts with MF and we would want to
             * include 7FFF if path starts with 7FFF
             */

            if ( uim_path_to_be_sel[UIM_PATH_TO_SEL_MF_INDEX] ==
                 UIM_SEL_AID_BY_FILE_ID )
            {
              start_index = UIM_PATH_TO_SEL_MF_INDEX;
            }
            else
            {
              start_index = UIM_PATH_TO_SEL_MF_INDEX + 1;
            }

            /* Initialize the number of bytes to zero */
            uim_req_buf_ptr->apdu_hdr.p3 = 0;

            for ( i = start_index, j = 0; i < 4; i++ )
            {
              if (uim_path_to_be_sel[i] != UIM_NO_FILE_SEL)
              {
                uim_req_buf_ptr->req.data[j++] =WORD_HI(uim_path_to_be_sel[i]);
                uim_req_buf_ptr->req.data[j++] =WORD_LO(uim_path_to_be_sel[i]);

                /* Set the param3 as the total number of bytes */
                uim_req_buf_ptr->apdu_hdr.p3 +=2;
              }
              else
              {
                break;
              }
            }
          }
          else
          {
            if (path == UIM_SEL_ADF)
            {
              uim_req_buf_ptr->apdu_hdr.p1 = UIM_USIM_SELECT_BY_DF_NAME;

              uim_req_buf_ptr->apdu_hdr.p2 |= UIM_USIM_SELECT_FIRST_OCCURENCE;

              /* If it is an external select */
              if (cmd_ptr->hdr.command == UIM_SELECT_F ||
                  cmd_ptr->hdr.command == UIM_CACHED_SEL_F)
              {
                switch (cmd_ptr->select.session)
                {
                   case UIM_ACTIVATION_OR_RESET:
                     uim_req_buf_ptr->apdu_hdr.p2 |=
                                        UIM_USIM_SESSION_ACTIVATION;
                   break;

                    /* no data returned for app termination, p2 would be 0x4C*/
                   case UIM_TERMINATION:
                     uim_req_buf_ptr->apdu_hdr.p2 =
                       UIM_USIM_SESSION_TERMINATION | UIM_USIM_RETURN_NO_DATA;
                   break;
                }
              }

              if (cmd_ptr->hdr.command == UIM_INTERNAL_SELECT_F)
              {
                memcpy(uim_req_buf_ptr->req.data, uim_adf_to_be_sel.data,
                       uim_adf_to_be_sel.aid_length);

                uim_req_buf_ptr->apdu_hdr.p3 = uim_adf_to_be_sel.aid_length;
              }
              else
              {
                memcpy(uim_req_buf_ptr->req.data, cmd_ptr->select.aid.data,
                       cmd_ptr->select.aid.aid_length);

                uim_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->select.aid.aid_length;
              }
            }
            else
            {
              uim_req_buf_ptr->apdu_hdr.p1 = UIM_USIM_SELECT_BY_FILE_ID;
              uim_req_buf_ptr->apdu_hdr.p3 = 2;

              uim_req_buf_ptr->req.mf_df_ef.high_byte = WORD_HI (path);
              uim_req_buf_ptr->req.mf_df_ef.low_byte  = WORD_LO (path);
            }
          }
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
           uim_req_buf_ptr->apdu_hdr.p1 = 0;
           uim_req_buf_ptr->apdu_hdr.p2 = 0;
           uim_req_buf_ptr->apdu_hdr.p3 = 2;

           uim_req_buf_ptr->req.mf_df_ef.high_byte = WORD_HI (path);
           uim_req_buf_ptr->req.mf_df_ef.low_byte  = WORD_LO (path);
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr );

      } /* end case - UIM_SELECT_ST */
      break;

    case UIM_STATUS_ST:                       /* Status State */
      {
        /* Get the status (last selected DF) of the UIM card */
        uim_req_buf_ptr->apdu_hdr.instrn = STATUS;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_2;

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS2;

           uim_req_buf_ptr->apdu_hdr.p1 = (int)cmd_ptr->status.status;
           uim_req_buf_ptr->apdu_hdr.p2 = (int)cmd_ptr->status.return_data;

           if ( cmd_ptr->status.return_data == UIM_RETURN_NONE )
           {
             /* expect nothing in the response */
             uim_req_buf_ptr->apdu_hdr.p3 = 0;
             uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_1;
           }
           else
           {
             /* Maximum length of data expected in the response */
             uim_req_buf_ptr->apdu_hdr.p3 = 255;
           }
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

           uim_req_buf_ptr->apdu_hdr.p1 = 0;
           uim_req_buf_ptr->apdu_hdr.p2 = 0;
           uim_req_buf_ptr->apdu_hdr.p3 = 22;
           /* 22 bytes are mandatory and shall be returned by the R-UIM for
           the DF */

        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr );

      }
      break;

    case UIM_READ_BIN_MEM_ST:                  /* Read Binary State */
      {
        /* The selected file is TRANSPARENT.  Use a binary read to read
           from the file. */
        uim_req_buf_ptr->apdu_hdr.instrn = READ_BINARY;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_2;

        /* Offset into the EF - high byte, low byte */
        uim_req_buf_ptr->apdu_hdr.p1 = WORD_HI(uim_rd_wr_info.offset);
        uim_req_buf_ptr->apdu_hdr.p2 = WORD_LO(uim_rd_wr_info.offset);


        if (UIM_MAX_TRANSACTION_SIZE <= uim_rd_wr_info.num_bytes)
        {
          uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_TRANSACTION_P3;
        }
        else
        {
          uim_req_buf_ptr->apdu_hdr.p3 = WORD_LO (uim_rd_wr_info.num_bytes);
        }

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;

           /* SFI referencing is not used and so clear the MSB of p1 */
           uim_req_buf_ptr->apdu_hdr.p1 = uim_req_buf_ptr->apdu_hdr.p1 ^
               (uim_req_buf_ptr->apdu_hdr.p1 & UIM_SFI_NOT_USED_BR_MASK);
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      } /* end case - UIM_READ_BIN_MEM_ST */
      break;

    case UIM_UPDATE_BIN_MEM_ST:                /* Update Binary State */
      {
        /* The selected file is TRANSPARENT.  Use a binary update to write
           to the file. */
        uim_req_buf_ptr->apdu_hdr.instrn = UPDATE_BINARY;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;

        /* Offset into the EF -- high byte, low byte */
        uim_req_buf_ptr->apdu_hdr.p1 = WORD_HI(  uim_rd_wr_info.offset );
        uim_req_buf_ptr->apdu_hdr.p2 = WORD_LO(  uim_rd_wr_info.offset );

        if (UIM_MAX_TRANSACTION_SIZE <= uim_rd_wr_info.num_bytes)
        {
          uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_TRANS_SIZE_OUT;
        }
        else
        {
          uim_req_buf_ptr->apdu_hdr.p3 =
            WORD_LO (uim_rd_wr_info.num_bytes);
        }

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;

           /* SFI referencing is not used and so clear the MSB of p1 */
           uim_req_buf_ptr->apdu_hdr.p1 &= ~UIM_SFI_NOT_USED_BR_MASK;
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        memcpy(uim_req_buf_ptr->req.data, uim_rd_wr_info.data_ptr,
               uim_req_buf_ptr->apdu_hdr.p3);

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      } /* end case - UIM_UPDATE_BIN_MEM_ST */
      break;

    case UIM_READ_BINARY_ST:                  /* Read Binary State */
      {
        /* The selected file is TRANSPARENT.  Use a binary read to read
           from the file. */
        uim_req_buf_ptr->apdu_hdr.instrn = READ_BINARY;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_2;

        /* Offset into the EF - high byte, low byte */
        uim_req_buf_ptr->apdu_hdr.p1 = WORD_HI(cmd_ptr->access_uim.offset);

        uim_req_buf_ptr->apdu_hdr.p2 = WORD_LO(cmd_ptr->access_uim.offset);

        /* If the number of bytes requested is greater than the file size, then
           P3 is filled with the file size as the number of bytes to read */
        if (cmd_ptr->access_uim.num_bytes >
              uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size)
        {
          /* For transaction requests greater than our max transfer size, ask
             for the maximum transaction size */
          if (UIM_MAX_TRANSACTION_SIZE <=
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size)
          {
            uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_TRANSACTION_P3;
          }
          else
          {
            uim_req_buf_ptr->apdu_hdr.p3 =
              WORD_LO (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size);
          }
        }
        else
        {
          /* For transaction requests greater than our max transfer size, ask
             for the maximum transaction size */
          if (UIM_MAX_TRANSACTION_SIZE <= cmd_ptr->access_uim.num_bytes)
          {
            uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_TRANSACTION_P3;
          }
          else
          {
            uim_req_buf_ptr->apdu_hdr.p3 = (byte) cmd_ptr->access_uim.num_bytes;
          }
        }

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;

           /* SFI referencing is not used and so clear the MSB of p1 */
           uim_req_buf_ptr->apdu_hdr.p1 = uim_req_buf_ptr->apdu_hdr.p1 ^
               (uim_req_buf_ptr->apdu_hdr.p1 & UIM_SFI_NOT_USED_BR_MASK);
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      } /* end case - UIM_READ_BINARY_ST */
      break;

    case UIM_UPDATE_BINARY_ST:                /* Update Binary State */
      {
        /* The selected file is TRANSPARENT.  Use a binary update to write
           to the file. */
        uim_req_buf_ptr->apdu_hdr.instrn = UPDATE_BINARY;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;

        /* Offset into the EF -- high byte, low byte */
        uim_req_buf_ptr->apdu_hdr.p1 = WORD_HI( cmd_ptr->access_uim.offset );
        uim_req_buf_ptr->apdu_hdr.p2 = WORD_LO( cmd_ptr->access_uim.offset );

        /* If the number of bytes requested is greater than the file size, then
           use the file size for the number of bytes to write */
        if (cmd_ptr->access_uim.num_bytes >
              uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size)
        {
          /* Set the number of bytes to write to the file size */
          cmd_ptr->access_uim.num_bytes =
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size;

          /* For transaction requests greater than our max transfer size, send
             the maximum transaction size */
          if (UIM_MAX_TRANS_SIZE_OUT <=uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size)
          {
            uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_TRANS_SIZE_OUT;
          }
          else
          {
            uim_req_buf_ptr->apdu_hdr.p3 =
              WORD_LO (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size);
          }
        }
        else
        {
          /* For transaction requests greater than our max transfer size, send
             the maximum transaction size */
          if (UIM_MAX_TRANS_SIZE_OUT <= cmd_ptr->access_uim.num_bytes)
          {
            uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_TRANS_SIZE_OUT;
          }
          else
          {
            uim_req_buf_ptr->apdu_hdr.p3 =
                                       WORD_LO (cmd_ptr->access_uim.num_bytes);
          }
        }

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;

           /* SFI referencing is not used and so clear the MSB of p1 */
           uim_req_buf_ptr->apdu_hdr.p1 &= ~UIM_SFI_NOT_USED_BR_MASK;
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        memcpy(uim_req_buf_ptr->req.data, cmd_ptr->access_uim.data_ptr,
               uim_req_buf_ptr->apdu_hdr.p3);

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      } /* end case - UIM_UPDATE_BINARY_ST */
      break;

    case UIM_READ_REC_MEM_ST:                  /* Read Record State */
      {
        /* The selected file is a record format.  Use a record read to read
           from the file. */
        uim_req_buf_ptr->apdu_hdr.instrn = READ_RECORD;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_2;

  #ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;
        }
  #endif /* FEATURE_UIM_UICC */
  #if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
  #ifdef FEATURE_UIM_UICC
        else
  #endif /* FEATURE_UIM_UICC */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
  #endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        /* Fill out APDU parameters based on the request mode */
        switch (cmd_ptr->read_rec.recmode)
        {
          case UIM_CURRENT:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = 0;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_ABS_RECORD_MODE;
            } /* end case - UIM_CURRENT */
            break;

          case UIM_NEXT:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = 0;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_NEXT_RECORD_MODE;
            } /* end case - UIM_NEXT */
            break;

          case UIM_PREVIOUS:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = 0;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_PREV_RECORD_MODE;
            }
            break;

          case UIM_ABSOLUTE:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = cmd_ptr->read_rec.rec_num;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_ABS_RECORD_MODE;
            }
            break;

          default:
            break;

        } /* end switch - cmd_ptr->read_rec.rec_mode */

  #ifdef FEATURE_UIM_UICC
        /* SFI referecning is not used, so clear the MS 5 bits of p2 */
        uim_req_buf_ptr->apdu_hdr.p2 = uim_req_buf_ptr->apdu_hdr.p2 ^
            (uim_req_buf_ptr->apdu_hdr.p2 & UIM_SFI_NOT_USED_RR_MASK);
  #endif /* FEATURE_UIM_UICC */

        /* Number of bytes to be read */
        uim_req_buf_ptr->apdu_hdr.p3 = uint16touint8(uim_rd_wr_info.num_bytes);

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      } /* end case - UIM_READ_REC_MEM_ST */
      break;

    case UIM_UPDATE_REC_MEM_ST:                /* Update Record State */
      {
        /* The last selected file is a record structure.  Use update record
           to write to this file */
        uim_req_buf_ptr->apdu_hdr.instrn = UPDATE_RECORD;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;

  #ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;
        }
  #endif /* FEATURE_UIM_UICC */
  #if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
  #ifdef FEATURE_UIM_UICC
        else
  #endif /* FEATURE_UIM_UICC */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
  #endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        /* Fill out APDU parameters based on the request mode */
        switch (cmd_ptr->write_rec.recmode)
        {
          case UIM_CURRENT:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = 0;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_ABS_RECORD_MODE;
            } /* end case - UIM_CURRENT */
            break;

          case UIM_NEXT:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = 0;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_NEXT_RECORD_MODE;
            } /* end case - UIM_NEXT */
            break;

          case UIM_PREVIOUS:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = 0;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_PREV_RECORD_MODE;
            } /* end case - UIM_PREVIOUS */
            break;

          case UIM_ABSOLUTE:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = cmd_ptr->write_rec.rec_num;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_ABS_RECORD_MODE;
            } /* end case - UIM_ABSOLUTE */
            break;

          default:
            break;

        } /* end switch - cmd_ptr->write_rec.rec_mode */

  #ifdef FEATURE_UIM_UICC
        /* SFI referecning is not used, so clear the MS 5 bits of p2 */
        uim_req_buf_ptr->apdu_hdr.p2 = uim_req_buf_ptr->apdu_hdr.p2 ^
            (uim_req_buf_ptr->apdu_hdr.p2 & UIM_SFI_NOT_USED_RR_MASK);
  #endif /* FEATURE_UIM_UICC */

        /* Number of bytes to write */
        uim_req_buf_ptr->apdu_hdr.p3 = uint16touint8(uim_rd_wr_info.num_bytes);

        memcpy(uim_req_buf_ptr->req.data, uim_rd_wr_info.data_ptr,
               uim_req_buf_ptr->apdu_hdr.p3);

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      } /* end case - UIM_UPDATE_REC_MEM_ST */
      break;

    case UIM_READ_RECORD_ST:                  /* Read Record State */
      {
        /* The selected file is a record format.  Use a record read to read
           from the file. */
        uim_req_buf_ptr->apdu_hdr.instrn = READ_RECORD;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_2;

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        /* Fill out APDU parameters based on the request mode */
        switch (cmd_ptr->access_uim.rec_mode)
        {
          case UIM_CURRENT:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = 0;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_ABS_RECORD_MODE;
            } /* end case - UIM_CURRENT */
            break;

          case UIM_NEXT:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = 0;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_NEXT_RECORD_MODE;
            } /* end case - UIM_NEXT */
            break;

          case UIM_PREVIOUS:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = 0;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_PREV_RECORD_MODE;
            }
            break;

          case UIM_ABSOLUTE:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = (byte)cmd_ptr->access_uim.offset;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_ABS_RECORD_MODE;
            }
            break;

          default:
            break;

        } /* end switch - cmd_ptr->access_uim.rec_mode */

#ifdef FEATURE_UIM_UICC
        /* SFI referecning is not used, so clear the MS 5 bits of p2 */
        uim_req_buf_ptr->apdu_hdr.p2 = uim_req_buf_ptr->apdu_hdr.p2 ^
            (uim_req_buf_ptr->apdu_hdr.p2 & UIM_SFI_NOT_USED_RR_MASK);
#endif /* FEATURE_UIM_UICC */

        /* Number of bytes to be read */
        uim_req_buf_ptr->apdu_hdr.p3 = (byte) cmd_ptr->access_uim.num_bytes;

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      } /* end case - UIM_READ_RECORD_ST */
      break;

    case UIM_UPDATE_RECORD_ST:                /* Update Record State */
      {
        /* The last selected file is a record structure.  Use update record
           to write to this file */
        uim_req_buf_ptr->apdu_hdr.instrn = UPDATE_RECORD;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        /* Fill out APDU parameters based on the request mode */
        switch (cmd_ptr->access_uim.rec_mode)
        {
          case UIM_CURRENT:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = 0;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_ABS_RECORD_MODE;
            } /* end case - UIM_CURRENT */
            break;

          case UIM_NEXT:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = 0;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_NEXT_RECORD_MODE;
            } /* end case - UIM_NEXT */
            break;

          case UIM_PREVIOUS:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = 0;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_PREV_RECORD_MODE;
            } /* end case - UIM_PREVIOUS */
            break;

          case UIM_ABSOLUTE:
            {
              uim_req_buf_ptr->apdu_hdr.p1 = (byte)cmd_ptr->access_uim.offset;
              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_ABS_RECORD_MODE;
            } /* end case - UIM_ABSOLUTE */
            break;

          default:
            break;

        } /* end switch - cmd_ptr->access_uim.rec_mode */

#ifdef FEATURE_UIM_UICC
        /* SFI referecning is not used, so clear the MS 5 bits of p2 */
        uim_req_buf_ptr->apdu_hdr.p2 = uim_req_buf_ptr->apdu_hdr.p2 ^
            (uim_req_buf_ptr->apdu_hdr.p2 & UIM_SFI_NOT_USED_RR_MASK);
#endif /* FEATURE_UIM_UICC */

        /* Number of bytes to write */
        uim_req_buf_ptr->apdu_hdr.p3 = (byte) cmd_ptr->access_uim.num_bytes;

        memcpy(uim_req_buf_ptr->req.data, cmd_ptr->access_uim.data_ptr,
               cmd_ptr->access_uim.num_bytes );

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      } /* end case - UIM_UPDATE_RECORD_ST */
      break;

    case UIM_INC_MEM_ST:                    /* Increment State */
      {
        /* Build an increase APDU */
        uim_req_buf_ptr->apdu_hdr.instrn = INCREASE;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS2;
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        uim_req_buf_ptr->apdu_hdr.p3 = uint16touint8(uim_rd_wr_info.num_bytes);

        memcpy( uim_req_buf_ptr->req.data, uim_rd_wr_info.data_ptr,
                 uim_rd_wr_info.num_bytes);
        /* Increase the currently selected record in USIM as SFI referencing
           is not used */
        uim_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_ptr->apdu_hdr.p2 = 0;

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      } /* end case - UIM_INCREMENT_ST */
      break;

    case UIM_INCREMENT_ST:                    /* Increment State */
      {
        /* Build an increase APDU */
        uim_req_buf_ptr->apdu_hdr.instrn = INCREASE;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS2;

           /* Length of data to be added */
           uim_req_buf_ptr->apdu_hdr.p3 = (byte) cmd_ptr->access_uim.num_bytes;

           /* Copy the bytes that are being sent as data */
           memcpy( uim_req_buf_ptr->req.data, cmd_ptr->access_uim.data_ptr,
                   uim_req_buf_ptr->apdu_hdr.p3);
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

           /* 3 bytes are mandatory for this command in GSM */
           uim_req_buf_ptr->apdu_hdr.p3 = (byte) cmd_ptr->access_uim.num_bytes;

           memcpy( uim_req_buf_ptr->req.data, cmd_ptr->access_uim.data_ptr, 3);
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        /* Increase the currently selected record in USIM as SFI referencing
           is not used */
        uim_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_ptr->apdu_hdr.p2 = 0;

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      } /* end case - UIM_INCREMENT_ST */
      break;

#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
    case UIM_VERIFY_CHV1_ST:                  /* Verify CHV 1 State */
      {
        /* Check if CHV1 has been cache'd or this is a command to verify CHV */
        if (uim_chv1[uim_drv_slot].use_chv)
        {
          /* Build a verify CHV APDU */
          uim_req_buf_ptr->apdu_hdr.instrn = VERIFY_CHV;
          uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;

          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

          uim_req_buf_ptr->apdu_hdr.p1 = 0;

          uim_req_buf_ptr->apdu_hdr.p2 = (byte) UIM_CHV1;

          /* bytes of data sent in the command */
          uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;

          uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

          uim_req_buf_ptr->rpt_function = uim_command_response_callback;

          /* The CHV digits are copied from the cache'd buffer so that CHV1
             can be verified on wake up of the UIM (no explicit command). */
          memcpy( (byte*) uim_req_buf_ptr->req.verify_chv.chv,
                  uim_chv1[uim_drv_slot].chv, UIM_MAX_CHV_DIGITS);

          uim_send_command(uim_req_buf_ptr);
        }
        else /* CHV 1 value is not cache'd */
        {
          /* Skip this state */
          ++uim_generic_state_ptr;
          /* Call this function to process the next state. */
          uim_generic_command (cmd_ptr);
          return; /* exit immediately */
        } /* end if - the CHV 1 value is cache'd */
      } /* end case - UIM_VERIFY_CHV1_ST */
      break;
#endif /* FEATURE_UIM_GSM || FEATURE_UIM_RUIM */

    case UIM_VERIFY_CHV2_ST:                  /* Verify CHV 2 State */
      {
        /* Build a verify CHV or a VERIFY PIN APDU */
        uim_req_buf_ptr->apdu_hdr.instrn = VERIFY_CHV;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;

        uim_req_buf_ptr->apdu_hdr.p1 = 0;

#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        if (cmd_ptr->hdr.protocol != UIM_UICC)
        {
          if (!uim_chv2[uim_drv_slot].use_chv)
          {
            /* Do not verify CHV2 and skip this state */
            ++uim_generic_state_ptr;
            /* Call this function to process the next state. */
            uim_generic_command (cmd_ptr);
            return; /* exit immediately */
          }

          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
          uim_req_buf_ptr->apdu_hdr.p2 = (byte) UIM_CHV2;  /* CHV2 */
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
#ifdef FEATURE_UIM_UICC
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif /* (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM) */
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;
          uim_set_uicc_pin_param_p2( cmd_ptr->verify_chv.chv,
                                    (byte*) &(uim_req_buf_ptr->apdu_hdr.p2));
        }
#endif /* FEATURE_UIM_UICC */

#ifdef FEATURE_UIM_UICC
        /* If the command is being sent just to get the number of re-tries
           remaining, do not include the data */
        if ((cmd_ptr->verify_chv.chv_digits[0] == '\0') &&
          (cmd_ptr->hdr.command == UIM_VERIFY_CHV_F))
        {
          uim_req_buf_ptr->apdu_hdr.p3 = 0;
          uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_1;
        }
        else
#endif /* FEATURE_UIM_UICC */
        {
          uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;

          memcpy( (byte*) uim_req_buf_ptr->req.verify_chv.chv,
                          uim_chv2[uim_drv_slot].chv, UIM_MAX_CHV_DIGITS);
        }

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);
      } /* end case - UIM_VERIFY_CHV2_ST */
      break;

#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
    case UIM_UNBLOCK_CHV1_ST:                 /* Unblock CHV 1 State */
      {
        /* Build an unblock CHV APDU */
        uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_req_buf_ptr->apdu_hdr.instrn = UNBLOCK_CHV;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
        uim_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_ptr->apdu_hdr.p2 = 0;     /* weird but TRUE,!= UIM_CHV1*/
        /* bytes of data sent in cmd */
        uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS * 2;

        /* The unblock digits are provided in the command buffer by the
           client task. */
        memcpy( (byte*) uim_req_buf_ptr->req.unblk_chv.chv,
                cmd_ptr->unblk_chv.chv_digits, UIM_MAX_CHV_DIGITS);

        /* The new CHV1 digits are provided in the command buffer by the
           client task. These digits are sent in the APDU.  The digits
           are copied to the cache buffer if the command completes
           successfully. */
        memcpy( (byte*) uim_req_buf_ptr->req.unblk_chv.new_chv,
                cmd_ptr->unblk_chv.new_chv_digits, UIM_MAX_CHV_DIGITS);

        /* Format the CHV digits in the request buffer */
        uim_format_chv( (byte*) uim_req_buf_ptr->req.unblk_chv.new_chv);

        /* Format the CHV digits in the request buffer */
        uim_format_chv( (byte*) uim_req_buf_ptr->req.unblk_chv.chv);

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      } /* end case - UIM_UNBLOCK_CHV1_ST */
      break;
#endif /* FEATURE_UIM_GSM || FEATURE_UIM_RUIM */

    case UIM_UNBLOCK_CHV2_ST:                 /* Unblock CHV 2 State */
      {
        /* Build an unblock CHV or Unblock PIN APDU */
        uim_req_buf_ptr->apdu_hdr.instrn = UNBLOCK_CHV;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;

        uim_req_buf_ptr->apdu_hdr.p1 = 0;

#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        if (cmd_ptr->unblk_chv.chv == UIM_CHV2)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

           uim_req_buf_ptr->apdu_hdr.p2 = (byte) UIM_CHV2;  /* CHV2 */
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
#ifdef FEATURE_UIM_UICC
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif /* (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM) */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;

           uim_set_uicc_pin_param_p2( cmd_ptr->unblk_chv.chv,
                                      (byte*) &(uim_req_buf_ptr->apdu_hdr.p2));
        }
#endif /* FEATURE_UIM_UICC */

#ifdef FEATURE_UIM_UICC
        /* If the command is being sent just to get the number of re-tries
           remaining, do not include the data */
        if (cmd_ptr->unblk_chv.chv_digits[0] == '\0')
        {
            uim_req_buf_ptr->apdu_hdr.p3 = 0;
            uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_1;
        }
        else
#endif /* FEATURE_UIM_UICC */
        {
           /* bytes of data sent in cmd */
           uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS * 2;

           /* The unblock digits are supplied in the command buffer by the
              client task. */
           memcpy( (byte*) uim_req_buf_ptr->req.unblk_chv.chv,
                  cmd_ptr->unblk_chv.chv_digits, UIM_MAX_CHV_DIGITS);

           /* Get the CHV digits from the command buffer.  The digits are not
              saved in a cache buffer for CHV2 since CHV2 is only verified
              by an explicit call from a client (not on wakeup of the UIM). */
           memcpy( (byte*) uim_req_buf_ptr->req.unblk_chv.new_chv,
                   cmd_ptr->unblk_chv.new_chv_digits, UIM_MAX_CHV_DIGITS);

           /* Format the CHV digits for the new CHV */
           uim_format_chv((byte*) uim_req_buf_ptr->req.unblk_chv.new_chv);

           /* Format the CHV digits for Unblock CHV */
           uim_format_chv((byte*) uim_req_buf_ptr->req.unblk_chv.chv);
        }

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      } /* end case - UIM_UNBLOCK_CHV2_ST */
      break;

#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
    case UIM_CHANGE_CHV1_ST:                  /* Change CHV 1 State */
      {
        /* Build an change CHV APDU */
        uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_req_buf_ptr->apdu_hdr.instrn = CHANGE_CHV;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
        uim_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_ptr->apdu_hdr.p2 = (int)UIM_CHV1; /* CHV1 */
        /* bytes of data sent in cmd */
        uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS * 2;

        /* The old CHV1 digits are supplied in the command buffer by the
           client task.  In this case, we do not use the cache buffer as
           the cache buffer may not contain the proper digits. */
        memcpy( (byte*) uim_req_buf_ptr->req.change_chv.chv,
                cmd_ptr->change_chv.chv_digits, UIM_MAX_CHV_DIGITS);

        /* The client task supplies the new CHV digits in the command buffer.
           The digits will be copied to the CHV1 cache buffer if the command
           completes successfully. */
        memcpy( (byte*) uim_req_buf_ptr->req.change_chv.new_chv,
                cmd_ptr->change_chv.new_chv_digits, UIM_MAX_CHV_DIGITS);

        /* Format the CHV digits */
        uim_format_chv((byte*) uim_req_buf_ptr->req.change_chv.chv);
        uim_format_chv((byte*) uim_req_buf_ptr->req.change_chv.new_chv);

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);
      } /* end case - UIM_CHANGE_CHV1_ST */
      break;
#endif /* FEATURE_UIM_GSM || FEATURE_UIM_RUIM */

    case UIM_CHANGE_CHV2_ST:                  /* Change CHV 2 State */
      {
        /* Build an change CHV APDU or CHANGE PIN APDU */
        uim_req_buf_ptr->apdu_hdr.instrn = CHANGE_CHV;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;

        uim_req_buf_ptr->apdu_hdr.p1 = 0;

#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        if (cmd_ptr->change_chv.chv == UIM_CHV2)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

           uim_req_buf_ptr->apdu_hdr.p2 = (byte) UIM_CHV2;  /* CHV2 */
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
#ifdef FEATURE_UIM_UICC
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif /* (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM) */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;

           uim_set_uicc_pin_param_p2( cmd_ptr->change_chv.chv,
                                      (byte*) &(uim_req_buf_ptr->apdu_hdr.p2));
        }
#endif /* FEATURE_UIM_UICC */

        /* bytes of data sent in cmd */
        uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS * 2;

        /* The old CHV2 digits are supplied in the command buffer by the
           client task.  The CHV2 digits are not kept in a cache buffer. */
        memcpy( (byte*) uim_req_buf_ptr->req.change_chv.chv,
                cmd_ptr->change_chv.chv_digits, UIM_MAX_CHV_DIGITS);

        /* The client task supplies the new CHV digits in the command buffer.
           The digits are not copied to a cache buffer as there is no need
           for a cache buffer for CHV2. */
        memcpy( (byte*) uim_req_buf_ptr->req.change_chv.new_chv,
                cmd_ptr->change_chv.new_chv_digits, UIM_MAX_CHV_DIGITS);

        /* Format the CHV digits */
        uim_format_chv((byte*) uim_req_buf_ptr->req.change_chv.chv);
        uim_format_chv((byte*) uim_req_buf_ptr->req.change_chv.new_chv);

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);
      } /* end case - UIM_CHANGE_CHV2_ST */
      break;

    case UIM_ENABLE_CHV1_ST:                  /* Enable CHV 1 State */
      {
        /* Build an enable CHV APDU */
        uim_req_buf_ptr->apdu_hdr.instrn = ENABLE_CHV;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;

        uim_req_buf_ptr->apdu_hdr.p1 = 0;

#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        if (cmd_ptr->enable_chv.chv == UIM_CHV1)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

           uim_req_buf_ptr->apdu_hdr.p2 = (byte) UIM_CHV1;  /* CHV2 */
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
#ifdef FEATURE_UIM_UICC
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif /* (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM) */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;

           uim_set_uicc_pin_param_p2( cmd_ptr->enable_chv.chv,
                                      (byte*) &(uim_req_buf_ptr->apdu_hdr.p2));
        }
#endif /* FEATURE_UIM_UICC */

        uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;
                      /* bytes of data sent in the command */

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        /* The CHV1 digits are provided in the command buffer by the
           client task. These digits are sent in the APDU.  The digits
           are copied to the cache buffer if the command completes
           successfully. */
        memcpy( (byte*) uim_req_buf_ptr->req.enable_chv.chv,
                cmd_ptr->enable_chv.chv_digits, UIM_MAX_CHV_DIGITS);

        uim_format_chv((byte*) uim_req_buf_ptr->req.enable_chv.chv);

        uim_send_command(uim_req_buf_ptr);
      } /* end case - UIM_ENABLE_CHV1_ST */
      break;

#if (defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)) && \
     defined (UIM_FEATURE_ALLOW_CHV2_DISABLE)
    case UIM_ENABLE_CHV2_ST:                  /* Enable CHV 2 State */
      {
        uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_req_buf_ptr->apdu_hdr.instrn = ENABLE_CHV;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
        uim_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_ptr->apdu_hdr.p2 = (byte) UIM_CHV2;
        uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;
                      /* bytes of data sent in the command */

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        memcpy( (byte*) uim_req_buf_ptr->req.enable_chv.chv,
                cmd_ptr->enable_chv.chv_digits, UIM_MAX_CHV_DIGITS);

        uim_format_chv((byte*) uim_req_buf_ptr->req.enable_chv.chv);

        uim_send_command(uim_req_buf_ptr);
      } /* end case - UIM_ENABLE_CHV2_ST */
      break;
#endif /* UIM_FEATURE_ALLOW_CHV2_DISABLE */

    case UIM_DISABLE_CHV1_ST:                 /* Disable CHV 1 State */
      {
        /* Build a disable CHV APDU */

        uim_req_buf_ptr->apdu_hdr.instrn = DISABLE_CHV;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;

        uim_req_buf_ptr->apdu_hdr.p1 = 0;

#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        if (cmd_ptr->disable_chv.chv == UIM_CHV1)
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

           uim_req_buf_ptr->apdu_hdr.p2 = (byte) UIM_CHV1;  /* CHV2 */
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
#ifdef FEATURE_UIM_UICC
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
        else
#endif /* (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM) */
        {
           uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;

           uim_set_uicc_pin_param_p2( cmd_ptr->disable_chv.chv,
                                      (byte*) &(uim_req_buf_ptr->apdu_hdr.p2));

           if(cmd_ptr->disable_chv.param_p2 != 0)
           {
              uim_req_buf_ptr->apdu_hdr.p1 = 0x80 | UIM_UNIVERSAL_PIN_VALUE;
           }
        }
#endif /* FEATURE_UIM_UICC */

        uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;
                      /* bytes of data sent in the command */

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        /* The CHV1 digits are provided in the command buffer by the
           client task. These digits are sent in the APDU.  The digits
           are copied to the cache buffer if the command completes
           successfully. */
        memcpy( (byte*) uim_req_buf_ptr->req.disable_chv.chv,
                cmd_ptr->disable_chv.chv_digits, UIM_MAX_CHV_DIGITS);

        uim_format_chv((byte*) uim_req_buf_ptr->req.disable_chv.chv);

        uim_send_command(uim_req_buf_ptr);
      } /* end case - UIM_DISABLE_CHV1_ST */
      break;

#if (defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)) && \
     defined ( UIM_FEATURE_ALLOW_CHV2_DISABLE)
    case UIM_DISABLE_CHV2_ST:                 /* Disable CHV 2 State */
      {
        /* Build a disable CHV APDU */
        uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        uim_req_buf_ptr->apdu_hdr.instrn = DISABLE_CHV;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
        uim_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_ptr->apdu_hdr.p2 = (byte) UIM_CHV2;
        uim_req_buf_ptr->apdu_hdr.p3 = UIM_MAX_CHV_DIGITS;
                      /* bytes of data sent in the command */

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        memcpy( (byte*) uim_req_buf_ptr->req.disable_chv.chv,
                cmd_ptr->disable_chv.chv_digits, UIM_MAX_CHV_DIGITS);

        uim_format_chv((byte*) uim_req_buf_ptr->req.disable_chv.chv);

        uim_send_command(uim_req_buf_ptr);
      } /* end case - UIM_DISABLE_CHV2_ST */
      break;
#endif /* UIM_FEATURE_ALLOW_CHV2_DISABLE */

    case UIM_INVALIDATE_ST:                   /* Invalidate State */
      {
        /* Build an invalidate APDU */
        uim_req_buf_ptr->apdu_hdr.instrn = INVALIDATE;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_1;

        /* P1 = P2 = 0 and data field is empty in USIM implies that the
           command applies on the current EF */
        uim_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_ptr->apdu_hdr.p2 = 0;
        uim_req_buf_ptr->apdu_hdr.p3 = 0;
                    /* bytes of data sent in the command */

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);
      } /* end case - UIM_INVALIDATE_ST */
      break;

    case UIM_REHABILITATE_ST:                 /* Rehab state */
      {
        /* Build a rehabilitate APDU */
        uim_req_buf_ptr->apdu_hdr.instrn = REHABILITATE;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_1;

        /* P1 = P2 = 0 and data field is empty in USIM implies that the
           command applies on the current EF */
        uim_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_ptr->apdu_hdr.p2 = 0;
        uim_req_buf_ptr->apdu_hdr.p3 = 0;
                    /* bytes of data sent in the command */

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);
      } /* end case - UIM_REHABILITATE_ST */
      break;

    case UIM_SEEK_ST:
      {
        /* Build a seek APDU */
        uim_req_buf_ptr->apdu_hdr.instrn = SEEK;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS1;

          /* Simple search or enhanced search */
          switch (cmd_ptr->seek.srch_mode)
          {
            case  UIM_SIMPLE_SRCH:

              uim_req_buf_ptr->apdu_hdr.p1 = cmd_ptr->seek.rec_number;
              uim_req_buf_ptr->apdu_hdr.p2 = (int)cmd_ptr->seek.srch_dir;

              /* Number of bytes of data sent in the command */
              uim_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->seek.num_bytes;

              /* Copy the seek patern from the command */
              memcpy( uim_req_buf_ptr->req.data, cmd_ptr->seek.srch_pattern,
                      cmd_ptr->seek.num_bytes );

            break; /* UIM_SIMPLE_SRCH  */

            case UIM_ENHANCED_SRCH:

              /* See if search is based on  rec # in P1 or current rec */
              if ((int)cmd_ptr->seek.srch_rec_mode_type & UIM_SRCH_MODE_MASK )
                uim_req_buf_ptr->apdu_hdr.p1 = 0;
              else
                uim_req_buf_ptr->apdu_hdr.p1 = cmd_ptr->seek.rec_number;

              uim_req_buf_ptr->apdu_hdr.p2 = UIM_P2_SRCH_ENHANCED;

              /* Number of bytes of data sent in the command */
              uim_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->seek.num_bytes +
                                              UIM_SRCH_IND_BYTES;

              /* Search indicator bytes */
              uim_req_buf_ptr->req.data[0] = (int)cmd_ptr->seek.offset_type
                                           | (int)cmd_ptr->seek.srch_rec_mode_type;

              uim_req_buf_ptr->req.data[1] = cmd_ptr->seek.offset;

              /* Copy the seek patern from the command */
              memcpy( &uim_req_buf_ptr->req.data[UIM_SRCH_IND_BYTES],
                      cmd_ptr->seek.srch_pattern, cmd_ptr->seek.num_bytes );

            break; /* UIM_ENHANCED_SRCH */

            default:

              MSG_ERROR("Wrong Search Mode %d", cmd_ptr->seek.srch_mode, 0, 0);

            break;
          } /* switch (cmd_ptr->seek.srch_mode) */
        } /* if (cmd_ptr->hdr.protocol == UIM_UICC) */
#endif /* FEATURE_UIM_UICC */

#if defined (FEATURE_UIM_RUIM) || defined (FEATURE_UIM_GSM)
        if ((cmd_ptr->hdr.protocol == UIM_GSM) ||
            (cmd_ptr->hdr.protocol == UIM_CDMA))
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;

          uim_req_buf_ptr->apdu_hdr.p1 = 0;

          uim_req_buf_ptr->apdu_hdr.p2 = (int)cmd_ptr->seek.seek_type
                                       | (int)cmd_ptr->seek.mode;

          /* Number of bytes of data sent in the command */
          uim_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->seek.num_bytes;

          /* Copy the seek patern from the command */
          memcpy( uim_req_buf_ptr->req.data, cmd_ptr->seek.srch_pattern,
                  cmd_ptr->seek.num_bytes );
        }
#endif /* (FEATURE_UIM_GSM ) || defined (FEATURE_UIM_RUIM) */

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      } /* end case for UIM_SEEK_ST */
      break;

#ifdef FEATURE_UIM_TOOLKIT

    case UIM_TERMINAL_PROFILE_ST:             /* Term profile State */
      {
        if ( ((cmd_ptr->hdr.command == UIM_INTERNAL_WAKE_UP_F) &&
              (uim_int_term_profile_data.num_bytes != 0)) ||
             (cmd_ptr->hdr.command == UIM_TERMINAL_PROFILE_F)
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
             || ((cmd_ptr->hdr.command == UIM_RESET_WITH_TP_F) &&
                 (cmd_ptr->reset_with_tp.terminal_profile) &&
                 (uim_int_term_profile_data.num_bytes != 0))
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
           )
        {
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
          /* If it is either one of the two commands, set the toolkit
             slot. If the download fails, then it would get reset in
             the processing of error resposne */
          if ((cmd_ptr->hdr.command == UIM_TERMINAL_PROFILE_F) ||
               (cmd_ptr->hdr.command == UIM_RESET_WITH_TP_F))
          {
            uim_toolkit_slot = cmd_ptr->hdr.slot;
            uim_polling_slot = cmd_ptr->hdr.slot;
          }
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

          /* Build a Terminal Profile APDU */
          uim_req_buf_ptr->apdu_hdr.instrn = TERMINAL_PROFILE;
          uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;

          /* P1 = P2 = 0 and data field is empty in USIM implies that the
          command applies on the current EF */
          uim_req_buf_ptr->apdu_hdr.p1 = 0;
          uim_req_buf_ptr->apdu_hdr.p2 = 0;

#ifdef FEATURE_UIM_UICC
          if (cmd_ptr->hdr.protocol == UIM_UICC)
          {
            uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS2;
          }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
          else
#endif /* FEATURE_UIM_UICC */
          {
            uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
          }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

          if ((cmd_ptr->hdr.command == UIM_INTERNAL_WAKE_UP_F)
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
              || (cmd_ptr->hdr.command == UIM_RESET_WITH_TP_F)
#endif /* FEATURE_UIM_DUAL_SLOTS */
             )
          {
            /* Number of bytes of Terminal Profile */
            uim_req_buf_ptr->apdu_hdr.p3 = uim_int_term_profile_data.num_bytes;

            /* Copy the data from the internal buffer */
            memcpy (uim_req_buf_ptr->req.data,
                    uim_int_term_profile_data.data,
                    uim_int_term_profile_data.num_bytes );
          }
          else
          {
            /* Number of bytes of Terminal Profile */
            uim_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->terminal_profile.num_bytes;

            /* Copy the data from the command */
            memcpy(uim_req_buf_ptr->req.data, cmd_ptr->terminal_profile.data,
                   cmd_ptr->terminal_profile.num_bytes );
          }

          uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

          uim_req_buf_ptr->rpt_function = uim_command_response_callback;

          uim_send_command(uim_req_buf_ptr);
        } /* end if command needs to be sent */
        else
        {
          /* Signal is set internally for proper operation of state machine */
#ifndef TEST_FRAMEWORK
          (void)rex_timed_wait(UIM_CMD_RSP_SIG, &uim_delay_timer,
                         UIM_CARD_DELAY_TIMEOUT);
#else
           (void)rex_set_sigs(&uim_tcb,UIM_CMD_RSP_SIG);
#endif/* !TEST_FRAMEWORK */
        }
      } /* end case - UIM_TERMINAL_PROFILE_ST */
      break;

    case UIM_ENVELOPE_ST:                     /* Envelope State */
      {
        /* Build an Envelope APDU */
        uim_req_buf_ptr->apdu_hdr.instrn = ENVELOPE;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;

        /* P1 = P2 = 0 and data field is empty in USIM implies that the
           command applies on the current EF */
        uim_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_ptr->apdu_hdr.p2 = 0;

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS2;
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        /* Number of bytes of Envelope command */
        uim_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->envelope.num_bytes;

        memcpy(uim_req_buf_ptr->req.data,
               cmd_ptr->envelope.data + cmd_ptr->envelope.offset,
               cmd_ptr->envelope.num_bytes );

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);
      } /* end case - UIM_ENVELOPE_ST */
      break;

    case UIM_FETCH_ST:                        /* Fetch State */
      {
        /* Build a Fetch APDU */
        uim_req_buf_ptr->apdu_hdr.instrn = FETCH;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_2;

        /* P1 = P2 = 0 and data field is empty in USIM implies that the
           command applies on the current EF */
        uim_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_ptr->apdu_hdr.p2 = 0;

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS2;
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        /* Number of bytes of Fetch command */
        uim_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->fetch.command_size;

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);
      } /* end case - UIM_FETCH_ST */
      break;

    case UIM_TERMINAL_RESPONSE_ST:            /* Term response State */
      {
        /* Build a Terminal Response APDU */
        uim_req_buf_ptr->apdu_hdr.instrn = TERMINAL_RESPONSE;
        uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;

        /* P1 = P2 = 0 and data field is empty in USIM implies that the
           command applies on the current EF */
        uim_req_buf_ptr->apdu_hdr.p1 = 0;
        uim_req_buf_ptr->apdu_hdr.p2 = 0;

#ifdef FEATURE_UIM_UICC
        if (cmd_ptr->hdr.protocol == UIM_UICC)
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_UICC_INSTRUCTION_CLASS2;
        }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
        else
#endif /* FEATURE_UIM_UICC */
        {
          uim_req_buf_ptr->apdu_hdr.uim_class = UIM_INSTRUCTION_CLASS;
        }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */

        /* Number of bytes of Terminal Response */
        uim_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->terminal_response.num_bytes;

        memcpy(uim_req_buf_ptr->req.data,
               cmd_ptr->terminal_response.data,
               cmd_ptr->terminal_response.num_bytes );

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      } /* end case - UIM_TERMINAL_RESPONSE_ST */
      break;

    case UIM_POLLING_INTERVAL_ST:
      {
        switch (cmd_ptr->polling.command_type)
        {
          case POLLING_INTERVAL:
            {
              uim_proactive_time_delay=cmd_ptr->polling.polling_interval_in_ms;

              if (uim_proactive_time_delay < UIM_MIN_POLL_DURATION)
              {
                uim_proactive_time_delay = UIM_MIN_POLL_DURATION;
              }

              /* Subtract UIM_POLL_OFFSET from the time_delay that is used by
                 the server */
#if (UIM_MIN_POLL_DURATION > UIM_POLL_OFFSET)
              uim_proactive_time_delay -= UIM_POLL_OFFSET;
#endif
            }
            break;

          case POLLING_OFF:
            {
              /* Set the proactive polling duration to the maximum allowed */
              uim_proactive_time_delay = (dword)UIM_MAX_POLL_DURATION;
            }
            break;
        } /* switch */

        /* Call the UIM power control function to use the new Proactive
           polling value.  Do not affect the power on voting. */
        uim_rpt_buf.rpt.polling_interval.actual_uim_polling_interval =
            uim_power_control( UIM_NONE, TRUE );

#if (UIM_MIN_POLL_DURATION > UIM_POLL_OFFSET)
          /* Add the UIM_POLL_OFFSET back so that the negotiated time does not
             include the poll_offset as poll_offset is for internal purposes
             only */
          if (cmd_ptr->polling.command_type != POLLING_OFF)
          {
            uim_rpt_buf.rpt.polling_interval.actual_uim_polling_interval +=
                                                               UIM_POLL_OFFSET;
          }
#endif
      }
      /* Signal is set internally for proper operation of state machine */
#ifndef TEST_FRAMEWORK
      (void)rex_timed_wait(UIM_CMD_RSP_SIG, &uim_delay_timer,
                     UIM_CARD_DELAY_TIMEOUT);
#else
       (void)rex_set_sigs(&uim_tcb,UIM_CMD_RSP_SIG);
#endif/* !TEST_FRAMEWORK */

      break;

#endif /* FEATURE_UIM_TOOLKIT */

    case UIM_STREAM_APDU_ST:
      {

        if (cmd_ptr->hdr.command == UIM_STREAM_APDU_F)
        {
          /* Build an APDU to stream to the card */
          uim_req_buf_ptr->apdu_hdr.instrn = cmd_ptr->stream_apdu.instrn;
          uim_req_buf_ptr->apdu_hdr.uim_class = cmd_ptr->stream_apdu.uim_class;
          uim_req_buf_ptr->apdu_hdr.p1 = cmd_ptr->stream_apdu.p1;
          uim_req_buf_ptr->apdu_hdr.p2 = cmd_ptr->stream_apdu.p2;
          uim_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->stream_apdu.p3;

          /* We are ONLY going to deal with APDUs that have cmd data for now */
          uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;

          if (cmd_ptr->stream_apdu.p3 != 0)
          {
            memcpy(uim_req_buf_ptr->req.data, cmd_ptr->stream_apdu.data,
                   cmd_ptr->stream_apdu.p3);
          }
        }
        else
        {
          /* Init the status words */
          uim_get_resp_sw1 = (uim_sw1_type) 0;
          uim_get_resp_sw2 = (uim_sw2_type) 0;

          /* Init the get resp status words */
          uim_rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw1 = uim_get_resp_sw1;
          uim_rpt_buf.rpt.stream_iso7816_apdu.get_resp_sw2 = uim_get_resp_sw2;

          /* Build an APDU to stream to the card */
          uim_req_buf_ptr->apdu_hdr.uim_class =
            cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_CLASS_OFFSET];
          uim_req_buf_ptr->apdu_hdr.instrn =
            cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_INSTRN_OFFSET];
          uim_req_buf_ptr->apdu_hdr.p1 =
            cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_P1_OFFSET];
          uim_req_buf_ptr->apdu_hdr.p2 =
            cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_P2_OFFSET];

          /* Figure out the case for the commands */
          if (cmd_ptr->stream_iso7816_apdu.num_cmd_bytes ==
                                                     UIM_CASE1_7816_APDU_SIZE)
          {
            /* Case 1 - no input / no output */
            uim_req_buf_ptr->apdu_hdr.p3 = 0;
            uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_1;
          }
          else if (cmd_ptr->stream_iso7816_apdu.num_cmd_bytes ==
                                                     UIM_CASE2_7816_APDU_SIZE)
          {
            /* Case 2 - no input / output of known or unknown length */
            uim_req_buf_ptr->apdu_hdr.p3 =
              cmd_ptr->stream_iso7816_apdu.cmd_data[UIM_CASE2_7816_LE_OFFSET];
            uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_2;
          }
          else if (cmd_ptr->stream_iso7816_apdu.num_cmd_bytes ==
                   ( UIM_CASE2_7816_APDU_SIZE +
                    cmd_ptr->stream_iso7816_apdu.cmd_data
                      [UIM_CASE3_4_7816_LC_OFFSET]
                    ))
          {
            /* Case 3 - input / no ouput */
            uim_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->stream_iso7816_apdu.cmd_data
                                                   [UIM_CASE3_4_7816_LC_OFFSET];

            /* Copy the command bytes into the request buffer */
            memcpy(uim_req_buf_ptr->req.data,
                   cmd_ptr->stream_iso7816_apdu.cmd_data +
                                                      UIM_CASE2_7816_APDU_SIZE,
                   cmd_ptr-> stream_iso7816_apdu.cmd_data
                                                [UIM_CASE3_4_7816_LC_OFFSET]);

            uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_3;
          }
          else
          {
            /* Case 4 - input / output of known or unknown length */
            uim_req_buf_ptr->apdu_hdr.p3 = cmd_ptr->stream_iso7816_apdu.cmd_data
                                                   [UIM_CASE3_4_7816_LC_OFFSET];

            /* Copy the command bytes into the request buffer */
            memcpy(uim_req_buf_ptr->req.data,
                   cmd_ptr->stream_iso7816_apdu.cmd_data +
                                                      UIM_CASE2_7816_APDU_SIZE,
                   cmd_ptr-> stream_iso7816_apdu.cmd_data
                                                [UIM_CASE3_4_7816_LC_OFFSET]);

            uim_req_buf_ptr->instrn_case = UIM_INSTRN_CASE_4;
          }
        }

        /* Set the protocol based on the class of the command */
        if (uim_req_buf_ptr->apdu_hdr.uim_class != UIM_INSTRUCTION_CLASS)
        {
          uim_req_buf_ptr->protocol = UIM_UICC;
        }
        else
        {
          uim_req_buf_ptr->protocol = UIM_GSM;
        }

        uim_req_buf_ptr->rsp_ptr = uim_rsp_buf_ptr;

        uim_req_buf_ptr->rpt_function = uim_command_response_callback;

        uim_send_command(uim_req_buf_ptr);

      }
      break;

    case UIM_CLEAR_CACHE_ST:
      {
        /* Start with good status */
        uim_rsp_buf_ptr->cmd_status = UIM_DONE;

        if ( cmd_ptr->clear_cache.clr_cache_option ==
            UIM_OPTION_CLEAR_CACHE_ALL )
        {
          /* Clear the entire cache */
          uim_clear_get_response_cache();
        }
        else if ( cmd_ptr->clear_cache.clr_cache_option ==
                 UIM_OPTION_CLEAR_CACHE_SELECTIVE )
        {
          /* Clear selectively */
          if (cmd_ptr->clear_cache.num_items == 0 )
          {
            /* Number of items is 0 for selective clear */
            uim_rsp_buf_ptr->cmd_status = UIM_CMD_ERR_NO_INFO;
          }
          else if (cmd_ptr->clear_cache.item_list == NULL )
          {
            /* The item list is NULL report error */
            uim_rsp_buf_ptr->cmd_status = UIM_CMD_ERR_NO_INFO;
          }
          else
          {
            for (item_ctr = 0; item_ctr < cmd_ptr->clear_cache.num_items;
                  item_ctr++ )
            {
              /* Initiate to null so that it the item is not found, the
               * item remains NULL and not cleared multiple times
               */
              select_rsp_cache_item_ptr = NULL;

              (void) uim_get_file_path(
                       cmd_ptr->clear_cache.item_list[item_ctr],
                       NULL);

              if ( select_rsp_cache_item_ptr != NULL )
              {
                select_rsp_cache_item_ptr->data_size = 0;
                select_rsp_cache_item_ptr->is_cached = FALSE;
                if ( select_rsp_cache_item_ptr->data != NULL )
                {
                  uim_free ( select_rsp_cache_item_ptr->data );
                  select_rsp_cache_item_ptr->data = NULL;
                }
              } /* select_rsp_cache_item_ptr != NULL */
            } /* for each item in the item_list */
          } /* if list is valid and has non zero elements */
        } /* If selective clear */
        else
        {
          /* Un-known option */
          uim_rsp_buf_ptr->cmd_status = UIM_CMD_ERR_NO_INFO;
        }

        /* Nothing is to be done set the response signal */
        (void) rex_set_timer(&uim_delay_timer,0);
      }
      break;
    case UIM_RECOVERY_COMPLETE_ST:
      {
        /* Switch the mode back to the saved previous mode */
        uim_mode = uim_save_mode;

        /* set the response as done */
        uim_rsp_buf_ptr->cmd_status = UIM_DONE;

        /* Nothing is to be done set the response signal */
        (void) rex_set_timer(&uim_delay_timer,0);
      }
      break;

    case UIM_DONE_ST:                         /* Done State */
    {
      /* Process the response of the command. */
      uim_process_generic_response(uim_rsp_buf_ptr, cmd_ptr);
    } /* end case - UIM_DONE_ST */
      break;

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

    default:
      {
        /* Send the command to the custom command processing since this
           function does not process this command */
        uim_custom_generic_command(cmd_ptr);
      } /* end case - default */
      break;

  } /* end of main switch */

  MSG_LOW("UIM state in uim_command %ld",*uim_generic_state_ptr, 0,0);

} /* uim_generic_command */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GENERIC_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response to a generic comamnd that has been
  recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  Status which is of apdu_status_type

SIDE EFFECTS
  Changes the UIM state variable uim_generic_state_ptr.

===========================================================================*/

uim_cmd_status_type uim_generic_command_response
(
 uim_rsp_buf_type *rsp_ptr,
 uim_cmd_type     *cmd
)
{
  uim_cmd_status_type status = UIM_CMD_SUCCESS;
  /* check to see what the status from the UIM is */
  byte i;
  /* Index into the ATR buffer */
  byte check_byte;
  /* Check byte */
  byte rx_value;
  /* temp storage for recd byte while parsing ATR */
  byte num_hist_chars;
  /* number of historical characters */
  int num_bytes_to_skip;
  /* number of bytes to skip */
  uim_atr_rx_mode_type receive_mode;
  /* ATR Receieve mode */
  boolean check_byte_expected;
  /* Flag set to TRUE if we would receive a check byte */
  boolean td_present = FALSE;
  /* Index to TC1 byte of ATR */
  int uim_proc_atr_byte_index;
  /* Indicates whether to skip PPS */
  static boolean skip_PPS = TRUE;
  /* Flag to indicate if TD character was present */
  uim_generic_state_type curr_uim_generic_state;
  /* Normally, the status when a command is completed is UIM_CMD_COMPLETED
     This is changed if there is a proactive command pending. */
  uim_cmd_status_type completed_status = UIM_CMD_COMPLETED;
#ifdef FEATURE_UIM_UICC
  /* Path counter for use in selection by path ref */
  byte path_ctr;
  /* Aid for status response */
  uim_aid_type status_adf;
  /* File ID for status response */
  uim_dir_type status_file_id = 0;
#endif /* FEATURE_UIM_UICC */
  /* Holds index in the ATR buffer of the TD byte */
  int uim_atr_td_index;

  uim_clock_rate_conversion_factor_type  FI = UIM_CRCF_372_1; /* CRCF index */
  uim_baud_rate_adjustment_factor_type   DI = UIM_BRAF_1; /* BRAF index */

#ifdef FEATURE_UIM_HANDLE_TA1_0X97_ATR
  /* Variable to store the TD1 byte position in the ATR.
     TD1 byte indicates if TA2 byte is present in the ATR or not.
     The TA2 byte would indicates the mode of the SIM. */
  byte uim_atr_td1_index = 0;
  /* Variable to store the TA2 byte.
     The TA2 byte indicates the mode of the SIM */
  byte uim_atr_ta2_byte = 0;
  /* Boolean variable to specific mode check */
  boolean skip_specific_mode_check = FALSE;
  /* Boolean variable to check for mode changeability */
  boolean skip_mode_changeability_check = FALSE;
  /* Boolean variable to indicate if TA2 byte is present in ATR */
  boolean uim_atr_ta2_byte_present = FALSE;
  /* Boolean variable to indicate the Bit 8 of the TA2 byte.
     If b8 bit of TA2 is set (TRUE), the SIM doesn't allow to change
     the mode.
     If b8 bit of TA2 is not set (FALSE), the SIM allows to change
     the mode. */
  boolean uim_atr_b8_bit_ta2 = FALSE;
 /*  Boolean variable to indicate the Bit 5 of the TA2 byte.
     If b5 bit of TA2 is set (TRUE), the baud-rate of the SIM is
     implicitly defined (default baud-rate).
     If b5 bit of TA2 is not set (FALSE), the baud-rate of the SIM is
     defined by the interface bytes. */
  boolean uim_atr_b5_bit_ta2 = FALSE;
#endif /* FEATURE_UIM_HANDLE_TA1_0X97_ATR */

#if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES ) && defined( FEATURE_UIM_UICC )
  /* Indicates whether we have the voltage class information present */
  boolean voltage_class_indicator_scan = FALSE;
  /* Maintains the number of the TDs that we have processed */
  byte number_of_td = 0;
  /* Holds the UI (voltage class indicator) value found in TA after T=15 */
  byte voltage_class_indicator;
#endif /* FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES && FEATURE_UIM_UICC */

#ifdef FEATURE_UIM_SUPPORT_CSIM
  /* Variable to indicate whether the selection was the
   * externally streamed select or an internal select required for
   * streaming the CSIM APDU
   */
  static boolean internal_sel_for_7816 = FALSE;
#endif /* FEATURE_UIM_SUPPORT_CSIM */

  if (rsp_ptr == NULL)
  {
    ERR_FATAL("uim_generic_command_response rsp_ptr is NULL",0,0,0);
  }

  if(cmd == NULL)
  {
    ERR_FATAL("uim_generic_command_response cmd is NULL",0,0,0);
  }

  MSG_HIGH("SW1 0x%x,SW2 0x%x, status %d",
           rsp_ptr->sw1, rsp_ptr->sw2, rsp_ptr->cmd_status);
  MSG_HIGH("generic_state %d, uim_reselect_mf %d",
           *(uim_generic_state_ptr), uim_reselect_mf,0);

  if ((*uim_generic_state_ptr != UIM_POWER_UP_ST) &&
         (*uim_generic_state_ptr != UIM_UPDATE_OP_PARAMS_ST))
  {
    /* UIM_PARITY is used to indicate an error from the UIM */
    if ( rsp_ptr->cmd_status == UIM_PARITY )
    {
      /* Fill in the status words that we did not understand.. the
       * client can look into it
       */

      uim_rpt_buf.sw1 = (uim_sw1_type) rsp_ptr->sw1;
      uim_rpt_buf.sw2 = (uim_sw2_type) rsp_ptr->sw2;

#ifdef FEATURE_UIM_RPT_BAD_SW_ON_POLL
      if (cmd->hdr.command == UIM_INTERNAL_POLL_F)
      {
        MSG_ERROR("Unknown status words while polling - SW1, SW2",
                  rsp_ptr->sw1, rsp_ptr->sw2, 0);
        return(UIM_POLL_ERROR);
      }
#endif
      MSG_ERROR("Received unknown status words - SW1, SW2",
                rsp_ptr->sw1, rsp_ptr->sw2, 0);

       /* If we get a parity error, we would like to treat it as a command
          error */
      MSG_ERROR("Returning error since cmd_status = UIM_PARITY",0,0,0);
      /* If we get a parity error, we would like to treat it as a command
         error */
      return(UIM_CMD_ERR);
    } /* end if - cmd_status == UIM_PARITY */
  } /* (uim_generic_state_ptr != UIM_POWER_UP_ST) &&
       (uim_generic_state_ptr != UIM_UPDATE_OP_PARAMS_ST) */

  /* just finished the select on MF after a previous failure */
  /* return at this point so that the uim_rpt_buf contains the */
  /* failure information that can be reported back to the caller */
  if (uim_reselect_mf)
  {
    if (rsp_ptr->cmd_status != UIM_DONE)
    {
      MSG_ERROR("Select on MF during resetting UIM dir fail", 0, 0, 0);
    }
    else
    {
      status = uim_update_last_sel_dir( rsp_ptr,
#ifdef FEATURE_UIM_UICC
                                        cmd,
#endif /* FEATURE_UIM_UICC */
                                        uim_path_to_be_sel[UIM_PATH_TO_SEL_MF_INDEX]);
#ifdef FEATURE_UIM_SUPPORT_CSIM
      if ( cmd->hdr.command == UIM_STREAM_ISO7816_APDU_F &&
           internal_sel_for_7816 )
      {
        /* This is an internal select for CSIM which failed
         * should never have had failed */
        MSG_ERROR("Internal select for CSIM failed setting to MF", 0, 0, 0);

        /* Assign the CSIM to the last selection -- MF */
        uim_csim_last_sel_dir[uim_drv_slot] =
          uim_last_sel_dir[uim_cur_channel][uim_drv_slot];
      }
#endif /*FEATURE_UIM_SUPPORT_CSIM*/
      MSG_HIGH("Returning ERR for select after re-selecting MF",0,0,0);
    } /* response is done */
    uim_reselect_mf = FALSE;

    return UIM_CMD_ERR;
  } /* reselect mf */

  /* Fill the status words into the report buffer. This could be sent for
     both passed and failed reports. If the command is not sent to the
     card, then the status words are cleared before sending the report */

  uim_rpt_buf.sw1 = (uim_sw1_type) rsp_ptr->sw1;
  uim_rpt_buf.sw2 = (uim_sw2_type) rsp_ptr->sw2;

  /* Return command error immediately when SIM returned a
     technical problem */
  if( uim_rpt_buf.sw1 == (uim_sw1_type) SW1_PROBLEM )
  {
    /* Return an error status if the command failed. */
    return (UIM_CMD_ERR);
  }

#ifdef FEATURE_UIM_TOOLKIT
  /* Change fetch responses to normal end responses only if we are not in
   * passive mode.  In passive mode, it is the client's responsibility
   * to do a fetch based on the status words reported
   * Let us not waste time trying to fetch while recovering.
   */
  if ( (SW1_END_FETCH == uim_rpt_buf.sw1) && !(uim_mode == UIM_PASSIVE_MODE) &&
       !(uim_mode == UIM_RECOVERY_MODE) )
  {
    /* Store the words for streaming result */
    /* Do not change the response to fetch response in case of warnings for envelope 
       command, to notify warnings to gstk */
    if(!(((uim_get_resp_sw1 == SW1_WARNINGS1) && 
            (*uim_generic_state_ptr == UIM_ENVELOPE_ST)) ||
         ((uim_get_resp_sw1 == SW1_WARNINGS2) && 
            (*uim_generic_state_ptr == UIM_ENVELOPE_ST))))
    {
      uim_get_resp_sw1 = uim_rpt_buf.sw1;
      uim_get_resp_sw2 = uim_rpt_buf.sw2;
    }

    uim_rpt_buf.sw1 = SW1_NORMAL_END;
    uim_rpt_buf.sw2 = SW2_NORMAL_END;
    /* If the command is completed, indicate there is a proactive command
       pending. */
    completed_status = UIM_CMD_FETCH;

    /* Set the protocol for the Internal FETCH command based on the last
       successful command */
    uim_static_cmd_buf.hdr.protocol = cmd->hdr.protocol;

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
    /* Set the slot for the command based on the slot in which the status
       words were received */
    uim_static_cmd_buf.hdr.slot = uim_toolkit_slot;
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

  } /* end if - fetch response */
#endif /* FEATURE_UIM_TOOLKIT */

  /* Necessary so that we do not change the uim state and switch into that
     case also */
  curr_uim_generic_state = *uim_generic_state_ptr;

  switch (curr_uim_generic_state)
  {
    case UIM_POWER_DOWN_ST:
      {
        ++uim_generic_state_ptr;
      }
      break;
    case UIM_POWER_UP_ST:                     /* Power Up state */
      {
        /* Get the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_POWER_UP_ST */
      break;

    case UIM_RESET_ST:                        /* Reset State */
      {
        /* variable is static so re-initialize */
        skip_PPS = TRUE;
          /* Check to see if there was a convention error */
        if (rsp_ptr->cmd_status == UIM_CONVENTION_ERROR)
        {
            /* If the number of times we are switching between direct & inverse
               convention is greater than the max number of attempts */
            if (++uim_convention_change_count > UIM_MAX_NUM_ATTEMPTS)
            {
              /* Return a cmd error after setting the command count to MAX */
              cmd->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
              return(UIM_CMD_ERR);
            }
            else
            {
              /* Return a convention changes error that will not increment the
              wake-up or power-up command count */
              return(UIM_CONVENTION_CHANGE);
            }
        }
        /* Check if there was a problem with the Reset */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return(UIM_CMD_ERR);
        } /* end if - there was a problem with the Reset */

        uim_atr_buf[uim_drv_slot].num_bytes = (byte)rsp_ptr->cmd_rsp_size;
        memcpy(uim_atr_buf[uim_drv_slot].data, rsp_ptr->rsp.data,
               rsp_ptr->cmd_rsp_size);

        /* Validate the ATR here */
        /* Check for an intelligible start byte */
        if ((uim_atr_buf[uim_drv_slot].data[UIM_ATR_TS_BYTE_INDEX] !=
                                                    UIM_ATR_TS_DIRECT_CONV) &&
            (uim_atr_buf[uim_drv_slot].data[UIM_ATR_TS_BYTE_INDEX] !=
                                                    UIM_ATR_TS_INVERSE_CONV))
        {
          return(UIM_CMD_ERR);
        }

        receive_mode = UIM_WAITING_FOR_T0;

        num_bytes_to_skip = 2;

        /* Assume there are no historical characters */
        num_hist_chars = 0;
        uim_hist_char_buf[uim_drv_slot].length = num_hist_chars;

        /* Initialize the check byte to TS */
        check_byte = uim_atr_buf[uim_drv_slot].data[0];

        /* Process all the bytes in the receive FIFO */
        for (i = 0;i < uim_atr_buf[uim_drv_slot].num_bytes;i++)
        {
          /* Get the next received byte. */
          rx_value = uim_atr_buf[uim_drv_slot].data[i];

          /* Keep a rolling value of the check byte */
          check_byte ^= rx_value;

          num_bytes_to_skip--;

#if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES ) && defined( FEATURE_UIM_UICC )
          if ( voltage_class_indicator_scan == TRUE)
          {
            voltage_class_indicator_scan = FALSE;
            voltage_class_indicator = ( rx_value & UIM_UI_MASK );
            voltage_class_known_from_atr = TRUE;

            /* Determine the current voltage on the interface */
            switch (uim_current_voltage_class[uim_drv_slot])
            {
              case UIM_VOLTAGE_1_8V:
                {
                  /* Check if the card does not support this voltage */
                  if ((voltage_class_indicator & UIM_UI_1_8_UIM_UICC) !=
                      UIM_UI_1_8_UIM_UICC)
                  {
                    MSG_LOW("Card does not support 1.8V",0,0,0);
                    /* Check if card supports other supported voltages */
                    if ((voltage_class_indicator & UIM_UI_3V_UIM_UICC) ==
                        UIM_UI_3V_UIM_UICC)
                    {
                      MSG_HIGH("Turning off the interface to start in 3V mode",0,0,0);
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
                      /* Switch to different voltage */
                      /* Power down the interface */
                      uim_power_down();
#else
                      /* Switch to different voltage */
                      /* Power down the interface */
                      uim_power_down(cmd->hdr.slot);
#endif
                      rex_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY);
                      /* Try the indicated voltage class */
                      uim_current_voltage_class[uim_drv_slot] = UIM_VOLTAGE_3V;
                      return(UIM_SWITCH_VOLTAGE);
                    }
                    else /* No voltages supported */
                    {
                      MSG_HIGH("Card does not support 3V mode either",0,0,0);
                      return(UIM_VOLTAGE_MISMATCH);
                    } /* end if - card supports other voltage */
                  } /* end if - card does not support this voltage */
                } /* end case - UIM_VOLTAGE_1_8V */
                break;

              case UIM_VOLTAGE_3V:
                {
                  /* Check if the card does not support this voltage */
                  if ((voltage_class_indicator & UIM_UI_3V_UIM_UICC) !=
                      UIM_UI_3V_UIM_UICC)
                  {
                    MSG_HIGH("Card does not support 3V",0,0,0);
                    return(UIM_VOLTAGE_MISMATCH);
                  } /* end if - card does not support this voltage */
                } /* end case - UIM_VOLTAGE_3V*/
                break;

              default:
                {
                  ERR( "Bad UIM voltage class", 0, 0, 0 );
                  return(UIM_VOLTAGE_MISMATCH);
                } /* end case - default */
            } /* end switch - voltage class */
          } /* voltage_class_indicator_scan */
#endif

          /* Determine if this byte should be processed. */
          if (num_bytes_to_skip == 0)
          {
            /* Process the byte based on what is expected by this function */
            switch (receive_mode)
            {
              case UIM_WAITING_FOR_T0:    /* Expecting the T0 character */
                {
                  /* Save the number of historical bytes */
                  num_hist_chars = (rx_value & UIM_K_FIELD);

                  /* Determine how many bytes follow this format byte. */
                  num_bytes_to_skip =
                    uim_y_format_table[rx_value >> UIM_Y_FIELD_SHIFT];

                  /* Determine if there will be global interface bytes.*/
                  if (rx_value & UIM_TD_PRESENT)
                  {
                    receive_mode = UIM_WAITING_FOR_TD;
                  }
                  else
                  {
                    /* The number of bytes to skip include the number of
                    bytes that follow this format character and the
                    number of historical characters. */
                    num_bytes_to_skip += num_hist_chars;

                    /* There will NOT be a TCK.  Determine if any bytes
                       follow in this ATR.*/
                    if (num_bytes_to_skip > 0)
                    {
                      /* Wait for the rest of the bytes of the ATR.*/
                      receive_mode = UIM_WAITING_FOR_END;
                    } /* end if - more bytes to skip. */
                  } /* end if - TD byte is present in following bytes */
                } /* end case - UIM_WAITING_FOR_T0 */
                break;

              case UIM_WAITING_FOR_TD:    /* Expecting a TD character */
                {
                  td_present = TRUE;
#if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES ) && defined( FEATURE_UIM_UICC )
                  number_of_td++;
#endif /* FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES && FEATURE_UIM_UICC */

                  /* Determine how many bytes follow this format byte.*/
                  num_bytes_to_skip =
                    uim_y_format_table[rx_value >> UIM_Y_FIELD_SHIFT];

                  /* Determine if there is a protocol other than 0. */
                  if ((rx_value & UIM_T_FIELD) == (int)UIM_T_0_PROTOCOL)
                  {
                    uim_prot_sup_by_card.t_0 = TRUE;
                    check_byte_expected = FALSE;
                  }
#if defined( FEATURE_UIM_T_1_SUPPORT )
                  else if ((rx_value & UIM_T_FIELD) == (int)UIM_T_1_PROTOCOL)
                  {
                    uim_prot_sup_by_card.t_1 = TRUE;
                    check_byte_expected = TRUE;
                  }
#endif /* FEATURE_UIM_T_1_SUPPORT */
                  else
                  {
                    /* This indicates the TCK byte is expected */
                    check_byte_expected = TRUE;
                  } /* end if - a protocol other than 0 */

#if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES ) && defined ( FEATURE_UIM_UICC )
                  /* check if we have TA(i) after the first occurrence of T=15 in TD(i-1)
                  for i > 2 (which means we have XI and UI ) */
                  if ( ((rx_value & UIM_T_FIELD) == UIM_T_FIELD) &&
                       (rx_value & UIM_ATR_TA_PRESENT) &&
                       (number_of_td >= 2) &&
                       (cmd->hdr.protocol == UIM_UICC) )
                  {
                    /* The next TA byte has the voltage class information we
                       need to process it */
                    voltage_class_indicator_scan = TRUE;
                  }
#endif
                  /* Determine if there will be another interface byte.*/
                  if (rx_value & UIM_TD_PRESENT)
                  {
                    receive_mode = UIM_WAITING_FOR_TD;
                  }
                  else
                  {
                    /* Determine if the check byte is expected */
                    if (check_byte_expected)
                    {
                      /* The number of bytes to skip include the
                      number of bytes that follow this format char
                      and the number of historical characters.The
                      check byte is also included.*/
                      num_bytes_to_skip += num_hist_chars + 1;

                      /* Wait for the check byte.*/
                      receive_mode = UIM_WAITING_FOR_TCK;
                    }
                    else /* No check byte is expected */
                    {
                      /* The number of bytes to skip include the num of
                      bytes that follow this format character and the
                      number of historical characters.*/
                      num_bytes_to_skip += num_hist_chars;

                      /* There will NOT be a TCK.  Determine if any
                         bytes follow in this ATR.*/
                      if (num_bytes_to_skip > 0)
                      {
                        /* Wait for the rest of bytes of the ATR.*/
                        receive_mode = UIM_WAITING_FOR_END;
                      }
                      else /* There are no more bytes in the ATR. */
                      {

                      } /* end if - more bytes to come.*/
                    } /* end if - more bytes to come.*/
                  } /* end if - TD byte is present in following bytes */

                } /* end case - UIM_WAITING_FOR_TD */
                break;

              case UIM_WAITING_FOR_TCK:
                {
                  /* Expecting the TCK character.  The ATR should have
                     been completely received.  No other bytes are
                     expected at this time */

                  /* Determine if the ATR was received with an error */
                  if (check_byte != 0)
                  {
                    /* We have recd an incorrect ATR */
                    return (UIM_CMD_ERR);
                  } /* end if - received an ATR with an error. */

                  /* Copy the historical characters to the historical characters
                     buffer. */
                  uim_hist_char_buf[uim_drv_slot].length = num_hist_chars;

                  /* This addresses the first historical character.
                     This takes into account the TCK byte at the end of
                     the buffer. */
                  memcpy (uim_hist_char_buf[uim_drv_slot].data,
                          (byte*) &(uim_atr_buf[uim_drv_slot].
                                                    data[(i - num_hist_chars)]),
                          num_hist_chars);

                } /* end case - UIM_WAITING_FOR_TCK */
                break;

              case UIM_WAITING_FOR_END:
                {
                  /* Expecting the last of the ATR with NO TCK char expected
                     The ATR should have been completely received. No other
                     bytes are expected at this time */

                  /* Copy the historical characters to the historical char
                     buffer. */
                  uim_hist_char_buf[uim_drv_slot].length = num_hist_chars;

                  /* This addresses the first historical character. The index
                     of the first historical character is adjusted by one since
                     i is the index of the last historical character. */
                  memcpy (uim_hist_char_buf[uim_drv_slot].data,
                          (byte*) &(uim_atr_buf[uim_drv_slot].
                                                    data[(i-num_hist_chars)+1]),
                          num_hist_chars);
                }
                break;

              default:
                {

                }
                break;
            } /* end switch - receive_mode */
          } /* end if - this function should process this byte */
        } /* end while - process all the bytes in the receive FIFO. */

#if defined( FEATURE_UIM_T_1_SUPPORT )
        /* If the preferred protocol is T = 1 */
        if (uim_nv_pref_protocol == NV_UIM_PREF_PROTOCOL_T_1)
        {
          /* If the card supports T=1 protocol */
          if (uim_prot_sup_by_card.t_1)
          {
            /* Set the current protocol to T = 1 */
            #ifdef FEATURE_UIM_FORCE_T_0_PROTOCOL
              uim_operating_protocol = UIM_T_0_PROTOCOL;
            #else
              uim_operating_protocol = UIM_T_1_PROTOCOL;
            #endif /* FEATURE_UIM_FORCE_T_0_PROTOCOL */
            /* Determine if T=0 is supported also */
            if (uim_prot_sup_by_card.t_0)
            {
              /* Do not skip the PPS since there are two protocols supported
                 and we will use T=1 */
              skip_PPS = FALSE;
            }/* end if - T=0 is also supported */
          }
          else if ((uim_prot_sup_by_card.t_0) || (!td_present))
          {
            /* Set the current protocol to T = 0 */
            uim_operating_protocol = UIM_T_0_PROTOCOL;
          }
          else
          {
            /* Return an error */
            uim_atr_buf[uim_drv_slot].num_bytes = 0;
            return (UIM_CMD_ERR);
          }
        }
        /* Else if the preferred protocol is T=0 or No preference */
        else if ((uim_nv_pref_protocol == NV_UIM_PREF_PROTOCOL_T_0) ||
                 (uim_nv_pref_protocol == NV_UIM_PREF_PROTOCOL_NONE))
        {
          if ((uim_prot_sup_by_card.t_0) || (!td_present))
          {
            /* Set the current protocol to T = 0 */
            uim_operating_protocol = UIM_T_0_PROTOCOL;
          }
          else if (uim_prot_sup_by_card.t_1)
          {
            /* Set the current protocol to T = 1 */
            #ifdef FEATURE_UIM_FORCE_T_0_PROTOCOL
              uim_operating_protocol = UIM_T_0_PROTOCOL;
            #else
              uim_operating_protocol = UIM_T_1_PROTOCOL;
            #endif /* FEATURE_UIM_FORCE_T_0_PROTOCOL */
          }
          else
          {
            /* Return an error for ATR */
            uim_atr_buf[uim_drv_slot].num_bytes = 0;
            return (UIM_CMD_ERR);
          }
        }
#else
        if ((td_present) && (!uim_prot_sup_by_card.t_0))
        {
          uim_atr_buf[uim_drv_slot].num_bytes = 0;
          return (UIM_CMD_ERR);
        }
        else
#endif /* FEATURE_UIM_T_1_SUPPORT */
        {
#ifdef FEATURE_UIM_HANDLE_TA1_0X97_ATR
          /* Check for TD1 byte */
          if (uim_atr_buf[uim_drv_slot].data[UIM_ATR_T0_BYTE] &
              (UIM_TD_PRESENT))
          {
            /* Calculate the TD1 index in the ATR */
            uim_atr_td1_index = UIM_ATR_T0_BYTE +
             uim_y_format_table[uim_atr_buf[uim_drv_slot].data[UIM_ATR_T0_BYTE]
               >> UIM_Y_FIELD_SHIFT];

            /* TA2 is present */
            if(uim_atr_buf[uim_drv_slot].data[uim_atr_td1_index]
               & UIM_ATR_TA_PRESENT)
            {
              /* Indicate that the TA2 is present */
              uim_atr_ta2_byte_present = TRUE;

              /* Get the TA2 byte */
              uim_atr_ta2_byte =
                 uim_atr_buf[uim_drv_slot].data[uim_atr_td1_index + 1];

              /* Is B8 bit in TA2 is set */
              if(uim_atr_ta2_byte & UIM_BIT8_CHECK)
              {
                /* B8 bit in TA2 is set */
                uim_atr_b8_bit_ta2 = TRUE;
              }

              /* Is B5 bit in TA2 is set */
              if(uim_atr_ta2_byte & UIM_BIT5_CHECK)
              {
                /* B5 bit in TA2 is set */
                uim_atr_b5_bit_ta2 = TRUE;
              }
            }
          }
#endif /* FEATURE_UIM_HANDLE_TA1_0X97_ATR */

          /* The TC1 byte may be in the first byte after T0 */
          uim_proc_atr_byte_index = UIM_ATR_TA1_BYTE;

          /* If TA1 is present, we want to do PPS, only if TA1 is not equal to
            '11' or '01' */
          if (uim_atr_buf[uim_drv_slot].data[UIM_ATR_T0_BYTE] &
                                                         (UIM_ATR_TA_PRESENT))
          {
            /* Count the number of bytes to TC1 */
            uim_proc_atr_byte_index++;

            /* Check TA1 for default values */
            if (((uim_atr_buf[uim_drv_slot].data[UIM_ATR_TA1_BYTE]) !=
                                                         UIM_NO_PPS_VALUE1) &&
                ((uim_atr_buf[uim_drv_slot].data[UIM_ATR_TA1_BYTE]) !=
                                                         UIM_NO_PPS_VALUE2))
            {
              /* Do not skip PPS.  Use these parameters */
              skip_PPS = FALSE;

              /* Check if we can handle these parameters */
              FI = (uim_clock_rate_conversion_factor_type)
                (uim_rsp_buf_ptr->rsp.data[UIM_TA1_INDEX]
                   >> UIM_FI_SHIFT_OF_TA1);
              DI = (uim_baud_rate_adjustment_factor_type)
                (uim_rsp_buf_ptr->rsp.data[UIM_TA1_INDEX]
                   & UIM_DI_MASK_OF_TA1);

#ifndef FEATURE_UIM_HANDLE_TA1_0X97_ATR
              /* Determine the index of the highest supported D by searching
                 the braf_values[] array. Note: Certain D values which are
                 "RFU" per spec and coded as 0s in the array are skipped */
              while ((braf_values[DI] >
                         UIM_MAX_BRAF[uim_max_freq_supported[uim_drv_slot]][FI])
                     || (((int)DI > 0) && (braf_values[DI] == 0)))
              {
                --DI;
              } /* end while - search for suitable BRAF value */
#endif /* FEATURE_UIM_HANDLE_TA1_0X97_ATR */

              /* Place parameters back into the ATR for processing of the
                 PPS */
              uim_rsp_buf_ptr->rsp.data[UIM_TA1_INDEX] =
                (uim_rsp_buf_ptr->rsp.data[UIM_TA1_INDEX] & (~UIM_DI_MASK_OF_TA1))
                | (int)DI;
            } /* Check if TA1 values need a PPS */
#ifdef FEATURE_UIM_HANDLE_TA1_0X97_ATR
            else
            {
              /* Skip the PPS */
              skip_PPS = TRUE;
              /* Skip the subsequent specific mode check */
              skip_specific_mode_check = TRUE;
            }
#endif /* FEATURE_UIM_HANDLE_TA1_0X97_ATR */
          } /* check for TA1 */
#ifdef FEATURE_UIM_HANDLE_TA1_0X97_ATR
          else
          {
            /* Skip the PPS */
            skip_PPS = TRUE;
            /* Skip the subsequent specific mode check */
            skip_specific_mode_check = TRUE;
          }

          /* Enter the specific mode check */
          if( skip_specific_mode_check == FALSE)
          {
            /* Is SIM in specific mode? */
            if( uim_atr_ta2_byte_present == TRUE )
            {
              // Don't perform PPS as the SIM is in specific mode
              skip_PPS = TRUE;

              /* Is TA1 supported by the platform? */
              if( uim_is_fi_di_supported(FI, DI) )
              {
                /* Check if b5 bit is set in TA2 */
                if(uim_atr_b5_bit_ta2 == FALSE)
                {
                  skip_mode_changeability_check = TRUE;
                }
                else
                {
                  /* Use implicit values for F and D.  Cause this by
                  indicating TA1 is not in the ATR */
                  uim_rsp_buf_ptr->rsp.data[UIM_ATR_T0_BYTE] &= ~UIM_ATR_TA_PRESENT;
                }
              }

              /* Enter the mode changeability check */
              if(skip_mode_changeability_check != TRUE)
              {
                /* Check if mode change is possible */
                if(uim_atr_b8_bit_ta2 == FALSE)
                {
                  /* Check if we had done warm reset earlier within the same powerup */
                  if(warm_reset_count > 0)
                  {
                    /* Set this to 0 as this is a static variable */
                    warm_reset_count = 0;

                    /* Is Implicit baud-rate possible? */
                    if(uim_atr_b5_bit_ta2 == TRUE)
                    {
                      /* Use implicit values for F and D.  Cause this by
                      indicating TA1 is not in the ATR */
                      uim_rsp_buf_ptr->rsp.data[UIM_ATR_T0_BYTE] &= ~UIM_ATR_TA_PRESENT;
                    }
                    else
                    {
                      /* Return error as this SIM is not useable */
                      /* Convert this to MSG_ERROR, if need be */
                      MSG_HIGH("THIS SIM IS NOT USEABLE BY OUR TERMINAL DEVICE",0,0,0);
                      uim_atr_buf[uim_drv_slot].num_bytes = 0;
                      return (UIM_CMD_ERR);
                    }
                  }
                  else
                  {
  #ifdef FEATURE_UIM_ENABLE_SIM_MODE_CHANGE_VIA_WARM_RESET
                    warm_reset_count++;
  #else /* ! FEATURE_UIM_ENABLE_SIM_MODE_CHANGE_VIA_WARM_RESET */
                    if(uim_atr_b5_bit_ta2 == TRUE)
                    {
                      /* Use implicit values for F and D.  Cause this by
                      indicating TA1 is not in the ATR */
                      uim_rsp_buf_ptr->rsp.data[UIM_ATR_T0_BYTE] &= ~UIM_ATR_TA_PRESENT;
                    }
                    else
                    {
                      /* Return error as this SIM is not useable */
                      MSG_HIGH("THIS SIM IS NOT USEABLE BY OUR TERMINAL DEVICE",0,0,0);
                      uim_atr_buf[uim_drv_slot].num_bytes = 0;
                      return (UIM_CMD_ERR);
                    }
  #endif /* FEATURE_UIM_ENABLE_SIM_MODE_CHANGE_VIA_WARM_RESET */
                  }
                }
                else /* Mode change is not possible */
                {
                  /* Reset the warm reset count to 0 */
                  warm_reset_count = 0;

                  /* Is B5 bit in TA2 byte set? */
                  if(uim_atr_b5_bit_ta2 == TRUE)
                  {
                    /* Use implicit values for F and D.  Cause this by
                    indicating TA1 is not in the ATR */
                    uim_rsp_buf_ptr->rsp.data[UIM_ATR_T0_BYTE] &= ~UIM_ATR_TA_PRESENT;
                  }
                  else
                  {
                    /* Return error as this SIM is not useable */
                    MSG_HIGH("THIS SIM IS NOT USEABLE BY OUR TERMINAL DEVICE",0,0,0);
                    uim_atr_buf[uim_drv_slot].num_bytes = 0;
                    return (UIM_CMD_ERR);
                  }
                }
              } /* skip_mode_changeability_check */
              else
              {
                warm_reset_count = 0;
              }
            }
            else /* SIM is not in specific mode */
            {
              warm_reset_count = 0;
              skip_PPS = FALSE;

              /* Is TA1 supported by the platform? */
              if( uim_is_fi_di_supported(FI, DI) )
              {
                skip_PPS = FALSE;
              }
              else /* TA1 not supported by the platform */
              {
                /* Fall back to 0x96 */
                DI = UIM_BRAF_32;
                /* Place parameters back into the ATR for processing of the
                   PPS */
                uim_rsp_buf_ptr->rsp.data[UIM_TA1_INDEX] =
                  (uim_rsp_buf_ptr->rsp.data[UIM_TA1_INDEX] & (~UIM_DI_MASK_OF_TA1))
                  | (int)DI;

                /* Perform PPS */
                skip_PPS = FALSE;
              }

            } /* else */
          } /* if skip_specific_mode_check ... */
          else
          {
            /* set the warm reset count to 0 */
            warm_reset_count = 0;
          }
#endif /* FEATURE_UIM_HANDLE_TA1_0X97_ATR */

          /* Now check if TB1 is present */
          if (uim_atr_buf[uim_drv_slot].data[UIM_ATR_T0_BYTE] &
                                                          (UIM_ATR_TB_PRESENT))
          {
            /* Count the number of bytes to TC1 */
            uim_proc_atr_byte_index++;
          }

          /* If TC1 is present, check to see if it is within acceptable
             values */
          if (uim_atr_buf[uim_drv_slot].data[UIM_ATR_T0_BYTE] &
                                                         (UIM_ATR_TC_PRESENT))
          {
            /* Check if TC1 is acceptable */
            if ((uim_atr_buf[uim_drv_slot].data[uim_proc_atr_byte_index]) !=
                                                                  UIM_TC1_IS_0)
            {
              if ((uim_atr_buf[uim_drv_slot].data[uim_proc_atr_byte_index]) !=
                   UIM_TC1_IS_255)
              {
                /* Fail the ATR from the SIM */
                uim_atr_buf[uim_drv_slot].num_bytes = 0;
                return (UIM_CMD_ERR);
              }
              else /* TC1 is the minimum value */
              {
                /* Use the minimum guardtime size */
                uim_use_minimum_guardtime_size = TRUE;
              } /* end if - TC1 is not within the acceptable range */
            } /* end if - TC1 is not 0 */
          } /* end if - TC1 is present */

          /* Check if TD1 is present */
          if (uim_atr_buf[uim_drv_slot].data[UIM_ATR_T0_BYTE] & (UIM_TD_PRESENT))
          {
            /* Determine the TD1 index */
            uim_atr_td_index = UIM_ATR_T0_BYTE +
              uim_y_format_table[uim_atr_buf[uim_drv_slot].data
                [UIM_ATR_T0_BYTE] >> UIM_Y_FIELD_SHIFT];

            /* Move the TD2 index to process the bytes after TD2 */
            uim_proc_atr_byte_index = uim_atr_td_index;

            /* Check if TA2 is present by checking TD1 */
            if (uim_atr_buf[uim_drv_slot].data[uim_atr_td_index] &
                 UIM_ATR_TA_PRESENT)
            {
              /* TA2 is the specific mode byte */
              ++uim_proc_atr_byte_index;
#ifndef FEATURE_UIM_HANDLE_TA1_0X97_ATR
              /* Check if implicit values are to be used for the protocol
                 parameters */
              if (uim_atr_buf[uim_drv_slot].data[uim_proc_atr_byte_index] &
                  UIM_ATR_TA2_IND)
              {
                /* Use implicit values for F and D.  Cause this by
                indicating TA1 is not in the ATR */
                uim_atr_buf[uim_drv_slot].data[UIM_ATR_T0_BYTE] &=
                                                            ~UIM_ATR_TA_PRESENT;
              } /* end if - implicit values are to be used for the protocol
                   parameters */
#endif /* FEATURE_UIM_HANDLE_TA1_0X97_ATR */

#if defined( FEATURE_UIM_T_1_SUPPORT )
              /* Set the current protocol to the specific mode */
              uim_operating_protocol =
                (uim_transport_protocol_type)
                (uim_atr_buf[uim_drv_slot].data[uim_proc_atr_byte_index] &
                                                            UIM_T_FIELD);
#endif /* FEATURE_UIM_T_1_SUPPORT */

#ifndef FEATURE_UIM_HANDLE_TA1_0X97_ATR
              /* Make sure to skip the PPS since this is specific mode */
              skip_PPS = TRUE;
#endif /* FEATURE_UIM_HANDLE_TA1_0X97_ATR */
            } /* end if - TA2 is present */

            /* Check if TB2 is present by checking TD1 */
            if (uim_atr_buf[uim_drv_slot].data[uim_atr_td_index] &
                 UIM_ATR_TB_PRESENT)
            {
              /* Do not process TB2.  Skip to next byte */
              ++uim_proc_atr_byte_index;
            } /* end if - TB2 is present */

            /* Check if TC2 is present by checking TD1 */
            if (uim_atr_buf[uim_drv_slot].data[uim_atr_td_index] &
                 (UIM_ATR_TC_PRESENT))
            {
              /* Determine the TC2 index.  Consider only those bytes to TC2 */
              /* Save the work waiting time */
              uim_WI[uim_drv_slot] =
                uim_atr_buf[uim_drv_slot].data[++uim_proc_atr_byte_index];
            } /* end if - TC2 is present */

#if defined( FEATURE_UIM_T_1_SUPPORT )
            /* Set initial values of send sequence numbers */
            uim_me_send_sequence_num = UIM_T_1_INIT_SEQ_NUM;
            uim_card_send_sequence_num = UIM_T_1_INIT_SEQ_NUM;

            /* Set default values of IFS */
            uim_t_1_ifsc = UIM_T_1_DEFAULT_IFS;
            uim_t_1_ifsd = UIM_T_1_DEFAULT_IFS;

            /* Set the default waiting times indeces for T=1 protocol */
            uim_t_1_bwi = UIM_T_1_DEFAULT_BWI;
            uim_t_1_cwi = UIM_T_1_DEFAULT_CWI;

            /* Set the default EDC controls */
            uim_t_1_edc = UIM_T_1_EDC_LRC;
            uim_epilogue_field_size = UIM_T_1_LRC_SIZE;

            /* Check if TD2 is present by checking TD1 */
            if (uim_atr_buf[uim_drv_slot].data[uim_atr_td_index] &
                                                              (UIM_TD_PRESENT))
            {
              /* Determine the TD2 index. Consider only those bytes to TD2*/
              uim_atr_td_index +=
              uim_y_format_table[(uim_atr_buf[uim_drv_slot].data
                [uim_atr_td_index] & 0xF0) >> UIM_Y_FIELD_SHIFT];

              /* Check to see if T=1 support is indicated by TD2 */
              if ((uim_atr_buf[uim_drv_slot].data[uim_atr_td_index] &
                  (UIM_T_FIELD)) == (int)UIM_T_1_PROTOCOL)
              {
                /* Move the TD2 index to process the bytes after TD2 */
                uim_proc_atr_byte_index = uim_atr_td_index;

                /* Get the protocol specific parameters */
                if (uim_atr_buf[uim_drv_slot].data[uim_atr_td_index] &
                     UIM_ATR_TA_PRESENT)
                {
                  /* Store the Information field size for ICC */
                  uim_t_1_ifsc =
                    uim_atr_buf[uim_drv_slot].data[++uim_proc_atr_byte_index];
                } /* end if - TA(i) is present */

                if (uim_atr_buf[uim_drv_slot].data[uim_atr_td_index] &
                     UIM_ATR_TB_PRESENT)
                {
                  /* Store the Character waiting time index */
                  uim_t_1_cwi =
                    (uim_atr_buf[uim_drv_slot].data[++uim_proc_atr_byte_index] &
                      UIM_T_1_CWI_FIELD);
                  /* Store the block waiting time index */
                  uim_t_1_bwi =
                    ((uim_atr_buf[uim_drv_slot].data[uim_proc_atr_byte_index] &
                      UIM_T_1_BWI_FIELD) >> UIM_T_1_BWI_FIELD_SHIFT);
                } /* end if - TB(i) is present */

                if (uim_atr_buf[uim_drv_slot].data[uim_atr_td_index] &
                      UIM_ATR_TC_PRESENT)
                {
                  if (uim_atr_buf[uim_drv_slot].data[++uim_proc_atr_byte_index]&
                        UIM_T_1_EDC_FIELD)
                  {
                    /* Check to see if CRC is used for error detection */
                    uim_t_1_edc = UIM_T_1_EDC_CRC;
                    uim_epilogue_field_size = UIM_T_1_CRC_SIZE;
                  } /* The CRC indication bit is set */
                } /* end if - TC(i) is present */
              } /* end if - T=1 is supported */
            } /* end if TD2 is present */
#endif /* FEATURE_UIM_T_1_SUPPORT */
          } /* end if - TD1 is present */
        } /* end if - T=0 is supported */
#if defined( FEATURE_UIM_FLEXIBLE_COMPLIANCE )
        /* Check if this is a card that needs more delay for response bytes */
        if (uim_hist_char_buf[uim_drv_slot].length > UIM_HIST_CHAR_CMP_SIZE)
        {
          if (!memcmp(uim_hist_char_buf[uim_drv_slot].data,
                      UIM_MORE_DELAY_HIST_CHARS,
                      UIM_HIST_CHAR_CMP_SIZE))
          {
            if ((uim_hist_char_buf[uim_drv_slot].data[UIM_HIST_CHAR_CMP_SIZE] >=
                 UIM_MIN_MINOR_REV_MORE_DELAY) &&
                (uim_hist_char_buf[uim_drv_slot].data[UIM_HIST_CHAR_CMP_SIZE] <=
                 UIM_MAX_MINOR_REV_MORE_DELAY))
            {
              /* Increase the response byte delay in the driver */
              uim_response_byte_delay = UIM_RESPONSE_BYTE_DELAY_MORE;
            } /* end if - check the minor revision */
          } /* end if - the historical bytes match the delay more test
               string */
        } /* end if - there are enough historical bytes to check for more
             delay for the response byte */
#endif /* FEATURE_UIM_FLEXIBLE_COMPLIANCE */

        /* Indicate that we have already received the ATR so that we
         * will not try next voltage leavel when MULTIPLE_VOLTAGE is supported*/
#ifdef FEATURE_UIM_HANDLE_TA1_0X97_ATR
        /* Move to the adjacent state if only we did not require a warm reset */
        if(warm_reset_count == 0)
#endif /* FEATURE_UIM_HANDLE_TA1_0X97_ATR */
        {
          uim_atr_received = TRUE;
          ++uim_generic_state_ptr;
        }

      } /* end case - UIM_RESET_ST */
      break;
    case UIM_DELAY_AFTER_ATR_ST:              /* Introduce delay after ATR */
      {
        /* This is just to create a delay.  The delay has been done just
         * Get the next state as nothing needs to be done here
         */
        /* Adjust the generic state pointer according */
        if (skip_PPS)
        {
          uim_generic_state_ptr += 2;
          /* Set the flag to TRUE */
          uim_atr_pps_done = TRUE;
        }
        else
        {
          ++uim_generic_state_ptr;
        } /* end if - skip the PPS */
#ifdef FEATURE_UIM_DEBUG_NO_PPS
#error code not present
#endif /* FEATURE_UIM_DEBUG_NO_PPS */
      }
      break;

    case UIM_PPS_ST:                          /* PPS State */
      {
        /* Check if there was a problem with the Reset */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return(UIM_CMD_ERR);
        } /* end if - there was a problem with the Reset */

#if defined( FEATURE_UIM_T_1_SUPPORT )
        /* Indicate which protocol is running */
        MSG_MED( "The UIM is operating under T=%d", uim_operating_protocol,
                                                    0, 0 );
#endif /* FEATURE_UIM_T_1_SUPPORT */
        /* Set the flag to TRUE */
        uim_atr_pps_done = TRUE;

        /* Process the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_PPS_ST */
      break;

    case UIM_UPDATE_OP_PARAMS_ST:  /* Update Operational Parameters State */
      {
#if defined( FEATURE_UIM_T_1_SUPPORT )
        /* Set the current protocol to the operating protocol determined
           in the ATR processing */
        uim_current_protocol = uim_operating_protocol;

        /* Check if the current protocol is not T=1 */
        if (UIM_T_1_PROTOCOL != uim_current_protocol)
        {
          /* Skip the IFS negotiation */
          ++uim_generic_state_ptr;
        } /* end if - the protocol is not T=1 */
#endif /* FEATURE_UIM_T_1_SUPPORT */

        /* Process the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_UPDATE_OP_PARAMS_ST */
      break;

#if defined( FEATURE_UIM_T_1_SUPPORT )
    case UIM_IFS_NEG_ST:
      {
        /* Process the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_IFS_NEG_ST */
      break;
#endif /* FEATURE_UIM_T_1_SUPPORT */

    case UIM_CHECK_CHARACTERISTICS_ST:
      {
#ifdef FEATURE_UIM_UICC
        /* If the class of the command was incorrect, set the flag to change
        the instruction class and return a CMD ERROR */
        if ((rsp_ptr->cmd_status == UIM_WRONG_CLASS) ||
            (rsp_ptr->cmd_status == UIM_UNKNOWN_INS) ||
            (rsp_ptr->cmd_status == UIM_INCORRECT_PARAM_P3) ||
            (rsp_ptr->cmd_status == UIM_INCORRECT_PARAM_P1_2) ||
            (rsp_ptr->cmd_status == UIM_INCORRECT_PARAMS_P1_P2) ||
            (rsp_ptr->cmd_status == UIM_INCORRECT_PARAM_IN_DATA_FIELD) ||
            ((rsp_ptr->cmd_status == UIM_DONE) &&
             (cmd->hdr.protocol == UIM_UICC) &&
             (rsp_ptr->rsp.data[0] != UIM_FCP_TEMPLATE_TAG))
           )
        {
          /* Toggle the instruction class */
          uim_toggle_instrn_class = !uim_toggle_instrn_class;

          if (++uim_instrn_toggle_count > UIM_MAX_NUM_ATTEMPTS)
          {
             cmd->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
             return(UIM_CMD_ERR);
          }
          else
          {
             return(UIM_INSTRN_CHANGE);
          }
        }
        else
#endif /* FEATURE_UIM_UICC */
        /* Check if there was a problem with the select */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          /* Check to see if there are no more directories to select
             Note: select_state_ctr is incremented in this instruction.
             By incrementing select_state_ctr, the next DF will be selected
             when executing the command. */
          if (uim_path_to_be_sel[++select_state_ctr] == UIM_NO_FILE_SEL)
          {
            /* Do not allow the UIM clock to be stopped */
            uim_clock_stop_cntrl[uim_drv_slot] = UIM_CLOCK_STOP_NOT_ALLOWED;
            /* No more DFs to try.  Continue with the power-up */
            ++uim_generic_state_ptr;
            /* Fill out the path to select the ICCID for the next state */
            uim_path_to_be_sel[0] = UIM_MF_ID;
            uim_path_to_be_sel[1] = UIM_ICCID_EF_ID;
            uim_path_to_be_sel[2] = UIM_NO_FILE_SEL;
            select_state_ctr = 0;
          } /* end if - no more directories to select */
        }
        else /* The select was success.  Check the characteristics. */
        {
          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 = UIM_NO_FILE_SEL;
          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef  = UIM_NO_FILE_SEL;
          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = FALSE;

          /* Initialize the file characteristics byte */
          file_char_byte = 0;

          if (cmd->hdr.protocol == UIM_GSM)
          {
            /* Save the last selected 1st level DF */
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type = UIM_DF;
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df =
                                          uim_path_to_be_sel[select_state_ctr];

            if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df == UIM_GSM_DF)
            {
#if defined (FEATURE_UIM_SUPPORT_DUAL_SLOTS)
              if (cmd->hdr.slot == UIM_SLOT_1)
              {
                /* Set the gsm info in dfs1 present structure */
                uim_dfs_1.gsm_df_present = TRUE;

              }
              else if (cmd->hdr.slot == UIM_SLOT_2)
              {
                /* Set the gsm info in dfs2 present structure */
                uim_dfs_2.gsm_df_present = TRUE;

              }
#else
              /* Set the flag for slot 1 */
              uim_dfs_1.gsm_df_present = TRUE;
#endif
            }

            /*Extract the file characteristic byte from the response */
            file_char_byte = rsp_ptr->rsp.data[UIM_FILE_CHAR_INDEX];

#if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES )
            /* Determine the current voltage on the interface */
            switch (uim_current_voltage_class[uim_drv_slot])
            {
              case UIM_VOLTAGE_1_8V:
                {
                  /* Check if the card does not support this voltage */
                  if ((file_char_byte & UIM_1_8_SIM_GSM) !=
                      UIM_1_8_SIM_GSM)
                  {
                    /* Check if card supports other supported voltages */
                    if ((file_char_byte & UIM_3V_SIM_GSM) ==
                        UIM_3V_SIM_GSM)
                    {
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
                      /* Switch to different voltage */
                      /* Power down the interface */
                      uim_power_down();
#else
                      /* Switch to different voltage */
                      /* Power down the interface */
                      uim_power_down(cmd->hdr.slot);
#endif
                      rex_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY);
                      /* Try the indicated voltage class */
                      uim_current_voltage_class[uim_drv_slot] = UIM_VOLTAGE_3V;
                      return(UIM_SWITCH_VOLTAGE);
                    }
                    else /* No voltages supported */
                    {
                      return(UIM_VOLTAGE_MISMATCH);
                    } /* end if - card supports other voltage */
                  } /* end if - card does not support this voltage */
                } /* end case - UIM_VOLTAGE_1_8V */
                break;

              case UIM_VOLTAGE_3V:
                {
                  /* Check if the card does not support this voltage */
                  if ((file_char_byte & UIM_3V_SIM_GSM) !=
                      UIM_3V_SIM_GSM)
                  {
                    return(UIM_VOLTAGE_MISMATCH);
                  } /* end if - card does not support this voltage */
                } /* end case - UIM_VOLTAGE_3V*/
                break;

              default:
                {
                  ERR( "Bad UIM voltage class", 0, 0, 0 );
                  return(UIM_VOLTAGE_MISMATCH);
                } /* end case - default */
            } /* end switch - voltage class */
#else /* Does not support multiple voltages */
            /* Check if the card is a 5V only card */
            if ((file_char_byte & UIM_3V_TECH_BIT_GSM) == 0)
            {
               /* The card only supports 5 volts */
               return(UIM_VOLTAGE_MISMATCH);
            } /* end if - check for 5V only card */
#endif /* FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES */
          }
#ifdef FEATURE_UIM_UICC
          else if (cmd->hdr.protocol == UIM_UICC)
          {
            /* Save the last selected 1st level DF */
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df = UIM_NO_FILE_SEL;
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type = UIM_NO_SUCH_FILE;


            /* call the function to parse the response from the UICC
               select for an MF select */
            uim_parse_uicc_select_response(
              rsp_ptr->rsp.data,
              rsp_ptr->cmd_rsp_size,
              FALSE,
              &uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info,
              &file_char_byte );

#ifdef FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /*FEATURE_UIM_USB_UICC*/
            {
              /* Determine the current voltage on the interface */
              switch (uim_current_voltage_class[uim_drv_slot])
              {
                case UIM_VOLTAGE_1_8V:
                  {
                    /* Check if the card does not support this voltage */
                    if ((file_char_byte & UIM_1_8_UIM_UICC) !=
                        UIM_1_8_UIM_UICC)
                    {
                      /* Check if card supports other supported voltages */
                      if ((file_char_byte & UIM_3V_UIM_UICC) ==
                          UIM_3V_UIM_UICC)
                      {
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
                        /* Switch to different voltage */
                        /* Power down the interface */
                        uim_power_down();
#else
                        /* Switch to different voltage */
                        /* Power down the interface */
                        uim_power_down(cmd->hdr.slot);
#endif
                        rex_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY);
                        /* Try the indicated voltage class */
                        uim_current_voltage_class[uim_drv_slot] = UIM_VOLTAGE_3V;
                        return(UIM_SWITCH_VOLTAGE);
                      }
                      else /* No voltages supported */
                      {
                        return(UIM_VOLTAGE_MISMATCH);
                      } /* end if - card supports other voltage */
                    } /* end if - card does not support this voltage */
                  } /* end case - UIM_VOLTAGE_1_8V */
                  break;

                case UIM_VOLTAGE_3V:
                  {
                    /* Check if the card does not support this voltage */
                    if ((file_char_byte & UIM_3V_UIM_UICC) !=
                        UIM_3V_UIM_UICC)
                    {
                      return(UIM_VOLTAGE_MISMATCH);
                    } /* end if - card does not support this voltage */
                  } /* end case - UIM_VOLTAGE_3V*/
                  break;

                default:
                  {
                    ERR( "Bad UIM voltage class", 0, 0, 0 );
                    return(UIM_VOLTAGE_MISMATCH);
                  } /* end case - default */
              } /* end switch - voltage class */
            }
#else /* Does not support multiple voltages */
            if ((file_char_byte & UIM_3V_TECH_BIT_USIM) == 0)
            {
              /* The card does not support 3 volts */
              return(UIM_VOLTAGE_MISMATCH);
            }
#endif /* FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES */
          } /* protocol is GSM */
#endif /* FEATURE_UIM_UICC */
          /* Check if the UIM clock can be stopped */
          if ((file_char_byte & UIM_CLOCK_STOP_ALL_BITS) == 0)
          {
            /* Do not allow the UIM clock to be stopped */
            uim_clock_stop_cntrl[uim_drv_slot] = UIM_CLOCK_STOP_NOT_ALLOWED;
          }
          else /* the card supports stopping the UIM clock */
          {
            /* Determine the recommended level to stop the clock */
            if (file_char_byte & UIM_CLOCK_STOP_B3)
            {
              /* Clock must be stopped at high level */
              uim_clock_stop_cntrl[uim_drv_slot] = UIM_CLOCK_STOP_HIGH;
            }
            else
            {
              /* Clock must be stopped at low level */
              uim_clock_stop_cntrl[uim_drv_slot] = UIM_CLOCK_STOP_LOW;
            } /* end if - determine clock stop level */
          } /* end if - check UIM clock stop. */

          /* A DF was processed.  Continue with the power-up */
          ++uim_generic_state_ptr;

          /* Fill out the path to select the ICCID for the next state */
          uim_path_to_be_sel[0] = UIM_MF_ID;
          uim_path_to_be_sel[1] = UIM_ICCID_EF_ID;
          uim_path_to_be_sel[2] = UIM_NO_FILE_SEL;
          select_state_ctr = 0;

        } /* end if - there was a problem with the APDU */

      } /* end case - UIM_CHECK_CHARACTERISTICS_ST */
      break;

#if defined( FEATURE_UIM_RUIM ) || \
    defined (FEATURE_UIM_SUPPORT_DUAL_SLOTS)
    case UIM_CHECK_FOR_CDMA_DF:
      {
        /* Check if there was a problem with selecting the CDMA DF */
        if (rsp_ptr->cmd_status == UIM_DONE)
        {
          /* Save the last selected 1st level DF */
          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type = UIM_DF;
          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df = UIM_CDMA_DF;
          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 = UIM_NO_FILE_SEL;
          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef  = UIM_NO_FILE_SEL;
          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = FALSE;

          /* Reset the data in the last selected EF structure */
          uim_reset_last_sel_ef_data();

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
          /* If CDMA DF is present, set the flag */
          if (cmd->hdr.slot == UIM_SLOT_1)
          {
            uim_dfs_1.cdma_df_present = TRUE;
          }
          else if (cmd->hdr.slot == UIM_SLOT_2)
          {
            uim_dfs_2.cdma_df_present = TRUE;
          }
#else
          /* Initialize the DFs present */
          uim_dfs_1.cdma_df_present = TRUE;

          nvruim_dir_present |= NVRUIM_CDMA_DF_PRESENT;
#endif
        } /* end if - the CDMA DF was selected */

/* For multimode builds, we do not consider if there are no DFs */
#if !defined(FEATURE_UIM_GSM) && !defined(FEATURE_UIM_USIM)
        else /* If there was a problem, return an error */
        {
          MSG_ERROR("DF CDMA read failure", 0, 0, 0);
          return(UIM_CMD_ERR);
        }
#endif /* !FEATURE_UIM_GSM && !FEATURE_UIM_USIM */

        /* Get the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_CHECK_FOR_CDMA_DF */
      break;
#endif /* FEATURE_UIM_RUIM */

    case UIM_READ_ICCID_ST:
      {
        /* Check if there was a problem with the read */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
#if defined(FEATURE_MFLO_OPENCA)
#error code not present
#elif defined(FEATURE_UIM_HANDLE_ICCID_READ_FAILURE)
          if(cmd->hdr.protocol == UIM_UICC)
          {
            /* Toggle the instruction class */
            MSG_HIGH(" ICCID read failure, toggling the inst class ", 0, 0, 0);
            uim_toggle_instrn_class = !uim_toggle_instrn_class;
            return(UIM_INSTRN_CHANGE);
          }
          else if(cmd->hdr.protocol == UIM_GSM)
          {
           /* If there was a read error for ICCID
            * and the class is already in GSM return
            * error
            */
            return(UIM_CMD_ERR);
          }
#else
          return(UIM_CMD_ERR);
#endif /* FEATURE_UIM_HANDLE_ICCID_READ_FAILURE, FEATURE_MFLO_OPENCA */
        } /* end if - there was a problem with the APDU */

        /* Process the ICCID data according to the command received. */
        if ( (cmd->hdr.command == UIM_INTERNAL_ME_PUP_F) ||
             (cmd->hdr.command == UIM_RESET_F)
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
             || (cmd->hdr.command == UIM_RESET_WITH_TP_F)
#endif /* FEATURE_UIM_DUAL_SLOTS */
           )
        {
          /* Store the ICCID */
          for (i = 0; i < UIM_ICCID_SIZE; ++i)
          {
            uim_iccid[uim_drv_slot][i] = rsp_ptr->rsp.data[i];
          } /* end for - store the ICCID */
         /* Get the next state. */
         ++uim_generic_state_ptr;

        }
        else if (cmd->hdr.command == UIM_INTERNAL_WAKE_UP_F)
        {
          /* Compare the ICCID with the stored ICCID */
          for (i = 0; i < UIM_ICCID_SIZE; ++i)
          {
            if (uim_iccid[uim_drv_slot][i] != rsp_ptr->rsp.data[i])
            {
              /* Indicate an ICCID mismatch */
              return (UIM_ICCID_MISMATCH);
            }
          } /* end for - compare the ICCID */
        /* We would like to verify CHV1 only if we are not waking up in
           WCDMA mode */
#ifdef FEATURE_UIM_UICC
          if (cmd->hdr.protocol == UIM_UICC)
          {
            /* Proceed past the VERIFY_CHV1_ST. */
            uim_generic_state_ptr += 2;
          }
#endif /* FEATURE_UIM_UICC */
#if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
#ifdef FEATURE_UIM_UICC
          else
#endif /* FEATURE_UIM_UICC */
          {
           /* Get the next state. */
           ++uim_generic_state_ptr;
          }
#endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
        }
        else
        {
          /* Something is wrong.  We should not be in this state
             while processing this command. */
          MSG_ERROR ("UIM_READ_ICCID_ST outside of PUP or WAKE_UP",0,0,0);
          return (UIM_CMD_ERR);
        } /* end if - command is a power-up command. */
      } /* end case - UIM_READ_ICCID_ST */
      break;

    case UIM_SELECT_ST:                       /* Select State */
      {
#ifdef FEATURE_UIM_SUPPORT_NO_ICCID
        /* don't fail if ICCID is missing */
        if ( (rsp_ptr->cmd_status != UIM_DONE) &&
             (select_state_ctr < (UIM_MAX_PATH_ELEMENTS-2)) &&
             (uim_path_to_be_sel[select_state_ctr+1] == UIM_ICCID_EF_ID))
        {
         uim_iccid_select_failed = TRUE;
         rsp_ptr->cmd_status = UIM_DONE;
        }
#endif /* FEATURE_UIM_SUPPORT_NO_ICCID */

        /* Did the command execute properly. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          /* Clear out certain fields in the directory structure */
          uim_reset_last_sel_ef_data();

          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef  = UIM_NO_FILE_SEL;
          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = FALSE;

#ifdef FEATURE_UIM_UICC
          /* The state of the card is unknown when this type of SELECT fails
             - so clear the directory structure
             -----!!! THIS MAY RESULT IN POLLING FAILURES !!!----*/
          if (uim_sel_by_path_ref_from_mf)
          {
            uim_sel_by_path_ref_from_mf = FALSE;
          }
#endif /* FEATURE_UIM_UICC */
#ifdef FEATURE_UIM_SUPPORT_CSIM
          if (cmd->hdr.command == UIM_STREAM_ISO7816_APDU_F)
          {
            /* Max the attempts */
            MSG_HIGH("Internal select for CSIM command failed",0,0,0);
            cmd->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
            /* Indicate that the failure is for an internal select for
             * 7816 APDU */
            internal_sel_for_7816 = TRUE;
          }
#endif /* FEATURE_UIM_SUPPORT_CSIM */
          /* Set uim_path_to_be_sel to MF so that we will not fail polling */
          memset(uim_path_to_be_sel, 0xFF, sizeof(uim_path_to_be_sel));
          uim_path_to_be_sel[UIM_PATH_TO_SEL_MF_INDEX] = UIM_MF_ID;
          uim_reselect_mf = TRUE;
          select_state_ctr = 0;
          break; /* break out from the switch so that we can proceed to
                    uim_generic_command for MF selection */
        }

        /* Get the cached information back from the cached get-response and
         * let it be processed as though received back from the card
         */
        if ( uim_item_sel_rsp_cached )
        {
          if (select_rsp_cache_item_ptr != NULL)
          {
            memcpy(rsp_ptr->rsp.data, select_rsp_cache_item_ptr->data,
                   select_rsp_cache_item_ptr->data_size);
            rsp_ptr->cmd_rsp_size = select_rsp_cache_item_ptr->data_size;

            if ( cmd->hdr.command == UIM_CACHED_SEL_F )
            {
              /* This is a cached select.. Just go to the next state without
               * updating the current path and last selected ef etc.
               */
              ++uim_generic_state_ptr;
              break;
            }
          }
          else
          {
            UIM_ERROR_MSG("Not expecting a null select_rsp_cache_item_ptr",
                          0,0,0);
          }
        }


#ifdef FEATURE_UIM_UICC
        /* If path referencing was the method used to SELECT */
        if (uim_sel_by_path_ref_from_mf)
        {
          path_ctr = UIM_PATH_TO_SEL_MF_INDEX;

          /* Update the directory structure for all the element in the
             path */
          while ((path_ctr < UIM_MAX_PATH_ELEMENTS ) &&
                 (uim_path_to_be_sel[path_ctr] != UIM_NO_FILE_SEL))
          {
            /* -----------------------------------------------------------------
               Any file that does not follow the standard will fall into this
               category. We check only for EFs that do not follow the convention
               here. For Orange and cingular, only the dfs were incorrect but
               the EFs were valid
            -----------------------------------------------------------------*/
            if (((uim_path_to_be_sel[path_ctr] & UIM_FILE_ID_LEVEL_MASK2) != UIM_MF_LEVEL_BYTE) &&
                ((uim_path_to_be_sel[path_ctr] & UIM_FILE_ID_LEVEL_MASK2) != UIM_DF_LEVEL_1_BYTE) &&
                ((uim_path_to_be_sel[path_ctr] & UIM_FILE_ID_LEVEL_MASK2) != UIM_DF_LEVEL_2_BYTE) &&
                ((uim_path_to_be_sel[path_ctr] & UIM_FILE_ID_LEVEL_MASK2) != UIM_EF_UNDER_MF_BYTE) &&
                ((uim_path_to_be_sel[path_ctr] & UIM_FILE_ID_LEVEL_MASK2) != UIM_EF_UNDER_1ST_DF_BYTE)&&
                ((uim_path_to_be_sel[path_ctr] & UIM_FILE_ID_LEVEL_MASK2) != UIM_EF_UNDER_2ND_DF_BYTE)&&
                ((uim_path_to_be_sel[path_ctr] & UIM_FILE_ID_LEVEL_MASK2) != UIM_ADF_LEVEL_BYTE))
            {
              /* path_ctr + 2 is checked for not exceeding the max path size
                 so that uim_path_to_be_sel array is indexed safely with path_ctr + 1 */
              if (((path_ctr + 2) <= UIM_MAX_PATH_ELEMENTS) &&
                  (uim_path_to_be_sel[path_ctr+1] == UIM_NO_FILE_SEL))
              {
                /* Save the last selected EF type */
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef = uim_path_to_be_sel[path_ctr];
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = TRUE;

                if (cmd->hdr.protocol == UIM_UICC)
                {
                  /* call the function to parse the response from the UICC
                    select for an EF select */
                  uim_parse_uicc_select_response(
                              rsp_ptr->rsp.data,
                              rsp_ptr->cmd_rsp_size,
                              TRUE,
                              &uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info,
                              &file_char_byte );
                }
                path_ctr+=1;
              }/* end of if(path_ctr) */

              /* This condition will cover all non-spec compliant DFs too */
              /* This code is  also required if we have path to be selected
                 as 7FFF, 5Fxx, 8Fxx, 9Fxx. Although this is a valid path,
                 but uim_set_select_path may start the path selection from,
                 MF if the current dir pointer is offset. In that case we
                 perform internal select in two steps -
                 step 1 - select 3F00 (MF), 7FFF (DF1), 5Fxx (DF2), 8Fxx (DF3)
                 step 2 - select 7FFF (ADF), 5Fxx (DF2). 8Fxx (DF3), 9Fxx (EF)
                 The limitation of this code is that it will not support,
                 proprietary EFs at index 3 in path "AND" not under ADF for e.g.
                 path 3Fxx, 7F10, 8Fxx, 9Fxx */
              else
              {
                /* These are non-spec compliant DFs */
                /* If the DFs are under USIM ADF then uim_path_to_be_sel[0]
                   will be 7FFFF */
                if(uim_path_to_be_sel[UIM_PATH_DF1_INDEX] == UIM_SEL_AID_BY_FILE_ID)
                {
                  switch(path_ctr)
                  {
                    case UIM_PATH_DF2_INDEX:
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 =
                        uim_path_to_be_sel[path_ctr];
                      break;
                    case UIM_PATH_DF3_INDEX:
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df3 =
                        uim_path_to_be_sel[path_ctr];
                      break;
                    case UIM_PATH_EF2_INDEX:
                      /* This is EF with path length of 4 */
                      /* Save the last selected EF type */
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef =
                        uim_path_to_be_sel[path_ctr];
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = TRUE;
                      if (cmd->hdr.protocol == UIM_UICC)
                      {
                        /* call the function to parse the response from the UICC
                           select for an EF select */
                        uim_parse_uicc_select_response(
                          rsp_ptr->rsp.data,
                          rsp_ptr->cmd_rsp_size,
                          TRUE,
                          &uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info,
                          &file_char_byte );
                      }
                      break;
                    default:
                      UIM_ERROR_MSG("uim select state resp - incorrect path_ctr=0x%x",
                                     path_ctr,0,0);
                      break;
                  }
                }
                else /* uim_path_to_be_sel[0] != ADF */
                {
                  /* Path starts from MF hence DF1, DF2, DF3 indices are
                     offset by 1. for Path 3F00 7F10,8Fxx,9Fxx DF1 shall
                     be 7F10, DF2 shall be 8Fxx etc. */
                  switch(path_ctr)
                  {
                    case (UIM_PATH_DF1_INDEX + 1):
                      /* Non standard DF1 */
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type =
                        UIM_DF;
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df =
                         uim_path_to_be_sel[path_ctr];
                      break;
                    case (UIM_PATH_DF2_INDEX + 1):
                      /* Non standard DF2 */
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 =
                        uim_path_to_be_sel[path_ctr];
                      break;
                    case (UIM_PATH_DF3_INDEX + 1):
                      /* Non standard DF3 */
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df3 =
                        uim_path_to_be_sel[path_ctr];
                      break;
                    default:
                      UIM_ERROR_MSG("uim select state resp - incorrect path_ctr=0x%x",
                                     path_ctr,0,0);
                      break;
                  } /* end of switch(path_ctr) */
                }
                path_ctr += 1;
              }

            }/* end of if (uim_path_to_be_sel) */
            else
            {
              /* This is a hack since the numbering of the Orange DF is not
                 compliant with spec. Without this it completely breaks the
                 directory maintenance. Second level DF should not start with
                 7FXX */
              if (uim_path_to_be_sel[path_ctr] == UIM_ORANGE_DF)
              {
                /* Do not go through the update function and instead set the
                   structure here as ADF info would be overwritten. Save the last
                   selected 2nd level DF */
                if ( uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_ADF )
                {
                  uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 =
                    UIM_ORANGE_DF;
                }
                else
                {
                  uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type =
                    UIM_DF;
                  uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df   =
                    UIM_ORANGE_DF;
                  uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2      =
                    UIM_NO_FILE_SEL;
                }

                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef  = UIM_NO_FILE_SEL;
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = FALSE;

                /* Reset the data in the last selected EF structure */
                uim_reset_last_sel_ef_data();

                path_ctr+=1;
              }
#ifdef FEATURE_ENHANCED_NW_SELECTION
              else if (uim_path_to_be_sel[path_ctr] == UIM_CINGULAR_DF)
              {
                /* Do not go through the update function and instead set the
                   structure here as ADF info would be overwritten. Save the last
                   selected 2nd level DF */
                if(uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_ADF)
                {
                  uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 = UIM_CINGULAR_DF;
                  uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef  = UIM_NO_FILE_SEL;
                  uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = FALSE;

                  /* Reset the data in the last selected EF structure */
                  uim_reset_last_sel_ef_data();
                }

                path_ctr+=1;
              }
#endif /* FEATURE_ENHANCED_NW_SELECTION */
              else
              {
                status = uim_update_last_sel_dir( rsp_ptr,
#ifdef FEATURE_UIM_UICC
                                                  cmd,
#endif /* FEATURE_UIM_UICC */
                                                  uim_path_to_be_sel[path_ctr++]);
              }
            }/* end of if (uim_path_to_be_sel) */
            /* Check to make sure that the status is good */
            if (status == UIM_CMD_ERR)
            {
              return(UIM_CMD_ERR);
            }

            /* Increment the select state counter */
            select_state_ctr+=1;
          } /* end of while */

          /* reset the flag */
          uim_sel_by_path_ref_from_mf = FALSE;

          if ( FALSE == uim_item_sel_rsp_cached &&
               select_rsp_cache_item_ptr != NULL)
          {
            if ( select_rsp_cache_item_ptr->data != NULL )
            {
              uim_free(select_rsp_cache_item_ptr->data);
            }

            select_rsp_cache_item_ptr->data =
              uim_malloc(rsp_ptr->cmd_rsp_size);
            if ( select_rsp_cache_item_ptr->data == NULL )
            {
              MSG_ERROR("Could not allocate memory to store cache response",
                        0,0,0);
            }
            else
            {
              select_rsp_cache_item_ptr->is_cached = TRUE;
              memcpy(select_rsp_cache_item_ptr->data, rsp_ptr->rsp.data,
                     rsp_ptr->cmd_rsp_size);
              select_rsp_cache_item_ptr->data_size = rsp_ptr->cmd_rsp_size;
            }
          }

          /* The file selection for the entire path is finished. Goto the next
             state. */
          ++uim_generic_state_ptr;

        } /* if (uim_sel_by_path_ref_from_mf) */
        else
#endif /* FEATURE_UIM_UICC */
        {
          if (((uim_path_to_be_sel[select_state_ctr] & UIM_FILE_ID_LEVEL_MASK2) != UIM_MF_LEVEL_BYTE) &&
              ((uim_path_to_be_sel[select_state_ctr] & UIM_FILE_ID_LEVEL_MASK2) != UIM_DF_LEVEL_1_BYTE) &&
              ((uim_path_to_be_sel[select_state_ctr] & UIM_FILE_ID_LEVEL_MASK2) != UIM_DF_LEVEL_2_BYTE) &&
              ((uim_path_to_be_sel[select_state_ctr] & UIM_FILE_ID_LEVEL_MASK2) != UIM_EF_UNDER_MF_BYTE) &&
              ((uim_path_to_be_sel[select_state_ctr] & UIM_FILE_ID_LEVEL_MASK2) != UIM_EF_UNDER_1ST_DF_BYTE) &&
              ((uim_path_to_be_sel[select_state_ctr] & UIM_FILE_ID_LEVEL_MASK2) != UIM_EF_UNDER_2ND_DF_BYTE) &&
              ((uim_path_to_be_sel[select_state_ctr] & UIM_FILE_ID_LEVEL_MASK2) != UIM_ADF_LEVEL_BYTE))
          {
            /* select_state_ctr + 2 is checked for not exceeding the max path size
               so that uim_path_to_be_sel array is indexed safely with select_state_ctr + 1 */
            if ((select_state_ctr + 2) <= UIM_MAX_PATH_ELEMENTS &&
                uim_path_to_be_sel[select_state_ctr+1] == 0xFFFF)
            {
              /* Save the last selected EF type */
              uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef = uim_path_to_be_sel[select_state_ctr];
              uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = TRUE;

              #ifdef FEATURE_UIM_UICC
              if (cmd->hdr.protocol == UIM_UICC)
              {
                /* call the function to parse the response from the UICC
                  select for an EF select */
                uim_parse_uicc_select_response(
                            rsp_ptr->rsp.data,
                            rsp_ptr->cmd_rsp_size,
                            TRUE,
                            &uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info,
                            &file_char_byte );
              }
              #endif /* FEATURE_UIM_UICC */
              #if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
              #ifdef FEATURE_UIM_UICC
              else
              #endif /* FEATURE_UIM_UICC */
              {
                /* Save the EF particulars */
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_id =
                                                (word)FLIPW (rsp_ptr->rsp.ef.file_id);
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size =
                                                (word)FLIPW (rsp_ptr->rsp.ef.file_size);
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_struct =
                                                        rsp_ptr->rsp.ef.file_struct;
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len =
                                                        rsp_ptr->rsp.ef.rec_len;
              }
              #endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
            }/* end of if(select_state_ctr) */
            else if( select_state_ctr < (UIM_MAX_PATH_ELEMENTS - 1))
            {
              /* This block will check for non-spec compliant DFs too */
                /* If the DFs are under USIM ADF then uim_path_to_be_sel[0] will be 7FFFF */
#ifdef FEATURE_UIM_UICC
                if(uim_path_to_be_sel[UIM_PATH_DF1_INDEX] == UIM_SEL_AID_BY_FILE_ID)
                {
                  switch(select_state_ctr)
                  {
                    case UIM_PATH_DF2_INDEX:
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 = uim_path_to_be_sel[select_state_ctr];
                      break;
                    case UIM_PATH_DF3_INDEX:
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df3 = uim_path_to_be_sel[select_state_ctr];
                      break;
                    case (UIM_PATH_EF2_INDEX):
                      /* This is EF with path length of 4 */
                      /* Save the last selected EF type */
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef = uim_path_to_be_sel[select_state_ctr];
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = TRUE;
                      if (cmd->hdr.protocol == UIM_UICC)
                      {
                        /* call the function to parse the response from the UICC
                          select for an EF select */
                        uim_parse_uicc_select_response(
                                    rsp_ptr->rsp.data,
                                    rsp_ptr->cmd_rsp_size,
                                    TRUE,
                                    &uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info,
                                    &file_char_byte );
                      }
                      #if defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)
                      else
                      {
                        /* Save the EF particulars */
                        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_id =
                          (word)FLIPW (rsp_ptr->rsp.ef.file_id);
                        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_size =
                          (word)FLIPW (rsp_ptr->rsp.ef.file_size);
                        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.file_struct =
                          rsp_ptr->rsp.ef.file_struct;
                        uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_info.rec_len =
                          rsp_ptr->rsp.ef.rec_len;
                      }
                      #endif /* FEATURE_UIM_GSM || defined FEATURE_UIM_RUIM */
                      break;
                    default:
                      UIM_ERROR_MSG("uim select state resp - incorrect select_state_ctr=0x%x",
                                     select_state_ctr,0,0);
                      break;
                  } /* switch(select_state_ctr) */
                } /* if(uim_path_to_be_sel[0] == ADF) */

                else /* uim_path_to_be_sel[0] != ADF */
#endif /* FEATURE_UIM_UICC */
                {
                  /* Path starts from MF hence DF1, DF2, DF3 indices are offset by 1.
                      for Path 3F00 7F10,8Fxx,9Fxx DF1 shall be 7F10, DF2 shall be 8Fxx etc. */
                  switch(select_state_ctr)
                  {
                    case (UIM_PATH_DF1_INDEX + 1):
                      /* Non standard DF1 */
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type =
                        UIM_DF;
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df =
                        uim_path_to_be_sel[select_state_ctr];
                      break;
                    case (UIM_PATH_DF2_INDEX + 1):
                      /* Non standard DF2 */
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 =
                        uim_path_to_be_sel[select_state_ctr];
                      break;
                    case (UIM_PATH_DF3_INDEX + 1):
                      /* Non standard DF3 */
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df3 =
                        uim_path_to_be_sel[select_state_ctr];
                      break;
                    default:
                      UIM_ERROR_MSG("uim select state resp - incorrect select_state_ctr=0x%x",
                                     select_state_ctr,0,0);
                      break;
                  } /* end of switch(select_state_ctr) */
                } /* else uim_path_to_be_sel[0] != ADF */
                select_state_ctr++;
            } /* else if( select_state_ctr < (UIM_MAX_PATH_ELEMENTS - 1)) */

          }/* end of if (uim_path_to_be_sel) */
          else
          {
#ifdef FEATURE_UIM_UICC
            if ((uim_path_to_be_sel[select_state_ctr] == UIM_ORANGE_DF) &&
                (cmd->hdr.protocol == UIM_UICC) &&
                (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_ADF))
            {
              /* Do not go through the update function and instead set the
                 structure here as ADF info would be overwritten. Save the
                 Orange DF as last selected 2nd level DF */
              uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 = UIM_ORANGE_DF;
              uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef  = UIM_NO_FILE_SEL;
              uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = FALSE;

              /* Reset the data in the last selected EF structure */
              uim_reset_last_sel_ef_data();
            }
#ifdef FEATURE_ENHANCED_NW_SELECTION
            else if ((uim_path_to_be_sel[select_state_ctr] == UIM_CINGULAR_DF) &&
                (cmd->hdr.protocol == UIM_UICC) &&
                (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_ADF))
            {
              /* Do not go through the update function and instead set the
                 structure here as ADF info would be overwritten. Save the
                 Orange DF as last selected 2nd level DF */
              uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 = UIM_CINGULAR_DF;
              uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef  = UIM_NO_FILE_SEL;
              uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel = FALSE;

              /* Reset the data in the last selected EF structure */
              uim_reset_last_sel_ef_data();
            }
#endif/*FEATURE_ENHANCED_NW_SELECTION*/
            else
#endif
            {

              /* Update the last selected dir structure */
              status = uim_update_last_sel_dir( rsp_ptr,
#ifdef FEATURE_UIM_UICC
                                              cmd,
#endif /* FEATURE_UIM_UICC */
                                              uim_path_to_be_sel[select_state_ctr]);
            }
          }/* end of if (uim_path_to_be_sel) */
          /* Check to make sure that the status is good */
          if (status == UIM_CMD_ERR)
          {
             return (UIM_CMD_ERR);
          }

          /* Move to the next state if we have reached the end of the path. */
          if ( select_state_ctr < UIM_MAX_PATH_ELEMENTS -1 )
          {
            /* Select the next file ID by incrementing the sel state counter */
            if (uim_path_to_be_sel[++select_state_ctr] == UIM_NO_FILE_SEL)
            {
              if ( FALSE == uim_item_sel_rsp_cached &&
                   select_rsp_cache_item_ptr != NULL)
              {
                if ( select_rsp_cache_item_ptr->data != NULL )
                {
                  uim_free(select_rsp_cache_item_ptr->data);
                }
                select_rsp_cache_item_ptr->data =
                  uim_malloc(rsp_ptr->cmd_rsp_size);
                if ( select_rsp_cache_item_ptr->data == NULL )
                {
                  MSG_ERROR("Could not allocate memory to store cache response",
                            0,0,0);
                }
                else
                {
                  select_rsp_cache_item_ptr->is_cached = TRUE;
                  memcpy(select_rsp_cache_item_ptr->data, rsp_ptr->rsp.data,
                         rsp_ptr->cmd_rsp_size);
                  select_rsp_cache_item_ptr->data_size = rsp_ptr->cmd_rsp_size;
                }
              }

              /* The file selection is finished.  Get to the next state. */
              ++uim_generic_state_ptr;
            }
          }
          else if ( select_state_ctr == (UIM_MAX_PATH_ELEMENTS -1))
          {
            if ( FALSE == uim_item_sel_rsp_cached &&
                 select_rsp_cache_item_ptr != NULL)
            {
              if ( select_rsp_cache_item_ptr->data != NULL )
              {
                uim_free(select_rsp_cache_item_ptr->data);
              }
              select_rsp_cache_item_ptr->data =
                uim_malloc(rsp_ptr->cmd_rsp_size);
              if ( select_rsp_cache_item_ptr->data == NULL )
              {
                MSG_ERROR("Could not allocate memory to store cache response",
                          0,0,0);
              }
              else
              {
                select_rsp_cache_item_ptr->is_cached = TRUE;
                memcpy(select_rsp_cache_item_ptr->data, rsp_ptr->rsp.data,
                       rsp_ptr->cmd_rsp_size);
                select_rsp_cache_item_ptr->data_size = rsp_ptr->cmd_rsp_size;
              }
            }

            /* The file selection is finished.  Goto the next state. */
            ++uim_generic_state_ptr;
          } /* Check the next file ID to determine if the cmd is finished.*/
        }
      } /* end case - UIM_SELECT_ST */
      break;

    case UIM_STATUS_ST:                       /* Status State */
      {
        /* Response to status is the same as Select MF/DF */
        /* Determine which command got us here. */
        if (cmd->hdr.command == UIM_INTERNAL_POLL_F)
        {
          if (rsp_ptr->cmd_status != UIM_DONE)
          {
#ifdef FEATURE_UIM_RPT_BAD_SW_ON_POLL
            if ( uim_mode != UIM_PASSIVE_MODE )
            {
              MSG_ERROR("Un-successful polling - SW1, SW2, cmd_status",
                        rsp_ptr->sw1, rsp_ptr->sw2, rsp_ptr->cmd_status);
              return(UIM_POLL_ERROR);
            }
#else
            MSG_ERROR("Polling failed while uim_mode is %d, ignoring",
                      uim_mode,0,0);
            /* Internal poll command failed for some odd reason
             * let us continue instead of comparing data with the
             * stale data in the response pointer
             */
            ++uim_generic_state_ptr;
            break;
#endif
          }

          if ( uim_mode == UIM_PASSIVE_MODE ||
               cmd->status.return_data == UIM_RETURN_NONE )
          {
            /* We are either in in passive mode or not in traffic.  It is
             * enough if we get a response, we need not compare the
             * directory maintainence just process the next state
             */
            ++uim_generic_state_ptr;
            break;
          }
#ifdef FEATURE_UIM_UICC
          status_adf.aid_length = 0;

          if (cmd->hdr.protocol == UIM_UICC)
          {
            uim_parse_uicc_status_response ( rsp_ptr->rsp.data,
                                           rsp_ptr->cmd_rsp_size,
                                           &status_file_id,
                                           &status_adf );
          }
#endif

          /* Determine if a level 2 DF was selected last. */
          if ( uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 != UIM_NO_FILE_SEL )
          {
#ifdef FEATURE_UIM_UICC
            if (cmd->hdr.protocol == UIM_UICC)
            {
              if ( status_file_id != uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 )
              {
                MSG_HIGH(" Mismatch in DF2 level - UICC protocol",0,0,0);
                MSG_HIGH( "Recd file ID in STATUS is %x",status_file_id,0,0);
                /* The selected file ID has changed */
                return(UIM_POLL_ERROR);
              }
            }
            else
#endif
            /* Verify the directory has not changed. */
            if ((FLIPW(rsp_ptr->rsp.mf_df.file_id) !=
                                            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2))
            {
              MSG_HIGH(" Mismatch in DF2 level - GSM protocol",0,0,0);
              MSG_HIGH( "Recd file ID in STATUS is %x",
                                         FLIPW(rsp_ptr->rsp.mf_df.file_id),0,0);

              /* The selected file ID has changed */
              return(UIM_POLL_ERROR);
            } /* end if - does the status file ID match the 2nd level DF */
          } /* end if - a 2nd level DF was last selected */
          else if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df3 !=  UIM_NO_FILE_SEL)
          {
#ifdef FEATURE_UIM_UICC
            if (cmd->hdr.protocol == UIM_UICC)
            {
              if ( status_file_id != uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df3 )
              {
                MSG_ERROR(" Mismatch in DF3 level - UICC protocol",0,0,0);
                MSG_ERROR( "Recd file ID in STATUS is %x",status_file_id,0,0);
                /* The selected file ID has changed */
                return(UIM_POLL_ERROR);
              }
            }
            else
#endif /* FEATURE_UIM_UICC */
            /* Verify the directory has not changed. */
            if ((FLIPW(rsp_ptr->rsp.mf_df.file_id) !=
                 uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df3))
            {
              MSG_ERROR(" Mismatch in DF3 level - GSM protocol",0,0,0);
              MSG_ERROR( "Recd file ID in STATUS is %x",
                         FLIPW(rsp_ptr->rsp.mf_df.file_id),0,0);

              /* The selected file ID has changed */
              return(UIM_POLL_ERROR);
            } /* end if - does the status file ID match the 3rd level DF */
          } /* end if - a 3rd level DF was last selected */
          else
          {
            /* Check to see if the last selected first level DF matches with the
               response when the DF type is a DF */
            if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type ==
                UIM_DF)
            {
#ifdef FEATURE_UIM_UICC
              if (cmd->hdr.protocol == UIM_UICC)
              {
                 if ( status_file_id !=
                      uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df )
                 {
                   MSG_HIGH(" Mismatch in DF1 level - UICC protocol",0,0,0);
                   MSG_HIGH( "Recd file ID in STATUS is %x",status_file_id,0,0);
                   /* The selected file ID has changed */
                   return(UIM_POLL_ERROR);
                 }
              }
              else
#endif
              if (FLIPW(rsp_ptr->rsp.mf_df.file_id) !=
                  uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df)
              {
                MSG_HIGH(" Mismatch in DF1 level - GSM protocol",0,0,0);
                MSG_HIGH( "Recd file ID in STATUS is %x",
                                         FLIPW(rsp_ptr->rsp.mf_df.file_id),0,0);
                return(UIM_POLL_ERROR);
              }
             /* Check to see if the last selected first level DF matches with
             response when the DF type is a ADF */
            }/* end if - DF was selected last */
#ifdef FEATURE_UIM_UICC
            else if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_ADF)
            {
              /* Parse the adf from the response */

              if (memcmp(status_adf.data,
                         uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf.data,
                         uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf.aid_length) != 0)
              {
                MSG_HIGH("Mismatch in ADF - UICC protocol",0,0,0);
                return(UIM_POLL_ERROR);
              }
            }  /* end if - ADF was selected last */
#endif /* FEATURE_UIM_UICC */
            /* The last selected file ID must be the MF */
            else if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_NO_SUCH_FILE)
            {
#ifdef FEATURE_UIM_UICC
              if (cmd->hdr.protocol == UIM_UICC)
              {
                if ( status_file_id != UIM_MF_ID )
                {
                  MSG_HIGH(" Mismatch in MF - UICC protocol",0,0,0);
                  MSG_HIGH( "Recd file ID in STATUS is %x",status_file_id,0,0);
                  /* The selected file ID has changed */
                  return(UIM_POLL_ERROR);
                }
              }
              else
#endif
              if (FLIPW(rsp_ptr->rsp.mf_df.file_id) != UIM_MF_ID)
              {
                MSG_HIGH(" Mismatch in MF - GSM protocol",0,0,0);
                MSG_HIGH( "Recd file ID in STATUS is %x",
                                         FLIPW(rsp_ptr->rsp.mf_df.file_id),0,0);
                return(UIM_POLL_ERROR);
              }
            } /* end if - MF was selected last */
          } /* end if - 2nd level DF last selected */
        } /* end if - the command was a poll command */
        else
        {
          if (rsp_ptr->cmd_status != UIM_DONE)
          {
            return(UIM_CMD_ERR);
          }
        }
        /* Process the next state */
        ++uim_generic_state_ptr;
      } /* end case - UIM_STATUS_ST */
      break;

    case UIM_READ_BIN_MEM_ST:                  /* Read Binary State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Temporary variable to hold the number of bytes actually read */
          word tmp_num_bytes_read;

          if ( uim_req_buf_ptr->apdu_hdr.p3 == 0 )
          {
            tmp_num_bytes_read = (word) UIM_MAX_TRANSACTION_SIZE;
          }
          else
          {
            tmp_num_bytes_read = (word) uim_req_buf_ptr->apdu_hdr.p3;
          }

          /* decrement the number of bytes left to read by the number read*/
          uim_rd_wr_info.num_bytes -= tmp_num_bytes_read;


          /* store the total number of bytes read up-to now */
          uim_rd_wr_info.num_bytes_rsp += tmp_num_bytes_read;

          /* Increment the offset so that next read would get
           * the right data
           */
          uim_rd_wr_info.offset += tmp_num_bytes_read;

          /* Copy over the data to the buffer provided each time we have
           * a peice of data returned
           */
          memcpy(uim_rd_wr_info.data_ptr , rsp_ptr->rsp.data,
                 tmp_num_bytes_read);

          /* Increment the pointer to point to the next
           * location to copy the data
           */
          uim_rd_wr_info.data_ptr = uim_rd_wr_info.data_ptr +
            tmp_num_bytes_read;

          /* if no more bytes to read go to the next step*/
          if (uim_rd_wr_info.num_bytes == 0)
          {
            /* Skip to the next state. */
            ++uim_generic_state_ptr;
          }
        }
      } /* end case - UIM_READ_BIN_MEM_ST */
      break;

    case UIM_UPDATE_BIN_MEM_ST:                /* Update Binary State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Decrement the number of bytes sent in the transaction */
          uim_rd_wr_info.num_bytes -= uim_req_buf_ptr->apdu_hdr.p3;

          /* store the total number of bytes read up-to now */
          uim_rd_wr_info.num_bytes_rsp  += uim_req_buf_ptr->apdu_hdr.p3;

          /* Check if we are done sending all the bytes
           * If the number of bytes requested is greater than the file size, then
           *  P3 is filled with the file size as the number of bytes to write
           */
          if (uim_rd_wr_info.num_bytes)
          {
            /* Change the offset for the next write */
            uim_rd_wr_info.offset += uim_req_buf_ptr->apdu_hdr.p3;

            /* Change the data pointer for the next write */
            uim_rd_wr_info.data_ptr += uim_req_buf_ptr->apdu_hdr.p3;
          }
          else
          {
            /* Skip to the next state. */
            ++uim_generic_state_ptr;
          }
        }
      } /* end case - UIM_UPDATE_BIN_MEM_ST */
      break;

    case UIM_READ_BINARY_ST:                  /* Read Binary State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          ++uim_generic_state_ptr;
        }
      } /* end case - UIM_READ_BINARY_ST */
      break;

    case UIM_UPDATE_BINARY_ST:                /* Update Binary State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Decrement the number of bytes sent in the transaction */
          cmd->access_uim.num_bytes -= uim_req_buf_ptr->apdu_hdr.p3;

          /* Check if we are done sending all the bytes */
          /* If the number of bytes requested is greater than the file size, then
             P3 is filled with the file size as the number of bytes to write */
          if (cmd->access_uim.num_bytes)
          {
            /* Change the offset for the next write */
            cmd->access_uim.offset += uim_req_buf_ptr->apdu_hdr.p3;
            /* Change the data pointer for the next write */
            cmd->access_uim.data_ptr = (byte*)cmd->access_uim.data_ptr + uim_req_buf_ptr->apdu_hdr.p3;
          }
          else
          {
            /* Skip to the next state. */
            ++uim_generic_state_ptr;
          }
        }
      } /* end case - UIM_UPDATE_BINARY_ST */
      break;

    case UIM_READ_REC_MEM_ST:                  /* Read Record State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Need to copy the data here */
          word tmp_num_bytes_read;

          if ( uim_req_buf_ptr->apdu_hdr.p3 == 0 )
          {
            tmp_num_bytes_read = (word) UIM_MAX_TRANSACTION_SIZE;
          }
          else
          {
            tmp_num_bytes_read = (word) uim_req_buf_ptr->apdu_hdr.p3;
          }

          /* Copy over the data to the buffer provided each time we have
           * a peice of data returned
           */
          memcpy(uim_rd_wr_info.data_ptr , rsp_ptr->rsp.data,
                 tmp_num_bytes_read);

          /* Set the number of response bytes received */
          uim_rd_wr_info.num_bytes_rsp = tmp_num_bytes_read;

          /* Skip to the next state. */
          ++uim_generic_state_ptr;
        }
      } /* end case - UIM_READ_REC_MEM_ST */
      break;

    case UIM_UPDATE_REC_MEM_ST:                /* Update Record State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          if (rsp_ptr->cmd_status == UIM_MEMORY_PROBLEM)
          {
             /* Set the command count to MAX so as to not repeat this cmd as a
                GCF test case fails when repeated */
             cmd->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
          }
          return (UIM_CMD_ERR);
        }
        else
        {
          uim_rd_wr_info.num_bytes_rsp  += uim_req_buf_ptr->apdu_hdr.p3;
          /* Skip to the next state. */
          ++uim_generic_state_ptr;
        }
      } /* end case - UIM_UPDATE_REC_MEM_ST */
      break;

    case UIM_READ_RECORD_ST:                  /* Read Record State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          ++uim_generic_state_ptr;
        }
      } /* end case - UIM_READ_RECORD_ST */
      break;

    case UIM_UPDATE_RECORD_ST:                /* Update Record State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          if (rsp_ptr->cmd_status == UIM_MEMORY_PROBLEM)
          {
             /* Set the command count to MAX so as to not repeat this cmd as a
                GCF test case fails when repeated */
             cmd->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
          }
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          ++uim_generic_state_ptr;
        }
      } /* end case - UIM_UPDATE_RECORD_ST */
      break;

    case UIM_INC_MEM_ST:                    /* Increment State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          ++uim_generic_state_ptr;
        }
      } /* end case - UIM_INCREMENT_ST */
      break;

    case UIM_INCREMENT_ST:                    /* Increment State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          ++uim_generic_state_ptr;
        }
      } /* end case - UIM_INCREMENT_ST */
      break;

    case UIM_VERIFY_CHV1_ST:                  /* Verify CHV 1 State */
      {
        /* If the response is good, then set the use flag to TRUE */
        if (rsp_ptr->cmd_status == UIM_DONE)
        {
          uim_chv1[uim_drv_slot].use_chv = TRUE;
        }
        else
        {
          /* Do not verify CHV1 until the verify succeeds. */
          uim_chv1[uim_drv_slot].use_chv = FALSE;
        } /* end if - the UIM command completed successfully. */

        /* Skip to the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_VERIFY_CHV1_ST */
      break;

    case UIM_VERIFY_CHV2_ST:                  /* Verify CHV 2 State */
      {
        /* If the response is good, then set the use flag to TRUE */
        if (rsp_ptr->cmd_status == UIM_DONE)
        {
          uim_chv2[uim_drv_slot].use_chv = TRUE;
        }
        else
        {
          /* Do not verify CHV1 until the verify succeeds. */
          uim_chv2[uim_drv_slot].use_chv = FALSE;

        } /* end if - the UIM command completed successfully. */
        /* Skip to the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_VERIFY_CHV2_ST */
      break;

    case UIM_UNBLOCK_CHV1_ST:                 /* Unblock CHV 1 State */
      {
        /* If the response is good, then set the use flag to TRUE */
        if (rsp_ptr->cmd_status == UIM_DONE)
        {
          /* Keep the new CHV value in the cache buffer.  The cache buffer
             is used to verify CHV1 on wake up. */
          memcpy( uim_chv1[uim_drv_slot].chv, cmd->unblk_chv.new_chv_digits,
                  UIM_MAX_CHV_DIGITS);

          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_chv1[uim_drv_slot].chv);

          uim_chv1[uim_drv_slot].use_chv = TRUE;
        }
        else
        {
          /* Do not verify CHV1 again until a CHV command succeeds. */
          uim_chv1[uim_drv_slot].use_chv = FALSE;
        } /* end if - the UIM command completed successfully. */

        /* Skip to the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_UNBLOCK_CHV1_ST */
      break;

    case UIM_UNBLOCK_CHV2_ST:                 /* Unblock CHV 2 State */
      {
        /* Skip to the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_UNBLOCK_CHV2_ST */
      break;

    case UIM_CHANGE_CHV1_ST:                  /* Change CHV 1 State */
      {
        /* If the response is good, then set the use flag to TRUE */
        if (rsp_ptr->cmd_status == UIM_DONE)
        {
          /* Keep the new CHV value in the cache buffer.  The cache buffer
             is used to verify CHV1 on wake up. */
          memcpy( uim_chv1[uim_drv_slot].chv, cmd->change_chv.new_chv_digits,
                  UIM_MAX_CHV_DIGITS);

          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_chv1[uim_drv_slot].chv);

          uim_chv1[uim_drv_slot].use_chv = TRUE;
        }
        else
        {
          /* Do not verify CHV1 again until the verify succeeds. */
          uim_chv1[uim_drv_slot].use_chv = FALSE;
        } /* end if - the UIM command completed successfully. */

        /* Skip to the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_CHANGE_CHV1_ST */
      break;

    case UIM_CHANGE_CHV2_ST:                  /* Change CHV 2 State */
      {
        /* Since we are not caching the CHV2, Skip to the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_CHANGE_CHV2_ST */
      break;

    case UIM_ENABLE_CHV1_ST:                  /* Enable CHV 1 State */
      {
        /* If the response is good, then set the use flag to TRUE */
        if (rsp_ptr->cmd_status == UIM_DONE)
        {
          /* Keep the new CHV value in the cache buffer.  The cache buffer
             is used to verify CHV1 on wake up. */
          memcpy( uim_chv1[uim_drv_slot].chv, cmd->enable_chv.chv_digits,
                  UIM_MAX_CHV_DIGITS);

          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_chv1[uim_drv_slot].chv);

          uim_chv1[uim_drv_slot].use_chv = TRUE;
        }
        else
        {
          /* Do not verify CHV1 again until a CHV command succeeds. */
          uim_chv1[uim_drv_slot].use_chv = FALSE;
        } /* end if - the UIM command completed successfully. */

        /* Skip to the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_ENABLE_CHV1_ST */
      break;

#if (defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)) && \
     defined (UIM_FEATURE_ALLOW_CHV2_DISABLE)
    case UIM_ENABLE_CHV2_ST:                  /* Enable CHV 2 State */
      {
        /* Since we are not caching the CHV2, Skip to the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_ENABLE_CHV2_ST */
      break;
#endif /* UIM_FEATURE_ALLOW_CHV2_DISABLE */

    case UIM_DISABLE_CHV1_ST:                 /* Disable CHV 1 State */
      {
        /* If this command was successful, then the CHV was disabled
           successfully, so we do not have to verify CHV upon power up.

           If the command failed with SW1 as 98, then the CHV presented
           is incorrect or already disabled or blocked, then the use
           flag is set to FALSE */

        /* If the response is good, then copy the CHV1 digits to the cache
           buffer. */
        if (rsp_ptr->cmd_status == UIM_DONE)
        {
          /* Keep the new CHV value in the cache buffer.  The cache buffer
             is used to verify CHV1 on wake up. */
          memcpy( uim_chv1[uim_drv_slot].chv, cmd->unblk_chv.new_chv_digits,
                  UIM_MAX_CHV_DIGITS);

          /* Format the CHV digits in the cache buffer. */
          uim_format_chv(uim_chv1[uim_drv_slot].chv);

        }
        /* Whether the command succeeds, there is no reason the verify CHV1.
           If the command fails, the CHV1 digits do not match the card.  In
           either case, CHV1 is not to be verified. */
        uim_chv1[uim_drv_slot].use_chv = FALSE;

        /* if the sw indicates that there is some other error, then we
           would like to continue verifying CHV on power up with the
           old CHV and not from the CHV in the disable command */
        if (rsp_ptr->sw1 != UIM_SW1_SECURITY)
        {
           uim_chv1[uim_drv_slot].use_chv = TRUE;
        } /* end if - sw1 indicates a problem other than security management.*/

        /* Skip to the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_DISABLE_CHV1_ST */
      break;

#if (defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_RUIM)) && \
     defined (UIM_FEATURE_ALLOW_CHV2_DISABLE)
    case UIM_DISABLE_CHV2_ST:                 /* Disable CHV 2 State */
      {
        /* Since we are not caching the CHV2, Skip to the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_DISABLE_CHV2_ST */
      break;
#endif /* UIM_FEATURE_ALLOW_CHV2_DISABLE */

    case UIM_INVALIDATE_ST:                   /* Invalidate State */
      {
        /* Check if there was a problem with Invalidation */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        } /* end if - there was a problem with the APDU */

        /* Get the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_INVALIDATE_ST */
      break;

    case UIM_REHABILITATE_ST:                 /* Rehab state */
      {
        /* Check if there was a problem with Rehab */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        } /* end if - there was a problem with the APDU */

        /* Get the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_REHABILITATE_ST */
      break;

    case UIM_SEEK_ST:
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          ++uim_generic_state_ptr;
        }
      } /* end case - UIM_SEEK_ST */
      break;

#ifdef FEATURE_UIM_TOOLKIT

    case UIM_TERMINAL_PROFILE_ST:             /* Term profile State */
      {

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
        /* If we came here because of Reset with TP command and there
           is no term profile to download, then set the status as DONE */
        if ((cmd->hdr.command == UIM_RESET_WITH_TP_F) &&
             !(cmd->reset_with_tp.terminal_profile))
        {
            rsp_ptr->cmd_status = UIM_DONE;
        }
#endif

        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          /* Set the number of bytes in the internal buffer to zero */
          uim_int_term_profile_data.num_bytes = 0;
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          ++uim_generic_state_ptr;

          if (cmd->hdr.command == UIM_TERMINAL_PROFILE_F)
          {
             /* Store the number of bytes that was sent in the command */
             uim_int_term_profile_data.num_bytes =
                                               cmd->terminal_profile.num_bytes;
             /* Copy the data from the command into internal buffer */
             memcpy(uim_int_term_profile_data.data,
                    cmd->terminal_profile.data,
                    cmd->terminal_profile.num_bytes);
          }
        }
      } /* end case - UIM_TERMINAL_PROFILE_ST */
      break;

    case UIM_ENVELOPE_ST:                     /* Envelope State */
      {
        /* Return an error status if the command failed. */
        if ((rsp_ptr->cmd_status != UIM_DONE) &&
            (rsp_ptr->cmd_status != UIM_TOOLKIT_BUSY))
        {
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          ++uim_generic_state_ptr;
        }
      } /* end case - UIM_ENVELOPE_ST */
      break;

    case UIM_FETCH_ST:                        /* Fetch State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          ++uim_generic_state_ptr;
        }
      } /* end case - UIM_FETCH_ST */
      break;

    case UIM_TERMINAL_RESPONSE_ST:            /* Term response State */
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          return (UIM_CMD_ERR);
        }
        else
        {

          if ( completed_status == UIM_CMD_FETCH)
          {
            /* Tell GSTK a proactive command pending */
            uim_rpt_buf.rpt.terminal_response.proactive_cmd_pending = TRUE;
          }

          /* Skip to the next state. */
          ++uim_generic_state_ptr;
        }
      } /* end case - UIM_TERMINAL_RESPONSE_ST */
      break;

    case UIM_POLLING_INTERVAL_ST:
      {
        /* Skip to the next state. */
        ++uim_generic_state_ptr;
      } /* end case - UIM_POLLING_INTERVAL_ST */
      break;

#endif /* FEATURE_UIM_TOOLKIT */

    case UIM_STREAM_APDU_ST:
      {
        /* Return an error status if the command failed. */
        if (rsp_ptr->cmd_status != UIM_DONE)
        {
          cmd->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
#ifdef FEATURE_UIM_SUPPORT_CSIM
          /* Reset the ef fields in CSIM structure if file is not found
             for commands in default channel */
          if ( ( (rsp_ptr->cmd_status == UIM_NOT_FOUND)
#ifdef FEATURE_UIM_UICC
                 || (rsp_ptr->cmd_status == UIM_FILE_NOT_FOUND)
#endif /* FEATURE_UIM_UICC */
               ) &&
              (!(cmd->stream_iso7816_apdu.cmd_data[UIM_7816_APDU_CLASS_OFFSET]
               & UIM_LOGICAL_CHANNEL_MASK)))
          {
            uim_csim_last_sel_dir[uim_drv_slot].ef  = UIM_NO_FILE_SEL;
            uim_csim_last_sel_dir[uim_drv_slot].ef_sel = FALSE;

            if(uim_mode != UIM_PASSIVE_MODE)
            {
              /* Indicate that this is not an internal select for 7816 APDU */
              internal_sel_for_7816 = FALSE;

              /* Set uim_path_to_be_sel to MF so that we will not fail polling */
              memset(uim_path_to_be_sel, 0xFF, sizeof(uim_path_to_be_sel));
              uim_path_to_be_sel[UIM_PATH_TO_SEL_MF_INDEX] = UIM_MF_ID;
              uim_reselect_mf = TRUE;
              select_state_ctr = 0;
              uim_generic_states[0] = UIM_SELECT_ST;
              uim_generic_states[1] = UIM_DONE_ST;
              uim_generic_state_ptr = uim_generic_states;
              break;
            }
          }
#endif /* FEATURE_UIM_SUPPORT_CSIM */

          return (UIM_CMD_ERR);
        }
        else
        {
          /* Skip to the next state. */
          ++uim_generic_state_ptr;
        }
      } /* end case - UIM_STREAM_APDU_ST */
      break;

    case UIM_CLEAR_CACHE_ST:
      {
        /* Just skip to the next state */
        ++uim_generic_state_ptr;
      }
      break;

    case UIM_RECOVERY_COMPLETE_ST:
      {
        /* Just skip to the next state */
        ++uim_generic_state_ptr;
      }
      break;
    case UIM_DONE_ST:                         /* Done State */
    {
      /* Do nothing */
    } /* end case - UIM_DONE_ST */
      break;

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

    default:
      {
        /* Process command as a custom command */
        status = uim_custom_generic_command_response(rsp_ptr,cmd);
      } /* end case - default */
      break;
  } /* end switch - curr_uim_generic_state */

  /* Continue processing the command only if the response indicates success. */
  if ((status == UIM_CMD_SUCCESS) || (uim_reselect_mf))
  {
    /* Process the next state of this command. */
    uim_generic_command(cmd);
  } /* end if - command is still in progress */

  /* Determine if the command has completed */
  if (UIM_DONE_ST == *uim_generic_state_ptr)
  {
    /* Indicate the command has completed successfully. */
    return (completed_status);
  } /* end if - uim_generic_state_ptr is UIM_DONE_ST */

  return(status);

} /* uim_generic_command_response */


/*===========================================================================

FUNCTION UIM_PROCESSING_ENVELOPE_APDU

DESCRIPTION
  This function informs whether UIM is processing an envelope command.

DEPENDENCIES
  None

RETURN VALUE
  TRUE : UIM is processing an envelope command
  FALSE: UIM is not processing an envelope command

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_processing_envelope_apdu
(
  void
)
{
#ifdef FEATURE_UIM_TOOLKIT
  /* Check if we are processing ENVELOPE command */
  if( UIM_ENVELOPE_ST == uim_generic_states[0] )
  {
    return TRUE;
  }
#endif /* FEATURE_UIM_TOOLKIT */

  return FALSE;
} /* uim_processing_envelope_apdu */


/*===========================================================================
FUNCTION UIM_IS_TEST_ICCID

DESCRIPTION
  This function checks if the current ICCID is GCF ICCID or not.

RETURN VALUE
  Boolean
    TRUE : If current iccid is same as iccid used in GCF test cases
    FALSE: If current iccid is NOT same as iccid used in GCF test cases

DEPENDENCIES
  None
===========================================================================*/
boolean uim_is_test_iccid( 
  void
)
{
  if(memcmp(uim_iccid[uim_drv_slot], UIM_TEST_ICCID, UIM_ICCID_SIZE) == 0)
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}/* uim_is_test_iccid */

#endif /* FEATURE_UIM */
