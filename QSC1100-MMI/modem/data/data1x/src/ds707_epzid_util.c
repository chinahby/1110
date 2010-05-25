/*===========================================================================
                     D A T A   S E R V I C E S   E P Z I D

                                  U T I L 

GENERAL DESCRIPTION
  This file contains utility functions to operate on EPZID 

EXTERNALIZED FUNCTIONS
  ds707_print_epzid()
    Function to print the contents of epzid
  ds707_epzid_compare_epzids()
    Function to compare two epzids
  ds707_epzid_is_epzid_in_list()
    Function to check the presence of passed EPZID in the list.

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2009 by QUALCOMM Incorporated.All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_epzid.c_v   1.2   24 Jan 2003 16:09:22   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_epzid_util.c#2 $ $DateTime: 2009/06/10 22:01:08 $ $Author: msankar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/05/09   ms   Initial version 
===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#ifdef FEATURE_DATA_EPZID
#include "msg.h"
#include "assert.h"
#include "ds707_epzid_util.h"



/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/* Type of the currently active Enhanced PZID */
byte ds707_active_epzid_type; 

/*===========================================================================
FUNCTION      DS707_PRINT_EPZID

DESCRIPTION   This function just prints the EPZID contents passed as arguement..
  
DEPENDENCIES  None

RETURN VALUE  None

SIDE EFFECTS  None
===========================================================================*/

void ds707_print_epzid( ds707_epzid_type *epzid)
{
  MSG_HIGH("\n PZID = %d ",epzid->pzid,0,0);
  MSG_HIGH("\n NID = %d ",epzid->nid,0,0);
  MSG_HIGH("\n SID = %d ",epzid->sid,0,0);
  return;
}


/*===========================================================================
FUNCTION      DS707_EPZID_IS_EPZID_IN_LIST

DESCRIPTION   This function checks if the epzid is in epzid list

DEPENDENCIES  epzid_ptr is initialized

RETURN VALUE  Location in the list if already in the list, -1 otherwise

SIDE EFFECTS  None
===========================================================================*/
int ds707_epzid_is_epzid_in_list
(
  ds707_epzid_type *epzid_ptr
)
{
  int  list_location;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  if(epzid_ptr == NULL)
  {
    MSG_ERROR("NULL pointer passed ",0,0,0);
    ASSERT(0);
    return -1;
  }
  /*-------------------------------------------------------------------------
    Go through the EPZID list to see if new pzid is already on the list.
    If EPZID was on the list, i would be its index in the EPZID array.
  -------------------------------------------------------------------------*/
  for (list_location = 0; list_location < ds707_epzid_oldest_epzid; list_location++) 
  {
    if (ds707_epzid_compare_epzids(
            &ds707_epzid_list[list_location],
            epzid_ptr) == EPZID_NO_CHANGE)
    {
      break;
    }
  }

  /* If the the epzid is not in the list, return invalid location */
  if (list_location == ds707_epzid_oldest_epzid)
  {
    list_location = -1;
  }
  return list_location;
}/* ds707_epzid_is_epzid_in_list() */


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
)
{
  ds707_epzid_change_type epzid_change = EPZID_NO_CHANGE;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  if((epzid_ptr1 == NULL) || (epzid_ptr2 == NULL))
  {
    MSG_ERROR("NULL pointer passed : epzid_ptr1 = %x, epzid_ptr2 = %x",epzid_ptr1, epzid_ptr2,0);
    ASSERT(0);
    return epzid_change;
  }

  
  /*-------------------------------------------------------------------------- 
   Three bits are used to represent the EPZID change. 

      ----------------------------
     |   |   |   |   | X | Y | Z |
     ----------------------------
     X - PZID CHANGE
     Y - NID  CHANGE
     Z - SID  CHANGE
  --------------------------------------------------------------------------*/ 

  if (epzid_ptr1->pzid != epzid_ptr2->pzid)
  {
    epzid_change |= EPZID_PZID_CHANGE; 
  } 
 
  if ((ds707_active_epzid_type != NV_EPZID_PZID) &&
      (epzid_ptr1->sid != epzid_ptr2->sid))
  {
    epzid_change |= EPZID_SID_CHANGE; 
  }
 
  if ((ds707_active_epzid_type == NV_EPZID_PZID_SID_NID) &&
      (epzid_ptr1->nid != epzid_ptr2->nid))
  {
    epzid_change |= EPZID_NID_CHANGE; 
  } 

  return epzid_change;

}
#endif /* FEATURE_DATA_EPZID */
