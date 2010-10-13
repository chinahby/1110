/* =======================================================================
                              zrex_ctype.h
DESCRIPTION
  Substitutions for some ctype.h macros that are used by the
  CoreLibs but not available in zrex.
  
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

$Header: //source/qcom/qct/multimedia/qtv/utils/zrex/main/latest/inc/zrex_ctype.h#7 $
$DateTime: 2008/05/08 14:44:36 $
$Change: 656482 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */
#ifndef _ZREX_CTYPE_H_
#define _ZREX_CTYPE_H_

//zrex ltk does not have all the ctype macros available.

#include <ctype.h>

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
#ifndef isdigit
#define isdigit(c) ((c >= '0') && (c <= '9'))
#endif

#ifndef isspace
#define isspace(c) (c=='\n' || c=='\t' || c=='\v' || c==' ' || c=='\r' || c=='\f')
#endif

#ifndef isalpha
#define isalpha(a) (a >= 'A' && a <= 'z')
#endif

#ifndef toupper
#define toupper(x) UPCASE(x)
#endif


/* =======================================================================
**                        Class Declarations
** ======================================================================= */
#endif //_ZREX_CTYPE_H_



