/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                     TIME PROFILE SERVICE

GENERAL DESCRIPTION
  The "profile.h" centralized the time profiling defintions and macros
  The timetest port has 16bits data width;  [15:14] is used to parition 
  the code domain into the following 4 sections

      0b00   extention  
      0b01   common software
      0b10   1X 
      0b11   HDR 

  In each sections, sub TAG may be used to further partition the code domain 
  and they are defined in the following files
  "profile_hdr.h" macros and definitions for the HDR sections
  "profile_1x.h"  macros and definitions for the 1x sections
  "profile_common.h"  macros and definitions for the 1x sections

EXTERNALIZED FUNCTIONS
  none

INITIALIZATION AND SEQUENCING REQUIREMENTS
  none

Copyright (c) 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002, 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2004, 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

$Header: 

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/10/05   jyw     created the file

===========================================================================*/
#include "timetest.h"
  /* important: import the TIMETEST_PORT defintion from timetest.h
     please make sure the right featurization from there */

#ifdef FEATURE_TIMELINE_PROFILE

#define PROFILE_OUT( val )  outpw( TIMETEST_PORT, (uint16) (val) )
#define PROFILE_VAL_MASK    0x3fff
#define PROFILE_VAL_BITS    14

typedef enum
{
  PROFILE_TAG_EXT    = 0x0000,
     /* extention tag is used to output the val[15:14] or maybe above*/

  PROFILE_TAG_COMMON = 0x4000,
     /* common software tag [15:14] = 0b01 */

  PROFILE_TAG_1X     = 0x8000,
     /* 1x tag [15:14] = 0b10 */

  PROFILE_TAG_HDR    = 0xC000,
     /* HDR tag [15:14] = 0b11 */

} profile_tag_type;


/* output the value stamped by the TAG at [15:14] */
#define PROFILE_TAG_OUT( tag,val ) \
  PROFILE_OUT(( tag ) | (( val ) & PROFILE_VAL_MASK ))

/* output the upper part of a value stamped by TAG 0b00 */
#define PROFILE_EXT_OUT( val ) \
  PROFILE_OUT(( val ) >> PROFILE_VAL_BITS ) & PROFILE_VAL_MASK )

#else  /* FEATURE_TIMELINE_PROFILE */

#define PROFILE_OUT( )
#define PROFILE_TAG_OUT( tag, val )
#define PROFILE_EXT_OUT( val )

#endif /* FEATURE_TIMELINE_PROFILE */
