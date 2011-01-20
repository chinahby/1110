#ifndef IPARAMETERS_H
#define IPARAMETERS_H

/*============================================================================
FILE:	IParameters.h

SERVICES:  

DESCRIPTION:
   This file defines the IParameters interface, providing a generic mechanism 
   to set and get data.
   
PUBLIC CLASSES:
   IParameters
   IParametersRO - the read-only version
   
INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2004-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

//==========================================================================
//   Includes
//==========================================================================

#include "AEEQueryInterface.h"

#define AEEIID_IPARAMETERSRO    	0x0102d13c
#define AEEIID_IPARAMETERS    	0x0102d13b

//==========================================================================
//   API definition
//========================================================================== 
// Define IParametersRO derived from IQueryInterface
typedef struct IParametersRO IParametersRO;

#define INHERIT_IParametersRO(iname) \
   INHERIT_IQueryInterface(iname); \
   int      (*GetParam)(iname* po, int id, void *pParam, unsigned *puParamLen)

AEEINTERFACE(IParametersRO)
{
   INHERIT_IParametersRO(IParametersRO);
};


static __inline uint32 IPARAMETERSRO_AddRef(IParametersRO *me)
{
   return AEEGETPVTBL(me,IParametersRO)->AddRef(me);
}

static __inline uint32 IPARAMETERSRO_Release(IParametersRO *me)
{
   return AEEGETPVTBL(me,IParametersRO)->Release(me);
}

static __inline uint32 IPARAMETERSRO_QueryInterface(IParametersRO *me,
                                                    AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,IParametersRO)->QueryInterface(me, cls, ppo);
}

static __inline int IPARAMETERSRO_GetParam(IParametersRO *me,
                                           int id, void *pParam, unsigned *puParamLen)
{
   return AEEGETPVTBL(me,IParametersRO)->GetParam(me, id, pParam, puParamLen);
}

/* pParam - place to return value of the parameter
   puParam - on in, the size of buffer pointed to by pParam. on out the size filled in
 */


typedef struct IParameters IParameters;

#define INHERIT_IParameters(iname) \
   INHERIT_IParametersRO(iname); \
   int      (*SetParam)(iname* po, int id, const void *pParam, unsigned uParamLen)

AEEINTERFACE(IParameters)
{
   INHERIT_IParameters(IParameters);
};


static __inline uint32 IPARAMETERS_AddRef(IParameters *me)
{
   return AEEGETPVTBL(me,IParameters)->AddRef(me);
}

static __inline uint32 IPARAMETERS_Release(IParameters *me)
{
   return AEEGETPVTBL(me,IParameters)->Release(me);
}

static __inline uint32 IPARAMETERS_QueryInterface(IParameters *me,
                                                  AEECLSID cls, void** ppo)
{
   return AEEGETPVTBL(me,IParameters)->QueryInterface(me, cls, ppo);
}

static __inline int IPARAMETERS_GetParam(IParameters *me,
                                         int id, void *pParam,
                                         unsigned *puParamLen)
{
   return AEEGETPVTBL(me,IParameters)->GetParam(me, id, pParam, puParamLen);
}

static __inline int IPARAMETERS_SetParam(IParameters *me,
                                         int id, const void *pParam,
                                         unsigned uParamLen)
{
   return AEEGETPVTBL(me,IParameters)->SetParam(me, id, pParam, uParamLen);
}

#endif /* IPARAMETERS_H */

/*===========================================================================
  INTERFACE DOCUMENTATION
=============================================================================

Interface Name: IParameters

Description:
   This interface defines a generic mechanism for setting and getting data
   using type, length value. 

   The IParametersRO interface is the read-only version - it simply disallows
   the SetParams function.
      
=============================================================================
IParametersRO 
   The IParametersRO interface is the read-only version of IParameters. 
   It simply disallows the SetParams function.
==============================================================================
Function: IPARAMETERS_AddRef()

Description:
   This function increments the reference count of the IParameters Interface
   object.

Dependencies:
   None

Prototype:
    uint32 IPARAMETERS_AddRef (IParameters *me)

Parameters:
   me:             [IN]     Pointer to IParameters object

Return Value:
   Returns the updated reference count.

Comments:
   None

Side Effects:
  None

Version:
   Introduced BREW SDK 3.1.3

See Also:
   IPARAMETERS_Release ()

=============================================================================

Function: IPARAMETERS_GetParam()

Description:
   This function allows the caller to get data.

Dependencies:
   None

Prototype:
   int IPARAMETERS_GetParam ( IParameters *me,
                              int          id, 
                              void        *pParam,
                              unsigned    *puParamLen )


Parameters:
   me:             [IN]     Pointer to IParameters object
   id:             [IN]     Indicates what the type of the following pointer to
                           void is.
   pParam:         [OUT]    The data returned.
   puParamLen:     [OUT]    The length of the above data.                           

Return Value:
   SUCCESS: if task is successful
   EFAILED: otherwise

Comments:
   None

Side Effects:
  None

Version:
   Introduced BREW SDK 3.1.3

See Also:
   IPARAMETERS_SetParam ()

=============================================================================

Function: IPARAMETERS_SetParam()

Description:
   This function allows the caller to set data.

Dependencies:
   None

Prototype:
   int IPARAMETERS_SetParam ( IParameters *me,
                              int          id, 
                              const void  *pParam,
                              unsigned     uParamLen )


Parameters:
   me:             [IN]     Pointer to IParameters object
   id:             [IN]     Indicates what the type of the following pointer to
                           void is.
   pParam:         [IN]     The data to be set.
   uParamLen:      [IN]     The length of the above data.                           

Return Value:
   SUCCESS: if OK,    
   EFAILED: otherwise.

Comments:
   None

Side Effects:
  None

Version:
   Introduced BREW SDK 3.1.3

See Also:
   IPARAMETERS_GetParam ()

=============================================================================

IPARAMETERS_Release()

Description:
   This function decrements the reference count for the IParameters Interface
   object and does appropriate cleanup if the reference count reaches 0 (zero).

Prototype:
   uint32 IPARAMETERS_Release (IParameters *me)

Parameters:
   me:             [IN]     Pointer to IParameter object

Return Value:
   Returns the updated reference count.

Comments:
   None

Side Effects:
   None

Version:
   Introduced BREW SDK 3.1.3

See Also:
    IPARAMETERS_AddRef ()

=============================================================================*/

// ---- end of IParameters.h -----
