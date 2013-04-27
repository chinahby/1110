/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        M A I N    U I M     T A S K

                  U S E R   I D E N T I T Y   M O D U L E

GENERAL DESCRIPTION
  This module is the server for all the commands to the User Identity Module
  in all the different modes of operation. The different modes being CDMA
  R-UIM, GSM SIM and WCDMA USIM. This module also does power management of
  the UIM to save battery power.

EXTERNALIZED FUNCTIONS

  uim_get_uim_status
    Returns the last known status of the UIM.

  uim_power_control
    Voting interface for power management of the UIM.

  uim_cmd
    External interface to queue a command for the UIM task.

  uim_task
    This is the entrance procedure for the UIM  task.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2001-2009 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/uim.c_v   1.30   14 Oct 2002 10:39:42   ckrishna  $
$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3.65/uim/uimdrv/src/uim.c#5 $ $DateTime: 2011/03/09 23:17:08 $ $Author: sratnu $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/03/10   ssr     Added support for BCSMS service in cdma service table
02/08/10   bd      Fixed POLLLING OFF issue
12/22/09   yb      Fix for voltage class switching G.C.F. failure on ULC
12/22/09   yb      1. Fix to avoid clock stop during recovery
                   2. Fix to add delays before clock stop and after clock restart,
                   to give time to card for finishing any pending internal processing.
08/10/09   rm      Error handling for T=1 protocol
08/10/09   rm      Fixes to have separate count for repeated technical problems
08/03/09   yb      Added support to read ENS ENABLED NV item
08/03/09   yb      Added support to read ENS ENABLED NV item
07/06/09   ps      Turn off the polling timer when the polling off proactive 
                   command comes in 
06/23/09   rm      Fixes to correct the featurization of the sleep APIs
06/04/09   rm      Fixes to handle technical problem reported by the card
06/01/09   ssr     Fixed compiler warnings
05/28/09   yk      Null ptr check before checking the contents
05/08/09   ps      Fixed 8200 compiler warnings
04/29/09   kk      Updated EF file path info for non-spec defined telecom EFs
04/24/09   tml     Merged back power down commnad handling in power down state
                   change
04/24/09   tml     Backed out power down command handling in power down state.
                   Fixed infinite loop issue with GSM Cingular file access
04/17/09   vs      Update to allow power down when already in power down state
04/09/09   kk      Fixed updating path_to_be_sel for proprietary EF access
04/07/09   nb      Fix for multiple select on MF
04/06/09   js      Support for accessing EFs by path with proprietary DFs
03/25/09   sun     Added support for Virtuim
03/11/09   kk      Fixed re-queueing of external RESET command during a reset
02/25/09   yk      Bring MDM8200 drivers
02/25/09   rm      Fix for RX break errors in Idle and Non-Idle
02/23/09   nd      Fixed linker error when FEATURE_VIRTUAL_SIM is defined
02/20/09   vs      De-featurized inclusion of fs_public.h header file
02/20/09   ssr     Fixed Compiler error
02/18/09   vs      Updated IC-USB logic to execute only if IC-USB is selected
02/17/09   ssr     Update latest EFS filesystem call
02/04/09   vs      Populated GSDI historical bytes from USB UICC ATR data
01/14/09   vs      Fix to indicate HCLK, MCLK restrictions during uim_init()
12/03/08   js      Support for accessing GSM Cingular DF by path
11/26/08   nb      Klockwork Fixes
11/19/08   vs      Moved sleep register call from uim_dev_init() to uim_init()
11/13/08   rm      Use timer callback for command response timer to handle the
                   work waiting timer expiry immediately in the timer context.
11/11/08   yb      Fixed compilation error when FEATURE_GSTK is not defined
11/05/08   vs      Update to allow UIM clock stop only after sending the first
                   instruction to the card in passive mode
10/07/08   vs      Fix to power up UIM LDO after card power down, vote TCXO off
                   permanently when IC-USB interface is selected, clear poll
                   timer before requesting ATR
10/06/08   yb      Sending BIP envelope after TR while BIP is working
09/19/08   tml     Re-select MF to force directory maintainance to MF when
                   received a sw1 warning with normal return
09/23/08   js      Added LTK Support for USB UICC
09/22/08   rm      Fix for parity errors in UART_DM
09/19/08   rm      Work waiting time fix for UART_DM
09/18/08   js      Fixed featurization for USB UICC.
09/17/08   vs      Updated UIM USB ICCD poll delay timer logic
09/17/08   js      Implemented UIM Card interface selection based on first
                   instruction class NV
09/15/08   vs      Added support for UIM USB ICCD pre-suspend timer
09/08/08   tml     Fixed Get ATR issue with USB featurization
09/05/08   vs      Moved fallback timer and signal clearance to UIM USB module
08/26/08   kk      Few comments from uim directory management review
08/22/08   vs      Added support for data re-poll request by USB UICC and ISO
                   fallback mechanism
08/08/08   vs      Removed featurization UIM_CLOCK_STOP_ALLOWED
08/05/08   vs      Renamed start and stop clock functions appropriately
07/22/08   js      Fixed UIM_MAX_PATH_ELEMENTS issue. Updated path length to 4
                   for all UIM commands. MAX path length in MMGSDI is set to 5.
07/24/08   vs      Fixed UIM USB UICC related featurization
07/17/08   vs      Added support for UIM USB UICC ICCD interface
06/21/08   kk      Added PKCS15 Support
04/30/08   kk      Added support for MMS EFs
03/20/08   nk      Added OMH EF and CST Support
01/02/08   jk      Added functions to check and remove memory allocs on cmd q
11/19/07   js      Radio Access Technoligy Balancing support
11/19/07   sun     Added file paths for ISIM and MFLO
11/08/07   vs/nk   Added EUIMID and OMH feature support
09/10/07   tml     Add MFLO support
08/16/07   jk      Rearranged featuization to support DATA Auth on 7K
07/02/07   sun     Added support for ODD INS Class
05/21/07   jk      Provide API to check if uim in recovery mode
05/22/07   nk      removed retry setting for jcdma feature to resolve power up
                   issue
05/21/07   sp      Fixed potential buffer overflow in uim_get_file path.
05/11/07   wli     Changes for JCDMA requirements
04/26/07   sp      Featurize for LTK
05/07/07   sun     Fixed Lint Error
05/02/07   sun     Added Support for MBMS
04/17/07   sp      Featurize for LTK
03/21/07   jk      Include tmc.h instead of mc.h if FEATURE_CDMA1X not defined
02/28/07   pv      Replace strncat with std_strlcat.
02/27/07   jk      Reduce Command queue size for memory reduction
02/22/07   pv      Revert NULL check for T=1 protocol.
02/15/07   pv      Featurize code using uim_current_protocol.
02/13/07   pv      When a recovery command was from uim_free_q, return
                   it back to the queue instead of just throwing it away.
02/13/07   pv      Add uim_clear_poll_timer and uim_set_poll_timer functions
                   to manage the poll timer.  These functions wrap the logic of
                   trying to be in synch with the DRX cycle while making sure
                   that we wake-up if there was no DRX on time.
02/12/07   pv      Do not ignore UIM_TRANSACTION_SIG even if there are
                   bytes in the FIFO since they could have been NULL bytes
                   that we might be receiving for a long time.
                   Clear the UIM_TRANSACTION_SIG when a time-out signal is
                   received.
02/05/07   pv      Avoid usage of cmd_ptr_in_use when NULL.
                   Avoid passing NULL cmd_ptr to uim_process_t_1_block.
01/25/07   jk      Fixed compile problem when FEATURE_UIM2 is defined instead of FEATURE_UIM1
01/16/07   nk      Append /apps/ to efsfilename when apps mount features defined
01/15/07   pv      Use strncat instead of strcat.
12/12/06   tml     Add uim_reset_dir_for_slot_channel to reset the ef/df/adf
                   info for a particular slot and channel
11/13/06   pv      Do not poll if in passive mode and protocol is unknown.
11/09/06   pv      Move logging of time-out information before resetting the
                   flag indicating that a command is in progress.
11/07/06   pv      Ignore timeout if we have a byte present even before PPS.
09/28/06   pv      Process the response for the external reset when internal
                   reset is issued as a result of that external reset command.
09/27/06   pv      Allow external select on ADF to use 0x7FFF if the
                   same application is already active.
09/20/06   jk      Added necessary lint suppresion due to featurization necessary
                   for 7200/7500 RPC Library convergance
09/11/06   jk      Changed featurization so that 7200 and 7500 generate identical
                   RPC Files
08/25/06   jar     Added handling for additional MMGSDI Card Error
                   enumerations in uim_gsdi_event_cb_func.
08/10/06   sp      Report poll error without retries for bad status words
                   when FEATURE_UIM_RPT_BAD_SW_ON_POLL is enabled
08/09/06   pv      Featurize the usage of members of nv_uim_config_params_type
                   under FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS for
                   targets not picking up the NV dependencies
08/01/06   sp      Changes to support when fetch fails.
07/28/06   jk      Fixed Error Fatal with No Card
07/27/06   jk      Lint fix
07/17/06   jk      Support for APDU logging to EFS
07/05/06   pv      Do not re-try authenticate or run gsm commands.
07/03/06   pv      Reset the uim_internal_command_used flag when uim_reset_uim
                   is called
06/28/06   pv      Do not switch instruction class on time-out if operating
                   in T=1 mode.
06/21/06   pv      Flush commands before notifying error.
06/14/06   pv      Do not ignore any max errors due to having overrun
                   or bad status words.
06/12/06   pv      Use uim_mode instead of uim_passive_mode.
                   Make all resets cold.
                   Initialize uim_internal_command_used when popping out a cmd.
                   Remove FEATURE_UIM_TRY_TO_RECOVER_FROM_ERRORS featurization.
                   Do not directly powerdown the interface when an internal
                   command times out while not doing a power-up.
                   Do not poll while in recovery mode.
                   Set and use uim_config_params.
                   Consider Transaction timer expiry as a regular time-out.
06/04/06   jar     Made local var back_ui_uim_rpt_buf a static to keep from
                   creating a var of 544 bytes on the stack
05/22/06   pv      Change timer_always_on_group to uim_always_on_timer_group
                   which is a locally defined group and is never disabled.
05/15/06   jk      Added extra NULL Param check in uim_log_timeout_info
04/24/06   pv      Added code to read uim_config_params from NV
04/21/06   pv      Featurize out code using tmc_heap_small based on
                   FEATURE_LOW_MEMORY_USAGE.
04/19/06   pv      Removed INLINE for uim_get_file_path
04/06/06   sun     Send the Error Report in case of error in Passive mode
04/11/06   pv      Change the call to UIM_READ_STATUS macro to a function call.
                   Add more information to uim_reset.txt
04/03/06   pv      Lint Fixes
03/29/06   pv      Set the poll timer followed by a poll error in
                   uim_process_cmd_err.
03/21/06   tml     added cfis support
03/14/06   pv      Added code to wait until uim_mv_init is done by tmc.
                   Merged missing HZI support.
03/09/06   jar     Merged in from Branched HZI
03/07/06   jk      Fixed featurization compile issue for SC-1X
02/16/06   pv      Featurized the code in uim_can_ignore_timeout based on
                   FEATURE_USE_TIME_VU.  Added support for UIM_USIM_EHPLMN
02/15/06   pv      Added function uim_can_ignore_timeout to check if a timeout
                   signal can be ignored and check if the timeout can be
                   ignored for T=1 protocol as well.
02/14/06   nk      Merge for: Use heap_small when the size of get response
                   allocation is less than 64 bytes; Remove uim rows from usim
                   path table for enums removed; and Added support to cache the
                   get response data and not do a get response if the item is
                   already cached.  Also, expose a new API to not send down the
                   select for the sake of getting the get response information
02/02/06   pv      Call the correct API to power down in order to do a
                   cold reset for dual slot.
01/30/06   pv      Remove code to switch instruction classes on a timeout
                   during power-up based on
                   FEATURE_UIM_NO_SWITCH_INST_ON_WWT_EXPIRY
01/30/06   pv      Do a cold reset instead of a warm reset when we timeout
                   during the power-up procedures
01/30/06   pv      Notify GSDI about the reason for power down.  Do not re-try
                   commands while in passive mode.
11/14/05   pv      Lint Fixes
10/27/05   pv      Code to turn off the interface if no ATR is received whthin
                   40,000 UIM clock cycles
10/26/05   pv      Allow uim_flush_command to be used without disturbing
                   the uim_rpt_buf
10/13/05   wli     Fixed comilation error when defining FEATURE_UIM2 alone.
09/13/05   jk      Added changes to keep interface powered down during voltage
                   switch for minimum time specified in ISO7816
05/26/05   jk      Added uim_get_uart_status() function to safely call
                   UIM_READ_STATUS() only when clock is still on
08/29/05   tml     Fixed compilation
08/29/05   jk      Added functions to get path to files from EF file ID
08/25/07   pv      Allow RESET command when in powered down state
08/04/05   wli     Added comments
08/02/05   wli     Added HOT SWAP feature.
07/28/05   pv      replaced UIM_MAX_NUM_CDD_ATTEMPTS with UIM_MAX_NUM_ATTEMPTS
                   where not necessary.
07/27/05   jk      Made work waiting time array for dual slots
07/26/05   pv      Internal polling message is now HIGH
07/22/05   pv      Poll with no data expected in response when not in a call.
07/22/05   pv      Define the polling timer to be in the timer_always_on_group
                   if FEATURE_USE_TIME_VU is defined
07/18/05   sun     Added support for USIM files in ENS
07/19/05   tml     Fixed switch slot issue
06/22/05   pv      Reset the transaction timer when we get resp from driver.
                   Allow power_up and power_down commands in passive mode
06/15/04   tml     Use correct type structure for new READ/WRITE cmd enums
06/13/05   pv      Code to support UIM commands with allocated memory.
06/02/05   pv      Code for FEATURE_UIM_ISIM & FEATURE_UIM_SESSION_MANAGEMENT
05/17/05   jk      Adjusted Fix for uim_clock_stop
05/13/05   jk      Only allow uim_clock_stop when uim_cmd_q is empty
05/10/05   pv      Code for BT sim access - Added checks to see if a
                   command is valid in the current status of UIM.  Allow
                   UIM_POWER_UP_UIM_PASSIVE_F when the UIM status is faulty.
05/10/05   sst     Lint fixes.
04/25/05   jk      Added FEATURE_UIM_RUIM around new HW ESN issue to fix compile
                   issue for non-RUIM
04/22/05   jk      Added function to return HW ESN for BREW
03/14/05   wli     Added 820B EFs
03/11/05   pv      Added code to select USIM ADF when authenticate
                   command comes in and a DF under USIM is currently selected
02/28/05   sun     Added file - EF MWIS
02/09/05   pv      Check if the CMD_RSP_SIG is received for the slot which
                   has an error to continue or ignore the signal.  Fix
                   compilation issue when both multiple voltage and dual_slots
                   is turned on.
01/04/05   sun     Added support for Cingular Wireless
02/03/05   pv      Added code to indicate that we have received a command_response even if
                   the uim_status is ERR if we have already received the task_stop signal
01/26/05   pv      Added additional check if atr is received if multiple
                   voltage support is on and we got a max_parity error
01/21/05   wli     Extend work waiting timer value for an out of spec RUIM card.
01/21/05   jk      Added gsdi_cmd_ext which checks GSDI CMD Q size to replace gsdi_cmd.
01/21/05   jar     Fixed Dual Slot Compile Error.
12/28/04   pv      Added code to handle timeout signaling from the driver based on
                   time stamp.  Also included more information in timeout_logging like
                   the ATR bytes, signal's pending in the UIM TCB etc.
                   Changed the timeout_logging function to be called every where
                   so that we will do a power down or reset before logging.
                   Added code to handle 27.11.2.6 (WWT exceeded during PPS phase)
                   when multiple voltage class support is turned on.
12/16/04   jk      Merged changes for Orange support from mainline
12/14/04   pv      Added check to make sure that cmd_ptr is not null
                   before invoking uim_process_card_response.
12/06/04   pv      Added code to log UIM driver/server status when a timeout occurs
11/22/04   jk      Added 3GPD support for CDMA service table check
11/21/04   wli     Dual slot merge and debug.
11/16/04   pv      Added code to power down the interface when a currupted
                   ATR is received and do not try a higher voltage if we
                   already have the voltage information from the ATR.
                   Cr. No. 51127
11/15/04   pv      Added definition for uim_t_1_bgt to be used as block gaurd time
11/01/04   pv      Include the work waiting report while sending the
                   link_established command to gsdi
10/08/04   jar     Updated Number of UIM Items for GSM SIM and USIM
10/06/04   jar     Merged in 4.2x Additional EFs
09/20/04   pv      Brought in support for dual slots when powering down the
                   interfaces because of maxing error count.  Also added
                   code to signal the requestor when the current command fails.
                   The new code checks if there is a next voltage class available
                   upon max rx break error and tries it instead of powering down.
09/01/04   jk      Added support for HRPD AN Authentication services in R-UIM
09/01/04   ck      Added support for CPHS files
09/01/04   ck      Added support for CSIM
08/30/04   pv      Take the protocol information from UIM variables instead of
                   requesting GSDI for the protocol information.
08/10/04   pv      Added more messages to debug timeout and cleared the error
                   flags conditionally.
08/02/04   ck      Reset the command_in_progress flag when the command was
                   rejected by the different modules.
07/19/04   pv      Added potential fix for a timeout issue by ignoring the time
                   out signal when a valid byte is present in the Rx FIFO.
07/14/04   wli     Changed Orange file path.
06/16/04   ck      Merged support for including protocol information in link
                   established message to GSDI from Cheetah branch
06/16/04   wli     Added ORANGE DF and its related EFs.
06/11/04   wli     Added EFs to support KDDI.
06/03/04   ts      Fixed featurization misspelling -
                   FEATURE_UIM_SUPPORT_DUAL_SLOT(S).
05/17/04   ck      Moved the featurizations around to send DF presence info
                   for single slot in GSDI Link est message
                   Also added ATR information in GSDI Link est
                   Added support to indicate whether the command was transacted
                   with the card.
04/16/04   ts      Fixed problems with error condition processing due to bad
                   merge.
04/08/04   wli     Featurize for non-gsdi targets.
03/26/04   ts      Added support to bring down the interface for excessive null
                   procedure bytes.
                   Added processing to bring down the interface for excessive
                   receive error conditions.
03/15/04   ts      Change the instruction class to GSM if card times out on
                   USIM instruction class.
03/12/04   ck      Removed the check for uim_powerdown while starting and
                   stopping the clock for dual slots.
03/08/04   ck      Added suport to read specific CDMA files from both the slots.
02/25/04   ck      Fixed a bug where the second slot was also initialized when
                   the first slot timed out after initial power up.
02/18/04   ck      Fixed Lint errors.
02/09/04   ck      Changed the if-else to switch stmt in the function uim_get_
                   slot() for certain GSM files.
02/05/04   ck      Added support to access certain GSM files from either slot
                   irrespective of operational slot.
02/03/04   ts      Added a check to wait until the power down vote allows power
                   down or when power down is required until powering down the
01/29/03   ck      Removed UIM_MC from the CDMA traffic channel mask
                   Cleared CMD_Q_SIG from the I-mask upon cmd completion before
                   processing the power up of the second slot. Clearing the sig
                   after the function uim_process_slot_power_up resulted in
                   processing of another command before completing the power up.
01/21/04   ck      Fixed a bug where the poll timer was not being reset when a
                   91 xx (Fetch) was returned for a Status response.
12/19/03   jk      Added processing for R-UIM 3GPD Services.
12/15/03   wli     Fixed GSM ACCESS compilation error.
12/10/03   ck      Cleaned up polling for dual slots.
12/09/03   ck      Commands between polls do not reset the poll timer.
12/04/03   wli     Added FEATURE_UIM_TOOLKIT feature switch around toolkit
                   related varibles.
12/04/03   ck      Added a feature to force the first instruction class as GSM
                   to be used for dual slot targets.
                   Modified the aggregation of bits in CDMA_TC and GW_TC.
11/04/03   ck      Ensured that the CMD_Q_SIG was added to the mask after the
                   processing of the PREF_SLOT_CHANGE_F command.
10/03/03   ck      Added support to stop the clock irrespective of the vote and
                   turn it ON only when needed to process commands.
                   Merged in the support to use the flag uim_atr_pps_done
09/17/03   ck      Added checks to make sure that PREF_SLOT_CHANGE command is
                   not rejected when the status for the slot specified in the
                   command header is invalid.
09/12/03   ck      Changed the polling architecture to make sure that the
                   interval between STATUS command is less than 30s while in
                   traffic channel even if there are other accesses to the card
                   Also, poll the card as soon we get an indication of the
                   traffic channel setup.
09/02/03   ck      Added support to set the CDMA directory present in the nvruim
                   structure for dual slots.
08/20/03   ck      Use the slot in the command for Invalidate and Rehabilitate
                   commands.
08/15/03   ck      Changes from code review.
08/14/03   ck      Added support for dual slots.
08/04/03   ck      Reset the poll timer when the POLL_TIMER_SIGNAL was set and
                   UIM status was CLK_OFF_S and the card in use was not a
                   pro-active card.
                   Included UIM_TC to the check for arriving at the poll timer
                   delay when in traffic channel.
07/09/03   ck      Initialized the overrun error flag to FALSE.
                   Fixed a comment line that threw up a warning.
                   Removed the featurization around the funtion uim_nv_init().
06/25/03   ck      Added check to make sure that we select the USIM ADF even
                   when we are in the second level DF under USIM ADF before
                   processing the AUTHENTICATE command.
06/19/03   ck      Moved the code that handles command error into a function and
                   added support to handle overrun error.
06/05/03   ck      Reset the curr_app data structure in the function
                   uim_reset_dir().
06/03/03   ck      Added support to reset the UIM based on the status rsp
                   UIM_INSTRN_CHANGE and UIM_CONVENTION_CHANGE.
05/18/03   wli     Fixed compilation error when GSM ACCESS feature turned on.
05/01/03   ck      Removed the featurization around forcefully voting off the
                   different power control bits upon receiving TASK STOP.
04/28/03   ts      Added support for multi-Voltage classes.
04/28/03   ts      Added support for T=1 protocol.
04/28/03   ck      Moved the powerdown processing code as the last check in the
                   uim_task function. This greatly improves the standby time as a
                   result of stopping the clock right after the command(poll) is
                   completed.
04/25/03   wli     Declaired temp_uim_aid that was missing.
04/24/03   ck      Fixed an issue with directory maintenace that was causing an
                   issue with selection of Telecom PBR following a failure in
                   selecting USIM PBR.
02/26/03   jk      Added UTIL and WIM functionality
03/14/03   ck      Added a check to see if the card is a pro-active card
                   before polling the card when the status is CLK_OFF_S.
03/04/03   ck      Added the function uim_get_enum_value_for_path.
02/13/03   wli     Changed to support GSTK.
02/02/03   ck      Lint changes.
01/17/03   ck      Added support for CHV operations in USIM protocol to
                   internally select the ADF specified in the cmd if reqd
01/07/03   ts      Added EFrplmnat
11/21/02   ts      For 1x only builds, vote everyone off when receiving a task
                   stop.  Set status to ERR to ensure UIM does not power-up
                   after task stop.
11/15/02   ts      Ensure power down status indicates it is okay to power down when
                   powering down due to bad UIM link.
10/31/02   ts      Removed extraneous parenthesis in power down/clock stop
                   determination.
10/29/02   ck      Declared the boolean uim_atr_recd and use it to check if the
                   command count needs to be incremented for the internal
                   power up command so that the command is not tried again.
10/25/02   ck      Added support to indicate the poll error with a new status to
                   GSDI
10/24/02   ts      Added support for work waiting time
10/24/02   ts      Set command_in_progress to false when command times out.
10/14/02   ck      Report error for GSM ECC if one attempt fails.
10/11/02   ck      Added support for reading the first instruction class to be
                   used in the SELECT command from NV in the context of TMC.
10/02/02   ts      Wait for requests to be finished before powering down UIM
                   for task stop.  Acknowledge task stop after powering down.
10/02/02   ts      Changed the featurization around the GSDI interface.
09/20/02   ts      Set the cmd_ptr to NULL when finished processing the
                   command.
08/27/01   ck      Added support for multimode architecture to start and stop
                   the UIM task.
                   Added support for Multimode GSDI.
08/14/02   wli     Modified the feature defines around stop clock or power down.
07/25/02   ck      Modified the feature defines around polling while in idle
                   mode to be only applicable when the UIM is proactive.
                   Modified the input parameters to STATUS command for an
                   internal poll.
                   Fixed a path selection issue that did not include MF in the
                   path when just the AID was selected.
07/09/02   ck      Added support for AID selection by path.
                   Modified powerdown of UIM to be featurized out for USIM and
                   allowed for clock stop based on the return type.
05/21/02   plm     Removed conditional code for T_KOVU and T_MSM5200 for calling
                   tmc_task_start(). This code is not necessary for GSM or MSM5200
                   since they already redefine mc_task_start() to tmc_task_start()
                   in mc.h.
05/15/02   ts      Featurized else for clock stop code when toolkit is not
                   enabled.
05/01/02   ts      Need to set the poll timer with the new timer duration when
                   the duration changes between the proactive duration to the
                   traffic channel duration.
04/29/02   ck      Added UIM_USIM_SESSION to uim_ctl as default for USIM targets.
04/09/02   ck      Modified the #defines to implement Clock stop for USIM.
04/03/02   ts      Implemented polling control for Proactive commands.  Polling
                   duration depends on whether MC votes to keep the UIM power
                   on.  This indicates the phone is in the traffic channel.
                   Otherwise, the polling frequency is the Proactive frequency.
02/11/02   ts      Changed the code surrounding uim_get_file_path to include
                   processing for items (EFs) whose paths are defined by the
                   card.
02/01/02   ts      Process a new status from processing a UIM command.  The new
                   status indicates the card does not support 3V technology.
01/25/02   ts      Cleared the poll timer, poll timer signal and mask the poll
                   timer task signal from the UIM task to ensure the poll does
                   not happen when processing a command.
01/25/02   ts      Added support for Kovu.  Kovu is same as MSM5200.
01/22/02   ck      Added support for clock stop mode.
01/08/02   ck      Wrapped the uim_return_cdma_svc_availabililty function
                   call around the feature FEATURE_UIM_RUIM
12/18/01   ck      Added support to access GSM EFs when build for R-UIM and
                   modified the featurization around GSDI code.
                   Added support for EFs in DCS1800 DF.
13/12/01   ts      Cleaned out the last selected directory path when resetting
                   the UIM card.
12/13/01   ts      Added some comments and fixed format of code.
12/04/01   ts      Added support for UTK.
11/19/01   ts      Fixed a polling bug for USIM.
10/11/01   ck      Removed the featurization for DOG reporting for MSM5200.
                   Modified the path for EF HLPMN selector in USIM ADF.
                   Enhanced the SEEK interface to include path in the command
08/10/01   ck      Fixed the problem where the command header status was not
                   set to RECEIVED when the processing of the command had
                   begun.
                   Fixed the problem where the card was being reset even after
                   it was declared as faulty.
07/27/01   ts      Set the UIM_AUTH voter flag on startup so that the UIM stays
                   powered on until Auth gets initialized (reads call count).
07/23/01   ck      Added support for EF ECC,SPC STATUS and MAX_PRL.
                   Added a function to export the response to store esn_me.
                   Fixed a bug in which a second select to the same EF did not
                   fill the path to be selected array.
05/28/01   ck      Added support for R-UIM, GSM and USIM modules.
01/15/01   ck      Created module from AUTH module rev 1.5 from Common
                   archive.

===========================================================================*/

/* <EJECT> */
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "target.h"
#include "customer.h"

#ifdef FEATURE_UIM

#ifdef FEATURE_USE_TIME_VU
  #include "timer.h"
#endif

  #include "intconv.h"
  #include "rex.h"
  #include "task.h"
  #include "dog.h"
  #include "msg.h"
  #include "err.h"
#ifdef FEATURE_CDMA1X
#include "mc.h"
#else
#include "tmc.h"
#endif /* FEATURE_CDMA1X */
  #include "queue.h"
  #include "uim.h"
  #include "uimi.h"
  #include "uimdrv.h"
  #include "uimgen.h"
  #include "memory.h"

  #include "fs.h"
  #include "stdio.h"

  #ifdef FEATURE_UIM_RUIM
    #include "uimcdma.h"
  #endif /* FEATURE_UIM_RUIM */

  #ifdef FEATURE_UIM_GSM
#error code not present
  #endif /* FEATURE_UIM_GSM */

  #ifdef FEATURE_UIM_USIM
#error code not present
  #endif /* FEATURE_UIM_USIM */

  #ifdef FEATURE_UIM_ISIM
    #include "uimisim.h"
  #endif

  #ifdef FEATURE_UIM_MFLO
#error code not present
  #endif

  #if defined( FEATURE_UIM_T_1_SUPPORT )
    #include "uimt1.h"  /* T=1 protocol support */
  #endif /* FEATURE_UIM_T_1_SUPPORT */

  #if (defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_USIM)) && \
     !defined (FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
  #endif /* USIM || GSM */

  #if defined (FEATURE_GSDI) || defined(FEATURE_UIM_USIM)
    #include "gsdi_exp.h"
    #include "gsdi.h"
  #endif /* FEATURE_MMGSDI */

  #ifdef FEATURE_GSTK
    #include "gstki.h"
  #endif

  #ifdef FEATURE_UIM_UTIL
    #include "uimutil.h"
  #endif /* FEATURE_UIM_UTIL */

  #ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
    #include "nvruimi.h"
  #endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

     #include "fs_public.h"

  #include "AEEstd.h"

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
#if defined(FEATURE_NEW_SLEEP_API) && defined(FEATURE_UIM_USES_NEW_SLEEP_API)
  #include "sleep.h"
#endif /* FEATURE_NEW_SLEEP_API && FEATURE_UIM_USES_NEW_SLEEP_API */

#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
#include "db.h"
extern boolean  InitRUIMAddrBkCacheCb(void);
extern boolean  wms_ruim_init_stepbystep(void);
#endif
#ifdef FEATURE_UIM_TOOLKIT_UTK
#ifdef FEATURE_UTK2
#include "uimtk.h"
extern set_UTK_session_status(byte st);
#endif
#endif

/* <EJECT> */
/*===========================================================================

            DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
  #define UIM_JCDMA_POLL_INTERVAL 5000
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */

#define data_to_write_size 1500
/* Maximum number of Buffers to be placed in the uim_free_q */
#ifdef FEATURE_UIM_MEMORY_REDUCTION
  #define                      UIM_NUM_CMD_BUFS 10
#else
  #define                      UIM_NUM_CMD_BUFS 20
#endif /* FEATURE_UIM_MEMORY_REDUCTION */

#define UIM_MAX_COMMAND_RETRIES       0x03

/* Pool of buffers to be placed in the uim_free_q */
LOCAL uim_cmd_type uim_cmd_pool[ UIM_NUM_CMD_BUFS ];

/* Queue to hold commands for uim */
q_type uim_cmd_q;

#ifdef FEATURE_UIM_LOG_TO_EFS
#ifdef FEATURE_UIM_LOG_APDU_TO_EFS
/* Local buffer to intermittently hold apdu logging data until written to EFS */
LOCAL uim_efslog_buffer_type  uim_efslog_apdu_buffer;

/* Stores the previously processed character's attribute
    so we can tell if it changes */
LOCAL byte              uim_efslog_previous_char_attribute = UIM_LOG_ATTRIB_INIT;

/* Secondary buffer to hold data that is being written to EFS */
LOCAL uim_efslog_buffer_type  uim_efslog_apdu_outgoing_data_buffer;

/* Holds file ID for APDU Log File */
LOCAL int                     uim_efslog_apdu_file_handle = 0;
#endif /* FEATURE_UIM_LOG_APDU_TO_EFS */

/* Flag so that we can tell if EFS logging is in progress */
LOCAL boolean                 uim_efslog_logging_in_progress = FALSE;

/* Count to tell how many times we've written to the EFS Log file */
LOCAL int                     uim_efslog_save_count = 0;
#endif /* FEATURE_UIM_LOG_TO_EFS */

/* Queue to hold free buffers to be placed on the uim_cmd_q */
q_type uim_free_q;

/* Count for repeated technical problems */
LOCAL int uim_inc_tech_prob_cnt = 0;

/* Buffer to hold report to be returned to task issuing command */
uim_rpt_type uim_rpt_buf;

/* Timer definitions */
LOCAL rex_timer_type uim_rpt_timer;       /* Watchdog timer */
/* Poll timer which might not be able to wake-up the ME from sleep */
LOCAL rex_timer_type uim_no_wake_up_poll_timer;
#if defined(FEATURE_USE_TIME_VU)
/* Polling timer capable of waking up the ME from sleep */
LOCAL rex_timer_type uim_wake_up_poll_timer;
#endif
rex_timer_type uim_cmd_rsp_timer;         /* Command Response Timer */
rex_timer_type uim_delay_timer;           /* Command Response Timer */
rex_timer_type uim_transaction_rsp_timer; /* Transaction Response Timer */

#ifdef FEATURE_UIM_CARD_ERR_UIM_LDO_PUP
#error code not present
#endif /* FEATURE_UIM_CARD_ERR_UIM_LDO_PUP */

rex_timer_cnt_type  uim_work_waiting_time[UIM_NUM_DRV_SLOTS]; /* Work waiting timeout (msecs) */
rex_timer_cnt_type  uim_original_work_waiting_time[UIM_NUM_DRV_SLOTS];
rex_timer_cnt_type  uim_extended_work_waiting_time[UIM_NUM_DRV_SLOTS];

#ifdef UIM_DRIVER_TIMESTAMP
/* Variable to hold the work waiting time in terms of SLEEP ticks */
int32 uim_work_waiting_time_sleep_ticks[UIM_NUM_DRV_SLOTS];
#endif
rex_timer_cnt_type  uim_WI[UIM_NUM_DRV_SLOTS];           /* Work waiting time parameter */
  #if defined(FEATURE_UIM_T_1_SUPPORT)
rex_timer_cnt_type  uim_t_1_bgt;            /* Block guard time when T=1 */
  #endif

uim_cmd_type uim_static_cmd_buf;
/* Static buffer for internally generated commands */

LOCAL uim_cmd_type *uim_static_cmd_ptr;
/* Pointer to static command */

/* Request buffer for a UIM command */
uim_req_buf_type uim_req_buf;

#ifndef FEATURE_VIRTUAL_SIM
/* Status of UIM initialization */
uim_status_type uim_status = UIM_UNINITIALIZED_S;
#endif

/* Response buffer */
LOCAL uim_rsp_buf_type uim_rsp_buf;
uim_rsp_buf_type *uim_rsp_buf_ptr;

/* Indicator as to whether static command buffer was used */
LOCAL boolean uim_static_buffer_used = FALSE;

/* indicate if the command is in progress */
LOCAL boolean uim_command_in_progress = FALSE;

/* indicates if a command has been re-queued */
LOCAL boolean uim_command_requeued = FALSE;

/* Indicates if UIM can be powered down */
boolean uim_powerdown = FALSE;

/* Indicates whether UIM clock stop is allowed in passive mode */
boolean uim_passive_clk_stop_allowed = FALSE;

/* Indicates we have received a task off from MC.  We must power the interface
   down. */
boolean uim_powering_down_task = FALSE;

/* This is set to UIM_MC in R-UIM targets so that the UIM is powered down
   only after MC indicates that is finished with power up */
uim_voter_type uim_ctl = (uim_voter_type)
                         (
                         (int)UIM_NONE
  #ifdef FEATURE_UIM_RUIM
                         | (int)UIM_MC
    #ifdef FEATURE_AUTH
                         | (int)UIM_AUTH
    #endif /* FEATURE_AUTH */
  #endif /* FEATURE_UIM_RUIM */
  #ifdef FEATURE_UIM_USIM
#error code not present
  #endif /* FEATURE_UIM_USIM */
                         );

/* Indicates if the mobile is to be powered up and is set to FALSE when
   the power up is complete */
boolean uim_me_powerup = TRUE;

/* This holds the polling duration of the UIM
   Initially this is set to the in traffic poll time */
LOCAL dword uim_poll_time_delay = UIM_POLL_TIME;

/* Flag to indicate that the task is starting up */
LOCAL boolean uim_task_startup = TRUE;

/* Data structure that contains the last selected directories */
uim_last_sel_dir_type uim_last_sel_dir[UIM_MAX_CHANNELS][UIM_NUM_DRV_SLOTS];

/* This array contains the path to be selected that is filled in by the
   generic module */
word uim_path_to_be_sel[UIM_MAX_PATH_ELEMENTS];

/* Holds the mode for the command ( CDMA, WCDMA, GSM) */
LOCAL uim_command_mode_type uim_cmd_mode;

/* Contains the ADF used for internal selection */
uim_aid_type uim_adf_to_be_sel;

boolean clear_timeout_on_response = TRUE;

/* Boolean to indicate if we received ATR */
boolean uim_atr_received = FALSE;

/* Boolean to indicate if ATR has been received */
boolean uim_atr_pps_done;

/* Flag to indicate if an external command has been translated to an
   internal command */
LOCAL boolean uim_internal_command_used = FALSE;

  #ifdef FEATURE_UIM_TOOLKIT
/* Information on the toolkit capability of the card */
LOCAL uim_proactive_uim_data_type uim_type = { FALSE, FALSE};

/* Time delay for polling */
dword uim_proactive_time_delay = UIM_POLL_TIME;

    #ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
/* Information on the toolkit capability of the card in second slot */
uim_proactive_uim_data_type uim_type_2 = { FALSE, FALSE};
    #endif
  #endif /* FEATURE_UIM_TOOLKIT */

  #ifdef FEATURE_UIM_RUIM
/* Response to STORE_ESN_ME command */
uim_store_esn_me_rsp_type uim_store_esn_me_rsp;
  #endif /* FEATURE_UIM_RUIM */

  #if defined (FEATURE_UIM_UICC)
/* First instruction class for the command */
nv_uim_first_inst_class_type uim_first_inst_class;

    #if defined (FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES)
/* Holds whether we got the voltage information from ATR or not */
boolean voltage_class_known_from_atr = FALSE;
    #endif

  #endif /* FEATURE_UIM_UICC */

  #if defined( FEATURE_UIM_T_1_SUPPORT )
/* Preferred protocol of operation */
nv_uim_pref_protocol_type uim_nv_pref_protocol = NV_UIM_PREF_PROTOCOL_T_1;
  #endif /* FEATURE_UIM_T_1_SUPPORT */

/* Flag to indicate if the driver has encountered an overrun error */
boolean uim_overrun_error = FALSE;

/* Flag to indicate that un-known status words have been received */
boolean uim_bad_status_words_error = FALSE;

/* Flag to indicate if the driver has encountered too many overrun errors */
boolean uim_max_overrun_error = FALSE;

/* Flag to indicate if the driver has encountered too many rx break errors */
boolean uim_max_rx_break_error = FALSE;

/* Flag to indicate if the driver has maxed the parity error count */
boolean uim_max_parity_error = FALSE;

#ifdef UIM_DRIVER_TIMESTAMP
/* Flag to indicate if the driver has signaled a timeout signal
 * before the real-timeout because the driver received a byte but the
 * time stamp indicates that exceeded the timeout value
 */
boolean uim_timeout_signaled_by_driver = FALSE;

/* Variable to hold the difference in time-stamps in SLEEP ticks */
int32 uim_num_sleep_ticks_elapsed = 0;
#endif

/* Flag to indicate that a poll is pending */
boolean uim_poll_pending = FALSE;

  #if defined (FEATURE_MMGSDI) || defined(FEATURE_UIM_USIM)
/* Pointer to GSDI command */
LOCAL gsdi_uim_link_established_T  gsdi_cmd_ptr;
  #endif /* FEATURE_MMGSDI */

#ifdef FEATURE_UIM_HOT_SWAP
gsdi_uim_swap_T gsdi_command_ptr;
#endif
/* Technology present in UIM1 */
uim_dfs_present_type uim_dfs_1;

/* Variable to hold if the current select going down to the driver
 * is a intermediate select ( a df in the path )or the final select
 */
boolean                  uim_intermediate_sel = FALSE;

/* Variable to indicate to the driver whether or not it should issue
 * a get response if it finds status words indicating a get response
 * expected
 */
boolean                  uim_item_sel_rsp_cached    = FALSE;

/* Variable to hold the cache item to look for or store the
 * get response information
 */
uim_get_rsp_cache_type * select_rsp_cache_item_ptr  = NULL;

  #ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
/* Status of UIM2 initialization */
uim_status_type uim_status_2 = UIM_UNINITIALIZED_S;

/* Slot for polling during call and for toolkit purposes */
uim_slot_type uim_polling_slot = UIM_SLOT_NONE;

/* Preferred slots for CDMA and GSM */
uim_slot_type uim_pref_cdma_slot = UIM_SLOT_NONE;
uim_slot_type uim_pref_gsm_slot = UIM_SLOT_NONE;

/* Used to store the operational slots */
uim_slot_type uim_cdma_slot = UIM_DEFAULT_SLOT_FOR_CDMA;
uim_slot_type uim_gsm_slot = UIM_DEFAULT_SLOT_FOR_GSM;
uim_slot_type uim_telecom_slot = UIM_DEFAULT_SLOT_FOR_GSM;

/* Technology present in UIM2 */
uim_dfs_present_type uim_dfs_2;

/* Operational slot for commands */
uim_slot_type uim_current_slot;
  #endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

  #ifdef FEATURE_UIM_SUPPORT_CSIM
uim_last_sel_dir_type uim_csim_last_sel_dir[UIM_NUM_DRV_SLOTS];
  #endif /* FEATURE_UIM_SUPPORT_CSIM */

/* Variable to indicate that GCF testing is being done */
boolean uim_gcf_testing_flag = FALSE;

/* Variable to indicate that nv item NV_ENS_ENABLED_I is enabled */
boolean uim_nv_ens_enabled_flag = FALSE;

LOCAL uim_get_rsp_cache_type uim_ef_by_path_select_rsp_cache[UIM_MAX_EF_BY_PATH_CACHE_ITEMS];

LOCAL uim_get_rsp_cache_type uim_ef_under_mf_select_rsp_cache[UIM_MAX_EF_UNDER_MF_ITEMS];

/* Varible to hold configurable parameters stored in the NV */
nv_uim_config_params_type uim_config_params;

/* Variable to indicate if sw1 warning1 that needs to be treated as normal
** end has occured or not */
boolean uim_sw1_warnings1_normal_end_occured = FALSE;

static word const
uim_ef_under_mf_file_path[UIM_MAX_EF_UNDER_MF_ITEMS][UIM_MAX_PATH_ELEMENTS - 1]=
{
  /* EFs under MF */
  { 0x3F00, 0xFFFF, 0x2FE2},        /* ICCID */
  { 0x3F00, 0xFFFF, 0x2F05},        /* Extended Language Preferences */
  { 0x3F00, 0xFFFF, 0x2F00},        /* EF DIR */
  { 0x3F00, 0xFFFF, 0x2F06},        /* Access Rule Reference */
  { 0xFFFF, 0xFFFF, 0xFFFF}         /* Empty */
};

#ifdef FEATURE_UIM_RUIM
LOCAL uim_get_rsp_cache_type uim_ruim_file_select_rsp_cache[UIM_MAX_RUIM_ITEMS];
static word const
uim_ruim_file_path[UIM_MAX_RUIM_ITEMS][UIM_MAX_PATH_ELEMENTS - 1] =
{   /* EFs under CDMA DF */
  { 0x7F25, 0xFFFF, 0x6F21},        /* Call Count */
  { 0x7F25, 0xFFFF, 0x6F22},        /* IMSI_M */
  { 0x7F25, 0xFFFF, 0x6F23},        /* IMSI_T */
  { 0x7F25, 0xFFFF, 0x6F24},        /* TMSI */
  { 0x7F25, 0xFFFF, 0x6F25},        /* Analog Home SID */
  { 0x7F25, 0xFFFF, 0x6F26},        /* Analog Operational Parameters */
  { 0x7F25, 0xFFFF, 0x6F27},        /* Analog Location and Regn Indctr */
  { 0x7F25, 0xFFFF, 0x6F28},        /* CDMA Home SID and NID */
  { 0x7F25, 0xFFFF, 0x6F29},        /* CDMA Zone Based Regn Indctr */
  { 0x7F25, 0xFFFF, 0x6F2A},        /* CDMA System/Network Regn Indctr */

  { 0x7F25, 0xFFFF, 0x6F2B},        /* CDMA Distance Based Regn Indctr */
  { 0x7F25, 0xFFFF, 0x6F2C},        /* Access Overload Class */
  { 0x7F25, 0xFFFF, 0x6F2D},        /* Call Termination Mode Preferences */
  { 0x7F25, 0xFFFF, 0x6F2E},        /* Suggested Slot Cycle Index */
  { 0x7F25, 0xFFFF, 0x6F2F},        /* Analog Channel Preferences */
  { 0x7F25, 0xFFFF, 0x6F30},        /* Preferred Roaming List */
  { 0x7F25, 0xFFFF, 0x6F31},        /* Removable UIM ID */
  { 0x7F25, 0xFFFF, 0x6F32},        /* CDMA Service Table */
  { 0x7F25, 0xFFFF, 0x6F33},        /* Service Programming Code */
  { 0x7F25, 0xFFFF, 0x6F34},        /* OTAPA/SPC-Enable */

  { 0x7F25, 0xFFFF, 0x6F35},        /* NAM-Lock */
  { 0x7F25, 0xFFFF, 0x6F36},        /* OTASP-OTAPA Parameters */
  { 0x7F25, 0xFFFF, 0x6F37},        /* Service Preferences */
  { 0x7F25, 0xFFFF, 0x6F38},        /* ESN_ME */
  { 0x7F25, 0xFFFF, 0x6F39},        /* R-UIM Phase */
  { 0x7F25, 0xFFFF, 0x6F3A},        /* Preferred Languages */
  { 0x7F25, 0xFFFF, 0x6F3B},        /* Reserved */
  { 0x7F25, 0xFFFF, 0x6F3C},        /* Short Messages */
  { 0x7F25, 0xFFFF, 0x6F3D},        /* Short Message Service Parameters */
  { 0x7F25, 0xFFFF, 0x6F3E},        /* SMS Status */

  { 0x7F25, 0xFFFF, 0x6F3F},        /* Supplementary Services Feature Code */
  { 0x7F25, 0xFFFF, 0x6F40},        /* Reserved */
  { 0x7F25, 0xFFFF, 0x6F41},        /* CDMA Home Service Provider Name */
  { 0x7F25, 0xFFFF, 0x6F42},        /* R-UIM ID usage indicator */
  { 0x7F25, 0xFFFF, 0x6F43},        /* Administrative Data */
  { 0x7F25, 0xFFFF, 0x6F44},        /* MSISDN from CDMA directory */
  { 0x7F25, 0xFFFF, 0x6F45},        /* Maximum PRL */
  { 0x7F25, 0xFFFF, 0x6F46},        /* SPC Status */
  { 0x7F25, 0xFFFF, 0x6F47},        /* Emergency Call Codes */
  { 0x7F25, 0xFFFF, 0x6F48},        /* 3GPD Operational Capabilities */

  { 0x7F25, 0xFFFF, 0x6F49},        /* 3GPD Operational Mode */
  { 0x7F25, 0xFFFF, 0x6F4A},        /* 3GPD Simple IP Capability Parameters */
  { 0x7F25, 0xFFFF, 0x6F4B},        /* 3GPD Mobile IP Capability Parameters */
  { 0x7F25, 0xFFFF, 0x6F4C},        /* 3GPD Simple IP User Profile Parameters */
  { 0x7F25, 0xFFFF, 0x6F4D},        /* 3GPD Mobile IP User Profile Parameters */
  { 0x7F25, 0xFFFF, 0x6F4E},        /* 3GPD Simple IP Status Parameters */
  { 0x7F25, 0xFFFF, 0x6F4F},        /* 3GPD Mobile IP Status Parameters */
  { 0x7F25, 0xFFFF, 0x6F50},        /* 3GPD Simple IP PAP SS Parameters */
  { 0x7F25, 0xFFFF, 0x6F51},        /* Reserved */
  { 0x7F25, 0xFFFF, 0x6F52},        /* Reserved */

  { 0x7F25, 0xFFFF, 0x6F53},        /* Preferred User Zone List */
  { 0x7F25, 0xFFFF, 0x6F54},        /* Maximum PUZL */
  { 0x7F25, 0xFFFF, 0x6F55},        /* ME-specific Config Request Parameters */
  { 0x7F25, 0xFFFF, 0x6F56},        /* HRPD Access Auth Capability Param */
  { 0x7F25, 0xFFFF, 0x6F57},        /* HRPD Access Auth User Profile Param */
  { 0x7F25, 0xFFFF, 0x6F58},        /* CUR_SSPR_P_REV */
  { 0x7F25, 0xFFFF, 0x6F59},        /* Access Terminal Class */
  { 0x7F25, 0xFFFF, 0x6F5A},        /* Extended Preferred Roaming List */
  { 0x7F25, 0xFFFF, 0x6F5B},        /* Broadcast SMS Configuration */
  { 0x7F25, 0xFFFF, 0x6F5C},        /* Broadcast SMS Preferences */

  { 0x7F25, 0xFFFF, 0x6F5D},        /* Broadcast SMS Table */
  { 0x7F25, 0xFFFF, 0x6F5E},        /* Broadcast SMS Paramaters */
  { 0x7F25, 0xFFFF, 0x6F65},        /* MMS Notification */
  { 0x7F25, 0xFFFF, 0x6F66},        /* MMS Extension 8 */
  { 0x7F25, 0xFFFF, 0x6F67},        /* MMS Issuer Connectivity Parameters */
  { 0x7F25, 0xFFFF, 0x6F68},        /* MMS User Preferences */
  { 0x7F25, 0xFFFF, 0x6F76},        /* SMS Capabilities */
  { 0x7F25, 0xFFFF, 0x6F77},        /* IPv6 Capabilities */
  { 0x7F25, 0xFFFF, 0x6F78},        /* Mobile IP Flags */
  { 0x7F25, 0xFFFF, 0x6F79},        /* TCP Configurations */

  { 0x7F25, 0xFFFF, 0x6F7A},        /* Data Generic Configurations */
  { 0x7F25, 0xFFFF, 0x6F7B},        /* Browser Connectivity Parameters */
  { 0x7F25, 0xFFFF, 0x6F7C},        /* Browser Bookmarks */
  { 0x7F25, 0xFFFF, 0x6F7D},        /* Simple IP User Profile Parameters Extension */
  { 0x7F25, 0xFFFF, 0x6F7E},        /* MMS Configuration */
  { 0x7F25, 0xFFFF, 0x6F7F},        /* Java Download URL */
  { 0x7F25, 0xFFFF, 0x6F80},        /* Mobile IP User Profile Parameters Extension */
  { 0x7F25, 0xFFFF, 0x6F81},        /* BREW Download */
  { 0x7F25, 0xFFFF, 0x6F82},        /* BREW Teleservice ID */
  { 0x7F25, 0xFFFF, 0x6F83},        /* BREW Subscriber ID */

  { 0x7F25, 0xFFFF, 0x6F84},        /* LBS XTRA Configuration */
  { 0x7F25, 0xFFFF, 0x6F85},        /* LBS XTRA Server URLs */
  { 0x7F25, 0xFFFF, 0x6F86},        /* LBS V2 Configuration */
  { 0x7F25, 0xFFFF, 0x6F87},        /* LBS V2 PDE Address */
  { 0x7F25, 0xFFFF, 0x6F88},        /* LBS V2 MPC Address */
  { 0x7F25, 0xFFFF, 0x6F89},        /* BREW Application Execution Policy */
  { 0x7F25, 0xFFFF, 0x6F90},        /* Model */
  { 0x7F25, 0xFFFF, 0x6F91},        /* Root Certificate */
  { 0x7F25, 0xFFFF, 0x6F92},        /* Application Labels */
  { 0x7F25, 0xFFFF, 0x6FE3},        /* KDDI Agent String */

  { 0x7F25, 0xFFFF, 0x6FE4},        /* KDDI GID2 */
  { 0x7F25, 0xFFFF, 0x6FE5},        /* KDDI GID1 */
  { 0x7F25, 0xFFFF, 0x6FE6},        /* KDDI GID References */
  { 0x7F25, 0xFFFF, 0x6FE7},        /* KDDI IMSI Status */
  { 0x7F25, 0xFFFF, 0x6FE8},        /* KDDI ME Downloadable Data */
  { 0x7F25, 0xFFFF, 0x6FE9},        /* KDDI ME Setting Date */
  { 0x7F25, 0xFFFF, 0x6FEA},        /* KDDI User Data */
  { 0x7F25, 0xFFFF, 0x6FEB},        /* KDDI Reserved */
  { 0x7F25, 0xFFFF, 0x6FEC},        /* KDDI Reserved */
  { 0x7F25, 0xFFFF, 0x6FED},        /* KDDI UIM Service Table */

  { 0x7F25, 0xFFFF, 0x6FEE},        /* KDDI UIM Features */
  { 0x7F25, 0xFFFF, 0x6FEF},        /* KDDI UIM Version */
  { 0x7F25, 0xFFFF, 0x6F57},        /* HRPD User Profile Parameters */
  { 0x7F25, 0xFFFF, 0x6F74},        /* SF_EUIMID */

  { 0xFFFF, 0xFFFF, 0xFFFF},        /* Empty */
  { 0xFFFF, 0xFFFF, 0xFFFF}         /* Empty */
};
  #endif /* FEATURE_UIM_RUIM */

  #ifdef FEATURE_UIM_GSM
#error code not present
  #endif /* FEATURE_UIM_GSM */

LOCAL uim_get_rsp_cache_type uim_telecom_file_select_rsp_cache[UIM_MAX_TELECOM_ITEMS];

static word const
uim_telecom_file_path[UIM_MAX_TELECOM_ITEMS][UIM_MAX_PATH_ELEMENTS - 1] =
{
  /* EFs under Telecom DF */
  { 0x7F10, 0xFFFF, 0x6F3A},        /* Abbreviated Dialing Numbers */
  { 0x7F10, 0xFFFF, 0x6F3B},        /* Fixed dialling numbers */
  { 0x7F10, 0xFFFF, 0x6F3C},        /* Short messages */
  { 0x7F10, 0xFFFF, 0x6F3D},        /* Capability Configuration Parameters */
  { 0x7F10, 0xFFFF, 0x6F4F},        /* Extended CCP */
  { 0x7F10, 0xFFFF, 0x6F40},        /* MSISDN */
  { 0x7F10, 0xFFFF, 0x6F42},        /* SMS parameters */
  { 0x7F10, 0xFFFF, 0x6F43},        /* SMS Status */
  { 0x7F10, 0xFFFF, 0x6F44},        /* Last number dialled */
  { 0x7F10, 0xFFFF, 0x6F49},        /* Service Dialling numbers */

  { 0x7F10, 0xFFFF, 0x6F4A},        /* Extension 1 */
  { 0x7F10, 0xFFFF, 0x6F4B},        /* Extension 2 */
  { 0x7F10, 0xFFFF, 0x6F4C},        /* Extension 3 */
  { 0x7F10, 0xFFFF, 0x6F4D},        /* Barred Dialing Numbers */
  { 0x7F10, 0xFFFF, 0x6F4E},        /* Extension 4 */
  { 0x7F10, 0xFFFF, 0x6F47},        /* SMS reports */
  { 0x7F10, 0xFFFF, 0x6F58},        /* Comparison Method Information */
  { 0x7F10, 0xFFFF, 0x6F54},        /* Setup Menu elements */
  { 0x7F10, 0xFFFF, 0x6F06},        /* Access Rule reference */
  { 0x7F10, 0x5F50, 0x4F20},        /* Image */

  { 0x7F10, 0x5F3A, 0x4F30},        /* Phone book reference file */
  { 0x7F10, 0x5F3A, 0x4F22},        /* Phone book synchronization center */
  { 0x7F10, 0x5F3A, 0x4F23},        /* Change counter */
  { 0x7F10, 0x5F3A, 0x4F24},        /* Previous Unique Identifier */
  { 0xFFFF, 0xFFFF, 0xFFFF},        /* Empty */
  { 0xFFFF, 0xFFFF, 0xFFFF},        /* Empty */
  { 0xFFFF, 0xFFFF, 0xFFFF},        /* Empty */
  { 0xFFFF, 0xFFFF, 0xFFFF}         /* Empty */
};

  #ifdef FEATURE_UIM_USIM
#error code not present
#endif /* FEATURE_UIM_USIM */

#ifdef FEATURE_UIM_ISIM
LOCAL uim_get_rsp_cache_type uim_isim_file_select_rsp_cache[UIM_MAX_ISIM_ITEMS];

static word const
uim_isim_file_path[UIM_MAX_ISIM_ITEMS][UIM_MAX_PATH_ELEMENTS - 1] =
{
  { 0xFFFF, 0xFFFF, 0x6F02},     /* IMS private user identity */
  { 0xFFFF, 0xFFFF, 0x6F03},     /* Home Network Domain Name */
  { 0xFFFF, 0xFFFF, 0x6F04},     /* IMS public user identity */
  { 0xFFFF, 0xFFFF, 0x6F06},     /* Access Rule Reference */
  { 0xFFFF, 0xFFFF, 0x6F08},     /* Ciphering and Integrity Keys for IMS */
  { 0xFFFF, 0xFFFF, 0x6FAD}      /* Administrative Data */
};
#endif /*FEATURE_UIM_ISIM */

LOCAL uim_get_rsp_cache_type uim_cingular_file_select_rsp_cache[UIM_MAX_CINGULAR_ITEMS];

static word const
uim_cingular_file_path[UIM_MAX_CINGULAR_ITEMS][UIM_MAX_PATH_ELEMENTS - 1] =
{
  /* EFs under CINGULAR DF */
  { 0x7F66, 0x5F30, 0x4F34},       /* Acting HPLMN*/
  { 0x7F66, 0xFFFF, 0x6FD2}        /* Support Table*/
};

static word const
uim_hzi_file_path[UIM_MAX_HZI_ITEMS][UIM_MAX_PATH_ELEMENTS - 1] =
{
  /* EFs under HZI DF */
  { 0x7F43, 0xFFFF, 0x6F60},       /* EF-HZ*/
  { 0x7F43, 0xFFFF, 0x6F61},       /* EF-CACHE1*/
  { 0x7F43, 0xFFFF, 0x6F62},       /* EF-CACHE2*/
  { 0x7F43, 0xFFFF, 0x6F63},       /* EF-CACHE3*/
  { 0x7F43, 0xFFFF, 0x6F64},       /* EF-CACHE4*/
  { 0x7F43, 0xFFFF, 0x6F81},       /* EF-SUBSCRIBED-LAC/CI-1*/
  { 0x7F43, 0xFFFF, 0x6F82},       /* EF-SUBSCRIBED-LAC/CI-2*/
  { 0x7F43, 0xFFFF, 0x6F83},       /* EF-SUBSCRIBED-LAC/CI-3*/
  { 0x7F43, 0xFFFF, 0x6F84},       /* EF-SUBSCRIBED-LAC/CI-4*/
  { 0x7F43, 0xFFFF, 0x6F80},       /* EF-Tags*/

  { 0x7F43, 0xFFFF, 0x6F87}        /* EF-UHZIUE-Settings*/
};

#ifdef FEATURE_UIM_WIM
uim_get_rsp_cache_type uim_wim_file_select_rsp_cache[UIM_MAX_WIM_ITEMS];

static word const
uim_wim_file_path[UIM_MAX_WIM_ITEMS][UIM_MAX_PATH_ELEMENTS - 1] =
{
  /* EFs under WIM ADF */
  { 0xFFFF, 0xFFFF, 0x5031},        /* WIM ODF */
  { 0xFFFF, 0xFFFF, 0x5032},        /* Token Info */
  { 0xFFFF, 0xFFFF, 0x5033}         /* Unused Space */
};
#endif /* FEATURE_UIM_WIM */

#ifdef FEATURE_UIM_MFLO
#error code not present
#endif /*FEATURE_UIM_MFLO */

static word const
uim_file_path[UIM_MAX_EF_BY_PATH_ITEMS][UIM_MAX_PATH_ELEMENTS] =
{
  { 0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF} /* Path is provided in the command */
};

#if defined(FEATURE_USE_TIME_VU)
static timer_group_type uim_always_on_timer_group;
#endif /* FEATURE_USE_TIME_VU */

/*
 * Boolean flag to indicate that we wish to reselect MF
 */
boolean uim_mf_reselect = FALSE;

/*
 * Number of attempts to select MF
 */
uint32   uim_mf_reselect_count = 1;
/*-------------------------------------------------------------------------*/
/*                                                                         */
/*                               Macros                                    */
/*                                                                         */
/*-------------------------------------------------------------------------*/

  /* Macro to identify if the command is allowed while UIM is in passive mode */
  #define UIM_IS_CMD_VALID_IN_MODE(cmdptr)                                           \
  ((uim_mode == UIM_PASSIVE_MODE)?(((cmdptr)->hdr.command == UIM_RESET_F) ||                     \
                       ((cmdptr)->hdr.command == UIM_RESET_WITH_TP_F) ||             \
                       ((cmdptr)->hdr.command == UIM_RESET_SWITCH_UIM_PASSIVE_F) ||  \
                       ((cmdptr)->hdr.command == UIM_POWER_UP_UIM_PASSIVE_F) ||      \
                       ((cmdptr)->hdr.command == UIM_POWER_DOWN_F) ||                \
                       ((cmdptr)->hdr.command == UIM_STREAM_ISO7816_APDU_F)) :       \
                      (TRUE))

/* Macro to get the table index for the item specified in the command */
  #define UIM_GET_TABLE_INDEX(item) (((uint2)item >> 8) & 0x0F)

/* Macro to get the item index for the item specified in the command */
  #define UIM_GET_ITEM_INDEX(item) ((item) & 0xFF)

/* Macro to get the command mode type for a command */
  #define UIM_GET_CMD_MODE(command) ((command >> 8) & 0x0F)

/* Macro to close timeout info log      */
/* Return allocated data memory to heap */
/* close file handle                    */
  #define UIM_CLOSE_LOG_TIMEOUT_INFO()                                         \
  MSG_ERROR("Failed to write on UimReset File : 0x%x",fhandle,0,0);            \
  mem_free(&tmc_heap, data_to_write);                                          \
  (void)efs_close(fhandle);

/*===========================================================================
              F U N C T I O N   P R O T O T Y P E S
===========================================================================*/
boolean uim_efs_write( int           fhandle,
                       char          *databuffer_p,
                       unsigned int  datasize);

LOCAL boolean uim_log_timeout_info(uim_cmd_type *external_cmd_ptr,
                             uim_cmd_type *static_cmd_ptr,
                             rex_sigs_type i_mask,
                             dword         uart_status);

LOCAL void uim_send_err_rpt(uim_cmd_type *cmd_ptr);

LOCAL boolean uim_set_select_path
(
  const word *new_path
#ifdef FEATURE_UIM_UICC
  , uim_aid_type const *new_adf
#endif /* FEATURE_UIM_UICC */
);

LOCAL boolean uim_directory_current(uim_cmd_type const *cmd_ptr);

LOCAL uim_cmd_status_type uim_command_response
(
  uim_rsp_buf_type *rsp_ptr,
  uim_cmd_type     *cmd
);

LOCAL void uim_process_command
(
  uim_cmd_type *cmd_ptr,
  /* Pointer to received command */
  rex_sigs_type *mask
  /* rex signals type mask */
);

#if (defined(FEATURE_UIM_SUPPORT_CSIM) && (!defined(FEATURE_UIM_SUPPORT_DUAL_SLOTS)))
LOCAL void uim_reset_csim_dir ( void );
#endif /*(FEATURE_UIM_SUPPORT_CSIM) && ! (FEATURE_UIM_SUPPORT_DUAL_SLOTS) */

LOCAL void uim_clear_get_response_table
(
  uim_get_rsp_cache_type *table,
  int32 num_rows
);

LOCAL void uim_reset_uim
(
  uim_cmd_type *cmd_ptr,
  rex_sigs_type *mask,
  boolean me_powerup
);

LOCAL void uim_poll
(
  uim_cmd_type *cmd_ptr,
  rex_sigs_type *mask
);

LOCAL void uim_gsdi_event_cb_func(gsdi_sim_events_T event);

#ifdef FEATURE_UIM_TOOLKIT
LOCAL void uim_fetch
(
  uim_cmd_type *cmd_ptr,
  rex_sigs_type *mask
);
#endif /* FEATURE_UIM_TOOLKIT*/

LOCAL void uim_init( void );

LOCAL dword uim_get_uart_status( void );

LOCAL void uim_process_cmd_err
(
  uim_cmd_type  *cmd_ptr, /* pointer to command */
  rex_sigs_type *i_mask   /* Mask of signals interested in */
);

LOCAL boolean uim_can_ignore_timeout(rex_sigs_type sigs);

LOCAL rex_sigs_type uim_wait_filter_non_recovery_cmds
(
  rex_sigs_type sigs_to_wait_for
);

LOCAL void uim_clear_poll_timer( void );

#ifdef FEATURE_UIM_LOG_TO_EFS
LOCAL boolean uim_efslog_purge_check( void );

LOCAL boolean uim_efslog_init( void );

LOCAL void uim_efslog_save_buffer_to_efs( void );

LOCAL void uim_efslog_clean_up( void );
#endif /* FEATURE_UIM_LOG_TO_EFS */


/*===========================================================================

FUNCTION UIM_SET_POLL_TIMER

DESCRIPTION
  This function should be used to set the time to trigger a poll.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  Sets the uim_poll_time_delay to the value passed in.
  Sets a second timer based on feature definition to ensure
  wake-up.

===========================================================================*/
void uim_set_poll_timer(rex_timer_cnt_type  time)
{
  /* Stop the poll timer if the timer value is UIM_MAX_POLL_DURATION */
  if (time == UIM_MAX_POLL_DURATION)
  {    
     uim_clear_poll_timer();
     MSG_MED( "Stop Proactive Polling Timer", 0, 0, 0);
  }
  else 
  {
    /* Set the regular timer */
    (void) rex_set_timer(&uim_no_wake_up_poll_timer, time);
#if defined(FEATURE_USE_TIME_VU)
    /* If this feature is defined, the timers which are not always on
     * would only be triggered when the ME wakes-up for any reason.
     * The always on timer would wake-up the ME when it goes off.
     * So we are giving UIM_WAKE_UP_POLL_OFFSET duration for a DRX
     * cycle so that the POLL could co-inside.  In the worst case,
     * we make sure that we wake-up within the UIM_WAKE_UP_POLL_OFFSET
     * duration of the given time (which is already reduced by 2 seconds
     * to compensate for timer inaccuracies)
     */
    (void) rex_set_timer(&uim_wake_up_poll_timer, time +
                         UIM_WAKE_UP_POLL_OFFSET);
#endif
  }
  uim_poll_time_delay = time;
}


/*===========================================================================

FUNCTION UIM_CLEAR_POLL_TIMER

DESCRIPTION
  This function clears any pending poll timer(s).

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  Clears all the timers relating to a poll.
===========================================================================*/
LOCAL void uim_clear_poll_timer( void )
{
  /* Clear the regular timer. */
  (void) rex_clr_timer(&uim_no_wake_up_poll_timer);
#if defined(FEATURE_USE_TIME_VU)
  /* If this feature is defined, clear the second timer */
  (void) rex_clr_timer(&uim_wake_up_poll_timer);
#endif
}


#ifdef FEATURE_UIM_LOG_TO_EFS
/*===========================================================================

FUNCTION UIM_EFSLOG_PURGE_CHECK

DESCRIPTION
  This file checks if a purge is necessary for any EFS Buffers.

DEPENDENCIES

RETURN VALUE
  Boolean returns true if any buffers must be purged

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean uim_efslog_purge_check( void )
{
#ifdef FEATURE_UIM_LOG_APDU_TO_EFS
  if(uim_efslog_apdu_buffer.buffer_size >= (UIM_EFSLOG_MAX_BUFFER_SIZE/2))
  {
    return TRUE;
  }
#endif /* FEATURE_UIM_LOG_APDU_TO_EFS */

  return FALSE;
}

#ifdef FEATURE_UIM_LOG_APDU_TO_EFS
/*===========================================================================

FUNCTION UIM_EFSLOG_APDU_TO_BUFFER

DESCRIPTION
  This function takes a character to be stored to the buffer and an attribute,
  only if EFS Logging is currently in progress.  It converts the character to
  ascii, and checks the attribute to see if it differs from the previous
  character's attribute.  If it does not, then it simply places the ASCII
  value of the character in the next space in the buffer (One character will
  become two ASCII bytes).  If the attribute is different, it will place a new
  line character '\n' followed by the attribute tag, a colon, and then the
  character (in ASCII format).

DEPENDENCIES
  uim_efslog_buffer_init must have already been called

RETURN VALUE
  void

SIDE EFFECTS
  Alters the uim_efslog_apdu_buffer

===========================================================================*/
void uim_efslog_apdu_to_buffer(byte attribute, byte character)
{
  /* for converting from HEX To ASCII */
  char ascii[17] = "0123456789ABCDEF";

  /* If the buffer is half full, set the EFSLOG signal */
  if(uim_efslog_purge_check())
  {
    (void) rex_set_sigs( &uim_tcb, UIM_EFSLOG_PURGE_SIG );
  }

  /* If EFS Logging not currently in progress, return without doing anyting */
  if(!uim_efslog_logging_in_progress || uim_efslog_apdu_buffer.data == NULL)
  {
    return;
  }

  if (uim_efslog_apdu_buffer.buffer_size >= (UIM_EFSLOG_MAX_BUFFER_SIZE - 8))
  {
    /* There must be at least 8 empty spaces left in the buffer
    to log another character (in case a new attribute tag is encountered */
    return;
  }

  /* If the attibute has changed, start a new line */
  if(attribute != uim_efslog_previous_char_attribute)
  {
    /* Insert a carriage return and line feed */
    uim_efslog_apdu_buffer.data[uim_efslog_apdu_buffer.buffer_size++] =
                                                      UIM_EFSLOG_ASCII_CR;
    uim_efslog_apdu_buffer.data[uim_efslog_apdu_buffer.buffer_size++] =
                                                      UIM_EFSLOG_ASCII_LF;
    /* Store Upper Nibble of Attribute tag in ASCII */
    uim_efslog_apdu_buffer.data[uim_efslog_apdu_buffer.buffer_size++] =
                                                      ascii[attribute >> 4];
    /* Store Lower Nibble of Attribute tag in ASCII */
    uim_efslog_apdu_buffer.data[uim_efslog_apdu_buffer.buffer_size++] =
                                                      ascii[attribute & 0x0F];
    /* Insert a Colon and a space */
    uim_efslog_apdu_buffer.data[uim_efslog_apdu_buffer.buffer_size++] = ':';
    uim_efslog_apdu_buffer.data[uim_efslog_apdu_buffer.buffer_size++] = ' ';
  }

  /* Store Upper Nibble of byte in ASCII */
  uim_efslog_apdu_buffer.data[uim_efslog_apdu_buffer.buffer_size++] =
                                                      ascii[character >> 4];

  /* Store Lower Nibble of byte in ASCII */
  uim_efslog_apdu_buffer.data[uim_efslog_apdu_buffer.buffer_size++] =
                                                      ascii[character & 0x0F];

  /* Update Previous Char Attribute tag */
  uim_efslog_previous_char_attribute = attribute;

} /* uim_efslog_apdu_to_buffer */

#endif /* FEATURE_UIM_LOG_APDU_TO_EFS */
/*===========================================================================

FUNCTION UIM_EFSLOG_EFS_INIT

DESCRIPTION
  This file establishes the file in EFS to which EFSLOG will write.  The
  local EFSLOG buffer cannot be purged until this function has completed
  successfully.  This function will also establish the local EFS buffer

DEPENDENCIES

RETURN VALUE
  void

SIDE EFFECTS
  Establishes a file to write to in EFS

===========================================================================*/
LOCAL boolean uim_efslog_init( void )
{
#ifdef FEATURE_UIM_LOG_APDU_TO_EFS

  /* Before creating the EFS File for APDU logging, erase the old one if
  there is one */
#if defined(FEATURE_EXTERNAL_APPS_MOUNT) && defined(FS_FEATURE_RMTEFS_CLIENT)
  if(efs_unlink("/apps/UimEfsAPDULog.Txt") < 0)
#else
  if(efs_unlink("UimEfsAPDULog.Txt") < 0)
#endif /* FEATURE_EXTERNAL_APPS_MOUNT && FS_FEATURE_RMTEFS_CLIENT */
  {
    MSG_ERROR("Error removing file for EFSLog, or no old file to remove",0,0,0);
  }

#if defined(FEATURE_EXTERNAL_APPS_MOUNT) && defined(FS_FEATURE_RMTEFS_CLIENT)
  uim_efslog_apdu_file_handle = efs_creat( "/apps/UimEfsAPDULog.Txt", S_IREAD | S_IWRITE);
#else
  uim_efslog_apdu_file_handle = efs_creat( "UimEfsAPDULog.Txt", S_IREAD | S_IWRITE);
#endif /* FEATURE_EXTERNAL_APPS_MOUNT && FS_FEATURE_RMTEFS_CLIENT */

  if (uim_efslog_apdu_file_handle < 0)
  {
    return FALSE;
  }

  /* Allocate space for main buffer */
  uim_efslog_apdu_buffer.data = uim_malloc(UIM_EFSLOG_MAX_BUFFER_SIZE);

  /* If allocation failed, return without turning on logging */
  if(uim_efslog_apdu_buffer.data == NULL)
  {
    /* Close the EFS file  */
    (void)efs_close(uim_efslog_apdu_file_handle);
    return FALSE;
  }

  /* Allocate space for secondary outgoing data buffer */
  uim_efslog_apdu_outgoing_data_buffer.data =
                                    uim_malloc(UIM_EFSLOG_MAX_BUFFER_SIZE);

  /* If allocation failed, return without turning on logging */
  if(uim_efslog_apdu_outgoing_data_buffer.data == NULL)
  {
    /* Close the EFS file and free up the local buffer memory */
    (void)efs_close(uim_efslog_apdu_file_handle);
    uim_free(uim_efslog_apdu_buffer.data);
    return FALSE;
  }
  uim_efslog_apdu_buffer.buffer_size = 0;
  uim_efslog_apdu_outgoing_data_buffer.buffer_size = 0;
#endif /* FEATURE_UIM_LOG_APDU_TO_EFS */
  uim_efslog_logging_in_progress = TRUE;
  return TRUE;
} /* uim_efslog_init */

/*===========================================================================

FUNCTION UIM_EFSLOG_SAVE_BUFFER_TO_EFS

DESCRIPTION
  This function gets called when a the local logging buffer reaches a certain
  threashold, and we want to purge this data to the EFS file.  This makes a
  blocking call to efs_write.  First, we copy into the secondary buffer and then
  write the contents of the secondary buffer to EFS.

DEPENDENCIES
  uim_efslog_buffer_init must have already been called, EFS init must be complete
  uim_efslog_efs_file_init must have already been called

RETURN VALUE
  void

SIDE EFFECTS
  Writes data in local EFS Log buffer to the EFS file, and clears the local
  buffer

===========================================================================*/
LOCAL void uim_efslog_save_buffer_to_efs( void )
{
#ifdef FEATURE_UIM_LOG_APDU_TO_EFS
  int return_status = 0;

  /* Before we write to the secondary buffer, lock the interrupts so we
  don't get any interference (unless we've already stopped logging */
  INTLOCK();
  if(uim_efslog_apdu_buffer.buffer_size > 0)
  {
    uim_efslog_apdu_outgoing_data_buffer.buffer_size =
                      uim_efslog_apdu_buffer.buffer_size;
    memcpy(uim_efslog_apdu_outgoing_data_buffer.data,
           uim_efslog_apdu_buffer.data,
           int32touint32(uim_efslog_apdu_outgoing_data_buffer.buffer_size));
    /* Clear the buffer and set size to 0 */
    memset(uim_efslog_apdu_buffer.data,0,UIM_EFSLOG_MAX_BUFFER_SIZE);
    uim_efslog_apdu_buffer.buffer_size = 0;
  }
  (void) rex_clr_sigs( &uim_tcb, UIM_EFSLOG_PURGE_SIG );
  INTFREE();

  /* Write to file */
  return_status = efs_write(uim_efslog_apdu_file_handle,
     uim_efslog_apdu_outgoing_data_buffer.data,
     int32touint32(uim_efslog_apdu_outgoing_data_buffer.buffer_size));

  /* If the write failed, we want to clean-up and stop logging.  However,
     if the write failed because the buffer size was zero, we don't count
     this as a failure.  Also, if logging is already off, it means that this
     is the final write called from uim_efslog_clean_up.  Clean-up will
     free the buffers, so don't do it here */

  if((return_status <= 0) &&
     (!(uim_efslog_apdu_outgoing_data_buffer.buffer_size == 0)) &&
     uim_efslog_logging_in_progress)
  {
    /* There was an error saving to EFS, or it's full.  Clean up without
    trying to save again */
    uim_efslog_logging_in_progress = FALSE;
    (void) rex_clr_sigs( &uim_tcb, UIM_EFSLOG_PURGE_SIG );
    (void)efs_close(uim_efslog_apdu_file_handle);
    uim_free(uim_efslog_apdu_buffer.data);
    uim_free(uim_efslog_apdu_outgoing_data_buffer.data);
    return;
  }
#endif /* FEATURE_UIM_LOG_APDU_TO_EFS */

  /* Increment the EFS Write counter in case we have to stop logging
     before we ever get a GSDI Event */
  uim_efslog_save_count++;

} /* uim_efslog_save_buffer_to_efs */


/*===========================================================================
FUNCTION  UIM_EFSLOG_CLEAN_UP

DESCRIPTION
  Cleans up EFSLOG when complete

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS
  Cleans EFSLOG Buffer and closes EFS File
===========================================================================*/
LOCAL void uim_efslog_clean_up( void )
{
  uim_efslog_logging_in_progress = FALSE;
  /* In case there's any data left in the buffer, send it to EFS now */
  uim_efslog_save_buffer_to_efs();

#ifdef FEATURE_UIM_LOG_APDU_TO_EFS
  /* Close the EFS file and free up the local buffer memory */
  (void) rex_clr_sigs( &uim_tcb, UIM_EFSLOG_PURGE_SIG );
  (void)efs_close(uim_efslog_apdu_file_handle);
  uim_free(uim_efslog_apdu_buffer.data);
  uim_free(uim_efslog_apdu_outgoing_data_buffer.data);
#endif /* FEATURE_UIM_LOG_APDU_TO_EFS */
  return;
}  /* uim_efslog_clean_up */

#endif /* FEATURE_UIM_LOG_TO_EFS */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_EFS_WRITE

DESCRIPTION
  This procedure write given databuffer to given file handler.

DEPENDENCIES
  File handler should be valid.

RETURN VALUE
 Return TRUE after written data successfully.
 Return FALSE if any error occur during write operation.

SIDE EFFECTS
  None.

===========================================================================*/
boolean uim_efs_write( int           fhandle,
                       char          *databuffer_p,
                       unsigned int  datasize)
{
  unsigned int bytewritten = 0;

  /* Check file handler, invalid file handler is denote by -1*/
  if (0 > fhandle)
  {
    MSG_ERROR("Invalid File Handle 0x%x",fhandle,0,0);
    return FALSE;
  }
  /* Check data buffer and data buffer length */
  if(NULL == databuffer_p)
  {
    MSG_ERROR("No data to write",0,0,0);
    return TRUE;
  }
  /* Writing entire data */
  while (datasize > 0)
  {
    bytewritten = efs_write(fhandle, (void *)(databuffer_p), datasize);
    /* efs_write return -1 on error case */
    if( bytewritten > datasize)
    {
     MSG_ERROR("Write to efs failed",0,0,0);
     return FALSE;
    }
    datasize -= bytewritten;
    databuffer_p += bytewritten;
  } /* end of while */

  return TRUE;
}/* uim_efs_write */


/*===========================================================================

FUNCTION UIM_LOG_TIMEOUT_INFO

DESCRIPTION
  This procedure would be invoked when we see a timeout signal after the
  PPS procedure has been completed.

DEPENDENCIES
  This function uses global variables defined for the UIM server to determine
  the current status of the uim server, driver.

RETURN VALUE
  cmd_ptr value.

SIDE EFFECTS
  Deletes and writes uimreset file in the EFS with the analysis of this timeout.

===========================================================================*/

LOCAL boolean uim_log_timeout_info(uim_cmd_type *external_cmd_ptr,
                             uim_cmd_type *static_cmd_ptr,
                             rex_sigs_type i_mask,
                             dword         uart_status)
{
#ifndef PLATFORM_LTK
/* -1 is used for invalid file handle */
  int           fhandle         = -1;
  uim_cmd_type  *cmd_ptr_in_use = NULL;
  int           num_bytes       = 0;
  unsigned int  unum_bytes      = 0;
  unsigned int  i               = 0;
  char          *data_to_write  = NULL;
  char          temp_buff[10];

  /* Initialize temp_buff */
  memset(temp_buff,'\0',sizeof(temp_buff));

  if (uim_static_buffer_used)
  {
    cmd_ptr_in_use = static_cmd_ptr;
  }
  else
  {
    cmd_ptr_in_use = external_cmd_ptr;
  }
  /* Let us allocate enough memory to contain the strings
   * that we are going to create for writing to the Log
   */
  data_to_write = (char *) mem_malloc(&tmc_heap, data_to_write_size);

  if ( data_to_write == NULL)
  {
    /* There is nothing much we can do if we do not have
     * the buffer space to write to */
    MSG_ERROR("Could not allocate 1500 bytes for logging buffer",0,0,0);
    return FALSE;
  }

 /* --------------------------------------------------------------------------
    Creating file in EFS
    ------------------------------------------------------------------------*/
  fhandle = efs_open("UimReset.Txt", O_CREAT | O_TRUNC | O_WRONLY);
  /* invalid file handle is denote by -1 */
  if (0 > fhandle)
  {
    MSG_ERROR("Failed to open UimReset File : 0x%x",fhandle,0,0);
    /* Let us return the memory allocated */
    mem_free(&tmc_heap, data_to_write);
    return FALSE;
  }

  MSG_HIGH("Starting to log the timeout Information to UimReset.Txt",0,0,0);

  /* Indicate that this timeout is after a PPS procedure */
  if (uim_atr_pps_done)
  {
    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s\r\n","Timeout occured after PPS Procedure");
    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }
  }
  else
  {
    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s\r\n","Timeout occured before PPS Procedure");
    if ((num_bytes >= 0) &&  (TRUE != uim_efs_write(fhandle,data_to_write,
                                                    int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }
  }

  //Print the voltage class information
#if defined (FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES)
  num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s %s\r\n",
                       "Operating Voltage = ",
                       (uim_current_voltage_class[uim_drv_slot]==
                        UIM_VOLTAGE_1_8V)?"1.8":"3.0");
  if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                 int32touint32(MIN(num_bytes,data_to_write_size)))))
  {
    UIM_CLOSE_LOG_TIMEOUT_INFO();
    return FALSE;
  }
#endif /* FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES */

  //Print the protocol being used
#if defined (FEATURE_UIM_UICC)
  if (cmd_ptr_in_use != NULL)
  {
    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s %d\r\n",
                         "Command Protocol = ",
                         cmd_ptr_in_use->hdr.protocol);
    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }
  }
  else
  {
    /* Indicate that this command is an Internal Command */
    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s\r\n","The command Pointer is NULL");
    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }
  }
#endif /* FEATURE_UIM_UICC */


  /* Check to see if the timeout is because of an error */
  if (uim_max_parity_error ||
      uim_max_rx_break_error ||
      uim_max_overrun_error)
  {
    /* Log the error because of which we have a timeout signal */
    (void)snprintf((char *)data_to_write,data_to_write_size,"%s","Timeout occured with the following error: ");
    if (uim_max_parity_error)
    {
      (void)std_strlcat((char *)data_to_write," Max Parity Error\r\n", data_to_write_size);
    }
    if (uim_max_rx_break_error)
    {
      (void)std_strlcat((char *)data_to_write," Max RX_Break Error\r\n",
                        data_to_write_size);
    }
    if (uim_max_overrun_error)
    {
      (void)std_strlcat((char *)data_to_write," Max Overrun Error\r\n",
                        data_to_write_size);
    }
    if (uim_overrun_error)
    {
      (void)std_strlcat((char *)data_to_write,"Processing Overrun Error\r\n",
                        data_to_write_size);
    }


    unum_bytes = strlen(data_to_write);
    if (TRUE != uim_efs_write(fhandle,data_to_write,unum_bytes))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }
  }
#ifdef UIM_DRIVER_TIMESTAMP
  else if ( uim_timeout_signaled_by_driver )
  {
    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s %s 0x%lX rcvd at 0x%lx\r\n",
                        "Timeout signaled by the driver",
                        "expecting a byte within",
                        uim_work_waiting_time_sleep_ticks[uim_drv_slot],
                        uim_num_sleep_ticks_elapsed);
    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }
  }
#endif /* UIM_DRIVER_TIMESTAMP */
  else
  {
    /* Not an error this is a genuine timeout */
    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s\r\n",
            "Timeout occured waiting for response from the card");
    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }
  }

  /* Check to see if a command is in progress */
  if (uim_command_in_progress)
  {
    /* Indicate that a command is in progress */
    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s\r\n",
                        "A command is in progress");
    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }

    /* Check to see if this is an internal command or an external command */
    if (uim_static_buffer_used)
    {
      /* Indicate that this command is an Internal Command */
      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s\r\n","The command is an Internal command");
      if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                     int32touint32(MIN(num_bytes,data_to_write_size)))))
      {
        UIM_CLOSE_LOG_TIMEOUT_INFO();
        return FALSE;
      }
    }
    else if (cmd_ptr_in_use != NULL)
    {
      /* Indicate that this command is an external command and also
       * log the response call back pointer we can later locate
       * who has initiated this command and other details
       */
      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s %s 0x%lX\r\n",
                          "The command is an External command ",
                          "With the call back function pointing to ",
                          (uint32)cmd_ptr_in_use->hdr.rpt_function);
      if ((num_bytes >= 0) &&  (TRUE != uim_efs_write(fhandle,data_to_write,
                                                      int32touint32(MIN(num_bytes,data_to_write_size)))))
      {
        UIM_CLOSE_LOG_TIMEOUT_INFO();
        return FALSE;
      }
    }
    else
    {
      /* Indicate that this command is an Internal Command */
      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s\r\n","The command Pointer is NULL");
      if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                     int32touint32(MIN(num_bytes,data_to_write_size)))))
      {
        UIM_CLOSE_LOG_TIMEOUT_INFO();
        return FALSE;
      }
    }

    if(cmd_ptr_in_use != NULL)
    {
      /* Indicate the command count done on this command*/
      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s %d\r\n",
                          "cmd_ptr->hdr.count : ",
                          cmd_ptr_in_use->hdr.cmd_count);
      if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                     int32touint32(MIN(num_bytes,data_to_write_size)))))
      {
        UIM_CLOSE_LOG_TIMEOUT_INFO();
        return FALSE;
      }

      /* Indicate the command itself, SELECT, Fetch etc */
      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s 0x%X\r\n","The command is : ",
                          cmd_ptr_in_use->hdr.command);
      if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                     int32touint32(MIN(num_bytes,data_to_write_size)))))
      {
        UIM_CLOSE_LOG_TIMEOUT_INFO();
        return FALSE;
      }


      /* Indicate the protocol GSM/USIM etc */
      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s 0x%X\r\n",
                          "The command protocol is : ",
                          uim_req_buf.protocol);
      if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                     int32touint32(MIN(num_bytes,data_to_write_size)))))
      {
        UIM_CLOSE_LOG_TIMEOUT_INFO();
        return FALSE;
      }
    }

    /* Check to see if the last selection was an ADF */
    if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_ADF)
    {
      /* Log the last selected ADF */
      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s 0x%x 0x%x\r\n",
                          "ADF was Last selected DF2,EF",
                          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2,
                          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef);
    }
    else
    {
      /* Log the last selected directory information */
      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s 0x%x 0x%x 0x%x\r\n",
                          "Last DF1,DF2,EF",
                          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df,
                          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2,
                          uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef);
    }

    /* Log the signals that are pending on the TCB */
    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s 0x%lX\r\n",
                        "Signals pending in uim TCB are : ",
                        uim_tcb.sigs);
    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }

    /* Log the signals that UIM task is waiting / will look for */
    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s 0x%lX\r\n",
                        "UIM Task currently is interested in signals : ",
                        i_mask);
    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
    /* Log the information that we have about the atr_bytes */
    if ( gsdi_data.card_atr.atr_len > 255 )
    {
      /* Indicate the fact that we are showing only 256 bytes */
      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"Num atr bytes is %ld %s\r\n",
                          gsdi_data.card_atr.atr_len,
                          "truncating to 255");
      if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                     int32touint32(MIN(num_bytes,data_to_write_size)))))
      {
        UIM_CLOSE_LOG_TIMEOUT_INFO();
        return FALSE;
      }
      gsdi_data.card_atr.atr_len = 255;
    }
    else
    {
      /* Indicate the number of ATR bytes */
      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"Num atr bytes is %ld\r\n",
                          gsdi_data.card_atr.atr_len);
      if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                     int32touint32(MIN(num_bytes,data_to_write_size)))))
      {
        UIM_CLOSE_LOG_TIMEOUT_INFO();
        return FALSE;
      }
    }

    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s",
                        "ATR : ");
    if(gsdi_data.card_atr.atr_len < GSDI_MAX_ATR_BYTES)
    {
      for(i=0; (int)i<gsdi_data.card_atr.atr_len; i++)
      {
        (void)snprintf(temp_buff,sizeof(temp_buff),"0x%x ",gsdi_data.card_atr.atr_buffer[i]);
        (void)std_strlcat(data_to_write,temp_buff,data_to_write_size);
      }
    }
    (void)std_strlcat(data_to_write,"\r\n",data_to_write_size);
    unum_bytes = strlen(data_to_write);
    if (TRUE != uim_efs_write(fhandle,data_to_write,unum_bytes))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }
#else
    /* Indicate the number of ATR bytes */
    if (cmd_ptr_in_use != NULL)
    {
      num_bytes = snprintf((char *)data_to_write,data_to_write_size,
                          "uim_drv_slot = %d cmd_ptr->hdr.slot = %d\r\n",
                          uim_drv_slot, cmd_ptr_in_use->hdr.slot);
      if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                     int32touint32(MIN(num_bytes,data_to_write_size)))))
      {
        UIM_CLOSE_LOG_TIMEOUT_INFO();
        return FALSE;
      }
    }

    if (uim_drv_slot == UIM_DRV_SLOT1)
    {
      /* Log the information that we have about the atr_bytes */
      if ( gsdi_data.card_atr.atr_len > 255 )
      {
        /* Indicate the fact that we are showing only 256 bytes */
        num_bytes = snprintf((char *)data_to_write,data_to_write_size,"Num atr bytes is %ld %s\r\n",
                            gsdi_data.card_atr.atr_len,
                            "truncating to 255");
        if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                       int32touint32(MIN(num_bytes,data_to_write_size)))))
        {
          UIM_CLOSE_LOG_TIMEOUT_INFO();
          return FALSE;
        }
        gsdi_data.card_atr.atr_len = 255;
      }
      else
      {
        /* Indicate the number of ATR bytes */
        num_bytes = snprintf((char *)data_to_write,data_to_write_size,"Num atr bytes is %ld\r\n",
                            gsdi_data.card_atr.atr_len);
        if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                       int32touint32(MIN(num_bytes,data_to_write_size)))))
        {
          UIM_CLOSE_LOG_TIMEOUT_INFO();
          return FALSE;
        }
      }

      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s",
                          "ATR : ");
      for(i=0; i<gsdi_data.card_atr.atr_len; i++)
      {
        (void)snprintf(temp_buff,sizeof(temp_buff),"0x%x ",gsdi_data.card_atr.atr_buffer[i]);
        (void)std_strlcat(data_to_write,temp_buff,data_to_write_size);
      }
      (void)std_strlcat(data_to_write,"\r\n",data_to_write_size);
      unum_bytes = strlen(data_to_write);
      if (TRUE != uim_efs_write(fhandle,data_to_write,unum_bytes))
      {
        UIM_CLOSE_LOG_TIMEOUT_INFO();
        return FALSE;
      }
    }
    else
    {
      /* Log the information that we have about the atr_bytes */
      if ( gsdi_data.card_atr.atr_len > 255 )
      {
        /* Indicate the fact that we are showing only 256 bytes */
        num_bytes = snprintf((char *)data_to_write,data_to_write_size,"Num atr bytes is %ld %s\r\n",
                            gsdi_data.card_atr_2.atr_len,
                            "truncating to 255");
        if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                       int32touint32(MIN(num_bytes,data_to_write_size)))))
        {
          UIM_CLOSE_LOG_TIMEOUT_INFO();
          return FALSE;
        }
        gsdi_data.card_atr_2.atr_len = 255;
      }
      else
      {
        /* Indicate the number of ATR bytes */
        num_bytes = snprintf((char *)data_to_write,data_to_write_size,"Num atr bytes is %ld\r\n",
                            gsdi_data.card_atr_2.atr_len);
        if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                       int32touint32(MIN(num_bytes,data_to_write_size)))))
        {
          UIM_CLOSE_LOG_TIMEOUT_INFO();
          return FALSE;
        }
      }

      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s",
                          "ATR : ");
      for(i=0; i<gsdi_data.card_atr_2.atr_len; i++)
      {
        (void)snprintf(temp_buff,sizeof(temp_buff),"0x%x ",gsdi_data.card_atr_2.atr_buffer[i]);
        (void)std_strlcat(data_to_write,temp_buff,data_to_write_size);
      }
      (void)std_strlcat(data_to_write,"\r\n",data_to_write_size);
      unum_bytes = strlen(data_to_write);
      if (TRUE != uim_efs_write(fhandle,data_to_write,unum_bytes))
      {
        UIM_CLOSE_LOG_TIMEOUT_INFO();
        return FALSE;
      }
    }
#endif /* ! FEATURE_UIM_SUPPORT_DUAL_SLOTS */

    if((uim_op_params_buf.FI) < UIM_CRCF_SIZE && (uim_op_params_buf.DI < UIM_BRAF_SIZE))
    {
      /* Log the information about the FI and DI values being used */
      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"Running with FI = %d, DI = %d \r\n",
                           (int)crcf_values[uim_op_params_buf.FI],
                           (int)braf_values[uim_op_params_buf.DI]);
    }
    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }


    /* Log the value of timeout value calculated */
    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"Calculated timeout value is %ld \r\n",
                        uim_work_waiting_time[uim_drv_slot]);
    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }

    /* Log the driver UIM_RX_STATE indicates what exactly the
     * driver is expecting
     */
    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s %d\r\n",
                        "uim_rx_state is : ",
                        uim_rx_state[uim_drv_slot]);
    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }

    /* Log the driver UIM_TX_STATE indicates what exactly the
     * driver is transmiting
     */
    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s %d\r\n",
                        "uim_tx_state is : ",
                        uim_tx_state[uim_drv_slot]);
    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }

    /* Check to see if this command has data to be sent*/
    if ( uim_req_buf.instrn_case != UIM_INSTRN_CASE_2)
    {
      /* Check to see if this is the last byte */
      if (num_bytes_to_send[uim_drv_slot] == 1)
      {
        /* Check to see if we are in transmitting data Mode */
        if ( uim_tx_state[uim_drv_slot] == UIM_TX_SND_CMD_DATA )
        {
          /* Log the info that we are done sending the APDU */
          num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s\r\n",
                              "Both the command and data has been sent");
          if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                         int32touint32(MIN(num_bytes,data_to_write_size)))))
          {
            UIM_CLOSE_LOG_TIMEOUT_INFO();
            return FALSE;
          }
        }
        else
        {
          /* Log the info that we are done sending the command */
          num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s\r\n",
                              "Command is sent but not the data");
          if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                         int32touint32(MIN(num_bytes,data_to_write_size)))))
          {
            UIM_CLOSE_LOG_TIMEOUT_INFO();
            return FALSE;
          }
        }
      }
      else
      {
        /* We are in the middle of sending bytes either command or data */
        num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s %d\r\n",
                            "Bytes remaining to send is : ",
                            num_bytes_to_send[uim_drv_slot]);
        if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                       int32touint32(MIN(num_bytes,data_to_write_size)))))
        {
          UIM_CLOSE_LOG_TIMEOUT_INFO();
          return FALSE;
        }
      }
    }
    else
    {
      /* This command does not have data to be sent but expects
       * data from the card.  Indicate the number of bytes expected
       */
      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s %d Bytes\r\n",
                          "This command expects data - Expecting ",
                          num_resp_data_bytes_expected[uim_drv_slot]);
      if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                     int32touint32(MIN(num_bytes,data_to_write_size)))))
      {
        UIM_CLOSE_LOG_TIMEOUT_INFO();
        return FALSE;
      }


      /* Indicate the number of bytes that we have received*/
      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s %d Bytes\r\n",
                          "We have received",
                          num_resp_bytes_rcvd[uim_drv_slot]);
      if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                     int32touint32(MIN(num_bytes,data_to_write_size)))))
      {
        UIM_CLOSE_LOG_TIMEOUT_INFO();
        return FALSE;
      }


      /* List the bytes that we have already received  if in
       * meaning full range*/

      if ( num_resp_bytes_rcvd[uim_drv_slot] > 256 )
      {
        num_resp_bytes_rcvd[uim_drv_slot] = 256;
        /* Indicate the fact that we are showing only 256 bytes */
        num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s\r\n",
                            "num_resp_bytes_rcvd out of range truncating to 256");
        if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                       int32touint32(MIN(num_bytes,data_to_write_size)))))
        {
          UIM_CLOSE_LOG_TIMEOUT_INFO();
          return FALSE;
        }
      }

      num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s",
                          "Bytes received :");
      for(i=0; i<num_resp_bytes_rcvd[uim_drv_slot]; i++)
      {
        (void)snprintf(temp_buff,sizeof(temp_buff),"0x%x ",*((unsigned char *)(&uim_req_buf.rsp_ptr->rsp) + i));
        (void)std_strlcat(data_to_write,temp_buff,data_to_write_size);
      }
      (void)std_strlcat(data_to_write,"\r\n",data_to_write_size);
      unum_bytes = strlen(data_to_write);
      if (TRUE != uim_efs_write(fhandle,data_to_write,unum_bytes))
      {
        UIM_CLOSE_LOG_TIMEOUT_INFO();
        return FALSE;
      }
    }

    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s",
                        "APDU being sent :");

    for(i=0;i<(NUM_BYTES_APDU_HDR +
               ((uim_req_buf.instrn_case == UIM_INSTRN_CASE_2)?0:
               uim_req_buf.apdu_hdr.p3));i++)
    {
      (void)snprintf(temp_buff,sizeof(temp_buff),"0x%x ",*((unsigned char *)(&uim_req_buf.apdu_hdr) + i));
      (void)std_strlcat(data_to_write,temp_buff,data_to_write_size);
    }
    (void)std_strlcat(data_to_write,"\r\n",data_to_write_size);

    unum_bytes = strlen(data_to_write);
    if (TRUE != uim_efs_write(fhandle,data_to_write,unum_bytes))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }

    if((request_buffer_ptr[uim_drv_slot]) != NULL)
    {
       num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s %d value 0x%x\r\n",
                          "Current position in the buffer is : ",
                          ((unsigned int) request_buffer_ptr[uim_drv_slot] -
                           (unsigned int) (&uim_req_buf.apdu_hdr))+1,
                          *(request_buffer_ptr[uim_drv_slot]));
    }
    else
    {
      MSG_HIGH("Request Buffer Ptr is NULL, did not write current position in buffer",0,0,0);
    }

    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }


    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s 0x%lX\r\n",
                        "UART STATUS is : ", uart_status);
    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }

  }
  else
  {
    num_bytes = snprintf((char *)data_to_write,data_to_write_size,"%s\r\n","No command in progress\r\n");
    if ((num_bytes >= 0) && (TRUE != uim_efs_write(fhandle,data_to_write,
                                                   int32touint32(MIN(num_bytes,data_to_write_size)))))
    {
      UIM_CLOSE_LOG_TIMEOUT_INFO();
      return FALSE;
    }
  }

  /* ------------------------------------------------------------------------
     Need to close the file_handle used in the open.
     If the close fails, we'll assume it is still usable for now.
     ----------------------------------------------------------------------*/
  fhandle = efs_close(fhandle);
  if (0 > fhandle)
  {
    MSG_ERROR("Failed to close written UimReset.txt file",0,0,0);
  }

  /* Let us return the memory allocated */
  mem_free(&tmc_heap, data_to_write);

  MSG_HIGH("End logging informationto UimReset.Txt",0,0,0);

  return TRUE;

#else
#error code not present
#endif /* PLATFORM_LTK*/

} /* uim_log_timeout_info */

/* <EJECT> */

/*===========================================================================

FUNCTION UIM_GET_USIM_PATH_FROM_FILE_ID

DESCRIPTION
  This function takes as parameters the file id.  Sets paramter path
  to the path of the File.

DEPENDENCIES


RETURN VALUE
  TRUE if the File ID was found

SIDE EFFECTS
  Will return the path field of the command for those items that are selected
  by using the command path.

===========================================================================*/

boolean uim_get_usim_path_from_file_id
(
word file_id,
word ** path
/* Parameter required for RPC to keep UMTS/1x interface the same
   Tell lint to ignore it. */
/*lint -esym(715,file_id) */
)
{
#ifdef FEATURE_UIM_USIM
#error code not present
#endif /* FEATURE_UIM_USIM */
  *path = NULL;
  return FALSE;
}
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_TELECOM_PATH_FROM_FILE_ID

DESCRIPTION
  This function takes as parameters the file id.  Sets paramter path
  to the path of the File.

DEPENDENCIES


RETURN VALUE
  TRUE if the File ID was found

SIDE EFFECTS
  Will return the path field of the command for those items that are selected
  by using the command path.

===========================================================================*/

boolean uim_get_telecom_path_from_file_id
(
word file_id,
word ** path
)
{
  byte i;
  if (path == NULL)
  {
    return FALSE;
  }
  for(i = 0; i < UIM_MAX_TELECOM_ITEMS-1; i++)
  {
    if(uim_telecom_file_path[i][UIM_MAX_PATH_ELEMENTS-2] == file_id)
    {
      *path = (word*)(uim_telecom_file_path[i]);
      return TRUE;
    }
  }
  *path = NULL;
  return FALSE;
}
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_EF_UNDER_MF_PATH_FROM_FILE_ID

DESCRIPTION
  This function takes as parameters the file id.  Sets paramter path
  to the path of the File.

DEPENDENCIES


RETURN VALUE
  TRUE if the File ID was found

SIDE EFFECTS
  Will return the path field of the command for those items that are selected
  by using the command path.

===========================================================================*/

boolean uim_get_ef_under_mf_path_from_file_id
(
word file_id,
word ** path
)
{
  byte i;
  if (path == NULL)
  {
    return FALSE;
  }
  for(i = 0; i < UIM_MAX_EF_UNDER_MF_ITEMS-1; i++)
  {
    if(uim_ef_under_mf_file_path[i][UIM_MAX_PATH_ELEMENTS-2] == file_id)
    {
      *path = (word*)(uim_ef_under_mf_file_path[i]);
      return TRUE;
    }
  }
  *path = NULL;
  return FALSE;
}
/* <EJECT> */

/*===========================================================================

FUNCTION UIM_GET_FILE_PATH

DESCRIPTION
  This function takes as parameters the table index and the item index of an
  EF and returns the full path to the EF.

DEPENDENCIES
  The path is determined from the item in the command.  Some items are
  identified as EFs selected by using the path field of the command.  This
  function will return the path field when these items are found in the
  item field of the command.

RETURN VALUE
  Returns a pointer to the path of the item.

SIDE EFFECTS
  Will return the path field of the command for those items that are selected
  by using the command path.

  Will update uim_item_sel_rsp_cached to indicate whether or not the driver
  needs to do a get response based on whether or not the cache
  for the response is populated.

  Exports the concerned pointer which holds or supposed to hold the
  select response

===========================================================================*/
const word* uim_get_file_path
(
uim_items_enum_type item,    /* EF to be accessed if file type is EF*/
word const *path
/* Selection of an EF by the specified path */
)
{
  /* Path to return */
  const word *select_path = NULL;
  /* Indices for the items table */
  byte table_index, item_index;

  /* get the table index and item index for the EF */
  table_index = UIM_GET_TABLE_INDEX(item);
  item_index  = UIM_GET_ITEM_INDEX((int)item);

  /* Get the path based on the table index and item index */
  switch (table_index)
  {
    case UIM_EF_UNDER_MF_START:
      if ( item_index >= ( sizeof(uim_ef_under_mf_select_rsp_cache) /
                           sizeof(uim_ef_under_mf_select_rsp_cache[0]) ) )
      {
        UIM_ERROR_MSG("Item index exceeds the cache size",item,0,0);
      }
      else
      {
        /* get the path from the table for EFs under MFs */
        select_path = uim_ef_under_mf_file_path[item_index];

        uim_item_sel_rsp_cached =
          uim_ef_under_mf_select_rsp_cache[item_index].is_cached;
        select_rsp_cache_item_ptr =
          &uim_ef_under_mf_select_rsp_cache[item_index];
      }
      break;

#ifdef FEATURE_UIM_RUIM
    case UIM_RUIM_EF_START:
      if ( item_index >= ( sizeof(uim_ruim_file_select_rsp_cache) /
                           sizeof(uim_ruim_file_select_rsp_cache[0]) ) )
      {
        UIM_ERROR_MSG("Item index exceeds the cache size",item,0,0);
      }
      else
      {
        /* get the path from the CDMA EFs table */
        select_path = uim_ruim_file_path[item_index] ;

        uim_item_sel_rsp_cached =
          uim_ruim_file_select_rsp_cache[item_index].is_cached;
        select_rsp_cache_item_ptr =
          &uim_ruim_file_select_rsp_cache[item_index];
      }
      break;
#endif

#ifdef FEATURE_UIM_GSM
#error code not present
#endif /* FEATURE_UIM_GSM */

#ifdef FEATURE_UIM_USIM
#error code not present
#endif

#ifdef FEATURE_UIM_ISIM
    case UIM_ISIM_EF_START:
      if ( item_index >= ( sizeof(uim_isim_file_select_rsp_cache) /
                           sizeof(uim_isim_file_select_rsp_cache[0]) ) )
      {
        UIM_ERROR_MSG("Item index exceeds the cache size",item,0,0);
      }
      else
      {
        /* get the path from the ISIM EFs table */
        select_path = uim_isim_file_path[item_index];

        uim_item_sel_rsp_cached =
          uim_isim_file_select_rsp_cache[item_index].is_cached;
        select_rsp_cache_item_ptr =
          &uim_isim_file_select_rsp_cache[item_index];
      }
      break;
#endif /* FEATURE_UIM_ISIM*/

#ifdef FEATURE_UIM_WIM
    case UIM_WIM_EF_START:
      if ( item_index >= ( sizeof(uim_wim_file_select_rsp_cache) /
                           sizeof(uim_wim_file_select_rsp_cache[0]) ) )
      {
        UIM_ERROR_MSG("Item index exceeds the cache size",item,0,0);
      }
      else
      {
        /* get the path from the WIM EFs table */
        select_path = uim_wim_file_path[item_index];

        uim_item_sel_rsp_cached =
          uim_wim_file_select_rsp_cache[item_index].is_cached;
        select_rsp_cache_item_ptr =
          &uim_wim_file_select_rsp_cache[item_index];
      }
      break;
#endif /* FEATURE_UIM_WIM */

    case UIM_TELECOM_EF_START:
      if ( item_index >= ( sizeof(uim_telecom_file_select_rsp_cache) /
                           sizeof(uim_telecom_file_select_rsp_cache[0]) ) )
      {
        UIM_ERROR_MSG("Item index exceeds the cache size",item,0,0);
      }
      else
      {
        /* get the path from the Telecom EFs table */
        select_path = uim_telecom_file_path[item_index];

        uim_item_sel_rsp_cached =
          uim_telecom_file_select_rsp_cache[item_index].is_cached;
        select_rsp_cache_item_ptr =
          &uim_telecom_file_select_rsp_cache[item_index];
      }
      break;

    case UIM_CINGULAR_EF_START:
      if ( item_index >= ( sizeof(uim_cingular_file_select_rsp_cache) /
                           sizeof(uim_cingular_file_select_rsp_cache[0]) ) )
      {
        UIM_ERROR_MSG("Item index exceeds the cache size",item,0,0);
      }
      else
      {
        select_path = uim_cingular_file_path[item_index];

        uim_item_sel_rsp_cached =
          uim_cingular_file_select_rsp_cache[item_index].is_cached;
        select_rsp_cache_item_ptr =
          &uim_cingular_file_select_rsp_cache[item_index];
      }
      break;

  case UIM_HZI_EF_START:
      if ( item_index >=UIM_MAX_HZI_ITEMS)
      {
        UIM_ERROR_MSG("Item index exceeds the table size",item,0,0);
      }
      else
      {
        select_path = uim_hzi_file_path[item_index];
        /* We do not want to cache this Proprietary files get response
         * there by no cache table
         */
      }
      break;

#ifdef FEATURE_UIM_MFLO
#error code not present
#endif /* FEATURE_UIM_MFLO*/

    case UIM_EF_PATH_START:
      select_path = path;
      /* Do not cache if uim does not expose this item */
      if ( item != UIM_EF_BY_PATH )
      {
        if ( item_index >= ( sizeof(uim_ef_by_path_select_rsp_cache) /
                             sizeof(uim_ef_by_path_select_rsp_cache[0]) ) )
        {
          UIM_ERROR_MSG("Item index exceeds the cache size",item,0,0);
        }
        else
        {
          uim_item_sel_rsp_cached =
            uim_ef_by_path_select_rsp_cache[item_index].is_cached;
          select_rsp_cache_item_ptr =
            &uim_ef_by_path_select_rsp_cache[item_index];
        }
      }
      break;

    default:
      /* Error case where the path gets set to all FFs */
      select_path = uim_file_path[0];
      break;
  }

  /* no caching for non default channels */
  if (uim_cur_channel != UIM_DEFAULT_CHANNEL)
  {
    uim_item_sel_rsp_cached =
      FALSE;
    select_rsp_cache_item_ptr = NULL;
  }

  return(select_path);
} /* uim_get_file_path */

#ifdef FEATURE_UIM_HOT_SWAP

/*===========================================================================

FUNCTION UIM_CARD_SWAP_DETECTED

DESCRIPTION
  This function is called when hardware detects a card removal or insertion.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void uim_card_swap_detected(uim_slot_type            slot,
              uim_card_swap_enum_type status)
{

  /* Match UIM to GSDI structure */
  switch (slot) {
    case UIM_SLOT_1:
    gsdi_command_ptr.slot = GSDI_SLOT_1;

    break;
    case UIM_SLOT_2:
    gsdi_command_ptr.slot = GSDI_SLOT_2;
    break;

    default:
    MSG_HIGH("Hot Swap: Wrong slot number!",0,0,0);
    return;
  }

  switch (status) {
    case UIM_CARD_REMOVED:
    gsdi_command_ptr.status = GSDI_UIM_CARD_REMOVED;
    /* Poll the card */
    uim_poll_pending = TRUE;
    break;
    case UIM_CARD_INSERTED:
    gsdi_command_ptr.status = GSDI_UIM_CARD_INSERTED;
    break;
    default:
    MSG_HIGH("Hot Swap: Wrong status!",0,0,0);
    return;
  }
  /* Signal UIM task to process this */
  (void) rex_set_sigs( &uim_tcb, UIM_CARD_SWAP_SIG );

}
#endif

/* <EJECT>^L */
/*===========================================================================

FUNCTION UIM_GET_ESN_ME ( )

DESCRIPTION
  This function returns the HW ME ESN cached by TMC at power up.

DEPENDENCIES
  None

RETURN VALUE
  The HW ME ESN.  If the ESN was not cached properly, 0 is returned

SIDE EFFECTS
  None

===========================================================================*/
dword uim_get_esn_me ( void )
{
#ifdef FEATURE_UIM_RUIM
  return tmc_get_stored_esn_me();
#else
  return 0;
#endif /* FEATURE_UIM_RUIM */
}

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_NOTIFY_ERRROR

DESCRIPTION
  This function notifies the higher layer (GSDI) if UIM encounters an
  unexpected error during power up or an internal wake up and passes the
  status of the UIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_notify_error
(
uim_status_type uim_status_param
  #ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
,uim_slot_type uim_slot_param
  #endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
)
{
#ifdef UIM_DRIVER_TIMESTAMP
/* Just in case if there was no card and we never have done
 * a link established but are reporting an error let us stop
 * the driver time stamping mechanism now
 */
uim_StopDriverTimeStamp();
#endif /* UIM_DRIVER_TIMESTAMP */
#if defined(FEATURE_MMGSDI) || defined(FEATURE_UIM_USIM)

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  gsdi_uim_error_condition ( uim_status_param );
#else
  gsdi_uim_slot_error_condition ( uim_status_param, uim_slot_param );
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

#else
  ERR (" Error condition in UIM, status is %d", uim_status_param, 0, 0);
#endif /* FEATURE_MMGSDI || FEATURE_UIM_USIM */
#ifdef FEATURE_UIM_LOG_TO_EFS
  if(uim_efslog_logging_in_progress)
  {
    uim_efslog_logging_in_progress = FALSE;
    (void) rex_set_sigs( &uim_tcb, UIM_EFSLOG_PURGE_SIG );
  }
#endif /* FEATURE_UIM_LOG_TO_EFS */
#ifdef FEATURE_OEMOMH
{
  extern void uim_send_err_to_ui(uim_status_type uim_status_param);
  uim_send_err_to_ui(uim_status_param);
}
#endif
#ifdef FEATURE_UIM_CARD_ERR_UIM_LDO_PUP
#error code not present
#endif /* FEATURE_UIM_CARD_ERR_UIM_LDO_PUP */

} /* uim_notify_error */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SEND_ERR_RPT

DESCRIPTION
  This function, based on the command mode calls the module that sends an
  error report.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The command buffer is released.

===========================================================================*/
LOCAL void uim_send_err_rpt
(
uim_cmd_type *cmd_ptr                        /* the command to queue up */
)
{
  /* This command has been completed */
  uim_command_in_progress = FALSE;

  /* Reset the command re-queued flag */
  uim_command_requeued = FALSE;

  switch (uim_cmd_mode)
  {
#ifdef FEATURE_UIM_RUIM
    case UIM_CDMA_CMD:
      uim_send_cdma_err_rpt ( cmd_ptr);
      break;
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_UIM_GSM
#error code not present
#endif /* FEATURE_UIM_GSM */

#ifdef FEATURE_UIM_USIM
#error code not present
#endif /* FEATURE_UIM_GSM */

#ifdef FEATURE_UIM_ISIM
    case UIM_ISIM_CMD:
      uim_send_isim_err_rpt ( cmd_ptr);
      break;
#endif /* FEATURE_UIM_ISIM */


#ifdef FEATURE_UIM_UTIL
    case UIM_UTIL_CMD:
      uim_send_util_err_rpt (cmd_ptr);
      break;
#endif /* FEATURE_UIM_UTIL */

#ifdef FEATURE_UIM_MFLO
#error code not present
#endif /* FEATURE_UIM_MFLO */

    case UIM_GENERIC_CMD:
    default:
      uim_send_generic_err_rpt (cmd_ptr);
      break;
  } /* end of switch */
} /* uim_send_err_rpt */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_FLUSH_COMMAND

DESCRIPTION
  This procedure sends an error report to the reqd tasks and releases the
  command buffer in some cases.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Releases the command buffer when uim_send_err_rpt() is not called.

===========================================================================*/
void uim_flush_command
(
uim_flush_type flush_type
)
{
  word i;
  /* index */
  int count;
  /* count */
  uim_cmd_type *cmd_ptr;
  /* Pointer to received command */

  static uim_rpt_type back_up_uim_rpt_buf;
  /* Space to back-up the current report buffer */

  count = 0;

  /* Back-up the current state of the uim_rpt_buf before flushing commands */
  memcpy(&back_up_uim_rpt_buf, &uim_rpt_buf, sizeof(uim_rpt_type));

  if (flush_type == UIM_FLUSH_ALL)
  {
    count = q_cnt(& uim_cmd_q);

    /* Command was not attempted when all the commands are flushed */
    uim_rpt_buf.cmd_transacted = FALSE;
  }
  else if (flush_type == UIM_FLUSH_ONLY)
  {
    count = 1;
  }

  MSG_HIGH("Flushing %d commands, uim status is %d", count,uim_status,0);

  for (i = 0; i < count; i++)
  {
    /* get the command from the queue */
    if ((cmd_ptr = (uim_cmd_type *)q_get( &uim_cmd_q )) != NULL)
    {
      /* Get the mode of the command */
      uim_cmd_mode = (uim_command_mode_type)
                     UIM_GET_CMD_MODE((int)cmd_ptr->hdr.command);

      /* if the command was queued again, it means that the processing
      of this was started and so we need to send an error report only
      for the first command in the queue */
      if (uim_command_requeued)
      {
        uim_send_err_rpt(cmd_ptr);
      }
      /* for the remaining commands, we need to only set the status
         as invalid and signal the task */
      else
      {
        /* return an invalid status in the header to all the commands */
        cmd_ptr->hdr.status = UIM_ERR_S;

        if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
        {
          (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                               cmd_ptr->hdr.cmd_hdr.sigs );
        }

        /* If always reporting & have a report function, report failure */
        if (((int)cmd_ptr->hdr.options & (int)UIM_OPTION_ALWAYS_RPT) != 0)
        {
          uim_send_err_rpt(cmd_ptr);
        }
        else
        {  /* return buffer to free queue */
          if ( cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
          {
            /* place command on requested queue */
            q_put( cmd_ptr->hdr.cmd_hdr.done_q_ptr,
                   &cmd_ptr->hdr.cmd_hdr.link );
          }
        } /* (cmd_ptr->hdr.options & UIM_OPTION_ALWAYS_RPT) */
      }  /*  (command_requeued) */
    } /* (cmd_ptr = (uim_cmd_type *)q_get( &uim_cmd_q )) != NULL) */
  }  /* for loop */

  /* Restore the state of the uim_rpt_buf before flushing commands */
  memcpy(&uim_rpt_buf, &back_up_uim_rpt_buf, sizeof(uim_rpt_type));

  /* Clear the command queue signal for the command */
  (void) rex_clr_sigs( &uim_tcb, UIM_CMD_Q_SIG);
} /* uim_flush command */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_COMMAND_RESPONSE_CALLBACK

DESCRIPTION
  This procedure is the call back function provided in the command to the
  driver and upon calling this it initiates the processing of the response
  that has been received from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the UIM_CMD_RSP_SIG.

===========================================================================*/

void uim_command_response_callback ( void )
{
  /* set the command response signal */
  MSG_LOW(" Recd Command Response Signal",0,0,0);
  (void) rex_set_sigs( &uim_tcb, UIM_CMD_RSP_SIG );

  /* Clear the transaction timer as soon as the driver receives the response */
  (void) rex_clr_timer( &uim_transaction_rsp_timer );

  /* clear the signal as well just in case if it was set */
  (void) rex_clr_sigs( &uim_tcb, UIM_TRANSACTION_SIG );
} /*  uim_command_response_callback */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_UNSOLICITED_RSP_CALLBACK

DESCRIPTION
  This procedure is called by the low level driver when an unsolicited response
  has been recd from the UIM and this initiates the processing of the
  unsolicited response.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the signal UIM_UNSOLICITED_RSP_SIG

===========================================================================*/

void uim_unsolicited_rsp_callback ( void )
{
  MSG_LOW(" Recd Unsolicited Response Signal",0,0,0);
  /* set the unsolicited response signal */
  (void) rex_set_sigs(& uim_tcb, UIM_UNSOLICITED_RSP_SIG );
} /*  uim_unsolicited_rsp_callback */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SET_SELECT_PATH

DESCRIPTION
  This procedure determines the path to be selected in order to access an EF.
  It uses the information in the uim_last_sel_dir structure and fills the
  uim_path_to_be_sel array. It also provides the optimal path for all the
  Selects of any file type.

DEPENDENCIES
  None

RETURN VALUE
  Returns a boolean status. TRUE if path is the correct path to execute the
  command. FALSE if it is not the right path.

SIDE EFFECTS
  Modifies the uim_path_to_be_sel array.

===========================================================================*/

LOCAL boolean uim_set_select_path
(
const word *new_path
  #ifdef FEATURE_UIM_UICC
, uim_aid_type const *new_adf
  #endif /* FEATURE_UIM_UICC */
)
{
  byte i = 0;
  boolean status = FALSE;


  if (new_path == NULL)
  {
    MSG_ERROR("uim_set_select_path new_path is NULL returning TRUE",0,0,0);
    return(TRUE);
  }
  /* Check to see if the EF is present in the path. If it does not exist,
     directly send the command to the generic module which will release
     the buffer if the path is invalid */
  if (new_path[UIM_PATH_EF_INDEX] == UIM_NO_FILE_SEL)
  {
    return(TRUE);
  }

  /* Check to see if the current directory selection already has the
     required path, for path length of 4 */

  if((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef        == new_path[UIM_PATH_EF_INDEX+1]) &&
     (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel    == TRUE) &&
     (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2       == new_path[UIM_PATH_DF2_INDEX])&&
     (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df3       == new_path[UIM_PATH_DF3_INDEX]) &&
     ((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_ADF) ||
     (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df    == new_path[UIM_PATH_DF1_INDEX])))
  {
    return(TRUE);
  }

  /* For EFs under the MF */
  if (new_path[UIM_PATH_DF1_INDEX] == UIM_MF_ID)
  {
    if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef != new_path[UIM_PATH_EF_INDEX])
    {
      /* Select the MF and then the EF */
      uim_path_to_be_sel[i++] = UIM_MF_ID;
      uim_path_to_be_sel[i++] = new_path[UIM_PATH_EF_INDEX];
    }
    else
    {
      status = TRUE;
    }
  }
#if defined (FEATURE_MMGSDI_GSM) || defined(FEATURE_MMGSDI_UMTS)
#error code not present
#endif /* FEATURE_MMGSDI_GSM || FEATURE_MMGSDI_UMTS */
  else if
    /* If the DF1 was never selected before */
     ((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_NO_SUCH_FILE) ||
#ifdef FEATURE_UIM_UICC
      /* The last selected ADF does not match or need to switch to a DF */
      ((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_ADF) &&
       ((memcmp(new_adf->data,uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf.data,
                uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf.aid_length) != 0) ||
        (new_path[UIM_PATH_DF1_INDEX] != UIM_APP_SELECTION))) ||
#endif /* FEATURE_UIM_UICC */

      /* The last selected DF does not match or need to switch to an ADF */
      ((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_DF) &&
       ((new_path[UIM_PATH_DF1_INDEX] == UIM_NO_FILE_SEL) ||
        (new_path[UIM_PATH_DF1_INDEX] !=uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df))))
  {
    /* The first level DFs do not match, build the path as MF,
       ADF/DF1, DF2 if different from DF1 and EF */
    /* If we have path to be selected as 7FFF, 5Fxx, 8Fxx, 9Fxx.
       We may start the path selection from, MF if the current
       dir pointer is offset. Hence we introduced a check for DF3.
       In that case we perform internal select in two steps -
       step 1 - select 3F00 (MF), 7FFF (DF1), 5Fxx (DF2), 8Fxx (DF3)
       step 2 - select 7FFF (ADF), 5Fxx (DF2). 8Fxx (DF3), 9Fxx (EF)
    */
    /* Select the MF if
       a. We have not selected a first level DF
       b. The current 1st level DF and the 2nd level DF are different or
       c. We are in an EF under a DF
       d. We are in an ADF and have not selected under the ADF yet
       e. We have path length of 4 and DF3 is not selected as yet.
    */
    if (((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_NO_SUCH_FILE) ||
        (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 != UIM_NO_FILE_SEL) ||
        (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel == TRUE)
#ifdef FEATURE_UIM_UICC
        ||(uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_ADF)
#endif /* FEATURE_UIM_UICC */
       ) && (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df3 == UIM_NO_FILE_SEL))
    {
      uim_path_to_be_sel[i++] = UIM_MF_ID;
    }

#ifdef FEATURE_UIM_UICC
    if (new_path[UIM_PATH_DF1_INDEX] == UIM_APP_SELECTION)
    {

      if (new_adf == NULL)
      {
        MSG_ERROR("uim_set_select_path new_adf is NULL returning TRUE",0,0,0);
        return TRUE;
      }

      /* If we are going back to the same app whose session is open, then
         there is no need to SELECT the AID BY DF NAME, instead select by
         path as "7FFF" */
      if (memcmp(new_adf->data,uim_last_sel_dir[uim_cur_channel][uim_drv_slot].curr_app.data,
                 new_adf->aid_length) == 0)
      {
        /* Select the AID by file ID */
        uim_path_to_be_sel[i++] = UIM_SEL_AID_BY_FILE_ID;
      }
      else
      {
        /* Select the AID using DF NAME */
        uim_path_to_be_sel[i++] = UIM_SEL_ADF;
      }
      /* Save the new adf information as this is reqd for selecting the AID
         when DF NAME is used. It is also used for updating the dir structure
         upon selecting the ADF when select by file id is used */
      uim_adf_to_be_sel.aid_length = new_adf->aid_length;
      memcpy(uim_adf_to_be_sel.data, new_adf->data, new_adf->aid_length);
    }
    else
#endif /* FEATURE_UIM_UICC */
    {
      uim_path_to_be_sel[i++] = new_path[UIM_PATH_DF1_INDEX];
    }

    /* In the new path, if there is no DF2, do not include DF2 */
    if (new_path[UIM_PATH_DF2_INDEX] != UIM_NO_FILE_SEL)
    {
      uim_path_to_be_sel[i++] = new_path[UIM_PATH_DF2_INDEX];
      uim_path_to_be_sel[i++] = new_path[UIM_PATH_EF_INDEX];
    }
    else
    {
      /* Include only EF */
      uim_path_to_be_sel[i++] = new_path[UIM_PATH_EF_INDEX];
    }
  }
  /* Check to see if the second level DFs are different */
  else if (new_path[UIM_PATH_DF2_INDEX] != uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2)
  {
    if ( new_path[UIM_PATH_DF2_INDEX] == UIM_NO_FILE_SEL )
    {
      /* We are in a second level DF and this EF has no second level DF,
         so choose the DF1 and EF */
#ifdef FEATURE_UIM_UICC
      if (new_path[UIM_PATH_DF1_INDEX] == UIM_APP_SELECTION)
      {
        if (new_adf == NULL)
        {
          MSG_ERROR("uim_set_select_path  new_adf is NULL returning TRUE",0,0,0);
          return TRUE;
        }
        /* If we are going back to the same app whose session is open, then
           there is no need to SELECT the AID BY DF NAME, instead select by
           path as "7FFF" */
        if (memcmp(new_adf->data, uim_last_sel_dir[uim_cur_channel][uim_drv_slot].curr_app.data,
                   new_adf->aid_length) == 0)
        {
          uim_path_to_be_sel[i++] = UIM_SEL_AID_BY_FILE_ID;
        }
        else
        {
          uim_path_to_be_sel[i++] = UIM_SEL_ADF;
        }
        uim_adf_to_be_sel.aid_length = new_adf->aid_length;
        memcpy(uim_adf_to_be_sel.data, new_adf->data, new_adf->aid_length);
      }
      else
#endif /* FEATURE_UIM_UICC */
      {
        uim_path_to_be_sel[i++] = new_path[UIM_PATH_DF1_INDEX];
      }
      uim_path_to_be_sel[i++] = new_path[UIM_PATH_EF_INDEX];
    }
    else
    {
      /* If we are in an EF under the second level DF, choose the parent
         DF before selecting the required DF */
      if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel == TRUE)
      {
#ifdef FEATURE_UIM_UICC
        if (new_path[UIM_PATH_DF1_INDEX] == UIM_APP_SELECTION)
        {
          if (new_adf == NULL)
          {
            MSG_ERROR("uim_set_select_path new_adf is NULL returning TRUE",0,0,0);
            return TRUE;
          }
          /* If we are going back to the same app whose session is open,then
             there is no need to SELECT the AID BY DF NAME, instead select
             by path as "7FFF" */
          if (memcmp(new_adf->data, uim_last_sel_dir[uim_cur_channel][uim_drv_slot].curr_app.data,
                     new_adf->aid_length) == 0)
          {
            uim_path_to_be_sel[i++] = UIM_SEL_AID_BY_FILE_ID;
          }
          else
          {
            uim_path_to_be_sel[i++] = UIM_SEL_ADF;
          }
          uim_adf_to_be_sel.aid_length = new_adf->aid_length;
          memcpy(uim_adf_to_be_sel.data, new_adf->data, new_adf->aid_length);
        }
        else
#endif /* FEATURE_UIM_UICC */
        {
          uim_path_to_be_sel[i++] = new_path[UIM_PATH_DF1_INDEX];
        }
        uim_path_to_be_sel[i++] = new_path[UIM_PATH_DF2_INDEX];
        uim_path_to_be_sel[i++] = new_path[UIM_PATH_EF_INDEX];
      }
      else
      {
        /* We are in a 2nd level DF and we need to move to another DF in
           the same level */
        uim_path_to_be_sel[i++] = new_path[UIM_PATH_DF2_INDEX];
        uim_path_to_be_sel[i++] = new_path[UIM_PATH_EF_INDEX];
      }
    }
  }
  /* check to see if the path only differs by EF */
  else if (new_path[UIM_PATH_EF_INDEX] != uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef)
  {
    uim_path_to_be_sel[i++] = new_path[UIM_PATH_EF_INDEX];
  }
  else   /* the path is the required path */
  {
    status = TRUE;
  }
  /* If we had DF3 selected with last internal select then we may have EF at
     index 3 */
  if((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df3 ==
     new_path[UIM_PATH_DF3_INDEX]) && (i < UIM_MAX_PATH_ELEMENTS))
  {
    uim_path_to_be_sel[i++] = new_path[UIM_PATH_EF_INDEX + 1];
  }
  else
  {
    /* reset last selected DF3 value */
    uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df3 = UIM_NO_FILE_SEL;
  }

  if (!status)
  {
    if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_ADF)
    {
      MSG_LOW( "ADF selected", 0, 0, 0);
      MSG_LOW( "Last sel DF2,EF %x %x", uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2,
               uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef, 0);
    }
    else
    {
      MSG_LOW( "Last DF1,DF2,EF %x %x %x",
               uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df, uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2,
               uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef);
    }

    MSG_LOW( "New path %x %x %x", new_path[UIM_PATH_DF1_INDEX],
             new_path[UIM_PATH_DF2_INDEX], new_path[UIM_PATH_EF_INDEX]);
  }

  return(status);

}  /* uim_set_select_path() */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_DIRECTORY_CURRENT

DESCRIPTION
  This procedure determines if the directory is current to handle the
  commands that require specific DF to be selected before the processing
  of the command. It also makes a call to fill the path to be selected
  array if an EF is to be accessed, selected or invalidated or rehabilitated.
  The return status is always forced to TRUE when the command is a
  SELECT command.

DEPENDENCIES
  None

RETURN VALUE
  Returns a boolean status.

SIDE EFFECTS
  Fills the path_to_be_sel array with the shortest path for the file/dir.

===========================================================================*/

LOCAL boolean uim_directory_current
(
uim_cmd_type const *cmd_ptr
)
{
  /* The default status is TRUE */
  boolean status = TRUE;
  /* Index for the path array */
  byte i = 0;
  /* Constant pointer for the required path */
  const word *temp_path;
#ifdef FEATURE_UIM_UICC
  /* Temporary storage for AID */
  uim_aid_type temp_uim_aid;
#endif /* FEATURE_UIM_UICC */

  /* Initialize the path to be selected to default values which is 0xFFFF */
  for (i = 0 ; i < UIM_MAX_PATH_ELEMENTS; i++)
  {
    uim_path_to_be_sel[i] = UIM_NO_FILE_SEL;
  }

  /* Reset the index variable */
  i = 0;

  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_directory_current cmd_ptr is NULL",0,0,0);
  }

  switch (cmd_ptr->hdr.command)
  {
    case UIM_ACCESS_F:

      /* get the path for the EF from the table or from the command */
      temp_path = uim_get_file_path ( cmd_ptr->access_uim.item,
                                      cmd_ptr->access_uim.path );

      /* Call the function to set the select path for the EF */
      status = uim_set_select_path( temp_path
#ifdef FEATURE_UIM_UICC
                                    , &(cmd_ptr->access_uim.aid)
#endif /* FEATURE_UIM_UICC */
                                  );

      break;

    case UIM_READ_REC_CMD_F:
      /* get the path for the EF from the table or from the command */
      temp_path = uim_get_file_path ( cmd_ptr->read_rec.item,
                                      cmd_ptr->read_rec.path );

      /* Call the function to set the select path for the EF */
      status = uim_set_select_path( temp_path
#ifdef FEATURE_UIM_UICC
                                    , &(cmd_ptr->read_rec.aid)
#endif /* FEATURE_UIM_UICC */
                                  );

      break;
    case UIM_READ_BIN_CMD_F:
      /* get the path for the EF from the table or from the command */
      temp_path = uim_get_file_path ( cmd_ptr->read_bin.item,
                                      cmd_ptr->read_bin.path );

      /* Call the function to set the select path for the EF */
      status = uim_set_select_path( temp_path
#ifdef FEATURE_UIM_UICC
                                    , &(cmd_ptr->read_bin.aid)
#endif /* FEATURE_UIM_UICC */
                                  );

      break;
    case UIM_WRITE_BIN_CMD_F:
      /* get the path for the EF from the table or from the command */
      temp_path = uim_get_file_path ( cmd_ptr->write_bin.item,
                                      cmd_ptr->write_bin.path );

      /* Call the function to set the select path for the EF */
      status = uim_set_select_path( temp_path
#ifdef FEATURE_UIM_UICC
                                    , &(cmd_ptr->write_bin.aid)
#endif /* FEATURE_UIM_UICC */
                                  );

      break;
    case UIM_WRITE_REC_CMD_F:
      /* get the path for the EF from the table or from the command */
      temp_path = uim_get_file_path ( cmd_ptr->write_rec.item,
                                      cmd_ptr->write_rec.path );

      /* Call the function to set the select path for the EF */
      status = uim_set_select_path( temp_path
#ifdef FEATURE_UIM_UICC
                                    , &(cmd_ptr->write_rec.aid)
#endif /* FEATURE_UIM_UICC */
                                  );

      break;
    case UIM_INC_CMD_F:
      /* get the path for the EF from the table or from the command */
      temp_path = uim_get_file_path ( cmd_ptr->inc.item,
                                      cmd_ptr->inc.path );

      /* Call the function to set the select path for the EF */
      status = uim_set_select_path( temp_path
#ifdef FEATURE_UIM_UICC
                                    , &(cmd_ptr->inc.aid)
#endif /* FEATURE_UIM_UICC */
                                  );

      break;

      /* Whenever there is an external SELECT command for a particular directory
         or a file, the optimal path is filled in the path_to_be_sel array and
         the status is forced to be TRUE, resulting in the external command
         to be exectuted directly using the path that is set by this module.

         The external SELECT commands will result in the actual select of
         that file or directory

         UIM_GET_SEL_RSP_NO_FORCE_SEL command how ever is treated
         similar to a select but, if the item for which the get response
         is requisted is already cached, we need not send down the
         select to the card
         */

    case UIM_CACHED_SEL_F:
    case UIM_SELECT_F:
      /* Check to see if the file type to be selected is an EF  */
      if (cmd_ptr->select.file_type == UIM_EF)
      {
        /* get the path for the EF from the table or from the command */
        temp_path = uim_get_file_path ( cmd_ptr->select.item,
                                        cmd_ptr->select.path );

        /* Call the function to set the select path for the EF and
           do not get the status since an external SELECT is always
           executed directly */
        status = uim_set_select_path( temp_path
#ifdef FEATURE_UIM_UICC
                                      , &(cmd_ptr->select.aid)
#endif /* FEATURE_UIM_UICC */
                                    );

        /* If we are in the correct path, then set_select_path() would not
           set the uim_path_to_be_sel structure, so set the path to include
           only the EF */
        if (status)
        {
          uim_path_to_be_sel[0] = temp_path[ UIM_PATH_EF_INDEX ];
        }

        /* Force the status to be TRUE so that the SELECT command is not
           re-queued and the command be directly issued */
        status = TRUE;
      }
      /* If the MF is requested, include the MF in the select path */
      else if (cmd_ptr->select.file_type == UIM_MF)
      {
        uim_path_to_be_sel[i++] = UIM_MF_ID;
      }
      /* Else if the requested file is a DF */
      else if (cmd_ptr->select.file_type == UIM_DF)
      {
        /* If the directory in the command is all F's, then use the path
           to select the DF */
        if (cmd_ptr->select.dir == UIM_NO_FILE_SEL)
        {
          /* set the path variable to point to the path */
          temp_path = cmd_ptr->select.path;

          /* Select the complete path */
          uim_path_to_be_sel[i++] = UIM_MF_ID;

#ifdef FEATURE_UIM_UICC
          if (temp_path[UIM_PATH_DF1_INDEX] == UIM_APP_SELECTION)
          {
            if (memcmp(cmd_ptr->select.aid.data,
                       uim_last_sel_dir[uim_cur_channel][uim_drv_slot].curr_app.data,
                       cmd_ptr->select.aid.aid_length) == 0)
            {
              uim_path_to_be_sel[i++] = UIM_SEL_AID_BY_FILE_ID;
            }
            else
            {
              uim_path_to_be_sel[i++] = UIM_SEL_ADF;
            }
          }
          else
#endif /* FEATURE_UIM_UICC */
          {
            uim_path_to_be_sel[i++] = temp_path[UIM_PATH_DF1_INDEX];
          }
          uim_path_to_be_sel[i++] = temp_path[UIM_PATH_DF2_INDEX];
        }
        else
        /* Since, only one DF can be sent directly in the command, then we
           assume that if that is valid, then the requested DF is a first
           level DF */
        {
          /* We have to select the MF if we are in a second level DF or if
             we are in an ADF */
          if ((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 != UIM_NO_FILE_SEL)
#ifdef FEATURE_UIM_UICC
              ||(uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type == UIM_ADF)
#endif /* FEATURE_UIM_UICC */
             )
          {
            /* Include the MF only if we cannot get to the DF directly */
            uim_path_to_be_sel[i++] = UIM_MF_ID;
          }
          uim_path_to_be_sel[i++] = cmd_ptr->select.dir;
        }
      }
#ifdef FEATURE_UIM_UICC
      /* If the requested file is an ADF */
      else if (cmd_ptr->select.file_type == UIM_ADF)
      {
        if (memcmp(cmd_ptr->select.aid.data,
                   uim_last_sel_dir[uim_cur_channel][uim_drv_slot].curr_app.data,
                   cmd_ptr->select.aid.aid_length) == 0)
        {
          if (cmd_ptr->select.session == UIM_TERMINATION)
          {
            /* When the command indicated that it is a Termination for AID
               we need to send down the AID instead of 7FFF */
            /* The check for whether such command is allowed or not will be
               done in the actual handling of the command in the
               corresponding file */
               uim_path_to_be_sel[i++] = UIM_SEL_ADF;
          }
          else
          {
            uim_path_to_be_sel[i++] = UIM_SEL_AID_BY_FILE_ID;
          }
        }
        else
        {
          /* We have to select the MF if the last selected DF is a DF or
             if it was a second level DF */
          if (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 != UIM_NO_FILE_SEL)
          {
            /* Include the MF only if we cannot get to the DF directly */
            uim_path_to_be_sel[i++] = UIM_MF_ID;
          }
          uim_path_to_be_sel[i++] = UIM_SEL_ADF;
        }
      }
#endif /* FEATURE_UIM_UICC */

      break;

    case UIM_INVALIDATE_F:

      /* get the path for the EF from the table */
      temp_path = uim_get_file_path ( cmd_ptr->invalidate.item,
                                      cmd_ptr->invalidate.path );

      /* Call the function to set the select path for the EF */
      status =
      uim_set_select_path( temp_path
#ifdef FEATURE_UIM_UICC
                           ,&(cmd_ptr->invalidate.aid )
#endif /* FEATURE_UIM_UICC */
                         );

      break;

    case UIM_REHABILITATE_F:

      /* get the path for the EF from the table */
      temp_path = uim_get_file_path ( cmd_ptr->rehab.item,
                                      cmd_ptr->rehab.path );

      /* Call the function to set the select path for the EF . */
      status =
      uim_set_select_path( temp_path
#ifdef FEATURE_UIM_UICC
                           ,&(cmd_ptr->rehab.aid )
#endif /* FEATURE_UIM_UICC */
                         );
      break;

    case UIM_SEEK_F:

      /* get the path for the EF from the table or from the command */
      temp_path = uim_get_file_path ( cmd_ptr->seek.item,
                                      cmd_ptr->seek.path );

      /* Call the function to set the select path for the EF . */
      status =
      uim_set_select_path( temp_path
#ifdef FEATURE_UIM_UICC
                           ,&(cmd_ptr->seek.aid )
#endif /* FEATURE_UIM_UICC */
                         );
      break;

#ifdef FEATURE_UIM_GSM
#error code not present
#endif /* FEATURE_UIM_GSM */

#ifdef FEATURE_UIM_USIM
#error code not present
#endif /* FEATURE_UIM_USIM */

#ifdef FEATURE_UIM_UICC
    case UIM_VERIFY_CHV_F:
    case UIM_CHANGE_CHV_F:
    case UIM_DISABLE_CHV_F:
    case UIM_ENABLE_CHV_F:
    case UIM_UNBLOCK_CHV_F:

      if (cmd_ptr->hdr.protocol != UIM_WCDMA)
      {
        return(status);
      }
      else
      {
        temp_uim_aid.aid_length = 0;

        switch (cmd_ptr->hdr.command)
        {
          case UIM_VERIFY_CHV_F:
            temp_uim_aid = cmd_ptr->verify_chv.aid;
            break;

          case UIM_CHANGE_CHV_F:
            temp_uim_aid = cmd_ptr->change_chv.aid;
            break;

          case UIM_DISABLE_CHV_F:
            temp_uim_aid = cmd_ptr->disable_chv.aid;
            break;

          case UIM_ENABLE_CHV_F:
            temp_uim_aid = cmd_ptr->enable_chv.aid;
            break;

          case UIM_UNBLOCK_CHV_F:
            temp_uim_aid = cmd_ptr->unblk_chv.aid;
            break;

          default:
            MSG_HIGH ( "Invalid command %d",cmd_ptr->hdr.command,0,0);
            break;
        }

        if ((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type != UIM_ADF) ||
            (memcmp(temp_uim_aid.data, uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf.data,
                    uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.adf.aid_length) != 0))
        {
          /* Build the path for selecting the ADF */
          if ((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 != UIM_NO_FILE_SEL) ||
              uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel )
          {
            /* Include the MF only if we cannot get to the ADF directly */
            uim_path_to_be_sel[i++] = UIM_MF_ID;
          }

          /* Set the path indicating that an ADF needs to be selected */
          if (memcmp(temp_uim_aid.data, uim_last_sel_dir[uim_cur_channel][uim_drv_slot].curr_app.data,
                     uim_last_sel_dir[uim_cur_channel][uim_drv_slot].curr_app.aid_length) == 0)
          {
            uim_path_to_be_sel[i++] = UIM_SEL_AID_BY_FILE_ID;
          }
          else
          {
            uim_path_to_be_sel[i++] = UIM_SEL_ADF;
          }

          /* Set the aid to be selected */
          uim_adf_to_be_sel = temp_uim_aid;

          /* Set the status FLAG to false */
          status = FALSE;
        }
      }
      break;
#endif /* FEATURE_UIM_UICC */

#ifdef FEATURE_UIM_RUIM

    case UIM_STORE_ESN_ME_F:
    case UIM_STORE_ESN_MEID_ME_F:
    case UIM_SSD_UPDATE_F:
    case UIM_CONFIRM_SSD_F:
    case UIM_RUN_CAVE_F:
    case UIM_GENERATE_KEYS_F:
    case UIM_BS_CHAL_F:
    case UIM_MS_KEY_REQ_F:
    case UIM_KEY_GEN_REQ_F:
    case UIM_COMMIT_F:
    case UIM_VALIDATE_F:
    case UIM_CONFIG_REQ_F:
    case UIM_DOWNLOAD_REQ_F:
    case UIM_SSPR_CONFIG_REQ_F:
    case UIM_SSPR_DOWNLOAD_REQ_F:
    case UIM_OTAPA_REQ_F:
    case UIM_COMPUTE_IP_AUTH_F:

      if ((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1_type != UIM_DF) ||
          (uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df1.df != UIM_CDMA_DF))
      {
        /* Build the path for selecting the CDMA DF */
        if ((uim_last_sel_dir[uim_cur_channel][uim_drv_slot].df2 != UIM_NO_FILE_SEL) ||
            uim_last_sel_dir[uim_cur_channel][uim_drv_slot].ef_sel )
        {
          /* Include the MF only if we cannot get to the CDMA DF directly */
          uim_path_to_be_sel[i++] = UIM_MF_ID;
        }

        /* Set the path as CDMA DF */
        uim_path_to_be_sel[i++] = UIM_CDMA_DF;

        status = FALSE;
      }

      break;
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_UIM_UTIL
    case UIM_SELECT_IN_CHAN_F:
      /* If the command is selecting a file (not an ADF), the file path must
      be added.  If the file is selecting an application, the state-machine
      will get the AID to select the application from the command, and no
      path needs to be set here */
      if (cmd_ptr->select_in_chan.file_type != UIM_ADF)
      {
        temp_path = uim_get_file_path ( cmd_ptr->select_in_chan.item,
                                        cmd_ptr->select_in_chan.path);
        uim_path_to_be_sel[0] = temp_path[ UIM_PATH_EF_INDEX ];
      }
      break;
#endif /* FEATURE_UIM_UTIL */

    default:
      break;
  }

  if (!status)
  {
    MSG_LOW( "uim_path %x %x %x", uim_path_to_be_sel[0],
             uim_path_to_be_sel[1], uim_path_to_be_sel[2]);

    if (uim_path_to_be_sel[3] != UIM_NO_FILE_SEL)
    {
      MSG_LOW( " And %x", uim_path_to_be_sel[3], 0, 0);
    }
  }
  return(status);
} /* uim_directory_current */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_COMMAND_RESPONSE

DESCRIPTION
  This procedure processes the response that has been recd from the UIM.

DEPENDENCIES
  None

RETURN VALUE
  status which is of apdu_status_type

SIDE EFFECTS
  None

===========================================================================*/

LOCAL uim_cmd_status_type uim_command_response
(
uim_rsp_buf_type *rsp_ptr,
uim_cmd_type     *cmd
)
{
  uim_cmd_status_type status = UIM_CMD_SUCCESS;

  switch (uim_cmd_mode)
  {
#ifdef FEATURE_UIM_RUIM
    case UIM_CDMA_CMD:
      status = uim_cdma_command_response( rsp_ptr, cmd);
      break;
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_UIM_GSM
#error code not present
#endif /* FEATURE_UIM_GSM */

#ifdef FEATURE_UIM_USIM
#error code not present
#endif /* FEATURE_UIM_USIM */

#ifdef FEATURE_UIM_ISIM
    case UIM_ISIM_CMD:
      status = uim_isim_command_response( rsp_ptr, cmd);
      break;
#endif /* FEATURE_UIM_USIM */

#ifdef FEATURE_UIM_UTIL
    case UIM_UTIL_CMD:
      status = uim_util_command_response( rsp_ptr, cmd);
      break;
#endif /* FEATURE_UIM_UTIL */

#ifdef FEATURE_UIM_MFLO
#error code not present
#endif /* FEATURE_UIM_MFLO */

    case UIM_GENERIC_CMD:
    default:
      status = uim_generic_command_response (rsp_ptr, cmd);
      break;
  }
  
// 没有本段代码的后果：进入UTK二级菜单后不能退到一级菜单
#ifdef FEATURE_UIM_TOOLKIT
#ifdef FEATURE_UTK2
  if (cmd->hdr.command == UIM_TERMINAL_RESPONSE_F &&
      status != UIM_CMD_FETCH)
  {
    set_UTK_session_status(UIM_TK_END_PROACTIVE_SESSION);
  }
#endif  
#endif

  return(status);
} /* uim_command_response */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_COMMAND

DESCRIPTION
  This procedure dispatches the command to the respective module based on the
  type of the command if the path to execute the command is correct. If not,
  this function queues the external command back into the head of the command
  queue and issues an internal select to the required path.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

LOCAL void uim_process_command
(
uim_cmd_type *cmd_ptr,
/* Pointer to received command */
rex_sigs_type *mask
/* rex signals type mask */
)
{
  /* Flag indicating if the directory is valid to execute the command */
  boolean dir_is_current = TRUE;
  /* Flag to indicate if the command processing modules have initiated any
     transactions with the UIM */
  boolean command_transacted = TRUE;

  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_process_command cmd_ptr is NULL",0,0,0);
  }

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  if (cmd_ptr->hdr.slot == UIM_SLOT_1)
  {
    uim_drv_slot = UIM_DRV_SLOT1;
  }
  else if (cmd_ptr->hdr.slot == UIM_SLOT_2)
  {
    uim_drv_slot = UIM_DRV_SLOT2;
  }

  MSG_MED("Processing in slot %d", cmd_ptr->hdr.slot, 0, 0);
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

#ifdef FEATURE_UIM_SESSION_MANAGEMENT
  uim_cur_channel = cmd_ptr->hdr.channel;
#else
  uim_cur_channel = UIM_DEFAULT_CHANNEL;
#endif

  /* Initialize the variables relating to get response and cache here */
  uim_item_sel_rsp_cached       = FALSE;
  select_rsp_cache_item_ptr = NULL;

  /* Call the function to check if the directory that we are currently in is
     valid to process the dequeued command */
  dir_is_current = uim_directory_current(cmd_ptr);

  /* Set the command mode to Generic Command mode as default */
  uim_cmd_mode = UIM_GENERIC_CMD;

  /* Set the command in progress FLAG to TRUE */
  uim_command_in_progress = TRUE;

  /* If the command was not requeued */
  if (!uim_command_requeued)
  {
    /* Let the client know the command is being processed if the client
       provides it's tcb pointer. */
    cmd_ptr->hdr.status = UIM_RECEIVED_S;

    if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
    {
      (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                           cmd_ptr->hdr.cmd_hdr.sigs );
    }
  }

  /* If the current directory is current to process the command, then
     process the command */
  if (dir_is_current)
  {
    /* The directory is current for this command let us let the get response
     * if the command is not an external select.  For an external select,
     * or UIM_GET_SEL_RSP_NO_FORCE_SEL, it is pre-determined whether or
     * not to do a get response based on the status of the cache
     */
    if ( cmd_ptr->hdr.command != UIM_SELECT_F &&
         cmd_ptr->hdr.command != UIM_CACHED_SEL_F )
    {
      uim_item_sel_rsp_cached = FALSE;
    }

    /* Set the requeued flag to FALSE since we are going to process the command,
       if the command is an external command */
    if (!uim_static_buffer_used)
    {
      uim_command_requeued = FALSE;
    }

    /* Get the mode of the command */
    uim_cmd_mode =
    (uim_command_mode_type) UIM_GET_CMD_MODE((int)cmd_ptr->hdr.command);

    switch (uim_cmd_mode)
    {
      /* Call the CDMA module if the command is a CDMA command */
#ifdef FEATURE_UIM_RUIM
      case UIM_CDMA_CMD:
        command_transacted = uim_process_cdma_command( cmd_ptr);
        break;
#endif /* FEATURE_UIM_RUIM */

        /* if the cmd is a GSM command, call the GSM command processing module */
#ifdef FEATURE_UIM_GSM
#error code not present
#endif /* FEATURE_UIM_GSM */

#ifdef FEATURE_UIM_USIM
#error code not present
#endif /* FEATURE_UIM_USIM */

#ifdef FEATURE_UIM_ISIM
      case UIM_ISIM_CMD:
        command_transacted = uim_process_isim_command( cmd_ptr);
        break;
#endif /* FEATURE_UIM_ISIM */

#ifdef FEATURE_UIM_UTIL
      case UIM_UTIL_CMD:
        command_transacted = uim_process_util_command( cmd_ptr);
        break;
#endif /* FEATURE_UIM_UTIL */

      /* Call the MFLO module if the command is a MFLO command */
#ifdef FEATURE_UIM_MFLO
#error code not present
#endif /* FEATURE_UIM_MFLO */

      case UIM_GENERIC_CMD:
      default:
        command_transacted = uim_process_generic_command (cmd_ptr);
        break;
    }  /* end for switch */
  }
  else
  {
    /* Get the protocol from the command for an internal select */
    uim_static_cmd_ptr->hdr.protocol = cmd_ptr->hdr.protocol;

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
    /* Get the slot from the command for an internal select */
    uim_static_cmd_ptr->hdr.slot = cmd_ptr->hdr.slot;

    MSG_MED("Processing in slot %d", cmd_ptr->hdr.slot,0,0);
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

    /* Put the command back in the head of the queue so that
       it can be processed again */
    uim_command_requeued = TRUE;

    /* Keeping track that an internal command is being used */
    uim_internal_command_used = TRUE;

    if ( q_cnt( & uim_cmd_q ) == 0 )
    {
      q_put( & uim_cmd_q, & cmd_ptr->hdr.cmd_hdr.link );
    }
    else
    {
#ifdef FEATURE_Q_NO_SELF_QPTR
      q_insert( & uim_cmd_q,
                & cmd_ptr->hdr.cmd_hdr.link,
                & ((uim_cmd_type*) q_check( & uim_cmd_q ))->hdr.cmd_hdr.link );
#else
      q_insert( & cmd_ptr->hdr.cmd_hdr.link,
                & ((uim_cmd_type*) q_check( & uim_cmd_q ))->hdr.cmd_hdr.link );
#endif /* FEATURE_Q_NO_SELF_QPTR */
    }

    /* Build an internal select command */
    uim_static_buffer_used  = TRUE;

    uim_static_cmd_ptr->hdr.command = UIM_INTERNAL_SELECT_F;
    uim_static_cmd_ptr->hdr.cmd_hdr.task_ptr = &uim_tcb;
    uim_static_cmd_ptr->hdr.cmd_hdr.sigs = NULL;
    uim_static_cmd_ptr->hdr.cmd_hdr.link.next_ptr = NULL;
#ifndef FEATURE_Q_SINGLE_LINK
    uim_static_cmd_ptr->hdr.cmd_hdr.link.prev_ptr = NULL;
#endif
#ifndef FEATURE_Q_NO_SELF_QPTR
    uim_static_cmd_ptr->hdr.cmd_hdr.link.self_ptr = NULL;
    uim_static_cmd_ptr->hdr.cmd_hdr.link.q_ptr = NULL;
#endif
    uim_static_cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
    uim_static_cmd_ptr->hdr.rpt_function = NULL;

    command_transacted = uim_process_generic_command( uim_static_cmd_ptr);
  }

  /* If the return status from the command processing module is FALSE indicating
     that the command was rejected and the command has not been transacted with
     the UIM, it is assumed that an error report has been sent to the task and
     so we can start processing other commands from the command queue

     OR if the command is a PREF SLOT Change command which does not interact
     with the R-UIM card */

  if ((!command_transacted) || (cmd_ptr->hdr.command == UIM_PREF_SLOT_CHANGE_F))
  {

    if (mask != NULL)
    {
      /* include the command Q sig to the mask */
      *mask |= UIM_CMD_Q_SIG;
    }
    else
    {
      ERR_FATAL("uim_process_command mask is NULL",0,0,0);
    }

    /* Clear the flag */
    uim_command_in_progress = FALSE;
  }

  /* Set the command transacted field in the report buffer */
  uim_rpt_buf.cmd_transacted = command_transacted;

} /* uim_process_command */

/* <EJECT> */
#ifndef FEATURE_VIRTUAL_SIM
/*===========================================================================

FUNCTION UIM_GET_UIM_STATUS

DESCRIPTION
  This procedure retrieves the current state of UIM.

DEPENDENCIES
  None

RETURN VALUE
  Returns uim status.

SIDE EFFECTS
  None

===========================================================================*/
uim_status_type uim_get_uim_status( void )
{
  return(uim_status);
}
#endif /* FEATURE_VIRTUAL_SIM */

  #ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_UIM_STATUS_2

DESCRIPTION
  This procedure retrieves the current state of UIM in slot 2.

DEPENDENCIES
  None

RETURN VALUE
  Returns uim status_2.

SIDE EFFECTS
  None

===========================================================================*/
uim_status_type uim_get_uim_status_2( void )
{
  return(uim_status_2);
}

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_SLOT_FOR_APP

DESCRIPTION
  This procedure retrieves the current slot corresponding to the application.

DEPENDENCIES
  None

RETURN VALUE
  Returns the slot type.

SIDE EFFECTS
  None

===========================================================================*/
uim_slot_type uim_get_slot_for_app
(
uim_app_type app
)
{
  uim_slot_type slot = UIM_SLOT_NONE;

  if (app == UIM_CDMA_APP)
  {
    if ((uim_cdma_slot == UIM_SLOT_1) || (uim_cdma_slot == UIM_SLOT_2))
    {
      slot = uim_cdma_slot;
    }
  }
  else if (app == UIM_GSM_APP)
  {
    if ((uim_gsm_slot == UIM_SLOT_1) || (uim_gsm_slot == UIM_SLOT_2))
    {
      slot = uim_gsm_slot;
    }
  }
  else if (app == UIM_TELECOM_APP)
  {
    if ((uim_gsm_slot == UIM_SLOT_1) || (uim_gsm_slot == UIM_SLOT_2))
    {
      slot = uim_telecom_slot;
    }
  }
  return(slot);
} /* uim_get_slot_for_app */
    #if defined (FEATURE_UIM_INACTIVE_SLOT_POWERDOWN)
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_NOTIFY_SYS_MODE

DESCRIPTION
  This procedure notifies the current mode to set the active slot information.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Sets the uim_active_slot information.

===========================================================================*/
void uim_notify_sys_mode
(
sys_sys_mode_e_type sys_mode
)
{

  uim_slot_type active_slot = UIM_SLOT_NONE;

  if (sys_mode == SYS_SYS_MODE_CDMA)
  {
    active_slot = uim_cdma_slot;
  }
  else if (sys_mode == SYS_SYS_MODE_GSM)
  {
    active_slot = uim_gsm_slot;
  }

  if (active_slot == UIM_SLOT_1)
  {
    uim_inactive_slot = UIM_SLOT_2;
  }
  else if (active_slot == UIM_SLOT_2)
  {
    uim_inactive_slot = UIM_SLOT_1;
  }
  else
  {
    uim_inactive_slot = UIM_SLOT_NONE;
  }

  /* Start the powerdown timer for the non active slot - just to be sure */
  (void) rex_set_timer( &uim_powerdown_timer, UIM_POWERDOWN_TIME );
}
    #endif /* FEATURE_UIM_INACTIVE_SLOT_POWERDOWN */

  #endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_CDMA_SVC_TABLE_ITEM

DESCRIPTION
  This function checks availabilities for a specified service.

DEPENDENCIES
  The CDMA service table has to be read first.

RETURN VALUE
  TRUE :   The service is activated.
  FALSE:   The service is unavailable.

SIDE EFFECTS
  None.

===========================================================================*/
uim_svc_table_return_type uim_return_cdma_svc_availabililty (
                                                            uim_cdma_svc_table_enum_type item,  /* The service to be checked*/
                                                            const byte                   *table /* Pointer to the service table */
                                                            )
{
  uim_svc_table_return_type svc;
  svc.activated = FALSE;
  svc.allocated = FALSE;

  switch (item)
  {
    case UIM_CDMA_SVC_CHV_DISABLE:
      /* CHV disable function */
      if (table[0] & UIM_CDMA_SVC_CHV_DISABLE_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[0] & UIM_CDMA_SVC_CHV_DISABLE_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_ADN:
      /* Abbreviated Dialling Numbers (ADN) */
      if (table[0] & UIM_CDMA_SVC_ADN_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[0] & UIM_CDMA_SVC_ADN_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_FDN:
      /* Fixed Dialling Numbers (FDN) */
      if (table[0] & UIM_CDMA_SVC_FDN_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[0] & UIM_CDMA_SVC_FDN_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_SMS:
      /* Short Message Storage (SMS) */
      if (table[0] & UIM_CDMA_SVC_SMS_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[0] & UIM_CDMA_SVC_SMS_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_EXT1:
      /* Extension 1 */
      if (table[2] & UIM_CDMA_SVC_EXT1_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[2] & UIM_CDMA_SVC_EXT1_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_EXT2:
      /* Extension 2 */
      if (table[2] & UIM_CDMA_SVC_EXT2_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[2] & UIM_CDMA_SVC_EXT2_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_SMS_PARAMS:
      /* SMS Parameters */
      if (table[2] & UIM_CDMA_SVC_SMS_PARAMS_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[2] & UIM_CDMA_SVC_SMS_PARAMS_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_LND:
      /* Last Number Dialled (LND) */
      if (table[3] & UIM_CDMA_SVC_LND_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[3] & UIM_CDMA_SVC_LND_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_HOME_SVC_PROVIDER_NAME:
      /* CDMA Home Service Provider Name */
      if (table[4] & UIM_CDMA_SVC_HOME_SVC_PVDR_NAME_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[4] & UIM_CDMA_SVC_HOME_SVC_PVDR_NAME_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_SDN:
      /* Service Dialling Numbers */
      if (table[4] & UIM_CDMA_SVC_SDN_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[4] & UIM_CDMA_SVC_SDN_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_EXT3:
      /* Extension 3 */
      if (table[4] & UIM_CDMA_SVC_EXT3_ALLOCATED_MASK)
      {
        svc.allocated = TRUE;
        if (table[4] & UIM_CDMA_SVC_EXT3_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

#ifdef FEATURE_UIM_TOOLKIT_UTK
    case UIM_CDMA_SVC_SMS_PP_DOWNLOAD:
      /* Check CDMA SMS PP download service */
      if ( table[6] & UIM_CDMA_SVC_SMS_PP_DOWNLOAD_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[6] & UIM_CDMA_SVC_SMS_PP_DOWNLOAD_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_MENU_SEL:
      /* Check menu selection service */
      if ( table[6] & UIM_CDMA_SVC_MENU_SEL_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[6] & UIM_CDMA_SVC_MENU_SEL_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_PROACTIVE_UIM:
      /* Check proactive uim service */
      if ( table[7] & UIM_CDMA_SVC_PROACTIVE_UIM_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[7] & UIM_CDMA_SVC_PROACTIVE_UIM_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;
#endif /* FEATURE_UIM_TOOLKIT_UTK */

#ifdef FEATURE_UIM_SUPPORT_3GPD
    case UIM_CDMA_SVC_3GPD_SIP:
      /* Check 3GPD SIP service */
      if ( table[4] & UIM_CDMA_SVC_3GPD_SIP_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[4] & UIM_CDMA_SVC_3GPD_SIP_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;
#ifdef FEATURE_UIM_3GPD_MOBILE_IP
    case UIM_CDMA_SVC_3GPD_MIP:
      /* Check 3GPD MIP service */
      if ( table[9] & UIM_CDMA_SVC_3GPD_MIP_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[9] & UIM_CDMA_SVC_3GPD_MIP_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;
#endif /* FEATURE_UIM_3GPD_MOBILE_IP */
#endif /* FEATURE_UIM_SUPPORT_3GPD */
#ifdef FEATURE_UIM_SUPPORT_HRPD_AN
    case UIM_CDMA_SVC_HRPD:
      /* Check HRPD service */
      if ( table[1] & UIM_CDMA_SVC_HRPD_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[1] & UIM_CDMA_SVC_HRPD_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;
#endif /* FEATURE_UIM_SUPPORT_HRPD_AN */
#ifdef FEATURE_UIM_SUPPORT_3GPD
    case UIM_CDMA_SVC_3GPD_MSG_EXT:
      /* Check 3GPD / Messaging Extensions service */
      if ( table[3] & UIM_CDMA_SVC_3GPD_MSG_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[3] & UIM_CDMA_SVC_3GPD_MSG_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;
#endif /* FEATURE_UIM_SUPPORT_3GPD */

    case UIM_CDMA_SVC_RC:
      /* Check Root Certificates service */
      if ( table[3] & UIM_CDMA_SVC_RC_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[3] & UIM_CDMA_SVC_RC_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_BROWSER:
      /* Check Browser service */
      if ( table[5] & UIM_CDMA_SVC_BROWSER_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[5] & UIM_CDMA_SVC_BROWSER_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_JAVA:
      /* Check Java service */
      if ( table[5] & UIM_CDMA_SVC_JAVA_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[5] & UIM_CDMA_SVC_JAVA_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_BREW:
      /* Check BREW service */
      if ( table[5] & UIM_CDMA_SVC_BREW_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[5] & UIM_CDMA_SVC_BREW_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_LBS:
      /* Check LBS service */
      if ( table[5] & UIM_CDMA_SVC_LBS_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[5] & UIM_CDMA_SVC_LBS_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_IPV6:
      /* Check IPv6 service */
      if ( table[7] & UIM_CDMA_IPV6_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[7] & UIM_CDMA_IPV6_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    case UIM_CDMA_SVC_BCSMS:
      /* Check BCSMS service */
      if ( table[3] & UIM_CDMA_SVC_BCSMS_ALLOCATED_MASK )
      {
        svc.allocated = TRUE;
        if (table[3] & UIM_CDMA_SVC_BCSMS_ACTIVATED_MASK)
          svc.activated = TRUE;
      }
      break;

    default:
      MSG_MED( "No such service!", 0, 0, 0);
      break;
  } /* end of switch */

  /* Return the result of the query */
  return svc;
}

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_POWER_CONTROL

DESCRIPTION
  This procedure is the voting interface for modules to indicate their pref as
  to whether they would like to keep the UIM powered ON or OFF.

DEPENDENCIES
  Poll time delay to be used.

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM_TOGLLE_STATE_SIG and could change the state of UIM
  as a result.

===========================================================================*/
dword uim_power_control
(
uim_voter_type mask,
boolean uim_reqd
)
{
#if defined ( FEATURE_UIM_TOOLKIT )
  /* New poll interval */
  dword new_uim_poll_time_delay;
#endif /* FEATURE_UIM_TOOLKIT */

  /* Is the vote to on or off */
  if (uim_reqd)
  {
    /* set the bit corresponding to the voter */
    uim_ctl = (uim_voter_type) ((int)uim_ctl | (int)mask );
  }
  else
  {
    /* clear the bit corresponding to the voter */
    uim_ctl = (uim_voter_type) ((int)uim_ctl ^ ((int)uim_ctl & (int)mask ));
  }
  if (uim_ctl != UIM_NONE)
  {
    uim_powerdown = FALSE;
  }
  else
  {
    uim_powerdown = TRUE;
  }

#if defined ( FEATURE_UIM_TOOLKIT )
  /* Check if MC or TC has voted the UIM on.  If so, it because the ME is in the
     traffic channel. */
  if ((int)uim_ctl & UIM_TRAF_CHAN)
  {
#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
    /* 5 seconds interval for traffic channel */
    new_uim_poll_time_delay = UIM_JCDMA_POLL_INTERVAL;
#else
   /* Set the polling timer to be the lower of the proactive duration and the
        in traffic duration. */
    new_uim_poll_time_delay = MIN( uim_proactive_time_delay, UIM_POLL_TIME );
#endif    /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */

  }
  else /* We are in the idle state and not in traffic channel */
  {
    /* Set the polling timer duration to be the proactive timer duration */
    new_uim_poll_time_delay = uim_proactive_time_delay;
  }

  /* Determine if the poll interval has changed */
  if (new_uim_poll_time_delay != uim_poll_time_delay)
  {
    /* Set the poll timer to the new delay */
    uim_set_poll_timer( new_uim_poll_time_delay );

    MSG_MED( "Setting UIM poll timer to new delay", 0, 0, 0);
  }
#endif /* FEATURE_UIM_TOOLKIT */

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  /* If the card present is not toolkit capable, then polling is based on the
     current serving system */
  if (uim_ctl & UIM_CDMA_TRAF_CHAN)
  {
    uim_polling_slot = uim_cdma_slot;
  }
  else if (uim_ctl & UIM_GW_TRAF_CHAN)
  {
    uim_polling_slot = uim_gsm_slot;
  }
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

  (void) rex_set_sigs(&uim_tcb, UIM_STATE_TOGGLE_SIG);
  MSG_MED( "Setting State toggle signal", 0, 0, 0);

  /* Start the polling as soon as we get to the traffic channel */
  if ((int)uim_ctl & UIM_GW_TRAF_CHAN)
  {
    (void) rex_set_sigs(&uim_tcb, UIM_POLL_TIMER_SIG);
    MSG_MED("Setting POLL_TIMER_SIG to kick off the poll in TC", 0, 0, 0);
  }

  return uim_poll_time_delay;
} /* uim_power_control */

  #ifdef FEATURE_UIM_TOOLKIT
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SET_PROACTIVE_UIM

DESCRIPTION
  This procedure is called by UI/any entity that read the CDMA servive table
  to indicate to UIM if the underlying UIM is pro-active or not.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM buffer.
===========================================================================*/
void uim_set_proactive_uim
(
boolean proactive_uim
)
{
  uim_type.proactive_uim = proactive_uim;
  uim_type.init = TRUE;
} /* uim_set_proactive_uim_info */

    #ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SET_PROACTIVE_UIM_2

DESCRIPTION
  This procedure is called by UI/any entity that read the CDMA servive table
  to indicate to UIM if the underlying UIM is pro-active or not.

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  Internally sets the UIM buffer.
===========================================================================*/
void uim_set_proactive_uim_2
(
boolean proactive_uim
)
{
  uim_type_2.proactive_uim = proactive_uim;
  uim_type_2.init = TRUE;
} /* uim_set_proactive_uim_info */
    #endif

  #endif /* FEATURE_UIM_TOOLKIT*/

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_ENUM_VALUE_FOR_PATH

DESCRIPTION
  This procedure takes as input a path that corresponds to an enum type in the
  items table and returns the enum value of the item ( reverse lookup )

DEPENDENCIES
  None

RETURN VALUE
  Enum value corresponding to the input path if a match is found, else error.

SIDE EFFECTS
  None
===========================================================================*/
word uim_get_enum_value_for_path
(
word const *path
)
{
  byte i = 0; /* Loop counter and index*/
  word result = (word)(UIM_NO_SUCH_ITEM); /* Return result */
  word temp_path[UIM_MAX_PATH_ELEMENTS - 1] = {0xFFFF, 0xFFFF, 0xFFFF};

  if (path == NULL)
  {
    MSG_ERROR("uim_get_enum_value_for_path path is NULL ret UIM_NO_SUCH_ITEM",
              0,0,0);
    return result;
  }

  /* If the first element in the path is not an MF, then return an error */
  if (path[i++] != UIM_MF_ID)
  {
    return(result);
  }

  /* Convert the external variable path to an internal format of three words */
  switch ((path[i] & UIM_FILE_ID_LEVEL_MASK) >> UIM_FILE_ID_LEVEL_SHIFT)
  {
    /* If the second word is an EF */
    case UIM_EF_UNDER_MF:
      /* Build the path as MF, NULL DF and EF*/
      temp_path[UIM_PATH_DF1_INDEX] = UIM_MF_ID;
      temp_path[UIM_PATH_EF_INDEX] = path[i];
      break;

    default:
      temp_path[UIM_PATH_DF1_INDEX] = path[i++];
      if (((path[i] & UIM_FILE_ID_LEVEL_MASK) >> UIM_FILE_ID_LEVEL_SHIFT)
          != UIM_EF_UNDER_1ST_DF)
      {
        /* Build the path as DF2 and EF */
        temp_path[UIM_PATH_DF2_INDEX] = path[i++];
        temp_path[UIM_PATH_EF_INDEX] = path[i];
      }
      else
      {
        /* Build the path as NULL DF and EF */
        temp_path[UIM_PATH_EF_INDEX] = path[i];
      }
      break;
  }

  /* Temp path has the internal path required for searching the table */
  switch (temp_path[UIM_PATH_DF1_INDEX])
  {
    case UIM_MF_ID:
      for (i = 0; i < UIM_MAX_EF_UNDER_MF_ITEMS; i++)
      {
        if ((temp_path[UIM_PATH_DF2_INDEX] ==
             uim_ef_under_mf_file_path[i][UIM_PATH_DF2_INDEX]) &&
            (temp_path[UIM_PATH_EF_INDEX] ==
             uim_ef_under_mf_file_path[i][UIM_PATH_EF_INDEX]))
        {
          result = (UIM_EF_UNDER_MF_START << 8) + i;
          break;
        }
      }
      break;

#ifdef FEATURE_UIM_RUIM
    case UIM_CDMA_DF:
      for (i = 0; i < UIM_MAX_RUIM_ITEMS; i++)
      {
        if ((temp_path[UIM_PATH_DF2_INDEX] ==
             uim_ruim_file_path[i][UIM_PATH_DF2_INDEX]) &&
            (temp_path[UIM_PATH_EF_INDEX] ==
             uim_ruim_file_path[i][UIM_PATH_EF_INDEX]))
        {
          result = (UIM_RUIM_EF_START << 8) + i;
          break;
        }
      }
      break;
#endif

#ifdef FEATURE_UIM_GSM
#error code not present
#endif

    case UIM_TELECOM_DF:
      for (i = 0; i < UIM_MAX_TELECOM_ITEMS; i++)
      {
        if ((temp_path[UIM_PATH_DF2_INDEX] ==
             uim_telecom_file_path[i][UIM_PATH_DF2_INDEX]) &&
            (temp_path[UIM_PATH_EF_INDEX] ==
             uim_telecom_file_path[i][UIM_PATH_EF_INDEX]))
        {
          result = (UIM_TELECOM_EF_START << 8) + i;
          break;
        }
      }
      break;

#ifdef FEATURE_UIM_USIM
#error code not present
#endif /* FEATURE_UIM_USIM */

    case UIM_CINGULAR_DF:
      for (i = 0; i < UIM_MAX_CINGULAR_ITEMS; i++)
      {
        if ((temp_path[UIM_PATH_DF2_INDEX] ==
             uim_cingular_file_path[i][UIM_PATH_DF2_INDEX]) &&
            (temp_path[UIM_PATH_EF_INDEX] ==
             uim_cingular_file_path[i][UIM_PATH_EF_INDEX]))
        {
          result = (UIM_CINGULAR_EF_START << 8) + i;
          break;
        }
      }
      break;

    case UIM_HZI_DF:
      for (i = 0; i < UIM_MAX_HZI_ITEMS; i++)
      {
        if ((temp_path[UIM_PATH_DF2_INDEX] ==
           uim_hzi_file_path[i][UIM_PATH_DF2_INDEX]) &&
           (temp_path[UIM_PATH_EF_INDEX] ==
           uim_hzi_file_path[i][UIM_PATH_EF_INDEX]))
        {
          result = (UIM_HZI_EF_START << 8) + i;
          break;
        }
      }
      break;

    default:
      break;
  }
  return(result);
} /* uim_get_enum_value_for_path */

static void uim_validate_and_set_config_params(
  nv_uim_config_params_type *out_uim_config_params,
  nv_uim_config_params_type const  *read_from_nv_uim_config_params,
  nv_stat_enum_type nv_read_status
)
{
  if (nv_read_status != NV_DONE_S)
  {
    MSG_HIGH("Nv Read UIM CONFIGURATION PARAMS failed - Setting defaults",
             0, 0, 0);
    /* Additional delay to receive ATR over and above 40000 clk cycles */
    out_uim_config_params->additional_delay_for_atr = 0;
#ifndef FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS
    /* Number of times an external uim command shall be re-tried over and above the default 2 */
    out_uim_config_params->ext_cmd_additional_re_try_cnt = 0;
    /* Number of parity errors that would be tolerated over and above 20 */
    out_uim_config_params->additional_parity_err_cnt = 0;
    /* Number of rx_break errors that would be tolerated over and above 10 */
    out_uim_config_params->additional_rx_break_err_cnt = 0;
    /* Additional delay when direction of transmission changes from card to ME above 150 micro seconds */
    out_uim_config_params->additional_delay_when_dir_change = 0;
    /* Boolean to indicate whether or not a poll time out would be recovered if in a call */
    out_uim_config_params->uim_recover_poll_t_out_while_in_call = 1;
    /* Whether or not CPHS is read after internal reset if UICC */
    out_uim_config_params->read_usim_cphs = TRUE;
    /* Whether or not UST is read after internal reset if UICC */
    out_uim_config_params->read_usim_ust = TRUE;
    /* Whether or not EST is read after internal reset if UICC */
    out_uim_config_params->read_usim_est = TRUE;
    /* Whether or not NETPAR is read after internal reset if UICC */
    out_uim_config_params->read_usim_netpar = TRUE;
    /* Whether or not RPLMNACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_rplmnact = TRUE;
    /* Whether or not USIM_AD is read after internal reset if UICC */
    out_uim_config_params->read_usim_ad = TRUE;
    /* Whether or not IMSI is read after internal reset if UICC */
    out_uim_config_params->read_usim_imsi = TRUE;
    /* Whether or not USIM_ACC is read after internal reset if UICC */
    out_uim_config_params->read_usim_acc = FALSE;
    /* Whether or not USIM_HPLMN is read after internal reset if UICC */
    out_uim_config_params->read_usim_hplmn = FALSE;
    /* Whether or not HPLMNWACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_hplmnwact = FALSE;
    /* Whether or not PLMNWACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_plmnwact = FALSE;
    /* Whether or not OPLMNWACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_oplmnwact = FALSE;
    /* Whether or not LOCI is read after internal reset if UICC */
    out_uim_config_params->read_usim_loci = FALSE;
    /* Whether or not PSLOCI is read after internal reset if UICC */
    out_uim_config_params->read_usim_psloci = FALSE;
    /* Whether or not KEYS is read after internal reset if UICC */
    out_uim_config_params->read_usim_keys = FALSE;
    /* Whether or not KEYSPS is read after internal reset if UICC */
    out_uim_config_params->read_usim_keysps = FALSE;
    /* Whether or not FPLMN is read after internal reset if UICC */
    out_uim_config_params->read_usim_fplmn = FALSE;
    /* Whether or not START_HFN is read after internal reset if UICC */
    out_uim_config_params->read_usim_start_hfn = FALSE;
    /* Whether or not THRESHOLD is read after internal reset if UICC */
    out_uim_config_params->read_usim_threshold = FALSE;
    /* Whether or not CBMID is read after internal reset if UICC */
    out_uim_config_params->read_usim_cbmid = FALSE;
    /* Whether or not KC is read after internal reset if UICC */
    out_uim_config_params->read_usim_kc = FALSE;
    /* Whether or not KCGPRS is read after internal reset if UICC */
    out_uim_config_params->read_usim_kcgprs = FALSE;
    /* Whether or not ACM is read after internal reset if UICC */
    out_uim_config_params->read_usim_acm = FALSE;
    /* Whether or not ACM_MAX is read after internal reset if UICC */
    out_uim_config_params->read_usim_acm_max = FALSE;
    /* Whether or not EHPLMN is read after internal reset if UICC */
    out_uim_config_params->read_usim_ehplmn = FALSE;


    /* Whether or not CPHS is read after internal reset if GSM */
    out_uim_config_params->read_gsm_cphs = TRUE;
    /* Whether or not SST is read after internal reset if GSM */
    out_uim_config_params->read_gsm_sst = TRUE;
    /* Whether or not HPLMN is read after internal reset if GSM */
    out_uim_config_params->read_gsm_hplmn = TRUE;
    /* Whether or not AD is read after internal reset if GSM */
    out_uim_config_params->read_gsm_ad = TRUE;
    /* Whether or not IMSI is read after internal reset if GSM */
    out_uim_config_params->read_gsm_imsi = TRUE;
    /* Whether or not ACC is read after internal reset if GSM */
    out_uim_config_params->read_gsm_acc = FALSE;
    /* Whether or not LOCI is read after internal reset if GSM */
    out_uim_config_params->read_gsm_loci = FALSE;
    /* Whether or not KC is read after internal reset if GSM */
    out_uim_config_params->read_gsm_kc = FALSE;
    /* Whether or not FPLMN is read after internal reset if GSM */
    out_uim_config_params->read_gsm_fplmn = FALSE;
    /* Whether or not PLMN is read after internal reset if GSM */
    out_uim_config_params->read_gsm_plmn = FALSE;
    /* Whether or not HPLMNACT is read after internal reset if GSM */
    out_uim_config_params->read_gsm_hplmnact = FALSE;
    /* Whether or not PLMNWACT is read after internal reset if GSM */
    out_uim_config_params->read_gsm_plmnwact = FALSE;
    /* Whether or not OPLMNWACT is read after internal reset if GSM */
    out_uim_config_params->read_gsm_oplmnwact = FALSE;
    /* Whether or not LOCIGPRS is read after internal reset if GSM */
    out_uim_config_params->read_gsm_locigprs = FALSE;
    /* Whether or not KCGPRS is read after internal reset if GSM */
    out_uim_config_params->read_gsm_kcgprs = FALSE;
    /* Whether or not CBMID is read after internal reset if GSM */
    out_uim_config_params->read_gsm_cbmid = FALSE;
    /* Whether or not ACM is read after internal reset if GSM */
    out_uim_config_params->read_gsm_acm = FALSE;
    /* Whether or not ACM_MAX is read after internal reset if GSM */
    out_uim_config_params->read_gsm_acm_max = FALSE;
#endif /* FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS */
  }
  else
  {
    /* Additional delay to receive ATR over and above 40000 clk cycles
     * Could go to the max limit of 65536 additional clock cycles
     */
    out_uim_config_params->additional_delay_for_atr =
      read_from_nv_uim_config_params->additional_delay_for_atr;

#ifndef FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS
    /* Number of times an external uim command shall be re-tried over and above the default 2
     * let us limit it to a maximum of 10
     */
    out_uim_config_params->ext_cmd_additional_re_try_cnt =
      ((read_from_nv_uim_config_params->ext_cmd_additional_re_try_cnt > 10)
      ? 10 : read_from_nv_uim_config_params->ext_cmd_additional_re_try_cnt);

    /* Number of parity errors that would be tolerated over and above 20
     * Could go to the max limit of 255
     */
    out_uim_config_params->additional_parity_err_cnt =
      read_from_nv_uim_config_params->additional_parity_err_cnt;

    /* Number of rx_break errors that would be tolerated over and above 10
     * Could go to the max limit of 255
     */
    out_uim_config_params->additional_rx_break_err_cnt =
      read_from_nv_uim_config_params->additional_rx_break_err_cnt;

    /* Additional delay when direction of transmission changes from card to ME above 150 micro seconds
     * Could go to the max limit of 255
     */
    out_uim_config_params->additional_delay_when_dir_change =
      read_from_nv_uim_config_params->additional_delay_when_dir_change;
    /* Add this to the var which uses the delay */
    uim_response_byte_delay +=
      out_uim_config_params->additional_delay_when_dir_change;

    /* Boolean to indicate whether or not a poll time out would be recovered if in a call */
    out_uim_config_params->uim_recover_poll_t_out_while_in_call =
      read_from_nv_uim_config_params->uim_recover_poll_t_out_while_in_call;


    /* Whether or not CPHS is read after internal reset if UICC */
    out_uim_config_params->read_usim_cphs =
      read_from_nv_uim_config_params->read_usim_cphs;
    /* Whether or not UST is read after internal reset if UICC */
    out_uim_config_params->read_usim_ust =
      read_from_nv_uim_config_params->read_usim_ust;
    /* Whether or not EST is read after internal reset if UICC */
    out_uim_config_params->read_usim_est =
      read_from_nv_uim_config_params->read_usim_est;
    /* Whether or not NETPAR is read after internal reset if UICC */
    out_uim_config_params->read_usim_netpar =
      read_from_nv_uim_config_params->read_usim_netpar;
    /* Whether or not RPLMNACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_rplmnact =
      read_from_nv_uim_config_params->read_usim_rplmnact;
    /* Whether or not USIM_AD is read after internal reset if UICC */
    out_uim_config_params->read_usim_ad =
      read_from_nv_uim_config_params->read_usim_ad;
    /* Whether or not IMSI is read after internal reset if UICC */
    out_uim_config_params->read_usim_imsi =
      read_from_nv_uim_config_params->read_usim_imsi;
    /* Whether or not USIM_ACC is read after internal reset if UICC */
    out_uim_config_params->read_usim_acc =
      read_from_nv_uim_config_params->read_usim_acc;
    /* Whether or not USIM_HPLMN is read after internal reset if UICC */
    out_uim_config_params->read_usim_hplmn =
      read_from_nv_uim_config_params->read_usim_hplmn;
    /* Whether or not HPLMNWACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_hplmnwact =
      read_from_nv_uim_config_params->read_usim_hplmnwact;
    /* Whether or not PLMNWACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_plmnwact =
      read_from_nv_uim_config_params->read_usim_plmnwact;
    /* Whether or not OPLMNWACT is read after internal reset if UICC */
    out_uim_config_params->read_usim_oplmnwact =
      read_from_nv_uim_config_params->read_usim_oplmnwact;
    /* Whether or not LOCI is read after internal reset if UICC */
    out_uim_config_params->read_usim_loci =
      read_from_nv_uim_config_params->read_usim_loci;
    /* Whether or not PSLOCI is read after internal reset if UICC */
    out_uim_config_params->read_usim_psloci =
      read_from_nv_uim_config_params->read_usim_psloci;
    /* Whether or not KEYS is read after internal reset if UICC */
    out_uim_config_params->read_usim_keys =
      read_from_nv_uim_config_params->read_usim_keys;
    /* Whether or not KEYSPS is read after internal reset if UICC */
    out_uim_config_params->read_usim_keysps =
      read_from_nv_uim_config_params->read_usim_keysps;
    /* Whether or not FPLMN is read after internal reset if UICC */
    out_uim_config_params->read_usim_fplmn =
      read_from_nv_uim_config_params->read_usim_fplmn;
    /* Whether or not START_HFN is read after internal reset if UICC */
    out_uim_config_params->read_usim_start_hfn =
      read_from_nv_uim_config_params->read_usim_start_hfn;
    /* Whether or not THRESHOLD is read after internal reset if UICC */
    out_uim_config_params->read_usim_threshold =
      read_from_nv_uim_config_params->read_usim_threshold;
    /* Whether or not CBMID is read after internal reset if UICC */
    out_uim_config_params->read_usim_cbmid =
      read_from_nv_uim_config_params->read_usim_cbmid;
    /* Whether or not KC is read after internal reset if UICC */
    out_uim_config_params->read_usim_kc =
      read_from_nv_uim_config_params->read_usim_kc;
    /* Whether or not KCGPRS is read after internal reset if UICC */
    out_uim_config_params->read_usim_kcgprs =
      read_from_nv_uim_config_params->read_usim_kcgprs;
    /* Whether or not ACM is read after internal reset if UICC */
    out_uim_config_params->read_usim_acm =
      read_from_nv_uim_config_params->read_usim_acm;
    /* Whether or not ACM_MAX is read after internal reset if UICC */
    out_uim_config_params->read_usim_acm_max =
      read_from_nv_uim_config_params->read_usim_acm_max;
    /* Whether or not EHPLMN is read after internal reset if UICC */
    out_uim_config_params->read_usim_ehplmn =
      read_from_nv_uim_config_params->read_usim_ehplmn;


    /* Whether or not CPHS is read after internal reset if GSM */
    out_uim_config_params->read_gsm_cphs =
      read_from_nv_uim_config_params->read_gsm_cphs;
    /* Whether or not SST is read after internal reset if GSM */
    out_uim_config_params->read_gsm_sst =
      read_from_nv_uim_config_params->read_gsm_sst;
    /* Whether or not HPLMN is read after internal reset if GSM */
    out_uim_config_params->read_gsm_hplmn =
      read_from_nv_uim_config_params->read_gsm_hplmn;
    /* Whether or not AD is read after internal reset if GSM */
    out_uim_config_params->read_gsm_ad =
      read_from_nv_uim_config_params->read_gsm_ad;
    /* Whether or not IMSI is read after internal reset if GSM */
    out_uim_config_params->read_gsm_imsi =
      read_from_nv_uim_config_params->read_gsm_imsi;
    /* Whether or not ACC is read after internal reset if GSM */
    out_uim_config_params->read_gsm_acc =
      read_from_nv_uim_config_params->read_gsm_acc;
    /* Whether or not LOCI is read after internal reset if GSM */
    out_uim_config_params->read_gsm_loci =
      read_from_nv_uim_config_params->read_gsm_loci;
    /* Whether or not KC is read after internal reset if GSM */
    out_uim_config_params->read_gsm_kc =
      read_from_nv_uim_config_params->read_gsm_kc;
    /* Whether or not FPLMN is read after internal reset if GSM */
    out_uim_config_params->read_gsm_fplmn =
      read_from_nv_uim_config_params->read_gsm_fplmn;
    /* Whether or not PLMN is read after internal reset if GSM */
    out_uim_config_params->read_gsm_plmn =
      read_from_nv_uim_config_params->read_gsm_plmn;
    /* Whether or not HPLMNACT is read after internal reset if GSM */
    out_uim_config_params->read_gsm_hplmnact =
      read_from_nv_uim_config_params->read_gsm_hplmnact;
    /* Whether or not PLMNWACT is read after internal reset if GSM */
    out_uim_config_params->read_gsm_plmnwact =
      read_from_nv_uim_config_params->read_gsm_plmnwact;
    /* Whether or not OPLMNWACT is read after internal reset if GSM */
    out_uim_config_params->read_gsm_oplmnwact =
      read_from_nv_uim_config_params->read_gsm_oplmnwact;
    /* Whether or not LOCIGPRS is read after internal reset if GSM */
    out_uim_config_params->read_gsm_locigprs =
      read_from_nv_uim_config_params->read_gsm_locigprs;
    /* Whether or not KCGPRS is read after internal reset if GSM */
    out_uim_config_params->read_gsm_kcgprs =
      read_from_nv_uim_config_params->read_gsm_kcgprs;
    /* Whether or not CBMID is read after internal reset if GSM */
    out_uim_config_params->read_gsm_cbmid =
      read_from_nv_uim_config_params->read_gsm_cbmid;
    /* Whether or not ACM is read after internal reset if GSM */
    out_uim_config_params->read_gsm_acm =
      read_from_nv_uim_config_params->read_gsm_acm;
    /* Whether or not ACM_MAX is read after internal reset if GSM */
    out_uim_config_params->read_gsm_acm_max =
      read_from_nv_uim_config_params->read_gsm_acm_max;
#endif /* FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS */
  }
}


/*===========================================================================

FUNCTION UIM_NV_INIT

DESCRIPTION
  It reads the NV item UIM_FIRST_INST_CLASS. This function should be called
  once at power-up, preferably in tmc_task_init().

DEPENDENCIES
  NV task has started.

RETURN VALUE
  None.

SIDE EFFECTS
  Saves the read data into the local variable uim_first_inst_class

===========================================================================*/
void uim_nv_init
(
rex_tcb_type  *task_ptr,              /* Calling task's TCB pointer */
rex_sigs_type task_wait_sig,          /* Task signal to wait for when
                                         reading NV */
void          (*task_wait_handler)( rex_sigs_type )
/* Task's wait function to be called
   when reading NV */
)
{
  /* Command buffer to NV */
  static nv_cmd_type  nv_cmd_buf;                 /* NV command buffer */
  static nv_item_type uim_nv_data_buf;            /* NV data buffer */


  if (task_wait_handler == NULL)
  {
    ERR_FATAL("uim_nv_init task_wait_handler is NULL",0,0,0);
  }
  
#ifdef FEATURE_DUAL_UIMCARD
  /* Prepare command buffer to NV. */
  nv_cmd_buf.cmd        = NV_READ_F;                 /* Read request         */
  nv_cmd_buf.tcb_ptr    = task_ptr;                  /* Notify back to me    */
  nv_cmd_buf.sigs       = task_wait_sig;             /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;                      /* No buffer to return  */
  nv_cmd_buf.item       = NV_SIM_SELECT_I;           /* Item to get        */
  nv_cmd_buf.data_ptr   = &uim_nv_data_buf ;         /* Where to return it   */

  /* Clear signal, issue the command, and wait for the response. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( &nv_cmd_buf );

  /* Wait for completion  */
  task_wait_handler( task_wait_sig );

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Check NV read status */
#if defined(FEATURE_VERSION_M8021) || defined(FEATURE_VERSION_MYA450) || defined(FEATURE_VERSION_N68) || defined(FEATURE_VERSION_C11)
	gpio_out(SIM_SEL,(GPIO_ValueType)GPIO_HIGH_VALUE);
#else
#if defined(FEATURE_VERSION_C306)
{
  if (nv_cmd_buf.status == NV_DONE_S)
  {
      if(uim_nv_data_buf.sim_select == 2 )
      {
          gpio_out(SIM_SEL,(GPIO_ValueType)GPIO_LOW_VALUE);
      }
      else
      {
          gpio_out(SIM_SEL,(GPIO_ValueType)GPIO_HIGH_VALUE);
      }
  }
  else
  {
      gpio_out(SIM_SEL,(GPIO_ValueType)GPIO_HIGH_VALUE);
  }
}

#else
{
  if (nv_cmd_buf.status == NV_DONE_S)
  {
      if(uim_nv_data_buf.sim_select == 2 )
      {
          gpio_out(SIM_SEL,(GPIO_ValueType)GPIO_HIGH_VALUE);
      }
      else
      {
          gpio_out(SIM_SEL,(GPIO_ValueType)GPIO_LOW_VALUE);
      }
  }
  else
  {
      gpio_out(SIM_SEL,(GPIO_ValueType)GPIO_LOW_VALUE);
  }
}
#endif
#endif
#else
#if defined(FEATURE_VERSION_S1000T) || defined(FEATURE_VERSION_W515V3)||defined(FEATURE_VERSION_W023)||defined(FEATURE_VERSION_W317A)||defined(FEATURE_VERSION_C337)||defined(FEATURE_VERSION_C316)||defined(FEATURE_VERSION_S600)\
	||defined(FEATURE_VERSION_C117_M74) //xxzhen
	gpio_out(SIM_SEL,(GPIO_ValueType)GPIO_LOW_VALUE);
#else
	gpio_out(SIM_SEL,(GPIO_ValueType)GPIO_HIGH_VALUE);
#endif
#endif

#ifdef FEATURE_UIM_UICC

#ifdef FEATURE_UIM_FORCE_GSM_FIRST_INST_CLASS
#error code not present
#else
  /* Prepare command buffer to NV. */
  nv_cmd_buf.cmd        = NV_READ_F;                 /* Read request         */
  nv_cmd_buf.tcb_ptr    = task_ptr;                  /* Notify back to me    */
  nv_cmd_buf.sigs       = task_wait_sig;             /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;                      /* No buffer to return  */
  nv_cmd_buf.item       = NV_UIM_FIRST_INST_CLASS_I; /* Item to get        */
  nv_cmd_buf.data_ptr   = &uim_nv_data_buf ;         /* Where to return it   */

  /* Clear signal, issue the command, and wait for the response. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( &nv_cmd_buf );

  /* Wait for completion  */
  task_wait_handler( task_wait_sig );

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Check NV read status */
  if (nv_cmd_buf.status != NV_DONE_S)
  {
    MSG_HIGH("Bad NV read status %d", nv_cmd_buf.status, 0, 0 );
    #ifdef FEATURE_UIM_USB_UICC
      uim_first_inst_class = NV_UIM_FIRST_INST_CLASS_USB_UICC;
    #else
      uim_first_inst_class = NV_UIM_FIRST_INST_CLASS_UMTS_SIM;
    #endif /* FEATURE_UIM_USB_UICC */
  }
  else
  {
    /* Check if valid value is read from NV (NV value is always >= 0 hence
       that check is not necessary */
    #ifdef FEATURE_UIM_USB_UICC
      if(uim_nv_data_buf.uim_first_inst_class <= NV_UIM_FIRST_INST_CLASS_USB_UICC_RST_HIGH)
    #else
      if(uim_nv_data_buf.uim_first_inst_class <= NV_UIM_FIRST_INST_CLASS_UMTS_SIM)
    #endif /* FEATURE_UIM_USB_UICC */
      {
        uim_first_inst_class = uim_nv_data_buf.uim_first_inst_class;
      }
      else /* Set default value for instruction class */
      {
        MSG_HIGH("Undefined value for 1st inst class %d, setting to default",
                  uim_nv_data_buf.uim_first_inst_class, 0, 0 );
        #ifdef FEATURE_UIM_USB_UICC
          uim_first_inst_class = NV_UIM_FIRST_INST_CLASS_USB_UICC;
        #else
          uim_first_inst_class = NV_UIM_FIRST_INST_CLASS_UMTS_SIM;
        #endif /* FEATURE_UIM_USB_UICC */
      }
  }
#endif /* FEATURE_UIM_FORCE_GSM_FIRST_INST_CLASS */

#endif /* FEATURE_UIM_UICC */

  /* Get the GCF testing FLAG */
  /* Prepare command buffer to NV. */
  nv_cmd_buf.cmd        = NV_READ_F;             /* Read request         */
  nv_cmd_buf.tcb_ptr    = task_ptr;              /* Notify back to me    */
  nv_cmd_buf.sigs       = task_wait_sig;         /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;                  /* No buffer to return  */
  nv_cmd_buf.item       = NV_GPRS_ANITE_GCF_I;   /* Item to get        */
  nv_cmd_buf.data_ptr   = &uim_nv_data_buf ;     /* Where to return it   */

  /* Clear signal, issue the command, and wait for the response. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( &nv_cmd_buf );

  /* Wait for completion  */
  task_wait_handler( task_wait_sig );

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Check NV read status */
  uim_gcf_testing_flag = (nv_cmd_buf.status == NV_DONE_S) ?
    uim_nv_data_buf.gprs_anite_gcf : FALSE;

  /* Get the ENS enabled FLAG */
  /* Prepare command buffer to NV. */
  nv_cmd_buf.cmd        = NV_READ_F;           /* Read request         */
  nv_cmd_buf.tcb_ptr    = task_ptr;            /* Notify back to me    */
  nv_cmd_buf.sigs       = task_wait_sig;       /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;                /* No buffer to return  */
  nv_cmd_buf.item       = NV_ENS_ENABLED_I;    /* Item to get          */
  nv_cmd_buf.data_ptr   = &uim_nv_data_buf ;   /* Where to return it   */

  /* Clear signal, issue the command, and wait for the response. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( &nv_cmd_buf );

  /* Wait for completion  */
  task_wait_handler( task_wait_sig );

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Check NV read status */
  uim_nv_ens_enabled_flag = (nv_cmd_buf.status == NV_DONE_S) ?
    uim_nv_data_buf.ens_enabled : FALSE;

#ifdef FEATURE_UIM_T_1_SUPPORT
  /* Prepare command buffer to NV. */
  nv_cmd_buf.cmd        = NV_READ_F;               /* Read request         */
  nv_cmd_buf.tcb_ptr    = task_ptr;                /* Notify back to me    */
  nv_cmd_buf.sigs       = task_wait_sig;           /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;                    /* No buffer to return  */
  nv_cmd_buf.item       = NV_UIM_PREF_PROTOCOL_I;  /* Item to get        */
  nv_cmd_buf.data_ptr   = &uim_nv_data_buf ;       /* Where to return it   */

  /* Clear signal, issue the command, and wait for the response. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( &nv_cmd_buf );

  /* Wait for completion  */
  task_wait_handler( task_wait_sig );

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Check NV read status */
  if (nv_cmd_buf.status != NV_DONE_S)
  {
    MSG_HIGH( "Bad NV read status %d", nv_cmd_buf.status, 0, 0 );
    uim_nv_pref_protocol = NV_UIM_PREF_PROTOCOL_T_0;
  }
  else
  {
    uim_nv_pref_protocol = uim_nv_data_buf.uim_pref_protocol;
  }
#endif /* FEATURE_UIM_T_1_SUPPORT */

#ifdef FEATURE_UIM_UART_DM
  /* Prepare command buffer to NV. */
  nv_cmd_buf.cmd        = NV_READ_F;                    /* Read request         */
  nv_cmd_buf.tcb_ptr    = task_ptr;                     /* Notify back to me    */
  nv_cmd_buf.sigs       = task_wait_sig;                /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;                         /* No buffer to return  */
  nv_cmd_buf.item       = NV_UIM_PAR_ERR_WORKAROUND_I;  /* Item to get  */
  nv_cmd_buf.data_ptr   = &uim_nv_data_buf ;            /* Where to return it   */

  /* Clear signal, issue the command, and wait for the response. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( &nv_cmd_buf );

  /* Wait for completion  */
  task_wait_handler( task_wait_sig );

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Check NV read status */
  if (nv_cmd_buf.status != NV_DONE_S)
  {
    /* If the NV read status is a failure, then the SW workaround for
       the GCF 27.11.1.5 testcase doesn't execute. */
    MSG_HIGH( "Bad NV read status %d", nv_cmd_buf.status, 0, 0 );
    uim_par_err_workaround = FALSE;
  }
  else
  {
    uim_par_err_workaround = uim_nv_data_buf.uim_par_err_workaround;
  }
#endif /* FEATURE_UIM_UART_DM */

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  /* Prepare command buffer to NV. */
  nv_cmd_buf.cmd        = NV_READ_F;               /* Read request         */
  nv_cmd_buf.tcb_ptr    = task_ptr;                /* Notify back to me    */
  nv_cmd_buf.sigs       = task_wait_sig;           /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;                    /* No buffer to return  */
  nv_cmd_buf.item       = NV_UIM_CDMA_PREF_SLOT_I; /* Item to get        */
  nv_cmd_buf.data_ptr   = &uim_nv_data_buf ;       /* Where to return it   */

  /* Clear signal, issue the command, and wait for the response. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( &nv_cmd_buf );

  /* Wait for completion  */
  task_wait_handler( task_wait_sig );

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Check NV read status */
  if (nv_cmd_buf.status != NV_DONE_S)
  {
    MSG_HIGH("Bad NV read status %d", nv_cmd_buf.status, 0, 0 );
    uim_pref_cdma_slot = UIM_SLOT_NONE;
  }
  else
  {
    uim_pref_cdma_slot = uim_nv_data_buf.uim_cdma_pref_slot;
  }

  nv_cmd_buf.cmd        = NV_READ_F;              /* Read request         */
  nv_cmd_buf.tcb_ptr    = task_ptr;               /* Notify back to me    */
  nv_cmd_buf.sigs       = task_wait_sig;          /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;                   /* No buffer to return  */
  nv_cmd_buf.item       = NV_UIM_GSM_PREF_SLOT_I; /* Item to get        */
  nv_cmd_buf.data_ptr   = &uim_nv_data_buf ;      /* Where to return it   */

  /* Clear signal, issue the command, and wait for the response. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( &nv_cmd_buf );

  /* Wait for completion  */
  task_wait_handler( task_wait_sig );

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Check NV read status */
  if (nv_cmd_buf.status != NV_DONE_S)
  {
    MSG_HIGH("Bad NV read status %d", nv_cmd_buf.status, 0, 0 );
    uim_pref_gsm_slot = UIM_SLOT_NONE;
  }
  else
  {
    uim_pref_gsm_slot = uim_nv_data_buf.uim_gsm_pref_slot;
  }
#endif

  /* Read the configurable parameters from NV */

  nv_cmd_buf.cmd        = NV_READ_F;              /* Read request         */
  nv_cmd_buf.tcb_ptr    = task_ptr;               /* Notify back to me    */
  nv_cmd_buf.sigs       = task_wait_sig;          /* With this signal     */
  nv_cmd_buf.done_q_ptr = NULL;                   /* No buffer to return  */
  nv_cmd_buf.item       = NV_UIM_CONFIG_PARAMS_I; /* Item to get        */
  nv_cmd_buf.data_ptr   = &uim_nv_data_buf ;      /* Where to return it   */

  /* Clear signal, issue the command, and wait for the response. */
  /* Clear signal for NV  */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  /* Issue the request    */
  nv_cmd( &nv_cmd_buf );

  /* Wait for completion  */
  task_wait_handler( task_wait_sig );

  /* clear the signal again */
  (void) rex_clr_sigs( task_ptr, task_wait_sig );

  uim_validate_and_set_config_params(&uim_config_params,
                                    &(uim_nv_data_buf.uim_config_params),
                                    nv_cmd_buf.status);
  /* Check NV read status */
  MSG_HIGH( "Finished UIM_NV_INIT", 0, 0, 0);
  (void) rex_set_sigs(&uim_tcb, UIM_NV_INIT_DONE_SIG);

}  /* uim_nv_init */

  #ifdef FEATURE_UIM_RUIM
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_STORE_ESN_ME_RESPONSE

DESCRIPTION
  This procedure returns the response to store esn me command.

DEPENDENCIES
  None

RETURN VALUE
  The response to store esn me command.

SIDE EFFECTS
  None.
===========================================================================*/
uim_store_esn_me_rsp_type uim_get_store_esn_me_response (void )
{
  return(uim_store_esn_me_rsp);
}
  #endif /* FEATURE_UIM_RUIM */

/*===========================================================================

FUNCTION UIM_RESET_DIR_FOR_SLOT_CHANNEL

DESCRIPTION
  This procedure resets all the members of the last selected directory
  to their defaults for the respective slot on the respective channel.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the directory structure for the slot specified.

===========================================================================*/
void uim_reset_dir_for_slot_channel (
  uim_drv_slot_type drv_slot,  uim_channel_type channel)
{
  if (drv_slot >= UIM_NUM_DRV_SLOTS)
  {
    MSG_ERROR("Invalid UIMDRV Slot: 0x%x", drv_slot, 0, 0);
    return;
  }

  if (channel >= UIM_MAX_CHANNELS)
  {
    MSG_ERROR("Invalid Channel: 0x%x", channel, 0, 0);
    return;
  }

  /* Initialize the last selected directories to NULL Values */
  uim_last_sel_dir[channel][drv_slot].ef  = UIM_NO_FILE_SEL;
  uim_last_sel_dir[channel][drv_slot].df1.df = UIM_NO_FILE_SEL;
  uim_last_sel_dir[channel][drv_slot].df2 = UIM_NO_FILE_SEL;
  uim_last_sel_dir[channel][drv_slot].df3 = UIM_NO_FILE_SEL;
  uim_last_sel_dir[channel][drv_slot].df1_type = UIM_NO_SUCH_FILE;
  uim_last_sel_dir[channel][drv_slot].ef_sel = FALSE;
#ifdef FEATURE_UIM_UICC
  memset( uim_last_sel_dir[channel][drv_slot].df1.adf.data, '0',
          UIM_AID_MAX_NAME_LENGTH);
  uim_last_sel_dir[channel][drv_slot].df1.adf.aid_length = 0;
  memset( uim_last_sel_dir[channel][drv_slot].curr_app.data, '0',
          UIM_AID_MAX_NAME_LENGTH);
  uim_last_sel_dir[channel][drv_slot].curr_app.aid_length = 0;
#endif /* FEATURE_UIM_UICC */
} /* uim_reset_dir_for_slot_channel */

  #ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_RESET_DIR

DESCRIPTION
  This procedure resets all the members of the last selected directory
  to their defaults.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the directory structure.

===========================================================================*/
void uim_reset_dir ( void )
{
  uim_drv_slot_type drv_slot;
  int i;
#ifdef FEATURE_UIM1
  drv_slot = UIM_DRV_SLOT1;
#else
  drv_slot = UIM_DRV_SLOT2;
#endif

  for(i = 0; i < (int) UIM_MAX_CHANNELS; i++)
  {
    /* Initialize the last selected directories to NULL Values */
    uim_last_sel_dir[i][drv_slot].ef  = UIM_NO_FILE_SEL;
    uim_last_sel_dir[i][drv_slot].df1.df = UIM_NO_FILE_SEL;
    uim_last_sel_dir[i][drv_slot].df2 = UIM_NO_FILE_SEL;
    uim_last_sel_dir[i][drv_slot].df3 = UIM_NO_FILE_SEL;
    uim_last_sel_dir[i][drv_slot].df1_type = UIM_NO_SUCH_FILE;
    uim_last_sel_dir[i][drv_slot].ef_sel = FALSE;
#ifdef FEATURE_UIM_UICC
    memset( uim_last_sel_dir[i][drv_slot].df1.adf.data, '0', UIM_AID_MAX_NAME_LENGTH);
    memset( uim_last_sel_dir[i][drv_slot].curr_app.data, '0', UIM_AID_MAX_NAME_LENGTH);
#endif /* FEATURE_UIM_UICC */
  }
} /* uim_reset_dir () */

    #ifdef FEATURE_UIM_SUPPORT_CSIM
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_RESET_CSIM_DIR

DESCRIPTION
  This procedure resets all the members of the last selected CSIM directory
  to their defaults.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the directory structure.

===========================================================================*/
LOCAL void uim_reset_csim_dir ( void )
{
  uim_drv_slot_type drv_slot;
#ifdef FEATURE_UIM1
  drv_slot = UIM_DRV_SLOT1;
#else
  drv_slot = UIM_DRV_SLOT2;
#endif

  /* Initialize the last selected directories to NULL Values */
  uim_csim_last_sel_dir[drv_slot].ef  = UIM_NO_FILE_SEL;
  uim_csim_last_sel_dir[drv_slot].df1.df = UIM_NO_FILE_SEL;
  uim_csim_last_sel_dir[drv_slot].df2 = UIM_NO_FILE_SEL;
  uim_csim_last_sel_dir[drv_slot].df3 = UIM_NO_FILE_SEL;
  uim_csim_last_sel_dir[drv_slot].df1_type = UIM_NO_SUCH_FILE;
  uim_csim_last_sel_dir[drv_slot].ef_sel = FALSE;
#ifdef FEATURE_UIM_UICC
  memset( uim_csim_last_sel_dir[drv_slot].df1.adf.data, '0',
          UIM_AID_MAX_NAME_LENGTH);
  memset( uim_csim_last_sel_dir[drv_slot].curr_app.data, '0',
          UIM_AID_MAX_NAME_LENGTH);
#endif /* FEATURE_UIM_UICC */

} /* uim_reset_csim_dir () */
    #endif /* FEATURE_UIM_SUPPORT_CSIM */

  #else
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_RESET_DIR_FOR_SLOT

DESCRIPTION
  This procedure resets all the members of the last selected directory
  to their defaults for the respective slot.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Resets the directory structure for the slot specified.

===========================================================================*/
void uim_reset_dir_for_slot ( uim_slot_type slot )
{
  uim_drv_slot_type drv_slot;
  int i;

  if (slot == UIM_SLOT_1)
  {
    drv_slot = UIM_DRV_SLOT1;
  }
  else
  {
    drv_slot = UIM_DRV_SLOT2;
  }

  for(i = 0; i < UIM_MAX_CHANNELS; i++)
  {
    /* Initialize the last selected directories to NULL Values */
    uim_last_sel_dir[i][drv_slot].ef  = UIM_NO_FILE_SEL;
    uim_last_sel_dir[i][drv_slot].df1.df = UIM_NO_FILE_SEL;
    uim_last_sel_dir[i][drv_slot].df2 = UIM_NO_FILE_SEL;
    uim_last_sel_dir[i][drv_slot].df1_type = UIM_NO_SUCH_FILE;
    uim_last_sel_dir[i][drv_slot].ef_sel = FALSE;
#ifdef FEATURE_UIM_UICC
    memset( uim_last_sel_dir[i][drv_slot].df1.adf.data, '0', UIM_AID_MAX_NAME_LENGTH);
    memset( uim_last_sel_dir[i][drv_slot].curr_app.data, '0', UIM_AID_MAX_NAME_LENGTH);
#endif /* FEATURE_UIM_UICC */
  }
} /* uim_reset_dir_for_slot */

  #endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

  #ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_INIT_DUAL_SLOTS

DESCRIPTION
  This procedure resets all the variables that are required for dual slots.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Inits the variables.

===========================================================================*/
void uim_init_dual_slots ( void )
{
  /* Initialize the technologies present for each slot */
  uim_dfs_1.cdma_df_present = FALSE;
  uim_dfs_2.cdma_df_present = FALSE;
  uim_dfs_1.gsm_df_present  = FALSE;
  uim_dfs_2.gsm_df_present  = FALSE;

} /* uim_init_dual_slots () */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SET_UIM_STATUS_FOR_SLOT

DESCRIPTION
  This procedure sets the status of UIM for one slot based on the current slot

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Updates uim_status or uim_status_2
===========================================================================*/
void uim_set_uim_status_for_slot
(
uim_status_type stat,
uim_slot_type slot
)
{
  if (slot == UIM_SLOT_1)
  {
    uim_status = stat;
  }
  else if (slot == UIM_SLOT_2)
  {
    uim_status_2 = stat;
  }
  else
  {
    ERR("Invalid slot type in uim_set_uim_status",0,0,0);
  }

  if ((stat == UIM_ERR_S) || (stat == UIM_POLL_ERR_S))
  {
    uim_notify_error(stat ,slot);
  }
} /* uim_set_uim_status_for_slot () */
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

/* <EJECT> */

/*===========================================================================

FUNCTION UIM_CLEAR_GET_RESPONSE_TABLE

DESCRIPTION
  This procedure clears all the cache in a given table

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void uim_clear_get_response_table(uim_get_rsp_cache_type *table, int32 num_rows)
{
  int i=0;

  if ( table == NULL )
  {
    MSG_ERROR("table pointer is NULL",0,0,0);
    return;
  }

  for (i=0; i<num_rows; i++ )
  {
    /* Set the data size to be zero */
    table[i].data_size = 0;

    /* Indicate that this item is no longer cached */
    table[i].is_cached = FALSE;

    if ( table[i].data != NULL )
    {
      /* If the data pointer is not null, free the data pointer */
      uim_free(table[i].data);

      /* Set the data pointer to NULL to indicate that this is un-allocated */
      table[i].data = NULL;
    }
  }
}
/* <EJECT> */


/*===========================================================================

FUNCTION UIM_CLEAR_RESPONSE_CACHE

DESCRIPTION
  This procedure clears all the cache maintained by uim to cache the
  get response data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  All the cache is cleared.  Which would mean that the selects from now
  on for the first time would do a get response

===========================================================================*/
void uim_clear_get_response_cache( void )
{
  uim_clear_get_response_table( uim_ef_by_path_select_rsp_cache,
                                UIM_MAX_EF_BY_PATH_CACHE_ITEMS);

  uim_clear_get_response_table( uim_ef_under_mf_select_rsp_cache,
                                UIM_MAX_EF_UNDER_MF_ITEMS);

  uim_clear_get_response_table( uim_cingular_file_select_rsp_cache,
                                UIM_MAX_CINGULAR_ITEMS);

  uim_clear_get_response_table( uim_telecom_file_select_rsp_cache,
                                UIM_MAX_TELECOM_ITEMS);

#ifdef FEATURE_UIM_GSM
#error code not present
#endif

#ifdef FEATURE_UIM_USIM
#error code not present
#endif


#ifdef FEATURE_UIM_RUIM
  uim_clear_get_response_table( uim_ruim_file_select_rsp_cache,
                                UIM_MAX_RUIM_ITEMS);
#endif

#ifdef FEATURE_UIM_ISIM
  uim_clear_get_response_table( uim_isim_file_select_rsp_cache,
                                UIM_MAX_ISIM_ITEMS);
#endif

#ifdef FEATURE_UIM_GSM_DCS_1800
#error code not present
#endif

#ifdef FEATURE_UIM_WIM
  uim_clear_get_response_table( uim_wim_file_select_rsp_cache,
                                UIM_MAX_WIM_ITEMS);
#endif

#ifdef FEATURE_UIM_MFLO
#error code not present
#endif
}
/* <EJECT> */



/*===========================================================================

FUNCTION UIM_RESET_UIM

DESCRIPTION
  This procedure builds the static command for power up and issues it. It
  issues different command based on whether the reset is for a wake up or
  a power up.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void uim_reset_uim
(
uim_cmd_type *cmd_ptr,
rex_sigs_type *mask,
boolean me_powerup
)
{
  MSG_HIGH(" Internal command to Reset the UIM ",0,0,0);

  /* Set the count of technical problems to zero */
  uim_inc_tech_prob_cnt = 0;

  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_reset_uim cmd_ptr is NULL",0,0,0);
  }

  /* Indicate that we did not receive the ATR */
  uim_atr_received = FALSE;
  /* Always perform a cold reset */
  if ( uim_clk_freq[uim_drv_slot] )
  {
    uim_power_down();
    rex_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY);
  }
  #if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES ) && defined( FEATURE_UIM_UICC )
  /* reset the voltage_class_known_from_atr varialbe */
  voltage_class_known_from_atr = FALSE;
  #endif
  if (me_powerup)
  {
    /* command to power up the UIM due to task start up */
    cmd_ptr->hdr.command = UIM_INTERNAL_ME_PUP_F;
  }
  else
  {
    /* command to power up the UIM due to power management */
    cmd_ptr->hdr.command = UIM_INTERNAL_WAKE_UP_F;
  }

  /* Clear out all the cache that UIM has been maintaining and start fresh */
  uim_clear_get_response_cache();

  /* Clear the transaction timer just in case it was still switched on*/
  (void) rex_clr_timer( &uim_transaction_rsp_timer );

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

  /* Remove the command q signal from the mask */
  if (mask != NULL)
    *mask &= ~UIM_CMD_Q_SIG;

  uim_static_buffer_used  = TRUE;

  uim_internal_command_used = FALSE;

  cmd_ptr->hdr.cmd_hdr.task_ptr = &uim_tcb;
  cmd_ptr->hdr.cmd_hdr.sigs = NULL;
  cmd_ptr->hdr.cmd_hdr.link.next_ptr = NULL;
#ifndef FEATURE_Q_SINGLE_LINK
  cmd_ptr->hdr.cmd_hdr.link.prev_ptr = NULL;
#endif
#ifndef FEATURE_Q_NO_SELF_QPTR
  cmd_ptr->hdr.cmd_hdr.link.self_ptr = NULL;
  cmd_ptr->hdr.cmd_hdr.link.q_ptr = NULL;
#endif
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
  cmd_ptr->hdr.rpt_function = NULL;
  uim_process_command(cmd_ptr, mask);

} /* uim_reset_uim() */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_POLL

DESCRIPTION
  This procedure builds the Internal Poll command to ping the UIM and issues
  the command.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void uim_poll
(
uim_cmd_type *cmd_ptr,
rex_sigs_type *mask
)
{
#if defined (FEATURE_UIM_USIM) && defined (FEATURE_UIM_GSM)
#error code not present
#endif /* FEATURE_UIM_USIM && FEATURE_UIM_GSM */

  /* Always do the polling on the default channel */
  uim_cur_channel = UIM_DEFAULT_CHANNEL;

  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_poll cmd_ptr is NULL",0,0,0);
  }

  /* Using the status command to ping on the UIM */
  if (mask != NULL)
    *mask &= ~UIM_CMD_Q_SIG;

  uim_static_buffer_used  = TRUE;

  cmd_ptr->hdr.command = UIM_INTERNAL_POLL_F;
  cmd_ptr->hdr.channel = UIM_DEFAULT_CHANNEL;
  cmd_ptr->hdr.cmd_hdr.task_ptr = &uim_tcb;
  cmd_ptr->hdr.cmd_hdr.sigs = NULL;
  cmd_ptr->hdr.cmd_hdr.link.next_ptr = NULL;
#ifndef FEATURE_Q_SINGLE_LINK
  cmd_ptr->hdr.cmd_hdr.link.prev_ptr = NULL;
#endif
#ifndef FEATURE_Q_NO_SELF_QPTR
  cmd_ptr->hdr.cmd_hdr.link.self_ptr = NULL;
  cmd_ptr->hdr.cmd_hdr.link.q_ptr = NULL;
#endif
  cmd_ptr->hdr.cmd_hdr.done_q_ptr = NULL;
  cmd_ptr->hdr.rpt_function = NULL;

  MSG_HIGH("Internal command to Poll the UIM",0,0,0);

#if defined (FEATURE_UIM_USIM) && defined (FEATURE_UIM_GSM)
#error code not present
#endif /* FEATURE_UIM_USIM && FEATURE_UIM_GSM */
  {
    cmd_ptr->hdr.protocol = UIM_GSM;
  }

  /* Clear the flag */
  uim_poll_pending = FALSE;

  /* Process the command */
  uim_process_command(cmd_ptr, mask);

} /* uim_poll() */


/*===========================================================================
FUNCTION  UIM_GSDI_EVENT_CB_FUNC

DESCRIPTION
  GSDI event callback function

DEPENDENCIES
  None

RETURNS
  None

SIDE EFFECTS

===========================================================================*/
LOCAL void uim_gsdi_event_cb_func
(
  gsdi_sim_events_T             event            /* Event ID              */
)
{
  /* Convert the Detailed Error to a Generic Error since UIM
  ** doesn't need to differentiate between the detailed Errors */
  gsdi_convert_detailed_err_evt_to_gen_err(event,
                                           &event);

  switch (event)
  {
  case GSDI_CARD_ERROR:
  case GSDI_SIM_REMOVED:
  case GSDI_SIM_INIT_COMPLETED:
  case GSDI_DRIVER_ERROR:
  case GSDI_MEMORY_WARNING:
  case GSDI_NO_SIM_EVENT:
  case GSDI_NO_SIM:
  case GSDI_SIM_INIT_COMPLETED_NO_PROV:
  case GSDI_REFRESH_FAILED:
  case GSDI_SIM_ILLEGAL:
#ifdef FEATURE_UIM_LOG_TO_EFS
    if(uim_efslog_logging_in_progress)
    {
      uim_efslog_logging_in_progress = FALSE;
#ifndef TEST_FRAMEWORK
      (void) rex_set_sigs( &uim_tcb, UIM_EFSLOG_PURGE_SIG );
#endif/*!TEST_FRAMEWORK*/
    }
#endif /* FEATURE_UIM_LOG_TO_EFS */
    break;

  default:
    /* Ignore event */
    break;
  }
}  /* uim_gsdi_event_cb_func */

/* <EJECT> */
  #if defined (FEATURE_MMGSDI) || defined(FEATURE_UIM_USIM)
/*===========================================================================

FUNCTION UIM_GSDI_LINK_EST

DESCRIPTION
  This procedure sends a link established command to GSDI

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void uim_gsdi_link_est ( void )
{


#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  uim_drv_slot_type drv_slot;

#ifdef FEATURE_UIM1
  drv_slot = UIM_DRV_SLOT1;
#else
  drv_slot = UIM_DRV_SLOT2;
#endif /* FEATURE_UIM1 */
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

#ifdef UIM_DRIVER_TIMESTAMP
  uim_StopDriverTimeStamp();
#endif /* UIM_DRIVER_TIMESTAMP */
  /* Pass the historical characters to GSDI task in the command
      which also indicates that the link has been established and
      this is done only on task start up */
  MSG_LOW("Sending the COMM LINK ESTABLISHED to GSDI", 0, 0, 0 );
  gsdi_cmd_ptr.message_header.message_set = MS_UIM_SERVER;

  gsdi_cmd_ptr.message_header.message_id  =  GSDI_LINK_ESTABLISHED_MSG;

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS

  /* Set the status to TRUE as link est is sent for single slot only when the
     card is present */
  gsdi_cmd_ptr.gsdi_status_slot1 = GSDI_SLOT_PRESENT;

  gsdi_cmd_ptr.gsdi_dfs_present_slot1.cdma_present = uim_dfs_1.cdma_df_present;
  gsdi_cmd_ptr.gsdi_dfs_present_slot1.gsm_present = uim_dfs_1.gsm_df_present;

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
  {
    /* Copy the ATR characters */
    memcpy(gsdi_cmd_ptr.atr_bytes,
           uim_atr_buf[drv_slot].data,
           uim_atr_buf[drv_slot].num_bytes
          );

    /* Set the number of bytes copied */
    gsdi_cmd_ptr.length_atr_bytes = uim_atr_buf[drv_slot].num_bytes;

    /* Copy the historichal characters */
    memcpy(gsdi_cmd_ptr.historical_bytes,
           uim_hist_char_buf[drv_slot].data,
           uim_hist_char_buf[drv_slot].length
          );

    /* Set the number of bytes copied */
    gsdi_cmd_ptr.length_historical_bytes = uim_hist_char_buf[drv_slot].length;
    MSG_HIGH ( "UIM link established with card over legacy ISO interface", 0, 0, 0 );
  }

  /* Set the protocol information for slot 1 */
  if (uim_dfs_1.protocol == UIM_GSM)
  {
    gsdi_cmd_ptr.protocol_slot1 = GSDI_PROTOCOL_GSM;
  }
  else if (uim_dfs_1.protocol == UIM_UICC)
  {
    gsdi_cmd_ptr.protocol_slot1 = GSDI_PROTOCOL_UICC;
  }
  else
  {
    gsdi_cmd_ptr.protocol_slot1 = GSDI_PROTOCOL_NONE;
  }
#else

  /* Set the status for slot 1 */
  if (uim_status == UIM_ERR_S)
  {
    gsdi_cmd_ptr.gsdi_status_slot1 = GSDI_SLOT_ERROR;
  }
  else
  {
    gsdi_cmd_ptr.gsdi_status_slot1 = GSDI_SLOT_PRESENT;
  }

  gsdi_cmd_ptr.gsdi_dfs_present_slot1.cdma_present = uim_dfs_1.cdma_df_present;
  gsdi_cmd_ptr.gsdi_dfs_present_slot1.gsm_present = uim_dfs_1.gsm_df_present;

  /* Set the status for slot 2 */
  if (uim_status_2 == UIM_ERR_S)
  {
    gsdi_cmd_ptr.gsdi_status_slot2 = GSDI_SLOT_ERROR;
  }
  else
  {
    gsdi_cmd_ptr.gsdi_status_slot2 = GSDI_SLOT_PRESENT;
  }

  gsdi_cmd_ptr.gsdi_dfs_present_slot2.cdma_present = uim_dfs_2.cdma_df_present;
  gsdi_cmd_ptr.gsdi_dfs_present_slot2.gsm_present = uim_dfs_2.gsm_df_present;

  /* Copy the ATR characters for slot 1*/
  memcpy( gsdi_cmd_ptr.atr_bytes, uim_atr_buf[UIM_DRV_SLOT1].data,
          uim_atr_buf[UIM_DRV_SLOT1].num_bytes );

  /* Set the number of bytes copied */
  gsdi_cmd_ptr.length_atr_bytes = uim_atr_buf[UIM_DRV_SLOT1].num_bytes;

  /* Copy the historichal characters for slot 1*/
  memcpy( gsdi_cmd_ptr.historical_bytes, uim_hist_char_buf[UIM_DRV_SLOT1].data,
          uim_hist_char_buf[UIM_DRV_SLOT1].length );

  /* Set the number of bytes copied */
  gsdi_cmd_ptr.length_historical_bytes = uim_hist_char_buf[UIM_DRV_SLOT1].length;

  /* Copy the ATR characters for slot 2*/
  memcpy( gsdi_cmd_ptr.atr_2_bytes, uim_atr_buf[UIM_DRV_SLOT2].data,
          uim_atr_buf[UIM_DRV_SLOT2].num_bytes );

  /* Set the number of bytes copied */
  gsdi_cmd_ptr.length_atr_2_bytes = uim_atr_buf[UIM_DRV_SLOT2].num_bytes;

  /* Copy the historichal characters for slot 2*/
  memcpy( gsdi_cmd_ptr.historical_bytes_2, uim_hist_char_buf[UIM_DRV_SLOT2].data,
          uim_hist_char_buf[UIM_DRV_SLOT2].length );

  /* Set the number of bytes copied */
  gsdi_cmd_ptr.length_historical_bytes_2 = uim_hist_char_buf[UIM_DRV_SLOT2].length;

  /* Set the protocol information for slot 1 */
  if (uim_dfs_1.protocol == UIM_GSM)
  {
    gsdi_cmd_ptr.protocol_slot1 = GSDI_PROTOCOL_GSM;
  }
  else if (uim_dfs_1.protocol == UIM_UICC)
  {
    gsdi_cmd_ptr.protocol_slot1 = GSDI_PROTOCOL_UICC;
  }
  else
  {
    gsdi_cmd_ptr.protocol_slot1 = GSDI_PROTOCOL_NONE;
  }

  /* Set the protocol information for slot 2 */
  if (uim_dfs_2.protocol == UIM_GSM)
  {
    gsdi_cmd_ptr.protocol_slot2 = GSDI_PROTOCOL_GSM;
  }
  else if (uim_dfs_2.protocol == UIM_UICC)
  {
    gsdi_cmd_ptr.protocol_slot2 = GSDI_PROTOCOL_UICC;
  }
  else
  {
    gsdi_cmd_ptr.protocol_slot2 = GSDI_PROTOCOL_NONE;
  }
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

#if (defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_USIM)) && \
     !defined (FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#else
  ((IMH_T *)( (void *) &(gsdi_cmd_ptr.message_header)))->message_len_msb =
  (unsigned char)((sizeof (gsdi_cmd_ptr) -
                   sizeof (gsdi_cmdhdr_T)) / 0x100);
  ((IMH_T *)( (void *) &(gsdi_cmd_ptr.message_header)))->message_len_lsb =
  (unsigned char)((sizeof (gsdi_cmd_ptr) -
                   sizeof (gsdi_cmdhdr_T)) % 0x100);
#endif

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  gsdi_cmd_ptr.work_waiting_time = MAX(uim_work_waiting_time[UIM_DRV_SLOT1],
                                       uim_work_waiting_time[UIM_DRV_SLOT2]);
#else
  gsdi_cmd_ptr.work_waiting_time = uim_work_waiting_time[drv_slot];
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

  /* Send the command to GSDI */
  (void)gsdi_cmd_ext(&gsdi_cmd_ptr);
  (void)gsdi_reg_callback (uim_gsdi_event_cb_func);
} /* uim_gsdi_link_est */
  #endif /* defined (FEATURE_MMGSDI) || defined(FEATURE_UIM_USIM) */

  #ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_SLOT

DESCRIPTION
  This procedure arrives at the slot to put the command through. The processing
  is done only for CDMA and GSM commands/accesses. USIM and UTIL commnads are
  not supported at this time for dual slots

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the uim_current_slot parameter
===========================================================================*/
uim_slot_type uim_get_slot
(
uim_cmd_type *cmd
)
{

  uim_command_mode_type mode;   /* Mode for the command */
  byte table_index;             /* Index for the items table */
  uim_items_enum_type item;     /* Item */
  uim_slot_type slot;

  /* Set the default slot to NONE */
  slot = UIM_SLOT_NONE;

  /* Get the command mode from the command */
  mode = (uim_command_mode_type) UIM_GET_CMD_MODE(cmd->hdr.command);

  /* Initialize the item to no item */
  item = UIM_NO_SUCH_ITEM;

  switch (mode)
  {
#ifdef FEATURE_UIM_RUIM
    case UIM_CDMA_CMD:
      slot = uim_cdma_slot;
      break;
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_UIM_GSM
#error code not present
#endif /* FEATURE_UIM_GSM */

    case UIM_GENERIC_CMD:
      {
        switch (cmd->hdr.command)
        {
          /* For all PIN related operations, use the slot information in
             the command */
          case UIM_VERIFY_CHV_F:
          case UIM_CHANGE_CHV_F:
          case UIM_DISABLE_CHV_F:
          case UIM_ENABLE_CHV_F:
          case UIM_UNBLOCK_CHV_F:

            /* Do the same for Status command */
          case UIM_STATUS_F:

            /* Also for invalidate and rehabilitate commands */
          case UIM_INVALIDATE_F:
          case UIM_REHABILITATE_F:

            /* For toolkit commands, use the slot in the cmd */
          case UIM_RESET_F:
          case UIM_TERMINAL_PROFILE_F:
          case UIM_RESET_WITH_TP_F:
          case UIM_TERMINAL_RESPONSE_F:
          case UIM_POLLING_INTERVAL_F:
          case UIM_ENVELOPE_F:

            slot = cmd->hdr.slot;

            break;


          /* For all commands that are related to an EF */
          case UIM_READ_REC_CMD_F:
            item = cmd->read_rec.item;
            break;


          case UIM_READ_BIN_CMD_F:
            item = cmd->read_bin.item;
            break;

          case UIM_WRITE_BIN_CMD_F:
            item = cmd->write_bin.item;
            break;

          case UIM_WRITE_REC_CMD_F:
            item = cmd->write_rec.item;
            break;

          case UIM_INC_CMD_F:
            item = cmd->inc.item;
            break;

          case UIM_ACCESS_F:
            item = cmd->access_uim.item;
            break;

          case UIM_CACHED_SEL_F:
          case UIM_SELECT_F:
            /* If the select is made for an EF, let the internal logic decide on
              slot, else use the slot from the command */
            if (cmd->select.file_type == UIM_EF)
            {
              item = cmd->select.item;
            }
            else
            {
              slot = cmd->hdr.slot;
            }
            break;

          case UIM_SEEK_F:
            item = cmd->seek.item;
            break;

          default:
            /* Do nothing */
            break;
        }

        /* If the command relates to an EF */
        if (item != UIM_NO_SUCH_ITEM)
        {
          /* get the table index and item index for the EF */
          table_index = UIM_GET_TABLE_INDEX(item);

          /* Get the path based on the table index and item index */
          switch (table_index)
          {
            case UIM_EF_UNDER_MF_START:
              if (cmd->hdr.slot == UIM_SLOT_AUTOMATIC )
              {
                slot = uim_gsm_slot;
              }
              else
              {
                slot = cmd->hdr.slot;
              }
              break;

#ifdef FEATURE_UIM_GSM
#error code not present
#endif /* FEATURE_UIM_GSM */

#ifdef FEATURE_UIM_RUIM
            case UIM_RUIM_EF_START:
              /* If it is any of the below files, then use the slot info from
                 the command */
              switch (item)
              {
                case UIM_CDMA_IMSI_M:
                case UIM_CDMA_IMSI_T:
                case UIM_CDMA_CDMA_SVC_TABLE:
                  if (cmd->hdr.slot == UIM_SLOT_AUTOMATIC )
                  {
                    slot = uim_cdma_slot;
                  }
                  else
                  {
                    slot = cmd->hdr.slot;
                  }
                  break;

                default:
                  slot = uim_cdma_slot;
                  break;
              }
              break;
#endif /* FEATURE_UIM_RUIM */

            case UIM_TELECOM_EF_START:
              if (cmd->hdr.slot == UIM_SLOT_AUTOMATIC)
              {
                slot = uim_telecom_slot;
              }
              else
              {
                /* Use the slot from the command */
                slot = cmd->hdr.slot;
              }
              break;

              /* We may have to do something more intelligently for this case */
            case UIM_EF_PATH_START:
              slot = uim_gsm_slot;
              break;

            default:
              MSG_ERROR("Invalid table index in uim_get_slot",0,0,0);
              break;
          } /* Switching on table index */
        } /* Case for EF selection */
      } /* Case for generic command */
      break;

    default:
      break;
  } /* Case for main switch on mode */
  return(slot);
} /* uim_get_slot() */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_SLOT_FOR_CMD

DESCRIPTION
  This procedure arrives at the slot to put the command through. The processing
  is done only for CDMA and GSM commands/accesses. USIM and UTIL commnads are
  not supported at this time for dual slots

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Sets the uim_current_slot parameter
===========================================================================*/
boolean uim_get_slot_for_cmd
(
uim_cmd_type *cmd,
uim_slot_type *slot_param
)
{
  uim_slot_type slot;           /* Temporary Slot */
  boolean status = TRUE;        /* Status to be returned */

#ifdef FEATURE_UIM_SLOT_AUTOMATIC
  slot = uim_get_slot(cmd);
#else
  if (cmd->hdr.slot == UIM_SLOT_AUTOMATIC)
  {
    slot = uim_get_slot(cmd);
  }
  else
  {
    slot = cmd->hdr.slot;
  }
#endif /* FEATURE_UIM_SLOT_AUTOMATIC */

  /* Make sure that the slot is valid */
  if ((slot != UIM_SLOT_1) && (slot != UIM_SLOT_2))
  {
    status = FALSE;
    MSG_HIGH("Invalid slot decision %d",slot,0,0);
  }
  else
  {
    *slot_param = slot;
  }
  /* Return status */
  return(status);
} /* uim_get_slot_for_cmd */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_CLOCK_START

DESCRIPTION
  This procedure checks for the status of the UIMs before starting the clock

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Starts the clock for one or both UIMs
===========================================================================*/
void uim_process_clock_start ( void )
{
  if (uim_status == UIM_CLK_OFF_S)
  {
    uim_stop_power_save (UIM_SLOT_1);

    /* Set the state back to Initialized state */
    uim_set_uim_status_for_slot (UIM_INITIALIZED_S, UIM_SLOT_1);

    MSG_MED ("Clock started for Slot 1",0,0,0);
  }
  if (uim_status_2 == UIM_CLK_OFF_S)
  {
    uim_stop_power_save (UIM_SLOT_2);

    /* Set the state back to Initialized state */
    uim_set_uim_status_for_slot (UIM_INITIALIZED_S, UIM_SLOT_2);

    MSG_MED ("Clock started for Slot 2",0,0,0);
  }
} /* uim_process_clock_start */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_POWER_DOWN_FOR_DUAL_SLOTS

DESCRIPTION
  This procedure powers down both the UIMs

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Powers down both UIMs
===========================================================================*/
void uim_process_power_down_for_dual_slots ( void )
{
  /* Power down UIM1 */
  if ((uim_status != UIM_POWERED_DOWN_S) && (uim_status != UIM_ERR_S))
  {
    uim_reset_dir_for_slot(UIM_SLOT_1);
    uim_power_down(UIM_SLOT_1);
    /* Ensure the link is never used again by setting the status to ERR */
    uim_set_uim_status_for_slot( UIM_ERR_S, UIM_SLOT_1 );

    MSG_MED ("Powering down Slot 1",0,0,0);
  }
  /* Power down UIM2 */
  if ((uim_status_2 != UIM_POWERED_DOWN_S) && (uim_status_2 != UIM_ERR_S))
  {
    uim_reset_dir_for_slot(UIM_SLOT_2);
    uim_power_down(UIM_SLOT_2);
    /* Ensure the link is never used again by setting the status to ERR */
    uim_set_uim_status_for_slot( UIM_ERR_S, UIM_SLOT_2 );

    MSG_MED ("Powering down Slot 2",0,0,0);
  }

#ifdef FEATURE_MULTIMODE_ARCH
  /* Process task stop procedure.*/
  task_stop();
#else
  (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
#endif
} /* uim_process_power_down_for_dual_slots */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_CLOCK_STOP_FOR_DUAL_SLOTS

DESCRIPTION
  This procedure stops the clock for both slots if it in the correct state

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Stops the clock for one or both UIMs
===========================================================================*/
void uim_process_clock_stop_for_dual_slots ( void )
{
  if ((uim_status != UIM_POWERED_DOWN_S) && (uim_status != UIM_ERR_S)
      && (uim_status != UIM_CLK_OFF_S))
  {
    /* Remove only the clock to the UIM */
    if (uim_start_power_save(UIM_SLOT_1))
    {
      /* Change the status if clock stop was successful */
      uim_set_uim_status_for_slot(UIM_CLK_OFF_S,UIM_SLOT_1);
      MSG_MED ("Clock stopped for Slot 1",0,0,0);
    }
  }

  if ((uim_status_2 != UIM_POWERED_DOWN_S) && (uim_status_2 != UIM_ERR_S)
      && (uim_status_2 != UIM_CLK_OFF_S))
  {
    /* Remove only the clock to the UIM */
    if (uim_start_power_save(UIM_SLOT_2))
    {
      /* Change the status if clock stop was successful */
      uim_set_uim_status_for_slot(UIM_CLK_OFF_S, UIM_SLOT_2);
      MSG_MED ("Clock stopped for Slot 2",0,0,0);
    }
  }
} /* uim_process_clock_stop_for_dual_slots */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_ERROR_FOR_DUAL_SLOTS

DESCRIPTION
  This procedure resets the rqd flags and powers down the UIM

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Changes flags
===========================================================================*/
void uim_process_error_for_dual_slots
(
rex_sigs_type *i_mask
)
{
  /* Clear out any votes to keep UIM powered on */
  uim_ctl = UIM_NONE;
  uim_powerdown = TRUE;
  uim_command_in_progress = FALSE;

  /* flush all the commands in the command queue */
  uim_flush_command(UIM_FLUSH_ALL);

  /* Whenver we receive a command, send an error report as the
  SIM is to be powered down */
  *i_mask |= UIM_CMD_Q_SIG;

  /* reset the directory struct and power down to UIM */
  uim_reset_dir_for_slot(UIM_SLOT_1);
  uim_reset_dir_for_slot(UIM_SLOT_2);

  /* Reset the static buffer used flag */
  uim_static_buffer_used = FALSE;

  /* Reset the static command count */
  uim_static_cmd_ptr->hdr.cmd_count = 0;
} /* uim_process_error_for_dual_slots */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SET_CDMA_SLOT

DESCRIPTION
  This procedure sets the slot for CDMA app based on an internal logic

DEPENDENCIES
  The NV items for Preferred slots for application must be valid.

RETURN VALUE
  None

SIDE EFFECTS
  Sets the slots for CDMA application.

===========================================================================*/
uim_slot_type uim_set_cdma_slot
(
void
)
{
  /* Initialize the variables to GSM and CDMA DF presence in slot 1 and 2 */
  boolean cdma_1 = uim_dfs_1.cdma_df_present;
  boolean cdma_2 = uim_dfs_2.cdma_df_present;

  /* Initialize the slot to none */
  uim_cdma_slot = UIM_SLOT_NONE;

  /* If CDMA App is present in both the slots */
  if ((cdma_1) && (cdma_2))
  {
    /* Use the preferred slot */
    if (uim_pref_cdma_slot == UIM_SLOT_NONE)
    {
      uim_cdma_slot = UIM_DEFAULT_SLOT_FOR_CDMA;
    }
    else
    {
      uim_cdma_slot = uim_pref_cdma_slot;
    }
  }
  /* CDMA app is present in only slot 1 */
  else if ((cdma_1) && (!cdma_2))
  {
    uim_cdma_slot = UIM_SLOT_1;
  }
  /* CDMA app is present in only slot 2 */
  else if ((!cdma_1) && (cdma_2))
  {
    uim_cdma_slot = UIM_SLOT_2;
  }
  return(uim_cdma_slot);
} /* uim_set_cdma_slot */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_SET_GSM_SLOT

DESCRIPTION
  This procedure sets the slot for GSM app based on an internal logic

DEPENDENCIES
  The NV items for Preferred slots for CDMA and GSM apps must be valid.

RETURN VALUE
  None

SIDE EFFECTS
  Sets the slots for GSM application.

===========================================================================*/
void uim_set_gsm_slot
(
boolean uim_powerup
)
{
  /* Initialize the variables to GSM and CDMA DF presence in slot 1 and 2 */
  boolean gsm_1 =  uim_dfs_1.gsm_df_present;
  boolean cdma_1 = uim_dfs_1.cdma_df_present;

  boolean gsm_2 =  uim_dfs_2.gsm_df_present;
  boolean cdma_2 = uim_dfs_2.cdma_df_present;

  /* Initialize the slot to none */
  uim_gsm_slot = UIM_SLOT_NONE;

  /* If GSM App is present in both the slots */
  if (gsm_1 && gsm_2)
  {
    /* Use the preferred slot */
    if (uim_pref_gsm_slot == UIM_SLOT_NONE)
    {
      uim_gsm_slot = UIM_DEFAULT_SLOT_FOR_GSM;
    }
    else
    {
      uim_gsm_slot = uim_pref_gsm_slot;
    }
  }
  /* GSM app is present in only slot 1 */
  else if (gsm_1 && !gsm_2)
  {
    uim_gsm_slot = UIM_SLOT_1;
  }
  /* GSM app is present in only slot 2 */
  else if (!gsm_1 && gsm_2)
  {
    uim_gsm_slot = UIM_SLOT_2;
  }

  /* Change the preferred slot */
  if ((uim_powerup) && (uim_pref_gsm_slot == UIM_SLOT_NONE))
  {
    /* Slot 1 has a dual mode R-UIM and slot 2 has a SIM */
    if (gsm_1 && cdma_1 && gsm_2 && !cdma_2)
    {
      uim_gsm_slot = UIM_SLOT_2;
    }
    /* Slot 1 has a SIM and slot 2 has a dual mode R-UIM */
    else if (gsm_1 && !cdma_1 && gsm_2 && cdma_2)
    {
      uim_gsm_slot = UIM_SLOT_1;
    }
  }
} /* uim_set_gsm_slot */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_SLOT_POWERUP

DESCRIPTION
  This procedure performs the power up of second slot

DEPENDENCIES
  This function gets called when at least one slot has been successfully
  initialized or an attempt for second slot is pending

RETURN VALUE
  None

SIDE EFFECTS
  Initiates power up of the second slot if not attempted
===========================================================================*/
void uim_process_slot_powerup
(
rex_sigs_type *i_mask,   /* Mask of signals interested in */
uim_cmd_type  *cmd_ptr
)
{
  /* Reset the static command count */
  cmd_ptr->hdr.cmd_count = 1;

  /* reset the static buffer used flag */
  uim_static_buffer_used = FALSE;

  /* Check to see if the second slot needs to be initialized */
  if ((cmd_ptr->hdr.slot == UIM_SLOT_1) && (uim_status_2 == UIM_UNINITIALIZED_S))
  {
    /* Change the slot */
    cmd_ptr->hdr.slot = UIM_SLOT_2;

    /* Reset the UIM for second slot */
    uim_reset_uim( cmd_ptr, i_mask, uim_me_powerup);
  }
  else /* Power up of both slots have been attempted */
  {
    /* At least one slot is init successfully */
    if (uim_me_powerup)
    {
      /* Set the slots for the apps */
      (void) uim_set_cdma_slot();
      uim_set_gsm_slot(TRUE);

      if (((uim_cdma_slot == UIM_SLOT_1) && (uim_dfs_1.cdma_df_present)) ||
          ((uim_cdma_slot == UIM_SLOT_2) && (uim_dfs_2.cdma_df_present)))
      {
        nvruim_dir_present |= NVRUIM_CDMA_DF_PRESENT;
      }

#if defined(FEATURE_MMGSDI) || defined(FEATURE_UIM_USIM)
      /* Send a link established messgae to GSDI */
      uim_gsdi_link_est();
#endif /* MMGSDI or USIM */

      /* Reset the flag */
      uim_me_powerup = FALSE;

      /* Trigger the first poll */
      uim_set_poll_timer( uim_poll_time_delay  );

    } /* Status is initialized */
    else
    {
      /* If we run into the situation where a slot is reset because of timeout
         when the other slot status is initialized, then set the command in
         progress flag to FALSE */
      uim_command_in_progress = FALSE;
    }

    /* Ready to process commands */
    *i_mask |= UIM_CMD_Q_SIG;

  } /* power up of second slot */
} /* uim_process_slot_powerup */
  #endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

/* <EJECT> */
  #ifdef FEATURE_UIM_TOOLKIT
/*===========================================================================

FUNCTION UIM_FETCH

DESCRIPTION
  This procedure builds the Internal Fetch command to retrieve a proactive
  command from the UIM.

DEPENDENCIES
  This function must be called from the UIM task after processing of a
  command is completed successfully.  The command queue mask must be
  disabled when this function is called as it sends an internal command
  to the UIM.

RETURN VALUE
  None

SIDE EFFECTS
  A Fetch command is sent to the UIM while other commands may still be in
  the command queue.

===========================================================================*/
LOCAL void uim_fetch
(
uim_cmd_type *cmd_ptr,
rex_sigs_type *mask
)
{
  /* Send a FETCH command to the SIM */
  /* Get the protocol from the command for an internal select */
//         uim_static_cmd_ptr->hdr.protocol = cmd_ptr->hdr.protocol;

  if (cmd_ptr == NULL)
  {
    ERR_FATAL("uim_fetch cmd_ptr is NULL",0,0,0);
  }

  /* Build an internal select command */
  uim_static_buffer_used  = TRUE;

  cmd_ptr->fetch.hdr.command = UIM_INTERNAL_FETCH_F;
  cmd_ptr->fetch.hdr.cmd_hdr.task_ptr = &uim_tcb;
  cmd_ptr->fetch.hdr.cmd_hdr.sigs = NULL;
  cmd_ptr->fetch.hdr.cmd_hdr.link.next_ptr = NULL;
#ifndef FEATURE_Q_SINGLE_LINK
  cmd_ptr->fetch.hdr.cmd_hdr.link.prev_ptr = NULL;
#endif
#ifndef FEATURE_Q_NO_SELF_QPTR
  cmd_ptr->fetch.hdr.cmd_hdr.link.self_ptr = NULL;
  cmd_ptr->fetch.hdr.cmd_hdr.link.q_ptr = NULL;
#endif
  cmd_ptr->fetch.hdr.cmd_hdr.done_q_ptr = NULL;
  cmd_ptr->fetch.hdr.rpt_function = NULL;

  /* Get the command size from the second status word. */
  cmd_ptr->fetch.command_size = uim_rsp_buf.sw2;

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  if (cmd_ptr->hdr.slot == UIM_SLOT_1)
  {
    cmd_ptr->hdr.protocol = uim_dfs_1.protocol;
  }
  else
  {
    cmd_ptr->hdr.protocoll = uim_dfs_2.protocol;
  }
#else
  cmd_ptr->hdr.protocol = uim_dfs_1.protocol;
#endif


  uim_process_command( cmd_ptr, mask );
} /* uim_fetch */
  #endif /* FEATURE_UIM_TOOLKIT */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_INIT

DESCRIPTION
  This procedure initializes the queues and timers for UIM Task.It should be
  called only once, at powerup time. It also initializes other UIM related
  variables.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void uim_init( void )
{
  byte i; /* Index into free buffer pool */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /* -----------------
  ** Initialize timers
  ** ----------------- */
  rex_def_timer( &uim_rpt_timer, &uim_tcb, UIM_RPT_TIMER_SIG );
  rex_def_timer_ex( &uim_cmd_rsp_timer, uim_cmd_rsp_timer_expiry_cb, NULL);
  rex_def_timer( &uim_delay_timer, &uim_tcb, UIM_CMD_RSP_SIG );
  rex_def_timer( &uim_transaction_rsp_timer, &uim_tcb, UIM_TRANSACTION_SIG );

#if defined(FEATURE_USE_TIME_VU)
  /* We want the poll timer to be on even-if the target is in sleep.. so
   * put it into the appropriate group
   */
  timer_def( &uim_wake_up_poll_timer, &uim_always_on_timer_group, &uim_tcb, UIM_POLL_TIMER_SIG, NULL, 0 );
#endif
  rex_def_timer( &uim_no_wake_up_poll_timer, &uim_tcb, UIM_POLL_TIMER_SIG );

#ifdef FEATURE_UIM_CARD_ERR_UIM_LDO_PUP
#error code not present
#endif /* FEATURE_UIM_CARD_ERR_UIM_LDO_PUP */

  /* -----------------
  ** Initialize queues
  ** ----------------- */

  (void) q_init( &uim_cmd_q );
  (void) q_init( &uim_free_q );

  /* Fill uim_free_q */
  for (i = 0; i < UIM_NUM_CMD_BUFS; i++)
  {
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
    /* Init the buffer with SLOT AUTOMATIC */
    uim_cmd_pool[i].hdr.slot = UIM_SLOT_AUTOMATIC;
#endif
    uim_cmd_pool[i].hdr.cmd_hdr.done_q_ptr = &uim_free_q;
    q_put( &uim_free_q, q_link( &uim_cmd_pool[i],
                                &uim_cmd_pool[i].hdr.cmd_hdr.link ) );
  }

  /* -------------------
  ** Initialize pointers
  ** ------------------- */

  uim_static_cmd_ptr = &uim_static_cmd_buf;
  uim_rsp_buf_ptr = &uim_rsp_buf;

  /* --------------------
  ** Initialize variables
  ** -------------------- */

  uim_me_powerup = TRUE;
  uim_task_startup = TRUE;

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  uim_reset_dir();
#ifdef FEATURE_UIM_SUPPORT_CSIM
  uim_reset_csim_dir();
#endif /* FEATURE_UIM_SUPPORT_CSIM*/
#else
  uim_reset_dir_for_slot(UIM_SLOT_1);
  uim_reset_dir_for_slot(UIM_SLOT_2);
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

  uim_dfs_1.cdma_df_present = FALSE;
  uim_dfs_1.gsm_df_present = FALSE;
  uim_dfs_1.protocol = UIM_NO_SUCH_MODE;

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  uim_dfs_2.cdma_df_present = FALSE;
  uim_dfs_2.gsm_df_present = FALSE;
  uim_dfs_2.protocol = UIM_NO_SUCH_MODE;
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

#ifdef FEATURE_UIM1
  uim_WI[UIM_DRV_SLOT1] = 10;
#endif
#ifdef FEATURE_UIM2
  uim_WI[UIM_DRV_SLOT2] = 10;
#endif

#ifdef PLATFORM_LTK
#error code not present
#endif /* PLATFORM_LTK */

#ifdef TEST_FRAMEWORK
  uim_first_inst_class = NV_UIM_FIRST_INST_CLASS_UMTS_SIM;
#endif /*TEST_FRAMEWORK*/

  /*-------------------------------------------------------------------------
  If using the new SLEEP API, Register and get handle for sleep task for sleep
  voting
  -------------------------------------------------------------------------*/
#if defined(FEATURE_NEW_SLEEP_API) && defined(FEATURE_UIM_USES_NEW_SLEEP_API)
  uim_sleep_okts_handle = sleep_register("UIM", TRUE);
#ifdef CLK_REGIME_UP_SLEEP__MCLK_HCLK 
  sleep_set_hclk_restriction(uim_sleep_okts_handle, CLKRGM_HCLK_FULL);
  sleep_set_mclk_restriction(uim_sleep_okts_handle, CLKRGM_MCLK_ON);
#endif /* CLK_REGIME_UP_SLEEP__MCLK_HCLK */
#endif /* FEATURE_NEW_SLEEP_API && FEATURE_UIM_USES_NEW_SLEEP_API */
} /* uim_init() */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_GET_UART_STATUS()

DESCRIPTION
  This procedure safely calls the UIM_UART_STATUS() Macro by first checking
  to make sure that the power and clock for the appropriate UIM is still on.
  If it isn't, it'll return 0 instead.  It always checks for the slot currently
  in use.

DEPENDENCIES
  None

RETURN VALUE
  Either Read Status or 0.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL dword uim_get_uart_status( void )
{
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

  if(UIM_IS_UIM_CLK_ON())
  {
   /* Before checking UART status, ensure that Power and clock
      are still on */
    return uim_read_status();
  }
  else
  {
    /* Otherwise, we can't reliably check the status, so just report 0 */
    return 0;
  }
} /* uim_get_uart_status() */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_PROCESS_CMD_ERR                                         INTERNAL

DESCRIPTION
  This procedure processes the error response received from the UIM card

DEPENDENCIES
  This function uses global variables defined for the UIM server to determine
  the proper processing of the card response.

RETURN VALUE
  cmd_ptr value.

SIDE EFFECTS
  The control variables of the UIM server are adjusted to reflect the change
  in the operation of the UIM server as a result of the processing on the
  response.

===========================================================================*/

LOCAL void uim_process_cmd_err
(
uim_cmd_type  *cmd_ptr, /* pointer to command */
rex_sigs_type *i_mask   /* Mask of signals interested in */
)
{
  uim_status_type  notification_reason = UIM_ERR_S;

  if (i_mask == NULL)
  {
    ERR_FATAL("uim_process_cmd_err i_mask  is NULL",0,0,0);
  }

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  /* Set the uim status to UIM_ERR_S when
     sim reports technical problem */
  if( uim_nv_ens_enabled_flag &&
      uim_inc_tech_prob_cnt >= UIM_MAX_REPEATED_TECH_PROBLEMS &&
      uim_mode != UIM_PASSIVE_MODE )
  {
    /* Clear out any votes to keep UIM powered on */
    uim_ctl = UIM_NONE;
    uim_powerdown = TRUE;
    uim_command_in_progress = FALSE;

    /* flush all the commands in the command queue */
    uim_flush_command(UIM_FLUSH_ALL);

    /* set the status to ERROR */
    uim_status = UIM_ERR_S;

    return;
  } 
#endif /* ! FEATURE_UIM_SUPPORT_DUAL_SLOTS */

  if ((uim_static_buffer_used) &&
      (uim_static_cmd_ptr->hdr.command == UIM_INTERNAL_FETCH_F))
  {
    MSG_HIGH("Ignore failure on an internal fetch", 0,0,0);
    //reset all the flags and include the imask
    *i_mask |= UIM_CMD_Q_SIG;
    /* Set the flag to false since the command is completed */
    uim_command_in_progress = FALSE;
    /* Reset the internal command used flag */
    uim_internal_command_used = FALSE;
    /* Reset the static_buffer */
    uim_static_buffer_used = FALSE;
    /* Reset the command requeued flag */
    uim_command_requeued = FALSE;
    return;

  }
  /* If the result is for an external command */
  if ((uim_internal_command_used) || (!uim_static_buffer_used))
  {
    if (cmd_ptr != NULL)
    {
      /* Avoid multiple attempts for ECC and authenticate as well as
         RUN_GSM_ALGO.  Also do not re-try while in passive mode */
      if (cmd_ptr->access_uim.item == UIM_GSM_ECC ||
          ( !uim_internal_command_used &&
            ( ( cmd_ptr->hdr.command == UIM_AUTHENTICATE_F ) ||
              ( cmd_ptr->hdr.command == UIM_AUTHENTICATE_ODD_INS_F ) ||
              ( cmd_ptr->hdr.command == UIM_RUN_GSM_ALGO_F )
            )
          ) ||
          uim_mode == UIM_PASSIVE_MODE)
      {
        /* Any error in passive mode goes directly to the client
         * without any re-tries
         */
        cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_CMD_ATTEMPTS;
      }
      /* If we have exceeded tbe max number of attempts */
      if (++cmd_ptr->hdr.cmd_count > UIM_MAX_NUM_CMD_ATTEMPTS)
      {
        /* include the command Q sig to the mask as we are going
           to process the command again */
        *i_mask |= UIM_CMD_Q_SIG;

        if (uim_internal_command_used)
        {
          /* flush the external command */
          uim_flush_command(UIM_FLUSH_ONLY);
        }
        else
        {
          /* send an error report */
          uim_send_err_rpt(cmd_ptr);
        }

        /* reset the static buffer used flag */
        uim_static_buffer_used = FALSE;

        /* reset the internal command used flag */
        uim_internal_command_used = FALSE;
      }
      else
      {
        /* Makes sure that we do not send the status again to
           the task that sent the command by setting the task pointer
           to NULL */
        cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;

        if (!uim_command_requeued)
        {
          /* Call the command processing function directly to retry
          the command */
          uim_process_command(cmd_ptr, i_mask);
        }
        else
        {
         /* Since the command was re-queued, include the command Q sig
             to the mask to process the command again */
          *i_mask |= UIM_CMD_Q_SIG;
        }
      }
    } /* cmd_Ptr !=NULL */
    else
    {
      MSG_HIGH("Command Pointer is Null",0,0,0);
      /* Log as much information as possible for postmortem analysis */

     (void)uim_log_timeout_info(cmd_ptr, uim_static_cmd_ptr, *i_mask,
                                            uim_get_uart_status());
      uim_reset_uim (uim_static_cmd_ptr, i_mask, uim_me_powerup);
    }
  }
  /* if it was an internal command */
  else
  {
    notification_reason = UIM_INT_CMD_ERR_EXCEED_MAX_ATTEMPTS_S;
#if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES )
    /* Determine if the ATR was corrupted */
    if (!uim_atr_received)
    {
      /* Check if the ATR was processed enough */
      if (uim_static_cmd_ptr->hdr.cmd_count == UIM_MAX_NUM_ATTEMPTS)
      {
        /* Check if there is a higher voltage class */
        if (UIM_MAX_VOLTAGE != uim_current_voltage_class[uim_drv_slot])
        {
          /* Power down the interface */
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
          /* Power down the interface */
          uim_power_down();
#else
          uim_power_down(uim_static_cmd_ptr->hdr.slot);
          MSG_MED ("Powering down Slot %d",uim_static_cmd_ptr->hdr.slot,0,0);
#endif

#ifdef FEATURE_UIM_UICC
          /* We will try the next voltage class only if the first_inst_class
          is GSM or we do not have the voltage class information from
          the ATR and we have a higher voltage available*/
          if ( ( uim_first_inst_class == NV_UIM_FIRST_INST_CLASS_GSM_SIM ) ||
               ( voltage_class_known_from_atr == FALSE )
             )
          {
            rex_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY);
            /* Try the next voltage class */
            uim_current_voltage_class[uim_drv_slot]++;
            /* Reset the error count */
            uim_static_cmd_ptr->hdr.cmd_count = 0;
          }
#else
          rex_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY);
          /* Try the next voltage class */
          uim_current_voltage_class[uim_drv_slot]++;
          /* We will try the next voltage class if UICC is not defined
          and we have a higher voltage available*/
          uim_static_cmd_ptr->hdr.cmd_count = 0;
#endif
        } /* end if - there are no more voltages to try */
        else
        {
          notification_reason = UIM_CORRUPT_ATR_RCVD_MAX_TIMES_S;
        }
      } /* end if - the ATR was processed enough */
#ifdef FEATURE_UIM_UICC
      else
      {
        /* Let us power down the interface if the first instruction
           class is set to USIM */
        if ((uim_first_inst_class == NV_UIM_FIRST_INST_CLASS_UMTS_SIM)
         #ifdef FEATURE_UIM_USB_UICC
#error code not present
         #endif /* FEATURE_UIM_USB_UICC */
           )
        {
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
          uim_power_down();
#else
          uim_power_down(uim_static_cmd_ptr->hdr.slot);
          MSG_MED ("Powering down Slot %d",uim_static_cmd_ptr->hdr.slot,0,0);
#endif
        }
      } /* else for ATR was processed enough */
#endif
    } /* end if - the ATR was corrupted */
#endif /* FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES */

    /* Any error in passive mode for an internal command goes
     * directly to the client without re-tries
     */
    if ( uim_mode == UIM_PASSIVE_MODE )
    {
      /* could be a polling error or any other internal command
       * error
       */
      uim_static_cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
      notification_reason               = UIM_INT_CMD_ERR_IN_PASSIVE_MODE_S;
    }

#ifdef  FEATURE_UIM_RPT_BAD_SW_ON_POLL
    /* Report poll error without retries */
    if( (uim_static_buffer_used) &&
         (uim_static_cmd_ptr->hdr.command == UIM_INTERNAL_POLL_F))
    {
       uim_static_cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
       notification_reason               = UIM_POLL_ERR_S;
    }
#endif

    /* If we have exceeded the number of attempts, then declare the
       UIM as faulty, flush the commands and indicate to the higher
       layer */
    if (++uim_static_cmd_ptr->hdr.cmd_count > UIM_MAX_NUM_ATTEMPTS)
    {
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
      uim_power_down();

      if(uim_atr_pps_done)
      {
       /* Log as much information as possible for postmortem analysis */
        (void)uim_log_timeout_info(cmd_ptr, uim_static_cmd_ptr, *i_mask,
                                                     uim_get_uart_status());
      }

      /* set the status to ERROR */
      uim_status = UIM_ERR_S;

      /* Clear out any votes to keep UIM powered on */
      uim_ctl = UIM_NONE;
      uim_powerdown = TRUE;
      uim_command_in_progress = FALSE;

      /* flush all the commands in the command queue */
      uim_flush_command(UIM_FLUSH_ALL);

      /* Whenever we receive a command, send an error report as the
      SIM is to be powered down */
      *i_mask |= UIM_CMD_Q_SIG;

      /* Notify the higher layers that UIM is declared faulty */
      uim_notify_error(notification_reason);

      /* reset the directories and send a power down to UIM */
      uim_reset_dir();
#ifdef FEATURE_UIM_SUPPORT_CSIM
      uim_reset_csim_dir();
#endif /* FEATURE_UIM_SUPPORT_CSIM*/

      /* Reset the static buffer used flag */
      uim_static_buffer_used = FALSE;

      /* Reset the static command count */
      uim_static_cmd_ptr->hdr.cmd_count = 0;
#else
      /* Set the status for the respective slot */
      uim_set_uim_status_for_slot (UIM_ERR_S, uim_static_cmd_ptr->hdr.slot);

      /* Power down the slot */
      uim_power_down (uim_static_cmd_ptr->hdr.slot);
      MSG_MED ("Powering down Slot %d",uim_static_cmd_ptr->hdr.slot,0,0);

      if ((uim_status == UIM_ERR_S) && (uim_status_2 == UIM_ERR_S))
      {
        /* Process power down for both the slots */
        uim_process_error_for_dual_slots(i_mask);
      }
      else
      {
        /* If required power up the second slot */
        uim_process_slot_powerup (i_mask, uim_static_cmd_ptr);
      }
#endif
    }
    /* else reset the UIM */
    else
    {
      if ((uim_static_cmd_ptr->hdr.command == UIM_INTERNAL_POLL_F)
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
          && (uim_static_cmd_ptr->hdr.slot == uim_polling_slot)
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
         )
      {
        MSG_MED("Setting UIM POLL timer after a poll failure", 0, 0, 0 );
        uim_set_poll_timer( uim_poll_time_delay );
      }
      uim_reset_uim (uim_static_cmd_ptr, i_mask, uim_me_powerup);
    }
  }  /* else an internal command */
} /* uim_process_cmd_err */

/* <EJECT> */


/*===========================================================================

FUNCTION UIM_HANDLE_SW1_WARNINGS1_NORMAL_END_RESELECT_MF         INTERNAL

DESCRIPTION
  This procedure builds a UIM MF select command and inserts the command
  to the front of the uim cmd queue

DEPENDENCIES
  This function uses global variable uim_sw1_warnings1_normal_end_occured.

RETURN VALUE
  None.

SIDE EFFECTS
  A Select MF will occur, which will bring the uim_last_sel_dir to change to
  MF.

===========================================================================*/
static void uim_handle_sw1_warnings1_normal_end_reselect_mf(
  const uim_cmd_type * cmd_ptr)
{
  uim_cmd_type *uim_cmd_ptr = NULL;

  if (uim_sw1_warnings1_normal_end_occured)
  {
   /*
    * Only try and Select MF two times if we received warning while selecting
    * MF. This will prevent code from entring into a loop trying to select MF
    * on receiving warnings during select.
    * If we have tried twice reset all the variables and move out.
    */
    uim_mf_reselect = TRUE;
    if(uim_mf_reselect_count >= UIM_MAX_COMMAND_RETRIES)
    {
      uim_mf_reselect = FALSE;
      uim_sw1_warnings1_normal_end_occured = FALSE;
      uim_mf_reselect_count = 1;
      return;
    }

    if (cmd_ptr == NULL)
    {
      MSG_ERROR("No valid input cmd_ptr", 0, 0, 0);
      return;
    }

    if ((uim_cmd_ptr = (uim_cmd_type *)q_get( &uim_free_q )) == NULL)
    {
      MSG_ERROR("No more free queue to get from uim_free_q", 0, 0, 0);
      return;
    }
    /*---------------------------------------------------------------------------
    Populate UIM Cmd pointer data
    ---------------------------------------------------------------------------*/
    uim_cmd_ptr->hdr.command      = UIM_SELECT_F;
    uim_cmd_ptr->hdr.protocol     = cmd_ptr->hdr.protocol;
    uim_cmd_ptr->hdr.slot         = cmd_ptr->hdr.slot;
    uim_cmd_ptr->hdr.channel      = cmd_ptr->hdr.channel;

    uim_cmd_ptr->hdr.cmd_hdr.task_ptr   = &uim_tcb;
    uim_cmd_ptr->hdr.cmd_hdr.sigs       = 0;
    uim_cmd_ptr->hdr.options            = UIM_OPTION_NONE;
    uim_cmd_ptr->hdr.rpt_function       = NULL;
    /* do not modify done_q_ptr, link since it is from free_q */

    uim_cmd_ptr->select.item      = UIM_NO_SUCH_ITEM;
    uim_cmd_ptr->select.dir       = (uim_dir_type)UIM_MF;
    uim_cmd_ptr->select.file_type = UIM_MF;

    uim_cmd_ptr->select.aid.aid_length = cmd_ptr->select.aid.aid_length;
    if (uim_cmd_ptr->select.aid.aid_length > UIM_AID_MAX_NAME_LENGTH)
    {
      MSG_ERROR("Aid length too long 0x%x, cannot build reselect MF command",
        uim_cmd_ptr->select.aid.aid_length, 0, 0);
      q_put( &uim_free_q, &uim_cmd_ptr->hdr.cmd_hdr.link );
      return;
    }
    if (uim_cmd_ptr->select.aid.aid_length > 0)
    {
      (void) memcpy(uim_cmd_ptr->select.aid.data,
                    cmd_ptr->select.aid.data,
                    cmd_ptr->select.aid.aid_length);
    }

    memset(uim_cmd_ptr->select.path, 0x00, sizeof(uim_cmd_ptr->select.path));

    /* if protocol == UICC, session should be set to UIM_ACTIVATION_OR_RESET (0)
       if protocol == ICC, session is not applicable, default to 0 */
    uim_cmd_ptr->select.session = UIM_ACTIVATION_OR_RESET;

    if ( q_cnt( &uim_cmd_q ) == 0 )
    {
      q_put( &uim_cmd_q, &uim_cmd_ptr->hdr.cmd_hdr.link );
    }
    else
    {
      /* Insert the reselect MF to the beginning of the queue */
#ifdef FEATURE_Q_NO_SELF_QPTR
      q_insert( & uim_cmd_q,
                & uim_cmd_ptr->hdr.cmd_hdr.link,
                & ((uim_cmd_type*) q_check( & uim_cmd_q ))->hdr.cmd_hdr.link );
#else
      q_insert( & uim_cmd_ptr->hdr.cmd_hdr.link,
                & ((uim_cmd_type*) q_check( & uim_cmd_q ))->hdr.cmd_hdr.link );
#endif /* FEATURE_Q_NO_SELF_QPTR */
    }
  }
} /* uim_handle_sw1_warnings1_normal_end_reselect_mf */


/*===========================================================================

FUNCTION UIM_PROCESS_CARD_RESPONSE                EXTERNAL

DESCRIPTION
  This procedure processes the response received from the UIM card and changes
  the UIM server control variables based upon the processing status.

DEPENDENCIES
  This function uses global variables defined for the UIM server to determine
  the proper processing of the card response.

RETURN VALUE
  cmd_ptr value.

SIDE EFFECTS
  The control variables of the UIM server are adjusted to reflect the change
  in the operation of the UIM server as a result of the processing on the
  response.

===========================================================================*/

uim_cmd_type *uim_process_card_response
(
uim_cmd_type  *cmd_ptr, /* pointer to command */
rex_sigs_type *i_mask   /* Mask of signals interested in */
)
{
  uim_cmd_status_type status = UIM_CMD_SUCCESS;      /* UIM result type */
  uim_cmd_type     *return_ptr = cmd_ptr; /* return value of cmd_ptr */
  boolean uim_special_handling = FALSE; /* flag to bypass the usual command 
                                           response handling */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  uim_slot_type cmd_slot;
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

  if (i_mask == NULL)
  {
    ERR_FATAL("uim_process_card_response i_mask  is NULL",0,0,0);
  }

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

  /* Set the special handling flag to TRUE, when NV ENS is set and 
     the SIM returns 6F 00 (technical problem) */
  if( uim_nv_ens_enabled_flag)
  {
    /* Return command error immediately when SIM returned a
       technical problem */
    if( uim_rsp_buf_ptr->sw1 == (byte) SW1_PROBLEM &&
        uim_mode != UIM_PASSIVE_MODE )
    {
      /* set flag to bypass the usual command response handling */
      uim_special_handling = TRUE;
      uim_inc_tech_prob_cnt++;
      /* Return an error status if the command failed. */
      status = UIM_CMD_ERR;
    }
    else
    {
      /* reset the count of technical problems */
      uim_inc_tech_prob_cnt = 0;
    }
  }


  /* Process the command response in the usual way if special handling
     is not set */
  if( !uim_special_handling)
  {
    /* If static buffer is used, use the static command pointer as a
       parameter to uim_command_response */
    if (uim_static_buffer_used)
    {
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
      cmd_slot = uim_static_cmd_ptr->hdr.slot;
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
      status = uim_command_response(uim_rsp_buf_ptr,uim_static_cmd_ptr);
    }
    else
    {
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
      cmd_slot = cmd_ptr->hdr.slot;
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
      status = uim_command_response(uim_rsp_buf_ptr,cmd_ptr);
    }
  }

#ifdef FEATURE_GSTK
  /* Mimic the case of no-FETCH to UIM_CMD_COMPLETED case */
  if(status == UIM_CMD_FETCH && !gstk_uim_fetch_allow())
  {
    status = UIM_CMD_COMPLETED;
  }
#endif /* FEATURE_GSTK */
  /* If the ICCID does not match, declare the UIM as faulty and
     power down the UIM */
  if ((status == UIM_ICCID_MISMATCH) || (status == UIM_VOLTAGE_MISMATCH))
  {
    /* Clear the command pointer as we are done with this command. */
    return_ptr = NULL;

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
    /* set the status to ERROR */
    uim_status = UIM_ERR_S;

    /* Clear out any votes to keep UIM powered on */
    uim_ctl = UIM_NONE;
    uim_powerdown = TRUE;

    /* flush all the commands in the command queue */
    uim_flush_command(UIM_FLUSH_ALL);

    /* Whenever we receive a command, send an error report as the
    SIM is to be powered down */
    *i_mask |= UIM_CMD_Q_SIG;

    /* Notify the higher layers that UIM is declared faulty */
    uim_notify_error(UIM_VOLTAGE_MISMATCH_S);

    /* reset the directories and send a power down to UIM */
    uim_reset_dir();
#ifdef FEATURE_UIM_SUPPORT_CSIM
    uim_reset_csim_dir();
#endif /* FEATURE_UIM_SUPPORT_CSIM*/
    uim_power_down();

    /* Reset the static command count */
    uim_static_cmd_ptr->hdr.cmd_count = 1;
#else
    /* Set the status for the respective slot */
    uim_set_uim_status_for_slot (UIM_ERR_S, uim_static_cmd_ptr->hdr.slot);

    /* Power down the slot */
    uim_power_down (uim_static_cmd_ptr->hdr.slot);

    MSG_MED ("Powering down Slot %d",uim_static_cmd_ptr->hdr.slot,0,0);

    if ((uim_status == UIM_ERR_S) && (uim_status_2 == UIM_ERR_S))
    {
      /* Process power down for both the slots */
      uim_process_error_for_dual_slots(i_mask);
    }
    else
    {
      /* If required power up the second slot */
      uim_process_slot_powerup (i_mask, uim_static_cmd_ptr);
    }
#endif
    /* reset the sw1 warning flag to FALSE without the need to reselect MF */
    uim_sw1_warnings1_normal_end_occured = FALSE;
  }
  /* else if the polling is not successful */
  else if (status == UIM_POLL_ERROR)
  {
    /* Clear the command pointer as we are done with this command. */
    return_ptr = NULL;

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
    uim_notify_error(UIM_POLL_ERR_S);
#else
    /* Set the status for the respective slot */
    uim_set_uim_status_for_slot (UIM_POLL_ERR_S, uim_static_cmd_ptr->hdr.slot);
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

    /* include the command Q sig to the mask */
    *i_mask |= UIM_CMD_Q_SIG;

    /* Reset the static command count */
    uim_static_cmd_ptr->hdr.cmd_count = 1;

    /* reset the static buffer used flag */
    uim_static_buffer_used = FALSE;

    /* Reset the command in progress flag */
    uim_command_in_progress = FALSE;

    /* reset the sw1 warning flag to FALSE without the need to reselect MF */
    uim_sw1_warnings1_normal_end_occured = FALSE;

    MSG_HIGH (" Polling Error in UIM %d", uim_status,0,0);
  }
#ifdef FEATURE_UIM_UICC
  else if (status == UIM_INSTRN_CHANGE)
  {
    uim_reset_uim (uim_static_cmd_ptr, i_mask, uim_me_powerup);
    /* reset the sw1 warning flag to FALSE without the need to reselect MF */
    uim_sw1_warnings1_normal_end_occured = FALSE;
  }
#endif

#ifdef FEATURE_UIM_SUPPORT_INVERSE_CONVENTION
  else if (status == UIM_CONVENTION_CHANGE)
  {
    uim_reset_uim (uim_static_cmd_ptr, i_mask, uim_me_powerup);
    /* reset the sw1 warning flag to FALSE without the need to reselect MF */
    uim_sw1_warnings1_normal_end_occured = FALSE;
  }
#endif

#ifdef FEATURE_UIM_TOOLKIT
  /* else if a proactive command is pending */
  else if (status == UIM_CMD_FETCH)
  {
    /* Clear the command pointer as we are done with this command. */
    return_ptr = NULL;

    /* If the command completed is an internal poll, then reset the timer */
    if ((uim_static_buffer_used) &&
        (uim_static_cmd_ptr->hdr.command == UIM_INTERNAL_POLL_F))
    {
      MSG_MED( "Set UIM POLL timer following a poll bf FETCH", 0, 0, 0 );
      uim_set_poll_timer( uim_poll_time_delay );
    }

    if (!uim_static_buffer_used)
    {
      /* Check if sw1 warnings1 with normal end was generated during this operation
       If so, built an internal command to reselect MF */
      uim_handle_sw1_warnings1_normal_end_reselect_mf(cmd_ptr);
    }

    /* Send a FETCH command to the SIM */
    uim_fetch (uim_static_cmd_ptr, i_mask);
  }
#endif /* FEATURE_UIM_TOOLKIT */
  /* else if the command was completed successfully */
  else if (status == UIM_CMD_COMPLETED)
  {
    /* Clear the command pointer as we are done with this command. */
    return_ptr = NULL;

    /* Set the flag to false since the command is completed */
    uim_command_in_progress = FALSE;

    /* Reset the internal command used flag */
    uim_internal_command_used = FALSE;

    /* include the command Q sig to the mask */
    *i_mask |= UIM_CMD_Q_SIG;

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
    if (uim_me_powerup)
    {
      if(cmd_ptr)
      {
        /* If cmd_ptr is not null and the external command is a reset
         * and the internal command is an INTERNAL PUP, we need
         * to send a response for the reset command
         */
        if((cmd_ptr->hdr.command == UIM_RESET_F) &&
           (uim_static_cmd_ptr->hdr.command == UIM_INTERNAL_ME_PUP_F))
        {
          MSG_HIGH("Processing response for the reset command", 0, 0, 0);
          uim_process_generic_response(uim_rsp_buf_ptr, cmd_ptr);
        }
      }
      else
      {
        MSG_HIGH("cmd_ptr is NULL",0,0,0);
      }

#if defined(FEATURE_MMGSDI) || defined(FEATURE_UIM_USIM)
      uim_gsdi_link_est();
#endif
      uim_me_powerup = FALSE;

      /* Trigger the first poll */
      uim_set_poll_timer( uim_poll_time_delay  );

    }
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

    if (uim_static_buffer_used)
    {
      /* If it is an internal poll command then set the poll timer */
      if ((uim_static_cmd_ptr->hdr.command == UIM_INTERNAL_POLL_F)
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
          && (cmd_slot == uim_polling_slot)
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
         )
      {
        MSG_MED("Setting UIM POLL timer following a poll", 0, 0, 0 );
        uim_set_poll_timer( uim_poll_time_delay );
      }
      /* Set the status as Initialized if the power up command is
         completed successfully */
      else if ((uim_static_cmd_ptr->hdr.command == UIM_INTERNAL_ME_PUP_F)||
               (uim_static_cmd_ptr->hdr.command == UIM_INTERNAL_WAKE_UP_F)
              )
      {
        /* Reset the counters */
#ifdef FEATURE_UIM_SUPPORT_INVERSE_CONVENTION
        uim_convention_change_count = 1;
#endif /* FEATURE_UIM_SUPPORT_INVERSE_CONVENTION */

#ifdef FEATURE_UIM_UICC
        uim_instrn_toggle_count = 1;
#endif /* FEATURE_UIM_UICC */

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
        uim_status = UIM_INITIALIZED_S;

        /* Reset the static command count */
        uim_static_cmd_ptr->hdr.cmd_count = 1;

        /* reset the static buffer used flag */
        uim_static_buffer_used = FALSE;
#else
        /* Set the status */
        uim_set_uim_status_for_slot(UIM_INITIALIZED_S,
                                    uim_static_cmd_ptr->hdr.slot);

        /* Process power up */
        uim_process_slot_powerup (i_mask, uim_static_cmd_ptr);
#endif
      } /* end if - powerup command */
    }
    else
    {
      /* Check if sw1 warnings1 with normal end was generated during this operation
       If so, built an internal command to reselect MF */
      uim_handle_sw1_warnings1_normal_end_reselect_mf(cmd_ptr);
    }
  } /* end if - command completed with no errors */
  /* if the result was not something that we expected */
  else if (status == UIM_CMD_ERR)
  {
    uim_process_cmd_err(cmd_ptr,i_mask);

    if (!uim_static_buffer_used)
    {
      /* Check if sw1 warnings1 with normal end was generated during this operation
       If so, built an internal command to reselect MF */
      uim_handle_sw1_warnings1_normal_end_reselect_mf(cmd_ptr);
    }
  }  /* status == UIM_CMD_ERR */
#if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES )
  else if (status == UIM_SWITCH_VOLTAGE)
  {
    /* Reset the card - the interface should already be powered down and
       the voltage class selected */
    uim_reset_uim (uim_static_cmd_ptr, i_mask, uim_me_powerup);

    /* reset the sw1 warning flag to FALSE without the need to reselect MF */
    uim_sw1_warnings1_normal_end_occured = FALSE;
  }  /* status == UIM_SWITCH_VOLTAGE */
#endif /* FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES */

  /* Return the new cmd_ptr value */
  return( return_ptr);
} /* uim_process_card_response */

/* <EJECT> */


/*===========================================================================

FUNCTION UIM_CAN_IGNORE_TIMEOUT

DESCRIPTION
  This function check whether or not we can ignore the timeout signal by
  checking some variables and the uart status to see if an rx_isr would
  eventually fire or the possibility that the interrupt was already fired
  after the rex_wait has exited.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if we can safely ignore the timeout signal
  FALSE otherwise

SIDE EFFECTS
  None

===========================================================================*/

LOCAL boolean uim_can_ignore_timeout(rex_sigs_type sigs)
{

  boolean ignore_timeout = FALSE;

  /* Start ignoring the time-outs only if we have an indication not to
   * ignore the timeout signal because of not receiving the start of
   * ATR on time.  Do not ignore the timeout even if there are
   * bytes in the FIFO if the transaction timer expired since there
   * could NULL bytes since a long time.
   */
  if ( clear_timeout_on_response && !( sigs & UIM_TRANSACTION_SIG ) )
  {
    /* Lock the interrupts to make sure that all the checks are done
     * atomically
     */
    INTLOCK();

    if ( (uim_get_uart_status() & MSMU_SR_RXRDY) ||
#ifdef FEATURE_USE_TIME_VU
         TIMER_IS_ACTIVE(&uim_cmd_rsp_timer) ||
#else
         (uim_cmd_rsp_timer.cnt != 0 ) ||
#endif
         (rex_get_sigs(&uim_tcb) & UIM_CMD_RSP_SIG) )
    {
      /* The following conditions based on which we ignore the time out signal
       *
       * 1. There are bytes in the FIFO indicating that the rx_isr
       *    would eventually be fired setting a new timer or completing
       *    the transaction based on the contents pending in the FIFO
       *
       * 2. After the signals have been extracted, the task was pre-empted
       *    and rx_isr was scheduled which would clear out the reeive FIFO
       *    and re-set the timer.  In this case, there whould be some
       *    count pending in the timer.
       *
       * 3. After the signals have been extracted, the task was pre-empted
       *    and rx_isr was scheduled which ended the transaction.  In This
       *    case, though there is no timer set, the response signal should
       *    be pending.
       */

      ignore_timeout = TRUE;
    }

    /* Free up the interrupts */
    INTFREE();
  }
  /* If we have an indication to not ignore the timeout signal,
   * prepare for the next iteration.
   */
  clear_timeout_on_response = TRUE;

  return ignore_timeout;
}
/* <EJECT> */


/*===========================================================================

FUNCTION UIM_PET_AND_WAIT

DESCRIPTION
  This procedure accepts rex_sigs_type mask and keeps waiting for any of
  the signals in the mask specified.  Also, if it is time to report to
  the dog, the dog would be pet.

DEPENDENCIES
  None

RETURN VALUE
  rex_sigs_type

SIDE EFFECTS
  None

===========================================================================*/
rex_sigs_type uim_pet_and_wait(rex_sigs_type sigs_to_wait_for)
{
  rex_sigs_type sigs_received;

  do
  {
    /* Wait for the signals requested and also, Include dog report
     * signal to pet the dog
     */
    sigs_received = rex_wait( sigs_to_wait_for | UIM_RPT_TIMER_SIG );

    if ((sigs_received & UIM_RPT_TIMER_SIG) != 0)
    {
      /* Clear the dog report timer signal */
      (void) rex_clr_sigs(&uim_tcb, UIM_RPT_TIMER_SIG);

      /* -----------------------------
      ** Kick watchdog and reset timer
      ** ----------------------------- */
      dog_report(DOG_UIM_RPT);
      (void) rex_set_timer( &uim_rpt_timer, DOG_UIM_RPT_TIME );
    } /* if UIM_RPT_TIMER_SIG */
  } while( (sigs_received & sigs_to_wait_for) == 0 );
  return sigs_received;
}
/* <EJECT> */

/*===========================================================================

FUNCTION UIM_WAIT_FILTER_NON_RECOVERY_CMDS

DESCRIPTION
This function waits for list of signals that we are interested in and if
we are interested in UIM_CMD_Q_SIG, we will send it out as a received
signal only if the command in the command queue is a recovery command.

DEPENDENCIES
  None

RETURN VALUE
  signals returned by rex_wait

SIDE EFFECTS
  Does not allow processing any other commands other than those commands
  in the uim_cmd_q which are marked to be recovery commands

===========================================================================*/
LOCAL rex_sigs_type uim_wait_filter_non_recovery_cmds(rex_sigs_type sigs_to_wait_for)
{
  rex_sigs_type sigs_received = 0;

  do
  {
    /* Check if are interested in looking at the commands (UIM_CMD_Q_SIG) and
     * there are commads in the command queue
     */
    if (((sigs_to_wait_for & UIM_CMD_Q_SIG) != 0) && (q_cnt(& uim_cmd_q) != 0))
    {
      /* Check to make sure that head of the queue has a recovery command */
      if ( (int)((uim_cmd_type *)q_check(&uim_cmd_q))->hdr.options &
           (int)UIM_OPTION_RECOVERY_CMD )
      {
        /* The command at the head of the command queue is a recovery
         * command.  Return a command queued signal along with any other
         * signals received so that the command would be processed.
         */
        MSG_HIGH("Received a recovery command returning %x",
                 ( UIM_CMD_Q_SIG | sigs_received ) ,0, 0);
        return ( UIM_CMD_Q_SIG | sigs_received );
      }
      else
      {
         MSG_HIGH("Command at the tip not a recovery command", 0, 0, 0 );
      }
    }

    /* Wait for signals we are interested in */
    sigs_received = rex_wait(sigs_to_wait_for);

    if (sigs_received & UIM_CMD_Q_SIG &&
        (uim_mode == UIM_RECOVERY_MODE) )
    {
      /* We have received a command que signal, clear the signal and
       * continue.  If this command is indeed a recovery command then
       * the signal will be attached to the list of signals and returned
       * to the caller in the next iteration
       */
      (void) rex_clr_sigs(&uim_tcb, UIM_CMD_Q_SIG);

      /* Remove the UIM_CMD_Q_SIG from the signals received */
      sigs_received &= ~UIM_CMD_Q_SIG;
    }
  } while ( (sigs_received & sigs_to_wait_for) == 0 );

  MSG_LOW("Returning signals %x", sigs_received, 0, 0);
  return sigs_received;
}
/* <EJECT> */


/*===========================================================================

FUNCTION UIM_TASK

DESCRIPTION
  This procedure is the entrance procedure for the UIM task.

DEPENDENCIES
  None

RETURN VALUE
  Does not return.

SIDE EFFECTS
  None

===========================================================================*/

void uim_task
(
dword dummy
/* Parameter required for REX.  Tell lint to ignore it. */
/*lint -esym(715,dummy) */
)
{

  rex_sigs_type rex_signals_mask;  /* Mask of signals returned by rex */
  rex_sigs_type i_mask;            /* Mask of signals interested in */
  uim_cmd_type *cmd_ptr;           /* Pointer to received command */
  uim_status_type uim_temp_status;
  uim_status_type notification_reason = UIM_ERR_S;

#ifdef FEATURE_UIM_TOOLKIT
  uim_proactive_uim_data_type uim_temp_type;
#endif
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  uim_slot_type uim_slot_for_cmd = UIM_SLOT_AUTOMATIC;
  boolean slot_status = FALSE;
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  i_mask = UIM_RPT_TIMER_SIG |
           TASK_OFFLINE_SIG |
           TASK_STOP_SIG |
           UIM_CMD_RSP_SIG |
           UIM_CMD_RSP_TIMEOUT_SIG |
           UIM_POLL_TIMER_SIG |
           UIM_UNSOLICITED_RSP_SIG |
           UIM_STATE_TOGGLE_SIG |
           UIM_TRANSACTION_SIG |
           UIM_CARD_SWAP_SIG|
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
           UIM_EFSLOG_PURGE_SIG;

#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
  i_mask |= UIMDATA_INIT_SIG;
#endif
  /* ----------------------
  ** Perform initialization
  ** ---------------------- */

  /* uim_dev_init() is called before UIM task is created.
     For 8200, it is called here because TMC task is running on AMR9 */
  #if defined( FEATURE_UIM_MDM8200_DRIVER )
#error code not present
  #endif

  uim_init();
  cmd_ptr = NULL;  /* Initialize the pointer to NULL pointer */

  /* ------------------------------------------------
  ** Wait for start signal from the Main Control task
  ** ------------------------------------------------ */
#ifndef TEST_FRAMEWORK
#ifdef FEATURE_MULTIMODE_ARCH
  task_start
  (
  UIM_RPT_TIMER_SIG,             /* report timer signal for task */
  DOG_UIM_RPT,                   /* watchdog report */
  &uim_rpt_timer                 /* pointer to report timer */
  );
#else
  mc_task_start
  (
  UIM_RPT_TIMER_SIG,             /* report timer signal for task */
  DOG_UIM_RPT,                   /* watchdog report */
  &uim_rpt_timer                 /* pointer to report timer */
  );
#endif /*FEATURE_MULTIMODE_ARCH*/
#else
  tmc_task_start();
#endif/* !TEST_FRAMEWORK */

  MSG_LOW( "Acknowledged the Task start signal ", 0, 0, 0 );
  /* ------------------------------------------------------------------
  ** Initially kick watchdog and set timer for watchdog report interval
  ** ------------------------------------------------------------------ */
  dog_report(DOG_UIM_RPT);
  (void) rex_set_timer( &uim_rpt_timer, DOG_UIM_RPT_TIME );

#if !defined( TEST_FRAMEWORK ) && !defined(PLATFORM_LTK)
  (void)uim_pet_and_wait(UIM_NV_INIT_DONE_SIG | TASK_OFFLINE_SIG | TASK_STOP_SIG);
#endif /* !TEST_FRAMEWORK && !PLATFORM_LTK */

  /* Set the signal so that we enter the task loop rightaway */
  (void) rex_set_sigs( &uim_tcb, UIM_STATE_TOGGLE_SIG);

#ifdef UIM_DRIVER_TIMESTAMP
  uim_StartDriverTimeStamp();
#endif

#ifdef FEATURE_UIM_LOG_TO_EFS
  (void)uim_efslog_init();
#endif /* FEATURE_UIM_LOG_TO_EFS */

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

  for (;;)
  {
    /* Never exit this loop... */

    /* Initialize the error to be notified in case we have to notify GSDI */
    notification_reason = UIM_ERR_S;

    /* If we are in recovery mode, filter away non recovery commands */
    if ( uim_mode == UIM_RECOVERY_MODE )
    {
      rex_signals_mask = uim_wait_filter_non_recovery_cmds(i_mask) & i_mask;
    }
    /* else if the command q sig is set in the mask and there is a command in
       the command queue, then set rex signals mask as command q sig */
    else if (((i_mask & UIM_CMD_Q_SIG) != 0) && (q_cnt(& uim_cmd_q) != 0))
    {
      rex_signals_mask = UIM_CMD_Q_SIG;
    }
    /* else do the rex wait and mask the signals that we are interested in */
    else
    {
      rex_signals_mask = rex_wait(i_mask) & i_mask;
    }

    /* ---------------------------------------------------------
    ** Check if report timer signal was set.  If set then clear
    ** signal, report to DOG, set the timer again and proceed.
    ** ------------------------------------------------------- */
    if ((rex_signals_mask & UIM_RPT_TIMER_SIG) != 0)
    {
      /* -----------------------------
      ** Kick watchdog and reset timer
      ** ----------------------------- */
      dog_report(DOG_UIM_RPT);
      (void) rex_set_timer( &uim_rpt_timer, DOG_UIM_RPT_TIME );
    }

    /* -------------------------------------------------------
    ** Check if powerdown command signal was set.  If set then
    ** clear signal, send ack back to MC, and proceed.
    ** ------------------------------------------------------- */
    if ((rex_signals_mask & TASK_STOP_SIG) != 0)
    {
      MSG_MED( "TASK_STOP_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &uim_tcb, TASK_STOP_SIG );

      /* Make sure to power down the interface when not in use. */
      uim_powering_down_task = TRUE;

      /* Vote everybody off */
      (void)uim_power_control((uim_voter_type)((int)UIM_AUTH | (int)UIM_MC | (int)UIM_PROACTIVE_UIM | (int)UIM_UI |
                               (int)UIM_USIM_SESSION | (int)UIM_SESSION | UIM_TRAF_CHAN),
                              FALSE);

      /* change the status to ERROR so that we will send back any commands that
       * might come our way after the task_stop signal
       */
      uim_status = UIM_ERR_S;

      /* Make sure task can process this control */
      (void) rex_set_sigs( &uim_tcb, UIM_STATE_TOGGLE_SIG);
    }

    /* -----------------------------------------------------
    ** Check if offline command signal was set.  If set then
    ** clear signal, send ack back to MC, and proceed.
    ** ----------------------------------------------------- */
    if ((rex_signals_mask & TASK_OFFLINE_SIG) != 0)
    {
      MSG_MED( "TASK_OFFLINE_SIG received", 0,0,0 );
      (void) rex_clr_sigs( &uim_tcb, TASK_OFFLINE_SIG );
#ifdef FEATURE_MULTIMODE_ARCH
      /* Process task stop procedure.*/
      task_offline();
#else
      (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
#endif
    }
#ifdef FEATURE_UIM_HOT_SWAP
  /* -----------------------------------------------------
  ** Check if card swap command signal was set.  If set then
  ** clear signal and send command to gsdi
  ** ----------------------------------------------------- */
  if ((rex_signals_mask & UIM_CARD_SWAP_SIG) != 0)
  {

    MSG_MED( "CARD_SWAP_SIG received", 0,0,0 );
    (void) rex_clr_sigs( &uim_tcb, UIM_CARD_SWAP_SIG );

    gsdi_command_ptr.message_header.message_set = MS_UIM_SERVER;
    gsdi_command_ptr.message_header.message_id  = GSDI_CARD_SWAP_MSG;

#if (defined (FEATURE_UIM_GSM) || defined (FEATURE_UIM_USIM)) && \
     !defined (FEATURE_UIM_RUIM_W_GSM_ACCESS)
#error code not present
#else
    ((IMH_T *)&(gsdi_command_ptr.message_header))->message_len_msb =
      (unsigned char)((sizeof (gsdi_command_ptr) -
                   sizeof (gsdi_cmdhdr_T)) / 0x100);
    ((IMH_T *)&(gsdi_command_ptr.message_header))->message_len_lsb =
      (unsigned char)((sizeof (gsdi_command_ptr) -
                   sizeof (gsdi_cmdhdr_T)) % 0x100);
#endif

    (void)gsdi_cmd_ext(&gsdi_command_ptr);

  }
#endif /* FEATURE_UIM_HOT_SWAP */

    /*-------------------------
    ** Poll Timer has expired
    ** ----------------------*/
    if ((rex_signals_mask & UIM_POLL_TIMER_SIG) != 0)
    {
      /* clear the timer since for dual timer solution, one more timer
       * might still be running.  And for single timer solution, this
       * would not affect any functionality.
       */
      uim_clear_poll_timer();

      /* clear the signal */
      (void) rex_clr_sigs( &uim_tcb, UIM_POLL_TIMER_SIG);

      /* Set the internal flag as poll pending */
      uim_poll_pending = TRUE;
    }

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

    /* -------------------------------------------------------
    ** Check if the power up is due to a task power up. If TRUE
    ** then start the Power up procedures for UIM
    ** ----------------------------------------------------- */
    if ((uim_task_startup) && (uim_me_powerup))
    {
      MSG_LOW( "This is the first attempt to reset the card", 0, 0, 0 );
      /* This is the first attempt for this command  */
      uim_static_cmd_ptr->hdr.cmd_count = 1;
      uim_task_startup = FALSE;

      /* Reset the counters */
#ifdef FEATURE_UIM_SUPPORT_INVERSE_CONVENTION
      uim_convention_change_count = 1;
#endif
#ifdef FEATURE_UIM_UICC
      uim_instrn_toggle_count = 1;
#endif
      /* Start the power up sequence for slot 1 */
      uim_static_cmd_ptr->hdr.slot = UIM_SLOT_1;

      /* The power up mode will be set after parsing the historical characters
         from the ATR */
      uim_reset_uim(uim_static_cmd_ptr, &i_mask, uim_me_powerup);
    }

    /* ---------------------------------------------------------
    ** For Dual slots, Clock stop is used as the option to
    ** conserve battery power and power down processing is done
    ** only when the task is stopped
    ** --------------------------------------------------------*/

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
    /* ---------------------------------------------------------
    ** Check if UIM is powered down. If TRUE, start the power up
    ** if any entity has requestd power or if there is a command
    ** in the command queue. If FALSE, then process a command if
    ** there is one
    ** ------------------------------------------------------- */
    if (uim_status == UIM_POWERED_DOWN_S)
    {
      /* if we got here bcos of the signal being set */
      if ((rex_signals_mask & UIM_STATE_TOGGLE_SIG) != 0)
      {
        /* clear the signal */
        (void) rex_clr_sigs(&uim_tcb, UIM_STATE_TOGGLE_SIG);
      }
      /* we have a command in the q or we do now want to remain powered down */
      if ((!uim_powerdown) || (q_cnt(& uim_cmd_q) != 0))
      {
        /* This is the first attempt for this command  */
        uim_static_cmd_ptr->hdr.cmd_count = 1;

        /* Reset the counters */
#ifdef FEATURE_UIM_SUPPORT_INVERSE_CONVENTION
        uim_convention_change_count = 1;
#endif /* FEATURE_UIM_SUPPORT_INVERSE_CONVENTION */
#ifdef FEATURE_UIM_UICC
        uim_instrn_toggle_count = 1;
#endif /* FEATURE_UIM_UICC */

        /* Peek into the command to see the protocol in the command */
        if (((cmd_ptr = (uim_cmd_type*) q_check( &uim_cmd_q )) != NULL) &&
            (q_cnt(& uim_cmd_q) != 0))
        {
          switch (cmd_ptr->hdr.protocol)
          {
#ifdef FEATURE_UIM_USIM
#error code not present
#endif /* FEATURE_UIM_USIM */

#ifdef FEATURE_UIM_UICC
            case UIM_UICC:
              /* Set the protocol so that we power up in the WCDMA mode */
              uim_static_cmd_ptr->hdr.protocol = UIM_UICC;
              break;
#endif /* FEATURE_UIM_UICC */

#ifdef FEATURE_UIM_RUIM
            case UIM_CDMA:
              /* Set the protocol so that we power up in the WCDMA mode */
              uim_static_cmd_ptr->hdr.protocol = UIM_CDMA;
              break;
#endif /* FEATURE_UIM_RUIM */

#ifdef FEATURE_UIM_GSM
#error code not present
#endif /* FEATURE_UIM_GSM */

            default:
#if defined (FEATURE_UIM_GSM)
#error code not present
#elif defined (FEATURE_UIM_RUIM)
              uim_static_cmd_ptr->hdr.protocol = UIM_CDMA;
#elif defined (FEATURE_UIM_UICC)
              uim_static_cmd_ptr->hdr.protocol = UIM_UICC;
#endif /* FEATURE_UIM_GSM */
              break;
          }
        }
        else
        {
          /* Check to see which entity wants to keep the UIM powered on and
             set the power up mode based on that */

          /* This may not be the right way to check, but we will get a
             better understanding in the future as the higher layers
             start using the server */
#ifdef FEATURE_UIM_USIM
#error code not present
#endif /* FEATURE_UIM_USIM */
          {
#ifdef FEATURE_UIM_GSM
#error code not present
#elif defined (FEATURE_UIM_RUIM)
            uim_static_cmd_ptr->hdr.protocol = UIM_CDMA;
#endif /* FEATURE_UIM_GSM */
          }
        }

        /* Reset the UIM and start the power up */
        uim_reset_uim(uim_static_cmd_ptr, &i_mask, uim_me_powerup);
      }
    }
    else
#endif
    {
      /*------------------------------
      ** Clear the state toggle signal
      ** ----------------------------- */
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
      if (uim_status == UIM_ERR_S)
#else
      if ((uim_status == UIM_ERR_S) && (uim_status_2 == UIM_ERR_S))
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
      {
        /* if the state toggle signal is set */
        if ((rex_signals_mask & UIM_STATE_TOGGLE_SIG) != 0)
        {
          /* clear the signal */
          (void) rex_clr_sigs(&uim_tcb, UIM_STATE_TOGGLE_SIG);
        }
      }

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
      if (uim_status == UIM_CLK_OFF_S)
      {
        /* if the state toggle signal is set */
        if ((rex_signals_mask & UIM_STATE_TOGGLE_SIG) != 0)
        {
          /* clear the signal */
          (void) rex_clr_sigs(&uim_tcb, UIM_STATE_TOGGLE_SIG);
        }

        /* we have a command in the q */
        if (q_cnt(& uim_cmd_q) != 0)
        {
          /* Depending on whether the ISO or USB interface is selected
             either provide clock to UIM or resume USB interface */
          uim_stop_power_save();

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
          {
            MSG_MED ("Clock started",0,0,0);
          }

          /* Set the state back to Initialized state */
          uim_status = UIM_INITIALIZED_S;
        } /* end if - Check if clock should be turned back on */
      } /* end if - Check if clock is stopped */
#else
      if ((uim_status == UIM_CLK_OFF_S) || (uim_status_2 == UIM_CLK_OFF_S))
      {
        /* if the state toggle signal is set */
        if ((rex_signals_mask & UIM_STATE_TOGGLE_SIG) != 0)
        {
          /* clear the signal */
          (void) rex_clr_sigs(&uim_tcb, UIM_STATE_TOGGLE_SIG);
        }

        /* We may be turning ON the clock for the slot that is not required for
           command processing. But if the command is slated for a slot that has
           been declared as error, the clock will be stopped right after the
           command queue is empty */
        /* we have a command in q or we don't want to remain powered down */
        if (q_cnt(& uim_cmd_q) != 0)
        {
          uim_process_clock_start();
        } /* end if - Check if clock should be turned back on */
      }
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
#ifdef FEATURE_INIT_RUIM_SMSandADD_BYUIMTASK
      if (rex_signals_mask & UIMDATA_INIT_SIG)
      {
         (void) rex_clr_sigs(&uim_tcb, UIMDATA_INIT_SIG);
         if(db_getuiminitmask()&INITUIMSMSMASK)
         {
            if (wms_ruim_init_stepbystep())
            {// 短信初始化完成
                db_items_value_type db_item;
                
                db_item.db_uimsmsinited = TRUE;
                db_put(DB_UIMSMSINIT, &db_item);
                
                // 去掉初始化短信的掩码
                db_removeuiminitmask(INITUIMSMSMASK);
                // 启动下轮初始化
                (void) rex_set_sigs(&uim_tcb, UIMDATA_INIT_SIG);
            }
         }
         else if(db_getuiminitmask()&INITUIMADDMASK)
         {
            if (InitRUIMAddrBkCacheCb())
            {// 电话本初始化完成
                db_items_value_type db_item;
                
                db_item.db_uimaddinited = TRUE;
                db_put(DB_UIMADDINIT, &db_item);
                
                // 去掉初始化电话本的掩码
                db_removeuiminitmask(INITUIMADDMASK);
                
                // 启动下轮初始化
                (void) rex_set_sigs(&uim_tcb, UIMDATA_INIT_SIG);
            }
         }
      }
#endif

      /*-----------------------------
      ** We have a command to process
      ** ---------------------------- */
      if ((rex_signals_mask & UIM_CMD_Q_SIG) != 0)
      {
        (void) rex_clr_sigs( &uim_tcb, UIM_CMD_Q_SIG );
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
        if ((cmd_ptr = (uim_cmd_type *)q_get( &uim_cmd_q )) != NULL)
        {
          /* Command has not been attempted yet */
          uim_rpt_buf.cmd_transacted = FALSE;

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
          /* Get the slot for this command if it was not re-queued*/
          if (!uim_command_requeued)
          {
            slot_status = uim_get_slot_for_cmd (cmd_ptr, &uim_slot_for_cmd);
          }

          /* Reject the command if the status of the chosen slot is faulty when
             the command is not a pref slot change command or power_up command */
          if (((((uim_slot_for_cmd == UIM_SLOT_1) && (uim_status==UIM_ERR_S))||
                ((uim_slot_for_cmd == UIM_SLOT_2) && (uim_status_2==UIM_ERR_S))
                || ((uim_status == UIM_ERR_S) && (uim_status_2 == UIM_ERR_S))
                || (!slot_status)
               ) && ( (cmd_ptr->hdr.command  != UIM_PREF_SLOT_CHANGE_F) &&
                      (cmd_ptr->hdr.command  != UIM_POWER_UP_UIM_PASSIVE_F) &&
                      (cmd_ptr->hdr.command  != UIM_POWER_DOWN_F) &&
                      (cmd_ptr->hdr.command  != UIM_RESET_F) )
               ) || (!UIM_IS_CMD_VALID_IN_MODE(cmd_ptr)))
#else
          /* If a command is queued when UIM is in an erroneous state allow the
             command to be processed only if it is a reset, power down or
             passive power up command type */
          if ( ( (uim_status == UIM_ERR_S) &&
                 (cmd_ptr->hdr.command  != UIM_POWER_UP_UIM_PASSIVE_F) &&
                 (cmd_ptr->hdr.command  != UIM_POWER_DOWN_F) &&
                 (cmd_ptr->hdr.command  != UIM_RESET_F) ) ||
               ! UIM_IS_CMD_VALID_IN_MODE(cmd_ptr) )
#endif
          {
            if (uim_status == UIM_ERR_S)
            {
              MSG_HIGH( "SIM in power down state", 0, 0, 0);
            }
            else
            {
              UIM_ERROR_MSG("Invalid cmd UIM in passive mode ret error",0,0,0);
            }
            cmd_ptr->hdr.status = UIM_ERR_S;

            if (cmd_ptr->hdr.cmd_hdr.task_ptr != NULL)
            {
              (void) rex_set_sigs( cmd_ptr->hdr.cmd_hdr.task_ptr,
                                   cmd_ptr->hdr.cmd_hdr.sigs );
            }

            /* Get the mode of the command */
            uim_cmd_mode = (uim_command_mode_type)
                           UIM_GET_CMD_MODE((int)cmd_ptr->hdr.command);

            /* If always reporting & have a rpt function, report failure */
            /* uim_send_err_rpt internally releases the buffer */
            if (((int)cmd_ptr->hdr.options & (int)UIM_OPTION_ALWAYS_RPT) != 0)
            {
              /* Set the status words to default before sending the rpt */
              uim_rpt_buf.sw1 = (uim_sw1_type) 0;
              uim_rpt_buf.sw2 = (uim_sw2_type) 0;
              uim_send_err_rpt(cmd_ptr);
            }
            else
            {  /* return buffer to free queue */
              if ( cmd_ptr->hdr.cmd_hdr.done_q_ptr != NULL )
              {
                /* place command on requested queue */
                q_put( cmd_ptr->hdr.cmd_hdr.done_q_ptr,
                       &cmd_ptr->hdr.cmd_hdr.link );
              }
            }
          }
          else
          {
            /* We do not want to look into the command queue until we are
            done processing this command */

            i_mask &= ~UIM_CMD_Q_SIG;
            uim_static_buffer_used = FALSE;

            if (cmd_ptr->hdr.cmd_count == 0)
            {
              /* Set Count to indicate that this is the first attempt.. once
               * set to 1, it would not be reset to 0
               */
              cmd_ptr->hdr.cmd_count = 1;

              /* Re-intatialize internal command used since we have popped a
               * fresh command out of the command queue.  If an internal
               * command has to be used, it would be set again
               */
              uim_internal_command_used = FALSE;
            }

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS

#ifdef FEATURE_UIM_SLOT_AUTOMATIC
            /* Overwrite the slot info in the command */
            cmd_ptr->hdr.slot = uim_slot_for_cmd;
#endif
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */


            /* Due to some out of spec, commercialized RUIM cards we have to
               extend our work waiting timer value. Set back to original work
               waiting time if current technology is other than CDMA.
            */
            if (cmd_ptr->hdr.protocol == UIM_CDMA)
            {
              uim_work_waiting_time[uim_drv_slot] =
                    uim_extended_work_waiting_time[uim_drv_slot];
            }
            else
            {
              uim_work_waiting_time[uim_drv_slot] =
                    uim_original_work_waiting_time[uim_drv_slot];
            }

            if ( cmd_ptr->hdr.cmd_count > UIM_MAX_NUM_CMD_ATTEMPTS)
            {
              /* include the command Q sig to the mask as we are going
                 to process the command again */
              i_mask |= UIM_CMD_Q_SIG;

              /* Get the mode of the command */
              uim_cmd_mode = (uim_command_mode_type)
                             UIM_GET_CMD_MODE((int)cmd_ptr->hdr.command);
              /* reset the static buffer used flag */
              uim_static_buffer_used = FALSE;

              /* reset the internal command used flag */
              uim_internal_command_used = FALSE;

              /* This command has already got enough chances but did not go
               * through so send an appropriate error report
               */
              uim_send_err_rpt(cmd_ptr);
            }
            else
            {
              uim_process_command( cmd_ptr, &i_mask );
            }
          } /* uim status is error */
        }  /* (cmd_ptr != NULL) */
      } /* end if ((rex_signals_mask & UIM_CMD_Q_SIG) != 0) */
    } /* uim_status is not powered down */

    /*-------------------------------------
    ** We have recd a response
    ** ------------------------------------ */
    if ((rex_signals_mask & UIM_CMD_RSP_SIG) != 0)
    {
      (void) rex_clr_sigs( &uim_tcb, UIM_CMD_RSP_SIG );

      /* Check to see if we have already flushed out all the commands and
       * are not in a state to process commands
       */
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
      uim_temp_status = uim_status;
#else
      if(uim_drv_slot == UIM_DRV_SLOT1)
      {
        uim_temp_status = uim_status;
      }
      else
      {
        uim_temp_status = uim_status_2;
      }
#endif
     /* If a command response is available when UIM is in an erroneous state
        allow the response to be processed only if it is a power down command */
      if (((uim_temp_status == UIM_ERR_S)  &&
           ((cmd_ptr != NULL) && (cmd_ptr->hdr.command  != UIM_POWER_DOWN_F))) ||
          (!uim_static_buffer_used && cmd_ptr == NULL) )
      {
        ERR("CMD_RSP Sig Rcvd uim_status=0x%x, uim_st_bf_us=0x%x, cmd_ptr=0x%x",
            uim_status, uim_static_buffer_used, cmd_ptr);
        /* we have received the response */
        uim_command_in_progress = FALSE;
        if (uim_powering_down_task)
        {
          /* Let us reserve a chance to be scheduled and run the loop */
          (void) rex_set_sigs(&uim_tcb, UIM_STATE_TOGGLE_SIG);
        }
        /* Go back to the beginning of this task loop*/
        continue;
      }

#if defined( FEATURE_UIM_T_1_SUPPORT )
      if (UIM_T_1_PROTOCOL == uim_current_protocol)
      {
        /* Process the card response for the T=1 protocol */
        cmd_ptr = uim_process_t_1_block( uim_rsp_buf_ptr, cmd_ptr, &i_mask);
      }
      else
#endif /* FEATURE_UIM_T_1_SUPPORT */
      {
        cmd_ptr = uim_process_card_response( cmd_ptr, &i_mask );

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
        if( uim_nv_ens_enabled_flag &&
            uim_inc_tech_prob_cnt >= UIM_MAX_REPEATED_TECH_PROBLEMS &&
            uim_status == UIM_ERR_S )
        {
          uim_inc_tech_prob_cnt = 0;

          /* Power down the interface */
          uim_power_down();

          /* Notify the higher layers that UIM is declared faulty */
          uim_notify_error(uim_status);

          continue;
        }
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

      } /* end if - the protocol is T=1 */

      /* Check to see if we have a timeout signal and clear it since
       * we already got the response and processed it
       * unless the signal is explicitly set.
       */
      if ( clear_timeout_on_response )
      {
        (void) rex_clr_sigs( &uim_tcb, UIM_CMD_RSP_TIMEOUT_SIG );
        rex_signals_mask &= ~UIM_CMD_RSP_TIMEOUT_SIG;
      }
      else
      {
        /* reset the flag for next use */
        clear_timeout_on_response = TRUE;
      }

    } /* end if ((rex_signals_mask & UIM_CMD_RSP_SIG) != 0) */

#ifdef FEATURE_UIM_LOG_TO_EFS
    /*----------------------------------------------------------------
    ** Update EFSLOG
    ** --------------------------------------------------------------- */
    if ((rex_signals_mask & UIM_EFSLOG_PURGE_SIG) != 0)
    {
      /* If we still haven't got a GSDI Event (such as SIM INIT COMPLETED
      telling us to stop logging, and our log is getting massive, clean
      up and stop logging

      Also, if the logging_in_progress flag has been turned off,
      we should stop logging */

      if((uim_efslog_save_count >= UIM_EFSLOG_MAX_SAVES_TO_EFS) ||
         !uim_efslog_logging_in_progress)
      {
        uim_efslog_clean_up();
      }
      else
      {
      /* When the purge EFSLOG Signal is sent, send it to EFS. */

        uim_efslog_save_buffer_to_efs();
      }

    } /* signal UIM_EFSLOG_PURGE_SIG */

#endif /* FEATURE_UIM_LOG_TO_EFS */
    /*----------------------------------------------------------------
    ** We have timed out on a response or recd an unsolicited response
    ** or the transaction timer has expired.
    ** --------------------------------------------------------------- */
    if (((rex_signals_mask & UIM_CMD_RSP_TIMEOUT_SIG) != 0) ||
        ((rex_signals_mask & UIM_UNSOLICITED_RSP_SIG) != 0) ||
        ((rex_signals_mask & UIM_TRANSACTION_SIG)     != 0))
    {

      /* Clear the transaction timer */
      (void) rex_clr_timer( &uim_transaction_rsp_timer );
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

#ifdef UIM_DRIVER_TIMESTAMP
      /* How ever, we are here with a timeout signal, let us turn off
       * the driver timeout mechanism
       */
      uim_is_timer_on = FALSE;
#endif
      if ((rex_signals_mask & UIM_UNSOLICITED_RSP_SIG) != 0)
      {
        (void) rex_clr_sigs( &uim_tcb, UIM_UNSOLICITED_RSP_SIG );
        ERR ("Received un-solicited byte", 0, 0, 0);
      }
      else
      {
        ERR ("Timed out on the command response", 0, 0, 0);
        (void) rex_clr_sigs( &uim_tcb, UIM_CMD_RSP_TIMEOUT_SIG );
        (void) rex_clr_sigs( &uim_tcb, UIM_TRANSACTION_SIG );
      }

#if defined( FEATURE_UIM_T_1_SUPPORT )
      if (UIM_T_1_PROTOCOL == uim_current_protocol)
      {
        /* For T=1, the handling of CWT timeout does not lead to
           power-down of the SIM interface always. */
        clear_timeout_on_response = TRUE;

        if ( uim_can_ignore_timeout(rex_signals_mask) )
        {
          /* This is a spurious timeout signal.  We have bytes in the FIFO
             Return from this point as though nothing happened */
          MSG_HIGH("Ignoring the spurious timeout signal for T=1",0,0,0);
          continue;
        }

        /* Process the timeout for the T=1 protocol */
        if (uim_process_t_1_timeout())
        {
          /* The timeout has been processed.  No need to process it
             further here */
          continue;
        } /* end if - the timeout has been processed by T=1 code */
      } /* end if - operating in the T=1 protocol */
#endif /* FEATURE_UIM_T_1_SUPPORT */

      if ( uim_max_parity_error   ||
           uim_max_rx_break_error ||
           uim_max_overrun_error )
      {
        /* We need to reset the interface so clear the values
         * for these error conditions since they would no longer
         * be right and could interfere in the sub-sequent time-out
         */
        uim_overrun_error          = FALSE;
        uim_bad_status_words_error = FALSE;
      }
      /* If an overrun error is encountered or bad status words
       * are received
       */
      if ( uim_overrun_error || uim_bad_status_words_error )
      {
        if ( uim_overrun_error )
        {
          MSG_ERROR("Processing UART Overrun Error in UIM task",0,0,0);
        }
        if ( uim_bad_status_words_error )
        {
          MSG_ERROR("Processing reception of bad status words",0,0,0);
        }

        /* Process the status as a command error */
        uim_process_cmd_err(cmd_ptr, &i_mask);

        /* Set the flag to FALSE */
        uim_overrun_error          = FALSE;
        uim_bad_status_words_error = FALSE;
      }
      else
      {
        /* Reset and try to recover from these error cases */
        if ( uim_max_parity_error   ||
             uim_max_rx_break_error ||
             uim_max_overrun_error)
        {
          if (uim_max_parity_error)
          {
            MSG_ERROR("Reset due to Parity error",0,0,0);
          }
          else if (uim_max_rx_break_error)
          {
            MSG_ERROR("Reset due to rx break errors",0,0,0);
          }
          else if (uim_max_overrun_error)
          {
            MSG_ERROR("Reset due to overrun errors",0,0,0);
          }
          /* Clear all the flags irrespective of which one
           * got us here
           */
          uim_max_parity_error   = FALSE;
          uim_max_rx_break_error = FALSE;
          uim_max_overrun_error  = FALSE;
        }
        else if ( uim_can_ignore_timeout(rex_signals_mask) )
        {
          /* This is a spurious timeout signal.  We have bytes in the FIFO
             Return from this point as though nothing happened */
          MSG_HIGH("Ignoring the spurious timeout signal for T=0",0,0,0);
          continue;
        }

        /* Any timeout while in passive mode would be reported to the client
         * both internal or external commands
         */

        if (uim_mode == UIM_PASSIVE_MODE)
        {
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
          uim_power_down();

          /* Set the flag to false since the command is completed */
          uim_command_in_progress = FALSE;

          /* set the status to ERROR */
          uim_status = UIM_ERR_S;

          /* Clear out any votes to keep UIM powered on */
          uim_ctl = UIM_NONE;
          uim_powerdown = TRUE;

          /* internal command timed-out for max attempts */

          /*Send the Error Report*/
          if(!uim_static_buffer_used && cmd_ptr != NULL)
          {
            uim_send_err_rpt(cmd_ptr);
          }

          /* flush all the commands in the command queue */
          uim_flush_command(UIM_FLUSH_ALL);

          /* Notify the higher layers that UIM is declared faulty */
          uim_notify_error(UIM_CMD_TIMED_OUT_IN_PASSIVE_MODE_S);


          /* Whenver we receive a command, send an error report as the
          SIM is to be powered down */
          i_mask |= UIM_CMD_Q_SIG;

          /* reset the directory struct and power down to UIM */
          uim_reset_dir();
#ifdef FEATURE_UIM_SUPPORT_CSIM
          uim_reset_csim_dir();
#endif /* FEATURE_UIM_SUPPORT_CSIM*/

          /* Reset the static buffer used flag */
          uim_static_buffer_used = FALSE;

          /* Reset the static command count */
          uim_static_cmd_ptr->hdr.cmd_count = 0;
#else
          MSG_MED("Setting UIM POLL timer after a timeout", 0, 0, 0 );
          uim_set_poll_timer( uim_poll_time_delay );

          /* Set the status for the respective slot */
          uim_set_uim_status_for_slot (UIM_ERR_S, uim_static_cmd_ptr->hdr.slot);

          /* Power down the slot */
          uim_power_down (uim_static_cmd_ptr->hdr.slot);
          MSG_MED ("Powering down Slot %d",uim_static_cmd_ptr->hdr.slot,0,0);

          if ((uim_status == UIM_ERR_S) && (uim_status_2 == UIM_ERR_S))
          {
            /* Process power down for both the slots */
            uim_process_error_for_dual_slots(&i_mask);
          }
          else
          {
            /* If required power up the second slot */
            uim_process_slot_powerup (&i_mask, uim_static_cmd_ptr);
          }
#endif
        }
        else if ((uim_static_buffer_used) && uim_static_cmd_ptr->hdr.command == UIM_INTERNAL_FETCH_F)
        {
          MSG_HIGH("Ignore timeout on an internal fetch", 0,0,0);

          //reset all the flags and include the imask
          i_mask |= UIM_CMD_Q_SIG;
          /* Set the flag to false since the command is completed */
          uim_command_in_progress = FALSE;
          /* Reset the internal command used flag */
          uim_internal_command_used = FALSE;
          /* Reset static buffer used*/
          uim_static_buffer_used = FALSE;
          uim_command_requeued = FALSE;

        }
        /* If we are in the middle of processing an external command */
        else if ((uim_internal_command_used) || (!uim_static_buffer_used))
        {
          /* Log as much information as possible for postmortem analysis */
          (void)uim_log_timeout_info(cmd_ptr, uim_static_cmd_ptr, i_mask,
                                     uim_get_uart_status());
          if (cmd_ptr != NULL)
          {
            /* This makes sure that we do not send the status again to
               the task that sent the command */
            cmd_ptr->hdr.cmd_hdr.task_ptr = NULL;

            /* If an internal command is used, an external command was already
               put back in the head of the queue */
            if (!uim_internal_command_used && cmd_ptr->hdr.command != UIM_RESET_F)
            {
              uim_command_requeued = TRUE;

              /* Put the command back in the head of the queue so that it can be
                processed again */
              if ( q_cnt( &uim_cmd_q ) == 0 )
              {
                q_put( &uim_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );
              }
              else
              {
#ifdef FEATURE_Q_NO_SELF_QPTR
                q_insert( & uim_cmd_q,
                          & cmd_ptr->hdr.cmd_hdr.link,
                          & ((uim_cmd_type*)q_check( &uim_cmd_q ))->hdr.cmd_hdr.link);
#else
                q_insert( & cmd_ptr->hdr.cmd_hdr.link,
                          & ((uim_cmd_type*)q_check( &uim_cmd_q ))->hdr.cmd_hdr.link);
#endif /* FEATURE_Q_NO_SELF_QPTR */
              }
            }
            else
            {
              MSG_HIGH("UIM Command Re-queue did not happen for cmd = 0x%x",
                        cmd_ptr->hdr.command,0,0);
            }

            /* Increment the number of times the command is executed
             * instead of flushing right away, it would be flushed when it is
             * taken out of the que for processing to avoid giving an error
             * response while in recovery mode
             */
            ++cmd_ptr->hdr.cmd_count;

            /* The command that has timed-out is a recovery command.  Just
             * throw it away since we would be giving a reset signal any way
             */
            if ( (int)(cmd_ptr->hdr.options) & (int)(UIM_OPTION_RECOVERY_CMD) )
            {
              uim_cmd_type *tmp_cmd_ptr;

              /* This command has been completed */
              uim_command_in_progress = FALSE;

              /* Reset the command re-queued flag */
              uim_command_requeued = FALSE;

              /* reset the internal command used flag */
              uim_internal_command_used = FALSE;

              /* Throw away this recovery command */
              tmp_cmd_ptr = q_get( &uim_cmd_q );

              /* If the command is from the free command pool,
               * return it back to that queue
               */
              if (tmp_cmd_ptr->hdr.cmd_hdr.done_q_ptr)
              {
                q_put(tmp_cmd_ptr->hdr.cmd_hdr.done_q_ptr,
                      &tmp_cmd_ptr->hdr.cmd_hdr.link);
              }
            } /* cmd is a recovery command */
          }  /* cmd_ptr != NULL */
          else
          {
            MSG_HIGH("Command Pointer is Null",0,0,0);
          }

          /* reset the card and we do not want to include the command q sig */
          MSG_HIGH("Reset after timeout Rx-state %d Tx-state %d",
                   uim_rx_state[uim_drv_slot], uim_tx_state[uim_drv_slot], 0);

          uim_reset_uim (uim_static_cmd_ptr, &i_mask, uim_me_powerup);
        }  /* if it was an internal command */
        else
        {
          /* If we have received an ATR and are timing out for any of the
             commands in the power up sequence, check if USIM instruction
             class */
          if (uim_atr_pps_done)
          {
            if ((uim_static_cmd_ptr->hdr.command ==
                 UIM_INTERNAL_ME_PUP_F)
#ifndef FEATURE_UIM_NO_SWITCH_INST_ON_WWT_EXPIRY
#ifdef FEATURE_UIM_UICC
                && (uim_static_cmd_ptr->hdr.cmd_count == 1)
                && ((uim_first_inst_class == NV_UIM_FIRST_INST_CLASS_UMTS_SIM)
                  #ifdef FEATURE_UIM_USB_UICC
#error code not present
                  #endif /* FEATURE_UIM_USB_UICC */
                   )
#if defined( FEATURE_UIM_T_1_SUPPORT )
                && (UIM_T_1_PROTOCOL != uim_current_protocol)
#endif /* FEATURE_UIM_T_1_SUPPORT */
#endif  /* FEATURE_UIM_UICC */
#endif
               )
#ifndef FEATURE_UIM_NO_SWITCH_INST_ON_WWT_EXPIRY
            {
#ifdef FEATURE_UIM_UICC
              /* Change the instruction class to GSM. */
              uim_toggle_instrn_class = TRUE;
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
              /* Power down the interface to force a cold reset */
              uim_power_down();
#else
              uim_power_down(uim_static_cmd_ptr->hdr.slot);
              MSG_MED ("Powering down Slot %d",uim_static_cmd_ptr->hdr.slot,0,0);
#endif
#endif /* FEATURE_UIM_UICC */
            }
            else
#endif /* !FEATURE_UIM_NO_SWITCH_INST_ON_WWT_EXPIRY*/
            {
              /* Log as much information as possible for postmortem analysis */
              (void)uim_log_timeout_info(cmd_ptr, uim_static_cmd_ptr, i_mask,
                                                        uim_get_uart_status());

              /* If an internal wake-up timed-out let it re-try */
              if ( uim_static_cmd_ptr->hdr.command == UIM_INTERNAL_ME_PUP_F )
              {
                /* Make sure the link is declared as bad */
                uim_static_cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
              }
              else if ( uim_static_cmd_ptr->hdr.command == UIM_INTERNAL_POLL_F )
              {
#ifndef FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS
                if ( !uim_config_params.uim_recover_poll_t_out_while_in_call &&
                     ((int)uim_ctl & UIM_TRAF_CHAN) )
                {
                  /* Make sure the link is declared as bad if we are in traffic
                   * and the command is an internal poll and the
                   * configuration parameter is set to do so
                   */
                  uim_static_cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
                } /* traf chan */
                else
                {
                  /* Set the timer to poll */
                  uim_set_poll_timer( uim_poll_time_delay );
                }
#else
                /* Make sure the link is declared as bad */
                uim_static_cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
#endif /* FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS */
              } /* Internal poll */
            }
          } /*atr pps done */
#if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES )
          else if (!uim_atr_received)/* No ATR received */
          {
            /* Check if there is a higher voltage class */
            if (UIM_MAX_VOLTAGE == uim_current_voltage_class[uim_drv_slot])
            {
              /* Stop interfacing with the card */
              uim_static_cmd_ptr->hdr.cmd_count = UIM_MAX_NUM_ATTEMPTS;
            }
            else /* There are more voltages to try */
            {
              /* Power down the interface */
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
              uim_power_down();
#else
              uim_power_down(uim_static_cmd_ptr->hdr.slot);
              MSG_MED("Powering down slot %d",uim_static_cmd_ptr->hdr.slot,0,0);
#endif
              rex_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY);
              /* Try the next voltage class */
              uim_current_voltage_class[uim_drv_slot]++;
              /* Reset the error count */
              uim_static_cmd_ptr->hdr.cmd_count = 0;
            } /* end if - there are no more voltages to try */
          } /* No ATR received */
#endif /* FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES */
          if (++uim_static_cmd_ptr->hdr.cmd_count > UIM_MAX_NUM_ATTEMPTS)
          {
            if (uim_atr_pps_done)
            {
              /* internal command failed after the PPS procedure */
              notification_reason = UIM_INTERNAL_CMD_TIMED_OUT_AFTER_PPS_S;
            }
            else if ( !uim_atr_received )
            {
              /* We have not yet received the ATR */
              if ( uim_static_cmd_ptr->hdr.command == UIM_INTERNAL_ME_PUP_F )
              {
                notification_reason = UIM_NO_ATR_RECEIVED_WITH_MAX_VOLTAGE_S;
              }
              else
              {
                notification_reason = UIM_NO_ATR_RECEIVED_AFTER_INT_RESET_S;
              }
            }
            else
            {
              /* Atr is received but PPS procedure is not done and we are done
               * with all the re-tries
               */
              notification_reason = UIM_PPS_TIMED_OUT_MAX_TIMES_S;
            }

#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
            uim_power_down();

            /* Set the flag to false since the command is completed */
            uim_command_in_progress = FALSE;

            /* set the status to ERROR */
            uim_status = UIM_ERR_S;

            /* Clear out any votes to keep UIM powered on */
            uim_ctl = UIM_NONE;
            uim_powerdown = TRUE;

            /* flush all the commands in the command queue */
            uim_flush_command(UIM_FLUSH_ALL);

            /* Notify the higher layers that UIM is declared faulty */
            uim_notify_error(notification_reason);

            /* Whenver we receive a command, send an error report as the
            SIM is to be powered down */
            i_mask |= UIM_CMD_Q_SIG;

            /* reset the directory struct and power down to UIM */
            uim_reset_dir();
#ifdef FEATURE_UIM_SUPPORT_CSIM
            uim_reset_csim_dir();
#endif /* FEATURE_UIM_SUPPORT_CSIM*/

            /* Reset the static buffer used flag */
            uim_static_buffer_used = FALSE;

            /* Reset the static command count */
            uim_static_cmd_ptr->hdr.cmd_count = 0;
#else
            MSG_MED("Setting UIM POLL timer after a timeout", 0, 0, 0 );
            uim_set_poll_timer( uim_poll_time_delay );

            /* Set the status for the respective slot */
            uim_set_uim_status_for_slot (UIM_ERR_S, uim_static_cmd_ptr->hdr.slot);

            /* Power down the slot */
            uim_power_down (uim_static_cmd_ptr->hdr.slot);
            MSG_MED ("Powering down Slot %d",uim_static_cmd_ptr->hdr.slot,0,0);

            if ((uim_status == UIM_ERR_S) && (uim_status_2 == UIM_ERR_S))
            {
              /* Process power down for both the slots */
              uim_process_error_for_dual_slots(&i_mask);
            }
            else
            {
              /* If required power up the second slot */
              uim_process_slot_powerup (&i_mask, uim_static_cmd_ptr);
            }
#endif
          }
          else
          {
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
            /* Power down the interface to force a cold reset */
            uim_power_down();
#else
            uim_power_down(uim_static_cmd_ptr->hdr.slot);
            MSG_MED("Cold reset on slot %d",uim_static_cmd_ptr->hdr.slot,0,0);
#endif
            rex_sleep(UIM_ISO7816_VOLTAGE_SWITCH_PWR_DOWN_DELAY);
            MSG_HIGH("Cold Reset after timeout Rx-state %d Tx-state %d",
                     uim_rx_state[uim_drv_slot], uim_tx_state[uim_drv_slot], 0);
            uim_reset_uim (uim_static_cmd_ptr, &i_mask, uim_me_powerup );
          }
        }  /* internal or external command */
      } /* uim_overrun_error */
    } /* end if ((rex_signals_mask & UIM_CMD_RSP_TIMEOUT_SIG) != 0) */

    /*-------------------------
    ** Process the pending poll
    ** -----------------------*/
    if (uim_poll_pending)
    {
      /* Do not allow the poll while a command is in progress are
       * recovery is in process
       */
      if (!uim_command_in_progress && (uim_mode != UIM_RECOVERY_MODE))
      {

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
        /* Set the desired slot for polling */
        uim_static_cmd_ptr->hdr.slot = uim_polling_slot;

        if (uim_polling_slot == UIM_SLOT_1)
        {
          uim_temp_status = uim_status;
#ifdef FEATURE_UIM_TOOLKIT
          uim_temp_type = uim_type;
#endif
        }
        else if (uim_polling_slot == UIM_SLOT_2)
        {
          uim_temp_status = uim_status_2;
#ifdef FEATURE_UIM_TOOLKIT
          uim_temp_type = uim_type_2;
#endif
        }
        else
        {
          uim_temp_status = UIM_ERR_S;
        }
#else
        uim_temp_status = uim_status;
#ifdef FEATURE_UIM_TOOLKIT
        uim_temp_type = uim_type;
#endif
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

        if ( (uim_mode == UIM_PASSIVE_MODE) &&
             ((int)uim_ctl & (int)UIM_PASSIVE) != (int)UIM_PASSIVE )
        {
          /* Restart the poll timer */
          uim_set_poll_timer( uim_poll_time_delay );

          /* Reset the flag */
          uim_poll_pending = FALSE;

          MSG_MED( "Protocol un-known in passive mode re-set poll timer", 0, 0, 0);
        }
        else if (uim_temp_status == UIM_INITIALIZED_S)
        {
          /* Poll the UIM only if we are already initialized */
          uim_poll(uim_static_cmd_ptr, &i_mask);
        }
        else if (uim_temp_status == UIM_CLK_OFF_S)
        {
          if ((uim_ctl != UIM_NONE)
#ifdef FEATURE_UIM_TOOLKIT
              || (uim_temp_type.init && uim_temp_type.proactive_uim)
#endif
             )
          {
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
            uim_stop_power_save(uim_polling_slot);
            uim_set_uim_status_for_slot(UIM_INITIALIZED_S, uim_polling_slot);
#else
            /* Depending on whether the ISO or USB interface is selected
               either provide clock to UIM or resume USB interface */
            uim_stop_power_save();

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
            {
              MSG_MED ("Clock started",0,0,0);
            }

            /* Set the status */
            uim_status = UIM_INITIALIZED_S;
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

            /* Poll the card */
            uim_poll(uim_static_cmd_ptr, &i_mask);
          } /* if the card is a pro-active card */
          else
          {
            /* Restart the poll timer */
            uim_set_poll_timer( uim_poll_time_delay );

            /* Reset the flag */
            uim_poll_pending = FALSE;

            MSG_MED( "Setting UIM Poll timer for non pro-active scenario", 0, 0, 0);
          }
        } /* status is CLK_OFF_S */
        else if (uim_temp_status == UIM_ERR_S)
        {
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
          /* Restart the poll timer if both the status are not errors*/
          if ((uim_status != UIM_ERR_S) || (uim_status_2 != UIM_ERR_S))
          {
            uim_set_poll_timer( uim_poll_time_delay );
          }
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */

          uim_poll_pending = FALSE;

          MSG_MED( "Clearing poll_pending flag as polling slot is invalid", 0, 0, 0);
        }
      }
      else
      {
        MSG_MED( "Not processing the pending poll as cmd is in progress", 0, 0, 0);
      }
    } /* uim_poll_pending */

    /*---------------------------------------------------------------
    ** Check to see if UIM can be powered down or the clock stopped
    ** --------------------------------------------------------------*/
    /* if the uim status is not powered down and is not faulty and the clock
       is not turned OFF to the UIM */
#ifndef FEATURE_UIM_SUPPORT_DUAL_SLOTS
    if (uim_powering_down_task || ((uim_status != UIM_POWERED_DOWN_S)
                                   && (uim_status != UIM_ERR_S)
                                   && (uim_status != UIM_CLK_OFF_S)
                                  ))
    {
      /* if we got here bcos of the signal being set */
      if ((rex_signals_mask & UIM_STATE_TOGGLE_SIG) != 0)
      {
        /* clear the signal */
        (void) rex_clr_sigs(&uim_tcb, UIM_STATE_TOGGLE_SIG);
        /* if queue is empty and we are voted to power down */
      }
      /* We would like to stop the clock to the UIM / power down if the below
         conditions are met
         a. There is no command in the command queue.
         b. All the required tasks have voted to power down the UIM.
         c. Current command has been completed  */
      if (!uim_command_in_progress)
      {
        if (( !uim_powering_down_task )
#if defined ( FEATURE_UIM_TOOLKIT ) && !defined ( FEATURE_UIM_UICC ) && \
    !defined (FEATURE_UIM_UTIL) && !defined (FEATURE_UIM_SUPPORT_3GPD)
            && ( uim_type.init && uim_type.proactive_uim )
#endif /* FEATURE_UIM_TOOLKIT && !FEATURE_UIM_UICC && ! FEATURE_UIM_UTIL && !FEATURE_UIM_SUPPORT_3GPD */
           )
        {
          if(q_cnt(& uim_cmd_q) == 0)
          {
            /* Depending on whether the ISO or USB interface has been selected,
               remove the clock to the UIM or suspend the ICCD interface */
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
            {
              if (uim_start_power_save())
              {
                /* Change the status if clock stop was successful */
                uim_status = UIM_CLK_OFF_S;
                MSG_MED ("Clock stopped",0,0,0);
              } /* Clock stop */
            } /* Interface selection */
          } /* Q CNT == 0 */
        }
        else
        {
          /* Check if the UIM interface is allowed to be powered down
             or if the interface must be powered down */
          if (uim_powerdown || uim_powering_down_task)
          {
            uim_reset_dir();
#ifdef FEATURE_UIM_SUPPORT_CSIM
            uim_reset_csim_dir();
#endif /* FEATURE_UIM_SUPPORT_CSIM*/
            uim_power_down();
            if (uim_powering_down_task)
            {
              /* Ensure the link is never used again by setting the status to
                 ERR */
              uim_status = UIM_ERR_S;
#ifdef FEATURE_MULTIMODE_ARCH
              /* Process task stop procedure.*/
              task_stop();
#else
              (void) rex_set_sigs( &mc_tcb, MC_ACK_SIG );
#endif /* FEATURE_MULTIMODE_ARCH */
               /* Let us not be here time and again */
               uim_powering_down_task = FALSE;
            } /* power down the card */
          } /* The interface is allowed to be powered down */
        } /* clock can be stopped */
      } /* cmd count is zero */
    } /* check for powering down / stopping the clock to the card */
#else
    /* if we got here bcos of the signal being set */
    if ((rex_signals_mask & UIM_STATE_TOGGLE_SIG) != 0)
    {
      /* clear the signal */
      (void) rex_clr_sigs(&uim_tcb, UIM_STATE_TOGGLE_SIG);
      /* if queue is empty and we are voted to power down */
    }

    /* If there is no command to process */
    if ((q_cnt(& uim_cmd_q) ==0) && (!uim_command_in_progress))
    {
      /* If the task is powering down */
      if (uim_powering_down_task)
      {
        uim_process_power_down_for_dual_slots();
      }
      else
      {
        uim_process_clock_stop_for_dual_slots();
      }
    }
#endif /* FEATURE_UIM_SUPPORT_DUAL_SLOTS */
  } /* end for (;;) */
} /* end uim_task */

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_CMD

DESCRIPTION
  The uim_cmd pointed to by the parameter passed in is queued up for UIM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sets the UIM_CMD_Q_SIG.  If passed a null cmd_ptr, the caller will
  be stuck waiting for response
===========================================================================*/
void uim_cmd (
             uim_cmd_type *cmd_ptr      /* the command to queue up */
             )
{
  if (cmd_ptr == NULL)
  {
    MSG_ERROR("uim_cmd cmd_ptr is NULL",0,0,0);
    return;
  }

  if (!UIM_IS_CMD_VALID_IN_MODE(cmd_ptr) )
  {
      UIM_ERROR_MSG("Un expected command while UIM in passive mode",0,0,0);
  }

#ifdef FEATURE_UIM_SESSION_MANAGEMENT
  if (cmd_ptr->hdr.channel >= UIM_MAX_CHANNELS)
  {
    ERR_FATAL("Received invalid channel ",
                  cmd_ptr->hdr.channel,0,0);
  }
#else
  cmd_ptr->hdr.channel = UIM_DEFAULT_CHANNEL;
#endif

  /* Always initialize the count to zero */
  cmd_ptr->hdr.cmd_count = 0;

  /* init link */
  ( void )q_link( cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link );

  /* and queue it */
  q_put( &uim_cmd_q, &cmd_ptr->hdr.cmd_hdr.link );

  /* and signal the UIM task */
  (void)rex_set_sigs( &uim_tcb, UIM_CMD_Q_SIG );
}

/* <EJECT> */

/*===========================================================================

FUNCTION UIM_RECOVERY_CMD

DESCRIPTION
  The uim_cmd pointed to by the parameter passed in is queued up for UIM
  as a recovery command after placing it at the head of the command queue.

DEPENDENCIES
  uim_mode should already be in UIM_RECOVERY_MODE

RETURN VALUE
  uim_return_type indicating success/failure/reason

SIDE EFFECTS
  Sets the UIM_CMD_Q_SIG.
===========================================================================*/
uim_return_type uim_recovery_cmd(
  uim_cmd_type *cmd_ptr /* the command to queue up */
)
{
  MSG_HIGH("Received a recovery command", 0, 0, 0);
  if (uim_mode != UIM_RECOVERY_MODE)
  {
    ERR_FATAL("UN-Expected recovery command",0,0,0);
  }

  if (cmd_ptr == NULL)
  {
    MSG_ERROR("cmd_ptr is NULL", 0 ,0 ,0);
    return UIM_INVALID_PARAMS;
  }

  /* Always initialize the count to zero */
  cmd_ptr->hdr.cmd_count = 0;

  /* Stamp this command to be a recovery command */
  cmd_ptr->hdr.options = (uim_option_type)
  ((int)cmd_ptr->hdr.options | (int)UIM_OPTION_RECOVERY_CMD);

  /* init link */
  ( void )q_link( cmd_ptr, &cmd_ptr->hdr.cmd_hdr.link );

  if ( q_cnt( & uim_cmd_q ) == 0 )
  {
    q_put( & uim_cmd_q, & cmd_ptr->hdr.cmd_hdr.link );
  }
  else
  {
#ifdef FEATURE_Q_NO_SELF_QPTR
    q_insert( & uim_cmd_q,
              & cmd_ptr->hdr.cmd_hdr.link,
              & ((uim_cmd_type*) q_check( & uim_cmd_q ))->hdr.cmd_hdr.link );
#else
    q_insert( & cmd_ptr->hdr.cmd_hdr.link,
              & ((uim_cmd_type*) q_check( & uim_cmd_q ))->hdr.cmd_hdr.link );
#endif /* FEATURE_Q_NO_SELF_QPTR */
  }

  /* signal the UIM task */
  (void)rex_set_sigs( &uim_tcb, UIM_CMD_Q_SIG );

  return UIM_SUCCESS;
}

/*===========================================================================

FUNCTION UIM_MALLOC

DESCRIPTION
  The uim_malloc called TMC alloc with different heap_size based the size
  of the allocation required.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  a pointer is allocated.
===========================================================================*/
void * uim_malloc(dword size)
{
#if !defined(FEATURE_LOW_MEMORY_USAGE) && !defined(PLATFORM_LTK)
  if (size <= tmc_get_mem_heap_size(TMC_MEM_HEAP_TYPE_THRD))
  {
    return mem_malloc(&tmc_heap_small, size);
  }
  else
  {
    return mem_malloc(&tmc_heap, size);
  }
#else
  return mem_malloc(&tmc_heap, size);
#endif
}

/* <EJECT> */
/*===========================================================================

FUNCTION UIM_FREE

DESCRIPTION
  The uim_free free the pointer from the regular tmc_heap or the tmc_heap_small
  based on the pointer address.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  the pointer is freed.
===========================================================================*/
void uim_free(void * ptr)
{
  if (ptr == NULL)
  {
    MSG_HIGH("Attempt to free null ptr, return", 0, 0, 0);
    return;
  }
#if !defined(FEATURE_LOW_MEMORY_USAGE) && !defined(PLATFORM_LTK)
  if (((uint8 *)ptr >= (uint8 *) tmc_heap_mem_buffer) &&
      ((uint8 *)ptr < ((uint8 *)tmc_heap_mem_buffer +
      tmc_get_mem_heap_size(TMC_MEM_HEAP_TYPE_BUF))))
  {
    mem_free(&tmc_heap, ptr);
  }
  else
  {
    mem_free(&tmc_heap_small, ptr);
  }
#else
  mem_free(&tmc_heap, ptr);
#endif /* FEATURE_LOW_MEMORY_USAGE */
}


/*===========================================================================

FUNCTION UIM_GET_MODE

DESCRIPTION
  This function returns the UIM Mode.

DEPENDENCIES

RETURN VALUE
  uim_mode_type indicating the current mode

SIDE EFFECTS
===========================================================================*/
uim_mode_type uim_get_mode(void)
{
  return uim_mode;
}


/*===========================================================================

FUNCTION UIM_SAFE_TO_FREE

DESCRIPTION
  This function takes a pointer to a tmc allocated piece of memory, and checks
  to see whether it is on the uim_cmd_q.  If it is, it is not considered safe
  to free.

DEPENDENCIES

RETURN VALUE
  FALSE if not safe to free (on the command q), TRUE if safe

SIDE EFFECTS
===========================================================================*/
boolean uim_safe_to_free(const void* cmd_ptr)
{
  uim_cmd_type * item_ptr = NULL;
  int q_counter = 0;
  boolean safe_to_free = TRUE;
  TASKLOCK();
  q_counter = q_cnt(&uim_cmd_q);
  item_ptr = q_check(&uim_cmd_q);

  while((q_counter > 0) && (item_ptr != NULL))
  {
    if(cmd_ptr == item_ptr)
    {
      safe_to_free = FALSE;
      break;
    } /* end if */
    q_counter--;
    item_ptr = q_next(&uim_cmd_q, &(item_ptr->hdr.cmd_hdr.link));
  } /* end while */
  TASKFREE();
  return safe_to_free;
} /* end uim_safe_to_free */


/*===========================================================================

FUNCTION UIM_REMOVE_COMMAND_FROM_UIM_CMD_Q

DESCRIPTION
  Checks to see if a pointer to a command is on the uim_cmd_q, and if it is,
  removes it.  This is not a function to be used widely, but is used if a
  a client (mmgsdi) has abandoned this command as a presumed failure, and
  wants to deallocate the memory associated with this command.  This procedure
  is used to prevent a corrupted uim_cmd_q in the event that mmgsdi deallocates
  a command which uim still intends to process.  It mostly will happen with
  bad UIM HW, where there is a reoccuring recovery scenario, and UIM and MMGSDI
  are having trouble keeping their states synched due to constant resets.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
===========================================================================*/
void uim_remove_command_from_uim_cmd_q(const uim_cmd_type* cmd_ptr)
{
  uim_cmd_type * item_ptr = NULL;
  int q_counter = 0;
  TASKLOCK();
  q_counter = q_cnt(&uim_cmd_q);
  item_ptr = q_check(&uim_cmd_q);

  while((q_counter > 0) && (item_ptr != NULL))
  {
    if(cmd_ptr == item_ptr)
    {
        /* Item found in queue */
#ifdef FEATURE_Q_NO_SELF_QPTR
       q_delete  (&uim_cmd_q, &(item_ptr->hdr.cmd_hdr.link));
#else
       q_delete  (&(item_ptr->hdr.cmd_hdr.link));
#endif
       break;
    } /* end if item not found */
    q_counter--;
    item_ptr = q_next(&uim_cmd_q, &(item_ptr->hdr.cmd_hdr.link));
  } /* end while */
  TASKFREE();
}/* end uim_remove_command_from_uim_cmd_q */


#endif /* FEATURE_UIM */
