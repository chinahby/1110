#ifndef AEEBT_H
#define AEEBT_H
/*=============================================================================
FILE:         AEEBT.h

SERVICES:     IBT Interfaces

DESCRIPTION:  IBT is the BREW interface that controls Bluetooth engine.

===============================================================================
        Copyright © 2006-2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=============================================================================*/
#include "AEEInterface.h"
#include "AEEBTCommon.h"

/*=============================================================================
  Interface Definition
=============================================================================*/
// BREW generated ID
#define AEEIID_BT    	    0x01039dfa

typedef struct IBT IBT;

AEEINTERFACE(IBT)
{
   INHERIT_IQueryInterface(IBT);

   int (*Command)         (IBT*               po,
                           int                cmd,
                           AEECallback*       pCb,
                           const void*        pInBuffer, 
                           int                inBufSize, 
                           void*              pOutBuffer,
                           int                outBufSize);
   int (*GetObject)       (IBT*               po,
                           AEECLSID           iId,
                           const void*        pInData,
                           int                size,
                           void**             ppObj);
};

#define AEEBT_CMD_GET_NOTIF_DATA        0x0001
#define AEEBT_CMD_GET_LOCAL_INFO        0x0002
#define AEEBT_CMD_SET_LOCAL_NAME        0x0003
#define AEEBT_CMD_SET_LOCAL_SECURITY    0x0004
#define AEEBT_CMD_CONFIG_APPLICATION    0x0005
#define AEEBT_CMD_GET_RADIO_STATE       0x01062bcd

// Standard QueryInterface Methods
static __inline uint32 IBT_AddRef (IBT* p)
{
   return AEEGETPVTBL((p),IBT)->AddRef((p));
}

static __inline uint32 IBT_Release (IBT* p)
{
   return AEEGETPVTBL((p),IBT)->Release((p));
}

static __inline int IBT_QueryInterface (IBT* p,
                                        AEECLSID clsid,
                                        void **ppo)
{
   return AEEGETPVTBL((p),IBT)->QueryInterface((p),(clsid),(ppo));
}

// IBT Commands
static __inline int IBT_Command (IBT*         p,
                                 int          cmd,
                                 AEECallback* pCb,
                                 const void*  pInBuffer, 
                                 int          inBufSize, 
                                 void*        pOutBuffer,
                                 int          outBufSize)
{
   return AEEGETPVTBL ((p),IBT)->Command((p),(cmd),(pCb),
                                         (pInBuffer),(inBufSize),
                                         (pOutBuffer),(outBufSize));
}

static __inline int IBT_GetNotificationData(IBT*        p,
                                            const void* pNotifInfo, 
                                            int         niSize,
                                            void*       pEventData,
                                            int         uEDSize)
{
   return AEEGETPVTBL((p),IBT)->Command((p),AEEBT_CMD_GET_NOTIF_DATA,         
                                        NULL,(pNotifInfo),(niSize),
                                        (pEventData),(uEDSize));
}

static __inline int IBT_GetLocalInfo (IBT*  p,
                                      void* pLocalInfo, 
                                      int   nSize)
{
   return AEEGETPVTBL((p),IBT)->Command((p),AEEBT_CMD_GET_LOCAL_INFO, 
                                        NULL,NULL,0,(pLocalInfo),(nSize));
}

static __inline int IBT_GetRadioState (IBT* p, BTRadioState* pdRadioState)
{
   return AEEGETPVTBL((p),IBT)->Command((p),AEEBT_CMD_GET_RADIO_STATE, 
                                        NULL,NULL,0,
                                        (pdRadioState),sizeof(BTRadioState));
}

static __inline int IBT_GetObject (IBT*         p,
                                   AEECLSID     cId,
                                   const void*  pInData,
                                   int          nSize,
                                   void**       ppObj)
{
   return AEEGETPVTBL ((p),IBT)->GetObject((p),(cId),(pInData),(nSize),(ppObj));
}

/*=============================================================================
INTERFACE DOCUMENTATION
===============================================================================
IBT Interface

Description:
   This interface enables BREW applications to control Bluetooth driver.
   
===============================================================================
IBT_AddRef()

Description:
   Inherited from IBASE_AddRef().

See Also:
   IBT_Release()
   
===============================================================================
IBT_Release()

Description:
   Inherited from IBASE_Release().

See Also:
   IBT_AddRef()
   
===============================================================================
IBT_QueryInterface()

Description:
   Inherited from IQI_QueryInterface().
   It can be used to
     -  Get a pointer to an available interface based on the input class ID
     -  Query an extended version of the IBT-derived class

Prototype:
   int IBT_QueryInterface(IBT*      po, 
                          AEECLSID  clsReq, 
                          void**    ppo);

Parameters:
   po [i]     : the object
   clsReq [i] : Class ID of the interface to query.
   ppo [o]    : Pointer to the interface.

Return Value:
   SUCCESS      : ppo points to a valid IBT object
   EBADPARM     : invalid input pointer(s)

Comments:
   If ppo is back a NULL pointer, the queried interface is not available.

Side Effects:
   If an interface is retrieved, then this function increments its 
   reference count.

===============================================================================
IBT_Command()

Description:
   This function creates an object based on the given class ID.

Prototype:
   int IBT_Command (IBT*         po, 
                    int          cmd
                    AEECallback* pCb,
                    const void*  pInData,
                    int          nInSize,
                    void*        pOutBuf,
                    int          nOutSize);

Parameters:
   po [i]         : the IBT object
   cmd [i]        : the command AEEBT_CMD_*
   pCb [i]        : callback to be invoked when command is done; can be NULL
   pInData [i]    : input parameters; can be NULL
   nInSize [i]    : size of pInData; ignored if pInData is NULL
   pOutData [o]   : destination for output data
   nOutSize [i]   : size of pOutBuf; ignored if pOutBuf is NULL

Return Value:
   command specific return value

===============================================================================
IBT_GetLocalInfo()

Description:
   This function retrieves information on the local BT device.

Prototype:
   int IBT_GetLocalInfo(IBT* po, void* pLocalInfo, int nSize);

Parameters:
   po [i]         : the IBT object
   pLocalInfo [o] : info on the local BT device will be copied to this location
   nSize [i]      : size of BTLocalInfo info structure                      

Return Value:
   SUCCESS        : ppSPPClient points to an object of type IBT
   EBADPARM       : invalid input pointer(s)

===============================================================================
IBT_GetNotificationData()

Description:
   The application should invoke this API to retrieve notification data;
   notifications will be received after registering for them. In the notification
   handler, the event is obtained by calling GET_NOTIFIER_VAL() on dwMask of AEENotify.
   This event should be passed in pNotifInfo->event. pData of AEENotify contains 
   a pointer to the timestamp. This timestamp should be passed in uTimeStamp of
   pNotifInfo.
  

Prototype:
   int IBT_GetNotificationData(IBT*        po, 
                               const void* pNotifInfo, 
                               int         niSize,
                               void*       pEventData,
                               int         nEDSize)

Parameters:
   po [i]           : the IBT object
   pNotifInfo [i]   : info to look up event.  
   niSize [i]       : size of notification info structure
   pEventData [o]   : all data associated with the event of interest
   nEDSize [i]      : size of event data structure

Return Value:
   SUCCESS          : eventData field contains valid info
   EBADPARM         : invalid input pointer(s)

See Also :
   AEENotify
   BTNotifyInfo
   BTEventData
===============================================================================
IBT_GetRadioState()

Description:
   This method retrieves the current state of the local device's radio.

Prototype:
   int IBT_GetRadioState (IBT* po, BTRadioState* pdRadioState);

Parameters:
   po [i]           : the IBT object
   pdRadioState [o] : destination for the current radio state

Return Value:
   SUCCESS          : radio state has been retrieved
   EBADPARM         : invalid input parameters

===============================================================================
IBT_GetObject()

Description:
   This function creates an object based on the given class ID.

Prototype:
   int IBT_GetObject (IBT* po, 
                      AEECLSID     cId,
                      const void*  pInData,
                      int          nSize,
                      void**       ppObj);

Parameters:
   po [i]           : the IBT object
   cId [i]          : class ID specifying the type of object to create
   pInData [i]      : input parameter; can be NULL
   nSize [i]        : size of pInData; ignored if pInData is NULL
   ppObj [o]        : points to the object created with cID

Return Value:
   SUCCESS          : ppObj points to an object of interface type supported
                        by the give class ID
   EBADPARM         : invalid input paramters
   ECLASSNOTSUPPORT : the specified class is not supported

=============================================================================*/

#endif // AEEBT_H


