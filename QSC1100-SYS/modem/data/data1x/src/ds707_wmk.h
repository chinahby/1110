#ifndef DS707_WMK_H
#define DS707_WMK_H
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

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_wmk.h_v   1.2   19 Nov 2002 19:23:58   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_wmk.h#1 $ $DateTime: 2007/11/06 01:17:08 $ $Author: nsivakum $
  
when       who        what, where, why
--------   ---        -------------------------------------------------------
05/21/03   vas        Removed AN watermarks. moved them to separate file.
07/22/02   ak         Added ds707_cleanup_wmk to file.
02/20/02   ak         First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/

#include "comdef.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#include "dsm.h"
#include "queue.h"

/*===========================================================================
                            VARIABLES
===========================================================================*/
/*---------------------------------------------------------------------------
  RLP FWD link queue's
---------------------------------------------------------------------------*/
extern q_type             ds707_wmk_um_SN_fwd_q; /* rlp fwd link queue     */

/*---------------------------------------------------------------------------
  RLP REV link watermark (from PPP to RLP) (SN/1X Link)
---------------------------------------------------------------------------*/
extern dsm_watermark_type ds707_wmk_um_SN_rev_wmk;
extern q_type             ds707_wmk_um_SN_rev_q;


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
void ds707_wmk_init( void);

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
void ds707_wmk_setup_wmks(void);

/*===========================================================================
FUNCTION      DS707_WMK_CLEANUP_WMKS

DESCRIPTION   This function cleans up the watermarks that are owned by 
              data services.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_wmk_cleanup_wmks(void);
#endif /* FEATURE_DATA_IS707 */
#endif /* DS707_WMK_H        */
