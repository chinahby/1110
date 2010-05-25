/* =======================================================================
                              atom.cpp
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/src/atom.cpp#10 $
$DateTime: 2008/07/09 23:19:05 $
$Change: 699016 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "atom.h"
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

/* ======================================================================
FUNCTION 
  Atom::Atom

DESCRIPTION
  Normal constructor from fullatom constructor

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
Atom::Atom(uint32 type)
{
    _type = type;
    _pparent = NULL; 
    _size=0;/*size is intialized */
    _success=true; /*success is intialized */
    _fileErrorCode = (int32)EVERYTHING_FINE; /*_filerrorcode is intialized*/	
    _offsetInFile = 0;	
}


/* ======================================================================
FUNCTION
  Atom::Atom

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
Atom::Atom(OSCL_FILE *fp)
{
  int32 atomendPointer; // file pointer at the end of the current atom
 
  _success = true;
  _fileErrorCode = (int32)EVERYTHING_FINE;
   _pparent = NULL; /*_pparent is intialized */
   _size=0; /*size is intialized */
   _type=0;
  // SAVE THE CURRENT FILE POSITION
  _offsetInFile = OSCL_FileTell(fp);

  if ( _offsetInFile == -1 )
  {
    _success = false;
    _fileErrorCode = (int32)DEFAULT_ERROR;
  }

  if( _success && !AtomUtils::read32read32(fp, _size, _type))
  {
    _success = false;
  }

  int32 size = (int32)_size;

  if (size <= 0)
  {
    _success = false;
    _fileErrorCode = (int32)ZERO_OR_NEGATIVE_ATOM_SIZE;
  }

  if (_success)
  {
    /* atom size must atleast be DEFAULT_ATOM_SIZE */
    if (size >= DEFAULT_ATOM_SIZE )
    {
      /*
       * Seek to the end of the atom, as size + tag field is included in
       * the atom Size, and as they have already been read, we subtract
       * (DEFAULT_ATOM_SIZE), from _size.
       */
      atomendPointer = _offsetInFile + (size - DEFAULT_ATOM_SIZE);

      /* In case the fp is greater than file size, _size is corrupt */
            if (atomendPointer > AtomUtils::fileSize)
               {
                _success = false;
             }
    }
    else
    {
      _success = false;
    }
  }
}

#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
/* ======================================================================
FUNCTION
  Atom::Atom

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
Atom::Atom(uint8 *&buf)
{
  _success = true;
  _fileErrorCode = (int32)EVERYTHING_FINE;
   _pparent = NULL;/*_pparent is intialized*/
  _offsetInFile = 0;
    if(!AtomUtils::read32read32(buf, _size, _type))
      {
        _success = false;
     }

  int32 size = (int32)_size;

  if (size <= 0)
  {
    _success = false;
    _fileErrorCode = (int32)ZERO_OR_NEGATIVE_ATOM_SIZE;
  }

  if (_success)
  {
    /* atom size must atleast be DEFAULT_ATOM_SIZE */
    if (size < DEFAULT_ATOM_SIZE )
        {
      _success = false;
    }
  }
}
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

/* ======================================================================
FUNCTION
  Atom::~Atom

DESCRIPTION
  Destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
Atom::~Atom()
{
    // Empty
}

/* ======================================================================
FUNCTION
  Atom::getDefaultSize

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
Atom::getDefaultSize() const
{
    return DEFAULT_ATOM_SIZE; 
}
