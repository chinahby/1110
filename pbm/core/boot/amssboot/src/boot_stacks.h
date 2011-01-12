#ifndef   _BOOT_STACKS_H
#define   _BOOT_STACKS_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

               B O A R D     S U P P O R T   P A C K A G E   H E A D E R

 GENERAL DESCRIPTION
   This file contains constant definitions which belong to AMSS BOOT CODE.
   It currently contains the following definitions:
   1.  Stack sizes definitions

 Copyright (c) 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
*===========================================================================

                           EDIT HISTORY FOR FILE

his section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath:$
$Header: //depot/asic/qsc1100/drivers/boot/boot_stacks.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/19/04   tkuo    Created for MSM6275 non-secured boot.

============================================================================
 
                           INCLUDE FILES

===========================================================================*/

/*  Stack sizes.               */
#define SVC_Stack_Size             0x800   /* => 832 bytes */
#define IRQ_Stack_Size             0x900   /* => 268 words */
#define FIQ_Stack_Size             0x400   
#define Abort_Stack_Size           0x190   /* => 200 words */
#define Undef_Stack_Size           0x100   /* => 128 words */
#define Idle_Stack_Size            0x100
#define Sys_Stack_Size             0xC00

/* Special return value to denote that RAM is initialized */
#define RAM_Initialized            0xffffffff
#define BOOT_STACK_PTR             0x58020000
#define Boot_MDSP_Stack_Pointer    0x50002000

#define RESET_REGISTER_FLAG 0x12345678

#ifndef _ARM_ASM_
/*===========================================================================

FUNCTION  BOOT_STACK_INITIALIZE

DESCRIPTION
  This function initialize the stack for different modes.
DEPENDENCIES
  None
  
RETURN VALUE
  None.
SIDE EFFECTS
  None

===========================================================================*/
void boot_stack_initialize(void);

#endif /* _ARM_ASM_ */
#endif /* _BOOT_STACKS_H */
