/*==============================================================================

  High Speed USB Composition Utilities 
  
  GENERAL DESCRIPTION
    This module includes various utility functions, dealing with device 
    compositions. 
      
  EXTERNALIZED FUNCTIONS
    hsu_conf_sel_comp_utils_comp_id_is_valid_for_hsu
    hsu_conf_sel_comp_utils_get_comp_details
    hsu_conf_sel_comp_utils_get_comp_details_by_hsu_id
    hsu_conf_sel_comp_utils_get_cur_prod_id
    hsu_conf_sel_comp_utils_prod_id_is_valid_for_hsu

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.

  Copyright (c) 2006 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_conf_sel_comp_utils.c#15 $
  $DateTime: 2009/04/02 10:21:06 $ $Author: tkwon $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  06/29/07  bt      Added MTP (IUSBDriver) support
  09/12/06  ke      Initial version
    
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "hsu_conf_sel_comp_utils.h"
#include "hsu_conf_sel_i.h"
#include "hsu_conf_sel_nv.h"
#include "hsu_common.h"
#include "hsu_app_apis.h"

/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================
Constants and Macros
==============================================================================*/
/*==============================================================================
  Typedefs
==============================================================================*/
/* Microsoft OS Feature Descriptor */
typedef PACKED struct {
    uint32      dwLength;           /* length in byte   */
    uint16      bcdVersion;         /* release number in BCD format */
    uint16      wIndex;             /* Fixed = 0x 0004 */
    uint8       bCount;             /* Total number of function sections = 0x01 */
    uint8       reserved_1[7];      /* Reserved */
    
} ms_os_descr_header_t;

typedef PACKED struct {
    uint8       bFirstInterfaceNumber;  /* starting interface number = 0x00 */
    uint8       bInterfaceCount;        /* total number of interfaces  */ 
    uint8       compatibleID[8];        /* compatible ID = MTP */
    uint8       subCompatibleID[8];     /* Sub compatible ID */ 
    uint8       reserved_2[6];          /* Reserved */
    
} ms_os_descr_function_t;

/*==============================================================================
  Variables
==============================================================================*/

/* MS OS Feature Descriptor for MTP composition */
const PACKED struct {
  ms_os_descr_header_t header;
  ms_os_descr_function_t mtp_function;
} hsu_mtp_only_msos_descriptor = {
  { 
    (sizeof(ms_os_descr_header_t) + sizeof(ms_os_descr_function_t)),
    0x0100,
    0x0004,
    1,
    {0,0,0,0,0,0,0}
   },
   {
    0, /* interface number */
    1, /* interface count */
    {'M','T','P',0,0,0,0,0},
    {0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0}
   }
};

/* MS OS Feature Descriptor for MTP + DIAG composition */
const PACKED struct {
  ms_os_descr_header_t header;
  ms_os_descr_function_t mtp_function;
  ms_os_descr_function_t diag_function;
} hsu_diag_mtp_msos_descriptor = {
  {
   (sizeof(ms_os_descr_header_t) + sizeof(ms_os_descr_function_t)*2),
   0x0100,
   0x0004,
   2,
   {0,0,0,0,0,0,0}
  },
  { /* MTP Interface */
   0, /* interface number */
   1, /* interface count */
   {'M','T','P',0,0,0,0,0},
   {0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0}
  },  
  { /* Diag Interface*/
   1, /* interface number */
   1, /* interface count */
   {0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0,0,0},
   {0,0,0,0,0,0}
  }
};

static const hsu_conf_sel_comp_info_type 
  compositions_details[HSU_CONF_SEL_NUM_OF_COMPS] = 
{
#ifdef FEATURE_HS_USB_MS_FD
  { /* DIAG & NMEA & MS & MDM & Net device */
    HSU_CONF_SEL_DIAG_NMEA_MS_MDM_NET_COMP,
    NV_HS_USB_DIAG_NMEA_MS_MDM_NET,
    HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MS_MDM_NET,
    {1,2,1,1,0,0,0,0,0},
    5,
    {RDM_USB_MMC_DEV, RDM_USB_DIAG_DEV, RDM_USB_NMEA_DEV, 
     RDM_USB_MDM_DEV, RDM_USB_NET_WWAN_DEV}
     ,NULL
 },
#endif /* FEATURE_HS_USB_MS_FD */
  { /* DIAG & NMEA & MDM & Net device */
    HSU_CONF_SEL_DIAG_NMEA_MDM_NET_COMP,
    NV_HS_USB_DIAG_NMEA_MDM_NET,
    HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM_NET,
    {0,2,1,1,0,0,0,0,0},
    4,
    {RDM_USB_DIAG_DEV, RDM_USB_NMEA_DEV, RDM_USB_MDM_DEV, 
     RDM_USB_NET_WWAN_DEV, RDM_SRVC_NOT_ALLOWED}
     ,NULL
  },
  { /* DIAG & NMEA & MDM device */
    HSU_CONF_SEL_DIAG_NMEA_MDM_COMP,
    NV_HS_USB_DIAG_NMEA_MDM,
    HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MDM,
    {0,2,1,0,0,0,0,0,0},
    3,
    {RDM_USB_DIAG_DEV, RDM_USB_NMEA_DEV, RDM_USB_MDM_DEV, 
     RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED}
     ,NULL
  },
#ifdef FEATURE_HS_USB_SER3_PORT
    { /* DIAG & NMEA & MDM & Net & SER3 device */
      HSU_CONF_SEL_DIAG_NMEA_MDM_NET_SER3_COMP,
      NV_HS_USB_DIAG_NMEA_SER3_MDM_NET,
      HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_SER3_MDM_NET,
      {0,3,1,1,0,0,0,0,0},
      4,
      {RDM_USB_DIAG_DEV, RDM_USB_NMEA_DEV, RDM_USB_MDM_DEV, 
      RDM_USB_NET_WWAN_DEV, RDM_SRVC_NOT_ALLOWED}
     ,NULL
    },
#endif
#ifdef FEATURE_HS_USB_MS_FD
  { /* MS only device */
    HSU_CONF_SEL_MS_COMP,
    NV_HS_USB_MS,
    HSU_PRODUCT_ID_MS,
    {1,0,0,0,0,0,0,0,0},
    1,
    {RDM_USB_MMC_DEV, RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED, 
     RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED}
     ,NULL
  },
#endif /* FEATURE_HS_USB_MS_FD */
  { /* PictBridge device */
    HSU_CONF_SEL_PICTBRIDGE_COMP,
    NV_HS_USB_NON_NV_DEVICE, /* this composition cannot be chosen through NV */
    HSU_PRODUCT_ID_PICTBRIDGE,
    {0,0,0,0,1,0,0,0,0},
    0,
    {RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED, 
     RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED}
     ,NULL
  },
  { /* DIAG & MDM  device */
    HSU_CONF_SEL_DIAG_MDM_COMP,
    NV_HS_USB_DIAG_MDM,
    HSU_PROD_ID_SINGLE_IF_DIAG_MDM,
    {0,1,1,0,0,0,0,0,0},
    2,
    {RDM_USB_DIAG_DEV, RDM_USB_MDM_DEV, RDM_SRVC_NOT_ALLOWED,
     RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED}
     ,NULL
  },
  { /* DIAG & MDM & Net device */
    HSU_CONF_SEL_DIAG_MDM_NET_COMP,
    NV_HS_USB_DIAG_MDM_NET,
    HSU_PROD_ID_SINGLE_IF_DIAG_MDM_NET,
    {0,1,1,1,0,0,0,0,0},
    3,
    {RDM_USB_DIAG_DEV, RDM_USB_MDM_DEV, RDM_USB_NET_WWAN_DEV, 
     RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED}
     ,NULL
  }
#ifdef FEATURE_HSU_MTP
  ,
  { /* BREW Extended dev (for using Blaster tool with BREW USBDriverDiag app) */
    HSU_CONF_SEL_BREW_EXT_COMP,
    NV_HS_USB_NON_NV_DEVICE,  /* this composition cannot be chosen through NV */
    HSU_PRODUCT_ID_BREW_EXT,
    {0,0,0,0,0,1,0,0,0},
    0,
    {RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED, 
     RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED}
     ,NULL
  },
  { /* MTP dev only  */
    HSU_CONF_SEL_MTP_COMP,
    NV_HS_USB_MTP,  /* this composition can be chosen through NV */
    HSU_PRODUCT_ID_MTP,
    {0,0,0,0,0,1,0,0,0},
    0,
    {RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED, 
     RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED}
     ,(void *)&hsu_mtp_only_msos_descriptor
  },
  { /* MTP + DIAG */
    HSU_CONF_SEL_MTP_DIAG_COMP,
    NV_HS_USB_DIAG_MTP,  /* this composition can be chosen through NV */
    HSU_PROD_ID_SINGLE_IF_MTP_DIAG,
    {0,1,0,0,0,1},
    1,
    {RDM_USB_DIAG_DEV, RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED, 
     RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED}
    ,(void *)&hsu_diag_mtp_msos_descriptor
  }
#endif /* FEATURE_HSU_MTP */
#ifdef FEATURE_HS_USB_MS_FD
  ,
  { /* Diag and Mass Storage */
    HSU_CONF_SEL_DIAG_MS_COMP,
    NV_HS_USB_DIAG_MS,	
    HSU_PROD_ID_SINGLE_IF_DIAG_MS,
    {1,1,0,0,0,0,0,0,0},
    2,
    {RDM_USB_MMC_DEV, RDM_USB_DIAG_DEV, RDM_SRVC_NOT_ALLOWED, 
     RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED}
     ,NULL
  }
#endif /* FEATURE_HS_USB_MS_FD */
#ifdef FEATURE_HS_USB_SAMPLE_LOOPBACK
  ,
  { /* Loop  */
    HSU_CONF_SEL_LOOPBACK_COMP,
    NV_HS_USB_LOOPBACK,  /* this composition can be chosen through NV */
    HSU_PRODUCT_ID_LOOPBACK,
    {0,0,0,0,0,0,0,1,0},
    0,
    {RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED, 
     RDM_SRVC_NOT_ALLOWED, RDM_SRVC_NOT_ALLOWED}
     ,NULL
  }
#endif /* FEATURE_HS_USB_SAMPLE_LOOPBACK */
#ifdef FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE
  ,
  { /* DIAG & NMEA & MS & MDM & Net $ Digital Audio device */
    HSU_CONF_SEL_DIAG_NMEA_MS_MDM_NET_AUDIO_COMP,
    NV_HS_USB_DIAG_NMEA_MS_MDM_NET_AUDIO, 
    HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MS_MDM_NET_AUDIO,
    {1,2,1,1,0,0,0,0,1},
    5,
    {RDM_USB_MMC_DEV, RDM_USB_DIAG_DEV, RDM_USB_NMEA_DEV, 
     RDM_USB_MDM_DEV, RDM_USB_NET_WWAN_DEV}
     ,NULL
  }
#endif /* FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE */
};

/*
 * The NV value for the default composition. 
 * This is used in case the NV item is not initialized properly on the phone. 
 */
#ifdef FEATURE_HSU_TEST
#error code not present
#else /* ! FEATURE_HSU_TEST */
  #ifdef FEATURE_HSU_EBI
#error code not present
  #else /* ! FEATURE_HSU_EBI */
    #ifdef FEATURE_HS_USB_MEM_OPTIMIZATION
    static const uint16 default_comp_nv_value = NV_HS_USB_DIAG_MDM;
    #else
    static const uint16 default_comp_nv_value = NV_HS_USB_DIAG_NMEA_MDM;
    #endif /* FEATURE_HS_USB_MEM_OPTIMIZATION */
  #endif /* FEATURE_HSU_EBI */
#endif /* FEATURE_HSU_TEST */

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

/*==============================================================================

FUNCTION          HSU_CONF_SEL_COMP_UTILS_COMP_ID_IS_VALID_FOR_HSU

DESCRIPTION
  Checks if the given value is a valid composition ID for HS-US.

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
)
{
  uint8 i;

#ifdef FEATURE_HS_USB_MEM_OPTIMIZATION
  switch(nv_comp_id)
  {
    case  NV_HS_USB_MS:
    case  NV_HS_USB_DIAG_MDM:
    case  NV_HS_USB_DIAG_MS:
      /* only these compositions are supported and go through to normal 
            * operation to find corresponding composition */
      break;

    default:
      /* The other compositions are not supported. return with flase immediately */
      return FALSE;
  }
#endif /* FEATURE_HS_USB_MEM_OPTIMIZATION */

  for( i = 0 ; i < (uint8)HSU_CONF_SEL_NUM_OF_COMPS ; ++i)
  {
    if(compositions_details[i].nv_comp_id == nv_comp_id)
    {
      return TRUE;
    }
  }
  return FALSE;
}/* hsu_conf_sel_comp_utils_comp_id_is_valid_for_hsu */

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
)
{
  uint16 i;

  for( i = 0 ; i < (uint8)HSU_CONF_SEL_NUM_OF_COMPS ; ++i )
  {
    if(compositions_details[i].nv_comp_id == nv_composition_id)
    {
      return &compositions_details[i];
    }
  }
  HSU_MSG_ERROR("*_get_comp_details(): Invalid NV composition ID %d.", 
    nv_composition_id,0,0);
  HSU_ASSERT(
    (char *)"*_get_comp_details(): Invalid NV composition ID." == NULL);
  return NULL; /* to please the compiler... */
} /* hsu_conf_sel_comp_utils_get_comp_details */

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
)
{
  HSU_ASSERT(hsu_comp_id < HSU_CONF_SEL_NUM_OF_COMPS);
  HSU_ASSERT(compositions_details[hsu_comp_id].hsu_comp_id == hsu_comp_id);
  return &compositions_details[hsu_comp_id];
} /* hsu_conf_sel_comp_utils_get_comp_details */

#if !(defined(FEATURE_HS_USB_BASIC) || defined (T_REXNT))
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
uint16 hsu_conf_sel_comp_utils_get_cur_prod_id(void)
{
  /* Use the remote API */
  return hsu_api_get_product_id();
} /* hsu_conf_sel_comp_utils_get_cur_prod_id */
#endif /* !(defined(FEATURE_HS_USB_BASIC) || defined (T_REXNT)) */

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
)
{
  uint8 i;

  for( i = 0 ; i < (uint8)HSU_CONF_SEL_NUM_OF_COMPS ; ++i)
  {
    if(compositions_details[i].product_id == product_id)
    {
      return TRUE;
    }
  }

  /* The non AMSS product_id (used by MDM1000) doesn't appear in any composition */
  if (product_id == HSU_PRODUCT_ID_NON_AMSS)
  {
    return TRUE;
  }

  /* Product IDs used by Linux: valid range is 0x9010 – 0x901F, 0xF000 – 0xF00F and 0x8080 - 0x8090 */
  if ((product_id >= HSU_PRODUCT_ID_LINUX_LOW_LIMIT1) && 
	  (product_id <= HSU_PRODUCT_ID_LINUX_HIGH_LIMIT1))
  {
    return TRUE;
  }

  if ((product_id >= HSU_PRODUCT_ID_LINUX_LOW_LIMIT2) && 
	  (product_id <= HSU_PRODUCT_ID_LINUX_HIGH_LIMIT2))
  {
    return TRUE;
  }

  if ((product_id >= HSU_PRODUCT_ID_LINUX_LOW_LIMIT3) && 
    (product_id <= HSU_PRODUCT_ID_LINUX_HIGH_LIMIT3))
  {
    return TRUE;
  }

  /* Support WM product IDs */
  if ((product_id >= HSU_PRODUCT_ID_WM_LOW_LIMIT) && 
      (product_id <= HSU_PRODUCT_ID_WM_HIGH_LIMIT))
  {
    return TRUE;
  }

  return FALSE;
}/* hsu_conf_sel_comp_utils_prod_id_is_valid_for_hsu */


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
  hsu_conf_sel_comp_utils_get_default_comp_info(void)
{
  const hsu_conf_sel_comp_info_type * default_comp_info = 
      hsu_conf_sel_comp_utils_get_comp_details(default_comp_nv_value);
  if(default_comp_info == NULL)
  {
    HSU_ASSERT(
      "*_get_default_comp_info(): hsu_conf_sel_comp_utils_get_comp_details failed" 
      == NULL);
    return NULL;
  }
  return default_comp_info;
}/* hsu_conf_sel_comp_utils_prod_id_is_valid_for_hsu */
