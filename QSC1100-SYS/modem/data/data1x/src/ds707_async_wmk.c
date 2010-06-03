/*===========================================================================
                         D S 7 0 7 _ A S Y N C _ W M K

DESCRIPTION
  This file contains functions and definitions related to watermarks used
  in IS707 circuit switched data calls (Async/Fax).

EXTERNALIZED FUNCTIONS

ds707_async_wmk_init
  This function initializes watermarks.

ds707_async_wmk_setup_um_wmk
  This function configures the Um watermark.

ds707_async_wmk_setup_sio_wmk
  This function configures the sio watermark.

 Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

    $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_async_wmk.c_v   1.5   02 Oct 2002 13:22:50   ajithp  $
    $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_async_wmk.c#2 $ $DateTime: 2009/01/29 05:35:22 $ $Author: nsivakum $
  
-----------------------------------------------------------------------------  
05/23/06    squ    Featurized async data
01/21/04    sv     Replaced all references to DEF_MSS with PS707_ASYNC_DEF_MSS
10/01/02    atp    Cleanup.
08/22/02    atp    Cleanup for shipping.
08/19/02    atp    Support for Um flow control for low-baud SIO links.
08/15/02    atp    Added support for mode-specific SIO wmks.
07/16/02    aku    changes to support ds_flow_control.h change.
07/15/02    atp    Added header.
07/13/02    atp    First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifndef FEATURE_ASYNC_DATA_NOOP
#ifdef FEATURE_DATA_IS707
#include "ds3gsiolib.h"
#include "ds707_async_defs.h"
#include "ds707_async_wmk.h"
#include "ds707_wmk.h"
#include "ps707_async.h"
#include "dsm.h"
#include "queue.h"
#include "rex.h"
#include "ps_svc.h"
#include "ps_tcp.h"


/*===========================================================================
                             EXTERNS     
===========================================================================*/


/*===========================================================================
                            VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
  SIO Tx watermark (from async to sio)
---------------------------------------------------------------------------*/
dsm_watermark_type ds707_wmk_async_to_sio_tx_wmk;
q_type             ds707_wmk_async_to_sio_tx_q;


/*===========================================================================
                        INTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_UM_TX_LO_WM_FUNC

DESCRIPTION   Called (via function pointer) when the tx watermark reg'd
              with RLP hits the low watermark event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_async_wmk_um_tx_lo_wm_func
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Enable flow from SIO.
  -------------------------------------------------------------------------*/
  ds3g_siolib_set_inbound_flow ( DS_FLOW_707_RLP_MASK, DS_FLOW_ENABLE );

} /* ds707_async_wmk_um_tx_lo_wm_func() */


/*===========================================================================
FUNCTION      DS707_WMK_UM_TX_HI_WM_FUNC

DESCRIPTION   Called (via function pointer) when the tx watermark reg'd
              with RLP hits the high watermark event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_async_wmk_um_tx_hi_wm_func
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Disable flow from SIO.
  -------------------------------------------------------------------------*/
  ds3g_siolib_set_inbound_flow ( DS_FLOW_707_RLP_MASK, DS_FLOW_DISABLE );

} /* ds707_async_wmk_um_tx_hi_wm_func() */


/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_TO_SIO_TX_LO_WM_FUNC

DESCRIPTION   Called (via function pointer) when the watermark hits the low
              watermark point.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_async_wmk_to_sio_tx_lo_wm_func
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ("Um flow enabled", 0, 0, 0);

  /*-------------------------------------------------------------------------
    Enable flow from Um.
  -------------------------------------------------------------------------*/
  ds707_async_state.um_flow_enable     = TRUE;
  ps_send_cmd ( PS_707_ASYNC_UM_FLOW_ENABLE_CMD, NULL );

} /* ds707_async_wmk_to_sio_tx_lo_wm_func() */


/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_TO_SIO_TX_HI_WM_FUNC

DESCRIPTION   Called (via function pointer) when the watermark hits the high
              watermark point.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_async_wmk_to_sio_tx_hi_wm_func
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_LOW ("Um flow disabled", 0, 0, 0);

  /*-------------------------------------------------------------------------
    Disable flow from Um.
  -------------------------------------------------------------------------*/
  ds707_async_state.um_flow_enable     = FALSE;

} /* ds707_async_wmk_to_sio_tx_hi_wm_func() */


/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_INIT

DESCRIPTION   Called at code startup (i.e, only once).  Sets up queues with
              watermarks, etc.

DEPENDENCIES  Called only when the phone powers up.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_wmk_init
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  INTLOCK();

  (void)q_init(&ds707_wmk_async_to_sio_tx_q);
  ds707_wmk_async_to_sio_tx_wmk.current_cnt = 0;
  ds707_wmk_async_to_sio_tx_wmk.q_ptr = &ds707_wmk_async_to_sio_tx_q;

  INTFREE();

} /* ds707_async_wmk_init() */

/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_SETUP_UM_WMK

DESCRIPTION   This function initializes the PPP to RLP tx watermark 
              to operate in 707 async mode. This is the same wm used in
              pkt mode. On the RLP rx side there's no wm, there's only a
              queue.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_wmk_setup_um_wmk
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Setup Um watermark for PPP to RLP SN/1X tx.
  -------------------------------------------------------------------------*/
  ds707_wmk_um_SN_rev_wmk.lo_watermark       = 1000;
  ds707_wmk_um_SN_rev_wmk.hi_watermark       = 2000;
  ds707_wmk_um_SN_rev_wmk.dont_exceed_cnt    = 0xFFFF;

  ds707_wmk_um_SN_rev_wmk.lowater_func_ptr = ds707_async_wmk_um_tx_lo_wm_func;
  ds707_wmk_um_SN_rev_wmk.hiwater_func_ptr = ds707_async_wmk_um_tx_hi_wm_func;

  ds707_wmk_um_SN_rev_wmk.gone_empty_func_ptr   = NULL;
  ds707_wmk_um_SN_rev_wmk.non_empty_func_ptr    = NULL;
  ds707_wmk_um_SN_rev_wmk.each_enqueue_func_ptr = NULL;

} /* ds707_async_setup_um_wmk() */


/*===========================================================================
FUNCTION      DS707_ASYNC_WMK_SETUP_SIO_WMK

DESCRIPTION   This function initializes the DS to SIO tx watermark 
              to operate in 707 async mode. This is the same wm used in
              pkt mode. On the SIO rx side there is no wm, instead the
              sio rx func is registered with SIO.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_async_wmk_setup_sio_wmk
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Setup SIO watermark for DS to SIO TX.
  -------------------------------------------------------------------------*/
  ds707_wmk_async_to_sio_tx_wmk.lo_watermark        = PS707_ASYNC_DEF_MSS + 3;
  ds707_wmk_async_to_sio_tx_wmk.hi_watermark        = 2 * PS707_ASYNC_DEF_MSS;
  ds707_wmk_async_to_sio_tx_wmk.dont_exceed_cnt     = 8 * PS707_ASYNC_DEF_MSS;

  /*-------------------------------------------------------------------------
    This WaterMark Queue is to be used for the purposes of SIO
    transmisssion.  
  -------------------------------------------------------------------------*/
  ds707_wmk_async_to_sio_tx_wmk.lowater_func_ptr
                                = ds707_async_wmk_to_sio_tx_lo_wm_func;
  ds707_wmk_async_to_sio_tx_wmk.hiwater_func_ptr
                                = ds707_async_wmk_to_sio_tx_hi_wm_func;

  ds707_wmk_async_to_sio_tx_wmk.gone_empty_func_ptr   = NULL;
  ds707_wmk_async_to_sio_tx_wmk.non_empty_func_ptr    = NULL;
  ds707_wmk_async_to_sio_tx_wmk.each_enqueue_func_ptr = NULL;

} /* ds707_async_wmk_setup_sio_wmk() */

#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_ASYNC_DATA_NOOP */

