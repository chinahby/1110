#ifndef AEEIBTCONNECTION_H
#define AEEIBTCONNECTION_H
/*=============================================================================
FILE:         AEEIBTConnection.h

SERVICES:     IBTConnection Interfaces

DESCRIPTION:  IBTConnection is the interface for monitoring and configuring
              Bluetooth connection.

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEEBTDef.h"
#include "AEEISignal.h"

/*=============================================================================
 Interface ID
=============================================================================*/
#define AEEIID_IBTConnection  0x0105a9ad

/*=============================================================================
 BTConnection Operation types
=============================================================================*/
#define AEEBTCONN_OP_NONE                          0
#define AEEBTCONN_OP_SET_ROLE                      1
#define AEEBTCONN_OP_SET_ROLE_SW_POLICY            2
#define AEEBTCONN_OP_SET_SEC                       3
#define AEEBTCONN_OP_SET_LOW_POWER_CONFIG          4

typedef uint32 BTConnOperationType;

/*=============================================================================
 BTConnection Event types
=============================================================================*/
#define AEEBTCONN_EV_NONE                          0
#define AEEBTCONN_EV_STATUS_CHANGED                1
#define AEEBTCONN_EV_SET_ROLE_SUCCESS              2
#define AEEBTCONN_EV_SET_ROLE_FAILED               3
#define AEEBTCONN_EV_SET_ROLE_SW_POLICY_SUCCESS    4
#define AEEBTCONN_EV_SET_ROLE_SW_POLICY_FAILED     5
#define AEEBTCONN_EV_SET_SECURITY_SUCCESS          6
#define AEEBTCONN_EV_SET_SECURITY_FAILED           7
#define AEEBTCONN_EV_SET_LOW_POWER_CONFIG_SUCCESS  8
#define AEEBTCONN_EV_SET_LOW_POWER_CONFIG_FAILED   9

typedef uint32 BTConnEventType;

/*=============================================================================
 Aspects of a connection
=============================================================================*/
#define AEEBT_CONN_STAT_LINK_MODE_B       0x0001
#define AEEBT_CONN_STAT_SECURITY_B        0x0002
#define AEEBT_CONN_STAT_ROLE_B            0x0004
#define AEEBT_CONN_STAT_SCO_PRESENT_B     0x0008
#define AEEBT_CONN_STAT_RSSI_B            0x0010
#define AEEBT_CONN_STAT_ROLE_SW_POLICY_B  0x0020
#define AEEBT_CONN_STAT_ROLE_SWITCH_B     0x0040
#define AEEBT_CONN_STAT_PENDING_OP_B      0x0080

typedef uint32  BTConnStatBitmap;

/*=============================================================================
 Low Power Mode
=============================================================================*/
#define AEEBT_LPM_DEFAULT                    0
#define AEEBT_LPM_ACTIVE                     1
#define AEEBT_LPM_SNIFF                      2
#define AEEBT_LPM_SNIFF_LOCK                 3

typedef uint32 BTLowPowerMode;

/*=============================================================================
 Structures
=============================================================================*/
typedef struct
{
   BTDeviceInfo         about;
   BTConnectionMode     dConnMode;         // active or one of low power modes
   BTSecurityLevel      dSecurity;         // none, auth, auth & encrypt
   boolean              bLocalIsMaster;    // what more can be said?
   boolean              bRoleSwitchAllowed;
   boolean              bSCOPresent;       // is audio conn riding on this conn?
   int8                 RSSI;              // current RSSI level
   BTConnOperationType  dPendingOp;        // pending operation, if any
}  BTConnectionStatus;

/*=============================================================================
                         IBTConnection Interface Definition
=============================================================================*/
#define INHERIT_IBTConnection(iname)  \
   INHERIT_IQI(iname);        \
   int (*OnEventNotify)       (iname*                 po,               \
                               ISignal*               piSignal);        \
   int (*GetEvent)            (iname*                 po,               \
                               BTConnEventType*       pdEv,             \
                               BTResult*              pdResult);        \
   int (*GetStatus)           (iname*                 po,               \
                               BTConnStatBitmap*      pdWhatsValid,     \
                               BTConnectionStatus*    psConnStatus);    \
   int (*SetRoleSwitchPolicy) (iname*                 po,               \
                               boolean                bAllow);          \
   int (*SetRole)             (iname*                 po,               \
                               boolean                bMaster);         \
   int (*SetMinSecurity)      (iname*                 po,               \
                               BTSecurityLevel        dSecLevel);       \
   int (*SetLowPowerConfig)   (iname*                 po,               \
                               BTLowPowerMode         dLowPowerMode,    \
                               uint16                 uMaxIntervalMs,   \
                               uint16                 uIdleTimeoutMs)


AEEINTERFACE_DEFINE(IBTConnection);

static __inline uint32 IBTConnection_AddRef (IBTConnection* p)
{
   return AEEGETPVTBL((p),IBTConnection)->AddRef((p));
}

static __inline uint32 IBTConnection_Release (IBTConnection* p)
{
   return AEEGETPVTBL((p),IBTConnection)->Release((p));
}

static __inline int IBTConnection_QueryInterface (IBTConnection* p,
                                                  AEECLSID clsid,
                                                  void **ppo)
{
   return AEEGETPVTBL((p),IBTConnection)->QueryInterface((p),(clsid),(ppo));
}

static __inline int IBTConnection_OnEventNotify (IBTConnection* p,
                                                 ISignal* piSignal)
{
   return AEEGETPVTBL((p),IBTConnection)->OnEventNotify((p),(piSignal));
}

static __inline int IBTConnection_GetEvent (IBTConnection* p, 
                                            BTConnEventType* pdEv, 
                                            BTResult* pdResult)
{
   return AEEGETPVTBL((p),IBTConnection)->GetEvent((p),(pdEv),(pdResult));
}

static __inline int IBTConnection_GetStatus (IBTConnection* p, 
                                             BTConnStatBitmap* pdBit, 
                                             BTConnectionStatus* psCS)
{
   return AEEGETPVTBL((p),IBTConnection)->GetStatus((p),(pdBit),(psCS));
}

static __inline int IBTConnection_SetRoleSwitchPolicy (IBTConnection* p, 
                                                       boolean bAllowed)
{
   return AEEGETPVTBL((p),IBTConnection)->SetRoleSwitchPolicy((p),(bAllowed));
}

static __inline int IBTConnection_SetRole (IBTConnection* p,
                                           boolean bMaster)
{
   return AEEGETPVTBL((p),IBTConnection)->SetRole((p),(bMaster));
}

static __inline int IBTConnection_SetMinSecurity (IBTConnection* p, 
                                                  BTSecurityLevel dSecLevel)
{
   return AEEGETPVTBL((p),IBTConnection)->SetMinSecurity((p),(dSecLevel));
}

static __inline int IBTConnection_SetLowPowerConfig (IBTConnection* p, 
                                                     BTLowPowerMode dM,
                                                     uint16 uM, uint16 uI)
{
   return AEEGETPVTBL((p),IBTConnection)->SetLowPowerConfig((p),(dM),(uM),(uI));
}

/*=============================================================================
DATA STRUCTURE DOCUMENTATION
===============================================================================
BTConnOperationType

Description:
   This data type specifies the operation that can be performed 
   on an IBTConnection object.
   
Definition:
   #define AEEBTCONN_OP_SET_ROLE                      1
   #define AEEBTCONN_OP_SET_ROLE_SW_POLICY            2
   #define AEEBTCONN_OP_SET_SEC                       3
   typedef uint32 BTConnOperationType;

===============================================================================
BTConnEventType

Description:
   This data type specifies the events that could occur on an IBTConnection
   object.
   
Definition:
   #define AEEBTCONN_EV_STATUS_CHANGED                1
   #define AEEBTCONN_EV_SET_ROLE_SUCCESS              2
   #define AEEBTCONN_EV_SET_ROLE_FAILED               3
   #define AEEBTCONN_EV_SET_ROLE_SW_POLICY_SUCCESS    4
   #define AEEBTCONN_EV_SET_ROLE_SW_POLICY_FAILED     5
   #define AEEBTCONN_EV_SET_SECURITY_SUCCESS          6
   #define AEEBTCONN_EV_SET_SECURITY_FAILED           7
   typedef uint32 BTConnEventType;

===============================================================================
BTConnStatBitmap

Description:
   This data type defines the bitmap of the aspects of connection status 
   that can change.

   #define AEEBT_CONN_STAT_LINK_MODE_B       0x0001
   #define AEEBT_CONN_STAT_SECURITY_B        0x0002
   #define AEEBT_CONN_STAT_ROLE_B            0x0004
   #define AEEBT_CONN_STAT_SCO_PRESENT_B     0x0008
   #define AEEBT_CONN_STAT_RSSI_B            0x0010
   #define AEEBT_CONN_STAT_ROLE_SW_POLICY_B  0x0020

   typedef uint32  BTConnStatBitmap;

Members:
   AEEBT_CONN_STAT_LINK_MODE_B              Change in ConnMode field
   AEEBT_CONN_STAT_SECURITY_B               Change in Security field
   AEEBT_CONN_STAT_ROLE_B                   Change in bLocalIsMaster field
   AEEBT_CONN_STAT_SCO_PRESENT_B            Change in bSCOPresent field
   AEEBT_CONN_STAT_RSSI_B                   Change in nRSSI
   AEEBT_CONN_STAT_ROLE_SW_POLICY_B         Change in bRoleSwitchAllowed

===============================================================================
BTConnectionStatus

Description:
   This data type defines the status of the connection.

   typedef struct
   {
      BDAddress            bdAddr;         // remote end of the connection
      BTConnectionMode     dConnMode;      // active or one of low power modes
      BTSecurityLevel      dSecurity;      // none, auth, auth & encrypt
      boolean              bLocalIsMaster; // what more can be said?
      boolean              bRoleSwitchAllowed;
      boolean              bSCOPresent;    // is audio conn riding on this conn?
      int8                 nRSSI;          // current RSSI level
      BTConnOperationType  dPendingOp;     // pending operation, if any
   }  BTConnectionStatus;

Members:
   bdAddr            : address of remote device
   dConnMode         : active? sniff? park? hold?
   dSecurity         : none? authenticated? authenticated & encrypted?
   bLocalIsMaster    : local device is master of the connection?
   bRoleSwitchAllowed: role switch request from remote device allowed?
   bSCOPresent       : an audio connection is riding on this connection?
   nRSSI             : the current receive signal strength indicator
   dPendingOp        : pending operation, if any

=============================================================================*/

/*=============================================================================
INTERFACE DOCUMENTATION
===============================================================================
IBTConnection Interface

Description:
   This interface provides methods to monitor and configure an existing
   Bluetooth connection.

   Applications who register for and receive AEEBT_EV_CONN_UP can create
   an IBTConnection object for each connection.  Applications should release
   these objects when they receive corresponding AEEBT_EV_CONN_DOWN.

===H2>
   Usage example:
===/H2>

   ===== To create an IBTConnection object, see instruction in 
         class header file


   ===== To register Event signal
   // first, create pMe->piConnEvSignal, then register:
   IBTConnection_OnEventNotify (pMe->piBTConn, pMe->piConnEvSignal);


   ===== To retrieve event:
   BTConnEventType dEv
   BTResult dR;
   while (IBTConnection_GetEvent (pMe->piBTConn, &dEv, &dR) == AEE_SUCCESS)
   {
      switch (dEv)
      {
         case AEEBTCONN_EV_STATUS_CHANGED:
            if (IBTConnection_GetStatus (pMe->piBTConn, &dWhatsValid, 
                                         &pMe->sConnStat) == AEE_SUCCESS)
            {
               if (dWhatsValid & AEEBT_CONN_STAT_LINK_MODE_B)
               {
                  // make use of new link mode
               }
               // similarly check for other fields of interest
            }
            break;
          // handle other events
      }
   }


   ===== To toggle role switch policy on the connection
   boolean bSwAllowed = pMe->sConnStat.bRoleSwitchAllowed ? FALSE : TRUE;
   IBTConnection_SetRoleSwitchPolicy (pMe->piBTConn, bSwAllowed);


   ===== To make local device master of the connection
   IBTConnection_SetRole (pMe->piBTConn, TRUE);


   ===== To set highest security on the connection
   IBTConnection_SetMinSecurity (pMe->piBTConn, 
                                 AEEBT_SEC_AUTHENTICATE_AND_ENCRYPT);


   ===== To choose Sniff as low power mode for the connection with
   ===== 1-second sniff interval and 5-second idle timeout
   IBTConnection_SetLowPowerConfig (pMe->piBTConn, AEEBT_LPM_SNIFF, 1000, 5000);


   ===== To release the IBTConnection object:
   IBTConnection_Release (pMe->piBTConn);

===============================================================================
IBTConnection_AddRef

Description:
   Inherited from IQI_AddRef().

See Also:
   IBTConnection_Release()
   
===============================================================================
IBTConnection_Release

Description:
   Inherited from IQI_Release().

See Also:
   IBTConnection_AddRef()
   
===============================================================================
IBTConnection_QueryInterface

Description:
   Inherited from IQI_QueryInterface().
   It can be used to
     -  Get a pointer to an available interface based on the input class ID
     -  Query an extended version of the IBTConnection-derived class

Prototype:
   int IBTConnection_QueryInterface (IBTConnection*  po, 
                                     AEECLSID        clsReq, 
                                     void**          ppo);

Parameters:
   po [i]     : the object
   clsReq [i] : Class ID of the interface to query.
   ppo [o]    : Pointer to the interface.

Return Value:
   AEE_SUCCESS: on success, 
   Otherwise: an error code.

Comments:
   If ppo is back a NULL pointer, the queried interface is not available.

Side Effects:
   If an interface is retrieved, then this function increments its 
   reference count.

===============================================================================
IBTConnection_OnEventNotify()
   
Description:
   This method registers a signal to be triggered when there is an event
   that occurred on the IBTConnection object.  Application should use 
   IBTConnection_GetEvent() to retrieve the event information.

Prototype:
   int IBTConnection_OnEventNotify (IBTConnection*    po,
                                    ISignal*          piSignal);

Parameters:
   po [in]         : the object
   piSignal [i]    : signal to be triggered when an event occurs

Return values:
   AEE_SUCCESS     : callback function has been scheduled
   AEE_EBADPARM    : input pointers are invalid
   AEE_ENOSUCH     : connection no longer exists

See Also:
   IBTConnection_GetEvent()

===============================================================================
IBTConnection_GetEvent()

Description:
   This method retrieves the next event on the connection object.  When the
   signal registered via IBTConnection_OnEventNotify() is triggered, this
   API should be called repeatedly until it returns AEE_ENOREMORE to retrieve
   all events.

Prototype:
   int IBTConnection_GetEvent (IBTConnection*    po, 
                               BTConnEventType*  pdEvent,
                               BTResult*         pdResult);

Parameters:
   po [i]          : the object
   pdEvent [o]     : destination for the event code
   pdResult [o]    : only valid for event representing failure operation

Return values:
   AEE_SUCCESS     : event info successfully retrieved
   AEE_EBADPARM    : input pointers are invalid
   AEE_ENOMORE     : no more events to retrieve

See Also:
   IBTConnection_OnEventNotify()
   
===============================================================================
IBTConnection_GetStatus()
   
Description:
   This method retrieves the current connection status.
   
Prototype:
   int IBTConnection_GetStatus (IBTConnection*         po,
                                BTConnStatBitmap*      pdWhatsValid,
                                BTConnectionStatus*    psConnStatus); 

Parameters:
   po [i]               : the connection object
   pdWhatsValid [o]     : bitmap of fields that are valid
   psConnStatus [o]     : destination for current connection status

Return values:
   AEE_SUCCESS     : connection status is successfully retrieved
   AEE_EBADPARM    : input pointers are invalid
   AEE_ENOSUCH     : connection no longer exists

===============================================================================
IBTConnection_SetRole()
   
Description:
   This method causes a role switch attempt to be made.  If the attempt 
   succeeds, the connection status will change, and application will be
   informed if it has scheduled a callback via IBTConnection_OnEventNotify().
   
Prototype:
   int IBTConnection_SetRole (IBTConnection*  po, 
                              boolean         bMaster);

Parameters:
   po [i]          : the object
   bMaster [i]     : TRUE to make local master of the connection

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EBADPARM    : input pointers are invalid
   AEE_ENOSUCH     : connection no longer exists
   AEE_EPRIVLEVEL  : not sufficient privilege level
   AEE_EALREADY    : the previous SetRole operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

===============================================================================
IBTConnection_SetRoleSwitchPolicy()
   
Description:
   This method sets the role switch policy for the connection.  This API 
   controls whether role switch request from remote device will be accepted
   or not.

Prototype:
   int (IBTConnection_SetRoleSwitchPolicy (IBTConnection*  po,
                                           boolean         bAllowed);

Parameters:
   po [in]         : the object
   bAllowed [in]   : TRUE if role switch should be allowed; 
                     FALSE if disallowed

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EBADPARM    : input pointers are invalid
   AEE_ENOSUCH     : connection no longer exists
   AEE_EPRIVLEVEL  : not sufficient privilege level
   AEE_EALREADY    : the previous SetRoleSwitchPolicy operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

===============================================================================
IBTConnection_SetMinSecurity()
   
Description:
   This method sets the minimum security level for the connection.

Prototype:
   int IBTConnection_SetMinSecurity (IBTConnection*   po,
                                     BTSecurityLevel dSecLevel);

Parameters:
   po [i]          : the connection object
   dSecLevel [i]   : minimum security level to apply

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EBADPARM    : input pointers are invalid
   AEE_EPRIVLEVEL  : not sufficient privilege level
   AEE_ENOSUCH     : connection no longer exists
   AEE_EALREADY    : the previous SetMinSecurity operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

===============================================================================
IBTConnection_SetLowPowerConfig()
   
Description:
   This method sets the low power mode for the connection.

   If dMode is either AEEBT_LPM_DEFAULT or AEEBT_LPM_ACTIVE, uMaxInterval and
   uIdleTimeout would be ignored.  Default value for uMaxInterval is 1280 ms and
   for uIdleTimeout is 7000 ms.

Prototype:
   int IBTConnection_SetLowPowerConfig (IBTConnection*   po,
                                        BTLowPowerMode   dMode,
                                        uint16           uMaxInterval, 
                                        uint16           uIdleTimeout);

Parameters:
   po [i]          : the connection object
   dMode [i]       : low power mode
   uMaxInterval [i]: maximum number of milliseconds between
   uIdleTimeout [i]: number of milliseconds of non-activity before 
                       lower power mode is entered

Return values:
   AEE_SUCCESS     : operation has been scheduled
   AEE_EBADPARM    : input parameters are invalid
   AEE_EPRIVLEVEL  : not sufficient privilege level
   AEE_ENOSUCH     : connection no longer exists
   AEE_EALREADY    : the previous SetLowPowerConfig operation hasn't completed
   AEE_EITEMBUSY   : another operation is in progress

==========================================================================*/

#endif // AEEIBTCONNECTION_H

