#ifndef AEESTDERR_H
#define AEESTDERR_H
/*
=======================================================================
        Copyright © 2005-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=======================================================================
*/

//
// Basic Error Codes
//
//

#define  AEE_SUCCESS              0  // No error
#define  AEE_EFAILED              1  // General failure
#define  AEE_ENOMEMORY            2  // Insufficient RAM
#define  AEE_ECLASSNOTSUPPORT     3  // Specified class unsupported
#define  AEE_EVERSIONNOTSUPPORT   4  // Version not supported
#define  AEE_EALREADYLOADED       5  // Object already loaded
#define  AEE_EUNABLETOLOAD        6  // Unable to load object/applet
#define  AEE_EUNABLETOUNLOAD      7  // Unable to unload object/applet
#define  AEE_EALARMPENDING        8  // Alarm is pending
#define  AEE_EINVALIDTIME         9  // Invalid time
#define  AEE_EBADCLASS            10 // NULL class object
#define  AEE_EBADMETRIC           11 // Invalid metric specified
#define  AEE_EEXPIRED             12 // App/Component Expired
#define  AEE_EBADSTATE            13 // Invalid state
#define  AEE_EBADPARM             14 // Invalid parameter
#define  AEE_ESCHEMENOTSUPPORTED  15 // Invalid URL scheme
#define  AEE_EBADITEM             16 // Invalid item
#define  AEE_EINVALIDFORMAT       17 // Invalid format
#define  AEE_EINCOMPLETEITEM      18 // Incomplete item
#define  AEE_ENOPERSISTMEMORY     19 // Insufficient flash
#define  AEE_EUNSUPPORTED         20 // API is not supported
#define  AEE_EPRIVLEVEL           21 // Privileges are insufficient for this operation
#define  AEE_ERESOURCENOTFOUND    22
#define  AEE_EREENTERED           23
#define  AEE_EBADTASK             24
#define  AEE_EALLOCATED           25 // App/Module left memory allocated when released.
#define  AEE_EALREADY             26 // Operation is already in progress
#define  AEE_EADSAUTHBAD          27 // ADS mutual authorization failed
#define  AEE_ENEEDSERVICEPROG     28 // Need service programming
#define  AEE_EMEMPTR              29 // bad memory pointer
#define  AEE_EHEAP                30 // heap corruption
#define  AEE_EIDLE                31 // Context (system, interface, etc.) is idle
#define  AEE_EITEMBUSY            32 // Context (system, interface, etc.) is busy
#define  AEE_EBADSID              33 // Invalid subscriber ID
#define  AEE_ENOTYPE              34 // No type detected/found
#define  AEE_ENEEDMORE            35 // Need more data/info
#define  AEE_EADSCAPS             36 // ADS Capabilities do not match those required for phone
#define  AEE_EBADSHUTDOWN         37 // App failed to close properly
#define  AEE_EBUFFERTOOSMALL      38 // Destination buffer given is too small
#define  AEE_ENOSUCH              39 // No such name/port/socket/service exists or valid
#define  AEE_EACKPENDING          40 // ACK pending on application
#define  AEE_ENOTOWNER            41 // Not an owner authorized to perform the operation
#define  AEE_EINVALIDITEM         42 // Current item is invalid
#define  AEE_ENOTALLOWED          43 // Not allowed to perform the operation
#define  AEE_EBADHANDLE           44 // Invalid handle
#define  AEE_EOUTOFHANDLES        45 // Out of handles
#define  AEE_EINTERRUPTED         46 // Waitable call is interrupted
#define  AEE_ENOMORE              47 // No more items available -- reached end
#define  AEE_ECPUEXCEPTION        48 // A CPU exception occurred
#define  AEE_EREADONLY            49 // Cannot change read-only object or parameter
// a moratorium on adding to AEEStdErr.h is in effect, 50 and later are 
//  already spoken for

#define  AEE_EWOULDBLOCK         516 // Operation would block if not non-blocking; wait and try again

/*
============================================================================
   ERRORS DOCUMENTATION
==============================================================================
Basic AEE Error Codes

This section lists the set of basic errors returned, the codes associated
with the errors, and descriptions of the errors.


Basic error codes

Definition:

Error                Code   Description

AEE_SUCCESS             0   Operation Successful
AEE_EFAILED             1   General failure.
AEE_ENOMEMORY           2   Insufficient RAM.
AEE_ECLASSNOTSUPPORT    3   Specified class unsupported.
AEE_EVERSIONNOTSUPPORT  4   Version not supported.
AEE_EALREADYLOADED      5   Object already loaded.
AEE_EUNABLETOLOAD       6   Unable to load object/applet.
AEE_EUNABLETOUNLOAD     7   Unable to unload object/applet.
AEE_EALARMPENDING       8   Alarm is pending.
AEE_EINVALIDTIME        9   Invalid time.
AEE_EBADCLASS           10  NULL class object.
AEE_EBADMETRIC          11  Invalid metric specified.
AEE_EEXPIRED            12  Application/Component Expired.
AEE_EBADSTATE           13  Invalid state.
AEE_EBADPARM            14  Invalid parameter.
AEE_ESCHEMENOTSUPPORTED 15  Invalid URL scheme.
AEE_EBADITEM            16  Invalid item.
AEE_EINVALIDFORMAT      17  Invalid format.
AEE_EINCOMPLETEITEM     18  Incomplete item.
AEE_ENOPERSISTMEMORY    19  Insufficient flash.
AEE_EUNSUPPORTED        20  API is not supported.
AEE_EPRIVLEVEL          21  Application privileges are insufficient for this operation.
AEE_ERESOURCENOTFOUND   22  Unable to find specified resource
AEE_EREENTERED          23  Non re-entrant API re-entered
AEE_EBADTASK            24  API called in wrong task context
AEE_EALLOCATED          25  Application/Module left memory allocated when released..
AEE_EALREADY            26  Operation is already in progress.
AEE_EADSAUTHBAD         27  ADS mutual authorization failed.
AEE_ENEEDSERVICEPROG    28  Need service programming.
AEE_EMEMPTR             29  bad memory pointer.
AEE_EHEAP               30  heap corruption.
AEE_EIDLE               31  Context (system, interface, etc.) is idle.
AEE_EITEMBUSY           32  Context (system, interface, etc.) is busy.
AEE_EBADSID             33  Invalid subscriber ID
AEE_ENOTYPE             34  No type detected/found
AEE_ENEEDMORE           35  Need more data/info
AEE_EADSCAPS            36  Capabilities do not match those required
AEE_EBADSHUTDOWN        37  Application failed to close properly
AEE_EBUFFERTOOSMALL     38  Destination buffer given is too small
AEE_ENOSUCH             39  No such name/port/socket/service exists or valid
AEE_EACKPENDING         40  ACK pending on application
AEE_ENOTOWNER           41  Not an owner authorized to perform the operation
AEE_EINVALIDITEM        42  Current item is invalid
AEE_ENOTALLOWED         43  Not allowed to perform the operation
AEE_EBADHANDLE          44  Invalid handle
AEE_EOUTOFHANDLES       45  Out of handles
AEE_EINTERRUPTED        46  Waitable call is interrupted
AEE_ENOMORE             47  No more items available -- reached end
AEE_ECPUEXCEPTION       48  A CPU exception occurred
AEE_EREADONLY           49  Cannot change read-only object or parameter
AEE_EWOULDBLOCK         516 Operation would block if not non-blocking; wait and try again

==================================================================
*/
#endif /* #ifndef AEESTDERR_H */

