#ifndef AEEIBTA2DPNOTIFIER_H
#define AEEIBTA2DPNOTIFIER_H

/*=============================================================================
FILE:         AEEIBTA2DPNotifier.h

SERVICES:     Bluetooth Advanced Audio Distribution Profile notifier interface

DESCRIPTION:  This file contains the interface definition and data structures
              for the Bluetooth A2DP notifier interface.
              Constants and definitions in the file refer to those
              defined in A2DP spec v1.2

===============================================================================
        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/


/*============================================================================
 Includes
============================================================================*/
#include "AEEIQI.h"
#include "AEEStdDef.h"
#include "AEEBTDef.h"
#include "AEEISignal.h"
/*=============================================================================
 Interface ID
=============================================================================*/
#define AEEIID_IBTA2DPNotifier 0x01061c2a

/*=============================================================================
   Definitions, Macros
==============================================================================*/


/*=============================================================================
   A2DP Event Types
==============================================================================*/
#define AEEBTA2DP_EVT_ENABLED              1
#define AEEBTA2DP_EVT_ENABLE_FAILED        2
#define AEEBTA2DP_EVT_DISABLED             3
#define AEEBTA2DP_EVT_CONNECTED            4
#define AEEBTA2DP_EVT_CON_FAILED           5
#define AEEBTA2DP_EVT_DISCONNECTED         6
#define AEEBTA2DP_EVT_START                7
#define AEEBTA2DP_EVT_SUSPEND              8
#define AEEBTA2DP_EVT_OPEN                 9
#define AEEBTA2DP_EVT_CLOSE                10

// A data type to refer to A2DP events
typedef uint32  BTA2DPEventType;

/*=============================================================================
   A2DP Result Types
==============================================================================*/
#define AEEA2DP_RESULT_SUCCESS                             0
#define AEEA2DP_RESULT_INCOMPATIBLE_DEVICES                1
#define AEEA2DP_RESULT_ENDPOINT_DISCOVERY                  2
#define AEEA2DP_RESULT_FAILED_TO_START_STREAM              3
#define AEEA2DP_RESULT_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM  4
#define AEEA2DP_RESULT_ABORT_FAILED                        5
#define AEEA2DP_RESULT_FAILED_TO_PROCESS_CMD               6
#define AEEA2DP_RESULT_SET_CONFIG_FAILED                   7
#define AEEA2DP_RESULT_CONFIG_PARAMS_NOT_AGREEABLE         8
#define AEEA2DP_RESULT_CONNECT_REFUSED_NO_RESOURCES        9
#define AEEA2DP_RESULT_PAGE_FAILED                         10
#define AEEA2DP_RESULT_AUTHENTICATION_FAILED               11
#define AEEA2DP_RESULT_NORMAL_DISCONNECT                   12
#define AEEA2DP_RESULT_LINK_LOST                           13
#define AEEA2DP_RESULT_WRONG_COMMAND                       14
#define AEEA2DP_RESULT_RESOURCES_NOT_AVAILABLE             15
#define AEEA2DP_RESULT_RESET_IN_PROGRESS                   16

// A data type to refer to A2DP result codes
typedef uint32    BTA2DPResult;

/*=============================================================================
   A2DP Event data 
==============================================================================*/
typedef struct
{
   BTA2DPEventType    event;
   BTA2DPResult       result;
   BDAddress          remoteAddr;
   boolean            bLocalInit;
}BTA2DPEvent;

/*=============================================================================
   A2DP Event data 
==============================================================================*/

#define INHERIT_IBTA2DPNotifier(iname)                \
   INHERIT_IQI(iname);                                \
                                                      \
   int (*OnEventNotify)  (iname         *po,          \
                          ISignal       *piSignal);   \
   int (*GetEvent)       (iname         *po,          \
                          BTA2DPEvent   *pEvent)
AEEINTERFACE_DEFINE(IBTA2DPNotifier);


/*============================================================================
 Interface Accessor Functions
============================================================================*/
static __inline uint32 IBTA2DPNotifier_AddRef(IBTA2DPNotifier *p)
{
   return AEEGETPVTBL((p),IBTA2DPNotifier)->AddRef(p);
}

static __inline uint32 IBTA2DPNotifier_Release(IBTA2DPNotifier *p)
{
   return AEEGETPVTBL((p),IBTA2DPNotifier)->Release(p);
}

static __inline int IBTA2DPNotifier_QueryInterface(IBTA2DPNotifier *p,
                                              AEEIID idReq, void **ppo)
{
   return AEEGETPVTBL((p),IBTA2DPNotifier)->QueryInterface(p, idReq, ppo);
}

static __inline int IBTA2DPNotifier_OnEventNotify(IBTA2DPNotifier *p,
                                                  ISignal         *piSignal)
{
   return AEEGETPVTBL((p),IBTA2DPNotifier)->OnEventNotify(p, piSignal);
}

static __inline int IBTA2DPNotifier_GetEvent(IBTA2DPNotifier *p,
                                             BTA2DPEvent     *pEvent)
{
   return AEEGETPVTBL((p),IBTA2DPNotifier)->GetEvent(p, pEvent);
}

/*=============================================================================
  DATA TYPES DOCUMENTATION
===============================================================================

BTA2DPEventType

Definition
   typedef uint32    BTA2DPEventType;

Description
   A data type to refer to A2DP events

   AEEBTA2DP_EVT_ENABLED         : Audio engine enabled and ready to 
                                 : accept/ initiate connections
   AEEBTA2DP_EVT_DISABLED        : Audio engine disabled and cannot accept/
                                 : initiate connections 
   AEEBTA2DP_EVT_CONNECTED       : connected to a device
   AEEBTA2DP_EVT_CON_FAILED      : connection to a device failed
   AEEBTA2DP_EVT_DISCONNECTED    : disconnected from a device
   AEEBTA2DP_EVT_START           : received start event
   AEEBTA2DP_EVT_SUSPEND         : received suspend event
   AEEBTA2DP_EVT_OPEN            : received open event
   AEEBTA2DP_EVT_CLOSE           : received close event

==============================================================================
BTA2DPResult

Definition
   typedef uint32    BTA2DPResult;

Description
   A data type to refer to A2DP Results 

   AEEA2DP_RESULT_SUCCESS                            : No error
   AEEA2DP_RESULT_INCOMPATIBLE_DEVICES               : No endpoints found during discovery
   AEEA2DP_RESULT_ENDPOINT_DISCOVERY                 : Endpoint discovery failed
   AEEA2DP_RESULT_FAILED_TO_START_STREAM             : Failed to start stream
   AEEA2DP_RESULT_ATTEMPTED_TO_CLOSE_AND_OPEN_STREAM : Source tried to close an open stream
                                                       but sink disconnects
   AEEA2DP_RESULT_ABORT_FAILED                       : abort failed
   AEEA2DP_RESULT_FAILED_TO_PROCESS_CMD              : lower layer failed to process 
                                                       BT command
   AEEA2DP_RESULT_SET_CONFIG_FAILED                  : Set configuration failed
   AEEA2DP_RESULT_CONFIG_PARAMS_NOT_AGREEABLE        : configuration params dont match
   AEEA2DP_RESULT_CONNECT_REFUSED_NO_RESOURCES       : connection rejected by sink
   AEEA2DP_RESULT_PAGE_FAILED                        : page failure
   AEEA2DP_RESULT_AUTHENTICATION_FAILED              : BT authentication failed
   AEEA2DP_RESULT_NORMAL_DISCONNECT                  : normal disconnect
   AEEA2DP_RESULT_LINK_LOST                          : link loss at lower layers
   AEEA2DP_RESULT_RESOURCES_NOT_AVAILABLE            : lower layer rejects connection
                                                       due to lack of resources 
   AEEA2DP_RESULT_RESET_IN_PROGRESS                  : often used when radio is off


==============================================================================
BTA2DPEvent

Description
   A data type to refer to A2DP Events 

Definition

typedef struct
{
   BTA2DPEventType     event;
   BTA2DPResult        result;
   BDAddress           remoteAddr;
   boolean             bLocalInit;
}BTA2DPEvent;

Members 
   event           : event type
   result          : result/ reason for the event
   remoteAddr      : address of the remote device corresponding to 
                     event data
   bLocalInit      : TRUE used to indicate that the event is due to a locally
                     initiated procedure esp for START and SUSPEND events.
                     for all other reasons, ie, due to signalling from
                     remote device, or a baseband timeout,
                     bLocalInit = FALSE

===============================================================================
   INTERFACE DOCUMENTATION
===============================================================================
Interface Name: IBTA2DPNotifier 

Description: BlueTooth Audio Distribution Profile Notifier Interface

   This interface provides a way for applications to register for and read
   A2DP events.


==============================================================================

IBTA2DPNotifier_AddRef

Description:
   Inherited from IQI_AddRef().

See Also:
   IBTA2DPNotifier_Release()
   
===============================================================================
IBTA2DPNotifier_Release

Description:
   Inherited from IQI_Release().

See Also:
   IBTA2DPNotifier_AddRef()
   
===============================================================================
IBTA2DPNotifier_QueryInterface

Description:
   Inherited from IQI_QueryInterface().
   It can be used to
     -  Get a pointer to an available interface based on the input interface ID

Prototype:
   int IBTMgr_QueryInterface (IBTA2DPNotifier*  po, 
                              AEEIID   iidReq, 
                              void**   ppo);

Parameters:
   po [i]     : the object
   iidReq [i] : IID of the interface to query.
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
Function: IBTA2DPNotifier_OnEventNotify()

Description:
   This method registers a signal to be triggered when there is an event
   to be delivered to the application

Prototype:

  int IBTA2DPNotifier_OnEventNotify(BTA2DPNotifier  *p,
                                    ISignal         *piSignal);

Parameters:
   p [i]          : pointer to the notifier object
   piSignal [i]   : signal to be registered

Return value:

   AEE_SUCCESS    : signal was registered
   AEE_EBADPARM   : bad pointers

===============================================================================

Function: IBTA2DPNotifier_GetEvent

Description:
   This method retrieves A2DP events. The application should call this API
   to retrieve events until the return value is AEE_ENOMORE.

Prototype:
   int IBTA2DPNotifier_GetEvent(IBTA2DPNotifier   *p,
                                BTA2DPEvent       *pEvent);

Parameters
   p [i]           : pointer to the notifier object
   pEvent [i]      : event data


Return Value:
   AEE_SUCCESS     : the event was returned
   AEE_EBADPARM    : input pointers are invalid
   AEE_ENOMORE     : no more events to be delivered
   
See Also:
   BTA2DPEvent

================================================================================*/
#endif /* _AEEIBTA2DPNOTIFIER_H_ */
