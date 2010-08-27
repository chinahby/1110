#ifndef PROFDIAG_H
#define PROFDIAG_H
/*===========================================================================

                         P R O F   D I A G    
                      
DESCRIPTION


GENERAL DESCRIPTION

  
EXTERNALIZED FUNCTIONS

  profdiag_init() - Registers the profiling diag command handlers with
                    Diag.

REGIONAL FUNCTIONS

  None

INITIALIZATION AND SEQUENCING REQUIREMENTS

  Must call profdiag_init() before commands will be processed by profdiag.
  

  Copyright (c) 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
  
===========================================================================*/

/*===========================================================================

                      EDIT HISTORY FOR FILE

$Header: //depot/asic/msm6800B/services/profdiag/profdiag.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/07/05   jwh     Moved internal info to profdiagi.h
12/07/04   zz      Added QGL DSP profiling and using FEATURE_QGL_STATISTICS
12/02/04   jwh     FEATURE-ized AHB statistics.
11/20/04   grl     Created

===========================================================================*/

/*==========================================================================

                    FUNCTION PROTOTYPES USED IN THE MODULE

This section contains function prototypes for the functions used in this
module.

==========================================================================*/


/*===========================================================================

FUNCTION PROFDIAG_INIT

DESCRIPTION
  Register the diag packet function dispatch table.
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/

#ifndef __cplusplus

extern void profdiag_init (void);

#endif

#endif /* PROFDIAG_H */
