#ifndef AEEIBTA2DP_H
#define AEEIBTA2DP_H

/*=============================================================================
FILE:         AEEIBTA2DP.h

SERVICES:     Bluetooth Advanced Audio Distribution Profile interface

DESCRIPTION:  This file contains the interface definition and data structures
              for the Bluetooth A2DP interface.
              Constants and definitions in the file refer to those
              defined in A2DP spec v1.2

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEEIQI.h"
#include "AEEStdDef.h"
#include "AEEBTDef.h"


/*=============================================================================
 Interface ID
=============================================================================*/
#define AEEIID_IBTA2DP    0x01061c2b

/*=============================================================================
   A2DP Configuration Commands
==============================================================================*/

#define AEEA2DP_CFG_SUSPEND_ON_END   0x0001
#define AEEA2DP_CFG_CLOSE_ON_END     0x0002
#define AEEA2DP_CFG_RECFG_BY_RECFG   0x0003
#define AEEA2DP_CFG_RECFG_BY_CLOSE   0x0004
#define AEEA2DP_CFG_AUTO_BITRATE     0x0005
#define AEEA2DP_CFG_INCR_BITRATE     0x0006
#define AEEA2DP_CFG_DECR_BITRATE     0x0007
#define AEEA2DP_CFG_LOWEST_BITRATE   0x0008

// A data type to refer to A2DP Cofiguration
typedef uint32    BTA2DPConfig;


/*============================================================================
 Bluetooth A2DP Interface Definition
============================================================================*/

#define INHERIT_IBTA2DP(iname)                                    \
   INHERIT_IQI(iname);                                            \
   int (*Enable)         (iname *p, BTSecurityLevel sec);         \
   int (*Disable)        (iname *p);                              \
   int (*SetDevice)      (iname *p, const BDAddress *pBDAddr);    \
   int (*Connect)        (iname *p, const BDAddress *pBDAddr);    \
   int (*Disconnect)     (iname *p, const BDAddress *pBDAddr);    \
   int (*Configure)      (iname *p,                               \
                          const BDAddress *pBDAddr,               \
                          BTA2DPConfig    cfg,                    \
                          const char      *pszConfig);            \
   int (*GetConfig)      (iname           *p,                     \
                          const BDAddress *pBDAddr,               \
                          char            *pszConfig,             \
                          int             nlen,                   \
                          int             *pnReqLen)
AEEINTERFACE_DEFINE(IBTA2DP);

/*============================================================================
 Interface Accessor Functions
============================================================================*/

static __inline uint32 IBTA2DP_AddRef(IBTA2DP *p)
{
   return AEEGETPVTBL((p),IBTA2DP)->AddRef(p);
}

static __inline uint32 IBTA2DP_Release(IBTA2DP *p)
{
   return AEEGETPVTBL((p),IBTA2DP)->Release(p);
}

static __inline int IBTA2DP_QueryInterface(IBTA2DP *p,
                                           AEEIID idReq, void **ppo)
{
   return AEEGETPVTBL((p),IBTA2DP)->QueryInterface(p, idReq, ppo);
}

static __inline int IBTA2DP_Enable(IBTA2DP *p, BTSecurityLevel sec)
{
   return AEEGETPVTBL((p),IBTA2DP)->Enable(p,sec);
}

static __inline int IBTA2DP_Disable(IBTA2DP *p)
{
   return AEEGETPVTBL((p),IBTA2DP)->Disable(p);
}

static __inline int IBTA2DP_Connect(IBTA2DP *p, const BDAddress *pBDAddr)
{
   return AEEGETPVTBL((p),IBTA2DP)->Connect(p,pBDAddr);
}

static __inline int IBTA2DP_Disconnect(IBTA2DP *p, const BDAddress *pBDAddr)
{
   return AEEGETPVTBL((p),IBTA2DP)->Disconnect(p, pBDAddr);
}

static __inline int IBTA2DP_SetDevice(IBTA2DP *p, const BDAddress *pBDAddr)
{
   return AEEGETPVTBL((p),IBTA2DP)->SetDevice(p,pBDAddr);
}

static __inline int IBTA2DP_Configure(IBTA2DP *p,
                                      const BDAddress *pBDAddr,
                                      BTA2DPConfig config,
                                      const char *pszConfig)
{
   return AEEGETPVTBL((p),IBTA2DP)->Configure(p, pBDAddr, config, pszConfig);
} 

static __inline int IBTA2DP_GetConfig(IBTA2DP   *p,
                                      const BDAddress *pBDAddr,
                                      char      *pszConfig,
                                      int       nlen,
                                      int       *pnReqLen)
{
   return AEEGETPVTBL((p),IBTA2DP)->GetConfig(p, pBDAddr, pszConfig, nlen, pnReqLen);
}
/*=============================================================================
  DATA TYPES DOCUMENTATION
===============================================================================
BTA2DPConfig

Definition
   typedef uint32    BTA2DPConfig;

Description
   A data type to refer to A2DP configure commands

   AEEA2DP_CFG_SUSPEND_ON_END     :Send AVDTP_SUSPEND when streaming ends.
                                   This is the default setting.
   AEEA2DP_CFG_CLOSE_ON_END       :Send AVDTP_CLOSE when streaming ends.
   AEEA2DP_CFG_RECFG_BY_RECFG     :Use AVDTP_RECONFIGURE to reconfigure streams.
   AEEA2DP_CFG_RECFG_BY_CLOSE     :Use AVDTP_CLOSE to reconfigure streams.
                                   This is the default setting.
   AEEA2DP_CFG_INCR_BITRATE       :Request sound subsystem to increase bitrate.
                                   Disables auto bitrate selection.
   AEEA2DP_CFG_DECR_BITRATE       :Request sound subsystem to decrease bitrate.
                                   Disables auto bitrate selection.
   AEEA2DP_CFG_AUTO_BITRATE       :Return to auto bitrate selection.
                                   This is the default setting.
   AEEA2DP_CFG_LOWEST_BITRATE     :Sets the lowest bitrate that AUTO_BITRATE will use.
                                   Enables auto bitrate selection

===============================================================================
   INTERFACE DOCUMENTATION
===============================================================================
Interface Name: IBTA2DP 

Description: BlueTooth Advanced Audio Distribution Profile

   This interface provides methods to control the Audio distribution
   Engine of the underlying Bluetooth driver.The interface supports Bluetooth
   Adavnced Audio Distribution Profile in the Source role only.

   To receive notifications about change in state of the engine and connections,
   the application should register with the IBTA2DPNotifier interface.

==============================================================================

IBTA2DP_AddRef()

Description:
   Inherited from IQI_AddRef().

See Also:
   IBTA2DP_Release()

==============================================================================

IBTA2DP_Release()

Description:
   Inherited from IQI_Release().

See Also:
   IBTA2DP_AddRef()
   
==============================================================================

IBTA2DP_QueryInterface()

Description:
   Inherited from IQI_QueryInterface().
   It can be used to
     -  Get a pointer to an available interface based on the input interface ID

Prototype:
   int IBTA2DP_QueryInterface (IBTA2DP* po, AEEIID clsReq, 
                               void** ppo);

Parameters:
   po [i]     : the object
   clsReq [i] : interface ID of the interface to query.
   ppo [o]    : place holder for pointer to the interface.

Return Value:
   AEE_SUCCESS: on success, 
   Otherwise: an error code.

Comments:
   On return of the API, if content of ppo is NULL, then the queried 
   interface is not available.

Side Effects:
   If an interface is retrieved, then this function increments its 
   reference count.

===============================================================================

Function: IBTA2DP_Enable

Description:
   Calling this API causes the Bluetooth device to be connectable.
   It will now scan for connections.

Prototype:
   int IBTA2DP_Enable(IBTA2DP         *p,
                      BTSecurityLevel sec);

Parameters
   p [i]           : pointer to the IBTA2DP object
   sec [i]         : Bluetooth service security level

Return Value:
   AEE_SUCCESS     : the request was queued.The signal registered with the 
                     IBTA2DPNotifier interface will be trigerred when operation
                     completes. The application should call IBTA2DPNotifier_GetEvent()
                     to retrieve the event.
   AEE_EBADSTATE   : bad state

See Also:
   BTSecurityLevel
   IBTA2DP_Disable()

Event(s):
   AEEBTA2DP_EVT_ENABLED : If the Bluetooth Audio Source is enabled
   AEEBTA2DP_EVT_ENABLE_FAILED : If the Bluetooth Audio Source could not be 
                                 enabled

===============================================================================

Function: IBTA2DP_Connect

Description:
   Connects to a Bluetooth Audio sink device.

Prototype:
   int IBTA2DP_Connect(IBTA2DP   *p,
                       const BDAddress *pBDAddr);
Parameters
   p [i]           : pointer to the IBTA2DP object
   pBDAddr [i]     : Bluetooth device address of the sink

Return Value:
   AEE_SUCCESS     : the connect request was queued.The signal registered with 
                     the IBTA2DPNotifier interface will be trigerred when the 
                     connection procedure completes.The application should call
                     IBTA2DPNotifier_GetEvent()to retrieve the event.
   AEE_EBADPARM    : invalid parameters
   AEE_EBADSTATE   : A2DP profile not enabled
   AEE_EITEMBUSY   : another operation is in progress
   AEE_ENOMORE     : cannot support another connection

See Also:
   BDAddress
   IBTA2DP_Disconnect()

Event(s):
   AEEBTA2DP_EVT_CONNECTED if connection to the sink is successful
   AEEBTA2DP_EVT_CON_FAILED if connection fails

=======================================================================

Function: IBTA2DP_Disconnect

Description:
   Disconnects from the specified A2DP sink device.

Prototype:
   int IBTA2DP_Disconnect(IBTA2DP   *p,
                          const BDAddress *pBDAddr);

Parameters
   p [i]           : pointer to the IBTA2DP object

Return Value:
   AEE_SUCCESS     : the request was queued.The signal registered with 
                     the IBTA2DPNotifier interface will be trigerred when the 
                     disconnection procedure completes.The application should
                     call IBTA2DPNotifier_GetEvent()to retrieve the event
   AEE_EBADPARM    : input pointers are invalid
   AEE_EBADSTATE   : source is not connected to the specified sink
   AEE_EITEMBUSY   : another operation is in progress

See Also:
   IBTA2DP_Connect()

Event(s) :
   AEEBTA2DP_EVT_DISCONNECTED : When disconnection procedure completes

===============================================================================

Function: IBTA2DP_Disable

Description:
   The Bluetooth device ceases to be connectable.Existing connections
   between phone and audio devices will be brought down as a result.

Prototype:

   int IBTA2DP_Disable(IBTA2DP   *p);

Parameters
   p [i]           : pointer to the IBTA2DP object

Return Value:
   SUCCESS         : the request was queued.The signal registered with 
                     the IBTA2DPNotifier interface will be trigerred when the 
                     procedure completes.The application should call 
                     IBTA2DPNotifier_GetEvent()to retrieve the event
   AEE_EBADSTATE   : not enabled
   AEE_EITEMBUSY   : another operation is in progress

See Also:
   IBTA2DP_Enable()

Events:
   AEEBTA2DP_EVT_DISABLED: when the audio engine is disabled

===============================================================================

Function: IBTA2DP_Configure()

Description:
   Used for fine grained control of audio engine, mostly useful for 
   interop with devices.Configuration data is not expected
   to be retained by the interface after the object is released, or across
   power cycles.This API may be called multiple times. For example, if the required
   configuration is to send a 'close' on end and reconfigure by re page,
   the API would be invoked twice, once with cfg as AEEA2DP_CFG_CLOSE_ON_END followed
   by cfg = AEEA2DP_CFG_RECFG_BY_REPAGE.

Prototype:

  int IBTA2DP_Configure(IBTA2DP           *p,
                        const BDAddress   *pBDAddr,
                        BTA2DPConfig      cfg,
                        const char        *pszConfig);

Parameters:
   p [i]            : pointer to the IBTA2DP object
   pBDAddr [i]      : Bluetooth device address. If NULL, this configuration will
                      apply to all devices and will override any existing
                      device specific configuration.
   cfg [i]          : A2DP configuration 
   pszConfig [i]    : configuration string consisting of name value pairs separated by 
                      a delimiter.This parameter is for future use. E.g name value pair 
                      "bit_rate=324"

Return value:

   AEE_SUCCESS      : indicates that the configuration was set
   AEE_EUNSUPPORTED : configuration not supported
   AEE_EBADSTATE    : specified device not currently connected

See Also:
   BTA2DPConfig
   BDAddress

===============================================================================

Function: IBTA2DP_SetDevice

Description:
   This API sets the specified device as the preferred device.
   If Bluetooth audio is the current sound device and there is no connection 
   to an audio sink, a connection will be initiated to the device when 
   streaming begins.

Prototype:
   int IBTA2DP_SetDevice(IBTA2DP         *p,
                         const BDAddress *pBDAddr);

Parameters
   p [i]           : pointer to the IBTA2DP object
   pBDAddr [i]     : bluetooth device address of the sink

Return Value:
   AEE_SUCCESS     : the device was set as preferred device.
   AEE_EBADPARM    : input pointers are invalid
   AEE_EBADSTATE   : currently connected to a device

See Also:
   BDAddress

===============================================================================

Function: IBTA2DP_GetConfig()

Description:
   This API gets the currently used configuration parameters for the specified
   device.It may include capabilities advertised by the sink.
   If BD address is supplied by the application,the source should be currently 
   connected to the specified sink.If BD address isn’t supplied,
   the default configuration settings for all devices would be returned by 
   the API.

Prototype:
   int IBTA2DP_GetConfig(IBTA2DP         *p,
                         const BDAddress *pBDAddr,
                         char            *pszConfig,
                         int             nlen,
                         int             *pnReqLen)

Parameters
   p [i]           : pointer to the IBTA2DP object
   pBDAddr [i]     : bluetooth device address of the sink
   pszConfig [o]   : configuration string consisting of name value pairs
                     separated by a delimiter.For E.g "bit_rate=324"
   nlen [i]        : length of buffer allocated for the configuration string
   pnReqLen [o]    : length of buffer required to return configuration string 

Return Value:
   AEE_SUCCESS         : the configuration string was returned successfully
   AEE_EBADPARM        : input pointers are invalid
   AEE_EUNSUPPORTED    : reading configuration string not supported
   AEE_EBUFFERTOOSMALL : buffer allocated for out put string insufficient.
                         pnReqLen will contain the required buffer length.
   AEE_EBADSTATE       : source currently not connected to the specified sink

See Also:
   BDAddress

=================================================================================*/
#endif /* _AEEIBTA2DP_H_ */
