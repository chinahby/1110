/*==============================================================================

  High Speed USB Vendor Specific command handler
  
  GENERAL DESCRIPTION
  This module defines functions that handle Vendor Specific commands received 
  from the Host.
      
  EXTERNALIZED FUNCTIONS
  hsu_vendor_cmd_in_cb

  INITALIZATION AND SEQUENCING REQUIREMENTS
  None

  Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_vendor_specific.c#4 $
  $DateTime: 2008/07/17 07:27:36 $ $Author: omanos $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  05/26/08  tl      Initial version
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_common.h"
#include "hsu_vendor_specific.h"
#include "jerrno.h"
#include "jtypes.h"

#ifdef FEATURE_HS_USB_TLP
#include "hsu_al_ecm.h"
#endif /*FEATURE_HS_USB_TLP*/
#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND
#include "hsu_al_ser_base_i.h"
#endif /*FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND*/
/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/
#ifdef FEATURE_HS_USB_TLP
#define HS_USB_TLP_VER 0x1 /* Current version of the TLP protocol*/
#endif /*FEATURE_HS_USB_TLP*/

#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND
#define HS_USB_BYTE_PADDING_PROT_VER 0x1
#endif /*FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND*/
/*==============================================================================
  Typedefs
==============================================================================*/
/*==============================================================================
  Variables
==============================================================================*/
#if defined(FEATURE_HS_USB_TLP) && !defined(FEATURE_HS_USB_BASIC)
static const uint8 hs_usb_tlp_ver = HS_USB_TLP_VER;
#endif /*FEATURE_HS_USB_TLP && !FEATURE_HS_USB_BASIC*/

/* This array should hold unique and positive values! 0 is illegal.*/
static juint8_t hsu_vendor_specific_str_idx_arr[HSU_STR_IDX_LAST] = 
{
  HSU_STR_IDX_NOT_SET,  /* HSU_STR_IDX_PRODUCT */
  HSU_STR_IDX_NOT_SET,  /* HSU_STR_IDX_MANUFACTURER */
  HSU_STR_IDX_NOT_SET,  /* HSU_STR_IDX_SERIAL_NUMBER (not set)*/
  HSU_STR_IDX_NOT_SET,  /* HSU_STR_IDX_CONFIGURATION (not set) */
};

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/
#ifndef FEATURE_HS_USB_BASIC
#ifdef FEATURE_HS_USB_TLP
/*==============================================================================

FUNCTION    hsu_vendor_specific_handle_enable_dl_tlp  

DESCRIPTION   
This function is called whenever we receive HSU_VENDOR_SPECIFIC_ENABLE_DL_TLP 
request.

DEPENDENCIES  
None.

RETURN VALUE  
0 for success, or an error code.

SIDE EFFECTS  
Enables DL TLP.

==============================================================================*/
static jresult_t hsu_vendor_specific_handle_enable_dl_tlp
(
  juint16_t version,
  juint8_t *resp_buf, 
  juint16_t resp_buf_size, 
  juint16_t *act_resp_buf_size
)
{
  HSU_MSG_HIGH("*_handle_enable_dl_tlp: enter",0,0,0);

  if (resp_buf == NULL )
  {
    HSU_MSG_ERROR("*_handle_enable_dl_tlp: Invalid input! (resp_buf = NULL)",0,0,0);
    return JEINVAL;
  }

  /* Check if version that the host uses, is greater or equal to the one 
  used here.*/
  if (version < hs_usb_tlp_ver)
  {
    HSU_MSG_ERROR("*_handle_enable_dl_tlp: TLP version mismatch: %d < %d", 
      version, hs_usb_tlp_ver,0);
    return JEINVAL;
  }

  /* Setup reply */
  if (resp_buf_size <  sizeof(hs_usb_tlp_ver))
  {
    HSU_MSG_ERROR("*_handle_enable_dl_tlp: response buffer is too small"
      "(resp_buf_size = %d)",resp_buf_size,0,0);
    return JEINVAL;
  }
  memcpy(resp_buf,&hs_usb_tlp_ver, sizeof(hs_usb_tlp_ver));
  *act_resp_buf_size =  sizeof(hs_usb_tlp_ver);

  hsu_al_ecm_enable_dl_tlp();

  HSU_MSG_HIGH("*_handle_enable_dl_tlp: exit",0,0,0);
  return HSU_JSUCCESS;
}

/*==============================================================================

FUNCTION    hsu_vendor_specific_handle_enable_ul_tlp  

DESCRIPTION   
This function is called when ever we receive HSU_VENDOR_SPECIFIC_ENABLE_UL_TLP 
request.

DEPENDENCIES  
None.

RETURN VALUE  
0 for success, or an error code.

SIDE EFFECTS  
Enables UL TLP.

==============================================================================*/
static jresult_t hsu_vendor_specific_handle_enable_ul_tlp
(
  juint16_t version,
  juint8_t *resp_buf, 
  juint16_t resp_buf_size, 
  juint16_t *act_resp_buf_size
)
{
  HSU_MSG_HIGH("*_handle_enable_ul_tlp: enter",0,0,0);

  if (resp_buf == NULL )
  {
    HSU_MSG_ERROR("*_handle_enable_ul_tlp: Invalid input! (resp_buf = NULL)",0,0,0);
    return JEINVAL;
  }

  /* Check if wValue that the host uses, is greater or equal to the one 
  used here.*/
  if (version < hs_usb_tlp_ver)
  {
    HSU_MSG_ERROR("*_handle_enable_ul_tlp: TLP version mismatch: %d < %d", 
      version, hs_usb_tlp_ver,0);
    return JEINVAL;
  }

  /* Setup reply */
  if (resp_buf_size <  sizeof(hs_usb_tlp_ver))
  {
    HSU_MSG_ERROR("*_handle_enable_dl_tlp: response buffer is too small",0,0,0);
    return JEINVAL;
  }
  memcpy(resp_buf, &hs_usb_tlp_ver,sizeof(hs_usb_tlp_ver));
  *act_resp_buf_size =  sizeof(hs_usb_tlp_ver);

  hsu_al_ecm_enable_ul_tlp();

  HSU_MSG_HIGH("*_handle_enable_ul_tlp: exit",0,0,0);
  return HSU_JSUCCESS;
}
#endif /*FEATURE_HS_USB_TLP*/

#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND
/*==============================================================================

FUNCTION    hsu_vendor_specific_enable_byte_pad  

DESCRIPTION   
This function is called when ever we receive HSU_VENDOR_SPECIFIC_ENABLE_BYTE_PAD 
request.

DEPENDENCIES  
None.

RETURN VALUE  
0 for success, or an error code

SIDE EFFECTS  
Enables byte pad for all serial AL.

==============================================================================*/
static jresult_t hsu_vendor_specific_enable_byte_pad
(
  juint16_t version,
  juint8_t *resp_buf, 
  juint16_t resp_buf_size, 
  juint16_t *act_resp_buf_size
)
{
  uint8 hs_usb_byte_pad_ver = HS_USB_BYTE_PADDING_PROT_VER;

  HSU_MSG_HIGH("*_enable_byte_pad: enter",0,0,0);

  if (resp_buf == NULL )
  {
    HSU_MSG_ERROR("*_enable_byte_pad: Invalid input! (resp_buf = NULL)",0,0,0);
    return JEINVAL;
  }
    
  /* Check if wValue that the host uses, is greater or equal to the one 
     used here. */
  if (version < hs_usb_byte_pad_ver)
  {
    HSU_MSG_ERROR("*_enable_byte_pad: Version mismatch: %d < %d", 
      version, hs_usb_byte_pad_ver,0);
    return JEINVAL;
  }

  /* Setup reply */
   if (resp_buf_size <  sizeof(hs_usb_byte_pad_ver))
  {
    HSU_MSG_ERROR("*_enable_byte_pad: response buffer is too small "
      "(resp_buf_size=%d)",resp_buf_size,0,0);
    return JEINVAL;
  }
  memcpy(resp_buf,&hs_usb_byte_pad_ver, sizeof(hs_usb_byte_pad_ver));
  *act_resp_buf_size =  sizeof(hs_usb_byte_pad_ver);
  
  hsu_al_ser_enable_byte_pad();

  HSU_MSG_HIGH("*_enable_byte_pad: exit",0,0,0);
  return HSU_JSUCCESS;
}
#endif /* FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND */

/*==============================================================================

FUNCTION  hsu_vendor_cmd_in_cb        

DESCRIPTION
This function is supplied as a callback for a Vendor Specific (IN) command
that is received from the host.
    
DEPENDENCIES
  None.

RETURN VALUE
0 for success, or an error code

SIDE EFFECTS
  None.

==============================================================================*/
jresult_t hsu_vendor_cmd_in_cb(juint8_t bRequest, juint16_t wValue,
    juint16_t wIndex, juint8_t *resp_buf, juint16_t resp_buf_size, 
    juint16_t *act_resp_buf_size)
{
  jresult_t rc = HSU_JSUCCESS;
  HSU_USE_PARAM(wIndex);
  switch (bRequest)
  {
#ifdef FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND
    case HSU_VENDOR_SPECIFIC_ENABLE_BYTE_PAD:
      rc = hsu_vendor_specific_enable_byte_pad(wValue, resp_buf, 
            resp_buf_size, act_resp_buf_size);
      break;
#endif /*FEATURE_HSU_1_2_BYTE_HW_CRC_ERROR_WORKAROUND*/
#ifdef FEATURE_HS_USB_TLP
    case HSU_VENDOR_SPECIFIC_ENABLE_DL_TLP:
      rc = hsu_vendor_specific_handle_enable_dl_tlp(wValue,resp_buf,
            resp_buf_size,act_resp_buf_size);
      break;
    case HSU_VENDOR_SPECIFIC_ENABLE_UL_TLP:
      rc = hsu_vendor_specific_handle_enable_ul_tlp(wValue,resp_buf,
            resp_buf_size,act_resp_buf_size);
      break;
#endif /*FEATURE_HS_USB_TLP*/
    default:
      HSU_MSG_ERROR("hsu_vendor_cmd_in_cb(): Unhandled Vendor command (%d)",bRequest,0,0);
      rc = JENOTSUP;
  }
  return rc;
}
#endif /*FEATURE_HS_USB_BASIC*/
/*==============================================================================

FUNCTION     hsu_vendor_specific_get_str_idx     

DESCRIPTION
This function returns the index that was reserved fo the given string (from 
device/configuration string descriptors)

DEPENDENCIES
None.

RETURN VALUE
Index of the given string or HSU_STR_IDX_NOT_SET

SIDE EFFECTS
  None.

==============================================================================*/
juint8_t hsu_vendor_specific_get_str_idx(hsu_str_idx_desc_type str_desc)
{
  if (str_desc >= HSU_STR_IDX_LAST  )
  {
    HSU_MSG_ERROR("*_get_str_idx(): Invalid str_desc = %d",str_desc,0,0);
    return HSU_STR_IDX_NOT_SET;
  }
  return hsu_vendor_specific_str_idx_arr[str_desc];
}

/*==============================================================================

FUNCTION     hsu_vendor_specific_is_idx_occupied     

DESCRIPTION
This function checks if the given index is allready occupied by some other string

DEPENDENCIES
None.

RETURN VALUE
TRUE if the given index is already occupied

SIDE EFFECTS
  None.

==============================================================================*/
boolean hsu_vendor_specific_is_idx_occupied(juint8_t idx_to_check)
{
  uint8 i;
  for(i = 0; i < HSU_STR_IDX_LAST; i++)
  {
    if (hsu_vendor_specific_str_idx_arr[i] == idx_to_check)
    {
      return TRUE;
    }
  }
  return FALSE;
}

