#ifndef BOOTDATA_H
#define BOOTDATA_H
/*===========================================================================

            B O O T    D A T A   S E G M E N T   H E A D E R

DESCRIPTION
  This header file contains declarations necessary to use static data from
  within the Boot Block.

Copyright (c) 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

$PVCSPath:
$Header: //depot/asic/msm6275/drivers/boot/bootdata.h#1

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

   
when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/01/04   tkuo    Ported from MSM6275.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "target.h"


/*===========================================================================

                        MACRO DECLARATIONS

===========================================================================*/

#define  BOOT_DATA



/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION BD_GET_WORD

DESCRIPTION
  This function takes a far pointer as a parameter and returns the word value
  stored at the location pointed to by that far pointer.  A call to this
  function causes the segment and offset of the far pointer to be pushed
  on the stack.  This is necessary to avoid having the Microsoft 6.0 
  compiler store the segment portion of the far pointer in a constant in 
  the CONST segment.  If this function were not used to dereference the
  far pointer, a software upgrade to a phone with a Boot Block would
  move the CONST segment and cause the reference to become invalid.  This
  ugly mechanism ensures the correct value for the pointer is compiled into
  code in the Boot Block where it will always stay correct.
  

DEPENDENCIES
  None

RETURN VALUE
  The value pointed to by the pointer passed in.

SIDE EFFECTS
  None

===========================================================================*/
extern word bd_get_word
(
  word *data_ptr  /* Pointer to the data word to be returned */
);



/*===========================================================================

FUNCTION BD_PUT_WORD

DESCRIPTION
  This function stores the word value passed in at the location pointed to by 
  the far pointer passed in.  Similar to TS_GET_DATA, a call to this
  function causes the segment and offset of the far pointer to be pushed
  on the stack.  This is necessary to avoid having the Microsoft 6.0 
  compiler store the segment portion of the far pointer in a constant in 
  the CONST segment.  If this function were not used to dereference the
  far pointer, a software upgrade to a phone with a Boot Block would
  move the CONST segment and cause the reference to become invalid.  This
  ugly mechanism ensures the correct value for the pointer is compiled into
  code in the Boot Block where it will always stay correct.
  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
void bd_put_word
(
  word *data_ptr,  /* Pointer to where the value should be put */
  word val         /* Value to put at the specified location   */
);

#endif /* BOOTDATA_H */
