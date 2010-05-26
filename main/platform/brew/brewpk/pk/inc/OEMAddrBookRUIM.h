#ifndef OEMADDRBOOKRUIM_H
#define OEMADDRBOOKRUIM_H
/*****************************************************************************
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

        O E M       R U I M   A D D R B O O K   I N T E R F A C E

  This file contains the exported interfaces and definitions for access to the
  Address book of RUIM on the handset.

        Copyright © 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
*****************************************************************************/

#include "AEE.h"
#include "OEMAddrBook.h"

// Init of AddressBook OEM function table.
VTBL(IOEMAddrBook) *OEMRUIMAddr_GetFuncs(void);
#ifdef CUST_EDITION	
uint16   OEMRUIMAddr_GetRUIMMaxNameSize(void);
uint16   OEMRUIMAddr_GetRUIMMaxNumberSize(void);
#endif /*CUST_EDITION*/
#endif      // OEMADDRBOOKRUIM_H

/*=====================================================================
  INTERFACES DOCUMENTATION
=======================================================================
Interface Name: OEMRUIMAddr 
=======================================================================

Function: OEMRUIMAddr_GetFuncs()

Description:
   This function is called when the BREW AddressBook interface is created with
   a ClassID of AEECLSID_RUIM.  This function returns the OEM R-UIM functions
   that are needed by the IAddrBook interface to access the R-UIM phone book.

Prototype:
   VTBL(IOEMAddrBook) *OEMAddrBook_Init(void);

Parameters:
   None

Return Value:
   Returns the table of OEMRUIM functions needed by the IAddrBook interface to
   access the R-UIM phone book.

Comments:
   None

Side Effects:
   None

See Also:
   IRUIM_QueryInterface()
============================================================================= */

