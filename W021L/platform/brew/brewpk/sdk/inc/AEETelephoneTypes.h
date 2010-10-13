#ifndef AEETELEPHONE_TYPES_H
#define AEETELEPHONE_TYPES_H
/*======================================================
FILE:  AEETelephoneTypes.h

SERVICES:

GENERAL DESCRIPTION:
   Definitions for constants and types used by AEETelephone.h

        Copyright © 2004 - 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

#include "../../inc/AEETelDef.h"
#include "AEE.h"
#include "AEEModel.h"
#include "AEETSuppsTypes.h"
#include "AEEWebOpts.h"

/*===========================================================================
                         T Y P E S
===========================================================================*/
/* Interface typedefines */
typedef struct ITelephone ITelephone;
typedef struct IPhoneNotifier IPhoneNotifier;
typedef struct ICall ICall;
typedef struct IMultipartyCall IMultipartyCall;
typedef struct ICallMgr ICallMgr;
typedef struct ICallOrigOpts ICallOrigOpts;
typedef struct IPhoneCtl IPhoneCtl;
typedef struct ISuppsTrans ISuppsTrans;

typedef WebOpt AEETOriginateOpt;

/*
=======================================================================
  DATA STRUCTURE DOCUMENTATION
======================================================================= 
AEETOriginateOpt

Description:
   Holds an (id, value) pair for AEETOriginateOpt.

Definition:
typedef WebOpt AEETOriginateOpt;

Members:
   int32  nId: option id, chosen from one of the Origination options
   void *pVal: option value, depends on nId

Comments: 
   None

See Also:
   None
=======================================================================
*/
#endif /* AEETELEPHONE_TYPES_H */
