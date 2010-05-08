#ifndef __SMS_AGENT_CLASSID_H__
#define __SMS_AGENT_CLASSID_H__

/*=========================================================================
FILE: SMSAgent_ClassId.h

SERVICES: GSM1x SS Agent Class Id definition

GENERAL DESCRIPTION:
    This is the definition of the SS Agent Class Id.

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION & SEQUENCING REQUIREMENTS:

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

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
    #include "SMSAgent.bid"  // Agent Applet class ID
#endif

#if (!defined AEECLSID_SMS_AGENT)
    #error AEECLSID_SMS_AGENT must be defined!
#endif

#endif // end __SMS_AGENT_CLASSID_H__
