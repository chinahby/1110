/*===========================================================================

FILE: OEMDiag.c

SERVICES: IDiag interface implementation

DESCRIPTION
   This file implements BREW IDiag. IDiag facilitates application diagnostics.

PUBLIC CLASSES:  Not Applicable

INITIALIZATION AND SEQUENCING REQUIREMENTS:  Not Applicable

        Copyright © 2004-2006 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/
/*===========================================================================
                        EDIT HISTORY FOR MODULE

$Header: //depot/asic/msmshared/apps/StaticExtensions/OEM/Src/OEMDiag.c#24 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/03/06   jas     Fixing lint directives
12/08/05   jas     Reordered header file for L4 builds
09/13/05   jas     Change mem allocs to use small heap whenever possible
06/03/05   cvs     Fixing L4 Build issues
05/24/05   jas     Fixing include file case for Linux builds.
03/10/05   cvs     Fix RVCT compile errors
01/06/05   jas     Correcting a memory leak.
12/04/04   sada    Rerouted APP_GetActiveDlg to Field Debug APP to support BREW 3.1
11/24/04   sada    Changed payload size intialization in GetRequestPayload()
10/25/04   sada    Adding Get_DiagVersion function
10/20/04   sada    Rerouted APP_SEND_EVENT to Field Debug APP to support BREW 3.1
10/14/04   jas     Fix to init req data ptrs to NULL when not used.
09/09/04   jas     Rerouting most diags to Field Debug app to support BREW 3.1
08/04/04   jas     Adding fix for OEMDIAG_STOP in BREW 3.0.x
05/07/04   sun     Initial Revision
===========================================================================*/
/*===========================================================================
 *
 * FILE ARRANGEMENT:  This file is divided into several sections.  Below
 * is an outline of the file to aid in navigating and maintaining the code.
 *
 * SECTION 1: #includes and local definitions
 * SECTION 2: Diagnostic registration table functions/Other utility functions
 * SECTION 3: Diagnostic request handlers and handler registration table
 * SECTION 4: Implementation of IDiag interface methods
 *
 ==========================================================================*/

/*===========================================================================
 *
 * SECTION 1: #includes and local definitions
 *
 ==========================================================================*/
//lint -save -e611 -e740 Suppress Unusual/Suspicious cast lint errors
//lint -save -e717 Suppress do..while(0) lint errors

#include "customer.h"
#ifdef FEATURE_APP_TEST_AUTOMATION
#error code not present
#endif /* FEATURE_APP_TEST_AUTOMATION */
