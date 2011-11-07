/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMPointerHelpers.h

SERVICES:  OEM Pointer Device Interface

GENERAL DESCRIPTION:
	Definitions, typedefs, etc. for pointing devices (touchpad, mouse, stylus etc)

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#ifndef OEMPOINTERHELPERS_H
#define OEMPOINTERHELPERS_H

#include "AEEPointerHelpers.h"
#include "AEEError.h"
#include "AEE_OEMEvent.h"


// EVT_POINTER_* helper functions
static __inline void AEE_POINTER_SEND_XY(AEEEvent evtCode, int x, int y)
{
   char pszXY[AEE_POINTER_X_SIZE + AEE_POINTER_Y_SIZE + 1] = {'\0'};
   int nBufferLength = sizeof(pszXY);
	
	AEE_POINTER_SET_XY(pszXY, nBufferLength, x, y);
#ifdef FEATURE_LCD_TOUCH_ENABLE
    if (OEMKeyguard_HandleEvent(evtCode, nBufferLength,(uint32)pszXY))
    {	
        return;
    }	
#endif
	(void)AEE_Event(evtCode, nBufferLength, (uint32)pszXY);
  
}

static __inline int AEE_POINTER_SET_XY_DISPID(char* pszDest, int nDestSize, \
                                              int x, int y, AEECLSID clsid)
{
   if (AEE_POINTER_X_SIZE + AEE_POINTER_Y_SIZE + AEE_POINTER_DISPID_SIZE > nDestSize) {
      return EBADPARM;
   }

   (void)AEE_POINTER_MEMMOVE((void *)pszDest, (void *)"x=00000000,y=00000000,dispID=00000000,", AEE_POINTER_X_SIZE+AEE_POINTER_Y_SIZE+AEE_POINTER_DISPID_SIZE);
   (void)AEE_POINTER_MAKE_HEX_STR(pszDest+AEE_POINTER_X_PREFIX_SIZE, AEE_POINTER_X_VALUE_SIZE, x);
   (void)AEE_POINTER_MAKE_HEX_STR(pszDest+AEE_POINTER_YSTR_OFFSET+AEE_POINTER_Y_PREFIX_SIZE, AEE_POINTER_Y_VALUE_SIZE, y);
   (void)AEE_POINTER_MAKE_HEX_STR(pszDest+AEE_POINTER_YSTR_OFFSET+AEE_POINTER_Y_SIZE+AEE_POINTER_DISPID_PREFIX_SIZE, AEE_POINTER_DISPID_VALUE_SIZE, clsid);

   return SUCCESS;
}

static __inline void AEE_POINTER_SEND_XY_DISPID(AEEEvent evtCode, \
                                                int x, int y, \
                                                AEECLSID clsid)
{
   char pszdwParam[AEE_POINTER_X_SIZE + AEE_POINTER_Y_SIZE + AEE_POINTER_DISPID_SIZE + 1] = {'\0'};
   int nBufferLength = sizeof(pszdwParam);
	
	AEE_POINTER_SET_XY_DISPID(pszdwParam, nBufferLength, x, y, clsid);
      
	(void)AEE_Event(evtCode, nBufferLength, (uint32)pszdwParam);
}

/*=====================================================================
MACROS DOCUMENTATION
=======================================================================

AEE_POINTER_SEND_XY()

Description:
   This helper function sends x & y coordinate values for the specified 
   event. This should be used in conjunction with EVT_POINTER_UP, 
   EVT_POINTER_DOWN, EVT_POINTER_MOVE, and EVT_POINTER_STALE_MOVE events. 
   Using this function to send the x and y coordinates will ensure that 
   EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, and 
   EVT_POINTER_STALE_MOVE events are generated and handled properly by BREW.

Prototype:
   void AEE_POINTER_SEND_XY(AEEEvent evtCode, int x, int y)

Parameters:
   evtCode: Event for which the new coordinates are to be sent
   x, y: Coordinate values

Return Value: None
   
Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4

See Also:
   AEE_POINTER_GET_X()
   AEE_POINTER_GET_Y()
   AEE_POINTER_SET_XY_OFFSET()

=======================================================================

AEE_POINTER_SET_XY_DISPID()

Description:
   This helper function sets the value for the pointing device's 
   x and y coordinates and display screen's AEECLSID. This should be used 
   in conjunction with EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, 
   and EVT_POINTER_STALE_MOVE events. 

Prototype:
   int AEE_POINTER_SET_XY_DISPID(char* pszDest, int nDestSize, 
                                 int x, int y, AEECLSID clsid)

Parameters:
   pszDest: Buffer to put the formatted string into. Should be at least 
            as big as AEE_POINTER_X_SIZE + AEE_POINTER_Y_SIZE + AEE_POINTER_DISPID_SIZE.
   nDestSize: Size of pszDest in bytes.
   x: X-coordinate of pointing device.
   y: Y-coordinate of pointing device.
   clsid: AEECLSID of display screen on which EVT_POINTER_* event occured.

Return Value:
   SUCCESS - X and Y coordinates and displayID formatted and written 
            to pszDest successfully.

   EBADPARM - invalid parameter passed to the function.

Comments:
   pszDest is not NUL-terminated by this function.

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4FP02

See Also:
   AEE_POINTER_GET_DISPID()

=======================================================================

AEE_POINTER_SEND_XY_DSIPID()

Description:
   This helper function sends x & y coordinate values and display screen 
   ID for the specified event. This should be used in conjunction with 
   EVT_POINTER_UP, EVT_POINTER_DOWN, EVT_POINTER_MOVE, and 
   EVT_POINTER_STALE_MOVE events. 

Prototype:
   void AEE_POINTER_SEND_XY_DISPID(AEEEvent evtCode, int x, int y, AEECLSID clsid)

Parameters:
   evtCode: Event for which the new coordinates are to be sent
   x, y: Coordinate values
   clsid: AEECLSID of display screen on which EVT_POINTER_* event occured.

Return Value: None
   
Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW Client 3.1.4

See Also:
   AEE_POINTER_SET_XY_DISPID()

=======================================================================

===========================================================================*/

#endif //OEMPOINTERHELPERS_H
