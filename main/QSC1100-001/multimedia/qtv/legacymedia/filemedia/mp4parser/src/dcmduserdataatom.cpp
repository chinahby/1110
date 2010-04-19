/* =======================================================================
                              dcmdUserDataAtom.cpp
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/src/dcmduserdataatom.cpp#9 $
$DateTime: 2008/05/07 15:14:25 $
$Change: 655407 $


========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "target.h"             /* Target specific definitions             */
#include "comdef.h"             /* Common definitions                      */

#include "dcmdUserDataAtom.h"

/* ======================================================================
FUNCTION 
  DcmdDrmAtom::DcmdDrmAtom

DESCRIPTION
  DcmdDrmAtom atom: dcmd constructor. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
DcmdDrmAtom::DcmdDrmAtom(OSCL_FILE *fp)
: Atom(fp)
{
  _dcmdData = NULL;
  _dcmdDataSize = 0;

  if ( _success )
  {

    /* skip 4 bytes size, 4 bytes type to get ftyp data size */
    _dcmdDataSize = Atom::getSize() - 8;
    if(_dcmdDataSize)
    {
      _dcmdData = (uint8*)QTV_Malloc(_dcmdDataSize); /* free in destructure */
      if(_dcmdData)
      {
        if ( !AtomUtils::readByteData(fp, _dcmdDataSize, _dcmdData) )
        {
          _dcmdDataSize = 0;
          _success = false;
          _fileErrorCode = (int32)READ_DCMD_DRM_ATOM_FAILED;
          return;
        }
      }
      else
      {
        QTV_MSG_PRIO(QTVDIAG_GENERAL, QTVDIAG_PRIO_ERROR, "Memory allocation for DCMD DATA failed.");
        _dcmdDataSize = 0;
        _success = false;
        _fileErrorCode = (int32)MEMORY_ALLOCATION_FAILED;
        return;
      }
    }
  }
  else
  {
    _fileErrorCode = (int32)READ_DCMD_DRM_ATOM_FAILED;
    return;
  }
}

/* ======================================================================
FUNCTION 
  DcmdDrmAtom::~DcmdDrmAtom

DESCRIPTION
  DcmdDrmAtom atom: dcmd destructor.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
DcmdDrmAtom::~DcmdDrmAtom()
{
  if(_dcmdData)
    QTV_Free(_dcmdData);
}

/* ======================================================================
FUNCTION:
  DcmdDrmAtom::getDcmdDataSize

DESCRIPTION:
  returns dcmd data size.

INPUT/OUTPUT PARAMETERS:
  None.

RETURN VALUE:
  size of dcmd data
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 DcmdDrmAtom::getDcmdDataSize()
{
  return _dcmdDataSize;
}

/* ======================================================================
FUNCTION:
  DcmdDrmAtom::getDcmdData

DESCRIPTION:
  copies the dcmd data into supplied buffer.

INPUT/OUTPUT PARAMETERS:
  pBuf      - INPUT/OUTPUT  - buffer for data to be copied.
  dwSize    - INPUT         - size of buffer and max data to be copied.
  dwOffset  - INPUT         - 0 based offset of data to be copied from start

RETURN VALUE:
  actual bytes copied into buffer
          
SIDE EFFECTS:
  None.
======================================================================*/
uint32 DcmdDrmAtom::getDcmdData(uint8* pBuf, uint32 dwSize, uint32 dwOffset)
{
  if(_dcmdDataSize)
  {
    uint32 dwDataToBeCopied = MIN(dwSize, _dcmdDataSize-dwOffset);
    memcpy(pBuf, _dcmdData+dwOffset, dwDataToBeCopied);
    return dwDataToBeCopied;
  }
  return 0;
}

