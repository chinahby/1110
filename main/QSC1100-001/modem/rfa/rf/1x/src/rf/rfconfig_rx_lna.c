/*===========================================================================
 R F C O N F I G   D E F I N I T I O N   I M P L E M E N T A T I O N   F I L E
                  F O R   R x   L N A   S T R U C T U R E

DESCRIPTION
  This file implements the interface available for manipulating the configuration
  items for an LNA on a single receive chain.

  CAUTION:
  Do NOT access the individual LNA config struct fields directly; 
  rather use only the function interfaces provided to ensure the integrity
  of the data contained within.

Copyright (c) 2007   by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.


$Header: //source/qcom/qct/modem/rfa/rf/1x/main/qsc1100/src/rf/rfconfig_rx_lna.c#2 $ $DateTime: 2008/11/19 11:46:56 $ $Author: lesp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/15/08   ag      lint errors fixed
04/29/08   et      klocwork fixes
06/05/07   Vish    Added rfconfig_rx_lna_[set/get]_switch_point_info().
03/14/07   Vish    Created initial version.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "rfconfig_rx_lna.h"
#include "rf_error.h"
#include "err.h"


#ifdef RF_HAS_RFCONFIG_RX_LNA

/*===========================================================================
                     LOCAL TYPE DEFINITIONS
===========================================================================*/


/*===========================================================================
                     LOCAL FUNCTION PROTOTYPES
===========================================================================*/
/* ------- START of functions not being used now -------

static void rfconfig_rx_lna_set_linearity_info(
   rfconfig_rx_lna_type                         *lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type         mode,
   uint32                                        linearity_index,
   const rfconfig_rx_lna_linearity_info_type    *linearity_info_ptr);
static void rfconfig_rx_lna_set_mode_info(
   rfconfig_rx_lna_type                         *lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type         mode,
   const rfconfig_rx_lna_mode_info_type         *mode_info_ptr);
static const rfconfig_rx_lna_mode_info_type* rfconfig_rx_lna_get_mode_info(
   const rfconfig_rx_lna_type                   *lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type         mode);
   
   ------- END of functions not being used now ------- */


/*===========================================================================
              LOCAL MACRO, VARIABLE AND CONSTANT DEFINITIONS
===========================================================================*/


/*===========================================================================
                       FUNCTION DEFINITIONS
===========================================================================*/

/*==========================================================================
FUNCTION   rfconfig_rx_lna_reset                          EXTERNAL FUNCTION

DESCRIPTION
   This function clears out the database for a *particular* Rx LNA. Here 
   "particular" refers to an LNA on a single RF chain only and it holds
   info about LNA switch points for all linearities (meaningful to that
   LNA) across various operational modes (like 1X or EVDO). Specifically
   it doesn't hold info for that LNA across multiple band classes, so
   multiple database have to be created for the same LNA h/w on a per band
   class basis.

DEPENDENCIES
   Use this function to start the database in a guaranteed cleared out
   state before operating on it using other APIs in this module:
    - rfconfig_rx_lna_set_switch_points()
    - rfconfig_rx_lna_get_linearity_info()
    - rfconfig_rx_lna_get_switch_point_info_from_linearity()

RETURN VALUE
   None.

SIDE EFFECTS
   None.
===========================================================================*/
void rfconfig_rx_lna_reset(
   rfconfig_rx_lna_type *lna_config_ptr)
{
   rfconfig_rx_lna_operational_mode_type    mode = RFCONFIG_RX_LNA_OPERATIONAL_MODE__NONE;
   rfconfig_rx_lna_mode_info_type          *mode_info_ptr = NULL;
   rfconfig_rx_lna_linearity_info_type     *linearity_info_ptr = NULL;
   uint32                                   linearity_index = 0;
   uint32                                   gain_step_index = 0;

   RF_ASSERT(NULL != lna_config_ptr);

   // perform a deep reset of the nested data structures...
   for (
      mode = RFCONFIG_RX_LNA_OPERATIONAL_MODE__1X;
      mode < RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES;
      mode ++)
   {
      mode_info_ptr = &lna_config_ptr->mode_info[mode];

      mode_info_ptr->mode_configured = FALSE;
      for (
         linearity_index = 0;
         linearity_index < RFCONFIG_RX_LNA_MAX_LINEARITY_STAGES;
         linearity_index ++)
      {
         linearity_info_ptr = &mode_info_ptr->linearity_info[linearity_index];

         linearity_info_ptr->num_gain_steps = 0;
         for (
            gain_step_index = 0;
            gain_step_index < RFCONFIG_RX_LNA_MAX_GAIN_STEPS;
            gain_step_index ++)
         {
            rfconfig_rx_lna_switch_pt_reset(&linearity_info_ptr->switch_pt[gain_step_index]);
         }
      }
   }
}


/*==========================================================================
FUNCTION   rfconfig_rx_lna_set_switch_points             EXTERNAL FUNCTION

DESCRIPTION
   This function programs the Rx LNA database with info about one or more
   Rx LNA switch points for a given linearity (normal or one of low power
   modes) while the LNA operates in a particualr operational mode (like 1X
   or EVDO).

DEPENDENCIES
   rfconfig_rx_lna_reset() should have been called prior to the *first* instance
   of the invocation of this API for a *particular* Rx LNA. That way, operations
   are performed for the first time on a properly guaranteed empty database.
   Note: Do not call rfconfig_rx_lna_reset() API before invoking this API unless
         you want LNA database to be cleared out completely before this API
         takes effect. This will be the case when this API is used multiple
         times for incrementally building the database for a particular Rx LNA.
         
RETURN VALUE
   None.

SIDE EFFECTS
   None.
===========================================================================*/
void rfconfig_rx_lna_set_switch_points(
   rfconfig_rx_lna_type                  *lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type  mode,
   uint32                                 linearity_index,
   uint32                                 num_switch_pts,
   const rfconfig_rx_lna_switch_pt_type  *switch_pt_array)
{
   uint32                                switch_pt_index = 0;
   rfconfig_rx_lna_linearity_info_type  *linearity_info_ptr = NULL;
   const rfconfig_rx_lna_switch_pt_type *curr_switch_pt_ptr = NULL;

   RF_ASSERT(NULL != lna_config_ptr);
   RF_ASSERT(mode < RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES);
   RF_ASSERT(linearity_index < RFCONFIG_RX_LNA_MAX_LINEARITY_STAGES);
   RF_ASSERT(num_switch_pts <= RFCONFIG_RX_LNA_MAX_GAIN_STEPS);
   RF_ASSERT(NULL != switch_pt_array);

   if (mode >= RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES)
   {
	   MSG_FATAL("mode invalid",0,0,0);
	   rf_error_flag = TRUE;
	   return;
   }
   if ((lna_config_ptr == NULL) || (switch_pt_array == NULL))
   {
	   MSG_FATAL("lna_config_ptr or switch_pt_array is NULL",0,0,0);
	   rf_error_flag = TRUE;
	   return;
   }
   if(linearity_index >= RFCONFIG_RX_LNA_MAX_LINEARITY_STAGES)
   {
	   MSG_FATAL("linearity_index invalid",0,0,0);
	   rf_error_flag = TRUE;
	   return;
   }
   if(num_switch_pts > RFCONFIG_RX_LNA_MAX_GAIN_STEPS)
   {
	   MSG_FATAL("num_switch_pts invalid",0,0,0);
	   rf_error_flag = TRUE;
	   return;
   } 

   linearity_info_ptr = &lna_config_ptr->mode_info[mode].linearity_info[linearity_index];
   for (
      switch_pt_index = 0;
      switch_pt_index < num_switch_pts;
      switch_pt_index ++)
   {
      curr_switch_pt_ptr = &switch_pt_array[switch_pt_index];
      RF_ASSERT( rfconfig_rx_lna_switch_pt_initialized(curr_switch_pt_ptr) );
      linearity_info_ptr->switch_pt[switch_pt_index] = *curr_switch_pt_ptr;
   }
   linearity_info_ptr->num_gain_steps = num_switch_pts;
   lna_config_ptr->mode_info[mode].mode_configured = TRUE;
}


/*==========================================================================
FUNCTION   rfconfig_rx_lna_get_linearity_info             EXTERNAL FUNCTION

DESCRIPTION
   This function returns info about LNA switch points that have been
   programmed into this database for the LNA on the specified RF chain 
   (primary or secondary), operational mode (like 1X or EVDO), and the 
   linearity (i.e normal or one of the low power modes).

DEPENDENCIES
   - At least rfconfig_rx_lna_reset() should have been called prior to this
     invocation to initialize the Rx LNA database to a clean void state that
     would cause this API to return a NULL value correctly (to indicate that
     the database doesn't have any programmed LNA switch points yet).
   - Typically though for this API to return a valid NON-NULL value, the
     database should have been properly initialized with switch point data
     using rfconfig_rx_lna_set_switch_points() calls.

RETURN VALUE
   - Returns info about available LNA switch points for a given mode (like 1X
     or EVDO) and the specified linearity.
   Type: const rfconfig_rx_lna_linearity_info_type*
   - Possible values:
     - non-NULL  --->  At least one LNA switch point data is available
     - NULL      --->  LNA switch point data isn't available

SIDE EFFECTS
   None.
===========================================================================*/
const rfconfig_rx_lna_linearity_info_type* rfconfig_rx_lna_get_linearity_info(
   const rfconfig_rx_lna_type                   *lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type         mode,
   uint32                                        linearity_index)
{
   const rfconfig_rx_lna_linearity_info_type  *linearity_info_ptr = NULL;

   RF_ASSERT(NULL != lna_config_ptr);
   RF_ASSERT(mode < RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES);
   RF_ASSERT(linearity_index < RFCONFIG_RX_LNA_MAX_LINEARITY_STAGES);

   if (mode >= RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES)
   {
	   MSG_FATAL("mode invalid",0,0,0);
	   rf_error_flag = TRUE;
	   return NULL;
   }
   if (linearity_index >= RFCONFIG_RX_LNA_MAX_LINEARITY_STAGES)
   {
	   MSG_FATAL("linearity_index invalid",0,0,0);
	   rf_error_flag = TRUE;
	   return NULL;
   }

   if ( lna_config_ptr->mode_info[mode].mode_configured
        && (lna_config_ptr->mode_info[mode].linearity_info[linearity_index].num_gain_steps > 0) )
   {
      linearity_info_ptr = &lna_config_ptr->mode_info[mode].linearity_info[linearity_index];
   }

   return linearity_info_ptr;
}


/*==========================================================================
FUNCTION   rfconfig_rx_lna_get_switch_point_info_from_linearity  EXTERNAL FUNCTION

DESCRIPTION
   This function returns info about a single Rx LNA switch point belonging to
   the specified linearity (info about which would have been retrieved earlier
   using rfconfig_rx_lna_get_linearity_info API) and LNA gain step.

DEPENDENCIES
   - At least rfconfig_rx_lna_reset() should have been called prior to this
     invocation to initialize the Rx LNA database to a clean void state that
     would cause this API to return a NULL value correctly (to indicate that
     the database doesn't have any programmed LNA switch points yet).
   - Typically though for this API to return a valid NON-NULL value, the
     database should have been properly initialized with switch point data
     using rfconfig_rx_lna_set_switch_points() calls.
   - Prior to invoking this API for getting LNA switch point data, info
     about the desired linearity must be retrieved first through
     rfconfig_rx_lna_get_linearity_info() API.

RETURN VALUE
   - Returns info about a single Rx LNA switch point.
   Type: const rfconfig_rx_lna_switch_pt_type*
   - Possible values:
     - non-NULL  --->  Valid LNA switch point data is available
     - NULL      --->  LNA switch point data isn't available

SIDE EFFECTS
   None.
===========================================================================*/
const rfconfig_rx_lna_switch_pt_type* rfconfig_rx_lna_get_switch_point_info_from_linearity(
   const rfconfig_rx_lna_linearity_info_type     *linearity_info_ptr,
   uint32                                         gain_step_index)
{
   RF_ASSERT(NULL != linearity_info_ptr);

   if (linearity_info_ptr == NULL) 
   {
	   MSG_FATAL("linearity_info_ptr is NULL",0,0,0);
	   rf_error_flag = TRUE;
	   return NULL;
   }

   RF_ASSERT(linearity_info_ptr->num_gain_steps > 0);
   RF_ASSERT(gain_step_index < linearity_info_ptr->num_gain_steps);

   return &(linearity_info_ptr->switch_pt[gain_step_index]);
}


const rfconfig_rx_lna_switch_pt_type* rfconfig_rx_lna_get_switch_point_info(
   const rfconfig_rx_lna_type            *lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type  mode,
   uint32                                 linearity_index,
   uint32                                 gain_step_index)
{
   const rfconfig_rx_lna_switch_pt_type   *switch_pt_ptr = NULL;
   const rfconfig_rx_lna_linearity_info_type  *linearity_info_ptr = NULL;

   RF_ASSERT(NULL != lna_config_ptr);
   RF_ASSERT(mode < RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES);
   RF_ASSERT(linearity_index < RFCONFIG_RX_LNA_MAX_LINEARITY_STAGES);
   RF_ASSERT(gain_step_index < RFCONFIG_RX_LNA_MAX_GAIN_STEPS);

   if (mode >= RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES)
   {
	   MSG_FATAL("mode invalid",0,0,0);
	   rf_error_flag = TRUE;
	   return NULL;
   }
   if(linearity_index >= RFCONFIG_RX_LNA_MAX_LINEARITY_STAGES)
   {
	   MSG_FATAL("linearity_index invalid",0,0,0);
	   rf_error_flag = TRUE;
	   return NULL;
   }
   linearity_info_ptr = &lna_config_ptr->mode_info[mode].linearity_info[linearity_index];

   if(gain_step_index >= RFCONFIG_RX_LNA_MAX_GAIN_STEPS)
   {
	   MSG_FATAL("gain_step_index invalid",0,0,0);
	   rf_error_flag = TRUE;
	   return NULL;
   }

   if ( lna_config_ptr->mode_info[mode].mode_configured
        && (gain_step_index < linearity_info_ptr->num_gain_steps) )
   {
      switch_pt_ptr = &linearity_info_ptr->switch_pt[gain_step_index];
   }

   return switch_pt_ptr;
}


void rfconfig_rx_lna_set_switch_point_info(
   rfconfig_rx_lna_type                  *lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type  mode,
   uint32                                 linearity_index,
   uint32                                 gain_step_index,
   const rfconfig_rx_lna_switch_pt_type  *switch_pt_ptr)
{
   rfconfig_rx_lna_switch_pt_type   *lhs_switch_pt_ptr = NULL;
   rfconfig_rx_lna_mode_info_type   *mode_info_ptr = NULL;

   RF_ASSERT(NULL != lna_config_ptr);
   RF_ASSERT(mode < RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES);
   RF_ASSERT(linearity_index < RFCONFIG_RX_LNA_MAX_LINEARITY_STAGES);
   RF_ASSERT(gain_step_index < RFCONFIG_RX_LNA_MAX_GAIN_STEPS);
   RF_ASSERT(rfconfig_rx_lna_switch_pt_initialized(switch_pt_ptr));

   if (mode >= RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES)
   {
	   MSG_FATAL("mode invalid",0,0,0);
	   rf_error_flag = TRUE;
	   return;
   }
   if (lna_config_ptr == NULL) 
   {
	   MSG_FATAL("lna_config_ptr is NULL",0,0,0);
	   rf_error_flag = TRUE;
	   return; 
   }

   mode_info_ptr = &lna_config_ptr->mode_info[mode];

   if (linearity_index >= RFCONFIG_RX_LNA_MAX_LINEARITY_STAGES)
   {
	   MSG_FATAL("linearity_index invalid",0,0,0);
	   rf_error_flag = TRUE;
	   return;
   }

   if (gain_step_index >= RFCONFIG_RX_LNA_MAX_GAIN_STEPS)
   {
	   MSG_FATAL("gain_step_index invalid",0,0,0);
	   rf_error_flag = TRUE;
	   return;
   }

   // Check to ensure that there will be no holes after setting this switch point!
   lhs_switch_pt_ptr = &mode_info_ptr->linearity_info[linearity_index].switch_pt[gain_step_index];

   if ( rfconfig_rx_lna_switch_pt_initialized(lhs_switch_pt_ptr) )
   {
      // If this switch point has already been configured, then simply overwrite it
      *lhs_switch_pt_ptr = *switch_pt_ptr;
   }
   else
   {
      // no switch point initialized/available already at the desired location; check that
      //  there won't be hole(s) after setting this switch point!
      if ( gain_step_index == mode_info_ptr->linearity_info[linearity_index].num_gain_steps )
      {
         *lhs_switch_pt_ptr = *switch_pt_ptr;
         mode_info_ptr->linearity_info[linearity_index].num_gain_steps ++;
         // mark this mode as having been initialized (with at least one valid switch point)
         mode_info_ptr->mode_configured = TRUE;
      }
      else
      {
         ERR_FATAL(
            "rfconfig_rx_lna_set_switch_point_info() - hole(s) discovered: next_avail_index(%d) desired_index(%d)",
            mode_info_ptr->linearity_info[linearity_index].num_gain_steps, 
            gain_step_index,
            0);
      }
   }
}


/* ------- START of functions not being used now -------

static void rfconfig_rx_lna_set_linearity_info(
   rfconfig_rx_lna_type                         *lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type         mode,
   uint32                                        linearity_index,
   const rfconfig_rx_lna_linearity_info_type    *linearity_info_ptr)
{
   uint32                                switch_pt_index = 0;
   const rfconfig_rx_lna_switch_pt_type *curr_switch_pt_ptr = NULL;
   rfconfig_rx_lna_linearity_info_type  *lhs_linearity_info_ptr = NULL;

   RF_ASSERT(NULL != lna_config_ptr);
   RF_ASSERT(mode < RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES);
   RF_ASSERT(linearity_index < RFCONFIG_RX_LNA_MAX_LINEARITY_STAGES);
   RF_ASSERT(NULL != linearity_info_ptr);
   RF_ASSERT(linearity_info_ptr->num_gain_steps > 0);

   lhs_linearity_info_ptr = &lna_config_ptr->mode_info[mode].linearity_info[linearity_index];

   for (
      switch_pt_index = 0;
      switch_pt_index < linearity_info_ptr->num_gain_steps;
      switch_pt_index ++)
   {
      curr_switch_pt_ptr = &linearity_info_ptr->switch_pt[switch_pt_index];
      RF_ASSERT( rfconfig_rx_lna_switch_pt_initialized(curr_switch_pt_ptr) );
      lhs_linearity_info_ptr->switch_pt[switch_pt_index] = *curr_switch_pt_ptr;
   }
   lhs_linearity_info_ptr->num_gain_steps = linearity_info_ptr->num_gain_steps;
   lna_config_ptr->mode_info[mode].mode_configured = TRUE;
}


static void rfconfig_rx_lna_set_mode_info(
   rfconfig_rx_lna_type                         *lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type         mode,
   const rfconfig_rx_lna_mode_info_type         *mode_info_ptr)
{
   RF_ASSERT(NULL != lna_config_ptr);
   RF_ASSERT(mode < RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES);
   RF_ASSERT(NULL != mode_info_ptr);
   RF_ASSERT(mode_info_ptr->mode_configured);

   lna_config_ptr->mode_info[mode] = *mode_info_ptr;
}


static const rfconfig_rx_lna_mode_info_type* rfconfig_rx_lna_get_mode_info(
   const rfconfig_rx_lna_type                   *lna_config_ptr,
   rfconfig_rx_lna_operational_mode_type         mode)
{
   const rfconfig_rx_lna_mode_info_type *mode_info_ptr = NULL;

   RF_ASSERT(NULL != lna_config_ptr);
   RF_ASSERT(mode < RFCONFIG_RX_LNA_OPERATIONAL_MODE__NUM_VALID_MODES);

   if ( lna_config_ptr->mode_info[mode].mode_configured )
   {
      mode_info_ptr = &lna_config_ptr->mode_info[mode];
   }

   return mode_info_ptr;
}

   ------- END of functions not being used now ------- */


#endif /* RF_HAS_RFCONFIG_RX_LNA */


