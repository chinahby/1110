#ifndef AEEPOINT_H
#define AEEPOINT_H
/*=============================================================================

FILE:          AEEPoint.h

SERVICES:      AEEPoint type

DESCRIPTION:

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

Copyright © 2001-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

#include "AEEStdDef.h"

// Point type
typedef struct _point
{
   int16       x, y;
}AEEPoint;

/*
===============================================================================
DATA STRUCTURES DOCUMENTATION
===============================================================================

AEEPoint

Description:
This structure defines the point data type.

Definition:
typedef struct _point
{
   int16 x, y;
} AEEPoint;

===============================================================================
*/
#endif //AEEPOINT_H

