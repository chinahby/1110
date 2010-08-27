#ifndef _AEEFIFO_H_
#define _AEEFIFO_H_

/*======================================================
FILE:      AEEFIFO.h

SERVICES:  FIFO (Named Pipes).

GENERAL DESCRIPTION:

  IFIFO provides a simple interface to a FIFO, allowing interprocess
  communication via a kernel buffer with a well known name.

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IFIFO

INITIALIZATION & SEQUENCING REQUIREMENTS:

	See Exported Routines

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

/*===========================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===========================================================================*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEEPort.h"
#include "AEEIOCtl.h"

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

//
//  "fifo:/" namespace macros
//
#define AEEFIFO_ROOT_DIR     "fifo:/" // ALL fifo names must start with this prefix
#define AEEFIFO_HOME_DIR     AEEFIFO_ROOT_DIR "~/"
#define AEEFIFO_SYS_DIR      AEEFIFO_ROOT_DIR "sys/"
#define AEEFIFO_SHARED_DIR   AEEFIFO_ROOT_DIR "shared/"

#define AEEFIFO_MODE_R  (0x01)
#define AEEFIFO_MODE_W  (0x02)
#define AEEFIFO_MODE_RW (AEEFIFO_MODE_R|AEEFIFO_MODE_W)

#define AEEFIFO_IOCTL_GETBUFSIZE  BREWIOCTL_RW(BREWIOCTL_FIFO_TYPE, 1, uint32)
#define AEEFIFO_IOCTL_SETBUFSIZE  BREWIOCTL(BREWIOCTL_FIFO_TYPE, 2)
#define AEEFIFO_IOCTL_GETBUFUSED  BREWIOCTL_RW(BREWIOCTL_FIFO_TYPE, 3, uint32)

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

typedef struct _IFIFO IFIFO;
AEEINTERFACE(IFIFO)
{
   INHERIT_IPort(IFIFO);
   int   (*OpenEx)  (IFIFO* po, const char* szName, uint16 wMode);
   int32 (*Flush) (IFIFO* po, int32 cbBuf);
};

#define IFIFO_AddRef(p)                   AEEGETPVTBL((p),IFIFO)->AddRef((p))
#define IFIFO_Release(p)                  AEEGETPVTBL((p),IFIFO)->Release((p))
#define IFIFO_QueryInterface(p,i,ppo)     AEEGETPVTBL((p),IFIFO)->QueryInterface((p),(i),(ppo))
#define IFIFO_Read(p,pc,cb)               AEEGETPVTBL((p),IFIFO)->Read((p),(pc),(cb))
#define IFIFO_Readable(p,pcb)             AEEGETPVTBL((p),IFIFO)->Readable((p),(pcb))
#define IFIFO_GetLastError(p)             AEEGETPVTBL((p),IFIFO)->GetLastError((p))
#define IFIFO_Write(p,pc,cb)              AEEGETPVTBL((p),IFIFO)->Write((p),(pc),(cb))
#define IFIFO_Writeable(p,pcb)            AEEGETPVTBL((p),IFIFO)->Writeable((p),(pcb))
#define IFIFO_IOCtl(p,opt,v)              AEEGETPVTBL((p),IFIFO)->IOCtl((p),(opt),(v))
#define IFIFO_Close(p)                    AEEGETPVTBL((p),IFIFO)->Close((p))
#define IFIFO_Open(p,n)                   AEEGETPVTBL((p),IFIFO)->Open((p),(n))
#define IFIFO_OpenEx(p,n,f)               AEEGETPVTBL((p),IFIFO)->OpenEx((p),(n),(f))
#define IFIFO_Flush(p,cb)                 AEEGETPVTBL((p),IFIFO)->Flush((p),(cb))

// Pseudo-methods
#define IFIFO_ReadableEx(p,pcb,pfn,pv)    CALLBACK_Init((pcb),(pfn),(pv)); \
                                          IFIFO_Readable((p), (pcb))
#define IFIFO_WriteableEx(p,pcb,pfn,pv)   CALLBACK_Init((pcb),(pfn),(pv)); \
                                          IFIFO_Writeable((p), (pcb))
static __inline int IFIFO_GetBufSize(IFIFO* po, uint32* pdwSize)
{
   return IFIFO_IOCtl(po, AEEFIFO_IOCTL_GETBUFSIZE, (uint32)pdwSize);
}
static __inline int IFIFO_SetBufSize(IFIFO* po, uint32 dwSize)
{
   return IFIFO_IOCtl(po, AEEFIFO_IOCTL_SETBUFSIZE, dwSize);
}
static __inline int IFIFO_GetBufUsed(IFIFO* po, uint32* pdwCount)
{
   return IFIFO_IOCtl(po, AEEFIFO_IOCTL_GETBUFUSED, (uint32)pdwCount);
}

/*=====================================================================
INTERFACES DOCUMENTATION
=======================================================================

IFIFO Interface

   The IFIFO interface provides a light weight form of InterProcess
   Communication (IPC).

   A FIFO is simply a kernel buffer with a name, to which multiple
   readers and/or writers may simultaneously attach.  The name space is a
   null terminated character string beginning with "fifo:/", which is
   typically "well known" to allow unrelated applications to attach to
   the same FIFO.

   As its name implies, a FIFO provides first-in, first-out IPC.  Write
   operations place bytes sequentially into the kernel buffer, until it
   is full.  Read operations return the same bytes in the same order, until
   the buffer is empty.  

   When the buffer is empty, a read operation will return IPORT_END if
   there are no writers, else it will return IPORT_WAIT.  To avoid
   processing an IPORT_END, a server may open a FIFO in read/write mode
   even when it is only interested in reading.

   If there are multiple readers (writers), the read (write) operations
   will be interleaved as they are scheduled by the operating environment,
   so some other form of cooperative synchronization may be needed.

Access Control Lists

   Modules may "publish" or export FIFOs in their home directories.  As
   of BREW 3.1.0, BREW looks for the RESTYPE_BINARY resource
   IDB_MIF_FIFO_ACLS in each module's MIF to resolve which FIFOs are
   published with what permissions.  See IFILEMGR_CheckPathAccess() for a
   complete description of ACL syntax.

   Here are BREW's FIFO ACLs (which are considered relative to "fifo:/"):
      const char *cpszzFIFOACL = 

         "0x00000000 =    /  r :/sys"          "\0" // all groups to read under fifo:/sys
         "0x00000000 =    /    :/sys/priv"     "\0" // no access to fifo:/sys/priv

         "0x00000000 =    / rw :/shared"       "\0" // all groups to read/write under fifo:/shared

         ;

=============================================================================

IFIFO_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

Version:
   Introduced BREW Client 3.1.0

See Also:
    BREW SDK(TM) AEE Hierarchy Chart.
    IFIFO_Release()

=============================================================================

IFIFO_Close()

Description:
    This function is inherited from IPORT_Close(). 

Version:
   Introduced BREW Client 3.1.0

See Also:
    BREW SDK(TM) AEE Hierarchy Chart.

=======================================================================

IFIFO_Flush()

Description: 
   Consumes bytes from the FIFO.  This is equivalent to IFIFO_Read(),
   except no copy is performed.

Prototype:
   int32 Flush(IFIFO* po, int32 cbBuf); 

Parameters:
   po : Pointer to the IFIFO interface 
   cbBuf : number of bytes to flush

Return value:
    indicates number of bytes flushed on success, or one of:

     ISOURCE_END   : peer shut down connection gracefully (no more data)
     ISOURCE_ERROR : error was encountered (no more data)
     ISOURCE_WAIT  : no data available at the moment; call Readable() to wait

Comments:  
   Note that the ISOURCE_WAIT return value is used instead of
an error return value and an EWOULDBLOCK error code.

Version:
   Introduced BREW Client 3.1.0

See Also:
    IFIFO_Read()

=======================================================================

IFIFO_GetBufSize()

Description:
   Returns the currently allocated buffer size

Prototype:
   int IFIFO_GetBufSize(IFIFO* po, uint32* pdwSize)

Parameters:
   po : Pointer to the IFIFO interface 
   pdwSize : pointer to an uint32 to be filled in with the allocated buffer size

Return value:
  SUCCESS      - the allocated buffer size has been written to *pdwSize
  EBADSTATE    - FIFO not open
  EMEMPTR      - invalid pdwSize pointer

Comments:
   This is not a formal member of IFIFO, instead it is a wrapper around IFIFO_IOCtl()

Version:
   Introduced BREW Client 3.1.0

See Also:
    IFIFO_SetBufSize()

=======================================================================

IFIFO_GetBufUsed()

Description:
   Returns the current number of bytes available for reading

Prototype:
   int IFIFO_GetBufUsed(IFIFO* po, uint32* pdwCount)

Parameters:
   po : Pointer to the IFIFO interface 
   pdwSize : pointer to an uint32 to be filled in with the current number
            of bytes available for reading

Return value:
  SUCCESS      - the current number of bytes available for reading has
                 been written to *pdwSize
  EBADSTATE    - FIFO not open
  EMEMPTR      - invalid pdwSize pointer

Comments:
   This is not a formal member of IFIFO, instead it is a wrapper around IFIFO_IOCtl()

Version:
   Introduced BREW Client 3.1.0

See Also:
    IFIFO_Read()

=============================================================================

IFIFO_GetLastError()

Description:
    This function is inherited from IPORT_GetLastError(). 

Version:
   Introduced BREW Client 3.1.0

See Also:
    BREW SDK(TM) AEE Hierarchy Chart.

=============================================================================

IFIFO_IOCtl()

Description:
    This function is inherited from IPORT_IOCtl(). 

Version:
   Introduced BREW Client 3.1.0

See Also:
    BREW SDK(TM) AEE Hierarchy Chart.

=============================================================================

IFIFO_Open()

Description:
    This function is inherited from IPORT_Open(). 

    The format of szPort is a name and a mode separated by a question mark ('?'):
       "<name>?<mode>"

    See IFIFO_OpenEx() for the format of <name>.

    Legal values for <mode> are "mode=r", "mode=w", or "mode=rw".

    Some examples:
       "fifo:/~/foobar?mode=r"
       "fifo:/shared/foobar?mode=w"
       "fifo:/sys/foobar?mode=r"          (only read access allowed)
       "fifo:/~0x01234567/foobar?mode=rw" (requires ACL access exported from 0x01234567's MIF)

Version:
   Introduced BREW Client 3.1.0

See Also:
    BREW SDK(TM) AEE Hierarchy Chart.
    IFIFO_OpenEx()
    IFIFO Interface

=======================================================================

IFIFO_OpenEx()

Description:
   Opens a FIFO for reading and/or writing.  

   The name must start with AEEFIFO_ROOT_DIR.  All applications can
   read and write FIFOs in their home directory and
   AEEFIFO_SHARED_DIR, and may read FIFOs in AEEFIFO_SYS_DIR.
   Applications may also access FIFOs in another module's home
   directory, providing that the other module has exported an
   appropriate ACL via their MIF (see IFIFO Interface).

   Some examples:
      "fifo:/~/foobar"
      "fifo:/shared/foobar"
      "fifo:/sys/foobar"          (only read access allowed)
      "fifo:/~0x01234567/foobar"  (requires ACL access exported from 0x01234567's MIF)

   The mode is one of the following flags:
      AEEFIFO_MODE_R  - open fifo for reading only
      AEEFIFO_MODE_W  - open fifo for writing only
      AEEFIFO_MODE_RW - open fifo for reading and writing

Prototype:
   int IFIFO_OpenEx(IFIFO* po, const char* szName, uint16 wMode);

Parameters:
   po : Pointer to the IFIFO interface 
   szName : null-terminated FIFO name
   wMode : AEEFIFO_MODE_R | AEEFIFO_MODE_W

Return value:

   SUCCESS    - the specified FIFO was opened in the mode requested
   EMEMPTR    - invalid szName pointer
   EBADPARM   - malformed length or invalid mode
   EBADSTATE  - FIFO is already open
   ENOMEMORY  - out of memory
   EPRIVLEVEL - insufficient privilege level

Version:
   Introduced BREW Client 3.1.0

See Also:
    IFIFO_Open()
    IFIFO_Close()
    IFIFO Interface

=============================================================================

IFIFO_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface(). 

Version:
   Introduced BREW Client 3.1.0

See Also:
    BREW SDK(TM) AEE Hierarchy Chart.

=============================================================================

IFIFO_Read()

Description:
    This function is inherited from ISOURCE_Read(). 

Version:
   Introduced BREW Client 3.1.0

See Also:
    BREW SDK(TM) AEE Hierarchy Chart.

=============================================================================

IFIFO_Readable()

Description:
    This function is inherited from ISOURCE_Readable(). 

Version:
   Introduced BREW Client 3.1.0

See Also:
    BREW SDK(TM) AEE Hierarchy Chart.

=============================================================================

IFIFO_Release()

Description:
    This function is inherited from IBASE_Release(). 

Version:
   Introduced BREW Client 3.1.0

See Also:
    BREW SDK(TM) AEE Hierarchy Chart.
    IFIFO_Release()

=======================================================================

IFIFO_SetBufSize()

Description: 
   Sets the allocated buffer size.  The FIFO must have been opened with
   a mode that includes AEEFIFO_MODE_W.
   
   Since there can be multiple writers per FIFO, the actual size of
   the kernel buffer will be the maximum of all buffer size requests.
   Specifying a buffer size smaller than the current size is not
   supported.  In the absence of any explicit requests, the buffer
   size will be set to the system default, which can be determined
   using IFIFO_GetBufSize();

Prototype:
   int IFIFO_SetBufSize(IFIFO* po, uint32 dwSize)

Parameters:
   po : Pointer to the IFIFO interface 
   dwSize : the desired buffer size

Return value:
  SUCCESS      - the buffer size has been written to *pdwSize
  EBADSTATE    - FIFO not open
  EBADPARM     - requested size smaller than current
  EPRIVLEVEL   - FIFO not open for write

Comments:
   This is not a formal member of IFIFO, instead it is a wrapper around IFIFO_IOCtl()

Version:
   Introduced BREW Client 3.1.0

See Also:
    IFIFO_GetBufSize()

=============================================================================

IFIFO_Write()

Description:
    This function is inherited from IPORT_Write(). 

Version:
   Introduced BREW Client 3.1.0

See Also:
    BREW SDK(TM) AEE Hierarchy Chart.

=============================================================================

IFIFO_Writeable()

Description:
    This function is inherited from IPORT_Writeable(). 

Version:
   Introduced BREW Client 3.1.0

See Also:
    BREW SDK(TM) AEE Hierarchy Chart.

=============================================================================

=======================================================================*/

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

AEEFIFO_IOCTL_GETBUFSIZE

Description:
   When AEEFIFO_IOCTL_GETBUFSIZE is used as the nOption parameter to
   IFIFO_IOCtl(), dwVal must be set to a pointer to an uint32, which will
   be filled in with the currently allocated buffer size.

As a convenience, IFIFO_GetBufSize() is provided as a wrapper around
IFIFO_IOCtl().

Definition:
   #define AEEFIFO_IOCTL_GETBUFSIZE  BREWIOCTL_RW(BREWIOCTL_FIFO_TYPE, 1, uint32)

Members:
    None

Comments:
    None

Version:
   Introduced BREW Client 3.1.0

See Also:
    IFIFO_GetBufSize()
    IFIFO_IOCtl()

======================================================================= 

AEEFIFO_IOCTL_GETBUFUSED

Description:
   When AEEFIFO_IOCTL_GETBUFUSED is used as the nOption parameter to
   IFIFO_IOCtl(), dwVal must be set to a pointer to an uint32, which will
   will be filled in with the number of bytes currently available for
   reading.

As a convenience, IFIFO_GetBufUsed() is provided as a wrapper around
IFIFO_IOCtl().

Definition:
   #define AEEFIFO_IOCTL_GETBUFUSED  BREWIOCTL_RW(BREWIOCTL_FIFO_TYPE, 3, uint32)

Members:
    None

Comments:
    None

Version:
   Introduced BREW Client 3.1.0

See Also:
    IFIFO_GetBufUsed()
    IFIFO_IOCtl()

======================================================================= 

AEEFIFO_IOCTL_SETBUFSIZE

Description:
   When AEEFIFO_IOCTL_SETBUFSIZE is used as the nOption parameter to
   IFIFO_IOCtl(), dwVal must be set to an uint32, which contains the
   desired kernel buffer size for this FIFO.
   
   Since there can be many readers and/or writers per FIFO, the actual
   size of the kernel buffer will be the maximum of all buffer size
   requests.  Specifying a buffer size smaller than the current size
   is not supported.  In the absence of any explicit requests, the
   buffer size will be set to the system default, which can be
   determined using IFIFO_GetBufSize().

   As a convenience, IFIFO_SetBufSize() is provided as a wrapper around
   IFIFO_IOCtl().

Definition:
   #define AEEFIFO_IOCTL_SETBUFSIZE  BREWIOCTL(BREWIOCTL_FIFO_TYPE, 2)

Members:
    None

Comments:
    None

Version:
   Introduced BREW Client 3.1.0

See Also:
    IFIFO_GetBufSize()
    IFIFO_SetBufSize()
    IFIFO_IOCtl()

======================================================================= 

=======================================================================*/

#endif /* _AEEFIFO_H_ */
