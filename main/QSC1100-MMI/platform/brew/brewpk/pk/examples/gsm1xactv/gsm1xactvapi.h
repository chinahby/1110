#ifndef GSM1XACTVAPI_H
#define GSM1XACTVAPI_H 1

/*======================================================================
FILE: gsm1xactvapi.h

DESCRIPTION:

     Defines interfaces between UI portion of the GSM1x Activation App
     and the rest (core) of the App.
     
     Call gsm1xactvapi_ActivateGSM1xMode to activate GSM1x Mode
     (provision GSM1x identity from a SIM(R-UIM) card, generate
     an appropriate PRL and re-do the system determination.)
  
     More interfaces are TBD to support various models of PRL
     generation and UI behavior settings. 
     
              (c) COPYRIGHT 2002,2003 QUALCOMM Incorporated.
                         All Rights Reserved.
                      QUALCOMM Proprietary/GTDR
======================================================================*/

/*----------------------------------------------------------------------
 Include files 
----------------------------------------------------------------------*/
#include "AEEGSM1xControl.h"
#include "gsm1xactvprl.h"


/*----------------------------------------------------------------------
 Global Constant Declarations
----------------------------------------------------------------------*/
#define    GSM1XACTVAPI_SUCCESS                        (0x0000)
#define    GSM1XACTVAPI_NO_CARD                        (0x0001)
#define    GSM1XACTVAPI_NO_INFO_ON_CARD                (0x0002)
#define    GSM1XACTVAPI_MODE_NOT_SUPPORTED             (0x0003)
#define    GSM1XACTVAPI_CANNOT_DO_AUTO_MODE_SELECTION  (0x0004)
#define    GSM1XACTVAPI_INVALID_MODE                   (0x0005)
#define    GSM1XACTVAPI_CANNOT_SET_DESIRED_MODE        (0x0006)
#define    GSM1XACTVAPI_NO_MODE_AVAILABLE              (0x0007)

typedef uint32   gsm1xactvapi_ReturnedStatusType;

#define    GSM1XACTVAPI_PRL_GEN_FROM_PLMN              (0x0001)
#define    GSM1XACTVAPI_PRL_GEN_FROM_SYSTEM_FILE       (0x0002)
#define    GSM1XACTVAPI_PRL_GEN_FROM_USER_FILE         (0x0004)

typedef uint32   gsm1xactvapi_PRLGenModeType;

/*----------------------------------------------------------------------
 Global Data Declarations
----------------------------------------------------------------------*/
extern     IGSM1xControl   *AEEGSM1xControl_InstancePtr;

/*----------------------------------------------------------------------
 Function Prototypes
----------------------------------------------------------------------*/


gsm1xactvapi_ReturnedStatusType  gsm1xactvapi_ActivateGSM1xMode
(
    IGSM1xControl                   *instancePtr,
    gsm1xactvprl_LargeDataBuffers  *largeDataBuf
);

#endif /* GSM1XACTVAPI_H */
