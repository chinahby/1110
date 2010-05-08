/* =======================================================================
                              fonttableatom.cpp
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/src/fonttableatom.cpp#9 $
$DateTime: 2008/05/07 15:14:25 $
$Change: 655407 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "fonttableatom.h"
#include "atomutils.h"

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

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
/* ======================================================================
FUNCTION 
  FontTableAtom::FontTableAtom

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
FontTableAtom::FontTableAtom (OSCL_FILE *fp) 
: Atom(fp)
{
  _pFontRecordArray = NULL;
  _entryCount = 0;

  if ( _success )
  {
    (void)AtomUtils::read16( fp, _entryCount);

    int16 tmp = (int16)_entryCount;

    if ( tmp < 0 )
    {
      _success = false;
      _fileErrorCode = (int32)READ_FONT_TABLE_ATOM_FAILED;
      return;
    }

    _pFontRecordArray = QTV_New( ZArray <FontRecord *> );

    for ( uint32 i = 0; i  <  _entryCount;  i++ )
    {
      FontRecord *rec = QTV_New_Args( FontRecord, (fp) );

      if ( (rec) && !rec-> GetFileSuccess() )
      {
        QTV_Delete( rec );
        _success = false;
        _fileErrorCode = (int32)READ_FONT_TABLE_ATOM_FAILED;
        return;
      }
      if(_pFontRecordArray)
      {
        (*_pFontRecordArray) += rec;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_FONT_TABLE_ATOM_FAILED;
  }
}

/* ======================================================================
FUNCTION 
  FontTableAtom::getFontRecordAt

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
FontRecord * 
FontTableAtom::getFontRecordAt (int32 index) 
{
  if ( index < (int32)_entryCount )
  {
    return(FontRecord *) (*_pFontRecordArray)[index];
  }
  else
  {
    return NULL;
  }
}

/* ======================================================================
FUNCTION 
  FontTableAtom::~FontTableAtom

DESCRIPTION
  Desctuctor

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
FontTableAtom::~FontTableAtom() 
{
  for ( uint32 i = 0; i < _entryCount; i++ )
  {
	if((*_pFontRecordArray) && (*_pFontRecordArray)[i] != NULL) 
	{
	  QTV_Delete( (*_pFontRecordArray)[i] );
          (*_pFontRecordArray)[i] = NULL; 
	}

  }

  QTV_Delete( (_pFontRecordArray) );
}

#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
