/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

  R A D I O   L I N K   P R O T O C O L  E X T E R N A L  I N T E R F A C E 

GENERAL DESCRIPTION
  This file contains externalized RLP interface functions to be used by 
  other modules. These modules could be other modules within data services
  or outside data services.  

EXTERNALIZED FUNCTIONS

DESCRIPTION
  

 Copyright (c) 2003 - 2007 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/modem/data/1x/1xrlp/main/lite/src/dsrlp_extif.c#1 $ $DateTime: 2007/11/06 01:21:33 $ $Author: nsivakum $

when       who        what, where, why
--------   ---        -------------------------------------------------------
10/18/07   sn         Added "err.h" as include file.
07/05/04   gr         Initial version
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "customer.h"
#include "comdef.h"

#if (defined( FEATURE_DS_RLP3) || defined (FEATURE_HDR))

#include "dsrlpi.h"
#include "dsrlp_extif.h"
#include "err.h"

/*===========================================================================

            EXTERNAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                    PUBLIC FUNCTION DEFINTIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================

FUNCTION       DSRLP_EXTIF_GET_NAK_CNT

DESCRIPTION    This function checks if RLP is in Established state and 
               populates the nak_count_ptr. It returns TRUE if the NAK
               Q is not empty. Otherwise it returns FALSE. 
               
DEPENDENCIES   NONE

RETURN VALUE   TRUE if NAK Q is non-empty

SIDE EFFECTS   
===========================================================================*/
boolean dsrlp_extif_get_nak_cnt
(
  uint32 *nak_count_ptr
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr=NULL;  /* rlp-specific control block  */
  boolean status = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (nak_count_ptr == NULL)
  {
    ERR("Input ptr NULL", 0, 0, 0);
    return status;
  }

  /*-------------------------------------------------------------------------
    Retrieve the NAK count for the RLP instance DSRLP_FIXED_SR_ID_INDEX. 
  -------------------------------------------------------------------------*/
  *nak_count_ptr = 0;
  rscb_ptr      = &(dsrlpi_rscb_array[DSRLP_FIXED_SR_ID_INDEX]);

  if ( rscb_ptr->state.state == DSRLPI_ESTABLISHED_STATE)
  {
    *nak_count_ptr =  (uint32)q_cnt(&(rscb_ptr->state.nak_q));
    if( (*nak_count_ptr) > 0) 
    {
      MSG_MED("NAK queue is not null - NACK pending", 0, 0,0);
    }
    status = TRUE;
  }
  return status; 
}

/*===========================================================================

FUNCTION       DSRLP_EXTIF_GET_IDLE_TX_FRAME_CNT

DESCRIPTION    This function checks if RLP is in Established state and 
               populates the idle_tx_fr_cnt_ptr. It returns TRUE if RLP is
               the Established state. Otherwise it returns FALSE. 
               
DEPENDENCIES   NONE

RETURN VALUE   TRUE if RLP is in the ESTABLISHED state

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlp_extif_get_idle_tx_frame_cnt
(
  uint32 *idle_tx_fr_cnt_ptr /* ptr to tx idle frame count to be populated */
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr=NULL;  /* rlp-specific control block  */
  boolean status = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (idle_tx_fr_cnt_ptr == NULL)
  {
    ERR("Input ptr NULL", 0, 0, 0);
    return status;
  }

  *idle_tx_fr_cnt_ptr = 0; 
  /*-------------------------------------------------------------------------
    Retrieve the contiguous tx idle frame count for the RLP 
    instance DSRLP_FIXED_SR_ID_INDEX. 
  -------------------------------------------------------------------------*/
  rscb_ptr      = &(dsrlpi_rscb_array[DSRLP_FIXED_SR_ID_INDEX]);

  if ( rscb_ptr->state.state == DSRLPI_ESTABLISHED_STATE)
  {
    *idle_tx_fr_cnt_ptr =  rscb_ptr->stats.tx_contig_idle_fr_cnt;
    status = TRUE;
  }

  return status;

}

/*===========================================================================

FUNCTION       DSRLP_EXTIF_GET_RX_IDLE_FRAME_CNT

DESCRIPTION    This function checks if RLP is in Established state and 
               populates the idle_rx_fr_cnt_ptr. It returns TRUE if RLP is
               the Established state. Otherwise it returns FALSE. 
               
DEPENDENCIES   NONE

RETURN VALUE   TRUE if RLP is in the ESTABLISHED state

SIDE EFFECTS   NONE
===========================================================================*/
boolean dsrlp_extif_get_idle_rx_frame_cnt
(
  uint32 *idle_rx_fr_cnt_ptr /* ptr to rx idle frame count to be populated */
)
{
  dsrlpi_sess_ctl_blk_type *rscb_ptr=NULL;  /* rlp-specific control block  */
  boolean status = FALSE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if (idle_rx_fr_cnt_ptr == NULL)
  {
    ERR("Input ptr NULL", 0, 0, 0);
    return status;
  }

  *idle_rx_fr_cnt_ptr = 0; 

  /*-------------------------------------------------------------------------
    Retrieve the contiguous rx idle frame count for the RLP 
    instance DSRLP_FIXED_SR_ID_INDEX. 
  -------------------------------------------------------------------------*/
  rscb_ptr      = &(dsrlpi_rscb_array[DSRLP_FIXED_SR_ID_INDEX]);

  if ( rscb_ptr->state.state == DSRLPI_ESTABLISHED_STATE)
  {
    *idle_rx_fr_cnt_ptr =  rscb_ptr->stats.rx_contig_idle_fr_cnt;
    status = TRUE;
  }

  return status;

}

#endif /* any of the RLPs defined */
