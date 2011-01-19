/* =======================================================================
                              zrex_string.cpp
DESCRIPTION
  Substitutions for Windows system calls and runtime
  library routines.  For use with the ZREX versions of the core libraries.
  
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

$Header: //source/qcom/qct/multimedia/qtv/utils/zrex/main/latest/src/zrex_string.cpp#7 $
$DateTime: 2008/05/08 14:44:36 $
$Change: 656482 $

========================================================================== */

/* ==========================================================================

                     INCLUDE FILES FOR MODULE

========================================================================== */

/* Includes custmp4.h. The following 2 includes must be the first includes in this file! */
#include "customer.h"
#include "qtvInternalDefs.h"
#include "qcplayer_oscl_utils.h"
#include "zrex_string.h"

/* ==========================================================================

                DEFINITIONS AND DECLARATIONS FOR MODULE

This section contains definitions for constants, macros, types, variables
and other items needed by this module.

========================================================================== */

/* -----------------------------------------------------------------------
** Constant / Define Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Type Declarations
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Local Object Definitions
** ----------------------------------------------------------------------- */

/* -----------------------------------------------------------------------
** Forward Declarations
** ----------------------------------------------------------------------- */

/* =======================================================================
**                            Function Definitions
** ======================================================================= */

///////////////////////////////////////////////////////////////////////////
//
// Standard String and widestring calls
//
///////////////////////////////////////////////////////////////////////////

/* ======================================================================
FUNCTION:
  zrex_strstri

DESCRIPTION:
  Case insensitive string search
  For case sensitive string search use the ANSI C <string.h> lib's strstr()

INPUT/OUPUT PARAMETERS:
  string
  strCharSet

RETURN VALUE:
  Pointer to first occurrence of strCharSet in string

SIDE EFFECTS:
  None.
========================================================================== */
char *zrex_strstri( const char *string, const char *strCharSet )
{
  int i = ZUtils::Find(string,strCharSet);
  return(i==ZUtils::npos) ? NULL : (char *)&string[i];
}

/* ======================================================================
FUNCTION:
  zrex_strupr

DESCRIPTION:
  Equivalent implementation

INPUT/OUPUT PARAMETERS:
  string

RETURN VALUE:
  Return pointer to string

SIDE EFFECTS:
  None.
========================================================================== */
char *zrex_strupr( char *string )
{
  char * upper = string;
  while ( *upper )
  {
    *upper = (char)toupper(*upper);
    upper++;
  }
  return string;
}

/* ======================================================================
FUNCTION:
  zrex_strlwr

DESCRIPTION:
  Equivalent implementation

INPUT/OUPUT PARAMETERS:
  string

RETURN VALUE:
   Returns a pointer to string

SIDE EFFECTS:
  None.
========================================================================== */
char *zrex_strlwr( char *string )
{
  char * lower = string;
  while ( *lower )
  {
    *lower = (char)tolower(*lower);
    lower++;
  }
  return string;
}
// Building the LTK will cause a conflict with zrex_wcscpy() defined in LIBCMT
#ifndef PLATFORM_LTK
/* ======================================================================
FUNCTION:
  zrex_wcscpy

DESCRIPTION:
  Equivalent implementation

INPUT/OUPUT PARAMETERS:
  strDestination
  strSource

RETURN VALUE:
  Returns pointer to strDestination

SIDE EFFECTS:
  None.
========================================================================== */
/*lint -e129 -e10 */
wchar_t *zrex_wcscpy( wchar_t *strDestination, const wchar_t *strSource )
{
  for (int i=0; ;i++)
  {
    strDestination[i] = strSource[i];
    if (strSource[i]==0)
    {
      break;
    }
  }
  return strDestination;
}
/*lint +e129 +e10 */
#endif /* PLATFORM_LTK */
/* ======================================================================
FUNCTION:
  zrex_wcslen

DESCRIPTION:
  Equivalent implementation

INPUT/OUPUT PARAMETERS:
  string

RETURN VALUE:
  Return length of string
  
SIDE EFFECTS:
  None.
========================================================================== */
/*lint -e10 -e49 -e40 */
size_t zrex_wcslen( const wchar_t *str )
{
  for (int i=0; ;i++)
  {
    if (str[i] == 0)
    {
      return i;
    }
  }
}
/*lint +e10 +e49 +e40 */

///////////////////////////////////////////////////////////////////////////
//
// Windows multi-byte support
//
///////////////////////////////////////////////////////////////////////////
/* ======================================================================
FUNCTION:
  CharToWideChar

DESCRIPTION:
  limited substitute for MultiByteToWideChar

INPUT/OUPUT PARAMETERS:
  src
  nSrc
  dest
  nDest

RETURN VALUE:
  String length

SIDE EFFECTS:
  None.
========================================================================== */

/*lint -e40 -e601 -e10 -e530 */
int CharToWideChar(
                  const char * src, // address of string to map
                  int nSrc,      // number of bytes in string
                  wchar_t * dest,  // address of wide-character buffer
                  int nDest        // size of buffer
                  )
{
  if (nDest==0)
  {
    //return size of required output buffer
    return strlen(src)+1;
  }
  else
  {
    //see how many chars to convert
    int n = (nSrc<0) ? strlen(src)+1 : nSrc;
    int count = 0;
    for (int i=0;i<n;i++)
    {
      if (i<nDest)
      {
        //convert to unsigned first to avoid sign-extension.
        dest[i]=(wchar_t)((unsigned char)src[i]);
        count++;
      }
    }

    dest[nDest - 1] = (wchar_t) 0;

    return count;
  }
}
/*lint +e40 +e601 +e10 +e530 */

/* ======================================================================
FUNCTION:
  WideCharToChar

DESCRIPTION:
  limited substitute for WideCharToMultiByte

INPUT/OUPUT PARAMETERS:
  src
  nSrc
  dest
  nDest

RETURN VALUE:
  Return string length

SIDE EFFECTS:
  None.
========================================================================== */
/*lint -e49 -e40 -e10 -e63 */
int WideCharToChar(
                  const wchar_t * src, // address of string to map
                  int nSrc,      // number of bytes in string
                  char * dest,  // address of wide-character buffer
                  int nDest        // size of buffer
                  )
{
  if (nDest==0)
  {
    //return size of required output buffer
    return zrex_wcslen(src)+1;
  }
  else
  {
    //see how many chars to convert
    int n = (nSrc<0) ? zrex_wcslen(src)+1 : nSrc;
    int count = 0;
    for (int i=0;i<n;i++)
    {
      if (i<nDest)
      {
        //truncate
        dest[i]=(char)src[i];
        count++;
      }
    }

    dest[nDest - 1] = '\0';

    return count;
  }
}
/*lint +e49 +e40 +e10 +e63 */

