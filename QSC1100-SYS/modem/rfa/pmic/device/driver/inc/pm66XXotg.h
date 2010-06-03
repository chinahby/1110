
#ifndef PM66XXOTG_H
#define PM66XXOTG_H

/*===========================================================================
                  P M 6 6 X X O T G   H E A D E R    F I L E

                              USB-OTG DRIVER

DESCRIPTION
   This file contains functions prototypes and variable/type/constant
   declarations for supporting USB On-The-Go in Qualcomm's PM66XX chips.

Copyright (c) 2003, 2004, 2005, 2006 by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/device/main/latest/driver/inc/pm66XXotg.h#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/30/08   vk      Added API to enable/disable SESS_COMP_EN for HAN/KIP B0/C0
10/09/08   jtn     Changed pm_otg_pwr_sel_type for QSC1100, additional control bit
06/03/08   vk      Added new enum in pm_otg_pwr_sel_type
                   PM6658 :get power from either VDD or For USB_VBUS 
01/07/08   jtn     Merged from QSC6270 branch
(begin QSC6270 changes) 
10/12/07   jnoblet Added APIs pm_usb_vbus_dischg_set, pm_usb_vbus_comp_en_set, 
                   pm_usb_vbus_drv_set
10/11/07   jnoblet Added APIs pm_usb_vbus_chg_set and pm_usb_vbus_ldo_en_set 
10/10/07   jnoblet Added API for Han USB VBUS control
(end QSC6270 changes)
12/22/06   aab     Added API to return current sourcing limit of PMICs that supports host mode.
06/12/06   Vish    Added APIs- pm_otg_enable_audx_pre_bias() and pm_otg_set_audx_bias()
                   to control pre-bias and bias for audio channels on USB 
                   D+/D- lines.
09/01/05   Vish    Added PM_OTG_CARKIT_AUDIO_MODE__TEST_MODE_STEREO_OUT_MIC_IN
                   to pm_otg_carkit_audio_mode_type.
11/30/04   Vish    Added enums for enabling both pull-up and pull-down
                   resistors at the same time on D+/- line.
11/17/04   Vish    - Removed carkit interrupt hysteresis from the prototype
                     of pm_otg_config_carkit_mode().
                   - Added pm_otg_set_carkit_interrupt_hysteresis().
10/12/04   Vish    - Added new carkit audio configurations to allow for
                     independent control of speaker/mic paths in mono mode.
                   - Changed carkit interrupt hysteresis value to be either
                     high/low instead of referring to PM chip specific values.
02/09/04   Vish    Added ID line, UART and USB carkit audio commands. Added
                   new APIs for configuring UART/carkit modes.
01/29/04   Vish    Removed "usb" reference from pm_otg_usb_pwr_sel() function
                   name as it is more appropriate.
01/28/04   rmd     Added initial support for multiple PMIC models/tiers.
01/16/04   rmd     Added the ability to select the USB transceiver power
                   source.
10/20/03   Vish    Added VBUS configuration commands.
09/12/03   Vish    Changed the signature of pm_otg_reset_mode() to return an
                   error.
09/04/03   Vish    Created.
===========================================================================*/


/*===========================================================================
                         TYPE DEFINITIONS
===========================================================================*/

// USB-OTG device type
typedef enum
{
   PM_OTG_DEV__A,         // OTG A-device
   PM_OTG_DEV__B,         // OTG B-device
   PM_OTG_DEV__INVALID
} pm_otg_dev_type;

// USB transceiver power source options.
typedef enum
{
   PM_OTG_PWR_SEL__BOOST,    // VREG_5V (internal +5V)
   PM_OTG_PWR_SEL__VDD,      // PM6658:power from either VDD or USB_VBUS 
   PM_OTG_PWR_SEL__VBUS,     // USB_VBUS (external device)
   PM_OTG_PWR_SEL__OFF,
   PM_OTG_PWR_SEL__INVALID
} pm_otg_pwr_sel_type;

// USB data signaling rate
typedef enum
{
   PM_OTG_USB_SPEED__LOW,          //  LOW speed nominally @  1.5 Mbps
   PM_OTG_USB_SPEED__FULL,         // FULL speed nominally @ 12.0 Mbps
   PM_OTG_USB_SPEED__INVALID
} pm_otg_usb_speed_type;

// Commands for managing transceiver power utilization
typedef enum
{
   PM_OTG_POWER_MANAGEMENT_CMD__SHUTDOWN,    // Everything within the transceiver
                                             // is turned OFF; draws minimal power.
   PM_OTG_POWER_MANAGEMENT_CMD__SUSPEND,     // Same as SHUTDOWN except that Vbus/ID
                                             // line voltage monitoring blocks remain
                                             // powered ON. Transceiver cannot transfer
                                             // data to/from the external connected
                                             // OTG device but can detect plugging in/out
                                             // of the external OTG device.
   PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE,    // Transceiver is made fully functional
   PM_OTG_POWER_MANAGEMENT_CMD__INVALID
} pm_otg_power_management_cmd_type;

// Commands for configuring VBUS line interface
typedef enum
{
   PM_OTG_VBUS_CONFIG_CMD__DRV,              // Power VBUS with 5V supply
   PM_OTG_VBUS_CONFIG_CMD__DISABLE_DRV,      // Disable 5V power supply to VBUS
   PM_OTG_VBUS_CONFIG_CMD__CHG,              // Charge VBUS through a resistor
   PM_OTG_VBUS_CONFIG_CMD__DISABLE_CHG,      // Stop charging VBUS
   PM_OTG_VBUS_CONFIG_CMD__DISCHG,           // Discharge VBUS through a resistor to ground
   PM_OTG_VBUS_CONFIG_CMD__DISABLE_DISCHG,   // Stop discharging VBUS
   PM_OTG_VBUS_CONFIG_CMD__NONE,             // Leave the interface unchanged
   PM_OTG_VBUS_CONFIG_CMD__INVALID
} pm_otg_vbus_config_cmd_type;

// Commands for configuring D+ data line interface
typedef enum
{
   PM_OTG_DP_CONFIG_CMD__PULL_UP,            // Pulls up to Vterm (termination voltage between
                                             //    3.0 and 3.6V) through a resistor Rpu of
                                             //    nominal value 1.5K
   PM_OTG_DP_CONFIG_CMD__DISABLE_PULL_UP,    // Disconnects Rpu
   PM_OTG_DP_CONFIG_CMD__PULL_DOWN,          // Pulls down to the ground through a resistor Rpd
                                             //    of nominal value 15K
   PM_OTG_DP_CONFIG_CMD__DISABLE_PULL_DOWN,  // Disconnects Rpd
   PM_OTG_DP_CONFIG_CMD__ENABLE_BOTH,        // Connects both Rpu and Rpd
   PM_OTG_DP_CONFIG_CMD__DISABLE_BOTH,       // Disconnects both Rpu and Rpd
   PM_OTG_DP_CONFIG_CMD__NONE,               // Leave the interface unchanged
   PM_OTG_DP_CONFIG_CMD__INVALID
} pm_otg_dp_config_cmd_type;

// Commands for configuring D- data line interface
typedef enum
{
   PM_OTG_DM_CONFIG_CMD__PULL_UP,            // Pulls up to Vterm (termination voltage between
                                             //    3.0 and 3.6V) through a resistor Rpu of
                                             //    nominal value 1.5K
   PM_OTG_DM_CONFIG_CMD__DISABLE_PULL_UP,    // Disconnects Rpu
   PM_OTG_DM_CONFIG_CMD__PULL_DOWN,          // Pulls down to the ground through a resistor Rpd
                                             //    of nominal value 15K
   PM_OTG_DM_CONFIG_CMD__DISABLE_PULL_DOWN,  // Disconnects Rpd
   PM_OTG_DM_CONFIG_CMD__ENABLE_BOTH,        // Connects both Rpu and Rpd
   PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH,       // Disconnects both Rpu and Rpd
   PM_OTG_DM_CONFIG_CMD__NONE,               // Leave the interface unchanged
   PM_OTG_DM_CONFIG_CMD__INVALID
} pm_otg_dm_config_cmd_type;

// Commands for configuring ID line interface
typedef enum
{
   PM_OTG_ID_CONFIG_CMD__CONNECT_TO_GROUND,          // Connect ID to ground
   PM_OTG_ID_CONFIG_CMD__DISCONNECT_FROM_GROUND,     // Disconnect ID from ground
   PM_OTG_ID_CONFIG_CMD__NONE,                       // Leave the interface unchanged
   PM_OTG_ID_CONFIG_CMD__INVALID
} pm_otg_id_config_cmd_type;

// Modulation characteristic of the UART driver in the OTG transceiver
typedef enum
{
   PM_OTG_UART_TX_MODULATION__NONE,                  // Normal default mode: logic (0/1) level at
                                                     //    the output of the xcvr is the same as
                                                     //    that of the input (0/1)
   PM_OTG_UART_TX_MODULATION__LOGIC_INVERSION,       // Inverted mode: logic level (1/0) at the
                                                     //    output is 1's compliment of the input (0/1)
   PM_OTG_UART_TX_MODULATION__INVALID
} pm_otg_uart_tx_modulation_type;

// Carkit audio mode configuration as seen from this OTG transceiver's end
typedef enum
{
   PM_OTG_CARKIT_AUDIO_MODE__MONO_OUT_MIC_IN,        // MONO speaker OUTput + MICrophone INput
   PM_OTG_CARKIT_AUDIO_MODE__MONO_OUT,               // Only MONO speaker OUTput
   PM_OTG_CARKIT_AUDIO_MODE__MIC_IN,                 // Only MICrophone INput
   PM_OTG_CARKIT_AUDIO_MODE__STEREO_OUT,             // STEREO speaker OUTput
   PM_OTG_CARKIT_AUDIO_MODE__TEST_MODE_STEREO_OUT_MIC_IN, // This is only a test mode for enabling
                                                          //  left and right audio buffers along with
                                                          //  switching in MIC_IN path.
                                                          // Qualcomm doesn't recommend this setting
                                                          //  as it creates an unstable right channel 
                                                          //  op-amp configuration with both inputs 
                                                          //  tied together.
                                                          // Warning: Use it at your own risk!!!
   PM_OTG_CARKIT_AUDIO_MODE__NONE,                   // Disable MONO_OUT_MIC_IN/STEREO_OUT/TEST_MODE_STEREO_OUT_MIC_IN
   PM_OTG_CARKIT_AUDIO_MODE__INVALID
} pm_otg_carkit_audio_mode_type;

// Time hysteresis for specifying how long a carkit must ground D+ before the carkit
//    generated interrupt is detected by this OTG transceiver
typedef enum
{
   PM_OTG_CARKIT_INT_HYST__LOW,                      // Hysteresis depends on PM chip revision
                                                     //    25 ms (PM6650: Until Rev. J)
                                                     //    0  ms (PM6650: From Rev. J onwards)
   PM_OTG_CARKIT_INT_HYST__HIGH,                     // Hysteresis of 50 milli-sec
   PM_OTG_CARKIT_INT_HYST__INVALID
} pm_otg_carkit_int_hyst_type;

// Modes for configuring carkit audio bias on Left/Right speaker output channels
typedef enum
{
   PM_OTG_CARKIT_AUDX_BIAS__DISABLED,             // AUD[L/R]_BIAS is disabled unconditionally.
   PM_OTG_CARKIT_AUDX_BIAS__AUDX_EN_FOLLOWER,     // AUD[L/R]_BIAS is enabled conditionally
                                                  // based on speaker output (not MIC input)
                                                  // mode configured using pm_otg_config_carkit_mode()
                                                  // for Left (on D-) and/or Right (on D+) channels.
                                                  //  i.e., for PM_OTG_CARKIT_AUDIO_MODE__MONO_OUT[_MIC_IN] mode,
                                                  //   bias for Left channel is automatically enabled;
                                                  //  for PM_OTG_CARKIT_AUDIO_MODE__STEREO_OUT, bias for
                                                  //   both Left and Right channels are automatically enabled.
   PM_OTG_CARKIT_AUDX_BIAS__INVALID
} pm_otg_carkit_audio_bias_type;

/* see document 80-VB831-14 Rev. A, table 3-154 */
typedef enum
{
    PM_USB_VBUS_CONTROL_OFF,                //off
    PM_USB_VBUS_CONTROL_VREG5V,             //power VREG_USB LDO from VREG_5V (internal +5 V)
    PM_USB_VBUS_CONTROL_VBUS,               //(default) power VREG_USB LDO from USB_VBUS (external device)
    PM_USB_VBUS_CONTROL_VDD,                //power VREG_USB LDO from VDD
    PM_USB_VBUS_CONTROL_INVALID
} pm_vbus_control_type;

/*===========================================================================
                            FUNCTION PROTOTYPES
===========================================================================*/

/*===========================================================================
FUNCTION  pm_otg_pwr_sel                                   EXTERNAL FUNCTION

DESCRIPTION
   This function selects the USB transceiver power source.

PARAMETERS
  1) Name: pwrSel
     - USB transceiver power source.

     Type: pm_otg_pwr_sel_type
     - Valid inputs:
        PM_OTG_PWR_SEL__BOOST = VREG_5V (internal +5V) (default)
        PM_OTG_PWR_SEL__VBUS  = USB_VBUS (external device).

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               --> Operation succeeded
     - PM_ERR_FLAG__SBI_OPT_ERR           --> Communication with PM chip failed
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     --> Illegal value provided for
                                              pm_otg_pwr_sel_type.
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED --> Feature not available on this
                                              version of the PMIC.

DEPENDENCIES
  The following function(s) must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  None.
===========================================================================*/
extern pm_err_flag_type pm_otg_pwr_sel(pm_otg_pwr_sel_type pwrSel);


/*===========================================================================
FUNCTION  pm_otg_reset_mode                                EXTERNAL FUNCTION

DESCRIPTION
   This function resets transceiver before it could be configured again
   for another (or potentially for the same) mode using any of the
   pm_otg_config_<usb/uart/carkit>_mode(...) fn calls.

PARAMETERS
   None.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               --> Operation succeeded
     - PM_ERR_FLAG__SBI_OPT_ERR           --> Communication with PM chip failed
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED --> Feature not available on this
                                              version of the PMIC.

DEPENDENCIES
  The following function(s) must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_otg_reset_mode(void);


/*===========================================================================
FUNCTION  pm_otg_config_usb_mode                           EXTERNAL FUNCTION

DESCRIPTION
   This function configures OTG transceiver for USB mode operation. After
   the user is done using the transceiver in USB mode, pm_otg_reset_mode()
   should be called for putting the transceiver in proper state for the next
   use.

PARAMETERS
  1) Name: otg_dev_type
     - The type of OTG device, this transceiver is going to be configured for.
     Type: pm_otg_dev_type (enum).
     - Valid inputs:
          PM_OTG_DEV__A
          PM_OTG_DEV__B
  2) Name: usb_speed
     - The USB data signaling speed, this transceiver is going to be configured
       for.
     Type: pm_otg_usb_speed_type (enum).
     - Valid inputs:
          PM_OTG_USB_SPEED__LOW
          PM_OTG_USB_SPEED__FULL

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               --> Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     --> Illegal value provided for
                                              otg_dev_type.
     - PM_ERR_FLAG__PAR2_OUT_OF_RANGE     --> Illegal value provided for
                                              usb_speed.
     - PM_ERR_FLAG__SBI_OPT_ERR           --> Communication with PM chip failed
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED --> Feature not available on this
                                              version of the PMIC.

DEPENDENCIES
  The following function(s) must have been called:
  1) rflib_init()
  2) pm_init()
  3) pm_otg_reset_mode() should have been called after the last
     pm_otg_config_<usb/uart/carkit>_mode(...)  and before this present
     invocation.

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_otg_config_usb_mode
(
   pm_otg_dev_type         otg_dev_type,
   pm_otg_usb_speed_type   usb_speed
);


/*===========================================================================
FUNCTION  pm_otg_config_power_management                   EXTERNAL FUNCTION

DESCRIPTION
   This function selectively turns ON/OFF certain blocks within the
   transceiver for efficient power utilization.

PARAMETERS
  1) Name: power_management_cmd
     - This command turns ON/OFF select modules within the transceiver for
       efficient power consumption.
     Type: pm_otg_power_management_cmd_type (enum).
     - Valid inputs:
          PM_OTG_POWER_MANAGEMENT_CMD__SHUTDOWN
          PM_OTG_POWER_MANAGEMENT_CMD__SUSPEND
          PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               --> Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     --> Illegal value provided for
                                              power_management_cmd.
     - PM_ERR_FLAG__SBI_OPT_ERR           --> Communication with PM chip failed
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED --> Feature not available on this
                                              version of the PMIC.

DEPENDENCIES
  The following function(s) must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_otg_config_power_management
(
   pm_otg_power_management_cmd_type power_management_cmd
);


/*===========================================================================
FUNCTION  pm_otg_config_interface                          EXTERNAL FUNCTION

DESCRIPTION
   This function configures the 4 USB interface lines - Vbus, D+, D- and ID
   lines at the same time. If the current configuration of any of these lines
   shouldn't be changed, then use the corresponding
   PM_OTG_<VBUS/DP/DM/ID>_CONFIG_CMD__NONE command. It is to be noted that
   all commands for a particular interface line are mutually exclusive of each
   other.

PARAMETERS
  1) Name: vbus_config_cmd
     - Command for changing the Vbus interface.
     Type: pm_otg_vbus_config_cmd_type (enum).
     - Valid inputs:
          PM_OTG_VBUS_CONFIG_CMD__DRV
          PM_OTG_VBUS_CONFIG_CMD__DISABLE_DRV
          PM_OTG_VBUS_CONFIG_CMD__CHG
          PM_OTG_VBUS_CONFIG_CMD__DISABLE_CHG
          PM_OTG_VBUS_CONFIG_CMD__DISCHG
          PM_OTG_VBUS_CONFIG_CMD__DISABLE_DISCHG
          PM_OTG_VBUS_CONFIG_CMD__NONE
  2) Name: dp_config_cmd
     - Command for changing the D+ data line interface.
     Type: pm_otg_dp_config_cmd_type (enum).
     - Valid inputs:
          PM_OTG_DP_CONFIG_CMD__PULL_UP
          PM_OTG_DP_CONFIG_CMD__DISABLE_PULL_UP
          PM_OTG_DP_CONFIG_CMD__PULL_DOWN
          PM_OTG_DP_CONFIG_CMD__DISABLE_PULL_DOWN
          PM_OTG_DP_CONFIG_CMD__ENABLE_BOTH
          PM_OTG_DP_CONFIG_CMD__DISABLE_BOTH
          PM_OTG_DP_CONFIG_CMD__NONE
  3) Name: dm_config_cmd
     - Command for changing the D- data line interface.
     Type: pm_otg_dm_config_cmd_type (enum).
     - Valid inputs:
          PM_OTG_DM_CONFIG_CMD__PULL_UP
          PM_OTG_DM_CONFIG_CMD__DISABLE_PULL_UP
          PM_OTG_DM_CONFIG_CMD__PULL_DOWN
          PM_OTG_DM_CONFIG_CMD__DISABLE_PULL_DOWN
          PM_OTG_DM_CONFIG_CMD__ENABLE_BOTH
          PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH
          PM_OTG_DM_CONFIG_CMD__NONE
  4) Name: id_config_cmd
     - Command for changing the ID line interface.
     Type: pm_otg_id_config_cmd_type (enum).
     - Valid inputs:
          PM_OTG_ID_CONFIG_CMD__CONNECT_TO_GROUND
          PM_OTG_ID_CONFIG_CMD__DISCONNECT_FROM_GROUND
          PM_OTG_ID_CONFIG_CMD__NONE

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               --> Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     --> Illegal value provided for vbus_config_cmd
     - PM_ERR_FLAG__PAR2_OUT_OF_RANGE     --> Illegal value provided for dp_config_cmd
     - PM_ERR_FLAG__PAR3_OUT_OF_RANGE     --> Illegal value provided for dm_config_cmd
     - PM_ERR_FLAG__PAR4_OUT_OF_RANGE     --> Illegal value provided for id_config_cmd
     - PM_ERR_FLAG__SBI_OPT_ERR           --> Communication with PM chip failed
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED --> Feature not available on this
                                              version of the PMIC.

DEPENDENCIES
  The following function(s) must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_otg_config_interface
(
   pm_otg_vbus_config_cmd_type   vbus_config_cmd,
   pm_otg_dp_config_cmd_type     dp_config_cmd,
   pm_otg_dm_config_cmd_type     dm_config_cmd,
   pm_otg_id_config_cmd_type     id_config_cmd
);


/*===========================================================================
FUNCTION  pm_otg_config_uart_mode                          EXTERNAL FUNCTION

DESCRIPTION
   This function configures OTG transceiver for UART operation over USB 
   lines. D+ is used for Rx and D- is used for Tx. After the user is done
   using the transceiver in UART mode, pm_otg_reset_mode() should be
   called for putting the transceiver in proper state for the next use.

PARAMETERS
   None.

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               --> Operation succeeded
     - PM_ERR_FLAG__SBI_OPT_ERR           --> Communication with PM chip failed
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED --> Feature not available on this
                                              version of the PMIC.

DEPENDENCIES
  The following function(s) must have been called:
  1) rflib_init()
  2) pm_init()
  3) pm_otg_reset_mode() should have been called after the last
     pm_otg_config_<usb/uart/carkit>_mode(...)  and before this present
     invocation.

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_otg_config_uart_mode(void);


/*===========================================================================
FUNCTION  pm_otg_set_uart_tx_modulation                    EXTERNAL FUNCTION

DESCRIPTION
   This function configures how UART driver's Tx output line (USB D- line)
   should transform the logic level arriving from MSM on the USB_SE0 line.

PARAMETERS
  1) Name: uart_tx_modulation
     - This parameter indicates the transformation desired on UART Tx line.
     Type: pm_otg_uart_tx_modulation_type (enum).
     - Valid inputs:
          PM_OTG_UART_TX_MODULATION__NONE             --> No transformation
          PM_OTG_UART_TX_MODULATION__LOGIC_INVERSION  --> Boolean logic level
                                                          inversion; logic low
                                                          on USB_SE0 becomes
                                                          logic high on D- line
                                                          and vice-versa

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               --> Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     --> Illegal value provided for
                                              uart_tx_modulation
     - PM_ERR_FLAG__SBI_OPT_ERR           --> Communication with PM chip failed
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED --> Feature not available on this
                                              version of the PMIC

DEPENDENCIES
  The following function(s) must have been called:
  1) rflib_init()
  2) pm_init()
  3) pm_otg_config_uart_mode()

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_otg_set_uart_tx_modulation
(
   pm_otg_uart_tx_modulation_type   uart_tx_modulation
);


/*===========================================================================
FUNCTION  pm_otg_config_carkit_mode                        EXTERNAL FUNCTION

DESCRIPTION
   This function configures OTG transceiver for carkit mode operation. USB
   lines D+ and D- are used for audio transfer between the phone and the
   carkit. After the user is done using the transceiver in carkit mode,
   pm_otg_reset_mode() should be called for putting the transceiver in proper
   state for the next use.

PARAMETERS
  1) Name: carkit_audio_mode
     - This parameter indicates how D+/- lines are configured for audio transfer.
     Type: pm_otg_carkit_audio_mode_type (enum).
     - Valid inputs:
          PM_OTG_CARKIT_AUDIO_MODE__MONO_OUT_MIC_IN
             - Mono speaker output from phone on D- and microphone input from
               carkit on D+ line.
          PM_OTG_CARKIT_AUDIO_MODE__MONO_OUT
             - Only mono speaker output from phone on D- line.  
          PM_OTG_CARKIT_AUDIO_MODE__MIC_IN
             - Only Microphone input from carkit on D+ line.
          PM_OTG_CARKIT_AUDIO_MODE__STEREO_OUT
             - Stereo speaker output from phone on D- (left channel) and
               D+ (right channel) lines.          
          PM_OTG_CARKIT_AUDIO_MODE__TEST_MODE_STEREO_OUT_MIC_IN
             - This is only a test mode for enabling left and right audio buffers
               along with switching in MIC_IN path.
               Qualcomm doesn't recommend this setting as it creates an unstable
               right channel op-amp configuration with both inputs tied together.
               Warning: Use it at your own risk!!!               
          PM_OTG_CARKIT_AUDIO_MODE__NONE
             - Disable mono speaker output/microphone input in case of mono
               mode or left/right speaker channels in case of stereo mode.      

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               --> Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     --> Illegal value provided for
                                              carkit_audio_mode
     - PM_ERR_FLAG__SBI_OPT_ERR           --> Communication with PM chip failed
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED --> Feature not available on this
                                              version of the PMIC.

DEPENDENCIES
  The following function(s) must have been called:
  1) rflib_init()
  2) pm_init()
  3) pm_otg_reset_mode() should have been called after the last
     pm_otg_config_<usb/uart/carkit>_mode(...)  and before this present
     invocation.

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_otg_config_carkit_mode
(
   pm_otg_carkit_audio_mode_type    carkit_audio_mode
);


/*===========================================================================
FUNCTION  pm_otg_set_carkit_interrupt_hysteresis           EXTERNAL FUNCTION

DESCRIPTION
   This function sets the hysteresis delay in the OTG transceiver for
   properly detecting carkit interrupts.

PARAMETERS
  1) Name: carkit_int_hyst
     - This parameter specifies how long a carkit must ground D+ before the
       carkit generated interrupt is detected by this OTG transceiver.
     Type: pm_otg_carkit_int_hyst_type (enum).
     - Valid inputs:
          PM_OTG_CARKIT_INT_HYST__LOW
             - For PM6650 until Rev. J, hysteresis delay is 25 ms;
               From PM6650 Rev. J onwards, this amounts to practically no delay (0 ms)
          PM_OTG_CARKIT_INT_HYST__HIGH
             - Hysteresis delay is 50 ms

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               --> Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     --> Illegal value provided for
                                              carkit_int_hyst
     - PM_ERR_FLAG__SBI_OPT_ERR           --> Communication with PM chip failed
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED --> Feature not available on this
                                              version of the PMIC.

DEPENDENCIES
  The following function(s) must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_otg_set_carkit_interrupt_hysteresis
(
   pm_otg_carkit_int_hyst_type      carkit_int_hyst
);


/*===========================================================================
FUNCTION  pm_otg_enable_audx_pre_bias                      EXTERNAL FUNCTION

DESCRIPTION
   This function sets the desired audio pre-bias configuration for both left
   and right speaker output channels. Note that there is no default
   configuration assumed by this driver, and so any setting done through this
   function will not be affected (reset) by pm_otg_reset_mode().

PARAMETERS
  1) Name: en_audl_pre_bias
     - This parameter specifies the desired audio pre-bias setting for the LEFT
       speaker output channel.
     Type: boolean
     - Valid inputs:
          TRUE  --> Enable pre-bias
          FALSE --> Disable pre-bias
  2) Name: en_audr_pre_bias
     - This parameter specifies the desired audio pre-bias setting for the RIGHT
       speaker output channel.
     Type: boolean
     - Valid inputs:
          TRUE  --> Enable pre-bias
          FALSE --> Disable pre-bias

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               --> Operation succeeded
     - PM_ERR_FLAG__SBI_OPT_ERR           --> Communication with PM chip failed
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED --> Feature not available on this
                                              version of the PMIC.

DEPENDENCIES
  The following function(s) must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_otg_enable_audx_pre_bias
(
   boolean  en_audl_pre_bias,
   boolean  en_audr_pre_bias
);


/*===========================================================================
FUNCTION  pm_otg_set_audx_bias                             EXTERNAL FUNCTION

DESCRIPTION
   This function sets the desired audio bias configuration for both left
   and right speaker output channels. Note that there is no default
   configuration assumed by this driver, and so any setting done through this
   function will not be affected (reset) by pm_otg_reset_mode().

PARAMETERS
  1) Name: audl_bias
     - This parameter specifies the desired audio bias setting for the LEFT
       speaker output channel.
     Type: pm_otg_carkit_audio_bias_type (enum).
     - Valid inputs:
          PM_OTG_CARKIT_AUDX_BIAS__DISABLED
          PM_OTG_CARKIT_AUDX_BIAS__AUDX_EN_FOLLOWER
  2) Name: audr_bias
     - This parameter specifies the desired audio bias setting for the RIGHT
       speaker output channel.
     Type: pm_otg_carkit_audio_bias_type (enum).
     - Valid inputs:
          PM_OTG_CARKIT_AUDX_BIAS__DISABLED
          PM_OTG_CARKIT_AUDX_BIAS__AUDX_EN_FOLLOWER

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS               --> Operation succeeded
     - PM_ERR_FLAG__PAR1_OUT_OF_RANGE     --> Illegal value provided for audl_bias     
     - PM_ERR_FLAG__PAR2_OUT_OF_RANGE     --> Illegal value provided for audr_bias
     - PM_ERR_FLAG__SBI_OPT_ERR           --> Communication with PM chip failed
     - PM_ERR_FLAG__FEATURE_NOT_SUPPORTED --> Feature not available on this
                                              version of the PMIC.

DEPENDENCIES
  The following function(s) must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern pm_err_flag_type pm_otg_set_audx_bias
(
   pm_otg_carkit_audio_bias_type  audl_bias,
   pm_otg_carkit_audio_bias_type  audr_bias
);

/*===========================================================================
FUNCTION  pm_otg_get_vbus_current_sourcing_capability        EXTERNAL FUNCTION

DESCRIPTION
   This function returns current sourcing limit of the PMIC

PARAMETERS
  None
  
RETURN VALUE
   Type: uint32  
     - PMIC current sourcing capacity when the phone acts as a host in mA

DEPENDENCIES
  The following function(s) must have been called:
  1) rflib_init()
  2) pm_init()

SIDE EFFECTS
  Interrupts are disabled during this function.
===========================================================================*/
extern uint32 pm_otg_get_vbus_current_sourcing_capability(void);

/**
 * Function:  pm_usb_vbus_control_set
 * 
 * Used to control voltage source for USB LDOs.  New for Han.
 * 
 * @param data   PM_USB_VBUS_CONTROL_OFF=off
 *               PM_USB_VBUS_CONTROL_VREG5V=power
 *               VREG_USB LDO from VREG_5V (internal +5 V)
 *               PM_USB_VBUS_CONTROL_VBUS=(default) power
 *               VREG_USB LDO from USB_VBUS (external device)
 *               PM_USB_VBUS_CONTROL_VDD=power VREG_USB LDO from VDD
 *               PM_USB_VBUS_CONTROL_INVALID
 * 
 * @return       pm_err_flag_type
 */
extern pm_err_flag_type pm_usb_vbus_control_set(pm_vbus_control_type data);
/**
 * Function:  pm_usb_vbus_ldo_en_set
 * 
 * Sets or clears the USB_LDO_EN bit in the VBUS_CTRL_1 register
 * on Han
 * 
 * @param data   TRUE:  set the bit
 *               FALSE: clear the bit
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_usb_vbus_ldo_en_set(boolean data);
/**
 * Function:  pm_usb_vbus_chg_set
 * 
 * Sets or clears the VBUS_CHG bit in the VBUS_CTRL_1 register
 * on Han
 * 
 * @param data   TRUE:  set the bit
 *               FALSE: clear the bit
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_usb_vbus_chg_set(boolean data);
/**
 * Function:  pm_usb_vbus_dischg_set
 * 
 * Sets or clears the VBUS_DISCHG bit in the VBUS_CTRL_1
 * register on Han
 * 
 * @param data   TRUE:  set the bit
 *               FALSE: clear the bit
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_usb_vbus_dischg_set(boolean data);
/**
 * Function:  pm_usb_vbus_comp_en_set
 * 
 * Sets or clears the VBUS_COMP_EN bit in the VBUS_CTRL_1
 * register on Han
 * 
 * @param data   TRUE:  set the bit
 *               FALSE: clear the bit
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_usb_vbus_comp_en_set(boolean data);
/**
 * Function:  pm_usb_vbus_drv_en_set
 * 
 * Sets or clears the VBUS_DRV bit in the VBUS_CTRL_1
 * register on Han
 * 
 * @param data   TRUE:  set the bit
 *               FALSE: clear the bit
 * 
 * @return pm_err_flag_type
 */
extern pm_err_flag_type pm_usb_vbus_drv_set(boolean data);
/**
 * Function:  pm_usb_vbus_sess_comp_en_set
 * 
 * Sets or clears the SESS_COMP_EN bit in the VBUS_CTRL_1 register
 * on Han/kip B0/C0
 * 
 * @param data   TRUE:  set the bit
 *               FALSE: clear the bit
 * 
 * @return pm_err_flag_type
 */
pm_err_flag_type pm_usb_vbus_sess_comp_en_set(boolean data);


#endif /* PM66XXOTG_H */
