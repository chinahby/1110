/*****************************************************************************
***
***  TITLE
***  MT OBEX OPP API Header
***
***
***  DESCRIPTION
***  This contains the Interface to a Multi Transport AMSS OBEX OPP
***  Implementation.
***
***
***  Copyright (c) 2004 QUALCOMM UK Limited.
***  All Rights Reserved. QUALCOMM Proprietary.
***
***  Export of this technology or software is regulated by the U.S.
***  Government. Diversion contrary to U.S. law prohibited.
***
*****************************************************************************/


/*****************************************************************************
***
***                        EDIT HISTORY FOR MODULE
***
***  This section contains comments describing changes made to the module.
***  Notice that changes are listed in reverse chronological order.
***
***  $Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/inc/mt_obex_opp.h#1 $
***
***  when       who     what, where, why
***  --------   ---     ----------------------------------------------------------
*** 01/17/2008  pg      Added empty line to end of file to resolve compile warning
***
***
***
*****************************************************************************/

#ifndef _MT_OBEX_OPP_H
#define _MT_OBEX_OPP_H

#ifdef FEATURE_BT
#ifdef FEATURE_BT_EXTPF_GOEP
#ifdef FEATURE_MT_OBEX

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/

#include "customer.h"
#include "comdef.h"
#include "bt.h"

#include "mt_obex.h"

/*****************************************************************************
***
***     Public Defines & Constants
***
*****************************************************************************/

#define MT_OBEX_MAX_SERVICE_NAME_LEN BT_SD_MAX_SERVICE_NAME_LEN
#define MT_OBEX_IRDA_SERVICE_STRING "IRDA"

/* event types re-named for the purposes of IOBEX in BREW */

/* event Names re-named for the purposes of IOBEX in BREW */


/*****************************************************************************
***
***     Public Data References
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Macros
***
*****************************************************************************/

/*****************************************************************************
***
***     Public Functions
***
*****************************************************************************/

/************************ Generic Functions *********************************/

/*===========================================================================

FUNCTION mt_obex_get_transports_supported

DESCRIPTION
  This function can be used by an application to find out what transports are
  supported for OPP to run on.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_tp_support_type mt_obex_opp_get_transports_supported(void);

/************************ client functions *********************************/

/*===========================================================================

FUNCTION mt_obex_opp_cli_connect

DESCRIPTION
  This command connects a client to an OPP OBEX server and thus begins an
  OBEX session.
  This command establishes a connection to an OPP OBEX server. For Bluetooth this
  includes setting up an RFCOMM connection to the OBEX server TBD. For IrDA
  this request potentially initiates device discovery and service discovery
  and establishes all necessary layers of connections (i.e. IrLAP/IrLMP/TinyTP).
  A ??? event will be generated to indicate if the connection
  is successful.

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_opp_cli_connect(
    mt_obex_tp_conn_struct_type          *transport_params,
    mt_obex_app_id_type                  app_id );



/************************ server functions *********************************/

/*===========================================================================

FUNCTION mt_obex_opp_srv_register

DESCRIPTION
  Register an OPP OBEX server. This command registers an OBEX server to accept
  push or pull requests from OPP clients.

//TODO: need a way to pass the supported transport to btpf opp and onto
//lower layers, so that transport layer can do what is needed. (sio_open etc)
//the bt_pf_opp_srv_cmd_register() does not accept supported_transports as
//an arg..

DEPENDENCIES
  None

RETURN VALUE
  General Status of the command that has been issued to OBEX.

SIDE EFFECTS
  None

===========================================================================*/
mt_obex_status_type mt_obex_opp_srv_register(
    mt_obex_app_id_type                  app_id,
    uint32                               supported_formats,
    char*                                srv_name,
    mt_obex_tp_config_type               supported_transports );



#endif /* FEATURE_MT_OBEX */
#endif /* FEATURE_BT_EXTPF_GOEP */
#endif /* FEATURE_BT */

#endif /* _MT_OBEX_H */

