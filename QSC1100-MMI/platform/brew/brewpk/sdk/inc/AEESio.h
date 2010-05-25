
/*============================================================================
FILE:	AEESIO.h

SERVICES:  Basic Serial IO Services

DESCRIPTION:
   This file provides defintions for the IPort interface for Serial Ports
   made available by the AEE to application developers. This is a
   standard header file that must be included by all applications
   using the SIO services of the AEE. For using IPort to open a serial port,
   the classs id AEECLSID_SERIAL ought to be used.

   
INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
============================================================================*/
#ifndef _AEESIO_H_
#define _AEESIO_H_

#include "AEEPort.h"    // IPort interface definition
#include "AEEIOCtl.h"

#ifdef __cplusplus
extern "C" {
#endif


/*==========================================================================

   For controlling serial ports (defined below), the IPort must be creating 
   using AEECLSID_SERIAL

============================================================================*/

#define AEECLSID_HTYPE_SERIALDEVICE    	0x01011be6



   
// AEESIOConfig Baud rate
// For BaudRate, well known baud rates should be used
// 115200 is the most commonly used baud rate and recommended for
// default usage

// Other well known baud rates are 38400, 19200 etc

// AEESIOConfig data bits
// The number of data bits per character.
// Allowed values are 5,6,7 and 8 bits.

#define AEESIO_DATABITS_5      5
#define AEESIO_DATABITS_6      6
#define AEESIO_DATABITS_7      7
#define AEESIO_DATABITS_8      8  //Use this as default

// AEESIOConfig stop bits
#define AEESIO_STOPBITS_1      1  //Use this as default 
#define AEESIO_STOPBITS_2      2

// AEESIOConfig parity
#define AEESIO_PARITY_NONE     0  //Use this as default 
#define AEESIO_PARITY_ODD      1
#define AEESIO_PARITY_EVEN     2
#define AEESIO_PARITY_MARK     3
#define AEESIO_PARITY_SPACE    4

// AEESIOConfig flow control
#define AEESIO_FLOW_NONE       0     // No flow control
#define AEESIO_FLOW_HARDWARE   1     // RTS/CTS flow control
#define AEESIO_FLOW_SOFTWARE   2     // XON/XOFF flow control. 
                                     // When this option is used, the data flow must not contain 
                                     // the XON/XOFF characters (ASCII 17 and 19)


// Use the following structure to configure the SIO device.
// If in doubt, use the default values of
// 115200, AEESIO_DATABITS_8, AEESIO_STOPBITS_1, AEESIO_FLOW_SOFTWARE and AEESIO_PARITY_NONE
typedef struct {
   uint32      dwBaudRate;
   int         nDataBits;
   int         nStopBits;
   int         fFlowControl;
   int         fParity;
} AEESIOConfig;

/*===========================================================================
  nReadIdleTimeout: 
      Number of milliseconds of idle time after receiving a character that 
      indicate a "triggered" state. The following triggers are only valid 
      if nReadIdleTimeout is set.

    cbReadTrigger : Number of available bytes that will trigger readability

    cbWriteTrigger: Number of bytes of room in the write buffer writeability
===========================================================================*/
typedef struct {
   uint32 nReadIdleTimeout;
   int    cbReadTrigger;
   int    cbWriteTrigger;
} AEESIOTriggers;




/*===========================================================================
   IPORT_Open taken a string as an argument to open a port. Some of the most 
   commonly used named are defined below.  An optional name scheme can also be
   specified to make IPort a very genric interface. The port name scheme for SIO
   is defined below as SIO_PORT_NAME_SCHEME.

   Examples:
   To open PORT1, you can use one of the following schemes.
   IPORT_Open(po, "PORT1") or
   IPORT_Open(po, "SIO:PORT1")

   Both IPORT_Opens above are referring to the same port.
===========================================================================*/

#define SIO_PORT_NAME_SCHEME    "SIO:"


/*===========================================================================
   BREW defines a set of port names for opening ports
   AEESIO_PORT_ part is common for all names. The rest of the string generally
   refers to the type of the port and its number. The types are expalined below.

   SIO: in general serial devices that are UARTs
   USB: USB ports
   IR : Ir (IrDA) ports

   OEMS are free to provide port types in addition to those listed below,
   and are similiarly free to implement more ports than listed below.
===========================================================================*/

#define AEESIO_PORT_SIO_PREFIX "PORT"
#define AEESIO_PORT_SIO1   (AEESIO_PORT_SIO_PREFIX "1")
#define AEESIO_PORT_SIO2   (AEESIO_PORT_SIO_PREFIX "2")
#define AEESIO_PORT_SIO3   (AEESIO_PORT_SIO_PREFIX "3")
#define AEESIO_PORT_SIO4   (AEESIO_PORT_SIO_PREFIX "4")

#define AEESIO_PORT_USB_PREFIX "USB"
#define AEESIO_PORT_USB1   (AEESIO_PORT_USB_PREFIX "1")
#define AEESIO_PORT_USB2   (AEESIO_PORT_USB_PREFIX "2")
#define AEESIO_PORT_USB3   (AEESIO_PORT_USB_PREFIX "3")
#define AEESIO_PORT_USB4   (AEESIO_PORT_USB_PREFIX "4")

#define AEESIO_PORT_IR_PREFIX "IR"
#define AEESIO_PORT_IR1    (AEESIO_PORT_IR_PREFIX "1")
#define AEESIO_PORT_IR2    (AEESIO_PORT_IR_PREFIX "2")

//This is an abstarct name to indicate that the port is waiting to be connected
#define AEESIO_PORT_INCOMING "Inc"


// IOCtl Option IDs for SIO

//Set Read/Write triggers
#define AEESIO_IOCTL_SET_TRIGGERS  BREWIOCTL_R(BREWIOCTL_SIO_TYPE, 1, AEESIOTriggers)

//Get the port config info
#define AEESIO_IOCTL_GCONFIG       BREWIOCTL_RW(BREWIOCTL_SIO_TYPE, 2, AEESIOConfig)

//Set the given port config info
#define AEESIO_IOCTL_SCONFIG       BREWIOCTL_R(BREWIOCTL_SIO_TYPE, 3, AEESIOConfig)

//The following three IOCTLs should not be used.
#define AEESIO_IOCTL_RESERVED4     BREWIOCTL_RW(BREWIOCTL_SIO_TYPE, 4, int)
#define AEESIO_IOCTL_RESERVED5     BREWIOCTL_RW(BREWIOCTL_SIO_TYPE, 5, int)
#define AEESIO_IOCTL_RESERVED6     BREWIOCTL_RW(BREWIOCTL_SIO_TYPE, 6, int)

#ifdef __cplusplus
}
#endif


/*=====================================================================
  
   SIO specific option IDs valid in IPORT_IOCtl()

   =========================================================
   AEESIO_IOCTL_GCONFIG
      Get the present configuration of the serial port.

   Option Value: AEESIOConfig pointer. 
   
   Example:
      AEESIOConfig cfg;
      nStatus = IPORT_IOCtl(piPort, AEESIO_IOCTL_GCONFIG, (uint32)&cfg);
      if(nStatus == SUCCESS) {
         //cfg now contains the port configuration
      }

   =========================================================
   AEESIO_IOCTL_SCONFIG
      Set the given configuration of the serial port passed by the AEESIOConfig 
      pointer. 

      Setting configuration may not always correctly configure the port to the
      specified values, due to the port/device limitations.  Please do a 
      AEESIO_IOCTL_GCONFIG to retrieve the resulting set values.

      If the port is not opened, AEESIO_IOCTL_GCONFIG returns EFAILED,
      eventhough AEESIO_IOCTL_SCONFIG is used previously to set the config. 
      These AEESIO_IOCTL_SCONFIG is cached and set once the port is open.
   
   Option Value: AEESIOConfig pointer. 
   
   Example:
      AEESIOConfig cfg;
      cfg.dwBaudRate    = 115200;
      cfg.nDataBits     = AEESIO_DATABITS_8;
      cfg.nStopBits     = AEESIO_STOPBITS_1;
      cfg.fFlowControl  = AEESIO_FLOW_NONE;
      cfg.fParity       = AEESIO_PARITY_NONE;

      nStatus = IPORT_IOCtl(piPort, AEESIO_IOCTL_SCONFIG, (uint32)&cfg);
   
   =========================================================
   AEESIO_IOCTL_SET_TRIGGERS
      Set triggers by suppling pointer to AEESIOTriggers strcuture

   Option Value: AEESIOTriggers pointer. 

   Example:
      AEESIOTriggers trig;
      trig.nReadIdleTimeout = 100; //ms
      trig.cbReadTrigger    = 64;
      trig.cbWriteTrigger   = 0;
      
      nStatus = IPORT_IOCtl(piPort, AEESIO_IOCTL_SET_TRIGGERS, (uint32)&trig);

   
   AEESIOTriggers Members:

   nReadIdleTimeout:
      Number of milliseconds of idle time after receiving a character that indicate 
      a "triggered" state. The triggers are only valid on nReadIdle being set

   cbReadTrigger: 
      Number of available bytes that will trigger readability
   
      While reading, if the number of bytes received >= cbReadTrigger, the receive queue is
      considered "ready".  If there are not that many bytes waiting, then the queue will
      be considered "ready" only if one or more characters arrived AND the idle timeout 
      nReadIdleTimeout expires.

   cbWriteTrigger:
     Minimum number of bytes of space in the internal write buffer, to be considered 
     writeable. In other words, if there is empty space in the write buffer for 
     cbWriteTrigger or more bytes, the write queue is considered "ready".

======================================================================= 

SIO Port Names

  Currently supported port names are declared in the AEESio.h header file in
  the format of AEESIO_PORT_XXX. The port name AEESIO_PORT_INCOMING has a 
  special meaning and opening it will acquire a device-initiated port requesting 
  the calling app.
  
=======================================================================*/

#endif  //_AEESIO_H_

