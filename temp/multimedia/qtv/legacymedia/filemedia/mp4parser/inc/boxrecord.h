#ifndef __BOX_RECORD_H__
#define __BOX_RECORD_H__
/* =======================================================================
                              boxrecord.h
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

$Header: //source/qcom/qct/multimedia/qtv/legacymedia/filemedia/mp4parser/main/latest/inc/boxrecord.h#9 $
$DateTime: 2008/05/07 15:14:25 $
$Change: 655407 $


========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "atomutils.h"

/* =======================================================================
**                        Class Declarations
** ======================================================================= */
class BoxRecord
{

public:
  BoxRecord (OSCL_FILE *fp); // Default constructor
  BoxRecord (uint8 *&buf); // Default constructor
  virtual ~BoxRecord ()
  {
  }; // Destructor

  int16 getBoxTop()
  {
    return _top;
  }

  int16 getBoxLeft()
  {
    return _left;
  }

  int16 getBoxBottom()
  {
    return _bottom;
  }

  int16 getBoxRight()
  {
    return _right;
  }

  bool  FileSuccess()
  {
    return _success;
  }

  int32 GetFileError()
  {
    return _fileErrorCode;
  }

private:
  int16 _top;
  int16 _left;
  int16 _bottom;
  int16 _right;

  bool  _success;
  int32 _fileErrorCode;
};


#endif /* __BOX_RECORD_H__ */
