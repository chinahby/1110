#ifndef SRCH_CGPS_H
#define SRCH_CGPS_H

/*===========================================================================

                S R C H _ C G P S    H E A D E R    F I L E

DESCRIPTION
    This module is the 1x L1 searcher to Converged GPS (CGPS) interface.
    This module includes both PPM and GPS utilities to collect data,
    start/stop actions and send data back to the CGPS module, achieved by
    sending an internal command handled by srchcom_sm.

  Copyright (c) 2007
                by QUALCOMM, Inc.  All Rights Reserved.
===========================================================================*/

/*=========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/srch_cgps.h#1 $

when       who     what, where, why
--------   ---     ------------------------------------------------------
08/22/07   tjc     Export a method for deleting outstanding 1x PPM requests
05/07/07   tjc     Add REQUEST_RF_INFO_CMD processing
02/15/07   rkc     Remove srch_cgps_enable_freq_est() and
                   srch_cgps_disable_freq_est()
01/18/07   rkc     Initial revision

==========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"
#include "qw.h"

#include "srchmc.h"


/*-------------------------------------------------------------------------
      Typedefs
-------------------------------------------------------------------------*/

/* 1x-PPM operation priorities. */
typedef enum
{
  OPERATION_NOT_REQUESTED,
  HIGH_PRIORITY,
  MEDIUM_PRIORITY,
  LOW_PRIORITY,
} srch_cgps_1x_ppm_operation_priorities_type;

/*-------------------------------------------------------------------------
      Other Exported Functions
-------------------------------------------------------------------------*/

/***************************************************************************/
/********** S R C H  -  P P M   I n t e r f a c e   F u n c t i o n s ******/
/***************************************************************************/

/*===========================================================================

FUNCTION       SRCH_CGPS_REQUEST_1X-PPM_OPERATION

DESCRIPTION    This function processes a request for 1x-PPM operation.
               Depending on the input priority, resources are allocated.
               The following is started: Sync80 data recording and
               Unsolicited Neighbor List Update.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Sync80 data recording and
               Unsolicited Neighbor List Update started

===========================================================================*/
extern void srch_cgps_request_1x_ppm_operation
(
  srch_cgps_1x_ppm_operation_priorities_type priority
);

/*===========================================================================

FUNCTION       SRCH_CGPS_RELEASE_1X-PPM_OPERATION

DESCRIPTION    This function processes a release for 1x-PPM operation.
               Release srch IRM resources.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   Sync80 data recording and
               Unsolicited Neighbor List Update stopped

===========================================================================*/
extern void srch_cgps_release_1x_ppm_operation( void );

/*===========================================================================

FUNCTION       SRCH_CGPS_DELETE_1X_PPM_OPERATION_REQUEST

DESCRIPTION    This function deletes any outstanding requests for 1x PPM
               operation.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_cgps_delete_1x_ppm_operation_request( void );

/*===========================================================================

FUNCTION       SRCH_CGPS_REQUEST_SYNC80_DATA

DESCRIPTION    This function handles the PPM request for sync80 data.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_cgps_request_sync80_data( void );

/*===========================================================================

FUNCTION       SRCH_CGPS_GET_SRCH_WINDOW_CENTER

DESCRIPTION    This function handles the PPM Search Window Center request.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_cgps_get_srch_window_center( void );

/*===========================================================================

FUNCTION       SRCH_CGPS_GET_REF_PN

DESCRIPTION    This function handles the PPM Reference PN request.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_cgps_get_ref_PN( void );

/*===========================================================================

FUNCTION       SRCH_CGPS_GET_ACTIVE_AND_CANDIDATE_SET

DESCRIPTION    This function handles the PPM active and candidate request.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_cgps_get_active_and_candidate_set( void );

/*===========================================================================

FUNCTION       SRCH_CGPS_PN_ROLL_ISR_HANDLER

DESCRIPTION    This function is called in srch_roll_common_isr() to
               perform PPM PN roll ISR processin.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_cgps_pn_roll_isr_handler ( void );

/***************************************************************************/
/********** S R C H  -  G P S   I n t e r f a c e   F u n c t i o n s ******/
/***************************************************************************/

/*===========================================================================

FUNCTION       SRCH_CGPS_ENABLE_TTD_RECORDING

DESCRIPTION    This function enables the Time Transfer Data (TTD) recording
               at each sync80 interrupt regardless if 1x srch is in service
               or not.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_cgps_enable_ttd_recording ( void );

/*===========================================================================

FUNCTION       SRCH_CGPS_DISABLE_TTD_RECORDING

DESCRIPTION    This function disables the Time Transfer Data (TTD) recording
               at each sync80 interrupt and marks the TTD data structure as
               invalid.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_cgps_disable_ttd_recording ( void );

/*===========================================================================

FUNCTION       SRCH_CGPS_REQUEST_SYS_TIME_UNCERTAINTY_EST

DESCRIPTION    This function handles the GPS request for System Time
               Uncertainty Estimate.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_cgps_request_sys_time_uncertainty_est ( void );

/*===========================================================================

FUNCTION       SRCH_CGPS_REQUEST_TTD

DESCRIPTION    This function handles the GPS request for Time Transfer Data.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_cgps_request_ttd ( void );

/*===========================================================================

FUNCTION       SRCH_CGPS_CANCEL_TTD_REQUEST

DESCRIPTION    This function cancels the GPS request for Time Transfer Data.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_cgps_cancel_ttd_request ( void );

/*===========================================================================

FUNCTION       SRCH_CGPS_REQUEST_RF_INFO

DESCRIPTION    This function handles the GPS request for RF info.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_cgps_request_rf_info ( void );

/*===========================================================================

FUNCTION       SRCH_CGPS_PN_ROLL_ISR_HANDLER

DESCRIPTION    This function is called in srch_roll_common_isr() for
               GPS PN roll ISR processing.

DEPENDENCIES   None

RETURN VALUE   None

SIDE EFFECTS   None

===========================================================================*/
extern void srch_cgps_pn_roll_isr_handler ( void );

#endif /* SRCH_CGPS_H */

