/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               D S 7 0 7 _ S D B   F I L E

GENERAL DESCRIPTION
  This file contains functions used to implement Short Data Burst (SDB)
  available in IS2000.

EXTERNALIZED FUNCTIONS
  ds707_sdb_DBM_event_handler()

  ds707_sdb_tx_sdb_pkt()

  ds707_sdb_init()

  ds707_sdb_supports_SDB_now()

INITIALIZATION AND SEQUENCING REQUIREMENTS

 Copyright (c) 2002-2009 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_sdb.c_v   1.4   06 Feb 2003 17:49:18   ajithp  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_sdb.c#3 $ $DateTime: 2009/06/10 22:01:08 $ $Author: msankar $

when        who    what, where, why
--------    ---    -------------------------------------------------------
05/30/09    ms     Added Support for EPZID Hysteresis.
10/24/07    sn     Explicitly casted ds707_sdb_to_cm_copy() callback in 
                   ds707_sdb_tx_handler().
10/18/07    sn     Replaced dsm_move_to_buf() with dsm_pullup(). Pulled in 
                   this change from tip version of this file.
04/05/05    gr     Fixed the handling of SO_OMIT field in the SDB header
09/08/04    vr     Notify the PZID hysterisis engine if an SDB is received
                   or transmitted
10/14/03    atp    Removed LOCAL declaration of ds707_sdb_supports_SDB_now().
07/31/03    atp    Changes to map CM status info to DS for apps.
07/07/03    atp    SDB enhancements to pass status notification to app.
04/17/03    atp    Minor change - use q_cnt() function instead of directly
                   accessing queue count field.
03/17/03    atp    Added 2 fixes: 
                   - scenario flags ACH=1,REACH=1, but REACH not supported.
                   - Fixed incoming SDB problem (not going to Rx queue).
02/06/03    atp    Moved ds707_sdb_supports_SDB_now() to .h as external fn.
11/27/02    atp    Get tx wmk from pkt mgr using call instance instead of 
                   sr_id. Removed assert if wmk is NULL.
11/08/02    atp    PS reverted to passing meta info as * in dsm item.
11/06/02    atp    Synced to PS change that passes meta info as ** in dsm.
10/29/02    atp/sy Initial version.
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"
#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_DATA_IS2000_SDB
#include "ds3gmgrint.h"
#include "ds707_pkt_mgr.h"
#include "ds707_sdb.h"
#include "dssocket_defs.h"
#include "mccdma.h"
#ifdef FEATURE_DS_PZID_HYSTERESIS
#include "ds707_pzid_hyst.h"
#else
#include "ds707_epzid_hyst.h"
#endif /* FEATURE_DS_PZID_HYSTERESIS */

/*===========================================================================
                         TYPEDEFS & VARIABLES
===========================================================================*/
word ds707_sdb_prev_data_call_so;
byte ds707_sdb_prev_data_sr_id;
ds707_sdb_info_type ds707_sdb_info;

boolean ds707_sdb_so_omit_indicator = FALSE;

/*===========================================================================
                            Forward Declarations
===========================================================================*/
LOCAL boolean ds707_sdb_is_reach_supported
(
  void
);


LOCAL boolean ds707_sdb_supports_SDB_now
(
  uint32 flags
);

LOCAL void ds707_sdb_cm_tx_err_status
(
  void   *data_block_ptr,         
  cm_dbm_cmd_e_type dbm_cmd, 
  cm_dbm_cmd_err_e_type dbm_cmd_err   
);

LOCAL byte ds707_sdb_sr_id_to_idx
(
  byte sr_id
);

LOCAL boolean ds707_sdb_tx_handler
(
  dsm_item_type * item_ptr
);

LOCAL void ds707_sdb_rx_handler
(
  uint8 *buf,
  uint32 buf_len
);

LOCAL int8  ds707_sdb_to_cm_copy
(
  void *dsm_item_ref_ptr,
  void *buf,
  int  *len_ptr
);

LOCAL ds707_radio_transport_in_dorm_type ds707_sdb_radio_transport_in_dorm
(
  dsm_watermark_type * tx_wm_ptr,
  byte * num_sdb_pkts_ptr,
  boolean            * set_sig
);


/*===========================================================================
                   EXTERNAL FUNCTION DEFINITIONS
===========================================================================*/

/*===========================================================================
FUNCTION      DS707_SDB_DBM_EVENT_HANDLER

DESCRIPTION   This function will be registered by DS on power on. This
              function will be called by CM when ever it obtains an MT
              DBM message, or it gets status of MO SDB that was transmitted.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sdb_DBM_event_handler
(
  cm_dbm_event_e_type event, 
  const cm_dbm_info_s_type* dbm_info_ptr
)
{
  int32             sock_id;  /* this was passed as user data to CM when
                                   SDB was sent                            */
  ps_iface_type     *iface_ptr;               /* ptr to pkt iface          */
  dss_sdb_ack_status_info_type  ack_status; /* status passed to upper layer*/
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    /*-----------------------------------------------------------------------
      Check the input values
      If they are unacceptable, just return from function without doing
      anything.
    -----------------------------------------------------------------------*/
    if( (event >= CM_DBM_EVENT_MAX) || (dbm_info_ptr == NULL) )
    {
       MSG_ERROR("invalid event:%d or ptr:%x",event,dbm_info_ptr,0);
       return;
    }

    /*-----------------------------------------------------------------------
      Process the input.
      Find which event occurred.
    -----------------------------------------------------------------------*/
    switch(event)
    {
    /*-----------------------------------------------------------------------
      Mobile-Terminated SDB.
    -----------------------------------------------------------------------*/
      case CM_DBM_EVENT_MT_MSG:
        /*-------------------------------------------------------------------
          Check if the incoming message is SDB or not.
          If it is SDB, then, handle it.
          If its not SDB,then ignore it.
        -------------------------------------------------------------------*/
        if(dbm_info_ptr->burst_type == CAI_SHORT_DATA_BURST)
        {
         /*-----------------------------------------------------------------
           Inform EPZID hysteresis processor of data ready state 
	 -----------------------------------------------------------------*/
#ifdef FEATURE_DS_PZID_HYSTERESIS
         ds707_pzid_hysteresis_post_event(PZID_HYS_SDB_DATA_EVENT);
#else
         ds707_epzid_hysteresis_post_event(EPZID_HYS_SDB_DATA_EVENT);
#endif /* FEATURE_DS_PZID_HYSTERESIS  */ 
          /*-----------------------------------------------------------------
            Call the SDB recive handler
          -----------------------------------------------------------------*/
          ds707_sdb_rx_handler(dbm_info_ptr->payload,
                               dbm_info_ptr->num_bytes
                              );
        }
        else
        {
            MSG_LOW("rxed Non-SDB DB event:Ignoring",0,0,0);
        }
        break;

    /*-----------------------------------------------------------------------
      Status of mobile-originated SDB, that was sent earlier.
    -----------------------------------------------------------------------*/
      case CM_DBM_EVENT_MO_STATUS:
        /*-------------------------------------------------------------------
          Check if SDB or not.
          If it is SDB, then, handle it.
          If its not SDB,then ignore it.
        -------------------------------------------------------------------*/
        if(dbm_info_ptr->burst_type == CAI_SHORT_DATA_BURST)
        {
          sock_id = (int32) dbm_info_ptr->user_data;
          /*-----------------------------------------------------------------
            Map CM info to DS type.
          -----------------------------------------------------------------*/
          ack_status.overflow = (uint32) dbm_info_ptr->burst_overflow_size;
          switch ( dbm_info_ptr->mo_status )
          {
            case CM_DBM_MO_NONE:
              ack_status.status = DSS_SDB_ACK_NONE;
              break;

            case CM_DBM_MO_OK:
              ack_status.status = DSS_SDB_ACK_OK;
              break;

            case CM_DBM_MO_HOLD_ORIG_RETRY_TIMEOUT:
              ack_status.status = DSS_SDB_ACK_HOLD_ORIG_RETRY_TIMEOUT;
              break;

            case CM_DBM_MO_HOLD_ORIG:
              ack_status.status = DSS_SDB_ACK_HOLD_ORIG;
              break;

            case CM_DBM_MO_NO_SRV:
              ack_status.status = DSS_SDB_ACK_NO_SRV;
              break;

            case CM_DBM_MO_ABORT:
              ack_status.status = DSS_SDB_ACK_ABORT;
              break;

            case CM_DBM_MO_NOT_ALLOWED_IN_AMPS:
              ack_status.status = DSS_SDB_ACK_NOT_ALLOWED_IN_AMPS;
              break;

            case CM_DBM_MO_NOT_ALLOWED_IN_HDR:
              ack_status.status = DSS_SDB_ACK_NOT_ALLOWED_IN_HDR;
              break;

            case CM_DBM_MO_L2_ACK_FAILURE:
              ack_status.status = DSS_SDB_ACK_L2_ACK_FAILURE;
              break;

            case CM_DBM_MO_OUT_OF_RESOURCES:
              ack_status.status = DSS_SDB_ACK_OUT_OF_RESOURCES;
              break;

            case CM_DBM_MO_ACCESS_TOO_LARGE:
              ack_status.status = DSS_SDB_ACK_ACCESS_TOO_LARGE;
              break;

            case CM_DBM_MO_DTC_TOO_LARGE:
              ack_status.status = DSS_SDB_ACK_DTC_TOO_LARGE;
              break;

            case CM_DBM_MO_OTHER:
              ack_status.status = DSS_SDB_ACK_OTHER;
              break;

            case CM_DBM_MO_ACCT_BLOCK:
              ack_status.status = DSS_SDB_ACK_ACCT_BLOCK;
              break;

            default:
              ack_status.status = DSS_SDB_ACK_NONE;
              break;
          }
          /*-----------------------------------------------------------------
            Call the ps_iface handler to convey SDB status to upper layers.
          -----------------------------------------------------------------*/
          iface_ptr = ds707_pkt_get_iface_ptr ( DS707_DEF_PKT_INSTANCE );
          if ( ps_iface_sdb_status_handler (iface_ptr, sock_id, &ack_status )
               < 0 )
          {
            ERR (" ps_iface_sdb_status_handler() returned error", 0,0,0);
          }
          else
          {
            MSG_HIGH ("SDB  status: %d sock_id: %d",
                      ack_status.status,
                      sock_id,
                      0);
          }
        }
        else
        {
          MSG_LOW("rxed Non-SDB DB event:Ignoring",0,0,0);
        }
        break;

      default:
        MSG_MED("Unreg Function called for %d event",event,0,0);
        break;
    }
} /* ds707_sdb_DBM_event_handler() */


/*===========================================================================
FUNCTION      DS707_SDB_PROCESS_TXQ

DESCRIPTION   Return true if we can do send the SDB packet or if there is
              nothing to do otherwise returns false saying that the packet
              needs to be sent using RLP3.

DEPENDENCIES  None.

RETURN VALUE  True/False.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_sdb_process_txq
(
  void
)
{
  ds707_radio_transport_in_dorm_type radio_transport;
  dsm_watermark_type         *tx_wm_ptr;/* Watermark pointer to take data  */
  dsm_item_type              *item_ptr; /* Packet to be sent               */
  byte num_sdb_pkts          = 0;       /* Number of SDB pkts to be sent   */
  boolean set_sig            = FALSE;   /* Set flag to reorig dormant call */
  boolean ret_val            = FALSE;   /* Need to send pkt using RLP3     */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Obtain pointer to the water mark that actually was written to;
  -------------------------------------------------------------------------*/
  tx_wm_ptr = ds707_pkt_get_curr_tx_wm (DS707_DEF_PKT_INSTANCE);

  /*-------------------------------------------------------------------------
    Note that the transmit water mark cannot be NULL for a RLP/1x
    call, since we are re-originating a data call.
  -------------------------------------------------------------------------*/
  if ( tx_wm_ptr == NULL )
  {
    MSG_HIGH("tx_wm_ptr is NULL", 0, 0, 0);
    return FALSE;
  }

  /*-------------------------------------------------------------------------
    Look at the Meta info contents of the first item of the watermark
    and decide if the data should go as SDB or Re-originate a call. If
    Packet wants to go on SDB, then make next check. Otherwise, return
    to originate a RLP call.
    Then look if the current DS scenario can support SDB call. If yes,
    then make SDB else make a RLP call.

    Assumptions:
    It is assumed that the SDB Tx Watermark pointer is already
    initialized to the Watermark currently used.

  -------------------------------------------------------------------------*/
  radio_transport = ds707_sdb_radio_transport_in_dorm ( tx_wm_ptr,
                                                        &num_sdb_pkts,
                                                        &set_sig );

  switch(radio_transport)
  {
    case RADIO_TRANSPORT_SDB:
      /*---------------------------------------------------------------------
        Remove packet out of the water mark and give it to the SDB.
      ---------------------------------------------------------------------*/
      while (num_sdb_pkts-- != 0)
      {
        item_ptr = (dsm_item_type *)dsm_dequeue(tx_wm_ptr);
        ASSERT(item_ptr != NULL); //We come here only if its not NULL.

        /*-------------------------------------------------------------------
          handle this packet.
        -------------------------------------------------------------------*/
        ds707_sdb_tx_handler(item_ptr);
      }
      if (TRUE == set_sig )
      {
         // Set signal to DS
      }
      ret_val = TRUE;
      /*----------------------------------------------------------------
        Inform PZID hysteresis processor of data ready state 
      -----------------------------------------------------------------*/
#ifdef FEATURE_DS_PZID_HYSTERESIS
      ds707_pzid_hysteresis_post_event(PZID_HYS_SDB_DATA_EVENT);
#else
      ds707_epzid_hysteresis_post_event(EPZID_HYS_SDB_DATA_EVENT);
#endif /* FEATURE_DS_PZID_HYSTERESIS  */
      break;

     case RADIO_TRANSPORT_RLP3:
     default:
       ret_val = FALSE;
  }

  return (ret_val);

} /* ds707_sdb_process_txq() */


/*===========================================================================
FUNCTION      DS707_SDB_INIT

DESCRIPTION   This function initializes all the SDB related variables. This
              will be called when a data call is established.  This function
              should be called whenever RLP is being set up. So, whenever
              dsrlp_reg_srvc() is called, its good to call this function also
              and set up the appropriate values.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sdb_init
(
  byte                sr_id_index,           /* index for a session id     */
  dsm_watermark_type  *tx_watermark_ptr,     /* ptr to Tx wm for txmit data*/
  void                (*post_rx_func_ptr)(), /* rx callback for rx frames  */
  q_type              *post_rx_q_ptr         /* queue for rx'ed frames     */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if( (sr_id_index      >= DSRLPI_MAX_SRID_INDEX) ||
      (tx_watermark_ptr == NULL)                  ||
      (post_rx_func_ptr == NULL)                  ||
      (post_rx_q_ptr    == NULL)
     )
     {
       MSG_ERROR("Cannot initialize SDB",0,0,0);
     }

  ds707_sdb_info.sdb_cfg[sr_id_index].tx_wm_ptr   = tx_watermark_ptr;
  ds707_sdb_info.sdb_cfg[sr_id_index].rx_func_ptr = post_rx_func_ptr;
  ds707_sdb_info.sdb_cfg[sr_id_index].rx_q_ptr    = post_rx_q_ptr;

} /* ds707_sdb_init */


/*===========================================================================
                             OTHER FUNCTIONS
===========================================================================*/
/*===========================================================================
FUNCTION      DS707_SDB_IS_REACH_SUPPORTED

DESCRIPTION   Returns TRUE if REACH supported else FALSE.

DEPENDENCIES  None.

RETURN VALUE  True/False.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_sdb_is_reach_supported
(
  void
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
#ifdef FEATURE_IS2000_REL_A
    if (OVHD_CHAN_IS_BCCH)
    {
      MSG_HIGH("REACH Supported",0,0,0);
      return TRUE;
    }
    else
    {
      return FALSE;
    }
#else
    {
      return FALSE;
    }
#endif
} /* ds707_sdb_is_reach_supported() */


/*===========================================================================
FUNCTION      DS707_SDB_CURR_PKT_NEEDS_SDB

DESCRIPTION   This function is used to confirm that the current packet wants
              to go on the SDB by looking at its Meta Info.
              If this packet does not desire SDB, originate a data call.
              If it is SDB, then, we need to check for two more things:
              a) Are  flags requesting for ACH or Enhanced-Access channel.
                 If Access channel, return TRUE
                 If Enhanced-Access Channel,
              b) Confirm the PREV before returning TRUE.
                 If PREV is >= 7, return TRUE.
                 Else return FALSE.

DEPENDENCIES  None.

RETURN VALUE  True/False.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_sdb_curr_pkt_needs_SDB
(
  dsm_item_type * out_item_ptr,
  uint32        * flags_ptr
)
{
  ps_meta_info_type *meta_info_ptr;         /* pointer to meta info        */
  boolean           pkt_needs_SDB  = FALSE; /* flag:Indicates SDB needed   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Input sanity check
  -------------------------------------------------------------------------*/
  if(NULL == out_item_ptr)
  {
     MSG_ERROR("Null pointer passed for SDB",0,0,0);
     return (pkt_needs_SDB);
  }

  /*-------------------------------------------------------------------------
    APP field has the pointer to a pointer to meta info type.
    If app_field is NULL then, we know its not SDB. Originate a RLP call.
    NOTE: meta_info_ptr is actually typecasted to data_ptr in a dsm_item.
  -------------------------------------------------------------------------*/
  meta_info_ptr = (ps_meta_info_type *) out_item_ptr->app_field;
  if(    ( meta_info_ptr == NULL )                              
      || ( ( PS_META_INFO_VALID ( meta_info_ptr ) ) == FALSE )       
    )
  {
    pkt_needs_SDB = FALSE;
    return (pkt_needs_SDB);
  }

  /*-------------------------------------------------------------------------
    We know that there is some Meta information. Let's see if it is
    SDB releated. If yes, we can process further.
    If the tx_flags are 0, it means we have no SDB information. So, this
    packet does not need SDB transport.
    Look if ACH flag is set or Rev-Enhanced Access Channel flag is set.
  -------------------------------------------------------------------------*/
  if ( meta_info_ptr->tx_flags == 0 )
  {
    pkt_needs_SDB = FALSE;
    return pkt_needs_SDB;
  }

  if((meta_info_ptr->tx_flags & MSG_FAST_EXPEDITE) == MSG_FAST_EXPEDITE)
  {
    /*-----------------------------------------------------------------------
      Check if REACH is supported.
    -----------------------------------------------------------------------*/
    if (ds707_sdb_is_reach_supported() == TRUE)
    {
      pkt_needs_SDB = TRUE;
      *flags_ptr = MSG_FAST_EXPEDITE;
      return pkt_needs_SDB;
    }
    else
    {
      pkt_needs_SDB = FALSE;
      *flags_ptr = meta_info_ptr->tx_flags;
    }
  }
  if ((meta_info_ptr->tx_flags & MSG_EXPEDITE) == MSG_EXPEDITE)
  {
    pkt_needs_SDB = TRUE;
    *flags_ptr = MSG_EXPEDITE;
    return pkt_needs_SDB;
  }

  return pkt_needs_SDB;

} /* ds707_sdb_curr_pkt_needs_SDB() */


/*===========================================================================
FUNCTION      DS707_SDB_SUPPORTS_SDB_NOW

DESCRIPTION   Confirm if MS supports SDB. We need to check for two things:
              1. Are the flags requesting for ACH or Enhanced-Access channel.
                 If (Access channel)
                   Continue to test 2.
                 If (Enhanced-Access Channel)
                   Confirm the PREV supports R-EACH before returning SDB.
                 If (PREV is >= 7)
                   continue to test 2
                 else
                   return RLP

              2. Is the Mobile state currently dormant.
                 If (MS is currently dormant)
                 If(previous data call  == SO 33)
                   Return SDB
                 Else
                   Return RLP

DEPENDENCIES  None.

RETURN VALUE  TRUE:  If SDB is supported in current DS state.(Depends on SO)
              FALSE: If SDB is not supported in current DS state.

SIDE EFFECTS  None.
===========================================================================*/
boolean ds707_sdb_supports_SDB_now
(
  uint32 flags
)
{
  boolean can_do_sdb = FALSE;
  ps_iface_type        *pkt_iface_ptr;               /* ptr to pkt iface   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Input Sanity Check
    If we get unknown flags, then return FALSE
  -------------------------------------------------------------------------*/
  if ((flags != MSG_EXPEDITE) &&
      (flags != MSG_FAST_EXPEDITE))
  {
    MSG_ERROR("unknown Flags for SDB %u",flags,0,0);
    return (can_do_sdb);
  }

  /*------------------------------------------------------------------------
    MS should be dormant to do SDB. So check it.
    If (MS is currently dormant) && If(previous data call  == SO 33)
       can_do_sdb = TRUE
    Else
       can_do_sdb = FALSE
  ------------------------------------------------------------------------*/
  pkt_iface_ptr = ds707_pkt_get_iface_ptr ( DS707_DEF_PKT_INSTANCE );
  ds707_sdb_prev_data_call_so = ds707_pkt_get_prev_call_so();
  if (   ( ds707_pkt_is_dormant(pkt_iface_ptr) == TRUE ) 
      && ( ds707_sdb_prev_data_call_so == CAI_SO_PPP_PKT_DATA_3G )
     )
  {
    can_do_sdb = TRUE;
  }
  else
  {
    MSG_HIGH("cannot do SDB;SO:%x or not dormant",
             ds707_sdb_prev_data_call_so,0,0);
    can_do_sdb = FALSE;
  }

  /*-------------------------------------------------------------------------
    Now check if the FLAGS are supported in current call.
  -------------------------------------------------------------------------*/
  if (TRUE == can_do_sdb)
  {
    switch (flags)
    {
      case MSG_EXPEDITE:
        /*-------------------------------------------------------------------
          Nothing need to be done for ACH case.  Return TRUE.
        -------------------------------------------------------------------*/
        break;

      case MSG_FAST_EXPEDITE:
        /*-------------------------------------------------------------------
          Check if Reverse Enhanced Access channel is supported.
        -------------------------------------------------------------------*/
         if (ds707_sdb_is_reach_supported() == TRUE)
         {
           can_do_sdb = TRUE;
         }
         else
         {
           can_do_sdb = FALSE;
         }
         break;

      default:
        /*-------------------------------------------------------------------
          Should be caught in the input parameters check. If we got this far,
          got to check the code again.  Play safe and return FALSE.
        -------------------------------------------------------------------*/
         MSG_HIGH("Unknown FLAGS given for SDB",0,0,0);
         can_do_sdb = FALSE;
         break;

    } /* switch FLAGS */
  }/* if can_do_sdb is TRUE */

  return (can_do_sdb);

} /* ds707_sdb_supports_SDB_now() */


/*===========================================================================
FUNCTION      DS707_SDB_CM_TX_ERR_STATUS

DESCRIPTION   This call back function will be called by CM if it has some
              thing to inform DS for bad SDB transmissions.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_sdb_cm_tx_err_status
(
  void   *data_block_ptr,         /* Pointer to client provided data block */
  cm_dbm_cmd_e_type dbm_cmd, /*Indicate which pd cmd this err status is for*/
  cm_dbm_cmd_err_e_type dbm_cmd_err                /* Reason for rejection */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  MSG_ERROR("CM could not transmit SDB data",0,0,0);
} /* ds707_sdb_cm_tx_err_status() */


/*===========================================================================
FUNCTION      DS707_SDB_SR_ID_TO_IDX

DESCRIPTION   This function will return the index to SDB configuration based
              on SR_ID.

DEPENDENCIES  None.

RETURN VALUE  Value of the index.
              NOTE: If index value is equal(or greater) to DS_NUM_SDB, then
              the caller should reject it.

SIDE EFFECTS  None.
===========================================================================*/
byte ds707_sdb_sr_id_to_idx
(
  byte sr_id
)
{
  byte sdbi_index = 0;       /* i is used to identify it as local variable */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Search if we have any sr_id saved. If yes, return the index.
    NOTE: This may not be same as RLP. So, may not be good to
    couple RLP sr_id_index with this.
  -------------------------------------------------------------------------*/
  for(sdbi_index = 0; sdbi_index < DS707_SDB_NUM; sdbi_index++)
  {
    if (ds707_sdb_info.sdb_cfg[sdbi_index].sr_id == sr_id)
    {
      break;
    }
  }
  return(sdbi_index);
} /* ds707_sdb_sr_id_to_idx() */


/*===========================================================================
FUNCTION      DS707_SDB_TX_HANDLER

DESCRIPTION   Function that builds the SDB functionality.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL boolean ds707_sdb_tx_handler
(
  dsm_item_type * item_ptr
)
{
  byte              msg_buf_index = 0;   
  byte              sdb_ctl_byte = 0x0; 
  byte              sdb_hdr_buf[3];         /* buffer to hold the SDB hdr  */
  uint16            sdb_size;             
  int16             sock_id;              
  uint32            flags = CM_DBM_TRANSMIT_IN_CURR_PROTOCOL_STATE; 
  uint8             l2_ack_wait_time,l3_ack_wait_time;            
  ps_meta_info_type *meta_info_ptr;         /* pointer to meta info        */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Tx Handler should fill up the message buffer with the data to be
    transmitted on the SDB.
    If the Service Option == 33, then SO Omit field = 1; Data starts
    from 2nd byte
    If the Service Option != 33, then SO omit field = 0, fill SO in
    2,3rd bytes; Data starts at byte 4 in the message buffer.
    NOTE: FOR NOW WE DONOT SUPPORT SO != 33.
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Initializations. This will be written into first byte of the msg_buf
  -------------------------------------------------------------------------*/
  sdb_ctl_byte = 0x0; /* clear the first byte */

  /*-------------------------------------------------------------------------
    Confirm that the service option before we went dormant is SO 33.

    If (previous data call is SO 33)
      Use the SO omit field with 1 and build the SDB
    Else
      We donot support Non-SO 33 calls to have SDB.

    Note: Update the variable ds_prev_so whenever a call is connected from
    CM. Hence, this variable is called only in dormancy.
   ------------------------------------------------------------------------*/
   ds707_sdb_prev_data_call_so  = ds707_pkt_get_prev_call_so();
   if(ds707_sdb_prev_data_call_so == CAI_SO_PPP_PKT_DATA_3G)
   {
     /*----------------------------------------------------------------------
       Obtain the Meta Info pointer
       For sanity, check Meta_Info pointer. If it is not as expected, return
       FALSE. Why? Just for debugging. Most probably this should never happen.
     ----------------------------------------------------------------------*/
      meta_info_ptr = (ps_meta_info_type *) item_ptr->app_field;
      if(    (NULL == meta_info_ptr)           
          || (PS_META_INFO_VALID(meta_info_ptr) == FALSE)
        )
      {
         return (FALSE);
      }

     /*----------------------------------------------------------------------
       The SDB structure now looks as:
        ______________________________________________________________________
       |  1st Byte               | 2nd byte | 3rd byte| 2nd byte(if SO_OMIT=1) 
       |                         |          |         | 4th byte(if SO_OMIT=0)
       |_________________________|__________|_________|_______________________
       |3 bits  | 1 bit | 4 bits |  16 optional bits  |          
       |________|_______|________|__________ _________|_______________________
       |sr_id   |SO_Omit| Rsvd   |  Service Option    | PPP Data
       |________|_______|________|____________________|_______________________

       Copy the sr_id and SO omit information into the first byte
       Shift SR_ID by 5 bits to be appended at upper position.
       Since first byte is zeroed out initially, reserved bits are already 0.
     ----------------------------------------------------------------------*/

    msg_buf_index = 1;

    /*-----------------------------------------------------------------------
      By now sdb_ctl_byte should be zero, so OR'ing it is no problem.
    -----------------------------------------------------------------------*/
     ds707_sdb_prev_data_sr_id = ds707_pkt_get_prev_sr_id();
     sdb_ctl_byte |= (ds707_sdb_prev_data_sr_id << 5);

    /*-----------------------------------------------------------------------
      If the ds707_sdb_so_omit_indicator is set via the callback registered
      with MC (either in a Service Connect/UHDM) message, set the SO OMIT
      mask in the SDB control byte
    -----------------------------------------------------------------------*/
     if (ds707_sdb_so_omit_indicator == TRUE)
     {
       sdb_ctl_byte  |=  DS707_SDB_SO_OMIT_MASK;
       sdb_hdr_buf[0] = sdb_ctl_byte;
     }
     else
     {
       sdb_hdr_buf[0] = sdb_ctl_byte;
    /*-----------------------------------------------------------------------
      Since SO_OMIT is 0, include the Service option in the SDB header. 
      Populate the Service option(2 bytes) and bump the message index by 2
    -----------------------------------------------------------------------*/
       sdb_hdr_buf[1] = (CAI_SO_PPP_PKT_DATA_3G & 0xff00)>> 8;
       sdb_hdr_buf[2] = CAI_SO_PPP_PKT_DATA_3G & 0xff;
       msg_buf_index += 2;
     }

     MSG_MED("sdb_hdr[0] %x sdb_hdr[1] %x sdb_hdr[2] %x", sdb_hdr_buf[0], 
                                                          sdb_hdr_buf[1],  
                                                          sdb_hdr_buf[2]);  
                                                       
    /*-----------------------------------------------------------------------
      Copy the SDB header to the packet
      Use dsm_pushdown to push the header on to the top.
      Note: For one byte, we are writing the entire small item. Find out
      if we could do better.
    -----------------------------------------------------------------------*/
    dsm_pushdown ( &item_ptr,
                   sdb_hdr_buf,
                   msg_buf_index,
                   DSM_DS_SMALL_ITEM_POOL);
   }
   else
   {
     /* We should not have come this far; Since we came, how to handle it? */
     MSG_ERROR("No support for %x SDB Non-SO33 calls",
                ds707_sdb_prev_data_call_so,0,0);
   }

    /*-----------------------------------------------------------------------
      Obtain the information from Meta Info
    -----------------------------------------------------------------------*/
    sock_id = meta_info_ptr->sockfd;

    /*-----------------------------------------------------------------------
      Translate the flags from Sockets ENUMs to CM ENUMs.
      Sockets Flags:
            MSG_EXPEDITE
            MSG_FAST_EXPEDITE

      CM Flags:
            CM_DBM_TRANSMIT_AUTO,
            CM_DBM_TRANSMIT_ENH_ACC_OR_TRAFFIC,
            CM_DBM_TRANSMIT_ANY_OR_TRAFFIC,

      The flags are translated as below:
        SOCKETS                      CM
        MSG_EXPEDITE                 CM_DBM_TRANSMIT_IN_CURR_PROTOCOL_STATE
        MSG_FAST_EXPEDITE            CM_DBM_TRANSMIT_ENH_ACCESS_OR_TRAFFIC
        EXPEDITE | FAST_EXPEDITE     CM_DBM_TRANSMIT_IN_CURR_PROTOCOL_STATE

    Implementation:  By default make the flags as
    CM_DBM_TRANSMIT_ANY_OR_TRAFFIC. This will take care of ACH. Note that
    sanity check of the FLAGS was done elsewhere.
    -----------------------------------------------------------------------*/
    if( (meta_info_ptr->tx_flags & MSG_EXPEDITE) == MSG_EXPEDITE)
    {
      flags = CM_DBM_TRANSMIT_IN_CURR_PROTOCOL_STATE;
    }
    else if((meta_info_ptr->tx_flags & MSG_FAST_EXPEDITE)==MSG_FAST_EXPEDITE)
    {
      flags = CM_DBM_TRANSMIT_ENH_ACCESS_OR_TRAFFIC;
    }
    else
    {
      MSG_ERROR("unknown SDB flags %D. Assume ACH", 
                 meta_info_ptr->tx_flags,0,0);
    }

    /*-----------------------------------------------------------------------
      Currently we donot have support for wait time for l2_ack. We may need
      a socket  option later to decide the wait time.
      l3_ack.. is of no use for SDB. Set it so that CM is happy.
    -----------------------------------------------------------------------*/
    l2_ack_wait_time = 15; //This value is given by CM. Enquire them for now.
    l3_ack_wait_time = 0;

    /*-----------------------------------------------------------------------
      Obtain the length of the SDB packet
    -----------------------------------------------------------------------*/
    sdb_size = dsm_length_packet( item_ptr);

    /*-----------------------------------------------------------------------
        Write the data to the CM api with the following parameters:
        Burst_type =    SDB. CAI_SHORT_DATA_BURST
        Cmd_cb_func =   ds_sdb_tx_err_status_fn; This function is called
                        whenever CM has some errors. If CM has errors, DS
                        does not care and expects that the upper application
                        will know it.
                        For our debugging ease, we write this function to
                        printout some messages on the DM screen.
        Data_block_ptr: This contains the SOCK_ID. This sock id is obtained
                        from the socket
        Client:         DS client.
        Payload:        Pointer to the array that contains the outgoing data.
                        We give the dsm_item_ptr and expect CM to interpret
                        it. We provide a function that will copy from DSM
                        to CM buffers.
        NumBytes:       Size of the data. This is same used field in the
                        item ptr.
        payload_copy_func: This function is provided by DS to copy the data
                        from DSM items to CM buffer. This is provided because
                        CM does not understand DSM structure.
                        Pointer to callback function to copy payload from
                        client's memory space to CM's memory space
        l2_ack_wait_time: This should be a FLAG that we provide again at
                          dss_sendto level?
        l3_ack_wait_time  Maximum number of seconds to wait for receiving L3 ack
                          before sending the pref sys change command
        trans_pref      Type of channel used to send the data out.
                        Transmission preference like REACH, ACH etc..
    -----------------------------------------------------------------------*/

    cm_dbm_cmd_mo_msg(CAI_SHORT_DATA_BURST,
                      NULL,
                      (void *)sock_id,
                      ds3gi_cm_client_id,
                      item_ptr,
                      sdb_size,
                      (cm_dbm_payload_copy_cb_f_type *) ds707_sdb_to_cm_copy,
                      l2_ack_wait_time,
                      l3_ack_wait_time,
                      flags
                     );

    /*-----------------------------------------------------------------------
      Sent the data out to CM. House cleaning time.
      Free the meta info pointer.
    -----------------------------------------------------------------------*/
    PS_META_INFO_FREE(&meta_info_ptr);

    /*-----------------------------------------------------------------------
      By now we removed the packet from the watermark. Find out if any more
      data is queued up in the Watermark.
      If yes, we need to again call the empty watermark function again.
    -----------------------------------------------------------------------*/
    return(TRUE);

} /* ds707_sdb_tx_handler() */


/*===========================================================================
FUNCTION      DS707_SDB_RX_HANDLER

DESCRIPTION   This function will be registered by the DS on power on. This
              function will be called by the CM when ever it obtains the 
              SDB data.  Note that only DS will register for Receiving the
              SDB data.
              On receiving the incoming data, DS should copy the incoming
              data into the queue that is registered in the SDB. Then it
              should inform PS about it by calling the function.

DEPENDENCIES  We assume that the DS state is Idle/Dormant. Just to be safe,
              we also accept incoming SDB packet.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL void ds707_sdb_rx_handler
(
  uint8 *buf, 
  uint32 buf_len
)
{
  q_type *rx_q_ptr;            /*                     Pointer to receive Q */
  byte   sdb_ctl_byte;         /* Ctl Byte: The first byte of incoming pkt */
  word   sdb_so;               /*                           Service Option */
  byte   bs_sr_id;             /*                          SR-ID of the BS */
  byte   sr_id_idx;            /*                   Index of the RLP array */
  dsm_item_type *sdb_item_ptr; /*            Item to copy incoming data to */
  byte   sdb_index = 0;        /* Index in CM buffer: Points to data start */
  ps_iface_type        *pkt_iface_ptr;               /* ptr to pkt iface   */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  MSG_HIGH("Received SDB Message:DS CB called by CM",0,0,0);
  /*-------------------------------------------------------------------------
    Input parameters Sanity Check
    Buf pointer: Should not be NULL
    buf_len: should not be zero.
  -------------------------------------------------------------------------*/
  if((buf == NULL) || (buf_len == 0))
  {
    MSG_ERROR("invalid inputs: buf:%x, buf_len:%d",buf,buf_len,0);
    return;
  }

  /*-------------------------------------------------------------------------
    Sanity Check time:
    Check for correctness in incoming data. It includes:
    a) Service option of dormant data call is SO 33. For future compatibility
       we are not returning, if its not SO 33. We just accept it. Hopefully
       CM and CP levels, they donot accept SDB if its not SO 33 call.
    b) MS is currently dormant? We test that MS is dormant to receive,
       because SDB is designed mainly for that. DS will accept the data even
       if its not dormant and expects higher apps to throw it off.
    c) Sr_Id of BS and MS are same? This will be a good check to see.
  -------------------------------------------------------------------------*/
  pkt_iface_ptr = ds707_pkt_get_iface_ptr ( DS707_DEF_PKT_INSTANCE );
  if ( ds707_pkt_is_dormant (pkt_iface_ptr) != TRUE )
  {
    MSG_ERROR("Rxed SDB in Non-Dormant state",0,0,0);
  }
  if (ds707_pkt_get_prev_call_so() != CAI_SO_PPP_PKT_DATA_3G)
  {
    MSG_ERROR("Rxed SDB in Non-SO 33 call",0,0,0);
  }

  /*-------------------------------------------------------------------------
    The SDB structure now looks as:
     _________________________________________
    |  1st Byte                | 2nd byte on~E
    |_________________________|__________________
    |3 bits  | 1 bit | 4 bits |
    |________|_______|________|_______________
    |sr_id   |SO_Omit| Rsvd   | PPP Data
    |________|_______|________|_______________
  -------------------------------------------------------------------------*/
  /*-------------------------------------------------------------------------
    Check SR_ID between BS and MS match. Thrash if they donot match
    Obtain the first byte of incoming SDB. 
  -------------------------------------------------------------------------*/
  sdb_ctl_byte = buf[0];

  /*-------------------------------------------------------------------------
    Since SR_ID is the upper 3 bits, we should shift it by 5 bits to get the
    sr_id.
    Obtain the SR_ID_Index in the SDB_Array
  -------------------------------------------------------------------------*/
  bs_sr_id = (sdb_ctl_byte >> 5);
  ds707_sdb_prev_data_sr_id = ds707_pkt_get_prev_sr_id();
  if( ds707_sdb_prev_data_sr_id != bs_sr_id)
  {
      MSG_ERROR("BS:%d & MS:%d SR_ID mismatch",
                ds707_sdb_prev_data_sr_id, bs_sr_id, 0);
  }

  sr_id_idx = DSRLP_FIXED_SR_ID_INDEX;

  /*-------------------------------------------------------------------------
    Check if the SO omit field is set. Else check if the SO included in
    the SDB packet corresponds to SO33. Currently we support only for SO 33.
    If its not, flag an error stating SDB not supported for the SO
  -------------------------------------------------------------------------*/
  if ((sdb_ctl_byte & DS707_SDB_SO_OMIT_MASK) != DS707_SDB_SO_OMIT_MASK)
  {
    sdb_so = (word)buf[1]; /* INTERFACE bytes 2 & 3 */
    
    if (sdb_so != CAI_SO_PPP_PKT_DATA_3G)
    {
      MSG_ERROR("rx'ed SDB for SO %x:Not Supported",sdb_so,0,0);
    }
  }

  /*-------------------------------------------------------------------------
    Check for correctness in SDB Rx path to DS:
      a) Confirm the Receive queue is registered
      b) Confirm that the recieve queue does not have any more data.

    Obtain the rx queue into which outgoing data has to be copied
    Confirm the Receive queue is registered correctly. If its not, then
    thrash the message.
  -------------------------------------------------------------------------*/
  rx_q_ptr = dsrlpi_rscb_array[sr_id_idx].cfg.rx_q_ptr;

  if(rx_q_ptr == NULL)
  {
    MSG_ERROR("No Rx Queue for Rx SDB",0,0,0);
    return;
  }

  /*-------------------------------------------------------------------------
    Confirm the Receive queue is empty. If its not empty, nothing much we
    can do now. Queue it any way after printing the message.
  -------------------------------------------------------------------------*/
  if(q_cnt(rx_q_ptr) != 0)
  {
    MSG_ERROR("SDB Write to non-empty queue",0,0,0);
  }

  /*-------------------------------------------------------------------------
    Remove the SDB header from the incoming packet.
    if SO Omit field is set to ONE, then copy from 2nd byte. If its set to
    0, copy from 4th byte onwards.
  -------------------------------------------------------------------------*/
  if( (sdb_ctl_byte & DS707_SDB_SO_OMIT_MASK) == DS707_SDB_SO_OMIT_MASK)
  {
  /*-------------------------------------------------------------------------
    buf_len should be >1 to have atleast one PPP byte on the payload 
  -------------------------------------------------------------------------*/
      ASSERT(buf_len > 1); 
      buf_len--;
      sdb_index++;
  }
  else
  {
  /*-------------------------------------------------------------------------
    buf_len should be >3 to have atleast one PPP byte on the payload 
  -------------------------------------------------------------------------*/
      ASSERT(buf_len > 3); 
      buf_len -= 3;
      sdb_index += 3;
  }

  /*-------------------------------------------------------------------------
    Obtain a new dsm item and copy the data from the buffer to the DSM_ITEM.
    We know that SDB packet cannot be more than 256 bytes, so we may chain 
    3-4 small items. So take a SMALL item.
    This is done to pass the received data up the data stack.
  -------------------------------------------------------------------------*/
  sdb_item_ptr = dsm_new_buffer(DSM_DS_SMALL_ITEM_POOL);
  dsm_pushdown(&sdb_item_ptr, &(buf[sdb_index]), buf_len, 
               DSM_DS_SMALL_ITEM_POOL);
  /*-------------------------------------------------------------------------
    Copy the data to the queue.
  -------------------------------------------------------------------------*/
  q_put(rx_q_ptr,&(sdb_item_ptr->link));

  /*-------------------------------------------------------------------------
    Now call the RX function. This will set the flag informing incoming data
  -------------------------------------------------------------------------*/
  if(dsrlpi_rscb_array[sr_id_idx].cfg.rx_func_ptr != NULL)
  {
    dsrlpi_rscb_array[sr_id_idx].cfg.rx_func_ptr();
  }
  else
  {
    /*-----------------------------------------------------------------------
      We should not come here. But just in case... let's inform developer.
    -----------------------------------------------------------------------*/
    MSG_ERROR("No function for SDB Rx",0,0,0);
  }

} /* ds707_sdb_rx_handler() */


/*===========================================================================
FUNCTION      DS707_SDB_TO_CM_COPY

DESCRIPTION   Copies to buffer.

DEPENDENCIES  This function assumes that any length data is valid. So, its
              upto calling functions to see if they are not happy with length
              passed here.

RETURN VALUE  -1: if there is an error (e.g. a parameter is invalid)
               0: if there were 'len' or fiewer bytes to copy from the packet
               chain number of bytes that were not copied

SIDE EFFECTS  None.
===========================================================================*/
LOCAL int8  ds707_sdb_to_cm_copy
(
  void *dsm_item_ref_ptr,
  void *buf,
  int  *len_ptr
)
{
  int8 ret;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Check the input values. If they are unacceptable, just return from
    function without doing anything.
  -------------------------------------------------------------------------*/
  if(  (NULL == dsm_item_ref_ptr) || (NULL == buf) || (NULL == len_ptr) )
  {
    MSG_ERROR("Invalid SDB ptrs Data:%x,CM:%x,len:%x",
              dsm_item_ref_ptr,
              buf,
              len_ptr);
    return (-1);
  }

  /*-------------------------------------------------------------------------
    Copy the data to the Buffer
  -------------------------------------------------------------------------*/
  *len_ptr -= dsm_pullup((dsm_item_type**)(&dsm_item_ref_ptr), (void*) buf, 
                          *len_ptr);
  ret = dsm_length_packet((dsm_item_type*)dsm_item_ref_ptr);
  dsm_free_packet((dsm_item_type**)(&dsm_item_ref_ptr));

  return ret;
} /* ds707_sdb_to_cm_copy() */


/*===========================================================================
FUNCTION      DS707_SDB_RADIO_TRANSPORT_IN_DORM

DESCRIPTION   To decide the type of Radio Transport Protocol for CDMA2000 
              look at the following:
              a) What transport does Packet request. If SDB it may request
                 explicitly. For RLP, packets may not specify any thing. If
                 the packet does not specify anything assume its RLP.
              b) What transport can Mobile support Now: If its dormant and
                 SO 33 call, it will be able to support SDB. Else, it will
                 only support RLP.

              Confirm that the current packet wants to go on SDB by
              looking at its Meta Info. If it does not desire SDB, return
              radio_transport as RLP and it will originate a call.

DEPENDENCIES  None.

RETURN VALUE  None.

SIDE EFFECTS  None.
===========================================================================*/
LOCAL ds707_radio_transport_in_dorm_type ds707_sdb_radio_transport_in_dorm
(
  dsm_watermark_type * tx_wm_ptr,           /* The RLP tx watermark        */
  byte               * num_sdb_pkts_ptr,    /* Total number of SDB packets */
  boolean            * set_sig     /* Signal for re-orig. Currently unused */
)
{
  uint32                       SDB_flags = 0;           /* Initialize      */
  dsm_item_type                *item_ptr;               /* ptr to DSM item */
  q_type                       *txq;
  ds707_radio_transport_in_dorm_type radio_transport_ret_val 
                                               = RADIO_TRANSPORT_RLP3;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*-------------------------------------------------------------------------
    Confirm that the input paramters are valid.
    If they are not valid, play safe and return RLP.
  -------------------------------------------------------------------------*/
  if ( ( NULL == tx_wm_ptr)              ||
       ( NULL == num_sdb_pkts_ptr)       ||
       ( NULL == set_sig)
     )
  {
    MSG_ERROR("Null ptr:Tx WM %x,NumPtr:%x,SetSig:%x",
              tx_wm_ptr,
              num_sdb_pkts_ptr,
              set_sig);

    /*-----------------------------------------------------------------------
      For safety, always originate an RLP call if we dont know what to do.
      We already initialized radio_transport_ret_val = RADIO_TRANSPORT_RLP3
    -----------------------------------------------------------------------*/
    return (radio_transport_ret_val);
  }

  txq = tx_wm_ptr->q_ptr;

  /*-------------------------------------------------------------------------
    To play safe, set the output values to safe value.
    Flag to FALSE. Later check if it should be set to TRUE.
    Number of SDB packets to 0; Calculate number of packets to be sent
    on SDB by looping the tx_wm
  -------------------------------------------------------------------------*/
  *set_sig = FALSE;
  *num_sdb_pkts_ptr = 0;

  /*-------------------------------------------------------------------------
    Confirm if the water mark has data or not. If it does not have any data
    then, bring up a data call.
  -------------------------------------------------------------------------*/
  if (0 == tx_wm_ptr->current_cnt)
  {
    MSG_ERROR("Tx WM cnt = %d;Originate TC",tx_wm_ptr->current_cnt,0,0);
    return (radio_transport_ret_val);
  }

   /*------------------------------------------------------------------------
     Obtain the pointer to the packet

     //NOTE: Modify logic here to say if we support Multiple SDB packets
     while (item_ptr != NULL && radio_transport_ret_val != RLP3)
   ------------------------------------------------------------------------*/
   if((item_ptr = (dsm_item_type *)q_check(tx_wm_ptr->q_ptr)) == NULL)
   {
       MSG_ERROR("No items in Transmit Queue",0,0,0);
   }

  /*-------------------------------------------------------------------------
     Check what transport does Packet request. If SDB it may request
     explicitly in its META information. If it is RLP, packets may not
     specify any thing. So if the packet META info has no information assume
     its RLP.

     SDB_flags is passed by reference. ds_curr_pkt_needs_SDB will write the
     value of the flags here. Use this to decide if the configuration can be
     supported by DS now.
  -------------------------------------------------------------------------*/
  if (ds707_sdb_curr_pkt_needs_SDB(item_ptr, &SDB_flags) == TRUE )
  {
    MSG_MED("Packet Wants SDB",0,0,0);
    (*num_sdb_pkts_ptr)++;
    radio_transport_ret_val = RADIO_TRANSPORT_SDB;
  }  

  /*-------------------------------------------------------------------------
     Note: Add logic here to see if there is any non-SDB packet beyond
             SDB packets.If yes, set flag to send a signal to DS again.
  -------------------------------------------------------------------------*/
  if (radio_transport_ret_val != RADIO_TRANSPORT_RLP3)
  {

    /*------------------------------------------------------------------------
      If we come until here, we know, outgoing packet prefers SDB. So let's
      see what transport can Mobile support Now:
      If its dormant and SO 33 call: it will be able to support SDB.
      Else: it will only support RLP.

      But we also have to decide if we can send the outgoing packet on the
      desired channel (i.e. ACH or REACH)
      SDB_flags is passed by value. This flag will inform we we need ACH or
      REACH or anything.
    ------------------------------------------------------------------------*/
    if(ds707_sdb_supports_SDB_now(SDB_flags) == TRUE )
    {
      MSG_HIGH("Sending SDB Packet",0,0,0);
      radio_transport_ret_val = RADIO_TRANSPORT_SDB;
    }
    else
    {
      radio_transport_ret_val = RADIO_TRANSPORT_RLP3;
    }
  }

  return (radio_transport_ret_val);

} /* ds707_sdb_radio_transport_in_dorm() */

/*===========================================================================
FUNCTION      DS707_SDB_SO_OMIT_HANDLER

DESCRIPTION   This is the callback function registered with MC (hence it 
              runs in the MC context). It just updates the SO_OMIT_INDICATOR
              as informed by the base station

DEPENDENCIES  None.

RETURN VALUE  None

SIDE EFFECTS  None.
===========================================================================*/
void ds707_sdb_so_omit_handler
(
  boolean so_omit_indicator
)
{
  /*------------------------------------------------------------------------
    Save the SO OMIT indicator to use it while transmitting SDB
  ------------------------------------------------------------------------*/
  MSG_MED("Changing SO Omit from %d to %d", ds707_sdb_so_omit_indicator, 
                                            so_omit_indicator, 
                                            0);
  ds707_sdb_so_omit_indicator = so_omit_indicator;
} 

#endif /* FEATURE_DATA_IS2000_SDB */
#endif /* FEATURE_DATA_IS707      */
