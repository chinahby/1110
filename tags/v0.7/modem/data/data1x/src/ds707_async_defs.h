#ifndef DS707_ASYNC_DEFS_H
#define DS707_ASYNC_DEFS_H
/*===========================================================================

                        D S 7 0 7 _ A S Y N C _ D E F S
DESCRIPTION
  This file contains definitions of data structures, enumerations, etc 
  that are used in an IS707 circuit switched data call.

EXTERNALIZED FUNCTIONS

 Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE
    $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_async_defs.h_v   1.3   02 Oct 2002 13:22:16   ajithp  $
    $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_async_defs.h#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $

-----------------------------------------------------------------------------
10/01/02    atp    Cleanup.
08/22/02    atp    Cleanup for shipping.
08/19/02    atp    Support for Um flow control for low-baud SIO links.
07/15/02    atp    Added header.
07/13/02    atp    First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "cm.h"
#include "ds707.h"
#include "dsm.h"
#include "ps_iface.h"


/*===========================================================================
                            TYPEDEFS
===========================================================================*/


/*---------------------------------------------------------------------------
  Enum that represents the state of the async TCP connection from DS 
  point of view.
---------------------------------------------------------------------------*/
typedef enum
{
  DS707_ASYNC_PROTOCOL_CLOSED,
  DS707_ASYNC_PROTOCOL_OPENING,
  DS707_ASYNC_PROTOCOL_OPEN,
  DS707_ASYNC_PROTOCOL_CLOSING

} ds707_async_protocol_state_enum_type;


/*---------------------------------------------------------------------------
  Traffic channel state info.  Contains info specific to one CM call.
---------------------------------------------------------------------------*/
typedef struct
{
  ds707_so_type            so;                       /* SO of this call    */
  cm_call_id_type          call_id;                  /* CM call id         */
  dsm_watermark_type      *tx_watermark_ptr;         /* RLP Tx watermark   */
  void                    (*post_rx_func_ptr)(void); /* RLP Rx callback    */
  q_type                  *post_rx_q_ptr;            /* RLP Rx queue       */

} ds707_async_tc_state_type;

/*---------------------------------------------------------------------------
  Structure that holds overall state info for async call. Includes info
  about the traffic channel and the ps_iface.
---------------------------------------------------------------------------*/
typedef struct
{
  /*-------------------------------------------------------------------------
    The PS iface for IS-707 async calls.
  -------------------------------------------------------------------------*/
  ps_iface_type             ps_iface;
  /*-------------------------------------------------------------------------
    State of traffic channel.
  -------------------------------------------------------------------------*/
  ds707_async_tc_state_type tc_state;
  /*-------------------------------------------------------------------------
    State of async TCP connection.
  -------------------------------------------------------------------------*/
  ds707_async_protocol_state_enum_type protocol_state;
  /*-------------------------------------------------------------------------
    Semaphores to remember if NO CARRIER, OK were sent to the serial port.
  -------------------------------------------------------------------------*/
  boolean no_carrier_txed;
  boolean ok_result_txed;
  boolean ps_closed_properly;

  /*-------------------------------------------------------------------------
    For enabling/disabling Um flow. Applicable for low-baud SIO links.
  -------------------------------------------------------------------------*/
  boolean um_flow_enable;

} ds707_async_state_type;


/*===========================================================================
                            VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
  Structure that holds state and other info for async call.
---------------------------------------------------------------------------*/
extern ds707_async_state_type   ds707_async_state;

/*---------------------------------------------------------------------------
  Data types needed to communicate with SIO.
---------------------------------------------------------------------------*/
extern sio_ioctl_param_type ds707_async_sio_ioctl_param;


#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_ASYNC_DEFS_H   */
