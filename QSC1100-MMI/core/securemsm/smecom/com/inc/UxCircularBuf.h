#ifndef __UXCIRCULARBUF_H__
#define __UXCIRCULARBUF_H__

/*===========================================================================


                       U X  C I R C U L A R  B U F
             
                          H E A D E R  F I L E

FILE:  UxCircularBuf.h


DESCRIPTION

  This header file defines the interface to UxCircularBuf.

  UxCircularBuf defines an interface to allocate and manage storage that can be 
  overwritten as more data is read in without allocating excessive amounts of memory.


EXTERNALIZED FUNCTIONS : 
  UxCircularBuf()       - Constructor for UxCircularBuf
  Init()                - Initialize UxCircularBuf
  Append()              - Append data to the end of the UxCircularBuf
  Read()                - Read a range of data from a given offset

  Copyright (c) 2007 - 2008 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/UxCircularBuf.h#2 $ 
  $DateTime: 2008/03/25 11:38:13 $
  $Author: wsoon $


when      who      what, where, why
--------  ---      ----------------------------------------------------
03/18/08   ws      Add CreateCopy & Length support
07/11/07   ws      Initial Version
      

===========================================================================*/

/*===========================================================================
                           
               INCLUDE FILES

============================================================================*/
#include "IxErrno.h"
#include "UxCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

/*===========================================================================

                           CLASS DEFINITION

===========================================================================*/

/*------------------------------------------------------------------------
                             PUBLIC TYPES
------------------------------------------------------------------------*/

typedef struct
{
  uint8*  circularBuf;
  uint32  first;
  uint32  next;
  uint32  bufSize;
  uint32  dataLength; 
} UxCircularBufStateDataT;

class UxCircularBuf
{

public:

  /*------------------------------------------------------------------------
                               PUBLIC METHODS
  ------------------------------------------------------------------------*/

    /* Constructor for UxCircularBuf
     * Creates a UxCircularBuf object.
     * 
     * @return
     */
    UxCircularBuf ( );

    /* Destructor for UxCircularBuf
     * Reclaims the underlying buffers of the UxBuf and deletes the UxBuf
     * object.
     *
     * @param 
     * 
     * @return
     */
    ~UxCircularBuf ();

    /* Initializes the UxCircularBuf
     *
     * @param uBufSize       - How big the circular buffer should be.
     *
     * @param pool           - Where the buffer should reside
     *
     * @return
     */
    IxErrnoType Init( uint32      uBufSize,
                      UxAllocE    pool=UX_ALLOC_SHARED );

    /* Creates a copy of this UxCircularBuf
     *
     * @param ppUxCircularBuf - Pointer to address of where to put the copy
	 *
	 * @param firstOffset     - First offset to start copying from.
     *
     * @return IxErrnoType    - Errno number of Success.
     */
    IxErrnoType CreateCopy( UxCircularBuf   **ppUxCircularBuf,
                            uint32            firstOffset );

    /* Reads from the UxCircularBuf
     * Copies the data of length size from the UxCircularBuf starting 
     * at Offset location into the Bytearray provided 
     *
     * @param Offset  -  Location to start reading at.
     * @param Length  -  The number of bytes to be copied .
     * @param Bytes   -  The pointer to destination bytearray into which  
     *                   the data is copied.
     *
     *
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Read( uint32   Offset, 
                       uint32   Length, 
                       byte    *Bytes, 
                       uint32  *BytesRead );

    /* Appends the data into UxCircularBuf
     * Sets the data of size Length into UxCircularBuf from Bytearray  
     * provided. The copying will start from the end of the UxCirculbarBuf.
     * 
     * @param Length  -  The number of bytes to be copied .
     * @param Bytes   -  The pointer to source bytearray from which the 
     *                   data is copied.
     *
     *
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Append( uint32   Length, 
                         byte    *Bytes, 
                         uint32  *BytesWritten);

    /* Returns the amount of data inside the circular buf.
     *
     * @param pLength -  The pointer to store the length.
     *
     * @return IxErrnoType  - Error number or Success.
     */
    IxErrnoType  Length( uint32   *pLength );

   /* Return state data for the UxCircularBuf.  This data can be used to
   ** recreate the uxCircularBuf.
   */
    void GetState( uint8** ppStateData, uint32* pStateDataLen );

    /* Recreate UxCircularBuf using specified state data.
    */
    void SetState( uint8* pStateData, uint32 stateDataLen );

private:

  /*------------------------------------------------------------------------
                               PRIVATE MEMBERS
  ------------------------------------------------------------------------*/
   uint32   __bufSize;
   uint32   __first;
   uint32   __next;
   uint32   __dataLength;
   uint8*   __circularBuf;
   UxAllocE __pool;

};

#ifdef __cplusplus
}
#endif

#endif


