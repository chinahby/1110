#ifndef MAR_H
#define MAR_H
/*===========================================================================

                   M A R    H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with Markov Service Option
  services.

Copyright (c) 1991-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mar.h_v   1.0.2.0   30 Nov 2001 16:58:06   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/inc/mar.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $
   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/15/96   gb      Fixed Markov reinitialization by adding mar_init_synch.
11/20/95   gb      Added Markov retrievable parameter support.
10/12/95   gb      Added constants for service option control.
06/14/95   gb      Added support for Rate set 2 service options.
05/12/94   gb      Added new update markov probability table call.
06/14/91   gb      Created file. 

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "cai.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/*
 * CTL_REC_TYPE Message Field  definitions
 */
#define MAR_CTL_TYP 0          /* byte index for ctl type field */
#define MAR_CTL_DIR 0          /* Control Directive in the CTL_REC_TYPE */
#define MAR_RET_DIR 1          /* Retrieve counters Directive */
#define MAR_VECT    1          /* byte index for retrieve vector id */

/*
 * Control Field definitions
 */
#define MAR_CTL 1              /* byte index for ctl field */
#define MAR_TST 0xE0           /* type of Markov test field */
#define MAR_TST_SHFT 5         /* # of bits to shift to use field */
#define MAR_COPY 0x10          /* copy counters bit field */      
#define MAR_ZERO 0x08          /* zero counters bit field */      

/*---------------------------------------------------------------------------
                        FER DATA RATE CONSTANTS

The following constants are used with the 'rate' fields of FER transmit
and receive routine parameters.
---------------------------------------------------------------------------*/

typedef enum  
{
  MAR_0_RATE = 0,      /* Indicates data was blanked. */
  MAR_8_RATE,          /* Indicates rate 1/8 data. */
  MAR_4_RATE,          /* Indicates rate 1/4 data. */
  MAR_2_RATE,          /* Indicates rate 1/2 data. */
  MAR_1_RATE,          /* Indicates rate 1   data. */
  MAR_VAR_RATE         /* Indicates variable rate data */
}mar_rate_type;

/*---------------------------------------------------------------------------
                        MAR ACUMULATOR STRUCTURE

The following structure is used to accumulate the different frame errors.
---------------------------------------------------------------------------*/

typedef struct
{
  dword no_err;     /* data was rx no error, correct rate.       */
  dword err;        /* data was rx in error, correct rate.       */
  dword rate1;      /* data was rx in error, incorrect rate hi.  */
  dword rate2;      /* data was rx in error, incorrect rate mid. */
  dword rate3;      /* data was rx in error, incorrect rate lo.  */
  dword erase;      /* erasures */
  dword frames;     /* total frames received */
  dword berrs;      /* total bits in error */
} mar_type;


/*---------------------------------------------------------------------------
                        MAR STATISTICS TYPE

The following structure is used to display Markov statistics.
---------------------------------------------------------------------------*/

typedef struct
{
  mar_rate_type m_rate;  /* current Markov rate */
  dword bit_errs;        /* current mar_bit_errs */
  dword bad_frames;      /* number of frames with errors */
  dword total_frames;    /* total number of frames */
  word mar_test[ 4 ] [ 10 ];
    /* full, half, quarter and eighth rate entries from mar_test array */
  word good_errs [4];    /* good rate frames with bit errs for 4 rates */
} mar_stat_type;


#define  MAR_192_BITS  192  /* Bits per rate 1   frame */
#define  MAR_96_BITS   96   /* Bits per rate 1/2 frame */
#define  MAR_48_BITS   48   /* Bits per rate 1/4 frame */
#define  MAR_24_BITS   24   /* Bits per rate 1/8 frame */

typedef struct
{
  byte rate1[ MAR_192_BITS / 8 ];  /* Rate 1   data bits */
  byte rate2[ MAR_96_BITS  / 8 ];  /* Rate 1/2 data bits */
  byte rate4[ MAR_48_BITS  / 8 ];  /* Rate 1/4 data bits */
  byte rate8[ MAR_24_BITS  / 8 ];  /* Rate 1/8 data bits */
}
mar_bits_type;
  /* Format of byte data read from decoder. */

typedef struct
{
   dword mso_e1_t1;
   dword mso_e2_t2;
   dword mso_e4_t4;
   dword mso_e8_t8;
   dword mso_e1_td;
   dword mso_e2_td;
   dword mso_e4_td;
   dword mso_e8_td;
   dword mso_e1_tb;
   dword mso_e2_tb;
   dword mso_e4_tb;
   dword mso_e8_tb;
} mso_e_tx_type;

typedef struct
{
  dword mso_eb_rd;
  dword mso_eb_r8;
  dword mso_eb_r4;
  dword mso_eb_r2;
  dword mso_eb_r1;
  dword mso_eb_re;

  dword mso_e8_rd;
  dword mso_e8_r8;
  dword mso_e8_r4;
  dword mso_e8_r2;
  dword mso_e8_r1;
  dword mso_e8_re;

  dword mso_e4_rd;
  dword mso_e4_r8;
  dword mso_e4_r4;
  dword mso_e4_r2;
  dword mso_e4_r1;
  dword mso_e4_re;

  dword mso_e2_rd;
  dword mso_e2_r8;
  dword mso_e2_r4;
  dword mso_e2_r2;
  dword mso_e2_r1;
  dword mso_e2_re;

  dword mso_e1_rd;
  dword mso_e1_r8;
  dword mso_e1_r4;
  dword mso_e1_r2;
  dword mso_e1_r1;
  dword mso_e1_re;
} mso_e_rx_type;

/* <EJECT> */
/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

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
);   
             
/* <EJECT> */
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
);

/* <EJECT> */
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
word rx_mux1,                            /* mux rate decision from rx */
dword frame32,                           /* ls 32 bits of frame # */
byte *mar_mask_ptr                       /* mask of bit errors */
);

/* <EJECT> */
/*===========================================================================

FUNCTION MAR_GET

DESCRIPTION
  This function accepts a pointer to the bit error rate accumulator
  structure to be filled from the current test statistics.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The structure pointed to by mar_ptr willed be filled with the current
    error statistics.

===========================================================================*/

void mar_get
(
mar_type *mar_ptr             /* pointer to structure defined in mar.h */
);

/* <EJECT> */
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
);

/* <EJECT> */
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
  The local global variables tri_bytes, last_mask, bytes, last_byte
  are initialized and the structure test may be zeroed.

===========================================================================*/

void mar_init
(
word test_rate ,             /* data rate to generate and test data at */
boolean zero_count,              /* flag TRUE = zero bit error counters */
boolean copy_count,              /* flag TRUE = copy bit error counters */
dword esn,                   /* Serial number of phone   */
boolean rate_set   /* rate set 1 or 2 */
);

/* <EJECT> */
/*===========================================================================

FUNCTION MAR_INIT_SYNCH

DESCRIPTION
 Initialize the state of the Markov model to unsynched.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  The local global boolean variable mar_in_sync is cleared.

===========================================================================*/
void mar_init_synch(void);

/* <EJECT> */
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
);

/* <EJECT> */
/*===========================================================================

FUNCTION MAR_UPDATE_TABLE

DESCRIPTION
  This function updates the Markov probability table for variable rate.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/

void mar_update_table
(
  word *new_table,   /* pointer to new table of probabilities */
  word length        /* size of table to copy */
);

/* <EJECT> */
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

void mar_log_tx (word mux);


/* <EJECT> */
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
);

#endif /* MAR_H */

