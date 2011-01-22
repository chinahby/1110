/*===========================================================================

FILE: QcamDiag.c

GENERAL DESCRIPTION
    Automation test shared functions.  

    Copyright (c) 2004 - 2005 by QUALCOMM, Incorporated.  All Rights Reserved.

===========================================================================*/

/*============================================================================

                      EDIT HISTORY FOR FILE

 This section contains comments describing changes made to this file.
 Notice that changes are listed in reverse chronological order.

 $Header: //source/qcom/qct/multimedia/camera/apps/qcamcommon/main/latest/src/QcamDiag.c#1 $ $DateTime: 2008/05/12 10:09:09 $ $Author: karenp $

 when      who    what, where, why
 --------  -----  ----------------------------------------------------------
 09/15/05  NC     Lint errors clean up.
 08/25/05  NC     Added checking for code security.
 07/29/05  NC     Update for SDK simulate.
 04/13/05  NC     Added message logs and simplify some codes.
 04/08/05  NC     Added status reports for failure cases.
 02/16/05  NC     Added #ifdef FEATURE_APP_QCAMERA condition.
 12/10/04  NC     Update and fix bugs.
 12/01/04  NC     Init file for DIAG helper functions 
============================================================================*/
/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */
#include "QcamDiag.h"
#ifdef AEE_SIMULATOR
#include "QcamSim.h"
#else
#include "msg.h"
#endif
#ifdef FEATURE_APP_QCAMERA
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif //#ifdef FEATURE_APP_TEST_AUTOMATION
#endif //#ifdef FEATURE_APP_QCAMERA
