#ifndef __SS_CALL_DATABASE_H__
#define __SS_CALL_DATABASE_H__

/*=========================================================================
FILE: SSCallDatabase.h

SERVICES: GSM1x SS Call Database

GENERAL DESCRIPTION:
    The Call Database stores call related information to be used by the FSM 
    and the SSOUI. 
    Each call has a single call record and a single or multiple
    call transaction information records. These call records are created when
    the call is setup and deleted when the call terminates.
    The Call Database supports up to 2 calls. Each call can be a MPTY call or
    a regular (non-MPTY) call. It supports up to 6 call transactions, which 
    is sufficient for handling one full MPTY call (5 remote parties) and an
    additional regular call.

    The Call Database is also used for storing the first (CDMA) call details.
    These details can be used later, when the SSO loads.

PUBLIC CLASSES AND STATIC FUNCTIONS:
    SSCallDatabase

INITIALIZATION & SEQUENCING REQUIREMENTS:
    Only one instance of Call Database should be used by the applet.

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "AEE.h"          // Standard AEE definitions
#include "AEEShell.h"     // AEE Shell Services
#include "AEEStdLib.h"    // BREW's standard library

#include "SSCallCommonDefinitions.h"

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/

// max number of calls
#define MAX_DATABASE_CALL_ENTRIES (2)
// max number of transactions
#define MAX_DATABASE_TRANSACTION_ENTRIES (6)
// valid transaction identifier values are 0-6
#define NUMBER_OF_MO_TI_VALUES 7

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

// Call Database's internal data
typedef struct _SSCallDatabase
{
    // External interfaces
    IShell                 *m_pShell;       // Shell interface

    // calls information
    SSCall_Info_type        calls[ MAX_DATABASE_CALL_ENTRIES ];
    boolean                 isCallAllocated[ MAX_DATABASE_CALL_ENTRIES ];

    // call transactions information
    SSCall_TInfo_type       transactions[ MAX_DATABASE_TRANSACTION_ENTRIES ];
    boolean                 isTransactionAllocated[ MAX_DATABASE_TRANSACTION_ENTRIES ];

    // track MO TI allocation
    boolean                 isMoTiAllocated[ NUMBER_OF_MO_TI_VALUES ];

} SSCallDatabase;

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

/*=============================================================================
FUNCTION: SSCallDatabase_New

DESCRIPTION: Create an SSCallDatabase instance

PARAMETERS:
    pIShell [in]: Pointer to the IShell instance

RETURN VALUE:
    pointer to the new SSCallDatabase or NULL on MALLOC failure.

COMMENTS:
    in order to free SSCallDatabase use SSCallDatabase_Release().
    The applet should use only one instance of SSCallDatabase.
=============================================================================*/
SSCallDatabase* SSCallDatabase_New(IShell* pIShell);

/*=============================================================================
FUNCTION: SSCallDatabase_Release

DESCRIPTION: Destroy an SSCallDatabase instance

PARAMETERS:
   pCallDB [in]: pointer to a valid SSCallDatabase to be destroyed

RETURN VALUE:
    None
=============================================================================*/
void SSCallDatabase_Release(SSCallDatabase* pCallDB);

/*=============================================================================
FUNCTION: SSCallDatabase_Reset

DESCRIPTION: clears the Call database data

PARAMETERS:
   pCallDB [in]: pointer to a valid SSCallDatabase

RETURN VALUE:
    None
=============================================================================*/
void SSCallDatabase_Reset(SSCallDatabase *pCallDB);

/*=============================================================================
FUNCTION: SSCallDatabase_AllocateMoTi

DESCRIPTION: Allocate a new Mobile originated Transaction Id value

PARAMETERS:
   pCallDB [in]   : pointer to a valid SSCallDatabase
   pTIValue [out] : on return, set to the allocated TI value.

RETURN VALUE:
    TRUE if a free TI value was found and allocated. FALSE otherwise.
=============================================================================*/
boolean SSCallDatabase_AllocateMoTi(
    SSCallDatabase      *pCallDB,
    SSCall_TIValue_type *pTIValue
    );

/*=============================================================================
FUNCTION: SSCallDatabase_DeallocateMoTi

DESCRIPTION: Deallocate a Mobile originated Transaction Id value

PARAMETERS:
   pCallDB [in]  : pointer to a valid SSCallDatabase
   tiValue  [in] : the transaction id value to deallocate

RETURN VALUE:
    TRUE if tiValue was previously allocated and is now deallocated. 
    FALSE if tiValue was not allocated.
=============================================================================*/
boolean SSCallDatabase_DeallocateMoTi(
    SSCallDatabase      *pCallDB,
    SSCall_TIValue_type  tiValue
    );

/*=============================================================================
FUNCTION: SSCallDatabase_IsMoTiAllocated

DESCRIPTION: Check if a Mobile originated Transaction Id value is allocated

PARAMETERS:
    pCallDB [in] : pointer to a valid SSCallDatabase
    tiValue [in] : the transaction id value to check

RETURN VALUE:
    TRUE if tiValue is allocated, FALSE otherwise.
=============================================================================*/
boolean SSCallDatabase_IsMoTiAllocated(
    SSCallDatabase      *pCallDB,
    SSCall_TIValue_type  tiValue
    );

/*=============================================================================
FUNCTION: SSCallDatabase_GetAllocatedMoTiCount

DESCRIPTION: 
    Get the number of allocated Mobile originated Transaction Id values.

PARAMETERS:
   pCallDB [in]: pointer to valid SSCallDatabase

RETURN VALUE:
    The number of allocated MO TI values.
=============================================================================*/
uint16 SSCallDatabase_GetAllocatedMoTiCount(SSCallDatabase *pCallDB);

/*=============================================================================
FUNCTION: SSCallDatabase_AddCall

DESCRIPTION: 
    Add a new call record, by allocating a new call info entry and a new call
    transaction info entry. 
    The new call transaction info's index is placed as the first and only
    transaction info index in the new call info.
    The call's MPTY information is set as for a non MPTY call.

PARAMETERS:
    pCallDB [in]      : pointer to valid SSCallDatabase
    pTinfo [in]       : pointer to a valid call transaction info structure that
                        holds information about the new call.
                        Ownership of pTinfo stays with the caller.
    pCallIndex [out]  : returns the allocated call index. This parameter is
                        optional, if it is NULL then it is ignored,
    pTInfoIndex [out] : returns the allocated transaction index. This parameter
                        is optional, if it is NULL then it is ignored,

RETURN VALUE:
    TRUE if a new call record was successfully added or FALSE otherwise.

COMMENTS:
    returns FALSE also if the call info or transaction info are not valid.
=============================================================================*/
boolean SSCallDatabase_AddCall(
    SSCallDatabase          *pCallDB,
    SSCall_TInfo_type       *pTInfo,
    SSCall_Index_type       *pCallIndex,
    SSCall_TInfo_Index_type *pTInfoIndex
    );

/*=============================================================================
FUNCTION: SSCallDatabase_RemoveCall

DESCRIPTION: Removes a call record 

PARAMETERS:
    pCallDB [in]   : pointer to a valid SSCallDatabase
    callIndex [in] : call index of the call to remove

RETURN VALUE:
   TRUE if the call existed and was removed successfully or FALSE otherwise.
=============================================================================*/
boolean SSCallDatabase_RemoveCall(
    SSCallDatabase    *pCallDB, 
    SSCall_Index_type  callIndex
    );

/*=============================================================================
FUNCTION: SSCallDatabase_IsCallPresent

DESCRIPTION: Check if a call with the specified call index exists

PARAMETERS:
    pCallDB [in]   : pointer to a valid SSCallDatabase
    callIndex [in] : call index of the call to check

RETURN VALUE:
   TRUE if the call exists in the call database, FALSE otherwise.
=============================================================================*/
boolean SSCallDatabase_IsCallPresent(
    SSCallDatabase    *pCallDB, 
    SSCall_Index_type  callIndex
    );

/*=============================================================================
FUNCTION: SSCallDatabase_GetCallInfo

DESCRIPTION: Get a stored call record structure

PARAMETERS:
    pCallDB [in]   : pointer to a valid SSCallDatabase
    callIndex [in] : call index of the call to retrieve
    callInfo [out] : if callIndex was found, callInfo will point to the 
                     appropriate record in the Call Database.
RETURN VALUE:
    TRUE if an appropriate call was found or FALSE otherwise.

COMMENTS:
    if callIndex was found, callInfo points to a memory owned by the Call 
    Database.
=============================================================================*/
boolean SSCallDatabase_GetCallInfo(
    SSCallDatabase     *pCallDB,
    SSCall_Index_type   callIndex,
    SSCall_Info_type  **callInfo
    );

/*=============================================================================
FUNCTION: SSCallDatabase_GetCallCount

DESCRIPTION: Get the number of call records

PARAMETERS:
    pCallDB [in]: pointer to valid SSCallDatabase

RETURN VALUE:
    The number of allocated call records.
=============================================================================*/
uint16 SSCallDatabase_GetCallCount(SSCallDatabase *pCallDB);

/*=============================================================================
FUNCTION: SSCallDatabase_GetCallTInfoCount

DESCRIPTION: Get the number of call transaction records

PARAMETERS:
    pCallDB [in]: pointer to valid SSCallDatabase

RETURN VALUE:
    The number of allocated call transaction records.
=============================================================================*/
uint16 SSCallDatabase_GetCallTInfoCount(SSCallDatabase *pCallDB);

/*=============================================================================
FUNCTION: SSCallDatabase_RemoveCallTransaction

DESCRIPTION: Remove a call Transaction Info

PARAMETERS:
    pCallDB [in]        : pointer to valid SSCallDatabase
    callTInfoIndex [in] : call transaction info index to remove

RETURN VALUE:
    TRUE if the call transaction was removed successfully or FALSE otherwise

COMMENTS:
    If the call that contains callTInfoIndex does not have other
    transaction infos, then this call is removed from the call database.

    Note that this function does not free an allocated MO TI!
    Releasing a transaction info that contains an allocated MO TI value is 
    considered an error!. The client shall deallocate the MO TI value prior to
    calling this function.

SEE ALSO:
    SSCallDatabase_DeallocateMoTi
=============================================================================*/
boolean SSCallDatabase_RemoveCallTransaction(
    SSCallDatabase          *pCallDB,
    SSCall_TInfo_Index_type  callTInfoIndex
    );

/*=============================================================================
FUNCTION: SSCallDatabase_MergeCalls

DESCRIPTION: 
    Merge two calls: move the single transaction info index of one call (found
    using fromCallTInfoIndex) to the other (toCallIndex) and remove the first
    call, which is left without any call transactions.

PARAMETERS:
    pCallDB [in]            : pointer to valid SSCallDatabase
    fromCallTInfoIndex [in] : call transaction index to merge from
    toCallIndex [in]        : call index to merge into

RETURN VALUE:
    TRUE if the calls were merged successfully or FALSE otherwise.

COMMENTS:
    fromCallTInfoIndex must be the only call transaction in the call that
    contains it.
=============================================================================*/
boolean SSCallDatabase_MergeCalls(
    SSCallDatabase          *pCallDB,
    SSCall_TInfo_Index_type  fromCallTInfoIndex,
    SSCall_Index_type        toCallIndex  
    );

/*=============================================================================
FUNCTION: SSCallDatabase_SplitCall

DESCRIPTION: 
    Split a call into two calls by removing a single transaction info
    from one call and creating a new call into which the transaction 
    info is added.

PARAMETERS:
    pCallDB [in]        : pointer to valid SSCallDatabase
    fromCallIndex [in]  : call index to split
    tInfoIndex [in]     : transaction info index to be taken out of 
                          fromCallIndex
    pNewCallIndex [out] : call index of the newly created call.

RETURN VALUE:
    TRUE if successful, FALSE otherwise.
=============================================================================*/
boolean SSCallDatabase_SplitCall(
    SSCallDatabase          *pCallDB,
    SSCall_Index_type        fromCallIndex,
    SSCall_TInfo_Index_type  tInfoIndex,
    SSCall_Index_type       *pNewCallIndex
    );

/*=============================================================================
FUNCTION: SSCallDatabase_GetTransactionInfo

DESCRIPTION: Get a Call Transaction info by its transaction index

PARAMETERS:
    pCallDB [in]        : pointer to valid SSCallDatabase
    callTInfoIndex [in] : call transaction info index to retrieve.
    pCallTInfo [out]    : if callTInfoIndex was found, pCallTInfo will point 
                          to the appropriate record in the Call Database.

RETURN VALUE:
    TRUE if callTInfoIndex was found, FALSE otherwise.

COMMENTS:
    if callTInfoIndex was found, pCallTInfo points to a memory owned by the 
    Call Database.
=============================================================================*/
boolean SSCallDatabase_GetTransactionInfo(
    SSCallDatabase           *pCallDB,
    SSCall_TInfo_Index_type   callTInfoIndex,
    SSCall_TInfo_type       **pCallTInfo
    );

/*=============================================================================
FUNCTION: SSCallDatabase_GetCallIndexByTInfoIndex

DESCRIPTION: Get the Call Index that contains the transaction info index.

PARAMETERS:
    pCallDB [in]        : pointer to valid SSCallDatabase
    callTInfoIndex [in] : call transaction info index to retrieve
    pCallIndex [out]    : returns the call index of the call that contains
                          callTInfoIndex.

RETURN VALUE:
    TRUE if callTInfoIndex was found, FALSE otherwise.
=============================================================================*/
boolean SSCallDatabase_GetCallIndexByTInfoIndex(
    SSCallDatabase          *pCallDB,
    SSCall_TInfo_Index_type  callTInfoIndex,
    SSCall_Index_type       *pCallIndex
    );

/*=============================================================================
FUNCTION: SSCallDatabase_GetTInfoIndexByCallIndex

DESCRIPTION: Get a non MPTY call's transaction info index.
             If the call index is that of a MPTY call it returns FALSE!

PARAMETERS:
    pCallDB [in]          : pointer to valid SSCallDatabase
    callIndex [in]        : call index to retrieve
    pCallTInfoIndex [out] : returns the single call transaction info index that
                            callIndex contains.

RETURN VALUE:
    TRUE if successful, FALSE otherwise.
    returns FALSE also if the call index belongs to a MPTY call.
=============================================================================*/
boolean SSCallDatabase_GetTInfoIndexByCallIndex(
    SSCallDatabase          *pCallDB,
    SSCall_Index_type        callIndex,
    SSCall_TInfo_Index_type *pCallTInfoIndex
    );

/*=============================================================================
FUNCTION: SSCallDatabase_GetCallTInfoByCallIndex

DESCRIPTION: Get a non MPTY call's transaction info entry.
             If the call index is that of a MPTY call it returns FALSE!

PARAMETERS:
    pCallDB [in]     : pointer to valid SSCallDatabase
    callIndex [in]   : call index to retrieve
    pCallTInfo [out] : if callIndex was found valid, pCallTInfo will point 
                       to the appropriate record in the Call Database.

RETURN VALUE:
    TRUE if callIndex was found valid, FALSE otherwise.
    returns FALSE also if the call index belongs to a MPTY call.

COMMENTS:
    if callIndex was found valid, pCallTInfo points to a memory owned by the 
    Call Database.
=============================================================================*/
boolean SSCallDatabase_GetCallTInfoByCallIndex(
    SSCallDatabase     *pCallDB,
    SSCall_Index_type   callIndex,
    SSCall_TInfo_type **pCallTInfo
    );

/*=============================================================================
FUNCTION: SSCallDatabase_GetTInfoIndexByIncomingTI

DESCRIPTION:
    Get the call transaction info index by an incoming TI.

PARAMETERS:
    pCallDB [in]          : pointer to a valid SSCallDatabase
    TI [in]               : transaction id (4 bits)
    pCallTInfoIndex [out] : transaction info index that was found

RETURN VALUE:
    TRUE if an appropriate transaction info index was found, FALSE otherwise.
=============================================================================*/
boolean SSCallDatabase_GetTInfoIndexByIncomingTI(
    SSCallDatabase          *pCallDB, 
    uint8                    TI,
    SSCall_TInfo_Index_type *pCallTInfoIndex
    );

/*=============================================================================
FUNCTION: SSCallDatabase_GetOutgoingTIByTInfoIndex

DESCRIPTION:
    Get an outgoing TI according to a call transaction info index.

PARAMETERS:
    pCallDB [in]        : pointer to a valid SSCallDatabase
    callTInfoIndex [in] : transaction info index to retrieve
    TI [out]            : returns the transaction id (4 bits) that was found

RETURN VALUE:
    TRUE if callTInfoIndex was found, FALSE otherwise.
=============================================================================*/
boolean SSCallDatabase_GetOutgoingTIByTInfoIndex(
    SSCallDatabase          *pCallDB, 
    SSCall_TInfo_Index_type  callTInfoIndex,
    uint8                   *TI
    );

#endif  //__SS_CALL_DATABASE_H__
