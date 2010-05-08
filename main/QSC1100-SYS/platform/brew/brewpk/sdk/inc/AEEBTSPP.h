#ifndef AEEBTSPP_H
#define AEEBTSPP_H
/*=============================================================================
FILE:         AEEBTSPP.h

SERVICES:     IBTSPP Interface

DESCRIPTION:  IBTSPP is the BREW interface that supports Bluetooth Serial
              Port Profile.Any interface that support either client or Server 
           functionalities should inherit from this interface

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEEBTCommon.h"
#include "AEEPort.h"

/*=============================================================================
 Brew generated Interface ID
=============================================================================*/
#define AEEIID_BTSPP                0x0104340b 

/*=============================================================================
 Stream ID
=============================================================================*/
#define AEEBTSPP_STREAM_ID_NULL     -1

/*=============================================================================
 SPP States
=============================================================================*/
#define AEEBTSPP_ST_CLOSED          0
#define AEEBTSPP_ST_OPENING         1
#define AEEBTSPP_ST_OPEN_ERROR      2
#define AEEBTSPP_ST_OPENED          3 // valid only for servers 
#define AEEBTSPP_ST_CONNECTED       4
#define AEEBTSPP_ST_DISCONNECTING   5
#define AEEBTSPP_ST_DISCONNECTED    6 // valid only for clients

// Define a data type to refer SPP states
typedef uint8   AEEBTSPPState;

/*=============================================================================
 Baud Rate field definitions
=============================================================================*/
#define AEEBTSPP_CFG_BR_2400_BPS_V   0x00
#define AEEBTSPP_CFG_BR_4800_BPS_V   0x01
#define AEEBTSPP_CFG_BR_7200_BPS_V   0x02
#define AEEBTSPP_CFG_BR_9600_BPS_V   0x03
#define AEEBTSPP_CFG_BR_19200_BPS_V  0x04
#define AEEBTSPP_CFG_BR_38400_BPS_V  0x05
#define AEEBTSPP_CFG_BR_57600_BPS_V  0x06
#define AEEBTSPP_CFG_BR_115200_BPS_V 0x07
#define AEEBTSPP_CFG_BR_230400_BPS_V 0x08

#define AEEBTSPP_CFG_BR_DEFAULT_V    AEEBTSPP_CFG_BR_9600_BPS_V
#define AEEBTSPP_CFG_BR_MIN_V        AEEBTSPP_CFG_BR_2400_BPS_V
#define AEEBTSPP_CFG_BR_MAX_V        AEEBTSPP_CFG_BR_230400_BPS_V

// Define a data type to refer SPP baud rate
typedef uint8     BTSPPBaudRate;

/*=============================================================================
 Serial data format definitions (value3 octet)
=============================================================================*/
#define AEEBTSPP_CFG_FMT_D_M        0x03  /*  Data bits (default = 8).  */
#define AEEBTSPP_CFG_FMT_D_5_V      0x00
#define AEEBTSPP_CFG_FMT_D_6_V      0x02
#define AEEBTSPP_CFG_FMT_D_7_V      0x01
#define AEEBTSPP_CFG_FMT_D_8_V      0x03

#define AEEBTSPP_CFG_FMT_S_1P5_B    0x04  /*  Stop Bits:                */
/*    0 = 1 (default)         */
/*    1 = 1.5                 */

#define AEEBTSPP_CFG_FMT_P_B        0x08  /*  Parity:                   */
/*    0 = no (default),       */
/*    1 = yes                 */

#define AEEBTSPP_CFG_FMT_PT_M       0x30  /*  Parity Type.              */
#define AEEBTSPP_CFG_FMT_PT_ODD_V   0x00
#define AEEBTSPP_CFG_FMT_PT_EVEN_V  0x20
#define AEEBTSPP_CFG_FMT_PT_MARK_V  0x10
#define AEEBTSPP_CFG_FMT_PT_SPACE_V 0x30

/*  Default format is 8 data bits, 1 stop bit, no parity.  */
#define AEEBTSPP_CFG_FMT_DEFAULT_V  AEEBTSPP_CFG_FMT_D_8_V

// Define a data type to refer SPP data format
typedef uint8     BTSPPDataFormat;

/*=============================================================================
 Flow Control definitions
=============================================================================*/
#define AEEBTSPP_CFG_FC_M           0x3F
#define AEEBTSPP_CFG_FC_XO_IN_B     0x01  /*  XON/XOFF on input.   */
#define AEEBTSPP_CFG_FC_XO_OUT_B    0x02  /*  XON/XOFF on output.  */
#define AEEBTSPP_CFG_FC_RTR_IN_B    0x04  /*  RTR on input.        */
#define AEEBTSPP_CFG_FC_RTR_OUT_B   0x08  /*  RTR on output.       */
#define AEEBTSPP_CFG_FC_RTC_IN_B    0x10  /*  RTC on input.        */
#define AEEBTSPP_CFG_FC_RTC_OUT_B   0x20  /*  RTC on output.       */

#define AEEBTSPP_CFG_FC_DEFAULT_V   0x00  /*  None.  */
#define AEEBTSPP_CFG_XON_DEFAULT_V  0x11  /*  DC1    */
#define AEEBTSPP_CFG_XOFF_DEFAULT_V 0x13  /*  DC3    */

// Define a data type to refer SPP flow control
typedef uint8     BTSPPFlowCtrl;

/*============================================================================
 IO Control Commands
============================================================================*/
#define AEEBTSPP_IOCTL_INBOUND_FLOW_ENABLE    1
#define AEEBTSPP_IOCTL_INBOUND_FLOW_DISABLE   2
#define AEEBTSPP_IOCTL_FLUSH_TX               3
#define AEEBTSPP_IOCTL_CD_ASSERT              4
#define AEEBTSPP_IOCTL_CD_DEASSERT            5
#define AEEBTSPP_IOCTL_RI_ASSERT              6
#define AEEBTSPP_IOCTL_RI_DEASSERT            7
#define AEEBTSPP_IOCTL_DSR_ASSERT             8
#define AEEBTSPP_IOCTL_DSR_DEASSERT           9
#define AEEBTSPP_IOCTL_ENABLE_DTR_EVENT      10
#define AEEBTSPP_IOCTL_DISABLE_DTR_EVENT     11
#define AEEBTSPP_IOCTL_GET_DTR               12
#define AEEBTSPP_IOCTL_GET_RTS               13
#define AEEBTSPP_IOCTL_SET_DTE               14
#define AEEBTSPP_IOCTL_SET_DCE               15


// Define a data type to refer SPP IO Control
typedef uint32    BTSPPIOCtlCommand;

/*============================================================================
 Modem Status Bitmap
============================================================================*/
#define AEEBTSPP_MS_DSR_ASSERTED_B          0x01
#define AEEBTSPP_MS_CTS_ASSERTED_B          0x02 
#define AEEBTSPP_MS_RI_ASSERTED_B           0x04
#define AEEBTSPP_MS_CD_ASSERTED_B           0x08
#define AEEBTSPP_MS_RTS_ASSERTED_B          0x10
#define AEEBTSPP_MS_DTR_ASSERTED_B          0x20
#define AEEBTSPP_MS_TX_FLUSHED_B            0x40

// Define a data type to refer SPP Modem Status
typedef uint8    BTSPPModemStatusBitmap;

/*============================================================================
 Line Error Bitmap
============================================================================*/
#define AEEBTSPP_LE_NONE                    0x00
#define AEEBTSPP_LE_OVERRUN_ERROR           0x01
#define AEEBTSPP_LE_PARITY_ERROR            0x02
#define AEEBTSPP_LE_FRAMING_ERROR           0x04

// Define a data type to refer SPP Line Error
typedef uint32  BTSPPLineErrorBitmap;

/*============================================================================
 SPP Config
============================================================================*/
typedef struct
{
   BTSPPBaudRate    baudRate;
   BTSPPDataFormat  format;
   BTSPPFlowCtrl    flowCtrl;
   uint8            uXonChar;
   uint8            uXoffChar;

}  BTSPPConfig;

/*============================================================================
 SPP Status
============================================================================*/
typedef struct
{
   BDAddress       BDAddr;           // BD address of remote device
   int16           streamID;         // the stream ID
   AEEBTSPPState   state;            // closed, opened, connected... 
   BTResult        result;           // reason stream gets to current state
   uint32          uChannel;         // server channel 

}  BTSPPStatus;

/*============================================================================
 SPP Modem Status
============================================================================*/
typedef struct
{
   int16                  streamID;    // the stream ID
   BTSPPModemStatusBitmap status;      // current status
   BTSPPModemStatusBitmap changedMask; // which of the bits have changed
   boolean                breakPresent;
   uint8                  breakLength;
   uint8                  breaks;

}  BTSPPModemStatus;

/*============================================================================
 Line Error Status
============================================================================*/
typedef struct
{
   int16                  streamID;         // the stream ID
   BTSPPLineErrorBitmap   lineError;
   uint32                 uOverRuns;
   uint32                 uParityErrors;
   uint32                 uFramingErrors;

}  BTSPPLineError;

/*=============================================================================
 SPP Device Information
=============================================================================*/
typedef struct
{
   BTServiceClassUUID     uuid; // either SPP or more specific UUID
   uint32                 uServerChannel;
   uint8                  szServiceName[ AEEBT_MAX_LEN_SERVICE_NAME + 1 ];

} BTSPPServiceRecord;

typedef struct
{
   BTDeviceInfo           bdi;
   uint32                 uNumSPPServices;
   BTSPPServiceRecord     serviceRecords[1];

} BTSPPDeviceInfo;


typedef struct IBTSPP IBTSPP;

/*============================================================================
 SPP Interface Definition
============================================================================*/
// Use to inherit from an IBTSPP
#define INHERIT_IBTSPP(iname)                                      \
   INHERIT_IPort(iname);                                           \
   int (*OnStatusChange)(iname* po, AEECallback* pCb);             \
   int (*OnModemStatusChange)(iname* po, AEECallback* pCb);        \
   int (*OnLineError)(iname* po, AEECallback* pCb);                \
   int (*Disconnect)(iname* po, BTResult* pR, AEECallback* pRCb);  \
   int (*GetStatus)(iname* po, BTSPPStatus* pS);                   \
   int (*GetModemStatus)(iname* po, BTSPPModemStatus* pS);         \
   int (*GetLineError)(iname* po, BTSPPLineError* pLE);            \
   int (*Configure)(iname* po, const BTSPPConfig* pCfg,            \
                    BTResult* pR, AEECallback* pRCb);              \
   int (*IOCtlEx)(iname* po, BTSPPIOCtlCommand IOctlCmd,           \
                  BTSPPModemStatus* const pModemStatus,            \
                  AEECallback* pResultCb)


AEEINTERFACE(IBTSPP)
{
   INHERIT_IBTSPP(IBTSPP);        
};



// Standard IPort Methods

static __inline uint32 IBTSPP_AddRef (IBTSPP* p)
{
   return AEEGETPVTBL((p),IBTSPP)->AddRef((p));
}

static __inline uint32 IBTSPP_Release (IBTSPP* p)
{
   return AEEGETPVTBL((p),IBTSPP)->Release((p));
}

static __inline int IBTSPP_QueryInterface (IBTSPP* p,
                                           AEECLSID      clsid,
                                           void**        ppo )
{
   return AEEGETPVTBL((p),IBTSPP)->QueryInterface((p),(clsid),(ppo));
}

static __inline int32 IBTSPP_Read(IBTSPP* p, 
                                  char* pcBuf,
                                  int32 cbBuf )
{
   return AEEGETPVTBL((p),IBTSPP)->Read((p),(pcBuf),(cbBuf));
}

static __inline void IBTSPP_Readable(IBTSPP* p,
                                     AEECallback* pcb )
{
   AEEGETPVTBL((p),IBTSPP)->Readable((p),(pcb));
}

static __inline int IBTSPP_GetLastError(IBTSPP* p)
{
   return AEEGETPVTBL((p),IBTSPP)->GetLastError((p));
}

static __inline int32 IBTSPP_Write(IBTSPP* p, 
                                   const char* pcBuf,
                                   int32 cbBuf )
{
   return AEEGETPVTBL((p),IBTSPP)->Write((p),(pcBuf),(cbBuf));
}

static __inline void IBTSPP_Writeable(IBTSPP* p,
                                     AEECallback* pcb )
{
   AEEGETPVTBL((p),IBTSPP)->Writeable((p),(pcb));
}

static __inline int IBTSPP_IOCtl (IBTSPP* p,
                                  int option,
                                  int val )
{
   return AEEGETPVTBL((p),IBTSPP)->IOCtl((p),(option),(val));
}

static __inline int IBTSPP_Close(IBTSPP* p)
{
   return AEEGETPVTBL((p),IBTSPP)->Close((p));
}

static __inline int IBTSPP_Open (IBTSPP* p,
                                 const char* szPort)
{
   return AEEGETPVTBL((p),IBTSPP)->Open((p),(szPort));
}

// Standard SPP Methods
static __inline int IBTSPP_OnStatusChange (IBTSPP* p, 
                                           AEECallback* pRCb)
{
   return AEEGETPVTBL((p),IBTSPP)->OnStatusChange((p),(pRCb));
}

static __inline int IBTSPP_OnModemStatusChange (IBTSPP* p, 
                                                AEECallback* pRCb)
{
   return AEEGETPVTBL((p),IBTSPP)->OnModemStatusChange((p),(pRCb));
}

static __inline int IBTSPP_OnLineError (IBTSPP* p, 
                                        AEECallback* pRCb)
{
   return AEEGETPVTBL((p),IBTSPP)->OnLineError((p),(pRCb));
}

static __inline int IBTSPP_Disconnect (IBTSPP* p,
                                       BTResult* pR, 
                                       AEECallback* pRCb)
{

   return AEEGETPVTBL((p),IBTSPP)->Disconnect((p),(pR),(pRCb));
}

static __inline int IBTSPP_GetStatus (IBTSPP* p,
                                      BTSPPStatus* pS)
{

   return AEEGETPVTBL((p),IBTSPP)->GetStatus((p),(pS));
}

static __inline int IBTSPP_GetModemStatus (IBTSPP* p,
                                           BTSPPModemStatus* pS)
{

   return AEEGETPVTBL((p),IBTSPP)->GetModemStatus((p),(pS));
}

static __inline int IBTSPP_GetLineError (IBTSPP* p,
                                         BTSPPLineError* pLineError)

{
   return AEEGETPVTBL((p),IBTSPP)->GetLineError((p),(pLineError));
}

static __inline int IBTSPP_Configure ( IBTSPP* p, 
                                       const BTSPPConfig* pCfg,
                                       BTResult* pR,
                                       AEECallback* pRCb)
{
   return AEEGETPVTBL((p),IBTSPP)->Configure((p),(pCfg),(pR),(pRCb));
}

static __inline int IBTSPP_IOCtlEx  ( IBTSPP* p, 
                                      BTSPPIOCtlCommand IOctlCmd,
                                      BTSPPModemStatus* pModemStatus, 
                                      AEECallback* pResultCb)
{
   return AEEGETPVTBL((p),IBTSPP)->IOCtlEx((p),(IOctlCmd),(pModemStatus),
                                                 (pResultCb));
}


/*============================================================================
DATA STRUCTURE DOCUMENTATION
==============================================================================
BTSPPBaudRate

Description:
   The following baudrates are the possible values for BTSPPBaudRate

   AEEBTSPP_CFG_BR_2400_BPS_V           : 2400 bits per second     
   AEEBTSPP_CFG_BR_4800_BPS_V           : 4800 bits per second      
   AEEBTSPP_CFG_BR_7200_BPS_V           : 7200 bits per second      
   AEEBTSPP_CFG_BR_9600_BPS_V           : 9600 bits per second 
   AEEBTSPP_CFG_BR_19200_BPS_V          : 19200 bits per second
   AEEBTSPP_CFG_BR_38400_BPS_V          : 38400 bits per second
   AEEBTSPP_CFG_BR_57600_BPS_V          : 57600 bits per second
   AEEBTSPP_CFG_BR_115200_BPS_V         : 115200 bits per second 
   AEEBTSPP_CFG_BR_230400_BPS_V         : 230400 bits per second 

   AEEBTSPP_CFG_BR_DEFAULT_V            : Default baudrate. Mapped to
                                          AEEBTSPP_CFG_BR_9600_BPS_V

   AEEBTSPP_CFG_BR_MIN_V                : Minimum baudrate. Mapped to
                                          AEEBTSPP_CFG_BR_2400_BPS_V
                                          
   AEEBTSPP_CFG_BR_MAX_V                : Maximum baudrate. Mapped to 
                                          AEEBTSPP_CFG_BR_230400_BPS_V

==============================================================================
BTSPPDataFormat

Description:
   SPP data format. Please refer to RS232 specs for
   more information.

                                                
   AEEBTSPP_CFG_FMT_D_M                 : 0x03
   AEEBTSPP_CFG_FMT_D_5_V               : 0x00   
   AEEBTSPP_CFG_FMT_D_6_V               : 0x02
   AEEBTSPP_CFG_FMT_D_7_V               : 0x01
   AEEBTSPP_CFG_FMT_D_8_V               : 0x03
   AEEBTSPP_CFG_FMT_S_1P5_B             : 0x04
   AEEBTSPP_CFG_FMT_P_B                 : 0x08
   AEEBTSPP_CFG_FMT_PT_M                : 0x30 
   AEEBTSPP_CFG_FMT_PT_ODD_V            : 0x00
   AEEBTSPP_CFG_FMT_PT_EVEN_V           : 0x20
   AEEBTSPP_CFG_FMT_PT_MARK_V           : 0x10
   AEEBTSPP_CFG_FMT_PT_SPACE_V          : 0x30
   AEEBTSPP_CFG_FMT_DEFAULT_V           : AEEBTSPP_CFG_FMT_D_8_V
                                          
   
==============================================================================
BTSPPFlowCtrl

Description:
   Data type to refer SPP flow control

   AEEBTSPP_CFG_FC_M                    
   AEEBTSPP_CFG_FC_XO_IN_B              : XON/XOFF on input.   
   AEEBTSPP_CFG_FC_XO_OUT_B             : XON/XOFF on output. 
   AEEBTSPP_CFG_FC_RTR_IN_B             : RTR on input.        
   AEEBTSPP_CFG_FC_RTR_OUT_B            : RTR on output.      
   AEEBTSPP_CFG_FC_RTC_IN_B             : RTC on input.       
   AEEBTSPP_CFG_FC_RTC_OUT_B            : RTC on output.     
   AEEBTSPP_CFG_FC_DEFAULT_V            : None. 
   AEEBTSPP_CFG_XON_DEFAULT_V  0x11     : DC1   
   AEEBTSPP_CFG_XOFF_DEFAULT_V 0x13     : DC3    
                                     
   
==============================================================================
BTSPPIOCtlCommand

Description:
   The following commands control the current SPP connection.

   AEEBTSPP_IOCTL_INBOUND_FLOW_ENABLE   : Enable inbound data flow
   AEEBTSPP_IOCTL_INBOUND_FLOW_DISABLE  : Disable inbound data flow
   AEEBTSPP_IOCTL_FLUSH_TX              : Register to be notified when tx buffer
                                          goes empty.This is a one time notification
                                          indicated as change in modem status bitmap.
   AEEBTSPP_IOCTL_CD_ASSERT             : Assert Carrier Detection Indicator
   AEEBTSPP_IOCTL_CD_DEASSERT           : Deassert Carrier Detection Indicator
   AEEBTSPP_IOCTL_RI_ASSERT             : Assert Ring Indication
   AEEBTSPP_IOCTL_RI_DEASSERT           : Deassert Ring Indication
   AEEBTSPP_IOCTL_DSR_ASSERT            : Assert DSR (DTR) if we are DCE (DTE)
   AEEBTSPP_IOCTL_DSR_DEASSERT          : Deassert DSR (DTR) if we are DCE (DTE)
   AEEBTSPP_IOCTL_ENABLE_DTR_EVENT      : Register to receive DTR_CHANGED evt
   AEEBTSPP_IOCTL_DISABLE_DTR_EVENT     : Cancel DTR_CHANGED evt registration
   AEEBTSPP_IOCTL_GET_DTR               : Retrieve current DTR state (TRUE=asserted)
   AEEBTSPP_IOCTL_GET_RTS               : Retrieve current RTS state (TRUE=asserted)
   AEEBTSPP_IOCTL_SET_DTE               : Interpret modem status as DTE
   AEEBTSPP_IOCTL_SET_DCE               : Interpret modem status as DCE 

==============================================================================
AEEBTSPPState

Description:
   The following indicate the state of the current SPP connection.

   AEEBTSPP_ST_CLOSED                    : Closed
   AEEBTSPP_ST_OPENING                   : Opening
   AEEBTSPP_ST_OPEN_ERROR                : Open error
   AEEBTSPP_ST_OPENED                    : Opened
   AEEBTSPP_ST_CONNECTED                 : Connected
   AEEBTSPP_ST_DISCONNECTING             : Disconnecting
   AEEBTSPP_ST_DISCONNECTED              : Disconnected
==============================================================================

BTSPPModemStatusBitmap

Description:
   Each bit represent a modem status line.  Please refer to RS232 specs for
   more information. AEEBTSPP_MS_TX_FLUSHED_B == TRUE indicates that the 
   transmit buffer has gone empty.The app should send AEEBTSPP_IOCTL_FLUSH_TX
   to be notified of this event.
   By default, the lines are interpreted as a DCE.

Definition:
   AEEBTSPP_MS_DSR_ASSERTED_B          0x01
   AEEBTSPP_MS_CTS_ASSERTED_B          0x02
   AEEBTSPP_MS_RI_ASSERTED_B           0x04
   AEEBTSPP_MS_CD_ASSERTED_B           0x08
   AEEBTSPP_MS_RTS_ASSERTED_B          0x10
   AEEBTSPP_MS_DTR_ASSERTED_B          0x20
   AEEBTSPP_MS_TX_FLUSHED_B            0x40

==============================================================================
BTSPPLineErrorBitmap

Description:
   Each bit represent a SPP line error status.  Please refer to RS232 specs for
   more information.

Definition:
   AEEBTSPP_LE_NONE                    0x00
   AEEBTSPP_LE_OVERRUN_ERROR           0x01
   AEEBTSPP_LE_PARITY_ERROR            0x02
   AEEBTSPP_LE_FRAMING_ERROR           0x04

==============================================================================

BTSPPStatus

Description:
   Specifies the current status of the SPP stream.
   BDAddress is only valid if the current state is CONNECTED.
   If the state is OPEN_ERROR, the result field indicates the reason for it.
   
typedef struct
{
   int16           streamID;         
   BDAddress       BDAddr;           
   AEEBTSPPState   state;            
   BTResult        result;           
   uint32          uChannel;
   
}  BTSPPStatus;


Members:
   streamID       : The stream ID
   BDAddr         : BD address 
   state          : Current SPP connection state(closed, opened, connected...)
   result         : Reason we get to current state
   uChannel       : Server channel number.

See Also:
   BDAddress
   AEEBTSPPState
   BTResult
==============================================================================
BTSPPConfig

Description:
   Specifies configuration parameters for the SPP stream.  The fields in 
   AEEBTSPPConfig do not control any aspect of the SPP stream.  They are 
   settings transferred across the BT connection but do not affect 
   the connection.
   
Definition:
typedef struct
{
   BTSPPBaudRate    baudRate;
   BTSPPDataFormat  format;
   BTSPPFlowCtrl    flowCtrl;
   uint8            uXonChar;
   uint8            uXoffChar;

}  BTSPPConfig;

Members:
   baudRate      : Baudrate
   format        : SPP data format
   flowCtrl      : SPP flow control            
   uXonChar      : Xon char
   uXoffChar     : Xoff char
   
Comments:
   Refer to RS232 specs for definitions of the fields within this structure.

See Also:
   BTSPPBaudRate
   BTSPPDataFormat
   BTSPPFlowCtrl
==============================================================================
BTSPPModemStatus

Description:
   This structure contains the current modem status.  The changedMask indicate
   which of the bits in status have changed since the previous modem status
   update.By default the bit map would indicate status of input lines as
   read by a DCE. To interpret the lines as a DTE, use AEEBTSPP_IOCTL_SET_DTE.

Definition:
typedef struct
{
   BTSPPModemStatusBitmap status;       
   BTSPPModemStatusBitmap changedMask;  
   boolean                breakPresent;
   uint8                  breakLength;
   uint8                  breaks;

}  BTSPPModemStatus;


Members:
   status         : Current status
   changedMask    : Which of the bits have changed
   breakPresent   : TRUE if Break present else FALSE
   breakLength    : Break length
   breaks         : Breaks

See Also:
   BTSPPModemStatusBitmap 
   
==============================================================================
BTSPPServiceRecord

Description:
   This structure contains the information identifying an SPP service record.
   
Definition:
typedef struct
{
   BTServiceClassUUID     uuid;
   uint32                 uServerChannel;
   uint8                  szServiceName[ AEEBT_MAX_LEN_SERVICE_NAME + 1];
} BTSPPServiceRecord;

Members:
   uuid           : if generic SPP UUID, the record has no specific SPP UUID 
   uServerChannel : the RFCOMM server channel assigned to this SPP service
   szServiceName  : name of this service, UTF8 format

==============================================================================
BTSPPDeviceInfo

Description:
   This structure contains all public information for an SPP server.

Definition:
typedef struct
{
   BTDeviceInfo           bdi;
   uint32                 uNumSPPServices;
   BTSPPServiceRecord     serviceRecords[1];

} BTSPPDeviceInfo;

Members:
   bdi             : generic information for a Bluetooth device
   uNumSPPServices : number of SPP services provided by this SPP server
   serviceRecords  : array of SPP service record retrieved from this SPP server

============================================================================
INTERFACE DOCUMENTATION
==============================================================================
IBTSPP Interface

Description:
   This is a generic interface for bidirectional data transfer over bluetooth 
   serial port profile. 

   This interface provides generic functionalities of Serial port profile such
   as Configure, IO Control, Disconnect, Get Status etc. 

   As IBTSPP is a generic interface, it must be created using class id 
   specific to its usage.

   For example:
   For SPP client, use AEEIID_BT_SPP_CLIENT.
   
   IBTSPP is the BREW interface that supports Bluetooth Serial
              Port Profile.Any interface that support either client or Server 
           functionalities should inherit from this interface


======================================================================
IBTSPP_AddRef()

Description:
   Inherited from IBASE_AddRef().

See Also:
   IBTSPP_Release()
   
==============================================================================
IBTSPP_Release()

Description:
   Inherited from IBASE_Release().

See Also:
   IBTSPP_AddRef()
   
==============================================================================
IBTSPP_QueryInterface()

Description:
   Inherited from IQI_QueryInterface().
   It can be used to
     -  Get a pointer to an available interface based on the input class ID
     -  Query an extended version of the IBTSPP-derived class

Prototype:
   int IBTSPP_QueryInterface (IBTSPP* po, AEECLSID clsReq, 
                              void** ppo);
   
Parameters:
   po [i]     : the object
   clsReq [i] : Class ID of the interface to query.
   ppo [o]    : Pointer to the interface.

Return Value:
   SUCCESS: on success, 
   Otherwise: an error code.

Comments:
   If ppo is back a NULL pointer, the queried interface is not available.

Side Effects:
   If an interface is retrieved, then this function increments its 
   reference count.

==============================================================================
IBTSPP_Read()

Description:
   Inherited from IPORT_Read().
   Currently not supported
   
Return values:
   EUNSUPPORTED

==============================================================================
IBTSPP_Readable()

Description:
   Inherited from IPORT_Readable().
    Currently not supported
   
Return values:
   EUNSUPPORTED

==============================================================================
IBTSPP_GetLastError()

Description:
   Inherited from IPORT_GetLastError().
   Currently not supported
   
Return values:
   EUNSUPPORTED

==============================================================================
IBTSPP_Write()

Description:
   Inherited from IPORT_Write().
   Currently not supported
   
Return values:
   EUNSUPPORTED

   
==============================================================================
IBTSPP_Writeable()

Description:
   Inherited from IPORT_Writeable().
   Currently not supported
   
Return values:
   EUNSUPPORTED


==============================================================================
IBTSPP_IOCtl()

Description:
   Inherited from IPORT_IOCtl().
   Currently not supported
   
Return values:
   EUNSUPPORTED

==============================================================================
IBTSPP_Open()

Description:
   Inherited from IPORT_Open().
   Currently not supported
   
Return values:
   EUNSUPPORTED

==============================================================================
IBTSPP_Close()

Description:
   Inherited from IPORT_Close().
   Currently not supported
   
Return values:
   EUNSUPPORTED


==============================================================================
IBTSPP_Disconnect()

Description:
   This API terminates the SPP connection.
   Currently not suported
   
Return values:
   EUNSUPPORTED

==============================================================================
IBTSPP_OnStatusChange()

Description:
  This API is currently not supported
  
Retrun values:
   EUNSUPPORTED
==============================================================================
IBTSPP_OnModemStatusChange()

Description:
  This API is currently not supported
  
Retrun values:
   EUNSUPPORTED
==============================================================================
IBTSPP_OnLineError()

Description:
  This API is currently not supported
  
Retrun values:
   EUNSUPPORTED
==============================================================================
IBTSPP_GetStatus()

Description:
  This API is currently not supported
  
Retrun values:
   EUNSUPPORTED
==============================================================================
IBTSPP_GetModemStatus()

Description:
  This API is currently not supported
  
Retrun values:
   EUNSUPPORTED
==============================================================================
IBTSPP_GetLineError()

Description:
  This API is currently not supported
  
Retrun values:
   EUNSUPPORTED
      
==============================================================================
IBTSPP_Configure()

Description:
  This API is currently not supported
  
Retrun values:
   EUNSUPPORTED
==============================================================================
IBTSPP_IOCtlEx()

Description:
  This API is currently not supported
  
Retrun values:
   EUNSUPPORTED
============================================================================*/

#endif //AEEBTSPP_H

