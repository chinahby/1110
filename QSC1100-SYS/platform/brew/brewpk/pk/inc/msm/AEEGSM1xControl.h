#ifndef AEEGSM1x_CONTROL_H
#define AEEGSM1x_CONTROL_H 1
/*===========================================================================

                       A E E G S M 1 x C O N T R O L. H
                

GENERAL DESCRIPTION
   AEE GSM1x Control Interfaces.
   The interfaces defined in this file are the building blocks
   to GSM1x Actiovation BREW Applet.  The interfaces support
   provisioning of GSM1x data from SIM to a specially designated
   NAM in NV as well as switching between NAMs at run time.
   
INITIALIZATION AND SEQUENCING REQUIREMENTS
   None.
   
   
   

Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "AEE.h"
#include "AEEGSM1xSig.h"

                                                                             
     
/*===========================================================================

                      CONSTANTS

===========================================================================*/

#define AEECLSID_IGSM1XCONTROL          0x01011ccb
#define AEECLSID_IGSM1XCONTROLNOTIFIER  0x0101217c
                                            
                                            
                     
/* Provisioning modes of a GSM1x enabled CDMA phone.*/
#define AEEGSM1XCONTROL_GSM1X_PROV_MASK                  (0x0001)
#define AEEGSM1XCONTROL_1X_NV_PROV_MASK                  (0x0002)
#define AEEGSM1XCONTROL_1X_RUIM_PROV_MASK                (0x0004)
#define AEEGSM1XCONTROL_EMERGENCY_PROV_MASK              (0x0008)



/* Bitmasks that are used to indicate directory presence 
   on an identity card.*/
#define AEEGSM1XCONTROL_MF_PRESENT                       (0x0001)
#define AEEGSM1XCONTROL_CDMA_DF_PRESENT                  (0x0002)
#define AEEGSM1XCONTROL_GSM_DF_PRESENT                   (0x0004)
#define AEEGSM1XCONTROL_DCS1800_DF_PRESENT               (0x0008)


/* Types of PLMN information.*/
#define AEEGSM1XCONTROL_HOME_PLMN	                     (0x0001)
#define AEEGSM1XCONTROL_SEL_PLMN   	                     (0x0002)
#define AEEGSM1XCONTROL_FORBIDDEN_PLMN                   (0x0004)



/* Returned status values used by routines in this class.*/
#define   AEEGSM1XCONTROL_STATUS_SUCCESS                 (0x0000)
#define   AEEGSM1XCONTROL_STATUS_NO_CARD                 (0x0001)
#define   AEEGSM1XCONTROL_STATUS_NO_INFO_ON_CARD         (0x0002)
#define   AEEGSM1XCONTROL_STATUS_MODE_NOT_SUPPORTED      (0x0003)
#define   AEEGSM1XCONTROL_STATUS_NAM_SELECTION_FAILED    (0x0004)
#define   AEEGSM1XCONTROL_STATUS_BUF_TOO_SMALL           (0x0005)
#define   AEEGSM1XCONTROL_STATUS_FIELD_UNINITIALIZED     (0x0006)
#define   AEEGSM1XCONTROL_STATUS_VALUE_OUT_OF_RANGE      (0x0007)
#define   AEEGSM1XCONTROL_STATUS_INVALID_POINTER         (0x0008)
#define   AEEGSM1XCONTROL_STATUS_GSM1X_NOT_SUPPORTED     (0x0009)
#define   AEEGSM1XCONTROL_STATUS_CANNOT_READ_FROM_UIM    (0x000A)
#define   AEEGSM1XCONTROL_STATUS_CANNOT_WRITE_TO_UIM     (0x000B)
#define   AEEGSM1XCONTROL_STATUS_FAILURE_NV_WRITE        (0x000C)
#define   AEEGSM1XCONTROL_STATUS_FAILURE_NV_READ         (0x000D)
#define   AEEGSM1XCONTROL_STATUS_FAILURE_NAM_SELECT      (0x000E)
#define   AEEGSM1XCONTROL_STATUS_INVALID_DATA_GSM_DF     (0x000F)
#define   AEEGSM1XCONTROL_STATUS_INTERNAL_ERROR          (0x0010)
#define   AEEGSM1XCONTROL_STATUS_INVALID_PARAMETER       (0x0011)
#define   AEEGSM1XCONTROL_STATUS_INVALID_INFO_IN_NV      (0x0012)
#define   AEEGSM1XCONTROL_STATUS_INVALID_PRL             (0x0013)
#define   AEEGSM1XCONTROL_STATUS_COULD_NOT_CREATE_IPHONE (0x0014)
#define   AEEGSM1XCONTROL_STATUS_CANNOT_SET_DESIRED_MODE (0x0015)
#define   AEEGSM1XCONTROL_STATUS_INVALID_MODE            (0x0016)


/* Action for the GSM1x Activation App */
#define   AEEGSM1XCONTROL_ACTION_RESERVED                (0x0000)
#define   AEEGSM1XCONTROL_ACTION_PROVISION_GSM1X         (0x0001)
#define   AEEGSM1XCONTROL_ACTION_BRING_UP_GUI            (0x0002)

/* More return status values to be added.*/               


// Notification Masks
//                   
#define NMASK_GSM1xCONTROL_STATUS_CHANGE                 (0x0001)  
/* Indicates activation app should be initiated */
#define NMASK_GSM1xCONTROL_ACTIVATION                    (0x0004)

                                                                             
/*===========================================================================

                      TYPES 

===========================================================================*/
typedef uint16 AEEGSM1xControl_modeBitMaskType;

typedef uint16 AEEGSM1xControl_DFPresenceBitMaskType;

typedef uint16 AEEGSM1xControl_PLMNTypeBitMaskType;

typedef PACKED struct {
   AEEGSM1xControl_PLMNTypeBitMaskType	PLMNtype;
   uint16                               MCC; 
   uint16                               MNC; 
} AEEGSM1xControl_PLMNTripletType;

typedef byte AEEGSM1xControl_NAMType;

typedef uint16 AEEGSM1xControl_statusType;


typedef PACKED struct {
  uint16      sid;
  uint16      nid;
} AEEGSM1xControl_SIDNIDPairType;


typedef byte AEEGSM1xControl_AppActionType;

/*===========================================================================

                    CLASS DEFINITIONS

===========================================================================*/

typedef struct IGSM1xControl IGSM1xControl;
AEEINTERFACE(IGSM1xControl)
{
    INHERIT_IQueryInterface(IGSM1xControl);
   
    AEEGSM1xControl_statusType (*ActivateNonGSM1xMode)
    (
       IGSM1xControl                         *pInstance,
       AEEGSM1xControl_modeBitMaskType        mode
    );


    AEEGSM1xControl_statusType (*GetSupportedProvisioningModes)
    ( 
       IGSM1xControl                         *pInstance,
       AEEGSM1xControl_DFPresenceBitMaskType *pModeMask 
    );


    AEEGSM1xControl_statusType (*GetUIMUniqueId)
    ( 
       IGSM1xControl                         *pInstance,
       uint16                                 maxBufLen,
       byte                                  *pId,
       uint16                                *actualLen
    );

    AEEGSM1xControl_statusType (*GetDFPresence)
    (
       IGSM1xControl                         *pInstance,
       AEEGSM1xControl_DFPresenceBitMaskType *presenceMask
    );

    AEEGSM1xControl_statusType (*ProvisionGSM1xParameters)
    ( 
       IGSM1xControl                         *pInstance
    );

  
    AEEGSM1xControl_statusType (*EnableGSM1xMode)
    ( 
       IGSM1xControl                         *pInstance
    );

    AEEGSM1xControl_statusType (*GetPLMN)
    (
       IGSM1xControl                         *pInstance,
       AEEGSM1xControl_PLMNTypeBitMaskType    types, 
       uint16                                 maxPLMNEntriesCnt, 
       AEEGSM1xControl_PLMNTripletType       *PLMNBuf,  
       uint16                                *actualPLMNEntriesCnt
    );


    AEEGSM1xControl_statusType (*SetPLMN)
    (
       IGSM1xControl                         *pInstance,
       uint16                                 PLMNEntriesCnt,
       AEEGSM1xControl_PLMNTripletType       *PLMNBuf
    );


    AEEGSM1xControl_statusType (*GetGSM1xPRL)
    (
       IGSM1xControl                         *pInstance,
       uint16                                 maxPRLSizeBytes, 
       byte                                  *packedPRL 
    );
    
    AEEGSM1xControl_statusType (*SetGSM1xPRL)
    (
       IGSM1xControl                         *pInstance,
       byte                                  *pPRLStruct
    );

    
    AEEGSM1xControl_statusType (*ValidatePRL)
    (  
       IGSM1xControl                         *pInstance,
       byte                                  *packedPRL,
       boolean                               *isValid
    );

    AEEGSM1xControl_statusType (*SetGSM1xSIDNIDPairs)
    (
       IGSM1xControl                         *pInstance,
       uint16                                 HomeSIDNIDCnt, 
       AEEGSM1xControl_SIDNIDPairType        *HomeSIDNIDPairs,
       uint16                                 LockedSIDNIDCnt, 
       AEEGSM1xControl_SIDNIDPairType        *LockedSIDNIDPairs
    );

    AEEGSM1xControl_statusType (*GetGSM1xSIDNIDPairs)
    (
       IGSM1xControl                         *pInstance,
       uint16                                 HomeSIDNIDMaxCnt, 
       AEEGSM1xControl_SIDNIDPairType        *HomeSIDNIDPairs,
       uint16                                *ActualHomeSIDNIDCnt,
       uint16                                 LockedSIDNIDMaxCnt, 
       AEEGSM1xControl_SIDNIDPairType        *LockedSIDNIDPairs,
       uint16                                *ActualLockedSIDNIDCnt
    );

    AEEGSM1xControl_statusType (*GetAvailableModes)
    (
       IGSM1xControl                         *instancePtr,
       AEEGSM1xControl_modeBitMaskType       *modeMask
    );

    AEEGSM1xControl_statusType (*GetCurrentMode)
    (
       IGSM1xControl                         *instancePtr,
       AEEGSM1xControl_modeBitMaskType       *modeMask
    );
};


//----------------------
// IGSM1xControl API
//----------------------
#define IGSM1xControl_AddRef(p) \
           AEEGETPVTBL(p,IGSM1xControl)->AddRef(p)

#define IGSM1xControl_Release(p) \
           AEEGETPVTBL(p,IGSM1xControl)->Release(p)

// QueryInterface
#define IGSM1xControl_QueryInterface(p,i,p2) \
           AEEGETPVTBL(p,IGSM1xControl)->QueryInterface(p,i,p2)

// GSM1xControl methods
#define IGSM1xControl_ActivateNonGSM1xMode(p,a) \
           AEEGETPVTBL(p,IGSM1xControl)->ActivateNonGSM1xMode(p,a)

#define IGSM1xControl_GetSupportedProvisioningModes(p,a)   \
                                                  AEEGETPVTBL(p,IGSM1xControl)->GetSupportedProvisioningModes(p,a)
           
#define IGSM1xControl_GetUIMUniqueId(p,a,b,c) \
           AEEGETPVTBL(p,IGSM1xControl)->GetUIMUniqueId(p,a,b,c)

#define IGSM1xControl_GetDFPresence(p,a) \
           AEEGETPVTBL(p,IGSM1xControl)->GetDFPresence(p,a)
           
#define IGSM1xControl_ProvisionGSM1xParameters(p) \
           AEEGETPVTBL(p,IGSM1xControl)->ProvisionGSM1xParameters(p)

#define IGSM1xControl_EnableGSM1xMode(p) \
           AEEGETPVTBL(p,IGSM1xControl)->EnableGSM1xMode(p)

#define IGSM1xControl_GetPLMN(p,a,b,c,d) \
           AEEGETPVTBL(p,IGSM1xControl)->GetPLMN(p,a,b,c,d)

#define IGSM1xControl_SetPLMN(p,a,b) \
           AEEGETPVTBL(p,IGSM1xControl)->SetPLMN(p,a,b)

// Access methods for some NV parameters (wrappers for IConfig)
#define IGSM1xControl_GetGSM1xPRL(p,a,b) \
           AEEGETPVTBL(p,IGSM1xControl)->GetGSM1xPRL(p,a,b)

#define IGSM1xControl_SetGSM1xPRL(p,a) \
           AEEGETPVTBL(p,IGSM1xControl)->SetGSM1xPRL(p,a)

#define IGSM1xControl_ValidatePRL(p,a,b) \
           AEEGETPVTBL(p,IGSM1xControl)->ValidatePRL(p,a,b)

#define IGSM1xControl_SetGSM1xSIDNIDPairs(p,a,b,c,d) \
           AEEGETPVTBL(p,IGSM1xControl)->SetGSM1xSIDNIDPairs(p,a,b,c,d)

#define IGSM1xControl_GetGSM1xSIDNIDPairs(p,a,b,c,d,e,f) \
           AEEGETPVTBL(p,IGSM1xControl)->GetGSM1xSIDNIDPairs(p,a,b,c,d,e,f)

#define IGSM1xControl_GetAvailableModes(p,a) \
           AEEGETPVTBL(p,IGSM1xControl)->GetAvailableModes(p,a)
#define IGSM1xControl_GetCurrentMode(p,a) \
           AEEGETPVTBL(p,IGSM1xControl)->GetCurrentMode(p,a)

#define IGSM1xControl_GetGSM1xSIDNIDPairs(p,a,b,c,d,e,f) \
                                                  AEEGETPVTBL(p,IGSM1xControl)->GetGSM1xSIDNIDPairs(p,a,b,c,d,e,f)

#define IGSM1xControl_SetGSM1xSIDNIDPairs(p,a,b,c,d) \
                                                  AEEGETPVTBL(p,IGSM1xControl)->SetGSM1xSIDNIDPairs(p,a,b,c,d)
                                                 

/*=====================================================================
  INTERFACES   DOCUMENTATION
======================================================================= 

Interface Name: IGSM1xControl

Description: 
   IGSM1xControl inteface enables GSM1x capability on the mobile. It contains
   interfaces that are the building blocks for GSM1x Activation BREW Applet.  
   The interfaces support provisioning of GSM1x data from SIM or R-UIM to 
   a specially designated NAM in NV as well as switching between NAMs 
   at run time.
   
   Typically, GSM1x Activation BREW Applet will follow the following 
   sequence of operations:
     - Find out the current mode using IGSM1xControl_GetCurrentMode().
     - Find out which modes are available using IGSM1xControl_GetAvailableModes().
     - If no provisioning modes are available, go to "Emergency calls only"
       state using IGSM1xControl_ActivateNonGSM1xMode() and exit.
     - Choose the new provisioning mode either automatically or by interacting
       with the user.
     - If the new mode is CDMA1x, select it using IGSM1xControl_ActivationNonGSM1xMode().
     - If the new mode is GSM1x, do the following sequence (order is important)
       of operations:
          - Call IGSM1xControl_ProvisionGSM1xParameters() to provision IMSI, ACCOLC and MSISDN.
          - Form GSM1x PRL and set it using IGSM1xControl_SetGSM1xPRL().
          - Set home and locked SID/NID pairs using IGSM1xControl_SetGSM1xSIDNIDPairs().
          - Call IGSM1xControl_EnableGSM1xMode().
                 
    GSM1x Activation BREW Applet can, optionally, provide interactive editing capability
    for PLMN selector and Forbidden PLMN information stored in SIM/R-UIM.                                                                                   
                                                                    
   
    In addition to its core functionality, this class is used to signal GSM1x BREW Applets
    (other than GSM1x Activation BREW Applet) whenever GSM1x mode is activated or de-activated.                                                                                    
    IGSM1xControl sends event notification whenever GSM1x mode changes (enabled or disabled).
    Whenever a new mode is activated on IGSM1xControl it sends out a 
    NMASK_GSM1x_STATUS_CHANGE notification. AEEGSM1xSig_Status is sent as 
    dwParam member of the EVT_NOTIFY event. 
    To send out notification, a helper class IGSM1xControlNotifier is used. 
    Methods for IGSM1xControlNotifier should not be called directly by BREW 
    applets. Brew applet should specify the class id for IGSM1xControlNotifier 
    and the NMASK=NMASK_GSM1xCONTROL_STATUS_CHANGE in its MIF file.
   
    In general, all GSM1x applets should behave as follows:
    On Init 
       - Register to receive NMASK_GSM1xCONTROL_STATUS_CHANGE.
       - Call IGSM1xSig_GetStatus() to ensure that GSM1x capability is enabled. 
         If GSM1x capability is disabled, app can exit with an appropriate message 
         or wait until it receives a GSM1xSIG_STATUS_CHANGE event with GSM1xSIG_ACTIVE.

    Runtime 
       - If a GSM1xSIG_STATUS_CHANGE event is received, handle it appropriately.


    The IGSM1xControl interface is obtained via the ISHELL_CreateInstance mechanism.

See Also:
   IGSM1xSig
   IQueryInterface

   
======================================================================= 
   
Function: IGSM1xControl_ActivateNonGSM1xMode()

Description: 
   Activate the specified provisioning mode - 
      AEEGSM1XCONTROL_1X_NV_PROV_MASK    
      AEEGSM1XCONTROL_1X_RUIM_PROV_MASK    
      AEEGSM1XCONTROL_EMERGENCY_PROV_MASK  

      
Prototype:

   AEEGSM1xControl_statusType IGSM1xControl_ActivateNonGSM1xMode
   (
      IGSM1xControl                         *instancePtr,
      AEEGSM1xControl_DFPresenceBitMaskType  mode
   )
   
Parameters:
   instancePtr is a pointer to the IGSM1xControl object.
   
   mode        is the desired provisioning mode.
   
Return Value:
   AEEGSM1xControl_statusType
   
Comments:
   This function is usually called by GSM1x Activation App when a user
   desires to switch to CDMA 1x provisioning mode.
      
Side Effects:
   NMASK_GSM1xCONTROL_STATUS_CHANGE notification sent to all registered apps.
   
See Also:
   None
   
======================================================================= 

Function:  IGSM1xControl_GetSupportedProvisioningModes

Description:
   Returns the bit mask that indicates which of the possible 
   provisioning modes are supported by the software (OEM).
   Note, that this calls does not take into account the presence
   and the type of a User Identity Module (smartcard.)  Thus,
   even if some mode is supported by the software, user might
   not be able to use it because he or she does not have
   a "right" UIM.
     
Prototype:
   AEEGSM1xControl_statusType IGSM1xControl_GetSupportedProvisioningModes 
   ( 
      IGSM1xControl                         *instancePtr,
      AEEGSM1xControl_DFPresenceBitMaskType *modeMask 
   );
 
Parameters:
   instancePtr is a pointer to the IGSM1xControl object.
   
   modeMask is a bit mask that contains zero or more values:
                 AEEGSM1XCONTROL_GSM1X_PROV_MASK, 
                 AEEGSM1XCONTROL_1X_NV_PROV_MASK,
                 AEEGSM1XCONTROL_1X_RUIM_PROV_MASK.
         
Return Value:
   AEEGSM1xControl_statusType - the set of returned values
   are defined by constants whose name starts with the prefix
   AEEGSM1XCONTROL_STATUS_  
   
Comments:
   Relies on the implementation provided by OEMs.
      
Side Effects:
   None.
      
See Also:
   None
           
======================================================================= 

Function: IGSM1xControl_GetUIMUniqueId

Description:
   Returns the unique ICCId stored in EFiccid field on SIM or R-UIM.
   "Usual" length is 10 bytes.
      
Prototype:
   AEEGSM1xControl_statusType IGSM1xControl_GetUIMUniqueId
   ( 
      IGSM1xControl                         *pInstance,
      uint16                                 maxBufLen,
      byte                                  *pId,
      uint16                                *actualLen
   );


      
Parameters:
   instancePtr is a pointer to the IGSM1xControl object.
   
   maxBufLen   specified the maximum length of a memory buffer pointed by pId.
   
   pId   points to a memory location to receive the ICCId.  The contents
         of this location are undefined unless AEEGSM1XCONTROL_STATUS_SUCCESS
         is returned.  This location should have at least 10 bytes available. 
          
   actualLen points to a memory location to receive the actual length of ICCId.
         The contents of this location are undefined unless 
         AEEGSM1XCONTROL_STATUS_SUCCESS is returned. 
     
Return Value:
   AEEGSM1xControl_statusType - the set of returned values
   are defined by constants whose name starts with the prefix
   AEEGSM1XCONTROL_STATUS_  
   
Comments:
   None.
      
Side Effects:
   None.
      
See Also:
   None.
  
======================================================================= 

Function: IGSM1xControl_GetDFPresence

Description:
   Returns the indication which directory files (DFs) are present 
   on the currently available user identity card (SIM or R-UIM.)
      
Prototype:

   AEEGSM1xControl_statusType IGSM1xControl_GetDFPresence
   (
      IGSM1xControl                          *instancePtr,
      AEEGSM1xControl_DFPresenceBitMaskType  *presenceMask
   );
      
Parameters:
   instancePtr is a pointer to the IGSM1xControl object.
   
   presenceMask points to a memory location to receive the bitmask
                  that indicates which DFs are present.
                  Currently, the following DFs are supported:
                     AEEGSM1XCONTROL_MF_PRESENT
                     AEEGSM1XCONTROL_CDMA_DF_PRESENT
                     AEEGSM1XCONTROL_GSM_DF_PRESENT
                  The following mode will be supported in future
                  releases:
                     AEEGSM1XCONTROL_DCS1800_DF_PRESENT.
                  The contents of this location are undefined 
                  unless AEEGSM1XCONTROL_STATUS_SUCCESS is returned. 
     
Return Value:
   AEEGSM1xControl_statusType - the set of returned values
   are defined by constants whose name starts with the prefix
   AEEGSM1XCONTROL_STATUS_ 
  
Comments:
   AEEGSM1XCONTROL_DCS1800_DF_PRESENT is currently not supported,
   its mask will not be returned by this call. 
      
Side Effects:
   None   
       
See Also:
   None
   
======================================================================= 

Function:  IGSM1xControl_ProvisionGSM1xParameters

Description:
   This routine reads GSM IMSI, ACCOLC, and MSISDN from the present UIM card 
   (if any), converts it to CDMA IMSI, ACCOLC and MSISDN according to GSM1x 
   provisioning algorithm, and writes the results into in NV, associated 
   with GSM1x NAM.
      
Prototype:
   AEEGSM1xControl_statusType  IGSM1xControl_ProvisionGSM1xParameters 
   ( 
       IGSM1xControl      *instancePtr 
   );
        
Parameters:
   instancePtr is a pointer to the IGSM1xControl object.
     
Return Value:
   AEEGSM1xControl_statusType - the set of returned values
   are defined by constants whose name starts with the prefix
   AEEGSM1XCONTROL_STATUS_
   
Comments:
   None
      
Side Effects:
   None
      
See Also:
   None
   
              
======================================================================= 

Function:  IGSM1xControl_EnableGSM1xMode

Description:
   This routine commands DMSS's Call Manager module to switch 
   to the GSM1x NAM.  Call Manager will take the phone offline 
   and then re-do the system determination.
      
Prototype:
   AEEGSM1xControl_statusType   IGSM1xControl_EnableGSM1xMode 
   ( 
      IGSM1xControl                    *instancePtr
   )
         
Parameters:
   instancePtr is a pointer to the IGSM1xControl object.
   
     
Return Value:
   AEEGSM1xControl_statusType - the set of returned values
   are defined by constants whose name starts with the prefix
   AEEGSM1XCONTROL_STATUS_
  
   
Comments:
   This routine is synchronious - it waits till the phone finishes
   system determination (that can take significant amount of time.)
   This function is usually called by the GSM1x Activation App
   as the final step in activating GSM1x mode.
      
Side Effects:
   NMASK_GSM1xCONTROL_STATUS_CHANGE notification sent to all registered apps.
      
See Also:
   None
           
======================================================================= 

Function:  IGSM1xControl_GetPLMN

Description:
   Reads (from SIM or R-UIM card) and returns the PLMN entries for all 
   types specified by the provided bitmask.
   
   Home PLMN is retrieved from EFimsi counting EFad.
   PLMN Selector is retrieved from EFplmnsel.
   Forbidden PLMN is retrieved from EFfplmn.
   
Prototype:
   
   AEEGSM1xControl_statusType    IGSM1xControl_GetPLMN
   (
      IGSM1xControl                         *instancePtr,
      AEEGSM1xControl_PLMNTypeBitMaskType    types, 
      uint16                                 maxPLMNEntriesCnt, 
      AEEGSM1xControl_PLMNTripletType       *PLMNBuf,  
      uint16                                *actualPLMNEntriesCnt
   )

      
Parameters:
   instancePtr - pointer to the IGSM1xControl object.
   
   types specifies a bitmask that specifies which types of PLMN information
           ( AEEGSM1XCONTROL_HOME_PLMN, 
             AEEGSM1XCONTROL_SEL_PLMN,
             AEEGSM1XCONTROL_FORBIDDEN_PLMN ) 
         are requested. 
             
   maxPLMNEntriesCnt specifies the maximum number of PLMN entries that
           can fit into a memory location pointed by PLMNBuf.
           
   PLMNBuf points to a memory location to receive the array of PLMN elements.
             The contents of this location are undefined unless 
             AEEGSM1XCONTROL_STATUS_SUCCESS is returned.
             
   actualPLMNEntriesCnt points to a memory location to receive the actual
             number of entries copied into a memory location pointed by PLMNBuf.
             The contents of the location pointed by actualPLMNEntriesCnt 
             are undefined unless AEEGSM1XCONTROL_STATUS_SUCCESS is returned.              
     
Return Value:
   AEEGSM1xControl_statusType - the set of returned values
   are defined by constants whose name starts with the prefix
   AEEGSM1XCONTROL_STATUS_  
   
Comments:
   None
      
Side Effects:
   None
      
See Also:
   IGSM1xControl_SetPLMN

======================================================================= 

Function:  IGSM1xControl_SetPLMN

Description:
   Write the supplied PLMN information to EFplmnsel and/or EFfplmn fields 
   in DFgsm on the currently available SIM or R-UIM card.  
   The entries having type AEEGSM1XCONTROL_HOME_PLMN are ignored.  
   The entries having type AEEGSM1XCONTROL_SEL_PLMN are written to 
   EFplmnsel in the same order as they are present in the supplied array.  
   If there are more entries specified than can fit into EFplmnsel field, 
   the extra entries are ignored.  The entries having type 
   AEEGSM1XCONTROL_FORBIDDEN_PLMN are written to EFfplmn field 
   in the same order as they are present in the supplied array.  
   If there are more entries specified than can fit into EFfplmn field, 
   the extra entries are ignored.  
   
      
Prototype:
   AEEGSM1xControl_statusType   IGSM1xControl_SetPLMN
   (
      IGSM1xControl                   *instancePtr,
      uint16                             PLMNEntriesCnt, 
      AEEGSM1xControl_PLMNTripletType *PLMNBuf
   )  
      
Parameters:
   instancePtr is a pointer to the IGSM1xControl object.
   
   PLMNEntriesCnt specifies the number of entries in the supplied array.
   
   PLMNBuf points to the array containing the PLMN entries to be written.
     
Return Value:
   AEEGSM1xControl_statusType - the set of returned values
   are defined by constants whose name starts with the prefix
   AEEGSM1XCONTROL_STATUS_
  
   
Comments:
   None
      
Side Effects:
   None
      
See Also:
   IGSM1xControl_GetPLMN

                
                    
======================================================================= 

                              
Function: IGSM1xControl_GetGSM1xPRL

Description:
   Returns the packed PRL stored in the NAM assigned to GSM1x. 
   Format of the PRL depends on the software build
   (IS683A or IS683C).
         
Prototype:
   AEEGSM1xControl_statusType IGSM1xControl_GetGSM1xPRL
   (
      IGSM1xControl           *instancePtr,
      uint16                   maxPRLSizeBytes, 
      byte                    *pPRLStruct 
   )

      
Parameters:
   instancePtr is a pointer to the IGSM1xControl object.
   
   maxPRLSizeBytes specifies the maximum size of the buffer pointed by packedPRL.
   
   packedPRL points to a memory location to receive the PRL in the following
             format:
                reserved    (1 byte)  will be filled by this function
                prl_version (2 bytes)
                size        (2 bytes) PRL size in bits
                valid       (1 byte)  boolean
                roaming_list(variable length) packed IS683A format   
             The contents of this location are undefined unless 
             AEEGSM1XCONTROL_STATUS_SUCCESS is returned.
   
   actualSizeBytes points to a memory location to receive the actual length
             in byte of the returned PRL byte array. The contents of this
             location are undefined unless AEEGSM1XCONTROL_STATUS_SUCCESS
             is returned.   
     
Return Value:
   AEEGSM1xControl_statusType - the set of returned values
   are defined by constants whose name starts with the prefix
   AEEGSM1XCONTROL_STATUS_
   
Comments:
  None
      
Side Effects:
  None
      
See Also:
  IGSM1xControl_SetPRL
  
  
======================================================================= 

Function:  IGSM1xControl_SetGSM1xPRL

Description:
   Validates supplied packed PRL and (if valid) writes it to 
   the GSM1x NAM in NV.
   
   The supplied PRL must have the following format 
   
      reserved    (1 byte)  will be filled by this function
      prl_version (2 bytes)
      size        (2 bytes) PRL size in bits
      valid       (1 byte)  boolean
      roaming_list(variable length) packed IS683A format   
      
Prototype:
   AEEGSM1xControl_statusType IGSM1xControl_SetGSM1xPRL
   (
      IGSM1xControl            *instancePtr,
      byte                     *pPRLStruct
   )
      
Parameters:
   instancePtr is a pointer to the IGSM1xControl object.
   
   PRLSizeBytes specifies the size of the provided PRL array in bytes.
   
   packedPRL points to a memory location that contains PRL to be written.
     
Return Value:
   AEEGSM1xControl_statusType - the set of returned values
   are defined by constants whose name starts with the prefix
   AEEGSM1XCONTROL_STATUS_ 
   
Comments:
   None
      
Side Effects:
   None
      
See Also: IGSM1xControl_GetPRL

======================================================================= 

Function:  IGSM1xControl_ValidatePRL

Description:
   Validates the supplied packed PRL.
      
   In order to validate, the supplied PRL must have the same format 
   (IS683A or IS683C) as the phone software.  
   The PRL format does not allow specification of the standard used.
      
Prototype:
   AEEGSM1xControl_statusType IGSM1xControl_ValidatePRL
   (
      IGSM1xControl            *instancePtr,
      byte                     *packedPRL,
      boolean                  *isValid
   )
      
Parameters:
   instancePtr is a pointer to the IGSM1xControl object.
   
   PRLSizeBytes specifies the size of PRL pointed by packedPRL.
    
   packedPRL points to a memory location that contains PRL to be validated.
   
   isValid points to a memory location to receive the information whether
           the supplied PRL is valid or not.  The contents of this location
           are undefined unless AEEGSM1XCONTROL_STATUS_SUCCESS is returned.
     
Return Value:
   AEEGSM1xControl_statusType - the set of returned values
   are defined by constants whose name starts with the prefix
   AEEGSM1XCONTROL_STATUS_ 
   
Comments:
   None
      
Side Effects:
   None
      
See Also: IGSM1xControl_SetPRL


======================================================================= 

Function:  IGSM1xControl_SetGSM1xSIDNIDPairs

Description:
   Set the specified Home and Locked SID/NID pairs in GSM1x NAM in NV.
         
Prototype:
   AEEGSM1xControl_statusType IGSM1xControl_SetGSM1xSIDNIDPairs
   (
       IGSM1xControl                         *pInstance,
       uint16                                 HomeSIDNIDCnt, 
       AEEGSM1xControl_SIDNIDPairType        *HomeSIDNIDPairs,
       uint16                                 LockedSIDNIDCnt, 
       AEEGSM1xControl_SIDNIDPairType        *LockedSIDNIDPairs
   );

         
Parameters:
   instancePtr is a pointer to the IGSM1xControl object.
   
   HomeSIDNIDCnt specifies the number of SID/NID pairs located in the buffer
        pointed by HomeSIDNIDPairs.
   
   HomeSIDNIDPairs points to a memory location that contains home SID/NID pairs.
   
   LockedSIDNIDCnt specifies the number of SID/NID pairs located in the buffer
        pointed by LockedSIDNIDPairs.
   
   LockedSIDNIDPairs points to a memory location that contains locked SID/NID pairs.
   
        
Return Value:
   AEEGSM1xControl_statusType - the set of returned values
   are defined by constants whose name starts with the prefix
   AEEGSM1XCONTROL_STATUS_ 
   
Comments:
   None
      
Side Effects:
   None
      
See Also: 
   IGSM1xControl_GetGSM1xSIDNIDPairs
   
                        
======================================================================= 

Function:  IGSM1xControl_GetGSM1xSIDNIDPairs

Description:
   Retrieve Home and Locked SID/NID pairs stored in GSM1x NAM in NV.
         
Prototype:
    AEEGSM1xControl_statusType IGSM1xControl_GetGSM1xSIDNIDPairs
    (
       IGSM1xControl                         *pInstance,
       uint16                                 HomeSIDNIDMaxCnt, 
       AEEGSM1xControl_SIDNIDPairType        *HomeSIDNIDPairs,
       uint16                                *ActualHomeSIDNIDCnt,
       uint16                                 LockedSIDNIDMaxCnt, 
       AEEGSM1xControl_SIDNIDPairType        *LockedSIDNIDPairs,
       uint16                                *ActualLockedSIDNIDCnt
    );
         
Parameters:
   instancePtr is a pointer to the IGSM1xControl object.
   
   HomeSIDNIDMaxCnt specifies the maximum number of SID/NID pairs that
   can be stored in a memory location pointed by HomeSIDNIDPairs.
   
   HomeSIDNIDPairs points to a memory location to receive Home SID/NID
   pairs. The contents of this location are undefined unless 
   AEEGSM1XCONTROL_STATUS_SUCCESS is returned.
   
   ActualHomeSIDNIDCnt points to a memory location to receive the actual
   count of Home SID/NID pairs written to HomeSIDNIDPairs. The contents 
   of this location are undefined unless AEEGSM1XCONTROL_STATUS_SUCCESS 
   is returned.
   
   LockedSIDNIDMaxCnt specifies the maximum number of SID/NID pairs that
   can be stored in a memory location pointed by LockedSIDNIDPairs.
   
   LockedSIDNIDPairs points to a memory location to receive Locked SID/NID
   pairs. The contents of this location are undefined unless 
   AEEGSM1XCONTROL_STATUS_SUCCESS is returned.
   
   ActualLockedSIDNIDCnt points to a memory location to receive the actual
   count of Home SID/NID pairs written to LockedSIDNIDPairs. The contents 
   of this location are undefined unless AEEGSM1XCONTROL_STATUS_SUCCESS 
   is returned.
 
        
Return Value:
   AEEGSM1xControl_statusType - the set of returned values
   are defined by constants whose name starts with the prefix
   AEEGSM1XCONTROL_STATUS_ 
   
Comments:
   None
      
Side Effects:
   None
      
See Also: 
   IGSM1xControl_SetGSM1xSIDNIDPairs
   
    
======================================================================= 

Function:  IGSM1xControl_GetAvailableModes

Description:
    Returns (as a bit mask) which of the provisioning modes are
    currently available for selection.  This is determined by
    the software build and by the presence and the type of the
    User Identity Module.
         
Prototype:
    AEEGSM1xControl_statusType IGSM1xControl_GetAvailableModes
    (
       IGSM1xControl                         *instancePtr,
       AEEGSM1xControl_modeBitMaskType       *modeMask
    );

         
Parameters:
   instancePtr is a pointer to the IGSM1xControl object.
   
   modeMask points to a memory location to receive the bitmask
      which indicates the supported modes.  The contents 
      of this location are undefined unless AEEGSM1XCONTROL_STATUS_SUCCESS 
      is returned.

   
        
Return Value:
   AEEGSM1xControl_statusType - the set of returned values
   are defined by constants whose name starts with the prefix
   AEEGSM1XCONTROL_STATUS_ 
   
Comments:
   None
      
Side Effects:
   None
      
See Also: 
   None
         
======================================================================= 

Function:  IGSM1xControl_GetCurrentMode

Description:
   Retrieve the current provisioning mode.

         
Prototype:
   AEEGSM1xControl_statusType IGSM1xControl_GetCurrentMode
   (
      IGSM1xControl                         *instancePtr,
      AEEGSM1xControl_modeBitMaskType       *modeMask
   );

         
Parameters:
   instancePtr is a pointer to the IGSM1xControl object.
   
   modeMask points to a memory location to receive the current
   provisioning mode.   The contents of this location are 
   undefined unless AEEGSM1XCONTROL_STATUS_SUCCESS is returned.

   
        
Return Value:
   AEEGSM1xControl_statusType - the set of returned values
   are defined by constants whose name starts with the prefix
   AEEGSM1XCONTROL_STATUS_ 
   
Comments:
   Can be used by the GSM1x Activation App to display the current
   provisioning mode.
      
Side Effects:
   None
      
See Also: 
   None
                                                        
                          
=======================================================================*/

#endif    // AEEGSM1x_CONTROL_H      
