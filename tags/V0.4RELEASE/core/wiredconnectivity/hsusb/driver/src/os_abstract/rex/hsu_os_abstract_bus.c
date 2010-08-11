/*==============================================================================

                 H S U _ O S _ A B S T R A C T _ B U S . C


GENERAL DESCRIPTION
  High-Speed USB Implementation of the REX OS abstraction layer 
  bus API for the REX OS.

EXTERNALIZED FUNCTIONS
  Externalized to outside of the OS abstraction layer:
    os_interrupt_setup
    os_interrupt_teardown
    os_bus_start_stop_lpm_timer
    os_notify
    os_hw_init
    os_hw_uninit
    os_bus_alloc_resource
    os_bus_release_resource
    os_bus_space_read_1
    os_bus_space_read_2
    os_bus_space_read_4
    os_bus_space_write_1
    os_bus_space_write_2
    os_bus_space_write_4

    hsu_os_bus_poll_interrupt
    hsu_os_bus_handle_interrupt
    hsu_os_notify_stack_ready

    hsu_os_bus_save_mem_dump
    hsu_os_bus_get_num_cores

    hsu_os_bus_lpm_enable
    hsu_os_bus_lpm_disable

    hsu_port_is_attached

    os_power_request
    os_get_port_info

    hsu_os_bus_interrupt_init
    hsu_os_bus_interrupt_uninit

  Externalized internally in the OS abstraction layer:
    hsu_os_bus_init
    hsu_os_bus_uninit

INITIALIZATION AND SEQUENCING REQUIREMENTS
  hsu_os_init should be called before using the API defined in this file.

Copyright (c) 2007, 2008, 2009 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/


/*===========================================================================

                            EDIT HISTORY FOR MODULE


  $Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/os_abstract/rex/hsu_os_abstract_bus.c#116 $
  $DateTime: 2009/03/29 08:32:35 $ $Author: dsegal $

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/08/06   ds      Created.

===========================================================================*/


/*==============================================================================

                               LOCAL LINT FLAGS

==============================================================================*/

/*lint -emacro(717, ASSERT_CONTEXT)   This macro is defined by 3rd party. */

/*===========================================================================

                            INCLUDE FILES FOR MODULE

===========================================================================*/

/* AMSS include files. */

#include <comdef.h>
#include <customer.h>

#ifndef FEATURE_HS_USB_BASIC
#include <rex.h>  /* For rex_sleep */

#ifdef FEATURE_L4
#error code not present
#endif
#endif /* FEATURE_HS_USB_BASIC */

#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
#include <sleep.h>
#include <event.h>
#include <hsu_phy_utils.h>  /* For hsu_phy_utils_spoof_connect\disconnect. */
#endif  /* FEATURE_HS_USB_BASIC */
#endif  /* !T_REXNT */

#ifndef FEATURE_HSU_TEST
#include <msm.h>

#ifndef FEATURE_HS_USB_BASIC
#include <tramp.h>
#include <clkrgm_msm.h>
#endif  /* FEATURE_HS_USB_BASIC */

#include <gpio_int.h> 
#include GPIO_H
#include <pm.h>

#endif  /* FEATURE_HSU_TEST */

/* High Speed USB subsystem include files. */
#include <jerrno.h>
#include <jtypes.h>
#include <hsu_al_task.h>
#include <hsu_common.h>
#include <controllers.h>

#include <port_sync.h>
#include <port_bus.h>

/* FIXME: remove this */
#include <port.h>

#include <hsu_core_config.h>

#ifndef FEATURE_HSU_TEST

#include <hsu_conf_sel_stack_utils.h>
#include <hsu_event_defs.h>
#endif /* ! FEATURE_HSU_TEST */

#include <hsu_config_selector.h>

#ifndef FEATURE_HS_USB_BASIC
  #ifdef FEATURE_HS_USB_CHARGING
    #ifndef FEATURE_HSU_TEST
      #include <hsu_chg_api_helper.h>
    #endif /* ! FEATURE_HSU_TEST */
    #ifdef FEATURE_HS_USB_CHG_REMOTE_API
      #include "charger.h"
    #endif /* FEATURE_HS_USB_CHG_REMOTE_API */
  #endif /* FEATURE_HS_USB_CHARGING */
#endif /* FEATURE_HS_USB_BASIC */

#include "hsu_bus_mode_notifier.h"

/* Local include files. */
#include "hsu_os_abstract_bus.h"

#ifndef FEATURE_HS_USB_BASIC
#include "hsu_apis.h"
#endif /* !FEATURE_HS_USB_BASIC */

#if defined (FEATURE_HS_USB_OTG) && !defined(FEATURE_HSU_TEST)
#error code not present
#endif /*FEATURE_HS_USB_OTG*/

#define HSU_MAX_PHCD_DWELL_TIME_ATTEMPT  100

#if !defined(T_REXNT) && !defined(FEATURE_HS_USB_BASIC)
#ifndef FEATURE_NEW_SLEEP_API
#error  FEATURE_NEW_SLEEP_API must be defined for HS-USB to vote for sleep correctly
#endif  /*FEATURE_NEW_SLEEP_API*/
#endif  /*!T_REXNT && !FEATURE_HS_USB_BASIC*/

#ifdef FEATURE_HS_USB_HAL
#include "HAL_hsusb.h"
#endif /* FEATURE_HS_USB_HAL */

#ifdef FEATURE_HS_USB_UICC
#include "hsu_usb_uicc.h"
#endif /* FEATURE_HS_USB_UICC */

#include "hsu_os_abstract_bus_ext.h"

#ifdef FEATURE_HS_USB_POLLING
#include "hsu_polling.h"
#endif /* FEATURE_HS_USB_POLLING */

/*==============================================================================

                LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

#if defined(T_MSM6246) ||  defined(T_MSM6290)
/* TODO: remove the hack when Koel tramp code gets updated for HS USB */
#ifndef TRAMP_USB_HS_ISR
#define TRAMP_USB_HS_ISR TRAMP_USB_OTG_ISR
#endif 
#endif /* T_MSM6246 || T_MSM6290 */

#ifdef T_QSC1100
/* TODO-ULC: remove the hack when ULC tramp code gets updated for HS USB */
#ifndef TRAMP_USB_HS_ISR
#define TRAMP_USB_HS_ISR TRAMP_USB_ISR
#endif 
#endif /* T_QSC1100 */

/* The following defines the memory areas that will be saved when ASSERTed*/
#if !(defined FEATURE_HS_USB_BASIC) && !(defined T_REXNT) 
#ifndef FEATURE_HS_USB_MEM_OPTIMIZATION
#define OTG_REG_MEM_SIZE  512 /*We want to read from BASE_ADDRESS 
                                to BASE_ADDRESS+1ffh*/
#define ULPI_REG_MEM_SIZE 64  /*We want to read 0 - 3Fh*/
#endif /* ! FEATURE_HS_USB_MEM_OPTIMIZATION */
#endif

/* prevent remote wakeup to be issued too soon after USB suspend. spec requires
5ms but testing shows that 30ms are needed or else bus get stuck in resume state */
#define OS_BUS_POSTPONE_REMOTE_WAKEUP_MS 30

#ifdef FEATURE_HS_USB_POLLING
#define HSU_MSM_SET( base, field ) \
    HWIO_OUTM( base, HWIO_FMSK(base,field), HWIO_FMSK(base,field))
#endif /* FEATURE_HS_USB_POLLING */

/*==============================================================================
Forward declarations
==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/

#ifdef T_REXNT
#error code not present
#endif

/* 
 * Resource IDs for OTG, Device and Host controllers. 
 * See the porting guide document, os_bus_alloc_resource() API (section 21.3)
 * "id" field description.
 */
enum 
{
  OS_BUS_OTG_RESOURCE_ID    = 0,
  OS_BUS_DEVICE_RESOURCE_ID = 1,
  OS_BUS_HOST_RESOURCE_ID   = 2,
  OS_BUS_NUM_OF_RESOURCE_IDS
};

enum  
{
  OS_BUS_MAX_CONTROLLERS_PER_CORE = 3,
    /* Possible controllers: OTG, Device, Host */

  OS_BUS_MAX_ISRS_PER_INTERRUPT   = 2,
    /* Interrupt handler chaining support. */

  OS_BUS_NUM_RES_PER_CONTROLLER   = 2
};

/* 
 * Defined the core type
 */
typedef enum
{
  OS_BUS_USB_HIGH_SPEED_CORE,
  OS_BUS_USB_FULL_SPEED_CORE,
  OS_BUS_USB_UICC_CORE
} hsusb_core_type;


/*lint -esym(754, length)  
** The length field of a memory resource (memory_resource_info_type) 
** is accessed by HSU_ASSERT macros, but for some reason Lint thinks
** that it is not referenced.
*/

/* I/O or Memory resource info type. */
typedef struct 
{
  uint32           core_index;
  juint8_t* const  start_address;
  const juint32_t  length;  
} memory_resource_info_type;



typedef union
{
  memory_resource_info_type     mem_resource_instance;
  interrupt_resource_info_type  int_resource_instance;
} resource_info_type;

/* High-Speed USB controller resource info type. */
typedef struct 
{
  jint_t                 resource_type;
  jint_t                 resource_id;
  resource_info_type     resource_instance;
} bus_resource_type;

/* High-Speed USB controller info type. */
typedef struct 
{
  jint_t                controller_type;

  /* The resource handle returned by add_generic_controller.
  ** A NULL value implies that the controller is 
  ** not registered with the stack.
  */
  void*                 controller_handle;

  bus_resource_type  resource_list[OS_BUS_NUM_RES_PER_CONTROLLER];

} usb_controller_info_type;

#ifndef T_REXNT
typedef struct  
{
  boolean
    lpm_is_active;

  boolean 
    is_in_low_power_mode;

  /* This flag reflects the most recent state of the HS-USB port
  ** before entering LPM (low-power mode). It is set to True if the HS-USB 
  ** port is attached when we start entering LPM. Otherwise, it is set to FALSE.
  ** When the HS-USB core is not in LPM, this flag has no meaning.
  */
  boolean
    cable_is_connected_during_lpm;

  /* This flag is TRUE when the LPM entry timer is armed or when
  ** it has been expired, but its callback (the LPM entry function)
  ** has not been executed yet.
  */
  boolean
    lpm_entry_is_pending;

  /* This timer is armed when the core enters into suspend state.
  ** When the timer expires, the core switches to low power mode.
  ** The timer is canceled if the core exits the suspend state
  ** before it expires.
  **
  ** The goal of this timer is to avoid unnecessary entry into low power
  ** mode if the core enters into suspend state only for a short time.
  */
  rex_timer_type
    lpm_entry_timer;

  /* This timer is used for USBCV remote-wakeup tests.
  ** It's initialized with the explicit_remote_wakeup_delay
  ** that is read from the NV.
  **
  ** This timer is armed when the core enters into suspend state.
  ** When the timer expires, the core issues a remote wakeup.
  ** The timer is canceled if the core exits the suspend state
  ** before it expires.
  **
  ** When the explicit remote wakeup delay NV value is 0, this 
  ** timer is used for ensuring that at least OS_BUS_POSTPONE_REMOTE_WAKEUP_MS
  ** milliseconds pass between each suspend and remote wakeup events.
  **
  ** A regular REX timer is not appropriate for this timer, because
  ** REX timers are not supported during power collapse and TCXO
  ** shutdown modes.
  */
  timer_type
    remote_wakeup_timer;

  /* The timer group to which the explicit remote wakeup timer belongs.
  ** The remote wakeup timer must belong to a non-NULL timer group
  ** in order to make it supported during power collapse and TCXO 
  ** shutdown modes.
  */
  timer_group_type
    remote_wakeup_timer_group;

  /* This timer is armed when the core exits from low-power mode
  ** due to an asynchronous interrupt. When this happens, a USB cable
  ** connection status check is scheduled, in order to decide whether
  ** to return to low-power mode or not.
  */
  rex_timer_type
    cable_status_check_timer;

  /* Enter LPM callback */
  void (*enter_lpm_handler)(uint32 core_index, 
                            boolean called_by_lpm_timer,
                            boolean analog_audio_mode);

  /* Exit LPM callback*/
  void (*exit_lpm_handler)(uint32 core_index);

  /* Port is attached callback */
  boolean (*port_is_attached)(uint32 core_index);

} usb_lpm_info_type;
#endif  /* T_REXNT */

/* USB core information type. */
typedef struct 
{
  char*              description;
  hsusb_core_type    core_type;
  uint32             number_of_controllers;

  usb_controller_info_type  
    controllers_list[OS_BUS_MAX_CONTROLLERS_PER_CORE];

  /* Core's USB interrupt number */
  uint32  interrupt_number;
  
  /* Core's main interrupt handler. */
  void (*interrupt_handler)(void);

  #ifndef T_REXNT
  /* Core's Low power mode information */
  usb_lpm_info_type
    usb_lpm_info;
  #endif /* T_REXNT */

  /* This timer is armed when the core exits from low-power mode
  ** due to an asynchronous interrupt. When this happens, if we are in host mode
  ** resume all of the connected peripherals sequence is scheduled. 
  ** This operation is delayed (via timer) in order to give the hs-usb0
  ** task enough time to update the cable connection status. 
  ** See documentation of hsu_os_bus_resume_peripherals() for more details.
  */
  #if !defined(T_REXNT) && !defined(FEATURE_HS_USB_BASIC)
  rex_timer_type
      resume_peripherals_timer;
  #endif /*!T_REXNT &&!FEATURE_HS_USB_BASIC*/

  #if (defined FEATURE_HS_USB_PMIC_PHY) || (defined FEATURE_HS_USB_LEGACY_DEVICE)
  /* When the PMIC PHY is used, this field holds the core's cable
  ** connection status (instead the BSV bit in the OTGSC register).
  ** This field is updated using notifications sent by the PMIC module.
  */
  boolean cable_is_connected;
  #endif  /* FEATURE_HS_USB_PMIC_PHY || FEATURE_HS_USB_LEGACY_DEVICE */

  uint32  num_of_registered_isrs;

  /* An array of the core's registered interrupt service routines
  ** chained by the core's main interrupt handler.
  */
  interrupt_resource_info_type*
    isr_list[OS_BUS_MAX_ISRS_PER_INTERRUPT];

  /* Core's attached controller */
  usb_controller_info_type* controller_info_ptr;

} usb_core_info_type;


typedef struct 
{
  uint32 
    spoofed_reconnects_on_reset_during_suspend_count;

} hsu_os_bus_statistics_type;

/*lint -e{754}
** This structure serves also for debugging purposes.
** Possibly not all of the fields are referenced 
** directly by the code.
*/
typedef struct 
{
  usb_core_info_type* const
    core_table_ptr;

  /* Map between core index used in hsu_os_abstract_bus, to HAL or 
     core_config core index */
  uint8
    *hal_core_index_map_ptr;

  /* Map between core index used in HAL or core_config, to hsu_os_abstract_bus
     core index */
  uint8
    *hal_core_index_inv_map_ptr;

  /* Statistic info collected for debugging purpose. */
  hsu_os_bus_statistics_type*
    hsu_os_bus_statistics_ptr;

  #if !(defined FEATURE_HS_USB_BASIC) && !(defined T_REXNT)

  sleep_okts_handle
    hsu_sleep_vote_handle;

  /* This flag enables\disables the low-power mode feature. */
  boolean
    lpm_feature_is_active;

  /* The usbh_pclk control feature is not enabled for the U2 target, because
  ** this feature works fine only if the ULPI_STP_CTRL bit in the USBCMD 
  ** register is set upon LPM entry and cleared in LPM exit. However, doing this
  ** causes problems in the USB resume sequence during TCXO shutdown on the U2 
  ** target.
  */
  /* This flag enables\disables the usbh_pclk clock control feature. */
  boolean
    usbh_pclk_control_active;

  /* This flag enables\disables the ULPI GPIO config\unconfig feature. */
  boolean
    gpio_control_active;

  /* This flag enables\disables the CPU clock control feature. */
  boolean
    dcvs_active;

  /* This critical section is used for maintaining mutual-exclusion 
  ** between the low-power mode entry and exit functions.
  */
  rex_crit_sect_type
    lpm_crit_sect;

  #ifdef FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND
  /* Indicates whether or not the PMIC MPP15 - GPIO 111 rework for supporting 
  USB reset waking device from TCXO shutdown exists. */
  boolean usb_reset_rework_is_installed;
  #endif /* FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */

  #ifndef FEATURE_HS_USB_MEM_OPTIMIZATION
  /* The following memory is used to saved the relevant USB registers
  ** in case of an ASSERT. It used for main core only
  */
  uint32   *otg_controller_mem_dump_ptr;
  uint8    *ulpi_reg_mem_dump_ptr;
  #endif  /* ! FEATURE_HS_USB_MEM_OPTIMIZATION */

  #endif /* !(defined FEATURE_HS_USB_BASIC) && !(defined T_REXNT) */

} os_bus_file_static_vars_type;

/*==============================================================================
  Constants and Macros
==============================================================================*/
#if (defined (T_QSC6270) || defined(T_MSM8650)) &&  \
    defined(FEATURE_DONT_USE_PMIC_OTG_COMPARATORS)
#error FEATURE_DONT_USE_PMIC_OTG_COMPARATORS is not supported in this target.
#endif  /* T_QSC6270 && FEATURE_DONT_USE_PMIC_OTG_COMPARATORS */

#ifndef FEATURE_HSU_TEST

#if !defined (FEATURE_HS_USB_NO_HS_OTG_CORE) || defined (FEATURE_HS_USB_BASIC)
/* High-Speed USB controller memory mapped section - base address and length */
#ifdef FEATURE_HS_USB_HAL
#define OS_BUS_EHCI_START_ADDRESS 0x100
#else /* FEATURE_HS_USB_HAL */
#define OS_BUS_EHCI_BASE_ADDRESS  USBH_BASE
#endif /* FEATURE_HS_USB_HAL */
#define OS_BUS_EHCI_SECTION_SIZE  0x1FC
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE || FEATURE_HS_USB_BASIC */

#if (defined (FEATURE_HS_USB_LEGACY_HOST) || defined (FEATURE_HS_USB_LEGACY_DEVICE))
/* Legacy FS-USB controller memory mapped section - base address and length */
#ifdef FEATURE_HS_USB_HAL
#define OS_BUS_UHCI_START_ADDRESS 0x1000
#else /* FEATURE_HS_USB_HAL */
#define OS_BUS_UHCI_BASE_ADDRESS  (USB_OTG_BASE + 0x1000)
#endif /* FEATURE_HS_USB_HAL */
#define OS_BUS_UHCI_SECTION_SIZE  0x4000
#endif /* defined FEATURE_HS_USB_LEGACY_HOST || defined FEATURE_HS_USB_LEGACY_DEVICE */

#ifdef FEATURE_HS_USB_UICC
/* UICC USB controller memory mapped section - base address and length */
#ifdef FEATURE_HS_USB_HAL
#define OS_BUS_USB_UICC_START_ADDRESS 0x100
#else /* FEATURE_HS_USB_HAL */
#define OS_BUS_USB_UICC_BASE_ADDRESS  (USB_HS_MUSIM_REG_BASE + 0x100)
#endif /* FEATURE_HS_USB_HAL */
#define OS_BUS_USB_UICC_SECTION_SIZE  0x1FC
#endif /* FEATURE_HS_USB_UICC */

#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
enum
{
  HSU_OS_BUS_LPM_ENTRY_TIMEOUT = 1000,
  HSU_OS_BUS_RESUME_PERIPHERALS_TIMEOUT = 600
};

#endif /* !FEATURE_HS_USB_BASIC */
#endif /* !T_REXNT */

#endif  /* FEATURE_HSU_TEST */

/* Maximum number of trials for setting the PHCD bit. */
const uint32  
  MAX_PHCD_BIT_WRITE_TRIALS = 500;

/* Maximum number of read trial of the PHCD bit, waiting for it to become 
** zero.
*/
const uint32  
  MAX_PHCD_BIT_READ_TRIALS = 500;

/* Map between core index used in hsu_os_abstract_bus, to HAL or 
core_config core index */
#define OS_BUS_TO_HAL_CORE_ID(os_bus_core_id) \
  (hsu_os_abstract_bus.hal_core_index_map_ptr[(os_bus_core_id)])

/* Map between core index used in HAL or core_config, to hsu_os_abstract_bus
core index */
#define OS_BUS_FROM_HAL_CORE_ID(hal_core_id) \
  (hsu_os_abstract_bus.hal_core_index_inv_map_ptr[(hal_core_id)])

/*==============================================================================
  Externalized Variables
==============================================================================*/


/*==============================================================================
  File Static Variables
==============================================================================*/

#ifdef T_REXNT
#error code not present
#endif

/* The following memory is used to save the relevant USB registers
** in case of an ASSERT
*/
#if !(defined FEATURE_HS_USB_BASIC) && !(defined T_REXNT)
#ifndef FEATURE_HS_USB_MEM_OPTIMIZATION
static uint32   otg_controller_mem_dump[OTG_REG_MEM_SIZE];
static uint8    ulpi_reg_mem_dump[ULPI_REG_MEM_SIZE];
#endif /* ! FEATURE_HS_USB_MEM_OPTIMIZATION */
#endif 

#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC

#ifdef FEATURE_HS_USB_LEGACY_DEVICE
static void hsu_os_bus_fs_legacy_enter_lpm(uint32 core_index, 
                                           boolean called_by_lpm_timer,
                                           boolean is_analog_audio_mode);

static void hsu_os_bus_fs_legacy_exit_lpm(uint32 core_index);
#endif /* FEATURE_HS_USB_LEGACY_DEVICE */

#ifdef FEATURE_HS_USB_UICC
static void hsu_os_bus_uicc_enter_lpm(uint32 core_index, 
                                      boolean called_by_lpm_timer,
                                      boolean is_analog_audio_mode);

static void hsu_os_bus_uicc_exit_lpm(uint32 core_index);
#endif /* FEATURE_HS_USB_UICC */

#endif /* T_REXNT */
#endif /* FEATURE_HS_USB_BASIC */

/* Forward declarations of interrupt wrapper functions
** referenced  by the USB core table. */
#if (!defined(FEATURE_HS_USB_NO_HS_OTG_CORE) || defined(FEATURE_HS_USB_BASIC) || defined(T_REXNT)) 
static void hsu_os_hs_isr_wrapper(void);
#endif 

#if ((defined(FEATURE_HS_USB_UICC) && !defined(FEATURE_HS_USB_BASIC)) || defined(T_REXNT)) 
static void hsu_os_uicc_isr_wrapper(void);
#endif

#if (((defined(FEATURE_HS_USB_LEGACY_DEVICE) || defined(FEATURE_HS_USB_LEGACY_HOST)) && \
  !defined(FEATURE_HS_USB_BASIC)) || defined(T_REXNT)) 
static void hsu_os_fs_isr_wrapper(void);
#endif

#if !defined( FEATURE_HSU_TEST ) && !defined( FEATURE_HS_USB_BASIC )
void hsu_txco_wakeup_gpio_handler(void);
#endif /* FEATURE_HSU_TEST && FEATURE_HS_USB_BASIC */

static void hsu_os_bus_init_core_index_maps(void);

/*lint -e{785} 
** The resource lists are initialized with fewer elements
** than the full capacity of the array.
*/

/* USB core table.
** Contains the details of the USB
** cores installed in the system.
*/
static usb_core_info_type 
hsu_core_table[] = 
{
  #if (!defined(FEATURE_HS_USB_LEGACY_HOST) && !defined(FEATURE_HS_USB_NO_HS_OTG_CORE)) || \
       defined(FEATURE_HS_USB_BASIC)  

  /* High Speed USB core info. */
  {
    "High Speed USB Core",
    OS_BUS_USB_HIGH_SPEED_CORE,

    /* Number of controllers. */
    3,

    {
      /* High-Speed USB core's OTG controller info. */
      {
        /* Controller type. Defined in 'controllers.h' */
        controller_type_tdi_4x_otg,

        /* Controller handle. */
        NULL,

        /* OTG controller's resource list. */
        {
          /* OTG interrupt resource info. */
          {
            SYS_RES_IRQ,
            OS_BUS_OTG_RESOURCE_ID,
            /* OTG controller interrupt resource.    */
            {
              {
                HS_USB_CORE_INDEX,
                NULL,
                /*, NULL, NULL, NULL*/
              }
            }
          },

          /* OTG memory resource info. */
          {
            SYS_RES_MEMORY,
            OS_BUS_OTG_RESOURCE_ID,
            /* High-Speed USB OTG controller memory space. */
            {
              {
                HS_USB_CORE_INDEX,
                #ifdef FEATURE_HS_USB_HAL
                (juint8_t*)0,
                #else /* FEATURE_HS_USB_HAL */
                (juint8_t*)(OS_BUS_EHCI_BASE_ADDRESS),
                #endif /* FEATURE_HS_USB_HAL */
                OS_BUS_EHCI_SECTION_SIZE
              }
            }
          }
        } /* End of resource list. */
      },  /* End of OTG controller info. */

      /* High-Speed USB core's device controller info. */
      {
        /* Controller type. Defined in 'controllers.h' */
        controller_type_tdi_4x,

        /* Controller handle. */
        NULL,

        /* High-Speed USB device controller's resource list. */
        {
          /* Device interrupt resource info. */
          {
            SYS_RES_IRQ,
            OS_BUS_DEVICE_RESOURCE_ID,
            /* Device controller interrupt resource. */
            {
              {
                HS_USB_CORE_INDEX, 
                NULL,
                /*, NULL, NULL, NULL */
              }
            }
          },
          /* Device memory resource info. */
          {
            SYS_RES_MEMORY,
            OS_BUS_DEVICE_RESOURCE_ID,
            /* High-Speed USB device controller memory space. */
            {
              {
                HS_USB_CORE_INDEX,
                #ifdef FEATURE_HS_USB_HAL
                (juint8_t*)0,
                #else /* FEATURE_HS_USB_HAL */
                (juint8_t*)(OS_BUS_EHCI_BASE_ADDRESS),
                #endif /* FEATURE_HS_USB_HAL */
                OS_BUS_EHCI_SECTION_SIZE
              }
            }
          }
        } /* End of resource list. */
      },  /* End of device controller info. */

      /* High-Speed USB core's host controller info. */
      {
        /* Controller type. Defined in 'controllers.h' */
        controller_type_ehci_local,
        /* Controller handle. */
        NULL,

        /* High-Speed USB host controller's resource list. */
        {
          /* Host interrupt resource info. */
          {
            SYS_RES_IRQ,
            OS_BUS_HOST_RESOURCE_ID,
            /* Host controller interrupt resource.   */
            {
              {
                HS_USB_CORE_INDEX, 
                NULL,
               /*, NULL, NULL, NULL*/
              }
            }
          },
          /* Host memory resource info. */
          {
            SYS_RES_MEMORY,
            OS_BUS_HOST_RESOURCE_ID,
            /* High-Speed USB host controller memory space. */
            {
              {
                HS_USB_CORE_INDEX,
                #ifdef FEATURE_HS_USB_HAL
                (juint8_t*)OS_BUS_EHCI_START_ADDRESS,
                #else /* !FEATURE_HS_USB_HAL */
                (juint8_t*)(OS_BUS_EHCI_BASE_ADDRESS+0x100),
                #endif /* FEATURE_HS_USB_HAL */
                OS_BUS_EHCI_SECTION_SIZE
              }
            }
          }
        }  /* End of resource list. */
      }  /* End of host controller info. */
    },  /* End of core controllers list. */

    /* High-Speed USB core interrupt number. */
    #if defined(T_REXNT) || defined(FEATURE_HS_USB_BASIC)
    0,
    #else   /* !T_REXNT && !FEATURE_HS_USB_BASIC */
    (uint32)TRAMP_USB_HS_ISR,
    #endif  /* !T_REXNT && !FEATURE_HS_USB_BASIC */

    /* High-Speed USB core interrupt wrapper function. */
    hsu_os_hs_isr_wrapper

    /* TODO - May this structure be completely removed for HS-USB basic? */
    #ifndef T_REXNT
    /* Core's LPM information */
    ,{    
      TRUE,   /* LPM feature is active */    
      FALSE,  /* Not in low-power mode.                             */
      FALSE,  /* Cable connected during LPM (a don't care init).    */
      FALSE,  /* LPM entry timer is not armed.                      */
      {0},    /* An initializer for low-power mode entry timer.     */
      {0},    /* An initializer for the remote wakeup timer.        */
      {0},    /* An initializer for the remote wakeup timer group.  */
      {0},    /* An initializer for cable status check timer.       */

      #ifndef FEATURE_HS_USB_BASIC

      hsu_os_bus_enter_lpm, /* Enter LPM handler function             */  
      hsu_os_bus_exit_lpm,  /* Exit LPM handler function              */
      hsu_port_is_attached  /* Port connection status check function. */

      #else

      NULL,
      NULL,
      NULL

      #endif /* !FEATURE_HS_USB_BASIC */
    }  /* End of core LPM information. */
    #endif /* !T_REXNT*/

    #ifdef FEATURE_HS_USB_PMIC_PHY
    ,FALSE  /* USB cable is not connected. */
    #endif  /* FEATURE_HS_USB_PMIC_PHY */

  },  /* End of High Speed USB core 1 info. */

  #endif /* !FEATURE_HS_USB_LEGACY_HOST && !FEATURE_HS_USB_NO_HS_OTG_CORE || 
             FEATURE_HS_USB_BASIC */

  #if (((defined(FEATURE_HS_USB_LEGACY_DEVICE) ||             \
         defined(FEATURE_HS_USB_LEGACY_HOST))  &&             \
         !defined(FEATURE_HS_USB_BASIC)) || defined(T_REXNT))

  /* Full Speed USB core info. */
  {
    "Full Speed USB Core",
    OS_BUS_USB_FULL_SPEED_CORE,
    /* Number of controllers. */
    1,

    {
      /* Full-Speed USB core's device controller info. */
      {
        /* Controller type. Defined in 'controllers.h' */
        #ifdef FEATURE_HS_USB_LEGACY_HOST
        controller_type_td243hc_rev2,
        #else
        controller_type_td243fc_rev2,
        #endif
        /* Controller handle. */
        NULL,

        /* Full-Speed USB device controller's resource list. */
        {
          /* Device interrupt resource info. */
          {
            SYS_RES_IRQ,
            0, /* only a single controller in the full-speed core */
            /* Device/Host controller interrupt resource.   */
            {
              {
                FS_USB_CORE_INDEX, 
                NULL, /*NULL, NULL, NULL*/
              }
            }
          },

          /* Device memory resource info. */
          {
            SYS_RES_MEMORY,
            0,/*OS_BUS_HOST_RESOURCE_ID,*/
            /* Full-Speed device controller memory space. */
            {
              {
              FS_USB_CORE_INDEX,
              #ifdef FEATURE_HS_USB_HAL
              (juint8_t*)OS_BUS_UHCI_START_ADDRESS, 
              #else /* FEATURE_HS_USB_HAL */
              (juint8_t*)(OS_BUS_UHCI_BASE_ADDRESS), 
              #endif /* FEATURE_HS_USB_HAL */
              OS_BUS_UHCI_SECTION_SIZE
              }
            }
          }
        } /* End of resource list. */
      }  /* End of device controller info. */
    },  /* End of core controllers list. */
    
    /* Full-Speed USB core interrupt number. */
    #ifdef T_REXNT
#error code not present
    #else  /* !T_REXNT */
    (uint32)TRAMP_USB_ISR,
    #endif  /* T_REXNT */

    /* Full-Speed USB core interrupt function. */
    hsu_os_fs_isr_wrapper

    #ifndef T_REXNT    
    /* Core's LPM information */
    ,{    
      TRUE,   /* The LPM feature is active */    
      FALSE,  /* Not in low-power mode.                            */
      FALSE,  /* Cable connected during LPM (a don't care init).   */
      FALSE,  /* LPM entry timer is not armed.                     */
      {0},    /* An initializer for low-power mode entry timer.    */
      {0},    /* An initializer for the remote wakeup timer.       */
      {0},    /* An initializer for the remote wakeup timer group. */
      {0},    /* An initializer for cable status check timer.      */

      #ifndef FEATURE_HS_USB_BASIC

      hsu_os_bus_fs_legacy_enter_lpm, /* Enter LPM handler function             */  
      hsu_os_bus_fs_legacy_exit_lpm,  /* Exit LPM handler function              */
      hsu_port_is_attached  /* Port connection status check function. */

      #else

      NULL,
      NULL,
      NULL

      #endif /* !FEATURE_HS_USB_BASIC */
    }
    #endif /*T_REXNT*/

    #if (defined FEATURE_HS_USB_PMIC_PHY) || (defined FEATURE_HS_USB_LEGACY_DEVICE)
    ,FALSE  /* USB cable is not connected. */
    #endif  /* FEATURE_HS_USB_PMIC_PHY || FEATURE_HS_USB_LEGACY_DEVICE */

  },  /* End of Full-Speed USB core 1 info. */
  #endif /* FEATURE_HS_USB_LEGACY_DEVICE ... */

  #if (defined(FEATURE_HS_USB_UICC) && !defined(FEATURE_HS_USB_BASIC)) 
  /* HS USB UICC core info. */
  {
    "HS USB UICC Core",
    OS_BUS_USB_UICC_CORE,

    /* Number of controllers. */
    1,
    {
      /* High-Speed USB core's device controller info. */
      {
        /* Controller type. Defined in 'controllers.h' */
        controller_type_ehci_local,
        /* Controller handle. */
        NULL,
        /* Full-Speed USB controller's resource list. */
        {
          /* Host interrupt resource info. */
          {
            SYS_RES_IRQ,
            0, /* only a single controller in the full-speed core */
            /* USB UICC controller interrupt resource.   */
            {
              {
                UICC_USB_CORE_INDEX,
                NULL /*, NULL, NULL, NULL */
              }
            }
          },

          /* Host memory resource info. */
          {
            SYS_RES_MEMORY,
            0,/*OS_BUS_HOST_RESOURCE_ID,*/
            /* UICC USB host controller memory space. */
            {
              {
                UICC_USB_CORE_INDEX,
                #ifdef FEATURE_HS_USB_HAL
                (juint8_t*)OS_BUS_USB_UICC_START_ADDRESS, 
                #else /* FEATURE_HS_USB_HAL */
                (juint8_t*)(OS_BUS_USB_UICC_BASE_ADDRESS), 
                #endif /* FEATURE_HS_USB_HAL */
                OS_BUS_USB_UICC_SECTION_SIZE
              }
            }
          }
        } /* End of resource list. */
      }  /* End of device controller info. */
    },  /* End of core controllers list. */

    /* USB UICC core interrupt number. */
    #ifdef T_REXNT
#error code not present
    #else  /* !T_REXNT */
    (uint32)TRAMP_MUSIM_IRQ,
    #endif  /* T_REXNT */
    /* USB core interrupt wrapper function. */
    hsu_os_uicc_isr_wrapper

    #ifndef T_REXNT
    /* Core's LPM information */
    ,{    
      TRUE,   /* The LPM is active */    
      FALSE,  /* Not in low-power mode.                           */
      FALSE,  /* Cable connected during LPM (a don't care init).  */
      FALSE,  /* LPM entry timer is not armed.                    */
      {0},    /* An initializer for low-power mode entry timer.   */
      {0},    /* An initializer for the remote wakeup timer.      */
      {0},    /* An initializer for the remote wakeup timer group.*/
      {0},    /* An initializer for cable status check timer.     */
      hsu_os_bus_uicc_enter_lpm, /* Enter LPM handler function.   */  
      hsu_os_bus_uicc_exit_lpm,  /* Exit LPM handler function.    */
      NULL              /* Port connection status check function. */
    }
    #endif /* T_REXNT */

    #ifdef FEATURE_HS_USB_PMIC_PHY
    ,FALSE  /* USB cable is not connected. */
    #endif  /* FEATURE_HS_USB_PMIC_PHY */

  }  /* End of Full-Speed USB core 1 info. */
  #endif /* FEATURE_HS_USB_UICC ... */

};  /* hsu_core_table */

/* Map between core index used in hsu_os_abstract_bus, to HAL or 
   core_config core index */
static uint8 hsu_hal_core_index_map[NUMBER_OF_CORES];

/* Map between core index used in HAL or core_config, to hsu_os_abstract_bus
   core index */
static uint8 hsu_hal_core_index_inv_map[HSU_MAX_NUM_OF_CORES];

static hsu_os_bus_statistics_type
  hsu_os_bus_statistics = 
  {
    0  /* Spoofed reconnections count - Init. */
  };  /* hsu_os_bus_statistics */


static os_bus_file_static_vars_type
  hsu_os_abstract_bus =
  {
    hsu_core_table,
    hsu_hal_core_index_map,
    hsu_hal_core_index_inv_map,
    &hsu_os_bus_statistics,

    #if !(defined FEATURE_HS_USB_BASIC) && !(defined T_REXNT)

    
    0,      /* Initial sleep vote handle value.                  */

    TRUE,   /* Enable PHY LPM feature.                           */
    #ifndef T_UNDP
    TRUE,   /* Enable usbh_pclk control.                         */
    #else
    FALSE,  /* Disable usbh_pclk control on the U2 target,
            ** since it works only when the ULPI_STP_CTRL bit
            ** is used.
            */
    #endif  /* !T_UNDP */

    TRUE,   /* Enable GPIO control.                              */
    TRUE,   /* Enable dynamic clock rate switching.              */
    {0}    /* An initializer for the lpm_crit_sect struct.      */

    #ifdef FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND
    ,FALSE
    #endif /* FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */

    #ifndef FEATURE_HS_USB_MEM_OPTIMIZATION
    ,otg_controller_mem_dump,  /* Memory area for saving the OTG 
                                  Controller registers */

    ulpi_reg_mem_dump          /* Memory area for saving the ULPI registers */
    #endif  /* !FEATURE_HS_USB_MEM_OPTIMIZATION */
    #endif  /* !FEATURE_HS_USB_BASIC && !T_REXNT */

  }; /* hsu_os_abstract_bus */

static os_bus_file_static_vars_type* const
  file_static_vars_ptr = &hsu_os_abstract_bus;

/*==============================================================================
  Constants and Macros
==============================================================================*/

/*==============================================================================

  FUNCTION DEFINITIONS

==============================================================================*/

/*===========================================================================
FUNCTION HSU_OS_BUS_INTERRUPT_INIT

DESCRIPTION
  Initializes a USB core's interrupt. Called when the core's interrupt is
  registered its first interrupt service routine (using the 
  os_interrupt_setup() call).

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes 
  defined in jerrno.h

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_bus_interrupt_init(uint32 core_index)
{
  
  #if !defined(FEATURE_HSU_TEST) && !defined(FEATURE_HS_USB_BASIC)

  usb_core_info_type* const core_info_ptr =
    &(file_static_vars_ptr->core_table_ptr[core_index]);

  void    (*core_interrupt_handler_ptr) (void);
  uint32  core_interrupt_number;

  
  HSU_MSG_HIGH("hsu_os_bus_interrupt_init: Entry.", 0, 0, 0);

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  if (core_info_ptr->num_of_registered_isrs == 0)
  {
    /* If there are no registered ISRs for this core,
    ** do nothing. This might occur when the stack is inactive
    ** and then LPM is activated.
    */
    HSU_MSG_HIGH("hsu_os_bus_interrupt_init: There are no registered ISRs. "
      "Do nothing.", 0, 0, 0);

    return HSU_JSUCCESS;
  }
  core_interrupt_handler_ptr = core_info_ptr->interrupt_handler;
  core_interrupt_number      = core_info_ptr->interrupt_number;

  /* Target platform implementation. */

  HSU_MSG_HIGH("hsu_os_bus_interrupt_init: Setting interrupt on Tramp "
               "interrupt %d.", 
               core_interrupt_number, 0, 0);

  tramp_set_isr((tramp_isr_type)core_interrupt_number,
                core_interrupt_handler_ptr);

  #else   /* FEATURE_HSU_TEST || FEATURE_HS_USB_BASIC */

  HSU_USE_PARAM(core_index);

  #endif  /* !FEATURE_HSU_TEST && !FEATURE_HS_USB_BASIC */

  HSU_MSG_HIGH("hsu_os_bus_interrupt_init: Exit.", 0, 0, 0);

  return HSU_JSUCCESS;

}  /* hsu_os_bus_interrupt_init */


/*===========================================================================
FUNCTION HSU_OS_BUS_INTERRUPT_UNINIT

DESCRIPTION
  Un-initializes High-Speed USB interrupt. Called by the bus module
  un-initialization function (hsu_os_bus_uninit).

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes 
  defined in jerrno.h

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_bus_interrupt_uninit(uint32 core_index)
{
  
  #if !defined(FEATURE_HSU_TEST) && !defined(FEATURE_HS_USB_BASIC)

  usb_core_info_type* const core_info_ptr =
    &(file_static_vars_ptr->core_table_ptr[core_index]);

  uint32  core_interrupt_number;

  HSU_MSG_HIGH("hsu_os_bus_interrupt_uninit: Entry.", 0, 0, 0);

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  core_interrupt_number = core_info_ptr->interrupt_number;

  /* Target platform implementation. */
  tramp_set_isr((tramp_isr_type)core_interrupt_number, NULL);

  #else   /* FEATURE_HSU_TEST || FEATURE_HS_USB_BASIC */
  HSU_USE_PARAM(core_index);
  #endif  /* !FEATURE_HSU_TEST && !FEATURE_HS_USB_BASIC */

  HSU_MSG_HIGH("hsu_interrupt_uninit: Exit.", 0, 0, 0);

  return HSU_JSUCCESS;

}  /* hsu_os_bus_interrupt_uninit */


#if !defined (FEATURE_HS_USB_NO_HS_OTG_CORE) || defined (FEATURE_HS_USB_BASIC)
#ifndef FEATURE_HS_USB_PMIC_PHY
#ifndef T_REXNT
/*===========================================================================
FUNCTION HSU_OS_BUS_READ_WRITE_PHY

DESCRIPTION
  Perform a read or write access to a PHY register using the USB core's 
  ULPI_VIEWPORT register.

DEPENDENCIES
  Assumes that the core operates in ULPI mode.
  hsu_os_bus_init() must have been called before using this function.

RETURN VALUE
  TRUE if the PHY access operation is successful. Otherwise, FALSE is returned.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hsu_os_bus_read_write_phy(boolean  is_write_operation,
                                         uint8    register_address,
                                         uint8*   register_value)
{
  #ifndef FEATURE_HS_USB_HAL
  const uint32  max_phy_access_trials         = 10000;
  const uint32  portsc_ulpi_xcvr_select_value = 0x2;

  uint32   portsc_pts_field_value;
  uint32   trials_count;
  uint32   viewport_reg_value;

  boolean  phy_access_is_successful;
  #endif /* FEATURE_HS_USB_HAL */

  #ifndef FEATURE_HS_USB_BASIC
  usb_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[0].usb_lpm_info);
  #endif  /* FEATURE_HS_USB_BASIC */

  #ifndef FEATURE_HS_USB_BASIC
  if (core_lpm_info->is_in_low_power_mode)
  {
    HSU_MSG_ERROR("hsu_os_bus_read_write_phy: Access to core registers during "
                  "low-power mode is not possible. register: %X",
                  register_address, 0, 0);

    HSU_ASSERT(FALSE);

    return FALSE;
  }
  #endif  /* FEATURE_HS_USB_BASIC */

  #ifdef FEATURE_HS_USB_HAL

  if (is_write_operation)
  {
    return (HAL_hsusb_WriteUlpiPhy(
      OS_BUS_TO_HAL_CORE_ID(HS_USB_CORE_INDEX), 0, 
      register_address, *register_value) == HAL_HSUSB_Success);
  }
  else
  {
    return (HAL_hsusb_ReadUlpiPhy(
      OS_BUS_TO_HAL_CORE_ID(HS_USB_CORE_INDEX), 0, 
      register_address, register_value) == HAL_HSUSB_Success);
  }

  #else /* FEATURE_HS_USB_HAL */

  portsc_pts_field_value = HWIO_INF(USB_OTG_HS_PORTSC, PTS);
  if (portsc_pts_field_value != portsc_ulpi_xcvr_select_value)
  {
    HSU_MSG_ERROR("hsu_os_bus_read_write_phy: Port is not configured "
                  "in ULPI mode. PTS field value: %X",
                  portsc_pts_field_value, 0, 0);

    return FALSE;
  }

  viewport_reg_value = 
    HWIO_FMSK(USB_OTG_HS_ULPI_VIEWPORT, ULPIRUN) |
    (register_address << HWIO_SHFT(USB_OTG_HS_ULPI_VIEWPORT, ULPIADDR));

  if (is_write_operation)
  {
    viewport_reg_value |= 
      HWIO_FMSK(USB_OTG_HS_ULPI_VIEWPORT, ULPIRW) |
      (*register_value << HWIO_SHFT(USB_OTG_HS_ULPI_VIEWPORT, ULPIDATWR));
  }

  HWIO_OUT(USB_OTG_HS_ULPI_VIEWPORT, viewport_reg_value);

  /* Wait until the ULPI run bit gets cleared */

  trials_count = 0;
  phy_access_is_successful = TRUE;
  while (HWIO_INF(USB_OTG_HS_ULPI_VIEWPORT, ULPIRUN) == 1)
  {
    trials_count++;

    if (trials_count == max_phy_access_trials)
    {
      HSU_MSG_ERROR("hsu_os_bus_read_write_phy: Failed to access PHY. "
                    "register: %u",
                    register_address, 0, 0);
  
      phy_access_is_successful = FALSE;

      break;
    }
  }

  if (phy_access_is_successful == TRUE)
  {
    if (!is_write_operation)
    {
      *register_value = (uint8)(HWIO_INF(USB_OTG_HS_ULPI_VIEWPORT, ULPIDATRD));
    }
  }

  return phy_access_is_successful;

  #endif /* FEATURE_HS_USB_HAL */

}  /* hsu_os_bus_read_write_phy */
#endif  /* !T_REXNT */
#endif  /* !FEATURE_HS_USB_PMIC_PHY */


#ifndef FEATURE_HS_USB_PMIC_PHY
#ifndef T_REXNT
/*===========================================================================
FUNCTION HSU_OS_BUS_READ_FROM_PHY

DESCRIPTION
  Read from a PHY register using the USB core's ULPI_VIEWPORT register.

DEPENDENCIES
  hsu_os_bus_init() must have been called before using this function.

RETURN VALUE
  TRUE if the PHY read operation is successful. Otherwise, FALSE is returned.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_os_bus_read_from_phy(uint8 phy_register, uint8* read_value)
{
  boolean phy_access_is_successful;


  if (read_value == NULL)
  {
    HSU_MSG_ERROR("hsu_os_bus_read_from_phy: Illegal NULL parameter.", 
                  0, 0, 0);

    HSU_ASSERT(FALSE);

    return FALSE;
  }
  
  HSU_MSG_HIGH("hsu_os_bus_read_from_phy: Reading from register %X",
               phy_register, 0, 0);

  phy_access_is_successful = 
    hsu_os_bus_read_write_phy(FALSE, phy_register, read_value);

  if (!phy_access_is_successful)
  {
    HSU_MSG_ERROR("hsu_os_bus_read_from_phy: PHY read operation failed.",
                  0, 0, 0);
  }

  return phy_access_is_successful;

}  /* hsu_os_bus_read_from_phy */
#endif  /* !T_REXNT */
#endif  /* !FEATURE_HS_USB_PMIC_PHY */


#ifndef FEATURE_HS_USB_PMIC_PHY
#ifndef T_REXNT
/*===========================================================================
FUNCTION HSU_OS_BUS_WRITE_TO_PHY

DESCRIPTION
  Write to a PHY register using the USB core's ULPI_VIEWPORT register.

DEPENDENCIES
  hsu_os_bus_init() must have been called before using this function.

RETURN VALUE
  TRUE if the PHY write operation is successful. Otherwise, FALSE is returned.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_os_bus_write_to_phy(uint8 phy_register, uint8 written_value)
{
  boolean phy_access_is_successful;

  HSU_MSG_HIGH("hsu_os_bus_write_to_phy: Writing %X to register %X",
               written_value, phy_register, 0);

  phy_access_is_successful = 
    hsu_os_bus_read_write_phy(TRUE, phy_register, &written_value);

  if (!phy_access_is_successful)
  {
    HSU_MSG_ERROR("hsu_os_bus_write_to_phy: PHY write operation failed.",
                  0, 0, 0);
  }

  return phy_access_is_successful;

}  /* hsu_os_bus_write_to_phy */
#endif  /* !T_REXNT */
#endif  /* !FEATURE_HS_USB_PMIC_PHY */


#if defined(FEATURE_HS_USB_INTEGRATED_PHY)
#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================
FUNCTION HSU_OS_BUS_PLL_SHUTDOWN

DESCRIPTION
  Configure the HS-USB integrated PHY to shutdown its PLL during low-power
  mode. 
  This is a SW workaround to a PHY HW bug which causes it to leave the PLL 
  on during low-power mode.

  Resolves CRMDB CR #137455.

DEPENDENCIES
  Applies to targets with an integrated HS-USB PHY only.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_pll_shutdown(void)
{
  const uint8 interface_ctrl_reg_clear_address = 0x09;
  const uint8 clock_suspendm_bit_position      = 3;

  /* Set ClockSuspendM = 0 in the PHY's interface control register. */
  /* This is a PHY HW bug workaround for CR #137455. */
  if (!hsu_os_bus_write_to_phy(interface_ctrl_reg_clear_address,
                               0x01 << clock_suspendm_bit_position))
  {
    HSU_MSG_ERROR("hsu_os_bus_pll_shutdown: Failed to write to the PHY's "
                  "Interface Control register",
                  0, 0, 0);

    HSU_ASSERT(FALSE);
  }

}  /* hsu_os_bus_pll_shutdown */
#endif  /* FEATURE_HS_USB_BASIC */
#endif  /* !T_REXNT */
#endif  /* FEATURE_HS_USB_INTEGRATED_PHY */


#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
#ifndef T_QSC1100
/*===========================================================================
FUNCTION CORE_READY_FOR_LPM

DESCRIPTION
This function verifies that LPM entry prerequisite exists. 
The PHY may be entered into low-power mode only:
- when the core is suspended OR
- when USB cable is not connected OR
- when the core is stopped

If none of these conditions met, PHY may not be entered into LPM

DEPENDENCIES
  None.

RETURN VALUE
  True when PHY may be entered into LPM

SIDE EFFECTS
  None
===========================================================================*/
static boolean core_ready_for_lpm()
{
  #ifdef FEATURE_HS_USB_HAL

  uint32 portsc_data, usbcmd_data;

  portsc_data = HAL_hsusb_Read32(
    OS_BUS_TO_HAL_CORE_ID(HS_USB_CORE_INDEX), 
    HAL_HSUSB_PORTSC_ADDR(0));
  
  usbcmd_data = HAL_hsusb_GetBits32(
    OS_BUS_TO_HAL_CORE_ID(HS_USB_CORE_INDEX),
    HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_RS_BMSK);

  return (((portsc_data & HAL_HSUSB_PORTSC_SUSP_BMSK) != 0) || /* Suspended    */
          ((portsc_data & HAL_HSUSB_PORTSC_CCS_BMSK)  == 0) || /* Disconnected */
          (usbcmd_data == 0) );                                /* Stopped      */
   
  #else /* FEATURE_HS_USB_HAL */

  return (( (HWIO_INF(USB_OTG_HS_PORTSC, SUSP) == 1) ||  /* Suspended    */
            (HWIO_INF(USB_OTG_HS_PORTSC, CCS)  == 0) ||  /* Disconnected */
            (HWIO_INF(USB_OTG_HS_USBCMD, RS)   == 0) )); /* Stopped      */

  #endif /* FEATURE_HS_USB_HAL */

}  /* core_ready_for_lpm */
#endif  /* T_QSC1100 */
#endif  /* !FEATURE_HS_USB_BASIC */
#endif  /* !T_REXNT */
#endif  /* !FEATURE_HS_USB_NO_HS_OTG_CORE || FEATURE_HS_USB_BASIC */


#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
#ifdef FEATURE_HS_USB_UICC
/*===========================================================================
FUNCTION CORE_READY_FOR_UICC_LPM

DESCRIPTION
  This function verifies that LPM entry prerequisite exists. 
  We may be entered into low-power mode only:
  - when the core is suspended OR
  - when USB cable is not connected OR
  - when the core is stopped

DEPENDENCIES
  None.

RETURN VALUE
  True when we may be entered into LPM

SIDE EFFECTS
  None
===========================================================================*/
static boolean core_ready_for_uicc_lpm()
{
  #ifdef FEATURE_HS_USB_HAL

  uint32 portsc_data;

  portsc_data = HAL_hsusb_Read32(
    OS_BUS_TO_HAL_CORE_ID(UICC_USB_CORE_INDEX),
    HAL_HSUSB_PORTSC_ADDR(0));

  return ( ((portsc_data & HAL_HSUSB_PORTSC_SUSP_BMSK) != 0) || /* suspended */
           ((portsc_data & HAL_HSUSB_PORTSC_CCS_BMSK) == 0) );  /* disconnected */

  #else /* FEATURE_HS_USB_HAL */

  return ( (HWIO_INF(USB_OTG_HS_PORTSC_MUSIM, SUSP) == 1) ||  /* suspended */
           (HWIO_INF(USB_OTG_HS_PORTSC_MUSIM, CCS)  == 0) );  /* disconnected */

  #endif /* FEATURE_HS_USB_HAL */
}
#endif  /* FEATURE_HS_USB_UICC */
#endif  /* !FEATURE_HS_USB_BASIC */
#endif  /* !T_REXNT */


#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
#if defined(FEATURE_HS_USB_SMSC_PHY)
#if !defined(T_REXNT) && !defined(FEATURE_HS_USB_BASIC)
/*===========================================================================
FUNCTION HSU_OS_BUS_SHUT_OFF_SMSC_PHY_COMPARATORS

DESCRIPTION
  Disable the VBUS Valid and Session-End interrupts in the SMSC USB3316 PHY.
  This is done in order to turn OFF unneeded comparators for the
  sake of power saving during low-power mode.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_shut_off_smsc_phy_comparators(void)
{

  /* Disable rising edge. Clear bits 1 and 3 in the PHY's USB interrupt 
  ** enable rising register. 
  */
  if (hsu_os_bus_write_to_phy(0x0F, 0x0A) == FALSE)
  {
    HSU_MSG_ERROR("hsu_os_bus_shut_off_smsc_phy_comparators: "
                  "failed to write to PHY register.",
                  0, 0, 0);

    HSU_ASSERT(FALSE);
  }

  /* Disable falling edge. Clear bits 1 and 3 in the PHY's USB interrupt 
  ** enable falling register. 
  */
  if (hsu_os_bus_write_to_phy(0x12, 0x0A) == FALSE)
  {
    HSU_MSG_ERROR("hsu_os_bus_shut_off_smsc_phy_comparators: "
                  "failed to write to PHY register.",
                  0, 0, 0);

    HSU_ASSERT(FALSE);
  }

}  /* hsu_os_bus_shut_off_smsc_phy_comparators */
#endif  /* !T_REXNT && !FEATURE_HS_USB_BASIC */
#endif  /* !FEATURE_HS_USB_SMSC_PHY */


#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
#if defined(FEATURE_HS_USB_SMSC_PHY)
/*===========================================================================
FUNCTION HSU_OS_BUS_TURN_ON_SMSC_PHY_COMPARATORS

DESCRIPTION
  Enable the VBUS Valid and Session-End interrupts in the SMSC USB3316 PHY.
  This is done right after exiting from low-power mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_turn_on_smsc_phy_comparators(void)
{

  /* Enable rising edge. Clear bits 1 and 3 in the PHY's USB interrupt 
  ** enable rising register. 
  */
  if (hsu_os_bus_write_to_phy(0x0E, 0x0A) == FALSE)
  {
    HSU_MSG_ERROR("hsu_os_bus_turn_on_smsc_phy_comparators: "
                  "failed to write to PHY register.",
                  0, 0, 0);

    HSU_ASSERT(FALSE);
  }

  /* Enable falling edge. Clear bits 1 and 3 in the PHY's USB interrupt 
  ** enable falling register. 
  */
    if (hsu_os_bus_write_to_phy(0x11, 0x0A) == FALSE)
  {
    HSU_MSG_ERROR("hsu_os_bus_turn_on_smsc_phy_comparators: "
                  "failed to write to PHY register.",
                  0, 0, 0);

    HSU_ASSERT(FALSE);
  }

}  /* hsu_os_bus_turn_on_smsc_phy_comparators */
#endif  /* FEATURE_HS_USB_SMSC_PHY */
#endif  /* !FEATURE_HS_USB_BASIC */
#endif  /* !T_REXNT */


#if defined(FEATURE_HS_USB_SMSC_PHY) || defined(FEATURE_HS_USB_PHILIPS_PHY)
#if (!defined(FEATURE_HSU_TEST) && !defined(FEATURE_HS_USB_BASIC))
/*===========================================================================
FUNCTION HSU_OS_BUS_CONFIG_GPIOS_FOR_TCXO_WAKEUP

DESCRIPTION
  Register for GPIO interrupts on the external SMSC/NXP (Philips) PHY GPIOs 
  in order to trigger a TCXO wakeup event when USB events occur during TCXO 
  shutdown mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_config_gpios_for_tcxo_wakeup(void)
{
  boolean gpio_api_call_succeeded;

  /* Make sure that the APPs processor is defined to be owner 
  ** of GPIO 112 and GPIO 114. Otherwise, and MAO driver might
  ** not recognize them as interrupt sources for TCXO wake up.
  ** This comes to resolve CR #170666 - 
  ** Phone doesn't wake up when resume event occurs during TCXO shutdown.
  */
  gpio_api_call_succeeded = 
    gpio_tlmm_config_remote(USBH_DATA_1);

  if (!gpio_api_call_succeeded)
  {
    HSU_MSG_ERROR("hsu_os_bus_config_gpios_for_tcxo_wakeup: "
                  "gpio_tlmm_config_remote(USBH_DATA_1) call failed.",
                  0, 0, 0);
  }

  gpio_api_call_succeeded = 
    gpio_tlmm_config_remote(USBH_DATA_3);

  if (!gpio_api_call_succeeded)
  {
    HSU_MSG_ERROR("hsu_os_bus_config_gpios_for_tcxo_wakeup: "
                  "gpio_tlmm_config_remote(USBH_DATA_3) call failed.",
                  0, 0, 0);
  }

  /* Register HS-USB USBH_DATA_3 for exiting TCXO shutdown in case of
  ** cable disconnection/reconnection.
  */
  gpio_api_call_succeeded = 
    gpio_int_set_detect((gpio_int_type)GPIO_INT_114, DETECT_LEVEL);

  HSU_ASSERT(gpio_api_call_succeeded);

  gpio_api_call_succeeded = 
    gpio_int_set_handler((gpio_int_type)GPIO_INT_114,
                          ACTIVE_HIGH, 
                          (gpio_int_handler_type)hsu_txco_wakeup_gpio_handler);

  HSU_ASSERT(gpio_api_call_succeeded);

  /* Register HS-USB USBH_DATA_1 for exiting TCXO shutdown in case of 
  ** selective suspend/resume.
  */
  gpio_api_call_succeeded = 
    gpio_int_set_detect((gpio_int_type)GPIO_INT_112, DETECT_EDGE);

  HSU_ASSERT(gpio_api_call_succeeded);

  gpio_api_call_succeeded = 
    gpio_int_set_handler((gpio_int_type)GPIO_INT_112,
                          ACTIVE_HIGH, 
                          (gpio_int_handler_type)hsu_txco_wakeup_gpio_handler);

  HSU_ASSERT(gpio_api_call_succeeded);

}  /* hsu_os_bus_config_gpios_for_tcxo_wakeup */
#endif  /* !FEATURE_HSU_TEST && !FEATURE_HS_USB_BASIC */
#endif  /* FEATURE_HS_USB_SMSC_PHY || FEATURE_HS_USB_PHILIPS_PHY*/


#if !(defined T_REXNT) && !(defined FEATURE_HS_USB_BASIC)
/*===========================================================================
FUNCTION HANDLE_LPM_VREG_REGISTRATION

DESCRIPTION
  This function registers/unregisters for relevant PMIC VREGs following enter/exit 
  LPM.

On some targets, VREG_5V is used as an input for the HS-USB PHY 3.3V LDO.
For these targets, we register for VREG_5V for normal operation and
unregister for LPM mode.

Handling VREG_5V for Host mode (for supplying VBUS) is done by 
hsu_api_vbus_powerup/shutdown

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None
===========================================================================*/
static void handle_lpm_vreg_registration(boolean register_for_vreg)
{
  #ifdef FEATURE_HS_USB_OTG
#error code not present
  #endif /* FEATURE_HS_USB_OTG */

  #ifdef FEATURE_HS_USB_PHY_DEPENDS_ON_VREG_5V
  if (register_for_vreg)
  {
    /* Vote to turn on VREG_5V in the PMIC */
    pm_vote_vreg_switch(PM_ON_CMD,
                        PM_VREG_BOOST_ID,
                        PM_VOTE_VREG_BOOST_APP__OTG);
  }
  else
  {
    /* Vote to turn off VREG_5V in the PMIC */
    pm_vote_vreg_switch(PM_OFF_CMD,
                        PM_VREG_BOOST_ID,
                        PM_VOTE_VREG_BOOST_APP__OTG);
  }
  #else
  HSU_USE_PARAM(register_for_vreg);
  #endif /* FEATURE_HS_USB_PHY_DEPENDS_ON_VREG_5V */

} /* handle_lpm_vreg_registration */
#endif  /* !T_REXNT && !FEATURE_HS_USB_BASIC */
#endif  /* !FEATURE_HS_USB_NO_HS_OTG_CORE */


#if !defined(FEATURE_HSU_TEST) && !defined(FEATURE_HS_USB_BASIC)
/*===========================================================================
FUNCTION HSU_ALL_CORES_IN_LPM

DESCRIPTION
  This function returns whether all of the HS-USB cores are currently in LPM.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE - All of the HS-USB cores are in LPM.
  FALSE - At least one of the HS-USB cores is not in LPM.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hsu_all_cores_in_lpm(void)
{
  uint32 core_index;
  usb_lpm_info_type* core_lpm_info;
  
  for (core_index = 0;
    core_index < NUMBER_OF_CORES;
    core_index++)
  { 
    if (hsu_core_config_get_core(OS_BUS_TO_HAL_CORE_ID(core_index)) == NULL)
    {
      continue;
    }
    core_lpm_info =
      &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);
    if (!core_lpm_info->is_in_low_power_mode)
    {
      return FALSE;
    }
  }
  return TRUE;
}  /* hsu_all_cores_in_lpm */
#endif  /* !(FEATURE_HSU_TEST) && !(FEATURE_HS_USB_BASIC) */

#if !defined(FEATURE_HSU_TEST) && !defined(FEATURE_HS_USB_BASIC)
/*===========================================================================
FUNCTION HSU_HANDLE_SLEEP_VOTE

DESCRIPTION
  This function is called when entering LPM. This function votes for sleep
  if all cores are in LPM and if sleep voting is enabled 
  (NV item hs_usb_disable_sleep_voting).
  This function is also disable the vote for sleep if one of the core
  exit form LPM.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_handle_sleep_vote(boolean assert_okts)
{
  if (assert_okts)
  {
    if (hsu_all_cores_in_lpm())
    {
      if (!hsu_conf_sel_disable_sleep_voting())
      {
        sleep_assert_okts(file_static_vars_ptr->hsu_sleep_vote_handle);
        HSU_MSG_HIGH("hsu_handle_sleep_vote() - Voted for sleep.", 0, 0, 0);
      }
      else
      {
        HSU_MSG_HIGH("hsu_handle_sleep_vote() - "
          "All cores are in LPM but Sleep voting is not enabled: "
          "Didn't vote for sleep.", 0, 0, 0);
      }
    }
    else
    {
      HSU_MSG_HIGH("hsu_handle_sleep_vote() - "
                   "One of the cores is not in LPM: Didn't vote for sleep.",
                   0, 0, 0);
    }     
  }
  else
  {
    sleep_negate_okts(file_static_vars_ptr->hsu_sleep_vote_handle);
    HSU_MSG_HIGH("hsu_handle_sleep_vote() - Disable the sleep vote.", 0, 0, 0);    
  }
}  /* hsu_handle_sleep_vote */
#endif  /* !(FEATURE_HSU_TEST) && !(FEATURE_HS_USB_BASIC) */

#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
#if defined(T_MSM6246) || defined(T_MSM6290) || defined(T_QSC6270)
/*===========================================================================
FUNCTION HSU_HANDLE_AHB_CONST_REGISTRATION

DESCRIPTION
  In some targets HS-USB should register with clk regime to keep AHB at 
  constant speed during ARM halt when USB is not in LPM. This function handles
  the registration / de-registration according to LPM state of all USB cores.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_handle_ahb_const_registration(boolean assert_ahb_const)
{
  if (assert_ahb_const)
  {
    /* If any core requires AHB to be const, register with clk regime */
    clk_regime_register_ahb_constant(CLK_AHB_USB);
    HSU_MSG_HIGH("hsu_handle_ahb_const_registration() - registered", 
      0, 0, 0);
  }
  else
  { /* If all cores are in LPM, we can de-register from clk regime */
    if (hsu_all_cores_in_lpm())
    {
      clk_regime_deregister_ahb_constant(CLK_AHB_USB);
      HSU_MSG_HIGH("hsu_handle_ahb_const_registration() - deregistered", 
        0, 0, 0);
    }
  }
} /* hsu_handle_ahb_const_registration */
#endif /* T_MSM6246 || T_MSM6290 || T_QSC6270 */

/*===========================================================================
FUNCTION HSU_HANDLE_CPU_RESOURCES_REGISTRATION

DESCRIPTION
  In some targets HS-USB should register with clk regime to keep from 
  dynamic clock rate switching when USB is not in LPM. This function handles 
  the registration / de-registration according to LPM state of all USB cores.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_handle_cpu_resources_registration
(
  boolean assert_cpu_regisrtation
)
{
  if (assert_cpu_regisrtation)
  {
    /* If any core requires CPU resources, register with clk regime */
    clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_HS_USB);
    HSU_MSG_HIGH("hsu_handle_cpu_resources_registration() - registered", 
      0, 0, 0);
  }
  else
  { /* If all cores are in LPM, we can de-register from clk regime */
    if (hsu_all_cores_in_lpm())
    {
      clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_HS_USB);
      HSU_MSG_HIGH("hsu_handle_cpu_resources_registration() - deregistered", 
        0, 0, 0);
    }
  }
} /* hsu_handle_cpu_resources_registration */
#endif  /* T_REXNT */
#endif  /* FEATURE_HS_USB_BASIC */

#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC

/*===========================================================================
FUNCTION HSU_OS_BUS_UPDATE_TARGET_AFTER_LPM_ENTRY

DESCRIPTION
  Perform target specific steps after LPM entry, like turning off clocks, 
  releasing bus resources and controlling GPIOs.

  This kind of operations are usually heavy and often require RPC calls
  in dual processor targets. Therefore, this function is intended to be 
  executed only when the USB port is disconnected, in order to preserve the 
  remote wakeup and reset from suspend timings.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

#if ((defined(T_MSM7625) || defined(T_MSM7600)) && !defined(T_MSM7800)) || \
     defined(T_MDM8200) || defined(T_MSM8650) || defined(T_MDM8900)
static void
hsu_os_bus_update_target_after_lpm_entry(boolean is_analog_audio_mode)
{
  #ifndef T_MSM8650
  HSU_USE_PARAM(is_analog_audio_mode);
  #endif  /* T_MSM8650 */


  HSU_MSG_HIGH("hsu_os_bus_update_target_after_lpm_entry: Entry.", 0, 0, 0);

  #ifdef T_MSM8650
  if (!is_analog_audio_mode)
  {
    if (hsu_api_use_pmic_otg_comparators())
    {
      hsu_api_turn_off_vreg_usb_ldo();
    }
  }
  #endif  /* T_MSM8650 */

  /* Withdraw the 5V power source request. */
  handle_lpm_vreg_registration(FALSE);

  if (file_static_vars_ptr->usbh_pclk_control_active)
  {
    /* Turn off HS-USB pbus clock. */
    clk_regime_msm_disable(CLKRGM_USB_HS_PCLK);
  }

  if (file_static_vars_ptr->dcvs_active)
  {
    /* Allow dynamic clock rate switching */
    hsu_handle_cpu_resources_registration(FALSE);
  }

  #ifndef FEATURE_HS_USB_INTEGRATED_PHY
  if (file_static_vars_ptr->gpio_control_active)
  {
   gpio_usbh_un_config();
  }
  #endif  /* !FEATURE_HS_USB_INTEGRATED_PHY */

  HSU_MSG_HIGH("hsu_os_bus_update_target_after_lpm_entry: Exit.", 0, 0, 0);

}  /* hsu_os_bus_update_target_after_lpm_entry */

#elif defined(T_MSM6246) || defined(T_MSM6290) || defined(T_QSC6270)

static void 
hsu_os_bus_update_target_after_lpm_entry(boolean is_analog_audio_mode)
{
  #ifdef FEATURE_DONT_USE_PMIC_OTG_COMPARATORS
  HSU_USE_PARAM(is_analog_audio_mode);
  #endif  /* FEATURE_DONT_USE_PMIC_OTG_COMPARATORS */

  HSU_MSG_HIGH("hsu_os_bus_update_target_after_lpm_entry: Entry.", 0, 0, 0);

  #ifndef T_QSC6270

  /* If the HS-USB port is not connected, and the PMIC OTG comparators
  ** are used for detecting USB cable insertions, the HS-USB PHY 
  ** 3.3V power source may be turned off(This power source is used for 
  ** D+ and ID pins pullups, and therefore is reqiured when the HS-USB 
  ** port is attached). 
  */
  #ifndef FEATURE_DONT_USE_PMIC_OTG_COMPARATORS
  if (!is_analog_audio_mode)
  {
    hsu_api_turn_off_vreg_usb_ldo();
  }
  #endif  /* FEATURE_DONT_USE_PMIC_OTG_COMPARATORS */

  #else /* T_QSC6270 */
  if (hsu_api_use_pmic_otg_comparators())
  { 
    hsu_api_turn_off_vreg_usb_ldo();
  }
  #endif  /* !T_QSC6270 */

  handle_lpm_vreg_registration(FALSE);

  if (file_static_vars_ptr->usbh_pclk_control_active)
  {
    /* Turn off HS-USB core's registers clock. */
    #ifndef T_QSC6270
    clk_regime_disable(CLK_RGM_USB_OTG_1_M);
    #else
    clk_regime_disable(CLK_RGM_USB_M);
    #endif  /* !T_QSC6270 */

    /* We do not need to keep AHB at constant sleep during LPM. */
    hsu_handle_ahb_const_registration(FALSE);
  }

  if (file_static_vars_ptr->dcvs_active)
  {
    /* Allow dynamic clock rate switching */
    hsu_handle_cpu_resources_registration(FALSE);
  }

  HSU_MSG_HIGH("hsu_os_bus_update_target_after_lpm_entry: Exit.", 0, 0, 0);

}  /* hsu_os_bus_update_target_after_lpm_entry */

#elif defined(T_QSC1100)
static void 
hsu_os_bus_update_target_after_lpm_entry(boolean is_analog_audio_mode)
{
  HSU_USE_PARAM(is_analog_audio_mode);


  HSU_MSG_HIGH("hsu_os_bus_update_target_after_lpm_entry: Entry.",
               0, 0, 0);

  if (file_static_vars_ptr->usbh_pclk_control_active)
  {
    /* Turn off HS-USB pbus clock. */
    clk_regime_msm_disable(CLK_RGM_USB_AHB_CLK_M);
  }

  /* Disable the USB serial clock. */
  clk_regime_msm_disable(CLK_RGM_USB_MCLK_M);


  if (file_static_vars_ptr->dcvs_active)
  {
    /* Allow dynamic clock rate switching */
    hsu_handle_cpu_resources_registration(FALSE);
  }

  HSU_MSG_HIGH("hsu_os_bus_update_target_after_lpm_entry: Exit.",
               0, 0, 0);

}  /* hsu_os_bus_update_target_after_lpm_entry */

#endif  /* (T_MSM7625 || T_MSM7600) && T_MSM7800 */

#endif  /* T_REXNT */
#endif  /* FEATURE_HS_USB_BASIC */


#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC


/*===========================================================================
FUNCTION HSU_OS_BUS_PREPARE_TARGET_FOR_LPM_EXIT

DESCRIPTION
  Perform target specific steps before PHY LPM exit, for allowing proper 
  functioning of the PHY after the LPM exit, like enabling clocks, 
  acquiring bus resources and configuring GPIOs. This function naturally 
  reverts the actions taken in the hsu_os_bus_update_target_after_lpm_entry()
  function after the latest PHY LPM entry.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#if ((defined(T_MSM7625) || defined(T_MSM7600)) && !defined(T_MSM7800)) || \
    defined(T_MDM8200) || defined(T_MSM8650) || defined(T_MDM8900)

static void hsu_os_bus_prepare_target_for_lpm_exit(void)
{

  HSU_MSG_HIGH("hsu_os_bus_prepare_target_for_lpm_exit: Entry.", 0, 0, 0);

  #ifndef FEATURE_HS_USB_INTEGRATED_PHY
  if (file_static_vars_ptr->gpio_control_active)
  {
    gpio_usbh_config();
  }
  #endif  /* !FEATURE_HS_USB_INTEGRATED_PHY */

  if (file_static_vars_ptr->dcvs_active)
  {
    /* Disallow dynamic clock rate switching. */
    hsu_handle_cpu_resources_registration(TRUE);
  }

  if (file_static_vars_ptr->usbh_pclk_control_active)
  {
    /* Turn on HS-USB pbus clock. */
    clk_regime_msm_enable(CLKRGM_USB_HS_PCLK);
  }

  /* Request 5V power source. */
  handle_lpm_vreg_registration(TRUE);

  #ifdef T_MSM8650
  if (hsu_api_use_pmic_otg_comparators()) 
  {
    hsu_api_turn_on_vreg_usb_ldo();
  }
  #endif  /* T_MSM8650 */

  HSU_MSG_HIGH("hsu_os_bus_prepare_target_for_lpm_exit: Exit.", 0, 0, 0);

}  /* hsu_os_bus_prepare_target_for_lpm_exit */

#elif defined(T_MSM6246) || defined(T_MSM6290) || defined(T_QSC6270)

static void hsu_os_bus_prepare_target_for_lpm_exit(void)
{

  HSU_MSG_HIGH("hsu_os_bus_prepare_target_for_lpm_exit: Entry.", 0, 0, 0);

  if (file_static_vars_ptr->dcvs_active)
  {
    /* Disallow dynamic clock rate switching. */
    hsu_handle_cpu_resources_registration(TRUE);
  }

  if (file_static_vars_ptr->usbh_pclk_control_active)
  {
    /* Turn on HS-USB core's registers clock. */
    #ifndef T_QSC6270
    clk_regime_enable(CLK_RGM_USB_OTG_1_M);
    #else
    clk_regime_enable(CLK_RGM_USB_M);
    #endif  /* !T_QSC6270 */

    /* Register to keep AHB speed constant during ARM halt */
    hsu_handle_ahb_const_registration(TRUE);
  }

  handle_lpm_vreg_registration(TRUE);

  #ifndef T_QSC6270

  #ifndef FEATURE_DONT_USE_PMIC_OTG_COMPARATORS
  hsu_api_turn_on_vreg_usb_ldo();
  #endif  /* FEATURE_DONT_USE_PMIC_OTG_COMPARATORS */
  
  #else
  if (hsu_api_use_pmic_otg_comparators()) 
  {
    hsu_api_turn_on_vreg_usb_ldo();
  }
  #endif  /* !T_QSC6270 */

  HSU_MSG_HIGH("hsu_os_bus_prepare_target_for_lpm_exit: Exit.", 0, 0, 0);

}  /* hsu_os_bus_prepare_target_for_lpm_exit */

#elif defined(T_QSC1100)
static void hsu_os_bus_prepare_target_for_lpm_exit(void)
{
  HSU_MSG_HIGH("hsu_os_bus_prepare_target_for_lpm_exit: Entry.", 0, 0, 0);

  if (file_static_vars_ptr->dcvs_active)
  {
    /* Disallow dynamic clock rate switching. */
    hsu_handle_cpu_resources_registration(TRUE);
  }

  /* Enable the USB serial clock. */
  clk_regime_msm_enable(CLK_RGM_USB_MCLK_M);

  if (file_static_vars_ptr->usbh_pclk_control_active)
  {
    /* Turn on HS-USB pbus clock. */
    clk_regime_msm_enable(CLK_RGM_USB_AHB_CLK_M);  
  }

  HSU_MSG_HIGH("hsu_os_bus_prepare_target_for_lpm_exit: Exit.", 0, 0, 0);

}  /* hsu_os_bus_prepare_target_for_lpm_exit */
#endif  /* (T_MSM7625 || T_MSM7600) && T_MSM7800 */

#endif  /* !FEATURE_HS_USB_BASIC */
#endif  /* !T_REXNT */
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */


#if defined(T_QSC1100) || defined (FEATURE_HS_USB_LEGACY_DEVICE)
#if !defined(FEATURE_HSU_TEST) && !defined(FEATURE_HS_USB_BASIC)
/*===========================================================================
FUNCTION HSU_OS_BUS_ENTER_PMIC_PHY_INTO_LPM

DESCRIPTION
  Enter the PMIC FS-USB transceiver into low-power mode.
  This function is invoked from the High-Speed USB module's main LPM 
  entry function.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the PHY has successfully entered LPM. Otherwise, FALSE is
  returned.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hsu_os_bus_enter_pmic_phy_into_lpm(void)
{
  pm_err_flag_type result;

  /* Put the PMIC OTG transceiver in suspend mode. */
  result = pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__SUSPEND);
  HSU_ASSERT(result == PM_ERR_FLAG__SUCCESS);

  return (result == PM_ERR_FLAG__SUCCESS);
}  /* hsu_os_bus_enter_pmic_phy_into_lpm */

#endif  /* !FEATURE_HSU_TEST && !FEATURE_HS_USB_BASIC */
#endif  /* T_QSC1100 || FEATURE_HS_USB_LEGACY_DEVICE */


#if defined(T_QSC1100) || defined (FEATURE_HS_USB_LEGACY_DEVICE)
#if !defined(FEATURE_HSU_TEST) && !defined(FEATURE_HS_USB_BASIC)
/*===========================================================================
FUNCTION HSU_OS_BUS_EXIT_PMIC_PHY_FROM_LPM

DESCRIPTION
  Take the PMIC FS-USB transceiver out from low-power mode.
  This function is invoked from the High-Speed USB module's main LPM 
  entry function.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the PHY has successfully exited LPM. Otherwise, FALSE is
  returned.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hsu_os_bus_exit_pmic_phy_from_lpm(void)
{
  pm_err_flag_type result;


  /* Put the PMIC OTG transceiver in suspend mode. */
  result = 
    pm_otg_config_power_management(PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE);

  HSU_ASSERT(result == PM_ERR_FLAG__SUCCESS);

  return(result == PM_ERR_FLAG__SUCCESS);

}  /* hsu_os_bus_exit_pmic_phy_from_lpm */

#endif  /* !FEATURE_HSU_TEST && !FEATURE_HS_USB_BASIC */
#endif  /* T_QSC1100 || FEATURE_HS_USB_LEGACY_DEVICE */


#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE

#if defined(T_MSM6246) ||  \
    defined(T_QSC6270) ||  \
    defined(T_MSM6290) ||  \
    defined(T_MSM7600) ||  \
    defined(T_MSM7625) ||  \
    defined(T_MDM8200) ||  \
    defined(T_MSM8650) ||  \
    defined(T_MDM8900)

#if !defined(FEATURE_HSU_TEST) && !defined(FEATURE_HS_USB_BASIC)
/*===========================================================================
FUNCTION HSU_OS_BUS_ENTER_PHY_INTO_LPM

DESCRIPTION
  Enter the High-Speed USB PHY into low-power mode.
  This function is invoked from the High-Speed USB module's main LPM 
  entry function.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the PHY has successfully entered LPM. Otherwise, FALSE is
  returned.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hsu_os_bus_enter_phy_into_lpm(uint32 core_index)
{
  /* Delay (in milliseconds) between writing the PHCD bit and reading
  ** its value.
  */
  #if defined(T_MSM7625) || defined(T_MSM8650)
  /* On integrated PHY targets it was observed that sometimes after writing a 
  ** logical '1' to the PHCD bit, it becomes set for a short time, and then
  ** reverts back to a logical '0' again (and LPM entry fails). Therefore,
  ** a 1 millisecond delay was inserted between the write operation to this bit
  ** and read operation after it, in order to make sure that we read a stable
  ** value.
  ** 
  ** TODO - Enable this for all of the integrated PHY targets.
  */
  const uint32  
    phcd_bit_read_after_write_delay_ms = 1;
  #else
  const uint32  
    phcd_bit_read_after_write_delay_ms = 0;
  #endif  /* T_MSM7625 || T_MSM8650 */

  /* Number of trials left for setting the PHCD bit. */
  uint32 
    phcd_bit_write_trials_left         = MAX_PHCD_BIT_WRITE_TRIALS;

  boolean return_value;


  #ifndef FEATURE_HS_USB_HAL
  HSU_USE_PARAM(core_index);
  #endif  /* FEATURE_HS_USB_HAL */

  /* SW workaround - Sometimes the LPM entry fails. Therefore, there are 
  ** retries for setting the PHCD bit.
  */
  return_value = FALSE;
  while (phcd_bit_write_trials_left > 0)
  {
    /* The PHY may be entered into low-power mode only
    ** when the core is not running, when it is suspended, or when it
    ** is not connected.
    ** If none of these conditions is met, abort the transition
    ** to low-power.
    ** 
    */

    /* SW workaround: Sometimes, if we try to enter the PHY into low-power 
    ** mode when the core has just been resumed from suspend, the PHY still 
    ** tries to enter low-power mode and gets stuck in an undefined state.
    ** Therefore, before trying to write to the PHCD bit, we check whether
    ** the core is ready for LPM.
    */
    if (!core_ready_for_lpm())
    {
      HSU_MSG_HIGH("hsu_os_bus_enter_phy_into_lpm: "
                   "USB core is not ready for LPM. Canceling LPM.",
                    0, 0, 0);

      break;
    }

    /* Set the PHCD bit in the port control register. 
    ** This stops the PHY clock and transfers the PHY into
    ** low power mode state.
    */
    #ifdef FEATURE_HS_USB_HAL

    HAL_hsusb_SetBits32(
      OS_BUS_TO_HAL_CORE_ID(core_index),
      HAL_HSUSB_PORTSC_ADDR(0), HAL_HSUSB_PORTSC_PHCD_BMSK); 

    #else

    HWIO_OUTF(USB_OTG_HS_PORTSC, PHCD, 1);

    #endif  /* FEATURE_HS_USB_HAL */

    phcd_bit_write_trials_left--;


    /* Let the PHCD bit stabilize before we test it.
    ** This is a SW workaround for a hardware issue: Sometimes the PHCD bit
    ** read value is '1' after we start LPM entry, but after then the LPM
    ** entry fails and the PHCD bit read value becomes zero. This delay
    ** ensures that we read a stable value of the PHCD bit.
    */
    if (phcd_bit_read_after_write_delay_ms > 0)
    {
      os_delay_us(phcd_bit_read_after_write_delay_ms * 1000);
    }

    /* Check whether the PHY has completed its transition into 
    ** low power mode.
    */
    #ifdef FEATURE_HS_USB_HAL

    if (HAL_hsusb_GetBits32(
      OS_BUS_TO_HAL_CORE_ID(core_index),
      HAL_HSUSB_PORTSC_ADDR(0),HAL_HSUSB_PORTSC_PHCD_BMSK) != 0)
    {
      return_value = TRUE;
      break;
    }

    #else

    if (HWIO_INF(USB_OTG_HS_PORTSC, PHCD) == 1)
    {
      return_value = TRUE;
      break;
    }

    #endif  /* FEATURE_HS_USB_HAL */

  }  /* while (phcd_bit_write_trials_left > 0) */

  if (return_value == FALSE)
  {
    HSU_MSG_ERROR("hsu_os_bus_enter_phy_into_lpm: "
                  "Failed to put the PHY in LPM.",
                  0, 0, 0);
  }

  return return_value;

}  /* hsu_os_bus_enter_phy_into_lpm */
#endif  /* !FEATURE_HSU_TEST && !FEATURE_HS_USB_BASIC */
#endif  /* T_MSM6246 || T_QSC6270 || T_MSM6290 || T_MSM7600 || T_MSM7625 */


#if defined(T_MSM6246) ||  \
    defined(T_QSC6270) ||  \
    defined(T_MSM6290) ||  \
    defined(T_MSM7625) ||  \
    defined(T_MDM8200) ||  \
    defined(T_MSM8650) ||  \
    defined(T_MDM8900)

#if !defined(FEATURE_HSU_TEST) && !defined(FEATURE_HS_USB_BASIC) 
/*===========================================================================
FUNCTION HSU_OS_BUS_EXIT_PHY_FROM_LPM

DESCRIPTION
  Take the High-Speed USB PHY out from low-power mode.
  This function is invoked from the High-Speed USB module's main LPM 
  entry function.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the PHY has successfully exited from LPM. Otherwise, FALSE is
  returned.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hsu_os_bus_exit_phy_from_lpm(uint32 core_index)
{
  uint32  loop_count;

  /* Maximum number of trials for setting the PHCD bit. */


  /* Set the PHCD bit in the port control register to zero. 
  ** This restarts the ULPI clock and transfers the PHY into
  ** normal operating state.
  */
  #ifdef FEATURE_HS_USB_HAL

  HAL_hsusb_ClearBits32(
    OS_BUS_TO_HAL_CORE_ID(core_index),
    HAL_HSUSB_PORTSC_ADDR(0), HAL_HSUSB_PORTSC_PHCD_BMSK); 

  #else /* FEATURE_HS_USB_HAL */

  HWIO_OUTF(USB_OTG_HS_PORTSC, PHCD, 0);

  #endif /* FEATURE_HS_USB_HAL */

  /* Wait until the PHY finished its transition out 
  ** of low power mode.
  */
  loop_count = 0;

  #ifdef FEATURE_HS_USB_HAL

  while (HAL_hsusb_GetBits32(
    OS_BUS_TO_HAL_CORE_ID(core_index),
    HAL_HSUSB_PORTSC_ADDR(0), HAL_HSUSB_PORTSC_PHCD_BMSK) 
    != 0)

  #else /* FEATURE_HS_USB_HAL */

  while (HWIO_INF(USB_OTG_HS_PORTSC, PHCD) != 0)

  #endif /* FEATURE_HS_USB_HAL */
  {
    /* Busy-wait loop. */
    loop_count++;
    if (loop_count == MAX_PHCD_BIT_READ_TRIALS)
    {
      return FALSE;
    }
  }

  return TRUE;
}  /* hsu_os_bus_exit_phy_from_lpm */
#endif  /* !FEATURE_HSU_TEST && !FEATURE_HS_USB_BASIC */
#endif  /* T_MSM6246 || T_QSC6270 || T_MSM6290 || T_MSM7625 ||
        ** T_MDM8200 || T_MSM8650 || T_MDM8900
        */

#if defined(FEATURE_HS_USB_INTEGRATED_PHY)
#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================
FUNCTION HSU_OS_BUS_CONFIG_INTG_PHY_FOR_LPM

DESCRIPTION
  Configure the HS-USB integrated PHY before putting it in low-power mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
static void hsu_os_bus_config_intg_phy_for_lpm(uint32 core_index)
{
  const uint8  phy_interrupt_latch_reg_address = 0x14;

  const uint8  phy_config_1_reg_address = 0x30;
  const uint8  disable_conf2            = 0x0; /*conf2=bit 0 */
  const uint8  enable_conf2             = 0x01;          
  boolean      result                   = FALSE;
  uint8        read_result;


#ifdef T_MSM8650
  usb_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);
#endif  /* T_MSM8650 */


  HSU_MSG_HIGH("hsu_os_bus_config_intg_phy_for_lpm: Entry.", 0, 0, 0);

  HSU_USE_PARAM(disable_conf2);
  HSU_USE_PARAM(enable_conf2);

  /* Clear the PHY interrupts by clearing the PHY interrupts latch register.
  */
  if (!hsu_os_bus_read_from_phy(phy_interrupt_latch_reg_address,
                                &read_result))
  {
    HSU_MSG_ERROR("hsu_os_bus_config_intg_phy_for_lpm: "
                  "Failed to read the interrupt latch register.",
                  0, 0, 0);

    HSU_ASSERT(FALSE);
  }

  hsu_os_bus_pll_shutdown();

  #ifdef T_MSM8650
  /* On Raptor2, Resume doesn't work properly when the PHY comparators
  ** are shut off. Therefore, they will be shut off only when the USB
  ** cable is not connected
  */
  if (core_lpm_info->cable_is_connected_during_lpm)
  {
    return;
  }
  #endif  /* T_MSM8650 */

  #if ( !defined(FEATURE_DONT_USE_PMIC_OTG_COMPARATORS) && \
        ( (defined(T_MSM6246) || defined(T_MSM6290)) && !defined(T_MSM7625)) )

  /* Turn off the HS-USB OTG comparators during LPM. Interrupts will
  ** be detected by PMIC, all but ID GRND which does not exist in LCU PMIC OTG 
  ** comparators, therefore the target won't be able to recognize a device
  ** connection to the target in LPM.
  */
  result = hsu_os_bus_write_to_phy(phy_config_1_reg_address, disable_conf2);

  #elif defined (T_QSC6270) || defined(T_MSM8650)

  if (!hsu_api_use_pmic_otg_comparators()) 
  {
    /* Configure the PHY to leave the OTG comparators on during low-power
    ** mode. OTG interrupts will be detected by the HS-USB PHY. Mini-A cable
    ** insertions or removals are also detectable in this state.
    */
    result = hsu_os_bus_write_to_phy(phy_config_1_reg_address, enable_conf2);
  }
  else
  {
    /* Turn off the HS-USB OTG comparators during LPM. Interrupts will
    ** be detected by PMIC, all but ID GRND which does not exist in LCU PMIC OTG 
    ** comparators, therefore the target won't be able to recognize a device
    ** connection to the target in LPM.
    */
    result = hsu_os_bus_write_to_phy(phy_config_1_reg_address, disable_conf2);

    HSU_MSG_HIGH ("hsu_os_bus_config_intg_phy_for_lpm: host mode detection "
                  "in LPM is disabled",
                  0, 0, 0);
  }


  #else /* T_QSC6270 */

  /* Configure the PHY to leave the OTG comparators on during low-power
  ** mode. OTG interrupts will be detected by the HS-USB PHY. Mini-A cable
  ** insertions or removals are also detectable in this state.
  */
  result = hsu_os_bus_write_to_phy(phy_config_1_reg_address, enable_conf2);

  #endif  /* !FEATURE_DONT_USE_PMIC_OTG_COMPARATORS && 
             (T_MSM8650 || ((T_MSM6246 || T_MSM6290) && !T_MSM7625)) */

  if (!result)
  {
    HSU_MSG_ERROR("hsu_os_bus_config_intg_phy_for_lpm: "
                  "Failed to write to the PHY configuration 1 register.",
                  0, 0, 0);

    HSU_ASSERT(FALSE);
  }

  HSU_MSG_HIGH("hsu_os_bus_config_intg_phy_for_lpm: Exit.", 0, 0, 0);

}  /* hsu_os_bus_config_intg_phy_for_lpm */
#endif  /* T_REXNT */
#endif  /* FEATURE_HS_USB_BASIC */
#endif  /* FEATURE_HS_USB_INTEGRATED_PHY */


#if defined(T_MSM6246) || defined(T_QSC6270) || defined(T_MSM6290) 
#if !defined(FEATURE_HSU_TEST) && !defined(FEATURE_HS_USB_BASIC)
/*===========================================================================
FUNCTION ENTER_PHY_INTO_LPM_FOR_ANALOG_AUDIO

DESCRIPTION
  Enter High-Speed USB PHY into low-power mode state when we configure the PHY
  to one of the Analog audio modes.
  This function is invoked from hsu_os_bus_enter_lpm(). See documentation of
  hsu_os_bus_enter_lpm for more details.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE/FALSE.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean enter_phy_into_lpm_for_analog_audio(uint32 core_index)
{
  /* In order to clear certain bits of the function control register (0x04)
  we write to register 0x06
  */
  const uint8  phy_function_control_reg_clear_addr = 0x06;

  /* In order to clear SuspendM bit we write 0x40 to reg 0x06*/
  const uint8  phy_SuspendM_clear_bit_val = 0x40;

  #ifndef FEATURE_HS_USB_HAL
  uint32   viewport_reg_value;
  uint32  trials_count = 0;
  #endif /*FEATURE_HS_USB_HAL*/

  if (!core_ready_for_lpm())
  {
    return FALSE;
  }

  /* Clear bit 6 (SuspendM) of reg 0x4 (Function Control Register)
  Clearing this bit places the PHY in low power consumption mode. */
  #ifdef FEATURE_HS_USB_HAL

  if (HAL_hsusb_WriteUlpiPhy(
    OS_BUS_TO_HAL_CORE_ID(core_index), 0, 
    phy_function_control_reg_clear_addr, phy_SuspendM_clear_bit_val) != HAL_HSUSB_Success)
  {
    HSU_ASSERT(FALSE);       
    return FALSE;
  }

  #else /*!FEATURE_HS_USB_HAL*/

  viewport_reg_value = 
    HWIO_FMSK(USB_OTG_HS_ULPI_VIEWPORT, ULPIRUN) |
      (phy_function_control_reg_clear_addr << 
       HWIO_SHFT(USB_OTG_HS_ULPI_VIEWPORT, ULPIADDR));

  viewport_reg_value |= 
    HWIO_FMSK(USB_OTG_HS_ULPI_VIEWPORT, ULPIRW) |
    (phy_SuspendM_clear_bit_val << HWIO_SHFT(USB_OTG_HS_ULPI_VIEWPORT, ULPIDATWR));

  HWIO_OUT(USB_OTG_HS_ULPI_VIEWPORT, viewport_reg_value);

  /* Wait until the ULPI run bit gets cleared */
  trials_count = 0;
  while (HWIO_INF(USB_OTG_HS_ULPI_VIEWPORT, ULPIRUN) == 1)
  {
    trials_count++;

    if (trials_count == 10000)
    {
      HSU_MSG_ERROR("enter_phy_into_lpm_for_analog_audio: "
                    "Failed to access PHY. register: %u",
                    phy_function_control_reg_clear_addr, 0, 0);

      HSU_ASSERT(FALSE);
      return = FALSE;
    }
  }

  #endif /*FEATURE_HS_USB_HAL*/

  return TRUE;
}  /* enter_phy_into_lpm_for_analog_audio */
#endif  /* !(FEATURE_HSU_TEST) && !(FEATURE_HS_USB_BASIC) */
#endif  /* T_MSM6246) || T_QSC6270 || T_MSM6290 */


#if !defined(FEATURE_HSU_TEST) && !defined(FEATURE_HS_USB_BASIC)
/*===========================================================================
FUNCTION HSU_OS_BUS_ENTER_LPM

DESCRIPTION
  Enter High-Speed USB PHY into low-power mode state.
  This function is invoked when the High-Speed USB bus is suspended, in 
  order to save battery power.
  This function stops the High-Speed USB core PHY clock and enables
  asynchronous interrupts from the PHY.
  After the PHY is entered into a low power-state, this function asserts
  the High-Speed USB sleep flag in the sleep task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#if defined(T_MSM7625) || defined(T_MDM8200) || \
    defined(T_MSM8650) || defined(T_MDM8900)

/**********************************/
/* sRoc\Mustang\Raptor2\Skywalker */
/**********************************/
void hsu_os_bus_enter_lpm(
  uint32 core_index, 
  boolean called_by_lpm_timer,
  boolean is_analog_audio_mode
)
{

  usb_lpm_info_type* const core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  boolean phy_entered_lpm;


  HSU_USE_PARAM(is_analog_audio_mode);

  /* Do not access PHY during USB charger is connected (SE1 state) */
  if (HAL_hsusb_GetBits32((uint8)core_index,
                          HAL_HSUSB_PORTSC_ADDR(0), 
                          HAL_HSUSB_PORTSC_LS_BMSK) == HAL_HSUSB_PORTSC_LS_BMSK)
  {
    HSU_MSG_HIGH("hsu_os_bus_enter_lpm: do not enter LPM on SE1", 0, 0, 0);
    return;
  }

  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  if (!file_static_vars_ptr->lpm_feature_is_active || 
      !core_lpm_info->lpm_is_active)
  {
    rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
    return;
  }

  /* Un-register the HS-USB interrupt handler. */
  hsu_os_bus_interrupt_uninit(core_index);

  HSU_MSG_HIGH("hsu_os_bus_enter_lpm: Entry.", 0, 0, 0);

  if (!(core_lpm_info->is_in_low_power_mode) && 
       (!called_by_lpm_timer || core_lpm_info->lpm_entry_is_pending))
  {
    /* Save the cable connection status. */
    core_lpm_info->cable_is_connected_during_lpm = 
      core_lpm_info->port_is_attached(core_index);

    HSU_MSG_HIGH("hsu_os_bus_enter_lpm: cable_is_connected_during_lpm = %d.", 
                 core_lpm_info->cable_is_connected_during_lpm,
                 0, 0);

    hsu_os_bus_config_intg_phy_for_lpm(core_index);

    core_lpm_info->is_in_low_power_mode = TRUE;
    core_lpm_info->lpm_entry_is_pending = FALSE;

    phy_entered_lpm = hsu_os_bus_enter_phy_into_lpm(core_index);

    /* If the LPM entry has succeeded, perform the rest of the power-saving 
    ** steps.
    */
    if (phy_entered_lpm)
    {

      /* Block the STP signal from going out from the USB core to the PHY.
      */
      #ifdef FEATURE_HS_USB_HAL

      HAL_hsusb_SetBits32(
        OS_BUS_TO_HAL_CORE_ID(core_index),
        HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK); 

      #else /* FEATURE_HS_USB_HAL */

      HWIO_OUTF(USB_OTG_HS_USBCMD, ULPI_STP_CTRL, 1);

      #endif /* FEATURE_HS_USB_HAL */

      /* Enable the USB core to send asynchronous interrupts to the MSM during 
      ** low power mode.
      */
      #ifdef FEATURE_HS_USB_HAL

      HAL_hsusb_SetBits32(
        OS_BUS_TO_HAL_CORE_ID(core_index),
        HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_BMSK); 

      #else /* FEATURE_HS_USB_HAL */

      HWIO_OUTF(USB_OTG_HS_USBCMD, ASYNC_INTR_CTRL, 1);

      #endif /* FEATURE_HS_USB_HAL */

      if (!core_lpm_info->cable_is_connected_during_lpm)
      {
        hsu_os_bus_update_target_after_lpm_entry(is_analog_audio_mode);
      }
      #if defined(T_MDM8200)
      else
      {
        if (file_static_vars_ptr->dcvs_active)
        {
          /* Allow dynamic clock rate switching */
          clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_HS_USB);
        }
      }
      #endif /* T_MDM8200 */

      hsu_handle_sleep_vote(TRUE);
    }
    else  /* phy_entered_lpm == FALSE */
    {
      core_lpm_info->is_in_low_power_mode = FALSE;
    }  /* if (phy_entered_lpm) */

  }  /* !(file_static_vars_ptr->is_in_low_power_mode) &&
     ** (!called_by_lpm_timer || file_static_vars_ptr->lpm_entry_is_pending))
     */

  HSU_MSG_HIGH("hsu_os_bus_enter_lpm: Exit.", 0, 0, 0);

  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  /* Register the HS-USB interrupt back. */
  hsu_os_bus_interrupt_init(core_index);
}  /* hsu_os_bus_enter_lpm */

#elif defined(T_MSM7800) 
/************************/
/*        Serra         */
/************************/
void hsu_os_bus_enter_lpm(
                          uint32 core_index, 
                          boolean called_by_lpm_timer,
                          boolean is_analog_audio_mode
                          )
{
  /* skip ENTER LPM for bring-up build */
}  /* hsu_os_bus_enter_lpm */

#elif defined(T_MSM7600)

void hsu_os_bus_enter_lpm
(
  uint32 core_index, 
  boolean called_by_lpm_timer,
  boolean is_analog_audio_mode
)
{
  usb_lpm_info_type* const core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  boolean phy_entered_lpm;


  HSU_USE_PARAM(is_analog_audio_mode);

  /* Do not access PHY during USB charger is connected (SE1 state) */
  if (HAL_hsusb_GetBits32((uint8)core_index,
                          HAL_HSUSB_PORTSC_ADDR(0), 
                          HAL_HSUSB_PORTSC_LS_BMSK) == HAL_HSUSB_PORTSC_LS_BMSK)
  {
    HSU_MSG_HIGH("hsu_os_bus_enter_lpm: do not enter LPM on SE1", 0, 0, 0);
    return;
  }

  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  if (!file_static_vars_ptr->lpm_feature_is_active || 
      !core_lpm_info->lpm_is_active)
  {
    rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
    return;
  }

  /* Un-register the HS-USB interrupt handler. */
  hsu_os_bus_interrupt_uninit(core_index);

  HSU_MSG_HIGH("hsu_os_bus_enter_lpm: Entry.", 0, 0, 0);

  if (!(core_lpm_info->is_in_low_power_mode) && 
       (!called_by_lpm_timer || core_lpm_info->lpm_entry_is_pending))
  {
    /* Save the cable connection status. */
    if (core_lpm_info->port_is_attached != NULL)
    {
      core_lpm_info->cable_is_connected_during_lpm = 
        core_lpm_info->port_is_attached(core_index);
    }
    else
    {
      core_lpm_info->cable_is_connected_during_lpm = TRUE;
    }

    HSU_MSG_HIGH("hsu_os_bus_enter_lpm: cable_is_connected_during_lpm = %d.", 
                 core_lpm_info->cable_is_connected_during_lpm,
                 0, 0);

    #if defined(FEATURE_HS_USB_SMSC_PHY)
    hsu_os_bus_shut_off_smsc_phy_comparators();
    #endif  /* FEATURE_HS_USB_SMSC_PHY */

    core_lpm_info->is_in_low_power_mode = TRUE;
    core_lpm_info->lpm_entry_is_pending = FALSE;

    phy_entered_lpm = hsu_os_bus_enter_phy_into_lpm(core_index);

    /* If the LPM entry has succeeded, perform the rest of the power-saving 
    ** steps.
    */
    if (phy_entered_lpm)
    {
      #ifdef FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND
      if (file_static_vars_ptr->usb_reset_rework_is_installed)
      {
        /* Enable workaround for USB_RESET waking TCXO shutdown */
        hsu_api_enable_pmic_ulpidata0_irq_handler();
      }
      #endif /* FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */

      /* Block the STP signal from going out from the USB core to the PHY.
      */
      #ifdef FEATURE_HS_USB_HAL

      HAL_hsusb_SetBits32(
        OS_BUS_TO_HAL_CORE_ID(core_index),
        HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK); 

      #else /* FEATURE_HS_USB_HAL */

      HWIO_OUTF(USB_OTG_HS_USBCMD, ULPI_STP_CTRL, 1);

      #endif /* FEATURE_HS_USB_HAL */

      /* Enable asynchronous interrupts during low power mode. */
      #ifdef FEATURE_HS_USB_HAL

      HAL_hsusb_SetBits32(
        OS_BUS_TO_HAL_CORE_ID(core_index),
        HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_BMSK); 

      #else /* FEATURE_HS_USB_HAL */

      HWIO_OUTF(USB_OTG_HS_USBCMD, ASYNC_INTR_CTRL, 1);

      #endif /* FEATURE_HS_USB_HAL */

      if (!core_lpm_info->cable_is_connected_during_lpm)
      {
        hsu_os_bus_update_target_after_lpm_entry(is_analog_audio_mode);
      }
      hsu_os_bus_config_gpios_for_tcxo_wakeup();
      hsu_handle_sleep_vote(TRUE);

    }
    else  /* phy_entered_lpm == FALSE */
    {
      core_lpm_info->is_in_low_power_mode = FALSE;      

      #if defined(FEATURE_HS_USB_SMSC_PHY)
      hsu_os_bus_turn_on_smsc_phy_comparators();
      #endif  /* FEATURE_HS_USB_SMSC_PHY */
    }  /* if (phy_entered_lpm) */

  }  /* !(file_static_vars_ptr->is_in_low_power_mode) && 
        (!called_by_lpm_timer || file_static_vars_ptr->lpm_entry_is_pending) */

  HSU_MSG_HIGH("hsu_os_bus_enter_lpm: Exit.", 0, 0, 0);

  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  /* Register the HS-USB interrupt back. */
  hsu_os_bus_interrupt_init(core_index);
}  /* hsu_os_bus_enter_lpm */

#elif defined(T_MSM6246) || defined(T_QSC6270) || defined(T_MSM6290) 
/************************************************/
/*              Koel \ LCU \ Hawk               */
/************************************************/
void hsu_os_bus_enter_lpm(uint32  core_index, 
                          boolean called_by_lpm_timer,
                          boolean is_analog_audio_mode
)
{
  usb_lpm_info_type* const core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  boolean phy_entered_lpm;


  /* Do not access PHY during USB charger is connected (SE1 state) */
  if (HAL_hsusb_GetBits32((uint8)core_index,
                          HAL_HSUSB_PORTSC_ADDR(0), 
                          HAL_HSUSB_PORTSC_LS_BMSK) == HAL_HSUSB_PORTSC_LS_BMSK)
  {
    HSU_MSG_HIGH("hsu_os_bus_enter_lpm: do not enter LPM on SE1", 0, 0, 0);
    return;
  }

  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  if (!file_static_vars_ptr->lpm_feature_is_active || 
      !core_lpm_info->lpm_is_active)
  {
    rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
    return;
  }

  /* Un-register the HS-USB interrupt handler. */
  hsu_os_bus_interrupt_uninit(core_index);

  HSU_MSG_HIGH("hsu_os_bus_enter_lpm: Entry.", 0, 0, 0);

  if (!(core_lpm_info->is_in_low_power_mode) && 
      (!called_by_lpm_timer || core_lpm_info->lpm_entry_is_pending))
  {
    /* Save the cable connection status. */
    core_lpm_info->cable_is_connected_during_lpm = 
      hsu_port_is_attached(core_index);

    HSU_MSG_HIGH("hsu_os_bus_enter_lpm: cable_is_connected_during_lpm = %d.", 
                 core_lpm_info->cable_is_connected_during_lpm,
                 0, 0);

    hsu_os_bus_config_intg_phy_for_lpm(core_index);

    core_lpm_info->is_in_low_power_mode = TRUE;
    core_lpm_info->lpm_entry_is_pending = FALSE;

    if (is_analog_audio_mode) 
    {
      phy_entered_lpm = enter_phy_into_lpm_for_analog_audio(core_index);
    }
    else
    {
      phy_entered_lpm = hsu_os_bus_enter_phy_into_lpm(core_index);
    }

    /* If the LPM entry has succeeded, perform the rest of the power-saving 
    ** steps.
    */
    if (phy_entered_lpm)
    {

      /* Block the STP signal from going out from the USB core to the PHY.
      */
      #ifdef FEATURE_HS_USB_HAL

      HAL_hsusb_SetBits32(
        OS_BUS_TO_HAL_CORE_ID(core_index),
        HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK); 

      #else /* FEATURE_HS_USB_HAL */

      HWIO_OUTF(USB_OTG_HS_USBCMD, ULPI_STP_CTRL, 1);

      #endif /* FEATURE_HS_USB_HAL */

      /* Enable the USB core to send asynchronous interrupts to the MSM during 
      ** low power mode.
      */
      #ifdef FEATURE_HS_USB_HAL

      HAL_hsusb_SetBits32(
        OS_BUS_TO_HAL_CORE_ID(core_index),
        HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_BMSK); 

      #else /* FEATURE_HS_USB_HAL */

      HWIO_OUTF(USB_OTG_HS_USBCMD, ASYNC_INTR_CTRL, 1);

      #endif /* FEATURE_HS_USB_HAL */

      if (!core_lpm_info->cable_is_connected_during_lpm)
      {
        hsu_os_bus_update_target_after_lpm_entry(is_analog_audio_mode);
      }

      /* !!! Danny - TEST !!! */
      pm_mbg_lpm_enable(FALSE, FALSE);
      /* !!! Danny - TEST !!! */

      hsu_handle_sleep_vote(TRUE);

    }
    else  /* phy_entered_lpm == FALSE */
    {
      core_lpm_info->is_in_low_power_mode = FALSE;
    }  /* if (!lpm_entry_failed) */

  }  /* !(file_static_vars_ptr->is_in_low_power_mode) &&
     ** (!called_by_lpm_timer || file_static_vars_ptr->lpm_entry_is_pending))
     */

  HSU_MSG_HIGH("hsu_os_bus_enter_lpm: Exit.", 0, 0, 0);

  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  /* Register the HS-USB interrupt back. */
  hsu_os_bus_interrupt_init(core_index);
}  /* hsu_os_bus_enter_lpm */

#elif defined(T_QSC1100)

/************************************************/
/*                    ULC                       */
/************************************************/
void hsu_os_bus_enter_lpm
(
  uint32 core_index, 
  boolean called_by_lpm_timer,
  boolean is_analog_audio_mode
)
{
  usb_lpm_info_type* const core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  boolean phy_entered_lpm;

  HSU_USE_PARAM(is_analog_audio_mode);

  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  if (!file_static_vars_ptr->lpm_feature_is_active || 
      !core_lpm_info->lpm_is_active)
  {
    rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
    return;
  }

  HSU_MSG_HIGH("hsu_os_bus_enter_lpm: Entry.", 0, 0, 0);

  /* Un-register the HS-USB interrupt handler. */
  hsu_os_bus_interrupt_uninit(core_index);

  if (!(core_lpm_info->is_in_low_power_mode) && 
      (!called_by_lpm_timer || core_lpm_info->lpm_entry_is_pending))
  {
    /* Save the cable connection status. */
    core_lpm_info->cable_is_connected_during_lpm = 
      core_lpm_info->port_is_attached(core_index);
    
    HSU_MSG_HIGH("hsu_os_bus_enter_lpm: cable_is_connected_during_lpm = %d.", 
                 core_lpm_info->cable_is_connected_during_lpm,
                 0, 0);

    core_lpm_info->is_in_low_power_mode = TRUE;
    core_lpm_info->lpm_entry_is_pending = FALSE;

    phy_entered_lpm = hsu_os_bus_enter_pmic_phy_into_lpm();

    /* If the LPM entry has succeeded, perform the rest of the power-saving 
    ** steps.
    */
    if (phy_entered_lpm)
    {
      if (!core_lpm_info->cable_is_connected_during_lpm)
      {
        hsu_os_bus_update_target_after_lpm_entry(is_analog_audio_mode);
      }

      hsu_handle_sleep_vote(TRUE);
    }
    else  /* phy_entered_lpm == FALSE */
    {
      core_lpm_info->is_in_low_power_mode = FALSE;
    }  /* if (!lpm_entry_failed) */

  }  /* !(file_static_vars_ptr->is_in_low_power_mode) &&
     ** (!called_by_lpm_timer || file_static_vars_ptr->lpm_entry_is_pending))
     */

  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  /* Register the HS-USB interrupt back. */
  hsu_os_bus_interrupt_init(core_index);

  HSU_MSG_HIGH("hsu_os_bus_enter_lpm: Exit.", 0, 0, 0);

}  /* hsu_os_bus_enter_lpm */
#endif /* T_QSC1100 */
#endif  /* !(FEATURE_HSU_TEST) && !(FEATURE_HS_USB_BASIC) */


#if !defined(FEATURE_HSU_TEST) && !defined(FEATURE_HS_USB_BASIC)
/*===========================================================================
FUNCTION HSU_OS_BUS_EXIT_LPM

DESCRIPTION
  Exit High-Speed USB PHY from the low-power mode state.
  This function is invoked when the High-Speed USB bus is resumed from a
  suspend state, in order to return the High-Speed USB PHY back to an 
  operational state.
  This function negates the High-Speed USB sleep flag in the sleep task.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#if defined(T_MSM7625) || defined(T_MDM8200) || \
    defined(T_MSM8650) || defined(T_MDM8900)
/**********************************/
/* sRoc\Mustang\Raptor2\Skywalker */
/**********************************/
void hsu_os_bus_exit_lpm(uint32 core_index)
{
  boolean phy_exited_lpm;

  usb_lpm_info_type* const core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);


  #ifdef FEATURE_HS_USB_POLLING
  if(hsu_polling_is_active()) {return;}
  #endif /* FEATURE_HS_USB_POLLING */

  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  if (!file_static_vars_ptr->lpm_feature_is_active || 
      !core_lpm_info->lpm_is_active)
  {
    rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
    return;
  }

  HSU_MSG_HIGH("hsu_os_bus_exit_lpm: Entry.", 0, 0, 0);

  if (core_lpm_info->is_in_low_power_mode)
  {
    hsu_handle_sleep_vote(FALSE);

    if (!core_lpm_info->cable_is_connected_during_lpm)
    {
      hsu_os_bus_prepare_target_for_lpm_exit();
    }
    #if defined(T_MDM8200)
    else
    {
      if (file_static_vars_ptr->dcvs_active)
      {
        /* Disallow dynamic clock rate switching. */
        clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_HS_USB);
      }
    }
    #endif /* T_MDM8200 */

    /* Disable asynchronous interrupts from the PHY. */
    #ifdef FEATURE_HS_USB_HAL
    HAL_hsusb_ClearBits32(
      OS_BUS_TO_HAL_CORE_ID(core_index),
      HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_BMSK);

    #else /* FEATURE_HS_USB_HAL */

    HWIO_OUTF(USB_OTG_HS_USBCMD, ASYNC_INTR_CTRL, 0);

    #endif /* FEATURE_HS_USB_HAL */

    /* Enable the USB core to drive the ULPI STP signal.
    */
    #ifdef FEATURE_HS_USB_HAL

    HAL_hsusb_ClearBits32(
      OS_BUS_TO_HAL_CORE_ID(core_index),
      HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK); 

    #else /* FEATURE_HS_USB_HAL */

    HWIO_OUTF(USB_OTG_HS_USBCMD, ULPI_STP_CTRL, 0);

    #endif /* FEATURE_HS_USB_HAL */

    phy_exited_lpm = hsu_os_bus_exit_phy_from_lpm(core_index);

    if (phy_exited_lpm)
    {
      /* This flag MUST be updated as the last thing here, after the low-power
      ** mode exit is complete. Otherwise, an interrupt might access the HS-USB
      ** hardware too early.
      */
      core_lpm_info->is_in_low_power_mode = FALSE;
    }
    else
    {
      HSU_MSG_ERROR("hsu_os_bus_exit_lpm: Failed to take the PHY "
        "out from low-power mode.", 0, 0, 0);

      HSU_ASSERT(FALSE);
    }

  }  /* file_static_vars_ptr->is_in_low_power_mode */

  HSU_MSG_HIGH("hsu_os_bus_exit_lpm: Exit.", 0, 0, 0);

  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
}  /* hsu_os_bus_exit_lpm */

#elif defined(T_MSM7800) 

/**********************/
/*       Serra        */
/**********************/
void hsu_os_bus_exit_lpm(uint32 core_index)
{
  /* skip EXIT LPM for bring-up build */
}  /* hsu_os_bus_exit_lpm */

#elif defined(T_MSM7600) 

void hsu_os_bus_exit_lpm(uint32 core_index)
{
  boolean gpio_int_set_handler_succeeded;
  uint32  loop_count = 0;

  usb_lpm_info_type* const core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  #ifdef FEATURE_HS_USB_POLLING
  if(hsu_polling_is_active()) {return;}
  #endif /* FEATURE_HS_USB_POLLING */

  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  if (!file_static_vars_ptr->lpm_feature_is_active || 
      !core_lpm_info->lpm_is_active)
  {
    rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
    return;
  }

  HSU_MSG_HIGH("hsu_os_bus_exit_lpm: Entry.", 0, 0, 0);

  if (core_lpm_info->is_in_low_power_mode)
  {
    hsu_handle_sleep_vote(FALSE);
    HSU_MSG_HIGH("hsu_os_bus_exit_lpm: Voted against sleep.", 0, 0, 0);

    /* Unregister GPIO detect handler to avoid getting endless interrupts */
    gpio_int_set_handler_succeeded = 
      gpio_int_set_handler((gpio_int_type)GPIO_INT_114,
                           ACTIVE_HIGH, 
                           (gpio_int_handler_type)NULL);

    HSU_ASSERT(gpio_int_set_handler_succeeded);

    gpio_int_set_handler_succeeded = 
      gpio_int_set_handler((gpio_int_type)GPIO_INT_112,
                           ACTIVE_HIGH, 
                          (gpio_int_handler_type)NULL);

    HSU_ASSERT(gpio_int_set_handler_succeeded);

    if (!core_lpm_info->cable_is_connected_during_lpm)
    {
      hsu_os_bus_prepare_target_for_lpm_exit();
    }

    #ifdef FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND
    if (file_static_vars_ptr->usb_reset_rework_is_installed)
    {
      /* Disable workaround for USB_RESET waking TCXO shutdown */
      hsu_api_disable_pmic_ulpidata0_irq_handler();
    }
    #endif /* FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */

    /* Disable asynchronous interrupts from the PHY. */
    #ifdef FEATURE_HS_USB_HAL

    HAL_hsusb_ClearBits32(
      OS_BUS_TO_HAL_CORE_ID(core_index),
      HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_BMSK); 

    #else /* FEATURE_HS_USB_HAL */

    HWIO_OUTF(USB_OTG_HS_USBCMD, ASYNC_INTR_CTRL, 0);

    #endif /* FEATURE_HS_USB_HAL */


    /* Let the STP signal to get out from the core to the PHY.
    */
    #ifdef FEATURE_HS_USB_HAL

    HAL_hsusb_ClearBits32(
      OS_BUS_TO_HAL_CORE_ID(core_index),
      HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK); 

    #else /* FEATURE_HS_USB_HAL */

    HWIO_OUTF(USB_OTG_HS_USBCMD, ULPI_STP_CTRL, 0);

    #endif /* FEATURE_HS_USB_HAL */

    /* Set the PHCD bit in the port control register to zero. 
    ** This restarts the PHY clock and transfers the PHY into
    ** normal operating state.
    */
    #ifdef FEATURE_HS_USB_HAL

    HAL_hsusb_ClearBits32(
      OS_BUS_TO_HAL_CORE_ID(core_index),
      HAL_HSUSB_PORTSC_ADDR(0), HAL_HSUSB_PORTSC_PHCD_BMSK); 

    #else /* FEATURE_HS_USB_HAL */

    HWIO_OUTF(USB_OTG_HS_PORTSC, PHCD, 0);

    #endif /* FEATURE_HS_USB_HAL */

    /* Wait until the PHY finished its transition out 
    ** of low power mode.
    */
    #ifdef FEATURE_HS_USB_HAL

    while (HAL_hsusb_GetBits32(
      OS_BUS_TO_HAL_CORE_ID(core_index),
      HAL_HSUSB_PORTSC_ADDR(0), HAL_HSUSB_PORTSC_PHCD_BMSK) 
      != 0)

    #else /* FEATURE_HS_USB_HAL */

    while (HWIO_INF(USB_OTG_HS_PORTSC, PHCD) != 0)

    #endif /* FEATURE_HS_USB_HAL */
    {
      /* Busy-wait loop. */
      HSU_ASSERT(loop_count < HSU_MAX_PHCD_DWELL_TIME_ATTEMPT);
      loop_count++;
    }

    /* This flag MUST be updated as the last thing here, after the low-power
    ** mode exit is complete. Otherwise, an interrupt might access the HS-USB
    ** hardware too early.
    */
    core_lpm_info->is_in_low_power_mode = FALSE;

    #if defined(FEATURE_HS_USB_SMSC_PHY)
    hsu_os_bus_turn_on_smsc_phy_comparators();
    #endif  /* FEATURE_HS_USB_SMSC_PHY */

  }  /* file_static_vars_ptr->is_in_low_power_mode */

  HSU_MSG_HIGH("hsu_os_bus_exit_lpm: Exit.", 0, 0, 0);

  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
}  /* hsu_os_bus_exit_lpm */

#elif defined(T_MSM6246) || defined(T_QSC6270) || defined(T_MSM6290) 

/************************************************/
/*              Koel \ LCU \ Hawk               */
/************************************************/
void hsu_os_bus_exit_lpm(uint32 core_index)
{
  boolean  phy_exited_lpm;

  usb_lpm_info_type* const core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  #ifdef FEATURE_HS_USB_POLLING
  if(hsu_polling_is_active()) {return;}
  #endif /* FEATURE_HS_USB_POLLING */

  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  if (!file_static_vars_ptr->lpm_feature_is_active || 
      !core_lpm_info->lpm_is_active)
  {
    rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
    return;
  }

  HSU_MSG_HIGH("hsu_os_bus_exit_lpm: Entry.", 0, 0, 0);

  if (core_lpm_info->is_in_low_power_mode)
  {
    hsu_handle_sleep_vote(FALSE);
    HSU_MSG_HIGH("hsu_os_bus_exit_lpm: Voted against sleep.", 0, 0, 0);

    if (!core_lpm_info->cable_is_connected_during_lpm)
    {
      hsu_os_bus_prepare_target_for_lpm_exit();
    }

    /* Disable asynchronous interrupts from the PHY. */
    #ifdef FEATURE_HS_USB_HAL

    HAL_hsusb_ClearBits32(
      OS_BUS_TO_HAL_CORE_ID(core_index),
      HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_ASYNC_INTR_CTRL_BMSK); 

    #else /* FEATURE_HS_USB_HAL */

    HWIO_OUTF(USB_OTG_HS_USBCMD, ASYNC_INTR_CTRL, 0);

    #endif /* FEATURE_HS_USB_HAL */


    /* Let the STP signal to get out from the core to the PHY.
    */
    #ifdef FEATURE_HS_USB_HAL

    HAL_hsusb_ClearBits32(
      OS_BUS_TO_HAL_CORE_ID(core_index),
      HAL_HSUSB_USBCMD_ADDR, HAL_HSUSB_USBCMD_ULPI_STP_CTRL_BMSK); 

    #else /* FEATURE_HS_USB_HAL */

    HWIO_OUTF(USB_OTG_HS_USBCMD, ULPI_STP_CTRL, 0);

    #endif /* FEATURE_HS_USB_HAL */

    phy_exited_lpm = hsu_os_bus_exit_phy_from_lpm(core_index);

    if (phy_exited_lpm)
    {
      /* This flag MUST be updated as the last thing here, after the low-power
      ** mode exit is complete. Otherwise, an interrupt might access the HS-USB
      ** hardware too early.
      */
      core_lpm_info->is_in_low_power_mode = FALSE;
    }
    else
    {
      HSU_MSG_ERROR("hsu_os_bus_exit_lpm: Failed to take the PHY "
                    "out from low-power mode.", 0, 0, 0);

      HSU_ASSERT(FALSE);
    }

  }  /* file_static_vars_ptr->is_in_low_power_mode */

  HSU_MSG_HIGH("hsu_os_bus_exit_lpm: Exit.", 0, 0, 0);

  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
}  /* hsu_os_bus_exit_lpm */

#elif defined(T_QSC1100)

/************************************************/
/*                    ULC                       */
/************************************************/
void hsu_os_bus_exit_lpm(uint32 core_index)
{
  boolean  phy_exited_lpm;

  usb_lpm_info_type* const core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  #ifdef FEATURE_HS_USB_POLLING
  if(hsu_polling_is_active()) {return;}
  #endif /* FEATURE_HS_USB_POLLING */

  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  if (!file_static_vars_ptr->lpm_feature_is_active || 
      !core_lpm_info->lpm_is_active)
  {
    rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
    return;
  }

  HSU_MSG_HIGH("hsu_os_bus_exit_lpm: Entry.", 0, 0, 0);

  if (core_lpm_info->is_in_low_power_mode)
  {
    hsu_handle_sleep_vote(FALSE);
    HSU_MSG_HIGH("hsu_os_bus_exit_lpm: Voted against sleep.", 0, 0, 0);

    if (!core_lpm_info->cable_is_connected_during_lpm)
    {
      hsu_os_bus_prepare_target_for_lpm_exit();
    }

    phy_exited_lpm = hsu_os_bus_exit_pmic_phy_from_lpm();

    if (phy_exited_lpm)
    {
      /* This flag MUST be updated as the last thing here, after the low-power
      ** mode exit is complete. Otherwise, an interrupt might access the HS-USB
      ** hardware too early.
      */
      core_lpm_info->is_in_low_power_mode = FALSE;
    }
    else
    {
      HSU_MSG_ERROR("hsu_os_bus_exit_lpm: Failed to take the PHY "
                    "out from low-power mode.", 0, 0, 0);

      HSU_ASSERT(FALSE);
    }

  }  /* file_static_vars_ptr->is_in_low_power_mode */

  HSU_MSG_HIGH("hsu_os_bus_exit_lpm: Exit.", 0, 0, 0);

  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

}  /* hsu_os_bus_exit_lpm */
#endif /* T_QSC1100 */
#endif /* !(FEATURE_HSU_TEST) && !(FEATURE_HS_USB_BASIC) */
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */


#ifndef T_REXNT
#if (defined FEATURE_HS_USB_LEGACY_DEVICE) && (!defined FEATURE_HS_USB_BASIC)
/*===========================================================================
FUNCTION HSU_OS_BUS_FS_LEGACY_ENTER_LPM

DESCRIPTION
Enter FS legacy USB PHY and core into low-power mode state.
This function is invoked when no device is connected to the Full-Speed legacy 
USB bus, in order to save battery power.
After the PHY is entered into a low power-state, this function botes to assert
the High-Speed USB sleep flag in the sleep task.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_os_bus_fs_legacy_enter_lpm
(
 uint32 core_index, 
 boolean called_by_lpm_timer,
 boolean is_analog_audio_mode
)
{
  usb_lpm_info_type* const core_lpm_info = &(file_static_vars_ptr->
    core_table_ptr[core_index].usb_lpm_info);

  boolean phy_entered_lpm;

  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  if (!file_static_vars_ptr->lpm_feature_is_active || 
      !core_lpm_info->lpm_is_active)
  {
    rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
    return;
  }

  HSU_MSG_HIGH("hsu_os_bus_fs_legacy_enter_lpm: Entry.", 0, 0, 0);

  /* Un-register the HS-USB interrupt handler. */
  hsu_os_bus_interrupt_uninit(core_index);

  if (!(core_lpm_info->is_in_low_power_mode) && 
    (!called_by_lpm_timer || core_lpm_info->lpm_entry_is_pending))
  {
    /* Save the cable connection status. */
    core_lpm_info->cable_is_connected_during_lpm = 
      core_lpm_info->port_is_attached(core_index);

    HSU_MSG_HIGH("hsu_os_bus_fs_legacy_enter_lpm: cable_is_connected_during_lpm = %d.", 
      core_lpm_info->cable_is_connected_during_lpm,
      0, 0);

    core_lpm_info->is_in_low_power_mode = TRUE;
    core_lpm_info->lpm_entry_is_pending = FALSE;

    phy_entered_lpm = hsu_os_bus_enter_pmic_phy_into_lpm();

    /* If the LPM entry has succeeded, perform the rest of the power-saving 
    ** steps.
    */
    if (phy_entered_lpm)
    {
      if (!core_lpm_info->cable_is_connected_during_lpm)
      {
        /* Turn off the 48MHz clock */
        clk_regime_usb_xtal_off();
      }

      if (file_static_vars_ptr->dcvs_active)
      {
        /* Allow dynamic clock rate switching */
        hsu_handle_cpu_resources_registration(FALSE);
      }

      hsu_handle_sleep_vote(TRUE);
    }
    else  /* phy_entered_lpm == FALSE */
    {
      core_lpm_info->is_in_low_power_mode = FALSE;
    }  /* if (!lpm_entry_failed) */

  }  /* !(file_static_vars_ptr->is_in_low_power_mode) &&
     ** (!called_by_lpm_timer || file_static_vars_ptr->lpm_entry_is_pending))
     */

  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  /* Register the HS-USB interrupt back. */
  hsu_os_bus_interrupt_init(core_index);

  HSU_MSG_HIGH("hsu_os_bus_fs_legacy_enter_lpm: Exit.", 0, 0, 0);

}  /* hsu_os_bus_fs_legacy_enter_lpm */

/*===========================================================================
FUNCTION HSU_OS_BUS_FS_LEGACY_EXIT_LPM

DESCRIPTION
Exit FS Legacy USB PHY from the low-power mode state.
This function is invoked when the Full-Speed USB bus is connected, 
in order to return the Full-Speed USB PHY back to an 
operational state.
This function negates the High-Speed USB sleep flag in the sleep task.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
void hsu_os_bus_fs_legacy_exit_lpm(uint32 core_index)
{
  boolean  phy_exited_lpm;

  usb_lpm_info_type* const core_lpm_info = &(file_static_vars_ptr->
    core_table_ptr[core_index].usb_lpm_info);

  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  if (!file_static_vars_ptr->lpm_feature_is_active || 
      !core_lpm_info->lpm_is_active)
  {
    rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
    return;
  }

  HSU_MSG_HIGH("hsu_os_bus_fs_legacy_exit_lpm: Entry.", 0, 0, 0);

  if (core_lpm_info->is_in_low_power_mode)
  {
    hsu_handle_sleep_vote(FALSE);
    HSU_MSG_HIGH("hsu_os_bus_fs_legacy_exit_lpm: Voted against sleep.", 0, 0, 0);

    if (!core_lpm_info->cable_is_connected_during_lpm)
    {
      /* Turn on the 48MHz clock */
      clk_regime_usb_xtal_on();
    }

    phy_exited_lpm = hsu_os_bus_exit_pmic_phy_from_lpm();

    if (file_static_vars_ptr->dcvs_active)
    {
      /* Disallow dynamic clock rate switching. */
      hsu_handle_cpu_resources_registration(TRUE);
    }

    if (phy_exited_lpm)
    {
      /* This flag MUST be updated as the last thing here, after the low-power
      ** mode exit is complete. Otherwise, an interrupt might access the HS-USB
      ** hardware too early.
      */
      core_lpm_info->is_in_low_power_mode = FALSE;
    }
    else
    {
      HSU_MSG_ERROR("hsu_os_bus_fs_legacy_exit_lpm: Failed to take the PHY "
        "out from low-power mode.", 0, 0, 0);

      HSU_ASSERT(FALSE);
    }

  }  /* file_static_vars_ptr->is_in_low_power_mode */

  HSU_MSG_HIGH("hsu_os_bus_exit_lpm: Exit.", 0, 0, 0);

  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

}  /* hsu_os_bus_fs_legacy_exit_lpm */
#endif /* FEATURE_HS_USB_LEGACY_DEVICE && !FEATURE_HS_USB_BASIC */
#endif /* T_REXNT */


#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
#ifdef  FEATURE_HS_USB_UICC
/*===========================================================================
FUNCTION HSU_OS_BUS_UICC_ENTER_LPM

DESCRIPTION
  Enter USB UICC core into low-power mode state.
  This function is invoked when the USB UICC bus is suspended, in 
  order to save battery power.
  This function asserts the USB sleep flag in the sleep task
  (If all other cores are also in LPM state).
 
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_uicc_enter_lpm(
                                      uint32 core_index, 
                                      boolean called_by_lpm_timer,
                                      boolean is_analog_audio_mode
                                     )
{
  boolean lpm_entry_failed;
  usb_lpm_info_type* const core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);
  
  HSU_ASSERT(is_analog_audio_mode == FALSE);

  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  if (!file_static_vars_ptr->lpm_feature_is_active || 
      !core_lpm_info->lpm_is_active)
  {
    rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
    return;
  }

  /* Un-register the HS-USB interrupt handler. */
  hsu_os_bus_interrupt_uninit(core_index);

  HSU_MSG_HIGH("hsu_os_bus_uicc_enter_lpm: Entry.", 0, 0, 0);

  if (!(core_lpm_info->is_in_low_power_mode) && 
     (!called_by_lpm_timer || core_lpm_info->lpm_entry_is_pending))
  {
    core_lpm_info->is_in_low_power_mode = TRUE;
    core_lpm_info->lpm_entry_is_pending = FALSE;
    lpm_entry_failed = FALSE;

    /* Verify that an LPM entry prerequisite exists. */
    if (!core_ready_for_uicc_lpm())
    {
      lpm_entry_failed = TRUE;
    }

    /* If the LPM entry has succeeded, perform the rest of the power-saving 
    ** steps.
    */
    if (!lpm_entry_failed)
    {
      /* Turn off USB UICC core clocks. */
      if (file_static_vars_ptr->usbh_pclk_control_active)
      {
        #if (defined(T_MSM6246) || defined(T_MSM6290))
        clk_regime_disable (CLK_RGM_USB_OTG_2_M);
        clk_regime_disable (CLK_RGM_USB_MUSIM_M);
        #elif defined(T_QSC6270)
        clk_regime_disable (CLK_RGM_MEGASIM_FS_M);
        clk_regime_disable (CLK_RGM_MEGASIM_HCLK_M);
        #endif /* defined(T_MSM6246 || T_MSM6290 || T_QSC6270) */
        /* We do not need to keep AHB at constant sleep during LPM. */
        hsu_handle_ahb_const_registration(FALSE);
      }

      if (file_static_vars_ptr->dcvs_active)
      {
        /* Allow dynamic clock rate switching */
        hsu_handle_cpu_resources_registration(FALSE);
      }

      hsu_handle_sleep_vote(TRUE);
    }
    else  /* lpm_entry_failed == TRUE */
    {
      core_lpm_info->is_in_low_power_mode = FALSE;
    }  /* if (!lpm_entry_failed) */

  }  /* !(file_static_vars_ptr->is_in_low_power_mode) &&
     ** (!called_by_lpm_timer || file_static_vars_ptr->lpm_entry_is_pending))
     */

  HSU_MSG_HIGH("hsu_os_bus_uicc_enter_lpm: Exit.", 0, 0, 0);

  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  /* Register the UICC interrupt back. */
  hsu_os_bus_interrupt_init(core_index);
}  /* hsu_os_bus_uicc_enter_lpm */

/*===========================================================================
FUNCTION HSU_OS_BUS_UICC_EXIT_LPM

DESCRIPTION
  Exit USB UICC from low-power mode state.
  This function is invoked when the USB UICC bus is resumed from a
  suspend state.
  This function negates the USB sleep flag in the sleep task.

  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_uicc_exit_lpm(uint32 core_index)
{
  usb_lpm_info_type* const core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  if (!file_static_vars_ptr->lpm_feature_is_active || 
      !core_lpm_info->lpm_is_active)
  {
    rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
    return;
  }

  HSU_MSG_HIGH("hsu_os_bus_uicc_exit_lpm: Entry.", 0, 0, 0);

  if (core_lpm_info->is_in_low_power_mode)
  {
    hsu_handle_sleep_vote(FALSE);

    /* Turn on USB UICC core clocks. */
    if (file_static_vars_ptr->usbh_pclk_control_active)
    {
        #if (defined(T_MSM6246) || defined(T_MSM6290))
        clk_regime_enable (CLK_RGM_USB_OTG_2_M);
        clk_regime_enable (CLK_RGM_USB_MUSIM_M);
        #elif defined(T_QSC6270)
        clk_regime_enable (CLK_RGM_MEGASIM_FS_M);
        clk_regime_enable (CLK_RGM_MEGASIM_HCLK_M);
        #endif /* defined(T_MSM6246 || T_MSM6290 || T_QSC6270) */
        /* Register to keep AHB speed constant during ARM halt */
        hsu_handle_ahb_const_registration(TRUE);
    }

    if (file_static_vars_ptr->dcvs_active)
    {
      /* Disallow dynamic clock rate switching. */
      hsu_handle_cpu_resources_registration(TRUE);
    }

    core_lpm_info->is_in_low_power_mode = FALSE;
  }

  HSU_MSG_HIGH("hsu_os_bus_uicc_exit_lpm: Exit.", 0, 0, 0);

  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
}  /* hsu_os_bus_uicc_exit_lpm */

#endif  /* FEATURE_HS_USB_UICC */
#endif  /* !FEATURE_HS_USB_BASIC */
#endif  /* !T_REXNT */

#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================
FUNCTION HSU_OS_BUS_RESUME_PERIPHERALS

DESCRIPTION
  This function resumes all of the connected peripherals after remote wakeup
  was received (host mode only).

  This is a SW workaround for the following scenario:
  We're operating in host mode and suspend the bus. As a result of the 
  suspend we enter LPM and vote for sleep. TCXO shutdown occurs. One of 
  the connected peripherals issues remote wakeup. According to the standard 
  the peripheral should hold the remote wakeup signal 1-15 msec (see section 
  7.1.7.7 in USB2.0). This time interval isn’t enough for us to exit TCXO 
  shutdown and thus we "miss" the  remote wakeup signal.

  This function is called in the AL task context.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  If in host mode - All the connected peripherals are resumed.
===========================================================================*/
static void hsu_os_bus_resume_peripherals(void *param)
{
  HSU_USE_PARAM(param);
  /* If we're operating in host_mode this function could be called
  ** only due to remote_wakeup being issued by one of the connected 
  ** peripherals. In such case, resume all the connected peripherals
  */
  #ifdef FEATURE_HS_USB_OTG 
#error code not present
  #endif /*FEATURE_HS_USB_OTG*/
}/*hsu_os_bus_resume_peripherals*/

#ifndef T_REXNT
/*===========================================================================
FUNCTION HSU_OS_BUS_SPOOF_RECONNECT

DESCRIPTION
This function causes the device to re-connect to the host.
Specifically, this function issue a spoof disconnect, waits some 
time to ensure that the disconnection is detected by the host, and then issues 
a spoof connect.

Note - This function assumes the Giant mutex is locked upon calling it, and 
therefore should be called from the AL task context only.

DEPENDENCIES
None.

RETURN VALUE
TRUE - Operation successful. FALSE - Otherwise.

SIDE EFFECTS
None.
===========================================================================*/
static boolean hsu_os_bus_spoof_reconnect(void)
{
  static uint32 hsu_os_bus_spoof_reconnect_delay_ms = 500;
    /* The delay between the spoofed disconnect to the spoofed connect.
    ** Should be long enough to allow the host to distinguish the spoofed 
    ** disconnection.
    */


  if (!hsu_phy_utils_spoof_disconnect())
  {
    HSU_MSG_ERROR("hsu_os_bus_spoof_reconnect: "
      "Failed do spoof USB cable disconnection", 
      0, 0, 0);

    return FALSE;
  }

  os_delay_ms(hsu_os_bus_spoof_reconnect_delay_ms);

  if (!hsu_phy_utils_spoof_connect())
  {
    HSU_MSG_ERROR("hsu_os_bus_spoof_reconnect: "
      "Failed do spoof USB cable connection", 
      0, 0, 0);

    return FALSE;
  }

  return TRUE;
}  /* hsu_os_bus_spoof_reconnect */
#endif  /* T_REXNT */


#ifndef T_REXNT
#if !defined(FEATURE_HS_USB_NO_HS_OTG_CORE) && !defined(FEATURE_HS_USB_BASIC)
/*===========================================================================
FUNCTION HSU_OS_BUS_SPOOF_RECONNECT_AL_TASK_COMMAND

DESCRIPTION
  This function is a callback function executed by the AL task.
  This function checks whether a reset event has occurred, and if so,
  emulates a USB cable re-connection event. It is used when there is a 
  USB bus reset during USB suspend state and the HS-USB module is in LPM.

  This function is needed because sometimes it takes a long time for the phone 
  to wake up from TCXO shutdown state when the USB bus reset event occurs. As a 
  result, the USB chirp timing is violated and the USB bus runs at full-speed 
  mode. Therefore, this function is used to force a new attachment of the phone
  to the host, this time at High-Speed mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void 
hsu_os_bus_spoof_reconnect_al_task_command(void* command_param)
{
  static uint32 hsu_os_bus_usb_reset_bit_update_delay_ms = 2;

  uint32 core_index = (uint32)command_param;


  /* Let the USBCMD register USB reset bit stabilize. */
  os_delay_ms(hsu_os_bus_usb_reset_bit_update_delay_ms);

  /* Check whether a USB bus reset event has occurred. */
  if (HAL_hsusb_GetBits32(HS_USB_CORE_INDEX, 
                          HAL_HSUSB_USBSTS_ADDR, 
                          HAL_HSUSB_USBSTS_URI_BMSK) != 0)
  {
    /* Emulate a reconnection of the USB cable. */
    if (!hsu_os_bus_spoof_reconnect())
    {
      HSU_MSG_ERROR("hsu_os_bus_spoof_reconnect: "
                    "Failed do spoof USB cable connection",
                    0, 0, 0);
    }
    else
    {
      file_static_vars_ptr->hsu_os_bus_statistics_ptr->
        spoofed_reconnects_on_reset_during_suspend_count++;

      HSU_MSG_HIGH("hsu_os_bus_spoof_reconnect_al_task_command: "
                  "Successfully spoofed USB reconnection event.",
                  0, 0, 0);
    }
  }

  /* Register the HS-USB interrupt back. */
  hsu_os_bus_interrupt_init(core_index);

}  /* hsu_os_bus_spoof_reconnect_al_task_command */
#endif  /* !FEATURE_HS_USB_NO_HS_OTG_CORE && !FEATURE_HS_USB_BASIC */
#endif  /* !T_REXNT */


/*===========================================================================
FUNCTION HSU_OS_BUS_EXIT_LPM_BY_INTR

DESCRIPTION
  This function is queued to the DPC task when an HS-USB interrupt occurs 
  during low-power mode, and is responsible for low-power mode exiting by an
  HS-USB interrupt. 

  The low-power power mode exit procedure involves RPC calls, mutex lock\unlock,
  and it is too heavy to be performed from an interrupt context. Therefore, the
  interrupt context queues this function to the DPC task, and un-registers 
  itself from the Tramp. When this functions executes, it performs the low-power 
  mode exit procedure, and registers the HS-USB interrupt bask in the Tramp, 
  in order to let the rest of the HS-USB interrupt events to be handled.

DEPENDENCIES
  None.

RETURN VALUE
  None in L4 builds, TRUE in non-L4 builds.

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_L4
#error code not present
#else
static boolean hsu_os_bus_exit_lpm_by_intr(unsigned long core_index,
                                           unsigned long unused_param2,
                                           unsigned long unused_param3)
#endif
{
  usb_lpm_info_type* const core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  #ifndef FEATURE_L4
  HSU_USE_PARAM(unused_param2);
  HSU_USE_PARAM(unused_param3);
  #endif  /* FEATURE_L4 */

  if (core_lpm_info->exit_lpm_handler)
  {
    core_lpm_info->exit_lpm_handler(core_index);
  }

  /* If the spoof reconnect on reset during suspend feature
  ** is enabled, check whether the USB reset bit is set in the 
  ** USB status register. If this bit is set, a spoofed reconnection
  ** event is issued.
  **
  ** This feature fails the USBCV tests. Therefore, it is not 
  ** activated when is configured for running under USBCV. That
  ** means, when the Remote Wakeup Delay NV item (used only for NV tests)
  ** is non-zero.
  */

  #ifndef FEATURE_HS_USB_NO_HS_OTG_CORE

  if (core_index == HS_USB_CORE_INDEX)
  {
    if ( hsu_conf_sel_reconnect_on_reset_during_suspend() &&
        (hsu_conf_sel_get_rm_wakeup_delay() == 0) )
    {
      if (core_lpm_info->cable_is_connected_during_lpm)
      {
        hsu_al_task_enqueue_cmd(hsu_os_bus_spoof_reconnect_al_task_command, 
                                (void*)core_index);

        #ifdef FEATURE_L4
#error code not present
        #else
        return TRUE;
        #endif /* FEATURE_L4 */
      }
    }
  }

  #endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */

  /* Register the HS-USB interrupt back. */
  hsu_os_bus_interrupt_init(core_index);

  HSU_MSG_HIGH("hsu_os_bus_exit_lpm_by_intr: scheduling cable status check",
               0, 0, 0);

  /* Resume all the connected peripherals if we're in host mode  */
  (void)rex_set_timer(&(file_static_vars_ptr->
                        core_table_ptr[core_index].
                          resume_peripherals_timer),
                      HSU_OS_BUS_RESUME_PERIPHERALS_TIMEOUT);

  /* If the High-Speed USB was exited from low-power mode due to 
  ** an interrupt, schedule a USB cable status check in order to
  ** decide whether to return to low-power mode.
  */

  /* Schedule a USB cable status check. */
  (void)rex_set_timer(&(core_lpm_info->cable_status_check_timer),
                      HSU_OS_BUS_LPM_ENTRY_TIMEOUT);

  #ifndef FEATURE_L4
  return TRUE;
  #endif  /* !FEATURE_L4 */

}  /* hsu_os_bus_exit_lpm_by_intr */
#endif /* !FEATURE_HS_USB_BASIC */
#endif /* !T_REXNT */


#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================
FUNCTION HSU_OS_BUS_LPM_DISABLE

DESCRIPTION
  Disable the LPM feature. Exit from low-power mode if needed.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_lpm_disable(uint32 core_index)
{
  usb_lpm_info_type* const core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
  if (core_lpm_info->exit_lpm_handler)
  {
    core_lpm_info->exit_lpm_handler(core_index);
  }
  core_lpm_info->lpm_is_active = FALSE;

  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

}  /* hsu_os_bus_lpm_disable */
#endif /* !FEATURE_HS_USB_BASIC */
#endif /* !T_REXNT */


#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================
FUNCTION HSU_OS_BUS_LPM_ENABLE

DESCRIPTION
  Enable the low-power mode feature.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_lpm_enable(uint32 core_index)
{
  usb_lpm_info_type* core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);
  
  rex_enter_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));
  core_lpm_info->lpm_is_active = TRUE;
  rex_leave_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  /* Schedule a USB cable status check. */
  (void)rex_set_timer(&(core_lpm_info ->cable_status_check_timer),
                      HSU_OS_BUS_LPM_ENTRY_TIMEOUT);
}  /* hsu_os_bus_lpm_enable */
#endif /* !FEATURE_HS_USB_BASIC */
#endif /* !T_REXNT */

#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================
FUNCTION HSU_OS_BUS_LPM_ENTRY_TIMER_CB

DESCRIPTION
  Called when the Low Power Mode entry timer expires. 
  Enters the core into low power mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_lpm_entry_timer_cb(unsigned long core_index)
{
  usb_lpm_info_type* core_lpm_info;

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  HSU_MSG_HIGH("hsu_os_bus_lpm_entry_timer_cb: Entry", 0, 0, 0);

  if (core_lpm_info->enter_lpm_handler)
  {
    core_lpm_info->enter_lpm_handler(core_index, TRUE, FALSE);
  }

  HSU_MSG_HIGH("hsu_os_bus_lpm_entry_timer_cb: Exit", 0, 0, 0);
} /* hsu_os_bus_lpm_entry_timer_cb */
#endif /* !FEATURE_HS_USB_BASIC */
#endif /* !T_REXNT */


#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================
FUNCTION HSU_OS_BUS_RM_WAKEUP_COMMAND

DESCRIPTION
  Called when the HSU AL task is ready to handle remote wakeup.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_rm_wakeup_command(void* cmd_param)
{
  juint8_t controller_idx = (juint8_t)((uint32)cmd_param);

  HSU_ASSERT((uint32)cmd_param == (uint32)controller_idx);

  jsafe_enter();
  (void)(jslave_wakeup_host(controller_idx));
  jsafe_leave();

} /* hsu_os_bus_rm_wakeup_command */
#endif /* !FEATURE_HS_USB_BASIC */
#endif /* !T_REXNT */


#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================
FUNCTION HSU_OS_BUS_RM_WAKEUP_TIMER_CB

DESCRIPTION
  Called when the remote wakeup timer expires.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_rm_wakeup_timer_cb(unsigned long callback_param)
{
  HSU_MSG_HIGH("hsu_os_bus_rm_wakeup_timer_cb: Entry", 0, 0, 0);

  /* In case NV delay value is set to 0, no need to explicitly 
  ** issue a remote wakeup signal. 
  */
  if (hsu_conf_sel_get_rm_wakeup_delay() != 0)
  {
    /* jslave_wakeup_host might block, therefore, need to schedule it 
    ** in the AL task
    */
    hsu_al_task_enqueue_cmd(hsu_os_bus_rm_wakeup_command,
                            (void*)callback_param);
  }

  HSU_MSG_HIGH("hsu_os_bus_rm_wakeup_timer_cb: Exit", 0, 0, 0);

} /* hsu_os_bus_rm_wakeup_timer_cb */
#endif /* !FEATURE_HS_USB_BASIC */
#endif /* !T_REXNT */

/*===========================================================================
FUNCTION HSU_PORT_IS_ATTACHED

DESCRIPTION
  This function checks whether the High-Speed USB port is attached.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if the high-speed USB port is attached. Otherwise, returns FALSE.

SIDE EFFECTS
  None.
===========================================================================*/
#if !defined(FEATURE_HS_USB_PMIC_PHY) && !defined(FEATURE_HS_USB_LEGACY_DEVICE) 
boolean hsu_port_is_attached(uint32 core_index)
{
  boolean return_value;

  #ifndef T_REXNT

  usb_core_info_type* core_ptr = 
    &(file_static_vars_ptr->core_table_ptr[core_index]);
  
  #ifndef FEATURE_HS_USB_HAL

  resource_info_type* const resource_info = 
    &(core_ptr->controllers_list[0].resource_list[1].resource_instance);

  /* TODO - Remove these variables when the address of the OTGSC
  ** register is corrected in msmhwioreg.h.
  */
  uint32* const otgsc_register_ptr = 
    (uint32*)(resource_info->mem_resource_instance.start_address + 0x1A4);

  uint32* const portsc_register_ptr = 
    (uint32*)(resource_info->mem_resource_instance.start_address + 0x184);

  const uint32  b_session_valid_bit_mask = 0x800;
  #endif /* FEATURE_HS_USB_HAL */

  #ifndef FEATURE_HS_USB_BASIC

  usb_lpm_info_type* const core_lpm_info = &(core_ptr->usb_lpm_info);


  /* If we are in low-power mode, register access is denied. */
  if (core_lpm_info->is_in_low_power_mode)
  {
    /*
    ** Return the cable status recorded just before the LPM entry.
    */
    return core_lpm_info->cable_is_connected_during_lpm;
  }

  #endif /* !FEATURE_HS_USB_BASIC */
  
  /* Check whether the High-Speed USB port is attached to a host.
  ** This check is done using the B-Session Valid bit in the OTGSC
  ** register. This bit is set the High-Speed USB port is successfully
  ** attached.
  ** This test is applicable both for OTG mode and for device-only
  ** mode.
  */

  /* This line is a temporary replacement.
  */
  #ifdef FEATURE_HS_USB_HAL

  if ( (HAL_hsusb_GetBits32(
    OS_BUS_TO_HAL_CORE_ID(core_index),
    HAL_HSUSB_OTGSC_ADDR, HAL_HSUSB_OTGSC_BSV_BMSK) != 0) ||
       (HAL_hsusb_GetBits32(
    OS_BUS_TO_HAL_CORE_ID(core_index),
    HAL_HSUSB_PORTSC_ADDR(0), HAL_HSUSB_PORTSC_CCS_BMSK) != 0) )

  #else /* FEATURE_HS_USB_HAL */

  if ((((*otgsc_register_ptr) & b_session_valid_bit_mask) != 0) ||
      (((*portsc_register_ptr) & HWIO_USB_USB_OTG_HS_PORTSC_CCS_BMSK) == 1))

  #endif /* FEATURE_HS_USB_HAL */
  {
    return_value = TRUE;
  }
  else
  {
    return_value = FALSE;
  }

  HSU_USE_PARAM(core_ptr);

  #else   /* T_REXNT */
#error code not present
  #endif  /* T_REXNT */

  return return_value;

}  /* hsu_port_is_attached */

#else  /* FEATURE_HS_USB_PMIC_PHY || FEATURE_HS_USB_LEGACY_DEVICE */

/* When the USB core uses the PMIC PHY, the core's cable
** status is stored in a boolean variable (located in the 
** core table data) is maintained using PMIC events.
*/
boolean hsu_port_is_attached(uint32 core_index)
{
  usb_core_info_type* const core_ptr = 
    &(file_static_vars_ptr->core_table_ptr[core_index]);

  return core_ptr->cable_is_connected;

}  /* hsu_port_is_attached */
#endif  /* !FEATURE_HS_USB_PMIC_PHY && !FEATURE_HS_USB_LEGACY_DEVICE */

#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================
FUNCTION HSU_OS_BUS_CABLE_STATUS_CHECK_TIMER_CB

DESCRIPTION
  Called when High-Speed USB cable status check timer expires. 
  Checks whether the High-Speed USB port is attached.
  If it isn't, enters the core into low power mode.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_cable_status_check_timer_cb(uint32 core_index)
{
  usb_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  HSU_MSG_HIGH("hsu_os_bus_cable_status_check_timer_cb: Entry", 0, 0, 0);

  if (core_lpm_info->port_is_attached  &&
      !core_lpm_info->port_is_attached(core_index))
  {
    HSU_MSG_HIGH("hsu_os_bus_cable_status_check_timer_cb: entering LPM", 0, 0, 0);
    if (core_lpm_info->enter_lpm_handler)
    {
      core_lpm_info->enter_lpm_handler(core_index, FALSE, FALSE);
    }
  }

  HSU_MSG_HIGH("hsu_os_bus_cable_status_check_timer_cb: Exit", 0, 0, 0);
} /* hsu_os_bus_cable_status_check_timer_cb */

/*===========================================================================
FUNCTION HSU_OS_BUS_RESUME_PERIPHERALS_TIMER_CB

DESCRIPTION
This function is called when the resume_peripherals timer expires. 
This timer is armed when the core exits from low-power mode
due to an asynchronous interrupt. When this happens, if we are in host mode
resume all of the connected peripherals sequence is scheduled. 
This operation is delayed (via timer) in order to give the hs-usb0
task enough time to update the cable connection status. 
See documentation of hsu_os_bus_resume_peripherals() for more details.

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_os_bus_resume_peripherals_timer_cb(uint32 in_param)
{
  HSU_USE_PARAM(in_param);
  hsu_al_task_enqueue_cmd(hsu_os_bus_resume_peripherals, NULL);
}/*hsu_os_bus_resume_peripherals_timer_cb*/
#endif /* !FEATURE_HS_USB_BASIC */
#endif /* !T_REXNT */

/*===========================================================================
FUNCTION HSU_OS_ISR_WRAPPER_INT

DESCRIPTION
  A common code for a wrapper function that handles an USB interrupt.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_isr_wrapper_int
(
  uint32 core_index
)
{
  jint_t     isr_result;

  uint32     isr_index;

  usb_core_info_type* const 
    core_ptr = &(file_static_vars_ptr->core_table_ptr[core_index]);

  interrupt_resource_info_type** const
    isr_list_ptr = core_ptr->isr_list;

  interrupt_resource_info_type** isr_ptr;

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  for(isr_index = 0, isr_ptr = isr_list_ptr;
      isr_index < core_ptr->num_of_registered_isrs;
      isr_index++, isr_ptr++)
  {
    /* Calling interrupt handler from ISR context */
    HSU_ASSERT((*isr_ptr)->isr_func != NULL);
    isr_result = (*isr_ptr)->isr_func((*isr_ptr)->isr_arg);

    if (isr_result == ISR_CALL_SOFT_INTR)
    {
      HSU_ASSERT((*isr_ptr)->dsr_func != NULL);

      /* As an optimization, the DSR function is directly called
      ** from the interrupt context, since its current implementation allows it.
      ** (i.e., the DSR function does not call any API which is forbidden in the
      ** interrupt context).
      */
      (*isr_ptr)->dsr_func((*isr_ptr)->dsr_arg);
    }
  }

}  /* hsu_os_isr_wrapper_int */

/*===========================================================================
FUNCTION HSU_OS_HS_ISR

DESCRIPTION
  Interrupt wrapper function that handles an USB interrupt. 
  Calls all the registered interrupt service routines and
  their deferred service routines in the order of their registration (using the 
  os_interrupt_setup function).

  If the core is low-power mode, this function votes against 
  sleeping in the sleep task and brings the core back to an active state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_hs_isr
(
  uint32 core_index
)
{
  juint8_t  prev_ctx;

  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC
  usb_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */

  prev_ctx  = jcurr_ctx;
  jcurr_ctx = ISR_CONTEXT;

  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC
  /* Cancel any pending entry of the PHY into low-power mode. */
  core_lpm_info->lpm_entry_is_pending = FALSE;

  if (rex_get_timer(&(core_lpm_info->lpm_entry_timer)) > 0)
  {
    (void)rex_clr_timer(&(core_lpm_info->lpm_entry_timer));
    HSU_MSG_HIGH("HSU interrupt in LPM pending - timer canceled.", 0, 0, 0);
  }

  /* If the PHY is in low power mode, then we are in a PHY
  ** asynchronous interrupt. In this case, low power mode is exited
  ** and the interrupt handler returns.
  */
  if (core_lpm_info->is_in_low_power_mode)
  {
    HSU_MSG_HIGH("hsu_os_hs_isr: need to exit LPM", 0, 0, 0);

    #ifdef FEATURE_L4
#error code not present
    #else
    if (rex_queue_dpc(hsu_os_bus_exit_lpm_by_intr, core_index, 0, 0))
    #endif
    {
      HSU_MSG_HIGH("hsu_os_hs_isr: Queued an LPM exit in the DPC task.",
                   0, 0, 0);
    }
    else
    {
      HSU_MSG_ERROR("hsu_os_hs_isr: Failed to queue an LPM exit in "
                    "the DPC task.",
                    0, 0, 0);
    }

    /* Un-register the HS-USB interrupt from the Tramp subsystem.
    ** The DPC task will register it back when it completes the 
    ** low-power mode exit procedure. 
    */
    hsu_os_bus_interrupt_uninit(core_index);

    /* Restore the original context. */
    jcurr_ctx = prev_ctx;

    return;
  }
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */

  hsu_os_isr_wrapper_int(core_index);

  /* Restore the original context. */
  jcurr_ctx = prev_ctx;

}  /* hsu_os_hs_isr */

#if !defined (FEATURE_HS_USB_NO_HS_OTG_CORE) || defined (FEATURE_HS_USB_BASIC) 
/*===========================================================================
FUNCTION HSU_OS_HS_ISR_WRAPPER

DESCRIPTION
  Core 0 (High-Speed core) interrupt wrapper function that handles an 
  USB interrupt. 

DEPENDENCIES
None.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_os_hs_isr_wrapper
(
 void
)
{
  hsu_os_hs_isr(HS_USB_CORE_INDEX);
}
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE || FEATURE_HS_USB_BASIC */

#if ((defined(FEATURE_HS_USB_UICC) && !defined(FEATURE_HS_USB_BASIC)) || defined(T_REXNT)) 
/*===========================================================================
FUNCTION HSU_OS_UICC_ISR_WRAPPER

DESCRIPTION
  UICC Core interrupt wrapper function that handles a USB interrupt. 

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_uicc_isr_wrapper
(
 void
 )
{
  hsu_os_hs_isr(UICC_USB_CORE_INDEX);
}
#endif /* (FEATURE_HS_USB_UICC && FEATURE_HS_USB_BASIC) || T_REXNT */

#if (((defined(FEATURE_HS_USB_LEGACY_DEVICE) || defined(FEATURE_HS_USB_LEGACY_HOST)) \
    && !defined(FEATURE_HS_USB_BASIC)) || defined(T_REXNT)) 
/*===========================================================================
FUNCTION HSU_OS_FS_ISR_WRAPPER

DESCRIPTION
Core 1 (Full-Speed core) interrupt wrapper function that handles an 
USB interrupt. Calls all the registered interrupt service routines and
their deferred service routines in the order of their registration (using the 
os_interrupt_setup() function).

*** TODO - Add phone sleep mode support. The phone needs to be waked-up if 
the interrupt occurs during sleep mode.

*** TODO - Update this section. ***
If the High-Speed USB core is low-power mode, this function votes against 
sleeping in the sleep task and brings the core back to an active state.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_fs_isr_wrapper
(
  void
)
{
  hsu_os_hs_isr(FS_USB_CORE_INDEX);
}  /* hsu_os_fs_isr_wrapper */
#endif /* FEATURE_HS_USB_LEGACY_DEVICE ... */

#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
/*===========================================================================
FUNCTION HSU_TXCO_WAKEUP_GPIO_HANDLER

DESCRIPTION
  DEM (DYNAMIC ENVIRONMENT MANAGER) GPIO handler must be registred with DEM
  in order to exit TCXO shutdown and wake up the processor HS-USB is on when 
  the USB cabled is plugged in while TCXO is shutdown.
  
DEPENDENCIES
  This function should be used for ROC target only.
  Using GPIO 114 and 112 for waking up is valid for ROC target only.
  Other targets such as sROC, Serra, Raptor2 that define T_MSM7600 should 
  not use this function as-is.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#if !defined( FEATURE_HSU_TEST ) && !defined( FEATURE_HS_USB_BASIC )
#if defined(T_MSM7600)  && !defined(FEATURE_HS_USB_INTEGRATED_PHY)
#error code not present
#endif /* defined(T_MSM7600) && !defined(FEATURE_HS_USB_INTEGRATED_PHY) */
#endif /* FEATURE_HSU_TEST && FEATURE_HS_USB_BASIC */
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */

/*===========================================================================
FUNCTION OS_INTERRUPT_SETUP

DESCRIPTION
  Register interrupt service routines for a USB core interrupt.
  Supports chaining of multiple interrupt service routines for the
  same USB core interrupt.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns an error code.

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t os_interrupt_setup
(
  /* (IN) Interrupt resource handle previously provided by
  ** the 'os_bus_alloc_resource' function.
  */
  os_bus_resource_h res,

  /* (IN) User supplied interrupt handler. */
  interrupt_handler isr_func, 

  /* (IN) Argument to be passed to the ISR function. */
  void*             isr_arg,

  /* (IN) User supplied deferred service routine handler. */
  jthread_func      dsr_func,

  /* (IN) Argument to be passed to the DSR function. */
  void*             dsr_arg
)
{
  interrupt_resource_info_type* const
    interrupt_resource_info_ptr = (interrupt_resource_info_type*)res;

  usb_core_info_type*            core_info_ptr = NULL;

  uint32                         core_index;
  interrupt_resource_info_type** isr_list_ptr = NULL;
  uint32                         index_in_isr_list;

  jresult_t result;


  HSU_PARAM_COULD_BE_CONST(res);

  HSU_MSG_HIGH("os_interrupt_setup: Entry.", 0, 0, 0);

  HSU_ASSERT(res != NULL);
  HSU_ASSERT(isr_func != NULL);

  core_index = interrupt_resource_info_ptr->core_index;
  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  core_info_ptr = &(file_static_vars_ptr->core_table_ptr[core_index]);

  isr_list_ptr      = core_info_ptr->isr_list;
  index_in_isr_list = core_info_ptr->num_of_registered_isrs;

  HSU_ASSERT(index_in_isr_list < OS_BUS_MAX_ISRS_PER_INTERRUPT);
  HSU_ASSERT(isr_list_ptr[index_in_isr_list] == NULL);

  interrupt_resource_info_ptr->isr_func = isr_func;
  interrupt_resource_info_ptr->isr_arg  = isr_arg;
  interrupt_resource_info_ptr->dsr_func = dsr_func;
  interrupt_resource_info_ptr->dsr_arg  = dsr_arg;

  #ifdef FEATURE_HS_USB_PMIC_PHY
  interrupt_resource_info_ptr->event  = NULL;
  #endif  /* FEATURE_HS_USB_PMIC_PHY */

  isr_list_ptr[index_in_isr_list] = interrupt_resource_info_ptr;

  core_info_ptr->num_of_registered_isrs++;

  /* If this is the first ISR registered for this USB core's 
  ** interrupt, then also hook the USB core's interrupt
  ** to the appropriate interrupt wrapper function in the AMSS 
  ** TRAMP subsystem. 
  */
  if (core_info_ptr->num_of_registered_isrs == 1)
  {
    result = hsu_os_bus_interrupt_init(core_index);
    if (result != HSU_JSUCCESS)
    {
      HSU_ASSERT((uint32)"Failed to initialize the interrupts." == NULL);
      HSU_MSG_ERROR("os_interrupt_setup: Failed to initialize the interrupts. Result: %d",
                    result, 0, 0);

      /* Roll-back changes. */
      core_info_ptr->num_of_registered_isrs--;

      isr_list_ptr[index_in_isr_list] = NULL;

      interrupt_resource_info_ptr->isr_func = NULL;
      interrupt_resource_info_ptr->isr_arg  = NULL;
      interrupt_resource_info_ptr->dsr_func = NULL;
      interrupt_resource_info_ptr->dsr_arg  = NULL;

      return result;
    }
  }

  HSU_MSG_HIGH("os_interrupt_setup: Exit.", 0, 0, 0);

  return HSU_JSUCCESS;

} /* os_interrupt_setup */


/*===========================================================================
FUNCTION OS_INTERRUPT_TEARDOWN

DESCRIPTION
  Remove an interrupt handler.

DEPENDENCIES
  Assume here that the unregistered interrupt handler is the last one that
  was registered (using 'os_interrupt_setup').

RETURN VALUE
  Returns 0 on success, otherwise returns an error code.

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t os_interrupt_teardown
(
  /* (IN) The interrupt resource handle 
  ** (provided by the 'os_bus_alloc_resource' function).
  */
  os_bus_resource_h res
)
{
  interrupt_resource_info_type* const
    interrupt_resource_info_ptr = (interrupt_resource_info_type*)res;

  usb_core_info_type*             core_info_ptr = NULL;

  uint32                          core_index;
  interrupt_resource_info_type**  isr_list_ptr;
  uint32                          num_of_registered_isrs;

  jresult_t result;


  HSU_PARAM_COULD_BE_CONST(res);

  HSU_MSG_HIGH("os_interrupt_teardown: Entry.", 0, 0, 0);

  HSU_ASSERT(res != NULL);

  core_index = interrupt_resource_info_ptr->core_index;
  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  core_info_ptr          = &(file_static_vars_ptr->core_table_ptr[core_index]);

  isr_list_ptr           = core_info_ptr->isr_list;
  num_of_registered_isrs = core_info_ptr->num_of_registered_isrs;

  /* Assert that the unregistered interrupt handler is the 
  ** last one that was registered.
  */
  HSU_ASSERT(isr_list_ptr[num_of_registered_isrs - 1] == 
             interrupt_resource_info_ptr);

  isr_list_ptr[num_of_registered_isrs - 1] = NULL;

  interrupt_resource_info_ptr->isr_func = NULL;
  interrupt_resource_info_ptr->isr_arg  = NULL;
  interrupt_resource_info_ptr->dsr_func = NULL;
  interrupt_resource_info_ptr->dsr_arg  = NULL;

  core_info_ptr->num_of_registered_isrs--;

  /* If there are no more ISRs registered for this USB core
  ** interrupt, then also un-hook the USB core interrupt
  ** from its interrupt wrapper function in the AMSS TRAMP
  ** subsystem.
  */
  if (core_info_ptr->num_of_registered_isrs == 0)
  {
    result = hsu_os_bus_interrupt_uninit(core_index);
    if (result != HSU_JSUCCESS)
    {
      HSU_ASSERT((uint32)"Failed to un-initialize the interrupts." == NULL);
      HSU_MSG_ERROR("os_interrupt_teardown: Failed to un-initialize the interrupts. Result: %d",
                    result, 0, 0);

      return result;
    }
  }

  HSU_MSG_HIGH("os_interrupt_teardown: Exit.", 0, 0, 0);

  return HSU_JSUCCESS;

} /* os_interrupt_teardown */


#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================
FUNCTION HSU_OS_BUS_SCHEDULE_LPM_ENTRY

DESCRIPTION
  Schedule an LPM entry

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_schedule_lpm_entry(uint32 core_index)
{
  usb_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);


  HSU_MSG_HIGH("hsu_os_bus_schedule_lpm_entry: Entry.", 0, 0, 0);

  /* Arm the low power mode entry timer. */
  (void)rex_set_timer(&(core_lpm_info->lpm_entry_timer),
                      HSU_OS_BUS_LPM_ENTRY_TIMEOUT);

  core_lpm_info->lpm_entry_is_pending = TRUE;

  HSU_MSG_HIGH("hsu_os_bus_schedule_lpm_entry: Exit.", 0, 0, 0);

}  /* hsu_os_bus_schedule_lpm_entry */
#endif  /* FEATURE_HS_USB_BASIC */
#endif  /* T_REXNT */


#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC
/*===========================================================================
FUNCTION HSU_OS_BUS_CANCEL_SCHEDULED_LPM_ENTRY

DESCRIPTION
  Cancel any scheduled LPM entry, if exists.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_cancel_scheduled_lpm_entry(uint32 core_index)
{
  usb_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);


  HSU_MSG_HIGH("hsu_os_bus_cancel_scheduled_lpm_entry: Entry.", 0, 0, 0);

  /* Cancel any pending entry of the Phy into low-power mode. */
  core_lpm_info->lpm_entry_is_pending = FALSE;
  (void)rex_clr_timer(&(core_lpm_info->lpm_entry_timer));

  HSU_MSG_HIGH("hsu_os_bus_cancel_scheduled_lpm_entry: Exit.", 0, 0, 0);

}  /* hsu_os_bus_cancel_scheduled_lpm_entry */
#endif  /* FEATURE_HS_USB_BASIC */
#endif  /* T_REXNT */


/*===========================================================================
FUNCTION HSU_OS_BUS_SUSPEND

DESCRIPTION
  This function is called when the High-Speed USB
  bus is suspended.

  This function enters the High-Speed USB Phy into low-power mode.
  The actual entry into the low power mode state is delayed using
  a timer in order to enable its cancellation if the bus is resumed
  shortly after returning from this callback.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes from 
  jerrno.h.

SIDE EFFECTS
  None.
===========================================================================*/
static jresult_t hsu_os_bus_suspend(uint32 core_index, boolean is_in_device_mode)
{
  #if (!defined FEATURE_HSU_TEST) && (!defined FEATURE_HS_USB_BASIC)
  uint16  dev_event_payload; /*help variable for event_report */    

  /* This variable indicates the number of milliseconds to wait before 
     performing explicit remote wakeup when we are in suspend mode. 
     If its value is set to 0 the device doesn't perform explicit remote wakeup.
  */
  uint32 explicit_remote_wakeup_delay = hsu_conf_sel_get_rm_wakeup_delay();
  #endif /*FEATURE_HSU_TEST & FEATURE_HS_USB_BASIC*/

  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC
  usb_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);
  #endif  /* FEATURE_HS_USB_BASIC */
  #endif  /* T_REXNT */

  if (core_index >= NUMBER_OF_CORES)
  {
    HSU_MSG_ERROR("hsu_os_bus_suspend: Invalid core index parameter [%d].",
      core_index, 0, 0);
    return JEINVAL;
  }
  
  HSU_MSG_HIGH("hsu_os_bus_suspend() - Entry.", 0, 0, 0);
 
  if (is_in_device_mode)
  {
    HSU_MSG_HIGH("hsu_os_bus_suspend() - Device mode.",
                 0, 0, 0);
  }
  else
  {
    HSU_MSG_HIGH("hsu_os_bus_suspend() - Host mode.",
                 0, 0, 0);
  }

  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC

  /* Cancel any pending cable status check. */
  (void)rex_clr_timer(&(core_lpm_info->cable_status_check_timer));
  HSU_MSG_HIGH("hsu_os_bus_suspend: Cable status check timer canceled.",
               0, 0, 0);

  /* Arm the low power mode entry timer. */
  HSU_MSG_HIGH("hsu_os_bus_suspend: scheduling LPM entry.", 0, 0, 0);
  hsu_os_bus_schedule_lpm_entry(core_index);

  if (is_in_device_mode)
  {

#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE

    if (core_index == HS_USB_CORE_INDEX)
    {
      #ifdef FEATURE_HS_USB_CHARGING
      /* If we are in device mode, notify charger device is suspended */
      hsu_chg_api_helper_suspended();
      #endif /* FEATURE_HS_USB_CHARGING */ 

      /* If we are in device mode: arm the remote wakeup timer. */
      if (explicit_remote_wakeup_delay > 0)
      {
        HSU_MSG_HIGH("hsu_os_bus_suspend: scheduling remote wakeup in %dms",
                    explicit_remote_wakeup_delay, 0, 0);

        timer_set(&(core_lpm_info->remote_wakeup_timer),
                  explicit_remote_wakeup_delay,
                  0,
                  T_MSEC);
      }
      else
      {
        /* Use the timer for delaying remote wakeup to at least 
        ** OS_BUS_POSTPONE_REMOTE_WAKEUP_MS milliseconds after suspend.
        */
        timer_set(&(core_lpm_info->remote_wakeup_timer),
                  OS_BUS_POSTPONE_REMOTE_WAKEUP_MS,
                  0,
                  T_MSEC);
      }
    }  /* if (core_index == HS_USB_CORE_INDEX) */

#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */

    /* Send a notification event */
    dev_event_payload = (uint16)HS_USB_PERIPHERAL_STACK;
    event_report_payload(EVENT_HS_USB_STACK_SUSPENDED ,
                         sizeof(uint16),
                         (void*)(&dev_event_payload) );

  }  
  else  /* if (is_in_device_mode) */
  {
    dev_event_payload = (uint16)HS_USB_HOST_STACK;
    event_report_payload(EVENT_HS_USB_STACK_SUSPENDED ,
                         sizeof(uint16),
                         (void*)(&dev_event_payload));

  }  /* if (is_in_device_mode) */
  
  #endif  /* FEATURE_HS_USB_BASIC */
  #endif  /* T_REXNT */

  hsu_bus_mode_changed(core_index, HSU_BUS_MODE_SUSPENDED);

  HSU_MSG_HIGH("hsu_os_bus_suspend() - Exit.", 0, 0, 0);
  
  return HSU_JSUCCESS;

} /* hsu_os_bus_suspend */

/*===========================================================================
FUNCTION HSU_OS_BUS_RESUME_COMPLETED

DESCRIPTION
  This function is called when the High-Speed USB bus received a resume 
  notification after it was suspended. The resume sequence is over.
  (The core is resumed)

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes from
  jerrno.h.

SIDE EFFECTS
  None.
===========================================================================*/

static void hsu_os_bus_resume_completed(uint32 core_index, 
                                        boolean is_in_device_mode)
{
  #if (!defined FEATURE_HSU_TEST) && (!defined FEATURE_HS_USB_BASIC)
  uint16  dev_event_payload; /*help variable for event_report */
  #else
  HSU_USE_PARAM(is_in_device_mode);
  #endif  /* !FEATURE_HSU_TEST && !FEATURE_HS_USB_BASIC */


  #if (!defined FEATURE_HSU_TEST) && (!defined FEATURE_HS_USB_BASIC)
  /* Send a notification event */
  if (is_in_device_mode)
  {
    dev_event_payload = (uint16)HS_USB_PERIPHERAL_STACK;
    event_report_payload(EVENT_HS_USB_STACK_RESUMED ,
      sizeof(uint16),(void*)(&dev_event_payload) );
  }
  else /*is_in_device_mode*/
  {
    dev_event_payload = (uint16)HS_USB_HOST_STACK;
    event_report_payload(EVENT_HS_USB_STACK_RESUMED ,
      sizeof(uint16),(void*)(&dev_event_payload));
  }
  #endif /*FEATURE_HSU_TEST & FEATURE_HS_USB_BASIC*/

  hsu_bus_mode_changed(core_index, HSU_BUS_MODE_RESUMED);
}/*hsu_os_bus_resume_completed*/

/*===========================================================================
FUNCTION HSU_OS_BUS_RESUME

DESCRIPTION
  This function is called when the High-Speed USB bus received a resume 
  notification after it was suspended. The resume sequence is not over yet!
  (The core is still suspended)

  Cancel any pending entry of the PHY into low-power mode.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes from
  jerrno.h.

SIDE EFFECTS
  None.
===========================================================================*/
static jresult_t hsu_os_bus_resume(uint32 core_index, boolean is_in_device_mode)
{
  #ifdef T_REXNT
#error code not present
  #else
  #ifndef FEATURE_HS_USB_BASIC
  usb_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);
  HSU_USE_PARAM(core_lpm_info);
  #endif  /* FEATURE_HS_USB_BASIC */
  #endif  /* T_REXNT */

  HSU_MSG_HIGH("hsu_os_bus_resume() - Entry.", 0, 0, 0);

  if (is_in_device_mode)
  {
    HSU_MSG_HIGH("hsu_os_bus_resume() - Device mode.",
                 0, 0, 0);
  }
  else
  {
    HSU_MSG_HIGH("hsu_os_bus_resume() - Host mode.",
                 0, 0, 0);
  }

  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC

  /* Cancel any pending entry of the Phy into low-power mode. */
  hsu_os_bus_cancel_scheduled_lpm_entry(core_index);
  HSU_MSG_HIGH("hsu_os_bus_resume: LPM entry was canceled.", 0, 0, 0);

  /* Cancel any pending cable status check. */

  /* TEMP FIX - Sometimes there are false resume events when the phone starts
  ** unconnected. This prevents the LPM entry. The workaround is not to cancel 
  ** the cable status check on resume events.
  ** This is a temp fix until issue B68042 in the Jungo support center
  ** is resolved.
  **
  ** (void)rex_clr_timer(&(core_lpm_info->cable_status_check_timer));
  ** HSU_MSG_HIGH("hsu_os_bus_resume: Cable status check timer canceled.",
  **              0, 0, 0);
  **
  ** TEMP FIX END !!!
  */

  if (is_in_device_mode)
  {

#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE

    if (core_index == HS_USB_CORE_INDEX)
    {
      /* Cancel any pending remote wakeup. */
      HSU_MSG_HIGH("hsu_os_bus_resume: un-scheduling remote wakeup", 0, 0, 0);
      (void)timer_clr(&(core_lpm_info->remote_wakeup_timer), 
                      T_NONE);

      #ifdef FEATURE_HS_USB_CHARGING
      /* Notify charger that host issued a resume */
      hsu_chg_api_helper_resumed();
      #endif /* FEATURE_HS_USB_CHARGING */
    }

#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */

  }

  #endif  /* FEATURE_HS_USB_BASIC */
  #endif  /* T_REXNT */
 
  return HSU_JSUCCESS;

} /* hsu_os_bus_resume */


/*===========================================================================
FUNCTION HSU_OS_BUS_CONNECT

DESCRIPTION
This function is called when the High-Speed USB bus received a connect 
notification. 

DEPENDENCIES
The connect event is received only when the stack is configured to
work in non-OTG mode.

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/
static void hsu_os_bus_connect(uint32 core_index)
{
  HSU_MSG_HIGH("hsu_os_bus_connect() - Entry.", 0, 0, 0);

#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE

  if (core_index == HS_USB_CORE_INDEX)
  {
    #ifndef T_REXNT
    #if !defined(FEATURE_HS_USB_BASIC) && defined(FEATURE_HS_USB_CHARGING)
    hsu_conf_sel_stack_utils_handle_chg_connected();
    #endif /* !FEATURE_HS_USB_BASIC & FEATURE_HS_USB_CHARGING */ 
    #endif /* T_REXNT */
  }

#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */

  /* Only applicable for stack configured as device only */
  hsu_bus_mode_changed(core_index, HSU_BUS_MODE_CONNECTED);
  HSU_MSG_HIGH("hsu_os_bus_connect() - Exit.", 0, 0, 0);

}  /* hsu_os_bus_connect */


/*===========================================================================
FUNCTION HSU_OS_BUS_DISCONNECT

DESCRIPTION
  This function is called when the High-Speed USB bus received a disconnect 
  notification after it was suspended.

DEPENDENCIES
  The disconnect event is received only when the stack is configured to
  work in non-OTG mode.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_disconnect(uint32 core_index)
{
  #ifndef FEATURE_HS_USB_BASIC
  #ifndef T_REXNT

  usb_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);

  HSU_MSG_HIGH("hsu_os_bus_disconnect() - Entry.", 0, 0, 0);

  /* Cancel any pending cable status check. */
  (void)rex_clr_timer(&(core_lpm_info->cable_status_check_timer));
  HSU_MSG_HIGH("hsu_os_bus_disconnect() - Cable status check timer"
                " was canceled.",
                0, 0, 0);

  /* Arm the low power mode entry timer. */
  HSU_MSG_HIGH("hsu_os_bus_disconnect() - scheduling LPM entry",
                0, 0, 0);

  hsu_os_bus_schedule_lpm_entry(core_index);

  #ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  #ifdef FEATURE_HS_USB_CHARGING
  if (core_index == HS_USB_CORE_INDEX)
  {
    hsu_conf_sel_stack_utils_handle_chg_disconnected();
  }
  #endif /* FEATURE_HS_USB_CHARGING */ 
  #endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */
  #endif /* T_REXNT */

  #ifdef FEATURE_HS_USB_CORE_RESET_ON_DISC
  if (file_static_vars_ptr->core_table_ptr[core_index].controller_info_ptr)
  {
    void* const core_index_ptr = 
      (void *)&(file_static_vars_ptr->core_table_ptr[core_index].controller_info_ptr->
      resource_list[0].resource_instance.mem_resource_instance.core_index);
    hsu_al_task_enqueue_cmd(hsu_conf_sel_stack_utils_device_restart_core, core_index_ptr);
  }
  #endif /* FEATURE_HS_USB_CORE_RESET_ON_DISC */
  
  #endif  /* !FEATURE_HS_USB_BASIC */

  /* Only applicable for stack configured as device only */
  hsu_bus_mode_changed(core_index, HSU_BUS_MODE_DISCONNECTED);
  HSU_MSG_HIGH("hsu_os_bus_disconnect() - Exit.", 0, 0, 0);

} /* hsu_os_bus_disconnect */


/*===========================================================================
FUNCTION OS_NOTIFY

DESCRIPTION
  This function is called by the High-Speed USB core controller
  driver for notifying about bus events.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes from
  jerrno.h.

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t os_notify(jevent_t event, void * param)
{
  uint32     core_index = OS_BUS_FROM_HAL_CORE_ID((uint32)param);
  jresult_t  return_value;

  #ifndef T_REXNT
  usb_lpm_info_type* core_lpm_info = 
      &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);
  #endif /* T_REXNT */

#if !defined(FEATURE_HS_USB_BASIC) && defined(FEATURE_HS_USB_POLLING)
  if(hsu_polling_is_active()) {return 0;}
#endif /* FEATURE_HS_USB_POLLING */

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  HSU_MSG_HIGH("os_notify: Entry.", 0, 0, 0);
  HSU_MSG_HIGH("os_notify: event=%d, core_index=%d", event, core_index, 0);

  /* 
  ** For now, power management is not supported for 
  ** the Full-Speed core. 
  ** TODO - Update this function when Full-Speed core power management 
  ** is implemented. Remember to take into account that the second core
  ** may be also a High-Speed core (use the 'core type' field).
  */

  switch (event)
  {
  case NOTIFY_DEVICE_SUSPEND:
    return_value = hsu_os_bus_suspend(core_index, TRUE);
    break;

  #ifdef CONFIG_JHOST 
  case NOTIFY_HOST_SUSPEND:
    return_value = hsu_os_bus_suspend(core_index, FALSE);
    break;
  #endif  /* CONFIG_JHOST */
    
  case NOTIFY_DEVICE_RESUME:
    return_value = hsu_os_bus_resume(core_index, TRUE);
    break;

  case NOTIFY_DEVICE_RESUME_COMPLETED:
    hsu_os_bus_resume_completed(core_index, TRUE);
    return_value = HSU_JSUCCESS;
    break;

  #ifdef CONFIG_JHOST
  case NOTIFY_HOST_RESUME:
    /* hsu_os_bus_resume() should be invoked before the call to exit_lpm
    ** in order to make sure that any pending LPM enrty will be cancelled.
    ** Otherwise, the LPM entry timer can expire after exit LPM begins, 
    ** resulting in entering LPM after exit LPM finishes. This is illegal 
    ** because host mode applications assume we are not in LPM after 
    ** requesting resume from the USB stack.
    */
    return_value = hsu_os_bus_resume(core_index, FALSE);

    #ifndef T_REXNT
    if (core_lpm_info->exit_lpm_handler)
    {
      core_lpm_info->exit_lpm_handler(core_index);
    }
    #endif /*T_REXNT*/

    /* In host mode RESUME_COMPLETED is missing so we need to 
    ** call hsu_os_bus_resume_completed in order to notify the applications
    */
    hsu_os_bus_resume_completed(core_index, FALSE);

    break;
  #endif  /* CONFIG_JHOST */

  case NOTIFY_DEVICE_REMOTE_WAKEUP:

    #ifndef FEATURE_HSU_TEST

    if ((core_lpm_info->port_is_attached != NULL) &&
        !core_lpm_info->port_is_attached(core_index))
    {
      HSU_MSG_HIGH("os_notify(REMOTE_WAKEUP): Port is not attached. "
                   "Denying remote wakeup.",
                    0, 0, 0);

      return_value = JEPERM;
    }
    else
    {
      #if !defined(FEATURE_HS_USB_BASIC)
      timetick_type remaining_time;
      #endif /* !FEATURE_HS_USB_BASIC */
      
      HSU_MSG_HIGH("os_notify(REMOTE_WAKEUP): Port is attached. "
                   "Allowing remote wakeup.",
                    0, 0, 0);

      return_value = HSU_JSUCCESS;

      #if !defined(FEATURE_HS_USB_BASIC)

      /* Cancel any pending LPM entry. */
      hsu_os_bus_cancel_scheduled_lpm_entry(core_index);
      HSU_MSG_HIGH("os_notify(REMOTE_WAKEUP) - LPM entry was canceled.",
                   0, 0, 0);

      core_lpm_info->exit_lpm_handler(core_index);

      /* prevent remote wakeup to be issued too soon. Insure that at least 30ms
      ** have passed from suspend (spec defines 5ms).
      */
      remaining_time = 
        timer_get(&(core_lpm_info->remote_wakeup_timer),
                  T_MSEC);

      if (remaining_time > 0)
      {
        HSU_MSG_HIGH("Postponing remote wakeup for %dms.", 
                     remaining_time, 0, 0);

        rex_sleep(remaining_time);
      }
      #endif /* !FEATURE_HS_USB_BASIC */

      HSU_MSG_HIGH("os_notify: Allowing remote wakeup.", 0, 0, 0);
      return_value = HSU_JSUCCESS;
    }

    #else  /* FEATURE_HSU_TEST */
#error code not present
    #endif /* FEATURE_HSU_TEST */

    break;

  #ifdef CONFIG_JHOST
  case NOTIFY_OTG_BUSY:
    return_value = hsu_os_bus_resume(core_index, FALSE);
    /*In host mode RESUME_COMPLETED is missing so we need to 
    call hsu_os_bus_resume_completed in order to notify the applications*/
    hsu_os_bus_resume_completed(core_index, FALSE);
    break;

  case NOTIFY_OTG_IDLE:
    return_value = hsu_os_bus_suspend(core_index, FALSE);
    break;

  #ifdef FEATURE_HS_USB_UICC
  case NOTIFY_HOST_CONNECT:
    if (file_static_vars_ptr->core_table_ptr[core_index].core_type 
      == OS_BUS_USB_UICC_CORE)
    {
      hsu_uicc_usb_conn_detected();
    }
    return_value = TRUE;
    break;
  #endif /* FEATURE_HS_USB_UICC */

  #endif  /* CONFIG_JHOST */

  case NOTIFY_DEVICE_CONFIGURED:
    hsu_bus_mode_changed(core_index, HSU_BUS_MODE_CONFIGURED);
    return_value = HSU_JSUCCESS;
    break;

  case NOTIFY_DEVICE_UNCONFIGURED:
    hsu_bus_mode_changed(core_index, HSU_BUS_MODE_UNCONFIGURED);
    return_value = HSU_JSUCCESS;
    break;

  case NOTIFY_DEVICE_CONNECT:

    hsu_os_bus_connect(core_index);
    return_value = HSU_JSUCCESS;
    break;

  case NOTIFY_DEVICE_DISCONNECT:

    hsu_os_bus_disconnect(core_index);
    return_value = HSU_JSUCCESS;
    break;

  case NOTIFY_DEVICE_RESET:
    HSU_MSG_LOW("os_notify: NOTIFY_DEVICE_RESET", 0, 0, 0);
    return_value = HSU_JSUCCESS;
    break;

  default:
    return_value = HSU_JSUCCESS;
    break;
  }

  HSU_MSG_HIGH("os_notify: Exit.", 0, 0, 0);
  return return_value;
}  /* os_notify */


#ifndef T_REXNT
#ifndef FEATURE_HS_USB_BASIC  
/*===========================================================================
FUNCTION HSU_OS_NOTIFY_STACK_READY

DESCRIPTION
  Tell the OS abstraction layer that the USB stack is fully initialized.
  This notification is used for suspending the USB bus and for power management.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_notify_stack_ready(void)
{
  /* Schedule a USB cable status check. */
  int i;
  usb_lpm_info_type* core_lpm_info;

  for (i = 0; i < NUMBER_OF_CORES; i++)
  {
    if (hsu_core_config_get_core(OS_BUS_TO_HAL_CORE_ID(i)) == NULL)
    {
      continue;
    }

    core_lpm_info = &(file_static_vars_ptr->core_table_ptr[i].usb_lpm_info);
    (void)rex_set_timer(&(core_lpm_info->cable_status_check_timer),
                      HSU_OS_BUS_LPM_ENTRY_TIMEOUT);
  }

}  /* hsu_os_notify_stack_ready */
#endif  /* !FEATURE_HS_USB_BASIC */
#endif  /* !T_REXNT */


/*===========================================================================
FUNCTION OS_HW_INIT

DESCRIPTION
  Discover all controllers on bus and call add_generic_controller
  for each.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes 
  defined in jerrno.h

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t os_hw_init()
{
  uint32 core_index;
  jresult_t  return_value = HSU_JSUCCESS;

  HSU_MSG_HIGH("os_hw_init: Entry.", 0, 0, 0);

  /* jsafe_enter needs to be called, because this function
  ** calls echi_init later on. ehci_init uses the os_delay_ms function,
  ** which requires that the Giant mutex will be locked before calling it.
  */
  #ifndef FEATURE_HS_USB_BASIC
  jsafe_enter();
  #endif  /* !FEATURE_HS_USB_BASIC */

  for (core_index = 0;
       core_index < NUMBER_OF_CORES;
       core_index++)
  {
    if (hsu_core_config_get_core(OS_BUS_TO_HAL_CORE_ID(core_index)) == NULL)
    {
      continue;
    }

    return_value = hsu_os_core_hw_init(core_index);
    if (return_value != HSU_JSUCCESS)
    {
      break;
    }
  }  /* for (core_index ...) */

  #ifndef FEATURE_HS_USB_BASIC
    jsafe_leave();
  #endif  /* !FEATURE_HS_USB_BASIC */

  HSU_MSG_HIGH("os_hw_init: Exit.", 0, 0, 0);

  return return_value;

} /* os_hw_init */


/*===========================================================================
FUNCTION HSU_OS_CORE_HW_INIT

DESCRIPTION
  call add_generic_controller for the core specified by core_index param.

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes 
  defined in jerrno.h

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_core_hw_init(uint32 core_index)
{
  usb_core_info_type* const 
  core_table_ptr= file_static_vars_ptr->core_table_ptr;

  jresult_t return_value;

#ifndef FEATURE_HSU_TEST
  hsu_core_config_init_mode_enum stack_init_mode;
#endif /* ! FEATURE_HSU_TEST */

  if (core_index >= NUMBER_OF_CORES)
  {
    HSU_MSG_ERROR("hsu_os_core_hw_init: Invalid core index parameter [%d].",
                  core_index, 0, 0);
    return JEINVAL;
  }

  HSU_MSG_HIGH("hsu_os_core_hw_init: HS_USB controller registration for core %d is starting...",
                core_index, 0, 0);

  if (core_table_ptr[core_index].controller_info_ptr)
  {
    HSU_MSG_HIGH("hsu_os_core_hw_init: HS_USB controller registration for core %d is already done.",
                  core_index, 0, 0);

    return HSU_JSUCCESS;
  }

  #ifndef FEATURE_HSU_TEST
  stack_init_mode = hsu_core_config_get_stack_init_mode(
    OS_BUS_TO_HAL_CORE_ID(core_index));
  if(stack_init_mode == HSU_CORE_CONFIG_INIT_MODE_DEVICE)
  {
    /* only the device entry in the controllers_list should be added. */
    #if (!defined(FEATURE_HS_USB_NO_HS_OTG_CORE) || defined(FEATURE_HS_USB_BASIC))
    if (core_index == HS_USB_CORE_INDEX)
    {
      /* The device controller's index is 1 for the high-speed core. */
      core_table_ptr[core_index].controller_info_ptr = 
        &(core_table_ptr[core_index].controllers_list[1]);
    }
    #endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE || FEATURE_HS_USB_BASIC */

    #if (defined(FEATURE_HS_USB_LEGACY_DEVICE) && !defined(FEATURE_HS_USB_BASIC))
    if (core_index == FS_USB_CORE_INDEX)
    {
      /* the host entry in the controllers_list should be added. */
      core_table_ptr[core_index].controller_info_ptr = 
        &(core_table_ptr[core_index].controllers_list[0]);
    }
    #endif /* FEATURE_HS_USB_LEGACY_DEVICE && !FEATURE_HS_USB_BASIC */
  }
  else if(stack_init_mode == HSU_CORE_CONFIG_INIT_MODE_OTG)
  {
    /* The OTG entry (or device entry for the full-speed core) 
    ** in the controllers_list should be added.
    */
    core_table_ptr[core_index].controller_info_ptr = 
      &(core_table_ptr[core_index].controllers_list[0]);
  }
  #if (defined (FEATURE_HS_USB_LEGACY_HOST) || defined (FEATURE_HS_USB_UICC))
  else if(stack_init_mode == HSU_CORE_CONFIG_INIT_MODE_HOST)
  {
    /* the host entry in the controllers_list should be added. */
    core_table_ptr[core_index].controller_info_ptr = &(core_table_ptr[core_index].controllers_list[0]);
  }
  #endif /* if defined FEATURE_HS_USB_LEGACY_HOST || defined FEATURE_HS_USB_UICC */
  else if(stack_init_mode == HSU_CORE_CONFIG_INIT_MODE_LAST)
  {
    HSU_MSG_HIGH("hsu_os_core_hw_init: HS_USB controller registration skipped, core %d is NULL.",
      core_index, 0, 0);
    return HSU_JSUCCESS;
  }
  else
  {
    HSU_ASSERT(
      "os_hw_init(): hsu_conf_sel_stack_utils_get_primary_stack_init_mode returned an undefined init option." 
      == NULL);
    return EUNKNOWN;
  }
  #else/* ! FEATURE_HSU_TEST */
#error code not present
  #endif /* ! FEATURE_HSU_TEST */
  
  return_value = 
    add_generic_controller((os_bus_h)OS_BUS_TO_HAL_CORE_ID(core_index),
                            (jint_t)core_table_ptr[core_index].controller_info_ptr->controller_type,
                            &(core_table_ptr[core_index].controller_info_ptr->controller_handle));

  if (return_value != HSU_JSUCCESS)
  {
    HSU_MSG_ERROR("hsu_os_core_hw_init: HS-USB controller registration for core %d failed. Type: %d, Result: %d.",
                  core_index,
                  (jint_t)core_table_ptr[core_index].controller_info_ptr->controller_type,
                  return_value);
  }
  else
  {
    HSU_MSG_HIGH("hsu_os_core_hw_init: HS_USB controller registration for core %d succeeded. Type: %d",
                 core_index,
                 (jint_t)core_table_ptr[core_index].controller_info_ptr->controller_type,
                 0);
  }

  return return_value;

} /* hsu_os_core_hw_init */


/*===========================================================================
FUNCTION OS_HW_UNINIT

DESCRIPTION
  Remove all controllers, call del_generic_controller for each.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void os_hw_uninit(void)
{
  uint32 core_index;

  HSU_MSG_HIGH("os_hw_uninit: Un-registering High-Speed USB controllers.",
               0, 0, 0);

  for (core_index = 0; core_index < NUMBER_OF_CORES; core_index++)
  {
    if (hsu_core_config_get_core(OS_BUS_TO_HAL_CORE_ID(core_index)) == NULL)
    {
      continue;
    }

    hsu_os_core_hw_uninit(core_index);
  }

  HSU_MSG_HIGH("os_hw_uninit: HS-USB controllers' un-registration is completed.",
               0, 0, 0);

} /* os_hw_uninit */

/*===========================================================================
FUNCTION HSU_OS_CORE_HW_UNINIT

DESCRIPTION
  Remove a specific controller, call del_generic_controller to delete it.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_core_hw_uninit(uint32 core_index)
{
  usb_core_info_type* const 
    core_table_ptr = file_static_vars_ptr->core_table_ptr;

  if (core_index >= NUMBER_OF_CORES)
  {
    HSU_MSG_ERROR("hsu_os_core_hw_uninit: Invalid core index parameter [%d].",
                  core_index, 0, 0);
    return;
  }
  
  HSU_MSG_HIGH("hsu_os_core_hw_uninit: HS_USB controller un-registration for core %d is starting...",
    core_index, 0, 0);

  if ( (core_table_ptr[core_index].controller_info_ptr)
    && (core_table_ptr[core_index].controller_info_ptr->controller_handle) )
  {
    jresult_t result = del_generic_controller(core_table_ptr[core_index].controller_info_ptr->controller_handle);

    if (result != HSU_JSUCCESS)
    {
      HSU_MSG_ERROR("hsu_os_core_hw_uninit: HS-USB controller un-registration for core %d failed. Handle: %p.",
        core_index, core_table_ptr[core_index].controller_info_ptr->controller_handle, 0);
    }
    else
    {
      HSU_MSG_HIGH("hsu_os_core_hw_uninit: HS_USB controller un-registration for core %d succeeded. Type: %d",
        core_index, (jint_t)core_table_ptr[core_index].controller_info_ptr->controller_type, 0);
    }

    HSU_ASSERT(result == HSU_JSUCCESS);

    core_table_ptr[core_index].controller_info_ptr = NULL;
  }
  else
  {
    HSU_MSG_HIGH("hsu_os_core_hw_uninit: HS_USB controller un-registration for core %d is already done.",
      core_index, 0, 0);
  }
} /* hsu_os_core_hw_uninit */


#if defined(FEATURE_HS_USB_BASIC) || (defined(FEATURE_HS_USB_POLLING) && \
  !defined(FEATURE_HS_USB_NO_HS_OTG_CORE))
/*===========================================================================
FUNCTION HSU_OS_BUS_POLL_INTERRUPT

DESCRIPTION
  Checks whether a High-Speed USB controller interrupt
  has occurred.

  This function accesses High-Speed USB controller interrupt status registers
  in order to determine whether an interrupt has occurred.

DEPENDENCIES
  None.

RETURN VALUE
  TRUE if an interrupt has occurred. FALSE otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
boolean hsu_os_bus_poll_interrupt(void)
{
  const uint32 usbsts_reg_offset  = 0x144;
  const uint32 usbintr_reg_offset = 0x148;
  const uint32 otgsc_reg_offset   = 0x1A4;

  /* Pointer to the the memory resource of the HS-USB core(OTG) */
  bus_resource_type* const bus_resource = 
    &(file_static_vars_ptr->core_table_ptr[HS_USB_CORE_INDEX].controllers_list[0].resource_list[1]);
  memory_resource_info_type* const mem_res_ptr = 
    &(bus_resource->resource_instance.mem_resource_instance);

  boolean  return_value;

  uint32   usbsts_reg_value;
  uint32   usbintr_reg_value;
  uint32   otgsc_reg_value;

  uint32   usbsts_reg_set_interrupt_bits;
  uint32   otgsc_reg_set_interrupt_bits;


  /* Read the enabled interrupts bit mask. */
  usbintr_reg_value = os_bus_space_read_4(mem_res_ptr,
                                          usbintr_reg_offset);

  /* Read the recent USB controller interrupt events. */
  usbsts_reg_value  = os_bus_space_read_4(mem_res_ptr,
                                          usbsts_reg_offset);

  usbsts_reg_set_interrupt_bits = usbsts_reg_value & usbintr_reg_value;

  /* Read the enabled interrupts bit mask & interrupts status from OTGSC 
     register */
  otgsc_reg_value = os_bus_space_read_4(mem_res_ptr, otgsc_reg_offset);

  /* OTGSC bits [31..24] are interrupt enable bits, and bits [23..16] are 
    interrupt status bits. */
  otgsc_reg_set_interrupt_bits = (otgsc_reg_value >> 24) & (otgsc_reg_value >> 16);

  /* Return TRUE if and only if identified an interrupt in USBSTS or OTGSC */
  if ((usbsts_reg_set_interrupt_bits != 0) || (otgsc_reg_set_interrupt_bits != 0))
  {
    return_value = TRUE;
  }
  else
  {
    return_value = FALSE;
  }

  return return_value;

}  /* hsu_os_bus_poll_interrupt */

/*===========================================================================
FUNCTION HSU_OS_BUS_HANDLE_INTERRUPT

DESCRIPTION
  This function handles a High-Speed USB OTG core interrupt.
  This function is used during the phone software download mode.

  NOTE - This function should only be called when a High-Speed USB OTG
         core interrupt is detected.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_handle_interrupt(void)
{
  hsu_os_isr_wrapper_int(HS_USB_CORE_INDEX);
}  /* hsu_os_bus_handle_interrupt */
#endif  /* FEATURE_HS_USB_BASIC || 
          (FEATURE_HS_USB_POLLING && !FEATURE_HS_USB_NO_HS_OTG_CORE) */


#if !defined(T_REXNT) && !defined(FEATURE_HS_USB_BASIC)
/*===========================================================================
FUNCTION HSU_OS_BUS_SLEEP_REGISTER

DESCRIPTION
  Register the High-Speed USB task at the sleep task's voters list.

DEPENDENCIES
  None.

RETURN VALUE
  True if the registration was successful. False otherwise.

SIDE EFFECTS
  None.
===========================================================================*/
static boolean hsu_os_bus_sleep_register(void)
{
  HSU_MSG_HIGH("hsu_os_bus_sleep_register: Entry.", 0, 0, 0);

  /* Catch doubly sleep registering cases. */
  HSU_ASSERT(file_static_vars_ptr->hsu_sleep_vote_handle == 0);

  file_static_vars_ptr->hsu_sleep_vote_handle = 
    sleep_register("HS-USB", FALSE);

  HSU_ASSERT(file_static_vars_ptr->hsu_sleep_vote_handle != 0);
  if (file_static_vars_ptr->hsu_sleep_vote_handle == 0)
  {
    HSU_MSG_ERROR("Registration of the High-Speed USB subsystem in the sleep task's voters list failed.",
                  0, 0, 0);
    return FALSE;
  }

  #if defined(T_QSC1100)
  sleep_set_hclk_restriction( file_static_vars_ptr->hsu_sleep_vote_handle,
                              CLKRGM_HCLK_FULL );
  #endif /* T_QSC1100 */

  HSU_MSG_HIGH("hsu_os_bus_sleep_register: Exit.", 0, 0, 0);

  return TRUE;
}
#endif  /* !T_REXNT && !FEATURE_HS_USB_BASIC */


/*===========================================================================
FUNCTION HSU_OS_BUS_INIT

DESCRIPTION
  Initializes the bus module. Called by the OS abstraction layer's main
  initialization function (hsu_os_init).

DEPENDENCIES
  None.

RETURN VALUE
  Returns 0 on success, otherwise returns one of the error codes 
  defined in jerrno.h

SIDE EFFECTS
  None.
===========================================================================*/
jresult_t hsu_os_bus_init(void)
{
  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC
  boolean  result;
  usb_lpm_info_type* core_lpm_info;
  uint32 core_index;
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */


  HSU_MSG_HIGH("hsu_os_bus_init: Entry.", 0, 0, 0);

  #ifdef FEATURE_HS_USB_HAL
  HAL_hsusb_Init();
  HSU_ASSERT(HAL_hsusb_SupportedCoresQty() >= NUMBER_OF_CORES);
  #endif /* FEATURE_HS_USB_HAL */

  hsu_os_bus_init_core_index_maps();

  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC

  result = hsu_os_bus_sleep_register();
  if (result == FALSE)
  {
    return EUNKNOWN;
  }

  #ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  /* set PMIC VREGS to working mode */
  handle_lpm_vreg_registration(TRUE);
  #endif /* FEATURE_HS_USB_NO_HS_OTG_CORE */

  /* Disallow dynamic clock rate switching. */
  clk_regime_register_for_cpu_resource(CLKRGM_CPU_CLIENT_HS_USB);
  
  #if defined(T_MSM7600) && !defined(FEATURE_HS_USB_INTEGRATED_PHY)
#error code not present
  #endif  /* T_MSM7600 && !FEATURE_HS_USB_INTEGRATED_PHY */

  #if defined(T_MSM7600) || defined(T_MSM7625) || \
      defined(T_MDM8200) || defined(T_MDM8900)

  /* Turn on the HS-USB registers clock. */
  clk_regime_msm_enable(CLKRGM_USB_HS_PCLK);

  #elif defined(T_MSM6246) || defined(T_MSM6290) 

  /* Turn on HS-USB registers clock. */
  clk_regime_enable(CLK_RGM_USB_OTG_1_M);

  /* Turn off HS-USB serial clock interface, as it is not
  ** used right now.
  */
  clk_regime_disable(CLK_RGM_USB_FS_M);

  /* Register to keep AHB speed constant during ARM halt */
  clk_regime_register_ahb_constant(CLK_AHB_USB);  

  #elif defined(T_QSC6270)

  /* Turn on HS-USB registers clock. */
  clk_regime_enable(CLK_RGM_USB_M);

  /* Register to keep AHB speed constant during ARM halt */
  clk_regime_register_ahb_constant(CLK_AHB_USB);  

  #endif /* T_MSM7600 || T_MSM7625 */

  for (core_index = 0;
  core_index < NUMBER_OF_CORES;
  core_index++)
  {
    if (hsu_core_config_get_core(OS_BUS_TO_HAL_CORE_ID(core_index)) == NULL)
    {
      continue;
    }

    core_lpm_info =
      &(file_static_vars_ptr->core_table_ptr[core_index].usb_lpm_info);
    
    rex_def_timer_ex(&(core_lpm_info->lpm_entry_timer),
                     hsu_os_bus_lpm_entry_timer_cb,
                     core_index);

    timer_def(&(core_lpm_info->remote_wakeup_timer),
              &(core_lpm_info->remote_wakeup_timer_group),
              NULL,
              NULL,
              hsu_os_bus_rm_wakeup_timer_cb,
              (timer_cb_data_type)core_index);

    rex_def_timer_ex(&(core_lpm_info->cable_status_check_timer),
                     hsu_os_bus_cable_status_check_timer_cb,
                     core_index);
    rex_def_timer_ex(
        &(file_static_vars_ptr->core_table_ptr[core_index].resume_peripherals_timer),
        hsu_os_bus_resume_peripherals_timer_cb,
        0);
  }
  rex_init_crit_sect(&(file_static_vars_ptr->lpm_crit_sect));

  #ifdef FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND
  file_static_vars_ptr->usb_reset_rework_is_installed = 
    hsu_api_usb_reset_rework_installed();
  #endif /* FEATURE_HS_USB_RESET_FROM_SUSPEND_WORKAROUND */

  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */
  
  HSU_MSG_HIGH("hsu_os_bus_init: Exit.", 0, 0, 0);

  return HSU_JSUCCESS;

} /* hsu_os_bus_init */


/*===========================================================================
FUNCTION HSU_OS_BUS_UNINIT

DESCRIPTION
  Un-initialize the bus module. Called by the OS abstraction layer's main
  un-initialization function (hsu_os_uninit).

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void hsu_os_bus_uninit(void)
{
  HSU_MSG_HIGH("hsu_os_bus_uninit: Entry.", 0, 0, 0);

  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC

  file_static_vars_ptr->hsu_sleep_vote_handle = 0;

  #if defined(T_MSM7600) || defined(T_MSM7625) || \
      defined(T_MDM8200) || defined(T_MDM8900)

  /* Turn off the HS-USB pbus clock. */
  clk_regime_msm_disable(CLKRGM_USB_HS_PCLK);

  #elif defined(T_MSM6246) || defined(T_MSM6290) 

  /* Turn on HS-USB registers clock. */
  clk_regime_disable(CLK_RGM_USB_OTG_1_M);

  /* We do not need to keep AHB at constant sleep during LPM. */
  clk_regime_deregister_ahb_constant(CLK_AHB_USB);

  #elif defined(T_QSC6270)

  clk_regime_disable(CLK_RGM_USB_M);

  /* We do not need to keep AHB at constant sleep during LPM. */
  clk_regime_deregister_ahb_constant(CLK_AHB_USB);
  
  #endif  /* T_MSM7600 || T_MSM7625 */

  #if defined(T_MSM7600) && !defined(FEATURE_HS_USB_INTEGRATED_PHY)
#error code not present
  #endif  /* T_MSM7600 && !FEATURE_HS_USB_INTEGRATED_PHY */

  /* Allow dynamic clock rate switching. */
  clk_regime_deregister_for_cpu_resource(CLKRGM_CPU_CLIENT_HS_USB);

  #ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  /* turn off PMIC VREGS */
  handle_lpm_vreg_registration(FALSE);
  #endif /* FEATURE_HS_USB_NO_HS_OTG_CORE */

  #endif /* FEATURE_HS_USB_BASIC */ 
  #endif  /* T_REXNT */

  HSU_MSG_HIGH("hsu_os_bus_uninit: Exit.", 0, 0, 0);

} /* hsu_os_bus_uninit */


/*===========================================================================
FUNCTION OS_BUS_ALLOC_RESOURCE

DESCRIPTION
  Returns a handle to a bus resource.

DEPENDENCIES
  hsu_os_bus_init must be called prior to calling this function.

RETURN VALUE
  Returns a handle to the allocated resource, 
  or NULL if the allocation fails.

SIDE EFFECTS
  None.
===========================================================================*/
os_bus_resource_h os_bus_alloc_resource
(
  /* High-Speed USB core handle 
  ** (provided by the 'add_generic_controller' function).
  */
  os_bus_h bus,

  /* Resource type.
  ** May be one of the following types: 
  **   SYS_RES_IRQ    - Interrupt line.
  **   SYS_RES_MEMORY - Physical address range.
  */
  jint_t type,

  /* Resource id.
  */
  jint_t id
)
{
  uint32  core_index,
          controller_index,
          resource_index;

  os_bus_resource_h return_value = NULL;

  bus_resource_type*
    resource_list_ptr = NULL;

  usb_core_info_type* const 
    core_table_ptr = file_static_vars_ptr->core_table_ptr;


  HSU_PARAM_COULD_BE_CONST(bus);

  core_index = OS_BUS_FROM_HAL_CORE_ID((uint32)bus);
  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  /* Only memory or interrupt resources are supported. */
  HSU_ASSERT((type == SYS_RES_MEMORY) ||
             (type == SYS_RES_IRQ));

  HSU_MSG_HIGH("os_bus_alloc_resource: Allocating %s resource of HS-USB core %d",
               (type == SYS_RES_IRQ) ? "an interrupt" : "a memory",
                core_index,
                0);

  for (controller_index = 0;
       controller_index < core_table_ptr[core_index].number_of_controllers;
       controller_index++)
  {
  /* Scan for the resource whose type is 'type'.
  */
    resource_list_ptr = core_table_ptr[core_index].
                          controllers_list[controller_index].resource_list;

  for (resource_index = 0;
       resource_index < OS_BUS_NUM_RES_PER_CONTROLLER;
       resource_index++)
  {
    if ((type != resource_list_ptr[resource_index].resource_type) ||
        (id != resource_list_ptr[resource_index].resource_id))
    {
      continue;
    }

    /* The requested resource was found.
    ** Return a pointer to this resource.
    */
    return_value = (os_bus_resource_h)
        &(resource_list_ptr[resource_index].resource_instance);

    /* Quit the for loop. */
    break;
  }  /* for (resource_index ...) */
  }  /* for (controller_index...) */

  /* Verify that the requested resource was
  ** found in the database.
  */
  HSU_ASSERT(return_value != NULL);

  HSU_MSG_HIGH("os_bus_alloc_resource: Resource allocation is successful.",
               0, 0, 0);

  return return_value;

} /* os_bus_alloc_resource */


/*===========================================================================
FUNCTION OS_BUS_RELEASE_RESOURCE

DESCRIPTION
  Releases a handle to a bus resource, acquired using
  os_bus_alloc_resource().

  Actually, this function does nothing but to assert that this
  resource handle refers to a valid resource.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void os_bus_release_resource
(
  /* A High-Speed USB core handle previously registered
  ** at the stack by 'add_generic_controller'.
  */
  os_bus_h          bus,

  /* The handle of the released resource.
  ** (Previously supplied by 'os_bus_alloc_resource').
  */
  os_bus_resource_h res
)
{
  uint32  core_index,
          controller_index,
          resource_index;

  bus_resource_type*
    resource_list_ptr = NULL;

  boolean resource_found;

  usb_core_info_type* const 
    core_table_ptr = file_static_vars_ptr->core_table_ptr;


  HSU_PARAM_COULD_BE_CONST(bus);
  HSU_PARAM_COULD_BE_CONST(res);

  core_index = OS_BUS_FROM_HAL_CORE_ID((uint32)bus);
  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  HSU_MSG_HIGH("os_bus_release_resource: Releasing HS-USB core %d resource. Handle: %p",
                core_index, res, 0);

  /* Scan for the resource. */
  resource_found = FALSE;

  for (controller_index = 0;
       controller_index < core_table_ptr[core_index].number_of_controllers;
       controller_index++)
  {
    /* Scan for the resource whose type is 'type'.
    */
    resource_list_ptr = core_table_ptr[core_index].
                          controllers_list[controller_index].resource_list;

  for (resource_index = 0;
       resource_index < OS_BUS_NUM_RES_PER_CONTROLLER;
       resource_index++)
  {
      if (res != &(resource_list_ptr[resource_index].resource_instance))
    {
      continue;
    }

    /* The requested resource was found.
    */
    resource_found = TRUE;

    /* Quit the for loop. */
    break;
  }  /* for (resource_index ...) */
  }  /* for (controller_index ...) */

  if (resource_found)
  {
    HSU_MSG_HIGH("os_bus_release_resource: Resource release is successful.",
                 0, 0, 0);
  }
  else
  {
    HSU_MSG_ERROR("os_bus_release_resource: Resource release has failed.",
                  0, 0, 0);

    HSU_ASSERT(FALSE);
  }

} /* os_bus_release_resource */


/*===========================================================================
FUNCTION OS_BUS_SPACE_READ_1

DESCRIPTION
  Reads a BYTE from a given memory resource.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the data that was read.

SIDE EFFECTS
  None.
===========================================================================*/
/* Read a Byte from the resource */
juint8_t os_bus_space_read_1
(
  os_bus_resource_h res,
  juint32_t         offset
)
{
  memory_resource_info_type* 
    memory_resource_ptr = (memory_resource_info_type*)res;

  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC 
  usb_lpm_info_type* core_lpm_info;

  HSU_USE_PARAM(core_lpm_info);
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */

  HSU_ASSERT(res != NULL);
  HSU_ASSERT(memory_resource_ptr->core_index < NUMBER_OF_CORES);

  /* Verify that the High-Speed USB core is not in low-power mode.
  ** (in this case, register access is disabled.)
  */
  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC 
  #ifndef FEATURE_HS_USB_NO_HS_OTG_CORE

  core_lpm_info = 
    &(file_static_vars_ptr->core_table_ptr[memory_resource_ptr->core_index].usb_lpm_info);
  
  /* If for some reason we are in LPM, force exiting LPM */
  if (core_lpm_info->is_in_low_power_mode)
  {
    hsu_os_bus_exit_lpm(memory_resource_ptr->core_index);

    HSU_MSG_HIGH("os_bus_space_read_1: forced LPM exit", 0, 0, 0);

    /* Schedule LPM entry which will take place 
    ** if no USB bus activity starts.
    */
    hsu_os_bus_schedule_lpm_entry(memory_resource_ptr->core_index);
  }

  #endif  /* !FEATURE_HS_USB_NO_HS_OTG_CORE */
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */

  HSU_ASSERT(offset < memory_resource_ptr->length);

  #ifdef FEATURE_HS_USB_HAL

  return HAL_hsusb_Read8(
    OS_BUS_TO_HAL_CORE_ID(memory_resource_ptr->core_index), 
    (uint32)memory_resource_ptr->start_address+offset);

  #else /* FEATURE_HS_USB_HAL */

  return inp(&(memory_resource_ptr->start_address[offset]));

  #endif /* FEATURE_HS_USB_HAL */

} /* os_bus_space_read_1 */


/*===========================================================================
FUNCTION OS_BUS_SPACE_READ_2

DESCRIPTION
  Reads a WORD (2 bytes) from a given memory resource.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the data that was read.

SIDE EFFECTS
  None.
===========================================================================*/
juint16_t os_bus_space_read_2
(
  os_bus_resource_h res,
  juint32_t         offset
)
{
  memory_resource_info_type* 
  memory_resource_ptr = (memory_resource_info_type*)res;

  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC 
  usb_lpm_info_type* core_lpm_info;

  HSU_USE_PARAM(core_lpm_info);
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */

  HSU_ASSERT(res != NULL);
  HSU_ASSERT(memory_resource_ptr->core_index < NUMBER_OF_CORES);

  /* Verify that 'offset' is a multiple of 2. */
  HSU_ASSERT((offset & 0x1) == 0);

  /* Verify that the High-Speed USB core is not in low-power mode.
  ** (in this case, register access is disabled.)
  */
  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC 
  #ifndef FEATURE_HS_USB_NO_HS_OTG_CORE

  core_lpm_info = 
    &(file_static_vars_ptr->
      core_table_ptr[memory_resource_ptr->core_index].usb_lpm_info);
  if (core_lpm_info->is_in_low_power_mode)
  {
    hsu_os_bus_exit_lpm(memory_resource_ptr->core_index);

    HSU_MSG_HIGH("os_bus_space_read_2: forced LPM exit", 0, 0, 0);

    /* Schedule LPM entry which will take place 
    ** if no USB bus activity starts.
    */
    hsu_os_bus_schedule_lpm_entry(memory_resource_ptr->core_index);
  }

  #endif   /* !FEATURE_HS_USB_NO_HS_OTG_CORE */
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */

  HSU_ASSERT(offset < memory_resource_ptr->length);

  #ifdef FEATURE_HS_USB_HAL

  return HAL_hsusb_Read16(
    OS_BUS_TO_HAL_CORE_ID(memory_resource_ptr->core_index), 
    (uint32)memory_resource_ptr->start_address + offset);

  #else /* FEATURE_HS_USB_HAL */

  return inpw((void*)(&(memory_resource_ptr->start_address[offset])));

  #endif /* FEATURE_HS_USB_HAL */

} /* os_bus_space_read_2 */


/*===========================================================================
FUNCTION OS_BUS_SPACE_READ_4

DESCRIPTION
  Reads a DWORD (4 bytes) from a given memory resource.

DEPENDENCIES
  None.

RETURN VALUE
  Returns the data that was read.

SIDE EFFECTS
  None.
===========================================================================*/
juint32_t os_bus_space_read_4
(
  os_bus_resource_h res,
  juint32_t         offset
)
{
  memory_resource_info_type* 
  memory_resource_ptr = (memory_resource_info_type*)res;

  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC 
  usb_lpm_info_type* core_lpm_info;

  HSU_USE_PARAM(core_lpm_info);
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */


  HSU_ASSERT(res != NULL);
  HSU_ASSERT(memory_resource_ptr->core_index < NUMBER_OF_CORES);

  /* Verify that 'offset' is a multiple of 4. */
  HSU_ASSERT((offset & 0x3) == 0);

  /* Verify that the High-Speed USB core is not in low-power mode.
  ** (in this case, register access is disabled.)
  */
  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC 
  #ifndef FEATURE_HS_USB_NO_HS_OTG_CORE

  core_lpm_info = 
    &(file_static_vars_ptr->
      core_table_ptr[memory_resource_ptr->core_index].usb_lpm_info);

  if (core_lpm_info->is_in_low_power_mode)
  {
    hsu_os_bus_exit_lpm(memory_resource_ptr->core_index);

    HSU_MSG_HIGH("os_bus_space_read_4: forced LPM exit", 0, 0, 0);

    /* Schedule LPM entry which will take place 
    ** if no USB bus activity starts.
    */
    hsu_os_bus_schedule_lpm_entry(memory_resource_ptr->core_index);
  }

  #endif  /* FEATURE_HSU_USB_MAIN_CORE */
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */

  HSU_ASSERT(offset < memory_resource_ptr->length);

  #ifdef FEATURE_HS_USB_HAL

  return HAL_hsusb_Read32(
    OS_BUS_TO_HAL_CORE_ID(memory_resource_ptr->core_index), 
    (uint32)memory_resource_ptr->start_address + offset);

  #else /* FEATURE_HS_USB_HAL */

  return inpdw((void*)(&(memory_resource_ptr->start_address[offset])));

  #endif /* FEATURE_HS_USB_HAL */

} /* os_bus_space_read_4 */


/*===========================================================================
FUNCTION OS_BUS_SPACE_WRITE_1

DESCRIPTION
  Writes a BYTE to a given memory resource.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void os_bus_space_write_1
(
  os_bus_resource_h res,
  juint32_t         offset,
  juint8_t          value
)
{
  memory_resource_info_type* 
  memory_resource_ptr = (memory_resource_info_type*)res;

  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC 
  usb_lpm_info_type* core_lpm_info;

  HSU_USE_PARAM(core_lpm_info);
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */


  HSU_ASSERT(res != NULL);
  HSU_ASSERT(memory_resource_ptr->core_index < NUMBER_OF_CORES);

  /* Verify that the High-Speed USB core is not in low-power mode.
  ** (in this case, register access is disabled.)
  */
  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC 
  #ifndef FEATURE_HS_USB_NO_HS_OTG_CORE

   core_lpm_info = 
    &(file_static_vars_ptr->
      core_table_ptr[memory_resource_ptr->core_index].usb_lpm_info);

  if (core_lpm_info->is_in_low_power_mode)
  {
    hsu_os_bus_exit_lpm(memory_resource_ptr->core_index);

    HSU_MSG_HIGH("os_bus_space_write_1: forced LPM exit", 0, 0, 0);

    /* Schedule LPM entry which will take place 
    ** if no USB bus activity starts.
    */
    hsu_os_bus_schedule_lpm_entry(memory_resource_ptr->core_index);
  }

  #endif  /* !FEATURE_HS_USB_NO_HS_OTG_CORE */
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */

  HSU_ASSERT(offset < memory_resource_ptr->length);

  #ifdef FEATURE_HS_USB_HAL

  HAL_hsusb_Write8(
    OS_BUS_TO_HAL_CORE_ID(memory_resource_ptr->core_index), 
    (uint32)memory_resource_ptr->start_address + offset, value);

  #else /* FEATURE_HS_USB_HAL */

  outp(&(memory_resource_ptr->start_address[offset]), value);

  #endif /* FEATURE_HS_USB_HAL */

} /* os_bus_space_write_1 */


/*===========================================================================
FUNCTION OS_BUS_SPACE_WRITE_2

DESCRIPTION
  Writes a WORD (2 bytes) to a given memory resource.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void os_bus_space_write_2
(
  os_bus_resource_h res,
  juint32_t         offset,
  juint16_t         value
)
{
  memory_resource_info_type* 
  memory_resource_ptr = (memory_resource_info_type*)res;

  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC 
  usb_lpm_info_type* core_lpm_info;

  HSU_USE_PARAM(core_lpm_info);
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */


  HSU_ASSERT(res != NULL);
  HSU_ASSERT(memory_resource_ptr->core_index < NUMBER_OF_CORES);

  /* Verify that 'offset' is a multiple of 2. */
  HSU_ASSERT((offset & 0x1) == 0);

  /* Verify that the High-Speed USB core is not in low-power mode.
  ** (in this case, register access is disabled.)
  */
  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC 
  #ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  
  core_lpm_info = 
    &(file_static_vars_ptr->
      core_table_ptr[memory_resource_ptr->core_index].usb_lpm_info);

  if (core_lpm_info->is_in_low_power_mode)
  {
    hsu_os_bus_exit_lpm(memory_resource_ptr->core_index);
    HSU_MSG_HIGH("os_bus_space_write_2: forced LPM exit", 0, 0, 0);
    /* Schedule LPM entry which will take place 
    ** if no USB bus activity starts.
    */
    hsu_os_bus_schedule_lpm_entry(memory_resource_ptr->core_index);

      /* Schedule a USB cable status check. */
      (void)rex_set_timer(&(core_lpm_info->cable_status_check_timer),
          HSU_OS_BUS_LPM_ENTRY_TIMEOUT);
  }
  
  #endif  /* !FEATURE_HS_USB_NO_HS_OTG_CORE */
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */

  HSU_ASSERT(offset < memory_resource_ptr->length);

  #ifdef FEATURE_HS_USB_HAL

  HAL_hsusb_Write16(
    OS_BUS_TO_HAL_CORE_ID(memory_resource_ptr->core_index), 
    (uint32)memory_resource_ptr->start_address + offset, value);

  #else /* FEATURE_HS_USB_HAL */

  outpw((void*)(&(memory_resource_ptr->start_address[offset])), value);

  #endif /* FEATURE_HS_USB_HAL */

} /* os_bus_space_write_2 */


/*===========================================================================
FUNCTION OS_BUS_SPACE_WRITE_4

DESCRIPTION
  Writes a DWORD to a given memory resource.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void os_bus_space_write_4
(
  os_bus_resource_h res,
  juint32_t         offset,
  juint32_t         value
)
{
  memory_resource_info_type* 
  memory_resource_ptr = (memory_resource_info_type*)res;

  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC 
  usb_lpm_info_type* core_lpm_info;

  HSU_USE_PARAM(core_lpm_info);
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */


  HSU_ASSERT(res != NULL);
  HSU_ASSERT(memory_resource_ptr->core_index < NUMBER_OF_CORES);

  /* Verify that 'offset' is a multiple of 4. */
  HSU_ASSERT((offset & 0x3) == 0);

  /* Verify that the High-Speed USB core is not in low-power mode.
  ** (in this case, register access is disabled.)
  */
  #ifndef T_REXNT
  #ifndef FEATURE_HS_USB_BASIC 
  #ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  
  core_lpm_info = 
    &(file_static_vars_ptr->
      core_table_ptr[memory_resource_ptr->core_index].usb_lpm_info);

  if (core_lpm_info->is_in_low_power_mode)
  {
    hsu_os_bus_exit_lpm(memory_resource_ptr->core_index);

    HSU_MSG_HIGH("os_bus_space_write_4: forced LPM exit", 0, 0, 0);

    /* Schedule LPM entry which will take place 
    ** if no USB bus activity starts.
    */
    hsu_os_bus_schedule_lpm_entry(memory_resource_ptr->core_index);
  }

  #endif  /* !FEATURE_HS_USB_NO_HS_OTG_CORE */
  #endif  /* !FEATURE_HS_USB_BASIC */
  #endif  /* !T_REXNT */

  HSU_ASSERT(offset < memory_resource_ptr->length);

  #ifdef FEATURE_HS_USB_HAL

  HAL_hsusb_Write32(
    OS_BUS_TO_HAL_CORE_ID(memory_resource_ptr->core_index), 
    (uint32)memory_resource_ptr->start_address + offset, value);

  #else /* FEATURE_HS_USB_HAL */

  outpdw((void*)(&(memory_resource_ptr->start_address[offset])), value);

  #endif /* FEATURE_HS_USB_HAL */

} /* os_bus_space_write_4 */


#if (!defined(T_REXNT)  && !defined(FEATURE_HS_USB_BASIC))
#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
/*===========================================================================
FUNCTION OS_BUS_START_STOP_LPM_TIMER

DESCRIPTION
  This function starts/stops the LPM timer and cancels any pending cable status 
  check depending on the received parameter (TRUE - start LPM timer, FALSE - Stop)

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void os_bus_start_stop_lpm_timer(boolean start_timer)
{
  usb_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[HS_USB_CORE_INDEX].usb_lpm_info);

  if (start_timer)
  {
    /* Cancel any pending cable status check. */
    (void)rex_clr_timer(&(core_lpm_info->cable_status_check_timer));
    HSU_MSG_HIGH("os_bus_start_stop_lpm_timer: Cable status check timer canceled.",
                 0, 0, 0);

    /* Arm the low power mode entry timer. */
    HSU_MSG_HIGH("os_bus_start_stop_lpm_timer: scheduling LPM entry", 0, 0, 0);
    hsu_os_bus_schedule_lpm_entry(HS_USB_CORE_INDEX);
  }
  else
  {
    /* Cancel any pending entry of the Phy into low-power mode. */
    hsu_os_bus_cancel_scheduled_lpm_entry(HS_USB_CORE_INDEX);
    HSU_MSG_HIGH("os_bus_start_stop_lpm_timer: LPM entry was canceled.",
                 0, 0, 0);

    /* Cancel any pending cable status check. */
    (void)rex_clr_timer(&(core_lpm_info->cable_status_check_timer));
    HSU_MSG_HIGH("os_bus_start_stop_lpm_timer: Cable status check timer canceled.",
                 0, 0, 0);
  }
}  /* os_bus_start_stop_lpm_timer */
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */

/*===========================================================================
FUNCTION HSU_OS_BUS_GET_NUM_CORES

DESCRIPTION
  This function returns the number of cores supported.

DEPENDENCIES
  None.

RETURN VALUE
  Number of supported cores.

SIDE EFFECTS
  None.
===========================================================================*/
uint32 hsu_os_bus_get_num_cores(void)
{
  return NUMBER_OF_CORES;
}

/*===========================================================================
FUNCTION HSU_OS_BUS_SAVE_MEM_DUMP

DESCRIPTION
  This function saves the following items in a predefined static memory:

    -USB device controller registers
    -USB host controller registers
    -ULPI registers

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#ifndef FEATURE_HS_USB_MEM_OPTIMIZATION
void hsu_os_bus_save_mem_dump(void)
{
#ifndef FEATURE_HS_USB_NO_HS_OTG_CORE
  uint8 phy_register;

  /* Pointer to the the memory resource of the first core (OTG) */
  bus_resource_type* const bus_resource = 
    &(file_static_vars_ptr->core_table_ptr[0].controllers_list[0].resource_list[1]);

  memory_resource_info_type* const mem_res_ptr = 
    &(bus_resource->resource_instance.mem_resource_instance);

  usb_lpm_info_type* const core_lpm_info =
    &(file_static_vars_ptr->core_table_ptr[0].usb_lpm_info);

  INTLOCK();

  if (!core_lpm_info->is_in_low_power_mode)
  {
    /*Save OTG controller registers: */
    memcpy(hsu_os_abstract_bus.otg_controller_mem_dump_ptr,
           (void*)(mem_res_ptr->start_address),
           OTG_REG_MEM_SIZE);

    /*Read the ULPI registers */
    for (phy_register = 0; phy_register < ULPI_REG_MEM_SIZE; phy_register++)
    {
      (void)hsu_os_bus_read_from_phy(phy_register, 
                                     &hsu_os_abstract_bus.ulpi_reg_mem_dump_ptr[phy_register]);
    }
  }
  else
  {
    memset((void*)hsu_os_abstract_bus.otg_controller_mem_dump_ptr,
            0xDEADBEEF,
            OTG_REG_MEM_SIZE);

    memset((void*)hsu_os_abstract_bus.ulpi_reg_mem_dump_ptr,
           0xDEADBEEF,
           ULPI_REG_MEM_SIZE);
  }

  INTFREE();
#endif /* !FEATURE_HS_USB_NO_HS_OTG_CORE */
}
#endif /* ! FEATURE_HS_USB_MEM_OPTIMIZATION */

#ifdef FEATURE_HS_USB_PMIC_PHY
/*===========================================================================
FUNCTION HSU_OS_BUS_SEND_EVENT_TO_CORE

DESCRIPTION
  This function sets events to event variable in interrupt resources 
  and calls the USB interrupt handler.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_send_event_to_core(uint32 core_index, uint32 event)
{
  interrupt_resource_info_type **isr_list;
  uint32 isr_list_num;
  int i;

  HSU_ASSERT( core_index < NUMBER_OF_CORES);

  /* broadcast the event on interrupt resources */
  isr_list = file_static_vars_ptr->core_table_ptr[core_index].isr_list;

  isr_list_num = file_static_vars_ptr->
                   core_table_ptr[core_index].num_of_registered_isrs;
  
  for(i=0; i< isr_list_num; i++)
  {
    isr_list[i]->event |= event;
  }

  /* trigger tramp interrupt to handle the event.
     */
  tramp_set_interrupt((tramp_isr_type)file_static_vars_ptr->
    core_table_ptr[core_index].interrupt_number);
}
#endif /* FEATURE_HS_USB_PMIC_PHY */


#if defined(FEATURE_HS_USB_PMIC_PHY) || defined(FEATURE_HS_USB_LEGACY_DEVICE)
/*===========================================================================
FUNCTION HSU_OS_BUS_HANDLE_PMIC_EVENT

DESCRIPTION
  This function handles PMIC PHY events when the PMIC PHY is used.
  
DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
#ifdef FEATURE_HS_USB_PMIC_PHY
void hsu_os_bus_handle_pmic_event(uint32 core_index, uint32 event)
{
  usb_core_info_type* const core_info =
    &(file_static_vars_ptr->core_table_ptr[core_index]);
  
  HSU_MSG_HIGH("hsu_os_bus_handle_pmic_event: Entry", 0, 0, 0);

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  switch(event)
  {
  case HSU_OS_BUS_DISCONNECT_EVT:
    core_info->cable_is_connected = FALSE;
    hsu_os_bus_send_event_to_core(core_index, event);
    break;

  case HSU_OS_BUS_CONNECT_EVT:
    core_info->cable_is_connected = TRUE;
    hsu_os_bus_send_event_to_core(core_index, event);
    break;

  case HSU_OS_BUS_RESUME_EVT:

    /* This function is called for raising a HS-USB interrupt
    ** in the Tramp. This would trigger an LPM exit and then the Resume
    ** event will get handled by the USB stack interrupt handler.
    ** In contrast to the connect\disconnect events, the HSU_OS_BUS_RESUME_EVT 
    ** event bit mask is not processed inside the HS-USB interrupt handler.
    */
    hsu_os_bus_send_event_to_core(core_index, event);
    break;

  default:
    HSU_MSG_ERROR("hsu_os_bus_handle_pmic_event: Unexpected evet - #%X.", 
                  event, 0, 0);
    break;

  }  /* switch(event) */

  HSU_MSG_HIGH("hsu_os_bus_handle_pmic_event: Exit.", 0, 0, 0);
}  /* hsu_os_bus_handle_pmic_event */
#elif defined(FEATURE_HS_USB_LEGACY_DEVICE)
void hsu_os_bus_handle_pmic_event(uint32 core_index, uint32 event)
{
  usb_core_info_type* const core_info =
    &(file_static_vars_ptr->core_table_ptr[core_index]);

  HSU_MSG_HIGH("hsu_os_bus_handle_pmic_event: Entry", 0, 0, 0);

  HSU_ASSERT(core_index < NUMBER_OF_CORES);

  switch(event)
  {
  case HSU_OS_BUS_DISCONNECT_EVT:
    core_info->cable_is_connected = FALSE;
    hsu_os_bus_schedule_lpm_entry(core_index);
    break;

  case HSU_OS_BUS_CONNECT_EVT:
    core_info->cable_is_connected = TRUE;
    hsu_os_bus_fs_legacy_exit_lpm(core_index);
    break;

  default:
    HSU_MSG_ERROR("hsu_os_bus_handle_pmic_event: Unexpected evet - #%X.", 
      event, 0, 0);
    break;

  }  /* switch(event) */

  HSU_MSG_HIGH("hsu_os_bus_handle_pmic_event: Exit.", 0, 0, 0);
}  /* hsu_os_bus_handle_pmic_event */
#endif /* FEATURE_HS_USB_LEGACY_DEVICE */
#endif /* FEATURE_HS_USB_PMIC_PHY || FEATURE_HS_USB_LEGACY_DEVICE */

#ifdef FEATURE_HS_USB_POLLING
/*===========================================================================

FUNCTION hsu_os_bus_init_polling

DESCRIPTION
  This function performs target dependant initialization for polling mode operation. 
  
DEPENDENCIES
  The function should run on polling mode. 

RETURN VALUE
None.

SIDE EFFECTS
None.
===========================================================================*/

void hsu_os_bus_init_polling(juint8_t core_id)
{
  
#if defined(T_QSC1100)
  /*============================================================
                   ULC
  =============================================================*/
  pm_err_flag_type ret = PM_ERR_FLAG__SUCCESS;

  /* activate the OTG xcvr */
  ret = pm_otg_config_power_management( PM_OTG_POWER_MANAGEMENT_CMD__ACTIVATE);
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);

  /* pull down D+/- low initially to start off from a USB disconnect state */
  ret = pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE, 
    PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH, 
    PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH, 
    PM_OTG_ID_CONFIG_CMD__NONE);
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);

  /* reset OTG transceiver's mode */
  ret = pm_otg_reset_mode();
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);

  /* config OTG xcvr for USB B-dev mode */
  ret = pm_otg_config_usb_mode( PM_OTG_DEV__B, PM_OTG_USB_SPEED__FULL);
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);

  /* pull D+ high to indicate to PC that the phone is ready for USB traffic */
  ret = pm_otg_config_interface( PM_OTG_VBUS_CONFIG_CMD__NONE,
    PM_OTG_DP_CONFIG_CMD__PULL_UP,
    PM_OTG_DM_CONFIG_CMD__DISABLE_BOTH, 
    PM_OTG_ID_CONFIG_CMD__NONE);
  HSU_ASSERT(ret == PM_ERR_FLAG__SUCCESS);

#elif defined(T_MSM7600) || defined(T_MSM7625)
/*============================================================
                      Roc & sRoc
=============================================================*/
  /* enable enterrupt  */
  if (file_static_vars_ptr->is_in_low_power_mode)
  {
    int loop_count = 0;

    /* turn on USBH_P_CLK */
    HSU_MSM_SET(GLBL_CLK_ENA, AXI_LI_APPS_CLK_ENA);
    HSU_MSM_SET(GLBL_CLK_ENA, USBH_P_CLK_ENA);

    /* Disable asynchronous interrupts from the PHY. */
    HWIO_OUTF(USB_OTG_HS_USBCMD, ASYNC_INTR_CTRL, 0);

    /* Let the STP signal to get out from the core to the PHY.
    */
    HWIO_OUTF(USB_OTG_HS_USBCMD, ULPI_STP_CTRL, 0);

    /* Set the PHCD bit in the port control register to zero. 
    ** This restarts the PHY clock and transfers the PHY into
    ** normal operating state.
    */
    HWIO_OUTF(USB_OTG_HS_PORTSC, PHCD, 0);

    /* Wait until the PHY finished its transition out 
    ** of low power mode.
    */
    while (HWIO_INF(USB_OTG_HS_PORTSC, PHCD) != 0)
    {
      /* Busy-wait loop. */
      HSU_ASSERT(loop_count < HSU_MAX_PHCD_DWELL_TIME_ATTEMPT);
      loop_count++;
    }

    #ifdef FEATURE_HS_USB_SMSC_PHY
    hsu_os_bus_turn_on_smsc_phy_comparators();
    #endif  /* FEATURE_HS_USB_SMSC_PHY */

    /* This flag MUST be updated as the last thing here, after the low-power
    ** mode exit is complete. Otherwise, an interrupt might access the HS-USB
    ** hardware too early.
    */
    file_static_vars_ptr->is_in_low_power_mode = FALSE;
  }  /* file_static_vars_ptr->is_in_low_power_mode */
#elif defined(T_MSM6246) || defined(T_MSM6290) || defined(T_QSC6270)
  hsu_os_bus_exit_lpm((uint32)core_id);
#endif /* T_MSM6246 || T_MSM6290 || T_QSC6270 */

}

#endif /* FEATURE_HS_USB_POLLING */

#ifdef FEATURE_HS_USB_UICC
jbool_t os_power_request(juint8_t voltage_class, juint8_t max_current, 
                         juint8_t *set_voltage_class, juint8_t *set_max_current)
{
  *set_voltage_class = voltage_class;
  *set_max_current = max_current;
  return TRUE;
}

void os_get_port_info(os_bus_h bus, juint8_t port, port_info_t *info)
{
  uint32 uicc_core_index = UICC_USB_CORE_INDEX;
  if (bus == (os_bus_h)uicc_core_index)
  {
    info->is_interchip = 1;
  }
  else
  {
    info->is_interchip = 0;
  }
}

#endif /* FEATURE_HS_USB_UICC */

#endif /* !T_REXNT !FEATURE_HS_USB_BASIC */


/*===========================================================================
FUNCTION HSU_OS_BUS_INIT_CORE_INDEX_MAPS

DESCRIPTION
  This function initializes the arrays used for translation between 
  hsu_os_abstract_bus index, and HAL (or core_config) core index.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
static void hsu_os_bus_init_core_index_maps(void)
{
#if !defined (FEATURE_HS_USB_NO_HS_OTG_CORE) || defined (FEATURE_HS_USB_BASIC)
    /* HS-OTG core */
    file_static_vars_ptr->hal_core_index_map_ptr[HS_USB_CORE_INDEX] = 0;
    file_static_vars_ptr->hal_core_index_inv_map_ptr[0] = HS_USB_CORE_INDEX;
#endif

#if defined(FEATURE_HS_USB_LEGACY_HOST) || defined(FEATURE_HS_USB_LEGACY_DEVICE)
    /* FS-Legacy core (7k HAL) */
    file_static_vars_ptr->hal_core_index_map_ptr[FS_USB_CORE_INDEX] = 1;
    file_static_vars_ptr->hal_core_index_inv_map_ptr[1] = FS_USB_CORE_INDEX;
#endif /* FEATURE_HS_USB_LEGACY_HOST || FEATURE_HS_USB_LEGACY_HOST */

#ifdef FEATURE_HS_USB_UICC
    /* UICC core (6k HAL) */
    file_static_vars_ptr->hal_core_index_map_ptr[UICC_USB_CORE_INDEX] = 1;
    file_static_vars_ptr->hal_core_index_inv_map_ptr[1] = UICC_USB_CORE_INDEX;
#endif /*FEATURE_HS_USB_UICC*/
}
