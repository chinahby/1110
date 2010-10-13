/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                    A T   C O M M A N D   P A R S E R

GENERAL DESCRIPTION
  This module implements the parsing of AT modem commands.

EXTERNALIZED FUNCTIONS
  dsatpar_parse_cmd_line
    Parses a NULL terminated command line creating a token structure
    for each command in command line.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  The AT command processor must be initialized by calling the function
  ds_atcop_init on initial power-up and calling ds_atcop_init_service_mode
  each time a new operating service mode is entered.


   Copyright (c) 1995, 1996 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 1997, 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 1999 by QUALCOMM, Incorporated.  All Rights Reserved.
   Copyright (c) 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatpar.c_v   1.0   08 Aug 2002 11:19:44   akhare  $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatpar.c#2 $ $DateTime: 2008/02/28 05:25:49 $ $Author: praveenp $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
02/28/08   pp      Klocwork fix.
08/10/01   sjd     Initial release to MSM5200 archives.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "target.h"
#include "err.h"
#include "dsati.h"
#include "sio.h"

#include "assert.h"


/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/* Running AT result code resulting from command line parsing. */
LOCAL dsat_result_enum_type result_code;  

/* Buffer to store token line data in. */
LOCAL byte token_line[MAX_LINE_SIZE];


/*-------------------------------------------------------------------------
            Constants:
-------------------------------------------------------------------------*/


/*-------------------------------------------------------------------------
    Protypes for local functions:
-------------------------------------------------------------------------*/

LOCAL byte * parse_extended_command
(
  byte *,
  tokens_struct_type *
);

LOCAL byte * parse_single_char_cmd
(
  byte *,
  tokens_struct_type *
);

LOCAL byte * parse_sregister_cmd
(
  byte *,
  tokens_struct_type *
);

LOCAL byte * parse_hard_to_parse_cmd
(
  byte *cmd_string,
  tokens_struct_type *tok_ptr
);

LOCAL int trim_string 
(
   byte **start   /*  Pointer to beginning of string */
);



/*===========================================================================

FUNCTION DSATPAR_PARSE_CMD_LINE

DESCRIPTION
  This function parses a NULL terminated command line with the "AT"
  removed.  It parses each command and its associated arguments within
  the command line.  A token structure is produced for each command
  parsed.  Each token structure produced is placed on the AT command
  processor token queue for later processing.  

DEPENDENCIES
  None

RETURN VALUE
  Result code indicating whether an error occurred during parsing or
  parsing was successful (DSAT_OK or DSAT_ERROR).

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatpar_parse_cmd_line
(
  byte *cmd_line                /*  NULL terminated command line with
                                    "AT" prefix and <CR> removed. */
)
{
  byte                *curr_char_ptr;   /*  running ptr for parsing cmd   */
  tokens_struct_type   token;           /*  parser results go here        */

  /*-------------------------------------------------------------------------
    Initialize token structure to point to buffer used for command
    line storage.
  -------------------------------------------------------------------------*/

  token.working_at_line = token_line;
  
  /*-------------------------------------------------------------------------
    Variable result_code contains the results codes specified in
    EIA 602 (plus some internal codes).  It is only used here to indicate
    if an error has occurred during parsing.
  -------------------------------------------------------------------------*/

  result_code = DSAT_OK;
  curr_char_ptr = cmd_line;

  /*--------------------------------------------------------------------------
    BEGIN MAIN LOOP parsing commands in the command line:
  --------------------------------------------------------------------------*/

  while ( *curr_char_ptr != '\0' && result_code == DSAT_OK )
  {
    /*------------------------------------------------------------------------
      Parse commands from command line by looking at leading character.

      Each case either leaves 'curr_char_ptr' pointing to the next character
      in the command line, or terminates the main loop by setting
      'result_code' to something other than DSAT_OK.
    ------------------------------------------------------------------------*/

    switch ( UPCASE( *curr_char_ptr ) )
    {
      case ' ':
        /*--------------------------------------------------------------------
          Ignore spaces
        --------------------------------------------------------------------*/
        ++curr_char_ptr;
        break;

      case '+':
        /*--------------------------------------------------------------------
          Extended format command         
        --------------------------------------------------------------------*/
        token.cmd_category = EXT_AT_CMD;
        curr_char_ptr = parse_extended_command( curr_char_ptr, &token );
        if ( result_code != DSAT_ERROR )
        {
          result_code = dsatcmdp_queue_token( &token );
        }
        
        break;

      case '$':
        /*--------------------------------------------------------------------
          Extended format vendor specific command
        --------------------------------------------------------------------*/
        token.cmd_category = VENDOR_AT_CMD;
        curr_char_ptr = parse_extended_command( curr_char_ptr, &token );
        if ( result_code != DSAT_ERROR )
        {
          result_code = dsatcmdp_queue_token( &token );
        }
        
        break;
        
      case 'S':
        /*--------------------------------------------------------------------
          S-register command
        --------------------------------------------------------------------*/
        token.cmd_category = SREG_AT_CMD;
        curr_char_ptr = parse_sregister_cmd( curr_char_ptr, &token );
        if ( result_code != DSAT_ERROR )
        {
          result_code = dsatcmdp_queue_token( &token );
        }
        
        break;

      case 'D':
        /*--------------------------------------------------------------------
          Normal dial command
        --------------------------------------------------------------------*/
        token.cmd_category = BASIC_AT_CMD;
        curr_char_ptr = parse_hard_to_parse_cmd( curr_char_ptr, &token );
        ASSERT( result_code != DSAT_UNREC );
        if ( result_code != DSAT_ERROR )
        {
          result_code = dsatcmdp_queue_token( &token );          
        }
        
        break;

      case ';':
        /*--------------------------------------------------------------------
          Command line error trap
        --------------------------------------------------------------------*/
        ++curr_char_ptr;
        result_code = DSAT_ERROR;       /*  Terminates loop */
        DSATI_ERROR_MSG();

        break;

      default:
        /*--------------------------------------------------------------------
          Single letter basic command with or without parameter,
          or <&> plus single letter with parameter            
         -------------------------------------------------------------------*/
        token.cmd_category = BASIC_AT_CMD;
        if ( *curr_char_ptr >= '0' && *curr_char_ptr <= '9' )
        {
          result_code = DSAT_ERROR;     /*  Terminates loop */
          DSATI_ERROR_MSG();
        }

        curr_char_ptr = parse_single_char_cmd( curr_char_ptr, &token );
        if ( result_code != DSAT_ERROR )
        {
          result_code = dsatcmdp_queue_token( &token );
        }
        
        break;

    }  /* End switch ( UPCASE (*curr_char_ptr) ) */

  }  /* End MAIN LOOP: while curr_char_ptr ... */

  /*-------------------------------------------------------------------------
    Check for error resulting from parsing command line.  Send error
    result code and flush all command line commands from token queue
    if error occurred.
   ------------------------------------------------------------------------*/

  if ( result_code == DSAT_ERROR )
  {
    dsatrsp_send_cell_result( DSAT_ERROR );

    dsatcmdp_flush_token_queue( );
  }

  return result_code;
  
} /*  dsatpar_parse_cmd_line( ) */


/*===========================================================================

FUNCTION PARSE_EXTENDED_COMMAND

DESCRIPTION
  This function parses an incoming character string, which is assumed
  to be an extended format AT+ command, with the AT removed. The
  results of the parse are placed in a specified structure for
  further processing.

  The structure specified in the argument list is filled in as
  follows:

  The 'name' field in the structure points to the first non-blank
  character in the input string (assumed to include the '+' character).

  The 'arg' fields point to the first character following =, ?, =?,
  or following a comma.

  The 'op' field contains the combined value of a set of bit-mapped
  manifest constants:
    NA is set at the beginning of the command name;
    EQ is set when an '=' is found after the name;
    QU is set after the name or after '='; it is not set after AR
        is set;
    AR is set when a non space or non ; is found after EQ and/or QU
        is set.

  The 'args_found' field counts the number of comma-separated arguments.

  The 'end_of_line' field points to the end of data (last NULL character)
  placed in the 'working_at_line' buffer.
  
  The ';' and ',' are not treated as delimiters inside '"'.

  The name and arg pointers point to a buffer contains in the
  'tokens_res' structure.

DEPENDENCIES
  None

RETURN VALUE
  A pointer is returned which points to the next character to be
  processed in the input string.  The character pointed to will be
  the character following a ';' or the NULL terminator.

SIDE EFFECTS
  If the internal buffer overflows, the global "result_code" is
  set to ERROR.

===========================================================================*/
LOCAL byte * parse_extended_command
(
  byte *a_ptr,                      /*  String to parse           */
  tokens_struct_type *tokens_res    /*  Structure to hold results */
)
{
  byte c, quote_toggle;
  byte *working_ptr;

  working_ptr = tokens_res->working_at_line;
  tokens_res->op = 0;
  tokens_res->name = working_ptr;
  tokens_res->args_found = 0;
  quote_toggle = 0;

  while ( (c = *a_ptr) != '\0' )
  {
    if ( working_ptr >= &tokens_res->working_at_line[MAX_LINE_SIZE-1] )
    {
      result_code = DSAT_ERROR;
      DSATI_ERROR_MSG();
      break;
    }
    
    *working_ptr = c;
    
    if ( c != ' ' && c != ';' && tokens_res->op == 0 )
    {
      tokens_res->op = NA;        /*  Start of Name */
      tokens_res->name = working_ptr;
    }
    
    else if ( c == '=' && tokens_res->op == NA )
    {
      *working_ptr = '\0';                  /*  Terminate Name  */
      tokens_res->arg[0] = working_ptr + 1; /*  Start of (first) argument */
      tokens_res->args_found = 1;
      tokens_res->op |= EQ;
    }
    
    else if ( c == '?' && (tokens_res->op & (NA|AR)) == NA )
    {
      *working_ptr = '\0';                  /*  Terminate Name  */
      tokens_res->arg[0] = working_ptr+1;   /*  Start of (first) argument */
      tokens_res->args_found = 1;
      tokens_res->op |= QU;
    }
    
    else if ( c != ' ' && c != ';' && tokens_res->op > NA )
    {
      tokens_res->op |=AR;        /*  Argument (or comma) actually found  */
    }
    
    if ( c == '"' && (tokens_res->op & AR) )
    {
      quote_toggle = !quote_toggle;
    }
    
    if ( ! quote_toggle )         /*  Allow ';' and ',' inside '"'  */
    {
      if ( c == ',' && (tokens_res->op & AR) )
      {
        *working_ptr = '\0';                /*  Terminate current argument  */
        if (tokens_res->args_found > 0) /* condition added only to appease klocwork*/
        {
          (void)trim_string(&tokens_res->arg [tokens_res->args_found-1]);
        }
        if ( tokens_res->args_found < MAX_ARG )
        {
          /*  Start of next arg */
          tokens_res->arg [tokens_res->args_found] = working_ptr + 1;
        }
        ++tokens_res->args_found;
      }
      
      if ( c == ';' )
      { 
        ++a_ptr;
        break;
      }
    }
    ++working_ptr;
    ++a_ptr;
  }

  *working_ptr = '\0';
  tokens_res->end_of_line = working_ptr;

  /* Ensure leading/trailing spaces are trimmed */
  (void)trim_string(&tokens_res->name);
  if (0 < tokens_res->args_found)
  {
     (void)trim_string(&tokens_res->arg [tokens_res->args_found-1]);
  }
  return a_ptr;
  
} /* parse_extended_command( ) */


/*===========================================================================

FUNCTION PARSE_SINGLE_CHAR_CMD

DESCRIPTION
  This function parses single character and '&' plus single character
  commands.  If there is an argument, the result structure is filled
  in as though an '=' were found (->op = (NA|EQ|AR)).  Otherwise,
  ->op = NA.  ->args_found is set to 0 or 1 as appropriate.

  An argument is a sequence of decimal digits; spaces are ignored.

  The name and arg pointers point to a buffer contains in the
  'tokens_res' structure.

DEPENDENCIES
  Assumes the argument, 's' points to the first character in the
  name with no leading blanks.

RETURN VALUE
  A pointer to the next character in the input string is returned.  It
  will point to the first non-space, non-decimal character following
  the last command character, or to the NULL terminator.

SIDE EFFECTS
  If the internal buffer overflows, the global "result_code"
  is set to ERROR.

===========================================================================*/
LOCAL byte * parse_single_char_cmd
(
  byte *s,                          /*  String to parse           */
  tokens_struct_type *tokens_res    /*  Structure to hold results */
)
{
  byte c, *scr_ptr;

  tokens_res->op = NA;
  tokens_res->name = &tokens_res->working_at_line[0];
  tokens_res->args_found = 0;

  tokens_res->working_at_line[0] = *s++;
  scr_ptr = &tokens_res->working_at_line[1];
  if ( tokens_res->working_at_line [0] == '&' )
  {
    while ( (c = *s) != 0 )
    {
      ++s;
      if ( c != ' ' )
      {
        break;
      }
    }
    *scr_ptr++ = c;
  }
  *scr_ptr++ = '\0';

  tokens_res->arg[0] = scr_ptr;

  while ( (c = *s) != 0 )
  {
    if ( c >= '0' && c <= '9' )
    {
      *scr_ptr++ = c;
      tokens_res->op |= (EQ|AR);
      tokens_res->args_found = 1;
      if ( scr_ptr == &tokens_res->working_at_line[MAX_LINE_SIZE-1] )
      {
        result_code = DSAT_ERROR;
        DSATI_ERROR_MSG();
        break;
      }
    }
    else if ( c != ' ' )
    {
      break;
    }
    
    ++s;
  }

  *scr_ptr = '\0';
  tokens_res->end_of_line = scr_ptr;
  
  return( s ) ;

} /* parse_single_char_cmd( ) */


/*===========================================================================

FUNCTION PARSE_SREGISTER_CMD

DESCRIPTION
  This function parses S-register commands. The result structure is filled
  in as follows:

  The 'name' field in the structure points to the first non-blank
  character in the input string.

  The 'arg[0]' field point to the first character following an = or ?.

  The 'op' field contains the combined value of a set of bit-mapped
  manifest constants:
    NA is set at the beginning of the command name;
    EQ is set when an '=' is found after the name;
    QU is set when a '?' is found after the name;
    AR is set when a decimal digit is found after EQ is set;

  The 'args_found' is set to 1 if an argument is found.

  The name and arg pointers point to a buffer contains in the
  'tokens_res' structure.

  The 'end_of_line' field points to the end of data (last NULL character)
  placed in the 'working_at_line' buffer.
  
DEPENDENCIES
  Assumes the argument, 's', points to the first character in the
  name, with no leading spaces.

RETURN VALUE
  The value returned is a pointer to the next character in the input
  stream to be processed.  It points to the first non-decimal character
  after the name, or to the NULL terminator.

SIDE EFFECTS
  If the internal buffer overflows, the global "result_code" is
  set to ERROR.

===========================================================================*/
LOCAL byte * parse_sregister_cmd
(
  byte *s,                          /*  String to parse           */
  tokens_struct_type *tokens_res    /*  Structure to hold results */
)
{
  byte c, *scr_ptr;

  tokens_res->op = NA;
  tokens_res->name = &tokens_res->working_at_line[0];
  tokens_res->args_found = 0;

  tokens_res->working_at_line[0] = *s++;
  scr_ptr = &tokens_res->working_at_line[1];
  while ( (c = *s) != 0 ) 
  {
    ++s;

    if ( c == '=' )
      {
      tokens_res->op |= EQ;
      break;
      }
    else if ( c == '?' )
    {
      tokens_res->op |= QU;
      break;
    }


    if ( c != ' ' )
    {
      if ( scr_ptr >= &tokens_res->working_at_line[MAX_LINE_SIZE-1] )
      {
        result_code = DSAT_ERROR;
        DSATI_ERROR_MSG();
        break;
      }
      *scr_ptr++ = c;
    }
  }
  *scr_ptr++ = '\0';

  tokens_res->arg[0] = scr_ptr;

  while ( (c = *s++) == ' ' || (c >= '0' && c <= '9') )
  {
    if ( c != ' ' )
    {
      if ( scr_ptr >= &tokens_res->working_at_line[MAX_LINE_SIZE-1] )
      {
        result_code = DSAT_ERROR;
        DSATI_ERROR_MSG();
        break;
      }
      *scr_ptr++ = c;
      tokens_res->args_found = 1;
      tokens_res->op |= AR;
    }
  }

  *scr_ptr = '\0';
  tokens_res->end_of_line = scr_ptr;
  
  return (s - 1) ;
  
} /*  parse_sregister_cmd */


/*===========================================================================

FUNCTION PARSE_HARD_TO_PARSE_CMD

DESCRIPTION
  Processes the hard to parse commands.

  These commands have no delimiters between the name and the argument.
  Therefore, the name string from the parser is searched for the command
  name, one character at a time, discarding spaces.  If found,
  the remainder of the string is treated as the argument, up to and
  including the ';' character.

  The item found is indicated by the result code returned.

  The 'tokens_struct' is filled in as though an '=' followed by
  a single argument had been found.

DEPENDENCIES
  None

RETURN VALUE
  The value returned is a pointer to the next character in the input
  stream to be processed.    

SIDE EFFECTS
  The global "result_code" may be set to DSAT_UNREC to indicate a
  command match was not found or DSAT_ERROR to indicate an internal
  buffer overflowed.
  
===========================================================================*/
/*-------------------------------------------------------------------------*/
/*                  Hard items to parse                                    */
/*-------------------------------------------------------------------------*/

#define N_HARD_ITEMS  1

static const byte  hard_to_parse_name[N_HARD_ITEMS][7] =
  { "D" };

/*-------------------------------------------------------------------------*/
/*                  End of hard item definition                            */
/*-------------------------------------------------------------------------*/

LOCAL byte * parse_hard_to_parse_cmd
(
  byte *cmd_string,               /*  Command to parse  */
  tokens_struct_type *tok_ptr     /*  results of parse  */
)
{
  byte c, *r_ptr, *w_ptr;
  const byte *htp_ptr;
  int n, t;

  r_ptr = NULL;
  /*-----------------------------------------------------------------------*/
  /*                  Begin Loop thru the item list                        */
  /*-----------------------------------------------------------------------*/

  for ( n = 0; n < N_HARD_ITEMS; ++n )
  {
    htp_ptr       = hard_to_parse_name[n];    /*  cmd name to search for   */
    r_ptr         = cmd_string;               /*  command to parse         */
    w_ptr         = tok_ptr->working_at_line; /*  buffer                   */
    tok_ptr->name = tok_ptr->working_at_line; /*  loc of cmd name in buf   */
    t             = 1;                        /*  found flag               */

    /*---------------------------------------------------------------------*/
    /*              Loop thru nth item, looking for character match        */
    /*---------------------------------------------------------------------*/

    while ( (c = *r_ptr) != '\0' && c != ';' && t != 0 && *htp_ptr )
    {
      if ( c != ' ' )
      {
        if ( UPCASE( c ) == *htp_ptr )
        {
          ++htp_ptr;    /*  Found match, increment index in nth item  */
          ASSERT( w_ptr < &tok_ptr->working_at_line[MAX_LINE_SIZE] );
          *w_ptr++ = c; /*  copy name to buffer   */
          ++r_ptr;      /*  bump read pointer only if used  */
        }
        else
        {
          t = 0;        /*  No match, exit loop */
        }
      }
      else
      {
        ++r_ptr;        /*  bump read pointer if char skipped */
      }
    }
    *w_ptr++ = '\0';    /*  Terminate name  */

    if ( t != 0 )
    {
      /*-------------------------------------------------------------------*/
      /*                 Match found:                                      */
      /*              copy argument up to ';' or null                      */
      /*-------------------------------------------------------------------*/
      tok_ptr->arg[0] = w_ptr;
      tok_ptr->args_found = 1;
      tok_ptr->op = NA|EQ;

      while ( (c = *r_ptr) != '\0' )
      {
        /* Make sure there is room to store argument in buffer. */
        if ( w_ptr >= &tok_ptr->working_at_line[MAX_LINE_SIZE-1] )
        {
          result_code = DSAT_ERROR;
          DSATI_ERROR_MSG();
          break;
        }

        /* Write character to buffer. */
        *w_ptr++ = c;
        ++r_ptr;

        /* If ';' has been included in argument, exit loop */    
        if ( c == ';' )         
        {
          break;

        }
      }
 
      *w_ptr = '\0';            /*  Terminate argument  */

      tok_ptr->end_of_line = w_ptr;
      return r_ptr;
    }
  }

  result_code =  DSAT_UNREC;
  tok_ptr->end_of_line = w_ptr;
  return r_ptr;
  
} /* parse_hard_to_parse_cmd( ) */


/*===========================================================================

FUNCTION TRIM_STRING

DESCRIPTION
  This function scans the string pointed to by the parameter for 
  leading and trailing spaces.  The string terminator is set to 
  after last non-space character.

DEPENDENCIES
  None

RETURN VALUE
  Return new string length

SIDE EFFECTS
  None

===========================================================================*/
int trim_string 
(
   byte **start   /*  Pointer to beginning of string */
)
{
  byte *end = *start;

  /* Advance to non-space character */
  while (*(*start) == ' ') 
  {
    (*start)++;
  }   

  /* Move to end of string */
  while (*end != '\0') 
  {
    (end)++;
  }
  /* Backup to non-space character */
  while ((end >= *start) && *(--end) == ' ') {}

  /* Terminate string after last non-space character */
  *(++end) = '\0';
  return (end - *start);
} /* trim_string */


#endif /* FEATURE_DATA */
