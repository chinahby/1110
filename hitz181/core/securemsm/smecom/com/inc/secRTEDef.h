#ifndef SECRTEDEF_H
#define SECRTEDEF_H

/*===========================================================================

                    R T E  C O M M O N  D E F I N I T I O N S

                            H E A D E R    F I L E


              
              
DESCRIPTION
  This is the header file contains common types used accorss RTE components.

EXTERNALIZED FUNCTIONS
  

  Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/secRTEDef.h#2 $
  $DateTime: 2008/02/08 11:43:16 $
  $Author: rajeevg $                        


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/11/2006 ssm     Use UxString instead of string
02/23/2006 rv      Changes to make file ARM compiler compliant
7/28/2005  David   Tsang  Initial Creation.
=============================================================================*/


/*===========================================================================

                             INCLUDE FILES

===========================================================================*/
#include <string>
#include "comdef.h"
#include "msg.h"
#include "UxString.h"

#ifdef PLATFORM_LTK
#error code not present
#endif

/** 8-bit Unicode Transformation Format.
 */
typedef unsigned char  utf8;


/** 16-bit Unicode Transformation Format.
 */
typedef unsigned short utf16;

typedef struct
{
  uint32  Year;            /* Year [1980..2100]                            */
  uint32  Month;           /* Month of year [1..12]                        */
  uint32  Day;             /* Day of month [1..31] or day of year [1..366] */
  uint32  Hour;            /* Hour of day [0..23]                          */
  uint32  Minute;          /* Minute of hour [0..59]                       */
  uint32  Second;          /* Second of minute [0..59]                     */
  uint32  DayOfWeek;       /* Day of the week [0..6] Monday..Sunday        */
}UxJulian;

#endif /* SECRTEDEF_H */

