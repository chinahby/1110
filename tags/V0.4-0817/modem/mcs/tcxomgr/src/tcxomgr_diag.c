/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   T C X O M G R _ D I A G - TCXOMGR Diag interface

GENERAL DESCRIPTION
  This module contains functions pertaining to the interface between tcxomgr
  and diag

EXTERNALIZED FUNCTIONS

INITIALIZATION AND SEQUENCING REQUIREMENTS

      Copyright (c) 2006 - 2009
                    by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/mcs/tcxomgr/main/latest/src/tcxomgr_diag.c#10 $

when       who      what, where, why
--------   ---      ---------------------------------------------------------
04/22/09   sst      Update invalid temp define
10/20/08   adw      Replaced FEATURE_XO_DIAG w/ FEATURE_TCXOMGR_DIAG.
09/09/08   sg       Fix lint/klocwork warnings
08/28/08   adw      Added FEATURE_XO_DIAG to featurize for ULC.
07/11/08   scm      Check for unknown temperature in rgs_override().
06/30/08   mca      Suppressed lint errors
06/20/08   mca      Merge from //depot
04/07/08   ljl      Supported multi-carrier AFC log.
03/03/08   va       Use temperature set instead of temp in RGS
12/20/07   scm      Check for NULL ptr from diagpkt_subsys_alloc().
11/16/07   va       Remove TCXO Diag cal functions. 
                    XO Freq Est logging Support
09/25/07   cab      Moved tcxomgr global object reference to header file
09/04/07   cab      Added support for diag control of slope error
                    Field cal logging support
07/26/07   sst      Change get_time_and_temp to use temp_set_type
07/20/07   sst      Update FacCal to increase offset size and enable NV write
06/26/07   sst      More FacCal initializations
06/13/07   sst      Update some FacCal initializations
06/08/07   sst      Set cal_test_dc__params.update_nv in FacCal diag cmd
06/06/07   sst      Added Factory Calibration diag command
03/20/07   cab      Removed type cast to log_type
03/15/07   cab      Removed inclusion of mc.h
10/10/06   cab      Initial version for tcxomgr v3
10/12/06   cab      Removed TCXOMGR_RGS_NONE
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"

#ifdef FEATURE_TCXOMGR_DIAG

#include "rex.h"
#include "tcxomgr_i.h"
#include "comdef.h"
#include "diagpkt.h"
#include "diagcmd.h"
#include "msg.h"
#include "err.h"

#include "tcxomgr.h"
#include "tcxomgr_diag.h"

/*===========================================================================
              LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*-------------------------------------------------------------------------
      Constants
-------------------------------------------------------------------------*/

/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/
typedef struct
{
  override_cmd_type          temp_table_override_enable;
  tcxomgr_vco_type           temp_table[VCO_TEMP_TABLE_SIZE];

  override_cmd_type          vco_dflt_override_enable;
  tcxomgr_vco_type           vco_dflt;

  override_cmd_type          vco_slope_override_enable;
  uint16                     vco_slope;

  override_cmd_type          slope_range_override_enable;
  uint8                      slope_range;

  override_cmd_type          rgs_override_enable;
  uint32                     rgs_delta_time;
  int16                      rgs_delta_temp;

  boolean                    rfe_slope_override_enable;
} tcxomgr_diag_type;


/*-------------------------------------------------------------------------
     Static Function Prototypes
-------------------------------------------------------------------------*/


static PACKED void *tcxomgr_diag_temp_table_override  (PACKED void* req_pkt, uint16 pkt_len);
static PACKED void *tcxomgr_diag_temp_table_read      (PACKED void* req_pkt, uint16 pkt_len);
static PACKED void *tcxomgr_diag_vco_default_override (PACKED void* req_pkt, uint16 pkt_len);
static PACKED void *tcxomgr_diag_vco_slope_override   (PACKED void* req_pkt, uint16 pkt_len);
static PACKED void *tcxomgr_diag_slope_range_override (PACKED void* req_pkt, uint16 pkt_len);
static PACKED void *tcxomgr_diag_rgs_override         (PACKED void* req_pkt, uint16 pkt_len);
#ifdef TCXOMGR_UNIT_TEST
static PACKED void *tcxomgr_diag_run_test_case        (PACKED void* req_pkt, uint16 pkt_len);
#endif
static PACKED void *tcxomgr_diag_rfe_slope_override   (PACKED void* req_pkt, uint16 pkt_len);

/*-------------------------------------------------------------------------
      Variables
-------------------------------------------------------------------------*/

/* Diag internal variables */
static tcxomgr_diag_type tcxomgr_diag;

static const diagpkt_user_table_entry_type tcxomgr_diag_tbl[] =
{
  /* start id, end id, callback function */
  {TEMP_TABLE_OVERRIDE,
   TEMP_TABLE_OVERRIDE,
   tcxomgr_diag_temp_table_override},
  {TEMP_TABLE_READ,
   TEMP_TABLE_READ,
   tcxomgr_diag_temp_table_read},
  {VCO_DFLT_OVERRIDE,
   VCO_DFLT_OVERRIDE,
   tcxomgr_diag_vco_default_override},
  {VCO_SLOPE_OVERRIDE,
   VCO_SLOPE_OVERRIDE,
   tcxomgr_diag_vco_slope_override},
  {SLOPE_RANGE_OVERRIDE,
   SLOPE_RANGE_OVERRIDE,
   tcxomgr_diag_slope_range_override},
  {RGS_OVERRIDE,
   RGS_OVERRIDE,
   tcxomgr_diag_rgs_override},
#ifdef TCXOMGR_UNIT_TEST
  {RUN_TEST_CASE,
   RUN_TEST_CASE,
   tcxomgr_diag_run_test_case},
#endif
  {RFE_SLOPE_OVERRIDE,
   RFE_SLOPE_OVERRIDE,
   tcxomgr_diag_rfe_slope_override},
};


/*===========================================================================
              FUNCTION DEFINITIONS FOR MODULE
===========================================================================*/


/*-------------------------------------------------------------------------
      Static  Functions
-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
      Diag Table Call Back Functions
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION TCXOMGR_DIAG_TEMP_TABLE_OVERRIDE

DESCRIPTION    This function is called when Diag requests to enable the
  Temp Table Override.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   New VCO Temp Table Values may be written to NV.

NOTE: Writes to NV may be undone by disabling the override and shutting
  down the phone.  This will restore the old data in RAM, and on shutdown
  the RAM values will be written to NV.


===========================================================================*/

static PACKED void *tcxomgr_diag_temp_table_override
(
  PACKED void* req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{
  int i;
  override_cmd_type old_override;

  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_OVERRIDE_rsp_type *rsp_ptr;

  /*-----------------------------------------------------------------------*/

  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_OVERRIDE_req_type))
  {
    MSG_ERROR("Invalid Temp Table Override Request (%d)",pkt_len,0,0);
    return(NULL);
  }

  /* Keep the current override state */
  old_override = tcxomgr_diag.temp_table_override_enable;

  /* Store the value from the message */
  tcxomgr_diag.temp_table_override_enable = (override_cmd_type)
     ((DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_OVERRIDE_req_type *)req_pkt)->enable;

  /* Did we just turn the override off? */
  if ( (old_override != NO_OVERRIDE) &&
       (tcxomgr_diag.temp_table_override_enable == NO_OVERRIDE) )
  {
    /* Put the old data back */
    for (i = 0; i < VCO_TEMP_TABLE_SIZE; i++)
    {
      /* Write the old table */
      tcxomgr.vco.temp[i] = tcxomgr_diag.temp_table[i];
    }
  }
  else if (tcxomgr_diag.temp_table_override_enable != NO_OVERRIDE)
  {
    for ( i = 0; i < VCO_TEMP_TABLE_SIZE; i++ )
    {
      /* Store the old table */
      tcxomgr_diag.temp_table[i] = tcxomgr.vco.temp[i];

      /* Overwrite the old value */
      tcxomgr.vco.temp[i] = (tcxomgr_vco_type)
                           ((DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_OVERRIDE_req_type *)req_pkt)->temp[i];
    }
  }

  /* Check if we need to write to NV */
  if ( tcxomgr_diag.temp_table_override_enable == OVERRIDE_AND_WRITE )
  {
    /* Copy the data into the nv data struct */
    for ( i=0; i<VCO_TEMP_TABLE_SIZE; i++ )
    {
      tcxomgr.nv.item.vco_temp[i] = tcxomgr.vco.temp[i];
    }

    /* Write the data into NV */
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_VCO_TEMP_I );
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_OVERRIDE_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_TCXOMGR, TEMP_TABLE_OVERRIDE,
              sizeof(DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_OVERRIDE_rsp_type));

  return(rsp_ptr);

} /* tcxomgr_diag_temp_table_override */




/*===========================================================================

FUNCTION TCXOMGR_DIAG_TEMP_TABLE_READ

DESCRIPTION    This function is called when Diag requests to read the
  temp table.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
static PACKED void *tcxomgr_diag_temp_table_read
(
  PACKED void* req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{
  DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_READ_rsp_type  *rsp_ptr;

  const int rsp_len = sizeof(DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_READ_rsp_type);

  int i;

  /*-----------------------------------------------------------------------*/

  NOTUSED(req_pkt);
  NOTUSED(pkt_len);

  /* Allocate memory for this packet */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_TEMP_TABLE_READ_rsp_type *)
              diagpkt_subsys_alloc(DIAG_SUBSYS_TCXOMGR,
                                   TEMP_TABLE_READ, rsp_len);

  if (rsp_ptr)
  {
    for (i=0; i<VCO_TEMP_TABLE_SIZE; i++)
    {
      rsp_ptr->temp[i] = tcxomgr.vco.temp[i];
    }
  }
  else
  {
    MSG_ERROR("diagpkt_subsys_alloc(%d, %d, %d) returned NULL",
              DIAG_SUBSYS_TCXOMGR, TEMP_TABLE_READ, rsp_len);
  }

  return rsp_ptr;
}




/*===========================================================================

FUNCTION TCXOMGR_DIAG_VCO_DEFAULT_OVERRIDE

DESCRIPTION    This function is called when Diag requests to enable the
  VCO Default Override.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   New VCO Default Value may be written to NV.

NOTE: Writes to NV may be undone by disabling the override and shutting
  down the phone.  This will restore the old data in RAM, and on shutdown
  the RAM values will be written to NV.


===========================================================================*/
static PACKED void *tcxomgr_diag_vco_default_override
(
  PACKED void* req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{
  override_cmd_type old_override;

  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_VCO_DFLT_OVERRIDE_rsp_type *rsp_ptr;

  /*-----------------------------------------------------------------------*/

  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_VCO_DFLT_OVERRIDE_req_type))
  {
    MSG_ERROR("Invalid VCO Default Override Request (%d)",pkt_len,0,0);
    return(NULL);
  }

  /* Keep the old override state */
  old_override = tcxomgr_diag.vco_dflt_override_enable;

  /* Store the value from the message */
  tcxomgr_diag.vco_dflt_override_enable = (override_cmd_type)
     ((DIAG_SUBSYS_TCXOMGR_VCO_DFLT_OVERRIDE_req_type *)req_pkt)->enable;

  /* Check to see if we just dissabled the override */
  if ( (old_override != NO_OVERRIDE) &&
       (tcxomgr_diag.vco_dflt_override_enable == NO_OVERRIDE) )
  {
    /* Disabling the override so put the old value back */
    tcxomgr.vco.dflt = tcxomgr_diag.vco_dflt;
  }
  else if (tcxomgr_diag.vco_dflt_override_enable != NO_OVERRIDE)
  {
    /* Save the current default value */
    tcxomgr_diag.vco_dflt = tcxomgr.vco.dflt;

    /* Write the new value */
    tcxomgr.vco.dflt = (tcxomgr_vco_type)
                      ((DIAG_SUBSYS_TCXOMGR_VCO_DFLT_OVERRIDE_req_type *)req_pkt)->dflt;
  }

  /* Check if we need to write to NV */
  if ( tcxomgr_diag.vco_dflt_override_enable == OVERRIDE_AND_WRITE )
  {
    /* Copy the data into the nv data struct */
    tcxomgr.nv.item.vco_default = tcxomgr.vco.dflt;

    /* Write the data into NV */
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_VCO_DEFAULT_I );
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_VCO_DFLT_OVERRIDE_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_TCXOMGR, VCO_DFLT_OVERRIDE,
              sizeof(DIAG_SUBSYS_TCXOMGR_VCO_DFLT_OVERRIDE_rsp_type));

  return(rsp_ptr);

}



/*===========================================================================

FUNCTION TCXOMGR_DIAG_VCO_SLOPE_OVERRIDE

DESCRIPTION    This function is called when Diag requests to enable the
  Temp Slope Override.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   New VCO Slope Value may be written to NV.

NOTE: Writes to NV may be undone by disabling the override and shutting
  down the phone.  This will restore the old data in RAM, and on shutdown
  the RAM values will be written to NV.

===========================================================================*/
static PACKED void *tcxomgr_diag_vco_slope_override
(
  PACKED void* req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{
  override_cmd_type old_override;

  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_VCO_SLOPE_OVERRIDE_rsp_type *rsp_ptr;

  /*-----------------------------------------------------------------------*/

  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_VCO_SLOPE_OVERRIDE_req_type))
  {
    MSG_ERROR("Invalid VCO Default Override Request (%d)",pkt_len,0,0);
    return(NULL);
  }

  /* Keep the old override state */
  old_override = tcxomgr_diag.vco_slope_override_enable;

  /* Store the value from the message */
  tcxomgr_diag.vco_slope_override_enable = (override_cmd_type)
     ((DIAG_SUBSYS_TCXOMGR_VCO_SLOPE_OVERRIDE_req_type *)req_pkt)->enable;

  /* Check to see if we just dissabled the override */
  if ( (old_override != NO_OVERRIDE) &&
       (tcxomgr_diag.vco_slope_override_enable == NO_OVERRIDE) )
  {
    /* We just dissabled the override so put the original value back */
    tcxomgr.vco.slope = tcxomgr_diag.vco_slope;
  }
  else if (tcxomgr_diag.vco_slope_override_enable != NO_OVERRIDE)
  {
    /* Save the old value */
    tcxomgr_diag.vco_slope = tcxomgr.vco.slope;

    /* Write the new value */
    tcxomgr.vco.slope = (uint16)
                       ((DIAG_SUBSYS_TCXOMGR_VCO_SLOPE_OVERRIDE_req_type *)req_pkt)->slope;
  }

  /* Check if we need to write to NV */
  if ( tcxomgr_diag.vco_slope_override_enable == OVERRIDE_AND_WRITE )
  {
    /* Copy the data into the nv data struct */
    tcxomgr.nv.item.vco_slope = tcxomgr.vco.slope;

    /* Write the data into NV */
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_VCO_SLOPE_I );
  }

  /* Since we probably changed the slope value lets update the thresholds */
  /* Set the update threshold to .5ppm or:
   * 2^20 / NV_VCO_SLOPE / 2 */
  tcxomgr.vco_temp_update_thresh  =
    TCXOMGR_GEN_UPDATE_THRESH( tcxomgr.vco.slope );

  /* Set the initial threshold to 5ppm or:
   * 5 * 2^20 / NV_VCO_SLOPE */
  tcxomgr.vco_temp_initial_thresh =
    TCXOMGR_GEN_INITIAL_THRESH( tcxomgr.vco.slope );

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_VCO_SLOPE_OVERRIDE_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_TCXOMGR, VCO_SLOPE_OVERRIDE,
              sizeof(DIAG_SUBSYS_TCXOMGR_VCO_SLOPE_OVERRIDE_rsp_type));

  return(rsp_ptr);

}



/*===========================================================================

FUNCTION TCXOMGR_DIAG_SLOPE_RANGE_OVERRIDE

DESCRIPTION    This function is called when Diag requests to enable the
  Temp Slope Override.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   New Slope Range Value may be written to NV.

NOTE: Writes to NV may be undone by disabling the override and shutting
  down the phone.  This will restore the old data in RAM, and on shutdown
  the RAM values will be written to NV.


===========================================================================*/
static PACKED void *tcxomgr_diag_slope_range_override
(
  PACKED void* req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{
  override_cmd_type old_override;

  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_SLOPE_RANGE_OVERRIDE_rsp_type *rsp_ptr;

  /*-----------------------------------------------------------------------*/

  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_SLOPE_RANGE_OVERRIDE_req_type))
  {
    MSG_ERROR("Invalid Slope Range Override Request (%d)",pkt_len,0,0);
    return(NULL);
  }

  /* Keep the old override state */
  old_override = tcxomgr_diag.slope_range_override_enable;

  /* Store the value from the message */
  tcxomgr_diag.slope_range_override_enable = (override_cmd_type)
     ((DIAG_SUBSYS_TCXOMGR_SLOPE_RANGE_OVERRIDE_req_type *)req_pkt)->enable;

  /* Check to see if we just dissabled the override */
  if ( (old_override != NO_OVERRIDE) &&
       (tcxomgr_diag.slope_range_override_enable == NO_OVERRIDE) )
  {
    /* Put the old value back */
    tcxomgr.vco.slope_range = tcxomgr_diag.slope_range;
  }
  else if (tcxomgr_diag.slope_range_override_enable != NO_OVERRIDE)
  {
    /* Save the old value */
    tcxomgr_diag.slope_range = tcxomgr.vco.slope_range;

    /* Write the new value */
    tcxomgr.vco.slope_range = (uint8)
                             ((DIAG_SUBSYS_TCXOMGR_SLOPE_RANGE_OVERRIDE_req_type *)req_pkt)->slope_range;
  }

  /* Check if we need to write to NV */
  if ( tcxomgr_diag.slope_range_override_enable == OVERRIDE_AND_WRITE )
  {
    /* Copy the data into the nv data struct */
    tcxomgr.nv.item.vco_slope_range = tcxomgr.vco.slope_range;

    /* Write the data into NV */
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_VCO_SLOPE_RANGE_I );
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_SLOPE_RANGE_OVERRIDE_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_TCXOMGR, SLOPE_RANGE_OVERRIDE,
              sizeof(DIAG_SUBSYS_TCXOMGR_SLOPE_RANGE_OVERRIDE_rsp_type));

  return(rsp_ptr);

}



/*===========================================================================

FUNCTION TCXOMGR_DIAG_RGS_OVERRIDE

DESCRIPTION    This function is called when Diag requests to enable the
  Temp Slope Override.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   New RGS Values may be written to NV.

NOTE: Writes to NV may be undone by disabling the override and shutting
  down the phone.  This will restore the old data in RAM, and on shutdown
  the RAM values will be written to NV.

===========================================================================*/
static PACKED void *tcxomgr_diag_rgs_override
(
  PACKED void* req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{
  override_cmd_type old_override;

  /* Time (in seconds) since 6 JAN 1980 00:00:00 */
  uint32                          time;

  /* Current Temperature set */
  tcxomgr_temp_set_type           temp;

  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_rsp_type *rsp_ptr;

  /*-----------------------------------------------------------------------*/

  /* Get temp */
  tcxomgr_get_time_and_temp(&time, &temp);

  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_req_type))
  {
    MSG_ERROR("Invalid RGSult Override Request (%d)",pkt_len,0,0);
    return(NULL);
  }

  /* Make sure we know temperature to process RGS override. */
  if ( temp.pa == TCXOMGR_INVALID_TEMP )
  {
    MSG_ERROR("Can't override RGS. Unknown UE temperature",0,0,0);
    return(NULL);
  }

  /* Keep the old override state */
  old_override = tcxomgr_diag.rgs_override_enable;

  /* Store the value from the message */
  tcxomgr_diag.rgs_override_enable = (override_cmd_type)
     ((DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_req_type *)req_pkt)->enable;


  /* Did we just disable the override? */
  if ( (old_override != NO_OVERRIDE) &&
       (tcxomgr_diag.rgs_override_enable == NO_OVERRIDE) )
  {
    /* Reset the RGS data so that it will not get used until someone
     * updates it */
    tcxomgr.rgs.system    = TCXOMGR_CLIENT_NONE;
    tcxomgr.rgs.vco       = tcxomgr.vco.dflt;
    tcxomgr.rgs.temp.pa   = 0;
    tcxomgr.rgs.temp.pmic = TCXOMGR_INVALID_TEMP;
    tcxomgr.rgs.temp.xo   = TCXOMGR_INVALID_TEMP;
    tcxomgr.rgs.time      = 0;
    tcxomgr.rgs.rot       = 0;
  }
  else if (tcxomgr_diag.rgs_override_enable != NO_OVERRIDE)
  {
    /* Write the new values */
    tcxomgr.rgs.system = (tcxomgr_client_id_type)
                         ((DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_req_type *)req_pkt)->rgs.system;

    /* Store the temp delta locally */
    tcxomgr_diag.rgs_delta_temp = (int16)
     ((DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_req_type *)req_pkt)->rgs.delta_temp;

    /* Store the current temp plus the delta temp to keep the data valid */
    tcxomgr.rgs.temp.pa = temp.pa + (uint8)tcxomgr_diag.rgs_delta_temp;

    /* However, we want the rgs packet to always return the delta time
    * passed in here. */
    tcxomgr_diag.rgs_delta_time = (uint32)
                                  ((DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_req_type *)req_pkt)->rgs.time;

    /* Store the current time in the RGS field so that it is valid */
    tcxomgr.rgs.time = time - tcxomgr_diag.rgs_delta_time;

    tcxomgr.rgs.vco = (tcxomgr_vco_type)
                      ((DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_req_type *)req_pkt)->rgs.vco;

    tcxomgr.rgs.rot = (tcxomgr_rotator_type)
                      ((DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_req_type *)req_pkt)->rgs.rot;
  }

  /* Check if we need to write to NV */
  if ( tcxomgr_diag.rgs_override_enable == OVERRIDE_AND_WRITE )
  {

    /* Write the data into NV */
    tcxomgr.nv.item.rgs_type = tcxomgr.rgs.system;
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_TYPE_I );

    /* Write the data into NV */
    tcxomgr.nv.item.rgs_temp = temp.pa + (uint8)tcxomgr_diag.rgs_delta_temp;
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_TEMP_I );

    /* Store the current time minus the delta in the RGS field */
    /* Write the data into NV */
    tcxomgr.nv.item.rgs_time = time - tcxomgr_diag.rgs_delta_time;
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_TIME_I );

    /* Write the data into NV */
    tcxomgr.nv.item.rgs_vco = tcxomgr.rgs.vco;
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_VCO_I );

    /* Write the data into NV */
    tcxomgr.nv.item.rgs_rot = tcxomgr.rgs.rot;
    (void) tcxomgr_nv_cmd( NV_WRITE_F, NV_RGS_ROT_I );
  }

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_TCXOMGR, RGS_OVERRIDE,
              sizeof(DIAG_SUBSYS_TCXOMGR_RGS_OVERRIDE_rsp_type));

  return(rsp_ptr);
}


#ifdef TCXOMGR_UNIT_TEST
/*===========================================================================

FUNCTION TCXOMGR_DIAG_RUN_TEST_CASE

DESCRIPTION

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS

===========================================================================*/
static PACKED void *tcxomgr_diag_run_test_case
(
  PACKED void* req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{

  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_RUN_TEST_CASE_rsp_type *rsp_ptr;

  /*-----------------------------------------------------------------------*/

  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_RUN_TEST_CASE_req_type))
  {
    MSG_ERROR("Invalid test case request (%d)",pkt_len,0,0);
    return(NULL);
  }

  /* Store the value from the message */
  tcxomgr.test_case = (uint8)
     ((DIAG_SUBSYS_TCXOMGR_RUN_TEST_CASE_req_type *)req_pkt)->test_case;

  /* Send signal to test tasks */
  rex_set_sigs(&tcxomgr_client1_tcb, TCXOMGR_RUN_TEST_CASE_SIG);
  rex_set_sigs(&tcxomgr_client2_tcb, TCXOMGR_RUN_TEST_CASE_SIG);

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_RUN_TEST_CASE_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_TCXOMGR, RUN_TEST_CASE,
              sizeof(DIAG_SUBSYS_TCXOMGR_RUN_TEST_CASE_rsp_type));

  return(rsp_ptr);
}
#endif /* TCXOMGR_UNIT_TEST */

/*===========================================================================

FUNCTION TCXOMGR_DIAG_RFE_SLOPE_OVERRIDE

DESCRIPTION    This function is called when Diag requests to enable the
  RFE Slope Override.

DEPENDENCIES   None.

RETURN VALUE   None.

SIDE EFFECTS   New RFE Slope Value may be written to NV.

NOTE: Writes to NV may be undone by disabling the override and shutting
  down the phone.  This will restore the old data in RAM, and on shutdown
  the RAM values will be written to NV.

===========================================================================*/
static PACKED void *tcxomgr_diag_rfe_slope_override
(
  PACKED void* req_pkt,  /* request packet */
  uint16 pkt_len         /* request packet length */
)
{
  /* response pkt */
  DIAG_SUBSYS_TCXOMGR_RFE_SLOPE_OVERRIDE_rsp_type *rsp_ptr;

  /*-----------------------------------------------------------------------*/

  if(pkt_len != sizeof(DIAG_SUBSYS_TCXOMGR_RFE_SLOPE_OVERRIDE_req_type))
  {
    MSG_ERROR("Invalid RFE Slope Override Request (%d)",pkt_len,0,0);
    return(NULL);
  }

  /* Store the value from the message */
  tcxomgr_diag.rfe_slope_override_enable = (boolean)
     ((DIAG_SUBSYS_TCXOMGR_RFE_SLOPE_OVERRIDE_req_type *)req_pkt)->enable;

  /* Allocate the memory for the response */
  rsp_ptr = (DIAG_SUBSYS_TCXOMGR_RFE_SLOPE_OVERRIDE_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_TCXOMGR, RFE_SLOPE_OVERRIDE,
              sizeof(DIAG_SUBSYS_TCXOMGR_RFE_SLOPE_OVERRIDE_rsp_type));

  return(rsp_ptr);

}

/*-------------------------------------------------------------------------
      External  Functions
-------------------------------------------------------------------------*/

/*===========================================================================

FUNCTION TCXOMGR_DIAG_INIT

DESCRIPTION    This function is called at startup to load Diag with the
      TCXOMGR logging dispatch table.

DEPENDENCIES   tcxomgr_diag_tbl.

RETURN VALUE   None.

SIDE EFFECTS   None.

===========================================================================*/
void tcxomgr_diag_init (void)
{

  /*-----------------------------------------------------------------------*/

  memset(&tcxomgr_diag,0,sizeof(tcxomgr_diag));

  DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_TCXOMGR, tcxomgr_diag_tbl);

} /* tcxomgr_diag_init */



/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_TEMP_TABLE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the temp table override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
boolean tcxomgr_diag_get_temp_table_override_enable( void )
{
  return( tcxomgr_diag.temp_table_override_enable);
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_VCO_DFLT_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the vco_dflt override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
boolean tcxomgr_diag_get_vco_dflt_override_enable( void )
{
  return( tcxomgr_diag.vco_dflt_override_enable);
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_VCO_SLOPE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the vco_slope override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
boolean tcxomgr_diag_get_vco_slope_override_enable( void )
{
  return( tcxomgr_diag.vco_slope_override_enable);
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_SLOPE_RANGE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the SLOPE_RANGE override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
boolean tcxomgr_diag_get_slope_range_override_enable( void )
{
  return( tcxomgr_diag.slope_range_override_enable);
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RFE_SLOPE_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the rfe_slope override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
boolean tcxomgr_diag_get_rfe_slope_override_enable( void )
{
  return( tcxomgr_diag.rfe_slope_override_enable);
}

/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RGS_OVERRIDE_ENABLE

DESCRIPTION    This function returns the state of the RGS override.

DEPENDENCIES   None.

RETURN VALUE   boolean.

SIDE EFFECTS   None.

===========================================================================*/
boolean tcxomgr_diag_get_rgs_override_enable( void )
{
  return( tcxomgr_diag.rgs_override_enable);
}



/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RGS_TIME

DESCRIPTION    This function returns the delta time for the overriden RGS

DEPENDENCIES   None.

RETURN VALUE   uint32 delta time.

SIDE EFFECTS   None.

===========================================================================*/
uint32 tcxomgr_diag_get_rgs_time( void )
{
  return( tcxomgr_diag.rgs_delta_time);
}


/*===========================================================================

FUNCTION TCXOMGR_DIAG_GET_RGS_TEMP_DIFF

DESCRIPTION    This function returns the temperature difference for the overriden RGS

DEPENDENCIES   None.

RETURN VALUE   uint32 delta time.

SIDE EFFECTS   None.

===========================================================================*/
int16 tcxomgr_diag_get_rgs_temp_diff( void )
{
  return( tcxomgr_diag.rgs_delta_temp);
}

#ifdef FEATURE_XO_FIELD_CAL
/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_FIELD_CAL_LOG

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tcxomgr_diag_send_field_cal_log
(
  tcxomgr_log_field_cal_struct_type *field_cal_data
)
{
  LOG_TCXOMGR_FIELD_CAL_C_type *log_ptr;

  log_ptr = (LOG_TCXOMGR_FIELD_CAL_C_type*)
            log_alloc_ex( (log_code_type) LOG_TCXOMGR_FIELD_CAL_C,
            sizeof(LOG_TCXOMGR_FIELD_CAL_C_type) );

  if ( log_ptr  != NULL )
  {
    INTLOCK( );

    /* Update the version number of the log packet */
    log_ptr->version = TCXOMGR_FIELD_CAL_LOG_VERSION;

    /* Copy the header information */
    log_ptr->field_cal_data   = *field_cal_data;

    INTFREE( );

    /* submit the log to DIAG */
    log_commit( log_ptr );
  }
}
#endif /* FEATURE_XO_FIELD_CAL */


#ifdef FEATURE_XO_TASK
/*===========================================================================

FUNCTION TCXOMGR_DIAG_SEND_FREQ_EST_LOG

DESCRIPTION

DEPENDENCIES

RETURN VALUE

SIDE EFFECTS

===========================================================================*/
void tcxomgr_diag_send_freq_est_log
(
  tcxomgr_log_freq_est_struct_type *freq_est_data
)
{
  LOG_XO_FREQ_EST_C_type *log_ptr;
  uint16                  log_size;
  uint8 *                 log_data_ptr;

  log_size = FPOS(LOG_XO_FREQ_EST_C_type, freq_est_data.data);
  log_size += freq_est_data->freq_est_hdr.num_freq_est_data *
              sizeof(tcxomgr_log_freq_est_data_struct_type);
  log_size += freq_est_data->freq_est_hdr.num_rpush_data *
              sizeof(tcxomgr_log_freq_est_rpush_struct_type);
  if (freq_est_data->freq_est_hdr.rot_valid)
  {
    log_size += freq_est_data->freq_est_hdr.num_freq_est_data *
                sizeof(tcxomgr_rotator_type);
  }

  log_ptr = (LOG_XO_FREQ_EST_C_type*)
            log_alloc_ex( (log_code_type) LOG_XO_FREQ_EST_C, log_size);

  if ( log_ptr  != NULL )
  {
    /* Update the version number of the log packet */
    log_ptr->version = TCXOMGR_FREQ_EST_LOG_VERSION;

    /* Copy the header information */
    log_ptr->freq_est_data.freq_est_hdr = freq_est_data->freq_est_hdr;

    log_data_ptr = (uint8 *)log_ptr->freq_est_data.data;

    /* Copy data */
    log_size = freq_est_data->freq_est_hdr.num_freq_est_data *
               sizeof(tcxomgr_log_freq_est_data_struct_type);
    memcpy( (void *)log_data_ptr, (void *)freq_est_data->data, log_size );
    log_data_ptr += log_size;

    /* Copy rpush */
    log_size = freq_est_data->freq_est_hdr.num_rpush_data *
               sizeof(tcxomgr_log_freq_est_rpush_struct_type);
    memcpy( (void *)log_data_ptr, (void *)freq_est_data->rpush, log_size );
    log_data_ptr += log_size;

    if (freq_est_data->freq_est_hdr.rot_valid)
    {
      log_size = freq_est_data->freq_est_hdr.num_freq_est_data *
                 sizeof(tcxomgr_rotator_type);
      memcpy( (void *)log_data_ptr, (void *)freq_est_data->rot, log_size );
    }

    /* submit the log to DIAG */
    log_commit( log_ptr );
  }
}
#endif /* FEATURE_XO_TASK */


/*============================================================================

FUNCTION TCXOMGR_MC_FLUSH_CLIENT_LOG

DESCRIPTION
  This function sends the AFC log to diag if the log buffer is full or if
  logging client wanted it to be flushed because of AFC state change etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void tcxomgr_mc_flush_client_log
(
  tcxomgr_mc_log_struct_type     *afc_log_ptr
)
{
  LOG_TCXOMGR_AFC_DATA_C_type *log_ptr;       /* Log buffer pointer */
  uint8                        i;             /* loop variable */
  uint32                       num_freq_entry;/* Number of frequency entries*/
  uint8                        c;             /* carrier index */
  uint8                        carrier_num = 0;  /* Number of carriers */
  uint16                       mc_log_header_size;
    /* log header size in byte */
  uint16                       mc_log_carriers_size;
    /* log carrier size in byte */
  uint16                       mc_log_entry_size;
    /* log size per entry in byte */
  uint16                       mc_log_entries_size;
    /* log size for all entries in byte */
  char                         *ptr;
    /* Pointer to the log buffer */

/*------------------------------------------------------------------------*/

  /* Get the current number of frequency entries */
  num_freq_entry = afc_log_ptr->afc_header_info.num_freq_entry;
  if ( num_freq_entry > TCXOMGR_MAX_FREQ_ENTRIES ) 
  {
    ERR_FATAL("num_freq_entries is too large", num_freq_entry, 0, 0);
  }

  /* Log the packet if this log code is on and if there is an entry */
  if ( log_status(LOG_TCXOMGR_AFC_DATA_C) && ( num_freq_entry != 0 ))
  {
    for ( c = 0; c < TCXOMGR_MAX_NUM_CARRIERS; c++ ) 
    {
      if ( afc_log_ptr->afc_header_info.carrier_mask & ( 1 << c )) 
      {
        carrier_num++;
         /* The carrier is enabled */
      }
    }

    mc_log_header_size = sizeof(tcxomgr_mc_log_header_struct_type);
      /* size of log header */

    mc_log_carriers_size = 
                        sizeof(tcxomgr_mc_log_carrier_struct_type) * carrier_num;
      /* Total size of carrier info */

    mc_log_entry_size = sizeof(tcxomgr_mc_log_entries_struct_type) -
               sizeof(uint16)*(TCXOMGR_MAX_NUM_CARRIERS - carrier_num );
      /* Size of each entry */

    mc_log_entries_size = mc_log_entry_size * num_freq_entry;
      /* Total size of entries */

    log_ptr = (LOG_TCXOMGR_AFC_DATA_C_type*)
              log_alloc_ex( (log_code_type) LOG_TCXOMGR_AFC_DATA_C,
              TCXOMGR_MC_VAR_SIZE( mc_log_header_size,
                                   mc_log_carriers_size,
                                   mc_log_entries_size ) );

    if ( log_ptr  != NULL )
    {
      INTLOCK( );

      /* Update the version number of the log packet */
      log_ptr->version = TCXOMGR_MC_AFC_LOG_VERSION;

      ptr = (void *)&log_ptr->afc_header_info;

      /*lint -save -e416 -e419 -e662 ignore errors from the variable sized log packet */
      /* Copy the header information */
      memcpy( (void *)ptr,
              (void *)&afc_log_ptr->afc_header_info,
              mc_log_header_size );

      ptr += mc_log_header_size;
        /* Update log buffer pointer */

      /* Copy channel info */
      memcpy( (void *)ptr,
              (void *)&afc_log_ptr->carrier[0],
              mc_log_carriers_size );

      ptr += mc_log_carriers_size;
        /* Update log buffer pointer */

      /* Copy frequency entries info into the log record */
      for (i = 0; i < num_freq_entry; i++)
      {
        memcpy( (void *)ptr,
                (void *)( &afc_log_ptr->entries[i] ),
                mc_log_entry_size );

        ptr += mc_log_entry_size;
          /* Update log buffer pointer */
      }
      /*lint -restore */

      INTFREE( );

      /* submit the log to DIAG */
      log_commit( log_ptr );
    } /* if log_ptr */

  } /* if log_status */

  /* Reset the number of frequency entries */
  afc_log_ptr->afc_header_info.num_freq_entry = 0;

} /* tcxomgr_mc_flush_client_log */


/*============================================================================

FUNCTION TCXOMGR_FLUSH_CLIENT_LOG

DESCRIPTION
  This function sends the AFC log to diag if the log buffer is full or if
  logging client wanted it to be flushed because of AFC state change etc.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

============================================================================*/

void tcxomgr_flush_client_log
(
  tcxomgr_log_struct_type     *afc_log_ptr
)
{
  LOG_TCXOMGR_AFC_DATA_C_type *log_ptr;       /* Log buffer pointer */
  uint8                        i;             /* loop variable */
  uint32                       num_freq_entry;/* Number of frequency entries*/

  /*------------------------------------------------------------------------*/

  /* Get the current number of frequency entries */
  num_freq_entry = afc_log_ptr->afc_header_info.num_freq_entry;

  /* Log the packet if this log code is on and if there is an entry */
  if ( log_status(LOG_TCXOMGR_AFC_DATA_C) && ( num_freq_entry != 0 ))
  {
    log_ptr = (LOG_TCXOMGR_AFC_DATA_C_type*)
              log_alloc_ex( (log_code_type) LOG_TCXOMGR_AFC_DATA_C,
                TCXOMGR_VAR_SIZEOF(LOG_TCXOMGR_AFC_DATA_C_type,
                tcxomgr_log_entries_struct_type,
                afc_log_ptr->afc_header_info.num_freq_entry)
                );

    if ( log_ptr  != NULL )
    {
      INTLOCK( );

      /* Update the version number of the log packet */
      log_ptr->version = TCXOMGR_AFC_LOG_VERSION;

      /* Copy the header information */
      log_ptr->afc_header_info   = afc_log_ptr->afc_header_info;

      /* Copy frequency entries info into the log record */
      for (i = 0; i < num_freq_entry; i++)
      {
        memcpy( (void *)( &log_ptr->entries[i] ),
                (void *)( &afc_log_ptr->entries[i] ),
                sizeof (tcxomgr_log_entries_struct_type) );
      }

      INTFREE( );

      /* submit the log to DIAG */
      log_commit( log_ptr );
    } /* if log_ptr */

  } /* if log_status */

  /* Reset the number of frequency entries */
  afc_log_ptr->afc_header_info.num_freq_entry = 0;

} /* tcxomgr_flush_client_log */

/*============================================================================

FUNCTION TCXOMGR_LOG_ENTRY_UPDATED

DESCRIPTION
  This function is used by clients to indicate that they have updated
  one frequency entry to their AFC log buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the AFC buffer is full, the packet is flushed.

============================================================================*/

void tcxomgr_log_entry_updated
(
  tcxomgr_log_struct_type     *afc_log_ptr
)
{

/*------------------------------------------------------------------------*/

  if( afc_log_ptr->afc_header_info.num_freq_entry  == 1 )
  {
    /* First entry in this log pkt, so update controlling client id */
    afc_log_ptr->afc_header_info.ctrl_client_id =
                                   (uint8) tcxomgr.client_in_control;
  }
  else if( afc_log_ptr->afc_header_info.num_freq_entry >=
           TCXOMGR_MAX_FREQ_ENTRIES )
  {
    /* Client log buffer full - send AFC log packet to diag, start afresh. */
    tcxomgr_flush_client_log( afc_log_ptr );
  }
  else if( afc_log_ptr->afc_header_info.ctrl_client_id !=
           (uint8) tcxomgr.client_in_control )
  {
    DBG_2( ERROR, "Logging client changed:  old %d, new %d",
            afc_log_ptr->afc_header_info.ctrl_client_id,
            tcxomgr.client_in_control );

    /* Controlling tech changed - send AFC log packet to diag, start afresh. */
    tcxomgr_flush_client_log( afc_log_ptr );
  }

} /* tcxomgr_log_entry_updated */


/*============================================================================

FUNCTION TCXOMGR_MC_LOG_ENTRY_UPDATED

DESCRIPTION
  This function is used by clients to indicate that they have updated
  one frequency entry to their AFC log buffer.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  If the AFC buffer is full, the packet is flushed.

============================================================================*/

void tcxomgr_mc_log_entry_updated
(
  tcxomgr_mc_log_struct_type     *afc_log_ptr
)
{

/*------------------------------------------------------------------------*/

  if( afc_log_ptr->afc_header_info.num_freq_entry  == 1 )
  {
    /* First entry in this log pkt, so update controlling client id */
    afc_log_ptr->afc_header_info.ctrl_client_id =
                                   (uint8) tcxomgr.client_in_control;
  }
  else if( afc_log_ptr->afc_header_info.num_freq_entry >=
           TCXOMGR_MAX_FREQ_ENTRIES )
  {
    /* Client log buffer full - send AFC log packet to diag, start afresh. */
    tcxomgr_mc_flush_client_log( afc_log_ptr );
  }
  else if( afc_log_ptr->afc_header_info.ctrl_client_id !=
           (uint8) tcxomgr.client_in_control )
  {
    DBG_2( ERROR, "Logging client changed:  old %d, new %d",
            afc_log_ptr->afc_header_info.ctrl_client_id,
            tcxomgr.client_in_control );

    /* Controlling tech changed - send AFC log packet to diag, start afresh. */
    tcxomgr_mc_flush_client_log( afc_log_ptr );
  }

} /* tcxomgr_log_entry_updated */

#else /* FEATURE_TCXOMGR_DIAG not defined, add external declaration 
         to avoid compiler warnings */

int tcxomgr_no_external_declarations_in_tcxomgr_diag;

#endif /* FEATURE_TCXOMGR_DIAG */
