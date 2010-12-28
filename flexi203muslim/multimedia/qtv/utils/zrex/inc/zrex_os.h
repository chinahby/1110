/* =======================================================================
                              zrex_os.h
DESCRIPTION
  Includes the zrex OS files

EXTERNALIZED FUNCTIONS
  List functions and a brief description that are exported from this file

INITIALIZATION AND SEQUENCING REQUIREMENTS
  Detail how to initialize and use this service.  The sequencing aspect
  is only needed if the order of operations is important.

Portions copyrighted by PacketVideo Corporation; 
Copyright 1998, 2002, 2003 PacketVideo Corporation, All Rights Reserved; and
Portions copyrighted by QUALCOMM Incorporated; 
Copyright 2003 QUALCOMM Incorporated, All Rights Reserved
========================================================================== */
/* =======================================================================
                             Edit History

$Header: //source/qcom/qct/multimedia/qtv/utils/zrex/main/latest/inc/zrex_os.h#7 $
$DateTime: 2008/05/08 14:44:36 $
$Change: 656482 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

#ifndef _FEATURE_QTV_ZREX_H_
#define _FEATURE_QTV_ZREX_H_


/* Standard header files must before zrex files else we run
   into compiling problems! */
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stddef.h>

/* Zrex OS file */
/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "zrex.h"

/* additional ANSI stuff not present in zrex */
#include "zrex_ctype.h"


/* ==========================================================================

                        DATA DECLARATIONS

========================================================================== */
/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Global Constant Data Declarations 
** ----------------------------------------------------------------------- */
/* -----------------------------------------------------------------------
** Global Data Declarations
** ----------------------------------------------------------------------- */
/* =======================================================================
**                          Macro Definitions
** ======================================================================= */
/* =======================================================================
**                        Class Declarations
** ======================================================================= */

#endif //_FEATURE_QTV_ZREX_H_
