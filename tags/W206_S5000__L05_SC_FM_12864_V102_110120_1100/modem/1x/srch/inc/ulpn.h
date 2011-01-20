#ifndef ULPN_H
#define ULPN_H
/*===========================================================================

                   U L P N    H E A D E R    F I L E

DESCRIPTION
   This file contains definitions used by the User Long PN services.

   It is used only in the portable subscriber station.

   Copyright (c) 1992, 1993 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 1994, 1995 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 1996, 1997 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 1998, 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 2002, 2003 by QUALCOMM, Incorporated.  All Rights Reserved.

   Copyright (c) 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.

   Copyright (c) 2006       by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/inc/ulpn.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/18/06   bn      Added new function prototype ulpn_advance_chips
12/03/02   bt      cleanup of the interface. All advance/retard func. were
                   moved into ulpn_adjust.
07/30/02   gs      Initial merge into perforce for cougar.
03/12/01   dgy     Added FEATURE_IS2000_QPCH_CCI.
11/18/99   ajn     Enhanced Standby III ... 320ms based ULPN calculations
08/06/99   ejv     ajn: Externed function ulpn_quarter_advance.
01/13/99   ejv     Removed _cdecl - not necessary for ARM.
09/01/98   thh     Added keyword _cdecl to the declarations of those functions
                   which are called by assembly codes using C function calling
                   convention.
                   Updated copyright date.
03/02/93   ptw     Created.

===========================================================================*/

#include "comdef.h"

/*===========================================================================

                      CONSTANTS

===========================================================================*/

#define ROLLS_MAX_ADJUST       24575
  /* Max Number of rolls that the ULPN can be adjusted by*/
  
/*===========================================================================

                      REGIONAL DATA DECLARATIONS

===========================================================================*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
      Declaration of Data Type for User Long PN Code
- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

typedef struct
  {
  dword  low;
  word   high;
  } ulpn_type;
   /* This structure of 48 bits is used to handle the 42-bit user long
      code PN states.  Most arithmetic on these numbers is done 48 bits
      wide for simplicity. */


/*===========================================================================

                      REGIONAL FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION ulpn_adjust

DESCRIPTION
  This function computes the user long PN code state at some time
  in the future/past, given a current user long PN code state and the
  number of intervening 26.666... ms periods.

DEPENDENCIES
  The function depends on the magic properties of the pn_advance/retard 
  array. (See comment at pn_advance/retard array declarations.)

RETURN VALUE
  No formal return value, but the ULPN state at *pn_state_ptr is
  updated to give the new ULPN state value.

SIDE EFFECTS
  None.

===========================================================================*/
extern void ulpn_adjust
(
  ulpn_type *pn_state_ptr,
    /* Pointer to the value of the user long PN;
       At entry: the old value
       At exit: the new value */

  int16 move
    /* Number of rolls to move the long code by */
);

/*===========================================================================

FUNCTION ULPN_QUARTER_ADVANCE

DESCRIPTION
  Advances the PN value by 1/4 PN cirle.

DEPENDENCIES
  The function depends on the magic properties of the pn_advance array.
  (See comment at pn_advance array declaration.)

RETURN VALUE 
  No formal return value, but the ULPN state at *new_pn_ptr is
  updated to give the new ULPN state value.

SIDE EFFECTS
  None

===========================================================================*/
extern void ulpn_quarter_advance
(
  ulpn_type *pn_state_ptr
    /* Pointer to the ULPN to be changed */
);

/*===========================================================================

FUNCTION ULPN_3_QUARTER_RETARD

DESCRIPTION
  Retards the PN value by 3/4 PN ROLL in time.

DEPENDENCIES

RETURN VALUE 
  No formal return value, but the ULPN state at *new_pn_ptr is
  updated to give the new ULPN state value.

SIDE EFFECTS
  None

===========================================================================*/
extern void ulpn_3_quarter_retard
(
  ulpn_type *pn_state_ptr
    /* Pointer to the ULPN to be changed */
);

#ifdef FEATURE_SRCH_VOIP_1X_HANDOFF
#error code not present
#endif /* FEATURE_SRCH_VOIP_1X_HANDOFF */

#endif /* ULPN_H */

