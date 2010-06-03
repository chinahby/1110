#ifndef __TextSampleEntry_H__
#define __TextSampleEntry_H__
/* =======================================================================
                               textsampleentry.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/inc/textsampleentry.h#9 $
$DateTime: 2008/05/07 15:14:25 $
$Change: 655407 $


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
#include "oscl_file_io.h"
#include "sampleentry.h"
#include "fontrecord.h"
#include "boxrecord.h"
#include "stylerecord.h"
#include "fonttableatom.h"

/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

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
#define PVTEXT_MAX_TEXT_COLORS 4
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
  TextSampleEntry

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
class TextSampleEntry : public SampleEntry
{

public:
  TextSampleEntry (OSCL_FILE *fp); // Default constructor
  virtual ~TextSampleEntry (); // Destructor

  uint32 getDisplayFlags()
  {
    return _displayFlags;
  }

  int8 getHorzJustification()
  {
    return _horzJustification;
  }

  int8 getVertJustification()
  {
    return _vertJustification;
  }

  uint8 *getBackgroundColourRGBA ()
  {
    return _pBackgroundRGBA;
  }

  int16 getBoxTop () 
  {
    if ( _pBoxRecord != NULL )
    {
      return(_pBoxRecord->getBoxTop()); 
    }
    else
    {
      return -1;
    }
  }

  int16 getBoxLeft () 
  {
    if ( _pBoxRecord != NULL )
    {
      return(_pBoxRecord->getBoxLeft()); 
    }
    else
    {
      return -1;
    }
  }

  int16 getBoxBottom () 
  {
    if ( _pBoxRecord != NULL )
    {
      return(_pBoxRecord->getBoxBottom()); 
    }
    else
    {
      return -1;
    }
  }

  int16 getBoxRight () 
  {
    if ( _pBoxRecord != NULL )
    {
      return(_pBoxRecord->getBoxRight()); 
    }
    else
    {
      return -1;
    }
  }

  uint16 getStartChar() 
  {
    if ( _pStyleRecord != NULL )
    {
      return _pStyleRecord-> getStartChar(); 
    }
    else
    {
      return 0;
    }
  }

  uint16 getEndChar() 
  {
    if ( _pStyleRecord != NULL )
    {
      return _pStyleRecord-> getEndChar(); 
    }
    else
    {
      return 0;
    }
  }

  uint16 getFontID() 
  {
    if ( _pStyleRecord != NULL )
    {
      return _pStyleRecord-> getFontID(); 
    }
    else
    {
      return 0;
    }
  }

  uint8 getFontStyleFlags() 
  {
    if ( _pStyleRecord != NULL )
    {
      return _pStyleRecord-> getFontStyleFlags(); 
    }
    else
    {
      return 0;
    }
  }

  uint8 getfontSize() 
  {
    if ( _pStyleRecord != NULL )
    {
      return _pStyleRecord->getfontSize(); 
    }
    else
    {
      return 0;
    }
  }

  uint8 *getTextColourRGBA () 
  {
    if ( _pStyleRecord != NULL )
    {
      return _pStyleRecord-> getTextColourRGBA(); 
    }
    else
    {
      return NULL;
    }
  }

  uint16  getFontListSize() 
  {
    if ( _pFontTableAtom != NULL )
    {
      return _pFontTableAtom-> getFontListSize(); 
    }
    else
    {
      return 0;
    }
  }

  FontTableAtom *getFontTable() 
  {
    return _pFontTableAtom;
  }

  FontRecord   *getFontRecordAt(uint32 index) 
  {
    if ( _pFontTableAtom != NULL )
    {
      return _pFontTableAtom->getFontRecordAt(index); 
    }
    else
    {
      return NULL;
    }
  }

private:
  uint32          _displayFlags;
  uint8           _horzJustification;
  uint8           _vertJustification;
  uint8         *_pBackgroundRGBA;
  BoxRecord       *_pBoxRecord;
  StyleRecord   *_pStyleRecord;
  FontTableAtom *_pFontTableAtom;
};


#endif
