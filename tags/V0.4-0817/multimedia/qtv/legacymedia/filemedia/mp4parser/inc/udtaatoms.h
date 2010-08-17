#ifndef __udtaAtoms_H__
#define __udtaAtoms_H__
/* =======================================================================
                              udtaAtoms.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/inc/udtaatoms.h#10 $
$DateTime: 2008/08/04 05:56:16 $
$Change: 716103 $


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
#include "fullatom.h"
#include "atomdefs.h"
#include "isucceedfail.h"
#include "atomutils.h"
#include "QtvPlayer.h"

#ifdef FEATURE_QTV_SKT_MOD_MIDI
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
  UdtaMidiAtom

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
class UdtaMidiAtom : public FullAtom
{

public:
    UdtaMidiAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaMidiAtom();

    uint32 getUdtaMidiDataSize();
    uint32 getUdtaMidiData(uint8* pBuf, uint32 dwSize, uint32 dwOffset);
    
private:
    uint32  _midiDataSize;
    uint8 * _midiData;
};

/* ======================================================================
CLASS 
  UdtaLinkAtom

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
class UdtaLinkAtom : public FullAtom
{

public:
    UdtaLinkAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaLinkAtom();

    uint32 getUdtaLinkDataSize();
    uint32 getUdtaLinkData(uint8* pBuf, uint32 dwSize);
    
private:
    uint32  _linkDataSize;
    uint8 * _linkData;
};

#endif /* FEATURE_QTV_SKT_MOD_MIDI */

/* ======================================================================
CLASS 
  FtypAtom

DESCRIPTION
  File Type atom.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class FtypAtom : public Atom
{

public:
    FtypAtom(OSCL_FILE *fp); // Constructor
    virtual ~FtypAtom();

    uint32 getFtypDataSize();
    uint32 getFtypData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _ftypDataSize;
    uint8 * _ftypData;
};


/* ======================================================================
CLASS 
  DrefAtom

DESCRIPTION
  Data Reference atom.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class DrefAtom : public FullAtom
{

public:
    DrefAtom(OSCL_FILE *fp); // Constructor
    virtual ~DrefAtom();

    uint32 getDrefDataSize();
    uint32 getDrefData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _drefDataSize;
    uint8 * _drefData;
};


/* ======================================================================
CLASS 
  UdtaCprtAtom

DESCRIPTION
  Copyright atom.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class UdtaCprtAtom : public FullAtom
{

public:
    UdtaCprtAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaCprtAtom();

    uint32 getUdtaCprtDataSize();
    uint32 getUdtaCprtData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _cprtDataSize;
    uint8 * _cprtData;
};


/* ======================================================================
CLASS 
  UdtaAuthAtom

DESCRIPTION
  Author atom.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class UdtaAuthAtom : public FullAtom
{

public:
    UdtaAuthAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaAuthAtom();

    uint32 getUdtaAuthDataSize();
    uint32 getUdtaAuthData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _authDataSize;
    uint8 * _authData;
};


/* ======================================================================
CLASS 
  UdtaTitlAtom

DESCRIPTION
  Title atom.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class UdtaTitlAtom : public FullAtom
{

public:
    UdtaTitlAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaTitlAtom();

    uint32 getUdtaTitlDataSize();
    uint32 getUdtaTitlData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _titlDataSize;
    uint8 * _titlData;
};


/* ======================================================================
CLASS 
  UdtaAlbumAtom

DESCRIPTION
  Album atom.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class UdtaAlbumAtom : public FullAtom
{

public:
    UdtaAlbumAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaAlbumAtom();

    uint32 getUdtaAlbumDataSize();
    uint32 getUdtaAlbumData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _albumDataSize;
    uint8 * _albumData;
};

/* ======================================================================
CLASS 
  UdtaYrrcAtom

DESCRIPTION
  Yrcc atom.

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class UdtaYrrcAtom : public FullAtom
{

public:
    UdtaYrrcAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaYrrcAtom();

    uint32 getUdtaYrrcDataSize();
    uint16 getUdtaYrrcData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _yrrcDataSize;
    uint8 * _yrrcData;
};

/* ======================================================================
CLASS 
  UdtaDscpAtom

DESCRIPTION
  Description atom. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class UdtaDscpAtom : public FullAtom
{

public:
    UdtaDscpAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaDscpAtom();

    uint32 getUdtaDscpDataSize();
    uint32 getUdtaDscpData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _dscpDataSize;
    uint8 * _dscpData;
};

/* ======================================================================
CLASS 
  UdtaRtnAtom

DESCRIPTION
  Rating atom. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class UdtaRtngAtom : public FullAtom
{

public:
    UdtaRtngAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaRtngAtom();

    uint32 getUdtaRtngDataSize();
    uint32 getUdtaRtngData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _rtngDataSize;
    uint8 * _rtngData;
};
/* ======================================================================
CLASS 
  UdtaRtngAtom

DESCRIPTION
  Rating atom. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class UdtaGnreAtom : public FullAtom
{

public:
    UdtaGnreAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaGnreAtom();

    uint32 getUdtaGnreDataSize();
    uint32 getUdtaGnreData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _gnreDataSize;
    uint8 * _gnreData;
};
/* ======================================================================
CLASS 
  UdtaPerfAtom

DESCRIPTION
  Performance atom. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class UdtaPerfAtom : public FullAtom
{

public:
    UdtaPerfAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaPerfAtom();

    uint32 getUdtaPerfDataSize();
    uint32 getUdtaPerfData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _perfDataSize;
    uint8 * _perfData;
};

/* ======================================================================
CLASS 
  UdtaclsfAtom

DESCRIPTION
  Classification atom. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class UdtaClsfAtom : public FullAtom
{

public:
    UdtaClsfAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaClsfAtom();

    uint32 getUdtaClsfDataSize();
    uint32 getUdtaClsfData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _clsfDataSize;
    uint8 * _clsfData;
};

/* ======================================================================
CLASS 
  UdtaKywdAtom

DESCRIPTION
  Keyword atom. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class UdtaKywdAtom : public FullAtom
{

public:
    UdtaKywdAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaKywdAtom();

    uint32 getUdtaKywdDataSize();
    uint32 getUdtaKywdData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _kywdDataSize;
    uint8 * _kywdData;
};

/* ======================================================================
CLASS 
  UdtaLociAtom

DESCRIPTION
  Location atom. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class UdtaLociAtom : public FullAtom
{

public:
    UdtaLociAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaLociAtom();

    uint32 getUdtaLociDataSize();
    uint32 getUdtaLociData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _lociDataSize;
    uint8 * _lociData;
};

/* ======================================================================
CLASS 
  UdtaMetaAtom

DESCRIPTION
  Location atom. 

DEPENDENCIES
  List any dependencies for this function, global variables, state,
  resource availability, etc.

RETURN VALUE
  Enumerate possible return values

SIDE EFFECTS
  Detail any side effects.
  
========================================================================== */
class UdtaMetaAtom : public FullAtom
{

public:
    UdtaMetaAtom(OSCL_FILE *fp); // Constructor
    virtual ~UdtaMetaAtom();

    uint32 getUdtaMetaDataSize();
    uint32 getUdtaMetaData(uint8* pBuf, uint32 dwSize, uint32 offset);
    
private:
    uint32  _metaDataSize;
    uint8 * _metaData;
};
#endif /* __udtaAtoms_H__ */

