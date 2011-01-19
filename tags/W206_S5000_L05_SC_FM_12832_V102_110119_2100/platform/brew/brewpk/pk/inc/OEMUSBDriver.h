/*===========================================================================
                    IMPORTANT NOTICE

              INTERNAL DISTRIBUTION ONLY

FILE:  OEMUSBDriver.h

SERVICES:  Testing mem stuff

GENERAL DESCRIPTION:
   This will be surplanted by a real OEMUSBDriver.h file

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
#ifndef OEMUSBDRIVER_H
#define OEMUSBDRIVER_H

#include "OEMFeatures.h"

#ifdef FEATURE_BREW_USBDRIVER
#include "AEEUSBDriver.h"

#define OEMUSBDRIVER_PORT_BIBULK1      "USBDRIVER_BIBULK_1"
#define OEMUSBDRIVER_PORT_INTERRUPT1   "USBDRIVER_INTERRUPT_1"

/*===========================================================================
The Section below contains functions exported by the BREW USB Driver to
AMSS.
**=========================================================================*/

/*===========================================================================
FUNCTION BREWDSM_NEW_BUFFER()

DESCRIPTION
  This is an ISR-safe function to obtain a preallocated BREW memory block.
  It returns a very large DSM item if available, NULL if not.

DEPENDENCIES
  None.

PARAMETERS
  None

RETURN VALUE
  A pointer to a 'dsm_item_type'.  NULL if a block could not be allocated.

SIDE EFFECTS
  None
===========================================================================*/
extern dsm_item_type* brewdsm_new_buffer(void);

/*===========================================================================
FUNCTION BREWDSM_FREE_BUFFER()

DESCRIPTION
  This function will free the BREW DSM buffer and the underlying
  DSM item.  It is meant to be called from within BREW.  It is *NOT*
  ISR-safe.

DEPENDENCIES
  None

PARAMETERS
  item_ptr - Pointer to item to return to free queue. 

RETURN VALUE
  0 if successful, any other value if not.

SIDE EFFECTS
  Note that this does not clear pointer, unlike free_packet
===========================================================================*/
extern int brewdsm_free_buffer(dsm_item_type *pDSMItem);

#endif /* FEATURE_BREW_USBDRIVER */
#endif /* OEMUSBDRIVER_H */
