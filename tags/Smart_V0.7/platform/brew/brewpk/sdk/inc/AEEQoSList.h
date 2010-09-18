#ifndef AEEQOSLIST_H
#define AEEQOSLIST_H
/*======================================================
FILE:  AEEQoSList.h

SERVICES:  BREW QoS List Interface.

GENERAL DESCRIPTION:
   Interface definition of Quality of service List.

        Copyright © 1999-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "AEECLSID_QOSLIST.BID"
#include "AEE.h"
#include "../../inc/AEEIQoSList.h"

#define IQOSLIST_AddRef          IQoSList_AddRef
#define IQOSLIST_Release         IQoSList_Release
#define IQOSLIST_QueryInterface  IQoSList_QueryInterface
#define IQOSLIST_Add             IQoSList_Add
#define IQOSLIST_Remove          IQoSList_Remove
#define IQOSLIST_Size            IQoSList_Size
#define IQOSLIST_AddAt           IQoSList_AddAt
#define IQOSLIST_RemoveAt        IQoSList_RemoveAt
#define IQOSLIST_GetAt           IQoSList_GetAt
#define IQOSLIST_Find            IQoSList_Find
#define IQOSLIST_Clear           IQoSList_Clear

#endif      // AEEQOSLIST_H
