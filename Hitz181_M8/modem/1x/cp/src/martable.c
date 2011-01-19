/*===========================================================================

                    M A R K O V    T A B L E S

DESCRIPTION
  This file contains tables needed for the markov services

Copyright (c) 1996-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/martable.c_v   1.1   03 Oct 2002 10:51:08   phosabet  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/martable.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/09/03   ph      removed FEATURE_NO_VBB
10/03/02   ph      Lint Cleanup
03/17/99    ms     Added FEATURE_NO_VBB
08/31/98   pms     Added Tan's Pascal calling convention code for ROM savings
07/29/98   lcc     Merged in ARM support.
04/08/96   dna     Changed external array names to follow convention.
02/28/96   dna     Created module from tables originally in martable.h.
                   The tables now reside in ROM.  I also made types for
                   the multi-dimentional arrays to make their refrences
                   more readable when moved into the Boot Block.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "martable.h"

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/* Enums for indexing frame and bit error counter test structure */
/* which is declared as an array internally but accessed as a    */
/* structure from externally */

/* lookup tables for masks, use (fer_rate - 1) as index */


/* no. of 24 bits in packets
 * 16,40,80,171 rate set 1
 * 20,54,124,266 rate set 2
*/
mar_2x4_type const mar_tri_bytes = { 1, 2, 4, 7,
                                  1, 3, 6, 11};

/* mask last dword to fill packet */
mar_2x4dw_type const mar_last_mask =
{ 0xFFFF, 0xFFFF, 0xFF,0xE0FFFFFF, //lint !e570 loss of sign
  0xF0FFFF, 0x0FC, 0xF0, 0xC0FFFFFF }; //lint !e570 loss of sign

/* no. of bytes in packet - 1 */
mar_2x4_type const mar_bytes = { 16/8-1, 40/8-1, 80/8-1, 171/8,
                              20/8, 54/8, 124/8, 266/8 };

/* mask last byte to compare bits */
mar_2x4_type const mar_last_byte =  { 0xFF, 0xFF, 0xFF, 0xE0,
                                   0xF0, 0xFC, 0xF0, 0xC0 };

 /* lookup table for bits set in a byte used for bit error counting */

  byte const mar_ones [256] = {
 0,  1,  1,  2,  1,  2,  2,  3,
 1,  2,  2,  3,  2,  3,  3,  4,
 1,  2,  2,  3,  2,  3,  3,  4,
 2,  3,  3,  4,  3,  4,  4,  5,
 1,  2,  2,  3,  2,  3,  3,  4,
 2,  3,  3,  4,  3,  4,  4,  5,
 2,  3,  3,  4,  3,  4,  4,  5,
 3,  4,  4,  5,  4,  5,  5,  6,
 1,  2,  2,  3,  2,  3,  3,  4,
 2,  3,  3,  4,  3,  4,  4,  5,
 2,  3,  3,  4,  3,  4,  4,  5,
 3,  4,  4,  5,  4,  5,  5,  6,
 2,  3,  3,  4,  3,  4,  4,  5,
 3,  4,  4,  5,  4,  5,  5,  6,
 3,  4,  4,  5,  4,  5,  5,  6,
 4,  5,  5,  6,  5,  6,  6,  7,
 1,  2,  2,  3,  2,  3,  3,  4,
 2,  3,  3,  4,  3,  4,  4,  5,
 2,  3,  3,  4,  3,  4,  4,  5,
 3,  4,  4,  5,  4,  5,  5,  6,
 2,  3,  3,  4,  3,  4,  4,  5,
 3,  4,  4,  5,  4,  5,  5,  6,
 3,  4,  4,  5,  4,  5,  5,  6,
 4,  5,  5,  6,  5,  6,  6,  7,
 2,  3,  3,  4,  3,  4,  4,  5,
 3,  4,  4,  5,  4,  5,  5,  6,
 3,  4,  4,  5,  4,  5,  5,  6,
 4,  5,  5,  6,  5,  6,  6,  7,
 3,  4,  4,  5,  4,  5,  5,  6,
 4,  5,  5,  6,  5,  6,  6,  7,
 4,  5,  5,  6,  5,  6,  6,  7,
 5,  6,  6,  7,  6,  7,  7,  8   };

/*===========================================================================

           MARKOV TABLE BINARY PROBABILITIES

Probabilities are listed as values which ran# must be less than to be
valid. Probs are listed eighth, eighth+quarter, eighth+quarter+half.
If ran# is greater or equal than last value then transition to full.

Sn = State at frame n.  Rn = Rate at frame n.
State is table 1st dim index.     Transition is index to table 2nd dim.

Sn+1 = 4 * ( Sn mod 4 ) + ( 3 - Trans ).
Rn+1 = 4 - ( Sn+1 mod 4 ).

===========================================================================*/

  mar_prob_type const mar_Markov_prob = {

0,     0,     2916,                  /* Variable Rate Probs */
0,     20906, 25264,
0,     0,     0,
0,     0,     0,
0,     0,     4915,
0,     17170, 24969,
21856, 25887, 27099,
0,     0,     0,
0,     0,     4522,
0,     5472,  16384,
21856, 21856, 24576,
28246, 29622, 30802,
0,     0,     5472,
0,     6554,  6554,
28377, 28934, 29491,
29753, 32473, 32571,

32768,32768,32768,                  /* Fixed Eighth Rate Probs */
32768,32768,32768,
32768,32768,32768,
32768,32768,32768,
32768,32768,32768,
32768,32768,32768,
32768,32768,32768,
32768,32768,32768,
32768,32768,32768,
32768,32768,32768,
32768,32768,32768,
32768,32768,32768,
32768,32768,32768,
32768,32768,32768,
32768,32768,32768,
32768,32768,32768,

0,32768,32768,                  /* Fixed Quarter Rate Probs */
0,32768,32768,
0,32768,32768,
0,32768,32768,
0,32768,32768,
0,32768,32768,
0,32768,32768,
0,32768,32768,
0,32768,32768,
0,32768,32768,
0,32768,32768,
0,32768,32768,
0,32768,32768,
0,32768,32768,
0,32768,32768,
0,32768,32768,

0,0,32768,                          /* Fixed Half Rate Probs */
0,0,32768,
0,0,32768,
0,0,32768,
0,0,32768,
0,0,32768,
0,0,32768,
0,0,32768,
0,0,32768,
0,0,32768,
0,0,32768,
0,0,32768,
0,0,32768,
0,0,32768,
0,0,32768,
0,0,32768,

0,0,0,                              /* Fixed Full Rate Probs */
0,0,0,
0,0,0,
0,0,0,
0,0,0,
0,0,0,
0,0,0,
0,0,0,
0,0,0,
0,0,0,
0,0,0,
0,0,0,
0,0,0,
0,0,0,
0,0,0,
0,0,0          };

  byte const mar_mux_late [5] =
  { 9, 7, 6, 5, 0 };    /* xlate rate into mux1 */
