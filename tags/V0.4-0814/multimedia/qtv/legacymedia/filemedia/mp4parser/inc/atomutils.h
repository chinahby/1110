#ifndef __AtomUtils_H__
#define __AtomUtils_H__
/* =======================================================================
                              atomutils.h
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/inc/atomutils.h#9 $
$DateTime: 2008/05/07 15:14:25 $
$Change: 655407 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_file_io.h"
#include "ztl.h"

/* =======================================================================
**                        Class Declarations
** ======================================================================= */
class AtomUtils
{

public:

  static uint32 fileSize;


  static bool READ_OLD_DESCRIPTOR;


  // Methods for reading in data from a file stream
  static bool read64(OSCL_FILE *fp, uint64 &data);
  static bool read32(OSCL_FILE *fp, uint32 &data);
  static bool read32read32(OSCL_FILE *fp, uint32 &data1, uint32 &data2);
  static bool read24(OSCL_FILE *fp, uint32 &data);
  static bool read16(OSCL_FILE *fp, uint16 &data);
  static bool read16read16(OSCL_FILE *fp, uint16 &data1, uint16 &data2);
  static bool read8(OSCL_FILE *fp, uint8 &data);
  static bool read8read8(OSCL_FILE *fp, uint8 &data1, uint8 &data2);
  static bool readNullTerminatedString(OSCL_FILE *fp, OSCL_STRING &data);

  static bool readNullTerminatedUnicodeString(OSCL_FILE *fp, OSCL_STRING &data);
  static bool readNullTerminatedAsciiString(OSCL_FILE *fp, OSCL_STRING &data);

  static bool readByteData(OSCL_FILE *fp, uint32 length, uint8 *data);

  // Method to set time value in seconds since 1904
  static void setTime(uint32& ulTime);

  // This method is used to calculate the number of bytes needed to store the 
  // overall size of the class - the value contentSize is the size of the class
  // NOT including the actual _sizeOfClass field (since this is a variable-length
  // field).  This is used when determining the actual _sizeOfClass value for
  // all the Descriptor and Command classes.
  static uint32 getNumberOfBytesUsedToStoreSizeOfClass(uint32 contentSize);

  // Returns the atom type from parsing the input stream
  static uint32 getNextAtomType(OSCL_FILE *fp);
  static int32 getNextAtomSize(OSCL_FILE *fp);
  static uint32 getMediaTypeFromHandlerType(uint32 handlerType);
  static uint32 getNumberOfBytesUsedToStoreContent(uint32 sizeOfClass);

  // Peeks and returns the next Nth byte from the file
  static uint32 peekNextNthBytes(OSCL_FILE *fp, int32 n);
  static uint8  peekNextByte(OSCL_FILE *fp);

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  static uint32 getNextUUIDAtomType(OSCL_FILE *fp);
  static bool read32(uint8 *&buf, uint32 &data);
  static bool read32read32(uint8 *&buf, uint32 &data1, uint32 &data2);
  static bool read16(uint8 *&buf, uint16 &data);
  static bool read8(uint8 *&buf, uint8 &data);
  static bool readByteData(uint8 *&buf, uint32 length, uint8 *data);
  static uint32 getNextAtomType(uint8 *buf);
  static int32 getNextAtomSize(uint8 *buf);
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

};

#endif /* __AtomUtils_H__ */
