#ifndef AEENTP_H
#define AEENTP_H
/*=====================================================================
FILE:  AEENTP.h

SERVICES:  AEE NTP Unicast Client Interface

DESCRIPTION:  Base level definitions, typedefs, etc. for a NTP Unicast Client

PUBLIC CLASSES: Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS: Not Applicable
        
        Copyright © 1999-2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

=====================================================================*/

//=====================================================================
// INCLUDES AND VARIABLE DEFINITIONS
//=====================================================================

//---------------------------------------------------------------------
// Defines
//---------------------------------------------------------------------
//---------------------------------------------------------------------
// Include Files
//---------------------------------------------------------------------

#include "AEE.h"
#include "AEEShell.h"
#include "AEEError.h"

#include "NTP.BID"

//---------------------------------------------------------------------
// Type Declarations
//---------------------------------------------------------------------

typedef struct _INTP INTP;

typedef struct
{
   int32    nResult;
   char     nStratum;
   uint32   dwSeconds;
   int32    dwOffset;
} AEENTPResult;

//---------------------------------------------------------------------
// Global Constant Declarations
//---------------------------------------------------------------------

//---------------------------------------------------------------------
// Global Data Declarations
//---------------------------------------------------------------------

//=====================================================================
// FUNCTION DECLARATIONS AND INLINE FUNCTION DEFINITIONS
//=====================================================================
//---------------------------------------------------------------------
// INTP Interface
//---------------------------------------------------------------------

AEEINTERFACE(INTP)
{
   INHERIT_IQueryInterface(INTP);

   int     (*GetTime)    (INTP * pINTP, char * pszServer,  uint32 nPort, char nNTPVersion, uint32 nTimeout, AEENTPResult * pres, AEECallback *pcb);
};

//---------------------------------------------------------------------
// Access Macros
//---------------------------------------------------------------------

#define INTP_AddRef(p)                                                AEEGETPVTBL(p,INTP)->AddRef(p)
#define INTP_Release(p)                                               AEEGETPVTBL(p,INTP)->Release(p)
#define INTP_QueryInterface(p,i,p2)                                   AEEGETPVTBL(p,INTP)->QueryInterface(p,i,p2)
#define INTP_GetTime(p,server, port, version, timeout, res, cb)       AEEGETPVTBL(p,INTP)->GetTime(p,server, port, version, timeout, res, cb)

/*============================================================================
   DATA STRUCTURE DOCUMENTATION
==============================================================================
AEENTPResult

Description:
   This structure is passed to INTP_GetTime(). Upon receiving the callback from
   INTP_GetTime(), this structure will contain the result of the NTP request.
   This API currently only supports up to 1 second resolution.


Definition:
typedef struct
{
   int32    nResult;
   char     nStratum;
   uint32   dwSeconds;
   int32    dwOffset;
} AEENTPResult;

Members:
   nResult: AEE_SUCCESS if NTP request was successful.
            EFAILED for general failure
            Network/Socket error code if the failure was network-related.
   nStratum: Stratum variable from NTP Server (refer to RFC-1305, RFC-2030)
   dwSeconds: Current Time (Seconds) as reported by the NTP Server. This time is
              referenced from 0h on 1 January 1900, as poer RFC-2030.
   dwOffset: Difference between the local clock and the server clock in Seconds.
             This is the most important value for synchronizing the local clock to 
             the server clock. (eg. Local Clock += dwOffset).

Comments:
   None

See Also:
   INTP_GetTime()


=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

Interface Name: INTP

Description: 
The INTP interface provides services for a Unicast NTP server.

To use the INTP interface, perform the following steps:

    Call ISHELL_CreateInstance() to create the NTP instance and
obtain an interface pointer to it.

    Call INTP_GetTime() to get the current time.

    When you no longer need the interface, call INTP_Release()
to free it.

Functions in this interface include:
INTP_AddRef() 
INTP_QueryInterface() 
INTP_Release() 
INTP_GetTime() 

=======================================================================

INTP_AddRef()

Description: 
    This function increments the reference count of the INTP Interface 
    object. This allows the object to be shared by multiple callers. The 
    object is freed when the reference count reaches 0 (zero).
 
Prototype:
    uint32 INTP_AddRef(INTP * pINTP)

Parameters:
    pINTP : Pointer to the INTP Interface object

Return Value:
    Incremented reference count for the object. 

Comments: 
    A valid object returns a positive reference count.

Side Effects: 
    None

See Also:
    INTP_Release() 

======================================================================= 

Function: INTP_QueryInterface()

Description:
   This function retrieves a pointer to an interface conforming to the
   definition of the specified class ID.  This can be used to query for
   extended functionality, like future versions or proprietary features.

   Upon a successful query, the interace is returned AddRef'ed.  The caller is
   responsible for calling Release() at some point in the future.  One
   exception is when the pointer returned is not an interface pointer.  In
   that case, the memory will share the lifetime of the object being queried,
   and the returned pointer will not be used to free or release the object.

Prototype:
   int INTP_QueryInterface(INTP *po, AEECLSID id, void **p);

Parameters:
   po [in]: Pointer to INTP interface.

   id [in]: A globally unique id to identify the entity (interface or data)
            that we are trying to query.

   p [out]: Pointer to the data or interface that we want to retrieve.  If
            the value passed back is NULL, the interface or data that we
            query are not available.

Return Value:
   Return SUCCESS on success, otherwise returns error code.

Comments:
   On failure, *po should be set to NULL, but it is good form to explicitly
   set *po to NULL before calling QueryInterface().

   NOTE: This function currently only responds to AEECLSID_NTP.

Side Effects:
   None

See Also:
   None

======================================================================= 

INTP_Release() 

Description:
    This function decrements the reference count for the INTP Interface 
    object and does appropriate cleanup if the reference count reaches 0 (zero).

Prototype:
    uint32 INTP_Release(INTP * pINTP)
 
Parameters:
    pINTP : Pointer to the INTP Interface object whose reference
                    count needs to be decremented

Return Value:
    Updated reference count for the object.

Comments: 
    None

Side Effects: 
    None

See Also:
    None

======================================================================= 

INTP_GetTime()

Description: 
    This function gets the current time from the NTP server. 
 
Prototype:
    int INTP_GetTime(INTP * pINTP, char * pszServer, uint32 nPort, uint32 nNTPVersion, uint32 nTimeout, AEENTPResult * pres, AEECallback *pcb)

Parameters:
    pINTP          : Pointer to the INTP Interface object
    pszServer      : The NTP Server to query
    nPort          : The Port of the NTP Server (123)
    nNTPVersion    : The version of the NTP Client request (1-4) this should be set to the highest version that
                     is known to be supported by the server
    nTimeout       : The amount of time to give the NTP Request before returning an error
    pres           : Pointer to the result structure.
    pcb            : The Callback to call when the NTP Request is complete

Return Value:
    SUCCESS : If successful
    EFAILED : If unsuccessful

Comments: 
    None

Side Effects: 
    None

See Also:
    AEENTPResult 

======================================================================= */


#endif /* ifndef AEENTP_H */
