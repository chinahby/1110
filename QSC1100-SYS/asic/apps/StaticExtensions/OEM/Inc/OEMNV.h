/*=============================================================================

FILE: OEMNV.h
 
SERVICES: OEM Non Volatile Memory Interface
 
GENERAL DESCRIPTION:
 
PUBLIC CLASSES AND STATIC FUNCTIONS:
 
INITIALIZATION AND SEQUENCING REQUIREMENTS:
 
(c) COPYRIGHT 2002 QUALCOMM Incorporated.
                    All Rights Reserved.
 
                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Inc/OEMNV.h#1 $
$DateTime: 2003/01/15 17:45:16 $
  $Author: karthick $
  $Change: 34303 $
                      EDIT HISTORY FOR FILE
 
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
 
when       who     what, where, why
--------   ---     ---------------------------------------------------------
07/24/02   mjv     Ported to BREW 2.0
05/09/02   mjv     Ported to BREW 1.2
02/26/02   mjv     Changed the return type of OEMNV_WSTR_TO_RUIMAlpha() to 
                   void.
01/31/02   mjv     Created edit history.  Added OEMNV_RUIMAlpha_TO_WSTR and 
                   OEMNV_WSTR_TO_RUIMAlpha.
=============================================================================*/
 
#ifndef OEM_NV_H
#define OEM_NV_H

// comdef.h unconditionally defines ARR_SIZE.
//
// Ensure ARR_SIZE is not defined (by AEEComdef.h) if comdef.h has not
// been included yet as the next include will cause comdef.h to be included 
// indirectly. 
#ifndef COMDEF_H
#undef ARR_SIZE
#endif
#include "nv.h"
#include "AEEStdLib.h"

/**************************************************************************/
/*             Define Constant Data                                       */
/**************************************************************************/
/* Default IMSI values */
#define IMSI_MCC_0 999       /* 000 */
#define IMSI_S1_0  16378855   /* 0000000 */
#define IMSI_S2_0  999       /* 000 */
#define IMSI_CLASS0_ADDR_NUM 0xFF
#define IMSI_11_12_0 99

#if defined(__cplusplus)
extern "C" {
#endif

/**************************************************************************/
/*             NON VOLATILE MEMORY EXPORTED OPERATIONS                    */
/**************************************************************************/

/*===========================================================================
FUNCTION: OEMNV_Get

DESCRIPTION
  Get an item from the nonvolatile memory.  Handles nonactive items by
  providing a default value.

RETURN VALUE
  The NV return code, except for NV_NOTACTIVE_S, which is handled
  internally.

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
extern nv_stat_enum_type OEMNV_Get
(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to space for item */
);


/* <EJECT> */
/*===========================================================================
FUNCTION: OEMNV_Put

DESCRIPTION
  Write an item to NV memory.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
extern nv_stat_enum_type OEMNV_Put
(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
);


/*===========================================================================
FUNCTION: OEMNV_Replace

DESCRIPTION
  Replace an item to NV memory.  Wait till write is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
extern nv_stat_enum_type OEMNV_Replace
(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
);


/*===========================================================================
FUNCTION: OEMNV_Free

DESCRIPTION
  Free an item in NV memory.  Wait till free is completed.

RETURN VALUE
  The NV Return Code

DEPENDENCIES
  This routine is not reentrant.  Shouldn't be a problem, as it doesn't exit
  till we're done, and it's only called from the UI task.
===========================================================================*/
extern nv_stat_enum_type OEMNV_Free
(
  nv_items_enum_type item,        /* which item */
  nv_item_type *data_ptr          /* pointer to data for item */
);


/*=============================================================================
FUNCTION:  OEMNV_RUIMAlpha_TO_WSTR
 
DESCRIPTION: Decode a R-UIM alpha tag into an Unicode string
 
PARAMETERS:
   *alpha:  source string
   nAlphaSize: source string length 
   *wStr:   destination AECHAR string
   nWStrSize:  max size of wStr
 
RETURN VALUE:
   None

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEMNV_RUIMAlpha_TO_WSTR(byte const *alpha,
                             uint32      nAlphaSize,
                             AECHAR     *wStr,
                             uint32      nWStrSize);


/*=============================================================================
FUNCTION:  OEMNV_WSTR_TO_RUIMAlpha
 
DESCRIPTION: Encode an Unicode string into an R-UIM alpha tag string
 
PARAMETERS:
   *wStr: source string
   *alpha: destination string
   nAlphaSize:  max size of the destination string.
 
RETURN VALUE:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
void OEMNV_WSTR_TO_RUIMAlpha(AECHAR const *wStr,
                             byte         *alpha,
                             uint32        nAlphaSize);


#if defined(__cplusplus)
}
#endif


#endif /* OEM_NV_H */
