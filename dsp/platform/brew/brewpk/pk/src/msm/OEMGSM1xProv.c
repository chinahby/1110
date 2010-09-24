/*======================================================================
FILE: OEMGSM1xProv.c

GENERAL DESCRIPTION:
    See OEMGSM1xProv.h for a general description.


IMPLEMENTATION-SPECIFIC COMMENTS:
    The routines in this class are called from a single thread
    (BREW UI), so there is not re-entrancy requirement.  Thus,
    the routines provided in this file should be assumed non-reentrant.
    
    The implementation is based on the following documents:
       1. QUALCOMM "GSM1x MS Provisioning & Activation", 80-31492-1 X1.
       2. TIA/EIA/IS-683-A Over-the-Air Provisioning of Mobile Stations
          in Spread Spectrum Systems.
       3. TIA/EIA/IS 2000.5-A-1 Upper Layer (Layer 3) Signaling Standard
          for cdma2000 Spread Spectrum Systems.   

              (c) COPYRIGHT 2002 QUALCOMM Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
======================================================================*/

/*----------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------*/

#include "msg.h"
#include "nv.h"
#include "nvruimi.h"
#include "task.h"
#include "rex.h"
#include "cm.h"

#include "OEMFeatures.h"

#ifdef FEATURE_GSM1x

#include "OEMGSM1xProv.h"
#include "OEMConfig.h"
#include "OEMGSM1xCardHandler.h"


/*----------------------------------------------------------------------
 Defines
----------------------------------------------------------------------*/                                     

/* Arbitrary value which does not match somebody else's id.*/                                 
#define GSM1X_PROV_CLIENT_ID (99)

/*----------------------------------------------------------------------
 Global Data Declarations
----------------------------------------------------------------------*/
/* None.*/

/*----------------------------------------------------------------------
 Static Data Declarations
----------------------------------------------------------------------*/
/* None.*/

/*----------------------------------------------------------------------
 Function Prototypes
----------------------------------------------------------------------*/
/* None.*/     

/*----------------------------------------------------------------------
 Function Definitions
----------------------------------------------------------------------*/



/*=============================================================================
FUNCTION: OEMGSM1xProv_ReturnNAMUsedByProvisioningMode 

DESCRIPTION:
   Returns which NAM is used for specified mode.     

RETURN VALUE:
   boolean competion status (TRUE=success)

SIDE EFFECTS:
   None
=============================================================================*/
boolean  OEMGSM1xProv_ReturnNAMUsedByProvisioningMode
(
    uint16    mode,    
    byte     *pNAM
)
{
    /* Validate argument(s).*/
    if(NULL == pNAM){
        return FALSE;
    }

    switch( mode ){
    
       case OEMGSM1XPROV_1X_RUIM_ONLY:
           *pNAM = 0;
           break;
    
       case OEMGSM1XPROV_1X_NV_ONLY:
           /* Mode not supported.*/
           return FALSE;
           break;
    
       case OEMGSM1XPROV_1X_RUIM_DROP_BACK_TO_NV:
           /* Mode not supported.*/
           return FALSE;
           break;
    
       case OEMGSM1XPROV_GSM1X:
           *pNAM = 0;
           break;
       
       default:
           /* Shoult not get here.*/
           return FALSE;
    } /* switch */

    return TRUE;
}


/*=============================================================================
FUNCTION: OEMGSM1xProv_IsModeSupported 

DESCRIPTION:
   Returns whether the specified provisioning mode is supported by 
   phone software/hardware.     

RETURN VALUE:
   boolean competion status (TRUE=success)

SIDE EFFECTS:
   None
=============================================================================*/
boolean  OEMGSM1xProv_IsModeSupported
(
    uint16 mode
)
{
    boolean modeIsSupported = FALSE;

    switch( mode ){
    
       case OEMGSM1XPROV_1X_RUIM_ONLY:
           modeIsSupported = TRUE;
           break;
    
       case OEMGSM1XPROV_1X_NV_ONLY:
           modeIsSupported = FALSE;
           break;
    
       case OEMGSM1XPROV_1X_RUIM_DROP_BACK_TO_NV:
           modeIsSupported = FALSE;
           break;
    
       case OEMGSM1XPROV_GSM1X:
           modeIsSupported = TRUE;
           break;
       
       default:
           /* Shoult not get here.*/
           modeIsSupported = FALSE;
    } /* switch */

    return  modeIsSupported;
}

#if defined(FEATURE_UIM_RUN_TIME_ENABLE)

/*=============================================================================
FUNCTION: OEMGSM1xProv_SendRTRECmd 

DESCRIPTION:
   This command updates dynamic state variabke (RTREControl) of RTRE.     

RETURN VALUE:
   boolean competion status (TRUE=success)

SIDE EFFECTS:
   None
=============================================================================*/
void  OEMGSM1xProv_SendRTRECmd
(
    uint16        newRTREConfig
)
{
    nv_item_type   nv_buf;
    nv_cmd_type    nv_cmd_buffer; 

     nv_buf.rtre_config = newRTREConfig;

    nv_cmd_buffer.tcb_ptr = rex_self();          /* notify this task when done */
    nv_cmd_buffer.sigs = OEMGSM1XPROV_UI_SIG_FOR_UIM;
    nv_cmd_buffer.done_q_ptr = NULL;             /* command goes on no queue when done */

    nv_cmd_buffer.item = NV_RTRE_CONFIG_I;       /* item to process */
    nv_cmd_buffer.cmd  = NV_RTRE_OP_CONFIG_F;

    /* Set up NV so that it will read the data into the correct location */
    nv_cmd_buffer.data_ptr = &nv_buf;

    /* Clear the return signal, call NV, and wait for a response */
    (void) rex_clr_sigs( rex_self(), OEMGSM1XPROV_UI_SIG_FOR_UIM );
    nv_cmd( &nv_cmd_buffer );
    rex_wait( OEMGSM1XPROV_UI_SIG_FOR_UIM );
}
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */



/*=============================================================================
FUNCTION: OEMGSM1xProv_SetESNSource 

DESCRIPTION:
   Set the usage for ESN.  Usage of ESN is not controlled by RTRE,
   so we might have to update this every time we update RTRE.     
   Implementation is OEM-specific.

RETURN VALUE:
   boolean competion status (TRUE=success)

SIDE EFFECTS:
   None
=============================================================================*/
void     OEMGSM1xProv_SetESNUsage
(
    uint16       ESNUsage
)
{
    switch( ESNUsage ){
    
       case  OEMGSM1XPROV_GET_ESN_FROM_RUIM:
          {
              nvruim_set_esn_usage( NV_RUIM_USE_RUIM_ID );
              break;
          }

       case  OEMGSM1XPROV_GET_ESN_FROM_NV:
          {
              nvruim_set_esn_usage( NV_RUIM_USE_ESN );
              break;
          }

       case  OEMGSM1XPROV_USE_ZERO_ESN:
          {
              nvruim_set_esn_usage( NV_RUIM_ZERO_ESN );
              break;
          }
    
       default:
          {
              /* Should never get here.  Do nothing if we ever do.*/
              break;
          }
    } /* switch */
}



/*=============================================================================
FUNCTION: OEMGSM1xProv_ActivateEmergencyCallOnlyState 

DESCRIPTION:
   Activate the mode when user can only make emergency (E911)
   calls.  Implementation is OEM specific.

RETURN VALUE:
   boolean competion status (TRUE=success)

SIDE EFFECTS:
   None
=============================================================================*/
void     OEMGSM1xProv_ActivateEmergencyCallOnlyState
(
    void
)
{
    /* TBD */
}



/*=============================================================================
FUNCTION:    OEMGSM1xProv_GetRTREConfig                    EXTERNAL FUNCTION

DESCRIPTION:
    Get RTRE configuration from NV.
    
RETURN VALUE:
    boolean completion status

SIDE EFFECTS:
    None
=============================================================================*/
boolean OEMGSM1xProv_GetRTREConfig
( 
    OEMGSM1xProv_RTREConfigType  *pConfig
)
{
#if defined(FEATURE_UIM_RUN_TIME_ENABLE)
   AEEGSM1xRTREConfig  RTREConfig; 
    
   if( 0 != OEM_GetConfig(CFGI_GSM1X_RTRE_CONFIG, (void*) &RTREConfig, sizeof(RTREConfig)) ){
      return FALSE;
   }

   *pConfig = (OEMGSM1xProv_RTREConfigType) RTREConfig;

#else
   {
       /* Is non-RTRE target, the configuration cannot change
          until a phone is powered down and the UIM card is replaced
          with another type.
       */
       static boolean                         IsFirstTime = TRUE;
       CardHandlerDirPresenceBitMaskType      cardHandlerBitMask;
       
       if( IsFirstTime ){
           if(UIM_PASS == OEMGSM1xCardHandler_GetDirPresence( &cardHandlerBitMask )){

               if( 0 != (OEMGSM1xCardHandler_CDMA_DF_PRESENT & cardHandlerBitMask) ){
                  *pConfig = NV_RTRE_CONFIG_RUIM_ONLY;
               }
               else if( 0 != (OEMGSM1xCardHandler_GSM_DF_PRESENT & cardHandlerBitMask) ){
                  *pConfig = NV_RTRE_CONFIG_SIM_ACCESS;
               }
               IsFirstTime = TRUE;
           }
       }
   }
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */

   return  TRUE;
}



/*=============================================================================
FUNCTION:    OEMGSM1xProv_SetRTREConfig                    EXTERNAL FUNCTION

DESCRIPTION:
    Set new RTRE configuration and update RTRE control variable.
        
RETURN VALUE:
    boolean completion status

SIDE EFFECTS:
    None
=============================================================================*/
boolean OEMGSM1xProv_SetRTREConfig
( 
    OEMGSM1xProv_RTREConfigType    newConfig
)
{

#if defined(FEATURE_UIM_RUN_TIME_ENABLE)
        
    AEEGSM1xRTREConfig  RTREConfig = (AEEGSM1xRTREConfig) newConfig; 
    
    if( 0 != OEM_SetConfig(CFGI_GSM1X_RTRE_CONFIG, (void*) &RTREConfig, sizeof(RTREConfig)) ){
       return FALSE;
    }

    /* Update dynamic state control variable of RTRE.*/
    OEMGSM1xProv_SendRTRECmd( newConfig );
    
    /* Set ESN usage since it might not be contolled by RTRE.*/
    switch( newConfig ){
    case NV_RTRE_CONFIG_RUIM_ONLY:
    case NV_RTRE_CONFIG_RUIM_OR_DROP_BACK:
        {
           OEMGSM1xProv_SetESNUsage( OEMGSM1XPROV_GET_ESN_FROM_RUIM );
           break;
        }
    case NV_RTRE_CONFIG_NV_ONLY:
    case NV_RTRE_CONFIG_SIM_ACCESS:
        {
           OEMGSM1xProv_SetESNUsage( OEMGSM1XPROV_GET_ESN_FROM_NV );
           break;
        }
    default:
        {
           /* Should not here.  Ignore if we do.*/
           break;
        }
    } /* switch */
#else
    /* If RTRE is not supported, this configuration cannot
       be changed*/
#endif /* FEATURE_UIM_RUN_TIME_ENABLE */
       
    return  TRUE;
}

#endif // FEATURE_GSM1x
