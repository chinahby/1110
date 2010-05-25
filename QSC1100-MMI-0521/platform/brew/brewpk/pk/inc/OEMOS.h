/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMOS.h

SERVICES:  OEM Operating System Interface

GENERAL DESCRIPTION:
	Definitions, typedefs, etc. for OEM Operating System

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#ifndef OEMOS_H
#define OEMOS_H

#include "AEEComdef.h"
#include "AEEStdLib.h"

#if defined(__cplusplus)
extern "C" {
#endif

/**************************************************************************/
/*               OPERATING SYSTEM EXPORTED OPERATIONS                     */
/**************************************************************************/
extern int16  OEMOS_InitLayer( void );
extern uint32 OEMOS_GetUpTime( void );
extern uint32 OEMOS_JulianToSeconds(JulianType* pDate);
extern void   OEMOS_SetTimer( uint32 nMSecs );
extern void   OEMOS_Sleep( uint32 nMSecs );
extern void   OEMOS_SignalDispatch( void );

extern uint32 OEMOS_IntLock( void );
extern void   OEMOS_IntFree( uint32 intSav );

extern void   OEMOS_CancelDispatch( void );
extern uint32 OEMOS_ActiveTaskID( void );
extern int32  OEMOS_LocalTimeOffset ( boolean *DaylightSavings );
extern void   OEMOS_ResetDevice(char * pszMsg, uint32 nCause );
extern void   OEMOS_Breakpoint(uint32 dwType, void * pData, uint32 nSize);
#if defined(__cplusplus)
}
#endif


/*========================================================================
  INTERFACES DOCUMENTATION
=======================================================================

OEMOS Interface
=======================================================================
Function: OEMOS_InitLayer()

Description:
   The OEM layer is expected to do any initializations necessary
   to provide OS services to BREW. This function gets called
   once during AEE_Init.

Prototype:
   int16  OEMOS_InitLayer( void );

Parameters:
   none

Return Value:
   Returns 0 for a successful initialization or any other 
   value to indicate error, in which case AEE will not be
   started.

Comments:
   If this function returns non-zero value, then BREW will not be
   started.

Side Effects:
   None

See Also:
   None
=======================================================================

Function: OEMOS_GetUptime()

Description:
   Returns the time in milliseconds since the device started.

Prototype:
   uint32  OEMOS_GetUptime( void );

Parameters:
   none

Return Value:
   Returns the time in milliseconds since the device started.

Comments:
   If a device is turned on for approx. 50 days, this value can
   roll over and start at zero again.

Side Effects:
   None

See Also:
   OEMOS_GetLocalTime()
   OEMOS_GetTimeMS()

========================================================================

Function: OEMOS_GetLocalTime()

Description:
  Returns the current time in seconds since 1/6/1980.

Prototype:
   uint32  OEMOS_GetLocalTime( void );

Parameters:
   none

Return Value:
   Returns the current time in seconds since 1/6/1980.

Comments:
   The time returned by this function can change when the device
   acquires time from a network.  Therefore, it should not be
   assumed that each call to this function will return a greater
   value.

   Even though this device returns the current time in seconds,
   the accuracy of that time is determined by the time resolution of
   the underlying hardware/software platform.

Side Effects:
   None

See Also:
   OEMOS_GetUptime()
   OEMOS_GetTimeMS()

========================================================================

Function: OEMOS_GetTimeMS()

Description:
  Returns the number of milliseconds since midnight.

Prototype:
   uint32  OEMOS_GetTimeMS( void );

Parameters:
   none

Return Value:
   Returns the number of milliseconds since midnight.

Comments:
   The time returned by this function is based on the system time.
   As such, it can change when the device acquires time from a network.  
   Therefore, it should not be assumed that each call to this function 
   will return a greater value.

   Even though this device returns the current time in milliseconds,
   the accuracy of that time is determined by the time resolution of
   the underlying hardware/software platform.

Side Effects:

See Also:
   OEMOS_GetUptime()
   OEMOS_GetLocalTime()

========================================================================

Function: OEMOS_SetTimer()

Description:

    *** This function is only for the use of BREW internals. If you are writing
    a static extension and need to set a timer, use AEE_SetTimer function ***
       
    Set the master OEM timer to nMSecs milliseconds. After nMSecs, the OEM
    code will call the AEE_Dispatch() function. A call to OEMOS_SetTimer
    while antoher timer is still pending will cancel the previous timer before
    setting the new one. A call to OEMOS_SetTimer with an nMSecs value of 0
    will cancel the pending timer, if one is active.

Prototype:
   void OEMOS_SetTimer( uint32 nMSecs );

Parameters:
   nMSecs: number of milliseconds to set the master OEM timer to

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMOS_Sleep()

Description:
    Delays execution of subsequent code for nMSecs.  This function will block
    for nMSecs.

Prototype:
   void OEMOS_Sleep( uint32 nMSecs );

Parameters:
   nMSecs: number of milliseconds to sleep

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMOS_SignalDispatch()

Description:
    Add an event to the event queue that will cause AEE_DISPATCH to be called.

Prototype:
   void OEMOS_SignalDispatch( void );

Parameters:
   None

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMOS_CancelDispatch()

Description:
    If there is an event in the queue to call AEE_DISPATCH, set it's enable
    field to false so that AEE_DISPATCH will not be called. If such an event
    does not exist one is created. (This is OK since the event will have it's
    enable set to false so it will be ignored by the event handler.)

Prototype:
   void OEMOS_CancelDispatch( void );

Parameters:
   None

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMOS_BrewHighPriority()

Description:
    Raise the BREW's task priority so that certain time limited
    operations will be performed more quickly, e.g. signature verification.

Prototype:
   void OEMOS_BrewHighPriority( void );

Parameters:
   None

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMOS_BrewNormalPriority()

Description:
    Return the BREW's task priority to its normal level.
    
Prototype:
   void OEMOS_BrewNormalPriority( void );

Parameters:
   None

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMOS_ActiveTaskID()

Description:
    Return the ID of the currently running task. 
    
Prototype:
   uint32 OEMOS_ActiveTaskID( void );

Parameters:
   None

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMOS_LocalTimeOffset()

Description:
   Returns the local timezone offset from UTC, in seconds. Optionally returns
   a flag indicating that daylight savings time is active (if it is, the value
   of the local timezone offset already takes the shift into account; the flag
   is just for controlling display of a timezone name if desired).
   The returned value is added to UTC to give local time, or subtracted from
   local time to give UTC time.
    
Prototype:
   int32 OEMOS_LocalTimeOffset( boolean  *DaylightSavings );

Parameters:
   DaylightSavings: pointer to boolean which is set to TRUE if daylight
                    savings time is active

Return Value:
   The local time zone offset from UTC in seconds.

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: OEMOS_Breakpoint()

Description:
   This function is used to notify the device that a breakpoint action has 
   occurred. By default the implementation simply prints a debug message
   with the type of problem that caused the breakpoint. In debug builds
   it may be useful to assert or place a software breakpoint in this routine
   when executing under a debugger to catch the source of these problems.
    
Prototype:
   void OEMOS_Breakpoint(uint32 dwType, void * pData, uint32 nSize);

Parameters:
   dwType: Type of fault that triggered the breakpoint request. These are of 
           type AEEBRK_*
   pData:  Context dependant data as determined by dwType.
   nSize:  Size of the type that pData points to. If 0, pData should not be 
           dereferenced and must be read as an integral data type. You can see
           the API reference for the size of data for that dwType.

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   AEEOS_Breakpoint()
   AEEBRK Type Definitions

========================================================================

Function: OEMOS_IntLock()

Description:
   This function is used to lock device interrupts.
    
Prototype:
   uint32 OEMOS_IntLock(void);

Parameters:
   None

Return Value:
   Saved interrupt state, should be used in the corresponding call to IntFree

Comments:
   None

Side Effects:
   None

See Also:
   OEMOS_IntFree()

========================================================================

Function: OEMOS_IntFree()

Description:
   This function is used to free/unlock device interrupts.
    
Prototype:
   void OEMOS_IntFree(uint32 intSav);

Parameters:
   intsav: saved interrupts state, returned from a previous call to IntLock

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   OEMOS_IntLock()

=========================================================================*/

#endif /* OEMOS_H */
