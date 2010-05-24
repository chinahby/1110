/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*


           W I R E L E S S    M E S S A G I N G   S E R V I C E S
                    -- Client Group

GENERAL DESCRIPTION
  The WMS module which implements the User API for SMS. This source file
  implements the client group functions which process the client
  requests for client management.

Copyright (c) 2001, 2002, 2003, 2004, 2005, 2006, 2007, 2008
              by QUALCOMM INCORPORATED.
  All Rights Reserved.

Export of this technology or software is regulated by the U.S. Government.
Diversion contrary to U.S. law prohibited.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/* ^L<EJECT> */
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$PVCSPath:  L:/src/asw/MSM_MM/Services/wms/vcs/wmscl.c_v   1.3   17 Jun 2002 15:47:18   hqu  $
$Header: //source/qcom/qct/modem/wms/wms/rel/qsc1100_05.04.00/src/WMSE/wmscl.c#2 $ $DateTime: 2008/12/02 13:02:59 $ $Author: pyaramad $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/12/08   PMD     Removed wms_bc_*** APIs for CDMA Broadcast
01/31/08   PMD     Updated Copyright Information to include 2008
12/10/07   PMD     Initialized parse_msg_cb in wms_client_init()
03/30/07   HQ      Added support for Power Collapse/Power Management
                   for Dual CPU chipsets.
01/17/07   HQ      Updated copyright info to include 2007.
06/20/06   PMD     Removed wms_task_init_done variable and check.
05/02/06   HQ      Added check for wms_task_init_done flag.
01/25/06   VK      Updated Copyright Info
11/21/05   VK      Feature define fixes
10/20/05   VK      Updated copyright info
03/23/04   PMD     Changed Primary Client and Memory Status Implementation.
03/08/04   PMD     Added Support for SMMA Notification using Client Memory
07/29/03   HQ      Added support for FEATURE_BROADCAST_SMS_MULTIMODE:
                      - New BC_MM commands, events and structures.
                      - New BC_MM API functions.
                      - GSM CB support.
                      - 1x BC SMS is still supported via BC group, not BC_MM.
02/14/03   AP      Added support for client parsing.
11/13/02   AP      Deleted support for message info cache.
10/09/02   HQ      Added support of FEATURE_CDSMS_MSG_INFO_CACHE.
08/24/02   HQ      Added support of FEATURE_CDSMS_MSG_INFO_CACHE and
                   FEATURE_GWSMS_MSG_INFO_CACHE.
06/15/02   HQ      Do CFG activate client processing.
03/09/02   HQ      Added DC, BC and DBG groups for Multimode WMS
                   - added FEATURE_CDSMS.
12/13/01   HQ      Renamed UASMS to WMS.
12/07/01   HQ      Initial version for MSM_MM WMS.
08/27/01   HQ      Initial version for FEATURE_GWSMS.
===========================================================================*/


/*===========================================================================
========================  INCLUDE FILES =====================================
===========================================================================*/

#include "comdef.h"

#if defined(FEATURE_CDSMS) || defined(FEATURE_GWSMS)

#include "wmscl.h"
#include "wmscfg.h"


/*===========================================================================
======================== STATIC DATA ========================================
===========================================================================*/
static wms_client_s_type   clients[WMS_CLIENT_TYPE_MAX];


/*===========================================================================
============================ FUNCTIONS ======================================
===========================================================================*/

/*===========================================================================
FUNCTION wms_client_ptr

DESCRIPTION
  Return the pointer to client data of a client.

DEPENDENCIES
  none

RETURN VALUE
  Pointer to client data.

SIDE EFFECTS
  None.
===========================================================================*/
wms_client_s_type* wms_client_ptr
(
  wms_client_id_type client_id
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( client_id >= (uint8)WMS_CLIENT_TYPE_MAX ||
      clients[client_id].initialized == FALSE )
  {
    return NULL;
  }
  else
  {
    return &clients[client_id];
  }

} /* wms_client_ptr() */



/*=========================================================================
FUNCTION
  wms_client_init

DESCRIPTION
  Allow the client to register itself with the API.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data initialized for the new client.

=========================================================================*/
wms_client_err_e_type wms_client_init
(
  wms_client_type_e_type   client_type,
  wms_client_id_type       *client_id_ptr
)
{

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( client_type >= WMS_CLIENT_TYPE_MAX )
  {
    return WMS_CLIENT_ERR_CLIENT_TYPE;
  }

  if( client_id_ptr == NULL )
  {
    return WMS_CLIENT_ERR_CLIENT_ID_PTR;
  }

  if( clients[client_type].initialized == FALSE )
  {
    clients[client_type].initialized  = TRUE;
  }

  clients[client_type].activated    = FALSE;
  clients[client_type].cfg_event_cb = NULL;
  clients[client_type].msg_event_cb = NULL;
  clients[client_type].dc_event_cb  = NULL;
  clients[client_type].bc_mm_event_cb  = NULL;
  clients[client_type].dbg_event_cb = NULL;
  clients[client_type].parse_msg_cb = NULL;

#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  clients[client_type].bc_mm_event_cb  = NULL;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

  *client_id_ptr = (wms_client_id_type) client_type;

  return WMS_CLIENT_ERR_NONE;

} /* wms_client_init() */


/*=========================================================================
FUNCTION
  wms_client_init_for_pm

DESCRIPTION
  Allow the client to register itself with the API for the purpose of
  power management.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data initialized for the new client.

=========================================================================*/
wms_client_err_e_type wms_client_init_for_pm
(
  wms_client_type_e_type       client_type,
  wms_client_id_type           * client_id_ptr,
  wms_client_processor_e_type  processor
)
{
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  return WMS_CLIENT_ERR_UNSUPPORTED;
#endif /* FEATURE_APPS_POWER_COLLAPSE */

} /* wms_client_init_for_pm() */


/*=========================================================================
FUNCTION
  wms_client_release

DESCRIPTION
  Allow the client to release itself from the API.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data reset for this client.

=========================================================================*/
wms_client_err_e_type wms_client_release
(
  wms_client_id_type       client_id
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr(client_id);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( client_ptr == NULL )
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->initialized = FALSE;

  return WMS_CLIENT_ERR_NONE;

} /* wms_client_release() */


/*=========================================================================
FUNCTION
  wms_client_activate

DESCRIPTION
  The client tells the API that it is ready to use the services from the API.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data changed.

=========================================================================*/
wms_client_err_e_type wms_client_activate
(
  wms_client_id_type       client_id
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr(client_id);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( client_ptr == NULL )
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->activated = TRUE;

  wms_cfg_activate_client( client_id );

  return WMS_CLIENT_ERR_NONE;

} /* wms_client_activate() */


/*=========================================================================
FUNCTION
  wms_client_deactivate

DESCRIPTION
  The client tells the API that it is not ready to use the services.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data changed.

=========================================================================*/
wms_client_err_e_type wms_client_deactivate
(
  wms_client_id_type       client_id
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr(client_id);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( client_ptr == NULL )
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->activated = FALSE;

  return WMS_CLIENT_ERR_NONE;

} /* wms_client_deactivate() */


/*=========================================================================
FUNCTION
  wms_client_reg_cfg_cb

DESCRIPTION
  Allow the client to register its configuration event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_cfg_cb
(
  wms_client_id_type       client_id,
  wms_cfg_event_cb_type    cfg_event_cb
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr(client_id);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( client_ptr == NULL )
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->cfg_event_cb = cfg_event_cb;

  return WMS_CLIENT_ERR_NONE;

} /* wms_client_reg_cfg_cb() */

/*=========================================================================
FUNCTION
  wms_client_reg_cfg_cb_for_pm

DESCRIPTION
  Allow the client to register its configuration event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_cfg_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         cfg_notify_type,
  uint32                       cfg_event_mask,
  wms_cfg_event_cb_type        cfg_event_cb
)
{
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  return WMS_CLIENT_ERR_UNSUPPORTED;
#endif /* FEATURE_APPS_POWER_COLLAPSE */

} /* wms_client_reg_cfg_cb_for_pm() */


/*=========================================================================
FUNCTION
  wms_client_reg_msg_cb

DESCRIPTION
  Allow the client to register its message event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_msg_cb
(
  wms_client_id_type       client_id,
  wms_msg_event_cb_type    msg_event_cb
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr(client_id);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( client_ptr == NULL )
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->msg_event_cb = msg_event_cb;

  return WMS_CLIENT_ERR_NONE;

} /* wms_client_reg_msg_cb() */


/*=========================================================================
FUNCTION
  wms_client_reg_msg_cb_for_pm

DESCRIPTION
  Allow the client to register its Message event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_msg_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         msg_notify_type,
  uint32                       msg_event_mask,
  wms_msg_event_cb_type        msg_event_cb
)
{
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  return WMS_CLIENT_ERR_UNSUPPORTED;
#endif /* FEATURE_APPS_POWER_COLLAPSE */

} /* wms_client_reg_msg_cb_for_pm() */


/*=========================================================================
FUNCTION
  wms_client_reg_dc_cb

DESCRIPTION
  Allow the client to register its DC event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_dc_cb
(
  wms_client_id_type       client_id,
  wms_dc_event_cb_type     dc_event_cb
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr(client_id);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( client_ptr == NULL )
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->dc_event_cb = dc_event_cb;

  return WMS_CLIENT_ERR_NONE;

} /* wms_client_reg_dc_cb() */

/*=========================================================================
FUNCTION
  wms_client_reg_dc_cb_for_pm

DESCRIPTION
  Allow the client to register its DC event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_dc_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         dc_notify_type,
  uint32                       dc_event_mask,
  wms_dc_event_cb_type         dc_event_cb
)
{
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  return WMS_CLIENT_ERR_UNSUPPORTED;
#endif /* FEATURE_APPS_POWER_COLLAPSE */

} /* wms_client_reg_dc_cb_for_pm() */

/*=========================================================================
FUNCTION
  wms_client_reg_bc_mm_cb

DESCRIPTION
  Allow the client to register its Broadcast event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_bc_mm_cb
(
  wms_client_id_type       client_id,
  wms_bc_mm_event_cb_type     bc_mm_event_cb
)
{
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
  wms_client_s_type  *client_ptr = wms_client_ptr(client_id);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( client_ptr == NULL )
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->bc_mm_event_cb = bc_mm_event_cb;

  return WMS_CLIENT_ERR_NONE;

#else /* FEATURE_BROADCAST_SMS_MULTIMODE */

  return WMS_CLIENT_ERR_GENERAL ;

#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */

} /* wms_client_reg_bc_mm_cb() */


/*=========================================================================
FUNCTION
  wms_client_reg_bc_mm_cb_for_pm

DESCRIPTION
  Allow the client to register its BC_mm event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_bc_mm_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         bc_mm_notify_type,
  uint32                       bc_mm_event_mask,
  wms_bc_mm_event_cb_type      bc_mm_event_cb
)
{
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  return WMS_CLIENT_ERR_UNSUPPORTED;
#endif /* FEATURE_APPS_POWER_COLLAPSE */

} /* wms_client_reg_bc_mm_cb_for_pm() */



/*=========================================================================
FUNCTION
  wms_client_reg_dbg_cb

DESCRIPTION
  Allow the client to register its Broadcast event callback function.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_dbg_cb
(
  wms_client_id_type        client_id,
  wms_dbg_event_cb_type     dbg_event_cb
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr(client_id);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( client_ptr == NULL )
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->dbg_event_cb = dbg_event_cb;

  return WMS_CLIENT_ERR_NONE;

} /* wms_client_reg_dbg_cb() */


/*=========================================================================
FUNCTION
  wms_client_reg_dbg_cb_for_pm

DESCRIPTION
  Allow the client to register its Debug event callback function
  with event notification filtering preferences for the purpose of Power
  Management.
  If a null pointer is passed, the callback is de-registered.

DEPENDENCIES
  None

RETURN VALUE
  client error codes

SIDE EFFECTS
  Internal data updated.

=========================================================================*/
wms_client_err_e_type wms_client_reg_dbg_cb_for_pm
(
  wms_client_id_type           client_id,
  wms_pm_notify_e_type         dbg_notify_type,
  uint32                       dbg_event_mask,
  wms_dbg_event_cb_type        dbg_event_cb
)
{
#ifdef FEATURE_APPS_POWER_COLLAPSE
#error code not present
#else /* FEATURE_APPS_POWER_COLLAPSE */
  return WMS_CLIENT_ERR_UNSUPPORTED;
#endif /* FEATURE_APPS_POWER_COLLAPSE */

} /* wms_client_reg_dbg_cb_for_pm() */



/*=========================================================================
FUNCTION
  wms_reg_cl_parse_msg_cb

DESCRIPTION
  Allow the client to register its callback function for additional parsing
  of a message.

  If a null pointer is passed, the callback is de-registered. No parsing of the
  message will be administered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The message may not be shared amoung other clients, depending on the criteria
  of the parsing being administered to the message.
=========================================================================*/
wms_client_err_e_type wms_client_reg_parse_msg_cb
(
  wms_client_id_type               client_id,
  wms_cl_parse_msg_cb_type         msg_parsing_cb
)
{
  wms_client_s_type  *client_ptr = wms_client_ptr(client_id);

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( client_ptr == NULL )
  {
    return WMS_CLIENT_ERR_CLIENT_ID;
  }

  client_ptr->parse_msg_cb = msg_parsing_cb;

  return WMS_CLIENT_ERR_NONE;
} /* wms_reg_cl_parse_msg_cb */

/*=========================================================================
FUNCTION
  wms_cl_init

DESCRIPTION
  Initialize all client data.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Internal data initialized.

=========================================================================*/
void wms_cl_init
(
  void
)
{
  int i;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  for( i=0; i<(uint8)WMS_CLIENT_TYPE_MAX; i++ )
  {
    clients[i].initialized  = FALSE;
    clients[i].activated    = FALSE;

    clients[i].cfg_event_cb = NULL;
    clients[i].msg_event_cb = NULL;
    clients[i].dc_event_cb  = NULL;
    clients[i].bc_mm_event_cb  = NULL;
    clients[i].dbg_event_cb = NULL;
    clients[i].parse_msg_cb = NULL;
#ifdef FEATURE_BROADCAST_SMS_MULTIMODE
    clients[i].bc_mm_event_cb  = NULL;
#endif /* FEATURE_BROADCAST_SMS_MULTIMODE */
  }

  /* INTERNAL client is ready immediately.
  */
  clients[WMS_CLIENT_TYPE_INTERNAL].initialized = TRUE;
  clients[WMS_CLIENT_TYPE_INTERNAL].activated   = TRUE;

  /* done */
  return;

} /* wms_cl_init */

#endif /* FEATURE_CDSMS || FEATURE_GWSMS */

/* END of wmscl.c */
