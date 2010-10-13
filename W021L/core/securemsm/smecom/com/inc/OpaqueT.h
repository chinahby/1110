#ifndef OPAQUET_H
#define OPAQUET_H

/*===========================================================================
FILE:  OpaqueT.h

DESCRIPTION
  The OpaqueT provides easy accessor methods to store data/len type of 
  information

EXTERNALIZED FUNCTIONS : 


  Copyright (c) 2005 by QUALCOMM, Incorporated.  All Rights Reserved.  
===========================================================================*/

/*===========================================================================

                            EDIT HISTORY FOR FILE
                            
  $Header: //source/qcom/qct/core/securemsm/smecom/main/latest/com/inc/OpaqueT.h#1 $
  $DateTime: 2007/07/17 11:00:30 $
  $Author: jmiera $ 


when		  who     what, where, why
--------	---     ----------------------------------------------------------
06/06/06	ssm	    Use UxMalloc
05/16/06	rv	    Fix for target build
04/30/06	ssm	    Created
===========================================================================*/


/*===========================================================================
                           
                                INCLUDE FILES

============================================================================*/

#include "IxErrno.h"
#include "comdef.h"
#include "err.h"
#include "stdlib.h"
#include "UxCommon.h"
#include "secdiag.h"

#ifdef __cplusplus
/*===========================================================================

                              CLASS DEFINITION

===========================================================================*/

class OpaqueT
{   
public:       

  uint8*  pData;
  uint32  dwLen;

  /* Default Constructor */
  OpaqueT() 
  {
    pData = NULL;
    dwLen = 0;
  }

  /* Constructor */
  OpaqueT(uint8* pInData, uint32 InLength):pData(NULL), dwLen(0)
  {
    /* Sanity Check */
    if (pInData == NULL || InLength == 0)
    {
      return;
    }

    pData = (uint8*) UxMalloc(InLength + 1, UX_ALLOC_LOCAL);
    if (pData == NULL)
    {
      SEC_DIAG_MSG( SECDIAG_IXCOMMON, MSG_SEC_ERROR, "No memory" );
      return;
    }
    memcpy(pData, pInData, InLength);
    pData[InLength] = '\0';
    dwLen = InLength;
  }

  /* Destructor */
  ~OpaqueT()
  {
    if (pData != NULL)
    {
      UxFree(pData);
      pData = NULL;
      dwLen = 0;
    }
  }

  // Copy constructor
  OpaqueT( const OpaqueT & Id ):pData(NULL), dwLen(0)
  {
    /* Sanity Check */
    if (Id.pData == NULL || Id.dwLen == 0)
    {
      return;
    }

    pData = (uint8*) UxMalloc(Id.dwLen + 1, UX_ALLOC_LOCAL);
    if (pData == NULL)
    {
      SEC_DIAG_MSG( SECDIAG_IXCOMMON, MSG_SEC_ERROR, "No memory" );
      return;
    }
    memcpy(pData, Id.pData, Id.dwLen);
    pData[Id.dwLen] = '\0';
    dwLen = Id.dwLen;
  }

  // Overload = operator
  OpaqueT & operator = (const OpaqueT& Id)
  {
    dwLen = 0;
    do
    {
      if (pData) 
      {
        UxFree(pData);
        pData = NULL;
      }
      /* Sanity Check */
      if (Id.pData == NULL || Id.dwLen == 0)
      {
        break;
      }

      pData = (uint8*) UxMalloc(Id.dwLen + 1, UX_ALLOC_LOCAL);
      if (pData == NULL)
      {
        SEC_DIAG_MSG( SECDIAG_IXCOMMON, MSG_SEC_ERROR, "No memory" );
        break;
      }
      memcpy(pData, Id.pData, Id.dwLen);
      pData[Id.dwLen] = '\0';
      dwLen = Id.dwLen;
    } while(0);

    return *this;
  }

  /* Method to set params */
  IxErrnoType Set(uint8* pInData, uint32 InLength)
  {
    IxErrnoType Errno = E_SUCCESS;
    
    /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

    do 
    {
      dwLen = 0;

      if (pData) 
      {
        UxFree(pData);
        pData = NULL;
      }
      /* Sanity Check */
      if (pInData == NULL || InLength == 0)
      {
        Errno = E_INVALID_ARG;
        break;
      }

      pData = (uint8*) UxMalloc(InLength + 1, UX_ALLOC_LOCAL);
      if (pData == NULL)
      {
        SEC_DIAG_MSG( SECDIAG_IXCOMMON, MSG_SEC_ERROR, "No memory" );
        Errno = E_NO_MEMORY;
        break;
      }
      memcpy(pData, pInData, InLength);
      pData[InLength] = '\0';
      dwLen = InLength;
    } while(0);
    return Errno;
  }
};

#endif // #ifdef _cplusplus
#endif


