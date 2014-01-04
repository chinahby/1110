 /*==============================================================================

  High Speed USB Configuration Selector Stack Utilities Module
  
  GENERAL DESCRIPTION
    This module defines function that setup, stop, re-configure, etc. the USB 
    stack.
      
  EXTERNALIZED FUNCTIONS
    hsu_conf_sel_stack_utils_init_memory
    fhsu_conf_sel_stack_utils_setup_device_stack
    hsu_conf_sel_stack_utils_switch_device_stack
    hsu_conf_sel_stack_utils_handle_chg_connected
    hsu_conf_sel_stack_utils_handle_chg_disconnected
    hsu_conf_sel_stack_utils_enable_perf_boost
    hsu_conf_sel_stack_utils_device_restart_core

    hsu_conf_sel_stack_utils_is_forced_disable
    hsu_conf_sel_stack_utils_force_disable_start
    hsu_conf_sel_stack_utils_force_disable_stop
    hsu_conf_sel_stack_utils_disable
    hsu_conf_sel_stack_utils_enable

 INITALIZATION AND SEQUENCING REQUIREMENTS
    hsu_conf_sel_stack_utils_init_memory should be called before any other 
    function, and before the CDC ALs are initialized. 

  Copyright (c) 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                            EDIT HISTORY FOR MODULE
  
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/common/hsu_conf_sel_stack_utils.c#67 $
  $DateTime: 2009/04/02 10:21:06 $ $Author: tkwon $
    
  when      who     what, where, why
  --------  ---     ------------------------------------------------------------
  12/06/06  ke      Initial version
  04/22/07  me      Adding logic for setting the serial number
  06/29/07  bt      Added MTP (IUSBDriver) support  
  01/14/08  ab      Added HID FD support for fast charging
==============================================================================*/

/*==============================================================================

                            INCLUDE FILES FOR MODULE
  
==============================================================================*/
#include "customer.h" /* for FEATURE_HSU_TEST to work */
#include "hsu_conf_sel_stack_utils.h"
#include "hsu_conf_sel_i.h"
#include "hsu_common.h"

#if (defined(FEATURE_HS_USB_LEGACY_DEVICE) && !defined(FEATURE_HS_USB_BASIC))
  #include "hsu_conf_sel_comp_utils.h"
#endif

#ifdef FEATURE_HS_USB_MS_FD
  #include "hsu_al_ms.h"
#endif
#include "jtypes.h"
#include "jslave_init.h"

#ifndef FEATURE_HS_USB_BASIC /* DLOAD/ARMPROG */
  #include "err.h"
  #include "hsu_mdm_apis.h"
  #include "hsu_conf_sel_nv.h"
  #include "hsu_conf_sel_types.h"
  #include "AEEstd.h"
  #include "hsu_phy_utils.h"
#ifdef FEATURE_HS_USB_ANALOG_AUDIO
  #include "hsu_analog_audio.h"
#endif /*FEATURE_HS_USB_ANALOG_AUDIO*/
#ifndef T_REXNT
  #include "msm.h"
#endif
#else
  #include "hsu_boot_dload.h"
  #include "boot_dload.h"
  #include "hsu_conf_sel_comp_utils.h"
#endif /* FEATURE_HS_USB_BASIC */

#ifndef T_REXNT
#include <hsu_os_abstract_bus.h>
#endif  /* !T_REXNT */

#ifndef FEATURE_HSU_TEST
  #ifdef FEATURE_PMEM
  #include "pmem.h"
  #endif
#include "hw.h" /* hw_partnum_version() */
#include "msm.h"

#include "event.h"
#include "hsu_event_defs.h"
#endif /*FEATURE_HSU_TEST*/

#ifdef FEATURE_HS_USB_CHARGING
  #ifndef FEATURE_HS_USB_BASIC
    #include "hsu_chg_api_helper.h"
  #endif /* FEATURE_HS_USB_BASIC */
  #include "hsu_chg_wallchg.h"
#endif /* FEATURE_HS_USB_CHARGING */

#ifdef T_UNDP
#include "undp_hsu_vid_pid.h"
#endif /* T_UNDP */

#if !defined(FEATURE_HS_USB_BASIC) && defined (FEATURE_HSU_MTP)
#include "mtp_fd.h"
#endif /* ! FEATURE_HS_USB_BASIC && FEATURE_HSU_MTP */

#if defined(FEATURE_HS_USB_BASIC) && defined (FEATURE_HS_USB_FAST_CHARGE_VIA_HID)
#include "hsu_hid_fd.h"
#endif /* FEATURE_HS_USB_BASIC && FEATURE_HS_USB_FAST_CHARGE_VIA_HID */

#if !defined(FEATURE_HS_USB_BASIC) && defined(FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D)
#include "hsu_audio_device_sample.h"
#endif /* FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D */

#include "hsu_dev_app.h"

#ifndef FEATURE_HS_USB_BASIC
#ifdef FEATURE_HS_USB_CHARGING
#ifdef FEATURE_HS_USB_CHG_REMOTE_API
#include "charger.h"
#endif /* FEATURE_HS_USB_CHG_REMOTE_API */
#endif /* FEATURE_HS_USB_CHARGING */
#endif /* !FEATURE_HS_USB_BASIC */

#include "hsu_al_task.h"

#ifdef FEATURE_HS_USB_SAMPLE_LOOPBACK
#include "hsu_fd_sample_loopback.h"
#endif /* FEATURE_HS_USB_SAMPLE_LOOPBACK */

#ifdef FEATURE_HS_USB_OTG
#error code not present
#endif /*FEATURE_HS_USB_OTG*/

#include "hsu_vendor_specific.h"

#ifdef FEATURE_HS_USB_HAL
#include "HAL_hsusb.h"
#endif /* FEATURE_HS_USB_HAL */

#include "hsu_conf_sel_core_config.h"
#include "hsu_core_config.h"


/*==============================================================================

                          DEFINITIONS AND DECLARATIONS FOR MODULE
  
==============================================================================*/
#ifdef T_REXNT
#error code not present
#endif /* T_REXNT */
/*==============================================================================
Constants and Macros
==============================================================================*/

/**************************************************
 * Definitions relevant for the Device Descriptor *
 **************************************************/
/* For the next defines, the reference was usbqcdev.h lines 58-60 */
#define HSU_DEVICE_CLASS_CODE              0x00
#define HSU_DEVICE_SUBCLASS_CODE           0x00
#define HSU_DEVICE_PROTOCOL_CODE           0x00
/* The reference was Jungo code, before the value was exposed */
#define HSU_DEVICE_BCD_USB                 0x0200

/* Taken from usbcdc.h lines 152-153 */
#define HSU_QC_VENDOR_ID_CODE              0x05C6

/* Used to provide a device version number, in the bcdDevice field. 
   This value is assigned by the developer. */
#ifdef T_UNDP
#define HSU_QC_DEVICE_REL_VER              0x0002
#else
#define HSU_QC_DEVICE_REL_VER              0x0000
#endif

/* Taken from services\dload\dloadusb.c line 447 */
#define HSU_MANUFACTURER_STRING  "Qualcomm, Incorporated"

/* Taken from services\dload\dloadusb.c line 447 */
#ifdef FEATURE_VERSION_K232_Y100A
#define HSU_PRODUCT_STRING       "Micromax C205"
#else
#define HSU_PRODUCT_STRING       "Qualcomm CDMA Technologies MSM"
#endif
/* String describing the configuration */
#define HSU_CONFIGURATION_STRING  "Qualcomm Configuration"

/* this should be defined in hw.h but it's not there yet */
#define HW_MSM7600_REV12_VERSION_ID 0x2

#ifndef FEATURE_HS_USB_HAL
/* HWIO_USB_OTG_HS_AHB_MODE_OUT definition is missing in some of the targets 
   TODO: remove this when HWIO_USB_OTG_HS_AHB_MODE_OUT is defined */
#ifndef HWIO_USB_OTG_HS_AHB_MODE_OUT
#define HWIO_USB_OTG_HS_AHB_MODE_OUT(v)                                     \
  out_dword(HWIO_USB_OTG_HS_AHB_MODE_ADDR,v)
#endif
#endif /* FEATURE_HS_USB_HAL */

/*==============================================================================
  Typedefs
==============================================================================*/

typedef struct {
  juint8_t core;
  device_params_t *new_config;
  reconfig_complete_cb_t cb;
  void *arg;
} hsu_jslave_reconfig_param_type;

/*==============================================================================
  Variables
==============================================================================*/
/*
 * Definitions related to the memory space used by the HS-USB stack 
 * (The size should be divisible by 32)
 */
#ifdef FEATURE_HSU_EBI
#error code not present
#endif /* FEATURE_HSU_EBI */

#if defined(FEATURE_HS_USB_BASIC)
  #define HSU_CFG_SELECTOR_SIZE_OF_STACK_MEM_POOL (64*1024)  /* 64 KB  */
#elif defined(FEATURE_HS_USB_MEM_OPTIMIZATION)
#ifdef FEATURE_HS_USB_MS_FD // Gemsea Modify
  #define HSU_CFG_SELECTOR_SIZE_OF_STACK_MEM_POOL ((HSU_SCSI_BUFFER_SIZE*2)+(24*1024))
#else
  #define HSU_CFG_SELECTOR_SIZE_OF_STACK_MEM_POOL (24*1024)  /* 24 KB */
#endif
#else

    #ifdef FEATURE_HS_USB_HOST_MS_PERF
#error code not present
    #elif defined(FEATURE_HS_USB_OBEX_PERF_TESTS)
#error code not present
    #else
      /* normal Full UI build */
      /* TODO: this was increased by 64KB for UICC support. Need to
    understand how to better featurize this. */
  #define HSU_STACK_MEM_POOL_SIZE_PRIMARY (128*1024)
  #endif

  #ifdef FEATURE_HS_USB_OTG
#error code not present
  #else
  #define HSU_STACK_MEM_POOL_ADDITION_OTG 0
  #endif /* FEATURE_HS_USB_OTG */

  #ifdef FEATURE_HS_USB_MS_FD
  /* MS FD uses 2 buffers of SCSI_BUFFER_SIZE */
  #define HSU_STACK_MEM_POOL_ADDITION_MS_FD (HSU_SCSI_BUFFER_SIZE*2)
  #else
  #define HSU_STACK_MEM_POOL_ADDITION_MS_FD 0
  #endif /* FEATURE_HS_USB_MS_FD */

  #ifdef FEATURE_HS_USB_UICC
  /* 32k For USB UICC MS LUN */
  /* 48k were added in a trial and error process */
  #define HSU_STACK_MEM_POOL_ADDITION_UICC (32*1024 + 48*1024)
  #else
  #define HSU_STACK_MEM_POOL_ADDITION_UICC 0
  #endif /* FEATURE_HS_USB_UICC */

  #ifdef FEATURE_HS_USB_TLP
    #if defined(HSUSB_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES)
    /* ECM TLP buffers with user defined size in bytes */
    #define HSU_STACK_MEM_POOL_ADDITION_TLP ((HSUSB_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES)*2)
    #else
    /* ECM TLP buffers are 16KB instead of 1514bytes */
    #define HSU_STACK_MEM_POOL_ADDITION_TLP (16*2*1024) /* 2 read buffers, 1 write buffer. */
    #endif /* HSUSB_ECM_TX_DATA_BUFFER_SIZE_IN_BYTES */
  #else
  #define HSU_STACK_MEM_POOL_ADDITION_TLP 0
  #endif /* FEATURE_HS_USB_TLP */
  
  #ifdef FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D
    #define HSU_STACK_MEM_POOL_ADDITION_AUDIO_LOOPBACK HSU_AUDIO_LOOPBACK_STACK_SIZE
  #else
    #define HSU_STACK_MEM_POOL_ADDITION_AUDIO_LOOPBACK 0
  #endif /* FEATURE_HS_USB_DIGITAL_AUDIO_LOOPBACK_H2D */
  
  #define HSU_CFG_SELECTOR_SIZE_OF_STACK_MEM_POOL \
            (HSU_STACK_MEM_POOL_SIZE_PRIMARY+     \
             HSU_STACK_MEM_POOL_ADDITION_OTG+     \
             HSU_STACK_MEM_POOL_ADDITION_MS_FD+   \
             HSU_STACK_MEM_POOL_ADDITION_UICC+    \
             HSU_STACK_MEM_POOL_ADDITION_TLP+     \
             HSU_STACK_MEM_POOL_ADDITION_AUDIO_LOOPBACK)

#endif  /* FEATURE_HS_USB_BASIC */

  static byte *   memory_for_stack_ptr = NULL; 
  static uint32   size_of_memory_for_stack = HSU_CFG_SELECTOR_SIZE_OF_STACK_MEM_POOL;
  static boolean  memory_requests_are_allowed = TRUE;

  /* Params used for checking if there are not two consecutive calls to 
  device configured (or two for not configured) */
  static boolean configured_cb_was_called_before = FALSE;
  static jbool_t dev_is_configured;

  #ifndef FEATURE_HS_USB_BASIC /* DLOAD/ARMPROG */
    /* Wall charger detected flag */
      boolean hsu_conf_sel_stack_utils_is_wall_charger;
    static char hsu_conf_sel_stack_utils_serial_number[HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN];
  #endif /* ! FEATURE_HS_USB_BASIC */

  #ifdef T_UNDP
    static UNDP_hsu_VID_PID_type hsu_dynamic_vid_pid;
  #endif /* T_UNDP */

#ifndef T_REXNT
/* For targets that do not have PMEM, we need to have something 
   equivalent to PMEM.  QSC1100 is such a target. */
#ifndef FEATURE_PMEM
#pragma arm section zidata = "PMEM_DATA_ZI"

byte usb_buffer[HSU_CFG_SELECTOR_SIZE_OF_STACK_MEM_POOL + 32];

#pragma arm section zidata
#endif /* !FEATURE_PMEM */

#else  /* T_REXNT */
#error code not present
#endif /* #ifndef T_REXNT */

#ifndef FEATURE_HS_USB_BASIC 
static hsu_jslave_reconfig_param_type  jslave_reconfig_param;

static boolean hsu_jslave_reconfig_started = FALSE;
#endif /*!FEATURE_HS_USB_BASIC */

#ifndef FEATURE_HS_USB_BASIC
/* describes whether USB core is in forced_disable mode */
static boolean hsu_stack_is_forced_disable = FALSE;
#endif

/*==============================================================================

                          FUNCTION DECLARATIONS FOR MODULE
  
==============================================================================*/

#ifndef FEATURE_HS_USB_BASIC 
static void hsu_jslave_reconfig(void * args);
#endif /* !FEATURE_HS_USB_BASIC */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_INIT_MEMORY

DESCRIPTION
  This function initializes variables related to the memory given to the stack. 
  
DEPENDENCIES
  Called once, when the device stack is first setup, before ALs are initialized.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
void hsu_conf_sel_stack_utils_init_memory(void)
{
  #if ((defined FEATURE_HS_USB_BASIC || defined T_REXNT))
  static byte 
    hs_usb_stack_memory_buffer[HSU_CFG_SELECTOR_SIZE_OF_STACK_MEM_POOL + 32];
  #endif

  #ifdef FEATURE_HSU_EBI
#error code not present
  #elif defined(FEATURE_HS_USB_BASIC)
    memory_for_stack_ptr = hs_usb_stack_memory_buffer;
  #elif !defined(FEATURE_PMEM)
    /* Added orginally for QSC1100.  It does not have PMEM. */
    memory_for_stack_ptr = (byte *)usb_buffer;
    memory_for_stack_ptr = HSU_ALIGN_PTR_TO_32_BYTE(memory_for_stack_ptr);
  #else
    /* Otherwise, the memory should be allocated from system memory on SMI/DDR. 
    This memory area MUST be non-cached */
    #if defined(IMAGE_APPS_PROC)
      memory_for_stack_ptr = (byte*)pmem_malloc(
                                  HSU_CFG_SELECTOR_SIZE_OF_STACK_MEM_POOL + 32, 
                                  PMEM_USB_ID); 
    #else
      memory_for_stack_ptr = (byte*)pmem_malloc(
                                  HSU_CFG_SELECTOR_SIZE_OF_STACK_MEM_POOL + 32, 
                                  PMEM_UNCACHED_ID); 
    #endif /* IMAGE_APPS_PROC */
    memory_for_stack_ptr = HSU_ALIGN_PTR_TO_32_BYTE(memory_for_stack_ptr);
  #endif /* FEATURE_HSU_EBI */

  HSU_ASSERT(memory_for_stack_ptr != NULL);  
}/* hsu_conf_sel_stack_utils_init_memory */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_ALLOC_STACK_MEMORY

DESCRIPTION
  This function is called by adaptation layers. If an AL needs to share data 
  with the stack (for example, buffers for TX/RX), then the AL must request this 
  memory from the hsu_config_selector. 
  It does so by calling this function with the number of bytes required, and it
  should use the memory which begins in the returned address. 
  Note: the addresses returned by this function are 32 bytes aligned, since the 
  DMOV requires buffers passed to it to be 32 bytes aligned. 

DEPENDENCIES
  This function MUST be called prior to the stack bring up (jstart_stack). 
  The memory can be used only until the AL's uninit function is called by the 
  stack. 

RETURN VALUE
  The start address of the allocated memory, or NULL if there was not enough 
  memory.

SIDE EFFECTS
  None. 

==============================================================================*/
byte * hsu_conf_sel_stack_utils_alloc_stack_memory
(
  uint32 required_space_in_bytes
)
{
  byte * memory_allocated_start_address = NULL;
  uint32 required_space_in_bytes_alligned;

  if(memory_requests_are_allowed == FALSE)
  {
    HSU_ASSERT(
      (char *)"hsu_conf_sel_stack_utils_alloc_stack_memory(): called when\
              memory requests are not allowed" == NULL);
  }

  /* make sure we're starting from a 32 bytes aligned memory start address. */
  HSU_ASSERT(HSU_ALIGN_PTR_TO_32_BYTE(memory_for_stack_ptr) == memory_for_stack_ptr);
  
  /* All memory allocations should be 32 bytes aligned. */
  required_space_in_bytes_alligned = HSU_ROUND_UP_TO_32(required_space_in_bytes);

  if(required_space_in_bytes_alligned > size_of_memory_for_stack)
  {
    HSU_ASSERT(
      (char *)"hsu_conf_sel_stack_utils_alloc_stack_memory(): bytes left < bytes requested" 
      == NULL);
    return NULL;
  }

  memory_allocated_start_address = memory_for_stack_ptr;
  memory_for_stack_ptr += required_space_in_bytes_alligned;

  size_of_memory_for_stack -= required_space_in_bytes_alligned;

  return memory_allocated_start_address;
} /* hsu_conf_sel_stack_utils_alloc_stack_memory */



/*==============================================================================

FUNCTION          BUILD_DEVICE_INFO_STRUCT

DESCRIPTION
  This function initializes a structure to be given to jslave_enable(). 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
static void build_device_info_struct
(
  uint8         core_idx,
  dev_info_t *  device_info
#ifdef T_UNDP
  ,boolean	use_amss_product_id
#endif
)
{
  hsu_core_config_type* core_config = NULL; 
  struct hsu_device_params_type* core_conf_sel_info = NULL;

  /* ***************************************************************************
  * Configuration Descriptor - 
  * For details about the content of the configuration descriptor, see section 
  * 9.6.3 and Table 9-10 in the USB 2.0 spec. 
  ****************************************************************************/
  /*
  * bmAttributes - 
  * Configuration characteristics, includes a Self-powered bit and a 
  * Remote Wakeup bit. 
  * Self-powered bit - A device configuration that uses power from the bus and a local source 
  * reports a non-zero value in bMaxPower to indicate the amount of bus power 
  * required and sets the Self-powered bit. TODO - isn't it the other way around?
  * The actual power source at runtime may be determined using the GetStatus
  * DEVICE) request (see Section 9.4.5).
  * Remote Wakeup bit - set to 1 if a device configuration supports remote wakeup.
  */
  device_info->self_powered = 1; /* TODO - verify */
  device_info->self_powered_func = NULL; /* TODO - verify */

#if defined(FEATURE_HS_USB_OTG) && !defined(FEATURE_HS_USB_BASIC)
#error code not present
#endif /* FEATURE_HS_USB_OTG && !FEATURE_HS_USB_BASIC */

  /* ***************************************************************************
  * Device Descriptor - 
  * For details about the content of the device descriptor, see section 9.6.1
  * and Table 9-8 in the USB 2.0 spec. 
  ****************************************************************************/
  /*
  * idVendor - 
  * Vendor ID (assigned by the USB-IF)
  */
  device_info->vendor = HSU_QC_VENDOR_ID_CODE;

  /*
  * idProduct - 
  * Product ID (assigned by the manufacturer)
  */
  core_config = hsu_core_config_get_core(core_idx);
  HSU_ASSERT((core_config != NULL) && (core_config->hsu_conf_sel_core_info != NULL));
  core_conf_sel_info = (struct hsu_device_params_type*)core_config->hsu_conf_sel_core_info;

  device_info->product = core_conf_sel_info->product_id;
#if !defined (FEATURE_HS_USB_BASIC) && defined (T_UNDP)
  /* Keep the new product id on the ARM9 ... UNDP_test */
  hsu_api_update_product_id(device_info->product);
#endif

  /* 
  * bcdDevice - 
  * Device version number in binary-coded decimal. Assigned by the developer. 
  */
  device_info->release = HSU_QC_DEVICE_REL_VER;
  /*
  * bDeviceClass -
  * zero means that each interface specifies its own class information, and that 
  * the various interfaces operate independently.
  */
  device_info->dev_class = HSU_DEVICE_CLASS_CODE; /* bDeviceClass */
  /*
  * bDeviceSubClass -
  * When the bDeviceClass field is zero, this field must also be set to zero.
  */
  device_info->dev_sub_class = HSU_DEVICE_SUBCLASS_CODE; 
  /* 
  * bDeviceProtocol - 
  * When this field is set to zero, the device does not use class-specific 
  * protocols on a device basis. However, it may use class specific protocols 
  * on an interface basis. 
  */
  device_info->dev_protocol = HSU_DEVICE_PROTOCOL_CODE; 

  /* String describing the product */
  device_info->product_name = HSU_PRODUCT_STRING;
  /* String describing the manufacturer */
  device_info->product_manufacturer = HSU_MANUFACTURER_STRING;
  /* String describing the configuration */
  device_info->configuration = HSU_CONFIGURATION_STRING;

#ifdef T_UNDP
  /* For UNDP (U2) data card, we need to check if dynamic VID/PID data is 
  available.  If so, use VID, PID, product_name, and product_manufacturer
  data provided by UNDP (U2).  Otherwise, keep what we have for all
  targets in general */

  /* clear the vid/pid data */
  memset(&hsu_dynamic_vid_pid, 0, sizeof(UNDP_hsu_VID_PID_type));

  /* update vid/pid data only if the API call goes through successfully */
  if ( 0 == UNDP_get_hsu_VID_PID(&hsu_dynamic_vid_pid, use_amss_product_id)  )
  {
    /* Vendor ID (assigned by the USB-IF) */
    device_info->vendor = hsu_dynamic_vid_pid.VID;

    /* Product ID (assigned by the manufacturer) */
    device_info->product = hsu_dynamic_vid_pid.PID;

    /* String describing the product */
    hsu_dynamic_vid_pid.prodStr[MAX_HSU_VID_PID_STRING_DESCIPTOR_STR_LEN] = 0;
    device_info->product_name = hsu_dynamic_vid_pid.prodStr;
        
    /* String describing the manufacturer */
    hsu_dynamic_vid_pid.mfgStr[MAX_HSU_VID_PID_STRING_DESCIPTOR_STR_LEN] = 0;
    device_info->product_manufacturer = hsu_dynamic_vid_pid.mfgStr;
  }
#endif /* T_UNDP */

#ifndef T_REXNT
  /* Set the serial number */
  hsu_conf_sel_stack_utils_set_serial_number(device_info, 
                                             device_info->product, 
                                             core_idx);
#else /* T_REXNT */
#error code not present
#endif /*#ifndef T_REXNT */

  device_info->bcdUSB = HSU_DEVICE_BCD_USB;
} /* build_device_info_struct */


/*==============================================================================

FUNCTION          BUILD_FD_STRUCT

DESCRIPTION
  This function builds the fd_instance_t structure, that is given to stack bring
  up functions, based on the number of FDs array, which is part of the 
  composition details. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
static void build_fd_struct
(
  const uint8     num_of_fd_instances_in_comp[HSU_CFG_SEL_NUM_OF_FD_TYPES],
  fd_instance_t * fds_to_init,
  uint8 *         num_of_fds
)
{
  uint8 fd_index = 0;

  HSU_ASSERT(fds_to_init != NULL);
  *num_of_fds = 0;

  /* 
  * IMPORTANT!!! 
  * OBEX must ALWAYS be the first FD to be enumerated. 
  * This is because DLOAD, before downloading a version to the phone, resets 
  * the phone and brings it up with only one OBEX FD instance (for DIAG), and 
  * the PC tries to connect to the phone on the same COM port as it was before 
  * the reset. 
  * But, due to that the MS Windows only supports interface 0 for MTP control 
  * request, MTP inferface should be located first, as well. On MTP + DIAG 
  * composition, SW download with DLOAD cannot be supported.   
  */
#if !defined(FEATURE_HS_USB_BASIC) && defined (FEATURE_HSU_MTP)
  if(num_of_fd_instances_in_comp[HSU_CFG_SEL_MTP_FD] > 0)
  {
    fds_to_init[fd_index].type = DEVICE_TYPE_VENDOR; /* required for up-init */
    fds_to_init[fd_index].count = num_of_fd_instances_in_comp[HSU_CFG_SEL_MTP_FD];
    fds_to_init[fd_index].init_func = mtpfd_init; /* register MTP FD init function */
    ++fd_index;
  }
#endif /* ! FEATURE_HS_USB_BASIC && FEATURE_HSU_MTP */

  if(num_of_fd_instances_in_comp[HSU_CFG_SEL_OBEX_FD] > 0)
  {
    fds_to_init[fd_index].type = DEVICE_TYPE_CDC_OBEX;
    fds_to_init[fd_index].count = num_of_fd_instances_in_comp[HSU_CFG_SEL_OBEX_FD];
    ++fd_index;
  }

  if(num_of_fd_instances_in_comp[HSU_CFG_SEL_MS_FD] > 0)
  {
#ifndef FEATURE_HS_USB_MS_FD
    HSU_ASSERT(
      (char*)"build_fd_struct(): can't have MS FD instance when FEATURE_HS_USB_MS_FD is not defined!" 
      == NULL);
#elif defined(FEATURE_HS_USB_BASIC) /* DLOAD/ARMPROG */
    HSU_ASSERT(
      (char*)"build_fd_struct(): can't have MS FD instance when FEATURE_HS_USB_BASIC is defined!" 
      == NULL);
#else /* ! FEATURE_HS_USB_BASIC */
    fds_to_init[fd_index].type = DEVICE_TYPE_MSD;
    fds_to_init[fd_index].count = hsu_al_ms_get_num_devices();
    ++fd_index;
#endif /* FEATURE_HS_USB_BASIC */
  }

  if(num_of_fd_instances_in_comp[HSU_CFG_SEL_ACM_FD] > 0)
  {
    fds_to_init[fd_index].type = DEVICE_TYPE_CDC_ACM;
    fds_to_init[fd_index].count = num_of_fd_instances_in_comp[HSU_CFG_SEL_ACM_FD];
    ++fd_index;
  }

  if(num_of_fd_instances_in_comp[HSU_CFG_SEL_ECM_FD] > 0)
  {
    fds_to_init[fd_index].type = DEVICE_TYPE_CDC_ECM;
    fds_to_init[fd_index].count = num_of_fd_instances_in_comp[HSU_CFG_SEL_ECM_FD];
    ++fd_index;
  }

  if(num_of_fd_instances_in_comp[HSU_CFG_SEL_SICD_FD] > 0)
  {
    fds_to_init[fd_index].type = DEVICE_TYPE_SICD;
    fds_to_init[fd_index].count = num_of_fd_instances_in_comp[HSU_CFG_SEL_SICD_FD];
    ++fd_index;
  }

#if defined(FEATURE_HS_USB_FAST_CHARGE_VIA_HID) && defined(FEATURE_HS_USB_BASIC) 
  if(num_of_fd_instances_in_comp[HSU_CFG_SEL_HID_FD] > 0)
  {
    fds_to_init[fd_index].type = DEVICE_TYPE_VENDOR;
    fds_to_init[fd_index].count = 1;
    /* register HID FD init function */
    fds_to_init[fd_index].init_func = hsu_hid_fd_init; 
    ++fd_index;
  }
#endif /* FEATURE_HS_USB_BASIC && FEATURE_HS_USB_FAST_CHARGE_VIA_HID*/

#if !defined(FEATURE_HS_USB_BASIC) && defined(FEATURE_HS_USB_SAMPLE_LOOPBACK)
    if(num_of_fd_instances_in_comp[HSU_CFG_SEL_LOOPBACK_FD] > 0)
    {
      fds_to_init[fd_index].type = DEVICE_TYPE_VENDOR;
      fds_to_init[fd_index].count = 1;
      fds_to_init[fd_index].init_func = hsu_fd_sample_loopback_init; 
      ++fd_index;
    }
#endif /* !FEATURE_HS_USB_BASIC FEATURE_HS_USB_SAMPLE_LOOPBACK */

#if !defined(FEATURE_HS_USB_BASIC) && defined (FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE)
  if(num_of_fd_instances_in_comp[HSU_CFG_SEL_AUDIO_FD] > 0)
  {
    fds_to_init[fd_index].type = DEVICE_TYPE_AUDIO; /* required for up-init */
    fds_to_init[fd_index].count = num_of_fd_instances_in_comp[HSU_CFG_SEL_AUDIO_FD];
    ++fd_index;
  }
#endif /* ! FEATURE_HS_USB_BASIC && FEATURE_HS_USB_DIGITAL_AUDIO_DEVICE */

  HSU_ASSERT(fd_index <= (uint8)HSU_CFG_SEL_NUM_OF_FD_TYPES);
  HSU_ASSERT(fd_index > 0);

  *num_of_fds = fd_index;
} /* build_fd_struct */

/*==============================================================================

FUNCTION          BUILD_CONFIG_PARAMS_STRUCT

DESCRIPTION
This function builds the config_param_t structure, that is given to stack bring
up functions, based on the number of the configurations number. 

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
static void build_config_params_struct
(
 uint8            core_idx,
 config_param_t * configs_to_init,
 uint8*           num_of_configs
 )
{
  uint8 num_of_fds = 0;
  uint8 config_idx;
  hsu_core_config_type* core_config = NULL;
  struct hsu_device_params_type* hsu_conf_sel_core_info;
  *num_of_configs = 0;

  HSU_ASSERT(configs_to_init != NULL);

  core_config = hsu_core_config_get_core(core_idx);
  HSU_ASSERT((core_config != NULL) && (core_config->hsu_conf_sel_core_info != NULL));

  hsu_conf_sel_core_info = (struct hsu_device_params_type*)core_config->hsu_conf_sel_core_info;

  for (config_idx = 0 ; config_idx < hsu_conf_sel_core_info->num_of_configs ; ++config_idx)
  {
    configs_to_init[config_idx].config_name = 
      hsu_conf_sel_core_info->configs_arr[config_idx].config_name;
    configs_to_init[config_idx].self_powered = 1; /* TODO - verify */
    configs_to_init[config_idx].remote_wakeup = 1;
    /*
    * bMaxPower - 
    * Maximum power consumption of the USB device from the bus in this specific
    * configuration when the device is fully operational. Expressed in 2 mA units
    * (i.e., 50 = 100 mA).
    */
    configs_to_init[config_idx].max_power = HSU_DEVICE_MAX_POWER;

    *num_of_configs = hsu_conf_sel_core_info->num_of_configs;

    build_fd_struct(
      hsu_conf_sel_core_info->configs_arr[config_idx].num_of_fd_instances, 
      configs_to_init[config_idx].fd_instances,
      &num_of_fds);

    configs_to_init[config_idx].num_of_fd_instances = num_of_fds;
  }
}

void dummy_std_cmd_cb(juint8_t bmRequestType, juint8_t bRequest, juint16_t wValue, juint16_t wIndex, juint16_t wLength)
{
	HSU_USE_PARAM(wIndex); /* to avoid warning */
	HSU_USE_PARAM(wLength);
  HSU_MSG_LOW("dummy_std_cmd_cb bmRequestType=%d bRequest=%d wValue=%d", bmRequestType, bRequest, wValue);
}

/*==============================================================================

FUNCTION          BUILD_DEVICE_PARAMS_STRUCT

DESCRIPTION
  This function builds the device_params_t structure given to stack bring-up 
  functions, based on the given fd_instance_t and product_id for the composition.

DEPENDENCIES
  None. 

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
static void build_device_params_struct
(
  uint8             core_idx,
  device_params_t * device_params_for_stack
#ifdef T_UNDP
  ,boolean 	use_amss_product_id
#endif
)
{
  HSU_ASSERT(device_params_for_stack != NULL);

  build_device_info_struct(core_idx, 
                           device_params_for_stack->dev_info
#ifdef T_UNDP
	                   ,use_amss_product_id
#endif
);

  /* Set the config param data of this device controller */

  build_config_params_struct(
    core_idx,
    device_params_for_stack->config_param,
    &device_params_for_stack->num_of_configs);

  device_params_for_stack->vendor_get_string_cb = NULL;
#if ((!defined FEATURE_HS_USB_BASIC) && (!defined T_REXNT))
  device_params_for_stack->vendor_cmd_in_cb = hsu_vendor_cmd_in_cb;
#else
  device_params_for_stack->vendor_cmd_in_cb = NULL;
#endif
  device_params_for_stack->vendor_cmd_out_cb = NULL;
  device_params_for_stack->vendor_cmd_out_data_stage_cb = NULL;
  device_params_for_stack->standard_cmd_cb = dummy_std_cmd_cb;
  device_params_for_stack->reserved_cmd_in_cb = NULL;
  device_params_for_stack->reserved_cmd_out_cb = NULL;
  device_params_for_stack->reserved_cmd_out_data_stage_cb = NULL;


  /* stack will automatically assign interface numbers */
  device_params_for_stack->static_interface_number = FALSE;

} /* build_device_params_struct */

#ifndef FEATURE_HSU_TEST

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_ENABLE_PERF_BOOST

DESCRIPTION
  This function configures various HW registers for throughput
  optimizations

DEPENDENCIES
  The enabling of the various HW registers are dependent on HW version.

RETURN VALUE
  None

SIDE EFFECTS
  None.

==============================================================================*/
#ifdef FEATURE_HS_USB_HAL

void hsu_conf_sel_stack_utils_enable_perf_boost(void)
{
  boolean enable_performance_boost = TRUE;

#if( defined (T_MSM7600) \
  && !(defined(T_MSM7625) || defined(T_MSM7800) || defined(T_MSM8650B) \
       || defined(T_MDM8200)) )
  {
    uint8 volatile hw_version_value;

#ifdef FEATURE_HS_USB_BASIC
    hw_version_value = HWIO_INF(HW_REVISION_NUMBER, VERSION_ID);
#else /*!FEATURE_HS_USB_BASIC*/
    {
      hw_rev_type hw_rev;
      hw_partnum_version(&hw_rev);
      hw_version_value = hw_rev.version;
    }
#endif /* FEATURE_HS_USB_BASIC */

    if (hw_version_value < HW_MSM7600_REV12_VERSION_ID)
    {
      /* disable performance boost for ROC1.0 and ROC1.1 */
      enable_performance_boost = FALSE;
    }
  }
#endif /* defined (T_MSM7600) ... */
  if (enable_performance_boost)
  {
    /* enable performance boost for ROC1.2 and greater */
    HSU_MSG_HIGH("enabling performance boost", 0, 0, 0);
    /* turn off posted writes because of HW stability issues */
    HAL_hsusb_SetXtorBypass(0,FALSE);
    HAL_hsusb_SetPostedWrites(0,TRUE);
  }
  HAL_hsusb_SetGrantStolen(0, FALSE);
  HAL_hsusb_SetHprotMode(0,HAL_HSUSB_HPROT_ALT);
  HAL_hsusb_SetAhbBurstMode(0,HAL_HSUSB_AHB_BURST_INCR);

#if defined (T_MDM8200)
  /* Enable TX_BUF_COLLISSION_MODE to enable 1-2 byte bug fix in HW */
  HAL_hsusb_SetTxBufCollisionMode(0, TRUE);
#endif /* T_MDM8200 */

}

#else /* FEATURE_HS_USB_HAL */

void hsu_conf_sel_stack_utils_enable_perf_boost(void)
{
  /* Configure AHB Translator and posted writes (chicken bit) */
#if defined(T_MDM8200)
  /* configure AHB mode */
  HWIO_OUT(USBH_USB_OTG_HS_AHB_MODE, 0x8);
  /* Enable TX_BUF_COLLISSION_MODE to enable 1-2 byte bug fix in HW */
  HWIO_OUT(USBH_USB_OTG_HS_GEN_CONFIG, 0x20);  
#elif defined(T_MSM7800) || defined(T_MSM6246) \
  || defined(T_MSM6290) || defined(T_QSC1100) || defined(T_QSC6270) \
  ||  defined(T_MSM8650B)
  HWIO_OUT(USB_OTG_HS_AHB_MODE, 0x8);
#elif defined (T_MSM7625)
  /* for sRoc, enable AHB transactor but turn off posted writes because of HW 
     stability issues */
  HWIO_OUT(USB_OTG_HS_AHB_MODE, 0x0);
  HWIO_OUT(USB_OTG_HS_AHB_BURST, 0x0);
#elif defined (T_MSM7600)
  uint8 volatile hw_version_value;

#ifdef FEATURE_HS_USB_BASIC
  hw_version_value = HWIO_INF(HW_REVISION_NUMBER, VERSION_ID);
#else /*!FEATURE_HS_USB_BASIC*/
  {
    hw_rev_type hw_rev;
    hw_partnum_version(&hw_rev);
    hw_version_value = hw_rev.version;
  }
#endif /* FEATURE_HS_USB_BASIC */

  if (hw_version_value >= HW_MSM7600_REV12_VERSION_ID)
  {
    /* enable performance boost for ROC1.2 and greater */
    HSU_MSG_HIGH("enabling performance boost (hw version %d)", hw_version_value, 0, 0);
    HWIO_OUT(USB_OTG_HS_AHBMODE, 0x2);
  }
#else /*!T_MSM7600*/
  #error "perf_boost not defined for this target"
#endif /* T_MSM7600 */
}

#endif /* FEATURE_HS_USB_HAL */

#endif /* #ifndef FEATURE_HSU_TEST*/

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_SETUP_STACK

DESCRIPTION
  This function is called both by the config_selector (when FEATURE_HSU_TEST is 
  NOT defined) and by the ARMPROG/DLOAD Adaptation Layers.
  It sets-up the USB stack (by calling jstart_stack) based on the parameters 
  given. 

DEPENDENCIES
  Assumes all AL related initialization were already done. 
  Should be called only once after power up.

RETURN VALUE
  jresult_t - HSU_JSUCCESS for success, other values for failure (currently all 
  cause an ASSERT).

SIDE EFFECTS
  None.

==============================================================================*/
jresult_t hsu_conf_sel_stack_utils_setup_stack(
#ifdef T_UNDP
  boolean 	use_amss_product_id
#endif
)
{
  device_params_t device_params_for_stack[HSU_NUM_OF_DEVICE_CONTROLLERS];
  dev_info_t      device_info[HSU_NUM_OF_DEVICE_CONTROLLERS];
  uw_hw_core_param_t      core_info[HSU_MAX_NUM_OF_CORES];
  config_param_t  config_params[HSU_NUM_OF_DEVICE_CONTROLLERS][HSU_MAX_NUM_OF_CONFIGS];
  fd_instance_t   fds_to_init[HSU_NUM_OF_DEVICE_CONTROLLERS][HSU_MAX_NUM_OF_CONFIGS][HSU_CFG_SEL_NUM_OF_FD_TYPES];
  uw_args_t       stack_init_args;
  uint32          device_controller_idx = 0;
  uint8           core_idx;
  uint8           cores_table_core_idx;
  uint32          config_idx;
  uint8           cores_table_size = 0;
  hsu_core_config_type* core_config = NULL;
  struct hsu_device_params_type* hsu_conf_sel_core_info = NULL;
#ifndef T_REXNT
  jresult_t       jresult;
#endif /* !T_REXNT */

  cores_table_size = hsu_core_config_get_cores_table_size();

  memset(&stack_init_args, 0, sizeof(stack_init_args));
  memset(&device_params_for_stack, 0, sizeof(device_params_for_stack));
  memset(&config_params, 0, sizeof(config_params));
  memset(&core_info, 0, sizeof(core_info));

  core_idx = 0;
  /* go over the cores table until we have all the active cores */
  for (cores_table_core_idx = 0; cores_table_core_idx < cores_table_size; ++cores_table_core_idx)
  {
    core_config = hsu_core_config_get_core(cores_table_core_idx);
    if (core_config == NULL)
    {
      continue; /* This core is irrelevant */
    }
    core_info[core_idx].core_number = cores_table_core_idx;
  
    if (core_config->init_mode == HSU_CORE_CONFIG_INIT_MODE_HOST)
    {
      core_info[core_idx].init_opt = UW_INIT_HOST;
      ++core_idx;
      continue; /* Nothing else to assign for host core */
    }

    core_info[core_idx].device_param = &device_params_for_stack[device_controller_idx];
    if (core_config->init_mode == HSU_CORE_CONFIG_INIT_MODE_OTG)
    {
        core_info[core_idx].init_opt = UW_INIT_OTG;
        core_info[core_idx].device_param->auto_enable = FALSE;
#ifndef T_REXNT
        #if defined(FEATURE_HS_USB_OTG) && !defined(FEATURE_HS_USB_BASIC)
#error code not present
#endif /* defined(FEATURE_HS_USB_OTG) && !defined(FEATURE_HS_USB_BASIC) */
#endif /* !T_REXNT */
    }
    else
    {
      if (core_config->init_mode == HSU_CORE_CONFIG_INIT_MODE_DEVICE)
      {
        core_info[core_idx].init_opt = UW_INIT_DEVICE;
        core_info[core_idx].device_param->auto_enable = TRUE;
      }
      else
      {
        HSU_MSG_ERROR("_setup_stack(): called with undefined init option", 0, 0, 0);
        return JEINVAL;
      }
    }

    hsu_conf_sel_core_info = (struct hsu_device_params_type*)core_config->hsu_conf_sel_core_info;

    /* We continue only if the core is OTG or DEVICE so it should include the device_params info */
    HSU_ASSERT(hsu_conf_sel_core_info != NULL);

    if (hsu_conf_sel_core_info->core_type == HSU_CONF_SEL_FULL_SPEED_CORE)
    {
        core_info[core_idx].device_param->full_speed_only = TRUE;
    }
    else
    {
        core_info[core_idx].device_param->full_speed_only = FALSE; 
    }

    /* Link the structure to all the structures it points to */
    device_params_for_stack[device_controller_idx].dev_info = &device_info[device_controller_idx];
    /* We want config_param to point to the first configuration of this device_controller_idx. */
    device_params_for_stack[device_controller_idx].config_param = &config_params[device_controller_idx][0];
    /* Go over all the configurations in this core */
    for (config_idx = 0; config_idx < hsu_conf_sel_core_info->num_of_configs; ++config_idx)
    {
      device_params_for_stack[device_controller_idx].config_param[config_idx].fd_instances = 
        &fds_to_init[device_controller_idx][config_idx][0];
    }

    /* Fill the structure */
    build_device_params_struct(
      cores_table_core_idx,
      &device_params_for_stack[device_controller_idx]
#ifdef T_UNDP
      , use_amss_product_id
#endif
    ); 

    ++device_controller_idx;
    ++core_idx;
  }
#ifndef T_REXNT

#ifndef FEATURE_HS_USB_BASIC
  /* Keep the new serial number and product ID of the main core on the ARM9
     to be used later on in Software Download*/
  hsu_api_update_product_id(device_info[HSU_DEVICE_CONTROLLER_IDX_0].product);
  if (device_info[HSU_DEVICE_CONTROLLER_IDX_0].product_serial != NULL)
  {
    hsu_api_update_serial_number(device_info[HSU_DEVICE_CONTROLLER_IDX_0].product_serial);
    hsu_api_update_is_serial_number_null(FALSE);
  }
  else
  {
    hsu_api_update_is_serial_number_null(TRUE);
  }
#endif /* FEATURE_HS_USB_BASIC */

  /* no more calls to hsu_config_selector_get_memory_start_address() are 
  allowed, since the memory is now given to the stack, in jstart_stack() */
  memory_requests_are_allowed = FALSE; 
#endif /* !T_REXNT */

  stack_init_args.mem_buf = memory_for_stack_ptr;
  stack_init_args.mem_buf_size = size_of_memory_for_stack;
  stack_init_args.num_of_cores = core_idx;
  stack_init_args.hw_core_params = &core_info[0];

#ifndef T_REXNT
  /* 
   * The Giant mutex is locked in hsu_os_sync_init() which is called from 
   * os_port_init(), that's called from jstart_stack(). 
   * This is because only then, the giant is created. 
   * The rest of the code under jstart_stack runs with the giant locked, and 
   * we should release it when the function returns. 
   */
  jresult = jstart_stack(&stack_init_args);
  
  if(jresult != HSU_JSUCCESS)
  {
#ifndef FEATURE_HS_USB_BASIC /* DLOAD/ARMPROG */
    ERR("*_setup_stack(): jstart_stack failed with %d",
      jresult,0,0);
#endif
    HSU_ASSERT((char*)"*_setup_stack(): jstart_stack failed"
      == NULL);
  }

#if !defined(FEATURE_HS_USB_NO_HS_OTG_CORE) && !defined(FEATURE_HS_USB_OTG)
  /* 
  * Workaround for a HW bug: After link reset, the link doesn't set BSVIS 
  * when BSV is set. So this workaround trigger a NOTIFY_DEVICE_CONNECT 
  * event on stack startup when BSV is high. For HS-OTG core only.
  */
  if (hsu_port_is_attached(HS_USB_CORE_INDEX))
  {
    os_notify(NOTIFY_DEVICE_CONNECT, HS_USB_CORE_INDEX);
  }
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE && !FEATURE_HS_USB_OTG */

#ifndef FEATURE_HS_USB_BASIC /* DLOAD/ARMPROG */
  jsafe_leave();
#endif

  hsu_conf_sel_stack_utils_enable_perf_boost();

#ifndef FEATURE_HS_USB_BASIC  
  /* Notify the OS layer that the USB stack is initialized. */
  hsu_os_notify_stack_ready();
#endif  /* !FEATURE_HS_USB_BASIC */
  return HSU_JSUCCESS;
#else /* T_REXNT */
#error code not present
#endif  /* !T_REXNT */

} /* hsu_conf_sel_stack_utils_setup_stack */

#ifndef FEATURE_HSU_TEST
#ifndef FEATURE_HS_USB_BASIC /* DLOAD/ARMPROG */
/*==============================================================================

FUNCTION          HSU_JSLAVE_RECONFIG_CB

DESCRIPTION
  The function is a call back function which is passed as the 'cb' parameter for
  jslave_reconfig function. It will call the 'cb' passed through 
  hsu_jslave_reconfig.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None.

==============================================================================*/
void hsu_jslave_reconfig_cb(void *arg, jresult_t status)
{
  /* reset the reconfig state flag */
  hsu_jslave_reconfig_started = FALSE;

  /* call the callback function which has been passed in args of 
   *  hsu_jslave_reconfig. 
   */
  if(jslave_reconfig_param.cb)
  {
    jslave_reconfig_param.cb(arg,status);
  }
}

/*==============================================================================

FUNCTION          hsu_jslave_reconfig

DESCRIPTION
  It is a wrapper function for jslave_reconfig which runs in AL TASK context.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None.

==============================================================================*/
static void hsu_jslave_reconfig(void * args)
{
  hsu_jslave_reconfig_param_type *par = (hsu_jslave_reconfig_param_type *) args;

  /* set the reconfig state flag */
  hsu_jslave_reconfig_started = TRUE;

  /* replace 'cb' with internal 'cb' hsu_jslave_reconfig_cb.*/
  jslave_reconfig(par->core,
                  par->new_config,
                  hsu_jslave_reconfig_cb, 
                  par->arg);
}

/*==============================================================================

FUNCTION          HSU_CONF_SEL_IS_MS_INCLUDED_IN_CORE

DESCRIPTION
This function checks if MS is included in the current composition of 
the core configurations

DEPENDENCIES

RETURN VALUE
boolean

SIDE EFFECTS
None.

==============================================================================*/
static boolean hsu_conf_sel_is_ms_included_in_core(uint8 core_idx)
{
#ifdef FEATURE_HS_USB_MS_FD
  hsu_core_config_type* core_config = NULL;
  struct hsu_device_params_type* conf_sel_core_info = NULL;
  uint8 config_idx;

  core_config = hsu_core_config_get_core(core_idx);
  HSU_ASSERT((core_config != NULL) && (core_config->hsu_conf_sel_core_info != NULL));

  conf_sel_core_info = (struct hsu_device_params_type*)core_config->hsu_conf_sel_core_info;

  for (config_idx = 0; config_idx < conf_sel_core_info->num_of_configs; ++config_idx)
  {  
    if (conf_sel_core_info->configs_arr[config_idx].num_of_fd_instances[HSU_CFG_SEL_MS_FD] != 0)
    {
      HSU_MSG_LOW("hsu_conf_sel_is_ms_included_in_core, return TRUE", 0, 0, 0);
      return TRUE;
    }
  }
  HSU_MSG_LOW("hsu_conf_sel_is_ms_included_in_core, return FALSE", 0, 0, 0);
  return FALSE;
#else /*!FEATURE_HS_USB_MS_FD */
  HSU_MSG_LOW("hsu_conf_sel_is_ms_included_in_core, return FALSE", 0, 0, 0);
  return FALSE;
#endif /*FEATURE_HS_USB_MS_FD */
}


#ifndef T_UNDP /* UNDP does not support UI/Pictbridge */
/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_SWITCH_DEVICE_STACK

DESCRIPTION
  This function re-configures the stack to include a new set of FDs, as 
  described by the num_of_fd_instances_in_comp parameter.
  This only affects the composition used for the first (and if multi-core is not
  enabled, the only) core.
  It registers reconfig_cb_func_ptr as the callback function to be invoked when 
  the stack re-configuration process is completed. 

  It should be called after all application using the old stack were stopped.

DEPENDENCIES
  Assumes that no application is using the stack at the time of the call. 
  Assumes all AL related initialization were already done. 

RETURN VALUE
  jresult_t - HSU_JSUCCESS for success, other values for failure (currently all 
  cause an ASSERT).

SIDE EFFECTS
  None.

==============================================================================*/
jresult_t hsu_conf_sel_stack_utils_switch_device_stack
(
 uint8                    core_idx,
  reconfig_complete_cb_t   reconfig_cb_func_ptr,
  void *        arg_for_reconfig_cb
)
{
  static device_params_t device_params_for_stack;
  static dev_info_t      device_info;
  static fd_instance_t   fds_to_init[HSU_MAX_NUM_OF_CONFIGS][HSU_CFG_SEL_NUM_OF_FD_TYPES];
  static config_param_t  config_params[HSU_MAX_NUM_OF_CONFIGS];
  hsu_core_config_type* core_config = NULL;
  uint32 config_idx;
  struct hsu_device_params_type* conf_sel_core_info = NULL;

  memset(&config_params, 0, sizeof(config_params));
  memset(&device_params_for_stack, 0, sizeof(device_params_for_stack));


  /* no more calls to hsu_config_selector_get_memory_start_address() are 
  allowed, since the memory is now given to the stack, in jstart_stack() */
  memory_requests_are_allowed = FALSE; 

  /* link the structure to all the structure it points to */
  device_params_for_stack.dev_info = &device_info;
  device_params_for_stack.config_param = &config_params[0];
  /* Go over all the configurations in this core */
  core_config =  hsu_core_config_get_core(core_idx);

  HSU_ASSERT((core_config != NULL) && (core_config->hsu_conf_sel_core_info != NULL));

  conf_sel_core_info = (struct hsu_device_params_type*)core_config->hsu_conf_sel_core_info;
  for (config_idx = 0; config_idx < conf_sel_core_info->num_of_configs; ++config_idx)
  {
    device_params_for_stack.config_param[config_idx].fd_instances = &fds_to_init[config_idx][0];
  }

  build_device_params_struct(
    core_idx,
    &device_params_for_stack
  );

  /* schedule jlave_reconfig on AL task. */
  jslave_reconfig_param.core = core_idx;
  jslave_reconfig_param.new_config = &device_params_for_stack;
  jslave_reconfig_param.cb = reconfig_cb_func_ptr;
  jslave_reconfig_param.arg = arg_for_reconfig_cb;
      
  hsu_al_task_enqueue_cmd( hsu_jslave_reconfig, &jslave_reconfig_param);

  hsu_conf_sel_stack_utils_enable_perf_boost();

  return HSU_JSUCCESS;
} /* hsu_conf_sel_stack_utils_switch_device_stack */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_RECONFIG_DONE

DESCRIPTION
This function is called when the stack re-configuration process is completed. 
This function updates the ARM9 product_id and serial_number to be used in SW 
download.

DEPENDENCIES

RETURN VALUE
None.

SIDE EFFECTS
None.

==============================================================================*/
void hsu_conf_sel_stack_utils_reconfig_done(hsu_conf_sel_comp_info_type* comp_info)
{

  boolean is_ms_included = FALSE;

  /* Keep the new serial number and product ID of the main core on the ARM9
  to be used later on in Software Download*/
  hsu_api_update_product_id(comp_info->product_id);

  /* @TODO - for now dynamic composition switch is applicable in primary core only.
     For supporting other cores, we also need to pass the core_idx to this function. */
  is_ms_included = hsu_conf_sel_is_ms_included_in_core(HSU_CORE_IDX_PRIMARY);

  /* When MS is not included in the composition - use NULL serial number */
  if (is_ms_included == FALSE)
  {
    hsu_api_update_is_serial_number_null(TRUE);
  }
  else
  {
    hsu_api_update_serial_number(hsu_conf_sel_stack_utils_serial_number);
    hsu_api_update_is_serial_number_null(FALSE);
  }
}

#endif /* !T_UNDP */

/*===========================================================================

FUNCTION hsu_conf_sel_stack_utils_force_disable_start

DESCRIPTION
This function disables the usb core, puts the USB PHY in LPM.

INPUT
cb - the callback for completion (of type 'hsu_conf_sel_stack_utils_force_disable_cb_type')

DEPENDENCIES
Run in the AL TASK command queue.

RETURN VALUE
TRUE/FALSE returned by the callback function ('cb' - input).

SIDE EFFECTS
===========================================================================*/
void hsu_conf_sel_stack_utils_force_disable_start(void* cb)
{
  hsu_conf_sel_stack_utils_force_disable_cb_type completion_cb = (hsu_conf_sel_stack_utils_force_disable_cb_type)cb;
  if (hsu_stack_is_forced_disable == TRUE)
  {
    HSU_MSG_HIGH("*_force_disable_start: already in forced_disable mode",0,0,0);
    completion_cb(FALSE);
	  return;
  }
#ifdef FEATURE_HS_USB_ANALOG_AUDIO
  if (hsu_analog_audio_get_curr_mode() != HSU_AUDIO_MODE_USB)
  {
    HSU_MSG_HIGH("*_force_disable_start: can't force_disable while Analog Audio is on",0,0,0);
    completion_cb(FALSE);
	  return;
  }
#endif /* FEATURE_HS_USB_ANALOG_AUDIO */
  hsu_stack_is_forced_disable = TRUE;

  if (hsu_conf_sel_stack_utils_disable() == FALSE)
  {
    completion_cb(FALSE);
    return;
  }
  hsu_os_bus_enter_lpm(HS_USB_CORE_INDEX, FALSE, FALSE);
  hsu_os_bus_interrupt_uninit(HS_USB_CORE_INDEX);
  completion_cb(TRUE);
  return;
}

/*==============================================================================

FUNCTION          hsu_conf_sel_stack_utils_force_disable_stop

DESCRIPTION
This function starts the USB core, activates the core and reconnects the PHY.

INPUT
cb - the callback for completion (of type 'hsu_conf_sel_stack_utils_force_disable_cb_type')

DEPENDENCIES
Run in the AL TASK command queue.

RETURN VALUE
TRUE/FALSE returned by the callback function ('cb' - input).

SIDE EFFECTS
None.

==============================================================================*/
void hsu_conf_sel_stack_utils_force_disable_stop(void* cb)
{
  hsu_conf_sel_stack_utils_force_disable_cb_type completion_cb = (hsu_conf_sel_stack_utils_force_disable_cb_type)cb;
  if (hsu_stack_is_forced_disable == FALSE)
  {
    HSU_MSG_HIGH("*_force_disable_stop: stack not in force_disable",0,0,0);
    completion_cb(FALSE);
    return;
  }
  hsu_stack_is_forced_disable = FALSE;
  hsu_conf_sel_stack_utils_enable();
  hsu_os_bus_interrupt_init(HS_USB_CORE_INDEX);
  completion_cb(TRUE);
  return;
}

/*==============================================================================

FUNCTION          hsu_conf_sel_stack_utils_is_forced_disable

DESCRIPTION
returns whether the USB core is in forced_disable mode..

DEPENDENCIES
None.

RETURN VALUE
FALSE - disabled.
TRUE - enabled.

SIDE EFFECTS
None.

==============================================================================*/

boolean hsu_conf_sel_stack_utils_is_forced_disable(void)
{
  return hsu_stack_is_forced_disable;
}

/*==============================================================================

FUNCTION          hsu_conf_sel_stack_utils_disable

DESCRIPTION
   This function disable the USB core, called from "hsu_conf_sel_stack_force_disable_start" 
   and "hsu_analog_audio_start".
    
DEPENDENCIES
  None.

RETURN VALUE
  FALSE - if failed.
  TRUE - on successes

SIDE EFFECTS
  None.

==============================================================================*/
boolean      hsu_conf_sel_stack_utils_disable(void)
{
  HSU_MSG_HIGH("hsu_conf_sel_stack_utils_disable - entered", 0, 0, 0);
  /* Stop the LPM timer. We will enter LPM later on */
  os_bus_start_stop_lpm_timer(FALSE);

  /* clear the OTG interrupt enable bits to avoid interrupts. */ 
#ifdef FEATURE_HS_USB_HAL
  HAL_hsusb_ClearBits32(0, HAL_HSUSB_OTGSC_ADDR, 
    HAL_HSUSB_OTGSC_DPIE_BMSK
    | HAL_HSUSB_OTGSC_1MSE_BMSK
    | HAL_HSUSB_OTGSC_BSEIE_BMSK
    | HAL_HSUSB_OTGSC_BSVIE_BMSK
    | HAL_HSUSB_OTGSC_ASVIE_BMSK
    | HAL_HSUSB_OTGSC_AVVIE_BMSK
    | HAL_HSUSB_OTGSC_IDIE_BMSK
    );
#else /* FEATURE_HS_USB_HAL */
  HWIO_USB_OTG_HS_OTGSC_OUTM(0xff000000, 0);
#endif /* FEATURE_HS_USB_HAL */

  /* If we are currently in LPM we need to exit it so we can work with the link.*/
  hsu_os_bus_exit_lpm(HS_USB_CORE_INDEX);

#ifdef FEATURE_BATTERY_CHARGER
  /* In case the charging configuration took place we need to cancel it */
  hsu_conf_sel_stack_utils_handle_chg_disconnected();
#endif /*FEATURE_BATTERY_CHARGER*/

#ifdef FEATURE_HS_USB_OTG
#error code not present
#endif /* FEATURE_HS_USB_OTG */
  hsu_os_core_hw_uninit(HS_USB_CORE_INDEX);

  hsu_api_disable_pmic_otg_comparators();

  /*Stop the USB link. This will also put D+/D- signals into high-impedance mode*/
  if (!hsu_phy_utils_spoof_disconnect())
  {
    HSU_MSG_ERROR("*_start: Couldn't stop USB link "
            "hsu_phy_utils_spoof_disconnect() returned false",0,0,0);
    return FALSE;
  }
  HSU_MSG_HIGH("hsu_conf_sel_stack_utils_disable - exit", 0, 0, 0);
  return TRUE;
}


/*==============================================================================

FUNCTION          hsu_conf_sel_stack_utils_enable

DESCRIPTION
   This function enable the USB core, called from "hsu_conf_sel_stack_force_disable_stop" 
   and "hsu_analog_audio_stop".
    
DEPENDENCIES
  None.

RETURN VALUE
  FALSE - if failed.
  TRUE - on successes

SIDE EFFECTS
  None.

==============================================================================*/
boolean hsu_conf_sel_stack_utils_enable(void)
{
  hsu_os_bus_exit_lpm(HS_USB_CORE_INDEX);
  /* Reset the PHY */
  hsu_api_init_phy();
  hsu_api_enable_pmic_otg_comparators();
  /* Init the USB link*/
  if(hsu_os_core_hw_init(HS_USB_CORE_INDEX) != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR("*_stop: hsu_os_core_hw_init failed",0,0,0);
    HSU_ASSERT((char*)"hsu_os_core_hw_init failed" == NULL);
    return FALSE;
  }

  os_bus_start_stop_lpm_timer(TRUE);
  return TRUE;
}


#ifdef FEATURE_HS_USB_CHARGING
/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_WALL_CHG_DETECT_CB

DESCRIPTION
   Called as a wall charger detection callback, with indication whether or not a 
   USB wall charger is connected.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
static void hsu_conf_sel_stack_utils_wall_chg_detect_cb
(
  boolean is_wall_chg
)
{
  hsu_conf_sel_stack_utils_is_wall_charger = is_wall_chg;

  if (is_wall_chg)
  {
    /* Since beforehand it was assumed this was a host PC, disconnect and 
          immediately connect as wall charger. */
    #ifdef FEATURE_HS_USB_CHG_REMOTE_API
    /* TODO: If charger is not activated, save state */
    chg_usb_charger_disconnected();
    chg_usb_charger_connected(PM_APP_OTG_A_DEV_TYPE__USB_CHARGER);
    chg_usb_i_is_available(HSU_CHG_API_HELPER_WALL_MAX_POWER);
    #else
    hsu_api_notif_charger_disconnected();
    hsu_api_notif_charger_connected(TRUE);
    hsu_api_notif_charger_is_avail(HSU_CHG_API_HELPER_WALL_MAX_POWER);
    #endif /* FEATURE_HS_USB_CHG_REMOTE_API */    
  }
}
#endif  /* FEATURE_HS_USB_CHARGING */

#endif /* #ifndef(FEATURE_HS_USB_BASIC) */

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_SET_SERIAL_NUMBER

DESCRIPTION
This function sets the serial number according to the composition and to the
configuration (AMSS / DLOAD / ARMPROG)

DEPENDENCIES

RETURN VALUE
None

SIDE EFFECTS
None. 

==============================================================================*/
void hsu_conf_sel_stack_utils_set_serial_number(  
  dev_info_t *  device_info,
  uint16        product_id,
  uint8         core_idx
)
{
#if defined(FEATURE_HS_USB_BASIC) 
  hsu_dload_info_type* dload_info_data_ptr = 
    hsu_boot_dload_get_hsu_dload_info_ptr();
#else
#ifdef FEATURE_HS_USB_MS_FD
  static boolean read_esn_imei = TRUE;
  char return_buff_ptr[HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE];
  int serial_number_size ;
  int i;
#endif /* FEATURE_HS_USB_MS_FD */
  boolean is_ms_included = FALSE;
#endif /* FEATURE_HS_USB_BASIC */
	HSU_USE_PARAM(product_id); /* to avoid unused symbol warning */

  /* 
  * String describing the device’s serial number. 
  * There are several issues to consider when choosing what value to use here:
  * - If a different string is used for each device (ESN, for example), then 
  *   the host driver would have to be installed for each device separately. 
  * - If the same non NULL serial # string is used for all devices, then 
  *   Windows crashes, if one device is connected to a PC port, and the other 
  *   through a USB HUB. 
  * - If NULL is used, the driver has to be installed for each port a device
  *   is connected to, but changing devices on the same port doesn't require 
  *   re-installation. The host communicates only with the device's first LUN, 
  *   and thus displays only a single drive instead of two.
  *
  *   In any case, MS FD spec requires the serial number to be at least 12 
  *   digits long
  */
  /*
  *   The solution to the above is:
  *
  *   If the composition includes MS - the serial number is the ESN / IMEI.
  *   In case that the ESN / IMEI is not available we use a constant "1234567890ABCDEF".
  *
  *   If the composition doesn't include MS - the serial number is null
  */

#ifndef FEATURE_HS_USB_BASIC /* DLOAD/ARMPROG */
#ifdef FEATURE_HS_USB_MS_FD
  /* Read the ESN / IMEI in any case so that if we change the composition dynamically
  to a composition that includes MS we will have it */
  if (read_esn_imei == TRUE)
  {
    memset (hsu_conf_sel_stack_utils_serial_number, 0, HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
    if (FALSE == hsu_conf_sel_nv_esn_imei_read(return_buff_ptr, HSU_CFG_SELECTOR_MAX_ESN_IMEI_SIZE))
    {
      /* ESN/IMEI could not be retrieved, the serial number should be set to the default value
      (HSU_CFG_SELECTOR_DEFAULT_SERIAL_NUMBER)*/
      (void)std_strlcpy(hsu_conf_sel_stack_utils_serial_number, 
        HSU_CFG_SELECTOR_DEFAULT_SERIAL_NUMBER, 
        HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
    }
    else
    {
      /* Save the ESN/IMEI as the serial number */
      (void)std_strlcpy(hsu_conf_sel_stack_utils_serial_number, 
        return_buff_ptr, 
        HSU_CFG_SELECTOR_SERIAL_NUMBER_LEN);
    }

    /* Pad the serial number with zero's in case it is less than 12 digits */
    serial_number_size = std_strlen(hsu_conf_sel_stack_utils_serial_number);
    if (serial_number_size < HSU_CFG_SELECTOR_SERIAL_NUMBER_MIN_LEN)
    {
      for (i = serial_number_size ; i < HSU_CFG_SELECTOR_SERIAL_NUMBER_MIN_LEN ; ++i)
      {
        hsu_conf_sel_stack_utils_serial_number[i] = '0';
      }
      hsu_conf_sel_stack_utils_serial_number[HSU_CFG_SELECTOR_SERIAL_NUMBER_MIN_LEN] = '\0';
    }
    read_esn_imei = FALSE;
  }
#endif /* FEATURE_HS_USB_MS_FD */

  is_ms_included = hsu_conf_sel_is_ms_included_in_core(core_idx);

  /* When MS is not included in the composition - use NULL serial number */
  if (is_ms_included == FALSE )
  {
    device_info->product_serial = NULL;
  }
  else
  {
    device_info->product_serial = hsu_conf_sel_stack_utils_serial_number;
  }
#else /* DLOAD / ARMPROG */
  if (dload_info_data_ptr == NULL)
  {
    HSU_MSG_ERROR("dload_info_data_ptr is NULL", 0, 0, 0);
    device_info->product_serial = NULL;
    return;
  }

  /* check if the IRAM is loaded with an old version. 
  If so change the IRAM data to fit the new version.
  It's important since the product_id in the new version was shifted by uint32 from it's old location.*/
  hsu_boot_dload_check_and_update_version_num();

  /* Check if the IRAM data is valid by checking the product_id. 
  If not, set the serial number to NULL */
  if (!hsu_boot_dload_is_dload_info_valid(dload_info_data_ptr))
  {
    device_info->product_serial = NULL;
  }
  else
  {
    /* Check if the target is loaded with an old version.
    In this case we want to set the serial number to the default value (same as the old implementation)
    otherwise drivers installation will be required and the Software Download will fail */
    if (dload_info_data_ptr->hsu_version_num == HSU_BOOT_DLOAD_INVALID_VERSION_NUM)
    {
      device_info->product_serial = HSU_CFG_SELECTOR_DEFAULT_SERIAL_NUMBER;
    }
    else
    {
      /* In version 3 we added a boolean flag to the IRAM to define if to set
         the serial number to NULL or set its value according to the IRAM value.
         In older versions we decided according to the composition, but it didn't work
         for Linux and Windows mobile, since we didn't have their composition structure */
      if (dload_info_data_ptr->hsu_version_num >= 3)
      {
        if (dload_info_data_ptr->is_null_serial_number == TRUE)
        {
          device_info->product_serial = NULL;
        }
        else
        {
          /* Read the serial number from the IRAM */
          device_info->product_serial = dload_info_data_ptr->serial_number;
        }
      }
      else /* version 2 and below */
      {
        /* When MS is not included in the composition - use NULL serial number */
        /* In DLOAD mode we can determine what the composition is according to the 
        product ID */
        if ((product_id != HSU_PRODUCT_ID_DIAG_NMEA_MS_MDM_NET) &&
          (product_id != HSU_PROD_ID_SINGLE_IF_DIAG_NMEA_MS_MDM_NET)&&
          (product_id != HSU_PROD_ID_SINGLE_IF_DIAG_MS))
        {
          device_info->product_serial = NULL;
        }
        else
        {
          /* Read the serial number from the IRAM */
          device_info->product_serial = dload_info_data_ptr->serial_number;
        }  
      }
    }
  }

#endif /* ! FEATURE_HS_USB_BASIC */  
}

/*==============================================================================

FUNCTION          DEV_NOTIFY_CONFIGURED_CB

DESCRIPTION
App's implementation of the HS-USB stack's notification in case it enters/leaves
the DEVICE CONFIGURED state.

DEPENDENCIES
TODO

RETURN VALUE
None

SIDE EFFECTS
None.

==============================================================================*/
static void dev_notify_configured_cb
(
  appctx_t ctx, 
  jbool_t configured
)
{
  #ifndef FEATURE_HS_USB_BASIC
  #ifdef FEATURE_HS_USB_CHARGING
  uint32 amount_of_current_to_draw = 0;
  #endif /* FEATURE_HS_USB_CHARGING */
  #endif /* ! FEATURE_HS_USB_BASIC */

  HSU_USE_PARAM(ctx);

  if (!configured_cb_was_called_before)
  {
    configured_cb_was_called_before = TRUE;
  }
  else /* configured_cb_was_called_before */
  {
    /* Note: It is allowed to have the device configured == TRUE twice in a row,
    for example USBCV invokes SET_CONFIGURATION(1) twice in a row.
    Getting FALSE twice in a row is also ok. */
    if (dev_is_configured == configured)
    {
      return;
    }
  }

  dev_is_configured = configured;

  #ifndef FEATURE_HS_USB_BASIC
  #ifdef FEATURE_HS_USB_CHARGING

  /* If we are already suspended, we must not notify charger that it can draw current! */
  if (hsu_chg_api_helper_is_resumed())
  {
    if (configured == TRUE)
    {
      amount_of_current_to_draw = HSU_CHG_API_HELPER_MAX_POWER;
    }
    else /* !configured */
    {
      amount_of_current_to_draw = HSU_CHG_API_HELPER_POWER_UNCONFIGURED;
    }

#ifdef FEATURE_HS_USB_CHG_REMOTE_API
    chg_usb_i_is_available(amount_of_current_to_draw);
#else
    hsu_api_notif_charger_is_avail(amount_of_current_to_draw);
#endif /* FEATURE_HS_USB_CHG_REMOTE_API */
  }

  #endif /* FEATURE_HS_USB_CHARGING */
  #endif /* !FEATURE_HS_USB_BASIC */
}

/*==============================================================================

FUNCTION          DEV_APP_INIT

DESCRIPTION
Called by the HS-USB stack in order to utilize app's callbacks, in case
interesting events (such as moving from/to DEVICE CONFIGURED state) occur.

DEPENDENCIES
TODO

RETURN VALUE
HSU_JSUCCESS - Always.

SIDE EFFECTS
None. 
==============================================================================*/
jresult_t dev_app_init(appctx_t ctx, app_callbacks_t *app_cbs)
{
  /* TODO: When more callbacks are needed, create a static variable to be passed.*/
  HSU_USE_PARAM(ctx);

  app_cbs->notify_device_configured = dev_notify_configured_cb;

  return HSU_JSUCCESS;
}


#ifdef FEATURE_HS_USB_CHARGING
#ifndef FEATURE_HS_USB_BASIC
/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_HANDLE_CHG_CONNECTED

DESCRIPTION
  Handle charger connected to HS-USB port. 
  This function gets called by the OTG stack connect/disconnect mechanism 
  implemented in the corresponding .c file, and by non-OTG connect/disconnect 
  mechanism (os_notify) implemented invoked in hsu_os_abstract_bus.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
void hsu_conf_sel_stack_utils_handle_chg_connected(void)
{
    /* First, assume host PC (since there is a timer for wall charger detection).
    If we don't call the _connected function, then there is a chance that we
    would be receiving a suspend callback prior to timer expiry.

    After the timer expires, if it appears this is not a wall charger, then there 
    is no change. Otherwise, disconnect and reconnect as wall charger. */
#ifdef FEATURE_HS_USB_CHG_REMOTE_API
    chg_usb_charger_connected(PM_APP_OTG_A_DEV_TYPE__USB_HOST);
#else
    hsu_api_notif_charger_connected(FALSE);
#endif /* FEATURE_HS_USB_CHG_REMOTE_API */
    hsu_chg_wallchg_detect_wall_charger(hsu_conf_sel_stack_utils_wall_chg_detect_cb);
}

/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_HANDLE_CHG_DISCONNECTED

DESCRIPTION
  Handle charger disconnected from HS-USB port. 
  This function gets called by the OTG stack connect/disconnect mechanism 
  implemented in the corresponding ,c file, and by non-OTG connect/disconnect 
  mechanism invoked in hsu_os_abstract_bus.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

==============================================================================*/
void hsu_conf_sel_stack_utils_handle_chg_disconnected()
{
    /* USB Cable is disconnected, clear the Wall Charger flag */
    hsu_conf_sel_stack_utils_is_wall_charger = FALSE;

    #ifdef FEATURE_HS_USB_CHG_REMOTE_API
    chg_usb_i_is_not_available();
    chg_usb_charger_disconnected();
    #else
    hsu_api_notif_charger_not_avail();
    hsu_api_notif_charger_disconnected();
    #endif /* FEATURE_HS_USB_CHG_REMOTE_API */
}

#endif /* FEATURE_HS_USB_BASIC */
#endif /* FEATURE_HS_USB_CHARGING */

#ifndef FEATURE_HS_USB_BASIC /* DLOAD/ARMPROG */
#ifdef FEATURE_HS_USB_CORE_RESET_ON_DISC 
/*==============================================================================

FUNCTION          HSU_CONF_SEL_STACK_UTILS_DEVICE_RESTART_CORE

DESCRIPTION       
 This function is the SW workaround for a HW bug regarding the
 stuck PHY issue. 
 On each disconnection this function is called and restarts the 
 core and the PHY.

DEPENDENCIES
 None.

RETURN VALUE
 None.

SIDE EFFECTS
 None.

==============================================================================*/
void hsu_conf_sel_stack_utils_device_restart_core
(
  void* core_index_ptr
)
{
#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE

  jresult_t  jresult;
  uint32     core_index = *(uint32*)core_index_ptr;
  hsu_core_config_type* core_config = NULL;

  HSU_MSG_HIGH("hsu_conf_sel_stack_utils_device_restart_core - entered", 0, 0, 0);

  /* The device_restart_core stops jslave_reconfig in progress. If 
   * jlsave_reconfig was in progress, it should be rescheduled to complete
   * its task.
   */
  if( hsu_jslave_reconfig_started )
  {
    core_clear_reconfig_flag(jslave_reconfig_param.core);
    hsu_al_task_enqueue_cmd( hsu_jslave_reconfig, &jslave_reconfig_param);
  }

  jsafe_enter();

  /* Stop the LPM timer in order to avoid LPM while reseting */
  os_bus_start_stop_lpm_timer(FALSE);

  /* clear the OTG interrupt enable bits to avoid interrupts during reset*/ 
  HAL_hsusb_ClearBits32(0, HAL_HSUSB_OTGSC_ADDR, 
    HAL_HSUSB_OTGSC_DPIE_BMSK
    | HAL_HSUSB_OTGSC_1MSE_BMSK
    | HAL_HSUSB_OTGSC_BSEIE_BMSK
    | HAL_HSUSB_OTGSC_BSVIE_BMSK
    | HAL_HSUSB_OTGSC_ASVIE_BMSK
    | HAL_HSUSB_OTGSC_AVVIE_BMSK
    | HAL_HSUSB_OTGSC_IDIE_BMSK
    );

  hsu_os_core_hw_uninit(HS_USB_CORE_INDEX);
  hsu_api_init_phy();

  jresult = hsu_os_core_hw_init(HS_USB_CORE_INDEX);
  if(jresult != HSU_JSUCCESS)
  {
    HSU_ASSERT((char*)"hsu_os_core_hw_init failed" == NULL);
  }

  /* In non-OTG mode, the DCD controller is not re-enabled when cable is   */
  /* connected. So this is needed in order to re-enable the DCD controller */
  core_config = hsu_core_config_get_core(core_index);
  HSU_ASSERT(core_config != NULL);

  if (core_config->init_mode == HSU_CORE_CONFIG_INIT_MODE_DEVICE)
  {
    jresult  = jslave_enable(core_index);
  }

  if (jresult != HSU_JSUCCESS)
  {
    HSU_ASSERT((char*)"jslave_enable failed" == NULL);
  }

  hsu_conf_sel_stack_utils_enable_perf_boost();

  /* Start the LPM timer - after core reset */
  os_bus_start_stop_lpm_timer(TRUE);

  jsafe_leave();

  HSU_MSG_HIGH("hsu_conf_sel_stack_utils_device_restart_core - exit", 0, 0, 0);

#endif /* FEATURE_HS_USB_NO_HS_OTG_CORE */
}
#endif /* FEATURE_HS_USB_CORE_RESET_ON_DISC*/
#endif /* !FEATURE_HS_USB_BASIC */

/*==============================================================================

FUNCTION          hsu_get_undp_hsu_comp_id

DESCRIPTION
This function gets HSU composition ID info from UNDP.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
Call to UNDP_get_hsu_VID_PID() also updates VID, PID, product_name, and 
product_manufacturer information as well in the static hsu_dynamic_vid_pid
structure.

==============================================================================*/
#ifdef T_UNDP
boolean hsu_get_undp_hsu_comp_id(nv_hs_usb_supported_compositions_type 
                                 * nv_composition_id)
{
  HSU_ASSERT(NULL != nv_composition_id);

  if ( 0 == UNDP_get_hsu_VID_PID(&hsu_dynamic_vid_pid, TRUE) )
  {
    /* UNDP_get_hsu_VID_PID() returns one of the valid values of 
    nv_hs_usb_supported_compositions_type for UNDP (U2) */
    switch(hsu_dynamic_vid_pid.composition)
    {
    case NV_HS_USB_DIAG_NMEA_MDM:
    case NV_HS_USB_DIAG_NMEA_MDM_NET:
    case NV_HS_USB_DIAG_MDM:
    case NV_HS_USB_DIAG_MDM_NET:
      *nv_composition_id = (nv_hs_usb_supported_compositions_type)
        hsu_dynamic_vid_pid.composition;
      return TRUE;

    default: /* No other values are legal for UNDP */
      return FALSE;
    }
  }

  return FALSE;
}
#endif /* T_UNDP */

/*==============================================================================

FUNCTION          hsu_is_dev_configured

DESCRIPTION
This function returns TRUE if the device is configured and FALSE otherwise

DEPENDENCIES
None.

RETURN VALUE
jbool_t - dev_is_configured

SIDE EFFECTS
None.
==============================================================================*/
jbool_t hsu_is_dev_configured(void)
{
  return dev_is_configured;
}

#endif /* ! FEATURE_HSU_TEST */
