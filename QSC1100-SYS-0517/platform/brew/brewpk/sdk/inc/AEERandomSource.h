#ifndef AEERANDOMSOURCE_H
#define AEERANDOMSOURCE_H
/*======================================================
FILE: AEERandomSource.h

SERVICES: RandomSource random number generator

DESCRIPTION: This file provides the documentation for 
   BREW's FIPS186 compliant random number generator.
   Note, RandomSource exists only as a class and reuses
   the ISource interface.

CLASS IDs:	AEECLSID_RANDOM

Copyright © 1999-2005,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

// AEECLSID_RANDOM defined in AEEClassIDs.h
#include "AEEClassIDs.h"

/*===========================================================================
   CLASS DOCUMENTATION
=============================================================================

AEECLSID_RANDOM

Description:
   This class provides a source of cryptographic quality random data from a 
FIPS186 compliant random number generator.

    The FIPS186 implementation is complete and complies with the standard. In 
 and of itself FIPS186 does not give cryptographic quality random numbers. Some
 true source of randomness is needed.  This implementation uses two sources of 
 randomness.

    The first source is timing from events seen by the Brew core. This includes
network activity timing and key press timing.  The time at which certain responses
are received from the network is added to the pool with millisecond accuracy. This
brings in randomness from both the IP network and wireless network depending
on how the network is loaded and being used. The time at which the user presses keys
with millisecond accuracy is also added. These two  alone gives cryptographic
quality random data.

    On some handsets radio frequency noise from the wireless modem is also used as a
second source of randomness.  This results in very high quality random numbers.

   This class shares the ISource interface and is used like any other 
ISource object.  In particular, the ISource_Read() function is 
used to fill a buffer with random bytes.

   Unlike a standard ISource object, the call to ISource_Read() will never
block with an ISOURCE_WAIT or fail with an ISOURCE_END result.  It will
always return the requested number of bytes.  While ISource_Readable() is
implemented for completeness, using it isn't required.  That is, a
RandomSource object can be used synchronously.

   Note that while a RandomSouce object can be used to generated a virtually 
unlimited amount of random data, care should be taken.  This interface 
relies on an internal buffer of pure random data which may be filled only a 
few times a second.  While ISource_Read() will continue to return random 
bytes, the quality of the bytes will decrease over time until fresh 
random data can be pulled in from this buffer.  In general, ISource_Read() 
will return the highest quality possible random bytes if it's not called 
more than a few times a second.

Default Interface:
   ISource

Other Supported Interfaces:
   None

=============================================================================*/

#endif /* ifndef AEERANDOMSOURCE_H */
