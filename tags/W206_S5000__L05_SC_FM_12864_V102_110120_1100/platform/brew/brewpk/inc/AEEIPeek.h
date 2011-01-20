#ifndef AEEIPEEK_H
#define AEEIPEEK_H
/*======================================================
FILE:  AEEIPeek.h

SERVICES:  IPeek

GENERAL DESCRIPTION:
   IPeek definitions, typedefs, etc.

Copyright © 2004-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

#include "AEEISource.h"

/*
  ||
  ||  IPeek : Abstract data source, buffer management
  ||
*/

/*
  || Exit codes for IPeek member functions:
 */

#define IPEEK_END     ISOURCE_END   // Read/Peek:  end of stream
#define IPEEK_ERROR   ISOURCE_ERROR // Read/Peek:  error in stream; no more data is available
#define IPEEK_WAIT    ISOURCE_WAIT  // Read/Peek:  try again later after calling Readable
#define IPEEK_FULL   -3             // Peek:  buffer is full
#define IPEEK_UNK    -4             // Peek:  did not call Read()


#define IPEEK_PEEKMAX     0x10000000  // pass this to Peek() to request "as much data as possible"
#define IPEEK_GETBUFSIZE  (IPEEK_PEEKMAX+1)  // pass this to Peek() to request bufsize (returned in pcbAvailable)


// IPeek_Exhausted(code) : TRUE when code indicates the source
//    will never return any more data.
//
static __inline boolean IPeek_Exhausted(int32 nCode)
{
   return ( (unsigned)-nCode < 2U);
}

#define INHERIT_IPeek(iname)\
   INHERIT_ISource(iname);\
   char  *(*Peek)     (iname *po, int32 cbWanted, int32 *pcbAvailable, int32 *pnCode);\
   void   (*Peekable) (iname *po, AEECallback *pcb);\
   void   (*Advance)  (iname *po, int32 cb)

#define AEEIID_IPeek 0x0100101C

/*  declare the actual IPeek interface */
AEEINTERFACE_DEFINE(IPeek);


static __inline uint32 IPeek_AddRef(IPeek *p)
{
   return AEEGETPVTBL(p, IPeek)->AddRef((p));
}
static __inline uint32 IPeek_Release(IPeek *p)
{
   return AEEGETPVTBL(p, IPeek)->Release((p));
}
static __inline int IPeek_QueryInterface(IPeek *p, AEEIID i, void **o)
{
   return AEEGETPVTBL(p, IPeek)->QueryInterface((p),(i),(o));
}
static __inline int32 IPeek_Read(IPeek *p, char *pc, int32 cb)
{
   return AEEGETPVTBL(p, IPeek)->Read((p),(pc),(cb));
}
static __inline void IPeek_Readable(IPeek *p, AEECallback *pcb)
{
   AEEGETPVTBL(p, IPeek)->Readable((p),(pcb));
}
static __inline char * IPeek_Peek(IPeek *p, int32 cb, int32 *pcb, int32 *pn)
{
   return AEEGETPVTBL(p, IPeek)->Peek((p),(cb),(pcb),(pn));
}
static __inline void IPeek_Peekable(IPeek *p, AEECallback *pcb)
{
   AEEGETPVTBL(p, IPeek)->Peekable((p),(pcb));
}
static __inline void IPeek_Advance(IPeek *p, int32 cb)
{
   AEEGETPVTBL(p, IPeek)->Advance((p),(cb));
}



/*
=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

IPeek Interface

The Peek Interface is an abstract data source that provides a Peek/Advance
functionality that can be used to accumulate data in a buffer owned by the
IPeek Interface object.  Data can be examined and sometimes modified
before being consumed, as long as callers don't try to modify peeked data
in an IPeek function if it's data received using
ISOURCEUTIL_PeekFromMemory() was created on const data.

=============================================================================


IPeek_AddRef()


Description:
   This function is inherited from IQI_AddRef().


See Also:

   IPeek_Release()


=============================================================================


IPeek_Advance()


Description:
   This function advances or retreats the peek pointer in the buffer.


Prototype:
   void IPeek_Advance(
      IPeek *  pIPeek,
      int      cbAdvance
   );


Parameters:

   pIPeek [in]
      Pointer to the IPeek Interface object.

   cbAdvance  [in]
      Number of bytes to advance (or retreat, when negative).


Return Value:
   None


Comments:
   Numbers larger than the number of buffered characters safely advance to
   the end of all buffered characters.


See Also:
   None


=============================================================================


IPEEK_Exhausted()


Description:
   This function determines if an IPeek is out of data.


Definition:
   IPEEK_Exhausted(code)((unsigned) - (code) < 2U)


Parameters:

   code
      Return code from Read(0), or Peek().


Return Value:

   TRUE, if the source is exhausted (no more data).

   FALSE, if otherwise.


Comments:
   There may still be data returned. Check the cbAvailable parameter in _Peek()


See Also:
   None


=============================================================================


IPeek_Peek()


Description:
   This function returns a pointer to the buffered data, and optionally tries
   to read more data into the buffer.


Prototype:

   char * IPeek_Peek(
      IPeek *  pIPeek,
      int32    cbWanted,
      int32 *  pcbAvailable,
      int32 *  pnCode
   );


Parameters:

   pIPeek [in]
      Pointer to the IPeek Interface object.

   cbWanted [in]

      Number of bytes specified.  If this many bytes are already
      available in the buffer, Peek() does no extra work; otherwise it
      tries to copy more data from the source.  Pass 0 (zero) to
      quickly query the current buffer status.  IPEEK_PEEKMAX is
      defined to represent "return as much as possible".
      IPEEK_GETBUFSIZE is defined to represent the amount of data
      buffering this IPeek is capable of.

   pcbAvailable [out]
      Size of the buffered data.

   pnCode [out]
      Describes the availability of data beyond what was returned:

      IPEEK_END, No more data (END was encountered in the stream).

      IPEEK_ERROR, No more data (ERROR was encountered in the stream).

      IPEEK_FULL, Data limited by buffer size; must advance over buffered data
         in order to obtain new data.

      IPEEK_WAIT, Wait on IPeek_Peekable() before calling again; data source was
         polled and None is available now.

      IPEEK_UNK, Amount requested was less than amount buffered; call again with
         larger cbWanted; data source was not polled.



Return Value:
   Start of the buffered data.


Comments:
   None


See Also:
   None


=============================================================================


IPeek_Peekable()


Description:
   This function calls a callback when more unbuffered data is available.
   After the callback, IPeek_Peek() may return more data than it did before.


Prototype:

   void IPeek_Peekable(
      IPeek *        pIPeek,
      AEECallback *  pcb
   );


Parameters:

   pIPeek
      Pointer to the IPeek Interface object.

   pcb
      Pointer to where to call when the IPeek may be readable.


Return Value:
   None


Comments:
   The presence of buffered data does not result in an immediate callback.
   Be sure to call this function only after determining that the
   currently-buffered data is insufficient to proceed.


See Also:
   None


=============================================================================

IPeek_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:

    IPeek_Release()

=============================================================================

IPeek_Read()

Description:
   This function is inherited from ISource_Read(). 

See Also:


=============================================================================

IPeek_Readable()

Description:
   This function is inherited from ISource_Readable(). 

See Also:


=============================================================================


IPeek_Release()


Description:
   This function is inherited from IQI_Release().


See Also:

   IPeek_AddRef()


===========================================================================
*/

#endif // AEEIPEEK_H
