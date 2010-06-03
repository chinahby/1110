#ifndef _OI_VARSTRING_H
#define _OI_VARSTRING_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_varstring.h#1 $ 

$Log: $

===========================================================================*/
/** @file   
  simple variable-length string package

  A variable-length string is represented by an OI_VARSTRING that keeps track of the maximum and current
  size of a string buffer.
  
  Functions are provided for appending strings, and integer values in
  hex or decimal formats to the string as well as some other convenience functions.

  Unlike C strings, the string buffer in a var string can contain embedded null characters. A
  helper function is provided for converting a var string to a C string.

*/
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include <stdarg.h>

#include "oi_stddefs.h"
#include "oi_status.h"


/**
 * VarStrings are either managed or unamanged. A managed VarString allocates
 * it's own buffer and will reallocate a new buffer (freeing the old one) as
 * required to grow the string. The buffer in an unmanaged VarString is
 * allocated by the caller.  Unmanaged VarStrings are truncated when the string
 * reaches the maximum length, which is one less than MaxLen to allow for null
 * termination.
 */

typedef struct {
    OI_BOOL Managed;   /**< Is string buffer managed by VarString */
    OI_UINT16 MaxLen;  /**< Size of buffer */
    OI_UINT16 Len;     /**< Current length of string */
    OI_CHAR* Buffer;   /**< Character buffer */
} OI_VARSTRING;


/**
 * Allocate and initialize an empty managed VarString.
 */

OI_STATUS OI_VStrAlloc(OI_VARSTRING *Vstr,
                      OI_UINT16 size);


/**
 * Free the string buffer for a managed VarString.
 */

void OI_VStrFree(OI_VARSTRING *Vstr);


/**
 * Append a null terminated C string to a VarString. The null is not appended.
 */

OI_STATUS OI_VStrCat(OI_VARSTRING *VStr,
                 const OI_CHAR *Str);


/**
 * Append N characters to a VarString. Any embedded nulls in the source string
 * will be appended.
 */

OI_STATUS OI_VStrnCat(OI_VARSTRING *VStr,
                  const OI_CHAR *Str,
                  OI_UINT16 N);

/**
 * Append white space to a VarString
 */

OI_STATUS OI_VSpaceCat(OI_VARSTRING *VStr,
                      OI_UINT8 Count);

/**
 * Append a hex integer string to a VarString
 *
 * Size is the number of the significant bytes in the integer. The hex value will
 * be written as (2 * Size) bytes. Leading zeroes are always written.
 */

OI_STATUS OI_VHexCat(OI_VARSTRING *VStr,
                 OI_UINT32 Val,
                 OI_UINT Size);


/**
 * 
 * Append a signed decimal integer string to a VarString.
 */

OI_STATUS OI_VDecCat(OI_VARSTRING *VStr,
                 OI_INT32 Val);

/**
 * OI_VFormatStr
 *
 */

OI_STATUS OI_VFormatStr(OI_VARSTRING *VStr,
                        const OI_CHAR* format,
                        va_list argp);

/**
 * sprintf-like formatting into a VarString.
 */

OI_STATUS OI_FormatStr(OI_VARSTRING *VStr,
                       const OI_CHAR *format,
                       ...);

/**
 * Get null terminated string from a var string. This appends a final null to the
 * string buffer and returns a pointer to the buffer.
 */

OI_CHAR* OI_VStrGetString(OI_VARSTRING *VStr);



/*****************************************************************************/
#endif /* _OI_VARSTRING_H */
