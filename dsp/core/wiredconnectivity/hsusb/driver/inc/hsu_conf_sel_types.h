#ifndef _HSU_CONF_SEL_TYPES_H_
#define _HSU_CONF_SEL_TYPES_H_

/*==============================================================================

     High Speed USB configuration Selector Module - Type Definitions
        I N T E R N A L  H E A D E R   F I L E
  
  GENERAL DESCRIPTION
    Type definitions used by the various config selector files. 

  EXTERNALIZED FUNCTIONS
    None.

  INITALIZATION AND SEQUENCING REQUIREMENTS
    None.
    
  Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/inc/hsu_conf_sel_types.h#8 $
  $DateTime: 2009/04/02 10:21:06 $ $Author: tkwon $
    
  when     who  what, where, why
  -------- ---  ---------------------------------------------------------
  09/12/06  ke      Initial version
  04/22/07  me      Adding constants for ESN/IMEI based serial number support
  06/29/07  bt      Added MTP support

==============================================================================*/
#include "nv_items.h"
#include "rdevmap.h"

/*==============================================================================
  Forward Declarations
==============================================================================*/

/*==============================================================================
  Constants
==============================================================================*/
/* 
* The maximum length for the serial number
*/
#define HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN 17
/* 
* The maximum length for keeping the ESN or IMEI
* The IMEI is structured from 6 digits for Type Approval Code (TAC),  
* 2 digits for Final Assembly Code (FAC), 6 digits for Serial Numberand one Check digit.
* Total of 15 digits.
* When getting it from the NV we have 2 redundant bytes - one for bcd_len and one ID digit 
* that is being ignored.
* this is why we keep size of (NV_UE_IMEI_SIZE-1)*2 = 15
*/
#define HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE (NV_UE_IMEI_SIZE-1)*2
/* 
* A constant serial number to be used when IMEI/ESN is not available 
*/
#define HSU_CFG_SELECTOR_DEFAULT_SERIAL_NUMBER "1234567890ABCDEF"
/* 
* The minimum length allowed for the serial number according to the MS standard 
*/
#define HSU_CFG_SELECTOR_SERIAL_NUMBER_MIN_LEN 12

/*==============================================================================
  Macros
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/
/*
 * Enumeration of the various FDs supported by the HS-USB device stack.
 * Used as the index for the num_of_fd_instances_in_comp array in the type 
 * hsu_conf_sel_comp_info_type
 */
enum
{
  HSU_CFG_SEL_MS_FD = 0,
  HSU_CFG_SEL_OBEX_FD,
  HSU_CFG_SEL_ACM_FD,
  HSU_CFG_SEL_ECM_FD,
  HSU_CFG_SEL_SICD_FD,
  HSU_CFG_SEL_MTP_FD,
  HSU_CFG_SEL_HID_FD,
  HSU_CFG_SEL_LOOPBACK_FD,
  HSU_CFG_SEL_AUDIO_FD,
  HSU_CFG_SEL_NUM_OF_FD_TYPES /* this has to be the last in the list! */
};

/*
 * The valid HSU compositions (NV based and dynamic). 
 * Currently: 
 * 1. DIAG & NMEA & MS & MDM & Net device
 * 2. DIAG & NMEA & MDM & Net device
 * 3. DIAG & NMEA & MDM device
 * 4. MS only device
 * 5. PictBridge device
 */
typedef enum
{
  HSU_CONF_SEL_FIRST_COMP = 0,
#ifdef FEATURE_HS_USB_MS_FD
  HSU_CONF_SEL_DIAG_NMEA_MS_MDM_NET_COMP = HSU_CONF_SEL_FIRST_COMP,
  HSU_CONF_SEL_DIAG_NMEA_MDM_NET_COMP,
#else
  HSU_CONF_SEL_DIAG_NMEA_MDM_NET_COMP = HSU_CONF_SEL_FIRST_COMP,
#endif
  HSU_CONF_SEL_DIAG_NMEA_MDM_COMP,
#ifdef FEATURE_HS_USB_SER3_PORT
  HSU_CONF_SEL_DIAG_NMEA_MDM_NET_SER3_COMP,
#endif
#ifdef FEATURE_HS_USB_MS_FD
  HSU_CONF_SEL_MS_COMP,
#endif
  HSU_CONF_SEL_PICTBRIDGE_COMP,
  HSU_CONF_SEL_DIAG_MDM_COMP,
  HSU_CONF_SEL_DIAG_MDM_NET_COMP,
#ifdef FEATURE_HSU_MTP
  HSU_CONF_SEL_BREW_EXT_COMP,
  HSU_CONF_SEL_MTP_COMP,
  HSU_CONF_SEL_MTP_DIAG_COMP,
#endif
#ifdef FEATURE_HS_USB_MS_FD
  HSU_CONF_SEL_DIAG_MS_COMP,
#endif  
#ifdef FEATURE_HS_USB_SAMPLE_LOOPBACK
  HSU_CONF_SEL_LOOPBACK_COMP,
#endif   
#ifdef FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE
  HSU_CONF_SEL_DIAG_NMEA_MS_MDM_NET_AUDIO_COMP,
#endif /* FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE */
  HSU_CONF_SEL_NUM_OF_COMPS /* must be the last one! */
} hsu_conf_sel_composition_id_enum;

/* 
 * The number of RDM enum values which represent USB ports 
 * Currently 5: RDM_USB_MDM_DEV, RDM_USB_MMC_DEV, RDM_USB_DIAG_DEV, 
 *              RDM_USB_NMEA_DEV, RDM_USB_NET_WWAN_DEV,
 */
#define HSU_NUM_OF_RDM_USB_PORTS 5

/*
 * A structure that contains all the relevant information for a composition.
 */
typedef struct 
{
  /* The HSU composition id representing this composition */
  hsu_conf_sel_composition_id_enum  hsu_comp_id;

  /* The NV item value representing this composition (NV_HS_USB_NON_NV_DEVICE if
     the composition cannot be chosen using an NV item). */
  nv_hs_usb_supported_compositions_type  nv_comp_id;

  /* The product ID for this composition */
  uint16  product_id;

  /* For each type of FD, the number of instances used in the composition. */
  uint8   num_of_fd_instances_in_comp[HSU_CFG_SEL_NUM_OF_FD_TYPES];

  /* 
  * Array of the RDM devices (USB ports) that participate in the composition.
  * Used for dynamic composition changes - 
  * we go over them one by one and call rdm_get_service. If there is in fact a 
  * service that uses this USB port, we call rdm_assign_port for that service,
  * with NULL as the destination port.
  */
  uint8   num_of_rdm_dev_in_comp;

  rdm_device_enum_type rdm_devices[HSU_NUM_OF_RDM_USB_PORTS];

  /* MS OS Feature Descriptor */
  void * ms_os_descriptor;
} hsu_conf_sel_comp_info_type;

#endif /* _HSU_CONF_SEL_TYPES_H_ */
