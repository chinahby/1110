/*==============================================================================

		            H S U _ O S _ A B S T R A C T _ G E N E R A L . C

  
GENERAL DESCRIPTION

  Implementation of general purpose High-Speed USB 
  OS abstraction layer functions.
	
EXTERNALIZED FUNCTIONS

  os_printf
    Print a string.

  os_panic
    Print a string and halt the stack execution.

  os_dprintf
    Print a string to debug console.


INITIALIZATION AND SEQUENCING REQUIREMENTS
  None.
		
Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
==============================================================================*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/09/06   ds      Created.

===========================================================================*/

/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

/* C-Runtime library include files. */
#include <stdio.h>
#include <stdarg.h>

/* DMSS include files. */
#ifndef FEATURE_HS_USB_BASIC
#include <err.h>
#endif
#include <msg.h>

/* Regional include file */
#include <hsu_common.h>
#include <port_generic.h>

/* Local include files. */

/*==============================================================================

                    DEFINITIONS AND DECLARATIONS FOR MODULE

==============================================================================*/

/*==============================================================================
  Constants and Macros
==============================================================================*/

#define HSU_OS_MAX_MESSAGE_LENGTH  HSU_MAX_MSG_LEN

#ifdef T_WINNT
#error code not present
#endif

#ifdef FEATURE_HS_USB_BASIC
#undef ERR_FATAL
#define ERR_FATAL(x,a,b,c)  while (1);
#endif /* FEATURE_HS_USB_BASIC */

/*==============================================================================
  Typedefs
==============================================================================*/


/*==============================================================================
  Externalized Variables
==============================================================================*/


/*==============================================================================
  File Static Variables
==============================================================================*/


/*==============================================================================

                    FUNCTION DEFINITIONS

==============================================================================*/

/*===========================================================================
FUNCTION OS_PRINTF

DESCRIPTION
  Print string to console/serial.
  If the resultant output string size is longer than
  HSU_OS_MAX_MESSAGE_LENGTH bytes, a truncated string is printed.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void os_printf
(
  /* Format string. */
  const char *fmt, 

  /* List of format string arguments. */
  ...
)
{
  static char printed_message[HSU_OS_MAX_MESSAGE_LENGTH];

  va_list arg_list;

  /* Ensure NULL termination. */
  printed_message[sizeof(printed_message) - 1] = '\0';

  /*lint -e{530}
  ** The ARM implementation of va_start raises Lint errors.
  */
  va_start(arg_list, fmt);

  (void)vsnprintf(printed_message,
                  sizeof(printed_message) - 1,
                  fmt,
                  arg_list);

  va_end(arg_list);

  HSU_MSG_HIGH(printed_message, 0, 0, 0);

} /* os_printf */


/*===========================================================================
FUNCTION OS_PANIC

DESCRIPTION
  Print string and halt stack execution.
  If the resultant output string size is longer than
  HSU_OS_MAX_MESSAGE_LENGTH bytes, a truncated string is printed.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void os_panic
(
  /* Format string. */
  const char *fmt,

  /* List of format string arguments. */
  ...
)
{
  int32        result;
  static char  error_message[HSU_OS_MAX_MESSAGE_LENGTH];

  va_list arg_list;


  /* Ensure NULL termination */
  error_message[sizeof(error_message) - 1] = '\0';

  /*lint -e{530}  
  ** The ARM implementation of va_start raises Lint errors.
  */
  va_start(arg_list, fmt);

  result = vsnprintf(error_message,
                     sizeof(error_message) - 1,
                     fmt,
                     arg_list);

  va_end(arg_list);

  if (result < 0)
  {
    /* Under RexNT, it's not possible to distinguish whether
    ** _vsnprintf returned -1 because it really failed or because 
    ** the input string was too long and was truncated. So, under
    ** RexNT we do noting in this case.
    */
    #ifndef T_REXNT

    /* vsnprintf returned in error */
    HSU_MSG_ERROR("os_panic Error - vsnprintf function failure. Return value: %d", 
      result, 0, 0);
    HSU_ASSERT (FALSE);

    #endif
  }
  else
  {
    HSU_MSG_ERROR (error_message, 0, 0, 0);
    HSU_ASSERT (FALSE);
  }
} /* os_panic */


/*===========================================================================
FUNCTION OS_DPRINTF

DESCRIPTION
  Print a string to debug console.
  Same implementation as os_printf.
  However, since the function gets a variable argument list,
  it is not possible to defer the call to os_prinf, 
  and therefore the function needs to be implemented again.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void os_dprintf(const char *fmt, ...)
{
  int32        result;
  static char  debug_message[HSU_OS_MAX_MESSAGE_LENGTH];

  va_list arg_list;


  /* Ensure NULL termination */
  debug_message[sizeof(debug_message) - 1] = '\0';

  /*lint -e{530}
  ** The ARM implementation of va_start raises a Lint error.
  */
  va_start(arg_list, fmt);

  result = vsnprintf(debug_message,
                     sizeof(debug_message) - 1,
                     fmt,
                     arg_list);

  va_end(arg_list);

  if (result < 0)
  {
    /* Under RexNT, it's not possible to distinguish whether
    ** _vsnprintf returned -1 because it really failed or because 
    ** the input string was too long and was truncated. So, under
    ** RexNT we do noting in this case.
    */
    #ifndef T_REXNT
    os_panic("os_dprintf Error - vsnprintf function failure.");
    #endif
  }
  else
  {
    HSU_MSG_HIGH(debug_message, 0, 0, 0);
  }
} /* os_dprintf */
