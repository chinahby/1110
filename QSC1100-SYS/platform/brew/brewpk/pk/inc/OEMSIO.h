/*===========================================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  oemsio.h

SERVICES:  OEM Serial IO Interface

GENERAL DESCRIPTION:
        Definitions, typedefs, etc. for OEM serial IO

        Copyright 1999-2002 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
#ifndef OEMSIO_H
#define OEMSIO_H

#include "AEE.h"
#include "AEESio.h"
#include "AEEComdef.h"


#ifdef __cplusplus
extern "C" {
#endif

#define OEMSIO_ST_BREW           1       // Device is connected & has entered BREW mode
#define OEMSIO_ST_READABLE       2       // Receive queue is "ready" for reading
#define OEMSIO_ST_WRITEABLE      4       // Transmit queue is "ready" for writing

typedef void (*PFNSIONOTIFY)(uint32 hPort, int unState);

int   OEMSIO_Init(PFNSIONOTIFY pfnNotify);
void  OEMSIO_Exit(void);

int   OEMSIO_Open     ( uint32 hPort                         );
void  OEMSIO_Close    ( uint32 hPort                         );
int   OEMSIO_Read     ( uint32 hPort, char *pcBuf, int nLen  );
int   OEMSIO_Write    ( uint32 hPort, const char *pcBuf, int nLen );
int   OEMSIO_IOCtl    ( uint32 hPort, int nOption, uint32 dwVal   );
int   OEMSIO_GetPortID( const char *pszName, uint32 *phPort       );



#ifdef __cplusplus
}
#endif


/*=====================================================================

  INTERFACE DOCUMENTATION

=======================================================================

Interface Name: OEMSIO

Description:
  This module specifies the functions required for serial IO support
  in BREW.

  There are no requirements for supporting calls from other tasks. It is
  assumed that only BREW will be making calls to this interface.

  Herein, "serial port" refers to a simple asynchronous serial port (RS-232
  or USB virtual port or some lower-power derivative). Specifically, this
  interface does not attempt to describe access to USB in a general manner
  except through OEM supplied USB virtual port that mimick UARTs.

=======================================================================

Function: OEMSIO_Init()

Description:

  This function is called by BREW to indicate that it is ready to accept
  notifications, and to specify the function to be called to deliver
  notifications.

  It should return SUCCESS. Non-functional stubs for this API will return EFAILED
  to indicate that serial I/O is unsupported.

  The function pointer passed by BREW should be used by the OEM serial layer
  to notify BREW of events. This notification function can be called from any
  task; BREW is responsible for taking care of thread safety issues.

Prototype:
  int OEMSIO_Init(PFNSIONOTIFY pfnNotify)

Parameters:
  pfnNotify = function to call with notifications

Return Value:
  SUCCESS (serial IO supported)
  EFAILED (serial IO unsupported)

Comments:
  None.

Side Effects:
  None.

See Also:
  OEMSIO_Exit()

===========================================================================

Function: OEMSIO_Exit()

Description:

  This function can be called by BREW at some point after OEMSIO_Init() to
  indicate that the previously-registered notify callback should not be
  called anymore.

  This will not otherwise affect the state of the serial driver or the
  receive and transmit buffers.


Prototype:
  void OEMSIO_Exit(void);

Parameters:
  None.

Return Value:
  None.

Comments:
  None.

Side Effects:
  None.

See Also:
  OEMSIO_Init()


===========================================================================

Function: OEMSIO_Open()

Description:

   This function is called to open and configure a serial port.

Prototype:
   int OEMSIO_Open(int hPort);

Parameters:

   hPort             port number in the range of 1 to (max ports)

Return Value:
   SUCCESS           port successfully opened.
   ENOSUCH           Invalid port number
   EFAILED           Failed to open port
   AEEPORT_WAIT      Port being opened.. Wait on writeable to get t


Comments:
  None

Side Effects:
  None

See Also:
  None
===========================================================================

Function: OEMSIO_Close()

Description:
   Port is closed and the control is given back to ATCOP of the Mobile

Prototype:
   void OEMSIO_Close(int hPort);

Return Value:
  None

Comments:
  None

Side Effects:
  None

See Also:
  None
===========================================================================

Function:  OEMSIO_Read()

Description:

  This function copies data from the receive buffer into the pcBuf[] buffer
  supplied by the caller. As much data as is available should be copied, never
  exceeding nLen bytes and never exceeding the number of bytes available in
  the receive buffer. Data copied into pcBuf[] should be removed from the
  receive buffer.

Prototype:

  int OEMSIO_Read(uint32 hPort, char *pcBuf, int nLen)

Parameters:

Return Value:
  The number of bytes removed from the receive buffer.

  If the receive buffer is empty, no bytes should be copied and this function
  should return 0.

  If the port is not in BREW serial mode, -1 should be returned.

Comments:
  None

Side Effects:
  None

See Also:
  None

===========================================================================

Function:  OEMSIO_Write()

Description:

  This should append nLen bytes from the specified buffer (pcBuf) onto the end
  of the transmit buffer. If the buffer cannot accomodate nLen bytes, as many
  bytes as can fit should be appended.

Prototype:

  int OEMSIO_Write(uint32 hPort, const char *pcBuf, int nLen)

Parameters:

Return Value:

  The number of bytes appended to the transmit buffer.

  If the receive buffer is full, no bytes should be copied and this function
  should return 0.

  If the port is not in BREW serial mode, -1 should be returned.

Comments:
  None

Side Effects:
  None

See Also:
  None
===========================================================================

Function:  OEMSIO_IOCtl()

Prototype:
   int OEMSIO_IOCtl(uint32 hPort, uint32 dwVal);

Description:

Options Id:

  Currently supported option ids:
  AEESIO_IOCTL_GCONFIG - Get the present configuration of the serial port
                         by filling the passed in AEESIOConfig pointer

  AEESIO_IOCTL_SCONFIG - Set the given configuration of the serial port
                         passed by AEESIOConfig pointer


  AEESIO_IOCTL_SET_TRIGGERS - Set triggers by suppling pointer to AEESIOTriggers strcuture

   nReadIdle = number of milliseconds of idle time after receiving a character
               that indicate a "triggered" state. The triggers are only valid
               on nReadIdle being set

   cbTrigRead = number of available bytes that will trigger readability
      Reading: If the number of bytes received >= cntRead, the receive queue is
               "ready".  If there are not that many bytes waiting, then the queue will
               be considered "ready" if one or more characters arrive AND then an idle
               timeout expires.

   cbTrigWrite = number of bytes of room in the write buffer writeability
       Writing: If there is room in the write buffer for cntWrite or more bytes,
               the write queue is considered "ready".

Return Values:
  SUCCESS -  the option id is known and the specified operation is being handled
  EUNSUPPORTED - the option id was unrecognized
  E??? - other failure return codes, depending on the option id specified

Comments:
  None

Side Effects:
  None

See Also:
  None
===========================================================================

Function: OEMSIO_GetPortID()

Description:
   This function is called to get the port Identifier of the given port name.
   This is used to map the port name to a small integer with in the limits
   specified by the AEESIO_MAX_PORTS constant.

Prototype:
   int OEMSIO_GetPortID(const char *pszName, uint32 *phPort)

Parameters:

   pszPortName       Name of the port
   *phPort           pointer to the port handle


Return Value:
   SUCCESS - Supports the requested port name, *phPort has valid port handle
   EFAILED - No support for the request port name. *phPort is not updated

Comments:
  None

Side Effects:
  None

See Also:
  None

==============================================================
AEESIO

See the IPort Interface in the BREW API Reference.
 

======================================================================
DATA STRUCTURE DOCUMENTATION
=======================================================================

PFNSIONOTIFY

Description:

  This function is called by the OEM layer to notify BREW of certain state
  transitions.

  Generally, this function must be called to notifiy BREW of any change(s) to
  the state flags described below.  However, transitions of
  OEMSIO_ST_READABLE and OEMSIO_ST_WRITEABLE flags from TRUE to FALSE
  shall *not* result in a call to this function.

  When OEMSIO_Init() is called, BREW will assume the current state is zero (0),
  so if any state flags are TRUE, PFNSIONOTIFY should be called at that point.


Prototype:

  void (*PFNSIONOTIFY)(uint32 hPort, uint32 unState)

Parameters:

  nPort: the ID of the serial port (0, when only one serial port is supported)

  unState: one of the following events:
~
    OEMSIO_ST_BREW : indicates that the serial port is in BREW mode.  This mode
      is entered when an "at$brew" command is received from an external device.
      After entering this mode, the OEM layer should be prepared for BREW
      to access the port via the OEMSIO_Read() and OEMSIO_Write() functions.
      This mode is exited when the external device is disconnected from the
      phone (see "Shutdown" note, below).

    OEMSIO_ST_READABLE : this flag is true when the receive queue is ready
      for reading.  The precise meaning of "ready" is defined in the
      description of the AEESIO_IOCTL_SET_TRIGGERS option.

    OEMSIO_ST_WRITEABLE : this flag is true when the transmit queue is ready
      for writing.  The precise meaning of "ready" is defined in the
      description of the AEESIO_IOCTL_SET_TRIGGERS option.
*

   NOTE: If OEMSIO_Read function returns 0 bytes, the OEMSIO_ST_READABLE must be
         turned off in the OEM layer.  If the state becomes readable therefater, it
         must be reported.

         If OEMSIO_Write function returns 0 bytes, the OEMSIO_ST_WRITEABLE must be
         turned off in the OEM layer.  If the state becomes writeable therefater, it
         must be reported.


===========================================================================*/

#endif  // OEMSIO_H
