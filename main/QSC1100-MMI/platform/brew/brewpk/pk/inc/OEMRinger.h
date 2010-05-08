/*======================================================
FILE:  OEMRinger.h

SERVICES:  OEM Ringer Interface header

GENERAL DESCRIPTION:

This file contains all of the function prototypes for
ringer routines that are required by the AEE.

PUBLIC CLASSES:

None.

INITIALIZATION AND SEQUENCING REQUIREMENTS
None.

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#if !defined(OEM_RINGER_H)
#define OEM_RINGER_H  1

#include "AEERinger.h"

#if defined(__cplusplus)
extern "C" {
#endif

//
// OEM Ringer functions
//
extern AEERingerID OEM_GetActiveRinger(AEERingerCatID idRingerCat, AECHAR * szwName);
extern int OEM_SetActiveRinger(AEERingerCatID idCategory, AEERingerID idRinger, char * szFilename);

#if defined(__cplusplus)
}
#endif

#endif // !defined(OEM_RINGER_H)

/*=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================
OEMRinger Interface
=======================================================================

Function: OEM_GetActiveRinger()

Description:

 This function returns the ID of the active ringer in the category given.

Prototype:
   AEERingerID OEM_GetActiveRinger(AEERingerCatID idRingerCat, AECHAR * szwName);

Parameters:

  idRinger: ID of the category the active ringer is in.
  szwName:  Name of the Ringer Category.

Return Value:  The ID of the active Ringer in the category given.
Comments:
    None

Side Effects:
    None

See Also:
    None

==================================================================
Function: OEM_SetActiveRinger()

Description:

 This function sets the active ringer, which will be used for subsequent calls to
 the IRINGERMGR_Play function for example.

Prototype:
   int OEM_SetActiveRinger(AEERingerCatID idCategory, AEERingerID idRinger, char * szFilename);

Parameters:

  idCategory: ID of the category the ringer is in.
  idRinger:   ID of the ringer.
  szFilename: Filename of the ringer.

Return Value:  SUCCESS if the operaion was successful. EFAILED for any errors.
Comments:
    None

Side Effects:
   None

See Also:
   None
   
==============================================================
AEERinger


See the IRinger Interface in the BREW API Reference.



==================================================================*/
