#ifndef OEMGSM1X_CONTROL_SVC_H
#define OEMGSM1X_CONTROL_SVC_H 1
/*===========================================================================

                       OEMGSM1xControl_SVC.H
                

GENERAL DESCRIPTION
   OEM GSM1x Control Interfaces.
   The interfaces defined in this file are the building blocks
   to GSM1x Actiovation BREW Applet.  The interfaces support
   provisioning of GSM1x data from SIM to a specially designated
   NAM in NV as well as switching between NAMs at run time.
   
INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.
   
 

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "AEEGSM1xControl.h"
                                                                             
/*===========================================================================

                      CONSTANTS

===========================================================================*/

/* The offset of the last field in nv_ram_roaming_list_type */                                       
#define PACKED_PRL_OFFSET           (6)
                                                                             
/*===========================================================================

                      TYPES 

===========================================================================*/


/*===========================================================================

                      FUNCTION DECLARATIONS

===========================================================================*/

void OEMGSM1xControl_SVC_Init
(
   void
);

AEEGSM1xControl_statusType  OEMGSM1xControl_SVC_ActivateNonGSM1xMode
(
   AEEGSM1xControl_modeBitMaskType         mode
);

AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetSupportedProvisioningModes
( 
   AEEGSM1xControl_DFPresenceBitMaskType  *pModeMask 
);

AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetUIMUniqueId
( 
   uint16                                  maxBufLen,
   byte                                   *pId,
   uint16                                 *actualLen
);

AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetDFPresence
(  
   AEEGSM1xControl_DFPresenceBitMaskType  *presenceMask
);

AEEGSM1xControl_statusType OEMGSM1xControl_SVC_ProvisionGSM1xParameters
( 
   void
);

AEEGSM1xControl_statusType OEMGSM1xControl_SVC_EnableGSM1xMode
(
   void
);

AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetPLMN
(
   AEEGSM1xControl_PLMNTypeBitMaskType     types, 
   uint16                                  maxPLMNEntriesCnt, 
   AEEGSM1xControl_PLMNTripletType        *PLMNBuf,  
   uint16                                 *actualPLMNEntriesCnt
);

AEEGSM1xControl_statusType OEMGSM1xControl_SVC_SetPLMN
(
   uint16                                  PLMNEntriesCnt, 
   AEEGSM1xControl_PLMNTripletType        *PLMNBuf
);


AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetGSM1xPRL
(
   uint16                                  maxPRLSizeBytes,
   byte                                   *packedPRL 
);

AEEGSM1xControl_statusType OEMGSM1xControl_SVC_SetGSM1xPRL
(
   uint16                                  PRLStructsize,
   byte                                   *pPRLStruct
);

AEEGSM1xControl_statusType OEMGSM1xControl_SVC_ValidatePRL
(
   uint16                                  packedPRLsize,
   byte                                   *packedPRL,
   boolean                                *isValid
);

AEEGSM1xControl_statusType OEMGSM1xControl_SVC_SetGSM1xSIDNIDPairs
(
   uint16                                  HomeSIDNIDCnt, 
   AEEGSM1xControl_SIDNIDPairType         *HomeSIDNIDPairs,
   uint16                                  LockedSIDNIDCnt, 
   AEEGSM1xControl_SIDNIDPairType         *LockedSIDNIDPairs
);

AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetGSM1xSIDNIDPairs
(
   uint16                                  HomeSIDNIDMaxCnt, 
   AEEGSM1xControl_SIDNIDPairType         *HomeSIDNIDPairs,
   uint16                                 *ActualHomeSIDNIDCnt,
   uint16                                  LockedSIDNIDMaxCnt, 
   AEEGSM1xControl_SIDNIDPairType         *LockedSIDNIDPairs,
   uint16                                 *ActualLockedSIDNIDCnt
);

AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetAvailableModes
(
   AEEGSM1xControl_modeBitMaskType        *modeMask
);

AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetCurrentMode
(
   AEEGSM1xControl_modeBitMaskType        *modeMask
);

// This function is called from AEEGSM1xSig to get the current GSM1x status.
AEEGSM1xSig_Status OEMGSM1xControl_SVC_GetStatus
(
   void
);

// This function is called from TAPI when the phone is ready to go ONLINE
// for the first time at power up.
void OEMGSM1xControl_SVC_ProcessPowerUp
(
   void
);


#endif    // OEMGSM1X_CONTROL_SVC_H      
