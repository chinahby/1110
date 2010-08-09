#ifndef PV_BIN_STREAM_H
#define PV_BIN_STREAM_H
/* =======================================================================
                               oscl_bin_stream.h
DESCRIPTION
**       This module defines a set of binary stream classes to provide portability of code accross
**       different platforms and compilers. Output and input stream classes are defined both for
**       little endian and big endian byte orders. These classes should be used when sharing
**       information across platforms (e.g. sending messages) instead of C structures. The classes
**       will read/write in the correct byte order regardless of the platform you are running on.
**       The classes are:
**           PVBinIStreamBigEndian - Input stream for big endian byte order.
**           PVBinIStreamLittleEndian - Input stream for little endian byte order.
**           PVBinOStreamBigEndian - Output stream for big endian byte order.
**           PVBinIStreamLittleEndian - Input stream for little endian byte order.
**
**      The preprocessor defined constants BYTE_ORDER_BIG_ENDIAN, BYTE_ORDER_LITTLE_ENDIAN are defined
**      in pv_standard.h and tell this module the byte order used by the native platform. This is used
**      to decide if we can do a straight copy or we need to manipulate the field one byte at a time.
**      The preprocessor defined constant INTEGERS_BYTE_ALIGNED tells us if the platform supports
**      accessing integers at any address.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/oscl/main/latest/inc/oscl_bin_stream.h#7 $
$DateTime: 2008/05/08 14:04:55 $
$Change: 656428 $


========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_types.h"
#include <string.h>

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
const static int16 NO_BITS_IN_BYTE = 8;
const static uint8 BYTE_MASK = 0xff;

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                          Macro Definitions
** ======================================================================= */

/* =======================================================================
MACRO MYOBJ

ARGS 
  xx_obj - this is the xx argument

DESCRIPTION:
  Complete description of what this macro does
========================================================================== */

/* =======================================================================
**                        Class Declarations
** ======================================================================= */

/* ======================================================================
CLASS 
  PVBinStream

DESCRIPTION
  Thorough, meaningful description of what this function does. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class PVBinStream
{
public:
  PVBinStream()
  : state(GOOD_STATE),
  pBasePosition(0),
  pPosition(0)
  {
  }

  /*
  ** Description: 
  **      Is the stream ok.
  **
  ** Returns: true if stream is ok.
  ** Side effects: None.
  */
  bool good()
  {
    return state == GOOD_STATE;
  }

  /*
  ** Description: 
  **      Is is the end of the stream.
  **
  ** Returns: true if end of stream has been reached.
  ** Side effects: None.
  */
  bool eof()
  {
    return state == EOF_STATE;
  }

  /*
  ** Description: 
  **      Has an error occured in the stream.
  **
  ** Returns: true if an error occured in the stream.
  ** Side effects: None.
  */
  bool fail()
  {
    return state == FAIL_STATE;
  }

  /*
  ** Description: 
  **      Specify the data buffer to attach to the stream. This buffer will provide the input
  **      data to the object.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  void Attach(void * buffer, uint32 l_length)
  {
    fragsLeft = 0;
    pBasePosition = (uint8 *)buffer;
    pPosition = pBasePosition;
    length = l_length;
    state = GOOD_STATE;
    specialFragBuffer.ptr = buffer;
    specialFragBuffer.len = l_length;
    numFrags = 1;
    firstFragPtr = &specialFragBuffer;
  }

  /*
  ** Description: 
  **      Specify the memory fragment array to use for input. This array should remain static
  **      while the stream refers to it.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  void Attach(const uint32 numFragments, const MemoryFragment * fragPtr)
  {
    pBasePosition = (uint8 *)fragPtr->ptr;
    pPosition = pBasePosition;
    numFrags = numFragments;
    firstFragPtr  = fragPtr;
    length = fragPtr->len;
    if ( numFragments > 1 )
    {
      fragsLeft = numFragments - 1;
      nextFragPtr = fragPtr + 1;
    }
    else
    {
      fragsLeft = 0;
    }
    state = GOOD_STATE;
  }

  /*
  ** Description: 
  **      Returns the current stream position.
  **
  ** Returns: Stream position.
  ** Side effects: None.
  */
  uint32 tellg()
  {
    uint32 pos = 0;
    for ( int ii = 0; ii < numFrags - fragsLeft - 1; ii++ )
    {
      pos += firstFragPtr[ii].len;
    }
    pos += PositionInBlock();

    return pos;
  }

  /*
  ** Description: 
  **      Seek to specified stream position.
  **
  ** Returns: Stream position.
  ** Side effects: None.
  */
  void Seek(uint32 absPosition)
  {
    uint32 pos = 0;
    int fragIndex;

    for ( fragIndex = 0; 
        fragIndex < numFrags && absPosition >= pos + firstFragPtr[fragIndex].len; 
        fragIndex++ )
    {
      pos += firstFragPtr[fragIndex].len;
    }

    if ( fragIndex >= numFrags )
    {
      fragsLeft = 0;
      pBasePosition = (uint8 *)firstFragPtr[numFrags-1].ptr;
      length = firstFragPtr[numFrags-1].len;
      pPosition = pBasePosition + length;
      if ( absPosition == pos )
      {
        state = EOF_STATE;
      }
      else
      {
        state = FAIL_STATE;
      }
      return;
    }

    // otherwise there is more data
    nextFragPtr = &firstFragPtr[fragIndex + 1];
    fragsLeft = numFrags - fragIndex - 1;
    pBasePosition = (uint8 *)firstFragPtr[fragIndex].ptr;
    length = firstFragPtr[fragIndex].len;
    uint32 reqBytes = absPosition - pos;
    if ( reqBytes <= length )
    {
      pPosition = pBasePosition + reqBytes;
    }
    else
    {
      pPosition = pBasePosition + length;
      state = FAIL_STATE;
    }
  }

  /*
  ** Description: 
  **      Returns the current stream position.
  **
  ** Returns: Stream position.
  ** Side effects: None.
  */
  uint32 PositionInBlock()
  {
    return pPosition - pBasePosition;
  }

  /*
  ** Description: 
  **      Returns the current stream position.
  **
  ** Returns: Stream position.
  ** Side effects: None.
  */
  void seekFromCurrentPosition(int32 offset)
  {
    Seek(tellg() + offset);
  }



protected:

  /*
  ** Description: 
  **      Reserves space for storing/reading. If no space is available it sets the stream state
  **      to fail and returns false. If the end of the stream has been reached, it sets the state
  **      to end of stream.
  **
  ** Returns: true if space was successfully reserved.
  ** Side effects: None.
  */
  bool ReserveSpace(uint32 size)
  {
    if ( fail() )
    {
      return false;
    }
    uint32 newSize = PositionInBlock() + size;
    if ( newSize > length )
    {
      state = FAIL_STATE;
      return false;
    }
    if ( newSize == length )
    {
      state = EOF_STATE;
    }
    return true;
  }

  bool HaveRoomInCurrentBlock(uint32 size)
  {
    uint32 pos = PositionInBlock() + size;
    if ( pos < length )
    {
      return true;
    }
    if ( pos == length && fragsLeft == 0 )
    {
      state = EOF_STATE;
    }
    return(pos <= length);
  }

  typedef enum 
  {
    GOOD_STATE,
    EOF_STATE,
    FAIL_STATE
  } state_t;

  state_t state;

  /* Position of the beginning of the data buffer */
  uint8 * pBasePosition;

  /* Current data buffer position */
  uint8 * pPosition;

  /* length of data buffer */
  uint32            length;

  const MemoryFragment *  nextFragPtr;
  int               fragsLeft;

  const MemoryFragment *  firstFragPtr;
  int               numFrags;
  MemoryFragment   specialFragBuffer;

};

/* ======================================================================
CLASS 
  PVBinIStream

DESCRIPTION
**      This class implements the basic stream functions for an input stream.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class PVBinIStream : public PVBinStream
{
private:
  public:
  /*
  ** Description: 
  **      Constructor.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinIStream()
  {
  }

  /*
  ** Description: 
  **      Destructor.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  virtual ~PVBinIStream()
  {
  }

  /*
  ** Description: 
  **      Reads an unsigned short from the stream.
  **
  ** Returns: Unsigned short read from the stream.
  ** Side effects: None.
  */
  uint8 Read_uint8()
  {
    if ( HaveRoomInCurrentBlock(sizeof(uint8)) )
    {
      return(*pPosition++);
    }
    if ( fragsLeft )
    {
      pBasePosition = (uint8 *)nextFragPtr->ptr;
      pPosition = pBasePosition;
      length = nextFragPtr->len;
      fragsLeft--;
      nextFragPtr++;
      return(*pPosition++);
    }
    state = FAIL_STATE;
    return 0;
  }

  /*
  ** Description: 
  **      Reads 'length' number of bytes from the stream and places them in 'data'.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinIStream & get(
                    int8 * data,    /* Pointer to the place to store the bytes read */
                    int32 size    /* Number of bytes to read */
                    )
  {
    if ( HaveRoomInCurrentBlock(size) )
    {
      memcpy(data, pPosition, size);
      pPosition += size;
    }
    else
    {
      uint32 pos = PositionInBlock();
      uint32 bytesToCopy = length - pos;
      memcpy(data, pPosition, bytesToCopy);
      data += bytesToCopy;
      uint32 bytesLeft = size - bytesToCopy;
      while ( bytesLeft > 0 && fragsLeft )
      {
        pBasePosition = (uint8 *)nextFragPtr->ptr;
        pPosition = pBasePosition;
        length = nextFragPtr->len;
        fragsLeft--;
        nextFragPtr++;

        if ( bytesLeft <= length )
        {
          bytesToCopy = bytesLeft;
        }
        else
        {
          bytesToCopy = length;
        }

        memcpy(data, pPosition, bytesToCopy);
        data += bytesToCopy;
        pPosition += bytesToCopy;
        bytesLeft -= bytesToCopy;
      }
    }
    return *this;
  }

protected:


};

/* ======================================================================
CLASS 
  PVBinIStreamLittleEndian

DESCRIPTION
**      This class implements a binary input stream using little endian byte ordering

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class PVBinIStreamLittleEndian : public PVBinIStream 
{
public:
  PVBinIStreamLittleEndian()
  {
  }

  /*
  ** Description: 
  **      Reads a int8 from the stream and stores it in 'data'.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinIStreamLittleEndian & operator>>(int8 & data)
  {
    data = int8(Read_uint8());
    return *this;
  }

  /*
  ** Description: 
  **      Reads a uint8 from the stream and stores it in 'data'.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinIStreamLittleEndian & operator>>(uint8 & data)
  {
    data = Read_uint8();
    return *this;
  }

  /*
  ** Description: 
  **      Reads a int16 from the stream and stores it in 'data'.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinIStreamLittleEndian & operator>>(int16 & data)
  {
    data = int16(Read_uint16());

    return *this;
  }

  /*
  ** Description: 
  **      Reads a uint16 from the stream and stores it in 'data'.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinIStreamLittleEndian & operator>>(uint16 & data)
  {
    data = Read_uint16();

    return *this;
  }

  /*
  ** Description: 
  **      Reads a int32 from the stream and stores it in 'data'.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinIStreamLittleEndian & operator>>(int32 & data)
  {
    data = int32(Read_uint32());
    return *this;
  }

  /*
  ** Description: 
  **      Reads a uint32 from the stream and stores it in 'data'.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinIStreamLittleEndian & operator>>(uint32 & data)
  {
    data = Read_uint32();
    return *this;
  }

protected:

  /*
  ** Description: 
  **      Reads an unsigned short from the stream.
  **
  ** Returns: Unsigned short read from the stream.
  ** Side effects: None.
  */
  uint16 Read_uint16()
  {
    if ( HaveRoomInCurrentBlock(sizeof(uint16)) )
    {
    #if defined(BYTE_ORDER_LITTLE_ENDIAN) && !defined(INTEGERS_WORD_ALIGNED)
      uint16 *ptr = (uint16 *)pPosition;
      pPosition += sizeof(uint16);
      return *ptr;
    #else
      uint16 byteB = *pPosition++;
      uint16 byteA = *pPosition++;

      return(( byteA << NO_BITS_IN_BYTE) | byteB );
    #endif
    }
    else
    {
      uint16 byteB = Read_uint8();
      uint16 byteA = Read_uint8();
      return(( byteA << NO_BITS_IN_BYTE) | byteB );
    }
  }

  /*
  ** Description: 
  **      Reads an unsigned long from the stream.
  **
  ** Returns: Unsigned long read from the stream.
  ** Side effects: None.
  */
  uint32 Read_uint32()
  {
    if ( HaveRoomInCurrentBlock(sizeof(uint32)) )
    {
    #if defined(BYTE_ORDER_LITTLE_ENDIAN) && !defined(INTEGERS_WORD_ALIGNED)
      uint32 *ptr = (uint32 *)pPosition;
      pPosition += sizeof(uint32);
      return *ptr;
    #else
      uint32 result;
      uint8 byteD = *pPosition++;
      uint8 byteC = *pPosition++;
      uint8 byteB = *pPosition++;
      uint8 byteA = *pPosition++;

      result = byteA;
      result = (result << NO_BITS_IN_BYTE) | byteB;
      result = (result << NO_BITS_IN_BYTE) | byteC;
      result = (result << NO_BITS_IN_BYTE) | byteD;

      return result;
    #endif
    }
    else
    {
      uint32 result;
      uint8 byteD = Read_uint8();
      uint8 byteC = Read_uint8();
      uint8 byteB = Read_uint8();
      uint8 byteA = Read_uint8();

      result = byteA;
      result = (result << NO_BITS_IN_BYTE) | byteB;
      result = (result << NO_BITS_IN_BYTE) | byteC;
      result = (result << NO_BITS_IN_BYTE) | byteD;

      return result;
    }
  }
};

/* ======================================================================
CLASS 
  PVBinIStreamBigEndian

DESCRIPTION
**      This class implements a binary input stream using big endian byte ordering

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class PVBinIStreamBigEndian : public PVBinIStream 
{
public:
  PVBinIStreamBigEndian()
  {
  }

  void Read(int8 & data)
  {
    data = int8(Read_uint8());
  }

  void Read(uint8 & data)
  {
    data = Read_uint8();
  }

  void Read(int16 & data)
  {
    data = int16(Read_uint16());
  }

  void Read(uint16 & data)
  {
    data = Read_uint16();
  }

  void Read(int32 & data)
  {
    data = int32(Read_uint32());
  }

  void Read(uint32 & data)
  {
    data = Read_uint32();
  }

  /*
  ** Description: 
  **      Reads a int8 from the stream and stores it in 'data'.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinIStreamBigEndian & operator>>(int8 & data)
  {
    data = int8(Read_uint8());
    return *this;
  }

  /*
  ** Description: 
  **      Reads a uint8 from the stream and stores it in 'data'.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinIStream & operator>>(uint8 & data)
  {
    data = Read_uint8();
    return *this;
  }

  /*
  ** Description: 
  **      Reads a int16 from the stream and stores it in 'data'.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinIStreamBigEndian & operator>>(int16 & data)
  {
    data = int16(Read_uint16());
    return *this;
  }

  /*
  ** Description: 
  **      Reads a uint16 from the stream and stores it in 'data'.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinIStreamBigEndian & operator>>(uint16 & data)
  {
    data = Read_uint16();
    return *this;
  }

  /*
  ** Description: 
  **      Reads a int32 from the stream and stores it in 'data'.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinIStreamBigEndian & operator>>(int32 & data)
  {
    data = int32(Read_uint32());
    return *this;
  }

  /*
  ** Description: 
  **      Reads a uint32 from the stream and stores it in 'data'.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinIStreamBigEndian & operator>>(uint32 & data)
  {
    data = Read_uint32();
    return *this;
  }

  /*
  ** Description: 
  **      Reads an unsigned short from the stream.
  **
  ** Returns: Unsigned short read from the stream.
  ** Side effects: None.
  */
  uint16 Read_uint16()
  {
    if ( HaveRoomInCurrentBlock(sizeof(uint16)) )
    {
    #if defined(BYTE_ORDER_BIG_ENDIAN) && !defined(INTEGERS_WORD_ALIGNED)
      uint16 *ptr = (uint16 *)pPosition;
      pPosition += sizeof(uint16);
      return *ptr;
    #else
      uint16 byteA = *pPosition++;
      uint16 byteB = *pPosition++;

      return(( byteA << NO_BITS_IN_BYTE) | byteB );
    #endif
    }
    else
    {
      uint16 byteA = Read_uint8();
      uint16 byteB = Read_uint8();

      return(( byteA << NO_BITS_IN_BYTE) | byteB );
    }
  }

  /*
  ** Description: 
  **      Reads an unsigned long from the stream.
  **
  ** Returns: Unsigned long read from the stream.
  ** Side effects: None.
  */
  uint32 Read_uint32()
  {
    if ( HaveRoomInCurrentBlock(sizeof(uint32)) )
    {
    #if defined(BYTE_ORDER_BIG_ENDIAN) && !defined(INTEGERS_WORD_ALIGNED)
      uint32 *ptr = (uint32 *)pPosition;
      pPosition += sizeof(uint32);
      return *ptr;
    #else
      uint32 result;

      result = *pPosition++;
      result = (result << NO_BITS_IN_BYTE) | *pPosition++;
      result = (result << NO_BITS_IN_BYTE) | *pPosition++;
      result = (result << NO_BITS_IN_BYTE) | *pPosition++;

      return result;
    #endif
    }
    else
    {
      uint32 result;

      result = Read_uint8();
      result = (result << NO_BITS_IN_BYTE) | Read_uint8();
      result = (result << NO_BITS_IN_BYTE) | Read_uint8();
      result = (result << NO_BITS_IN_BYTE) | Read_uint8();

      return result;
    }
  }
protected:

};


/* ======================================================================
CLASS 
  PVBinOStream

DESCRIPTION
**      This class implements the basic stream functions for an output stream.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class PVBinOStream : public PVBinStream
{
public:
  PVBinOStream()
  {
  }

  virtual ~PVBinOStream()
  {
  }

  /*
  ** Description: 
  **      Writes 'length' number of bytes stored in 'data' to the stream.
  **
  ** Returns: none.
  ** Side effects: None.
  */

  PVBinOStream & write(
                      const int8 * data,      /* data to store */
                      int32 size      /* length of data to store */
                      )
  {
    if ( ReserveSpace(size) )
    {
      memcpy(pPosition, data, size);
      pPosition += size;
    }
    return *this;
  }

protected:
};

/* ======================================================================
CLASS 
  PVBinOStreamLittleEndian

DESCRIPTION
**      This class implements a binary output stream using little endian byte ordering

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class PVBinOStreamLittleEndian : public PVBinOStream 
{
public:
  PVBinOStreamLittleEndian()
  {
  }

  /*
  ** Description: 
  **      Writes a int8 from 'data' to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinOStreamLittleEndian & operator<<(const int8 & data)
  {
    if ( ReserveSpace(sizeof(data)) )
    {
      *((int8 *)pPosition) = data;
      pPosition++;
    }
    return *this;
  }

  /*
  ** Description: 
  **      Writes a uint8 from 'data' to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinOStreamLittleEndian & operator<<(const uint8 & data)
  {
    if ( ReserveSpace(sizeof(data)) )
    {
      *pPosition++ = data;
    }
    return *this;
  }

  /*
  ** Description: 
  **      Writes a int16 from 'data' to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinOStreamLittleEndian & operator<<(const int16 & data)
  {
    if ( ReserveSpace(sizeof(data)) )
    {
      WriteUnsignedShort((uint16)data);
    }

    return *this;
  }

  /*
  ** Description: 
  **      Writes a uint16 from 'data' to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinOStreamLittleEndian & operator<<(const uint16 & data)
  {
    if ( ReserveSpace(sizeof(data)) )
    {
      WriteUnsignedShort(data);
    }
    return *this;
  }

  /*
  ** Description: 
  **      Writes a int32 from 'data' to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinOStreamLittleEndian & operator<<(const int32 & data)
  {
    if ( ReserveSpace(sizeof(data)) )
    {
      WriteUnsignedLong(uint32(data));
    }

    return *this;
  }

  /*
  ** Description: 
  **      Writes a uint32 from 'data' to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinOStreamLittleEndian & operator<<(const uint32 & data)
  {
    if ( ReserveSpace(sizeof(data)) )
    {
      WriteUnsignedLong(data);
    }

    return *this;
  }

protected:

  /*
  ** Description: 
  **      Writes 'data' (unsigned short) to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  void WriteUnsignedShort(const uint16 data)
  {
    #if defined(BYTE_ORDER_LITTLE_ENDIAN) && !defined(INTEGERS_WORD_ALIGNED)
    uint16 *ptr = (uint16 *)pPosition;
    pPosition += sizeof(uint16);
    *ptr = data;
    #else
    uint8 byteB = (uint8)data;
    uint8 byteA = data >> NO_BITS_IN_BYTE;

    *pPosition++ = byteB;
    *pPosition++ = byteA;
    #endif
  }

  /*
  ** Description: 
  **      Writes 'data' (unsigned long) to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  void WriteUnsignedLong(const uint32 data)
  {
    #if defined(BYTE_ORDER_LITTLE_ENDIAN) && !defined(INTEGERS_WORD_ALIGNED)
    uint32 *ptr = (uint32 *)pPosition;
    pPosition += sizeof(uint32);
    *ptr = data;
    #else
    uint32 temp = data;
    *pPosition++ = (uint8)temp;
    temp >>= NO_BITS_IN_BYTE;
    *pPosition++ = (uint8)temp;
    temp >>= NO_BITS_IN_BYTE;
    *pPosition++ = (uint8)temp;
    temp >>= NO_BITS_IN_BYTE;
    *pPosition++ = (uint8)temp;
    #endif
  }

};

/* ======================================================================
CLASS 
  PVBinOStreamBigEndian

DESCRIPTION
**      This class implements a binary output stream using big endian byte ordering

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class PVBinOStreamBigEndian : public PVBinOStream 
{
public:
  PVBinOStreamBigEndian()
  {
  }

  /*
  ** Description: 
  **      Writes a int8 from 'data' to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinOStreamBigEndian & operator<<(const int8 & data)
  {
    if ( ReserveSpace(sizeof(data)) )
    {
      *((int8 *)pPosition) = data;
      pPosition++;
    }

    return *this;
  }

  /*
  ** Description: 
  **      Writes a uint8 from 'data' to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinOStreamBigEndian & operator<<(const uint8 & data)
  {
    if ( ReserveSpace(sizeof(data)) )
    {
      *pPosition++ = data;
    }
    return *this;
  }

  /*
  ** Description: 
  **      Writes a int16 from 'data' to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinOStreamBigEndian & operator<<(const int16 & data)
  {
    if ( ReserveSpace(sizeof(data)) )
    {
      WriteUnsignedShort((uint16)data);
    }
    return *this;
  }

  /*
  ** Description: 
  **      Writes a uint16 from 'data' to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinOStreamBigEndian & operator<<(const uint16 & data)
  {
    if ( ReserveSpace(sizeof(data)) )
    {
      WriteUnsignedShort(data);
    }
    return *this;
  }

  /*
  ** Description: 
  **      Writes a int32 from 'data' to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinOStreamBigEndian & operator<<(const int32 & data)
  {
    if ( ReserveSpace(sizeof(data)) )
    {
      WriteUnsignedLong(uint32(data));
    }
    return *this;
  }

  /*
  ** Description: 
  **      Writes a uint32 from 'data' to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  PVBinOStreamBigEndian & operator<<(const uint32 & data)
  {
    if ( ReserveSpace(sizeof(data)) )
    {
      WriteUnsignedLong(data);
    }

    return *this;
  }

protected:

  /*
  ** Description: 
  **      Writes 'data' (unsigned short) to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  void WriteUnsignedShort(const uint16 data)
  {
    #if defined(BYTE_ORDER_BIG_ENDIAN) && !defined(INTEGERS_WORD_ALIGNED)
    uint16 *ptr = (uint16 *)pPosition;
    pPosition += sizeof(uint16);
    *ptr = data;
    #else
    uint8 byteB = (uint8)data;
    uint8 byteA = data >> NO_BITS_IN_BYTE;

    *pPosition++ = byteA;
    *pPosition++ = byteB;
    #endif
  }

  /*
  ** Description: 
  **      Writes 'data' (unsigned long) to the stream.
  **
  ** Returns: None.
  ** Side effects: None.
  */
  void WriteUnsignedLong(const uint32 data)
  {
    #if defined(BYTE_ORDER_BIG_ENDIAN) && !defined(INTEGERS_WORD_ALIGNED)
    uint32 *ptr = (uint32 *)pPosition;
    pPosition += sizeof(uint32);
    *ptr = data;
    #else
    uint32 temp = data;
    uint8 byteD = (uint8)temp;
    temp >>= NO_BITS_IN_BYTE;
    uint8 byteC = (uint8)temp;
    temp >>= NO_BITS_IN_BYTE;
    uint8 byteB = (uint8)temp;
    temp >>= NO_BITS_IN_BYTE;
    uint8 byteA = (uint8)temp;

    *pPosition++ = byteA;
    *pPosition++ = byteB;
    *pPosition++ = byteC;
    *pPosition++ = byteD;
    #endif
  }
};

#endif
