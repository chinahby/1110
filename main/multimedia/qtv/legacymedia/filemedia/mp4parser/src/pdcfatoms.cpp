/* =======================================================================
                              pdcfatoms.cpp
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
  
Copyright 2004 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */

/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/src/pdcfatoms.cpp#10 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "target.h"             /* Target specific definitions             */

#ifdef FEATURE_QTV_PDCF

#include "pdcfatoms.h"

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
  PdcfAtom::PdcfAtom

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
PdcfAtom::PdcfAtom()
{
  m_OffsetinAtomArray = 0;  
}

/* ======================================================================
FUNCTION:
PdcfAtom::isAtomPresent

DESCRIPTION:
Returns the data-type size.

INPUT/OUTPUT PARAMETERS:
dType - data-type.

RETURN VALUE:
true - Atom present
false - atom not present

SIDE EFFECTS:
None.
======================================================================*/
bool PdcfAtom::isAtomPresent(QtvPlayer::DataT dType, uint32 track_id )
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "PdcfAtom::isAtomPresent");
  for(int i=0; i<MPEG4_MAX_PDCF_ATOMS; i++)
  {
    if(( m_AtomData[i].track_id == track_id ) 
       && (( m_AtomData[i].type == dType ) 
       || ( m_AtomData[i].offset_type == dType )))
    {
      return true;
    }
  }  
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "PdcfAtom::isAtomPresent Not found the specified PDCF atom");
  return false;
}

/* ======================================================================
FUNCTION:
PdcfAtom::getAtomSize

DESCRIPTION:
Returns the PDCF data-type size.

INPUT/OUTPUT PARAMETERS:
dType - data-type.

RETURN VALUE:
Size of the atom
0 if no atom is available

SIDE EFFECTS:
None.
======================================================================*/
uint32 PdcfAtom::getAtomSize(QtvPlayer::DataT dType, uint32 track_id )
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "PdcfAtom::getAtomSize");
  for(int i=0; i<MPEG4_MAX_PDCF_ATOMS; i++)
  {
    if(( m_AtomData[i].track_id == track_id ) && ( m_AtomData[i].type == dType ))
    {
      return m_AtomData[i].size;
    }
  }  
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "PdcfAtom::getAtomSize Not found for the specified PDCF atom");
  return 0;
}

/* ======================================================================
FUNCTION:
PdcfAtom::getAtomOffset

DESCRIPTION:
Returns the data-type offset.

INPUT/OUTPUT PARAMETERS:
dType - data-type.
track_id - track id

RETURN VALUE:
offset of the atom. Here we are returning the offset of the atom including the header.
0 if no atom is available

SIDE EFFECTS:
None.
======================================================================*/
uint32 PdcfAtom::getAtomOffset(QtvPlayer::DataT dType,uint8 *pBuf, uint32 size, uint32 offset)
{
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "PdcfAtom::getAtomOffset");
  (void)size;
  for(int i=0; i<MPEG4_MAX_PDCF_ATOMS; i++)
  {
    if(( m_AtomData[i].track_id == offset ) && ( m_AtomData[i].offset_type == dType ))
    {      
      if(pBuf)
      {        
        *pBuf++ = (uint8)((m_AtomData[i].offset - ATOM_HEADER_SIZE) & 0x000000FF);
        *pBuf++ = (uint8)(((m_AtomData[i].offset - ATOM_HEADER_SIZE) & 0x0000FF00) >> 8L);
        *pBuf++ = (uint8)(((m_AtomData[i].offset - ATOM_HEADER_SIZE) & 0x00FF0000) >> 16L);
        *pBuf   = (uint8)(((m_AtomData[i].offset - ATOM_HEADER_SIZE) & 0xFF000000) >> 24L);
        return(sizeof(uint32));
      }
    }      
  }
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "PdcfAtom::getAtomOffset Not found for the specified PDCF atom");
  return 0;
}

/* ======================================================================
FUNCTION:
PdcfAtom::getAtomData

DESCRIPTION:
copies the specified PDCF data-type data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
fp - file pointer
dType - data-type.
pBuf  - INPUT/OUTPUT  - buffer for data to be copied.
size  - INPUT         - size of buffer and max data to be copied.
offset- INPUT         - using this as a track id.

RETURN VALUE:
actual bytes copied into buffer
0 if no data is available

SIDE EFFECTS:
None.
======================================================================*/
uint32 PdcfAtom::getAtomData(OSCL_FILE *fp, QtvPlayer::DataT dType,uint8 *pBuf, uint32 size, uint32 offset)
{  
  uint32 file_offset = 0;
  uint32 atom_Size = 0;
  QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_MED, "PdcfAtom::getAtomData");
  for(int i=0; i<MPEG4_MAX_PDCF_ATOMS; i++)
  {
    if(( m_AtomData[i].track_id == offset ) && ( m_AtomData[i].type == dType ))
    {
      file_offset = m_AtomData[i].offset;
      atom_Size = m_AtomData[i].size;
      break;
    }
  }
  if(file_offset)
  {
    if( fp )
    {
      uint32 copiedSize = 0;	  
      if(atom_Size <= size)
      {
        OSCL_FileSeek(fp, file_offset, SEEK_SET);        
        copiedSize = MIN(size, atom_Size);
        if ( OSCL_FileRead (pBuf, copiedSize, 1, fp) == 0 )
        {
          QTV_MSG (MSG_LEGACY_ERROR, "Failed to readFile");
          copiedSize = 0;
        }
      }
      return copiedSize;    
    }   
  }
  return 0;
}

/* ======================================================================
FUNCTION:
PdcfAtom::saveAtom

DESCRIPTION:
copies the specified data-type data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
dType - atom-type.
dType_offset - atom_offset_type
track_id  - track id
size  - INPUT         - size of the atom
offset- INPUT         - offset of the atom

RETURN VALUE:
true - saved the atom
false - there is no space in the array to store this then may be we have to increase the 
        static array size.
SIDE EFFECTS:
None.
======================================================================*/
bool PdcfAtom::saveAtom(QtvPlayer::DataT dType,QtvPlayer::DataT dType_offset, uint32 track_id, uint32 size, uint32 offset)  
{  
  if(m_OffsetinAtomArray < MPEG4_MAX_PDCF_ATOMS)
  {
    m_AtomData[m_OffsetinAtomArray].type = dType;
    m_AtomData[m_OffsetinAtomArray].offset_type = dType_offset;
    m_AtomData[m_OffsetinAtomArray].track_id = track_id;            
    m_AtomData[m_OffsetinAtomArray].size = size;
    m_AtomData[m_OffsetinAtomArray].offset = offset;
    m_OffsetinAtomArray++;
    return true;
  }
  return false;
}
#endif /* FEATURE_QTV_PDCF */

