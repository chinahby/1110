/* =======================================================================
                              udtaAtoms.cpp
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
  
Copyright 2004 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/src/udtaatoms.cpp#10 $
$DateTime: 2008/08/04 05:56:16 $
$Change: 716103 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "target.h"             /* Target specific definitions             */

#include "udtaatoms.h"
#include "atomdefs.h"

#if defined(FEATURE_QTV_SKT_MOD_MIDI)
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
  UdtaMidiAtom::UdtaMidiAtom

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
UdtaMidiAtom::UdtaMidiAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _midiData = NULL;
  _midiDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag to get midi data size */
    _midiDataSize = Atom::getSize() - 12;
    if(_midiDataSize)
    {
      _midiData = (uint8*)QTV_Malloc(_midiDataSize); /* free in destructure */
      if(_midiData)
      {
        if ( !AtomUtils::readByteData(fp, _midiDataSize, _midiData) )
        {
          _midiDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for MIDI failed.");
        _midiDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  UdtaMidiAtom::UdtaMidiAtom

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
UdtaMidiAtom::~UdtaMidiAtom()
{
  if(_midiData)
    QTV_Free(_midiData);
}

/* ======================================================================
FUNCTION:
  UdtaMidiAtom::getUdtaMidiDataSize

DESCRIPTION:
  returns MIDI data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of MIDI data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaMidiAtom::getUdtaMidiDataSize()
{
  return _midiDataSize;
}

/* ======================================================================
FUNCTION:
  UdtaMidiAtom::getUdtaMidiData

DESCRIPTION:
  copies the MIDI data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaMidiAtom::getUdtaMidiData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_midiDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _midiDataSize-dwOffset);
    memcpy(pBuf, _midiData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}


/* ======================================================================
FUNCTION 
  UdtaLinkAtom::UdtaLinkAtom

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
UdtaLinkAtom::UdtaLinkAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _linkData = NULL;
  _linkDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag to get link data size */
    _linkDataSize = Atom::getSize() - 12;
    if(_linkDataSize)
    {
      _linkData = (uint8*)QTV_Malloc(_linkDataSize); /* free in destructure */
      if(_linkData)
      {
        if ( !AtomUtils::readByteData(fp, _linkDataSize, _linkData) )
        {
          _linkDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for LINK DATA failed.");
        _linkDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  UdtaLinkAtom::~UdtaLinkAtom

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
UdtaLinkAtom::~UdtaLinkAtom()
{
  if(_linkData)
    QTV_Free(_linkData);
}

/* ======================================================================
FUNCTION:
  UdtaLinkAtom::getUdtaLinkDataSize

DESCRIPTION:
  returns Link data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of Link data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaLinkAtom::getUdtaLinkDataSize()
{
  return _linkDataSize;
}

/* ======================================================================
FUNCTION:
  UdtaLinkAtom::getUdtaLinkData

DESCRIPTION:
  copies the Link data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaLinkAtom::getUdtaLinkData(uint8* pBuf, uint32 dwSize)
{
  if(_linkDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _linkDataSize);
    memcpy(pBuf, _linkData, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}

#endif /* defined(FEATURE_QTV_SKT_MOD_MIDI) */

/* ======================================================================
FUNCTION 
  FtypAtom::FtypAtom

DESCRIPTION
  Filt Type atom constructor. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
FtypAtom::FtypAtom(OSCL_FILE *fp)
: Atom(fp)
{
  _ftypData = NULL;
  _ftypDataSize = 0;

  if ( _success )
  {

    /* skip 4 bytes size, 4 bytes type to get ftyp data size */
    _ftypDataSize = Atom::getSize() - 8;
    if(_ftypDataSize)
    {
      _ftypData = (uint8*)QTV_Malloc(_ftypDataSize); /* free in destructure */
      if(_ftypData)
      {
        if ( !AtomUtils::readByteData(fp, _ftypDataSize, _ftypData) )
        {
          _ftypDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_FTYP_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for FTYP DATA failed.");
        _ftypDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_FTYP_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  FtypAtom::~FtypAtom

DESCRIPTION
  Filt Type atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
FtypAtom::~FtypAtom()
{
  if(_ftypData)
    QTV_Free(_ftypData);
}

/* ======================================================================
FUNCTION:
  FtypAtom::getFtypDataSize

DESCRIPTION:
  returns ftyp data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of ftyp data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 FtypAtom::getFtypDataSize()
{
  return _ftypDataSize;
}

/* ======================================================================
FUNCTION:
  FtypAtom::getFtypData

DESCRIPTION:
  copies the ftyp data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 FtypAtom::getFtypData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_ftypDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _ftypDataSize-dwOffset);
    memcpy(pBuf, _ftypData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}


/* ======================================================================
FUNCTION 
  DrefAtom::DrefAtom

DESCRIPTION
  Data Reference atom constructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
DrefAtom::DrefAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _drefData = NULL;
  _drefDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag to get link data size */
    _drefDataSize = Atom::getSize() - 12;
    if(_drefDataSize)
    {
      _drefData = (uint8*)QTV_Malloc(_drefDataSize); /* free in destructure */
      if(_drefData)
      {
        if ( !AtomUtils::readByteData(fp, _drefDataSize, _drefData) )
        {
          _drefDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_DREF_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for DREF DATA failed.");
        _drefDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_DREF_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  DrefAtom::~DrefAtom

DESCRIPTION
  Data Reference atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
DrefAtom::~DrefAtom()
{
  if(_drefData)
    QTV_Free(_drefData);
}

/* ======================================================================
FUNCTION:
  DrefAtom::getDrefDataSize

DESCRIPTION:
  returns dref data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of dref data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 DrefAtom::getDrefDataSize()
{
  return _drefDataSize;
}

/* ======================================================================
FUNCTION:
  DrefAtom::getDrefData

DESCRIPTION:
  copies the dref data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 DrefAtom::getDrefData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_drefDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _drefDataSize-dwOffset);
    memcpy(pBuf, _drefData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}


/* ======================================================================
FUNCTION 
  UdtaCprtAtom::UdtaCprtAtom

DESCRIPTION
  Copyright atom constructor. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaCprtAtom::UdtaCprtAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _cprtData = NULL;
  _cprtDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag to get link data size */
    _cprtDataSize = Atom::getSize() - 12;
    if(_cprtDataSize)
    {
      _cprtData = (uint8*)QTV_Malloc(_cprtDataSize); /* free in destructure */
      if(_cprtData)
      {
        if ( !AtomUtils::readByteData(fp, _cprtDataSize, _cprtData) )
        {
          _cprtDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for CPRT DATA failed.");
        _cprtDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  UdtaCprtAtom::~UdtaCprtAtom

DESCRIPTION
  Copyright atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaCprtAtom::~UdtaCprtAtom()
{
  if(_cprtData)
    QTV_Free(_cprtData);
}

/* ======================================================================
FUNCTION:
  UdtaCprtAtom::getUdtaCprtDataSize

DESCRIPTION:
  returns cprt data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of cprt data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaCprtAtom::getUdtaCprtDataSize()
{
  return _cprtDataSize;
}

/* ======================================================================
FUNCTION:
  UdtaCprtAtom::getUdtaCprtData

DESCRIPTION:
  copies the cprt data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaCprtAtom::getUdtaCprtData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_cprtDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _cprtDataSize-dwOffset);
    memcpy(pBuf, _cprtData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}


/* ======================================================================
FUNCTION 
  UdtaAuthAtom::UdtaAuthAtom

DESCRIPTION
  Author atom constructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaAuthAtom::UdtaAuthAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _authData = NULL;
  _authDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag to get link data size */
    _authDataSize = Atom::getSize() - 12;
    if(_authDataSize)
    {
      _authData = (uint8*)QTV_Malloc(_authDataSize); /* free in destructure */
      if(_authData)
      {
        if ( !AtomUtils::readByteData(fp, _authDataSize, _authData) )
        {
          _authDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for AUTH DATA failed.");
        _authDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  UdtaAuthAtom::~UdtaAuthAtom

DESCRIPTION
  Author atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaAuthAtom::~UdtaAuthAtom()
{
  if(_authData)
    QTV_Free(_authData);
}

/* ======================================================================
FUNCTION:
  UdtaAuthAtom::getUdtaAuthDataSize

DESCRIPTION:
  returns auth data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of auth data.
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaAuthAtom::getUdtaAuthDataSize()
{
  return _authDataSize;
}

/* ======================================================================
FUNCTION:
  UdtaAuthAtom::getUdtaAuthData

DESCRIPTION:
  copies the auth data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaAuthAtom::getUdtaAuthData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_authDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _authDataSize-dwOffset);
    memcpy(pBuf, _authData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}

/* ======================================================================
FUNCTION 
  UdtaAlbumAtom::UdtaAlbumAtom

DESCRIPTION
  Author atom constructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaAlbumAtom::UdtaAlbumAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _albumData = NULL;
  _albumDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag to get link data size */
    _albumDataSize = Atom::getSize() - 12;
    if(_albumDataSize)
    {
      _albumData = (uint8*)QTV_Malloc(_albumDataSize); /* free in destructure */
      if(_albumData)
      {
        if ( !AtomUtils::readByteData(fp, _albumDataSize, _albumData) )
        {
          _albumDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for ALBUM DATA failed.");
        _albumDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  UdtaAlbumAtom::~UdtaAlbumAtom

DESCRIPTION
  Author atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaAlbumAtom::~UdtaAlbumAtom()
{
  if(_albumData)
    QTV_Free(_albumData);
}

/* ======================================================================
FUNCTION:
  UdtaAlbumAtom::getUdtaAlbumDataSize

DESCRIPTION:
  returns auth data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of auth data.
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaAlbumAtom::getUdtaAlbumDataSize()
{
  return _albumDataSize;
}

/* ======================================================================
FUNCTION:
  UdtaAlbumAtom::getUdtaAlbumData

DESCRIPTION:
  copies the auth data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaAlbumAtom::getUdtaAlbumData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_albumDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _albumDataSize-dwOffset);
    memcpy(pBuf, _albumData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}

/* ======================================================================
FUNCTION 
  UdtaYrrcAtom::UdtaYrrcAtom

DESCRIPTION
  YRCC atom constructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaYrrcAtom::UdtaYrrcAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _yrrcData = NULL;
  _yrrcDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag to get link data size */
    _yrrcDataSize = Atom::getSize() - 12;
    if(_yrrcDataSize)
    {
      _yrrcData = (uint8*)QTV_Malloc(_yrrcDataSize); /* free in destructure */
      if(_yrrcData)
      {
        if ( !AtomUtils::readByteData(fp, _yrrcDataSize, _yrrcData) )
        {
          _yrrcDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for YRRC DATA failed.");
        _yrrcDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }	
}

/* ======================================================================
FUNCTION 
  UdtaYrrcAtom::~UdtaYrrcAtom

DESCRIPTION
  YRCC atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaYrrcAtom::~UdtaYrrcAtom()
{  
  if(_yrrcData)
    QTV_Free(_yrrcData);
}


/* ======================================================================
FUNCTION:
  UdtaYrrcAtom::getUdtaYrrcDataSize

DESCRIPTION:
  returns YRCC data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of auth data.
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaYrrcAtom::getUdtaYrrcDataSize()
{
  return _yrrcDataSize;
}


/* ======================================================================
FUNCTION:
  UdtaYrrcAtom::getUdtaYrrcData

DESCRIPTION:
  copies the YRCC data.

INPUT/OUTPUT PARAMETERS:
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint16 UdtaYrrcAtom::getUdtaYrrcData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_yrrcDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _yrrcDataSize-dwOffset);
    memcpy(pBuf, _yrrcData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}

/* ======================================================================
FUNCTION 
  UdtaTitlAtom::UdtaTitlAtom

DESCRIPTION
  Title atom constructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaTitlAtom::UdtaTitlAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _titlData = NULL;
  _titlDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag to get link data size */
    _titlDataSize = Atom::getSize() - 12;
    if(_titlDataSize)
    {
      _titlData = (uint8*)QTV_Malloc(_titlDataSize); /* free in destructure */
      if(_titlData)
      {
        if ( !AtomUtils::readByteData(fp, _titlDataSize, _titlData) )
        {
          _titlDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for TITL DATA failed.");
        _titlDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  UdtaTitlAtom::~UdtaTitlAtom

DESCRIPTION
  Title atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaTitlAtom::~UdtaTitlAtom()
{
  if(_titlData)
    QTV_Free(_titlData);
}

/* ======================================================================
FUNCTION:
  UdtaTitlAtom::getUdtaTitlDataSize

DESCRIPTION:
  returns titl data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of titl data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaTitlAtom::getUdtaTitlDataSize()
{
  return _titlDataSize;
}

/* ======================================================================
FUNCTION:
  UdtaTitlAtom::getUdtaTitlData

DESCRIPTION:
  copies the titl data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaTitlAtom::getUdtaTitlData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_titlDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _titlDataSize-dwOffset);
    memcpy(pBuf, _titlData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}


/* ======================================================================
FUNCTION 
  UdtaDscpAtom::UdtaDscpAtom

DESCRIPTION
  Description atom constructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaDscpAtom::UdtaDscpAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _dscpData = NULL;
  _dscpDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag to get link data size */
    _dscpDataSize = Atom::getSize() - 12;
    if(_dscpDataSize)
    {
      _dscpData = (uint8*)QTV_Malloc(_dscpDataSize); /* free in destructure */
      if(_dscpData)
      {
        if ( !AtomUtils::readByteData(fp, _dscpDataSize, _dscpData) )
        {
          _dscpDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for TITL DATA failed.");
        _dscpDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  UdtaDscpAtom::~UdtaDscpAtom

DESCRIPTION
  Description atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaDscpAtom::~UdtaDscpAtom()
{
  if(_dscpData)
    QTV_Free(_dscpData);
}

/* ======================================================================
FUNCTION:
  UdtaDscpAtom::getUdtaDscpDataSize

DESCRIPTION:
  returns dscp data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of dscp data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaDscpAtom::getUdtaDscpDataSize()
{
  return _dscpDataSize;
}

/* ======================================================================
FUNCTION:
  UdtaDscpAtom::getUdtaDscpData

DESCRIPTION:
  copies the dscp data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaDscpAtom::getUdtaDscpData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_dscpDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _dscpDataSize-dwOffset);
    memcpy(pBuf, _dscpData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}
/* ======================================================================
FUNCTION 
  UdtaRtngAtom::UdtaRtngAtom

DESCRIPTION
  Rating atom constructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaRtngAtom::UdtaRtngAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _rtngData = NULL;
  _rtngDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag ,4bytes of Rating Entity,4 bytes Rating Criteria to get link data size */
    _rtngDataSize = Atom::getSize() - 20;
    if(_rtngDataSize)
    {
      _rtngData = (uint8*)QTV_Malloc(_rtngDataSize); /* free in destructure */
      if(_rtngData )
      {
        if ( !AtomUtils::readByteData(fp, _rtngDataSize, _rtngData) )
        {
          _rtngDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for RTNG DATA failed.");
        _rtngDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  UdtaRtngAtom::~UdtaRtngAtom

DESCRIPTION
  Rating atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaRtngAtom::~UdtaRtngAtom()
{
  if(_rtngData)
    QTV_Free(_rtngData);
}

/* ======================================================================
FUNCTION:
  UdtaRtngAtom::getUdtaRtngDataSize

DESCRIPTION:
  returns rtng data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of rtng data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaRtngAtom::getUdtaRtngDataSize()
{
  return _rtngDataSize;
}

/* ======================================================================
FUNCTION:
  UdtaRtngAtom::getUdtaRtngData

DESCRIPTION:
  copies the rtng data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaRtngAtom::getUdtaRtngData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_rtngDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _rtngDataSize-dwOffset);
    memcpy(pBuf, _rtngData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}
/* ======================================================================
FUNCTION 
  UdtaGnreAtom::UdtaGnreAtom

DESCRIPTION
  Genreatom constructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaGnreAtom::UdtaGnreAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _gnreData = NULL;
  _gnreDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag to get link data size */
    _gnreDataSize = Atom::getSize() - 12;
    if(_gnreDataSize)
    {
      _gnreData = (uint8*)QTV_Malloc(_gnreDataSize); /* free in destructure */
      if(_gnreData )
      {
        if ( !AtomUtils::readByteData(fp, _gnreDataSize, _gnreData) )
        {
          _gnreDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for GNRE DATA failed.");
        _gnreDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  UdtaGnreAtom::~UdtaGnreAtom

DESCRIPTION
  Genre atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaGnreAtom::~UdtaGnreAtom()
{
  if(_gnreData)
    QTV_Free(_gnreData);
}

/* ======================================================================
FUNCTION:
  UdtaGnreAtom::getUdtaGnreDataSize

DESCRIPTION:
  returns gnre data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of gnre data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaGnreAtom::getUdtaGnreDataSize()
{
  return _gnreDataSize;
}

/* ======================================================================
FUNCTION:
  UdtaGnreAtom::getUdtaGnreData

DESCRIPTION:
  copies the Gnre data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaGnreAtom::getUdtaGnreData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_gnreDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _gnreDataSize-dwOffset);
    memcpy(pBuf, _gnreData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}
/* ======================================================================
FUNCTION 
  UdtaPerfAtom::UdtaPerfAtom

DESCRIPTION
  Perf atom constructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaPerfAtom::UdtaPerfAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _perfData = NULL;
  _perfDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag to get link data size */
    _perfDataSize = Atom::getSize() - 12;
    if(_perfDataSize)
    {
      _perfData = (uint8*)QTV_Malloc(_perfDataSize); /* free in destructure */
      if(_perfData )
      {
        if ( !AtomUtils::readByteData(fp, _perfDataSize, _perfData) )
        {
          _perfDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for PERF DATA failed.");
        _perfDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  UdtaPerfAtom::~UdtaPerfAtom

DESCRIPTION
  Performance atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaPerfAtom::~UdtaPerfAtom()
{
  if(_perfData)
    QTV_Free(_perfData);
}

/* ======================================================================
FUNCTION:
  UdtaPerfAtom::getUdtaPerfDataSize

DESCRIPTION:
  returns perf data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of perf data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaPerfAtom::getUdtaPerfDataSize()
{
  return _perfDataSize;
}

/* ======================================================================
FUNCTION:
  UdtaPerfAtom::getUdtaPerfData

DESCRIPTION:
  copies the Perf data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaPerfAtom::getUdtaPerfData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_perfDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _perfDataSize-dwOffset);
    memcpy(pBuf, _perfData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}
/* ======================================================================
FUNCTION 
  UdtaClsfAtom::UdtaClsfAtom

DESCRIPTION
  Classification atom constructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaClsfAtom::UdtaClsfAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _clsfData = NULL;
  _clsfDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag ,4bytes of classification Entity,
	2bytes of classification table to get link data size */
    _clsfDataSize = Atom::getSize() - 18;
    if(_clsfDataSize)
    {
      _clsfData = (uint8*)QTV_Malloc(_clsfDataSize); /* free in destructure */
      if(_clsfData )
      {
        if ( !AtomUtils::readByteData(fp, _clsfDataSize, _clsfData) )
        {
          _clsfDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for CLSF DATA failed.");
        _clsfDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  UdtaClsfAtom::~UdtaClsfAtom

DESCRIPTION
  Classification atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaClsfAtom::~UdtaClsfAtom()
{
  if(_clsfData)
    QTV_Free(_clsfData);
}

/* ======================================================================
FUNCTION:
  UdtaClsfAtom::getUdtaClsfDataSize

DESCRIPTION:
  returns clsf data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of clsf data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaClsfAtom::getUdtaClsfDataSize()
{
  return _clsfDataSize;
}

/* ======================================================================
FUNCTION:
  UdtaClsfAtom::getUdtaClsfData

DESCRIPTION:
  copies the Classification data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaClsfAtom::getUdtaClsfData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_clsfDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _clsfDataSize-dwOffset);
    memcpy(pBuf, _clsfData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}
/* ======================================================================
FUNCTION 
  UdtaKywdAtom::UdtaKywdAtom

DESCRIPTION
  Keyword atom constructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaKywdAtom::UdtaKywdAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _kywdData = NULL;
  _kywdDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag to get link data size */
    _kywdDataSize = Atom::getSize() - 12;
    if(_kywdDataSize)
    {
      _kywdData = (uint8*)QTV_Malloc(_kywdDataSize); /* free in destructure */
      if(_kywdData )
      {
        if ( !AtomUtils::readByteData(fp, _kywdDataSize, _kywdData) )
        {
          _kywdDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for KYWD DATA failed.");
        _kywdDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  UdtaKywdAtom::~UdtaKywdAtom

DESCRIPTION
  Keyword atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaKywdAtom::~UdtaKywdAtom()
{
  if(_kywdData)
    QTV_Free(_kywdData);
}

/* ======================================================================
FUNCTION:
  UdtaKywdAtom::getUdtaKywdDataSize

DESCRIPTION:
  returns kywd data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of kywd data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaKywdAtom::getUdtaKywdDataSize()
{
  return _kywdDataSize;
}

/* ======================================================================
FUNCTION:
  UdtaKywdAtom::getUdtaKywdData

DESCRIPTION:
  copies the Keyword data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaKywdAtom::getUdtaKywdData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_kywdDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _kywdDataSize-dwOffset);
    memcpy(pBuf, _kywdData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}
/* ======================================================================
FUNCTION 
  UdtaLociAtom::UdtaLociAtom

DESCRIPTION
  Location atom constructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaLociAtom::UdtaLociAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _lociData = NULL;
  _lociDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag to get link data size */
    _lociDataSize = Atom::getSize() - 12;
    if(_lociDataSize)
    {
      _lociData = (uint8*)QTV_Malloc(_lociDataSize); /* free in destructure */
      if(_lociData )
      {
        if ( !AtomUtils::readByteData(fp, _lociDataSize, _lociData) )
        {
          _lociDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for LOCI DATA failed.");
        _lociDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  UdtaLociAtom::~UdtaLociAtom

DESCRIPTION
  Location atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaLociAtom::~UdtaLociAtom()
{
  if(_lociData)
    QTV_Free(_lociData);
}

/* ======================================================================
FUNCTION:
  UdtaLociAtom::getUdtaLociDataSize

DESCRIPTION:
  returns loci data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of loci data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaLociAtom::getUdtaLociDataSize()
{
  return _lociDataSize;
}

/* ======================================================================
FUNCTION:
  UdtaLociAtom::getUdtaLociData

DESCRIPTION:
  copies the Location data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaLociAtom::getUdtaLociData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_lociDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _lociDataSize-dwOffset);
    memcpy(pBuf, _lociData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}

/* ======================================================================
FUNCTION 
  UdtaMetaAtom::UdtaMetaAtom

DESCRIPTION
  Meta atom constructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaMetaAtom::UdtaMetaAtom(OSCL_FILE *fp)
: FullAtom(fp)
{
  _metaData = NULL;
  _metaDataSize = 0;

  if ( _success )
  {
    if( getVersion() != 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_CONTENT_VERSION_FAILED;
      return;
    }

    /* skip 4 bytes size, 4 bytes type and 4 bytes ver/flag to get link data size */
    _metaDataSize = Atom::getSize() - 12;
    if(_metaDataSize)
    {
      _metaData = (uint8*)QTV_Malloc(_metaDataSize); /* free in destructure */
      if(_metaData )
      {
        if ( !AtomUtils::readByteData(fp, _metaDataSize, _metaData) )
        {
          _metaDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for META DATA failed.");
        _metaDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_USER_DATA_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  UdtaMetaAtom::~UdtaMetaAtom

DESCRIPTION
  Location atom destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
UdtaMetaAtom::~UdtaMetaAtom()
{
  if(_metaData)
    QTV_Free(_metaData);
}

/* ======================================================================
FUNCTION:
  UdtaLociAtom::getUdtaLociDataSize

DESCRIPTION:
  returns loci data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of loci data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaMetaAtom::getUdtaMetaDataSize()
{
  return _metaDataSize;
}

/* ======================================================================
FUNCTION:
  UdtaLociAtom::getUdtaMetaData

DESCRIPTION:
  copies the Location data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 UdtaMetaAtom::getUdtaMetaData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_metaDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _metaDataSize-dwOffset);
    memcpy(pBuf, _metaData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}
