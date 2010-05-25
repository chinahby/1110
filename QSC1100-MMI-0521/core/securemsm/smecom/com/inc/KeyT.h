#ifndef KEYT_H
#define KEYT_H

/*===========================================================================
FILE:  KeyT.h

DESCRIPTION
  The OpaqueT provides easy accessor methods to store data/len type of 
  information

EXTERNALIZED FUNCTIONS : 


  Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/KeyT.h#1 $
  $DateTime: 2007/07/17 11:00:30 $
  $Author: jmiera $ 


when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/06/06   ssm     Use UxMalloc
05/16/06   ssm     Fix for target build
04/30/06   ssm     Created
===========================================================================*/


/*===========================================================================
                           
                                INCLUDE FILES

============================================================================*/


#include "IxErrno.h"
#include "comdef.h"
#include "err.h"
#include "OpaqueT.h"
#include "stdlib.h"
#include "secdiag.h"

#ifdef __cplusplus
/*===========================================================================

                              CLASS DEFINITION

===========================================================================*/

class KeyT : public OpaqueT
{
  public:

  /* Default Constructor */
  KeyT() 
  {
    pData = NULL;
    dwLen = 0;
  }

  /* Constructor */
  KeyT(uint8* pInData, uint32 InLength) : OpaqueT(pInData, InLength) {}
  
  /* Destructor */
  ~KeyT() {}
  
  /* Copy Constructor */
  KeyT( const KeyT & InKey ) : OpaqueT(InKey) {}

  /* Overload the assignment operator */
  KeyT & operator = (const KeyT& InKey) 
  { 
    OpaqueT::operator = (InKey);
    return *this;
  }

  /* Set Method */
  IxErrnoType Set(uint8* pInData, uint32 InLength)
  {
    OpaqueT::Set(pInData, InLength);
    return E_SUCCESS;
  }
};
#endif // __splusplsu
#endif // KEYT_H
