#ifndef PMAPP_IRQ_DEBOUNCER_H
#define PMAPP_IRQ_DEBOUNCER_H

/*===========================================================================

            P M   I R Q   D E B O U N C E R   A P P L I C A T I O N
                           H E A D E R   F I L E

DESCRIPTION
  This file contains function prototypes and variable/type/constant 
  declarations for the interrupt line debouncer developed for the 
  Qualcomm Power Management Chip Set.
  
Copyright (c) 2003, 2004, 2005, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/rfa/pmic/common/main/latest/app/inc/pmapp_irq_debouncer.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/31/06   Vish    Fixed LINT warnings.
03/17/06   Vish    Added two fields - irq_service__initialized and irq_service__en_irq
                   to pm_app_irq_debouncer_instance_type in support of IRQ
                   service manager that would handle enabling, disabling,
                   and clearing of IRQs.
03/03/06   Vish    Added 'tmr__both_irq_off' and 'tmr__comp_irq_on' fields to 
                   pm_app_irq_debouncer_instance_type and made them configurable
                   through pm_app_irq_debouncer_init().
12/12/05   Vish    Made cached_evt_handler_stack_depth a volatile field in
                   pm_app_irq_debouncer_instance_type.
09/03/05   Vish    1. Re-architected code to handle unexpected pre-emptions in
                      the debouncer state machine.
                   2. Modified the signature of 'pm_app_irq_debouncer_delegate_fn1_type'.
03/17/05   Vish    Changed the way timer is used by employing target
                   independent timer type as defined in "pmapp_timer.h".
11/20/04   Vish    Added comment header for pm_app_irq_debouncer_reset().
08/04/04   Vish    Added pm_app_irq_debouncer_get_status() for getting the
                   current state of the debounced IRQ.
11/26/03   Vish    Added documentation.
10/25/03   Vish    Created.
===========================================================================*/

/* =========================================================================
                         INCLUDE FILES
========================================================================= */
#include "pm.h"
#include "pmapp_timer.h"


/* =========================================================================
                       MACROS / CONSTANTS
========================================================================= */


/* =========================================================================
                         TYPE DEFINITIONS
========================================================================= */
typedef enum
{
   PM_APP_IRQ_DEBOUNCER_STATUS__HIGH,
   PM_APP_IRQ_DEBOUNCER_STATUS__LOW,
   PM_APP_IRQ_DEBOUNCER_STATUS__UNKNOWN
} pm_app_irq_debouncer_status_type;

// This notification fn is used to let the user of this debouncer know
// that a new "debounced" interrupt has happened and that the user can
// do his own processing to act on that interrupt.
//
//       !!!!   !!!!   !!!!  IMPORTANT  !!!!   !!!!   !!!!
//
// It is extremely important to note that the user shall not perform
// IRQ level functionalities like clearing/masking/unmasking the interrupt
// because interrupt management is completely taken care of by this
// debouncer. The user is expected to perform only user level functionalities
// like what it means to get an interrupt of the specified type.
typedef void (*pm_app_irq_debouncer_process_irq_cb) (pm_app_irq_debouncer_status_type);


// User of this debouncer must create
//  - 1 function of pm_app_irq_debouncer_delegate_fn1_type
//  - 1 function of pm_app_irq_debouncer_delegate_fn2_type
//
//  Implementation of pm_app_irq_debouncer_delegate_fn1_type fn should look like
//     void my_delegate_fn1(pm_app_timer_type *param1_ptr, pm_app_timer_sess_id_type param2, int32 param3)
//     {
//        pm_app_irq_debouncer_process_delegate_fn1(
//           &my_debouncer,
//           my_delegate_fn1,
//           param1_ptr,
//           param2,
//           param3);
//     }
//
//  Implementation of pm_app_irq_debouncer_delegate_fn2_type fn should look like
//     void my_delegate_fn2(void)
//     {
//        pm_app_irq_debouncer_process_delegate_fn2(
//           &my_debouncer,
//           my_delegate_fn2);
//     }

typedef void (*pm_app_irq_debouncer_delegate_fn1_type) (
   pm_app_timer_type                  *param1_ptr,
   pm_app_timer_sess_id_type           param2,
   int32                               param3);
typedef void (*pm_app_irq_debouncer_delegate_fn2_type) (void);


// None of the members shall be modified by the user of this
// debouncer; the user is expected to only create an instance
// of this struct and use it as the argument while invoking
// the debouncer fns below ...
typedef struct
{
   // Statically initialized variables (variables that are initialized only once)
   uint32                                            debouncer_init_magic_pattern__static;
   struct pm_app_irq_debouncer_state_type           *current_state_ptr;
   boolean                                           reset_request_pending;
   //    Event processing vars that help in properly executing state
   //    transitions if events happen to occur in a nested fashion
   volatile uint8                                    cached_evt_handler_stack_depth;

   // Dynamically initialized variables (everytime there is an initialization op)
   uint32                                   debouncer_init_magic_pattern__dynamic;
   pm_irq_hdl_type                          re_irq;
   pm_irq_hdl_type                          fe_irq;
   pm_rt_status_type                        rt_irq;
   int32                                    tmr__both_irq_off;
   int32                                    tmr__comp_irq_on;
   pm_app_irq_debouncer_process_irq_cb      process_irq_cb;
   pm_app_irq_debouncer_delegate_fn1_type   delegate_fn1;
   pm_app_irq_debouncer_delegate_fn2_type   delegate_fn2;
   pm_app_irq_debouncer_status_type         status;
   pm_app_timer_type                        timer;
   pm_app_timer_sess_id_type                curr_timer_sess_id;
   boolean                                  tmo_occured;
   boolean                                  irq_occured;
   // IRQ service internal vars
   boolean                                  irq_service__initialized;
   pm_irq_hdl_type                          irq_service__en_irq;
} pm_app_irq_debouncer_instance_type;


/* =========================================================================
                         FUNCTION PROTOTYPES
========================================================================= */

/*===========================================================================
FUNCTION  pm_app_irq_debouncer_init                        EXTERNAL FUNCTION

DESCRIPTION
   This function initializes and sets up the specified IRQ debouncer for
   detection and notification of true state of any IRQ line. 

PARAMETERS
   1) Name: debouncer_ptr
      - Pointer to the debouncer object. User needs to create a debouncer for
        each IRQ line and pass it to this fn for initialization.
      Type: pm_app_irq_debouncer_instance_type*
           
   2) Name: re_irq
      - Raising-edge IRQ, if it is an EDGE triggered interrupt, e.g. PM_USB_BUS_VALID_RE_IRQ_HDL
      - For LEVEL triggered interrupt, use the one that indicates the
        positive/high/valid state of the interrupt, e.g. PM_VALID_CHG_IRQ_HDL
      Type: pm_irq_hdl_type

   3) Name: fe_irq
      - Falling-edge IRQ, if it is an EDGE triggered interrupt, e.g. PM_USB_BUS_VALID_FE_IRQ_HDL
      - For LEVEL triggered interrupt, use the one that indicates the
        negative/low/invalid state of the interrupt, e.g. PM_INVALID_CHG_IRQ_HDL
      Type: pm_irq_hdl_type

   4) Name: rt_irq
      - Real-time state of the IRQ line for EDGE triggered interrupts, e.g. PM_USB_BUS_VALID_RT_ST
      - For LEVEL triggered interrupt, use the one that corresponds to the
        positive/high/valid state of the interrupt, e.g. PM_VALID_CHG_RT_ST
      Type: pm_rt_status_type

   5) Name: process_irq_cb
      - User fn that is to be called back whenever the debouncer detects a new "debounced"
        interrupt on the specified IRQ line.
      Type: pm_app_irq_debouncer_process_irq_cb
         
   6) Name: delegate_fn1
      - A function that has to be defined by the user for each debouncer object.
        Please refer to the type definition above for details on how to code this fn.
      Type: pm_app_irq_debouncer_delegate_fn1_type

   7) Name: delegate_fn2
      - A function that has to be defined by the user for each debouncer object.
        Please refer to the type definition above for details on how to code this fn.
      Type: pm_app_irq_debouncer_delegate_fn2_type

RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS           -->  Operation succeeded

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked during the execution of this function.
===========================================================================*/
extern pm_err_flag_type pm_app_irq_debouncer_init(
   pm_app_irq_debouncer_instance_type      *debouncer_ptr,
   pm_irq_hdl_type                          re_irq,
   pm_irq_hdl_type                          fe_irq,
   pm_rt_status_type                        rt_irq,
   int32                                    tmr__both_irq_off,
   int32                                    tmr__comp_irq_on,
   pm_app_irq_debouncer_process_irq_cb      process_irq_cb,
   pm_app_irq_debouncer_delegate_fn1_type   delegate_fn1,
   pm_app_irq_debouncer_delegate_fn2_type   delegate_fn2);


/*===========================================================================
FUNCTION  pm_app_irq_debouncer_get_status                  EXTERNAL FUNCTION

DESCRIPTION
   This function returns the current status of the IRQ line serviced by the
   indicated IRQ debouncer. 

PARAMETERS
   1) Name: debouncer_ptr
      - Pointer to the debouncer object.
      Type: pm_app_irq_debouncer_instance_type*
           
RETURN VALUE
   Type: pm_app_irq_debouncer_status_type
   - Possible values:
     - PM_APP_IRQ_DEBOUNCER_STATUS__HIGH     -->  IRQ happened (debounced status)
     - PM_APP_IRQ_DEBOUNCER_STATUS__LOW      -->  IRQ gone     (debounced status)
     - PM_APP_IRQ_DEBOUNCER_STATUS__UNKNOWN  -->  IRQ debounce in progress
                                                     or NULL debouncer_ptr

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()
   3) pm_app_irq_debouncer_init()

SIDE EFFECTS
   None.
===========================================================================*/
extern pm_app_irq_debouncer_status_type pm_app_irq_debouncer_get_status(
   const pm_app_irq_debouncer_instance_type      *debouncer_ptr);


/*===========================================================================
FUNCTION  pm_app_irq_debouncer_reset                       EXTERNAL FUNCTION

DESCRIPTION
   This function stops the debouncer state machine in its tracks and prevents
   any future notifications of "debounced" IRQ state.
   To restart the debouncer, call pm_app_irq_debouncer_init() again. 

PARAMETERS
   1) Name: debouncer_ptr
      - Pointer to the debouncer that needs to be reset.
      Type: pm_app_irq_debouncer_instance_type*
           
RETURN VALUE
   Type: pm_err_flag_type
   - Possible values:
     - PM_ERR_FLAG__SUCCESS           -->  Operation succeeded

DEPENDENCIES
   The following function(s) must have been called:
   1) rflib_init()
   2) pm_init()

SIDE EFFECTS
   Interrupts are locked during the execution of this function.
===========================================================================*/
extern pm_err_flag_type pm_app_irq_debouncer_reset(
   pm_app_irq_debouncer_instance_type     *debouncer_ptr);


extern void pm_app_irq_debouncer_process_delegate_fn1(
   pm_app_irq_debouncer_instance_type     *debouncer_ptr,
   pm_app_irq_debouncer_delegate_fn1_type  delegate_fn,
   pm_app_timer_type                      *param1_ptr,
   pm_app_timer_sess_id_type               param2,
   int32                                   param3);

extern void pm_app_irq_debouncer_process_delegate_fn2(
   pm_app_irq_debouncer_instance_type     *debouncer_ptr,
   pm_app_irq_debouncer_delegate_fn2_type  delegate_fn);

#endif // PMAPP_IRQ_DEBOUNCER_H
