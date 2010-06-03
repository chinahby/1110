#ifndef CMIPAPPPI_H
#define CMIPAPPPI_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/*===========================================================================

               C A L L   M A N A G E R   V o I P   A P P L I C A T I O N

                   I N T E R N A L  H E A D E R   F I L E

DESCRIPTION

   This header describes interfaces that are not exposed to IP apps but are
   internally available to Call Manager.


  Copyright (c) 2005 - 2006 by QUALCOMM INCORPORATED. All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.

===========================================================================*/



/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/modem/mmode/cm/rel/08H1/src/cmipappi.h#3 $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/10/09   sv      Lint cleanup
02/26/07   ka      Changes to support Voip deregistration.
06/27/06   ka      Adding feature flag FEATURE_IP_CALL
12/11/05   ic      Lint cleanup
10/08/05   ic      Added Header: field 
04/20/05   ka      Informing IP apps when there is a service change
04/05/06   ka      Adding new header functions find_active_app and get_cb_ptrs
03/14/05   dk/ka   Initial revision

===========================================================================*/

#if defined(FEATURE_IP_CALL)

#include "cmipapp.h"

/**--------------------------------------------------------------------------
** Forward declarations
** --------------------------------------------------------------------------
*/



/**--------------------------------------------------------------------------
** Defines
** --------------------------------------------------------------------------
*/


/**--------------------------------------------------------------------------
** Enums
** --------------------------------------------------------------------------
*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

/**--------------------------------------------------------------------------
** Datatypes
** --------------------------------------------------------------------------
*/

/**--------------------------------------------------------------------------
**   CM IP APP INTERFACE FUNCTIONS
** --------------------------------------------------------------------------
*/

/*===========================================================================

FUNCTION cmipapp_init

DESCRIPTION
  Initializes table to maintain app details. Reads phone identity

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmipapp_init(void);

/*===========================================================================

FUNCTION cmipapp_find_active_app

DESCRIPTION
  Finds an active app with a particular sys_mode and call type

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE

  CMIPAPP_STATUS_OK         - Deregistration Successful

SIDE EFFECTS
  None

===========================================================================*/
extern cmipapp_id_type cmipapp_find_active_app (

  sys_sys_mode_e_type     sys_mode,
    /* The system mode
    */

  cm_call_type_e_type     call_type
    /* call type
    */

);


/*===========================================================================

FUNCTION cmipapp_inform_srv_system_change

DESCRIPTION
  Clients that have a registered function for being informed of service
  change, get notified.

DEPENDENCIES
  CM must have already been initialized with
  cm_init_before_task_start() and cm_init_after_task_start().

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
extern void cmipapp_inform_srv_system_change (

   const cm_mm_ss_info_s_type *ss_info_ptr
     /* Serving system information */
);

/*===========================================================================

FUNCTION cmipapp_send_oprt_mode

DESCRIPTION
  Send Phone's operating mode to IP app

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern void cmipapp_send_oprt_mode
(
  sys_oprt_mode_e_type  oprt_mode
    /* Operating mode
    */
);


/*===========================================================================

FUNCTION cmipapp_is_wait_on_oprt_mode_cnf

DESCRIPTION
  Checks if ipapp would need to deregister on this request and
  also if inform_oprt_mode callback is registered.

DEPENDENCIES
  Call object must have been initialized with cmcall_init.

RETURN VALUE
  None.

SIDE EFFECTS
  Nothing.

===========================================================================*/
extern boolean cmipapp_is_wait_on_oprt_mode_cnf
(
  sys_oprt_mode_e_type  oprt_mode
    /* Operating mode
    */
);

/*===========================================================================

FUNCTION cmipapp_reset_trans_id

DESCRIPTION
  Reset trans id being used for tracking app's oprt mode cnf

DEPENDENCIES
  None

RETURN VALUE
  none

SIDE EFFECTS
  none

===========================================================================*/
extern void cmipapp_reset_trans_id (void);


#endif /* defined(FEATURE_IP_CALL) */

#endif /* CMIPAPPPI_H */
