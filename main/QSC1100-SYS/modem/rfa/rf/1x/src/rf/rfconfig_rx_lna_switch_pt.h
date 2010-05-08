#ifndef RFCONFIG_RX_LNA_SWITCH_PT_H
#define RFCONFIG_RX_LNA_SWITCH_PT_H

/*===========================================================================
         R F C O N F I G   D E F I N I T I O N   H E A D E R   F I L E
       F O R   R x   L N A   S W I T C H   P O I N T   S T R U C T U R E

DESCRIPTION
  This file lists the interface available for manipulating individual
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


$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfconfig_rx_lna_switch_pt.h#1 $ $DateTime: 2008/04/11 05:11:57 $ $Author: ppottier $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/14/07   Vish    Created initial version.
===========================================================================*/

/* =========================================================================
                         INCLUDE FILES
========================================================================= */
#include "comdef.h"
#include "rficap.h"   // for RF_HAS_RFCONFIG_RX_LNA
#include "rf_error.h"

#ifdef RF_HAS_RFCONFIG_RX_LNA

/* =========================================================================
                       MACROS / CONSTANTS
========================================================================= */
// Use this to static initialize a switch point to reset state at load time;
//  similar effect as using rfconfig_rx_lna_switch_pt_reset() at run-time
#define RFCONFIG_RX_LNA_SWITCH_PT_RESET_VAL_INITIALIZER  \
   {                                                     \
      RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__NONE,       \
      {0,0,0},                                           \
      0,                                                 \
      0                                                  \
   }


/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */
typedef enum
{
   RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED,
   RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__OLTA,
   RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__NONE
} rfconfig_rx_lna_switch_pt_config_mode_type;

typedef struct
{
   rfconfig_rx_lna_switch_pt_config_mode_type config_mode;
   union
   {
      struct rfconfig_rx_lna_switch_pt_fixed_config
      {
         int16 rise;
         int16 fall;
         int16 im;
      } fixed;
      struct rfconfig_rx_lna_switch_pt_olta_config
      {
         int16 hysteresis;
         int16 backoff;
      } olta;
   } config_data;
   int16 bypass_hold_timer;       // Bypass hold timer
   int16 non_bypass_hold_timer;   // Non bypass hold timer
} rfconfig_rx_lna_switch_pt_type;


/* =========================================================================
                         FUNCTION PROTOTYPES
========================================================================= */
extern void rfconfig_rx_lna_switch_pt_reset(
   rfconfig_rx_lna_switch_pt_type *switch_pt_ptr);

extern void rfconfig_rx_lna_switch_pt_set_fixed_config(
   rfconfig_rx_lna_switch_pt_type *switch_pt_ptr,
   int16                           rise,
   int16                           fall,
   int16                           im,
   int16                           bypass_hold_timer,
   int16                           non_bypass_hold_timer);
extern void rfconfig_rx_lna_switch_pt_set_olta_config(
   rfconfig_rx_lna_switch_pt_type *switch_pt_ptr,
   int16                           hysteresis,
   int16                           backoff,
   int16                           bypass_hold_timer,
   int16                           non_bypass_hold_timer);

extern boolean rfconfig_rx_lna_switch_pt_initialized(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr);

extern rfconfig_rx_lna_switch_pt_config_mode_type rfconfig_rx_lna_switch_pt_get_config_mode(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr);
extern int16 rfconfig_rx_lna_switch_pt_get_rise(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr);
extern int16 rfconfig_rx_lna_switch_pt_get_fall(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr);
extern int16 rfconfig_rx_lna_switch_pt_get_im(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr);
extern int16 rfconfig_rx_lna_switch_pt_get_hysteresis(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr);
extern int16 rfconfig_rx_lna_switch_pt_get_backoff(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr);
extern int16 rfconfig_rx_lna_switch_pt_get_bypass_hold_timer(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr);
extern int16 rfconfig_rx_lna_switch_pt_get_non_bypass_hold_timer(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr);

extern void rfconfig_rx_lna_switch_pt_set_rise(
   rfconfig_rx_lna_switch_pt_type *switch_pt_ptr,
   int16                           rise_val);
extern void rfconfig_rx_lna_switch_pt_set_fall(
   rfconfig_rx_lna_switch_pt_type *switch_pt_ptr,
   int16                           fall_val);
extern void rfconfig_rx_lna_switch_pt_set_im(
   rfconfig_rx_lna_switch_pt_type *switch_pt_ptr,
   int16                           im_val);
extern void rfconfig_rx_lna_switch_pt_set_hysteresis(
   rfconfig_rx_lna_switch_pt_type *switch_pt_ptr,
   int16                           hysteresis);


// All inline defs below... (INLINE didn't work quite right- got multiple defn linker errors)
/*
INLINE boolean rfconfig_rx_lna_switch_pt_initialized(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   return ( (NULL != switch_pt_ptr)
            && (switch_pt_ptr->config_mode < RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__NONE) );
}

INLINE rfconfig_rx_lna_switch_pt_config_mode_type rfconfig_rx_lna_switch_pt_get_config_mode(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);

   return switch_pt_ptr->config_mode;
}

INLINE int16 rfconfig_rx_lna_switch_pt_get_rise(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   RF_ASSERT(RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == switch_pt_ptr->config_mode);

   return switch_pt_ptr->config_data.fixed.rise;
}

INLINE int16 rfconfig_rx_lna_switch_pt_get_fall(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   RF_ASSERT(RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == switch_pt_ptr->config_mode);

   return switch_pt_ptr->config_data.fixed.fall;
}

INLINE int16 rfconfig_rx_lna_switch_pt_get_im(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   RF_ASSERT(RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__FIXED == switch_pt_ptr->config_mode);

   return switch_pt_ptr->config_data.fixed.im;
}

INLINE int16 rfconfig_rx_lna_switch_pt_get_hysteresis(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   RF_ASSERT(RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__OLTA == switch_pt_ptr->config_mode);

   return switch_pt_ptr->config_data.olta.hysteresis;
}

INLINE int16 rfconfig_rx_lna_switch_pt_get_backoff(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   RF_ASSERT(RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__OLTA == switch_pt_ptr->config_mode);

   return switch_pt_ptr->config_data.olta.backoff;
}

INLINE int16 rfconfig_rx_lna_switch_pt_get_bypass_hold_timer(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   RF_ASSERT(switch_pt_ptr->config_mode < RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__NONE);

   return switch_pt_ptr->bypass_hold_timer;
}

INLINE int16 rfconfig_rx_lna_switch_pt_get_non_bypass_hold_timer(
   const rfconfig_rx_lna_switch_pt_type *switch_pt_ptr)
{
   RF_ASSERT(NULL != switch_pt_ptr);
   RF_ASSERT(switch_pt_ptr->config_mode < RFCONFIG_RX_LNA_SWITCH_PT_CONFIG_MODE__NONE);

   return switch_pt_ptr->non_bypass_hold_timer;
}
*/

#endif /* RF_HAS_RFCONFIG_RX_LNA */

#endif //RFCONFIG_RX_LNA_SWITCH_PT_H


