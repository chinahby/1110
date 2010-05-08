/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=

                               T M   D A T A
                           TCXO Manager NV and RGS data

GENERAL DESCRIPTION

  This module maintains the Recent Good System values, and VCO-Temperature
  tables, for proper TCXO operation.

EXTERNALIZED FUNCTIONS


REGIONAL FUNCTIONS

  None


INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2006 - 2009 by QUALCOMM, Inc.  All Rights Reserved.

=*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=



==============================================================================

                           EDIT HISTORY FOR MODULE

$Header: //source/qcom/qct/modem/mcs/tcxomgr/main/latest/src/tcxomgr_data.c#14 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/30/09   ns       Added temperature check for using old rgs info
04/21/09   sst      Update XO targets to use temp.xo, check for temp table sat
12/10/08   sb       Modify tcxomgr_get_uncertainty() to check in RGS is more
                    than 30 seconds old.
12/05/08   sst      Update for QSC11x0 to us XO therm
10/20/08   adw      Replaced FEATURE_XO_DIAG w/ FEATURE_TCXOMGR_DIAG.
09/15/08   sg       Fix lint/klocwork warnings
09/15/08   sst      Correctly featurize for XO_TASK
08/28/08   adw      Added FEATURE_XO_DIAG to featurize for ULC.
07/11/08   scm      Support new RF temperature API: rfm_adc_read().
                      Implement tcxomgr_reset_rgs_and_temp_table().q
07/08/08   bb       Remove hard coded trim current value for QSC1100 target
06/20/08   mca      Merge from //depot
04/10/08   sg       Guard against getting temp before adc is running
03/14/08   sst      Temporarily disable sleep and QPCH for QSC1100
03/03/08   va       For XO - Adjust rot in RGS by freq-temp table for temp
11/16/07   mca      Fixed compiler warnings
11/14/07   sst      Properly set nv_fac_data->ft_qual_ind in cal_data_write
11/06/07   cab      Remove "double translation" of temp to index in update tbl
10/30/07   cab      If a trk_lo vs. temperature entry is empty, search +/-3
                    indexes for a trk_lo value, instead of +/-1.
10/18/07   sst      Updated calculations for nominal C2/C3
10/16/07   sst      Move XO Trim NV functions to tcxomgr_data.c
10/10/07   va       Do not allow sleep when reading from XO ADC
09/25/07   cab      Move field cal to tcxomgr task
                    Allow client to call update rgs in quick succession
09/06/07   sst      Sync enum of ft_qual btw NV and TCXOMGR
                    Lint fixes
09/05/07   cab      Cache therm read due to timeline impacts
08/13/07   sst      Add missing return in tcxomgr_get_temp_table
08/03/07   sst      Correct XO Fac Cal NV read fail check
08/02/07   va       PMIC temp should not be divided by 10.
07/26/07   sst      Change get_time_and_temp to use temp_set_type
                    Call tcxomgr_field_switch_xo_trim on power-up
07/10/07   va       Updates to tcxomgr_nv_field_cal_data_write.
07/09/07   sst      Use current task for NV read/writes
                    Remove NV fac cal featurizations
06/29/07   cab      Added tcxomgr_field_cal
06/18/07   aps      Added factory cal NV read/write functions
06/18/07    va      Macros for reading temperature
06/14/07   cab      Adding back in factory cal
06/13/07   cab      Reverted factory cal, removed check for controlling
                    client id in write_rgs_values
06/08/07   sst      Moved tcxomgr_get_temp from _cal.c
06/06/07   sst      Added xo_trim NV read/write functions
05/17/07   cab      Added uncertainty for passive clients
05/11/07   cab      Get rgs value only when it is valid and time is valid
03/02/07   nnv      Added code not to look rotator upper limit while updating
                    temp table upon storing rgs data and to use rotator data
                    for frequency adjustment in XO targets.
01/19/07   cab      Removed featurization of xo cal nv items (always present)
                    Fixed compiler warning for unsigned comparison (>=) 0
11/14/06   cab      Fix to return RGS value if updated in last second
10/12/06   cab      Removed TCXOMGR_RGS_NONE
09/29/06   cab      Initial version

============================================================================*/



/*============================================================================

                           INCLUDE FILES FOR MODULE

============================================================================*/
#include "customer.h"
#include "adc.h"
#include "clk.h"
#include "err.h"
#include "therm.h"
#include "task.h"

#include "tcxomgr_i.h"
#include "tcxomgr_task.h"

#ifdef FEATURE_TCXOMGR_DIAG
#include "tcxomgr_diag.h"
#endif /* FEATURE_TCXOMGR_DIAG */

#ifdef FEATURE_XO
#ifdef FEATURE_XO_FACTORY_CAL
#include "tcxomgr_cal.h"
#endif /* FEATURE_XO_FACTORY_CAL */
#include "tcxomgr_xo_trim.h"
#endif /* FEATURE_XO */

#ifdef FEATURE_TCXOMGR_USE_RF_TEMPERATURE_API
#error code not present
#endif /* FEATURE_TCXOMGR_USE_RF_TEMPERATURE_API */



/*---------------------------------------------------------------------------
  TCXO Manager local variables 
---------------------------------------------------------------------------*/

extern tcxomgr_client_type clients[];

/*============================================================================

                   Defines

============================================================================*/

#define TCXOMGR_MAX_CORRECTION          922      /* 0.9 ppm in 2^-10ppm units */

/* If RGS < 30 secs old, return RGS_RECENT */
#define TCXOMGR_RGS_RECENT_THRESHOLD    30

/*============================================================================

                            FUNCTION DECLARATIONS

============================================================================*/

/*============================================================================

FUNCTION TCXOMGR_NV_INIT

DESCRIPTION
  This function initializes the NV member of the TCXO manager

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_nv_init
(
  /* Calling task's TCB pointer */
  rex_tcb_type *tcb_ptr,

  /* Signal to wait for, for NV reading */
  rex_sigs_type sig,

  /* Function to call to wait for NV reading to complete, eg) rex_wait */
  void (*wait)(rex_sigs_type)
)
{

/*--------------------------------------------------------------------------*/

  /* Initialize NV command buffer */
  tcxomgr.nv.cmd.data_ptr   = &tcxomgr.nv.item;
  tcxomgr.nv.cmd.tcb_ptr    = tcb_ptr;
  tcxomgr.nv.cmd.sigs       = sig;
  tcxomgr.nv.cmd.done_q_ptr = NULL;

  /* Remember the wait function */
  tcxomgr.nv.wait = wait;

} /* nv_init */


/*============================================================================

FUNCTION TCXOMGR_NV_CMD

DESCRIPTION
  This function reads/write an item from/to the NV into tcxomgr.nv.item

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

nv_stat_enum_type tcxomgr_nv_cmd
(
  /* READ, WRITE, PEEK, POKE, etc. */
  nv_func_enum_type cmd,

  /* Item ID to read/write from/to NV */
  nv_items_enum_type id
)
{

/*--------------------------------------------------------------------------*/

  /* Setup NV command buffer to perform the required operation on the item */
  tcxomgr.nv.cmd.cmd     = cmd;

  tcxomgr.nv.cmd.item    = id;
  tcxomgr.nv.cmd.tcb_ptr = rex_self();

  /* Ensure the signal we are about to wait on is not already set */
  (void) rex_clr_sigs( tcxomgr.nv.cmd.tcb_ptr, tcxomgr.nv.cmd.sigs );

  /* Command the NV task to process the command */
  nv_cmd( &tcxomgr.nv.cmd );

  /* Wait for NV task to set the signal indicating operation is complete */
  tcxomgr.nv.wait( tcxomgr.nv.cmd.sigs );

  /* Clear the signal we waited on */
  (void) rex_clr_sigs( tcxomgr.nv.cmd.tcb_ptr, tcxomgr.nv.cmd.sigs );

  if ( tcxomgr.nv.cmd.status != NV_DONE_S )
  {
    DBG_3( HIGH, "NV cmd %d item %d status %d", cmd, id,
          tcxomgr.nv.cmd.status );
  }

  return tcxomgr.nv.cmd.status;

} /* tcxomgr_nv_cmd */


#ifdef FEATURE_XO
/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_READ

DESCRIPTION
  This function reads the XO Trim/Capacitor/Coarse Rotator values from NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  XO Trim will be initialized

============================================================================*/

void tcxomgr_xo_trim_read( void )
{

  uint8 pm_xo_trim_val;

/*--------------------------------------------------------------------------*/

  /* Initialize values, so "else { ... }" clauses are not needed */
  tcxomgr.xo_trim.curr_val        = XO_TRIM_DEFAULT_VALUE;
  tcxomgr.xo_trim.table_val       = XO_TRIM_DEFAULT_VALUE;
  tcxomgr.xo_trim.switched        = FALSE;

  /* Attempt to read the curr and table xo_trim values from NV */
  #ifdef T_QSC60X5
  if ( tcxomgr_nv_cmd( NV_READ_F, NV_XO_TRIM_VALUES_I ) == NV_DONE_S )
  #else
#error code not present
  #endif
  {
    tcxomgr.xo_trim.curr_val  = (uint8)tcxomgr.nv.item.xo_trim_values.current;
    tcxomgr.xo_trim.table_val = (uint8)tcxomgr.nv.item.xo_trim_values.table;
  }

  /* get PMIC hardware value */
  pm_xo_trim_val = pm_xo_get_xo_trim();

  /* ensure the cap value from the tcxomgr is the same as that which is in
     the PMIC */
  if ( pm_xo_trim_val != tcxomgr.xo_trim.curr_val )
  {
    /* send the request for the new xo_trim value to the PMIC */
    (void) pm_xo_set_xo_trim( tcxomgr.xo_trim.curr_val );
  }

  DBG_3( MED, "xo_trim_read: curr = %8d  tbl = %8d, pmic = %8d",
        tcxomgr.xo_trim.curr_val, tcxomgr.xo_trim.table_val, pm_xo_trim_val );

} /* tcxomgr_xo_trim_read( ) */
#endif /* FEATURE_XO */


#ifdef FEATURE_XO
/*============================================================================

FUNCTION TCXOMGR_XO_TRIM_WRITE

DESCRIPTION
  This function writes the XO Trim/Capacitor/Coarse Rotator values to NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/

void tcxomgr_xo_trim_write( void )
{
/*--------------------------------------------------------------------------*/

  /* get PMIC hardware value */
  tcxomgr.xo_trim.curr_val = pm_xo_get_xo_trim();

  /* Attempt to write the current and table xo_trim value */
  tcxomgr.nv.item.xo_trim_values.current = (uint8)tcxomgr.xo_trim.curr_val;
  tcxomgr.nv.item.xo_trim_values.table   = (uint8)tcxomgr.xo_trim.table_val;

  #ifdef T_QSC60X5
  (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_XO_TRIM_VALUES_I );
  #else
#error code not present
  #endif

  DBG_2( MED, "xo_trim_write: curr = %8d, tbl = %8d",
         tcxomgr.xo_trim.curr_val, tcxomgr.xo_trim.table_val );

} /* tcxomgr_xo_trim_write( ) */
#endif /* FEATURE_XO */


#ifdef FEATURE_XO_FACTORY_CAL
/*============================================================================

FUNCTION TCXOMGR_NV_CAL_DATA_READ

DESCRIPTION
  This function reads the calibration data from NV.

  If NV read of factory cal data fails or if ft_qual of factory cal data is
  FAIL, use defaults. If field cal data read fails, load the factory cal
  data into field cal structure.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_nv_cal_data_read( void )
{
  nv_xo_cal_fac_data_type *nv_fac_data     =
    &(tcxomgr.nv.item.xo_factory_calibration);
  nv_xo_cal_field_data_type *nv_field_data =
    &(tcxomgr.nv.item.xo_field_calibration);
  tcxomgr_cal_fac_data_type   *fac_data    = &(tcxomgr.cal_data.fac_data);
  tcxomgr_cal_field_data_type *field_data  = &(tcxomgr.cal_data.field_data);

/*--------------------------------------------------------------------------*/

  /* read in conversion factors for calibration coefficients */
  if ( tcxomgr_nv_cmd ( NV_READ_F, NV_XO_CAL_CONV_I ) == NV_DONE_S )
  {
    tcxomgr.cal_data.conv.c1toc2_0 = tcxomgr.nv.item.xo_cal_conv.c1toc2_0;
    tcxomgr.cal_data.conv.c1toc2_1 = tcxomgr.nv.item.xo_cal_conv.c1toc2_1;
    tcxomgr.cal_data.conv.c1toc3_0 = tcxomgr.nv.item.xo_cal_conv.c1toc3_0;
    tcxomgr.cal_data.conv.c1toc3_1 = tcxomgr.nv.item.xo_cal_conv.c1toc3_1;
  }
  else
  {
    DBG_0( ERROR,
           "XO Cal Conv factors read from NV failed, loading default values");

    /* set default values */
    tcxomgr.cal_data.conv.c1toc2_0 = CAL_NV_DEFAULT_VAL_D20;
    tcxomgr.cal_data.conv.c1toc2_1 = CAL_NV_DEFAULT_VAL_D21;
    tcxomgr.cal_data.conv.c1toc3_0 = CAL_NV_DEFAULT_VAL_D30;
    tcxomgr.cal_data.conv.c1toc3_1 = CAL_NV_DEFAULT_VAL_D31;
  }

  /* read in factory calibration coefficients */
  if (( tcxomgr_nv_cmd ( NV_READ_F, NV_XO_FACTORY_CALIBRATION_I ) == NV_DONE_S ) &&
      ( nv_fac_data->ft_qual_ind != TCXOMGR_CAL_FT_QUAL_FAILED ))
  {
    fac_data->c3          = nv_fac_data->c3;
    fac_data->c2          = nv_fac_data->c2;
    fac_data->c1          = nv_fac_data->c1;
    fac_data->c0          = nv_fac_data->c0;
    fac_data->t0          = nv_fac_data->t0;
    fac_data->t0p         = nv_fac_data->t0p;
    fac_data->p           = nv_fac_data->p;
    fac_data->xo_trim     = nv_fac_data->xo_trim;
    CAL_FT_QUAL_FROM_NV( nv_fac_data->ft_qual_ind, fac_data->ft_qual_ind );
  }
  else /* Use defaults if read fails */
  {
    DBG_0(ERROR,
          "XO Factory calibration data read from NV failed, loading defaults");

    /* Load default values */
    fac_data->t0          = CAL_NV_DEFAULT_VAL_T0;
    fac_data->t0p         = CAL_NV_DEFAULT_VAL_T0P;
    fac_data->p           = CAL_NV_DEFAULT_VAL_P;
    fac_data->c0          = CAL_NV_DEFAULT_VAL_C0;
    fac_data->c1          = CAL_NV_DEFAULT_VAL_C1;
    tcxomgr_cal_calc_c2( fac_data);
    tcxomgr_cal_calc_c3( fac_data );
    (void) tcxomgr_xo_trim_get_val( &(fac_data->xo_trim) );
    fac_data->ft_qual_ind = TCXOMGR_CAL_FT_QUAL_NOT_CAL;
  }

  /* read in field calibration coefficients */
  if ( tcxomgr_nv_cmd ( NV_READ_F, NV_XO_FIELD_CALIBRATION_I ) == NV_DONE_S )
  {
    field_data->c3            = nv_field_data->c3;
    field_data->c2            = nv_field_data->c2;
    field_data->c1            = nv_field_data->c1;
    field_data->c0            = nv_field_data->c0;
    field_data->t0            = nv_field_data->t0;
    field_data->t0p           = nv_field_data->t0p;
    field_data->p             = nv_field_data->p;
    field_data->timestamp     = nv_field_data->timestamp;
    field_data->xo_trim       = nv_field_data->xo_trim;
    field_data->ft_sample_cnt = nv_field_data->ft_sample_cnt;
    field_data->state         = (tcxomgr_field_state_type)nv_field_data->state;
    CAL_FT_QUAL_FROM_NV( nv_field_data->ft_qual_ind, field_data->ft_qual_ind );

  }
  else
  {
    DBG_0(ERROR,
          "XO Field calibration data read from NV failed, loading defaults");

    /* copy factory values on failed read and initialize rest of fields to
       their default values */
    tcxomgr_cal_copy_fac_to_field();
  }

  /* check if the C0 error needs to be pulled in using the XO trim */
  tcxomgr_field_switch_xo_trim();

} /* tcxomgr_nv_cal_data_read */


/*============================================================================

FUNCTION TCXOMGR_NV_CAL_DATA_WRITE

DESCRIPTION
  This function writes xo calibration data values to NV.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_nv_cal_data_write( void )
{

  nv_xo_cal_fac_data_type *nv_fac_data     =
    &(tcxomgr.nv.item.xo_factory_calibration);
  nv_xo_cal_field_data_type *nv_field_data =
    &(tcxomgr.nv.item.xo_field_calibration);
  tcxomgr_cal_fac_data_type   *fac_data    = &(tcxomgr.cal_data.fac_data);
  tcxomgr_cal_field_data_type *field_data  = &(tcxomgr.cal_data.field_data);

/*--------------------------------------------------------------------------*/

  /* Fill in cal conversion values */
  tcxomgr.nv.item.xo_cal_conv.c1toc2_0 = tcxomgr.cal_data.conv.c1toc2_0;
  tcxomgr.nv.item.xo_cal_conv.c1toc2_1 = tcxomgr.cal_data.conv.c1toc2_1;
  tcxomgr.nv.item.xo_cal_conv.c1toc3_0 = tcxomgr.cal_data.conv.c1toc3_0;
  tcxomgr.nv.item.xo_cal_conv.c1toc3_1 = tcxomgr.cal_data.conv.c1toc3_1;

  /* Attempt to write cal conversion values */
  (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_XO_CAL_CONV_I );


  /* Fill in field calibration coefficients */
  nv_field_data->c3                    = field_data->c3;
  nv_field_data->c2                    = field_data->c2;
  nv_field_data->c1                    = field_data->c1;
  nv_field_data->c0                    = field_data->c0;
  nv_field_data->t0                    = field_data->t0;
  nv_field_data->t0p                   = field_data->t0p;
  nv_field_data->p                     = field_data->p;
  nv_field_data->timestamp             = field_data->timestamp;
  nv_field_data->xo_trim               = field_data->xo_trim;
  nv_field_data->ft_sample_cnt         = field_data->ft_sample_cnt;
  nv_field_data->state                 = field_data->state;
  CAL_FT_QUAL_TO_NV( field_data->ft_qual_ind, nv_field_data->ft_qual_ind );

  /* Attempt to write field calibration values */
  (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_XO_FIELD_CALIBRATION_I );

  /* Fill in factory calibration coefficients */
  nv_fac_data->c3                      = fac_data->c3;
  nv_fac_data->c2                      = fac_data->c2;
  nv_fac_data->c1                      = fac_data->c1;
  nv_fac_data->c0                      = fac_data->c0;
  nv_fac_data->t0                      = fac_data->t0;
  nv_fac_data->t0p                     = fac_data->t0p;
  nv_fac_data->p                       = fac_data->p;
  nv_fac_data->xo_trim                 = fac_data->xo_trim;
  CAL_FT_QUAL_TO_NV( fac_data->ft_qual_ind, nv_fac_data->ft_qual_ind );

  /* Attempt to write factory calibration values */
  (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_XO_FACTORY_CALIBRATION_I );

} /* tcxomgr_nv_cal_data_write */


/*============================================================================

FUNCTION TCXOMGR_NV_FIELD_CAL_DATA_WRITE

DESCRIPTION
  This function writes the field calibration coefficients to NV.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_nv_field_cal_data_write( void )
{
  nv_xo_cal_field_data_type   *nv_field_data =
    &(tcxomgr.nv.item.xo_field_calibration);
  tcxomgr_cal_field_data_type *field_data    = &(tcxomgr.cal_data.field_data);

/*--------------------------------------------------------------------------*/

  /* Fill in field calibration coefficients */
  nv_field_data->c3            = field_data->c3;
  nv_field_data->c2            = field_data->c2;
  nv_field_data->c1            = field_data->c1;
  nv_field_data->c0            = field_data->c0;
  nv_field_data->t0            = field_data->t0;
  nv_field_data->t0p           = field_data->t0p;
  nv_field_data->p             = field_data->p;
  nv_field_data->timestamp     = field_data->timestamp;
  nv_field_data->xo_trim       = field_data->xo_trim;
  nv_field_data->ft_sample_cnt = field_data->ft_sample_cnt;
  nv_field_data->state         = field_data->state;
  CAL_FT_QUAL_TO_NV( field_data->ft_qual_ind, nv_field_data->ft_qual_ind );

  /* Attempt to write field calibration values */
  (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_XO_FIELD_CALIBRATION_I );

} /* tcxomgr_nv_field_cal_data_write */

#endif /* FEATURE_XO_FACTORY_CAL */


/*============================================================================

FUNCTION TCXOMGR_NV_VCO_READ

DESCRIPTION
  This function reads the VCO data from NV.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_nv_vco_read( void )
{
  /* Loop index */
  int i;

/*--------------------------------------------------------------------------*/

  /* Initialize values, so "else { ... }" clauses are not needed */

  for ( i = 0; i < VCO_TEMP_TABLE_SIZE; i++ )
  {
    tcxomgr.vco.temp[ i ] = VCO_UNDEFINED;
  }

  tcxomgr.vco.dflt        = VCO_DEFAULT;
  tcxomgr.vco.slope       = VCO_SLOPE_DEFAULT;
  tcxomgr.vco.slope_range = VCO_SLOPE_RANGE_DEFAULT;

  /* Now read values from NV, if possible */
  #ifdef FEATURE_TCXOMGR_DIAG
  if ( !tcxomgr_diag_get_temp_table_override_enable() )
  #endif /* FEATURE_TCXOMGR_DIAG */
  {
    /* Attempt to read temperature-frequency table */
    if ( tcxomgr_nv_cmd( NV_READ_F, NV_VCO_TEMP_I ) == NV_DONE_S )
    {
      for ( i = 0; i < VCO_TEMP_TABLE_SIZE; i++ )
      {
        tcxomgr.vco.temp[ i ] = tcxomgr.nv.item.vco_temp[ i ];
      }
    }
  }

  #ifdef FEATURE_TCXOMGR_DIAG
  if ( !tcxomgr_diag_get_vco_dflt_override_enable() )
  #endif /* FEATURE_TCXOMGR_DIAG */
  {
    /* Attempt to read default VCO value, for undefined table entries */
    if ( tcxomgr_nv_cmd( NV_READ_F, NV_VCO_DEFAULT_I ) == NV_DONE_S )
    {
      tcxomgr.vco.dflt = tcxomgr.nv.item.vco_default;
    }
  }

  /* Attempt to read VCO Slope value */
  #ifdef FEATURE_TCXOMGR_DIAG
  if ( !tcxomgr_diag_get_vco_slope_override_enable() )
  #endif /* FEATURE_TCXOMGR_DIAG */
  {
    if ( tcxomgr_nv_cmd( NV_READ_F, NV_VCO_SLOPE_I ) == NV_DONE_S )
    {
      tcxomgr.vco.slope = tcxomgr.nv.item.vco_slope;
    }
  }

  /* Set the update threshold to .5ppm or 2^20 / NV_VCO_SLOPE / 2 */
  tcxomgr.vco_temp_update_thresh =
    TCXOMGR_GEN_UPDATE_THRESH( tcxomgr.vco.slope );

  /* Set the initial threshold to 5ppm or 5 * 2^20 / NV_VCO_SLOPE */
  tcxomgr.vco_temp_initial_thresh =
    TCXOMGR_GEN_INITIAL_THRESH( tcxomgr.vco.slope );

  /* Attempt to read VCO Slope Range value */
  #ifdef FEATURE_TCXOMGR_DIAG
  if ( !tcxomgr_diag_get_slope_range_override_enable() )
  #endif /* FEATURE_TCXOMGR_DIAG */
  {
    if ( tcxomgr_nv_cmd( NV_READ_F, NV_VCO_SLOPE_RANGE_I  ) == NV_DONE_S )
    {
      tcxomgr.vco.slope_range = tcxomgr.nv.item.vco_slope_range;
    }
  }

  /* Use the moment we loaded the table as the last update to the table. */
  tcxomgr.vco.last_update_secs = clk_read_secs();

} /* tcxomgr_nv_vco_read */


/*============================================================================

FUNCTION TCXOMGR_NV_VCO_WRITE

DESCRIPTION
  This function writes the frequency-temperature table to NV.

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_nv_vco_write( void )
{
  /* Loop index */
  int i;

/*--------------------------------------------------------------------------*/

  #ifdef FEATURE_TCXOMGR_DIAG
  /* Don't store values passed in from DIAG */
  if ( tcxomgr_diag_get_temp_table_override_enable() )
  {
    return;
  }
  #endif /* FEATURE_TCXOMGR_DIAG */

  for ( i = 0;  i < VCO_TEMP_TABLE_SIZE; i++ )
  {
    tcxomgr.nv.item.vco_temp[ i ] = tcxomgr.vco.temp[ i ];
  }

  /* Attempt to write VCO temperature-frequency table */
  (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_VCO_TEMP_I );

} /* tcxomgr_nv_vco_write */


/*============================================================================

FUNCTION TCXOMGR_NV_RGS_READ

DESCRIPTION
  This function reads the RGS (recent good system) values from NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_nv_rgs_read( void )
{

/*--------------------------------------------------------------------------*/

  #ifdef FEATURE_TCXOMGR_DIAG
  if ( tcxomgr_diag_get_rgs_override_enable() )
  {
    /* Do Nothing because the data has already been filled in */
    return;
  }
  #endif /* FEATURE_TCXOMGR_DIAG */

  /* Initialize values, so "else { ... }" clauses are not needed */

  tcxomgr.rgs.system    = TCXOMGR_CLIENT_NONE;
  tcxomgr.rgs.vco       = tcxomgr.vco.dflt;
  tcxomgr.rgs.rot       = 0;
  tcxomgr.rgs.temp.pa   = TCXOMGR_INVALID_TEMP;
  tcxomgr.rgs.temp.pmic = TCXOMGR_INVALID_TEMP;
  tcxomgr.rgs.temp.xo   = TCXOMGR_INVALID_TEMP;
  tcxomgr.rgs.time      = 0;


  /* Now read values from NV, if possible */

  /* Attempt to read last RGS system type */
  if ( tcxomgr_nv_cmd( NV_READ_F, NV_RGS_TYPE_I ) == NV_DONE_S )
  {
    tcxomgr.rgs.system = (tcxomgr_client_id_type)tcxomgr.nv.item.rgs_type;
  }

  /* Attempt to read last RGS update time */
  if ( tcxomgr_nv_cmd( NV_READ_F, NV_RGS_TIME_I ) == NV_DONE_S )
  {
    tcxomgr.rgs.time = tcxomgr.nv.item.rgs_time;
  }

  /* Attempt to read last RGS update temperature */
  if ( tcxomgr_nv_cmd( NV_READ_F, NV_RGS_TEMP_I ) == NV_DONE_S )
  {
    #ifndef FEATURE_XO
    tcxomgr.rgs.temp.pa = tcxomgr.nv.item.rgs_temp;
    #else
    tcxomgr.rgs.temp.xo = tcxomgr.nv.item.rgs_temp;
    #endif
  }

  /* Attempt to read last RGS VCTCXO accumulator value */
  if ( tcxomgr_nv_cmd( NV_READ_F, NV_RGS_VCO_I ) == NV_DONE_S )
  {
    tcxomgr.rgs.vco = tcxomgr.nv.item.rgs_vco;
  }

  /* Attempt to read last RGS rotator accumulator value */
  if ( tcxomgr_nv_cmd( NV_READ_F, NV_RGS_ROT_I ) == NV_DONE_S )
  {
    tcxomgr.rgs.rot = tcxomgr.nv.item.rgs_rot;
  }

} /* tcxomgr_nv_rgs_read */


/*============================================================================

FUNCTION TCXOMGR_NV_RGS_WRITE

DESCRIPTION
  This function writes the RGS (recent good system) values to NV

DEPENDENCIES
  tcxomgr.nv

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_nv_rgs_write( void )
{

/*--------------------------------------------------------------------------*/

  /* Attempt to write last RGS system type */
  tcxomgr.nv.item.rgs_type = (uint8)tcxomgr.rgs.system;
  (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_TYPE_I );

  /* Attempt to write last RGS update time */
  tcxomgr.nv.item.rgs_time = tcxomgr.rgs.time;
  (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_TIME_I );

  /* Attempt to write last RGS update temperature */
  #ifndef FEATURE_XO
  tcxomgr.nv.item.rgs_temp = (uint8) tcxomgr.rgs.temp.pa;
  #else
  tcxomgr.nv.item.rgs_temp = (uint8) tcxomgr.rgs.temp.xo;
  #endif
  (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_TEMP_I );

  /* Attempt to write last RGS VCTCXO accumulator value */
  tcxomgr.nv.item.rgs_vco = tcxomgr.rgs.vco;
  (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_VCO_I );

  /* Attempt to write last RGS rotator accumulator value */
  tcxomgr.nv.item.rgs_rot = tcxomgr.rgs.rot;
  (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_ROT_I );

} /* tcxomgr_nv_rgs_write */


/*============================================================================

FUNCTION TCXOMGR_GET_VCO_SLOPE

DESCRIPTION


DEPENDENCIES


RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_get_vco_slope
(
  /* vco slope */
  uint16 *slope,

  /* vco range */
  uint8 *range
)
{

  if (slope != NULL)
  {
    *slope = tcxomgr.vco.slope;
  }

  if (range != NULL)
  {
    *range = tcxomgr.vco.slope_range;
  }

} /* tcxomgr_get_vco_slope */



/*============================================================================

FUNCTION TCXOMGR_GET_TEMP

DESCRIPTION
  This function reads the thermistors for the XO, PMIC, and PA

DEPENDENCIES
  None

RETURN VALUE
  temperature of XO, PMIC, and PA (in 2^-10 deg C)

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_temp_set_type tcxomgr_get_temp( void )
{
  /* temperature return */
  tcxomgr_temp_set_type      temp = { TCXOMGR_INVALID_TEMP,
                                      TCXOMGR_INVALID_TEMP,
                                      TCXOMGR_INVALID_TEMP };  

  #ifdef FEATURE_TCXOMGR_USE_RF_TEMPERATURE_API
#error code not present
  #endif

  /* --------------------------------------------------------------------- */

  #ifdef FEATURE_XO
  /* This block of code does not work properly if the
   * xotherm_adc_pwr has not been enabled, but I can't find a good
   * way to query this.  */
  extern boolean xotherm_adc_pwr_on;

  if ( xotherm_adc_pwr_on ) 
  {
    #ifdef FEATURE_XO_TASK
    /* Sleeping stops TCXO clock, hence all clocks. */
    sleep_negate_okts(tcxomgr_sleep_okts_handle);
    #endif

    CALIBRATE_XO_THERM_ADC();
    CALIBRATE_XO_THERM_GAIN();
    temp.xo    = (tcxomgr_temp_type)( ADC_READ_XO_THERM() );
    temp.pmic  = (tcxomgr_temp_type)( ADC_READ_PM_THERM() );
    temp.pa    = (tcxomgr_temp_type)( ADC_READ_PA_THERM() );

    /* Revisit - store cached values for timeline critical code */
    tcxomgr.cached_xo_temp = temp;
    tcxomgr.cached_xo_temp_time = clk_read_secs();

    #ifdef FEATURE_XO_TASK
    sleep_assert_okts(tcxomgr_sleep_okts_handle);
    #endif
  } 

  return( temp );

  #else  /* !FEATURE_XO */

  #ifdef FEATURE_TCXOMGR_USE_RF_TEMPERATURE_API
#error code not present
  #else  /* #ifdef FEATURE_TCXOMGR_USE_RF_TEMPERATURE_API */

  temp.pa = (tcxomgr_temp_type)( therm_read() );

  #endif  /* #ifdef FEATURE_TCXOMGR_USE_RF_TEMPERATURE_API */

  return( temp );

  #endif /* FEATURE_XO */

} /* tcxomgr_get_temp */


/*============================================================================

FUNCTION TCXOMGR_GET_TIME_AND_TEMP

DESCRIPTION
  Returns the temperature set and the current time
  Note: temp.pa gives the equivilant functionality as the previous
        implementation

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_get_time_and_temp
(
  uint32                *time,
  tcxomgr_temp_set_type *temp
)
{
  #ifndef FEATURE_XO_FACTORY_CAL
  static uint32                 thermistor_timestamp = 0;
  static tcxomgr_temp_set_type  thermistor_value     = {0,0,0};
  #endif

/*--------------------------------------------------------------------------*/

  *time = clk_read_secs();

  #ifndef FEATURE_XO_FACTORY_CAL

  /* Only read the temp, if tcxomgr has not read it lately */
  if ( ( *time - thermistor_timestamp ) > TCXOMGR_READ_THERM_SEC )
  {
    *temp = tcxomgr_get_temp();
    thermistor_value = *temp;

    /* Only update temperature timestamp if we actually know current temp */
    #ifndef FEATURE_XO
    if ( temp->pa != TCXOMGR_INVALID_TEMP )
    #else
    if ( temp->xo != TCXOMGR_INVALID_TEMP )
    #endif
    {
      thermistor_timestamp = *time;
    }
  }
  else
  {
    *temp = thermistor_value;
  }

  #else /* FEATURE_XO_FACTORY_CAL */

  /*******************************************************
  Revisit

  If the thermisstors have been read fairly recently, then
  we don't need to read these again.
  As of now, since we know that the XO Compensation algorithm
  is executing evenry 100 ms, we should alwaya have a fairly
  recent time stamp.

  If the compensation algorithm doesn't execute - for whatever reason
  including a potential future design change - then the error message
  below will start to show up on F3 and force us to do something
  different.

  It should also be noted that for most of the usage of this function
  it doesn't quite matter too much if the thermistor read is as old
  as 30 seconds or so. The only reason the "freshness" matters is because
  at one place, the return value of this function is used for XO Field CAL.
  This requires the temp values to be fairly recent.

  However - there is no good reason why Field cal could not be done at a
  different place. Notably, the TCXOMGR thread can execute the field cal
  functionality as a potential future change.

  Another possibility is to use a coarse temperature reading using
  only SYNC filter of the XO ADC - which will require only 1 ms wait.
  *****************************************************************/

  /* if it has been longer than 1 second since therm read */
  if ( ( (*time) - tcxomgr.cached_xo_temp_time ) > 1 )
  {
    DBG_2( ERROR,"Stale Therm. Read Ts %d Curr Ts %d",
           tcxomgr.cached_xo_temp_time, (*time) );
  }

  *temp = tcxomgr.cached_xo_temp;

  #endif /* !FEATURE_XO_FACTORY_CAL */

} /* tcxomgr_get_time_and_temp */


/*============================================================================

FUNCTION TCXOMGR_GET_TEMP_TABLE

DESCRIPTION
  This function retrieves the given temperature's index value.
  Index returned will be in the VCO_TEMP table domain [0, 63]

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
static tcxomgr_vco_info_type tcxomgr_get_temp_table
(
  /* temperature at which to find a vco value */
  uint8 temp
)
{
  /* Interpolation point at cooler temperatures. */
  tcxomgr_vco_type                vco_low;

  /* Interpolation point at hotter temperatures. */
  tcxomgr_vco_type                vco_high;

  /* Interpolation point, distance from current index. */
  uint16                          index_count;

  /* Return value */
  tcxomgr_vco_info_type vco;

/*--------------------------------------------------------------------------*/

  if ( !VALID_TEMP( temp ) )
  {
    DBG_1( ERROR, "Temp Table Req, Sat or Inv Temp (%d)", temp );
    vco.vco_type  = TCXOMGR_VCO_DEFAULT;
    vco.vco_value = tcxomgr.vco.dflt;

    return vco;
  }

  /* convert thermistor reading to index */
  temp = TEMP_TO_INX(temp);

  /* rot value not used in temp table */
  vco.rot_value = 0;

  /* check for an exact value in the temp table */
  if ( tcxomgr.vco.temp[ temp ] != VCO_UNDEFINED )
  {
    vco.vco_type = TCXOMGR_VCO_EXACT;
    vco.vco_value = tcxomgr.vco.temp[ temp ];

    return vco;
  }

  /* We have no table entry for the current temperature.  Look and look 3
  ** entries in either direction (cooler and hotter) to find a valid entry
  ** in the table.  If we find one, use it.  Otherwise, use default. */
  for (index_count = 1; index_count <= 3; index_count++)
  {
    /* Look lower in the temperature table for an entry. */
    vco_low  = (((temp - index_count) >= 0) ?
                 tcxomgr.vco.temp[ ( temp - index_count ) ] : VCO_UNDEFINED );

    /* Look higher in the temperature table for an entry. */
    vco_high = (((temp + index_count) < VCO_TEMP_TABLE_SIZE) ?
                 tcxomgr.vco.temp[ ( temp + index_count ) ] : VCO_UNDEFINED );

    if ((vco_low != VCO_UNDEFINED) || (vco_high != VCO_UNDEFINED))
    {
      /* We found a temperature table entry close by.  Quit looking. */
      break;
    }
  }

  /* if neither lower or higher VCO_TEMP values are defined use the default */
  if ( vco_low == VCO_UNDEFINED && vco_high == VCO_UNDEFINED )
  {
    vco.vco_type = TCXOMGR_VCO_DEFAULT;
    vco.vco_value = tcxomgr.vco.dflt;
  }
  else
  {
    /* One or both of the lower & higher values are defined so interpolate */
    vco.vco_type = TCXOMGR_VCO_INTERPOLATED;
    if ( vco_low == VCO_UNDEFINED )
    {
      vco.vco_value = vco_high;
    }
    else if ( vco_high == VCO_UNDEFINED )
    {
      vco.vco_value = vco_low;
    }
    else
    {
      vco.vco_value = (vco_low + vco_high) / 2;
    }
  }

  return vco;

} /* tcxomgr_get_temp_table */


/*============================================================================

FUNCTION TCXOMGR_GET_VCO

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_info_type

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_vco_info_type tcxomgr_get_stored_vco (void)
{
  /* Time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32 time;

  /* Current temperature set */
  tcxomgr_temp_set_type temp;

  /* Return Freq/Temp information */
  tcxomgr_vco_info_type ft;

  #ifdef FEATURE_XO_FACTORY_CAL
  /* Temperature correction for XO targets */
  tcxomgr_ppm_type correction;
  #endif /* !FEATURE_XO_FACTORY_CAL */
/*--------------------------------------------------------------------------*/

  /* Get current time and temperature */
  tcxomgr_get_time_and_temp(&time, &temp);

  #ifndef FEATURE_XO
  if ( !VALID_TEMP( temp.pa ) )
  #else
  if ( !VALID_TEMP( TCXOMGR_XO_TEMP_TO_DEGC( temp.xo ) ) )
  #endif
  {
    if ( ( tcxomgr.rgs.system != TCXOMGR_CLIENT_NONE ) &&
         ( time >= tcxomgr.rgs.time )                  &&
         ( ( time - tcxomgr.rgs.time ) <=
           TCXOMGR_RGS_DELTA_TIME_NO_TEMP_THRESH ) )
    {
      /* Even though we don't know the temperature of the phone, the RGS value
      ** is not very old.  Temperature should not have changed much, so let's
      ** still go ahead and use the RGS values. */
      ft.vco_type   = TCXOMGR_VCO_RGS;
      ft.rot_value  = tcxomgr.rgs.rot;
      ft.vco_value  = tcxomgr.rgs.vco;

      DBG_3( ERROR, "Invalid temp ret RGS sys/time:%d/%d Cur time:%d",
             tcxomgr.rgs.system, tcxomgr.rgs.time, time );
    }
    else
    {
      /* We do not know the current temperature, and RGS is not reliable, so
      ** return default values to the caller. */
      ft.vco_type   = TCXOMGR_VCO_DEFAULT;
      ft.rot_value  = 0;
      ft.vco_value  = tcxomgr.vco.dflt;

      DBG_3( ERROR, "Invalid temp ret defaults RGS old sys/time:%d/%d Cur time:%d",
             tcxomgr.rgs.system, tcxomgr.rgs.time, time );
    }
  }
  else if (    ( tcxomgr.rgs.system != TCXOMGR_CLIENT_NONE )
            && ( time >= tcxomgr.rgs.time )
            && ( ( time - tcxomgr.rgs.time ) <= TCXOMGR_RGS_DELTA_TIME_THRESH )
            #ifndef FEATURE_XO
            && ( ABS( temp.pa - tcxomgr.rgs.temp.pa ) <=
                 TCXOMGR_RGS_DELTA_TEMP_THRESH )
            #else
            && ( ABS( TCXOMGR_XO_TEMP_TO_DEGC( temp.xo ) -
                      TCXOMGR_XO_TEMP_TO_DEGC( tcxomgr.rgs.temp.xo ) ) <=  
                 ( TCXOMGR_RGS_DELTA_TEMP_THRESH ) )
            #endif /* FEATURE_XO */
          )
  {
    ft.vco_type  = TCXOMGR_VCO_RGS;
    ft.rot_value = tcxomgr.rgs.rot;
    ft.vco_value = tcxomgr.rgs.vco;

    #ifdef FEATURE_XO_FACTORY_CAL
    correction = tcxomgr_ft_eval( temp ) - 
                 tcxomgr_ft_eval( tcxomgr.rgs.temp );
    if ( ABS( correction ) < TCXOMGR_MAX_CORRECTION ) /* 0.9 ppm */
    {
      ft.rot_value += correction;
    }
    #endif /* FEATURE_XO_FACTORY_CAL */
  }
  else
  #ifndef FEATURE_XO_FACTORY_CAL
  {
    /* returns temp tbl entry, or default if no entry exists */
    #ifndef FEATURE_XO
    ft = tcxomgr_get_temp_table( (uint8)temp.pa );
    #else
    ft = tcxomgr_get_temp_table( 
           (uint8)( TCXOMGR_XO_TEMP_TO_DEGC( temp.xo ) ) );
    #endif
    if (    ( ft.vco_type == TCXOMGR_VCO_DEFAULT ) 
	       && ( tcxomgr.rgs.system != TCXOMGR_CLIENT_NONE ) 
         #ifndef FEATURE_XO
	       && ( ABS( temp.pa - tcxomgr.rgs.temp.pa ) <= 
                    TCXOMGR_RGS_DELTA_TEMP_THRESH )
	       #else
               && ( ABS( TCXOMGR_XO_TEMP_TO_DEGC( temp.xo ) - 
	                 TCXOMGR_XO_TEMP_TO_DEGC( tcxomgr.rgs.temp.xo ) ) <=  
	            ( TCXOMGR_RGS_DELTA_TEMP_THRESH ) )
	       #endif /* FEATURE_XO */
             )
    {
      ft.vco_type  = TCXOMGR_VCO_OLD_RGS;
      ft.rot_value = tcxomgr.rgs.rot;
      ft.vco_value = tcxomgr.rgs.vco;
    }
  }
  #else /* FEATURE_XO_FACTORY_CAL */
  {
    /* read the field cal quality indicator to determine the vco_type */
    ft.vco_type  =
      (tcxomgr_vco_indicator_type) tcxomgr.cal_data.field_data.ft_qual_ind;
    /* Calculate the Freq vs. Temp PPM error value using the calibrated
       FT curve */
    ft.rot_value = tcxomgr_ft_eval( temp );
    /* This is for XO targets only, so vco value is 0 */
    ft.vco_value = 0;
  }
  #endif /* !FEATURE_XO_FACTORY_CAL */

  /* keep record of worst case stored value used */
  if ( ft.vco_type < tcxomgr.uncertainty )
  {
    tcxomgr.uncertainty = ft.vco_type;
  }

  return ft;

}


/*============================================================================

FUNCTION TCXOMGR_GET_UNCERTAINTY

DESCRIPTION

DEPENDENCIES
  None

RETURN VALUE
  tcxomgr_vco_indicator_type

SIDE EFFECTS
  None

============================================================================*/
tcxomgr_vco_indicator_type tcxomgr_get_uncertainty (void)
{
  if ( tcxomgr.uncertainty == TCXOMGR_VCO_RGS )
  {
    /* Is RGS newer than 30 secs */
    if ( ( clk_read_secs() - tcxomgr.rgs.time ) < TCXOMGR_RGS_RECENT_THRESHOLD )
    {
      return TCXOMGR_VCO_RGS_RECENT;
    }
  }
  return tcxomgr.uncertainty;
}


/*============================================================================

FUNCTION TCXOMGR_UPDATE_TEMP_TABLE

DESCRIPTION
  This function updates the VCO temperature calibrated table

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
static void tcxomgr_update_temp_table
(
  /* VCTCXO accumulator value (TRK_LO_ADJ PDM value) */
  tcxomgr_vco_type vco_acc,

  /* Time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32 time,

  /* Temperature */
  uint8 temp
)
{
  /* Previous VCTCXO value for current temperature */
  tcxomgr_vco_info_type old_vco_temp_info;

  /* Previous VCTCXO value for current temperature */
  tcxomgr_vco_type old_vco_temp;

  /* New VCTCXO value for current temperature */
  tcxomgr_vco_type new_vco_temp;

  /* Delta between old and new vco_temp */
  tcxomgr_vco_type delta_vco_temp;

/*--------------------------------------------------------------------------*/

  /* Don't update the table if the temperature is out of range or invalid */
  /* Can't update VCO_TEMP if we're too close to last update */
  if ( 
      ( !VALID_TEMP( temp ) ) ||
      ( time < ( tcxomgr.vco.last_update_secs + TCXOMGR_UPDATE_TEMP_TBL_SEC ) )
     )
  {
    return;
  }

  /* Get old VCTCXO accumulator value */
  old_vco_temp_info = tcxomgr_get_temp_table( temp );
  old_vco_temp      = old_vco_temp_info.vco_value;

  /* convert thermistor reading to index */
  temp = TEMP_TO_INX(temp);

  if ( old_vco_temp_info.vco_type != TCXOMGR_VCO_DEFAULT )
  {
    /* The previous value was defined.  Filter in the new sample and check
       to see that delta < 0.5ppm.  Saturate if it's too large */
    delta_vco_temp = VCO_TEMP_FILT( old_vco_temp, vco_acc ) - old_vco_temp;

    if ( ABS( delta_vco_temp ) > tcxomgr.vco_temp_update_thresh )
    {
      DBG_2( ERROR, "delta_vco_temp (%d) > 0.5ppm, bounded to %d",
             ABS( delta_vco_temp ), tcxomgr.vco_temp_update_thresh );

      delta_vco_temp = SIGN( delta_vco_temp ) *
                       tcxomgr.vco_temp_update_thresh;
    }

    new_vco_temp = old_vco_temp + delta_vco_temp;
    DBG_3( MED, "VCO_TEMP[%d]: %d --> %d", temp, old_vco_temp, new_vco_temp );
  }
  else    /* old_vco_temp_info.vco_type is TCXOMGR_VCO_DEFAULT */
  {
    /* There was no previous value.  Overwrite with the new sample as
       long as it is not too large ( < 5ppm of default ) */
    delta_vco_temp = vco_acc - tcxomgr.vco.dflt;

    if ( ABS( delta_vco_temp ) > tcxomgr.vco_temp_initial_thresh )
    {
      DBG_2( ERROR, "Initial temp (%d) > 5ppm from default, bounded to %d",
             delta_vco_temp, tcxomgr.vco_temp_update_thresh );

      delta_vco_temp = SIGN( delta_vco_temp ) * tcxomgr.vco_temp_update_thresh;
    }

    new_vco_temp = tcxomgr.vco.dflt + delta_vco_temp;

    DBG_2( MED, "VCO_TEMP[%d]: n/a --> %d", temp, new_vco_temp );
  }

  /* Update RAM version of the temp table */
  tcxomgr.vco.temp[ temp ] = new_vco_temp;

  /* Update time we last updated the VCO_TEMP table */
  tcxomgr.vco.last_update_secs = time;

} /* tcxomgr_update_temp_table */


/*============================================================================

FUNCTION TCXOMGR_UPDATE_RGS_VALUES

DESCRIPTION
  This function updates the Recent Good System values

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_update_rgs_values
(
  /* client doing the update */
  tcxomgr_client_id_type  id,

  /* VCTCXO accumulator value (TRK_LO_ADJ PDM value) */
  tcxomgr_vco_type        vco_val,

  /* Rotator accumulator (in 2^-10 ppm units) */
  tcxomgr_rotator_type    rot_val
)
{
  /* Current time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32   time;

  /* Current temperature set */
  tcxomgr_temp_set_type temp;

  #ifdef FEATURE_XO_FIELD_CAL
  /* Ping the field cal state machine periodically */
  static uint32 last_field_cal_time = 0;
  #endif

/*--------------------------------------------------------------------------*/

  /* Check to make sure that this client is the current active client */
  if ( tcxomgr.client_in_control != id )
  {
    DBG_STR1( HIGH, "Client %s, attempt to update RGS when not in control",
              clients[id].name );
  }

  /* Get current time and temperature */
  tcxomgr_get_time_and_temp(&time, &temp);

  #ifndef FEATURE_XO
	DBG_STR4( HIGH, "Client %s, update RGS: vco=%d, rot=%d, temp=%d DegC",
						clients[id].name, vco_val, rot_val, temp.pa);
  #else
	DBG_STR3( HIGH, "Client %s, update RGS: vco=%d, rot=%d",
						clients[id].name, vco_val, rot_val );
	DBG_3(    HIGH, "  temp PA=%d XO=%d PMIC=%d  DegC",
						temp.pa, TCXOMGR_XO_TEMP_TO_DEGC( temp.xo ), temp.pmic );
  #endif

  /* Check if we even know the temperature of the phone. */
  #ifndef FEATURE_XO
  if ( !VALID_TEMP( temp.pa ) )
  #else
  if ( !VALID_TEMP( TCXOMGR_XO_TEMP_TO_DEGC( temp.xo ) ) )
  #endif
  {
    DBG_0( ERROR, "Can't update RGS. Unknown or Out of Bounds temperature" );
    return;
  }

  /* Store the RGS values */
  tcxomgr.rgs.system = id;
  tcxomgr.rgs.time   = time;
  tcxomgr.rgs.temp   = temp;
  tcxomgr.rgs.vco    = vco_val;
  tcxomgr.rgs.rot    = rot_val;

  #ifndef FEATURE_XO
  if ( ABS( rot_val ) < UPDATE_ROTATOR_PPM_LIMIT )
  {
    /* Update the temp table, if appropriate */
    tcxomgr_update_temp_table( vco_val, time, (uint8)temp.pa );
  }
  else
  {
    /* Can't update temp table if rotator is too large */
    DBG_2( MED, "Temp table not updated, rot val |%d| > %d",
            rot_val, UPDATE_ROTATOR_PPM_LIMIT );
  }
  #else
  tcxomgr_update_temp_table( rot_val, time, 
                             (uint8)( TCXOMGR_XO_TEMP_TO_DEGC( temp.xo ) ) );
  #endif /* FEATURE_XO */

  #ifdef FEATURE_XO_FIELD_CAL
  if ( ( time - last_field_cal_time ) >= FIELD_CAL_UPDATE_PERIOD )
  {
    rex_set_sigs( &tcxomgr_tcb, TCXOMGR_FIELD_CAL_SIG );
    last_field_cal_time = time;
  }
  #endif

  /* reset uncertainty to RGS */
  tcxomgr.uncertainty = TCXOMGR_VCO_RGS;

} /* tcxomgr_update_rgs_values */


/*============================================================================

FUNCTION TCXOMGR_RESET_RGS_AND_TEMP_TABLE

DESCRIPTION
  This function blows away all Recent Good System data and the entire trk_lo
  vs. temperature table.  It should be called very rarely and very carefully.
  If some RAT (GSM, WCDMA, ...) feels TCXO Manager data is bad, corrupt,
  invalid, etc., and should no longer be used, it calls this function to dump
  all the data.

  Effects of this function take place immediately in RAM cached copies.  New
  data doesn't go to NV until power down.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

============================================================================*/
void tcxomgr_reset_rgs_and_temp_table
(
  /* Client resetting RGS and temperature table. */
  tcxomgr_client_id_type  client_id
)
{
  /* Loop index */
  uint16                 index;

  /* Current time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32                 current_time;

  /* Current temperature set */
  tcxomgr_temp_set_type  current_temp;

/*--------------------------------------------------------------------------*/

  /* Get current time and temperature. */
  tcxomgr_get_time_and_temp(&current_time, &current_temp);

  /* Pring debug about client doing reset and current time and temperature. */
  #ifndef FEATURE_XO
	DBG_STR3( ERROR, "reset_rgs() by client:%s curr_time:%d curr_temp:%d",
						clients[client_id].name, current_time, current_temp.pa );
  #else
	DBG_STR2( ERROR, "reset_rgs() by client:%s curr_time:%d",
						clients[client_id].name, current_time );
	DBG_3(    ERROR, "  curr_temp PA:%d  XO:%d  PMIC:%d",
						current_temp.pa, current_temp.xo, current_temp.pmic );
  #endif

  /* Print debug about RGS value, before blowing it away. */
  DBG_3( ERROR, "reset_rgs() old RGS system:%d vco:%d rot:%d",
        tcxomgr.rgs.system, tcxomgr.rgs.vco, tcxomgr.rgs.rot );

  #ifndef FEATURE_XO
	DBG_3( ERROR, "reset_rgs() old RGS temp:%d time:%d last_update:%d",
				tcxomgr.rgs.temp.pa, tcxomgr.rgs.time, tcxomgr.vco.last_update_secs );
  #else
	DBG_3( ERROR, "reset_rgs() old RGS temp:%d time:%d last_update:%d",
				 tcxomgr.rgs.temp.xo, tcxomgr.rgs.time, tcxomgr.vco.last_update_secs );
  #endif /* FEATURE_XO */

  /* Dumping the entire trk_lo vs. temperature table. */
  for ( index = 0; (index + 3) < VCO_TEMP_TABLE_SIZE; index += 4 )
  {
    DBG_4( ERROR, "reset_rgs() temp table: [%d] [%d] [%d] [%d]",
          tcxomgr.vco.temp[ index ],     tcxomgr.vco.temp[ index + 1 ],
          tcxomgr.vco.temp[ index + 2 ], tcxomgr.vco.temp[ index + 3 ] );
  }

  #ifndef FEATURE_XO
  if ( !VALID_TEMP( current_temp.pa ) )
  {
    /* Write an entry to error log in NV. */
    ERR( "reset_rgs() by client:%d old RGS system:%d current_temp vco:%d",
         client_id, tcxomgr.rgs.vco,
         tcxomgr.vco.temp[ TEMP_TO_INX( current_temp.pa ) ]);
  }
  #else
  if ( !VALID_TEMP( TCXOMGR_XO_TEMP_TO_DEGC( current_temp.xo ) ) )
  {
    /* Write an entry to error log in NV. */
    ERR( "reset_rgs() by client:%d old RGS system:%d current_temp vco:%d",
         client_id, tcxomgr.rgs.vco,
         tcxomgr.vco.temp[ TEMP_TO_INX( current_temp.xo ) ]);
  }
  #endif

  /* Reset all of our RGS data. */
  tcxomgr.rgs.system    = TCXOMGR_CLIENT_NONE;
  tcxomgr.rgs.vco       = tcxomgr.vco.dflt;
  tcxomgr.rgs.rot       = 0;
  tcxomgr.rgs.temp.pa   = TCXOMGR_INVALID_TEMP;
  tcxomgr.rgs.temp.pmic = TCXOMGR_INVALID_TEMP;
  tcxomgr.rgs.temp.xo   = TCXOMGR_INVALID_TEMP;
  tcxomgr.rgs.time      = 0;

  /* Next reset the entire trk_lo vs. temperature table. */
  for ( index = 0; index < VCO_TEMP_TABLE_SIZE; index++ )
  {
    tcxomgr.vco.temp[ index ] = VCO_UNDEFINED;
  }

  /* Restart last update time, since we just blew the table away. */
  tcxomgr.vco.last_update_secs = clk_read_secs();

} /* tcxomgr_reset_rgs_and_temp_table */

