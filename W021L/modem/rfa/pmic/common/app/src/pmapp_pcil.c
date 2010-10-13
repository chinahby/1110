/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

 PMIC DRIVER SERVICES FOR PROC COMM

GENERAL DESCRIPTION
     This file contains functions and variable declarations to support 
  the PROC COMM interface to the PMIC driver.

INITIALIZATION AND SEQUENCING REQUIREMENTS                            
  None.

Copyright (c) 2008, 2009 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/src/pmapp_pcil.c#11 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/24/09   hs      Added proc_comm functions to support USB charging.
03/19/09   APU     Removed the header that perforce while auto-resolving.
03/16/09   APU     Added SMEM support for vbatt_read_mv() API
03/03/09   hs      Modified pm_chg_is_charging_pcil().
01/26/09   vk      Resolved dependency on DEMNvRead API
                   Added PROCOMM support for MPP digital input
11/17/08   jtn     Changed PCIL functions to type boolean instead of void to
                   match change in mproc/smem.
                   Changed DEM header file and prototype to match change in DEM
                   code
09/16/08   jtn     Move PMIC-related proc comm functions from smem files
                   to PMIC
09/04/08   jtn     Use macro to register proc comm functions
08/03/08   jtn     Change all proc comm functions to "static"
08/01/08   jtn     Move all proc comm functions to pmapp_pcil.c
05/13/08   jlk     initial version split off from pm66XXvregs_i.c
===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "pm.h"

#ifdef FEATURE_PMIC_PROCCOMM_SUPPORT
#include "smem_pcmod.h"
#include "vbatt.h"
#include "nv.h"
#include "pmapp_nv.h"
#include "charger.h"

/*===========================================================================

                   Preprocessor Definitions and Constants
===========================================================================*/
/* Use only if API is called in context of DEM task */
#define DEM_TASK_NV_RD_WR_SIG          0x80000000
            
/*===========================================================================

                 PMIC driver PROC COMM interface functions

===========================================================================*/
/*===========================================================================
FUNCTION      pm_vote_vreg_switch_pcil

DESCRIPTION   wrapper function for pm_vote_vreg_switch for use with proc comm

ARGUMENTS     
              vreg_id  <see regulator definitions in 
                       drivers\pmic\pmic3\lib\interface\pm66XXvregs.h> 
                       definitions in pm_vreg_id_type include all PMIC versions, 
                       not all vregs are available on all PMICs 
              cmd_on_off = 0 vote to turn regulator off 
                         = 1 turn regulator on 

DEPENDENCIES  None

RETURN VALUE  pm_vote_vreg_switch returns void.  There isn't an smem function type to 
              match, so pm_vote_vreg_switch_pc returns 1

SIDE EFFECTS  None

CONTEXT 
===========================================================================*/
static boolean pm_vote_vreg_switch_pcil(
                                       uint32 *vreg_id, 
                                       uint32 *cmd_on_off
                                       )
{
    pm_switch_cmd_type switch_cmd;
    pm_vreg_id_type vreg;

    if (*cmd_on_off == 1)
        switch_cmd = PM_ON_CMD;
    else
        switch_cmd = PM_OFF_CMD;

    vreg = (pm_vreg_id_type) *vreg_id;

    pm_vote_vreg_switch(switch_cmd,
                        vreg,
                        PM_VOTE_VREG_LINUX_APP__ALL_VREGS); 
    return TRUE;  
}/*pm_vote_vreg_switch_pcil*/

/*===========================================================================
FUNCTION      pm_vreg_set_level_pcil

DESCRIPTION   Wrapper function for pm_vreg_set_level for use with proc comm

ARGUMENTS     vreg_id  <see regulator definitions in drivers\pmic\pmic3\lib\interface\pm66XXvregs.h> 
              definitions in pm_vreg_id_type include all PMIC versions, not 
              all vregs are available on all PMICs 
              voltage 0-3600 in millivolts 

DEPENDENCIES  None

RETURN VALUE    Always returns TRUE.  The following is returned in vreg_id:
                pm_err_flag_type, cast as uint32 for proc comm
                PM_ERR_FLAG__PAR1_OUT_OF_RANGE    = Input Parameter one is out of range.
                PM_ERR_FLAG__PAR2_OUT_OF_RANGE    = Input Parameter two is out of range.
                PM_ERR_FLAG__FEATURE_NOT_SUPPORTED= Feature not available on this 
                                                    version of the PMIC. 
                PM_ERR_FLAG__SBI_OPT_ERR          = The SBI driver failed to communicate 
                                                    with the PMIC.                      
                PM_ERR_FLAG__SUCCESS              = SUCCESS.    

SIDE EFFECTS  None
CONTEXT  
===========================================================================*/
static boolean pm_vreg_set_level_pcil(
                                     uint32 *vreg_id, 
                                     uint32 *voltage
                                     )
{
    pm_vreg_id_type vreg;
    uint16 level;

    level = (uint16) *voltage;

    vreg = (pm_vreg_id_type) *vreg_id;
    *vreg_id = (uint32) pm_vreg_set_level(vreg, level);
    return TRUE;
} /* pm_vreg_set_level_pcil */

/**
 * This is a wrapper function for proc_comm access to the
 * pm_secure_mpp_config_digital_output API.
 * 
 * See comments for pm_secure_mpp_config_digital_output for
 * detailed description of valid settings and return values.
 * 
 * @param param1 pm_mpp_which_type 
 * @param param2 pm_mpp_dlogic_lvl_type << 16 | pm_mpp_dlogic_out_ctrl_type
 *               The upper 16 bits in param2 is the logic level,
 *               the lower 16 bits is the output control.
 * 
 * @return always true
 */
static boolean pm_secure_mpp_config_digital_output_pcil(
                                                       uint32* param1,
                                                       uint32* param2
                                                       )
{
    pm_mpp_which_type           mpp;
    pm_mpp_dlogic_lvl_type      level;
    pm_mpp_dlogic_out_ctrl_type output_ctrl;

    mpp = (pm_mpp_which_type) *param1;
    level = (pm_mpp_dlogic_lvl_type) (*param2 >> 16);
    output_ctrl = (pm_mpp_dlogic_out_ctrl_type) (*param2 & 0x0000FFFF);

    (void) pm_secure_mpp_config_digital_output(mpp,
                                               level,
                                               output_ctrl);

    return TRUE;
}
/**
 * pm_vreg_pull_down_switch_pc(uint32 *cmd_on_off,
 *                             uint32 *vreg_id)
 * 
 * This API provides a proc comm interface for the Apps
 * processor to control PMIC VREG pulldown resistors.
 * 
 * The pulldown resistor should be turned on to bring
 * the VREG output to ground when it is turned off.
 * The pulldown resistor can be turned off to save power.
 * 
 * @param cmd_on_off    PM_ON_CMD will enable the pulldown resistor.
 *                      PM_OFF_CMD will disable the pulldown resistor.
 * @param vreg_pdown_id This is a uint32* for proc comm, but the parameter must
 *                      be one of the enumerations in pm_vreg_pdown_id_type
 * 
 * @return boolean
 * @example 
 * uint32 vreg;
 * uint32 cmd;
 * 
 * //enable the pulldown resistor for VREG_AUX
 * vreg = (uint32) PM_VREG_PDOWN_AUX_ID;
 * cmd = PM_ON_CMD;
 * boolean err_flag;
 * 
 * err_flag = pm_vreg_pull_down_switch_pc(&cmd, &vreg);
 * 
 * //disable the pulldown resistor for VREG_AUX
 * vreg = (uint32) PM_VREG_PDOWN_AUX_ID;
 * cmd = PM_OFF_CMD;
 * boolean err_flag;
 * 
 * err_flag = pm_vreg_pull_down_switch_pc(&cmd, &vreg);
 */
static boolean pm_vreg_pull_down_switch_pcil(
                                            uint32 *cmd_on_off, 
                                            uint32 *vreg_pdown_id
                                            )
{
    pm_err_flag_type err_flag;

    err_flag = pm_vreg_pull_down_switch((pm_switch_cmd_type)    *cmd_on_off, 
                                        (pm_vreg_pdown_id_type) *vreg_pdown_id);

    if (err_flag == PM_ERR_FLAG__SUCCESS)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/**
   @brief PCIL function that gets the pm wake up status

   This pcil function gets the wake up status from pm_get_wake_up_status.  Called
   in the ISR context.

   @param   status_loc    [output] the wake up satus from pm_get_wake_up_status.
   @param   ignore        ignored, used to match pcil format
   @return  TRUE if the status is obtained successfully.
*/
static boolean pm_get_wake_up_status_pcil(
                                      uint32 *status_loc,
                                      uint32 *ignore
                                      )
{
    boolean      ret_val = TRUE;

    if ( pm_get_wake_up_status( status_loc ) != PM_ERR_FLAG__SUCCESS )
    {
        ret_val = FALSE;
    }

    return( ret_val );

} /* get_wake_up_status_pcil */
/*===========================================================================

FUNCTION      pm_get_batt_level_pcil
                                                                             
DESCRIPTION   Calls vbatt_read() to get the current battery level.   

ARGUMENTS     batt_level_loc  Pointer to the location to write the battery 
                              level.
              dummy           unused parameter to match proc comm function type
                  
DEPENDENCIES  None
      
RETURN VALUE  boolean - always TRUE
  
SIDE EFFECTS  None  

CONTEXT       ISR  Called when this processor receives a command
                   to get the current battery level.

===========================================================================*/
static boolean pm_get_batt_level_pcil
(
    uint32 *batt_level_loc,
    uint32 *dummy
)
{
    *batt_level_loc = ( uint32 )vbatt_read();
    return TRUE;
} /* smem_proc_comm_get_batt_level */
/*===========================================================================

FUNCTION      pm_chg_is_charging_pcil
                                                                             
DESCRIPTION   Calls chg_is_charging() to see if the modem is actively 
              charging the battery or not.

ARGUMENTS     chg_loc   Pointer to the location to write whether the charger is
                        currently charging.
              supported Pointer to the location to write whether the charging
                        feature is supported in the build.

DEPENDENCIES  None
      
RETURN VALUE  boolean - TRUE The SMEM command has been excuted successfully.
 
SIDE EFFECTS  None  

CONTEXT       ISR  Called when this processor receives a command
                   to see if the battery is being charged.

===========================================================================*/
static boolean pm_chg_is_charging_pcil
( 
    uint32 *chg_loc,
    uint32 *supported
)
{     
    *supported = ( uint32 )TRUE;

    #ifdef FEATURE_BATTERY_CHARGER
    *chg_loc = ( uint32 )chg_is_charging();

    #else /* !FEATURE_BATTERY_CHARGER */
    *chg_loc = ( uint32 )FALSE;
    *supported = ( uint32 )FALSE;

    #endif /* !FEATURE_BATTERY_CHARGER */
    return TRUE;
} /* pm_chg_is_charging_pcil */

/*===========================================================================

FUNCTION      pm_get_power_on_status_pcil
                                                                             
DESCRIPTION   Calls the function to get the reason that we powered on 
              (i.e., END key press, USB cable inserted, charger inserted).
              Also reads the NV_FTM_MODE_I.

ARGUMENTS     status_loc  Pointer to the location to write the power on 
                          status.
              ftm_loc     Pointer to the location to write the value
                          returned from demmod_nv_read. If NV read
                          fails this would be set to 0xFF.
                  
DEPENDENCIES  The third argument to smem_proc_comm_register_cmd() is “FALSE”, 
              so the command will be executed in DEM task context.

      
RETURN VALUE  TRUE pm_get_power_status(status loc) == PM_ERR_FLAG_SUCCESS

SIDE EFFECTS  None  

CONTEXT       DEM   Called when this processor receives a command
                    to get the power on reason.

===========================================================================*/
static boolean pm_get_power_on_status_pcil
(
    uint32 *status_loc,
    uint32 *ftm_loc
)
{
    boolean      ret_val = TRUE;
    nv_item_type ftm_mode_read;

    if ( pm_get_power_on_status( status_loc ) != PM_ERR_FLAG__SUCCESS )
    {
        ret_val = FALSE;
    }
    else
    {
        /* The pm_get_nv_item funtion does not change the value of 
         * the data if the NV read fails. Set it to 0xFF for
         * failure indication to caller. */
        ftm_mode_read.ftm_mode = 0xFF;
        if ( pm_get_nv_item(   NV_FTM_MODE_I,
                               &ftm_mode_read,
                               rex_self(),
                               DEM_TASK_NV_RD_WR_SIG,  
                               (void (*)( rex_sigs_type ))rex_wait ) == NV_DONE_S )
        {
            *ftm_loc = (uint32)ftm_mode_read.ftm_mode;
        }
    }
    return( ret_val );

} /* pm_get_power_on_status_pcil */

/**
 * This is a wrapper function for proc_comm access to the
 pm_secure_mpp_config_i_sink API.
 * 
 * See comments for pm_secure_mpp_config_i_sink for
 * detailed description of valid settings and return values.
 * 
 * @param param1 pm_mpp_which_type 
 * @param param2 pm_mpp_i_sink_level_type << 16 | pm_mpp_i_sink_switch_type
 *               The upper 16 bits in param2 is the logic level,
 *               the lower 16 bits is the i_sink switch type.
 * 
 * @return always true
 */
static boolean pm_secure_mpp_config_i_sink_pcil
(
    uint32* param1,
    uint32* param2
                                                       
)
{
    pm_mpp_which_type           mpp;
    pm_mpp_i_sink_level_type    level;
    pm_mpp_i_sink_switch_type   OnOff;

    mpp = (pm_mpp_which_type) *param1;
    level = (pm_mpp_i_sink_level_type) (*param2 >> 16);
    OnOff = (pm_mpp_i_sink_switch_type) (*param2 & 0x0000FFFF);

    (void) pm_secure_mpp_config_i_sink (mpp,
                                        level,
                                        OnOff);

    return TRUE;
}

/**
 * This is a wrapper function for proc_comm access to the
 * pm_secure_mpp_config_digital_input API.
 * 
 * See comments for pm_secure_mpp_config_digital_input for
 * detailed description of valid settings and return values.
 * 
 * @param param1 pm_mpp_which_type 
 * @param param2 pm_mpp_dlogic_lvl_type << 16 | pm_mpp_dlogic_in_dbus_type
 *               The upper 16 bits in param2 is the logic level,
 *               the lower 16 bits is the data line.
 * 
 * @return always true
 */
static boolean pm_secure_mpp_config_digital_input_pcil
(
    uint32* param1,
    uint32* param2
)
{
    pm_mpp_which_type           mpp;
    pm_mpp_dlogic_lvl_type      level;
    pm_mpp_dlogic_in_dbus_type  dbus;

    mpp = (pm_mpp_which_type) *param1;
    level = (pm_mpp_dlogic_lvl_type) (*param2 >> 16);
    dbus = (pm_mpp_dlogic_in_dbus_type) (*param2 & 0x0000FFFF);

    (void) pm_secure_mpp_config_digital_input(mpp,
                                              level,
                                              dbus);

    return TRUE;
}


/*===========================================================================

FUNCTION      pm_get_batt_level_mv_pcil
                                                                             
DESCRIPTION   Calls vbatt_read_mv () to get the current battery level.   

ARGUMENTS     batt_level_loc  Pointer to the location to write the battery 
                              level.
              dummy           unused parameter to match proc comm function type
                  
DEPENDENCIES  None
      
RETURN VALUE  boolean - always TRUE
  
SIDE EFFECTS  None  

CONTEXT       TASK Called when this processor receives a command
                   to get the current battery level.

===========================================================================*/
static boolean pm_get_batt_level_mv_pcil
(
    uint32 *batt_level_loc,
    uint32 *dummy
)
{
    *batt_level_loc = ( uint32 ) vbatt_read_mv () ;
    return TRUE;
} /* smem_proc_comm_get_batt_mv_level */

/*===========================================================================

FUNCTION      pm_chg_usb_is_pc_connected_pcil
                                                                             
DESCRIPTION   Apps processor calls this API to tell modem processor that 
              a PC USB is connected

ARGUMENTS     supported Pointer to the location to write whether the charging
                        feature is supported in the build.
              ignore    ignored (used to fit function format)

DEPENDENCIES  None
      
RETURN VALUE  boolean - TRUE The SMEM command has been excuted successfully.
 
SIDE EFFECTS  None  

CONTEXT       ISR  Called when this processor receives a command
                   to inform that a PC USB is connected.

===========================================================================*/
static boolean pm_chg_usb_is_pc_connected_pcil
( 
    uint32 *supported,
    uint32 *ignore
)
{       
    *supported = ( uint32 )TRUE;

    #ifdef FEATURE_BATTERY_CHARGER
    chg_usb_charger_connected(PM_APP_OTG_A_DEV_TYPE__USB_HOST);

    #else /* !FEATURE_BATTERY_CHARGER */
    *supported = ( uint32 )FALSE;

    #endif /* !FEATURE_BATTERY_CHARGER */
    return TRUE;
} /* pm_chg_usb_is_pc_connected_pcil */

/*===========================================================================

FUNCTION      pm_chg_usb_is_charger_connected_pcil
                                                                             
DESCRIPTION   Apps processor calls this API to tell modem processor that 
              a USB Wall charger is connected

ARGUMENTS     supported Pointer to the location to write whether the charging
                        feature is supported in the build.
              ignore    ignored (used to fit function format)

DEPENDENCIES  None
      
RETURN VALUE  boolean - TRUE The SMEM command has been excuted successfully.
 
SIDE EFFECTS  None  

CONTEXT       ISR  Called when this processor receives a command
                   to inform that a USB wall charger is connected

===========================================================================*/
static boolean pm_chg_usb_is_charger_connected_pcil
( 
    uint32 *supported,
    uint32 *ignore
)
{       
    *supported = ( uint32 )TRUE;

    #ifdef FEATURE_BATTERY_CHARGER
    chg_usb_charger_connected(PM_APP_OTG_A_DEV_TYPE__USB_CHARGER);

    #else /* !FEATURE_BATTERY_CHARGER */
    *supported = ( uint32 )FALSE;

    #endif /* !FEATURE_BATTERY_CHARGER */
    return TRUE;
} /* pm_chg_usb_is_charger_connected_pcil */

/*===========================================================================

FUNCTION      pm_chg_usb_is_disconnected_pcil
                                                                             
DESCRIPTION   Apps processor calls this API to tell modem processor that 
              a USB cable is disconnected

ARGUMENTS     supported Pointer to the location to write whether the charging
                        feature is supported in the build.
              ignore    ignored (used to fit function format)

DEPENDENCIES  None
      
RETURN VALUE  boolean - TRUE The SMEM command has been excuted successfully.
 
SIDE EFFECTS  None  

CONTEXT       ISR  Called when this processor receives a command
                   to inform that a USB cable is disconnected

===========================================================================*/
static boolean pm_chg_usb_is_disconnected_pcil
( 
    uint32 *supported,
    uint32 *ignore
)
{       
    *supported = ( uint32 )TRUE;

    #ifdef FEATURE_BATTERY_CHARGER
    chg_usb_charger_disconnected();
    chg_usb_i_is_not_available();

    #else /* !FEATURE_BATTERY_CHARGER */
    *supported = ( uint32 )FALSE;

    #endif /* !FEATURE_BATTERY_CHARGER */
    return TRUE;
} /* pm_chg_usb_is_disconnected_pcil */

/*===========================================================================

FUNCTION      pm_chg_usb_i_available_pcil
                                                                             
DESCRIPTION   Apps processor calls this API to tell modem processor that it's  
              OK to draw the amount of current passed by the parameter

ARGUMENTS     usb_i     Pointer to the location that saves the current 
                        allowed to draw from the USB cable.
              supported Pointer to the location to write whether the charging
                        feature is supported in the build.

DEPENDENCIES  None
      
RETURN VALUE  boolean - TRUE The SMEM command has been excuted successfully.
 
SIDE EFFECTS  None  

CONTEXT       ISR      Called when this processor receives a command
                       to inform the current allowed to draw from
                       the USB cable 

===========================================================================*/
static boolean pm_chg_usb_i_available_pcil
( 
    uint32 *usb_i,
    uint32 *supported
)
{       
    *supported = ( uint32 )TRUE;

    #ifdef FEATURE_BATTERY_CHARGER
    chg_usb_i_is_available(*usb_i);

    #else /* !FEATURE_BATTERY_CHARGER */
    *supported = ( uint32 )FALSE;

    #endif /* !FEATURE_BATTERY_CHARGER */
    return TRUE;
} /* pm_chg_usb_i_available_pcil */

/**
   @brief Registers the functions in this file with proc_comm

*  Calls SMEM_PROC_COMM_REGISTER_CMD() for each of the pcil
*  functions above.
   
   @return  TRUE if all functions were sucessfully registered
*/
boolean pm_register_pcil_cmds(void)
{
    boolean all_reged = TRUE;
 
    all_reged &= SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_VREG_SWITCH,
                                             &pm_vote_vreg_switch_pcil,
                                             TRUE);

    all_reged &= SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_VREG_SET_LEVEL,
                                             &pm_vreg_set_level_pcil,
                                             TRUE); 

    all_reged &= SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_PM_MPP_CONFIG,
                                             &pm_secure_mpp_config_digital_output_pcil,
                                             TRUE);

    all_reged &= SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_PM_MPP_CONFIG_DIGITAL_INPUT,
                                             &pm_secure_mpp_config_digital_input_pcil,
                                             TRUE);

    all_reged &= SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_PM_MPP_CONFIG_I_SINK,
                                             &pm_secure_mpp_config_i_sink_pcil,
                                             TRUE);

    all_reged &= SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_VREG_PULLDOWN ,
                                             &pm_vreg_pull_down_switch_pcil,
                                             TRUE);

    all_reged &= SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_GET_WAKE_UP_STATUS,
                                             &pm_get_wake_up_status_pcil,
                                             TRUE);

    all_reged &= SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_GET_BATT_LEVEL,
                                             &pm_get_batt_level_pcil,
                                             TRUE);

    all_reged &= SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_CHG_IS_CHARGING,
                                            &pm_chg_is_charging_pcil,
                                             TRUE);

    all_reged &= SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_GET_POWER_ON_STATUS,
                                             &pm_get_power_on_status_pcil,
                                             FALSE);

    all_reged &= SMEM_PROC_COMM_REGISTER_CMD ( SMEM_PROC_COMM_GET_BATT_MV_LEVEL ,
                                               &pm_get_batt_level_mv_pcil ,
                                               TRUE ) ;

    all_reged &= SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_CHG_USB_IS_PC_CONNECTED,
                                            &pm_chg_usb_is_pc_connected_pcil,
                                             TRUE);

    all_reged &= SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_CHG_USB_IS_CHARGER_CONNECTED,
                                            &pm_chg_usb_is_charger_connected_pcil,
                                             TRUE);

    all_reged &= SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_CHG_USB_IS_DISCONNECTED,
                                            &pm_chg_usb_is_disconnected_pcil,
                                             TRUE);

    all_reged &= SMEM_PROC_COMM_REGISTER_CMD(SMEM_PROC_COMM_CHG_USB_I_AVAILABLE,
                                            &pm_chg_usb_i_available_pcil,
                                             TRUE);

    return all_reged;
}/*pm_register_pcil_cmds*/

#endif /* FEATURE_PMIC_PROCCOMM_SUPPORT */
