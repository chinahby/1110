#ifndef DIAGNV_H
#define DIAGNV_H
/*==========================================================================
  
General Description
 This module contains prototypes for diag_nv_read & diag_nv_write.
                
Copyright (c) 2001-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/* <EJECT> */
/*===========================================================================

                            Edit History

  $Header: //depot/asic/msmshared/services/diag/MSMSHARED_DIAG_1.2/diagnv.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/03/08   vg      Updated code to use PACK() vs. PACKED
04/04/05   as     Modified MEID code to use DIAG_MEID_ENABLE
03/30/05   as     Replaced FEATURE_NV_MEID_ENABLE with
                  FEATURE_DIAG_MEID_ENABLE.
03/14/05   as     Added prototype for diagnv_get_meid.
11/11/01   as     Created file.
                   
===========================================================================*/

#include "comdef.h"
#include "diagcmd.h"
#include "diagpkt.h"
#include "log_codes.h"
#include "feature.h"
#include "nv.h"

#ifdef FEATURE_MEID_SUPPORT
 #define DIAG_MEID_ENABLE
#endif


/*===========================================================================

FUNCTION DIAG_NV_READ

DESCRIPTION
  This procedure encapsulates an NV read operation for the diag task.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diag_nv_read ( 
  nv_items_enum_type item,              /* Which NV item to read */
  PACK(void *) buf_ptr           /* Where to put the data */
);



/*===========================================================================

FUNCTION DIAG_NV_WRITE
DESCRIPTION
  This procedure encapsulates an NV read operation for the diag task.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diag_nv_write ( 
  nv_items_enum_type item,
  PACK(void *) buf_ptr
);



/*===========================================================================

FUNCTION DIAGNV_PEEK

DESCRIPTION
  This procedure encapsulates an NV peek operation for the diag task.
  This capability is not recommended and is intended to be deprecated.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diagnv_peek (nv_peek_type *peek);



/*===========================================================================

FUNCTION DIAGNV_POKE
DESCRIPTION
  This procedure encapsulates an NV poke operation for the diag task.
  This capability is not recommended and is intended to be deprecated.
  
DEPENDENCIES
  Assumes that diag_init has set up the NV command buffer, ncmd.

RETURN VALUE
  Status of NV operation.

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diagnv_poke (nv_poke_type *poke);



/*===========================================================================

FUNCTION DIAGNV_GET_ESN

DESCRIPTION
DEPENDENCIES
  None.

RETURN VALUE
  Pointer to response packet.

SIDE EFFECTS
  None.
 
===========================================================================*/
dword diagnv_get_esn  (void);



#ifdef DIAG_MEID_ENABLE
/*===========================================================================

FUNCTION DIAGNV_GET_MEID

DESCRIPTION
DEPENDENCIES
  None.

RETURN VALUE

SIDE EFFECTS
  None.
 
===========================================================================*/
nv_stat_enum_type diagnv_get_meid  (qword);
#endif


#ifdef FEATURE_DIAG_CACHE
/*===========================================================================

FUNCTION DIAGNV_REFRESH_CACHE

DESCRIPTION
  This procedure reads the NV data for the status packet into cache.
  This is done after the mc_task_start call and after OTASP successfully
  completes.

===========================================================================*/
void diagnv_refresh_cache (void);
#endif

#endif /* DIAGNV_H*/

