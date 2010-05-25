#ifndef __SS_OPERATIONS_CLASSID_H__
#define __SS_OPERATIONS_CLASSID_H__

/*=============================================================================
FILE: SSOperations_ClassId.h

SERVICES: GSM1x SS Operations Class Id definition

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
=============================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

/*===========================================================================
                              DEFINITIONS AND CONSTANTS
===========================================================================*/
#if defined(BREW_STATIC_APP)
   #include "OEMClassIDs.h"      // Applet Ids
#else
   // Use the .bid file if compiling as dynamic applet
   #include "SSOperations.bid" // Applet class ID
   #include "SSPromptApp.bid" // Prompt Applet class ID
#endif

#if ((!defined AEECLSID_SS_OPERATIONS) || (!defined AEECLSID_SS_PROMPTAPP))
    #error AEECLSID_SS_OPERATIONS and AEECLSID_SS_PROMPTAPP must be defined!
#endif


#endif // end __SS_OPERATIONS_CLASSID_H__
