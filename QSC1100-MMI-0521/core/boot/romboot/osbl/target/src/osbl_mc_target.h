#ifndef OSBL_MC_TARGET_H
#define OSBL_MC_TARGET_H

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

$Header: //depot/asic/qsc1100/romboot/osbl/osbl_mc_target.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/09/07   ly      Create
============================================================================*/

/*===========================================================================
 
                           INCLUDE FILES

===========================================================================*/
#include "osbl_mc.h"

/*===========================================================================
 
                              DEFINES

===========================================================================*/

/*===========================================================================

                      PUBLIC DATA DECLARATIONS

===========================================================================*/

/* Main list of OSBL procedures */
extern osbl_procedure_func_type osbl_main_procs[];

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

#endif  /* OSBL_MC_TARGET_H */

