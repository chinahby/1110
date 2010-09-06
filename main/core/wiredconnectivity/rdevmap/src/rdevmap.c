/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

       R U N T I M E   D E V I C E   M A P P E R   S E R V I C E S


GENERAL DESCRIPTION
  This file contains types and declarations associated with the DMSS
  Runtime Device Mapper (RDM).

EXTERNALIZED FUNCTIONS

  rdm_init()
    Initializes the Runtime Device Mapper Services. This function must
    be called before other functions of this module.

  rdm_set_bt_mode()
    Set RDM mode to be BT or Non-BT mode.
    Since on PPG (MSM3300) BlueTooth & USB are mutually exclusive, RDM's
    tables must change depending on what *mode* we're in.

  rdm_get_prev_ports()
    Read in the port mappings last know by RDM from EFS.

  rdm_issue_opens()
    RDM issues opens to SRVCs based on last mapping before power down. If there
    is no EFS, the default mapping of DS->U1 and DIAG->U2 (if U2 defined) is
    performed.

  rdm_assign_port()
    Request RDM assign a specified application to the specified device.
  
  rdm_assign_port_tmp()
    Same function as rdm_assign_port(), except the change is non persistant.
   Restarting the phone will cause the port mapping to set to last known value.

  rdm_close_device()
    This function will close a device used by a service.

  rdm_notify()
    Notify RDM of request completion/status.

  rdm_get_device()
    Query RDM for the device that is currently assigned to the specified
    port (if any).

  rdm_get_multi_dev()
    Query the Runtime Device Mapper for the ports/devices currently being 
    used by the specified application/service.

  rdm_get_service()
    Query the Runtime Device Mapper for the application/service that is 
    currently assigned to the specified port/device (if any).

  rdm_get_valid_ports()
    Query the RDM for the valid devices the specified application can map to.

  rdm_register_open_func()
    Function for Service Tasks to register their open function. This open 
    function is what RDM calls to instruct a Service to open a specified port.

  rdm_register_close_func()
    Function for Service Tasks to register their close function. This close 
    function is what RDM calls to instruct a Service to close it's open port.

  rdm_register_func_multi_dev()
    Function for Service Tasks to register their open/close functions.  The 
    functions are what RDM calls to instruct a Service to open/close device.

  rdm_get_device_str()
    Query the RDM for the ASCII string that corresponds to the 
    rdm_device_enum_type specified.

  rdm_is_srv_holding_dev()
    Test if a service is using a device.

  rdm_set_srv_compat_mask()
    Set device compatible mask for the service from 
    (RDM_LAST_KNOWN_SRVC + 1) to (RDM_SRVC_MAX - 1). It tells RDM which 
    device this service can use.

  rdm_is_srv_dev_compat()
    Test if a service and a device is compatible. That is whether the service
    can use the device.

INITIALIZATION AND SEQUENCING REQUIREMENTS

  rdm_init() must be called prior to any of the other functions of
  this module.

Copyright (c) 2000-2008 by QUALCOMM, Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/COMMON/vcs/rdevmap.c_v   1.37.1.2   16 May 2003 14:33:04   wxie  $
  $Header: //depot/asic/msmshared/services/rdevmap/02.00.XX/rdevmap.c#1 $ $DateTime: 2008/10/19 19:39:23 $ $Author: vaghasia $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/08   rh      Klocwork cleanup2
05/12/08   rh      Include tramp.h
04/30/08   rh      Klocwork cleanup
02/20/08   jmm     Added messages to help debug failure return.
10/31/07   rh      FEATURE_BT_EXT_SOC_UART_TL has been replaced by 
                   FEATURE_BT_SOC_UART_TL FEATURE_BT_EXT_HC_SOC has been 
                   replaced by FEATURE_BT_SOC
10/08/07   ar      Added support for RDM_SIO_SMD_DATA11.
10/05/07   rh      Add support for modem restart
11/30/06   dk      Modified rdm_issue_opens() to work with modem only build.
11/15/06   TMR     Removing some RMNET featured code as per Jeff D.
10/11/06   rh      Adding support for FEATURE_HS_USB
08/29/06   ih      Default Diag to UART1 for WinMob build with 
                   FEATURE_DIAG_MP_MODEM_ONLY defined
08/23/06   sg      Purge saved port map for RDM_BREW_EXTENDED_SRVC on bootup.
06/26/06   gd      Removed MMC feature hack and cleaned up some code
06/19/06   bt      Added FEATURE_USB_BREW_EXTENDED_DEVICE support. 
06/19/06   sb      Added the function to control clk and modem ISR for USB
27/06/06   rh      BT support for diag is now controled by 
                   FEATURE_DIAG_SUPPORT_WIRELESS
05/01/06   vp      Changes for enabling multi ports for data
06/12/06   dsn     Support to use UART1DM for BT SoC support
25/05/06   rh      Add rdm_assign_port_tmp function for non-persistant port 
                   assignment.  Code merged from toddb.
01/04/06   ptm     Restore SMEM code, removed RPCMP and DIAG_MP services and
                   devices. Fixed RVCT warnings.
11/18/05   vs      Removed / supressed lint warnings
08/19/05   cp      Support for APS virtual port
06/05/05   TMR     Merged in MSM7500 code
05/09/05   cc      Updated rdm_assign_port() and rdm_close_device() to call 
                   the callback with device, as opposite to RDM_NULL_DEV, 
                   once found RDM state machine is busy.  
04/15/05   tmr     Changed name of SMEM data device to APPS uP
04/06/05   cc      Corprate code-review comments. 
                   Change LOCAL to static
                   Prevent OEM services request for non-existing NET_WWAN or
                   USB_SER2 port. 
                   Mark NET_WWAN unavailable if FEATURE_DATA_RM_NET not defined. 
03/08/05   cc      Fixed Typo of FEATURE_DATA_RM_NET in one place. 
03/08/05   cc      Merged in OSIFDev_Dec04 branck. 
2/11/05     nrh    Added conditional port map file name depending on build (apps only,
                   modem only, dual processor)
01/11/05   cc      Fixed function rdm_update_srv_dev_compat_table().
12/23/04   cc      Rename USB_CDC, USB_DIAG, USB_NMEA device to be USB_MDM, USB_SER1
                   and USB_SER2 respectively. 
                   Added FEATURE_DATA_RM_NET support. 
11/24/04   cc      Added support for RDM_USB_NET_WWAN_DEV and RDM_DATA_WWAN_SRVC. 
01/25/05   cc      Purge saved Part map for BREW_SRVC.  
10/20/04   ptm     Added default connection for RPC SM service and cleaned up
                   some white space and comments.
10/20/04   adm     Added support for featurization of UART1. 
                   Added support for multiple DS pipes. 
10/05/04   jtl     Allow Bluetooth HCI mode to use USB
06/15/04   cc      Added FEATURE_RDEVMAP_DIAG_DEFAULT_TO_USB
04/20/04   pn      Added support for BT_SPP port.
08/15/03   cc      Disallow any service to be mapped to RDM_MUSB_MMC_DEV at 
                   boot up, i.e. rdm_get_prev_ports().   
07/01/03   cc      Set RDM_NULL_DEV_BM as default value to rdm_srv_dev_compat_tab[]
                   for all OEM services.   
04/08/02   wxie    Protect RDM state in critical section, add capability 
                   to open multiple ports for a service, add new service 
                   easily, and other internal RDM changes.
02/21/03   cc      Added FEATURE_USB_DIAG_NMEA support. 
12/03/02   cc      Added back accidently ommitted callback calls in 
                   rdm_adssign_port() when return(TRUE).  
11/22/02   ltan    Replaced FEATURE_MMGPS with FEATURE_MMGPS_CSD, and added 
                   sanity check for RDM_LSM_CSD_DEV in rdm_get_prev_ports().     
10/11/02   cc      Changed rdm_get_prev_ports() to preserv DIAG on USB_DIAG.  
                   This fixes CR25022. 
08/24/02   ry      Updated NMEA featurization by adding FEATURE_MMGPS
07/15/02   ry      Added support for the LSM_CSD port
06/17/02   cc      Removes feature wrapping around RDM_RPC_SRVC in all tables 
                   and functions.  
                   Check rdm_configuration_table[][] see if each service is 
                   available when read port mapping from EFS.  
                   Added funciton rdm_remove_dup_mappings() and call this 
                   function from rdm_issue_opens().
                   All above is to fix CR23030
03/26/02   cc      rdm_cinfiguration_table[RPC][USB_CDC] define is dependent on 
                   if BT mode or USB mode is used.  Cofigure ...[RPC][..] in 
                   both sections of rdm_set_bt_mode().  
                   Also Added rdm_configuration_table[RPC][UART3] settings.  
03/08/02   cc      function rdm_data_got_atqcdmg() returns immediately 
                   as long as #FEATURE_USB_DIAG is defined. 
03/05/02   cc      function rdm_data_got_atqcdmg() returns immediately
                   if DIAG is on USB_DIAG port.
02/06/02   cc      Add third UART supporting.   
01/09/02   cc      Add ONCRPC service supporting.
                   In rdm_set_bt_mode(), configuration for NMEA and ONCRPC 
                   services are independent of bt_mode. 
12/04/01   cc      Change the function name rdm_data_got_at$qcdmg() 
                   to rdm_data_got_atqcdmg() to avoid future headach.
                   Update EFS when DS and DIAG getting into dance 
                   or getting out of dance completed. 
                   Preserve the DIAG on USB_CDC info in rdm_get_prev_ports.      
10/16/01   cc      Merge in mods of SIMBA from MSM5200_SIO.00.00.17, includes 
                   following 1 item.
08/13/01   cc      Add support for FEATURE_DATA.           
08/09/01   cc      Removed function rdm_mc_direct_opens() which is not used. 
08/06/01   cc      Merge gpsOne rdevmap.c -r1.3 to add NMEA support.
07/23/01   dwp     Don't save RDM_MMC_SRVC port mappings to EFS.
06/15/01   dwp     Special handling of when DS & DIAG dance over the USB CDC
                   endpoints. Make a call to sio_usb_diag_over_data() when
                   going into and coming out of the dance.
05/08/01   dwp     Implement smarter to_data_mode logic. Also, don't save port
                   mappings to EFS when in the dance.
03/30/01   dwp     Make rdm_get_prev_ports() more robust for devices coming &
                   going by build features compared to what's read from EFS.
03/28/01   dwp     Cleaned up rdm_get_device_str().
03/26/01   dl      Add support for SMEM_DS port.
03/26/01   dwp     Fix some intializations for HCI and DS in rdm_set_bt_mode.
03/22/01   dwp     FEATURE_DS_MAPPABLE_TO_UART2 is now required to be able to
                   have DS mappable to U2. This is in addition to 
                   FEATURE_SECOND_UART.
03/16/01   dwp     Removed restriction of DIAG not being able to boot to NULL.
03/09/01   dwp     Added logic in rdm_get_prev_ports() to not let DIAG
                   boot to NULL. Also DIAG can't boot to USB CDC ACM.
03/08/01   dwp     Added logic in rdm_get_valid_ports() to show if DIAG is
                   on USB when DS & DIAG are in the dance on CDC ACM.
02/28/01   dwp     Merge in Bama mods from MSM5105_SIO.00.00.09.
02/09/01   dwp     Add logic to handle port mappings read from EFS to devices
                   that may not be allowed in the current build.
01/31/01   dwp     Add call to rdm_update_efs() in rdm_issue_opens().
12/21/00   dwp     Add portmap persistence across resets using EFS. 
12/14/00   dwp     Need to display HCI mapping options even in USB mode.
12/13/00   dwp     RDM wasn't recording the callback pointer when a service
                   was assigned to the NULL device.
12/12/00   dwp     RDM must handle to_data_mode calls when not in the dance.
12/11/00   dwp     Add test for FEATURE_SECOND_UART in rdm_init().
12/06/00   dwp     Add call to rdm_reset_state_machine() in rdm_init().
11/27/00   dwp     Change SIO_PORT_BT_RFCOMM's to SIO_PORT_BT_NA. Mapped
                   BT_HCI_SRVC to U1,U2 only. Use some up front #defines for
                   devices to make rdm_set_bt_mode() easier to read/maintain.
11/16/00   dwp     Added UI interfaces and updated comments.
11/14/00   dwp     Created module.

===========================================================================*/



/*===========================================================================

                            INCLUDE FILES


===========================================================================*/
#include "rdevmap.h"
#include "AEEstd.h"

#include "comdef.h"

#include "err.h"
#include "rdevmap.h"               /* Runtime Device Mapper.               */
#include "string.h"
#include "assert.h"
#include "rex.h"

#ifdef FEATURE_EFS
#include "fs.h"
#endif

#ifdef FEATURE_USB_ON_APPS_PROC
#include "oncrpc.h"
#include "oncrpc_proxy.h"
#include "remote_apis.h"
#include "clkregim.h"
#include "tramp.h"
#endif /*FEATURE_USB_ON_APPS_PROC*/


/*===========================================================================
  Debugging support for this file
===========================================================================*/
#ifdef DEBUG_RDM
#define DEBUG( x )  { x }
#else
#define DEBUG( x )
#endif /* DEBUG_RDM */

/*===========================================================================

                     CONSTANT AND VARIABLE DECLARATIONS

===========================================================================*/

/*---------------------------------------------------------------------------
                                Macros
---------------------------------------------------------------------------*/

#if defined(FEATURE_USB) || defined(FEATURE_HS_USB)
#define RDM_USES_FSHS_USB
#endif

/* If RDM is always non-presistent, make RDM works like there is no EFS */
#ifdef FEATURE_RDM_ALWAYS_NON_PERSISTENT
#undef FEATURE_EFS
#endif

#ifdef CUST_EDITION
#define RDM_USB_MDM_INIT_SRVC   RDM_DATA_SRVC
#endif
/*---------------------------------------------------------------------------
  Bit mask defines for setting rdm_srv_dev_compat_tab in rdm_set_bt_mode and
  Sio Port settings for device_to_port_id_table depending on feature defines
---------------------------------------------------------------------------*/
/* NULL device is compatible with all the services, this means every 
   service has an option of not using any device                        */
#define RDM_NULL_DEV_BM     (1 << RDM_NULL_DEV)

/* UART1 is always an option */
#define RDM_UART1_BM        (1 << RDM_UART1_DEV)
#define RDM_SIO_UART1       SIO_PORT_UART_MAIN

#ifdef FEATURE_SECOND_UART_ON_ANY_PROC
#define RDM_UART2_BM        (1 << RDM_UART2_DEV)
#define RDM_SIO_UART2       SIO_PORT_UART_AUX
#else
#define RDM_UART2_BM        0
#define RDM_SIO_UART2       SIO_PORT_NULL
#endif /* FEATURE_SECOND_UART_ON_ANY_PROC */

#ifdef FEATURE_THIRD_UART_ON_ANY_PROC
#define RDM_UART3_BM        (1 << RDM_UART3_DEV)
#define RDM_SIO_UART3       SIO_PORT_UART_THIRD
#else
#define RDM_UART3_BM        0
#define RDM_SIO_UART3       SIO_PORT_NULL
#endif /* FEATURE_THIRD_UART_ON_ANY_PROC */

#ifdef FEATURE_USB_CDC_ACM
#define RDM_USB_MDM_BM      (1 << RDM_USB_MDM_DEV)
#define RDM_SIO_USB_MDM     SIO_PORT_USB_MDM
#else
#define RDM_USB_MDM_BM      0
#define RDM_SIO_USB_MDM     SIO_PORT_NULL
#endif /* FEATURE_USB_CDC_ACM */

#if defined(FEATURE_USB_PERIPHERAL_MASS_STORAGE)
#define RDM_USB_MMC_BM      (1 << RDM_USB_MMC_DEV)
#define RDM_SIO_USB_MMC     SIO_PORT_USB_MMC
#else
#define RDM_USB_MMC_BM      0
#define RDM_SIO_USB_MMC     SIO_PORT_NULL
#endif /* FEATURE_USB && FEATURE_MMC */

#if defined(RDM_USES_FSHS_USB) && \
    (defined(FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA) )
#define RDM_USB_SER1_BM     (1 << RDM_USB_SER1_DEV)
#define RDM_SIO_USB_SER1    SIO_PORT_USB_SER1
#else
#define RDM_USB_SER1_BM     0
#define RDM_SIO_USB_SER1    SIO_PORT_NULL
#endif /* FEATURE_USB && FEATURE_USB_DIAG */

#ifdef FEATURE_BT
#define RDM_BT_SPP_BM       (1 << RDM_BT_SPP_DEV)
#define RDM_BT_NA_BM        (1 << RDM_BT_NA_DEV)
#define RDM_SIO_BT_SPP      SIO_PORT_BT_SPP
#define RDM_SIO_BT_NA       SIO_PORT_BT_NA
#else
#define RDM_BT_SPP_BM       0
#define RDM_BT_NA_BM        0
#define RDM_SIO_BT_SPP      SIO_PORT_NULL
#define RDM_SIO_BT_NA       SIO_PORT_NULL
#endif /* FEATURE_BT */

#if defined(FEATURE_SMEM_DS) && !defined (FEATURE_DATA_ON_APPS)
#error code not present
#else
#define RDM_SMEM_DS_BM      0
#define RDM_SIO_SMEM_DS     SIO_PORT_NULL
#endif /* FEATURE_SMEM_DS */

#if defined(FEATURE_SMD) && !defined (FEATURE_DATA_ON_APPS)
#define RDM_SMD_DS_BM      (1 << RDM_SMD_DS_DEV)
#define RDM_SIO_SMD_DS     SIO_PORT_SMD_DS
#else
#define RDM_SMD_DS_BM      0
#define RDM_SIO_SMD_DS     SIO_PORT_NULL
#endif 

#ifdef FEATURE_SMD
#define RDM_SMD_NMEA_BM    (1 << RDM_SMD_NMEA_DEV)
#define RDM_SIO_SMD_NMEA   SIO_PORT_SMD_GPS_NMEA
#else
#define RDM_SMD_NMEA_BM    0
#define RDM_SIO_SMD_NMEA   SIO_PORT_NULL
#endif 

#if defined(FEATURE_SMD) && defined(FEATURE_DATA_SERIALIZER) && defined(FEATURE_DATA_MULTI_PORTS)
#define RDM_SMD_DATA1_BM   (1 << RDM_SMD_DATA1_DEV)
#define RDM_SMD_DATA2_BM   (1 << RDM_SMD_DATA2_DEV)
#define RDM_SMD_DATA3_BM   (1 << RDM_SMD_DATA3_DEV)
#define RDM_SMD_DATA4_BM   (1 << RDM_SMD_DATA4_DEV)
#define RDM_SIO_SMD_DATA1  SIO_PORT_SMD_DATA1 
#define RDM_SIO_SMD_DATA2  SIO_PORT_SMD_DATA2 
#define RDM_SIO_SMD_DATA3  SIO_PORT_SMD_DATA3 
#define RDM_SIO_SMD_DATA4  SIO_PORT_SMD_DATA4 
#else
#define RDM_SMD_DATA1_BM   0 
#define RDM_SMD_DATA2_BM   0
#define RDM_SMD_DATA3_BM   0 
#define RDM_SMD_DATA4_BM   0 
#define RDM_SIO_SMD_DATA1  SIO_PORT_NULL
#define RDM_SIO_SMD_DATA2  SIO_PORT_NULL
#define RDM_SIO_SMD_DATA3  SIO_PORT_NULL
#define RDM_SIO_SMD_DATA4  SIO_PORT_NULL
#endif /* FEATURE_SMD */

#if defined(FEATURE_SMD)
#define RDM_SMD_DATA11_BM  (1 << RDM_SMD_DATA11_DEV)
#define RDM_SIO_SMD_DATA11 SIO_PORT_SMD_DATA11  
#else
#define RDM_SMD_DATA11_BM  0
#define RDM_SIO_SMD_DATA11 SIO_PORT_NULL
#endif /* FEATURE_SMD */

#ifdef FEATURE_MMGPS_CSD
#error code not present
#else
#define RDM_LSM_CSD_BM      0
#define RDM_SIO_LSM_CSD     SIO_PORT_NULL
#endif /* FEATURE_MMGPS_CSD */

#if defined(RDM_USES_FSHS_USB) &&  defined(FEATURE_USB_DIAG_NMEA) 
#define RDM_USB_SER2_BM     (1 << RDM_USB_SER2_DEV)
#define RDM_SIO_USB_SER2    SIO_PORT_USB_SER2
#else
#define RDM_USB_SER2_BM     0
#define RDM_SIO_USB_SER2    SIO_PORT_NULL
#endif /* FEATURE_USB && FEATURE_USB_DIAG_NMEA */

#if defined (FEATURE_USB) && defined( FEATURE_USB_BREW_EXTENDED_DEVICE)
#define RDM_USB_BREW_EXTENDED_DATA_BM    (1<< RDM_USB_BREW_EXTENDED_DATA_DEV)
#define RDM_SIO_USB_BREW_EXTENDED_DATA   SIO_PORT_USB_BREW_EXTENDED_DATA
#define RDM_USB_BREW_EXTENDED_NOTIFICATION_BM  (1<< RDM_USB_BREW_EXTENDED_NOTIFICATION_DEV)
#define RDM_SIO_USB_BREW_EXTENDED_NOTIFICATION  SIO_PORT_USB_BREW_EXTENDED_NOTIFICATION
#else
#define RDM_USB_BREW_EXTENDED_DATA_BM    0
#define RDM_SIO_USB_BREW_EXTENDED_DATA   SIO_PORT_NULL
#define RDM_USB_BREW_EXTENDED_NOTIFICATION_BM  0
#define RDM_SIO_USB_BREW_EXTENDED_NOTIFICATION  SIO_PORT_NULL
#endif /* FEATURE_USB_BREW_EXTENDED_DEVICE */

#ifdef FEATURE_DATA_RM_NET
#define RDM_USB_NET_WWAN_BM  (1 << RDM_USB_NET_WWAN_DEV)
#define RDM_SIO_USB_NET_WWAN SIO_PORT_USB_NET_WWAN
#else
#define RDM_USB_NET_WWAN_BM  0
#define RDM_SIO_USB_NET_WWAN SIO_PORT_NULL
#endif
/*lint -save -e750*/
#ifdef FEATURE_APS
#define RDM_APS_SERIAL_VSP_BM   (1 << RDM_APS_SERIAL_VSP_DEV)
#define RDM_SIO_APS_SERIAL_VSP  SIO_PORT_APS_SERIAL_VSP
#else 
#define RDM_APS_SERIAL_VSP_BM   0
#define RDM_SIO_APS_SERIAL_VSP  SIO_PORT_NULL
#endif /* FEATURE_APS */
/*lint restore*/

/* The union of all the devices allowed         */
#define RDM_DEV_WHOLE_SET   (RDM_NULL_DEV_BM | RDM_UART1_BM | RDM_UART2_BM | \
                             RDM_UART3_BM |RDM_USB_MDM_BM | RDM_USB_MMC_BM | \
                             RDM_USB_SER1_BM | RDM_BT_NA_BM | \
                             RDM_SMEM_DS_BM | RDM_SMD_DS_BM | \
                             RDM_SMD_DATA1_BM | RDM_SMD_DATA2_BM | \
                             RDM_SMD_DATA3_BM | RDM_SMD_DATA4_BM | \
                             RDM_SMD_DATA11_BM | RDM_USB_SER2_BM | \
                             RDM_LSM_CSD_BM | RDM_BT_SPP_BM |\
                             RDM_USB_BREW_EXTENDED_DATA_BM |\
                             RDM_USB_BREW_EXTENDED_NOTIFICATION_BM |\
                             RDM_USB_NET_WWAN_BM)

/* check if dev and srv is compatible           */
#define RDM_SD_COMPAT(srv, dev)     (rdm_srv_dev_compat_tab[srv] & (1 << dev))

/* check if srv is using dev                    */
#define RDM_SRV_USE_DEV(srv, dev)   (rdm_get_multi_dev(srv) & (1 << dev))

/* used to specify all RDM devices              */
#define RDM_DEV_ALL (-1)

/* Device name string length */
#define MAX_DEVICE_NAME_LEN 13    

/*---------------------------------------------------------------------------
                          Local Typedefs
---------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------
  Type of DEVMAP State
--------------------------------------------------------------------------*/
typedef enum
{
  RDM_IDLE_STATE,               /* DEVMAP Idle, nothing in process       */
  RDM_CLOSING_STATE,            /* DEVMAP Closing a serial port          */
  RDM_OPENING_STATE,            /* DEVMAP Opening a serial port          */
  RDM_INIT_STATE,               /* DEVMAP restoring the previous setting */
  RDM_LOCKED_STATE              /* DEVMAP has been locked by client req  */
} rdm_state_enum_type;

/*---------------------------------------------------------------------------
  Type of DEVMAP state and related variable of its State Machine 
--------------------------------------------------------------------------*/
typedef struct {
  /* Current state variable for RDM state machine.      */
  rdm_state_enum_type   state;
  
  /* The next 4 fields are meaningful only when 
     rdm_state is in RDM_CLOSING_STATE or 
     RDM_CLOSING_STATE                                  */

  /* this is the service requesting the desired_device    
     Its init value is RDM_NULL_SRVC                    */
  rdm_service_enum_type service_requester;

  /* this is the device the service requester desires     
     Its init value is RDM_DEV_MAX                      */
  rdm_device_enum_type  desired_device;

  /* this the service that taking current action          
     Its init value is RDM_NULL_SRVC                    */
  rdm_service_enum_type inprocess_service;

  /* this is the device current action aims
     Its init value is RDM_DEV_MAX                      */
  rdm_device_enum_type inprocess_device;

  /* assign_port call back function to inform 
     service_requester of the assign_port status        */
  rdm_assign_port_cb_type cbptr;

  /* whether the transition should be persistent across
     reboots or not                                     */
  boolean is_persistent;

} rdm_state_type;

/*---------------------------------------------------------------------------
  Type of storage to keep the Services registered Open and Close routines.
---------------------------------------------------------------------------*/
typedef union {
  /* open routine support only one device for one service       */
  rdm_service_open_func_ptr_type        single_dev_p;

  /* open routine support multiple devices for one service      */
  rdm_srv_func_ptr_multi_dev_type       multi_dev_p;
} rdm_open_func_u_type;

typedef union {
  /* close routine support only one device for one service       */
  rdm_service_close_func_ptr_type       single_dev_p;

  /* close routine support multiple devices for one service      */
  rdm_srv_func_ptr_multi_dev_type       multi_dev_p;
} rdm_close_func_u_type;

typedef struct {
  /* test if the service cb functions are supporting multi 
     devices for the service                                            */
  boolean                   multi_dev;

  /* when multi_dev is TRUE, open_func union takes multi_dev_p
     when multi_dev is FALSE, open_func union takes single_dev_p        */
  rdm_open_func_u_type      open_func;

  /* when multi_dev is TRUE, close_func union takes multi_dev_p
     when multi_dev is FALSE, close_func union takes single_dev_p       */
  rdm_close_func_u_type     close_func;

  /* a user data, service register it to the RDM with open/close function
     pointers. RDM passes this pointer back to the service when calling
     open or closing function.
  */
  void *                data;
} rdm_srv_cb_type;

#ifdef FEATURE_USB_ON_APPS_PROC
typedef void (*usbotg_remote_vv_f_type) (void);
extern void usbotg_turn_on_apps_isr_control(void);
#endif /* FEATURE_USB_ON_APPS_PROC */

/*---------------------------------------------------------------------------
                          Local Variables
---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------
  Flag used for multi-processor builds when data is on applications processor
  to synchronize so that rdm_issue_opens isn't called until both processors
  have registered their open/close functions.  Both processors will call
  rdm_issue_opens, and when the second one does so, the function will
  actually be executed.
  Add the variable to the area that need to be re-initialized during
  modem restart
---------------------------------------------------------------------------*/ 
#ifdef RDM_SKIP_FIRST_ISSUE_OPEN
#if (defined(FEATURE_AMSS_RESTART) && !defined(FEATURE_NO_USB_HW_ON_PROCESSOR)) 
#pragma arm section rwdata = "RESTART_REGION"
static boolean rdm_first_issue_opens_call_made = FALSE;
#pragma arm section rwdata
#else
static boolean rdm_first_issue_opens_call_made = FALSE;
#endif
#endif


/*---------------------------------------------------------------------------
  Current state variable for RDM state machine.
  local variables that stores the Service task ID, in progress Service 
  task, and inprogress Device, and the final desired device during DEVMAP 
  state machine operation.
---------------------------------------------------------------------------*/
static rdm_state_type rdm_state = {
  RDM_IDLE_STATE,       /* state                */
  RDM_NULL_SRVC,        /* service_requester    */
  RDM_DEV_MAX,          /* desired_device       */
  RDM_NULL_SRVC,        /* inprocess_service    */
  RDM_DEV_MAX,          /* inprocess_device     */
  NULL                  /* cbptr                */
};
  
/*---------------------------------------------------------------------------
  Service Device Compatibility Table. This table specifies the serial port 
  devices that each Service Tasks can use. Its number is a uint32 bit mask.
  If a service, srv1, is compatible with port RDM_UART1_DEV and 
  RDM_USB_SER1_DEV for example, srv_dev_compat_tab[srv1] should be 
  set as ( (1 << RDM_UART1_DEV) | (1 << RDM_USB_SER1_DEV) ). 
---------------------------------------------------------------------------*/
static uint32 rdm_srv_dev_compat_tab[RDM_SRVC_MAX];

/*---------------------------------------------------------------------------
  Current Configuration Table. This table contains the current owner sevice
  for each device. If the port is not used, the value is RDM_NULL_SRVC.
---------------------------------------------------------------------------*/
static rdm_service_enum_type rdm_current_device_map[ RDM_DEV_MAX ];

/*---------------------------------------------------------------------------
  number of requests to the services to open previously opened devices 
  at the powerup time
---------------------------------------------------------------------------*/
static int rdm_req_init_num;

/*---------------------------------------------------------------------------
  Storage to keep the Services registered Open and Close routines.
---------------------------------------------------------------------------*/
static rdm_srv_cb_type rdm_srv_cbs[RDM_SRVC_MAX];

/*---------------------------------------------------------------------------
  Storage to keep track of rdm_device_enum_type to sio_port_id_type mapping.
  This table is for mapping the rdm_device_enum_type to sio_port_id_types. 
  Use SIO_PORT_NULL for unavailable ports.
---------------------------------------------------------------------------*/
static const sio_port_id_type rdm_device_to_port_id_table[RDM_DEV_MAX] =
{
  SIO_PORT_NULL,            
  RDM_SIO_UART1,            
  RDM_SIO_UART2,            
  RDM_SIO_UART3,            
  RDM_SIO_USB_MDM,         
  RDM_SIO_USB_MMC,        
  RDM_SIO_USB_SER1,      
  RDM_SIO_BT_NA,        
  RDM_SIO_SMEM_DS,     
  RDM_SIO_LSM_CSD,    
  RDM_SIO_USB_SER2,         
  RDM_SIO_BT_SPP,          
  RDM_SIO_USB_NET_WWAN,   
  RDM_SIO_SMD_DS,        
  RDM_SIO_SMD_DATA1,    
  RDM_SIO_SMD_DATA2,   
  RDM_SIO_SMD_DATA3,  
  RDM_SIO_SMD_DATA4, 
  RDM_SIO_SMD_DATA11,        /* QVP Data channel */ 
  RDM_SIO_APS_SERIAL_VSP,  
  RDM_SIO_SMD_NMEA,
  RDM_SIO_USB_BREW_EXTENDED_DATA,
  RDM_SIO_USB_BREW_EXTENDED_NOTIFICATION,
};


#ifdef FEATURE_EFS
/*---------------------------------------------------------------------------
  Where and what in EFS to keep port mappings across MSM reset's.
---------------------------------------------------------------------------*/
static fs_handle_type       rdm_efs_handle           = FS_NULL_HANDLE;
static const char           *rdm_efs_port_map_file   = "RDM_DEV_MAP";
#endif /* FEATURE_EFS */

/*---------------------------------------------------------------------------
                    Local Function Prototypes
---------------------------------------------------------------------------*/
LOCAL boolean rdm_assign_port_common
(
  /* Service Task desiring a device                     */
  rdm_service_enum_type         service,

  /* Device to give to service task                     */
  rdm_device_enum_type          device,

  /* Optional callback function to notify the status of 
     the assign action                                  */  
  rdm_assign_port_cb_type       cbptr,

  /* Whether this transition should persist across      */
  /* device resets.                                     */
  boolean                       is_persistent
);

static boolean rdm_start_open_device
(
  /* Service Task desiring a new port    */
  rdm_service_enum_type         service,

  /* Device port to give to Service task */
  rdm_device_enum_type          device,

  /* callback function         */  
  rdm_assign_port_cb_type       cbptr,

  /* is this transition persistent? */
  boolean                       is_persistent
);

static void rdm_reset_state_machine (void);

static boolean rdm_send_service_cmd
(
  /* the service executing the cmd              */
  rdm_service_enum_type  service, 

  /* the cmd                                    */
  rdm_command_enum_type  port_cmd,

  /* the device which the cmd act on            */
  rdm_device_enum_type   device
);

#ifdef FEATURE_EFS
LOCAL void rdm_update_efs_entry(int device_index);
#endif /* FEATURE_EFS */

static boolean rdm_notify_close
(
  /* Service Task desiring a new port                           */
  rdm_service_enum_type  service,

  /* status of requested task change                            */
  rdm_assign_status_type status,

  /* what to notify the service_requester if the port assign 
     is done or aborted                                         */
  rdm_assign_status_type * notify_ptr
);

static void rdm_remove_dup_mappings(void);

static boolean rdm_start_close_device
(
  /* this service whoes device is to be closed          */
  rdm_service_enum_type         service,

  /* device to be closed                                */
  rdm_device_enum_type          device,

  /* call back function                                 */
  rdm_assign_port_cb_type       cbptr
);

static boolean rdm_lock_state ( void );

void rdm_set_bt_mode(void);

/*===========================================================================

                           PUBLIC FUNCTIONS

===========================================================================*/

/*===========================================================================

FUNCTION RDM_INIT                                     EXTERNALIZED FUNCTION

DESCRIPTION
  Initializes the Runtime Device Mapper Services. This function must
  be called before other functions of this module.

DEPENDENCIES
  Should be called just once.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
/*lint -save -e641 -e539 -e64*/
void rdm_init(void)
{
  /* device index to traverse valid rdm_device_enum_type        */
  rdm_device_enum_type          dev_index;

/*------------------------------------------------------------------*/

  DEBUG(MSG_MED("rdm_init()", 0, 0, 0););

  /*-------------------------------------------------------------------------
    Initialize the Services Close & Open routines to NULL. If a Service in
    rdm_service_enum_type does not register these, they don't get to play.
  -------------------------------------------------------------------------*/
  memset(rdm_srv_cbs, 0, RDM_SRVC_MAX * sizeof(rdm_srv_cb_type));

  /* Set compatability of service and port                          */
  rdm_set_bt_mode();

  /* init rdm_current_device_map                                    */
  for(dev_index = RDM_FIRST_DEV; dev_index < RDM_DEV_MAX; dev_index++) /*lint !e539*/
  {
    rdm_current_device_map[dev_index] = RDM_NULL_SRVC;
  }

#ifdef FEATURE_DIAG_MP_MODEM_ONLY
#ifdef RDM_MPO_DEFAULT_DIAG_PORT
  #error "Outdated feature usage, please use RDM_xxx_INIT_SRVC instead"
#endif
#endif

#ifdef RDM_USB_MDM_INIT_SRVC
  rdm_current_device_map[RDM_USB_MDM_DEV]    = RDM_USB_MDM_INIT_SRVC;
#endif

#ifdef RDM_USB_SER1_INIT_SRVC
  rdm_current_device_map[RDM_USB_SER1_DEV]   = RDM_USB_SER1_INIT_SRVC;
#endif
  
#ifdef RDM_UART1_INIT_SRVC
  rdm_current_device_map[RDM_UART1_DEV]      = RDM_UART1_INIT_SRVC;
#endif

#ifdef RDM_UART2_INIT_SRVC
  rdm_current_device_map[RDM_UART2_DEV]      = RDM_UART2_INIT_SRVC;
#endif

#ifdef RDM_UART3_INIT_SRVC
  rdm_current_device_map[RDM_UART3_DEV]      = RDM_UART3_INIT_SRVC;
#endif

#ifdef RDM_SMD_DATA_INIT_SRVC
  rdm_current_device_map[RDM_SMD_DATA1_DEV]  = RDM_SMD_DATA_INIT_SRVC;
  rdm_current_device_map[RDM_SMD_DATA2_DEV]  = RDM_SMD_DATA_INIT_SRVC;
  rdm_current_device_map[RDM_SMD_DATA3_DEV]  = RDM_SMD_DATA_INIT_SRVC;
  rdm_current_device_map[RDM_SMD_DATA4_DEV]  = RDM_SMD_DATA_INIT_SRVC;
  rdm_current_device_map[RDM_SMD_DATA11_DEV] = RDM_SMD_DATA_INIT_SRVC;
#endif

#ifdef RDM_SMD_DS_INIT_SRVC
  rdm_current_device_map[RDM_SMD_DS_DEV]       = RDM_SMD_DS_INIT_SRVC;
#endif

#ifdef RDM_APS_VSP_INIT_SRVC
  rdm_current_device_map[RDM_APS_SERIAL_VSP_DEV] = RDM_APS_VSP_INIT_SRVC;
#endif

#ifdef RDM_USES_FSHS_USB
  rdm_current_device_map[RDM_USB_NET_WWAN_DEV]   = RDM_DATA_WWAN_SRVC;
#endif
} /* rdm_init() */
/*lint restore*/


/*===========================================================================

FUNCTION  RDM_SET_BT_MODE

DESCRIPTION
  The function sets which device is compatable with which port.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rdm_set_bt_mode(void)
{
  rdm_service_enum_type srv_index;

  INTLOCK();
  /*------------------------------------------------------------------
    set rdm_srv_dev_compat_tab, turn the bits on for the devices common 
    to the both mode
    ------------------------------------------------------------------*/
  /* Define what DEV's DIAG can use.                     
     UART1(DM) will be used for BT SoC support if
     FEATURE_BT_SOC_UART_TL is defined.                           */
  rdm_srv_dev_compat_tab[RDM_DIAG_SRVC] = (RDM_NULL_DEV_BM
#ifndef FEATURE_BT_SOC_UART_TL
    | RDM_UART1_BM 
#endif /* FEATURE_BT_SOC_UART_TL */
    | RDM_UART2_BM | RDM_UART3_BM | RDM_SMEM_DS_BM
#if (defined(FEATURE_BT) && defined(FEATURE_DIAG_SUPPORT_WIRELESS))
    | RDM_BT_SPP_BM                                           
#endif /* FEATURE_BT */
    );

  /* Define what DEV's DATA can use based on Feature    */
  rdm_srv_dev_compat_tab[RDM_DATA_SRVC] = (RDM_NULL_DEV_BM
#ifdef FEATURE_APS
    | RDM_APS_SERIAL_VSP_BM
#endif /* FEATURE_APS */

#if defined (FEATURE_DS) || defined (FEATURE_DATA)
#if !((defined FEATURE_BT_SOC_UART_TL) || (defined SIORS_UART1_USES_4WIRE_ONLY))
    | RDM_UART1_BM 
#endif 
#ifdef FEATURE_DS_MAPPABLE_TO_UART2
    | RDM_UART2_BM
#endif /* FEATURE_DS_MAPPABLE_TO_UART2 */

#ifdef FEATURE_DS_MAPPABLE_TO_UART3
    | RDM_UART3_BM
#endif /* FEATURE_DS_MAPPABLE_TO_UART3 */

    | RDM_BT_NA_BM | RDM_SMEM_DS_BM | RDM_LSM_CSD_BM | RDM_SMD_DS_BM
    | RDM_SMD_DATA1_BM | RDM_SMD_DATA2_BM | RDM_SMD_DATA3_BM 
    | RDM_SMD_DATA4_BM | RDM_SMD_DATA11_BM
#endif /* FEATURE_DS || FEATURE_DATA  */
    );

  /* Define what DEV's BT_HCI can use based on Feature  */
  rdm_srv_dev_compat_tab[RDM_BT_HCI_SRVC] = (RDM_NULL_DEV_BM
#ifdef FEATURE_BT
  #ifndef FEATURE_BT_SOC_UART_TL
    | RDM_UART1_BM 
  #endif /* FEATURE_BT_SOC_UART_TL */
    | RDM_UART2_BM | RDM_UART3_BM | RDM_USB_MDM_BM
#endif /* FEATURE_BT */
    );

#ifdef FEATURE_BT_SOC
  /* Define what DEV's BT_SOC can use based on Feature  */
  rdm_srv_dev_compat_tab[RDM_BT_SOC_SRVC] = (RDM_NULL_DEV_BM
  #ifdef FEATURE_BT_SOC_UART_TL
    | RDM_UART1_BM 
  #endif /* FEATURE_BT_SOC_UART_TL */
    );
#endif /* FEATURE_BT_SOC */

  /* Define what DEV's MMC can used                     */
  rdm_srv_dev_compat_tab[RDM_MMC_SRVC] = RDM_NULL_DEV_BM;

  /* Define what DEV's GPS NMEA can use based on Feature */
  rdm_srv_dev_compat_tab[RDM_NMEA_SRVC] = (RDM_NULL_DEV_BM
#if defined( FEATURE_GPSONE_PE ) || defined( FEATURE_MMGPS ) \
          || defined( FEATURE_CGPS )
#error code not present
#endif /* FEATURE_GPSONE_PE || FEATURE_MMGPS || FEATURE_CGPS */
    );

  /* Define what DEV's RPC can use base on FEATURE */
  rdm_srv_dev_compat_tab[RDM_RPC_SRVC] = (RDM_NULL_DEV_BM
#ifdef FEATURE_ONCRPC
#error code not present
#endif /* FEATURE_ONCRPC */
    );

  rdm_srv_dev_compat_tab[RDM_DATA_WWAN_SRVC] = (RDM_NULL_DEV_BM |
                                                RDM_USB_NET_WWAN_BM);

    /* Non BT mode USB_DIAG can be used for RDM_DIAG_SRVC       */
    rdm_srv_dev_compat_tab[RDM_DIAG_SRVC] |= RDM_USB_SER1_BM;

#if !(defined(FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA))
    /* when USB DIAG is not available for DIAG, DIAG can use USB_CDC */
    rdm_srv_dev_compat_tab[RDM_DIAG_SRVC] |= RDM_USB_MDM_BM;
#endif /* !(defined(FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA)) */

#if defined (FEATURE_DS) || defined (FEATURE_DATA)
    /* Non BT mode USB_CDC can be used for RDM_DATA_SRVC        */
    rdm_srv_dev_compat_tab[RDM_DATA_SRVC] |= RDM_USB_MDM_BM;
#endif /* FEATURE_DS || FEATURE_DATA  */

    /* Non BT mode RDM_USB_MMC_BM can be used for RDM_MMC_SRVC      */
    rdm_srv_dev_compat_tab[RDM_MMC_SRVC] |= RDM_USB_MMC_BM;
      
#ifdef FEATURE_ONCRPC
#error code not present
#endif /* FEATURE_ONCRPC */

    rdm_srv_dev_compat_tab[RDM_NMEA_SRVC] |= RDM_USB_SER2_BM;

  /* reserved OEM services cannot open any port until the OEM 
     registed its open/close functions and rdm_srv_dev_compat_mask.
   */    
  for ( srv_index = (rdm_service_enum_type)(RDM_LAST_KNOWN_SRVC+1); 
        srv_index < RDM_SRVC_MAX; srv_index++ ) 
  {
    rdm_srv_dev_compat_tab[srv_index] = RDM_NULL_DEV_BM;
  }
  INTFREE();

} 


/*===========================================================================

FUNCTION  RDM_GET_PREV_PORTS                            EXTERNALIZED FUNCTION

DESCRIPTION
  Read in the port mappings last know by RDM from EFS. 
  
  It is here that the compile time defaults for port mapping may be 
  overwritten by what is read from EFS at power up.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Port mappings before the last reset are restored at boot.

===========================================================================*/
void rdm_get_prev_ports(void)
{
#ifdef FEATURE_EFS

  fs_rsp_msg_type  fs_rsp;

  /* get device-service map from efs and save it in this array  */
  rdm_service_enum_type         efs_dev_map[RDM_DEV_MAX];

  /* device index to traverse valid rdm_device_enum_type        */
  rdm_device_enum_type          dev_index;

  /* previous service for a dev_index                           */
  rdm_service_enum_type         prev_srv;

  /* efs file open options                                      */
  fs_open_xparms_type           efs_open_options;

/*------------------------------------------------------------------*/

  DEBUG( MSG_MED("rdm_get_prev_ports()",0,0,0); );
  
  /*------------------------------------------------
    Test to see if we have a port map file in EFS.
  ------------------------------------------------*/
  fs_nametest(rdm_efs_port_map_file, FS_TEST_FILE, NULL, &fs_rsp);

  /* we are going to try creat/open the file, set efs_open_options */ 
  efs_open_options.create.cleanup_option   = FS_OC_CLOSE;
  efs_open_options.create.buffering_option = FS_OB_PROHIBIT;
  efs_open_options.create.attribute_mask   = FS_FA_UNRESTRICTED;
  efs_open_options.create.base_address     = 0;

  /* if we found the file, we will open the file,
     otherwise we create a file                         */
  if( (fs_rsp.nametest.status == FS_OKAY_S) && (fs_rsp.nametest.name_found) )
  {
    /*-------------------------------------------------
      File already exist. Open w/read & write access
    -------------------------------------------------*/
    fs_open(rdm_efs_port_map_file, FS_OA_READWRITE, &efs_open_options,
            NULL, &fs_rsp);

    /* if the file is opened correctly, we read it into 
       current_device _map, otherwise rdm_current_device_map is untouched 
       as set by rdm_init                                               */
    if( ((rdm_efs_handle = fs_rsp.open.handle) != FS_NULL_HANDLE) && 
        (fs_rsp.any.status == FS_OKAY_S) )
    {
      /* init efs_dev_map                                            */
      for(dev_index = RDM_FIRST_DEV; dev_index < RDM_DEV_MAX; dev_index++)
      {
        efs_dev_map[dev_index] = RDM_NULL_SRVC;
      }

      /* Read into efs_dev_map                                          */
      fs_seek_read(rdm_efs_handle, FS_SEEK_SET, 0, efs_dev_map,
                   RDM_DEV_MAX, NULL, &fs_rsp);

      if (fs_rsp.read.status == FS_OKAY_S)
      {
        /*--------------------------------------------------------
          Check SRVC's in file from EFS & see if allowed now.
          A new build could have been loaded since the last and
          many devices may not be allowed now.
          Traverve device from RDM_UART1_DEV (RDM_FIRST_DEV + 1) to 
          (RDM_DEV_MAX - 1)
        --------------------------------------------------------*/
        for(dev_index = (rdm_device_enum_type)(RDM_FIRST_DEV + 1);
            dev_index < RDM_DEV_MAX; dev_index++)
        {
          prev_srv = efs_dev_map[dev_index];

          /*-------------------------------------------------------------
             Check if previous port map for each port is allowed now.
             A service availabe in an old build maybe gone in a new build. 
             In this case, it is not allowed to map to any device.  
           --------------------------------------------------------------*/   
          if ( (prev_srv < RDM_NULL_SRVC || prev_srv >= RDM_SRVC_MAX) ||
               (prev_srv != RDM_NULL_SRVC && 
                !RDM_SD_COMPAT(prev_srv, dev_index)) 
             )
          {
            efs_dev_map[dev_index] = RDM_NULL_SRVC;
          } 
          /*-------------------------------------------------------------
            BREW srvc should hold no ports at bootup
           ------------------------------------------------------------*/ 
          if (prev_srv == RDM_BREW_SRVC ||
              prev_srv == RDM_BREW_EXTENDED_SRVC)
          {
            efs_dev_map[dev_index] = RDM_NULL_SRVC;
          }
        }
#if defined(FEATURE_SMD)
#if defined(FEATURE_DATA_SERIALIZER) && defined(FEATURE_DATA_MULTI_PORTS)
        efs_dev_map[RDM_SMD_DS_DEV]     = RDM_DATA_SRVC;
        efs_dev_map[RDM_SMD_DATA1_DEV]  = RDM_DATA_SRVC;
        efs_dev_map[RDM_SMD_DATA2_DEV]  = RDM_DATA_SRVC;
        efs_dev_map[RDM_SMD_DATA3_DEV]  = RDM_DATA_SRVC;
        efs_dev_map[RDM_SMD_DATA4_DEV]  = RDM_DATA_SRVC;
#endif /* FEATURE_DATA_SERIALIZER && FEATURE_DATA_MULTI_PORTS */
        efs_dev_map[RDM_SMD_DATA11_DEV] = RDM_DATA_SRVC;
#endif /* FEATURE_SMD */
        
        memcpy(rdm_current_device_map, efs_dev_map, 
               RDM_DEV_MAX * sizeof(rdm_service_enum_type));

#ifdef CUST_EDITION
// 修正UDISK设置之后再也找不到DIAG口的问题
#ifdef RDM_USB_MDM_INIT_SRVC
        rdm_current_device_map[RDM_USB_MDM_DEV]    = RDM_USB_MDM_INIT_SRVC;
#endif

#ifdef RDM_USB_SER1_INIT_SRVC
        rdm_current_device_map[RDM_USB_SER1_DEV]   = RDM_USB_SER1_INIT_SRVC;
#endif
#endif //CUST_EDITION

#ifndef FEATURE_SECOND_UART_ON_ANY_PROC     /* No U2                */
        rdm_current_device_map[RDM_UART2_DEV] = RDM_NULL_SRVC;
#endif /* FEATURE_SECOND_UART_ON_ANY_PROC */
        
#ifndef FEATURE_THIRD_UART_ON_ANY_PROC      /* No U3                */
        rdm_current_device_map[RDM_UART3_DEV] = RDM_NULL_SRVC;
#endif /* FEATURE_THIRD_UART_ON_ANY_PROC */
        
#ifndef FEATURE_USB_CDC_ACM     /* No USB_CDC_ACM       */
        rdm_current_device_map[RDM_USB_MDM_DEV] = RDM_NULL_SRVC;
#endif /* FEATURE_USB_CDC_ACM */

        /* USB_MMC_DEV is not allowed to use at boot up */ 
        rdm_current_device_map[RDM_USB_MMC_DEV] = RDM_NULL_SRVC;

#if !defined(RDM_USES_FSHS_USB) || \
    !(defined(FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA))
        rdm_current_device_map[RDM_USB_SER1_DEV] = RDM_NULL_SRVC;
#endif /* !defined(FEATURE_USB) || !defined(FEATURE_USB_DIAG) */
        
#ifndef FEATURE_BT              /* No BT_NA nor BT_SPP  */
        rdm_current_device_map[RDM_BT_NA_DEV] = RDM_NULL_SRVC;
        rdm_current_device_map[RDM_BT_SPP_DEV] = RDM_NULL_SRVC;
#endif /* FEATURE_BT */

#ifndef FEATURE_SMEM_DS         /* No SMEM_DS           */
        rdm_current_device_map[RDM_SMEM_DS_DEV] = RDM_NULL_SRVC;
#endif /* FEATURE_SMEM_DS */

#ifndef FEATURE_SMD             /* No SMD  */
        rdm_current_device_map[RDM_SMD_DS_DEV] = RDM_NULL_SRVC;
#endif /* FEATURE_SMD */

#if !defined(FEATURE_SMD) && !defined(FEATURE_DATA_SERIALIZER) && !defined(FEATURE_DATA_MULTI_PORTS)
        rdm_current_device_map[RDM_SMD_DATA1_DEV] = RDM_NULL_SRVC;
        rdm_current_device_map[RDM_SMD_DATA2_DEV] = RDM_NULL_SRVC;
        rdm_current_device_map[RDM_SMD_DATA3_DEV] = RDM_NULL_SRVC;
        rdm_current_device_map[RDM_SMD_DATA4_DEV] = RDM_NULL_SRVC;
#endif          
#if !defined(FEATURE_SMD)
        rdm_current_device_map[RDM_SMD_DATA11_DEV] = RDM_NULL_SRVC;
#endif          

#ifndef FEATURE_MMGPS_CSD       /* No MMGPS_CSD         */
        rdm_current_device_map[RDM_LSM_CSD_DEV] = RDM_NULL_SRVC;
#endif /* FEATURE_MMGPS_CSD */

#ifndef FEATURE_DATA_RM_NET
        rdm_current_device_map[RDM_USB_NET_WWAN_DEV] = RDM_NULL_SRVC;
#endif /* FEATURE_DATA_RM_NET */

#ifdef FEATURE_BT_SOC_UART_TL
        rdm_current_device_map[RDM_UART1_DEV]        = RDM_BT_SOC_SRVC;
#endif /* FEATURE_BT_SOC_UART_TL */
      }
      else /* (fs_rsp.read.status == FS_OKAY_S) */
      {
        ERR("fs_open: errcode %d", (int) fs_rsp.read.status, 0, 0);

        /* Close the flash file */
        fs_close (rdm_efs_handle, NULL, &fs_rsp);
        if (fs_rsp.close.status != FS_OKAY_S)
        {
          ERR("fs_close: errcode %d", (int) fs_rsp.close.status, 0, 0);
        }
      } /* else of (fs_rsp.read.status == FS_OKAY_S) */

    } /* file opened */
  } /* file exists */
  else
  {
    /*----------------------------------------------------------
      File doesn't exist yet, create one w/read & write access
    ----------------------------------------------------------*/
    fs_open(rdm_efs_port_map_file, FS_OA_CREATE, &efs_open_options,
            NULL, &fs_rsp);

    if((rdm_efs_handle = fs_rsp.open.handle) == FS_NULL_HANDLE ||
        fs_rsp.any.status != FS_OKAY_S)
    {
      ERR("can't create efs file, %d",fs_rsp.any.status,0,0);
    }

    /* Since the file is blank, update it with default port mapping */
    rdm_update_efs_entry(RDM_DEV_ALL);
  } /* created file */

#endif /* FEATURE_EFS */
} /* rdm_get_prev_ports() */

  
#ifdef FEATURE_DATA_RM_NET
/*============================================================================

FUNCTION  RDM_UPDATE_SRV_DEV_COMPAT_TABLE                    INTERNAL FUNCTION

DESCRIPTION
  update rdm_srv_dev_compat_table according to network_device_flag. 
  
  If USB is NETWORK device, USB_SER2 port/interface is not available to any service.
  If USB is MODEM device, USB_NET_WWAN interface is not available to any service.  
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  rdm_srv_dev_compat_table can be modified
=============================================================================*/
void   rdm_update_srv_dev_compat_table(boolean network_device_flag)
{
  uint32  markoff_mask;
  int  i;
  if (network_device_flag)
  {
    markoff_mask = ~RDM_USB_SER2_BM;
  }
  else
  {
    markoff_mask = ~RDM_USB_NET_WWAN_BM;
  }
  for (i = RDM_FIRST_SRVC; i<= RDM_LAST_KNOWN_SRVC; i++ )
  {
    rdm_srv_dev_compat_tab[i] &= markoff_mask;
  }
}
/*============================================================================

FUNCTION  RDM_UPDATE_CURRENT_DEVICE_MAP                      INTERNAL FUNCTION

DESCRIPTION
  update rdm_current_device_map according to network_device_flag. 
  
  If USB is NETWORK device, USB_SER2 is mapped to NULL service.
  If USB is MODEM device, USB_NET_WWAN is mapped to NULL service.  
  
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  rdm_current_device_map is modified. 
=============================================================================*/
void  rdm_update_current_device_map(boolean network_device_flag)
{
   if (network_device_flag)
   {
     rdm_current_device_map[RDM_USB_SER2_DEV] = RDM_NULL_SRVC;
     rdm_current_device_map[RDM_USB_NET_WWAN_DEV] = RDM_DATA_WWAN_SRVC;
   }
   else
   {
      rdm_current_device_map[RDM_USB_NET_WWAN_DEV] = RDM_NULL_SRVC;
   }
}
#endif
/*===========================================================================

FUNCTION  RDM_ISSUE_OPENS                               EXTERNALIZED FUNCTION

DESCRIPTION
  RDM issues opens to SRVCs at the power up time based on last mapping 
  before power down.

  Services using the RDM should not just open a port when the MSM boots.
  RDM will call the registered open routines for the SRVC's if they should
  open a port at boot.

  If EFS is being used, whatever the previous mapping was will be reinstated.
  If there was no previous mapping, DS will open UART1, and DIAG will open
  UART2 if it is defined.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Instruct tasks to open specific shared serial devices.
===========================================================================*/
void rdm_issue_opens(void)
{
  /* index to traverse rdm_device_enum_type             */
  rdm_device_enum_type dev_index;

  /* service mapped to dev_index                        */
  rdm_service_enum_type srv;

/*------------------------------------------------------------------*/

  DEBUG(MSG_MED("rdm_issue_opens()",0,0,0););

#ifdef RDM_SKIP_FIRST_ISSUE_OPEN
  if (!rdm_first_issue_opens_call_made)
  {
    rdm_first_issue_opens_call_made = TRUE;
    MSG_HIGH ("Ignoring first rdm_issue_opens() call",0,0,0);
    return;
  }
#endif

  rdm_remove_dup_mappings();

  ASSERT(rdm_state.state == RDM_IDLE_STATE);
  rdm_state.state = RDM_INIT_STATE;

  /* init rdm_req_init_num          */
  rdm_req_init_num = 0;

  /* we will decides how many openings will be issued by traverving 
     from RDM_UART1_DEV (RDM_FIRST_DEV + 1) to (RDM_DEV_MAX - 1)        */
  for(dev_index = (rdm_device_enum_type)(RDM_FIRST_DEV + 1);
      dev_index < RDM_DEV_MAX; dev_index++)
  {
    srv = rdm_get_service(dev_index);

    if(srv != RDM_NULL_SRVC)
    {
      /* see how many requests we are going to make     */
      ASSERT( (srv >= RDM_FIRST_SRVC) && (srv < RDM_SRVC_MAX) );

      /* check if the service has registered open/close functions */
      if(rdm_srv_cbs[srv].multi_dev)
      {
        /* the service uses multi_dev interface         */
        if( rdm_srv_cbs[srv].open_func.multi_dev_p && 
            rdm_srv_cbs[srv].close_func.multi_dev_p   )
        {
          rdm_req_init_num++;
        }
        else
        {
          /* the src can not use the device             */
          rdm_current_device_map[dev_index] = RDM_NULL_SRVC;
        }
      }
      else
      {
        /* the service uses single_dev interface        */
        if( rdm_srv_cbs[srv].open_func.single_dev_p &&
            rdm_srv_cbs[srv].close_func.single_dev_p   )
        {
          rdm_req_init_num++;
        }
        else
        {
          /* the src can not use the device                 */
          rdm_current_device_map[dev_index] = RDM_NULL_SRVC;
        }
      }
    }
  }

  if(rdm_req_init_num > 0)
  {
    /* Then we open the devices by traversing from 
     RDM_UART1_DEV (RDM_FIRST_DEV + 1) to (RDM_DEV_MAX - 1) */
    for(dev_index = (rdm_device_enum_type)(RDM_FIRST_DEV + 1);
        dev_index < RDM_DEV_MAX; dev_index++)
    {
      srv = rdm_get_service(dev_index);

      if(srv != RDM_NULL_SRVC)
      {
        (void) rdm_send_service_cmd(srv, RDM_OPEN_PORT, dev_index);
      }
    }
  }
  else
  {
    /* we return to RDM_IDLE_STATE from RDM_INIT_STATE  */
    rdm_state.state = RDM_IDLE_STATE;
  }

} /* rdm_issue_opens() */


/*===========================================================================

FUNCTION RDM_ASSIGN_PORT                                EXTERNALIZED FUNCTION

DESCRIPTION
  This function initiates the DEVMAP state machine to change the current 
  serial port configuration to give the specified serial port to the 
  specified Service task. This function will perform checks to ensure that 
  the specified configuration is proper.

  The actual port change may or may not be completed prior to this function
  return, depending upon the priorities of the tasks involved.

  If the device is RDM_NULL_DEV, this function tries to close the first
  device in the rdm_device_enum_type which the service is using. 
  Please use rdm_close_device if you want to specify which port to close.

  This function records the transition to the EFS so that the assignment
  persists across power cycles.

  Since RDM hasn't its own task, this function run's in caller's context,
  Don't call it in ISR.
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean 
    True : if the mapping is valid and will be attempted.
           cbptr is called immediatly before the function returns
           if the port mapping is already set as requested.
           cbptr will be called later to inform the mapping status 
           otherwise.
    False: if the requested mapping is not valid, RDM is busy, or
           the function is call during an ISR.
           cbptr is called immediately to reject the request before
           the function returns.

SIDE EFFECTS
  This function may cause a task context switch prior to returning.
===========================================================================*/
boolean rdm_assign_port
(
  /* Service Task desiring a device                     */
  rdm_service_enum_type         service,

  /* Device to give to service task                     */
  rdm_device_enum_type          device,

  /* Optional callback function to notify the status of 
     the assign action                                  */  
  rdm_assign_port_cb_type       cbptr
)
{
   return rdm_assign_port_common(service,device,cbptr,TRUE);
}

/*===========================================================================

FUNCTION RDM_ASSIGN_PORT_TMP                            EXTERNALIZED FUNCTION

DESCRIPTION
  This function initiates the DEVMAP state machine to change the current 
  serial port configuration to give the specified serial port to the 
  specified Service task. This function will perform checks to ensure that 
  the specified configuration is proper.

  The actual port change may or may not be completed prior to this function
  return, depending upon the priorities of the tasks involved.

  If the device is RDM_NULL_DEV, this function tries to close the first
  device in the rdm_device_enum_type which the service is using. 
  Please use rdm_close_device if you want to specify which port to close.

  This function does not record the port transition to the EFS.  It is
  thus a non-persistent assignment.

  Since RDM hasn't its own task, this function run's in caller's context,
  Don't call it in ISR.
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean 
    True : if the mapping is valid and will be attempted.
           cbptr is called immediatly before the function returns
           if the port mapping is already set as requested.
           cbptr will be called later to inform the mapping status 
           otherwise.
    False: if the requested mapping is not valid, RDM is busy, or
           the function is call during an ISR.
           cbptr is called immediately to reject the request before
           the function returns.

SIDE EFFECTS
  This function may cause a task context switch prior to returning.
===========================================================================*/
boolean rdm_assign_port_tmp
(
  /* Service Task desiring a device                     */
  rdm_service_enum_type         service,

  /* Device to give to service task                     */
  rdm_device_enum_type          device,

  /* Optional callback function to notify the status of 
     the assign action                                  */  
  rdm_assign_port_cb_type       cbptr
)
{
   return rdm_assign_port_common(service,device,cbptr,FALSE);
}

/*===========================================================================

FUNCTION RDM_ASSIGN_PORT_COMMON

DESCRIPTION
  Port assignment code shared between persistent and non-persistent forms
  of the function.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the operation succeeded
  FALSE otherwise

SIDE EFFECTS
  None
===========================================================================*/
LOCAL boolean rdm_assign_port_common
(
  /* Service Task desiring a device                     */
  rdm_service_enum_type         service,

  /* Device to give to service task                     */
  rdm_device_enum_type          device,

  /* Optional callback function to notify the status of 
     the assign action                                  */  
  rdm_assign_port_cb_type       cbptr,

  /* Whether this transition should persist across      */
  /* device resets.                                     */
  boolean                       is_persistent
)
{
  /* result of the function return              */
  boolean               result;

/*------------------------------------------------------------------*/

  DEBUG(MSG_MED("rdm_assign_port(SRVC=%d, DEV=%d, CB=%x)",
                service, device, cbptr););

  /* Make sure we are not called in ISR         */
  if (rex_is_in_irq_mode())
  {
    ERR("rdm_assign_port called in ISR",0,0,0);
    if(cbptr)
    {
      cbptr(RDM_RESTRICTED_S, service, device);
    }
    return FALSE;
  }

  /* Check if service & device are w/i range    */
  if(device < RDM_NULL_DEV || device >= RDM_DEV_MAX || 
     service < RDM_FIRST_SRVC || service >= RDM_SRVC_MAX)
  {
    if(cbptr)
    {
      cbptr(RDM_NOT_ALLOWED_S, service, device);
    }
    return FALSE;
  }

  /* Make sure the requested SRVC/DEV combo 
     is allowed                                 */
  if(!RDM_SD_COMPAT(service, device) )
  {
    if(cbptr)
    {
      cbptr(RDM_NOT_ALLOWED_S, service, device);
    }

    return FALSE;
  }

  /* check if the rdm is busy                    */
  if(!rdm_lock_state())
  {
    /* DEVMAP state machine in process, inform device_requester that
       we are busy                                                      */
    if(cbptr)
    {
      cbptr(RDM_DEVMAP_BUSY_S, service, device);
    }

    return FALSE;
  }

  /*------------------------------------------------------------------
    we passed the parameter and state checking 
    ------------------------------------------------------------------*/

  if(device == RDM_NULL_DEV)
  {
    /* Special case, the request is to just close down a service's port */
    rdm_device_enum_type        first_dev;

    first_dev   = rdm_get_device(service);

    if(first_dev == RDM_NULL_DEV)
    {
      /* we are ready to service next request           */
      rdm_reset_state_machine();

      /* The service already has no port/device open... */
      if(cbptr)
      {
        cbptr(RDM_DONE_S, service, RDM_NULL_DEV);
      }

      result = TRUE;
    }
    else
    {
      rdm_state.is_persistent           = is_persistent;
      result = rdm_start_close_device(service, first_dev, cbptr);
    }
  }
  else
  {
    /* device which the service is using        */
    rdm_device_enum_type          srv_dev;

    /* if service support only one device openning, we need to close
       its old port before openning a new one   */
    srv_dev = rdm_get_device(service);

    if( (srv_dev != RDM_NULL_DEV) && (srv_dev != device) &&
        !rdm_srv_cbs[service].multi_dev )
    {

      /* the service is using a device which is not the reqested 
         device to be opened and the service is not able to open 
         multiple device.  RDM will close the service's current 
         device then request opening of the new device          */

      rdm_state.state                   = RDM_CLOSING_STATE;
      rdm_state.service_requester       = service;
      rdm_state.desired_device          = device;
      rdm_state.inprocess_service       = service;
      rdm_state.inprocess_device        = srv_dev;

      rdm_state.cbptr                   = cbptr;
      rdm_state.is_persistent           = is_persistent;

      /* close the device the service holds */
      if(rdm_send_service_cmd(rdm_state.inprocess_service, 
                              RDM_CLOSE_PORT, rdm_state.inprocess_device))
      { 
        result = TRUE;
      }
      else
      {
        rdm_assign_port_cb_type notify_cb = rdm_state.cbptr;

        /* we are ready to service next request           */
        rdm_reset_state_machine();

        /* inform the requester of the failure */
        if(notify_cb)
        {
          notify_cb(RDM_NOT_ALLOWED_S, service, RDM_NULL_DEV);
        }
        
        result = FALSE;
      }
    }
    else
    {
      /* open the device for the service */
      result = rdm_start_open_device(service, device, cbptr, is_persistent);
    }
  } /* else (device == RDM_NULL_DEV) */

  return result;
} /* rdm_assign_port */


/*===========================================================================

FUNCTION rdm_close_device                               EXTERNALIZED FUNCTION

DESCRIPTION
  This function will close a device used by a service and map the device
  to NULL service.
  
DEPENDENCIES
  None

RETURN VALUE
  Boolean 
    True : the closing action is made
    False: If the device or service is not valid or
           If the service is not using the device or
           If the function is call in an ISR

SIDE EFFECTS
  This function may cause a task context switch prior to returning.
===========================================================================*/
boolean rdm_close_device
(
  /* Service Task request closing of the device         */
  rdm_service_enum_type         service,

  /* Device/port to be closed                           */
  rdm_device_enum_type          device,

  /* Optional callback function to inform the result of
     the closing action                                 */  
  rdm_assign_port_cb_type       cbptr
)
{
  /* result of the function return              */
  boolean               result;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DEBUG(MSG_MED("rdm_close_device(SRVC=%d, DEV=%d, CB=%x)",
                service, device, cbptr););

  /* Make sure we are not called in an ISR      */
  if (rex_is_in_irq_mode())
  {
    ERR("rdm_close_device called in ISR",0,0,0);
    if(cbptr)
    {
      cbptr(RDM_RESTRICTED_S, service, device);
    }
    return FALSE;
  }

  /* Check if service & device are w/i range    */
  if(device <= RDM_FIRST_DEV || device >= RDM_DEV_MAX || 
     service < RDM_FIRST_SRVC || service >= RDM_SRVC_MAX)
  {
    if(cbptr)
    {
      cbptr(RDM_NOT_ALLOWED_S, service, device);
    }
    return FALSE;
  }

  /* check if service is using the device       */
  if ( !RDM_SRV_USE_DEV(service, device) )
  {
    if(cbptr)
    {
      cbptr(RDM_NOT_ALLOWED_S, service, device);
    }
    return FALSE;
  }

  /* check if the rdm is busy                   */
  if(!rdm_lock_state())
  {
    /* DEVMAP state machine in process, inform device_requester that
       we are busy                                                      */
    if(cbptr)
    {
      cbptr(RDM_DEVMAP_BUSY_S, service, device);
    }

    return FALSE;
  }

  /*------------------------------------------------------------------
    we passed the parameter and state checking 
    ------------------------------------------------------------------*/
  result = rdm_start_close_device(service, device, cbptr);
  
  return result;
} /* rdm_close_device */

#ifdef FEATURE_USB_ON_APPS_PROC
/*===========================================================================

FUNCTION rdm_notify_cb_helper_f

DESCRIPTION
  This function calls the pm_app_otg_set_remote_a_dev_info 
  which is the real callback function registered from the applications
  processor.  It uses the parameters sent with the message  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Makes an RPC call to the applications processor.
===========================================================================*/
void 
rdm_notify_cb_helper_f
(
  /* ONCRPC Proxy command message */
  oncrpc_proxy_cmd_client_call_type *msg
)
{
  rdm_assign_port_cb_type notify_cb = (rdm_assign_port_cb_type)msg->data[0];
  rdm_assign_status_type notify_s = (rdm_assign_status_type)msg->data[1];
  rdm_service_enum_type   srv_req = (rdm_service_enum_type)msg->data[2];
  rdm_device_enum_type    dev_req = (rdm_device_enum_type)msg->data[3];

  /* Make call to callback with parameter data */
  notify_cb(notify_s, srv_req, dev_req);

} /* usbsdms_rdm_notify_helper_f */



/*===========================================================================

FUNCTION rdm_notify_cb_handler

DESCRIPTION
  This function is the callback that is registered locally with the
  pm_app_otg_set_remote_a_dev_info() function.  When called, this function
  will save parameter information and queue a command with the RPC 
  proxy task to call the real callback from the applications processor.
  This is necessary because this callback is called from interrrupt space
  and we cannot make RPC calls from interrupt space.    

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sends a message to RPC proxy task.
===========================================================================*/
void
rdm_notify_cb_handler
(
  rdm_assign_port_cb_type notify_cb,
  rdm_assign_status_type notify_s,
  rdm_service_enum_type   srv_req,
  rdm_device_enum_type    dev_req
)
{
  /* ONCRPC Proxy command structure */
  oncrpc_proxy_cmd_client_call_type *rdm_notify_cb_msg_ptr;

  rdm_notify_cb_msg_ptr = remote_apis_client_call_get();

  /* Check that the message pointer is valid */
  ASSERT (rdm_notify_cb_msg_ptr != NULL);

  if( rdm_notify_cb_msg_ptr != NULL )
  {

    /* Fill up the pointer data */
    rdm_notify_cb_msg_ptr->client_call =
      (void(*)( struct oncrpc_proxy_cmd_client_call_type *ptr ))
        rdm_notify_cb_helper_f;

    /* Send parameter data with message */
    rdm_notify_cb_msg_ptr->data[0] = (uint32)notify_cb;
    rdm_notify_cb_msg_ptr->data[1] = (uint32)notify_s;
    rdm_notify_cb_msg_ptr->data[2] = (uint32)srv_req;
    rdm_notify_cb_msg_ptr->data[3] = (uint32)dev_req;

    /* Queue up the message */
    oncprc_proxy_client_call_send( rdm_notify_cb_msg_ptr );

    /* Explicitly set to NULL */
    rdm_notify_cb_msg_ptr = NULL;
  }
} /* rdm_notify_cb_handler */
#endif /*FEATURE_USB_ON_APPS_PROC*/


/*===========================================================================

FUNCTION RDM_NOTIFY                                     EXTERNALIZED FUNCTION

DESCRIPTION
  This function will notify to the DEVMAP state machine the specified result
  of a requested serial port change. This function performs a portion of the
  DEVMAP state machine.

DEPENDENCIES
  Should not be called in ISR.

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rdm_notify
(
  rdm_service_enum_type  service,    /* Service Task desiring a new port    */
  rdm_assign_status_type status      /* status of requested task change     */
)
{

  /* determine if RDM is done                   */
  boolean       port_mapping_done = FALSE;

  /* the status to notify the requester, init it to be status, 
     it is meaningful only when port_mapping_done is TRUE */
  rdm_assign_status_type notify_s = status;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  DEBUG(MSG_MED("rdm_notify(SRVC=%d, STATUS=%d)", service, status, 0););

  /* make sure the function is called in correct state and is called
     by right task */
  ASSERT( (rdm_state.state == RDM_INIT_STATE) ||
          ( (rdm_state.state == RDM_CLOSING_STATE || 
             rdm_state.state == RDM_OPENING_STATE) && 
            (rdm_state.inprocess_service == service)) );

  /* Make sure we are not called in ISR */
  if (rex_is_in_irq_mode())
  {
    ERR("rdm_notify called in ISR",0,0,0);
  }

  /*----------------------------------------------------
    This is the scheduled notification operation. Proceed
    to continue DEVMAP state machine processing
    ----------------------------------------------------*/
  switch(rdm_state.state)
  {
    case RDM_CLOSING_STATE:
      port_mapping_done = rdm_notify_close(service, status, &notify_s);
      break;

    case RDM_OPENING_STATE:
      if( status == RDM_DONE_S )
      {
        ASSERT( (rdm_state.inprocess_device > RDM_FIRST_DEV) && 
                (rdm_state.inprocess_device < RDM_DEV_MAX));
        /*-------------------------------------------------------
          Update the current config table, if opened, the service
          will be using the new port.
        -------------------------------------------------------*/
        rdm_current_device_map[rdm_state.inprocess_device] = 
          rdm_state.inprocess_service;

#ifdef FEATURE_EFS
        /* inprocess_service opened inprocess_device, update the maps */
        rdm_update_efs_entry(rdm_state.inprocess_device);
#endif /* FEATURE_EFS */
      }
 
      /*----------------------------------------------------------------
      Attempt made at opening Desired port now done.
      ----------------------------------------------------------------*/
      port_mapping_done = TRUE;
      break;

    case RDM_INIT_STATE:
      /* we have one opening done at power up */
      if( status != RDM_DONE_S)
      {
        rdm_device_enum_type device;
        /* Remove the port mapping of the device fail to open */
        device = rdm_get_device(service);
        rdm_current_device_map[device] = RDM_NULL_SRVC;
        ERR("srv: %d fail to open port", service, 0,0);
      }

      /* set/test in atomity */
      TASKLOCK();
      
      if(--rdm_req_init_num == 0)
      {
        /* all the open of previous port has been done, 
           update the port mapping                      */

        port_mapping_done = TRUE;
      }

      TASKFREE();

      /* If we created a port map file in EFS for the first time, we need to
         write out the current mapping. If we did not, the file will be there
         next time, but empty. This will assign all the devices to NULL on the
         next boot (unless the UI was used for port mapping a SRVC).
      */
#ifdef FEATURE_EFS
      if(port_mapping_done)
      {
        rdm_update_efs_entry(RDM_DEV_ALL);
      }
#endif /* FEATURE_EFS */
      break;

    default:
      ERR( "Invalid DEVMAP State: %d", rdm_state.state, 0, 0);
      break;
  }

  /* If we are done mapping the port for whatever reason, 
     notify the requester */
  if (port_mapping_done)
  {
    rdm_assign_port_cb_type notify_cb = rdm_state.cbptr;
    rdm_service_enum_type   srv_req   = rdm_state.service_requester;
    rdm_device_enum_type    dev_req   = rdm_state.desired_device;

    /* we are ready to service next request           */
    rdm_reset_state_machine();

    if(notify_cb)
    {
#if (!defined(FEATURE_DIAG_MP_MASTER_MODEM) && defined(FEATURE_MULTIPROCESSOR)) && \
    (!defined(FEATURE_STANDALONE_MODEM)) && (defined(FEATURE_USB_ON_APPS_PROC))
#error code not present
#else
      notify_cb(notify_s, srv_req, dev_req);
#endif 
    }

  }
} /* rdm_notify */


/*===========================================================================

FUNCTION RDM_GET_DEVICE                                 EXTERNALIZED FUNCTION

DESCRIPTION
  Query the Runtime Device Mapper for the port/device currently being used
  by the specified application/service.

  It returns the first device a service owns. This is good enough for service
  which can hold only one device.

  Please use rdm_get_multi_dev to get all the device a service holds.

DEPENDENCIES
  None

RETURN VALUE
  rdm_device_enum_type : the device which the service holds.

SIDE EFFECTS
  None

===========================================================================*/
rdm_device_enum_type rdm_get_device
(
  /* Service to query what device it may be using, if any */
  rdm_service_enum_type service 
)
{
  /* device index to traverse valid devices                     */
  rdm_device_enum_type  dev_index;

/*------------------------------------------------------------------*/

  if(service < RDM_FIRST_SRVC || service >= RDM_SRVC_MAX)
  {
    ERR("wrong srv: %d", service,0,0);
    return RDM_NULL_DEV;
  }

  /* Test devices from RDM_UART1_DEV (RDM_FIRST_DEV + 1) to 
     RDM_LSM_CSD_DEV (RDM_DEV_MAX - 1)                          */
  for(dev_index = (rdm_device_enum_type)(RDM_FIRST_DEV + 1);
      dev_index < RDM_DEV_MAX; dev_index++)
  {
    if(rdm_current_device_map[dev_index] == service)
    {
      return dev_index;
    }
  }
  return RDM_NULL_DEV;

} /* rdm_get_device */


/*===========================================================================

FUNCTION RDM_GET_MULTI_DEV                              EXTERNALIZED FUNCTION

DESCRIPTION
  Query the Runtime Device Mapper for the port/devices currently being used
  by the specified application/service.

  It returns a device mask for the devices the service holds. The caller can
  test if the service is using a dev (for example, RDM_USB_SER1_DEV) by
  if (dev_mask & (1 << RDM_USB_SER1_DEV)).

DEPENDENCIES
  None

RETURN VALUE
  device mask.

SIDE EFFECTS
  None

===========================================================================*/
uint32 rdm_get_multi_dev
(
  /* Service to query what devices it may be using, if any */
  rdm_service_enum_type service 
)
{
  /* index to traverse the valid devices                */
  rdm_device_enum_type  index;

  /* device mask to return                              */
  uint32                dev_mask = 0;

/*------------------------------------------------------------------*/

  if(service >= RDM_FIRST_SRVC && service < RDM_SRVC_MAX)
  {
    /* Then we test the devices by traverving from 
       RDM_UART1_DEV (RDM_FIRST_DEV + 1) to (RDM_DEV_MAX - 1) */
    for(index = (rdm_device_enum_type)(RDM_FIRST_DEV + 1);
        index < RDM_DEV_MAX; index++)
    {
      if(rdm_current_device_map[index] == service)
      {
        dev_mask |= (1 << index);
      }
    }
  }

  return dev_mask;
} /* rdm_get_multi_dev */


/*===========================================================================

FUNCTION RDM_GET_SERVICE                                EXTERNALIZED FUNCTION

DESCRIPTION
  Query the Runtime Device Mapper for the application/service that is 
  currently holding the specified port/device (if any).

DEPENDENCIES
  None

RETURN VALUE
  rdm_service_enum_type

SIDE EFFECTS
  None

===========================================================================*/
rdm_service_enum_type rdm_get_service
(
  /* Device to see what service is using it, if any */
  rdm_device_enum_type device 
)
{
  if(device <= RDM_FIRST_DEV || device >= RDM_DEV_MAX )
  {
    ERR("wrong device: %d", device,0,0);
    return RDM_NULL_SRVC;
  }
  else
  {
    return rdm_current_device_map[device];
  }

} /* rdm_get_service */


/*===========================================================================

FUNCTION  RDM_GET_VALID_PORTS                           EXTERNALIZED FUNCTION

DESCRIPTION
  Query the RDM for the list of valid ports for the specified service.

DEPENDENCIES
  The caller must have allocated space of RDM_DEV_MAX to the 
  dev_displays array.

RETURN VALUE
  Boolean TRUE if the service is valid, else FALSE.

SIDE EFFECTS
  The contents of dev_displays[] will be written to.
===========================================================================*/
boolean rdm_get_valid_ports
(
  /* The service that getting the valid ports           */
  rdm_service_enum_type    service,

  /* array of rdm_service_display of length RDM_DEV_MAX */
  rdm_service_display_type dev_displays[] 
)
{
  /* index to traverse rdm_device_enum_type     */
  rdm_device_enum_type  dev_index;

/*------------------------------------------------------------------*/

  DEBUG(MSG_MED("rdm_get_valid_ports: %d",service,0,0););

  if(service < RDM_FIRST_SRVC || service >= RDM_SRVC_MAX)
  {
    ERR("wrong service: %df",service,0,0);
    return FALSE;
  }
  
  INTLOCK();

  /*----------------------------------------------------------------------
    Opt to build this table on the fly versus keeping them all in memory
    and having to update them often
  ----------------------------------------------------------------------*/
  for(dev_index = RDM_FIRST_DEV; dev_index < RDM_DEV_MAX; dev_index++)
  {
    /* set device for dev_displays[dev_index]   */
    if( RDM_SD_COMPAT(service, dev_index) )
    {
      dev_displays[dev_index].device = dev_index;
    }
    else
    {
      dev_displays[dev_index].device  = RDM_SRVC_NOT_ALLOWED;
      dev_displays[dev_index].display = FALSE;
      dev_displays[dev_index].current = FALSE;
      continue;
    }

    /* set display for dev_displays[dev_index]  */
    if( (dev_displays[dev_index].device == RDM_USB_MDM_DEV) ||
        (dev_displays[dev_index].device == RDM_BT_NA_DEV) )
    {
      /*------------------------------------------------
        Let UI show & therefore map DS to either BT or 
        USB which ever mode we're in ...
      ------------------------------------------------*/
      if(service == RDM_DATA_SRVC)
      {
        dev_displays[dev_index].display = TRUE;
      }
      else
      {
        dev_displays[dev_index].display = TRUE;
      }
    }
    else if( (dev_displays[dev_index].device == RDM_USB_MMC_DEV) ||
             (dev_displays[dev_index].device == RDM_USB_SER1_DEV) )
    {
      dev_displays[dev_index].display = TRUE;
    }
#if defined(FEATURE_SMD) && defined(FEATURE_DATA_SERIALIZER) && defined(FEATURE_DATA_MULTI_PORTS) && defined(IMAGE_MODEM_PROC)
    else if( (dev_displays[dev_index].device >= RDM_SMD_DS_DEV) && 
             (dev_displays[dev_index].device <= RDM_SMD_DATA4_DEV) )
    {
      dev_displays[dev_index].display = FALSE;
    }
#endif /* defined(FEATURE_SMD)... */
    else
    {
      dev_displays[dev_index].display = TRUE;
    }

    /* set current for dev_displays[dev_index]. Current is TRUE when 
        dev_displays[dev_index]'s device is not NULL and the service 
        is using it                                                     */
    dev_displays[dev_index].current = rdm_is_srv_holding_dev(service, dev_index);
  } /* end for loop */
  

  INTFREE();
  return TRUE;
} /* rdm_get_valid_ports() */


/*===========================================================================

FUNCTION RDM_REGISTER_OPEN_FUNC                         EXTERNALIZED FUNCTION

DESCRIPTION
  Function for Service Tasks to register their open function. This open 
  function is what RDM calls to instruct a Service to open a specified port.

  The function is used in conjunction with rdm_register_close_func.

  The service can only hold one device at a time if it registers
  its open function by this function. If the service holds dev1 and requests
  RDM to map dev2 to it, RDM will take dev1 from the service and then assign
  dev2 to the service.

  Please use rdm_register_func_multi_dev if you want multi-dev support.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rdm_register_open_func
(
  /* What Service is registering the function */
  rdm_service_enum_type             service,

  /* the routine to be called for opening */
  rdm_service_open_func_ptr_type    open_func
)
{
  DEBUG(MSG_MED("SRVC: %d, open: %x)", service, open_func, 0););

  /* check the parameter                                        */
  if(service >= RDM_SRVC_MAX || service < RDM_FIRST_SRVC)
  {
    ERR("wrong service: %d", service,0,0);
    return;
  }

  if(open_func == NULL)
  {
    ERR("null open_func",0,0,0);
    return;
  }
  
  /* save the open function                                     */
  rdm_srv_cbs[service].multi_dev                    = FALSE;
  rdm_srv_cbs[service].open_func.single_dev_p       = open_func;

} /* rdm_register_open_func() */


/*===========================================================================

FUNCTION RDM_REGISTER_CLOSE_FUNC                        EXTERNALIZED FUNCTION

DESCRIPTION
  Function for Service Tasks to register their close function. This close 
  function is what RDM calls to instruct a Service to close it's open port.

  The function is used in conjunction with rdm_register_open_func.

  By using this function, a service can have only one port open, hence
  no sio_port_id_type argument in the close function.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void rdm_register_close_func
(
  /* What Service is registering the function */
  rdm_service_enum_type              service,

  /* the routine to be called for closing */
  rdm_service_close_func_ptr_type    close_func
)
{
  DEBUG(MSG_MED("SRVC: %d, close: %x)", service, close_func, 0););

  /* check the parameter                                        */
  if(service >= RDM_SRVC_MAX || service < RDM_FIRST_SRVC)
  {
    ERR("wrong service: %d", service,0,0);
    return;
  }

  if(close_func == NULL)
  {
    ERR("null close_func",0,0,0);
    return;
  }

  /* save the close function                                     */
  rdm_srv_cbs[service].multi_dev                    = FALSE;
  rdm_srv_cbs[service].close_func.single_dev_p      = close_func;

} /* rdm_register_close_func() */


/*===========================================================================

FUNCTION RDM_REGISTER_FUNC_MULTI_DEV                    EXTERNALIZED FUNCTION

DESCRIPTION
  Function for Service Tasks to register their open/close functions.  The 
  functions are what RDM calls to instruct a Service to open/close device.

  This function supports multiple ports functionality, ie a service can
  open more than one device at a time.

  It is newer than rdm_register_open_func and rdm_register_close_func, 
  which register the opening/closing function when RDM only supports one 
  port for one service. rdm_register_open_func and rdm_register_close_func 
  are maintained for back compatibility.

DEPENDENCIES
  None

RETURN VALUE
  boolean
    TRUE : the registration succeeds
    FALSE: the registration fails. The following reasons could cause failure.
           1) open or close function is NULL
           2) service is out of range

SIDE EFFECTS
  None
===========================================================================*/
boolean rdm_register_func_multi_dev
(
  /* the service who registers the open and close function      */
  rdm_service_enum_type                 service,

  /* function for opening device                                */
  rdm_srv_func_ptr_multi_dev_type       open_func,

  /* function for closing device                                */
  rdm_srv_func_ptr_multi_dev_type       close_func,

  /* user data, the pointer will be passed back to the service  
     It can be set to NULL.                                     */
  void *                                data
)
{
  DEBUG(MSG_MED("srv: %d, open: %p, close: %p", service, open_func, 
                close_func );
        MSG_MED("data: %p", data,0,0);
        );

  /* check parameters                                           */
  if(service >= RDM_SRVC_MAX || service < RDM_FIRST_SRVC)
  {
    ERR("wrong service: %d", service,0,0);
    return FALSE;
  }

  if(open_func == NULL || close_func == NULL)
  {
    ERR("null open func and close function",0,0,0);
    return FALSE;
  }

  /* save the registered info                                   */
  rdm_srv_cbs[service].multi_dev                    = TRUE;
  rdm_srv_cbs[service].open_func.multi_dev_p        = open_func;
  rdm_srv_cbs[service].close_func.multi_dev_p       = close_func;
  rdm_srv_cbs[service].data                         = data;

  return TRUE;

} /* rdm_register_func_multi_dev */


/*===========================================================================

FUNCTION RDM_GET_DEVICE_STR                             EXTERNALIZED FUNCTION

DESCRIPTION
  Query the RDM for the ASCII string that corresponds to the 
  rdm_device_enum_type specified.

  Note:This is used by the UI, and only 13 bytes of storage will be
  available in *str. One byte is for the NULL.

DEPENDENCIES
  The device must be in rdm_device_enum_type. The str has pre-allocated
  space of at least 13 chars including null-terminator.

RETURN VALUE
  Boolean TRUE if the device is valid, else FALSE.

SIDE EFFECTS
  The contents of pointer str will be written to. If the specified device
  is not valid, str is set to NULL.
===========================================================================*/
boolean rdm_get_device_str
(
  rdm_device_enum_type  device, /* device to look up            */
  char                  *str    /* where to write the string    */
)
{
  /*---------------------------------------------------------------------------
  Have a table of rdm_device_enum_type to UI displayable ASCII strings.
  Supports rdm_device_to_str(). The order of the device_to_str_table[]
  table must match the order of rdm_device_enum_type.
---------------------------------------------------------------------------*/
  static const char * device_to_str_table[RDM_DEV_MAX] =
  { 
    "NULL DEV",                     /* RDM_NULL_DEV         */
    "UART1",                        /* RDM_UART1_DEV        */
    "UART2",                        /* RDM_UART2_DEV        */
    "UART3",                        /* RDM_UART3_DEV        */
#ifdef FEATURE_DATA_RM_NET
    "USB MDM",                      /* RDM_USB_MDM_DEV      */
#else
    "USB CDC",                      /* RDM_USB_MDM_DEV      */
#endif
    "USB MMC",                      /* RDM_USB_MMC_DEV      */
#ifdef FEATURE_DATA_RM_NET
    "USB SER1",                     /* RDM_USB_SER1_DEV     */
#else
    "USB DIAG",                     /* RDM_USB_SER1_DEV     */
#endif
    "BT NA",                        /* RDM_BT_NA_DEV        */
    "SMEM DS",                      /* RDM_SMEM_DS_DEV      */
    "LSM CSD",                      /* RDM_LSM_CSD_DEV      */
#ifdef FEATURE_DATA_RM_NET
    "USB SER2",                     /* RDM_USB_SER2_DEV     */
#else
    "USB NMEA",                     /* RDM_USB_SER2_DEV     */
#endif
    "BT SPP",                       /* RDM_BT_SPP_DEV       */
    "USB NET WWAN",                 /* RDM_USB_NET_WWAN_DEV */
    "APPS uP DATA",                 /* RDM_SMEM_DS_DEV      */
    "APPS uP DATA1",                /* RDM_SMD_DATA1_DEV */
    "APPS uP DATA2",                /* RDM_SMD_DATA2_DEV */
    "APPS uP DATA3",                /* RDM_SMD_DATA3_DEV */
    "APPS uP DATA4",                /* RDM_SMD_DATA4_DEV */
    "APPS DATA11",                  /* RDM_SMD_DATA11_DEV   */
    "3RD PARTY OS",                 /* RDM_APS_SERIAL_VSP_DEV */
    "APPS NMEA",                    /* RDM_SMD_NMEA_DEV     */
    "BREW EX DATA",            /* RDM_USB_BREW_EXTENDED_DATA_DEV */
    "BREW EX NOTI",            /* RDM_USB_BREW_EXTENDED_NOTIFICATION_DEV */
    "MAXDEV"                      /* Just a place holder */
  };

  boolean       result;

  /*------------------------------------------------------------------*/

  if(device < RDM_NULL_DEV || device >= RDM_DEV_MAX)
  {
    str[0] = '\0';
    result = FALSE;
  }
  else
  {
    (void) std_strlcpy(str, device_to_str_table[device], MAX_DEVICE_NAME_LEN);
    result = TRUE;
  }

  return result;
}


/*===========================================================================

FUNCTION RDM_IS_SRV_HOLDING_DEV                         EXTERNALIZED FUNCTION

DESCRIPTION
  Test if a service is using a device.

DEPENDENCIES
  None

RETURN VALUE
  Boolean
    TRUE if the service is holding the device or
         if the service does not hold any device and the second
            parameter is RDM_NULL_DEV
    FALSE if the service is not holding the device or
          if the parameters are not right

SIDE EFFECTS
  None

===========================================================================*/
boolean rdm_is_srv_holding_dev
(
  rdm_service_enum_type service,        /* service to test      */
  rdm_device_enum_type  device          /* device to test       */
)
{
  /* function return                                    */
  boolean       result;

  /* the dev mask show what devices the service holds   */
  uint32        dev_mask;

/*------------------------------------------------------------------*/

  /* Check if service & device are w/i range    */
  if(device < RDM_NULL_DEV || device >= RDM_DEV_MAX || 
     service < RDM_FIRST_SRVC || service >= RDM_SRVC_MAX)
  {
    return FALSE;
  }

  dev_mask = rdm_get_multi_dev(service);

  /* the service does not hold any device and the second
     parameter is RDM_NULL_DEV                                  */
  if( (dev_mask == 0) && (device == RDM_NULL_DEV) )
  {
    result = TRUE;
  }
  else
  {
    result = ( RDM_SRV_USE_DEV(service, device) > 0 );
  }

  return result;
} /* rdm_is_srv_holding_dev */


/*===========================================================================

FUNCTION rdm_set_srv_compat_mask                        EXTERNALIZED FUNCTION

DESCRIPTION
  Set device compatible mask for the service from 
  (RDM_LAST_KNOWN_SRVC + 1) to (RDM_SRVC_MAX - 1). It tells RDM which 
  device this service can use.

  It provides an interface to dynamically set the compatiblity mask. Don't
  use this function for services of from RDM_DIAG_SRVC to 
  RDM_LAST_KNOWN_SRVC (RDM_RPC_SRVC). The device compatibility table 
  are set for those serivce at compile time.

  The caller can set dev_mask as this: if a service can use devices
  RDM_UART1_DEV and RDM_USB_SER1_DEV.  The dev_mask is 
  (1 << RDM_NULL_DEV) | (1 << RDM_UART1_DEV) | (1 << RDM_USB_SER1_DEV).
  RDM_NULL_DEV is here since RDM can always map NULL DEV to the service, 
  meaning closing the device of the service.

DEPENDENCIES
  None

RETURN VALUE
  Boolean
    TRUE successfully set
    FALSE if the service is incorrect.

SIDE EFFECTS
  None

===========================================================================*/
boolean rdm_set_srv_compat_mask
(
  rdm_service_enum_type service,        /* service to set       */
  uint32                dev_mask        /* dev mask to set      */
)
{
  /* This function is for the services from 
     (RDM_LAST_KNOWN_SRVC + 1) to (RDM_SRVC_MAX - 1)   */
  if (service <= RDM_LAST_KNOWN_SRVC  || service >= RDM_SRVC_MAX)
  {
    ERR("Invalid serivce %d", service,0,0);
    return FALSE;
  }

  
  /* mask out the incompatible device for this service */
  rdm_srv_dev_compat_tab[service] = dev_mask & RDM_DEV_WHOLE_SET;

  return TRUE;
} /* rdm_set_srv_compat_mask */


/*===========================================================================

FUNCTION RDM_IS_SRV_DEV_COMPAT                          EXTERNALIZED FUNCTION

DESCRIPTION
  Test if a service and a device is compatible. That is whether the service
  can use the device.

DEPENDENCIES
  None

RETURN VALUE
  Boolean
    TRUE if the service and the device is compatible, 
    FALSE if the service and the device is not compatible or
          if the parameters are not right

SIDE EFFECTS
  None

===========================================================================*/
boolean rdm_is_srv_dev_compat
(
  rdm_service_enum_type service,        /* service to test      */
  rdm_device_enum_type  device          /* device to test       */
)
{
  /* function return                            */
  boolean result;

/*------------------------------------------------------------------*/

  /* Check if service & device are w/i range    */
  if(device < RDM_NULL_DEV || device >= RDM_DEV_MAX || 
     service < RDM_FIRST_SRVC || service >= RDM_SRVC_MAX)
  {
    return FALSE;
  }

  result = (RDM_SD_COMPAT(service, device) > 0);

  return result;
} /* rdm_is_srv_dev_compat */

/*===========================================================================
                           LOCAL FUNCTIONS
===========================================================================*/


/*===========================================================================

FUNCTION RDM_RESET_STATE_MACHINE

DESCRIPTION
  Cleanup/reset RDM state varaibles

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
static void rdm_reset_state_machine (void)
{

  DEBUG(MSG_MED("rdm_reset_state_machine()", 0, 0, 0););

  rdm_state.service_requester   = RDM_NULL_SRVC;
  rdm_state.desired_device      = RDM_DEV_MAX;
  rdm_state.inprocess_service   = RDM_NULL_SRVC;
  rdm_state.inprocess_device    = RDM_DEV_MAX;

  rdm_state.cbptr               = NULL;

  rdm_state.is_persistent       = FALSE;

  /* reset rdm_state.state at the end */
  rdm_state.state               = RDM_IDLE_STATE;

} /* rdm_reset_state_machine() */


/*===========================================================================

FUNCTION RDM_SEND_SERVICE_CMD

DESCRIPTION
  This function will call the registered open or close routine provided by
  the service task. If the service task did not register both an open and
  close routine, then it cannot use the RDM services.
   
DEPENDENCIES
  None

RETURN VALUE
  Boolean - False means the Service has not registered an open or close 
  routine.

SIDE EFFECTS
  None
===========================================================================*/
static boolean rdm_send_service_cmd
(
  /* the service executing the cmd              */
  rdm_service_enum_type  service, 

  /* the cmd                                    */
  rdm_command_enum_type  port_cmd,

  /* the device which the cmd act on            */
  rdm_device_enum_type   device
)
{
  /* function return                                    */
  boolean result;

  /* sio port type corresponding to device              */
  sio_port_id_type sio_port;

/*------------------------------------------------------------------*/
  DEBUG(MSG_MED("rdm_send_service_cmd(SRV=%d, CMD=%d, DEV=%d)",
                service, port_cmd, device););

  /* the parameters should be correct   */
  ASSERT( (device > RDM_FIRST_DEV) && (device < RDM_DEV_MAX));
  ASSERT( (service >= RDM_FIRST_SRVC) && (service < RDM_SRVC_MAX) );
  ASSERT( (port_cmd == RDM_OPEN_PORT) || (port_cmd == RDM_CLOSE_PORT));

  /* check rdm's state                  */
  ASSERT( ( (port_cmd == RDM_OPEN_PORT) && 
            (rdm_state.state == RDM_OPENING_STATE || 
             rdm_state.state == RDM_INIT_STATE))         || 

          ( (port_cmd == RDM_CLOSE_PORT) &&
            (rdm_state.state == RDM_CLOSING_STATE))
        );

  /* check if the service has registered open/close functions */
  if(rdm_srv_cbs[service].multi_dev)
  {
    /* the service uses multi_dev interface   */
    if( (rdm_srv_cbs[service].open_func.multi_dev_p == NULL) ||
        (rdm_srv_cbs[service].close_func.multi_dev_p == NULL))
    {
      MSG_ERROR("Open and close callbacks not registered",0,0,0);
      return FALSE;
    }
  }
  else
  {
    /* the service uses single_dev interface   */
    if( (rdm_srv_cbs[service].open_func.single_dev_p == NULL) ||
        (rdm_srv_cbs[service].close_func.single_dev_p == NULL))
    {
      MSG_ERROR("Open and close callbacks not registered",0,0,0);
      return FALSE;
    }
  }

  /* We are in the right state and parameters are corrent, 
     go ahead to fulfill the request                            */
  sio_port = rdm_device_to_port_id_table[device];

  if(port_cmd == RDM_OPEN_PORT)
  {
    if(rdm_srv_cbs[service].multi_dev)
    {
      (rdm_srv_cbs[service].open_func.multi_dev_p)(sio_port, rdm_srv_cbs[service].data);

    }
    else
    {
      (rdm_srv_cbs[service].open_func.single_dev_p)(sio_port);
    }

    result = TRUE;
  }
  else if(port_cmd == RDM_CLOSE_PORT)
  {
    if(rdm_srv_cbs[service].multi_dev)
    {
      (rdm_srv_cbs[service].close_func.multi_dev_p)(sio_port, rdm_srv_cbs[service].data);

    }
    else
    {
      (rdm_srv_cbs[service].close_func.single_dev_p)();
    }
    
    result = TRUE;
  }
  else
  {
    ERR("Invalid cmd: %d", port_cmd, 0, 0);
    result = FALSE;
  }

  return result;

} /* rdm_send_service_cmd */

#ifdef FEATURE_EFS

/*===========================================================================

FUNCTION  RDM_UPDATE_EFS_ENTRY

DESCRIPTION
  Used by RDM to update port mapping information in EFS. This is called after
  a successful SRVC to DEV mapping.

  If called with RDM_DEV_ALL as an argument will unconditionally write the
  entire file to EFS.  This is useful to generate the file initially.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  The port mapping is written to EFS.

===========================================================================*/
LOCAL void rdm_update_efs_entry(int device_index)
{
  /* msg return for fs_seek_write       */
  fs_rsp_msg_type  fs_rsp;

/*------------------------------------------------------------------*/

  if(rdm_efs_handle == FS_NULL_HANDLE)
  {
    ERR("no efs file opened",0,0,0);
  }
  else
  {
    if (device_index >= 0)
    {
       // Do not record non-persistent transitions
       if (rdm_state.is_persistent == FALSE)
       {
         return;
       }

       fs_seek_write(rdm_efs_handle, FS_SEEK_SET, device_index, 
                     rdm_current_device_map + device_index,
                     1, NULL, &fs_rsp);
    }
    else if (device_index == RDM_DEV_ALL)
    {
       fs_seek_write(rdm_efs_handle, FS_SEEK_SET, 0, 
                     rdm_current_device_map,
                     RDM_DEV_MAX, NULL, &fs_rsp);
    }

    if (fs_rsp.any.status != FS_OKAY_S)
    {
      ERR("can't update mapping, %d", fs_rsp.any.status,0,0);
    }
  }
} /* rdm_update_efs_entry() */

#endif /* FEATURE_EFS */


/*===========================================================================

FUNCTION RDM_NOTIFY_CLOSE

DESCRIPTION
  This function process rdm_notify status when RDM is in RDM_CLOSING_STATE.

DEPENDENCIES
  None

RETURN VALUE
  boolean
    TRUE : if the port mapping ended, the status is in notify_ptr
    FALSE: if the port mapping hasnot been completed

SIDE EFFECTS
  None
===========================================================================*/
static boolean rdm_notify_close
(
  /* Service Task desiring a new port                           */
  rdm_service_enum_type  service,

  /* status of requested task change                            */
  rdm_assign_status_type status,

  /* what to notify the service_requester if the port assign 
     is done or aborted                                         */
  rdm_assign_status_type * notify_ptr
)
{
  /* function return            */
  boolean result;

/*------------------------------------------------------------------*/
  DEBUG(MSG_MED("notify(srv: %d, status: %d, state: %d", 
                service, status, rdm_state.state););

  ASSERT( (rdm_state.state == RDM_CLOSING_STATE) && 
          (rdm_state.inprocess_service == service) );
  ASSERT( (rdm_state.inprocess_device > RDM_FIRST_DEV) && 
          (rdm_state.inprocess_device < RDM_DEV_MAX));

  /*----------------------------------------------------
      The Service that has Closed the desired port has
      now completed the Close. Proceed to ask
      the proper Service to Open the desired port. Note 
      the order is important because informing the service 
      task (queuing a command to another task) can cause a 
      context switch and a potential invocation of 
      "rdm_notify")
      ----------------------------------------------------*/
  if( status == RDM_DONE_S )
  {
    /* Update the current config table. */
    rdm_current_device_map[rdm_state.inprocess_device] = RDM_NULL_SRVC;

#ifdef FEATURE_EFS
    /* a port has been close, update the port map */
    rdm_update_efs_entry(rdm_state.inprocess_device);
#endif /* FEATURE_EFS */

    /*----------------------------------------------------
      if the service was only told to close (assign(srvc, NULL))
      versus a goto type command, then there is no open to issue,
      and we are done here. Call callback if one provided.
      ----------------------------------------------------*/
    if(rdm_state.desired_device == RDM_NULL_DEV)
    {
      result            = TRUE;
      *notify_ptr       = status;
    }
    else
    {
      /* service holding desired_device                         */
      rdm_service_enum_type     srv_using_desired_device;
      
      /* command to take                                        */
      rdm_command_enum_type     srv_cmd;
      

      srv_using_desired_device = rdm_get_service(rdm_state.desired_device);
      
      if(srv_using_desired_device != RDM_NULL_SRVC)
      {
        /* rdm closed the device the service_requester holds, continue to 
           close desired_device so that it can be opened for 
           service_requester                                              */
        rdm_state.state                 = RDM_CLOSING_STATE;
        rdm_state.inprocess_service     = srv_using_desired_device;

        srv_cmd                         = RDM_CLOSE_PORT;
      }
      else
      {
        /* we need continue to open the desired_device for service_requester */
        rdm_state.state                 = RDM_OPENING_STATE;
        rdm_state.inprocess_service     = rdm_state.service_requester;

        srv_cmd                         = RDM_OPEN_PORT;
      }
      
      rdm_state.inprocess_device        = rdm_state.desired_device;

      if(rdm_send_service_cmd(rdm_state.inprocess_service, srv_cmd,
                              rdm_state.inprocess_device))
      {
        /* port mapping continues               */
        result = FALSE;
      }
      else
      {
        /* port mapping aborts                  */
        result          = TRUE;
        *notify_ptr     = RDM_NOT_ALLOWED_S;
      }
    } /* else (rdm_state.desired_device == RDM_NULL_SRVC) */
        
  } /* if( status == RDM_DONE_S ) */
  else
  {
    /* Close did not occur, reset DEVMAP state machine, we are done */
    result = TRUE;
    *notify_ptr = status;
  }

  return result;
} /* rdm_notify_close */


/*=======================================================================
FUNCTION RDM_REMOVE_DUP_MAPPINGS

DESCRIPTION: 
  If a service maps to multiple ports but the service does not support
  multiple device openning, give only the first port to the service.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Some default port mapping or port mapping read from EFS could be 
  changed to RDM_NULL_SRVC.
=======================================================================*/
static void rdm_remove_dup_mappings(void)
{
  /* index to traverse rdm_device_enum_type             */
  rdm_device_enum_type  dev_index;

  /* service mapped to dev_index                        */
  rdm_service_enum_type srv;

  /* first device held by srv                           */
  rdm_device_enum_type  first_dev;

/*------------------------------------------------------------------*/

  /* Traverve device from RDM_UART1_DEV (RDM_FIRST_DEV + 1) to 
     (RDM_DEV_MAX - 1) to search for duplicates                 */
  for(dev_index = (rdm_device_enum_type)(RDM_FIRST_DEV + 1);
      dev_index < RDM_DEV_MAX; dev_index++)
  {
    /* get the service mapped to dev_index              */
    srv         = rdm_get_service(dev_index);

    if(srv != RDM_NULL_SRVC)
    {
      /* get the first device held by srv                 */
      ASSERT( (srv >= RDM_FIRST_SRVC) && (srv < RDM_SRVC_MAX) );
      first_dev   = rdm_get_device(srv);

      if(
         /* srv holds both first_dev and dev_index */
         (first_dev < dev_index) &&
         /* but srv support only one device */
         (!rdm_srv_cbs[srv].multi_dev)
         )
      {
        rdm_current_device_map[dev_index] = RDM_NULL_SRVC;
      }
    }
  }

} /* rdm_remove_dup_mappings() */


/*=======================================================================
FUNCTION RDM_START_CLOSE_DEVICE

DESCRIPTION: 
  Close a device being used by a service.

DEPENDENCIES
  The device is not a null device.
  The service is using the device.

RETURN VALUE
  boolean
    TRUE : the closing action is made
    FALSE: the closing request is rejected when
           1) rdm is busy, not in Idle state

SIDE EFFECTS
  None

=======================================================================*/
static boolean rdm_start_close_device
(
  /* this service whose device is to be closed          */
  rdm_service_enum_type         service,

  /* device to be closed                                */
  rdm_device_enum_type          device,

  /* call back function                                 */
  rdm_assign_port_cb_type       cbptr
)
{
  /* function return                            */
  boolean       result;

/*------------------------------------------------------------------*/

  /* check parameter                            */
  ASSERT(device > RDM_FIRST_DEV && device < RDM_DEV_MAX);
  ASSERT(service >= RDM_FIRST_SRVC && service < RDM_SRVC_MAX);
  /* service should be using device             */
  ASSERT(RDM_SRV_USE_DEV(service, device) );

  /*-------------------------------------------------------------
    If we made it here, we just request the service to close it's
    port. We setup RDM state variables such that when the notify
    comes in for this close, we do not then issue an open to any
    service.
    -------------------------------------------------------------*/
  rdm_state.state               = RDM_CLOSING_STATE;

  rdm_state.service_requester   = service;
  rdm_state.desired_device      = RDM_NULL_DEV;
  rdm_state.inprocess_service   = service;
  rdm_state.inprocess_device    = device;

  rdm_state.cbptr               = cbptr;

  /* close the device the service holds */
  if(rdm_send_service_cmd(rdm_state.inprocess_service, 
                          RDM_CLOSE_PORT, rdm_state.inprocess_device))
  { 
    result = TRUE;
  }
  else
  {
    rdm_assign_port_cb_type notify_cb = rdm_state.cbptr;

    /* we are ready to service next request           */
    rdm_reset_state_machine();

    /* inform the requester of the failure */
    if(notify_cb)
    {
      notify_cb(RDM_NOT_ALLOWED_S, service, RDM_NULL_DEV);
    }
        
    result = FALSE;
  }

  return result;
} /* rdm_start_close_device() */



/*=======================================================================
FUNCTION RDM_START_OPEN_DEVICE

DESCRIPTION: 
  Open a device and assign it to a service. 

DEPENDENCIES
  None

RETURN VALUE
  boolean
    TRUE : open action is made.
    FALSE: open action is rejected.

SIDE EFFECTS
  None

=======================================================================*/
static boolean rdm_start_open_device
(
  /* Service Task desiring a new port    */
  rdm_service_enum_type         service,

  /* Device port to give to Service task */
  rdm_device_enum_type          device,

  /* callback function         */  
  rdm_assign_port_cb_type       cbptr,

  /* is this transition persistent? */
  boolean                       is_persistent
)
{
  /* function return                    */
  boolean                       result;

  /* the service who is using device    */
  rdm_service_enum_type         dev_owner;

  /* action to take                     */
  rdm_command_enum_type         srv_cmd;

/*------------------------------------------------------------------*/

  /* check parameters                                           */
  ASSERT( (device > RDM_FIRST_DEV) && (device < RDM_DEV_MAX) );
  ASSERT( (service >= RDM_FIRST_SRVC) && (service < RDM_SRVC_MAX) );

  /* get who is using the device                                */
  dev_owner = rdm_get_service(device);

  /* if the service already holds the device, we are done       */
  if(dev_owner == service)
  {
    /* we are ready to serve the next request, 
       unlock the state machine                                 */
    rdm_reset_state_machine();

    if(cbptr)
    {
      cbptr(RDM_DONE_S, service, device);
    }
    
    result = TRUE;
  }
  else
  {
    if( dev_owner != RDM_NULL_SRVC)
    {
      /*------------------------------------------------------
        The desired port is now in use. Set the DEVMAP State
        and set the in-process Service indicator then prepare 
        to ask the correct Service task to close the 
        specified port.
        ------------------------------------------------------*/
      rdm_state.state                 = RDM_CLOSING_STATE;
      srv_cmd                         = RDM_CLOSE_PORT;
      rdm_state.inprocess_service     = dev_owner;
    }
    else /* the device is free, no need to close it */
    {
      /*------------------------------------------------------
        Skipping Closing State. Request specified Service
        task to Open specified port. Note the order is 
        important because informing the service task (queuing 
        a command to another task) can cause a context switch 
        and a potential invocation of "rdm_notify")
        ------------------------------------------------------*/
      rdm_state.state                 = RDM_OPENING_STATE;
      srv_cmd                         = RDM_OPEN_PORT;
      rdm_state.inprocess_service     = service;
    }

    rdm_state.service_requester       = service;
    rdm_state.desired_device          = device;
    rdm_state.inprocess_device        = device;
    /* record the callback function */
    rdm_state.cbptr = cbptr;
    rdm_state.is_persistent           = is_persistent;

    if(rdm_send_service_cmd(rdm_state.inprocess_service, 
                            srv_cmd, rdm_state.inprocess_device))
    {
      result = TRUE;
    }
    else
    {
      rdm_assign_port_cb_type notify_cb = rdm_state.cbptr;

      /* we are ready to service next request           */
      rdm_reset_state_machine();

      /*------------------------------------------------------
        If rdm_send_service_cmd() returns FALSE, that means the 
        service did not register a close routine. This means
        that service cannot use the RDM utility.
        ------------------------------------------------------*/
      if(notify_cb)
      {
        notify_cb(RDM_NOT_ALLOWED_S, service, device);
      }
          
      /* the request aborts           */
      result = FALSE;
    }
  } /* else (dev_owner == service) */

  return result;

} /* rdm_start_open_device */


/*=======================================================================
FUNCTION RDM_LOCK_STATE

DESCRIPTION: 
  Check if RDM is in IDLE state.  If it is, change it to non-idle state
  so that the RDM will not accept any other port mapping request until 
  it finishes its current task and goes back to IDLE.

DEPENDENCIES
  Don't call this in ISR context.

RETURN VALUE
  boolean
    TRUE : RDM is in IDLE and change to non-idle.
    FALSE: RDM is not in IDLE state.

SIDE EFFECTS
  None

=======================================================================*/
static boolean rdm_lock_state ( void )
{
  /* if RDM can serve the request               */
  boolean       rdm_locked = FALSE;

/*------------------------------------------------------------------*/

  ASSERT(!rex_is_in_irq_mode());

  /* guarantee atomity of test/set rdm_state

     Enter critical section for test/set rdm_state, tasklock should be
     good enough here to guarantee atomity of the code from here to
     TASKFREE if we make sure non of the RDM functions is executed in
     ISR.
  */
  TASKLOCK();

  if(rdm_state.state == RDM_IDLE_STATE)
  {
    /* set the state to be locked so that we will reject any other
       request to map the port.                                         */
    rdm_state.state     = RDM_LOCKED_STATE;

    rdm_locked          = TRUE;
  }

  /* free task lock asap to let tasks run to prevent deadlock */
  TASKFREE();

  return rdm_locked;

} /* rdm_lock_state */

#ifdef FEATURE_USB_ON_APPS_PROC

/*===========================================================================

FUNCTION usbotg_remote_vv_cb_helper_f

DESCRIPTION
  This function calls the pm_app_otg_ctrlr_remote_dev_resumed_cb 
  which is the real callback function registered from the applications
  processor.  It uses the parameters sent with the message  

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Makes an RPC call to the applications processor.
===========================================================================*/
void 
usbotg_remote_vv_cb_helper_f
(
  /* ONCRPC Proxy command message */
  oncrpc_proxy_cmd_client_call_type *msg
)
{
  usbotg_remote_vv_f_type cb_func  = 
                (usbotg_remote_vv_f_type)msg->data[0];

  /* Make call to callback with parameter data */
  if (cb_func != NULL)
  {
    cb_func();
  }

} /* usbotg_remote_vv_cb_helper_f */



/*===========================================================================

FUNCTION pm_app_otg_remote_vv_cb_handler

DESCRIPTION
  This function is the callback that is registered locally with the
  pm_app_otg_ctrlr_remote_dev_resumed_cb_handler() function.  When called, this function
  will save parameter information and queue a command with the RPC 
  proxy task to call the real callback from the applications processor.
  This is necessary because this callback is called from interrrupt space
  and we cannot make RPC calls from interrupt space.    

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sends a message to RPC proxy task.
===========================================================================*/
void
usbotg_remote_vv_handler
(
  usbotg_remote_vv_f_type cb_func
)
{
  /* ONCRPC Proxy command structure */
  oncrpc_proxy_cmd_client_call_type *usbotg_remote_vv_cb_msg_ptr;

  usbotg_remote_vv_cb_msg_ptr = remote_apis_client_call_get();

  /* Check that the message pointer is valid */
  ASSERT (usbotg_remote_vv_cb_msg_ptr != NULL);

  if( usbotg_remote_vv_cb_msg_ptr != NULL )
  {

    /* Fill up the pointer data */
    usbotg_remote_vv_cb_msg_ptr->client_call =
      (void(*)( struct oncrpc_proxy_cmd_client_call_type *ptr ))
        usbotg_remote_vv_cb_helper_f;

    /* Send parameter data with message */
    usbotg_remote_vv_cb_msg_ptr->data[0] = (uint32) cb_func;

    /* Queue up the message */
    oncprc_proxy_client_call_send( usbotg_remote_vv_cb_msg_ptr );

    /* Explicitly set to NULL */
    usbotg_remote_vv_cb_msg_ptr = NULL;
  }
} /* usbotg_remote_vv_handler */

/*===========================================================================

FUNCTION usbotg_turn_off_modem_isr_control

DESCRIPTION
  This function call USB clock enable function and gives the interrupt control 
  to apps.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  Sends a message to RPC proxy task.
===========================================================================*/

void usbotg_turn_off_modem_isr_control(void)
{
  /* Called based on signal from usb_modem_isr */
  clk_regime_usb_xtal_on();
  if(rex_is_in_irq_mode())
  {
    usbotg_remote_vv_handler(usbotg_turn_on_apps_isr_control);/*RPC to ARM11*/
  }
  else
  {
    usbotg_turn_on_apps_isr_control();
  }
} /* usbotg_turn_off_modem_isr_control */

/*===========================================================================

FUNCTION usbotg_modem_isr

DESCRIPTION
  This function sets modem usb ISR to NULL and calls for turn off modem 
  interrupt control.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/

void usbotg_modem_isr(void)
{
      /* Need to disable too or we will never see user code again */
  tramp_set_isr( TRAMP_USB_ISR, NULL );
  /*signal something to be done at task context rather than interrupt context.*/
  usbotg_turn_off_modem_isr_control();
} /* usbotg_modem_isr */

/*===========================================================================

FUNCTION usbotg_turn_on_modem_isr_control

DESCRIPTION
  This function sets modem usb ISR and calls USB clock disable.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.
===========================================================================*/
void usbotg_turn_on_modem_isr_control(void)
{
     tramp_set_isr( TRAMP_USB_ISR, usbotg_modem_isr );
     clk_regime_usb_xtal_off();

#if 0 /* commented for future purpose*/
  if( tramp_is_interrupt_pending( TRAMP_USB_ISR ) == TRUE )
  {
    /* In this case the USB interrupt already occurred and we should just turn on USB again */
    /*signal something to be done at task context rather than interrupt context.*/
      usbotg_turn_off_modem_isr_control();
  }
  else
  {
     tramp_set_isr( TRAMP_USB_ISR, usbotg_modem_isr );
     clk_regime_usb_xtal_off();
  }
#endif
} /* usbotg_turn_on_modem_isr_control */
#else
/* temporary stub, needed because the auto-generated file misc_modem_apis_svc.c
** refers to this function.  Added by <dlansky>
*/
void usbotg_turn_on_modem_isr_control(void){}
#endif /*FEATURE_USB_ON_APPS_PROC*/

