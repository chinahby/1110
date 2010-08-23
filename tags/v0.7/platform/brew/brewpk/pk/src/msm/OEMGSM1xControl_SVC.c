/*===========================================================================

FILE: OEMGSM1xControl_SVC.c

SERVICES: GSM1x control interface.

GENERAL DESCRIPTION
   AEE GSM1x Control Interfaces to activate/deactivate GSM1x mode.
   
Copyright © 2002-2005 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* First include DMSS headers.*/
#include "nv.h"
#include "cm.h"
#include "ui.h"
#include "diag.h"
#include "prl.h"

/* Then include OEM headers.*/
#include "OEMFeatures.h"

/* The whole file depends on the following include.*/                         
#if defined(FEATURE_GSM1x)

#include "OEMClassIDs.h"

#include "OEMGSM1xControl_SVC.h"
#include "OEMGSM1xProv.h"
#include "OEMConfig.h"
#include "OEMGSM1xCardHandler_SVC.h"
#include "OEMFS.h"

#include "AEEStdLib.h"


/*===========================================================================

                      CONSTANTS and ENUMs

===========================================================================*/

#define FARF_ERROR    0
#define FARF_HIGH     0
#define FARF_MED      0
#define FARF_LOW      0

#ifdef FARF
#undef FARF
#endif /* #ifdef FARF */

#if defined(AEE_SIMULATOR)
#define FARF_ALWAYS  1
#else
#define FARF_ALWAYS  0
#endif

#define FARF(x, p)  if (1 == FARF_##x) DBGPRINTF p

#define ASSERT_FARF(x) \
           { \
              if (!(x)) { \
                  FARF(ALWAYS,("assertion %s failed, "__FILE__":%d", #x, __LINE__)); \
              } \
           }


// TODO: replace this message with calls to FARF 
#define OEMGSM1xControl_SVC_MSG_ERROR(__msg,__p1,__p2,__p3) \
        DBGPRINTF((__msg),(__p1),(__p2),(__p3))



#define OEMGSM1xControl_SVC_GSM_ACCOLC_LEN_BYTES            (2)
#define OEMGSM1xControl_SVC_GSM_IMSI_LEN_BYTES              (9)

#define OEMGSM1xControl_SVC_GSM_MSISDN_LEN_BYTES            (255)

/* State definitions for presence/id of a user identitu module.*/
#define CARD_STATE_NO_CARD          (0)
#define CARD_STATE_SAME_SIM         (1)
#define CARD_STATE_NEW_SIM          (2)
#define CARD_STATE_SAME_RUIM_W_GSM  (3)
#define CARD_STATE_NEW_RUIM_W_GSM   (4)
#define CARD_STATE_SAME_RUIM_NO_GSM (5)
#define CARD_STATE_NEW_RUIM_NO_GSM  (6)
#define CARD_STATE_CNT              (7)

/* State definitions for RTRE */
#define RTRE_STATE_INVALID          (0)
#define RTRE_STATE_RUIM_ONLY        (1)
#define RTRE_STATE_NV_ONLY          (2)
#define RTRE_STATE_RUIM_DROPBACK_NV (3)
#define RTRE_STATE_SIM_ACCESS       (4)
#define RTRE_SATTE_CNT              (5)


/*===========================================================================

                      EXTERNAL FUNCTIONS

===========================================================================*/

extern void OEMGSM1xControl_GiveControlToActivationApp
( 
   AEEGSM1xControl_AppActionType AppAction
);

extern void OEMGSM1xControl_SendStatusChange
( 
   AEEGSM1xSig_Status newStatus
);

/*===========================================================================

                      TYPE DEFINITIONS

===========================================================================*/
typedef struct
{
   rex_tcb_type         *waitingThread;
   rex_sigs_type         wakeUpSignal;
   cm_ph_cmd_err_e_type  returnedStatus;
} callbackDataNAMSelectType;


typedef uint16 OEMGSM1xProv_RTREConfigType;

typedef   AEEGSM1xControl_modeBitMaskType  
   (*ActivationAPI_StateTransition)
   ( 
      AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask,
      OEMGSM1xProv_RTREConfigType            RTREconfig,
      boolean                                IsSameCard,
      AEEGSM1xControl_modeBitMaskType        supportedModeMask
   );

/*===========================================================================

                      PUBLIC METHOD DECLARATIONS

These functions are called from BREW applets
===========================================================================*/

#if !defined(FEATURE_UIM_RUN_TIME_ENABLE)
extern void nvruim_set_uim_dir_present(byte dir);
#endif /* ! FEATURE_UIM_RUN_TIME_ENABLE */

/*===========================================================================

                    Protected Functions

These functions can be called from static BREW classes and DMSS/OEM code.
===========================================================================*/


/*===========================================================================

                    Private Functions

These functions are local to this module
===========================================================================*/


/*===========================================================================

                     LOCAL/STATIC DATA

===========================================================================*/

static ActivationAPI_StateTransition   StateTransitionTable[CARD_STATE_CNT][RTRE_SATTE_CNT];

static boolean                         IsCalledFromPowerUp = FALSE;

static CardHandlerImsiBufType          IMSIStruct;
static boolean                         IsIMSIRead = FALSE;

static boolean                         gbInitialized = FALSE;

static AEEGSM1xSig_Status              gStatus = GSM1xSIG_INACTIVE;

/*===========================================================================

                     Static Functions

===========================================================================*/

static boolean readIMSIFromIdCard
(
   uint32         IMSIMaxSize,
   byte          *IMSI,
   uint32        *pIMSISize
);

static boolean readACCOLCFromIdCard
(
   uint32         ACCOLCMaxSize,
   byte          *ACCOLC,
   uint32        *pACCOLCSize
);

static boolean readMSISDNFromIdCard
(
   uint32         MSISDNMaxSize,
   byte          *MSISDN,
   uint32        *pMSISDNSize
);

static boolean convertACCOLCFromGSMToCDMA
(
   uint32          GSMACCOLCSize,
   byte           *GSMACCOLC,
   nv_accolc_type *pCDMAACCOLCStruct
);
                                       
static uint16 convertIMSISTriplet
(
    byte    digits[]
);

static byte convertIMSIPair
(
    byte    digits[]
);

static byte convertIMSIDigit
(
    byte    digit
);

static boolean convertIMSIFromGSMToCDMA
(
   byte                       GSMIMSI[],
   uint32                     GSMIMSISize,
   nv_min1_type              *imsi_s1,
   nv_min2_type              *imsi_s2,
   nv_imsi_11_12_type        *imsi_11_12,
   nv_imsi_mcc_type          *imsi_mcc,
   nv_imsi_addr_num_type     *imsi_addrt,
   byte                      *imsi_class
);

static boolean extractMCCMNCFromPLMN
(
   byte           arrayOfThreeBytes[],
   unsigned int  *MCC,
   unsigned int  *MNC
);

static boolean packMCCMNCIntoPLMN
(
   unsigned int   MCC,
   unsigned int   MNC,
   byte           arrayOfThreeBytes[]
);

static void  cmCmdCallbackStatus 
(
   void                *pUserData,
   cm_ph_cmd_e_type     cmd,
   cm_ph_cmd_err_e_type ph_cmd_err
);

static boolean ResetGSM1xParametersInNV
( 
   void 
);

static boolean readConfigDataFromFile
(
    char    *FileName,
    uint16   MaxBufLen,
    byte    *BufPtr,
    uint16  *ActualLen
);

static boolean writeConfigDataToFile
(
    char    *FileName,
    byte    *BufPtr,
    uint16   BufLen
);

static boolean removeConfigDataFile
(
    char    *FileName
);

static void    InitStateMachine( void );

/* Functions that are needed to implement state transitions.*/
static AEEGSM1xControl_modeBitMaskType    NoAction
( 
    AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask,
    OEMGSM1xProv_RTREConfigType            RTREconfig,
    boolean                                IsSameCard,
    AEEGSM1xControl_modeBitMaskType        supportedModeMask
 
);

static AEEGSM1xControl_modeBitMaskType    GoTo1xNV
(
    AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask,
    OEMGSM1xProv_RTREConfigType            RTREconfig,
    boolean                                IsSameCard,
    AEEGSM1xControl_modeBitMaskType        supportedModeMask
);

static AEEGSM1xControl_modeBitMaskType    GoToGSM1x
(
    AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask,
    OEMGSM1xProv_RTREConfigType            RTREconfig,
    boolean                                IsSameCard,
    AEEGSM1xControl_modeBitMaskType        supportedModeMask
);

static AEEGSM1xControl_modeBitMaskType    BringApp
(
    AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask,
    OEMGSM1xProv_RTREConfigType            RTREconfig,
    boolean                                IsSameCard,
    AEEGSM1xControl_modeBitMaskType        supportedModeMask
); 

static AEEGSM1xControl_modeBitMaskType    GoTo1xRUIMOr1xNV
(
    AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask,
    OEMGSM1xProv_RTREConfigType            RTREconfig,
    boolean                                IsSameCard,
    AEEGSM1xControl_modeBitMaskType        supportedModeMask
);

static AEEGSM1xControl_statusType EnableNewProvisioningMode
( 
   AEEGSM1xControl_modeBitMaskType  mode 
);

/*===========================================================================

                     BREW Methods

===========================================================================*/

/*=============================================================================
FUNCTION: OEMGSM1xControl_SVC_Init                               EXTERNAL FUNCTION

DESCRIPTION:
   This method is called to initialize OEMGSM1xControl_SVC.

PARAMETERS:
   None
   
RETURN VALUE:
   None.
   
COMMENTS:
   
SIDE EFFECTS:
   None.
   
SEE ALSO:
=============================================================================*/
void OEMGSM1xControl_SVC_Init(void)
{
   gbInitialized = TRUE;

   // TODO remove this line before releasing. 
   // For debugging purpose we are setting GSM1xStatus to active.
   gStatus = GSM1xSIG_ACTIVE;
}


/*=============================================================================
FUNCTION: OEMGSM1xControl_SVC_GetStatus                          EXTERNAL FUNCTION 

DESCRIPTION:
    This function is called from IGSM1xSig to obtain the current GSM1x status.
        
RETURN VALUE:
    Current GSM1x status

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xSig_Status OEMGSM1xControl_SVC_GetStatus
(
   void
) 
{
    boolean                      isSuccess;
    OEMGSM1xProv_RTREConfigType  config;

    if (! gbInitialized){
       return GSM1xSIG_INACTIVE;  
    }

    isSuccess = OEMGSM1xProv_GetRTREConfig( &config );
    
    if( 
         isSuccess
         &&
         (NV_RTRE_CONFIG_SIM_ACCESS == config)
      ){

        return GSM1xSIG_ACTIVE;
    }
  
    return GSM1xSIG_INACTIVE;  
}


/*=============================================================================
FUNCTION:    OEMGSM1xControl_SVC_GetSupportedProvisioningModes                EXTERNAL FUNCTION

DESCRIPTION:
   Retrieve which of the provisioning modes are supported by the phone
   software.    
        
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetSupportedProvisioningModes
( 
   AEEGSM1xControl_modeBitMaskType *pModeMask 
)
{
   AEEGSM1xControl_modeBitMaskType modeMask;

   /* Validation of argument(s).*/
   if(NULL == pModeMask){
       return AEEGSM1XCONTROL_STATUS_INVALID_POINTER;
   }

   modeMask = 0;

   if( OEMGSM1xProv_IsModeSupported( OEMGSM1XPROV_1X_RUIM_ONLY ) ){
       modeMask |= AEEGSM1XCONTROL_1X_RUIM_PROV_MASK; 
   }
   if( OEMGSM1xProv_IsModeSupported( OEMGSM1XPROV_1X_NV_ONLY ) ){
       modeMask |= AEEGSM1XCONTROL_1X_NV_PROV_MASK; 
   }
   if( OEMGSM1xProv_IsModeSupported( OEMGSM1XPROV_GSM1X ) ){
       modeMask |= AEEGSM1XCONTROL_GSM1X_PROV_MASK; 
   }

   COPY_TO_USER(pModeMask, &modeMask, sizeof(*pModeMask));

   return AEEGSM1XCONTROL_STATUS_SUCCESS;
}


/*=============================================================================
FUNCTION:    OEMGSM1xControl_SVC_GetUIMUniqueId                   EXTERNAL FUNCTION

DESCRIPTION:
   Returns the ICCId from the card (if any.)    
        
RETURN VALUE:
   AEEGSM1xControl_statusType

SIDE EFFECTS:
   None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetUIMUniqueId
( 
   uint16  maxBufLen,
   byte   *pId,
   uint16 *actualLen
)
{
   CardHandlerICCIdBufType   ICCId;
   uint16                    iccidLength;
   uint32                    status;

   /* Validate argment(s).*/
   if (OEMGSM1xCardHandler_SVC_Init() != TRUE)
   {
      return AEEGSM1XCONTROL_STATUS_INTERNAL_ERROR;
   }
   
   /* Read EFiccid field.*/
   status = OEMGSM1xCardHandler_SVC_ReadICCId( &ICCId );
   if( UIM_PASS != status ){
      return AEEGSM1XCONTROL_STATUS_CANNOT_READ_FROM_UIM;
   }

   if( sizeof(ICCId.data) >  maxBufLen ){
      return AEEGSM1XCONTROL_STATUS_BUF_TOO_SMALL;
   }

   iccidLength = sizeof(ICCId.data);

   COPY_TO_USER(pId, (byte*)(ICCId.data), sizeof(ICCId.data));
   COPY_TO_USER(actualLen, &iccidLength, sizeof(*actualLen));
   
   return AEEGSM1XCONTROL_STATUS_SUCCESS;
}


/*=============================================================================
FUNCTION:    OEMGSM1xControl_SVC_GetDFPresence                    EXTERNAL FUNCTION

DESCRIPTION:
   Returns the ICCId from the card (if any.)    
        
RETURN VALUE:
   AEEGSM1xControl_statusType

SIDE EFFECTS:
   None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetDFPresence
(
   AEEGSM1xControl_DFPresenceBitMaskType *presenceMask
)
{
    static AEEGSM1xControl_DFPresenceBitMaskType dirPresenceMask = 0;
    CardHandlerDirPresenceBitMaskType            cardHandlerBitMask;

    /* Optimization: If dirPresenceMask is not zero, we've been here once
       already.  Directory information is fixed for a particular card,
       there is no point of repeating this process.*/ 
    if(0 != dirPresenceMask)
    {
        COPY_TO_USER(presenceMask, &dirPresenceMask, sizeof(*presenceMask));
        return AEEGSM1XCONTROL_STATUS_SUCCESS;
    }
    
    cardHandlerBitMask = 0;

    if (OEMGSM1xCardHandler_SVC_Init() != TRUE)
    {
       return AEEGSM1XCONTROL_STATUS_INTERNAL_ERROR;
    }

    if(UIM_PASS == OEMGSM1xCardHandler_SVC_GetDirPresence( &cardHandlerBitMask )){

       if( 0 != (OEMGSM1xCardHandler_MF_PRESENT & cardHandlerBitMask) ){
          dirPresenceMask |= AEEGSM1XCONTROL_MF_PRESENT;
       }

       if( 0 != (OEMGSM1xCardHandler_CDMA_DF_PRESENT & cardHandlerBitMask) ){
          dirPresenceMask |= AEEGSM1XCONTROL_CDMA_DF_PRESENT;
       }

#if !defined(FEATURE_UIM_RUN_TIME_ENABLE)
       /* For non-RTRE targets the mode is selected based on the type of the UIM
          card used: R-UIM implies CDMA1x, SIM implies GSM1x.
          Thus, if we a card with a CDMA directory, we ignore that
          GSM directory might also be present.*/ 
       else{
#endif /* ! FEATURE_UIM_RUN_TIME_ENABLE */

       if( 0 != (OEMGSM1xCardHandler_GSM_DF_PRESENT & cardHandlerBitMask) ){
          dirPresenceMask |= AEEGSM1XCONTROL_GSM_DF_PRESENT;
       }
#if !defined(FEATURE_UIM_RUN_TIME_ENABLE)
       }
#endif /* ! FEATURE_UIM_RUN_TIME_ENABLE */

    }

    COPY_TO_USER(presenceMask, &dirPresenceMask, sizeof(*presenceMask));

    return AEEGSM1XCONTROL_STATUS_SUCCESS;
}


/*=============================================================================
FUNCTION:    OEMGSM1xControl_SVC_ProvisionGSM1xParameters         EXTERNAL FUNCTION

DESCRIPTION:
   Provision GSM1x identity-related parameters.
   
RETURN VALUE:
   AEEGSM1xControl_statusType

SIDE EFFECTS:
   None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SVC_ProvisionGSM1xParameters
(
   void
)
{
   static AEEGSM1xIdentityParams   params;

   /* We have to fill out all the field in the structure and then
      use IConfig to write the whole thing to GSM1x NAM in NV.*/

      
   static byte GsmIMSI[OEMGSM1xControl_SVC_GSM_IMSI_LEN_BYTES];
   uint32      GsmIMSISize;

   static byte GsmACCOLC[OEMGSM1xControl_SVC_GSM_ACCOLC_LEN_BYTES];
   uint32      GsmACCOLCSize;
   
   static byte GsmMSISDN[OEMGSM1xControl_SVC_GSM_MSISDN_LEN_BYTES];
   uint32      GsmMSISDNSize;
   
   static nv_item_type       s1_nv_buf; 
   static nv_item_type       s2_nv_buf; 
   static nv_item_type       i_11_12_nv_buf; 
   static nv_item_type       mcc_nv_buf; 
   static nv_item_type       addrt_nv_buf; 
   static byte               imsi_class; 
   static nv_item_type       nv_buf;
   
   AEEGSM1xControl_NAMType   GSM1xNAM;


   OEMGSM1xControl_SVC_MSG_ERROR("OEMGSM1xControl_SVC_ProvisionGSM1xParameters",0,0,0);


   /*-------------------------------- Get GSM1x NAM.*/

   if( ! OEMGSM1xProv_ReturnNAMUsedByProvisioningMode( OEMGSM1XPROV_GSM1X, &GSM1xNAM ) ){
      return AEEGSM1XCONTROL_STATUS_GSM1X_NOT_SUPPORTED;
   }

   params.nam = GSM1xNAM;


   /*-------------------------------- Change RTRE to SIM_ACCESS.*/

   if(! OEMGSM1xProv_SetRTREConfig( NV_RTRE_CONFIG_SIM_ACCESS)){
       return AEEGSM1XCONTROL_STATUS_CANNOT_SET_DESIRED_MODE;
   }
   
   /*-------------------------------- Clean up GSM1x NAM in NV.*/

   (void)ResetGSM1xParametersInNV();
   
   /*-------------------------------- Read GSM IMSI and generate CDMA IMSI */

   if (OEMGSM1xCardHandler_SVC_Init() != TRUE)
   {
      return AEEGSM1XCONTROL_STATUS_INTERNAL_ERROR;
   }

   if( ! readIMSIFromIdCard
         (
            sizeof(GsmIMSI),
            GsmIMSI,
           &GsmIMSISize
         )
     )
   {
        OEMGSM1xControl_SVC_MSG_ERROR("Could not read GSM IMSI",0,0,0);
        return FALSE;
   }

   
   if( ! convertIMSIFromGSMToCDMA
         (
            GsmIMSI, 
            GsmIMSISize,
           (nv_min1_type*)          &s1_nv_buf,
           (nv_min2_type*)          &s2_nv_buf,
           (nv_imsi_11_12_type*)    &i_11_12_nv_buf,
           (nv_imsi_mcc_type*)      &mcc_nv_buf,
           (nv_imsi_addr_num_type*) &addrt_nv_buf,
           &imsi_class
         )
     )
   {
       OEMGSM1xControl_SVC_MSG_ERROR("Could not convert GSM IMSI to CDMA newIMSI",0,0,0);
       return FALSE;
   }
   

   params.imsi_s1[0] = params.t_imsi_s1[0] = s1_nv_buf.min1.min1[0];
   params.imsi_s1[1] = params.t_imsi_s1[1] = s1_nv_buf.min1.min1[1];
   params.imsi_s2[0] = params.t_imsi_s2[0] = s2_nv_buf.min2.min2[0];
   params.imsi_s2[1] = params.t_imsi_s2[1] = s2_nv_buf.min2.min2[1];
   params.mcc        = params.t_mcc = mcc_nv_buf.imsi_mcc.imsi_mcc;
   params.imsi_11_12 = params.t_imsi_11_12 = i_11_12_nv_buf.imsi_11_12.imsi_11_12;
   params.addr_num   = params.t_addr_num = addrt_nv_buf.imsi_t_addr_num.num;
   
   
   /*----------------------------- Read GSM ACC and generate CDMA ACCOLC */
   if( ! readACCOLCFromIdCard
         (
            sizeof(GsmACCOLC),
            GsmACCOLC,
           &GsmACCOLCSize
          )
     )
   {
        OEMGSM1xControl_SVC_MSG_ERROR("Could not read GSM ACCOLC",0,0,0);
        return FALSE;
   }
        
   if( ! convertACCOLCFromGSMToCDMA
         (
            GsmACCOLCSize,
            GsmACCOLC,
           &(nv_buf.accolc)
         )
     )
   {
        OEMGSM1xControl_SVC_MSG_ERROR("Could not convert GSM ACCOLC to CDMA ACCOLC",0,0,0); 
        return FALSE;
   }
 
   OEMGSM1xControl_SVC_MSG_ERROR("GSM ACCOLC (0)=%d (1)=%d",GsmACCOLC[0],GsmACCOLC[1],0);
   OEMGSM1xControl_SVC_MSG_ERROR
   (
       "Derived ACCOLC (0)=%d (1)=%d",
       nv_buf.accolc.OEMGSM1XPROV_ACCOLC_FIELD_NAME[0], 
       nv_buf.accolc.OEMGSM1XPROV_ACCOLC_FIELD_NAME[1],
       0
   );
   params.accolc[0] = nv_buf.accolc.OEMGSM1XPROV_ACCOLC_FIELD_NAME[0];
   params.accolc[1] = nv_buf.accolc.OEMGSM1XPROV_ACCOLC_FIELD_NAME[1];

   
   /*---------------------------Read GSM MSISDN and generate CDMA MDN.*/

   /* Fill with spaces.*/
   MEMSET( GsmMSISDN, ' ', sizeof(GsmMSISDN));
   GsmMSISDNSize = 10; /* In case of disabled MSISDN, we will set it to 10 spaces.*/

   if( ! readMSISDNFromIdCard
         (
            sizeof(GsmMSISDN),
            GsmMSISDN,
           &GsmMSISDNSize
         )
     )
   {
        OEMGSM1xControl_SVC_MSG_ERROR("Could not read GSM MSISDN",0,0,0);
        OEMGSM1xControl_SVC_MSG_ERROR("We are ignoring this since it is an optional parameter.",0,0,0);
        
        /* Intended to do nothing when this happens.*/
   }

   MEMCPY( (char*)params.msisdn, GsmMSISDN, sizeof(params.msisdn) );
   params.num_digits = (uint8)GsmMSISDNSize;
   MEMCPY( (char*)params.digitn, GsmMSISDN, sizeof(params.digitn) );

   
   /* Write all the data into GSM1x NAM in NV.*/
   if( 0 != OEM_SetConfig(CFGI_GSM1X_IDENTITY_PARAMS, (void*) &params, sizeof(params)) )
   {
      return AEEGSM1XCONTROL_STATUS_FAILURE_NV_WRITE;
   }

   OEMGSM1xControl_SVC_MSG_ERROR("Success: OEMGSM1xControl_SVC_ProvisionGSM1xParameters",0,0,0);

   return AEEGSM1XCONTROL_STATUS_SUCCESS;
}


/*=============================================================================
FUNCTION:    ResetGSM1xParametersInNV         EXTERNAL FUNCTION

DESCRIPTION:
   Reset GSM1x Identity parameters to "zero" or "empty" initial state.
   This function is usually called on power up.
   
RETURN VALUE:
   AEEGSM1xControl_statusType

SIDE EFFECTS:
   None
=============================================================================*/
static boolean ResetGSM1xParametersInNV
( 
   void 
)
{
   uint16                          failureCnt;
    
   static AEEGSM1xIdentityParams   idParams;
   AEEGSM1xPRLInfo                 PRLParams;
   AEEGSM1xSIDNIDParams            SIDNIDParams;
   
   AEEGSM1xControl_NAMType   GSM1xNAM;

   uint8                     cleanPRL[8];
   
   uint16                    zeroArray[20];
  
   

   OEMGSM1xControl_SVC_MSG_ERROR("ResetGSM1xParametersInNV",0,0,0);

   /*-------------------------------- Get GSM1x NAM.*/

   if( ! OEMGSM1xProv_ReturnNAMUsedByProvisioningMode( OEMGSM1XPROV_GSM1X, &GSM1xNAM ) ){
      return FALSE;
   }

   /* Initialize all the values to 0.*/
   MEMSET( (char*)&idParams,     0, sizeof(idParams)     );
   MEMSET( (char*)&PRLParams,    0, sizeof(PRLParams)    );
   MEMSET( (char*)&SIDNIDParams, 0, sizeof(SIDNIDParams) );
   MEMSET( (char*)cleanPRL,     0, sizeof(cleanPRL)     );
   MEMSET( (char*)zeroArray,    0, sizeof(zeroArray)    );

         
   idParams.nam = SIDNIDParams.nam = GSM1xNAM;
   
   PRLParams.enabled    = FALSE;

   /* Initilize PRL to be a "clean" prl, that cannot be 0.*/
   cleanPRL[0]           = GSM1xNAM;
   PRLParams.pRoamingListStruct = cleanPRL;

   SIDNIDParams.homePairCnt = 20;
   SIDNIDParams.pHomeSID = zeroArray;
   SIDNIDParams.pHomeNID = zeroArray;

   SIDNIDParams.lockedPairCnt = 10;
   SIDNIDParams.pLockedSID = zeroArray;
   SIDNIDParams.pLockedNID = zeroArray;
   
   failureCnt = 0;
   
   if( 0 != OEM_SetConfig(CFGI_GSM1X_IDENTITY_PARAMS, (void*) &idParams, sizeof(idParams)) ){
     failureCnt++;
   }

   /* Do not clean up PRL for this release.
   if( 0 != OEM_SetConfig(CFGI_GSM1X_PRL, (void*) &PRLParams, sizeof(PRLParams)) ){
     failureCnt++;
   }
   */

   /* Do not clean up SID&NID for this release.
   if( 0 != OEM_SetConfig(CFGI_GSM1X_SID_NID_PARAMS, (void*) &SIDNIDParams, sizeof(SIDNIDParams)) ){
     failureCnt++;
   }
   */

   if( failureCnt > 0 ){
       return FALSE;
   }

   OEMGSM1xControl_SVC_MSG_ERROR("Success: ResetGSM1xParametersInNV",0,0,0); 

   return TRUE;
}


/*=============================================================================
FUNCTION:          OEMGSM1xControl_SVC_EnableGSM1xMode            EXTERNAL FUNCTION

DESCRIPTION:
   Select a new NAM via Call Manager (that will re-do system determination).
    
   This routine can block the execution for a significant amount of time.
    
RETURN VALUE:
   AEEGSM1xControl_statusType

SIDE EFFECTS:
   None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SVC_EnableGSM1xMode
( 
   void
)
{
   return EnableNewProvisioningMode( AEEGSM1XCONTROL_GSM1X_PROV_MASK );
}


/*=============================================================================
FUNCTION:          EnableNewProvisioningMode                  EXTERNAL FUNCTION

DESCRIPTION:
   Select a new NAM via Call Manager (that will re-do system determination).
    
   This routine can block the execution for a significant amount of time.
    
RETURN VALUE:
   AEEGSM1xControl_statusType

SIDE EFFECTS:
   None
=============================================================================*/
static AEEGSM1xControl_statusType EnableNewProvisioningMode
( 
   AEEGSM1xControl_modeBitMaskType mode 
)
{
   int                        failureCnt;
   callbackDataNAMSelectType  cmCallbackData;
   
   byte                       newNAM;
   uint16                     mappedMode;


   cmCallbackData.waitingThread  = NULL;
   cmCallbackData.wakeUpSignal   = UI_NV_SIG;
   cmCallbackData.returnedStatus = CM_PH_CMD_ERR_NOERR;


   switch( mode ){

     case AEEGSM1XCONTROL_GSM1X_PROV_MASK:
        {
            mappedMode = OEMGSM1XPROV_GSM1X;
            break;
        }

     case AEEGSM1XCONTROL_1X_NV_PROV_MASK:
        {
            mappedMode = OEMGSM1XPROV_1X_NV_ONLY;
            break;
        }

     case AEEGSM1XCONTROL_1X_RUIM_PROV_MASK:
        {
            mappedMode = OEMGSM1XPROV_1X_RUIM_ONLY;
            break;
        }

     case AEEGSM1XCONTROL_EMERGENCY_PROV_MASK:
        {
           OEMGSM1xProv_ActivateEmergencyCallOnlyState();
           return AEEGSM1XCONTROL_STATUS_SUCCESS;
        }
     
     default:
        {
           return AEEGSM1XCONTROL_STATUS_INVALID_PARAMETER;
           break;
        }
   } /* switch */


   if( ! OEMGSM1xProv_ReturnNAMUsedByProvisioningMode( mappedMode, &newNAM ) ){
      return AEEGSM1XCONTROL_STATUS_MODE_NOT_SUPPORTED;
   }
   
   OEMGSM1xControl_SVC_MSG_ERROR("SwitchToProvisioningMode %d",(int)newNAM,0,0);

   cmCallbackData.waitingThread = rex_self();
   (void) rex_clr_sigs( cmCallbackData.waitingThread, cmCallbackData.wakeUpSignal );
    
   failureCnt = 0;

   if( ! cm_ph_cmd_nam_sel
         (
            cmCmdCallbackStatus, 
            &cmCallbackData,
            CM_CLIENT_TYPE_UI, 
            newNAM
          )
     )
   {
      OEMGSM1xControl_SVC_MSG_ERROR("NAM re-selection failed", 0, 0, 0);
      failureCnt++;
   }
  
   if( 0 == (cmCallbackData.wakeUpSignal & rex_wait(cmCallbackData.wakeUpSignal) ) ){
      OEMGSM1xControl_SVC_MSG_ERROR("Received wrong signal (bug).", 0, 0, 0);
      failureCnt++;
   }

   if( 
       !( 
      
          (CM_PH_CMD_ERR_NOERR == cmCallbackData.returnedStatus)
          ||
          (
             IsCalledFromPowerUp
             &&
             (CM_CALL_CMD_ERR_OFFLINE_S == cmCallbackData.returnedStatus)
          )
        )
     ){
       OEMGSM1xControl_SVC_MSG_ERROR("NAM re-selection failed with error %d", (int)cmCallbackData.returnedStatus, 0, 0);
       failureCnt++;
   }

   /* If we had a failure during NAM re-selection, 
      we have to restore RTRE config and control.
   */
   if(failureCnt > 0){
       return AEEGSM1XCONTROL_STATUS_FAILURE_NAM_SELECT;
   }

      
   OEMGSM1xControl_SVC_MSG_ERROR("Success: SwitchToProvisioningMode %d",(int)newNAM,0,0);

   gStatus = (AEEGSM1XCONTROL_GSM1X_PROV_MASK == mode) 
                                   ? GSM1xSIG_ACTIVE : GSM1xSIG_INACTIVE;

   OEMGSM1xControl_SendStatusChange(gStatus);

   /* Tell DIAG to update its cached values for ESN and IMSI.*/
   diag_otasp_update();

   IsCalledFromPowerUp = FALSE;
   
   return AEEGSM1XCONTROL_STATUS_SUCCESS;
}


/*=============================================================================
FUNCTION:    OEMGSM1xControl_SVC_GetPLMN                          EXTERNAL FUNCTION

DESCRIPTION:
   Retrieve PLMN information from SIM (or R-UIM card).
    
RETURN VALUE:
   AEEGSM1xControl_statusType

SIDE EFFECTS:
   None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetPLMN
(
   AEEGSM1xControl_PLMNTypeBitMaskType  types,
   uint16                               maxPLMNEntriesCnt, 
   AEEGSM1xControl_PLMNTripletType     *PLMNBuf,  
   uint16                              *actualPLMNEntriesCnt
)
{
   #define     HOME_PLMN_SIZE_BYTES  (3)
   #define     EFAD_MNC_LENGTH_BYTE  (3)

   AEEGSM1xControl_PLMNTripletType PLMNtripletInfo;
      
   uint32                                 status;
   static CardHandlerPLMNSelectorListType PLMNSelStruct;
   static CardHandlerFPLMNListType        FPLMNStruct;
   static CardHandlerADBufType            ADStruct;

   byte                                   HPLMN[HOME_PLMN_SIZE_BYTES];

   unsigned int                           MCC;
   unsigned int                           MNC;

   uint16                                 PLMNEntriesCnt;

   OEMGSM1xControl_SVC_MSG_ERROR("OEMGSM1xControl_SVC_GetPLMN",0,0,0); 

   PLMNEntriesCnt = 0;

   /* Argument validation*/
   if( 
        (NULL == PLMNBuf)
        ||
        (NULL == actualPLMNEntriesCnt)
     )
   {
      return AEEGSM1XCONTROL_STATUS_INVALID_POINTER;
   }

   if(0 == maxPLMNEntriesCnt) 
   {
      return AEEGSM1XCONTROL_STATUS_BUF_TOO_SMALL;
   }

   if (OEMGSM1xCardHandler_SVC_Init() != TRUE)
   {
      return AEEGSM1XCONTROL_STATUS_INTERNAL_ERROR;
   }
   
   /* Read all relevant fields from SIM (R-UIM.)*/           
   if( ! IsIMSIRead )
   {
      status = OEMGSM1xCardHandler_SVC_ReadIMSI( &IMSIStruct );
      if( UIM_PASS != status )
      {
         OEMGSM1xControl_SVC_MSG_ERROR("OEMGSM1xCardHandler_ReadIMSI returned %d",(int)status,0,0);
         return AEEGSM1XCONTROL_STATUS_CANNOT_READ_FROM_UIM;
      }
      IsIMSIRead = TRUE;
   }

   MEMSET( (char*)&ADStruct, 0, sizeof(ADStruct) );
   /* The reading of EFad might fail for older revisions of SIMs.
      If that happens, we are going to fall back to MNC length to be 2.
      (see code below.)*/
   status = OEMGSM1xCardHandler_SVC_ReadAD( &ADStruct );
   if( UIM_PASS != status ){
      OEMGSM1xControl_SVC_MSG_ERROR("OEMGSM1xCardHandler_ReadAD returned %d",(int)status,0,0);
   }
   
   
   /*-----------------------------------------------------------------
             Process Home PLMN if requested.
   -----------------------------------------------------------------*/

   if( 0 != (types & AEEGSM1XCONTROL_HOME_PLMN) ){
   
       /* EFad contains information on whether MNC is 2 or 3 digits.*/
       HPLMN[0]   = (IMSIStruct.imsi[1] & 0xF0) >> 4;
       HPLMN[0]  |= (IMSIStruct.imsi[2] & 0x0F) << 4;

       HPLMN[1] = (IMSIStruct.imsi[2] & 0xF0) >> 4;
   
       
       if(0x3 == (0x3 & ADStruct.data[EFAD_MNC_LENGTH_BYTE]) ){
          HPLMN[1] |=  (IMSIStruct.imsi[4] & 0x0F) << 4;
       }
       else if(0x2 ==  (0x3 & ADStruct.data[EFAD_MNC_LENGTH_BYTE]) ){
          HPLMN[1] |= 0xF0;
       }
       else{
          OEMGSM1xControl_SVC_MSG_ERROR("Invalid len of HomeMNC in EFad, will use 2",0,0,0);
          HPLMN[1] |= 0xF0;
       }
    
       HPLMN[2]  = IMSIStruct.imsi[3] & 0xF0;
       HPLMN[2] |= IMSIStruct.imsi[3] & 0x0F;


       if( ! extractMCCMNCFromPLMN( HPLMN, &MCC, &MNC)){
           return AEEGSM1XCONTROL_STATUS_INTERNAL_ERROR;
       }

       if(PLMNEntriesCnt >= maxPLMNEntriesCnt){
           return AEEGSM1XCONTROL_STATUS_BUF_TOO_SMALL;
       }
   
       PLMNtripletInfo.PLMNtype = AEEGSM1XCONTROL_HOME_PLMN;
       PLMNtripletInfo.MCC = (uint16)MCC;
       PLMNtripletInfo.MNC = (uint16)MNC;

       COPY_TO_USER((byte*)&PLMNBuf[PLMNEntriesCnt], (byte*)&PLMNtripletInfo, sizeof(PLMNtripletInfo));

       PLMNEntriesCnt++;

       OEMGSM1xControl_SVC_MSG_ERROR("HOME MCC=%d MNC=%d",(int)MCC,(int)MNC,0); 
   }
   
   
   /*-----------------------------------------------------------------
             Process PLMN selector, if requested.
   -----------------------------------------------------------------*/
   if( 0 != (types & AEEGSM1XCONTROL_SEL_PLMN) ){


       int   bufIndex;
       status = OEMGSM1xCardHandler_SVC_ReadPLMNSelector( &PLMNSelStruct );
       if( UIM_PASS != status ){
           OEMGSM1xControl_SVC_MSG_ERROR("OEMGSM1xCardHandler_ReadPLMNSelector returned %d",(int)status,0,0);
           return AEEGSM1XCONTROL_STATUS_CANNOT_READ_FROM_UIM;     
       }
       
       for( bufIndex = 0; bufIndex < sizeof(PLMNSelStruct.data); bufIndex += 3){

           /* If the three bytes contain all 'F's, we skip it.*/
           if(
                (0xFF != PLMNSelStruct.data[bufIndex])
                ||
                (0xFF != PLMNSelStruct.data[bufIndex+1])
                ||
                (0xFF != PLMNSelStruct.data[bufIndex+2])
             ){

               if( ! extractMCCMNCFromPLMN( (byte*)&(PLMNSelStruct.data[bufIndex]), &MCC, &MNC)){
                  return AEEGSM1XCONTROL_STATUS_INTERNAL_ERROR;
               }
               
               if(PLMNEntriesCnt >= maxPLMNEntriesCnt){
                  return AEEGSM1XCONTROL_STATUS_BUF_TOO_SMALL;
               }

               PLMNtripletInfo.PLMNtype = AEEGSM1XCONTROL_SEL_PLMN;
               PLMNtripletInfo.MCC = (uint16)MCC;
               PLMNtripletInfo.MNC = (uint16)MNC;
               /*OEMGSM1xControl_SVC_MSG_ERROR("PLMNsel entry=%d type=%d",(int)PLMNEntriesCnt, (int)PLMNtripletInfo.PLMNtype,0);
               OEMGSM1xControl_SVC_MSG_ERROR("PLMNsel MCC=%d MNC=%d",(int)PLMNtripletInfo.MCC,(int)PLMNtripletInfo.MNC,0);
               */
               if( (0 != MCC) || (0 != MNC) )
               {
                  COPY_TO_USER((byte*)&PLMNBuf[PLMNEntriesCnt], (byte*)&PLMNtripletInfo, sizeof(PLMNtripletInfo));
                  PLMNEntriesCnt++;
               }
           }

       }

       OEMGSM1xControl_SVC_MSG_ERROR("After adding PLMNsel, cnt=%d",PLMNEntriesCnt,0,0); 
   }

   /*-----------------------------------------------------------------
             Process Forbidden PLMN
   -----------------------------------------------------------------*/
   if( 0 != (types & AEEGSM1XCONTROL_FORBIDDEN_PLMN) ){

       int   bufIndex;

       status = OEMGSM1xCardHandler_SVC_ReadForbiddenPLMN( &FPLMNStruct );
       if( UIM_PASS != status ){
           OEMGSM1xControl_SVC_MSG_ERROR("OEMGSM1xCardHandler_ReadForbiddenPLMN returned %d",(int)status,0,0);
           return AEEGSM1XCONTROL_STATUS_CANNOT_READ_FROM_UIM;
       }

       for( bufIndex = 0; bufIndex < sizeof(FPLMNStruct.data); bufIndex += 3){

           /* If the three bytes contain all 'F's, we skip it.*/
           if(
                (0xFF != FPLMNStruct.data[bufIndex])
                ||
                (0xFF != FPLMNStruct.data[bufIndex+1])
                ||
                (0xFF != FPLMNStruct.data[bufIndex+2])
             ){

               if( ! extractMCCMNCFromPLMN( (byte*)&(FPLMNStruct.data[bufIndex]), &MCC, &MNC)){
                  return AEEGSM1XCONTROL_STATUS_INTERNAL_ERROR;
               }
               
               if(PLMNEntriesCnt >= maxPLMNEntriesCnt){
                  return AEEGSM1XCONTROL_STATUS_BUF_TOO_SMALL;
               }

               PLMNtripletInfo.PLMNtype = AEEGSM1XCONTROL_FORBIDDEN_PLMN;
               PLMNtripletInfo.MCC = (uint16)MCC;
               PLMNtripletInfo.MNC = (uint16)MNC;
               
               /*OEMGSM1xControl_SVC_MSG_ERROR("FPLMN entry=%d type=%d",(int)PLMNEntriesCnt, (int)PLMNtripletInfo.PLMNtype,0);
               OEMGSM1xControl_SVC_MSG_ERROR("FPLMN MCC=%d MNC=%d",(int)PLMNtripletInfo.MCC,(int)PLMNtripletInfo.MNC,0);
               */

               if( (0 != MCC) || (0 != MNC) )
               {
                   COPY_TO_USER((byte*)&PLMNBuf[PLMNEntriesCnt], (byte*)&PLMNtripletInfo, sizeof(PLMNtripletInfo));
                   PLMNEntriesCnt++;
               }
           }
       }

       OEMGSM1xControl_SVC_MSG_ERROR("After adding FPLMN, cnt=%d",PLMNEntriesCnt,0,0); 
   }

   COPY_TO_USER(actualPLMNEntriesCnt, &PLMNEntriesCnt, sizeof(*actualPLMNEntriesCnt));

   return AEEGSM1XCONTROL_STATUS_SUCCESS;
}


/*=============================================================================
FUNCTION:    OEMGSM1xControl_SVC_SetPLMN                          EXTERNAL FUNCTION

DESCRIPTION:
    
        
RETURN VALUE:
   AEEGSM1xControl_statusType

SIDE EFFECTS:
   None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SVC_SetPLMN
(
   uint16                              PLMNEntriesCnt, 
   AEEGSM1xControl_PLMNTripletType    *PLMNBuf
)
{
   int  entryIndex;
   byte tempArray[3];

   uint32  status;
   AEEGSM1xControl_PLMNTripletType PLMNtripletInfo;

   static  CardHandlerPLMNSelectorListType PLMNSelStruct;
   static  CardHandlerFPLMNListType        FPLMNStruct;

   uint32  PLMNSelArrayIndex = 0;
   uint32  FPLMNArrayIndex   = 0;

   OEMGSM1xControl_SVC_MSG_ERROR("OEMGSM1xControl_SVC_SetPLMN",0,0,0); 

   if (OEMGSM1xCardHandler_SVC_Init() != TRUE)
   {
      return AEEGSM1XCONTROL_STATUS_INTERNAL_ERROR;
   }

   MEMSET( (byte*)&PLMNSelStruct, 0xFF, sizeof(PLMNSelStruct));
   MEMSET( (byte*)&FPLMNStruct,   0xFF, sizeof(FPLMNStruct));

   
   for( entryIndex = 0; entryIndex < PLMNEntriesCnt; entryIndex++ )
   {
       COPY_FROM_USER((byte*)&PLMNtripletInfo, (byte*)&PLMNBuf[entryIndex], sizeof(PLMNtripletInfo));
       
       switch( PLMNtripletInfo.PLMNtype ){
       
          case AEEGSM1XCONTROL_SEL_PLMN:
              {
                  if(! packMCCMNCIntoPLMN( PLMNtripletInfo.MCC, PLMNtripletInfo.MNC, tempArray )){
                      return AEEGSM1XCONTROL_STATUS_INVALID_PARAMETER;
                  }
    
                  if( PLMNSelArrayIndex < sizeof(PLMNSelStruct.data) ){

                     PLMNSelStruct.data[PLMNSelArrayIndex]   = tempArray[0];
                     PLMNSelStruct.data[PLMNSelArrayIndex+1] = tempArray[1];
                     PLMNSelStruct.data[PLMNSelArrayIndex+2] = tempArray[2];
                     
                     PLMNSelArrayIndex += 3;
                  }
                  
                  break;
              }
          
          case AEEGSM1XCONTROL_FORBIDDEN_PLMN:
              {
                  if(! packMCCMNCIntoPLMN( PLMNtripletInfo.MCC, PLMNtripletInfo.MNC, tempArray )){
                      return AEEGSM1XCONTROL_STATUS_INVALID_PARAMETER;
                  }
    
                  if( FPLMNArrayIndex < sizeof(FPLMNStruct.data) ){

                     FPLMNStruct.data[FPLMNArrayIndex]   = tempArray[0];
                     FPLMNStruct.data[FPLMNArrayIndex+1] = tempArray[1];
                     FPLMNStruct.data[FPLMNArrayIndex+2] = tempArray[2];
                     
                     FPLMNArrayIndex += 3;
                  }
                  
                  break;
              }

          case AEEGSM1XCONTROL_HOME_PLMN:
             /* Do nothing, we cannot update GSMimsi.*/
             break;
       
          default:
             /* Should never get here.*/
             return AEEGSM1XCONTROL_STATUS_INVALID_PARAMETER;
       } /* switch */
   
   } /* for */

   
   /* Write PLMN Selector if any entries were present.*/
   if( PLMNSelArrayIndex > 0 ){
       status = OEMGSM1xCardHandler_SVC_WritePLMNSelector( &PLMNSelStruct );
       if( UIM_PASS != status ){
           OEMGSM1xControl_SVC_MSG_ERROR("OEMGSM1xCardHandler_WritePLMNSelector returned %d",(int)status,0,0);
           return AEEGSM1XCONTROL_STATUS_CANNOT_WRITE_TO_UIM;     
       }
   }

   /* Write Forbidden PLMN if any etnries were present.*/
   if( FPLMNArrayIndex > 0 ){
   
       status = OEMGSM1xCardHandler_SVC_WriteForbiddenPLMN( &FPLMNStruct );
       if( UIM_PASS != status ){
           OEMGSM1xControl_SVC_MSG_ERROR("OEMGSM1xCardHandler_WriteForbiddenPLMN returned %d",(int)status,0,0);
           return AEEGSM1XCONTROL_STATUS_CANNOT_WRITE_TO_UIM;
       }
   }

   OEMGSM1xControl_SVC_MSG_ERROR("Success: OEMGSM1xControl_SVC_SetPLMN",0,0,0); 
   
   return AEEGSM1XCONTROL_STATUS_SUCCESS;
}


/*=============================================================================
FUNCTION:    OEMGSM1xControl_SVC_GetGSM1xPRL                      EXTERNAL FUNCTION

DESCRIPTION:
    Read IS683A PRL from GSM1x NAM in NV into the provided buffer.
    
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetGSM1xPRL
(
    uint16  maxPRLSizeBytes,
    byte   *packedPRL
)
{
    AEEGSM1xControl_NAMType   GSM1xNAM;
    AEEGSM1xPRLInfo           PRLInfo;

    byte                     *NAMPtr;
  
    /*-------------------------------- Get GSM1x NAM.-----------------*/
    if( ! OEMGSM1xProv_ReturnNAMUsedByProvisioningMode( OEMGSM1XPROV_GSM1X, &GSM1xNAM ) )
    {
       return AEEGSM1XCONTROL_STATUS_GSM1X_NOT_SUPPORTED;
    }

    /* Fill GSM1xNAM into PRL structure.*/
    NAMPtr  = (byte*)packedPRL;
   *NAMPtr  = GSM1xNAM;

    /* Specify the receiving buffer's memory location and max length.*/
    PRLInfo.maxSizeBytes       = maxPRLSizeBytes;
    PRLInfo.pRoamingListStruct = packedPRL;

    if( 0 != OEM_GetConfig(CFGI_GSM1X_PRL, (void*) &PRLInfo, sizeof(PRLInfo)) )
    {
       return AEEGSM1XCONTROL_STATUS_BUF_TOO_SMALL;
    }

    OEMGSM1xControl_SVC_MSG_ERROR("Success: OEMGSM1xControl_SVC_GetGSM1xPRL=%d",0,0,0);

    return  AEEGSM1XCONTROL_STATUS_SUCCESS;
}


/*=============================================================================
FUNCTION:    OEMGSM1xControl_SVC_SetGSM1xPRL                      EXTERNAL FUNCTION

DESCRIPTION:
    Write the supplied IS683A PRL into GSM1x NAM in NV.
            
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SVC_SetGSM1xPRL
(
    uint16                    PRLStructsize,
    byte                     *pPRLStruct
)
{
    AEEGSM1xControl_NAMType   GSM1xNAM;
    AEEGSM1xPRLInfo           PRLInfo;
    uint16                    prlVersion;

    byte                     *NAMPtr;
    
    OEMGSM1xControl_SVC_MSG_ERROR("OEMGSM1xControl_SVC_SetGSM1xPRL=%d",0,0,0);

    /*-------------------------------- Get GSM1x NAM.*/

    if( ! OEMGSM1xProv_ReturnNAMUsedByProvisioningMode( OEMGSM1XPROV_GSM1X, &GSM1xNAM ) )
    {
       return AEEGSM1XCONTROL_STATUS_GSM1X_NOT_SUPPORTED;
    }
    
    if( PRL_VALID != prl_validate( pPRLStruct + PACKED_PRL_OFFSET, &prlVersion) )
    {
       return AEEGSM1XCONTROL_STATUS_INVALID_PRL;
    }
    
    NAMPtr                     = (byte*)pPRLStruct;
   *NAMPtr                     = GSM1xNAM;
  
    PRLInfo.pRoamingListStruct = pPRLStruct;

    /* Initialize PRL_EMABLED field in NV.*/
    PRLInfo.enabled            = TRUE;
           
    if( 0 != OEM_SetConfig(CFGI_GSM1X_PRL, (void*) &PRLInfo, sizeof(PRLInfo)) ){
       return AEEGSM1XCONTROL_STATUS_FAILURE_NV_READ;
    }
    
    OEMGSM1xControl_SVC_MSG_ERROR("Success: OEMGSM1xControl_SVC_SetGSM1xPRL=%d",0,0,0);

    return  AEEGSM1XCONTROL_STATUS_SUCCESS;
}


/*=============================================================================
FUNCTION:    OEMGSM1xControl_SVC_ValidatePRL                      EXTERNAL FUNCTION

DESCRIPTION:
    Validate the supplied PRL bytestream.
        
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SVC_ValidatePRL
(
   uint16                                 packedPRLsize,
   byte                                  *packedPRL,
   boolean                               *isValid
)
{
    uint16                      prlVersion;

    OEMGSM1xControl_SVC_MSG_ERROR("OEMGSM1xControl_SVC_ValidatePRL",0,0,0);    
                               
    *isValid = FALSE;

    if( PRL_VALID == prl_validate( packedPRL, &prlVersion) ){
       *isValid = TRUE;
    }

    if(*isValid) {
        OEMGSM1xControl_SVC_MSG_ERROR("PRL is Valid",0,0,0);
    }
    else{
        OEMGSM1xControl_SVC_MSG_ERROR("PRL is Invalid",0,0,0);
    }

    return  AEEGSM1XCONTROL_STATUS_SUCCESS;
}


/*======================================================================
FUNCTION:   readIMSIFromIdCard

DESCRIPTION:
   Read IMSI into the provided array from an
   identity card.

RETURN VALUE:
   boolean completion status

SIDE EFFECTS:
   None
======================================================================*/
static boolean readIMSIFromIdCard
(
   uint32      IMSIMaxSize,
   byte       *IMSIBuf,
   uint32     *pIMSISize
)
{
   uint32                           status;
   
   /* Argument validation*/
   if( 
        (NULL == IMSIBuf)
        ||
        (NULL == pIMSISize)
     ){
      OEMGSM1xControl_SVC_MSG_ERROR("Unexpected NULL pointer.",0,0,0);
      return FALSE;
   }

   if( 0 == IMSIMaxSize ){
      OEMGSM1xControl_SVC_MSG_ERROR("Max size cannot be zero.",0,0,0);
      return FALSE;
   }

   
   if( ! IsIMSIRead ){
      status = OEMGSM1xCardHandler_SVC_ReadIMSI( &IMSIStruct );
      if( UIM_PASS != status ){
         OEMGSM1xControl_SVC_MSG_ERROR("OEMGSM1xCardHandler_ReadIMSI returned %d",(int)status,0,0);
         return FALSE;
      }
      IsIMSIRead = TRUE;
   }

   MEMCPY(IMSIBuf, (byte*)(IMSIStruct.imsi), sizeof(IMSIStruct.imsi));
  *pIMSISize = sizeof(IMSIStruct.imsi);
                      
   return TRUE;
}


/*======================================================================
FUNCTION:   readACCOLCFromIdCard

DESCRIPTION:
   Read Access Control Class information into the provided array 
   from an identity card.

RETURN VALUE:
   boolean completion status

SIDE EFFECTS:
   None
======================================================================*/
static boolean readACCOLCFromIdCard
(
   uint32      ACCOLCMaxSize,
   byte       *ACCOLC,
   uint32     *pACCOLCSize
)
{
   uint32 status;
   static CardHandlerAaccessClassBufType      ACCOLCStruct;

   /* Argument validation*/
   if( 
        (NULL == ACCOLC)
        ||
        (NULL == pACCOLCSize)
     ){
      OEMGSM1xControl_SVC_MSG_ERROR("Unexpected NULL pointer.",0,0,0);
      return FALSE;
   }

   if( 0 == ACCOLCMaxSize ){
      OEMGSM1xControl_SVC_MSG_ERROR("Max size cannot be zero.",0,0,0);
      return FALSE;
   }

   if( ACCOLCMaxSize < sizeof(ACCOLCStruct.data)){
      OEMGSM1xControl_SVC_MSG_ERROR("Provided buffer is too small.",0,0,0);
      return FALSE;
   }
   

   status = OEMGSM1xCardHandler_SVC_ReadAccessClass( &ACCOLCStruct );
   if( UIM_PASS != status ){
       OEMGSM1xControl_SVC_MSG_ERROR("OEMGSM1xCardHandler_ReadAccessClass returned %d",(int)status,0,0);
       return FALSE;
   }

   MEMCPY(ACCOLC, (byte*)(ACCOLCStruct.data), sizeof(ACCOLCStruct.data));
  *pACCOLCSize = sizeof(ACCOLCStruct.data);
                 
   return TRUE;
}


/*======================================================================
FUNCTION:   readMSISDNFromIdCard

DESCRIPTION:
   Read Mobile Subscriber Directory Number (MSISDN) information 
   into the provided array from an identity card.
   
RETURN VALUE:
   boolean completion status

SIDE EFFECTS:
   None
======================================================================*/
static boolean readMSISDNFromIdCard
(
   uint32      MSISDNMaxSize,
   byte       *MSISDN,
   uint32     *pMSISDNSize
)
{
   uint32 status;
   uint8  alphaIdLength;
   static CardHandlerMSISDNBufType      MSIDSNStruct;
   uint8  numLenInBytes;
   int    MSISDNDigCnt;
   int    byteInd;
   int    currentOffset;

#define MAX_LEN_GSM_MSISDN_DIAL_NUM_IN_BYTES (10)  
#define DIAL_NUM_OFFSET (2)
      
   /* Argument validation*/
   if( 
        (NULL == MSISDN)
        ||
        (NULL == pMSISDNSize)
     ){
      OEMGSM1xControl_SVC_MSG_ERROR("Unexpected NULL pointer.",0,0,0);
      return FALSE;
   }

   if( 0 == MSISDNMaxSize ){
      OEMGSM1xControl_SVC_MSG_ERROR("Max size cannot be zero.",0,0,0);
      return FALSE;
   }

   if( MSISDNMaxSize < sizeof(MSIDSNStruct.data)){
      OEMGSM1xControl_SVC_MSG_ERROR("Provided buffer is too small.",0,0,0);
      return FALSE;
   }
   
   status = OEMGSM1xCardHandler_SVC_ReadMSISDN( &MSIDSNStruct, 0,  &alphaIdLength );
   if( UIM_PASS != status ){
       OEMGSM1xControl_SVC_MSG_ERROR("OEMGSM1xCardHandler_ReadMSISDN returned %d",(int)status,0,0);
       return FALSE;
   }

   /* Get the length of dialing number in bytes.*/
   numLenInBytes = MSIDSNStruct.data[ alphaIdLength ] ;

   /* Convert the supplied stream to digits.*/
   MSISDNDigCnt = 0;
   for( byteInd = 0; byteInd < MAX_LEN_GSM_MSISDN_DIAL_NUM_IN_BYTES; byteInd++ ){

       currentOffset = alphaIdLength + DIAL_NUM_OFFSET + byteInd;

       /* Extract first digit from the byte.*/
       if( 0x0F != (MSIDSNStruct.data[ currentOffset ] & 0x0F) ){
          sprintf( (char*)(&MSISDN[MSISDNDigCnt]), "%c", MSIDSNStruct.data[ currentOffset ]  & 0x0F );
          MSISDNDigCnt++;
          }
       else{
          /* 0xF represents the end of the digits.*/
          break;
       }
       
       /* Extract the second digit from the byte.*/
       if( 0xF0 != (MSIDSNStruct.data[ currentOffset ] & 0xF0) ){
          sprintf( (char*)(&MSISDN[MSISDNDigCnt]), "%c", ( MSIDSNStruct.data[ currentOffset ] & 0xF0) >> 4 ); 
          MSISDNDigCnt++;
       }
       else{
          /* 0xF represents the end of the digits.*/
          break;
       }
    }
    
    *pMSISDNSize = (uint32)MSISDNDigCnt;
                 
    return TRUE;
}


/*======================================================================
FUNCTION:   convertACCOLCFromGSMToCDMA

DESCRIPTION:
   The name says it all.
   
RETURN VALUE:
   boolean 

SIDE EFFECTS:
   None
======================================================================*/
static boolean convertACCOLCFromGSMToCDMA
(
   uint32          GSMACCOLCSize,
   byte            GSMACCOLC[],
   nv_accolc_type *pCDMAACCOLCStruct
)
{
#define  SECURITY_SERVICES_MASK  (0x10)  /*GSM ACCOLC 12*/
#define  PUBLIC_UTILITIES_MASK   (0x20)  /*GSM ACCOLC 13*/
#define  EMERGENCY_SERVICES_MASK (0x40) /*GSM ACCOLC 14*/
#define  PLMN_USE                (0x08)  /*GSM ACCOLC 11*/
#define  PLMNG_STAFF             (0x80) /*GSM ACCOLC 15*/

   MEMSET( (char*)pCDMAACCOLCStruct, 0, sizeof(*pCDMAACCOLCStruct) );


   if( 
       (GSMACCOLC[0] & SECURITY_SERVICES_MASK)
       ||
       (GSMACCOLC[0] & PUBLIC_UTILITIES_MASK)
       ||
       (GSMACCOLC[0] & EMERGENCY_SERVICES_MASK)
     ){

      pCDMAACCOLCStruct->OEMGSM1XPROV_ACCOLC_FIELD_NAME[0] = 11;
   }
   else if(
            (GSMACCOLC[0] & PLMN_USE)
            ||
            (GSMACCOLC[0] & PLMNG_STAFF)
          ){
      pCDMAACCOLCStruct->OEMGSM1XPROV_ACCOLC_FIELD_NAME[0] = 10;
   }
   else if(
            (GSMACCOLC[0] & 0x03)
            ||
            (GSMACCOLC[1] & 0xFF)
          ){
      
      /* Set CDMA ACCOLC class 0 through 9
         to be the same as GSM ACCOLC class (0-9).*/
      
       /* Handle classes 9 and 8.*/
      if( GSMACCOLC[0] & 0x02 ){
          pCDMAACCOLCStruct->OEMGSM1XPROV_ACCOLC_FIELD_NAME[0] = 9;
      }
      else if( GSMACCOLC[0] & 0x01 ){
          pCDMAACCOLCStruct->OEMGSM1XPROV_ACCOLC_FIELD_NAME[0] = 8;
      }
      else{ /* Handle classes 7 through 0.*/
          int  classInd;
          for( classInd = 7; classInd >=0; classInd-- ){
              if( GSMACCOLC[1] & (0x01 << classInd) ){
                  pCDMAACCOLCStruct->OEMGSM1XPROV_ACCOLC_FIELD_NAME[0] = classInd;
                  break;
              }
          } /*for*/
      }
   }


   pCDMAACCOLCStruct->OEMGSM1XPROV_ACCOLC_FIELD_NAME[1] 
       = pCDMAACCOLCStruct->OEMGSM1XPROV_ACCOLC_FIELD_NAME[0];
  
  
   /* GSM ACCOLC is not assigned.*/
   return TRUE;
}

            
/*======================================================================
FUNCTION:   convertIMSISTriplet

DESCRIPTION:
   Convert three digits specified as binary numbers
   of an array into a 10 bits byte array
   according to IS-2000.5-A-1, 2.3.1.1, p.2-3.
      
RETURN VALUE:
   result in binary form (only 10 bits are significant.)

SIDE EFFECTS:
   None
======================================================================*/
static uint16 convertIMSISTriplet
(
    byte    digits[]
)
{
   byte   ind;
   uint16 result;
   
   for( ind = 0; ind < 3; ind++ ){
      if( 0 == digits[ind] ){
          digits[ind] = 10;
      }
   }

   result = 100 * digits[0] + 10 * digits[1] + digits[2] - 111;
   return result;
}


/*======================================================================
FUNCTION:   convertIMSIPair

DESCRIPTION:
   Convert two digits specified as binary numbers
   of an array into a 10 bits byte array
   according to IS-2000.5-A-1, 2.3.1.2, p.2-6.

RETURN VALUE:
   result in binary form (7 bits are significant)

SIDE EFFECTS:
   None
======================================================================*/
static byte convertIMSIPair
(
    byte    digits[]
)
{
   byte ind;
   byte result;
   
   for( ind = 0; ind < 2; ind++ ){
      if( 0 == digits[ind] ){
          digits[ind] = 10;
      }
   }

   result = 10 * digits[0] + digits[1] - 11;
   return result;
}


/*======================================================================
FUNCTION:   convertIMSIDigit

DESCRIPTION:
   Convert a fourth digit of IMSI_S_1
   of an array into a 10 bits byte array
   according to IS-2000.5-A-1, 2.3.1.1-2.
      
RETURN VALUE:
   result in binary form (4 bits are significant)

SIDE EFFECTS:
   None
======================================================================*/
static byte convertIMSIDigit
(
    byte    digit

)
{
   return ((0 == digit) ? 10 : digit);
}


/*======================================================================
FUNCTION:   convertIMSIFromGSMToCDMA

DESCRIPTION:
   Convert IMSI from GSM to CDMA.
   
   The tricky part of this conversion happens if GSM IMSI has less 
   than 15 digits.  This is done in accordance with 
   CDMA document TIA/EIA/IS-2000.5-A-1, section 2.3.
      
RETURN VALUE:
   boolean (completion status, TRUE=success)

SIDE EFFECTS:
   None
======================================================================*/
static boolean convertIMSIFromGSMToCDMA
(
   byte                       GSMIMSI[],
   uint32       GSMIMSISize,
   nv_min1_type              *imsi_s1,
   nv_min2_type              *imsi_s2,
   nv_imsi_11_12_type        *imsi_11_12,
   nv_imsi_mcc_type          *imsi_mcc,
   nv_imsi_addr_num_type     *imsi_addrt,
   byte                      *imsi_class
)
{

#define IMSI_MAX_DIGIT_CNT      (15)

/* Width of the fields in GSM IMSI.*/   
#define MCC_DIGIT_CNT           (3)
#define I_11_12_DIGIT_CNT       (2)
#define S2_DIGIT_CNT            (3)
#define S1_TRIPLET_DIGIT_CNT    (3)
#define S1_THOUSANDS_DIGIT_CNT  (1)

/* Offsets of the fields in GSM IMSI*/    
#define MCC_OFFSET           (0)
#define IMSI_11_12_OFFSET    (MCC_OFFSET        + MCC_DIGIT_CNT)

#define IMSI_MINIMUM_DIGIT_CNT   (5)

/* Witdh of packed fields in CDMA2000 IMSI.*/   
#define IMSI_TRIPLET_PACKED_SIZE_BITS   (10)
#define IMSI_PAIR_PACKED_SIZE_BITS      (8)
#define IMSI_THOUSANDS_PACKED_SIZE_BITS (4)


    byte    GSMIMSIDigitsCnt; 
    byte    GSMIMSIDecDigits[IMSI_MAX_DIGIT_CNT];
    byte    byteInd;
    byte    digitInd;
    uint16  temp16;
    
    dword   tempdword;

    /* Validation of argument(s).*/
    if( NULL == GSMIMSI ){
        OEMGSM1xControl_SVC_MSG_ERROR("NULL pointer is not allowed here.",0,0,0);
        return FALSE;
    }

    /* Verify GSM IMSI length.*/
    if(
        (0 == GSMIMSI[0])
        ||
        (8 <  GSMIMSI[0])
      ){
        OEMGSM1xControl_SVC_MSG_ERROR("Wrong length of GSM IMSI",0,0,0);
        return FALSE;
    }

    /* Verify GSM IMSI prefix.*/
    if( 0x01 != (GSMIMSI[1] & 0x07) ){
        OEMGSM1xControl_SVC_MSG_ERROR("Wrong prefix of GSM IMSI",0,0,0);
        return FALSE;
    }

    /* Verify parity of GSM IMSI.*/
    /* TBD.*/
    
    /* Convert the supplied stream to digits.*/
    GSMIMSIDigitsCnt = 0;
    for( byteInd = 1; byteInd <= GSMIMSI[0]; byteInd++ ){
       /* Extract first digit from the byte.
          Skip the first digit from the first byte, 
          it is actually a prefix + parity.*/
       if( 1 != byteInd){

          if( 0x0F != (GSMIMSI[byteInd] & 0x0F) ){
             GSMIMSIDecDigits[GSMIMSIDigitsCnt] = GSMIMSI[byteInd] & 0x0F;
             GSMIMSIDigitsCnt++;
          }
          else{
             /* 0xF represents the end of the digits.*/
             break;
          }
       }

       /* Extract the second digit from the byte.*/
       if( 0xF0 != (GSMIMSI[byteInd] & 0xF0) ){
          GSMIMSIDecDigits[GSMIMSIDigitsCnt] = (GSMIMSI[byteInd] & 0xF0) >> 4;
          GSMIMSIDigitsCnt++;
       }
       else{
          /* 0xF represents the end of the digits.*/
          break;
       }
    }

    
    if( GSMIMSIDigitsCnt < IMSI_MINIMUM_DIGIT_CNT ){
       OEMGSM1xControl_SVC_MSG_ERROR("GSM IMSI is less than %d digits.",(int)IMSI_MINIMUM_DIGIT_CNT,0,0);
       return FALSE;
    }

    MEMSET( (char*)imsi_s1,    0, sizeof(*imsi_s1)    );
    MEMSET( (char*)imsi_s2,    0, sizeof(*imsi_s2)    );
    MEMSET( (char*)imsi_11_12, 0, sizeof(*imsi_11_12) );
    MEMSET( (char*)imsi_mcc,   0, sizeof(*imsi_mcc)   );
    MEMSET( (char*)imsi_addrt, 0, sizeof(*imsi_addrt) );
    
    /* Set IMSI class.*/
    *imsi_class = (IMSI_MAX_DIGIT_CNT == GSMIMSIDigitsCnt) ? 0 : 1;

    /* Set ADDR_NUM */
    /* See section 2.3.1of IS-2000.5-A-1*/
    if( 1 == (*imsi_class)){
        imsi_addrt->num =  (GSMIMSIDigitsCnt - MCC_DIGIT_CNT) - 4;
    }
    else{
        /* This is not necessary, but we don't like to leave
           it uninitialized.*/
        imsi_addrt->num = 0xFF;
    }
    
    /* Set MCC.*/
    /* MCC is the first three digits.  We don't get here unless we have at least 3 digits.*/
    imsi_mcc->imsi_mcc = convertIMSISTriplet( &(GSMIMSIDecDigits[ MCC_OFFSET]) );
    OEMGSM1xControl_SVC_MSG_ERROR("Successfully converted GSM IMSI into CDMA IMSI T.",0,0,0);

    /* Set IMSI_11_12.  We pad these with zeroes if IMSI has less than 12 digits.*/
    {
       byte  imsi11_12_digits[2] = { 0, 0};

       if(GSMIMSIDigitsCnt >= 12){
          imsi11_12_digits[0] = GSMIMSIDecDigits[ GSMIMSIDigitsCnt - 12 ];
          imsi11_12_digits[1] = GSMIMSIDecDigits[ GSMIMSIDigitsCnt - 11 ];
       }
       else{
          imsi11_12_digits[0] = 0;
          imsi11_12_digits[1] = (GSMIMSIDigitsCnt < 11) ? 0 : GSMIMSIDecDigits[ 0 ];  
       }

       imsi_11_12->imsi_11_12 = convertIMSIPair( imsi11_12_digits );
       OEMGSM1xControl_SVC_MSG_ERROR("Successfully generated IMSI_11_12 =%d.",(int)imsi_11_12->imsi_11_12,0,0);
    }


    /* Set IMSI_S fields.*/
    {
#define  IMSI_S_DIGITS_CNT    (10)

#define  IMSI_S2_OFFSET       (0)
#define  IMSI_S1_OFFSET_1     (IMSI_S2_OFFSET    + S2_DIGIT_CNT)
#define  IMSI_S1_OFFSET_2     (IMSI_S1_OFFSET_1  + S1_TRIPLET_DIGIT_CNT)
#define  IMSI_S1_OFFSET_3     (IMSI_S1_OFFSET_2  + S1_THOUSANDS_DIGIT_CNT)
        
        byte   imsi_s_digits[IMSI_S_DIGITS_CNT];
        int    available_imsi_dig_cnt = (GSMIMSIDigitsCnt > IMSI_S_DIGITS_CNT) ? IMSI_S_DIGITS_CNT : GSMIMSIDigitsCnt;

        MEMSET( (char*)imsi_s_digits, 0, sizeof(imsi_s_digits)    );
        
        /* Copy up to 10 IMSI digits, starting from the last.
           If we have less than 10 digits, we automatically get
           padding with zero of the most significant digits.*/

        for( digitInd = 1; digitInd <= available_imsi_dig_cnt; digitInd++ ){
           imsi_s_digits[ IMSI_S_DIGITS_CNT - digitInd] = 
                GSMIMSIDecDigits[ GSMIMSIDigitsCnt - digitInd];
        }

        /* Compute and pack IMSI_S2*/
        temp16 = convertIMSISTriplet( &(imsi_s_digits[ IMSI_S2_OFFSET]) );
    
        imsi_s2->min2[0] = imsi_s2->min2[1] = temp16;
          
    
        /* Compute and pack IMSI_S1*/
        /* Process the first three digits of IMSI_S1.*/
        tempdword = convertIMSISTriplet( &(imsi_s_digits[IMSI_S1_OFFSET_1 ]));
        tempdword <<= 14;

        imsi_s1->min1[0] = tempdword;
       
    
        tempdword = convertIMSIDigit(imsi_s_digits[IMSI_S1_OFFSET_2]);
        tempdword <<= 10;

        imsi_s1->min1[0] |= tempdword;


        /* Process the last three digits of IMSI_S1.*/
        tempdword = convertIMSISTriplet( &(imsi_s_digits[IMSI_S1_OFFSET_3 ]));
        imsi_s1->min1[0] |= tempdword;

    
        imsi_s1->min1[1] = imsi_s1->min1[0];
    }
    
    return TRUE;
}


/*=============================================================================
FUNCTION: cmCmdCallbackStatus 

DESCRIPTION:
    Callback to return status from CM command (nam selection.)     

RETURN VALUE:
    N/A

SIDE EFFECTS:
    None
=============================================================================*/
static void  cmCmdCallbackStatus 
(
   void                *pUserData,
   cm_ph_cmd_e_type     cmd,
   cm_ph_cmd_err_e_type errorCode
)
{
   callbackDataNAMSelectType   *pCallbackData;
    
   pCallbackData = (callbackDataNAMSelectType*)pUserData;
     
   if(NULL == pCallbackData){
      OEMGSM1xControl_SVC_MSG_ERROR("NULL is not allowed here.",0,0,0);
      return;
   }

   pCallbackData->returnedStatus  = errorCode;

   if( CM_PH_CMD_ERR_NOERR != errorCode){
      OEMGSM1xControl_SVC_MSG_ERROR("CallManager cmd %d returned status %d",(int)cmd,(int)errorCode,0);
   }

   (void) rex_set_sigs( pCallbackData->waitingThread, pCallbackData->wakeUpSignal);
}


/*======================================================================
FUNCTION:   extractMCCMNCFromPLMN

DESCRIPTION:
   Decode MCC and MNC from a 3-byte array into numerical values.
   
   The three bytes are
       0xPQ 0xRS 0xTU,
   where
      Q is the first digit of MCC
      P is the second digit of MCC
      S is the third digit of MCC
      R is 0xF if MNC has 2 digits
        is the third digit of MNC if it has 3 digits
      T is the second digit of MNC
      U is the first digit of MNC 
      
RETURN VALUE:
   boolean completion status

SIDE EFFECTS:
   None
======================================================================*/
static boolean extractMCCMNCFromPLMN
(
   byte          *arrayOfThreeBytes,
   unsigned int  *MCC,
   unsigned int  *MNC
)
{
    byte  byteOne   = arrayOfThreeBytes[0];
    byte  byteTwo   = arrayOfThreeBytes[1];
    byte  byteThree = arrayOfThreeBytes[2];
    
    /* Validate argument(s).*/
    if( (NULL == MCC) || (NULL == MNC) ){
        OEMGSM1xControl_SVC_MSG_ERROR("NULL pointer is not allowed here.",0,0,0);
        return FALSE;
    }

    *MCC = 
       ( 
         ((byteOne & 0x0F) == 0x0F) ? 0 : ( ( byteOne & 0x0F ) * 100 ) 
       )
     + ( 
         ((byteOne & 0xF0) == 0xF0) ? 0 : ( ( (byteOne & 0xF0) >> 4 ) * 10  ) 
       )
     + ( 
         ((byteTwo & 0x0F) == 0x0F) ? 0 : ( byteTwo & 0x0F )        
       );

    /* If MNC has two digits, this would be 0xF0*/
    if( 0xF0 == (byteTwo & 0xF0) ){

       *MNC = ( 
                 ((byteThree & 0x0F) == 0x0F) ? 0 : ( (byteThree & 0x0F) * 10 )
              )
              + 
              ( 
                 ((byteThree & 0xF0) == 0xF0) ? 0 : ( (byteThree & 0xF0) >> 4 )
              );
    }
    else{  /* MNC has 3 digits.*/
       *MNC = (       
                 ((byteTwo   & 0xF0) == 0xF0) ? 0 : ( (byteTwo & 0xF0) >> 4   ) 
              )
              + 
              ( 
                 ((byteThree & 0x0F) == 0x0F) ? 0 : ( (byteThree & 0x0F) * 100 )
              )
              + 
              ( 
                 ((byteThree & 0xF0) == 0xF0) ? 0 : (( (byteThree & 0xF0) >> 4 ) * 10)
              );
    }
    
    return TRUE;
}


/*======================================================================
FUNCTION:   packMCCMNCIntoPLMN

DESCRIPTION:
   Does the opposite to extractMCCMNCFromPLMN.
   See that function for the algorithm.
      
RETURN VALUE:
   boolean completion status

SIDE EFFECTS:
   None
======================================================================*/
static boolean packMCCMNCIntoPLMN
(
   unsigned int   MCC,
   unsigned int   MNC,
   byte           arrayOfThreeBytes[]
)
{
   unsigned int tempMCC;
   unsigned int tempMNC;
    
   byte MCC1, MCC2, MCC3;
   byte MNC1, MNC2, MNC3;
   
   /* Check data.*/
   if( (MCC > 999) || (MNC > 999) ){
       return FALSE;
   }

   /* Compute decimal digits.*/
   tempMCC = MCC;

   MCC1 = tempMCC / 100;
   tempMCC -= MCC1 * 100;
   
   MCC2 = MCC / 10;
   tempMCC -= MCC2 * 10;
      
   MCC3 = tempMCC;


   tempMNC = MNC;
   
   MNC1 = tempMNC / 100;
   tempMNC -= MNC1 * 100;
   
   if(0 == MNC1){
       MNC1 = 0xF;
   }
   
   MNC2 = tempMNC / 10;
   tempMNC -= MNC2 * 10;

   MNC3 = tempMNC;

   /* Pack the digits into array.*/
   arrayOfThreeBytes[0] = (MCC2 << 4) + MCC1;
   arrayOfThreeBytes[1] = (MNC1 << 4) + MCC3;
   arrayOfThreeBytes[2] = (MNC3 << 4) + MNC2;

   return TRUE;
}


/*=============================================================================
FUNCTION:    OEMGSM1xControl_SVC_GetGSM1xSIDNIDPairs              EXTERNAL FUNCTION

DESCRIPTION:
    
        
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetGSM1xSIDNIDPairs
(
   uint16                                 HomeSIDNIDMaxCnt, 
   AEEGSM1xControl_SIDNIDPairType        *HomeSIDNIDPairs,
   uint16                                *ActualHomeSIDNIDCnt,
   uint16                                 LockedSIDNIDMaxCnt, 
   AEEGSM1xControl_SIDNIDPairType        *LockedSIDNIDPairs,
   uint16                                *ActualLockedSIDNIDCnt
)
{
    uint16                         pairIndex;
    AEEGSM1xControl_NAMType        GSM1xNAM;
    AEEGSM1xSIDNIDParams           SIDNIDparams;

    AEEGSM1xControl_SIDNIDPairType sidnidPair;

    static uint16                  homeSID[NV_MAX_HOME_SID_NID];
    static uint16                  homeNID[NV_MAX_HOME_SID_NID];
    static uint16                  lockedSID[NV_MAX_SID_NID_LOCK];
    static uint16                  lockedNID[NV_MAX_SID_NID_LOCK];

    /*-------------------------------- Get GSM1x NAM.-----------------*/
    if( ! OEMGSM1xProv_ReturnNAMUsedByProvisioningMode( OEMGSM1XPROV_GSM1X, &GSM1xNAM ) )
    {
       return AEEGSM1XCONTROL_STATUS_GSM1X_NOT_SUPPORTED;
    }

    MEMSET( (byte*)homeSID,   0, sizeof(homeSID)   );
    MEMSET( (byte*)homeNID,   0, sizeof(homeNID)   );
    MEMSET( (byte*)lockedSID, 0, sizeof(lockedSID) );
    MEMSET( (byte*)lockedNID, 0, sizeof(lockedNID) );

    SIDNIDparams.nam           = GSM1xNAM;
    SIDNIDparams.homePairCnt   = HomeSIDNIDMaxCnt;
    SIDNIDparams.pHomeSID      = homeSID;
    SIDNIDparams.pHomeNID      = homeNID;
    SIDNIDparams.lockedPairCnt = LockedSIDNIDMaxCnt;
    SIDNIDparams.pLockedSID    = lockedSID;
    SIDNIDparams.pLockedNID    = lockedNID;
    
    if( 0 != OEM_GetConfig(CFGI_GSM1X_SID_NID_PARAMS, (void*) &SIDNIDparams, sizeof(SIDNIDparams)) )
    {
       return AEEGSM1XCONTROL_STATUS_FAILURE_NV_READ;
    }

    COPY_TO_USER((byte*)ActualHomeSIDNIDCnt, (byte*)&SIDNIDparams.homePairCnt, sizeof(*ActualHomeSIDNIDCnt));

    COPY_TO_USER((byte*)ActualLockedSIDNIDCnt, (byte*)&SIDNIDparams.lockedPairCnt, sizeof(*ActualLockedSIDNIDCnt));

    for( pairIndex = 0; pairIndex < SIDNIDparams.homePairCnt; pairIndex++ )
    {             
       sidnidPair.sid = SIDNIDparams.pHomeSID[pairIndex];
       sidnidPair.nid = SIDNIDparams.pHomeNID[pairIndex];
       COPY_TO_USER((byte*)&HomeSIDNIDPairs[pairIndex], (byte*)&sidnidPair, sizeof(sidnidPair));
    }

    for( pairIndex = 0; pairIndex < SIDNIDparams.lockedPairCnt; pairIndex++ )
    {
       sidnidPair.sid = SIDNIDparams.pLockedSID[pairIndex];
       sidnidPair.nid = SIDNIDparams.pLockedNID[pairIndex];
       COPY_TO_USER((byte*)&LockedSIDNIDPairs[pairIndex], (byte*)&sidnidPair, sizeof(sidnidPair));
    }

    return AEEGSM1XCONTROL_STATUS_SUCCESS;
}


/*=============================================================================
FUNCTION:    OEMGSM1xControl_SVC_SetGSM1xSIDNIDPairs              EXTERNAL FUNCTION

DESCRIPTION:
    Set home and locked SID,NID pairs in NV according to the specified
    values.
        
RETURN VALUE:
    AEEGSM1xControl_statusType

SIDE EFFECTS:
    None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SVC_SetGSM1xSIDNIDPairs
(
   uint16                                 HomeSIDNIDCnt, 
   AEEGSM1xControl_SIDNIDPairType        *HomeSIDNIDPairs,
   uint16                                 LockedSIDNIDCnt, 
   AEEGSM1xControl_SIDNIDPairType        *LockedSIDNIDPairs
)
{
    uint16                         pairIndex;
    AEEGSM1xControl_NAMType        GSM1xNAM;
    AEEGSM1xSIDNIDParams           SIDNIDparams;
    static uint16                  homeSID[NV_MAX_HOME_SID_NID];
    static uint16                  homeNID[NV_MAX_HOME_SID_NID];
    static uint16                  lockedSID[NV_MAX_SID_NID_LOCK];
    static uint16                  lockedNID[NV_MAX_SID_NID_LOCK];

    static AEEGSM1xControl_SIDNIDPairType sidnidPair;
    
    /*-------------------------------- Get GSM1x NAM.-----------------*/
    if( ! OEMGSM1xProv_ReturnNAMUsedByProvisioningMode( OEMGSM1XPROV_GSM1X, &GSM1xNAM ) )
    {
       return AEEGSM1XCONTROL_STATUS_GSM1X_NOT_SUPPORTED;
    }

    SIDNIDparams.nam           = GSM1xNAM;
    SIDNIDparams.homePairCnt   = HomeSIDNIDCnt;
    SIDNIDparams.pHomeSID      = homeSID;
    SIDNIDparams.pHomeNID      = homeNID;
    SIDNIDparams.lockedPairCnt = LockedSIDNIDCnt;
    SIDNIDparams.pLockedSID    = lockedSID;
    SIDNIDparams.pLockedNID    = lockedNID;

    MEMSET( (byte*)homeSID,   0, sizeof(homeSID));
    MEMSET( (byte*)homeNID,   0, sizeof(homeNID));
    MEMSET( (byte*)lockedSID, 0, sizeof(lockedSID));
    MEMSET( (byte*)lockedNID, 0, sizeof(lockedNID));
        
    for( pairIndex = 0; pairIndex < HomeSIDNIDCnt; pairIndex++ )
    {
       COPY_FROM_USER((byte*)&sidnidPair, (byte*)&HomeSIDNIDPairs[pairIndex], sizeof(sidnidPair));
       homeSID[pairIndex] = sidnidPair.sid;
       homeNID[pairIndex] = sidnidPair.nid;
    }

    for( pairIndex = 0; pairIndex < LockedSIDNIDCnt; pairIndex++ )
    {
       COPY_FROM_USER((byte*)&sidnidPair, (byte*)&LockedSIDNIDPairs[pairIndex], sizeof(sidnidPair));
       lockedSID[pairIndex] = sidnidPair.sid;
       lockedNID[pairIndex] = sidnidPair.nid; 
    }

    if( 0 != OEM_SetConfig(CFGI_GSM1X_SID_NID_PARAMS, (void*) &SIDNIDparams, sizeof(SIDNIDparams)) )
    {
       return AEEGSM1XCONTROL_STATUS_FAILURE_NV_READ;
    }

    return  AEEGSM1XCONTROL_STATUS_SUCCESS;
}


/*=============================================================================
FUNCTION:    readConfigDataFromFile                           INTERNAL FUNCTION

DESCRIPTION:
    Read the specified data from the specified EFS file.
        
RETURN VALUE:
    boolean success status

SIDE EFFECTS:
    None
=============================================================================*/
static boolean readConfigDataFromFile
(
    char    *FileName,
    uint16   MaxBufLen,
    byte    *BufPtr,
    uint16  *ActualLen
)
{ 
   void        *FileHandle;
   int          bytesRead;
   int          status;
   boolean      returnValue;

   FileHandle  = NULL;
   returnValue = FALSE;
   
   OEMGSM1xControl_SVC_MSG_ERROR("Reading from file",0,0,0);

   do
   {
      if((NULL == BufPtr) || (NULL == ActualLen) || (NULL == FileName)){
          OEMGSM1xControl_SVC_MSG_ERROR("One of parameters is NULL",0,0,0);
          break;
      }
     
      status = OEMFS_Open(FileName, _OFM_READ, &FileHandle);

      if(SUCCESS != status){
          OEMGSM1xControl_SVC_MSG_ERROR("OEMFS_Open failed!",0,0,0);
          break;
      }
  
      bytesRead = OEMFS_Read( FileHandle, BufPtr, MaxBufLen, &status);
   
      if( bytesRead <= 0 ){
          OEMGSM1xControl_SVC_MSG_ERROR("Read %d bytes",bytesRead,0,0);
          break;
      }

      *ActualLen = bytesRead;

      returnValue = TRUE;

   } while(FALSE);

   /* Return values are not useful here, so we don't check them.*/
   if (NULL != FileHandle)
   {
       (void) OEMFS_Close ( FileHandle );
   }
   
   return returnValue;
}


/*=============================================================================
FUNCTION:    writeConfigDataToFile                            INTERNAL FUNCTION

DESCRIPTION:
    Write the specified data into the specified EFS file.
        
RETURN VALUE:
    boolean success status

SIDE EFFECTS:
    None
=============================================================================*/
static boolean writeConfigDataToFile
(
    char    *FileName,
    byte    *BufPtr,
    uint16   BufLen
)
{ 
   void        *FileHandle;
   int          bytesWritten;
   int          status;
   boolean      returnValue;

   FileHandle     = NULL;
   returnValue = FALSE;
   
   OEMGSM1xControl_SVC_MSG_ERROR("Writing to file",0,0,0);

   do
   {
       if((NULL == BufPtr) || (NULL == FileName)){
           OEMGSM1xControl_SVC_MSG_ERROR("One of parameters is NULL",0,0,0);
           break;
       }

       status = OEMFS_Open(FileName, _OFM_READWRITE, &FileHandle);

       if(SUCCESS != status){
           OEMGSM1xControl_SVC_MSG_ERROR("File does not exist - create",0,0,0);

            status = OEMFS_Open(FileName, _OFM_CREATE, &FileHandle);

           if(SUCCESS != status){
               OEMGSM1xControl_SVC_MSG_ERROR("Could not create the file",0,0,0);
               break;
           }
       }
  
       bytesWritten = OEMFS_Write(FileHandle, BufPtr, BufLen, &status);
   
       if( bytesWritten <= 0 ){
           OEMGSM1xControl_SVC_MSG_ERROR("Written %d bytes",bytesWritten,0,0);
           break;
       }

       returnValue = TRUE;

   } while(FALSE);

   /* Return values are not useful here, so we don't check them.*/
   if(NULL != FileHandle){
       (void) OEMFS_Close ( FileHandle );
   }
   
   return returnValue;
}

/*=============================================================================
FUNCTION:    removeConfigDataFile                            INTERNAL FUNCTION

DESCRIPTION:
    Remove the specified EFS file.
    If the file does not exist return success. 
        
RETURN VALUE:
    boolean success status

SIDE EFFECTS:
    None
=============================================================================*/
static boolean removeConfigDataFile
(
    char    *FileName
)
{ 
   int          status;

   OEMGSM1xControl_SVC_MSG_ERROR("Removing file",0,0,0);

   if(NULL == FileName){
       OEMGSM1xControl_SVC_MSG_ERROR("NULL file name for erase",0,0,0);
       return FALSE;
   }

   status = OEMFS_Remove(FileName);
   if (EFILENOEXISTS == status){
       OEMGSM1xControl_SVC_MSG_ERROR("File %s does not exist- considered as removed",FileName,0,0);
       return TRUE;
   }
   if (SUCCESS != status){
       OEMGSM1xControl_SVC_MSG_ERROR("Failed to remove %s",FileName,0,0);
       return FALSE;
   }
   OEMGSM1xControl_SVC_MSG_ERROR("%s was removed",FileName,0,0);

   return TRUE;
}

/*=============================================================================
FUNCTION: OEMGSM1xControl_SVC_ActivateNonGSM1xMode 

DESCRIPTION:
   Activate the specified mode (cannot be GSM1x). 

RETURN VALUE:
   ActivationAPI_ReturnedStatusType - competion status

SIDE EFFECTS:
   None
=============================================================================*/
AEEGSM1xControl_statusType  OEMGSM1xControl_SVC_ActivateNonGSM1xMode
(
    AEEGSM1xControl_modeBitMaskType mode
)
{

    AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask;

    AEEGSM1xControl_statusType             status;

    OEMGSM1xProv_RTREConfigType            newConfig;

    
    switch( mode ) {
    
        case AEEGSM1XCONTROL_GSM1X_PROV_MASK:
           {
               return AEEGSM1XCONTROL_STATUS_CANNOT_SET_DESIRED_MODE;
               break;
            }
        
        case AEEGSM1XCONTROL_1X_RUIM_PROV_MASK:
           {
               /* Check for the presence of the Card and the presence of CDMA on it.*/
               status = OEMGSM1xControl_SVC_GetDFPresence
                        (                         
                           &DFPresenceMask
                        );

               if(AEEGSM1XCONTROL_STATUS_NO_CARD == status){
                   return  AEEGSM1XCONTROL_STATUS_NO_CARD;
               }
               else if(AEEGSM1XCONTROL_STATUS_SUCCESS != status){
                   return AEEGSM1XCONTROL_STATUS_CANNOT_SET_DESIRED_MODE;
               }
               
               if( 0 == (AEEGSM1XCONTROL_CDMA_DF_PRESENT & DFPresenceMask) ){
                   return AEEGSM1XCONTROL_STATUS_NO_INFO_ON_CARD;
               }
               
               /* Fell through - intended.*/
           }

        case AEEGSM1XCONTROL_1X_NV_PROV_MASK:
           {
               newConfig = (mode == AEEGSM1XCONTROL_1X_NV_PROV_MASK) 
                           ? 
                           NV_RTRE_CONFIG_NV_ONLY : NV_RTRE_CONFIG_RUIM_ONLY;
               
               
               if(! OEMGSM1xProv_SetRTREConfig( newConfig )){
                   return AEEGSM1XCONTROL_STATUS_CANNOT_SET_DESIRED_MODE;
               }

               if( 
                    AEEGSM1XCONTROL_STATUS_SUCCESS != EnableNewProvisioningMode
                                                      (                                                      
                                                        mode
                                                      )
                 ){
                   return AEEGSM1XCONTROL_STATUS_CANNOT_SET_DESIRED_MODE;
               }
    
               OEMGSM1xControl_SVC_MSG_ERROR("Entered 1xMode(%d)",(int)mode,0,0);
               
               break;
           }

       case AEEGSM1XCONTROL_EMERGENCY_PROV_MASK:
           {
               
               OEMGSM1xProv_ActivateEmergencyCallOnlyState();
                                                            
               break;
           }

       default:
           {
               return AEEGSM1XCONTROL_STATUS_INVALID_PARAMETER;
               break;
           }
    } /* switch */
    
    return AEEGSM1XCONTROL_STATUS_SUCCESS;
} 


/*=============================================================================
FUNCTION: NoAction 

DESCRIPTION:
   This state is needed to distinguish between a transition that 
   does not have to change current state with uninitialized/unsupported
   state transition.     

RETURN VALUE:
   boolean

SIDE EFFECTS:
   None
=============================================================================*/
static AEEGSM1xControl_modeBitMaskType    NoAction
(
    AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask,
    OEMGSM1xProv_RTREConfigType            RTREconfig,
    boolean                                IsSameCard,
    AEEGSM1xControl_modeBitMaskType        supportedModeMask
)
{

   AEEGSM1xControl_modeBitMaskType  currentState;

   switch(RTREconfig){
   
   case NV_RTRE_CONFIG_RUIM_ONLY:
      {
          currentState =  AEEGSM1XCONTROL_GSM1X_PROV_MASK;
          break;
      }
   
   case NV_RTRE_CONFIG_NV_ONLY:
      {
          currentState =  AEEGSM1XCONTROL_1X_NV_PROV_MASK;
          break;
      }
          
   case NV_RTRE_CONFIG_RUIM_OR_DROP_BACK:
      {
          if( 0 != (AEEGSM1XCONTROL_CDMA_DF_PRESENT & DFPresenceMask) ){
             currentState = AEEGSM1XCONTROL_GSM1X_PROV_MASK;
          }
          else{
             currentState = AEEGSM1XCONTROL_1X_NV_PROV_MASK;
          }
          break;
      }

   case NV_RTRE_CONFIG_SIM_ACCESS:
      {
         currentState =  AEEGSM1XCONTROL_GSM1X_PROV_MASK;
         break;
      }
   default:
      {
         /*Should never get here.*/
         currentState = AEEGSM1XCONTROL_EMERGENCY_PROV_MASK;
      }
   } /* switch */
      
   return  currentState;
}


/*=============================================================================
FUNCTION: GoTo1xNV 

DESCRIPTION:
   Attempt to set 1xNV is the phone SW supports it, otherwise go to emergency.     

RETURN VALUE:
   new provisioning mode

SIDE EFFECTS:
   None
=============================================================================*/
static AEEGSM1xControl_modeBitMaskType    GoTo1xNV
(
    AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask,
    OEMGSM1xProv_RTREConfigType            RTREconfig,
    boolean                                IsSameCard,
    AEEGSM1xControl_modeBitMaskType        supportedModeMask
)
{
    AEEGSM1xControl_statusType status;

    if( 0 == ( AEEGSM1XCONTROL_1X_NV_PROV_MASK & supportedModeMask )){
        (void) OEMGSM1xControl_SVC_ActivateNonGSM1xMode( AEEGSM1XCONTROL_EMERGENCY_PROV_MASK );
        return  AEEGSM1XCONTROL_EMERGENCY_PROV_MASK;
    }

    status = OEMGSM1xControl_SVC_ActivateNonGSM1xMode( AEEGSM1XCONTROL_1X_NV_PROV_MASK );
    
    if(AEEGSM1XCONTROL_STATUS_SUCCESS == status){
       return AEEGSM1XCONTROL_1X_NV_PROV_MASK;
    }

    (void) OEMGSM1xControl_SVC_ActivateNonGSM1xMode( AEEGSM1XCONTROL_EMERGENCY_PROV_MASK );
    return  AEEGSM1XCONTROL_EMERGENCY_PROV_MASK;
}


/*=============================================================================
FUNCTION: GoToGSM1x 

DESCRIPTION:
   Attempt to set GSM1x is the phone SW supports it, otherwise go to emergency.     

RETURN VALUE:
   new provisioning mode


SIDE EFFECTS:
   None
=============================================================================*/
static AEEGSM1xControl_modeBitMaskType    GoToGSM1x
(
    AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask,
    OEMGSM1xProv_RTREConfigType            RTREconfig,
    boolean                                IsSameCard,
    AEEGSM1xControl_modeBitMaskType        supportedModeMask
)
{
    OEMGSM1xControl_SVC_MSG_ERROR("GoToGSM1x executed",0,0,0);


    if( 0 == ( AEEGSM1XCONTROL_GSM1X_PROV_MASK & supportedModeMask )){
        (void) OEMGSM1xControl_SVC_ActivateNonGSM1xMode( AEEGSM1XCONTROL_EMERGENCY_PROV_MASK );
        return  AEEGSM1XCONTROL_EMERGENCY_PROV_MASK;
    }


    /* Wake up the Activation App via notification, return 0 since we don't
       know which mode will be set.*/
    OEMGSM1xControl_GiveControlToActivationApp( AEEGSM1XCONTROL_ACTION_PROVISION_GSM1X );
    return 0;
}


/*=============================================================================
FUNCTION: GoTo1xRUIMOr1xNV 

DESCRIPTION:
   Attempt to set 1xRUIM is available, otherwise attempt 1xNV, otherwise
   go to emergency mode.      

RETURN VALUE:
   new provisioning mode


SIDE EFFECTS:
   None
=============================================================================*/
static AEEGSM1xControl_modeBitMaskType    GoTo1xRUIMOr1xNV
(
    AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask,
    OEMGSM1xProv_RTREConfigType            RTREconfig,
    boolean                                IsSameCard,
    AEEGSM1xControl_modeBitMaskType        supportedModeMask
)
{
    AEEGSM1xControl_statusType status;

    if( 0 != ( AEEGSM1XCONTROL_1X_RUIM_PROV_MASK & supportedModeMask )){
        
        status = OEMGSM1xControl_SVC_ActivateNonGSM1xMode( AEEGSM1XCONTROL_1X_RUIM_PROV_MASK );
        
        if(AEEGSM1XCONTROL_STATUS_SUCCESS == status){
           return  AEEGSM1XCONTROL_1X_RUIM_PROV_MASK;
        }
    }

    if( 0 != ( AEEGSM1XCONTROL_1X_NV_PROV_MASK & supportedModeMask )){
        
        status =  OEMGSM1xControl_SVC_ActivateNonGSM1xMode( AEEGSM1XCONTROL_1X_NV_PROV_MASK );
        
        if(AEEGSM1XCONTROL_STATUS_SUCCESS == status){
           return  AEEGSM1XCONTROL_1X_NV_PROV_MASK;
        }
    }
 
    (void) OEMGSM1xControl_SVC_ActivateNonGSM1xMode( AEEGSM1XCONTROL_EMERGENCY_PROV_MASK );
    return  AEEGSM1XCONTROL_EMERGENCY_PROV_MASK;
}


/*=============================================================================
FUNCTION: BringApp 

DESCRIPTION:
   Bring activation app - no decision can be made.      

RETURN VALUE:
   all the alternative modes that are available

SIDE EFFECTS:
   None
=============================================================================*/
static AEEGSM1xControl_modeBitMaskType    BringApp
(    
    AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask,
    OEMGSM1xProv_RTREConfigType            RTREconfig,
    boolean                                IsSameCard,
    AEEGSM1xControl_modeBitMaskType        supportedModeMask
)
{
    OEMGSM1xControl_GiveControlToActivationApp( AEEGSM1XCONTROL_ACTION_BRING_UP_GUI );
    return 0;
}


/*=============================================================================
FUNCTION: InitStateMachine 

DESCRIPTION:
   Initialize internal state machine.    

RETURN VALUE:
   void

SIDE EFFECTS:
   None
=============================================================================*/
static void    InitStateMachine
(
    void
)
{
    MEMSET( StateTransitionTable, 0, sizeof(StateTransitionTable) );

    StateTransitionTable[ CARD_STATE_NO_CARD ][ RTRE_STATE_INVALID          ] = GoTo1xNV ;
    StateTransitionTable[ CARD_STATE_NO_CARD ][ RTRE_STATE_RUIM_ONLY        ] = GoTo1xNV ;
    StateTransitionTable[ CARD_STATE_NO_CARD ][ RTRE_STATE_NV_ONLY          ] = NoAction ;
    StateTransitionTable[ CARD_STATE_NO_CARD ][ RTRE_STATE_RUIM_DROPBACK_NV ] = NoAction ;
    StateTransitionTable[ CARD_STATE_NO_CARD ][ RTRE_STATE_SIM_ACCESS       ] = GoTo1xNV ;

    StateTransitionTable[ CARD_STATE_SAME_SIM ][ RTRE_STATE_INVALID          ] = GoToGSM1x ;
    StateTransitionTable[ CARD_STATE_SAME_SIM ][ RTRE_STATE_RUIM_ONLY        ] = GoToGSM1x ;
    StateTransitionTable[ CARD_STATE_SAME_SIM ][ RTRE_STATE_NV_ONLY          ] = NoAction ;
    StateTransitionTable[ CARD_STATE_SAME_SIM ][ RTRE_STATE_RUIM_DROPBACK_NV ] = NoAction ;
    StateTransitionTable[ CARD_STATE_SAME_SIM ][ RTRE_STATE_SIM_ACCESS       ] = GoToGSM1x ;

    StateTransitionTable[ CARD_STATE_NEW_SIM ][ RTRE_STATE_INVALID          ] = GoToGSM1x ;
    StateTransitionTable[ CARD_STATE_NEW_SIM ][ RTRE_STATE_RUIM_ONLY        ] = GoToGSM1x ;
    StateTransitionTable[ CARD_STATE_NEW_SIM ][ RTRE_STATE_NV_ONLY          ] = GoToGSM1x ;
    StateTransitionTable[ CARD_STATE_NEW_SIM ][ RTRE_STATE_RUIM_DROPBACK_NV ] = GoToGSM1x ;
    StateTransitionTable[ CARD_STATE_NEW_SIM ][ RTRE_STATE_SIM_ACCESS       ] = GoToGSM1x ;

    StateTransitionTable[ CARD_STATE_SAME_RUIM_W_GSM ][ RTRE_STATE_INVALID          ] = GoToGSM1x ;
    StateTransitionTable[ CARD_STATE_SAME_RUIM_W_GSM ][ RTRE_STATE_RUIM_ONLY        ] = NoAction ;
    StateTransitionTable[ CARD_STATE_SAME_RUIM_W_GSM ][ RTRE_STATE_NV_ONLY          ] = NoAction ;
    StateTransitionTable[ CARD_STATE_SAME_RUIM_W_GSM ][ RTRE_STATE_RUIM_DROPBACK_NV ] = NoAction ;
    StateTransitionTable[ CARD_STATE_SAME_RUIM_W_GSM ][ RTRE_STATE_SIM_ACCESS       ] = GoToGSM1x ;

    StateTransitionTable[ CARD_STATE_NEW_RUIM_W_GSM ][ RTRE_STATE_INVALID          ] = GoTo1xRUIMOr1xNV ;
    StateTransitionTable[ CARD_STATE_NEW_RUIM_W_GSM ][ RTRE_STATE_RUIM_ONLY        ] = NoAction ;
    StateTransitionTable[ CARD_STATE_NEW_RUIM_W_GSM ][ RTRE_STATE_NV_ONLY          ] = BringApp ;
    StateTransitionTable[ CARD_STATE_NEW_RUIM_W_GSM ][ RTRE_STATE_RUIM_DROPBACK_NV ] = NoAction ;
    StateTransitionTable[ CARD_STATE_NEW_RUIM_W_GSM ][ RTRE_STATE_SIM_ACCESS       ] = GoTo1xRUIMOr1xNV ;

    StateTransitionTable[ CARD_STATE_SAME_RUIM_NO_GSM ][ RTRE_STATE_INVALID          ] = GoTo1xRUIMOr1xNV ;
    StateTransitionTable[ CARD_STATE_SAME_RUIM_NO_GSM ][ RTRE_STATE_RUIM_ONLY        ] = NoAction ;
    StateTransitionTable[ CARD_STATE_SAME_RUIM_NO_GSM ][ RTRE_STATE_NV_ONLY          ] = NoAction ;
    StateTransitionTable[ CARD_STATE_SAME_RUIM_NO_GSM ][ RTRE_STATE_RUIM_DROPBACK_NV ] = NoAction ;
    StateTransitionTable[ CARD_STATE_SAME_RUIM_NO_GSM ][ RTRE_STATE_SIM_ACCESS       ] = GoTo1xRUIMOr1xNV ;

    StateTransitionTable[ CARD_STATE_NEW_RUIM_NO_GSM ][ RTRE_STATE_INVALID          ] = GoTo1xRUIMOr1xNV ;
    StateTransitionTable[ CARD_STATE_NEW_RUIM_NO_GSM ][ RTRE_STATE_RUIM_ONLY        ] = NoAction ;
    StateTransitionTable[ CARD_STATE_NEW_RUIM_NO_GSM ][ RTRE_STATE_NV_ONLY          ] = GoTo1xRUIMOr1xNV ;
    StateTransitionTable[ CARD_STATE_NEW_RUIM_NO_GSM ][ RTRE_STATE_RUIM_DROPBACK_NV ] = NoAction ;
    StateTransitionTable[ CARD_STATE_NEW_RUIM_NO_GSM ][ RTRE_STATE_SIM_ACCESS       ] = GoTo1xRUIMOr1xNV ;
}


/*=============================================================================
FUNCTION: OEMGSM1xControl_SVC_ProcessPowerUp 

DESCRIPTION:
   Attempt to do automatic mode selection on power up.       
   
RETURN VALUE:
   N/A

SIDE EFFECTS:
   None
=============================================================================*/
void OEMGSM1xControl_SVC_ProcessPowerUp
(
    void
)
{

#define   ICC_ID_LEN      (14)
#define   ICC_FILE_NAME   "fs:/iccid.bin"

    static boolean                         FirstTime = TRUE;

   
    AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask;
    int                                    cardState;

    boolean                                GSM_or_DCS_DF_Present;
    boolean                                CDMA_DF_Present;

    OEMGSM1xProv_RTREConfigType            RTREconfig;
    int                                    RTREstate;


    boolean                                IsSameCard;
    boolean                                Got_ICCID_FromCard;
    AEEGSM1xControl_modeBitMaskType        supportedModeMask;

    ActivationAPI_StateTransition          stateTransitionFuncPtr;

    AEEGSM1xControl_modeBitMaskType        newMode;

    boolean                                selectedSomeMode;

    AEEGSM1xControl_statusType             status;

    byte                                   currentICCId[ICC_ID_LEN];
    uint16                                 currentICCIdLen;

    byte                                   storedICCId[ICC_ID_LEN];
    uint16                                 storedICCIdLen;
    
    Got_ICCID_FromCard  = FALSE;
    IsSameCard          = TRUE;
    IsCalledFromPowerUp = TRUE;

    if (! gbInitialized){
      return;
    }

    if( FirstTime ){
        FirstTime = FALSE;
    }
    else{
        return;
    }


    OEMGSM1xControl_SVC_MSG_ERROR("AEEGSM1xControl_DoAutoModeSelect",0,0,0);

    status =  OEMGSM1xControl_SVC_GetSupportedProvisioningModes
              (                  
                 &supportedModeMask
              );

    if(AEEGSM1XCONTROL_STATUS_SUCCESS != status){
       /* No modes supported.*/
       supportedModeMask = 0;
    }


    OEMGSM1xControl_SVC_MSG_ERROR("Supported modes 0x%x",supportedModeMask,0,0);

    if(! OEMGSM1xProv_GetRTREConfig( &RTREconfig )){
        RTREconfig = 55; /* arbitrary invalid value.*/
    }
    

    /* For other modes, we need to check if there is an identity card
       and which directories are on it.*/
    status = OEMGSM1xControl_SVC_GetDFPresence
             (
                &DFPresenceMask
             );


    if(AEEGSM1XCONTROL_STATUS_SUCCESS != status){
       DFPresenceMask = 0x0; /* No card or nothing useful on the card.*/
    }

    OEMGSM1xControl_SVC_MSG_ERROR("DF presence mask 0x%x",DFPresenceMask,0,0);


    CDMA_DF_Present = (0 != (DFPresenceMask & AEEGSM1XCONTROL_CDMA_DF_PRESENT));
    
    GSM_or_DCS_DF_Present = 
          (
            (0 != (DFPresenceMask & AEEGSM1XCONTROL_GSM_DF_PRESENT))
            ||
            (0 != (DFPresenceMask & AEEGSM1XCONTROL_DCS1800_DF_PRESENT))
          );

#if !defined(FEATURE_UIM_RUN_TIME_ENABLE)
    {
        byte dirFlag = 0;
        
        dirFlag |= (0 != (DFPresenceMask & AEEGSM1XCONTROL_GSM_DF_PRESENT)) 
                       ? NVRUIM_GSM_DF_PRESENT : 0;
        dirFlag |= (0 != (DFPresenceMask & AEEGSM1XCONTROL_DCS1800_DF_PRESENT)) 
                       ? NVRUIM_DCS1800_DF_PRESENT : 0;
        dirFlag |= (0 != (DFPresenceMask & AEEGSM1XCONTROL_CDMA_DF_PRESENT)) 
                       ? NVRUIM_CDMA_DF_PRESENT : 0;

        nvruim_set_uim_dir_present(dirFlag);

        /* Let the software use ESN from NV if in GSM1x mode
           (SIM card.) */
        if(
              (0 != dirFlag)
              &&
              (0 == (DFPresenceMask & AEEGSM1XCONTROL_CDMA_DF_PRESENT))
          ){
            OEMGSM1xProv_SetESNUsage(OEMGSM1XPROV_GET_ESN_FROM_NV);
        }
    }
#endif  /* FEATURE_UIM_RUN_TIME_ENABLE */

    /* Get current ICC Id from smart card.*/
    MEMSET( currentICCId, 0, sizeof(currentICCId) );
    MEMSET( storedICCId,  0, sizeof(storedICCId)  );
        
    status =  OEMGSM1xControl_SVC_GetUIMUniqueId
              ( 
                 sizeof(currentICCId), 
                 currentICCId, 
                &currentICCIdLen
              );

    if(AEEGSM1XCONTROL_STATUS_SUCCESS != status){
       /* Since we are having trouble reading ICC, we assume this is 
          not the same card as before.*/
       IsSameCard = FALSE;
    }
    else
    {
       Got_ICCID_FromCard = TRUE;
    }

    OEMGSM1xControl_SVC_MSG_ERROR("1.ICCId, %d",IsSameCard,0,0);

    /* Get stored ICC Id from EFS file.*/
    if( IsSameCard ){
                
        if(! readConfigDataFromFile(ICC_FILE_NAME, sizeof(storedICCId), storedICCId, &storedICCIdLen))
        {
           IsSameCard = FALSE;
        }
    }

    OEMGSM1xControl_SVC_MSG_ERROR("2.ICCId, %d",IsSameCard,0,0);

    /* Compare the two ICC Ids.*/
    if( IsSameCard ){
        if( currentICCIdLen != storedICCIdLen ){
           IsSameCard = FALSE;
           OEMGSM1xControl_SVC_MSG_ERROR("3.ICCId diff length %d",IsSameCard,0,0);
        }
        else if(0 != MEMCMP( currentICCId, storedICCId, storedICCIdLen ) ){
           IsSameCard = FALSE;
           OEMGSM1xControl_SVC_MSG_ERROR("4.ICCId same len, diff contents %d",IsSameCard,0,0);
        }
    }
    /*Remove the previous file. This remove operation takes care for ICCIDs with different length (no need for truncating)
    and for cases where the ICCID couldn't be read from the card.*/
    removeConfigDataFile(ICC_FILE_NAME);

    if (Got_ICCID_FromCard)
    {
    /* Store the current ICC Id for next time (best effort.*/
    writeConfigDataToFile( ICC_FILE_NAME, currentICCId, currentICCIdLen);
    OEMGSM1xControl_SVC_MSG_ERROR("5.ICCId stored",0,0,0);
    }
    
    /* Initialize Card State Index to the State Transition Table.*/
    if( (! CDMA_DF_Present) && (! GSM_or_DCS_DF_Present) ){
        cardState = CARD_STATE_NO_CARD;
    }
    else if(! CDMA_DF_Present && GSM_or_DCS_DF_Present ){
        cardState = (IsSameCard) ? CARD_STATE_SAME_SIM : CARD_STATE_NEW_SIM;
    }
    else if( CDMA_DF_Present && GSM_or_DCS_DF_Present ){
        cardState = (IsSameCard) ? CARD_STATE_SAME_RUIM_W_GSM : CARD_STATE_NEW_RUIM_W_GSM;
    }
    else {/* if( CDMA_DF_Present && (! GSM_or_DCS_DF_Present) ){  */
        cardState = (IsSameCard) ? CARD_STATE_SAME_RUIM_NO_GSM : CARD_STATE_NEW_RUIM_NO_GSM;
    }
    

    OEMGSM1xControl_SVC_MSG_ERROR("Card State is %d",  (int)cardState,0,0);
    
    /* Initialize RTRE State Index to the State Transition Table.*/

    switch( RTREconfig ){
       case NV_RTRE_CONFIG_RUIM_ONLY:
          {
             RTREstate = RTRE_STATE_RUIM_ONLY;
             break;
          }
       case NV_RTRE_CONFIG_NV_ONLY:
          {
             RTREstate = RTRE_STATE_NV_ONLY;
             break;
          }
       case NV_RTRE_CONFIG_RUIM_OR_DROP_BACK:
          {
             RTREstate = RTRE_STATE_RUIM_DROPBACK_NV;
             break;
          }
       case NV_RTRE_CONFIG_SIM_ACCESS:
          {
             RTREstate =  RTRE_STATE_SIM_ACCESS;
             break;
          } 
       default:
          {
             RTREstate = RTRE_STATE_INVALID;
             break;
          }
    } /* switch */

    OEMGSM1xControl_SVC_MSG_ERROR("RTRE State is %d",  (int)RTREstate,0,0);              
    
    /* Initialize state machine array.*/
    InitStateMachine();

    OEMGSM1xControl_SVC_MSG_ERROR("State machine initialized",0,0,0);
    
    /* State machine to figure out what do we do now.*/
    stateTransitionFuncPtr = StateTransitionTable[ cardState ][ RTREstate];

    if(NULL != stateTransitionFuncPtr){

        OEMGSM1xControl_SVC_MSG_ERROR("Exist state trans",0,0,0);

        newMode = (*stateTransitionFuncPtr)(DFPresenceMask, RTREconfig, IsSameCard, supportedModeMask);
    
        switch(newMode){
        
           case AEEGSM1XCONTROL_1X_NV_PROV_MASK:
           case AEEGSM1XCONTROL_1X_RUIM_PROV_MASK:
           case AEEGSM1XCONTROL_EMERGENCY_PROV_MASK:
              {
                 selectedSomeMode =  AEEGSM1XCONTROL_STATUS_SUCCESS;
                 break;
              }
           case 0:  /* No mode known yet - activation app will be lauched.*/
           case AEEGSM1XCONTROL_GSM1X_PROV_MASK:
           default:
              {
                 selectedSomeMode =  AEEGSM1XCONTROL_STATUS_CANNOT_SET_DESIRED_MODE;
                 break;
              }
        } /* switch */    
    }
    else
    {
        OEMGSM1xControl_SVC_MSG_ERROR("State transition not defined",0,0,0);
        /* State transition is not defined.  Proceed to ONLINE.*/
        selectedSomeMode =  AEEGSM1XCONTROL_STATUS_SUCCESS;
    }

    return;
}


/*=============================================================================
FUNCTION: OEMGSM1xControl_SVC_GetAvailableModes                 EXTERNAL FUNCTION

DESCRIPTION:
   Get the available options based on SW capabilities and presence/type
   of user identity card.     

RETURN VALUE:
   ActivationAPI_ReturnedStatusType - competion status

SIDE EFFECTS:
   None
=============================================================================*/
AEEGSM1xControl_statusType  OEMGSM1xControl_SVC_GetAvailableModes
(
    AEEGSM1xControl_modeBitMaskType *modeMask
)
{
    AEEGSM1xControl_modeBitMaskType        tempModeMask;
    AEEGSM1xControl_statusType             status;
    AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask;
    AEEGSM1xControl_modeBitMaskType        modeMaskResult; 

    modeMaskResult = 0;

    status =  OEMGSM1xControl_SVC_GetSupportedProvisioningModes
              ( 
                &tempModeMask
              );

    if (AEEGSM1XCONTROL_STATUS_SUCCESS != status)
    {
        /* Nothing is available.*/
        OEMGSM1xControl_SVC_MSG_ERROR("No modes available",0,0,0);

        COPY_TO_USER(modeMask, &modeMaskResult, sizeof(*modeMask));
        return AEEGSM1XCONTROL_STATUS_SUCCESS;
    }

    /* If 1xNV is available, set that.*/
    if( 0 != (AEEGSM1XCONTROL_1X_NV_PROV_MASK & tempModeMask) )
    {
       modeMaskResult |= AEEGSM1XCONTROL_1X_NV_PROV_MASK;
    }

    /* For other modes, we need to check if there is an identity card
       and which directories are on it.*/
    status = OEMGSM1xControl_SVC_GetDFPresence
             (
                &DFPresenceMask
             );

    if(AEEGSM1XCONTROL_STATUS_SUCCESS == status)
    {   
        if( 
             (0 != (AEEGSM1XCONTROL_GSM1X_PROV_MASK & tempModeMask))
             &&
             ( 
                (0 != (AEEGSM1XCONTROL_GSM_DF_PRESENT & DFPresenceMask))
                ||
                (0 != (AEEGSM1XCONTROL_DCS1800_DF_PRESENT & DFPresenceMask))
             )
          )
        {
              modeMaskResult |= AEEGSM1XCONTROL_GSM1X_PROV_MASK;
        }

        if( 
             (0 != (AEEGSM1XCONTROL_1X_RUIM_PROV_MASK & tempModeMask))
             &&
             (0 != (AEEGSM1XCONTROL_CDMA_DF_PRESENT & DFPresenceMask))
          )
        {
              modeMaskResult |= AEEGSM1XCONTROL_1X_RUIM_PROV_MASK;
        }
    }

    OEMGSM1xControl_SVC_MSG_ERROR("GetAvailModes=0x%x %d", (int)modeMaskResult, modeMaskResult, 0);

    COPY_TO_USER(modeMask, &modeMaskResult, sizeof(*modeMask));
    
    return AEEGSM1XCONTROL_STATUS_SUCCESS;
}


/*=============================================================================
FUNCTION: OEMGSM1xControl_SVC_GetCurrentMode                      EXTERNAL FUNCTION

DESCRIPTION:
   Return the current provisioning mode..     

RETURN VALUE:
   ActivationAPI_ReturnedStatusType - competion status

SIDE EFFECTS:
   None
=============================================================================*/
AEEGSM1xControl_statusType OEMGSM1xControl_SVC_GetCurrentMode
(
   AEEGSM1xControl_modeBitMaskType       *modeMask
)
{
   OEMGSM1xProv_RTREConfigType            config;
   AEEGSM1xControl_modeBitMaskType        modeMaskResult;

   if(! OEMGSM1xProv_GetRTREConfig( &config ))
   {
      return AEEGSM1XCONTROL_STATUS_INVALID_MODE;
   }

   modeMaskResult = 0x0;

   switch( config ){

      case NV_RTRE_CONFIG_SIM_ACCESS:
         {
             modeMaskResult = AEEGSM1XCONTROL_GSM1X_PROV_MASK;
             break;
         }
      case NV_RTRE_CONFIG_NV_ONLY:
         {
             modeMaskResult = AEEGSM1XCONTROL_1X_NV_PROV_MASK;
             break;
         }
      case  NV_RTRE_CONFIG_RUIM_ONLY:
         {
             modeMaskResult = AEEGSM1XCONTROL_1X_RUIM_PROV_MASK;
             break;
         }
      case NV_RTRE_CONFIG_RUIM_OR_DROP_BACK:
      default:
         {
             return AEEGSM1XCONTROL_STATUS_INVALID_MODE;
         }
   } /* switch */

   COPY_TO_USER(modeMask, &modeMaskResult, sizeof(*modeMask));

   return AEEGSM1XCONTROL_STATUS_SUCCESS;
}


#endif /*FEATURE_GSM1x*/

