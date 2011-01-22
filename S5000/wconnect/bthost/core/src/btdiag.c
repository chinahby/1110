/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Bluetooth Diag Packet Processing

General Description
  Diagnostic packet processing routines for Bluetooth

Copyright (c) 2005-2008 by QUALCOMM Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/core/rel/00.00.26/src/btdiag.c#1 $
  $DateTime: 2009/01/07 18:14:54 $
  $Author: deepikas $

  when        who  what, where, why
  ----------  ---  -----------------------------------------------------------
  2008-03-27   pn  Fixed potential NULL pointer access.

 *
 *    #5         23 Apr 2006            SSK
 * Removing compiler warning in bt_diag_request_handler().
 *
 *    #4         23 May 2006            JTL
 * Featurizing for FEATURE_BT.
 *
 *    #3         25 Aug 2005            GS
 * Fixed bug in call to allocate diag delayed response packet.
 * 
 *    #2         02 Aug 2005            GS
 * Modified bt_diag_response_cb() so that all DSM items are sent in a 
 * single Diag response packet.
 *
 *    #1         25 Jul 2005            GS
 * Added support for HCI over DIAG (ASIA).

===========================================================================*/

#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_BT
#include "bt.h"
#ifdef FEATURE_FACTORY_TESTMODE
#include "btdiag.h"
#include "diagbuf.h"
#include "dsm.h"

/* -------------------------------------------------------------------------
** Definitions
** ------------------------------------------------------------------------- */
static uint16 delayed_rsp_id  = 0;
static uint16 subsys_cmd_code = 0;
static uint16 rsp_count       = 0;

LOCAL dsm_watermark_type* diag_to_bt_wm_ptr   = 0;
LOCAL dsm_watermark_type* diag_from_bt_wm_ptr = 0;

/* Diag request packet handler */
PACKED void * bt_diag_request_handler (PACKED void *, uint16);

/* The dispatch table for the Bluetooth subsystem */
static const diagpkt_user_table_entry_type bt_diag_tbl[] =
{
  {BT_DIAG_CMD_START,    BT_DIAG_CMD_END,    bt_diag_request_handler},
};  

/*===========================================================================

FUNCTION BT_DIAG_RESONSE_CB
DESCRIPTION
  Constructs diag delayed response packet and sends it out
============================================================================*/
void bt_diag_response_cb( void )
{
  dsm_item_type*    dsm_ptr;
  uint32            pkt_size = 0;
  bt_diag_rsp_type* bt_rsp_ptr;

  /* Get the sum total length of all dsm items on the wm */
  pkt_size = diag_from_bt_wm_ptr->current_cnt;

  /* Create diag response packet */
  bt_rsp_ptr = (bt_diag_rsp_type*) diagpkt_subsys_alloc_v2_delay(
                  DIAG_SUBSYS_BT,
                  subsys_cmd_code, 
                  delayed_rsp_id,
                  pkt_size + sizeof(diagpkt_subsys_header_v2_type) );

  if ( bt_rsp_ptr == NULL ) 
  {
    BT_ERR( "BT DIAG: Failed to create diag rsp pkt", 0, 0, 0 );
    return;
  }

  pkt_size = 0;

  /* Copy out dsm data on to Diag packet */
  dsm_ptr = dsm_dequeue( diag_from_bt_wm_ptr );
  while ( dsm_ptr != NULL ) 
  {
    pkt_size += 
      dsm_pullup(&dsm_ptr,
                 (void*) (bt_rsp_ptr->bt_diag_evt_msg + pkt_size),
                 dsm_length_packet( dsm_ptr ) );
           
    /* De-queue the next dsm item in the wm */
    dsm_ptr = dsm_dequeue( diag_from_bt_wm_ptr );
  }

  rsp_count = rsp_count + 1;

  /* Set response count (with sign bit set always => 'more to come') */
  diagpkt_subsys_set_rsp_cnt( bt_rsp_ptr, rsp_count ^ 0x8000 );

  /* Now, send it */
  diagpkt_delay_commit( bt_rsp_ptr );
}

/*===========================================================================

FUNCTION BT_DIAG_DISPATCH
DESCRIPTION
  Enqueues diag data onto the BT watermark.  BT task dequeues the data
  from the wm using the non-empty callback mechanism and feeds data
  into the BT HCI parsing engine.
============================================================================*/
void bt_diag_dispatch( PACKED byte* data, uint16 length )
{
  dsm_item_type* dsm_ptr;

  dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, length );

  (void) dsm_pushdown_tail( &dsm_ptr,
                            (const void*) data,
                            length,
                            DSM_DS_POOL_SIZE( length ) );

  dsm_enqueue( diag_to_bt_wm_ptr, &dsm_ptr );
}

/*===========================================================================

FUNCTION BT_DIAG_REQUEST_HANDLER
DESCRIPTION
  Handles all BT diag request packets. 
============================================================================*/
PACKED void *
bt_diag_request_handler (PACKED void *req_ptr, uint16 pkt_len)
{
  bt_diag_req_type* bt_req_ptr;
  bt_diag_rsp_type* bt_rsp_ptr;

  uint16 rsp_len = 0;

  if ( ( diag_to_bt_wm_ptr   == NULL ) || 
       ( diag_from_bt_wm_ptr == NULL ) )
  {
    (void)bt_cmd_dc_set_hci_mode( BT_HCIM_STANDARD_HCI_FTM );
    bt_cmd_dc_ftm_get_bt_wm( &diag_to_bt_wm_ptr, &diag_from_bt_wm_ptr );

    if ( ( diag_to_bt_wm_ptr   == NULL ) &&
         ( diag_from_bt_wm_ptr == NULL ) )
    {
      BT_ERR( "BT DIAG: Unable to get BT watermarks", 0, 0, 0 );
    }
    else if ( diag_from_bt_wm_ptr != NULL )
    {
      /* Register diag response handler callback */
      diag_from_bt_wm_ptr->non_empty_func_ptr = &bt_diag_response_cb; /*lint !e546*/
    }
  }

  rsp_len = sizeof (bt_diag_rsp_type);

  if (pkt_len < sizeof (bt_diag_req_type))
  {
    return diagpkt_err_rsp (DIAG_BAD_LEN_F, req_ptr, pkt_len);
  }

  bt_req_ptr = (bt_diag_req_type *) req_ptr;

  /* Save subsys command code info in request packet */
  subsys_cmd_code = diagpkt_subsys_get_cmd_code( req_ptr );

  /* Create the immediate response packet (even though it has no meaningful payload info) */
  bt_rsp_ptr = (bt_diag_rsp_type *)
                  diagpkt_subsys_alloc_v2 (DIAG_SUBSYS_BT,
                                           subsys_cmd_code,
                                           rsp_len);
  if (bt_rsp_ptr == NULL)
  {
    return NULL;
  }

  /* Save the delayed response id info (for future use, while
     sending delayed response packets) */
  delayed_rsp_id  = diagpkt_subsys_get_delayed_rsp_id( bt_rsp_ptr );

  /* Reset response count since this is a new 'transaction' */
  rsp_count       = 0;

  /* Now, send it */
  diagpkt_delay_commit( bt_rsp_ptr );

  /* Enqueue diag packet on the BT watermark (ignore if it is a 
    'dummy' request)  */
  if ( bt_req_ptr->bt_diag_cmd_msg[0] != 0x99 ) 
  {
    bt_diag_dispatch( bt_req_ptr->bt_diag_cmd_msg,
                      pkt_len - sizeof( diagpkt_subsys_header_type ) );
  }

  return bt_rsp_ptr;
}

/*===========================================================================

FUNCTION BT_DIAG_INIT
DESCRIPTION
  Initialization function for this module. Registers the packet handlers
  and sets up local data structures.
============================================================================*/
void bt_diag_init ( void )
{
  DIAGPKT_DISPATCH_TABLE_REGISTER_V2_DELAY (DIAG_SUBSYS_CMD_VER_2_F,
                                            DIAG_SUBSYS_BT, 
                                            bt_diag_tbl);

  return;
}

#endif /* FEATURE_FACTORY_TESTMODE */

#endif /* FEATURE_BT */

