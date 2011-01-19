/*===========================================================================

                        D S M I P _ P D S N _ D B . C

DESCRIPTION

  File containing the data structure and access methods for maintianing the
  SID/NID/PZID -> PDSN IP address mappings.

Copyright (c) 2001, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/
/*===========================================================================

                            EDIT HISTORY FOR FILE

  $PVCSPath: O:/src/asw/COMMON/vcs/dsmip_pdsn_db.c_v   1.2   07 Aug 2001 21:59:28   jeffd  $
  $Header: //source/qcom/qct/modem/data/1x/mip/main/lite/src/dsmip_pdsn_db.c#2 $ $DateTime: 2007/11/22 23:33:56 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
12/07/05    as/kvd Code cleanup due to change in CP APIs
07/27/01    mvl    Changed pdsn_db_get_drs() to get_kind() to reflect new
                   functionality that has been added to funciton to allow
                   differentiation of different PDSN types (same, old, new)
07/10/01    mvl    Added pdsn_db_get_current() and pdsn_db_is_current()
07/05/01    mvl    Created module
===========================================================================*/

/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DS_MOBILE_IP

#include "assert.h"
#include "dsmip_pdsn_db.h"
#include "mc.h"
#include "mccsyobj.h"

#include <memory.h>

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            INTERNAL DEFINITIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/

#define PZID_DB_SIZE 15

/*---------------------------------------------------------------------------
  The arrays that actually make up the mappings - the entries for a given
  index in both arrays correspond to one another.  The mappins are from
  SID/NID/PZID -> PDSN IP address -> "timestamp".  The timestamp value is
  a counter indicating pdsndbi_relative time.

  The tables are used maintained using the least recently used
---------------------------------------------------------------------------*/
LOCAL uint32 pdsndbi_sid_nid_pzid_entries[PZID_DB_SIZE];
LOCAL uint32 pdsndbi_pdsn_ip_addr_entries[PZID_DB_SIZE];
LOCAL uint16 pdsndbi_relative_timestamp[PZID_DB_SIZE];
LOCAL uint8  pdsndbi_curr_pdsn_index = 0;

/*---------------------------------------------------------------------------
  Is the DRS algorith enabled?  By default it is
---------------------------------------------------------------------------*/
LOCAL boolean  pdsndbi_do_drs_alg = TRUE;

/*---------------------------------------------------------------------------
  Typedef that places the PZID, NID and SID into something that will fit into
  a uint32.
---------------------------------------------------------------------------*/
typedef struct
{
  uint8 sid;
  uint8 nid;
  uint8 pzid;
  uint8 _pad;
} snpzid_type;

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                            FORWARD DECLARATIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
INLINE void pdsndbi_get_ids
(
  snpzid_type *snpzid_ptr
);

/*= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =
                        EXTERNAL FUNCTION DEFINTIONS
= = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = = =*/
/*===========================================================================
FUNCTION PDSN_DB_INIT()

DESCRIPTION
  This function initializes the pdsn database.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pdsn_db_init
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  memset(pdsndbi_pdsn_ip_addr_entries,
         0,
         sizeof(pdsndbi_pdsn_ip_addr_entries));
  memset(pdsndbi_relative_timestamp, 0, sizeof(pdsndbi_relative_timestamp));

} /* pdsn_db_init() */



/*===========================================================================
FUNCTION PDSN_DB_ENABLE()

DESCRIPTION
  This function is used to enable and disable the DRS algorithm

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pdsn_db_enable
(
  boolean enable
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  pdsndbi_do_drs_alg = enable;  

} /* pdsn_db_enable() */



/*===========================================================================
FUNCTION PDSN_DB_PUT()

DESCRIPTION
  This function creates a new pdsn database entry.  If the DB is full the
  oldest entry will be replaced.  The oldest entry is the one with the
  smallest pdsndbi_relative time entry.

PARAMETERS
  ip_addr: the PDSNs IP address

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pdsn_db_put
(
  uint32 ip_addr
)
{
  uint32 tmp_snpzid = 0;
  snpzid_type *snpzid_ptr = (snpzid_type*)(&tmp_snpzid);
  static uint16 curr_relative_time = 0;
  uint8 loop, insertion_index = 0;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If the algorithm is disabled or the current PREV is not 6 return
  -------------------------------------------------------------------------*/
  if(pdsndbi_do_drs_alg == FALSE ||
     mc_get_p_rev_in_use() != 6)
  {
    return;
  }

  /*-------------------------------------------------------------------------
    Get the IDs and place then into the temp storage
  -------------------------------------------------------------------------*/
  pdsndbi_get_ids(snpzid_ptr);

  /*-------------------------------------------------------------------------
    Is there an exact match?  If so then do nothing
  -------------------------------------------------------------------------*/
  if(tmp_snpzid == pdsndbi_sid_nid_pzid_entries[pdsndbi_curr_pdsn_index] &&
     ip_addr    == pdsndbi_pdsn_ip_addr_entries[pdsndbi_curr_pdsn_index])
  {
    /* ignore call */
    MSG_LOW("found exact match for entry - ignoring\n", 0, 0, 0);
    return;
  }

  /*-------------------------------------------------------------------------
    Loop through all of the table entries to determine where to insert entry
  -------------------------------------------------------------------------*/
  for(loop = 0; loop < PZID_DB_SIZE; loop += 1)
  {
    /*-----------------------------------------------------------------------
      Optimization: if the IP address is zero the entry is empty so use it.
    -----------------------------------------------------------------------*/
    if(pdsndbi_pdsn_ip_addr_entries[loop] == 0)
    {
      MSG_LOW("Index %d empty, inserting here.\n", 0, 0, 0);
      insertion_index = loop;
      break;
    }

    /*-----------------------------------------------------------------------
      Have we been at this PCF before?  If so then this is the insertion
      point.  It is not an exact match because that would have been caught at
      the top of the function.
    -----------------------------------------------------------------------*/
    if(tmp_snpzid == pdsndbi_sid_nid_pzid_entries[loop])
    {
      MSG_LOW("found match @ row %d for sid/nid/pzid\n", loop, 0, 0);
      insertion_index = loop;
      break;
    }

    /*-----------------------------------------------------------------------
      figure out the entry with the oldest timestamp as that is the default
      insertion point.
    -----------------------------------------------------------------------*/
    if(pdsndbi_relative_timestamp[loop] <=
       pdsndbi_relative_timestamp[insertion_index])
    {
      insertion_index = loop;
    }
  } /* for(all DB rows) */

  MSG_LOW("Inserting @ row %d\n", insertion_index, 0, 0);
  pdsndbi_curr_pdsn_index = insertion_index;
  pdsndbi_sid_nid_pzid_entries[insertion_index] = tmp_snpzid;
  pdsndbi_pdsn_ip_addr_entries[insertion_index] = ip_addr;
  pdsndbi_relative_timestamp[insertion_index]   = curr_relative_time;
  curr_relative_time += 1;

} /* pdsn_db_put() */



/*===========================================================================
FUNCTION PDSN_DB_GET_KIND()

DESCRIPTION
  This function returns an indication of whether the PDSN correspinding to
  the current SID/NID/PZID is new, old or the same as the current one

PARAMETERS
  None

RETURN VALUE
  pdsn_kind_enum_type: returns the type of network being visited based on the 
    previous PDSN database entry.
    
DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
pdsn_db_kind_enum_type  pdsn_db_get_kind
( 
  void
)
{
  uint32 tmp_snpzid = 0;
  snpzid_type *snpzid_ptr = (snpzid_type*)(&tmp_snpzid);
  uint8 loop;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  /*-------------------------------------------------------------------------
    If the algorithm is disabled or the current PREV is not 6 return that it
    is the same PDSN
  -------------------------------------------------------------------------*/
  if(pdsndbi_do_drs_alg == FALSE ||
     mc_get_p_rev_in_use() != 6)
  {
    return DB_DISABLED;
  }

  /*-------------------------------------------------------------------------
    Get the IDs and place then into the temp storage
  -------------------------------------------------------------------------*/
  pdsndbi_get_ids(snpzid_ptr);

  /*-------------------------------------------------------------------------
    Loop through all of the table entries to see if there is a match
  -------------------------------------------------------------------------*/
  for(loop = 0;
      loop < PZID_DB_SIZE && pdsndbi_pdsn_ip_addr_entries[loop] != 0;
      loop += 1)
  {
    if(tmp_snpzid == pdsndbi_sid_nid_pzid_entries[loop])
    {
      /*---------------------------------------------------------------------
        Is the mobile where it was before?
      ---------------------------------------------------------------------*/
      if(loop == pdsndbi_curr_pdsn_index)
      {
        MSG_MED("Are still at same PCF as before", 0, 0, 0);
        return SAME_PDSN_SAME_PCF;
      }

      /*---------------------------------------------------------------------
        The PCF has changed, has the PDSN changed?
      ---------------------------------------------------------------------*/
      else if(pdsndbi_pdsn_ip_addr_entries[loop] ==
              pdsndbi_pdsn_ip_addr_entries[pdsndbi_curr_pdsn_index])
      {
        MSG_MED("Same PDSN but diff PCF", 0, 0, 0);
        return SAME_PDSN_DIFF_PCF;
      }

      /*---------------------------------------------------------------------
        The PCF has been visited and has a different PDSN
      ---------------------------------------------------------------------*/
      else
      {
        MSG_MED("Previously visited PDSN", 0, 0, 0);
        return VISITED_PDSN;
      }
    }
  } /* for(all indexes) */

  /*-------------------------------------------------------------------------
    We have not been at this PCF before - and so have no PDSN mapping for it
  -------------------------------------------------------------------------*/
  MSG_MED("Unknown PDSN", 0, 0, 0);
  return UNKNOWN_PDSN;

} /* pdsn_db_get_kind() */



/*===========================================================================
FUNCTION PDSN_DB_GET_CURRENT()

DESCRIPTION
  This function returns the IP address of the current PDSN

PARAMETERS
  None

RETURN VALUE 
  uint32 - IP address of the current PDSN

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
uint32 pdsn_db_get_current
(
  void
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  return pdsndbi_pdsn_ip_addr_entries[pdsndbi_curr_pdsn_index];

} /* pdsn_db_get_current() */



/*===========================================================================
FUNCTION PDSN_DB_IS_CURRENT()

DESCRIPTION
  This function returns whether the PDSN address passed in corresponds to the
  PDSN for the PCF where we find ourselves.

PARAMETERS
  pdsn_ip: the IP address of the PDSN

RETURN VALUE
  boolean:
    TRUE:  if the PDSN address matches for the PCF where the mobile is
           currently.
    FALSE: if there is no match for the PDSN address passed in.

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
boolean pdsn_db_is_current
(
  uint32 pdsn_ip
)
{
  uint32 tmp_snpzid = 0;
  snpzid_type *snpzid_ptr = (snpzid_type*)(&tmp_snpzid);
  uint8 loop;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  /*-------------------------------------------------------------------------
    Get the IDs and place then into the temp storage
  -------------------------------------------------------------------------*/
  pdsndbi_get_ids(snpzid_ptr);

  /*-------------------------------------------------------------------------
    Loop through all of the table entries to see if there is a match - if the
    PDSN IP address for that entry is zero then we have reached the end of
    the valid entries.
  -------------------------------------------------------------------------*/
  for(loop = 0;
      loop < PZID_DB_SIZE && pdsndbi_pdsn_ip_addr_entries[loop] != 0;
      loop += 1)
  {
    if(tmp_snpzid == pdsndbi_sid_nid_pzid_entries[loop] &&
       pdsn_ip    == pdsndbi_pdsn_ip_addr_entries[loop])
    {
      MSG_MED("PDSN %x at this PCF", pdsn_ip, 0, 0);
      return TRUE;
    }
  }

  MSG_MED("PDSN %x is not at this PCF", pdsn_ip, 0, 0);
  return FALSE;

} /* pdsn_db_is_current() */



#ifndef T_ARM
#include <stdio.h>
/*===========================================================================
FUNCTION PDSN_DB_PRINT()

DESCRIPTION
  This function prints the contents of the PDSN database.

PARAMETERS
  None

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
void pdsn_db_print
(
  void
)
{
  snpzid_type *snpzid_ptr;
  uint8 loop;

  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  printf("\n=> Table entries\n");
  for(loop = 0; loop < PZID_DB_SIZE; loop += 1)
  {
    if(pdsndbi_pdsn_ip_addr_entries[loop] == 0)
    {
      continue;
    }

    snpzid_ptr = (snpzid_type*)(pdsndbi_sid_nid_pzid_entries + loop);
    printf("%2d (%3d, %3d, %3d => %08x @ %3d)\n",
           loop,
           snpzid_ptr->sid,
           snpzid_ptr->nid,
           snpzid_ptr->pzid,
           pdsndbi_pdsn_ip_addr_entries[loop],
           pdsndbi_relative_timestamp[loop]);
  }

} /* pdsn_db_print() */

#endif /* T_ARM */


/*=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
                             INTERNAL FUNCTIONS
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=*/
/*===========================================================================
FUNCTION PDSNDBI_GET_IDS()

DESCRIPTION
  This function gets the current SID, NID and PZID and places them into the
  data structure passed in.

PARAMETERS
  snpzid_ptr: pointer to the sid/nid/pzid data structure.

RETURN VALUE
  None

DEPENDENCIES
  None

SIDE EFFECTS
  None
===========================================================================*/
INLINE void pdsndbi_get_ids
(
  snpzid_type *snpzid_ptr
)
{
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  snpzid_ptr->sid  = mccsyobj_get_sid();
  snpzid_ptr->nid  = mccsyobj_get_nid();
  snpzid_ptr->pzid = mccsyobj_get_pzid();

} /* pdsndbi_get_ids() */                

#endif  /* FEATURE_DS_MOBILE_IP */
