/*****************************************************************************
***
***  TITLE
***  MT OBEX OPP API Implementation
***
***
***  DESCRIPTION
***  This contains the Implementation of the Multi Transport AMSS OBEX.
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
***  $Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/mt_obex_opp.c#1 $
***
***  when       who     what, where, why
***  --------   ---     ----------------------------------------------------------
***  17/01/08   PG      Resolved Compiler Warnings
***  02/23/05   sl      Added MT_OPP API
***  03/17/05   ABu     Took out BT sending a specific channel bit set in the 
***                     mt_obex_opp_cli_connect() function
*****************************************************************************/

/*****************************************************************************
***
***     Included Files
***
*****************************************************************************/



#include "target.h"
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT_EXTPF_OPP
#ifdef FEATURE_MT_OBEX

#include "mt_obex.h"
#include "mt_obex_opp.h"
#include <string.h> /* for memcpy */

/*****************************************************************************
***
***     Private Constants & Defines
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Typedefs
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Data Declarations
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Function Prototypes
***
*****************************************************************************/


/*****************************************************************************
***
***     Public Data Declarations
***
***     Ensure an external reference is added to the module's header file!
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Macros
***
*****************************************************************************/


/*****************************************************************************
***
***     Private Functions
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
mt_obex_tp_support_type mt_obex_opp_get_transports_supported(void)
{
    /* change this code if different transports need to be supported, this
       will only need to change most likely on a project by project basis */
    return (MT_OBEX_BLUETOOTH_TP | MT_OBEX_IRDA_TP);
}


/************************ client functions *********************************/

/*===========================================================================

FUNCTION mt_obex_cli_connect

DESCRIPTION
  This command connects a client to an OBEX server and thus begins an OBEX session.
  This command establishes a connection to an OBEX server. For Bluetooth this
  includes setting up an RFCOMM connection to the OBEX server TBD. For IrDA
  this request potentially initiates device discovery and service discovery
  and establishes all necessary layers of connections (i.e. IrLAP/IrLMP/TinyTP).
  A MT_OBEX_EV_CLI_CON_CFM event will be generated to indicate if the connection
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
    mt_obex_app_id_type                  app_id )
{
    uint8 transport_id = 0;

    switch(transport_params->transport)
    {
        case MT_OBEX_BLUETOOTH_TRANSPORT:
            /* transport_id = MT_OBEX_BLUETOOTH_TP_ID;
               BT will work OK without setting the channel to a particular value
               as BT is the default transport of operation and if no other channel
               bitmasks are set then BT will work fine
               transport_params->transport_conn.bt_conn.channel =
               (transport_params->transport_conn.bt_conn.channel | transport_id);*/
            return ( (mt_obex_status_type)bt_cmd_pf_opp_cli_connect(app_id,
                     transport_params->transport_conn.bt_conn.bd_addr_ptr,
                     transport_params->transport_conn.bt_conn.channel) );

        case MT_OBEX_IRDA_TRANSPORT:
            transport_id = MT_OBEX_IRDA_TP_ID;
            return ( (mt_obex_status_type)bt_cmd_pf_opp_cli_connect(app_id, NULL, transport_id) );

        default:
            return ( (mt_obex_status_type)bt_cmd_pf_opp_cli_connect(app_id,
                     transport_params->transport_conn.bt_conn.bd_addr_ptr,
                     transport_params->transport_conn.bt_conn.channel) );
    }
}

/************************ server functions *********************************/

/*===========================================================================

FUNCTION mt_obex_srv_register

DESCRIPTION
  Register an OPP server. This command registers an OPP server to accept put
  and/or get requests from OPP clients. This command is returned immediately
  and is not placed on the command queue.

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
    mt_obex_tp_config_type               supported_transports )
{
    char  service_name_str[ MT_OBEX_MAX_SERVICE_NAME_LEN + 1 ];

    if(supported_transports == NULL)
    {
        return MT_OBEX_UNSUPPORTED_TP;
    }

    if( (supported_transports & MT_OBEX_USB_TP) )
    {
        /* check whether this transport is supported */
        if( !(mt_obex_opp_get_transports_supported() & MT_OBEX_USB_TP) )
        {
            return MT_OBEX_UNSUPPORTED_TP;
        }
    }
    else if( (supported_transports & MT_OBEX_IRDA_TP) )
    {
        /* check whether this transport is supported */
        if( !(mt_obex_get_transports_supported() & MT_OBEX_IRDA_TP) )
        {
            return MT_OBEX_UNSUPPORTED_TP;
        }
        /* IrDA only OBEX server */
        if(supported_transports == MT_OBEX_IRDA_TP)
        {
            /* tell the lower layers that this is an IrDA OBEX server */
            memcpy( service_name_str, MT_OBEX_IRDA_SERVICE_STRING,
                                      strlen(MT_OBEX_IRDA_SERVICE_STRING));
            srv_name = service_name_str;
        }
    }
    else if( (supported_transports & MT_OBEX_RS232_TP) )
    {
        /* check whether this transport is supported */
        if( !(mt_obex_get_transports_supported() & MT_OBEX_RS232_TP) )
        {
            return MT_OBEX_UNSUPPORTED_TP;
        }
    }

    else if( (supported_transports & MT_OBEX_BLUETOOTH_TP) )
    {
        if( !(mt_obex_get_transports_supported() & MT_OBEX_BLUETOOTH_TP) )
        {
            return MT_OBEX_UNSUPPORTED_TP;
        }
    }

    else
        return MT_OBEX_UNSUPPORTED_TP;

    return ( (mt_obex_status_type)bt_cmd_pf_opp_srv_register(app_id, supported_formats,
                                                srv_name) );
}

#endif /* FEATURE_MT_OBEX */
#endif /* FEATURE_BT_EXTPF_OPP */
