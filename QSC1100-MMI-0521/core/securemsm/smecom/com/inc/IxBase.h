#ifndef IXBASE_H
#define IXBASE_H

/*===========================================================================
*
*                               I X  B A S E
*             
*                            H E A D E R  F I L E
*
* FILE:  IxBase.h
*
* DESCRIPTION :
*  Base class from which all the Ix classes are derived.
*  Contains just pure virtual functions for AddRef and Release.
*
* Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/IxBase.h#1 $
  $DateTime: 2007/07/17 11:00:30 $
  $Author: jmiera $


when       who      what, where, why
--------   ---      ------------------------------------
10/21/05    gr      Created
      
===========================================================================*/
#include "comdef.h"

class IxBase {

public:

  virtual unsigned long AddRef( void ) = 0;

  virtual unsigned long Release( void ) = 0;

  /* Gets the data representing the object */
  virtual void GetState( uint8** stateDataPtr, uint32* stateDataLenPtr ) = 0;

  /* Sets the data for the object */
  virtual void SetState( uint8* stateDataPtr, uint32 stateDataLen ) = 0;
};

#endif /* IXBASE_H */

