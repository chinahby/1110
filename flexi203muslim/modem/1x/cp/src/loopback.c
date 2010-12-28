/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             L O O P B A C K    S E R V I C E S    F I L E

GENERAL DESCRIPTION
  This module contains functions and variables to support Mobile Station
  Loopback Service Option processing as specified in PN-3291.

EXTERNALIZED FUNCTIONS
  loopback_rx
    Buffers loopback data received on the forward link.

  loopback_tx
    Formats the transmit data according to the receive data rate and
    the rate limit imposed by the MUX1 layer.

  loopback_init
    Sets the loopback receive data buffer to a known state.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Prior to using the other Loopback service option services, function
  loopback_init should be called to initialize the loopback rx data buffer.

Copyright (c) 1994-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/loopback.c_v   1.1   03 Oct 2002 10:51:06   phosabet  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/loopback.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/02   ph      Lint Cleanup
08/14/95   gb      Added new Loopback_13K service option support.
03/16/94   jca     Created module.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "bit.h"
#include "loopback.h"
#include "cai.h"
#include "err.h"

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains local definitions for constants, macros, types,
variables and other items needed by this module.

===========================================================================*/

#define LOOPBACK_BUF_SIZE 2

typedef struct
{
  byte               buf_ptr;
  loopback_data_type buf[LOOPBACK_BUF_SIZE];
} loopback_data_buffer_type;

loopback_data_buffer_type loopback_data;

int    lb_full_rate_bits;
int    lb_half_rate_bits;
int    lb_quarter_rate_bits;
int    lb_eighth_rate_bits;

/* <EJECT> */
/*===========================================================================

FUNCTION LOOPBACK_RX

DESCRIPTION
  Buffers Loopback Service Option data received on the forward link
  until the Transmit Task can loop the data back to the base station
  on the reverse link.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void loopback_rx
(
  loopback_data_type rx_data
    /* Received packet data for MS Loopback service option */
)
{
  word sav_var;
    /* Word to use for INTLOCK_SAV/INTFREE_SAV macros */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  INTLOCK_SAV( sav_var );

  loopback_data.buf_ptr = (loopback_data.buf_ptr + 1) % LOOPBACK_BUF_SIZE;
  loopback_data.buf[loopback_data.buf_ptr] = rx_data;

  INTFREE_SAV( sav_var );

} /* end loopback_rx() */

/*===========================================================================

FUNCTION LOOPBACK_TX

DESCRIPTION
  Formats the transmit data according to the receive data rate and
  the rate limit imposed by the MUX1 layer.

DEPENDENCIES
  None

RETURN VALUE
  The transmit data rate to use is returned.

SIDE EFFECTS
  None

===========================================================================*/

cai_data_rate_type loopback_tx
(
  cai_data_rate_type max_rate,
    /* Max rate for this frame */
  cai_frame_type    *frame_data
    /* Pointer to loopback data bits */
)
{
  word sav_var;
    /* Word to use for INTLOCK_SAV/INTFREE_SAV macros */
  loopback_data_type rx_data_item;
    /* Buffer for Loopback service option rx packet data */
  cai_data_rate_type tx_data_rate;
    /* To return the transmit data rate to use */

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  /* Initialize tx_data_rate to avoid compiler warning */
  tx_data_rate = CAI_FULL_RATE;

  INTLOCK_SAV( sav_var );

  rx_data_item = loopback_data.buf[loopback_data.buf_ptr];

  INTFREE_SAV( sav_var );

  if (max_rate == CAI_FULL_RATE)
  {
    /* ----------------------------------------------------------
    ** Service Option 2 has not been commanded to adapt its rates
    ** ---------------------------------------------------------- */

    switch (rx_data_item.frame_category)
    {
      case CAI_MUX2_FOR_1_ID:
      case CAI_MUX1_FOR_1_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '00' and the data bits to
        ** the received data bits.  The resulting information bits
        ** shall be supplied to the multiplex sublayer as a Rate 1 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '00' */
        *frame_data[0] = 0;

        /* Copy the received data bits into transmit frame */
        b_copy( rx_data_item.data_bits, 2, (byte *) frame_data, 2,
                (lb_full_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_FULL_RATE;
        break;

      case CAI_MUX2_FOR_2_ID:
      case CAI_MUX2_FOR_6_ID:
      case CAI_MUX2_FOR_11_ID:

      case CAI_MUX1_FOR_2_ID:
      case CAI_MUX1_FOR_6_ID:
      case CAI_MUX1_FOR_11_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1/2, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '00' and the data bits to
        ** the rxed data bits.  The resulting information bits shall
        ** be supplied to the multiplex sublayer as a Rate 1/2 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '00' */
        *frame_data[0] = 0;

        /* Copy the received data bits into transmit frame */
        b_copy( rx_data_item.data_bits, 2, (byte *) frame_data, 2,
                (lb_half_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_HALF_RATE;
        break;

      case CAI_MUX2_FOR_3_ID:
      case CAI_MUX2_FOR_7_ID:
      case CAI_MUX2_FOR_12_ID:
      case CAI_MUX2_FOR_15_ID:
      case CAI_MUX2_FOR_19_ID:

      case CAI_MUX1_FOR_3_ID:
      case CAI_MUX1_FOR_7_ID:
      case CAI_MUX1_FOR_12_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1/4, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '00' and the data bits to
        ** the rxed data bits.  The resulting information bits shall
        ** be supplied to the multiplex sublayer as a Rate 1/4 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '00' */
        *frame_data[0] = 0;

        /* Copy the received data bits into transmit frame */
        b_copy( rx_data_item.data_bits, 2, (byte *) frame_data, 2,
                (lb_quarter_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_QUARTER_RATE;
        break;

      case CAI_MUX2_FOR_4_ID:
      case CAI_MUX2_FOR_8_ID:
      case CAI_MUX2_FOR_10_ID:
      case CAI_MUX2_FOR_13_ID:
      case CAI_MUX2_FOR_16_ID:
      case CAI_MUX2_FOR_18_ID:
      case CAI_MUX2_FOR_20_ID:
      case CAI_MUX2_FOR_22_ID:
      case CAI_MUX2_FOR_24_ID:

      case CAI_MUX1_FOR_4_ID:
      case CAI_MUX1_FOR_8_ID:
      case CAI_MUX1_FOR_13_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1/8, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '00' and the data bits to
        ** the rxed data bits.  The resulting information bits shall
        ** be supplied to the multiplex sublayer as a Rate 1/8 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '00' */
        *frame_data[0] = 0;

        /* Copy the received data bits into transmit frame */
        b_copy( rx_data_item.data_bits, 2, (byte *) frame_data, 2,
                (lb_eighth_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_EIGHTH_RATE;
        break;

      case CAI_MUX2_FOR_5_ID:
      case CAI_MUX2_FOR_9_ID:
      case CAI_MUX2_FOR_14_ID:
      case CAI_MUX2_FOR_17_ID:
      case CAI_MUX2_FOR_21_ID:
      case CAI_MUX2_FOR_23_ID:
      case CAI_MUX2_FOR_25_ID:

      case CAI_MUX1_FOR_5_ID:
      case CAI_MUX1_FOR_14_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is blank, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '01' and shall set data
        ** bits to any desired value.  The resulting information bits
        ** shall be supplied to the multiplex sublayer as a Rate 1 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '01' */
        *frame_data[0] = 0x40;

        tx_data_rate = CAI_FULL_RATE;
        break;

      case CAI_MUX1_FOR_9_ID:
        /* ----------------------------------------------------------------
        ** If the received packet is Rate 1 with bit errors, then Service
        ** Option 2 shall set the CAT_TYPE field equal to '10' and the 169
        ** data bits to the received 169 received data bits.  The resulting
        ** 171 information bits shall be supplied to the multiplex sublayer
        ** as a Rate 1 packet.
        ** ---------------------------------------------------------------- */

        /* Ensure that first two bits are set to '10' */
        *frame_data[0] = 0x80;

        /* Copy the 169 received data bits into transmit frame */
        b_copy( rx_data_item.data_bits, 2, (byte *) frame_data, 2,
                (lb_full_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_FULL_RATE;
        break;

      case CAI_MUX2_FOR_26_ID:
      case CAI_MUX1_FOR_10_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is insufficient frame quality (erasure),
        ** then Service Option 2 shall set the CAT_TYPE field equal to
        ** '11' and shall set the 169 data bits to any desired value.
        ** The resulting 171 information bits shall be supplied to the
        ** multiplex sublayer as a Rate 1 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '11' */
        *frame_data[0] = 0xC0;

        tx_data_rate = CAI_FULL_RATE;
        break;

      default:
        ERR_FATAL( "Invalid rx frame category",
                    rx_data_item.frame_category, 0, 0 );

    } /* end switch (rx_data_item.frame_category) */
  } /* end if (max_rate == CAI_FULL_RATE) */

  else if (max_rate == CAI_HALF_RATE)
  {
    /* ------------------------------------------------------------------
    ** Service Option 2 has been commanded to generate a non-blank packet
    ** with a maximum rate of 1/2
    ** ------------------------------------------------------------------ */

    switch (rx_data_item.frame_category)
    {
      case CAI_MUX1_FOR_1_ID:
        /* ----------------------------------------------------------------
        ** If the received packet is Rate 1, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '11' and shall set 78 data bits
        ** to the rightmost 78 of the 169 rxed data bits.  The resulting
        ** 80 information bits  shall be supplied to the multiplex sublayer
        ** as a Rate 1/2 packet.
        ** ---------------------------------------------------------------- */

        /* Ensure that first two bits are set to '11' */
        *frame_data[0] = 0xC0;

        /* Copy the rightmost 78 received data bits into transmit frame */
        b_copy( rx_data_item.data_bits, (lb_full_rate_bits - 78), //lint !e734 loss of precision
                (byte *) frame_data, 2, (lb_half_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_HALF_RATE;
        break;

      case CAI_MUX2_FOR_1_ID:
        /* ----------------------------------------------------------------
        ** If the received packet is Rate 1, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '11' and shall set 122 data bits
        ** to the rightmost 122 of the 169 rxed data bits.  The resulting
        ** 124 information bits  shall be supplied to the multiplex sublayer
        ** as a Rate 1/2 packet.
        ** ---------------------------------------------------------------- */

        /* Ensure that first two bits are set to '11' */
        *frame_data[0] = 0xC0;

        /* Copy the rightmost 122 received data bits into transmit frame */
        b_copy( rx_data_item.data_bits, (lb_full_rate_bits - 122), //lint !e734 loss of precision
                (byte *) frame_data, 2, (lb_half_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_HALF_RATE;
        break;

      case CAI_MUX2_FOR_2_ID:
      case CAI_MUX2_FOR_6_ID:
      case CAI_MUX2_FOR_11_ID:

      case CAI_MUX1_FOR_2_ID:
      case CAI_MUX1_FOR_6_ID:
      case CAI_MUX1_FOR_11_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1/2, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '00' and the data bits to
        ** the rxed data bits.  The resulting information bits shall
        ** be supplied to the multiplex sublayer as a Rate 1/2 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '00' */
        *frame_data[0] = 0;

        /* Copy the received data bits into transmit frame */
        b_copy( rx_data_item.data_bits, 2, (byte *) frame_data, 2,
                (lb_half_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_HALF_RATE;
        break;

      case CAI_MUX2_FOR_3_ID:
      case CAI_MUX2_FOR_7_ID:
      case CAI_MUX2_FOR_12_ID:
      case CAI_MUX2_FOR_15_ID:
      case CAI_MUX2_FOR_19_ID:

      case CAI_MUX1_FOR_3_ID:
      case CAI_MUX1_FOR_7_ID:
      case CAI_MUX1_FOR_12_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1/4, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '00' and the data bits to
        ** the rxed data bits.  The resulting information bits shall
        ** be supplied to the multiplex sublayer as a Rate 1/4 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '00' */
        *frame_data[0] = 0;

        /* Copy the received data bits into transmit frame */
        b_copy( rx_data_item.data_bits, 2, (byte *) frame_data, 2,
                (lb_quarter_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_QUARTER_RATE;
        break;

      case CAI_MUX2_FOR_4_ID:
      case CAI_MUX2_FOR_8_ID:
      case CAI_MUX2_FOR_10_ID:
      case CAI_MUX2_FOR_13_ID:
      case CAI_MUX2_FOR_16_ID:
      case CAI_MUX2_FOR_18_ID:
      case CAI_MUX2_FOR_20_ID:
      case CAI_MUX2_FOR_22_ID:
      case CAI_MUX2_FOR_24_ID:

      case CAI_MUX1_FOR_4_ID:
      case CAI_MUX1_FOR_8_ID:
      case CAI_MUX1_FOR_13_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1/8, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '00' and the data bits to
        ** the rxed data bits.  The resulting information bits shall
        ** be supplied to the multiplex sublayer as a Rate 1/8 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '00' */
        *frame_data[0] = 0;

        /* Copy the received data bits into transmit frame */
        b_copy( rx_data_item.data_bits, 2, (byte *) frame_data, 2,
                (lb_eighth_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_EIGHTH_RATE;
        break;


      case CAI_MUX2_FOR_5_ID:
      case CAI_MUX2_FOR_9_ID:
      case CAI_MUX2_FOR_14_ID:
      case CAI_MUX2_FOR_17_ID:
      case CAI_MUX2_FOR_21_ID:
      case CAI_MUX2_FOR_23_ID:
      case CAI_MUX2_FOR_25_ID:

      case CAI_MUX1_FOR_5_ID:
      case CAI_MUX1_FOR_14_ID:
        /* -------------------------------------------------------------
        ** If the received packet is blank, then Service Option 2 shall
        ** set the CAT_TYPE field equal to '01' and shall set 78 data
        ** bits to any desired value.  The resulting 80 infor bits shall
        ** be supplied to the multiplex sublayer as a Rate 1/2 packet.
        ** ------------------------------------------------------------- */

        /* Ensure that first two bits are set to '01' */
        *frame_data[0] = 0x40;

        tx_data_rate = CAI_HALF_RATE;
        break;

      case CAI_MUX1_FOR_9_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is Rate 1 with bit errors, then Service
        ** Option 2 shall set the CAT_TYPE field equal to '10' and shall
        ** set 78 data bits to the rightmost 78 of the rxed 169 received
        ** data bits.  The resulting 80 information bits shall be supplied
        ** to the multiplex sublayer as a Rate 1/2 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '10' */
        *frame_data[0] = 0x80;

        /* Copy the rightmost 78 received data bits into transmit frame */
        b_copy( rx_data_item.data_bits, (lb_full_rate_bits - 78), //lint !e734 loss of precision
                (byte *) frame_data, 2, (lb_half_rate_bits - 2) ); //lint !e734 loss of precision

        tx_data_rate = CAI_FULL_RATE;
        break;

      case CAI_MUX2_FOR_26_ID:
      case CAI_MUX1_FOR_10_ID:
        /* ---------------------------------------------------------------
        ** If the received packet is insufficient frame quality (erasure),
        ** then Service Option 2 shall set the CAT_TYPE field equal to
        ** '11' and shall set 14 data bits to any desired value.  The
        ** resulting 16 info bits shall be supplied to the multiplex
        ** sublayer as a Rate 1/8 packet.
        ** --------------------------------------------------------------- */

        /* Ensure that first two bits are set to '11' */
        *frame_data[0] = 0xC0;

        tx_data_rate = CAI_EIGHTH_RATE;
        break;

      default:
        ERR_FATAL( "Invalid rx frame category",
                    rx_data_item.frame_category, 0, 0 );

    } /* end switch (rx_data_item.frame_category) */
  } /* end else if (max_rate == CAI_HALF_RATE) */

  else if (max_rate == CAI_BLANK_RATE)
  {
    /* ------------------------------------------------------------------
    ** If Service Option 2 has been commanded to generate a Blank packet,
    ** it shall do so, thus providing zero information bits to the
    ** multiplex sublayer.
    ** ------------------------------------------------------------------ */
    tx_data_rate = CAI_BLANK_RATE;
  }

  else
  {
    ERR_FATAL( "Invalid Rate", max_rate, 0, 0 );
  }

  return (tx_data_rate);

} /* end loopback_tx() */

/*===========================================================================

FUNCTION LOOPBACK_INIT

DESCRIPTION
  Sets the loopback receive data buffer to a known state.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

void loopback_init(
int rate   /* current rate set of loopback service option */
)
{
  /* set the rate set information */
  if (rate == CAI_MUX1 )
  {
    lb_full_rate_bits = 171;
    lb_half_rate_bits = 80;
    lb_quarter_rate_bits = 40;
    lb_eighth_rate_bits = 16;
  }
  else if ( rate== CAI_MUX2 )
  {
    lb_full_rate_bits = 266;
    lb_half_rate_bits = 124;
    lb_quarter_rate_bits = 54 ;
    lb_eighth_rate_bits = 20;
  }

  /* ----------------------------------------------------------------
  ** Set index pointer to point at first item in loopback data buffer
  ** ---------------------------------------------------------------- */
  loopback_data.buf_ptr = 0;

  /* ------------------------------------------------------------
  ** Set frame category field to indicate a blank received packet
  ** ------------------------------------------------------------ */
  loopback_data.buf[0].frame_category = CAI_MUX1_FOR_5_ID;

} /* end loopback_init() */


