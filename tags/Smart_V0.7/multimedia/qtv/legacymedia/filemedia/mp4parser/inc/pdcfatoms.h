#ifndef __pdcfAtoms_H__
#define __pdcfAtoms_H__
/* =======================================================================
                              pdcfatoms.h
DESCRIPTION
  Meaningful description of the definitions contained in this file.
  Description must specify if the module is portable specific, mobile
  specific, or common to both, and it should alert the reader if the
  module contains any conditional definitions which tailors the module to
  different targets.
  
Copyright 2004 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/inc/pdcfatoms.h#9 $


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
#include "isucceedfail.h"
#include "QtvPlayer.h"

#ifdef FEATURE_QTV_PDCF
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
/* This is to maintain the hash table 
   Right now we have 8 different PDCF atoms
*/
#define MPEG4_MAX_PDCF_ATOMS 8
#define ATOM_HEADER_SIZE 8
/* =======================================================================

========================================================================== */

/* =======================================================================
**                        Class Declarations
** ======================================================================= */

/* ======================================================================
CLASS 
  PdcfAtom

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
class PdcfAtom
{
public:
  PdcfAtom(); // Constructor
  uint32 getAtomSize(QtvPlayer::DataT dType, uint32 track_id);
  uint32 getAtomData(OSCL_FILE *fp, QtvPlayer::DataT dType,uint8 *pBuf, uint32 size, uint32 offset);
  uint32 getAtomOffset(QtvPlayer::DataT dType,uint8 *pBuf, uint32 size, uint32 offset);  
  bool saveAtom(QtvPlayer::DataT dType,QtvPlayer::DataT dType_offset, uint32 track_id, uint32 size, uint32 offset);  
  bool isAtomPresent(QtvPlayer::DataT dType, uint32 track_id );
    
private:
	/* This is to hold the each PDCF atom */
  typedef struct
  {
   uint32 track_id;
   QtvPlayer::DataT type;
   QtvPlayer::DataT offset_type;
   uint32 offset;
   uint32 size;
  }Atom_type;

  /* Array to hold the PDCF atoms */
  Atom_type  m_AtomData[MPEG4_MAX_PDCF_ATOMS];
  /* offset to m_AtomData array */
  uint8 m_OffsetinAtomArray;  
};
#endif /* FEATURE_QTV_PDCF*/

#endif /* __pdcfAtoms_H__ */

