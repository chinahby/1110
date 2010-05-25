/*=============================================================================

FILE: oembn.c

SERVICES: OEM Interface to sec_api bignumber capabilities (ModExp) 

GENERAL DESCRIPTION:
  
PUBLIC CLASSES AND STATIC FUNCTIONS:
OEMBN_Create
OEMBN_Free
OEMBN_ModExpEx


INITIALIZATION AND SEQUENCING REQUIREMENTS:
No additional module-level initialization or sequencing requirements.

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/


//-----------------------------------------------------------------------------
//  Includes
//-----------------------------------------------------------------------------

#include "AEEStdLib.h"
#include "AEE_OEM.h"
#include "AEE_OEMDispatch.h"

#include "OEMBN.h"


//-----------------------------------------------------------------------------
//  Global Function Implementations
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
//
//   OEMBN_Supported
//
//  Public method. See header file for detailed description. 
//
//  
//-----------------------------------------------------------------------------
boolean OEMBN_Supported (void)
{
   return FALSE;
}

//-----------------------------------------------------------------------------
//
//   OEMBN_ModExpEx
//
//  Public method. See header file for detailed description. 
//
//  For reference, modexp param is:
//
//  typedef struct
//  {
//    uint8*  base_ptr;
//    uint16  base_size;
//    uint8*  modulus_ptr;
//    uint16  modulus_size;
//    uint8*  exponent_ptr;
//    uint16  exponent_size;
//  } secapi_pkx_modexp_param_type;
//  
//-----------------------------------------------------------------------------
void OEMBN_ModExp ( const uint8       *pbMsgBuf,
                          uint16       uMsgLen,
                    const uint8       *pbExponentBuf,
                          uint16       uExponentLen,                     
                    const uint8       *pbModulusBuf,
                          uint16       uModulusLen,
                          uint8       *pResultBuf,
                          uint32      *puResultLen,
                          int         *pnError,
                          AEECallback *pcb )
{
   ACONTEXT           *pac;
   *pnError = EUNSUPPORTED;
   if (pcb) { 
      pac = AEE_GetAppContext();
      AEE_ResumeCallback((AEECallback *)pcb, pac);
   }
}

