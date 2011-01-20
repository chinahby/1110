#ifndef AEESOURCE_H
#define AEESOURCE_H
/*======================================================
FILE:  AEESource.h

SERVICES:
        ISource, IGetLine

GENERAL DESCRIPTION:
        Buffered IO functionality, header file

=====================================================

Copyright © 2001-2004,2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary

=====================================================*/

#include "AEE.h"
#include "AEENet.h"


/*
  ||
  ||  ISource : Abstract data source
  ||
*/

#include "../../inc/AEEISource.h"

// ### Deprecated ### - Use INHERIT_ISource
#define DECLARE_ISOURCE(iname)\
   int32 (*Read)    (iname *po, char *pcBuf, int32 cbBuf);\
   void  (*Readable)(iname *po, AEECallback *pcb)


#define ISOURCE_AddRef           ISource_AddRef
#define ISOURCE_Release          ISource_Release
#define ISOURCE_QueryInterface   ISource_QueryInterface
#define ISOURCE_Read             ISource_Read
#define ISOURCE_Readable         ISource_Readable
#define ISOURCE_Exhausted(code)  ((unsigned) -(code) < 2U)

/*
  ||
  ||  IPeek : Abstract data source, buffer management
  ||
*/

#include "../../inc/AEEIPeek.h"

// ### Deprecated ### - Use INHERIT_IPeek
#define DECLARE_IPEEK(iname)\
   char  *(*Peek)     (iname *po, int32 cbWanted, int32 *pcbAvailable, int32 *pnCode);\
   void   (*Peekable) (iname *po, AEECallback *pcb);\
   void   (*Advance)  (iname *po, int32 cb)

#define IPEEK_AddRef             IPeek_AddRef
#define IPEEK_Release            IPeek_Release
#define IPEEK_QueryInterface     IPeek_QueryInterface
#define IPEEK_Read               IPeek_Read
#define IPEEK_Readable           IPeek_Readable
#define IPEEK_Peek               IPeek_Peek
#define IPEEK_Peekable           IPeek_Peekable
#define IPEEK_Advance            IPeek_Advance
#define IPEEK_Exhausted(code)    ((unsigned) -(code) < 2U)


/*
  ||
  ||  IGetLine : Abstract data source, buffer management, line parsing
  ||
*/

/*
  || Exit codes for IGetLine member functions:
 */
#define IGETLINE_CRLF    4             // GetLine:  CRLF line ending was seen
#define IGETLINE_LF      3             // GetLine:  LF line ending was seen
#define IGETLINE_CR      2             // GetLine:  CR line ending was seen (not supported yet)
#define IGETLINE_END     ISOURCE_END   // Read/Peek/GetLine:  end of stream
#define IGETLINE_ERROR   ISOURCE_ERROR // Read/Peek/GetLine:  error in stream; no more data is available
#define IGETLINE_WAIT    ISOURCE_WAIT  // Read/Peek/GetLine:  try again later after calling Readable
#define IGETLINE_FULL    IPEEK_FULL    // Peek/GetLine:  buffer is full
#define IGETLINE_UNK     IPEEK_UNK     // Peek/GetLine:  did not call Read()


#define IGETLINE_PEEKMAX     IPEEK_PEEKMAX // pass this to Peek() to request "as much data as possible"
#define IGETLINE_GETBUFSIZE  IPEEK_GETBUFSIZE // pass this to Peek() to request bufsize (returned in pcbAvailable)


// IGETLINE_LineComplete() : Tests GetLine exit codes, and evaluates to
//    TRUE if a complete line was received (i.e. the line was not
//    truncated by buffer limits or by an error).
//
#define IGETLINE_LineComplete(code)   ( (code) >= 0 )


// IGETLINE_EOLSize() : Gives length of the EOL sequence that terminated
//    the line just returned by GetLine, give the result code.  EOL
//    sequences are stripped by GetLine before lines are returned.
//    Possible values are:
//        2 : CRLF seen
//        1 : single CR or LF seen
//        0 : line ended at END, ERROR, WAIT, or FULL exit code.
//
#define IGETLINE_EOLSize(code)    ( (code) > 0 ? ((code) >> 1) : 0)


// IGETLINE_Exhausted(code) : TRUE when code indicates the source
//    will never return any more data.
//
#define IGETLINE_Exhausted(code) ISOURCE_Exhausted(code)


typedef struct GetLine {
   char    *psz;        /* line */
   int32    nLen;       /* len of psz, psz is \0 terminated, but _may_ also
                           _contain_ \0 */
   boolean  bLeftover;  /* TRUE if part of previous line */
   boolean  bTruncated; /* TRUE if incomplete (out of read buffer space) */
} GetLine;

// ### Deprecated ### - Use INHERIT_IGetline
#define DECLARE_IGETLINE(iname)\
   int32  (*GetLine)  (iname *po, struct GetLine *pgl, int32 nTypeEOL);\
   void   (*UngetLine)(iname *po, int32 nLenUnused)

#define INHERIT_IGetline(iname)\
   INHERIT_IPeek(iname);\
   int32  (*GetLine)  (iname *po, struct GetLine *pgl, int32 nTypeEOL);\
   void   (*UngetLine)(iname *po, int32 nLenUnused)

typedef struct IGetLine IGetLine;
AEEINTERFACE(IGetLine) {
   INHERIT_IGetline(IGetLine);
};


#define IGETLINE_AddRef(p)             AEEGETPVTBL((p),IGetLine)->AddRef((p))
#define IGETLINE_Release(p)            AEEGETPVTBL((p),IGetLine)->Release((p))
#define IGETLINE_QueryInterface(p,i,o) AEEGETPVTBL((p),IGetLine)->QueryInterface((p),(i),(o))
#define IGETLINE_Read(p,pc,cb)         AEEGETPVTBL((p),IGetLine)->Read((p),(pc),(cb))
#define IGETLINE_Readable(p,pcb)       AEEGETPVTBL((p),IGetLine)->Readable((p),(pcb))
#define IGETLINE_Peek(p,cb,pcb,pn)     AEEGETPVTBL((p),IGetLine)->Peek((p),(cb),(pcb),(pn))
#define IGETLINE_Peekable(p,pcb)       AEEGETPVTBL((p),IGetLine)->Peekable((p),(pcb))
#define IGETLINE_Advance(p,cb)         AEEGETPVTBL((p),IGetLine)->Advance((p),(cb))
#define IGETLINE_GetLine(p,pr,n)       AEEGETPVTBL((p),IGetLine)->GetLine((p),(pr),(n))
#define IGETLINE_UngetLine(p,n)        AEEGETPVTBL((p),IGetLine)->UngetLine((p),(n))



/*
  ||
  ||  ISourceUtil : utility for transformations, etc.
  ||
*/
typedef struct ISourceUtil ISourceUtil;
AEEINTERFACE(ISourceUtil) {
   INHERIT_IQueryInterface(ISourceUtil);

   int32 (*GetLineFromSource)(ISourceUtil *po, ISource *pis,
                              int32 nBufSize, IGetLine **ppigl);
   int32 (*SourceFromSocket)(ISourceUtil *po, ISocket *ps, ISource **ppis);
   int32 (*SourceFromMemory)(ISourceUtil *po, const void *pBuf, int32 nBufLen,
                             PFNNOTIFY pUserFreeFn, void *pUserFreeData,
                             ISource **ppis);
   int32 (*SourceFromAStream)(ISourceUtil *po, IAStream *pias, ISource **ppis);
   int32 (*AStreamFromSource)(ISourceUtil *po, ISource *pis, IAStream **ppias);
   int32 (*PeekFromMemory)   (ISourceUtil *po, const void *pBuf, int32 nBufLen,
                              PFNNOTIFY pUserFreeFn, void *pUserFreeData,
                              IPeek **ppip);
   int32 (*PeekFromSource)   (ISourceUtil *po, ISource *pis,
                              int32 nBufSize, IPeek **ppip);
};

#define ISOURCEUTIL_AddRef(p)                        AEEGETPVTBL((p),ISourceUtil)->AddRef((p))
#define ISOURCEUTIL_Release(p)                       AEEGETPVTBL((p),ISourceUtil)->Release((p))
#define ISOURCEUTIL_QueryInterface(p,i,o)            AEEGETPVTBL((p),ISourceUtil)->QueryInterface((p),(i),(o))
#define ISOURCEUTIL_GetLineFromSource(p,r,n,pp)      AEEGETPVTBL((p),ISourceUtil)->GetLineFromSource((p),(r),(n),(pp))
#define ISOURCEUTIL_SourceFromSocket(p,s,pp)         AEEGETPVTBL((p),ISourceUtil)->SourceFromSocket((p),(s),(pp))
#define ISOURCEUTIL_SourceFromMemory(p,b,l,cf,cx,pp) AEEGETPVTBL((p),ISourceUtil)->SourceFromMemory((p),(b),(l),(cf),(cx),(pp))
#define ISOURCEUTIL_SourceFromAStream(p,as,pp)       AEEGETPVTBL((p),ISourceUtil)->SourceFromAStream((p),(as),(pp))
#define ISOURCEUTIL_AStreamFromSource(p,s,pp)        AEEGETPVTBL((p),ISourceUtil)->AStreamFromSource((p),(s),(pp))
#define ISOURCEUTIL_PeekFromMemory(p,b,l,cf,cx,pp)   AEEGETPVTBL((p),ISourceUtil)->PeekFromMemory((p),(b),(l),(cf),(cx),(pp))
#define ISOURCEUTIL_PeekFromSource(p,r,n,pp)         AEEGETPVTBL((p),ISourceUtil)->PeekFromSource((p),(r),(n),(pp))


/*=====================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

IGetLine Interface

The IGetLine Interface provides functions for line parsing data.  IGetLine
uses a buffer (allocated at creation) to accumulate the data for the lines
it returns.



=============================================================================


IGETLINE_AddRef()


Description:

   This function is inherited from IBASE_AddRef().

Version:
   Introduced BREW Client 1.1

See Also:
   IGETLINE_Release()


=============================================================================


IGETLINE_Advance()


Description:

   This function advances or retreats the peek pointer in the buffer.


Prototype:

   void IGETLINE_Advance(
      IGetLine *  pIGetLine,
      int32       cbAdvance
   );


Parameters:

   pIGetLine [in]
      Pointer to the IGetLine Interface object.

   cbAdvance [in]
      Number of bytes to advance (or retreat, when negative).


Return Value:

   None


Comments:

   Numbers larger than the number of buffered characters safely advance to
   the end of all buffered characters.

Version:
   Introduced BREW Client 1.1

See Also:
   None


=============================================================================


IGETLINE_EOLSize()


Description:

   This macro returns the length of the EOL sequence that terminated the line returned
   by GetLine, given the result code. EOL sequences are stripped by GetLine before
   lines are returned.


Definition:

   IGETLINE_EOLSize(code)((code) > 0 ? ((code) >> 1): 0)


Parameters:

   code
      Return value from GetLine().


Return Value:

   2  CRLF seen.

   1  single CR or LF seen.

   0  line ended at END, ERROR, WAIT, or FULL exit code.


Comments:

   None


See Also:

   IGETLINE_GetLine()


=============================================================================


IGETLINE_Exhausted()


Description:

   This macro determines if an IGetLine is out of data.


Definition:

   IGETLINE_Exhausted(code)((unsigned) - (code) < 2U)


Parameters:

   code
      Return value from GetLine().


Return Value:

   TRUE, if the source is exhausted (no more data).

   FALSE, if otherwise.


Comments:

   There may still be data returned. Check the GetLine data structure.


See Also:

   None


=============================================================================


IGETLINE_GetLine()


Description:

   This function gets the next logical line of text from the data source.  A
   logical line is a sequence of bytes terminated by a line terminator
   character sequence (EOL).  The currently supported line terminators are CR
   and CRLF.  This function also returns lines that are improperly
   terminated, and partial lines when the logical line length exceeds the
   available buffer size.  Return codes indicate the reason for return, so
   the exact binary form of the input stream can be reconstructed, if
   necessary.

   IGETLINE_GetLine() is a synchronous call.  It returns what information is
   available at present.  If it needs to buffer more data before it can
   determine where the next line ends, it will return IGETLINE_WAIT, and the
   caller will have to try again later.  After receiving a return code of
   IGETLINE_WAIT, the caller can use the IGETLINE_Peekable() member function
   to be notified of when IGETLINE_GetLine() should be tried again.  In all
   other cases, IGETLINE_GetLine() should be called without waiting on
   IGETLINE_Peekable() so that it can examine any buffered data and determine
   whether the caller should wait on IGETLINE_Peekable().


Prototype:

   int32 IGETLINE_GetLine(
      IGetLine *  pIGetLine,
      GetLine *   pline,
      int32       nTypeEOL
   );


Parameters:

   pIGetLine
      Pointer to the IGetLine Interface object.

   pline
      Pointer to structure to hold results. On exit:

      pline->psz = Pointer the start of line data (zero-terminated).

      pline->nLen = Number of characters in line.

      pline->bLeftover = TRUE when the returned text is the continuation
         of a previous truncated line.  In other words, the last return
         code  was IGETLINE_FULL (ignoring IGETLINE_WAIT return codes).

      The text described by *pline does not include any EOL characters.
      If the line was terminated by an EOL character sequence, GetLine
      writes a NULL character over the initial EOL character to ensure
      zero termination

      nLen is the same as strlen(psz), unless the line contains a NULL
      character.  GetLine() does not restrict the contents of lines, so
      returned lines may contain binary data (except, of course, a valid
      EOL sequence).

      All of the line data, including the EOL characters, can be modified
      by the caller, so the original binary form, including EOL
      characters, can be reconstructed, if necessary.

   nTypeEOL
      Specifies which EOL sequences should be recognized:

      IGETLINE_CRLF = Only CRLF sequences terminate lines.

      IGETLINE_LF = CRLF or single LF terminate lines.

      IGETLINE_CR = CRLF, single LF, or single CR terminate lines.

      This option is not fully supported; it is equivalent to IGETLINE_LF.


Return Value:

   One of the following exit codes, describing the reason GetLine returned:

   IGETLINE_WAIT, if the line is not yet complete; try again.

      In this case, *pline describes the available data (the incomplete
      line), but the data is not processed.  Callers should use
      IGETLINE_Peekable() to be schedule the next call to
      IGETLINE_GetLine().

      In all other cases, the data described in *pline is consumed, and
      IGETLINE_Peekable() should not be used.  IGETLINE_Readable() or
      ISHELL_Resume() can be used instead.

   IGETLINE_CRLF, the line was terminated at a two-character EOL (CRLF).

   IGETLINE_CR, the line was terminated at a single-character EOL (CR) [not implemented].

   IGETLINE_LF, the line was terminated at a single-character EOL (LF).

   IGETLINE_END, the line ended at end of stream; no EOL seen.

   IGETLINE_ERROR, the line ended at ERROR; no EOL seen.

   IGETLINE_FULL, the data was limited by buffer size; no EOL seen; remainder to follow.


Comments:

   In the case of IGETLINE_WAIT, the contents (if any) of the incomplete line
   are not processed.  This means that subsequent calls to IGETLINE_Read(),
   IGETLINE_Peek(), or IGETLINE_GetLine() will return that data.

   In all other cases, the data is consumed.  This means that
   IGETLINE_GetLine() advances the read pointer past the contents of the
   line, including the line ending, so that subsequent IGETLINE_Read(),
   IGETLINE_Peek(), or IGETLINE_GetLine() calls will return data that follows
   the line.

   IGETLINE_END or IGETLINE_ERROR are the only two cases that indicate that
   there will be no more data.  IGETLINE_Exhausted() can be handy for this.

   IGETLINE_UngetLine() is provided to restore the stream to the exact binary
   state it was in before the line was consumed.  This is mainly provided for
   HTTP 0.9 compatibility.  IGETLINE_UngetLine() should be passed the same
   length value returned by the previous IGETLINE_GetLine() call, and no
   other member functions should have been call in the interim, otherwise the
   results are undetermined.

   IGETLINE_LineComplete(code) can be used to test for normal cases where a a
   line is terminated by a valid EOL sequence or by the end of the stream.
   Returns TRUE for {IGETLINE_END, IGETLINE_CR, IGETLINE_LF, IGETLINE_CRLF}

   In the IGETLINE_WAIT case, IGETLINE_Readable() does not provide
   appropriate notifications of when to retry IGETLINE_GetLine().
   IGETLINE_Readable() indicates readiness for IGETLINE_Read(), and therefore
   notifies of unbuffered *or* buffered data.  Whereas IGETLINE_Read() will
   succeed whenever there is buffered data, GetLine() in this case will not
   -- it has already examined the buffered data and determined there is not
   enough to complete a line.  Calling IGETLINE_Readable() after an
   IGETLINE_WAIT result can result in repeated callbacks that make no
   progress, preventing the application from properly yielding time to other
   applications.  IGETLINE_Peekable(), which notifies of new unbuffered data,
   will properly yield time until IGETLINE_GetLine() needs to be called
   again.

   Before the initial call to IGETLINE_GetLine() or after result codes other
   than IGETLINE_WAIT, either IGETLINE_Readable() or ISHELL_Resume() can be
   used to schedule the call to IGETLINE_GetLine().  Either of those
   functions will allow IGETLINE_GetLine() to examine any buffered data
   before waiting.


See Also:

   GetLine


=============================================================================


IGETLINE_LineComplete()


Description:

   This macro determines if a complete line was returned.


Definition:

   IGETLINE_LineComplete(code)((code) >= 0)


Parameters:

   code
      Return value of _GetLine().


Return Value:

   TRUE, if a complete line was received (such as when the line is not truncated by
   buffer limits or an error).

   FALSE, if otherwise.


Comments:

   The bTruncated field of the GetLine structure is also be set.


See Also:

   IGETLINE_GetLine()


=============================================================================


IGETLINE_Peek()


Description:

   This function returns a pointer to the buffered data, and optionally tries to read
   more data into the buffer.


Prototype:

   char * IGETLINE_Peek(
      IGetLine *  pIGetLine,
      int32       cbWanted,
      int32 *     pcbAvailable,
      int32 *     pnCode
   );


Parameters:

   pIGetLine [in]
      Pointer to the IGetLine Interface object.

   cbWanted [in]
      Number of bytes specified.  If this many bytes are already available
      in the buffer, Peek() does no extra work; otherwise, it tries to
      copy more data from the network or other original source.

      Pass 0 to quickly query the current buffer status.  IGETLINE_PEEKMAX
      is defined to represent "return as much as possible".
      IGETLINE_GETBUFSIZE is defined to represent the amount of data
      buffering this IGetLine is capable of.

   pcbAvailable [out]
      Size of the buffered data.

   pnCode [out]
      Describes the availability of data beyond what was returned:

      IGETLINE_END, No more data (END was encountered in the stream).

      IGETLINE_ERROR, No more data (ERROR was encountered in the stream).

      IGETLINE_FULL, Data limited by buffer size.  Must advance over
      buffered data to obtain new data.

      IGETLINE_WAIT, Wait on IGETLINE_Peekable() before calling again.
      Data source was polled and None is available now.

      IGETLINE_UNK, Amount requested was less than amount buffered.  Call
      again with larger cbWanted.  Data source was not polled.



Return Value:

   The character at the start of buffered data.


Comments:

   None


See Also:

   None


=============================================================================


IGETLINE_Peekable()


Description:

   This function calls a callback when more unbuffered data is available.
   After the callback, IGETLINE_Peek() may return more data than it did
   before.

Prototype:

   void IGETLINE_Peekable(
      IGetLine *     pIGetLine,
      AEECallback *  pcb
   );


Parameters:

   pIGetLine
      Pointer to the IGetLine Interface object.

   pcb
      Pointer to where to call when the IGetLine may be readable.


Return Value:

   None


Comments:

   The presence of buffered data does not result in an immediate
   callback.  Be sure to call this only after you determine that the
   currently buffered data is insufficient to proceed.


See Also:

   None


=============================================================================


IGETLINE_QueryInterface()


Description:

   This function asks an object for another API contract from that object.


Prototype:

   int IGETLINE_QueryInterface(
      IGetLine *  pIGetLine,
      AEECLSID    idReq,
      void **     ppo
   );


Parameters:

   pIGetLine [in]
      Pointer to the IGetLine Interface object.

   idReq [in]
      Requested ClassID exposed by the object.

   ppo [out]
      Returned object. Filled by this function.


Return Value:

   SUCCESS, Interface found.

   ENOMEMORY, Insufficient memory.

   ECLASSNOTSUPPORT, Requested interface is not supported.


Comments:

   If *ppo is an interface pointer, then the pointer in *ppo is set to
   the new interface (with refcount incremented), or NULL if the
   ClassID is not supported by the object.

   If *ppo is a data structure pointer, then *ppo is set to the
   internal data represented by the classID or set to NULL if classID
   is not supported by the object.


See Also:

   AEECLSID List


=============================================================================


IGETLINE_Read()


Description:

   This function copies bytes from the source stream to the specified
   buffer.  All copied bytes are consumed (that is, the read pointer is
   advanced).


Prototype:

   int IGETLINE_Read(
      IGetLine *  pIGetLine,
      char *      pcBuf,
      int32       cbBuf
   );


Parameters:

   pIGetLine
      Pointer to the IGetLine Interface object.

   pcBuf
      Pointer to buffer to receive bytes read.

   cbBuf
      sizeof(pc).


Return Value:

   Number of bytes read, if successful.  This will be a value greater than
   zero.

   One of the following, if fails:

      IGETLINE_END, peer shut down connection gracefully (no more data).

      IGETLINE_ERROR, error was encountered (no more data).

      IGETLINE_WAIT, no data available at the moment; call Readable() to wait.


Comments:

   The IGETLINE_WAIT return value is used instead of an error return
   value and an EWOULDBLOCK error code.


See Also:

   None


=============================================================================


IGETLINE_Readable()


Description:

   This function schedules or cancels a callback when IGETLINE_Read()
   should return something other than IGETLINE_WAIT.


Prototype:

   void IGETLINE_Readable(
      IGetLine *     pIGetLine,
      AEECallback *  pcb
   );


Parameters:

   pIGetLine
      Pointer to the IGetLine Interface object.

   pcb
      Where to call when the IGetLine may be readable.


Return Value:

   None


Comments:

   None


See Also:

   PFNNOTIFY


=============================================================================


IGETLINE_Release()


Description:

   This function is inherited from IBASE_Release().


See Also:
   IGETLINE_AddRef()

=============================================================================


IGETLINE_UngetLine()


Description:

   This function lets go of (ungets) some or all of the last logical
   line from the data source and replace the line terminators in the
   stream.


Prototype:

   void IGETLINE_UngetLine(
      IGetLine *  pIGetLine,
      int32       nLenUnused
   );


Parameters:

   pIGetLine
      Pointer to the IGetLine Interface object.

   nLenUnused
      Specifies how much (up to the length of the last line) to let go of
      (unget).  This number must be less than or equal to nLen from the
      GetLine structure filled by the last call to GetLine(), i.e.  it
      must *not* include the length of line terminator(s).


Return Value:

   None


Comments:

   IGETLINE_UngetLine() works only once until the next call to
   IGETLINE_GetLine(), calling IGETLINE_UngetLine() with a number
   greater than the last line length or calling IGETLINE_UngetLine()
   twice results in undefined behavior.


See Also:

   None



=============================================================================
   DATA STRUCTURES DOCUMENTATION
===========================================================================


GetLine


Description:

   Type passed to IGETLINE_GetLine to hold results of line-parsing


Definition:

   typedef struct GetLine {
      char    *   psz;
      int32       nLen;
      boolean     bLeftover;
      boolean     bTruncated;
   } GetLine;

Members:

   psz
      null-terminated pointer to the parsed line, not including
      any line terminators

   nLen
      length of psz, psz is \0 terminated, but may also
      contain \0

   bLeftover
      TRUE if is continuation of previous line

   bTruncated
      TRUE if incomplete (out of read buffer space,
      line termination chars not found)


Comments:

   None


See Also:

   None


=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

ISourceUtil Interface

This interface constructs common sources.



=============================================================================


ISOURCEUTIL_AddRef()


Description:
   This function is inherited from IBASE_AddRef().

Version:
   Introduced BREW Client 1.1

See Also:
   ISOURCEUTIL_Release()


=============================================================================


ISOURCEUTIL_AStreamFromSource()


Description:
   This function makes a new IAStream Interface object that gets data from an
   ISource Interface object.


Prototype:

   int32 ISOURCEUTIL_AStreamFromSource(
      ISourceUtil *  pISourceUtil,
      ISource *      pis,
      IAStream **    ppias
   );


Parameters:

   pISourceUtil [in]
      Pointer to the ISourceUtil Interface object.

   pis [in]
      Data source to read from.

   ppias [out]
      Pointer to the interface pointer to be filled with resulting IAStream
      Interface object.


Return Value:

   SUCCESS, if successful.

   ENOMEMORY, if the function can't make a new IAStream Interface object.


Comments:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   ISOURCEUTIL_SourceFromAStream()


=============================================================================


ISOURCEUTIL_GetLineFromSource()


Description:
   This function makes a new IGetLine Interface object that gets data from an
   ISource Interface object.


Prototype:

   int32 ISOURCEUTIL_GetLineFromSource(
      ISourceUtil *  pISourceUtil,
      ISource *      pirs,
      int32          nBufSize,
      IGetLine **    ppigl
   );


Parameters:

   pISourceUtil [in]
      Pointer to the ISourceUtil Interface object.

   pirs [in]
      Data source.

   nBufSize [in]
      Amount of memory to try to allocate for buffering, should have some relevance to
      data being read.

   ppigl [out]
      Pointer to the interface pointer to be filled with resulting IGetLine
      Interface object.


Return Value:

   SUCCESS, if successful.

   ENOMEMORY, if the function can't make a new IGetLine Interface object.


Comments:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   ISOURCEUTIL_PeekFromMemory()


=============================================================================


ISOURCEUTIL_PeekFromMemory()


Description:
   This function makes a new IPeek Interface object that gets data from a memory block.


Prototype:

   int32 ISOURCEUTIL_PeekFromMemory(
      ISourceUtil *  pISourceUtil,
      const void *   pBuf,
      int32          nBufLen,
      PFNNOTIFY      pUserFreeFn,
      void *         pUserFreeData,
      IPeek **       ppip
   );


Parameters:

   pISourceUtil [in]
      Pointer to the ISourceUtil Interface object.

   pBuf [in]
      Buffer that is the data.

   nBufLen [in]
      Number of bytes in pBuf.

   pUserFreeFn [in]
      Where to call when finished with the memory, may be NULL.

   pUserFreeData [in]
      What to call pUserFreeFn with.

   ppip [out]
      Pointer to the interface pointer to be filled with resulting IPeek
      Interface object.


Return Value:

   SUCCESS, if successful.

   ENOMEMORY, if the function can't make a new IPeek Interface object.


Comments:
   If the IPeek Interface object is created with read-only memory, be careful
   not to modify buffered data returned by IPEEK_Peek().

Version:
   Introduced BREW Client 1.1

See Also:
   ISOURCEUTIL_SourceFromAStream()


=============================================================================


ISOURCEUTIL_PeekFromSource()


Description:
   This function makes a new IPeek Interface object that gets data from an
   ISource Interface object.


Prototype:

   int32 ISOURCEUTIL_PeekFromSource(
      ISourceUtil *  pISourceUtil,
      ISource *      pirs,
      int32          nBufSize,
      IPeek **       ppips
   );


Parameters:

   pISourceUtil [in]
      Pointer to the ISourceUtil Interface object.

   pirs [in]
      ISource Interface object that is source of data.

   nBufSize [in]
      Amount of memory to try to allocate for buffering, should have some
      relevance to data being read.

   ppips [in/out]
      Pointer to the interface pointer to be filled with resulting IPeek
      Interface object.



Return Value:

   SUCCESS, if successful.

   ENOMEMORY, if the function can't make a new IPeek Interface object.


Comments:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   None


=============================================================================


ISOURCEUTIL_QueryInterface()

This function is inherited from IQI_QueryInterface()


Description:
   This function asks an object for another API contract from the object in
   question.


Prototype:
   int IQI_QueryInterface(
      IQueryInterface * pIQueryInterface,
      AEECLSID          idReq,
      void **           ppo
   );


Parameters:

   pIQueryInterface [in]
      Pointer to the IQueryInterface object.

   idReq [in]
      Requested ClassID exposed by the object.

   ppo [in/out]
      Returned object. Filled by this function.


Return Value:

   SUCCESS, interface found.

   ENOMEMORY, insufficient memory.

   ECLASSNOTSUPPORT, requested interface is unsupported.


Comments:

   If *ppo is an interface pointer, then the pointer in *ppo is set to the new
   interface (with refcount incremented), or NULL if the ClassID is not supported by
   the object.

   If *ppo is a data structure pointer, then *ppo is set to the internal data
   represended by the classID or set to NULL if classID is not supported by
   the object.

Version:
   Introduced BREW Client 2.1


See Also:
   None


=============================================================================


ISOURCEUTIL_Release()


Description:
   This function is inherited from IBASE_Release().


See Also:
   ISOURCEUTIL_AddRef()


=============================================================================


ISOURCEUTIL_SourceFromAStream()


Description:
   This function makes a new ISource Interface object that gets data from an
   IAStream Interface object.


Prototype:

   int32 ISOURCEUTIL_SourceFromAStream(
      ISourceUtil *  pISourceUtil,
      IAStream *     pias,
      ISource **     ppis
   );


Parameters:

   pISourceUtil [in]
      Pointer to the ISourceUtil Interface object.

   pias [in]
      IAStream Interface object to read from.

   ppis [out]
      Pointer to the interface pointer to be filled with resulting ISource
      Interface object.




Return Value:
   SUCCESS, if successful.

   ENOMEMORY, if the function can't make a new ISource Interface object.


Comments:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   ISOURCEUTIL_AStreamFromSource()


=============================================================================


ISOURCEUTIL_SourceFromMemory()


Description:
   This function makes a new ISource Interface object that gets data from a memory
   block.


Prototype:

   int32 ISOURCEUTIL_SourceFromMemory(
      ISourceUtil *  pISourceUtil,
      const void *   pBuf,
      int32          nBufLen,
      PFNNOTIFY      pUserFreeFn,
      void *         pUserFreeData,
      ISource **     ppis
   );


Parameters:

   pISourceUtil [in]
      Pointer to the ISourceUtil Interface object.

   pBuf [in]
      Data buffer.

   nBufLen [in]
      Number of bytes in pBuf.

   pUserFreeFn [in]
      Where to call when finished with the memory, may be NULL.

   pUserFreeData [in]
      What to call pUserFreeFn with.

   ppirs [in/out]
      Pointer to the interface pointer to be filled with resulting ISource Interface
      object.


Return Value:

   SUCCESS, if successful.

   ENOMEMORY, if the function can't make a new ISource Interface object.


Comments:
   None

Version:
   Introduced BREW Client 1.1

See Also:
   PFNNOTIFY
   FREE()


=============================================================================


ISOURCEUTIL_SourceFromSocket()


Description:
   This function makes a new ISource Interface object that gets data from an
   ISocket Interface object.


Prototype:
   int32 ISOURCEUTIL_SourceFromSocket(
      ISourceUtil *  pISourceUtil,
      ISocket *      ps,
      ISource **     ppirs
   );


Parameters:

   pISourceUtil [in]
      Pointer to the ISourceUtil Interface object.

   ps [in]
      ISocket Interface object to read from.

   ppirs [out]
      Pointer to the interface pointer to be filled with resulting ISource
      Interface object.


Return Value:

   SUCCESS, if successful.

   ENOMEMORY, if the function can't make a new ISource Interface object.


Comments:
   User can query to see if data is available in socket connection
   without reading the data. This can be done by obtaining the ISource
   object from the ISocket object by calling ISOURCEUTIL_SourceFromSocket()
   and then to get the IPeek object from ISource object by calling
   ISOURCEUTIL_PeekFromSource() then to use IPeek interface to check on the
   data.
   Note that technically the data is consumed out of the actual "socket"
   source and put into the IPeek buffer, but the data can be retrieved even
   after checking it out. When IPEEK_Advance is called the data will be
   considered "consumed" fully.

Version:
   Introduced BREW Client 1.1

See Also:
   None


=============================================================================

===========================================================================*/



#endif /* #ifndef AEESOURCE_H */
