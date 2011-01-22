/*=============================================================================
FILE:         AEEIBTAVRCPTarget.h

SERVICES:     Bluetooth AVRCP Target (TG) Interface

DESCRIPTION:  Bluetooth AVRCP enables remote controllers to interact with
              the device via the AEEIBTAVRCPTarget interface.

===============================================================================
        Copyright © 2007 Qualcomm Incorporated.
               All Rights Reserved.
            Qualcomm Proprietary/GTDR
=============================================================================*/

#ifndef AEEIBTAVRCPTARGET_H
#define AEEIBTAVRCPTARGET_H

/*============================================================================
 * Includes
 *============================================================================*/
#include "AEEStdDef.h"

#include "AEEInterface.h"
#include "AEEIQI.h"
#include "AEEBTDef.h"
#include "AEEISignal.h"

#ifndef _AEERESULT_DEFINED
typedef int                AEEResult;
#define _AEERESULT_DEFINED
#endif

/*=============================================================================
 * Interface ID
 *=============================================================================*/
#define AEEIID_IBTAVRCPTarget 0x1062249

/*============================================================================
 * AVRCP Target Interface Definition and types
 *============================================================================*/
enum AEEBTAVRCPEventEnum
{
    AEEBTAVRCP_NONE,
    AEEBTAVRCP_ENABLED,
    AEEBTAVRCP_ENABLE_FAILED,
    AEEBTAVRCP_CONNECTED,
    AEEBTAVRCP_CONN_FAILED,
    AEEBTAVRCP_DISCONNECTED,
    AEEBTAVRCP_DISABLED,
    AEEBTAVRCP_EVENT_Q_OVERFLOW,
    _QIDL_PLACEHOLDER_AEEBTAVRCPEventEnum = 0x7fffffff
};
typedef enum AEEBTAVRCPEventEnum AEEBTAVRCPEventEnum;

typedef struct AEEBTAVRCPEventType AEEBTAVRCPEventType;
struct AEEBTAVRCPEventType
{
    AEEBTAVRCPEventEnum Event;
    BDAddress Address;
    BTResult Result;
};

#define INHERIT_IBTAVRCPTarget(iname) \
    INHERIT_IQI(iname); \
    AEEResult (*Enable)(iname* _me, /*in*/ BTSecurityLevel Security); \
    AEEResult (*Connect)(iname* _me, /*in*/ const BDAddress* Address); \
    AEEResult (*Disconnect)(iname* _me, /*in*/ const BDAddress* Address); \
    AEEResult (*Disable)(iname* _me); \
    AEEResult (*GetStatus)(iname* _me, /*out*/ BTSecurityLevel* Security, /*out*/ boolean* Enabled, /*out*/ BDAddress* ConnectedDevices, /*in*/ int ConnectedDevicesLen, /*out*/ int* ConnectedDevicesLenReq); \
    AEEResult (*OnEventNotify)(iname* _me, /*in*/ ISignal* Sig); \
    AEEResult (*GetEvent)(iname* _me, /*out*/ AEEBTAVRCPEventType* Event)

AEEINTERFACE_DEFINE(IBTAVRCPTarget);

static __inline unsigned int IBTAVRCPTarget_AddRef(IBTAVRCPTarget* _me)
{
    return AEEGETPVTBL(_me,IBTAVRCPTarget)->AddRef(_me);
}

static __inline unsigned int IBTAVRCPTarget_Release(IBTAVRCPTarget* _me)
{
    return AEEGETPVTBL(_me,IBTAVRCPTarget)->Release(_me);
}

static __inline int IBTAVRCPTarget_QueryInterface(IBTAVRCPTarget* _me, AEEIID cls, void** ppo)
{
    return AEEGETPVTBL(_me,IBTAVRCPTarget)->QueryInterface(_me, cls, ppo);
}

static __inline AEEResult IBTAVRCPTarget_Enable(IBTAVRCPTarget* _me, /*in*/ BTSecurityLevel Security)
{
    return AEEGETPVTBL(_me,IBTAVRCPTarget)->Enable(_me, Security);
}

static __inline AEEResult IBTAVRCPTarget_Connect(IBTAVRCPTarget* _me, /*in*/ const BDAddress* Address)
{
    return AEEGETPVTBL(_me,IBTAVRCPTarget)->Connect(_me, Address);
}

static __inline AEEResult IBTAVRCPTarget_Disconnect(IBTAVRCPTarget* _me, /*in*/ const BDAddress* Address)
{
    return AEEGETPVTBL(_me,IBTAVRCPTarget)->Disconnect(_me, Address);
}

static __inline AEEResult IBTAVRCPTarget_Disable(IBTAVRCPTarget* _me)
{
    return AEEGETPVTBL(_me,IBTAVRCPTarget)->Disable(_me);
}

static __inline AEEResult IBTAVRCPTarget_GetStatus(IBTAVRCPTarget* _me, /*out*/ BTSecurityLevel* Security, /*out*/ boolean* Enabled, /*out*/ BDAddress* ConnectedDevices, /*in*/ int ConnectedDevicesLen, /*out*/ int* ConnectedDevicesLenReq)
{
    return AEEGETPVTBL(_me,IBTAVRCPTarget)->GetStatus(_me, Security, Enabled, ConnectedDevices, ConnectedDevicesLen, ConnectedDevicesLenReq);
}

static __inline AEEResult IBTAVRCPTarget_OnEventNotify(IBTAVRCPTarget* _me, /*in*/ ISignal* Sig)
{
    return AEEGETPVTBL(_me,IBTAVRCPTarget)->OnEventNotify(_me, Sig);
}

static __inline AEEResult IBTAVRCPTarget_GetEvent(IBTAVRCPTarget* _me, /*out*/ AEEBTAVRCPEventType* Event)
{
    return AEEGETPVTBL(_me,IBTAVRCPTarget)->GetEvent(_me, Event);
}

#endif

/*=============================================================================
    INTERFACE DOCUMENTATION
===============================================================================

Interface Name: IBTAVRCPTarget

Description: Bluetooth Audio/Visual Remote Control Profile

    This interface enables the Audio Visual Remote Control Profile as defined
    by the Bluetooth SIG.
    This interface only handles the Bluetooth specfic commands of the profile,
    such as connecting/disconnecting. Once this interface is enabled and a
    remote device connected, applications using the IRemoteControl interface
    will receive AVRCP commands from the remote Bluetooth device.

    IBTAVRCPTarget_OnEventNotify() should be called to register a signal that
    will be set when the application is to be notified of an event. The event
    is retrieved by getting the readonly attribute Event. Each event is
    internally queued and presented in order to the application. If too many
    events have been received before the application processes the events,
    the event IBTAVRCP_EVENT_Q_OVERFLOW will be generated.

==============================================================================

IBTAVRCPTarget_AddRef()

Description:
   Inherited from IQI_AddRef().

See Also:
   IBTAVRCPTarget_Release()

==============================================================================

IBTAVRCPTarget_Release()

Description:
   Inherited from IQI_Release().

See Also:
   IBTAVRCPTarget_AddRef()
   
==============================================================================

IBTAVRCPTarget_QueryInterface()

Description:
   Inherited from IQI_QueryInterface().

===============================================================================

Function: IBTAVRCPTarget_Enable

Description:
   This will enable the AVRCP interface, and allow for incoming connections.

Prototype:
   AEEResult IBTAVRCPTarget_Enable( IBTAVRCPTarget* _me, BTSecurityLevel* Security );

Parameters:
   _me [i]     : the object
   Security[i] : Service security level for AVRCP.

Return Values:
   AEE_SUCCESS   : The command was issued successfully, and an AVRCP_ENABLED or
                   AVRCP_ENABLE_FAILED event will be generated at a later time.
   AEE_EALREADY  : The interface is currently proccessing a enable command.
   AEE_EITEMBUSY : The interface is processing another command.
   AEE_EBADSTATE : The interface cannot be enabled (perhaps it already is enabled).

See Also:

===============================================================================

Function: IBTAVRCPTarget_Connect

Description:
   Connects the AVRCP target to a remote Bluetooth AVRCP controller. Note that
   this interface is added for completeness, and is generally not needed. It is
   recommended that the AVRCP controller always initiate the connection.

Prototype:
   AEEResult IBTAVRCPTarget_Connect( IBTAVRCPTarget* _me,
                                     const BDAddress* pAddress );

Parameters:
   _me [i]      : the object
   pAddress [i] : The Bluetooth address of the remote controller.

Return Values:
   AEE_SUCCESS   : The command was issued successfully, and an AVRCP_CONNECTED or
                   AVRCP_CONN_FAILED event will be generated at a later time.
   AEE_EALREADY  : In the process of connecting to a device.
   AEE_EITEMBUSY : The interface is processing another command.

See Also:

===============================================================================

Function: IBTAVRCPTarget_Disconnect

Description:
   Disconnects from a remote AVRCP controller.

Prototype:
   AEEResult IBTAVRCPTarget_Disconnect( IBTAVRCPTarget* _me,
                                        const BDAddress* Address );

Parameters:
   _me [i]     : the object
   Address [i] : The Bluetooth address of the connected remote controller.

Return Values:
   AEE_SUCCESS   : The command was issued, and an AVRCP_DISCONNECTED event
                   will be generated when disconnected.
   AEE_EBADPARAM : Not connected to a device with the specified Bluetooth 
                   address.
   AEE_EALREADY  : The interface is already processing a disconnect command.
   AEE_EITEMBUSY : The interface is processing another command.

See Also:

===============================================================================

Function: IBTAVRCPTarget_OnEventNotify

Description:
   Registers a signal with the interface to be set when an event occurs. Once
   this signal is set, the event may be retrieved by calling 
   IBTAVRCPTarget_GetEvent().

Prototype:
   AEEResult OnEventNotify( IBTAVRCPTarget* _me,
                            ISignal* Sig );

Parameters:
   _me [i] : the object
   Sig [i] : Signal to be set when the event occurs.

Return Values:
   AEE_SUCCESS : The event signal handler was successfully registered.

See Also:

===============================================================================

Function: IBTAVRCPTarget_Disable

Description:
   Disables the AVRCP interface. This will disconnect any existing connections,
   and prevent future incoming connections to AVRCP.

Prototype:
   AEEResult IBTAVRCPTarget_Disable( IBTAVRCPTarget* _me );

Parameters:
   _me [i]    : the object

Return Values:
   AEE_SUCCESS   : The commmand was queued successfully, and an AVRCP_DISABLED event
                   will be generated in the future.
   AEE_EBADSTATE : AVRCP is already disabled.
   AEE_EALREADY  : The interface is already processing a disable command.
   AEE_EITEMBUSY : The interface is processing another command.

See Also:

===============================================================================

Function: IBTAVRCPTarget_GetStatus

Description:
   Gets the current status of AVRCP. This includes whether the driver is enabled,
   and a list of connected devices.

Prototype:
   AEEResult IBTAVRCPTarget_GetStatus(  IBTAVRCPTarget* _me,
                                        BTSecurityLevel* Security,
                                        boolean*         Enabled,
                                        BDAddress*       ConnectedDevices,
                                        int              ConnectedDevicesLen,
                                        int*             ConnectedDevicesLenReq );

Parameters:
   _me                    [i] : the object
   Security               [o] : the security setting if AVRCP is enabled
   Enabled                [o] : TRUE if AVRCP is enabled
   ConnectedDevices       [o] : Array of addresses of connected devices.
   ConnectedDevicesLen    [i] : Elements in the array provided by the
                                application
   ConnectedDevicesLenReq [o] : Number of elements required in the array to
                                return all values

Return Values:
   AEE_SUCCESS : The list has been generated properly.
   AEE_EBADPARM        : bad pointers
   AEE_EBUFFERTOOSMALL : number of elements in 'ConnectedDevices' parameter is too small to hold the 
                         entire list of connected devices.

See Also:

==============================================================================*/
