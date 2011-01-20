/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                         P S _ P P P _ S N O O P . C

GENERAL DESCRIPTION

  This contains the code for Data Services Snoop layer.

EXTERNALIZED FUNCTIONS

  These functions are defined in ds_snoop.h and are externally visible.

  snoop_init()
    Initializes snoop data structure.

  snoop_reg_event()
    Used to register a callback to be triggered by a particular protocol
    and message type.

  snoop_unreg_event()
    Used to deregister a specific function that was previously registered.

  snoop_unreg_all()
    Used to unregister ALL currently registered callbacks (is synonymous to
    snoop_init()).

  snoop_early_proto_detect()
    Used to perform the early protocol detection of the PPP packets.  This is
    the first phase of the snooping that is done.

  snoop_proto_msg_detect()
    Used to detect protocol and message combinations.  Also calls any
    registered callbacks for the protocol & message detected.

  Copyright (c) 1998-2008 QUALCOMM Incorporated. 
  All Rights Reserved.
  Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ps_ppp_snoop.c_v   1.6   13 Feb 2003 14:20:20   ubabbar  $
  $Header: //source/qcom/qct/modem/data/cdps/ps/main/lite/src/ps_ppp_snoop.c#4 $
  $Author: praveenp $ $DateTime: 2008/06/27 15:37:54 $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
06/28/08    pp     Fixed RVCT compiler warnings.
02/22/08    pp     Klocwork fixes.
02/19/08    sn     Merged lint error fix.
10/18/07    sn     Removed the 'cookie' parameter validation as 'dsm_item_type' 
                   structure no longer supports 'cookie' parameter.
11/01/06    an     Featurize ATCOP, RMSM, Async Fax
06/03/04    jd     Added CCP snoop support under FEATURE_DATA_PS_CCP_REJECT
04/07/03    usb    Fixed snoop_proto_msg_detect() to handle SNOOP_INTERNAL
                   PPP mode.
03/26/03    mvl    removed authentication featurization
02/11/03    usb    Removed FEATURE_DS_SNOOP_IP, mainlined IP pkt snooping
02/02/03    usb    Added support for snooping IP protocol packets, changed
                   return value of snoop_proto_msg_detect() from boolean
                   to snoop_cb_ret_val_enum_type.
09/12/02    mvl    Removed netmodel featurization.
08/14/02    jd     Added PPP cfg pkts to snoop_is_registration_pkt so they 
                   don't get passed to Rm during MIP reregistration
07/25/02    mvl    Updates for PPP renaming.
05/22/02    mvl    changed the interface name for MM PS.
05/08/02    mvl    File cleanup and added arg to ppp_outgoing() for new proto
12/18/01    rsl    Updated tracer field.
11/12/01    dwp    Fixed typo in a MSG macro (#d -> %d).
11/07/01    vr     added FEATURE_DS_MOBILE_IP wrapper
11/05/01    vr     added function snoop_is_registration_pkt for increasing
                   laptop throughput during re-registration
08/31/01    mvl    removed reference to FEATURE_DS_PPP_C_REJ_OPTS as it
                   isn't anywhere else in the code.
05/23/01    pjb    Moved protocol types out of DSM to PS
05/18/01    mvl    Changed OUT_IFACE() macro call to SWAP_IFACE()
08/25/00    rc     Changed FEATURE_DS_MEM_CHK to FEATURE_DSM_MEM_CHK
07/31/00    ak     Added further MSM5000_IRAM_FWDs for help with IRAM.
06/20/00    ak     Added MSM5000_IRAM_FWD to help with IRAM.
05/04/00    mvl    Changed calls to ppp_outgoing() to reflect new interface.
04/24/00    snn    Added support for CHAP Resync. The new changes have been 
                   added under the feature DS_CHAP feature. Note that these
                   changes currently work only if DS_CHAP_RESYNC feature
                   is also defined, because the call back is defined only if
                   DS_CHAP_RESYNC feature is defined.
10/27/99    mvl    Code review changes and general cleanup
08/20/99    mvl    Changed MSG_MED()s in various places to MEG_LOW()s
07/16/99    mvl    Added PAP support - inside DS_PAP feature.
08/31/98    na/ml  Created module.
===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"
#include "comdef.h"
#ifdef FEATURE_DATA
#include "assert.h"
#include "ps_ppp_snoop.h"
#include "ps_ppp.h"
#include "ps_pppi.h"
#include "ps_ppp_fsm.h"
#include "ps_ppp_defs.h"
#include "dsm.h"
#ifndef FEATURE_DATA_STRIP_ATCOP
#include "ps_ppp_netmodel.h"
#endif
#include "err.h"
#include "msg.h"
#include "memory.h"

#ifdef FEATURE_DS_MOBILE_IP
#include "dsmip.h"
#include "dsmip_msg.h"
#include "internet.h"
#endif /* FEATURE_DS_MOBILE_IP */



/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

===========================================================================*/
/*---------------------------------------------------------------------------
  All of the data structures associated with snoop function registration.
---------------------------------------------------------------------------*/
#define SNOOP_MAX_MSGS (TERM_REQ+1)     /* Max snoopable msgs              */

#define LINK_PROTO_BYTE1 0xC0    /* The first byte in lcp protocol field   */
#define LCP_BYTE2        0x21    /* The second byte in lcp protocol field  */

#define PAP_BYTE2        0x23    /* The second byte in pap protocol field  */

#define CHAP_BYTE1       0xC2    /* The First byte in chap protocol field  */
#define CHAP_BYTE2       0x23    /* The second byte in chap protocol field */

#define IPCP_BYTE1       0x80    /* The first byte in ipcp/ccp/ilccp field */
#define IPCP_BYTE2       0x21    /* The second byte in ipcp protocol field */
#define CCP_BYTE2        0xFD    /* The second byte in ccp protocol field  */
#define ILCCP_BYTE2      0xFB    /* The second byte in ilccp protocol      */

#define NO_COMPRESS      0x00    /* First byte in uncompr protocol field   */
#define TYPE_IP          0x21    /* Unique byte for IP protocol            */
#define VJ_COMPRESS      0x2D    /* Unique byte for compr VJ protocol      */
#define VJ_UNCOMPRESS    0x2F    /* Unique byte for uncompr VJ protocol    */

#define DEF_FN           SNOOP_ANY_MSG  /* default callback is for any msg */

/*---------------------------------------------------------------------------
  The function array index type.  This is used to index into the callback
  array, using symbols representing the protocol for which the callback is
  registered.
---------------------------------------------------------------------------*/
typedef enum
{
  FA_LCP_IDX =0,            /* the index into the callback array for LCP   */
  FA_IPCP_IDX,              /* the index into the callback array for IPCP  */
  FA_IP_IDX,                /* the index into the callback array for IP    */
  FA_VJCH_IDX,              /* the index into the callback array for VJCH  */
  FA_VJUCH_IDX,             /* the index into the callback array for VJUCH */
  FA_PAP_IDX,               /* the index into the callback array for PAP   */
  FA_CHAP_IDX,              /* the index into the callback array for CHAP  */
#ifdef FEATURE_DATA_PS_CCP_REJECT
  FA_CCP_IDX,               /* the index into the callback array for CCP   */
  FA_ILCCP_IDX,             /* the index into the callback array for ILCCP */
#endif /* FEATURE_DATA_PS_CCP_REJECT */
  FA_MAX_IDX                /* max value, used for bounds checking         */

} fa_index_enum_type;

/*lint -e641
 * Ignore the error of enums being converted to ints - that is the point!
*/

/*---------------------------------------------------------------------------
  The structure of the "opaque" registration tag.  This is only used
  internally to generate an ID for a particular callback registration, and to
  later parse that ID when deregistering that callback.
---------------------------------------------------------------------------*/
typedef struct
{
  byte proto;      /* the protocol index                                   */
  byte msg;        /* the message index                                    */
  byte iface;      /* the stack index                                      */
  byte fill;       /* filler for bits unused                               */
} snoop_tag_type;


/*---------------------------------------------------------------------------
  It is expected that the first element in the f_ptr_arr array will be the
  default callback function for the protocol in question.  And the rest of
  them will be the callbacks for particular messages in that protocol.  Each
  of these corresponds to a row in the table that follows, and contains
  possible function pointers for each of the possible PPP messages (C-Ack,
  T-Req, etc...)
---------------------------------------------------------------------------*/
typedef struct
{
  byte num_clbks;                                  /* # of reged callbacks */
  snoop_cb_f_ptr_type f_ptr_arr[SNOOP_MAX_MSGS];   /* slots for callbacks  */
} proto_msg_array_type;

/*---------------------------------------------------------------------------
  The table that contains the callbacks -- actually it is two tables, one for
  each interface -- each row corresponds to a protocol and contains the
  msg_array type, which contains function pointers for each possible message.
---------------------------------------------------------------------------*/
LOCAL proto_msg_array_type clbk_tab[PPP_MAX_DEV][FA_MAX_IDX];


/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                      EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

/*===========================================================================
FUNCTION  SNOOP_INIT()

DESCRIPTION
  This function initiailzes the snoop data structures, namely it zeros out
  the data structure that contains all of the call back functions.  This
  function is called whenever a non packet call is made (i.e. sockets,
  async...)

DEPENDENCIES
  It is assumed that one can zero out a multidimentional array by knowing it's
  base address, the range of all of it's indices and the size of the base
  data type.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void snoop_init(void)
{
  /*lint -e648
   * Stop lint from complaining about overflows that won't happen
   */

  /*-------------------------------------------------------------------------
    The size used in the memset is:
      # of interface stacks * # of indecees * size off the callback array
  -------------------------------------------------------------------------*/
  (void)memset(clbk_tab,
               0,
               PPP_MAX_DEV*FA_MAX_IDX*sizeof(proto_msg_array_type));
  /*lint +e648 */

} /* snoop_init() */


/*===========================================================================
FUNCTION  SNOOP_REG_EVENT()

DESCRIPTION
  This function stores a function pointer that will be used as a callback
  when a particular PPP protocol and message are seen.

DEPENDENCIES
  None

RETURN VALUE
  A 32bit opaque identifier that can be used to unregister the function that
  has been registered.

SIDE EFFECTS
  If this function is called for a protocol and message combination that
  already has a function call associated with it, it will remove the old
  function and replace it with the new function that is being added.
===========================================================================*/
uint32 snoop_reg_event
(
  ppp_dev_enum_type     iface,    /* the stack the registration is for     */
  uint16                protocol, /* the protocol the registration is for  */
  byte                  msg_type, /* the message the registration is for   */
  snoop_cb_f_ptr_type   f_ptr     /* the callback funtion being registered */
)
{
  snoop_tag_type return_val;                /* return tag is 32 bit number */

  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  /*lint -e527 */
  ASSERT(iface < PPP_MAX_DEV);
  ASSERT(msg_type < SNOOP_MAX_MSGS);
  ASSERT(f_ptr != NULL);
  /*lint +e527 */

  /*-------------------------------------------------------------------------
    Figure out the protocol and do processing for that protocol
  -------------------------------------------------------------------------*/
  switch(protocol)
  {
    case PPP_LCP_PROTOCOL:
      /*---------------------------------------------------------------------
        Insert function pointer in appropriate LCP message slot
      ---------------------------------------------------------------------*/
      return_val.proto = FA_LCP_IDX;
      MSG_LOW("Reg LCP cb code: %d iface %d", msg_type, iface, 0);
      break;

    case PPP_PAP_PROTOCOL:
      /*---------------------------------------------------------------------
        Insert function pointer in appropriate PAP message slot
      ---------------------------------------------------------------------*/
      return_val.proto = FA_PAP_IDX;
      MSG_LOW("Reg PAP cb code %d iface %d", msg_type, iface, 0);
      break;

    case PPP_CHAP_PROTOCOL:
    /*-----------------------------------------------------------------------
      Insert function pointer in appropriate CHAP message slot
    ------------------------------------------------------------------------*/
      return_val.proto = FA_CHAP_IDX;
      MSG_LOW("Reg CHAP cb code %d iface %d", msg_type, iface, 0);
      break;

    case PPP_IPCP_PROTOCOL:
      /*---------------------------------------------------------------------
        Insert function pointer in appropriate IPCP message slot
      ---------------------------------------------------------------------*/
      return_val.proto = FA_IPCP_IDX;
      MSG_LOW("Reg IPCP cb code %d iface %d", msg_type, iface, 0);
      break;
    
    case PPP_IP_PROTOCOL:
      /*---------------------------------------------------------------------
        If the protocol is IP we ignore the message field sent in
        and instead place it in the first slot.
      ---------------------------------------------------------------------*/
      return_val.proto = FA_IP_IDX;
      msg_type = SNOOP_ANY_MSG;
      MSG_LOW("Reg IP callback code %d iface %d", msg_type, iface, 0);
      break;

    case PPP_VJCH_PROTOCOL:
      /*---------------------------------------------------------------------
        If the protocol is VJ Compressed we ignore the message field sent in
        and instead place it in the first slot.
      ---------------------------------------------------------------------*/
      return_val.proto = FA_VJCH_IDX;
      msg_type = SNOOP_ANY_MSG;
      MSG_LOW("Reg VJCH callback code %d iface %d", msg_type, iface, 0);
      break;

    case PPP_VJUCH_PROTOCOL:
      /*---------------------------------------------------------------------
        If the protocol is VJ Uncompressed we ignore the message field sent
        in and instead place it in the first slot.
      ---------------------------------------------------------------------*/
      return_val.proto = FA_VJUCH_IDX;
      msg_type = SNOOP_ANY_MSG;
      MSG_LOW("Reg VJUCH callback code %d iface %d", msg_type, iface, 0);
      break;

#ifdef FEATURE_DATA_PS_CCP_REJECT
    case PPP_CCP_PROTOCOL:
      /*---------------------------------------------------------------------
        Insert function pointer in appropriate CCP message slot
      ---------------------------------------------------------------------*/
      return_val.proto = FA_CCP_IDX;
      MSG_LOW("Reg CCP callback code %d iface %d", msg_type, iface, 0);
      break;

    case PPP_ILCCP_PROTOCOL:
      /*---------------------------------------------------------------------
        Insert function pointer in appropriate CCP message slot
      ---------------------------------------------------------------------*/
      return_val.proto = FA_ILCCP_IDX;
      MSG_LOW("Reg ILCCP callback code %d iface %d", msg_type, iface, 0);
      break;
#endif /* FEATURE_DATA_PS_CCP_REJECT */

    default:
      /*---------------------------------------------------------------------
        We have been passed an invalid protocol, ASSERT() failure.
      ---------------------------------------------------------------------*/
      ASSERT(0);
  } /* switch(protocol) */

  /*lint -e644 -e661 */
  /*-------------------------------------------------------------------------
    Register the callback function
  -------------------------------------------------------------------------*/
  if(clbk_tab[iface][return_val.proto].f_ptr_arr[msg_type]==NULL)
  {
    clbk_tab[iface][return_val.proto].num_clbks += 1;
  }
  clbk_tab[iface][return_val.proto].f_ptr_arr[msg_type]=f_ptr;
  /*lint +e644 +e661 */

  /*-------------------------------------------------------------------------
    Assign rest of parameters to the registration tag
  -------------------------------------------------------------------------*/
  return_val.msg = msg_type;
  return_val.iface = (byte)iface;
  return_val.fill = 0;

  MSG_LOW ("Registered iface %d prot %d msg %d", 
           return_val.iface, return_val.proto, return_val.msg);
  /*lint -e740 */
  /*-------------------------------------------------------------------------
    Return the tag - do evil casting to convert data struct to uint32.
  -------------------------------------------------------------------------*/
  return *((uint32*)(&return_val));

  /*lint -e740 */
} /* snoop_reg_event() */



/*===========================================================================
FUNCTION  SNOOP_UNREG_EVENT()

DESCRIPTION
  This function removes a particular callback function from the snoop
  registration data structure.  This stops it from being called when its
  associated protocol and message are found.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void snoop_unreg_event
(
  uint32 reg_tag        /* the xparent reg tag returned by snoop_reg_event */
)
{
  /*lint -e740 */
  /*-------------------------------------------------------------------------
    Convert the uint32 into the snoop_tag_type so that fields are easily
    accessible and understandable.
  -------------------------------------------------------------------------*/
  snoop_tag_type *tag = (snoop_tag_type*)&reg_tag;
  /*lint +e740 */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    Make sure tag fields are valid
  -------------------------------------------------------------------------*/
  /*lint -e527 */
  ASSERT(tag->proto < FA_MAX_IDX);
  ASSERT(tag->msg < SNOOP_MAX_MSGS);
  ASSERT(tag->iface < PPP_MAX_DEV);
  /*lint +e527 */

  /*-------------------------------------------------------------------------
    Make sure that there is a function registered for the tag that is passed
    in.  If so clear it and return, otherwise tag is bogus, so generate an
    error.
  -------------------------------------------------------------------------*/
  if(clbk_tab[tag->iface][tag->proto].f_ptr_arr[tag->msg] != NULL)
  {
    MSG_LOW ("Unregister iface %d prot %d msg %d", 
             tag->iface, tag->proto, tag->msg);
    clbk_tab[tag->iface][tag->proto].num_clbks -= 1;
    clbk_tab[tag->iface][tag->proto].f_ptr_arr[tag->msg] = NULL;
  }
  else
  {
    MSG_MED("Unregistering non-existent function!", 0, 0, 0);
  }
} /* snoop_unreg_event() */



/*===========================================================================
FUNCTION  SNOOP_UNREG_ALL()

DESCRIPTION
  This function removes all callback functions that are registered for ALL
  protocols.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void snoop_unreg_all()
{
  snoop_init();
}



#ifndef MSM5000_IRAM_FWD
/*===========================================================================
FUNCTION  SNOOP_EARLY_PROTO_DETECT()

DESCRIPTION
  This function does early protocol detection of unframed PPP packets. It is
  passed a dsm_item that contains either the first two or four UNESCAPED
  bytes of a PPP packet. These are used to determine if this is a packet we
  are interested in.

  The item_ptr argument must have either 2 or 4 bytes.

  The item_ptr->kind field specifies whether this pkt. was on the Um or the
  Rm interface.

DEPENDENCIES
  This function expects that the first two or four bytes of the PPP packet
  are the first two or four bytes of the first DSM item.

RETURN VALUE
  One of three values:
  SNOOP_IGNORE_PKT:     Return to looking only for the 7E
  SNOOP_DO_UNFRAMING:   Unframe whole packet are send to snoop
  SNOOP_NEXT_TWO_BYTES: Not enough information here, need to look at next
                            two bytes.

SIDE EFFECTS
  None
===========================================================================*/
snoop_proto_ret_val_enum_type snoop_early_proto_detect
(
  ppp_dev_enum_type     iface,          /* iface (Rm or Um) pkt arrived on */
  const dsm_item_type  *item_ptr        /* Item containing input PPP pkt.  */
)
{

  byte* curr_byte_ptr;                 /* pointer to byte we are examining */
  ppp_dev_enum_type index;         /* Rm or Um index in callback array */

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*lint -e527 -esym(613, item_ptr) */
  ASSERT(item_ptr != NULL);
  ASSERT(item_ptr->used  == PPP_PROTO_SIZ ||
         item_ptr->used  == PPP_ACF_PROTO_SIZ);
  ASSERT(iface < PPP_MAX_DEV);
  /*lint +e527 */

  /*--------------------------------------------------------------------------
    When there are 2 bytes in the packet, check for Addr/Proto field if that
    is missing, they should be protocol.
  --------------------------------------------------------------------------*/
  if( item_ptr->used == PPP_PROTO_SIZ)
  {
    /*------------------------------------------------------------------------
      Get the first byte of the pkt.  If we find FF03 next two bytes are
      needed to determine protocol return this indication.  If the first two
      bytes are FFXX this packet is weird and so should be ignored, return
      this indication.
    ------------------------------------------------------------------------*/
    curr_byte_ptr = item_ptr->data_ptr;

    if( *curr_byte_ptr == HDLC_ALL_ADDR)
    {
      if( *(curr_byte_ptr+1) == HDLC_UI)
      {
        MSG_LOW("Got FF03, snoop next two bytes.",0, 0, 0);
        return SNOOP_NEXT_TWO_BYTES;
      }
      else
      {
        MSG_LOW("Got FF%02X, ignoring.",
                *(curr_byte_ptr+1), 0, 0);
        return SNOOP_IGNORE_PKT;
      }
    } /* if FF */

  } /* if(2 byte pkt) */

  else /* look at following two bytes */
  {
    curr_byte_ptr = item_ptr->data_ptr + PPP_PROTO_SIZ;
  }

  /*--------------------------------------------------------------------------
    Set the index of the function lookup to the input interface (Um or Rm).
    Figure out what protocol we have, by looking at the two protocol bytes.
  --------------------------------------------------------------------------*/
  index = iface;

  /*lint -e661 -e662 */
  /*-------------------------------------------------------------------------
    Determine the protocol in the message
  -------------------------------------------------------------------------*/
  switch(*curr_byte_ptr)
  {
    case LINK_PROTO_BYTE1:
      /*---------------------------------------------------------------------
        Check if we got LCP, and if there is a callback.
      ---------------------------------------------------------------------*/
      if( (*(curr_byte_ptr+1)) == LCP_BYTE2)
      {
        if( clbk_tab[index][FA_LCP_IDX].num_clbks > 0 )
        {
          MSG_LOW("Found LCP callback, unframe", 0, 0, 0);
          return SNOOP_DO_UNFRAMING;
        }
        MSG_LOW("Got LCP, no callback", 0, 0, 0);
      }

      /*---------------------------------------------------------------------
        Check if we got PAP, and if there is a callback.
      ---------------------------------------------------------------------*/
      else if( (*(curr_byte_ptr+1)) == PAP_BYTE2)
      {
        if( clbk_tab[index][FA_PAP_IDX].num_clbks > 0 )
        {
          MSG_LOW("Found PAP callback, unframe", 0, 0, 0);
          return SNOOP_DO_UNFRAMING;
        }
        MSG_LOW("Got PAP, no callback", 0, 0, 0);
      }
      break;

  case  CHAP_BYTE1:
      /*---------------------------------------------------------------------
        Check if we got CHAP, and if there is a callback.
      ---------------------------------------------------------------------*/
      if( (*(curr_byte_ptr+1)) == CHAP_BYTE2)
      {
        if( clbk_tab[index][FA_CHAP_IDX].num_clbks > 0 )
        {
          MSG_LOW("Found CHAP callback, unframe", 0, 0, 0);
          return SNOOP_DO_UNFRAMING;
        }
        MSG_LOW("Got CHAP, no callback", 0, 0, 0);
      }
      break;

    case IPCP_BYTE1:
      /*---------------------------------------------------------------------
        Check if we got IPCP, and if there is a callback.
      ---------------------------------------------------------------------*/
      if( (*(curr_byte_ptr+1)) == IPCP_BYTE2)
      {
        if( clbk_tab[index][FA_IPCP_IDX].num_clbks > 0 )
        { 
          MSG_LOW("Found IPCP callback, unframe", 0, 0, 0);
          return SNOOP_DO_UNFRAMING;
        }
        MSG_LOW("Got IPCP, no callback", 0, 0, 0);
      }
#ifdef FEATURE_DATA_PS_CCP_REJECT
      /*---------------------------------------------------------------------
        Check if we got CCP, and if there is a callback.
      ---------------------------------------------------------------------*/
      if( (*(curr_byte_ptr+1)) == CCP_BYTE2)
      {
        if( clbk_tab[index][FA_CCP_IDX].num_clbks > 0 )
        { 
          MSG_LOW("Found CCP callback, unframe", 0, 0, 0);
          return SNOOP_DO_UNFRAMING;
        }
        MSG_LOW("Got CCP, no callback", 0, 0, 0);
      }
      /*---------------------------------------------------------------------
        Check if we got ILCCP, and if there is a callback.
      ---------------------------------------------------------------------*/
      if( (*(curr_byte_ptr+1)) == ILCCP_BYTE2)
      {
        if( clbk_tab[index][FA_ILCCP_IDX].num_clbks > 0 )
        { 
          MSG_LOW("Found ILCCP callback, unframe", 0, 0, 0);
          return SNOOP_DO_UNFRAMING;
        }
        MSG_LOW("Got ILCCP, no callback", 0, 0, 0);
      }
#endif /* FEATURE_DATA_PS_CCP_REJECT */
      break;

    case  NO_COMPRESS:
      /*---------------------------------------------------------------------
        In this case we simply want to skip over this byte (in doesn't tell
        us anything) and check the following byte.  Then check for callbacks.
      ---------------------------------------------------------------------*/
      curr_byte_ptr++;
      
      if(*curr_byte_ptr == TYPE_IP &&
         clbk_tab[index][FA_IP_IDX].num_clbks > 0 )
      {
        MSG_LOW("Found IP protocol callback, unframe", 0, 0, 0);
        return SNOOP_DO_UNFRAMING;
      }
      
      if(*curr_byte_ptr == VJ_COMPRESS &&
         clbk_tab[index][FA_VJCH_IDX].num_clbks > 0 )
      {
        MSG_LOW("Found VJ compressed callback, unframe", 0, 0, 0);
        return SNOOP_DO_UNFRAMING;
      }

      else if(*curr_byte_ptr == VJ_UNCOMPRESS &&
              clbk_tab[index][FA_VJUCH_IDX].num_clbks > 0)
      {
        MSG_LOW("Found VJ uncompressed callback, unframe", 0, 0, 0);
        return SNOOP_DO_UNFRAMING;
      }
      break;

    case TYPE_IP:
      /*---------------------------------------------------------------------
        The byte we have seen indicates IP protocol, check for a callback
      ---------------------------------------------------------------------*/
      if( clbk_tab[index][FA_IP_IDX].num_clbks > 0 )
      {
        MSG_LOW("Found IP protocol callback, unframe", 0, 0, 0);
        return SNOOP_DO_UNFRAMING;
      }
      break;

    case VJ_COMPRESS:
      /*---------------------------------------------------------------------
        The byte we have seen indicats VJ compressed, check for a callback
      ---------------------------------------------------------------------*/
      if( clbk_tab[index][FA_VJCH_IDX].num_clbks > 0 )
      {
        MSG_LOW("Found VJ compressed callback, unframe", 0, 0, 0);
        return SNOOP_DO_UNFRAMING;
      }
      break;

    case VJ_UNCOMPRESS:
      /*---------------------------------------------------------------------
        The byte we have seen indicats VJ uncompressed, check for a callback
      ---------------------------------------------------------------------*/
      if( clbk_tab[index][FA_VJUCH_IDX].num_clbks > 0 )
      {
        MSG_LOW("Found VJ compressed callback, unframe", 0, 0, 0);
        return SNOOP_DO_UNFRAMING;
      }
      break;

    default:
      /*---------------------------------------------------------------------
        Ignore any other byte.
      ---------------------------------------------------------------------*/
      MSG_LOW("Found unknown protocol", 0, 0, 0);
  } /* switch(protocol) */
  /*lint +e661 +e662 */

  /*--------------------------------------------------------------------------
    If we made it this far, we know we are not interested, so we can ingnore
    the rest of the packet.
  --------------------------------------------------------------------------*/
  MSG_LOW("Ignoring packet.", 0, 0, 0);
  return SNOOP_IGNORE_PKT;

  /*lint +e613 */
} /* snoop_early_proto_detect() */
#endif /* MSM5000_IRAM_FWD */



#ifndef MSM5000_IRAM_FWD
/*===========================================================================
FUNCTION  SNOOP_PROTO_MSG_DETECT()

DESCRIPTION
  This function is given a pointer to a dsm_item_ptr and looks at the
  protocol and msg fields to determine if a callback function has been
  registered for this protocol and message combination.

DEPENDENCIES
  This function expects that the first four bytes of the PPP packet are in
  the first DSM item.
  It is also expected that the FF03 has been removed from the front and that
  the protocol ID has been stripped from the packet. The protocol of the 
  packet is known and is passed as an input parameter.

RETURN VALUE
  SNOOP_CB_IGNORE_PKT      packet processing complete, ignore it
  SNOOP_CB_SNOOP_AGAIN     call snoop callback again with the same DSM item -
                           implies that there is a new callback that needs 
                           to process this packet
  SNOOP_CB_PROCESS_PKT     send pkt up the stack for further processing

SIDE EFFECTS
  None
===========================================================================*/
snoop_cb_ret_val_enum_type snoop_proto_msg_detect
(
  ppp_dev_enum_type device,             /* device (Rm or Um) pkt coming on   */
  dsm_item_type **item_head_ptr,       /* pointer to packet chain of data  */
  uint16 protocol                      /* protocol of packet               */
)
{
  byte* msg_byte_ptr;                  /* pointer to byte we are examining */
  snoop_cb_ret_val_enum_type ret_val;  /* how to process pkt afterwards    */
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if( NULL == item_head_ptr || NULL == *item_head_ptr )
  {
    MSG_ERROR( "NULL item", 0, 0, 0 );
    return SNOOP_CB_IGNORE_PKT;
  }

  if( PPP_MAX_DEV <= device )
  {
    ASSERT( 0 );
    dsm_free_packet( item_head_ptr );
    return SNOOP_CB_IGNORE_PKT;
  }

  if( PPP_MAX_DEV <= ppp_cb_array[device].bridge_dev )
  {
    ASSERT( 0 );
    dsm_free_packet( item_head_ptr );
    return SNOOP_CB_IGNORE_PKT;
  }

  /*-------------------------------------------------------------------------
    Call the appropriate snooping callback, if no callback is registered
    take the default action depending upon PPP mode
  -------------------------------------------------------------------------*/

  ret_val = (ppp_cb_array[device].mode == PPP_INTERNAL_SNOOP_MODE) ?
    SNOOP_CB_PROCESS_PKT :
    SNOOP_CB_FORWARD_PKT;
  
  msg_byte_ptr = (*item_head_ptr)->data_ptr;
  MSG_LOW("snooping device %d, prot %x, msg %d", 
          device, protocol, *msg_byte_ptr);


  switch(protocol)
  {
    case PPP_LCP_PROTOCOL:
      /*---------------------------------------------------------------------
        First check to see if there is a default callback for this protocol,
        if so call that function and return its return value.
      ---------------------------------------------------------------------*/
      if(clbk_tab[device][FA_LCP_IDX].f_ptr_arr[DEF_FN] != NULL)
      {
        MSG_LOW("device %d: Calling default LCP callback",
                device, 0, 0);
        return (*clbk_tab[device][FA_LCP_IDX].f_ptr_arr[DEF_FN])
          (device, PPP_LCP_PROTOCOL, item_head_ptr);
      }
      /*---------------------------------------------------------------------
        Check if a function was registered for a message. If yes, call
        that function.
      ---------------------------------------------------------------------*/
      else if(*msg_byte_ptr < SNOOP_MAX_MSGS &&
              clbk_tab[device][FA_LCP_IDX].f_ptr_arr[*msg_byte_ptr] != NULL)
      {
        MSG_LOW("device %d: LCP msg %d, calling callback",
                device, *msg_byte_ptr, 0);
        return (*clbk_tab[device][FA_LCP_IDX].f_ptr_arr[*msg_byte_ptr])
          (device, PPP_LCP_PROTOCOL, item_head_ptr);
      }

      /*---------------------------------------------------------------------
        Send packet to TX framer or up the stack based upon the mode
      ---------------------------------------------------------------------*/
      else
      {
        MSG_LOW("device %d: LCP msg %d, no callbacks registered",
                device, *msg_byte_ptr, 0);
        (*item_head_ptr)->kind = DSM_PS_LCP_PROTOCOL;
      }
      break;

    case PPP_PAP_PROTOCOL:
      /*---------------------------------------------------------------------
        First check to see if there is a default callback for this protocol,
        if so call that function and return its return value.
      ---------------------------------------------------------------------*/
      if(clbk_tab[device][FA_PAP_IDX].f_ptr_arr[DEF_FN] != NULL)
      {
        MSG_LOW("device %d: Calling default PAP callback",
                device, 0, 0);
        return (*clbk_tab[device][FA_PAP_IDX].f_ptr_arr[DEF_FN])
          (device, PPP_PAP_PROTOCOL, item_head_ptr);
      }
      /*---------------------------------------------------------------------
        Check if a function was registered for a message. If yes, call
        that function.
      ---------------------------------------------------------------------*/
      else if(*msg_byte_ptr < SNOOP_MAX_MSGS &&
              clbk_tab[device][FA_PAP_IDX].f_ptr_arr[*msg_byte_ptr] != NULL)
      {
        MSG_LOW("device %d: PAP msg %d, calling callback",
                device, *msg_byte_ptr, 0);
        return (*clbk_tab[device][FA_PAP_IDX].f_ptr_arr[*msg_byte_ptr])
          (device, PPP_PAP_PROTOCOL, item_head_ptr);
      }
      /*---------------------------------------------------------------------
        Send packet to TX framer or up the stack based upon the mode
      ---------------------------------------------------------------------*/
      else
      {
        MSG_LOW("device %d: PAP msg %d, no callbacks registered",
                device, *msg_byte_ptr, 0);
        (*item_head_ptr)->kind = DSM_PS_PAP_PROTOCOL;
      }
      break;

    case PPP_CHAP_PROTOCOL:
      /*---------------------------------------------------------------------
        First check to see if there is a default callback for this protocol,
        if so call that function and return its return value.
      ---------------------------------------------------------------------*/
      if(clbk_tab[device][FA_CHAP_IDX].f_ptr_arr[DEF_FN] != NULL)
      {
        MSG_LOW("device %d: Calling default CHAP callback",
                device, 0, 0);
        return (*clbk_tab[device][FA_CHAP_IDX].f_ptr_arr[DEF_FN])
          (device, PPP_CHAP_PROTOCOL, item_head_ptr);
      }
      /*---------------------------------------------------------------------
        Check if a function was registered for a message. If yes, call
        that function.
      ---------------------------------------------------------------------*/
      else if(*msg_byte_ptr < SNOOP_MAX_MSGS &&
              clbk_tab[device][FA_CHAP_IDX].f_ptr_arr[*msg_byte_ptr] != NULL)
      {
        MSG_LOW("device %d: CHAP msg %d, calling callback",
                device, *msg_byte_ptr, 0);
        return (*clbk_tab[device][FA_CHAP_IDX].f_ptr_arr[*msg_byte_ptr])
          (device, PPP_CHAP_PROTOCOL, item_head_ptr);
      }
      /*---------------------------------------------------------------------
        Send packet to TX framer or up the stack based upon the mode
      ---------------------------------------------------------------------*/
      else
      {
        MSG_LOW("device %d: CHAP msg %d, no callbacks registered",
                device, *msg_byte_ptr, 0);
        (*item_head_ptr)->kind = DSM_PS_CHAP_PROTOCOL;
      }
      break;

    case PPP_IPCP_PROTOCOL:
      /*---------------------------------------------------------------------
        First check to see if there is a default callback for this protocol,
        if so call that function.
      ---------------------------------------------------------------------*/
      if(clbk_tab[device][FA_IPCP_IDX].f_ptr_arr[DEF_FN] != NULL)
      {
        MSG_LOW("device %d: Calling default IPCP callback",
                device, 0, 0);
        return (*clbk_tab[device][FA_IPCP_IDX].f_ptr_arr[DEF_FN])
          (device, PPP_IPCP_PROTOCOL, item_head_ptr);
      }
      /*---------------------------------------------------------------------
        Check if a function was registered for a message. If yes, call
        that function.
      ---------------------------------------------------------------------*/
      else if(*msg_byte_ptr < SNOOP_MAX_MSGS &&
              clbk_tab[device][FA_IPCP_IDX].f_ptr_arr[*msg_byte_ptr]!= NULL)
      {
        MSG_LOW("device %d: IPCP msg %d, calling callback",
                device, *msg_byte_ptr, 0);
        return (*clbk_tab[device][FA_IPCP_IDX].f_ptr_arr[*msg_byte_ptr])
          (device, PPP_IPCP_PROTOCOL, item_head_ptr);
      }

      /*---------------------------------------------------------------------
        Send packet to TX framer or up the stack based upon the mode
      ---------------------------------------------------------------------*/
      else 
      {
        MSG_LOW("device %d: IPCP msg %d, no callbacks registered",
                device, *msg_byte_ptr, 0);
        (*item_head_ptr)->kind = DSM_PS_IPCP_PROTOCOL;
      }
      break;

#ifdef FEATURE_DATA_PS_CCP_REJECT
    case PPP_CCP_PROTOCOL:
      /*---------------------------------------------------------------------
        First check to see if there is a default callback for this protocol,
        if so call that function.
      ---------------------------------------------------------------------*/
      if(clbk_tab[device][FA_CCP_IDX].f_ptr_arr[DEF_FN] != NULL)
      {
        MSG_LOW("device %d: Calling default CCP callback",
                device, 0, 0);
        return (*clbk_tab[device][FA_CCP_IDX].f_ptr_arr[DEF_FN])
          (device, PPP_CCP_PROTOCOL, item_head_ptr);
      }
      /*---------------------------------------------------------------------
        Check if a function was registered for a message. If yes, call
        that function.
      ---------------------------------------------------------------------*/
      else if(*msg_byte_ptr < SNOOP_MAX_MSGS &&
              clbk_tab[device][FA_CCP_IDX].f_ptr_arr[*msg_byte_ptr]!= NULL)
      {
        MSG_LOW("device %d: CCP msg %d, calling callback",
                device, *msg_byte_ptr, 0);
        return (*clbk_tab[device][FA_CCP_IDX].f_ptr_arr[*msg_byte_ptr])
          (device, PPP_CCP_PROTOCOL, item_head_ptr);
      }
      /*---------------------------------------------------------------------
        Send packet to TX framer or up the stack based upon the mode
      ---------------------------------------------------------------------*/
      else 
      {
        MSG_LOW("device %d: CCP msg %d, no callbacks registered",
                device, *msg_byte_ptr, 0);
        (*item_head_ptr)->kind = DSM_PS_CCP_PROTOCOL;
      }
      break;

    case PPP_ILCCP_PROTOCOL:
      /*---------------------------------------------------------------------
        First check to see if there is a default callback for this protocol,
        if so call that function.
      ---------------------------------------------------------------------*/
      if(clbk_tab[device][FA_ILCCP_IDX].f_ptr_arr[DEF_FN] != NULL)
      {
        MSG_LOW("device %d: Calling default ILCCP callback",
                device, 0, 0);
        return (*clbk_tab[device][FA_ILCCP_IDX].f_ptr_arr[DEF_FN])
          (device, PPP_ILCCP_PROTOCOL, item_head_ptr);
      }
      /*---------------------------------------------------------------------
        Check if a function was registered for a message. If yes, call
        that function.
      ---------------------------------------------------------------------*/
      else if(*msg_byte_ptr < SNOOP_MAX_MSGS &&
              clbk_tab[device][FA_ILCCP_IDX].f_ptr_arr[*msg_byte_ptr]!= NULL)
      {
        MSG_LOW("device %d: ILCCP msg %d, calling callback",
                device, *msg_byte_ptr, 0);
        return (*clbk_tab[device][FA_ILCCP_IDX].f_ptr_arr[*msg_byte_ptr])
          (device, PPP_ILCCP_PROTOCOL, item_head_ptr);
      }
      /*---------------------------------------------------------------------
        Send packet to TX framer or up the stack based upon the mode
      ---------------------------------------------------------------------*/
      else 
      {
        MSG_LOW("device %d: ILCCP msg %d, no callbacks registered",
                device, *msg_byte_ptr, 0);
        (*item_head_ptr)->kind = DSM_PS_ILCCP_PROTOCOL;
      }
      break;
#endif /* FEATURE_DATA_PS_CCP_REJECT */

    case PPP_IP_PROTOCOL:
      /*---------------------------------------------------------------------
        The byte we have seen indicates IP protocol.  Since there are no
        specific messages related to this protocol all we have to check is
        that the default function pointer has a value.
      ---------------------------------------------------------------------*/
      if( clbk_tab[device][FA_IP_IDX].f_ptr_arr[DEF_FN] != NULL)
      {
        MSG_LOW("device %d: Calling default IP callback", device, 0, 0);
        return (*clbk_tab[device][FA_IP_IDX].f_ptr_arr[DEF_FN])
          (device, PPP_IP_PROTOCOL, item_head_ptr);
      }
      else
      {
        /*-------------------------------------------------------------------
          Send packet to TX framer or up the stack based upon the mode
        -------------------------------------------------------------------*/
        MSG_LOW("device %d: IP packet, no callbacks registered",
                device, 0, 0);
        (*item_head_ptr)->kind = PPP_IP_PROTOCOL;
      }
      break;
    
    case PPP_VJCH_PROTOCOL:
      /*---------------------------------------------------------------------
        The byte we have seen indicates VJ compressed.  Since there are no
        specific messages related to this protocol all we have to check is
        that the default function pointer has a value.
      ---------------------------------------------------------------------*/
      if( clbk_tab[device][FA_VJCH_IDX].f_ptr_arr[DEF_FN] != NULL)
      {
        MSG_LOW("device %d: Calling default VJCH callback", device, 0, 0);
        return (*clbk_tab[device][FA_VJCH_IDX].f_ptr_arr[DEF_FN])
          (device, PPP_VJCH_PROTOCOL, item_head_ptr);
      }
      else
      {
        /*-------------------------------------------------------------------
          Send packet to TX framer or up the stack based upon the mode
        -------------------------------------------------------------------*/
        MSG_LOW("device %d: VJCH packet, no callbacks registered",
                device, 0, 0);
        (*item_head_ptr)->kind = PPP_VJCH_PROTOCOL;
      }
      break;

    case PPP_VJUCH_PROTOCOL:
      /*---------------------------------------------------------------------
        The byte we have seen indicates VJ uncompressed.  Since there are no
        specific messages related this protocol all we have to check is that
        the default function pointer has a value.
      ---------------------------------------------------------------------*/
      if(clbk_tab[device][FA_VJUCH_IDX].f_ptr_arr[DEF_FN] != NULL)
      {
        MSG_LOW("device %d: Calling default VJUCH callback", device, 0, 0);
        return (*clbk_tab[device][FA_VJUCH_IDX].f_ptr_arr[DEF_FN])
          (device, PPP_VJUCH_PROTOCOL, item_head_ptr);
      }
      else
      {
        /*-------------------------------------------------------------------
          Send packet to TX framer or up the stack based upon the mode
        -------------------------------------------------------------------*/
        MSG_LOW("device %d: VJUCH packet, no callbacks registered",
                device, 0, 0);
        (*item_head_ptr)->kind = PPP_VJUCH_PROTOCOL;
      }
      break;

    default:
      /*---------------------------------------------------------------------
        Send packet to TX framer or up the stack based upon the mode
      ---------------------------------------------------------------------*/
      MSG_LOW("Unknown protocol %x on device %d -- forwarding", 
              protocol, device, 0);
  } /* switch protocol */

  /*-------------------------------------------------------------------------
    if we make it this far it means there were no callbacks registered,
    so tell the caller to perform default handling. 
  -------------------------------------------------------------------------*/
  return ret_val;

  /*lint +e413 +e613 */

}  /* snoop_proto_msg_detect() */
#endif /* MSM5000_IRAM_FWD */

#ifdef FEATURE_DS_MOBILE_IP
/*===========================================================================
FUNCTION  SNOOP_IS_REGISTRTION_PKT()

DESCRIPTION
  This function is given a pointer to a dsm_item_ptr and looks at the
  packet to determine if it is a MIP registration reply packet or an
  agent advertisement.

DEPENDENCIES
  It is expected that the FF03 has been removed from the front and that
  the protocol ID has been stripped from the packet. The protocol of the 
  packet is known and is passed as an input parameter.

RETURN VALUE
  TRUE: The packet is a MIP registration related packet (process internally)
  FALSE: The packet is not a MIP reg. pkt (can forward to laptop)

SIDE EFFECTS
  None
===========================================================================*/
boolean snoop_is_registration_pkt
(
  dsm_item_type **item_head_ptr,       /* pointer to packet chain of data  */
  uint16 protocol                      /* protocol of packet               */
)
{
  uint32   source_addr;               /* Source IP address in the packet   */
  byte     icmp_type;          /* ICMP type field, if it is an ICMP packet */
  byte     ip_proto;                  /* IP protocol (UDP/TCP/etc.)        */

  ASSERT(item_head_ptr != NULL && *item_head_ptr != NULL);

  switch(protocol)
  {
    case PPP_PAP_PROTOCOL:
    case PPP_CHAP_PROTOCOL:
    case PPP_LCP_PROTOCOL:
    case PPP_IPCP_PROTOCOL:
      /*----------------------------------------------------------------------
        We already handled this stuff internally, don't forward to Rm
      ----------------------------------------------------------------------*/
      return TRUE;

    case PPP_VJCH_PROTOCOL:
    case PPP_VJUCH_PROTOCOL:
      /*----------------------------------------------------------------------
        VJ compression acts only on TCP packets, and MIP reg'n packets
        are UDP and ICMP only, so pass these on
      ----------------------------------------------------------------------*/
      return FALSE;

    case PPP_IP_PROTOCOL:
      /*----------------------------------------------------------------------
        Check if this is a registration packet. 

        If the packet originates from the FA and the protocol is UDP, it 
        could potentially be a RRP packet and need to be handled internally 
        by the Um protocol stack.

        If the protocol is ICMP and the type is 9 (router advertisement), it
        could potentially be an agent advertisement, so let the Um handle it
        internally.

        Assume here that byte order is okay. Must add code to take care if
        it ever changes

      ----------------------------------------------------------------------*/

      /* Read the IP protocol */
      dsm_extract(*item_head_ptr, PPP_IPPROTO_OFFSET,
                  &ip_proto,sizeof(ip_proto));

      if (ip_proto == UDP_PTCL)
      {
        /* Read the IP source address                                         */
        dsm_extract(*item_head_ptr, PPP_IPSADDR_OFFSET,
                    &source_addr,sizeof(source_addr));

        /* check for RRP */
        if (source_addr == ma_info.ma_addr)
        {
          /* JD - might also check port # for 434 for reg'n packet, in
             case PDSN talks to mobile apps */
          return TRUE;
        }
      } 

      if (ip_proto == ICMP_PTCL)
      {
        /*--------------------------------------------------------------------
          Read the ICMP type field, assuming it is an ICMP packet.
          The ICMP type field is the first byte in the ICMP header.
          It's offset is found by looking into the length field in the IP 
          header (first lower nibble in units of 4-bytes).
        --------------------------------------------------------------------*/
        dsm_extract(*item_head_ptr,
                    ((* ((byte*) (*item_head_ptr)->data_ptr)) & 0x0f) * 4,
                    &icmp_type,sizeof(icmp_type));

        /* check for agent advetisement */
        if (icmp_type == ICMP_MSGTYPE_ROUTER_ADVERTISEMENT)
        {
          return TRUE;
        }
      }
      return FALSE;

    default:
      break;
  }

  return FALSE;
}
#endif /* FEATURE_DS_MOBILE_IP */

#endif /*FEATURE_DATA */
