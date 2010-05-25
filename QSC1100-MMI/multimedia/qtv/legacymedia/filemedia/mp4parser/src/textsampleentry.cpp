/* =======================================================================
                               textsampleentry.cpp
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/src/textsampleentry.cpp#9 $
$DateTime: 2008/05/07 15:14:25 $
$Change: 655407 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "target.h"             /* Target specific definitions             */

#if defined(FEATURE_MP4_3GPP_TIMED_TEXT)

#include "textsampleentry.h"
#include "atomutils.h"
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
  TextSampleEntry:: TextSampleEntry

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
TextSampleEntry:: TextSampleEntry (OSCL_FILE *fp) : SampleEntry (fp)
{
  _pparent           = NULL;
  _pBackgroundRGBA   = NULL;
  _pBoxRecord        = NULL;
  _pStyleRecord      = NULL;
  _pFontTableAtom    = NULL;
  _displayFlags      = 0;
  _horzJustification = 0;
  _vertJustification = 0;

  if(_success)
  {
    _pBackgroundRGBA = (uint8*)QTV_Malloc(PVTEXT_MAX_TEXT_COLORS);
    if ( !_pBackgroundRGBA )
    {
      _success = false;
      _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
      return;
    }
    (void)AtomUtils::read32( fp, _displayFlags);
    (void)AtomUtils::read8( fp,  _horzJustification);
    (void)AtomUtils::read8( fp,  _vertJustification);
    (void)AtomUtils::readByteData(fp, 4, _pBackgroundRGBA);

    _success = false;

    _pBoxRecord = QTV_New_Args( BoxRecord , (fp) );
    if (!_pBoxRecord)
    {
      _fileErrorCode = (int32)DEFAULT_ERROR;
      return;
    }
    else if (!_pBoxRecord->FileSuccess())
    {
      _fileErrorCode = _pBoxRecord->GetFileError();
      return;
    }

    _pStyleRecord = QTV_New_Args( StyleRecord , (fp) );
    if (!_pStyleRecord)
    {
      _fileErrorCode = (int32)DEFAULT_ERROR;
      return;
    }
    else if (!_pStyleRecord->FileSuccess())
    {
      _fileErrorCode = _pStyleRecord->GetFileError();
      return;
    }

    _pFontTableAtom = QTV_New_Args( FontTableAtom , (fp) );
    if (!_pFontTableAtom)
    {
      _fileErrorCode = (int32)DEFAULT_ERROR;
      return;
    }
    else if (!_pFontTableAtom->FileSuccess())
    {
      _fileErrorCode = _pFontTableAtom->GetFileError();
      return;
    }

    _success = true;
  }
}

/* ======================================================================
FUNCTION 
  TextSampleEntry::~TextSampleEntry

DESCRIPTION
  Destructor
  
DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
TextSampleEntry::~TextSampleEntry()
{
  if (_pBackgroundRGBA != NULL)
  {
    QTV_Free(_pBackgroundRGBA);
  }
  if (_pBoxRecord != NULL)
  {
    QTV_Delete( _pBoxRecord );
  }
  if (_pStyleRecord != NULL)
  {
    QTV_Delete( _pStyleRecord );
  }
  if (_pFontTableAtom != NULL)
  {
    QTV_Delete( _pFontTableAtom ); 
  }
}

#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
