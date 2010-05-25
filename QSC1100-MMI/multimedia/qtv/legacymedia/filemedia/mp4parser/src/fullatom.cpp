/* =======================================================================
                              fullatom.cpp
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/src/fullatom.cpp#9 $
$DateTime: 2008/05/07 15:14:25 $
$Change: 655407 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */


/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "fullatom.h"
#include "atomutils.h"
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
#include "atomdefs.h"
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
const uint32 DEFAULT_FULL_ATOM_SIZE = 12; // (8 bytes from Atom + 1 for  
                                          //  version and 3 for flags)

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
  FullAtom::FullAtom

DESCRIPTION
  Constructor

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
FullAtom::FullAtom(uint32 type, uint8 version, uint32 flags) 
: Atom(type)
{
  _version = version;
  _flags = flags;
  #ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    _uuidType=0;
    _uuidExt1=0;
    _uuidExt2=0;
    _uuidExt3=0;
  #endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
}

/* ======================================================================
FUNCTION 
  FullAtom::FullAtom

DESCRIPTION
  Constructor

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
FullAtom::FullAtom(OSCL_FILE *fp)
: Atom(fp)
{
  _version  = 0;
  _flags    = 0;
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  _uuidExt1 = 0;
  _uuidExt2 = 0;
  _uuidExt3 = 0;  
  _uuidType = UNKNOWN_ATOM;
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

  if ( _success )
  {
    // Size and type set in Atom constructor
    uint32 data;

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
    // Read in the UUID extension if present
    if ( getType() == UUID_ATOM )
    {
      if ( !AtomUtils::read32(fp, _uuidType) )
        _success = false;
      if ( !AtomUtils::read32(fp, _uuidExt1) )
        _success = false;
      if ( !AtomUtils::read32(fp, _uuidExt2) )
        _success = false;
      if ( !AtomUtils::read32(fp, _uuidExt3) )
        _success = false;
    }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

    if ( !AtomUtils::read32(fp, data) )
      _success = false;

    _version = (uint8)(data >> 24);
    _flags   = data & 0x00ffffff;
  }
}

/* ======================================================================
FUNCTION 
  FullAtom::~FullAtom

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
FullAtom::~FullAtom()
{
  // Empty
}

/* ======================================================================
FUNCTION 
  FullAtom::getDefaultSize

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
//Note: ARM linker can't find this symbol if it's declared "inline"
#ifndef __CC_ARM
inline 
#endif
uint32
FullAtom::getDefaultSize() const
{
  return DEFAULT_FULL_ATOM_SIZE; 
}


