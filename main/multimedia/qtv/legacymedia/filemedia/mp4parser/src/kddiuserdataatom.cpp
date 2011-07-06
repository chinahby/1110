/* =======================================================================
                              kddiuserdataatom.cpp
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.  Include any initialization and synchronizing
  requirements.

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.
  
Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/src/kddiuserdataatom.cpp#12 $
$DateTime: 2010/04/01 01:53:20 $
$Change: 1247377 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "target.h"             /* Target specific definitions             */

#if defined(FEATURE_MP4_KDDI_META_DATA)

#include "kddiuserdataatom.h"
#include "atomdefs.h"

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

/* ======================================================================
FUNCTION 
  KDDIDrmAtom::KDDIDrmAtom

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
KDDIDrmAtom::KDDIDrmAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  if ( _success )
  {
    if ( !AtomUtils::read32(fp, _copyGaurdAttribute) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_KDDI_DRM_ATOM_FAILED;
      return;
    }
    if ( !AtomUtils::read32(fp, _limitDate) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_KDDI_DRM_ATOM_FAILED;
      return;
    }
    if ( !AtomUtils::read32(fp, _limitPeriod) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_KDDI_DRM_ATOM_FAILED;
      return;
    }
    if ( !AtomUtils::read32(fp, _limitCount) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_KDDI_DRM_ATOM_FAILED;
      return;
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_KDDI_DRM_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  KDDIContentPropertyAtom::KDDIContentPropertyAtom

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
KDDIContentPropertyAtom::KDDIContentPropertyAtom(OSCL_FILE *fp) : FullAtom(fp)
{
  if ( _success )
  {
    uint32 atomType, atomSize;
    uint8 tempBuf[256]; /* temp buffer to hold data */
    _dllVersion = 0;

    uint32 count = DEFAULT_ATOM_SIZE + 4 + 16;

    /* SKA: Since this is stored as another atom, first read its size and type
    */
    if ( !AtomUtils::read32read32(fp, atomSize, atomType) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_KDDI_CONTENT_PROPERTY_ATOM_FAILED;
      return;
    }
    count += 8;

    while ( (count < _size) &&
            ((atomType == FourCharConstToUint32('t', 'i', 't', 'l')) ||
             (atomType == FourCharConstToUint32('r', 'g', 'h', 't')) ||
             (atomType == FourCharConstToUint32('a', 't', 'h', 'r')) ||
             (atomType == FourCharConstToUint32('m', 'e', 'm', 'o')) ||
             (atomType == FourCharConstToUint32('v', 'r', 's', 'n'))) )
    {
      if ( atomType == FourCharConstToUint32('t', 'i', 't', 'l') )
      {
        uint8 readSize = (uint8)MIN(atomSize-8, sizeof(tempBuf)-1);
        if ( !AtomUtils::readByteData(fp, readSize, tempBuf) ) /* 4 bytes for size and 4 bytes for type */
        {
          _success = false;
          _fileErrorCode = (int32)READ_KDDI_CONTENT_PROPERTY_ATOM_FAILED;
          return;
        }
        tempBuf[readSize] = '\0';
        _title = (const OSCL_TCHAR *)tempBuf;
        count += (atomSize-8);
      }
      else if ( atomType == FourCharConstToUint32('r', 'g', 'h', 't') )
      {
        uint8 readSize = (uint8)MIN(atomSize-8, sizeof(tempBuf)-1);
        if ( !AtomUtils::readByteData(fp, readSize, tempBuf) ) /* 4 bytes for size and 4 bytes for type */
        {
          _success = false;
          _fileErrorCode = (int32)READ_KDDI_CONTENT_PROPERTY_ATOM_FAILED;
          return;
        }
        tempBuf[readSize] = '\0';
        _copyRight = (const OSCL_TCHAR *)tempBuf;
        count += (atomSize-8);
      }
      else if ( atomType == FourCharConstToUint32('a', 't', 'h', 'r') )
      {
        uint8 readSize = (uint8)MIN(atomSize-8, sizeof(tempBuf)-1);
        if ( !AtomUtils::readByteData(fp, readSize, tempBuf) ) /* 4 bytes for size and 4 bytes for type */
        {
          _success = false;
          _fileErrorCode = (int32)READ_KDDI_CONTENT_PROPERTY_ATOM_FAILED;
          return;
        }
        tempBuf[readSize] = '\0';
        _author = (const OSCL_TCHAR *)tempBuf;
        count += (atomSize-8);
      }
      else if ( atomType == FourCharConstToUint32('m', 'e', 'm', 'o') )
      {
        uint8 readSize = (uint8)MIN(atomSize-8, sizeof(tempBuf)-1);
        if ( !AtomUtils::readByteData(fp, readSize, tempBuf) ) /* 4 bytes for size and 4 bytes for type */
        {
          _success = false;
          _fileErrorCode = (int32)READ_KDDI_CONTENT_PROPERTY_ATOM_FAILED;
          return;
        }
        tempBuf[readSize] = '\0';
        _memo = (const OSCL_TCHAR *)tempBuf;
        count += (atomSize-8);
      }
      else if ( atomType == FourCharConstToUint32('v', 'r', 's', 'n') )
      {
        if ( !AtomUtils::read32(fp, _dllVersion) )
        {
          _success = false;
          _fileErrorCode = (int32)READ_KDDI_CONTENT_PROPERTY_ATOM_FAILED;
          return;
        }
        count += 4;
      }

      if ( count < _size )
      {
        /* get next atom info */
        if ( !AtomUtils::read32read32(fp, atomSize, atomType) )
        {
          _success = false;
          _fileErrorCode = (int32)READ_KDDI_CONTENT_PROPERTY_ATOM_FAILED;
          return;
        }
        count += 8;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_KDDI_CONTENT_PROPERTY_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  KDDIMovieMailAtom::KDDIMovieMailAtom

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
KDDIMovieMailAtom::KDDIMovieMailAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  if ( _success )
  {
    uint32 temp;

    if ( !AtomUtils::read32(fp, temp) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_KDDI_MOVIE_MAIL_ATOM_FAILED;
      return;
    }
    _editFlags     = (temp >> 8) & 0x00FFFFFF;
    _recordingMode = (uint8)(temp & 0xFF);

    if ( !AtomUtils::read32(fp, _recDate) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_KDDI_MOVIE_MAIL_ATOM_FAILED;
      return;
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_KDDI_MOVIE_MAIL_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  KDDIEncoderInformationAtom::KDDIEncoderInformationAtom

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
KDDIEncoderInformationAtom::KDDIEncoderInformationAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _deviceName         = NULL;
  _modelName          = NULL;
  _encoderInformation = NULL;
  _muxInformation     = NULL;

  if ( _success )
  {
    _deviceName         = (uint8*)QTV_Malloc(8);
    _modelName          = (uint8*)QTV_Malloc(8);
    _encoderInformation = (uint8*)QTV_Malloc(8);
    _muxInformation     = (uint8*)QTV_Malloc(8);

    if ( !AtomUtils::readByteData(fp, 8, _deviceName) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_KDDI_ENCODER_INFO_ATOM_FAILED;
      return;
    }
    if ( !AtomUtils::readByteData(fp, 8, _modelName) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_KDDI_ENCODER_INFO_ATOM_FAILED;
      return;
    }
    if ( !AtomUtils::readByteData(fp, 8, _encoderInformation) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_KDDI_ENCODER_INFO_ATOM_FAILED;
      return;
    }
    if ( !AtomUtils::readByteData(fp, 8, _muxInformation) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_KDDI_ENCODER_INFO_ATOM_FAILED;
      return;
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_KDDI_ENCODER_INFO_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  KDDIEncoderInformationAtom::~KDDIEncoderInformationAtom

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
KDDIEncoderInformationAtom::~KDDIEncoderInformationAtom()
{
  if ( _deviceName != NULL )
  {
    QTV_Free(_deviceName);
  }
  if ( _modelName != NULL )
  {
    QTV_Free(_modelName);
  }
  if ( _encoderInformation != NULL )
  {
    QTV_Free(_encoderInformation);
  }
  if ( _muxInformation != NULL )
  {
    QTV_Free(_muxInformation);
  }
}

/* ======================================================================
FUNCTION 
  KDDITelopAtom::KDDITelopAtom

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
KDDITelopAtom::KDDITelopAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _pKddiTelopText = NULL;
  _telopSize      = 0;

  if ( _success )
  {
    _telopSize = _size - (DEFAULT_UUID_ATOM_SIZE);

    _pKddiTelopText = (uint8*)QTV_Malloc(_telopSize);

    if ( !AtomUtils::readByteData(fp, _telopSize, _pKddiTelopText) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_KDDI_TELOP_ATOM_FAILED;
      return;
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_KDDI_TELOP_ATOM_FAILED;
    return;
  }
}


/* ======================================================================
FUNCTION 
  KDDITelopAtom::~KDDITelopAtom

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
KDDITelopAtom::~KDDITelopAtom()
{
  if ( _pKddiTelopText != NULL )
  {
    QTV_Free(_pKddiTelopText);
  }
}

/* ======================================================================
FUNCTION 
  KDDIGPSIFDEntry::KDDIGPSIFDEntry

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
KDDIGPSIFDEntry::KDDIGPSIFDEntry(OSCL_FILE *fp)
{
  _success = true;

  if ( !AtomUtils::read16(fp, _tagID) )
  {
    _success = false;
    return;
  }
  if ( !AtomUtils::read16(fp, _typeValue) )
  {
    _success = false;
    return;
  }
  if ( !AtomUtils::read32(fp, _dataCount) )
  {
    _success = false;
    return;
  }
  if ( !AtomUtils::read32(fp, _value) )
  {
    _success = false;
    return;
  }
}

/* ======================================================================
FUNCTION 
  KDDIGPSExtensionAtom::KDDIGPSExtensionAtom

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
KDDIGPSExtensionAtom::KDDIGPSExtensionAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  if ( _success )
  {
    if ( !AtomUtils::read64(fp, _mapScaleInfo) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_KDDI_GPS_ATOM_FAILED;
      return;
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_KDDI_GPS_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  KDDIGPSAtom::KDDIGPSAtom

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
KDDIGPSAtom::KDDIGPSAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _kddiGPSExtensionAtom = NULL;
  _gpsIFDEntryVec = QTV_New( ZArray<KDDIGPSIFDEntry*> );

  uint32 i, j, type, atomSize, versionAndFlags, startOfAtom, startOfData;
  KDDIGPSIFDEntry *ifdElement;


  startOfAtom = (uint32)OSCL_FileTell(fp);

  /* Since this is stored as another atom, first read its size, type and flags
  */
  if ( !AtomUtils::read32read32(fp, atomSize, type) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_KDDI_GPS_ATOM_FAILED;
    return;
  }

  if ( !AtomUtils::read32(fp, versionAndFlags) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_KDDI_GPS_ATOM_FAILED;
    return;
  }

  startOfData = (uint32)OSCL_FileTell(fp);

  if ( !AtomUtils::read16(fp, _byteOrder) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_KDDI_GPS_ATOM_FAILED;
    return;
  }

  if ( !AtomUtils::read16(fp, _numIFD) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_KDDI_GPS_ATOM_FAILED;
    return;
  }

  /* read all the IFD entries */
  for ( i = 0; i < _numIFD; i++ )
  {
    ifdElement = QTV_New_Args( KDDIGPSIFDEntry, (fp) );
    if(!ifdElement)
    {
      _success = false;
      _fileErrorCode = (int32)READ_KDDI_GPS_ATOM_FAILED;
      return;
    }

    if ( ifdElement->getSuccess() )
    {
      if(_gpsIFDEntryVec)
      {
        (*_gpsIFDEntryVec) += ifdElement;
      }
      else
      {
        _success = false;
        _fileErrorCode = (int32)READ_KDDI_GPS_ATOM_FAILED;
        QTV_Delete(ifdElement);
        ifdElement = NULL;
        return;
      }
    }
    else
    {
      _success = false;
      if(ifdElement)
      {
        QTV_Delete(ifdElement);
      }
      _fileErrorCode = (int32)READ_KDDI_GPS_ATOM_FAILED;
      return;
    }
  }

  if ( !AtomUtils::read32(fp, _nextIFDOffset) )
  {
    _success = false;
    _fileErrorCode = (int32)READ_KDDI_GPS_ATOM_FAILED;
    return;
  }

  /* get data from all the IFD entries */
  uint32 ifdValue = 0;
  uint32 tagID = 0;
  uint32 dataCount = 0;
  for ( i = 0; i < _numIFD; i++ )
  {
    ifdElement  = getIFDEntryAt((int32)i);
    if(ifdElement != NULL)
    {
      ifdValue    = ifdElement->getValue();
      tagID       = ifdElement->getTagID();
      dataCount   = ifdElement->gatDataCount();

      switch ( tagID )
      {
      case KDDI_GPS_IFD_TAG_VERSION:
        _versionID = ifdValue;
        break;

      case KDDI_GPS_IFD_TAG_LATITUDE_REF:
        _latitudeRef = ifdValue;
        break;

      case KDDI_GPS_IFD_TAG_LATITUDE:
        /* ifdVakue is offset for data */
        (void)OSCL_FileSeek(fp, (int32)(startOfData+ifdValue), SEEK_SET);
        for ( j = 0; j < QTV_MAX_GPS_LATITUDE; j++ )
        {
          if ( !AtomUtils::read64(fp, _gpsLatitude[j]) )
          {
            _success = false;
            _fileErrorCode = (int32)READ_KDDI_GPS_ATOM_FAILED;
            return;
          }
        }
        break;

      case KDDI_GPS_IFD_TAG_LONGITUDE_REF:
        _longitudeRef = ifdValue;
        break;

      case KDDI_GPS_IFD_TAG_LONGITUDE:
        /* ifdVakue is offset for data */
        (void)OSCL_FileSeek(fp, (int32)(startOfData+ifdValue), SEEK_SET);
        for ( j = 0; j < QTV_MAX_GPS_LONGITUDE; j++ )
        {
          if ( !AtomUtils::read64(fp, _gpsLongitude[j]) )
          {
            _success = false;
            _fileErrorCode = (int32)READ_KDDI_GPS_ATOM_FAILED;
            return;
          }
        }
        break;

      case KDDI_GPS_IFD_TAG_ALTITUDE_REF:
        _altitudeRef = ifdValue;
        break;

      case KDDI_GPS_IFD_TAG_ALTITUDE:
        /* ifdVakue is offset for data */
        (void)OSCL_FileSeek(fp, (int32)(startOfData+ifdValue), SEEK_SET);
        if ( !AtomUtils::read64(fp, _gpsAltitude) )
        {
          _success = false;
          _fileErrorCode = (int32)READ_KDDI_GPS_ATOM_FAILED;
          return;
        }
        break;

      case KDDI_GPS_IFD_TAG_TIMESTAMP:
        /* ifdVakue is offset for data */
        (void)OSCL_FileSeek(fp, (int32)(startOfData+ifdValue), SEEK_SET);
        for ( j = 0; j < QTV_MAX_GPS_TIME; j++ )
        {
          if ( !AtomUtils::read64(fp, _gpsTime[j]) )
          {
            _success = false;
            _fileErrorCode = (int32)READ_KDDI_GPS_ATOM_FAILED;
            return;
          }
        }
        break;

      case KDDI_GPS_IFD_TAG_GEODETIC_SRV:
       /* ifdVakue is offset for data */
        (void)OSCL_FileSeek(fp, (int32)(startOfData+ifdValue), SEEK_SET);
        if ( !AtomUtils::readNullTerminatedString(fp, _gpsGeodeticSurveyData) )
        {
          _success = false;
          _fileErrorCode = (int32)READ_PV_USER_DATA_ATOM_FAILED;
          return;
        }
        break;
  
      case KDDI_GPS_IFD_TAG_POS_SYSTEM:
        {
          uint8 * tempBuf = (uint8*)QTV_Malloc(256);
          if (!tempBuf)
          {
            _success = false;
            _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
            return;
          }
          /* ifdVakue is offset for data */
          (void)OSCL_FileSeek(fp, (int32)(startOfData+ifdValue), SEEK_SET);
          if ( !AtomUtils::readByteData(fp, dataCount, tempBuf) )
          {
            _success = false;
            _fileErrorCode = (int32)READ_PV_USER_DATA_ATOM_FAILED;
            QTV_Free(tempBuf);
            return;
          }
          tempBuf[dataCount] = '\0';
          OSCL_STRING tempStr((const OSCL_TCHAR *)(tempBuf+8));
         _positioningMethod = tempStr;
          QTV_Free(tempBuf);
          break;
       }

       case KDDI_GPS_IFD_TAG_POS_POINT_NAME:
        {
          uint8 * tempBuf = (uint8*)QTV_Malloc(256);
          if (!tempBuf)
          {
            _success = false;
            _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
            return;
          }
          /* ifdVakue is offset for data */
          (void)OSCL_FileSeek(fp, (int32)(startOfData+ifdValue), SEEK_SET);
          if ( !AtomUtils::readByteData(fp, dataCount, tempBuf) )
          {
            _success = false;
            _fileErrorCode = (int32)READ_PV_USER_DATA_ATOM_FAILED;
            QTV_Free(tempBuf);
            return;
          }
          tempBuf[dataCount] = '\0';
          OSCL_STRING tempStr((const OSCL_TCHAR *)(tempBuf+8));
          _positioningMethod = tempStr;
          QTV_Free(tempBuf);
          break;
        }

      case KDDI_GPS_IFD_TAG_DATESTAMP:
        /* ifdVakue is offset for data */
        (void)OSCL_FileSeek(fp, (int32)(startOfData+ifdValue), SEEK_SET);
        if ( !AtomUtils::readNullTerminatedString(fp, _gpsDate) )
        {
          _success = false;
          _fileErrorCode = (int32)READ_PV_USER_DATA_ATOM_FAILED;
          return;
        }
        break;

      default:
        break;
      }
    }
  } /* end of for(num of IFDs) */

  /* set the file pointer to end of GPS Atom */
  (void)OSCL_FileSeek(fp, (int32)(startOfAtom+atomSize), SEEK_SET);

  if ( AtomUtils::getNextAtomType(fp) == KDDI_GPS_EXTENSION_ATOM )
  {
    _kddiGPSExtensionAtom = QTV_New_Args( KDDIGPSExtensionAtom, (fp) );
  }
}

/* ======================================================================
FUNCTION 
  KDDIGPSAtom::~KDDIGPSAtom

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
KDDIGPSAtom::~KDDIGPSAtom()
{ 
  if ( _kddiGPSExtensionAtom != NULL )
  {
    QTV_Delete( _kddiGPSExtensionAtom );
  }

  for ( int32 i = 0; i < _gpsIFDEntryVec->GetLength(); i++ ) 
  {
    QTV_Delete( (*_gpsIFDEntryVec)[i] );
  }

  QTV_Delete( (_gpsIFDEntryVec) );
}
/* ======================================================================
FUNCTION 
  KDDIGPSAtom::getIFDEntryAt

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
KDDIGPSIFDEntry *KDDIGPSAtom::getIFDEntryAt(int32 index)
{
  if ( (_gpsIFDEntryVec->GetLength() == 0) ||
       (index > (_gpsIFDEntryVec->GetLength())) )
  {
    return NULL;
  }
  else
  {
    return(*_gpsIFDEntryVec)[index];
  }
}

#endif /* defined(FEATURE_MP4_KDDI_META_DATA) */
