/*==============================================================================

                 H S U _ O S _ A B S T R A C T _ S T D L I B . C


GENERAL DESCRIPTION
  High Speed USB stack REX OS abstraction layer standard library 
  API implementation.

EXTERNALIZED FUNCTIONS
  os_memcmp
    Rex porting of the memcmp function.

  jos_memcpy
    Rex porting of the memcpy function.

  jos_memset
    Rex porting of the memset function.

  jos_strlen
    Rex porting of the strlen function.

  os_strlcpy
    String copy.

  jos_strcmp
    Rex porting of the strcmp function.

  os_snprintf
    Rex porting of the snprintf function.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/

/*==============================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/core/hsusb/driver/main/latest/src/os_abstract/rex/hsu_os_abstract_stdlib.c#2 $

when      who     what, where, why
--------  ---     --------------------------------------------------------------
05/10/06  ds      Initial version

==============================================================================*/

/*==============================================================================

                      INCLUDE FILES

==============================================================================*/

/* C runtime library include files. */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* AMSS include files. */
#include <comdef.h>

/* OS abstraction layer include files. */
#include <port_generic.h>

#if !defined(T_REXNT) && !defined(FEATURE_HS_USB_BASIC)
#include <AEEstd.h>
#endif  /* !T_REXNT && !FEATURE_HS_USB_BASIC */

/*==============================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Typedefs
==============================================================================*/


/*==============================================================================
  Constants and Macros
==============================================================================*/

#ifdef T_WINNT
#error code not present
#endif

/*==============================================================================
  Externalized Variables
==============================================================================*/

/*==============================================================================
  File Static Variables
==============================================================================*/

/*==============================================================================

                    FUNCTION DEFINITIONS

==============================================================================*/

/* ==========================================================================
FUNCTION OS_MEMCMP

DESCRIPTION
  Rex porting of the memcmp function..

DEPENDENCIES
  None.

RETURN VALUE
  Same as the standard library memcmp function.

SIDE EFFECTS
  None.
============================================================================= */
jint_t os_memcmp(const void *s1, const void *s2, juint32_t n)
{
  return memcmp(s1, s2, n);
} /* os_memcmp */


/* ==========================================================================
FUNCTION jos_memcpy

DESCRIPTION
  Rex porting of the memcpy function..

DEPENDENCIES
  None.

RETURN VALUE
  Same as the standard library memcpy function.

SIDE EFFECTS
  None.
============================================================================= */
void* jos_memcpy(void *out, const void *in, juint32_t n)
{
#ifdef FEATURE_HSU_EBI
#error code not present
#else
  return (void*)memcpy(out, in, n);
#endif
} /* jos_memcpy */


/* ==========================================================================
FUNCTION jos_memset

DESCRIPTION
  Rex porting of the memset function..

DEPENDENCIES
  None.

RETURN VALUE
  Same as the standard library memset function.

SIDE EFFECTS
  None.
============================================================================= */
void* jos_memset(void *b, jint_t c, juint32_t len)
{
  return (void*)memset(b, c, len);
} /* jos_memset */


/* ==========================================================================
FUNCTION jos_strlen

DESCRIPTION
  Rex porting of the strlen function..

DEPENDENCIES
  None.

RETURN VALUE
  Same as the standard library strlen function.

SIDE EFFECTS
  None.
============================================================================= */
juint32_t jos_strlen(const char *str)
{
  return (juint32_t)strlen(str);
} /* jos_strlen */


/* ==========================================================================
FUNCTION jos_strcmp

DESCRIPTION
   Rex porting of the strcmp function..

DEPENDENCIES
   None.

RETURN VALUE
   Same as the standard library strcmp function.

SIDE EFFECTS
   None.
============================================================================= */
jint_t jos_strcmp(const char *s1, const char *s2)
{
  return strcmp(s1, s2);
} /* jos_strcmp */


/* ==========================================================================
FUNCTION jos_strncmp

DESCRIPTION
  Rex porting of the strncmp function..

DEPENDENCIES
  None.

RETURN VALUE
  Same as the standard library strncmp function.

SIDE EFFECTS
  None.
============================================================================= */
jint_t jos_strncmp(const char *s1, const char *s2, const juint32_t n)
{
  return strncmp(s1, s2, n);
} /* jos_strncmp */


/* ==========================================================================
FUNCTION OS_STRLCPY

DESCRIPTION
  A variant of the strncpy function which guarantees
  NULL termination of the destination string.

  Copies a specified number of characters from a string.

  Guarantees null-termination of the destination string when its
  given size is not zero.

  When the size of the destination string is larger than the source string,
  the function does not zero-fill the remaining characters of the destination
  string (apart from the null-terminator character).


DEPENDENCIES
  None.

RETURN VALUE
  Returns the length of the source string.

SIDE EFFECTS
  None.
============================================================================= */
juint32_t os_strlcpy(char *dst, const char *src, juint32_t n)
{
  #if !defined(T_REXNT) && !defined(FEATURE_HS_USB_BASIC)

  /*lint -e{732} 
  ** os_strlcpy gets n as an unsigned int, while std_strlcpy 
  ** gets it as a signed int. Lint yells about this. 
  */
  return std_strlcpy(dst, src, (int)n);

  #else  /* T_REXNT || FEATURE_HS_USB_BASIC */

  uint32 i;
  juint32_t    return_value;

  if (src == NULL)
  {
    return_value = 0;
  }
  else if ((dst == NULL) || (n == 0))
  {
    return_value = (juint32_t)strlen(src);
  }
  else
  {
    i = 0;

    /* Copy at most n-1 non-NULL characters. */
    while ((*src != '\0') && (i < n - 1)) 
    {
      *dst++ = *src++;
      i++;
    }

    *dst = '\0';

    /* Run up to the end of the source in order
    ** to count its length.
    */
    while (*src++ != '\0')
    {
       i++;
    }

    return_value = (juint32_t)i;
  }

  return return_value;

  #endif  /* !T_REXNT && !FEATURE_HS_USB_BASIC */

} /* os_strlcpy */


/* ==========================================================================
FUNCTION OS_SNPRINTF

DESCRIPTION
  Rex porting of the snprintf function..

DEPENDENCIES
  None.

RETURN VALUE
  Same as the standard library snprintf function.

SIDE EFFECTS
  None.
============================================================================= */
jint32_t os_snprintf(char *buf, juint32_t size, const char *fmt, ...)
{
  int32       string_length;
  jint32_t  return_value;
  va_list   arg_list;

  #ifdef T_REXNT
#error code not present
  #endif

  /*lint -e{530}
  ** The ARM implementation of va_start raises a Lint error.
  */
  va_start(arg_list, fmt);

  string_length = vsnprintf(buf, size, fmt, arg_list);
  va_end(arg_list);

  #ifndef T_REXNT

  /* AMSS target. */

  if (string_length < 0)
  {
    /* Output error. */
    return_value = -1;
  }
  else if (string_length >= (int32)size)
  {
    /* The number of bytes required to store the string
    ** (including the NULL terminator) exceeds 'size' bytes.
    */
    return_value = -1;
  }
  else
  {
    return_value = string_length;
  }

  #else  /* #ifndef T_REXNT */
#error code not present
  #endif /* #ifndef T_REXNT */

  return return_value;
} /* os_snprintf */
