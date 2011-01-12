#ifndef AEEIPORT1_H
#define AEEIPORT1_H
/*============================================================================

FILE:         AEEIPort1.h

DESCRIPTION:  definition of the IPort1 interface

PUBLIC CLASSES:  IPort1 interface.

Copyright © 2007-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
============================================================================*/

/*===========================================================================
INCLUDES
===========================================================================*/

#include "AEEISignal.h"

/*===========================================================================
Macro definitions
===========================================================================*/

#define AEEIID_IPort1        0x01056898

//==========================================================================
//   Type definitions
//==========================================================================
//
// IPort1 interface.
//
#define INHERIT_IPort1(iname) \
   INHERIT_IQI(iname);\
   int   (*Read)(iname *po, byte *pBuf, int nBufLen, int *pnRead);\
   int   (*Readable)(iname *po, ISignal *piSignal); \
   int   (*Write)(iname *pme, const byte *pBuf, int nBufLen, int *pnWritten); \
   int   (*Writeable)(iname *pme, ISignal *piSignal); \
   int   (*Control)(iname *po, AEEUID  uidControl, \
                    const byte *pIn, int nInLen,\
                    byte *pOut, int nOutLen, int* pnOutLenReq);\
   int   (*Close)(iname* po)

//
//  Declare the actual IPort1 interface.
//

AEEINTERFACE_DEFINE(IPort1);

//==========================================================================
//   API definitions
//==========================================================================

static __inline uint32 IPort1_AddRef(IPort1 *me)
{
   return AEEGETPVTBL(me,IPort1)->AddRef(me);
}

static __inline uint32 IPort1_Release(IPort1 *me)
{
   return AEEGETPVTBL(me,IPort1)->Release(me);
}

static __inline int IPort1_QueryInterface(IPort1 *me,
                                          AEEIID iid, void** ppo)
{
   return AEEGETPVTBL(me,IPort1)->QueryInterface(me, iid, ppo);
}

static __inline int IPort1_Read(IPort1 *me, byte *pBuf,
                                int nBufSize, int *pnRead)
{
   return AEEGETPVTBL(me,IPort1)->Read(me, pBuf, nBufSize, pnRead);
}

static __inline int IPort1_Readable(IPort1 *me, ISignal *piSignal)
{
   return AEEGETPVTBL(me,IPort1)->Readable(me, piSignal);
}

static __inline int IPort1_Write(IPort1 *me, const byte *pBuf,
											 int nBufSize,int *pnWrote)
{
   return AEEGETPVTBL(me,IPort1)->Write(me, pBuf, nBufSize, pnWrote);
}

static __inline int IPort1_Writeable(IPort1 *me, ISignal *piSignal)
{
   return AEEGETPVTBL(me,IPort1)->Writeable(me, piSignal);
}

static __inline int IPort1_Control(IPort1 *me, AEEUID uidControl, 
                                   const byte *pIn, int nInLen,
                                   byte *pOut, int nOutLen, int* pnOutLenReq)
{
   return AEEGETPVTBL(me,IPort1)->Control(me, uidControl, pIn, nInLen,
                                          pOut, nOutLen, pnOutLenReq);
}

static __inline int IPort1_Close(IPort1 *me)
{
   return AEEGETPVTBL(me,IPort1)->Close(me);
}

/*=====================================================================
  INTERFACES DOCUMENTATION
=======================================================================

IPort1 Interface

Description:

   Generic interface for bidirectional data stream providing
   Read/Readable, Write/Writeable, GetLastError and Control interfaces.

   Instances of IPort1 usually come from a factory or some other 
   object that owns or provides access to the data to be read or written.

   For example if there is a IPort1 based implementation of file,
   you may get an instances of the file from the filesystem when the 
   file is opened or an instance of IPort1 can be created to access
   the seria IO.

=======================================================================

IPort1_AddRef()

Description:
    This function is inherited from IQI_AddRef().

See Also:
    IPort1_Release()

=======================================================================

IPort1_Release()

Description:
    This function is inherited from IQI_Release()..

See Also:
    IPort1_AddRef()

=======================================================================

IPort1_QueryInterface()

Description:
    This function is inherited from IQI_QueryInterface().

=======================================================================

IPort1_Read()

Description:
   Copy bytes from the source stream to the specified buffer.  All copied
   bytes are "consumed" (i.e. the "read pointer" is advanced).

Prototype:
   int IPort1_Read
   (
      IPort1 *me,
      byte *pBuf,
      int nBufSize,
      int *pnRead
   )

Parameters:
   me      : [in]  : Pointer IPort1 object
   pBuf    : [out] : pointer to buffer to receive bytes read
   nBufSize: [in]  : size of pBuf buffer
   pnRead  : [out] : the number of bytes read successfully

Return Value:

   AEE_SUCCESS      : if pnRead = 0 and nBufSize != 0, end of read stream,
                      if not pnRead indicates the amount of data read.
   AEE_EWOULDBLOCK  : no data available at the moment call Readable() to wait
   AEE_ENOTALLOWED  : Does not have the capabilities to perform the operation.

   Another appropriate error code if operation was not successful

Comments:
   Note that the AEE_EWOULDBLOCK return value is used so that the Readable
   can be called. pnRead > 0 indicates that data has been sucessfully read
   from the port.

Side Effects:
   None

See Also:
   IPort1_Readable()

=======================================================================

IPort1_Readable()

Description:
   Schedule a function to be called when IPort1_Read() would
   return something other than AEE_EWOULDBLOCK.

Prototype:
   int IPort1_Readable
   (
      IPort1 *me,
      ISignal *piSignal
   )

Parameters:
   me       :  [in] : Pointer IPort1 object
   piSignal :  [in] : Pointer to a signal that is associated with a
   					  callback function that can be invoked when the port
   					  may be read with IPort1_Read().

Return Value:
   AEE_SUCCESS  : Sucessfully registered the readable.

   Another appropriate error code if operation was not successful.

Comments:
   The signal pointed by the piSignal must be created before calling 
   this funciton.

Side Effects:
   None

See Also:
   IPort1_Read()


===========================================================================

IPort1_Write()

Description:
   Copy bytes from the specified buffer to the source stream.  All copied
   bytes are "consumed" (i.e. the "write pointer" is advanced).

Prototype:
   int IPort1_Write
   (
      IPort1 *me,
      const byte *pBuf,
      int nBufSize,
      int *pnWritten
   )

Parameters:
   me       : [in]  : pointer to the IPort1 object
   pBuf     : [in]  : pointer to buffer from which bytes are taken to write
   			          to the port
   nBufSize : [in]  : size of data in pcBuf
   pnWritten: [out] : size of data written to the Port

Return Value:

   AEE_SUCCESS      : Sucessfully written pnWritten bytes to the port
   AEE_ENOTALLOWED  : Does not have the capabilities to perform the operation.
   AEE_EWOULDBLOCK  : no data available at the moment call Writeable() to wait

   Another appropriate error code if operation was not successful

Comments:
   Note that the AEE_EWOULDBLOCK return value is used so that the writeable
   can be called. pnWritten > 0 indicates that data has been sucessfully
   written to the port.

Side Effects:
   None

See Also:
   IPort1_Writeable()

=======================================================================

IPort1_Writeable()

Description:
   Schedule a function to be called when IPort1_Write() would
   return something other than AEE_EWOULDBLOCK.

Prototype:
   void IPort1_Writeable
   (
      IPort1 *me,
      ISignal *piSignal
   )

Parameters:
   me       : [in] : Pointer IPort1 object
   piSignal : [in] : Pointer to a signal that is associated with a
   					 callback function that can be invoked when the port
   					 may be written with IPort1_Write().

Return Value:
   AEE_SUCCESS  : Sucessfully registered the readable.
   Another appropriate error code if operation was not successful.

Comments:
   The signal pointed by the piSignal must be created before calling 
   this funciton.

Side Effects:
   None

See Also:
   IPort1_Write()

=======================================================================

IPort1_Control()

Description:
   This function allows some control over the behaviour of an IPort1,
   e.g. setting/getting configuration.

   Its form is analogous to SYSV's Control, with encoded arguments that
   should be specified with the *_Control_* macros listed here.  It's
   intended use is for special, out-of-object-signature behaviour.  For
   that reason, it is super-generic.

   This method could also be used to selectively close the capabilities
   of the port.

Prototype:
   int IPort1_Control
   (
     IPort1 *me,
     AEEUID  uidControl,
     const byte *pIn, 
     int nInLen,
     byte *pOut, 
     int nOutLen, 
     int* pnOutLenReq
   )


Parameters:
   me         : [in]  : pointer to the IPort object
   uidControl : [in]  : uid of the control operation, defined by the class
                        implementing IPort1
   pIn        : [in]  : input buffer argument for the operation
   nInLen     : [in]  : length of pIn
   pOut       : [out] : output buffer argument for the operation, filled
                         up to nOutLen bytes of the result of the operation,
                         if any.
   nOutLen    : [in]  : length of pOut
   pnOutLen   : [out] : set to length of pOut required to complete 
                         the operation

Return Value :
  AEE_SUCCESS  : Sucessfully registered the readable.

  Another appropriate error code if operation was not successful.

Comments:

  Data passed in the buffer arguments must always be arranged as 
  if they've already been marshalled, i.e. the precise byte ordering must
  be specified.  The data must not include pointers or objects. 


Side Effects:
   None

See Also:
   None

=======================================================================

IPort1_Close()

Description:
   This function closes the port.  Any further Read() and Write() on 
   the port will fail, and the IPort1 will become readable and writeable.

Prototype:
   int IPort1_Close
   (
      IPort1 *po
   )

Parameters:
   po: [in] : Pointer to the IPort object

Return Value:

  AEE_SUCCESS      : Successfully closed the port

  Another appropriate error code if operation was not successful.

Comments:

Side Effects:
   None

See Also:
   None


=============================================================================*/

#endif // AEEIPORT1_H
