
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     TIME PROFILE SERVICE
                     -- HDR definitions

GENERAL DESCRIPTION
  The file defined further code parition, events defintions and macros
  definitions in the hdr timeline profiling section; In the HDR section, 
  in addition [15:14] as toplevel tag, bits [13:12] are used for further 
  code partition

EXTERNALIZED FUNCTIONS
  none

INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

Copyright (c) 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002, 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/05   jyw     created the file

===========================================================================*/

#include "profile.h"

#define PROFILE_HDR_VAL_MASK 0xfff
  /* the HDR used 12 bits for the events output */

/* bits [13:12] is used for sub TAG */
typedef enum
{
  PROFILE_HDR_SUBTAG_SRCH              = 0x0000,
    /* sub TAG [13:8] = 0b __00 xxxx xxxx xxxx for the HDR SRCH */

  /* --- add here for the new sub TAG ---- */

} profile_hdr_subtag_type;

#define PROFILE_HDR( sub_tag,val ) \
  PROFILE_TAG_OUT( PROFILE_TAG_HDR, \
     ( sub_tag )|(( val ) & PROFILE_HDR_VAL_MASK ))

