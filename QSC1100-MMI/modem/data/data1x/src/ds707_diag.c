/*===========================================================================

                        D S 7 0 7 _ D I A G 
GENERAL DESCRIPTION
  This file contains the functionality for the IS-707 interface to QXDM/DIAG.
  This implements the commands a user can issue from QXDM to the mobile, 
  which are specific to 1X data.

EXTERNALIZED FUNCTIONS
 ds707_diag_init()
   Registers the functions in this file with DIAG.
   
INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.

 Copyright (c) 2003-2008 by QUALCOMM Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_diag.c#2 $
  
when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/08/08   sn      Fixed klockwork error.
05/01/03   ak      First version of file.
===========================================================================*/


/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#ifdef __cplusplus
  extern "C" {
#endif

#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_diag.h"
#include "ds707_pkt_mgr.h"
#include "diagcmd.h"
#include "dsrlp.h"
#include "err.h"

#ifdef FEATURE_IS95B_MDR
  #include "mdrrlp.h"
#else
  #include "rlp.h"
#endif


/*===========================================================================
                              FORWARD DECLARATIONS
===========================================================================*/
PACKED void* ds707_diagi_get_rlp_stats
(
  PACKED void* req_pkt,
  uint16       pkt_len
);


/*===========================================================================
                                VARIABLES
===========================================================================*/

static const diagpkt_user_table_entry_type  ds707_diag_tbl[] = 
{
  {
    DS707_DIAG_RLP_STATS,         /* Lower bound sub-command code          */
    DS707_DIAG_RLP_STATS,         /* Upper bound sub-command code          */
    ds707_diagi_get_rlp_stats
  }
};


/*===========================================================================
                           INTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_DIAGI_GET_RLP_STATS

DESCRIPTION   Retrieves the RLP statistics for RLP I/II and III.  Also 
              returns the last pkt_so that was used.
              
              Note that the function definition below is spec'd by DIAG.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
PACKED void* ds707_diagi_get_rlp_stats
(
  PACKED void* req_pkt,
  uint16       pkt_len
)
{
  rlp_ext_stats_type        rlp_stats;
  dsrlp_stats_info_type    *rlp3_stats_ptr = NULL;

  DIAG_SUBSYS_DATA1X_DS707_DIAG_RLP_STATS_rsp_type  *rsp_ptr;

  const int rsp_len = 
                    sizeof(DIAG_SUBSYS_DATA1X_DS707_DIAG_RLP_STATS_rsp_type);
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Allocate response buffer
  -------------------------------------------------------------------------*/
  rsp_ptr = (DIAG_SUBSYS_DATA1X_DS707_DIAG_RLP_STATS_rsp_type *)
            diagpkt_subsys_alloc(DIAG_SUBSYS_DATA1X,
                                 DS707_DIAG_RLP_STATS,
                                 rsp_len);
  if( rsp_ptr == NULL )
  {
    MSG_ERROR("Couldn't allocate DIAG response buffer", 0,0,0);
    return NULL;
  }
     
  /*-------------------------------------------------------------------------
    Allocate response buffer
  -------------------------------------------------------------------------*/
  rsp_ptr->last_pkt_so   = ds707_pkt_get_last_so();

  /*-------------------------------------------------------------------------
  Retrieve RLP 3 statistics for RLP I/II.  Note that these could be weird
  if there was an aysnc data call beforehand.
  -------------------------------------------------------------------------*/
  rlp_get_stats(&rlp_stats);
  rsp_ptr->rlp_rx_bytes  = rlp_stats.rx_data_cnt;
  rsp_ptr->rlp_tx_bytes  = rlp_stats.tx_data_cnt;

  /*-------------------------------------------------------------------------
    Retrieve RLP 3 statistics for 1X.
  -------------------------------------------------------------------------*/
  dsrlp_get_stats(DSRLP_FIXED_SR_ID_INDEX, &rlp3_stats_ptr);

  if (rlp3_stats_ptr != NULL)
  {
    rsp_ptr->rlp3_rx_bytes = rlp3_stats_ptr->rx_total_bytes;
    rsp_ptr->rlp3_tx_bytes = rlp3_stats_ptr->tx_total_bytes;
  }
  else
  {
    rsp_ptr->rlp3_rx_bytes = 0;
    rsp_ptr->rlp3_tx_bytes = 0;
    ERR("Could not retrieve RLP 3 stats",0,0,0);
  }

#ifdef FEATURE_HDR
#error code not present
#else
  rsp_ptr->rlp_hdr_SN_rx_bytes = 0;
  rsp_ptr->rlp_hdr_SN_tx_bytes = 0;
  rsp_ptr->rlp_hdr_AN_rx_bytes = 0;
  rsp_ptr->rlp_hdr_AN_tx_bytes = 0;
#endif

  return(rsp_ptr);
} /* ds707_diagi_get_rlp_stats() */


/*===========================================================================
                           EXTERNAL FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_DIAG_INIT

DESCRIPTION   Registers functions with DIAG.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/
#ifdef __cplusplus
  DIAGPKT_DISPATCH_AUTOREGISTER(DIAG_SUBSYS_DATA1X, ds707_diag_tbl);
#else
  void ds707_diag_init(void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER(DIAG_SUBSYS_DATA1X, ds707_diag_tbl);
  } /* ds707_diag_init() */
#endif /* __cplusplus */

#endif /* FEATURE_DATA_IS707 */

#ifdef __cplusplus
  }
#endif


