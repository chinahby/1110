/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*



                            FTM PMIC COMMANDS

GENERAL DESCRIPTION
  This is the FTM PMIC test interface 

Copyright (c) 2000, 2001, 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2003, 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2006, 2007       by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/ftm/src/ftm_pmic.c#5 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/03/09   APU     Added extended VREG command.
11/29/07   APU     Fixed a compiler warning in Fuse Module.
07/02/08   hs      Added support for FEATURE_CHG_TASK.
11/29/07   apu     Added fuse commands.
07/23/07   apu     Added peek and poke functionality to PMIC registers.CR# 122589.
06/15/07   cng     Added one more parameter to the pm_mpp_config_analog_output API
05/18/07   hs      Updated mpp FTM support.
04/26/07   ab      Removed et vreg enum ftm api since we are getting errors on string fns. 
                     It will be re added again once we figure out the issue
04/20/07   ab      Fix processing speaker High level mux control commands bug inside ftm_pmic_process_spkr_commands()
10/11/06   ab      Added missing Vregs in get_vereg_enum() 
10/11/06   jf      Added FEATURE_FTM_PMIC_BYPASS
08/04/06   ab      Added get vreg value ftm api, get vreg enum ftm api  
                     and get pmic model ftm api support
05/31/06   hs      Added mpp FTM support.
05/24/06   hs      Added feature to include header file "ftm_diagpkt.h" 
01/04/06   VK      Added speaker, mic, vid FTM support
09/02/05   cng     Removed DVT code
07/25/05   cng     Removed some legacy code
07/01/05   cng     Added the DVT feature
11/03/04   rmd     Added #ifdef around charge.h and chargeri.h to allow 
                   the code to compile if charging is not supported.
10/27/04   rmd     Changed the naming conversion of the pulse charger code 
                   to reflect that the PMIC pulse charger API supports  
                   applies for both wall and usb chargers.
10/12/04   jac     Mainlined ASYNC packet code.
09/13/04   dp      Minor RTC bugfix
04/25/04   dp      Created
===========================================================================*/
#include "customer.h"
#include "target.h"                // Target specific definitions            //
#include "comdef.h"                // Common stuff                           //

#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_PMIC
#ifndef FEATURE_FTM_PMIC_BYPASS

#include <stdio.h>

#include "ftm.h"
#ifdef T_QSC60X0
#include "ftm_diagpkt.h"
#else
#include "ftm_task.h"
#endif

#include "ftm_pmic.h"
#include "pm.h"
#include "pmapp.h"
#include "pmapp_rtc.h"
#include "pm66XXrtc.h"

#if defined(FEATURE_BATTERY_CHARGER)
  #include "charger.h"
  #include "chargeri.h"
#elif defined(FEATURE_CHG_TASK)
  #include "charger.h"
#elif defined( FEATURE_BATMAN_TASK)
#error code not present
#endif

#include "msg.h"                     /* Debug related stuff                         */     


#include "pmgen.h" 
#include "adc.h"
#include "pm66XXmpps.h"

/*===========================================================================
                Define global variables  
===========================================================================*/


/*===========================================================================
                Define LOCAL variables  
===========================================================================*/

pm_err_flag_type ftm_chg_app_enable( pm_switch_cmd_type);


#if defined( FEATURE_PMIC_RTC)
ftm_rsp_pkt_type ftm_pmic_process_rtc_commands(        ftm_pmic_pkt_type *cmd_ptr);
void ftm_pmic_rtc_alarm_isr_cb( void);
#endif

ftm_rsp_pkt_type ftm_pmic_process_vreg_commands(       ftm_pmic_pkt_type *cmd_ptr);

/* FTM extended commands for external PMICs */
ftm_rsp_pkt_type 
ftm_pmic_process_vreg_commands_ext ( ftm_pmic_pkt_type *cmd_ptr ) ;

ftm_rsp_pkt_type ftm_pmic_process_chg_commands(        ftm_pmic_pkt_type *cmd_ptr);
ftm_rsp_pkt_type ftm_pmic_process_amux_commands(       ftm_pmic_pkt_type *cmd_ptr);
ftm_rsp_pkt_type ftm_pmic_process_chg_config_commands( ftm_pmic_pkt_type *cmd_ptr, pm_err_flag_type *err );
ftm_rsp_pkt_type ftm_pmic_process_int_commands(        ftm_pmic_pkt_type *cmd_ptr);
ftm_rsp_pkt_type ftm_pmic_process_ui_commands(         ftm_pmic_pkt_type *cmd_ptr);

ftm_rsp_pkt_type ftm_pmic_process_spkr_commands(       ftm_pmic_pkt_type *cmd_ptr);
ftm_rsp_pkt_type ftm_pmic_process_vid_commands(        ftm_pmic_pkt_type *cmd_ptr);
ftm_rsp_pkt_type ftm_pmic_process_mic_commands(        ftm_pmic_pkt_type *cmd_ptr);
ftm_rsp_pkt_type ftm_pmic_process_mpp_commands(        ftm_pmic_pkt_type *cmd_ptr);
ftm_rsp_pkt_type ftm_pmic_process_reset(               ftm_pmic_pkt_type *cmd_ptr);
ftm_rsp_pkt_type ftm_pmic_process_gen_commands(        ftm_pmic_pkt_type *cmd_ptr);

ftm_rsp_pkt_type ftm_pmic_create_gen_response( ftm_pmic_pkt_type *cmd_ptr, uint16 status, uint16 cid, uint16 scid);
ftm_rsp_pkt_type ftm_pmic_create_get_value_response( ftm_pmic_pkt_type *cmd_ptr, uint16 level, uint16 status, uint16 cid, uint16 scid);
ftm_rsp_pkt_type ftm_pmic_create_get_vreg_enum_response( ftm_pmic_pkt_type *cmd_ptr, 
                                                          uint16 num_vregs,
                                                          uint16 list_str_length, 
                                                          char* vreg_id_name_list, 
                                                          uint16 status, uint16 cid, uint16 scid);
ftm_rsp_pkt_type ftm_pmic_process_fuse_commands (         ftm_pmic_pkt_type *cmd_ptr ) ;


/* Table of function pointers to support enable/disable of various charger features */
const ftm_pmic_chg_switch_func_type  switch_func_table[FTM_PMIC_CHG_SWITCH_NUM_SWITCHES] = 
{
    pm_chg_wall_charger_transistor_switch,
    pm_chg_battery_transistor_switch,
    pm_chg_pulse_charger_switch,
    pm_chg_vcp_switch,
    pm_chg_bat_alarm_switch,
    pm_coin_cell_chg_switch,
    pm_chg_usb_charger_transistor_switch,
    ftm_chg_app_enable                      /* Enable/Disable charger state machine */
};

/*===========================================================================

FUNCTION           FTM_PMIC_DISPATCH

DESCRIPTION        Main dispatch routine for PMIC commands.
  
   
DEPENDENCIES
   None.

RETURN VALUE
                   Response packet

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_pmic_dispatch( ftm_pmic_pkt_type *cmd_ptr)
{


    ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};



    switch ( cmd_ptr->ftm_cmd_header.cmd_id )
    {
   
#if defined( FEATURE_PMIC_RTC)
        case FTM_PMIC_TEST_RTC:
      
            rsp_pkt = ftm_pmic_process_rtc_commands( cmd_ptr);
            break;

#endif /* FEATURE_PMIC_RTC */

        case FTM_PMIC_TEST_CHG:
            
            rsp_pkt = ftm_pmic_process_chg_commands( cmd_ptr);
            break;

        case FTM_PMIC_TEST_AMUX:

            rsp_pkt = ftm_pmic_process_amux_commands( cmd_ptr);
            break;

        case FTM_PMIC_TEST_USB:

            MSG_HIGH("PMIC: USB tests not supported",0,0,0);
            break;

        case FTM_PMIC_TEST_VREG:

            rsp_pkt = ftm_pmic_process_vreg_commands( cmd_ptr);
            break;

        case FTM_PMIC_TEST_VREG_EXT:  
            /* FTM PMIC VREG extended command set */
            rsp_pkt = ftm_pmic_process_vreg_commands_ext ( cmd_ptr ) ;
            break ;

        case FTM_PMIC_TEST_INT:
            
            rsp_pkt = ftm_pmic_process_int_commands( cmd_ptr);
            break;

        case FTM_PMIC_TEST_UI:

            rsp_pkt = ftm_pmic_process_ui_commands(cmd_ptr);
            break;

        case FTM_PMIC_TEST_SPKR:
  
            rsp_pkt = ftm_pmic_process_spkr_commands(cmd_ptr);
            break;
  
        case FTM_PMIC_TEST_VID:

            rsp_pkt = ftm_pmic_process_vid_commands(cmd_ptr);
            break;
  
        case FTM_PMIC_TEST_MIC:

            rsp_pkt = ftm_pmic_process_mic_commands(cmd_ptr);
            break;

        case FTM_PMIC_TEST_MPP:

            rsp_pkt = ftm_pmic_process_mpp_commands(cmd_ptr);
            break;

        case FTM_PMIC_TEST_RESET:
            rsp_pkt = ftm_pmic_process_reset(cmd_ptr);
            break;

        case FTM_PMIC_TEST_GEN:
            rsp_pkt = ftm_pmic_process_gen_commands(cmd_ptr);
            break;


        case FTM_PMIC_FUSE_CMDS:
            rsp_pkt = ftm_pmic_process_fuse_commands ( cmd_ptr ) ;
            break;

      default:

            rsp_pkt = ftm_pmic_create_gen_response( cmd_ptr,
                                                    PM_ERR_FLAG__PAR1_OUT_OF_RANGE, 
                                                    cmd_ptr->ftm_cmd_header.cmd_id, 
                                                    0
                                                   );

            MSG_HIGH("PMIC:unknown command", 0, 0, 0);
            break;
    }


    return rsp_pkt;
    
}





/*===========================================================================

FUNCTION      FTM_PMIC_CREATE_GEN_RESPONSE

DESCRIPTION   Allocates and populates a response packet structure, with the 
              result of a particular command.

   
DEPENDENCIES
   None.

RETURN VALUE  Response packet to send back to host.
              

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_pmic_create_gen_response( ftm_pmic_pkt_type *cmd_ptr, uint16 status,
                                               uint16 cid, uint16 scid)
{

    ftm_rsp_pkt_type  gen_rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    ftm_pmic_gen_rsp_pkt_type   *gen_rsp_payload;


    gen_rsp = ftmdiag_create_new_pkt( sizeof( ftm_pmic_gen_rsp_pkt_type));

    if( gen_rsp.pkt_payload != NULL)
    {
        gen_rsp_payload = ( ftm_pmic_gen_rsp_pkt_type *) gen_rsp.pkt_payload;

        gen_rsp_payload->header                  = cmd_ptr->header;
        gen_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_PMIC_GEN_RESPONSE;
        gen_rsp_payload->ftm_rsp_header.rsp_cid  = cid;
        gen_rsp_payload->ftm_rsp_header.rsp_scid = scid;
        gen_rsp_payload->status                  = status;

    }

    return gen_rsp;
}

/*===========================================================================
FUNCTION           FTM_PMIC_CREATE_GET_VALUE_RESPONSE
DESCRIPTION        Allocates and populates a response packet structure for 
                    get voltage regulator value api call response.
DEPENDENCIES       None.
RETURN VALUE       Response packet
SIDE EFFECTS       None.
===========================================================================*/
ftm_rsp_pkt_type ftm_pmic_create_get_value_response( ftm_pmic_pkt_type *cmd_ptr ,
                                                     uint16 level, uint16 status,
                                                     uint16 cid , uint16 scid )
{
    ftm_rsp_pkt_type  gen_rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    ftm_pmic_get_value_rsp_pkt_type *get_value_rsp_payload;
    gen_rsp = ftmdiag_create_new_pkt( sizeof( ftm_pmic_get_value_rsp_pkt_type));
   
    if( gen_rsp.pkt_payload != NULL)
    {
        get_value_rsp_payload = ( ftm_pmic_get_value_rsp_pkt_type *) gen_rsp.pkt_payload;
        get_value_rsp_payload->header                  = cmd_ptr->header;
        get_value_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_PMIC_GEN_RESPONSE;
        get_value_rsp_payload->ftm_rsp_header.rsp_cid  = cid;
        get_value_rsp_payload->ftm_rsp_header.rsp_scid = scid;
        get_value_rsp_payload->level                   = level;
        get_value_rsp_payload->status                  = status;
    }
   
    return gen_rsp;
}
   

/*===========================================================================
FUNCTION           FTM_PMIC_CREATE_FTM_FUSE_RESPONSE
DESCRIPTION        Allocates and populates a response packet structure for 
                    PMIC fuse commands.
DEPENDENCIES       None.
RETURN VALUE       Response packet
SIDE EFFECTS       None.
===========================================================================*/
ftm_rsp_pkt_type ftm_pmic_create_ftm_fuse_response( ftm_pmic_pkt_type *cmd_ptr,
                                                    uint16 status ,
                                                    uint16 cid , uint16 scid )
{
  ftm_rsp_pkt_type  gen_rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  ftm_pmic_gen_rsp_pkt_type *pmic_fuse_rsp_payload ;
  gen_rsp = ftmdiag_create_new_pkt( sizeof( ftm_pmic_gen_rsp_pkt_type) ) ;

  if( gen_rsp.pkt_payload != NULL)
  {
      pmic_fuse_rsp_payload = ( ftm_pmic_gen_rsp_pkt_type *) gen_rsp.pkt_payload ;
      pmic_fuse_rsp_payload->header                  = cmd_ptr->header;
      pmic_fuse_rsp_payload->ftm_rsp_header.rsp_id   = (uint16) FTM_PMIC_GEN_RESPONSE;
      pmic_fuse_rsp_payload->ftm_rsp_header.rsp_cid  = cid;
      pmic_fuse_rsp_payload->ftm_rsp_header.rsp_scid = scid;
      pmic_fuse_rsp_payload->status                  = status;
  }

  return gen_rsp;
}

/*===========================================================================

FUNCTION           FTM_PMIC_PROCESS_SPKR_COMMANDS

DESCRIPTION        Handler for speaker FTM PMIC commands.

DEPENDENCIES
None.

RETURN VALUE       Response packet

SIDE EFFECTS
None.

===========================================================================*/
ftm_rsp_pkt_type  ftm_pmic_process_spkr_commands( ftm_pmic_pkt_type *cmd_ptr)
{
    pm_spkr_hpf_corner_freq_type                freq;
    pm_spkr_gain_type                           gain;
    pm_spkr_dly_type                            delay;
    ftm_pmic_spkr_header_type                   *spkr_header;
    ftm_pmic_spkr_set_get_corner_freq_cmd_type  *spkr_set_get_corner_freq_cmd;
    ftm_pmic_speaker_LL_mux_cntl2_cmd_type      *spkr_LL_mux_cntl2_cmd;
    ftm_pmic_speaker_HL_mux_cntl2_cmd_type      *spkr_HL_mux_cntl2_cmd;
    ftm_pmic_spkr_left_right_en_mute_cmd_type   *spkr_left_right_en_mute_cmd;
    ftm_pmic_spkr_left_right_gain_cmd_type      *spkr_left_right_gain_cmd;
    ftm_pmic_spkr_left_right_delay_cmd_type     *spkr_left_right_delay_cmd;

    pm_spkr_config_mode_type                    queried_config;

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    boolean status = FALSE;

    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    
    spkr_header = (ftm_pmic_spkr_header_type *) cmd_ptr;



    switch( spkr_header->spkr_cmd_id)
    {
        //set-get speaker corner frequency
        case FTM_PMIC_SPKR_SET_GET_HPF_CORNER_FREQ:
            
            spkr_set_get_corner_freq_cmd = ( ftm_pmic_spkr_set_get_corner_freq_cmd_type *) cmd_ptr;
            
            if( spkr_set_get_corner_freq_cmd->ftm_spkr_rw == PM_SPKR_SET_FREQ_CMD)
            {
                err_flag = pm_spkr_set_mux_hpf_corner_freq( spkr_set_get_corner_freq_cmd->freq);
            
            }
            else if( spkr_set_get_corner_freq_cmd->ftm_spkr_rw == PM_SPKR_GET_FREQ_CMD)
            {
                if( pm_spkr_get_mux_hpf_corner_freq(&freq) == PM_ERR_FLAG__SUCCESS )
                {
                        spkr_set_get_corner_freq_cmd->freq = freq;
                }
            }
            else
            {
                err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            }
            break;

        //set & get enable/disable speaker low level configuration 
        case FTM_PMIC_SPKR_LL_CONFIG:

            spkr_LL_mux_cntl2_cmd = ( ftm_pmic_speaker_LL_mux_cntl2_cmd_type *) cmd_ptr;
    
            switch( spkr_LL_mux_cntl2_cmd->spkr_LL_config_cntl2_cmd_id)
            {
                case FTM_PMIC_SPKR_SINK_SET:  
                    err_flag =pm_spkr_en_sink_curr_from_ref_volt_cir(spkr_LL_mux_cntl2_cmd->enable_disable);
                    break;
                case FTM_PMIC_SPKR_SINK_GET:
                    err_flag = pm_spkr_is_sink_curr_from_ref_volt_cir_en(&status);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_LL_mux_cntl2_cmd->enable_disable = status;
                    } 
                    break;
                case FTM_PMIC_SPKR_FILTERDIS_SET: 
                    err_flag =pm_spkr_en_hpf(spkr_LL_mux_cntl2_cmd->enable_disable);
                    break;
                case FTM_PMIC_SPKR_FILTERDIS_GET:
                    err_flag = pm_spkr_is_hpf_en(&status);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_LL_mux_cntl2_cmd->enable_disable = status;
                    } 
                    break;
                case FTM_PMIC_SPKR_BYPASS_SET: 
                    err_flag =pm_spkr_bypass_mux(spkr_LL_mux_cntl2_cmd->enable_disable);
                    break;
                case FTM_PMIC_SPKR_BYPASS_GET:
                    err_flag = pm_spkr_is_mux_bypassed(&status);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_LL_mux_cntl2_cmd->enable_disable = status;
                    } 
                    break;
                case FTM_PMIC_SPKR_USB_SET: 
                    err_flag =pm_spkr_select_usb_with_hpf_20hz(spkr_LL_mux_cntl2_cmd->enable_disable);
                    break;
                case FTM_PMIC_SPKR_USB_GET:
                    err_flag = pm_spkr_is_usb_with_hpf_20hz(&status);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_LL_mux_cntl2_cmd->enable_disable = status;
                    } 
                    break;
                case FTM_PMIC_SPKR_STEREO_SET: 
                    err_flag =pm_spkr_en_stereo(spkr_LL_mux_cntl2_cmd->enable_disable);
                    break;
                case FTM_PMIC_SPKR_STEREO_GET:
                    err_flag = pm_spkr_is_stereo_en(&status);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_LL_mux_cntl2_cmd->enable_disable = status;
                    }
                    break;
                case FTM_PMIC_SPKR_EN_L_R_SET:
                    err_flag =pm_spkr_add_right_left_chan(spkr_LL_mux_cntl2_cmd->enable_disable);
                    break;
                case FTM_PMIC_SPKR_EN_L_R_GET:
                    err_flag = pm_spkr_is_right_left_chan_added(&status);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_LL_mux_cntl2_cmd->enable_disable = status;
                    } 
                    break;
                case FTM_PMIC_SPKR_EN_L_SET:
                    err_flag =pm_spkr_en_left_chan(spkr_LL_mux_cntl2_cmd->enable_disable);
                    break;
                case FTM_PMIC_SPKR_EN_L_GET:
                    err_flag = pm_spkr_is_left_chan_en(&status);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_LL_mux_cntl2_cmd->enable_disable = status;
                    }
                    break;
                case FTM_PMIC_SPKR_EN_R_SET:
                    err_flag =pm_spkr_en_right_chan(spkr_LL_mux_cntl2_cmd->enable_disable);
                    break;
                case FTM_PMIC_SPKR_EN_R_GET:
                    err_flag = pm_spkr_is_right_chan_en(&status);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_LL_mux_cntl2_cmd->enable_disable = status;
                    } 
                    break;
                
                default:
                    err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
                    break;
            }
            break;

        //set & get speaker mode High level configuration status 
        case FTM_PMIC_SPKR_HL_CONFIG:

            spkr_HL_mux_cntl2_cmd = ( ftm_pmic_speaker_HL_mux_cntl2_cmd_type  *) cmd_ptr;

            switch( spkr_HL_mux_cntl2_cmd->spkr_HL_config_cntl2_cmd_id)
            {   case FTM_PMIC_SPKR_STEREO_MODE_SET: 
                    queried_config = pm_speaker_config__stereo;
                    err_flag = pm_set_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                    }
                    break;
                case FTM_PMIC_SPKR_STEREO_MODE_GET:
                    queried_config = pm_speaker_config__default;
                    err_flag = pm_get_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   
                        if(memcmp( (void*)&pm_speaker_config__stereo, (void*)&queried_config,
                                   sizeof(pm_spkr_config_mode_type)) == 0 )
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                        }
                        else
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = FALSE;   
                        }
                    }
                    break;
                case FTM_PMIC_SPKR_LEFT_MONO_SET: 
                    queried_config = pm_speaker_config__left_mono;
                    err_flag = pm_set_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                    }
                    break;
                case FTM_PMIC_SPKR_LEFT_MONO_GET:
                    queried_config = pm_speaker_config__default;
                    err_flag = pm_get_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   
                        if(memcmp( (void*)&pm_speaker_config__left_mono, (void*)&queried_config,
                                   sizeof(pm_spkr_config_mode_type)) == 0 )
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                        }
                        else
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = FALSE;   
                        }
                    }
                    break;

                case FTM_PMIC_SPKR_RIGHT_MONO_SET: 
                   queried_config = pm_speaker_config__right_mono;
                    err_flag = pm_set_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                    }
                    break;
                case FTM_PMIC_SPKR_RIGHT_MONO_GET:
                    queried_config = pm_speaker_config__default;
                    err_flag = pm_get_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   
                        if(memcmp( (void*)&pm_speaker_config__right_mono, (void*)&queried_config,
                                   sizeof(pm_spkr_config_mode_type)) == 0 )
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                        }
                        else
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = FALSE;   
                        }
                    }
                    break;
                case FTM_PMIC_SPKR_DUAL_MONO_SET: 
                    queried_config = pm_speaker_config__dual_mono;
                    err_flag = pm_set_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                    }
                    break;
                case FTM_PMIC_SPKR_DUAL_MONO_GET:
                    queried_config = pm_speaker_config__default;
                    err_flag = pm_get_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   
                        if(memcmp( (void*)&pm_speaker_config__dual_mono, (void*)&queried_config,
                                   sizeof(pm_spkr_config_mode_type)) == 0 )
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                        }
                        else
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = FALSE;   
                        }
                    }
                    break;
                case FTM_PMIC_SPKR_LEFT_8ohm_RECEIVER_SET: 
                    queried_config = pm_speaker_config__left_8ohm_recv;
                    err_flag = pm_set_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                    }
                    break;
                case FTM_PMIC_SPKR_LEFT_8ohm_RECEIVER_GET:
                    queried_config = pm_speaker_config__default;
                    err_flag = pm_get_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   
                        if(memcmp( (void*)&pm_speaker_config__left_8ohm_recv, (void*)&queried_config,
                                   sizeof(pm_spkr_config_mode_type)) == 0 )
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                        }
                        else
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = FALSE;   
                        }
                    }
                    break;
                case FTM_PMIC_SPKR_RIGHT_8ohm_RECEIVER_SET: 
                    queried_config = pm_speaker_config__right_8ohm_recv;
                    err_flag = pm_set_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                    }
                    break;
                case FTM_PMIC_SPKR_RIGHT_8ohm_RECEIVER_GET:
                    queried_config = pm_speaker_config__default;
                    err_flag = pm_get_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   
                        if(memcmp( (void*)&pm_speaker_config__right_8ohm_recv, (void*)&queried_config,
                                   sizeof(pm_spkr_config_mode_type)) == 0 )
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                        }
                        else
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = FALSE;   
                        }
                    }
                    break;
                case FTM_PMIC_SPKR_SUMMED_MONO_LEFT_SET: 
                    queried_config = pm_speaker_config__summed_mono_left;
                    err_flag = pm_set_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                    }
                    break;
                case FTM_PMIC_SPKR_SUMMED_MONO_LEFT_GET:
                    queried_config = pm_speaker_config__default;
                    err_flag = pm_get_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   
                        if(memcmp( (void*)&pm_speaker_config__summed_mono_left, (void*)&queried_config,
                                   sizeof(pm_spkr_config_mode_type)) == 0 )
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                        }
                        else
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = FALSE;   
                        }
                    }
                    break;
                case FTM_PMIC_SPKR_SUMMED_MONO_RIGHT_SET: 
                    queried_config = pm_speaker_config__summed_mono_right;
                    err_flag = pm_set_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                    }
                    break;
                case FTM_PMIC_SPKR_SUMMED_MONO_RIGHT_GET:
                    queried_config = pm_speaker_config__default;
                    err_flag = pm_get_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   
                        if(memcmp( (void*)&pm_speaker_config__summed_mono_right, (void*)&queried_config,
                                   sizeof(pm_spkr_config_mode_type)) == 0 )
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                        }
                        else
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = FALSE;   
                        }
                    }
                    break;
                case FTM_PMIC_SPKR_USB_CARKIT_SET: 
                    queried_config = pm_speaker_config__usb_carkit;
                    err_flag = pm_set_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                    }
                    break;
                case FTM_PMIC_SPKR_USB_CARKIT_GET:
                    queried_config = pm_speaker_config__default;
                    err_flag = pm_get_spkr_configuration(&queried_config);
                    if(err_flag == PM_ERR_FLAG__SUCCESS )
                    {   
                        if(memcmp( (void*)&pm_speaker_config__usb_carkit, (void*)&queried_config,
                                   sizeof(pm_spkr_config_mode_type)) == 0 )
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = TRUE;
                        }
                        else
                        {
                           spkr_HL_mux_cntl2_cmd->enable_disable = FALSE;   
                        }
                    }
                    break;
                
                default:
                    err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
                    break;
                }
                break;

        //set left-right speaker
        case FTM_PMIC_SPKR_LEFT_RIGHT_CONFIG:
            
            spkr_left_right_en_mute_cmd = ( ftm_pmic_spkr_left_right_en_mute_cmd_type *) cmd_ptr;

            switch( spkr_left_right_en_mute_cmd->spkr_cmd)
            {
                case PM_SPKR_DISABLE:
                    err_flag = pm_spkr_en(spkr_left_right_en_mute_cmd->left_right, FALSE);
                    break;
                case PM_SPKR_ENABLE:
                    err_flag = pm_spkr_en(spkr_left_right_en_mute_cmd->left_right, TRUE);
                    break;
                case PM_SPKR_MUTE_OFF:
                    err_flag = pm_spkr_en_mute(spkr_left_right_en_mute_cmd->left_right, TRUE);
                    break;
                case PM_SPKR_MUTE_ON:
                    err_flag = pm_spkr_en_mute(spkr_left_right_en_mute_cmd->left_right, FALSE);
                    break;
                case PM_SPKR_SET_GAIN_CMD:
                    spkr_left_right_gain_cmd = (ftm_pmic_spkr_left_right_gain_cmd_type *) cmd_ptr;
                    err_flag = pm_spkr_set_gain(spkr_left_right_gain_cmd->left_right, 
                                                     spkr_left_right_gain_cmd->gain);
                    break;
                case PM_SPKR_GET_GAIN_CMD:
                    spkr_left_right_gain_cmd = (ftm_pmic_spkr_left_right_gain_cmd_type *) cmd_ptr;
                    if( pm_spkr_get_gain(spkr_left_right_gain_cmd->left_right, &gain) == PM_ERR_FLAG__SUCCESS )
                    {
                        spkr_left_right_gain_cmd->gain = gain;
                    }
                    break;
                case PM_SPKR_SET_DELAY_CMD:
                    spkr_left_right_delay_cmd = (ftm_pmic_spkr_left_right_delay_cmd_type *) cmd_ptr;
                    err_flag = pm_spkr_set_delay(spkr_left_right_delay_cmd->left_right, 
                                                      spkr_left_right_delay_cmd->delay);
                    break;
                case PM_SPKR_GET_DELAY_CMD:
                    spkr_left_right_delay_cmd = (ftm_pmic_spkr_left_right_delay_cmd_type *) cmd_ptr;
                    if( pm_spkr_get_delay(spkr_left_right_delay_cmd->left_right, &delay) == PM_ERR_FLAG__SUCCESS )
                    {
                        spkr_left_right_delay_cmd->delay = delay;
                    }
                    break;     
                default:
                    err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
                    break;
            }
            break;
       default:
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            break;

    }


    if( err_flag != PM_ERR_FLAG__SUCCESS)
    {
        MSG_HIGH("FTM PMIC SPKR -- Cmd: %d, Error Flag 0x%x",spkr_header->spkr_cmd_id, err_flag ,0);
    
        rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                         err_flag, 
                                         FTM_PMIC_TEST_SPKR, 
                                         spkr_header->spkr_cmd_id );
    }
    return( rsp);

}

/*===========================================================================

FUNCTION           FTM_PMIC_PROCESS_VID_COMMANDS

DESCRIPTION        Handler for Video amplifier FTM PMIC commands.

DEPENDENCIES
None.

RETURN VALUE       Response packet

SIDE EFFECTS
None.

===========================================================================*/
ftm_rsp_pkt_type  ftm_pmic_process_vid_commands( ftm_pmic_pkt_type *cmd_ptr)
{
    ftm_pmic_vid_header_type             *vid_header;
    ftm_pmic_vid_status_cmd_type         *vid_status_cmd;
    boolean                              status;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    vid_header = (ftm_pmic_vid_header_type *) cmd_ptr;
    
    
    switch( vid_header->vid_cmd_id)
    {
    
        case FTM_PMIC_VID_ENABLE:
    
            err_flag = pm_vid_en( TRUE );
            break;
    
        case FTM_PMIC_VID_DISABLE:
    
            err_flag = pm_vid_en( FALSE );
            break;

        case FTM_PMIC_VID_GET_STATUS:
         
            vid_status_cmd = ( ftm_pmic_vid_status_cmd_type *) cmd_ptr;
            err_flag = pm_vid_is_en( &status );
            if(err_flag == PM_ERR_FLAG__SUCCESS )
            {   vid_status_cmd->enable_disable = status;
            }
            break;

        default:
            MSG_HIGH( "FTM PMIC Vid -- Invalid Command",0,0,0);
            break;
    }
    
    if( err_flag != PM_ERR_FLAG__SUCCESS)
    {
        MSG_HIGH("FTM PMIC VidAmp -- Cmd: %d, Error Flag 0x%x",vid_header->vid_cmd_id, err_flag,0);
        rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                          err_flag, 
                                          FTM_PMIC_TEST_VID, 
                                          vid_header->vid_cmd_id );
    }
    return( rsp);

}


/*===========================================================================

FUNCTION           FTM_PMIC_PROCESS_MIC_COMMANDS

DESCRIPTION        Handler for MIC FTM PMIC commands.

DEPENDENCIES
None.

RETURN VALUE       Response packet

SIDE EFFECTS
None.

===========================================================================*/

ftm_rsp_pkt_type  ftm_pmic_process_mic_commands( ftm_pmic_pkt_type *cmd_ptr)
{

    ftm_pmic_mic_header_type             *mic_header;
    ftm_pmic_mic_status_cmd_type         *mic_status_cmd;
    ftm_pmic_mic_set_level_cmd_type      *mic_set_level_cmd;
    boolean                              status;
    pm_mic_volt_type                     volt;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    
    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    
    mic_header = (ftm_pmic_mic_header_type *) cmd_ptr;

    switch( mic_header->mic_cmd_id)
    {
    
        case FTM_PMIC_MIC_ENABLE:
    
            err_flag = pm_mic_en( TRUE );
    
            break;
    
        case FTM_PMIC_MIC_DISABLE:
    
            err_flag = pm_mic_en( FALSE );
            break;

        case FTM_PMIC_MIC_GET_STATUS:
         
            mic_status_cmd = ( ftm_pmic_mic_status_cmd_type *) cmd_ptr;
            err_flag = pm_mic_is_en( &status );
            if(err_flag == PM_ERR_FLAG__SUCCESS )
            {   mic_status_cmd->enable_disable = status;
            }
            break;
    
        case FTM_PMIC_MIC_SET_LEVEL:
    
            mic_set_level_cmd = ( ftm_pmic_mic_set_level_cmd_type *) cmd_ptr;
            err_flag = pm_mic_set_volt( mic_set_level_cmd->volt_level );
            break;
        case FTM_PMIC_MIC_GET_LEVEL:
    
            mic_set_level_cmd = ( ftm_pmic_mic_set_level_cmd_type *) cmd_ptr;
            err_flag = pm_mic_get_volt(&volt);
            if(err_flag == PM_ERR_FLAG__SUCCESS )
            {   mic_set_level_cmd->volt_level = volt;
            }
            break;
        default:
            MSG_HIGH( "FTM PMIC MIC -- Invalid Command",0,0,0);
            break;
    }
    
    if( err_flag != PM_ERR_FLAG__SUCCESS)
    {
        MSG_HIGH("FTM PMIC MIC -- Cmd: %d, Error Flag 0x%x",mic_header->mic_cmd_id, err_flag,0);
        rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                            err_flag, 
                                            FTM_PMIC_TEST_MIC, 
                                            mic_header->mic_cmd_id );
    }
    return( rsp);
    
}

/*===========================================================================

FUNCTION           FTM_PMIC_PROCESS_MPP_COMMANDS

DESCRIPTION        Handler for MPP FTM PMIC commands.

DEPENDENCIES
None.

RETURN VALUE       Response packet

SIDE EFFECTS
None.

===========================================================================*/

ftm_rsp_pkt_type  ftm_pmic_process_mpp_commands( ftm_pmic_pkt_type *cmd_ptr )
{

    ftm_pmic_mpp_header_type                *mpp_header;
    ftm_pmic_mpp_digital_in_cmd_type        *mpp_digital_in_cmd;
    ftm_pmic_mpp_digital_out_cmd_type       *mpp_digital_out_cmd;                                 
    ftm_pmic_mpp_digital_inout_cmd_type     *mpp_digital_inout_cmd;                               
    ftm_pmic_mpp_analog_in_cmd_type         *mpp_analog_in_cmd;                                   
    ftm_pmic_mpp_analog_out_cmd_type        *mpp_analog_out_cmd;                                  
    ftm_pmic_mpp_i_sink_cmd_type            *mpp_i_sink_cmd;                                      

    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;

    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};                                  

    mpp_header = (ftm_pmic_mpp_header_type *)cmd_ptr;

                                                                                                   
     switch(mpp_header->mpp_cmd_id)                                                                
     {                                                                                             
                                                                                                   
     case FTM_PMIC_MPP_CONFIG_DIGITAL_IN:                                                          
                                                                                                   
         mpp_digital_in_cmd = (ftm_pmic_mpp_digital_in_cmd_type *)cmd_ptr;                         
                                                                                                   
         err_flag = pm_mpp_config_digital_input(mpp_digital_in_cmd->mpp,                           
                                                 mpp_digital_in_cmd->level,
                                                  mpp_digital_in_cmd->dbus);                        
                                                                                                   
             break;                                                                                
                                                                                                   
     case FTM_PMIC_MPP_CONFIG_DIGITAL_OUT:                                                         
                                                                                                   
         mpp_digital_out_cmd = (ftm_pmic_mpp_digital_out_cmd_type *)cmd_ptr;                       
                                                                                                   
         err_flag = pm_mpp_config_digital_output(mpp_digital_out_cmd->mpp,                         
                                                 mpp_digital_out_cmd->level,                       
                                                 mpp_digital_out_cmd->ctrl);                       
                                                                                                   
             break;                                                                                
                                                                                                   
     case FTM_PMIC_MPP_CONFIG_DIGITAL_INOUT:                                                       
                                                                                                   
         mpp_digital_inout_cmd = (ftm_pmic_mpp_digital_inout_cmd_type *)cmd_ptr;                   
                                                                                                   
         err_flag = pm_mpp_config_digital_inout(mpp_digital_inout_cmd->mpp,                        
                                                mpp_digital_inout_cmd->level,                      
                                                mpp_digital_inout_cmd->pup);                       
                                                                                                   
             break;                                                                                
                                                                                                   
     case FTM_PMIC_MPP_CONFIG_ANALOG_IN:                                                           
                                                                                                   
         mpp_analog_in_cmd = (ftm_pmic_mpp_analog_in_cmd_type *)cmd_ptr;                           
                                                                                                   
         err_flag = pm_mpp_config_analog_input(mpp_analog_in_cmd->mpp,                             
                                               mpp_analog_in_cmd->channel);                        
                                                                                                   
             break;                                                                                
                                                                                                   
     case FTM_PMIC_MPP_CONFIG_ANALOG_OUT:                                                          
                                                                                                   
         mpp_analog_out_cmd = (ftm_pmic_mpp_analog_out_cmd_type *)cmd_ptr;                         
                                                                                                   
         err_flag = pm_mpp_config_analog_output(mpp_analog_out_cmd->mpp,                           
                                                mpp_analog_out_cmd->level,
                                                mpp_analog_out_cmd->_switch_);                     
                                                                                                   
             break;                                                                                
                                                                                                   
     case FTM_PMIC_MPP_CONFIG_I_SINK:                                                              
                                                                                                   
         mpp_i_sink_cmd = (ftm_pmic_mpp_i_sink_cmd_type *)cmd_ptr;                                 
                                                                                                   
         err_flag = pm_mpp_config_i_sink(mpp_i_sink_cmd->mpp,                                      
                                         mpp_i_sink_cmd->level,                                    
                                         mpp_i_sink_cmd->_switch_);                                
                                                                                                   
             break;                                                                                
                                                                                                   
     default:                                                                                      
                                                                                                   
                                                                                                   
             MSG_HIGH( "FTM PMIC MPP -- Invalid Command",0,0,0);                                   
                                                                                                   
             break;                                                                                
     }                                                                                             
                                                                                                   
                                                                                                   
     if( err_flag != PM_ERR_FLAG__SUCCESS)                                                         
     {                                                                                             
         MSG_HIGH("FTM PMIC MPP -- Cmd: %d, Error Flag 0x%x",mpp_header->mpp_cmd_id, err_flag,0);  
     }                                                                                             
                                                                                                   
                                                                                                   
     rsp = ftm_pmic_create_gen_response( cmd_ptr,                                                  
                                         err_flag,                                                 
                                         FTM_PMIC_TEST_MPP,                                        
                                         mpp_header->mpp_cmd_id );                                 
     return( rsp);                                                                                 
    
}

/*===========================================================================

FUNCTION           FTM_PMIC_PROCESS_RESET

DESCRIPTION        Handler for FTM PMIC RESET commands.

DEPENDENCIES
None.

RETURN VALUE       Response packet

SIDE EFFECTS
None.

===========================================================================*/
ftm_rsp_pkt_type  ftm_pmic_process_reset( ftm_pmic_pkt_type *cmd_ptr)
{

    ftm_pmic_reset_header_type             *reset_header;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    
    reset_header = (ftm_pmic_reset_header_type *) cmd_ptr;
    
    switch( reset_header->reset_cmd_id)
    {
    
        case FTM_PMIC_HARD_RESET:

            err_flag = pm_hard_reset();
            break;

        default:
            MSG_HIGH( "FTM PMIC RESET -- Invalid Command",0,0,0);
            break;
    
    }
    
    if( err_flag != PM_ERR_FLAG__SUCCESS)
    {
        MSG_HIGH("FTM PMIC RESET -- Cmd: %d, Error Flag 0x%x",reset_header->reset_cmd_id, err_flag,0);
        rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                        err_flag, 
                                        FTM_PMIC_TEST_RESET, 
                                        reset_header->reset_cmd_id );
    }
    return( rsp);

}

#if defined( FEATURE_PMIC_RTC)
/*===========================================================================

FUNCTION            FTM_PMIC_PROCESS_RTC_COMMANDS

DESCRIPTION         Handler for all RTC related FTM PMIC commands.
                   
   
DEPENDENCIES
   None.

RETURN VALUE        Response packet
              

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type  ftm_pmic_process_rtc_commands( ftm_pmic_pkt_type *cmd_ptr)
{
    
    pm_rtc_julian_type                          time;     
    pm_err_flag_type                            err_flag = PM_ERR_FLAG__SUCCESS;
    pm_rtc_alarm_type                           alarm;
    uint8                                       alarm_status;
    uint8                                       time_adjust;
    ftm_pmic_rtc_set_get_time_cmd_type          *rtc_set_get_time_cmd;
    ftm_pmic_rtc_set_get_alarm_time_cmd_type    *rtc_set_get_alarm_time_cmd;

    ftm_pmic_rtc_disable_alarm_cmd_type         *rtc_disable_alarm_cmd;
    ftm_pmic_rtc_set_get_time_adjust_cmd_type   *rtc_set_get_time_adj_cmd;
    ftm_pmic_rtc_get_alarm_status_cmd_type      *rtc_get_alarm_status_cmd;
    ftm_pmic_rtc_header_type                    *rtc_header;
    ftm_rsp_pkt_type                            rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    pm_rtc_display_type                         display_mode = PM_RTC_12HR_MODE;

    rtc_header = ( ftm_pmic_rtc_header_type *) cmd_ptr;


    switch( rtc_header->rtc_cmd_id )
    {

        case FTM_PMIC_RTC_SET_GET_TIME:

           

            rtc_set_get_time_cmd = (ftm_pmic_rtc_set_get_time_cmd_type *) cmd_ptr;

            

            if( rtc_set_get_time_cmd->ftm_rtc_rw == PM_RTC_SET_CMD)
            {

                if( rtc_set_get_time_cmd->time_mode == FTM_PMIC_RTC_MODE_12_HOUR_AM ||
                    rtc_set_get_time_cmd->time_mode == FTM_PMIC_RTC_MODE_12_HOUR_PM)
                {
                    
                    display_mode = PM_RTC_12HR_MODE;


                }
                else if( rtc_set_get_time_cmd->time_mode == FTM_PMIC_RTC_MODE_24_HOUR)
                {
                    display_mode = PM_RTC_24HR_MODE;
                }
                else
                {
                    err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
                }

                err_flag |= pm_set_rtc_display_mode( display_mode);


                time.day_of_week = 5; /* TGIF!...day of week not really necessary */
                time.month       = (word) rtc_set_get_time_cmd->rtc_time.ftm_rtc_month;
                time.day         = (word) rtc_set_get_time_cmd->rtc_time.ftm_rtc_day;
                time.year        = (word) rtc_set_get_time_cmd->rtc_time.ftm_rtc_year;
                time.hour        = (word) rtc_set_get_time_cmd->rtc_time.ftm_rtc_hour;
                time.minute      = (word) rtc_set_get_time_cmd->rtc_time.ftm_rtc_min;
                time.second      = (word) rtc_set_get_time_cmd->rtc_time.ftm_rtc_sec;
            
                /* Convert 12 PM mode times into internal format used by API */

                if( rtc_set_get_time_cmd->time_mode == FTM_PMIC_RTC_MODE_12_HOUR_PM)
                {
                    
                    time.hour += 20;
                }

                err_flag |= pm_rtc_rw_cmd( PM_RTC_SET_CMD, &time);


                rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                    err_flag, 
                                                    FTM_PMIC_TEST_RTC, 
                                                    rtc_header->rtc_cmd_id );

            }
            else if( rtc_set_get_time_cmd->ftm_rtc_rw == PM_RTC_GET_CMD)
            {

                err_flag |= pm_set_rtc_display_mode( PM_RTC_24HR_MODE);
                err_flag |= pm_rtc_rw_cmd( PM_RTC_GET_CMD, &time);

                rtc_set_get_time_cmd->time_mode = FTM_PMIC_RTC_MODE_24_HOUR;
                    
                rtc_set_get_time_cmd->rtc_time.ftm_rtc_month = (byte) time.month;  
                rtc_set_get_time_cmd->rtc_time.ftm_rtc_day   = (byte) time.day;    
                rtc_set_get_time_cmd->rtc_time.ftm_rtc_year  = (word) time.year;   
                rtc_set_get_time_cmd->rtc_time.ftm_rtc_hour  = (byte) time.hour;   
                rtc_set_get_time_cmd->rtc_time.ftm_rtc_min   = (byte) time.minute;    
                rtc_set_get_time_cmd->rtc_time.ftm_rtc_sec   = (byte) time.second;    


                if( err_flag != PM_ERR_FLAG__SUCCESS)
                {
                    rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                        err_flag, 
                                                        FTM_PMIC_TEST_RTC, 
                                                        rtc_header->rtc_cmd_id );
                }
            }
            else
            {

                err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
                rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                    err_flag, 
                                                    FTM_PMIC_TEST_RTC, 
                                                    rtc_header->rtc_cmd_id );
            }
            


            break;

        case FTM_PMIC_RTC_STOP:

            err_flag = pm_hal_rtc_stop();
            rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                err_flag, 
                                                FTM_PMIC_TEST_RTC, 
                                                rtc_header->rtc_cmd_id );


            break;


        case FTM_PMIC_RTC_SET_GET_ALARM_TIME:

            
            rtc_set_get_alarm_time_cmd = (ftm_pmic_rtc_set_get_alarm_time_cmd_type *) cmd_ptr;


            alarm = rtc_set_get_alarm_time_cmd->alarm_id;

            if( rtc_set_get_alarm_time_cmd->ftm_rtc_rw == PM_RTC_SET_CMD)
            {
                if( rtc_set_get_alarm_time_cmd->time_mode == FTM_PMIC_RTC_MODE_12_HOUR_AM ||
                    rtc_set_get_alarm_time_cmd->time_mode == FTM_PMIC_RTC_MODE_12_HOUR_PM)
                {
                    
                    display_mode = PM_RTC_12HR_MODE;


                }
                else if( rtc_set_get_alarm_time_cmd->time_mode == FTM_PMIC_RTC_MODE_24_HOUR)
                {
                    display_mode = PM_RTC_24HR_MODE;
                }
                else
                {
                    err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
                }

                err_flag |= pm_set_rtc_display_mode( display_mode);


                time.day_of_week = 5; /* TGIF!...day of week not really necessary */
                time.month       = (word) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_month;
                time.day         = (word) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_day;
                time.year        = (word) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_year;
                time.hour        = (word) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_hour;
                time.minute      = (word) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_min;
                time.second      = (word) rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_sec;
            
                if( rtc_set_get_alarm_time_cmd->time_mode == FTM_PMIC_RTC_MODE_12_HOUR_PM)
                {

                    time.hour += 20;
                }

                err_flag  = pm_clear_irq( PM_RTC_ALRM_IRQ_HDL);
                err_flag |= pm_set_irq_handle( PM_RTC_ALRM_IRQ_HDL, ftm_pmic_rtc_alarm_isr_cb);
                err_flag |= pm_rtc_alarm_rw_cmd( PM_RTC_SET_CMD, alarm, &time);

                rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                    err_flag, 
                                                    FTM_PMIC_TEST_RTC, 
                                                    rtc_header->rtc_cmd_id );

            }
            else if( rtc_set_get_alarm_time_cmd->ftm_rtc_rw == PM_RTC_GET_CMD)
            {

                err_flag |= pm_set_rtc_display_mode( PM_RTC_24HR_MODE);
                err_flag |= pm_rtc_alarm_rw_cmd( PM_RTC_GET_CMD, alarm, &time);

                rtc_set_get_alarm_time_cmd->time_mode = FTM_PMIC_RTC_MODE_24_HOUR;

                rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_month = (byte) time.month;  
                rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_day   = (byte) time.day;    
                rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_year  = (word) time.year;   
                rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_hour  = (byte) time.hour;   
                rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_min   = (byte) time.minute;    
                rtc_set_get_alarm_time_cmd->alarm_time.ftm_rtc_sec   = (byte) time.second;    


                if( err_flag != PM_ERR_FLAG__SUCCESS)
                {
                     rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                         err_flag, 
                                                         FTM_PMIC_TEST_RTC, 
                                                         rtc_header->rtc_cmd_id );
                }
            }
            else
            {

                err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
                rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                    err_flag, 
                                                    FTM_PMIC_TEST_RTC, 
                                                    rtc_header->rtc_cmd_id );

            }


            break;


        case FTM_PMIC_RTC_GET_ALARM_STATUS:

            rtc_get_alarm_status_cmd = (ftm_pmic_rtc_get_alarm_status_cmd_type *) cmd_ptr;


            err_flag =  pm_get_rtc_alarm_status( &alarm_status);
            rtc_get_alarm_status_cmd->alarm_status = alarm_status;

            if( err_flag != PM_ERR_FLAG__SUCCESS)
            {
                rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                    err_flag, 
                                                    FTM_PMIC_TEST_RTC, 
                                                    rtc_header->rtc_cmd_id );
            }

            break;

        case FTM_PMIC_RTC_DISABLE_ALARM:

            rtc_disable_alarm_cmd = ( ftm_pmic_rtc_disable_alarm_cmd_type *) cmd_ptr;

            alarm = rtc_disable_alarm_cmd->alarm_id;
            
            err_flag  = pm_reset_rtc_alarm( alarm);
            err_flag |= pm_clear_irq( PM_RTC_ALRM_IRQ_HDL);
            err_flag |= pm_set_irq_handle( PM_RTC_ALRM_IRQ_HDL, NULL);


            rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                err_flag, 
                                                FTM_PMIC_TEST_RTC, 
                                                rtc_header->rtc_cmd_id );

            break;

        case FTM_PMIC_RTC_SET_GET_TIME_ADJUST:

            rtc_set_get_time_adj_cmd = ( ftm_pmic_rtc_set_get_time_adjust_cmd_type *) cmd_ptr;

            if( rtc_set_get_time_adj_cmd->ftm_rtc_rw == PM_RTC_SET_CMD)
            {
                time_adjust = (uint8) rtc_set_get_time_adj_cmd->time_adjust;
                err_flag    = pm_hal_rtc_set_time_adjust( time_adjust);

                rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                    err_flag, 
                                                    FTM_PMIC_TEST_RTC, 
                                                    rtc_header->rtc_cmd_id );


            }
            else if( rtc_set_get_time_adj_cmd->ftm_rtc_rw == PM_RTC_GET_CMD)
            {

                err_flag = pm_hal_rtc_get_time_adjust( &time_adjust);
                rtc_set_get_time_adj_cmd->time_adjust = time_adjust;

                if( err_flag != PM_ERR_FLAG__SUCCESS)
                {
                    rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                        err_flag, 
                                                        FTM_PMIC_TEST_RTC, 
                                                        rtc_header->rtc_cmd_id );
                }
            }
            else
            {

                err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
                rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                    err_flag, 
                                                    FTM_PMIC_TEST_RTC, 
                                                    rtc_header->rtc_cmd_id );
            }



            break;

        

        default:

            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                err_flag, 
                                                FTM_PMIC_TEST_RTC, 
                                                rtc_header->rtc_cmd_id );

            MSG_HIGH( "FTM PMIC RTC -- Invalid Command",0,0,0);
            break;

    }

    if( err_flag != PM_ERR_FLAG__SUCCESS)
    {
               
        MSG_HIGH("FTM PMIC RTC -- Cmd: %d, Error Flag 0x%x",rtc_header->rtc_cmd_id, err_flag,0);
    }

    return( rsp);

}

#endif /* FEATURE_PMIC_RTC */


/*

Only one VREG can be operated at one time.
*/
ftm_rsp_pkt_type  
ftm_pmic_process_vreg_commands_ext ( ftm_pmic_pkt_type *cmd_ptr )
{
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS ;

    ftm_pmic_vreg_header_type                  *vreg_header = NULL;
    ftm_pmic_vreg_control_ext_cmd_type         *vreg_control_cmd = NULL;
    ftm_pmic_vreg_set_level_cmd_type           *vreg_set_level_cmd = NULL;
    ftm_pmic_vreg_get_level_cmd_type           *vreg_get_level_cmd = NULL;
    ftm_pmic_vreg_config_smps_mode_cmd_type    *vreg_config_smps_mode_cmd = NULL;
    ftm_pmic_vreg_config_ldo_pd_sw_cmd_type    *vreg_config_ldo_pd_sw_cmd = NULL;
    ftm_pmic_vreg_lp_mode_control_cmd_type     *vreg_lp_mode_control_cmd = NULL;
    ftm_pmic_vreg_smps_config_clk_cmd_type     *vreg_smps_clk_config_cmd = NULL ;


    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE} ;

    vreg_header = (ftm_pmic_vreg_header_type *) cmd_ptr ;


    switch( vreg_header->vreg_cmd_id)
    {
        
        case FTM_PMIC_VREG_CONTROL:
        {
            pm_vreg_masked_type vreg_mask = 1 ; 
            vreg_control_cmd = ( ftm_pmic_vreg_control_ext_cmd_type *) cmd_ptr ;
            vreg_mask = ( 1ULL << vreg_control_cmd->vregs ) ; 
            err_flag = pm_vreg_control ( vreg_control_cmd->vreg_cmd , 
                                         vreg_mask ) ;
            break;
        }


        case FTM_PMIC_VREG_SET_LEVEL:
            vreg_set_level_cmd = 
                ( ftm_pmic_vreg_set_level_cmd_type *) cmd_ptr ;
            err_flag = pm_vreg_set_level ( vreg_set_level_cmd->vreg,
                                           vreg_set_level_cmd->level ) ;
            break;

         case FTM_PMIC_VREG_GET_LEVEL:
            vreg_get_level_cmd = 
                ( ftm_pmic_vreg_get_level_cmd_type *) cmd_ptr ;
            err_flag = 
                pm_vreg_get_level ( vreg_get_level_cmd->vreg,
                                    (uint16*) &(vreg_get_level_cmd->level) ) ; 
            break;

        case FTM_PMIC_VREG_CONFIG_SMPS_MODE:
            vreg_config_smps_mode_cmd = 
                ( ftm_pmic_vreg_config_smps_mode_cmd_type *) cmd_ptr ;
            err_flag = 
                pm_vreg_smps_config ( vreg_config_smps_mode_cmd->vreg_smps , 
                                      vreg_config_smps_mode_cmd->mode ) ;
            break;

        case FTM_PMIC_VREG_CONFIG_LDO_PD_SW:
            vreg_config_ldo_pd_sw_cmd = 
                ( ftm_pmic_vreg_config_ldo_pd_sw_cmd_type * ) cmd_ptr ;
            err_flag = pm_vreg_pull_down_switch ( vreg_config_ldo_pd_sw_cmd->vreg_cmd , 
                                                 vreg_config_ldo_pd_sw_cmd->vreg ) ;
            break;


        case FTM_PMIC_VREG_LP_MODE_CONTROL:
             vreg_lp_mode_control_cmd = 
                 ( ftm_pmic_vreg_lp_mode_control_cmd_type *) cmd_ptr ;
            err_flag = pm_lp_mode_control ( vreg_lp_mode_control_cmd->vreg_cmd , 
                                          vreg_lp_mode_control_cmd->vreg ) ;
            break;

        case FTM_PMIC_VREG_SMPS_CLK_CONFIG:
            vreg_smps_clk_config_cmd = 
                ( ftm_pmic_vreg_smps_config_clk_cmd_type *) cmd_ptr ;

            switch( vreg_smps_clk_config_cmd->vreg_smps_clk_cmd)
            {

                case FTM_PM_VREG_SMPS_CLK_SEL_CMD:

                    err_flag = 
                        pm_vreg_smps_clock_sel 
                        ( vreg_smps_clk_config_cmd->
                          smps_clk_config_union.vreg_smps_clk_sel ) ;

                    break;


                case FTM_PM_VREG_SMPS_CLK_TCXO_DIV_CMD:
                    err_flag = 
                        pm_vreg_smps_tcxo_div_sel
                        ( vreg_smps_clk_config_cmd->
                          smps_clk_config_union.vreg_smps_tcxo_clk_div ) ;

                    break;


                default:
                    err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
                    break;
            }
            break ; /* FTM_PMIC_VREG_SMPS_CLK_CONFIG */


        default:

            MSG_HIGH( "FTM PMIC VREG EXT -- Invalid Command",0,0,0);
            break;

    }

    if( err_flag != PM_ERR_FLAG__SUCCESS)
    {
        MSG_HIGH( "FTM PMIC VREG EXT -- Cmd: %d, Error Flag 0x%x" , 
                  vreg_header->vreg_cmd_id , err_flag,0 ) ;
    }

    if(vreg_header->vreg_cmd_id == FTM_PMIC_VREG_GET_LEVEL)
    {
        rsp = ftm_pmic_create_get_value_response ( cmd_ptr,
                                                   vreg_get_level_cmd->level ,
                                                   err_flag, 
                                                   FTM_PMIC_TEST_VREG_EXT , 
                                                   vreg_header->vreg_cmd_id ) ;
    }
    else
    {
        rsp = ftm_pmic_create_gen_response ( cmd_ptr,
                                             err_flag, 
                                             FTM_PMIC_TEST_VREG_EXT , 
                                             vreg_header->vreg_cmd_id ) ;
    }
    
    return( rsp);     
}





/*===========================================================================

FUNCTION           FTM_PMIC_PROCESS_VREG_COMMANDS

DESCRIPTION        Handler for all VREG related FTM PMIC commands.
.  
   
DEPENDENCIES
   None.

RETURN VALUE       Response packet
              

SIDE EFFECTS
   None.
 
===========================================================================*/
ftm_rsp_pkt_type  ftm_pmic_process_vreg_commands( ftm_pmic_pkt_type *cmd_ptr)
{
    pm_err_flag_type   err_flag = PM_ERR_FLAG__SUCCESS;

    ftm_pmic_vreg_header_type                  *vreg_header = NULL;
    ftm_pmic_vreg_control_cmd_type             *vreg_control_cmd = NULL;
    ftm_pmic_vreg_set_level_cmd_type           *vreg_set_level_cmd = NULL;
    ftm_pmic_vreg_get_level_cmd_type           *vreg_get_level_cmd = NULL;
    ftm_pmic_vreg_config_smps_mode_cmd_type    *vreg_config_smps_mode_cmd = NULL;
    ftm_pmic_vreg_config_ldo_pd_sw_cmd_type    *vreg_config_ldo_pd_sw_cmd = NULL;
    ftm_pmic_vreg_lp_mode_control_cmd_type     *vreg_lp_mode_control_cmd = NULL;
    ftm_pmic_vreg_smps_config_clk_cmd_type     *vreg_smps_clk_config_cmd = NULL;


    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

    vreg_header = (ftm_pmic_vreg_header_type *) cmd_ptr;


    switch( vreg_header->vreg_cmd_id)
    {
        
        case FTM_PMIC_VREG_CONTROL:

            vreg_control_cmd = ( ftm_pmic_vreg_control_cmd_type *) cmd_ptr;

            err_flag = pm_vreg_control( vreg_control_cmd->vreg_cmd, 
                                        vreg_control_cmd->vregs);
            


            break;


        case FTM_PMIC_VREG_SET_LEVEL:

            vreg_set_level_cmd = ( ftm_pmic_vreg_set_level_cmd_type *) cmd_ptr;
            
            err_flag = pm_vreg_set_level( vreg_set_level_cmd->vreg,
                                          vreg_set_level_cmd->level );

            break;

         case FTM_PMIC_VREG_GET_LEVEL:
   
            vreg_get_level_cmd = ( ftm_pmic_vreg_get_level_cmd_type *) cmd_ptr;
            
            err_flag = pm_vreg_get_level( vreg_get_level_cmd->vreg,
                                          (uint16*) &(vreg_get_level_cmd->level) );
            break;

        case FTM_PMIC_VREG_CONFIG_SMPS_MODE:

            vreg_config_smps_mode_cmd = ( ftm_pmic_vreg_config_smps_mode_cmd_type *) cmd_ptr;

            err_flag = pm_vreg_smps_config( vreg_config_smps_mode_cmd->vreg_smps, 
                                            vreg_config_smps_mode_cmd->mode);
            break;

        case FTM_PMIC_VREG_CONFIG_LDO_PD_SW:

            vreg_config_ldo_pd_sw_cmd = ( ftm_pmic_vreg_config_ldo_pd_sw_cmd_type *) cmd_ptr;

            err_flag = pm_vreg_pull_down_switch(vreg_config_ldo_pd_sw_cmd->vreg_cmd, 
                                                vreg_config_ldo_pd_sw_cmd->vreg);
            break;


        case FTM_PMIC_VREG_LP_MODE_CONTROL:


            vreg_lp_mode_control_cmd = ( ftm_pmic_vreg_lp_mode_control_cmd_type *) cmd_ptr;


            err_flag = pm_lp_mode_control(vreg_lp_mode_control_cmd->vreg_cmd, 
                                          vreg_lp_mode_control_cmd->vreg);

            break;



        case FTM_PMIC_VREG_SMPS_CLK_CONFIG:

            vreg_smps_clk_config_cmd = ( ftm_pmic_vreg_smps_config_clk_cmd_type *) cmd_ptr;


            switch( vreg_smps_clk_config_cmd->vreg_smps_clk_cmd)
            {

                case FTM_PM_VREG_SMPS_CLK_SEL_CMD:

                    err_flag = pm_vreg_smps_clock_sel( vreg_smps_clk_config_cmd->smps_clk_config_union.vreg_smps_clk_sel);

                    break;


                case FTM_PM_VREG_SMPS_CLK_TCXO_DIV_CMD:

                    err_flag = 
                               pm_vreg_smps_tcxo_div_sel( vreg_smps_clk_config_cmd->smps_clk_config_union.vreg_smps_tcxo_clk_div);

                    break;


                default:

                    err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
                    break;
            }
            break ; /* FTM_PMIC_VREG_SMPS_CLK_CONFIG */


        default:

            MSG_HIGH( "FTM PMIC Vreg -- Invalid Command",0,0,0);
            break;



    }

    if( err_flag != PM_ERR_FLAG__SUCCESS)
    {
        MSG_HIGH("FTM PMIC VREG -- Cmd: %d, Error Flag 0x%x",vreg_header->vreg_cmd_id, err_flag,0);
    }

    if(vreg_header->vreg_cmd_id == FTM_PMIC_VREG_GET_LEVEL)
    {
        rsp = ftm_pmic_create_get_value_response( cmd_ptr,
                                                   vreg_get_level_cmd->level,
                                                   err_flag, 
                                                   FTM_PMIC_TEST_VREG, 
                                                   vreg_header->vreg_cmd_id );
    }
    else
    {
        rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                             err_flag, 
                                             FTM_PMIC_TEST_VREG, 
                                             vreg_header->vreg_cmd_id );
    }
    
    return( rsp);
}


   
/*===========================================================================
FUNCTION           FTM_PMIC_PROCESS_GEN_COMMANDS
DESCRIPTION        Handler for all general FTM PMIC commands.
DEPENDENCIES       None.
RETURN VALUE       Response packet
SIDE EFFECTS       None.
===========================================================================*/
ftm_rsp_pkt_type  ftm_pmic_process_gen_commands( ftm_pmic_pkt_type *cmd_ptr)
{
    pm_err_flag_type  err_flag = PM_ERR_FLAG__SUCCESS;
    pm_model_type pmic_model = PMIC_IS_INVALID;
    ftm_pmic_gen_header_type  *gen_header;

    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

    gen_header = (ftm_pmic_gen_header_type *) cmd_ptr;

    switch( gen_header->gen_cmd_id)
    {
        case FTM_PMIC_GET_MODEL:
            pmic_model = (pm_model_type)pm_get_pmic_model();
            break;
        default:
            pmic_model = PMIC_IS_INVALID;
            break;
         //insert other general PMIC FTM calls here ...
    }

    if(pmic_model == PMIC_IS_INVALID)
    {
        err_flag = PM_ERR_FLAG__INVALID_PMIC_MODEL;
    }
    else
    {
        rsp = ftm_pmic_create_get_value_response( cmd_ptr,
                                                  pmic_model,
                                                  err_flag, 
                                                  FTM_PMIC_TEST_GEN,
                                                  gen_header->gen_cmd_id );
    }

    return rsp;
}


/*===========================================================================

FUNCTION          FTM_PMIC_PROCESS_CHG_COMMANDS

DESCRIPTION       Handler for all Charger related FTM PMIC commands.
.  
   
DEPENDENCIES      
   None.

RETURN VALUE      Response packet.
   
   
SIDE EFFECTS
   None.
 
===========================================================================*/
ftm_rsp_pkt_type  ftm_pmic_process_chg_commands( ftm_pmic_pkt_type *cmd_ptr)
{


   pm_err_flag_type                        err_flag = PM_ERR_FLAG__SUCCESS;
   ftm_pmic_chg_switch_func_type           sw_func;
   ftm_pmic_chg_switch_type                sw_type;
   ftm_pmic_chg_header_type                *chg_header;
   ftm_pmic_chg_set_switch_cmd_type        *chg_set_switch_cmd;

   ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

   
   chg_header = (ftm_pmic_chg_header_type *) cmd_ptr;


   switch( chg_header->chg_cmd_id)
   {
        
       case FTM_PMIC_CHG_SET_SWITCH_STATE:

           chg_set_switch_cmd = (ftm_pmic_chg_set_switch_cmd_type *) cmd_ptr;

           sw_type = chg_set_switch_cmd->chg_switch_type;

           if( sw_type <  FTM_PMIC_CHG_SWITCH_NUM_SWITCHES )
           {
               sw_func = switch_func_table[ sw_type];
               err_flag = sw_func( chg_set_switch_cmd->chg_switch_cmd );
           }

           rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                               err_flag, 
                                               FTM_PMIC_TEST_CHG, 
                                               chg_header->chg_cmd_id );
       

           break;

       case FTM_PMIC_CHG_SET_CONFIG:

           rsp = ftm_pmic_process_chg_config_commands( cmd_ptr, &err_flag);

           break;


       default:

           err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;


           rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                               err_flag, 
                                               FTM_PMIC_TEST_CHG, 
                                               chg_header->chg_cmd_id );

           MSG_HIGH( "FTM PMIC CHG -- Invalid Command",0,0,0);
           break;

   }



    if( err_flag != PM_ERR_FLAG__SUCCESS)
    {
        MSG_HIGH("FTM PMIC VREG -- Cmd: %d, Error Flag 0x%x",chg_header->chg_cmd_id, err_flag,0);
    }




    return( rsp);
}



/*===========================================================================

FUNCTION         FTM_PMIC_PROCESS_CHG_CONFIG_COMMANDS

DESCRIPTION      Handler for Charger configuration related FTM PMIC commands.
.  
   
DEPENDENCIES
   None.

RETURN VALUE     Response packet.
  

SIDE EFFECTS
   None.
 
===========================================================================*/
ftm_rsp_pkt_type  ftm_pmic_process_chg_config_commands( ftm_pmic_pkt_type *cmd_ptr, pm_err_flag_type *err )
{


    ftm_pmic_chg_set_config_header_type       *chg_header;
       
    pm_err_flag_type                          err_flag = PM_ERR_FLAG__SUCCESS;

    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    
    pm_chg_pulse_chg_config_type               pulse_chg_config;
    ftm_pmic_chg_pulse_chg_config_type         *ftm_pulse_chg_config;

  
    ftm_pmic_chg_wchg_vbatt_det_config_type       *ftm_wchg_vbatt_det_config;
    ftm_pmic_chg_wchg_transistor_lim_config_type  *ftm_wchg_transistor_lim_config;
    ftm_pmic_chg_tr_imax_type                     *ftm_tr_imax_cmd;
    ftm_pmic_chg_coin_cell_config_type            *ftm_coin_cell_config;
    ftm_pmic_chg_batt_alarm_config_type           *ftm_batt_alarm_config;
    ftm_pmic_chg_uchg_q_imax_config_type          *ftm_uchg_q_imax_config;
    ftm_pmic_chg_pwr_ctrl_limit_config_type       *ftm_pwr_ctrl_limit_config;


    chg_header = (ftm_pmic_chg_set_config_header_type *) cmd_ptr;

    switch( chg_header->chg_config_type)
    {

        case FTM_PMIC_CHG_CONFIG_PULSE_CHGR:
                        
            ftm_pulse_chg_config = ( ftm_pmic_chg_pulse_chg_config_type *) cmd_ptr;

            pulse_chg_config.p_type = ftm_pulse_chg_config->p_type;
            pulse_chg_config.t_done = ftm_pulse_chg_config->t_done;
            pulse_chg_config.t_off  = ftm_pulse_chg_config->t_off;
            pulse_chg_config.t_on   = ftm_pulse_chg_config->t_on;

            err_flag = pm_chg_pulse_charger_config( &pulse_chg_config);

            break;

        case FTM_PMIC_CHG_CONFIG_PULSE_CHGR_VBAT_DET:
               
            ftm_wchg_vbatt_det_config = ( ftm_pmic_chg_wchg_vbatt_det_config_type *) cmd_ptr;

            err_flag = pm_chg_pulse_charger_vbatdet_config( (uint16) ftm_wchg_vbatt_det_config->vbatt_det_vlt );

            
            break;

        case FTM_PMIC_CHG_CONFIG_WALL_CHGR_TRANISTOR_LIMITS:
             
            ftm_wchg_transistor_lim_config = ( ftm_pmic_chg_wchg_transistor_lim_config_type *) cmd_ptr;
            
            err_flag =  pm_chg_wall_charger_transistor_imaxsel( ftm_wchg_transistor_lim_config->wchg_q_imax);
            err_flag |= pm_chg_charger_transistor_vmaxsel( ftm_wchg_transistor_lim_config->wchg_q_vmax);

                                                               
            break;

        case FTM_PMIC_CHG_CONFIG_TRICKLE_CHARGER:

            ftm_tr_imax_cmd = ( ftm_pmic_chg_tr_imax_type *) cmd_ptr;

            err_flag = pm_chg_trickle_config( ftm_tr_imax_cmd->tr_imax );

            break;

        case FTM_PMIC_CHG_CONFIG_COIN_CELL_CHGR:

            ftm_coin_cell_config = ( ftm_pmic_chg_coin_cell_config_type *) cmd_ptr;
                         
            err_flag = pm_coin_cell_chg_config( ftm_coin_cell_config->coin_cell_vset,
                                                ftm_coin_cell_config->coin_cell_rset );

            break;

        case FTM_PMIC_CHG_CONFIG_BATT_ALARM:
                             
            ftm_batt_alarm_config = ( ftm_pmic_chg_batt_alarm_config_type *) cmd_ptr;

            err_flag = pm_chg_bat_alarm_config( (uint16) ftm_batt_alarm_config->upper_lim,
                                                (uint16) ftm_batt_alarm_config->lower_lim,
                                                 ftm_batt_alarm_config->batt_alarm_hyst );

            break;


        case FTM_PMIC_CHG_CONFIG_USB_TRANSISTOR_LIMITS:

            ftm_uchg_q_imax_config = (ftm_pmic_chg_uchg_q_imax_config_type *) cmd_ptr;

            err_flag = pm_chg_usb_charger_transistor_imaxsel( ftm_uchg_q_imax_config->usb_trans_lim_config );


            break;


        case FTM_PMIC_CHG_CONFIG_PWR_CTRL_LIMIT:

            ftm_pwr_ctrl_limit_config = ( ftm_pmic_chg_pwr_ctrl_limit_config_type *) cmd_ptr;

            switch( ftm_pwr_ctrl_limit_config->select)
            {

                case FTM_PMIC_CHG_PWR_CTRL_LIMIT_WALL:

                    err_flag = pm_chg_wall_power_ctrl_limit( ftm_pwr_ctrl_limit_config->pmax);
                    break;

                case FTM_PMIC_CHG_PWR_CTRL_LIMIT_USB:

                    err_flag = pm_chg_usb_power_ctrl_limit( ftm_pwr_ctrl_limit_config->pmax);
                    break;

                default:

                    err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
                    MSG_HIGH("FTM PMIC CHG PWR CTRL LIMIT -- Invalid selection: %d",ftm_pwr_ctrl_limit_config->select,0,0);
            }
           


            break;

        default:
            
            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            MSG_HIGH( "FTM PMIC CHG Config -- Invalid Command",0,0,0);
            break;

    }

    *err = err_flag;
    rsp  = ftm_pmic_create_gen_response( cmd_ptr,
                                         err_flag, 
                                         FTM_PMIC_TEST_CHG, 
                                         FTM_PMIC_CHG_SET_CONFIG );

    return( rsp);

}





/*===========================================================================

FUNCTION           FTM_PMIC_PROCESS_AMUX_COMMANDS

DESCRIPTION        Handler for AMUX related FTM PMIC commands.
   
DEPENDENCIES
   None.

RETURN VALUE       Response packet

SIDE EFFECTS
   None.
 
===========================================================================*/

ftm_rsp_pkt_type  ftm_pmic_process_amux_commands( ftm_pmic_pkt_type *cmd_ptr)
{

    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    ftm_pmic_amux_header_type              *amux_header;
    ftm_pmic_amux_config_type              *amux_config;
    pm_err_flag_type                        err_flag = PM_ERR_FLAG__SUCCESS;

    amux_header = ( ftm_pmic_amux_header_type *) cmd_ptr;

    switch( amux_header->amux_cmd_id)
    {

        case FTM_PMIC_AMUX_CONFIG:

            amux_config = (ftm_pmic_amux_config_type *) cmd_ptr;
            err_flag    = pm_adc_config_mux( amux_config->mux_sel, amux_config->mux_scale);           

            break;

        default:

            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            
            MSG_HIGH( "FTM PMIC AMUX -- Invalid Command",0,0,0);
            break;

    }

    rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                        err_flag, 
                                        FTM_PMIC_TEST_AMUX, 
                                        amux_header->amux_cmd_id );

    return( rsp);

}



/*===========================================================================

FUNCTION          FTM_PMIC_PROCESS_INT_COMMANDS

DESCRIPTION       Handler for Interrupt related FTM PMIC commands.
   
DEPENDENCIES
   None.

RETURN VALUE      Response packet.
   

SIDE EFFECTS
   None.
 
===========================================================================*/
ftm_rsp_pkt_type ftm_pmic_process_int_commands( ftm_pmic_pkt_type *cmd_ptr)
{

    boolean status = FALSE;
    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    
    ftm_pmic_int_header_type                *int_header;
    pm_err_flag_type                        err_flag = PM_ERR_FLAG__SUCCESS;
    ftm_pmic_int_get_rt_status_cmd_type     *int_get_rt_status_cmd;
    ftm_pmic_int_clr_irq_cmd_type           *int_clr_irq_cmd;
    
    int_header = ( ftm_pmic_int_header_type *) cmd_ptr;


    switch( int_header->int_cmd_id)
    {
        case FTM_PMIC_INT_GET_RT_STATUS:
    
            int_get_rt_status_cmd = ( ftm_pmic_int_get_rt_status_cmd_type *) cmd_ptr;
            err_flag              = pm_get_rt_status( int_get_rt_status_cmd->status_id, &status);
            
            int_get_rt_status_cmd->status = status;
            
            if( err_flag != PM_ERR_FLAG__SUCCESS)
            {
                 rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                     err_flag, 
                                                     FTM_PMIC_TEST_INT, 
                                                     int_header->int_cmd_id );   
            }

            break;

        case FTM_PMIC_INT_CLR_IRQ:

            int_clr_irq_cmd = ( ftm_pmic_int_clr_irq_cmd_type *) cmd_ptr;
            err_flag        = pm_clear_irq( int_clr_irq_cmd->irq_handle);
           
            rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                                err_flag, 
                                                FTM_PMIC_TEST_INT, 
                                                int_header->int_cmd_id );      

            break;

        default:

            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;
            rsp      = ftm_pmic_create_gen_response( cmd_ptr,
                                                     err_flag, 
                                                     FTM_PMIC_TEST_INT, 
                                                     int_header->int_cmd_id );   
            
            MSG_HIGH( "FTM PMIC INT -- Invalid Command",0,0,0);
            break;

    }

    return( rsp);


    
}





/*===========================================================================

FUNCTION        FTM_PMIC_PROCESS_UI_COMMANDS

DESCRIPTION     Handler for UI related FTM PMIC commands.  
   
DEPENDENCIES
   None.

RETURN VALUE    Reponse packet.

SIDE EFFECTS
   None.
 
===========================================================================*/
ftm_rsp_pkt_type ftm_pmic_process_ui_commands(         ftm_pmic_pkt_type *cmd_ptr)
{
    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
    
    ftm_pmic_ui_header_type                 *ui_header;
    pm_err_flag_type                        err_flag = PM_ERR_FLAG__SUCCESS;
    ftm_pmic_ui_set_drive_level_cmd_type    *ui_set_drive_level_cmd;
    ftm_pmic_ui_spkr_config_cmd_type        *ui_config_spkr_cmd;
    uint8                                   drive_level;
    uint16                                  vib_mot_level;
    ui_header = (ftm_pmic_ui_header_type *) cmd_ptr;


    switch( ui_header->ui_cmd_id)
    {

        case FTM_PMIC_UI_SET_DRIVE_LEVEL:

            ui_set_drive_level_cmd = (ftm_pmic_ui_set_drive_level_cmd_type *) cmd_ptr;
            drive_level            = ui_set_drive_level_cmd->drive_level;

            switch( ui_set_drive_level_cmd->high_drive_id)
            {

                case FTM_PMIC_UI_DRV_SEL_LCD:
                      
                    drive_level &= 0x0F;   /* Keep 4 LSB */
                    err_flag     = pm_set_led_intensity( PM_LCD_LED, drive_level);

                    break;

                case FTM_PMIC_UI_DRV_SEL_KPD:
                     
                    drive_level &= 0x0F;   /* Keep 4 LSB */
                    err_flag     = pm_set_led_intensity( PM_KBD_LED, drive_level);

                    break;

                case FTM_PMIC_UI_DRV_SEL_VIB:
                     
                    drive_level  &= 0x1F;  /* Keep 5 LSB */
                    vib_mot_level = drive_level * 100;
                    
                    err_flag |= pm_vib_mot_set_mode( PM_VIB_MOT_MODE__MANUAL);
                    err_flag |= pm_vib_mot_set_volt( vib_mot_level);

                    break;

                default:
                    
                    err_flag = PM_ERR_FLAG__PAR2_OUT_OF_RANGE;
                    break;

            }
            break;

        
        case FTM_PMIC_UI_CONFIG_SPKR:

            ui_config_spkr_cmd = ( ftm_pmic_ui_spkr_config_cmd_type *) cmd_ptr;

            err_flag  = pm_set_speaker_gain( ui_config_spkr_cmd->spkr_gain);
            err_flag |= pm_set_speaker_delay( ui_config_spkr_cmd->spkr_dly);
            err_flag |= pm_speaker_cmd( ui_config_spkr_cmd->spkr_cmd);


            break;
   


        default:

            err_flag = PM_ERR_FLAG__PAR1_OUT_OF_RANGE;

            MSG_HIGH( "FTM PMIC UI -- Invalid Command",0,0,0);


            break;
    }

    rsp = ftm_pmic_create_gen_response( cmd_ptr,
                                    err_flag, 
                                    FTM_PMIC_TEST_UI, 
                                    ui_header->ui_cmd_id );   


    return rsp;
}

/*===========================================================================

FUNCTION           FTM_PMIC_PROCESS_FUSE_CMDS

DESCRIPTION        Handler for manipulating the fuse registers.

DEPENDENCIES
None.

RETURN VALUE       Generic FTM Response packet

SIDE EFFECTS
None.

===========================================================================*/
/* 
The fuse is a sensitive command so a special sequence is introduced for extra
protection. Therefore in the FTM-command packet, each fuse command will contain 2 
special bytes along with the fuse-command. 
*/
#define PM_FUSE_COMMAND_SPECIAL_SEQUENCE1         0xA5
#define PM_FUSE_COMMAND_SPECIAL_SEQUENCE2         0x5A

ftm_rsp_pkt_type ftm_pmic_process_fuse_commands ( ftm_pmic_pkt_type *cmd_ptr )
{
    /* PMIC FTM FUSE Packet defined in ftm_pmic.h */    
    ftm_pmic_process_fuse_commands_type     *pmic_fuse_modify_header ;
    pm_err_flag_type err_flag = PM_ERR_FLAG__SUCCESS;
    ftm_rsp_pkt_type rsp = { FTM_RSP_DO_LEGACY , 0 , NULL , FALSE } ;
    pmic_fuse_modify_header = ( ftm_pmic_process_fuse_commands_type * ) cmd_ptr ;
            
    switch( pmic_fuse_modify_header->ftm_pmic_fuse_cmd_id )
    {
        case FTM_PMIC_CBL_PWR_ON_SWITCH:
        /* 
            For extra protection the "FTM_PMIC_CBL_PWR_ON_SWITCH" sub-command
            has an opcode 0x34 as opposed to 0 or 1.  Also this sub-command 
            can only be invoked if this sub-command is followed by 2
            special sequence bytes, 0xA5 and 0x5A (FUSE COMMAND SPECIAL SEQUENCE 
            BYTE 1 and FUSE COMMAND SPECIAL SEQUENCE BYTE 2 respectively) followed 
            by pm_cbl_pwr_on_switch() options. 
  
            ftm_pmic_fuse_option_byte1  ==> PM_FUSE_COMMAND_SPECIAL_SEQUENCE1 
            ftm_pmic_fuse_option_byte2  ==> PM_FUSE_COMMAND_SPECIAL_SEQUENCE2 
            ftm_pmic_fuse_option_byte3  ==> pm_cbl_pwr_on_switch options.
        */
                
            /* Satisfy sequence criteria */
            if ( ( pmic_fuse_modify_header->ftm_pmic_fuse_option_byte1  
                   == PM_FUSE_COMMAND_SPECIAL_SEQUENCE1 ) &&
              ( pmic_fuse_modify_header->ftm_pmic_fuse_option_byte2  
                == PM_FUSE_COMMAND_SPECIAL_SEQUENCE2 ) && 
              ( (( (pm_cbl_pwr_on_switch_type) 
                  pmic_fuse_modify_header->ftm_pmic_fuse_option_byte3) ) 
               < PM_CBL_PWR_ON__INVALID ) )                   
            {
                err_flag = pm_cbl_pwr_on_switch 
                  ( ( (pm_cbl_pwr_on_switch_type)
                      pmic_fuse_modify_header->ftm_pmic_fuse_option_byte3 ) ) ;
                /* 
                The ftm_pmic_cbl_pwr_on_switch options are:
                    PM_CBL_PWR_ON__ENABLE = 0 ,
                    PM_CBL_PWR_ON__DISABLE = 1 ,
                    PM_CBL_PWR_ON__DIS_PERMANENTLY = 2 ,
                    PM_CBL_PWR_ON__INVALID = 3 and above
                */
            }
            else
            {
                err_flag = 0x8000 ; 
                MSG_HIGH( "FTM PMIC FUSE COMMANDS -- Invalid Fuse Sequence" , 0 , 0 , 0 ) ;
            }
            break;
   
          default:
            err_flag = 0x4000 ; 
              MSG_HIGH( "FTM PMIC FUSE COMMANDS -- Invalid Command" , 0 , 0 , 0 ) ;
              break;
      
    }
    
    if( err_flag != PM_ERR_FLAG__SUCCESS)
    {
        MSG_HIGH( "FTM PMIC GENERIC RW -- Cmd: %d, Error Flag 0x%x" , 
                 pmic_fuse_modify_header->ftm_pmic_fuse_cmd_id , 
                 err_flag , 0 ) ;
    }
    rsp = ftm_pmic_create_ftm_fuse_response  (  cmd_ptr,
                                                err_flag, 
                                                FTM_PMIC_FUSE_CMDS, 
                                                pmic_fuse_modify_header->ftm_pmic_fuse_cmd_id ) ;
    return( rsp ) ;
}


#if defined( FEATURE_PMIC_RTC)
/*==========================================================================

FUNCTION         FTM_PMIC_RTC_ALARM_ISR_CB

DESCRIPTION      Test ISR for use with the FTM PMIC command to set an RTC 
                 alarm.

   
DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
 
===========================================================================*/
void ftm_pmic_rtc_alarm_isr_cb( void)
{
    

    pm_err_flag_type   err_flag;

    //-->TODO read alarm status to find which alarm actually triggered.  Reset
    //appropriate alarm.  Ok for now since only have one alarm in use
    
    err_flag  = pm_reset_rtc_alarm( PM_RTC_ALARM_1);
    err_flag |= pm_clear_irq( PM_RTC_ALRM_IRQ_HDL);
    



    FTM_MSG_HIGH("RTC Alarm ISR handler, Err Flag 0x%x",err_flag,0,0);
}

#endif /* FEATURE_PMIC_RTC */

/*===========================================================================

FUNCTION        FTM_CHG_APP_ENABLE

DESCRIPTION     Interfaces to Charger application to enable or disable the
                charger state machine.

   
DEPENDENCIES
   None.

RETURN VALUE
   pm_err_flag_type -- always SUCCESS.  param returned for consistency. 
   
 
SIDE EFFECTS
   None.
 
===========================================================================*/
pm_err_flag_type ftm_chg_app_enable( pm_switch_cmd_type pm_cmd)
{


    switch( pm_cmd)
    {
        case PM_OFF_CMD:
        {

            /* Kill app code state machine */

#if defined( FEATURE_BATTERY_CHARGER)
            chg_stop_state_machine();
#elif defined(FEATURE_CHG_TASK)
            chg_charging_ftm_switch(CHG_CMD_DISABLE);
#elif defined( FEATURE_BATMAN_TASK)
#error code not present
#endif

            break;
        }

        case PM_ON_CMD:
        {
            
            /* Force App code to initial state */
        
#if defined( FEATURE_BATTERY_CHARGER)
            chg_idle_state_init();
#elif defined(FEATURE_CHG_TASK)
            chg_charging_ftm_switch(CHG_CMD_ENABLE);
#elif defined( FEATURE_BATMAN_TASK)         
#error code not present
#endif

            break;
        }

        default: 
            break;
    }
    return( PM_ERR_FLAG__SUCCESS);
}


#endif /* FEATURE_PMIC_FTM_BYPASS */
#endif /* FEATURE_PMIC */
#endif /* FEATURE_FACTORY_TESTMODE */
