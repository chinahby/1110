#ifndef __FullAtom_H__
#define __FullAtom_H__
/* =======================================================================
                              fullatom.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/inc/fullatom.h#9 $
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
#include "atom.h"

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
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
// tag - 4; extended tag - 16; version - 1; flags - 3
#define DEFAULT_UUID_ATOM_SIZE 28
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

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
  FullAtom

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
class FullAtom : public Atom
{

public:
  FullAtom(uint32 type, uint8 version, uint32 flags); // Constructor
  FullAtom(OSCL_FILE *fp); // stream-in constructor
  virtual ~FullAtom();

  // No "set" methods as they get set directly in the constructor
  uint8  getVersion() const
  {
    return _version;
  }
  uint32 getFlags() const
  {
    return _flags;
  }
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  uint32 getUUIDType() const
  {
    return _uuidType;
  }
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */

#ifndef __CC_ARM
  inline 
#endif
  virtual uint32 getDefaultSize() const;



private:
  uint8 _version; // 1 (8bits)
  uint32 _flags; // 3 (24bits) -- Will need to crop when writing to stream
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  uint32 _uuidType; // 32 bit Kddi UUID Type
  uint32 _uuidExt1; // 32 bits ext 1
  uint32 _uuidExt2; // 32 bits ext 2
  uint32 _uuidExt3; // 32 bits ext 3
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
};


#endif
