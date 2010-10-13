/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                Diagnostics Packet Processing Common Routines

General Description
  Core diagnostic packet processing routines that are common to all targets.

Copyright (c) 2000-2008 Qualcomm Incorporated. 
All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                           Edit History

$Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diagdiag.c#4 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/25/06   kss     Use dword transfers for PEEKD/POKED if address is aligned.
09/17/06   kss     Use word transfers for PEEKW/POKEW if address is aligned.
10/31/05    as     Fixed lint errors.
05/19/05    as     Fixed argument name in DIAG POKE DWORD 
22/03/05    as     Added type cast in memory poke operations
06/11/04    gr     Added support for event masks.
05/18/04    as     Added security check to diagdiag_get_property and 
                   diagdiag_put_property. Removed support for DIAG_USER_CMD_F 
                   & DIAG_PERM_USER_CMD_F
10/28/03   as      Changes to fix errors when compiling with RVCT2.0
04/07/03   jct     Added featurization to dissallow certain operations that
                   can access memory (FEATURE_DIAG_DISALLOW_MEM_OPS):
                      DIAG_PEEKB_F
                      DIAG_PEEKW_F
                      DIAG_PEEKD_F
                      DIAG_POKEB_F
                      DIAG_POKEW_F
                      DIAG_POKED_F
                      DIAG_OUTP_F
                      DIAG_OUTPW_F
                      DIAG_INP_F
                      DIAG_INPW_F
01/07/03   djm     add FEATURE_DIAG_RPC support for WCDMA_PLT use
08/20/02   lad     Moved diagpkt_error() to diagpkt.c.
                   Moved diagpkt_mode_chg() to diagcomm_sio.c as
                   diagcomm_mode_chg_pkt().
                   Removed msg_nice[] and log_nice[] references pending a
                   redesign of the load balancing feature.
                   Moved diagpkt_dload() to diagdload.c.
                   Featurized outp/inp routines for off target removal.
06/07/02   lad     Added DIAG_PROTOCOL_LOOPBACK_F.
11/01/01   jal     Support for DIAG_SERIAL_CHG_F (switch to data mode)
09/18/01   jal     Support for DIAG_CONTROL_F (mode reset/offline)
08/20/01   jal     Support for Diag packet: DIAG_TS_F (timestamp),
                   DIAG_SPC_F (service programming code), DIAG_DLOAD_F
                   (start downloader), DIAG_OUTP_F/DIAG_OUTPW_F (IO
                   port byte/word output), DIAG_INP_F/DIAG_INPW_F (IO
                   port byte/word input) 
06/27/01   lad     Moved diagpkt_err_rsp() to diagpkt.c.
                   Updated diagpkt_stream_config() for logging service.
                   diagpkt_stream_config() can now return DIAGBUF_SIZ.
04/17/01   lad     Added #include of diagtune.h.
04/06/01   lad     Introduced typedefs for command codes, etc.
                   Updated DIAGPKT_SUBSYS_REQ_DEFINE macros.
                   Added diagpkt_subsys_alloc().
                   Removed diagpkt_process_request since it is not part of the
02/23/01   lad     Created file.

===========================================================================*/
#if defined __cplusplus
  extern "C" {
#endif

#include "comdef.h"
#include "customer.h"

#include "bio.h"
#include "diag.h"
#include "diagcmd.h"
#include "diagi.h"
#include "diagpkt.h"
#include "diagdiag.h"
#include "diagtarget.h"
#include "event.h"
#include "feature.h"

#ifdef FEATURE_DIAG_HW_ADDR_CHECK
#include "hw.h"
#endif

#include "msg.h"

#include "qw.h"
#include "ts.h"

#ifdef FEATURE_VOC_TASK
#include "voc.h"
#endif


/* Tell lint that we are OK with the fact that pkt_len and req are not
** used in some of the packets, as this is dictated by the awesome design */
/*lint -esym(715,pkt_len,req) */



#ifdef DEBUG_DIAG_TEST
#error code not present
#endif /* DEBUG_DIAG_TEST */

/*lint -save -e729 */
/* Symbol 'diag_prop_table' not explicitly initialized */
/* Also there is no reference to filling this table.  */


/*===========================================================================

FUNCTION DIAGDIAG_PEEKB

DESCRIPTION
  This procedure processes a received peek byte request. It performs the peek
  operation and formats a peek response message.

===========================================================================*/
PACKED void * diagdiag_peekb (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_PEEKB_F_req_type *req = (DIAG_PEEKB_F_req_type *) req_pkt;
  DIAG_PEEKB_F_rsp_type *rsp;
  const int rsp_len = sizeof(DIAG_PEEKB_F_rsp_type);
  int i;
  uint16 count;
  PACKED byte *src = NULL;
  PACKED byte *dest = NULL;

  count = req->length;
  src = (byte *) req->ptr;

  /*-------------------------------------------------------------------------
    Check security, since this is a secure funciton
  --------------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len) );
  }

  /*-------------------------------------------------------------------------
    Check memory peek length against max allowable length.
  --------------------------------------------------------------------------*/
  if (count > DIAG_MAX_PEEK_B) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }

#ifdef FEATURE_DIAG_HW_ADDR_CHECK
  /*-------------------------------------------------------------------------
    Check to see if block requested is within a valid range
  --------------------------------------------------------------------------*/
  if (!hw_valid_addr ((void *) src, count) ) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }
#endif

  /* Allocate space for response packet */
  rsp = (DIAG_PEEKB_F_rsp_type *)diagpkt_alloc(DIAG_PEEKB_F, rsp_len);

  if (rsp) {
    dest = &rsp->data[0];

    /*--------------------------------------------------------------------------
      Fill in the boilerplate for the response.
    --------------------------------------------------------------------------*/
    rsp->ptr      = (byte *) src;
    rsp->length   = count;

#ifdef FEATURE_VOC_TASK
    /*---------------------------------------------------------------------------
      The vocoder memory space is a special case since the DSP's clocks are
      turned off during sleep, making the address invalid.  Let voc handle it.
    ----------------------------------------------------------------------------*/
    if (voc_valid_addr ((void *) src, count))
    {
      voc_status_type eVocStatus;

      /* Since the data is byte ordered, no need to hanlde endian-ness here. */
      eVocStatus = voc_peek_cmd((void *) src, (void *) dest,
                                count * sizeof(byte));

      if (eVocStatus != VOC_DONE_S) {
        return (diagpkt_err_rsp (DIAG_BAD_PARM_F, req_pkt, pkt_len));
      }
    }
    else /* do it ourselves. */
#endif /* FEATURE_VOC_TASK */
    {
      /*----------------------------------------------------------------------
        Lock out interrupts to preserve consistency of the memory block.
      ----------------------------------------------------------------------*/
      INTLOCK ();
      for (i = 0; i < count; i++) {
        *dest = *src;
        src++;
        dest++;
      }
      INTFREE ();
    }
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_peekb.", 0, 0, 0);
  }

  return (rsp);

} /* diagdiag_peekb */



/*===========================================================================

FUNCTION DIAGDIAG_PEEKW

DESCRIPTION
  This procedure processes a received peek word request. It performs the peek
  operation and formats a peek response message.

===========================================================================*/
PACKED void * diagdiag_peekw (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_PEEKW_F_req_type *req =(DIAG_PEEKW_F_req_type *)req_pkt;
  DIAG_PEEKW_F_rsp_type *rsp;
  const int rsp_len = sizeof(DIAG_PEEKW_F_rsp_type);
  int i;
  uint16 count;
  PACKED word *src = NULL;
  PACKED word *dest = NULL;

  word * src_w;  /* Unpacked word pointer */
  word   val_w;  /* Result of peek */

  count = req->length;
  src = (word *) req->ptr;

  /*-------------------------------------------------------------------------
    Check security, since this is a secure funciton
  --------------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len) );
  }

  /*-------------------------------------------------------------------------
    Check block length against max allowed length.
  -------------------------------------------------------------------------*/
  if (count > DIAG_MAX_PEEK_W) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }

#ifdef FEATURE_DIAG_HW_ADDR_CHECK
 /*-------------------------------------------------------------------------
    Check to see that the block requested is within a valid range.
  --------------------------------------------------------------------------*/
  if (!hw_valid_addr ((void *) src, (word)(count * sizeof (word)))) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }
#endif

  /* Allocate space for response packet */
  rsp = (DIAG_PEEKW_F_rsp_type *)diagpkt_alloc(DIAG_PEEKW_F, rsp_len);

  if (rsp) {
    dest = &rsp->data[0];

    /*--------------------------------------------------------------------------
      Fill in the boilerplate for the response.
    --------------------------------------------------------------------------*/
    rsp->ptr = (word *) src;
    rsp->length = count;

#ifdef FEATURE_VOC_TASK
    /*---------------------------------------------------------------------------
      The vocoder memory space is a special case since the DSP's clocks are
      turned off during sleep, making the address invalid.  Let voc handle it.
    ----------------------------------------------------------------------------*/
    if (voc_valid_addr ((void *) src, (uint16) (count * sizeof(word))  ))
    {
      voc_status_type eVocStatus;

      /* Since the data is byte ordered, no need to hanlde endian-ness here. */
      eVocStatus = voc_peek_cmd ((void *) src, (void *) dest,
                                (uint16) (count * sizeof(word)) );

      if (eVocStatus != VOC_DONE_S)
      {
        return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
      }
    }
    else /* do it ourselves. */
#endif /* FEATURE_VOC_TASK */
    {
      /* Test for alignment. If source address is word aligned, use
         non-packed word transfers. */
      if (((dword)src & 0x1) == 0x00)
      {
        /* Copy packed pointer to non-packed pointer */
        src_w = (word*)src; 

        /*----------------------------------------------------------------------
          Lock out interrupts to preserve consistency of the memory block.
        ----------------------------------------------------------------------*/
        INTLOCK ();
        for (i = 0; i < count; i++) {
          val_w = *src_w;
          *dest = val_w;
          src_w++;
          dest++;
        }
        INTFREE ();
      }
      else  /* Copy with packed variables */
      {
        /*----------------------------------------------------------------------
          Lock out interrupts to preserve consistency of the memory block.
        ----------------------------------------------------------------------*/
        INTLOCK ();
        for (i = 0; i < count; i++) {
          *dest = *src;
          src++;
          dest++;
        }
        INTFREE ();
      }
    }
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_peekw.", 0, 0, 0);
  }
  return (rsp);

} /* diagdiag_peekw */



/*===========================================================================

FUNCTION DIAGDIAG_PEEKD

DESCRIPTION
  This procedure processes a received peek dword request. It performs the peek
  operation and formats a peek response message.

===========================================================================*/
PACKED void * diagdiag_peekd (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_PEEKD_F_req_type *req = (DIAG_PEEKD_F_req_type *)req_pkt;
  DIAG_PEEKD_F_rsp_type *rsp;
  const int rsp_len = sizeof(DIAG_PEEKD_F_rsp_type);
  int i;
  uint16 count;
  PACKED dword *src = NULL;
  PACKED dword *dest = NULL;

  dword * src_dw;  /* Unpacked word pointer */
  dword   val_dw;  /* Result of peek */

  count = req->length;
  src = (dword *) req->ptr;

  /*--------------------------------------------------------------------------
    Check security, since this is a secure funciton
  --------------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len) );
  }

  /*--------------------------------------------------------------------------
    Check requested length against max allowable length.
  --------------------------------------------------------------------------*/
  if (count > DIAG_MAX_PEEK_D) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }

#ifdef FEATURE_DIAG_HW_ADDR_CHECK
  /*-------------------------------------------------------------------------
    Check to see that the block requested is within a valid range.
  --------------------------------------------------------------------------*/
  if (!hw_valid_addr ((void *) src, (word)(count * sizeof(dword)))) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }
#endif

  /* Allocate space for response packet */
  rsp = (DIAG_PEEKD_F_rsp_type *)diagpkt_alloc(DIAG_PEEKD_F, rsp_len);

  if (rsp) {
    dest = &rsp->data[0];

    /*--------------------------------------------------------------------------
      Fill in the boilerplate for the response.
    --------------------------------------------------------------------------*/
    rsp->ptr      = (dword *) src;
    rsp->length   = count;

#ifdef FEATURE_VOC_TASK
    /*---------------------------------------------------------------------------
      The vocoder memory space is a special case since the DSP's clocks are
      turned off during sleep, making the address invalid.  Let voc handle it.
    ----------------------------------------------------------------------------*/
    if (voc_valid_addr ((void *) src, (uint16) (count * sizeof(dword)) ))
    {
      voc_status_type eVocStatus;

      /* Since the data is byte ordered, no need to hanlde endian-ness here. */
      eVocStatus = voc_peek_cmd ((void *) src, (void *) dest,
                                 (uint16) (count * sizeof(dword)));

      if (eVocStatus != VOC_DONE_S)
      {
        return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
      }
    }
    else /* do it ourselves. */
#endif /* FEATURE_VOC_TASK */
    {
      /* Test for alignment. If source address is dword aligned, use
         non-packed dword transfers. */
      if (((dword)src & 0x3) == 0x00)
      {
        /* Copy packed pointer to non-packed pointer */
        src_dw = (dword*)src; 

        /*----------------------------------------------------------------------
          Lock out interrupts to preserve consistency of the memory block.
        ----------------------------------------------------------------------*/
        INTLOCK ();
        for (i = 0; i < count; i++) {
          val_dw = *src_dw;
          *dest = val_dw;
          src_dw++;
          dest++;
        }
        INTFREE ();
      }
      else  /* Copy with packed variables */
      {
        /*----------------------------------------------------------------------
          Lock out interrupts to preserve consistency of the memory block.
        ----------------------------------------------------------------------*/
        INTLOCK ();
        for (i = 0; i < count; i++) {
          *dest = *src;
          dest++;
          src++; 
        }
        INTFREE ();
      }
    }
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_peekd.", 0, 0, 0);
  }

  return (rsp);

} /* diagdiag_peekd */



/*===========================================================================

FUNCTION DIAGDIAG_POKEB

DESCRIPTION
  This procedure pokes a byte into memory and formats a poke response
  buffer.

===========================================================================*/
PACKED void * diagdiag_pokeb (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_POKEB_F_req_type *req = (DIAG_POKEB_F_req_type *)req_pkt;
  DIAG_POKEB_F_rsp_type *rsp;
  const int rsp_len = sizeof(DIAG_POKEB_F_rsp_type);
  int i;
  uint16 count;
  PACKED byte *src = NULL;
  PACKED byte *dest = NULL;

  count = req->length;
  src = &req->data[0];
  dest = req->ptr;

  /*----------------------------------------------------------------------
    Check length of request packet.
  ----------------------------------------------------------------------*/
  if (pkt_len != sizeof (DIAG_POKEB_F_req_type))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
  ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len) );
  }

  /*----------------------------------------------------------------------
    Check block size against maximum allowable block size.
  ----------------------------------------------------------------------*/
  if (count > DIAG_MAX_POKE_B) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }

#ifdef FEATURE_DIAG_HW_ADDR_CHECK
  /*---------------------------------------------------------------------
    Check to see if address requested is within a valid range.
  ----------------------------------------------------------------------*/
  else if (!hw_valid_addr ((void *) dest, count * sizeof (byte))) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }
#endif

#ifdef FEATURE_VOC_TASK
  /*---------------------------------------------------------------------------
    The vocoder memory space is a special case since the DSP's clocks are
    turned off during sleep, making the address invalid.  Let voc handle it.
  ----------------------------------------------------------------------------*/
  if (voc_valid_addr ((void *) dest, (uint16) (count * sizeof (byte)) ))
  {
    voc_status_type eVocStatus;

    /* Since the data is byte ordered, no need to hanlde endian-ness here. */
    eVocStatus = voc_poke_cmd ((void *) dest, (void *) src,
                               count * sizeof (byte));

    if (eVocStatus != VOC_DONE_S)
    {
      return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
    }
  }
  else /* do it ourselves. */
#endif /* FEATURE_VOC_TASK */
  {
    /*----------------------------------------------------------------------
      Lock out interrupts to preserve consistency of the memory block.
    ----------------------------------------------------------------------*/
    INTLOCK ();
    for (i = 0; i < count; i++) {
      *dest = *src;
      dest++;
      src++;
    }
    INTFREE ();
  }

  /*-------------------------------------------------------------------------
   Prepare the response packet.
  -------------------------------------------------------------------------*/
  rsp = (DIAG_POKEB_F_rsp_type *)diagpkt_alloc(DIAG_POKEB_F, rsp_len);

  if (rsp)
  {
    *rsp = *req;
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_pokeb.", 0, 0, 0);
  }

  return (rsp);

} /* diagdiag_pokeb */


/*===========================================================================

FUNCTION DIAGDIAG_POKEW

DESCRIPTION
  This procedure pokes a word into memory and formats a poke response
  message.

===========================================================================*/
PACKED void * diagdiag_pokew (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_POKEW_F_req_type *req = (DIAG_POKEW_F_req_type *)req_pkt;
  DIAG_POKEW_F_rsp_type *rsp;
  const int rsp_len = sizeof(DIAG_POKEW_F_rsp_type);
  int i;
  uint16 count;
  PACKED word *src = NULL;
  PACKED word *dest = NULL;

  word * dest_w; /* Unpacked word pointer */
  word   val_w;  /* Value to poke */
  
  count = req->length;
  src = &req->data[0];
  dest = req->ptr;

  /*----------------------------------------------------------------------
    Check length of request packet.  This is critical since the download
    protocol used by QPST uses HDLC encoded packets with the equivalent
    of command code 6.  Length check is the only thing that prevents
    accidental pokes!
  ----------------------------------------------------------------------*/
  if (pkt_len != sizeof (DIAG_POKEW_F_req_type))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
  ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len) );
  }

  /*----------------------------------------------------------------------
    Check block length against maximum allowable length.
  ----------------------------------------------------------------------*/
  if (count > DIAG_MAX_POKE_W) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }

#ifdef FEATURE_DIAG_HW_ADDR_CHECK
  /*----------------------------------------------------------------------
    Check to be sure the block is a valid block.
  ----------------------------------------------------------------------*/
  if (!hw_valid_addr ((void *) dest, (word)(count * sizeof (word)))) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }
#endif

#ifdef FEATURE_VOC_TASK
  /*---------------------------------------------------------------------------
    The vocoder memory space is a special case since the DSP's clocks are
    turned off during sleep, making the address invalid.  Let voc handle it.
  ----------------------------------------------------------------------------*/
  if (voc_valid_addr ((void *) dest, (uint16) (count * sizeof(word)) ))
  {
    voc_status_type eVocStatus;

    /* Since the data is byte ordered, no need to hanlde endian-ness here. */
    eVocStatus = voc_poke_cmd((void *) dest, (void *) src,
                              (uint16) (count * sizeof(word)) );

    if (eVocStatus != VOC_DONE_S)
    {
      return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
    }
  }
  else /* do it ourselves. */
#endif /* FEATURE_VOC_TASK */
  {

    /* Test for alignment. If destination address is word aligned, use
       non-packed word transfers. */
    if (((dword)dest & 0x01) == 0x00)
    {
      /* Copy packed pointer to non-packed pointer */
      dest_w = (word*)dest; 

      /*----------------------------------------------------------------------
        Lock out interrupts to preserve consistency of the memory block.
      ----------------------------------------------------------------------*/
      INTLOCK ();
      for (i = 0; i < count; i++) {
        val_w = *src;
        *dest_w = val_w;
        dest_w++;
        src++;
      }
      INTFREE ();
    }
    else  /*  Copy with packed variables */
    {
      /*----------------------------------------------------------------------
        Lock out interrupts to preserve consistency of the memory block.
      ----------------------------------------------------------------------*/
      INTLOCK ();
      for (i = 0; i < count; i++) {
        *dest = *src;
        dest++;
        src++;
      }
      INTFREE ();
    }
  }

  /*----------------------------------------------------------------------
    Prepare the response packet.
  ----------------------------------------------------------------------*/
  rsp = (DIAG_POKEW_F_rsp_type *)diagpkt_alloc(DIAG_POKEW_F, rsp_len);

  if (rsp)
  {
    *rsp = *req;
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_pokew.", 0, 0, 0);
  }

  return (rsp);

} /* diagdiag_pokew */



/*===========================================================================

FUNCTION DIAGDIAG_POKED

DESCRIPTION
  This procedure pokes a dword into memory and formats a poke response
  message.

===========================================================================*/
PACKED void * diagdiag_poked (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_POKED_F_req_type *req = (DIAG_POKED_F_req_type *)req_pkt;
  DIAG_POKED_F_rsp_type *rsp;
  const int rsp_len = sizeof(DIAG_POKED_F_rsp_type);
  int i;
  uint16 count;
  PACKED dword *src = NULL;
  PACKED dword *dest = NULL;

  dword * dest_dw; /* Unpacked dword pointer */
  dword   val_dw;  /* Value to poke */


  count = req->length;
  src = &req->data[0];
  dest = req->ptr;

  /*----------------------------------------------------------------------
    Check length of request packet.
  ----------------------------------------------------------------------*/
  if (pkt_len != sizeof (DIAG_POKED_F_req_type))
  {
    return (diagpkt_err_rsp (DIAG_BAD_LEN_F, req_pkt, pkt_len));
  }

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
  ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len) );
  }

  /*----------------------------------------------------------------------
    Check block length against maximum allowed length.
  ----------------------------------------------------------------------*/
  if (count > DIAG_MAX_POKE_D) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }

#ifdef FEATURE_DIAG_HW_ADDR_CHECK
  /*----------------------------------------------------------------------
    Check to see that requested block is a valid block.
  ----------------------------------------------------------------------*/
  if (!hw_valid_addr ((void *) dest, (word)(count * sizeof (dword))) ) {
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }
#endif

#ifdef FEATURE_VOC_TASK
  /*---------------------------------------------------------------------------
    The vocoder memory space is a special case since the DSP's clocks are
    turned off during sleep, making the address invalid.  Let voc handle it.
  ----------------------------------------------------------------------------*/
  if (voc_valid_addr ((void *) dest, (uint16) (count * sizeof(dword)) ))
  {
    voc_status_type eVocStatus;

    /* Since the data is byte ordered, no need to hanlde endian-ness here. */
    eVocStatus = voc_poke_cmd((void *) dest, (void *) src,
                              (uint16) (count * sizeof(dword)) );

    if (eVocStatus != VOC_DONE_S)
    {
      return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
    }
  }
  else /* do it ourselves. */
#endif /* FEATURE_VOC_TASK */
  {
    /* Test for alignment. If destination address is dword aligned, use
       non-packed dword transfers. */
    if (((dword)dest & 0x03) == 0x00)
    {
      /* Copy packed pointer to non-packed pointer */
      dest_dw = (dword*)dest; 

      /*----------------------------------------------------------------------
        Lock out interrupts to preserve consistency of the memory block.
      ----------------------------------------------------------------------*/
      INTLOCK ();
      for (i = 0; i < count; i++) {
        val_dw = *src;
        *dest_dw = val_dw;
        dest_dw++;
        src++;
      }
      INTFREE ();
    }
    else  /*  Copy with packed variables */
    {

      /*----------------------------------------------------------------------
        Lock out interrupts to preserve consistency of the memory block.
      ----------------------------------------------------------------------*/
      INTLOCK ();
      for (i = 0; i < count; i++) {
        *dest = *src;
        dest++;
        src++;
      }
      INTFREE ();
    }
  }

  /*-----------------------------------------------------------------------
   Prepare the response packet.
  -----------------------------------------------------------------------*/
  rsp = (DIAG_POKED_F_rsp_type *)diagpkt_alloc(DIAG_POKED_F, rsp_len);

  if (rsp)
  {
    *rsp = *req;
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_poked.", 0, 0, 0);
  }

  return (rsp);

} /* diagdiag_poked */



/*===========================================================================

FUNCTION DIAGDIAG_VER

DESCRIPTION
  Return the diag version

===========================================================================*/
PACKED void * diagdiag_ver (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_DIAG_VER_F_rsp_type *rsp;
  const int rsp_len = sizeof (DIAG_DIAG_VER_F_rsp_type);

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

  rsp = (DIAG_DIAG_VER_F_rsp_type *)diagpkt_alloc(DIAG_DIAG_VER_F, rsp_len);

  if(rsp)
  {
    /*-------------------------------------------------------------------------
      Return diag source code version
    -------------------------------------------------------------------------*/
    rsp->ver = DIAG_DIAGVER;
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_ver.", 0, 0, 0);
  }

  return (rsp);

} /* diagdiag_diag_ver */



/*===========================================================================

FUNCTION DIAGDIAG_FEATURE_QUERY

DESCRIPTION
  This procedure processes a request to query for features in the current
  build.  It returns a variable length bit mask where each bit corresponds to
  a particular feature defined in diagpkt.h in feature_query_enum_type.

============================================================================*/
PACKED void * diagdiag_feature_query (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_FEATURE_QUERY_F_rsp_type *rsp;
  const int rsp_len = sizeof(DIAG_FEATURE_QUERY_F_rsp_type);

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

  rsp =(DIAG_FEATURE_QUERY_F_rsp_type *)diagpkt_alloc(DIAG_FEATURE_QUERY_F, rsp_len);

  if(rsp)
  {
    rsp->feature_mask_size = FEATURE_MASK_LENGTH;

    /* Copy the current mask into the diag pkt */
    memcpy( (void *) rsp->feature_mask,
            diag_feature_mask,
            FEATURE_MASK_LENGTH
          );
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_feature_query.", 0, 0, 0);
  }
  /* return the size of the variable length packet.  command_code and
  ** feature_mask_size is constant.  FEATURE_MASK_LENGTH changes
  ** as more features are added
  */
  return (rsp);

} /* diagdiag_feature_query */



/*===========================================================================

FUNCTION DIAGDIAG_PASSWORD

DESCRIPTION
  Processes an entry of the Security Password.
  This function compares the password in the phone to what was sent in
  the packet.  If they are the same, the successful Security unlock is
  noted (setting the diag_get_security_state() mask to UNLOCKED), and a value of TRUE
  is returned to the external device.
  If they are NOT the same, Security remains locked, a value of FALSE is
  returned to the external device, and the phone is powered off,
  to deter hackers.

SIDE EFFECTS
  May cause the phone to power off!  (After returning from this routine).

===========================================================================*/
PACKED void * diagdiag_password (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_PASSWORD_F_req_type *req = (DIAG_PASSWORD_F_req_type *) req_pkt;
  DIAG_PASSWORD_F_rsp_type *rsp;
  const int rsp_len = sizeof(DIAG_PASSWORD_F_rsp_type);

  rsp = (DIAG_PASSWORD_F_rsp_type *) diagpkt_alloc (DIAG_PASSWORD_F, rsp_len);

  if(rsp == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_password.", 0, 0, 0);
    return(rsp);
  }
/*------------------------------------------------------------------------*/

  MSG_LOW ("Security Password given", 0, 0, 0);

  /* If the security is already unlocked, then it doesn't matter what
  ** you give as the security code.  Otherwise we need to compare
  ** the given security code with the one in the phone.
  */
  if ((diag_get_security_state() == DIAG_SEC_UNLOCKED) ||
      (diag_check_password((void *) req->password))) {

    rsp->password_ok = TRUE;
    (void) diag_set_security_state(DIAG_SEC_UNLOCKED);
  }
  /* Otherwise, the code was incorrect.  Diag will now powerdown the phone.
  */
  else {
    rsp->password_ok = FALSE;
    (void) diag_set_security_state(DIAG_SEC_LOCKED);

    diagpkt_commit(rsp);
    rsp = NULL;

    /* Flush DIAG's TX buffer */
    diagbuf_flush();

    /* Power down the phone.  This function does not return.
    */
    diag_powerdown();
  }

  return (rsp);

} /* diagdiag_password */



/*===========================================================================
FUNCTION DIAGDIAG_TS

DESCRIPTION
  Return an IS-95/IS-2000 timestamp

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * diagdiag_ts (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  qword ts_time;                 /* time stamp */
  DIAG_TS_F_rsp_type *rsp;
  const int rsp_len = sizeof( DIAG_TS_F_rsp_type );

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /*------------------------------------------------------------------------
    Get the time, stuff the packet.
  -------------------------------------------------------------------------*/

  ts_get (ts_time);

  rsp = (DIAG_TS_F_rsp_type *) diagpkt_alloc (DIAG_TS_F, rsp_len);

  if(rsp == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_ts.", 0, 0, 0);
  }
  else
  {
    qw_equ_misaligned (rsp->ts, ts_time);
  }
  
  return (rsp);

} /* diagdiag_ts */



#if defined (T_MSM3)
#define MSM_MEMMAP_IO_OFFSET 0x03000000
#else
#define MSM_MEMMAP_IO_OFFSET 0x00000000
#endif

/*===========================================================================

FUNCTION DIAGDIAG_OUTP

DESCRIPTION
  This procedure processes a received command to write a byte to a port.
  The byte is written to the input port and a response packet is built.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * diagdiag_outp (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_OUTP_F_rsp_type *rsp;
  const int rsp_len = sizeof( DIAG_OUTP_F_rsp_type );
  DIAG_OUTP_F_req_type *req = (DIAG_OUTP_F_req_type *) req_pkt;

  /*----------------------------------------------------------------------
    Check security, since this is a secure function
  ----------------------------------------------------------------------*/
  if (diag_get_security_state() == DIAG_SEC_LOCKED) {
    rsp = (DIAG_OUTP_F_rsp_type *)diagpkt_err_rsp( DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len );
    return rsp;
  }

  /*----------------------------------------------------------------------
    Write byte to port
  ----------------------------------------------------------------------*/
  (void) outp( req->port + MSM_MEMMAP_IO_OFFSET, req->data );

  /*-----------------------------------------------------------------------
    Prepare the response packet.
  -----------------------------------------------------------------------*/
  /* Allocate space for rsponse packet */
  rsp = (DIAG_OUTP_F_rsp_type *)diagpkt_alloc( DIAG_OUTP_F, rsp_len );

  if (rsp)
  {
    rsp->port = req->port;
    rsp->data = req->data;
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_outp.", 0, 0, 0);
  }

  return (rsp);

} /* diagdiag_outp */



/*===========================================================================

FUNCTION DIAGDIAG_OUTPW

DESCRIPTION
  This procedure processes a received command to write a word to a port.
  The word is written to the input port and a response packet is built.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * diagdiag_outpw (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_OUTPW_F_rsp_type *rsp;
  const int rsp_len = sizeof( DIAG_OUTPW_F_rsp_type );
  DIAG_OUTPW_F_req_type *req = (DIAG_OUTPW_F_req_type *) req_pkt;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
  ----------------------------------------------------------------------*/
  if (diag_get_security_state() == DIAG_SEC_LOCKED) {
    rsp = (DIAG_OUTPW_F_rsp_type *)diagpkt_err_rsp( DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len );
    return rsp;
  }

  /*----------------------------------------------------------------------
    Write word to port
  ----------------------------------------------------------------------*/
  (void) outpw( req->port+ MSM_MEMMAP_IO_OFFSET, req->data );

  /*-----------------------------------------------------------------------
    Prepare the response packet.
  -----------------------------------------------------------------------*/
  /* Allocate space for rsponse packet */
  rsp = (DIAG_OUTPW_F_rsp_type *)diagpkt_alloc( DIAG_OUTPW_F, rsp_len );

  if (rsp)
  {
    rsp->port = req->port;
    rsp->data = req->data;
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_outpw.", 0, 0, 0);
  }

  return (rsp);

} /* diagdiag_outpw */



/*===========================================================================

FUNCTION DIAGDIAG_INP

DESCRIPTION
  This procedure processes a received command to read a byte from a port.
  The byte is read from the port and a response packet is built.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * diagdiag_inp (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_INP_F_rsp_type *rsp;
  const int rsp_len = sizeof( DIAG_INP_F_rsp_type );
  DIAG_INP_F_req_type *req = (DIAG_INP_F_req_type *) req_pkt;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
  ----------------------------------------------------------------------*/
  if (diag_get_security_state() == DIAG_SEC_LOCKED) {
    rsp = ( DIAG_INP_F_rsp_type *)diagpkt_err_rsp( DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len );
    return rsp;
  }

  /* Allocate space for rsponse packet */
  rsp = (DIAG_INP_F_rsp_type *)diagpkt_alloc( DIAG_INP_F, rsp_len );

  if(rsp)
  {
    /*------------------------------------------------------------------------
     Fill in the boilerplate for the response packet, then read the byte in
     from the port.
    ------------------------------------------------------------------------*/
    rsp->port = req->port;
    rsp->data = (byte) inp( req->port + MSM_MEMMAP_IO_OFFSET );
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_inp.", 0, 0, 0);
  }

  return (rsp);

} /* diagdiag_inp */



/*===========================================================================

FUNCTION DIAGDIAG_INPW

DESCRIPTION
  This procedure processes a received command to read a word from a port.
  The word is read from the port and a response packet is built.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * diagdiag_inpw (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_INPW_F_rsp_type *rsp;
  const int rsp_len = sizeof( DIAG_INPW_F_rsp_type );
  DIAG_INPW_F_req_type *req = (DIAG_INPW_F_req_type *) req_pkt;

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
  ----------------------------------------------------------------------*/
  if (diag_get_security_state() == DIAG_SEC_LOCKED) {
    rsp = (DIAG_INPW_F_rsp_type *)diagpkt_err_rsp( DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len );
    return rsp;
  }

  /* Allocate space for rsponse packet */
  rsp = (DIAG_INPW_F_rsp_type *)diagpkt_alloc( DIAG_INPW_F, rsp_len );

  if(rsp)
  {
    /*------------------------------------------------------------------------
      Fill in the boilerplate for the response, then input the word directly
      into the response packet.
    -------------------------------------------------------------------------*/
    rsp->port = req->port;

    rsp->data = inpw( req->port + MSM_MEMMAP_IO_OFFSET );
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_inpw.", 0, 0, 0);
  }

  return (rsp);

} /* diagdiag_inpw */



/*===========================================================================

FUNCTION DIAGDIAG_GET_DIPSW

DESCRIPTION
  Processes a request to retrieve current dip switch settings.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * diagdiag_get_dipsw (
  PACKED void *req_ptr,      /* pointer to request packet  */
  word pkt_len                 /* length of request packet   */
)
{
  diag_dipsw_rsp_type *rsp_ptr;
  const int rsp_len = sizeof(diag_dipsw_rsp_type);

  (void) req_ptr;   /* suppress compiler warning */
  (void) pkt_len;   /* suppress compiler warning */

  rsp_ptr = (diag_dipsw_rsp_type *)diagpkt_alloc(DIAG_GET_DIPSW_F, rsp_len);

  if(rsp_ptr)
  {
    rsp_ptr->switches =
        BIO_GET_SW ( BIO_SW1_M | BIO_SW2_M | BIO_SW3_M | BIO_SW4_M |
                     BIO_SW5_M | BIO_SW6_M | BIO_SW7_M | BIO_SW8_M |
                     BIO_SW9_M | BIO_SW10_M | BIO_SW11_M | BIO_SW12_M |
                     BIO_SW13_M | BIO_SW14_M | BIO_SW15_M | BIO_SW16_M );
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_get_dipsw.", 0, 0, 0);
  }
  return (rsp_ptr);
} /* diagdiag_get_dipsw */



/*===========================================================================

FUNCTION DIAGDIAG_SET_DIPSW

DESCRIPTION
  Processes a request to set dip switch settings.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
===========================================================================*/
PACKED void * diagdiag_set_dipsw (
  PACKED void *req_pkt,   /* pointer to request packet  */
  word pkt_len               /* length of request packet   */
)
{
  diag_dipsw_req_type *req = (diag_dipsw_req_type *) req_pkt;
  diag_dipsw_rsp_type *rsp;
  const int rsp_len = sizeof(diag_dipsw_rsp_type);

  (void) pkt_len; /* suppress compiler warning */

  bio_set_sw ( ( bio_sw_type ) req->switches );

  rsp = (diag_dipsw_rsp_type *)diagpkt_alloc(DIAG_SET_DIPSW_F, rsp_len);

  if(rsp)
  {
    rsp->switches =
        BIO_GET_SW ( BIO_SW1_M | BIO_SW2_M | BIO_SW3_M | BIO_SW4_M |
                     BIO_SW5_M | BIO_SW6_M | BIO_SW7_M | BIO_SW8_M |
                     BIO_SW9_M | BIO_SW10_M | BIO_SW11_M | BIO_SW12_M |
                     BIO_SW13_M | BIO_SW14_M | BIO_SW15_M | BIO_SW16_M );
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_set_dipsw.", 0, 0, 0);
  }

  return (rsp);
} /* diagdiag_set_dipsw */


#if 1 //For now, use a fake GUID. -LD
/* Default GUID (since it was unspecified) */
static const diag_guid_type diag_guid =
{
   0xDEADD00DU,
   0xDEADD00DU,
   0xDEADD00DU,
   0x06101975
};
#endif


LOCAL diag_prop_struct_type diag_prop_table[DIAG_MAX_NUM_PROPS];

/*===========================================================================

FUNCTION DIAG_LOOKUP_PROP

DESCRIPTION
  Looks up the address of a callback function given its name.

DEPENDENCIES
  None.

RETURN VALUE
  The address of the function or NULL if the function is not found in the
  function table.

SIDE EFFECTS
  None.

===========================================================================*/
void *diag_lookup_prop
(
  char *prop_name
)
{
  int i = 0;

  if ( prop_name == NULL )
  {
    return (void *) NULL;
  }

  while ( diag_prop_table[ i ].name != NULL )
  {
    if ( strncmp(
                 diag_prop_table[ i ].name,
                 prop_name,
                 DIAG_MAX_PROPERTY_NAME_SIZE
         ) == 0 )
    {
      return diag_prop_table[ i ].address;
    }
    i++;
    if ( i >= DIAG_MAX_NUM_PROPS )
    {
      /*
      ** Will get here only if the properties table has been corrupted.
      */
      break;
    }
  }
  return (void *) NULL;
} /* diag_lookup_prop */



/*===========================================================================

FUNCTION DIAG_GUID_IS_VALID

DESCRIPTION
  This static function returns true or false depending on whether the
  input guid (globally unique identifier) matches that of the current build.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the input guid is valid, FALSE otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
LOCAL boolean diag_guid_is_valid
(
  diag_guid_type guid
)
{
  if ( ( guid[ 0 ] == diag_guid[ 0 ] ) &&
       ( guid[ 1 ] == diag_guid[ 1 ] ) &&
       ( guid[ 2 ] == diag_guid[ 2 ] ) &&
       ( guid[ 3 ] == diag_guid[ 3 ] ) )
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
} /* diag_guid_is_valid */


/*===========================================================================

FUNCTION DIAGDIAG_GET_PROPERTY

DESCRIPTION
  This procedure processes a get_property request. The request packet
  specifies an address in memory and a number of bytes. The specifed number
  of bytes are retrieved starting at the specified address and returned in
  the response packet.

DEPENDENCIES
  None.

RETURN VALUE
  The length of the response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * diagdiag_get_property
(
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  DIAG_GET_PROPERTY_F_req_type *req = (DIAG_GET_PROPERTY_F_req_type *) req_pkt;
  DIAG_GET_PROPERTY_F_rsp_type *rsp;
  dword rsp_len = FPOS(DIAG_GET_PROPERTY_F_rsp_type, data );

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
  ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len) );
  }

#ifdef FEATURE_DIAG_HW_ADDR_CHECK
  /*-------------------------------------------------------------------------
    Check to see if block requested is within a valid range
  --------------------------------------------------------------------------*/
  if (!hw_valid_addr ((void *) &req->guid[0], sizeof(diag_guid_type)) ) {
    MSG_ERROR("Invalid Address",0,0,0);
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }
#endif

/*----------------------------------------------------------------------------
  Allocate buffer space for response packet.
----------------------------------------------------------------------------*/
  rsp = (DIAG_GET_PROPERTY_F_rsp_type * )
    diagpkt_alloc(DIAG_GET_PROPERTY_F, MAX(rsp_len + 1, rsp_len + req->size));

  if(rsp == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_get_property.", 0, 0, 0);
    return(rsp);
  }
/*----------------------------------------------------------------------------
  Fill in some of the fields in the response packet.
----------------------------------------------------------------------------*/
  memcpy((void *) &rsp->guid[0],
         (const void *) &req->guid[0],
         sizeof(diag_guid_type));

  rsp->address  = req->address;

/*---------------------------------------------------------------------------
  Check for valid GUID.
----------------------------------------------------------------------------*/
  if ( !diag_guid_is_valid( req->guid ) ) {
    rsp->size    = 0;
    rsp->data[0] = DIAG_EXTN_INVALID_GUID;

    /* Adjust length and return */
    diagbuf_shorten(rsp, rsp_len + 1);
    return rsp;
  }

/*---------------------------------------------------------------------------
  Check property size against maximum allowed size.
----------------------------------------------------------------------------*/
  if ( req->size > DIAG_MAX_PROPERTY_SIZE ) {
    rsp->size     = 0;
    rsp->data[0]  = DIAG_EXTN_INVALID_SIZE;

    /* Adjust length and return */
    diagbuf_shorten(rsp, rsp_len + 1);
    return rsp;
  }

  rsp->size = req->size;

/*---------------------------------------------------------------------------
  Move the memory bytes into the response buffer.  Lock out interrupts to
  preserve consistency of the memory block.
----------------------------------------------------------------------------*/
  INTLOCK( );
  memcpy((void *) rsp->data,
         (const void *) req->address,
         req->size);
  INTFREE( );

  /* Adjust length and return */
  diagbuf_shorten(rsp, rsp_len + req->size);
  return rsp;

} /* diagdiag_get_property */



/*===========================================================================

FUNCTION DIAGDIAG_PUT_PROPERTY

DESCRIPTION
  This procedure processes a put_property request. The values specified in
  the request packet are written into memory starting at the specified
  address.

DEPENDENCIES
  None.

RETURN VALUE
  The length of the response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * diagdiag_put_property
(
  PACKED void *req_pkt, /* pointer to request packet  */
  uint16 pkt_len            /* length of request packet  */
)
{
  DIAG_PUT_PROPERTY_F_req_type *req = (DIAG_PUT_PROPERTY_F_req_type *) req_pkt;
  DIAG_PUT_PROPERTY_F_rsp_type *rsp;
  dword rsp_len = FPOS(DIAG_PUT_PROPERTY_F_rsp_type, data );

  /*----------------------------------------------------------------------
    Check security, since this is a secure funciton
  ----------------------------------------------------------------------*/
  if (diag_get_security_state() != DIAG_SEC_UNLOCKED) {
    return( diagpkt_err_rsp(DIAG_BAD_SEC_MODE_F, req_pkt, pkt_len) );
  }

#ifdef FEATURE_DIAG_HW_ADDR_CHECK
  /*-------------------------------------------------------------------------
    Check to see if block requested is within a valid range
  --------------------------------------------------------------------------*/
  if (!hw_valid_addr ((void *) &req->guid[0], sizeof(diag_guid_type)) ) {
    MSG_ERROR("Invalid Address",0,0,0);
    return( diagpkt_err_rsp(DIAG_BAD_PARM_F, req_pkt, pkt_len) );
  }
#endif

/*----------------------------------------------------------------------------
  Allocate buffer space for response packet.
----------------------------------------------------------------------------*/
  rsp = (DIAG_PUT_PROPERTY_F_rsp_type *) diagpkt_alloc (DIAG_PUT_PROPERTY_F,
                                                        rsp_len + 1);

  if(rsp == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_put_property.", 0, 0, 0);
    return(rsp);
  }
/*----------------------------------------------------------------------------
  Fill in some of the fields in the response packet.
----------------------------------------------------------------------------*/
  memcpy((void *) &rsp->guid[0],
         (const void *) &req->guid[0],
         sizeof(diag_guid_type));

  rsp->address = req->address;

/*---------------------------------------------------------------------------
  Check for valid GUID.
----------------------------------------------------------------------------*/
  if ( !diag_guid_is_valid( req->guid ) ) {
    rsp->size    = 0;
    rsp->data[0] = DIAG_EXTN_INVALID_GUID;

    return rsp;
  }

/*---------------------------------------------------------------------------
  Check property size against maximum allowed size.
----------------------------------------------------------------------------*/
  if ( req->size > DIAG_MAX_PROPERTY_SIZE ) {
    rsp->size    = 0;
    rsp->data[0] = DIAG_EXTN_INVALID_SIZE;

    return rsp;
  }

  rsp->size = req->size;

  /*------------------------------------------------------------------------
    Transfer the bytes from the request packet into memory.  Lock out
    interrupts to ensure consistency of the write.
  -------------------------------------------------------------------------*/
  INTLOCK( );
  memcpy((void *) req->address,
         (const void *) req->data,
         req->size);
  INTFREE( );

  /* Adjust length and return */
  diagbuf_shorten(rsp, rsp_len);
  return rsp;

} /* diagdiag_put_property */



/*===========================================================================

FUNCTION DIAGDIAG_GET_GUID

DESCRIPTION
  This procedure processes a get_guid request. The GUID (globally unique
  identifier) for this build is retrieved and returned in the response
  packet. The GUID is stored during the build process.

===========================================================================*/
PACKED void * diagdiag_get_guid
(
  PACKED void *req_pkt, /* pointer to request packet  */
  uint16 pkt_len            /* length of request packet  */
)
{
  DIAG_GET_GUID_F_rsp_type *rsp;
  const unsigned int rsp_len = sizeof (DIAG_GET_GUID_F_rsp_type);

  (void) req_pkt; /* suppress compiler warning */
  (void) pkt_len; /* suppress compiler warning */

  /*--------------------------------------------
    Allocate buffer space for response packet.
  --------------------------------------------*/
  rsp = (DIAG_GET_GUID_F_rsp_type *) diagpkt_alloc (DIAG_GET_GUID_F, rsp_len);

  if(rsp)
  {
    memcpy((void *) &rsp->guid[0],
           (const void *) &diag_guid[0],
           sizeof(diag_guid_type));
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_get_guid.", 0, 0, 0);
  }

  return rsp;

} /* diagdiag_get_guid */



/*===========================================================================

FUNCTION DIAGDIAG_GET_PERM_PROPERTY

DESCRIPTION
  This procedure processes a get_perm_property request. The contents of
  the structure specified by name in the request packet are retrieved.

DEPENDENCIES
  None.

RETURN VALUE
  The length of the response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * diagdiag_get_perm_property
(
  PACKED void *req_pkt, /* pointer to request packet  */
  uint16 pkt_len            /* length of request packet  */
)
{
  void *prop_addr;    /* address of the specified property */
  DIAG_GET_PERM_PROPERTY_F_req_type *req =
    (DIAG_GET_PERM_PROPERTY_F_req_type *) req_pkt;
  DIAG_GET_PERM_PROPERTY_F_rsp_type *rsp;
  const unsigned int rsp_len = sizeof( DIAG_GET_PERM_PROPERTY_F_rsp_type );

/*----------------------------------------------------------------------------
  Allocate buffer space for response packet.
----------------------------------------------------------------------------*/
  rsp = (DIAG_GET_PERM_PROPERTY_F_rsp_type *)
    diagpkt_alloc (DIAG_GET_PERM_PROPERTY_F, rsp_len);

  if(rsp == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_get_perm_property.", 0, 0, 0);
    return(rsp);
  }
/*----------------------------------------------------------------------------
  Fill in some of the fields in the response packet.
----------------------------------------------------------------------------*/
  memcpy((void *) &rsp->guid[0],
         (const void *) &req->guid[0],
         sizeof(diag_guid_type));

  memcpy((void *) &rsp->name[0],
         (const void *) &req->name[0],
         sizeof(req->name));

/*---------------------------------------------------------------------------
  Check for valid GUID.
----------------------------------------------------------------------------*/
  if ( !diag_guid_is_valid( req->guid ) ) {
    rsp->size    = 0;
    rsp->data[0] = DIAG_EXTN_INVALID_GUID;
    return rsp;
  }

/*---------------------------------------------------------------------------
  Check data size against maximum allowed size.
----------------------------------------------------------------------------*/
  if ( req->size > DIAG_MAX_PROPERTY_SIZE ) {
    rsp->size    = 0;
    rsp->data[0] = DIAG_EXTN_INVALID_SIZE;
    return rsp;
  }

/*---------------------------------------------------------------------------
  Look up the address of the specified property. Return with an error status
  if we do not get a valid address. If we get a valid address, move the
  memory bytes into the response buffer.  Lock out interrupts to preserve
  consistency of the memory block.
----------------------------------------------------------------------------*/
  prop_addr = diag_lookup_prop( (char *) req->name );

  if ( prop_addr == NULL ) {
    rsp->size    = 0;
    rsp->data[0] = DIAG_EXTN_INVALID_NAME;
    return rsp;
  }

  rsp->size = req->size;

  INTLOCK( );
  memcpy((void *) &rsp->data[0],
         (const void *) prop_addr,
         req->size * sizeof(byte));
  INTFREE( );

  return rsp;

} /* diagdiag_get_perm_property */



/*===========================================================================

FUNCTION DIAGDIAG_PUT_PERM_PROPERTY

DESCRIPTION
  This procedure processes a put_perm_property request. The structure
  specified by name in the request packet is filled in using the values
  specified in the packet.

DEPENDENCIES
  None.

RETURN VALUE
  The length of the response packet.

SIDE EFFECTS
  None.

===========================================================================*/
PACKED void * diagdiag_put_perm_property
(
  PACKED void *req_pkt, /* pointer to request packet  */
  uint16 pkt_len            /* length of request packet  */
)
{
  void *prop_addr;    /* address of the specified property */
  DIAG_PUT_PERM_PROPERTY_F_req_type *req =
    (DIAG_PUT_PERM_PROPERTY_F_req_type *) req_pkt;
  DIAG_PUT_PERM_PROPERTY_F_rsp_type *rsp;
  const unsigned int rsp_len = sizeof( DIAG_PUT_PERM_PROPERTY_F_rsp_type );

/*----------------------------------------------------------------------------
  Allocate buffer space for response packet.
----------------------------------------------------------------------------*/
  rsp = (DIAG_PUT_PERM_PROPERTY_F_rsp_type *)
    diagpkt_alloc (DIAG_PUT_PERM_PROPERTY_F, rsp_len);

  if(rsp == NULL)
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_put_perm_property.", 0, 0, 0);
    return(rsp);
  }
/*----------------------------------------------------------------------------
  Fill in some of the fields in the response packet.
----------------------------------------------------------------------------*/
  memcpy((void *) &rsp->guid[0],
         (const void *) &req->guid[0],
         sizeof(diag_guid_type));

  memcpy((void *) &rsp->name[0],
         (const void *) &req->name[0],
         sizeof(req->name));

/*---------------------------------------------------------------------------
  Check for valid GUID.
----------------------------------------------------------------------------*/
  if ( !diag_guid_is_valid( req->guid ) ) {
    rsp->size    = 0;
    rsp->data[0] = DIAG_EXTN_INVALID_GUID;
    return rsp;
  }

/*---------------------------------------------------------------------------
  Check data size against maximum allowed size.
----------------------------------------------------------------------------*/
  if ( req->size > DIAG_MAX_PROPERTY_SIZE ) {
    rsp->size    = 0;
    rsp->data[0] = DIAG_EXTN_INVALID_SIZE;
    return rsp;
  }

/*---------------------------------------------------------------------------
  Look up the address of the specified property. Return with an error status
  if we do not get a valid address. If we get a valid address, write bytes
  into memory.  Lock out interrupts to preserve consistency of the memory
  block.
----------------------------------------------------------------------------*/
  prop_addr = diag_lookup_prop( (char *) req->name );

  if ( prop_addr == NULL ) {
    rsp->size    = 0;
    rsp->data[0] = DIAG_EXTN_INVALID_NAME;
    return rsp;
  }

  rsp->size = req->size;

  INTLOCK( );
  memcpy(prop_addr,
         (const void *) &rsp->data[0],
         req->size * sizeof(byte));
  INTFREE( );

  return rsp;

} /* diagdiag_put_perm_property */





/*===========================================================================

FUNCTION DIAGDIAG_LOOPBACK

DESCRIPTION
  This procedure echos the request in the response.

RETURN VALUE
  Pointer to response packet.

============================================================================*/
PACKED void * diagdiag_loopback (
  PACKED void *req_pkt,
  uint16 pkt_len
)
{
  PACKED void *rsp = NULL;

  /* Allocate the same length as the request. */
  rsp = diagpkt_alloc (DIAG_PROTOCOL_LOOPBACK_F, pkt_len);

  if (rsp != NULL) {
    memcpy ((void *) rsp, (void *) req_pkt, pkt_len);
  }
  else
  {
    MSG_HIGH ("Diagpkt_alloc returned NULL in diagdiag_loopback.", 0, 0, 0);
  }

  return (rsp);
} /* diagdiag_loopback */

#ifdef DEBUG_DIAG_TEST
#error code not present
#endif /* DEBUG_DIAG_TEST  */



/* Tell lint that we are OK with the fact that pkt_len and req are not
** used in some of the packets, as this is dictated by the awesome design */
/*lint +esym(715,pkt_len,req) */

/* Define and register the dispatch table for common diagnostic packets */
#define DISPATCH_DECLARE(func) \
extern PACKED void * func(PACKED void *req_pkt, word len)

DISPATCH_DECLARE (err_read_diagpkt);
DISPATCH_DECLARE (err_clear_diagpkt);

DISPATCH_DECLARE (event_inbound_pkt);

DISPATCH_DECLARE (log_process_legacy_logmask);
DISPATCH_DECLARE (log_process_ext_logmask_pkt);
DISPATCH_DECLARE (log_process_config_pkt);
DISPATCH_DECLARE (log_on_demand_pkt);

#if defined(FEATURE_DIAG_NON_STREAMING)
DISPATCH_DECLARE (log_get_pkt);
#endif

DISPATCH_DECLARE (msg_pkt_process_config);
DISPATCH_DECLARE (msg_pkt_legacy_req);

#if defined(FEATURE_DIAG_RPC)
#error code not present
#endif /* FEATURE_WCDMA_PLT */

DISPATCH_DECLARE (event_mask_get_handler);
DISPATCH_DECLARE (event_mask_set_handler);

static const diagpkt_user_table_entry_type diagdiag_tbl[] =
{
  /* Place these at the top of the list for faster search. */
  {DIAG_MSG_F, DIAG_MSG_F, msg_pkt_legacy_req},

#if defined(FEATURE_DIAG_NON_STREAMING)
  {DIAG_LOG_F, DIAG_LOG_F, log_get_pkt},
#endif

  {DIAG_TS_F, DIAG_TS_F, diagdiag_ts},
  {DIAG_DIAG_VER_F, DIAG_DIAG_VER_F, diagdiag_ver},

#if defined(FEATURE_DIAG_RPC)
#error code not present
#endif

  /* Common diagostic packets */
  {DIAG_EXT_MSG_CONFIG_F, DIAG_EXT_MSG_CONFIG_F, msg_pkt_process_config},
  {DIAG_LOG_CONFIG_F, DIAG_LOG_CONFIG_F, log_process_config_pkt},
  {DIAG_LOG_ON_DEMAND_F, DIAG_LOG_ON_DEMAND_F, log_on_demand_pkt},
  {DIAG_EXT_LOGMASK_F, DIAG_EXT_LOGMASK_F, log_process_ext_logmask_pkt},
  {DIAG_LOGMASK_F, DIAG_LOGMASK_F, log_process_legacy_logmask},
  {DIAG_EVENT_REPORT_F, DIAG_EVENT_REPORT_F, event_inbound_pkt},
#ifndef FEATURE_DIAG_DISALLOW_MEM_OPS
  {DIAG_PEEKB_F, DIAG_PEEKB_F, diagdiag_peekb},
  {DIAG_PEEKW_F, DIAG_PEEKW_F, diagdiag_peekw},
  {DIAG_PEEKD_F, DIAG_PEEKD_F, diagdiag_peekd},
  {DIAG_POKEB_F, DIAG_POKEB_F, diagdiag_pokeb},
  {DIAG_POKEW_F, DIAG_POKEW_F, diagdiag_pokew},
  {DIAG_POKED_F, DIAG_POKED_F, diagdiag_poked},
  {DIAG_OUTP_F, DIAG_OUTP_F, diagdiag_outp},
  {DIAG_OUTPW_F, DIAG_OUTPW_F, diagdiag_outpw},
  {DIAG_INP_F, DIAG_INP_F, diagdiag_inp},
  {DIAG_INPW_F, DIAG_INPW_F, diagdiag_inpw},
#endif
  {DIAG_PASSWORD_F, DIAG_PASSWORD_F, diagdiag_password},
  {DIAG_FEATURE_QUERY_F, DIAG_FEATURE_QUERY_F, diagdiag_feature_query},
  {DIAG_PROTOCOL_LOOPBACK_F, DIAG_PROTOCOL_LOOPBACK_F, diagdiag_loopback},
  {DIAG_ERR_READ_F, DIAG_ERR_READ_F, err_read_diagpkt},
  {DIAG_ERR_CLEAR_F, DIAG_ERR_CLEAR_F, err_clear_diagpkt},
  {DIAG_GET_DIPSW_F, DIAG_GET_DIPSW_F, diagdiag_get_dipsw},
  {DIAG_SET_DIPSW_F, DIAG_SET_DIPSW_F, diagdiag_set_dipsw},

  {DIAG_GET_PROPERTY_F, DIAG_GET_PROPERTY_F, diagdiag_get_property},
  {DIAG_PUT_PROPERTY_F, DIAG_PUT_PROPERTY_F, diagdiag_put_property},
  {DIAG_GET_GUID_F, DIAG_GET_GUID_F, diagdiag_get_guid },
  {DIAG_GET_PERM_PROPERTY_F, DIAG_GET_PERM_PROPERTY_F, diagdiag_get_perm_property},
  {DIAG_PUT_PERM_PROPERTY_F, DIAG_PUT_PERM_PROPERTY_F, diagdiag_put_perm_property},
  {DIAG_EVENT_MASK_GET_F, DIAG_EVENT_MASK_GET_F, event_mask_get_handler },
  {DIAG_EVENT_MASK_SET_F, DIAG_EVENT_MASK_SET_F, event_mask_set_handler },
};

#ifdef DEBUG_DIAG_TEST
#error code not present
#endif

#ifdef __cplusplus

  DIAGPKT_DISPATCH_AUTOREGISTER (DIAGPKT_NO_SUBSYS_ID, diagdiag_tbl);
#ifdef DEBUG_DIAG_TEST
#error code not present
#endif

#else

  void diagdiag_init (void)
  {
    DIAGPKT_DISPATCH_TABLE_REGISTER (DIAGPKT_NO_SUBSYS_ID, diagdiag_tbl);
#ifdef DEBUG_DIAG_TEST
#error code not present
#endif
  }

#endif

#if defined __cplusplus
  }
#endif
