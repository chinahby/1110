#if !defined(OEMSMSSTORAGE_H)
#define OEMSMSSTORAGE_H
/*======================================================
                    IMPORTANT NOTICE

                  OEM DISTRIBUTION ONLY

FILE:  OEMSMSStorage.h

SERVICES:  AEE OEM SMS Interface header

GENERAL DESCRIPTION:

This file contains all of the function prototypes for
routines that are required by the AEE to function with
SMS.

        Copyright (c) 1999-2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEE.h"
#include "AEESMS.h"
#include "OEMSMS.h"

typedef struct
{
   uint32 nEvent;
   AEESMSStorageEvtData sEventData;
} OEMStorageUpdateData;

#ifdef __cplusplus
extern "C" {
#endif

AEE_EXPORTS boolean OEMSMS_IsStorageInitialized(AEESMSStorageType st);   
AEE_EXPORTS void OEMSMS_StoreMsg(AEESMSStorageType st, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 * pnIndex, uint32 *pnRv, AEECallback *pcb);
AEE_EXPORTS void OEMSMS_ReadMsg(AEESMSStorageType st, uint32 nIndex, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 * pnRv, AEECallback *pcb);
AEE_EXPORTS void OEMSMS_RemoveMsg(AEESMSStorageType st, AEESMSTag mt, uint32 nIndex, uint32 * pnRv, AEECallback *pcb);
AEE_EXPORTS void OEMSMS_GetStorageStatus(AEESMSStorageType st, AEESMSTag mt, AEESMSStorageStatus * pss, uint32 * pnRv, AEECallback *pcb);
AEE_EXPORTS int OEMSMS_GetEnumMsgIDs(AEESMSStorageType st, AEESMSTag mt, uint32 * pdwIndex, uint32 * pdwSize);
AEE_EXPORTS void OEMSMS_UpdateMsg(AEESMSStorageType st, uint32 nIndex, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 *pnRv, AEECallback *pcb);
AEE_EXPORTS void OEMSMS_AutoStoreMsg(AEESMSClass mc, AEESMSStorageType st, uint32 *pnRv, AEECallback *pcb);
AEE_EXPORTS void OEMSMS_OnStorageUpdate(PFNNOTIFY pfnNotify);
            
#ifdef __cplusplus
}
#endif

/*========================================================================
   DATA STRUCTURE DOCUMENTATION
==========================================================================

OEMStorageUpdateData

Description:
   This structure is used to pass model event and corresponding data by
OEM layer to AEE layer when calling OEMSMS_OnStorageUpdate().

Definition:

   typedef struct
   {
      uint32 nEvent;
      AEESMSStorageEvtData sEventData;
   } OEMStorageUpdateData;

Comments:
   None.

See Also:
   OEMSMS_OnStorageUpdate()

==========================================================================
  INTERFACES DOCUMENTATION
==========================================================================
Interface Name: OEMSMS
==========================================================================

Function: OEMSMS_IsStorageInitialized()

Description:
   Determines if storage specified is initialized and ready for use.

Prototype:
   boolean OEMSMS_IsStorageInitialized(AEESMSStorageType st);

Parameters:
   st: Specifies storage type

Return Value:
   TRUE - If storage is initialized and ready for use
   FALSE - Otherwise

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMS_GetStorageStatus()

Description:
   Determines storage status for specified storage and message type. Callback
   is invoked at the end of the operation and storage status and return value
   are provided in pss and pnRv respectively.

Prototype:
   void OEMSMS_GetStorageStatus( AEESMSStorageType st, AEESMSTag mt, AEESMSStorageStatus * pss, uint32 * pnRv, AEECallback *pcb )

Parameters:
   st: storage type
   mt: message type
   pss: place holder for storage status
   pnRv: place holder for return value
   pcb: pointer to callback

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMS_StoreMsg()

Description:
   Stores the message specified by pMsgData and pMsgPayload in st storage type. 
Callback is invoked at the end of the operation and storage location index and 
return value are provided in pnIndex and pnRv respectively.

Prototype:
   void OEMSMS_StoreMsg( AEESMSStorageType st, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 * pnIndex, uint32 *pnRv, AEECallback *pcb )

Parameters:
   st: storage type
   pMsgData: pointer to message data
   pMsgPayload: pointer to payload data
   pnIndex: place holder for storage location index
   pnRv: place holder for return value
   pcb: pointer to callback

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMS_ReadMsg()

Description:
   Reads the message specified by st storage type and nIndex storage location index.
Callback is invoked at the end of the operation and message data, message payload and 
return value are provided in pMsgData, pMsgPayload and pnRv respectively.

Prototype:
   void OEMSMS_ReadMsg( AEESMSStorageType st, uint32 nIndex, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 * pnRv, AEECallback *pcb )

Parameters:
   st: storage type
   nIndex: message storage location index
   pMsgData: pointer to message data
   pMsgPayload: pointer to payload data
   pnRv: place holder for return value
   pcb: pointer to callback

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMS_RemoveMsg()

Description:
   Removes message(s) of specified index or type from the specified storage type.
Callback is invoked at the end of the operation and return value is provided in pnRv.

Prototype:
   void OEMSMS_RemoveMsg( AEESMSStorageType st, AEESMSTag mt, uint32 nIndex, uint32 * pnRv, AEECallback *pcb )

Parameters:
   st: storage type
   mt: message type
   nIndex: message storage location index
   pnRv: place holder for return value
   pcb: pointer to callback

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMS_GetEnumMsgIDs()

Description:
   Gets storage location indices of messages of type mt from storage type st.
If buffer specified by pdwIndex is smaller than the size required, size of the
required buffer is specified in pdwSize.

Prototype:
   int OEMSMS_GetEnumMsgIDs(AEESMSStorageType st, AEESMSTag mt, uint32 * pdwIndex, uint32 * pdwSize )

Parameters:
   st: storage type
   mt: message type
   pdwIndex: pointer to buffer to hold storage location indices
   pdwSize: size of buffer specified by pdwIndex in bytes

Return Value:
   SUCCESS on SUCCESS
   If there is an error, this function should return an error
    from AEEError.h

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMS_UpdateMsg()

Description:
   Replaces the message stored in nIndex index of stotage type st with
the message specified by pMsgData and pMsgPayload. Callback is invoked at the 
end of the operation and return value is provided in pnRv.

Prototype:
   void OEMSMS_UpdateMsg( AEESMSStorageType st, uint32 nIndex, OEMMsgData * pMsgData, OEMMsgPayload * pMsgPayload, uint32 *pnRv, AEECallback *pcb )

Parameters:
   st: storage type
   nIndex: storage location index
   pMsgData: pointer to message data
   pMsgPayload: pointer to payload data
   pnRv: place holder for return value
   pcb: pointer to callback

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMS_AutoStoreMsg()

Description:
   Specifies auto storage type for the messages of the specified message class. 
Callback is invoked at the end of the operation and return value is provided in pnRv.

Prototype:
   void OEMSMS_AutoStoreMsg(AEESMSClass mc, AEESMSStorageType st, uint32 *pnRv, AEECallback *pcb);

Parameters:
   mc: message class
   st: storage type
   pnRv: place holder for return value
   pcb: pointer to callback

Return Value:
   None.

Comments:
   None.

Side Effects:
   None.

See Also:
   None.

==========================================================================

Function: OEMSMS_OnStorageUpdate()

Description:
   Called to notify of EVT_MDL_SMSSTORAGE_XXX events.

Prototype:
   OEMSMS_OnStorageUpdate(PFNNOTIFY pfnNotify)

Parameters:
   pfnNotify: place holder for callback function

Return Value:
   None.

Comments:
   pfnNotify must be called with pointer to OEMStorageUpdateData containing new event data.

Side Effects:
   None.

See Also:
   OEMStorageUpdateData

========================================================================*/

#endif /*OEMSMSSTORAGE_H*/
