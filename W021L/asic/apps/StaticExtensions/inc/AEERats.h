#ifndef AEERATS_H
#define AEERATS_H

/*============================================================================
FILE:  AEECard.h

SERVICES:  AEE Remote API Test Suite Interface.

GENERAL DESCRIPTION:
        Base level definitions, typedefs, etc.

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/Inc/AEERats.h#2 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/24/05   jas     Fixing include file case for Linux builds.
03/18/04   at      Initial release.
=============================================================================*/

#include "AEE.h"
#include "AEEComdef.h"
#include "AEEFile.h"

/* Class Ids
*/
#include "RATS.bid"

//*********************************************************************
//
// IRats Interface
//
//*********************************************************************

// Notification Masks...
// These are used when to determine what changed in the status function.
// Users can register for this notification via the INOTIFY interface.
// The below are implemented in the ICARD_INOTIFY class ID.
#define NMASK_DATA_AVAILABLE        0x0001  /* More data is available. */
#define NMASK_STATUS_CHANGE         0x0002  /* More data is available. */

// Settings for Client and Server Status.
#define IRATS_STATUS_DOWN        0 
#define IRATS_STATUS_UP          1 
#define IRATS_STATUS_PENDING     2 

/* Forward typedef reference of the objects. */
typedef struct IRats IRats;
AEEINTERFACE(IRats)
{
   INHERIT_INotifier(IRats);

   int     (*GetServerStatus)      (IRats *pMe);
   int     (*SetServerStatus)      (IRats *pMe, int NewStatus);
   int     (*GetClientStatus)      (IRats *pMe);
   int     (*SetClientStatus)      (IRats *pMe, int NewStatus);
   uint32  (*GetRemoteIP)          (IRats *pMe);
   uint32  (*GetLocalIP)           (IRats *pMe);
   int     (*SetRemoteIP)          (IRats *pMe, uint32 ip);
   IFile  *(*GetFile)              (IRats *pMe);
   int     (*RunTest)              (IRats *pMe, uint32 test);
};

#define IRATS_AddRef(p)            AEEGETPVTBL(p,IRats)->AddRef(p)
#define IRATS_Release(p)           AEEGETPVTBL(p,IRats)->Release(p)
#define IRATS_GetClientStatus(p)   AEEGETPVTBL(p,IRats)->GetClientStatus(p)
#define IRATS_GetServerStatus(p)   AEEGETPVTBL(p,IRats)->GetServerStatus(p)
#define IRATS_SetClientStatus(p,u) AEEGETPVTBL(p,IRats)->SetClientStatus(p,u)
#define IRATS_SetServerStatus(p,u) AEEGETPVTBL(p,IRats)->SetServerStatus(p,u)
#define IRATS_GetRemoteIP(p)       AEEGETPVTBL(p,IRats)->GetRemoteIP(p)
#define IRATS_GetLocalIP(p)        AEEGETPVTBL(p,IRats)->GetLocalIP(p)
#define IRATS_SetRemoteIP(p,i)     AEEGETPVTBL(p,IRats)->SetRemoteIP(p,i)
#define IRATS_GetFile(p)           AEEGETPVTBL(p,IRats)->GetFile(p)
#define IRATS_RunTest(p,t)         AEEGETPVTBL(p,IRats)->RunTest(p,t)

/*=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================
Interface Name: IRats

Description:
   IRats is an interface between brew applications and the Remote API
   Test Software.   The IRATS interface implements the Notifier interface
   as well.  The clients are capable of requesting notifications on
   changes in the server/client status as well as updates to the text
   file.

It provides the following services:

- Access to the STA RATs calls.
- Maintains a file containing the responses from various calls.
- Maintaining the client pointer received from the underlying code.
- Maintains the server status.
- Notifying clients of server/client status change events.
- Notifying clients of new information.

The IRats interface is obtained via the ISHELL_CreateInstance mechanism.

Users can register for this notification via the INOTIFIER interface.
#define NMASK_DATA_AVAILABLE        0x0001   More data is available.
#define NMASK_STATUS_CHANGE         0x0002   Server or Client Status has changed.

The following defines are used to communicate server/client status.
IRATS_STATUS_DOWN        The client or server is not connected at this time.
IRATS_STATUS_UP          The client or server is connected.
IRATS_STATUS_PENDING     The status is in the process of changing. 
                         i.e. Bringing up the client can take several seconds.
			      During this time, IRats_GetClientStatus will return
			      IRATS_STATUS_PENDING.   Also, at this time, requests
			      to change the status will be rejected.


=======================================================================

Function: IRATS_AddRef

Description:
   This function increments the number of clients using the current object.
   
Prototype:
   int IRATS_AddRef(IRats * po);

Parameters:
   po: [Input] Pointer to the IRats object

Return Value:
  Number of references to the given object.

Comments:
   None

Side Effects:
   None

See Also:
   IRATS_Release
=======================================================================

=======================================================================

Function: IRATS_Release

Description:
   This function decrements the number of clients using the current object.
   When the number of references to the object reaches 0, the memory
   associated with the object will be released provided that the server
   and client are down.  If a connection currently exists, the memory will
   remain to allow applications to maintain the correct status of the
   underlying object.
   
Prototype:
   int IRATS_Release(IRats * po);

Parameters:
   po: [Input] Pointer to the IRats object

Return Value:
  Number of references to the given object after this is released

Comments:
   None

Side Effects:
   When the object is released, any calls from the lower level object
   will be ignored.

See Also:
   IRATS_AddRef
=======================================================================

Function: IRATS_GetClientStatus

Description
  Gets the status of the client. 

Dependencies
  none

Prototype:
  int IRATS_GetClientStatus(IRats *pMe);

Parameters:
   pMe: [Input] Pointer to the IRats object

Return Value:
  IRATS_STATUS_DOWN        The client is not connected at this time.
  IRATS_STATUS_UP          The client is connected.
  IRATS_STATUS_PENDING     The status is in the process of changing. 
			   i.e. Bringing up the client can take several seconds.
				During this time, IRats_GetClientStatus will return
				IRATS_STATUS_PENDING.

Side Effects:
  None

See Also:
  IRATS_SetClientStatus

=======================================================================

FUNCTION: IRATS_GetServerStatus

Description
  Gets the status of the server. 

Dependencies
  none

Prototype:
  int IRATS_GetServerStatus(IRats *pMe)

Parameters:
   pMe: [Input] Pointer to the IRats object

Return Value:
  IRATS_STATUS_DOWN        The server is not available at this time.
  IRATS_STATUS_UP          The server is available.
  IRATS_STATUS_PENDING     The status is in the process of changing. 
			   i.e. Bringing up the server can take several seconds.
				During this time, IRats_GetServerStatus will return
				IRATS_STATUS_PENDING.

Side Effects:
  None

See Also:
  IRATS_SetServerStatus

=======================================================================

FUNCTION: IRATS_SetClientStatus

Description
  Attempts to set the status of the client.  The function verifies
  that the attempted state change is allowed.  Note that you can only
  start a client that is down, and stop a client that is up.  No changes
  are allowed when the state is pending.

Dependencies
  none

Prototype:
  int IRATS_SetClientStatus(IRats *pMe, int NewStatus)

Parameters:
   pMe: [Input] Pointer to the IRats object
   NewStatus: [Input]  Must be one of the following values:
     IRATS_STATUS_DOWN        Stops the client.
     IRATS_STATUS_UP          Starts the client.

Return Value:
  EBADPARM:                Parameters were not valid.
  EBADSTATE:               Attempted to perform an illegal state change.
  AEE_SUCCESS:             The request has been accepted.
                              

Side Effects:
  None

See Also:
  IRATS_GetClientStatus

=======================================================================

FUNCTION: IRATS_SetServerStatus

Description
  Attempts to set the status of the server.  The function verifies
  that the attempted state change is allowed.  Note that you can only
  start the server when it is down, and stop the server when it is up. 
  No changes are allowed when the state is pending.

Dependencies
  none

Prototype:
  int IRATS_SetServerStatus(IRats *pMe, int NewStatus)

Parameters:
   pMe: [Input] Pointer to the IRats object
   NewStatus: [Input]  Must be one of the following values:
     IRATS_STATUS_DOWN        Stops the server.
     IRATS_STATUS_UP          Starts the server.

Return Value:
  EBADPARM:                Parameters were not valid.
  EBADSTATE:               Attempted to perform an illegal state change.
  AEE_SUCCESS:             The request has been accepted.
                              

Side Effects:
  When the SERVER is brought from DOWN to UP, the results file is 
  emptied.

See Also:
  IRATS_GetServerStatus

=======================================================================

FUNCTION: IRATS_GetRemoteIP

Description
  Retrieves the current setting for the remote IP address.  This is the
  address that will be used if IRATS_SetClientStatus attempts to bring
  the client up. 

Dependencies
  none

Prototype:
  uint32 IRATS_GetRemoteIP(IRats *pMe)

Parameters:
   pMe: [Input] Pointer to the IRats object

Return Value:
   The IP address in network byte order.  0 indicates the
   Remote IP address is not set.

Side Effects:
  None

See Also:
  IRATS_SetRemoteIP
  IRATS_SetClientStatus

=======================================================================

FUNCTION: IRATS_SetRemoteIP

Description
  Sets the Remote IP address to be used when we attempt to bring
  the client UP.

Dependencies
  none

Prototype:
  int IRATS_SetRemoteIP(IRats *pMe, uint32 ip)

Parameters:
   pMe: [Input] Pointer to the IRats object
   ip:  [Input] IP Address in network byte order.

Return Value:
   AEE_SUCCESS: IP address has been saved.

Side Effects:
  None

See Also:
  IRATS_GetRemoteIP
  IRATS_SetClientStatus
=======================================================================

FUNCTION: IRATS_GetLocalIP

Description
  Gets the local IP address.

Dependencies
  none

Prototype:
  uint32 IRATS_GetLocalIP(IRats *pMe)

Parameters:
   pMe: [Input] Pointer to the IRats object

Return Value:
  The IP address in network byte order.  0 indicates the
  Local IP address is not set.

Side Effects:
  None

=======================================================================

FUNCTION: IRATS_GetFile

Description
  Retrives an IFILE pointer to the results file.  When the server is
  connected, the results are placed into a file.  This call is used
  to retrieve a pointer to the file opened for reading only.  The
  FILE retrieved contains all the messages regarding the tests as well
  as possbily error messages when attempting to bring up the client
  and server.

Dependencies
  none

Prototype:
  IFile *IRATS_GetFile(IRats *pMe)

Parameters:
   pMe: [Input] Pointer to the IRats object

Return Value:
  If the file exists, a pointer to the IFILE object.  Note that
  this object must be released by the application when it has
  completed using this object.  If the server is brought down,
  this object MUST be released before the server is allowed to
  come back up.

Side Effects:
  None

=======================================================================

FUNCTION: IRATS_RunTest

Description
  When the client is up, this allows the user to run a given
  test.

Dependencies
  none

Prototype:
  uint32 IRATS_RunTest(IRats *pMe, uint32 test)

Parameters:
   pMe: [Input] Pointer to the IRats object

Return Value:
  AEE_SUCCESS if the request was successfully sent to the lower levels.
  EBADSTATE if the client is not up.

Side Effects:
  None

See Also:
  IRATS_SetClientStatus

=======================================================================
*/
#endif    /* AEERATS_H */

