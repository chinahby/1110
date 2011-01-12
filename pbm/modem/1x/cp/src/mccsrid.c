 /*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                  M A I N   C O N T R O L   T A S K
                  S R I D   M A N A G E R

GENERAL DESCRIPTION
  This module is responsible for managing the SRIDs. 

EXTERNALIZED FUNCTIONS

Copyright (c) 2001-2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*===========================================================================
                      EDIT HISTORY FOR FILE

This section contains comments describing changes made to this file.
Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MSM5100/CP_REL_A/vcs/mccsrid.c_v   1.8   28 Jun 2002 12:50:26   lhang  $
$Header: //source/qcom/qct/modem/1x/cp/rel/1h08/src/mccsrid.c#1 $ $DateTime: 2009/05/29 03:54:56 $ $Author: prabhuk $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/11/07   pg      Lint Cleanup.
11/21/06   sb      Added support for VOIP-1X handoffs.
05/19/06   fc      Fixed bogus error message.
03/28/05   fc      Made FCS also available to Rel D.
12/10/04   sb      Fixed lint warnings.
05/19/04   sb      Made following merge official via integrate.
05/14/04   sb      Merged the following from msmshared:
                   03/05/04   bkm     Lint cleanup.
03/12/04   fc      Added support for Release C fast call setup.
03/05/04   bkm     Lint cleanup.
04/26/02   lh      Code review comments added.
04/04/02   lh      Fixed a INTLOCK/FREE bug, and use INTLOCK/FREE_SAV.
02/08/02   lh      Free srid only changes owner to NUll if that srid is active.
02/07/02   lh      Added mccsrid_free_all_owned_srids().
01/08/02   lh      Function name change.
08/28/01   lh      Bug fixes.
05/14/00   lh      Initial revision of the file.
===========================================================================*/


/*==========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "msg.h"
#include "err.h"
#include "mccsrid.h"

#ifdef FEATURE_IS2000_REL_A_SVD

/*===========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

===========================================================================*/


/* Enum for the state of an SR_ID */             
typedef enum
{
  SRID_AVAILABLE,       /* SRID is available for allocation */
  SRID_USED,            /* SRID is allocated */
  SRID_ACTIVE           /* SRID is used by an active service connection */
} mccsrid_srid_state_type;


/* Data Type for SR_ID objects */
typedef struct
{
  mccsrid_srid_state_type state;   /* The state of the SRID object */
  mccsrid_srid_owner_type owner;   /* The owner of the SRID object */
} mccsrid_srid_type;


/* SRID objects array */
mccsrid_srid_type sr_id[ MCCSRID_SRID_NUM_MAX ];  /* 7 items to begin with */


/*===========================================================================

FUNCTION mccsrid_initialize

DESCRIPTION
  Performs necessary initialization for the SR_ID objects.  This function
  is called upon power up initialization. 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  This function erase all the SR_ID information. Caller needs to make sure
  that it is safe to call this function. 
===========================================================================*/
void mccsrid_initialize(void)
{
  byte i;    /* sr_id array index */

  /* 
  ** SR_ID 0 is reserved for signaling. Set it to USED so we don't allocate
  ** it by mistake. And set the owner to reserved so that it should be
  ** always be allocated. 
  */
  sr_id[0].state = SRID_USED;
  sr_id[0].owner = SRID_OWNER_RESERVED;
  
  /* Initialize the rest of the SR_ID array. */
  for (i=1; i<MCCSRID_SRID_NUM_MAX; i++)
  {
    sr_id[i].owner = SRID_OWNER_NULL;
    sr_id[i].state = SRID_AVAILABLE;
  }
} /* mccsrid_initialize */

/*===========================================================================

FUNCTION mccsrid_print

DESCRIPTION
  Prints all the SR_ID to screen for diagnostic purpose. 

DEPENDENCIES
  None

RETURN VALUE
  None.

SIDE EFFECTS
  This function is not protected by INTLOCK. 
   
===========================================================================*/
void mccsrid_print(void)
{
  byte i;    /* sr_id array index */
#ifdef T_WINNT
#error code not present
#endif /* T_WINNT */

  for (i=1; i<MCCSRID_SRID_NUM_MAX; i++)
  {
#ifdef T_WINNT
#error code not present
#else
    MSG_MED("sr_id %d: owner %d, state %d", 
                 i, sr_id[i].owner, sr_id[i].state);
#endif /* T_WINNT */
  }
} /* mccsrid_print */

/*===========================================================================

FUNCTION mccsrid_available_srid

DESCRIPTION
  This function returns the lowest unused SR_ID number to the caller.  But
  it does not allocate the sr_id.

DEPENDENCIES
  None

RETURN VALUE
  SR_ID number [1, MCCSRID_SRID_NUM_MAX-1], if a SR_ID is available. 
  Returns MCCSRID_INVALID_SRID if none of the SRID is available.

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.
===========================================================================*/
uint8 mccsrid_available_srid
(
 void
)
{
  uint8    i;           /* index */
  uint8    srid = MCCSRID_INVALID_SRID;     /* return value */
  uint32  i_save;      /* Variable into which current flag */
  boolean found = FALSE;   /* flag used to stop the loop */

  for (i=1; i<MCCSRID_SRID_NUM_MAX && !found; i++)
  {
    /* -----------------------------------------------------------------
    ** This function can be called from multiple task. Protect the 
    ** critical section using INTLOCK
    ** ----------------------------------------------------------------- */

    INTLOCK_SAV(i_save);
    if (sr_id[i].state == SRID_AVAILABLE)
    {
      found = TRUE;
      srid = i;
    }
    INTFREE_SAV(i_save);
  }

  if (!found)
  {
    /* There must be an SR_ID leak */
    srid = MCCSRID_INVALID_SRID;
    MSG_ERROR("Out of available sr_id!", 0, 0, 0);
  }

  return srid;
} /* mccsrid_available_srid */

/*===========================================================================

FUNCTION mccsrid_allocate_srid

DESCRIPTION
  This function returns the lowest unused SR_ID number to the caller.

DEPENDENCIES
  None

RETURN VALUE
  SR_ID number [1, MCCSRID_SRID_NUM_MAX-1], if a SR_ID is available. 
  Returns MCCSRID_INVALID_SRID if none of the SRID is available.

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.
===========================================================================*/
byte mccsrid_allocate_srid
(
  mccsrid_srid_owner_type owner
    /* Owner ID of the allocation request */
)
{
  byte    i;           /* index */
  byte    srid = MCCSRID_INVALID_SRID;     /* return value */
  uint32  i_save;      /* Variable into which current flag */
  boolean found = FALSE;   /* flag used to stop the loop */

  for (i=1; i<MCCSRID_SRID_NUM_MAX && !found; i++)
  {
    /* -----------------------------------------------------------------
    ** This function can be called from multiple task. Protect the 
    ** critical section using INTLOCK
    ** ----------------------------------------------------------------- */
    
    INTLOCK_SAV(i_save);
    if(sr_id[i].state == SRID_AVAILABLE)
    {
      sr_id[i].state = SRID_USED;       /* Set state to USED */
      sr_id[i].owner = owner;           /* Set the owner */
      found = TRUE;
      srid = i;
    }
    INTFREE_SAV(i_save);
  }

  if(!found) 
  {
    /* There must be an SR_ID leak */
    srid = MCCSRID_INVALID_SRID;
    MSG_ERROR("Out of available sr_id!", 0, 0, 0);
  }

  mccsrid_print();

  return srid;
} /* mccsrid_allocate_srid */

#if defined(FEATURE_IS2000_REL_C_FCS) || defined(FEATURE_IS2000_REL_D)
/*===========================================================================

FUNCTION MCCSRID_ALLOCATE_SPECIFIC_SRID

DESCRIPTION
  This function marks specified SR_ID as used if it is available  

DEPENDENCIES
  None

RETURN VALUE
  Specified SR_ID number if it is available.
  Otherwise, returns MCCSRID_INVALID_SRID. 

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.

===========================================================================*/
byte mccsrid_allocate_specific_srid
(
  byte id,
    /* SR_ID number */
  mccsrid_srid_owner_type owner
    /* Owner ID of the allocation request */
)
{
  byte    srid = MCCSRID_INVALID_SRID;     /* return value */
  uint32  i_save;      /* Variable into which current flag */

  if ((id > 0) && (id < MCCSRID_SRID_NUM_MAX))
  {
    /* -----------------------------------------------------------------
    ** This function can be called from multiple task. Protect the 
    ** critical section using INTLOCK
    ** ----------------------------------------------------------------- */
    
    INTLOCK_SAV(i_save);
    if (sr_id[id].state == SRID_AVAILABLE)
    {
      sr_id[id].state = SRID_USED;       /* Set state to USED */
      sr_id[id].owner = owner;           /* Set the owner */
      srid = id;
    }
    else
    {
      MSG_HIGH("sr_id %d: owner %d, state %d not available", 
        id, sr_id[id].owner, sr_id[id].state);
    }
    INTFREE_SAV(i_save);
  }
  else 
  {
    MSG_ERROR("Invalid sr_id %d", id, 0, 0);
  }

  return srid;

} /* mccsrid_allocate_specific_srid */
#endif /* FEATURE_IS2000_REL_C_FCS || FEATURE_IS2000_REL_D */

/*===========================================================================

FUNCTION mccsrid_check_specific_srid

DESCRIPTION
  This function is used to verify if an SR_ID is still reserved for the same
  owner. This function is called by MC when it receives an SR_ID from DS.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the SI_ID is used by the same owner. 

SIDE EFFECTS
===========================================================================*/
boolean mccsrid_check_specific_srid
(
  byte id 
    /* SR_ID number to check */
)
{
  boolean ret_val = TRUE;  /* Return value */


  /*
  ** There is no INTLOCK required in this function, because
  ** the only other entity that can change SRID state is DS, and it thinks
  ** an SR_ID is reserved for them already.
  */

  if ((id > 0) && (id < MCCSRID_SRID_NUM_MAX))
  {
    if (sr_id[id].state == SRID_ACTIVE)
    {
      MSG_HIGH("SR_ID already active.", 0, 0, 0);
      ret_val = FALSE;
    }
  }
  else
  {
    /* Invalid SR_ID range */
    MSG_ERROR( "Invalid SR_ID range to check!", 0, 0, 0); 
    ret_val = FALSE;
  }

  return ret_val;
} /* mccsrid_check_specific_srid */



/* <EJECT> */
/*===========================================================================

FUNCTION mccsrid_free_srid

DESCRIPTION
  This function deallocates an SRID owned by the owner.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.
===========================================================================*/
void mccsrid_free_srid
(
  byte id, 
    /* SR_ID number to free */
  mccsrid_srid_owner_type owner
    /* Current owner of the SR_ID to free */
)
{
  boolean free_from_non_owner = FALSE;
  uint32  i_save;      /* Variable into which current flag */


  MSG_HIGH("Free SRID %d requested by %d", id, owner, 0);

  if ( (id > 0) && (id < MCCSRID_SRID_NUM_MAX))
  {
    /*
    ** The owner can change after DS calling this function. Critical 
    ** section is protected by INTLOCK.
    */

    INTLOCK_SAV(i_save);

    if (sr_id[id].owner == owner)
    {
      /* Free SR_ID */
      sr_id[id].owner = SRID_OWNER_NULL;
      
      /* If SR_ID is reserved by this owner, free will cause
      ** that SR_ID to be available. If it is currently active,
      ** then there is only the onwership change, no state change.
      ** e.g., if the sr_id is in the current SCR, but user wants
      ** to release it. Owner's call to this function will give
      ** the ownership to BS, but the SR_ID is not usable till
      ** BS release the call.
      */
      if ( sr_id[id].state == SRID_USED )
      {
        sr_id[id].state = SRID_AVAILABLE;
      }
    }
    else
    {
      free_from_non_owner = TRUE;
    }

    INTFREE_SAV(i_save);

    if (free_from_non_owner)
    {
      MSG_MED("Free attempt from non-owner ", 0, 0, 0);
    }
  }
  else if ( id > 0 )
  {
    MSG_ERROR("SR_ID %d out of range, from %d!", id, owner, 0);
  }

  mccsrid_print();

} /* mccsrid_free_srid */

/* <EJECT> */
/*===========================================================================

FUNCTION mccsrid_free_all_owned_srids

DESCRIPTION
  This function deallocates all SRID owned by the owner.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.
===========================================================================*/
void mccsrid_free_all_owned_srids
(
  mccsrid_srid_owner_type owner
    /* Current owner of the SR_ID to free */
)
{
  int id;
  uint32  i_save;      /* Variable into which current flag */

  for (id = 1; id<MCCSRID_SRID_NUM_MAX; id++)
  {
    /*
    ** The owner can change after DS calling this function. Critical 
    ** section is protected by INTLOCK.
    */
    INTLOCK_SAV(i_save);

    if (sr_id[id].owner == owner)
    {
      /* Free SR_ID */
      sr_id[id].owner = SRID_OWNER_NULL;
      
      if ( sr_id[id].state == SRID_USED )
      {
        sr_id[id].state = SRID_AVAILABLE;
      }
    }

    INTFREE_SAV(i_save);
  }

  MSG_HIGH("All SR_ID owned by %d freed!", owner, 0, 0);

  mccsrid_print();

} /* mccsrid_free_all_owned_srids */



/* <EJECT> */
/*===========================================================================

FUNCTION mccsrid_update_srid

DESCRIPTION
  This function updates SR_ID from the connection record.

DEPENDENCIES
  SR_ID range checking has been performed by the caller.
  cfg_ptr should not be a NULL pointer.

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.
===========================================================================*/
void mccsrid_update_srid
(
  const caii_srv_cfg_type  *cfg_ptr 
    /* Ptr to the service configuration record for this update */
)
{
  byte i;  /* iteration index */
  uint32  i_save;      /* Variable into which current flag */

  /* Caller should have checked this pointer */
  if (cfg_ptr == NULL)
  {
    ERR_FATAL("cfg_ptr is NULL", 0,0,0);
    /* return; */
  }

  INTLOCK_SAV(i_save);

  /* 
  ** First update the state of all the srid objects. 
  ** This procedure is the same as releaes, i.e., it 
  ** is treat like the old SCR is released
  */
  mccsrid_release();

  
  /* Then update those SRID with the connection record */

  for (i=0; i<cfg_ptr->num_con; i++)
  {
    /* Range checking should have been done. But just check again
    ** for safty.
    */
    if ( (cfg_ptr->con[i].sr_id > 0) && 
         (cfg_ptr->con[i].sr_id < MCCSRID_SRID_NUM_MAX)
       )
    {
      sr_id[cfg_ptr->con[i].sr_id].state = SRID_ACTIVE;
    }
    else
    {
      MSG_ERROR("SR_ID %d OUT OF RANGE!", cfg_ptr->con[i].sr_id, 0, 0);
    }

  }

  INTFREE_SAV(i_save);

  MSG_HIGH("Updating SR_IDs", 0, 0, 0);
  mccsrid_print();

} /* mccsrid_update_srid */


/* <EJECT> */
/*===========================================================================

FUNCTION mccsrid_release

DESCRIPTION
  This function is called when LL3 exits TC state. SR_ID manager changes 
  all the active SR_ID to available if the owner is NULL, otherwise, SR_ID
  manager changes active SR_ID to used if owner is not NULL. Notice that
  this function does not free an SRID if it has an owner.


DEPENDENCIES
  None. 

RETURN VALUE
  None

SIDE EFFECTS
  Interrupt is locked while accessing the SR_ID array.
===========================================================================*/
void mccsrid_release(void)
{
  byte i;  /* index for iteration */
  uint32  i_save;      /* Variable into which current flag */

  for (i=1; i<MCCSRID_SRID_NUM_MAX; i++)
  {
    /* Make sure one array element access is atomic. */
    INTLOCK_SAV(i_save);

    if (sr_id[i].state == SRID_ACTIVE) 
    {
      /* Release only the active sr_id */
      if (sr_id[i].owner == SRID_OWNER_NULL)
      {
        sr_id[i].state = SRID_AVAILABLE;
      }
      else
      {
        sr_id[i].state = SRID_USED;
      }
    }

    INTFREE_SAV(i_save);
  }

} /* mccsrid_release */

#endif /* FEATURE_IS2000_REL_A_SVD */

