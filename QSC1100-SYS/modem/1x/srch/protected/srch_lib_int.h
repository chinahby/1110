#ifndef SRCH_LIB_INT_H
#define SRCH_LIB_INT_H
/*====*====*====*====*====*====*====*========*====*====*====*====*====*====*==*

        S E A R C H   T A S K  --  
        S E A R C H   L I B R A R Y   I N T E R N A L   H E A D E R

GENERAL DESCRIPTION
  This file contains library interfaces to the rest of searcher code 
  (outside of the library)
  
EXTERNALIZED FUNCTIONS
  None

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

      Copyright (c) 2004 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*=*/


/*=============================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/1x/srch/rel/1h08/protected/srch_lib_int.h#1 $

when       who      what, where, why
--------   ---      -----------------------------------------------------------
12/28/04   bt       implementation, first cut

=============================================================================*/


/*=============================================================================

                     INCLUDE FILES FOR MODULE

=============================================================================*/


/*===========================================================================

               SEARCHER LIBRARY FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION SRCH_LIB_INT_VER

DESCRIPTION   Returns the library version string.

DEPENDENCIES  None

RETURN VALUE  Pointer to the library version string.

SIDE EFFECTS  None

===========================================================================*/
extern const char* srch_lib_int_ver(void);

/*===========================================================================

FUNCTION SRCH_LIB_INT_CHECK_MDSP_VER

DESCRIPTION   Checks the MDSP Version at runtime and compares this to the 
              version the search library was compliled with.  An error 
              results if these versions do not match.

DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None

===========================================================================*/
extern void srch_lib_int_check_mdsp_ver(void);



#endif /* SRCH_LIB_INT_H */

