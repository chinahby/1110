#ifndef _HSU_VENDOR_SPECIFIC_H_
#define _HSU_VENDOR_SPECIFIC_H_

/*==============================================================================

  High Speed USB Vendor Specific command handler
        H E A D E R   F I L E
  
  GENERAL DESCRIPTION
  This module defines function that handle Vendor Specific commands received 
  from the Host.

  EXTERNALIZED FUNCTIONS
  hsu_vendor_cmd_in_cb

  INITALIZATION AND SEQUENCING REQUIREMENTS
  None.
    
  Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_vendor_specific.h#3 $
  $DateTime: 2008/07/07 05:45:50 $ $Author: smalichi $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  05/27/08  tl      Initial version
    
==============================================================================*/
#include "jtypes.h"


/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
enum hsu_vendor_requests_t
{
  /* Enable DL Net Pkt Bundling */
  HSU_VENDOR_SPECIFIC_ENABLE_DL_TLP = 0x02,
  /* Enable UL Net Pkt Bundling */
  HSU_VENDOR_SPECIFIC_ENABLE_UL_TLP = 0x03,
  /* Enable byte pad (workaround for 1_2 byte error)*/
  HSU_VENDOR_SPECIFIC_ENABLE_BYTE_PAD = 0x81
};

/* This enum describes the strings that are sent in the device/configuration
   descriptor. This defenitions are used as indexes of the 
   hsu_vendor_specific_str_idx_arr (defined in hsu_vendor_specific.c). The 
   hsu_vendor_specific_str_idx_arr  is used for determine in what index the
   specific string (e.q HSU_STR_IDX_PRODUCT -> product name) will be saved.
   This index will be sent to the host at enumeration 
   (e.q if hsu_vendor_specific_str_idx_arr[HSU_STR_IDX_PRODUCT] = 1 then
   iProduct = 0x01)
 */
typedef enum 
{ 
  HSU_STR_IDX_PRODUCT = 0,  /* Product name */
  HSU_STR_IDX_MANUFACTURER, /* Product Manufacturer */
  HSU_STR_IDX_SERIAL_NUMBER,/* Serial Number */
  HSU_STR_IDX_CONFIGURATION,/* Product Configuration */
  HSU_STR_IDX_LAST,          /* Size of the enum */
  HSU_STR_IDX_NOT_SET = 0xFF
}hsu_str_idx_desc_type;
/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

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
    juint16_t *act_resp_buf_size);

/*==============================================================================

FUNCTION     hsu_vendor_specific_is_idx_occupied     

DESCRIPTION
This function checks if the given index is allready occupied by some other string
See documentation of hsu_str_idx_desc_type for more details

DEPENDENCIES
None.

RETURN VALUE
TRUE if the given index is already occupied

SIDE EFFECTS
  None.

==============================================================================*/
boolean hsu_vendor_specific_is_idx_occupied(juint8_t idx_to_check);

/*==============================================================================

FUNCTION     hsu_vendor_specific_get_str_idx     

DESCRIPTION
This function returns the index that was reserved fo the given string (from 
device/configuration string descriptors)
See documentation of hsu_str_idx_desc_type for more details

DEPENDENCIES
None.

RETURN VALUE
Index of the given string or HSU_STR_IDX_NOT_SET

SIDE EFFECTS
  None.

==============================================================================*/
juint8_t hsu_vendor_specific_get_str_idx(hsu_str_idx_desc_type str_desc);

#endif /* _HSU_VENDOR_SPECIFIC_H_ */

