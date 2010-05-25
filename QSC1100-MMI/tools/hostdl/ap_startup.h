#ifndef AP_STARTUP_H
#define AP_STARTUP_H

/**====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
;
; GENERAL DESCRIPTION
;	This header file consists the common definitions used during the boot-up
;	time.
;
; EXTERNALIZED FUNCTIONS
;   None. 
;
; INITIALIZATION AND SEQUENCING REQUIREMENTS
;	None
;
;  Copyright (c) 1997, 1998, 1999, 2000, 2001, 2002 by QUALCOMM Incorporated.  
   All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*============================================================================
                          EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/tools/hostdl/HOSTDL.14.00/common/ap_startup.h#1 $
  Integrated from P402:  //depot/common-sw/armprg/armprg-9.0/common/ap_startup.h#1 
  
 when       who     what, where, why
 --------   ---     --------------------------------------------------------
 01/16/02   drh     Initial version of armprg to support the software 
                    downloads over UART on a single MSM target built in
                    the DMSS directory using DMSS header files.  Based on
                    code cleanup effort by lnr 11/2001.

============================================================================*/

//--------------------------------------------------------------------------
// Defines
//--------------------------------------------------------------------------

// STACK DEFINITIONS
//
//  These stacks are defined to be the same size as they are defined
//  in startup.s.  However, where they are used, they are used
//  with divide by 4 and are defined to be dword, not byte.  
//  This prevents unaligned access.
//
//  Used as flashprg stack (FLSH_Stack_Size). Allocate 16K for the    
//  stack.  A largish stack is required because      
//  the packet buffer will be allocated on the stack.

#define SVC_Stack_Size   0x4000

//  Define other stacks, these can probably be combined to use
//  the same stack

#define IRQ_Stack_Size   0x800
#define Abort_Stack_Size 0x100
#define Undef_Stack_Size 0x100

#endif /* AP_STARTUP_H */

