/* =======================================================================
                               textsamplemodifiers.cpp
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/src/textsamplemodifiers.cpp#11 $
$DateTime: 2010/06/09 10:21:06 $
$Change: 1328021 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "textsamplemodifiers.h"
#include "atomutils.h"
#include "atomdefs.h"

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT

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
  TextStyleBox:: TextStyleBox

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
TextStyleBox:: TextStyleBox (uint8  *buf) : Atom (buf)
{
  _pparent         = NULL;

  _pStyleRecordVec = NULL;
  _entryCount      = 0;

  if ( _success && AtomUtils::read16( buf, _entryCount) )
  {
    _pStyleRecordVec = QTV_New( ZArray<StyleRecord*> );

    if (_pStyleRecordVec)
    {
      uint16 numStyleRecordsAdded;
      for ( numStyleRecordsAdded = 0; 
            numStyleRecordsAdded < _entryCount; 
            numStyleRecordsAdded++ )
      {
        StyleRecord *pStyleRecord = QTV_New_Args( StyleRecord , (buf) );

        if (!pStyleRecord)
        {
          break; // fall through
        }
        else if ( !pStyleRecord->FileSuccess() )
        {
          _success = false;
          _fileErrorCode = pStyleRecord->GetFileError();
          QTV_Delete(pStyleRecord); 
	   	  
          return;
        }
        else
        {
          (*_pStyleRecordVec)+=(pStyleRecord);
        }
      }

      // No error only if all StyleRecords were added.
      if (numStyleRecordsAdded == _entryCount)
        return;
    }
  }

  _success = false;
  _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
  return;
}

/* ======================================================================
FUNCTION 
  TextStyleBox::~TextStyleBox

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
TextStyleBox::~TextStyleBox()
{
  if ( _pStyleRecordVec != NULL )
  {
    for ( int32 i=0; i<_pStyleRecordVec->GetLength(); i++ )  
    {
      QTV_Delete( (*_pStyleRecordVec)[i] ); 
    }
    QTV_Delete( _pStyleRecordVec );
  }
}

/* ======================================================================
FUNCTION 
  TextHighlightBox::TextHighlightBox

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
TextHighlightBox::TextHighlightBox (uint8  *buf) : Atom(buf)
{
  _startCharOffset = 0;
  _endCharOffset   = 0;

  if ( _success )
  {
    if ( !AtomUtils::read16( buf, _startCharOffset) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }

    if ( !AtomUtils::read16( buf, _endCharOffset) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }
  }
  else
  {
    _success = false;
    _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  TextHilightColorBox::TextHilightColorBox

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
TextHilightColorBox::TextHilightColorBox (uint8  *buf) : Atom(buf)
{
  _pHighlightColorRGBA = NULL;

  if ( _success )
  {
    _pHighlightColorRGBA = (uint8*)QTV_Malloc(4);
    if ( !_pHighlightColorRGBA )
    {
      _success = false;
      _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
      return;
    }

    if ( !AtomUtils::readByteData(buf, 4, _pHighlightColorRGBA) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }
  }
  else
  {
    _success = false;
    _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  TextHilightColorBox::~TextHilightColorBox

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
TextHilightColorBox::~TextHilightColorBox()
{
  if ( _pHighlightColorRGBA != NULL )
  {
    QTV_Free(_pHighlightColorRGBA);
  }
}

/* ======================================================================
FUNCTION 
  TextKaraokeBox::TextKaraokeBox

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
TextKaraokeBox::TextKaraokeBox (uint8  *buf) : Atom(buf)
{
  _pHighLightEndTimeVec = NULL;
  _pStartCharOffsetVec  = NULL;

  if ( _success )
  {
    if ( !AtomUtils::read32( buf, _highLightStartTime) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }

    if ( !AtomUtils::read16( buf, _entryCount) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }

    uint32 endTime;
    uint16 startOffset;

    if ( _entryCount > 0 )
    {
      _pHighLightEndTimeVec = (uint32*)QTV_Malloc(_entryCount);
      _pStartCharOffsetVec = (uint16*)QTV_Malloc(_entryCount);
      if ( !_pHighLightEndTimeVec || !_pStartCharOffsetVec)
      {
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }

      for ( uint16 i = 0; i < _entryCount; i++ )
      {
        if ( !AtomUtils::read32(buf, endTime) )
        {
          _success = false;
          _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
          return;
        }

        if ( !AtomUtils::read16(buf, startOffset) )
        {
          _success = false;
          _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
          return;
        }

        _pHighLightEndTimeVec[i] = endTime;
        _pStartCharOffsetVec[i]  = startOffset;
      }
    }
  }
  else
  {
    _success = false;
    _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  TextKaraokeBox::~TextKaraokeBox

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
TextKaraokeBox::~TextKaraokeBox()
{
  if ( _pHighLightEndTimeVec != NULL )
  {
    QTV_Free(_pHighLightEndTimeVec);
  }

  if ( _pStartCharOffsetVec != NULL )
  {
    QTV_Free(_pStartCharOffsetVec);
  }
}

/* ======================================================================
FUNCTION 
  TextScrollDelay::TextScrollDelay

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
TextScrollDelay::TextScrollDelay (uint8  *buf) : Atom(buf)
{
  if ( _success )
  {
    if ( !AtomUtils::read32( buf, _scrollDelay) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }
  }
  else
  {
    _success = false;
    _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  TextHyperTextBox::TextHyperTextBox

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
TextHyperTextBox::TextHyperTextBox (uint8  *buf) : Atom(buf)
{
  if ( _success )
  {
    if ( !AtomUtils::read16( buf, _startCharOffset) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }

    if ( !AtomUtils::read16( buf, _endCharOffset) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }

    if ( !AtomUtils::read8( buf, _urlLength) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }

    _pURL = (uint8*)QTV_Malloc(_urlLength);

    if ( NULL == _pURL || !AtomUtils::readByteData(buf, _urlLength, _pURL) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }

    if ( !AtomUtils::read8( buf, _altLength) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }

    _pAltString = (uint8*)QTV_Malloc(_altLength);

    if ( NULL == _pAltString || !AtomUtils::readByteData(buf, _altLength, _pAltString) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }
  }
  else
  {
    _success = false;
    _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  TextHyperTextBox::~TextHyperTextBox

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
TextHyperTextBox::~TextHyperTextBox()
{
  if ( _pURL != NULL )
  {
    QTV_Free(_pURL);
  }

  if ( _pAltString != NULL )
  {
    QTV_Free(_pAltString);
  }
}

/* ======================================================================
FUNCTION 
  TextBoxBox::TextBoxBox

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
TextBoxBox::TextBoxBox (uint8  *buf) : Atom(buf)
{
  _pBoxRecord = NULL;

  if ( _success )
  {
    _pBoxRecord = QTV_New_Args( BoxRecord, (buf) );

    if(!_pBoxRecord)
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }
    if ( !_pBoxRecord->FileSuccess() )
    {
      _success = false;
      _fileErrorCode = _pBoxRecord->GetFileError();
      return;
    }
  }
  else
  {
    _success = false;
    _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  TextBoxBox::~TextBoxBox

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
TextBoxBox::~TextBoxBox()
{
  if ( _pBoxRecord != NULL )
  {
    QTV_Delete( _pBoxRecord );
  }
}

/* ======================================================================
FUNCTION 
  BlinkBox::BlinkBox

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
BlinkBox::BlinkBox (uint8  *buf) : Atom(buf)
{
  if ( _success )
  {
    if ( !AtomUtils::read16( buf, _startCharOffset) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }

    if ( !AtomUtils::read16( buf, _endCharOffset) )
    {
      _success = false;
      _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
      return;
    }
  }
  else
  {
    _success = false;
    _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  TextSampleModifiers::parseTextModifiers

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
TextSampleModifiers* 
TextSampleModifiers::parseTextModifiers(uint8* buf, uint32 size)
{
  TextSampleModifiers *pTextSampleModifiers = 
    QTV_New_Args( TextSampleModifiers, (buf, size) );

  if (pTextSampleModifiers && !pTextSampleModifiers->FileSuccess() )
  {
    QTV_Delete(pTextSampleModifiers);
    return NULL;
  }

  return(pTextSampleModifiers);
}

/* ======================================================================
FUNCTION 
  TextSampleModifiers:: TextSampleModifiers

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
TextSampleModifiers:: TextSampleModifiers (uint8  *buf, uint32 size)
{
  _pTextStyleBoxVec       = NULL;
  _pTextHighlightBoxVec   = NULL;
  _pTextKaraokeBoxVec     = NULL;
  _pTextHyperTextBoxVec   = NULL;
  _pBlinkBoxVec           = NULL;
  _pTextHilightColorBox   = NULL;
  _pTextScrollDelay       = NULL;
  _pTextBoxBox            = NULL;

  _success = true;

  _pTextStyleBoxVec     = QTV_New( ZArray<TextStyleBox*> );
  _pTextHighlightBoxVec = QTV_New( ZArray<TextHighlightBox*> );
  _pTextKaraokeBoxVec   = QTV_New( ZArray<TextKaraokeBox*> );
  _pTextHyperTextBoxVec = QTV_New( ZArray<TextHyperTextBox*> );
  _pBlinkBoxVec         = QTV_New( ZArray<BlinkBox*> );
  _fileErrorCode        = ( int32 )EVERYTHING_FINE;

  if ( _success )
  {
    uint32 count    = 0;
    uint32 atomType = AtomUtils::getNextAtomType(buf);

    while ( ((atomType == TEXT_STYLE_BOX) ||
             (atomType == TEXT_HIGHLIGHT_BOX) ||
             (atomType == TEXT_HILIGHT_COLOR_BOX) ||
             (atomType == TEXT_KARAOKE_BOX)||
             (atomType == TEXT_SCROLL_DELAY_BOX) ||
             (atomType == TEXT_HYPER_TEXT_BOX) ||
             (atomType == TEXT_OVER_RIDE_BOX) ||
             (atomType == TEXT_BLINK_BOX)) &&
            (count < size) )
    {
      if ( atomType == TEXT_STYLE_BOX )
      {
        TextStyleBox *pStyleBox = QTV_New_Args( TextStyleBox, (buf) );

        if ( !pStyleBox )
        {
          _success = false;
          _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
          return;
        }

        if ( !pStyleBox->FileSuccess() )
        {
          _success = false;
          _fileErrorCode = pStyleBox->GetFileError();
          QTV_Delete( pStyleBox );
          return;
        }

        if(_pTextStyleBoxVec)
        {
          (*_pTextStyleBoxVec) += pStyleBox;
        }
        count += pStyleBox->getSize();
        buf   += pStyleBox->getSize();
      }
      else if ( atomType == TEXT_HIGHLIGHT_BOX )
      {
        TextHighlightBox *pHighlightBox = QTV_New_Args( TextHighlightBox, (buf) );

        if ( !pHighlightBox )
        {
          _success = false;
          _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
          return;
        }

        if ( !pHighlightBox->FileSuccess() )
        {
          _success = false;
          _fileErrorCode = pHighlightBox->GetFileError();
          QTV_Delete( pHighlightBox ); 
          return;
        }

        if(_pTextHighlightBoxVec)
        {
          (*_pTextHighlightBoxVec) += pHighlightBox;
        }
        count += pHighlightBox->getSize();
        buf   += pHighlightBox->getSize();
      }
      else if ( atomType == TEXT_HILIGHT_COLOR_BOX )
      {
        if ( _pTextHilightColorBox == NULL )
        {
          _pTextHilightColorBox = QTV_New_Args( TextHilightColorBox, (buf) );

          if(!_pTextHilightColorBox)
          {
            _success = false;
            _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
            return;
          }
          if ( !_pTextHilightColorBox->FileSuccess() )
          {
            _success = false;
            _fileErrorCode = _pTextHilightColorBox->GetFileError();
            return;
          }

          count += _pTextHilightColorBox->getSize();
          buf   += _pTextHilightColorBox->getSize();
        }
        else
        {
          _success = false;
          _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
          return;
        }
      }
      else if ( atomType == TEXT_KARAOKE_BOX )
      {
        TextKaraokeBox *pKaraokeBox = QTV_New_Args( TextKaraokeBox, (buf) );
        if(!pKaraokeBox)
        {
          _success = false;          
          _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
          return;
        } 
        if ( !pKaraokeBox->FileSuccess() )
        {
          _success = false;
          _fileErrorCode = pKaraokeBox->GetFileError();
		  QTV_Delete(pKaraokeBox); 
          return;
        }
        if(_pTextKaraokeBoxVec)
        {
          (*_pTextKaraokeBoxVec) += pKaraokeBox;
        }
        count += pKaraokeBox->getSize();
        buf   += pKaraokeBox->getSize();
      }
      else if ( atomType == TEXT_SCROLL_DELAY_BOX )
      {
        if ( _pTextScrollDelay == NULL )
        {
          _pTextScrollDelay = QTV_New_Args( TextScrollDelay, (buf) );
          if(!_pTextScrollDelay)
          {
            _success = false;
            _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
            return;
          }
          if ( !_pTextScrollDelay->FileSuccess() )
          {
            _success = false;
            _fileErrorCode = _pTextScrollDelay->GetFileError();
            return;
          }
          count += _pTextScrollDelay->getSize();
          buf   += _pTextScrollDelay->getSize();
        }
        else
        {
          _success = false;
          _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
          return;
        }
      }
      else if ( atomType == TEXT_HYPER_TEXT_BOX )
      {
        TextHyperTextBox *pHyperTextBox = QTV_New_Args( TextHyperTextBox, (buf) );
        if(!pHyperTextBox)
        {
          _success = false;
          _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
          return;
        }

        if ( !pHyperTextBox->FileSuccess() )
        {
          _success = false;
          _fileErrorCode = pHyperTextBox->GetFileError();
		  QTV_Delete(pHyperTextBox); 
          return;
        }

        if(_pTextHyperTextBoxVec)
        {
          (*_pTextHyperTextBoxVec) += pHyperTextBox;
        }
        count += pHyperTextBox->getSize();
        buf   += pHyperTextBox->getSize();
      }
      else if ( atomType == TEXT_OVER_RIDE_BOX )
      {
        if ( _pTextBoxBox == NULL )
        {
          _pTextBoxBox = QTV_New_Args( TextBoxBox, (buf) );
          if(!_pTextBoxBox)
          {
            _success = false;            
            _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
            return;
          }

          if ( !_pTextBoxBox->FileSuccess() )
          {
            _success = false;
            _fileErrorCode = _pTextBoxBox->GetFileError();
            return;
          }
          count += _pTextBoxBox->getSize();
          buf   += _pTextBoxBox->getSize();
        }
        else
        {
          _success = false;
          _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
          return;
        }
      }
      else if ( atomType == TEXT_BLINK_BOX )
      {
        BlinkBox *pBlinkBox = QTV_New_Args( BlinkBox, (buf) );
        if(!pBlinkBox)
        {
          _success = false;            
          _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
          return;
        }

        if ( !pBlinkBox->FileSuccess() )
        {
          _success = false;
          _fileErrorCode = pBlinkBox->GetFileError();
		  QTV_Delete(pBlinkBox); 
          return;
        }

        if(_pBlinkBoxVec)
        {
          (*_pBlinkBoxVec) += pBlinkBox;
        }
        count += pBlinkBox->getSize();
        buf   += pBlinkBox->getSize();
      }
      atomType = AtomUtils::getNextAtomType(buf);
    }
  }
  else
  {
    _success = false;
    _fileErrorCode = (int32)READ_TEXT_SAMPLE_MODIFIERS_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  TextSampleModifiers::~TextSampleModifiers

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
TextSampleModifiers::~TextSampleModifiers()
{
  if ( _pTextStyleBoxVec != NULL )
  {
    for ( int32 i=0; i<_pTextStyleBoxVec->GetLength(); i++ ) 
    {
      QTV_Delete( (*_pTextStyleBoxVec)[i] ); 
    }
    QTV_Delete( _pTextStyleBoxVec );
  }
  if ( _pTextHighlightBoxVec != NULL )
  {
    for ( int32 i=0; i<_pTextHighlightBoxVec->GetLength(); i++ ) 
    {
      QTV_Delete( (*_pTextHighlightBoxVec)[i] ); 
    }
    QTV_Delete( _pTextHighlightBoxVec );
  }
  if ( _pTextKaraokeBoxVec != NULL )
  {
    for ( int32 i=0; i<_pTextKaraokeBoxVec->GetLength(); i++ ) 
    {
      QTV_Delete( (*_pTextKaraokeBoxVec)[i] ); 
    }
    QTV_Delete( _pTextKaraokeBoxVec );
  }
  if ( _pTextHyperTextBoxVec != NULL )
  {
    for ( int32 i=0; i<_pTextHyperTextBoxVec->GetLength(); i++ ) 
    {
      QTV_Delete( (*_pTextHyperTextBoxVec)[i] ); 
    }
    QTV_Delete( _pTextHyperTextBoxVec );
  }
  if ( _pBlinkBoxVec != NULL )
  {
    for ( int32 i=0; i<_pBlinkBoxVec->GetLength(); i++ ) 
    {
      QTV_Delete( (*_pBlinkBoxVec)[i] ); 
    }
    QTV_Delete( _pBlinkBoxVec );
  }

  if ( _pTextHilightColorBox != NULL )
  {
    QTV_Delete( _pTextHilightColorBox );
  }
  if ( _pTextScrollDelay != NULL )
  {
    QTV_Delete( _pTextScrollDelay );
  }
  if ( _pTextBoxBox != NULL )
  {
    QTV_Delete( _pTextBoxBox );
  }
}

#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
