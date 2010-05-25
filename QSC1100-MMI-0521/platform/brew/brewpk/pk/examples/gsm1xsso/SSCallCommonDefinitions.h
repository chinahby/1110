#ifndef __SS_CALL_COMMON_DEFINITIONS_H__
#define __SS_CALL_COMMON_DEFINITIONS_H__

/*=========================================================================
FILE: SSCallCommonDefinitions.h

SERVICES: Call related definitions common to GSM1x SS Operations.

PUBLIC CLASSES AND STATIC FUNCTIONS:
    SSCall_Info_type
    SSCall_TInfo_type

                  (c) Copyright 2000-2003 QUALCOMM Incorporated.
                  All Rights Reserved.
                  QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===========================================================================
                              INCLUDE FILES
===========================================================================*/

#include "AEE.h"
#include "AEEComdef.h"
#include "SS_CommDefs.h"

/*===========================================================================
                    DEFINITIONS AND CONSTANTS
===========================================================================*/

// max length of call string
#define MAX_SIZE_DIALER_TEXT 62
// max length of phone book match for call string
#define MAX_SIZE_NAME_TEXT 62
// max number of transactions per call
#define MAX_CALL_TRANSACTION_INFO_NUMBER 5

// transaction identifier macros
#define GET_TI_FLAG(TI) ((uint8)((TI & 0x08) >> 3))
#define GET_TI_VALUE(TI) ((uint8)(TI & 0x07))
#define COMPOSE_TI(flag, value) (uint8)((flag << 3) | value)

/*===========================================================================
                              TYPE DECLARATIONS
===========================================================================*/

// Transaction Id value type definition
typedef uint8 SSCall_TIValue_type;

// Transaction info index type definition
typedef uint8 SSCall_TInfo_Index_type;

// Call Index type definition
typedef uint8 SSCall_Index_type;

// Presentation Indicator type definition
#define SS_PI_ALLOWED     0
#define SS_PI_RESTRICTED  1
#define SS_PI_UNAVAILABLE 2

typedef int SSCall_PI_type;

// Call Status
#define SS_CALL_STATUS_SETUP        0
#define SS_CALL_STATUS_CONNECTED    1
#define SS_CALL_STATUS_ACTIVE       2
#define SS_CALL_STATUS_DISCONNECTED 3

typedef int  SSCall_Status_type;

// Call origination type definition
#define SSCall_MO 0
#define SSCall_MT 1

typedef int SSCall_Origination_type;

// Call Information structure definition
typedef struct
{
    boolean            isMPTY;  // is it a MPTY call ?
    boolean            isHeld;  // is it a held call ?
    uint32             MPTYStartTime;   // only for a MPTY call

    // Transactions indices array for this call
    SSCall_TInfo_Index_type 
                       transactionIndices[MAX_CALL_TRANSACTION_INFO_NUMBER];

    // The number of valid transaction indices, starting from the
    // the begining of the transactionIndices array.
    uint8              numTransactions;

} SSCall_Info_type;

// Transaction Information structure definition
typedef struct
{
    SSCall_Origination_type callOrigType;
    SSCall_TIValue_type     TIValue;

    SSCall_Status_type      callStatus;

    // Full state of the call, to be used by the MN and the CC.
    SSCall_State_type       callState;
    
    // The call's displayed number
    AECHAR                  callString[MAX_SIZE_DIALER_TEXT]; 

    // Presentation indicator for callString
    SSCall_PI_type          callStringPI;

    // Phone book match of callString
    AECHAR                  callName[MAX_SIZE_NAME_TEXT];

    uint32                  callStartTime;

} SSCall_TInfo_type;

#endif  //__SS_CALL_COMMON_DEFINITIONS_H__
