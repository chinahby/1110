#ifndef PMAPP_TIMER_H
#define PMAPP_TIMER_H

/*===========================================================================

        P M   A P P L I C A T I O N   T I M E R   D E F I N I T I O N 
                         H E A D E R   F I L E

DESCRIPTION
  This file contains generic timer type definitions for use by application code
  provided with the software library for the Qualcomm Power Management Chip Set.
  
Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/inc/pmapp_timer.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/03/05   Vish    Incorporated a wrapper around basic CLK/TIME services to
                   provide a mechanism for reliable detection/cancellation
                   of timeouts.
03/17/05   Vish    Created.
===========================================================================*/

/* =========================================================================
                         INCLUDE FILES
========================================================================= */
#include "comdef.h"

#ifdef FEATURE_USE_TIME_VU
#include "timer.h"
#else
#include "clk.h"
#endif //FEATURE_USE_TIME_VU


/* =========================================================================
                       MACROS / CONSTANTS
========================================================================= */


/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */
typedef uint16 pm_app_timer_sess_id_type;
typedef uint32 pm_app_timer_token_type;

struct pm_app_timer_struct_type;
typedef void (*pm_app_timer_cb_func_type) (
   struct pm_app_timer_struct_type    *timer_ptr,       // what timer
   pm_app_timer_sess_id_type           tmo_sess_id,     // ID of session that timed out
   int32                               delay);          // delay in ms

struct pm_app_timer_struct_type
{
#ifdef FEATURE_USE_TIME_VU
   timer_type                  __core_timer;
#else
   clk_cb_type                 __core_timer;
#endif //FEATURE_USE_TIME_VU
   pm_app_timer_token_type     __token;
   pm_app_timer_cb_func_type   __client_cb;
};
typedef struct pm_app_timer_struct_type pm_app_timer_type;



/* =========================================================================
                         FUNCTION PROTOTYPES
========================================================================= */
extern void pm_app_timer_def(
   pm_app_timer_type *timer_ptr);

extern pm_app_timer_sess_id_type pm_app_timer_start(
   pm_app_timer_type           *timer_ptr,
   pm_app_timer_cb_func_type    tmo_cb,
   int32                        tmo_duration);

extern void pm_app_timer_invalidate_sess_id(
   pm_app_timer_sess_id_type   *timer_sess_id_ptr);

extern void pm_app_timer_stop(
   pm_app_timer_type           *timer_ptr);


#endif // PMAPP_TIMER_H

