#ifndef __FONT_TABLE_ATOM_H__
#define __FONT_TABLE_ATOM_H__
/* =======================================================================
                              fonttableatom.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/inc/fonttableatom.h#9 $
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
#include "fontrecord.h"

#include "ztl.h"
#include "atom.h"
#include "atomdefs.h"

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
  FontTableAtom

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
class FontTableAtom : public Atom
{

public:
  FontTableAtom (OSCL_FILE *fp); // Default constructor
  virtual ~FontTableAtom (); // Destructor

  uint16  getFontListSize()
  {
    return _entryCount;
  }

  FontRecord*  getFontRecordAt(int32 index);

private:
  uint16 _entryCount;
  ZArray<FontRecord *> *_pFontRecordArray;
};


#endif /* __FONT_TABLE_ATOM_H__ */
