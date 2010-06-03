#ifndef _OEMBTEXTNA_H_
#define _OEMBTEXTNA_H_

/*===========================================================================

FILE:      OEMBTExtNA.h

SERVICES:  BlueTooth Network Access QCT extension

GENERAL DESCRIPTION: BREW interface to the Bluetooth Network Access

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  N/A

        Copyright © 2004-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
$Header: //source/qcom/qct/wconnect/bthost/brew/statext/rel/00.00.26/src/OEMBTExtNA.h#1 $
$DateTime: 2009/01/07 18:14:54 $
===========================================================================*/

//==========================================================================
//   Macro definitions
//==========================================================================

//==========================================================================
//   Function prototypes
//==========================================================================

/*===========================================================================

Function:  OEMBTExtNA_New()

Description:
   This is called from AEE when an app tries to create an instance of a
   class ID associated with this module.
   There can only be one of these interfaces at a time.

Parameters:
   IBTExtNA *pParent: Pointer to IBTExtNA allocated by AEE

Return Value:  SUCCESS if object created and initialized
               ENOMEMORY if unable to create object
               EFAILED if unable to initialize object

Componts:      None

Side Effects:  Allocates app-associated memory.

===========================================================================*/
int OEMBTExtNA_Init( IBTExtNA *pParent );

// IBase (see AEE.h)
int OEMBTExtNA_Release( IBTExtNA *pParent );

// IBTEXTNA_Enable (see AEEBTExtNA.h)
int OEMBTExtNA_Enable( IBTExtNA *pParent );

// IBTEXTNA_Disable (see AEEBTExtNA.h)
int OEMBTExtNA_Disable( IBTExtNA *pParent );

#endif // _OEMBTEXTNA_H_
