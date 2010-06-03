#ifndef OSBL_MC_H
#define OSBL_MC_H

/*===========================================================================

               O S  S e c o n d a r y   B o o t   L o a d e r
                         M a i n  C o n t r o l
                          H e a d e r  F i l e

GENERAL DESCRIPTION
  This file contains the main control for QCSBL execution.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None
  
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
============================================================================*/

/*===========================================================================

                           EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/msmshared/secboot2/osbl/1_0/osbl_mc.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/09/07   ly      Create
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "boot_dbl_if.h"

/*===========================================================================
 
                              DEFINES

===========================================================================*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* The OSBL procedure type */
typedef void (*osbl_procedure_func_type)
(
  bl_shared_data_type *bl_shared_data
);

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION  OSBL_DO_PROCEDURES

DESCRIPTION
   Processes an array of OSBL procedures.
        
DEPENDENCIES
  None
  
RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
extern void osbl_do_procedures
(
  bl_shared_data_type *bl_shared_data,
  osbl_procedure_func_type *procs
);

/*===========================================================================

FUNCTION  OSBL_MAIN_CTL

DESCRIPTION
   The OSBL Main Controller
        
DEPENDENCIES
  None
  
RETURN VALUE
  It will never return.

SIDE EFFECTS
  None

===========================================================================*/
extern void osbl_main_ctl
(
    boot_dbl_if_shared_info_type *dbl_shared_info_ptr
);


#endif  /* OSBL_MC_H */

