#ifndef AEEISOURCE_H
#define AEEISOURCE_H
/*======================================================
FILE:  AEEISource.h

SERVICES:  ISource

GENERAL DESCRIPTION:
   ISource definitions, typedefs, etc.

Copyright © 2004-2008 QUALCOMM Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary
=====================================================*/

#include "AEEIQI.h"
#include "AEECallback.h"

/*
  ||
  ||  ISource : Abstract data source
  ||
*/

/*
  || Exit codes for ISource member functions:
 */
#define ISOURCE_END          0     // Read:  end of stream
#define ISOURCE_ERROR       -1     // Read:  error in stream; no more data is available
#define ISOURCE_WAIT        -2     // Read:  try again later after calling Readable


// ISource_Exhausted(nCode) : TRUE when nCode indicates the source
//    will never return any more data.
//
static __inline boolean ISource_Exhausted(int32 nCode)
{
   return ( (unsigned)-nCode < 2U);
}

#define INHERIT_ISource(iname)\
   INHERIT_IQI(iname);\
   int32 (*Read)    (iname *po, char *pcBuf, int32 cbBuf);\
   void  (*Readable)(iname *po, AEECallback *pcb)

#define AEEIID_ISource 0x01001012

/*  declare the actual ISource interface */
AEEINTERFACE_DEFINE(ISource);


static __inline uint32 ISource_AddRef(ISource* pme)
{
   return AEEGETPVTBL(pme, ISource)->AddRef(pme);
}
static __inline uint32 ISource_Release(ISource* pme)
{
   return AEEGETPVTBL(pme, ISource)->Release(pme);
}
static __inline int ISource_QueryInterface(ISource* pme, AEEIID aeeiid, void** ppVoid)
{
   return AEEGETPVTBL(pme, ISource)->QueryInterface(pme, aeeiid, ppVoid);
}
static __inline int32 ISource_Read(ISource* po, char* pcBuf, int32 cbBuf)
{
   return AEEGETPVTBL(po, ISource)->Read(po, pcBuf, cbBuf);
}
static __inline void ISource_Readable(ISource* po, AEECallback* pcb)
{
    AEEGETPVTBL(po, ISource)->Readable(po, pcb);
}



/*=====================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

ISource Interface

   An abstract data source that provides a Read/Readable interface.


=============================================================================

ISource_AddRef()


Description:
   This function is inherited from IQI_AddRef(). 

=============================================================================


ISource_Exhausted()


Description:
   This function determines if an ISource is out of data


Prototype:
   boolean ISource_Exhausted(int32 nCode);


Parameters:

   nCode
      Return value from Read()
 

Return Value:

   TRUE if the source is exhausted (no more data)
   
   FALSE, if otherwise.


Comments:
   None


See Also:
   None


=============================================================================


ISource_Read()


Description:
   This function copies bytes from the source stream to the specified buffer.  
   All copied bytes are consumed (that is, the read pointer is advanced).  

Prototype:
   int32 ISource_Read(
      ISource *   pISource, 
      char *      pc, 
      int32       cb
   );


Parameters:

   pISource
      Pointer to the ISource Interface object.
   
   pc
      Pointer to the buffer to receive bytes read
   
   cb
      Size of pc, in bytes
 


Return Value:

   Number of bytes read, if successful.  This will be a value greater than 
   zero.  
   
   Error code, if otherwise.
   
      ISOURCE_END, end of stream (no more data).
      
      ISOURCE_ERROR, error was encountered (no more data).
      
      ISOURCE_WAIT, no data available at the moment; call Readable() to wait.


Comments:  
   Note that the IWOURCE_WAIT return value is used instead of
   an error return value and an EWOULDBLOCK error code.

See Also:
   None


=============================================================================


ISource_Readable()


Description: 

   This function schedules a callback to be called when ISource_Read() may be 
   able to return something other than ISOURCE_WAIT.  


Prototype:

   void ISource_Readable(
      ISource *      pISource, 
      AEECallback *  pcb
   );


Parameters:

   pISource
      Pointer to the ISource Interface object.
   
   pcb
      Where to call when the ISource may be readable.
 

Return Value:
   None


Comments:
   The AEECallback struct pointed to by the 'pcb' parameter must be 
   initialized with the CALLBACK_Init() macro before calling this funciton.

See Also:
   None


=============================================================================

ISource_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface(). 

=============================================================================

ISource_Release()

Description:
   This function is inherited from IQI_Release(). 

===========================================================================
*/


#endif // AEEISOURCE_H
