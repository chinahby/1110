/* =======================================================================
                              zrex_string.h
DESCRIPTION
  Substitutions for Windows system calls and runtime
  library routines.  For use with the ZREX versions
  of the core libraries.
  
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

$Header: //source/qcom/qct/multimedia/qtv/utils/zrex/main/latest/inc/zrex_string.h#7 $
$DateTime: 2008/05/08 14:44:36 $
$Change: 656482 $

========================================================================== */

/* =======================================================================
**               Includes and Public Data Declarations
** ======================================================================= */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

#ifndef _ZREX_STRING_H_
#define _ZREX_STRING_H_


/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "zrex_os.h"


#ifdef __cplusplus
extern "C" {
#endif



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
/*
///////////////////////////////////////////////////////////////////////////
//
// Standard String and widestring calls
//
///////////////////////////////////////////////////////////////////////////
*/

char *zrex_strstri( const char *string, const char *strCharSet );
char *zrex_strupr(char * string);
char *zrex_strlwr(char * string);
wchar_t *zrex_wcscpy( wchar_t *strDestination, const wchar_t *strSource );

size_t zrex_wcslen( const wchar_t *string );

/*
//////////////////////////////////////////////////////////////////////////////////
//
// Windows WideChar support calls.  
//
//////////////////////////////////////////////////////////////////////////////////
*/

/* 
** Redefine the multi-byte calls in the core libs.
** This is only limited support.
*/

int CharToWideChar(const char * src, int nSrc, wchar_t * dest, int nDest );
#define MultiByteToWideChar(a,b,c,d,e,f) CharToWideChar(c,d,e,f)

int WideCharToChar(const wchar_t * src, int nSrc, char * dest, int nDest );
#define WideCharToMultiByte(a,b,c,d,e,f,g,h) WideCharToChar(c,d,e,f)


#ifdef __cplusplus
}
#endif

#endif //_ZREX_STRING_H_

