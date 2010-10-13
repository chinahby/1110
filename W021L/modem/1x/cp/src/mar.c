/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   M A R    S E R V I C E S    F I L E

GENERAL DESCRIPTION
  This module contains functions and variables to support Frame & Bit Error
  Rate Services, using a Markov process to model rate changes.
  This includes generating tx packets, checking received
  packets and keeping statistics on the bit error types. The generation
  of the pseudo-random sequence of numbers is in accordance with the algo-
  rithm described in section 2.6.9 of the CDMA Digital CAI Standard. This
  algorithm is also presented, in great detail, in an article entitled
  "Random Number Generators: Good Ones Are Hard To Find", by Stephen K. Park
  and Keith W. Miller, Communications of the ACM, October 1988, Volume 31,
  Number 10.

  The algorithm uses the following iteration:

    z(n) = a x z(n-1) mod m

  where z(n) are integers, a = 7**5 = 16807, m = 2**31 - 1 = 2147483647.

  A generator based on this algorithm is known formally as a prime
  modulus multiplicative linear congruential generator.

EXTERNALIZED FUNCTIONS
  mar_init - for setting up the rate and mask variables.
  mar_tx - for generating the next pseudo-random packet.
  mar_rx - for checking the received packet against a reference.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Prior to using the other bit error rate services, function mar_init
  should be called to initialize the rate dependant variables.

  Note: these services are re-entrant, i.e., they may be called concurrently
  from multiple tasks and/or interrupt handlers.

Copyright (c) 1990-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mar.c_v   1.3   03 Oct 2002 10:51:08   phosabet  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mar.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/26/06   an      Fixed Lint Errors
02/03/05   fc      Fixed lint error.
01/09/03   ph      removed FEATURE_NO_VBB
12/12/02   dlb     Typecast data_ptr as PACKED so that compiler won't assume
                   that it's word/dword aligned. 
10/03/02   ph      Lint Cleanup
05/31/02   va      Fixed case 2 of mar_get_vect to retrieve values from mso_tx_save.
04/18/01   lcc     Removed mar_log_tx from IRAM.
07/27/00   rmd     Added MSM5000_IRAM_REV #ifndef around the functions that need to 
                   be placed inside IRAM.
03/17/99   ms      Added FEATURE_NO_VBB.
10/02/98   pms     Neared more variables to save ROM space.
09/15/98   pms     Backed out _near pointers
09/04/98   pms     Neared the variables to save ROM space.
09/01/98   lcc     Merged in ARM support.
04/08/96   gb      Fixed Markov tx to send 8th rate frames before sync.
04/08/96   dna     Changed external array names to follow convention.
02/28/96   dna     Modified references to muli-dimentional markov tables, now
                   accessed through an indirect pointer, to dereference
                   the pointer.
01/15/96   gb      Fixed Markov reinitialization by adding mar_init_synch.
12/11/95   gb      Fixed mar_rx to not count signalling frames as errors
                   and fixed variable rate initialization.
11/20/95   gb      Added Markov retrievable parameter support.
10/06/95   gb      Fixed new vector counters to be spec compliant.
09/29/95   gb      Changed rate indexing to be raw VOC_RATE to handle blank.
09/07/95   gb      Fixed mar_get_stat reporting inverted rate indexing.
08/16/95   gb      Changed mar_get_stat reporting to support rate set 2.
06/14/95   gb      Added support for Rate set 2 service options.
05/12/94   gb      Added service to update markov probability table.
07/21/92   gb      Created DMSS port.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include <memory.h>
#include "comdef.h"
#include "qw.h"
#include "cai.h"
#include "mar.h"
#include "martable.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/
/* SEEDS need to be reversed on opposite end of link - MTSO */
  /* random number generator seed for TX data and rates */
#define DEF_TX_SEED    0x55555555
  /* random number generator seed for RX data and rates */
#define DEF_RX_SEED    0x2aaaaaaa
//#define DEF_RX_SEED    0x55555555  /* For OFFLINE TEST only */

enum  mar_index_type {
  TOTAL_FRAMES,
  BAD_FRAMES,
  BIT_ERRS
} ;                /* indices into totals array */

word mar_stop=0;                /* debug */

dword mar_test [5] [6];   /* error counters, indices in martable.h */
dword mar_totals [3];           /* a few pertinent totals */
word  good_errs [5];            /* frames detected correct rate but bit errs */
word  rx_M_state = 15;          /* current state of rx Markov model */
word  tx_M_state = 15;          /* current state of tx Markov model */
dword rx_M_seed = DEF_RX_SEED;  /* current seed for rx Markov model */
dword tx_M_seed = DEF_TX_SEED ; /* current seed for tx Markov model */
word  mar_reset_state = 15;     /* reset state of this rate Markov model */
word  M_rate;                   /* index into tables of probabilities */
word  M_rate_set;               /* index into tables of bitmasks */
dword mar_esn;                  /* Serial num of phone used to sync Markov */


boolean mar_limit_state = FALSE;     /* limit state enabled flag */
word    mar_limit_frames = 3;        /* limit state frame count */
boolean mar_limit_flag = TRUE;       /* limit state invert flag */
word    mar_limit_count;             /* limit state counter */
word    new_limit_count;
byte    new_rate;

boolean mar_copy_rx_counters;
boolean mar_copy_tx_counters;

/* This defines the 'a' value used by the generator. */
#define  RAN_A_VAL  16807

/* This defines the 'm' value used by the generator. */
#define  RAN_M_VAL  2147483647

/* indicator of whether markov service is in sync with base station */
boolean mar_rx_sync;
boolean mar_tx_sync;

/* Formal statistics types to store copied counters */
mso_e_tx_type mso_tx, mso_tx_save;
mso_e_rx_type mso_rx;

void mar_copy_rx_stats (void);
void mar_copy_tx_stats (void);

/*===========================================================================

FUNCTION MAR_RAN_NEXT

DESCRIPTION
  This function seeds the pseudo-random number generator with a specified
  value, performs the next iteration for the pseudo-random
  number sequence and returns the resulting value.

DEPENDENCIES
  None.

RETURN VALUE
  This function a returns a left justified, 31-bit psuedo-random dword
  representing a binary fraction in the range: 0 < binary fraction < 1.
  Notice that bit zero of the return value will always be zero.
  This value can be used to re-seed the generator.

SIDE EFFECTS
  None.

===========================================================================*/

dword  mar_ran_next
(
  dword  seed
    /* This value is used to seed the pseudo-random number generator.
       It MUST be in the following range: 0 < seed < 2**31 - 1. Notice
       that the seed must NOT be zero and must NOT be 2**31 - 1. */
)
{
  /*------------------------------------------------------------------------
    This function calculates the next value of ran_reg using the formula:

          z(n) = a * z(n-1) mod( 2^31 - 1 )

    The value of z(n) is calculated as follows using a 32-bit wordsize:

    1. Shift z(n-1) left by one bit so it is right justified.

    2. Multiply a*z(n-1) and place the result into two 32-bit words called
       az_hi and az_lo.

       At this point, we have:   a*z(n-1) = az_hi*(2^32) + az_lo

    3. Keep the lower 31 bits of the product in the 32-bit word az_lo, and
       place the remaining upper 15 bits in the 32-bit word az_hi.

       At this point, we have:   a*z(n-1) = az_hi*(2^31) + az_lo
       which can be rewritten:   a*z(n-1) = az_hi*(2^31-1) + (az_lo+az_hi)

    4. To get a*z(n-1)mod(2^31-1), add az_hi to az_lo and check to see if
       the result is equal to or greater than 2^31-1.  If it is, subtract
       off 2^31-1. (This is why z(n-1) was right justified in step 1)

    5. Finally, shift the result left by one bit to make z(n) a left
       justified binary fraction again.
  ------------------------------------------------------------------------*/

  dword az_hi,az_lo;

  /* Shift z(n-1) left one bit so we can use MSB as an overflow bit */

  seed >>= 1;

  /* Multiply a*z(n-1), putting the lower 31 bits into az_lo, and the
  ** remaining upper bits into az_hi
  */

  az_lo = RAN_A_VAL * (seed & 0xFFFF);
  az_hi = RAN_A_VAL * (seed >> 16) + (az_lo >> 16);

  az_lo = ( (az_hi << 16) | (az_lo & 0xFFFF) ) & RAN_M_VAL;
  az_hi >>= 15;

  /* Add the upper bits to the lower bits to get a*z(n-1)mod(2^31-1), if the
  ** result is not less than the modulus, subtract the modulus from the result
  */

  az_lo += az_hi;

  if ( az_lo >= RAN_M_VAL ){
    az_lo -= RAN_M_VAL;
  }

  /* Place the result into z(n) and shift it back to a left justified
  ** binary fraction
  */

  seed = az_lo << 1;

  return seed;

} /* mar_ran_next */

/*===========================================================================

FUNCTION MAR_TX

DESCRIPTION
  This function seeds the pseudo-random number generator with the frame
  value, performs the next iterations for the pseudo-random
  number sequence and stores the resulting packet in the buffer pointed
  to by data_ptr.

DEPENDENCIES
  The rate dependant variables should have been initialized by a call to
  mar_init before calling this function.

RETURN VALUE
  Returns the test rate value of the frame produced.

SIDE EFFECTS
  The buffer pointed to by data_ptr will be filled with up to 171 bits.

===========================================================================*/

byte  mar_tx (
byte *data_ptr,                 /* buffer to be filled */
dword frame32,                  /* ls 32 bits of frame number */
boolean is_rx                   /* indicator whether rx frame or tx frame */
)
{
  word i;
  dword *seed_ptr;              /* pointer to previous value */
  word cur_state;               /* temp variable for state */
  byte mar_rate;                /* rate of this frame */
  word mod4;                    /* Markov state modulo 4 */

/*-------------------------------------------------------------------------*/

  if (is_rx)                    /* is it an RX frame? */
  {
    seed_ptr = &rx_M_seed;      /* point to RX seed */
    cur_state = rx_M_state;     /* point to RX state */
    frame32 ^= DEF_RX_SEED;     /* XOR frame with RX SEED */
  }
  else
  {
    seed_ptr = &tx_M_seed;      /* point to TX seed */
    cur_state = tx_M_state;     /* point to TX state */
    frame32 ^= DEF_TX_SEED;     /* XOR frame with TX SEED */
  }

  if ((frame32 & 0x1FF) == (mar_esn & 0x1FF ))  /* 512 frame Markov sync */
  {
    cur_state = mar_reset_state;                /* Reset Markov state */
    *seed_ptr = frame32 << 1;                   /* Reset seed to current */
    *seed_ptr = mar_ran_next (*seed_ptr);       /*    frame and crank a */
    *seed_ptr = mar_ran_next (*seed_ptr);       /*    few times */
    *seed_ptr = mar_ran_next (*seed_ptr);
    if (is_rx)                                  /* is it an RX frame? */
    {
      if (mar_copy_rx_counters && is_rx) /*lint !e774*/
      {
        mar_copy_rx_counters = FALSE;
        mar_copy_rx_stats ();
      }
      mar_rx_sync = TRUE;                       /* flag RX now in sync */
    }
    else                                        /* is it a TX frame? */
    {
      if (mar_copy_tx_counters)
      {
        mar_copy_tx_counters = FALSE;
        mar_copy_tx_stats ();
      }
      mar_tx_sync = TRUE;                       /* flag TX now in sync */
    }
  }

  mod4 = cur_state & 0x3;                       /* cur_state mod 4 */

  *seed_ptr = mar_ran_next (*seed_ptr);         /* get next ran number */
  /* store 24 bits in frame */
  *((PACKED dword *) (data_ptr)) = *seed_ptr >> 8;  /*lint !e826 */   

  for (i=0;i<3;i++)
  {                         /* test ran # against Markov probabilities */
    if ( (*((PACKED word *)(data_ptr)) & 0x7FFF) <  
          mar_Markov_prob[M_rate] [cur_state] [i] )  /*lint !e826 */
    {
      cur_state = 4 * mod4 + ( 3 - i );         /* set next state in model */ //lint !e734 precision loss
      break;                                    /* no need to test more */
    }
  }
    /* if tested 3 times must be in last probability range - set new state */
  if (i == 3) cur_state = 4 * mod4; //lint !e734 precision loss
  mar_rate = 4 - (cur_state & 0x3);             /* get rate from state */

    /* look up the number of 24 bit fields we need to generate */
  for (i=1; i < mar_tri_bytes[M_rate_set] [mar_rate-1]; i++)
  {
    *seed_ptr = mar_ran_next (*seed_ptr);       /* use prev to seed next */
    /* store next tri-byte */
    *(PACKED dword *) (data_ptr + i * 3) = *seed_ptr >> 8; /*lint !e826 */
  }
    /* mask unused bits in last tri-byte according to rate of frame */
  *(PACKED dword *) (data_ptr + (i-1) * 3) &= mar_last_mask [M_rate_set] [mar_rate-1]; /*lint !e826 */

  if (is_rx)
    rx_M_state = cur_state;                     /* update rx Markov */
  else
    tx_M_state = cur_state;                     /* update tx Markov */

  if (( mar_limit_state ) && ( !is_rx ))
  {
    new_rate = mar_rate;
    new_limit_count = mar_limit_count;
    if ( ((!mar_limit_flag) &&
          (mar_rate == MAR_1_RATE) && /*lint !e641 */
          (mar_limit_count <  mar_limit_frames)) ||
         (( mar_limit_flag) &&
          (mar_rate == MAR_1_RATE) &&  /*lint !e641 */
          (mar_limit_count >= mar_limit_frames)) )
    {
      new_rate = MAR_2_RATE;  /*lint !e641 */
    }
    if (((mar_rate == MAR_1_RATE) && (mar_limit_count >= mar_limit_frames)) || /*lint !e641 */
        (( mar_limit_flag) && (mar_rate != MAR_1_RATE)) ) /*lint !e641 */
    {
      new_limit_count = 0;
    }
    if ( ((!mar_limit_flag ) && (mar_rate != MAR_1_RATE)) ||  /*lint !e641 */
         ((mar_rate == MAR_1_RATE) && (mar_limit_count <  mar_limit_frames)) )  /*lint !e641 */
    {
        new_limit_count = mar_limit_count + 1;
    }
    mar_rate = new_rate;
    mar_limit_count = new_limit_count;
  }

  if ((!mar_tx_sync) && (!is_rx))
    return (MAR_8_RATE); /*lint !e641 */   /* return eighth rate until sync */

  return ( mar_rate );                          /* return frame rate */

}

/*===========================================================================

FUNCTION MAR_RX

DESCRIPTION
  This function calls the mar_tx function to generate a reference packet
  and seeds the pseudo-random number generator with the received frame
  number, checks the rx packet against the reference and increments the
  appropriate error counters according to received rate decision rx_rate.

DEPENDENCIES
  The rate dependant variables should have been initialized by a call to
  mar_init before calling this function.

RETURN VALUE
  If Markov services are in sync this procedure returns a word being Markov
  rate in lower byte and frame bit errors in upper byte.  If Markov services
  are not in sync this procedure returns 0xFFFF,  indicating that the value
  should not be logged.

SIDE EFFECTS
  Some of the error counters in the structure mar_test may be incremented.

===========================================================================*/

word  mar_rx (
byte *mar_rx_data,                       /* pointer to rx dec buffer */
word rx_rate,                            /* rate decision from rx */
dword frame32,                           /* ls 32 bits of frame # */
byte *mar_mask_ptr                       /* mask of bit errors */
)
{
  word i;
  byte err_mask;
  byte bers;                                 /* number of bit errs in pkt */
  word exp_rate;                             /* Markov rate and mux */
  int err = FALSE;                           /* bit error flag */
  static byte mar_tx_data [ 265 / 8 + 1 ];   /* tx reference pkt storage */

/*-------------------------------------------------------------------------*/
  exp_rate = mar_tx ( mar_tx_data, frame32, TRUE ); /* create ref packet */

  /* check each byte for errors and lookup # of bit errors in mar_ones table */

  bers = 0;

  if (exp_rate == rx_rate)
  {
    for (i=0; i < mar_bytes[M_rate_set] [exp_rate-1]; i++)
    {
      if ( (err_mask = (mar_rx_data [i] ^ mar_tx_data [i])) != 0)
      {
        if (mar_mask_ptr != NULL)
          mar_mask_ptr [i] = err_mask;
        bers += mar_ones [ err_mask ];
        err = TRUE;
      }
    }
    /* check last bits for errors and
    lookup # of bit errors in mar_ones table */

    err_mask =  (mar_rx_data [i] ^ mar_tx_data [i]) &
                           mar_last_byte [M_rate_set] [exp_rate-1];
    if (err_mask)
    {
      if (mar_mask_ptr != NULL)
        mar_mask_ptr [i] = err_mask;
      bers += mar_ones [ err_mask ];
      err = TRUE;
    }
  }

  if( mar_rx_sync )
  {
    /* markov service is in sync - tally statistics */

    mar_test [exp_rate] [rx_rate]++;    /* incr error counters in array */
    mar_totals [TOTAL_FRAMES]++;
    if (( err ) || ( rx_rate != exp_rate ))     /* bad rate decision */
    {
      if (rx_rate != MAR_0_RATE)  /*lint !e641 */   /* ignore signalling frames */
        mar_totals [BAD_FRAMES]++;
    }
    if (( err ) && ( exp_rate == rx_rate ))     /* good rate decision */
      good_errs [exp_rate]++;

    mar_totals [BIT_ERRS] += bers;            /* update total bit errors */

    return (exp_rate + (bers << 8) );           /* return stats */
  }
  else
  {
    return( 0xFFFF );                           /* return nosync flag */
  }

}  /*lint !e818*/

/*===========================================================================

FUNCTION MAR_GET_STAT

DESCRIPTION
  This function accepts a pointer to a Markov statistic buffer.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mar_get_stat
(
  mar_stat_type *mar_ptr
    /* pointer to structure defined in mar.h */
)
{
  word i;

  mar_ptr->m_rate = (mar_rate_type) M_rate;         /* rate of Markov model */
  mar_ptr->bit_errs = mar_totals [BIT_ERRS];        /* bit error total */
  mar_ptr->bad_frames = mar_totals[BAD_FRAMES];     /* frame error total */
  mar_ptr->total_frames = mar_totals[TOTAL_FRAMES]; /* total frames rxed */
  for( i=0; i< 4; i++ )
  {
    mar_ptr->good_errs[i] = good_errs[i+1]; /* bit errs in good rate frames */
  }
  for( i=0; i< 4; i++ )
  {
    mar_ptr->mar_test[i][9] = mar_test[4-i][5];       /* Erasures */ //lint !e734 precision loss
    mar_ptr->mar_test[i][0] = mar_test[4-i][4];       /* Got full errs */ //lint !e734 precision loss
    mar_ptr->mar_test[i][5] = mar_test[4-i][3];       /* Got 1/2 errs */ //lint !e734 precision loss
    mar_ptr->mar_test[i][6] = mar_test[4-i][2];       /* Got 1/4 errs */ //lint !e734 precision loss
    mar_ptr->mar_test[i][7] = mar_test[4-i][1];       /* Got 1/8 errs */ //lint !e734 precision loss
    mar_ptr->mar_test[i][3] = mar_test[4-i][0];       /* Signalling */ //lint !e734 precision loss
  }

} /* mar_get_stat */

/*===========================================================================

FUNCTION MAR_COPY_RX_STAT

DESCRIPTION
  This function takes a snapshot of the Markov statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mar_copy_rx_stats (void)
{

 (void) memcpy ( &mso_rx, mar_test, sizeof(mar_test) );

} /* mar_copy_rx_stats */

/*===========================================================================

FUNCTION MAR_COPY_TX_STATS

DESCRIPTION
  This function takes a snapshot of the Markov statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mar_copy_tx_stats (void)
{

 mso_tx_save = mso_tx;

} /* mar_copy_tx_stats */

/*===========================================================================

FUNCTION MAR_LOG_TX

DESCRIPTION
  This function counts up the Markov TX statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mar_log_tx (word mux)
{

  if (!mar_tx_sync)
    return;

  /* determine counter based on frame catetory */
  switch( mux )
  {

    case CAI_MUX1_REV_1_ID:
    case CAI_MUX2_REV_1_ID:
    {
      /* full rate */
      mso_tx.mso_e1_t1++;
      break;
    }

    case CAI_MUX1_REV_6_ID:
    case CAI_MUX2_REV_11_ID:
    {
      /* half rate frame */
      mso_tx.mso_e2_t2++;
      break;
    }

    case CAI_MUX1_REV_7_ID:
    case CAI_MUX2_REV_19_ID:
    {
      /* quarter rate frame */
      mso_tx.mso_e4_t4++;
      break;
    }

    case CAI_MUX1_REV_8_ID:
    case CAI_MUX2_REV_24_ID:
    {
      /* eigth rate frame */
      mso_tx.mso_e8_t8++;
      break;
    }

    case CAI_MUX1_REV_2_ID:
    case CAI_MUX1_REV_3_ID:
    case CAI_MUX1_REV_4_ID:
    case CAI_MUX1_REV_11_ID:
    case CAI_MUX1_REV_12_ID:
    case CAI_MUX1_REV_13_ID:

    case CAI_MUX2_REV_2_ID:
    case CAI_MUX2_REV_3_ID:
    case CAI_MUX2_REV_4_ID:
    case CAI_MUX2_REV_6_ID:
    case CAI_MUX2_REV_7_ID:
    case CAI_MUX2_REV_8_ID:
    case CAI_MUX2_REV_10_ID:
    {
      /* full rate d&b frame */
      mso_tx.mso_e1_td++;
      break;
    }

    case CAI_MUX1_REV_5_ID:
    case CAI_MUX1_REV_14_ID:

    case CAI_MUX2_REV_5_ID:
    case CAI_MUX2_REV_9_ID:
    {
      /* full rate b&b frame */
      mso_tx.mso_e1_tb++;
      break;
    }

    case CAI_MUX2_REV_12_ID:
    case CAI_MUX2_REV_13_ID:
    case CAI_MUX2_REV_15_ID:
    case CAI_MUX2_REV_16_ID:
    case CAI_MUX2_REV_18_ID:
    {
      /* half rate d&b frame */
      mso_tx.mso_e2_td++;
      break;
    }

    case CAI_MUX2_REV_14_ID:
    case CAI_MUX2_REV_17_ID:
    {
      /* half rate b&b frame */
      mso_tx.mso_e2_tb++;
      break;
    }

    case CAI_MUX2_REV_20_ID:
    case CAI_MUX2_REV_22_ID:
    {
      /* quarter rate d&b frame */
      mso_tx.mso_e4_td++;
      break;
    }

    case CAI_MUX2_REV_21_ID:
    case CAI_MUX2_REV_23_ID:
    {
      /* quarter rate b&b frame */
      mso_tx.mso_e4_tb++;
      break;
    }

    case CAI_MUX2_REV_25_ID:
    {
      /* eighth rate b&b frame */
      mso_tx.mso_e8_tb++;
      break;
    }

    default:
      /* appease lint */
      break;
  } /* switch */

} /* mar_copy_tx_stats */

/*===========================================================================

FUNCTION MAR_INIT

DESCRIPTION
  This function initializes the rate dependant variable global to this
  module and if requested zeroes the bit error accumulators.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The local global variables mar_tri_bytes, mar_last_mask, mar_bytes,
  mar_last_byte are initialized and the structure test may be zeroed.

===========================================================================*/

void mar_init
(
word test_rate ,             /* data rate to generate and test data at */
boolean zero_count,              /* flag TRUE = zero frame error counters */
boolean copy_count,              /* flag TRUE = copy frame error counters */
dword esn,                   /* Serial number of phone   */
boolean rate_set                /* rate set 1 or 2 (0 or 1) */
)
{
  M_rate_set = rate_set;  /* which frame sizes to generate and check */

  mar_esn = esn;          /* When to sync markov model for this phone */

  if (M_rate != test_rate)
  {
    M_rate = test_rate;     /* which Markov table of probabilities to use */
    switch (test_rate)
    {
      case MAR_8_RATE :
         mar_reset_state = 15;     /* reset state for 1/8 rate model */
         break;
      case MAR_4_RATE :
         mar_reset_state = 10;     /* reset state for 1/4 rate model */
         break;
      case MAR_2_RATE :
         mar_reset_state = 5;      /* reset state for 1/2 rate model */
         break;
      case MAR_1_RATE :
         mar_reset_state = 0;      /* reset state for full rate model */
         break;
      case MAR_0_RATE :
      case MAR_VAR_RATE :
         mar_reset_state = 15;     /* reset state for variable rate model */
         M_rate = 0;               /* set index into probability table */
         break;
      default:
         break;
    }
    /* initialize markov services to not in sync */
    mar_rx_sync = FALSE;
    mar_tx_sync = FALSE;
  }
  if (zero_count) {             /* command to reset all counters to zero */
    (void) memset ( mar_test, 0, sizeof(mar_test) );
    (void) memset ( mar_totals, 0, sizeof(mar_totals) );
    (void) memset ( good_errs, 0, sizeof(good_errs) );
    (void) memset ( &mso_tx, 0, sizeof(mso_tx) );
    (void) memset ( &mso_rx, 0, sizeof(mso_rx) );
  }
  if (copy_count) {             /* command to copy counter values */
   mar_copy_rx_counters = TRUE;
   mar_copy_tx_counters = TRUE;
  }
  else {
   mar_copy_rx_counters = FALSE;
   mar_copy_tx_counters = FALSE;
  }

}

/*===========================================================================

FUNCTION MAR_INIT_SYNCH

DESCRIPTION
 Initialize the state of the Markov model to unsynched.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The local global boolean variable mar_rx_sync is cleared.

===========================================================================*/
void mar_init_synch(void)
{
    /* initialize markov services to not in sync */
    mar_rx_sync = FALSE;
    mar_tx_sync = FALSE;

}

/*===========================================================================

FUNCTION MAR_RATE_LIMIT

DESCRIPTION
  This function initializes the rate limiting algorithm to the specified rate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mar_rate_limit
(
  word rate_factor        /* rate limit factor to use in sysparm format */
)
{
  switch (rate_factor)
  {
    case 0:
      mar_limit_state = FALSE;     /* limit state for normal operation */
      break;
    case 2:
      mar_limit_state = TRUE;     /* limit state for 7/8 rate limit */
      mar_limit_frames = 3;
      mar_limit_flag = TRUE;
      break;
    case 4:
      mar_limit_state = TRUE;     /* limit state for 3/4 rate limit */
      mar_limit_frames = 1;
      mar_limit_flag = TRUE;
      break;
    case 6:
      mar_limit_state = TRUE;     /* limit state for 5/8 rate limit */
      mar_limit_frames = 3;
      mar_limit_flag = FALSE;
      break;
    case 8:
      mar_limit_state = TRUE;     /* limit state for 1/2 rate limit */
      mar_limit_frames = 0;
      mar_limit_flag = TRUE;
      break;
    default:
      break;
  }
}

/*===========================================================================

FUNCTION MAR_GET_VECT

DESCRIPTION
  This function updates a given table with the appropriate Mar stats.

DEPENDENCIES
  None.

RETURN VALUE
  Length of vector (vector elements are dwords here).

SIDE EFFECTS
  None.

===========================================================================*/

byte mar_get_vect
(
byte vect_counter_id,   /* id of vector of counters */
dword *tsf               /* address of storage for vector */
)
{
  byte len;

  switch (vect_counter_id)
  {
    case 0:
      tsf[0] = mso_rx.mso_e1_r1;
      tsf[1] = mso_rx.mso_e1_r1 + mso_rx.mso_e2_r2 +
               mso_rx.mso_e4_r4 + mso_rx.mso_e8_r8;
      tsf[2] = mso_rx.mso_e1_r2 + mso_rx.mso_e1_r4 +
               mso_rx.mso_e1_r8 + mso_rx.mso_e1_re +
               good_errs [MAR_1_RATE];
      tsf[3] = mso_rx.mso_e2_r1 + mso_rx.mso_e2_r4 +
               mso_rx.mso_e2_r8 + mso_rx.mso_e2_re +
               good_errs [MAR_2_RATE] +
               mso_rx.mso_e4_r1 + mso_rx.mso_e4_r2 +
               mso_rx.mso_e4_r8 + mso_rx.mso_e4_re +
               good_errs [MAR_4_RATE] +
               mso_rx.mso_e8_r1 + mso_rx.mso_e8_r2 +
               mso_rx.mso_e8_r4 + mso_rx.mso_e8_re +
               good_errs [MAR_8_RATE] + tsf [2];
      len = 4;
      break;

    case 1:
      tsf[0] = mso_rx.mso_e1_r1;
      tsf[1] = mso_rx.mso_e2_r2;
      tsf[2] = mso_rx.mso_e4_r4;
      tsf[3] = mso_rx.mso_e8_r8;
      len = 4;
      break;

    case 2:
      tsf[0] = mso_tx_save.mso_e1_t1;
      tsf[1] = mso_tx_save.mso_e2_t2;
      tsf[2] = mso_tx_save.mso_e4_t4;
      tsf[3] = mso_tx_save.mso_e8_t8;
      len = 4;
      break;

    case 3:
      /*lint -e668*/
      (void) memcpy (tsf, &mso_tx_save, sizeof (mso_tx_save) );
      /*lint +e668*/
      len = 12;
      break;

    case 4:
      tsf[0] = mso_rx.mso_e1_r1;
      tsf[1] = mso_rx.mso_e1_rd;
      tsf[2] = 0L;
      tsf[3] = mso_rx.mso_e1_r2;
      tsf[4] = mso_rx.mso_e1_r4;
      tsf[5] = mso_rx.mso_e1_r8;
      tsf[6] = 0L;
      tsf[7] = mso_rx.mso_e1_re;
      tsf[8] = good_errs [MAR_1_RATE];
      len = 9;
      break;

    case 5:
      tsf[0] = mso_rx.mso_e2_r1;
      tsf[1] = mso_rx.mso_e2_rd;
      tsf[2] = 0L;
      tsf[3] = mso_rx.mso_e2_r2;
      tsf[4] = mso_rx.mso_e2_r4;
      tsf[5] = mso_rx.mso_e2_r8;
      tsf[6] = 0L;
      tsf[7] = mso_rx.mso_e2_re;
      tsf[8] = good_errs [MAR_2_RATE];
      len = 9;
      break;

    case 6:
      tsf[0] = mso_rx.mso_e4_r1;
      tsf[1] = mso_rx.mso_e4_rd;
      tsf[2] = 0L;
      tsf[3] = mso_rx.mso_e4_r2;
      tsf[4] = mso_rx.mso_e4_r4;
      tsf[5] = mso_rx.mso_e4_r8;
      tsf[6] = 0L;
      tsf[7] = mso_rx.mso_e4_re;
      tsf[8] = good_errs [MAR_4_RATE];
      len = 9;
      break;

    case 7:
      tsf[0] = mso_rx.mso_e8_r1;
      tsf[1] = mso_rx.mso_e8_rd;
      tsf[2] = 0L;
      tsf[3] = mso_rx.mso_e8_r2;
      tsf[4] = mso_rx.mso_e8_r4;
      tsf[5] = mso_rx.mso_e8_r8;
      tsf[6] = 0L;
      tsf[7] = mso_rx.mso_e8_re;
      tsf[8] = good_errs [MAR_8_RATE];
      len = 9;
      break;

    default:
      len = 0;
   }
   return (len);

}
