#ifndef DS3GMGRINT_H
#define DS3GMGRINT_H
/*===========================================================================

               3 G   D A T A   S E R V I C E S   M A N A G E R

                   I N T E R N A L   H E A D E R   F I L E

DESCRIPTION
  This file contains definitions of 3G Dsmgr state variables, and some
  utility functions. This header file is intended to be used internal to the
  3G Dsmgr module only.


  Copyright (c) 2001 by QUALCOMM Incorporated. All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds3gmgrint.h_v   1.5   06 Nov 2002 10:47:16   trebman  $
  $Header: //source/qcom/qct/modem/data/common/dsmgr/main/lite/inc/ds3gmgrint.h#1 $ $DateTime: 2008/04/11 12:46:53 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/04/02   tmr     Added call_was_connected field to ds3gi_call_info_type
10/14/02   vsk     Added last_call_mode and last_call_type to track the 
                   previous calls mode & type : Reqd to retrieve stats for 
                   call which has ended
08/19/02   ak      Updated for multiple calls.
06/18/02   tmr     Added DS3G_WAIT_FOR_LL_DISCONNECT_STATE
06/05/02   ar      Removed online-command mode SIO watermark preservation 
                   variables.
05/21/02   smp     Replaced ds3g_mode_e_type with sys_sys_mode_e_type.
04/15/02   rc      Added ds_call_stack_e_type field to ds3gi_call_info_type
03/14/02   smp     Added end_result state variable to call_info.
02/26/02   tmr     Added DS3G_WAIT_FOR_LL_CONNECT_STATE
02/22/02   smp     Externalize variable to store mode-sp. handler
                   sio_rx_func_ptr.
02/19/02   rlp     Code review changes.
02/13/02   vsk     change call_id to be of cm_call_id_type
02/05/02   rlp     Added online-command mode support.
12/14/01   smp     Added state variables: ring_counter to call_info and
                   ri_asserted to serial_info.
08/17/01   smp     Renamed ATCOP files/functions/data types.
06/29/01   smp     Initial version.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "dsm.h"
#include "cm.h"
#include "ds3gmgr.h"


/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
  3G Dsmgr Call States
---------------------------------------------------------------------------*/ 
typedef enum
{
  DS3G_IDLE_CALL_STATE,                             /* No data call active */
  DS3G_ORIG_CALL_STATE,               /* Data call origination in progress */
  DS3G_CALL_ACTIVE_CALL_STATE,               /* Data call currently active */
  DS3G_RINGING_CALL_STATE,          /* User alert i.e. ringing in progress */
  DS3G_WAIT_FOR_CONN_CALL_STATE,    /* Waiting for MT call to be connected */
  DS3G_WAIT_FOR_LL_CONNECT_STATE,           /* In call connect delay state */
  DS3G_WAIT_FOR_LL_DISCONNECT_STATE,     /* In call disconnect delay state */
  DS3G_END_CALL_STATE,                          /* Waiting for call to end */
  DS3G_INVALID_CALL_STATE                       /* an Invalid call state   */
} ds3gi_call_state_e_type;

/*---------------------------------------------------------------------------
  Maximum number of calls in the system.  Currently support 1.
---------------------------------------------------------------------------*/
#define DS3GI_MAX_NUM_CALLS          1

#define DS3GI_DEFAULT_CALL_INSTANCE  0
#define DS3GI_INVALID_CALL_INSTANCE  0xFFFF

/*---------------------------------------------------------------------------
  Type for 3G Dsmgr call related information
---------------------------------------------------------------------------*/ 
typedef struct
{
  cm_call_id_type            call_id;      /* Call id assigned by Call Mgr */
  ds_call_type_e_type        call_type;    /* Call type - circuit or pkt   */
  ds3gi_call_state_e_type    call_state;   /* Call state                   */
  boolean                    call_was_connected; /* Call ever connected    */
} ds3gi_call_info_type;

/*---------------------------------------------------------------------------
  Table of function ptrs based on Mode (GSM/WCDMA/CDMA) and call type (pkt
  or circuit-switched)
---------------------------------------------------------------------------*/
extern ds3g_hdlr_func_tbl_type    *ds3gi_hdlr_tbl[SYS_SYS_MODE_MAX]
                                                 [DS_NUM_CALL_TYPES];

/*---------------------------------------------------------------------------
  Call state info.  
---------------------------------------------------------------------------*/
extern ds3gi_call_info_type   ds3gi_call_info[DS3GI_MAX_NUM_CALLS];

/*---------------------------------------------------------------------------
  The current operating mode.
---------------------------------------------------------------------------*/
extern sys_sys_mode_e_type    ds3gi_mode;

/*---------------------------------------------------------------------------
  Client id assigned by Call Manager.
---------------------------------------------------------------------------*/ 
extern cm_client_id_type      ds3gi_cm_client_id;

/*---------------------------------------------------------------------------
  The operating mode the for the previous call 
---------------------------------------------------------------------------*/ 
extern sys_sys_mode_e_type     ds3gi_last_call_mode; 

/*---------------------------------------------------------------------------
  The call type for the previous call 
---------------------------------------------------------------------------*/ 
extern ds_call_type_e_type     ds3gi_last_call_type;  /* circuit or packet */ 

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/
/*===========================================================================

FUNCTION DS3G_GET_CALL_STATE

DESCRIPTION
  This function returns the current call state.
  
DEPENDENCIES
  None

RETURN VALUE
  Returns the current call state.

SIDE EFFECTS
  None

===========================================================================*/

extern ds3gi_call_state_e_type  ds3g_get_call_state
(
  cm_call_id_type    cm_call_id
);



/*===========================================================================

FUNCTION DS3GI_FIND_EMPTY_CALL_STATE

DESCRIPTION
  Returns index to the first empty call state element.

DEPENDENCIES
  None
  
RETURN VALUE
  If found, returns the call instance index.  If not found, then returns 
  DS3GI_INVALID_CALL INSTANCE

SIDE EFFECTS
  None

===========================================================================*/
uint32 ds3gi_find_empty_call_state(void);


/*===========================================================================

FUNCTION DS3GI_FIND_CALL_INSTANCE

DESCRIPTION
  Given a call_id, will return the index into the call_state array.  If 
  no instance is found, then returns DS3GI_INVALID_CALL_INSTANCE

DEPENDENCIES
  None
  
RETURN VALUE
  If found, returns the call instance.  If not found, then returns 
  DS3GI_INVALID_CALL INSTANCE

SIDE EFFECTS
  None

===========================================================================*/
uint32 ds3gi_find_call_instance
(
  cm_call_id_type     cm_call_id
);
#endif /* FEATURE_DATA */

#endif /* DS3GMGRINT_H */
