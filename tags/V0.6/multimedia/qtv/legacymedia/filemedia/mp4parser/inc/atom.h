#ifndef __Atom_H__
#define __Atom_H__
/* =======================================================================
                              atom.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/inc/atom.h#9 $
$DateTime: 2008/05/07 15:14:25 $
$Change: 655407 $


========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "oscl_file_io.h"
#include "parentable.h"
#include "renderable.h"
#include "isucceedfail.h"


#define DEFAULT_ATOM_SIZE 8 //need to change when they use various size field

class Atom : public Parentable, public Renderable, public ISucceedFail
{

public:
  Atom(uint32 type);
  Atom(OSCL_FILE *fp); // file pointer constructor
#ifdef FEATURE_MP4_3GPP_TIMED_TEXT
  Atom(uint8* &buf);
#endif /* FEATURE_MP4_3GPP_TIMED_TEXT */
  virtual ~Atom();

  // Member get methods
  virtual uint32 getSize() const { return _size;}
  uint32 getType() const { return _type;}
  virtual uint32 getOffsetInFile() const { return _offsetInFile;}
#ifndef __CC_ARM
  inline 
#endif
  virtual uint32 getDefaultSize() const;//tag+size field length

protected:
  uint32 _size; // 4 (32bits)
  uint32 _type; // 4 (32bits)
  int32 _offsetInFile; // 4 (32bits)

private:


};

#endif /* __Atom_H__ */
