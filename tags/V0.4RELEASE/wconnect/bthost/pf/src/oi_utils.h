#ifndef _OI_UTILS_H
#define _OI_UTILS_H
/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/wconnect/bthost/pf/rel/00.00.26/src/oi_utils.h#1 $ 

$Log: $
*
*    #3        18 Oct  2006           PR
*  Merged OI Code v3.0.14rev1revQ
*
===========================================================================*/
/**
 * @file
 *
 * This file provides the interface for utility functions.
 * Among the utilities are strlen (string length), strcmp (string compare), and
 * other string manipulation functions. These are provided for those plaforms
 * where this functionality is not available in stdlib.
 */
/**********************************************************************************
  $Revision: #1 $
  Copyright 2002 - 2003 Open Interface North America, Inc. All rights reserved.
***********************************************************************************/

#include "oi_common.h"

/*
     String functions
*/     

OI_CHAR* OI_Strcpy(OI_CHAR *pDest, OI_CHAR const *pStr) ;
OI_CHAR* OI_Strcat(OI_CHAR *pDest, OI_CHAR const *pStr) ;
OI_INT OI_StrLen(OI_CHAR const *pStr) ;
OI_INT OI_Strcmp(OI_CHAR const *p1, OI_CHAR const *p2);
OI_INT OI_StrcmpInsensitive(OI_CHAR const *p1, OI_CHAR const *p2);
OI_INT OI_Strncmp(OI_CHAR const *p1, OI_CHAR const *p2, OI_UINT16 len);
OI_INT OI_StrncmpInsensitive(OI_CHAR const *p1,
                             OI_CHAR const *p2,
                             OI_UINT16 len);

/** 
 * Parse a Bluetooth device address from the specified string. 
 *
 * @param str   the string to parse
 * @param addr  the parsed address, if successful
 * 
 * @return TRUE if an address was successfully parsed, FALSE otherwise
 */

OI_BOOL OI_ParseBdAddr(OI_CHAR *str, OI_BD_ADDR *addr) ;

/** 
 * Printf() function for platforms which have no stdio or printf available.
 * OI_Printf supports the basic formatting types (no floating point)
 * Additionally, OI_Printf supports a few formats specific to BLUEmagic 3.0 software:
 *
 *  -  %! is format specifier for an OI_STATUS value.
 *  -  %: is format specifier for a Bluetooth device address; variable type is OI_BD_ADDR* .
 *  -  %@ is format specifier for an array of hex bytes; it takes two variables, hex pointer and length.
 *
 * OI_Printf output is sent to OI_Print()
 *
 */
void OI_Printf(const OI_CHAR *format, ...) ;


/** Convert a string to an integer.
 *
 * @param str  the string to parse
 *
 * @return the integer value of the string or 0 if the string could not be parsed
 */
OI_INT OI_atoi(OI_CHAR *str);


/** Get a character from a digit integer value (0 - 9). */
#define OI_DigitToChar(d) ((d) + '0')

#endif /* _OI_UTILS_H */
