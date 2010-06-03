/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


                        RF AUTOMATIC GAIN CONTROL SERVICES

GENERAL DESCRIPTION
   This module contains the CDMA AGC control functions

EXTERNALIZED FUNCTIONS

rf_get_raw_cdma_rx_agc
  This function returns the CDMA Rx AGC setting in 8 bit two's complement
  format.

rf_get_raw_cdma_tx_agc
  This function returns the raw CDMA Tx AGC setting in 10 bit binary format.

rf_is_lna1_on
  This function returns the current LNA 1 on/off state

rf_islna2_on
  This function returns the current LNA 2 on/off state

rf_get_lna_state()
  Returns the current LNA state


INITIALIZATION AND SEQUENCING REQUIREMENTS


Copyright (c) 1998,1999,2000 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2001,2002,2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2004,2005,2006 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2007           by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*=================================================================================

 10/12/08  vm      Added support for different Access Probe switchpoints   
7/02/01   aks     Created from re-organization of rfmsm.c, rev 1.10.
                   O:/src/asw/COMMON/vcs/rfmsm.c_v   1.10   21 May 2001 16:40:16
===================================================================================*/
#include "rf_device_functions.h"
#include "rft.h"
#include "rfi.h"

#ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG
extern boolean rf_pa_config_is_traffic_state;
#ifdef RF_HAS_HDR
#error code not present
#endif
#endif

#define RF_PA_RISE_MAX_VAL   255 

#ifdef RF_HAS_HDR
#error code not present
#endif

int8 rf_debug_rise_fall_diff_val = 10;

extern pa_switchpoint_type rf_cagc_get_max_active_pa_switchpoint(void);

#ifdef RF_HAS_PA_ACCESS_PROBE_CONFIG 
/*===========================================================================
FUNCTION RFT_CONFIG_PA_HYST_TIMER_DIFF_IN_ACCESS_PROBE

DESCRIPTION
This function is called upon to determine if PA Hysterisi timer is different
in Access Probe and Traffic

DEPENDENCIES
  None

RETURN VALUE
 Boolean : True if difefrent, ELSE FALSE

SIDE EFFECTS
  None

===========================================================================*/
boolean rft_config_pa_hyst_timer_diff_in_access_probe()
{
    rft_device_enum rft_device_in_card;

    rft_device_in_card = rft_get_hw_rft_device();    

    switch( rft_device_in_card)
    {    
    case QSC60X5_TX:
    case RTR6500_TX:
    case QSC11X0_TX:
      return TRUE;
      break;    

    default:
      return FALSE;
      break;
    }
}
/*=========================================================================

FUNCTION RFT_GET_1X_ACCESS_PROBE_PA_BACKOFF

DESCRIPTION
This function is called so that RF Driver can program different 
PA hysterisis timerand PA switchpoints for Access Probes different 
from traffic.
Returns the Backoff in PA switchpoints for Access Probe from traffic state
in 2/5th of dB

DEPENDENCIES
  None

RETURN VALUE
Returns the Backoff in PA switchpoints for Access Probe from traffic state
in 2/5th of dB
===========================================================================*/
int2 rft_get_1X_access_probe_pa_backoff(rfcom_device_enum_type device,rf_card_band_type band)
{
  rft_device_enum rft_device_in_card;
  int2 rf_pa_access_switch_pt;
  int2 pa_r1_rise_value;
  pa_switchpoint_type max_pa_switchpoint;
  int16 backoff = 0;
 
  rf_chain_state_type *rf_chain_ptr = 
        &rf_chain_status[ rfcom_to_path[ RFCOM_TRANSCEIVER_0 ] ];

  pa_r1_rise_value = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r1_rise);

  max_pa_switchpoint = rf_cagc_get_max_active_pa_switchpoint();
  
  rft_device_in_card = rft_get_hw_rft_device();

  if(max_pa_switchpoint == PA_SWITCHPOINT_3) 
  {
    pa_r1_rise_value = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r3_rise);
  }
  else if(max_pa_switchpoint == PA_SWITCHPOINT_2) 
  {
    pa_r1_rise_value = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r2_rise);
  }
  else if(max_pa_switchpoint == PA_SWITCHPOINT_1) 
  {
    pa_r1_rise_value = (int2) ((word) rf_chain_ptr->rf_nv_dig_items_ptr->rfnv_r1_rise);
  }
  else
  {
    return(0);
  }
  
  switch(rft_device_in_card)
  {  
  case QSC60X5_TX:
   switch (band)
   {
   case RF_BC0_BAND:
   case RF_BC3_BAND:
   case RF_BC10_BAND:
     rf_pa_access_switch_pt = 112;
     break;

   case RF_BC1_BAND:
   case RF_BC4_BAND:
   case RF_BC6_BAND:
   case RF_BC14_BAND:
   case RF_BC15_BAND:
     rf_pa_access_switch_pt = 120;
     break;

   default:
     rf_pa_access_switch_pt = pa_r1_rise_value;
     break;
   }
   break;

  case QSC11X0_TX:
   switch (band)
   {
   case RF_BC0_BAND:
   case RF_BC5_BAND:
   case RF_BC10_BAND:
   case RF_BC11_BAND:
     rf_pa_access_switch_pt = 112;
     break;

   case RF_BC1_BAND:   
   case RF_BC6_BAND:      
     rf_pa_access_switch_pt = 120;
     break;

   default:
     rf_pa_access_switch_pt = pa_r1_rise_value;
     break;
   }
   break;
    
  case RTR6500_TX:
    switch (band)
    {
    case RF_BC0_BAND:
    case RF_BC3_BAND:
    case RF_BC10_BAND:
      rf_pa_access_switch_pt = 112;
      break;

    case RF_BC1_BAND:
    case RF_BC4_BAND:
    case RF_BC6_BAND:
    case RF_BC14_BAND:
    case RF_BC15_BAND:
      rf_pa_access_switch_pt = 120;
      break;

    default:
      rf_pa_access_switch_pt = pa_r1_rise_value;
      break;
     }
     break;   
  default:
   rf_pa_access_switch_pt = pa_r1_rise_value;
   break;
  }
    if ((pa_r1_rise_value != RF_PA_RISE_MAX_VAL) && (pa_r1_rise_value > rf_pa_access_switch_pt))
    {
      backoff =  pa_r1_rise_value -  rf_pa_access_switch_pt;
    }
    return(backoff); 
}
/*===========================================================================
FUNCTION RF_SET_PA_ACCESS_PROBE_CONFIG

DESCRIPTION
This function is called by Mux Layer to infrorm the RF Driver
that its going to be in traffic state.
This is done so that RF Driver can program different PA hysterisis timer
and PA switchpoints for Access Probes different from traffic.

DEPENDENCIES
  None

RETURN VALUE
  Sets the variable used for traking when the traffic state begin
  and ends 

SIDE EFFECTS
  None

===========================================================================*/
void rf_set_pa_access_probe_config(rfcom_device_enum_type device ,boolean is_traffic_state)
{    
    rft_device_enum rft_device_in_card;

    rft_device_in_card = rft_get_hw_rft_device();    

    switch( rft_device_in_card)
    {    
    case QSC60X5_TX:
    case RTR6500_TX:
    case QSC11X0_TX:
    if (is_traffic_state)
    {
      rf_pa_config_is_traffic_state = TRUE;
    }
    else
    {
      rf_pa_config_is_traffic_state = FALSE;
    }
    break;
    
    default:
      rf_pa_config_is_traffic_state = FALSE;
      break;
    }
}
#endif

/*boolean rft_pa_hysterisis_timer_access_probe_is_disabled(rfcom_device_enum_type device,rf_card_band_type band)
{
    rft_device_type *rftdev;
    boolean rf_access_probe_timer_is_disabled = FALSE;

    rftdev = &rft_device_0;

    switch(rftdev->rft_device)
    {
    #ifdef ZIF_LIBRARY_SUPPORTS_QSC60X5
    case QSC60X5_TX:
      rf_access_probe_timer_is_disabled = TRUE;
      break;
    #endif
    #ifdef ZIF_LIBRARY_SUPPORTS_RTR6500
    case RTR6500_TX:
      rf_access_probe_timer_is_disabled = TRUE;
      break;
    #endif
    default:
     rf_access_probe_timer_is_disabled = FALSE;
     break;
    }
    return(rf_access_probe_timer_is_disabled); 
}*/

#ifdef RF_HAS_1X_PA_SWITCH_POINTS_DIFF_FROM_NV
/*===========================================================================
FUNCTION RFT_HAS_PA_SWITCHPOINTS_DIFF_FROM_NV

DESCRIPTION
This function is called to determine if RFT device has PA 
Switchpoints difefrent from NV

DEPENDENCIES
  None

RETURN VALUE
  Boolean

SIDE EFFECTS
  None

===========================================================================*/
boolean rft_has_pa_switchpoints_diff_from_NV(void)
{    
    rft_device_enum rft_device_in_card;

    boolean rf_pa_sw_is_diff_from_nv = FALSE;

    rft_device_in_card = rft_get_hw_rft_device();
    
    switch(rft_device_in_card)
    {    
    case QSC60X5_TX:
    case RTR6500_TX:
    case QSC11X0_TX:
      rf_pa_sw_is_diff_from_nv = TRUE;
      break;
            
    default:
     rf_pa_sw_is_diff_from_nv = FALSE;
     break;
    }
    return(rf_pa_sw_is_diff_from_nv); 
}


/*===========================================================================
FUNCTION RFT_GET_1X_PA_RISE_FALL_DIFF_VAL

DESCRIPTION
This function is called to get the Hardcoded minimum Power Backoffs 
between Rise and FALL needed for 4 PA Switchpoints.

DEPENDENCIES
  None

RETURN VALUE
 Valuses for Power backoffs in PA Switch point units
 Value of 5 corresponds to 2 dB Backoff

SIDE EFFECTS
  None

===========================================================================*/
void  rft_get_1X_pa_rise_fall_diff_val(int8* diff_val_ptr)
{
    rft_device_enum rft_device_in_card;

    int i;
    //boolean rf_pa_sw_is_diff_from_nv = FALSE;
    rft_device_in_card = rft_get_hw_rft_device();
    
    switch(rft_device_in_card)
    {   
    case QSC60X5_TX:    
      for (i=0;i<(RF_NUM_TX_LINEARIZERS - 1);i++) 
      {
        diff_val_ptr[i] = 10;
      }     
      break;
        
    case RTR6500_TX:
      for (i=0;i<(RF_NUM_TX_LINEARIZERS - 1);i++) 
      {
        diff_val_ptr[i] = rf_debug_rise_fall_diff_val;//10;
      }        
      break;

    case QSC11X0_TX:
      for (i=0;i<(RF_NUM_TX_LINEARIZERS - 1);i++) 
      {
        diff_val_ptr[i] = 5;
      }        
      break;
    
    default:     
      for (i=0;i<(RF_NUM_TX_LINEARIZERS - 1);i++) 
      {
        diff_val_ptr[i] = 0;
      }        
     break;
    }
}
#endif


#ifdef RF_HAS_HDR
#error code not present
#endif
