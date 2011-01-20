/*===========================================================================
 R F C O N F I G   D E F I N I T I O N   I M P L E M E N T A T I O N   F I L E
       F O R   R x   L N A   S W I T C H   P O I N T   S T R U C T U R E

DESCRIPTION
  This file implements the interface available for manipulating individual
  Rx LNA switch points.

  CAUTION:
  Do NOT access the individual switch point struct fields directly; rather
  use only the function interfaces provided to ensure the integrity of the data 
  contained within.

Copyright (c) 2007   by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfconfig_rx_lna_switch_pt.c#2 $ $DateTime: 2008/11/19 11:46:56 $ $Author: lesp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/29/08   et      klocwork fixes
03/14/07   Vish    Created initial version.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "rfconfig_rx_lna_switch_pt.h"
#include "rf_error.h"

#ifdef RF_HAS_RFCONFIG_RX_LNA

/*===========================================================================
                     LOCAL TYPE DEFINITIONS
===========================================================================*/


/*===========================================================================
                     LOCAL FUNCTION PROTOTYPES
===========================================================================*/


/*===========================================================================
              LOCAL MACRO, VARIABLE AND CONSTANT DEFINITIONS
===========================================================================*/


/*===========================================================================
                       FUNCTION DEFINITIONS
===========================================================================*/
void rfconfig_rx_lna_switch_pt_reset(
   rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return; 
   }

   switch_pt_ptr->config_mode = RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__NONE;
   switch_pt_ptr->bypass_hold_timer = 0;
   switch_pt_ptr->non_bypass_hold_timer = 0;
}

void rfconfig_rx_lna_switch_pt_set_fixed_config(
   rfconfig_rx_lna_switch_pt_type *switch_pt_ptr,
   int16                           rise,
   int16                           fall,
   int16                           im,
   int16                           bypass_hold_timer,
   int16                           non_bypass_hold_timer)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return; 
   }

   switch_pt_ptr->config_mode = RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED;
   switch_pt_ptr->config_data.fixed.rise = rise;
   switch_pt_ptr->config_data.fixed.fall = fall;
   switch_pt_ptr->config_data.fixed.im = im;

   switch_pt_ptr->bypass_hold_timer = bypass_hold_timer;
   switch_pt_ptr->non_bypass_hold_timer = non_bypass_hold_timer;
}

void rfconfig_rx_lna_switch_pt_set_olta_config(
   rfconfig_rx_lna_switch_pt_type *switch_pt_ptr,
   int16                           hysteresis,
   int16                           backoff,
   int16                           bypass_hold_timer,
   int16                           non_bypass_hold_timer)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return; 
   }

   switch_pt_ptr->config_mode = RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__OLTA;
   switch_pt_ptr->config_data.olta.hysteresis = hysteresis;
   switch_pt_ptr->config_data.olta.backoff = backoff;

   switch_pt_ptr->bypass_hold_timer = bypass_hold_timer;
   switch_pt_ptr->non_bypass_hold_timer = non_bypass_hold_timer;
}

boolean rfconfig_rx_lna_switch_pt_initialized(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   return ( (NULL != switch_pt_ptr)
            && (switch_pt_ptr->config_mode < RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__NONE) );
}

rfconfig_rx_lna_switch_pt_config_mode_type rfconfig_rx_lna_switch_pt_get_config_mode(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__NONE; 
   }

   return switch_pt_ptr->config_mode;
}

int16 rfconfig_rx_lna_switch_pt_get_rise(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return -1; 
   }
   RF_ASSERT(RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == switch_pt_ptr->config_mode);

   return switch_pt_ptr->config_data.fixed.rise;
}

int16 rfconfig_rx_lna_switch_pt_get_fall(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return -1; 
   }
   RF_ASSERT(RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == switch_pt_ptr->config_mode);

   return switch_pt_ptr->config_data.fixed.fall;
}

int16 rfconfig_rx_lna_switch_pt_get_im(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return -1; 
   }
   RF_ASSERT(RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == switch_pt_ptr->config_mode);

   return switch_pt_ptr->config_data.fixed.im;
}

int16 rfconfig_rx_lna_switch_pt_get_hysteresis(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return -1; 
   }
   RF_ASSERT(RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__OLTA == switch_pt_ptr->config_mode);

   return switch_pt_ptr->config_data.olta.hysteresis;
}

int16 rfconfig_rx_lna_switch_pt_get_backoff(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return -1; 
   }
   RF_ASSERT(RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__OLTA == switch_pt_ptr->config_mode);

   return switch_pt_ptr->config_data.olta.backoff;
}

int16 rfconfig_rx_lna_switch_pt_get_bypass_hold_timer(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return -1; 
   }
   RF_ASSERT(switch_pt_ptr->config_mode < RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__NONE);

   return switch_pt_ptr->bypass_hold_timer;
}

int16 rfconfig_rx_lna_switch_pt_get_non_bypass_hold_timer(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return -1; 
   }
   RF_ASSERT(switch_pt_ptr->config_mode < RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__NONE);

   return switch_pt_ptr->non_bypass_hold_timer;
}

void rfconfig_rx_lna_switch_pt_set_rise(
   rfconfig_rx_lna_switch_pt_type *switch_pt_ptr,
   int16                           rise_val)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return; 
   }
   RF_ASSERT(RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == switch_pt_ptr->config_mode);

   switch_pt_ptr->config_data.fixed.rise = rise_val;
}

void rfconfig_rx_lna_switch_pt_set_fall(
   rfconfig_rx_lna_switch_pt_type *switch_pt_ptr,
   int16                           fall_val)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return; 
   }
   RF_ASSERT(RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == switch_pt_ptr->config_mode);

   switch_pt_ptr->config_data.fixed.fall = fall_val;
}

void rfconfig_rx_lna_switch_pt_set_im(
   rfconfig_rx_lna_switch_pt_type *switch_pt_ptr,
   int16                           im_val)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return; 
   }
   RF_ASSERT(RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == switch_pt_ptr->config_mode);

   switch_pt_ptr->config_data.fixed.im = im_val;
}

void rfconfig_rx_lna_switch_pt_set_hysteresis(
   rfconfig_rx_lna_switch_pt_type *switch_pt_ptr,
   int16                           hysteresis)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   if (switch_pt_ptr == NULL) 
   {
	   RF_FATAL_ERROR("switch_pt_ptr is NULL");
	   rf_error_flag = TRUE;
	   return; 
   }
   RF_ASSERT(RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__OLTA == switch_pt_ptr->config_mode);

   switch_pt_ptr->config_data.olta.hysteresis = hysteresis;
}

#endif /* RF_HAS_RFCONFIG_RX_LNA */


