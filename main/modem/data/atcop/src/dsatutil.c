/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S

                        A T   C O M M A N D 
                        
                        U T I L I T Y   S E R V I C E S

GENERAL DESCRIPTION
  This module contains utility functions that are used in executing/processing
  AT commands.

EXTERNALIZED FUNCTIONS

  dsatutil_fmt_num_val
    This function formats the result of a value inquiry "?" for a normal
    numeric variable.  The return string is a null terminated 3 - digit
    decimal number with leading zeros.

  dsatutil_fmt_num_val_ex
    This function formats the value of an extended format numeric
    variable.  The value may be formatted as the result of a READ
    "?" command, or in the form of a SET command for use in uploading
    the configuration.

  dsatutil_fmt_mixed_param_val
    This function formats the values of a mixed parameter.
    The values may be formatted as the result of a READ "?" command,
    or in the form of a SET command for use in uploading the configuration.

  dsatutil_fmt_num_range
    This function formats the result of a range inquiry "=?" command
    for a numeric variable.

  dsatutil_fmt_string_range
    This function formats the result of a range inquiry "=?" for a string
    variable.

  dsatutil_fmt_string_val
    This function formats the value of an extended format string
    variable.  The value may be formatted as the result of a READ
    "?" command, or in the form of a SET command for use in uploading
    the configuration.

  dsatutil_put_num_arg
    This function checks the value and number of numeric arguments for
    a command, and, if they are correct, stores the value(s) in the
    parameter.

  dsatutil_put_string_arg
    This checks an extended format string WRITE command for correct
    syntax and string length.

  dsatutil_itoa
    This function converts an integer to ASCII, using a specified
    radix. The resulting string is null terminated.

  dsatutil_atoi
    This function converts an ASCII string to an integer, using a specified
    radix.  Spaces are ignored. Letters used for radices above 10 may be of
    either case.

  dsatutil_strcmp_ig_sp_case
    This function compares two null terminated strings, ignoring case.

  dsatutil_strip_quotes_out
    This function strips the quotes out from the quoted_str, the results
    are in out_str as a null-terminated string.  str_len is the max length 
    of the out_str string, the applocated space for out_str.

  dsatutil_format_ipaddr
    This function checks for the validity of the IP address and converts it
    to and from internal storage format.
    It verifies the length of the IP address and also verifies if all
    the 4 tuples, when converted to int, are below 255.

  dsatutil_reset_password
    Clear the password variable or specified length.
    The password string is set to 0xFF, null terminated.  

  dsatutil_append_dsm_item
    This function checks the buffer length against the passed threshold
    value.  If the threshold is exceeded, a new DSM item is requested and
    appended to the buffer chain.  An error is generated if no dsm_items
    are available.

  dsatutil_validate_dial_char
    This function examines the passed dial string charater and returns a
    validation result.  It is up to the caller to act upon the result as
    approppriate within its context.

  dsatutil_srch_list
    This function searchs a list of strings, and returns the index
    to the item found.
      
INITIALIZATION AND SEQUENCING REQUIREMENTS


   Copyright (c) 2001 - 2011 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$PVCSPath: L:/src/asw/MM_DATA/vcs/dsatutil.c_v   1.7   11 Nov 2002 14:02:48   sbandaru  $
$Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatutil.c#4 $ $DateTime: 2011/04/20 04:28:48 $ $Author: c_avijay $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/20/11   dvk     Fixed compilation issues.
01/02/09   bs      Added support for 1X AT Phonebook commands.
04/11/08   bs      Fixed Lint High's.
10/29/07   sn      Replaced banned string APIs.
12/04/06   squ     fixed a lint warning
11/01/06   an      Featurize ATCOP, RMSM, Async Fax
08/09/06   az      Fixed compiler issues (check in for Anuj)
12/25/03   rsl     Changes relating to Unrec. cmds not working.
04/17/03   sb      Allow 0-1 options in *98 dial string  and 0-3 options in 
                   *99 dial string.
04/02/03   ar      Correct null parameter handling for BROKEN_RANGE attribute
03/26/03   ar      Fix formatting in diagnostic messages
03/24/03   ar      Enforce cid option position on packet dial string
03/17/03   ar      Correct test response formatting for BROKEN_RANGE logic.
03/07/03   ar      Remove FEATURE_DATA_ETSI_SUPSERV wrappers
02/20/03   wx      Return (x) instead of (x-x) for test command when a
                   command's numerical range is only one value
02/20/03   wx      Accept leading 0's for numerical arguments
01/24.03   ar      Migrated context cmds to persistent PDP profile support 
01/20/03   ar      Wrapped dsat_cgdcont_contexts under ETSI Packet FEATURE
01/17/03   wx      Added dsatutil_fill_modifier_info()
12/31/02   wx      Add function dsatutil_strcmp_pkt_dial_str and 
                   dsatutil_check_opt_dial_str
01/07/03   wx      Import dsat707_pkt_orig_str
11/11/02   sb      Definition of dsatutil_srch_list() function
09/18/02   jay     Removed casting to word in calls to itoa.
09/09/02   ar      Added dsatutil_validate_dial_char routine
06/27/02   ar      Added dsatutil_append_dsm_item routine
02/15/02   ar      Added dsatutil_format_ipaddr routine
10/09/01   wx      Modify function dsatutil_put_string_list_arg so that it
                   supports double quoted arguments
08/10/01   sjd     Initial release to MSM5200 archives.
05/15/01   sb      Created module.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA
#include "AEEstd.h"
#include "dsati.h"
#include "dstaski.h"
#include "ps_utils.h"
#include "dsatvoice.h"

#ifndef FEATURE_DATA_STRIP_ATCOP
#include "err.h"
#include "msg.h"
#include "assert.h"
#include <string.h>
#include <stdio.h>

#if defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS)
#error code not present
#endif /* defined(FEATURE_DATA_WCDMA_PS) || defined(FEATURE_GSM_GPRS */

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*-------------------------------------------------------------------------
            Import External Variables
-------------------------------------------------------------------------*/
#ifdef FEATURE_CDMA
extern byte dsat707_pkt_orig_str[];
#endif /* FEATURE_CDMA */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

/*-------------------------------------------------------------------------
            Local Defines
-------------------------------------------------------------------------*/
rex_timer_type dsatvoice_ring_timer;
#define N_DIAL_STRS 2

/*-------------------------------------
  Table to convert IRA characters to GSM 7 bit default alphabets
  Refer to 3G TS 27.005 Annex A for more details                 
---------------------------------------*/
const uint8 ira_to_gsm7[128] =
{
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255,  10, 255, 255,  13, 255, 255,
  255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
  32,  33,  34,  35,   2,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
  32,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90, 255, 255, 255, 255,  17,
  255,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
  112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 255, 255, 255, 255, 255
};

/*-------------------------------------
  Table to convert GSM 7 bit default alphabets to IRA characters 
---------------------------------------*/
const uint8 gsm7_to_ira[128] =
{
  64, 255,  36, 255, 255, 255, 255, 255, 255, 255,  10, 255, 255,  13, 255, 255,
  255,  95, 255, 255, 255, 255, 255, 255, 255, 255, 255, 32, 255, 255, 255, 255,
  32,  33,  34,  35, 255,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
  48,  49,  50,  51,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61,  62,  63,
  255,  65,  66,  67,  68,  69,  70,  71,  72,  73,  74,  75,  76,  77,  78,  79,
  80,  81,  82,  83,  84,  85,  86,  87,  88,  89,  90, 255, 255, 255, 255, 255,
  255,  97,  98,  99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111,
  112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 255, 255, 255, 255, 255
};


/* Replacement character (space) specified by 23.038 */
#define REPLACEMENT_CHAR 0x20
/* Escape character to enter GSM alphabet extension table of 23.038 */
#define GSM_EXT_TBL_ESC_CHAR 0x1B
/* minimum and maximum acceptable value for characters in these alphabets:
   0 value cannot be accepted for NULL terminated strings used throughout
   ATCOP and 0x7F is max possible in 7 bit alphabet */
#define MIN_IRA_GSM_CHAR 0x01
#define MAX_IRA_GSM_CHAR 0x7F
#define MAX_8BIT_CHAR    0xFF

const char hex_chars[] = "0123456789ABCDEF";

/* Characters in the "C0 control and Basic Latin" code chart of UTF-16 that 
   aren't common to IRA T.50(C0+G0) and GSM 7bit alphabets */
const char basic_latin_not_common[] = 
{
  0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0B,0x0C,0x0E,0x0F,0x10,0x11,0x12,
  0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x1B,0x1C,0x1D,0x1E,0x1F,0x24,0x40,0x5B,
  0x5C,0x5D,0x5E,0x5F,0x60,0x7B,0x7C,0x7D,0x7E,0x7F,0x00
};

/* Complementary arrays for converting between PBM 8bit and UCS2 hex 4-tuples:
   for those values that aren't common in the range supported by both */
const char pbm8_to_ucs2_not_common[] = 
  "\x90\x92\x93\x94\x95\x96\x97\x98\x99\x9A";
const byte ucs2_to_pbm8_not_common[][8] = 
{
  "0394", "03A6", "0393", "039B", "03A9", 
  "03A0", "03A8", "03A3", "0398", "039E",
  ""
};
 
const byte ucs2_to_pbm8_not_supported[][8] =
{
  "0090", "0092", "0093", "0094", "0095", 
  "0096", "0097", "0098", "0099", "009A",
  ""
};

const char pbm8_to_gsm7_exttbl[] = "\x5B\x5C\x5D\x5E\x7B\x7C\x7D\x7E";
const char gsm7_to_pbm8_exttbl[] = "\x3C\x2F\x3E\x14\x28\x40\x29\x3D";

/*---------------------------------------------------------------------------
     Operating system timers
---------------------------------------------------------------------------*/
LOCAL void dsatutil_timer_cb(unsigned long timer_id);

dsat_timer_s_type dsatutil_timer_table[] =
{
  {DSAT_TIMER_VOICE_RING,
   &dsatvoice_ring_timer, dsatutil_timer_cb,
   dsatvoice_timer_expired_handler,
   0, FALSE, FALSE}  /* Duration varies, set in cmd handler */

  ,{DSAT_TIMER_FREE_ITEM,
   &dsati_free_item_timer, dsatutil_timer_cb,
   dsati_free_item_timer_expired_handler,
   DSAT_FREE_ITEM_DLY, FALSE, FALSE}  
};

/*-------------------------------------------------------------------------
            Local Variables
-------------------------------------------------------------------------*/

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/
LOCAL byte * itoa_zfill
(
  word v,
  byte *rb_ptr
);

LOCAL action_quote_e_type determine_quote_action(uint32 attrib);
nv_stat_enum_type dsatutil_put_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
);

nv_stat_enum_type dsatutil_get_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
);

LOCAL void pbm_char_to_gsm
(
  char pbm_in, 
  char **gsm_out
);

LOCAL void gsm_char_to_pbm
(
  char **gsm_in, 
  char *pbm_out
);

/*===========================================================================

FUNCTION DSATUTIL_FMT_NUM_VAL

DESCRIPTION
  This function formats the result of a value inquiry "?" for a normal
  numeric variable.  The return string is a null terminated 3 - digit
  decimal number with leading zeros.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
unsigned int dsatutil_fmt_num_val
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table  */
  byte *rb_ptr                          /*  point to buffer for result  */
)
{
  unsigned int result = 0;

  /* Abort formatting if no display attribute set */
  if ((tab_entry->attrib & NO_DISPLAY) == NO_DISPLAY)
  {
    *rb_ptr = '\0';
  }
  else
  {
    result = (unsigned int)( itoa_zfill((word)(0xFF &
                                               *((dsat_num_item_type*)tab_entry->val_ptr)),rb_ptr
                                       ) - rb_ptr );
  }
  return result;
} /*  dsatutil_fmt_num_val  */

/*===========================================================================

FUNCTION DSATUTIL_FMT_NUM_VAL_EX

DESCRIPTION
  This function formats the value of an extended format numeric
  variable.  The value may be formatted as the result of a READ
  "?" command, or in the form of a SET command for use in uploading
  the configuration.  For the READ command, the returned string is
  of the form:

      +NAME: <value>                            for a simple variable, or
      +NAME: <value1>, <value2>, ... , <valueN> for a compound variable.

  For the SET command, it is in the form

      +NAME=<value>;                            for a simple variable, or
      +NAME=<value1>, <value2>, ... , <valueN>; for a compound variable.


  The value will be in decimal, unless the parameter is specified with
  a HEX attribute in the parse table entry.  Leading zeros are suppressed.

  ".0" is added to the result for +FCLASS=2.0

  The formatted value is returned as a null terminated string.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
unsigned int dsatutil_fmt_num_val_ex
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table  */
  byte *rb_ptr,                         /*  point to buffer for result  */
  fmt_enum_type fmt_sw                  /*  selects response or set fmt */
)
{
  unsigned int i, j;
  boolean print_name;
  uint16 radix;
  byte c, *w_ptr;
  const byte *s, *l_ptr;
  boolean skip_fmt = FALSE;

  w_ptr = rb_ptr;
  radix = ( (tab_entry->attrib & HEX) ? 16 : 10);

  print_name = ( ((tab_entry->attrib & FCLASS2P0) == 0)
        && ((tab_entry->special) != SPECIAL_FCLASS))
      || fmt_sw == SET
      || fmt_sw == RESPONSE_ALL;

#if 0
  if (tab_entry->attrib & EXTENDED) /*  Since this function is also */
  {                               /*  used to SET CONFIG for the  */
    *w_ptr++ = '+';                 /*  S-registers.                */
  }
#endif /* if 0 */

  if (print_name)
    {
      s = tab_entry->name;

      while ( (c = *s++) !=0 )
        {
          *w_ptr++ = c;
        }
    }

  switch (fmt_sw)
    {
      case RESPONSE:
      case RESPONSE_ALL:
        if (print_name)
        {
          *w_ptr++ = ':';
          *w_ptr++ = ' ';
        }
        /* Abort further formatting if no display attribute set */
        if ((tab_entry->attrib & NO_DISPLAY) == NO_DISPLAY)
        {
          skip_fmt = TRUE;
        }
        break;

    case SET:
        *w_ptr++ = '=';
        break;

    default:
        /*  ERROR */
        ;
    }

  if (!skip_fmt)
  {
    if ((tab_entry->attrib & LIST) != 0)
    {
      for (j=0; j < tab_entry->compound; ++j)
      {
        i = ((dsat_num_item_type*)tab_entry->val_ptr)[j];
        l_ptr = ((def_list_type *) tab_entry->def_lim_ptr)[j].list_v[i];
      
        if(tab_entry->attrib & YES_QUOTE)
        {
          *w_ptr++ = '"';
        }

        while ( (c = *l_ptr++) != '\0')
        {
          *w_ptr++ = c;
        }

        if(tab_entry->attrib & YES_QUOTE)
        {
          *w_ptr++ = '"';
        }
        *w_ptr++ = ',';
      }
      --w_ptr;
    }
    else if (tab_entry->compound > 1)
    {
      for (j=0; j < tab_entry->compound; ++j)
      {
        w_ptr = dsatutil_itoa(((dsat_num_item_type*)tab_entry->val_ptr)[j],
                              w_ptr,
                              radix );
        *w_ptr++ = ',';
      }
      --w_ptr;
    }
    else
    {
      w_ptr = dsatutil_itoa(*(dsat_num_item_type*)tab_entry->val_ptr,
                            w_ptr,
                            radix );
    }
  
    if ( (tab_entry->special) == SPECIAL_FCLASS
        && *((dsat_num_item_type*)tab_entry->val_ptr) == 2)
    {
      *w_ptr++ = '.';
      *w_ptr++ = '0';
    }

    if (fmt_sw == SET && tab_entry->attrib & EXTENDED)
    {
      *w_ptr++ = ';';
    }
  }
  *w_ptr = '\0';

  return (unsigned int) (w_ptr - rb_ptr);
} /*  dsatutil_fmt_num_val_ex */

/*===========================================================================

FUNCTION DSATUTIL_FMT_NUM_RANGE

DESCRIPTION
  This function formats the result of a range inquiry "=?" command
  for a numeric variable.

  The formatted value is returned as a null terminated string.
DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.


SIDE EFFECTS

===========================================================================*/
unsigned int dsatutil_fmt_num_range
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table  */
  byte *rb_ptr                          /*  point to buffer for result  */
)
{
  unsigned int j;
  unsigned int offset = 0;
  word radix;
  byte c, *w_ptr;
  const byte *r_ptr, *s;
  uint32 brtest = 0;

  w_ptr = rb_ptr;

  radix = ( (tab_entry->attrib & HEX) ? 16 : 10);

  s = tab_entry->name;

  if ((tab_entry->attrib & FCLASS2P0) == 0)
  {
    while ( (c = *s++) !=0 )
    {
      *w_ptr++ = c;
    }

    *w_ptr++ = ':';
    *w_ptr++ = ' ';
  }

  if ( (tab_entry->attrib & LIST) == 0)
  {
    /* Loop over number of parameters */
    for (j=0; j < tab_entry->compound; ++j)
    {
      brtest = (tab_entry->attrib & BROKEN_RANGE);
      *w_ptr++ = '(';

      /* Loop over BROKEN_RANGE segments (if any) */
      do
      {
        /* if the lower and upper are the same value, such as 7
           we should return (7) instead of (7-7) */
        if(((dflm_type *) tab_entry->def_lim_ptr)[j+offset].lower ==
           ((dflm_type *) tab_entry->def_lim_ptr)[j+offset].upper)
        {
          w_ptr = dsatutil_itoa (
                                 ((dflm_type *) tab_entry->def_lim_ptr)[j+offset].lower,
                                 w_ptr,
                                 radix );
        }
        else
        {
          w_ptr = dsatutil_itoa (
                                 ((dflm_type *) tab_entry->def_lim_ptr)[j+offset].lower,
                                 w_ptr,
                                 radix );
          *w_ptr++ = '-';
          w_ptr = dsatutil_itoa (
                                 ((dflm_type *) tab_entry->def_lim_ptr)[j+offset].upper,
                                 w_ptr,
                                 radix );

        }
        if (BROKEN_RANGE == brtest)
        {
          *w_ptr++ = ',';
          offset++;
        }
      } while (brtest &&
               (MAX_BROKEN_RANGE !=
                ((dflm_type *) tab_entry->def_lim_ptr)[j+offset].default_v));

      /* Remove last comma if broken range */
      if (BROKEN_RANGE == brtest)
      {
        w_ptr--;
      }

      *w_ptr++ = ')';
      *w_ptr++ = ',';
    }
    *(w_ptr-1) = '\0';
  }
  else
  {
    for (j=0; j < tab_entry->compound; ++j)
    {
      r_ptr = ((def_list_type *) tab_entry->def_lim_ptr)[j].list_t;
      while ( (c = *r_ptr++) != '\0')
      {
        *w_ptr++ = c;
      }
      *w_ptr++ = ',';
    }
    *(w_ptr-1) = '\0';
  }

  return (unsigned int) (w_ptr - rb_ptr);

} /*  dsatutil_fmt_num_range  */

/*===========================================================================

FUNCTION DSATUTIL_FMT_STRING_RANGE

DESCRIPTION
  This function formats the result of a range inquiry "=?" for a string
  variable.  The function always returns
      +NAME: (20,21,23-7E)
  which is the range of printable ASCII characters.

DEPENDENCIES

RETURN VALUE
  None.

SIDE EFFECTS

===========================================================================*/
void dsatutil_fmt_string_range
(
  const dsati_cmd_type *tab_entry,          /*  entry in parse table  */
  byte *rb_ptr,                             /*  pointer to buffer for result  */
  const dsat_string_item_type *range_ptr    /*  pointer to range  */
)
{
  dsat_string_item_type c;
  const dsat_string_item_type *s_ptr;

  if ((tab_entry->attrib & FCLASS2P0) == 0)
  {
    s_ptr = tab_entry->name;

    while ( (c = *s_ptr++) !=0 )
    {
      *rb_ptr++ = c;
    }

    *rb_ptr++ = ':';
    *rb_ptr++ = ' ';
  }

  s_ptr = range_ptr;
  while ( (*rb_ptr++ = *s_ptr++) != 0)
    ;
} /*  dsatutil_fmt_string_range */

/*===========================================================================

FUNCTION DSATUTIL_FMT_STRING_VAL

DESCRIPTION
  This function formats the value of an extended format string
  variable.  The value may be formatted as the result of a READ
  "?" command, or in the form of a SET command for use in uploading
  the configuration.  For the READ command, the returned string is
  of the form:

      +NAME: <string>

  For the SET command, it is in the form

      +NAME=<value>;

  The formatted value is returned as a null terminated string.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
unsigned int dsatutil_fmt_string_val
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table  */
  byte *rb_ptr,                         /*  point to buffer for result  */
  fmt_enum_type fmt_sw                  /*  selects response or set fmt */
)
{
  dsat_string_item_type c, *w_ptr;
  const dsat_string_item_type *s;
  boolean print_name;
  boolean skip_fmt = FALSE;

  w_ptr = rb_ptr;

  print_name = ( ((tab_entry->attrib & FCLASS2P0) == 0)
        || fmt_sw == SET
        || fmt_sw == RESPONSE_ALL);

  if (print_name)
  {
    s = tab_entry->name;

    while ( (c = *s++) != 0 )
    {
      *w_ptr++ = c;
    }
  }

  switch (fmt_sw)
  {
    case RESPONSE:
    case RESPONSE_ALL:
      if (print_name)
      {
        *w_ptr++ = ':';
        *w_ptr++ = ' ';
      }
      
      /* Abort further formatting if no display attribute set */
      if ((tab_entry->attrib & NO_DISPLAY) == NO_DISPLAY)
      {
        skip_fmt = TRUE;
      }
      break;

    case SET:
      *w_ptr++ = '=';
      break;

    default:
      /*  ERROR */
      ;

  }

  if(!skip_fmt)
  {
    s = (dsat_string_item_type *) tab_entry->val_ptr;

    if ((tab_entry->attrib & NO_QUOTE) == 0)
    {
      *w_ptr++ = '"';
    }

    while ( (c = *s++) != 0)
    {
      *w_ptr++ = c;
    }

    if ((tab_entry->attrib & NO_QUOTE) == 0)
    {
      *w_ptr++ = '"';
    }

    if (fmt_sw == SET)
    {
      *w_ptr++ = ';';
    }
  }

  *w_ptr = '\0';

  return (unsigned int) (w_ptr - rb_ptr);
} /*  dsatutil_fmt_string_val */

/*===========================================================================

FUNCTION DSATUTIL_FMT_MIXED_PARAM_VAL

DESCRIPTION
  This function formats the values of a mixed parameter.
  The values may be formatted as the result of a READ "?" command,
  or in the form of a SET command for use in uploading the configuration.
  For the READ command, the returned string is of the form:

      +NAME: <p1>,<p2>,...

  For the SET command, it is in the form

      +NAME=<value>;

  The formatted value is returned as a null terminated string.

DEPENDENCIES

RETURN VALUE
  Number of characters stored, not including the trailing null character.

SIDE EFFECTS

===========================================================================*/
unsigned int dsatutil_fmt_mixed_param_val
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table  */
  dsm_item_type *res_buff_ptr,          /*  Place to put response       */
  fmt_enum_type fmt_sw                  /*  selects response or set fmt */
)
{
  dsat_string_item_type c, *w_ptr;
  const dsat_string_item_type *s;
  boolean print_name;
  int index = 0;
  uint16 buff_len = 0;
  
  dsat_mixed_param_val_type * val_list = 
    (dsat_mixed_param_val_type *) tab_entry->val_ptr;

  mixed_def_s_type ** mixed_def_list = 
    (mixed_def_s_type **) tab_entry->def_lim_ptr;

  w_ptr = res_buff_ptr->data_ptr + res_buff_ptr->used;
  buff_len = res_buff_ptr->size - res_buff_ptr->used;

  print_name = ( ((tab_entry->attrib & FCLASS2P0) == 0)
                 || fmt_sw == SET
                 || fmt_sw == RESPONSE_ALL);

  if (print_name)
  {
    s = tab_entry->name;

    while ( (c = *s++) != 0 )
    {
      *w_ptr++ = c;
    }
  }

  switch (fmt_sw)
  {
    case RESPONSE:
    case RESPONSE_ALL:
      if (print_name)
      {
        *w_ptr++ = ':';
        *w_ptr++ = ' ';
      }
      break;

    case SET:
      *w_ptr++ = '=';
      break;

    default:
      /*  ERROR */
      ;

  }

  for (index = 0; index < tab_entry->compound; index++)
  {
    /* the index-th parameter is not to be displayed  */
    if( (mixed_def_list[index]->attrib & (NO_DISPLAY)) == 
        (NO_DISPLAY) )
    {
      /* Skip formatting this parameter */
    }
    
    /* the index-th parameter is of type STRING, 
       val_ptr points to its real value */
    else if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
             (CONFIG | STRING) )
    {
      dsat_string_item_type * val_string = val_list[index].string_item;
        
      if(mixed_def_list[index]->attrib & YES_QUOTE )
      {
         w_ptr += (word) std_strlprintf((char *)w_ptr,buff_len,
                                        "\"%s\",", val_string);
      }
      else
      {
         w_ptr += (word) std_strlprintf((char *)w_ptr,buff_len,
                                        "%s,", val_string);
      }
    }

    /* the index-th parameter is a string within a string list,
         we use val_ptr to index into the string list to get the string */
    else if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
             (CONFIG | LIST) )
    {
      const dsat_string_item_type * val_string = 
        mixed_def_list[index]->def->def_list.list_v[val_list[index].num_item];
        
      if(mixed_def_list[index]->attrib & YES_QUOTE )
      {
        w_ptr += (word) std_strlprintf((char *)w_ptr,buff_len,
                                       "\"%s\",", val_string);
      }
      else
      {
        w_ptr += (word) std_strlprintf((char *)w_ptr, buff_len,
                                       "%s,", val_string);
      }
    }

    else if( (mixed_def_list[index]->attrib & (CONFIG | STRING | LIST)) == 
             (CONFIG) )
    {

      w_ptr += (word) std_strlprintf((char *)w_ptr,buff_len, "%u,",
                                     val_list[index].num_item);
    }

    /* the argument is not anticipated */
    else
    {
      ERR("Mixed parameter setting action %d, %dth param has wrong attrib %d",
          tab_entry->name, index, mixed_def_list[index]->attrib);
      return 0;
    }
  }

  /* get rid of the last ',' & terminate string */
  *(--w_ptr) = '\0';

  return (unsigned int) (w_ptr - (res_buff_ptr->data_ptr + res_buff_ptr->used));
} /*  dsatutil_fmt_mixed_param_val */

/*===========================================================================

FUNCTION DSATUTIL_PUT_NUM_ARG

DESCRIPTION
  This function checks the value and number of numeric arguments for
  a command, and, if they are correct, stores the value(s) in the
  parameter.

DEPENDENCIES
  If command uses the BROKEN_RANGE attribute, all parameters in the
  default/limits array must be configured with MAX_BROKEN_RANGE delimiters.

RETURN VALUE
  Returns a boolean. Returns FALSE if there is an error else returns
  TRUE

SIDE EFFECTS
  Describe here any side effects that may occur as a result of calling this
  function. This includes such things as global vars that are modified, etc.

===========================================================================*/
boolean dsatutil_put_num_arg
(
  const dsati_cmd_type *tab_entry,      /*  entry in parse table        */
  const tokens_struct_type *tok_ptr     /*  Command tokens from parser  */
)
{
  dsat_num_item_type scr_val [MAX_ARG];
  byte flag [MAX_ARG]; /* used to flag that there is match in the val list */
  atoi_enum_type e;
  param_srch_enum_type srchrc;
  word radix;
  uint32 j,i;
  uint32 range_offset = 0;
  uint32 brtest = (tab_entry->attrib & BROKEN_RANGE);

  for (i=0;i<MAX_ARG;i++)
  {
    flag[i]=0;
    scr_val[i]=0;
  }

  radix = ( (tab_entry->attrib & HEX) ? 16 : 10);

  if (tok_ptr->args_found > tab_entry->compound)
  {
    return (FALSE);
  }

  for (j = 0; j < tok_ptr->args_found; ++j)
  {
    flag [j] = 0;
    
    if ((tab_entry->attrib & LIST) == 0)
    {
      e = dsatutil_atoi (&scr_val [j], tok_ptr->arg[j], radix);
      if (e == ATOI_OK)
      {
        /* Adjust for broken ranges */
        i = j + range_offset;

        if (0 == brtest)
        {
          /* Continuous range */
          if (scr_val [j] < ((dflm_type *) tab_entry->def_lim_ptr)[i].lower ||
              scr_val [j] > ((dflm_type *) tab_entry->def_lim_ptr)[i].upper)
          {
            /* The argument is out of range */
            return FALSE;
          }
          else
          {
            flag [j] = 1;
          }
        } 
        else
        {
          /* Discontinuous range so loop over each segment */
          while (MAX_BROKEN_RANGE !=
                 ((dflm_type *) tab_entry->def_lim_ptr)[i].default_v)
          {
            if ((scr_val [j] >= ((dflm_type *) tab_entry->def_lim_ptr)[i].lower) &&
                (scr_val [j] <= ((dflm_type *) tab_entry->def_lim_ptr)[i].upper))
            {
              flag [j] = 1;
            }
            i++;
          }
          /* See if one segment worked */
          if (0 == flag [j])
          {
            /* The argument is out of range */
            return FALSE;
          }
          range_offset = i - j;
        }
      } 
      else if (e == ATOI_OUT_OF_RANGE)
      {
        /* the char code of is out of range */
        return FALSE;
      }
      else if (0 != brtest)
      {
	/* Advance to next discontinuous range segment */
        i = j + range_offset;
	while (MAX_BROKEN_RANGE !=
	       ((dflm_type *) tab_entry->def_lim_ptr)[i].default_v) 
        { 
          i++; 
        }
        range_offset = i - j;
      }
      /*else if (e == ATOI_NO_ARG) do nothing: flag[j] remains unset  */
      /*    and the value is not stored                               */
    }
    else  /*  LIST  */
    {
      srchrc = dsatutil_srch_list (
                 &scr_val [j], 
                 ((def_list_type *) tab_entry->def_lim_ptr)[j].list_v,
                 tok_ptr->arg[j],
                 determine_quote_action(tab_entry->attrib) );
      
      /* Continue if paramter detected */
      if (PARAM_NULL != srchrc)
      {
        if (PARAM_HIT == srchrc)
        {
          flag [j] = 1;
        }
        else
        {
          /* not found in list */
          return FALSE;
        }
      }
    }
  }
  
  for (j = 0; j < tok_ptr->args_found; ++j)
  {
    if (flag[j])
    {
      * ((dsat_num_item_type *)tab_entry->val_ptr + j)
          = scr_val [j];
    }
  }
  return TRUE;
} /*  dsatutil_put_num_arg  */

/*===========================================================================

FUNCTION DSATUTIL_PUT_STRING_ARG

DESCRIPTION
  This checks an extended format string WRITE command for correct
  syntax and string length.  If everthing is OK, the string is copied
  into the specified parameter.  Otherwise, the global dsat_result" is
  set to ERROR. Non space characters outside of the '"' are considered
  an error.

DEPENDENCIES


RETURN VALUE
  returns a boolean. Returns FALSE if there is an error else returns
  TRUE

SIDE EFFECTS


===========================================================================*/
boolean dsatutil_put_string_arg
(
  const dsati_cmd_type *tab_entry,   /*  entry in parse table        */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  put_string_enum_type action     /*  Store or concatenate  */
)
{
  byte c, *s_ptr, *t_ptr;
  unsigned int arg_cnt, total_cnt;
  unsigned int n_args;
  boolean quoted = FALSE;

  if ((tok_ptr->args_found != 1) &&         /*  One and only one argument */
      (tab_entry->attrib & NO_QUOTE) == 0)  /*  allowed, unless NOQUOTE   */
  {
     return ( FALSE );
  }

  s_ptr = tok_ptr->arg[0];
  arg_cnt = 0;
  total_cnt = 0;

  if ((tab_entry->attrib & NO_QUOTE) == 0)
  {
    while ( ((c = *s_ptr++) !=0 ) && c != '"')/* No characters except spaces */
    {                                         /* allowed between '=' and '"' */
      if (c != ' ')
      {
         return ( FALSE );
      }
    }

    t_ptr = s_ptr;                        /*  Points to 1st char after '"'*/
    while ( (c = *s_ptr++) != '"')        /*  Count characters inside the */
    {                                     /*  '"'. Non-print not allowed. */
      ++arg_cnt;
      if (arg_cnt > tab_entry->compound || c <' ' || c > '\x7E')
      {
         return ( FALSE );
      }
    }

    total_cnt = arg_cnt;

    while ( (c = *s_ptr++) != 0 )         /*  No trailing characters before */
    {                                     /*  ';' except spaces             */
      if (c != ' ')
      {
          return ( FALSE );
      }
    }
  }
  else                                    /*  NO_QUOTE: count all chars in  */
  {                                       /*  all args to end of command.   */
    n_args = 0;
    t_ptr = s_ptr;
    for ( ;; )
    {
      if ((c = *s_ptr++) == '\0')
      {
        if (++n_args < tok_ptr->args_found)
        {
          s_ptr = tok_ptr->arg[n_args];
          c = ',';
        }
        else
        {
          break;
        }
      }
      ++arg_cnt;
      if (arg_cnt > tab_entry->compound || c <' ' || c > '\x7E')
      {
         return ( FALSE );
      }
    }
  }

  /* Update the parameter value only if non-null string */
  if (!(0==arg_cnt && !quoted))
  {
    s_ptr = (dsat_string_item_type *) tab_entry->val_ptr;
    
    /*----------------------------------------------------------  
    If concatenate requested, and argument is not a NULL string
    advance the store point to the end of the current value, and
    increment total character count.  Compare count to allowable
    for the parameter.
    
    If concatenate requested, and argument is the NULL string,
    leave pointers alone. This will cause a NULL character to
    be stored in the paramter.
    ----------------------------------------------------------*/
    
    if (action == CAT && arg_cnt != 0)
    {
      while (*s_ptr)                      /*  Count current length and add  */
      {                                   /*  to total count                */
        ++s_ptr;
        ++total_cnt;
      }
      if (total_cnt > tab_entry->compound)
      {
        return ( FALSE );
      }
    }
    
    n_args = 0;
    while (arg_cnt--)                   /*  Copy arg to end to parameter  */
    {
      c = *t_ptr++;
      if (c == '\0')
      {
        if (++n_args < tok_ptr->args_found)
        {
          c = ',';
          t_ptr = tok_ptr->arg[n_args];
        }
      }
      *s_ptr++ = c;
    }
    
    /* Verify string length */
    if (s_ptr >
        ((dsat_string_item_type *) tab_entry->val_ptr + tab_entry->compound))
    {
      ERR_FATAL ("OVERFLOW", 0, 0, 0);
    }
    
    *s_ptr = '\0';
  }
  return ( TRUE );

} /*  dsatutil_put_string_arg */

/*===========================================================================

FUNCTION DSATUTIL_MIXED_PARAM_SET_NUM_ARG

DESCRIPTION
  This function checks the value of a string argument for
  a command in the def list.  It determines the string argument's index 
  into the string list.  Use this index to set the val_ptr.

DEPENDENCIES


RETURN VALUE
  returns a param_srch_enum_type:
    PARAM_MISS - Not found in values list 
    PARAM_HIT  - Found in values list            
    PARAM_NULL - No parameter specified          

SIDE EFFECTS
  None

===========================================================================*/
param_srch_enum_type dsatutil_mixed_param_set_num_arg
(
  dsat_num_item_type * val_num_ptr, /* the index number to be returned */
  const def_list_type * def_list_ptr, /* the def list */
  const byte * arg,                   /* the token argument */
  uint32 attrib                       /* the attrib of the MIXED_PARAM */
)
{
  return dsatutil_srch_list(val_num_ptr, 
                   def_list_ptr->list_v,
                   arg,
                   determine_quote_action(attrib));
}

/*===========================================================================

FUNCTION DSATUTIL_ITOA

DESCRIPTION
  This function converts an integer to ASCII, using a specified
  radix. The resulting string is null terminated.

DEPENDENCIES
  The function does not work correctly if r < 2 or r > 36.
  (r is the radix argument.)

  The length of the returned string depends on the value and the
  radix.  The worst case is the value 0xFFFFFFFF and a radix of 2,
  which returns 33 bytes (32 one's and the null terminator.)
  The array pointed to by 's' must be large enough for the returned
  string.

RETURN VALUE
  A pointer to the null terminator is returned.

SIDE EFFECTS

===========================================================================*/
byte * dsatutil_itoa
(
  uint32 v,         /*  Value to convert to ASCII     */
  byte *s,        /*  Pointer to string for result  */
  uint16 r          /*  Conversion radix              */
)
{
  byte buf[33], c;
  int n;

  n = sizeof(buf) - 1;

  buf[n] = '\0';


  do
  {
    c = (byte) (v % r);

    if (n <= 0)
    {
      MSG_HIGH ("OVERFLOW", 0, 0, 0);
      break;
    }

    buf[--n] = (byte) ( (c > 9) ? c + 'A' - 10 : c + '0');

  } while ( (v /= r) > 0);

  while ( (*s++ = buf[n++]) != 0)
      ;

  return (s-1);
} /*  dsatutil_itoa */

/*===========================================================================

FUNCTION DSATUTIL_XITOA

DESCRIPTION
  Formats an unsigned long to hexadecimal.
  Supplies leading zeros.

DEPENDENCIES

RETURN VALUE
  A pointer to the null terminator is returned.


SIDE EFFECTS

===========================================================================*/
unsigned char * dsatutil_xitoa
(
  unsigned long val,  /*  value to convert  */
  byte * a_ptr        /*  pointer to null terminator of result  */
)
{
  byte c, *s_ptr;
  int count = 8;

  s_ptr = a_ptr + 8;

  *s_ptr-- = '\0';

  while (--count >= 0)
  {
    c = val & 0x0F;
    val >>= 4;

    *s_ptr-- = (byte) ( (c > 9) ? c + 'A' - 10 : c + '0');
  }

  return a_ptr+8;
} /* dsatutil_xitoa() */

/*===========================================================================

FUNCTION DSATUTIL_STRCMP_IG_SP_CASE

DESCRIPTION
  This function compares two null terminated strings, ignoring case.

DEPENDENCIES
  None.

RETURN VALUE
  Returns   0 if the strings are identical, except for case,
            otherwise, it returns the difference between the first
            characters to disagree. The difference is first arg minus
            second arg.  Lower case letters are converted to upper
            case before the difference is taken.

SIDE EFFECTS
  None.

===========================================================================*/
int dsatutil_strcmp_ig_sp_case
(
  const byte *s1,             /*  Pointer to string 1 */
  const byte *s2              /*  Pointer to string 2 */
)
{
  byte c1, c2;
  int d;

  do
  {
    c1 = *s1++;
    c2 = *s2++;

    d = UPCASE(c1) - UPCASE(c2);
  }
  while ( d == 0 && c1 != '\0' && c2 != '\0');

  return d;
} /*  dsatutil_strcmp_ig_sp_case  */

/*===========================================================================
FUNCTION DSATUTIL_STRIP_QUOTES_OUT

DESCRIPTION
  This function strips the quotes out from the quoted_str, the results
  are in out_str as a null-terminated string.  str_len is the max length 
  of the out_str string, the applocated space for out_str.

  The quoted_str is a null terminated string.

DEPENDENCIES
  None

RETURN VALUE
  TRUE if the action succeeds.
  FALSE if the action fails.
        The reasons will output to ERR.  The reasons could be 
        1. the str_len is less than the length of
           quoted_str -2, ie the out_str is too small to hold the result.
        2. the quoted_str is not quoted

SIDE EFFECTS
  None
===========================================================================*/
boolean dsatutil_strip_quotes_out
( 
  const byte * quoted_str, byte * out_str, word str_len
)
{
  int index;
  boolean end_quote_found = FALSE;

  if(quoted_str[0] != '"')
  {
    /* This error is caused by user input, not a system error.
       Use MSG_MED */
    MSG_MED("The in_str must be quoted",0,0,0);
    return FALSE;
  }
  
  for (index = 0; 
       (index < str_len) && (quoted_str[index + 1] != '\0') 
         && !end_quote_found; 
       index++)
  {
    if(quoted_str[index + 1] == '"')
    {
      end_quote_found = TRUE;
      out_str[index] = '\0';
    }
    else
    {
      out_str[index] = quoted_str[index + 1];
    }
  }

  if(!end_quote_found)
  {
    /* This error is caused by user input, not a system error.
       Use MSG_MED */
    MSG_MED("The in_str has opened quotes, or out_str is small: %d",
            str_len,0,0);
    return FALSE;
  }
  return TRUE;
} /* dsatutil_strip_quotes_out */

/*===========================================================================
FUNCTION DSATUTIL_SRCH_LIST

DESCRIPTION
  This function searchs a list of strings, and returns the index to the
  item found.  (The first item index is 0).  Index set to NULL if 
  parameter is null, as with empty commas entered, 

  If ignore_quote is true, then srch_list will find quoted args in 
  unquoted lists.  For instance, \"ME\" will find itself as the first 
  item in list ME, MT, SM

DEPENDENCIES
  None

RETURN VALUE
  PARAM_HIT if found.
  PARAM_MISS if no match.
  PARAM_NULL if no parameter specified.

SIDE EFFECTS
  None
===========================================================================*/
param_srch_enum_type dsatutil_srch_list
(
  dsat_num_item_type *val_arg_ptr,  /*  value returned  */
  const byte l[][8],
  const byte * arg,
  action_quote_e_type action_quote
)
{
  unsigned int i = 0;
  const byte * use_arg = arg; /* the argument used to search through arg list */
  byte stripped_arg[MAX_LINE_SIZE];
  dsat_num_item_type arg_num, list_ele_num;

  if(arg[0] == '\0')
  {
    val_arg_ptr = NULL;
    return PARAM_NULL;
  }
  else
  {
    /* In ignore_quote case, if the arg has double quote 
       surrounding it, strip the quotes out */
    switch (action_quote)
    {
      case ACTION_QUOTE_BOTH_OK:
        /* don't care about the quote */
        if(arg[0] == '"')
        {
          (void) dsatutil_strip_quotes_out(arg, stripped_arg, MAX_LINE_SIZE);
          use_arg = stripped_arg;
        }
        break;

      case ACTION_QUOTE_YES_QUOTE:
        /* strip out the quotes */
        if(!dsatutil_strip_quotes_out(arg, stripped_arg, MAX_LINE_SIZE))
        {
          return PARAM_MISS;
        }
        use_arg = stripped_arg;
        break;
        
      case ACTION_QUOTE_NO_QUOTE:
        /* don't strip quote */
        break;
       
      default:
        ERR_FATAL("No such case: %d",action_quote,0,0);
        break;
    }
  }

  /* Abort if parameter null */  
  if (NULL == *use_arg)
  {
    val_arg_ptr = NULL;
    return PARAM_NULL;
  }
  
  /* Search values list for match */  
  while (*l[i] != NULL)
  {
    /* if both the argument and l[i] are valid numbers, do 
       numerical comparision */
    if(dsatutil_atoi(&arg_num, use_arg, 10)   == ATOI_OK &&
       dsatutil_atoi(&list_ele_num, l[i], 10) == ATOI_OK)
    {
      if(arg_num == list_ele_num)
      {
        *val_arg_ptr = i;
        return PARAM_HIT;
      }
    }
    /* do string comparision */
    else if (dsatutil_strcmp_ig_sp_case (l[i], use_arg) == 0)
    {
      *val_arg_ptr = i;
      return PARAM_HIT;
    }
    ++i;
  }

  /* if the execution come here, there is no match */
  return PARAM_MISS;
} /* dsatutil_srch_list ()*/


/*===========================================================================

FUNCTION ITOA_ZFILL

DESCRIPTION
  This function formats a word variable.  The return string is a null
  terminated 3 - digit decimal number with leading zeros.

DEPENDENCIES

RETURN VALUE
  Pointer to the NULL terminator.

SIDE EFFECTS

===========================================================================*/
LOCAL byte * itoa_zfill
(
  word v,
  byte *rb_ptr
)
{
  int n;
  byte c, *ptr;

  ptr = rb_ptr + 3;
  *ptr = '\0';

  for (n = 0; n < 3; ++n)
  {
    c = (byte) (v % 10);
    v /= 10;
    *--ptr = (c + '0');
  }
  
  return rb_ptr + 3;
} /*  itoa_zfill */

/*===========================================================================
FUNCTION DETERMINE_QUOTE_ACTION

DESCRIPTION
  This function determines the action_quote from a parameter's attrib.

DEPENDENCIES
  None

RETURN VALUE
  ACTION_QUOTE_YES_QUOTE if attrib has YES_QUOTE 
  ACTION_QUOTE_NO_QUOTE  if attrib has NO_QUOTE 
  ACTION_QUOTE_BOTH_OK if none of the above

SIDE EFFECTS
  None
===========================================================================*/
LOCAL action_quote_e_type determine_quote_action(uint32 attrib)
{
  /* if the command table does not say this command's 
     arguments do not contain quotes, we ignore the 
     surrounding quotes */
  action_quote_e_type action_quote = ACTION_QUOTE_BOTH_OK;

  if(attrib & NO_QUOTE)
  {
    action_quote = ACTION_QUOTE_NO_QUOTE;
  }
  else if(attrib & YES_QUOTE)
  {
    action_quote = ACTION_QUOTE_YES_QUOTE;
  }
  
  return action_quote;
} /* determine_quote_action */



/*===========================================================================

FUNCTION DSATUTIL_FORMAT_IPADDR

DESCRIPTION
  This function checks for the validity of the IP address and converts it
  to and from internal storage format.
  It verifies the length of the IP address and also verifies if all
  the 4 tuples, when converted to int, are below and upper limit.
  The limit is 254 or 255, depending on passed flag.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatutil_format_ipaddr 
(
   ip_convert_enum_type mode,                /* Mode of conversion      */
   char* ipstr,                              /* String representation   */
   uint32* ipnum,                            /* Interger representation */
   boolean allow_255                         /* Validation flag         */
)
{
  int index, cpi=0, vali=0;
  atoi_enum_type result;
  byte cp[4];
  byte * p;
  dsat_num_item_type val[4];
  uint32 limit = (allow_255) ? 255 : 254;

  for (index = 0; index < 4; index++)
  {
    val[index] = 0;
  }

  if (STRING_TO_UINT == mode)
  {
    /*---------------------------------------------------------------
    The following loop processes the string representing an IP
    address. Each one of the four-tuples is converted in to decimal
    integer and stored in a temporary array.
    If the value is greater than the limit, we return an error.
    For DNS IP address, each tuple in the IP cannot be greater than
    254 (255 is only for broadcasting).
    ---------------------------------------------------------------*/
    for(index = 0; index <= (int)strlen((char*)ipstr); index++)
    {
      if(*(ipstr +index) != '"')
      {
        if((*(ipstr +index) != '.') && (*(ipstr +index) != '\0')) 
        {
          cp[cpi++]=*(ipstr + index);
        }
        else 
        {
          cp[cpi]='\0';
          result = dsatutil_atoi((val+vali), cp, 10);
          if (ATOI_OK != result)
          {
             return DSAT_ERROR;
          }
          if((*(val+vali)) > limit)
          {
             MSG_MED("IP tuple %d larger than limit %d",*(val+vali),limit,0);
             return DSAT_ERROR;
          }
          cpi = 0;         
          vali++;
        }
      }
    }
    
    /* Verify four-tuples found */
    if (4 != vali)
    {
       MSG_MED("More than four tuples",0,0,0);
       return DSAT_ERROR;
    }
    
    *ipnum = (((uint32) val[0]) << 24) | 
             (((uint32) val[1]) << 16) | 
             (((uint32) val[2]) <<  8) | 
               (uint32) val[3];
  }
  else
  {
      p = (byte*) ipstr;
      p = dsatutil_itoa ((uint32)((*ipnum >> 24) & 0xFF), p, 10);
      *p++ = '.';
      
      p = dsatutil_itoa ((uint32)((*ipnum >> 16) & 0xFF), p, 10);
      *p++ = '.';
      
      p = dsatutil_itoa ((uint32)((*ipnum >>  8) & 0xFF), p, 10);
      *p++ = '.';
      
      p = dsatutil_itoa ((uint32)(*ipnum & 0xFF), p, 10);
      *p = '\0';
   }

  return DSAT_OK;
}  /* dsatutil_format_ipaddr */

/*===========================================================================

FUNCTION PBM_CHAR_TO_GSM

DESCRIPTION
  This function performs conversion of a single PBM 8bit character into GSM
  7bit alphabet. On return **gsm_out will point to the last converted character 
  written and the pointer *gsm_out may have been advanced by one place encode 
  a GSM character of the alphabet extension table.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void pbm_char_to_gsm
(
  char pbm_in, 
  char **gsm_out
)
{
  char *tmp_chr_ptr = NULL;
  /* Complementary arrays for converting between PBM 8bit and GSM 7bit:
     for those values that aren't common to both.
     Used separate arrays for pbm_char_to_gsm() and gsm_char_to_pbm() to handle
     the conversion of GSM character '@'. */
  const char pbm8_to_gsm7_single[] = 
  {
    0x24,0x40,0x5F,0x90,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA1,0xA3,0xA4,
    0xA5,0xA7,0xBF,0xC4,0xC5,0xC6,0xC7,0xC9,0xD1,0xD6,0xD8,0xDC,0xDF,0xE0,0xE4,0xE5,
    0xE6,0xE8,0xE9,0xEC,0xF1,0xF2,0xF6,0xF8,0xF9,0xFC,0x00
  };
  const char gsm7_to_pbm8_single[] = 
  {
    0x02,0x20,0x11,0x10,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x40,0x01,0x24,
    0x03,0x5F,0x60,0x5B,0x0E,0x1C,0x09,0x1F,0x5D,0x5C,0x0B,0x5E,0x1E,0x7F,0x7B,0x0F,
    0x1D,0x04,0x05,0x07,0x7D,0x08,0x7C,0x0C,0x06,0x7E,0x00
  };

  ASSERT( NULL != gsm_out && NULL != *gsm_out );

  /* If pbm_in character is in the range 0x00-0x7F and
     is in the group of Basic Latin characters common to PBM 8bit and 
     GSM 7bit alphabets... */
  if ( pbm_in <= MAX_IRA_GSM_CHAR &&
       NULL == strchr( basic_latin_not_common, pbm_in ) )
  {
    **gsm_out = pbm_in;
  }
  /* If convertible to GSM 7bit as a single character... */
  else if ( NULL != 
            ( tmp_chr_ptr = (char *)strchr( pbm8_to_gsm7_single, pbm_in ) ) )
  {
    /* Output character at same offset in complementary array */
    **gsm_out = gsm7_to_pbm8_single[tmp_chr_ptr - pbm8_to_gsm7_single];
  }
  /* If convertible to GSM 7bit as two characters:
     part of the GSM 7bit alphabet extension table of 23.038... */
  else if ( NULL != 
            ( tmp_chr_ptr = (char *)strchr( pbm8_to_gsm7_exttbl, pbm_in ) ) )
  {
    /* Output escape character, advancing pointer... */
    *(*gsm_out)++ = (char)GSM_EXT_TBL_ESC_CHAR;
    /* Then output character at same offset in complementary array
       implementing extension table */
    **gsm_out = gsm7_to_pbm8_exttbl[tmp_chr_ptr - pbm8_to_gsm7_exttbl];
  }
  else
  {
    /* Replace inconvertible character with a space */
    **gsm_out = (char)REPLACEMENT_CHAR;
  }
} /* pbm_char_to_gsm */



/*===========================================================================

FUNCTION GSM_CHAR_TO_PBM

DESCRIPTION
  This function performs conversion of a one or two GSM 7bit characters
  into the PBM 8bit alphabet. If the escape character is encountered the
  function will advance gsm_in by one character and attempt to find the next
  character in the alphabet extension table. If found this is conversion 
  character, if not found it will use the replacement character. 
  On return *pbm_out will have been set to the converted character but 
  pointer will not have been advanced.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void gsm_char_to_pbm
(
  char **gsm_in, 
  char *pbm_out
)
{
  char *tmp_chr_ptr = NULL;
  /* Complementary arrays for converting between PBM 8bit and GSM 7bit:
     for those values that aren't common to both.
     Used separate arrays for pbm_char_to_gsm() and gsm_char_to_pbm() to handle
     the conversion of GSM character '@'. */
  const char pbm8_to_gsm7_single[] = 
  {
    0x24,0x5F,0x90,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,0x9A,0xA1,0xA3,0xA4,
    0xA5,0xA7,0xBF,0xC4,0xC5,0xC6,0xC7,0xC9,0xD1,0xD6,0xD8,0xDC,0xDF,0xE0,0xE4,
    0xE5,0xE6,0xE8,0xE9,0xEC,0xF1,0xF2,0xF6,0xF8,0xF9,0xFC,0x00
  };
  const char gsm7_to_pbm8_single[] = 
  {
    0x02,0x11,0x10,0x12,0x13,0x14,0x15,0x16,0x17,0x18,0x19,0x1A,0x40,0x01,0x24,
    0x03,0x5F,0x60,0x5B,0x0E,0x1C,0x09,0x1F,0x5D,0x5C,0x0B,0x5E,0x1E,0x7F,0x7B,
    0x0F,0x1D,0x04,0x05,0x07,0x7D,0x08,0x7C,0x0C,0x06,0x7E,0x00
  };

  ASSERT( NULL != gsm_in && NULL != *gsm_in && NULL != pbm_out );

  /* If gsm_in character is in the range 0x00-0x7F and
     is in the group of Basic Latin characters common to PBM 8bit and 
     GSM 7bit alphabets... */
  if ( **gsm_in <= MAX_IRA_GSM_CHAR &&
       NULL == strchr( basic_latin_not_common, **gsm_in ) )
  {
    *pbm_out = **gsm_in;
  }
  /* If a single non-escape character is convertible to PBM 8bit... */
  else if ( GSM_EXT_TBL_ESC_CHAR != **gsm_in &&
            NULL != ( tmp_chr_ptr = 
                        (char *)strchr( gsm7_to_pbm8_single, **gsm_in ) ) )
  {
    /* Output character at same offset in complementary array */
    *pbm_out = pbm8_to_gsm7_single[tmp_chr_ptr - gsm7_to_pbm8_single];
  }
  /* If using the GSM 7bit alphabet extension table of 23.038... */
  else if ( GSM_EXT_TBL_ESC_CHAR == **gsm_in )
  {
    /* Advance to next character after escape character... */
    (*gsm_in)++;

    /* If that character is found in the extension table... */
    if ( NULL != ( tmp_chr_ptr = 
                     (char *)strchr( gsm7_to_pbm8_exttbl, **gsm_in ) ) )
    {
      /* Then output character at same offset in complementary array
         implementing extension table */
      *pbm_out = pbm8_to_gsm7_exttbl[tmp_chr_ptr - gsm7_to_pbm8_exttbl];
    }
    else
    {
      *pbm_out = (char)REPLACEMENT_CHAR;
    }
  }
  else
  {
    /* Replace inconvertible character with a space */
    *pbm_out = (char)REPLACEMENT_CHAR;
  }
}

/*===========================================================================

FUNCTION DSATUTIL_RESET_PASSWORD

DESCRIPTION
  This function clears the password variable of specified length.
  The password string is set to 0xFF, null terminated.  

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void dsatutil_reset_password
(
  byte * password,            /* String containing password */
  word length                 /* Maximum length of string */
)
{
  int index;

  /* Do nothing if null pointer passed */
  if (NULL == password) return; 

  /* init the password */
  for (index = 0; index < length; index++)
  {
    password[index] = (char) 0xFF;
  }
  password[0] = 0;
  return;
} /* dsatutil_reset_password */

/*===========================================================================

FUNCTION DSATUTIL_APPEND_DSM_ITEM

DESCRIPTION
  This function checks the buffer length against the passed threshold
  value.  If the threshold is exceeded, a new DSM item is requested and
  appended to the buffer chain.  An error is generated if no dsm_items
  are available. A pointer to the new DSM item is returned as a parameter;
  it equals the input DSM item if no action taken.

DEPENDENCIES
  None

RETURN VALUE
  Pointer to current(appended) DSM item 

SIDE EFFECTS
  A new dsm_item may be allocated.
  
===========================================================================*/
dsm_item_type * dsatutil_append_dsm_item
(
  dsm_item_type * dsm_item_ptr,          /* Pointer to head DSM item */
  word threshold                         /* Threshold length of buffer */
)
{
  dsm_item_type *overflow_buff_ptr;
  dsm_item_type *cur_buff_ptr;
  byte x;
  byte y;
  word * used_ptr;
  byte * buff_ptr;

  cur_buff_ptr = dsm_item_ptr;
  buff_ptr = &dsm_item_ptr->data_ptr[dsm_item_ptr->used];
  used_ptr = &dsm_item_ptr->used;
  
  /*  If the data item is full, acquire a new data item. ERROR if none  */
  /*  available.                                                        */
  /*  Remove the last two characters from the old data item             */
  /*  (which will either be "; " or "<CR><LF>"), and put them on the    */
  /*  front of the new item, so they can be over written if need be.    */
  /*  Use dsm_append to append the two buffers                          */
  if ( *used_ptr > threshold )
  {
    y = *(buff_ptr-1);
    x = *(buff_ptr-2);

    overflow_buff_ptr = dsm_new_buffer (DSM_DS_LARGE_ITEM_POOL);
    if(overflow_buff_ptr == NULL)
    {
      cur_buff_ptr = NULL;
      ERR_FATAL("Running out of buffers",0,0,0);
    }

    /* Now the buffer ptr should point to the new dsm item ptr */
    buff_ptr = overflow_buff_ptr->data_ptr;
    used_ptr = &overflow_buff_ptr->used;
    cur_buff_ptr = overflow_buff_ptr;

    dsm_append( &dsm_item_ptr, &overflow_buff_ptr );
      
    *(buff_ptr) = x;
    *(buff_ptr+1) = y;
  }
  return cur_buff_ptr;
} /* dsatutil_append_dsm_item */

/*===========================================================================

FUNCTION DSATUTIL_CONVERT_CHSET

DESCRIPTION
  This function performs conversion of a given null terminated string in one 
  supported character set to another supported character set, writing it into
  an output buffer provided and null terminating it. It will handle 
  inconvertible characters as indicated by drop_inconvertible, simply not 
  copying them if TRUE or stopping conversion and returning NULL if FALSE. 
  The character with value 0x00 is not supported in the GSM 7 bit or IRA 
  alphabet as NULL is used for terminating strings, neither will the function 
  output this character value except as a termination. When converting from 
  HEX/UCS2 for instance this counts as an inconvertible character.
  It will stop the conversion if indicated maximum output buffer length 
  in bytes is reached.

DEPENDENCIES
  None.

RETURN VALUE
  possible values:
    NULL : if drop_inconvertible FALSE and conversion fails
    pointer to terminating NULL in output buffer : if conversion successful

SIDE EFFECTS
  None
  
===========================================================================*/
char *dsatutil_convert_chset
(
  const char *in_str, 
  dsati_chset_type in_chset,
  const char *out_str,
  dsati_chset_type out_chset,
  uint16 out_max,
  boolean drop_inconvertible
)
{
  #define LWR_BYTE 0x00FF
  /* Macro to convert a character to a NULL-terminated pair of hex characters
     at the location pointed to by out_str_dup, which is left pointing to the 
     terminating NULL */
  #define CVT_TO_HEX_PAIR(ch)\
    if (ch < 16) *out_str_dup++ = '0';\
    out_str_dup = (char *)dsatutil_itoa((uint32)ch, (byte *)out_str_dup, 16);

  char *in_str_dup = (char *)in_str;
  char *out_str_dup = (char *)out_str;
  uint8 in_char_len = 0, out_char_len = 0;
  char   conv_char;
  dsat_num_item_type conv_word;
  char   conv_buf[5] = {0};
  dsat_num_item_type in_char_val;
  char *tmp_chr_ptr = NULL;
  /* Mark the end of output buffer */
  char *last_char_lim;
  uint16 in_str_max_len; /* max length of the data area that can be accessed from in_str */
  
  if ( in_chset >= ALPHA_MAX ||
       out_chset >= ALPHA_MAX )
  {
    return NULL;
  }

  /* Save character lengths */
  switch ( in_chset )
  {
    case ALPHA_UCS2:
      in_char_len = UCS2_CHAR_LEN;
      break;
    case ALPHA_HEX:
      in_char_len = HEX_CHAR_LEN;
      break;
    case ALPHA_GSM:
    case ALPHA_GSM_WITH_AT:
      in_char_len = GSM_CHAR_LEN;
      break;
    default:
    case ALPHA_IRA:
    case ALPHA_PBM_8BIT:
      in_char_len = IRA_CHAR_LEN;
      break;
    case ALPHA_PBM_UCS2:
      in_char_len = PBM_UCS2_CHAR_LEN;
      break;
  }
  switch ( out_chset )
  {
    case ALPHA_UCS2:
      out_char_len = UCS2_CHAR_LEN;
      break;
    case ALPHA_HEX:
      out_char_len = HEX_CHAR_LEN;
      break;
    case ALPHA_GSM:
    case ALPHA_GSM_WITH_AT:
      out_char_len = GSM_CHAR_LEN;
      break;
    default:
    case ALPHA_IRA:
    case ALPHA_PBM_8BIT:
      out_char_len = IRA_CHAR_LEN;
      break;
    case ALPHA_PBM_UCS2:
      out_char_len = PBM_UCS2_CHAR_LEN;
      break;
  }

  /* Adjust last possible char position to save room for 
     the character size and a terminating NULL */
  if ( out_chset == ALPHA_PBM_UCS2 )
  {
   last_char_lim = (out_str_dup + out_max) - (2 * out_char_len );
  }
  else
  {
    last_char_lim = (out_str_dup + out_max) - (out_char_len+1);
  }
  /* compute max size of in_str */
  if (in_chset == ALPHA_8BIT)
  { 
    /* in the case of ALPHA_8BIT let the output buffer determine how much to read from in_str.  This 
    is because NULL characters can be expected in the input stream for conversion and hence can't rely
    on strlen() */
    in_str_max_len = (uint16)(((out_max-1)/out_char_len) * in_char_len); 
  }
  else if (in_chset == ALPHA_PBM_UCS2 )
  {
    in_str_max_len = (uint16)(dsatutil_ucs2_strlen(in_str) * 2);
  }
  else
  {
    /* For all other character sets, we're guaranteed to get NULL-terminated strings */
    in_str_max_len = (uint16)strlen(in_str_dup);
  }

  if ((in_str_max_len % in_char_len) != 0 )
  {
    /* Return with error condition if not a 
       'whole' number of input chars in input string */
    MSG_ERROR("incorrect input string len %d for char set size %d",in_str_max_len-1,in_char_len,0);
    return NULL;
  }

  /* Go until there are characters still to be read AND 
  there is room left in out_str for converted output */
  while ( ((in_str_dup - in_str) < in_str_max_len) &&
          (out_str_dup <= last_char_lim) )
  {
    /* sanity check for internal error */
   if ((in_str_dup - in_str) % in_char_len) /* we didn't consume the right number of chars */
   {
     ERR_FATAL("Internal Error: in_char_len %d processed %d",in_char_len, in_str_dup-in_str,0);
   }

   switch ( in_chset )
    {
      /* Input character set IRA */
      case ALPHA_IRA:
        if ( *in_str_dup > MAX_IRA_GSM_CHAR )
        {
          return NULL;
        }
        switch ( out_chset )
        {
          case ALPHA_GSM:
          case ALPHA_GSM_WITH_AT:
            if ( *in_str_dup == 0x40 ) /* ASCII @ */
            {
              if ( out_chset == ALPHA_GSM )
              {
                *out_str_dup++= GSM_AT_CHAR_REPLACEMENT; 
              }
              else
              {
                *out_str_dup++= GSM_AT_CHAR;
              }
            }
            else
            {
              conv_char = ira_to_gsm7[*in_str_dup];
              if ( conv_char != MAX_8BIT_CHAR && conv_char >= MIN_IRA_GSM_CHAR )
              {
                *out_str_dup++ = conv_char;
              }
              else if ( !drop_inconvertible )
              {
                return NULL;
              }
              else
              {
                /* Replace inconvertible char with a space */
                *out_str_dup++ = (char)REPLACEMENT_CHAR;
              }
            }
            break;
          case ALPHA_UCS2:
            memset( out_str_dup, '0', 2 );
            out_str_dup += 2;
            CVT_TO_HEX_PAIR( *in_str_dup );
            break;
          case ALPHA_HEX:
            CVT_TO_HEX_PAIR( *in_str_dup );
            break;
          case ALPHA_IRA:
          case ALPHA_8BIT:
          case ALPHA_PBM_8BIT:
            *out_str_dup++ = *in_str_dup;
            break;
          case ALPHA_PBM_UCS2:
            *out_str_dup++ = *in_str_dup;
            *out_str_dup++ ='\0';
            break;
          /* If unknown output character set specified */
          default:
            return NULL;
        }
        /* Advance by one input octet */
        in_str_dup += IRA_CHAR_LEN;
        break;

      /* Input character set GSM */
      case ALPHA_GSM:
        if ( (*in_str_dup > MAX_IRA_GSM_CHAR )&& (*in_str_dup != GSM_AT_CHAR_REPLACEMENT ))
        {
          return NULL;
        }
        switch ( out_chset )
        {
          case ALPHA_IRA:
            if ( *in_str_dup == GSM_AT_CHAR_REPLACEMENT)
            {
              *out_str_dup++ = 0x40; /* 0x40 is IRA '@' character */
            }
            else if ( (conv_char = gsm7_to_ira[*in_str_dup]) != MAX_8BIT_CHAR ) 
            {
              *out_str_dup++ = conv_char;
            }
            else if ( !drop_inconvertible )
            {
              return NULL;
            }
            else
            {
              /* Replace inconvertible char with a space */
              *out_str_dup++ = (char)REPLACEMENT_CHAR;
            }
            break;
          case ALPHA_UCS2:
            /* First convert to PBM 8bit alphabet, which is nearly identical to
               UCS2: if escape character is found in_str_dup will be advanced 
               by one and left pointing at character following it */
            if ( *in_str_dup == GSM_AT_CHAR_REPLACEMENT )
            {
              memset( out_str_dup, '0', 2 );
              out_str_dup += 2;
              /* 0x40 is UCS2 equivalent for @ */
              out_str_dup = 
                 (char *)dsatutil_itoa( 0x40, (byte *)out_str_dup, 16);
            }
            else
            {
              gsm_char_to_pbm( &in_str_dup, &conv_char );
              /* If the PBM 8bit character is one not common to UCS2... */
              if ( NULL != (tmp_chr_ptr = 
                          (char *)strchr( pbm8_to_ucs2_not_common, conv_char )) )
              {
                /* Replace it with the proper UCS2 value as hex 4-tuple */
                (void)std_strlcpy( out_str_dup, 
                                  (char *)ucs2_to_pbm8_not_common[tmp_chr_ptr - 
                                                        pbm8_to_ucs2_not_common],
                                   UCS2_CHAR_LEN + 1 );
                out_str_dup += UCS2_CHAR_LEN;
              }
              else
              {
                *out_str_dup++ = '0';
                out_str_dup = (char *)dsatutil_itoa( 0, (byte *)out_str_dup, 16);
                CVT_TO_HEX_PAIR( conv_char );
              }
            }
            break;
          case ALPHA_HEX:
            if ( *in_str_dup == GSM_AT_CHAR_REPLACEMENT )
            {
              /* 0x40 is UCS2 equivalent for @ */
              out_str_dup = 
                 (char *)dsatutil_itoa( 0x40, (byte *)out_str_dup, 16);
            }
            else
            {
              CVT_TO_HEX_PAIR( *in_str_dup );
            }
            break;
          case ALPHA_8BIT:
          case ALPHA_GSM:
            *out_str_dup++ = *in_str_dup;
            break;
          case ALPHA_GSM_WITH_AT:
           if ( *in_str_dup == GSM_AT_CHAR_REPLACEMENT )
           {
             *out_str_dup++ = GSM_AT_CHAR;
           }
           else
           {
             *out_str_dup++ = *in_str_dup;
           }
           break; 
          case ALPHA_PBM_8BIT:
            /* if escape character is found in_str_dup will be advanced by one
               and left pointing at character following it */
            if ( *in_str_dup == GSM_AT_CHAR_REPLACEMENT )
            {
              *out_str_dup++=0x40; /* 0x40 is PBM8bit '@' character */
            }
            else
            {
              gsm_char_to_pbm( &in_str_dup, out_str_dup++ );
            }
            break;
          case ALPHA_PBM_UCS2:
            /* if escape character is found in_str_dup will be advanced by one
               and left pointing at character following it */
            if ( *in_str_dup == GSM_AT_CHAR_REPLACEMENT )
            {
              *out_str_dup++=0x40; /* 0x40 is PBM8bit '@' character */
            }
            else
            {
              gsm_char_to_pbm( &in_str_dup, out_str_dup++ );
            }
            *out_str_dup++ ='\0';
            break;
          /* If unknown output character set specified */
          default:
            return NULL;
        }
        /* Advance by one input octet */
        in_str_dup += GSM_CHAR_LEN;
        break;

      /* Input character set UCS2 */
      case ALPHA_UCS2:
        /* Input UCS2 character must be composed completely of HEX chars */
        if ( strchr(hex_chars, *in_str_dup) == NULL ||
             strchr(hex_chars, *(in_str_dup + 1)) == NULL ||
             strchr(hex_chars, *(in_str_dup + 2)) == NULL ||
             strchr(hex_chars, *(in_str_dup + 3)) == NULL ) 
        {
          return NULL;
        }

        switch ( out_chset )
        {
          case ALPHA_IRA:
          case ALPHA_8BIT:
          case ALPHA_HEX:
            /* If first 2 chars are not '0's */
            if ( *in_str_dup != '0' ||
                 *(in_str_dup + 1) != '0' )
            {
              if ( drop_inconvertible )
              {
                /* Replace with a space */
                *out_str_dup++ = (char)REPLACEMENT_CHAR;
              }
              else
              {
                return NULL;
              }
            }
            else
            {
              if ( ALPHA_HEX == out_chset )
              {
                memcpy( out_str_dup, (in_str_dup+2), HEX_CHAR_LEN );
                out_str_dup += HEX_CHAR_LEN;
              }
              else /* ALPHA_IRA or ALPHA_8BIT */
              {
                /* First convert to a char... */
                (void)std_strlcpy( conv_buf, in_str_dup+2, HEX_CHAR_LEN + 1 );
                /* Already verified input is all HEX chars 
                   so safe to ignore return value */
                (void)dsatutil_atoi( &in_char_val, 
                                     (const byte *)&conv_buf[0], 16 );

                /* IRA is 7 bit alphabet and character 
                   with value 0x00 is not allowed*/
                if ( out_chset == ALPHA_IRA &&
                     ( in_char_val < MIN_IRA_GSM_CHAR ||
                       in_char_val > MAX_IRA_GSM_CHAR ) )
                {
                  if ( !drop_inconvertible )
                  {
                    return NULL;
                  }
                  else
                  {
                    /* Replace inconvertible char with a space */
                    *out_str_dup++ = (char)REPLACEMENT_CHAR;
                  }
                }
                else
                {
                  *out_str_dup++ = (uint8)in_char_val;
                }
              }
            }
            break;
          case ALPHA_GSM:
          case ALPHA_PBM_8BIT:
          case ALPHA_GSM_WITH_AT:
            /* Copy UCS2 character (as hex 4-tuple) into temporary buffer */
            (void)std_strlcpy( conv_buf, in_str_dup, UCS2_CHAR_LEN + 1 );
            /* If found in array of characters not common to both 
               PBM8 and UCS2... */
            if ( PARAM_HIT == dsatutil_srch_list( 
                                     &conv_word,  
                                     ucs2_to_pbm8_not_common,     
                                     (const byte *)conv_buf,
                                     ACTION_QUOTE_NO_QUOTE ) )
            {
              /* Get word from same offset of complementary array */
              in_char_val = pbm8_to_ucs2_not_common[conv_word];
            }
            else if ( PARAM_HIT == dsatutil_srch_list( 
                                          &conv_word,
                                          ucs2_to_pbm8_not_supported,     
                                          (const byte *)conv_buf,
                                          ACTION_QUOTE_NO_QUOTE ) )
            {
              in_char_val = (char)REPLACEMENT_CHAR;
            }
            else
            {
              /* Convert to a word:
                 Already verified input is all HEX chars 
                 so safe to ignore return value */
              (void)dsatutil_atoi( &in_char_val, 
                                   (const byte *)conv_buf, 16 );
            }

            /* If upper byte is not zero */
            if ( in_char_val > MAX_8BIT_CHAR )
            {
              if ( drop_inconvertible )
              {
                /* Replace with a space */
                *out_str_dup++ = (char)REPLACEMENT_CHAR;         
              }
              else
              {
                return NULL;
              }
            }
            else
            {
              if ( ALPHA_GSM == out_chset || ALPHA_GSM_WITH_AT == out_chset)
              {
                if ( (in_char_val & LWR_BYTE) == 0x40 ) /* UCS2 @ char */
                {
                  if ( ALPHA_GSM == out_chset )
                  {
                    *out_str_dup = GSM_AT_CHAR_REPLACEMENT; 
                  }
                  else
                  {
                    *out_str_dup = GSM_AT_CHAR;
                  }
                }
                else
                {
                  pbm_char_to_gsm( (char)(in_char_val & LWR_BYTE), &out_str_dup );
                }
              }
              else
              {
                *out_str_dup = (char)(in_char_val & LWR_BYTE);
              }
              out_str_dup += GSM_CHAR_LEN;
            }
            break;
          case ALPHA_UCS2:
            memcpy( out_str_dup, in_str_dup, UCS2_CHAR_LEN );
            out_str_dup += UCS2_CHAR_LEN;
            break;
          case ALPHA_PBM_UCS2:
            /* Convert the lower 16 bits of the input and add to output then 
             * the upper 16 bits */
            (void)std_strlcpy( conv_buf, in_str_dup + 2, PBM_UCS2_CHAR_LEN + 1 );
            (void)dsatutil_atoi( &in_char_val, 
                                 (const byte *)conv_buf, 16 );
            *out_str_dup++ = (char)in_char_val;
            (void)std_strlcpy( conv_buf, in_str_dup, PBM_UCS2_CHAR_LEN + 1 );
            (void)dsatutil_atoi( &in_char_val, 
                                 (const byte *)conv_buf, 16 );
            *out_str_dup++ = (char)in_char_val;
            break;
          /* If unknown output character set specified */
          default:
            return NULL;
        }
        in_str_dup += UCS2_CHAR_LEN;
        break;

      /* Input character set HEX */
      case ALPHA_HEX:
        /* Input HEX character pair must be composed completely of HEX chars */
        if ( strchr(hex_chars, *in_str_dup) == NULL ||
             strchr(hex_chars, *(in_str_dup + 1)) == NULL )
        {
          return NULL;
        }

        switch ( out_chset )
        {
          case ALPHA_IRA:
          case ALPHA_GSM:
          case ALPHA_8BIT:
          case ALPHA_GSM_WITH_AT:
            (void)std_strlcpy( conv_buf, in_str_dup, (HEX_CHAR_LEN + 1) );

            /* Already verified input is all HEX chars 
               so safe to ignore return value */
            (void)dsatutil_atoi( &in_char_val,
                           (const byte *)&conv_buf[0], 16 );


            /* GSM and IRA are 7 bit alphabets and character 
               with value 0x00 is not allowed */
            if ( out_chset != ALPHA_8BIT &&
                 ( in_char_val < MIN_IRA_GSM_CHAR ||
                   in_char_val > MAX_IRA_GSM_CHAR ) )
            {
              if ( !drop_inconvertible )
              {
                return NULL;
              }
              else
              {
                /* Replace inconvertible char with a space */
                *out_str_dup++ = (char)REPLACEMENT_CHAR;
              }
            }
            else
            { /* Check for HEX @ character */
              if ( ( in_char_val == 0x40 ) && ( out_chset == ALPHA_GSM ))
              {
                *out_str_dup++ = GSM_AT_CHAR_REPLACEMENT;
              }
              else if ( ( in_char_val == 0x40 ) && ( out_chset == ALPHA_GSM_WITH_AT ))
              {
                *out_str_dup++ = GSM_AT_CHAR;
              }
              else
              {
                *out_str_dup++ = (uint8)in_char_val;
              }
            }
            break;
          case ALPHA_UCS2:
            memset( out_str_dup, '0', HEX_CHAR_LEN );
            out_str_dup += HEX_CHAR_LEN;
            break;
          case ALPHA_HEX:
            memcpy( out_str_dup, in_str_dup, HEX_CHAR_LEN );
            out_str_dup += HEX_CHAR_LEN;
            break;
          /* If unsupported output character set specified */
          default:
            return NULL;
        }
        in_str_dup += HEX_CHAR_LEN;
        break;

      /* Input character set 8BIT */
      case ALPHA_8BIT:
        switch ( out_chset )
        {
          case ALPHA_IRA:
          case ALPHA_GSM:
          case ALPHA_GSM_WITH_AT:
            if ( *in_str_dup < MIN_IRA_GSM_CHAR ||
                 *in_str_dup > MAX_IRA_GSM_CHAR )
            {
              if ( !drop_inconvertible )
              {
               return NULL;
              }
              else
              {
                /* Replace inconvertible char with a space */
                *out_str_dup++ = (char)REPLACEMENT_CHAR;
              }
            }
            else
            {  /* check for ALPHA 8 BIT @ character */
              if ( ( *in_str_dup == 0x40 ) && ( out_chset == ALPHA_GSM ))
              {
                *out_str_dup++ = GSM_AT_CHAR_REPLACEMENT;
              }
              else if ( ( *in_str_dup == 0x40 ) && ( out_chset == ALPHA_GSM_WITH_AT ) )
              {
                *out_str_dup++ = GSM_AT_CHAR;
              }
              else
              {
                *out_str_dup++ = (uint8)*in_str_dup;
              }
            }
            break;
          case ALPHA_UCS2:
            memset( out_str_dup, '0', 2 );
            out_str_dup += 2;
            CVT_TO_HEX_PAIR( *in_str_dup );
            break;
          case ALPHA_HEX:
            CVT_TO_HEX_PAIR( *in_str_dup );
            break;
          /* If unsupported output character set specified */
          default:
            return NULL;
        }
        in_str_dup += EIGHT_BIT_CHAR_LEN;
        break;

      /* Input character set PBM 8BIT */
      case ALPHA_PBM_8BIT:
        switch ( out_chset )
        {
          case ALPHA_IRA:
            /* PBM 8bit and IRA T.50 are the same over the range 0x00-0x7F */
            *out_str_dup++ = ( *in_str_dup <= MAX_IRA_GSM_CHAR ) ?
                               *in_str_dup : REPLACEMENT_CHAR;
            break;
          case ALPHA_UCS2:
            /* If the PBM 8bit character is one not common to UCS2... */
            if ( NULL != (tmp_chr_ptr = 
                      (char *)strchr( pbm8_to_ucs2_not_common, *in_str_dup )) )
            {
              /* Replace it with the proper UCS2 value as hex 4-tuple */
              (void)std_strlcpy( out_str_dup, 
                                (char *)ucs2_to_pbm8_not_common[tmp_chr_ptr - 
                                                      pbm8_to_ucs2_not_common],
                                 UCS2_CHAR_LEN + 1);
              out_str_dup += UCS2_CHAR_LEN;
            }
            else
            {
              /* Converting from 8bit alphabet upper 16 bits always 0 */
              memset( out_str_dup, '0', 2 );
              out_str_dup += 2;
              CVT_TO_HEX_PAIR( *in_str_dup );
            }
            break;
          case ALPHA_GSM:
          case ALPHA_GSM_WITH_AT: /* first check for PBM 8 bit @ character */
            if ( ( out_chset == ALPHA_GSM ) && ( *in_str_dup == 0x40 ))
            {
              *out_str_dup = GSM_AT_CHAR_REPLACEMENT; 
            }
            else if ( ( out_chset == ALPHA_GSM_WITH_AT ) && ( *in_str_dup == 0x40 ))
            {
              *out_str_dup = GSM_AT_CHAR; 
            }
            else
            {
              pbm_char_to_gsm( *in_str_dup, &out_str_dup );
            }
            out_str_dup++;
            break;
          /* If unsupported output character set specified */
          default:
            MSG_ERROR("Conversion from PBM alpha limited to IRA, GSM, UCS2",
                      0,0,0);
            return NULL;
        }
        in_str_dup += PBM_8BIT_CHAR_LEN;
        break;
      /* Input character set PBM UCS2 */
      case ALPHA_PBM_UCS2:
        switch ( out_chset )
        {
          case ALPHA_IRA:
            /* Conversion not possible if in the range 0x0100-0xFFFF */
            if ( *(in_str_dup + 1) != '\0' )
            {
              *out_str_dup++ = REPLACEMENT_CHAR;
              break;
            }
            /* PBM UCS2 and IRA T.50 are the same over the range 0x0000-0x007F */
            *out_str_dup++ = ( *in_str_dup <= MAX_IRA_GSM_CHAR ) ?
                               *in_str_dup : REPLACEMENT_CHAR;
            break;
          case ALPHA_GSM:
          case ALPHA_GSM_WITH_AT: 
            /* Conversion not possible if in the range 0x0100-0xFFFF */
            if ( *(in_str_dup + 1) != '\0' )
            {
              *out_str_dup++ = REPLACEMENT_CHAR;
              break;
            }
            /* first check for PBM UCS2 bit @ character */
            if ( ( out_chset == ALPHA_GSM ) && ( *in_str_dup == 0x40 ))
            {
              *out_str_dup = GSM_AT_CHAR_REPLACEMENT; 
            }
            else if ( ( out_chset == ALPHA_GSM_WITH_AT ) && ( *in_str_dup == 0x40 ))
            {
              *out_str_dup = GSM_AT_CHAR; 
            }
            else
            {
              pbm_char_to_gsm( *in_str_dup, &out_str_dup );
            }
            out_str_dup++;
            break;
          case ALPHA_UCS2:
              CVT_TO_HEX_PAIR( *(in_str_dup + 1) );
              CVT_TO_HEX_PAIR( *in_str_dup );
            break;
          /* If unknown output character set specified */
          default:
            MSG_ERROR("Conversion from ALPHA_PBM_UCS2 is limited to IRA, GSM, UCS2",
                       0,0,0);
            return NULL;
        }
        in_str_dup += PBM_UCS2_CHAR_LEN;
        break ;
      /* If unknown input character set specified */
      default:
        MSG_ERROR("Invalid input character set specified %d",
                   in_chset,0,0);
        return NULL;
    }
  }
  
  if (out_chset == ALPHA_PBM_UCS2)
  {
    *out_str_dup++ = '\0';
  }
  *out_str_dup = '\0';
  return out_str_dup;
} /* dsatutil_convert_chset */

/*===========================================================================

FUNCTION DSATUTIL_TIMER_CB

DESCRIPTION
  This function is called when timer expires.

DEPENDENCIES

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
LOCAL void dsatutil_timer_cb(unsigned long timer_id)
{
  /* submit dstask a cmd */
  ds_cmd_type *cmd_ptr;

    /*-------------------------------------------------------------------------
    Send a DS_TIMER_EXPIRED_CMD to the DS task, and indicate which timer
    expired.
  -------------------------------------------------------------------------*/
  if( ( cmd_ptr = ds_get_cmd_buf() ) != NULL )
  {
    cmd_ptr->hdr.cmd_id = DS_AT_TIMER_EXPIRED_CMD;
    cmd_ptr->cmd.dsat_timer_id = timer_id;

    ds_put_cmd( cmd_ptr );
  }
  else
  {
    ERR_FATAL( "Can't get cmd buf from DS task", 0, 0, 0 );
  }
} /* dsatutil_timer_cb */

/*===========================================================================
FUNCTION DSATUTIL_UCS2_STRLEN

DESCRIPTION
  This function counts the number of UCS2 characters in a string.
  The  string must be terminated by 0x0000.

DEPENDENCIES
  None

RETURN VALUE
  The number of UCS2 characters in the given string.

SIDE EFFECTS
  None.
===========================================================================*/
 uint16 dsatutil_ucs2_strlen
(
  const char* str
)
{
  uint16 i = 0;
  while( str[i] || str[i + 1])
  {
    i += 2;
  }
  return i/2;
} /* dsatutil_ucs2_strlen */

/*===========================================================================

FUNCTION DSATUTIL_VALIDATE_DIAL_CHAR

DESCRIPTION
  This function examines the passed dial string charater and returns a
  validation result.  It is up to the caller to act upon the result as
  approppriate within its context.

  Refer to 3GPP TS 27.007 section 6.2 for details on the valid dialing 
  digits.  The supported set are summarized as follows:
    0 - 9 
    A - C
    + * ; #           
    D - E             (ETSI SMS PDU mode)
    W , $ T P ! @ D   (accepted but flagged for strip)
  
DEPENDENCIES
  None

RETURN VALUE
  Returns enum value indicating validation result.

SIDE EFFECTS
  None

===========================================================================*/
dsat_dial_val_e_type dsatutil_validate_dial_char
(
  byte * c_ptr                 /*  Input character   */
)
{
  dsat_dial_val_e_type result =  DSAT_DIAL_EMPTY;

  /* Characters that are ignored in a dial string */
  const byte d_strip        [] = "W,$TP!@D";

  /* Test for empty input */
  if (*c_ptr != '\0')
  {
    byte c = UPCASE (*c_ptr);

    /* Catagorize character */
    if (c >= '0' && c <= '9')
    {
      result = DSAT_DIAL_DIGIT;
    }
    else if (c >= 'A' && c <= 'C')
    {
      result = DSAT_DIAL_ASCII_ABC;
    }
    else if (c == 'D')
    {
      result = DSAT_DIAL_ASCII_D;
    }
    else if (c == 'E')
    {
      result = DSAT_DIAL_ASCII_E;
    }
    else if (c == '*')
    {
      result = DSAT_DIAL_STAR;
    }
    else if (c == '+')
    {
      result = DSAT_DIAL_PLUS;
    }
    else if (c == '#')
    {
      result = DSAT_DIAL_POUND;
    }
    else if (c == ';')
    {
      result = DSAT_DIAL_SEMICOLON;
    }
    else if (0 != strchr ((char*)d_strip, (char)c))
    {
      result = DSAT_DIAL_STRIP;
    }
    else
    {
      result = DSAT_DIAL_IGNORE;
    }
  }
  return result;
} /* dsatutil_validate_dial_char () */


/*===========================================================================

FUNCTION DSATUTIL_STRCMP_PKT_DIAL_STR

DESCRIPTION
  This function scans the input null terminated string for packet data
  control string prefix and suffix.  
  For ETSI mode Prefix is "*98" and suffix is "#".

DEPENDENCIES
  None.

RETURN VALUE
  Returns TRUE if the input string has correct prefix and suffix,  FALSE
  otherwise.

SIDE EFFECTS
  None.

===========================================================================*/
boolean dsatutil_strcmp_pkt_dial_str
(
  byte               *orig_dial_str,   /*  Pointer to the dial string */
  dsati_mode_e_type  mode              /*  Mode of Operation          */
)
{
  byte ** pkt_call_id = NULL; 
  byte *s;
  boolean result;
  byte    i;

#ifdef FEATURE_CDMA
  byte *  cdma_pkt_call_id[N_DIAL_STRS] = 
          { dsat707_pkt_orig_str, dsat707_pkt_orig_str };
#endif /* FEATURE_CDMA */
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
  
  
  /* Get pkt origination string by current mode */  
  if ( mode == DSAT_MODE_CDMA )
  {
#ifdef FEATURE_CDMA
    pkt_call_id = cdma_pkt_call_id;
#endif /* FEATURE_CDMA */
  }
  else
  {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
  }
  
  /* If no support for current mode, abort */
  if (!pkt_call_id)  
  {
    MSG_HIGH ("Can't originate pkt in %d mode!", mode,0,0);
    return FALSE;
  }

  /* sanity check */
  ASSERT((pkt_call_id[0] != NULL) && (pkt_call_id[1] != NULL));

  /* now check if dial string matches pkt origination */
  s = orig_dial_str;

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* (FEATURE_WCDMA) || (FEATURE_GSM) */

  /* check to see if dial string matches any of the possible strings */
  /* note this corrupts pkt_call_id pointers */
  result = TRUE;
  for (i=0; i < N_DIAL_STRS; i++)
  {
    /* remember it after there is a mismatch */ 
    boolean is_match = TRUE;

    s = orig_dial_str;
    while (*s && *pkt_call_id[i])
    {
#ifdef FEATURE_CDMA
      /* only digit and * # are used to compare with 
         the packet dial string, skip other chars */
      if(mode == DSAT_MODE_CDMA)
      {
        if (! ((*s <= '9' && *s > '0') || *s == '#' || *s == '*' )
            )
        {
          s++;
          /* do while loop check again */
          continue;
        }
      }
#endif /* FEATURE_CDMA */
      if (*s++ != *pkt_call_id[i]++) {
        is_match = FALSE;
        break;  /* this character doesn't match */
      }
    }

    if (mode == DSAT_MODE_CDMA)
    {
      /* skip the non-digit-*-# chars, this makes sure #777TT matches #777 */
      while (*s && ! ((*s <= '9' && *s > '0') || *s == '#' || *s == '*' ))
      {
        s++;
      }

      /* CDMA mode the pattern needs a strict match */
      if (is_match && !(*s) && !*pkt_call_id[i])
      {
        break;  /* the strings matched */
      }
    }
    else
    {
      /* we are in etsi mode, the comparation is done to the 
         beginning part of the dail string
      */
      ASSERT(mode == DSAT_MODE_GSM || mode == DSAT_MODE_WCDMA);
      if (is_match && (!(*s) || !*pkt_call_id[i]))
      {
        break;  /* the strings matched */
      }

    }
  }
  if (i==N_DIAL_STRS)
  {
    result = FALSE;  /* no string matched */
  }
  
  MSG_HIGH("strcmp_pkt_dial_str result = %d", result, 0, 0);
  return result;
} /*  dsatutil_strcmp_pkt_dial_str() */


#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* (FEATURE_WCDMA) || (FEATURE_GSM) */

#endif /* FEATURE_DATA_STRIP_ATCOP */

/*===========================================================================
FUNCTION DSATUTIL_GET_NV_ITEM 

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_get_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
)
{
  nv_cmd_type  nv_command;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  memset ((void*)&nv_command, 0x0, sizeof(nv_cmd_type));
  
  if (rex_self() == &ds_tcb)
  {
    nv_command.status = dsi_get_nv_item (item, data_ptr);
  }
  else if (rex_self() == &ps_tcb) 
  {
        nv_command.status = ps_get_nv_item (item, data_ptr);
  }
  return( nv_command.status );
}

/*===========================================================================
FUNCTION DSATUTIL_PUT_NV_ITEM 

DESCRIPTION
  Get an item from the nonvolatile memory.  

RETURN VALUE
  The NV return code.
  
DEPENDENCIES
  NV task is expected to be available.

SIDE EFFECTS
  None

===========================================================================*/
nv_stat_enum_type dsatutil_put_nv_item 
(
  nv_items_enum_type  item,           /* Which item */
  nv_item_type       *data_ptr        /* Pointer to space for item */
)
{
  nv_cmd_type  nv_command;

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
  
  memset ((void*)&nv_command, 0x0, sizeof(nv_cmd_type));
  
  if (rex_self() == &ds_tcb)
  {
    nv_command.status = dsi_put_nv_item (item, data_ptr);
  }
  else if (rex_self() == &ps_tcb) 
  {
        nv_command.status = ps_put_nv_item (item, data_ptr);
  }
  return( nv_command.status );
}

/*===========================================================================

FUNCTION DSATUTIL_ATOI

DESCRIPTION
  This function converts an ASCII string to an integer, using a specified
  radix.  Spaces are ignored. Letters used for radices above 10 may be of
  either case.

DEPENDENCIES


RETURN VALUE
  Returns 
    ATOI_OK           : for a correct conversion,
    ATOI_NO_ARG       : 1 if no argument was found,
    ATOI_OUT_OF_RANGE : if a character or the total value is out of range.

SIDE EFFECTS

===========================================================================*/
atoi_enum_type dsatutil_atoi
(
  dsat_num_item_type *val_arg_ptr,      /*  value returned  */
  const byte *s,                        /*  points to string to eval  */
  unsigned int r                        /*  radix */
)
{
  atoi_enum_type err_ret = ATOI_NO_ARG;
  byte c;
  dsat_num_item_type val, val_lim, dig_lim;

  val = 0;
  val_lim = (dsat_num_item_type) ((unsigned int)MAX_VAL_NUM_ITEM / r);
  dig_lim = (dsat_num_item_type) ((unsigned int)MAX_VAL_NUM_ITEM % r);

  while ( (c = *s++) != '\0')
  {
    if (c != ' ')
    {
      c = (byte) UPCASE (c);
      if (c >= '0' && c <= '9')
      {
        c -= '0';
      }
      else if (c >= 'A')
      {
        c -= 'A' - 10;
      }
      else
      {
        err_ret = ATOI_OUT_OF_RANGE;  /*  char code too small */
        break;
      }

      if (c >= r || val > val_lim
          || (val == val_lim && c > dig_lim))
      {
        err_ret = ATOI_OUT_OF_RANGE;  /*  char code too large */
        break;
      }
      else
      {
        err_ret = ATOI_OK;            /*  arg found: OK so far*/
        val = (dsat_num_item_type) (val * r + c);
      }
    }
    *val_arg_ptr =  val;
  }
  
  return err_ret;

} /*  dsatutil_atoi */

#endif /* FEATURE_DATA */

