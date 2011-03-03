/*---------------------------------------------------------------------------
 The includes below are put outside all compiler switches to avoid
 compilation warnings.
---------------------------------------------------------------------------*/

#include "intconv.h"
#include "customer.h"              /* Customer Specific Definitions        */

#ifdef FEATURE_UIM_DRIVER
#ifdef FEATURE_UIM_HANDLE_NO_ATR_IN_40000_CLK_CYCLES
  #include "ts.h"
#endif /*FEATURE_UIM_HANDLE_NO_ATR_IN_40000_CLK_CYCLES*/
#include "task.h"                  /* Access to uim_tcb                    */
#include "target.h"                /* Target specific definitions          */
#include "comdef.h"                /* Definitions for byte, word, etc.     */
#if defined( T_MSM31 )             /* MSM3100 or MSM3300 */
#include "tlmm.h"                  /* Top level mode MUX                   */
#endif /* MSM3100 or MSM3300 */

#ifdef FEATURE_UIM_DEBUG_LOG
#include "log.h"
#endif /* FEATURE_UIM_DEBUG_LOG */

#include "uimgen.h"                /* To access uim_gen APIs */

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                             U I M  D R I V E R

                       User Identity Module Low Level Driver

GENERAL DESCRIPTION

  This module contains the device driver for the UIM.  It handles the low
  level hardware interface to a UIM.

EXTERNALIZED FUNCTIONS

  uim_device_init()
  Turns on the clock regime, places the control lines in a low state and
  selects or enables the control lines.

  uim_power_up()
  Places the control lines in the proper state in the proper order so that
  the driver is ready to reset the UIM.

  uim_reset()
  Prepares the driver for a reset then resets the UIM.

  uim_send_pps()
  Sends a PPS request to the UIM then expects a PPS response.

  uim_update_op_params()
  Changes the baud rate on the I/O line based on a frequency conversion
  factor index and a baud rate adjustment factor index.

  uim_send_command()
  Causes an APDU to be sent to the UIM then waits for a response.  The driver
  may send a "GET RESPONSE" APDU to get the response and/or may send a "FETCH"
  to get a pro-active SIM command.

  uim_power_down()
  Places the control lines in a low state in the proper order for powering
  off the UIM.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  uim_device_init() must be called prior to any of the other functions of
  this module.

Copyright (c) 1999-2010 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: O:/src/asw/COMMON/vcs/uimdrv.c_v   1.37   11 Oct 2002 10:13:30   ckrishna  $
$Header: //source/qcom/qct/modem/uim/su/baselines/qsc1110/rel/3.3.65/uim/uimdrv/src/uimdrv.c#4 $ $DateTime: 2010/02/17 07:40:30 $ $Author: rmandala $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/17/10   rm      Workaround to send card error when 
                   FEATURE_UIM_INTERFACE_NOT_USED is defined
12/22/09   yb      Fix for voltage class switching G.C.F. failure on ULC
12/22/09   yb      1. Fix to avoid clock stop during recovery
                   2. Fix to add delays before clock stop and after clock restart,
                   to give time to card for finishing any pending internal processing.
08/10/09   rm      Error handling for T=1 protocol
08/03/09   yb      Fix for setting UIM clock frequency globals to 3.4MHz for
                   MSM6290
08/03/09   yb      Fix to do get response for seek command if a GCF test case
                   is running or NV_ENS_ENABLED_I NV item is enabled.
07/27/09   rm      Add an extra guard time for UART DM
07/01/09   yb      1. Fix to do get response for envelope command when card 
                   responds with warnings.
                   2. Fix to report warnings to GSTK on receiving warnings in
                   Envelope response.
06/17/09   rm      Fix for clock power-on timing
06/15/09   rm      Fixes to correct the GPIO PULL settings for UIM_DATA and
                   UIM_CLK GPIOs
06/10/09   yb      Fix to send status word as received in get response to AT COP
                   even if card returns error for envelope sent.
06/02/09   yk      Fixed if statement conditions.
05/29/09   rm      Fixes to avoid overrun errors due to RX break errors
                   and to set the command response timeout signal appropriately
                   in such cases
05/11/09   rm      Fix to correct the feature macro name
05/04/09   rm      Fix in parity error handling logic for T=1 protocol
05/02/09   ps      Fix 8200 Warnings
04/14/09   yk      Flushing the FIFO for LCU in case of overrun errors
04/13/09   yk      Enable error mode in UART_DM for T=1
04/07/09   nb      Fix for multiple select on MF
03/30/09   rm      Reset the UART received upon receipt of each RX break error
                   interrupt
03/25/09   rm      Fix for APDU logging leading to a crash
03/24/09   rm      Fix for improper featurization for TD5 macros
03/11/09   nb      Fix to return received warnings when processing Envelope
                   command
03/04/09   rm      Fix for slow initialization of phonebook with UART_DM
02/25/09   yk      Bring MDM8200 drivers
02/25/09   rm      Fix for RX break errors in Idle and Non-Idle
02/20/09   rm      Fix to enable the parity error logic in UART DM which
                   ignores the bytes received with parity / framing error in
                   character mode
02/06/09   ssr     Removed lint error
01/29/09   rm      Speed enhancement for slowest bit-rate card for ULC
01/16/09   rm      Fix for update record failure with UART_DM
01/13/09   jk      Support for VREG Control with FET
01/09/09   rm      Fix for a possible update record failure with UART_DM
01/06/09   yk      Bring MSM7X27 drivers
11/20/08   rm      Fix for reset issue due to a 2G sim card on LCU
11/19/08   vs      Removed sleep register call and added TLMM config for LCU
11/13/08   rm      Use timer callback for command response timer to handle the
                   work waiting timer expiry immediately in the timer context.
11/11/08   yk      Fixed Lint errors
11/05/08   vs      Update to allow UIM clock stop only after sending the first
                   instruction to the card in passive mode
10/20/08   rm      Fix the problem with NULL Procedure bytes before ACK byte
10/16/08   rm      Correcting RX and TX packet logging for UART DM
10/07/08   vs      Fix to power up UIM LDO after card power down, vote TCXO off
                   permanently when IC-USB interface is selected
09/19/08   tml     Re-select MF to force directory maintainance to MF when
                   received a sw1 warning with normal return
09/23/08   nb      Fixed compiler errors
09/22/08   rm      Fix for the authentication failure issue
09/22/08   rm      Fix for parity errors in UART_DM
09/19/08   rm      Work waiting time fix for UART_DM
09/17/08   js      Implemented UIM Card interface selection based on first
                   instruction class NV
09/12/08   nb      Added support to handle warnings returned by Envelope Command
09/05/08   jk      Fixed Nested comment which caused problems with Strip tool
08/29/08   rm      Fix for divide-by-zero error
08/20/08   ssr     Added support not to do get response for seek on warning
08/15/08   jk      Fix to UART DM featurization
08/08/08   vs      Fixed compile errors related to featurization
08/07/08   rm      Fix for speed improvement.
07/30/08   SSR     Added support not to do get response for seek on warning
07/17/08   vs      Added support for UIM USB UICC ICCD interface
07/03/08   jk      Fix to not allow GPIO 10 to be configured for 1100
06/24/08   rm      Fix to resolve merge problem
06/18/08   jk      Bring QSC11X0 drivers to mainline
06/18/08   rm      Fix to not call pm_vreg_control(PM_OFF_CMD, PM_VREG_RUIM_M)
                   when FEATURE_UIM_USE_ONLY_VREG_ON_PMIC is enabled to set
                   VREG_RUIM to 2.85V. pm_vreg_control(PM_OFF_CMD,
                   PM_VREG_USIM_M) is called via the UIM_POWER_OFF() macro.
06/16/08   rm      Fix to handle Parity errors in Block mode.
06/12/08   rm      Fix to reduce the M/N ratio when D value cannot fit into
                   the DREG and MND registers.
05/23/08   rm      TA1=0x97 feature support
05/02/08   rm      T=1 protocol support for LCU.
04/22/08   rm      Fix for LCU to handle the SIM responses correctly.
                   for all instruction cases.
04/18/08   rm      Fix in SIM powerdown procedure for LCU.
04/01/08   vs      Fixed compiler warning
03/31/08   jar     Fixed compile issue.
03/31/08   rm      Changes to:
                   1. Enable clock stop feature in LCU
                   2. Fix the handling of warning status bytes for LCU
                   3. Correct the UIM_POWER_DOWN macro for LCU
03/30/08   rm      Fix to not call MPP config APIs in the case of 6290
03/21/08   jk      Fixed Compile Warnings
03/13/08   rm/jar  Phase1 LCU UIM bring-up in UART word mode with Data Mover
                   functionality disabled.
                   Pending:
                   1. Enabling clock stop feature
                   2. NULL procedure byte handling for case 2 instructions
                   3. Card boot-up time improvisation
                   4. T=1 protocol support
03/10/08   jk      Added changes to use new SLEEP Voting API
03/05/08   jar     Allow PMIC to put VREG RUIM into LPM MODE when TXCO
                   is off under feature:  FEATURE_UIM_ENABLE_VREG_UIM_LPM
01/08/08   rm      Set UIM clock to 3.84MHz for M6280 under
                   FEATURE_UIM_3P84MHZ_M6280_PLATFORM feature macro
12/27/07   rm      8650 UIM bring up
12/06/07   rm      To fix the RX break error issues due to improper insertion
                   of SIM cards
11/29/07   rm      when no ATR with in 40K + 1 byte time, set the RX_ISR
                   state to "IDLE", reset the UART and mask all the
                   interrupts in the UART IMR register
09/10/07   tml     Add Media Flo support
09/14/07   jk      PMIC HW workaround softare support
07/20/07   jar     Added FEATURE_UIM_DRIVE_MAX_PMIC_VOLTAGE to allow the PMIC
                   to be driven to MAX of 3.05 Volts
06/26/07   jk      Correct uim_clear_timeout_on_response flag to proper name
05/21/07   jk      Reset error counters so that they don't max out again right
                   after reset
05/11/07   wli     Changes for JCDMA requirements
01/19/07   jk      SC-2X Driver bring-up
01/15/07   pv      Consider it an error if there are no more bytes to send
                   and we are processing an ACK byte.
11/30/06   pv      Fix compiler warnings and lint errors for non 7600 builds.
11/27/06   pv      Bring-up for 7600.
09/29/06   sun     Fixed Compile Error for 7600
09/25/06   pv      Lint fixes due to changes in clk_busy_wait signature.
09/13/06   pv      Fix compilation error for 6800 when FEATURE_UIM_MSM6800_TCXO_DIV5 is
                   not defined.
09/08/06   jk      Support for TCXO Div 5 clock on 6800
08/10/06   pv      Use clock regime resource manager when FEATURE_CLKREGIM_RM
                   is defined for 7200 and 7500 targets.
08/09/06   pv      Featurize the usage of members of nv_uim_config_params_type
                   under FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS for
                   targets not picking up the NV dependencies
07/31/06   pv      Do a get response irrespective of the class for CASE 4
                   commands other than for some specific instructions in
                   WIM which could return 6200 indicating warning.
                   no requirement to do a get response.
07/17/06   jk      Support for APDU logging to EFS
07/25/06   pv      Remove dependency on T_MSM6500 for 7200 targets
07/25/06   pv      Featurize API usage for pmic
07/05/06   pv      Use new api for pm_mpp_config_digital_input
06/12/06   pv      Do not leave the receiver disabled because of receiving
                   un-known status words treat it similar to overrun error.
04/24/06   pv      Use uim_config_params.additional_delay_for_atr to wait for
                   additional time after 40,000 UIM clk cycles to receive ATR
04/19/06   pv      7200 GSM bringup changes
04/11/06   pv      Implement function uim_read_status which would do the same
                   operation as the macro UIM_READ_STATUS but would release
                   the dependency on msm.h for the caller.
04/03/06   pv      Lint Fixes
03/29/06   pv      switch the order of uim_send_byte and putting the byte
                   in the diag log packet.  To avoid seeing the sent byte
                   in the same iteration.
                   Do not let a time-out while in idle state.
                   Add more debug variables to keep track of general health.
03/07/06   jk      Bring SC-1X to mainline
02/14/06   nk      Merge For: Fix to ensure we all WWT Timer Expire during RX
                   Break Error and Fake a normal response and do not send the
                   get response down to the card if requested
01/31/06   pv      Do not process rx interrupt when the clock is already
                   turned off.  Since the register reads might return
                   garbage/previous values resulting in an infinite loop
                   in the ISR.
01/18/06   tml     added support for status word 6999
12/14/05   jk      Do not set the state to IDLE after RX BREAK receiver reset
                   to allow timeout
11/23/05   jk      Added missing tlmm init to reset line not present in tlmm
11/22/05   jk      Driver Changes for 7500
11/14/05   pv      Lint Fixes
11/04/05   pv      Featurize code to timeout after 40,000 uim clk cycles
10/27/05   pv      Code to turn off the interface if no ATR is received whthin
                   40,000 UIM clock cycles
09/06/05   jk      Changes to support only allowing reading UART Status while
                   there is still power to UART
08/25/05   pv      Some 3V cards go haywire when supplied 1.8V so, always
                   set the timer if ATR is not received
07/27/05   jk      Made work waiting time array for dual slots
07/18/05   pv      Check to see if the number of bytes indicated in the
                   prologue exceed IFSD - Test case 7.3.5 on IT3 Ver 3.6
07/12/05   jk      Created Clock Regime macros for 6800 R-UIM Bring-up
07/06/05   jk      Added call to sleep_require_5ms_ticks() under
                   TCXO_MUST_BE_ON() macro
06/22/05   pv      Set the transaction timer at the beginning of a UIM
                   transaction and do not clear it untill the entire
                   transaction is done.
06/14/05   pv      Changed UIM_TRANSACTION_TIME_DURATION to 20 seconds to work with
                   cards which are taking 11.5 seconds to respond to the 5th
                   part of a multi-part SMS.
06/02/05   pv      Code for FEATURE_UIM_ISIM & FEATURE_UIM_SESSION_MANAGEMENT
05/13/05   jk      Call sleep_relinquish_5ms_rex_ticks in UIM_TCXO_CAN_BE_OFF
05/10/05   sst     Lint fixes.
04/08/05   jk      Added changes for Raven T=1 Block mode support
04/03/05   pv      Changes to set the stop bit configuration for Ravem
03/18/05   pv      Changes for Jaguar dual-slot multiplevoltage support
03/18/05   pv      Clear the work waiting timer at power down.
02/23/05   pv      Set the timer just after doing a reset receiver because of
                   an rx-break error. Fix compile error when T=1 is turned off
                   for Raven.
02/02/04   pv      Do the max parity error checking only for T=0 protocol
01/26/05   pv      Removed uart_reset_receiver.  This was causing infinite
                   parity errors.  Instead just reset the error status.
01/27/05   jar     Fixed runtime bug due to missing break in switch-case
01/12/05    jk     Added code to process Response properly for Global Platform/WIM
                   cards
12/28/04   pv      Added code to take time stamps and signal a timeout in order
                   to pass the USIM 5 etu WWT exceeded test case
12/27/04   jk      Fixed Compiler warnings.
12/10/04   pv      Cr.Fix 53534.  We need to send a get response inspite of
                   receiving warning message for case 4 commands.
12/08/04   pv      Raven merged to main-line
12/06/04   pv      Externalised variables required for logging timeout info
11/23/04   pv      Code to select appropriate clock source for 6550 based on
                   MSM revision (To avoid data aborts when
                   FEATURE_UIM_GSM_CLK_SRC is selected for 6550 MSM 1.1)
11/21/04   wli     Dual slot merge and debug.
11/15/04   pv      Added definition for UART_TFWR_TX_RX_MODE_MASK to be used to
                   set the UART to block mode for T=1 protocol parity error
                   handling
10/29/04  pv/jar   Removed calling pm_vreg_pull_down_switch(ON) from u
                   uim_dev_init().
09/20/04   pv      Added code to disable the interrupts after maxing errors
                   in order to avoid race conditions
09/01/04   ck      Added support for CSIM.
08/31/04   ck      Fixed a bug where the last sent APDU was re-sent internally
                   by driver with wrong instrn case that resulted in timeout.
08/18/04   pv      Changed uim_clk_source_freq to 26000000 since we are dividing
                   by 8 and the uim clock frequency is 3.25 MHZ
08/12/04   ck      Added delay before sending the command header to the card
                   in the function uim_send_command().
08/10/04   pv      Removed code which re-writes to the waterlevel and
                   added more messages
07/19/04   pv      Moved some definitions to driver header file
06/09/04   ts      Clear the timeout signal when processing a character in
                   case ISR has not run since timeout.
06/03/04   ts      Added a call to configure the UIM GPIOs during
                   initialization for Saber.
05/11/04   ts      Pull down Vreg R-UIM and wait 2ms when powering down.
05/11/04   ts      Added support for controlling UIM power through PMIC Vreg.
05/11/04   ts      Added support for direct connect on Saber.
04/16/04   ts      Fixed problems with error condition processing due to bad
                   merge.
03/26/04   ts      Added a timer to determine when excessive null procedure
                   bytes occur.
                   Added processing to detect excessive receive errors and
                   disable the receiver so the task may bring down the
                   interface.
03/15/04   ts      Fixed rx_value parameter in uim_process_overrun.
03/10/04   wli     Change MSM6500 clock source to 13Mhz.
02/20/04   ts      Fixed lint errors
02/03/04   ts      Added support for PMIC on UIM2.
01/23/04   ts      Changed the voltage reference on the UIM side of the
                   interface depending upon interface voltage.  Added a 2msec
                   delay after powering down the UIM interface because of the
                   amount of time it takes the PMIC Vreg RUIM to power off.
01/13/04   ts      Added support for the PMIC on the UIM interface.  Also
                   included multi-voltage support when using the PMIC.
01/12/04   ck      Fixed compile errors that showed up when FEATURE_UIM_UTIL
                   was defined with FEATURE_UIM_UICC
12/18/03   ck      Merged in the Real fix to use the correct instruction class
                   for GET RESPONSE when done in non-default logical channel.
10/31/03   ts      Added support for file accesses greater than the maximum
                   transaction size.
10/23/03   ck      Merged in changes to support 18etus between reception of a
                   byte and transmission of byte.
10/17/03   ck      Added UART_TFWR_TFW_MASK to Saber definitions.
10/10/03   ts      Removed specializations includes for GPIO and TLMM.
                   Added support for PMIC on UIM1.
                   Added UART MND definitions for MSM6500.
                   Added control for block mode in TFWR.
10/02/03   ts      Fixed the uim_drv_slot parameter name in
                   uim_update_op_params.  This fixes the macros use of the
                   selected slot for TCXO shutdown.
09/29/03   ck      Added support to process 67xx which is not clearly specified
                   in ETSI specifications.
                   Saved the protocol in the cmd buffer when the APDU is built by
                   the driver.
08/27/03   ck      Fixed a compilation issue with uim_clock_control_val
08/21/03   ts      Added a delay in responding to the procedure byte to fix
                   parity errors.
08/21/03   ck      Merged in support for Saber and TCXO/5 for USIM clock.
08/21/03   ts      New fix for response time issue that caused parity errors.
08/15/03   ck      Removed an erroneous paranthesis and featurized uim_rx_isr1
                   and uim_rx_isr2 around the right features.
08/14/03   ck      Merged in the support for dual slots.
07/22/03   ts      Added a table that indicates the maximum BRAF for the CRCF.
07/15/03   ts      Removed target specific include of clock regime definition
                   file.
06/19/03   ck      Added support to flush the bytes upon receiving an
                   overrun error without resetting the card.
06/12/03   ck      Added the check for UIM_UICC in addition to UIM_WCDMA to
                   arrive at the UIM instruction class.
06/03/03   ck      Changed the status from UIM_PROBLEM to UIM_CONVENTION_ERROR
                   when the error is because of change in the convention.
04/29/03   wli     Added missing "\" to end of #define ENABLE_UART_INTERFACE()
04/28/03   ts      Added support for multi-Voltage classes.
04/28/03   ts      Added support for T=1 protocol.
04/25/03   ts      Added support for GSM clock source on MSM6100.
04/25/03   ts      Enable UIM data and clk for MSM6100.
02/26/03   jk      Added UIM UTIL and WIM functionality
03/10/03   djd     Added support for UIM2.
02/24/03   ts      Reset the UART receiver after stopping break.  This keeps
                   the receiver from detecting a character after releasing
                   the break condition.
02/19/03   ts      Fixed lint issues with kjm changes and UIM logging.
02/13/03   ck      Merged in the changes from kjm to improve the stability of
                   the driver.
01/21/03   ts      Removed #define FEATURE_UIM_USE_13MHZ_GSM_CLOCK.
01/20/03   ts      Added run-time support of 13MHz GSM clock.
12/20/02   ts      Fixed Lint issues
12/04/02   ts      Need the clocks to power down the UIM in the case the UIM
                   has already powered down or the clocks are turned off -
                   like when receiving a task stop.
11/26/02   ts      Moved tcxo frequency determination to uimdrv.h for use by
                   uimgen.c
                   Start with maximum frequency allowed when powering up.
11/21/02   ts      Give access to braf_values table for use in determining
                   invalid TA1 values.
10/31/02   ts      Brought the 6100 code into the mainline.
10/24/02   ts      Added support for work waiting time
10/24/02   ts      Added support for re-transmission due to parity errors.
10/24/02   ts      Changed the clock control to start out with 50% duty cycle.
10/04/02   ts      Commented the code that turned off the clock before turning
                   it back on.  This does not work with the cards.
10/02/02   ts      Added a new feature switch that disables converting the
                   transmitted byte to inverse convention.  This is needed to
                   work with SIM glue logic that changes both the transmit
                   and receive bytes with the SIM configuration control.
10/02/02   ts      Throw away retransmitted bytes received from the card.  This
                   is indicated by the parity error status.
10/02/02   ts      Changed MSM_INH to MSM_IN since the read is masked already by
                   MSM_IN.
09/20/02   ts      Reset the receiver when a receive overrun occurs.
09/20/02   ts      Use the last stop clock value to stop the clock when changing
                   the frequency.
09/20/02   ts      Turn off UIM clock and set clock select before turning UIM
                   clock back on.
                   Use shadow register when changing MISC_CTL_0.
                   Put time delays between turning off the UIM interface lines
                   when powering down.
09/16/02   ts      Added control to the stop bit length in the SIM configuration
                   register for the 6100.
09/12/02   ts      Removed enabling UART1_INT_ENABLE_UART_DP_RX_DATA_ENA for the
                   Cheetah. This conflicts with using GPIO55 and is not needed by
                   the UIM server.
08/07/02   wli     Added UIM UART clock regime control.
07/28/02   sb      Merged changes from rev1.22.1.2 by pmarsh & mainline CHEETAH_GSM.
05/28/02   plm     Added reference to T_UIM_NO_RX_WATERLEVEL to disable programming
                   of rx level for some targets when UIM_NO_SIM_GLUE_LOGIC undefined.
05/22/02   plm     Replaced references to T_KOVU, T_MSM5200 and T_MSM6200 with
                   T_UIM_USE_BSP_DEFINES, T_UIM_UART1_USIM_CFG, T_UIM_SIM_GLUE_LOGIC
                   and T_UIM_UART2_NON_SPLIT_MND_REGS target symbols to be defined
                   in targetXXX.h file for GSM and WCDMA.
07/18/02   wli     Put #include "log.h" back to this file. Removed diagbuf_init().
07/09/02   ts      Ported driver to MSM6100.
07/09/02   ck      Added the feature FEATURE_SLEEP_TASK to the featurization
                   around sleep.h
06/10/02   ts      Fixed the MND values to ensure a 50% duty cycle with the
                   UART clock.
05/21/02   ts      Added support for 256 bytes transfers from the UIM card.
05/09/02   ts      Needed to include 6200 in the definitions of the USIM
                   configuration register with 5200 and Kovu.
05/02/02   ts      Moved the UIM pad enable to the end of uim_dev_init.  Even
                   though the code uses MSM_OUT, it should use MSM_OUTSH.  The
                   rest of the code that accesses MISC_CTL_0 should also use
                   MSM_OUTSH and the shadow register needs to be defined.
04/03/02   AT      Added support for CHEETAH_GSM
03/18/02   ts      Moved the MSMU_OUT and MSMUW_OUT from uimdrv.h to this file
                   to avoid multi-definition problems with these macros.
02/06/02   ts      Programmed the clock source for the UART to be tcxo/4 for Leo.
                   Use a different feature switch to configure which clock source
                   the UIM UART uses.
02/29/02   ts      Changed the constant name in programming inverse convention
                   for Leo and PPG.  The constant (UART_SIM_CFG__SIM_CLK_OFF)
                   is used because it is defined for both targets.
02/08/02   ts      Added support for Leo (MSM6050).  Cleaned up the UART
                   configuration register access for SIM Glue logic chips.
02/07/02   wli     Added inverse convetion support for non sim glue logic targets.
01/25/02   ts      Added support for Kovu project.  Kovu is like 5200 with a few
                   changes.
01/23/02   ts      Added support to turn the UIM clock off at a high level and at
                   a low level through uim_update_op_params.
01/11/02   ts      Need to select UART2 as an alternate function when using GPIO
                   lines for UIM.
01/10/02   ts      Defined symbols to use proper GPIO lines for UIM power and UIM reset
                   control for Panther SURF.
12/20/01   ts      Set rx watermark level to 0 when sending bytes while processing
                   procedure bytes.  This keeps the driver from sending a tx
                   byte too soon after receiving a procedure byte.
12/13/01   ts      Fixed alignment of code and comments.
12/04/01   ts      Added support for UIM application toolkit and toolkit
                   download error.
10/26/01   ts      Stopped processing the NULL procedure byte.
10/18/01   ck      Added featurization around bsp header file as 3000 and 3100
                   targets do not use it.
10/10/01   ts      Changed the MSM5200 macro definition for accessing the UART
                   configuration register and the UART1 command register.  Also,
                   featurized (out) the watermaark level while waiting for transmitted
                   bytes for the MSM5200.  It causes the 5200 to lose a received
                   byte.
09/13/01   ts      Added support for inverse convention of UIM cards.  Featurized
                   under FEATURE_UIM_SUPPORT_INVERSE_CONVENTION.
08/10/01   ck      Removed  FEATURE that was prepended to UIM_NO_SIM_GLUE_LOGIC
                   as it is an internally defined feature.
07/27/01   ts      Consolidated the register and constant differences between
                   the targets.  Changed the UIM clock control to start with
                   tcxo/8 and switch to tcxo/4 if UIM supports higher
                   frequency.
07/26/01   wli     Added a time stamp for debug log.
06/26/01   ts      Updated the feature switches to have "FEATURE_" prepended.
                   Selected GPIO lines for reset and power control based on
                   SURF/FFA hardware feature switches.  Added 5200 driver
                   changes.  Changed clock control names and frequencies based
                   on tcxo relationship.
06/26/01   ts      Added checks for parity errors for targets that have SIM glue
                   logic to throw away received bytes with parity errors.
06/04/01   ts      Fixed the clock frequency value used to determine the
                   number of clock cycles for clk_busy_wait.
05/22/01   ck      Merged in support for logging the UIM transactions.
05/18/01   ck      Added support for USIM.
01/15/01   ck      Created file from R-UIM driver version 1.1.
===========================================================================*/

/*===========================================================================

                              INCLUDE FILES

===========================================================================*/
#include "uimdrv.h"                /* UIM driver header file               */

#if defined( FEATURE_UIM_MDM8200_DRIVER )
  #include "uim8200drv.h"
#elif defined( FEATURE_UIM_MSM7X27_DRIVER )
  #include "uim7x27drv.h"
#elif defined( FEATURE_UIM_QSC11X0_DRIVER )
  #include "uim11x0drv.h"
#else
  #include "uimlegacydrv.h"
#endif /* FEATURE_UIM_QSC11X0_DRIVER */

#include "msm.h"                   /* MSM3000 Register Aliases             */

#include "clk.h"

#include "tramp.h"
#include "bio.h"                   /* Bit I/O macros                       */
#include "clkregim.h"              /* Clock Regime Definitions             */
#include "err.h"                   /* Error handling utilities             */
#include "msg.h"                   /* Message utilities                    */
#include "uimi.h"                  /* UIM Internal header file             */
#if defined( FEATURE_UIM_T_1_SUPPORT )
#include "crc.h"
#include "uimt1.h"                 /* T=1 protocol support                 */
#endif /* FEATURE_UIM_T_1_SUPPORT */
#include "hw.h"                    /* Target definitions */

#ifdef FEATURE_UIM_UTIL
#include "uimutil.h"
#endif /* FEATURE_UIM_UTIL */

#ifdef FEATURE_UIM_MSM6800_TCXO_DIV5
#define UIM_MSM6800_CLK_SRCSEL_UART2_TCXO_DIV5 0x4
#define UIM_MSM6800_CLK_SRCSEL_UART3_TCXO_DIV5 0x4
#endif /* FEATURE_UIM_MSM6800_TCXO_DIV5 */

#if defined( T_SLEEP ) || defined( FEATURE_SLEEP_TASK )
#include "sleep.h"
#endif
#if defined( FEATURE_UIM_PMIC_ON_UIM1 ) || \
    defined( FEATURE_UIM_PMIC_ON_UIM2 ) || \
    defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC )
#include "pm.h"
#if defined(T_QSC6270) || defined(FEATURE_UIM_QSC11X0_DRIVER)
#include "pmgen.h"
#endif /* T_QSC6270 || FEATURE_UIM_QSC11X0_DRIVER */
#endif


#if defined (FEATURE_UIM2) && ! ((defined T_MSM6100) || (defined T_UIM_SABER_DEFS) \
             || (defined T_MSM6275))
#error code not present
#endif /* FEATURE_UIM2 && ! T_MSM6100 */
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */
/*===========================================================================

                Lint message inhibitions

===========================================================================*/
/*lint -esym(578,uim_drv_slot)
   uim_drv_slot is used in the macros to write to the UART registers to
   support dual slots.  I declare uim_drv_slot locally to override the global
   uim_drv_slot so that I can use the same macros whether uim_drv_slot (dual
   slots) is used or not.
*/
/*lint -esym(715,uim_drv_slot)
   uim_drv_slot is not accessed from the macros when the build configuration
   is for one slot.
*/
/*lint -e750
   There are a number of macros and definitions that are not always used
   due to the build configuration (one slot or two).  Also, there are a
   number of constants defined in the file to completely describe the
   access to the registers.  These are not used.
*/
/*lint -e752
   uim_rx_isr_1 or uim_rx_isr_2 are defined based upon the which slots are
   supported in the build
*/
/*lint -esym(550,rx_value)
   rx_value is used to pass the received value to the debug log.  If the
   debug logging is not enabled, this variable is not accessed in the code.
*/
/*lint -esym(714,uim_tcxo_vote)
  uim_tcxo_vote is used to check if the clock is on or not in other modules
*/


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

boolean uim_clock_is_on = TRUE;
boolean uim_clock_is_on_2 = TRUE;

/* Queue to hold commands for uim */
extern q_type uim_cmd_q;

#ifdef FEATURE_UIM_UART_DM
/* A static variable to track the number of bytes received */
static int32 num_bytes_received = 0;

/* Initialize the boolean which enables the parity error workaround with
   UART_DM */
boolean uim_par_err_workaround = FALSE;

/* Count to keep track of the RX bytes being received */
static int32 uim_gcf_byte_count = 0;

/* Counter for RX bytes in a RX data transfer */
int32 num_bytes_in_this_rx_transfer[UIM_NUM_DRV_SLOTS] = {0};

/* A static variable to count the RXLEVs happened */
int32 uim_rxlev_count = 0;

/* use an extra guard time of 1 etus over the default guard time of 2 etus */
#define UIM_UART_DM_EXTRA_GUARD_TIME 1

#else

/* use an extra guard time of 0 etus over the default guard time of 2 etus */
#define UIM_UART_DM_EXTRA_GUARD_TIME 0

#endif /* FEATURE_UIM_UART_DM */


#if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES )
#define UIM_MICROSECONDS_PER_TWO_MILLISECONDS 2000
uim_voltage_class_type uim_current_voltage_class[UIM_NUM_DRV_SLOTS];
#endif /* FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES */

/* Variable to indicate whether a warning or error is indicated by the card */
LOCAL boolean uim_force_get_response = FALSE;
/* This holds the frequency of the UIM clock source */
LOCAL dword uim_clk_source_freq[UIM_NUM_DRV_SLOTS];
/* This holds the UIM clock frequency. */
dword uim_clk_freq[UIM_NUM_DRV_SLOTS];
/* This holds the MAXIMUM value the UIM clock can be */
uim_clk_freq_type uim_max_freq_supported[UIM_NUM_DRV_SLOTS];
/* This holds the programmed UIM clock */
LOCAL uim_clock_control_type uim_clock_control_value[UIM_NUM_DRV_SLOTS];

#ifdef FEATURE_UIM_JCDMA_RUIM_SUPPORT
  #define UIM_TRANSACTION_TIME_DURATION 10000
#else
/* Maximum time duration to receive null procedure bytes */
  #define UIM_TRANSACTION_TIME_DURATION 20000
#endif /* FEATURE_UIM_JCDMA_RUIM_SUPPORT */

#ifdef FEATURE_UIM_UART_DM

/* This variable holds the sub-state of UIM_RX state state */
uim_rx_state_type uim_rx_sub_state = UIM_RX_PROCESS_IDLE;

/* Static variable to hold the current baud rate parameters */
uim_clock_rate_conversion_factor_type uim_FI_current = UIM_CRCF_372_1;
uim_baud_rate_adjustment_factor_type  uim_DI_current = UIM_BRAF_1;

/* Boolean to indicate if SW1 status byte is received */
boolean sw1_byte_rcvd = FALSE;
/* Boolean to indicate if SW2 status byte is received */
boolean sw2_byte_rcvd = FALSE;
/* Boolean to indicate if the UIM response bytes are ready to be processed */
boolean resp_bytes_being_rcvd = FALSE;

#endif /* FEATURE_UIM_UART_DM*/

/*
 * This table contains the M, N and D register values used to program the
 * UART clock which determines the baudrate for the I/O line.  The table
 * index is based on the frequency conversion factor of the UIM interface.
 * The baud rate adjustment factor affects the M value.
 */
typedef struct
{
  word mreg;
  word nreg;
  word dreg;
} mnd_record_type;

/* For M/N counter registers */

#if !defined (T_UIM_UART2_NON_SPLIT_MND_REGS)
/* The maximum acceptable D value is 10 bits, hence (2^10)-1. */
#define UIM_MAX_D_VALUE_IN_SPLIT_MND_REGS 1023
#endif /* !defined (T_UIM_UART2_NON_SPLIT_MND_REGS) */

/* These values are computed by using the following formula.
   The example uses the default values of Fi = 372, D = 1.

     Fyamn = Ftcxo/4 * (M/N)yamn
     Fuart = Ftcxo/4 * (M/N)uart

     Fetu = Fyamn/372   ETU is the baudrate
     Fetu = Fuart/16    ETU is the baudrate

   solving for (M/N)uart and (M/N)yamn

     (M/N)uart = (M/N)yamn * 4 / 93

     (M/N)yamn = 1/2, D = 1 -> (M/N)uart = 2/93 D = 47                      */


#if !defined(FEATURE_UIM_TCXO_CLK_FREQ)
LOCAL const mnd_record_type uart_clock_values[UIM_CRCF_SIZE] =
/*   MREG      NREG      DREG                                      */
{
    {   4,     186,       93},       /* Conversion factor = 372   */
    {   4,     186,       93},       /* Conversion factor = 372   */
    {   8,     558,      279},       /* Conversion factor = 558   */
    {   2,     186,       93},       /* Conversion factor = 744   */
    {   4,     558,      279},       /* Conversion factor = 1116  */
    {   1,     186,       93},       /* Conversion factor = 1488  */
    {   4,     930,      465},       /* Conversion factor = 1860  */
    {   0,       0,        0},       /* Reserved for future use   */
    {   0,       0,        0},       /* Reserved for future use   */
    {   1,      64,       32},       /* Conversion factor = 512   */
    {   1,      96,       48},       /* Conversion factor = 768   */
    {   1,     128,       64},       /* Conversion factor = 1024  */
    {   1,     192,       96},       /* Conversion factor = 1536  */
    {   1,     256,      128},       /* Conversion factor = 2048  */
    {   0,       0,        0},       /* Reserved for future use   */
    {   0,       0,        0}        /* Reserved for future use   */
};
/* This type defines the maximum allowable BRAF for the given UART
   clock source, CRCF and UIM clock frequency (TD8, TD5 and TD4) */
const byte UIM_MAX_BRAF[UIM_MAX_BRAF_TD_INDEX][UIM_CRCF_SIZE] = {
/*  372 372 558 744 1116 1488 1860 RFU RFU 512 768 1024 1536 2048 RFU RFU */
  {  20, 20, 32, 32,  32,  32,  32,  0,  0, 32, 32,  32,  32,  32,  0,  0 },
#if defined( FEATURE_UIM_USE_TCXO_DIV_5 )
  {  12, 12, 20, 20,  32,  32,  32,  0,  0, 32, 32,  32,  32,  32,  0,  0 },
#endif /* FEATURE_UIM_USE_TCXO_DIV_5 */
  {   8,  8, 16, 20,  32,  32,  32,  0,  0, 16, 20,  32,  32,  32,  0,  0 }
};

#else /* MSM is using tcxo as UART clock */
LOCAL const mnd_record_type uart_clock_values[UIM_CRCF_SIZE] =
/*   MREG      NREG      DREG                                      */
{
    {   2,     372,      186},       /* Conversion factor = 372   */
    {   2,     372,      186},       /* Conversion factor = 372   */
    {   2,     558,      279},       /* Conversion factor = 558   */
    {   1,     372,      186},       /* Conversion factor = 744   */
    {   2,    1116,      558},       /* Conversion factor = 1116  */
    {   1,     744,      372},       /* Conversion factor = 1488  */
    {   2,    1860,      930},       /* Conversion factor = 1860  */
    {   0,       0,        0},       /* Reserved for future use   */
    {   0,       0,        0},       /* Reserved for future use   */
    {   1,     256,      128},       /* Conversion factor = 512   */
    {   1,     384,      192},       /* Conversion factor = 768   */
    {   1,     512,      256},       /* Conversion factor = 1024  */
    {   1,     768,      384},       /* Conversion factor = 1536  */
    {   1,    1024,      512},       /* Conversion factor = 2048  */
    {   0,       0,        0},       /* Reserved for future use   */
    {   0,       0,        0}        /* Reserved for future use   */
};
/* This type defines the maximum allowable BRAF for the given UART
   clock source, CRCF and UIM clock frequency (TD8, TD5 and TD4) */
const byte UIM_MAX_BRAF[UIM_MAX_BRAF_TD_INDEX][UIM_CRCF_SIZE] = {
/*  372 372 558 744 1116 1488 1860 RFU RFU 512 768 1024 1536 2048 RFU RFU */
  {  32, 32, 32, 32,  32,  32,  32,  0,  0, 32, 32,  32,  32,  32,  0,  0 },
#if defined( FEATURE_UIM_USE_TCXO_DIV_5 )
  {  32, 32, 32, 32,  32,  32,  32,  0,  0, 32, 32,  32,  32,  32,  0,  0 },
#endif /* FEATURE_UIM_USE_TCXO_DIV_5 */
  {  32, 32, 32, 32,  32,  32,  32,  0,  0, 32, 32,  32,  32,  32,  0,  0 }
};

#if defined( FEATURE_UIM_USE_TCXO_DIV_5 )
/* MREG, NREG and DREG values for TA1=0x97.
   The legacy values some how are not working for TA1=0x97.
   Hence, these values are calculated using the formula:
   (M/N) * uim_clk/16 = sim_clk/ (F/D), where F = 512 and D = 64. */
#define MREG_TA10X97  4
#define NREG_TA10x97 10
#define DREG_TA10x97  5
#endif /* FEATURE_UIM_USE_TCXO_DIV_5 */

#endif /* !defined(FEATURE_UIM_TCXO_CLK_FREQ) */


/* This is the delay used before sending a byte to the UIM card directly after
   processing a byte from the card. */
dword uim_response_byte_delay = UIM_RESPONSE_BYTE_DELAY_DEFAULT;

/*
 * The following constants define the fields in the MND_LSB byte
 */
#define MLSB_MASK          0x01
#define MMSB_SHIFT         1
#define MLSB_SHIFT         5

#define NLSB_MASK          0x07
#define NMSB_SHIFT         3
#define NLSB_SHIFT         2

#define DLSB_MASK          0x03
#define DMSB_SHIFT         2

/*
 * The following table holds the Baud Rate Adjustment Factors defined
 * for the DI value of an ATR or PPS response
 */
const byte braf_values[UIM_BRAF_SIZE] =
{
   0,        /* DI = 0 - Reserved for future use  */
   1,        /* DI = 1                            */
   2,        /* DI = 2                            */
   4,        /* DI = 3                            */
   8,        /* DI = 4                            */
  16,        /* DI = 5                            */
  32,        /* DI = 6                            */
#ifdef FEATURE_UIM_HANDLE_TA1_0X97_ATR
  64,        /* DI = 7                            */
#else
   0,        /* DI = 7 - Reserved for future use  */
#endif /* FEATURE_UIM_HANDLE_TA1_0X97_ATR */
  12,        /* DI = 8                            */
  20,        /* DI = 9                            */
   0,        /* DI = 10 - Reserved for future use  */
   0,        /* DI = 11 - Reserved for future use  */
   0,        /* DI = 12 - Reserved for future use  */
   0,        /* DI = 13 - Reserved for future use  */
   0,        /* DI = 14 - Reserved for future use  */
   0,        /* DI = 15 - Reserved for future use  */
};

/*
 * The following table holds the Clock Rate Conversion Factors defined
 * for the FI value of an ATR or PPS response
 */
const dword crcf_values[UIM_CRCF_SIZE] =
{
  372,        /* FI = 0                             */
  372,        /* FI = 1                             */
  558,        /* FI = 2                             */
  744,        /* FI = 3                             */
 1116,        /* FI = 4                             */
 1488,        /* FI = 5                             */
 1860,        /* FI = 6                             */
    0,        /* FI = 7  - Reserved for future use  */
    0,        /* FI = 8  - Reserved for future use  */
  512,        /* FI = 9                             */
  768,        /* FI = 10                            */
 1024,        /* FI = 11                            */
 1536,        /* FI = 12                            */
 2048,        /* FI = 13                            */
    0,        /* FI = 14 - Reserved for future use  */
    0,        /* FI = 15 - Reserved for future use  */
};
/*
 * These values are specific to the UART registers.
 */

/* Mode Register 1 */

#define MSMU_MR1_RFRC         0x80     /* Ready-for-receiving Control      */
#define MSMU_MR1_CTSC         0x40     /* Clear-to-send Control            */
#define MSMU_MR1_RFR_LVL_MASK 0x3f     /* Mask to use when writing to lower
                                          six bits of MSMU_MR1 register    */
#define MSMU_MR1_DISABLE_RFR_CTS 0x00  /* Disable RFR and CTS control      */

/* Mode Register 2 */

#define MSMU_MR2_LOOPBACK          0x80    /* Channel Mode                */
#define MSMU_MR2_ERRMODE_CHAR      0x00    /* Error Mode - Character mode */
#define MSMU_MR2_ERRMODE_BLOCK     0x40    /* Error Mode - Block mode     */
#define MSMU_MR2_5BPC              0x00    /* 5 Bits per character        */
#define MSMU_MR2_6BPC              0x10    /* 6 Bits per character        */
#define MSMU_MR2_7BPC              0x20    /* 7 Bits per character        */
#define MSMU_MR2_8BPC              0x30    /* 8 Bits per character        */
#define MSMU_MR2_1SB               0x04    /* 1 Stop Bit                  */
#define MSMU_MR2_2SB               0x0C    /* 2 Stop Bits                 */
#define MSMU_MR2_OPAR              0x01    /* Odd Parity                  */
#define MSMU_MR2_EPAR              0x02    /* Even Parity                 */
#define MSMU_MR2_SPAR              0x03    /* Space Parity                */

/* Clock Select Register */

#define MSMU_CSR_2BITS_BIT    0xAA     /* 2 bits/bit in sample mode         */
#define MSMU_CSR_4BITS_BIT    0xCC     /* 4 bits/bit in sample mode         */
#define MSMU_CSR_8BITS_BIT    0xEE     /* 8 bits/bit in sample mode         */
#define MSMU_CSR_16BITS_BIT   0xFF     /* baud rate determined by M/N clock
                                          16 bits per bit in sample mode    */

#define MSMU_CSR_TX_ONLY_MASK  0x0F    /* Mask to change only TX CSR       */
#define MSMU_CSR_RX_ONLY_MASK  0xF0    /* Mask to change only RX CSR       */

/* Command Register */

#define MSMU_CR_ENA_STALE_EVT    0x500  /* Enable Stale Event                 */
#define MSMU_CR_CLEAR_STALE_EVT  0x080  /* Clear State Event                  */
#define MSMU_CR_FORCE_STALE_EVT  0x400  /* Force a Stale Event: FLUSH RX Pkr  */
#define MSMU_CR_CLR_TX_READY     0x300  /* Clears the TX Ready Event          */
#define MSMU_CR_RESET_RX      0x10     /* Reset Receiver                   */
#define MSMU_CR_RESET_TX      0x20     /* Reset Transmitter                */
#define MSMU_CR_RESET_ERR     0x30     /* Reset Error Status               */
#define MSMU_CR_RESET_BRK_INT 0x40     /* Reset Break Change Interrupt     */
#define MSMU_CR_STA_BRK       0x50     /* Start Break                      */
#define MSMU_CR_STO_BRK       0x60     /* Stop Break                       */
#define MSMU_CR_CLR_DCTS      0x70     /* Clear CTS Change (delta)         */
#define MSMU_CR_CLR_TX_ERROR  0x80     /* Clear TX_ERROR status            */
#define MSMU_CR_SAMP_MODE     0x90     /* Sample Data Mode                 */
#define MSMU_CR_TEST_PARITY   0xA0     /* Test Parity                      */
#define MSMU_CR_TEST_FRAME    0xB0     /* Test Frame                       */
#define MSMU_CR_RESET_SAMPLE  0xC0     /* Reset Sample Data Mode           */
#define MSMU_CR_SET_RFR       0xD0     /* Set RFR                          */
#define MSMU_CR_RESET_RFR     0xE0     /* Reset RFR                        */
#define MSMU_CR_DIS_TX        0x08     /* Disable Transmitter              */
#define MSMU_CR_ENA_TX        0x04     /* Enable Transmitter               */
#define MSMU_CR_DIS_RX        0x02     /* Disable Receiver                 */
#define MSMU_CR_ENA_RX        0x01     /* Enable Receiver                  */

/* Interrupt Programming Register */

#define MSMU_IPR_NEW_SAMPLE_MODE  0x40  /* New Sample Mode method          */
#define MSMU_IPR_NEW_RXSTALE      0x20  /* New RXSTALE Interrupt method    */
#define MSMU_IPR_TWO_CHARS        0x02  /* Two character timeout           */

/* Speed enhancement logic for LCU doesn't aim at reducing the etu (elementary
   time unit) value as it is depends frequency applied to card and the baud-rate
   negotiated with the card.

   Speed enhancement logic for LCU is based on programming a proper stale
   timeout value and rx water mark levels. RX water mark level is to receive
   and process the data as fast as possible. Stale timeout is to determine the
   end of data transfer. For a high stale timeout value, we wait un-necessarily
   for that much *extra* time if our expected size of data has already been
   received but just lying in the RX FIFO and packing buffer of the UART.
   So, to enhance the speed, we need to program lesser values of stale timeout
   and RX water mark levels. We are only trying to not let the race conditions
   or timing issues happen by not programming very low values of stale timeout
   and RX water mark levels. The UART HW designers recommended not to use very
   low RX STALE timeout values. The below stale and rx water mark levels were
   determined only by experimentation. */

/* Optimized the stale timeout value to 2 (i.e about 2 character times
   or ~20 etus for speed enhancement. */
#define MSMU_IPR_DEF_STALE_TIMEOUT 0x02

/* The IPR register bit-mask for enabling the logic in UART DM that
   ignores the bytes received with parity error in character mode */
#define MSMU_IPR_PAR_ERR_FIX_ENABLE 0x20

/* Default RX water mark level for LCU set to 36 words just
   enough for only one RXLEV interrupt to happen */
#define MSMU_DEF_RFWR_VAL 36

/* Macro for number of ack bytes */
#define NUM_ACK_BYTES 1

/* Macro for number of status bytes */
#define NUM_STATUS_BYTES 2

/* Macro for maximum RX FIFO size */
#define MAX_RXFIFO_SIZE 256

/* The DMRX register value should always be a multiple of 16.
   Since, the maximum size of SIM response is 256 + 1 (ACK) + 2 (status bytes),
   the DMRX register value is set to 272 */
#define MSMU_DEF_RX_INIT_VAL 272

/* Interrupt Mask Register */
#define MSMU_IMR_TX_DONE     0x100     /* Only for T=1 Protocol            */
#define MSMU_IMR_TX_READY    0x80      /* TX FIFO Empty Or NO_CHARS FOR TX */

#define MSMU_IMR_TX_ERROR    0x80      /* Parity error on transmission     */
#define MSMU_IMR_CUR_CTS     0x40      /* Current CTS                      */
#define MSMU_IMR_DELTA_CTS   0x20      /* Delta CTS                        */
#define MSMU_IMR_RXLEV       0x10      /* RX Level exceeded                */
#define MSMU_IMR_RXSTALE     0x08      /* Stale RX character occurred      */
#define MSMU_IMR_RXBREAK     0x04      /* RX Break occurred                */
#define MSMU_IMR_RXHUNT      0x02      /* RX Hunt character received       */
#define MSMU_IMR_TXLEV       0x01      /* TX Level or below met            */
#define MSMU_IMR_MASK_ALL    0x00      /* Mask all interrupts              */

/* Interrupt Status Register */

#define MSMU_ISR_TX_READY    0x80      /* TX FIFO Empty or NO_CHARS for TX */
#define MSMU_ISR_TX_ERROR    0x80      /* Parity error on transmission     */
#define MSMU_ISR_CUR_CTS     0x40      /* Current CTS                      */
#define MSMU_ISR_DELTA_CTS   0x20      /* Delta CTS                        */
#define MSMU_ISR_RXLEV       0x10      /* RX Level met or exceeded         */
#define MSMU_ISR_RXSTALE     0x08      /* Stale RX character occurred      */
#define MSMU_ISR_RXBREAK     0x04      /* RX Break occurred                */
#define MSMU_ISR_RXHUNT      0x02      /* RX Hunt character received       */
#define MSMU_ISR_TXLEV       0x01      /* TX Level or below met            */

#ifdef FEATURE_UIM_ADD_DELAY_BEFORE_AND_AFTER_CLOCK_STOP
  /* Delay before doing clock stop */
  #define UIM_DELAY_BEFORE_CLOCK_STOP 30

  /* Delay after clock start */
  #define UIM_DELAY_AFTER_CLOCK_RESTART 5
#endif /* FEATURE_UIM_ADD_DELAY_BEFORE_AND_AFTER_CLOCK_STOP */

#if defined( FEATURE_UIM_SUPPORT_INVERSE_CONVENTION )
/*
 * This type identifies the convention of the SIM card interface.
 */
typedef enum {
  /* Identifies direct convention on the SIM interface */
  UIM_DIRECT_CONVENTION,
  /* Identifies inverse convention on the SIM interface */
  UIM_INVERSE_CONVENTION
} uim_convention_type;

#ifndef FEATURE_UIM_SUPPORT_INVERSE_CONVENTION_FOR_TX_IN_HW
#define BYTE_RANGE 256
LOCAL const byte change_convention[BYTE_RANGE] =
{
/* 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, */
   0xff, 0x7f, 0xbf, 0x3f, 0xdf, 0x5f, 0x9f, 0x1f,
/* 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, */
   0xef, 0x6f, 0xaf, 0x2f, 0xcf, 0x4f, 0x8f, 0x0f,
/* 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, */
   0xf7, 0x77, 0xb7, 0x37, 0xd7, 0x57, 0x97, 0x17,
/* 0x18, 0x19, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, */
   0xe7, 0x67, 0xa7, 0x27, 0xc7, 0x47, 0x87, 0x07,
/* 0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, */
   0xfb, 0x7b, 0xbb, 0x3b, 0xdb, 0x5b, 0x9b, 0x1b,
/* 0x28, 0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, */
   0xeb, 0x6b, 0xab, 0x2b, 0xcb, 0x4b, 0x8b, 0x0b,
/* 0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, */
   0xf3, 0x73, 0xb3, 0x33, 0xd3, 0x53, 0x93, 0x13,
/* 0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f, */
   0xe3, 0x63, 0xa3, 0x23, 0xc3, 0x43, 0x83, 0x03,
/* 0x40, 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, */
   0xfd, 0x7d, 0xbd, 0x3d, 0xdd, 0x5d, 0x9d, 0x1d,
/* 0x48, 0x49, 0x4a, 0x4b, 0x4c, 0x4d, 0x4e, 0x4f, */
   0xed, 0x6d, 0xad, 0x2d, 0xcd, 0x4d, 0x8d, 0x0d,
/* 0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, */
   0xf5, 0x75, 0xb5, 0x35, 0xd5, 0x55, 0x95, 0x15,
/* 0x58, 0x59, 0x5a, 0x5b, 0x5c, 0x5d, 0x5e, 0x5f, */
   0xe5, 0x65, 0xa5, 0x25, 0xc5, 0x45, 0x85, 0x05,
/* 0x60, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, */
   0xf9, 0x79, 0xb9, 0x39, 0xd9, 0x59, 0x99, 0x19,
/* 0x68, 0x69, 0x6a, 0x6b, 0x6c, 0x6d, 0x6e, 0x6f, */
   0xe9, 0x69, 0xa9, 0x29, 0xc9, 0x49, 0x89, 0x09,
/* 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, */
   0xf1, 0x71, 0xb1, 0x31, 0xd1, 0x51, 0x91, 0x11,
/* 0x78, 0x79, 0x7a, 0x7b, 0x7c, 0x7d, 0x7e, 0x7f, */
   0xe1, 0x61, 0xa1, 0x21, 0xc1, 0x41, 0x81, 0x01,
/* 0x80, 0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87, */
   0xfe, 0x7e, 0xbe, 0x3e, 0xde, 0x5e, 0x9e, 0x1e,
/* 0x88, 0x89, 0x8a, 0x8b, 0x8c, 0x8d, 0x8e, 0x8f, */
   0xee, 0x6e, 0xae, 0x2e, 0xce, 0x4e, 0x8e, 0x0e,
/* 0x90, 0x91, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, */
   0xf6, 0x76, 0xb6, 0x36, 0xd6, 0x56, 0x96, 0x16,
/* 0x98, 0x99, 0x9a, 0x9b, 0x9c, 0x9d, 0x9e, 0x9f, */
   0xe6, 0x66, 0xa6, 0x26, 0xc6, 0x46, 0x86, 0x06,
/* 0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7, */
   0xfa, 0x7a, 0xba, 0x3a, 0xda, 0x5a, 0x9a, 0x1a,
/* 0xa8, 0xa9, 0xaa, 0xab, 0xac, 0xad, 0xae, 0xaf, */
   0xea, 0x6a, 0xaa, 0x2a, 0xca, 0x4a, 0x8a, 0x0a,
/* 0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6, 0xb7, */
   0xf2, 0x72, 0xb2, 0x32, 0xd2, 0x52, 0x92, 0x12,
/* 0xb8, 0xb9, 0xba, 0xbb, 0xbc, 0xbd, 0xbe, 0xbf, */
   0xe2, 0x62, 0xa2, 0x22, 0xc2, 0x42, 0x82, 0x02,
/* 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, */
   0xfc, 0x7c, 0xbc, 0x3c, 0xdc, 0x5c, 0x9c, 0x1c,
/* 0xc8, 0xc9, 0xca, 0xcb, 0xcc, 0xcd, 0xce, 0xcf, */
   0xec, 0x6c, 0xac, 0x2c, 0xcc, 0x4c, 0x8c, 0x0c,
/* 0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7, */
   0xf4, 0x74, 0xb4, 0x34, 0xd4, 0x54, 0x94, 0x14,
/* 0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf, */
   0xe4, 0x64, 0xa4, 0x24, 0xc4, 0x44, 0x84, 0x04,
/* 0xe0, 0xe1, 0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, */
   0xf8, 0x78, 0xb8, 0x38, 0xd8, 0x58, 0x98, 0x18,
/* 0xe8, 0xe9, 0xea, 0xeb, 0xec, 0xed, 0xee, 0xef, */
   0xe8, 0x68, 0xa8, 0x28, 0xc8, 0x48, 0x88, 0x08,
/* 0xf0, 0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, */
   0xf0, 0x70, 0xb0, 0x30, 0xd0, 0x50, 0x90, 0x10,
/* 0xf8, 0xf9, 0xfa, 0xfb, 0xfc, 0xfd, 0xfe, 0xff  */
   0xe0, 0x60, 0xa0, 0x20, 0xc0, 0x40, 0x80, 0x00
};
#endif /* FEATURE_UIM_SUPPORT_INVERSE_CONVENTION_FOR_TX_IN_HW */
#endif /* FEATURE_UIM_SUPPORT_INVERSE_CONVENTION */

/*
 * These static variables hold the callback function pointers for
 * the driver software.
 */
#ifndef FEATURE_UIM_IGNORE_UNSOLICITED_BYTES
LOCAL uim_unsolict_resp_callback_type
  unsolict_resp_callback_func[UIM_NUM_DRV_SLOTS];
#endif /* FEATURE_UIM_IGNORE_UNSOLICITED_BYTES */
LOCAL uim_resp_callback_type atr_rx_callback_func[UIM_NUM_DRV_SLOTS];
LOCAL uim_resp_callback_type pps_resp_callback_func[UIM_NUM_DRV_SLOTS];
LOCAL uim_resp_callback_type apdu_resp_callback_func[UIM_NUM_DRV_SLOTS];

/*
 * These static variables hold the state and processing variables
 * for the driver.  These are shared with the UART Rx ISR.
 * the driver software.
 */
#if defined( FEATURE_UIM_SUPPORT_INVERSE_CONVENTION )
  LOCAL uim_convention_type uim_convention[UIM_NUM_DRV_SLOTS];
#endif /* FEATURE_UIM_SUPPORT_INVERSE_CONVENTION */

#ifdef FEATURE_UIM_UART_DM
#define UIM_FLUSH_UART_DM_TIME_INC              5 /* 5  ms*/
#define UIM_MAX_FLUSH_TIMER_ALLOW_CARD_TIMEOUT 50 /* 15 ms*/
uint32  uim_time_ellapsed;
#endif /* FEATURE_UIM_UART_DM */

byte *request_buffer_ptr[UIM_NUM_DRV_SLOTS];
int  num_bytes_to_send[UIM_NUM_DRV_SLOTS];
byte *resp_buffer_ptr[UIM_NUM_DRV_SLOTS];
LOCAL word  num_resp_bytes_rcvd_total[UIM_NUM_DRV_SLOTS];
word  num_resp_bytes_rcvd[UIM_NUM_DRV_SLOTS];
LOCAL boolean check_byte_expected[UIM_NUM_DRV_SLOTS];
LOCAL byte check_byte[UIM_NUM_DRV_SLOTS];
LOCAL int  num_historical_chars[UIM_NUM_DRV_SLOTS];
LOCAL int  num_bytes_to_skip[UIM_NUM_DRV_SLOTS];
LOCAL int  num_guardtime_bytes[UIM_NUM_DRV_SLOTS];
LOCAL dword uim_tfwr_shadow[UIM_NUM_DRV_SLOTS];

#ifdef FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG
LOCAL dword uim_uart_shadow[UIM_NUM_DRV_SLOTS];
#else
LOCAL word uim_uart_shadow[UIM_NUM_DRV_SLOTS];
#endif /* FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG */

LOCAL byte num_cmd_data_bytes_to_send[UIM_NUM_DRV_SLOTS];
LOCAL byte *apdu_command_data_ptr[UIM_NUM_DRV_SLOTS];
word num_resp_data_bytes_expected[UIM_NUM_DRV_SLOTS];
LOCAL byte ins_byte[UIM_NUM_DRV_SLOTS];
LOCAL boolean send_all_remaining_bytes[UIM_NUM_DRV_SLOTS];
LOCAL uim_protocol_type uimdrv_protocol[UIM_NUM_DRV_SLOTS];
#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  LOCAL boolean processing_download_error[UIM_NUM_DRV_SLOTS];
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */
/*
   This holds the receive mode while processing ATR or PPS.
 */
LOCAL uim_atr_rx_mode_type receive_mode[UIM_NUM_DRV_SLOTS];

uim_rx_state_type uim_rx_state[UIM_NUM_DRV_SLOTS];


uim_tx_state_type uim_tx_state[UIM_NUM_DRV_SLOTS];

/*
   This type defines the result of the error signal check
 */
typedef enum {
  GUARDTIME_RESEND_BYTE,          /* There was an error detected in the
                                     guardtime                               */
  GUARDTIME_BYTE_SENT_SUCCESSFUL  /* There was no error indication in the
                                     guardtime                               */
} proc_guardtime_return_status;

/* This holds the error status received during the guardtime.                */
LOCAL proc_guardtime_return_status
  guardtime_error_return_code[UIM_NUM_DRV_SLOTS];

/* This holds the number of sample bytes that are processed for a
   transmitted byte
 */
LOCAL int num_samples_of_tx_byte[UIM_NUM_DRV_SLOTS];

#if defined(FEATURE_NEW_SLEEP_API) && defined(FEATURE_UIM_USES_NEW_SLEEP_API)
/* This variable holds the SLEEP Handle for voting */
sleep_okts_handle uim_sleep_okts_handle;
#endif /* FEATURE_NEW_SLEEP_API && FEATURE_UIM_USES_NEW_SLEEP_API */

/*
   These are UIM card constants
 */
#if defined( FEATURE_UIM_SUPPORT_INVERSE_CONVENTION )
  #define INVERSE_CONVENTION_TS_DETECT 0x03
  #define DIRECT_CONVENTION_TS_DETECT  0x23
#endif /* FEATURE_UIM_SUPPORT_INVERSE_CONVENTION */
#define MAX_NUM_ATR_BYTES 33
#define MAX_NUM_PPS_BYTES 6
#define NUM_STATUS_WORDS   2
/* These define the result of XORing the Ack byte with the INS byte          */
#define PAUSE_REMAINING_BYTES 0x00    /* Send the remaining bytes of the data*/
#define PROG_REMAINING_BYTES  0x01    /* Send the remaining bytes of the data*/
#define PAUSE_NEXT_BYTE       0xFF    /* Send only the next byte and wait for
                                         Ack                                 */
#define PROG_NEXT_BYTE        0xFE    /* Send only the next byte and wait for
                                         Ack                                 */

/*
   Make sure the following values match each other.  The clock select
   register value determines the number of samples per bit time.
 */
#define CSR_VALUE MSMU_CSR_16BITS_BIT
#define NUM_BYTES_PER_BIT 2
#define ERROR_SIGNAL_BITS 3
/* This defines no error value of the sample byte for the error signal       */
#define NO_ERROR 0xFF

/* These constants define the receive watermark levels. */
#define UIM_RX_WATER_MARK_LEVEL_ZERO      0
#define UIM_RX_WATER_MARK_LEVEL_FIVE      5

/* These constants define the M, N and D values for controlling YAMN1. */
/* This set defines the control values to keep the YAMN clock line low. */
#define UIM_CLK_MDIV_LSB_LOW  0x00;   /* This is the M value when the clock is held */
#define UIM_CLK_MDIV_MSB_LOW  0x00;   /* LOW                                        */
#define UIM_CLK_NDIV_LSB_LOW  0x00;   /* This is the N value when the clock is held */
#define UIM_CLK_NDIV_MSB_LOW  0x10;   /* LOW                                        */
#define UIM_CLK_DUTY_LSB_LOW  0xFF;   /* This is the DUTY value when the clock is   */
#define UIM_CLK_DUTY_MSB_LOW  0x1F;   /* held LOW                                   */

/* This set defines the control values to keep the YAMN clock line high. */
#define UIM_CLK_MDIV_LSB_HIGH  0x00;   /* This is the M value when the clock is held */
#define UIM_CLK_MDIV_MSB_HIGH  0x00;   /* HIGH                                       */
#define UIM_CLK_NDIV_LSB_HIGH  0x00;   /* This is the N value when the clock is held */
#define UIM_CLK_NDIV_MSB_HIGH  0x10;   /* HIGH                                       */
#define UIM_CLK_DUTY_LSB_HIGH  0x00;   /* This is the DUTY value when the clock is   */
#define UIM_CLK_DUTY_MSB_HIGH  0x00;   /* held HIGH                                  */

/* This set defines the control values to program the YAMN clock line at txco/8. */
#define UIM_CLK_MDIV_LSB_TD8  0x01;   /* This is the M value when the clock is held */
#define UIM_CLK_MDIV_MSB_TD8  0x00;   /* HIGH                                       */
#define UIM_CLK_NDIV_LSB_TD8  0xFE;   /* This is the N value when the clock is held */
#define UIM_CLK_NDIV_MSB_TD8  0x1F;   /* HIGH                                       */
#define UIM_CLK_DUTY_LSB_TD8  0x01;   /* This is the DUTY value when the clock is   */
#define UIM_CLK_DUTY_MSB_TD8  0x00;   /* held HIGH                                  */

/* Maximum time we want uim module to do a clk_busy_wait */
#define UIM_MAX_CLK_BUSY_WAIT 1000

/*
  This is a conversion table that returns the number of bytes that follow the
  format byte.  Only the "presents of interface bytes" field is used in the
  index.  The table contains the number of bits set in the index.
 */
const byte uim_y_format_table[16] = {
/* 0    1    2    3    4    5    6    7                                      */
   0,   1,   1,   2,   1,   2,   2,   3,
/* 8    9   10   11   12   13   14    15                                     */
   1,   2,   2,   3,   2,   3,   3,    4
};

/*
 * This is an APDU command buffer that holds the command for a supplemental
 * APDU transaction that is compiled and sent by the R-UIM driver
 */
LOCAL uim_req_buf_type cmd_req[UIM_NUM_DRV_SLOTS];

/*
 * This points to the response buffer for the current APDU transaction.
 */
LOCAL uim_rsp_buf_type *resp_buf[UIM_NUM_DRV_SLOTS];

/* Copy of the last sent APDU header */
LOCAL uim_apdu_type last_sent_apdu[UIM_NUM_DRV_SLOTS];

/* Copy of the last instrn case */
LOCAL uim_instrn_case_type uim_last_instrn_case[UIM_NUM_DRV_SLOTS];

/* Parity error counter */
LOCAL byte uim_parity_error_count = 0;

/* Overrun error counter */
LOCAL byte uim_overrun_error_count = 0;

/* Overrun error counter */
LOCAL byte uim_rx_break_error_count = 0;

/* Debug variable to maintain total parity error count since power_up */
dword uim_tot_parity_error_count = 0;

/* Debug variable to maintain total overrun error count since power_up */
dword uim_tot_overrun_error_count = 0;

/* Debug variable to maintain total rx_break error count since power_up */
dword uim_tot_rx_break_error_count = 0;


/* Debug variable to maintain total parity error count while in idle state */
dword uim_idle_parity_error_count = 0;

/* Debug variable to maintain total overrun error count while in idle state */
dword uim_idle_overrun_error_count = 0;

/* Debug variable to maintain total rx_break error count while in idle state */
dword uim_idle_rx_break_error_count = 0;

#ifdef UIM_DRIVER_TIMESTAMP
/* flag to indicate whether the timer is currently ON */
boolean uim_is_timer_on = FALSE;

/* flag to indicate whether to use driver timestamp mechanism */
LOCAL boolean uim_UseDriverTimeStamp = FALSE;
#endif

#ifdef FEATURE_UIM1
uim_drv_slot_type uim_drv_slot = UIM_DRV_SLOT1;
#else
uim_drv_slot_type uim_drv_slot = UIM_DRV_SLOT2;
#endif

/* Flags to indicate whether we received any warning while
   processing Envelope Command */
boolean uim_warning1_indicator = FALSE;
boolean uim_warning2_indicator = FALSE;

#ifdef FEATURE_UIM_DEBUG_LOG
#include "ts.h"

typedef struct
{
  byte    attrib;
  byte    the_char;
}uim_log_char_type;

uim_log_data_type         uim_log_data;
LOCAL uim_log_char_type   uim_log_char;

/*===========================================================================
  I N T E R N A L   F U N C T I O N   P R O T O T Y P E S
===========================================================================*/
LOCAL void uim_log_put_buffer(void);
LOCAL void uim_log_put_char(void);
LOCAL void uim_log_put_tstamp(void);
LOCAL void uim_enable_rx
(
  int rx_watermark_size,
  boolean sample_mode
);
LOCAL void uim_send_byte
(
  byte tx_byte
);
LOCAL void uim_send_t_1_byte
(
  byte tx_byte
);
LOCAL void rx_isr_process_tx_byte
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_process_guardtime
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_process_last_tx_byte
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL int uim_gcd(int uim_a,int uim_b);
LOCAL void set_uim_baud_rate
(
  uim_clock_rate_conversion_factor_type  FI,
  uim_baud_rate_adjustment_factor_type   DI,
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void uim_clock_control
(
  uim_clock_control_type uim_clock_control_req,
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void uim_initialize_state(void);
LOCAL void uim_send_apdu
(
  uim_req_buf_type *uim_req /* This points to the APDU command buffer, the
                                 command data buffer, the response buffer, the
                                 APDU status and the callback function.      */
);
LOCAL void uim_reset_receiver(void);
LOCAL void rx_isr_process_idle
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_receive_atr
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_receive_pps
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_process_resp_data
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_process_overrun_data
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_proc_procedure_bytes
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_normal_end
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_fetch_end
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_dload_err
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_resp_end
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_resend_apdu
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_busy
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_retries
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_reference
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_security
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_p3_bad
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_p1_p2_bad
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_ins_bad
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_cla_bad
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_problem
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_warnings1
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_warnings2
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_execn_err2
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_cmd_not_allowed
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_wrong_params
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_execn_err1
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_sw1_cla_not_supported
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_send_prologue
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_send_information
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_send_epilogue
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_rx_prologue
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_rx_information
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);
LOCAL void rx_isr_rx_epilogue
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
);

LOCAL boolean uim_check_to_do_get_response_if_seek
(
  void
);

/*===========================================================================

FUNCTION UIM_LOG_PUT_BUFFER

DESCRIPTION
  This function allocate memory and copy buffered data to the memory for diag.
  It initializes the buffer parameters when copying is done.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
LOCAL void uim_log_put_buffer(void)
{
  uim_log_data_type *log_ptr;

  if (uim_log_data.length == 0)
    return; /* Nothing to log */

  log_ptr = (uim_log_data_type *) log_alloc (LOG_UIM_DATA_C,
              FPOS(uim_log_data_type, data) + uim_log_data.length);

  if (log_ptr != NULL)
  {
    log_ptr->length = uim_log_data.length;

    memcpy( (void *) (log_ptr->data),
            (void *) uim_log_data.data,
            uim_log_data.length );

    log_commit((uim_log_data_type  *) log_ptr);
  }

  /* Reset the parameters */
  uim_log_data.length = 0;
  uim_log_char.attrib = UIM_LOG_ATTRIB_INIT;
}

/*===========================================================================

FUNCTION UIM_LOG_PUT_CHAR

DESCRIPTION
  This funciton put a byte along with the attrib byte to the buffer.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
LOCAL void uim_log_put_char(void)
{

  /* Store the attribute byte */
  uim_log_data.data[uim_log_data.length++] = uim_log_char.attrib;
  /* Store the byte */
  uim_log_data.data[uim_log_data.length++] = uim_log_char.the_char;

  if(uim_log_data.length  >= LOG_UIM_MAX_CHARS)
  {
    uim_log_put_buffer();
  }
}

/*===========================================================================

FUNCTION UIM_LOG_PUT_TSTAMP

DESCRIPTION
  This function put time stamp along with attrib byte to the buffer.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
LOCAL void uim_log_put_tstamp(void)
{
  qword t_stamp;

  uim_log_char.attrib = UIM_LOG_TSTAMP;

  /* Store the attribute byte */
  uim_log_data.data[uim_log_data.length++] = uim_log_char.attrib;

  /* Get time stamp */
  /*lint -e{792) supress void cast of void */
  ts_get(t_stamp);

  /* Copy the time stamp to the buffer */
  memcpy( (void *) &uim_log_data.data[uim_log_data.length],
          (void *) t_stamp,
          LOG_UIM_TSTAMP_SIZE);

  /* Update the buffer length */
  uim_log_data.length   += LOG_UIM_TSTAMP_SIZE;

  if(uim_log_data.length  >= LOG_UIM_MAX_CHARS)
  {
    uim_log_put_buffer();
  }
}

#ifdef FEATURE_UIM_LOG_APDU_TO_EFS
  /*===========================================================================
  MACRO UIM_LOG_PUT_BYTE
  ===========================================================================*/
  #define UIM_LOG_PUT_BYTE(attr, ch) uim_log_char.attrib = attr; \
                  uim_log_char.the_char = ch; uim_log_put_char();\
                  uim_efslog_apdu_to_buffer(attr,ch)
#else
  /*===========================================================================
  MACRO UIM_LOG_PUT_BYTE
  ===========================================================================*/
  #define UIM_LOG_PUT_BYTE(attr, ch) uim_log_char.attrib = attr; \
                  uim_log_char.the_char = ch; uim_log_put_char()
#endif /* !FEATURE_UIM_LOG_TO_EFS */


/*===========================================================================
MACRO UIM_LOG_PUT_TSTAMP
===========================================================================*/
#define UIM_LOG_PUT_TSTAMP() uim_log_put_tstamp()

/*===========================================================================
MACRO UIM_LOG_PUT_LAST_BYTE
===========================================================================*/
#define UIM_LOG_PUT_LAST_BYTE() \
            uim_log_put_tstamp(); uim_log_put_buffer()


#endif /* FEATURE_UIM_DEBUG_LOG */
/*
 * Local function prototypes
 */
LOCAL void uim_rx_isr
(
  uim_drv_slot_type uim_drv_slot
);

LOCAL void uim_rx_isr_1
(
  void
);

#if defined( FEATURE_UIM2 )
LOCAL void uim_rx_isr_2
(
  void
);
#endif
LOCAL void send_subsequent_byte
(
  void
);



/*===========================================================================

FUNCTION UIM_ENABLE_RX                         INTERNAL FUNCTION

DESCRIPTION
  This function programs and enables the receiver on the UART.  The receiver
  may receive the bytes or may be programmed in sample mode.  The rx level
  is also programmed.

DEPENDENCIES
  This function assumes that the receiver has been programmed and or reset
  and that the interrupt mask has already been defined to process receive
  interrupts.

RETURN VALUE
  None

SIDE EFFECTS
  The UART will be programmed and enabled to receive characters over the I/O
  line.

===========================================================================*/
LOCAL void uim_enable_rx
(
  int rx_watermark_size,
  boolean sample_mode
)
{
  /* Reset the sample mode - in case it was enabled. */
  UIM_RESET_SAMPLE_MODE ();

  /* Reset the receiver to get rid of any data in the FIFO */
  UIM_RESET_RX ();

  /* Reset the error status */
  UIM_RESET_ERR ();

  /* Reset the break change interrupt */
  UIM_RESET_BREAK ();

  /* Program the receive level to the selected level  */
  UIM_RX_WATERLEVEL ( rx_watermark_size );

  /* Determine if sample mode is requested. */
  if (sample_mode)
  {
    UIM_SAMPLE_MODE ();
  } /* end if - sample mode is programmed */

  /* Enable the receiver */
  UIM_ENABLE_UART_RX ();

} /* end uim_enable_rx */


#ifdef FEATURE_UIM_UART_DM
/*===========================================================================
FUNCTION UIM_SEND_PPS_BYTES                         INTERNAL FUNCTION

DESCRIPTION
  This function sends the PPS bytes in the case of UART DM.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void uim_send_pps_bytes()
{

  byte pps_byte_0 = 0x00;
  byte pps_byte_1 = 0x00;
  byte pps_byte_2 = 0x00;
  byte pps_byte_3 = 0x00;
  uint32 pps_word = 0x00;

  /* Wait for the PPS response in UIM_RX_RECEIVE_PPS state */
  uim_rx_state[uim_drv_slot] = UIM_RX_RECEIVE_PPS;

  pps_byte_0 = *request_buffer_ptr[uim_drv_slot]++;
  pps_byte_1 = *request_buffer_ptr[uim_drv_slot]++;
  pps_byte_2 = *request_buffer_ptr[uim_drv_slot]++;
  pps_byte_3 = *request_buffer_ptr[uim_drv_slot]++;

  /* Frame the bytes into a word */
  pps_word = (pps_byte_3 << 24) | (pps_byte_2 << 16) | (pps_byte_1 << 8) | (pps_byte_0);

  /* Enable the TX_READY, RXSTALE and RXBREAK interrupts */
  UIM_PRG_IMR ( ( MSMU_IMR_TX_READY | MSMU_IMR_RXSTALE | MSMU_IMR_RXBREAK) );

  /* Wait for TXREADY event to make sure the PPS bytes have been seen successfully */
  uim_rx_sub_state = UIM_RX_WAIT_FOR_TXREADY;

  /* Wait for 4-byte PPS response */
  /* UART_DM SWI: number of bytes needs to be multipled by 16 */
  UIM_DM_RX_INIT(16);

  /* Enable the stale char event */
  UIM_ENABLE_RX_STALE_INT();

  /* Program the number of bytes to be sent as PPS request */
  UIM_NUM_CHARS_TO_SEND(num_bytes_to_send[uim_drv_slot]);

  /* Send the word*/
  UIM_SEND_TX_FIFO(pps_word);
}
#endif /* FEATURE_UIM_UART_DM */

/*===========================================================================

FUNCTION UIM_SEND_BYTE                         INTERNAL FUNCTION

DESCRIPTION
  This function sets up the driver to send a character.  The UART receiver
  is placed in sample mode.

DEPENDENCIES
  This function is called from either task context (on a request) or in ISR
  context (when sending a buffer of bytes).  This design assumes the UIM
  does NOT send characters until the request is sent out.  Also, this design
  assumes the Rx and Tx lines are connected, so the Rx ISR receives the
  transmitted byte.

RETURN VALUE
  None

SIDE EFFECTS
  This causes the Rx ISR to receive the sampled data of the transmitted byte.

===========================================================================*/
LOCAL void uim_send_byte
(
  byte tx_byte
)
{
  /* Determine if this is the last byte for this buffer */
  if ((num_bytes_to_send[uim_drv_slot] == 1) ||
      !send_all_remaining_bytes[uim_drv_slot])
  {
    /* Receive the last byte and look for an error signal or procedure byte */
    uim_rx_state[uim_drv_slot] = UIM_RX_LAST_TX_BYTE;
  }
  else
  {
#ifdef UIM_NO_SIM_GLUE_LOGIC
    /* Put the UART Rx in sample mode */
    uim_enable_rx ((int)((10 * NUM_BYTES_PER_BIT)), TRUE);

    /* Assume there is no error in the guardtime */
    num_samples_of_tx_byte[uim_drv_slot] = 21;
#endif /* UIM_NO_SIM_GLUE_LOGIC */

    /* Set the receive ISR state to process the samples of the transmitted
       byte and the error signal window */
    uim_rx_state[uim_drv_slot] = UIM_RX_TX_BYTE;
  } /* end if - this is the last byte of the current buffer */

#if defined( FEATURE_UIM_SUPPORT_INVERSE_CONVENTION ) && \
    !defined (FEATURE_UIM_SUPPORT_INVERSE_CONVENTION_FOR_TX_IN_HW)
  /* Check if the UIM link is operating in inverse convention */
  if (UIM_INVERSE_CONVENTION == uim_convention[uim_drv_slot])
  {
    tx_byte = change_convention[tx_byte];
  } /* end if - the UIM link is running in inverse convention */
#endif /* FEATURE_UIM_SUPPORT_INVERSE_CONVENTION && ! FOR_TX_IN_HW*/

  /* Send the byte. */
  UIM_SEND_TX_FIFO (tx_byte);

  /* Indicate the byte has been sent */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_SEND_BYTE);
} /* end uim_send_byte */
#if defined( FEATURE_UIM_T_1_SUPPORT )


/*===========================================================================

FUNCTION UIM_SEND_T_1_BYTE                        INTERNAL FUNCTION

DESCRIPTION
  This function sets up the driver to send a character and operate the UIM
  driver in T=1 protocol.

DEPENDENCIES
  This function is called from either task context (on a request) or in ISR
  context (when sending a buffer of bytes).  This design assumes the UIM
  does NOT send characters until the request is sent out.  Also, this design
  assumes the Rx and Tx lines are connected, so the Rx ISR receives the
  transmitted byte.

RETURN VALUE
  None

SIDE EFFECTS
  This causes the Rx ISR to receive the transmitted byte.  The ISR states
  process the bytes in T=1 protocol.

===========================================================================*/
LOCAL void uim_send_t_1_byte
(
  byte tx_byte
)
{
#if defined( FEATURE_UIM_SUPPORT_INVERSE_CONVENTION ) && \
    !defined (FEATURE_UIM_SUPPORT_INVERSE_CONVENTION_FOR_TX_IN_HW)
  /* Check if the UIM link is operating in inverse convention */
  if (UIM_INVERSE_CONVENTION == uim_convention[uim_drv_slot])
  {
    tx_byte = change_convention[tx_byte];
  } /* end if - the UIM link is running in inverse convention */
#endif /* FEATURE_UIM_SUPPORT_INVERSE_CONVENTION && ! FOR_TX_IN_HW*/

  /* Send the byte. */
  UIM_SEND_TX_FIFO (tx_byte);

  /* Keep track of the number of bytes transmitted */
  uim_num_txd_bytes++;

  /* Indicate the byte has been sent */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_SEND_BYTE);
} /* end uim_send_t_1_byte */
#endif /* FEATURE_UIM_T_1_SUPPORT */


/*===========================================================================

FUNCTION RX_ISR_PROCESS_TX_BYTE                         INTERNAL FUNCTION

DESCRIPTION
  This function processes the sample bytes that result from sending a character
  to the UIM.  The sample bytes include the samples of the character sent as
  well as one bit time afterward for error checking.

DEPENDENCIES
  This function processes the bytes in the UART receive FIFO as samples of the
  transmitted byte and error signal.  This function assumes the UART receiver
  is in sample mode and the byte was sent out via uim_send_byte.

RETURN VALUE
  This function saves the result of the error signal processing for the
  uim_process_guardtime function.  It uses the following status:
    GUARDTIME_RESEND_BYTE,          There was an error detected in the
                                    guardtime
    GUARDTIME_BYTE_SENT_SUCCESSFUL  There was no error indication in the
                                    guardtime


SIDE EFFECTS
  This function saves the error signal indicator for the
  uim_process_guardtime function.  Also, when transmitting the last byte
  of a buffer, it does NOT wait for the end of the guardtime so that this
  driver may set up the UART receiver to receive procedure bytes soon
  enough.

===========================================================================*/
LOCAL void rx_isr_process_tx_byte
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  dword uart_status;  /* Holds the UART status */
#ifdef UIM_NO_SIM_GLUE_LOGIC
  /* This holds the byte read from the receive FIFO */
  volatile byte rx_value;
#endif /* UIM_NO_SIM_GLUE_LOGIC */
  //byte isr_reg;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef UIM_NO_SIM_GLUE_LOGIC
  /* Get the next 21 sample bytes.  This makes up the transmitted byte
     as well as the error signal bit. */
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();
  rx_value = UIM_READ_RX_FIFO ();

  /* Look for the error signal in the second half of the 11th bit. */
  /* Look at byte 21 because only the second half of the first bit
     makes it to the sample buffer */
  if (rx_value != NO_ERROR)
  {
    /* Indicate a need to re-send the byte.  Keep this status
       around until the entire guardtime has been processed.             */
    guardtime_error_return_code[uim_drv_slot] = GUARDTIME_RESEND_BYTE;
  }
  else /* The byte was sent successfully */
  {
    /* Indicate a need to re-send the byte.  Keep this status
       around until the entire guardtime has been processed.             */
    guardtime_error_return_code[uim_drv_slot] = GUARDTIME_BYTE_SENT_SUCCESSFUL;
  } /* end if - there is an error within the guardtime */

  /* Now process the guardtime samples */
  uim_rx_state[uim_drv_slot] = UIM_RX_GUARDTIME;
#else /* !UIM_NO_SIM_GLUE_LOGIC */

#ifdef UIM_DRIVER_TIMESTAMP
  if(!uim_UseDriverTimeStamp)
#endif /* UIM_DRIVER_TIMESTAMP */
  {
    /* Make sure there is no problem with the received byte */
    //if ((isr_reg = UIM_READ_ISR()) & MSMU_ISR_TX_ERROR)
    if (UIM_READ_ISR() & MSMU_ISR_TX_ERROR)
    {
      /* Clear the Tx error status */
      UIM_RESET_TX_ERROR();
    } /* end if - check for parity error. */
  }
  /* Clear out the Rx FIFO */
  for (uart_status = UIM_READ_STATUS();
       (uart_status & MSMU_SR_RXRDY);
       uart_status = UIM_READ_STATUS())
  {
    /* Get the transmitted byte */
    (void)UIM_GET_RX_BYTE ();
  }

  /* Since the HW processes errors, the software does not have to. */
  guardtime_error_return_code[uim_drv_slot] = GUARDTIME_BYTE_SENT_SUCCESSFUL;
#ifdef UIM_DRIVER_TIMESTAMP
  if(uim_UseDriverTimeStamp)
  {
    /* Make sure there is no problem with the received byte */
    //if ((isr_reg = UIM_READ_ISR()) & MSMU_ISR_TX_ERROR)
    if (UIM_READ_ISR() & MSMU_ISR_TX_ERROR)
    {
      /* Clear the Tx error status */
      UIM_RESET_TX_ERROR();
    } /* end if - check for parity error. */
    else
    {
      /* Send the next byte */
      send_subsequent_byte ();
    }
  }
  else
  {
    send_subsequent_byte ();
  }
#else
  /* Send the next byte */
  send_subsequent_byte ();
#endif /* UIM_DRIVER_TIMESTAMP */
#endif /* UIM_NO_SIM_GLUE_LOGIC */

} /* end rx_isr_process_tx_byte */


/*===========================================================================

FUNCTION RX_ISR_PROCESS_GUARDTIME                         INTERNAL FUNCTION

DESCRIPTION
  This function processes the sample bytes of the guardtime after a character
  is transmitted.  The purpose of this function is to wait for th end of the
  guartime before sending the next byte.

DEPENDENCIES
  This function processes the bytes in the UART receive FIFO as samples of the
  guardtime.  This function assumes the UART receiver is in sample mode and
  the byte was sent out via uim_send_byte.

RETURN VALUE
  None.

SIDE EFFECTS
  This function uses the guardtime_error_return_code to determine whether to
  resend the last byte or process the next.

===========================================================================*/
LOCAL void rx_isr_process_guardtime
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Process all the bytes in the receive FIFO */
  //while ((uart_status = UIM_READ_STATUS ())  & MSMU_SR_RXRDY)
  while (UIM_READ_STATUS()  & MSMU_SR_RXRDY)
  {
    (void)UIM_READ_RX_FIFO ();
    ++num_samples_of_tx_byte[uim_drv_slot];

    /* Determine if the entire guardtime has been processed */
    if (num_samples_of_tx_byte[uim_drv_slot] >=
        (((10 * NUM_BYTES_PER_BIT) - 1) + num_guardtime_bytes[uim_drv_slot]))
    {
      /* Reset the receiver to receive the response */
      uim_enable_rx (UIM_RX_WATER_MARK_LEVEL_FIVE, FALSE);

      /* Determine whether to re-send the last byte */
      if (guardtime_error_return_code[uim_drv_slot] == GUARDTIME_RESEND_BYTE)
      {
        /* Re-send the last byte */
        uim_send_byte (*request_buffer_ptr[uim_drv_slot]);
#ifdef FEATURE_UIM_DEBUG_LOG
        UIM_LOG_PUT_BYTE(UIM_LOG_TX_REPEAT, *request_buffer_ptr[uim_drv_slot]);
#endif
      }
      else /* The byte was sent successfully.  Process the next byte */
      {
        /* Send the next byte */
        send_subsequent_byte ();
      } /* end if - need to re-send the last byte */

      break;
    } /* end if - the guardtime is being processed */
  } /* end while - process all the bytes in the receive FIFO. */

} /* end uim_process_guardtime */


/*===========================================================================

FUNCTION RX_ISR_PROCESS_LAST_TX_BYTE                      INTERNAL FUNCTION

DESCRIPTION
  This function processes the last byte sent in a buffer.  The receiver is
  is NOT in sample mode for the last byte.  This allows the UART to receive
  the procedure byte from the card without a problem.  If there is an error
  signal from the card, it will manifest itself as another byte with the value
  of 0xFF or 0xFE.  If such a byte follows the transmitted byte, the
  transmitted byte must be re-sent.  Otherwise, it is a procedure byte.

DEPENDENCIES
  This function processes the bytes in the UART receive FIFO as the last byte
  transmitted with the chance of an error signal (second byte) or a procedure
  byte.

RETURN VALUE
  None.

SIDE EFFECTS
  This function either re-sends the last byte or processes a procedure byte.

===========================================================================*/
LOCAL void rx_isr_process_last_tx_byte
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
#ifdef UIM_NO_SIM_GLUE_LOGIC
  /* This holds the UART status */
  volatile byte uart_status;
#endif /* UIM_NO_SIM_GLUE_LOGIC */
  dword isr_reg;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

#ifdef UIM_NO_SIM_GLUE_LOGIC
  /* Check if there is a receive error */
  if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  {
    /* Re-send the last byte */
    uim_send_byte (*request_buffer_ptr[uim_drv_slot]);
#ifdef FEATURE_UIM_DEBUG_LOG
    UIM_LOG_PUT_BYTE(UIM_LOG_TX_REPEAT, *request_buffer_ptr[uim_drv_slot]);
#endif
    return;
  }
#else /* !UIM_NO_SIM_GLUE_LOGIC */

#ifndef T_UIM_NO_RX_WATERLEVEL
  /* Program the receive level to the wait after receiving the procedure
     byte(s).  */
  UIM_RX_WATERLEVEL ( UIM_RX_WATER_MARK_LEVEL_FIVE );
#endif
  isr_reg = UIM_READ_ISR();
  /* Make sure there is no problem with the received byte */
  if (isr_reg & MSMU_ISR_TX_ERROR)
  {
    /* Drop this byte as it will be re-transmitted */
    (void)UIM_GET_RX_BYTE ();

    /* Clear the Tx error status */
    UIM_RESET_TX_ERROR();

#ifdef UIM_DRIVER_TIMESTAMP
    if (uim_UseDriverTimeStamp)
    {
      /* we might not already have another byte if we have
       * one, we will come back here any way
       */

    return;
    }
#endif
  } /* end if - check for tx error. */
#endif /* UIM_NO_SIM_GLUE_LOGIC */

  /* Get the transmitted byte */
  (void)UIM_GET_RX_BYTE ();

  /* Process the next byte as a procedure byte */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROC_PRCDR_BYTES;

#ifdef FEATURE_UIM_DEBUG_LOG
    UIM_LOG_PUT_TSTAMP();
#endif /* FEATURE_UIM_DEBUG_LOG */

} /* end rx_isr_process_last_tx_byte */


/*===========================================================================

FUNCTION UIM_GCD                         INTERNAL FUNCTION

DESCRIPTION
  This function calculates the GCD (greatest common divisor) of two integers.
  This is the standard Euclid's GCD calculation algorithm.

DEPENDENCIES
  None

RETURN VALUE
  GCD value

SIDE EFFECTS
  None

===========================================================================*/
LOCAL int uim_gcd(int uim_a,int uim_b)
{
  int uim_c;
  while(1)
  {
    uim_c = uim_a%uim_b;

    if(uim_c==0)
      return uim_b;

    uim_a = uim_b;

    uim_b = uim_c;
  }
} /* end uim_gcd */

/*===========================================================================

FUNCTION SET_UIM_BAUD_RATE                         INTERNAL FUNCTION

DESCRIPTION
  Changes the UART clock via MND values.  The clock is changed to change the
  baud rate.

DEPENDENCIES
  The clock select register for the UART should be set to F (115.2k baud) to get
  the desired baudrate.  The clock select register may be different if using
  sample mode on receive.

RETURN VALUE
  None

SIDE EFFECTS
  This changes the baud rate of the UART.

===========================================================================*/
LOCAL void set_uim_baud_rate
(
  uim_clock_rate_conversion_factor_type  FI,
  uim_baud_rate_adjustment_factor_type   DI,
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{

#if defined(FEATURE_UIM_NO_LEGACY_MND_TABLE) && defined(FEATURE_UIM_UART_DM)

  word mreg;        /* Holds the M value */
  word nreg;        /* Holds the N value   */
  unsigned int temp1 = 0; /* temporary variable for M, N, D reg calculations */
  unsigned int temp2 = 0; /* temporary variable for M, N, D reg calculations */
  unsigned int temp3 = 0; /* temporary variable for M, N, D reg calculations */
  /*
   Per the UART_DM's design, the following equation holds good:
  (M/N)*(uart2_dm_clk_src/(csr * 16) = (uart2_dm_clk_src)/(div * td8 * (Fi/Di))

  In LCU, we choose the following values
  1. csr = 1,
  2. uart2_dm_clk_src = tcxo,
  3. uart2_dm_clk_src = tcxo/5,
  4. div = 1,
  5. td8 = 1 ...for LCU uart
  6. Fi = FI (first argument to this function)
  7. Di = DI (second argument to this function)

  With the above values, reducing the equation would give us:
  (M/N) = (16*Di)/(5*Fi)

  But, the goal is to make sure GCD of M and N is 1. For this, we need to divide
  numerator and denominator with their gcd. That gives a finer reduction of the
  M and N.
  */

  /* Store the current FI and DI */
  uim_FI_current = FI;
  uim_DI_current = DI;

  mreg = (16*braf_values[DI])/uim_gcd((int)(16*braf_values[DI]),(int)(5*crcf_values[FI]));
  nreg =  (5*crcf_values[FI])/uim_gcd((int)(16*braf_values[DI]),(int)(5*crcf_values[FI]));

#ifdef FEATURE_UIM_QSC11X0_DRIVER
  temp1 = (mreg<<16) & HWIO_FMSK(GSBI0_MN_REG, M_VAL);
  temp2 = (~((word)(nreg-mreg))) & HWIO_FMSK(GSBI0_MN_REG, N_VAL);
  temp3 = (~((word)(nreg))) & HWIO_FMSK(GSBI0_CD_REG, D_VAL);


  HWIO_OUT(GSBI0_MN_REG, temp1|temp2);
  HWIO_OUT(GSBI0_CD_REG,
           CR_FVAL(GSBI0_CD_REG, SRC_SEL, XO) |
           CR_FVAL(GSBI0_CD_REG, DIV_MND_EN, ENABLED) |
           CR_FVAL(GSBI0_CD_REG, DIV_MND_MODE, DUAL_EDGE_MODE) |
           CR_FVAL(GSBI0_CD_REG, SIM_SRC_SEL, XO) |
           CR_FVAL(GSBI0_CD_REG, SIM_DIV_MOD, DIV_5) |
           CR_FVAL(GSBI0_CD_REG, GSBI0_CLK_EN, ENABLED) |
           CR_FVAL(GSBI0_CD_REG, GSBI0_SIM_CLK_EN, ENABLED) |
           temp3
           );

  return;
#elif defined(T_QSC6270)
  temp1 = (mreg<<16);
  temp2 = (~((word)(nreg))) & 0xFFFF;
  temp3 = (~(nreg-mreg)<<16)& 0xFFFF0000;

  HWIO_OUT(UART2_DM_CLK_MD,temp1|temp2);
  HWIO_OUT(UART2_DM_CLK_NS,temp3|0x500);
#endif /* FEATURE_UIM_QSC11X0_DRIVER */
#else

  /* Holds the baud rate adjustment factor */
  byte baud_rate_adjustment_factor;
  word mreg;        /* Holds the M value */
  word nreg;        /* Holds the N value   */
  word dreg;        /* Holds the D value   */

#ifdef FEATURE_UIM_UART_DM
  unsigned int temp1 = 0; /* temporary variable for M, N, D reg calculations */
  unsigned int temp2 = 0; /* temporary variable for M, N, D reg calculations */
  unsigned int temp3 = 0; /* temporary variable for M, N, D reg calculations */
#endif /* FEATURE_UIM_UART_DM */

#if !defined( T_UIM_UART2_NON_SPLIT_MND_REGS )
  byte mreg_msb;    /* Holds the MREG_MSB value   */
  word nreg_msb;    /* Holds the NREG_MSB value   */
  word dreg_msb;    /* Holds the DREG_MSB value   */
  byte mnd_lsb;     /* Holds the MND_LSB value    */
#endif /* !defined T_UIM_UART2_NON_SPLIT_MND_REGS */

  /* Retrieve the UART clock values from the clock value table.
     The table values are defined for Di = 1.                                */
  mreg     = uart_clock_values[FI].mreg;
  nreg     = uart_clock_values[FI].nreg;
  dreg     = uart_clock_values[FI].dreg;

  /* Get the baud rate adjustment factor */
  baud_rate_adjustment_factor = braf_values[DI];

  /* Adjust the UART baud rate based upon the UIM clock frequency */
  switch (uim_clock_control_value[uim_drv_slot])
  {
    case UIM_CLOCK_TD4:
      {
        /* The baud rate adjustment factor is doubled because
           of the selected clock frequency */
        baud_rate_adjustment_factor *= 2;
        /* The M value is multiplied by Di.  This increases the
           clock rate and therefore, increases the baud rate. */
        mreg = (word)(mreg * baud_rate_adjustment_factor);
      }
      break;

#if defined( FEATURE_UIM_USE_TCXO_DIV_5 )
    case UIM_CLOCK_TD5:
      {
        /* To achieve the proper divide down for the UART baud rate, need to
           multiply the M value by 8 and multiply the N value by 5.
           Make sure this does not cause an overflow in the MND registers. */
        /* Multiply M by 8. */
        mreg = (word)(mreg * 8);
        /* Now multiply N by 5 */
        nreg = (word)(nreg * 5);

        /* The M value is multiplied by Di.  This increases the clock rate and
           therefore, increases the baud rate. */
        mreg = (word)(mreg * baud_rate_adjustment_factor);

#if !defined( T_UIM_UART2_NON_SPLIT_MND_REGS )
        /* Check if D (=nreg/2) value cannot fit into the DREG register as its
           width is 8 bits */
        if(nreg/2 >= UIM_MAX_D_VALUE_IN_SPLIT_MND_REGS)
        {
          /* Reduce the M/N ratio by dividing M and N with GCD(M,N).
             Re-calculate D = N/2. */
          mreg =  mreg/uim_gcd((int)(mreg),(int)(nreg));
          nreg =  nreg/uim_gcd((int)(mreg),(int)(nreg));

          /* Now compute the proper D value */
          dreg = nreg / 2;
        }
        else
#endif /* !defined( T_UIM_UART2_NON_SPLIT_MND_REGS ) */
        {
          /* Now compute the proper D value */
          dreg = nreg / 2;
        }

      }
      break;
#endif /* FEATURE_UIM_USE_TCXO_DIV_5 */

    case UIM_CLOCK_TD8:
    default:
       /* The M value is multiplied by Di.  This increases the clock rate and
          therefore, increases the baud rate                               */
          mreg = (word)(mreg * baud_rate_adjustment_factor);
      /* no change to divide down */
      break;
  }

#ifdef FEATURE_UIM_QSC11X0_DRIVER
  temp1 = (mreg<<16) & HWIO_FMSK(GSBI0_MN_REG, M_VAL);
  temp2 = (~((word)(nreg-mreg))) & HWIO_FMSK(GSBI0_MN_REG, N_VAL);
  temp3 = (~((word)(nreg))) & HWIO_FMSK(GSBI0_CD_REG, D_VAL);


  HWIO_OUT(GSBI0_MN_REG, temp1|temp2);
  HWIO_OUT(GSBI0_CD_REG,
           CR_FVAL(GSBI0_CD_REG, SRC_SEL, XO) |
           CR_FVAL(GSBI0_CD_REG, DIV_MND_EN, ENABLED) |
           CR_FVAL(GSBI0_CD_REG, DIV_MND_MODE, DUAL_EDGE_MODE) |
           CR_FVAL(GSBI0_CD_REG, SIM_SRC_SEL, XO) |
           CR_FVAL(GSBI0_CD_REG, SIM_DIV_MOD, DIV_5) |
           CR_FVAL(GSBI0_CD_REG, GSBI0_CLK_EN, ENABLED) |
           CR_FVAL(GSBI0_CD_REG, GSBI0_SIM_CLK_EN, ENABLED) |
           temp3
           );
  return;
#elif defined(T_QSC6270)
  temp1 = (mreg<<16);
  temp2 = (~((word)(nreg))) & 0xFFFF;
  temp3 = (~(nreg-mreg)<<16)& 0xFFFF0000;

  HWIO_OUT(UART2_DM_CLK_MD,temp1|temp2);
  HWIO_OUT(UART2_DM_CLK_NS,temp3|0x500);
  return;
#else
#ifdef FEATURE_UIM_HANDLE_TA1_0X97_ATR
#if defined (FEATURE_UIM_TCXO_CLK_FREQ) && defined( FEATURE_UIM_USE_TCXO_DIV_5 )
  if(FI == UIM_CRCF_512 && DI == UIM_BRAF_64)
  {
    mreg = MREG_TA10X97;
    nreg = NREG_TA10x97;
    dreg = DREG_TA10x97;
  }
#endif /* FEATURE_UIM_TCXO_CLK_FREQ && FEATURE_UIM_USE_TCXO_DIV_5 */
#endif /* FEATURE_UIM_HANDLE_TA1_0X97_ATR */
#endif /* FEATURE_UIM_QSC11X0_DRIVER */

  /* The N-register value is ~(N-M).  Compute the N-register value. */
  nreg = ~((word)(nreg - mreg));

#if !defined( T_UIM_UART2_NON_SPLIT_MND_REGS )
  /* Split the M, N and D values into the register format.
     M-register = M bits 8:1
     N-register = ~(N-M) bits 10:3
     D-register = D bits 9:2
     MNDlsb = M-reg bit 0; N-reg bits 2:0; D-reg bits 1:0                    */
  mnd_lsb  = (mreg & MLSB_MASK) << MLSB_SHIFT;
  mreg_msb = (byte) (mreg >> MMSB_SHIFT);
  mnd_lsb |= (nreg & NLSB_MASK) << NLSB_SHIFT;
  nreg_msb = nreg >> NMSB_SHIFT;
  mnd_lsb |= dreg & DLSB_MASK;
  dreg_msb = dreg >> DMSB_SHIFT;

  /*-------------------------------------------------------------------------
   Program M, N, and D registers to the proper rates.  These four
   statements essentially inform UART of the system clock rate.  With the
   information, the UART can receive and transmit at the programmed clock
   rate.
  -------------------------------------------------------------------------*/
#if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
  if (UIM_DRV_SLOT1 == uim_drv_slot) {
    MSM_OUT( MSMU2_MVR,  mreg_msb);
    MSM_OUT( MSMU2_NVR,  nreg_msb);
    MSM_OUT( MSMU2_DVR,  dreg_msb);
    MSM_OUT( MSMU2_MNDR, mnd_lsb);
  }else{
    MSM_OUT( MSMU3_MVR,  mreg_msb);
    MSM_OUT( MSMU3_NVR,  nreg_msb);
    MSM_OUT( MSMU3_DVR,  dreg_msb);
    MSM_OUT( MSMU3_MNDR, mnd_lsb);}
#elif !defined( FEATURE_UIM2 )
  MSM_OUT( MSMU2_MVR,  mreg_msb);
  MSM_OUT( MSMU2_NVR,  nreg_msb);
  MSM_OUT( MSMU2_DVR,  dreg_msb);
  MSM_OUT( MSMU2_MNDR, mnd_lsb);
#else /* FEATURE_UIM2 */
  MSM_OUT( MSMU3_MVR,  mreg_msb);
  MSM_OUT( MSMU3_NVR,  nreg_msb);
  MSM_OUT( MSMU3_DVR,  dreg_msb);
  MSM_OUT( MSMU3_MNDR, mnd_lsb);
#endif

#else
  /*-------------------------------------------------------------------------
   Program M, N, and D registers to the proper rates.  These three
   statements essentially inform UART of the system clock rate.  With the
   information, the UART can receive and transmit at the programmed clock
   rate.
  -------------------------------------------------------------------------*/
#if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
  if (UIM_DRV_SLOT1 == uim_drv_slot) {
    MSM_OUT( MSMU2_MVR,  mreg);
    MSM_OUT( MSMU2_NVR,  nreg);
    MSM_OUT( MSMU2_DVR,  dreg);
  }else{
    MSM_OUT( MSMU3_MVR,  mreg);
    MSM_OUT( MSMU3_NVR,  nreg);
    MSM_OUT( MSMU3_DVR,  dreg);}
#elif !defined( FEATURE_UIM2 )
  MSM_OUT( MSMU2_MVR,  mreg);
  MSM_OUT( MSMU2_NVR,  nreg);
  MSM_OUT( MSMU2_DVR,  dreg);
#else /* FEATURE_UIM2 */
  MSM_OUT( MSMU3_MVR,  mreg);
  MSM_OUT( MSMU3_NVR,  nreg);
  MSM_OUT( MSMU3_DVR,  dreg);
#endif

#endif /* !defined T_UIM_UART2_NON_SPLIT_MND_REGS */

#endif /* FEATURE_UIM_UART_DM */

} /* end set_uim_baud_rate */


/*===========================================================================

FUNCTION UIM_RESET_UART                         INTERNAL FUNCTION

DESCRIPTION
  This function resets the UART used to communicate with the UIM.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The UART will no longer be transmitting nor receiving data.  The receive
  and transmit FIFOs will be flushed.

===========================================================================*/
static void uim_reset_uart
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{

  #ifdef FEATURE_UIM_UART_DM
    UIM_RESET_RX ();
    UIM_RESET_TX ();
  #else

  /* Disable UART Tx and Rx and reset the Rx error status. */
  UIM_DISABLE_UART_RX ();
  UIM_DISABLE_UART_TX ();
  UIM_RESET_ERR ();
  UIM_RESET_BREAK ();
  UIM_RESET_TX_ERROR ();

  /*-------------------------------------------------------------------------
   Double reset is necessary for some old revisions of hardware.
  -------------------------------------------------------------------------*/
  UIM_RESET_RX ();

  /*-------------------------------------------------------------------------
   Double reset is necessary for some old revisions of hardware.
  -------------------------------------------------------------------------*/
  UIM_RESET_TX ();

  #endif /* FEATURE_UIM_UART_DM */

} /* end uim_reset_uart */

/*===========================================================================

FUNCTION UIM_CLOCK_CONTROL                           INTERNAL FUNCTION

DESCRIPTION
  Programs the UIM clock for either a digital value or a frequency.

DEPENDENCIES
  None.

RETURN VALUE
  None

SIDE EFFECTS
  This changes the clock signal to the UIM.

===========================================================================*/
LOCAL void uim_clock_control
(
  uim_clock_control_type uim_clock_control_req,
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
/* MSM3300, MSM5105, MSM5100, MSM6050 */
#if !defined (UIM_NO_SIM_GLUE_LOGIC)
  static dword uim_preferred_clock_stop_ctl[UIM_NUM_DRV_SLOTS] = {
#if defined( FEATURE_UIM1 )
    UART_SIM_CFG__SIM_CLK_OFF
#endif /* FEATURE_UIM1 */
#if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    ,
#endif /* FEATURE_UIM1 && FEATURE_UIM2 */
#if defined( FEATURE_UIM2 )
    UART_SIM_CFG__SIM_CLK_OFF
#endif /* FEATURE_UIM2 */
  };

  static dword uim_preferred_clock_ctl[UIM_NUM_DRV_SLOTS] = {
#if defined( FEATURE_UIM1 )
    UART_SIM_CFG__SIM_CLK_TD8_SEL_MASK
#endif /* FEATURE_UIM1 */
#if defined( FEATURE_UIM1 ) && defined( FEATURE_UIM2 )
    ,
#endif /* FEATURE_UIM1 && FEATURE_UIM2 */
#if defined( FEATURE_UIM2 )
    UART_SIM_CFG__SIM_CLK_TD8_SEL_MASK
#endif /* FEATURE_UIM2 */
  };

  switch (uim_clock_control_req)
  {
    /* Leave the clock signal LOW                      */
    case UIM_CLOCK_LOW:
      uim_preferred_clock_stop_ctl[uim_drv_slot] = UART_SIM_CFG__SIM_CLK_OFF;
      UIM_UART_CNFG( UART_SIM_CFG__CLK_MASK,
                     UART_SIM_CFG__SIM_CLK_OFF |
                     uim_preferred_clock_stop_ctl[uim_drv_slot] |
                     uim_preferred_clock_ctl[uim_drv_slot]);
      break;
    /* Leave the clock signal HIGH                     */
    case UIM_CLOCK_HIGH:
      uim_preferred_clock_stop_ctl[uim_drv_slot] =
        UART_SIM_CFG__SIM_CLK_STOP_HI_MASK;
      UIM_UART_CNFG( UART_SIM_CFG__CLK_MASK,
                     UART_SIM_CFG__SIM_CLK_OFF |
                     uim_preferred_clock_stop_ctl[uim_drv_slot] |
                     uim_preferred_clock_ctl[uim_drv_slot]);
      break;
    /* Program the clock at tcxo/8                     */
    case UIM_CLOCK_TD8:
      uim_preferred_clock_ctl[uim_drv_slot] =
        UART_SIM_CFG__SIM_CLK_TD8_SEL_MASK;

      /* Program the UIM clock */
      UIM_UART_CNFG( UART_SIM_CFG__CLK_MASK,
                     UART_SIM_CFG__SIM_CLK_ON_MASK |
                     uim_preferred_clock_stop_ctl[uim_drv_slot] |
                     uim_preferred_clock_ctl[uim_drv_slot]);
      break;
#if defined( FEATURE_UIM_USE_TCXO_DIV_5 )
    /* Program the clock at tcxo/5                     */
    case UIM_CLOCK_TD5:
      /* Evaluate the UIM clock frequency and conversion factor */
      uim_clk_freq[uim_drv_slot] = uim_clk_source_freq[uim_drv_slot]/5;

#ifdef FEATURE_UIM_UART_DM
      uim_preferred_clock_ctl[uim_drv_slot] = 0;
#else
#if defined(T_MSM7600)
#error code not present
#else
      uim_preferred_clock_ctl[uim_drv_slot] =
        UART_SIM_CFG__SIM_CLK_TD5_SEL_MASK;
#endif
#endif /* FEATURE_UIM_UART_DM */
      /* Program the UIM clock */
      UIM_UART_CNFG( UART_SIM_CFG__CLK_MASK,
                     UART_SIM_CFG__SIM_CLK_ON_MASK |
                     uim_preferred_clock_stop_ctl[uim_drv_slot] |
                     uim_preferred_clock_ctl[uim_drv_slot]);
      break;
#endif /* FEATURE_UIM_USE_TCXO_DIV_5 */
    /* Program the clock at tcxo/4                     */
    case UIM_CLOCK_TD4:
      /* Evaluate the UIM clock frequency and conversion factor */
#ifdef FEATURE_UIM_MSM6800_TCXO_DIV5
      uim_clk_freq[uim_drv_slot] = uim_clk_source_freq[uim_drv_slot]/5;
#elif defined(T_MSM6280) && defined(FEATURE_UIM_3P84MHZ_M6280_PLATFORM)
#error code not present
#else
        uim_clk_freq[uim_drv_slot] = uim_clk_source_freq[uim_drv_slot]/4;
#endif /* FEATURE_UI_MSM6800_TCXO_DIV5 */
      uim_preferred_clock_ctl[uim_drv_slot] =
        UART_SIM_CFG__SIM_CLK_TD4_SEL_MASK;

      /* Program the UIM clock */
      UIM_UART_CNFG( UART_SIM_CFG__CLK_MASK,
                     UART_SIM_CFG__SIM_CLK_ON_MASK |
                     uim_preferred_clock_stop_ctl[uim_drv_slot] |
                     uim_preferred_clock_ctl[uim_drv_slot]);
      break;
    default:
      break;
  }

#else /* MSM3000, MSM3100 and MSM5000 uses YAMN1 */
  byte clk_mdiv_lsb;    /* Holds the M value of the M/N counter              */
  byte clk_mdiv_msb;
  byte clk_ndiv_lsb;    /* Holds the N value of the M/N counter              */
  byte clk_ndiv_msb;
  byte clk_duty_lsb;    /* Holds the Duty value of the M/N clock signal      */
  byte clk_duty_msb;

  /* Program the UIM clock according to the parameter. */
  switch (uim_clock_control_req)
  {
    case UIM_CLOCK_LOW:
      {
        /* This is the M value when the clock is held LOW */
        clk_mdiv_lsb = UIM_CLK_MDIV_LSB_LOW;
        clk_mdiv_msb = UIM_CLK_MDIV_MSB_LOW;
        /* This is the N value when the clock is held LOW */
        clk_ndiv_lsb = UIM_CLK_NDIV_LSB_LOW;
        clk_ndiv_msb = UIM_CLK_NDIV_MSB_LOW;
        /* This is the DUTY value when the clock is held LOW */
        clk_duty_lsb = UIM_CLK_DUTY_LSB_LOW;
        clk_duty_msb = UIM_CLK_DUTY_MSB_LOW;
      }
      break;

    case UIM_CLOCK_HIGH:
      {
        /* This is the M value when the clock is held HIGH */
        clk_mdiv_lsb = UIM_CLK_MDIV_LSB_HIGH;
        clk_mdiv_msb = UIM_CLK_MDIV_MSB_HIGH;
        /* This is the N value when the clock is held HIGH */
        clk_ndiv_lsb = UIM_CLK_NDIV_LSB_HIGH;
        clk_ndiv_msb = UIM_CLK_NDIV_MSB_HIGH;
        /* This is the DUTY value when the clock is held HIGH */
        clk_duty_lsb = UIM_CLK_DUTY_LSB_HIGH;
        clk_duty_msb = UIM_CLK_DUTY_MSB_HIGH;
      }
      break;

    case UIM_CLOCK_TD8:
      {
        /* Evaluate the UIM clock frequency and conversion factor */
        uim_clk_freq[UIM_DRV_SLOT1] = uim_clk_source_freq/8;
        /* This is the M value to get tcxo/8.                 */
        clk_mdiv_lsb = UIM_CLK_MDIV_LSB_TD8;
        clk_mdiv_msb = UIM_CLK_MDIV_MSB_TD8;
        /* This is the N value to get tcxo/8.                 */
        clk_ndiv_lsb = UIM_CLK_NDIV_LSB_TD8;
        clk_ndiv_msb = UIM_CLK_NDIV_MSB_TD8;
        /* This is the DUTY value on tcxo/8 at 50% duty cycle */
        clk_duty_lsb = UIM_CLK_DUTY_LSB_TD8;
        clk_duty_msb = UIM_CLK_DUTY_MSB_TD8;
      }
      break;

    default:
      return;
      break;
  } /* end switch (uim_clock_control) */

  /* Now program the clock */
  MSM_OUT (UIM_CLK_MDIV_LSB, clk_mdiv_lsb);
  MSM_OUT (UIM_CLK_MDIV_MSB, clk_mdiv_msb);
  MSM_OUT (UIM_CLK_NDIV_LSB, clk_ndiv_lsb);
  MSM_OUT (UIM_CLK_NDIV_MSB, clk_ndiv_msb);
  MSM_OUT (UIM_CLK_DUTY_LSB, clk_duty_lsb);
  MSM_OUT (UIM_CLK_DUTY_MSB, clk_duty_msb);

#endif /* T_MSM5105 */

  /* Save the clock control for use elsewhere */
  uim_clock_control_value[uim_drv_slot] = uim_clock_control_req;
} /* end - uim_clock_control */


/*===========================================================================

FUNCTION UIM_INITIALIZE_STATE                         INTERNAL FUNCTION

DESCRIPTION
  Initialize state variables for the R-UIM device driver.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void uim_initialize_state
(
  void
)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Initialize the uim Rx character ISR processing state. */
#if defined( FEATURE_UIM1 )
  uim_rx_state[UIM_DRV_SLOT1] = UIM_RX_PROCESS_IDLE;
#endif /* FEATURE_UIM1 */
#if defined( FEATURE_UIM2 )
  uim_rx_state[UIM_DRV_SLOT2] = UIM_RX_PROCESS_IDLE;
#endif /* FEATURE_UIM2 */

#if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES )
#if defined( FEATURE_UIM1 )
  /* Set the UIM interface voltage to the lowest voltage */
  uim_current_voltage_class[UIM_DRV_SLOT1] = UIM_MIN_VOLTAGE;
#endif /* FEATURE_UIM1 */
#if defined( FEATURE_UIM2 )
  uim_current_voltage_class[UIM_DRV_SLOT2] = UIM_MIN_VOLTAGE;
#endif /* FEATURE_UIM2 */
#endif /* FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES */

  /* Initialize the number of sample bytes variable(s) */
#if defined( FEATURE_UIM1 )
  num_samples_of_tx_byte[UIM_DRV_SLOT1] = 21;
#endif /* FEATURE_UIM1 */
#if defined( FEATURE_UIM2 )
  num_samples_of_tx_byte[UIM_DRV_SLOT2] = 21;
#endif /* FEATURE_UIM2 */

} /* end - uim_initialize_state */


/*===========================================================================

FUNCTION UIM_DEV_INIT                             EXTERNALIZED FUNCTION

DESCRIPTION
  Initialize the clock regime, configures the GPIOs, MSM UART serial
  interface.  It places the UIM power line low, the UIM reset line low,
  the I/O line low and the clock line low.

DEPENDENCIES
  The routine MUST be called before any other UIM driver function.  It must
  only be called once on power up of the phone.

RETURN VALUE
  None

SIDE EFFECTS
  This function leaves ALL the UIM lines low after it executes.

===========================================================================*/
void uim_dev_init
(
  void
)
{
   /*@-shadow@*/ uim_drv_slot_type uim_drv_slot; /*@+shadow@*/ /* Slot control variable */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  MSG_LOW( "uim_dev_init", 0, 0, 0 );
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_DEV_INIT);

#ifdef FEATURE_UIM_INTERFACE_NOT_USED
  /* Just return without initializing the UART, PMIC etc */
  return;
#endif /* FEATURE_UIM_INTERFACE_NOT_USED */

#if defined (T_MSM6050) || defined (T_MSM7200)
#if defined( FEATURE_UIM1 )
  /*-------------------------------------------------------------------------
   Program the clock source for the UIM UART to be tcxo/4.  This is necessary
   because the SIM glue logic that provides the clock to the UIM must be
   driven by tcxo/4 to achieve UIM clock frequencies of tcxo/4 and tcxo/8.
  -------------------------------------------------------------------------*/
  UIM_SELECT_CLOCK_REGIME_SOURCE_1();
#endif /* FEATURE_UIM1 */
#if defined( FEATURE_UIM2 )
  /*-------------------------------------------------------------------------
   Program the clock source for the UIM UART to be tcxo/4.  This is necessary
   because the SIM glue logic that provides the clock to the UIM must be
   driven by tcxo/4 to achieve UIM clock frequencies of tcxo/4 and tcxo/8.
  -------------------------------------------------------------------------*/
  UIM_SELECT_CLOCK_REGIME_SOURCE_2();
#endif /* FEATURE_UIM2 */
#endif /* T_MSM6050 */

  /*-------------------------------------------------------------------------
   Do device specific initialization of state variables for device indicated
   by port_id.
  -------------------------------------------------------------------------*/

  uim_initialize_state( );

  /*-------------------------------------------------------------------------
   Turn on the clock for the R-UIM interface
  -------------------------------------------------------------------------*/
#if defined( FEATURE_UIM1 )
  uim_drv_slot = UIM_DRV_SLOT1;
  UIM_TCXO_MUST_BE_ON();
#endif /* FEATURE_UIM1 */
#if defined( FEATURE_UIM2 )
  uim_drv_slot = UIM_DRV_SLOT2;
  UIM_TCXO_MUST_BE_ON();
#endif /* FEATURE_UIM2 */

  /*-------------------------------------------------------------------------
   Set up Clock Regime Management.  UART clock regime controls both UARTs.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_UIM_CONTROL_UIM_UART_CLK_REGIME
  /* MSM5200, MSM6200, MSM6050, MSM6100, MSM6300 */
#if defined( FEATURE_UIM1 )
  UIM_CLOCK_REGIME_ENABLE_1();
#endif /* FEATURE_UIM1 */
#if defined( FEATURE_UIM2 )
  UIM_CLOCK_REGIME_ENABLE_2();
#endif /* FEATURE_UIM2 */
#else /* MSM3000, MSM3100, MSM3300, MSM5000, MSM5100, MSM5105, MSM6000 */
  clk_regime_enable( CLK_RGM_UART_M);
#endif

  /*-------------------------------------------------------------------------
   Select UIM configuration on the UART.  Start with direct convention.
  -------------------------------------------------------------------------*/
#if defined( FEATURE_UIM_SUPPORT_INVERSE_CONVENTION )
#if defined( FEATURE_UIM1 )
  uim_convention[UIM_DRV_SLOT1] = UIM_DIRECT_CONVENTION;
#endif /* FEATURE_UIM1 */
#if defined( FEATURE_UIM2 )
  uim_convention[UIM_DRV_SLOT2] = UIM_DIRECT_CONVENTION;
#endif /* FEATURE_UIM2 */
#endif /* FEATURE_UIM_SUPPORT_INVERSE_CONVENTION */

/* MSM3300, MSM5105, MSM5100, MSM6050 */
#if !defined (UIM_NO_SIM_GLUE_LOGIC)
#if defined( FEATURE_UIM1 )
  /* Initialize all bits in USIM_CFG register to zero.  Turn off UIM clock and
     keep UIM clock signal low */
  uim_drv_slot = UIM_DRV_SLOT1;
  UIM_UART_CNFG( UART_SIM_CFG__CLK_MASK |
                 UART_SIM_CFG__MASK,
                 UART_SIM_CFG__SIM_CLK_OFF );
  /* Set USIM_SEL to turn on USIM mode of operation */
  UIM_UART_CNFG( UART_SIM_CFG__CLK_MASK |
                 UART_SIM_CFG__MASK,
                 UART_SIM_CFG__SIM_CLK_OFF |
                 UART_SIM_CFG__SIM_SEL_MASK );
#if defined (T_MSM6100) || defined (T_MSM6275)
  /* Set the stop bit number to default (2) */
  UIM_UART_CNFG( UART_SIM_CFG__STOP_BIT_LEN_MASK,
                 UIM_DEFAULT_STOP_BIT_VALUE);
#endif /* T_MSM6100 */
#endif /* FEATURE_UIM1 */
#if defined( FEATURE_UIM2 )
  /* Initialize all bits in USIM_CFG register to zero.  Turn off UIM clock and
     keep UIM clock signal low */
  uim_drv_slot = UIM_DRV_SLOT2;
  UIM_UART_CNFG( UART_SIM_CFG__CLK_MASK |
                 UART_SIM_CFG__MASK,
                 UART_SIM_CFG__SIM_CLK_OFF );
  /* Set USIM_SEL to turn on USIM mode of operation */
  UIM_UART_CNFG( UART_SIM_CFG__CLK_MASK |
                 UART_SIM_CFG__MASK,
                 UART_SIM_CFG__SIM_CLK_OFF |
                 UART_SIM_CFG__SIM_SEL_MASK );
#if defined (T_MSM6100) || defined (T_UIM_SABER_DEFS) || defined (T_MSM6275)
  /* Set the stop bit number to default (2) */
  UIM_UART_CNFG( UART_SIM_CFG__STOP_BIT_LEN_MASK,
                 UIM_DEFAULT_STOP_BIT_VALUE);
#endif /* T_MSM6100 || T_UIM_SABER_DEFS */
#endif /* FEATURE_UIM2 */
#endif /* !UIM_NO_SIM_GLUE_LOGIC */

  /*-------------------------------------------------------------------------
   Disable IRDA tranceiver.  This is not used for the UIM.
  -------------------------------------------------------------------------*/
  UIM_DISABLE_IRDA ();

#ifdef FEATURE_UIM_UART_DM
  /* Disable the DM functionality */
  UIM_DISABLE_DATA_MOVER();
#endif /* FEATURE_UIM_UART_DM */

#if defined( FEATURE_UIM1 )
  /* Set the UART clock to the default values. */
  set_uim_baud_rate ( UIM_CRCF_372_1, UIM_BRAF_1, UIM_DRV_SLOT1);

#endif /* FEATURE_UIM1 */
#if defined( FEATURE_UIM2 )
  /* Set the UART clock to the default values. */
  set_uim_baud_rate ( UIM_CRCF_372_1, UIM_BRAF_1, UIM_DRV_SLOT2);
#endif /* FEATURE_UIM2 */

#if defined(T_UIM_UART1_ENABLE_SECONDARY_UART)
  /* Initialize rx data secondary uart register */
  MSM_OUT( UART1_RX_DATA_INT_CLEAR,
           UART1_RX_DATA_INT_CLEAR_RMSK );
#endif /* T_UIM_UART1_ENABLE_SECONDARY_UART */

#if defined( FEATURE_UIM1 )
  /* Set the current slot to UIM1 */
  uim_drv_slot = UIM_DRV_SLOT1;

  /* Mask all interrupts from UART. */
  UIM_PRG_IMR ( MSMU_IMR_MASK_ALL );

  #ifdef FEATURE_UIM_UART_DM
  /* Set the Stale character timeout to maximum possible */
  UIM_PRG_IPR(MSMU_IPR_DEF_STALE_TIMEOUT);
  #else
  /* Program one stale character timeout from the last byte received. */
  UIM_PRG_IPR ( (MSMU_IPR_NEW_RXSTALE | MSMU_IPR_TWO_CHARS) );

  /*-------------------------------------------------------------------------
   Reset the UART.  This flushes the Rx and Tx FIFOs on the UART.
  -------------------------------------------------------------------------*/
  uim_reset_uart (uim_drv_slot);
  #endif /* FEATURE_UIM_UART_DM */

  /*-------------------------------------------------------------------------
   Set the transmit FIFO watermark level.  This is used when sending out block
   transfers.
  -------------------------------------------------------------------------*/
  UIM_TX_WATERLEVEL ( 30 );

  /*-------------------------------------------------------------------------
   Disable the RFR and CTS hardware control.
  -------------------------------------------------------------------------*/

  #ifdef FEATURE_UIM_UART_DM
  /* MR1 register is insignificant in UART in SIM mode */
  UIM_PRG_MR1 (0x00);
  #else
  UIM_PRG_MR1 ( MSMU_MR1_DISABLE_RFR_CTS );
  #endif /* FEATURE_UIM_UART_DM */

  /*-------------------------------------------------------------------------
   Program for eight bits per character, 2 stop bit and character check error
   mode
  -------------------------------------------------------------------------*/

  #ifdef FEATURE_UIM_UART_DM
  UIM_PRG_MR2 ( 0x3E );
  #else
  UIM_PRG_MR2 ( (MSMU_MR2_8BPC | MSMU_MR2_2SB | MSMU_MR2_EPAR) );
  #endif /* FEATURE_UIM_UART_DM */

  /*-------------------------------------------------------------------------
   Set the UIM clock signal to the LOW state.
  -------------------------------------------------------------------------*/
  uim_clock_control ( UIM_CLOCK_LOW, uim_drv_slot );

  /*-------------------------------------------------------------------------
   Program the clock select for the Rx and Tx.  The baudrate is determined by
   the M/N clock counter.  This value is used for non-sample mode.
  -------------------------------------------------------------------------*/
  UIM_PRG_UART_CSR ( CSR_VALUE );

  /*-------------------------------------------------------------------------
   First, set the GPIO pins used for reset and power enable to a low state.
  -------------------------------------------------------------------------*/
  UIM_SET_RESET_AS_OUTPUT ();

#if defined ( T_MSM7500 ) && !defined( FEATURE_UIM_MDM8200_DRIVER )
  gpio_tlmm_config(UIM_RESET);
#endif /* T_MSM7500 */

  UIM_UIMDRV_PWR_DIS();

#ifdef FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM
#error code not present
#else
  UIM_POWER_OFF();
#endif /* FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM */

  UIM_ASSERT_RESET();

  /*-------------------------------------------------------------------------
   Set GPIO pin for power enable as output.
  -------------------------------------------------------------------------*/
  UIM_SET_POWER_AS_OUTPUT ();

  /*-----------------------------------------------------------------------
   Put the UART transmitter in break mode
  -----------------------------------------------------------------------*/
  UIM_START_BREAK ();

  /*-----------------------------------------------------------------------
   Enable the UART transmitter
  -----------------------------------------------------------------------*/
  UIM_ENABLE_UART_TX ();

#if defined( FEATURE_UIM_PMIC_ON_UIM1 )
  /*-----------------------------------------------------------------------
      ENABLE PM66XX UIM VOLTAGE TRANSLATION
  -----------------------------------------------------------------------*/

#ifdef FEATURE_UIM_USES_OLD_PMIC_API
#if !defined(T_QSC6270) && !defined(FEATURE_UIM_QSC11X0_DRIVER)
  /*     MPP5: RUIM_RST          MPP6: UIM_RST        */
  /*     MSM side (input)   =>   UIM side (drive low) */
  /*     VREF = MSMP VREG        VREF = MSMP          */
  (void)pm_mpp_config_digital_input( PM_MPP_5,
                                      PM_MPP__DLOGIC__LVL_MSMP,
                                       PM_MPP__DLOGIC_IN__DBUS_NONE);

  /*     MPP9: RUIM_CLK          MPP10: UIM_CLK       */
  /*     MSM side (input)   =>   UIM side (drive low) */
  /*     VREF = MSMP VREG        VREF = MSMP          */
  (void)pm_mpp_config_digital_input( PM_MPP_9,
                                      PM_MPP__DLOGIC__LVL_MSMP,
                                       PM_MPP__DLOGIC_IN__DBUS_NONE);
#endif /* !T_QSC6270 && !FEATURE_UIM_QSC11X0_DRIVER */
#else
#if !defined(T_QSC6270) && !defined(FEATURE_UIM_QSC11X0_DRIVER)
  /*     MPP5: RUIM_RST          MPP6: UIM_RST        */
  /*     MSM side (input)   =>   UIM side (drive low) */
  /*     VREF = MSMP VREG        VREF = MSMP          */
  (void)pm_mpp_config_digital_input( PM_MPP_5,
                                      PM_MPP__DLOGIC__LVL_MSMP);

  /*     MPP9: RUIM_CLK          MPP10: UIM_CLK       */
  /*     MSM side (input)   =>   UIM side (drive low) */
  /*     VREF = MSMP VREG        VREF = MSMP          */
  (void)pm_mpp_config_digital_input( PM_MPP_9,
                                      PM_MPP__DLOGIC__LVL_MSMP);
#endif /* !T_QSC6270 && !FEATURE_UIM_QSC11X0_DRIVER */
#endif /*FEATURE_UIM_USES_OLD_PMIC_API*/
#if !defined(T_QSC6270) && !defined(FEATURE_UIM_QSC11X0_DRIVER)
  /*     MPP11: RUIM_DATA        MPP12: UIM_DATA      */
  /*     MSM side (bidirec) <=>  UIM side (drive low) */
  /*     VREF = RUIM VREG        VREF = MSMP          */
  /*               Pull up resistor = 30K              */
  (void)pm_mpp_config_digital_inout( PM_MPP_11,
                                      PM_MPP__DLOGIC__LVL_MSMP,
                                       PM_MPP__DLOGIC_INOUT__PUP_30K);
#endif /* !T_QSC6270 && !defined(FEATURE_UIM_QSC11X0_DRIVER) */
#endif /* FEATURE_UIM_PMIC_ON_UIM1 */
#endif /* FEATURE_UIM1 */
#if defined( FEATURE_UIM2 )
  /* Set the current slot to UIM1 */
  uim_drv_slot = UIM_DRV_SLOT2;

  /* Mask all interrupts from UART. */
  UIM_PRG_IMR ( MSMU_IMR_MASK_ALL );

  /* Program one stale character timeout from the last byte received. */
  UIM_PRG_IPR ( (MSMU_IPR_NEW_RXSTALE | MSMU_IPR_TWO_CHARS) );

  /*-------------------------------------------------------------------------
   Reset the UART.  This flushes the Rx and Tx FIFOs on the UART.
  -------------------------------------------------------------------------*/
  uim_reset_uart (uim_drv_slot);

  /*-------------------------------------------------------------------------
   Set the transmit FIFO watermark level.  This is used when sending out block
   transfers.
  -------------------------------------------------------------------------*/
  UIM_TX_WATERLEVEL ( 30 );

  /*-------------------------------------------------------------------------
   Disable the RFR and CTS hardware control.
  -------------------------------------------------------------------------*/

  UIM_PRG_MR1 ( MSMU_MR1_DISABLE_RFR_CTS );

  /*-------------------------------------------------------------------------
   Program for eight bits per character, 2 stop bit and character check error
   mode
  -------------------------------------------------------------------------*/

  UIM_PRG_MR2 ( (MSMU_MR2_8BPC | MSMU_MR2_2SB | MSMU_MR2_EPAR) );

  /*-------------------------------------------------------------------------
   Set the UIM clock signal to the LOW state.
  -------------------------------------------------------------------------*/
  uim_clock_control ( UIM_CLOCK_LOW, uim_drv_slot );

  /*-------------------------------------------------------------------------
   Program the clock select for the Rx and Tx.  The baudrate is determined by
   the M/N clock counter.  This value is used for non-sample mode.
  -------------------------------------------------------------------------*/
  UIM_PRG_UART_CSR ( CSR_VALUE );

  /*-------------------------------------------------------------------------
   First, set the GPIO pins used for reset and power enable to a low state.
  -------------------------------------------------------------------------*/

#ifdef FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM
#error code not present
#else
  UIM_POWER_OFF();
#endif /* FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM */
  UIM_ASSERT_RESET();

  /*-------------------------------------------------------------------------
   Set GPIO pins for reset and power enable as outputs.
  -------------------------------------------------------------------------*/
  UIM_SET_POWER_AS_OUTPUT ();
  UIM_SET_RESET_AS_OUTPUT ();

  /*-----------------------------------------------------------------------
   Put the UART transmitter in break mode
  -----------------------------------------------------------------------*/
  UIM_START_BREAK ();

  /*-----------------------------------------------------------------------
   Enable the UART transmitter
  -----------------------------------------------------------------------*/
  UIM_ENABLE_UART_TX ();

#if defined( FEATURE_UIM_PMIC_ON_UIM2 )
  /*-----------------------------------------------------------------------
      ENABLE PM66XX UIM VOLTAGE TRANSLATION
  -----------------------------------------------------------------------*/

#ifdef FEATURE_UIM_USES_OLD_PMIC_API
  /*     MPP5: RUIM_RST          MPP6: UIM_RST        */
  /*     MSM side (input)   =>   UIM side (drive low) */
  /*     VREF = MSMP VREG        VREF = MSMP          */
  (void)pm_mpp_config_digital_input( PM_MPP_5,
                                      PM_MPP__DLOGIC__LVL_MSMP,
                                       PM_MPP__DLOGIC_IN__DBUS_NONE);

  /*     MPP9: RUIM_CLK          MPP10: UIM_CLK       */
  /*     MSM side (input)   =>   UIM side (drive low) */
  /*     VREF = MSMP VREG        VREF = MSMP          */
  (void)pm_mpp_config_digital_input( PM_MPP_9,
                                      PM_MPP__DLOGIC__LVL_MSMP,
                                       PM_MPP__DLOGIC_IN__DBUS_NONE);
#else
  /*     MPP5: RUIM_RST          MPP6: UIM_RST        */
  /*     MSM side (input)   =>   UIM side (drive low) */
  /*     VREF = MSMP VREG        VREF = MSMP          */
  (void)pm_mpp_config_digital_input( PM_MPP_5,
                                      PM_MPP__DLOGIC__LVL_MSMP);

  /*     MPP9: RUIM_CLK          MPP10: UIM_CLK       */
  /*     MSM side (input)   =>   UIM side (drive low) */
  /*     VREF = MSMP VREG        VREF = MSMP          */
  (void)pm_mpp_config_digital_input( PM_MPP_9,
                                      PM_MPP__DLOGIC__LVL_MSMP);
#endif /* FEATURE_UIM_USES_OLD_PMIC_API */
  /*     MPP11: RUIM_DATA        MPP12: UIM_DATA      */
  /*     MSM side (bidirec) <=>  UIM side (drive low) */
  /*     VREF = RUIM VREG        VREF = MSMP          */
  /*               Pull up resistor = 30K              */
  (void)pm_mpp_config_digital_inout( PM_MPP_11,
                                      PM_MPP__DLOGIC__LVL_MSMP,
                                       PM_MPP__DLOGIC_INOUT__PUP_30K);

#endif /* FEATURE_UIM_PMIC_ON_UIM2 */
#endif /* FEATURE_UIM2 */
  /*-----------------------------------------------------------------------
   Enable UART to drive the interface lines.
  -----------------------------------------------------------------------*/
  ENABLE_UART_INTERFACE ();

  /* Configure the UIM DATA and CLK GPIOs to PULL_DOWN */
  CONFIGURE_UIM_DATA_AND_CLK_AS_PULL_DOWN();

  /*-------------------------------------------------------------------------
   Install MSM UART interrupt handlers.
  -------------------------------------------------------------------------*/
#if defined (FEATURE_UIM1)
  UIM_TRAMP_SET_ISR();
#if defined(MSMHW_UART_HAS_INT_ENABLE_REG)
/* for GSM, WCDMA targets, only one interrupt source is defined
   for each UART in the interrupt controller.
   It is sub-multiplexed in UART*_INT_ENABLE register.
   Use the UART interrupt output
*/
#if defined (HWIO_UART_INT_ENABLE_ADDR)
    HWIO_OUTM( UART_INT_ENABLE, HWIO_FMSK(UART_INT_ENABLE, UART2_INT), HWIO_FMSK(UART_INT_ENABLE, UART2_INT) );
#else
    HWIO_OUT( UART2_INT_ENABLE, HWIO_FMSK(UART2_INT_ENABLE, UART_INT_ENA) );
#endif
#endif
#endif

#if defined( FEATURE_UIM2 )
  tramp_set_isr( TRAMP_UART3_ISR, uim_rx_isr_2);
#if defined(MSMHW_UART_HAS_INT_ENABLE_REG)
/* for GSM, WCDMA targets, only one interrupt source is defined
   for each UART in the interrupt controller.
   It is sub-multiplexed in UART*_INT_ENABLE register.
  Use the UART interrupt output
*/
#if defined (HWIO_UART_INT_ENABLE_ADDR)
    HWIO_OUTM( UART_INT_ENABLE, HWIO_FMSK(UART_INT_ENABLE, UART3_INT), HWIO_FMSK(UART_INT_ENABLE, UART3_INT) );
#else
    HWIO_OUT( UART3_INT_ENABLE, HWIO_FMSK(UART3_INT_ENABLE, UART_INT_ENA) );
#endif
#endif
#endif /* !FEATURE_UIM2 */

#if defined(T_UIM_USIM_PAD_EN)
  MSM_OUTSH( MISC_CTL_0,
             MISC_CTL_0_USIM_PAD_EN_BMSK,
             MISC_CTL_0_USIM_PAD_EN_BMSK
             );
#endif /* T_UIM_USIM_PAD_EN */

  /*-------------------------------------------------------------------------
   Allow the clock for the R-UIM interface to be turned off
  -------------------------------------------------------------------------*/
#if defined( FEATURE_UIM1 )
  uim_drv_slot = UIM_DRV_SLOT1;
  UIM_TCXO_CAN_BE_OFF();
#endif /* FEATURE_UIM1 */
#if defined( FEATURE_UIM2 )
  uim_drv_slot = UIM_DRV_SLOT2;
  UIM_TCXO_CAN_BE_OFF();
#endif /* FEATURE_UIM2 */

  uim_enable_vreg_lpm();

  /* If using the FET For VREG RUIM control, make sure that the voltage is set
    to 1.8V before TMC turns on VREG RUIM */
#if defined(FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM) && defined(FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES)
#error code not present
#endif /* FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM &&  FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES */
} /* end of uim_dev_init */


/*===========================================================================

FUNCTION UIM_POWER_UP                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function turns on the power for the UIM.  It puts the I/O line in
  reception mode (stops sending break on the I/O line) and turns on the
  UIM clock.

DEPENDENCIES
  This function must be executed either after uim_dex_init or after
  uim_power_down.

RETURN VALUE
  None

SIDE EFFECTS
  After this function is executed, the UIM is powered up and has a clock.

===========================================================================*/
void uim_power_up
(
#ifndef FEATURE_UIM_IGNORE_UNSOLICITED_BYTES
  uim_unsolict_resp_callback_type unsolict_resp_callback
#else
  void
#endif /* FEATURE_UIM_IGNORE_UNSOLICITED_BYTES */
)
{
  /* Clock control variable for powering up */
  uim_clock_control_type uim_clock_control_val;

#ifdef FEATURE_UIM_INTERFACE_NOT_USED
  /* Send error signal to GSDI */
  gsdi_uim_error_condition ( UIM_ERR_S );

  /* Set the card status to "error" */
  uim_status = UIM_ERR_S;

  return;
#endif /* FEATURE_UIM_INTERFACE_NOT_USED */

  /* Configure the UIM DATA and CLK GPIOs as NO PULL */
  CONFIGURE_UIM_DATA_AND_CLK_AS_NO_PULL();

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

  MSG_LOW( "uim_power_up", 0, 0, 0 );

#if (defined( FEATURE_UIM_PMIC_ON_UIM1 ) || \
     defined( FEATURE_UIM_PMIC_ON_UIM2 ) || \
     defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC ))
#if (!defined( T_QSC6270 ) && \
     !defined( FEATURE_UIM_QSC11X0_DRIVER ) && \
     !defined( FEATURE_UIM_MDM8200_DRIVER ))
  /* Disable the pulldown switch on I/O to get around
     PMIC6650 LPM mode problem */
  (void)pm_vreg_pull_down_switch( PM_OFF_CMD, PM_VREG_PDOWN_RUIM_ID );
#endif /* !T_QSC6270 && !FEATURE_UIM_QSC11X0_DRIVER && !FEATURE_UIM_MDM8200_DRIVER */
#endif

#ifndef FEATURE_UIM_IGNORE_UNSOLICITED_BYTES
  /* Make sure the callback function is non-NULL. */
  ASSERT (unsolict_resp_callback);

  /* Save the unsolicited response callback pointer. */
  unsolict_resp_callback_func[uim_drv_slot] = unsolict_resp_callback;
#endif /* FEATURE_UIM_IGNORE_UNSOLICITED_BYTES */

#if defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES )
  /* Program the currently selected voltage */
  switch (uim_current_voltage_class[uim_drv_slot])
  {
    case UIM_VOLTAGE_1_8V:
      {
        UIM_PROG_VOLTAGE_CLASS( UIM_1_8V_PROG );
#if !defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC )
        /* Set the SIM side MPP reference to VREG MSME */
        uim_pmic_sim_side_ref = PM_MPP__DLOGIC__LVL_MSME;
#endif /* FEATURE_UIM_USE_ONLY_VREG_ON_PMIC */
      } /* end case - UIM_VOLTAGE_1_8_V */
      break;

    case UIM_VOLTAGE_3V:
      {
        UIM_PROG_VOLTAGE_CLASS( UIM_3V_PROG );
#if !defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC )
        /* Set the SIM side MPP reference to VREG RUIM */
        uim_pmic_sim_side_ref = PM_MPP__DLOGIC__LVL_RUIM;
#endif /* FEATURE_UIM_USE_ONLY_VREG_ON_PMIC */
      } /* end case - UIM_VOLTAGE_1_8_V */
      break;

    default:
      {
        ERR_FATAL( "Bad UIM voltage class", 0, 0, 0 );
      }
      break;
  }
#elif ((defined( FEATURE_UIM1 ) && defined( FEATURE_UIM_PMIC_ON_UIM1 )) || \
       (defined( FEATURE_UIM2 ) && defined( FEATURE_UIM_PMIC_ON_UIM2 )) || \
       (defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC )))
  UIM_PROG_VOLTAGE_CLASS( UIM_3V_PROG );
#if !defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC )
  /* Set the SIM side MPP reference to VREG RUIM */
  uim_pmic_sim_side_ref = PM_MPP__DLOGIC__LVL_RUIM;
#endif /* FEATURE_UIM_USE_ONLY_VREG_ON_PMIC */
#endif /* FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES */
#if defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC )
  /* Turn on the Vreg RUIM */
#ifdef FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM
#error code not present
#else
  (void)pm_vreg_control( PM_ON_CMD, PM_VREG_RUIM_M );
#endif /* FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM */
#endif /* FEATURE_UIM_USE_ONLY_VREG_ON_PMIC */

  /* Initialize the UIM clock source frequency */
  uim_clk_source_freq[uim_drv_slot] = UIM_TCXO_FREQ;

  /* Setup the UIM clock based on available clock frequency. */
#if (UIM_MAX_FREQ_SUPPORTED_PREPROC == UIM_CLK_TD4_PREPROC)
  /* Initialize the clock frequency since it is used before the clock
     is turned on */
  uim_clk_freq[uim_drv_slot] = uim_clk_source_freq[uim_drv_slot]/4;
  uim_max_freq_supported[uim_drv_slot] = UIM_CLK_TD4;
  uim_clock_control_val = UIM_CLOCK_TD4;
#elif (UIM_MAX_FREQ_SUPPORTED_PREPROC == UIM_CLK_TD8_PREPROC)
  /* Initialize the clock frequency since it is used before the clock
     is turned on */
  uim_clk_freq[uim_drv_slot] = uim_clk_source_freq[uim_drv_slot]/8;
  uim_max_freq_supported[uim_drv_slot] = UIM_CLK_TD8;
  uim_clock_control_val = UIM_CLOCK_TD8;
#elif (UIM_MAX_FREQ_SUPPORTED_PREPROC == UIM_CLK_TD5_PREPROC)
  /* Initialize the clock frequency since it is used before the clock
     is turned on */
  uim_clk_freq[uim_drv_slot] = uim_clk_source_freq[uim_drv_slot]/5;
  uim_max_freq_supported[uim_drv_slot] = UIM_CLK_TD5;
  uim_clock_control_val = UIM_CLOCK_TD5;
#endif

/* Check the clock source for this target if it supports run time
     switching */

#if defined(T_MSM7600) || defined(T_QSC6270) || defined(FEATURE_UIM_QSC11X0_DRIVER)
  /* UART running at TCXO frequency for 7600 */
  uim_clk_source_freq[uim_drv_slot] = UIM_TCXO_FREQ;

  /* Frequency to the card is TCXO/5 for 7600*/
  uim_clk_freq[uim_drv_slot] = uim_clk_source_freq[uim_drv_slot]/5;
#ifdef FEATURE_UIM_USE_TCXO_DIV_5
  uim_max_freq_supported[uim_drv_slot] = UIM_CLK_TD5;
  uim_clock_control_val = UIM_CLOCK_TD5;
#endif /* FEATURE_UIM_USE_TCXO_DIV_5 */
#elif defined(FEATURE_UIM_MSM6800_TCXO_DIV5)
  /* TCXO DIV 5 for 6800 target */
  uim_clk_source_freq[uim_drv_slot] = 19200000;
  uim_clk_freq[uim_drv_slot] = uim_clk_source_freq[uim_drv_slot]/5;
  uim_max_freq_supported[uim_drv_slot] = UIM_CLK_TD4;
  uim_clock_control_val = UIM_CLOCK_TD4;
#elif defined( FEATURE_UIM_USE_13MHZ_GSM_CLOCK )
  /* Determine which revision of Cheetah we are running on */
  if ( HW_IS_UIM_CLK_13MHZ() )
  {
    /* Using the 13 MHz GSM clock as the UIM UART clock source */
    uim_clk_source_freq[uim_drv_slot] = 13000000;
    uim_clk_freq[uim_drv_slot] = uim_clk_source_freq[uim_drv_slot]/4;
    uim_max_freq_supported[uim_drv_slot] = UIM_CLK_TD4;
    uim_clock_control_val = UIM_CLOCK_TD4;
  } /* end if - The UIM clock source is 13MHz GSM clock */
#elif defined(T_MSM6280) && defined(FEATURE_UIM_3P84MHZ_M6280_PLATFORM)
  /* uart_clk_src and uart_sim_clk_src are being configured in clock regime
     as well under this feature macro */
  uim_clk_source_freq[uim_drv_slot] = 3840000;
  uim_clk_freq[uim_drv_slot] = uim_clk_source_freq[uim_drv_slot];
  /* TD4 means that td4_source clock is not divided to drive uim_clk.*/
  uim_max_freq_supported[uim_drv_slot] = UIM_CLK_TD4;
  uim_clock_control_val = UIM_CLOCK_TD4;
#elif defined(T_MSM6290)
  /* UART clock was recently set at 6.8MHz.  TD8 bit divides this clock by 2
     to give the UIM clock which is 3.4MHz */
  uim_clk_source_freq[uim_drv_slot] = 6800000;
  uim_clk_freq[uim_drv_slot] = 3400000;
  uim_max_freq_supported[uim_drv_slot] = UIM_CLK_TD8;
  uim_clock_control_val = UIM_CLOCK_TD8;
#elif defined(T_MSM6246)
  /* UART clock was recently set at 7.68MHz.  TD8 bit divides this clock by 2
     to give the UIM clock which is 3.84MHz */
  uim_clk_source_freq[uim_drv_slot] = 7680000;
  uim_clk_freq[uim_drv_slot] = 3840000;
  uim_max_freq_supported[uim_drv_slot] = UIM_CLK_TD8;
  uim_clock_control_val = UIM_CLOCK_TD8;
#elif defined( T_MSM6275)
  /* We are using GSM clk frequency.  We have the uim_clk_source_freq
   * Multiplied by 2 because of selecting TD8 bit*/
  uim_clk_source_freq[uim_drv_slot] = 26000000;
  uim_clk_freq[uim_drv_slot] = uim_clk_source_freq[uim_drv_slot]/8;
  uim_max_freq_supported[uim_drv_slot] = UIM_CLK_TD8;
  uim_clock_control_val = UIM_CLOCK_TD8;
#elif defined( T_MSM6500 )
#if defined (FEATURE_UIM_GSM_CLK_SRC)
#error code not present
#else /* NOT (FEATURE_UIM_GSM_CLK_SRC) */
  /* We are using TCXO clk frequency.*/
  uim_clk_source_freq[uim_drv_slot] = 19200000;
  uim_clk_freq[uim_drv_slot] = uim_clk_source_freq[uim_drv_slot]/8;
  uim_max_freq_supported[uim_drv_slot] = UIM_CLK_TD8;
  uim_clock_control_val = UIM_CLOCK_TD8;
#endif /*if defined (FEATURE_UIM_GSM_CLK_SRC)*/
#elif defined( FEATURE_UIM_GSM_CLK_SRC )
  /* Using the 13 MHz GSM clock as the UIM UART clock source */
  uim_clk_source_freq[uim_drv_slot] = 13107200;
  uim_clk_freq[uim_drv_slot] = uim_clk_source_freq[uim_drv_slot]/4;
  uim_max_freq_supported[uim_drv_slot] = UIM_CLK_TD4;
  uim_clock_control_val = UIM_CLOCK_TD4;
#endif /* FEATURE_UIM_USE_13MHZ_GSM_CLOCK */

#ifdef FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM
#error code not present
#endif /* FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM */
  /* Turn on the clock for the R-UIM interface */
  UIM_TCXO_MUST_BE_ON();

  /* First, turn on the UIM power. */

#ifndef FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM
  UIM_POWER_ON ();
#endif /* !FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM */
  /* Wait for 400 clock cycles before enabling the receiver. */
  uim_clk_busy_wait ((400*1000000)/uim_clk_freq[uim_drv_slot]);

  /* Setup the UIM clock based on available clock frequency. */
  uim_clock_control ( uim_clock_control_val, uim_drv_slot );

  /* Next, place the I/O line in reception mode. */
  UIM_STOP_BREAK ();

  /* Reset the receiver so that the receiver does not process the stop break
     as a received byte */
  UIM_RESET_RX ();

} /* end - uim_power_up */


#ifdef FEATURE_UIM_HANDLE_NO_ATR_IN_40000_CLK_CYCLES
/* boolean variable indicating if a byte is received since reset */
LOCAL boolean uim_atr_byte_received = FALSE;

/* Time it takes for 1 byte to get to the UART fifo from the start bit */
#define UIM_BYTE_TIME ( (int32)( 372 * 12 ))

/* Macro for ISO7816 defined maximum ATR wait UIM clock cycles */
#define UIM_ISO7816_MAX_ATRWAIT_UIMCYCLES (int32)(40000)

/* Macro for 1.25ms equivalent sleep count */
#define UIM_1PMS_EQ_SLEEP_COUNT  49152

/* Macro for 1.25 */
#define UIM_1P25   1.25

/* To convert the time stamp to number of 1.25ms,
   Shift out lower 16 bits */
#define UIM_RSHIFT_16BITS -16

/* To convert number of 1.25msto an equivalent time stamp,
   Shift in lower 16 bits */
#define UIM_LSHIFT_16BITS 16

/* Mask to extract lower 16 bits from timestamp */
#define UIM_LOWER16BITS_MASK         0x0FFFFUL

#define ATR_BYTE_RECEIVED() ( (UIM_READ_STATUS() & MSMU_SR_RXRDY) \
                              || uim_atr_byte_received )

/* Frequency of the sleep clock from which we get the time stmaps */
#define SLEEP_CLK_FREQ 32768

#endif /*FEATURE_UIM_HANDLE_NO_ATR_IN_40000_CLK_CYCLES*/

/*===========================================================================

FUNCTION UIM_RESET                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function asserts the reset line on the UIM.  Any data received by the
  phone will be flushed.  Any transfer from the phone will be terminated and
  the buffers flushed.

DEPENDENCIES
  This function must be executed after uim_dex_init.  The UIM should be
  powered up (via uim_power_up) for this function to have any effect.

RETURN VALUE
  None

SIDE EFFECTS
  The UIM driver is expecting an ATR after this reset.

===========================================================================*/
void uim_reset
(
  uim_rsp_buf_type  *rsp_ptr,    /* Defines where to put the ATR */
  uim_resp_callback_type atr_rx_callback /* Callback for ATR */
)
{
#ifdef FEATURE_UIM_HANDLE_NO_ATR_IN_40000_CLK_CYCLES
  /* Variable to store the current time stamp */
  ts_type running_time_stamp = {0, 0};
  /* Maximum time stamp that we can wait to conclude that ATR is not
     received */
  ts_type max_atr_time_stamp = {0, 0};
  /* This is the variable that stores the time stamp equivalent to
     40K + UIM_BYTE_TIME UIM clock cycles */
  double no_of_1p25_msecs_sleep_count = 0.0;
  /* Stores the maximum number of UIM clock cycles to wait for ATR */
  int32 max_count = 0;
  /* Integral part of the time stamp in sleep timer ticks */
  dword count = 0;
  /* Fractional part of the time stamp in sleep timer ticks */
  dword count_decimal = 0;
  /* Lower 16 bits of the time stamp which indicates the sleep timer ticks */
  dword lo_16bits = 0;
#endif /*FEATURE_UIM_HANDLE_NO_ATR_IN_40000_CLK_CYCLES*/

#ifdef FEATURE_UIM_UART_DM
  num_bytes_in_this_rx_transfer[uim_drv_slot] = 0;
#endif /* FEATURE_UIM_UART_DM */

  MSG_LOW( "uim_reset", 0, 0, 0 );

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

  if (rsp_ptr == NULL)
  {
    ERR_FATAL("uim_reset rsp_ptr is NULL",0,0,0);
  }

  #ifdef FEATURE_UIM_UART_DM

  /* Set the stale timeout: Do not enable the parity error workaround in hardware */
  UIM_PRG_IPR(MSMU_IPR_DEF_STALE_TIMEOUT);

  /* Turn OFF the ability to echo back sent bytes.
     Set SIM_CFG: MASK_RX to 1 */
  UIM_UART_CNFG(UART_SIM_CFG__MASK_RX, 8);


  /* Enable the receive interrupts - stale character timeout,
                                     receive break condition */
  UIM_PRG_IMR ( ( MSMU_IMR_RXSTALE | MSMU_IMR_RXBREAK) );

  #endif /* FEATURE_UIM_UART_DM */


  /* Reset/flush the UART just in case there was something going on. */
  uim_reset_uart (uim_drv_slot);

  #ifdef T_QSC6270
  /* Give some time for the UART reset. Some how this is needed for
     LCU UART.... NEEDED FOR ULC? */
  uim_clk_busy_wait (500);
  #endif /* T_QSC6270 */

  /* Assert the reset line */
  UIM_ASSERT_RESET ();

#if defined(T_UIM_UART1_ENABLE_SECONDARY_UART)
  /* Enable secondary UART register */
  MSM_OUT ( UART1_INT_ENABLE,
            UART1_INT_ENABLE_UART_INT_ENA);
#endif /* T_UIM_UART1_ENABLE_SECONDARY_UART */

  #ifndef FEATURE_UIM_UART_DM
  /* Enable the receive interrupts - receive FIFO level reached,
                                     stale character timeout,
                                     receive break condition */
  UIM_PRG_IMR ( (MSMU_IMR_RXLEV | MSMU_IMR_RXSTALE | MSMU_IMR_RXBREAK) );
  #endif /* !FEATURE_UIM_UART_DM */

  /* Enable the UART transmitter */
  UIM_ENABLE_UART_TX ();

  #ifdef FEATURE_UIM_UART_DM
  /* Enable the UART receiver */
  UIM_ENABLE_UART_RX ();
  #endif /* FEATURE_UIM_UART_DM */

  /* Save the callback function pointer and the ATR receive buffer pointer. */
  atr_rx_callback_func[uim_drv_slot] = atr_rx_callback;
  resp_buf[uim_drv_slot] = rsp_ptr;
  resp_buffer_ptr[uim_drv_slot] = (byte *)&rsp_ptr->rsp;

  /* Set up all the driver variables to expect an ATR. */
  uim_rx_state[uim_drv_slot] = UIM_RX_RECEIVE_ATR;
  num_resp_bytes_rcvd[uim_drv_slot] = 0;
  check_byte_expected[uim_drv_slot] = FALSE;
  check_byte[uim_drv_slot] = 0;
  num_historical_chars[uim_drv_slot] = 0;
  num_bytes_to_skip[uim_drv_slot] = 2;
  receive_mode[uim_drv_slot] = UIM_WAITING_FOR_T0;

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This status is returned in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Set the byte response delay to the default */
  uim_response_byte_delay = UIM_RESPONSE_BYTE_DELAY_DEFAULT;

  /* Configure the UIM parameters.  Reset the guardtime and use the
     the default baud rate. Guardtime = 2 bits * Bytes/bit.               */
  num_guardtime_bytes[uim_drv_slot] = (ERROR_SIGNAL_BITS * NUM_BYTES_PER_BIT);

  /* Set the UART clock to the default values. */
  set_uim_baud_rate ( UIM_CRCF_372_1, UIM_BRAF_1, uim_drv_slot );

  /* Wait for 200 clock cycles before enabling the receiver. */
  uim_clk_busy_wait ((200*1000000)/uim_clk_freq[uim_drv_slot]);

#ifdef UIM_DRIVER_TIMESTAMP
  if (uim_UseDriverTimeStamp)
  {
    /* Enable the receiver on the UART. */
    uim_enable_rx ( UIM_RX_WATER_MARK_LEVEL_ZERO, FALSE );
  }
  else
  {
  /* Enable the receiver on the UART. */
    uim_enable_rx ( UIM_RX_WATER_MARK_LEVEL_FIVE, FALSE );
  }
#else
#ifndef FEATURE_UIM_UART_DM
  /* Enable the receiver on the UART. */
  uim_enable_rx ( UIM_RX_WATER_MARK_LEVEL_FIVE, FALSE );
#endif /* !FEATURE_UIM_UART_DM */
#endif /* UIM_DRIVER_TIMESTAMP */

  /* Wait another 200 clock cycles (at 2.46Mhz) to deassert the reset. */
  uim_clk_busy_wait ((200*1000000)/uim_clk_freq[uim_drv_slot]);

#ifdef FEATURE_UIM_HANDLE_NO_ATR_IN_40000_CLK_CYCLES
  /* reset the flag right before releasing the reset line */
  uim_atr_byte_received = FALSE;
#endif /*FEATURE_UIM_HANDLE_NO_ATR_IN_40000_CLK_CYCLES*/

#ifdef FEATURE_UIM_UART_DM
  /* Initialize a new RX transfer */
  /* The ATR size cannot exceed 0x30 */
  UIM_DM_RX_INIT(0x30);

  /* Enable the stale character event */
  UIM_ENABLE_RX_STALE_INT();

  /* Some how this delay is needed for LCU UART.
     Will need to find out ways to remove this delay. NEEDED FOR ULC? */
  uim_clk_busy_wait (500);
#endif /* FEATURE_UIM_UART_DM */

  /* De-assert the reset line */
  UIM_DEASSERT_RESET ();

#ifdef FEATURE_UIM_HANDLE_NO_ATR_IN_40000_CLK_CYCLES
  /* Get the system time stamp as soon as we release the reset line */
  ts_get(running_time_stamp);

  /* Set the maximum UIM clock cycles to wait */
  #ifndef FEATURE_UIM_NO_ADDITIONAL_CONFIG_PARAMS
  max_count = UIM_ISO7816_MAX_ATRWAIT_UIMCYCLES + UIM_BYTE_TIME \
              + uim_config_params.additional_delay_for_atr;
  #else
  max_count = UIM_ISO7816_MAX_ATRWAIT_UIMCYCLES + UIM_BYTE_TIME;
  #endif

  /* The below calculations are based on the system time stamp definition
     as in services/time/time.c file. */

  /* The logic below calculates the system time stamp when the 40K +
     UIM_BYTE_TIME UIM clock cycles would actually expire with respect to
     time stamp when RESET is de-asserted. */

  /* Calculate that many sleep ticks to wait for ATR */
  no_of_1p25_msecs_sleep_count = (  (double)(max_count * 1000)  \
                                   /(double)(uim_clk_freq[uim_drv_slot] \
                                    * UIM_1P25) ) * UIM_1PMS_EQ_SLEEP_COUNT;

  /* Get the integral part (number of 1.25ms) to add to the current
     time stamp */
  count = (dword)no_of_1p25_msecs_sleep_count / UIM_1PMS_EQ_SLEEP_COUNT;

  /* Get the fractional part (less than 1.25ms) to add to the current
     time stamp */
  count_decimal = (dword)no_of_1p25_msecs_sleep_count \
                         % UIM_1PMS_EQ_SLEEP_COUNT;

  /* get the lower 16 bits (a value less than 1.25ms) from the current
     time stamp */
  lo_16bits = qw_lo(running_time_stamp) & UIM_LOWER16BITS_MASK;

  /* First step is to store the current time stamp to calculate the
     maximum time stamp to wait for ATR */
  qw_equ(max_atr_time_stamp, running_time_stamp);

  /* Shift out 16 bits of current time stamp to adjust the integral part */
  qw_shift(max_atr_time_stamp, UIM_RSHIFT_16BITS);

  /* Adjust the integral part to add that many 1.25ms equivalent to
     maximum time to wait for ATR */
  qw_inc(max_atr_time_stamp, (uint32)count);

  /*  Adjust the fractional part and the integral part to account
      for extra 1.25 ms */
  if( (lo_16bits + count_decimal) >= UIM_1PMS_EQ_SLEEP_COUNT)
  {
    qw_inc(max_atr_time_stamp, (uint32)1);
    qw_shift(max_atr_time_stamp, UIM_LSHIFT_16BITS);
    qw_inc(max_atr_time_stamp, (uint32)(lo_16bits + count_decimal \
                                        - UIM_1PMS_EQ_SLEEP_COUNT));
  }
  else
  {
    qw_shift(max_atr_time_stamp, UIM_LSHIFT_16BITS);
    qw_inc(max_atr_time_stamp, (uint32)(lo_16bits + count_decimal));
  }

  /* Until above, we calculated the time stamp (max_atr_time_stamp)
     to wait for ATR from the time when the SIM is RESET */

  /* Now, get inside the loop to check if we get the ATR before
     the maximum time to wait for ATR. */
  do
  {
    /* Get the current time stamp */
    ts_get(running_time_stamp);
  }
  while ((qw_cmp(running_time_stamp, max_atr_time_stamp) < 0)
         && !ATR_BYTE_RECEIVED());

  /* If we receive no byte power down the interface */
  if ( !ATR_BYTE_RECEIVED() )
  {
    /* Let us not clear the timeout signal */
    clear_timeout_on_response = FALSE;

    /* set the RX state to Idle */
    uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

    /* Reset/flush the UART */
    uim_reset_uart (uim_drv_slot);

    /* Mask all interrupts from UART. */
    UIM_PRG_IMR ( MSMU_IMR_MASK_ALL );

    ERR("No ATR in 40,000 clk cycles %d",4000,0,0);

    /* Signal a timeout to the task to handle a NO ATR situation */
    (void)rex_set_timer(&uim_cmd_rsp_timer, 0);
  }
#else /*not defined FEATURE_UIM_HANDLE_NO_ATR_IN_40000_CLK_CYCLES*/
#ifdef FEATURE_UIM_UART_DM
  /* Set the comand response timer since we are waiting for the ATR */
  /* Adding 5ms to the work waiting time as SW overhead */
  (void)rex_set_timer(&uim_cmd_rsp_timer, ( uim_work_waiting_time[uim_drv_slot] + 5 ) );
#else
  (void)rex_set_timer(&uim_cmd_rsp_timer, uim_work_waiting_time[uim_drv_slot]);
#endif /* FEATURE_UIM_UART_DM */
#endif /*FEATURE_UIM_HANDLE_NO_ATR_IN_40000_CLK_CYCLES*/
} /* end - uim_reset */


/*===========================================================================

FUNCTION UIM_SEND_PPS                             EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets up the transfer of a PPS request to the UIM.  This
  function starts the transfer, which, if all goes well, is finished by the
  uim_rx_isr.

DEPENDENCIES
  This function starts a PPS exchange.  This exchange can only be done
  immediately after an ATR is received.

RETURN VALUE
  None

SIDE EFFECTS
  The UIM driver will continue to send out the PPS request.  Then the
  driver will expect the PPS response.

===========================================================================*/
void uim_send_pps
(
  uim_pps_req_buf_type const *pps_request  /* This points to the PPS request and
                                         the PPS response buffer.            */
)
{
#ifdef FEATURE_UIM_UART_DM
  num_bytes_in_this_rx_transfer[uim_drv_slot] = 0;
#endif /* FEATURE_UIM_UART_DM */

#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */


#ifdef FEATURE_UIM_UART_DM
  /* Turn OFF the ability to echo-back receive bytes.
     Set SIM_CFG: MASK_RX to 1 */
  UIM_UART_CNFG(UART_SIM_CFG__MASK_RX, 8);


  /* Reset/flush the UART just in case there was something going on. */
  uim_reset_uart (uim_drv_slot);

  /* Enable the UART transmitter */
  UIM_ENABLE_UART_TX ();

  /* Enable the UART receiver */
  UIM_ENABLE_UART_RX ();

  /* Some how this delay is needed for LCU UART.
     Will need to find out ways to remove this delay. */
  uim_clk_busy_wait (500);
#endif /* FEATURE_UIM_UART_DM */

  MSG_LOW( "uim_send_pps", 0, 0, 0 );
  if (pps_request == NULL)
  {
    ERR_FATAL("uim_send_pps pps_request is NULL",0,0,0);
  }

  /* Save the number of bytes to send of the request. */
  num_bytes_to_send[uim_drv_slot] = pps_request->size_of_pps_request;

  /* Save the PPS request and response buffer pointers. */
  pps_resp_callback_func[uim_drv_slot] = pps_request->pps_resp_callback;
  request_buffer_ptr[uim_drv_slot] = pps_request->request_buf;
  resp_buf[uim_drv_slot] = pps_request->rsp_ptr;
  resp_buffer_ptr[uim_drv_slot] = (byte *)&pps_request->rsp_ptr->rsp;


  /* Set up the driver variables to expect a PPS response */
  /* Indicate the remaining bytes are to be sent */
  send_all_remaining_bytes[uim_drv_slot] = TRUE;
  uim_tx_state[uim_drv_slot] = UIM_TX_SEND_PPS;
  num_resp_bytes_rcvd[uim_drv_slot] = 0;
  check_byte[uim_drv_slot] = 0;
  num_bytes_to_skip[uim_drv_slot] = 2;
  receive_mode[uim_drv_slot] = UIM_WAITING_FOR_T0;

#if !defined(UIM_NO_SIM_GLUE_LOGIC) && !defined(T_UIM_NO_RX_WATERLEVEL)
  /* Program the receive level to allow fast processing of tx'd bytes */
  UIM_RX_WATERLEVEL ( UIM_RX_WATER_MARK_LEVEL_ZERO );
#endif /* !UIM_NO_SIM_GLUE_LOGIC && T_UIM_NO_RX_WATERLEVEL */

#ifndef FEATURE_UIM_UART_DM
#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_TX_DATA, *request_buffer_ptr[uim_drv_slot]);
#endif
#endif /* FEATURE_UIM_UART_DM */

#ifdef FEATURE_UIM_UART_DM
  /* This is when we use the word mode UART */
  uim_send_pps_bytes();
#else
  /* Send the first byte */
  uim_send_byte (*request_buffer_ptr[uim_drv_slot]);
#endif /* FEATURE_UIM_UART_DM */

} /* end - uim_send_pps */


/*===========================================================================

FUNCTION UIM_UPDATE_OP_PARAMS                         EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets up the operational parameters of the UIM interface.  It
  can change the baud rate of the UIM interface, change the UIM clock
  frequency or change the guardtime.

DEPENDENCIES
  This function changes the operational parameters of the UIM interface.
  Whatever changes are requested in this function shall apply as soon as
  possible.  The interface will reflect the changes.

RETURN VALUE
  None

SIDE EFFECTS
  All transfers over the UIM interface will occur at the frequency, baudrate,
  or guardtime as specified in this function call.

===========================================================================*/
void uim_update_op_params
(
  uim_op_params_type const *op_params,       /* Proposed operational parameters */
  uim_drv_slot_type uim_drv_slot
)
{
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

  MSG_LOW( "uim_update_op_params", 0, 0, 0);
  if (op_params == NULL)
  {
    ERR_FATAL("uim_update_op_params op_params is NULL",0,0,0);
  }

  /* Check if the UIM clock frequency is to change. */
  if (op_params->change_clk_freq)
  {
    /* Change the UIM clock frequency based on the selected rate */
    switch (op_params->clock_frequency)
    {
      case UIM_CLK_TD8:
        {
          /*-------------------------------------------------------------------------
           Turn on the clock for the R-UIM interface
          -------------------------------------------------------------------------*/
          UIM_TCXO_MUST_BE_ON();

          /* Set the clock to this rate. */
          uim_clock_control ( UIM_CLOCK_TD8, uim_drv_slot );
#ifdef FEATURE_UIM_ADD_DELAY_BEFORE_AND_AFTER_CLOCK_STOP
          if(uim_mode ==  UIM_NORMAL_MODE)
          {
            rex_sleep(UIM_DELAY_AFTER_CLOCK_RESTART);
          }
#endif /* FEATURE_UIM_ADD_DELAY_BEFORE_AND_AFTER_CLOCK_STOP */
        } /* end case - UIM_CLK_TD8 */
        break;

#if defined( FEATURE_UIM_USE_TCXO_DIV_5 )
      case UIM_CLK_TD5:
        {
          /*-------------------------------------------------------------------------
           Turn on the clock for the R-UIM interface
          -------------------------------------------------------------------------*/
          UIM_TCXO_MUST_BE_ON();

          /* Set the clock to this rate. */
          uim_clock_control (UIM_CLOCK_TD5, uim_drv_slot );
#ifdef FEATURE_UIM_ADD_DELAY_BEFORE_AND_AFTER_CLOCK_STOP
          if(uim_mode ==  UIM_NORMAL_MODE)
          {
            rex_sleep(UIM_DELAY_AFTER_CLOCK_RESTART);
          }
#endif /* FEATURE_UIM_ADD_DELAY_BEFORE_AND_AFTER_CLOCK_STOP */
          #ifdef FEATURE_UIM_UART_DM
          set_uim_baud_rate (uim_FI_current, uim_DI_current, uim_drv_slot);
          #endif /* FEATURE_UIM_UART_DM */
        } /* end case - UIM_CLK_TD5 */
        break;
#endif /* FEATURE_UIM_USE_TCXO_DIV_5 */

      case UIM_CLK_TD4:
        {
          /*-------------------------------------------------------------------------
           Turn on the clock for the R-UIM interface
          -------------------------------------------------------------------------*/
          UIM_TCXO_MUST_BE_ON();

          /* Set the clock to this rate. */
          uim_clock_control ( UIM_CLOCK_TD4, uim_drv_slot );
#ifdef FEATURE_UIM_ADD_DELAY_BEFORE_AND_AFTER_CLOCK_STOP
          if(uim_mode ==  UIM_NORMAL_MODE)
          {
            rex_sleep(UIM_DELAY_AFTER_CLOCK_RESTART);
          }
#endif /* FEATURE_UIM_ADD_DELAY_BEFORE_AND_AFTER_CLOCK_STOP */
        } /* end case - UIM_CLK_TD4 */
        break;

      case UIM_CLK_STOP_HIGH:
        {
#ifdef FEATURE_UIM_ADD_DELAY_BEFORE_AND_AFTER_CLOCK_STOP
          rex_sleep(UIM_DELAY_BEFORE_CLOCK_STOP);
          /* Do clock stop only if it is normal mode and there is no command
             in command queue */
          if(( q_cnt( & uim_cmd_q ) == 0 ) &&
              (uim_mode ==  UIM_NORMAL_MODE)
            )
#endif /* FEATURE_UIM_ADD_DELAY_BEFORE_AND_AFTER_CLOCK_STOP */
          {
            /* Set the clock to high state. */
            uim_clock_control ( UIM_CLOCK_HIGH, uim_drv_slot );

            /*-------------------------------------------------------------------------
             Allow the clock for the R-UIM interface to be turned off
            -------------------------------------------------------------------------*/
            UIM_TCXO_CAN_BE_OFF();
          }
        } /* end case - UIM_CLK_STOP_HIGH */
        break;

      case UIM_CLK_STOP_LOW:
        {
#ifdef FEATURE_UIM_ADD_DELAY_BEFORE_AND_AFTER_CLOCK_STOP
          rex_sleep(UIM_DELAY_BEFORE_CLOCK_STOP);
          /* Do clock stop only if there is no command in command queue 
             and it is normal mode */
          if(( q_cnt( & uim_cmd_q ) == 0 ) &&
              (uim_mode ==  UIM_NORMAL_MODE)
            )
#endif /* FEATURE_UIM_ADD_DELAY_BEFORE_AND_AFTER_CLOCK_STOP */
          {
            /* Set the clock to low state. */
            uim_clock_control ( UIM_CLOCK_LOW, uim_drv_slot );

            /*-------------------------------------------------------------------------
             Allow the clock for the R-UIM interface to be turned off
            -------------------------------------------------------------------------*/
            UIM_TCXO_CAN_BE_OFF();
          }
        } /* end case - UIM_CLK_STOP_LOW */
        break;

      default: /* do nothing. */
        {
        }
        break;
    } /* end switch - clock frequency */

  } /* end if - change the UIM clock frequency */

  /* Determine if the baud rate is to change */
  if (op_params->change_baud_rate)
  {
    /* Change the baudrate with the supplied parameters. */
    set_uim_baud_rate (op_params->FI, op_params->DI, uim_drv_slot);
  } /* end if - the baud rate is to change */

  /* Determine if the guardtime is to change. */
  if (op_params->change_guardtime)
  {
#ifdef UIM_NO_SIM_GLUE_LOGIC
    /* Change the guardtime control variable. */
    num_guardtime_bytes[uim_drv_slot] =
      ((op_params->guardtime_bits - 2) * NUM_BYTES_PER_BIT) +
      (ERROR_SIGNAL_BITS * NUM_BYTES_PER_BIT);
#else /* Sim glue logic */
#if defined( T_MSM6100 ) || defined( T_UIM_SABER_DEFS ) || defined (T_MSM6275)
    /* Set the stop bit number */
        UIM_UART_CNFG( UART_SIM_CFG__STOP_BIT_LEN_MASK,
                      (op_params->guardtime_bits + UIM_UART_DM_EXTRA_GUARD_TIME)
                       <<UART_SIM_CFG__STOP_BIT_LEN_SHFT);
#endif /* T_MSM6100 || T_UIM_SABER_DEFS */
#if defined( FEATURE_UIM_T_1_SUPPORT )
#if defined( FEATURE_UIM_BLOCK_MODE_SUPPORT )
  /* Check if the protocol is T=1 */
  if ( UIM_T_1_PROTOCOL == uim_operating_protocol )
  {
    /* Program the interface to operate in block mode */
    #if defined (FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG)
      UIM_UART_CNFG(UART_SIM_CFG_TX_RX_MODE_MASK,UART_SIM_CFG_TX_RX_MODE_MASK);
    #else
      UIM_BLOCK_MODE(  );
    #endif /* FEATURE_UIM_BLOCK_MODE_BITS_IN_SIM_CONFIG_REG */
    /* Determine if the stop bits are 1 */
    if (UIM_MINIMUM_T_1_GUARDTIME == op_params->guardtime_bits )
    {
      if ( UIM_DIRECT_CONVENTION == uim_convention[uim_drv_slot])
      {
#if defined(FEATURE_UIM_UART_DM)
        /* Use only one stop bit */
        UIM_PRG_MR2 ( MSMU_MR2_ERRMODE_BLOCK | MSMU_MR2_8BPC |
                      MSMU_MR2_1SB | MSMU_MR2_EPAR );
#else
        /* Use only one stop bit */
        UIM_PRG_MR2 ( MSMU_MR2_8BPC | MSMU_MR2_1SB | MSMU_MR2_EPAR );
#endif /* FEATURE_UIM_UART_DM */
      }
      else /* inverse convention */
      {
#if defined(FEATURE_UIM_UART_DM)
        /* Use only one stop bit */
        UIM_PRG_MR2 ( MSMU_MR2_ERRMODE_BLOCK | MSMU_MR2_8BPC |
                      MSMU_MR2_1SB | MSMU_MR2_OPAR );
#else
        /* Use only one stop bit */
        UIM_PRG_MR2 ( MSMU_MR2_8BPC | MSMU_MR2_1SB | MSMU_MR2_OPAR );
#endif /* FEATURE_UIM_UART_DM */
      } /* end if - direct convention */
    } /* end if - guardtime bits are 1 */
#if defined(FEATURE_UIM_UART_DM)
    else /* we only use 2 stop bits for T=1 at maximum */
    {
      if ( UIM_DIRECT_CONVENTION == uim_convention[uim_drv_slot])
      {
        /* Use two stop bits */
        UIM_PRG_MR2 ( MSMU_MR2_ERRMODE_BLOCK | MSMU_MR2_8BPC |
                      MSMU_MR2_2SB | MSMU_MR2_EPAR );
      }
      else /* inverse convention */
      {
        /* Use two stop bits */
        UIM_PRG_MR2 ( MSMU_MR2_ERRMODE_BLOCK | MSMU_MR2_8BPC |
                      MSMU_MR2_2SB | MSMU_MR2_OPAR );
      } /* end if - direct convention */
    }
#endif /* FEATURE_UIM_UART_DM */
  }
#endif /* FEATURE_UIM_BLOCK_MODE_SUPPORT */
#endif /* FEATURE_UIM_T_1_SUPPORT */
#endif /* UIM_NO_SIM_GLUE_LOGIC */
  } /* end if - change the guardtime. */
} /* end - uim_update_op_params */


#ifdef FEATURE_UIM_UART_DM
/*===========================================================================

FUNCTION UIM_WRITE_TO_UART                         INTERNAL FUNCTION

DESCRIPTION
  This function writes the TX DATA to the UART TX FIFO.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void uim_write_to_uart(unsigned char *buffer, int size)
{

  int i =0;
  unsigned char mul =0, rem=0;
  unsigned int y = 0;

  if(size <= 0)
  {
    MSG_ERROR ("Invalid size of data while writing to UART",0,0,0);
    return;
  }

  mul = size/4;
  rem = size%4;

  /* Send that many words */
  if(mul > 0)
  {
    for (i=0; i<mul; i++)
    {
      y = (buffer[i*4+3] << 24) | (buffer[i*4+2] << 16) | (buffer[i*4+1] << 8) | (buffer[i*4]);
      UIM_SEND_TX_FIFO(y);
    }
  }

  /* Send the remaining bytes packed as a word */
  if(rem > 0)
  {
    y = 0;
    for(i=0; i<rem; i++)
    {
      y += (buffer[4*mul+i] << 8*i);
    }
    UIM_SEND_TX_FIFO(y);
  }

#ifdef FEATURE_UIM_DEBUG_LOG
  for(i=0; i<size; i++)
  {
    UIM_LOG_PUT_BYTE(UIM_LOG_TX_DATA, buffer[i]);
  }
  UIM_LOG_PUT_LAST_BYTE();
#endif /* FEATURE_UIM_DEBUG_LOG */

  return;
}

/*===========================================================================

FUNCTION UIM_SEND_APDU_COMMAND_HDR                         INTERNAL FUNCTION

DESCRIPTION
  This function sends the APDU command in the case of UART DM.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/

void uim_send_apdu_command_hdr()
{

  /* Initialize these globals to FALSE for every APDU send */
  sw1_byte_rcvd  = FALSE;
  sw2_byte_rcvd  = FALSE;
  resp_bytes_being_rcvd = FALSE;

  /* Initialize the RXLEV count for every APDU transaction */
  uim_rxlev_count = 0;

  /* Reset the RX byte count to zero */
  num_bytes_received = 0;

  /* set the rx state to wait for procedure bytes */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROC_PRCDR_BYTES;

  /* Clear if there was a pending stale interrupt */
  if (UIM_READ_MISR() & MSMU_IMR_RXSTALE)
  {
    ERR ("This condition should not happen",0,0,0);
    /* clear the stale char event */
    UIM_CLEAR_RX_STALE_INT();
  }

  /* RAM_ADDR_WIDTH is 7 bits in our version of LCU MSM chip
     - This RAM is used as both RX FIFO and TX FIFO
     - 128 words is the total size of RAM. Which means, the size of RX and
       TX FIFO is 128 words.
     - Default configuration:  (we use this)
       RX FIFO size = TX FIFO size, that means 64 words (256 bytes) each.
   */

  /* If the response size (ack byte + data + status bytes) is greater than
     RX FIFO size, then enable RXLEV interrupt */
  if ((num_resp_data_bytes_expected[uim_drv_slot] > 0) &&
      (num_resp_data_bytes_expected[uim_drv_slot] +
       NUM_ACK_BYTES + NUM_STATUS_BYTES > MAX_RXFIFO_SIZE) )
  {
    UIM_RX_WATERLEVEL( MSMU_DEF_RFWR_VAL );
    UIM_PRG_IMR ( (MSMU_IMR_TX_READY | MSMU_IMR_RXLEV |    \
                   MSMU_IMR_RXSTALE | MSMU_IMR_RXBREAK) );
  }
  else
  {
    /*
     If the response APDU size (ack byte + data + status bytes) is less than
     256 bytes, then there is no chance of an overflow error. Hence, SCT
     interrupt is only used and RXLEV interrupt is not used.
     */
    UIM_PRG_IMR ( (MSMU_IMR_TX_READY | MSMU_IMR_RXSTALE | MSMU_IMR_RXBREAK) );
  }

  /* Initialize a new RX transfer
    UART_DM SWI says: number of bytes needs to be multipled by 16 */
  UIM_DM_RX_INIT(MSMU_DEF_RX_INIT_VAL);

  /* Program the default stale timeout value */
  UIM_PRG_IPR(MSMU_IPR_DEF_STALE_TIMEOUT | MSMU_IPR_PAR_ERR_FIX_ENABLE);

  /* Wait for TXREADY event to make sure the APDU command bytes have been seen successfully */
  uim_rx_sub_state = UIM_RX_WAIT_FOR_TXREADY;

  /* Enable Stale event */
  UIM_ENABLE_RX_STALE_INT();

  /* Program the number of bytes to be sent as APDU command header */
  UIM_NUM_CHARS_TO_SEND(num_bytes_to_send[uim_drv_slot]);

  /* Clear the work waiting timer when transmitting bytes */
  (void) rex_clr_timer( &uim_cmd_rsp_timer);

  /* Write the words to the UART */
  uim_write_to_uart(request_buffer_ptr[uim_drv_slot], num_bytes_to_send[uim_drv_slot]);
}

#endif /* FEATURE_UIM_UART_DM */

/*===========================================================================

FUNCTION UIM_SEND_APDU                          INTERNAL FUNCTION

DESCRIPTION
  This function starts the transfer of an APDU to the UIM.  This function
  starts the transfer, which, if all goes well, is finished by the
  uim_rx_isr.

DEPENDENCIES
  This function starts an APDU exchange.  The exchange can only happen after
  the ATR has been received and, optionally, after a PPS exchange.

RETURN VALUE
  None

SIDE EFFECTS
  The UIM driver will continue to send out the APDU.  All the stages of
  sending the APDU are handled by the UIM driver.  Then the driver expects
  the APDU response.  Some commands solicit a GET_RESPONSE or a FETCH command
  to be sent after the APDU.

===========================================================================*/
LOCAL void uim_send_apdu
(
  uim_req_buf_type *uim_req /* This points to the APDU command buffer, the
                                 command data buffer, the response buffer, the
                                 APDU status and the callback function.      */
)
{
#ifdef FEATURE_UIM_UART_DM
  /* Initialize the byte count */
  uim_gcf_byte_count = 0;

  num_bytes_in_this_rx_transfer[uim_drv_slot] = 0;
#endif /* FEATURE_UIM_UART_DM */

  if (uim_req == NULL)
  {
    ERR_FATAL("uim_send_apdu uim_req is NULL",0,0,0);
  }
  /* Initially, send the APDU header. */

  if(uim_mode == UIM_PASSIVE_MODE)
  {
    uim_passive_clk_stop_allowed = TRUE;
  }

  /* Save the number of bytes to send of the APDU header. */
  num_bytes_to_send[uim_drv_slot] = NUM_BYTES_APDU_HDR;

  /* Append the channel inforation here */
  uim_req->apdu_hdr.uim_class |= (int) uim_cur_channel;

  /* Save the request buffer pointer. */
  request_buffer_ptr[uim_drv_slot] = (byte *)&uim_req->apdu_hdr;

  /* Copy the contents of APDU header into last sent APDU header */
  last_sent_apdu[uim_drv_slot] = uim_req->apdu_hdr;

  /* Copy the instruction case */
  uim_last_instrn_case[uim_drv_slot] = uim_req->instrn_case;

  /* Determine which direction the data flows */
  switch (uim_req->instrn_case)
  {
    case UIM_INSTRN_CASE_2:   /* Case 2 - Response data only */
      {
        /* The data flows from the UIM to the ME or not at all. */
        num_cmd_data_bytes_to_send[uim_drv_slot] = 0;
        /* Determine if 256 bytes are expected */
        if (UIM_MAX_TRANSACTION_P3 == uim_req->apdu_hdr.p3)
        {
          /* Wait for 256 bytes */
          num_resp_data_bytes_expected[uim_drv_slot] = UIM_MAX_TRANSACTION_SIZE;
        }
        else
        {
          num_resp_data_bytes_expected[uim_drv_slot] = uim_req->apdu_hdr.p3;
        } /* end if - check if max number of bytes are expected */
      } /* UIM_INSTRN_CASE_2 */
      break;

    case UIM_INSTRN_CASE_1:   /* Case 1 - No data exchanged */
    case UIM_INSTRN_CASE_3:   /* Case 3 - Command data only */
    case UIM_INSTRN_CASE_4:   /* Case 4 - Response and command data */
      {
        num_cmd_data_bytes_to_send[uim_drv_slot] = uim_req->apdu_hdr.p3;
        num_resp_data_bytes_expected[uim_drv_slot] = 0;
      } /* UIM_INSTRN_CASE_1 or UIM_INSTRN_CASE_3 or UIM_INSTRN_CASE_4 */
      break;
    default:
      {
        /* The instruction case is not valid.  Cannot process command.
           Cause a failure through the time out mechanism. */
        MSG_HIGH("Invalid instrn case %d",uim_req->instrn_case,0,0);
        (void) rex_set_timer(&uim_cmd_rsp_timer, 0);
        return;
      } /* default */
  } /* end switch - instruction case */

  /* Set up the driver variables to send the APDU header */
  /* Indicate the remaining bytes are to be sent */
  send_all_remaining_bytes[uim_drv_slot] = TRUE;
  uim_tx_state[uim_drv_slot] = UIM_TX_SEND_CMD_HDR;
  num_resp_bytes_rcvd[uim_drv_slot] = 0;

#if !defined(UIM_NO_SIM_GLUE_LOGIC) && !defined(T_UIM_NO_RX_WATERLEVEL)
  /* Program the receive level to allow fast processing of tx'd bytes */
  UIM_RX_WATERLEVEL ( UIM_RX_WATER_MARK_LEVEL_ZERO );
#endif /* !UIM_NO_SIM_GLUE_LOGIC && T_UIM_NO_RX_WATERLEVEL */

  /* Set the transaction timer (some additional slack is implicitly assumed
   * to be included in UIM_TRANSACTION_TIME_DURATION)
   */
  (void) rex_set_timer( &uim_transaction_rsp_timer, UIM_TRANSACTION_TIME_DURATION +
                        UIM_MAX_TRANSACTION_SIZE * uim_work_waiting_time[uim_drv_slot] );

#ifdef FEATURE_UIM_DEBUG_LOG
  /* Initialize the logging length to zero when
     beginning a new APDU transaction as a way of defensive
     programming */
  uim_log_data.length = 0;
#ifndef FEATURE_UIM_UART_DM
  UIM_LOG_PUT_BYTE(UIM_LOG_TX_DATA, *request_buffer_ptr[uim_drv_slot]);
#endif /* FEATURE_UIM_UART_DM */
#endif /* FEATURE_UIM_DEBUG_LOG */

#ifdef FEATURE_UIM_UART_DM
  /* Send the APDU command header in the case of word mode UART */
  uim_send_apdu_command_hdr();
#else
  /* Send the first byte */
  uim_send_byte (*request_buffer_ptr[uim_drv_slot]);
#endif /* FEATURE_UIM_UART_DM */

} /* end - uim_send_apdu */


/*===========================================================================

FUNCTION UIM_SEND_COMMAND                          EXTERNALIZED FUNCTION

DESCRIPTION
  This function sets up the transfer of an APDU to the UIM.  This function
  starts the transfer, which, if all goes well, is finished by the
  uim_rx_isr.

DEPENDENCIES
  This function starts an APDU exchange.  The exchange can only happen after
  the ATR has been received and, optionally, after a PPS exchange.

RETURN VALUE
  None

SIDE EFFECTS
  The UIM driver will continue to send out the APDU.  All the stages of
  sending the APDU are handled by the UIM driver.  Then the driver expects
  the APDU response.  Some commands solicit a GET_RESPONSE or a FETCH command
  to be sent after the APDU.

===========================================================================*/
void uim_send_command
(
  uim_req_buf_type *uim_req /* This points to the APDU command buffer, the
                                 command data buffer, the response buffer, the
                                 APDU status and the callback function.      */
)
{
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

  MSG_LOW( "uim_send_command", 0, 0, 0 );

  /* Reset the error counts */
  uim_parity_error_count = 0;
  uim_rx_break_error_count = 0;
  uim_overrun_error_count = 0;

  if (uim_req == NULL)
  {
    ERR_FATAL("uim_send_command uim_req is NULL",0,0,0);
  }

#if defined( FEATURE_UIM_T_1_SUPPORT )
  if (UIM_T_1_PROTOCOL == uim_current_protocol)
  {
#ifdef FEATURE_UIM_UART_DM
    /* Initialize the RX sub-state for T-1 protocol */
    uim_rx_sub_state = UIM_RX_T_1_RX_PROLOGUE_ST;
#endif /* FEATURE_UIM_UART_DM */

    /* Process the card response for the T=1 protocol */
    uim_process_t_1_command( uim_req );
  }
  else
#endif /* FEATURE_UIM_T_1_SUPPORT */
  {
    /* Send an indication the UIm driver started processing a command. */
    UIM_TEST_INT_PROCESSING (UIM_DEBUG_SEND_CMD | uim_req->apdu_hdr.instrn)

    /* Save the protocol of the command to use the correct instruction class
       in a GET RESPONSE or FETCH command */
    uimdrv_protocol[uim_drv_slot] = uim_req->protocol;

    /* Save the callback function pointer. */
    apdu_resp_callback_func[uim_drv_slot] = uim_req->rpt_function;

    /* Initialize the command data and the response pointers */
    apdu_command_data_ptr[uim_drv_slot] = (byte *)&uim_req->req;
    resp_buf[uim_drv_slot] = uim_req->rsp_ptr;
    resp_buffer_ptr[uim_drv_slot] = (byte *)&uim_req->rsp_ptr->rsp;
    num_resp_bytes_rcvd_total[uim_drv_slot] = 0;

    /* Reset the warning status indicators */
    uim_warning1_indicator = FALSE;
    uim_warning2_indicator = FALSE;

    /* Zero the command response size */
    resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

    /* Wait long enough before sending the header */
    uim_clk_busy_wait( uim_response_byte_delay );

#ifdef FEATURE_UIM_UART_DM
    /* Initialize the RX sub-state for T-0 protocol */
    uim_rx_sub_state = UIM_RX_PROCESS_IDLE;
#endif /* FEATURE_UIM_UART_DM */

    /*
     * If this is the new command, reset the variables to their default values.
     * Else we are retrying the select command, increase the select count and
     * set the warning flag to FALSE.
     */
    if( uim_sw1_warnings1_normal_end_occured &&
        uim_mf_reselect )
    {
       uim_mf_reselect_count++;
       uim_sw1_warnings1_normal_end_occured = FALSE;
    }
    else
    {
      uim_sw1_warnings1_normal_end_occured = FALSE;
      uim_mf_reselect = FALSE;
      uim_mf_reselect_count = 1;
    }

    /* Send the APDU */
    uim_send_apdu (uim_req);
  } /* end if - the protocol is T=1 */
} /* end - uim_send_command */
#if defined( FEATURE_UIM_T_1_SUPPORT )


/*===========================================================================

FUNCTION UIM_T_1_SEND_I_BLOCK

DESCRIPTION
  This function builds a T=1 information block and sends it to the UIM card.

DEPENDENCIES
  This function uses global variables to control how the information block is
  compiled and sent.

RETURN VALUE
  None

SIDE EFFECTS
  The information block will be sent and a response block will be expected or
  a timeout will occur.

===========================================================================*/
void uim_t_1_send_i_block
(
  byte node_address
)
{
  /* wait here for the BGT time */
  rex_sleep(uim_t_1_bgt);

  /* Clear the UART RX FIFO by reading byte/word one by one and
     by doing a UART RX reset to clear the error flags */
  uim_uimdrv_flush_uart();

  /* Build the Prologue field */
  uim_tx_prologue_field.nad = node_address;
  uim_tx_prologue_field.pcb = UIM_T_1_PCB_I_BLOCK;
  /* Set the send sequence number */
  if (uim_me_send_sequence_num != UIM_T_1_INIT_SEQ_NUM)
  {
    uim_tx_prologue_field.pcb |= UIM_T_1_PCB_SEND_SEQ_NUM;
  }
  /* Determine the information field length and more bit */
  if (uim_apdu_request_bytes_to_send > uim_t_1_ifsc)
  {
    /* Send the maximum I-block size */
    num_cmd_data_bytes_to_send[uim_drv_slot] = uint16touint8(uim_t_1_ifsc);
    uim_tx_prologue_field.len = uint16touint8(uim_t_1_ifsc);
    uim_tx_prologue_field.pcb |= UIM_T_1_PCB_MORE_DATA_BIT;

    /* Save the length of the I block being sent.  There might
    be some more blocks (R & S) between sending the blocks of the
    chained I blocks */
    uim_last_tx_i_block_length = uim_tx_prologue_field.len;

    /* Change the T=1 protocol state */
    uim_t_1_protocol_state = UIM_T_1_SENT_CHAINED_APDU_ST;
  }
  else
  {
    /* The APDU fits within the I-block */
    num_cmd_data_bytes_to_send[uim_drv_slot] = uint16touint8(uim_apdu_request_bytes_to_send);
    uim_tx_prologue_field.len = (byte)uim_apdu_request_bytes_to_send;

    /* Change the T=1 protocol state */
    uim_t_1_protocol_state = UIM_T_1_SENT_APDU_ST;
  } /* end if - the APDU is larger than the block size */

  /* Initialize the request pointer to send out the prologue field */
  request_buffer_ptr[uim_drv_slot] = (byte *)&uim_tx_prologue_field;
  /* Number of bytes to send of the prologue. */
  num_bytes_to_send[uim_drv_slot] = UIM_T_1_NUM_BYTES_PROLOGUE;

  /* Save the APDU and response buffer pointers. */
  apdu_resp_callback_func[uim_drv_slot] = uim_t_1_apdu_callback_func;
  apdu_command_data_ptr[uim_drv_slot] = uim_apdu_req_buffer_ptr;
  resp_buf[uim_drv_slot] = uim_t_1_apdu_resp_buf;
  resp_buffer_ptr[uim_drv_slot] = uim_apdu_rsp_buffer_ptr;
  uim_rx_prologue_field_buffer_ptr = (byte *)&uim_rx_prologue_field_buffer;

  /* Zero the command response size */
  resp_buf[uim_drv_slot]->cmd_rsp_size = 0;
  /* Initialize the EDC fields */
  if (UIM_T_1_EDC_LRC == uim_t_1_edc)
  {
    /* Initialize the crc field as it initializes both bytes of the EDC
       field including the lrc field */
    uim_tx_epilogue_field.crc = 0;
  }
  else /* The EDC is CRC */
  {
    uim_tx_epilogue_field.crc = CRC_16_L_SEED;
  } /* The EDC is LRC */

  /* Start sending the prologue */
  uim_rx_state[uim_drv_slot] = UIM_RX_T_1_SEND_PROLOGUE_ST;

#ifdef FEATURE_UIM_UART_DM
  /* Enable Tx level interrupt to cause T=1 block to be sent out.
     TX READY interrupt tells us when the FIFO is empty (last word moved
     to the packing buffer) and NO_CHARS_FOR_TX characters were written
     into the FIFO. As per the UART_DM design, it is forbidden to write
     the next TX transfer size into NO_CHARS_FOR_TX register before TX_READY
     interrupt is triggered. */
  UIM_PRG_IMR ( (MSMU_IMR_TX_READY | MSMU_IMR_RXLEV | MSMU_IMR_RXSTALE |
                 MSMU_IMR_RXBREAK | MSMU_IMR_TXLEV) );
#else
  /* Enable Tx level interrupt to cause T=1 block to be sent out */
  UIM_PRG_IMR ( (MSMU_IMR_RXLEV   | MSMU_IMR_RXSTALE |
                 MSMU_IMR_RXBREAK | MSMU_IMR_TXLEV) );
#endif /* FEATURE_UIM_UART_DM */
} /* uim_t_1_send_i_block */


/*===========================================================================

FUNCTION UIM_T_1_SEND_R_BLOCK

DESCRIPTION
  This function builds a T=1 receive ready block and sends it to the UIM card.

DEPENDENCIES
  This function uses global variables to control how the receive ready block is
  compiled and sent.

RETURN VALUE
  None

SIDE EFFECTS
  The receive ready block will be sent and a response block will be expected or
  a timeout will occur.

===========================================================================*/
void uim_t_1_send_r_block
(
  byte node_address,        /* Node address byte */
  byte r_block_code         /* R-block response code */
)
{
  /* wait here for the BGT time */
  rex_sleep(uim_t_1_bgt);

  /* Clear the UART RX FIFO by reading byte/word one by one and
     by doing a UART RX reset to clear the error flags */
  uim_uimdrv_flush_uart();

  /* Build the Prologue field */
  uim_tx_prologue_field.nad = node_address;
  uim_tx_prologue_field.pcb = UIM_T_1_PCB_R_BLOCK | r_block_code;
  /* Set the send sequence number */
  if (uim_card_send_sequence_num != 0)
  {
    uim_tx_prologue_field.pcb |= UIM_T_1_PCB_NEXT_SEQ_NUM;
  }

  /* There is no information field in an R-block */
  num_cmd_data_bytes_to_send[uim_drv_slot] = 0;
  uim_tx_prologue_field.len = 0;

  /* Initialize the request pointer to send out the prologue field */
  request_buffer_ptr[uim_drv_slot] = (byte *)&uim_tx_prologue_field;
  /* Number of bytes to send of the prologue. */
  num_bytes_to_send[uim_drv_slot] = UIM_T_1_NUM_BYTES_PROLOGUE;

  /* Save the APDU and response buffer pointers. */
  apdu_resp_callback_func[uim_drv_slot] = uim_t_1_apdu_callback_func;
  apdu_command_data_ptr[uim_drv_slot] = uim_apdu_req_buffer_ptr;
  resp_buf[uim_drv_slot] = uim_t_1_apdu_resp_buf;
  resp_buffer_ptr[uim_drv_slot] = uim_apdu_rsp_buffer_ptr;
  uim_rx_prologue_field_buffer_ptr = (byte *)&uim_rx_prologue_field_buffer;

  /* Zero the command response size */
  resp_buf[uim_drv_slot]->cmd_rsp_size = 0;
  /* Initialize the EDC fields */
  if (UIM_T_1_EDC_LRC == uim_t_1_edc)
  {
    /* Initialize the crc field as it initializes both bytes of the EDC
       field including the lrc field */
    uim_tx_epilogue_field.crc = 0;
  }
  else /* The EDC is CRC */
  {
    uim_tx_epilogue_field.crc = CRC_16_L_SEED;
  } /* The EDC is LRC */

  /* Start sending the prologue */
  uim_rx_state[uim_drv_slot] = UIM_RX_T_1_SEND_PROLOGUE_ST;

#ifdef FEATURE_UIM_UART_DM
  /* Enable Tx level interrupt to cause T=1 block to be sent out.
     TX READY interrupt tells us when the FIFO is empty (last word moved
     to the packing buffer) and NO_CHARS_FOR_TX characters were written
     into the FIFO. As per the UART_DM design, it is forbidden to write
     the next TX transfer size into NO_CHARS_FOR_TX register before TX_READY
     interrupt is triggered. */
  UIM_PRG_IMR ( (MSMU_IMR_TX_READY | MSMU_IMR_RXLEV | MSMU_IMR_RXSTALE |
                 MSMU_IMR_RXBREAK | MSMU_IMR_TXLEV) );
#else
  /* Enable Tx level interrupt to cause T=1 block to be sent out */
  UIM_PRG_IMR ( (MSMU_IMR_RXLEV   | MSMU_IMR_RXSTALE |
                 MSMU_IMR_RXBREAK | MSMU_IMR_TXLEV) );
#endif /* FEATURE_UIM_UART_DM */
} /* uim_t_1_send_r_block */


/*===========================================================================

FUNCTION UIM_T_1_SEND_S_BLOCK

DESCRIPTION
  This function builds a T=1 supervisory block and sends it to the UIM card.

DEPENDENCIES
  This function uses global variables to control how the supervisory block is
  compiled and sent.

RETURN VALUE
  None

SIDE EFFECTS
  The supervisory block will be sent and a response block will be expected or
  a timeout will occur.

===========================================================================*/
void uim_t_1_send_s_block
(
  byte node_address,        /* Node address byte */
  byte s_block_code,        /* S-block code */
  byte *s_block_data,       /* S-block data */
  byte s_block_data_size    /* S-block data size */
)
{
  /* wait here for the BGT time */
  rex_sleep(uim_t_1_bgt);

  /* Clear the UART RX FIFO by reading byte/word one by one and
     by doing a UART RX reset to clear the error flags */
  uim_uimdrv_flush_uart();

  /* Build the Prologue field */
  uim_tx_prologue_field.nad = node_address;
  uim_tx_prologue_field.pcb = UIM_T_1_PCB_S_BLOCK | s_block_code;

  /* There is no information field in an R-block */
  num_cmd_data_bytes_to_send[uim_drv_slot] = s_block_data_size;
  uim_tx_prologue_field.len  = s_block_data_size;

  /* Initialize the request pointer to send out the prologue field */
  request_buffer_ptr[uim_drv_slot] = (byte *)&uim_tx_prologue_field;
  /* Number of bytes to send of the prologue. */
  num_bytes_to_send[uim_drv_slot] = UIM_T_1_NUM_BYTES_PROLOGUE;

  /* Save the APDU and response buffer pointers. */
  apdu_resp_callback_func[uim_drv_slot] = uim_t_1_apdu_callback_func;
  apdu_command_data_ptr[uim_drv_slot] = s_block_data;
  resp_buf[uim_drv_slot] = uim_t_1_apdu_resp_buf;
  resp_buffer_ptr[uim_drv_slot] = uim_apdu_rsp_buffer_ptr;
  uim_rx_prologue_field_buffer_ptr = (byte *)&uim_rx_prologue_field_buffer;

  /* Zero the command response size */
  resp_buf[uim_drv_slot]->cmd_rsp_size = 0;
  /* Initialize the EDC fields */
  if (UIM_T_1_EDC_LRC == uim_t_1_edc)
  {
    /* Initialize the crc field as it initializes both bytes of the EDC
       field including the lrc field */
    uim_tx_epilogue_field.crc = 0;
  }
  else /* The EDC is CRC */
  {
    uim_tx_epilogue_field.crc = CRC_16_L_SEED;
  } /* The EDC is LRC */

  /* Start sending the prologue */
  uim_rx_state[uim_drv_slot] = UIM_RX_T_1_SEND_PROLOGUE_ST;

#ifdef FEATURE_UIM_UART_DM
  /* Enable Tx level interrupt to cause T=1 block to be sent out.
     TX READY interrupt tells us when the FIFO is empty (last word moved
     to the packing buffer) and NO_CHARS_FOR_TX characters were written
     into the FIFO. As per the UART_DM design, it is forbidden to write
     the next TX transfer size into NO_CHARS_FOR_TX register before TX_READY
     interrupt is triggered. */
  UIM_PRG_IMR ( (MSMU_IMR_TX_READY | MSMU_IMR_RXLEV | MSMU_IMR_RXSTALE |
                 MSMU_IMR_RXBREAK | MSMU_IMR_TXLEV) );
#else
  /* Enable Tx level interrupt to cause T=1 block to be sent out */
  UIM_PRG_IMR ( (MSMU_IMR_RXLEV   | MSMU_IMR_RXSTALE |
                 MSMU_IMR_RXBREAK | MSMU_IMR_TXLEV) );
#endif /* FEATURE_UIM_UART_DM */
} /* uim_t_1_send_s_block */
#endif /* FEATURE_UIM_T_1_SUPPORT */


/*===========================================================================

FUNCTION UIM_POWER_DOWN                          EXTERNALIZED FUNCTION

DESCRIPTION
  This function deactivates the UIM so it does becomes inactive and does not
  use power.  The electrical lines to the UIM are all set low in the
  appropriate order.

DEPENDENCIES
  This function inactivates the UIM.  The uim_power_up and uim_reset
  functions must be called after this function to bring the UIM back up
  to use.

RETURN VALUE
  None

SIDE EFFECTS
  After this function is executed, the UIM is power down and inactive.

===========================================================================*/
void uim_power_down
(
#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  uim_slot_type slot
#else
  void
#endif
)
{
#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */

#ifdef FEATURE_UIM_SUPPORT_DUAL_SLOTS
  /* Set the slot as the slot to power down */
  if (slot == UIM_SLOT_1)
  {
    uim_drv_slot = UIM_DRV_SLOT1;
  }
  else if (slot == UIM_SLOT_2)
  {
    uim_drv_slot = UIM_DRV_SLOT2;
  }
#endif

#if defined(T_QSC6270) // ULC??????
  /* Enable the VREG-USIM pull-down resistor. Just before
     powering down the interface.*/
  (void) pm_vreg_pull_down_switch( PM_ON_CMD,  PM_VREG_PDOWN_USIM_ID);
#elif defined( FEATURE_UIM_PMIC_ON_UIM1 ) || \
      defined( FEATURE_UIM_PMIC_ON_UIM2 ) || \
      defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC )
  #if !defined( FEATURE_UIM_MDM8200_DRIVER )
    /* Do not turn off VREG_RUIM for 8200 */
    /* Enable the VREG-RUIM pull-down resistor. Just before
       powering down the interface.*/
    (void)pm_vreg_pull_down_switch( PM_ON_CMD, PM_VREG_PDOWN_RUIM_ID );
  #endif
#endif /* T_QSC6270 */

  /* Make sure the clock is on for the R-UIM interface */
  UIM_TCXO_MUST_BE_ON();

  /* Mask all interrupts from UART. */
  UIM_PRG_IMR ( MSMU_IMR_MASK_ALL );

  /* Reset the UART receiver and disable */
  UIM_RESET_RX ();

  MSG_LOW( "uim_power_down", 0, 0, 0 );

  /* Set the reset active */
  UIM_ASSERT_RESET ();

  /* Wait for 100 clock cycles before turning the clock off. */
  uim_clk_busy_wait ((100*1000000)/uim_clk_freq[uim_drv_slot]);

  /* Turn the clock to the UIM off */
  uim_clock_control ( UIM_CLOCK_LOW, uim_drv_slot );

  /* Wait for 100 clock cycles before setting I/O line low. */
  uim_clk_busy_wait ((100*1000000)/uim_clk_freq[uim_drv_slot]);

  /* Set the I/O line Low */
  UIM_START_BREAK ();

  /* Wait for 100 clock cycles before turning power off. */
  uim_clk_busy_wait ((100*1000000)/uim_clk_freq[uim_drv_slot]);

  /* Turn off the UIM */

#ifndef FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM
  UIM_POWER_OFF ();
#endif /* !FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM */

  /* Allow the clock for the R-UIM interface to be turned off */
  UIM_TCXO_CAN_BE_OFF();

#ifdef FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM
#error code not present
#endif /* FEATURE_UIM_CONTROL_FET_FOR_VREG_RUIM */

#if (defined( FEATURE_UIM_PMIC_ON_UIM1 ) || \
     defined( FEATURE_UIM_PMIC_ON_UIM2 ) || \
     defined( FEATURE_UIM_USE_ONLY_VREG_ON_PMIC )) && \
     defined( FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES )
  /* Wait an extra 2msecs for the PMIC to power down.
  If this causes problems in the build, remove the code and get the phone
  changed to power down the Vcc quicker. */
  uim_clk_busy_wait ( UIM_MICROSECONDS_PER_TWO_MILLISECONDS );
#endif

  /* clear the timer and the signal just in case
   * there is a dangling timer/signal
   */

  /* Clear the timer first */
  (void) rex_clr_timer(&uim_cmd_rsp_timer);

  /* Now clear the signal */
  (void) rex_clr_sigs( &uim_tcb, UIM_CMD_RSP_TIMEOUT_SIG );

  /* Configure the UIM_DATA and UIM_CLK GPIOs as PULL_DOWN */
  CONFIGURE_UIM_DATA_AND_CLK_AS_PULL_DOWN();

#ifdef FEATURE_UIM_QSC11X0_DRIVER
  /* Do not do rex_sleep due to TASK_STOP_SIG leading to uim_power_down */
  if(uim_powering_down_task != TRUE)
  {
    /* Add delay between voltage class switching for voltage ramp down issues */
    rex_sleep(UIM_VOLTAGE_CLASS_SWITCHING_DELAY);
  }
#endif /* FEATURE_UIM_QSC11X0_DRIVER */

} /* end - uim_power_down */


/*===========================================================================

FUNCTION UIM_RESET_RECEIVER                          EXTERNALIZED FUNCTION

DESCRIPTION
  This function resets the receiver and places the rx ISR to the idle
  state.

DEPENDENCIES
  This function inactivates the UART receiver.  Any data received while
  in idle are processed as errors.

RETURN VALUE
  None

SIDE EFFECTS
  After this function is executed, the UART receiver is disabled and the
  Rx ISR is in the idle state.

===========================================================================*/
LOCAL void uim_reset_receiver
(
  void
)
{
  /* Indicate a receive error condition */
/*  UIM_TEST_INT_PROCESSING(UIM_DEBUG_RX_ERROR); */

  /* Reset the receiver and flush the FIFO. */
  UIM_RESET_RX ();

  /* Reset the error status */
  UIM_RESET_ERR ();

  /* Reset the break change interrupt */
  UIM_RESET_BREAK ();

  /* Re-enable the receiver */
  UIM_ENABLE_UART_RX ();

} /* end - uim_reset_receiver */


/*===========================================================================

FUNCTION RX_ISR_PROCESS_IDLE                             INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when no bytes are expected from the UIM.  If called,
  bytes were received from the UIM.  This indicates a problem with the UIM/
  UIM interface.  The receiver is disabled and the higher layer software is
  informed of the situation.

DEPENDENCIES
  This function uses the callback function supplied in the uim_power_up
  function call.

RETURN VALUE
  None

SIDE EFFECTS
  This function resets the receiver to disable the receiver and flush the
  FIFO.

===========================================================================*/
LOCAL void rx_isr_process_idle
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Mask the Tx interrupt */
  UIM_PRG_IMR ( (MSMU_IMR_RXLEV | MSMU_IMR_RXSTALE | MSMU_IMR_RXBREAK) );

  /* Reset the receiver and leave it disabled.  This will ignore any
     more characters received from the UIM.       */
  UIM_RESET_RX ();

  /* This indicates something is wrong with the UIM/UIM interface
     Tell the upper layer                                           */
#ifndef FEATURE_UIM_IGNORE_UNSOLICITED_BYTES
  unsolict_resp_callback_func[uim_drv_slot] ();
#endif /* FEATURE_UIM_IGNORE_UNSOLICITED_BYTES */

} /* end rx_isr_process_idle */


/*===========================================================================

FUNCTION RX_ISR_RECEIVE_ATR                             INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting an ATR from the UIM.  The ATR bytes
  are processed as they come in.  If the ATR is received without conflict,
  the UIM server is notified.

DEPENDENCIES
  This function uses the callback function supplied in the uim_reset
  function call.  This processing state is set up after a UIM reset.

RETURN VALUE
  None

SIDE EFFECTS
  This function processes the received data stream as an ATR.  The data
  is checked as an ATR and if the data represents a valid ATR, the UIM
  server is notified.

===========================================================================*/
LOCAL void rx_isr_receive_atr
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  dword uart_status;  /* Holds the UART status */

  byte rx_value;     /* Holds the received value read from the receive FIFO. */

  #ifdef FEATURE_UIM_UART_DM
  uint32 rx_word;     /* Holds 4 Bytes of the ATR for some targets */
  byte  rx_value_array[4];
  uint32 i = 0; /* running index */
  int32 bytes_remaining;

  /* Boolean to indicate if a re-initialization of a new
     RX transfer is required */
  boolean reinit_rxinit = TRUE;

  num_bytes_received = UIM_GET_NUM_BYTES_IN_RX();
  bytes_remaining    = num_bytes_received;

  /* Ensure to clear the RX Stale Interrupt */
  if ( UIM_READ_MISR() & MSMU_ISR_RXSTALE )
  {
    /* Reset the rx bytes count after every stale timeout */
    num_bytes_in_this_rx_transfer[uim_drv_slot] = 0;

    /* Clear the Stale Interrupt Event */
    UIM_CLEAR_RX_STALE_INT();
  }
  #endif /* FEATURE_UIM_UART_DM */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Process all the ATR bytes in the receive FIFO */
  for (uart_status = UIM_READ_STATUS ();
       ((uart_status & (MSMU_SR_RXRDY | MSMU_SR_PF_ERR)) &&
#ifdef FEATURE_UIM_UART_DM
       (num_bytes_to_skip[uim_drv_slot] != 0) && (num_bytes_received>0) );
#else
        (num_bytes_to_skip[uim_drv_slot] != 0));
#endif /* FEATURE_UIM_UART_DM */
       uart_status = UIM_READ_STATUS ())
  {
    #ifdef FEATURE_UIM_UART_DM

    rx_word  = UIM_GET_RX_WORD();
    rx_value_array[0] = rx_word & 0xFF;
    rx_value_array[1] = (rx_word & 0xFF00)     >> 8;
    rx_value_array[2] = (rx_word & 0xFF0000)   >> 16;
    rx_value_array[3] = (rx_word & 0xFF000000) >> 24;

    for ( i = 0; i < 4 && num_bytes_received>0; i++)
    {

      num_bytes_received -=1;

      rx_value = rx_value_array[i];
      if ( bytes_remaining <= 0 )
      {
        while (UIM_READ_STATUS() & MSMU_SR_RXRDY)
        {
          /* Clear the FIFO in case it padded */
          (void)UIM_GET_RX_WORD();
        }
        break;
      }
      bytes_remaining--;

    #else

    /* Get the next received byte. */
    rx_value = UIM_GET_RX_BYTE ();

    #endif

    /* Make sure there is no problem with the received byte */
    if (uart_status & MSMU_SR_PF_ERR)
    {
#if defined( FEATURE_UIM_SUPPORT_INVERSE_CONVENTION )
      /* Check if this is the first byte */
      if (0 == num_resp_bytes_rcvd[uim_drv_slot])
      {
        /* Check if first byte indicates an inverse convention UIM card. */
        if (rx_value == INVERSE_CONVENTION_TS_DETECT)
        {
          /*----------------------------------------------------------------
           Setup the SIM glue logic for inverse convention mode.
          ----------------------------------------------------------------*/
          UIM_PROGRAM_INVERSE_CONVENTION();

          /* Reset the UART receiver.  All data is dropped.  Go back to
             idle.                                                         */
          UIM_RESET_RX ();
          uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

          /* Set the number of bytes received to zero. */
          resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

          /* Give a bad status. */
          resp_buf[uim_drv_slot]->cmd_status = UIM_CONVENTION_ERROR;

          /* Inform the UIM server that the ATR reception is finished */
          atr_rx_callback_func[uim_drv_slot] ();
#ifdef FEATURE_UIM_UART_DM
          reinit_rxinit = FALSE;
#endif /* FEATURE_UIM_UART_DM */

        }
        /* Check for an indication of a direct convention TS when in inverse
           mode */
        else if (rx_value == DIRECT_CONVENTION_TS_DETECT)
        {
          /*----------------------------------------------------------------
           Setup the SIM glue logic for inverse convention mode.
          ----------------------------------------------------------------*/
          UIM_PROGRAM_DIRECT_CONVENTION();

          /* Reset the UART receiver.  All data is dropped.  Go back to
             idle.                                                         */
          UIM_RESET_RX ();
          uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

          /* Set the number of bytes received to zero. */
          resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

          /* Give a bad status. */
          resp_buf[uim_drv_slot]->cmd_status = UIM_CONVENTION_ERROR;

          /* Inform the UIM server that the ATR reception is finished*/
          atr_rx_callback_func[uim_drv_slot] ();
#ifdef FEATURE_UIM_UART_DM
          reinit_rxinit = FALSE;
#endif /* FEATURE_UIM_UART_DM */

        } /* end if - check for inverse convention byte. */

      } /* end if - this is the first byte received. */
#endif /* FEATURE_UIM_SUPPORT_INVERSE_CONVENTION */
      /* Do not process this byte. */
      UIM_PROCESS_PARITY_ERROR_CONT( );
    } /* end if - check for parity error. */

#ifdef FEATURE_UIM_DEBUG_LOG
    UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, rx_value);
#endif

    /* Place the byte in the ATR receive buffer. */
    *resp_buffer_ptr[uim_drv_slot]++ = rx_value;

    /* Keep track of the number of atr bytes received. */
    num_resp_bytes_rcvd[uim_drv_slot]++;

    /* Another byte is processed.  Determine how many more bytes
       until this function needs to process a format byte.                   */
    num_bytes_to_skip[uim_drv_slot]--;

    /* Determine if this byte should be processed. */
    if (num_bytes_to_skip[uim_drv_slot] == 0)
    {
      /* Process the byte based on what is expected by this function */
      switch (receive_mode[uim_drv_slot])
      {
        case UIM_WAITING_FOR_T0:    /* Expecting the T0 character */
          {
#ifdef FEATURE_UIM_UART_DM
            reinit_rxinit = TRUE;
#endif /* #ifdef FEATURE_UIM_UART_DM */
            /* Save the number of historical bytes */
            num_historical_chars[uim_drv_slot] = (rx_value & UIM_K_FIELD);

            /* Determine how many bytes follow this format byte. */
            num_bytes_to_skip[uim_drv_slot] = uim_y_format_table[rx_value >>
              UIM_Y_FIELD_SHIFT];

            /* Determine if there will be another format byte. */
            if (rx_value & UIM_TD_PRESENT)
            {
              receive_mode[uim_drv_slot] = UIM_WAITING_FOR_TD;
            }
            else
            {
              /* The number of bytes to skip include the number of
                 bytes that follow this format character and the
                 number of historical characters.                            */
              num_bytes_to_skip[uim_drv_slot] +=
                num_historical_chars[uim_drv_slot];

              /* There will NOT be a TCK.  Determine if any bytes
                 follow in this ATR.                                         */
              if (num_bytes_to_skip[uim_drv_slot] > 0)
              {
                /* Wait for the rest of the bytes of the ATR. */
                receive_mode[uim_drv_slot] = UIM_WAITING_FOR_END;
              }
              else /* There are no more bytes in the ATR. */
              {
                /* Give a good status. */
                resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

                /* Copy the number of bytes received. */
                resp_buf[uim_drv_slot]->cmd_rsp_size =
                  num_resp_bytes_rcvd[uim_drv_slot];

                /* Inform the UIM server that the ATR has been received */
                atr_rx_callback_func[uim_drv_slot] ();
#ifdef FEATURE_UIM_UART_DM
                reinit_rxinit = FALSE;
#endif /* FEATURE_UIM_UART_DM */

                /* No more characters are expected at this time */
                uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
              } /* end if - more bytes to come. */
            } /* end if - TD byte is present in following bytes */
          } /* end case - UIM_WAITING_FOR_T0 */
          break;

        case UIM_WAITING_FOR_TD:    /* Expecting a TD character */
          {
#ifdef FEATURE_UIM_UART_DM
            reinit_rxinit = TRUE;
#endif /* FEATURE_UIM_UART_DM */
            /* Determine how many bytes follow this format byte. */
            num_bytes_to_skip[uim_drv_slot] =
              uim_y_format_table[rx_value >> UIM_Y_FIELD_SHIFT];

            /* Determine if there is a protocol other than 0. */
            if ((rx_value & UIM_T_FIELD) != 0)
            {
              /* This is indicates the TCK byte is expected */
              check_byte_expected[uim_drv_slot] = TRUE;
            } /* end if - a protocol other than 0 */

            /* Determine if there will NOT be another format byte. */
            if (!(rx_value & UIM_TD_PRESENT))
            {
              /* Determine if the check byte is expected */
              if (check_byte_expected[uim_drv_slot])
              {
                /* The number of bytes to skip include the number of
                   bytes that follow this format character and the
                   number of historical characters.  The check byte
                   is also included.                                         */
                num_bytes_to_skip[uim_drv_slot] +=
                  num_historical_chars[uim_drv_slot] + 1;

                /* Wait for the check byte. */
                receive_mode[uim_drv_slot] = UIM_WAITING_FOR_TCK;
              }
              else /* No check byte is expected */
              {
                /* The number of bytes to skip include the number of
                   bytes that follow this format character and the
                   number of historical characters.                          */
                num_bytes_to_skip[uim_drv_slot] +=
                  num_historical_chars[uim_drv_slot];

                /* There will NOT be a TCK.  Determine if any bytes
                   follow in this ATR.                                       */
                if (num_bytes_to_skip[uim_drv_slot] > 0)
                {
                  /* Wait for the rest of the bytes of the ATR. */
                  receive_mode[uim_drv_slot] = UIM_WAITING_FOR_END;
                }
                else /* There are no more bytes in the ATR. */
                {
                  /* Give a good status. */
                  resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

                  /* Copy the number of bytes received. */
                  resp_buf[uim_drv_slot]->cmd_rsp_size =
                    num_resp_bytes_rcvd[uim_drv_slot];

                  /* Inform the UIM server that the ATR has been received */
                  atr_rx_callback_func[uim_drv_slot] ();
#ifdef FEATURE_UIM_UART_DM
                  reinit_rxinit = FALSE;
#endif /* FEATURE_UIM_UART_DM */

                  /* No more characters are expected at this time */
                  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
                } /* end if - more bytes to come. */
              } /* end if - more bytes to come. */
            } /* end if - TD byte is present in following bytes */
          } /* end case - UIM_WAITING_FOR_TD */
          break;

        case UIM_WAITING_FOR_TCK:   /* Expecting the TCK character */
          {
            /* The ATR should have been completely received.  No other bytes
               are expected at this time                                     */
            uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

            /* Respond with status DONE */
            resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

            /* Copy the number of bytes received. */
            resp_buf[uim_drv_slot]->cmd_rsp_size =
              num_resp_bytes_rcvd[uim_drv_slot];

            /* Inform the UIM server the ATR was received without error */
            atr_rx_callback_func[uim_drv_slot] ();
#ifdef FEATURE_UIM_UART_DM
            reinit_rxinit = FALSE;
#endif /* FEATURE_UIM_UART_DM */

          } /* end case - UIM_WAITING_FOR_TCK */
          break;

        case UIM_WAITING_FOR_END:   /* Expecting the last of the
                                        ATR with NO TCK character expected   */
          {
            /* The ATR should have been completely received.  No other bytes
               are expected at this time                                     */
            uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

            /* Give a good status. */
            resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

            /* Copy the number of bytes received. */
            resp_buf[uim_drv_slot]->cmd_rsp_size =
              num_resp_bytes_rcvd[uim_drv_slot];

            /* Inform the UIM server the ATR was received */
            atr_rx_callback_func[uim_drv_slot] ();
#ifdef FEATURE_UIM_UART_DM
            reinit_rxinit = FALSE;
#endif /* FEATURE_UIM_UART_DM */
          } /* end case - UIM_WAITING_FOR_END */
          break;

        default:
          {
            /* The driver should never be here.  This is not a valid
               driver state.  Catch this problem.                            */
            MSG_ERROR ("Invalid receive mode while processing ATR",0,0,0);
            /* Reset the UART receiver.  All data is dropped.  Go back to
             * idle.  It is OK to disable the receiver since the failure
             * during ATR would result in a reset
             */
            uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

            /* Set the number of bytes received to zero. */
            resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

            /* Give a bad status. */
            resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

            /* Inform the UIM server that the ATR reception is finished*/
            atr_rx_callback_func[uim_drv_slot] ();
#ifdef FEATURE_UIM_UART_DM
            reinit_rxinit = FALSE;
#endif /* FEATURE_UIM_UART_DM */

          } /* end case - default */
          break;
      } /* end switch - receive_mode */
    } /* end if - this function should process this byte */
#ifdef FEATURE_UIM_UART_DM
    } /* end for */
#endif /* FEATURE_UIM_UART_DM */

    /* Make sure the maximum number of bytes are not exceeded.
       The maximum number of bytes are okay if the processing is complete */
    if ((num_resp_bytes_rcvd[uim_drv_slot] >= MAX_NUM_ATR_BYTES) &&
        (uim_rx_state[uim_drv_slot] != UIM_RX_PROCESS_IDLE))
    {
      /* Force the end of processing ATR bytes */
      num_bytes_to_skip[uim_drv_slot] = 0;

      /* Set the number of bytes received to zero. */
      resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

      /* Give a bad status. */
      resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

      /* Inform the UIM server that the ATR reception is finished*/
      atr_rx_callback_func[uim_drv_slot] ();
#ifdef FEATURE_UIM_UART_DM
      reinit_rxinit = FALSE;
#endif /* FEATURE_UIM_UART_DM */

      /* No more characters are expected at this time */
      uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

    } /* end if - check for maximum number of bytes */
  } /* end while - process all the bytes in the receive FIFO. */
#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_LAST_BYTE();
#endif

#ifdef FEATURE_UIM_UART_DM
  /* Re-initialize a new RX data transfer if needed. */
  if(reinit_rxinit == TRUE)
  {

    num_bytes_in_this_rx_transfer[uim_drv_slot] = 0;

    reinit_rxinit = FALSE;

    /* Re-initialize a new RX data transfer */
    UIM_DM_RX_INIT(MSMU_DEF_RX_INIT_VAL);

    /* Enable the RXSTALE and RXBREAK */
    UIM_PRG_IMR ( MSMU_IMR_RXSTALE | MSMU_IMR_RXBREAK );

    /* Enable Stale event */
    UIM_ENABLE_RX_STALE_INT();
  }

  if(uim_rx_state[uim_drv_slot] == UIM_RX_PROCESS_IDLE)
  {
    /* enable the parity error work-around only after the ATR is received */
    UIM_PRG_IPR(MSMU_IPR_DEF_STALE_TIMEOUT | MSMU_IPR_PAR_ERR_FIX_ENABLE);

    uim_reset_uart(uim_drv_slot);
  }
#endif /* FEATURE_UIM_UART_DM */
} /* end rx_isr_receive_atr */


/*===========================================================================

FUNCTION RX_ISR_RECEIVE_PPS                             INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting to receive a PPS response.  The PPS
  response bytes are processed as they come in.  If the PPS response is
  received without conflict, the UIM server is notified.

DEPENDENCIES
  This function uses the callback function supplied in the uim_send_pps
  function call.  This processing state is set up after sending the PPS
  request.

RETURN VALUE
  None

SIDE EFFECTS
  This function processes the received data as a PPS response.  The data
  is checked as a PPS response and if the data represents a valid PPS response,
  the UIM server id notified.

===========================================================================*/
LOCAL void rx_isr_receive_pps
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{

  dword uart_status;  /* Holds the UART status */

  byte rx_value;     /* Holds the received value read from the receive FIFO. */

#ifdef FEATURE_UIM_UART_DM
  uint32 rx_word;     /* Holds 4 Bytes of the PPS */
  byte  rx_value_array[4];
  uint32 i = 0; /* Running index */
  int32 bytes_remaining;

  /* Boolean variable to indicate if a new RX initialization is required */
  boolean reinit_rxinit = TRUE;

  /* Wait for TX_READY event to get confirmation that the last PPS byte was
     sent */
  if ( (uim_rx_sub_state == UIM_RX_WAIT_FOR_TXREADY) &&
       (UIM_READ_MISR() & MSMU_ISR_TX_READY) )
  {
    /* Clear the TX_READY interrupt */
    UIM_CLEAR_TX_READY_INT();

    /* There is no need to turn ON the WWT timer here as we do that
       in uim_rx_isr() */

    return;
  }

  num_bytes_received = UIM_GET_NUM_BYTES_IN_RX();
  bytes_remaining    = num_bytes_received;

  num_bytes_in_this_rx_transfer[uim_drv_slot] = 0;
#endif /* FEATURE_UIM_UART_DM */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Process all the bytes in the receive FIFO until the PPS is received */
  for (uart_status = UIM_READ_STATUS ();
       ((uart_status & MSMU_SR_RXRDY) &&
#ifdef FEATURE_UIM_UART_DM
       (num_bytes_to_skip[uim_drv_slot] != 0) && (num_bytes_received>0));
#else
        (num_bytes_to_skip[uim_drv_slot] != 0));
#endif /* FEATURE_UIM_UART_DM */
       uart_status = UIM_READ_STATUS ())
  {
    #ifdef FEATURE_UIM_UART_DM
    rx_word  = UIM_GET_RX_WORD();
    rx_value_array[0] = rx_word & 0xFF;
    rx_value_array[1] = (rx_word & 0xFF00)     >> 8;
    rx_value_array[2] = (rx_word & 0xFF0000)   >> 16;
    rx_value_array[3] = (rx_word & 0xFF000000) >> 24;

    for ( i = 0; i < 4; i++)
    {

       num_bytes_received -=1;

       rx_value = rx_value_array[i];

       if ( bytes_remaining <= 0 )
       {
         while (UIM_READ_STATUS() & MSMU_SR_RXRDY)
         {
            /* Clear the FIFO in case it padded */
            (void)UIM_GET_RX_WORD();
         }
         break;
       }
       bytes_remaining--;

    #else

    /* Get the next received byte. */
    rx_value = UIM_GET_RX_BYTE ();

    #endif /* FEATURE_UIM_UART_DM */

    /* Make sure there is no problem with the received byte */
    if (uart_status & MSMU_SR_PF_ERR)
    {
      UIM_PROCESS_PARITY_ERROR_CONT( );
    } /* end if - check for parity error. */

#ifdef FEATURE_UIM_DEBUG_LOG
    UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, rx_value);
#endif

    /* Place the byte in the ATR receive buffer. */
    *resp_buffer_ptr[uim_drv_slot]++ = rx_value;

    /* Keep a rolling value of the check byte. */
    check_byte[uim_drv_slot] ^= rx_value;

    /* Keep track of the number of PPS bytes received. */
    num_resp_bytes_rcvd[uim_drv_slot]++;

    /* Another byte is processed.  Determine how many more bytes
       until this function needs to process a format byte.                   */
    num_bytes_to_skip[uim_drv_slot]--;

    /* Determine if this byte should be processed. */
    if (num_bytes_to_skip[uim_drv_slot] == 0)
    {
      /* Process the byte based on what is expected by this function */
      switch (receive_mode[uim_drv_slot])
      {
        case UIM_WAITING_FOR_T0:    /* Expecting the T0 character */
          {
            /* Determine how many bytes follow this format byte. */
            num_bytes_to_skip[uim_drv_slot] =
                      uim_y_format_table[rx_value >> UIM_Y_FIELD_SHIFT] + 1;

            /* Wait for the check character. */
            receive_mode[uim_drv_slot] = UIM_WAITING_FOR_TCK;
#ifdef FEATURE_UIM_UART_DM
            reinit_rxinit = TRUE;
#endif /* FEATURE_UIM_UART_DM */
          } /* end case - UIM_WAITING_FOR_T0 */
          break;

        case UIM_WAITING_FOR_TCK:   /* Expecting the TCK character */
          {
#ifdef FEATURE_UIM_UART_DM
            reinit_rxinit = TRUE;
#endif /* FEATURE_UIM_UART_DM */
            /* The ATR should have been completely received.  No other bytes
               are expected at this time                                     */
            uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

            /* Determine if the ATR was received without error */
            if (check_byte[uim_drv_slot] == 0)
            {
              /* Give a good status. */
              resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

              /* Copy the number of bytes received. */
              resp_buf[uim_drv_slot]->cmd_rsp_size =
                num_resp_bytes_rcvd[uim_drv_slot];

              /* Inform the UIM server the PPS response was received without
                 error                                                       */
              pps_resp_callback_func[uim_drv_slot] ();
#ifdef FEATURE_UIM_UART_DM
              reinit_rxinit = FALSE;
#endif /* FEATURE_UIM_UART_DM */
            } /* end if - ATR was received without error */
            else
            {
              /* Set the number of bytes received to zero. */
              resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

              /* Give a bad status. */
              resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

              /* Inform the UIM server the PPS response is finished */
              pps_resp_callback_func[uim_drv_slot] ();
#ifdef FEATURE_UIM_UART_DM
              reinit_rxinit = FALSE;
#endif /* FEATURE_UIM_UART_DM */
            }
          } /* end case - UIM_WAITING_FOR_TCK */
          break;

        case UIM_WAITING_FOR_TD:    /* Expecting a TD character */
        case UIM_WAITING_FOR_END:   /* Expecting the last of the
                                   ATR with NO TCK character expected */
        default:
          {
            /* The driver should never be here.  This is not a valid
               driver state.  Catch this problem.                            */
            MSG_ERROR ("Invalid receive mode while processing the PPS",0,0,0);
            /* Reset the UART receiver.  All data is dropped.  Go back to
             * idle.  It is OK to disable the receiver since the failure
             * during PPS would result in a reset
             */
            UIM_RESET_RX ();
            uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

            /* Force an end to this processing */
            num_bytes_to_skip[uim_drv_slot] = 0;

            /* Set the number of bytes received to zero. */
            resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

            /* Give a bad status. */
            resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

            /* Inform the UIM server the PPS response was received with an
               error                                                       */
            pps_resp_callback_func[uim_drv_slot] ();
#ifdef FEATURE_UIM_UART_DM
            reinit_rxinit = FALSE;
#endif /* FEATURE_UIM_UART_DM */
          } /* end case - default */
          break;
      } /* end switch - receive_mode */
    } /* end if - this function should process this byte */

    /* Make sure the maximum number of bytes are not exceeded. */
    if ((num_resp_bytes_rcvd[uim_drv_slot] >= MAX_NUM_PPS_BYTES) &&
        (uim_rx_state[uim_drv_slot] != UIM_RX_PROCESS_IDLE))
    {
      /* Force the end of processing PPS response */
      num_bytes_to_skip[uim_drv_slot] = 0;

      /* Set the number of bytes received to zero. */
      resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

      /* Give a bad status. */
      resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

      /* Inform the UIM server the PPS response was received with an
         error                                                       */
      pps_resp_callback_func[uim_drv_slot] ();

      /* No more characters are expected at this time */
      uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
#ifdef FEATURE_UIM_UART_DM
      reinit_rxinit = FALSE;
#endif /* FEATURE_UIM_UART_DM */
    } /* end if - check for maximum number of bytes */
  } /* end while - process all the bytes in the receive FIFO. */
#ifdef FEATURE_UIM_UART_DM
 }
#endif /* FEATURE_UIM_UART_DM */

#ifdef FEATURE_UIM_UART_DM
/* Re-initialize a new RX data transfer if needed. */
if(reinit_rxinit == TRUE)
{
      reinit_rxinit = FALSE;

      num_bytes_in_this_rx_transfer[uim_drv_slot] = 0;

      /* Re-initialize a new RX data transfer */
      UIM_DM_RX_INIT(MSMU_DEF_RX_INIT_VAL);

      UIM_PRG_IMR ( MSMU_IMR_RXSTALE | MSMU_IMR_RXBREAK );

      /* Enable Stale event */
      UIM_ENABLE_RX_STALE_INT();
}
#endif /* FEATURE_UIM_UART_DM */

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_LAST_BYTE();
#endif
} /* end rx_isr_receive_pps */


/*===========================================================================

FUNCTION SEND_SUBSEQUENT_BYTE                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when a command data byte has been sent properly.  This
  function ensures the command data bytes are all sent then waits for a
  procedure byte.

DEPENDENCIES
  This function is called from the receive ISR after processing an ACK and
  while sending out all the data bytes.  This function is called after each
  character's sample data has been received.

RETURN VALUE
  None

SIDE EFFECTS
  This function attempts to send the command data bytes one at a time.

===========================================================================*/
LOCAL void send_subsequent_byte
(
  void
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Keep track of the number of bytes to send */
  --num_bytes_to_send[uim_drv_slot];

  /* Send the next byte */
  request_buffer_ptr[uim_drv_slot]++;
#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_TX_DATA, *request_buffer_ptr[uim_drv_slot]);
#endif
  uim_send_byte (*request_buffer_ptr[uim_drv_slot]);
} /* end send_subsequent_byte */


#ifdef FEATURE_UIM_UART_DM
/*===========================================================================

FUNCTION RX_ISR_SW1_RESP_END_DM                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a response
  end status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of a
  response ending status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_resp_end_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte sw2
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Send a get_response to this command */
  /* Get the length of the GET RESPONSE command response. */
  uim_get_resp_sw2 = (uim_sw2_type)sw2;

  /* Determine if the local buffer is full */
  if (num_resp_bytes_rcvd_total[uim_drv_slot] != UIM_MAX_CHARS)
  {
    /* If we choose not to do a get response either because of
     * having the response cached, or due to this being an
     * intermediate select or even if this is a re-selection
     * to the MF since the last selection failed.  Do a get response
     * any way if there was an error or a warning
     */
    if ( ( uim_item_sel_rsp_cached  || uim_intermediate_sel ||
           uim_reselect_mf) && ! ( uim_force_get_response ||
                                   uim_gcf_testing_flag) )
    {
      resp_buf[uim_drv_slot]->sw1 = (int)SW1_NORMAL_END;
      resp_buf[uim_drv_slot]->sw2 = (int)SW2_NORMAL_END;

      /* Indicate APDU result */
      resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

      /* Call the response callback function */
      apdu_resp_callback_func[uim_drv_slot]();

      /* Change to idle */
      uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
    }
    else
    {
      /* Determine how much will fit into the buffer */
      if ((num_resp_bytes_rcvd_total[uim_drv_slot] + sw2) > UIM_MAX_CHARS)
      {
        sw2 = (byte)(UIM_MAX_CHARS - num_resp_bytes_rcvd_total[uim_drv_slot]);
      } /* end if - requested size is too big for local buffer */

      /* Build the GET RESPONSE command */
      /* This needs to be modified to work with WIM */
      if ((uimdrv_protocol[uim_drv_slot] == UIM_UICC) ||
          (uimdrv_protocol[uim_drv_slot] == UIM_WCDMA))
      {
        cmd_req[uim_drv_slot].apdu_hdr.uim_class = 0x00;
#ifdef FEATURE_UIM_UTIL
        cmd_req[uim_drv_slot].apdu_hdr.uim_class |=
          (last_sent_apdu[uim_drv_slot].uim_class & UIM_LOGICAL_CHANNEL_MASK);
#endif /* FEATURE_UIM_UTIL */
      }
      else
      {
        cmd_req[uim_drv_slot].apdu_hdr.uim_class = 0xA0;
      }

      cmd_req[uim_drv_slot].protocol        = uimdrv_protocol[uim_drv_slot];
      cmd_req[uim_drv_slot].apdu_hdr.instrn = GET_RESPONSE;
      cmd_req[uim_drv_slot].apdu_hdr.p1     = 0x00;
      cmd_req[uim_drv_slot].apdu_hdr.p2     = 0x00;
      cmd_req[uim_drv_slot].instrn_case     = UIM_INSTRN_CASE_2;
      cmd_req[uim_drv_slot].rsp_ptr         = resp_buf[uim_drv_slot];
      cmd_req[uim_drv_slot].rpt_function    =
        apdu_resp_callback_func[uim_drv_slot];

      /* If this was a warning let us set the length to 0 */
      if (uim_rx_state[uim_drv_slot] == UIM_RX_SW1_WARNINGS1 ||
          uim_rx_state[uim_drv_slot] == UIM_RX_SW1_WARNINGS2)
      {
        cmd_req[uim_drv_slot].apdu_hdr.p3     = 0;
      }
      else
      {
        cmd_req[uim_drv_slot].apdu_hdr.p3     = sw2;
      }

      /* Wait long enough before responding to procedure byte */
      uim_clk_busy_wait( uim_response_byte_delay );

      /* Send the GET RESPONSE command */
      uim_send_apdu ( &cmd_req[uim_drv_slot] );
    } /* else of uim_item_sel_rsp_cached */
  }
  else /* The local buffer is full */
  {
    /* End this transaction */
#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
    /* Check if this from processing a download error */
    if (processing_download_error[uim_drv_slot])
    {
      resp_buf[uim_drv_slot]->sw1 = (int)SW1_DLOAD_ERR;
      resp_buf[uim_drv_slot]->sw2 = cmd_req[uim_drv_slot].apdu_hdr.p3;
    } /* end if - Finished processing a data download error */
    else /* Emulate a normal end */
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */
    {
      resp_buf[uim_drv_slot]->sw1 = (int)SW1_NORMAL_END;
      resp_buf[uim_drv_slot]->sw2 = (int)SW2_NORMAL_END;
    }
    /* Place the number of bytes read in the response buffer */
    resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd_total[uim_drv_slot];

    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
    /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
    processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

    /* Change to idle */
    uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
  } /* end if - the local buffer is full */
} /* end rx_isr_sw1_resp_end_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_NORMAL_END                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a normal
  end status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of a
  normal ending status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_normal_end_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte sw2
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Make sure the second status word is proper */
  if (sw2 == (int)SW2_NORMAL_END)
  {
    /* Place the number of bytes read in the response buffer */
    resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd_total[uim_drv_slot];

    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

    /* Check if we are processing an ENVELOPE APDU:
       If TRUE and warnings were received earlier, set the
       appropriate SW1 WARNING status. */

    if(uim_processing_envelope_apdu() == TRUE)
    {
      if(uim_warning1_indicator == TRUE)
      {
        uim_get_resp_sw1 = SW1_WARNINGS1;
      }
      else if(uim_warning2_indicator == TRUE)
      {
        uim_get_resp_sw1 = SW1_WARNINGS2;
      }
    } /* end  if - finished processing ENVELOPE which returned warning status */

    /* Reset the warning1 indicators */
    uim_warning1_indicator = FALSE;

    /* Reset the warning2 indicators */
    uim_warning2_indicator = FALSE;

    /* Send an indication the APDU completed successfully. */
    UIM_TEST_INT_PROCESSING(UIM_DEBUG_NORMAL_END);

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();

    /* Change to idle */
    uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

  } /* end if - the second status word is proper */
  else
  {

    /* For LCU - Need to do something with this case.. Later */

    /* Something went wrong.  We got a bad uim_tx_state value
     * set the sate to UIM_RX_UNKNOWN_PROC_BYTE_RCVD this would ignore all
     * bytes untill a time-out and then re-try the command
     */
    uim_rx_state[uim_drv_slot] = UIM_RX_UNKNOWN_PROC_BYTE_RCVD;

    /* Indicate the task to ignore the following timeout */
    uim_bad_status_words_error = TRUE;

    /* Set the number of bytes received to zero. */
    resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

    /* Give a bad status. */
    resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();
  }
#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

} /* end rx_isr_sw1_normal_end_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_FETCH_END_DM                       INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_fetch_end_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2 /* Second status byte */
)
{

  /* Save the status word.  It contains the size of the proactive command. */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Place the number of bytes read in the response buffer */
  resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd_total[uim_drv_slot];

  /* Indicate APDU result */
  resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* Check if this from processing a download error */
  if (processing_download_error[uim_drv_slot])
  {
    /* Set the status words to reflect a download error */
    resp_buf[uim_drv_slot]->sw1 = (int)SW1_DLOAD_ERR;
    resp_buf[uim_drv_slot]->sw2 = cmd_req[uim_drv_slot].apdu_hdr.p3;
  } /* end if - Finished processing a data download error */
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Check if we are processing an ENVELOPE APDU:
     If TRUE and warnings were received earlier, set the
     appropriate SW1 WARNING status. */
  if(uim_processing_envelope_apdu() == TRUE)
  {
    if(uim_warning1_indicator == TRUE)
    {
      uim_get_resp_sw1 = SW1_WARNINGS1;
    }
    else if(uim_warning2_indicator == TRUE)
    {
      uim_get_resp_sw1 = SW1_WARNINGS2;
    }
  }

  /* Reset the warning1 indicators */
  uim_warning1_indicator = FALSE;

  /* Reset the warning2 indicators */
  uim_warning2_indicator = FALSE;

  /* Send an indication the APDU completed successfully. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_NORMAL_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;


} /* end rx_isr_sw1_fetch_end_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_DLOAD_ERR_DM                       INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_dload_err_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

  uim_get_resp_sw1 = SW1_DLOAD_ERR;
  uim_get_resp_sw2 = (uim_sw2_type)sw2;

  /* Determine if the local buffer is full */
  if (num_resp_bytes_rcvd_total[uim_drv_slot] != UIM_MAX_CHARS)
  {
    /* Determine how much will fit into the buffer */
    if ((num_resp_bytes_rcvd_total[uim_drv_slot] + sw2) > UIM_MAX_CHARS)
    {
      sw2 = (byte)(UIM_MAX_CHARS - num_resp_bytes_rcvd_total[uim_drv_slot]);
    } /* end if - requested size is too big for local buffer */

    /* Build the GET RESPONSE command */
    /* This needs to be modified to work with WIM */
    if ((uimdrv_protocol[uim_drv_slot] == UIM_UICC) ||
        (uimdrv_protocol[uim_drv_slot] == UIM_WCDMA))
    {
      cmd_req[uim_drv_slot].apdu_hdr.uim_class = 0x00;

#ifdef FEATURE_UIM_UTIL
      cmd_req[uim_drv_slot].apdu_hdr.uim_class |=
        (last_sent_apdu[uim_drv_slot].uim_class & UIM_LOGICAL_CHANNEL_MASK);
#endif /* FEATURE_UIM_UTIL */
    }
    else
    {
      cmd_req[uim_drv_slot].apdu_hdr.uim_class = 0xA0;
    }

    cmd_req[uim_drv_slot].protocol        = uimdrv_protocol[uim_drv_slot];
    cmd_req[uim_drv_slot].apdu_hdr.instrn = GET_RESPONSE;
    cmd_req[uim_drv_slot].apdu_hdr.p1     = 0x00;
    cmd_req[uim_drv_slot].apdu_hdr.p2     = 0x00;
    cmd_req[uim_drv_slot].apdu_hdr.p3     = sw2;
    cmd_req[uim_drv_slot].instrn_case     = UIM_INSTRN_CASE_2;
    cmd_req[uim_drv_slot].rsp_ptr         = resp_buf[uim_drv_slot];
    cmd_req[uim_drv_slot].rpt_function    =
      apdu_resp_callback_func[uim_drv_slot];

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
    /* This flag is processed in the Normal end SW processing.
       It distinguishes between data download error and all other
       results. */
    processing_download_error[uim_drv_slot] = TRUE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

    /* Wait long enough before responding to procedure byte */
    uim_clk_busy_wait( uim_response_byte_delay );

    /* Send the GET RESPONSE command */
    uim_send_apdu ( &cmd_req[uim_drv_slot] );
  }
  else /* The local buffer is full */
  {
    /* End this transaction */
    resp_buf[uim_drv_slot]->sw1 = (int)SW1_DLOAD_ERR;
    resp_buf[uim_drv_slot]->sw2 = cmd_req[uim_drv_slot].apdu_hdr.p3;
    /* Place the number of bytes read in the response buffer */
    resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd_total[uim_drv_slot];

    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
    /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
    processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

    /* Change to idle */
    uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
  } /* end if - the local buffer is full */

} /* end rx_isr_sw1_dload_err_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_BUSY_DM                       INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_busy_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

    /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Make sure the second status word is proper */
  if (sw2 == (int)SW2_NORMAL_END)
  {
    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_TOOLKIT_BUSY;

    /* Set the number of bytes received to zero. */
    resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

    /* Send an indication the APDU completed with an error. */
    UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();

    /* Change to idle */
    uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
  } /* end if - the second status word is proper */
  else
  {
    /* Something went wrong.  We got a bad uim_tx_state value
     * set the sate to UIM_RX_UNKNOWN_PROC_BYTE_RCVD this would ignore all
     * bytes untill a time-out and then re-try the command
     */
    uim_rx_state[uim_drv_slot] = UIM_RX_UNKNOWN_PROC_BYTE_RCVD;

    /* Indicate the task to ignore the following timeout */
    uim_bad_status_words_error = TRUE;

    /* Set the number of bytes received to zero. */
    resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

    /* Give a bad status. */
    resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();
  }
#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

} /* end rx_isr_sw1_busy_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_RETRIES_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_retries_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

    /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Determine if it is a memory problem */
  if (sw2 == (int)SW2_MEM_PROBLEM)
  {
    /* Set the number of bytes received to zero. */
    resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_MEMORY_PROBLEM;
  }
  else /* successful after retries */
  {
    /* Place the number of bytes read in the response buffer */
    resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd_total[uim_drv_slot];

    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_SUCCESS_AFTER_MULTI_TRIES;
  } /* end if - the second status word indicates a memory problem */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

} /* end rx_isr_sw1_retries_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_REFERENCE_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_reference_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

    /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Process the second status word */
  switch (sw2)
  {
    case SW2_NO_EF:           /* No EF selected */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_NO_EF_SELECTED;
      } /* end case - SW2_NO_EF */
      break;

    case SW2_OUT_OF_RANGE:    /* Out of range (invalid address) */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_OUT_OF_RANGE;
      } /* end case - SW2_OUT_OF_RANGE */
      break;

    case SW2_NOT_FOUND:       /* File ID/pattern not found */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_NOT_FOUND;
      } /* end case - SW2_NOT_FOUND */
      break;

    case SW2_INCONSISTENT:    /* File inconsistent with command */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_INCONSISTENT_WITH_COMMAND;
      } /* end case - SW2_INCONSISTENT */
      break;

#if defined(FEATURE_UIM_UICC) || defined(FEATURE_UIM_UTIL)
    case SW2_ALGORITHM_NOT_SUPPORTED:
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_ALGORITHM_NOT_SUPPORTED;
      } /* end case - SW2_ALGORITHM_NOT_SUPPORTED */
      break;

    case SW2_INVALID_KEY_CHECK_VALUE:
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_INVALID_KEY_CHECK_VALUE;
      } /* end case - SW2_INVALID_KEY_CHECK_VALUE */
      break;

#endif /* FEATURE_UIM_UICC  || FEATURE_UIM_UTIL */
    default:
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

} /* end rx_isr_sw1_reference_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_SECURITY_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_security_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

    /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Process the second status word */
  switch (sw2)
  {
    case SW2_NO_CHV:          /* No CHV initialized */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_NO_CHV_INITIALIZED;
      } /* end case - SW2_NO_CHV */
      break;

    case SW2_NOT_FULFILLED:   /* Access condition not fulfilled */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_ACCESS_NOT_FULFILLED;
      } /* end case - SW2_NOT_FULFILLED */
      break;

    case SW2_CONTRADICTION:   /* In contradiction with CHV status */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_CONTRADICTION_W_CHV_STATUS;
      } /* end case - SW2_CONTRADICTION */
      break;

    case SW2_INVALIDATION:    /* In contradiction with invalidation status */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status =
          UIM_CONTRADICTION_W_INVALID_STATUS;
      } /* end case - SW2_INVALIDATION */
      break;

    case SW2_SEQ_PROBLEM:     /* Sequence problem */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_SSD_UPDATE_SEQ_PROBLEM;
      } /* end case - SW2_SEQ_PROBLEM */
      break;

    case SW2_BLOCKED:         /* Access blocked */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_CHV_BLOCKED;
      } /* end case - SW2_BLOCKED */
      break;

    case SW2_MAX_REACHED:     /* Increase cannot be performed; Max value
                                 reached                          */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_MAX_VALUE_REACHED;
      } /* end case - SW2_MAX_REACHED */
      break;

#ifdef FEATURE_UIM_UICC
    case SW2_APPLN_SPECIFIC_AUTH_ERR:  /* Applcn specific auth error */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_APPLCN_SPECIFIC_AUTH_ERROR;
      } /* end case - SW2_APPLN_SPECIFIC_AUTH_ERR */
      break;
#endif /* FEATURE_UIM_UICC */

    default:
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

} /* end rx_isr_sw1_security_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_P3_BAD_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_p3_bad_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

    uim_get_resp_sw1 = SW1_P3_BAD;
  uim_get_resp_sw2 = (uim_sw2_type)sw2;

  if (sw2 == 0x00)
  {
    /* Save the status word in case it is a true status word response */
    resp_buf[uim_drv_slot]->sw2 = sw2;

    /* Set the number of bytes received to zero. */
    resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_INCORRECT_PARAM_P3;

    /* Send an indication the APDU completed with an error. */
    UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
    /* This flag is processed in the Normal end SW processing.
       It distinguishes between data download error and all other
       results. */
    processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

    /* Change to idle */
    uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
  }
  /* If SW2 is XX !=0, then resend the cmd with SW2 as P3 */
  else
  {
    /* Determine if the local buffer is full */
    if (num_resp_bytes_rcvd_total[uim_drv_slot] != UIM_MAX_CHARS)
    {
      /* Determine how much will fit into the buffer */
      if ((num_resp_bytes_rcvd_total[uim_drv_slot] + sw2) > UIM_MAX_CHARS)
      {
        sw2 = (byte)(UIM_MAX_CHARS - num_resp_bytes_rcvd_total[uim_drv_slot]);
      } /* end if - requested size is too big for local buffer */

      /* Resend the last APDU with p3 equal to SW2 */
      cmd_req[uim_drv_slot].protocol        = uimdrv_protocol[uim_drv_slot];
      cmd_req[uim_drv_slot].apdu_hdr        = last_sent_apdu[uim_drv_slot];
      cmd_req[uim_drv_slot].apdu_hdr.p3     = sw2;
      cmd_req[uim_drv_slot].instrn_case     = uim_last_instrn_case[uim_drv_slot];
      cmd_req[uim_drv_slot].rsp_ptr         = resp_buf[uim_drv_slot];
      cmd_req[uim_drv_slot].rpt_function    =
        apdu_resp_callback_func[uim_drv_slot];
      /* Wait long enough before responding to procedure byte */
      uim_clk_busy_wait( uim_response_byte_delay );
      /* Send the last command command */
      uim_send_apdu ( &cmd_req[uim_drv_slot] );
    }
    else /* The local buffer is full */
    {
      /* End this transaction */
#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
      /* Check if this from processing a download error */
      if (processing_download_error[uim_drv_slot])
      {
        resp_buf[uim_drv_slot]->sw1 = (int)SW1_DLOAD_ERR;
        resp_buf[uim_drv_slot]->sw2 = cmd_req[uim_drv_slot].apdu_hdr.p3;
      } /* end if - Finished processing a data download error */
      else /* Emulate a normal end */
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */
      {
        resp_buf[uim_drv_slot]->sw1 = (int)SW1_NORMAL_END;
        resp_buf[uim_drv_slot]->sw2 = (int)SW2_NORMAL_END;
      }
      /* Place the number of bytes read in the response buffer */
      resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd_total[uim_drv_slot];

      /* Indicate APDU result */
      resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

      /* Call the response callback function */
      apdu_resp_callback_func[uim_drv_slot]();

  #ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
      /* This flag is processed in the Normal end SW processing.
       It distinguishes between data download error and all other
       results. */
      processing_download_error[uim_drv_slot] = FALSE;
  #endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

      /* Change to idle */
      uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
    } /* end if - the local buffer is full */
  } /* SW2 is 0 */

} /* end rx_isr_sw1_p3_bad_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_P1_P2_BAD_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_p1_p2_bad_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

    /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Set the number of bytes received to zero. */
  resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

  /* Indicate APDU result */
  resp_buf[uim_drv_slot]->cmd_status = UIM_INCORRECT_PARAM_P1_2;

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

} /* end rx_isr_sw1_p1_p2_bad_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_INS_BAD_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_ins_bad_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

    /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Set the number of bytes received to zero. */
  resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

  /* Indicate APDU result */
  resp_buf[uim_drv_slot]->cmd_status = UIM_UNKNOWN_INS;

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

} /* end rx_isr_sw1_ins_bad_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_PROBLEM_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_problem_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

    /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Set the number of bytes received to zero. */
  resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

  /* Indicate APDU result */
  resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

} /* end rx_isr_sw1_problem_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_RESEND_APDU_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_resend_apdu_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

  uim_get_resp_sw1 = SW1_USIM_RESEND_APDU;
  uim_get_resp_sw2 = (uim_sw2_type)sw2;

  /* Determine if the local buffer is full */
  if (num_resp_bytes_rcvd_total[uim_drv_slot] != UIM_MAX_CHARS)
  {
    /* Determine how much will fit into the buffer */
    if ((num_resp_bytes_rcvd_total[uim_drv_slot] + sw2) > UIM_MAX_CHARS)
    {
      sw2 = (byte)(UIM_MAX_CHARS - num_resp_bytes_rcvd_total[uim_drv_slot]);
    } /* end if - requested size is too big for local buffer */

    /* Resend the last APDU with p3 equal to SW2*/
    cmd_req[uim_drv_slot].protocol        = uimdrv_protocol[uim_drv_slot];
    cmd_req[uim_drv_slot].apdu_hdr        = last_sent_apdu[uim_drv_slot];
    cmd_req[uim_drv_slot].apdu_hdr.p3     = sw2;
    cmd_req[uim_drv_slot].instrn_case     = uim_last_instrn_case[uim_drv_slot];
    cmd_req[uim_drv_slot].rsp_ptr         = resp_buf[uim_drv_slot];
    cmd_req[uim_drv_slot].rpt_function    =
      apdu_resp_callback_func[uim_drv_slot];

    /* Wait long enough before responding to procedure byte */
    uim_clk_busy_wait( uim_response_byte_delay );

    /* Send the last command command */
    uim_send_apdu ( &cmd_req[uim_drv_slot] );
  }
  else /* The local buffer is full */
  {
    /* End this transaction */
#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
    /* Check if this from processing a download error */
    if (processing_download_error[uim_drv_slot])
    {
      resp_buf[uim_drv_slot]->sw1 = (int)SW1_DLOAD_ERR;
      resp_buf[uim_drv_slot]->sw2 = cmd_req[uim_drv_slot].apdu_hdr.p3;
    } /* end if - Finished processing a data download error */
    else /* Emulate a normal end */
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */
    {
      resp_buf[uim_drv_slot]->sw1 = (int)SW1_NORMAL_END;
      resp_buf[uim_drv_slot]->sw2 = (int)SW2_NORMAL_END;
    }
    /* Place the number of bytes read in the response buffer */
    resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd_total[uim_drv_slot];

    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
    /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
    processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

    /* Change to idle */
    uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
  } /* end if - the local buffer is full */

} /* end rx_isr_sw1_resend_apdu_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_WARNINGS1_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_warnings1_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

    /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Process the second status word */
  switch (sw2)
  {
    case SW2_NV_STATE_UNCHANGED:          /* No info with NV unchanged */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_NO_INFO_AND_NV_UNCHANGED;
      } /* end case - SW2_NV_STATE_UNCHANGED */
      break;
#ifdef FEATURE_UIM_UICC
    case SW2_RET_DATA_MAY_BE_CORRUPTED:   /* Part of ret data may be corrupt */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PART_DATA_MAY_BE_CORRUPTED;
      } /* end case - SW2_RET_DATA_MAY_BE_CORRUPTED */
      break;

    case SW2_EOF_REACHED:            /* EOF reached before reading Le bytes  */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_EOF_BEFORE_READING_LE_BYTES;
      } /* end case - SW2_EOF_REACHED */
      break;

    case SW2_SEL_EF_INVALIDATED:    /* Selected file invalidated */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_SELECTED_FILE_INVALIDATED;
      } /* end case - SW2_SEL_EF_INVALIDATED */
      break;

    case SW2_INCORRECT_FCI_FORMAT:     /* FCI not formatted correctly */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_INCORRECT_FCI_FORMAT;
      } /* end case - SW2_INCORRECT_FCI_FORMAT */
      break;
#endif /* FEATURE_UIM_UICC */
    default:
      {

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

} /* end rx_isr_sw1_warnings1_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_WARNINGS2_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_warnings2_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  switch (sw2)                 /* Process the second status word */
  {
    case SW2_NV_STATE_CHANGED:          /* No info with NV changed */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_NO_INFO_AND_NV_CHANGED;
      } /* end case -  */
      break;
#ifdef FEATURE_UIM_UICC
    case SW2_FILE_FILLED_BY_LAST_WRITE:   /* File filled up by last write */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_FILE_FILLED_UP_BY_LAST_WRITE;
      } /* end case -  */
      break;
    case SW2_MORE_DATA_AVAILABLE:         /* More Data Available */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_MORE_DATA_AVAILABLE;
      } /* end case -  */
      break;
#endif /* FEATURE_UIM_UICC */
    default:
      {
        if ((sw2 & 0xF0) == (int)SW2_CMD_SUCCESS_INT_X_RETRIES)
        {
           /* Indicate APDU result */
           resp_buf[uim_drv_slot]->cmd_status = UIM_CMD_SUCCESS_WITH_X_RETRIES;
        }
        else
        {
           /* Indicate APDU result */
           resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

           /* Reset the receiver and leave it disabled.  This will ignore any
              more characters received from the UIM.        */
           UIM_RESET_RX ();
           /* Should I tell the UIM server of this failure? */
        }
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

} /* end rx_isr_sw1_warnings2_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_EXECN_ERR2_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_execn_err2_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

    /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  switch (sw2)                 /* Process the second status word */
  {
    case SW2_NV_STATE_CHANGED:          /* No info with NV changed */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_EXEC_ERR_AND_NV_CHANGED;
      } /* end case - SW2_NV_STATE_CHANGED */
      break;

    case SW2_MEMORY_PROBLEM:            /* Memory problem  */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_EXEC_ERR_MEMORY_PROBLEM;
      } /* end case -  */
      break;

    default:
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

} /* end rx_isr_sw1_execn_err2_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_CMD_NOT_ALLOWED_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_cmd_not_allowed_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

    /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  switch (sw2)                 /* Process the second status word */
  {
    case SW2_NO_INFO_GIVEN:          /* No info given  */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_CMD_ERR_NO_INFO;
      } /* end case - SW2_NO_INFO_GIVEN */
      break;

    case SW2_FILE_STRUCT_INCOMPATIBLE:  /* Cmd incompatible with file struct */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status =
          UIM_CMD_INCOMPATIBLE_WITH_FILE_STRUCT;
      } /* end case -  SW2_FILE_STRUCT_INCOMPATIBLE */
      break;


    case SW2_SECURITY_NOT_SATISFIED:     /* Security status not satisifed */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_SEC_STATUS_NOT_SATISFIED;
      } /* end case - SW2_SECURITY_NOT_SATISFIED  */
      break;

    case SW2_PIN_BLOCKED:     /* Authentication/PIN method blocked */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_AUTH_PIN_BLOCKED;
      } /* end case - UIM_SEC_STATUS_NOT_SATISFIED */
      break;

    case SW2_REF_DATA_INVALIDATED:     /* Referenced data invalidated */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_REF_DATA_INVALIDATED;
      } /* end case - SW2_REF_DATA_INVALIDATED */
      break;

    case SW2_CONDITIONS_NOT_SATISFIED:  /* Conditions of used not satisfied */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_USE_CONDNS_NOT_SATISFIED;
      } /* end case - SW2_CONDITIONS_NOT_SATISFIED */
      break;

    case SW2_NO_EF_SELECTED:     /* Cmd not allowed - No EF selected */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_CMD_ERR_NO_EF_SELECTED;
      } /* end case - SW2_NO_EF_SELECTED */
      break;

    case SW2_UNDEF_99:
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_WRONG_CLASS;
      } /* end case - SW2_UNDEF_99 */
      break;

    default:
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

} /* end rx_isr_sw1_cmd_not_allowed_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_WRONG_PARAMS_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_wrong_params_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

    /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  switch (sw2)                 /* Process the second status word */
  {
    case SW2_BAD_PARAMS_IN_DATA_FIELD:  /* Incorrect params in the data field */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_INCORRECT_PARAM_IN_DATA_FIELD;
      } /* end case - SW2_BAD_PARAMS_IN_DATA_FIELD */
      break;

    case SW2_FUNCTION_NOT_SUPPORTED:  /* Function not supported */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_FUNCTION_NOT_SUPPORTED;
      } /* end case - SW2_FUNCTION_NOT_SUPPORTED */
      break;

    case SW2_FILE_NOT_FOUND:     /* File not found */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_FILE_NOT_FOUND;
      } /* end case - SW2_FILE_NOT_FOUND */
      break;

    case SW2_RECORD_NOT_FOUND:     /* Record not found */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_RECORD_NOT_FOUND;
      } /* end case - SW2_RECORD_NOT_FOUND */
      break;

    case SW2_INCONSISTENT_LC:     /* Lc inconsistent with TLV structure */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_LC_INCONSISITENT_WITH_TLV;
      } /* end case - SW2_INCONSISTENT_LC */
      break;

    case SW2_BAD_PARAMS_P1_P2:  /* Incorrect parameters P1_P2 */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_INCORRECT_PARAMS_P1_P2;
      } /* end case - SW2_BAD_PARAMS_P1_P2 */
      break;

    case SW2_LC_INCONSISTENT_WITH_P1_P2:  /* Lc inconsistent with P1 and P2 */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_LC_INCONSISITENT_WITH_P1_P2;
      } /* end case - SW2_LC_INCONSISTENT_WITH_P1_P2 */
      break;

    case SW2_REF_DATA_NOT_FOUND:  /* Referenced data not found */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_REF_DATA_NOT_FOUND;
      } /* end case - SW2_REF_DATA_NOT_FOUND */
      break;

    case SW2_NOT_ENOUGH_MEMORY_SPACE: /* Not Enough Memory Space */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_NOT_ENOUGH_MEMORY_SPACE;
      } /* end case - SW2_NOT_ENOUGH_MEMORY_SPACE */
      break;

    default:
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

} /* end rx_isr_sw1_wrong_params_dm */

#ifdef FEATURE_UIM_WIM
/*===========================================================================

FUNCTION RX_ISR_SW1_SE_FAIL_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_se_fail_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

    /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Set the number of bytes received to zero. */
  resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

  /* Indicate APDU result */
  resp_buf[uim_drv_slot]->cmd_status = UIM_SE_NOT_SET;

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

} /* end rx_isr_sw1_se_fail_dm */
#endif /* FEATURE_UIM_WIM */

/*===========================================================================

FUNCTION RX_ISR_SW1_EXECN_ERR1_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_execn_err1_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{
    /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  switch (sw2)                 /* Process the second status word */
  {
    case SW2_NV_STATE_UNCHANGED:          /* No info with NV changed */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_EXEC_ERR_AND_NV_UNCHANGED;
      } /* end case - SW2_NV_STATE_UNCHANGED */
      break;

    default:
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_sw1_execn_err1_dm */

/*===========================================================================

FUNCTION RX_ISR_SW1_CLA_NOT_SUPPOSED_DM                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS


===========================================================================*/
void rx_isr_sw1_cla_not_supported_dm
(
  uim_drv_slot_type uim_drv_slot,  /* Slot control variable */
  byte              sw2
)
{

    /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  switch (sw2)                 /* Process the second status word */
  {
    case SW2_NO_INFO_GIVEN:          /* No info given  */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_CLA_ERR_NO_INFO;
      } /* end case - No info given */
      break;

    case SW2_LOGICAL_CHAN_NOT_SUPPORTED:   /* Logical channel not supported */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_LOGICAL_CHAN_NOT_SUP;
      } /* end case - SW2_LOGICAL_CHAN_NOT_SUPPORTED */
      break;


    case SW2_SECURE_MSG_NOT_SUPPORTED:     /* Secure messaging not supported */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_SECURE_MESSAGING_NOT_SUP;
      } /* end case -  SW2_SECURE_MSG_NOT_SUPPORTED */
      break;

    default:
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

} /* end rx_isr_sw1_cla_not_supported_dm */

/*===========================================================================

FUNCTION RX_ISR_PROC_PROCEDURE_BYTES_DM                     INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

  This state processes all the instruction cases.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming bytes to be status words.  This function
  can send another APDU based on the status words received.  Or this function
  will notify the UIM server when the APDU transfer has completed.

===========================================================================*/
LOCAL void rx_isr_proc_procedure_bytes_dm
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  dword uart_status;  /* Holds the UART status */
  byte sw1 = 0;           /* Holds the first status word */
  byte sw2 = 0;           /* Holds the second status word */

  byte result;       /* This holds the result of the ACK byte xored with the
                        INS byte                                             */

  uint32 rx_word;
  byte  rx_value_array[4];
  int i = 0; /* Running index */
  /* A temporary variable to read the bytes in one RX transfer */
  int32 bytes_to_read = 0;

  /* A boolean to indicate stale received */
  boolean stale_rcvd = FALSE;

  /* Boolean to indicate if we need to re-initialize the RX data transfer */
  boolean reinit_rxtransfer = TRUE;

  /* NOTE: There is a scope to reduce the RXLEV interrupt latency here. */

  /* Wait for TX_READY event to get confirmation that the last APDU Cmd byte was
     sent */
  if ( (uim_rx_sub_state == UIM_RX_WAIT_FOR_TXREADY) &&
       (UIM_READ_MISR() & MSMU_ISR_TX_READY) )
  {
    /* Clear the TX_READY interrupt */
    UIM_CLEAR_TX_READY_INT();

    /* There is no need to turn ON the WWT timer here as we do that
       in uim_rx_isr() */

    return;
  }

  if (UIM_READ_MISR() & MSMU_IMR_RXLEV)
  {
    /* Increment the RXLEV interrupt count */
    uim_rxlev_count += 1;
  }
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  ///////////////////////////////////////////////////////

  /* Note that RXSTALE and RXLEV can happen together
     any time. Hence, give preference to RXSTALE. */
  if(UIM_READ_MISR() & MSMU_IMR_RXSTALE)
  {
    stale_rcvd = TRUE;

    /* Reset the RX count when stale happens */
    num_bytes_in_this_rx_transfer[uim_drv_slot] = 0;

    /* Clear the RX STALE interrupt */
    UIM_CLEAR_RX_STALE_INT();

    /* Read only the unread bytes from the UART_DM */
    bytes_to_read = UIM_GET_NUM_BYTES_IN_RX() - num_bytes_received;

    /* Error indication if bytes to read becomes negative somehow */
    if(bytes_to_read < 0)
    {
      MSG_HIGH("Error: Bytes to read cannot be negative",0,0,0);
    }

    num_bytes_received = 0;
  }
  else if(UIM_READ_MISR() & MSMU_IMR_RXLEV)
  {
    num_bytes_received += (4 * MSMU_DEF_RFWR_VAL);
    bytes_to_read = 4 * MSMU_DEF_RFWR_VAL;

    /* Reset the RX count when RXLEV happens */
    num_bytes_in_this_rx_transfer[uim_drv_slot] = 0;
  }


  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Make sure there is a word in the receive FIFO */
  for (uart_status = UIM_READ_STATUS();
      (uart_status & MSMU_SR_RXRDY) && (bytes_to_read > 0) ;
     uart_status = UIM_READ_STATUS())
  {
    if(bytes_to_read >0)
    {
      rx_word  = UIM_GET_RX_WORD();
      rx_value_array[0] = rx_word & 0xFF;
      rx_value_array[1] = (rx_word & 0xFF00)     >> 8;
      rx_value_array[2] = (rx_word & 0xFF0000)   >> 16;
      rx_value_array[3] = (rx_word & 0xFF000000) >> 24;

      /* Process each byte in the received word */
      for ( i = 0; (i < 4) && (bytes_to_read > 0) && (uart_status & MSMU_SR_RXRDY); i++)
      {
#ifdef FEATURE_UIM_DEBUG_LOG
        UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, rx_value_array[i]);
#endif /* FEATURE_UIM_DEBUG_LOG */

        bytes_to_read -= 1;

        /* The GCF 27.11.1.5 testcase generates bytes with parity
           errors in a pattern. Every 1st, 4th, 7th, 10th, 13th and so on,
           bytes are received with parity error. Therefore, the workaround for
           just GCF 27.11.1.5 testcase basically ignores the bytes with parity
           error as the UART_DM captures them in the RX FIFO. */
        if (TRUE == uim_par_err_workaround)
        {
          /* Ignore 1st, 4th, 7th,..., bytes following the pattern */
          if(uim_gcf_byte_count % 3 == 0)
          {
            /* Increment the byte count */
            uim_gcf_byte_count +=1;
            /* Ignore the byte */
            continue;
          }

          /* Increment the byte count */
          uim_gcf_byte_count +=1;
        }

        /* The data that is being received is the expected data from the SIM */
        if(resp_bytes_being_rcvd == TRUE)
        {
          /* Place the byte in the response buffer. */
          *resp_buffer_ptr[uim_drv_slot]++ = rx_value_array[i];
          num_resp_bytes_rcvd_total[uim_drv_slot]++;

          /* Keep track of the number of bytes received. */
          num_resp_bytes_rcvd[uim_drv_slot]++;

          reinit_rxtransfer = TRUE;

          /* Determine if all the expected bytes have been received. */
          if (num_resp_bytes_rcvd[uim_drv_slot] ==
              num_resp_data_bytes_expected[uim_drv_slot])
          {
            resp_bytes_being_rcvd = FALSE;
          }
          continue;
        }

        /* Check if we have received the SW1 status byte */
        if(sw1_byte_rcvd == FALSE)
        {
          /* Check if the TX state is set to process the command header */
          if (uim_tx_state[uim_drv_slot] == UIM_TX_SEND_CMD_HDR)
          {
            /* Save the INS byte */
            ins_byte[uim_drv_slot] = *(request_buffer_ptr[uim_drv_slot] + 1);

            /* Set up the data to send just in case there is data to send. */
            num_bytes_to_send[uim_drv_slot] =
                      num_cmd_data_bytes_to_send[uim_drv_slot];
            request_buffer_ptr[uim_drv_slot] =
                      apdu_command_data_ptr[uim_drv_slot];

            /* Put the transmit state to CMD data */
            uim_tx_state[uim_drv_slot] = UIM_TX_SND_CMD_DATA;
          }

          /* Save the status word in case it is a true status word response */
          resp_buf[uim_drv_slot]->sw1 = rx_value_array[i];
          sw1 = resp_buf[uim_drv_slot]->sw1;

          /* Determine what the first status word and process accordingly */
          switch (sw1)
          {
            case SW1_NULL:          /* NULL procedure byte */
              /* This byte is sent by the UIM to keep the ME from the work waiting
                 time timeout.  We handle UIM interaction timeouts at a higher
                 layer.  Nothing needs to be done for this byte.  We are waiting
                 for another procedure byte. */
              /* NULL procedure byte is not a status byte */
              sw1_byte_rcvd = FALSE;
              /* set the flag to re-initialize a new RX transfer if this is the last
              byte received in the current RX transfer */
              reinit_rxtransfer = TRUE;
              break;

            case SW1_NORMAL_END:    /* Normal ending of the command */
            case SW1_END_FETCH:     /* Normal ending of the command with extra
                                       command                            */
      #ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
            case SW1_DLOAD_ERR:     /* SIM data download error */
      #endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

            case SW1_END_RESP:      /* Normal ending of the command with a rsp */
            case SW1_BUSY:          /* SIM App toolkit is busy */
            case SW1_END_RETRY:     /* Command successful with update retries or
                                       memory error                       */
            case SW1_REFERENCE:     /* A reference management problem */
            case SW1_SECURITY:      /* A security problem */
            case SW1_P3_BAD:        /* Incorrect parameter P3 */
            case SW1_P1_P2_BAD:     /* Incorrect parameter P1 or P2 */
            case SW1_INS_BAD:       /* Unknown instruction code */
            case SW1_CLA_BAD:       /* Wrong instruction class given in command */
            case SW1_PROBLEM:       /* Technical problem; no diagnostic given */

      #if defined (FEATURE_UIM_UTIL) && !defined(FEATURE_UIM_UICC)
            case SW1_UTIL_END_RESP:
      #endif /* FEATURE_UIM_UTIL */
      #ifdef FEATURE_UIM_UICC
            case SW1_USIM_END_RESP:
      #endif /* FEATURE_UIM_UICC */
      #if defined(FEATURE_UIM_UICC) || defined(FEATURE_UIM_UTIL)

      #if defined (FEATURE_UIM_UTIL) && !defined(FEATURE_UIM_UICC)
            case SW1_LENGTH_ERROR:
      #endif /* FEATURE_UIM_UTIL */
      #ifdef FEATURE_UIM_UICC
            case SW1_USIM_RESEND_APDU:
      #endif /* FEATURE_UIM_UICC */

            case SW1_WARNINGS1:
            case SW1_WARNINGS2:
            case SW1_EXECUTION_ERR2:
            case SW1_CMD_NOT_ALLOWED:
            case SW1_WRONG_PARAMS:

      #endif /* FEATURE_UIM_UICC || FEATURE_UIM_UTIL */

      #ifdef FEATURE_UIM_WIM
            case SW1_SE_FAIL:
      #endif /* FEATURE_UIM_WIM */

      #ifdef FEATURE_UIM_UICC

            case SW1_EXECUTION_ERR1:
            case SW1_CLA_NOT_SUPPORTED:
      #endif /* FEATURE_UIM_UICC */
              {
                /* We have received the SW1 status byte */
                sw1_byte_rcvd = TRUE;
                /* set the flag to re-initialize a new RX transfer if this is the last
                byte received in the current RX transfer */
                reinit_rxtransfer = TRUE;
              }
            break;

            default:
              {
                /* This may be an ACK byte */
                /* The next operation is dependent on the value of the Ack byte
                   with respect to the INS of the APDU.  The action is determined
                   by XORing the Ack byte with the INS                    */
                result = sw1 ^ ins_byte[uim_drv_slot];

                /* Determine what to do next */
                switch (result)
                {
                  case PAUSE_REMAINING_BYTES:   /* Send the remaining bytes of the
                                                   data                            */
                  case PROG_REMAINING_BYTES:    /* Send the remaining bytes of the
                                                   data                            */
                    {
                      /* Determine if command data bytes are to be sent */
                      if ( (num_cmd_data_bytes_to_send[uim_drv_slot] > 0) &&
                           (num_bytes_to_send[uim_drv_slot]  > 0) )
                      {

#ifdef FEATURE_UIM_DEBUG_LOG
                        UIM_LOG_PUT_LAST_BYTE();
#endif /* FEATURE_UIM_DEBUG_LOG */

                        /* Change the Rx ISR state to send the command data bytes */
                        uim_tx_state[uim_drv_slot] = UIM_TX_SND_CMD_DATA;

                        /* Indicate the remaining bytes are to be sent */
                        send_all_remaining_bytes[uim_drv_slot] = TRUE;

                        /* set the flag to re-initialize a new RX transfer if this is the last
                        byte received in the current RX transfer */
                        reinit_rxtransfer = TRUE;

                        /* Wait long enough before responding to procedure byte */
                        uim_clk_busy_wait( uim_response_byte_delay );

                        if(num_resp_data_bytes_expected[uim_drv_slot] == 0)
                        {
                          /* set the rx state to wait for procedure bytes */
                          /* We are entering in to UIM_RX_PROC_PRCDR_BYTES since we are expecting only
                             status words */
                          uim_rx_state[uim_drv_slot] = UIM_RX_PROC_PRCDR_BYTES;

                          /* set the flag to re-initialize a new RX transfer if this is the last
                          byte received in the current RX transfer */
                          reinit_rxtransfer = TRUE;

                          /* Intialize the byte count */
                          if (uim_par_err_workaround == TRUE)
                          {
                            uim_gcf_byte_count = 0;
                          }

                          /* Enable TX READY, RX STALE and RX BRK interrupts */
                          UIM_PRG_IMR ( (MSMU_IMR_TX_READY | MSMU_IMR_RXSTALE
                                         | MSMU_IMR_RXBREAK) );

                          /* Wait for TXREADY event to make sure the APDU
                             command bytes have been seen successfully */
                          uim_rx_sub_state = UIM_RX_WAIT_FOR_TXREADY;

                          /* Program the number of bytes to be sent as APDU command header */
                          UIM_NUM_CHARS_TO_SEND(num_bytes_to_send[uim_drv_slot]);

                          num_cmd_data_bytes_to_send[uim_drv_slot] =
                                 num_cmd_data_bytes_to_send[uim_drv_slot] - num_bytes_to_send[uim_drv_slot];

                          /* Clear the work waiting timer when transmitting bytes */
                          (void) rex_clr_timer( &uim_cmd_rsp_timer);

                          /* Write the words to the UART */
                          uim_write_to_uart(request_buffer_ptr[uim_drv_slot], num_bytes_to_send[uim_drv_slot]);

                          num_bytes_to_send[uim_drv_slot] = 0;
                          num_cmd_data_bytes_to_send[uim_drv_slot] = 0;

                        }
                        else
                        {
                          // This condition is not possible
                        }
                      }
                      /* Determine if a response is expected */
                      else if (num_resp_data_bytes_expected[uim_drv_slot] != 0)
                      {
                        /* Now we are getting the response data from the SIM */
                        resp_bytes_being_rcvd = TRUE;
                        /* set the flag to re-initialize a new RX transfer if this is the last
                        byte received in the current RX transfer */
                        reinit_rxtransfer = TRUE;
                      }
                      else /* Neither command data to send nor response expected
                              AND we are processing an ACK.  Something went wrong  */
                      {
                        /* Indicate the task to ignore the following timeout */
                        uim_bad_status_words_error = TRUE;

                        /* Set the number of bytes received to zero. */
                        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

                        /* Give a bad status. */
                        resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

                        /* Reset the UART to starting the incoming bytes and as well will
                           start ignoring the incoming bytes from the UIM
                           and also flush the bytes in the RX FIFO */
                        uim_reset_receiver();

                       /* Call the response callback function */
                       apdu_resp_callback_func[uim_drv_slot]();

                       uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

                        return;
                      } /* end if - there are command bytes to send */
                    } /* end case - REMAINING_BYTES */
                    break;

                  case PAUSE_NEXT_BYTE:         /* Send only the next byte and wait
                                                   for Ack                         */
                  case PROG_NEXT_BYTE:          /* Send only the next byte and wait
                                                   for Ack                         */
                    {
                      /* Determine if command data bytes are to be sent */
                      if ( (num_cmd_data_bytes_to_send[uim_drv_slot] > 0) &&
                           (num_bytes_to_send[uim_drv_slot]  > 0) )
                      {

#ifdef FEATURE_UIM_DEBUG_LOG
                        UIM_LOG_PUT_LAST_BYTE();
#endif /* FEATURE_UIM_DEBUG_LOG */

                        /* Change the Rx ISR state to send the command data bytes */
                        uim_tx_state[uim_drv_slot] = UIM_TX_SND_CMD_DATA;

                        /* Indicate only send one byte at a time */
                        send_all_remaining_bytes[uim_drv_slot] = FALSE;

                        /* set the flag to re-initialize a new RX transfer if this is the last
                        byte received in the current RX transfer */
                        reinit_rxtransfer = TRUE;

                        /* set the rx state to wait for procedure bytes */
                        uim_rx_state[uim_drv_slot] = UIM_RX_PROC_PRCDR_BYTES;

                        /* Initialize the byte count */
                        if (uim_par_err_workaround == TRUE)
                        {
                          uim_gcf_byte_count = 0;
                        }

                        /* Enable TX READY, RX STALE and RX BRK interrupts */
                        UIM_PRG_IMR ( (MSMU_IMR_TX_READY | MSMU_IMR_RXSTALE | MSMU_IMR_RXBREAK) );

                        /* Wait for TXREADY event to make sure the APDU command bytes have been seen successfully */
                        uim_rx_sub_state = UIM_RX_WAIT_FOR_TXREADY;

                        /* Program the number of bytes to be sent as APDU command header */
                        UIM_NUM_CHARS_TO_SEND(1);

                        /* Clear the work waiting timer when transmitting bytes */
                        (void) rex_clr_timer( &uim_cmd_rsp_timer);

                        /* Write the words to the UART */
                        uim_write_to_uart(request_buffer_ptr[uim_drv_slot], 1);

                        /* Keep track of the number of bytes to send */
                        --num_bytes_to_send[uim_drv_slot];

                        /* Send the next byte */
                        request_buffer_ptr[uim_drv_slot]++;

                      }
                      /* Determine if a response is expected */
                      else if (num_resp_data_bytes_expected[uim_drv_slot] != 0)
                      {
                        // do nothing
                      }
                      else /* Neither command data to send nor response expected
                              AND we are processing an ACK.  Something went wrong  */
                      {
                        /* Indicate the task to ignore the following timeout */
                        uim_bad_status_words_error = TRUE;

                        /* Set the number of bytes received to zero. */
                        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

                        /* Give a bad status. */
                        resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

                        /* Reset the UART to starting the incoming bytes and as well will
                           start ignoring the incoming bytes from the UIM
                           and also flush the bytes in the RX FIFO */
                        uim_reset_receiver();

                       /* Call the response callback function */
                       apdu_resp_callback_func[uim_drv_slot]();

                       uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

                        return;
                      } /* end if - there are command bytes to send */
                    } /* end case - NEXT_BYTE */
                    break;

                  default:
                    {
                      /* Indicate the task to ignore the following timeout */
                      uim_bad_status_words_error = TRUE;

                      /* Set the number of bytes received to zero. */
                      resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

                      /* Give a bad status. */
                      resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

                        /* Reset the UART to starting the incoming bytes and as well will
                           start ignoring the incoming bytes from the UIM
                           and also flush the bytes in the RX FIFO */
                        uim_reset_receiver();

                       /* Call the response callback function */
                       apdu_resp_callback_func[uim_drv_slot]();

                       uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

                      return;
                      /* Should I tell the UIM server of this failure? */
                    } /* end case - default */
                } /* end switch - result of Ack */
              } /* end case - default */
              break;
          } /* end switch - the first status word */
        }
        else
        {
          /* No more reinit required as we got the SW2 status byte indicating
          the response from the SIM is completed */
          reinit_rxtransfer = FALSE;

#ifdef FEATURE_UIM_DEBUG_LOG
          UIM_LOG_PUT_LAST_BYTE();
#endif /* FEATURE_UIM_DEBUG_LOG */

          /* We got the complete response. Stop processing any more data if padded by
             the UART_DM */
          bytes_to_read = -1;

          /* Check if Stale happened */
          if (UIM_READ_MISR() & MSMU_IMR_RXSTALE)
          {
            /* Clear the stale interrupt */
            UIM_CLEAR_RX_STALE_INT();
          }

          /* Disable RXLEV and RXSTALE */
          UIM_PRG_IMR ( MSMU_IMR_RXBREAK );

          /* SW2 status byte is received */
          sw2_byte_rcvd = TRUE;

          sw2 = rx_value_array[i];

          /* Determine what the first status word and process accordingly */
          switch (resp_buf[uim_drv_slot]->sw1)
          {
            case SW1_NORMAL_END:    /* Normal ending of the command */
              {
                rx_isr_sw1_normal_end_dm(uim_drv_slot, sw2);
              } /* end case - SW1_NORMAL_END */
              break;

            case SW1_END_FETCH:     /* Normal ending of the command with extra
                                       command                            */
              {
                rx_isr_sw1_fetch_end_dm(uim_drv_slot, sw2);
              } /* end case - SW1_END_FETCH */
              break;

      #ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
            case SW1_DLOAD_ERR:     /* SIM data download error */
              {
                rx_isr_sw1_dload_err_dm(uim_drv_slot, sw2);
              } /* end case - SW1_DLOAD_ERR */
              break;
      #endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

            case SW1_END_RESP:      /* Normal ending of the command with a rsp */
              {
                uim_get_resp_sw1 = SW1_END_RESP;
                rx_isr_sw1_resp_end_dm(uim_drv_slot, sw2);
              } /* end case - SW1_END_RESP */
              break;

            case SW1_BUSY:          /* SIM App toolkit is busy */
              {
                rx_isr_sw1_busy_dm(uim_drv_slot, sw2);
              } /* end case - SW1_BUSY */
              break;

            case SW1_END_RETRY:     /* Command successful with update retries or
                                       memory error                       */
              {
                rx_isr_sw1_retries_dm(uim_drv_slot, sw2);
              } /* end case - SW1_END_RETRY */
              break;

            case SW1_REFERENCE:     /* A reference management problem */
              {
                rx_isr_sw1_reference_dm(uim_drv_slot, sw2);
              } /* end case - SW1_REFERENCE */
              break;

            case SW1_SECURITY:      /* A security problem */
              {
                rx_isr_sw1_security_dm(uim_drv_slot, sw2);
              } /* end case - SW1_SECURITY */
              break;

            case SW1_P3_BAD:        /* Incorrect parameter P3 */
              {
                rx_isr_sw1_p3_bad_dm(uim_drv_slot, sw2);
              } /* end case - SW1_P3_BAD */
              break;

            case SW1_P1_P2_BAD:     /* Incorrect parameter P1 or P2 */
              {
                rx_isr_sw1_p1_p2_bad_dm(uim_drv_slot, sw2);
              } /* end case - SW1_P1_P2_BAD */
              break;

            case SW1_INS_BAD:       /* Unknown instruction code */
              {
                rx_isr_sw1_ins_bad_dm(uim_drv_slot, sw2);
              } /* end case - SW1_INS_BAD */
              break;

            case SW1_CLA_BAD:       /* Wrong instruction class given in command */
              {

                /* Save the status word in case it is a true status word response */
                resp_buf[uim_drv_slot]->sw2 = sw2;

                /* Set the number of bytes received to zero. */
                resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

                /* Indicate APDU result */
                resp_buf[uim_drv_slot]->cmd_status = UIM_WRONG_CLASS;

                /* Send an indication the APDU completed with an error. */
                UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

                /* Call the response callback function */
                apdu_resp_callback_func[uim_drv_slot]();

                #ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
                  /* This flag is processed in the Normal end SW processing.
                     It distinguishes between data download error and all other
                     results. */
                  processing_download_error[uim_drv_slot] = FALSE;
                #endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

                /* Change to idle */
                uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
              } /* end case - SW1_CLA_BAD */
              break;

            case SW1_PROBLEM:       /* Technical problem; no diagnostic given */
              {
                rx_isr_sw1_problem_dm ( uim_drv_slot, sw2 );
              } /* end case - SW1_PROBLEM */
              break;

      #if defined (FEATURE_UIM_UTIL) && !defined(FEATURE_UIM_UICC)
            case SW1_UTIL_END_RESP:
      #endif /* FEATURE_UIM_UTIL */
      #ifdef FEATURE_UIM_UICC
            case SW1_USIM_END_RESP:
      #endif /* FEATURE_UIM_UICC */
      #if defined(FEATURE_UIM_UICC) || defined(FEATURE_UIM_UTIL)
              {
                uim_get_resp_sw1 = SW1_USIM_END_RESP;
                rx_isr_sw1_resp_end_dm ( uim_drv_slot, sw2 );
              } /* end case - SW1_USIM_END_RESP */
              break;

      #if defined (FEATURE_UIM_UTIL) && !defined(FEATURE_UIM_UICC)
            case SW1_LENGTH_ERROR:
      #endif /* FEATURE_UIM_UTIL */
      #ifdef FEATURE_UIM_UICC
            case SW1_USIM_RESEND_APDU:
      #endif /* FEATURE_UIM_UICC */
              {
                rx_isr_sw1_resend_apdu_dm ( uim_drv_slot, sw2 );
              } /* end case - SW1_USIM_RESEND_APDU */
              break;

            case SW1_WARNINGS1:
              {
                  /* Update the flag to indicate that WARNINGS2 status 
                     is returned by the SIM */
                    uim_warning1_indicator = TRUE;
                  uim_rx_state[uim_drv_slot] = UIM_RX_SW1_WARNINGS1;
                  if ( (uim_last_instrn_case[uim_drv_slot] == UIM_INSTRN_CASE_4 ) &&
                       (last_sent_apdu[uim_drv_slot].instrn != UIM_GENERATE_ASY_KEY_PAIR ) &&
                       uim_check_to_do_get_response_if_seek())
                  {
                    uim_sw1_warnings1_normal_end_occured = TRUE;

                    uim_force_get_response = TRUE;
                    uim_get_resp_sw1 = SW1_END_RESP;
                    rx_isr_sw1_resp_end_dm ( uim_drv_slot, sw2 );
                    uim_force_get_response = FALSE;
                  }
                  else
                  {
                    rx_isr_sw1_warnings1_dm ( uim_drv_slot, sw2 );

                  }
              } /* end case - SW1_WARNINGS1 */
              break;

            case SW1_WARNINGS2:
              {
                  /* Update the flag to indicate that WARNINGS2 status 
                     is returned by the SIM */
                  uim_warning2_indicator = TRUE;
                  uim_rx_state[uim_drv_slot] = UIM_RX_SW1_WARNINGS2;
                  /* Let us consider this as a normal response
                   * but only with p3 = 0 for non GSM case-4 other than
                   * GENERATE_ASY_KEY_PAIR WIM instruction
                   */
                  if ( (uim_last_instrn_case[uim_drv_slot] == UIM_INSTRN_CASE_4 ) &&
                       (last_sent_apdu[uim_drv_slot].instrn != UIM_GENERATE_ASY_KEY_PAIR ) &&
                       uim_check_to_do_get_response_if_seek())
                  {

                    uim_force_get_response = TRUE;
                    uim_get_resp_sw1 = SW1_END_RESP;
                    rx_isr_sw1_resp_end_dm ( uim_drv_slot, sw2 );
                    uim_force_get_response = FALSE;
                  }
                  else
                  {
                    rx_isr_sw1_warnings2_dm ( uim_drv_slot, sw2 );
                  }
              } /* end case - SW1_WARNINGS2 */
              break;

            case SW1_EXECUTION_ERR2:
              {
                rx_isr_sw1_execn_err2_dm ( uim_drv_slot, sw2 );
              } /* end case - SW1_EXECUTION_ERR2 */
              break;

            case SW1_CMD_NOT_ALLOWED:
              {
                rx_isr_sw1_cmd_not_allowed_dm ( uim_drv_slot, sw2 );
              } /* end case - SW1_CMD_NOT_ALLOWED */
              break;

            case SW1_WRONG_PARAMS:
              {
                rx_isr_sw1_wrong_params_dm ( uim_drv_slot, sw2 );
              } /* end case - SW1_WRONG_PARAMS */
              break;

      #endif /* FEATURE_UIM_UICC || FEATURE_UIM_UTIL */

      #ifdef FEATURE_UIM_WIM
            case SW1_SE_FAIL:
              {
                rx_isr_sw1_se_fail_dm ( uim_drv_slot, sw2 );
              } /* end case - SW1_USIM_END_RESP */
              break;
      #endif /* FEATURE_UIM_WIM */

      #ifdef FEATURE_UIM_UICC

            case SW1_EXECUTION_ERR1:
              {
                rx_isr_sw1_execn_err1_dm ( uim_drv_slot, sw2 );
              } /* end case - SW1_EXECUTION_ERR1 */
              break;

            case SW1_CLA_NOT_SUPPORTED:
              {
                rx_isr_sw1_cla_not_supported_dm ( uim_drv_slot, sw2 );
              } /* end case - SW1_CLA_NOT_SUPPORTED */
              break;

      #endif /* FEATURE_UIM_UICC */

          default:
              reinit_rxtransfer = FALSE;
              sw1_byte_rcvd = FALSE;
              sw2_byte_rcvd = FALSE;

              MSG_ERROR ("Invalid SW1 status byte",0,0,0);
              break;
          }
        }
      }
    }
  }

  /* If end of data event occurred and still some more data expected */
  if( reinit_rxtransfer == TRUE )
  {
      reinit_rxtransfer = FALSE;

      /* This stale event might have happened while we were processing the
         RXLEV interrupt. In that case, we would like to preserve it. */
      if (UIM_READ_MISR() & MSMU_IMR_RXSTALE)
      {
        /* Return from the ISR to process this RXSTALE */
        return;
      }

      if ( (TRUE == stale_rcvd) ||
           /* A case when RXSTALE did not happened in this condition when
              we had continuous RXLEVs happening and for the remaining data
              whose size is less than the RX watermark level the RXSTALE
              did not happen. Let us revisit this again with a better way of
              handling this condition. */
           (uim_rxlev_count == ( (num_resp_data_bytes_expected[uim_drv_slot] + 1 + 2)/4 ) )
         )
      {

        /* Since we are going to re-init a new RX data transfer
           initialize this variable to 0. */
        num_bytes_received = 0;

        /* Re-initialize a new RX data transfer */
        UIM_DM_RX_INIT(MSMU_DEF_RX_INIT_VAL);

        /* Enable Stale event */
        UIM_ENABLE_RX_STALE_INT();
      }
  }

  return;
} /* end rx_isr_proc_procedure_bytes_dm */

#endif /* FEATURE_UIM_UART_DM */

/*===========================================================================

FUNCTION RX_ISR_PROCESS_RESP_DATA                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting response data from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after sending out the APDU
  command header AND data is to be received from the UIM.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming bytes to be an APDU response.  This
  function expects to receive the selected number of response bytes.

===========================================================================*/
LOCAL void rx_isr_process_resp_data
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  dword uart_status;  /* Holds the UART status */

  /* This holds the byte read from the receive FIFO */
  byte rx_value;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Make sure there is a byte in the receive FIFO */
  for (uart_status = UIM_READ_STATUS();
       (uart_status & MSMU_SR_RXRDY);
       uart_status = UIM_READ_STATUS())
  {
    /* Get the next received byte. */
    rx_value = UIM_GET_RX_BYTE ();
#ifdef FEATURE_UIM_DEBUG_LOG
    UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, rx_value);
#endif

    /* Make sure there is no problem with the received byte */
    if (uart_status & MSMU_SR_PF_ERR)
    {
      UIM_PROCESS_PARITY_ERROR_CONT( );
    } /* end if - check for parity error. */

    /* Place the byte in the response buffer. */
    *resp_buffer_ptr[uim_drv_slot]++ = rx_value;
    num_resp_bytes_rcvd_total[uim_drv_slot]++;

    /* Keep track of the number of bytes received. */
    num_resp_bytes_rcvd[uim_drv_slot]++;

    /* Determine if all the expected bytes have been received. */
    if (num_resp_bytes_rcvd[uim_drv_slot] ==
        num_resp_data_bytes_expected[uim_drv_slot])
    {
#if !defined ( UIM_NO_SIM_GLUE_LOGIC ) && !defined ( T_UIM_NO_RX_WATERLEVEL )
      /* Program the receive level to wait for procedure byte(s). */
      UIM_RX_WATERLEVEL ( UIM_RX_WATER_MARK_LEVEL_FIVE );
#endif /* !UIM_NO_SIM_GLUE_LOGIC */
      /* Change the Rx ISR state to wait for status words */
      uim_rx_state[uim_drv_slot] = UIM_RX_PROC_PRCDR_BYTES;

      /* Stop processing the receive fifo */
      return;
    } /* end if - all the expected bytes have been received */
  } /* end while - receive characters to process. */
} /* end rx_isr_process_resp_data */


/*===========================================================================

FUNCTION RX_ISR_PROCESS_OVERRUN_DATA                        INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when UART is overrun when the driver state is not idle.
  This function reads the bytes from the UART and flushes them.

DEPENDENCIES
  This function is called from the receive ISR when a receive overrun is
  detected.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
LOCAL void rx_isr_process_overrun_data
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  dword uart_status;  /* Holds the UART status */

#ifdef FEATURE_UIM_UART_DM
  volatile uint32 rx_value;
  uint32 i;
#else
  volatile byte rx_value;     /* Holds the received value read from the receive FIFO. */
#endif /* FEATURE_UIM_UART_DM */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Make sure there is a byte in the receive FIFO */
  for (uart_status = UIM_READ_STATUS();
       (uart_status & MSMU_SR_RXRDY);
       uart_status = UIM_READ_STATUS())
  {
#ifdef FEATURE_UIM_UART_DM
     /* do not care about how many bytes in the word are, just flush them*/
    rx_value = UIM_GET_RX_WORD();
#ifdef FEATURE_UIM_DEBUG_LOG
    for (i = 0;i < 4; i++)
    {
      UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, (byte)( (rx_value >> (i * 8)) & 0xFF) );
    }
#endif
#else
    /* Get the next received byte. */
    rx_value = UIM_GET_RX_BYTE();
#ifdef FEATURE_UIM_DEBUG_LOG
    UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, rx_value);
#endif /* FEATURE_UIM_DEBUG_LOG */
#endif /* FEATURE_UIM_UART_DM */
  } /* end while - receive characters to process. */
} /* end rx_isr_process_overrun_data */


/*===========================================================================

FUNCTION RX_ISR_PROC_PROCEDURE_BYTES                       INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting status words from the UIM.  The response
  comes as the result of an APDU.

DEPENDENCIES
  This function is called from the receive ISR after the data transfer of
  an APDU.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming bytes to be status words.  This function
  can send another APDU based on the status words received.  Or this function
  will notify the UIM server when the APDU transfer has completed.

===========================================================================*/
LOCAL void rx_isr_proc_procedure_bytes
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  dword uart_status;  /* Holds the UART status */
  byte sw1;          /* Holds the first status word */

  byte result;       /* This holds the result of the ACK byte xored with the
                        INS byte                                             */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Read the UART receive status */
  uart_status = UIM_READ_STATUS ();

  /* Get the next received byte. */
  sw1 = UIM_GET_RX_BYTE ();
#ifdef FEATURE_UIM_DEBUG_LOG
    UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw1);
#endif

  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw1 = sw1;

  if ((sw1 == (int)SW1_SIG_ERROR2) || (uart_status & MSMU_SR_PF_ERR))
  {
#if defined( UIM_NO_SIM_GLUE_LOGIC )
    /* The byte received looks like an error signal
       Re-send the last byte */
    uim_send_byte (*request_buffer_ptr[uim_drv_slot]);

#ifdef FEATURE_UIM_DEBUG_LOG
    UIM_LOG_PUT_BYTE(UIM_LOG_TX_REPEAT, *request_buffer_ptr[uim_drv_slot]);
#endif
#else
    ERR ("UIM received parity error",0,0,0);
    /* Ignore the byte as it has a parity error and the SIM glue logic
       will generate an error signal and cause a re-transmission */
    return;
#endif /* UIM_NO_SIM_GLUE_LOGIC */

  }
  /* The byte is NOT a error signal.  Skip the NULL procedure byte */
  else if ((int)SW1_NULL != sw1)
  {

    /* Process the byte based on the tx state */
    switch (uim_tx_state[uim_drv_slot])
    {
      case UIM_TX_SEND_PPS:  /* Process this byte as a PPS byte */
        {
          /* Process the rest of the bytes as a PPS response */
          uim_rx_state[uim_drv_slot] = UIM_RX_RECEIVE_PPS;

          /* Place the byte in the receive buffer. */
          *resp_buffer_ptr[uim_drv_slot]++ = sw1;

          /* Keep a rolling value of the check byte. */
          check_byte[uim_drv_slot] ^= sw1;

          /* Keep track of the number of bytes received. */
          num_resp_bytes_rcvd[uim_drv_slot]++;

          /* Another byte is processed.  Determine how many more bytes
             until this function needs to process a format byte.             */
          num_bytes_to_skip[uim_drv_slot]--;
        } /* end case - UIM_TX_SEND_PPS */
        /* Finished processing with this byte */
        return;

      case UIM_TX_SEND_CMD_HDR:
        {
          /* Save the INS byte */
          ins_byte[uim_drv_slot] = *(request_buffer_ptr[uim_drv_slot] - 3);

          /* Set up the data to send just in case there is data to send. */
          num_bytes_to_send[uim_drv_slot] =
            num_cmd_data_bytes_to_send[uim_drv_slot];
          request_buffer_ptr[uim_drv_slot] =
            apdu_command_data_ptr[uim_drv_slot];

        } /* end case - UIM_TX_SEND_CMD_HDR */
        break;

      case UIM_TX_SND_CMD_DATA:
        {
          /* Keep track of the number of bytes to send */
          --num_bytes_to_send[uim_drv_slot];

          /* Send the next byte */
          request_buffer_ptr[uim_drv_slot]++;
        } /* end case - UIM_TX_SND_CMD_DATA */
        break;

      default:
        {
          MSG_ERROR ("Invalid uim_tx_state",0,0,0);
          /* Something went wrong.  We got a bad uim_tx_state value
           * set the sate to UIM_RX_UNKNOWN_PROC_BYTE_RCVD this would ignore all
           * bytes untill a time-out and then re-try the command
           */
          uim_rx_state[uim_drv_slot] = UIM_RX_UNKNOWN_PROC_BYTE_RCVD;

          /* Set the number of bytes received to zero. */
          resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

          /* Give a bad status. */
          resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

          /* Indicate the task to ignore the following timeout */
          uim_bad_status_words_error = TRUE;

          return;
        } /* end case - default */
    } /* end switch - uim_tx_state */

    /* Determine what the first status word and process accordingly */
    switch (sw1)
    {
      case SW1_NULL:          /* NULL procedure byte */
        /* This byte is sent by the UIM to keep the ME from the work waiting
           time timeout.  We handle UIM interaction timeouts at a higher
           layer.  Nothing needs to be done for this byte.  We are waiting
           for another procedure byte. */
        break;

      case SW1_NORMAL_END:    /* Normal ending of the command */
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_NORM_END;
        } /* end case - SW1_NORMAL_END */
        break;

      case SW1_END_FETCH:     /* Normal ending of the command with extra
                                 command                            */
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_FETCH_END;
        } /* end case - SW1_END_FETCH */
        break;
#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
      case SW1_DLOAD_ERR:     /* SIM data download error */
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_DLOAD_ERR;
        } /* end case - SW1_DLOAD_ERR */
        break;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */
      case SW1_END_RESP:      /* Normal ending of the command with a rsp */
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_RESP_END;
        } /* end case - SW1_END_RESP */
        break;

      case SW1_BUSY:          /* SIM App toolkit is busy */
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_BUSY;
        } /* end case - SW1_BUSY */
        break;

      case SW1_END_RETRY:     /* Command successful with update retries or
                                 memory error                       */
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_RETRIES;
        } /* end case - SW1_END_RETRY */
        break;

      case SW1_REFERENCE:     /* A reference management problem */
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_REFERENCE;
        } /* end case - SW1_REFERENCE */
        break;

      case SW1_SECURITY:      /* A security problem */
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_SECURITY;
        } /* end case - SW1_SECURITY */
        break;

      case SW1_P3_BAD:        /* Incorrect parameter P3 */
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_P3_BAD;
        } /* end case - SW1_P3_BAD */
        break;

      case SW1_P1_P2_BAD:     /* Incorrect parameter P1 or P2 */
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_P1_P2_BAD;
        } /* end case - SW1_P1_P2_BAD */
        break;

      case SW1_INS_BAD:       /* Unknown instruction code */
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_INS_BAD;
        } /* end case - SW1_INS_BAD */
        break;

      case SW1_CLA_BAD:       /* Wrong instruction class given in command */
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_CLA_BAD;
        } /* end case - SW1_CLA_BAD */
        break;

      case SW1_PROBLEM:       /* Technical problem; no diagnostic given */
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_PROBLEM;
        } /* end case - SW1_PROBLEM */
        break;

#if defined (FEATURE_UIM_UTIL) && !defined(FEATURE_UIM_UICC)
      case SW1_UTIL_END_RESP:
#endif /* FEATURE_UIM_UTIL */
#ifdef FEATURE_UIM_UICC
      case SW1_USIM_END_RESP:
#endif /* FEATURE_UIM_UICC */
#if defined(FEATURE_UIM_UICC) || defined(FEATURE_UIM_UTIL)
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_USIM_END_RESP;
        } /* end case - SW1_USIM_END_RESP */
        break;

#if defined (FEATURE_UIM_UTIL) && !defined(FEATURE_UIM_UICC)
      case SW1_LENGTH_ERROR:
#endif /* FEATURE_UIM_UTIL */
#ifdef FEATURE_UIM_UICC
      case SW1_USIM_RESEND_APDU:
#endif /* FEATURE_UIM_UICC */
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_USIM_RESEND_APDU;
        } /* end case - SW1_USIM_RESEND_APDU */
        break;

      case SW1_WARNINGS1:
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_WARNINGS1;
        } /* end case - SW1_WARNINGS1 */
        break;

      case SW1_WARNINGS2:
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_WARNINGS2;
        } /* end case - SW1_WARNINGS2 */
        break;

      case SW1_EXECUTION_ERR2:
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_EXECUTION_ERR2;
        } /* end case - SW1_EXECUTION_ERR2 */
        break;

      case SW1_CMD_NOT_ALLOWED:
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_CMD_NOT_ALLOWED;
        } /* end case - SW1_CMD_NOT_ALLOWED */
        break;

      case SW1_WRONG_PARAMS:
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_WRONG_PARAMS;
        } /* end case - SW1_WRONG_PARAMS */
        break;

#endif /* FEATURE_UIM_UICC || FEATURE_UIM_UTIL */

#ifdef FEATURE_UIM_WIM
      case SW1_SE_FAIL:
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_SE_FAIL;
        } /* end case - SW1_USIM_END_RESP */
        break;
#endif /* FEATURE_UIM_WIM */

#ifdef FEATURE_UIM_UICC

      case SW1_EXECUTION_ERR1:
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_EXECUTION_ERR1;
        } /* end case - SW1_EXECUTION_ERR1 */
      break;

      case SW1_CLA_NOT_SUPPORTED:
        {
          /* Wait for the next status word */
          uim_rx_state[uim_drv_slot] = UIM_RX_SW1_CLA_NOT_SUPPORTED;
        } /* end case - SW1_CLA_NOT_SUPPORTED */
      break;

#endif /* FEATURE_UIM_UICC */

      default:
        {
#if !defined(UIM_NO_SIM_GLUE_LOGIC) && !defined(T_UIM_NO_RX_WATERLEVEL)
          /* Program the receive level to allow fast processing of tx'd bytes */
          UIM_RX_WATERLEVEL ( UIM_RX_WATER_MARK_LEVEL_ZERO );
#endif /* !UIM_NO_SIM_GLUE_LOGIC && T_UIM_NO_RX_WATERLEVEL */

          /* This may be an ACK byte */
          /* The next operation is dependent on the value of the Ack byte
             with respect to the INS of the APDU.  The action is determined
             by XORing the Ack byte with the INS                    */
          result = sw1 ^ ins_byte[uim_drv_slot];

          /* Determine what to do next */
          switch (result)
          {
            case PAUSE_REMAINING_BYTES:   /* Send the remaining bytes of the
                                             data                            */
            case PROG_REMAINING_BYTES:    /* Send the remaining bytes of the
                                             data                            */
              {
                /* Determine if command data bytes are to be sent */
                if ( (num_cmd_data_bytes_to_send[uim_drv_slot] > 0) &&
                     (num_bytes_to_send[uim_drv_slot]  > 0) )
                {
                  /* Change the Rx ISR state to send the command data bytes */
                  uim_tx_state[uim_drv_slot] = UIM_TX_SND_CMD_DATA;

                  /* Indicate the remaining bytes are to be sent */
                  send_all_remaining_bytes[uim_drv_slot] = TRUE;

                  /* Wait long enough before responding to procedure byte */
                  uim_clk_busy_wait( uim_response_byte_delay );

#ifdef FEATURE_UIM_DEBUG_LOG
                  UIM_LOG_PUT_LAST_BYTE();
                  UIM_LOG_PUT_BYTE(UIM_LOG_TX_DATA,
                                   *request_buffer_ptr[uim_drv_slot]);
#endif
                  /* Send the first data byte */
                  uim_send_byte (*request_buffer_ptr[uim_drv_slot]);
                }
                /* Determine if a response is expected */
                else if (num_resp_data_bytes_expected[uim_drv_slot] != 0)
                {
                  /* Process the incoming bytes */
                  uim_rx_state[uim_drv_slot] = UIM_RX_PROC_RESP_DATA;
                }
                else /* Neither command data to send nor response expected
                        AND we are processing an ACK.  Something went wrong  */
                {
                  /* Something went wrong.  We got a bad uim_tx_state value
                   * set the sate to UIM_RX_UNKNOWN_PROC_BYTE_RCVD this would ignore all
                   * bytes untill a time-out and then re-try the command
                   */
                  uim_rx_state[uim_drv_slot] = UIM_RX_UNKNOWN_PROC_BYTE_RCVD;

                  /* Indicate the task to ignore the following timeout */
                  uim_bad_status_words_error = TRUE;
#ifdef FEATURE_UIM_DEBUG_LOG
                  UIM_LOG_PUT_LAST_BYTE();
#endif
                  /* Set the number of bytes received to zero. */
                  resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

                  /* Give a bad status. */
                  resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

                  return;
                } /* end if - there are command bytes to send */
              } /* end case - REMAINING_BYTES */
              break;

            case PAUSE_NEXT_BYTE:         /* Send only the next byte and wait
                                             for Ack                         */
            case PROG_NEXT_BYTE:          /* Send only the next byte and wait
                                             for Ack                         */
              {
                /* Determine if command data bytes are to be sent */
                if ( (num_cmd_data_bytes_to_send[uim_drv_slot] > 0) &&
                     (num_bytes_to_send[uim_drv_slot]  > 0) )
                {
                  /* Change the Rx ISR state to send the command data bytes */
                  uim_tx_state[uim_drv_slot] = UIM_TX_SND_CMD_DATA;

                  /* Indicate only send one byte at a time */
                  send_all_remaining_bytes[uim_drv_slot] = FALSE;

                  /* Wait long enough before responding to procedure byte */
                  uim_clk_busy_wait( uim_response_byte_delay);

#ifdef FEATURE_UIM_DEBUG_LOG
                  UIM_LOG_PUT_LAST_BYTE();
                  UIM_LOG_PUT_BYTE(UIM_LOG_TX_DATA,
                                   *request_buffer_ptr[uim_drv_slot]);
#endif
                  /* Send the first data byte */
                  uim_send_byte (*request_buffer_ptr[uim_drv_slot]);
                }
                /* Determine if a response is expected */
                else if (num_resp_data_bytes_expected[uim_drv_slot] != 0)
                {
                  /* Process the incoming bytes */
                  uim_rx_state[uim_drv_slot] = UIM_RX_PROC_RESP_DATA;
                }
                else /* Neither command data to send nor response expected
                        AND we are processing an ACK.  Something went wrong  */
                {
                  /* Something went wrong.  We got a bad uim_tx_state value
                   * set the sate to UIM_RX_UNKNOWN_PROC_BYTE_RCVD this would ignore all
                   * bytes untill a time-out and then re-try the command
                   */
                  uim_rx_state[uim_drv_slot] = UIM_RX_UNKNOWN_PROC_BYTE_RCVD;

                  /* Indicate the task to ignore the following timeout */
                  uim_bad_status_words_error = TRUE;
#ifdef FEATURE_UIM_DEBUG_LOG
                  UIM_LOG_PUT_LAST_BYTE();
#endif
                  /* Set the number of bytes received to zero. */
                  resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

                  /* Give a bad status. */
                  resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

                  return;
                } /* end if - there are command bytes to send */
              } /* end case - NEXT_BYTE */
              break;

            default:
              {
                /* Something went wrong.  We got a bad uim_tx_state value
                 * set the sate to UIM_RX_UNKNOWN_PROC_BYTE_RCVD this would ignore all
                 * bytes untill a time-out and then re-try the command
                 */
                uim_rx_state[uim_drv_slot] = UIM_RX_UNKNOWN_PROC_BYTE_RCVD;

                /* Indicate the task to ignore the following timeout */
                uim_bad_status_words_error = TRUE;
#ifdef FEATURE_UIM_DEBUG_LOG
                UIM_LOG_PUT_LAST_BYTE();
#endif
                /* Set the number of bytes received to zero. */
                resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

                /* Give a bad status. */
                resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

                return;
                /* Should I tell the UIM server of this failure? */
              } /* end case - default */
          } /* end switch - result of Ack */
        } /* end case - default */
        break;
    } /* end switch - the first status word */
  } /* end if - the byte is a an error signal */
} /* end rx_isr_proc_procedure_bytes */


/*===========================================================================

FUNCTION RX_ISR_SW1_NORMAL_END                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a normal
  end status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of a
  normal ending status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_normal_end
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the next received byte. */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif
  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Make sure the second status word is proper */
  if (sw2 == (int)SW2_NORMAL_END)
  {
    /* Place the number of bytes read in the response buffer */
    resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd_total[uim_drv_slot];

    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

    /* Check if we are processing an ENVELOPE APDU:
       If TRUE and warnings were received earlier, set the
       appropriate SW1 WARNING status. */

    if(uim_processing_envelope_apdu() == TRUE)
    {
      if(uim_warning1_indicator == TRUE)
      {
        uim_get_resp_sw1 = SW1_WARNINGS1;
      }
      else if(uim_warning2_indicator == TRUE)
      {
        uim_get_resp_sw1 = SW1_WARNINGS2;
      }
    }

    /* Reset the warning1 indicators */
    uim_warning1_indicator = FALSE;

    /* Reset the warning2 indicators */
    uim_warning2_indicator = FALSE;

    /* Send an indication the APDU completed successfully. */
    UIM_TEST_INT_PROCESSING(UIM_DEBUG_NORMAL_END);

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();

    /* Change to idle */
    uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

  } /* end if - the second status word is proper */
  else
  {
    /* Something went wrong.  We got a bad uim_tx_state value
     * set the sate to UIM_RX_UNKNOWN_PROC_BYTE_RCVD this would ignore all
     * bytes untill a time-out and then re-try the command
     */
    uim_rx_state[uim_drv_slot] = UIM_RX_UNKNOWN_PROC_BYTE_RCVD;

    /* Indicate the task to ignore the following timeout */
    uim_bad_status_words_error = TRUE;

    /* Set the number of bytes received to zero. */
    resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

    /* Give a bad status. */
    resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();
  }
#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

} /* end rx_isr_sw1_normal_end */


/*===========================================================================

FUNCTION RX_ISR_SW1_FETCH_END                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a fetch
  end status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of a
  fetch ending status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_fetch_end
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Send a fetch to this command */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();

    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the length of the FETCH command response. */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif

  /* Save the status word.  It contains the size of the proactive command. */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Place the number of bytes read in the response buffer */
  resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd_total[uim_drv_slot];

  /* Indicate APDU result */
  resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* Check if this from processing a download error */
  if (processing_download_error[uim_drv_slot])
  {
    /* Set the status words to reflect a download error */
    resp_buf[uim_drv_slot]->sw1 = (int)SW1_DLOAD_ERR;
    resp_buf[uim_drv_slot]->sw2 = cmd_req[uim_drv_slot].apdu_hdr.p3;
  } /* end if - Finished processing a data download error */
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Check if we are processing an ENVELOPE APDU:
     If TRUE and warnings were received earlier, set the
     appropriate SW1 WARNING status. */

  if(uim_processing_envelope_apdu() == TRUE)
  {
    if(uim_warning1_indicator == TRUE)
    {
      uim_get_resp_sw1 = SW1_WARNINGS1;
    }
    else if(uim_warning2_indicator == TRUE)
    {
      uim_get_resp_sw1 = SW1_WARNINGS2;
    }
  }

  /* Reset the warning1 indicators */
  uim_warning1_indicator = FALSE;

  /* Reset the warning2 indicators */
  uim_warning2_indicator = FALSE;

  /* Send an indication the APDU completed successfully. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_NORMAL_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

} /* end rx_isr_sw1_fetch_end */


#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
/*===========================================================================

FUNCTION RX_ISR_SW1_DLOAD_ERR                         INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a SIM
  data download error status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of a
  SIM data download error status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_dload_err
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Send a get_response to this command */
  /* Get the length of the GET RESPONSE command response. */
  sw2 = UIM_GET_RX_BYTE ();

  uim_get_resp_sw1 = SW1_DLOAD_ERR;
  uim_get_resp_sw2 = (uim_sw2_type)sw2;

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif
  /* Determine if the local buffer is full */
  if (num_resp_bytes_rcvd_total[uim_drv_slot] != UIM_MAX_CHARS)
  {
    /* Determine how much will fit into the buffer */
    if ((num_resp_bytes_rcvd_total[uim_drv_slot] + sw2) > UIM_MAX_CHARS)
    {
      sw2 = (byte)(UIM_MAX_CHARS - num_resp_bytes_rcvd_total[uim_drv_slot]);
    } /* end if - requested size is too big for local buffer */

    /* Build the GET RESPONSE command */
    /* This needs to be modified to work with WIM */
    if ((uimdrv_protocol[uim_drv_slot] == UIM_UICC) ||
        (uimdrv_protocol[uim_drv_slot] == UIM_WCDMA))
    {
      cmd_req[uim_drv_slot].apdu_hdr.uim_class = 0x00;

#ifdef FEATURE_UIM_UTIL
      cmd_req[uim_drv_slot].apdu_hdr.uim_class |=
        (last_sent_apdu[uim_drv_slot].uim_class & UIM_LOGICAL_CHANNEL_MASK);
#endif /* FEATURE_UIM_UTIL */
    }
    else
    {
      cmd_req[uim_drv_slot].apdu_hdr.uim_class = 0xA0;
    }

    cmd_req[uim_drv_slot].protocol        = uimdrv_protocol[uim_drv_slot];
    cmd_req[uim_drv_slot].apdu_hdr.instrn = GET_RESPONSE;
    cmd_req[uim_drv_slot].apdu_hdr.p1     = 0x00;
    cmd_req[uim_drv_slot].apdu_hdr.p2     = 0x00;
    cmd_req[uim_drv_slot].apdu_hdr.p3     = sw2;
    cmd_req[uim_drv_slot].instrn_case     = UIM_INSTRN_CASE_2;
    cmd_req[uim_drv_slot].rsp_ptr         = resp_buf[uim_drv_slot];
    cmd_req[uim_drv_slot].rpt_function    =
      apdu_resp_callback_func[uim_drv_slot];

    /* This flag is processed in the Normal end SW processing.
       It distinguishes between data download error and all other
       results. */
    processing_download_error[uim_drv_slot] = TRUE;

    /* Wait long enough before responding to procedure byte */
    uim_clk_busy_wait( uim_response_byte_delay );

    /* Send the GET RESPONSE command */
    uim_send_apdu ( &cmd_req[uim_drv_slot] );
  }
  else /* The local buffer is full */
  {
    /* End this transaction */
    resp_buf[uim_drv_slot]->sw1 = (int)SW1_DLOAD_ERR;
    resp_buf[uim_drv_slot]->sw2 = cmd_req[uim_drv_slot].apdu_hdr.p3;
    /* Place the number of bytes read in the response buffer */
    resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd_total[uim_drv_slot];

    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();

    /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
    processing_download_error[uim_drv_slot] = FALSE;

    /* Change to idle */
    uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
  } /* end if - the local buffer is full */
} /* end rx_isr_sw1_dload_err */
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */


/*===========================================================================

FUNCTION RX_ISR_SW1_RESP_END                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a response
  end status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of a
  response ending status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_resp_end
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Send a get_response to this command */
  /* Get the length of the GET RESPONSE command response. */
  sw2 = UIM_GET_RX_BYTE ();

  uim_get_resp_sw2 = (uim_sw2_type)sw2;

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif
  /* Determine if the local buffer is full */
  if (num_resp_bytes_rcvd_total[uim_drv_slot] != UIM_MAX_CHARS)
  {
    /* If we choose not to do a get response either because of
     * having the response cached, or due to this being an
     * intermediate select or even if this is a re-selection
     * to the MF since the last selection failed.  Do a get response
     * any way if there was an error or a warning
     */
    if ( ( uim_item_sel_rsp_cached  || uim_intermediate_sel ||
           uim_reselect_mf) && ! ( uim_force_get_response ||
                                   uim_gcf_testing_flag) )
    {
      resp_buf[uim_drv_slot]->sw1 = (int)SW1_NORMAL_END;
      resp_buf[uim_drv_slot]->sw2 = (int)SW2_NORMAL_END;

      /* Indicate APDU result */
      resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

      /* Call the response callback function */
      apdu_resp_callback_func[uim_drv_slot]();

      /* Change to idle */
      uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
    }
    else
    {
      /* Determine how much will fit into the buffer */
      if ((num_resp_bytes_rcvd_total[uim_drv_slot] + sw2) > UIM_MAX_CHARS)
      {
        sw2 = (byte)(UIM_MAX_CHARS - num_resp_bytes_rcvd_total[uim_drv_slot]);
      } /* end if - requested size is too big for local buffer */

      /* Build the GET RESPONSE command */
      /* This needs to be modified to work with WIM */
      if ((uimdrv_protocol[uim_drv_slot] == UIM_UICC) ||
          (uimdrv_protocol[uim_drv_slot] == UIM_WCDMA))
      {
        cmd_req[uim_drv_slot].apdu_hdr.uim_class = 0x00;
#ifdef FEATURE_UIM_UTIL
        cmd_req[uim_drv_slot].apdu_hdr.uim_class |=
          (last_sent_apdu[uim_drv_slot].uim_class & UIM_LOGICAL_CHANNEL_MASK);
#endif /* FEATURE_UIM_UTIL */
      }
      else
      {
        cmd_req[uim_drv_slot].apdu_hdr.uim_class = 0xA0;
      }

      cmd_req[uim_drv_slot].protocol        = uimdrv_protocol[uim_drv_slot];
      cmd_req[uim_drv_slot].apdu_hdr.instrn = GET_RESPONSE;
      cmd_req[uim_drv_slot].apdu_hdr.p1     = 0x00;
      cmd_req[uim_drv_slot].apdu_hdr.p2     = 0x00;
      cmd_req[uim_drv_slot].instrn_case     = UIM_INSTRN_CASE_2;
      cmd_req[uim_drv_slot].rsp_ptr         = resp_buf[uim_drv_slot];
      cmd_req[uim_drv_slot].rpt_function    =
        apdu_resp_callback_func[uim_drv_slot];

      /* If this was a warning let us set the length to 0 */
      if (uim_rx_state[uim_drv_slot] == UIM_RX_SW1_WARNINGS1 ||
          uim_rx_state[uim_drv_slot] == UIM_RX_SW1_WARNINGS2)
      {
        cmd_req[uim_drv_slot].apdu_hdr.p3     = 0;
      }
      else
      {
        cmd_req[uim_drv_slot].apdu_hdr.p3     = sw2;
      }

      /* Wait long enough before responding to procedure byte */
      uim_clk_busy_wait( uim_response_byte_delay );

      /* Send the GET RESPONSE command */
      uim_send_apdu ( &cmd_req[uim_drv_slot] );
    } /* else of uim_item_sel_rsp_cached */
  }
  else /* The local buffer is full */
  {
    /* End this transaction */
#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
    /* Check if this from processing a download error */
    if (processing_download_error[uim_drv_slot])
    {
      resp_buf[uim_drv_slot]->sw1 = (int)SW1_DLOAD_ERR;
      resp_buf[uim_drv_slot]->sw2 = cmd_req[uim_drv_slot].apdu_hdr.p3;
    } /* end if - Finished processing a data download error */
    else /* Emulate a normal end */
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */
    {
      resp_buf[uim_drv_slot]->sw1 = (int)SW1_NORMAL_END;
      resp_buf[uim_drv_slot]->sw2 = (int)SW2_NORMAL_END;
    }
    /* Place the number of bytes read in the response buffer */
    resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd_total[uim_drv_slot];

    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
    /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
    processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

    /* Change to idle */
    uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
  } /* end if - the local buffer is full */
} /* end rx_isr_sw1_resp_end */


/*===========================================================================

FUNCTION RX_ISR_SW1_RESEND_APDU                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a resend
  APDU status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of a
  status word indicating that the last APDU needs to be re-sent.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_resend_apdu
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check for a receive error */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Send a get_response to this command */
  /* Get the length of the GET RESPONSE command response. */
  sw2 = UIM_GET_RX_BYTE ();

  uim_get_resp_sw1 = SW1_USIM_RESEND_APDU;
  uim_get_resp_sw2 = (uim_sw2_type)sw2;

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif
  /* Determine if the local buffer is full */
  if (num_resp_bytes_rcvd_total[uim_drv_slot] != UIM_MAX_CHARS)
  {
    /* Determine how much will fit into the buffer */
    if ((num_resp_bytes_rcvd_total[uim_drv_slot] + sw2) > UIM_MAX_CHARS)
    {
      sw2 = (byte)(UIM_MAX_CHARS - num_resp_bytes_rcvd_total[uim_drv_slot]);
    } /* end if - requested size is too big for local buffer */

    /* Resend the last APDU with p3 equal to SW2*/
    cmd_req[uim_drv_slot].protocol        = uimdrv_protocol[uim_drv_slot];
    cmd_req[uim_drv_slot].apdu_hdr        = last_sent_apdu[uim_drv_slot];
    cmd_req[uim_drv_slot].apdu_hdr.p3     = sw2;
    cmd_req[uim_drv_slot].instrn_case     = uim_last_instrn_case[uim_drv_slot];
    cmd_req[uim_drv_slot].rsp_ptr         = resp_buf[uim_drv_slot];
    cmd_req[uim_drv_slot].rpt_function    =
      apdu_resp_callback_func[uim_drv_slot];

    /* Wait long enough before responding to procedure byte */
    uim_clk_busy_wait( uim_response_byte_delay );

    /* Send the last command command */
    uim_send_apdu ( &cmd_req[uim_drv_slot] );
  }
  else /* The local buffer is full */
  {
    /* End this transaction */
#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
    /* Check if this from processing a download error */
    if (processing_download_error[uim_drv_slot])
    {
      resp_buf[uim_drv_slot]->sw1 = (int)SW1_DLOAD_ERR;
      resp_buf[uim_drv_slot]->sw2 = cmd_req[uim_drv_slot].apdu_hdr.p3;
    } /* end if - Finished processing a data download error */
    else /* Emulate a normal end */
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */
    {
      resp_buf[uim_drv_slot]->sw1 = (int)SW1_NORMAL_END;
      resp_buf[uim_drv_slot]->sw2 = (int)SW2_NORMAL_END;
    }
    /* Place the number of bytes read in the response buffer */
    resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd_total[uim_drv_slot];

    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
    /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
    processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

    /* Change to idle */
    uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
  } /* end if - the local buffer is full */
} /* end rx_isr_sw1_resend_apdu */


/*===========================================================================

FUNCTION RX_ISR_SW1_BUSY                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a busy
  status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of a
  busy status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_busy
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif
  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Make sure the second status word is proper */
  if (sw2 == (int)SW2_NORMAL_END)
  {
    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_TOOLKIT_BUSY;

    /* Set the number of bytes received to zero. */
    resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

    /* Send an indication the APDU completed with an error. */
    UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();

    /* Change to idle */
    uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
  } /* end if - the second status word is proper */
  else
  {
    /* Something went wrong.  We got a bad uim_tx_state value
     * set the sate to UIM_RX_UNKNOWN_PROC_BYTE_RCVD this would ignore all
     * bytes untill a time-out and then re-try the command
     */
    uim_rx_state[uim_drv_slot] = UIM_RX_UNKNOWN_PROC_BYTE_RCVD;

    /* Indicate the task to ignore the following timeout */
    uim_bad_status_words_error = TRUE;

    /* Set the number of bytes received to zero. */
    resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

    /* Give a bad status. */
    resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();
  }
#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

} /* end rx_isr_sw1_busy */


/*===========================================================================

FUNCTION RX_ISR_SW1_RETRIES                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a retries
  status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of a
  retries status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_retries
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif
  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Determine if it is a memory problem */
  if (sw2 == (int)SW2_MEM_PROBLEM)
  {
    /* Set the number of bytes received to zero. */
    resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_MEMORY_PROBLEM;
  }
  else /* successful after retries */
  {
    /* Place the number of bytes read in the response buffer */
    resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd_total[uim_drv_slot];

    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_SUCCESS_AFTER_MULTI_TRIES;
  } /* end if - the second status word indicates a memory problem */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_sw1_retries */


/*===========================================================================

FUNCTION RX_ISR_SW1_REFERENCE                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a reference
  management problem status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of a
  reference management problem status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_reference
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif

  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Process the second status word */
  switch (sw2)
  {
    case SW2_NO_EF:           /* No EF selected */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_NO_EF_SELECTED;
      } /* end case - SW2_NO_EF */
      break;

    case SW2_OUT_OF_RANGE:    /* Out of range (invalid address) */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_OUT_OF_RANGE;
      } /* end case - SW2_OUT_OF_RANGE */
      break;

    case SW2_NOT_FOUND:       /* File ID/pattern not found */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_NOT_FOUND;
      } /* end case - SW2_NOT_FOUND */
      break;

    case SW2_INCONSISTENT:    /* File inconsistent with command */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_INCONSISTENT_WITH_COMMAND;
      } /* end case - SW2_INCONSISTENT */
      break;

#if defined(FEATURE_UIM_UICC) || defined(FEATURE_UIM_UTIL)
    case SW2_ALGORITHM_NOT_SUPPORTED:
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_ALGORITHM_NOT_SUPPORTED;
      } /* end case - SW2_ALGORITHM_NOT_SUPPORTED */
      break;

    case SW2_INVALID_KEY_CHECK_VALUE:
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_INVALID_KEY_CHECK_VALUE;
      } /* end case - SW2_INVALID_KEY_CHECK_VALUE */
      break;

#endif /* FEATURE_UIM_UICC  || FEATURE_UIM_UTIL */
    default:
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_sw1_reference */


/*===========================================================================

FUNCTION RX_ISR_SW1_SECURITY                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a security
  problem status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of a
  security problem status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_security
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif

  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Process the second status word */
  switch (sw2)
  {
    case SW2_NO_CHV:          /* No CHV initialized */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_NO_CHV_INITIALIZED;
      } /* end case - SW2_NO_CHV */
      break;

    case SW2_NOT_FULFILLED:   /* Access condition not fulfilled */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_ACCESS_NOT_FULFILLED;
      } /* end case - SW2_NOT_FULFILLED */
      break;

    case SW2_CONTRADICTION:   /* In contradiction with CHV status */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_CONTRADICTION_W_CHV_STATUS;
      } /* end case - SW2_CONTRADICTION */
      break;

    case SW2_INVALIDATION:    /* In contradiction with invalidation status */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status =
          UIM_CONTRADICTION_W_INVALID_STATUS;
      } /* end case - SW2_INVALIDATION */
      break;

    case SW2_SEQ_PROBLEM:     /* Sequence problem */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_SSD_UPDATE_SEQ_PROBLEM;
      } /* end case - SW2_SEQ_PROBLEM */
      break;

    case SW2_BLOCKED:         /* Access blocked */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_CHV_BLOCKED;
      } /* end case - SW2_BLOCKED */
      break;

    case SW2_MAX_REACHED:     /* Increase cannot be performed; Max value
                                 reached                          */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_MAX_VALUE_REACHED;
      } /* end case - SW2_MAX_REACHED */
      break;

#ifdef FEATURE_UIM_UICC
    case SW2_APPLN_SPECIFIC_AUTH_ERR:  /* Applcn specific auth error */
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_APPLCN_SPECIFIC_AUTH_ERROR;
      } /* end case - SW2_APPLN_SPECIFIC_AUTH_ERR */
      break;
#endif /* FEATURE_UIM_UICC */

    default:
      {
        /* Set the number of bytes received to zero. */
        resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_sw1_security */


/*===========================================================================

FUNCTION RX_ISR_SW1_P3_BAD                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a P3 is
  bad status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of a
  P3 is bad status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_p3_bad
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

  uim_get_resp_sw1 = SW1_P3_BAD;
  uim_get_resp_sw2 = (uim_sw2_type)sw2;

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif

  if (sw2 == 0x00)
  {
    /* Save the status word in case it is a true status word response */
    resp_buf[uim_drv_slot]->sw2 = sw2;

    /* Set the number of bytes received to zero. */
    resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_INCORRECT_PARAM_P3;

    /* Send an indication the APDU completed with an error. */
    UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

    /* Call the response callback function */
    apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
    /* This flag is processed in the Normal end SW processing.
       It distinguishes between data download error and all other
       results. */
    processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

    /* Change to idle */
    uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
  }
  /* If SW2 is XX !=0, then resend the cmd with SW2 as P3 */
  else
  {
    /* Determine if the local buffer is full */
    if (num_resp_bytes_rcvd_total[uim_drv_slot] != UIM_MAX_CHARS)
    {
      /* Determine how much will fit into the buffer */
      if ((num_resp_bytes_rcvd_total[uim_drv_slot] + sw2) > UIM_MAX_CHARS)
      {
        sw2 = (byte)(UIM_MAX_CHARS - num_resp_bytes_rcvd_total[uim_drv_slot]);
      } /* end if - requested size is too big for local buffer */

      /* Resend the last APDU with p3 equal to SW2 */
      cmd_req[uim_drv_slot].protocol        = uimdrv_protocol[uim_drv_slot];
      cmd_req[uim_drv_slot].apdu_hdr        = last_sent_apdu[uim_drv_slot];
      cmd_req[uim_drv_slot].apdu_hdr.p3     = sw2;
      cmd_req[uim_drv_slot].instrn_case     = uim_last_instrn_case[uim_drv_slot];
      cmd_req[uim_drv_slot].rsp_ptr         = resp_buf[uim_drv_slot];
      cmd_req[uim_drv_slot].rpt_function    =
        apdu_resp_callback_func[uim_drv_slot];
      /* Wait long enough before responding to procedure byte */
      uim_clk_busy_wait( uim_response_byte_delay );
      /* Send the last command command */
      uim_send_apdu ( &cmd_req[uim_drv_slot] );
    }
    else /* The local buffer is full */
    {
      /* End this transaction */
#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
      /* Check if this from processing a download error */
      if (processing_download_error[uim_drv_slot])
      {
        resp_buf[uim_drv_slot]->sw1 = (int)SW1_DLOAD_ERR;
        resp_buf[uim_drv_slot]->sw2 = cmd_req[uim_drv_slot].apdu_hdr.p3;
      } /* end if - Finished processing a data download error */
      else /* Emulate a normal end */
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */
      {
        resp_buf[uim_drv_slot]->sw1 = (int)SW1_NORMAL_END;
        resp_buf[uim_drv_slot]->sw2 = (int)SW2_NORMAL_END;
      }
      /* Place the number of bytes read in the response buffer */
      resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd_total[uim_drv_slot];

      /* Indicate APDU result */
      resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

      /* Call the response callback function */
      apdu_resp_callback_func[uim_drv_slot]();

  #ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
      /* This flag is processed in the Normal end SW processing.
       It distinguishes between data download error and all other
       results. */
      processing_download_error[uim_drv_slot] = FALSE;
  #endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

      /* Change to idle */
      uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
    } /* end if - the local buffer is full */
  } /* SW2 is 0 */
} /* end rx_isr_sw1_p3_bad */


/*===========================================================================

FUNCTION RX_ISR_SW1_P1_P2_BAD                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a P1 or
  P2 is bad status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of a
  P1 or P2 is bad status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_p1_p2_bad
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif

  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Set the number of bytes received to zero. */
  resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

  /* Indicate APDU result */
  resp_buf[uim_drv_slot]->cmd_status = UIM_INCORRECT_PARAM_P1_2;

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_sw1_p1_p2_bad */


/*===========================================================================

FUNCTION RX_ISR_SW1_INS_BAD                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after an
  instruction is bad status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of an
  instruction is bad status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_ins_bad
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif
  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Set the number of bytes received to zero. */
  resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

  /* Indicate APDU result */
  resp_buf[uim_drv_slot]->cmd_status = UIM_UNKNOWN_INS;

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_sw1_ins_bad */


/*===========================================================================

FUNCTION RX_ISR_SW1_CLA_BAD                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a class
  is bad status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of a
  class is bad status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_cla_bad
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif
  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Set the number of bytes received to zero. */
  resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

  /* Indicate APDU result */
  resp_buf[uim_drv_slot]->cmd_status = UIM_WRONG_CLASS;

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_sw1_cla_bad */


/*===========================================================================

FUNCTION RX_ISR_SW1_PROBLEM                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after an
  undisclosed problem.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of an
  undisclosed problem status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_problem
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif
  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Set the number of bytes received to zero. */
  resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

  /* Indicate APDU result */
  resp_buf[uim_drv_slot]->cmd_status = UIM_PROBLEM;

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_sw1_problem */


#if defined (FEATURE_UIM_UICC) || defined (FEATURE_UIM_UTIL)

/*===========================================================================

FUNCTION RX_ISR_SW1_WARNINGS1                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a status
  byte coding indicating a warning.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of an
  warning status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_warnings1
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif

  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Process the second status word */
  switch (sw2)
  {
    case SW2_NV_STATE_UNCHANGED:          /* No info with NV unchanged */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_NO_INFO_AND_NV_UNCHANGED;
      } /* end case - SW2_NV_STATE_UNCHANGED */
      break;
#ifdef FEATURE_UIM_UICC
    case SW2_RET_DATA_MAY_BE_CORRUPTED:   /* Part of ret data may be corrupt */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PART_DATA_MAY_BE_CORRUPTED;
      } /* end case - SW2_RET_DATA_MAY_BE_CORRUPTED */
      break;

    case SW2_EOF_REACHED:            /* EOF reached before reading Le bytes  */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_EOF_BEFORE_READING_LE_BYTES;
      } /* end case - SW2_EOF_REACHED */
      break;

    case SW2_SEL_EF_INVALIDATED:    /* Selected file invalidated */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_SELECTED_FILE_INVALIDATED;
      } /* end case - SW2_SEL_EF_INVALIDATED */
      break;

    case SW2_INCORRECT_FCI_FORMAT:     /* FCI not formatted correctly */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_INCORRECT_FCI_FORMAT;
      } /* end case - SW2_INCORRECT_FCI_FORMAT */
      break;
#endif /* FEATURE_UIM_UICC */
    default:
      {

        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_sw1_warnings1 */


/*===========================================================================

FUNCTION RX_ISR_SW1_WARNINGS2                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a status
  byte coding indicating a warning.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of an
  warning status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_warnings2
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif
  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  switch (sw2)                 /* Process the second status word */
  {
    case SW2_NV_STATE_CHANGED:          /* No info with NV changed */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_NO_INFO_AND_NV_CHANGED;
      } /* end case -  */
      break;
#ifdef FEATURE_UIM_UICC
    case SW2_FILE_FILLED_BY_LAST_WRITE:   /* File filled up by last write */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_FILE_FILLED_UP_BY_LAST_WRITE;
      } /* end case -  */
      break;
    case SW2_MORE_DATA_AVAILABLE:         /* More Data Available */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_MORE_DATA_AVAILABLE;
      } /* end case -  */
      break;
#endif /* FEATURE_UIM_UICC */
    default:
      {
        if ((sw2 & 0xF0) == (int)SW2_CMD_SUCCESS_INT_X_RETRIES)
        {
           /* Indicate APDU result */
           resp_buf[uim_drv_slot]->cmd_status = UIM_CMD_SUCCESS_WITH_X_RETRIES;
        }
        else
        {
           /* Indicate APDU result */
           resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

           /* Reset the receiver and leave it disabled.  This will ignore any
              more characters received from the UIM.        */
           UIM_RESET_RX ();
           /* Should I tell the UIM server of this failure? */
        }
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_sw1_warnings2 */


/*===========================================================================

FUNCTION RX_ISR_SW1_EXECN_ERR2                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a status
  byte coding indicating an execution error .

DEPENDENCIES
  This function is called from the receive ISR after the receipt of an
  warning status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_execn_err2
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS () & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif

  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  switch (sw2)                 /* Process the second status word */
  {
    case SW2_NV_STATE_CHANGED:          /* No info with NV changed */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_EXEC_ERR_AND_NV_CHANGED;
      } /* end case - SW2_NV_STATE_CHANGED */
      break;

    case SW2_MEMORY_PROBLEM:            /* Memory problem  */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_EXEC_ERR_MEMORY_PROBLEM;
      } /* end case -  */
      break;

    default:
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_sw1_execn_err2 */


/*===========================================================================

FUNCTION RX_ISR_SW1_CMD_NOT_ALLOWED                     INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a status
  byte coding indicating command is not allowed .

DEPENDENCIES
  This function is called from the receive ISR after the receipt of an
  warning status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_cmd_not_allowed
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif

  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  switch (sw2)                 /* Process the second status word */
  {
    case SW2_NO_INFO_GIVEN:          /* No info given  */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_CMD_ERR_NO_INFO;
      } /* end case - SW2_NO_INFO_GIVEN */
      break;

    case SW2_FILE_STRUCT_INCOMPATIBLE:  /* Cmd incompatible with file struct */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status =
          UIM_CMD_INCOMPATIBLE_WITH_FILE_STRUCT;
      } /* end case -  SW2_FILE_STRUCT_INCOMPATIBLE */
      break;


    case SW2_SECURITY_NOT_SATISFIED:     /* Security status not satisifed */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_SEC_STATUS_NOT_SATISFIED;
      } /* end case - SW2_SECURITY_NOT_SATISFIED  */
      break;

    case SW2_PIN_BLOCKED:     /* Authentication/PIN method blocked */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_AUTH_PIN_BLOCKED;
      } /* end case - UIM_SEC_STATUS_NOT_SATISFIED */
      break;

    case SW2_REF_DATA_INVALIDATED:     /* Referenced data invalidated */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_REF_DATA_INVALIDATED;
      } /* end case - SW2_REF_DATA_INVALIDATED */
      break;

    case SW2_CONDITIONS_NOT_SATISFIED:  /* Conditions of used not satisfied */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_USE_CONDNS_NOT_SATISFIED;
      } /* end case - SW2_CONDITIONS_NOT_SATISFIED */
      break;

    case SW2_NO_EF_SELECTED:     /* Cmd not allowed - No EF selected */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_CMD_ERR_NO_EF_SELECTED;
      } /* end case - SW2_NO_EF_SELECTED */
      break;

    case SW2_UNDEF_99:
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_WRONG_CLASS;
      } /* end case - SW2_UNDEF_99 */
      break;

    case SW2_UCAST_AUTH_KEY_NOT_INIT:
      {
#ifdef FEATURE_UIM_MFLO
#error code not present
#endif /* FEATURE_UIM_MFLO */
      } /* end case - SW2_UCAST_AUTH_KEY_NOT_INIT */
      break;

    case SW2_LOCKED_INIT_PARAM:
      {
#ifdef FEATURE_UIM_MFLO
#error code not present
#endif /* FEATURE_UIM_MFLO */
      } /* end case - SW2_LOCKED_INIT_PARAM */
      break;

    case SW2_VERIFY_UCAST_FAILED:
      {
#ifdef FEATURE_UIM_MFLO
#error code not present
#endif /* FEATURE_UIM_MFLO */
      } /* end case - SW2_VERIFY_UCAST_FAILED */
      break;

    case SW2_UCAST_DATA_BLOCK_NOT_INIT:
      {
#ifdef FEATURE_UIM_MFLO
#error code not present
#endif /* FEATURE_UIM_MFLO */
      } /* end case - SW2_UCAST_DATA_BLOCK_NOT_INIT */
      break;

    case SW2_APPLET_NOT_INIT:
      {
#ifdef FEATURE_UIM_MFLO
#error code not present
#endif /* FEATURE_UIM_MFLO */
      } /* end case - SW2_APPLET_NOT_INIT */
      break;

    case SW2_PUBLIC_KEY_OR_CERT_NOT_INIT:
      {
#ifdef FEATURE_UIM_MFLO
#error code not present
#endif /* FEATURE_UIM_MFLO */
      } /* end case - SW2_PUBLIC_KEY_OR_CERT_NOT_INIT */
      break;

    default:
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_cmd_not_allowed */

#if defined(FEATURE_UIM_UICC) || defined(FEATURE_UIM_UTIL)

/*===========================================================================

FUNCTION RX_ISR_SW1_WRONG_PARAMS                     INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a status
  byte coding indicating wrong parameters in the command .

DEPENDENCIES
  This function is called from the receive ISR after the receipt of an
  warning status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_wrong_params
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif

  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  switch (sw2)                 /* Process the second status word */
  {
    case SW2_BAD_PARAMS_IN_DATA_FIELD:  /* Incorrect params in the data field */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_INCORRECT_PARAM_IN_DATA_FIELD;
      } /* end case - SW2_BAD_PARAMS_IN_DATA_FIELD */
      break;

    case SW2_FUNCTION_NOT_SUPPORTED:  /* Function not supported */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_FUNCTION_NOT_SUPPORTED;
      } /* end case - SW2_FUNCTION_NOT_SUPPORTED */
      break;

    case SW2_FILE_NOT_FOUND:     /* File not found */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_FILE_NOT_FOUND;
      } /* end case - SW2_FILE_NOT_FOUND */
      break;

    case SW2_RECORD_NOT_FOUND:     /* Record not found */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_RECORD_NOT_FOUND;
      } /* end case - SW2_RECORD_NOT_FOUND */
      break;

    case SW2_INCONSISTENT_LC:     /* Lc inconsistent with TLV structure */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_LC_INCONSISITENT_WITH_TLV;
      } /* end case - SW2_INCONSISTENT_LC */
      break;

    case SW2_BAD_PARAMS_P1_P2:  /* Incorrect parameters P1_P2 */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_INCORRECT_PARAMS_P1_P2;
      } /* end case - SW2_BAD_PARAMS_P1_P2 */
      break;

    case SW2_LC_INCONSISTENT_WITH_P1_P2:  /* Lc inconsistent with P1 and P2 */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_LC_INCONSISITENT_WITH_P1_P2;
      } /* end case - SW2_LC_INCONSISTENT_WITH_P1_P2 */
      break;

    case SW2_REF_DATA_NOT_FOUND:  /* Referenced data not found */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_REF_DATA_NOT_FOUND;
      } /* end case - SW2_REF_DATA_NOT_FOUND */
      break;

    case SW2_NOT_ENOUGH_MEMORY_SPACE: /* Not Enough Memory Space */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_NOT_ENOUGH_MEMORY_SPACE;
      } /* end case - SW2_NOT_ENOUGH_MEMORY_SPACE */
      break;

    default:
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_wrong_params */
#endif /* FEATURE_UIM_UICC || FEATURE_UIM_UTIL */


#ifdef FEATURE_UIM_WIM
/*===========================================================================

FUNCTION RX_ISR_SW1_SE_FAIL                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after an
  instruction is bad status word.

DEPENDENCIES
  This function is called from the receive ISR after the receipt of an
  instruction is bad status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
void rx_isr_sw1_se_fail
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif
  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  /* Set the number of bytes received to zero. */
  resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

  /* Indicate APDU result */
  resp_buf[uim_drv_slot]->cmd_status = UIM_SE_NOT_SET;

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_sw1_ins_bad */


#endif /* FEATURE_UIM_WIM */

#ifdef FEATURE_UIM_UICC


/*===========================================================================

FUNCTION RX_ISR_SW1_EXECN_ERR1                          INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a status
  byte coding indicating an execution error .

DEPENDENCIES
  This function is called from the receive ISR after the receipt of an
  warning status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_execn_err1
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif

  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  switch (sw2)                 /* Process the second status word */
  {
    case SW2_NV_STATE_UNCHANGED:          /* No info with NV changed */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_EXEC_ERR_AND_NV_UNCHANGED;
      } /* end case - SW2_NV_STATE_UNCHANGED */
      break;

    default:
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_sw1_execn_err */


/*===========================================================================

FUNCTION RX_ISR_SW1_CLA_NOT_SUPPORTED                     INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called when expecting the second status word after a status
  byte coding indicating functions in CLA not supported .

DEPENDENCIES
  This function is called from the receive ISR after the receipt of an
  warning status word.

RETURN VALUE
  None

SIDE EFFECTS
  This function expects the incoming byte to be the second status word.
  This function may notify the UIM server when the APDU transfer has
  completed.

===========================================================================*/
LOCAL void rx_isr_sw1_cla_not_supported
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  byte sw2;       /* Holds the second status word */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is no problem with the received byte */
  //if ((uart_status = UIM_READ_STATUS ()) & MSMU_SR_PF_ERR)
  if (UIM_READ_STATUS() & MSMU_SR_PF_ERR)
  {
    /* Drop this byte as it will be re-transmitted */
    sw2 = UIM_GET_RX_BYTE ();
    UIM_PROCESS_PARITY_ERROR_RETURN( );
  } /* end if - check for parity error. */

  /* Get the second status word */
  sw2 = UIM_GET_RX_BYTE ();

#ifdef FEATURE_UIM_DEBUG_LOG
  UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, sw2);
  UIM_LOG_PUT_LAST_BYTE();
#endif

  /* Save the status word in case it is a true status word response */
  resp_buf[uim_drv_slot]->sw2 = sw2;

  switch (sw2)                 /* Process the second status word */
  {
    case SW2_NO_INFO_GIVEN:          /* No info given  */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_CLA_ERR_NO_INFO;
      } /* end case - No info given */
      break;

    case SW2_LOGICAL_CHAN_NOT_SUPPORTED:   /* Logical channel not supported */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_LOGICAL_CHAN_NOT_SUP;
      } /* end case - SW2_LOGICAL_CHAN_NOT_SUPPORTED */
      break;


    case SW2_SECURE_MSG_NOT_SUPPORTED:     /* Secure messaging not supported */
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_SECURE_MESSAGING_NOT_SUP;
      } /* end case -  SW2_SECURE_MSG_NOT_SUPPORTED */
      break;

    default:
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_PARITY;

        /* Reset the receiver and leave it disabled.  This will ignore any
           more characters received from the UIM.        */
        UIM_RESET_RX ();
        /* Should I tell the UIM server of this failure? */
      } /* end case - default */
      break;

  } /* end switch - sw2 */

  /* Send an indication the APDU completed with an error. */
  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ERROR_END);

  /* Call the response callback function */
  apdu_resp_callback_func[uim_drv_slot]();

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
  /* This flag is processed in the Normal end SW processing.
     It distinguishes between data download error and all other
     results. */
  processing_download_error[uim_drv_slot] = FALSE;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

  /* Change to idle */
  uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
} /* end rx_isr_sw1_cla_not_supported */


#endif /* FEATURE_UIM_UICC */
#endif /* FEATURE_UIM_UICC || FEATURE_UIM_UTIL */

#if defined( FEATURE_UIM_T_1_SUPPORT )


/*===========================================================================

FUNCTION RX_ISR_SEND_PROLOGUE                       INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called while sending the prologue field of a T=1 block.

DEPENDENCIES
  This function is called from the receive ISR while sending the prologue
  field of a T=1 block.

RETURN VALUE
  None

SIDE EFFECTS
  This function sends the prologue field out in order and changes state
  when the prologue field has been sent out.  Also, the appropriate error
  detection code is computed over the sent bytes.

===========================================================================*/
LOCAL void rx_isr_send_prologue
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  /* This holds the byte read from the receive FIFO */
  volatile byte rx_value;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set the ISR timer to the character waiting time */
  uim_work_waiting_time[uim_drv_slot] = uim_t_1_cwt;

  /* Check if there is room in the Tx FIFO */
  //while (( misr_status = UIM_READ_MISR()) & MSMU_ISR_TXLEV )
  while (UIM_READ_MISR() & MSMU_ISR_TXLEV )
  {
    /* Keep track of EDC */
    if (UIM_T_1_EDC_LRC == uim_t_1_edc)
    {
      uim_tx_epilogue_field.lrc ^= *request_buffer_ptr[uim_drv_slot];
    }
    else /* The EDC is CRC */
    {
      /* Compute the CRC with this byte */
      uim_tx_epilogue_field.crc =
        crc_16_l_table[ (uim_tx_epilogue_field.crc ^
                         *request_buffer_ptr[uim_drv_slot]) & 0x00ff ]
         ^ (uim_tx_epilogue_field.crc >> 8);
    } /* end if - the EDC is LRC */

    /* Send the next byte */
    uim_send_t_1_byte (*request_buffer_ptr[uim_drv_slot]);

    /* Determine if this is the last byte for this buffer */
    if (--num_bytes_to_send[uim_drv_slot] == 0)
    {
      /* Determine if there is an information field to send */
      if (num_cmd_data_bytes_to_send[uim_drv_slot] != 0)
      {
        /* Send the information field */
        num_bytes_to_send[uim_drv_slot] =
          num_cmd_data_bytes_to_send[uim_drv_slot];
        request_buffer_ptr[uim_drv_slot] = apdu_command_data_ptr[uim_drv_slot];
        uim_rx_state[uim_drv_slot] = UIM_RX_T_1_SEND_INFO_ST;
      }
      else /* no information field to send out */
      {
        /* Send the epilogue field */
        num_bytes_to_send[uim_drv_slot] = uim_epilogue_field_size;
        /* Determine if CRC is used for EDC */
        if (UIM_T_1_EDC_CRC == uim_t_1_edc)
        {
          /* Complement the CRC field */
          uim_tx_epilogue_field.crc = ~uim_tx_epilogue_field.crc;
        } /* end if - check if we are using CRC for EDC */
        request_buffer_ptr[uim_drv_slot] = (byte *)&uim_tx_epilogue_field;
        uim_rx_state[uim_drv_slot] = UIM_RX_T_1_SEND_EPILOGUE_ST;
      } /* end if - there is an information field to send */
      /* Exit this loop */
      return;
    }
    else /* there are more prologue bytes to send */
    {
      /* Send the next prologue byte */
      request_buffer_ptr[uim_drv_slot]++;

      /* Clear out the Rx FIFO */
      //if (( uart_status = UIM_READ_STATUS()) & MSMU_SR_RXRDY )
      if (UIM_READ_STATUS() & MSMU_SR_RXRDY )
      {
        /* Keep track of the number of transmitted bytes removed from
           the Rx FIFO */
        uim_num_txd_bytes--;
        /* Get the transmitted byte */
        rx_value = UIM_GET_RX_BYTE ();
      } /* end if - read bytes in the receive FIFO */
    } /* end if - this is the last byte of the current buffer */
  } /* end while - fill the Tx FIFO while there is room */
} /* end rx_isr_send_prologue */


#ifdef FEATURE_UIM_UART_DM
/*===========================================================================

FUNCTION RX_ISR_SEND_PROLOGUE_DM                    INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called while sending the prologue field of a T=1 block.

DEPENDENCIES
  This function is called from the receive ISR while sending the prologue
  field of a T=1 block.

RETURN VALUE
  None

SIDE EFFECTS
  This function sends the prologue field out in order and changes state
  when the prologue field has been sent out.  Also, the appropriate error
  detection code is computed over the sent bytes.

===========================================================================*/
LOCAL void rx_isr_send_prologue_dm
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  unsigned char *byte_ptr = NULL;

  int i = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set the ISR timer to the character waiting time */
  uim_work_waiting_time[uim_drv_slot] = uim_t_1_cwt;

  /* Check if there is room in the Tx FIFO */
  if ( UIM_READ_MISR() & MSMU_ISR_TXLEV )
  {
    /* Clear the TX_READY to write into NO_CHARS_FOR_TX register */
    UIM_CLEAR_TX_READY_INT();

    byte_ptr = request_buffer_ptr[uim_drv_slot];

    /* Keep track of EDC */
    if (UIM_T_1_EDC_LRC == uim_t_1_edc)
    {
      for(i = 0; i< num_bytes_to_send[uim_drv_slot]; i++)
      {
        uim_tx_epilogue_field.lrc ^= byte_ptr[i];
      }
    }
    else /* The EDC is CRC */
    {
      for(i = 0; i< num_bytes_to_send[uim_drv_slot]; i++)
      {
        /* Compute the CRC with this byte */
        uim_tx_epilogue_field.crc =
          crc_16_l_table[ (uim_tx_epilogue_field.crc ^
                           byte_ptr[i]) & 0x00ff ]
           ^ (uim_tx_epilogue_field.crc >> 8);
      }
    } /* end if - the EDC is LRC */

    /* Program the NO_CHARS_FOR_TX register with the size of TX bytes
       to be sent */
    UIM_NUM_CHARS_TO_SEND(num_bytes_to_send[uim_drv_slot]);

    /* Write the TX bytes into the TX FIFO */
    uim_write_to_uart(request_buffer_ptr[uim_drv_slot], num_bytes_to_send[uim_drv_slot]);

    num_bytes_to_send[uim_drv_slot] = 0;

    /* Determine if this is the last byte for this buffer */
    if (num_bytes_to_send[uim_drv_slot] == 0)
    {
      /* Determine if there is an information field to send */
      if (num_cmd_data_bytes_to_send[uim_drv_slot] != 0)
      {
        /* Send the information field */
        num_bytes_to_send[uim_drv_slot] =
          num_cmd_data_bytes_to_send[uim_drv_slot];
        request_buffer_ptr[uim_drv_slot] = apdu_command_data_ptr[uim_drv_slot];
        uim_rx_state[uim_drv_slot] = UIM_RX_T_1_SEND_INFO_ST;
      }
      else /* no information field to send out */
      {
        /* Send the epilogue field */
        num_bytes_to_send[uim_drv_slot] = uim_epilogue_field_size;
        /* Determine if CRC is used for EDC */
        if (UIM_T_1_EDC_CRC == uim_t_1_edc)
        {
          /* Complement the CRC field */
          uim_tx_epilogue_field.crc = ~uim_tx_epilogue_field.crc;
        } /* end if - check if we are using CRC for EDC */
        request_buffer_ptr[uim_drv_slot] = (byte *)&uim_tx_epilogue_field;
        uim_rx_state[uim_drv_slot] = UIM_RX_T_1_SEND_EPILOGUE_ST;
      } /* end if - there is an information field to send */
      /* Exit this loop */
      return;
    }
  } /* end while - fill the Tx FIFO while there is room */
} /* end rx_isr_send_prologue_dm */

/*===========================================================================

FUNCTION RX_ISR_RECV_T_1_BLOCK                      INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called while receiving a T=1 block.

DEPENDENCIES
  This function is called from the receive ISR while receiving a T=1 block.

RETURN VALUE
  None

SIDE EFFECTS
  This function may receive the T=1 block in one chunk or in instalments.

===========================================================================*/
LOCAL void rx_isr_recv_t_1_block
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{

  /* Junk code at this moment */
  uint32 rx_word;
  byte  rx_value_array[4];
  int i = 0; /* Running index */
  boolean reinit_rxtransfer = TRUE;
  dword uart_status;  /* Holds the UART status */

  /* A temporary variable to store the bytes to be read */
  int32 bytes_to_read = 0;

  /* A boolean variable to indicate if stale received */
  boolean stale_rcvd = FALSE;

  if (UIM_READ_MISR() & MSMU_IMR_RXLEV)
  {
    /* Increment the RXLEV interrupt count */
    uim_rxlev_count += 1;
  }


  /* Give RXSTALE the high priority when both RXSTALE and RXLEV
     happened at the same time */
  if(UIM_READ_MISR() & MSMU_IMR_RXSTALE)
  {
    stale_rcvd = TRUE;

    /* Clear the RX STALE interrupt */
    UIM_CLEAR_RX_STALE_INT();

    /* Read only the unread bytes from the UART_DM */
    bytes_to_read = UIM_GET_NUM_BYTES_IN_RX() - num_bytes_received;

    num_bytes_received = 0;
  }
  else if(UIM_READ_MISR() & MSMU_IMR_RXLEV)
  {
    num_bytes_received += (4 * MSMU_DEF_RFWR_VAL);
    bytes_to_read = 4 * MSMU_DEF_RFWR_VAL;
  }

  /* Read the Status register */
  uart_status = UIM_READ_STATUS();

  /* Check if there is a framing error or a parity error */
  if( uart_status & MSMU_SR_PF_ERR )
  {
    /* Indicate APDU result */
    resp_buf[uim_drv_slot]->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
  }

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /* Make sure there is a word in the receive FIFO */
  for (uart_status = UIM_READ_STATUS();
      (uart_status & MSMU_SR_RXRDY) && (bytes_to_read > 0) ;
     uart_status = UIM_READ_STATUS())
  {
    if(bytes_to_read >0)
    {
      rx_word  = UIM_GET_RX_WORD();
      rx_value_array[0] = rx_word & 0xFF;
      rx_value_array[1] = (rx_word & 0xFF00)     >> 8;
      rx_value_array[2] = (rx_word & 0xFF0000)   >> 16;
      rx_value_array[3] = (rx_word & 0xFF000000) >> 24;

      /* Process each byte in the received word */
      for ( i = 0; i < 4 && bytes_to_read > 0; i++)
      {
        bytes_to_read -= 1;

        /* Sub state waiting for PROLOGUE bytes */
        if(uim_rx_sub_state == UIM_RX_T_1_RX_PROLOGUE_ST)
        {
          /* Reset the waiting time extension */
          uim_t_1_wtx = 1;
          /* Default command status is a successful command */
          resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;

          /* Set the ISR timer to the character waiting time */
          uim_work_waiting_time[uim_drv_slot] = uim_t_1_cwt;

          /* Make sure there is no problem with the received byte */
          if (uart_status & MSMU_SR_PF_ERR)
          {
            /* Indicate APDU result */
            resp_buf[uim_drv_slot]->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
          }
          else /* no parity error */
          {
            /* Process error detection code for this byte */
            if (UIM_T_1_EDC_LRC == uim_t_1_edc)
            {
              uim_error_detection_code ^= rx_value_array[i];
            }
            else /* The EDC is CRC */
            {
              /* Compute the CRC with this byte */
              uim_error_detection_code =
                crc_16_l_table[ (uim_error_detection_code ^ rx_value_array[i]) & 0x00ff ]
                 ^ (uim_error_detection_code >> 8);
            } /* The EDC is LRC */
          } /* end if - check for parity error. */

          /* Place the byte in the prologue field buffer. */
          uim_rx_prologue_field_buffer_ptr[num_resp_bytes_rcvd[uim_drv_slot]] = rx_value_array[i];

          /* Keep track of the number of bytes received. */
          num_resp_bytes_rcvd[uim_drv_slot]++;

          /* Determine if all the expected bytes have been received. */
          if (num_resp_bytes_rcvd[uim_drv_slot] == UIM_T_1_NUM_BYTES_PROLOGUE)
          {
            /* Initialize the number of bytes received */
            num_resp_bytes_rcvd[uim_drv_slot] = 0;

            /* Let us receive the entire APDU but throw it away and request
             * a re-transmission by indicating a failure
             */
            if (uim_rx_prologue_field_buffer.len > uim_t_1_ifsd)
            {
              /* Indicate APDU result */
              resp_buf[uim_drv_slot]->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
            }

            /* determine if there are any information bytes */
            if (uim_rx_prologue_field_buffer.len > 0)
            {
              /* Prepare to receive the information field */
              uim_rx_sub_state = UIM_RX_T_1_RX_INFO_ST;
              num_resp_data_bytes_expected[uim_drv_slot] =
                uim_rx_prologue_field_buffer.len;
            }
            else /* no information bytes are expected */
            {
              /* Prepare to receive the epilogue field */
              uim_rx_sub_state = UIM_RX_T_1_RX_EPILOGUE_ST;

              /* Place the number of bytes read in the response buffer */
              resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

              /* Determine if CRC is used for EDC */
              if (UIM_T_1_EDC_CRC == uim_t_1_edc)
              {
                /* Complement the CRC field */
                uim_error_detection_code = ~uim_error_detection_code;
              } /* end if - check if we are using CRC for EDC */
            } /* end if - There are information bytes to process */
            continue;
          } /* end if - all the expected bytes have been received */
        }

        /* Sub state waiting for INFORMATION bytes */
        if(uim_rx_sub_state == UIM_RX_T_1_RX_INFO_ST)
        {
          /* Make sure there is no problem with the received byte */
          if (uart_status & MSMU_SR_PF_ERR)
          {
            /* Indicate APDU result */
            resp_buf[uim_drv_slot]->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
          }
          else /* no parity error */
          {
            /* Process error detection code for this byte */
            if (UIM_T_1_EDC_LRC == uim_t_1_edc)
            {
              uim_error_detection_code ^= rx_value_array[i];
            }
            else /* The EDC is CRC */
            {
              /* Compute the CRC with this byte */
              uim_error_detection_code =
                crc_16_l_table[ (uim_error_detection_code ^ rx_value_array[i]) & 0x00ff ]
                 ^ (uim_error_detection_code >> 8);
            } /* The EDC is LRC */
          } /* end if - check for parity error. */

          /* Place the byte in the response buffer. */
          *resp_buffer_ptr[uim_drv_slot]++ = rx_value_array[i];

          /* Keep track of the number of bytes received. */
          num_resp_bytes_rcvd[uim_drv_slot]++;

          /* Determine if all the expected bytes have been received. */
          if (num_resp_bytes_rcvd[uim_drv_slot] ==
              num_resp_data_bytes_expected[uim_drv_slot])
          {
            /* Set the number of bytes received in the response buffer */
            resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd[uim_drv_slot];

            /* Initialize the number of bytes received */
            num_resp_bytes_rcvd[uim_drv_slot] = 0;

            /* Prepare to receive the epilogue field */
            uim_rx_sub_state = UIM_RX_T_1_RX_EPILOGUE_ST;

            /* Determine if CRC is used for EDC */
            if (UIM_T_1_EDC_CRC == uim_t_1_edc)
            {
              /* Complement the CRC field */
              uim_error_detection_code = ~uim_error_detection_code;
            } /* end if - check if we are using CRC for EDC */
            continue;
          } /* end if - all the expected bytes have been received */
        }

        /* Sub state waiting for EPILOGUE bytes */
        if(uim_rx_sub_state == UIM_RX_T_1_RX_EPILOGUE_ST)
        {
          /* Make sure there is no problem with the received byte */
          if (uart_status & MSMU_SR_PF_ERR)
          {
            /* Indicate APDU result */
            resp_buf[uim_drv_slot]->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
          } /* end if - check for parity error. */

          /* Place the byte in the epilogue field buffer. */
          ((byte *)&uim_tx_epilogue_field)[num_resp_bytes_rcvd[uim_drv_slot]] =
            rx_value_array[i];

          /* Keep track of the number of bytes received. */
          num_resp_bytes_rcvd[uim_drv_slot]++;

          /* Determine if all the expected bytes have been received. */
          if (num_resp_bytes_rcvd[uim_drv_slot] == uim_epilogue_field_size)
          {
            /* Check the error detection code */
            if (uim_error_detection_code != uim_tx_epilogue_field.crc)
            {
              /* Indicate APDU result */
              resp_buf[uim_drv_slot]->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
            } /* end if - Check the error detection result */

            /* Change to idle */
            uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
            uim_rx_sub_state = UIM_RX_PROCESS_IDLE;

            reinit_rxtransfer = FALSE;

            /* Check if stale happened */
            if (UIM_READ_MISR() & MSMU_IMR_RXSTALE)
            {
              /* Clear the stale interrupt */
              UIM_CLEAR_RX_STALE_INT();
            }

            /* Disable RXLEV and RXSTALE */
            UIM_PRG_IMR ( MSMU_IMR_RXBREAK );

            /* Reset the UART receiver only if there is a comunication error
               and that too after all the bytes are received */
            if( resp_buf[uim_drv_slot]->cmd_status ==
                UIM_T_1_BLOCK_RX_FAILURE )
            {
              /* Clear the statuses and the RX FIFO in UART */
              uim_reset_receiver();
            }

            /* Call the response callback function */
            apdu_resp_callback_func[uim_drv_slot]();

          } /* end if - all the expected bytes have been received */
        }
      }
    }
  }

  /* If end of data event occurred and still some more data expected */
  if( reinit_rxtransfer == TRUE )
  {
      reinit_rxtransfer = FALSE;

      /* This stale event might have happened while we were processing the
         RXLEV interrupt. In that case, we would like to preserve it. */
      if (UIM_READ_MISR() & MSMU_IMR_RXSTALE)
      {
        return;
      }

      if ( (TRUE == stale_rcvd) ||
           /* A case when RXSTALE did not happened when
              we had continuous RXLEVs happening and for the remaining data
              (whose size is less than RFWR) the RXSTALE did not happen.
              Since check solves the problem but need to still find the
              root-cause and fix it appropriately. */
           (uim_rxlev_count == ( (num_resp_data_bytes_expected[uim_drv_slot] + 1 + 2)/4 ) )
         )
      {

        /* Since we are going to re-init a new RX data transfer,
           initialize this variable to 0. */
        num_bytes_received = 0;

        /* Re-initialize a new RX data transfer */
        UIM_DM_RX_INIT(MSMU_DEF_RX_INIT_VAL);

        /* Enable Stale event */
        UIM_ENABLE_RX_STALE_INT();
      }
  }


  return;

} /* end rx_isr_recv_t_1_block */
#endif /* FEATURE_UIM_UART_DM */

/*===========================================================================

FUNCTION RX_ISR_SEND_INFORMATION                       INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called while sending the information field of a T=1 block.

DEPENDENCIES
  This function is called from the receive ISR while sending the information
  field of a T=1 block.

RETURN VALUE
  None

SIDE EFFECTS
  This function sends the information field out in order and changes state
  when the information field has been sent out.  Also, the appropriate error
  detection code is computed over the sent bytes.

===========================================================================*/
LOCAL void rx_isr_send_information
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  /* This holds the byte read from the receive FIFO */
  volatile byte rx_value;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check if there is room in the Tx FIFO */
  //while (( misr_status = UIM_READ_MISR()) & MSMU_ISR_TXLEV )
  while (UIM_READ_MISR() & MSMU_ISR_TXLEV )
  {
    /* Keep track of EDC */
    if (UIM_T_1_EDC_LRC == uim_t_1_edc)
    {
      uim_tx_epilogue_field.lrc ^= *request_buffer_ptr[uim_drv_slot];
    }
    else /* The EDC is CRC */
    {
      /* Compute the CRC with this byte */
      uim_tx_epilogue_field.crc =
        crc_16_l_table[ (uim_tx_epilogue_field.crc ^
                         *request_buffer_ptr[uim_drv_slot]) & 0x00ff ]
         ^ (uim_tx_epilogue_field.crc >> 8);
    } /* end if - the EDC is LRC */

    /* Send the next byte */
    uim_send_t_1_byte (*request_buffer_ptr[uim_drv_slot]);

    /* Determine if this is the last byte for this buffer */
    if (--num_bytes_to_send[uim_drv_slot] == 0)
    {
      /* Send the epilogue field */
      num_bytes_to_send[uim_drv_slot] = uim_epilogue_field_size;
      /* Determine if CRC is used for EDC */
      if (UIM_T_1_EDC_CRC == uim_t_1_edc)
      {
        /* Complement the CRC field */
        uim_tx_epilogue_field.crc = ~uim_tx_epilogue_field.crc;
      } /* end if - check if we are using CRC for EDC */
      request_buffer_ptr[uim_drv_slot] = (byte *)&uim_tx_epilogue_field;
      uim_rx_state[uim_drv_slot] = UIM_RX_T_1_SEND_EPILOGUE_ST;
      /* Exit the loop */
      return;
    }
    else /* there are more information bytes to send */
    {
      /* Send the next prologue byte */
      request_buffer_ptr[uim_drv_slot]++;

      /* Clear out the Rx FIFO */
      //if (( uart_status = UIM_READ_STATUS()) & MSMU_SR_RXRDY )
      if (UIM_READ_STATUS() & MSMU_SR_RXRDY )
      {
        /* Keep track of the number of transmitted bytes removed from
           the Rx FIFO */
        uim_num_txd_bytes--;
        /* Get the transmitted byte */
        rx_value = UIM_GET_RX_BYTE ();
      } /* end if - read bytes in the receive FIFO */
    } /* end if - this is the last byte of the current buffer */
  } /* end while - fill the Tx FIFO while there is room */
} /* end rx_isr_send_information */

#ifdef FEATURE_UIM_UART_DM
/*===========================================================================

FUNCTION RX_ISR_SEND_INFORMATION_DM                       INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called while sending the information field of a T=1 block.

DEPENDENCIES
  This function is called from the receive ISR while sending the information
  field of a T=1 block.

RETURN VALUE
  None

SIDE EFFECTS
  This function sends the information field out in order and changes state
  when the information field has been sent out.  Also, the appropriate error
  detection code is computed over the sent bytes.

===========================================================================*/
LOCAL void rx_isr_send_information_dm
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  unsigned char *byte_ptr = NULL;

  int i = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check if there is room in the Tx FIFO */
  if ( UIM_READ_MISR() & MSMU_ISR_TX_READY )
  {

    /* Clear the TX_READY to write into NO_CHARS_FOR_TX register */
    UIM_CLEAR_TX_READY_INT();

    byte_ptr = request_buffer_ptr[uim_drv_slot];

    /* Keep track of EDC */
    if (UIM_T_1_EDC_LRC == uim_t_1_edc)
    {
      for(i = 0; i< num_bytes_to_send[uim_drv_slot]; i++)
      {
        uim_tx_epilogue_field.lrc ^= byte_ptr[i];
      }
    }
    else /* The EDC is CRC */
    {
      for(i = 0; i< num_bytes_to_send[uim_drv_slot]; i++)
      {
        /* Compute the CRC with this byte */
        uim_tx_epilogue_field.crc =
          crc_16_l_table[ (uim_tx_epilogue_field.crc ^
                           byte_ptr[i]) & 0x00ff ]
           ^ (uim_tx_epilogue_field.crc >> 8);
      }
    } /* end if - the EDC is LRC */

    /* Program the NO_CHARS_FOR_TX register with the size of TX bytes
       to be sent */
    UIM_NUM_CHARS_TO_SEND(num_bytes_to_send[uim_drv_slot]);

    /* Write the TX bytes into the TX FIFO */
    uim_write_to_uart(request_buffer_ptr[uim_drv_slot], num_bytes_to_send[uim_drv_slot]);

    num_bytes_to_send[uim_drv_slot] = 0;

    /* Determine if this is the last byte for this buffer */
    if (num_bytes_to_send[uim_drv_slot] == 0)
    {
      /* Send the epilogue field */
      num_bytes_to_send[uim_drv_slot] = uim_epilogue_field_size;
      /* Determine if CRC is used for EDC */
      if (UIM_T_1_EDC_CRC == uim_t_1_edc)
      {
        /* Complement the CRC field */
        uim_tx_epilogue_field.crc = ~uim_tx_epilogue_field.crc;
      } /* end if - check if we are using CRC for EDC */
      request_buffer_ptr[uim_drv_slot] = (byte *)&uim_tx_epilogue_field;
      uim_rx_state[uim_drv_slot] = UIM_RX_T_1_SEND_EPILOGUE_ST;
      /* Exit the loop */
      return;
    }
  } /* end while - fill the Tx FIFO while there is room */
} /* end rx_isr_send_information_dm */
#endif /* FEATURE_UIM_UART_DM */

/*===========================================================================

FUNCTION RX_ISR_SEND_EPILOGUE                       INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called while sending the epilogue field of a T=1 block.

DEPENDENCIES
  This function is called from the receive ISR while sending the epilogue
  field of a T=1 block.

RETURN VALUE
  None

SIDE EFFECTS
  This function sends the epilogue field out in order and changes state
  when the epilogue field has been sent out.

===========================================================================*/
LOCAL void rx_isr_send_epilogue
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  /* This holds the byte read from the receive FIFO */
  volatile byte rx_value;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check if there is room in the Tx FIFO */
  //while (( misr_status = UIM_READ_MISR()) & MSMU_ISR_TXLEV )
  while (UIM_READ_MISR() & MSMU_ISR_TXLEV )
  {
    /* Send the next byte */
    uim_send_t_1_byte (*request_buffer_ptr[uim_drv_slot]);

    /* Determine if this is the last byte for this buffer */
    if (--num_bytes_to_send[uim_drv_slot] == 0)
    {
      /* Done sending the block.  Mask the Tx interrupt */
      UIM_PRG_IMR ( (MSMU_IMR_RXLEV | MSMU_IMR_RXSTALE | MSMU_IMR_RXBREAK) );

      /* Receive the prologue field from the card */
      num_resp_bytes_rcvd[uim_drv_slot] = 0;
      uim_rx_state[uim_drv_slot] = UIM_RX_T_1_RX_PROLOGUE_ST;
      /* Initialize the EDC fields */
      if (UIM_T_1_EDC_LRC == uim_t_1_edc)
      {
        uim_error_detection_code = 0;
        /* Initialize the crc field as it initializes both bytes of the EDC
           field including the lrc field */
        uim_tx_epilogue_field.crc = 0;
      }
      else /* The EDC is CRC */
      {
        uim_error_detection_code = CRC_16_L_SEED;
        uim_tx_epilogue_field.crc = 0;
      } /* The EDC is LRC */
      /* Exit this loop */
      return;
    }
    else /* there are more epilogue bytes to send */
    {
      /* Send the next prologue byte */
      request_buffer_ptr[uim_drv_slot]++;

      /* Clear out the Rx FIFO */
      //if (( uart_status = UIM_READ_STATUS()) & MSMU_SR_RXRDY )
      if (UIM_READ_STATUS() & MSMU_SR_RXRDY )
      {
        /* Keep track of the number of transmitted bytes removed from
           the Rx FIFO */
        uim_num_txd_bytes--;
        /* Get the transmitted byte */
        rx_value = UIM_GET_RX_BYTE ();
      } /* end if - read bytes in the receive FIFO */
    } /* end if - this is the last byte of the current buffer */
  } /* end while - fill the Tx FIFO while there is room */
} /* end rx_isr_send_epilogue */

#ifdef FEATURE_UIM_UART_DM
/*===========================================================================

FUNCTION RX_ISR_SEND_EPILOGUE_DM                    INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called while sending the epilogue field of a T=1 block.

DEPENDENCIES
  This function is called from the receive ISR while sending the epilogue
  field of a T=1 block.

RETURN VALUE
  None

SIDE EFFECTS
  This function sends the epilogue field out in order and changes state
  when the epilogue field has been sent out.

===========================================================================*/
LOCAL void rx_isr_send_epilogue_dm
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  unsigned char *byte_ptr = NULL;

  /* Initialize the RXLEV count for every APDU transaction */
  uim_rxlev_count = 0;

  /* Reset the RX byte count to zero */
  num_bytes_received = 0;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Check if there is room in the Tx FIFO */
  if ( UIM_READ_MISR() & MSMU_ISR_TX_READY )
  {
    /* Clear the TX_READY to write into NO_CHARS_FOR_TX register */
    UIM_CLEAR_TX_READY_INT();

    byte_ptr = request_buffer_ptr[uim_drv_slot];

    /* Program the NO_CHARS_FOR_TX register with the size of TX bytes
       to be sent */
    UIM_NUM_CHARS_TO_SEND(num_bytes_to_send[uim_drv_slot]);

    /* Write the TX bytes into the TX FIFO */
    uim_write_to_uart(request_buffer_ptr[uim_drv_slot], num_bytes_to_send[uim_drv_slot]);

    num_bytes_to_send[uim_drv_slot] = 0;

    /* Determine if this is the last byte for this buffer */
    if (num_bytes_to_send[uim_drv_slot] == 0)
    {
      /* Done sending the block.  Mask the Tx interrupt */

      /* Initialize the a new RX data transfer of size MSMU_DEF_RX_INIT_VAL */
      UIM_DM_RX_INIT(MSMU_DEF_RX_INIT_VAL);

      /* Program the RX water mark level */
      UIM_RX_WATERLEVEL( MSMU_DEF_RFWR_VAL );

      /* Program the stale timeout value */
      UIM_PRG_IPR(MSMU_IPR_DEF_STALE_TIMEOUT);

      /* Enable the RXLEV, RXSTALE and RXBREAK interrupts */
      UIM_PRG_IMR ( ( MSMU_IMR_RXLEV | MSMU_IMR_RXSTALE | MSMU_IMR_RXBREAK) );

      /* Enable Stale event */
      UIM_ENABLE_RX_STALE_INT();

      /* Receive the prologue field from the card */
      num_resp_bytes_rcvd[uim_drv_slot] = 0;
      uim_rx_state[uim_drv_slot] = UIM_RX_T_1_RECV_T_1_BLOCK_DM;
      uim_rx_sub_state = UIM_RX_T_1_RX_PROLOGUE_ST;
      /* Initialize the EDC fields */
      if (UIM_T_1_EDC_LRC == uim_t_1_edc)
      {
        uim_error_detection_code = 0;
        /* Initialize the crc field as it initializes both bytes of the EDC
           field including the lrc field */
        uim_tx_epilogue_field.crc = 0;
      }
      else /* The EDC is CRC */
      {
        uim_error_detection_code = CRC_16_L_SEED;
        uim_tx_epilogue_field.crc = 0;
      } /* The EDC is LRC */
      /* Exit this loop */
      return;
    }
  } /* end while - fill the Tx FIFO while there is room */
} /* end rx_isr_send_epilogue_dm */

#endif /* FEATURE_UIM_UART_DM */

/*===========================================================================

FUNCTION RX_ISR_RX_PROLOGUE                       INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called while receiving the prologue field of a T=1 block.

DEPENDENCIES
  This function is called from the receive ISR while receiving the prologue
  field of a T=1 block.

RETURN VALUE
  None

SIDE EFFECTS
  This function receives the prologue field and changes state when the
  prologue field has been sent fully received.

===========================================================================*/
LOCAL void rx_isr_rx_prologue
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  dword uart_status;  /* Holds the UART status */
  /* This holds the byte read from the receive FIFO */
  byte rx_value;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is a byte in the receive FIFO */
  for (uart_status = UIM_READ_STATUS();
       (uart_status & MSMU_SR_RXRDY);
       uart_status = UIM_READ_STATUS())
  {
    /* First, makes sure to read all the bytes that were transmitted */
    if (0 != uim_num_txd_bytes)
    {
      /* Keep track of the number of transmitted bytes removed from
         the Rx FIFO */
      uim_num_txd_bytes--;
      /* Get the transmitted byte */
      rx_value = UIM_GET_RX_BYTE ();
      /* Check if this is the last byte transmitted */
      if (0 == uim_num_txd_bytes)
      {
        /* Set the ISR timer to the block waiting time */
        uim_work_waiting_time[uim_drv_slot] = uim_t_1_bwt * uim_t_1_wtx;
        /* Reset the waiting time extension */
        uim_t_1_wtx = 1;
        /* Default command status is a successful command */
        resp_buf[uim_drv_slot]->cmd_status = UIM_DONE;
      } /* end if - this is the last byte transmitted of the block */
    }
    else
    {
      /* Set the ISR timer to the character waiting time */
      uim_work_waiting_time[uim_drv_slot] = uim_t_1_cwt;
      /* Get the next received byte. */
      rx_value = UIM_GET_RX_BYTE ();
#ifdef FEATURE_UIM_DEBUG_LOG
      UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, rx_value);
#endif

      /* Make sure there is no problem with the received byte */
      if (uart_status & MSMU_SR_PF_ERR)
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
      }
      else /* no parity error */
      {
        /* Process error detection code for this byte */
        if (UIM_T_1_EDC_LRC == uim_t_1_edc)
        {
          uim_error_detection_code ^= rx_value;
        }
        else /* The EDC is CRC */
        {
          /* Compute the CRC with this byte */
          uim_error_detection_code =
            crc_16_l_table[ (uim_error_detection_code ^ rx_value) & 0x00ff ]
             ^ (uim_error_detection_code >> 8);
        } /* The EDC is LRC */
      } /* end if - check for parity error. */

      /* Place the byte in the prologue field buffer. */
      uim_rx_prologue_field_buffer_ptr[num_resp_bytes_rcvd[uim_drv_slot]] =
        rx_value;

      /* Keep track of the number of bytes received. */
      num_resp_bytes_rcvd[uim_drv_slot]++;

      /* Determine if all the expected bytes have been received. */
      if (num_resp_bytes_rcvd[uim_drv_slot] == UIM_T_1_NUM_BYTES_PROLOGUE)
      {
        /* Initialize the number of bytes received */
        num_resp_bytes_rcvd[uim_drv_slot] = 0;

        /* Let us receive the entire APDU but throw it away and request
         * a re-transmission by indicating a failure
         */
        if (uim_rx_prologue_field_buffer.len > uim_t_1_ifsd)
        {
          /* Indicate APDU result */
          resp_buf[uim_drv_slot]->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
        }

        /* determine if there are any information bytes */
        if (uim_rx_prologue_field_buffer.len > 0)
        {
          /* Prepare to receive the information field */
          uim_rx_state[uim_drv_slot] = UIM_RX_T_1_RX_INFO_ST;
          num_resp_data_bytes_expected[uim_drv_slot] =
            uim_rx_prologue_field_buffer.len;
        }
        else /* no information bytes are expected */
        {
          /* Prepare to receive the epilogue field */
          uim_rx_state[uim_drv_slot] = UIM_RX_T_1_RX_EPILOGUE_ST;

          /* Place the number of bytes read in the response buffer */
          resp_buf[uim_drv_slot]->cmd_rsp_size = 0;

          /* Determine if CRC is used for EDC */
          if (UIM_T_1_EDC_CRC == uim_t_1_edc)
          {
            /* Complement the CRC field */
            uim_error_detection_code = ~uim_error_detection_code;
          } /* end if - check if we are using CRC for EDC */
        } /* end if - There are information bytes to process */
        /* Stop processing the receive fifo */
        return;
      } /* end if - all the expected bytes have been received */
    } /* end if - read the bytes that were transmitted */
  } /* end while - receive characters to process. */
} /* end rx_isr_rx_prologue */


/*===========================================================================

FUNCTION RX_ISR_RX_INFORMATION                       INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called while receiving the information field of a T=1 block.

DEPENDENCIES
  This function is called from the receive ISR while receiving the information
  field of a T=1 block.

RETURN VALUE
  None

SIDE EFFECTS
  This function receives the information field and changes state when the
  information field has been fully received.

===========================================================================*/
LOCAL void rx_isr_rx_information
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  dword uart_status;  /* Holds the UART status */

  /* This holds the byte read from the receive FIFO */
  byte rx_value;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is a byte in the receive FIFO */
  for (uart_status = UIM_READ_STATUS();
       (uart_status & MSMU_SR_RXRDY);
       uart_status = UIM_READ_STATUS())
  {
    /* Get the next received byte. */
    rx_value = UIM_GET_RX_BYTE ();
#ifdef FEATURE_UIM_DEBUG_LOG
    UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, rx_value);
#endif

    /* Make sure there is no problem with the received byte */
    if (uart_status & MSMU_SR_PF_ERR)
    {
      /* Indicate APDU result */
      resp_buf[uim_drv_slot]->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
    }
    else /* no parity error */
    {
      /* Process error detection code for this byte */
      if (UIM_T_1_EDC_LRC == uim_t_1_edc)
      {
        uim_error_detection_code ^= rx_value;
      }
      else /* The EDC is CRC */
      {
        /* Compute the CRC with this byte */
        uim_error_detection_code =
          crc_16_l_table[ (uim_error_detection_code ^ rx_value) & 0x00ff ]
           ^ (uim_error_detection_code >> 8);
      } /* The EDC is LRC */
    } /* end if - check for parity error. */

    /* Place the byte in the response buffer. */
    *resp_buffer_ptr[uim_drv_slot]++ = rx_value;

    /* Keep track of the number of bytes received. */
    num_resp_bytes_rcvd[uim_drv_slot]++;

    /* Determine if all the expected bytes have been received. */
    if (num_resp_bytes_rcvd[uim_drv_slot] ==
        num_resp_data_bytes_expected[uim_drv_slot])
    {
      /* Set the number of bytes received in the response buffer */
      resp_buf[uim_drv_slot]->cmd_rsp_size = num_resp_bytes_rcvd[uim_drv_slot];

      /* Initialize the number of bytes received */
      num_resp_bytes_rcvd[uim_drv_slot] = 0;

      /* Prepare to receive the epilogue field */
      uim_rx_state[uim_drv_slot] = UIM_RX_T_1_RX_EPILOGUE_ST;

      /* Determine if CRC is used for EDC */
      if (UIM_T_1_EDC_CRC == uim_t_1_edc)
      {
        /* Complement the CRC field */
        uim_error_detection_code = ~uim_error_detection_code;
      } /* end if - check if we are using CRC for EDC */

      /* Stop processing the receive fifo */
      return;
    } /* end if - all the expected bytes have been received */
  } /* end while - receive characters to process. */
} /* end rx_isr_rx_information */


/*===========================================================================

FUNCTION RX_ISR_RX_EPILOGUE                       INTERNAL FUNCTION

DESCRIPTION
  This function runs within the context of the Rx ISR of the UART.  This
  function is called while receiving the epilogue field of a T=1 block.

DEPENDENCIES
  This function is called from the receive ISR while receiving the epilogue
  field of a T=1 block.

RETURN VALUE
  None

SIDE EFFECTS
  This function receives the epilogue field and changes state when the
  epilogue field has been fully received.

===========================================================================*/
LOCAL void rx_isr_rx_epilogue
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  dword uart_status;  /* Holds the UART status */

  /* This holds the byte read from the receive FIFO */
  byte rx_value;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Make sure there is a byte in the receive FIFO */
  for (uart_status = UIM_READ_STATUS();
       (uart_status & MSMU_SR_RXRDY);
       uart_status = UIM_READ_STATUS())
  {
    /* Get the next received byte. */
    rx_value = UIM_GET_RX_BYTE ();
#ifdef FEATURE_UIM_DEBUG_LOG
    UIM_LOG_PUT_BYTE(UIM_LOG_RX_DATA, rx_value);
#endif

    /* Make sure there is no problem with the received byte */
    if (uart_status & MSMU_SR_PF_ERR)
    {
      /* Indicate APDU result */
      resp_buf[uim_drv_slot]->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
    } /* end if - check for parity error. */

    /* Place the byte in the epilogue field buffer. */
    ((byte *)&uim_tx_epilogue_field)[num_resp_bytes_rcvd[uim_drv_slot]] =
      rx_value;

    /* Keep track of the number of bytes received. */
    num_resp_bytes_rcvd[uim_drv_slot]++;

    /* Determine if all the expected bytes have been received. */
    if (num_resp_bytes_rcvd[uim_drv_slot] == uim_epilogue_field_size)
    {
      /* Check the error detection code */
      if (uim_error_detection_code != uim_tx_epilogue_field.crc)
      {
        /* Indicate APDU result */
        resp_buf[uim_drv_slot]->cmd_status = UIM_T_1_BLOCK_RX_FAILURE;
      } /* end if - Check the error detection result */

      /* Change to idle */
      uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

      /* Call the response callback function */
      apdu_resp_callback_func[uim_drv_slot]();

      /* Stop processing the receive fifo */
      return;
    } /* end if - all the expected bytes have been received */
  } /* end while - receive characters to process. */
} /* end rx_isr_rx_epilogue */

#endif /* FEATURE_UIM_T_1_SUPPORT */



/*===========================================================================

FUNCTION UIM_RX_ISR                             INTERNAL FUNCTION

DESCRIPTION
  This is the Rx ISR for the UART used to communicate with the UIM.  This ISR
  handles ALL (receive and transmit) operations of the UIM interface.  It
  handles transmitting because the Rx and Tx lines are shared.  The receiver
  is placed in sample mode while transmitting.  This allows the ISR to handle
  the re-transmit scheme of the UIM.

DEPENDENCIES
  This ISR works in conjunction with a number of static variables.  These
  variables control the operation of the ISR.  Care must be taken when
  changing any of these variables.

RETURN VALUE
  None

SIDE EFFECTS
  This ISR services the UART interrupts.  After servicing the UART hardware,
  the UART hardware interrupt status is reset.

===========================================================================*/
LOCAL void uim_rx_isr
(
  uim_drv_slot_type uim_drv_slot  /* Slot control variable */
)
{
  dword uart_status;  /* Holds the UART status */
  dword uart_misr_status; /* Holds the UART ISR status */

#ifdef UIM_DRIVER_TIMESTAMP
  /* Variables to hold the timestamp of the last byte.  Declared static because
   * we will have to re-enter the function for the current time stamp
   */
  static qword last_byte_time_stamp;
#endif /* UIM_DRIVER_TIMESTAMP */

  /* Do not process the interrupt if the clock to the UART is already
   * turned off
   */
  if ( !UIM_IS_UIM_CLK_ON() )
  {
    return;
  }

#ifdef UIM_DRIVER_TIMESTAMP

  if (uim_UseDriverTimeStamp)
  {
    /* Variables to hold the timestamp of the current byte*/
    qword curr_byte_time_stamp;

    /* Variable to hold the difference between the timestamps */
    qword time_stamp_difference;

    /* Variables to hold the modulo 1.25 ms of the last and current bytes */
    word mod_125_ms_elapsed_last_byte;
    word mod_125_ms_elapsed_curr_byte;


#ifdef FEATURE_UIM_T_1_SUPPORT
    if ( UIM_T_0_PROTOCOL == uim_operating_protocol )
    {
#endif /* FEATURE_UIM_T_1_SUPPORT */
      if (uim_is_timer_on)
      {
        ts_get(curr_byte_time_stamp);

        /* get the modulo 1.25 ms returned from ts_get for the current byte*/
        mod_125_ms_elapsed_curr_byte =
          qw_lo(curr_byte_time_stamp) & 0xffff;

        /* get the modulo 1.25 ms returned from ts_get for the last byte*/
        mod_125_ms_elapsed_last_byte =
          qw_lo(last_byte_time_stamp) & 0xffff;

        /* get the 1.25 ms counters out from the time stamp*/
        qw_shift(curr_byte_time_stamp, -16);
        qw_shift(last_byte_time_stamp, -16);

        /* Get the difference in the 1.25 ms between the two time stamps */
        qw_sub(time_stamp_difference, curr_byte_time_stamp ,
                last_byte_time_stamp);

        /* Calculate the difference in sleep_ticks */
        uim_num_sleep_ticks_elapsed = qw_lo(time_stamp_difference);

        if (uim_num_sleep_ticks_elapsed < 0 )
        {
          uim_num_sleep_ticks_elapsed = 0;
        }
        else
        {
          uim_num_sleep_ticks_elapsed *= TS_SLEEP_TMR_0_CNT_125MS;
        }

        if (uim_num_sleep_ticks_elapsed == 0)
        {
          /* we are in the same mod 1.25 counter value */
          /*the difference is the diff in the lows*/
          if (mod_125_ms_elapsed_curr_byte >= mod_125_ms_elapsed_last_byte)
          {
             uim_num_sleep_ticks_elapsed = mod_125_ms_elapsed_curr_byte -
               mod_125_ms_elapsed_last_byte;
          }
          else
          {
            /* We should not be here but might be here because of a roll over*/
            uim_num_sleep_ticks_elapsed = 0;
          }
        }
        else if (mod_125_ms_elapsed_curr_byte >= mod_125_ms_elapsed_last_byte)
        {
          /* atleast one tick has happened */
          uim_num_sleep_ticks_elapsed += mod_125_ms_elapsed_curr_byte -
            mod_125_ms_elapsed_last_byte;
        }
        else
        {
          if (uim_num_sleep_ticks_elapsed >= TS_SLEEP_TMR_0_CNT_125MS )
          {
            /* we borrow to subtract from the greater modulo */
            uim_num_sleep_ticks_elapsed-=TS_SLEEP_TMR_0_CNT_125MS;
          }

          uim_num_sleep_ticks_elapsed += (mod_125_ms_elapsed_curr_byte + TS_SLEEP_TMR_0_CNT_125MS) -
            mod_125_ms_elapsed_last_byte;
        }

        if ( uim_num_sleep_ticks_elapsed > uim_work_waiting_time_sleep_ticks[uim_drv_slot])
        {
          /* Let us indicate the error some error */
          uim_timeout_signaled_by_driver = TRUE;

          /* Disable the receiver to avoid interrupts */
          uim_reset_uart(uim_drv_slot);

          /* Set the timer to trigger a time-out */
          (void) rex_set_timer(&uim_cmd_rsp_timer, 0);

          uim_rx_state[uim_drv_slot]  = UIM_RX_PROCESS_IDLE;
        }
        uim_is_timer_on = FALSE;

        /* We took about 2 SLEEP cycles to access the SLEEP_TIMER_0_CNT register
         * this time amounts to about 62 micro seconds.  Assumption here that
         * the status register would be updated by now if a parity error
         * has been signaled by the receiver
         */
      }
      else
      {
        /* Wait here for the worst case 0.5 ETU's so that the ISR would be
         * up to date with TX_ERROR if we have it
         */
        uim_clk_busy_wait(60);
      }
#ifdef FEATURE_UIM_T_1_SUPPORT
    }
#endif /* FEATURE_UIM_T_1_SUPPORT */
  }
#endif /* UIM_DRIVER_TIMESTAMP  */
  /* Clear out the command response timer as we have received a byte */
  (void) rex_clr_timer( &uim_cmd_rsp_timer);

  /* Indicate the beginning of the UIM driver */
/*  UIM_TEST_INT_PROCESSING(UIM_DEBUG_ENTER_UART_ISR); */

  /*-------------------------------------------------------------------------
   Read the UART status.  Pass this status to the different state processing
   routines.
  -------------------------------------------------------------------------*/
  /* Process all the bytes in the receive FIFO and the receive errors */
#ifdef FEATURE_UIM_UART_DM
  for (uart_status = UIM_READ_STATUS (), uart_misr_status = UIM_READ_MISR ();
       ((uart_status &
         (MSMU_SR_RXRDY | MSMU_SR_RXBREAK | MSMU_SR_PF_ERR | MSMU_SR_OVR_ERR))
       || (uart_misr_status & ( MSMU_ISR_RXBREAK | MSMU_ISR_RXSTALE |
                                MSMU_ISR_TXLEV | MSMU_ISR_RXHUNT |
                                MSMU_ISR_TX_READY | MSMU_ISR_RXLEV )) );
       uart_status = UIM_READ_STATUS(), uart_misr_status = UIM_READ_MISR())
#else
  for (uart_status = UIM_READ_STATUS (), uart_misr_status = UIM_READ_MISR ();
       ((uart_status &
        (MSMU_SR_RXRDY | MSMU_SR_RXBREAK | MSMU_SR_PF_ERR | MSMU_SR_OVR_ERR))
       || (uart_misr_status & ( MSMU_ISR_RXBREAK | MSMU_ISR_TXLEV)));
       uart_status = UIM_READ_STATUS(), uart_misr_status = UIM_READ_MISR())
#endif /* FEATURE_UIM_UART_DM */
  {
    /* check for RX break errors */
    if ((uart_status & (MSMU_SR_RXBREAK))
        || (uart_misr_status & MSMU_ISR_RXBREAK))
    {
      /* Increment the total rx_break error counter */
      uim_tot_rx_break_error_count++;

      if ( uim_rx_break_error_count == UIM_MAX_RX_BREAK_ERR_COUNT )
      {
        /* don’t ignore the time out signal */
        clear_timeout_on_response = FALSE;

        /* set the UIM RX_ISR state to IDLE */
        uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

        /* Mask all interrupts from UART and stop processing
        any further bytes/interrupts in a loop.  We do not
        want to do this time and again... We have decided
        do power down the interface */
        UIM_PRG_IMR ( MSMU_IMR_MASK_ALL );

        /* Let the task know there were too many rx break errors */
        uim_max_rx_break_error = TRUE;
        uim_rx_break_error_count = 0;

        MSG_ERROR ("Maxed the Rx break error count",0,0,0);

        /* Disable the receiver to avoid interrupts */
        uim_reset_uart(uim_drv_slot);

        /* Clear the WWT timer */
        (void) rex_clr_timer(&uim_cmd_rsp_timer);

        /* Set the command response timeout signal */
        (void) rex_set_sigs( &uim_tcb, UIM_CMD_RSP_TIMEOUT_SIG );
      }
      else
      {
        /* If the I/O line is held low, each time we recet the
           receiver, we are going to get a rx-break interrupt
           so let us do it only if we have not maxed out the error*/
        uim_reset_receiver ();
#ifdef FEATURE_UIM_DONOT_IGNORE_COMMUNICATION_ERRORS_IN_IDLE
        /* Increment the RX break error count */
        ++uim_rx_break_error_count;
#else /* !FEATURE_UIM_DONOT_IGNORE_COMMUNICATION_ERRORS_IN_IDLE */
        if (uim_rx_state[uim_drv_slot] != UIM_RX_PROCESS_IDLE)
        {
          /* If in idle state, we want to ignore any rx_break errors */
          ++uim_rx_break_error_count;
        } /* Not in idle state */
        else
        {
          MSG_ERROR ("RX Break in IDLE",0,0,0);

          /* Increment the idle rx_break error counter */
          uim_idle_rx_break_error_count++;
        } /* IDLE state */
#endif /* FEATURE_UIM_DONOT_IGNORE_COMMUNICATION_ERRORS_IN_IDLE */
      }

    } /* end if - the receiver detected an rx break error. */
    /* Check for receive overrun condition */
    else if (uart_status & (MSMU_SR_OVR_ERR))
    {
      /* increment the total overrun error count variable */
      uim_tot_overrun_error_count++;

      /* Reset the error status.  At this point, we probably lost some
         of the bytes that we are supposed to receive.  Let us
         receive all the bytes that the card has to send and
         re-try the command
      */
      UIM_RESET_ERR ();

      if (uim_overrun_error_count == UIM_MAX_OVERRUN_ERR_COUNT)
      {
        /* Let the task know there were too many overrun errors */
        uim_max_overrun_error = TRUE;
        uim_overrun_error_count = 0;

        MSG_ERROR ("Maxed the Overrun error count",0,0,0);

        /* Disable the receiver to avoid interrupts */
        uim_reset_uart(uim_drv_slot);

        /* Clear the WWT timer */
        (void) rex_clr_timer(&uim_cmd_rsp_timer);

        /* Set the command response timeout signal */
        (void) rex_set_sigs( &uim_tcb, UIM_CMD_RSP_TIMEOUT_SIG );

        /* Mask all interrupts from UART and stop processing
        any further bytes/interrupts in a loop.  We do not
        want to do this time and again... We have decided
        do power down the interface */
        UIM_PRG_IMR ( MSMU_IMR_MASK_ALL );

      }
      else
      {
        if (uim_rx_state[uim_drv_slot] != UIM_RX_PROCESS_IDLE)
        {
          /* If in idle state, we want to ignore any overrun errors */
          ++uim_overrun_error_count;
          /* Set the state to Overrun state */
          uim_rx_state[uim_drv_slot] = UIM_RX_OVERRUN;

          /* Set the flag */
          uim_overrun_error = TRUE;

          /* Flush the data */
          rx_isr_process_overrun_data( uim_drv_slot );

        } /* Non in IDLE state */
        else
        {
            MSG_ERROR ("Overrun error in IDLE",0,0,0);
            /* Increment the idle overrun error count */
            uim_idle_overrun_error_count++;
        } /* IDLE state */
      } /* max overrun error */
    } /* end if - the receiver detected an overrun error. */
    /* check if there is a character in the receive FIFO */
#ifdef FEATURE_UIM_UART_DM
    else if ((uart_status & MSMU_SR_RXRDY)        ||
             (uart_misr_status & MSMU_ISR_TXLEV)  ||
             (uart_misr_status & MSMU_ISR_RXLEV)  ||
             (uart_misr_status & MSMU_ISR_TX_READY)  ||
             (uart_misr_status & MSMU_ISR_RXSTALE))
#else
    else if ((uart_status & MSMU_SR_RXRDY) ||
             (uart_misr_status & MSMU_ISR_TXLEV))
#endif /* FEATURE_UIM_UART_DM */
    {
#ifdef FEATURE_UIM_HANDLE_NO_ATR_IN_40000_CLK_CYCLES
      /* Even receiving a parity error is K to not turn off the interface */
      uim_atr_byte_received = TRUE;
#endif /*FEATURE_UIM_HANDLE_NO_ATR_IN_40000_CLK_CYCLES*/
      /* If we received a parity error */
      if (uart_status & MSMU_SR_PF_ERR)
      {
        /* Increment the total parity error counter */
        uim_tot_parity_error_count++;

        if ( ( uim_parity_error_count == UIM_MAX_PARITY_ERR_COUNT )
#ifdef FEATURE_UIM_T_1_SUPPORT
              && (UIM_T_0_PROTOCOL == uim_operating_protocol)
#endif /* FEATURE_UIM_T_1_SUPPORT */
           )
        {
          /* Let the task know there were too many parity errors */
           uim_max_parity_error = TRUE;
           uim_parity_error_count =0;

           /* Reset the UART receiver (switches to idle state) */
           uim_reset_receiver ();
           uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
           MSG_ERROR ("Maxed the parity error count",0,0,0);

           /* Disable the receiver to avoid interrupts */
           uim_reset_uart(uim_drv_slot);

           /* Clear the WWT timer */
           (void) rex_clr_timer(&uim_cmd_rsp_timer);

           /* Set the command response timeout signal */
           (void) rex_set_sigs( &uim_tcb, UIM_CMD_RSP_TIMEOUT_SIG );

          /* Mask all interrupts from UART and stop processing
          any further bytes/interrupts in a loop.  We do not
          want to do this time and again... We have decided
          do power down the interface */
          UIM_PRG_IMR ( MSMU_IMR_MASK_ALL );

        }
        else
        {
          if (uim_rx_state[uim_drv_slot] != UIM_RX_PROCESS_IDLE)
          {
            /* Ignore any parity error while in idle state */
            ++uim_parity_error_count;
          } /* Not in idle state */
          else
          {
            MSG_ERROR ("Parity error in IDLE",0,0,0);

            /* Increment the idle parity error counter */
            uim_idle_parity_error_count++;
          } /* IDLE state */
        }
      }
      else
      {
        /* check to see if we maxed out on any error if we did
           we want to power down the interface (so let the timeout
           signal remail for the task to process*/
        if (!uim_max_overrun_error && !uim_max_rx_break_error && !uim_max_parity_error
#ifdef UIM_DRIVER_TIMESTAMP
            && !uim_timeout_signaled_by_driver
#endif /* UIM_DRIVER_TIMESTAMP */
           )
        {
          /* Clear the timer signal in case it has already been set */
          (void) rex_clr_sigs( &uim_tcb, UIM_CMD_RSP_TIMEOUT_SIG );
        }

        /* Reset the error counts */
        uim_parity_error_count = 0;
        uim_rx_break_error_count = 0;
        uim_overrun_error_count = 0;

        if (uim_rx_state[uim_drv_slot] != UIM_RX_PROCESS_IDLE)
        {
          /* On receiving a valid byte, while not in idle state, reset the
           * idle state error counters
           */
          uim_idle_parity_error_count = 0;
          uim_idle_rx_break_error_count = 0;
          uim_idle_overrun_error_count = 0;
        }
      }

      /* Process the interrupt based on the current processing state */
      switch (uim_rx_state[uim_drv_slot])
      {
        case UIM_RX_PROCESS_IDLE:         /* Not expecting any data */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_PROCESS_IDLE); */

            rx_isr_process_idle ( uim_drv_slot );
          }
          break;

        case UIM_RX_RECEIVE_ATR:          /* Expecting the ATR */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_RECEIVE_ATR); */

            #ifdef FEATURE_UIM_UART_DM
            if ( UIM_READ_MISR() & MSMU_IMR_RXSTALE)
            {
                // clear the stale char event
                UIM_CLEAR_RX_STALE_INT();
            }
            #endif /* FEATURE_UIM_UART_DM */
            rx_isr_receive_atr ( uim_drv_slot );
          }
          break;

        case UIM_RX_TX_BYTE:              /* Process the transmitted byte
                                              and error signal             */
          {
            /* Process the transmitted byte and error signal */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_TX_BYTE); */

            rx_isr_process_tx_byte ( uim_drv_slot );
          }
          break;

        case UIM_RX_GUARDTIME:            /* Process the guardtime of the
                                              transmitted byte             */
          {
            /* Process the guardtime of the transmitted byte */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_GUARDTIME); */

            rx_isr_process_guardtime ( uim_drv_slot );
          }
          break;

        case UIM_RX_LAST_TX_BYTE:         /* Process the last byte of the
                                              tx buffer */
          {
            /* Process the last byte of the buffer */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_LAST_BYTE); */

            rx_isr_process_last_tx_byte ( uim_drv_slot );
          }
          break;

        case UIM_RX_RECEIVE_PPS:          /* Expecting the PPS response */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_RECEIVE_PPS);*/

            rx_isr_receive_pps ( uim_drv_slot );

            #ifdef FEATURE_UIM_UART_DM
            if ( UIM_READ_MISR() & MSMU_IMR_RXSTALE)
            {
                // clear the stale char event.
                UIM_CLEAR_RX_STALE_INT();
            }
            #endif /* FEATURE_UIM_UART_DM */
          }
          break;

        case UIM_RX_OVERRUN:              /* Rx overrun has occured */
        case UIM_RX_UNKNOWN_PROC_BYTE_RCVD:
          {
            /* Flush the data */
            rx_isr_process_overrun_data( uim_drv_slot );
          }
          break;

        case UIM_RX_PROC_PRCDR_BYTES:/* Expecting procedure bytes from the
                                         UIM                                 */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_PROC_PRCDR_BYTES);*/

            #ifdef FEATURE_UIM_UART_DM
            rx_isr_proc_procedure_bytes_dm ( uim_drv_slot );
            #else
            rx_isr_proc_procedure_bytes ( uim_drv_slot );
            #endif /* FEATURE_UIM_UART_DM */
          }
          break;

        case UIM_RX_SW1_NORM_END:   /* Expecting the second status word */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_NORM_END);*/

            rx_isr_sw1_normal_end ( uim_drv_slot );
          }
          break;

        case UIM_RX_SW1_FETCH_END:  /* Expecting the second status word */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_FETCH_END);*/

            rx_isr_sw1_fetch_end ( uim_drv_slot );
          }
          break;

#ifdef FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR
        case UIM_RX_SW1_DLOAD_ERR:  /* Expecting the second status word */
          {
            rx_isr_sw1_dload_err ( uim_drv_slot );
          }
          break;
#endif /* FEATURE_UIM_TOOLKIT_DOWNLOAD_ERROR */

        case UIM_RX_SW1_RESP_END:   /* Expecting the second status word */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_RESP_END); */
            uim_get_resp_sw1 = SW1_END_RESP;
            rx_isr_sw1_resp_end ( uim_drv_slot );
          }
          break;

        case UIM_RX_SW1_BUSY:       /* Expecting the second status word */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_BUSY);*/

            rx_isr_sw1_busy ( uim_drv_slot );
          }
          break;

        case UIM_RX_SW1_RETRIES:    /* Expecting the second status word */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_RETRIES);*/

            rx_isr_sw1_retries ( uim_drv_slot );
          }
          break;

        case UIM_RX_SW1_REFERENCE:  /* Expecting the second status word */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_REFERENCE);*/

            rx_isr_sw1_reference ( uim_drv_slot );
          }
          break;

        case UIM_RX_SW1_SECURITY:   /* Expecting the second status word */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_SECURITY);*/

            rx_isr_sw1_security ( uim_drv_slot );
          }
          break;

        case UIM_RX_SW1_P3_BAD:     /* Expecting the second status word */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_P3_BAD);*/

            rx_isr_sw1_p3_bad ( uim_drv_slot );
          }
          break;

        case UIM_RX_SW1_P1_P2_BAD:  /* Expecting the second status word */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_P1_P2_BAD);*/

            rx_isr_sw1_p1_p2_bad ( uim_drv_slot );
          }
          break;

        case UIM_RX_SW1_INS_BAD:    /* Expecting the second status word */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_INS_BAD);*/

            rx_isr_sw1_ins_bad ( uim_drv_slot );
          }
          break;

        case UIM_RX_SW1_CLA_BAD:    /* Expecting the second status word */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_CLA_BAD);*/

            rx_isr_sw1_cla_bad ( uim_drv_slot );
          }
          break;

        case UIM_RX_SW1_PROBLEM:    /* Expecting the second status word */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_PROBLEM);*/

            rx_isr_sw1_problem ( uim_drv_slot );
          }
          break;

        case UIM_RX_PROC_RESP_DATA: /* Expecting the APDU response */
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_PROC_RESP_DATA);*/

            rx_isr_process_resp_data ( uim_drv_slot );
          }
          break;

#if defined(FEATURE_UIM_UICC) || defined(FEATURE_UIM_UTIL)

        case UIM_RX_SW1_USIM_END_RESP:
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_USIM_END_RESP);*/

            /* The processing for this case is identical to the processing of
               the status word SW1_END_RESP */
            uim_get_resp_sw1 = SW1_USIM_END_RESP;
            rx_isr_sw1_resp_end ( uim_drv_slot );
          }
        break;

#ifdef FEATURE_UIM_WIM

        case UIM_RX_SW1_SE_FAIL:
          {
            rx_isr_sw1_se_fail ( uim_drv_slot );
          }
        break;
#endif /* FEATURE_UIM_WIM */

        case UIM_RX_SW1_USIM_RESEND_APDU:
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_USIM_RESEND_APDU);*/

            /* The processing for this case is identical to the processing of
             the status word SW1_END_RESP */

            rx_isr_sw1_resend_apdu ( uim_drv_slot );
          }
        break;

#ifdef FEATURE_UIM_UICC
        case UIM_RX_SW1_EXECUTION_ERR1:
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_EXECUTION_ERR1);*/

            rx_isr_sw1_execn_err1 ( uim_drv_slot );
          }
        break;
        case UIM_RX_SW1_CLA_NOT_SUPPORTED:
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_CLA_NOT_SUPPORTED);*/

            rx_isr_sw1_cla_not_supported ( uim_drv_slot );
          }
        break;
#endif /* FEATURE_UIM_UICC */
        case UIM_RX_SW1_WARNINGS1:
          {
            /* Update the flag to indicate that WARNINGS2 status 
                is returned by the SIM */
            uim_warning1_indicator = TRUE;
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_WARNINGS1);*/

            /* Let us consider this as a normal response
             * but only with p3 = 0 for non GSM case-4 other than
             * GENERATE_ASY_KEY_PAIR WIM instruction
             */
            if ( (uim_last_instrn_case[uim_drv_slot] == UIM_INSTRN_CASE_4 ) &&
                 (last_sent_apdu[uim_drv_slot].instrn != UIM_GENERATE_ASY_KEY_PAIR ) &&
                 uim_check_to_do_get_response_if_seek())
            {
              uim_sw1_warnings1_normal_end_occured = TRUE;

              uim_force_get_response = TRUE;
              uim_get_resp_sw1 = SW1_END_RESP;
              rx_isr_sw1_resp_end ( uim_drv_slot );
              uim_force_get_response = FALSE;
            }
            else
            {
              rx_isr_sw1_warnings1 ( uim_drv_slot );
            }
          }
        break;

        case UIM_RX_SW1_WARNINGS2:
          {
            /* Update the flag to indicate that WARNINGS2 status 
               is returned by the SIM */
            uim_warning2_indicator = TRUE;
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_WARNINGS2);*/

            /* Let us consider this as a normal response
             * but only with p3 = 0 for non GSM case-4 other than
             * GENERATE_ASY_KEY_PAIR WIM instruction
             */
            if ( (uim_last_instrn_case[uim_drv_slot] == UIM_INSTRN_CASE_4 ) &&
                 (last_sent_apdu[uim_drv_slot].instrn != UIM_GENERATE_ASY_KEY_PAIR )&&
                 uim_check_to_do_get_response_if_seek())
            {
              uim_force_get_response = TRUE;
              uim_get_resp_sw1 = SW1_END_RESP;
              rx_isr_sw1_resp_end ( uim_drv_slot );
              uim_force_get_response = FALSE;
            }
            else
            {
              rx_isr_sw1_warnings2 ( uim_drv_slot );
            }
          }
        break;

        case UIM_RX_SW1_EXECUTION_ERR2:
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_EXECUTION_ERR2);*/

            rx_isr_sw1_execn_err2 ( uim_drv_slot );
          }
        break;

        case UIM_RX_SW1_CMD_NOT_ALLOWED:
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_CMD_NOT_ALLOWED);*/

            rx_isr_sw1_cmd_not_allowed ( uim_drv_slot );
          }
        break;

        case UIM_RX_SW1_WRONG_PARAMS:
          {
            /* Indicate the operation of the driver */
/*            UIM_TEST_INT_PROCESSING(UIM_DEBUG_SW1_WRONG_PARAMS);*/

            rx_isr_sw1_wrong_params ( uim_drv_slot );
          }
        break;
#endif /* FEATURE_UIM_UICC || FEATURE_UIM_UTIL */

#if defined( FEATURE_UIM_T_1_SUPPORT )

#ifdef FEATURE_UIM_UART_DM

        case UIM_RX_T_1_RECV_T_1_BLOCK_DM: /* Receive the T=1 block */
          {
            rx_isr_recv_t_1_block( uim_drv_slot );
          } /* end case - UIM_RX_T_1_RECV_T_1_BLOCK_DM */
          break;

#endif /* FEATURE_UIM_UART_DM */


        case UIM_RX_T_1_SEND_PROLOGUE_ST:  /* Send the prologue field for T=1 */
          {
#ifdef FEATURE_UIM_UART_DM
            rx_isr_send_prologue_dm ( uim_drv_slot );
#else
            rx_isr_send_prologue ( uim_drv_slot );
#endif /* FEATURE_UIM_UART_DM */
          } /* end case - UIM_RX_T_1_SEND_PROLOGUE_ST */
          break;

        case UIM_RX_T_1_SEND_INFO_ST:   /* Send the information field for T=1 */
          {
#ifdef FEATURE_UIM_UART_DM
            rx_isr_send_information_dm ( uim_drv_slot );
#else
            rx_isr_send_information ( uim_drv_slot );
#endif /* FEATURE_UIM_UART_DM */
          } /* end case - UIM_RX_T_1_SEND_INFO_ST */
          break;

        case UIM_RX_T_1_SEND_EPILOGUE_ST:  /* Send the epilogue field for T=1 */
          {
#ifdef FEATURE_UIM_UART_DM
            rx_isr_send_epilogue_dm ( uim_drv_slot );
#else
            rx_isr_send_epilogue ( uim_drv_slot );
#endif /* FEATURE_UIM_UART_DM */
          } /* end case - UIM_RX_T_1_SEND_EPILOGUE_ST */
          break;

        case UIM_RX_T_1_RX_PROLOGUE_ST: /* Receive the prologue field for T=1 */
          {
            rx_isr_rx_prologue ( uim_drv_slot );
          } /* end case - UIM_RX_T_1_RX_PROLOGUE_ST */
          break;

        case UIM_RX_T_1_RX_INFO_ST:  /* Receive the information field for T=1 */
          {
            rx_isr_rx_information ( uim_drv_slot );
          } /* end case - UIM_RX_T_1_RX_INFO_ST */
          break;

        case UIM_RX_T_1_RX_EPILOGUE_ST: /* Receive the epilogue field for T=1 */
          {
            rx_isr_rx_epilogue ( uim_drv_slot );
          } /* end case - UIM_RX_T_1_RX_EPILOGUE_ST */
          break;
#endif /* FEATURE_UIM_T_1_SUPPORT */

        default:
          {
            /* Something is terribly wrong.  The state variable is out of
               range.                                                        */
            /* Go back to the idle state. */
            uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;
#ifdef FEATURE_UIM_DEBUG_LOG
            UIM_LOG_PUT_LAST_BYTE();
#endif
            MSG_HIGH("Invalid uim_rx_state %d",uim_rx_state[uim_drv_slot],0,0);
            (void)rex_set_timer(&uim_cmd_rsp_timer, 0);
          } /* end case - default */
      } /* end switch - uim_rx_state */
    } /* end if - uart status indicates an error condition */
  } /* end for - read the UART status and process */

  /* Indicate the driver is exiting */
/*  UIM_TEST_INT_PROCESSING(UIM_DEBUG_EXIT_UART_ISR);*/

  /* Check if we are still waiting for more bytes. */
  if ((uim_rx_state[uim_drv_slot] != UIM_RX_PROCESS_IDLE)
#ifdef FEATURE_UIM_SUPPORT_MULTIPLE_VOLTAGE_CLASSES
       || ( !uim_atr_pps_done && (num_bytes_to_skip[uim_drv_slot] !=0) )
#endif
      )
  {
    /* Set the comand response timer since we are still waiting for more
       bytes */
#ifdef FEATURE_UIM_UART_DM

    /* Enabling the work waiting fix for only T=0 protocol */
    switch(uim_rx_state[uim_drv_slot])
    {
#ifdef FEATURE_UIM_T_1_SUPPORT
      case UIM_RX_T_1_RECV_T_1_BLOCK_DM:
      case UIM_RX_T_1_SEND_PROLOGUE_ST:
      case UIM_RX_T_1_SEND_INFO_ST:
      case UIM_RX_T_1_SEND_EPILOGUE_ST:
      case UIM_RX_T_1_RX_PROLOGUE_ST:
      case UIM_RX_T_1_RX_INFO_ST:
      case UIM_RX_T_1_RX_EPILOGUE_ST:
        {
          (void) rex_set_timer(&uim_cmd_rsp_timer, 256*uim_work_waiting_time[uim_drv_slot]);
        }
        break;
#endif /* FEATURE_UIM_T_1_SUPPORT */
      default:
        {
          /* Adding 5ms to the work waiting time as SW overhead */
          (void) rex_set_timer(&uim_cmd_rsp_timer, (uim_work_waiting_time[uim_drv_slot] + 5) );
        }
        break;
    }
#else
    (void) rex_set_timer(&uim_cmd_rsp_timer, uim_work_waiting_time[uim_drv_slot]);
#endif /* FEATURE_UIM_UART_DM */

#ifdef UIM_DRIVER_TIMESTAMP
#ifdef FEATURE_UIM_T_1_SUPPORT
    if ( UIM_T_0_PROTOCOL == uim_operating_protocol )
#endif /* FEATURE_UIM_T_1_SUPPORT */
    {
      if(uim_UseDriverTimeStamp)
      {
        uim_is_timer_on = TRUE;
        ts_get(last_byte_time_stamp);
      }
    }
#endif /* UIM_DRIVER_TIMESTAMP */
  } /* end if - waiting for more bytes */
} /* end uim_rx_isr */


#ifdef UIM_DRIVER_TIMESTAMP
/*===========================================================================

FUNCTION uim_StartDriverTimeStamp                      EXTERNALIZED FUNCTION

DESCRIPTION
  This function starts the usage of the uim_rx_isr driver time stamp mechanism
  and sets the priority of the rx_isr to a high priority

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void uim_StartDriverTimeStamp()
{
  uim_UseDriverTimeStamp = TRUE;
  /* set the priority to high so that we will be able to get
     more accurate time stamps*/
  MSG_HIGH("Starting UIM Drv TimeStamp mechanism Rx_isr High Priority",0,0,0);
  UIM_SET_INT_PRIORITY(HI_PRIORITY);
}


/*===========================================================================

FUNCTION uim_StopDriverTimeStamp                      EXTERNALIZED FUNCTION

DESCRIPTION
  This function stops the usage of the time stamping mechanism of the
  rx_isr.  Also lowers back the priority of the rx_isr back to normal
  and sets back the Watermark level to 5

DEPENDENCIES
  This function starts an APDU exchange.  The exchange can only happen after
  the ATR has been received and, optionally, after a PPS exchange.

RETURN VALUE
  None

SIDE EFFECTS

===========================================================================*/
void uim_StopDriverTimeStamp()
{
  if (uim_UseDriverTimeStamp)
  {
    INTLOCK();
    uim_UseDriverTimeStamp = FALSE;
    /* Set the priority here to normal*/
    UIM_SET_INT_PRIORITY(LO_PRIORITY);
    /* Set the water mark level back to normal */
    UIM_RX_WATERLEVEL ( UIM_RX_WATER_MARK_LEVEL_FIVE );
    INTFREE();
    MSG_HIGH("Stopped UIM Drv TimeStamp mechanism Rx_isr Normal Priority",0,0,0);
  }
}
#endif /* UIM_DRIVER_TIMESTAMP */

#ifdef FEATURE_UIM1

/*===========================================================================

FUNCTION UIM_RX_ISR_1                           INTERNAL FUNCTION

DESCRIPTION
  This is the Rx ISR for UART 2 used to communicate with the UIM.  This ISR
  handles ALL (receive and transmit) operations of the UIM interface.  It
  handles transmitting because the Rx and Tx lines are shared.

DEPENDENCIES
  This ISR calls a common ISR handler that uses the slot parameter to access
  the proper UART.  This allows the ISR to be re-entrant.

RETURN VALUE
  None

SIDE EFFECTS
  This ISR services the UART interrupts.  After servicing the UART hardware,
  the UART hardware interrupt status is reset.

===========================================================================*/
LOCAL void uim_rx_isr_1
(
  void
)
{
  /* Call the common ISR function with the proper slot information */
  uim_rx_isr( UIM_DRV_SLOT1 );
} /* end uim_rx_isr_1 */
#endif /* FEATURE_UIM1 */

#ifdef FEATURE_UIM2


/*===========================================================================

FUNCTION UIM_RX_ISR_2                           INTERNAL FUNCTION

DESCRIPTION
  This is the Rx ISR for UART 3 used to communicate with the UIM.  This ISR
  handles ALL (receive and transmit) operations of the UIM interface.  It
  handles transmitting because the Rx and Tx lines are shared.

DEPENDENCIES
  This ISR calls a common ISR handler that uses the slot parameter to access
  the proper UART.  This allows the ISR to be re-entrant.

RETURN VALUE
  None

SIDE EFFECTS
  This ISR services the UART interrupts.  After servicing the UART hardware,
  the UART hardware interrupt status is reset.

===========================================================================*/
LOCAL void uim_rx_isr_2
(
  void
)
{
  /* Call the common ISR function with the proper slot information */
  uim_rx_isr( UIM_DRV_SLOT2 );
} /* end uim_rx_isr_2 */
#endif /* FEATURE_UIM2 */

/*===========================================================================

FUNCTION uim_read_status

DESCRIPTION
  This function could be used to read the UART STATUS register value

DEPENDENCIES
  None

RETURN VALUE
  UART status ( for the current slot if dual slot )

SIDE EFFECTS
  None

===========================================================================*/

dword uim_read_status(void)
{
  return UIM_READ_STATUS();
}

/*===========================================================================

FUNCTION uim_clk_busy_wait

DESCRIPTION
  This function is a wrapper around clk_busy_wait to avoid data conversion
  issues for different targets and to validate the supplied values.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void uim_clk_busy_wait(dword busy_wait_time)
{
  if ( busy_wait_time > UIM_MAX_CLK_BUSY_WAIT )
  {
    MSG_ERROR("uim_clk_busy_wait large value %d", busy_wait_time, 0, 0);
    busy_wait_time = UIM_MAX_CLK_BUSY_WAIT;
  }

  /*lint -e{732} suppress "loss of sign" since different targets
   *have different signature for clk_busy_wait.  This should be
   *safe after the above check
   */
  clk_busy_wait( (int) busy_wait_time );
}

/*===========================================================================

FUNCTION UIM_ENABLE_VREG_LPM

DESCRIPTION
  The PMIC has the ability to put the different Voltage Regulators into
  a Low Power Mode.  The Regulator will go in and out of LPM based on
  the TCXO state of TCXO.  When in TXCO Shutdown, VREG (R)UIM is put
  into an LPM Mode. When TCXO is enabled, VREG (R)UIM is pulled out
  of LPM Mode.

  Going in an out of LPM is controlled by the PMIC.

  The UIM Drivers will need to ensure that the PMIC is notified that LPM
  mode is allowed for VREG (R)UIM.

  The UIM will not have to take VREG (R)UIM out of LPM

DEPENDENCIES
  Prior to calling this function, we must ensure from a system perspective
  that the following functions have been called.

  1) rflib_init()
  2) pm_init()

RETURN VALUE
  void - If setting to LPM Fails, not much the UIM can do about it.

SIDE EFFECTS
  After this function is executed, the VREG (R)UIM is allowed to get into
  LPM following the state of TCXO.

===========================================================================*/
void uim_enable_vreg_lpm
(
  void
)
{
  #ifdef FEATURE_UIM_ENABLE_VREG_UIM_LPM

  pm_err_flag_type uim_pm_status = PM_ERR_FLAG__SUCCESS;

  uim_pm_status = pm_lp_mode_control( PM_ON_CMD,PM_VREG_LP_RUIM_ID);

  if ( PM_ERR_FLAG__SUCCESS != uim_pm_status )
  {
    MSG_ERROR("Failed to put VREG (R)UIM into LPM: 0x%x",uim_pm_status,0,0);
  }

  #endif /* FEATURE_UIM_ENABLE_VREG_UIM_LPM */

  return;

}/* uim_enable_vreg_lpm */

#ifdef FEATURE_UIM_HANDLE_TA1_0X97_ATR
/*===========================================================================

FUNCTION UIM_IS_FI_DI_SUPPORTED

DESCRIPTION
  This procedure determines if the platform can support any {FI, DI}
  combination.

DEPENDENCIES
  None

RETURN VALUE
  BOOL

SIDE EFFECTS
  None

===========================================================================*/
boolean uim_is_fi_di_supported
(
  uim_clock_rate_conversion_factor_type  FI, /* CRCF index */
  uim_baud_rate_adjustment_factor_type   DI /* BRAF index */
)
{

/*
  The FI (CRCF) and DI (BRAF) that is negotiated determines the baud rate
  that is set on the SIM card, which is:
  uim_clk / (FI / DI) = uim_clk * DI / FI.

  This baud-rate will have to be set on the UART so that UART and the SIM are
  in sync. The baud-rate that is set on the UART is a function of
  {M, N, uart_clk} which is equal to (M / N) * (uart_clk / 16).

  M/N divisor is needed to divide the input clock, and 16 division factor is
  per UART’s design that the UART’s baud-rate is 16 times less than the
  uart_clk frequency. Also, there is a constraint in the M/N counter, whether
  inside the UART or outside, that N/M ratio should be greater than or equal
  to 2.

  Therefore,

  (M/N) * (uart_clk / 16) = (uim_clk) * DI/ FI     --- equation (1)
  N/M >= 2            --- equation (2)

  Solving the above two equations (eliminating M and N terms), we get the
  following:

  ( uart_clk / uim_clk ) >=  (32 * DI / FI)            --- equation (3)

  This means that, a {FI, DI} combination is supported by a platform as long
  as the above equation (3) is satisfied.

*/

  if((FI < UIM_CRCF_SIZE) &&
     (DI < UIM_BRAF_SIZE))
  {
    if( (uim_clk_freq[uim_drv_slot] > 0) &&
        (uim_clk_source_freq[uim_drv_slot] > 0) &&
        (crcf_values[FI] > 0) && (braf_values[DI] > 0) )
    {
      if ( (uim_clk_source_freq[uim_drv_slot] / uim_clk_freq[uim_drv_slot]) >=
           ((32 * braf_values[DI]) / crcf_values[FI]) )
      {
        MSG_HIGH("FI and DI are supported",0,0,0);
        return TRUE;
      }
      else
      {
        MSG_HIGH("FI and DI are NOT supported !!!",0,0,0);
        return FALSE;
      }
    }
    else
    {
      MSG_HIGH("UIM CLK FREQ is not valid !!!",0,0,0);
      return FALSE;
    }
  }

  MSG_HIGH("FI and DI are NOT supported",0,0,0);
  return FALSE;

} /* uim_is_fi_di_supported */

#endif/* FEATURE_UIM_HANDLE_TA1_0X97_ATR */

#ifdef FEATURE_UIM_UART_DM
/*===========================================================================

FUNCTION uim_bytes_waiting_in_rxfifo

DESCRIPTION
  This procedure determines how many bytes are waiting to be read in the
  RX FIFO by querying the RXFS register.

DEPENDENCIES
  None

RETURN VALUE
  Number of byte present in the RX FIFO which are yet to be read.

SIDE EFFECTS
  None

===========================================================================*/
int32 uim_bytes_waiting_in_rxfifo(void)
{
  int32 bytes = 0;
  int32 num_bytes = 0;

  /* Read the RXFS register to get the number of bytes
     waiting in the RX FIFO and the packing buffer */
  bytes = UIM_GET_NUM_BYTES_WAITING_IN_RX();

  /* Calculate the count of bytes in the RX FIFO and packing buffer */
  num_bytes =
   ( 4 * ( (bytes & UART_DM_RXFS__RXFIFO_STATE_LSB_MASK)
     >> UART_DM_RXFS__RXFIFO_STATE_LSB_SHFT) ) +
   ( (bytes & UART_DM_RXFS__RX_BUFFER_STATE_MASK)
     >> UART_DM_RXFS__RX_BUFFER_STATE_SHFT) ;


  return num_bytes;
}

/*===========================================================================

FUNCTION uim_bytes_waiting_in_txfifo

DESCRIPTION
  This procedure determines how many bytes are waiting to be sent in the
  TX FIFO plus un-packing buffer by querying the TXFS register.

DEPENDENCIES
  None

RETURN VALUE
  Number of byte present in the TX FIFO which are yet to be read.

SIDE EFFECTS
  None

===========================================================================*/
int32 uim_bytes_waiting_in_txfifo(void)
{
  int32 bytes = 0;
  int32 num_bytes = 0;

  /* Read the TXFS register to get the number of bytes
     waiting in the TX FIFO and the un-packing buffer */
  bytes = (int32)UIM_GET_NUM_BYTES_WAITING_IN_TX();

  /* Calculate the count of bytes in the TX FIFO and un-packing buffer */
  num_bytes =
   ( 4 * ( (bytes & UART_DM_TXFS__TXFIFO_STATE_LSB_MASK)
     >> UART_DM_TXFS__TXFIFO_STATE_LSB_SHFT) ) +
   ( (bytes & UART_DM_TXFS__TX_BUFFER_STATE_MASK)
     >> UART_DM_TXFS__TX_BUFFER_STATE_SHFT) ;


  return num_bytes;
} /* uim_bytes_waiting_in_txfifo */

#endif /* FEATURE_UIM_UART_DM */


#ifdef FEATURE_UIM_USB_UICC
#error code not present
#endif /* FEATURE_UIM_USB_UICC */


#ifdef FEATURE_UIM_CARD_ERR_UIM_LDO_PUP
#error code not present
#endif /* FEATURE_UIM_CARD_ERR_UIM_LDO_PUP */

/*===========================================================================

FUNCTION UIM_CMD_RSP_TIMER_EXPIRY_CB

DESCRIPTION
  This procedure is registered as a call-back associated with the expiry of
  the command response timer. This call-back is called in the context of the
  high priority timer task as soon as the rex timer for the work waiting time
  expires.

  The logic in this call-back is as follows:
    - (for LCU) check if the count in the rx fifo + the packing buffer increased
        from the last check.
          - If the count increased, then re-start the wwt timer.
          - else, put driver’s uim_rx_state to IDLE, mask all interrupts, reset
            uart, set cmd_rsp_timeout signal and powerdown the sim interface

    - (for non-LCU) check if RXRDY bit is set in the status. If set,
        set cmd_rsp_timeout and return. If not set, put driver’s uim-rx-state to
        idle, mask all interrupts, reset uart, set cmd_rsp_timeout signal and
        power down the sim interface.

  NOTE: The powerdown sequence of the UIM (as in the uim_power_down() function)
        is not started within this call-back as it is a huge-piece of logic
        which might hog the CPU as this call-back is being executed in the
        context of the timer task. Hence, this call-back only signals the
        UIM task to initiate the powerdown sequence, if needed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Power down of the SIM interface if the command response timeout expired.

===========================================================================*/
void uim_cmd_rsp_timer_expiry_cb (unsigned long unused)
{
  (void)unused;

#ifdef FEATURE_UIM_UART_DM

  /* Ignore the WWT expiry if there are any TX bytes
   * waiting to be sent
   */
  if( uim_bytes_waiting_in_txfifo() != 0)
  {
    return;
  }

  /* In UART_DM mode, when work waiting timer (WWT) expires, check if the RXFS
     count changed since the previous count. If the count increased, then
     ignore the WWT timeout. If the count is 0 or remained constant with respect
     to the previous count, then do not ignore the WWT timeout and proceed in
     powering down the SIM interface. */

  if( (num_bytes_in_this_rx_transfer[uim_drv_slot] < uim_bytes_waiting_in_rxfifo())
        || (UIM_READ_STATUS() & MSMU_SR_RXRDY) )
  {
    num_bytes_in_this_rx_transfer[uim_drv_slot] = 0;

    /* Set the command response timer since we are still waiting for more
       bytes. Adding 5ms to the work waiting time as SW overhead */
    (void) rex_set_timer(&uim_cmd_rsp_timer, (uim_work_waiting_time[uim_drv_slot] + 5));
  }
#else /* ! FEATURE_UIM_UART_DM */
  if ( (UIM_READ_STATUS() & MSMU_SR_RXRDY) ||
       (rex_get_sigs(&uim_tcb) & UIM_CMD_RSP_SIG) )
  {
    /* Leave the handling of the command response signal to the UIM task as the data
       is received within work waiting time */
    (void) rex_set_sigs( &uim_tcb, UIM_CMD_RSP_TIMEOUT_SIG );
  }
#endif /* FEATURE_UIM_UART_DM */
  else
  {
    /* Set the command response timeout signal so that UIM task can initiate
       the powerdown procedure */
    (void) rex_set_sigs( &uim_tcb, UIM_CMD_RSP_TIMEOUT_SIG );

    /* Let us not clear the timeout signal */
    clear_timeout_on_response = FALSE;

    /* For T=0 protocol, UIM interface is powered-down after WWT timeout */
    /* For T=1 protocol, UIM interface is not powered-down but R-block
       (receive-ready) is transmitted. Hence, the below is needed for T=0
       protocol only. */ 
    if ( UIM_T_0_PROTOCOL == uim_operating_protocol )
    {
      /* set the RX state to Idle */
      uim_rx_state[uim_drv_slot] = UIM_RX_PROCESS_IDLE;

      /* Mask all interrupts from UART since we are going to powerdown
         anyway. */
      UIM_PRG_IMR ( MSMU_IMR_MASK_ALL );

      /* Reset/flush the UART */
      uim_reset_uart (uim_drv_slot);
    }
  }

  return;
} /* uim_cmd_rsp_timer_expiry_cb */

/*===========================================================================

FUNCTION UIM_UIMDRV_FLUSH_UART

DESCRIPTION
  This function flushes the bytes in the RX FIFO by reading the bytes/words 
  from the UART one by one and resets the UART RX to reset the error flags in 
  UART. This way of reading the byte/words from the UART might, most of the 
  times, take care of flushing those bytes/words that have just entered the 
  UART and/or entering into the UART. 

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void uim_uimdrv_flush_uart(void)
{
  dword uart_status;  /* Holds the UART status */

#ifdef FEATURE_UIM_UART_DM
  volatile uint32 rx_value;   /* Holds the received word from the receive FIFO. */
#else
  volatile byte rx_value;     /* Holds the received byte from the receive FIFO. */
#endif /* FEATURE_UIM_UART_DM */

  /* Make sure there is a byte in the receive FIFO */
  for (uart_status = UIM_READ_STATUS();
       (uart_status & MSMU_SR_RXRDY);
       uart_status = UIM_READ_STATUS())
  {
#ifdef FEATURE_UIM_UART_DM
    /* Get the next received word. */
    rx_value = UIM_GET_RX_WORD();
#else
    /* Get the next received byte. */
    rx_value = UIM_GET_RX_BYTE();
#endif /* FEATURE_UIM_UART_DM */
  }

  /* reset all the error flags in the UART RX */
  uim_reset_receiver();
} /* uim_uimdrv_flush_uart */

/*===========================================================================

FUNCTION UIM_CHECK_TO_DO_GET_RESPONSE_IF_SEEK

DESCRIPTION
  This procedure determines if get response should be done or not if the last
  sent APDU is SEEK command.

DEPENDENCIES
  None

RETURN VALUE
  TRUE   if SEEK command and ens is enabled or gcf testing is going on
  FALSE  if SEEK command and both ens or gcf testing flags are off.

SIDE EFFECTS
  None

===========================================================================*/
LOCAL boolean uim_check_to_do_get_response_if_seek()
{
  if(last_sent_apdu[uim_drv_slot].instrn == SEEK)
  {
    if((TRUE == uim_nv_ens_enabled_flag) || 
        (TRUE == uim_is_test_iccid()))
    {
      return TRUE;
    }
    else
    {
      /* ENS nv item is not set to 1 and iccid is not a gcf iccid */
      return FALSE; 
    }
  }
  else
  {
    return TRUE; /* Not a SEEK command */
  }
}/* uim_check_to_do_get_response_if_seek */


/*===========================================================================

                Lint message inhibitions turned back on

===========================================================================*/
/*lint +esym(578,uim_drv_slot)*/
/* uim_drv_slot is used in the macros to write to the UART registers to
   support dual slots.  I declare uim_drv_slot locally to override the global
   uim_drv_slot so that I can use the same macros whether uim_drv_slot (dual
   slots) is used or not.
*/
/*lint +esym(715,uim_drv_slot)*/
/* uim_drv_slot is not accessed from the macros when the build configuration
   is for one slot.
*/

/* There are a number of macros and definitions that are not always used
   due to the build configuration (one slot or two).  Also, there are a
   number of constants defined in the file to completely describe the
   access to the registers.  These are not used.
*/
/*lint +esym(752,uim_rx_isr_1)*/
/*lint +esym(752,uim_rx_isr_2)*/
/* uim_rx_isr_1 or uim_rx_isr_2 are defined based upon the which slots are
   supported in the build
*/

#endif /* FEATURE_UIM_DRIVER */
