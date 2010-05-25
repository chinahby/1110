/*======================================================================
FILE: gsm1xactvapi.c

GENERAL DESCRIPTION:
    See gsm1xactvapi.h for a general description.


IMPLEMENTATION-SPECIFIC COMMENTS:
    None.
    
              (c) COPYRIGHT 2002,2003 QUALCOMM Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
======================================================================*/

/*----------------------------------------------------------------------
 Include Files
----------------------------------------------------------------------*/

#include "AEEStdLib.h"                   
                 
#include "gsm1xactvapi.h"
#include "AEEGSM1xControl.h"
#include "gsm1xactvprl.h"



/*----------------------------------------------------------------------
 Defines
----------------------------------------------------------------------*/                                     

/* This one is used for debugging only.*/
#if defined(DEBUG_GSM1XAPI)
#define GSM1XACTVAPI_MSG_ERROR(__msg,__p1,__p2,__p3)  \
           DBGPRINTF((__msg),(__p1),(__p2),(__p3))  
#else
#define GSM1XACTVAPI_MSG_ERROR(__msg,__p1,__p2,__p3) 
#endif /* DEBUG_GSM1XAPI */


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
FUNCTION: gsm1xactvapi_ActivateGSM1xMode 

DESCRIPTION:
   Activate the specified mode.     

RETURN VALUE:
   gsm1xactvapi_ReturnedStatusType - competion status

SIDE EFFECTS:
   None
=============================================================================*/
gsm1xactvapi_ReturnedStatusType  gsm1xactvapi_ActivateGSM1xMode
(
    IGSM1xControl                   *instancePtr,
    gsm1xactvprl_LargeDataBuffers  *largeDataBuf
)
{
    AEEGSM1xControl_DFPresenceBitMaskType  DFPresenceMask;

    AEEGSM1xControl_statusType     status;

    if(NULL == instancePtr){
        return  GSM1XACTVAPI_CANNOT_SET_DESIRED_MODE;
    }

    status = IGSM1xControl_GetDFPresence
             (
                 instancePtr,
                &DFPresenceMask
             );
    
    if(AEEGSM1XCONTROL_STATUS_NO_CARD == status){
        return  GSM1XACTVAPI_NO_CARD;
    }
    else if(AEEGSM1XCONTROL_STATUS_SUCCESS != status){
        return GSM1XACTVAPI_CANNOT_SET_DESIRED_MODE;
    }
               

    if( 
         (0 == (AEEGSM1XCONTROL_GSM_DF_PRESENT & DFPresenceMask))
         &&
         (0 == (AEEGSM1XCONTROL_DCS1800_DF_PRESENT & DFPresenceMask))
      ){
        return GSM1XACTVAPI_NO_INFO_ON_CARD;
    }
               
    if(
         AEEGSM1XCONTROL_STATUS_SUCCESS != IGSM1xControl_ProvisionGSM1xParameters
                                           (
                                              instancePtr
                                           )
      ){
         return GSM1XACTVAPI_CANNOT_SET_DESIRED_MODE;
    }

    /* Do not provision PRL for this release.
    if(! gsm1xactvprl_provisionGSM1xPRLToNV( instancePtr, largeDataBuf )){
        GSM1XACTVAPI_MSG_ERROR("Could not provision GSM1X PRL to NV.",0,0,0);
        return GSM1XACTVAPI_CANNOT_SET_DESIRED_MODE;
    }
    */
        

    if( 
         AEEGSM1XCONTROL_STATUS_SUCCESS != IGSM1xControl_EnableGSM1xMode
                                           (
                                              instancePtr
                                           )
      ){
         return GSM1XACTVAPI_CANNOT_SET_DESIRED_MODE;
    }

    GSM1XACTVAPI_MSG_ERROR("Entered GSM1xMode",0,0,0);

    return GSM1XACTVAPI_SUCCESS;

}