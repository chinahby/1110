
#ifndef __UXBUF_H__
#define __UXBUF_H__

/*===========================================================================


                              U X  B U F
             
                          H E A D E R  F I L E

FILE:  UxBuf.h


DESCRIPTION

  This header file defines the interface to UxBuf.

  UxBuf defines an interface to allocate and manage storage that can be 
  shared across process domains without copying data. Storage is allocated 
  from a pool of memory with a specified access control.

  UxBuf implments a buffer aggregate that encapsulates a sequence of bytes. 
  The data may be stored in a number of discontinuous segments (where a segment 
  is a contiguous memory region).

  Update:

  Under the LTK, UxBuf will allocate and maintain memory in contiguous blocks
  using standard malloc/free calls.  This is to bypass a limitation of 
  resources when using the dsm blocks on the LTK.  Future updates will likely
  include the conversion from malloc/free to KxMem.


EXTERNALIZED FUNCTIONS : 
  UxBuf()       - Constructor for UxBuf
  Prepend()     - Append another UxBuf at the beginning.
  Append()      - Append another UxBuf to the end.
  Alloc()       - Allocate memory at a specific offset.
  Dup()         - Duplicate a subrange of the contents of a UxBuf.
  Copy()        - Do deep copy of a subrange of a UxBuf.
  Trim()        - Trim some bytes from a UxBuf.
  Truncate()    - Truncate a UxBuf to 'Length' bytes starting 
                  from the beginning.
  Length()      - Return the length of a UxBuf.
  Seek()        - Seek to a position in a UxBuf.
  Tell()        - Tell the position of Read/Write pointer.
  NextSegment() - Return a pointer to the next contiguous segment.
  Get()         - Get the subrange of data from the UxBuf into a byte array.
  Set()         - Set the data into a UxBuf from a byte array.
  GetNext()     - Get the next subrange of data stored in a UxBuf into a
                  byte array 
  SetNext()     - Set the data stored in a byte array into a UxBuf starting
                  from next location pointed to by curPos.
  Grant()       - Grant, to a specified process, access to a UxBuf. The
                  current process loses access.
  Revoke()      - Revoke, from a specified process, access to a UxBuf.
                  The current process gains access.
  Relinquish()  - Relinquish access to a UxBuf. The process that previously
                  had access gains access.


  Copyright (c) 2005, 2006 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/UxBuf.h#1 $ 
  $DateTime: 2007/07/17 11:00:30 $
  $Author: jmiera $


when      who      what, where, why
--------  ---      ----------------------------------------------------
11/22/06   gr      Added GetState and GetState
05/15/06   rv      Fixed compiler error on ARM
03/20/06   gr      Changes to support remoting of new UxBuf structure
03/20/06   rv      Fixed target Compiler error
02/21/06   gr      Fixed featurization problem
10/21/05   gr      Clean-up, API update, bug fixes
10/11/05  Sur      Initial Version
      

===========================================================================*/

/*===========================================================================
                           
               INCLUDE FILES

============================================================================*/
#include "IxErrno.h"
#include "msg.h"
#include "iovec.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                           CLASS DEFINITION

===========================================================================*/

/* define a minimum size packet in the case of dynamic memory allocation
 * to help keep from fragmenting memory.
 */


/*------------------------------------------------------------------------
                             PUBLIC TYPES
------------------------------------------------------------------------*/

typedef struct UxBufMetaDataT_
{
  UxIOVEC_T *iovt;
  UxBufPool m_pool;
} UxBufMetaDataT;

typedef struct
{
  UxIOVEC_T *iovt;
  UxBufPool pool;
} UxBufStateDataT;


class UxBuf
{
  friend IxErrnoType UxBufGetMetaData (UxBuf *pBuf, UxBufMetaDataT *pMetadata);

  friend IxErrnoType UxBufCreateBufFromMetaData (UxBuf **ppBuf,
    UxBufMetaDataT *pMetadata);

public:

  /*------------------------------------------------------------------------
                               PUBLIC METHODS
  ------------------------------------------------------------------------*/

    /* Constructor for UxBuf
     * Creates a UxBuf object.
     * UxBuf is an aggregator object which encapsulates an ordered sequence
     * of bytes. In general data may be stored in a number of discontiguous
     * segments.
     * 
     * @param Pool      - Buffer pool used to create the buffers.
     * 
     * @return
     */
    UxBuf (UxBufPool pool=BUF_POOL_LOCAL);    // enum in iovec.h
	UxBuf(const UxBuf &u);
	UxBuf(const char *p, UxBufPool pool=BUF_POOL_LOCAL);

	UxBuf& operator=(const UxBuf &u);
	UxBuf& operator=(const char *p);

	UxBuf& operator+(const UxBuf &b);
	UxBuf& operator+= (const UxBuf &u);

    /* Destructor for UxBuf
     * Reclaims the underlying buffers of the UxBuf and deletes the UxBuf
     * object.
     *
     * @param 
     * 
     * @return
     */
    ~UxBuf ();


    /* Prepend a UxBuf
     * The buffers pointed to by the input UxBuf are inserted at the beginning.
     * Buffer data is not copied. Only references to buffers are added.
     * 
     * @param Buf       - UxBuf to append
     * 
     * @return IxErrnoType  - Error number or Success.
     *
     * Side Effects     - This function does not take care of duplication 
     * of buffer packets in UxBuf passed in.
     */
    IxErrnoType  Prepend (UxBuf **Buf);

    /* Append a UxBuf
     * The buffers pointed to by the input UxBuf are inserted at end.
     * Buffer data is not copied. Only references to buffers are added.
     * 
     * @param Buf       - UxBuf to append
     * 
     * @return IxErrnoType  - Error number or Success.
     *
     * Side Effects     - This function does not take care of duplication 
     * of buffer packets in UxBuf passed in.
     */
    IxErrnoType  Append (UxBuf **Buf);
	IxErrnoType  Append (byte* src, uint32 num_bytes);

    /* Allocate storage at an offset.
     * UxBuf allocates 'Size' amount of storage at a specific offset. Allocated
     * storage is not guaranteed to be contiguous.
     * 
     * 
     * @param Offset    - Offset within UxBuf where storage is to be allocated.
     *                    A value of -1 (0xffffffff) allocates storage at the
     *                    end.
     * @param Size      - Size of aggregator object to create.
     * 
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Alloc (uint32 Offset, uint32 Size);

    /* Duplicate a subrange of the contents of a UxBuf
     * Returns a new UxBuf that contains the subrange of 
     * the contents of the UxBuf. The subrange starts at the byte offset
     * specified by 'Offset' and contains 'Length' bytes.
     * The value of Offset should be within range or an error is 
     * returned.
     * This is a shallow copy where the top level objects are duplicated
     * and the lower levels contain references.
     * 
     * @param Offset  - Offset which specifies the start of new UxBuf.
     * @param Length  - Length of bytes of the new UxBuf.
     * @param Buf     - Pointer to the new UxBuf
     * 
     * 
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Dup (uint32 Offset, uint32 Length, UxBuf** Buf);

    /* Copy a subrange of the contents of a UxBuf
     * Returns a new UxBuf which is a deep copy of a subrange of the UxBuf.
     * The subrange starts at the byte offset specified by 'Offset' and contains
     * 'Length' bytes.
     * The value of Offset should be within range or an error is 
     * returned.
     * This is a deep copy operation where UxBuf structure and 
     * segment data are duplicated.
     * 
     * @param Offset  - Offset which specifies the start of new UxBuf.
     * @param Length  - Length of bytes of the new UxBuf.
     * @param Buf     - Pointer to the new UxBuf
     * 
     * 
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Copy (uint32 Offset, uint32 Length, UxBuf** Buf);

    /* Trim some bytes from a UxBuf
     * The data that is trimmed starts at 'Offset' byte position and
     * includes 'Length' bytes.
     * 
     * @param Offset    - Offset where UxBuf is trimmed.
     * @param Length    - Length of bytes to trim.
     * 
     * 
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Trim (uint32 Offset, uint32 Length);

    /* Truncate a UxBuf
     * Truncates the UxBuf to 'Length' bytes starting from the beginning of
     * the buffer.
     * 
     * @param Length    - Length of the new UxBuf.
     * 
     * 
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Truncate (uint32 Length);

    /* Returns the Length of the UxBuf.
     * 
     * @param Length    - Pointer to Length field filled in by callee.
     * 
     * 
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Length (uint32* Length);

	/* Returns the Size of the UxBuf.
	 *
	 *   @param Size - Pointer to the Size field filled in by callee.
	 *
	 *
	 *   @return IxErrnoType  - Error number or Success.
	 */
	IxErrnoType  Size (uint32* Size);

    /* Seek to a position in UxBuf.
     * This call is typically used prior to accessing the contents of buffer
     * at a position.
     * The position is relative to the start of the UxBuf. If position
     * value is not within the range of the buffer, an error is
     * returned.
     * 
     * @param Position  - Position to seek to.
     * 
     * 
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Seek (uint32  Position);

    /* Tell the position of Read/Write pointer
     * This call is typically used when accessing the contents of the UxBuf
     * using 'SetNext' and 'GetNext' calls.
     * The position is relative to the start of the UxBuf.
     * 
     * @param Position  - Pointer to Position filled by callee.
     * 
     * 
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Tell (uint32*  Position);

    /* Return a pointer to the next contiguous segment.
     * Returns the [pointer, length] pair of the next
     * contiguous segment relative to the position. The position is set
     * using the 'Seek' method.
     * 
     * @param SegAddr  - Pointer to buffer segment. This is filled by callee.
     * @param Length  - Length bytes that are valid at SegAddr.
     *                   This is filled by callee.
     * 
     * 
     * @return IxErrnoType- Error number or Success.
     */
    IxErrnoType  NextSegment (uint8**  SegAddr, uint32*  Length);

	/* Return a pointer to the current contiguous segment.
     * Returns the [pointer, length] pair of the next
     * contiguous segment relative to the position. The position is set
     * using the 'Seek' method.
     * 
     * @param SegAddr  - Pointer to buffer segment. This is filled by callee.
     * @param Length  - Length bytes that are valid at SegAddr.
     *                   This is filled by callee.
     * 
     * 
     * @return IxErrnoType- Error number or Success.
     */
	IxErrnoType  GetFirstSegment (uint8** SegAddr, uint32* Length);
    IxErrnoType  GetSegment (uint8**  SegAddr, uint32*  Length);

    /* Get the subrange of data from the UxBuf into a Bytearray
     * Copies the data of length size from the UxBuf starting 
     * at Offset location inside the UxBuf into the Bytearray provided 
     *
     * @param Offset  -  The byte at which the data to be copied starts.
     * @param Length  -  The number of bytes to be copied .
     * @param Bytes   -  The pointer to destination bytearray into which  
     *          the data is copied.
     *
     *
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Get(uint32 Offset, uint32 Length, byte** Bytes, uint32 *BytesRead);

    /* Set the data into UxBuf from a Bytearry 
     * Sets the data of size Length into UxBuf from Bytearray  
     * provided. The copying will start from the location Offset inside UxBuf.
     * 
     * @param Offset  -  This parameter specifies the first location in the  
     *           UxBufdata from where to start Coppying the data.
     * @param Length  -  The number of bytes to be copied .
     * @param Bytes   -  The pointer to source bytearray from which the 
     *          data is copied.
     *
     *
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Set( uint32 Offset, uint32 Length , byte* Bytes, uint32 *BytesWritten);

    /* Get the next subrange of data stored in UxBuf into a Bytearry 
     * Gets the data from the location set by SEEK () operation
     * or previous GetNext operation. After reading the data it sets the   
     * curPos to the next data byte to the byte last read.
     *
     * @param Length  -  The number of bytes to be copied .
     * @param Bytes   -  The pointer to destination bytearray in which the 
     *           data is copied.
     *
     *
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  GetNext( uint32 Length , byte** Bytes );

    /* Set the data stored in a Bytearry into UxBuf starting from next 
     * location pointed by curPos.
     * After reading the data the curPos is set to the data byte after
     * the byte last written into UxBuf.
     *
     * @param Length  -  The number of bytes to be copied .
     * @param Bytes   -  The pointer to source bytearray from which the 
     *           data is copied.
     *
     *
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  SetNext( uint32 Length , byte* pBytes );

    /* Grant privilege to a specified process to access the UxBuf.
     * This function returns an error unless called from the process that
     * currently has access to the UxBuf. The process that currently has
     * access to the UxBuf loses access if the call succeeds.
     *
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Grant( uint32 pId );

    /* Revoke access privilege to the UxBuf from a specified process.
     * The process that currently has access to the UxBuf loses access.
     *
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Revoke( uint32 pId );

    /* Relinquish access privilege to the UxBuf. Access privilege
     * reverts to the process that previously had access.
     *
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Relinquish( void );

    /* Return state data for the UxBuf. This data can be used to
    ** recreate the UxBuf.
    */
    void GetState( uint8** ppStateData, uint32* pStateDataLen );

    /* Recreate UxBuf using specified state data.
    */
    void SetState( uint8* pStateData, uint32 stateDataLen );

  /*------------------------------------------------------------------------
                               PRIVATE METHODS
  ------------------------------------------------------------------------*/
private:

  /*------------------------------------------------------------------------
                               PUBLIC MEMBERS
  ------------------------------------------------------------------------*/
  //No Public Members 

	/*------------------------------------------------------------------------
                               PRIVATE MEMBERS

	------------------------------------------------------------------------*/
    UxIOVEC_T *m_iovt;  // beginning of our iovec table
	uint32 m_offset;    // current offset of the UxBuf
	UxBufPool m_pool;   // the allocation pool type
};

#ifdef __cplusplus
}
#endif

#endif


