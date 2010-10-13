#ifndef _HSU_CONF_SEL_COMP_UTILS_H_
#define _HSU_CONF_SEL_COMP_UTILS_H_

/*==============================================================================

     High Speed USB Composition Utilities 
        H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    This module includes various utility functions, dealing with device 
    compositions. 

  EXTERNALIZED FUNCTIONS
    hsu_conf_sel_comp_utils_comp_id_is_valid_for_hsu
    hsu_conf_sel_comp_utils_get_comp_details
    hsu_conf_sel_comp_utils_get_comp_details_by_hsu_id
    hsu_conf_sel_comp_utils_get_cur_prod_id

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.
    
  Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_conf_sel_comp_utils.h#10 $
  $DateTime: 2009/04/02 10:21:06 $ $Author: tkwon $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  06/29/07 bt  Added MTP support
  11/19/06 ke  Initial version
    
==============================================================================*/
#include "customer.h" /* for FEATURE_HSU_TEST to work */
#include "nv_items.h"
#include "hsu_conf_sel_types.h"

/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/

/*************************** HS-USB Product IDs *******************************/
/*
 * High Speed USB devices have to use a product ID that's different than any 
 * Product ID used by FS devices. 
 * HS-USB product IDs begin with an MSB of 1, that is 0x8000 and above 
 * (regular expresion: 0x[8,9,a,b,c,d,e,f]hhh where h is any hexadecimal digit).
 * The product ID should be specified in the relevant *.inf files, so that the 
 * host will be able to load the correct driver. 
 */
#define HSU_PRODUCT_ID_DIAG_NMEA_MS_MDM_NET  0x8000
#define HSU_PRODUCT_ID_DIAG_NMEA_MDM_NET     0x8001
#define HSU_PRODUCT_ID_DIAG_NMEA_MDM         0x8002

/*
 * Product IDs for compositions that do not use IAD, but enumerate with a 
 * single interface per function instead. 
 */
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MS_MDM_NET    0x9000
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM_NET       0x9001
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM           0x9002
#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_SER3_MDM_NET  0x9003
#define HSU_PROD_ID_SINGLE_IF_DIAG_MDM                0x9004
#define HSU_PROD_ID_SINGLE_IF_DIAG_MDM_NET            0x9005
#define HSU_PROD_ID_SINGLE_IF_DIAG_MS                 0x9006
#define HSU_PROD_ID_SINGLE_IF_MTP_DIAG                0x901B
/*
 * A product ID used for download into RAM in the U2 target.
 * This is intended to ensure that the device gets a different
 * port number when performing download into RAM than when running
 * AMSS.
 * This product ID always uses the single interface configuration.
 */
#define HSU_PRODUCT_ID_NON_AMSS           0x9008

/* The folowing defines the valid range for product IDs used by Linux */
#define HSU_PRODUCT_ID_LINUX_LOW_LIMIT1		  0x9010 
#define HSU_PRODUCT_ID_LINUX_HIGH_LIMIT1		  0x9020
#define HSU_PRODUCT_ID_LINUX_LOW_LIMIT2		  0xF000
#define HSU_PRODUCT_ID_LINUX_HIGH_LIMIT2		  0xF010
#define HSU_PRODUCT_ID_LINUX_LOW_LIMIT3		  0x8080
#define HSU_PRODUCT_ID_LINUX_HIGH_LIMIT3	  0x8090

/* The following defines the valid range for product IDs used by WM.
   We need 0x3200 to be legal PID, we kept a buffer of 0x20 PIDs around this PID */
#define HSU_PRODUCT_ID_WM_LOW_LIMIT 0x31F0
#define HSU_PRODUCT_ID_WM_HIGH_LIMIT 0x3210



/*
 * A product ID used for the OBEX-only composition
 * of the full-speed core (used for DIAG).
 */
#define HSU_PRODUCT_ID_FS_DIAG            0x9009

#define HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MS_MDM_NET_AUDIO 0x900A

/*
 * Product IDs that do not require a Qualcomm inf file
 * (0xF*** format is used for those )
 */ 
#define HSU_PRODUCT_ID_MS                    0xF000
#define HSU_PRODUCT_ID_PICTBRIDGE            0xF001
#define HSU_PRODUCT_ID_BREW_EXT              0xF002
#define HSU_PRODUCT_ID_MTP                   0xF003

/*
 * A product ID used for standalone HID enumeration, 
 * for fast dead battery charging
 */
#define HSU_PRODUCT_ID_HID                   0xF006
#define HSU_PRODUCT_ID_LOOPBACK              0xF008 


/******************************************************************************/


/* 
 * A define for a composition that cannot be set using the NV item, but only
 * through the dynamic composition menu (UI)
 */
#define NV_HS_USB_NON_NV_DEVICE              0xFF

/* 
 * High Speed USB device product ID - invalid value. 
 */
#define HSU_PRODUCT_ID_INVALID               0xFFFF

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

/*==============================================================================
  PUBLIC FUNCTION DECLARATIONS FOR MODULE
==============================================================================*/

/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_COMP_ID_IS_VALID_FOR_HSU

DESCRIPTION
  Checks if the given value is a valid composition ID for HS-USB.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if valid, FALSE otherwise.

SIDE EFFECTS
  None. 

==============================================================================*/
boolean hsu_conf_sel_comp_utils_comp_id_is_valid_for_hsu
(
  nv_hs_usb_supported_compositions_type nv_comp_id
);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_GET_COMP_DETAILS

DESCRIPTION
  This function returns a pointer to the information relevant for the given 
  composition ID.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to hsu_conf_sel_comp_info_type for success, 
  Currently all failures cause ASSERT.

SIDE EFFECTS
  None.

==============================================================================*/
const hsu_conf_sel_comp_info_type * hsu_conf_sel_comp_utils_get_comp_details
(
  nv_hs_usb_supported_compositions_type nv_composition_id
);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_GET_COMP_DETAILS_BY_HSU_ID

DESCRIPTION
  This function returns a pointer to the information relevant for the given 
  HSU composition ID.

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to hsu_conf_sel_comp_info_type for success, 
  Currently all failures cause ASSERT.

SIDE EFFECTS
  None.

==============================================================================*/
const hsu_conf_sel_comp_info_type * 
  hsu_conf_sel_comp_utils_get_comp_details_by_hsu_id
(
  hsu_conf_sel_composition_id_enum hsu_comp_id
);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_GET_CUR_PROD_ID

DESCRIPTION
  This function returns the product ID of the current device composition. 
  
  NOTE: this function is deprecated, and should only be used when going into 
  download mode. In all other cases, use the remote API hsu_api_get_product_id()
  directly. 
  TODO - remove this function when boot_dload.c uses hsu_api_get_product_id() 
  directly.

DEPENDENCIES
  None.

RETURN VALUE
  HSU_PRODUCT_ID_INVALID for error, the product ID for success.

SIDE EFFECTS
  None.

==============================================================================*/
uint16 hsu_conf_sel_comp_utils_get_cur_prod_id(void);


/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_PROD_ID_IS_VALID_FOR_HSU

DESCRIPTION
  Checks if the given value is a valid product ID for HS-USB.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if valid, FALSE otherwise.

SIDE EFFECTS
  None. 

==============================================================================*/
boolean hsu_conf_sel_comp_utils_prod_id_is_valid_for_hsu
(
  uint16 product_id
);

/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_GET_DEFAULT_COMP_INFO

DESCRIPTION
  Returns the composition info for the default composition. 

DEPENDENCIES
  None.

RETURN VALUE
  Pointer to the default composition's info for success, ASSERTs on failure.

SIDE EFFECTS
  None. 

==============================================================================*/
const hsu_conf_sel_comp_info_type * 
  hsu_conf_sel_comp_utils_get_default_comp_info(void);

#endif /* _HSU_CONF_SEL_COMP_UTILS_H_ */
