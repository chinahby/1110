#ifndef __DCMDUserDataAtom_H__
#define __DCMDUserDataAtom_H__
/* =======================================================================
                              dcmdUserDataAtom.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/inc/dcmduserdataatom.h#9 $
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
#include "atom.h"
#include "atomdefs.h"
#include "isucceedfail.h"
#include "atomutils.h"

/* =======================================================================
**                        Class Declarations
** ======================================================================= */

/* ======================================================================
CLASS 
  DcmdDrmAtom

DESCRIPTION
  DcmdDrmAtom atom: dcmd.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class DcmdDrmAtom : public Atom
{

public:
    DcmdDrmAtom(OSCL_FILE *fp); // Constructor
    virtual ~DcmdDrmAtom();

    uint32 getDcmdDataSize();
    uint32 getDcmdData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _dcmdDataSize;
    uint8 * _dcmdData;
};

#endif //__DCMDUserDataAtom_H__
