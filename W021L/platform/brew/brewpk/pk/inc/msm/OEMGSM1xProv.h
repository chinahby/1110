#ifndef OEMGSM1XPROV_H
#define OEMGSM1XPROV_H 1

/*======================================================================
FILE: OEMGSM1xProv.h

DESCRIPTION:

     Defines interfaces for GSM1x provisioning using a removable 
     identity card (SIM or R-UIM.)  The interfaces defined
     in this file must be custom implemented by OEMs
     as they are phone and DMSS version specific.
     
     See QUALCOMM document "GSM1x MS Provisioning & Activation", 80-31492-1 X1
     for background information and GSM1x algorithms details.
     
     
              (c) COPYRIGHT 2002 QUALCOMM Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
======================================================================*/

/*----------------------------------------------------------------------
 Include files 
----------------------------------------------------------------------*/
/* None.*/


/*----------------------------------------------------------------------
 Global Constant Declarations
----------------------------------------------------------------------*/
#define OEMGSM1XPROV_1X_RUIM_ONLY             (0)
#define OEMGSM1XPROV_1X_NV_ONLY               (1)
#define OEMGSM1XPROV_1X_RUIM_DROP_BACK_TO_NV  (2)
#define OEMGSM1XPROV_GSM1X                    (3)
  

#define OEMGSM1XPROV_USE_ZERO_ESN             (0)
#define OEMGSM1XPROV_GET_ESN_FROM_RUIM        (1)
#define OEMGSM1XPROV_GET_ESN_FROM_NV          (2)


#define OEMGSM1XPROV_UI_SIG_FOR_UIM           (0x0800)

#if !defined(FEATURE_UIM_RUN_TIME_ENABLE)
#define NV_RTRE_CONFIG_RUIM_ONLY         (0)
#define NV_RTRE_CONFIG_NV_ONLY           (1) 
#define NV_RTRE_CONFIG_RUIM_OR_DROP_BACK (2)
#define NV_RTRE_CONFIG_SIM_ACCESS        (3)
#endif  /* FEATURE_UIM_RUN_TIME_ENABLE */


/* This should be "class" for CRP target and "ACCOLCpClass"
   for most other targets.*/
#define OEMGSM1XPROV_ACCOLC_FIELD_NAME  ACCOLCpClass
                                       
typedef uint16 OEMGSM1xProv_RTREConfigType;

/*----------------------------------------------------------------------
 Global Data Declarations
----------------------------------------------------------------------*/
/* None.*/

/*----------------------------------------------------------------------
 Function Prototypes
----------------------------------------------------------------------*/


boolean  OEMGSM1xProv_ReturnNAMUsedByProvisioningMode
(
    uint16        mode,    
    byte         *pNAM
);

boolean  OEMGSM1xProv_IsModeSupported
(
    uint16        mode
);

#if defined(FEATURE_UIM_RUN_TIME_ENABLE)
void     OEMGSM1xProv_SendRTRECmd
(
    uint16        newRTREConfig
);
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

void     OEMGSM1xProv_SetESNUsage
(
    uint16       ESNUsage
);

void     OEMGSM1xProv_ActivateEmergencyCallOnlyState
(
    void
);

boolean  OEMGSM1xProv_GetRTREConfig
( 
    OEMGSM1xProv_RTREConfigType        *pConfig
);


boolean  OEMGSM1xProv_SetRTREConfig
( 
    OEMGSM1xProv_RTREConfigType         newConfig
);

    

#endif /* OEMGSM1XPROV_H */
