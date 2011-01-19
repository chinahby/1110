/*=============================================================================
FILE:         AEEIRemoteControl_Target.h

SERVICES:     Remote Control Target interface

DESCRIPTION:  The Remote Control Target allows appliciatons (Media Players
              in particular) to be controled by remote devices, such as
              Bluetooth remote control devices.
              This interface depends on one or more control interfaces
              being enabled in the system (such as AEEBT_IAVRCPTarget
              for Bluetooth).

===============================================================================
        Copyright © 2007 Qualcomm Incorporated.
               All Rights Reserved.
            Qualcomm Proprietary/GTDR
=============================================================================*/

#ifndef AEEIREMOTECONTROL_TARGET_H
#define AEEIREMOTECONTROL_TARGET_H

/*============================================================================
 * Includes
 *============================================================================*/
#include "AEEStdDef.h"
#include "AEEInterface.h"
#include "AEEIQI.h"
#include "AEEISignal.h"
#include "AEE64structs.h"

#ifndef _AEERESULT_DEFINED
typedef int                AEEResult;
#define _AEERESULT_DEFINED
#endif

/*============================================================================
 * Interface ID
 *============================================================================*/
#define AEEIID_IRemoteControl_Target 0x1062245

/*============================================================================
 * Types and Definitions
 *============================================================================*/
#define AEERemoteControl_OP_SELECT              0x0000000000000001ull
#define AEERemoteControl_OP_UP                  0x0000000000000002ull
#define AEERemoteControl_OP_DOWN                0x0000000000000004ull
#define AEERemoteControl_OP_LEFT                0x0000000000000008ull
#define AEERemoteControl_OP_RIGHT               0x0000000000000010ull
#define AEERemoteControl_OP_RIGHT_UP            0x0000000000000020ull
#define AEERemoteControl_OP_RIGHT_DOWN          0x0000000000000040ull
#define AEERemoteControl_OP_LEFT_UP             0x0000000000000080ull
#define AEERemoteControl_OP_LEFT_DOWN           0x0000000000000100ull
#define AEERemoteControl_OP_ROOT_MENU           0x0000000000000200ull
#define AEERemoteControl_OP_SETUP_MENU          0x0000000000000400ull
#define AEERemoteControl_OP_CONTENTS_MENU       0x0000000000000800ull
#define AEERemoteControl_OP_FAVORITE_MENU       0x0000000000001000ull
#define AEERemoteControl_OP_EXIT                0x0000000000002000ull
#define AEERemoteControl_OP_0                   0x0000000000004000ull
#define AEERemoteControl_OP_1                   0x0000000000008000ull
#define AEERemoteControl_OP_2                   0x0000000000010000ull
#define AEERemoteControl_OP_3                   0x0000000000020000ull
#define AEERemoteControl_OP_4                   0x0000000000040000ull
#define AEERemoteControl_OP_5                   0x0000000000080000ull
#define AEERemoteControl_OP_6                   0x0000000000100000ull
#define AEERemoteControl_OP_7                   0x0000000000200000ull
#define AEERemoteControl_OP_8                   0x0000000000400000ull
#define AEERemoteControl_OP_9                   0x0000000000800000ull
#define AEERemoteControl_OP_DOT                 0x0000000001000000ull
#define AEERemoteControl_OP_ENTER               0x0000000002000000ull
#define AEERemoteControl_OP_CLEAR               0x0000000004000000ull
#define AEERemoteControl_OP_CHANNEL_UP          0x0000000008000000ull
#define AEERemoteControl_OP_CHANNEL_DOWN        0x0000000010000000ull
#define AEERemoteControl_OP_PREVIOUS_CHANNEL    0x0000000020000000ull
#define AEERemoteControl_OP_SOUND_SELECT        0x0000000040000000ull
#define AEERemoteControl_OP_INPUT_SELECT        0x0000000080000000ull
#define AEERemoteControl_OP_DISPLAY_INFORMATION 0x0000000100000000ull
#define AEERemoteControl_OP_HELP                0x0000000200000000ull
#define AEERemoteControl_OP_PAGE_UP             0x0000000400000000ull
#define AEERemoteControl_OP_PAGE_DOWN           0x0000000800000000ull
#define AEERemoteControl_OP_POWER               0x0000001000000000ull
#define AEERemoteControl_OP_VOLUME_UP           0x0000002000000000ull
#define AEERemoteControl_OP_VOLUME_DOWN         0x0000004000000000ull
#define AEERemoteControl_OP_MUTE                0x0000008000000000ull
#define AEERemoteControl_OP_PLAY                0x0000010000000000ull
#define AEERemoteControl_OP_STOP                0x0000020000000000ull
#define AEERemoteControl_OP_PAUSE               0x0000040000000000ull
#define AEERemoteControl_OP_RECORD              0x0000080000000000ull
#define AEERemoteControl_OP_REWIND              0x0000100000000000ull
#define AEERemoteControl_OP_FAST_FORWARD        0x0000200000000000ull
#define AEERemoteControl_OP_EJECT               0x0000400000000000ull
#define AEERemoteControl_OP_FORWARD             0x0000800000000000ull
#define AEERemoteControl_OP_BACKWARD            0x0001000000000000ull
#define AEERemoteControl_OP_ANGLE               0x0002000000000000ull
#define AEERemoteControl_OP_SUBPICTURE          0x0004000000000000ull
#define AEERemoteControl_OP_F1                  0x0008000000000000ull
#define AEERemoteControl_OP_F2                  0x0010000000000000ull
#define AEERemoteControl_OP_F3                  0x0020000000000000ull
#define AEERemoteControl_OP_F4                  0x0040000000000000ull
#define AEERemoteControl_OP_F5                  0x0080000000000000ull

enum AEERemoteControl_OpDirection
{
    AEERemoteControl_OP_PRESSED,
    AEERemoteControl_OP_RELEASED,
    _QIDL_PLACEHOLDER_AEERemoteControl_OpDirection = 0x7fffffff
};
typedef enum AEERemoteControl_OpDirection AEERemoteControl_OpDirection;

typedef struct AEERemoteControl_CommandType AEERemoteControl_CommandType;
struct AEERemoteControl_CommandType
{
    uint64struct                 Type;
    AEERemoteControl_OpDirection Direction;
};

/*============================================================================
 * Transport IDs
 *============================================================================*/
#define AEERemoteControl_TRANS_BT_AVRCP_1_0 0x1062246
#define AEERemoteControl_TRANS_BT_AVRCP_1_3 0x1062247
#define AEERemoteControl_TRANS_BT_AVRCP_1_4 0x1062248
typedef AEEUID AEERemoteControl_Transport;

#ifndef __CS_TYPE_AEEREMOTECONTROL___SEQ_UNSIGNED_LONG__
#define __CS_TYPE_AEEREMOTECONTROL___SEQ_UNSIGNED_LONG__
typedef struct AEERemoteControl___seq_unsigned_long AEERemoteControl___seq_unsigned_long;

struct AEERemoteControl___seq_unsigned_long
{
    AEERemoteControl_Transport* data;
    int dataLen;
    int dataLenReq;
};

#endif /* __CS_TYPE_AEEREMOTECONTROL___SEQ_UNSIGNED_LONG__ */

typedef AEERemoteControl___seq_unsigned_long AEERemoteControl_Transports;

typedef struct AEERemoteControl_VendorCommandType AEERemoteControl_VendorCommandType;

struct AEERemoteControl_VendorCommandType
{
    AEERemoteControl_Transport TransportID;
    char* VendorData;
    int VendorDataLen;
    int VendorDataLenReq;
};

#define INHERIT_IRemoteControl_Target(iname) \
    INHERIT_IQI(iname); \
    AEEResult (*RegCommandMask)(iname* _me, /*in*/ const uint64struct* CommandMask); \
    AEEResult (*OnCommandNotify)(iname* _me, /*in*/ ISignal* Sig); \
    AEEResult (*GetCommand)(iname* _me, /*out*/ AEERemoteControl_CommandType* Command); \
    AEEResult (*SetFocus)(iname* _me); \
    AEEResult (*GetSupportedTransports)(iname* _me, /*out*/ AEERemoteControl_Transport* TransportIDList, /*in*/ int TransportIDListLen, /*out*/ int* TransportIDListLenReq); \
    AEEResult (*RegVendorID)(iname* _me, /*in*/ const char* VendorID, /*in*/ AEERemoteControl_Transport TransportID); \
    AEEResult (*OnVendorCommandNotify)(iname* _me, /*in*/ ISignal* Sig); \
    AEEResult (*GetVendorCommand)(iname* _me, /*out*/ AEERemoteControl_VendorCommandType* Command); \
    AEEResult (*ReplyVendorCommand)(iname* _me, /*in*/ const AEERemoteControl_VendorCommandType* Command); \
    AEEResult (*RegExtension)(iname* _me, /*in*/ IQI* Extension)

AEEINTERFACE_DEFINE(IRemoteControl_Target);

static __inline unsigned int IRemoteControl_Target_AddRef(IRemoteControl_Target* _me)
{
    return AEEGETPVTBL(_me,IRemoteControl_Target)->AddRef(_me);
}

static __inline unsigned int IRemoteControl_Target_Release(IRemoteControl_Target* _me)
{
    return AEEGETPVTBL(_me,IRemoteControl_Target)->Release(_me);
}

static __inline int IRemoteControl_Target_QueryInterface(IRemoteControl_Target* _me, AEEIID cls, void** ppo)
{
    return AEEGETPVTBL(_me,IRemoteControl_Target)->QueryInterface(_me, cls, ppo);
}

static __inline AEEResult IRemoteControl_Target_RegCommandMask(IRemoteControl_Target* _me, /*in*/ const uint64struct* CommandMask)
{
    return AEEGETPVTBL(_me,IRemoteControl_Target)->RegCommandMask(_me, CommandMask);
}

static __inline AEEResult IRemoteControl_Target_OnCommandNotify(IRemoteControl_Target* _me, /*in*/ ISignal* Sig)
{
    return AEEGETPVTBL(_me,IRemoteControl_Target)->OnCommandNotify(_me, Sig);
}

static __inline AEEResult IRemoteControl_Target_GetCommand(IRemoteControl_Target* _me, /*out*/ AEERemoteControl_CommandType* Command)
{
    return AEEGETPVTBL(_me,IRemoteControl_Target)->GetCommand(_me, Command);
}

static __inline AEEResult IRemoteControl_Target_SetFocus(IRemoteControl_Target* _me)
{
    return AEEGETPVTBL(_me,IRemoteControl_Target)->SetFocus(_me);
}

static __inline AEEResult IRemoteControl_Target_GetSupportedTransports(IRemoteControl_Target* _me, /*out*/ AEERemoteControl_Transport* TransportIDList, /*in*/ int TransportIDListLen, /*out*/ int* TransportIDListLenReq)
{
    return AEEGETPVTBL(_me,IRemoteControl_Target)->GetSupportedTransports(_me, TransportIDList, TransportIDListLen, TransportIDListLenReq);
}

static __inline AEEResult IRemoteControl_Target_RegVendorID(IRemoteControl_Target* _me, /*in*/ const char* VendorID, /*in*/ AEERemoteControl_Transport TransportID)
{
    return AEEGETPVTBL(_me,IRemoteControl_Target)->RegVendorID(_me, VendorID, TransportID);
}

static __inline AEEResult IRemoteControl_Target_OnVendorCommandNotify(IRemoteControl_Target* _me, /*in*/ ISignal* Sig)
{
    return AEEGETPVTBL(_me,IRemoteControl_Target)->OnVendorCommandNotify(_me, Sig);
}

static __inline AEEResult IRemoteControl_Target_GetVendorCommand(IRemoteControl_Target* _me, /*out*/ AEERemoteControl_VendorCommandType* Command)
{
    return AEEGETPVTBL(_me,IRemoteControl_Target)->GetVendorCommand(_me, Command);
}

static __inline AEEResult IRemoteControl_Target_ReplyVendorCommand(IRemoteControl_Target* _me, /*in*/ const AEERemoteControl_VendorCommandType* Command)
{
    return AEEGETPVTBL(_me,IRemoteControl_Target)->ReplyVendorCommand(_me, Command);
}

static __inline AEEResult IRemoteControl_Target_RegExtension(IRemoteControl_Target* _me, /*in*/ IQI* Extension)
{
    return AEEGETPVTBL(_me,IRemoteControl_Target)->RegExtension(_me, Extension);
}


/*=============================================================================
    INTERFACE DOCUMENTATION
===============================================================================

Interface Name: IRemoteControl_Target

Description: Allows applications to be controlled via a remote control device

    The IRemoteControl_Target allows remote controllers to have access to
    applications on the target. This interface is primarily intended to allow
    controll over Media Player like applications, but can be applied to any
    application.
    This interface relies on another transport mechanism to be enabled. The
    internal transport will be responible for directing commands from the remote
    device through this interface.


==============================================================================

IRemoteControl_Target_AddRef()

Description:
   Inherited from IQI_AddRef().

See Also:
   IRemoteControl_Target_Release()

==============================================================================

IRemoteControl_Target_Release()

Description:
   Inherited from IQI_Release().

See Also:
   IRemoteControl_Target_AddRef()
   
==============================================================================

IRemoteControl_Target_QueryInterface()

Description:
   Inherited from IQI_QueryInterface().

===============================================================================

Function: IRemoteControl_Target_RegCommandMask

Description:
   Registers the set of commands the application handles. This is a bitmask of
   the AEERemoteControl_OP_* types supported. The signal registered via 
   IRemoteControl_Target_OnCommandNotify() will only be set for operations in
   this command mask. All operations not in the command mask will be hidden
   from the application.

Prototype:
   AEEResult IRemoteControl_Target_RegCommandMask(IRemoteControl_Target* _me,
                                                  const uint64struct* CommandMask )

Parameters:
   _me [i]         : The object
   CommandMask [i] : The bit-mask of AEERemoteControl_OP commands supported 
                     by the instantiator.

Return Values:
   AEE_SUCCESS: The mask has been registered.

See Also:

===============================================================================

Function: IRemoteControl_Target_OnCommandNotify

Description:
   Registers a signal handler with the interface. This signal will be set
   when a basic command has been received from a remote controller. Only
   commands registered via IRemoteControl_Target_RegCommandMask will generate
   signals; all other commands received from the controller will not
   set the signal registered here.

Prototype:
   AEEResult IRemoteControl_Target_OnCommandNotify(IRemoteControl_Target* _me,
                                                   ISignal* Sig)

Parameters:
   _me [i] : The object
   Sig [i] : Signal to be set upon receipt of a basic command.

Return Values:
   AEE_SUCCESS   : Signal handler has been registered
   AEE_EBADPARAM : Invalid parameters.

See Also:

===============================================================================

Function: IRemoteControl_Target_GetCommand

Description:
   Retrieves the next command sent by the remote controller.

Prototype:
   int IRemoteControl_Target_GetCommand(IRemoteControl_Target* _me, 
                                        AEERemoteControl_CommandType* Command)

Parameters:
   _me [i]     : The object
   Command [o] : Contains the next command sent by the remote controller.

Return Values:
   AEE_SUCCESS : Command returned
   AEE_ENOMORE : No more commands available

See Also:

===============================================================================

Function: IRemoteControl_Target_SetFocus

Description:
   Sets the focus for directing remote control events. The object which
   most recently called IRemoteControl_Target_SetFocus should receive
   all basic (ie non-Vendor) commands. An application should call SetFocus
   when it has user interaction, or has some other indication that it should
   be the target of Remote Control commands.

   Note: some implementations may support sending commands to objects
   which are not "in focus".
   Note 2: this is separate from UI focus. An appliaction need not be
   in the foreground to receive commands.

Prototype:
   AEEResult IRemoteControl_Target_SetFocus(IRemoteControl_Target* _me)

Parameters:
   _me [i] : The object

Return Values:
   AEE_SUCCESS : Focus Set

See Also:

===============================================================================

Function: IRemoteControl_Target_GetSupportedTransports

Description:
   This function returns a list of available implementation-dependant transports
   which can be used by the application. The format of this list shall be defined
   in the implementation specfic documentation.

Prototype:
   AEEResult IRemoteControl_Target_GetSupportedTransports(IRemoteControl_Target* _me,
                                                          AEERemoteControl_Transport* TransportIDList, 
                                                          int TransportIDListLen, 
                                                          int* TransportIDListLenReq)

Parameters:
   _me [i]                   : The object
   TransportIDList [o]       : sequence of Transport types containing implementation
                               defined transports.
   TransportIDListLen [i]    : The size allocated by the application for the TransportIDList
   TransportIDListLenReq [o] : The size needed to contain the full TransportIDList

Return Values:
   AEE_SUCCESS : Transports have been returned successfully
   AEE_EBUFFERTOOSMALL : the number of elements in TransportIDList array is insufficient

See Also:

===============================================================================

Function: IRemoteControl_Target_RegVendorID

Description:
   This registers a vendor specific ID with the interface via a particular
   transport. This vendor ID allows specific vendor unique commands to be
   conveyed over the transport and directed to the registered application.

Prototype:
   AEEResult IRemoteControl_Target_RegVendorID(IRemoteControl_Target* _me,
                                               const char* VendorID,
                                               AEERemoteControl_Transport TransportID)

Parameters:
   _me [i]        : The object
   VendorID [i]   : Idendification for this vendor, as defined in the
                    implementation and transport.
   TransportID[i] : The UID of the transport for which this VendorID
                    applies.

Return Values:
   AEE_SUCCESS : The registration was successful
   AEE_EALREADY : A registration for this VendorID already exists

See Also:

===============================================================================

Function: IRemoteControl_Target_OnVendorCommandNotify

Description:
   Registers a signal to be set upon receipt of a vendor-specific command with
   an ID previously registered with IRemoteControl_RegVendorID().

Prototype:
   AEEResult IRemoteControl_Target_OnVendorCommandNotify(IRemoteControl_Target* _me,
                                                         ISignal* Sig)

Parameters:
   _me [i] : The object
   Sig [i] : Signal to be set.

Return Values:
   AEE_SUCCESS : The registration was successful

See Also:
   IRemoteControl_RegVendorID()

===============================================================================

Function: IRemoteControl_Target_GetVendorCommand

Description:
   Retreives a vendor command sent by the remote controller.

Prototype:
    int IRemoteControl_Target_GetVendorCommand(IRemoteControl_Target* _me,
                                               AEERemoteControl_VendorCommandType* Command)

Parameters:
   _me [i]           : The object
   VendorCommand [o] : The implementation and transport defined string containing
                       the command sent by the remote controller.

Return Values:
   AEE_SUCCESS : The command was successfully retreived.
   AEE_ENOMORE : No more commands are ready to be received
   AEE_EBUFFERTOOSMALL : The buffer is too small to retreive the command.

See Also:

===============================================================================

Function: IRemoteControl_Target_ReplyVendorCommand

Description:
   This should be used to send a reply to a vendor command to the remote
   controller. The format of the reply is dependant on the class implementation
   and the transport used.

Prototype:
   int IRemoteControl_Target_ReplyVendorCommand(IRemoteControl_Target* _me,
                                                const AEERemoteControl_VendorCommandType* Command)

Parameters:
   _me [i]           : The object
   VendorCommand [i] : string representation of the reply.

Return Values:
   AEE_SUCCESS : The reply has been queued for transport

See Also:

===============================================================================

Function: IRemoteControl_Target_RegExtension

Description:
   This is used to register an extension to the remote control interface to add
   extended control and data capabilities. Planned extensions include the ability
   to inform the controller of the current state, notifications for changes in
   state, metadata for current media, and the ability to browse through objects
   available in the application.
   These extension interfaces will be defined by the implementers of IRemoteControl,
   and the extension interface itself will be implemented by the application and
   used by the IRemoteControl interface. The extensions shall be remotable, and
   the implementation shall release all registered extensions when it's
   reference count reaches zero.

Prototype:
   AEEResult IRemoteControl_Target_RegExtension(IRemoteControl_Target* _me,
                                                IQI* Extension)

Parameters:
   _me [i]       : The object
   Extension [i] : Instantiation of an extension object handled by the implemenatiton.

Return Values:
   AEE_SUCCESS : The extension has been registered
   AEE_ECLASSNOTSUPPORT : The class is not supported by the implemenation.

See Also:

==============================================================================*/
#endif
