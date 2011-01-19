#ifndef AEEQOSSPEC_H
#define AEEQOSSPEC_H
/*======================================================
FILE:  AEEQoSSpec.h

SERVICES:  BREW QoS Spec Interfaces

GENERAL DESCRIPTION:
	Interface definitions and data structures
   for BREW QoSSpec Interface

        Copyright © 1999-2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEECLSID_QOSSPEC.BID"
#include "AEE.h"
#include "AEEQoSList.h"
#include "../../inc/AEEIQoSSpec.h"

#define IQOSSPEC_AddRef           IQoSSpec_AddRef
#define IQOSSPEC_Release          IQoSSpec_Release
#define IQOSSPEC_QueryInterface   IQoSSpec_QueryInterface
#define IQOSSPEC_SetRxFlowSpec    IQoSSpec_SetRxFlowSpec
#define IQOSSPEC_SetTxFlowSpec    IQoSSpec_SetTxFlowSpec
#define IQOSSPEC_SetRxFilterSpec  IQoSSpec_SetRxFilterSpec
#define IQOSSPEC_SetTxFilterSpec  IQoSSpec_SetTxFilterSpec
#define IQOSSPEC_GetRxFlowSpec    IQoSSpec_GetRxFlowSpec
#define IQOSSPEC_GetTxFlowSpec    IQoSSpec_GetTxFlowSpec
#define IQOSSPEC_GetRxFilterSpec  IQoSSpec_GetRxFilterSpec
#define IQOSSPEC_GetTxFilterSpec  IQoSSpec_GetTxFilterSpec

#endif      // AEEQOSSPEC_H



