/*============================================================================

FILE:  OEMDebug.h

SERVICES:  
        OEM Debug Services

GENERAL DESCRIPTION:
        OEM Debug Output functions/services

               Copyright © 1999-2002 QUALCOMM Incorporated 
                       All Rights Reserved.
                   QUALCOMM Proprietary/GTDR

============================================================================*/
#ifndef OEMDEBUG_H
#define OEMDEBUG_H /* #ifndef OEMDEBUG_H */

#include "AEEComdef.h"
#include "AEEShell.h"

extern void OEMDebug_Trigger(unsigned uDebugTriggerNum);
extern void OEMDebug_MsgPut(int nLevel, 
                            const char *cpszFile, int nLine, 
                            const char *cpszMsg);
extern void OEMDebug_LogEvent(AEEEvent evt, AEECLSID cls, uint32 pl);

/*
==================================================================
  INTERFACES   DOCUMENTATION
=======================================================================
OEMDebug Interface

=======================================================================
Function: OEMDebug_LogEvent()

Description:
  This function sends a BREW event through a debug mechanism implmented
  by the OEM.

Prototype:
   void OEMDebug_LogEvent(AEEEvent evt, AEECLSID cls, uint32 pl);

Parameters:
   evt: BREW event to send (see AEE.h)
   cls: ClassID of the app or extension sending the event.  If cls is zero then
        the ClassID of the currently running app will be filled used.
   pl:  Four byte optional payload

Return Value: 
   None.

Comments:     

Side Effects:
   None.

See Also:
   None

==================================================================
Function: OEMDebug_MsgPut

Description:
  This function is called by BREW when a BREW debug message needs
  to be output.

Prototype:
   void OEMDebug_MsgPut(int nLevel, 
                        const char *cpszFile, int nLine, 
                        const char *cpszMsg);
Parameter(s):
   nLevel: one of DBGPRINTF_LEVEL_* from AEEStdLib.h, the OEM
            must map these to the appropriate OEM priority
            (if applicable)
   cpszFile, nLine: file and line information from the program
   cpszMsg: string to be output to the OEM-specific diagnostic
           output subsystem

Return Value: 
   None

Comments:
   None

Side Effects:
   None.

See Also:

==================================================================
Function: OEMDebug_Trigger

Description:
  This function is called by BREW when a BREW debug sequence is 
  activated, to enable the OEM to extend BREW's debug sequences.

Prototype:
   void OEMDebug_Trigger(unsigned uDebugTriggerNum);

Parameter(s):
   uDebugTriggerNum: the number typed by the user as the debug trigger

Return Value: 
   None

Comments:
   This function is called for debug triggers 1000-1999.  Triggers
   less than 1000 are reserved for the BREW core.  Triggers greater
   than 1999 are reserved for dynamic components.

Side Effects:
   None.

See Also:
==================================================================*/
 
#endif /* #ifndef OEMDEBUG_H */
