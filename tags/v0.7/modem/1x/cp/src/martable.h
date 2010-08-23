#ifndef MARTABLE_H
#define MARTABLE_H
/*===========================================================================

           B E R    T A B L E   H E A D E R    F I L E

DESCRIPTION
  This file contains declarations associated with the MAR services

Copyright (c) 1991-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/martable.h_v   1.0.2.0   30 Nov 2001 16:59:16   fchan  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/martable.h#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/09/03   ph      removed FEATURE_NO_VBB
03/17/99   ms      Added FEATURE_NO_VBB
08/31/98   pms     Added Tan's Pascal calling convention code for ROM savings
07/29/98   lcc     Merged in ARM support.
04/08/96   dna     Changed external array names to follow convention.
02/28/96   dna     Moved tables to a C file and added the proper external
                   declarations here.
06/14/95   gb      Added support for Rate set 2 service options.
06/27/91   gb      Created file.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"

/* <EJECT> */
/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/

/* Types for different array sizes */
typedef word mar_2x4_type [2] [4];
typedef dword mar_2x4dw_type [2] [4];
typedef word mar_prob_type [5] [16] [3];

/* Enums for indexing frame and bit error counter test structure */
/* which is declared as an array internally but accessed as a    */
/* structure from externally */

/* lookup tables for masks, use (fer_rate - 1) as index */


/* no. of 24 bits in packets
 * 16,40,80,171 rate set 1
 * 20,54,124,266 rate set 2
*/
extern mar_2x4_type const mar_tri_bytes;

/* mask last dword to fill packet */
extern mar_2x4dw_type const mar_last_mask;

/* no. of bytes in packet - 1 */
extern mar_2x4_type const mar_bytes;

/* mask last byte to compare bits */
extern mar_2x4_type const mar_last_byte;


 /* lookup table for bits set in a byte used for bit error counting */

extern byte const mar_ones[];

#define NUM_OF_ONES_IN (byte b) ones [b]

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

extern mar_prob_type const mar_Markov_prob;
extern byte const mar_mux_late[];
#endif /* MARTABLE_H */
