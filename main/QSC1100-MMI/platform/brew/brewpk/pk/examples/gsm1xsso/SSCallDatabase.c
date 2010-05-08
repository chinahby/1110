/*=========================================================================
FILE: SSCallDatabase.c

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

#include "SSCallDatabase.h"
#include "SS_CommonMacros.h"

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/

#define TESTING

// check the validity of a TI value. TIValue is assumed to be unsigned
#define CHECK_TI_VALUE_RANGE(TIValue)                                               \
    {                                                                               \
        if ((TIValue) >= NUMBER_OF_MO_TI_VALUES )         \
        {                                                                           \
            assert(!"TI value is not in range");                                    \
            return FALSE;                                                           \
        }                                                                           \
    }

// check the validity of a call index. Index is assumed to be unsigned
#define CHECK_CALL_INDEX_RANGE(Index)                                               \
    {                                                                               \
        if ( (Index) >= MAX_DATABASE_CALL_ENTRIES )          \
        {                                                                           \
            assert(!"Call index is not in range");                                  \
            return FALSE;                                                           \
        }                                                                           \
    }

// check the validity of a call transaction index. Index is assumed to be 
// unsigned
#define CHECK_TRANSACTION_INFO_RANGE(Index)                                         \
    {                                                                               \
        if ( (Index) >= MAX_DATABASE_TRANSACTION_ENTRIES )   \
        {                                                                           \
            assert(!"Transaction Info index is not in range");                      \
            return FALSE;                                                           \
        }                                                                           \
    }

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

/*===========================================================================
                              FUNCTION DECLARATIONS
===========================================================================*/

static boolean SSCallDatabase_FindFreeCall(
    SSCallDatabase    *pCallDB,
    SSCall_Index_type *callIndex
    );

static boolean SSCallDatabase_IsCallTransactionInfoValid(
    SSCallDatabase    *pCallDB,
    SSCall_TInfo_type *pTinfo
    );

static boolean SSCallDatabase_FindFreeCallTInfo(
    SSCallDatabase          *pCallDB,
    SSCall_TInfo_Index_type *callTInfoIndex
    );

static boolean SSCallDatabase_IsTransactionPresent(
    SSCallDatabase          *pCallDB, 
    SSCall_TInfo_Index_type  callTInfoIndex
    );

static boolean SSCallDatabase_RemoveCallTInfoEntries(
    SSCallDatabase    *pCallDB,
    SSCall_Index_type  callIndex
    );

static boolean SSCallDatabase_RemoveCallTransactionIndexFromCall(
    SSCallDatabase          *pCallDB,
    SSCall_Index_type        callIndex,
    SSCall_TInfo_Index_type  callTInfoIndex
    );

static boolean SSCallDatabase_GetCallTInfoIndex(
    SSCallDatabase          *pCallDB,
    SSCall_Origination_type  callOrigination,
    SSCall_TIValue_type      tiValue, 
    SSCall_TInfo_Index_type *pCallTInfoIndex
    );

static boolean SSCallDatabase_AddCallUsingAnAllocatedTInfo(
    SSCallDatabase          *pCallDB,
    SSCall_TInfo_Index_type  tInfoIndex,
    SSCall_Index_type       *pCallIndex
    );

/*===========================================================================
                              LOCAL/STATIC DATA
===========================================================================*/

/*===============================================================================
FUNCTION DEFINITIONS
================================================================================*/

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
SSCallDatabase* SSCallDatabase_New(IShell* pIShell)
{
    SSCallDatabase *pObj;

    pObj = MALLOC(sizeof(SSCallDatabase));
    if (pObj == NULL)
        return NULL;

    SSCallDatabase_Reset(pObj);

    pObj->m_pShell = pIShell;
    ISHELL_AddRef(pIShell);

    return pObj;
}

/*=============================================================================
FUNCTION: SSCallDatabase_Release

DESCRIPTION: Destroy an SSCallDatabase instance

PARAMETERS:
   pCallDB [in]: pointer to a valid SSCallDatabase to be destroyed

RETURN VALUE:
    None
=============================================================================*/
void SSCallDatabase_Release(SSCallDatabase *pCallDB)
{
#ifdef TESTING
    uint8              moIndex, tIndex, callIndex;
    boolean            found;
    SSCall_TInfo_type *pCallTInfo;
    SSCall_Info_type  *pCallInfo;

    for (moIndex=0; moIndex < NUMBER_OF_MO_TI_VALUES; ++moIndex)
    {
        if (pCallDB->isMoTiAllocated[moIndex] == TRUE)
        {
            found = FALSE;

            for (tIndex=0; tIndex < MAX_DATABASE_TRANSACTION_ENTRIES; ++tIndex)
            {
                if (pCallDB->isTransactionAllocated[tIndex])
                {
                    pCallTInfo = &pCallDB->transactions[tIndex];

                    if ( (pCallTInfo->callOrigType == SSCall_MO) &&
                         (pCallTInfo->TIValue == moIndex) )
                    {
                        found = TRUE;
                        break;
                    }
                }
            }

            if (!found) assert(!"a free floating allocated MO TI found");
        }
    }

    for (tIndex=0; tIndex < MAX_DATABASE_TRANSACTION_ENTRIES; ++tIndex)
    {
        if (pCallDB->isTransactionAllocated[tIndex] == TRUE)
        {
            found = FALSE;

            for (callIndex=0; callIndex < MAX_DATABASE_CALL_ENTRIES; ++callIndex)
            {
                uint8 tArrayIndex;

                if (pCallDB->isCallAllocated[callIndex])
                {
                    pCallInfo = &pCallDB->calls[callIndex];

                    for (tArrayIndex=0; tArrayIndex < pCallInfo->numTransactions; ++tArrayIndex)
                    {
                        if ( pCallInfo->transactionIndices[tArrayIndex] == tIndex)
                        {
                            found = TRUE;
                            break;
                        }
                    }
                }

                if (found) break;
            }

            if (!found) assert(!"a free floating transaction info found");
        }
    }
#endif // TESTING 

    ISHELL_Release(pCallDB->m_pShell);
    FREE(pCallDB);
}

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
    )
{
    SSCall_TIValue_type searchIndex;

    // search for an un-allocated TI value 
    for(searchIndex=0; searchIndex < NUMBER_OF_MO_TI_VALUES; ++searchIndex)
    {
        if (pCallDB->isMoTiAllocated[searchIndex] == FALSE) 
        {
            pCallDB->isMoTiAllocated[searchIndex] = TRUE;
            (*pTIValue) = searchIndex;
            return TRUE;
        }
    }

    return FALSE;
}

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
    )
{
    CHECK_TI_VALUE_RANGE(tiValue);

    if (pCallDB->isMoTiAllocated[tiValue] == TRUE)
    {
        pCallDB->isMoTiAllocated[tiValue] = FALSE;
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

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
    )
{
    CHECK_TI_VALUE_RANGE(tiValue);
    
    return pCallDB->isMoTiAllocated[tiValue];
}

/*=============================================================================
FUNCTION: SSCallDatabase_GetAllocatedMoTiCount

DESCRIPTION: 
    Get the number of allocated Mobile originated Transaction Id values.

PARAMETERS:
   pCallDB [in]: pointer to valid SSCallDatabase

RETURN VALUE:
    The number of allocated MO TI values.
=============================================================================*/
uint16 SSCallDatabase_GetAllocatedMoTiCount(SSCallDatabase *pCallDB)
{
    uint16 index;
    uint16 moTiCount=0;

    for (index=0; index < NUMBER_OF_MO_TI_VALUES; ++index)
    {
        if (pCallDB->isMoTiAllocated[index]) ++moTiCount;
    }

    return moTiCount;
}

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
    )
{
    boolean                  success;
    SSCall_TInfo_Index_type  callTInfoIndex;
    SSCall_Index_type        callIndex;

    assert(pCallDB != NULL);
    assert(pTInfo != NULL);

    if (SSCallDatabase_IsCallTransactionInfoValid(pCallDB, pTInfo) == FALSE)
        return FALSE;

    if (SSCallDatabase_FindFreeCallTInfo(pCallDB, &callTInfoIndex) == FALSE)
        return FALSE;

    // copy call transaction info into free call transaction info entry
    MEMCPY(
       &(pCallDB->transactions[callTInfoIndex]), 
       pTInfo, 
       sizeof(pCallDB->transactions[0])
       );

    success = SSCallDatabase_AddCallUsingAnAllocatedTInfo(
                 pCallDB, 
                 callTInfoIndex,
                 &callIndex
                 );

    if (success)
    {
        // mark call transaction info as allocated
        pCallDB->isTransactionAllocated[callTInfoIndex] = TRUE;

        if (pCallIndex != NULL) 
        {
            (*pCallIndex) = callIndex;
        }
        if (pTInfoIndex != NULL) 
        {
            (*pTInfoIndex) = callTInfoIndex;
        }
    }

    return success;
}

/*=============================================================================
FUNCTION: SSCallDatabase_AddCallUsingAnAllocatedTInfo

DESCRIPTION: 
    Utility function to add a new call record, using a previously allocated 
    call transaction info entry.

PARAMETERS:
    pCallDB [in]     : pointer to valid SSCallDatabase
    tInfoIndex [in]  : call transaction info index
    pCallIndex [out] : allocated call index

RETURN VALUE:
    TRUE if call was added, FALSE if an error occurred.

COMMENTS:
    returns FALSE if the call database is full.
=============================================================================*/
boolean SSCallDatabase_AddCallUsingAnAllocatedTInfo(
    SSCallDatabase          *pCallDB,
    SSCall_TInfo_Index_type  tInfoIndex,
    SSCall_Index_type       *pCallIndex
    )
{
    SSCall_Info_type *pAllocatedCallInfo;

    if (SSCallDatabase_FindFreeCall(pCallDB, pCallIndex) == FALSE) 
        return FALSE;

    pAllocatedCallInfo = &(pCallDB->calls[(*pCallIndex)]);

    // set the first transaction info index value
    pAllocatedCallInfo->transactionIndices[0] = tInfoIndex;

    // set the transactions number
    pAllocatedCallInfo->numTransactions = 1;

    // init MPTY data to a non MPTY call
    pAllocatedCallInfo->isMPTY = FALSE;
    pAllocatedCallInfo->isHeld = FALSE;
    pAllocatedCallInfo->MPTYStartTime = 0;

    // mark call as allocated
    pCallDB->isCallAllocated[(*pCallIndex)] = TRUE;

    return TRUE;
}

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
    )
{
    assert(pCallDB != NULL);
    CHECK_CALL_INDEX_RANGE(callIndex);

    if (pCallDB->isCallAllocated[callIndex] == TRUE)
    {
        pCallDB->isCallAllocated[callIndex] = FALSE;
        return SSCallDatabase_RemoveCallTInfoEntries(pCallDB, callIndex);
    }
    else 
    {
        return FALSE;
    }
}

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
    )
{
   assert(pCallDB != NULL);
   CHECK_CALL_INDEX_RANGE(callIndex);

   if (pCallDB->isCallAllocated[callIndex])
   {
      // sanity check
      assert((pCallDB->calls[callIndex].numTransactions) > 0);
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

/*=============================================================================
FUNCTION: SSCallDatabase_IsTransactionPresent

DESCRIPTION: 
    Utility function to check if a transaction info with the specified index
    exists.

PARAMETERS:
    pCallDB [in]        : pointer to a valid SSCallDatabase
    callTInfoIndex [in] : transaction info index to check

RETURN VALUE:
   TRUE if the transaction exists, FALSE otherwise.
=============================================================================*/
boolean SSCallDatabase_IsTransactionPresent(
    SSCallDatabase          *pCallDB, 
    SSCall_TInfo_Index_type  callTInfoIndex
    )
{
   assert(pCallDB != NULL);
   CHECK_TRANSACTION_INFO_RANGE(callTInfoIndex);

   if (pCallDB->isTransactionAllocated[callTInfoIndex])
   {
      return TRUE;
   }
   else
   {
      return FALSE;
   }
}

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
    )
{
    if (SSCallDatabase_IsCallPresent(pCallDB, callIndex))
    {
        (*callInfo) = &pCallDB->calls[callIndex]; 
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
FUNCTION: SSCallDatabase_GetCallCount

DESCRIPTION: Get the number of call records

PARAMETERS:
    pCallDB [in]: pointer to valid SSCallDatabase

RETURN VALUE:
    The number of allocated call records.
=============================================================================*/
uint16 SSCallDatabase_GetCallCount(SSCallDatabase *pCallDB)
{
    uint16 index;
    uint16 callCount=0;

    for (index=0; index < MAX_DATABASE_CALL_ENTRIES; ++index)
    {
        if (pCallDB->isCallAllocated[index]) ++callCount;
    }

    return callCount;
}

/*=============================================================================
FUNCTION: SSCallDatabase_GetCallTInfoCount

DESCRIPTION: Get the number of call transaction records

PARAMETERS:
    pCallDB [in]: pointer to valid SSCallDatabase

RETURN VALUE:
    The number of allocated call transaction records.
=============================================================================*/
uint16 SSCallDatabase_GetCallTInfoCount(SSCallDatabase *pCallDB)
{
    uint16 index;
    uint16 tInfoCount=0;

    for (index=0; index < MAX_DATABASE_TRANSACTION_ENTRIES; ++index)
    {
        if (pCallDB->isTransactionAllocated[index]) ++tInfoCount;
    }

    return tInfoCount;
}

/*=============================================================================
FUNCTION: SSCallDatabase_Reset

DESCRIPTION: clears the Call database data

PARAMETERS:
   pCallDB [in]: pointer to a valid SSCallDatabase

RETURN VALUE:
    None
=============================================================================*/
void SSCallDatabase_Reset(SSCallDatabase *pCallDB)
{
    uint16 index;

#ifdef TESTING
    {
        IShell* pIShell = pCallDB->m_pShell;
        MEMSET(pCallDB, 0xff, sizeof(SSCallDatabase));
        pCallDB->m_pShell = pIShell;
    }
#endif

    for (index=0; index < MAX_DATABASE_CALL_ENTRIES; ++index)
    {
        pCallDB->isCallAllocated[index] = FALSE;
    }

    for (index=0; index < MAX_DATABASE_TRANSACTION_ENTRIES; ++index)
    {
        pCallDB->isTransactionAllocated[index] = FALSE;
    }

    for (index=0; index < NUMBER_OF_MO_TI_VALUES; ++index)
    {
        pCallDB->isMoTiAllocated[index] = FALSE;
    }
}

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
    )
{
    boolean           success;
    SSCall_Index_type callIndex;

    // find the call transaction
    success = SSCallDatabase_GetCallIndexByTInfoIndex(
                 pCallDB,
                 callTInfoIndex,
                 &callIndex
                 );

    if (success == TRUE)
    {
        // remove the transaction from the call. this will also remove the
        // call if this was the only transaction for this call.
        success = SSCallDatabase_RemoveCallTransactionIndexFromCall(
                     pCallDB,
                     callIndex,
                     callTInfoIndex
                     );

        assert(success == TRUE);
        pCallDB->isTransactionAllocated[callTInfoIndex] = FALSE;
    }

    return success;
}

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
    )
{
    SSCall_Info_type *pFromCallEntry;
    SSCall_Info_type *pToCallEntry;
    SSCall_Index_type fromCallIndex;
    uint8             toArrayStartingIndex;
    uint8             arrayIndex;
    boolean           success;

    if (SSCallDatabase_IsCallPresent(pCallDB, toCallIndex) == FALSE)
        return FALSE;

    success = SSCallDatabase_GetCallIndexByTInfoIndex(
        pCallDB,
        fromCallTInfoIndex,
        &fromCallIndex
        );

    if (!success) return FALSE;

    if (fromCallIndex == toCallIndex)
    {
        assert(!"Can not merge the same call to itself");
        return FALSE;
    }

    pToCallEntry = &(pCallDB->calls[toCallIndex]);
    pFromCallEntry = &(pCallDB->calls[fromCallIndex]);

    if (pFromCallEntry->numTransactions != 1)
    {
        assert(!"Merge from a call with a wrong number of transactions");
        return FALSE;
    }

    if (pToCallEntry->numTransactions + pFromCallEntry->numTransactions > 
            MAX_CALL_TRANSACTION_INFO_NUMBER)
    {
        assert(!"Too many transactions in merged call");
        return FALSE;
    }

    toArrayStartingIndex = pToCallEntry->numTransactions;

    // copy transactions from "pFromCallEntry" to "pToCallEntry"
    for (arrayIndex=0; arrayIndex < pFromCallEntry->numTransactions; ++arrayIndex)
    {
        pToCallEntry->transactionIndices[arrayIndex + toArrayStartingIndex] = 
            pFromCallEntry->transactionIndices[arrayIndex];
    }

    pToCallEntry->numTransactions += pFromCallEntry->numTransactions;
    pFromCallEntry->numTransactions = 0;

    success = SSCallDatabase_RemoveCall(pCallDB, fromCallIndex);

    if (success)
    {
        if (pToCallEntry->isMPTY == FALSE)
        {
            pToCallEntry->isMPTY = TRUE;
            pToCallEntry->MPTYStartTime = ISHELL_GetUpTimeMS(pCallDB->m_pShell);
        }

        pToCallEntry->isHeld = FALSE;
    }

    return success;
}

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
    )
{
    boolean success;
    assert(pNewCallIndex != NULL);

    success = SSCallDatabase_RemoveCallTransactionIndexFromCall(
                 pCallDB,
                 fromCallIndex,
                 tInfoIndex
                 );

    if (success)
    {   
        success = SSCallDatabase_AddCallUsingAnAllocatedTInfo(
                     pCallDB, 
                     tInfoIndex,
                     pNewCallIndex
                     );
    }
    
    return success;
}

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
    )
{
    CHECK_TRANSACTION_INFO_RANGE(callTInfoIndex);

    if (pCallDB->isTransactionAllocated[callTInfoIndex] == TRUE) 
    {
        (*pCallTInfo) = &(pCallDB->transactions[callTInfoIndex]);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
FUNCTION: SSCallDatabase_FindFreeCall

DESCRIPTION: Find a free Call Index

PARAMETERS:
    pCallDB [in]     : pointer to valid SSCallDatabase
    pCallIndex [out] : returns a free call index

RETURN VALUE:
    TRUE if a free call index was found, FALSE otherwise.

COMMENTS:
    This function doesn't change the status of the free call index.
=============================================================================*/
boolean SSCallDatabase_FindFreeCall(
    SSCallDatabase    *pCallDB,
    SSCall_Index_type *pCallIndex
    )
{
    SSCall_Index_type searchIndex;

    // search for an un-allocated call
    for(searchIndex=0; searchIndex < MAX_DATABASE_CALL_ENTRIES; ++searchIndex)
    {
        if (pCallDB->isCallAllocated[searchIndex] == FALSE) 
        {
            (*pCallIndex) = searchIndex;
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
FUNCTION: SSCallDatabase_FindFreeCallTInfo

DESCRIPTION: Find a free Call transaction info Index

PARAMETERS:
    pCallDB [in]          : pointer to valid SSCallDatabase
    pCallTInfoIndex [out] : returns a free call transaction info index

RETURN VALUE:
    TRUE if a free call transaction index was found, FALSE otherwise.

COMMENTS:
    This function doesn't change the status of the free call transaction index.
=============================================================================*/
boolean SSCallDatabase_FindFreeCallTInfo(
    SSCallDatabase          *pCallDB,
    SSCall_TInfo_Index_type *pCallTInfoIndex
    )
{
    SSCall_TInfo_Index_type searchIndex;

    // search for an un-allocated Tinfo
    for (searchIndex=0; searchIndex < MAX_DATABASE_TRANSACTION_ENTRIES; ++searchIndex)
    {
        if (pCallDB->isTransactionAllocated[searchIndex] == FALSE) 
        {
            (*pCallTInfoIndex) = searchIndex;
            return TRUE;
        }
    }
    return FALSE;
}

/*=============================================================================
FUNCTION: SSCallDatabase_RemoveCallTInfoEntries

DESCRIPTION: 
    Utility function to free all call's associated transaction info entries

PARAMETERS:
    pCallDB [in]   : pointer to valid SSCallDatabase
    callIndex [in] : call index to free all its call transactions

RETURN VALUE:
    TRUE if successful, FALSE otherwise.

COMMENTS:
    This function only marks the appropriate call transactions as free. It
    doesn't remove them from the call record.
=============================================================================*/
boolean SSCallDatabase_RemoveCallTInfoEntries(
    SSCallDatabase    *pCallDB,
    SSCall_Index_type  callIndex
    )
{
   SSCall_Info_type       *pCallEntry = &(pCallDB->calls[callIndex]);
   uint8                   numTransactions = pCallEntry->numTransactions;
   uint8                   arrayIndex;
   SSCall_TInfo_Index_type tInfoIndex;

   for (arrayIndex=0; arrayIndex < numTransactions; ++arrayIndex)
   {
       tInfoIndex = pCallEntry->transactionIndices[arrayIndex];
       CHECK_TRANSACTION_INFO_RANGE(tInfoIndex);

       // sanity check
       if (pCallDB->isTransactionAllocated[tInfoIndex])
       {
           pCallDB->isTransactionAllocated[tInfoIndex] = FALSE;
       }
       else
       {
           assert(!"Transaction Info index not allocated!");
           return FALSE;
       }
   }

   return TRUE;
}

/*=============================================================================
FUNCTION: SSCallDatabase_RemoveCallTransactionIndexFromCall

DESCRIPTION: 
    Remove a call transaction Info index from its call, does not change the 
    allocated transaction info itself. If a call has no transactions left
    it gets removed from the Call Database.

PARAMETERS:
    pCallDB [in]        : pointer to valid SSCallDatabase
    callIndex [in]      : call index to remove the transaction from.
    callTInfoIndex [in] : call transaction info index to remove.

RETURN VALUE:
    TRUE if successful, FALSE otherwise

COMMENTS:
    SSCallDatabase_RemoveCall
=============================================================================*/
boolean SSCallDatabase_RemoveCallTransactionIndexFromCall(
    SSCallDatabase          *pCallDB,
    SSCall_Index_type        callIndex,
    SSCall_TInfo_Index_type  callTInfoIndex
    )
{
    SSCall_Info_type *pCallEntry;
    uint8             arrayIndex;
    boolean           isTinfoFound = FALSE;
    boolean           moveArrayEntryBack = FALSE;

    if (SSCallDatabase_IsTransactionPresent(pCallDB, callTInfoIndex) == FALSE)
        return FALSE;

    if (SSCallDatabase_IsCallPresent(pCallDB, callIndex) == FALSE)
        return FALSE;

    pCallEntry = &(pCallDB->calls[callIndex]);

    // update the call record 
    for (arrayIndex=0; arrayIndex < pCallEntry->numTransactions; ++arrayIndex)
    {
        if (pCallEntry->transactionIndices[arrayIndex] == callTInfoIndex)
        {
            isTinfoFound = TRUE;
            moveArrayEntryBack = TRUE;
            continue;
        }

        if (moveArrayEntryBack)
        {
            // shift down all other transactionIndices
            pCallEntry->transactionIndices[arrayIndex-1] = 
                pCallEntry->transactionIndices[arrayIndex];
        }
    }

    if (isTinfoFound == TRUE)
    {
        assert(moveArrayEntryBack == TRUE);
        pCallEntry->numTransactions -= 1;

        // if a call has no transaction left, remove it
        if (pCallEntry->numTransactions == 0)
        {
            SSCallDatabase_RemoveCall(pCallDB, callIndex);
        }
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

/*=============================================================================
FUNCTION: SSCallDatabase_IsCallTransactionInfoValid

DESCRIPTION: Checks that a call transaction info's data is valid 

PARAMETERS:
    pCallDB [in] : SSCallDatabase structure pointer   
    pTinfo [in]  : call transaction info to check

RETURN VALUE:
    TRUE if valid, FALSE otherwise.
=============================================================================*/
boolean SSCallDatabase_IsCallTransactionInfoValid(
    SSCallDatabase    *pCallDB,
    SSCall_TInfo_type *pTinfo
    )
{
    switch (pTinfo->callOrigType)
    {
        case SSCall_MO:
            return SSCallDatabase_IsMoTiAllocated(pCallDB, pTinfo->TIValue);
            break;

        case SSCall_MT:
            return TRUE;
            break;

        default:
            assert(!"bad call transaction info value");
    }

    return FALSE;
}

/*=============================================================================
FUNCTION: SSCallDatabase_GetCallTInfoIndex

DESCRIPTION:
   Get call transaction info index by the call type (MO/MT) and the TI value.
   
PARAMETERS:
    pCallDB [in]: pointer to valid SSCallDatabase
    callOrigination [in]: call origination type (MO/MT)
    tiValue [in]: TI value (without the TI flag) to find
    pCallTInfoIndex [out]: returns the call transaction info index 

RETURN VALUE:
    TRUE if an appropriate call transaction info was found, FALSE otherwise.
=============================================================================*/
boolean SSCallDatabase_GetCallTInfoIndex(
    SSCallDatabase          *pCallDB,
    SSCall_Origination_type  callOrigination,
    SSCall_TIValue_type      tiValue, 
    SSCall_TInfo_Index_type *pCallTInfoIndex
    )
{
    SSCall_Index_type  callIndex;
    uint8              arrayIndex;
    uint8              numCallTransactions;
    boolean            success;
    SSCall_Info_type  *pCallInfo;
    SSCall_TInfo_type *pCallTinfo;

    // iterate through all calls
    for (callIndex=0; callIndex < MAX_DATABASE_CALL_ENTRIES; ++callIndex)
    {
        if (SSCallDatabase_GetCallInfo(pCallDB, callIndex, &pCallInfo) == FALSE)
            continue;

        numCallTransactions = pCallInfo->numTransactions;

        // iterate through all call's call transactions
        for (arrayIndex=0; arrayIndex < numCallTransactions; ++arrayIndex)
        {
            success = SSCallDatabase_GetTransactionInfo(
                            pCallDB,
                            pCallInfo->transactionIndices[arrayIndex],
                            &pCallTinfo
                            );

            if (!success)
            {
                assert(!"bad call information detected");
                return FALSE;
            }

            if ((pCallTinfo->callOrigType == callOrigination) &&
                (pCallTinfo->TIValue == tiValue))
            {
                *pCallTInfoIndex = pCallInfo->transactionIndices[arrayIndex];
                return TRUE;
            }
        }
    }
    return FALSE;
}

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
    )
{
    SSCall_Index_type  callIndex;
    uint8              arrayIndex;
    uint8              numCallTransactions;
    SSCall_Info_type  *pCallInfo;

    if (SSCallDatabase_IsTransactionPresent(pCallDB, callTInfoIndex) == FALSE)
        return FALSE;

    // iterate through all calls
    for (callIndex=0; callIndex < MAX_DATABASE_CALL_ENTRIES; ++callIndex)
    {
        if (SSCallDatabase_GetCallInfo(pCallDB, callIndex, &pCallInfo) == FALSE)
            continue;

        numCallTransactions = pCallInfo->numTransactions;

        // iterate through all call's call transactions
        for (arrayIndex=0; arrayIndex < numCallTransactions; ++arrayIndex)
        {
            if (callTInfoIndex == pCallInfo->transactionIndices[arrayIndex])
            {
                *pCallIndex = callIndex;
                return TRUE;
            }
        }
    }
    return FALSE;
}

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
    )
{
    boolean success = 
        SSCallDatabase_IsCallPresent(pCallDB, callIndex);

    if (success)
    {
        SSCall_Info_type *pCallInfo = &pCallDB->calls[callIndex];

        if ((pCallInfo->isMPTY == FALSE) && (pCallInfo->numTransactions == 1))
        {
            (*pCallTInfoIndex) = pCallInfo->transactionIndices[0];
            success = TRUE;
        }
        else
        {
            success = FALSE;
        }
    }

    return success;
}

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
    )
{
    SSCall_TInfo_Index_type callTInfoIndex;

    boolean success = SSCallDatabase_GetTInfoIndexByCallIndex(
                        pCallDB,
                        callIndex,
                        &callTInfoIndex
                        );
    if (success)
    {
        (*pCallTInfo) = &(pCallDB->transactions[callTInfoIndex]);
    }

    return success;
}

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
    )
{
    SSCall_Origination_type callType;
    int tiFlag = GET_TI_FLAG(TI);
    
    // incoming TI => if TI flag is 1 then the transaction was allocated 
    // by us, meaning that this is an MO call
    callType = ((tiFlag == 1) ? SSCall_MO : SSCall_MT);
    
    return SSCallDatabase_GetCallTInfoIndex(
                pCallDB, 
                callType, 
                GET_TI_VALUE(TI), 
                pCallTInfoIndex
                );
}

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
    )
{
    SSCall_TInfo_type *pCallTinfo;
    uint8              tiFlag;
    boolean            success;

    success = SSCallDatabase_GetTransactionInfo(
                    pCallDB,
                    callTInfoIndex,
                    &pCallTinfo
                    );
    if (success)
    {    
        tiFlag = (uint8)((pCallTinfo->callOrigType == SSCall_MO) ? 0 : 1);
        (*TI) = COMPOSE_TI(tiFlag, pCallTinfo->TIValue);
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

