/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            S I O    
                            
                       SIO Server Layer

GENERAL DESCRIPTION
  This module contains the interface layer for SIO.  
  
EXTERNALIZED FUNCTIONS

  sio_init()
    Initialize SIO.
                                                     
  sio_open()
    This procedure opens a stream across the serial link.
    
  sio_close()
    This procedure closes a stream.

  sio_transmit()
    Transmit data.

  sio_ioctl()
    Control open stream.
         
  sio_flush_tx()
    Transmits any queued data and then call the registered callback.

  sio_convert_fctl()
    Conversion utility between DS flow control values and SIO flow control
    values.

  sio_disable_device()
    Disables given device, including turning off clock to save power.  

  sio_is_baud_valid()
    Check whether the specified baud rate is valid under the current
    feature set.

  sio_register_driver()
    Register a device driver with the SIO service

  sio_device_id_lookup()
    Look up the device id by supplying the major and minor number for the device.
  
INITIALIZATION AND SEQUENCING REQUIREMENTS

  sio_init()     must be called by a task upon power-up.  This needs to be done
                 once and only once.  
                 
  sio_open()     must be called by each caller (task or ISR) that wants to use 
                 SIO services.  
                 
  sio_close()    needs to be called when the caller wants to give up the use
                 of SIO services to allow some other caller to use SIO.
  
Copyright (c) 1998-2008 QUALCOMM Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/COMMON/vcs/sio.c_v   1.19   17 Apr 2003 17:19:34   chenc  $
  $Header: //depot/asic/msmshared/services/sio/02.00.XX/sio.c#1 $ $DateTime: 2008/10/19 17:40:04 $ $Author: vaghasia $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/08   amv     Added SMD ports for MODEM, APP and QDSP processor
09/04/08   amv     changed stream_id assignment policy in sio_get_stream_id() from first-availble to round-robbin scheme
06/03/08   rh      Fix an issue in sio_control_close
05/14/08   rh      Warning cleanup
04/30/08   rh      Klocwork cleanup
04/24/08   rh      Add new API:  sio_device_id_lookup
01/31/08   rh      Add sio_open and control_open to registerable function
08/10/07   rh      Add the dependency for the siors232_mdd change
07/30/07   rh      Add SIO_PORT_USB_SER3 to the forwarding port list
07/26/07   rh      Add support for split physical/media driver architecture
07/12/07   rh      Add new port - SIO_PORT_USB_SER3
10/13/06   TMR     Merging in changes for supporting QMI bridges
10/11/06   rh      Adding support for FEATURE_HS_USB
09/25/06   rh      Add support for custom baud rate
09/19/06   dsn     Reinsert changes to exclude BT ports from SMD bridge
09/18/06   ptm     Remove separate close callback handling by bridge code.
09/01/06   bfc     Featurized smd_port_mgr
08/23/06   sg      Remove support for brew extended device.
05/24/06   bfc     Cleaned up some #ifdefs
07/31/06   sr      Add support for 3000kbps
07/27/06   rh      Add support for sio port SIO_PORT_SMD_GPS_NMEA
07/17/06   taw     Added support for SMD bridging on apps when USB is on apps.
06/27/06   gd      Added code to work USB Diag on apps side which was there 
                   in intial version, removed in second version. Added again 
                   in this version under FEATURE_USB_ON_APPS_PROC.
06/26/06   gd      Removed MMC feature hack.
06/19/06   bt      Added FEATURE_USB_BREW_EXTENDED_DEVICE support. 
06/12/06   dsn     Exclude SIO_PORT_BT_SPP, SIO_PORT_BT_NA from SMD bridge
05/24/06   bfc     Cleaned up some #ifdefs
05/18/06   sr      Add support for 921.6kbps
05/01/06   bfc     Added featurization for smd_bridge related code
04/25/06   ptm     Added SMD port for GPS NMEA and RPC reply.
03/28/06   bfc     Changed the arguement for extended callback function from
                   a uint32 to a void *
02/28/06   vs      Removed port_id from sio_close_ext and sio_flush_tx_ext
01/26/06   bfc     Added sio_close_ext and sio_flush_tx_ext which work 
                   specifically with SMD streams.
01/18/06   TMR     Added FEATURE_NO_USB_HW_ON_PROCESSOR
01/04/06   ptm     Restore SMEM code, expanded SMD ports as required.
11/18/05   vs      Removed / supressed lint warnings
09/28/05   TMR     Added MSM7500 bridging capability, removed SIO_PORT_SMEMDS_DIAG_RSP
                   and added smd_sio_dev_init()
07/24/05   vs      7500 featurizes first UART, for all other targets this feature 
                    should be defined. 
06/05/05   TMR     Merged in MSM7500 code
06/01/05   adm     Added support for additional SMD DIAG pipe SIO_PORT_SMEMDS_DIAG_RSP
04/14/05   adm     Added smd_sio related calls.
04/14/05   tmr     Removing RDM calls from apps build.
04/14/05   adm     Added smd_sio related calls.
<<<<
03/24/05   jhs     Removed T_RUMI2 protection from sio_usb_dev_init call.
11/09/04   adm     Modified sio_get_stream_id to return fixed stream_is'd
                   in case of SMEMDS related ports 
10/20/04   ptm     Enabled more routines in sio_init for msm7500/RUMI builds.
10/20/04   adm     Added support for featurization of UART1. Featurized USB init.
                   Added support for multiple DS pipes.
04/20/04   pn      Added support for FEATURE_BT in sio_flush_tx().
04/06/05   cc      Corporate in code review comments. 
                   Move MSG_ERROR out of INTLOCK/INTFREE. 
                   Control stream not available for bulk only USB_MMC device.  
03/08/05   cc      Merged in OSIFDev_Dec04 branck. 
12/23/04   cc      Renamed SIO_PORT_USB_CDC_ACM, _DIAG, _NMEA to be 
                   SIO_PORT_USB_MDM,_SER1 and _SER2 respectively. 
                   Added sio_control_stream_port to memorize the port opened
                   each control stream. 
11/24/04   cc      Added SIO_PORT_USB_RRDM and SIO_PORT_USB_NET_WWAN support. 
                   Added sio_control_open/close/transmit() APIs. 
04/20/04   pn      Added support for FEATURE_BT in sio_flush_tx().
11/24/03   cc      Added FEATURE_JCDMA_SIO and OR it with FEATURE_JCDMA_DS.                     
02/21/03   cc      Added FEATURE_USB_DIAG_NMEA support. 
10/17/02   cc      Added FEATURE_JCDMA_1X support. 
07/31/02   cc      added pkt_ptr handling in sio_transmit(), UART, USB and BT 
                   would be able to handling pkt_ptr.
07/15/02   ry      Added support for the LSM_CSD port
07/15/02   ak      Added FEATURE_DATA_MM support.
03/18/02   cc      Changed ASSERT to ERR if(tx_ptr == NULL) in sio_transmit().
                   This fixes CR19326/19895. 
                   Also, added SIO_PORT_USB_MMC handling for bit_rate changing
                   in sio_open().  
02/06/02   cc      Add third UART support.
10/16/01   cc      Remove code dependency on T_MSM3.
10/16/01   cc      Merge in mods of SIMBA from MSM5200_SIO.00.00.17, includes 
                   following 1 item.
05/01/01   dwp     Change arg type for INTLOCK_SAV() to uint32.
06/18/01   dwp     Add support for FEATURE_UART_RATE1.
06/01/01   dwp     Removed dependencies on DS.
03/26/01   dl      Add support for SMEM_DS port.
01/26/01   ych     Merged T-53/JCDMA features for SK.
01/09/01   dwp     Removed bt_sio_cmd_initialize() from sio_init().
11/27/00   dwp     Change multiple SIO_PORT_BT_RFCOMM's to SIO_PORT_BT_SPP.
                   Added support for SIO_PORT_BT_NA.
11/17/00   dwp     Added support for MMC over USB.
11/14/00   dwp     Added support for FEATURE_RUNTIME_DEVMAP.
11/01/00   dwp     Removed call to dsm_init().
09/05/00   dwp     Additonal modifications for FEATURE_BT.
07/27/00   dwp     Added support for FEATURE_USB_DIAG and FEATURE_BT.
05/23/00   jwh     Added sio_is_baud_valid(), mainly to check NV values.
09/09/99   jwh     Added support for FEATURE_SERIAL_DEVICE_MAPPER with a
                   STATIC runtime port map.  Function names to all lower case.
                   SIO Port name changes.  File "siodev.h" became 
                   "siors232.h."  Improved comments. Merge with MSM3100 1.0C 
                   changes.
04/09/99   jwh     Moved #include "comdef.h" outside of conditional file 
                   inclusion to avoid "no external declaration . . ."
                   compiler warning.
03/08/99   jwh     Removed the dependency of "FEATURE_SECOND_UART" on
                   "#ifdef T_SURF".
02/24/99   ak      Removed warning in sio_tx_flush (unused local var)
02/17/99   ak      Added function sio_tx_flush.
01/17/99   ak      Featurized entire file based on whether or not user wants
                   to use old or new SIO (FEATURE_NEW_SIO)
08/03/98   jjn     Created module.
===========================================================================*/


/*===========================================================================

                            INCLUDE FILES

===========================================================================*/

/*---------------------------------------------------------------------------
 Need to include this header file before any reference to T_MSM3.
---------------------------------------------------------------------------*/
#include "target.h"                /* Target specific definitions          */

/*---------------------------------------------------------------------------
 Need an external declaration regardless of feature to avoid a compiler warning.
---------------------------------------------------------------------------*/
#include "comdef.h"                /* Definitions for byte, word, etc.     */

#include "sio.h"                   /* Serial I/O Services.                 */
#include "sio_priv.h"
#include "sio_wrapper.h"           /* Adaptation layer */

#ifdef FEATURE_SERIAL_DEVICE_MAPPER
#include "sdevmap.h"               /* Serial Device Mapper.                */
#endif

#ifdef FEATURE_RUNTIME_DEVMAP
#include "rdevmap.h"               /* Runtime Device Mapper                */
#endif

#ifdef FEATURE_SMEM_DS
#error code not present
#endif  /* FEATURE_SMEM_DS */

#ifdef FEATURE_SMD
#include "smd_sio.h"

#ifdef FEATURE_SMD_PORT_MGR
  #include "smd_port_mgr.h"
#endif

#include "oncrpc.h"
#include "oncrpc_proxy.h"
#include "remote_apis.h"
#endif  /* FEATURE_SMD */

#include "msg.h"                   /* Message Logging Services.            */
#include "err.h"                   /* Error Handling Services.             */
#include "assert.h"                /* Assert Services.                     */

#include "customer.h"              /* Customer specific configuration.     */
#include "memory.h"                /* Standard memory management routines. */


/*===========================================================================

                     CONSTANT AND VARIABLE DECLARATIONS

===========================================================================*/
/* Array indicating whether or not given stream has been allocated SIO
   resources.                                                              */
boolean              sio_stream_opened[ SIO_MAX_STREAM];
/* Array indicating port used by stream with given stream ID.              */
sio_port_id_type     sio_stream_port[ SIO_MAX_STREAM];
/* Array indicating port used by a control stream with given stream ID    */
sio_port_id_type     sio_control_stream_port[ SIO_MAX_STREAM];

#if defined (FEATURE_SMD) && defined (FEATURE_SMD_BRIDGE)

#include "smd_bridge.h"

#ifdef IMAGE_APPS_PROC
  #define SMD_BRIDGE_CONNECT_FUNC     smd_bridge_connect_modem
  #define SMD_BRIDGE_DISCONNECT_FUNC  smd_bridge_disconnect_modem
#else /* !IMAGE_APPS_PROC */
  #define SMD_BRIDGE_CONNECT_FUNC     smd_bridge_connect_apps
  #define SMD_BRIDGE_DISCONNECT_FUNC  smd_bridge_disconnect_apps
#endif /* IMAGE_APPS_PROC */

#endif /* FEATURE_SMD && FEATURE_SMD_BRIDGE */

/*===========================================================================

                     VARIABLE FOR NEW2SIO

===========================================================================*/

sio_device_info_type    sio_device_head[SIO_DEVICE_NUM_MAX];        /* Move to linked list later */
uint32                  sio_device_last_item;
uint32                  sio_stream_mark = 0;

/*===========================================================================

                                FUNCTIONS

===========================================================================*/


/*===========================================================================

FUNCTION sio_register_driver                                EXTERNAL FUNCTION

DESCRIPTION
  Register device driver with the SIO service
      
DEPENDENCIES
  None
                                                       
RETURN VALUE
  0 - Failed to register the driver with sio service
  1 - Success
    
SIDE EFFECTS
  None
  
===========================================================================*/
uint32 sio_register_driver(sio_device_info_type *pHead)
{
    uint32               i_save;              /* Variable for INTLOCK */
    sio_device_info_type *pInfo;
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

    /* To prevent more than one process from registering driver */
    INTLOCK_SAV( i_save);                   

    if(sio_device_last_item >= SIO_DEVICE_NUM_MAX)
    {
        INTFREE_SAV( i_save);
        ERR( "Too many sio driver being registered",0,0,0);
        return 0;
    }

    pInfo = &(sio_device_head[sio_device_last_item]);

    if( pHead->HW_open != NULL ) {
        pInfo->HW_open = pHead->HW_open; }
    else { 
        pInfo->HW_open = sio_wrapper_null_open; }
    
    if( pHead->HW_close != NULL ) {
        pInfo->HW_close = pHead->HW_close; }
    else {
        pInfo->HW_close = sio_wrapper_null_close; }
    
    if( pHead->HW_ioctl != NULL ) {
        pInfo->HW_ioctl = pHead->HW_ioctl; }
    else {
        pInfo->HW_ioctl = sio_wrapper_null_ioctl; }

    if( pHead->HW_transmit != NULL ) {
        pInfo->HW_transmit = pHead->HW_transmit; }
    else {
        pInfo->HW_transmit = sio_wrapper_null_transmit; }
    
    if( pHead->HW_flush_tx != NULL ) {
        pInfo->HW_flush_tx = pHead->HW_flush_tx; }
    else {
        pInfo->HW_flush_tx = sio_wrapper_null_flush_tx; }

    if( pHead->HW_disable_device != NULL ) {
        pInfo->HW_disable_device = pHead->HW_disable_device; }
    else {
        pInfo->HW_disable_device = sio_wrapper_null_disable_device; }

    if( pHead->HW_control_transmit != NULL ) {
        pInfo->HW_control_transmit = pHead->HW_control_transmit; }
    else {
        pInfo->HW_control_transmit = sio_wrapper_null_control_transmit; }

    if( pHead->HW_control_close != NULL ) {
        pInfo->HW_control_close = pHead->HW_control_close; }
    else {
        pInfo->HW_control_close = sio_wrapper_null_control_close; }
    
    if( pHead->HW_control_open != NULL ) {
        pInfo->HW_control_open = pHead->HW_control_open; }
    else {
        pInfo->HW_control_open = sio_wrapper_null_control_open; }
    
    if( pHead->device_id != 0 ) {
        pInfo->device_id = pHead->device_id; }
    else {
        pInfo->device_id = 0; }

    pInfo->magic_num = SIO_DEVICE_HEAD_MAGIC_NUM;

    pInfo->forward_port = pHead->forward_port;

    pInfo->forward_port_id = SIO_PORT_NULL;

    sio_device_last_item++;

    INTFREE_SAV( i_save);
    
    return 1;
}



/*===========================================================================

FUNCTION sio_device_id_lookup                         EXTERNAL FUNCTION

DESCRIPTION
  Look up the corresponding port_id that is defined 
  by the major and minor device ID
      
DEPENDENCIES
  None
                                                       
RETURN VALUE
  -1 or SIO_PORT_NULL = No available driver registered for the port ID
  Others = SIO port ID r driver found
    
SIDE EFFECTS
  None
  
===========================================================================*/
sio_port_id_type  sio_device_id_lookup(uint32 device_id)
{
   int32 i;
   sio_device_info_type *pInfo;

   for(i=0;i<sio_device_last_item;i++)
   {
     pInfo = &(sio_device_head[i]);
     if( pInfo->magic_num != SIO_DEVICE_HEAD_MAGIC_NUM ) {
        continue;
     }
     if( pInfo->device_id == device_id ) {
        return (sio_port_id_type)i;
     }
   }
   return SIO_PORT_NULL;
}


/*===========================================================================

FUNCTION SIO_GET_STREAM_ID                                  REGIONAL FUNCTION

DESCRIPTION
  Return stream ID which is not currently allocated (i.e. opened).  
  INTLOCK_SAV statement and INTFREE_SAV statement makes this function
  re-entrant.
      
DEPENDENCIES
  None
                                                       
RETURN VALUE
  First available stream ID. 
  If no stream identifiers are available, return SIO_NO_STREAM_ID.
    
SIDE EFFECTS
  None
  
===========================================================================*/

sio_stream_id_type sio_get_stream_id( void)
{
  int2               i;           /* Counter                               */
  uint32             i_save;      /* Variable into which current flag 
                                     register will be stored               */
  sio_stream_id_type stream_id = SIO_NO_STREAM_ID;  /* Return Value                          */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  INTLOCK_SAV(i_save);
  for(i = sio_stream_mark; i < SIO_MAX_STREAM; i++)
  {
    if (sio_stream_opened[i] == FALSE)
    {
      stream_id = (sio_stream_id_type)i;
      break;  
    }
  }
  if(stream_id == SIO_NO_STREAM_ID)
  {
      for (i = 0; i < sio_stream_mark; i++)
      {
        if (sio_stream_opened[i] == FALSE)
        {
          stream_id = (sio_stream_id_type)i;
          break;  
        }
      }
  }

  if (stream_id != SIO_NO_STREAM_ID)
  {
    sio_stream_opened[stream_id] = TRUE;
    sio_stream_mark = (stream_id+1)%SIO_MAX_STREAM;      // pointing to the next stream_id.
  }
  
  INTFREE_SAV(i_save);

  return stream_id;
} /* sio_get_stream_id */

/*lint restore*/
/*===========================================================================

FUNCTION SIO_PUT_STREAM_ID                                  REGIONAL FUNCTION

DESCRIPTION
  Allows caller to deallocate or put back stream ID previously allocated.
  INTLOCK_SAV & INTFREE_SAV statements make this procedure re-entrant.
      
DEPENDENCIES
  None
                                                       
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/

void sio_put_stream_id( stream_id_type stream_id)
{
  uint32               i_save;      /* Variable into which current flag 
                                     register will be stored               */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT( stream_id < SIO_MAX_STREAM);

  INTLOCK_SAV( i_save);

  sio_stream_opened[ stream_id] = FALSE;
  
  INTFREE_SAV( i_save);

} /* sio_put_stream_id */


/*===========================================================================

FUNCTION SIO_STREAM_OPEN                                    REGIONAL FUNCTION

DESCRIPTION
  Allows caller to ascertain whether or not stream is open.
      
DEPENDENCIES
  None
                                                       
RETURN VALUE
  TRUE:  If stream open
  FALSE: If stream is not open
  
SIDE EFFECTS
  None
  
===========================================================================*/

boolean sio_stream_open( stream_id_type stream_id)
{
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  return( sio_stream_opened[ stream_id]);
} /* sio_stream_open */


/*===========================================================================

FUNCTION SIO_INITIALIZE_STREAM_OPENED                       REGIONAL FUNCTION

DESCRIPTION
  Allows caller to initialize data structure used in indicating whether or
  not certain streams are open.
      
DEPENDENCIES
  None
                                                       
RETURN VALUE
  
SIDE EFFECTS
  None
  
===========================================================================*/

void sio_initialize_stream_opened( void)
{
  memset( sio_stream_opened, FALSE, SIO_MAX_STREAM);

  memset( sio_stream_port, (int) SIO_PORT_NULL, SIO_MAX_STREAM);

  memset( sio_control_stream_port, (int) SIO_PORT_NULL, SIO_MAX_STREAM);

  return;
} /* sio_initialize_stream_opened */


#if defined (FEATURE_SMD)

/*===========================================================================

FUNCTION sio_open_remap_to_smd_port_needed 

DESCRIPTION
  This function checks to see if an SMD bridge is necessary for the 
  specified port.  An SMD bridge will be necessary if the device
  is not present on this processor. 
      
DEPENDENCIES
  port must be bridge port on dual procssor build
      
RETURN VALUE
  TRUE  if a remap to an SMD port is needed
  FALSE if a remap to an SMD port is not needed
    
SIDE EFFECTS
  None
  
===========================================================================*/
static boolean
sio_open_remap_to_smd_port_needed
( 
  sio_port_id_type  *port_id
)
{
  boolean ret_val = FALSE;
  sio_port_id_type   new_port_id = SIO_PORT_NULL;

  switch (*port_id)
  {
   #if defined(FEATURE_HS_USB) || defined(FEATURE_USB)
    /* If the port is a USB port */
    #ifdef FEATURE_USB_CDC_ACM
    case SIO_PORT_USB_MDM:
    #endif /* FEATURE_USB_CDC_ACM */

    #if defined(FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA)
    case SIO_PORT_USB_SER1:
    #endif /* FEATURE_USB_DIAG || FEATURE_USB_DIAG_NMEA */

    #if defined(FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA)
    case SIO_PORT_USB_SER3:
    #endif /* FEATURE_USB_DIAG || FEATURE_USB_DIAG_NMEA */

    #if defined(FEATURE_USB_PERIPHERAL_MASS_STORAGE) 
    case SIO_PORT_USB_MMC:
    #endif /* FEATURE_USB_PERIPHERAL_MASS_STORAGE */

    #ifdef FEATURE_USB_DIAG_NMEA
    case SIO_PORT_USB_SER2:
    #endif /* FEATURE_USB_DIAG_NMEA */

    case SIO_PORT_USB_RRDM:
      #ifdef FEATURE_NO_USB_HW_ON_PROCESSOR
      ret_val = TRUE;
      #endif /* FEATURE_NO_USB_HW_ON_PROCESSOR */
      break;

    case SIO_PORT_USB_NET_WWAN:
      #ifdef FEATURE_NO_USB_HW_ON_PROCESSOR
      ret_val = TRUE;
      /* These are QMI ports and get remapped to specific ports
      ** as opposed to using dynamic bridges 
      */
      new_port_id = SIO_PORT_SMD_WINMOB_QMI_WWAN;
      #endif /* FEATURE_NO_USB_HW_ON_PROCESSOR */
      break;
   #endif /* FEATURE_USB */

    /* BT port should not be accessed from the MODEM processor */
#ifdef FEATURE_BT
    case SIO_PORT_BT_SPP:
    case SIO_PORT_BT_NA:
      ret_val = FALSE;
      break;
#endif 

    /* Not forwarding RS232 port if it is on the processor already */   
#ifdef FEATURE_FIRST_UART
    case SIO_PORT_UART_MAIN:
      ret_val = FALSE;
      break;
#endif
#ifdef FEATURE_SECOND_UART
    case SIO_PORT_UART_AUX:
      ret_val = FALSE;
      break;
#endif 
#ifdef FEATURE_THIRD_UART
    case SIO_PORT_UART_THIRD:
      ret_val = FALSE;
      break;
#endif

    /* For all other ports (non-USB) */
    default:
    {
      #ifdef IMAGE_APPS_PROC
      /* Bridge all other ports that are being opened from the apps 
      ** processor except shared memory ports.
      */
      if ((*port_id < SIO_PORT_SMD_FIRST )|| 
          (*port_id > SIO_PORT_SMD_LAST  ))
      {
        ret_val = TRUE;
      }
      #endif /* IMAGE_APPS_PROC */

    } /* Port is not USB port */

  } /* switch (port_id) */

  /* Return the port_id of the new port ID */
  *port_id = new_port_id;

  /* And return whether or not we need to remap */
  return ( ret_val );

} /* sio_open_remap_to_smd_port_needed */

#endif /* FEATURE_SMD && FEATURE_SMD_PORT_MGR*/


/*===========================================================================

FUNCTION SIO_OPEN                                       EXTERNALIZED FUNCTION

DESCRIPTION
  This function opens a stream across the serial link.
      
DEPENDENCIES
  sio_init must have been called.  sio_init needs to be called by just one
  task and only once upon power-up.
      
RETURN VALUE
  SIO_NO_STREAM_ID:  The open request could not be completed. (ERROR)

  If successful, the "stream_id" is returned for use in future SIO requests.
    
SIDE EFFECTS
  None
  
===========================================================================*/

sio_stream_id_type sio_open
( 
  sio_open_type *open_ptr                       /* SIO Open structure      */
)
{
  sio_stream_id_type   stream_id;               /* Stream ID               */
  sio_status_type      return_val = SIO_DONE_S; /* Return Value            */
  boolean              rx_defined = FALSE;      /* Is RX defined?          */
  boolean              tx_defined = FALSE;      /* Is TX defined?          */
  sio_ioctl_param_type param;                   /* IOCTL parameter         */
  sio_port_id_type     port_id;                 /* Port ID                 */
  uint32               i_save;                  /* Variable into which
                                                   current flag register
                                                   will be stored          */
#if defined (FEATURE_SMD)
  sio_port_id_type     saved_port_id;           /* Saved Port ID           */
  sio_port_id_type     smd_port_id;             /* SMD port Id if re-mapping */
#endif /* defined FEATURE_SMD */

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT( open_ptr != NULL);
  

#if defined(FEATURE_SMD)
  /* Save the physical port ID before possibly changing port ID to SMD 
  ** re-map port.  Also initialize smd_port_id to the port we are trying
  ** to open 
  */
  smd_port_id = saved_port_id = open_ptr->port_id;

  /* Sanity check, remove in the future */
  if ( sio_open_remap_to_smd_port_needed ( &smd_port_id ) )
  {
     ASSERT(sio_device_head[ saved_port_id].forward_port);
  }

  /* In dual processor builds, bridge all ports that are not local to this
  ** host with a shared memory device.  Right now, there is only a single 
  ** shared memory device for this, however in the future, there should 
  ** probably be a pool of devices from which a free one could be selected.
  */
  if ( sio_device_head[ saved_port_id].forward_port )
  {

    if (smd_port_id == SIO_PORT_NULL)    
    {
 #if defined (FEATURE_SMD_BRIDGE)
    /* Open the bridge connection on the other processor... returned value
    ** will be SMD port on which bridge is established, or SIO_PORT_NULL
    ** if bridge was not established.
    */     
      smd_port_id = SMD_BRIDGE_CONNECT_FUNC (open_ptr->stream_mode,
                                          open_ptr->rx_bitrate,        
                                          open_ptr->tx_bitrate,         
                                          open_ptr->port_id,         
                                          open_ptr->tail_char_used,     
                                          open_ptr->tail_char,     
                                          open_ptr->tx_flow,          
                                          open_ptr->rx_flow);    
 #else  /* if defined (FEATURE_SMD_BRIDGE) */
   #if defined (IMAGE_MODEM_PROC)
      ERR ("MODEM sio_open: Bridging not supported, bridge needed for port=%d",
                                                        open_ptr->port_id,0,0);
   #else
      ERR ("APPS sio_open: Bridging not supported, bridge needed for port=%d",
                                                        open_ptr->port_id,0,0);
   #endif
 #endif /* defined (FEATURE_SMD_BRIDGE) */
    }
    
    if (smd_port_id == SIO_PORT_NULL)
    {
      ERR ("sio_open: remap fails for port=%d",saved_port_id,0,0);
      return SIO_NO_STREAM_ID;
    }

    MSG_HIGH ("sio_open: Re-mapping to SMD, phys=%d, smd=%d",
                                  open_ptr->port_id,smd_port_id,0);

    /* On successful return, the SMD bridging port ID will be
    ** returned.  Change the port_id and open_ptr port
    ** ID for the remainder of this function.
    */
    open_ptr->port_id = smd_port_id;
      
  } /* remap to SMD is TRUE */
#endif /* FEATURE_SMD */

  /* Get port identifier */
  port_id = open_ptr->port_id;
  stream_id = sio_get_stream_id();

  /* If there are no more stream identifiers available,
  ** indicate error and return immediately.  */
  if ( stream_id == SIO_NO_STREAM_ID)
  {
    ERR( "No Stream IDs available",0,0,0);
    return stream_id; 
  }

  open_ptr->stream_id = stream_id;

  if ( open_ptr->stream_mode >= SIO_MAX_MODE)
  { 
    ERR_FATAL( "Invalid stream mode",0,0,0);
  } 

  if ( ( open_ptr->rx_queue    == NULL) && 
       ( open_ptr->tx_queue    == NULL) && 
       ( open_ptr->rx_func_ptr == NULL))
  {
    ERR_FATAL( "sio_open called with both RX and TX undefined",0,0,0);
  }

  /* If tx_queue is defined, a method of handing data to
  ** caller is defined.  Therefore, we take the fact that
  ** tx_queue is defined as evidence that TX is defined.  */  
  if ( open_ptr->tx_queue)
  {
    tx_defined = TRUE;  
  } 
  
  /* If either rx_queue or rx_func_ptr are defined, then
  ** some method of handing data to caller (task or ISR
  ** which called this routine) is defined.  So, we take
  ** the fact that either of these parameters are defined
  ** as evidence that RX is defined.  */
  if ( ( open_ptr->rx_queue) || ( open_ptr->rx_func_ptr))
  {
    rx_defined = TRUE;  
  }
  
  if ( rx_defined && tx_defined)
  {
    open_ptr->stream_type = SIO_STREAM_RXTX;  
  }
  else if ( rx_defined)
  {
    open_ptr->stream_type = SIO_STREAM_RX_ONLY;  
  }
  else
  {
    open_ptr->stream_type = SIO_STREAM_TX_ONLY;  
  }
  
  /* Lock interrupts to prevent un-planned context
  ** switches while within function call.  */
  INTLOCK_SAV( i_save);

  /* Calling the driver that is registered with SIO */
  ASSERT(port_id < SIO_DEVICE_NUM_MAX && port_id > SIO_PORT_NULL);
  if(sio_device_head[ port_id].magic_num != SIO_DEVICE_HEAD_MAGIC_NUM)
  {
      ERR_FATAL( "Incorrect SIO device magic number!",0,0,0);
  }
  else
  {
      ASSERT((sio_device_head[ port_id].HW_open));
      return_val = (sio_device_head[ port_id].HW_open) (open_ptr);
  }


  INTFREE_SAV( i_save);

  if ( return_val != SIO_DONE_S)
  {
    /* If we could not allocate TX, exit
    */
    sio_put_stream_id( stream_id);

#if defined(FEATURE_SMD)
    /* Restore previously saved port_id */
    open_ptr->port_id = saved_port_id;
#endif /* defined (FEATURE_SMD) */

    return SIO_NO_STREAM_ID;  
  }
  
  /* Save port ID */
  INTLOCK_SAV( i_save);
  sio_stream_port[ stream_id] = open_ptr->port_id;
  INTFREE_SAV( i_save);

  /* Set flow control method.
  */
  param.flow_ctl.tx_flow = open_ptr->tx_flow;
  param.flow_ctl.rx_flow = open_ptr->rx_flow;

  sio_ioctl( stream_id, SIO_IOCTL_SET_FLOW_CTL, &param);

#if defined(FEATURE_SMD)
  /* Restore previously saved port_id */
  open_ptr->port_id = saved_port_id;
#endif /* defined (FEATURE_SMD) */

  return stream_id;
} /* sio_open */


/*===========================================================================

FUNCTION SIO_CLOSE                                      EXTERNALIZED FUNCTION
                                                                             
DESCRIPTION
  This procedure does all the book-keeping necessary to close a stream.  
  Optional tcb_ptr and sigs can be passed to this function, if task wants
  to be signalled when the last bit of pending transmission leaves phone.
                  
DEPENDENCIES
  None
      
RETURN VALUE
  None
  
SIDE EFFECTS
  close_func_ptr will be called when all pending data has left transmitter.
  
===========================================================================*/

void sio_close
( 
  sio_stream_id_type stream_id,             /* Stream ID                   */
  void             (*close_func_ptr)(void)  /* Function to call when 
                                               transmission is complete.   */
)
{
  uint32             i_save;                /* Variable into which current 
                                               flag register will be 
                                               stored                      */
  sio_port_id_type   port_id;               /* Port which corresponds to
                                               stream_id                   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( sio_stream_open( stream_id) == FALSE 
       || sio_stream_port[stream_id] == SIO_PORT_NULL )
  {
    ERR( "Attempt to close stream which was not opened",0,0,0);
    return;
  }
  
  /* This procedure may be entered simultaneously by multiple
  ** callers.  So, lock interrupts to prevent un-wanted 
  ** context switch.
  */
  INTLOCK_SAV( i_save);

  sio_put_stream_id( stream_id);

  port_id = sio_stream_port[ stream_id];
  
  ASSERT(port_id < SIO_DEVICE_NUM_MAX && port_id > SIO_PORT_NULL);
  if(sio_device_head[ port_id].magic_num != SIO_DEVICE_HEAD_MAGIC_NUM)
  {
      ERR_FATAL( "Incorrect SIO device magic number!",0,0,0);
  }
  else
  {
     ASSERT((sio_device_head[ port_id].HW_close));
      (sio_device_head[ port_id].HW_close) (stream_id, close_func_ptr);
  }

  sio_stream_port[ stream_id] = SIO_PORT_NULL;

  INTFREE_SAV( i_save);
  return;
} /* sio_close */

/*===========================================================================

FUNCTION SIO_CLOSE_EXT                           EXTERNALIZED FUNCTION

DESCRIPTION
  This procedure does all the book-keeping necessary to close a stream.  
  Optional tcb_ptr and sigs can be passed to this function, if task wants
  to be signalled when the last bit of pending transmission leaves phone.
                  
DEPENDENCIES
  This function is only supported in shared memory ports.  Bridges are
  not included?. 
      
RETURN VALUE
  None
  
SIDE EFFECTS
  close_func_ptr will be called when all pending data has left transmitter.
  
===========================================================================*/

void sio_close_ext
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  /* Function to call when transmission is complete.                       */
  void (*close_func_ptr)(void * cb_data),
  void *ext_cb_data
)
{
  uint32             i_save;                /* Variable into which current 
                                               flag register will be 
                                               stored                      */
  sio_port_id_type   port_id;               /* Port which corresponds to
                                               stream_id                   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( sio_stream_open( stream_id) == FALSE 
       || sio_stream_port[stream_id] == SIO_PORT_NULL )
  {
    ERR( "Attempt to close stream which was not opened",0,0,0);
    return;
  }
  
  /* This procedure may be entered simultaneously by multiple
  ** callers.  So, lock interrupts to prevent un-wanted 
  ** context switch.
  */
  INTLOCK_SAV( i_save);

  sio_put_stream_id( stream_id);

  port_id = sio_stream_port[ stream_id];

  sio_stream_port[ stream_id] = SIO_PORT_NULL;
  
  /* Close RX and/or TX streams.  Also, do
  ** all of the necessary book-keeping.  
  */
  switch ( port_id)
  {
#ifdef FEATURE_SMD
    case SIO_PORT_SMD_DS:
    case SIO_PORT_SMD_DIAG:
    case SIO_PORT_SMD_DIAG_APPS:
    case SIO_PORT_SMD_DIAG_MODEM:
    case SIO_PORT_SMD_DIAG_QDSP:
    case SIO_PORT_SMD_RPC_CALL:
    case SIO_PORT_SMD_RPC_REPLY:
    case SIO_PORT_SMD_BT:
    case SIO_PORT_SMD_CONTROL:
    case SIO_PORT_SMD_MEMCPY_SPARE1:
    case SIO_PORT_SMD_DATA1:
    case SIO_PORT_SMD_DATA2:
    case SIO_PORT_SMD_DATA3:
    case SIO_PORT_SMD_DATA4:
    case SIO_PORT_SMD_DATA5:
    case SIO_PORT_SMD_DATA6:
    case SIO_PORT_SMD_DATA7:
    case SIO_PORT_SMD_DATA8:
    case SIO_PORT_SMD_DATA9:
    case SIO_PORT_SMD_DATA10:
    case SIO_PORT_SMD_DATA11:
    case SIO_PORT_SMD_DATA12:
    case SIO_PORT_SMD_DATA13:
    case SIO_PORT_SMD_DATA14:
    case SIO_PORT_SMD_DATA15:
    case SIO_PORT_SMD_DATA16:
    case SIO_PORT_SMD_DATA17:
    case SIO_PORT_SMD_DATA18:
    case SIO_PORT_SMD_DATA19:
    case SIO_PORT_SMD_DATA20:
    case SIO_PORT_SMD_GPS_NMEA:
      smd_sio_close_ext( port_id, close_func_ptr, ext_cb_data);
      break;

#ifdef FEATURE_SMD_BRIDGE

    case SIO_PORT_SMD_BRIDGE_1:
    case SIO_PORT_SMD_BRIDGE_2:
    case SIO_PORT_SMD_BRIDGE_3:
    case SIO_PORT_SMD_BRIDGE_4:
    case SIO_PORT_SMD_BRIDGE_5:
        /* Close the shared memory port */
        smd_sio_close_ext (port_id, 
                           sio_smd_bridge_port_close_cb, 
                         (void *)(int32) port_id);

      /* Close the bridge */
        SMD_BRIDGE_DISCONNECT_FUNC (port_id,
                               NULL,
                               close_func_ptr,
                               ext_cb_data);   
      break;

#endif /* FEATURE_SMD_BRIDGE */
#endif /* FEATURE_SMD */
    default:
      ERR_FATAL( "Invalid port id",0,0,0);
      break;
  } /* switch port_id */

  INTFREE_SAV( i_save);

  return;
} /* sio_close_ext */


/*===========================================================================

FUNCTION SIO_FLUSH_TX                                  EXTERNALIZED FUNCTION
                                                                             
DESCRIPTION
  This function will cause any queued data to be sent out of the UART, with 
  the caveat that a callback will be called when it's done.  This assumes
  that the user won't queue more data until after the callback is executed.
                    
DEPENDENCIES
  None
      
RETURN VALUE
  None
  
SIDE EFFECTS
  close_func_ptr will be called when all pending data has left transmitter.
  
===========================================================================*/

void sio_flush_tx
( 
  sio_stream_id_type stream_id,             /* Stream ID                   */
  void             (*flush_func_ptr)(void)  /* Function to call when 
                                               transmission is complete.   */
)
{
  uint32             i_save;                /* Saving the status reg       */
  sio_port_id_type   port_id;               /* Port which corresponds to
                                               stream_id                   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( sio_stream_open( stream_id) == FALSE ||
       sio_stream_port[stream_id] == SIO_PORT_NULL )
  {
    ERR( "Attempt to use stream which was not opened",0,0,0);
    return;
  }
  
  /* This procedure may be entered simultaneously by multiple
  ** callers.  So, lock interrupts to prevent un-wanted 
  ** context switch.
  */
  INTLOCK_SAV( i_save);

  port_id = sio_stream_port[ stream_id];

  ASSERT(port_id < SIO_DEVICE_NUM_MAX && port_id > SIO_PORT_NULL);
  if(sio_device_head[ port_id].magic_num != SIO_DEVICE_HEAD_MAGIC_NUM)
  {
      ERR_FATAL( "Incorrect SIO device magic number!",0,0,0);
  }
  else
  {
     ASSERT((sio_device_head[ port_id].HW_flush_tx));
      (sio_device_head[ port_id].HW_flush_tx) (stream_id, port_id, flush_func_ptr);
  }

  INTFREE_SAV( i_save);
} /* sio_flush_tx */

/*===========================================================================

FUNCTION SIO_FLUSH_TX_EXT                           EXTERNALIZED FUNCTION

DESCRIPTION
  This function will transmit any queued data and then call the registered
  callback.

DEPENDENCIES
  This is only supported on Shared memroy ports
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/

void sio_flush_tx_ext
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  /* Function to call when transmission is complete.                       */
  void (*flush_func_ptr)(void * cb_data),
  void *ext_cb_data
)
{
  sio_port_id_type   port_id;               /* Port which corresponds to
                                               stream_id                   */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( sio_stream_open( stream_id) == FALSE ||
       sio_stream_port[stream_id] == SIO_PORT_NULL )
  {
    ERR( "Attempt to use stream which was not opened",0,0,0);
    return;
  }
  
  port_id = sio_stream_port[ stream_id];

  /* Flush Tx.
  */
  switch ( port_id)
  {
#ifdef FEATURE_SMD
    case SIO_PORT_SMD_DS:
    case SIO_PORT_SMD_DIAG:
    case SIO_PORT_SMD_DIAG_APPS:
    case SIO_PORT_SMD_DIAG_MODEM:
    case SIO_PORT_SMD_DIAG_QDSP:
    case SIO_PORT_SMD_RPC_CALL:
    case SIO_PORT_SMD_RPC_REPLY:
    case SIO_PORT_SMD_BT:
    case SIO_PORT_SMD_CONTROL:
    case SIO_PORT_SMD_MEMCPY_SPARE1:
    case SIO_PORT_SMD_DATA1:
    case SIO_PORT_SMD_DATA2:
    case SIO_PORT_SMD_DATA3:
    case SIO_PORT_SMD_DATA4:
    case SIO_PORT_SMD_DATA5:
    case SIO_PORT_SMD_DATA6:
    case SIO_PORT_SMD_DATA7:
    case SIO_PORT_SMD_DATA8:
    case SIO_PORT_SMD_DATA9:
    case SIO_PORT_SMD_DATA10:
    case SIO_PORT_SMD_DATA11:
    case SIO_PORT_SMD_DATA12:
    case SIO_PORT_SMD_DATA13:
    case SIO_PORT_SMD_DATA14:
    case SIO_PORT_SMD_DATA15:
    case SIO_PORT_SMD_DATA16:
    case SIO_PORT_SMD_DATA17:
    case SIO_PORT_SMD_DATA18:
    case SIO_PORT_SMD_DATA19:
    case SIO_PORT_SMD_DATA20:
    case SIO_PORT_SMD_GPS_NMEA:

#ifdef FEATURE_SMD_BRIDGE
    case SIO_PORT_SMD_BRIDGE_1:
    case SIO_PORT_SMD_BRIDGE_2:
    case SIO_PORT_SMD_BRIDGE_3:
    case SIO_PORT_SMD_BRIDGE_4:
    case SIO_PORT_SMD_BRIDGE_5:
#endif /* FEATURE_SMD_BRIDGE */

      smd_sio_flush_tx_ext(port_id, flush_func_ptr, ext_cb_data);
      break;

#endif /* FEATURE_SMD */
    default:
      ERR_FATAL( "Invalid port id",0,0,0);
      break;
  } /* switch port_id */

  return;
} /* sio_flush_tx_ext */


/*===========================================================================

FUNCTION SIO_TRANSMIT                                   EXTERNALIZED FUNCTION

DESCRIPTION
  This function will transmit a packet or if transmitter is busy, schedule
  the transmission of a packet.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
void sio_transmit
( 
  sio_stream_id_type stream_id,           /* Stream ID                     */
  dsm_item_type     *tx_ptr               /* Packet for transmission       */
)
{
  sio_port_id_type   port_id;             /* SIO Port ID                   */
  uint32             i_save;              /* Variable into which current 
                                             flag register will be 
                                             stored                        */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( tx_ptr == NULL) 
  {
     MSG_ERROR("NULL dsm_item ptr for transmit, stream_id: %d", stream_id, 0, 0 );
     return;
  }
  ASSERT( sio_stream_open( stream_id) == TRUE &&
          sio_stream_port[stream_id] != SIO_PORT_NULL );

  /* This procedure may be entered simultaneously by multiple
  ** callers.  So, lock interrupts to prevent un-wanted 
  ** context switch.
  */
  INTLOCK_SAV( i_save);

  port_id = sio_stream_port[ stream_id];

  ASSERT(port_id < SIO_DEVICE_NUM_MAX && port_id > SIO_PORT_NULL);
  if(sio_device_head[ port_id].magic_num != SIO_DEVICE_HEAD_MAGIC_NUM)
  {
      ERR_FATAL( "Incorrect SIO device magic number!",0,0,0);
  }
  else
  {
     ASSERT((sio_device_head[ port_id].HW_transmit));
      (sio_device_head[ port_id].HW_transmit) (stream_id, port_id, tx_ptr);
  }

  INTFREE_SAV( i_save);

} /* sio_transmit */


/*===========================================================================

FUNCTION SIO_INIT                                       EXTERNALIZED FUNCTION

DESCRIPTION
  Initializes SIO.
  
DEPENDENCIES
  Should be called just once.
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
void sio_init ( void )
{
  uint32 i_save;
/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/

  sio_device_last_item = 0;
  /* Initialize everything in a critical section.
  */
  INTLOCK_SAV( i_save);

#if defined (FEATURE_RUNTIME_DEVMAP) && (!defined (IMAGE_APPS_PROC) || defined (FEATURE_STANDALONE))
  rdm_init();
#endif

#ifdef FEATURE_SERIAL_DEVICE_MAPPER
  sdm_init();
#endif

  /* Initialize SIO wrapper functions, this is done until all devices that uses the SIO
   * service start to register their own API */ 
  sio_wrapper_init();

  sio_initialize_stream_opened();

  INTFREE_SAV( i_save);
} /* sio_init */

/*===========================================================================

FUNCTION SIO_IOCTL_HELPER                                      LOCAL FUNCTION

DESCRIPTION
  Common code for sio_ioctl, sio_control_ioctl.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
static void sio_ioctl_helper
(
  sio_stream_id_type    stream_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type *param,
  boolean               bIsControlStream
)
{
  sio_port_id_type   port_id;             /* SIO Port ID                   */
  uint32             i_save;              /* Variable into which current 
                                             flag register will be stored  */
  sio_port_id_type  *avail_port;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if (TRUE == bIsControlStream)
  {
    avail_port = sio_control_stream_port;
  }
  else
  {
    avail_port = sio_stream_port;
  }

  /*---------------------------------------------
   Cannot do anything if the stream is not open.
  ---------------------------------------------*/
  if ( sio_stream_open( stream_id) == FALSE ||
       avail_port[stream_id] == SIO_PORT_NULL )
  {
    ERR( "sio_ioctl called with closed (or never opened) stream",0,0,0);
   return;
  }
  
  /* This procedure may be entered simultaneously by multiple
  ** callers.  So, lock interrupts to prevent un-wanted 
  ** context switch.
  */
  INTLOCK_SAV( i_save);

  port_id = avail_port[ stream_id];

  ASSERT(port_id < SIO_DEVICE_NUM_MAX && port_id > SIO_PORT_NULL);
  if(sio_device_head[ port_id].magic_num != SIO_DEVICE_HEAD_MAGIC_NUM)
  {
      ERR_FATAL( "Incorrect SIO device magic number!",0,0,0);
  }
  else
  {
     ASSERT((sio_device_head[ port_id].HW_ioctl));
      (sio_device_head[ port_id].HW_ioctl) (stream_id, port_id, cmd, param);
  }

  INTFREE_SAV( i_save);
} /* sio_ioctl_helper */


/*===========================================================================

FUNCTION SIO_IOCTL                                      EXTERNALIZED FUNCTION

DESCRIPTION
  Allows control of open stream.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void sio_ioctl
(
  sio_stream_id_type    stream_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type *param
)
{
  sio_ioctl_helper( stream_id, cmd, param, FALSE );
} /* sio_ioctl */


/*===========================================================================

FUNCTION SIO_CONTROL_IOCTL                              EXTERNALIZED FUNCTION

DESCRIPTION
  Allows control of open control stream.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void sio_control_ioctl
(
  sio_stream_id_type    stream_id,
  sio_ioctl_cmd_type    cmd,
  sio_ioctl_param_type *param
)
{
  sio_ioctl_helper( stream_id, cmd, param, TRUE );
} /* sio_control_ioctl */


/*===========================================================================

FUNCTION SIO_DISABLE_DEVICE                             EXTERNALIZED FUNCTION

DESCRIPTION
  This routine disables given device, including turning off clock to save
  power.  This is different from UART powerdown feature which attempts to
  save power with minimal loss of data.  Instead, this function is meant 
  for customers who may not want to use both UARTs.
  
  Effects of this function may be reversed by calling sio_init.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/

void sio_disable_device
(
  sio_port_id_type   port_id              /* SIO Port ID                   */  
)
{
  uint32             i_save;              /* Variable into which current 
                                             flag register will be stored  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  INTLOCK_SAV( i_save);

  ASSERT(port_id < SIO_DEVICE_NUM_MAX && port_id > SIO_PORT_NULL);
  if(sio_device_head[ port_id].magic_num != SIO_DEVICE_HEAD_MAGIC_NUM)
  {
      ERR_FATAL( "Incorrect SIO device magic number!",0,0,0);
  }
  else
  {
     ASSERT((sio_device_head[ port_id].HW_disable_device));
      (sio_device_head[ port_id].HW_disable_device) (port_id);
  }

  INTFREE_SAV( i_save);
} /* sio_disable_device */


/*===========================================================================

FUNCTION SIO_IS_BAUD_VALID                             EXTERNALIZED FUNCTION

DESCRIPTION
  This routine checks if the specified baud rate is valid under the
  current feature set.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the baud rate is valid under the current feature set.
  FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/

boolean sio_is_baud_valid
(
  sio_bitrate_type   check_baud              /* SIO baud rate              */
)
{
  boolean return_val = FALSE;

 /* !!!! Must keep this in sync with sio.h:sio_bitrate_type def'n. !!!! */

  switch ( check_baud )
  {
#ifndef FEATURE_JCDMA_1X
    case SIO_BITRATE_300:
    case SIO_BITRATE_600:
    case SIO_BITRATE_1200:
    case SIO_BITRATE_2400:
    case SIO_BITRATE_4800:
    case SIO_BITRATE_9600:
    case SIO_BITRATE_19200:
    case SIO_BITRATE_38400:
    case SIO_BITRATE_57600:
#endif
    case SIO_BITRATE_115200:
    case SIO_BITRATE_230400:
    case SIO_BITRATE_460800:
#if defined(FEATURE_JCDMA_DS) || defined(FEATURE_JCDMA_SIO)
    case SIO_BITRATE_14400:
    case SIO_BITRATE_76800:
#endif
    case SIO_BITRATE_921600:
    case SIO_BITRATE_2900000:
    case SIO_BITRATE_3200000:
    case SIO_BITRATE_3686400:
    case SIO_BITRATE_4000000:
#ifdef MSMU_MVR_HS_CUSTOM
    case SIO_BITRATE_HS_CUSTOM:
#endif
      return_val = TRUE;  /* baud rate is valid */
      break;

    default:
      /* The baud rate is not valid under the current
       * FEATURE set. We want to return FALSE.
       */
      break;
  }

  return ( return_val );

} /* sio_is_baud_valid */

/*===========================================================================

FUNCTION SIO_CONTROL_OPEN                               EXTERNALIZED FUNCTION

DESCRIPTION
  This function opens a control stream across the serial link.
      
DEPENDENCIES
  sio_init must have been called.  sio_init needs to be called by just one
  task and only once upon power-up.
  
  Only USE currently support control streams associated with a data stream.  
      
RETURN VALUE
  SIO_NO_STREAM_ID:  The open request could not be completed. (ERROR)

  If successful, the "stream_id" is returned for use in future SIO requests.
    
SIDE EFFECTS
  None
  
===========================================================================*/

sio_stream_id_type sio_control_open
( 
  sio_open_type *open_ptr                       /* SIO Open structure      */
)
{
  sio_stream_id_type   stream_id;               /* Stream ID               */
  sio_status_type      return_val = SIO_DONE_S; /* Return Value            */
  sio_port_id_type     port_id;                 /* Port ID                 */
  
#if defined (FEATURE_SMD)
  sio_port_id_type     saved_port_id;           /* Saved Port ID           */
  sio_port_id_type     smd_port_id;             /* SMD port Id if re-mapping */
#endif /* defined FEATURE_SMD */
 
  /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  
  ASSERT( open_ptr != NULL);
  
  

#if defined(FEATURE_SMD)
  /* Save the physical port ID before possibly changing port ID to SMD 
  ** re-map port.  Also initialize smd_port_id to the port we are trying
  ** to open 
  */
  smd_port_id = saved_port_id = open_ptr->port_id;

  /* Sanity check, remove in the future */
  if ( sio_open_remap_to_smd_port_needed ( &smd_port_id ) )
  {
     ASSERT(sio_device_head[ saved_port_id].forward_port);
  }

  /* In dual processor builds, bridge all ports that are not local to this
  ** host with a shared memory device.  Right now, there is only a single 
  ** shared memory device for this, however in the future, there should 
  ** probably be a pool of devices from which a free one could be selected.
  */
  if ( sio_device_head[ saved_port_id].forward_port )
  {

    if (smd_port_id == SIO_PORT_NULL)    
    {
   #if defined (IMAGE_MODEM_PROC)
      ERR ("MODEM sio_control_open: Bridging not supported, bridge needed for port=%d",
                                                                open_ptr->port_id,0,0);
   #else
      ERR ("APPS sio_control_open: Bridging not supported, bridge needed for port=%d",
                                                                open_ptr->port_id,0,0);
   #endif
    }
    
    if (smd_port_id == SIO_PORT_NULL)
    {
      ERR ("sio_control_open: remap fails for port=%d",
                                 open_ptr->port_id,0,0);
      return SIO_NO_STREAM_ID;
    }

    MSG_HIGH ("sio_control_open: Re-mapping to SMD, phys=%d, smd=%d",
                                     open_ptr->port_id,smd_port_id,0);

    /* On successful return, the SMD bridging port ID will be
    ** returned.  Change the port_id and open_ptr port
    ** ID for the remainder of this function.
    */
    open_ptr->port_id = smd_port_id;
      
  } /* remap to SMD is TRUE */
#endif /* FEATURE_SMD */
  
  /* Get port identifier
  */
  port_id = open_ptr->port_id;

  stream_id = sio_get_stream_id();

  /* If there are no more stream identifiers available,
  ** indicate error and return immediately.
  */
  if ( stream_id == SIO_NO_STREAM_ID)
  {
    ERR( "No Stream IDs available",0,0,0);
    return stream_id; 
  }

  open_ptr->stream_id = stream_id;

  if ( ( open_ptr->tx_queue    == NULL) || 
       ( open_ptr->rx_queue    == NULL) && 
       ( open_ptr->rx_func_ptr == NULL))
  {
    ERR_FATAL( "sio_control_open called with either RX or TX undefined",0,0,0);
  }

  
  /* Lock interrupts to prevent un-planned context
  ** switches while within function call.
  */
  INTLOCK();

  /* Calling the driver that is registered with SIO */
  ASSERT(port_id < SIO_DEVICE_NUM_MAX && port_id > SIO_PORT_NULL);
  if(sio_device_head[ port_id].magic_num != SIO_DEVICE_HEAD_MAGIC_NUM)
  {
      ERR_FATAL( "Incorrect SIO device magic number!",0,0,0);
  }
  else
  {
      ASSERT((sio_device_head[ port_id].HW_control_open));
      return_val = (sio_device_head[ port_id].HW_control_open) (open_ptr);
  }

  INTFREE();

  if ( return_val != SIO_DONE_S)
  {
    /* If we could not allocate TX, exit */
    sio_put_stream_id( stream_id);

#if defined(FEATURE_SMD)
    /* Restore previously saved port_id */
    open_ptr->port_id = saved_port_id;
#endif /* defined (FEATURE_SMD) */

    return SIO_NO_STREAM_ID;  
  }
  else
  {
    INTLOCK();
    sio_control_stream_port[ stream_id] = open_ptr->port_id;
    INTFREE();

#if defined(FEATURE_SMD)
    /* Restore previously saved port_id */
    open_ptr->port_id = saved_port_id;
#endif /* defined (FEATURE_SMD) */

    return stream_id;
  }

} /* sio_control_open */


/*===========================================================================

FUNCTION SIO_CONTROL_CLOSE                              EXTERNALIZED FUNCTION
                                                                             
DESCRIPTION
  This procedure does all the book-keeping necessary to close a control stream.  
  Optional tcb_ptr and sigs can be passed to this function, if task wants
  to be signalled when the last bit of pending transmission leaves phone.
                  
DEPENDENCIES
  Only USB supports control streams. 
          
RETURN VALUE
  None
  
SIDE EFFECTS
  close_func_ptr will be called when all pending data has left transmitter.
  
===========================================================================*/

void sio_control_close
( 
  sio_stream_id_type stream_id,             /* Stream ID                   */
  void             (*close_func_ptr)(void)  /* Function to call when 
                                               transmission is complete.   */
)
{
  uint32             i_save;              /* Variable into which current 
                                             flag register will be stored  */
  sio_port_id_type   port_id;                   /* Port ID                 */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */
  if ( sio_stream_open( stream_id) == FALSE ||
       SIO_PORT_NULL == sio_control_stream_port[stream_id] )
  {
    ERR( "Attempt to use stream which was not opened",0,0,0);
    return;
  }
  
  /* This procedure may be entered simultaneously by multiple
  ** callers.  So, lock interrupts to prevent un-wanted 
  ** context switch.
  */
  INTLOCK_SAV( i_save);

  sio_put_stream_id( stream_id);
  port_id = sio_control_stream_port[ stream_id];

  ASSERT(port_id < SIO_DEVICE_NUM_MAX && port_id > SIO_PORT_NULL);
  if(sio_device_head[ port_id].magic_num != SIO_DEVICE_HEAD_MAGIC_NUM)
  {
      ERR_FATAL( "Incorrect SIO device magic number!",0,0,0);
  }
  else
  {
     ASSERT((sio_device_head[ port_id].HW_control_close));
      (sio_device_head[ port_id].HW_control_close) (stream_id, close_func_ptr);
  }

  sio_control_stream_port[ stream_id] = SIO_PORT_NULL;

  INTFREE_SAV( i_save);
} /* sio_control_close */

/*===========================================================================

FUNCTION SIO_CONTROL_TRANSMIT                           EXTERNALIZED FUNCTION

DESCRIPTION
  This function will transmit a packet or if transmitter is busy, schedule
  the transmission of a packet.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/
void sio_control_transmit
( 
  sio_stream_id_type stream_id,           /* Stream ID                     */
  dsm_item_type     *tx_ptr               /* Packet for transmission       */
)
{
  sio_port_id_type   port_id;             /* SIO Port ID                   */
  uint32             i_save;              /* Variable into which current 
                                             flag register will be stored  */
/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - */

  if ( tx_ptr == NULL) 
  {
     MSG_ERROR("NULL dsm_item ptr for transmit, stream_id: %d", stream_id, 0, 0 );
     return;
  }
  ASSERT( sio_stream_open( stream_id) == TRUE && 
          sio_control_stream_port[ stream_id] != SIO_PORT_NULL);

  /* This procedure may be entered simultaneously by multiple
  ** callers.  So, lock interrupts to prevent un-wanted 
  ** context switch.
  */
  INTLOCK_SAV( i_save);

  port_id = sio_control_stream_port[ stream_id];

  ASSERT(port_id < SIO_DEVICE_NUM_MAX && port_id > SIO_PORT_NULL);
  if(sio_device_head[ port_id].magic_num != SIO_DEVICE_HEAD_MAGIC_NUM)
  {
      ERR_FATAL( "Incorrect SIO device magic number!",0,0,0);
  }
  else
  {
     ASSERT((sio_device_head[ port_id].HW_control_transmit));
      (sio_device_head[ port_id].HW_control_transmit) (stream_id, tx_ptr);
  }

  INTFREE_SAV( i_save);
} /* sio_control_transmit */
