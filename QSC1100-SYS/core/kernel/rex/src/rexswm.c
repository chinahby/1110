
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                       REX STACK WATERMARK


Not currently providing watermark update capability on system mode stacks 
such as irq_stack, sys_stack, and svc_stack.

Copyright (c) 1998,1999 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 2000,2001 by QUALCOMM Incorporated.  All Rights Reserved.
Copyright (c) 2002,2003 by QUALCOMM Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/
     
/*===========================================================================

                        EDIT HISTORY FOR MODULE

 $Header: //source/qcom/qct/core/kernel/rex/main/latest/rexswm.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/26/03   gr      Put in a typedef to get rid of a compiler warning. This is
                   a better solution than including comdef.h.
10/30/03   gr      Included comdef.h to fix compiler warning.
05/05/03   gr      Fixed featurization.
07/30/01   cr      Created module.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"

#ifdef FEATURE_SI_STACK_WM
#error code not present
#else

/* Put a bogus typedef here to mollify the compiler.
*/
typedef unsigned long rexswm_bogus_type;

#endif /* FEATURE_SI_STACK_WM */

