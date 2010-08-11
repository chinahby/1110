#ifndef _AEE_BT_EXT_NA_H_
#define _AEE_BT_EXT_NA_H_

/*===========================================================================
FILE:      AEEBTExtNA.h

SERVICES:  BlueTooth Network Access

GENERAL DESCRIPTION: BREW interface to the BT Network Access

PUBLIC CLASSES AND STATIC FUNCTIONS:

    IBTExtNA

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/

// BREW generated ID
#define AEECLSID_BLUETOOTH_NA    	          0x01013e08

// event types returned via HandleEvent()
enum
{
  AEEBT_NA_EVT_ENABLED,
  AEEBT_NA_EVT_CONNECTED,
  AEEBT_NA_EVT_DISCONNECTED,
  AEEBT_NA_EVT_DISABLED
};

// command status
enum
{
  AEEBT_NA_ERR_NONE,
  AEEBT_NA_ERR_BAD_STATE,
  AEEBT_NA_ERR_NO_SIO_PORT,
  AEEBT_NA_ERR_ALREADY_ENABLED,
  AEEBT_NA_ERR_NOT_ALLOWED
};

/* ========================================================================= */
typedef struct _IBTExtNA IBTExtNA;

AEEINTERFACE(IBTExtNA)
{
   INHERIT_IQueryInterface(IBTExtNA);
   int  (*Enable)(IBTExtNA* po);
   int  (*Disable)(IBTExtNA* po);
};


#define IBTEXTNA_AddRef(p)                \
        AEEGETPVTBL((p),IBTExtNA)->AddRef((p))
        
#define IBTEXTNA_Release(p)               \
        AEEGETPVTBL((p),IBTExtNA)->Release((p))
        
#define IBTEXTNA_QueryInterface(p,i,ppo)  \
        AEEGETPVTBL((p),IBTExtNA)->QueryInterface((p),(i),(ppo))
        
#define IBTEXTNA_Enable(p)                \
        AEEGETPVTBL((p),IBTExtNA)->Enable((p))
        
#define IBTEXTNA_Disable(p)               \
        AEEGETPVTBL((p),IBTExtNA)->Disable((p))
        
/*=====================================================================
  DATA TYPES DOCUMENTATION
=======================================================================


=======================================================================
   INTERFACE DOCUMENTATION
=======================================================================

Interface Name:  IBTExtNA

Description:

  IBTExtNA is a simple interface to the BlueTooth Network Access layer.
  It provides the following services:
  - Enabling/disabling Network Access functionality of BT driver.
  - Notifying applications of changes in BT Network Access connection state.

  The IBTExtNA interface is obtained via the ISHELL_CreateInstance mechanism.

  The use of IBTExtAG consists of sending commands and receiving events.
  These events are delivered to IBTExtNA clients as notifications, hence
  the clients must register for NA notification via ISHELL_RegisterNotify()
  and the notification mask to use is NMASK_BT_NA.  See AEEBTExt.h for
  more information on IBTExtNotifier.
  
  If a command returns SUCCESS, then an event is guaranteed to be generated 
  in response at a later time.  However, not all events are associated with 
  a command.  Some events are asyncronously generated such as connection
  state change.

  In the event handler, the dwParam for EVT_NOTIFY is a pointer to an AEENotify
  whose dwMask field is the combination of the notification mask and the
  event which is one of the AEEBT_AG_EVT_* events when the notification mask
  is NMASK_BT_NA.  The pData field of the AEENotify is a pointer to 
  NotificationData.
  

  IBTEXTNA also provides several additional interfaces; please see the
  corresponding header file for complete documentation.
  
  IBASE                        AEE.h
    IBTEXTNA_AddRef()
    IBTEXTNA_Release()
  IQUERYINTERFACE              AEE.h
    IBTEXTNA_QueryInterface()

=======================================================================

Function:  IBTEXTNA_Enable()

Description: Enables Bluetooth Network Access.

Prototype:

   int IBTEXTNA_Enable( IBTExtNA* po );

Parameters:

   po        : [Input] Pointer to the IBTExtNA object

Return value:

   SUCCESS indicates that the enable Network Access request was queued and
   that AEEBT_NA_EVT_ENABLED event will be generated at a later time.
   All other values indicate failure, and will not generate an event.
   In this context, the following error codes have special meaning:

  EBADPARM  - invalid input param
  ENOMEMORY - out of memory

Events:

  AEEBT_NA_EVT_ENABLED can be accompanied by any of the following
  error codes:
  
  AEEBT_NA_ERR_NONE             - no error
  AEEBT_NA_ERR_NO_SIO_PORT      - no SIO stream was opened
  AEEBT_NA_ERR_ALREADY_ENABLED  - 

=======================================================================

Function:  IBTEXTNA_Disable()

Description: Disables Bluetooth Network Access.

Prototype:

  int IBTEXTNA_Disable( IBTExtNA* po )

Parameters:

  po        : [Input] Pointer to the IBTExtNA object

Return value:

   SUCCESS indicates that the disable Network Access request was queued and 
   that AEEBT_NA_EVT_DISABLED event will be generated at a later time.
   All other values indicate failure, and will not generate an event.
   In this context, the following error codes have special meaning:

  EBADPARM  - invalid input param
  ENOMEMORY - out of memory

Events:

  AEEBT_NA_EVT_DISABLED can be accompanied by any of the following
  error codes:
  
  AEEBT_NA_ERR_NONE         - no error
  AEEBT_NA_ERR_NOT_ALLOWED  - another client has ownership of this object

=======================================================================*/
#endif /* _AEE_BT_EXT_NA_H_ */
