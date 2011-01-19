#ifndef AEEUSBDRIVER_H
#define AEEUSBDRIVER_H
/*======================================================
FILE:  AEEUSBDriver.h

SERVICES:  BREW USB Driver Implementation API

GENERAL DESCRIPTION:
   These are APIs to expose USB internals so that BREW applications can
   implement an arbitrary USB driver.

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IUSBDriver

       Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

/*--- Include files ---*/
#include "AEE.h"
#include "AEEPort.h"
#include "AEEIOCtl.h"
#include "AEECLSID_USBDRIVER.bid"

/*--- Constant Definitions ---*/
// Constants used in other macros below
#define USBDRIVER_PORT_NAME_SCHEME     "USBDRIVER:"
#define AEEUSBDRIVER_BULK_BASE         "BULK:"

/*===========================================================================
   IUSBDRIVER_Open takes a string as an argument to open a port. Some of the 
   most commonly used names are defined below.

   Examples:
   To open birectional BULK data endpoints for your USB interface, do:
   IPORT_Open(po, "USBDRIVER:BULK:BIDIR") or just
   IPORT_Open(po, "BULK:BIDIR")

   Both IPORT_Opens above are referring to the same port.
===========================================================================*/

// Valid values for the IUSBDRIVER_Open() call
#define AEEUSBDRIVER_BULK_BIDIR        (AEEUSBDRIVER_BULK_BASE "BIDIR")
#define AEEUSBDRIVER_BULK_INONLY       (AEEUSBDRIVER_BULK_BASE "IN")
#define AEEUSBDRIVER_BULK_OUTONLY      (AEEUSBDRIVER_BULK_BASE "OUT")
#define AEEUSBDRIVER_BULK_NONE         (AEEUSBDRIVER_BULK_BASE "NONE")

#define AEEUSBDRIVER_BM_INENDPOINT     0x0001  
#define AEEUSBDRIVER_BM_OUTENDPOINT    0x0002    

// IOCtl Option IDs for IUSBDriver
#ifndef BREWIOCTL_USBDRIVER_TYPE
// This is included for backporting to versions of BREW that do not have
// BREWIOCTL_USBDRIVER_TYPE defined.
#define BREWIOCTL_USBDRIVER_TYPE (3)
#endif /* BREWIOCTL_USBDRIVER_TYPE */

typedef struct {
   byte *pData;
   int   nSize;
} AEEUSBDriver_ReadType;

#define AEEUSBDRIVER_IOCTL_GETCNXNSTATUS  BREWIOCTL_RW(BREWIOCTL_USBDRIVER_TYPE, 1, boolean)
#define AEEUSBDRIVER_IOCTL_NTFYCNXNSTATUS BREWIOCTL_RW(BREWIOCTL_USBDRIVER_TYPE, 2, AEECallback)
#define AEEUSBDRIVER_IOCTL_NTFYBUFEMPTY   BREWIOCTL_RW(BREWIOCTL_USBDRIVER_TYPE, 3, AEECallback)
#define AEEUSBDRIVER_IOCTL_SETSTREAMMODE  BREWIOCTL_RW(BREWIOCTL_USBDRIVER_TYPE, 4, boolean)
#define AEEUSBDRIVER_IOCTL_GETSTREAMMODE  BREWIOCTL_RW(BREWIOCTL_USBDRIVER_TYPE, 5, boolean)
#define AEEUSBDRIVER_IOCTL_FLUSHENDPOINT  BREWIOCTL_RW(BREWIOCTL_USBDRIVER_TYPE, 6, uint32)
#define AEEUSBDRIVER_IOCTL_STALLENDPOINT  BREWIOCTL_RW(BREWIOCTL_USBDRIVER_TYPE, 7, uint32)
#define AEEUSBDRIVER_IOCTL_READCTRLDATA   BREWIOCTL_RW(BREWIOCTL_USBDRIVER_TYPE, 8, AEEUSBDriver_ReadType)

/*--- Data Types ---*/
typedef struct IUSBDriver IUSBDriver;
typedef struct AEEUSBSetup_Req AEEUSBSetup_Req;
typedef boolean (*PFNUSBSETUPNOTIFIER)(void *pUser, AEEUSBSetup_Req *pReq);

struct AEEUSBSetup_Req
{
   uint16   wRequestAndType;
   union
   {
      word wValue;
      struct
      {
         byte cIndex;
         byte cType;
      } Descriptor;
   } uValue;
   uint16   wIndex;
   uint16   wLength;
};

AEEINTERFACE(IUSBDriver)
{
   INHERIT_IPort(IUSBDriver);

   IPort* (*GetInterruptEndpoint)(IUSBDriver *po, int nLatency);
   void (*SetSetupNotifier)(IUSBDriver *po, PFNUSBSETUPNOTIFIER pfn, void *pUser);
   int (*SendSetupResponse)(IUSBDriver *po, const byte *pResponse, int nSize);
};

// Accessor macros
#define IUSBDRIVER_AddRef(p)                 AEEGETPVTBL((p),IUSBDriver)->AddRef(p)
#define IUSBDRIVER_Release(p)                AEEGETPVTBL((p),IUSBDriver)->Release(p)
#define IUSBDRIVER_QueryInterface(p,i,ppo)   AEEGETPVTBL((p),IUSBDriver)->QueryInterface(p,i,ppo)
#define IUSBDRIVER_Read(p,pc,cb)             AEEGETPVTBL((p),IUSBDriver)->Read(p,pc,cb)
#define IUSBDRIVER_Readable(p,pcb)           AEEGETPVTBL((p),IUSBDriver)->Readable(p,pcb)
#define IUSBDRIVER_GetLastError(p)           AEEGETPVTBL((p),IUSBDriver)->GetLastError(p)
#define IUSBDRIVER_Write(p,pc,cb)            AEEGETPVTBL((p),IUSBDriver)->Write(p,pc,cb)
#define IUSBDRIVER_Writeable(p,pcb)          AEEGETPVTBL((p),IUSBDriver)->Writeable(p,pcb)
#define IUSBDRIVER_IOCtl(p,opt,v)            AEEGETPVTBL((p),IUSBDriver)->IOCtl(p,opt,v)
#define IUSBDRIVER_Close(p)                  AEEGETPVTBL((p),IUSBDriver)->Close(p)
#define IUSBDRIVER_Open(p,pname)             AEEGETPVTBL((p),IUSBDriver)->Open(p,pname)
#define IUSBDRIVER_GetInterruptEndpoint(p,l) AEEGETPVTBL((p),IUSBDriver)->GetInterruptEndpoint(p,l)
#define IUSBDRIVER_SetSetupNotifier(p,pfn,pu) AEEGETPVTBL((p),IUSBDriver)->SetSetupNotifier(p,pfn,pu)
#define IUSBDRIVER_SendSetupResponse(p,pc,cb) AEEGETPVTBL((p),IUSBDriver)->SendSetupResponse(p,pc,cb)

static __inline int IUSBDRIVER_ReadCtrlData(IUSBDriver *po, byte *pData, int nSize)
{
   AEEUSBDriver_ReadType readStruct;
   readStruct.pData = pData;
   readStruct.nSize = nSize;

   return IUSBDRIVER_IOCtl(po, AEEUSBDRIVER_IOCTL_READCTRLDATA,
                              (uint32)&readStruct);
}

/*

=======================================================================
  DATA TYPES DOCUMENTATION
=======================================================================

AEEUSBSetup_Req

Description:
   A data structure representing unknown control channel setup messages.
   Refer to table 9-2 of the USB 1.1 specification for complete details.

Definition:
   typedef struct AEEUSBSetup_Req
   {
      uint16   wRequestAndType;
      union
      {
         word wValue;
         struct
         {
            byte cIndex;
            byte cType;
         } Descriptor;
      } uValue;
      uint16   wIndex;
      uint16   wLength;
   } AEEUSBSetup_Req;

Members:
   wRequestAndType:
      Most Significant Byte is bmRequestByte ("Characteristics of Request"):
         Bit 7:   Data transfer direction.
                     0 = Host-to-device
                     1 = Device-to-host
         Bit 6-5: Type:
                     0 = Standard
                     1 = Class
                     2 = Vendor
                     3 = Reserved
         Bit 4-0: Recipient:
                     0 = Device
                     1 = Interface
                     2 = Endpoint
                     3 = Other
                     4..31 = Reserved
      Least Significant Byte is bRequest ("Specific Request"):
         (The following request is abbreviated.  Refer to the USB specification
         for full details).
            0x06 = GET_DESCRIPTOR

   uValue.wValue:
      Value specific to the request type specified in wRequestAndType.
   
   uValue.cType:
      For GET_DESCRIPTOR setup messages:
         STRING = 0x3.  Used to specify configuration strings, including
                        the OS Descriptor.

   uValue.cIndex:
      Specific

   wIndex:
      Index value.  Specific meaning is based on type specified in
      wRequestAndType.  For string descriptors, this should be the
      language ID.

   wLength:
      Number of bytes to transfer if there is a data stage.


=======================================================================
  IOCTLS  DOCUMENTATION
=======================================================================
   AEEUSBDRIVER Interface
   
   USB speicific option IDs valid in IUSBDRIVER_IOCtl()

   ========================================================
   AEEUSBDRIVER_IOCTL_GETCNXNSTATUS
      Get the current connection status of the device with respect to
      a USB host.  This may be TRUE even if the interface has not been
      opened.

      This IOCtl can be called only while the IUSBDriver object is open.

   Option Value: Pointer to a boolean value

   Example:
      boolean bConnected;
      nErr = IUSBDRIVER_IOCtl(piPort, AEEUSBDRIVER_IOCTL_GETCNXNSTATUS,
                              (uint32)&bConnected);
      if (nErr == SUCCESS)
      {
         // bConnected is true if the device is connected to a USB host
      }

   ========================================================
   AEEUSBDRIVER_IOCTL_NTFYCNXNSTATUS
      Set a callback to be called when the user should recheck
      the connection status provided by AEEUSBDRIVER_IOCTL_GETCNXNSTATUS.

      The callback structure must remain in memory until it is either
      cancelled or called back.

      This callback can be registered after a successful initial
      IUSBDRIVER_Open() and before the object is closed.  The call,
      like other AEECallbacks is one-shot; the callback must be
      re-registerd after the callback executes.

   Option Value: Pointer to an AEECallback structure

   Example:
      CALLBACK_Init(&pMyApp->cbUSBStatus, MyApp_RecheckUSBStatus, 
                    (void*)pMyApp);
      nErr = IUSBDRIVER_IOCtl(piPort, AEEUSBDRIVER_IOCTL_NTFYCNXNSTATUS,
                              (uint32)(&pMyApp->cbUSBStatus));
      if (nErr == SUCCESS)
      {
         // the MyApp_RecheckUSBStatus function will be called when
         // appropriate with the pMyApp pointer as its argument.
      }

   See Also:
      CALLBACK_Init()
      CALLBACK_Cancel()

   ========================================================
   AEEUSBDRIVER_IOCTL_NTFYXBUFEMPTY
      Set a callback to be called when there is no more data to be sent
      on a given endpoint.  Valid for both the bulk IPort as well as the
      IPort returned by IUSBDRIVER_GetInterruptEndpoint().

      The IOCtl can be called anytime after a successful IUSBDRIVER_Open()
      call and before the object is closed.  After the callback is called,
      it must be re-registered to receive subsequent notifications.

   Option Value: Pointer to an AEECallback structure

   Example:
      // Let us know when all of the data on the interrupt endpoint has
      // been transmitted.
      CALLBACK_Init(&pMyApp->cbIntEmpty, MyApp_IntEmptyCB, (void*)pMyApp);
      nErr = IUSBDRIVER_IOCtl(piPort, AEEUSBDRIVER_IOCTL_NTFYBUFEMPTY,
                              (uint32)(&pMyApp->cbIntEmpty));
      if (nErr == SUCCESS)
      {
         // The MyAPP_IntEmptyCB function will be called when all data 
         // has been transmitted.
      }

   See Also:
      CALLBACK_Init()
      CALLBACK_Cancel()

   ========================================================
   AEEUSBDRIVER_IOCTL_SETSTREAMMODE
      Sets the current stream mode of the IPORT. If this is set to FALSE
      each IUSBDriver_Write will cause the USB transfer to be terminated
      with a short/zero length packet. If this is set to TRUE than the 
      USB transfer will only be terminated by calling IUSBDriver_Write
      with a size of 0.
      This setting will default to FALSE.

      For best performance when this IOCTL is set to TRUE, the writes
      should be multiples of 512 bytes.

   Option Value: boolean value

   Example:
      nErr = IUSBDRIVER_IOCtl(piPort, AEEUSBDRIVER_IOCTL_SETSTREAMMODE,
                              TRUE);
      if (nErr == SUCCESS)
      {
         // Transfers will now only be ended by writes of 0
      }
      
   ========================================================
   AEEUSBDRIVER_IOCTL_GETSTREAMMODE
      Gets the current stream mode of the IPORT. If this is set to FALSE
      each IUSBDriver_Write will cause the USB transfer to be terminated
      with a short/zero length packet. If this is set to TRUE than the 
      USB transfer will only be terminated by calling IUSBDriver_Write
      with a size of 0.
      This setting will default to FALSE.

   Option Value: Pointer to a boolean value

   Example:
      boolean bStreamMode;
      nErr = IUSBDRIVER_IOCtl(piPort, AEEUSBDRIVER_IOCTL_GETSTREAMMODE,
                              (uint32) &bStreamMode);
      if (nErr == SUCCESS)
      {
         //bStreamMode will now contain the stream mode state.
      }

   ========================================================
   AEEUSBDRIVER_IOCTL_FLUSHENDPOINT
      This IOCtl will cause any data that is pending for a given endpoint to
      be discarded. The uint32 option value will be a bitmask of the endpoints
      to be flushed. 

   Option Value: uint32

   Example:
      nErr = IUSBDRIVER_IOCtl(piPort, AEEUSBDRIVER_IOCTL_FLUSHENDPOINT,
                              AEEUSBDRIVER_BM_INENDPOINT);
      if (nErr == SUCCESS)
      {
         //Any data pending on the IN endpoint will be flushed.
      }

   ========================================================
   AEEUSBDRIVER_IOCTL_STALLENDPOINT
      This IOCtl will cause the given endpoint to be stalled.
      The uint32 option value is a bitmask of the endpoints
      to be flushed. 

   Option Value: uint32

   Example:
      nErr = IUSBDRIVER_IOCtl(piPort, AEEUSBDRIVER_IOCTL_STALLENDPOINT,
                              AEEUSBDRIVER_BM_INENDPOINT | AEEUSBDRIVER_BM_OUTENDPOINT); 
      if (nErr == SUCCESS)
      {
         //Both the IN and OUT endpoints are stalled.
      }

   ========================================================
   AEEUSBDRIVER_IOCTL_READCTRLDATA
      This IOCtl is used for the implementation of IUSBDRIVER_ReadCtrlData
      and should not be used by itself. See IUSBDRIVER_ReadCtrlData for more
      information. 

   Option Value: Pointer to an AEEUSBDriver_ReadType 

   Example:
      See IUSBDRIVER_ReadCtrlData.

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IUSBDriver

The IUSBDriver interface allows a BREW application to access USB-specific
constructs necessary to write a USBDriver.

The IUSBDriver inherits from the IPort class and uses that class to
represent the BULK endpoints on the device.  It contains additional
methods to access other USB-specific items such as vendor-specific
configuration items and Interrupt data.

The following header file is requierd:~
AEEUSBDriver.h

=============================================================================

IUSBDRIVER_AddRef()

Description:
   This function is inherited from IBASE_AddRef().

See Also:
   IUSBDRIVER_Release()

=============================================================================

IUSBDRIVER_Release()

Description:
   This function is inherited from IBASE_Release().

See Also:
   IUSBDRIVER_AddRef()

==============================================================================

IUSBDRIVER_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

See Also:
   None

==============================================================================

IUSBDRIVER_Read()

Description:
   This function is inherited from ISOURCE_Read().

   The data read corresonds to data on the USB bulk-out endpoint.

See Also:
   IUSBDRIVER_Readable()
   IUSBDRIVER_Write()

==============================================================================

IUSBDRIVER_GetLastError()

Description:
   This function is inherited from IPORT_GetLastError()

See Also:
   None

==============================================================================

IUSBDRIVER_Write()

Description:
   This function is inherited from IPORT_Write().

   The data written corresponds to data on the USB bulk-in endpoint.

See Also:
   IUSBDriver_Read()
   IUSBDriver_Writeable()

==============================================================================

IUSBDRIVER_Writeable()

Description:
   This function is inherited from IPORT_Writeable().

See Also:
   IUSBDRIVER_Write()

==============================================================================

IUSBDRIVER_IOCtl()

Description:
   This function is inherited from IPORT_IOCtl().

   IOCtl operations specific to USB are documented above.

See Also:
   None

=============================================================================

IUSBDRIVER_Close()

Description:
   This function is inherited from IPORT_Close().

   This function closes the BREW USB interface and allows its control to
   pass back to the native OEM driver.

   When calling IUSBDRIVER_Close(), the interface may return an 
   AEEPORT_WAIT error code.  In this case, a callback should be
   registered with the IUSBDRIVER_Writeable() function.
   Upon reception of the callback, the user should check
   IUSBDRIVER_GetConnectionStatus() to determine the status of the call.

Prototype:
   int IUSBDRIVER_Disable(IUSBDriver *po);

Parameters:
   po: [in]: the interface pointer

Return Value:
   SUCCESS: Control of the USB port was successfully returned to the OEM

   AEEPORT_WAIT: The USB driver is attempting to disable BREW mode.

   EFAILED: other miscellaneous error

Comments:
   None.

Version:
   Introuced BREW Client <2.1>

See Also:
   IPORT_Close()
   IUSBDRIVER_Open()

==============================================================================

IUSBDRIVER_Open()

Description:
   This function is inherited from IPORT_Open().

   This function enables the BREW USB interface.  By enabling the interface,
   the OEM may disconnect its current native driver and restart negotiations
   with the USB host using the BREW driver.

   All interface endpoints must be made prior to calling IUSBDRIVER_Open().

   When calling IUSBDRIVER_Open(), the interface may return a
   AEEPORT_WAIT error code.  In this case, a callback should be registered
   with the IUSBDRIVER_Writeable() function.  Upon reception
   of the callback, the user should call IUSBDRIVER_Open again with a NULL
   argument to check the result of the operation.

Prototype:
   int IUSBDRIVER_Open(IUSBDriver *po, const char *pszPort);

Parameters:
   po: [in]: the interface pointer

   pszPort: [in]: pointer to a valid open string.  Valid open strings
      currently are one of the following:
         AEEUSBDRIVER_BULK_BIDIR:   Bind the interface to both a USB
            bulk-in a USB bulk-out endpoints.

         AEEUSBDRIVER_BULK_INONLY:  Bind the interface to a USB bulk-in
            (device-to-PC) endpoint only.  The _Read() call will always
            return 0 in this case and the _Readable() callback will always
            fire immediately.

         AEEUSBDRIVER_BULK_OUTONLY: Bind the interface to a USB bulk-out
            (PC-to-device) endpoint only.  The _Write() call will always
            return 0 in this case and the _Writeable() callback will always
            fire immediately, except when the port is transitioning to an
            open or a closed state.

         AEEUSBDRIVER_BULK_NONE: Do not bind the interface to either
            a bulk-in or bulk-out endpoint.

      Note that the OEM may not support some of these options.

Return Value:
   SUCCESS: The USB Driver interface was successfully enabled/opened.

   AEEPORT_WAIT: The USB Driver is attempting to open the BREW
      driver.

   EFAILED: Other miscellaneous error

Comments:
   Calling IUSBDRIVER_GetInterruptEndpoint() after opening the interface is
   not allowed.

Version:
   Introduced BREW Client <2.1>

See Also:
   IPORT_Open()
   IUSBDRIVER_Close()

=============================================================================

IUSBDRIVER_GetInterruptEndpoint

Description:
   This function allocates an interrupt endpoint on the device and returns
   an IPort object used to access that endpoint.  Endpoint addresses
   are assigned by the OEM.

Prototype:
   IPort* IUSBDRIVER_GetInterruptEndpoint(IUSBDriver *po, int nLatency);

Parameters:
   po: [in]: the interface pointer

   nLatency: [in]: the maximum latency, in milliseconds between polls
      from the host.  This value must conform to legal values in the
      USB specification.

Return Value:
   An IPort object if successful.
   NULL otherwise.

Comments:
   The returned IPort object is assumed to be open.  Releasing or closing
   the returned IPort object will deallocate the associated endpoint and
   must be done only in the disabled state.  The open method of the returned
   object has no effect and will return SUCCESS if the endpoint resources
   are still allocated (i.e. the IPort has not been IPORT_Close()'d) or
   EFAILED if the IPort has been closed previously.  To reallocate
   endpoint resources, call IUSBDRIVER_GetInterruptEndpoint again.

   Any callback registered with IPORT_Writeable() will immediately be
   resumed.  Any call to IPORT_Write() on the returned IPort will return
   an AEEPORT_FAILED error code.

   Details such as maximum PDU size on the endpoint and transfer/transaction
   management are left to the OEM to implement.

See Also:
   IPORT_Read
   IPORT_Readable
   IPORT_Write
   IPORT_Writeable
   IPORT_Close
   IUSBDRIVER_GetConnectionStatus

=============================================================================

IUSBDRIVER_SetSetupNotifier

Description:
   Sets a callback function to be called when the device receives an
   unknown vendor-specific request from the host.

   To cancel the callback, call this function with a NULL pfn argument.

   This callback remains in effect until cancelled, either explicitly or
   by releasing the IUSBDriver interface.

Prototype:
   void IUSBDRIVER_SetSetupNotifier(IUSBDriver *po, PFNUSBSETUPNOTIFIER pfn,
                                    void *pUser);

Parameters:
   po: [in]: the interface pointer

   pfn: [in]: if non-NULL, indicates the function to be called when the
      device receives a vendor-specific request.  If NULL indicates that
      the previous callback should be cancelled.  Setting a new callback
      function implicitly overwrites the previous setting.

Return Value:
   None

Comments:
   None

See Also:
   IUSBDRIVER_SendSetupResponse
   IUSBDRIVER_ReadCtrlData

=============================================================================

IUSBDRIVER_SendSetupResponse

Description:
   Allows a BREW applet to return a response to a previously issued
   USB vendor-specific request.  Function is only defined to be called from
   within the function specified in IUSBDRIVER_SetSetupNotifier().

   If the setup message that is being responded to has a host to device data
   phase the reponse should be an integer with the value of SUCCESS if the 
   message should be acknowledged otherwise the reponse will be stalled.

Prototype:
   int IUSBDRIVER_SendSetupResponse(IUSBDriver *po, const byte *pResponse,
                                    int nSize);

Parameters:
   po: [in]: the interface pointer

   pResponse: [in]: pointer to buffer containing user-defined response

   nSize: [in]: size of buffer pointed at by pResponse

Return Value:
   SUCCESS, if the message was successfuly sent to the host
   EBADPARM, if the function was called with invalid arguments
   EBADSTATE, if the response was sent from outside the user
      callback function specified in IUSBDRIVER_SetSetupNotifier or the
      response was not returned in a sufficiently short period of time.
   EFAILED, otherwise

Comments:
   This function is time critical.  Applications needing to do anything more
   than simple processing should try to anticipate the query.

See Also:
   IUSBDRIVER_SetSetupNotifier

=============================================================================

IUSBDRIVER_ReadCtrlData

Description:
   Allows a BREW applet to retrieve the data from the data phase of a control 
   channel message the contains a host to device transfer. Function is only 
   defined to be called from within the function specified in 
   IUSBDRIVER_SetSetupNotifier().

Prototype:
   int IUSBDRIVER_ReadCtrlData(IUSBDriver *po, const byte *pData, int nSize);

Parameters:
   po: [in]: the interface pointer

   pData: [out]: pointer to buffer that will contain the data.

   nSize: [in]: size of buffer pointed at by pData

Return Value:
   SUCCESS, if the data was successfuly retrieved
   EBADPARM, if the function was called with invalid arguments
   EBADSTATE, if the function was called from outside the user
      callback function specified in IUSBDRIVER_SetSetupNotifier.
   EFAILED, otherwise

Comments:
   None.

See Also:
   IUSBDRIVER_SetSetupNotifier
   IUSBDRIVER_SendSetupResponse

=============================================================================
*/
#endif /* AEEUSBDRIVER_H */

