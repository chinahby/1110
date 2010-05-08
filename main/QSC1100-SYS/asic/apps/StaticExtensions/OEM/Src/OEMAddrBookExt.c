/*=============================================================================

FILE: OEMAddrBookExt.c

SERVICES: OEMAddrBookExt implementation

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2006 QUALCOMM Incorporated.
                    All Rights Reserved.

                    QUALCOMM Proprietary
=============================================================================*/


/*=============================================================================

  $Header: //depot/asic/msmshared/apps/StaticExtensions/oem/src/OEMAddrBookExt.c#2 $
$DateTime: 2006/10/25 15:10:34 $
  $Author: t_sueker $
  $Change: 394224 $

=============================================================================*/



/*===========================================================================

                    INCLUDE FILES FOR MODULE

===========================================================================*/
#include "OEMFeatures.h"

#include "AEEAddrBookExt.h"
#include "AddrBookExt.bid"

#include "AEEAddrBook.h"

/*lint -save -library */
#include "uiupbm.h"
/*lint -restore*/

/*===========================================================================

                    DEFINITIONS AND CONSTANTS

===========================================================================*/

// Useful macro for Lint
#ifndef PARAM_NOT_REF
#define PARAM_NOT_REF(param)         /*lint -esym(715,param) */
#endif


/*===========================================================================

                    TYPE DECLARATIONS

===========================================================================*/

struct OEMAddrBookExtClass {
   AEEVTBL(IAddrBookExt) *pvt;
};


/*===========================================================================

                    STATIC DATA

===========================================================================*/

// The one and only IAddrBookExt implementation
static struct OEMAddrBookExtClass sOEMAddrBookExt;

// Pointer to hold the parameter return value for OEMAddrBookExt_GetRecInfoEx.
static AEEAddrCatRecInfo *spCatRecInfo = NULL;

/*===========================================================================

                    FUNCTION IMPLEMENTATIONS

===========================================================================*/


/*=============================================================================
FUNCTION: OEMAddrBookExt_AddRef

DESCRIPTION:  Implements IBASE_AddRef()

PARAMETERS:
   *po  [in]:  IAddrBookExt interface

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 OEMAddrBookExt_AddRef(IAddrBookExt *po)
{
   PARAM_NOT_REF(po)

   return 1;
}

/*=============================================================================
FUNCTION: OEMAddrBookExt_Release

DESCRIPTION:  Implements IBASE_Release() 

PARAMETERS:
   *p  [in]:  IAddrBookExt interface

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static uint32 OEMAddrBookExt_Release(IAddrBookExt *po)
{
   PARAM_NOT_REF(po)

   return 0;
}

/*=============================================================================
FUNCTION: OEMAddrBookExt_QueryInterface

DESCRIPTION:  Implements IQI_QueryInterface()

PARAMETERS:
   *p    [in]:  IAddrBookExt interface
    id   [in]:
  **ppo [out]:

RETURN VALUE:
   int:  BREW result code

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMAddrBookExt_QueryInterface(IAddrBookExt *po, AEECLSID id, void **ppNew)
{
   PARAM_NOT_REF(po)
   PARAM_NOT_REF(id)
   PARAM_NOT_REF(ppNew)

   return ECLASSNOTSUPPORT;
}

/*========================================================================
Function: cat_to_phonebook()

Description:
   Helper function that translates from AEEAddrCat to pbm_device_type (phonebook)

Prototype:
   pbm_device_type cat_to_phonebook(AEEAddrCat wCat);

Parameters:
   wCat: [in]: Specifies the address category.

Return Value:
   The phonebook that corresponds to the category
========================================================================*/
static pbm_device_type cat_to_phonebook(AEEAddrCat wCat)
{
  switch (wCat)
  {
  case AEE_ADDR_CAT_NONE:
  case AEE_ADDR_CAT_PERSONAL:
  case AEE_ADDR_CAT_BUSINESS:
    return PBM_EFS;

#ifdef FEATURE_UIM
  case AEE_ADDR_CAT_SIM_ADN:
    return PBM_ADN;
    
  case AEE_ADDR_CAT_SIM_ADN2:
    return PBM_ADN2;
#endif /* FEATURE_UIM */

  case AEE_ADDR_CAT_SIM_FDN:
    return PBM_FDN;

  case AEE_ADDR_CAT_SIM_FDN2:
    return PBM_FDN2;

  case AEE_ADDR_CAT_SIM_SDN:
    return PBM_SDN;

  case AEE_ADDR_CAT_SIM_SDN2:
    return PBM_SDN2;

  case AEE_ADDR_CAT_SIM_MSISDN:
    return PBM_MSISDN;

  case AEE_ADDR_CAT_SIM_MSISDN2:
    return PBM_MSISDN2;

  case AEE_ADDR_CAT_SPEEDDIAL:  // Speed dial list
    return PBM_SDL;

  default:
    return PBM_EFS;  // Error here instead????
  }
}

/*========================================================================

Function: store_pbm_file_info_cb()

Description:
   This function stores the number of records used, number of records free 
   for a category, the text length, and the number length as a helper to
   OEMAddrBookExt_GetRecInfo.

Prototype:
  static void store_pbm_file_info_cb( pbm_return_type pbm_ret,
                                      pbm_device_type pbm_id, 
                                      pbm_extended_fileinfo_s_type *pInfo);

Parameters:
   pbm_ret:           [in]: return value from pbm_file_info
   pbm_id:            [in]: phonebook used for usage information
   pInfo:             [in]: structure containing results

Return Value:
  None

Comments:
   None

Side Effects:
   Stores the information in the static global return value to the caller
   of OEMAddrBookExt_GetRecInfo.

See Also:

   typedef struct {
      int records_used;         number of records used
      int number_of_records;    total number of records
      int text_len;             length of text fields
      int number_len;           length of number fields, not including + or NULL
   } pbm_extended_fileinfo_s_type;
========================================================================*/
static void store_pbm_file_info_cb( pbm_return_type pbm_ret,
                                    pbm_device_type pbm_id, 
                                    pbm_extended_fileinfo_s_type *pInfo )
{

   PARAM_NOT_REF(pbm_id)

   if ((PBM_SUCCESS != pbm_ret) || (NULL == spCatRecInfo) ) {
      return;
   }

   spCatRecInfo->nRecordsInUse = pInfo->records_used;         
   spCatRecInfo->nRecordsFree =  pInfo->number_of_records - pInfo->records_used;
   spCatRecInfo->nTextLength =   pInfo->text_len;   /* length of text fields */
   spCatRecInfo->nNumberLength = pInfo->number_len; /* length of number fields, not including + or NULL */
}

/*========================================================================
Function: pbm_error_to_aee()

Description:
   Helper function that translates from PBM errors to AEE errors.

Prototype:
   static int pbm_error_to_aee(pbm_return_type pbmRet);

Parameters:
   pbmRet: [in]: Specifies the PBM error

Return Value:
   The AEE error that corresponds to PBM error specified
========================================================================*/
static int pbm_error_to_aee(pbm_return_type pbmRet)
{
  int ret = EFAILED;

  switch(pbmRet) {
    case PBM_SUCCESS:
    case PBM_EOF:
    case PBM_ERROR_NOT_FOUND:
      ret = AEE_SUCCESS;
      break;

    case PBM_ERROR_NO_SIM:
    case PBM_ERROR_NOT_AVAILABLE:
      ret = ENOSUCH;
      break;

    case PBM_ERROR_NOT_READY:
    case PBM_ERROR_SYNC:
      ret = EITEMBUSY;
      break;

    case PBM_ERROR_RESTRICTED:
      ret = EBADSTATE;
      break;

    case PBM_ERROR_SIM_PIN_REQ:
    case PBM_ERROR_SIM_PUC_REQ:
    case PBM_ERROR_PIN2_REQ:
    case PBM_ERROR_PUC2_REQ:
      ret = ENEEDMORE;
      break;

    case PBM_ERROR_MEM_FULL:
      ret = ENOMEMORY;
      break;

    case PBM_ERROR_INDEX_INVALID:
    case PBM_ERROR_TEXT_TOO_LONG:
    case PBM_ERROR_NUMBER_TOO_LONG:
      ret = EBADPARM;
      break;
      
    default:
      break;
  }

  return ret;
}

/*=============================================================================
FUNCTION: OEMAddrBookExt_GetRecInfo

DESCRIPTION:  Retrieve extended record information pertaining to the category

PARAMETERS:
   po             [in]: Pointer to the IAddrBookExt object
   wCategory      [in]: Address book category
   pCatRecInfo   [out]: Record information for the specified category

RETURN VALUE:
   int: BREW error code

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
static int OEMAddrBookExt_GetRecInfo( IAddrBookExt *po,
                                      AEEAddrCat wCategory, 
                                      AEEAddrCatRecInfo *pCatRecInfo)
{
   PARAM_NOT_REF(po)

   pbm_device_type pb_id;
   pbm_return_type pbm_ret;
   
   if (!pCatRecInfo) {
      return EBADPARM;
   }

   pb_id = cat_to_phonebook(wCategory);

   // Store return value pointer for callback function to fill in.
   spCatRecInfo = pCatRecInfo;

   pbm_ret = pbm_extended_file_info(pb_id, store_pbm_file_info_cb);

   // Clear out return value pointer.
   spCatRecInfo = NULL;

   if (PBM_SUCCESS != pbm_ret) {
      return pbm_error_to_aee(pbm_ret);
   }

   return AEE_SUCCESS;
}

/*=============================================================================
FUNCTION: OEMAddrBookExt_New

DESCRIPTION:  Constructs a new IAddrBookExt interface

PARAMETERS:
   *pIShell  [in]:
    cls      [in]:
  **ppif    [out]:

RETURN VALUE:
   int:

COMMENTS:

SIDE EFFECTS:

SEE ALSO:

=============================================================================*/
int OEMAddrBookExt_New(IShell *pIShell, AEECLSID cls, void **ppif)
{
   /* The function supported by this object. */
   static const AEEVTBL(IAddrBookExt) sOEMAddrBookExtFuncs = {
      OEMAddrBookExt_AddRef,
      OEMAddrBookExt_Release,
      OEMAddrBookExt_QueryInterface,
      OEMAddrBookExt_GetRecInfo
   };

   // Check parameters.
   if ((NULL == pIShell) || (NULL == ppif)) {
      return EBADPARM;
   }

   if (cls != AEECLSID_ADDRBOOKEXT) {
      return EUNSUPPORTED;
   }

   sOEMAddrBookExt.pvt = (AEEVTBL(IAddrBookExt) *) &sOEMAddrBookExtFuncs;

   *ppif = &sOEMAddrBookExt;

   return AEE_SUCCESS;
}
