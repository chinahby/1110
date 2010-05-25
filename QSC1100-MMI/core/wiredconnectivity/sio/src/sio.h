#ifndef SIO_H
#define SIO_H

/*===========================================================================

                S E R I A L    I / O    S E R V I C E S

                       H E A D E R    F I L E

DESCRIPTION
  This file contains types and declarations associated with the DMSS
  Serial I/O Services.

Copyright (c) 1990-1993, 1997-2008 QUALCOMM Incorporated.  All Rights Reserved.
Qualcomm Confidential and Proprietary

===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $PVCSPath: L:/src/asw/COMMON/vcs/sio.h_v   1.20   17 Apr 2003 17:19:50   chenc  $
  $Header: //depot/asic/msmshared/services/sio/02.00.XX/sio.h#1 $ $DateTime: 2008/10/19 17:40:04 $ $Author: vaghasia $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
09/12/08   amv     Added SMD ports for MODEM, APP and QDSP processor
08/12/08   cpm     Add sio_control_ioctl API call.
07/30/08   cpm     Reserve some SMD ports for RmNET usage.
06/23/08   bfc     Added new IOCTL for SMD data stacking.
04/24/08   rh      Add new API:  sio_device_id_lookup
03/21/08   bfc     Added new ports for DATA.
01/31/08   rh      Add sio_open and control_open to registerable function
11/21/07   rh      Add IOCTL for RX line state detection
10/08/07   ar      Added SIO_PORT_SMD_WINMOB_QVP_MVS & DATA.
09/24/07   rh      Add new IOCTL function to control TX BREAK event
07/12/07   rh      Add new port - SIO_PORT_USB_SER3
10/11/06   rh      Adding support for FEATURE_HS_USB
09/25/06   rh      Add support for custom baud rate
10/24/06   bfc     Modified some enumarations that are used by the SMD
09/27/06   bfc     Added get/set drop pend ioctl (for SMD ports)
09/05/06   ptm     Add get/set buffer size IOCTL (for SMD ports).
07/31/06   sr      Add support for 3000kbps
07/27/06   rh      Add new sio port SIO_PORT_SMD_GPS_NMEA
07/05/06   bfc     Added some metacomments that are needed by smd_bridge
06/19/06   bt      Added FEATURE_USB_BREW_EXTENDED_DEVICE support. 
05/24/06   bfc     Added additional support for RI and CD signals
05/18/06   sr      Add support for 921.6kbps
05/10/06   rh      Adding a new flow control type SIO_CTSRFR_AUTO_FCTL
05/01/06   bfc     Added featurization for smd_bridge related code
04/25/06   ptm     Added SMD port for GPS NMEA and RPC reply.
03/28/06   bfc     Changed the arguement for extended callback function from
                   a uint32 to a void *
01/26/06   bfc     Added the DISABLE/ENABLE_CD_EVENT_EXT feature for SMD streams.
                   Also added sio_close_ext and sio_flush_tx_ext which work 
                   specifically with SMD streams.
01/04/06   ptm     Restore SMEM code, expanded SMD ports as required.
09/28/05   TMR     Added SIO_PORT_SMEMDS_BRIDGE_1 and other SMD definitions
08/19/05   cp      Support for APS virtual port
06/05/05   TMR     Merged in MSM7500 code
06/01/05   adm     Added support for additional SMD DIAG pipe SIO_PORT_SMEMDS_DIAG_RSP
04/14/05   adm     Added support for dataxfr_mode and xfrflow_type
                   attributes for SMD
03/08/05   cc      Merged in OSIFDev_Dec04 branck. 
01/11/05   cc      Duplicate USB port entries with old names in 
                   sio_port_id_type for better backwards compatibility.
12/23/04   cc      Renamed SIO_PORT_USB_CDC_ACM, _DIAG, _NMEA to be 
                   SIO_PORT_USB_MDM,_SER1 and _SER2 respectively. 
11/24/04   cc      Added SIO_PORT_USB_RRDM and SIO_PORT_USB_NET_WWAN support. 
                   Added sio_control_open/close/transmit() APIs. 
11/09/04   adm     Modified sio_get_stream_id to return fixed stream_is'd
                   in case of SMEMDS related ports 
12/09/04   cc      Define SIO_PAYLOAD define to be DSM_PS_RAW_APP_PAYLOAD for 
                   WCDMA_PLT and GSM_PLT.
10/20/04   adm     Added support for multiple DS pipes. 
12/02/03   cc      Changed SIO_PAYLOAD define to be DSM_PS_RAW_APP_PAYLOAD
                   Or PS_RAW_APP_PAYLOAD to sync with new DSM. 
11/24/03   cc      Added FEATURE_JCDMA_SIO and OR it with FEATURE_JCDMA_DS.                     
09/28/03   dsp     Added BT SPP status support.
02/21/03   cc      Added FEATURE_USB_DIAG_NMEA support. 
12/03/02   wal     Added FEATURE_BT_SPP.  Removed 
                   FEATURE_BT_SIO_API_ENHANCEMENT.
11/22/02   ltan    Replaced FEATURE_MMGPS with FEATURE_MMGPS_CSD 
07/15/02   ry      Added support for the LSM_CSD port
07/15/02   ak      Added FEATURE_DATA_MM support.
02/21/02   ry    Added the support for FEATURE_BT_SIO_API_ENHANCEMENT. 
02/06/02   cc      Add third UART support. 
11/07/01   cc      Move #define SIO_PAYLOAD from siors232.c to sio.h
10/16/01   cc      Merge in Mods of Simba from MSM5200_SIO.00.00.17, includes 
                   following 1 item
05/01/01   dwp     Add ioctl command SIO_IOCTL_GET_CURRENT_RTS and support it
                   with rts_asserted in sio_ioctl_param_type.  
06/18/01   dwp     Add support for FEATURE_UART_RATE1.
06/07/01   dwp     Modify sio_port_id_type such that when a minimal DMSS build
                   is made, only three entries will be enumerated.
06/01/01   dwp     Removed dependencies on DS.
03/26/01   dl      Add support for SMEM_DS port.
02/19/01   dwp     Reordered sio_bitrate_type to coincide w/new
                   nv_sio_baudrate_type.
02/13/01   dwp     Removed sizing of sio_bitrate_type.
01/26/01   ych     Merged T-53/JCDMA features for SK.
11/27/00   dwp     Change multiple SIO_PORT_BT_RFCOMM's to SIO_PORT_BT_SPP.
                   Add SIO_PORT_BT_NA. Remove SIO_IOCTL_BT_ENABLE and
                   SIO_IOCTL_BT_DISABLE.
11/18/00   dwp     Added additional requirement of FEATURE_USB on port 
                   definition SIO_PORT_USB_MMC.
11/14/00   dwp     Added support for FEATURE_MMC which is used in support of
                   FEATURE_RUNTIME_DEVMAP.
09/05/00   dwp     Additional FEATURE_BT modifications.
07/27/00   dwp     Added support for FEATURE_USB_DIAG and FEATURE_BT.
07/20/00   jwh     Added support for SIO_IOCTL_GET_FLOW_CTL and
                   FEATURE_SIO_INT_LOCKOUT.
06/08/00   jwh     Update comments.
05/22/00   jwh     Changed sio_bitrate_type and sio_port_id_type enum
                   featurization to allow compile time checking and keep
                   numeric values consistent for NV. Add sio_is_baud_valid().
04/15/00   jwh     Add SIO_IOCTL_CHANGE_BAUD_NOW, clarify operation of 
                   SIO_IOCTL_CHANGE_BAUD_RATE.
04/11/00   jwh     Make SIO_BITRATE_230400 defn unconditional.
02/16/00   jwh     Removed support for OLD SIO.  Added support for 230,400
                   baud for MSM5000.
09/09/99   jwh     Changes to NEW SIO API: sio_port_id_type name changes and
                   now #ifdef'd with FEATUREs; deleted sio_priority_type and
                   sio_bandwidth_type and the four associated (unused) 
                   members of sio_open_type; added SIO_IOCTL_FLUSH_TX.  Also
                   improved some comments.  Merged with MSM3100 1.0C.
08/05/99   mk      Added support for old SIO if T_MSM31 defined.
04/25/99   ak      Added ioctl def's to support mode change and baudrate 
                   change via an ioctl, versus using open/close.
04/09/99   jwh     Made sio_hdr_type typedef PACKED to avoid ARM 2.5 error.
02/17/99   ak      Added extern for sio_tx_flush.
01/17/99   ak      Featurized file (after INCLUDE FILES section), based on
                   whether user wants old or new SIO.  (FEATURE_NEW_SIO)
06/08/98   jjn     Exports new partitioned SIO.
10/29/97   jjn     Implemented DM Enter and DM Exit
10/10/97   grl     Allowed customerization for DM
09/09/97   ldg     (jjn) Changed and renamed flow control functions.
06/16/97   fkm     FEATURE_xxx Updates (via Automatic Scripts)
01/20/97   jjn     Implemented 617 transparency in SIO
05/24/96   jjw     Gemini & ISS2 Data Services release.
04/23/96   rdh     Added support for 115.2K DM.
11/09/95   jjw     Completed changes to support fully compliant IS-99
08/18/95   jmk     Took out T_SP conditional compile.
08/02/95   jmk     Added T_SP conditional compilation (exclude rex, protos, etc)
07/21/95   jjw     Fixed boo-boo within #ifdef DS definitions
07/13/95   jjw     Changes to support IS-99 protocol integration
03/23/95   rdh     Added ISS1 to sio_baud_clock_ctl prototype.
01/22/95   jjw     Added 2nd generation definitions and function prototypes
05/25/94   jjw     Made change to Ring buffer type parameter
05/18/94   jjw     Added changes for DM multi-drop
08/19/93   jjw     Modfied some existing function names, added all Data 
                   services functionality
12/07/93   jah     Added prototype for sio_baud_clock_ctl().
06/07/93   jjw     Made code review changes, added some more changes for Beta
                   II initial functionality
05/14/93   jjw     Added T_B2 compile-time directive for Beta II target
05/11/93   jjw     Cleaned up for code review, phase 1
03/12/93   jjw     Added typedef for Async. SIO channel A usage.
01/28/92   jah     Improved comments before passing this module to Jim Willkie.
08/25/92   jah     Improved error logging to differentiate errors more clearly.
07/23/92   jah     Added comments.
06/22/92   jah     Ported from brassboard, removed async.

===========================================================================*/

/*===========================================================================

                        INCLUDE FILES

===========================================================================*/

#include "comdef.h"     /* Definitions for byte, word, etc.                */
#include "queue.h"      /* Queue declaration package                       */
#include "rex.h"        /* REX multitasking package                        */
#include "target.h"     /* Target specific definitions                     */
#include "customer.h"   /* Customer Specific Definitions                   */
#include "dsm.h"        /* Data service buffer definition                  */

#ifndef FEATURE_DATA_MM
  #ifdef FEATURE_DS
    #include "ds.h"                /* Data Services Interface.             */
  #endif
#endif /* FEATURE_DATA_MM */
#ifdef FEATURE_BT_SPP
struct bt_spp_open_struct;
struct bt_spp_config_struct;
struct bt_spp_status_struct;
#endif

/* DSM payload type for Unescaped bytes for Async data */
#if defined(FEATURE_DATA_MM) || defined(FEATURE_GSM_PLT) || defined(FEATURE_WCDMA_PLT) || defined (FEATURE_CORE_SERVICE_ONLY)
#define SIO_PAYLOAD       DSM_PS_RAW_APP_PAYLOAD
#else 
#define SIO_PAYLOAD       PS_RAW_APP_PAYLOAD
#endif

#ifdef FEATURE_SMD_BRIDGE
#define SIO_PORT_IS_BRIDGE(port_id) \
         ((port_id >= SIO_PORT_SMD_BRIDGE_FIRST) && \
      (port_id <= SIO_PORT_SMD_BRIDGE_LAST))

#else
#define SIO_PORT_IS_BRIDGE(port_id) (0)
#endif


#define DEVICE_ID(major, minor) ((major << 16) | (minor & 0x00ff))

/*===========================================================================

                        FEATURE FLAG MAPPING

===========================================================================*/

/* For 7500, USB2 signal uses UART1 MODEM signals, remove modem GPIOs if
 * UART2 is in use                                                         */
#ifdef FEATURE_USB2_HOST_PORT
#define SIORS_UART1_USES_4WIRE_ONLY
#endif


#if defined(FEATURE_USE_UART1DM) && !defined(FEATURE_SIO_NO_DEBUG_TRACE)
#define FEATURE_SIO_DEBUG_TRACE         /* Turn on the debug feature here */
#endif

#if !defined(FEATURE_FIRST_UART) && !defined(FEATURE_SECOND_UART) && !defined(FEATURE_THIRD_UART)
#define SIO_NO_UART
#endif

/*===========================================================================

                        DATA DECLARATIONS

===========================================================================*/


/*---------------------------------------------------------------------------
 SIO flow control types.
---------------------------------------------------------------------------*/

typedef enum
{
  SIO_FCTL_BEST = 0,                   /* Best Flow control method.  Can
                                          mean default or no change.     */
  SIO_FCTL_OFF,                        /* Flow control disabled          */
  SIO_XONXOFF_STRIP_FCTL_FS,           /* Use fail-safe XON/XOFF flow 
                                          control but strip XON/XOFF 
                                          characters from stream         */
  SIO_XONXOFF_STRIP_FCTL_NFS,          /* Use non-failsafe XON/XOFF flow
                                          control but strip XON/XOFF 
                                          characters from stream         */
  SIO_CTSRFR_FCTL,                     /* Use CTS/RFR flow control       */
  SIO_XONXOFF_NSTRIP_FCTL_FS,          /* Use fail-safe XON/XOFF flow 
                                          control and leave in stream    */
  SIO_XONXOFF_NSTRIP_FCTL_NFS,         /* Use non-failsafe XON/XOFF flow 
                                          control and leave in stream    */                                        
  SIO_MAX_FLOW,                        /* For bounds checking only       */
  SIO_CTSRFR_AUTO_FCTL                 /* Use CTS/RFR flow control with
                                          auto RX RFR signal generation  */
} sio_flow_ctl_type;


/*---------------------------------------------------------------------------
 SIO Stream Identifier type.  
---------------------------------------------------------------------------*/

typedef int2 sio_stream_id_type;

#define SIO_NO_STREAM_ID -1


/*---------------------------------------------------------------------------
 Maximum number of streams allowed.  At this time, only 2 simultaneous 
 streams are required.
---------------------------------------------------------------------------*/
#define SIO_MAX_STREAM 40

typedef enum    
{
  SIO_DS_AUTODETECT_MODE,          /* AutoDetect Mode (Backwards comp.)    */
  SIO_DS_RAWDATA_MODE,             /* RawData Mode (Backwards compatible)  */
  SIO_DS_PKT_MODE,                 /* Packet mode                          */
  SIO_DM_MODE,                     /* DM mode                              */
  SIO_GENERIC_MODE,                /* Generic Mode.  Most streams should 
                                      use this mode.                       */
  SIO_MAX_MODE                     /* Maximum Mode Value.                  */
} sio_stream_mode_type;


/* Enum type to let SMD know whether the transfer has to be done via memcpy or DM */
typedef enum    
{
  SIO_SMD_MEMCPY_MODE,          
  SIO_SMD_DM_MODE,              
  SIO_SMD_INVALID_MODE
} sio_dataxfr_mode_type;

/* Enum type to let SMD know whether the transfer should be streaming 
  ( data in packets is appended ) or packet type */
typedef enum    
{
  SIO_SMD_STREAMING_TYPE,        
  SIO_SMD_PKTXFR_TYPE,            
  SIO_SMD_INVALID_TYPE
} sio_xfrflow_type;

/*---------------------------------------------------------------------------
 SIO BITRATE type.  The unit of measure is bits per second.
 The "ILLEGAL" entries are to retain compile time range checking 
 while keeping the actual values invariant for NVRAM consistency.
---------------------------------------------------------------------------*/
/* !!!! Must keep siors232.c:sio_is_baud_valid() in sync with this. !!!! */
typedef enum     
{
  SIO_BITRATE_ILLEGAL_1,
  SIO_BITRATE_ILLEGAL_3,
  SIO_BITRATE_ILLEGAL_4,
  SIO_BITRATE_ILLEGAL_5,
  SIO_BITRATE_ILLEGAL_6,
  SIO_BITRATE_ILLEGAL_7,
  SIO_BITRATE_300,
  SIO_BITRATE_600,
  SIO_BITRATE_1200,
  SIO_BITRATE_2400,
  SIO_BITRATE_4800,
  SIO_BITRATE_9600,
#if defined(FEATURE_JCDMA_DS) || defined(FEATURE_JCDMA_SIO)
  SIO_BITRATE_14400,
#else
  SIO_BITRATE_ILLEGAL_8,
#endif
  SIO_BITRATE_19200,
  SIO_BITRATE_38400,
  SIO_BITRATE_57600,
#if defined(FEATURE_JCDMA_DS) || defined(FEATURE_JCDMA_SIO)
  SIO_BITRATE_76800,
#else
  SIO_BITRATE_ILLEGAL_9,
#endif
  SIO_BITRATE_115200,
#ifdef FEATURE_UART_TCXO_CLK_FREQ
  SIO_BITRATE_230400,
#else
  SIO_BITRATE_ILLEGAL_2,
#endif
#ifdef FEATURE_UART_TCXO_CLK_FREQ 
  SIO_BITRATE_460800,
#else
  SIO_BITRATE_ILLEGAL_10,
#endif
  SIO_BITRATE_921600,
  SIO_BITRATE_2900000,
  SIO_BITRATE_3200000,
  SIO_BITRATE_3686400,
  SIO_BITRATE_4000000,
#ifdef MSMU_MVR_HS_CUSTOM
  SIO_BITRATE_HS_CUSTOM,
#else
  SIO_BITRATE_ILLEGAL_11,
#endif 

  SIO_BITRATE_BEST = 0x7FFE,  /* Best bitrate (default, fastest, etc) */
  SIO_BITRATE_MAX = 0x7FFF    /* For bounds checking only             */
} sio_bitrate_type;


/*---------------------------------------------------------------------------
 SIO Port Identifier type.  This allows SIO operation over multiple ports.
 Only define sio_port_id_type enum values for ports that actually exist 
 (hence the #ifdef's below).
 The "ILLEGAL" entry(ies) are to retain compile time range checking
 while keeping the actual values invariant for NVRAM consistency.
---------------------------------------------------------------------------*/
typedef enum    
{
  SIO_PORT_NULL = -1,              /* Non-existent Port                    */
  SIO_PORT_UART_MAIN = 0,          /* Main UART port (RS232)               */
  SIO_PORT_UART_AUX,               /* Second UART port (RS232)             */
  SIO_PORT_UART_THIRD,             /* Third UART port (RS232)              */
  SIO_PORT_USB_MDM,                /* USB Comm. Device Class ACM           */
  SIO_PORT_USB_CDC_ACM = SIO_PORT_USB_MDM,
  SIO_PORT_USB_SER1,               /* DIAG on USB                          */
  SIO_PORT_USB_DIAG = SIO_PORT_USB_SER1,
  SIO_PORT_USB_MMC,                /* MMC on USB                           */
  SIO_PORT_BT_SPP,                 /* BlueTooth Serial Port Profile support */
  SIO_PORT_BT_NA,                  /* BlueTooth Network Access support      */
  SIO_PORT_QMIP,                   /* Virtual QMIP device                  */
  SIO_PORT_SMEM_DS,                /* Shared Memory Data Service           */
  SIO_PORT_SMD_FIRST,              /* Shared Memory Driver                 */
  SIO_PORT_SMD_DS = SIO_PORT_SMD_FIRST,
  SIO_PORT_SMD_DIAG,
  SIO_PORT_SMD_DIAG_APPS,
  SIO_PORT_SMD_DIAG_MODEM,
  SIO_PORT_SMD_DIAG_QDSP,
  SIO_PORT_SMD_RPC_CALL,
  SIO_PORT_SMD_RPC_REPLY,
  SIO_PORT_SMD_BT,
  SIO_PORT_SMD_CONTROL,
  SIO_PORT_SMD_MEMCPY_SPARE1,
  SIO_PORT_SMD_DATA_FIRST,
  SIO_PORT_SMD_DATA1 = SIO_PORT_SMD_DATA_FIRST,
  SIO_PORT_SMD_WINMOB_MODEM_PORT1 = SIO_PORT_SMD_DATA1,
  SIO_PORT_SMD_DATA2,
  SIO_PORT_SMD_WINMOB_MODEM_PORT2 = SIO_PORT_SMD_DATA2,
  SIO_PORT_SMD_DATA3,
  SIO_PORT_SMD_WINMOB_MODEM_PORT3 = SIO_PORT_SMD_DATA3,
  SIO_PORT_SMD_DATA4,
  SIO_PORT_SMD_WINMOB_MODEM_PORT4 = SIO_PORT_SMD_DATA4,
  SIO_PORT_SMD_DATA5,
  SIO_PORT_SMD_WINMOB_QMI_WWAN = SIO_PORT_SMD_DATA5,
  SIO_PORT_SMD_RMNET1 = SIO_PORT_SMD_DATA5,
  SIO_PORT_SMD_DATA6,
  SIO_PORT_SMD_RMNET2 = SIO_PORT_SMD_DATA6,
  SIO_PORT_SMD_DATA7,
  SIO_PORT_SMD_RMNET3 = SIO_PORT_SMD_DATA7,
  SIO_PORT_SMD_DATA8,
  SIO_PORT_SMD_RMNET4 = SIO_PORT_SMD_DATA8,
  SIO_PORT_SMD_DATA9,
  SIO_PORT_SMD_RMNET5 = SIO_PORT_SMD_DATA9,
  SIO_PORT_SMD_DATA10,
  SIO_PORT_SMD_WINMOB_QVP_MVS = SIO_PORT_SMD_DATA10,
  SIO_PORT_SMD_DATA11,
  SIO_PORT_SMD_WINMOB_QVP_DATA = SIO_PORT_SMD_DATA11,
  SIO_PORT_SMD_DATA12,
  SIO_PORT_SMD_RMNET6 = SIO_PORT_SMD_DATA12,
  SIO_PORT_SMD_DATA13,
  SIO_PORT_SMD_RMNET7 = SIO_PORT_SMD_DATA13,
  SIO_PORT_SMD_DATA14,
  SIO_PORT_SMD_RMNET8 = SIO_PORT_SMD_DATA14,
  SIO_PORT_SMD_DATA15,
  SIO_PORT_SMD_DATA16,
  SIO_PORT_SMD_DATA17,
  SIO_PORT_SMD_DATA18,
  SIO_PORT_SMD_DATA19,
  SIO_PORT_SMD_DATA20,
  SIO_PORT_SMD_DATA21,
  SIO_PORT_SMD_DATA22,
  SIO_PORT_SMD_DATA23,
  SIO_PORT_SMD_DATA24,
  SIO_PORT_SMD_DATA25,
  SIO_PORT_SMD_DATA26,
  SIO_PORT_SMD_DATA27,
  SIO_PORT_SMD_DATA28,
  SIO_PORT_SMD_DATA29,
  SIO_PORT_SMD_DATA30,
  SIO_PORT_SMD_DATA31,
  SIO_PORT_SMD_DATA32,
  SIO_PORT_SMD_DATA33,
  SIO_PORT_SMD_DATA34,
  SIO_PORT_SMD_DATA35,
  SIO_PORT_SMD_DATA36,
  SIO_PORT_SMD_DATA37,
  SIO_PORT_SMD_DATA38,
  SIO_PORT_SMD_DATA39,
  SIO_PORT_SMD_DATA40,
  SIO_PORT_SMD_DATA_LAST = SIO_PORT_SMD_DATA40,
  SIO_PORT_SMD_GPS_NMEA,
  SIO_PORT_SMD_BRIDGE_FIRST,
  SIO_PORT_SMD_BRIDGE_1 = SIO_PORT_SMD_BRIDGE_FIRST,
  SIO_PORT_SMD_BRIDGE_2,
  SIO_PORT_SMD_BRIDGE_3,
  SIO_PORT_SMD_BRIDGE_4,
  SIO_PORT_SMD_BRIDGE_5,
  SIO_PORT_SMD_BRIDGE_LAST = SIO_PORT_SMD_BRIDGE_5,
  SIO_PORT_LSM_CSD,                /* Port for for DS - LSM to SMLC via CSD */
  SIO_PORT_USB_SER2,
  SIO_PORT_USB_NMEA = SIO_PORT_USB_SER2,
  SIO_PORT_USB_RRDM,
  SIO_PORT_USB_NET_WWAN,
  SIO_PORT_USB_RMNET2,
  SIO_PORT_USB_RMNET3,
  SIO_PORT_APS_SERIAL_VSP,
  SIO_PORT_USB_SER3,
  SIO_PORT_USB_BREW_EXTENDED_DATA,
  SIO_PORT_USB_BREW_EXTENDED_NOTIFICATION,
  SIO_PORT_MAX                     /* For bounds checking only             */
} sio_port_id_type;


/*---------------------------------------------------------------------------
 Resolve the maximum port number for a particular device group
---------------------------------------------------------------------------*/
#ifdef FEATURE_SMD_BRIDGE
  #define SIO_PORT_SMD_LAST   SIO_PORT_SMD_BRIDGE_LAST
#else
  #define SIO_PORT_SMD_LAST   SIO_PORT_SMD_GPS_NMEA
#endif

#ifdef FEATURE_THIRD_UART
  #define SIO_PORT_UART_MAX   SIO_PORT_UART_THIRD
#elif defined FEATURE_SECOND_UART
  #define SIO_PORT_UART_MAX   SIO_PORT_UART_AUX
#else
  #define SIO_PORT_UART_MAX   SIO_PORT_UART_MAIN
#endif

/*---------------------------------------------------------------------------
 The "ILLEGAL" entry(ies) are for compile time range checking, if a
 feature is not defined, the enum is replaced with an illegal port tag
---------------------------------------------------------------------------*/
#ifndef FEATURE_FIRST_UART_ON_ANY_PROC
  #define SIO_PORT_MAIN           SIO_PORT_ILLEGAL
#endif 

#ifndef FEATURE_SECOND_UART_ON_ANY_PROC
  #define SIO_PORT_UART_AUX       SIO_PORT_ILLEGAL
#endif 

#ifndef FEATURE_THIRD_UART_ON_ANY_PROC
  #define SIO_PORT_UART_THIRD     SIO_PORT_ILLEGAL 
#endif

#ifndef FEATURE_USB_CDC_ACM
  #define SIO_PORT_USB_MDM        SIO_PORT_ILLEGAL 
  #define SIO_PORT_USB_CDC_ACM    SIO_PORT_ILLEGAL 
#endif

#if !(defined(FEATURE_USB_DIAG) || defined(FEATURE_USB_DIAG_NMEA))
  #define SIO_PORT_USB_SER3       SIO_PORT_ILLEGAL 
  #define SIO_PORT_USB_SER1       SIO_PORT_ILLEGAL 
  #define SIO_PORT_USB_DIAG       SIO_PORT_ILLEGAL 
#endif

#if !(defined(FEATURE_USB_PERIPHERAL_MASS_STORAGE))
  #define SIO_PORT_USB_MMC        SIO_PORT_ILLEGAL 
#endif

#ifndef FEATURE_BT
  #define SIO_PORT_BT_SPP         SIO_PORT_ILLEGAL 
  #define SIO_PORT_BT_NA          SIO_PORT_ILLEGAL 
#endif

#ifndef FEATURE_QMIP
  #define SIO_PORT_QMIP           SIO_PORT_ILLEGAL 
#endif 

#ifndef FEATURE_SMEM_DS  
  #define SIO_PORT_SMEM_DS        SIO_PORT_ILLEGAL 
#endif       

#ifndef FEATURE_SMD //{    
  #define SIO_PORT_SMD_FIRST             SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DS                SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DIAG              SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DIAG_APPS         SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DIAG_MODEM        SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DIAG_QDSP         SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_RPC_CALL          SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_RPC_REPLY         SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_BT                SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_CONTROL           SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_MEMCPY_SPARE1     SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA_FIRST        SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA1             SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA2             SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA3             SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA4             SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA5             SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA6             SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA7             SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA8             SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA9             SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA10            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA11            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA12            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA13            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA14            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA15            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA16            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA17            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA18            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA19            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA20            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA21            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA22            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA23            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA24            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA25            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA26            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA27            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA28            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA29            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA30            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA31            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA32            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA33            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA34            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA35            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA36            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA37            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA38            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA39            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA40            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_DATA_LAST         SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_GPS_NMEA          SIO_PORT_ILLEGAL

  #define SIO_PORT_SMD_RMNET1            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_RMNET2            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_RMNET3            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_RMNET4            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_RMNET5            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_RMNET6            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_RMNET7            SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_RMNET8            SIO_PORT_ILLEGAL
#endif //}

#ifndef FEATURE_SMD_BRIDGE              
  #define SIO_PORT_SMD_BRIDGE_FIRST      SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_BRIDGE_1          SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_BRIDGE_2          SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_BRIDGE_3          SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_BRIDGE_4          SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_BRIDGE_5          SIO_PORT_ILLEGAL
  #define SIO_PORT_SMD_BRIDGE_LAST       SIO_PORT_ILLEGAL
#endif 

#ifndef FEATURE_MMGPS_CSD
  #define SIO_PORT_LSM_CSD               SIO_PORT_ILLEGAL
#endif

#ifndef FEATURE_USB_DIAG_NMEA
  #define SIO_PORT_USB_SER2              SIO_PORT_ILLEGAL
  #define SIO_PORT_USB_NMEA              SIO_PORT_ILLEGAL
#endif

#ifndef FEATURE_APS
  #define SIO_PORT_APS_SERIAL_VSP        SIO_PORT_ILLEGAL
#endif 

#ifndef FEATURE_USB_BREW_EXTENDED_DEVICE
  #define SIO_PORT_USB_BREW_EXTENDED_DATA              SIO_PORT_ILLEGAL
  #define SIO_PORT_USB_BREW_EXTENDED_NOTIFICATION      SIO_PORT_ILLEGAL
#endif


#ifdef FEATURE_SMD
/*------------------------------------------------------------
 * This symbol defined for backwards compatibility when the SMD
 * names were changed. It should be deleted when the references
 * are removed.
 *------------------------------------------------------------*/
#define SIO_PORT_SMEMDS_DS SIO_PORT_SMD_DS
#endif /* FEATURE_SMD */


/*---------------------------------------------------------------------------
 Identifier for stream which is opened.
---------------------------------------------------------------------------*/
typedef int2 stream_id_type;


/*---------------------------------------------------------------------------
 Type of stream which is being opened.
---------------------------------------------------------------------------*/

typedef enum
{
  SIO_STREAM_RX_ONLY = 0,                      /* Stream only used for RX  */
  SIO_STREAM_TX_ONLY,                          /* Stream only used for TX  */
  SIO_STREAM_RXTX,                             /* Stream used for RX & TX  */
  SIO_STREAM_MAX                               /* Used for bounds checking */
} sio_stream_type;




/*---------------------------------------------------------------------------
 Function pointer type for use by SIO RX ISR to pass to caller incoming data
 packaged in DSM item.
---------------------------------------------------------------------------*/

typedef void (*sio_rx_func_ptr_type)( dsm_item_type **);

/*---------------------------------------------------------------------------
 SIO Open Type.  A variable of this type needs to be passed to sio_open.
 sio_open will not modify any of the parameters (hence declaration of
 parameter to sio_open as "const").
---------------------------------------------------------------------------*/

typedef struct
{
  sio_stream_id_type    stream_id;          /* Stream ID Type.  Filled in
                                               by SIO for internal use.    */
  sio_stream_type       stream_type;        /* Type of stream being opened.
                                               Filled in by SIO for 
                                               internal use only.          */
  sio_stream_mode_type  stream_mode;        /* Stream Mode Type            */
  dsm_watermark_type   *rx_queue;           /* Received Data Queue         */
  dsm_watermark_type   *tx_queue;           /* Transmit Data Queue         */
  sio_bitrate_type      rx_bitrate;         /* Bit-rate for reception      */
  sio_bitrate_type      tx_bitrate;         /* Bit-rate for transmission   */
  sio_port_id_type      port_id;            /* Port which needs to be used */
  boolean               tail_char_used;     /* Is tail character used ?    */
  
  byte                  tail_char;          /* If tail character is used,
                                               this is the tail character. */

  sio_rx_func_ptr_type  rx_func_ptr;        /* If non-NULL, this function 
                                               will be called for each 
                                               packet which is received.   */
  sio_flow_ctl_type     tx_flow;            /* TX flow control method      */
  sio_flow_ctl_type     rx_flow;            /* RX flow control method      */

#ifdef FEATURE_BT_SPP
  struct bt_spp_open_struct*  bt_open_ptr;  /* Additional data to open a   */
                                            /*   Bluetooth port via SIO.   */
#endif
} sio_open_type;


/*---------------------------------------------------------------------------
 SIO Status type.  Indicates the different results of SIO operation.
---------------------------------------------------------------------------*/

typedef enum
{
  SIO_DONE_S=0,                    /* Done                                 */
  SIO_BADP_S,                      /* Bad parameter                        */
  SIO_UNAVAIL_S,                   /* No more streams can be allocated.    */
  SIO_BUSY_S,                      /* Command is still being processed.    */
  SIO_RX_NOT_OPEN_S,               /* RX was not opened for this stream    */
  SIO_TX_NOT_OPEN_S,               /* TX was not opened for this stream    */
  SIO_BITRATE_CHANGE_FAILED_S,     /* Attempt to change bitrate failed     */
  SIO_MAX_S                        /* For bounds checking only             */
} sio_status_type;


/*---------------------------------------------------------------------------
 SIO IOCTL command type.  These are the commands which can be carried out by
 sio_ioctl.
---------------------------------------------------------------------------*/

typedef enum
{
  SIO_IOCTL_CD_ASSERT=0,             /* Assert Carrier Detection Indicator   */
  SIO_IOCTL_CD_DEASSERT,             /* Deassert Carrier Detection Indicator */
  SIO_IOCTL_GET_CURRENT_CD,          /* Return a boolean (in paramter) which
                                        indicates whether or not the CD
                                        signal is asserted                   */
  SIO_IOCTL_RI_ASSERT,               /* Assert Ring Indication               */
  SIO_IOCTL_RI_DEASSERT,             /* Dassert Ring Indication              */
  SIO_IOCTL_GET_CURRENT_RI,          /* Return a boolean (in paramter) which
                                        indicates whether or not the RI
                                        signal is asserted                   */
  SIO_IOCTL_ENABLE_DTR_EVENT,        /* Register clock call-back which 
                                        informs caller of change in state of
                                        data terminal ready indication       */  
  SIO_IOCTL_DISABLE_DTR_EVENT,       /* De-register clock call-back for
                                        informing of change in state of 
                                        data terminal ready indication       */
  SIO_IOCTL_INBOUND_FLOW_ENABLE,     /* Enable inbound flow using current
                                        flow control method                  */
  SIO_IOCTL_INBOUND_FLOW_DISABLE,    /* Disable inbound fow using current
                                        flow control method                  */
  SIO_IOCTL_DSR_ASSERT,              /* Assert Data Set Ready Indication     */
  SIO_IOCTL_DSR_DEASSERT,            /* Deassert Data Set Ready Indication   */
  SIO_IOCTL_DTE_READY_ASSERTED,      /* Return a boolean (in paramter) which
                                        indicates whether or not DTE ready
                                        signal is asserted                   */
  SIO_IOCTL_GET_CURRENT_RTS,         /* Read the current status of RTS from
                                        the PC(DTE),i.e. the MSM(DCE) CTS pin*/
  SIO_IOCTL_SET_FLOW_CTL,            /* Allows setting of flow control 
                                        method                               */
  SIO_IOCTL_GET_FLOW_CTL,            /* Allows getting the flow control
                                          methods                            */
  SIO_IOCTL_ENABLE_AUTODETECT,       /* Enable AutoDetect.  Has no effect if
                                        AutoDetect already enabled.          */
  SIO_IOCTL_RECORD_ESC_FUNC_PTR,     /* Register clock call-back function
                                        which informs caller of arrival of
                                        escape code ( +++ )                  */
  SIO_IOCTL_RECORD_PKT_FUNC_PTR,     /* Register clock call-back function
                                        whcih informs caller of arrival of
                                        PPP packet                           */
  SIO_IOCTL_BEGIN_FAX_IMAGE,         /* Used by caller to signal beginning
                                        of fax image.  From the beginning of
                                        fax image to the end of fax image on
                                        given line, software flow control 
                                        needs to be ignored because they 
                                        may be unescaped data bytes          */
  SIO_IOCTL_CHANGE_BAUD_RATE,        /* Used to change the baud rate of the  
                                        serial port.  THIS IS FOR BACKWARD
                                        COMPATIBILITY WITH DATA SERVICES ONLY.
                                        The change takes place _only_  _after_
                                        a transition from a non-empty TX FIFO
                                        to an empty TX FIFO. If the TX FIFO
                                        is empty when this ioctl is received, 
                                        no baud change occurs until more data 
                                        is Tx'd.                             */
  SIO_IOCTL_CHANGE_BAUD_NOW,         /* Used to change the baud rate of the
                                        serial port.  Change takes place 
                                        immediately without regard to any data
                                        in the TX FIFO. Hence it is up to the 
                                        app to flush the tx beforehand. The 
                                        usage is otherwise identical to 
                                        SIO_IOCTL_CHANGE_BAUD_RATE.          */
  SIO_IOCTL_FLUSH_TX,                /* Register call-back function which
                                        informs caller when all pending
                                        output data has been transmitted
                                        WARNING: Unreliable results may occur
                                        if data is queued after this function
                                        call is made and before the callback
                                        function has been called.            */
  SIO_IOCTL_CHANGE_MODE,             /* Change the serial port's mode, i.e.,
                                        from AUTODETECT to RAWDATA for
                                        example                              */
  SIO_IOCTL_REG_TLP_CHANGE_CB,       /* Register a callback to be invoked when
                                        the version and/or use of the USB TLP
                                        changes.  Callback invoked immediately
                                        to provide initial state.            */
  SIO_IOCTL_GET_PACKET_SIZE,         /* Read the maximum packet size for
                                        packet devices */
  SIO_IOCTL_SET_PACKET_SIZE,         /* Change the maximum packet size for
                                        packet devices */
  SIO_IOCTL_GET_BUFFER_SIZE,         /* Read the buffer size for devices that
                                        have buffers */
  SIO_IOCTL_SET_BUFFER_SIZE,         /* Change the buffer size for devices
                                        that have variable buffers */
  SIO_IOCTL_GET_XFR_MODE,            /* Read the transfer mode for device */
  SIO_IOCTL_SET_XFR_MODE,            /* Set the transfer mode for device */
  SIO_IOCTL_GET_XFR_TYPE,            /* Read the transfer type for device */
  SIO_IOCTL_SET_XFR_TYPE,            /* Set the transfer type for device */
  SIO_IOCTL_ENABLE_DTR_EVENT_EXT,    /* Register callback which informs caller
                                        of change in state of data terminal
                                        ready indication - callback gets
                                        port_id and user data as args */  
  SIO_IOCTL_DISABLE_DTR_EVENT_EXT,   /* De-register call-back for informing of
                                        change in state of data terminal ready
                                        indication       */
  SIO_IOCTL_ENABLE_CD_EVENT_EXT,     /* Register callback which informs caller
                                        of change in state of carrier detect
                                        indication - callback gets port_id 
                                        and user data as args */  
  SIO_IOCTL_DISABLE_CD_EVENT_EXT,    /* De-register call-back for informing of
                                        change in state of carrier detect
                                        indication       */
  SIO_IOCTL_ENABLE_RI_EVENT_EXT,     /* Register callback for informing of
                                        change in state of ring indication  */
  SIO_IOCTL_DISABLE_RI_EVENT_EXT,    /* De-register callback for informing of
                                        change in state of ring indication  */
  SIO_IOCTL_SET_DROP_PEND,           /* Set the Drop Pending state  */
  SIO_IOCTL_GET_DROP_PEND,           /* Get the Drop Pending state  */
  SIO_IOCTL_BT_CONFIGURE,            /* Configure Bluetooth port.            */
  SIO_IOCTL_BT_DISCONNECT,           /* Disconnect Bluetooth port.           */
  SIO_IOCTL_BT_GET_STATUS,           /* Get Bluetooth status.                */
  SIO_IOCTL_POWERDOWN,               /* Power down the port                  */
  SIO_IOCTL_WAKEUP,                  /* Wake up the port                     */
  SIO_IOCTL_TX_START_BREAK,          /* Start a break event on the TX side   */
  SIO_IOCTL_TX_END_BREAK,            /* End a break event on the TX side     */
  SIO_IOCTL_RX_HIGH_EVENT,           /* Register a RX line high event CB     */
  SIO_IOCTL_CHECK_RX_STATE,          /* Get the current state of RX line     */ 
  SIO_IOCTL_WM_SET_WAIT,             /* Sets the wait flag in SMD            */
  SIO_IOCTL_WM_GET_WAIT              /* Gets the wait flag in SMD            */
} sio_ioctl_cmd_type;


/*---------------------------------------------------------------------------
 SIO IOCTL parameter types.  These are to be used in conjunction with the 
 above command type.
---------------------------------------------------------------------------*/

/* Default parameter to be used with ioctl command.  Should only be used as
   place-holder for operations which do not require parameters.            */
typedef word sio_ioctl_default_param_type;

/* Flow control type.  Need to specify TX flow and RX flow control methods.
   Because standard allows setting of these methods separately, SIO
   interface has to do so as well.                                         */  
typedef struct sio_ioctl_flow_ctl {
  sio_flow_ctl_type tx_flow;
  sio_flow_ctl_type rx_flow;
} sio_ioctl_flow_ctl_type;

/* Structure with current TLP settings to be passed to callback registered 
   via SIO_IOCTL_REG_TLP_CHANGE_CB ioctl */
typedef struct
{
  uint8    version;
  boolean  downlink_tlp_on;
  boolean  uplink_tlp_on;
} sio_ioctl_usb_tlp_settings_type;

/* Prototype for callback registered via SIO_IOCTL_REG_TLP_CHANGE_CB ioctl */
typedef void (* sio_usb_tlp_change_cb_f_type)
(
  sio_ioctl_usb_tlp_settings_type,
  void *
);

/* Structure to be passed as SIO_IOCTL_REG_TLP_CHANGE_CB ioctl parameter */
typedef struct
{
  sio_usb_tlp_change_cb_f_type  callback;    /* function to call when TLP
                                                settings change            */
  void *                        user_data;   /* will be passed to callback */
} sio_ioctl_usb_reg_tlp_change_cb_type;

/*---------------------------------------------------------------------------
 Function pointer type which points to a function that takes no arguments
 and returns nothing (i.e. void).
---------------------------------------------------------------------------*/

typedef void (*sio_vv_func_ptr_type)( void);

/*---------------------------------------------------------------------------
 Function pointer type which points to a function that takes a uint32 as 
 arguments and returns nothing (i.e void). This is meant to be
 used for callbacks.
---------------------------------------------------------------------------*/

typedef void (*sio_vpu_func_ptr_type)(void * cb_data);

typedef struct sio_ioctl_enable_dtr_event_ext{
  sio_vpu_func_ptr_type cb_func;
  void *                cb_data;
} sio_ioctl_enable_dtr_event_ext_type;

typedef struct sio_ioctl_enable_dtr_event_ext sio_ioctl_enable_cb_event_ext_type;
typedef struct sio_ioctl_enable_dtr_event_ext sio_ioctl_enable_ri_event_ext_type;

/*---------------------------------------------------------------------------
  Aggregate SIO IOCTL parameter type.
---------------------------------------------------------------------------*/

typedef union sio_ioctl_param {
  sio_ioctl_default_param_type   carrier_detect_assert;
  sio_ioctl_default_param_type   carrier_detect_deassert;
  sio_ioctl_default_param_type   ring_indicator_assert;
  sio_ioctl_default_param_type   ring_indicator_deassert;
  sio_vv_func_ptr_type           enable_dte_ready_event;
  sio_ioctl_default_param_type   disable_dte_ready_event;
  sio_ioctl_default_param_type   inbound_flow_enable;
  sio_ioctl_default_param_type   inbound_flow_disable;
  sio_ioctl_default_param_type   data_set_ready_assert;
  sio_ioctl_default_param_type   data_set_ready_deassert;
  boolean                       *dte_ready_asserted;
  boolean                       *rts_asserted;
  boolean                       *cd_asserted;
  boolean                       *ri_asserted;
  sio_ioctl_flow_ctl_type        flow_ctl;
  sio_ioctl_default_param_type   rearm_autobaud;
  sio_vv_func_ptr_type           record_escape_func_ptr;
  sio_vv_func_ptr_type           record_pkt_func_ptr;
  sio_ioctl_default_param_type   begin_fax_image;
  sio_bitrate_type               bitrate;
  sio_stream_mode_type           stream_mode;
  sio_open_type                 *open_ptr;
  sio_vv_func_ptr_type           record_flush_func_ptr;
  uint32                         packet_size;
  uint32                         buffer_size;
  boolean                        drop_asserted;
  sio_dataxfr_mode_type          dataxfr_mode;
  sio_xfrflow_type               xfrflow_type;
  sio_ioctl_enable_dtr_event_ext_type   enable_dte_ready_event_ext;
  sio_ioctl_enable_cb_event_ext_type    enable_cd_ready_event_ext;
  sio_ioctl_enable_ri_event_ext_type    enable_ri_ready_event_ext;
  sio_ioctl_usb_reg_tlp_change_cb_type  tlp_change_cb;
  struct bt_spp_config_struct*   bt_spp_config_ptr;
  struct bt_spp_status_struct*   bt_spp_status_ptr;
  sio_vv_func_ptr_type           rx_high_event_func_ptr;
  boolean                       *rx_line_state;
  boolean                        wm_wait;
} sio_ioctl_param_type;

#ifdef FEATURE_HTORPC_METACOMMENTS
  /*~ PARTIAL sio_ioctl_param */
  /*~ CASE SIO_IOCTL_CHANGE_BAUD_RATE sio_ioctl_param.rearm_autobaud */
  /*~ CASE SIO_IOCTL_CHANGE_BAUD_NOW sio_ioctl_param.rearm_autobaud */
  /*~ DEFAULT sio_ioctl_param.void */
#endif

/*---------------------------------------------------------------------------
 Device information package
---------------------------------------------------------------------------*/

typedef struct
{
    uint32      magic_num;          /* Magic number for sanity check */
    
    /* Data related to the device being used */
    uint32      device_id;          /* Device unique ID, Major/Minor */
    boolean     forward_port;       /* Non zero = forward this port  */
    sio_port_id_type forward_port_id;  /* Which port to use          */

    /* Pointer to the public functions provided by the device driver */
    sio_status_type (*HW_open) (sio_open_type *open_ptr);
    void (*HW_close) (sio_stream_id_type stream_id,
                        void (*close_func_ptr)(void));
    void (*HW_ioctl) (sio_stream_id_type stream_id,
                      sio_port_id_type      port_id,
                      sio_ioctl_cmd_type    cmd,
                      sio_ioctl_param_type *param);
    void (*HW_transmit) ( sio_stream_id_type stream_id,
                      sio_port_id_type   port_id,
                      dsm_item_type     *tx_ptr);
    void (*HW_flush_tx) (sio_stream_id_type stream_id,
                         sio_port_id_type   port_id,
                         void (*flush_func_ptr)(void));
    void (*HW_disable_device) (sio_port_id_type   port_id);
    void (*HW_control_transmit) (sio_stream_id_type stream_id,
                                 dsm_item_type     *tx_ptr);
    void (*HW_control_close) (sio_stream_id_type stream_id,
                              void (*close_func_ptr)(void));
    sio_status_type (*HW_control_open) (sio_open_type *open_ptr);

} sio_device_info_type;


/*---------------------------------------------------------------------------
 This is the function pointer type which will be passed to clk_reg.
---------------------------------------------------------------------------*/

typedef void (*sio_clk_routine_ptr_type)( int4 ms_interval);

/*-   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -   -*/

/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

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
uint32 sio_register_driver(sio_device_info_type *pHead);


/*===========================================================================

FUNCTION sio_device_id_lookup                        EXTERNALIZED FUNCTION

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
sio_port_id_type  sio_device_id_lookup(uint32 device_id);



/*===========================================================================

FUNCTION SIO_INIT                                    EXTERNALIZED FUNCTION

DESCRIPTION
  Initializes SIO.  
  
DEPENDENCIES
  None
    
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/

extern void sio_init
(
  void
);


/*===========================================================================

FUNCTION SIO_OPEN                           EXTERNALIZED FUNCTION

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

extern sio_stream_id_type sio_open
( 
  sio_open_type *open_ptr                      /* Configurations of stream */
);


/*===========================================================================

FUNCTION SIO_CLOSE                           EXTERNALIZED FUNCTION

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

extern void sio_close
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  void             (*close_func_ptr)(void)    /* Function to call when 
                                                 transmission is complete. */
);


/*===========================================================================

FUNCTION SIO_CLOSE_EXT                           EXTERNALIZED FUNCTION

DESCRIPTION
  This procedure does all the book-keeping necessary to close a stream.  
  Optional tcb_ptr and sigs can be passed to this function, if task wants
  to be signalled when the last bit of pending transmission leaves phone.
                  
DEPENDENCIES
  This function is only supported in shared memory ports 
      
RETURN VALUE
  None
  
SIDE EFFECTS
  close_func_ptr will be called when all pending data has left transmitter.
  
===========================================================================*/

extern void sio_close_ext
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  /* Function to call when transmission is complete.                       */
  void (*close_func_ptr)(void * cb_data),
  void *ext_cb_data
);

/*===========================================================================

FUNCTION SIO_TRANSMIT                           EXTERNALIZED FUNCTION

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

extern void sio_transmit
( 
  sio_stream_id_type stream_id,    /* Stream ID                            */
  dsm_item_type *tx_ptr            /* Packet which needs to be transmitted */
);


/*===========================================================================

FUNCTION SIO_FLUSH_TX                           EXTERNALIZED FUNCTION

DESCRIPTION
  This function will transmit any queued data and then call the registered
  callback.

DEPENDENCIES
  None
  
RETURN VALUE
  None
  
SIDE EFFECTS
  None
  
===========================================================================*/

extern void sio_flush_tx
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  void                   (*close_func_ptr)(void) /* Function to call when 
                                                 transmission is complete. */    
);

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

extern void sio_flush_tx_ext
( 
  sio_stream_id_type stream_id,               /* Stream ID                 */
  /* Function to call when transmission is complete.                       */
  void (*flush_func_ptr)(void * cb_data),
  void *ext_cb_data
);


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
  sio_stream_id_type    stream_id, /* Stream ID                            */ 
  sio_ioctl_cmd_type    cmd,       /* IOCTL command                        */
  sio_ioctl_param_type *param      /* Parameter to be used for command     */
);


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
  sio_stream_id_type    stream_id, /* Stream ID                            */ 
  sio_ioctl_cmd_type    cmd,       /* IOCTL command                        */
  sio_ioctl_param_type *param      /* Parameter to be used for command     */
);


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

sio_stream_id_type sio_get_stream_id( void);

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

void sio_put_stream_id( stream_id_type stream_id);

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

boolean sio_stream_open( stream_id_type stream_id);

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

void sio_initialize_stream_opened( void);

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

extern void sio_disable_device
(
  sio_port_id_type   port_id              /* SIO Port ID                   */  
);

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

extern boolean sio_is_baud_valid
(
  sio_bitrate_type   check_baud              /* SIO baud rate              */
);


#ifdef FEATURE_QMIP
#error code not present
#endif /* FEATURE_QMIP */

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
);

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
);


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
);

#endif /* SIO_H */

