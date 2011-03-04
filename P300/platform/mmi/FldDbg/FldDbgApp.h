#ifndef FLDDBGAPP_H
#define FLDDBGAPP_H

/*=============================================================================

FILE: FldDbgApp.h

SERVICES: FieldDebug Applet

GENERAL DESCRIPTION: Main applet functions

PUBLIC CLASSES AND STATIC FUNCTIONS:

INITIALIZATION AND SEQUENCING REQUIREMENTS:

(c) COPYRIGHT 2004 QUALCOMM Incorporated.
All Rights Reserved.

QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
EDIT HISTORY FOR MODULE

$Header: //depot/asic/qsc60x0/apps/FldDbg/FldDbgApp.h#1 $
$DateTime: 2006/08/08 08:56:50 $
$Author: johns $
$Change: 361045 $

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ---------------------------------------------------------
10/15/05   sada    Request & response payload defns for new diags: Get Active Applet, Get App Name & Get Selected Menu Ctl Text
12/04/04   sada    Rerouted APPGetActiveDlgIf to Field Debug App to support Brew 2.1     
10/14/04   sada    Rerouted APP_SEND_EVENT to Field Debug APP to support BREW 3.1
08/30/04   JAS     Created Field Debug Header File

=============================================================================*/
/*=============================================================================

                       INCLUDES

=============================================================================*/
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif
#include "BREWVersion.h"


/*=============================================================================

                       CONSTANTS

=============================================================================*/
// Version number for Field Debug App
#define FLDDBG_SW_VERSION "1.2"

/*=============================================================================

                       FIELD DEBUG APP USER EVENTS

=============================================================================*/
/*
 * Event values for diagnostic events
 */
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

#define MAX_RES_SIZE    64   /* Size of a resource file string. */

/*===========================================================================

                       TYPE DECLARATIONs

===========================================================================*/
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif // FEATURE_APP_TEST_AUTOMATION

#endif // FLDDBGAPP_H

