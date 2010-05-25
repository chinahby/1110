/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   AutoConfig Diagnostic Interface 

        Supports automated configuration of serial ports for DIAG

Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

#ifdef __cplusplus
  extern "C" {
#endif

/*===========================================================================

                           Edit History
                           
 $Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/autoconfigdiag.c#4 $
 
when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/08   vg      Updated code to use PACK() vs. PACKED
10/31/05   as      Fixed lint errors.
03/05/05   sl      Corrected BT Featurization under FEATURE_DIAG_MULTI_PORTS
02/09/05   abu     Added support for Diag over IrDA under FEATURE_IRDA flag  

===========================================================================*/

#ifdef FEATURE_DIAG_MULTI_PORTS
#include "customer.h"
#include "autoconfigdiag.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "diagbuf.h"
#include "diagcomm.h"
#include "sio.h"

/*==========================================================================

PACKET   AUTOCONFIG_RELEASE_PORT

PURPOSE  Request sent from the DM to the DMSS to release the active DIAG port

RETURN VALUE
         Pointer to response packet.

============================================================================*/

PACK(void *) 
autoconfigdiag_release_port (PACK(void *) req_pkt, uint16 pkt_len)
{
  autoconfig_diag_release_port_req_type *req_ptr = 
      (autoconfig_diag_release_port_req_type *) req_pkt;
  autoconfig_diag_release_port_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(autoconfig_diag_release_port_rsp_type);

  /* Allocate the memory for this unused response item. */
  rsp_ptr = (autoconfig_diag_release_port_rsp_type *)diagpkt_subsys_alloc(
                                 DIAG_SUBSYS_AUTOCONFIG, 
                                 AUTOCONFIG_DIAG_RELEASE_PORT_F,
                                 rsp_len);
  
  if(rsp_ptr != NULL) {
      diagpkt_commit(rsp_ptr);
      diagbuf_flush();
      diagcomm_reset_sio();
      rsp_ptr = NULL;
  }
  return rsp_ptr ;
}


/*==========================================================================

PACKET   AUTOCONFIG_SWITCH_PORT

PURPOSE  Request send from the DM to DMSS to switch the active port to an
         alternative port

RETURN VALUE
         Pointer to response packet (NULL).

============================================================================*/
PACK(void *) 
autoconfigdiag_switch_port (PACK(void *) req_pkt, uint16 pkt_len)
{
  autoconfig_diag_switch_port_req_type *req_ptr = 
        (autoconfig_diag_switch_port_req_type *) req_pkt;
  autoconfig_diag_switch_port_rsp_type *rsp_ptr;

  const int rsp_len = sizeof(autoconfig_diag_switch_port_rsp_type);

  /* Allocate the memory for this unused response item. */
  rsp_ptr = (autoconfig_diag_switch_port_rsp_type *)diagpkt_subsys_alloc(
                               DIAG_SUBSYS_AUTOCONFIG, 
                               AUTOCONFIG_DIAG_SWITCH_PORT_F,
                               rsp_len);

  if(rsp_ptr != NULL) 
  {
    rsp_ptr->status = 1;

    switch(req_ptr->diagcomm_port) 
    {
      case SIO_PORT_UART_MAIN:
        if(diagcomm_get_multi_sio_stream_id(PORT_UART1) != SIO_NO_STREAM_ID) 
        { 
          rsp_ptr->diagcomm_port = req_ptr->diagcomm_port;
          rsp_ptr->status = 0;
          diagpkt_commit(rsp_ptr);
          diagbuf_flush();
          diagcomm_set_sio_stream_id(PORT_UART1);
        }
        break;
      case SIO_PORT_USB_DIAG:
        if(diagcomm_get_multi_sio_stream_id(PORT_USB_DIAG) != SIO_NO_STREAM_ID) 
        {
          rsp_ptr->diagcomm_port = req_ptr->diagcomm_port;
          rsp_ptr->status = 0;
          diagpkt_commit(rsp_ptr);
          diagbuf_flush();
          diagcomm_set_sio_stream_id(PORT_USB_DIAG);
        }
        break;
#if (defined(FEATURE_BT) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
      case SIO_PORT_BT_SPP:
        if(diagcomm_get_multi_sio_stream_id(PORT_BT_SPP) != SIO_NO_STREAM_ID) 
        {
          rsp_ptr->diagcomm_port = req_ptr->diagcomm_port;
          rsp_ptr->status = 0;
          diagpkt_commit(rsp_ptr);
          diagbuf_flush();
          diagcomm_set_sio_stream_id(PORT_BT_SPP);
        }
        break;
#endif
#if (defined(FEATURE_IRDA) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
#error code not present
#endif        
      default:
        break;
    } /* End of switch case */

    if(rsp_ptr->status)
      diagpkt_commit(rsp_ptr);
    rsp_ptr = NULL;
  }
  
  return rsp_ptr ;
}


/**************************************************************************
** AUTOCONFIG diag DISPATCH TABLE.
***************************************************************************/

static const diagpkt_user_table_entry_type autoconfigdiag_tbl[] =
{
  {AUTOCONFIG_DIAG_RELEASE_PORT_F, AUTOCONFIG_DIAG_RELEASE_PORT_F, autoconfigdiag_release_port},
  {AUTOCONFIG_DIAG_SWITCH_PORT_F, AUTOCONFIG_DIAG_SWITCH_PORT_F, autoconfigdiag_switch_port}
};

#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAG_SUBSYS_AUTOCONFIG, autoconfigdiag_tbl);

#else

  void autoconfigdiag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAG_SUBSYS_AUTOCONFIG, autoconfigdiag_tbl);
  }

#endif

#else

/* To supress warning: no external declaration in translation unit */
void autoconfigdiag_foo (void)
{
  return;
}

#endif /* FEATURE_DIAG_MULTI_PORTS */

#ifdef __cplusplus
  }
#endif
