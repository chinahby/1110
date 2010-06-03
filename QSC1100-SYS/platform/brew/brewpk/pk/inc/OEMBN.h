#ifndef OEMBN_DSP_H
#define OEMBN_DSP_H
/*============================================================================
FILE:        OEMBN.h

SERVICES:  BREW DSP Big Number Services

DESCRIPTION:
   This file is contains defines for OEMBN.c

   Right now, big number operations on the DSP are
   limited to ModExp

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/

#include "AEE.h"

//-------------------------------------------------------------------
//      Macros
//-------------------------------------------------------------------

//-------------------------------------------------------------------
//     Type Declarations
//-------------------------------------------------------------------

/*==================================================================

==================================================================*/

//---------------------------------------------------------------------------
//    AEE-OEM Function Declarations
//---------------------------------------------------------------------------

extern boolean OEMBN_Supported (void);

extern void OEMBN_ModExp (const uint8  *pbMsgBuf,
                                uint16  uMsgLen,
                          const uint8  *pbExponentBuf,
                                uint16  uExponentLen,                     
                          const uint8  *pbModulusBuf,
                                uint16  uModulusLen,
                                uint8  *pbResultBuf,
                                uint32 *puResultLen,
                                int    *pnResult,
                                AEECallback *pcb );


/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

=======================================================================

=======================================================================
 INTERFACE   DOCUMENTATION
=======================================================================
OEMBN Interface

   OEMBN implementation in AEE layer uses the interface defined in
   this file to perform all the OEMBN operations.

==============================================================================

Function: OEMBN_Supported()

Description:

  Tells us whether or not OEMBN is supported on the target platform. Essentially
  this depends on whether or not hardware based mod exp capabilities are enabled.

   
Prototype:

  boolean OEMBN_Supported (void)


Parameters:
   None

Return Value:
   boolean:

  TRUE: OEMBN is supported - i.e. we have DSP based mod exp capability
  FALSE: OEMBN is not supported - i.e. we don't DSP based mod exp capability

Comments:
   
Side Effects: 
   None     

==============================================================================

Function: OEMBN_ModExp()

Description:

  Computes the modular exponentiation of the message (input via pbMsgBuf).
  The result is stored in pbResultBuf.

  NOTE: It is extremely important that pbMsgBuf does not intersect with
  pbResultBuf. The DSP is unable to process the result in place.
  
  pbResult = pbmsgBuf^pbExponent mod pbModulus

  Modulus must be odd (which is the case for most security related moduli)
  If the callback is null, the function is executed synchronously.  The error is
  status always returned via pnResult.

   
Prototype:

  void OEMBN_ModExp ( const uint8  *pbMsgBuf,
                            uint16  uMsgLen,
                      const uint8  *pbExponentBuf,
                            uint16  uExponentLen,                     
                      const uint8  *pbModulusBuf,
                            uint16  uModulusLen,
                            uint8  *pbResultBuf,
                            uint32 *puResultLen,
                            int    *pnResult,
                            AEECallback *pCB );


Parameters:
   pbMsgBuf:      The data to be mod-exp-ed,
   uMsgLen:       Length of the data
   pbExponentBuf: The exponent
   uExponentLen:  Length of the exponent                   
   pbModulusBuf:  The modulus
   uModulusLen:   Length of the modulus
   pbResultBuf:   Buffer to hold the result 
   puResultLen:   Length of the result
   pnResult: Populated with the result of the operation
   pCB: Callback to be called on completion. NULL value 
        results in syncronous operation.


Return Value:
   None

  *pnResult will have:
  ENOMEMORY: on lack of memory
  EBADPARM:  if modulus is not a odd number

Comments:
   
Side Effects: 
   None     

=============================================================================*/
#endif // #ifndef OEMBN_DSP_H
