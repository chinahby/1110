#ifndef OEMCMHELPER_H
#define OEMCMHELPER_H
/*=============================================================================

FILE: OEMCMHelper.h

GENERAL DESCRIPTION: Helper functions used only by OEMCM.c.

        Copyright c 2003-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
/*=============================================================================

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Inc/OEMCMHelper.h#3 $

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when         who     what, where, why
----------   ---     ---------------------------------------------------------
11/08/07     alb     Remove CM_API_MBMS ifdefs.
11/02/07     alb     Add MBMS helpers.
08/06/07     alb     Created
=============================================================================*/

#include "prl.h"
#include "AEECM.h"

#ifdef FEATURE_MBMS
#include "cm.h"
#endif

#define ERR_IF_NULL(x) \
if(NULL == x) { \
ERR("NULL PARAM",0,0,0); \
return; }

/*=============================================================================
FUNCTION: OEMCMHelper_CopyToPrlAcqRec

DESCRIPTION:
   Copies the AEECMPrlAcqRecord struct to its prl.h equivalent, 
   prli_acq_rec_type.  

PARAMETERS:
   pDest                [out]: The structure from prl.h to copy to.
   pSrc                 [in]:  The AEECM structure to copy from.

=============================================================================*/
void OEMCMHelper_CopyToPrlAcqRec(prli_acq_rec_type *pDest,
                                 const AEECMPrlAcqRecord *pSrc);

/*=============================================================================
FUNCTION: OEMCMHelper_CopyFromPrlAcqRec

DESCRIPTION:
   Copies to the AEECMPrlAcqRecord struct from its prl.h equivalent, 
   prli_acq_rec_type.  

PARAMETERS:
   pDest                [out]: The AEECM structure to copy to.
   pSrc                 [in]:  The prl.h structure to copy from.

=============================================================================*/
void OEMCMHelper_CopyFromPrlAcqRec(AEECMPrlAcqRecord *pDest,
                                   const prli_acq_rec_type *pSrc);

#ifdef FEATURE_MBMS
/*=============================================================================
FUNCTION: OEMCMHelper_CopyMBMSInfo

DESCRIPTION:
   Copies from cm_mbms_info_s_type to AEECMMBMSEventData.
   This is a file scope only utility function.

DEPENDENCIES:
  cm_mbms_info_s_type in cm.h
=============================================================================*/
void OEMCMHelper_CopyMBMSInfo(AEECMMBMSEventData *pAEECMMBMSInfo,
                              const cm_mbms_info_s_type *pCMMBMSInfo);

/*=============================================================================
FUNCTION: OEMCMHelper_CopyMBMSCmdFromAEECM

DESCRIPTION:
   Copies the data from the AEECMMBMSCmd struct to the CM equivalent struct,
   cm_mbms_generic_cmd_s_type, so that the command data can be passed on to 
   the CM function cm_mbms_cmd_generic.  The AEECMMBMSCmd struct should be
   filled out by the app when calling ICM_SendMBMSCommand.
  
PARAMETERS:
  pMbmsGenericCmd    [out]: Struct that is passed to CM 
  pMbmsCmd           [in]:  Struct that is filled by the APP.

=============================================================================*/
void OEMCMHelper_CopyMBMSCmdFromAEECM(cm_mbms_generic_cmd_s_type *pMbmsGenericCmd,
                                      const AEECMMBMSCmd *pMbmsCmd);
#endif

#endif  //OEMCMHELPER_H
