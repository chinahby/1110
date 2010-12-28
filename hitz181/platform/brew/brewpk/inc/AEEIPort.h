#ifndef AEEIPORT_H
#define AEEIPORT_H
/*======================================================
FILE:  AEEIPort.h

SERVICES:  IPort

GENERAL DESCRIPTION:
   IPort definitions, typedefs, etc.

        Copyright © 2004-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEEISource.h"

#define AEEIID_IPort     0x01001018

/* Return Codes for Read */
#define IPORT_END       ISOURCE_END
#define IPORT_ERROR     ISOURCE_ERROR
#define IPORT_WAIT      ISOURCE_WAIT

#if defined(AEEINTERFACE_CPLUSPLUS)
struct IPort : public ISource
{
   virtual int CDECL GetLastError() = 0;
   virtual int32 CDECL Write(const char *pcBuf, int32 cbBuf) = 0;
   virtual void CDECL Writable(AEECallback *pcb) = 0;
   virtual int CDECL IOCtl(int nOption, uint32 dwVal) = 0;
   virtual int CDECL Close() = 0;
   virtual int CDECL Open(const char * szPort) = 0;
};

#else /* #if defined(AEEINTERFACE_CPLUSPLUS) */

// Use to inherit from an IPort
#define INHERIT_IPort(iname)\
   INHERIT_ISource(iname); \
   int   (*GetLastError)(iname * po); \
   int32 (*Write)(iname *pme, const char *pcBuf, int32 cbBuf); \
   void  (*Writeable)(iname *pme, AEECallback *pcb); \
   int   (*IOCtl)(iname *po, int nOption, uint32 dwVal); \
   int   (*Close)(iname * po); \
   int   (*Open)(iname * po, const char * szPort)

/*  declare the actual IPort interface */
AEEINTERFACE_DEFINE(IPort);

static __inline uint32 IPort_AddRef(IPort* pme)
{
   return AEEGETPVTBL(pme, IPort)->AddRef(pme);
}
static __inline uint32 IPort_Release(IPort* pme)
{
   return AEEGETPVTBL(pme, IPort)->Release(pme);
}
static __inline int IPort_QueryInterface(IPort* pme, AEEIID aeeiid, void** ppVoid)
{
   return AEEGETPVTBL(pme, IPort)->QueryInterface(pme, aeeiid, ppVoid);
}
static __inline int32 IPort_Read(IPort* po, char* pcBuf, int32 cbBuf)
{
   return AEEGETPVTBL(po, IPort)->Read(po, pcBuf, cbBuf);
}
static __inline void IPort_Readable(IPort* po, AEECallback* pcb)
{
    AEEGETPVTBL(po, IPort)->Readable(po, pcb);
}
static __inline int IPort_GetLastError(IPort* po)
{
   return AEEGETPVTBL(po, IPort)->GetLastError(po);
}
static __inline int32 IPort_Write(IPort* pme, const char* pcBuf, int32 cbBuf)
{
   return AEEGETPVTBL(pme, IPort)->Write(pme, pcBuf, cbBuf);
}
static __inline void IPort_Writeable(IPort* pme, AEECallback* pcb)
{
    AEEGETPVTBL(pme, IPort)->Writeable(pme, pcb);
}
static __inline int IPort_IOCtl(IPort* po, int nOption, uint32 dwVal)
{
   return AEEGETPVTBL(po, IPort)->IOCtl(po, nOption, dwVal);
}
static __inline int IPort_Close(IPort* po)
{
   return AEEGETPVTBL(po, IPort)->Close(po);
}
static __inline int IPort_Open(IPort* po, const char* szPort)
{
   return AEEGETPVTBL(po, IPort)->Open(po, szPort);
}

#endif /* #if defined(AEEINTERFACE_CPLUSPLUS) */

/*
=======================================================================
  INTERFACES DOCUMENTATION
=======================================================================

IPort Interface

Description:
   Generic interface for bidirectional data stream providing
   Read/Readable, Write/Writeable, GetLastError and IOCtl interfaces.

   As IPort is a generic interface, it must be created using class id
   specific to its usage.

   For example:
   For Serial IO usage, use AEECLSID_SERIAL.

=============================================================================

IPort_AddRef()

Description:
   This function is inherited from IQI_AddRef().


   

See Also:
     IPort_Release()

=======================================================================

IPort_Close()

Description:
   This function closes an opened port or stops a open in progress.

Prototype:
   int IPort_Close
   (
      IPort *po
   )

Parameters:
   po: Pointer to the IPort object

Return Value:
  AEE_SUCCESS  - Successfully closed the port
  AEEPORT_ENOTOPEN - Port is not in open state
  AEE_EFAILED  - Generic failure code

Comments:

Side Effects:
   None

=======================================================================

IPort_GetLastError()

Description:
  This function returns the last error that occurred with the IPort. The
  value returned depends on the most recently called function. The documentation
  for each function, of the interface that implemented IPort, describes what
  GetLastError() can return when called right after that function.

Prototype:
  int IPort_GetLastError
  (
   IPort * po
  )

Parameters:
  po: Pointer to IPort object

Return Value:
   The most recently occurred error.  This could be any if possible BREW error codes.
   The most common ones are listed below.

   AEEPORT_ENOTOPEN  - Port is not open
   AEE_EBADSTATE         - The port is in an incompatible state

Comments:
   None

Side Effects:
   None

See Also:
   IPort_Read()
   IPort_Write()

=======================================================================

IPort_IOCtl()

Description:
   This function allows some control over the behaviour of an IPort,
   e.g., setting/getting configuration.

   The form is analogous to SYSV's ioctl, with encoded arguments that
   should be specified with the *_IOCTL_* macros listed here.  The
   intended use is for special, out-of-object-signature behaviour.  For
   that reason, it's super-generic.

   The *_IOCTL_* macros that are supported are implementation defined;
   please see the appropriate class documentation.

Prototype:
   int IPort_IOCtl
   (
      IPort * po,
      int nOption,
      uint32 dwVal
   )

Parameters:
   po:         Pointer to the IPort object
   nOption:    the option id
   dwVal:      option value : this value depends on the option id. If could be
               a 32 bit value or a pointer to a structure that is specific to
               the option id

Return Value:
  AEE_SUCCESS -  the option id is known and the specified operation succeeds,
             else an error code, depending on the option id specified

Comments:

  Currently supported option ids are specified by each class that
  implements them


Side Effects:
   None

See Also:
   AEEIOCtl

=============================================================================

IPort_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().


   

See Also:
     IPort_Release()

=======================================================================

IPort_Open()

Description:
   This function opens the given port. This call becomes
   asynchronous if the IPORT_WAIT error code is returned. The app
   on receiving this error code should call Writeable to be notified when
   calling IPort_Open() would return a different result.

   Call IPort_Close() to stop a pending IPort_Open().

Prototype:
   int IPort_Open
   (
      IPort *po,
      const char * szPort
   )

Parameters:
   po:      Pointer to the IPort object
   szPort:  Name of the port to be opened

Return Value:
  AEE_SUCCESS         - Successfully opened the port
  IPORT_WAIT    - Open in progress; Call Open() again for status
  AEEPORT_EINUSE  - Port is currently open by some other entity
  AEE_ENOSUCH         - The asked port name is not supported
  AEE_ENOMEMORY       - Out of memory
  AEE_EFAILED         - General failure code

Comments:
  IPort is an abstract interface. Hence, the supported names must be looked up
  in each implementation's specific header file (such as AEESio.h for Serial Ports)


Side Effects:
   None

=============================================================================

IPort_Read()

Description:
   This function is inherited from ISource_Read().


   

See Also:

=============================================================================

IPort_Readable()

Description:
   This function is inherited from ISource_Readable().


   

See Also:

=============================================================================

IPort_Release()

Description:
   This function is inherited from IQI_Release().


   

See Also:
     IPort_AddRef()

===========================================================================

IPort_Write()

Description:
   Copy bytes from the specified buffer to the source stream.

Prototype:
   int32 IPort_Write
   (
      IPort *po,
      const char *pcBuf,
      int32 cbBuf
   )

Parameters:
   po   :  pointer to the IPort object
   pcBuf:  pointer to buffer from which bytes are taken to write to the port
   cbBuf:  size of data in pcBuf

Return Value:
   If the return value is non-negative, the return
   value indicates the number of bytes successfully written.

   Otherwise, the return value will be one of:

   IPORT_ERROR : error was encountered
   IPORT_WAIT  : no data can be sent at the moment; call Writeable() to wait


Comments:
   Note that the IPORT_WAIT return value is used instead of
   an error return value and an EWOULDBLOCK error code.

Side Effects:
   None

See Also:
   IPort_Writeable()

=======================================================================

IPort_Writeable()

Description:
   Schedule a function to be called when IPort_Write() would
   return something other than IPORT_WAIT.


Prototype:
   void IPort_Writeable
   (
      IPort *po,
      AEECallback *pcb
   )

Parameters:
   po: Pointer IPort object
   pcb:  Pointer to a pre-initialized AEECallback struct which
         contains a function to call when the IPort may be
         writeable with IPort_Write().

Return Value:
   None

Comments:
   The AEECallback struct pointed to by the 'pcb' parameter must be
   initialized with the CALLBACK_Init() macro before calling this funciton.

Side Effects:
   None

See Also:
   IPort_Write()

=============================================================================
*/

#endif // AEEIPORT_H
