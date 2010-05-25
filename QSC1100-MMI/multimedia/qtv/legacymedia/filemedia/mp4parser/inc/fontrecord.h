#ifndef __FONT_RECORD_H__
#define __FONT_RECORD_H__
/* =======================================================================
                              fontrecord.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/inc/fontrecord.h#9 $
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
#include "atomutils.h"

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

/* =======================================================================
**                          Class Declarations
** ======================================================================= */

/* ======================================================================
CLASS 
  FontRecord

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
class FontRecord
{

public:
  FontRecord (OSCL_FILE *fp); // Default constructor
  virtual ~FontRecord (); // Destructor

  uint16 getFontID()
  {
    return _fontID;
  }

  int8   getFontLength()
  {
    return _fontLength;
  }

  uint8* getFontName()
  {
    return _pFontName;
  }

  bool GetFileSuccess()
  {
    return _success;
  }

  int32 GetFileError()
  {
    return _fileErrorCode;
  }

private:
  uint16 _fontID;
  int8   _fontLength;
  uint8  *_pFontName;

  bool  _success;
  int32 _fileErrorCode;
};

#endif /* __FONT_RECORD_H__ */
