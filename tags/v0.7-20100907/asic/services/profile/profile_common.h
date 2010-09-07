/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     TIME PROFILE SERVICE
                     common software definitions

GENERAL DESCRIPTION
  The file centralized the time profiling defintions and macros for 
  the common software section.
  for common software [ 13:8 ] are used for the sub tag and [ 7:0 ]
  are used for the events definitions.

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
04/26/05   jyw     created the file

===========================================================================*/
#include "profile.h"

#define PROFILE_COMMON_VAL_MASK 0xff
  /* common sw used 8 bits [7:0] for the events/vals output */

typedef enum
{
  PROFILE_COMMON_SUBTAG_SLEEP = 0x0000,
    /* sub TAG [13:8] = 0b __00 0000 xxxx xxxx for the sleep task */

  PROFILE_COMMON_SUBTAG_BT    = 0x0100
    /* sub TAG [13:8] = 0b __00 0001 xxxx xxxx for the BT*/

  /* --- add here for the new sub TAG --- */
} profile_common_subtag_type;

#define PROFILE_COMMON( sub_tag, val ) \
  PROFILE_TAG_OUT( PROFILE_TAG_COMMON, \
     ( sub_tag )|(( val ) & PROFILE_COMMON_VAL_MASK ))

