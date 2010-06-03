/*===========================================================================

     S O C K E T   I N T E R F A C E   C A L L B A C K S   H D R   F I L E
                   
DESCRIPTION

 
EXTERNALIZED FUNCTIONS


Copyright (c) 2002 by QUALCOMM, Incorporated. All Rights Reserved.
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dssifcbacks.h_v   1.1   06 Oct 2002 23:51:00   akuzhiyi  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/dssifcbacks.h#1 $ $DateTime: 2007/11/05 03:25:37 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/06/02    aku    Added support for processing iface callbacks in PS task
                   context.
07/09/02    na    Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
                        DEFINES & CONSTANTS
---------------------------------------------------------------------------*/

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/
/*===========================================================================

FUNCTION DSSIFCB_INIT()

DESCRIPTION
  Called on initialization. 
  
  This registers all the callbacks with PSIFACE.

DEPENDENCIES
  None
  
RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void dssifcb_init
(
  void
);

/*===========================================================================

FUNCTION DSSIFCB_IP_ADDR_CHANGED_CBACK()

DESCRIPTION
  Called when IP addr is changed on any interface

DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.

===========================================================================*/
void dssifcb_ip_addr_changed_cback
(
  ps_cmd_enum_type             cmd_name,
  void                         *user_info_ptr
);

/*===========================================================================

FUNCTION DSSIFCB_FLOW_ENABLED_CBACK()

DESCRIPTION
  Posts signal to PS to process enabling of IFACE flow.

DEPENDENCIES
  None.

RETURN VALUE
  void

SIDE EFFECTS
  None.

===========================================================================*/
void dssifcb_flow_enabled_cback
(
  ps_iface_type             *this_iface_ptr,
  ps_iface_event_enum_type   event,
  ps_iface_event_info_u_type event_info,
  void                      *user_data_ptr
);

/*===========================================================================

FUNCTION DSSIFCB_PROCESS_FLOW_ENABLED_CBACK()

DESCRIPTION
  Called when flow is enabled on any interface

DEPENDENCIES
  None.

RETURN VALUE
  boolean

SIDE EFFECTS
  None.

===========================================================================*/
boolean dssifcb_process_flow_enabled_cback
(
  ps_sig_enum_type sig,                    /* Signal to be processed       */
  void *user_data_ptr                      /* Parameter carrying user data */
);

