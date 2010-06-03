#ifndef AEEIMediaAudioDEFS_H
#define AEEIMediaAudioDEFS_H

/*============================================================================
FILE: AEEIMediaAudioDefs.h

SERVICES:  BREW Media Audio Special Effects Services

DESCRIPTION:
   This file contains common definitions for all BREW Media Audio Special Effects Services

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2005-2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
//==========================================================================
//    Include Files
//==========================================================================

#include "AEEStdDef.h"

//==========================================================================
//   Type definitions
//==========================================================================

//
// Parameter value range and default value
//
typedef struct AEEAudioParmRange
{
   int32 nMin;         // Minimum value
   int32 nMax;         // Maximum value
   int32 nDefault;     // Default value
   int32 nStep;        // Distance between subsequent values
} AEEAudioParmRange;

//
// Vector specification structure
//
typedef struct AEEVector {
    int32 x;  //[Mandatory] x coordinate in millimeters
    int32 y;  //[Mandatory] y coordinate in millimeters
    int32 z;  //[Mandatory] z coordinate in millimeters
} AEEVector;

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 

AEEAudioParmRange

Description:
This structure specifies a range for a prameter value and it's default value

Definition:
   typedef struct AEEAudioParmRange
   {
      int32 nMin;
      int32 nMax;
      int32 nDefault;
      int32 nStep;
   } AEEAudioParmRange;

Members:
   nMin:     Minimum value
   nMax:     Maximum value
   nDefault: Default value
   nStep:    Distance between subsequent values

Comments:
   None.

See Also:
   None

=============================================================================

AEEVector

Description: 
This structure specifies location in Cartesian coordinates.

Definition:
   typedef struct AEEVector {
       int32 x;
       int32 y;
       int32 z;
   } AEEVector;

Members:
   x:     [Mandatory] x coordinate in millimeters
   y:     [Mandatory] y coordinate in millimeters
   z:     [Mandatory] z coordinate in millimeters

Comments:
   The default location is (0, 0, 0) for the listener and all the sound sources.

See Also:
   None
============================================================================= */

#endif // AEEIMediaAudioDEFS_H
