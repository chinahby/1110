#ifndef DS707_EPZID_UTIL_H
#define DS707_EPZID_UTIL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                               EP Z I D UTILITIES

GENERAL DESCRIPTION
  This file contains utility functions to operate on EPZID 

EXTERNALIZED FUNCTIONS



   Copyright (c) 2009 by QUALCOMM Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pzid.c_v   1.2   24 Jan 2003 16:09:22   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_epzid_util.h#1 $ $DateTime: 2009/05/30 06:04:02 $ $Author: msankar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/09   ms      Created the Initial Revision

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "ds707_epzid.h"


/*-------------------------------------------------------------------------- 
Three bits are used to represent the EPZID change. 

		    ------------------------
		   |  |  |  |  | X | Y | Z |
		   ------------------------
		   X - PZID CHANGE
		   Y - NID  CHANGE
		   Z - SID  CHANGE
--------------------------------------------------------------------------*/ 
typedef enum
{
  EPZID_NO_CHANGE              = 0x0,
  EPZID_SID_CHANGE             = 0x1,
  EPZID_NID_CHANGE             = 0x2,
  EPZID_SID_NID_CHANGE         = 0x3,
  EPZID_PZID_CHANGE            = 0x4,
  EPZID_PZID_SID_CHANGE        = 0x5,
  EPZID_PZID_NID_CHANGE        = 0x6,
  EPZID_PZID_SID_NID_CHANGE    = 0x7
} ds707_epzid_change_type;

/*===========================================================================
FUNCTION      DS707_PRINT_EPZID

DESCRIPTION   This function just prints the EPZID contents passed as arguement..
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void ds707_print_epzid( ds707_epzid_type *epzid);


/*===========================================================================
FUNCTION      DS707_EPZID_IS_EPZID_IN_LIST

DESCRIPTION   This function checks if the epzid is in epzid list

DEPENDENCIES  epzid_ptr is initialized

RETURN VALUE  Location in the list if already in the list, -1 otherwise

SIDE EFFECTS  None
===========================================================================*/
int ds707_epzid_is_epzid_in_list(ds707_epzid_type *epzid_ptr);
/*===========================================================================
FUNCTION      DS707_EPZID_COMPARE_EPZIDS

DESCRIPTION   Given two EPZIDs, this function compares them and returns
              if there is a change. Return value is an enumeration of 
              epzid_change_type

DEPENDENCIES  Valid input pointers

RETURN VALUE  ds707_epzid_change_type

SIDE EFFECTS  None
===========================================================================*/

ds707_epzid_change_type ds707_epzid_compare_epzids
(
  ds707_epzid_type *epzid_ptr1,
  ds707_epzid_type *epzid_ptr2
);
#endif /* FEATURE_DATA_IS707 */
#endif
