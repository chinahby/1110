/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

             B O O T   D A T A   R O U T I N E S

GENERAL DESCRIPTION
  This module contains functions for accessing static data from the Boot
  Block.

REFERENCES
  None

EXTERNALIZED FUNCTIONS
  bd_get_word - Gets a word from the Boot Block's initialized data segment.

  bd_put_word - Puts a word to the Boot Block's initialized data segment.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

Copyright (c) 2004 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

$PVCSPath:
$Header: //depot/asic/msm6275/drivers/boot/bootdata.c#1

    This section contains comments describing changes made to the module.
    Notice that changes are listed in reverse chronological order.


when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/01/04   tkuo    Ported from MSM6250.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"             /* Definitions for byte, word, etc.     */


/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

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
word bd_get_word
(
  word *data_ptr  /* Pointer to the data word to be returned */
)
{
  return (*data_ptr);
}


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
)
{
   *data_ptr = val;
}
