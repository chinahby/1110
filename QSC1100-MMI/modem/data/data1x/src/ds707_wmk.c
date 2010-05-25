/*===========================================================================
                         D S 7 0 7 _ W M K
GENERAL DESCRIPTION
  This file contains the configuration for the watermarks which go between
  PPP and RLP.  Includes the functions called at particular watermark 
  events. 
  
  This is specific to packet data calls.

EXTERNALIZED FUNCTIONS
 DS707_WMK_UM_TX_LO_WM_FUNC
   Registered with watermark.  Called when low watermark setting reached.
   Enables flow control on the 1x ps_iface (basically allows PS to start
   sending data to RLP).
   
 DS707_WMK_UM_TX_HI_WM_FUNC
   Registered with watermark.  Called when hi watermark setting reached.
   Disables flow control on the 1x ps_iface (basically tells PS to stop
   sending data to RLP).
   
 DS707_WMK_INIT
   Called at startup.  Initializes the watermarks and underlying 
   queues.
 
 DS707_WMK_SETUP_WMKS
   Called to setup the watermarks and set various levels, etc.
 
 DS707_WMK_CLEANUP_WMKS
   Cleans up the watermarks at the end of a call.  Frees up any
   remaining DSM items.
 
 INITIALIZATION AND SEQUENCING REQUIREMENTS
   Must call ds707_wmk_init() at startup.

 Copyright (c) 2002 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_wmk.c_v   1.2   19 Nov 2002 19:23:50   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_wmk.c#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $
  
when       who        what, where, why
--------   ---        -------------------------------------------------------
05/21/02   vas        Removed AN watermarks. Moved these to separate file.
07/16/02   ak         Minor comment cleanup.
07/14/02   ak         Added cleanup_wmk function.
02/20/02   ak         First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#include "ds707_wmk.h"
#include "ds707_pkt_mgr.h"
#include "ds_flow_control.h"
#include "dsm.h"
#include "ps_iface.h"
#include "dsm.h"
#include "queue.h"
#include "rex.h"

/*===========================================================================
                            VARIABLES
===========================================================================*/

/*---------------------------------------------------------------------------
  RLP FWD link queue's
---------------------------------------------------------------------------*/
q_type             ds707_wmk_um_SN_fwd_q;    /* rlp fwd link queue         */ //VIP Does not change

/*---------------------------------------------------------------------------
  RLP REV link watermark (from PPP to RLP) (SN/1X Link)
---------------------------------------------------------------------------*/
dsm_watermark_type ds707_wmk_um_SN_rev_wmk;
q_type             ds707_wmk_um_SN_rev_q;


/*===========================================================================
                        INTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_WMK_UM_TX_LO_WM_FUNC

DESCRIPTION   Called (via function pointer) when the tx watermark reg'd
              with RLP hits the low watermark event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_wmki_um_tx_lo_wm_func(void)
{
  ps_iface_type    *iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Enable the flow on this interface since the TX wm is now below the lower
    limit
  -------------------------------------------------------------------------*/
  iface_ptr = ds707_pkt_get_iface_ptr(DS707_DEF_PKT_INSTANCE);
  ps_iface_enable_flow(iface_ptr, DS_FLOW_707_RLP_MASK);
} /* ds707_wmki_um_tx_lo_wm_func() */

/*===========================================================================
FUNCTION      DS707_WMK_UM_TX_HI_WM_FUNC

DESCRIPTION   Called (via function pointer) when the tx watermark reg'd
              with RLP hits the high watermark event.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_wmki_um_tx_hi_wm_func(void)
{
  ps_iface_type    *iface_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Disable the flow on this interface since the TX wm reached the lo limit
  -------------------------------------------------------------------------*/
  iface_ptr = ds707_pkt_get_iface_ptr(DS707_DEF_PKT_INSTANCE);
  ps_iface_disable_flow(iface_ptr, DS_FLOW_707_RLP_MASK);
} /* ds707_wmki_um_tx_hi_wm_func() */

/*===========================================================================
                        EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_WMK_INIT

DESCRIPTION   Called at code startup (i.e, only once).  Sets up queues with
              watermarks, etc.

DEPENDENCIES  Called only when the phone powers up.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_init( void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  INTLOCK();
  (void)q_init(&ds707_wmk_um_SN_fwd_q);
  (void)q_init(&ds707_wmk_um_SN_rev_q);
  ds707_wmk_um_SN_rev_wmk.current_cnt = 0;
  ds707_wmk_um_SN_rev_wmk.q_ptr = &ds707_wmk_um_SN_rev_q;

  INTFREE();
} /* ds_wmk_init() */



/*===========================================================================
FUNCTION      DS707_WMK_SETUP_WMKS

DESCRIPTION   This function will initialize the service-configurable DS task
              shared watermark items to operate for the current data service.
              The 'dsi_srvc_mode' variable determines the service mode to
              setup the Watermark items for.

DEPENDENCIES  Hi/Lo Watermark Settings: The Lo and Hi watermark values and
              the don't exceed values for each watermark is inter-dependent
              with the DSM small and large item counts (DSM_LARGE_ITEM_CNT
              and DSM_SMALL_ITEM_CNT) in the DSM module. Any change to lo/hi
              watermark or don't exceed values can cause ERR_FATALs due to
              insufficient memory items available.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_setup_wmks(void)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Intialize PPP -> RLP SN/1X watermark
  -------------------------------------------------------------------------*/
  ds707_wmk_um_SN_rev_wmk.lo_watermark       = 1000;
  ds707_wmk_um_SN_rev_wmk.hi_watermark       = 2000;
  ds707_wmk_um_SN_rev_wmk.dont_exceed_cnt    = 0xFFFF;

  ds707_wmk_um_SN_rev_wmk.lowater_func_ptr   = ds707_wmki_um_tx_lo_wm_func;
  ds707_wmk_um_SN_rev_wmk.hiwater_func_ptr   = ds707_wmki_um_tx_hi_wm_func;

  ds707_wmk_um_SN_rev_wmk.non_empty_func_ptr    = NULL;
  ds707_wmk_um_SN_rev_wmk.gone_empty_func_ptr   = NULL;
  ds707_wmk_um_SN_rev_wmk.each_enqueue_func_ptr = NULL;

} /* ds707_wmk_setup_wmks() */

/*===========================================================================
FUNCTION      DS707_WMK_CLEANUP_WMKS

DESCRIPTION   This function cleans up the watermarks that are owned by 
              data services.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_cleanup_wmks(void)
{
  dsm_item_type             *item_ptr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Empty SN and AN watermarks (Fwd Link)
  -------------------------------------------------------------------------*/
  dsm_empty_queue(&ds707_wmk_um_SN_rev_wmk);

  /*-------------------------------------------------------------------------
    Empty SN and AN queues (Rev Link)
  -------------------------------------------------------------------------*/
  while ((item_ptr = (dsm_item_type *)q_get(&ds707_wmk_um_SN_fwd_q)) != NULL)
  {
    (void)dsm_free_packet(&item_ptr);
  }

} /* ds707_wmk_cleanup_wmks() */
#endif /* FEATURE_DATA_IS707 */
