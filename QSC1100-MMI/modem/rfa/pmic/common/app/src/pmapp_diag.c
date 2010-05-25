/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               Power Management Diagnostics Packet Processing

General Description
  Diagnostic packet processing routines for the power management driver.

Copyright (c) 2003,2004 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2005,2006 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/src/pmapp_diag.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/26/06   vk      Suppressed lint warnings
12/15/05   cng     Revised diag lib
08/25/05   fpe     Added diag lib.
04/05/04   rmd     Featurized PMIC DIAG services under 
                   "FEATURE_PMIC_DIAG_SERVICES".
03/24/04   rmd     Updated file to follow PMIC VU coding standards.
02/29/04   st      Created file.
===========================================================================*/

#ifdef __cplusplus
  extern "C" {
#endif

#include "customer.h"
#include "pm.h"
#include "pmapp_diag.h"

/* Tell lint that we are OK with the fact that pkt_len and req_ptr are not 
** used in some of the packets, as this is dictated by the awesome design */
/*lint -esym(715,pkt_len,req_ptr) */



/*===========================================================================

                   DIAG COMMAND FUNCTION DEFINITIONS

===========================================================================*/


/*===========================================================================

FUNCTION PM_APP_DIAG_VREG_SET

DESCRIPTION
  Processes a request to set the PM6650 MSMC VREG voltage.

===========================================================================*/
static PACKED void * pm_app_diag_vreg_set 
(
  PACKED void *                   req_pkt,
  uint16                          pkt_len       
)
{
  pm_err_flag_type                err_flag = PM_ERR_FLAG__SUCCESS;
  pm_app_diag_vreg_set_req_type * req_ptr = (pm_app_diag_vreg_set_req_type *) req_pkt;
  pm_app_diag_vreg_set_req_type * rsp_ptr = NULL;
  const int                       rsp_len = sizeof (pm_app_diag_vreg_set_req_type);
  
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /* Set the MSMC VREG voltage to the specified value. */

  err_flag = pm_vreg_set_level((pm_vreg_id_type)req_ptr->vreg_id, req_ptr->millivolts); 

  if (err_flag == PM_ERR_FLAG__SUCCESS) 
  {
        
    /* Allocate, initialize and return a response packet that is a copy of the
    request packet that was received. */

    rsp_ptr = (pm_app_diag_vreg_set_req_type *)
           diagpkt_subsys_alloc((uint8)DIAG_SUBSYS_PM, PM_APP_DIAG_VREG_SET_F, rsp_len );

    if( rsp_ptr != NULL ) 
    {
      rsp_ptr->vreg_id = req_ptr->vreg_id;
      rsp_ptr->millivolts = req_ptr->millivolts;
    }
  }
  else
  {
      MSG_ERROR("PMIC API ERROR DETECTED",0,0,0);
  }

  return ( rsp_ptr );

} /* pm_app_diag_msmc_vreg_set */


/*===========================================================================

FUNCTION PM_APP_DIAG_LIB_DIAG

DESCRIPTION
  This function is defined in the PMIC library and handles the DIAG 
  functionality in the PMIC library.

===========================================================================*/
static PACKED void * pm_app_diag_lib_diag
(
  PACKED void *                   req_pkt,
  uint16                          pkt_len       
)
{
    PACKED void* data = diagpkt_subsys_alloc( (uint8)DIAG_SUBSYS_PM, 1, 7 );

    pm_app_diag_lib_diag_i( req_pkt, data );

    return ( data );
}
////

/*===========================================================================

                  DIAG COMMAND FUNCTION DISPATCH TABLE

===========================================================================*/

static const diagpkt_user_table_entry_type pm_app_diag_tbl[] =
{
  {PM_APP_DIAG_VREG_SET_F, PM_APP_DIAG_VREG_SET_F, pm_app_diag_vreg_set},
  {PM_APP_DIAG_LIB_DIAG, PM_APP_DIAG_LIB_DIAG, pm_app_diag_lib_diag}
};



/*===========================================================================

FUNCTION PM_APP_DIAG_INIT

DESCRIPTION
  Register the diag packet function dispatch table.

===========================================================================*/
#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_PM, pm_app_diag_tbl);

#else

  void pm_app_diag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER ((uint8)DIAG_SUBSYS_PM, pm_app_diag_tbl);
  }

#endif


/* Restore error reporting for these symbols */
/*lint +esym(715,pkt_len,req_ptr) */
#ifdef __cplusplus
  }
#endif

