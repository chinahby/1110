/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                      P S _ P P P _ N E T M O D E L . C

GENERAL DESCRIPTION

  This contains the code for Data Services network model.

  FEATURES:
    FEATURE_DS_NET_MODEL_DROPS_CALL: if this feature is defined an event that
        would trigger an end-to-end resync will result in the call dropping.
        If the feature is not defined an end-to-end resync will occur.

EXTERNALIZED FUNCTIONS
  These functions are defined in dsnetmdl.h and are externally visible to
  this file.

  netmdl_set_meta_state()
    Change the Meta state of PPP.

  netmdl_get_meta_state()
    Return the Meta state of PPP.

  netmdl_reset_ppp()
    reset the TE-IWF PPP by either terminatingf the link or restarting LCP

  Copyright (c) 1998-2011 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_netmodel.c_v   1.17   03 Feb 2003 19:25:44   jeffd  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_netmodel.c#13 $
  $Author: nsivakum $ $DateTime: 2011/03/09 10:06:08 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
03/09/11    sn     Fix to read RM NAI (multiple NAI) only if phone is in NV 
                   mode.
04/19/10    sn     Merged support for PAP internal auth.
11/30/09    ps     Fixed the issue of not picking up credentials from DUN when
                   Legacy RUIM card is inserted in an OMH phone.
05/26/09    sn     Merged fix in ppp_down_callback() - Instead of sending the 
                   term req to the bridged PPP explicitly tear it down.
04/29/09    sn     Ported support for call throttle feature (DCTM).
04/16/09    sn     Merged support for separate NAI for Tethered SIP and MIP 
                   calls. Featurized under FEATURE_DS_MULTIPLE_NAI.
02/11/09    psng   Fixed CHAP failure issue when MIP DMU feature is off and AT
                   is in NV mode.
06/28/08    pp     Fixed RVCT compiler warnings.
04/03/08    pp     Klocwork fixes.
02/22/08    pp     Klocwork fixes.
02/15/08    sn     Merged lint error fix.
09/21/07 sq/mga/yz Merged changes to support FEATURE_UIM_SUPPORT_3GPD.
07/05/04    jd     CCP will be rejected on both Rm and Um interfaces if 
                   FEATURE_DATA_PS_CCP_REJECT is turned on
07/02/04    jd     Fix slot-id check bug and check # rev link slots in 
                   compression check in compare_ipcp_opts
01/09/04    mvl    Merged in fix for bad saving of the PAP code.
08/19/03    jd     Ensure IPCP complete callback is reset on transition to
                   OOC metastate
05/13/03    usb    Always enable flow when going to OOC meta state, check
                   for NULL bridge iface in netmdli_ppp_down_callback().
05/05/03    jd     Removed dependency on dsiface.h
04/17/03    mvl    Fixed feature check for DMU and JCDMA as there was a typo
04/11/03    jd     Added wrappers to make sure netmdl unreg's only the snoop
                   callbacks it itself has registered
04/11/03    ak     Updated for new 707 RMSM interfaces.
04/02/03    mvl    Complete modularization of the authentication code.
03/26/03    mvl    removed authentication featurization
03/19/03    jd     Include pstimer.h for dmu builds to fix compiler warning
03/19/03    mvl    Fixed bug where which options were negotiated was not
                   being stored correctly.
02/02/03    usb    Updated snooop callback functions with new return values
12/30/02   mvl/jd  Added code to do handle CHAP within the handset during 
                   netmodel calls for DMU (under FEATURE_DS_MOBILE_IP_DMU).  
                   Will also swallow CHAP response from laptop.
12/04/02    usb    Wrapped ds707_rmsm.h in FEATURE_DATA_IS707
11/27/02   na/ak   Fixed compile time problem in netmdli_ppp_resynced.
11/14/02   jd/usb  Added netmdl_set_resync_behaviour() to select between 
                   "drop call" (for MIP) and "end-to-end" resync (SimpleIP)
                   when Um resync results in Um-Rm PPP option mismatch.
                   If netmodel drops call on resync, now does it nicely
                   through RMSM event rather than ppp_stop.
11/04/02    mvl    renamed CHAP_RESYNC feature to JCDMA_1X and moved #defines
                   that were only used in this file here from the header.
10/25/02    usb    Modified the condition to go to INIT state while resyncing 
                   for UMTS PPP calls.
09/12/02    mvl    Generified the network model resync 
                   callcack and some general cleanup.  Removed netmodel 
                   featurization.
09/03/02    usb    Fixed netmdli_ppp_down_callback to get iface_ptr from the
                   PPP control block
08/13/02    mvl    Removed the iface_cb_array and have a curr_id array
                   instead (all the other items are in the PPP cb) moved the
                   set/get_mode() functions to pppi.c.
08/07/02    mvl    Fixed the DROPS_CALL feature so now we close gracefully.
08/06/02    mvl    Reworked authentication.
07/25/02    mvl    Updates for PPP renaming.
07/16/02    mvl    Fixed bug in ACCM comparison.
07/15/02    usb    Moved ipcp_c_ack_status initialization to OOC meta state 
                   from INIT meta state.
07/11/02    mvl    Updates for new PPP architecture.
06/16/02    usb    Changed the function netmdli_ppp_down_callback based upon
                   new architecture
05/22/02    mvl    changed all of the interface names to MM PS values. 
05/20/02     vr    Changed hdlc_enabled flag to hdlc_mode
05/08/02    mvl    File cleanup and added arg to ppp_outgoing() for new proto
03/16/02    vas    Fixed typo in netmdli_init_callback_chap_resp()
03/01/02    vas    Updated to allow authentication (PAP/CHAP) parameters to
                   be allocated per iface.
02/20/02    sjy    Mobile IP performance now featurized under 
                   FEATURE_DS_MOBILE_IP_PERF
02/07/02    sjy    Added support for Mobile IP call performance metrics.
02/07/02    vsk    Updated error message when dropping pkt in 
                   netmdli_init_callback_lcp_c_ack
12/21/01    dwp    Let netmdli_init_callback_lcp_c_ack() work in FULL_NET_MODE
12/18/01    rsl    Updated tracer field.
11/12/01    dwp    Use FEATURE_DATA for old/new DS Mgr dependencies. Keep the
                   R interface in FULL NET mode when the U has HDLC disabled.
09/25/01    mvl    Fixed variable decalrations in netmdli_ppp_resynced() so
                   that only appropriate variables are defined for drop call
                   feature.
09/20/01    mvl    Setting the will field in the save_opts() function because
                   it is not set otherwise (due to PPP restructure).
09/13/01    mvl    Removed optimization from the save_opts() function as it
                   was incompatible with DNS mods.
08/22/01    sjy    Removed duplicate code in netmdli_compare_lcp_opts(); 
                   it now calls netmdl_compare_lcp_opts() to carry out the 
                   LCP option comparisons
08/03/01    mvl    Removed Mobile IP specific code because network model is
                   not used to bring up Mobile IP calls.
07/31/01    mvl    Made modification to the IPCP comparison logic to only
                   compare IP addresses if both the Um and Rm wanted to
                   negotiate for IP addresses.
07/30/01    sjy    Added functions netmdl_compare_lcp_opts() and 
                   netmdl_compare_ipcp_opts() to compare IPCP and LCP options 
                   on UM and RM interfaces.
07/30/01    mvl    Stripped out M.IP code, as network model is not used
                   during call setup.  Made a fix in save_options to save the
                   default options if they were negotiated.
07/27/01    aku    Removed calls to dsiface_mip_reset. No longer required
                   due to M. IP redesign
07/27/01    mvl    Fixed compile time warning.
07/23/01    pjb	   Removed the DS-PS command queue.  Move PS kinds on to PS.
                   Changed calls to dsm_offset_new_buffer to allocate space
		   for headers. 
07/22/01    mvl    removed restriction against putting Rm in internal mode.
07/10/01    na     Added extra debug messages.
07/07/01    mvl    Added M.IP support for the case when the TE is ignoring
                   hardware flow control - packets will be discarded.
06/27/01    mvl    modified debug message to give more info.
06/26/01    mvl    now resetting Meta State to init when resetting PPP.
06/21/01    mvl    Added PPP meta state variable and netmdl_get_meta_state()
                   to check state and netmdl_reset_ppp() function
06/20/01    mvl    Fixed several bugs to support M.IP->Simple IP fallback.
06/14/01    mvl    Fixed some debug messages to be more meaningful.
06/04/01  jd/mvl   Resolved Ip_addr byte-ordering issues
05/02/01    na     When PPP negotiation with laptop was complete, meta state
                   was not being set to UP. Now fixed.
05/18/01    mvl    Added Mobile IP support code.
08/25/00    rc     Changed dsm_new_buffer() calls to take pool id as input 
                   parameter. 
06/30/00    ttl    Adds FEATURE_DS_PPP_TERM_REQ_DROP which will avoid a call
                   origination when the call is in dormant idle state in
                   network model mode.
05/04/00    mvl    Changed calls to ppp_outgoing() to reflect new interface.
04/24/00    snn    Renamed ds_prev_nw_ppp_call_had_pap to 
                   ds_prev_nw_ppp_call_had_auth because this flag is now used
                   for both PAP and CHAP.
                   Added a new pointer, ppp_chap_resync_passwd. This will
                   store the default password to be used CHAP resync occurs
                   in N/W model.
                   Added a new function netmdli_init_callback_chap_resp for
                   doing the chap call back.
04/12/00    mvl    Added new compile time feature that will cause a call to
                   drop rather than ce an end-to-end resync.
02/17/00    mvl    Fixed some typos that were causing compilation failures if
                   MSG_LOW()s were compiled in.
10/27/99    mvl    Code review clean up and much more efficient
                   implementation of the IPCP C-Ack code.
09/27/99    snn    Rewrote the pap call back function to be more code 
                   efficient.
09/14/99    mvl    Changed a message that was misleading.
09/01/99    mvl    Removed support for full network mode - this included
                   changing the way that options are compared, and removing
                   support for only resyncing IPCP.
08/20/99    mvl    Changed the way that options are saved.  Only saving Um
                   want and Rm work and want.  Um work is not saved.
08/19/99    mvl    Moved the LCP and IPCP option saving routines to ppplcp.c
                   and pppipcp.c respectively.
08/12/99    mvl    Fixed the IPCP option checking after a resync - not all
                   the compression possibilities were being examined.
08/10/99    snn    modified comments to the PAP call back function.
07/28/99    snn    Added support for the PAP.
06/07/99    snn    Disable Rm flow control in the function 
                   netmdli_ppp_resynced() after the IP address mismatch
06/07/99    snn    In netmdli_ppp_resynced() function deregistered function 
                   initially, so that it takes effect even if the IP
                   addresses do not match. 
10/04/98  mvl/na   Created module.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"

#ifdef FEATURE_DATA
#include "assert.h"
#include "ds3gsiolib.h"
#include "ps_ppp_snoop.h"
#include "dsm.h"
#include "ps_ppp_netmodel.h"
#include "err.h"
#include "memory.h"
#include "msg.h"
#include "ps_ppp.h"
#include "ps_ppp_defs.h"
#include "ps_pppi.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_ipcp.h"
#include "ps_ppp_lcp.h"
#include "ps_ppp_auth.h"
#include "ps_iface.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_rmsm.h"  // we should abstract this call somehow?
#endif /* FEATURE_DATA_IS707 */

#ifdef FEATURE_DS_MULTIPLE_NAI
#include "ps_utils.h"
#endif /* FEATURE_DS_MULTIPLE_NAI */

#ifdef FEATURE_DS_MOBILE_IP_PERF
#include "dsmip_perf.h"
#endif /* FEATURE_DS_MOBILE_IP_PERF */

#if defined(FEATURE_DS_MOBILE_IP_DMU) || defined(FEATURE_UIM_SUPPORT_3GPD)
#include "pstimer.h"
#endif /* FEATURE_DS_MOBILE_IP_DMU || FEATURE_UIM_SUPPORT_3GPD */

#ifdef FEATURE_UIM_SUPPORT_3GPD
#include "nvruimi.h"
#endif /* FEATURE_UIM_SUPPORT_3GPD */

#ifdef FEATURE_CALL_THROTTLE
#include "dsdctm.h"
#endif /* FEATURE_CALL_THROTTLE */

/*===========================================================================
  make sure that the the CHAP_RESYNC feature and the DMU feature are not on
  at the same time as they are NOT compatible!
===========================================================================*/
#if defined(FEATURE_JCDMA_1X) && defined(FEATURE_DS_MOBILE_IP_DMU)
#error FEATUREs JCDMA_1X and DS_MOBILE_IP_DMU are mutually exclusive!
#endif /* FEATURE_JCDMA_1X || FEATURE_DS_MOBILE_IP_DMU */

#if defined(FEATURE_DS_MOBILE_IP_DMU) || defined(FEATURE_UIM_SUPPORT_3GPD)
#define FEATURE_DATA_PS_INTERNAL_AUTH
#endif /* FEATURE_DS_MOBILE_IP_DMU || FEATURE_UIM_SUPPORT_3GPD */

#if defined(FEATURE_DATA_PS_INTERNAL_AUTH) && !defined(FEATURE_DSM_DUP_ITEMS)
#error FEATUREs FEATURE_DATA_PS_INTERNAL_AUTH requires DSM_DUP_ITEMS
#endif /* FEATURE_DATA_PS_INTERNAL_AUTH && !FEATURE_DSM_DUP_ITEMS */


/*===========================================================================

            REGIONAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
  The interface control block array.
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  the current PPP id to use.
---------------------------------------------------------------------------*/
byte curr_ppp_id[PPP_MAX_DEV];


/*---------------------------------------------------------------------------
  The state variable for the network model state machine.
---------------------------------------------------------------------------*/
LOCAL netmdl_meta_state_enum_type netmdli_state = PPP_OOC_META_STATE;

/*---------------------------------------------------------------------------
  Runtime configurable resync failure behaviour
---------------------------------------------------------------------------*/
#ifdef FEATURE_DS_NET_MODEL_DROPS_CALL
LOCAL boolean netmdl_drop_call = TRUE;
#else
LOCAL boolean netmdl_drop_call = FALSE;
#endif /* FEATURE_DS_NET_MODEL_DROPS_CALL */

#if defined(FEATURE_DS_MULTIPLE_NAI) && defined(FEATURE_DATA_PS_INTERNAL_AUTH)
LOCAL void netmdli_copy_rm_nai
(
  ppp_dev_enum_type device
);
#endif /* FEATURE_DS_MULTIPLE_NAI && FEATURE_DATA_PS_INTERNAL_AUTH */
/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/

/*---------------------------------------------------------------------------
   Generic error return values.
---------------------------------------------------------------------------*/
#define RET_ERROR -1
#define RET_OK     0

/*---------------------------------------------------------------------------
  'typedef' for the IPCP config ack callbacks.
---------------------------------------------------------------------------*/
typedef enum
{
  NO_ACK_RCVD    = 0x0,
  Rm_ACK_RCVD    = 0x1,
  Um_ACK_RCVD    = 0x2,
  BOTH_ACKS_RCVD = 0x3
} ack_status_enum_type;

/*---------------------------------------------------------------------------
  'typedef' for the LCP option comparison routine

  NOTE: the values allow assignment to a bit mask, so it is possible to
  compose multiple values together to return more than one value.
---------------------------------------------------------------------------*/
typedef enum
{
  NO_LCP_CHANGED     = 0x0,
  MOBILE_LCP_CHANGED = 0x1,
  IWF_LCP_CHANGED    = 0x2
} lcp_opt_change_enum_type;

/*---------------------------------------------------------------------------
  Variables that maintain snoop/callback state information, such as the
  current LCP and IPCP id numbers.
---------------------------------------------------------------------------*/
LOCAL int ipcp_c_ack_status = NO_ACK_RCVD;  /* c-ack status                */

#ifdef FEATURE_JCDMA_1X
/*---------------------------------------------------------------------------
  ppp_chap_resync_passwd is a constant pointer and contains the password to 
  be used during the Resync in the network model call.
  Note that the password used during the Network model resync is different
  from the password used before.
---------------------------------------------------------------------------*/
#define PPP_CHAP_RESYNC_PASSWD  "Packet-One000001"
#define CHAP_RESYNC_PASSWD_LENGTH (sizeof(PPP_CHAP_RESYNC_PASSWD) -1 )

const char *ppp_chap_resync_passwd = PPP_CHAP_RESYNC_PASSWD;
#endif /* FEATURE_JCDMA_1X */

/*---------------------------------------------------------------------------
  Array to keep track of callbacks registered with snoop.

  MAX_SNOOPED_MESSAGES must be increased if more callbacks are added.
---------------------------------------------------------------------------*/
#define MAX_SNOOPED_MESSAGES (15)
LOCAL uint32 snoop_id_array[MAX_SNOOPED_MESSAGES];

/*---------------------------------------------------------------------------
  Number of snooped message id's in the netmdl snoop list.  The whole list
  must be deregistered at once, as we can't handle insertion/shift-deletion
---------------------------------------------------------------------------*/
LOCAL byte snoop_id_array_entries = 0;



/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =

                          LOCAL FUNCTION DEFINTIONS

= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
FUNCTION NETMDLI_SAVE_OPTS()

DESCRIPTION
  This function will go through all of the options in a C-Ack and store them
  in the appropriate control block (i.e. LCP vs. IPCP).  It only saves the
  want values for the Um interface, and stores the work and want values for
  the Rm interface.  This is done because the Um work is irrelevant until a
  renegotiation happens, and at that point these options are reset.

DEPENDENCIES
  Expects the data_ptr in the item to point to the code portion of the PPP
  packet.

  Assumes that it will ONLY be call for LCP or IPCP

  lcp_init() and ipcp_init() should have been called before this function.

RETURN VALUE
  RET_ERROR (-1) if there is an error (i.e. parsing of the packet breaks).
  RET_OK    (0)  on sucessful completion of recording all options in packet.

SIDE EFFECTS
  The local portion of the control block for the protocol in question is
  updated to contain the information in the C-Ack.
===========================================================================*/
LOCAL sint15 netmdli_save_opts
(
  dsm_item_type **item_head_ptr, /* Ptr to PPP packet                      */
  uint16 protocol,               /* protocol whose options are being saved */
  ppp_fsm_side_type* rm_opt_ptr, /* Ptr to Rm options                      */
  ppp_fsm_side_type* um_opt_ptr  /* Ptr to Um options                      */
)
{
  dsm_item_type* item_ptr;       /* Working ptr to PPP packet              */
  struct option_hdr opt_hdr = {0};/* option header                          */
  struct config_hdr cfg_hdr = {0};/* Config hdr: has Code, ID, len          */
  sint31 pkt_len;                /* Length of the entire packet            */
  uint16 offset;                 /* offset from begining of current item   */
  uint16 copy_len;               /* Number of bytes copied into buffer     */
  byte opt_val[PPP_OPT_MAX_LEN]; /* array to copy option value into        */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if( item_head_ptr == NULL ||
      *item_head_ptr == NULL ||
      rm_opt_ptr == NULL ||
      um_opt_ptr == NULL )
  {
    ASSERT( 0 );
    return RET_ERROR;
  }

  item_ptr = *item_head_ptr;

  /*-------------------------------------------------------------------------
    Extract the code, id and length fields (4 bytes) into cfg_hdr. Convert
    the length from network to host.
  -------------------------------------------------------------------------*/
  copy_len = CONFIG_HDR_LEN;
  offset   = 0;
  if(dsm_seek_extract(&item_ptr, &offset, &cfg_hdr, &copy_len) == FALSE)
  {
    ERR("seek_extract() failed - dumping pkt", 0, 0, 0);
    dsm_free_packet(item_head_ptr);
    return RET_ERROR;
  }
  pkt_len = ntohs(cfg_hdr.length);
  pkt_len -= CONFIG_HDR_LEN;
  offset += CONFIG_HDR_LEN;

  /*-------------------------------------------------------------------------
    Reset the negotiatied option fields to zero
  -------------------------------------------------------------------------*/
  rm_opt_ptr->work = 0;
  um_opt_ptr->want = 0;
  if(protocol ==  PPP_LCP_PROTOCOL)
  {
    memset(rm_opt_ptr->work_pdv, 0, sizeof(lcp_value_type));
  }

  else /* is IPCP */
  {
    memset(rm_opt_ptr->work_pdv, 0, sizeof(ipcp_value_type));
  } /* if(LCP) */

  /*-------------------------------------------------------------------------
    Go through the entire PPP packet, one option at a time.
  -------------------------------------------------------------------------*/
  while ( pkt_len > 0)
  {
    /*-----------------------------------------------------------------------
      Get the option header, and update the packet length to reflect that the
      header has been grabbed.
    -----------------------------------------------------------------------*/
    copy_len = OPTION_HDR_LEN;
    if(dsm_seek_extract(&item_ptr, &offset, &opt_hdr, &copy_len) == FALSE)
    {
      ERR( "seek_extract() failed - dumping pkt", 0, 0, 0);
      dsm_free_packet( item_head_ptr);
      return RET_ERROR;
    }
    pkt_len -= opt_hdr.length;

    /*-----------------------------------------------------------------------
      Get the options value IFF it has one.
    -----------------------------------------------------------------------*/
    offset += OPTION_HDR_LEN;
    copy_len =  opt_hdr.length - OPTION_HDR_LEN;
    if ( copy_len > 0)
    {
      if(dsm_seek_extract(&item_ptr, &offset, opt_val, &copy_len) == FALSE)
      {
        ERR( "seek_extract() failed - dumping pkt", 0, 0, 0);
        dsm_free_packet( item_head_ptr);
        return RET_ERROR;
       }
      offset += copy_len;
    }

    /*-----------------------------------------------------------------------
      Save the option for the protocol in question.
    -----------------------------------------------------------------------*/
    if(protocol == PPP_LCP_PROTOCOL)
    {
      lcp_save_work_opt(rm_opt_ptr, opt_hdr.type, opt_val);
    }

    else /* is IPCP */
    {
      ipcp_save_work_opt(rm_opt_ptr, opt_hdr.type, opt_val);
    }

  } /* while(more options) */

  /*-------------------------------------------------------------------------
    Save the negotiated work values into the future want values, and also the
    work_pdv into the want_pdv.
  -------------------------------------------------------------------------*/
  rm_opt_ptr->will = rm_opt_ptr->work;
  rm_opt_ptr->want = rm_opt_ptr->work;
  um_opt_ptr->will = rm_opt_ptr->work;
  um_opt_ptr->want = rm_opt_ptr->work;
  um_opt_ptr->work = rm_opt_ptr->work;
  
  if(protocol ==  PPP_LCP_PROTOCOL)
  {
    memcpy(((lcp_value_type*)(rm_opt_ptr->want_pdv)),
           ((lcp_value_type*)(rm_opt_ptr->work_pdv)),
           sizeof(lcp_value_type));
    memcpy(((lcp_value_type*)(um_opt_ptr->want_pdv)),
           ((lcp_value_type*)(rm_opt_ptr->work_pdv)),
           sizeof(lcp_value_type));
    memcpy(((lcp_value_type*)(um_opt_ptr->work_pdv)),
           ((lcp_value_type*)(rm_opt_ptr->work_pdv)),
           sizeof(lcp_value_type));
  }

  else /* is IPCP */
  {
    memcpy(((ipcp_value_type*)(rm_opt_ptr->want_pdv)),
           ((ipcp_value_type*)(rm_opt_ptr->work_pdv)),
           sizeof(ipcp_value_type));
    memcpy(((ipcp_value_type*)(um_opt_ptr->want_pdv)),
           ((ipcp_value_type*)(rm_opt_ptr->work_pdv)),
           sizeof(ipcp_value_type));
    memcpy(((ipcp_value_type*)(um_opt_ptr->work_pdv)),
           ((ipcp_value_type*)(rm_opt_ptr->work_pdv)),
           sizeof(ipcp_value_type));
  } /* if(LCP) */

  /*-------------------------------------------------------------------------
    And we are done.
  -------------------------------------------------------------------------*/
  MSG_MED("%x loc opts %x, rem opts %x",
          protocol,
          rm_opt_ptr->work,
          rm_opt_ptr->work);

  return RET_OK;

  /*lint +e613 */
} /* netmdli_save_opts() */



/*===========================================================================
FUNCTION   NETMDLI_INIT_CALLBACK_LCP_C_REQ()

DESCRIPTION
  The callback function that is called when LCP C-Req is found.

DEPENDENCIES
  This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type netmdli_init_callback_lcp_c_req
(
  ppp_dev_enum_type device,           /* the arrival device               */
  uint16           protocol,         /* protocol of packet               */
  dsm_item_type **item_head_ptr      /* item containing data             */
)
{
  uint16 len = 1;                   /* the length of a PPP ID is one byte  */
  uint16 offset = 1;                /* offset to use in seek_extract: ID=1 */
  dsm_item_type *item_ptr = *item_head_ptr; /* pointer to the item      */
  ppp_dev_enum_type out_device;
  byte pkt_id = 0;                 /* ID field of packet                  */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e527 -esym(613, item_head_ptr) */
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT((*item_head_ptr)->size > 0 && (*item_head_ptr)->used > 0);
  ASSERT(device < PPP_MAX_DEV);
  ASSERT(protocol == PPP_LCP_PROTOCOL);

  out_device = ppp_cb_array[device].bridge_dev;
  ASSERT(out_device < PPP_MAX_DEV);
  /*lint +e527 */

  /*-------------------------------------------------------------------------
    Take note of id for future C-Ack
  -------------------------------------------------------------------------*/
  if(dsm_seek_extract(&item_ptr,
                      &offset,
                      &pkt_id,
                      &len) == FALSE)
  {
    ERR( "seek_extract() failed - dumping pkt", 0, 0, 0);
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    Only store the ID in the packet IFF the packet hasn't been freed (i.e. a
    C-Rej was made.  If the packet has been freed, then point the head_ptr to
    the new packet that contains the C-Rej.
  -------------------------------------------------------------------------*/
  if(*item_head_ptr != NULL)
  {
    curr_ppp_id[out_device] = pkt_id;
    /*lint -e539 */
    MSG_MED("C-Req CB device %d, id %d, proto %x",
            device,
            curr_ppp_id[out_device],
            protocol);
    /*lint +e539 */
  }

  /*-----------------------------------------------------------------------
    Change phase to establish for device and out_device
  -----------------------------------------------------------------------*/
  if( ppp_cb_array[device].phase == pppDEAD )
  {
    ppp_cb_array[device].phase = pppESTABLISH;
  }

  if( ppp_cb_array[out_device].phase == pppDEAD )
  {
    ppp_cb_array[out_device].phase = pppESTABLISH;
  }

  /*-------------------------------------------------------------------------
    Assign protocol to packet kind field and forward packet.
  -------------------------------------------------------------------------*/
  if ((*item_head_ptr) != NULL)
  {
    (*item_head_ptr)->kind = DSM_PS_LCP_PROTOCOL;
#ifdef FEATURE_DSM_MEM_CHK
    (*item_head_ptr)->tracer = DSM_PPP_TX_WM;
#endif /* FEATURE_DSM_MEM_CHK */

    (void)pppi_outgoing(out_device, protocol, item_head_ptr, NULL);
  }

  return SNOOP_CB_IGNORE_PKT;

  /*lint +e613 */
} /* netmdli_init_callback_lcp_c_req() */



/*===========================================================================
FUNCTION   NETMDLI_INIT_CALLBACK_LCP_C_ACK()

DESCRIPTION
  The callback function that is called when an LCP C-Ack is found.

DEPENDENCIES
  This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type netmdli_init_callback_lcp_c_ack
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16            protocol,          /* protocol of packet               */
  dsm_item_type  **item_head_ptr       /* item containing data             */
)
{
  dsm_item_type *item_ptr = *item_head_ptr;         /* pointer to the item */
  ppp_type      *ppp_cb_ptr;         /* ptr to incoming PPP cb             */
  uint16 len = 1;                    /* the length of a PPP ID is one byte */
  uint16 offset = 1;  /* offset is one past the start of what we are given */
  byte id = 0;        /* to store the ID of the packet to use for comparison */
  ppp_dev_enum_type out_device;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e527 -esym(613, item_head_ptr) */
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT((*item_head_ptr)->size > 0 && (*item_head_ptr)->used > 0);
  ASSERT(device < PPP_MAX_DEV);
  ASSERT(protocol == PPP_LCP_PROTOCOL);

  ppp_cb_ptr = ppp_cb_array + device;
  out_device = ppp_cb_ptr->bridge_dev;

  ASSERT(out_device  < PPP_MAX_DEV);
  /*lint +e527 */

  /*-------------------------------------------------------------------------
    Make sure that we are in Psuedo Network mode, or Full Net mode.
    If we are not log error and dump packet
  -------------------------------------------------------------------------*/
  /*lint -e661 */
  if(ppp_cb_ptr->mode != PPP_PSEUDO_NET_MODE &&
     ppp_cb_ptr->mode != PPP_FULL_NET_MODE)
  {
    ERR("Saw LCP C-Ack when NOT in Pseudo Net/Full Net mode!Dumping packet",
        0, 0, 0);
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    Get the ID of the packet, then compare it to the current PPP id.  If it
    is the same then we must take note of LCP options done, otherwise ignore
    packet and send it on it's way.
  -------------------------------------------------------------------------*/
  if( dsm_seek_extract(&item_ptr, &offset, &id, &len) == FALSE)
  {
    ERR( "seek_extract() failed - dumping pkt", 0, 0, 0);
    dsm_free_packet( item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  MSG_MED("LCP C-Ack CB device %d, id: %d", device, id, 0);

  if(id == curr_ppp_id[device])
  {
    MSG_MED("Saving LCP options for device %d", device, 0, 0);

    /*-----------------------------------------------------------------------
      Save the options in the PPP control block on the Rm side
      NOTE: if we received this C-Ack on the Um it is the TE2 options that
            are accepted, so they are LOCAL for the Um device, the remote
            options are for the Rm interface.  (The same holds true if TE2 is
            replaced with IWF and Um and Rm are swapped).
    -----------------------------------------------------------------------*/
    if(device == PPP_RM_DEV)
    {
      if(netmdli_save_opts(item_head_ptr,
                           protocol,
                           &(ppp_cb_ptr->fsm[Lcp].local),
                           &(ppp_cb_array[out_device].fsm[Lcp].remote))
         == RET_ERROR)
      {
        return SNOOP_CB_IGNORE_PKT;
      }
    }

    else /* Um device */
    {
      if(netmdli_save_opts(item_head_ptr,
                           protocol,
                           &(ppp_cb_array[out_device].fsm[Lcp].remote),
                           &(ppp_cb_ptr->fsm[Lcp].local)) == RET_ERROR)
      {
        return SNOOP_CB_IGNORE_PKT;
      }
    }
  } /* if(id matches) */

  /*-------------------------------------------------------------------------
    Assign protocol to packet kind field and forward packet.
  -------------------------------------------------------------------------*/
  if ( (*item_head_ptr) != NULL)
  {
    (*item_head_ptr)->kind = DSM_PS_LCP_PROTOCOL;
#ifdef FEATURE_DSM_MEM_CHK
    (*item_head_ptr)->tracer = DSM_PPP_TX_WM;
#endif /* FEATURE_DSM_MEM_CHK */
    (void)pppi_outgoing(out_device, protocol, item_head_ptr, NULL);
  }

  return SNOOP_CB_IGNORE_PKT;

  /*lint +e613 +e661 */
} /* netmdli_init_callback_lcp_c_ack() */



/*===========================================================================
FUNCTION   NETMDLI_INIT_CALLBACK_IPCP_C_REQ()

DESCRIPTION
  The callback function that is called when an IPCP C-Req is found.

DEPENDENCIES
  This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type netmdli_init_callback_ipcp_c_req
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
)
{
  dsm_item_type *item_ptr = *item_head_ptr; /* pointer to the item         */
  uint16 len = 1;              /* the length of a PPP ID is one byte       */
  uint16 offset = 1;           /* the ID field is 1 past beginning of data */
  uint8  pkt_id = 0;
  ppp_dev_enum_type out_device;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e527 -esym(613, item_head_ptr) */
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT((*item_head_ptr)->size > 0 && (*item_head_ptr)->used > 0);
  ASSERT(device < PPP_MAX_DEV);
  ASSERT(protocol == PPP_IPCP_PROTOCOL);

  out_device = ppp_cb_array[device].bridge_dev;
  ASSERT(out_device < PPP_MAX_DEV);
  /*lint +e527 */

  /*-------------------------------------------------------------------------
    Take note of id for future C-Ack
  -------------------------------------------------------------------------*/
  if(dsm_seek_extract(&item_ptr,
                      &offset,
                      &(pkt_id),
                      &len) == FALSE)
  {
    ERR( "seek_extract() failed - dumping pkt", 0, 0, 0);
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }
  curr_ppp_id[device] = pkt_id;

  MSG_MED("Rx Init IPCP CReq ifac %d id %d", device, pkt_id, 0);

  /*-------------------------------------------------------------------------
    Change phase to AUTHENTICATE for device
  -------------------------------------------------------------------------*/
  if( ppp_cb_array[device].phase == pppESTABLISH )
  {
    ppp_cb_array[device].phase = pppAUTHENTICATE;
  }

  if( ppp_cb_array[out_device].phase == pppESTABLISH )
  {
    ppp_cb_array[out_device].phase = pppAUTHENTICATE;
  }

  /*-------------------------------------------------------------------------
    Store protocol in packet kind field and forward to the other interface 
    (other than the receiving interface).
  -------------------------------------------------------------------------*/
  if ((*item_head_ptr) != NULL)
  {
    (*item_head_ptr)->kind = DSM_PS_IPCP_PROTOCOL;
#ifdef FEATURE_DSM_MEM_CHK
    (*item_head_ptr)->tracer = DSM_PPP_TX_WM;
#endif /* FEATURE_DSM_MEM_CHK */
    (void)pppi_outgoing(out_device, protocol, item_head_ptr, NULL);
  }
  return SNOOP_CB_IGNORE_PKT;

  /*lint +e613 */
} /* netmdli_init_callback_ipcp_c_req() */



/*===========================================================================
FUNCTION   NETMDLI_INIT_CALLBACK_IPCP_C_ACK()

DESCRIPTION
  This is the callback function that is called for IPCP C-Acks.  It handles
  to storing of all of the IPCP parameters for the resync PPP stack.  It also
  will inform the Mobile Node Registration task that PPP has completed (if
  MNR support is compiled in) by invoking a callback function.  Currently the
  name of this function is hardwired, although this might change.

DEPENDENCIES
  This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type netmdli_init_callback_ipcp_c_ack
(
  ppp_dev_enum_type device,           /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
)
{
  dsm_item_type *item_ptr = *item_head_ptr; /* pointer to the item         */
  ppp_type      *ppp_cb_ptr; 
  uint16 len = 1;    /* the length of a PPP ID is one byte                 */
  uint16 offset = 1; /* offset is one past the start of what we are given  */
  byte pkt_id = 0;  /* to store the ID of the packet to use for comparison */
  ppp_dev_enum_type out_device;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e527 -esym(613, item_head_ptr) */
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT((*item_head_ptr)->size > 0 && (*item_head_ptr)->used > 0);
  ASSERT(device < PPP_MAX_DEV);
  ASSERT(protocol == PPP_IPCP_PROTOCOL);

  ppp_cb_ptr = ppp_cb_array + device;
  out_device = ppp_cb_ptr->bridge_dev;

  ASSERT(out_device < PPP_MAX_DEV);
  /*lint +e527 */


  /*-------------------------------------------------------------------------
    Get the ID of the packet, then compare it to the current PPP id.  If it
    is the same then negotiations might be done, otherwise ignore packet and
    send it on it's way.
  -------------------------------------------------------------------------*/
  if( dsm_seek_extract(&item_ptr, &offset, &pkt_id, &len) == FALSE)
  {
    ERR( "Seek extract failed - dumping pkt", 0, 0, 0);
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  MSG_LOW("IPCP C_Ack, device %d, id =%d", device, pkt_id, 0);

  if(pkt_id == curr_ppp_id[out_device])
  {
    MSG_MED("Saving IPCP options for device %d", device, 0, 0);

    /*-----------------------------------------------------------------------
      Save the options in the PPP control block
      NOTE: if we received this C-Ack on the Um it is the TE2 options that
            are accepted, so they are LOCAL for the Um device, the remote
            options are for the Rm interface.  (THE same holds true if TE2 is
            replaced with IWF and Um and Rm are swapped).
    -----------------------------------------------------------------------*/
    if(device == PPP_RM_DEV)
    {
      if(netmdli_save_opts(item_head_ptr,
                           protocol,
                           &(ppp_cb_ptr->fsm[IPCP].local),
                           &(ppp_cb_array[out_device].fsm[IPCP].remote))
         == RET_ERROR)
      {
        return SNOOP_CB_IGNORE_PKT;
      }
    }

    else /* is from Um interface */
    {
      if(netmdli_save_opts(item_head_ptr,
                           protocol,
                           &(ppp_cb_array[out_device].fsm[IPCP].remote),
                           &(ppp_cb_ptr->fsm[IPCP].local)) == RET_ERROR)
      {
        return SNOOP_CB_IGNORE_PKT;
      }
    }

    /*-----------------------------------------------------------------------
      Check the interface on which this arrived, to determine the ack status
    -----------------------------------------------------------------------*/
    if(device == PPP_RM_DEV)
    {
#ifdef MSG_HIGH
      if(ipcp_c_ack_status & Rm_ACK_RCVD)
      {
        MSG_HIGH("Received two Rm IPCP C-Acks!", 0,0,0);
      }
#endif
      ipcp_c_ack_status |= Rm_ACK_RCVD;
    }

    else /* this came in on Um */
    {
#ifdef MSG_HIGH
      if(ipcp_c_ack_status & Um_ACK_RCVD)
      {
        MSG_HIGH("Received two Um IPCP C-Acks!", 0,0,0);
      }
#endif
      ipcp_c_ack_status |= Um_ACK_RCVD;
    }

    /*-----------------------------------------------------------------------
      Have we seen both acks?
    -----------------------------------------------------------------------*/
    if(ipcp_c_ack_status == BOTH_ACKS_RCVD)
    {
      /*---------------------------------------------------------------------
        We have seen IPCP C-Acks on both interfaces, so we know PPP is
        established.  Change to the PPP upstate.
      ---------------------------------------------------------------------*/
      MSG_HIGH("PPP now established!", 0, 0, 0);

      netmdl_set_meta_state(PPP_UP_META_STATE);

#ifdef FEATURE_CALL_THROTTLE
      dctm_post_ppp_call_status( PPP_SUCCESS_EV, PPP_PROTOCOL_IPCP, 
                                 PPP_FAIL_REASON_NONE );
#endif /* FEATURE_CALL_THROTTLE */
    } /* if(both acks received) */
  } /* if(expected ID) */

  /*-------------------------------------------------------------------------
    Assign protocol to packet kind field and forward packet.
  -------------------------------------------------------------------------*/
  if((*item_head_ptr) != NULL)
  {
    (*item_head_ptr)->kind = DSM_PS_IPCP_PROTOCOL;
#ifdef FEATURE_DSM_MEM_CHK
    (*item_head_ptr)->tracer = DSM_PPP_TX_WM;
#endif /* FEATURE_DSM_MEM_CHK */
    (void)pppi_outgoing(out_device, protocol, item_head_ptr, NULL);
  }

  return SNOOP_CB_IGNORE_PKT;

  /*lint +e613 */
} /* netmdli_init_callback_ipcp_c_ack() */



/*===========================================================================
FUNCTION   NETMDLI_INIT_CALLBACK_PAP_A_REQ()

DESCRIPTION
  This is the callback function that is called when Auth-Req for PAP is
  found. The Incoming packet has 1 byte of Auth-Req, 1 byte of ID, 2 bytes of
  length, 1 byte of user-id length, user-id , 1 byte of password length and
  password.  The User-id and Password are copied from the incoming packet
  into the PAP data structure.

DEPENDENCIES
  This function expects to be called with a dsm_item whose data pointer is
  pointing at the code byte for the ppp packet.
  The PAP user-id and the Password of the Mobile are modified with that of 
  the TE2 device. 

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type netmdli_init_callback_pap_a_req
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
)
{
  dsm_item_type *item_ptr = *item_head_ptr;         /* pointer to the item */
  ppp_type      *ppp_cb_ptr;
  uint16 len;
  uint16 offset;
  ppp_dev_enum_type out_device;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*lint -e527 -esym(613, item_head_ptr) */
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT((*item_head_ptr)->size > 0 && (*item_head_ptr)->used > 0);
  ASSERT(device < PPP_MAX_DEV);
  ASSERT(protocol == PPP_PAP_PROTOCOL);
  /*lint +e527 */

  /*-------------------------------------------------------------------------
    Get the device that this user name and password will be forwarded over -
    that is where things will be stored.
  -------------------------------------------------------------------------*/
  out_device = ppp_cb_array[device].bridge_dev;
  ASSERT(out_device < PPP_MAX_DEV);
  ppp_cb_ptr = ppp_cb_array + out_device;

  /*-----------------------------------------------------------------------
    An auth request looks like:

      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | 1 (the value) |  Identifier   |            Length             |
      +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
      | User-ID Length|  User-Id  ...
      +-+-+-+-+-+-+-+-+-+-+-+-+-+
      | Passwd-Length |  Password ...
      +-+-+-+-+-+-+-+-+-+-+-+-+-+

    Populate the authentication info for the UM device with the info extracted
    from the PAP packet - the total length is of no interest as we will
    extract all of the other fields explicitly.

    Start by extracting the User-ID length - make sure we have enough buffer
    to copy the user ID.
  -------------------------------------------------------------------------*/
  len= PPP_PAP_UID_SIZE;
  offset = PPP_AUTH_CODE_SIZE +
           PPP_AUTH_ID_SIZE   +
           PPP_AUTH_LENGTH_SIZE;
  if(dsm_seek_extract(&item_ptr,
                      &offset,
                      &ppp_cb_ptr->auth.info.user_id_len,
                      &len) == FALSE)
  {
    MSG_HIGH("failed to get user ID length forwarding packet!", 0, 0, 0);
    goto finished;
  }

  if(ppp_cb_ptr->auth.info.user_id_len > PPP_MAX_USER_ID_LEN)
  {
    MSG_HIGH("User ID too long - forwarding packet!", 0, 0, 0);
    ppp_cb_ptr->auth.info.user_id_len = 0;
    goto finished;
  }

  /*-------------------------------------------------------------------------
    We can copy the entire user ID, so extract it.
  -------------------------------------------------------------------------*/
  len = ppp_cb_ptr->auth.info.user_id_len;
  offset += PPP_PAP_UID_SIZE;
  if(dsm_seek_extract(&item_ptr,
                      &offset,
                      &ppp_cb_ptr->auth.info.user_id_info,
                      &len) == FALSE)
  {
    MSG_HIGH("failed to get user ID forwarding packet!", 0, 0, 0);
    ppp_cb_ptr->auth.info.user_id_len = 0;
    goto finished;
  }

  /*-------------------------------------------------------------------------
    Now extract the password length.  Set the offset past the user ID info.
  -------------------------------------------------------------------------*/
  len = PPP_PAP_PWD_SIZE;
  offset += ppp_cb_ptr->auth.info.user_id_len;
  if(dsm_seek_extract(&item_ptr,
                      &offset,
                      &ppp_cb_ptr->auth.info.passwd_len,
                      &len) == FALSE)
  {
    MSG_HIGH("failed to get password length forwarding packet!", 0, 0, 0);
    ppp_cb_ptr->auth.info.user_id_len = 0;
    goto finished;
  }

  /*-------------------------------------------------------------------------
    Make sure we have enough buffer to copy whole thing - if the length is
    zero however, then skip to the end as the seek_extract() will fail as a
    zero read at the end of an item fails.
  -------------------------------------------------------------------------*/
  if(ppp_cb_ptr->auth.info.passwd_len > PPP_MAX_PASSWD_LEN)
  {
    MSG_HIGH("Password too long - forwarding packet!", 0, 0, 0);
    ppp_cb_ptr->auth.info.user_id_len = 0;
    ppp_cb_ptr->auth.info.passwd_len = 0;
    goto finished;
  }
  else if(ppp_cb_ptr->auth.info.passwd_len == 0)
  {
    goto finished;
  }

  /*-------------------------------------------------------------------------
    We can copy the entire password, so extract it.
  -------------------------------------------------------------------------*/
  len = ppp_cb_ptr->auth.info.passwd_len;
  offset += PPP_PAP_PWD_SIZE;
  if(dsm_seek_extract(&item_ptr,
                      &offset,
                      &ppp_cb_ptr->auth.info.passwd_info,
                      &len) == FALSE)
  {
    MSG_HIGH("failed to get passwd forwarding packet!", 0, 0, 0);
    ppp_cb_ptr->auth.info.user_id_len = 0;
    ppp_cb_ptr->auth.info.passwd_len = 0;
  }

 finished:
  (*item_head_ptr)->kind = DSM_PS_PAP_PROTOCOL;
#ifdef FEATURE_DSM_MEM_CHK
  (*item_head_ptr)->tracer = DSM_PPP_TX_WM;
#endif /* FEATURE_DSM_MEM_CHK */
  (void)pppi_outgoing(out_device, protocol, item_head_ptr, NULL);

  return SNOOP_CB_IGNORE_PKT;

} /* netmdli_init_callback_pap_a_req() */



#ifdef FEATURE_JCDMA_1X
/*===========================================================================
FUNCTION   NETMDLI_INIT_CALLBACK_CHAP_RESP()

DESCRIPTION
This is the callback function that is called when CHAP Response is found. The
Incoming packet has 1 byte of CHAP RESPONSE, 1 byte of ID, 2 bytes of total 
packet length, 1 byte of response length, Hashed response and user-id. The 
User-id will be extracted from the outgoing packet and will be appended to 
the authentication data structure. Password will be a constant value for the 
resync. This will be copied into the PAP data structure.

DEPENDENCIES
This function expects to be called with a dsm_item whose data pointer is 
pointing at the code byte for the ppp packet. The authentication user-id and 
the Password of the Mobile are modified. The user-id will be same as that of 
the TE2 device and Password will be a constant value.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type netmdli_init_callback_chap_resp
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16            protocol,          /* protocol of packet               */
  dsm_item_type   **item_head_ptr      /* item containing data             */
)
{
  /*-------------------------------------------------------------------------
    Contains total length of the CHAP (PPP) packet. This will be read from 
    the incoming CHAP packet. It is 16 bit wide.
  -------------------------------------------------------------------------*/
  uint16 len_of_chap_info;
  
  /*-------------------------------------------------------------------------
    Contains length of the CHAP response (PPP packet without Id, code, 
    user-id etc). This will be read from the incoming CHAP packet. It is 8 
    bits wide.
  -------------------------------------------------------------------------*/
  byte len_of_chap_resp;

  /*-------------------------------------------------------------------------
    len and offset are used to inform "Length of information to extract" 
    and "the offset from which to start" in the incoming PPP packet. Since 
    length of the PPP packet can be 16 bit wide, these are also chosen as 
    16 bit numbers.
  -------------------------------------------------------------------------*/
  uint16 len;
  uint16 offset;

  /*-------------------------------------------------------------------------
    pointer to the item containing the packet.
  -------------------------------------------------------------------------*/
  dsm_item_type *item_ptr = *item_head_ptr;         
  ppp_dev_enum_type out_device;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*lint -e527 -esym(613, item_head_ptr) */
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT((*item_head_ptr)->size > 0 && (*item_head_ptr)->used > 0);
  ASSERT(device < PPP_MAX_DEV);
  ASSERT(protocol == PPP_CHAP_PROTOCOL);

  out_device = ppp_cb_array[device].bridge_dev;
  ASSERT(out_device < PPP_MAX_DEV);
  /*lint +e527 */

  /*-------------------------------------------------------------------------
    Extract the total length from the incoming PPP CHAP packet. This length 
    will be used to extract to rest of the information from the packet. 
    Length will be the 3rd byte and 4th byte in the packet from the current 
    position of the pointer. So the offset will be 2 
    
    Number of bytes to read is 2 so the "len" will be 2.
    
    The total length of the CHAP information i.e. 
    len_of_chap_info =    1 byte for Code (i.e CHAP Response)
              + 1 byte for ID 
              + 2 bytes Total packet length 
              + 1 byte for Length of the Response 
              + Length of the CHAP Response in bytes
              + Length of the User-Id in bytes
    will be copied into len_of_chap_info.
  -------------------------------------------------------------------------*/
  len= 2;
  offset = 2;
  if(dsm_seek_extract(&item_ptr,
                       &offset,                 
                       &len_of_chap_info,        
                       &len) == FALSE)       
  {
    ERR( "Seek extract failed in CHAP call back - dumping pkt", 0, 0, 0);
    dsm_free_packet( item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    Extract the length of the CHAP Response field into len_of_chap_resp. 
    This will be 16 (0x10) for the MD5 but may change for other algorithms. 
    Offset should now be 2 past the previous offset, because we should 
    extract from Value Size field (which follows the PPP length field).
    Since Value Size field is only one byte make the len =1; 
  -------------------------------------------------------------------------*/
  offset += 2;        
  len = 1;                            
  if(dsm_seek_extract(&item_ptr,
                        &offset,
                        &len_of_chap_resp,
                        &len) == FALSE)
    {
      ERR( "Seek extract failed - dumping pkt", 0, 0, 0);
      dsm_free_packet( item_head_ptr);
      return SNOOP_CB_IGNORE_PKT;
    }

  /*-------------------------------------------------------------------------
    Populate the authentication info for the UM device with the info extracted
    from the CHAP packet
  -------------------------------------------------------------------------*/

  /*-------------------------------------------------------------------------
    Extract the user_id_len and User_id from the CHAP packet into the 
    authentication data structure. 
    
    Modify the endianness of len_of_chap_info to get exact length. Subtract
    ( 5 + len_chap_response) to remove the code (1 byte), ID (1 byte), PPP 
    packet length ( 2 bytes), Value size (1 byte) and response bytes 
    (len_of_chap_resp).
    
    Offset is now pointing to the value_size field. This should now point 
    to the user_id field. So add "contents of value_size" field + 1
    (1 for Value size field) to obtain the new offset. 
    
    Copy the length of the user_id into the auth_info.user_id_len. 
    Copy the user_id into the auth_info.user_id_info;
  -------------------------------------------------------------------------*/
  len = (ntohs(len_of_chap_info)) - (len_of_chap_resp + 5);
  offset += (len_of_chap_resp + 1);                                   

  ppp_cb_array[out_device].auth.info.user_id_len = len;

  if(dsm_seek_extract(&item_ptr,
                      &offset,
                      &(ppp_cb_array[out_device].auth.info.user_id_info),
                      &len) == FALSE)
    {
      ERR( "Seek extract failed - dumping pkt", 0, 0, 0);
      dsm_free_packet( item_head_ptr);
      return SNOOP_CB_IGNORE_PKT;
    }

  /*-------------------------------------------------------------------------
    Copy the Password and password length into the authentication data 
    structure.
    
    Note that during resync the MS does not use the same password as the 
    TE2. But it uses a constant password. This password is stored in 
    ppp_chap_resync_passwd_info. This should be copied into the password 
    fields of auth_info.
    
    Obtain the size in bytes for the ppp_chap_resync_passwd and store it 
    in auth_info.passwd_len.
    Copy the memcpy(void *dest, const void *src, uint16 count);
  -------------------------------------------------------------------------*/
  ppp_cb_array[out_device].auth.info.passwd_len = 
      CHAP_RESYNC_PASSWD_LENGTH;

  memcpy(ppp_cb_array[out_device].auth.info.passwd_info, 
         ppp_chap_resync_passwd,
         ppp_cb_array[out_device].auth.info.passwd_len);

  /*-------------------------------------------------------------------------
    The auth_info structure is now populated with the CHAP information 
    needed for the resync. 
    
    Transmit the original packet over the Um interface.
  -------------------------------------------------------------------------*/
  if ((*item_head_ptr) != NULL)
  {
    (*item_head_ptr)->kind = DSM_PS_CHAP_PROTOCOL;
#ifdef FEATURE_DSM_MEM_CHK
    (*item_head_ptr)->tracer = DSM_PPP_TX_WM;
#endif /* FEATURE_DSM_MEM_CHK */
    (void)pppi_outgoing(out_device, protocol, item_head_ptr, NULL);
  }

  /*-------------------------------------------------------------------------
    Since we donot have any thing else to process in this packet return FALSE
  -------------------------------------------------------------------------*/
  return SNOOP_CB_IGNORE_PKT;

} /* netmdli_init_callback_chap */

#endif /* FEATURE_JCDMA_1X */



#ifdef FEATURE_DATA_PS_INTERNAL_AUTH
/*===========================================================================
FUNCTION NETMDLI_UM_CHAP_CBACK()

DESCRIPTION
  This function is used to support doing CHAP on the handset during a network
  model call.  The packet is duplicated and will forward one copy to the
  laptop, and have the other processed by the CHAP code.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL snoop_cb_ret_val_enum_type netmdli_um_chap_cback
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
)
{
  ppp_type *      ppp_cb_ptr;
  dsm_item_type * dup_item;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  
  ASSERT(protocol == PPP_CHAP_PROTOCOL);
  ASSERT(device == PPP_UM_SN_DEV);

  if(item_head_ptr == NULL || *item_head_ptr == NULL)
  {
    ERR("um_chap_cback called with invalid item!", 0, 0, 0);
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    duplicate the packet so that we have on copy to forward and another to
    process locally.

    NOTE: as we are duping the whole packet, the offset should be zero, and
    the length can be set to the max possible length (which is 0xFFFF for a
    uint16).
  -------------------------------------------------------------------------*/
  dsm_dup_packet(&dup_item, *item_head_ptr, 0, 0xFFFF);

  /*-------------------------------------------------------------------------
    Send the original to the laptop and send the dup to the CHAP processing
    function.
  -------------------------------------------------------------------------*/
  (void) pppi_outgoing( ppp_cb_array[device].bridge_dev, 
                        protocol, 
                        item_head_ptr, 
                        NULL );

#ifdef FEATURE_DS_MULTIPLE_NAI
  /*-------------------------------------------------------------------------
    Set RM NAI in ppp_cb_array to laptop value before sending the 
    CHAP response. Multiple NAIs are applicable only for NV mode and so check 
    if phone is in NV mode before copying RM NAI.
  -------------------------------------------------------------------------*/
#ifdef FEATURE_UIM_SUPPORT_3GPD
  if(uim_3gpd_support() == NVRUIM_3GPD_MIP_RUIM_SIP_NV || 
     uim_3gpd_support() == NVRUIM_3GPD_MIP_NV_SIP_NV)
#endif /* FEATURE_UIM_SUPPORT_3GPD */
  {
    netmdli_copy_rm_nai(device);
  }
#endif /* FEATURE_DS_MULTIPLE_NAI */

  chap_proc(device, &dup_item);

  /*-------------------------------------------------------------------------
    Stop the CHAP timer.  We will not support CHAP response retransmission
    for netmodel CHAP processing (i.e. in TE2 calls).  This is done because
    the CHAP processing is coupled to the internal PPP stack which is not
    keeping track of the end-to-end PPP negotiation done in network model.

    The network server will be responsible for re-sending the 
    (or sending a new) challenge which is RFC 1323 compliant.
  --------------------------------------------------------------------------*/
  ppp_cb_ptr = ppp_cb_array + device;
  ps_timer_cancel(ppp_cb_ptr->fsm[LCP].timer);

  return SNOOP_CB_IGNORE_PKT;

} /* netmdli_um_chap_cback() */

#ifdef FEATURE_DS_MULTIPLE_NAI
/*===========================================================================
FUNCTION NETMDLI_COPY_RM_NAI()

DESCRIPTION
  This function is used to copt RM NAI into the user_id field, before
  sending CHAP response. 

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void netmdli_copy_rm_nai
(
  ppp_dev_enum_type device
)
{
  ppp_type *        ppp_cb_ptr;
  nv_stat_enum_type nv_status;
  char *            nai_temp;
  nv_item_type      nv_item;
  byte              nai_len=0;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/  

  ppp_cb_ptr = ppp_cb_array + device;

  nv_status = ps_get_nv_item( NV_DS_SIP_RM_NAI_I, &nv_item);
  if( nv_status != NV_DONE_S )
  {
    MSG_ERROR("Laptop NAI cannot be retrieved", 0, 0, 0);
    return;
  }

  nai_temp = (char*) nv_item.ds_sip_rm_nai.nai;

  /*-----------------------------------------------------------------------
    Only copy NAI if its length is valid.  If NAI length is > max allowed
    assume a memory/flash scribble and set length to 0.
  -----------------------------------------------------------------------*/
  nai_len = nv_item.ds_sip_rm_nai.nai_length;
  if( nai_len <= NV_MAX_NAI_LENGTH && nai_len > 0 )
  {
    ppp_cb_ptr->auth.info.user_id_len = nai_len;
    memcpy(ppp_cb_ptr->auth.info.user_id_info,
           nai_temp,
           nai_len);
  }
} /* netmdli_copy_rm_nai() */
#endif /* FEATURE_DS_MULTIPLE_NAI */

/*===========================================================================
FUNCTION NETMDLI_RM_CHAP_CBACK()

DESCRIPTION
  This function is used to swallow any chap reponses from the laptop.  
  It is expected to be used in conjunction with the challenge callback.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

DEPENDENCIES
  None

SIDE EFFECTS
  Any packet that is passed in it is freed.
===========================================================================*/
LOCAL snoop_cb_ret_val_enum_type netmdli_rm_chap_cback
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(protocol == PPP_CHAP_PROTOCOL);
  ASSERT(device == PPP_RM_DEV);

  MSG_LOW ("Ate CHAP response from TE2",0,0,0);

  if(item_head_ptr == NULL || *item_head_ptr == NULL)
  {
    MSG_ERROR("rm_chap_cback called with invalid item!", 0, 0, 0);
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    We are freeing the item here with the expectation that the Mobile is
    generating the response, so this one is meaningless.
  -------------------------------------------------------------------------*/
  dsm_free_packet(item_head_ptr);

  return SNOOP_CB_IGNORE_PKT;

} /* netmdli_rm_chap_cback() */

#ifdef FEATURE_DATA_PS_PAP_INTERNAL_AUTH
/*===========================================================================
FUNCTION NETMDLI_RM_PAP_CBACK()

DESCRIPTION
  This function is used to swallow any pap reponses from the laptop. 
  It is expected to be used in conjunction with the challenge callback.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it 
                       processes be re-examined.

  Packets need to be looked at again when new callbacks have been registered 
  which should be called with the packet in question.

DEPENDENCIES
  None

SIDE EFFECTS
  Any packet that is passed in it is freed.
===========================================================================*/
LOCAL snoop_cb_ret_val_enum_type netmdli_rm_pap_cback
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
)
{
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  ASSERT(protocol == PPP_PAP_PROTOCOL);
  ASSERT(device == PPP_RM_DEV);

  MSG_LOW ("Ate PAP response from TE2",0,0,0);

  if(item_head_ptr == NULL || *item_head_ptr == NULL)
  {
    MSG_ERROR("rm_pap_cback called with invalid item!", 0, 0, 0);
    return SNOOP_CB_IGNORE_PKT;
  }

  pap_internal_proc(PPP_UM_SN_DEV, item_head_ptr);

  return SNOOP_CB_IGNORE_PKT;
} /* netmdli_rm_pap_cback() */
#endif /* FEATURE_DATA_PS_PAP_INTERNAL_AUTH */

#endif /* FEATURE_DATA_PS_INTERNAL_AUTH */


/*===========================================================================
FUNCTION NETMDLI_CALLBACK_CCP_C_REQ()

DESCRIPTION
  This function is used to swallow any chap reponses from the laptop.  
  It is expected to be used in conjunction with the challenge callback.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

DEPENDENCIES
  None

SIDE EFFECTS
  Any packet that is passed in it is freed.
===========================================================================*/
LOCAL snoop_cb_ret_val_enum_type netmdli_callback_ccp_c_req
(
  ppp_dev_enum_type  device,           /* the arrival device               */
  uint16             protocol,         /* protocol of packet               */
  dsm_item_type **   item_head_ptr     /* item containing data             */
)
{
  static byte id = 0;
  PACKED struct
  {
    byte    code;
    byte    id;
    uint16  length;
    uint16  rejected_protocol;
  } p_rej_hdr;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT(protocol == PPP_CCP_PROTOCOL || protocol == PPP_ILCCP_PROTOCOL);
  ASSERT(device == PPP_RM_DEV || device == PPP_UM_SN_DEV);

  MSG_MED( "Got CCP (0x%x) msg %d from device %d",
           protocol,
           (byte) *((*item_head_ptr)->data_ptr),
           device );

  if(item_head_ptr == NULL || *item_head_ptr == NULL)
  {
    MSG_ERROR("callback_ccp_c_req called with invalid item!", 0, 0, 0);
    return SNOOP_CB_IGNORE_PKT;
  }

  MSG_LOW ("Ate CCP message",0,0,0);

  if(item_head_ptr == NULL || *item_head_ptr == NULL)
  {
    MSG_ERROR("rm_chap_cback called with invalid item!", 0, 0, 0);
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    Build protocol reject and pushdown at head of c-req (this is passed 
    back as 'Rejected-Information' as per RFC 1661.
  -------------------------------------------------------------------------*/
  p_rej_hdr.code   = PROT_REJ;
  p_rej_hdr.id     = id++;
  p_rej_hdr.length = dsm_length_packet(*item_head_ptr) + sizeof(p_rej_hdr);
  p_rej_hdr.length = htons( p_rej_hdr.length );
  p_rej_hdr.rejected_protocol = htons(protocol);

  if (sizeof(p_rej_hdr) != dsm_pushdown( item_head_ptr,
                                         (void *) &p_rej_hdr,
                                         sizeof(p_rej_hdr),
                                         DSM_DS_SMALL_ITEM_POOL ) )
  {
    MSG_ERROR ("No memory to send CCP prot-rej!  silently discard",0,0,0);
    dsm_free_packet(item_head_ptr);
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    Send the protocol reject out the same interface.  memory will be freed
    by pppi_outgoing, even if error return occurs.  We ignore the return
    value here as the action would be the same - to silently discard 
    (and there are enough error messages elsewhere).
  -------------------------------------------------------------------------*/
  (void) pppi_outgoing( device,
                        PPP_LCP_PROTOCOL,
                        item_head_ptr,
                        NULL );

  return SNOOP_CB_IGNORE_PKT;

} /* netmdli_callback_ccp_c_req() */



/*===========================================================================
FUNCTION   NETMDLI_PPP_UP_CALLBACK()

DESCRIPTION
  The generic call back for the PPP up state.  Any time that a PPP
  configuration packet is seen this callback is invoked.

DEPENDENCIES
  This function expects that the first four bytes of the PPP packet are in
  the first DSM item. For this information to be useful it is expected that
  the FF03 has been removed from the front and that any leading 00 s have
  been removed from the protocol ID.

RETURN VALUE
  SNOOP_CB_SNOOP_AGAIN: if packet arrived on Rm or if Um is UMTS
                        (it does need to be looked at again)  
  SNOOP_CB_PROCESS_PKT: if packet arrived on Um (it doesn't need to be 
                        looked at again) but it needs to go up the
                        stack for further processing
  
  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type netmdli_ppp_up_callback
(
  ppp_dev_enum_type device,           /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
)
{
#ifdef FEATURE_DATA_WCDMA_PS
#error code not present
#endif /* FEATURE_DATA_WCDMA_PS */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e527 -esym(613, item_head_ptr) */
  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);
  ASSERT(device < PPP_MAX_DEV);
  ASSERT(protocol == PPP_LCP_PROTOCOL || protocol == PPP_IPCP_PROTOCOL);
  /*lint +e527 */

  /*-------------------------------------------------------------------------
    For UMTS calls we always go to Init state.
    For other Um calls change the current state depending on the interface 
    on which this packet arrived, if on
    Um -> resync state
    Rm -> Init state
  -------------------------------------------------------------------------*/
#ifdef FEATURE_DATA_WCDMA_PS
#error code not present
#endif /* FEATURE_DATA_WCDMA_PS */

  if( device == PPP_RM_DEV )
  {
    /*-----------------------------------------------------------------------
      Change to the init state and return true eventually indicating that
      snoop_proto_msg_detect() needs to be called again
    -----------------------------------------------------------------------*/
    MSG_MED("0x%x rx but Rm PPP UP: going INITmeta", protocol, 0, 0);
    netmdl_set_meta_state(PPP_INIT_META_STATE);
    return SNOOP_CB_SNOOP_AGAIN;
  }

  else /* from Um */
  {
    /*-----------------------------------------------------------------------
      the packet was on the Um, change to the resync state
    -----------------------------------------------------------------------*/
    MSG_MED("0x%x rx but Um PPP UP: Resyncing", protocol, 0, 0);
    netmdl_set_meta_state(PPP_RESYNC_META_STATE);
    return SNOOP_CB_PROCESS_PKT;
  }

  /*lint +e613 */
} /* netmdli_ppp_up_callback() */


/*===========================================================================
FUNCTION   NETMDLI_PPP_DOWN_CALLBACK()

DESCRIPTION
  A callback function to handle the receiving TERM_REQ from the RM. If the 
  TERM_REQ packet seen from RM when the physical is not completely up 
  (dormant or coming out of dormancy), the type of cleanup performed is 
  based upon the policy set by the interface controller. If the policy is to 
  always perform the cleanup the TERM request is forwarded onto the UM 
  interface otherwise the call is aborted without cleaning up the network 
  resources.  This is to avoid the call origination if we receive TERM_REQ 
  from the RM interface.

DEPENDENCIES
  This function expects that the first four bytes of the PPP packet are in
  the first DSM item. For this information to be useful it is expected that
  the FF03 has been removed from the front and that any leading 00 s have
  been removed from the protocol ID.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT: this callback never requires that the packets it
                       processes be re-examined

  Packets need to be looked at again when new callbacks have been registered
  which should be called with the packet in question.

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type netmdli_ppp_down_callback
(
  ppp_dev_enum_type device,            /* the arrival device               */
  uint16 protocol,                     /* protocol of packet               */
  dsm_item_type **item_head_ptr        /* item containing data             */
)
{
  ps_iface_type *bridge_iface_ptr;         /* Get ptr to Um PS Iface       */
  ppp_dev_enum_type   bridge_dev;          /* figure out Um interface      */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( item_head_ptr != NULL && *item_head_ptr != NULL );
  ASSERT( device == PPP_RM_DEV );
  ASSERT( protocol == PPP_LCP_PROTOCOL || protocol == PPP_IPCP_PROTOCOL );

  bridge_dev = ppp_cb_array[device].bridge_dev;
  ASSERT(bridge_dev < PPP_MAX_DEV);
 
  MSG_MED("Received Term Req, prot: %d", protocol, 0, 0 );

  /*-------------------------------------------------------------------------
    Get the iface associated with the corresponding UM PPP device
  -------------------------------------------------------------------------*/
  bridge_iface_ptr = ppp_cb_array[bridge_dev].rx_iface_ptr;

  /*-------------------------------------------------------------------------
    Do not cleanup PPP on the network side if
    - bridged device PPP is already terminated OR
    - Bridged device is dormant and not required to be brought up per policy
  -------------------------------------------------------------------------*/
  if ( ( bridge_iface_ptr == NULL ) || 
       ( ps_iface_phys_link_state(bridge_iface_ptr) != PHYS_LINK_UP ) && 
       ( bridge_iface_ptr->graceful_dormant_close == FALSE) )
  {
    /*-----------------------------------------------------------------------
      Yes, this is the place where we do not want the call origination.
      Free the data buffer and terminate PPP on RM (rather than posting 
      event to rmsm) which will terminate the data call appropriately for 
      all targets.
    -----------------------------------------------------------------------*/
    MSG_MED("Terminating data session", 0, 0, 0 );
    dsm_free_packet( item_head_ptr );
    ppp_stop(PPP_RM_DEV);
  } 
  else 
  {
    MSG_MED("Protocol %d Bridge Dev %d", protocol, bridge_dev, 0 );

    /*-----------------------------------------------------------------------
      Still in a call (ppp bridge is still there) so stop Um PPP
    -----------------------------------------------------------------------*/
    switch( protocol )
    {
      case PPP_LCP_PROTOCOL:
      case PPP_IPCP_PROTOCOL:
        (void) ppp_stop( bridge_dev );
        break;

      default:
        break;
    }

    dsm_free_packet( item_head_ptr );
  }

  return SNOOP_CB_IGNORE_PKT;
} /* netmdli_ppp_down_callback */


/*===========================================================================
FUNCTION   NETMDLI_COMPARE_LCP_OPTS()

DESCRIPTION
  This function is used to compare LCP options for the various entities
  involved: Old/new IWF and the TE2/MT2.

  The only options that are being compated are: ACCM, ACFC, PFC.  MRU and
  Magic number do not affect the way data is transfered.
  
PARAMETERS
  None
  
RETURN VALUE
  NO_LCP_CHANGED:     None of the options is different
  MOBILE_LCP_CHANGED: The TE2/MT2 are different
  IWF_LCP_CHANGED:    The old/new IWF values are different

  Note: These are bit fields and so can be composed together to generate a
        return value.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
lcp_opt_change_enum_type netmdli_compare_lcp_opts
(
  void
)
{
  /*-------------------------------------------------------------------------
    Pointers to the lcp FSM structures
  -------------------------------------------------------------------------*/
  ppp_fsm_type *rm = &(ppp_cb_array[PPP_RM_DEV].fsm[Lcp]);
  ppp_fsm_type *um = &(ppp_cb_array[PPP_UM_SN_DEV].fsm[Lcp]);

  ppp_fsm_side_type *te2 = &(rm->remote);     /* pointer to te2 options    */
  ppp_fsm_side_type *mt2 = &(um->local);      /* pointer to mt2 options    */
  ppp_fsm_side_type *new_iwf = &(um->remote); /* pointer to new IWF options*/
  ppp_fsm_side_type *old_iwf = &(rm->local);  /* pointer to old IWF options*/

  lcp_opt_change_enum_type lcp_opts = NO_LCP_CHANGED;  /* return value     */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-------------------------------------------------------------------------
    Check ACFC is not disabled at the new IWF when old IWF had it enabled
  -------------------------------------------------------------------------*/
  if((old_iwf->work &  LCP_N_ACFC) && !(new_iwf->work & LCP_N_ACFC))
  {
    MSG_LOW("Old IWF ACFC %d, new IWF ACFC %d",
            (old_iwf->work & LCP_N_ACFC) == LCP_N_ACFC,
            (new_iwf->work & LCP_N_ACFC) == LCP_N_ACFC,
            0);
    MSG_MED("Old/New IWF ACFC different!", 0, 0, 0);
    lcp_opts |= IWF_LCP_CHANGED;
  }

  /*-------------------------------------------------------------------------
    Check PFC is not disabled at the new IWF when old IWF had it enabled
  -------------------------------------------------------------------------*/
  if((old_iwf->work &  LCP_N_PFC) && !(new_iwf->work & LCP_N_PFC))
  {
    MSG_LOW("Old IWF PFC %d, new IWF PFC %d",
            (old_iwf->work & LCP_N_PFC) == LCP_N_PFC,
            (new_iwf->work & LCP_N_PFC) == LCP_N_PFC,
            0);
    MSG_MED("Old/New IWF PFC different!", 0, 0, 0);
    lcp_opts |= IWF_LCP_CHANGED;
  }

  /*-------------------------------------------------------------------------
    Check ACFC is not disabled at the MT2 when TE2 had it enabled
  -------------------------------------------------------------------------*/
  if((mt2->work & LCP_N_ACFC) && !(te2->work & LCP_N_ACFC))
  {
    MSG_LOW("MT2 ACFC %d, TE2 ACFC %d",
            (mt2->work & LCP_N_ACFC) == LCP_N_ACFC,
            (te2->work & LCP_N_ACFC) == LCP_N_ACFC,
            0);
    MSG_MED("TE2/MT2 ACFC different!", 0, 0, 0);
    lcp_opts |= MOBILE_LCP_CHANGED;
  }

  /*-------------------------------------------------------------------------
    Check PFC is not disabled at the MT2 when TE2 had it enabled
  -------------------------------------------------------------------------*/
  if((mt2->work & LCP_N_PFC) && !(te2->work & LCP_N_PFC))
  {
    MSG_LOW("MT2 PFC %d, TE2 PFC %d",
            (mt2->work & LCP_N_PFC) == LCP_N_PFC,
            (te2->work & LCP_N_PFC) == LCP_N_PFC,
            0);
    MSG_MED("TE2/MT2 PFC different!", 0, 0, 0);
    lcp_opts |= MOBILE_LCP_CHANGED;
  }

  /*-------------------------------------------------------------------------
    Check ACCM negotiation - if this has changed there is a problem
  -------------------------------------------------------------------------*/
  if((mt2->work & LCP_N_ACCM) != (te2->work & LCP_N_ACCM))
  {
    MSG_LOW("MT2 ACCM neg %d, TE2 ACCM neg %d",
            (mt2->work & LCP_N_ACCM) == LCP_N_ACCM,
            (te2->work & LCP_N_ACCM) == LCP_N_ACCM,
            0);
    MSG_MED("TE2/MT2 ACCM neg different!", 0, 0, 0);
    lcp_opts |= MOBILE_LCP_CHANGED;
  }

  if((old_iwf->work & LCP_N_ACCM) != (new_iwf->work & LCP_N_ACCM))
  {
    MSG_LOW("Old IWF ACCM neg %d, New IWF ACCM neg %d",
            (old_iwf->work & LCP_N_ACCM) == LCP_N_ACCM,
            (new_iwf->work & LCP_N_ACCM) == LCP_N_ACCM,
            0);
    MSG_MED("Old/new IWF ACCM neg different!", 0, 0, 0);
    lcp_opts |= IWF_LCP_CHANGED;

  }

  /*-------------------------------------------------------------------------
    Check the ACCM values
  -------------------------------------------------------------------------*/
  if( ((lcp_value_type*)(old_iwf->work_pdv))->accm !=
      ((lcp_value_type*)(new_iwf->work_pdv))->accm )
  {
    MSG_LOW("Old IWF ACCM val 0x%x:%x",
            (uint16)((((lcp_value_type*)(old_iwf->work_pdv))->accm) >> 16),
            (uint16)((((lcp_value_type*)(old_iwf->work_pdv))->accm) &
                      0x0000FFFF),
            0);
    MSG_LOW("New IWF ACCM val 0x%x:%x",
            (uint16)((((lcp_value_type*)(new_iwf->work_pdv))->accm) >> 16),
            (uint16)((((lcp_value_type*)(new_iwf->work_pdv))->accm) &
                     0x0000FFFF),
            0);
    MSG_MED("Old/new IWF ACCM values different!", 0, 0, 0);
    lcp_opts |= IWF_LCP_CHANGED;
  }

  if( ((lcp_value_type*)(mt2->work_pdv))->accm !=
      ((lcp_value_type*)(te2->work_pdv))->accm )
  {
    MSG_LOW("MT2 ACCM val 0x%x:%x",
            (uint16)((((lcp_value_type*)(mt2->work_pdv))->accm) >> 16),
            (uint16)((((lcp_value_type*)(mt2->work_pdv))->accm) &0x0000FFFF),
            0);
    MSG_LOW("TE2 ACCM val 0x%x:%x",
            (uint16)((((lcp_value_type*)(te2->work_pdv))->accm) >> 16),
            (uint16)((((lcp_value_type*)(te2->work_pdv))->accm) &0x0000FFFF),
            0);
    MSG_MED("MT2/TE2 ACCM values different!", 0, 0, 0);
    lcp_opts |= MOBILE_LCP_CHANGED;
  }

  return lcp_opts;
} /* netmdli_compare_lcp_opts() */


/*===========================================================================
FUNCTION   NETMDL_COMPARE_LCP_OPTS()

DESCRIPTION
  This function is used to compare LCP options for the various entities
  involved (old/new IWF and the TE2/MT2), and sets the network model mode
  on the Um/Rm device to pseudo network if no LCP options on that particular
  interface have been changed.
  
  The only options that are being compated are: ACCM, ACFC, PFC.  MRU and
  Magic number do not affect the way data is transfered.
  
PARAMETERS
  None

RETURN VALUE
    TRUE - Rm and Um LCP options match
    FALSE - Rm and Um LCP options differ    

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean netmdl_compare_lcp_opts
(
  void
)
{
  lcp_opt_change_enum_type lcp_opts;            /* LCP option change value */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  lcp_opts = netmdli_compare_lcp_opts();

  if (lcp_opts == NO_LCP_CHANGED)
  {
    return TRUE;
  }

  return FALSE;
} /* netmdl_compare_lcp_opts() */


/*===========================================================================
FUNCTION NETMDL_COMPARE_IPCP_OPTS()

DESCRIPTION
  This function is used to compare IPCP options for the various entities
  involved: Old/new IWF and the TE2/MT2.

PARAMETERS
  None
  
RETURN VALUE
  TRUE - Rm and Um IPCP options match
  FALSE - Rm and Um IPCP options differ

DEPENDENCIES
  None
  
SIDE EFFECTS
  None
===========================================================================*/
boolean netmdl_compare_ipcp_opts
(
  void
)
{
  ppp_type *um_ppp_ptr = ppp_cb_array + PPP_UM_SN_DEV;
  ppp_type *rm_ppp_ptr = ppp_cb_array + PPP_RM_DEV;
  
  /*-------------------------------------------------------------------------
    Copies of the work values for the various entities (mt2, te2, etc...)
  -------------------------------------------------------------------------*/
  negotiate_t mt2_ipcp_work = um_ppp_ptr->fsm[IPCP].local.work;
  negotiate_t te2_ipcp_work = rm_ppp_ptr->fsm[IPCP].remote.work;
  negotiate_t mt2_ipcp_want = um_ppp_ptr->fsm[IPCP].local.want;
  negotiate_t te2_ipcp_want = rm_ppp_ptr->fsm[IPCP].remote.want;
  negotiate_t new_iwf_ipcp_work = um_ppp_ptr->fsm[IPCP].remote.work;
  negotiate_t orig_iwf_ipcp_work = rm_ppp_ptr->fsm[IPCP].local.work;

  /*-------------------------------------------------------------------------
    Pointers to the work_pdvs for the various entities (mt2, te2, etc...)
  -------------------------------------------------------------------------*/
  ipcp_value_type *mt2_ipcp_opts = 
    (ipcp_value_type*)um_ppp_ptr->fsm[IPCP].local.work_pdv;
  ipcp_value_type *te2_ipcp_opts =
    (ipcp_value_type*)rm_ppp_ptr->fsm[IPCP].remote.work_pdv;
  ipcp_value_type *new_iwf_ipcp_opts =
    (ipcp_value_type*)um_ppp_ptr->fsm[IPCP].remote.work_pdv;
  ipcp_value_type *orig_iwf_ipcp_opts =
    (ipcp_value_type*)rm_ppp_ptr->fsm[IPCP].local.work_pdv;

  boolean ret_val = TRUE;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  /*-----------------------------------------------------------------------
    Check IPCP options.  If any of the following questions can be answered
    true then an end-to-end resync must occur.

    Has the TE2's IP address changed? (make sure it was negotiatied too)
  -----------------------------------------------------------------------*/
  if(((mt2_ipcp_want & IPCP_N_1ADDRESS) ==
      (te2_ipcp_want & IPCP_N_1ADDRESS))     &&
     /*--------------------------------------------------------------------
       Both sides wanted to negotiate an IP address for the mobile - did
       they negotiate for it? did the mobiles address change? 
     --------------------------------------------------------------------*/
      ((mt2_ipcp_work & IPCP_N_1ADDRESS) &&
       (te2_ipcp_work & IPCP_N_1ADDRESS) &&
       (mt2_ipcp_opts->address != te2_ipcp_opts->address)))
  {
    MSG_MED("TE2 IP address changed!",0,0,0);
    MSG_MED("New TE2 address 0x%x:%x",
            (uint16)(mt2_ipcp_opts->address >> 16),
            (uint16)(mt2_ipcp_opts->address & 0x0000ffff),
            0);
    MSG_LOW("Old TE2 address 0x%x:%x",
            (uint16)(te2_ipcp_opts->address >> 16),
            (uint16)(te2_ipcp_opts->address & 0x0000ffff),
            0);
    ret_val = FALSE;
  }

  /*-----------------------------------------------------------------------
    Has TE2 compression been enabled when it previously wasn't?
  -----------------------------------------------------------------------*/
  else if((mt2_ipcp_work & IPCP_N_COMPRESS) &&
          !(te2_ipcp_work & IPCP_N_COMPRESS))
  {
    MSG_LOW("TE2 %d, MT2 %d",
            (te2_ipcp_work & IPCP_N_COMPRESS),
            (mt2_ipcp_work & IPCP_N_COMPRESS),
            0);
    MSG_MED("Mobile VJ comp on when was off!",0,0,0);
    ret_val = FALSE;
  }

  /*-----------------------------------------------------------------------
    Has IWF compression been disabled when it was previously enabled?
  -----------------------------------------------------------------------*/
  else if((orig_iwf_ipcp_work & IPCP_N_COMPRESS) &&
          !(new_iwf_ipcp_work & IPCP_N_COMPRESS))
  {
    MSG_LOW("Original IWF %d, new IWF %d",
            (orig_iwf_ipcp_work & IPCP_N_COMPRESS),
            (new_iwf_ipcp_work & IPCP_N_COMPRESS),
            0);
    MSG_MED("IWF VJ comp off when was on!",0,0,0);
    ret_val = FALSE;
  }

  /*-----------------------------------------------------------------------
    if IWF compression has always been on check IWF compression options
  -----------------------------------------------------------------------*/
  else if((orig_iwf_ipcp_work & IPCP_N_COMPRESS) &&
          (new_iwf_ipcp_work & IPCP_N_COMPRESS))
  {
    /*---------------------------------------------------------------------
      Are the old and new IWF's using the same compression protocol?
    ---------------------------------------------------------------------*/
    if(orig_iwf_ipcp_opts->compression != new_iwf_ipcp_opts->compression)
    {
      MSG_LOW("New IWF compr %x, Old IWF compr %d",
            new_iwf_ipcp_opts->compression,
            orig_iwf_ipcp_opts->compression,
            0);
      MSG_MED("Compression type changed!",0,0,0);
      ret_val = FALSE;
    }

    /*---------------------------------------------------------------------
      Does the new IWF have less recieve slots than the old IWF?
    ---------------------------------------------------------------------*/
    else if(new_iwf_ipcp_opts->slots < orig_iwf_ipcp_opts->slots)
    {
      MSG_LOW("New IWF slots %d, Old IWF slots %d",
              new_iwf_ipcp_opts->slots,
              orig_iwf_ipcp_opts->slots,
              0);
      MSG_MED("More VJ slots on old IWF than new IWF!",0,0,0);
      ret_val = FALSE;
    }

    /*--------------------------------------------------------------------
      Has IWF slot compression been disabled when it was previously on?
    ---------------------------------------------------------------------*/
    else if(new_iwf_ipcp_opts->slot_compress == 0 &&
            orig_iwf_ipcp_opts->slot_compress != 0)
    {
      MSG_LOW("New IWF slot comp %d, Old IWF slot comp %d",
              new_iwf_ipcp_opts->slot_compress,
              orig_iwf_ipcp_opts->slot_compress,
              0);
      MSG_MED("IWF slot comp NOW off when was on!",0,0,0);
      ret_val = FALSE;
    }
  } /* if(IWF compression has always been on) */

  /*-----------------------------------------------------------------------
    Has TE2 and MT2 compression been on and has slot compression has been
    enabled when it was previously disabled?
  -----------------------------------------------------------------------*/
  else if((te2_ipcp_work & IPCP_N_COMPRESS) &&
          (mt2_ipcp_work & IPCP_N_COMPRESS))
  {
    /*---------------------------------------------------------------------
      Are the MT2 and TE2 using the same compression protocol?
    ---------------------------------------------------------------------*/
    if(te2_ipcp_opts->compression != mt2_ipcp_opts->compression)
    {
      MSG_LOW("MT2 compr %x, TE2 compr %d",
            mt2_ipcp_opts->compression,
            te2_ipcp_opts->compression,
            0);
      MSG_MED("Compression type changed!",0,0,0);
      ret_val = FALSE;
    }

    /*---------------------------------------------------------------------
      Has number of MT2 slots increased?
    ---------------------------------------------------------------------*/
    else if(te2_ipcp_opts->slots < mt2_ipcp_opts->slots)
    {
      MSG_LOW("New TE2 slots %d, Old TE2 slots %d",
              mt2_ipcp_opts->slots,
              te2_ipcp_opts->slots,
              0);
      MSG_MED("Network transmitting more VJ slots than TE can handle!",
                 0,0,0);
      ret_val = FALSE;
    }

    /*--------------------------------------------------------------------
      Has MT2 slot compression been enabled when it previously wasn't?
    ---------------------------------------------------------------------*/
    else if(te2_ipcp_opts->slot_compress == 0 &&
            mt2_ipcp_opts->slot_compress != 0)
    {
      MSG_LOW("TE2 slot comp now ON (%d), was OFF (%d)!",
              mt2_ipcp_opts->slot_compress,
              te2_ipcp_opts->slot_compress,
              0);
      MSG_MED("TE2 slot comp now ON when was OFF!",0,0,0);
      ret_val = FALSE;
    }
  } /* if(MT2/TE2 compression has always been on) */

  return ret_val;
} /* netmdl_compare_ipcp_opts() */


/*===========================================================================
  FUNCTION NETMDL_SET_RESYNC_BEHAVIOUR

  DESCRIPTION
    Accessor function to force resync failure to drop call

  PARAMETERS
    NETMDL_RESYNC_DLFT      - do the default behaviour
    NETMDL_RESYNC_DROP_CALL - force call to drop on resync failure

  RETURN VALUE
    None

  DEPENDENCIES
    None

  SIDE EFFECTS
    None
===========================================================================*/
void netmdl_set_resync_behaviour
(
  netmdl_drop_behav_enum_type behav
)
{
  if (behav == NETMDL_RESYNC_DFLT )
  {
#ifdef FEATURE_DS_NET_MODEL_DROPS_CALL
    netmdl_drop_call = TRUE;
#else
    netmdl_drop_call = FALSE;
#endif /* FEATURE_DS_NET_MODEL_DROPS_CALL */
  } 
  else if (behav == NETMDL_RESYNC_DROP_CALL )
  {
    netmdl_drop_call = TRUE;
  }
  else
  {
    MSG_ERROR ("You called with bad param %d", behav,0,0);
  }
}


/*===========================================================================
FUNCTION   NETMDLI_PPP_RESYNCED()

DESCRIPTION
  This function is called once PPP has been established.  It checks the
  various IPCP and LCP options to determine whether or not an end-to-end
  resync will be required.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void netmdli_ppp_resynced
(
  ppp_dev_enum_type device
)
{
  dsm_item_type *creq_spoof_ptr;   /* dsm pointer for config request spoof */
  ppp_config_hdr_type header;                       /* header for spoofing */
  uint16 protocol = PPP_LCP_PROTOCOL;             /* protocol for spoofing */
  lcp_opt_change_enum_type lcp_opts;           /* LCP options change value */
  boolean take_action = FALSE;                    /* do we need to resync? */  
#ifdef FEATURE_DATA_IS707
  ps_iface_type *  um_iface_ptr;
#endif /* FEATURE_DATA_IS707 */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if(device != PPP_UM_SN_DEV)
  {
    ASSERT(0);
    return;
  }

  MSG_HIGH("PPP dev %d now resynced!", device, 0, 0);

  /*-------------------------------------------------------------------------
    deregister this function from being called after PPP negotiations are
    complete
  -------------------------------------------------------------------------*/
  ipcp_set_net_model_resync_fnc( NULL, device );

  /*-------------------------------------------------------------------------
    Check if the LCP options are the same for mobile and IWF
  -------------------------------------------------------------------------*/
  lcp_opts = netmdli_compare_lcp_opts();
  
  /*-------------------------------------------------------------------------
    If LCP options are unchanged on the mobile or IWF, set that interface
    to pseudo network model mode.
  -------------------------------------------------------------------------*/
  if(lcp_opts & MOBILE_LCP_CHANGED)
  {
    MSG_MED("TE2 options changed Um in FN mode", 0, 0, 0);
    take_action = TRUE;
  }
  else
  {
    MSG_MED("TE2 options unchanged Um in PN mode", 0, 0, 0);
    ppp_set_mode(PPP_UM_SN_DEV, PPP_PSEUDO_NET_MODE);
  }
  if(lcp_opts & IWF_LCP_CHANGED)
  {
    MSG_MED("IWF options changed Um in FN mode", 0, 0, 0);
    take_action = TRUE;
  }
  else
  {
    MSG_MED("IWF options unchanged Rm in PN mode", 0, 0, 0);
    /*------------------------------------------------------------------------
      When the Um (Uu) interface has HDLC disabled, then we run the R
      interface in FULL NETWORK model.
    ------------------------------------------------------------------------*/
    if(ppp_cb_array[PPP_UM_SN_DEV].hdlc_mode == HDLC_OFF)
    {
      ppp_set_mode(PPP_RM_DEV, PPP_FULL_NET_MODE);
    }
    else
    {
      ppp_set_mode(PPP_RM_DEV, PPP_PSEUDO_NET_MODE);
    }
  }

  /*--------------------------------------------------------------------------
    Check if the IPCP options are the same for mobile and IWF
  --------------------------------------------------------------------------*/
  if (netmdl_compare_ipcp_opts() == FALSE)
  {
    take_action = TRUE;
  }

  /*-------------------------------------------------------------------------
    Regardless of whether options match or not, enable the RM flow
  -------------------------------------------------------------------------*/
  ds3g_siolib_set_inbound_flow(DS_FLOW_NETMODEL_MASK, DS_FLOW_ENABLE);

  if(take_action == TRUE)
  {
    if (netmdl_drop_call)
    {
      /*----------------------------------------------------------------------
        Drop terminate the call
      ----------------------------------------------------------------------*/
    MSG_HIGH("PPP Options changed stopping PPP!", 0, 0, 0);
#ifdef FEATURE_DATA_IS707
      um_iface_ptr = ppp_cb_array[PPP_UM_SN_DEV].rx_iface_ptr;
      if (um_iface_ptr->name == CDMA_SN_IFACE)
      {
        /*--------------------------------------------------------------------
          In MIP calls, tell RMSM to bring down Um interface
          In Simple IP calls , this tears down TC without sending TERM-REQ
        --------------------------------------------------------------------*/
        MSG_HIGH("Drop call since Um resync failed",0,0,0);
        ds707_rmsm_post_event (DS707_RMSM_UM_PPP_DOWN_EV);
      }
      else
#endif /* FEATURE_DATA_IS707 */
      {
        MSG_ERROR("drop_call true when !CDMA_SN_IFACE",0,0,0);
      }
    }
    else
    {
      /*----------------------------------------------------------------------
        force an end-to-end resync do so by spoofing an empty PPP packet.
      -----------------------------------------------------------------------*/
      MSG_HIGH("PPP Options changed, resyncing.", 0, 0, 0);

      creq_spoof_ptr = dsm_offset_new_buffer( DSM_DS_LARGE_ITEM_POOL, 
         				      PPP_HDR_LEN );
      if(creq_spoof_ptr != NULL)
      {
        ppp_set_mode(PPP_RM_DEV, PPP_PSEUDO_NET_MODE);
        ppp_set_mode(PPP_UM_SN_DEV, PPP_PSEUDO_NET_MODE);
        header.code = CONFIG_REQ;
        header.identifier = curr_ppp_id[PPP_RM_DEV] - 1;
        /*lint -e572
         * for some reason the ntohs() macro is making lint complain about an
         * excessive shift?  Tried all sorts of things to fix it, nothing
         * worked, so turning off that error.
         */
        header.length = ntohs(CONFIG_HDR_LEN);
        /*lint +e572 */
        memcpy( creq_spoof_ptr->data_ptr, &header, sizeof(header));
        creq_spoof_ptr->used += CONFIG_HDR_LEN;
  
        /*-------------------------------------------------------------------
          Put meta state machine into initialization state - we are 
          resyncing and shove the packet out to the TE2.
        -------------------------------------------------------------------*/
        netmdl_set_meta_state(PPP_INIT_META_STATE);
  #ifdef FEATURE_DSM_MEM_CHK
        creq_spoof_ptr->tracer = DSM_PPP_TX_WM;
  #endif /* FEATURE_DSM_MEM_CHK */
        (void)pppi_outgoing(PPP_RM_DEV, protocol, &creq_spoof_ptr, NULL);
        
        return;

      } /* if(mem alloc worked) */
      else
      {
        MSG_HIGH("C-Req item allocation failed, not spoofing",0, 0, 0);
      }
    }
  } /* if(had to resync) */

  /*-------------------------------------------------------------------------
    Everything is in the right mode at this point so change the state to
    PPP Up state and deregister this function for callback.
  -------------------------------------------------------------------------*/
  netmdl_set_meta_state(PPP_UP_META_STATE);

} /* netmdli_ppp_resynced() */



/*===========================================================================
FUNCTION NETMDLI_SNOOP_REG()

DESCRIPTION
  Wrapper to register snoop callbacks and remember what was registered so
  we can unregister only those that were registered by netmdl later

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void netmdli_snoop_reg
(
  ppp_dev_enum_type      iface,       /* Interface to snoop on             */
  uint16                 protocol,    /* Procotol to snoop                 */
  byte                   msg_type,    /* Protocol spec. msg to snoop       */
  snoop_cb_f_ptr_type    callback     /* Function to call when msg arrives */
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  ASSERT( snoop_id_array_entries < MAX_SNOOPED_MESSAGES );

  /*-------------------------------------------------------------------------
    Snoop the message and add to netmdl's list of snoop'ed messages
  -------------------------------------------------------------------------*/
  snoop_id_array[snoop_id_array_entries++] = 
    snoop_reg_event( iface, protocol, msg_type, callback );

}  /* netmdli_snoop_reg() */



/*===========================================================================
FUNCTION NETMDLI_SNOOP_UNREG_ALL()

DESCRIPTION
  Unregister all callbacks previously snooped by netdml.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
LOCAL void netmdli_snoop_unreg_all
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Deregister all previously snooped messages
  -------------------------------------------------------------------------*/
  while( snoop_id_array_entries ) 
  {
    snoop_unreg_event( snoop_id_array[--snoop_id_array_entries] );
  }

  ASSERT( snoop_id_array_entries == 0 );

}  /* netmdli_snoop_unreg_all() */



/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-

                    EXTERNAL FUNCTION DEFINTIONS

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION   NETMDL_SET_META_STATE()

DESCRIPTION
  This function sets the state of the PPP Meta state machine.  This is mostly
  registereing the callbacks that are associated with that state.  The
  notable exception to this is the Resync State, which doesn't have any
  callback associated with it, but does have a fairly complex algorithm.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void netmdl_set_meta_state
(
  netmdl_meta_state_enum_type new_ppp_meta_state /* the new meta state     */
)
{
  ppp_type *um_ppp_ptr = ppp_cb_array + PPP_UM_SN_DEV;
  ppp_type *rm_ppp_ptr = ppp_cb_array + PPP_RM_DEV;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Set the state variable for this interface
  -------------------------------------------------------------------------*/
  MSG_MED("PPP Meta state from %d to %d",
          netmdli_state,
          new_ppp_meta_state, 0);
  netmdli_state =  new_ppp_meta_state;

  /*-------------------------------------------------------------------------
    Since all of the states require that the callbacks that are currently
    registered be deregistered, do that up front.
  -------------------------------------------------------------------------*/
  netmdli_snoop_unreg_all();

  switch(new_ppp_meta_state)
  {
    /*-----------------------------------------------------------------------
      Change either to the OOC state or the Relay state.  As both of them
      check whether the netmodel is enabled and take action based on that the
      code is pretty much the same.  If netmode is enabled then regardless of
      which of these two states we are changing to we will end up in the OOC
      state, otherwise if it is disabled we will end up in the Relay state.
    -----------------------------------------------------------------------*/
  case PPP_RELAY_META_STATE:
    ASSERT(0);
    /* Fall through */

  case PPP_OOC_META_STATE:
    /*-----------------------------------------------------------------------
      If PPP resync failed, flow on RM would have never got enabled since 
      netmodel never get notified.  Hence enable flow during cleanup.
    -----------------------------------------------------------------------*/
    ds3g_siolib_set_inbound_flow(DS_FLOW_NETMODEL_MASK, DS_FLOW_ENABLE);

    /*-----------------------------------------------------------------------
      clear PPP complete callback function in case PPP resync fails and the
      callback is never cleared in the callback itself.
    -----------------------------------------------------------------------*/
    ipcp_set_net_model_resync_fnc( NULL, PPP_UM_SN_DEV );
    return;

    /*-----------------------------------------------------------------------
      Change to the Init state: register all of Init state callbacks.
    -----------------------------------------------------------------------*/
  case PPP_INIT_META_STATE:
    /*-----------------------------------------------------------------------
      LCP C-Req on Um and Rm
    -----------------------------------------------------------------------*/
    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_LCP_PROTOCOL,
                          CONFIG_REQ,
                          netmdli_init_callback_lcp_c_req);
    netmdli_snoop_reg(PPP_UM_SN_DEV,
                          PPP_LCP_PROTOCOL,
                          CONFIG_REQ,
                          netmdli_init_callback_lcp_c_req);
    /*-----------------------------------------------------------------------
      IPCP C-Req on Um and Rm
    -----------------------------------------------------------------------*/
    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_IPCP_PROTOCOL,
                          CONFIG_REQ,
                          netmdli_init_callback_ipcp_c_req);
    netmdli_snoop_reg(PPP_UM_SN_DEV,
                          PPP_IPCP_PROTOCOL,
                          CONFIG_REQ,
                          netmdli_init_callback_ipcp_c_req);
    /*-----------------------------------------------------------------------
      LCP C-Ack on Um and Rm
    -----------------------------------------------------------------------*/
    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_LCP_PROTOCOL,
                          CONFIG_ACK,
                          netmdli_init_callback_lcp_c_ack);
    netmdli_snoop_reg(PPP_UM_SN_DEV,
                          PPP_LCP_PROTOCOL,
                          CONFIG_ACK,
                          netmdli_init_callback_lcp_c_ack);
    /*-----------------------------------------------------------------------
      IPCP C-Ack on Um and Rm
    -----------------------------------------------------------------------*/
    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_IPCP_PROTOCOL,
                          CONFIG_ACK,
                          netmdli_init_callback_ipcp_c_ack);
    netmdli_snoop_reg(PPP_UM_SN_DEV,
                          PPP_IPCP_PROTOCOL,
                          CONFIG_ACK,
                          netmdli_init_callback_ipcp_c_ack);

    /*-----------------------------------------------------------------------
      PAP config request on Rm only
    -----------------------------------------------------------------------*/
    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_PAP_PROTOCOL,
                          CONFIG_REQ,
                          netmdli_init_callback_pap_a_req);
#ifdef FEATURE_JCDMA_1X
    /*-----------------------------------------------------------------------
      CHAP config request on Rm only
    -----------------------------------------------------------------------*/
    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_CHAP_PROTOCOL,
                          PPP_CHAP_RESPONSE,
                          netmdli_init_callback_chap_resp);
#endif /* FEATURE_JCDMA_1X */

#ifdef FEATURE_DATA_PS_INTERNAL_AUTH
#ifdef FEATURE_UIM_SUPPORT_3GPD
  if(UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
     UIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
#endif /* FEATURE_UIM_SUPPORT_3GPD */
  {
    /*-----------------------------------------------------------------------
      in the case of DMU the challenge and response callbacks should be used
      as they will guarantee that chap runs on the handset.
    -----------------------------------------------------------------------*/
    netmdli_snoop_reg(PPP_UM_SN_DEV,
                          PPP_CHAP_PROTOCOL,
                          PPP_CHAP_CHALLENGE,
                          netmdli_um_chap_cback);
    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_CHAP_PROTOCOL,
                          PPP_CHAP_RESPONSE,
                          netmdli_rm_chap_cback);
#ifdef FEATURE_DATA_PS_PAP_INTERNAL_AUTH
    netmdli_snoop_reg(PPP_RM_DEV, 
                          PPP_PAP_PROTOCOL, 
                          PPP_PAP_REQ, 
                          netmdli_rm_pap_cback);
#endif /* FEATURE_DATA_PS_PAP_INTERNAL_AUTH */
  }
#endif /* FEATURE_DATA_PS_INTERNAL_AUTH */

#ifdef FEATURE_DATA_PS_CCP_REJECT 
    /*-----------------------------------------------------------------------
      CCP (single and multilink/individual link CCP) config request on all
      interfaces
    -----------------------------------------------------------------------*/
    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_CCP_PROTOCOL,
                          SNOOP_ANY_MSG,
                          netmdli_callback_ccp_c_req);
    netmdli_snoop_reg(PPP_UM_SN_DEV,
                          PPP_CCP_PROTOCOL,
                          SNOOP_ANY_MSG,
                          netmdli_callback_ccp_c_req);
    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_ILCCP_PROTOCOL,
                          SNOOP_ANY_MSG,
                          netmdli_callback_ccp_c_req);
    netmdli_snoop_reg(PPP_UM_SN_DEV,
                          PPP_ILCCP_PROTOCOL,
                          SNOOP_ANY_MSG,
                          netmdli_callback_ccp_c_req);
#endif /* FEATURE_DATA_PS_CCP_REJECT */

    /*-----------------------------------------------------------------------
      Reset variables - ipcp_c_ack_status
    -----------------------------------------------------------------------*/
    ipcp_c_ack_status = NO_ACK_RCVD;
    return;

    /*-----------------------------------------------------------------------
      Change to the PPP Up state.  Register all relevant callbacks.
    -----------------------------------------------------------------------*/
  case PPP_UP_META_STATE:
    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_LCP_PROTOCOL,
                          CONFIG_REQ,
                          netmdli_ppp_up_callback);
    netmdli_snoop_reg(PPP_UM_SN_DEV,
                          PPP_LCP_PROTOCOL,
                          CONFIG_REQ,
                          netmdli_ppp_up_callback);

    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_LCP_PROTOCOL,
                          TERM_REQ,
                          netmdli_ppp_down_callback);

    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_IPCP_PROTOCOL,
                          TERM_REQ,
                          netmdli_ppp_down_callback);

    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_IPCP_PROTOCOL,
                          CONFIG_REQ,
                          netmdli_ppp_up_callback);
    netmdli_snoop_reg(PPP_UM_SN_DEV,
                          PPP_IPCP_PROTOCOL,
                          CONFIG_REQ,
                          netmdli_ppp_up_callback);

#ifdef FEATURE_DATA_PS_INTERNAL_AUTH
#ifdef FEATURE_UIM_SUPPORT_3GPD
  if(UIM_3GPD_MIP_RUIM_SIP_RUIM == uim_3gpd_support() ||
     UIM_3GPD_MIP_NV_SIP_RUIM == uim_3gpd_support())
#endif /* FEATURE_UIM_SUPPORT_3GPD */
  {
    /*-----------------------------------------------------------------------
      in the case of DMU the challenge and response callbacks should be used
      as they will guarantee that chap runs on the handset.
    -----------------------------------------------------------------------*/
    netmdli_snoop_reg(PPP_UM_SN_DEV,
                          PPP_CHAP_PROTOCOL,
                          PPP_CHAP_CHALLENGE,
                          netmdli_um_chap_cback);
    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_CHAP_PROTOCOL,
                          PPP_CHAP_RESPONSE,
                          netmdli_rm_chap_cback);
#ifdef FEATURE_DATA_PS_PAP_INTERNAL_AUTH
    netmdli_snoop_reg(PPP_RM_DEV, 
                          PPP_PAP_PROTOCOL, 
                          PPP_PAP_REQ, 
                          netmdli_rm_pap_cback);
#endif /* FEATURE_DATA_PS_PAP_INTERNAL_AUTH */
  }
#endif /* FEATURE_DATA_PS_INTERNAL_AUTH */

#ifdef FEATURE_DATA_PS_CCP_REJECT
    /*-----------------------------------------------------------------------
      CCP (single and multilink/individual link CCP) config request on all
      interfaces
    -----------------------------------------------------------------------*/
    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_CCP_PROTOCOL,
                          SNOOP_ANY_MSG,
                          netmdli_callback_ccp_c_req);
    netmdli_snoop_reg(PPP_UM_SN_DEV,
                          PPP_CCP_PROTOCOL,
                          SNOOP_ANY_MSG,
                          netmdli_callback_ccp_c_req);
    netmdli_snoop_reg(PPP_RM_DEV,
                          PPP_ILCCP_PROTOCOL,
                          SNOOP_ANY_MSG,
                          netmdli_callback_ccp_c_req);
    netmdli_snoop_reg(PPP_UM_SN_DEV,
                          PPP_ILCCP_PROTOCOL,
                          SNOOP_ANY_MSG,
                          netmdli_callback_ccp_c_req);
#endif /* FEATURE_DATA_PS_CCP_REJECT */

    /*-----------------------------------------------------------------------
      Set up the state machines to look like they are open so we can
      close gracefully if need be.  
    -----------------------------------------------------------------------*/
    um_ppp_ptr->phase = pppNETWORK;
    um_ppp_ptr->fsm[LCP].state  = fsmOPENED;
    um_ppp_ptr->fsm[IPCP].state = fsmOPENED;
    rm_ppp_ptr->phase = pppNETWORK;
    rm_ppp_ptr->fsm[LCP].state  = fsmOPENED;
    rm_ppp_ptr->fsm[IPCP].state = fsmOPENED;
    return;

    /*-----------------------------------------------------------------------
      Change to the Resync state.
    -----------------------------------------------------------------------*/
  case PPP_RESYNC_META_STATE:
    ppp_set_mode(PPP_UM_SN_DEV, PPP_INTERNAL_MODE);

    /*-----------------------------------------------------------------------
      enable Rm flow control
    -----------------------------------------------------------------------*/
    ds3g_siolib_set_inbound_flow(DS_FLOW_NETMODEL_MASK, DS_FLOW_DISABLE);

    /*-----------------------------------------------------------------------
      register callback function to be called when IPCP competes (i.e. PPP
      is done.
    -----------------------------------------------------------------------*/
    ipcp_set_net_model_resync_fnc( netmdli_ppp_resynced, PPP_UM_SN_DEV );

    /*-----------------------------------------------------------------------
      Initialize PPP and start resyncing
    -----------------------------------------------------------------------*/
    MSG_HIGH("Resyncing PPP here", 0, 0, 0);

#ifdef FEATURE_DS_MOBILE_IP_PERF
    /*-----------------------------------------------------------------------
      Notify M.IP call performance that Um PPP resync is starting.
    -----------------------------------------------------------------------*/
    mip_perf_delay(&mip_perf_info[PERF_UM_PPP_DELAY],
                   PERF_UM_PPP_DELAY,
                   PERF_START_TS);
#endif /* FEATURE_DS_MOBILE_IP_PERF */

    fsm_init(&(um_ppp_ptr->fsm[LCP]));
    fsm_init(&(um_ppp_ptr->fsm[IPCP]));
    fsm_open(&(um_ppp_ptr->fsm[LCP]));
    fsm_open(&(um_ppp_ptr->fsm[IPCP]));

    fsm_up(&(um_ppp_ptr->fsm[Lcp]));
    break;

  default:
    /*-----------------------------------------------------------------------
      We should NEVER! be in this state!
    -----------------------------------------------------------------------*/
      /*lint -e527 -e506 -e774*/
    ASSERT(FALSE);
    /*lint +e527 +e506 +e774 */

  } /* switch(new state) */

} /* netmdl_set_meta_state() */



/*===========================================================================
FUNCTION NETMDL_GET_META_STATE()

DESCRIPTION
  This function returns the current state of the network meta state machine.

PARAMETERS
  None

RETURN VALUE
  The current network model meta state machine state.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
netmdl_meta_state_enum_type netmdl_get_meta_state
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return netmdli_state;

} /* netmdl_get_meta_state() */

#endif /* FEATURE_DATA */
