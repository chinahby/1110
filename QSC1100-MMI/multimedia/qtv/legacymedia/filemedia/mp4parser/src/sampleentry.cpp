/* =======================================================================
                              sampleentry.cpp
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/src/sampleentry.cpp#9 $
$DateTime: 2008/05/07 15:14:25 $
$Change: 655407 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "sampleentry.h"
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
  SampleEntry::SampleEntry

DESCRIPTION
  Stream-in Constructor

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
SampleEntry::SampleEntry(OSCL_FILE *fp) 
: Atom(fp)
{ 
  for(int i=0;i< 6;i++)
  {
    _reserved[i] = 0;
  }

  _dataReferenceIndex = 0;
  
  if ( _success )
  {

    if ( !AtomUtils::read8read8(fp, _reserved[0], _reserved[1]) )
      _success = false;
    if ( !AtomUtils::read8read8(fp, _reserved[2], _reserved[3]) )
      _success = false;
    if ( !AtomUtils::read8read8(fp, _reserved[4], _reserved[5]) )
      _success = false;

    if ( !AtomUtils::read16(fp, _dataReferenceIndex) )
      _success = false;

    if ( !_success )
      _fileErrorCode = (int32)READ_SAMPLE_ENTRY_FAILED;
  }
  else
  {
    _fileErrorCode = (int32)READ_SAMPLE_ENTRY_FAILED;
  }
}

/* ======================================================================
FUNCTION 
  SampleEntry::~SampleEntry

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
SampleEntry::~SampleEntry()
{
  // Empty
}

/* ======================================================================
FUNCTION 
  GenericSampleEntry::GenericSampleEntry

DESCRIPTION
  Stream-in Constructor

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
GenericSampleEntry::GenericSampleEntry(OSCL_FILE *fp) 
: Atom(fp)
{
  if ( _success )
  {
  }
  else
  {
    _fileErrorCode = (int32)READ_SAMPLE_ENTRY_FAILED;
  }
}

/* ======================================================================
FUNCTION 
  GenericSampleEntry::~GenericSampleEntry

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
GenericSampleEntry::~GenericSampleEntry()
{
  // Empty
}

