#ifndef AEERECT_H
#define AEERECT_H
/*=============================================================================
FILE:          AEERect.h

SERVICES:      AEERect type definition

DESCRIPTION:

PUBLIC CLASSES:   Not Applicable

INITIALIAZTION AND SEQUENCEING REQUIREMENTS: Not Applicable

Copyright © 2000-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=============================================================================*/

#include "AEEStdDef.h"

typedef struct {
   int16    x,y;
   int16   dx, dy;
} AEERect;

#ifndef SETAEERECT
#define SETAEERECT(prc,l,t,w,h)   (prc)->x=(int16)(l),(prc)->y=(int16)(t),(prc)->dx=(int16)(w),(prc)->dy=(int16)(h)
#endif

/*
===============================================================================
DATA STRUCTURES DOCUMENTATION
===============================================================================

AEERect

Description:
AEERect is used to define a rectangle used by various Display, Graphics, Text Control,
and other helper functions.

Definition:
typedef struct
{
   int16 x,y;
   int16 dx, dy;
} AEERect;

Members:

x : The horizontal coordinate for the beginning (top left corner) of the rectangle
y : The vertical coordinate for the beginning (top left corner) of the rectangle
dx : The width of the rectangle (in pixels)
dy : The height of the rectangle (in pixels)


Comments:
None

See Also:
None

===============================================================================
*/

/*
===============================================================================
MACROS DOCUMENTATION
===============================================================================

Function: SETAEERECT()

Description:
  This macro initializes an AEERECT struct.

Prototype:
  #define SETAEERECT(prc,l,t,w,h)   (prc)->x=(int16)(l),(prc)->y=(int16)(t),(prc)->dx=(int16)(w),(prc)->dy=(int16)(h)

Parameters:
  prc       [out]: Pointer to AEERECT struct to be filled
  l          [in]: X coordinate of left side of rectangle
  t          [in]: Y coordinate of top side of rectangle
  w          [in]: Width of rectangle
  h          [in]: Height of rectangle

Return Value:
  None.

Comments:
  None.

Version:
   Introduced BREW Client 1.0

See Also:
  AEERECT

===============================================================================
*/

#endif //AEERECT_H
