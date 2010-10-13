/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  AEEOS.h

SERVICES:  AEE Interface to OEM Operating System Services required by BREW

GENERAL DESCRIPTION:
        Definitions, typedefs, etc. for OEM Operating System

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

#ifndef AEEOS_H
#define AEEOS_H

#include "AEE.h"


#if defined(__cplusplus)
extern "C" {
#endif

//********************************************************************************************************************************
//
// Used for AEEOS_Breakpoint
//
//********************************************************************************************************************************
#define AEEBRK_MEMLEAK     0x00000001
#define AEEBRK_IFACELEAK   0x00000002
#define AEEBRK_CORRUPTNODE 0x00000003
#define AEEBRK_EXCEPTION   0x00000004

//
// AEEAppLeak - passed to AEEOS_Breakpoint() for AEEBRK_MEMLEAK and AEEBRK_IFACELEAK
//
typedef struct _AEEAppLeak
{
   void *   pBuffer;          // Buffer Leaked
   uint32   dwMemGroup;       // Group ID of the owner of the buffer
} AEEAppLeak;


/**************************************************************************/
/*               OPERATING SYSTEM EXPORTED OPERATIONS                     */
/**************************************************************************/
extern AEE_EXPORTS int16   AEEOS_InitLayer(void);
extern AEE_EXPORTS uint32  AEEOS_GetUpTime(void);
extern AEE_EXPORTS uint32  AEEOS_GetLocalTime(void);
extern AEE_EXPORTS uint32  AEEOS_GetTimeMS(void);
extern AEE_EXPORTS uint32  AEEOS_JulianToSeconds(JulianType* pDate);
extern AEE_EXPORTS void    AEEOS_Sleep(uint32 nMSecs);
extern AEE_EXPORTS void    AEEOS_SignalDispatch(void);
extern AEE_EXPORTS uint32  AEEOS_ActiveTaskID(void);
extern AEE_EXPORTS int32   AEEOS_LocalTimeOffset(boolean *DaylightSavings);
extern AEE_EXPORTS boolean AEEOS_IsBadPtr(int chkType, void * pBuf, uint32 len);
extern AEE_EXPORTS void    AEEOS_ResetDevice(char * pszMsg, int nCause);
extern AEE_EXPORTS uint32  AEEOS_GetUTCSeconds(void);
extern AEE_EXPORTS void    AEEOS_Breakpoint(uint32 dwType, void * pData, uint32 nSize);
extern AEE_EXPORTS uint32  AEEOS_IntLock(void);
extern AEE_EXPORTS void    AEEOS_IntFree(uint32 intSav);

#if defined(__cplusplus)
}
#endif

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
AEEBRK_

AEEBRK_XXXXX is the format for AEE Breakpoint Faults
=======================================================================
AEE Breakpoint Faults

Description:
This specifies the ID of the item whose information is given. This is used
in the function AEEOS_Breakpoint()


Definition:
uint32

Members:

The following Items are supported:

~
   Name:
   AEEBRK_MEMLEAK
~
   Description:
   This triggers a breakpoint call for a memory leak fault. It will be called for each node
   that is leaked by the application when the module is unloaded.Do not free this buffer,
   it will be freed after AEEOS_Breakpoint() returns.
===pre>
   void  AEEOS_Breakpoint(uint32 dwType, void * pData, uint32 nSize);
===/pre>

   Parameters:
      dwType: Specifies AEEBRK_MEMLEAK
      pData: Pointer to a AEEAppLeak structure
      nSize: Size of AEEAppLeak structure
*

   Name:
   AEEBRK_IFACELEAK
~
   Description:
   This triggers a breakpoint call for an interface leak fault, when the interface has registered
   and linked system object attached to the application whose module is unloading. The extension
   registers with AEE_LinkSysObject(). Do not release this object, it will be released after
   AEEOS_Breakpoint() returns.
===pre>
   void  AEEOS_Breakpoint(uint32 dwType, void * pData, uint32 nSize);
===/pre>

   Parameters:
      dwType: Specifies AEEBRK_IFACELEAK
      pData: Pointer to a AEEAppLeak structure
      nSize: Size of AEEAppLeak structure

*

   Name:
   AEEBRK_CORRUPTNODE
~
   Description:
   This triggers a breakpoint call when a node is corrupt. This may not be the exact corrupt
   node, but in most cases should be close. You can then use the AEEkHeap_Walk to walk the
   heap for more information if necessary.
===pre>
   void  AEEOS_Breakpoint(uint32 dwType, void * pData, uint32 nSize);
===/pre>

   Parameters:
      dwType: Specifies AEEBRK_CORRUPTNODE
      pData: Address of a corrupt node
      nSize: 0

*

   Name:
   AEEBRK_EXCEPTION
~
   Description:
   This triggers a breakpoint call for an exception that will happen synchronously before an
   exception is thrown or device reset is issued.
===pre>
   void  AEEOS_Breakpoint(uint32 dwType, void * pData, uint32 nSize);
===/pre>

   Parameters:
      dwType: Specifies AEEBRK_EXCEPTION
      pData: Pointer to a AEEExceptionType type
      nSize: Size of AEEExceptionType type

=======================================================================
AEEAppLeak

Description:
This structure is passed to AEEOS_Breakpoint() for AEEBRK_MEMLEAK and AEEBRK_IFACELEAK.

Definition:
typedef struct _AEEAppLeak
{
   void *   pBuffer;
   uint32   dwMemGroup;
} AEEAppLeak;

Members:
   pBuffer:       Pointer to the Buffer or Interface's SyObject's pNotifyData
                  that was Leaked
   dwMemGroup:    Group ID of the owner of the buffer

Comments:
   You can use the group ID with AEE_GetMemGroupName() to obtain more information.

See Also:
   AEEOS_Breakpoint()

========================================================================
  INTERFACES DOCUMENTATION
=======================================================================
AEEOS Interface

=========================================================================
Function: AEEOS_InitLayer()

Description:
  Requests the OEM layer code to do necessary initializations
  for providing OS services to BREW. This function gets called
  once during AEE_Init.

Prototype:
   int16  AEEOS_InitLayer( void );

Parameters:
   None

Return Value:
  Returns 0 for a successful initialization or any other
  value to indicate error, in which case AEE will not be
  started.

Comments:
  If this function returns nonzero value, BREW will not be
  started.

Side Effects:
   None

See Also:
   None
=======================================================================

Function: AEEOS_GetUptime()

Description:
   Returns the time in milliseconds since the device started.

Prototype:
   uint32  AEEOS_GetUptime( void );

Parameters:
   None

Return Value:
   Returns the time in milliseconds since the device started.

Comments:
   If a device is turned on for approximately 50 days, this value can
   roll over and start at zero again.

Side Effects:

See Also:
   AEEOS_GetLocalTime()
   AEEOS_GetTimeMS()

========================================================================

Function: AEEOS_GetLocalTime()

Description:
  Returns the current time in seconds since 1/6/1980.

Prototype:
   uint32  AEEOS_GetLocalTime( void );

Parameters:
   None

Return Value:
   Returns the current time in seconds since 1/6/1980 adjusted for local time zone.

Comments:
   The time returned by this function can change when the device
   acquires time from a network.  Therefore, it should not be
   assumed that each call to this function will return a greater
   value.

   Even though this device returns the current time in seconds,
   the accuracy of that time is determined by the time resolution of
   the underlying hardware/software platform.

Side Effects:

See Also:
   AEEOS_GetUptime()
   AEEOS_GetTimeMS()

========================================================================

Function: AEEOS_GetTimeMS()

Description:
  Returns the number of milliseconds since midnight.

Prototype:
   uint32  AEEOS_GetTimeMS( void );

Parameters:
   None

Return Value:
   Returns the number of milliseconds since midnight.

Comments:
   The time returned by this function can change when the device
   acquires time from a network.  Therefore, it should not be
   assumed that each call to this function will return a greater
   value.

   Even though this device returns the current time in milliseconds,
   the accuracy of that time is determined by the time resolution of
   the underlying hardware/software platform.

Side Effects:
   None

See Also:
   AEEOS_GetUptime()
   AEEOS_GetLocalTime()

========================================================================

Function: AEEOS_Sleep()

Description:
    Delays execution of subsequent code for nMSecs.  This function will block
    for nMSecs.

Prototype:
   void AEEOS_Sleep( uint32 nMSecs );

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

Function: AEEOS_SignalDispatch()

Description:
    Add an event to the event queue that will cause AEE_DISPATCH to be called.

Prototype:
   void AEEOS_SignalDispatch( void );

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

Function: AEEOS_ActiveTaskID()

Description:
    Return the ID of the currently running task.

Prototype:
   uint32 AEEOS_ActiveTaskID( void );

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

Function: AEEOS_LocalTimeOffset()

Description:
   Returns the local time zone offset from UTC, in seconds. Optionally returns
   a flag indicating that daylight savings time is active (if it is, the value
   of the local time zone offset already takes the shift into account; the flag
   is just for controlling display of a time zone name if desired).
   The returned value is added to UTC to give local time, or subtracted from
   local time to give UTC time.

Prototype:
   int32 AEEOS_LocalTimeOffset( boolean  *DaylightSavings );

Parameters:
   DaylightSavings: pointer to boolean which is set to TRUE if daylight
                    savings time is active

Return Value:
   The local time zone offset from UTC in seconds

Comments:
   None

Side Effects:
   None

See Also:
   None

========================================================================

Function: AEEOS_GetUTCSeconds()

Description:
  Returns the current UTC time in seconds since 1/6/1980.

Prototype:
   uint32  AEEOS_GetUTCSeconds( void );

Parameters:
   None

Return Value:
   Returns the current time in seconds since 1/6/1980 UTC time.

Comments:
   The time returned by this function can change when the device
   acquires time from a network.  Therefore, it should not be
   assumed that each call to this function will return a greater
   value.

   Even though this device returns the current time in seconds,
   the accuracy of that time is determined by the time resolution of
   the underlying hardware/software platform.

Side Effects:

See Also:
   AEEOS_GetUptime()
   AEEOS_GetTimeMS()

========================================================================

Function: AEEOS_Breakpoint()

Description:
   This function is used to notify the device that a breakpoint action has
   occurred. By default the implementation simply prints a debug message
   with the type of problem that caused the breakpoint. In debug builds
   it may be useful to assert or place a software breakpoint in this routine
   when executing under a debugger to catch the source of these problems.

Prototype:
   void AEEOS_Breakpoint(uint32 dwType, void * pData, uint32 nSize);

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
   AEEBRK Type Definitions

========================================================================

Function: AEEOS_IntLock()

Description:
   This function is used to lock device interrupts.

Prototype:
   uint32 AEEOS_IntLock(void);

Parameters:
   None

Return Value:
   Saved interrupt state, should be used in the corresponding call to IntFree

Comments:
   None

Side Effects:
   None

See Also:
   AEEOS_IntFree

========================================================================

Function: AEEOS_IntFree()

Description:
   This function is used to free/unlock device interrupts.

Prototype:
   void AEEOS_IntFree(uint32 intSav);

Parameters:
   intsav: saved interrupts state, returned from a previous call to IntLock

Return Value:
   None

Comments:
   None

Side Effects:
   None

See Also:
   AEEOS_IntLock

=========================================================================*/

#endif /* AEEOS_H */

