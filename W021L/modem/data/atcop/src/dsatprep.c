/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
       A T   C O M M A N D   S I O   D A T A   P R E P R O C E S S O R

GENERAL DESCRIPTION
  This module contains the externally callable functions which interface
  the AT Command Processor to data received by serial I/O.  It
  preprocesses the received data differently for the three modes of
  operation it supports:
        - AT command mode
        - Abort command mode
        - SMS message entry mode
  
EXTERNALIZED FUNCTIONS
  dsat_process_sio_command
    This function is to be called from the data services task to process 
    incoming data from the serial port, when the AT command processor
    is in AT command mode or online command mode.
    
EXTERNALIZED FUNCTIONS INTERNAL TO DSAT UNIT    
  dsatprep_set_preprocessor_mode
    Changes the SIO data preprocessor mode of operation.
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  The AT command processor must be initialized by calling the function
  dsat_init on initial power-up and calling dsat_init_service_mode
  each time a new operating service mode is entered.


  Copyright (c) 1995 - 2009 by QUALCOMM Incorporated.
  All Rights Reserved.
  Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  
  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatprep.c_v   1.7   22 Oct 2002 13:33:12   sbandaru  $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatprep.c#3 $ $DateTime: 2009/07/29 07:43:57 $ $Author: bhaviks $

when       who     what, where, why
-------    ---     ----------------------------------------------------------
07/23/09   bs      Corrected to traverse all the packets in a  DSM Pkt chain.
01/21/09   nc      Added support for SMS Commands 
02/14/03   wx      Add ABORTING state to the abortable command to 
                   handle the aborting in async command processing.
10/22/02   sb      Merge of latest ETSI related changes
10/09/02   sb      Merged the latest ETSI file
09/12/02    ar     Merged in SIO interface changes from MM_DATA
06/11/02    wx     Exit process_at_cmd_mode function for every complete AT cmd line
                   to check preprocessor_mode.
08/10/01   sjd     Initial release to MSM5200 archives.

===========================================================================*/


/*===========================================================================

                          INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"       /* Customer Specific Features */

#ifdef FEATURE_DATA           

#include "target.h"
#include "err.h"
#include "assert.h"
#include "dsati.h"

#ifdef FEATURE_DATA_MM
  #include "ds3gsiolib.h"
#else
  #include "ds3gsioif.h"
#endif /* FEATURE_DATA_MM */

#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
#include "dsatsms.h"
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_ETSI_SMS) */


/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/

/*===========================================================================

                    LOCAL DEFINITIONS AND DECLARATIONS

===========================================================================*/

/*--------------------------------------------------------------------------
  Debug macro for this module.
---------------------------------------------------------------------------*/
#ifdef DEBUG_DSATCOP
#define DEBUG( x )  { x }
#else
#define DEBUG( x )
#endif /* DEBUG_DSATCOP */

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
/*--------------------------------------------------------------------------
   Character definitions and strings for SMS message entry mode.
---------------------------------------------------------------------------*/
#define CTRL_Z_CHAR  '\x1A'
#define ESC_CHAR     '\x1B'

LOCAL const char *prompt = "\r\n> ";
LOCAL const char *eom    = "\r\n";
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_ETSI_SMS) */

/* point to current dsm item under processing */
LOCAL dsm_item_type * atprep_dsm_item_ptr = NULL;     

LOCAL byte cmd_line_buffer[MAX_LINE_SIZE];     /*  buffer for AT cmd line  */
LOCAL byte prev_line_buffer[MAX_LINE_SIZE];  /*  buffer previous AT cmd line */

/*--------------------------------------------------------------------------
   SIO data preprocessor mode of operation.
---------------------------------------------------------------------------*/
LOCAL dsati_preproc_mode_enum_type preprocessor_mode = DSATI_AT_CMD_MODE;

/*--------------------------------------------------------------------------
    The following type represents the states for the command line   
    parser:                                                                
---------------------------------------------------------------------------*/
typedef enum       
{                                                                      
  HUNT,              /*  Initial:  looking for a or A                      */
  FOUND_A,           /*  Found A, looking for t, T or /                    */
  FOUND_AT,          /*  Found AT: filling buffer and processing backspace */
  ERROR              /*  Error!  loop until end of line                    */
} at_cmd_prep_state_enum_type;

LOCAL at_cmd_prep_state_enum_type at_cmd_prep_state = HUNT; 

LOCAL byte *build_cmd_ptr;                     /* next loc in buffer       */
LOCAL byte *prev_cmd_ptr;                      /* next loc in buffer       */

/*---------------------------------------------------------------------------
    Local function declarations to allow the desired function ordering.    
---------------------------------------------------------------------------*/

static void dump_echo
(
  dsm_item_type **
);

static void store_echo
(
  byte,  
  dsm_item_type **
);

static boolean process_at_cmd_mode
(
  dsat_mode_enum_type,
  dsm_watermark_type *
);

static boolean process_abort_cmd_mode
(
  dsat_mode_enum_type,
  dsm_watermark_type *
);

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
static boolean process_sms_msg_entry_mode
(
  dsat_mode_enum_type,
  dsm_watermark_type *
);

static void send_string
(
  const char *
);
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_ETSI_SMS) */



/*===========================================================================

FUNCTION dsatprep_set_preprocessor_mode

DESCRIPTION
  Changes the SIO data preprocessor mode of operation.  Operating modes
  can be: AT command mode, abort command mode, or SMS message entry mode.
  AT command mode is the default mode of SIO data preprocessor operation.
  It handles normal AT command line entry.  Abort command mode provides
  processing to abort an abortable command when any character is received.
  SMS message entry mode handles input of SMS messages.

  A state machine in this function controls transitions between SMS
  message entry mode and abort command mode when abortable commands are
  being processed.  When an abortable command is being processed and
  current mode is SMS message entry mode, entry into abort command mode
  will be delayed until the next call to this function.  Once abort
  command mode is exited, AT command mode will always be entered.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  Mode changes may be delayed when abortable SMS commands are being
  processed (see description above).  SIO AUTODETECT mode is rearmed
  on entry into AT command line mode.

===========================================================================*/
void dsatprep_set_preprocessor_mode
(
  dsati_preproc_mode_enum_type mode    /* mode to set data preprocessor to */
)
{
#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
  /*------------------------------------------------------------------------
    This simple state machine is used to control transitions between
    abort command mode and SMS message entry mode when an abortable
    command is being processed.  It insures proper abort command
    processing of abortable SMS commands using SMS message entry mode.
  ------------------------------------------------------------------------*/
  {
    static dsati_preproc_mode_enum_type next_mode = DSATI_INVALID_MODE;

    if ( dsatcmdp_abortable_state != NOT_ABORTABLE )
    {
      /* Processing an abortable command. */
      
      if ( mode == DSATI_ABORT_CMD_MODE &&
           preprocessor_mode == DSATI_SMS_MSG_ENTRY_MODE )
      {
        /* Currently in SMS mode and directed to abort mode, so delay
           entry into abort mode until next call to this function. */
        next_mode = mode;
        
        return;
      }
      else if ( next_mode == DSATI_ABORT_CMD_MODE )
      {
        /* Entry into abort mode was delayed, so enter it now.  Once
           abort command processing is completed, AT command mode
           will be entered. */
        mode = next_mode;
        next_mode = DSATI_INVALID_MODE;
      }
    }
    else
    {
      /* Insure we reset next mode when not processing an abortable
         command. */
      next_mode = DSATI_INVALID_MODE;
    }
  }
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */

  /*------------------------------------------------------------------------
    Normal processing of changes between SIO preprocessor modes.
  ------------------------------------------------------------------------*/
  switch ( mode )
  {
  case DSATI_AT_CMD_MODE:
    /* Enter AT command mode. */
    preprocessor_mode = mode;

    /* Reset AT command preprocessor state machine. */
    at_cmd_prep_state = HUNT;

    /* Re-arm SIO autodetect mode. */
#ifdef FEATURE_DATA_MM
   (void) ds3g_siolib_rearm_autodetect( );
#else
    (void) ds3g_sio_rearm_autodetect( );
#endif /* FEATURE_DATA_MM */
    break;
    
  case DSATI_ABORT_CMD_MODE:
    /* Enter abort command mode. */
    preprocessor_mode = mode;    
    break;

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
  case DSATI_SMS_MSG_ENTRY_MODE:
    /* Enter SMS message entry mode. */
    preprocessor_mode = mode;    

    /* Send SMS message entry prompt to TE. */
    send_string( prompt );

    /* Empty current dsm_item */
    if(atprep_dsm_item_ptr != NULL)
    {
      atprep_dsm_item_ptr->used = 0;
      (void) dsm_free_buffer( atprep_dsm_item_ptr );
      atprep_dsm_item_ptr = NULL;
    }

    /* Assert the DCD line for SMS message entry. */
#ifdef FEATURE_DATA_MM
   (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_SMS_MSG_START );
#else
    (void) ds3g_sio_set_cd_state( DS3G_SIO_SMS_MSG_START );              
#endif /* FEATURE_DATA_MM */
                  

    break;
case DSATI_SMS_MSG_END_MODE:
  /* We have either received the message complete character (^Z) or */
  /* escape character. Reset the AT command mode to AT state but do */
  /* not enable the autodetect yet. Eventual result code generation */
  /* will enable the autodetect mode                                */
  preprocessor_mode = DSATI_AT_CMD_MODE;

  /* Reset AT command preprocessor state machine. */
  at_cmd_prep_state = HUNT;
  break;

#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_ETSI_SMS) */

  default:
    ERR( "Invalid SIO data preprocessor mode %d", mode, 0, 0 );
  }
  
} /* dsatprep_set_preprocessor_mode( ) */


/*===========================================================================

FUNCTION DSAT_PROCESS_SIO_COMMAND

DESCRIPTION
 This function invokes SIO data preprocessor that filters and echoes
 received characters, handles line editing, based on internal operating
 mode.  Operating modes can be AT command line, abort command, or SMS
 message entry mode.  Operating modes are changed during SMS command
 processing and when abortable commands are being processed.

 In AT command line mode, once AT command line parsing is completed
 AT commands are processed.  AT commands are processed based on AT
 state (Command or Online Command) and operating service mode.
 
 In abort command mode, any received character causes the command
 executing to be aborted.  The SIO data preprocessor returns a Boolean
 indicating the call should be aborted for call related commands or it
 calls a function to terminate other abortable commands.

 In SMS message entry mode, the execution of SMS command is completed
 after complete SMS message is received or the command is cancelled.

 Called from 3G DSMGR when data is received from SIO.

DEPENDENCIES
  None

RETURN VALUE
  Abort call result; TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  There are side effects associated with processing a command line,
  aborting a command, and SMS message entry.

===========================================================================*/
boolean dsat_process_sio_command
(
  dsat_mode_enum_type at_state,         /* Command, online data, or online
                                           command state */
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           retrieve command line data from */
)
{
  boolean abort_call = FALSE;           

  /*-------------------------------------------------------------------------
    Initialize the regional AT state variable.
  -------------------------------------------------------------------------*/  
  dsatcmdp_at_state = at_state;

  /*-------------------------------------------------------------------------
    Process data in watermark item based on SIO data preprocessor mode.
  -------------------------------------------------------------------------*/  
  while (atprep_dsm_item_ptr != NULL || 
         (atprep_dsm_item_ptr = dsm_dequeue( wm_item_ptr )) != NULL )
  {
    switch ( preprocessor_mode )
    {
    case DSATI_AT_CMD_MODE:
      abort_call = process_at_cmd_mode( at_state, wm_item_ptr );
      break;
    
    case DSATI_ABORT_CMD_MODE:
      abort_call = process_abort_cmd_mode( at_state, wm_item_ptr );
      break;

#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
    case DSATI_SMS_MSG_ENTRY_MODE:
      abort_call = process_sms_msg_entry_mode( at_state, wm_item_ptr );
      break;
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_ETSI_SMS) */

    default:
      ERR_FATAL("Invalid preprocessor_mode: %d",preprocessor_mode,0,0);
      break;
      
    } /* switch ( preprocessor_mode ) */

    /* free current dsm_item if all the data in it has been processed */
    if((atprep_dsm_item_ptr != NULL) && (!atprep_dsm_item_ptr->used))
    {
      atprep_dsm_item_ptr = dsm_free_buffer( atprep_dsm_item_ptr );
    }
  }
    
  return abort_call;
  
} /* dsat_process_sio_command( ) */


/*===========================================================================

FUNCTION PROCESS_AT_CMD_MODE

DESCRIPTION
  This function builds the AT command line from data obtained from
  the dsm item passed in. The command line begins with the "AT"
  characters (either case or mixed case, but with nothing in between), 
  and is terminated with the end of line character stored in modem 
  register S3.  The backspace character (in modem register S5) is processed.
  
  When a complete line has been accumulated, dsat_process_cmd_line( ) is
  called. The line will have been edited per the backspace (above), and the
  AT removed.  The end of line character will have been replaced with the null
  character, '\0'.  Any spaces will still be present. The eighth bit of all 
  characters will have been set to zero.

  If the character combination, "A/" or "a/" is encountered while looking
  for the beginning of a command line, the previous command line, if any,
  will be immediately passed to dsat_process_cmd_line( ).

  If the command line buffer overflows, the data will be discarded and
  an error flag set.  The error will be reported when the end of line
  character in S3 is encountered.

  Input data will be echoed to the serial port.  Data is echoed at the end
  of each input data item, when a small data item (used for echo output) is
  filled, or when the S3 character is found, or the buffer overflows,
  whichever comes first.  The echo is controlled by the 'ATE' flag.

DEPENDENCIES
  None

RETURN VALUE
  Always FALSE

SIDE EFFECTS
  Buffer accumulates data until a full line is found.

  See also dsat_process_cmd_line which has many side effects.

===========================================================================*/
static boolean process_at_cmd_mode
(
  dsat_mode_enum_type at_state,         /* Command, online data, or online
                                           command state */
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           retrieve command line data from */
)
{
  byte *item_data_ptr;                  /* item from queue data pointer    */
  dsm_item_type *echo_item_ptr = NULL;  /* echo item                       */
  byte cc;                              /* current character               */
  byte cce;                             /* current echo character          */
  boolean processing_cmd_line = TRUE;   /* this flag will be set to FALSE 
                                           if one complete at cmd line is 
                                           processed */


  /*-------------------------------------------------------------------------

      The following loop implements a state machine, with the incoming 
      characters being the events.

                                                _____
                                               /     | incoming stream 
   +--------+  A,a   +---------+       +---------+   | copied to 
   |        |------->|         | T, t  |         |___| cmd_line_buffer.
 ~>|  HUNT  | /,A,a  | FOUND_A |------>| FOUND_AT|                   +-------+
   |        |<-------|         |       |         |------------------>| ERROR |
   +--------+        +---------+       +---------+ cmd_line_buffer   +-------+
       ^                                   |       overflow.             |
       |                                   |                             |
       |___________________________________V_____________________________|
                                       EOL


      In the HUNT state, the characters 'A' or 'a' are looked for.
      If they are found, the machine goes to the FOUND_A state.

      The FOUND_A state looks for 't', 'T', or '/', or another 'a' or 'A'.
      't' or 'T' causes a transition to the FOUND_AT state.
      '/' causes immediate processing of the previous AT command line
      which is still in cmd_line_buffer, followed by a transition to 
      the HUNT state.  'a' or 'A' causes the machine to remain in the 
      FOUND_A state.  Anything else causes a return to the HUNT state.

      The FOUND_AT state causes the incoming stream to be copied to 
      cmd_line_buffer[].  If a backspace character (dsat_s5_val) 
      is encountered, one character is erased from cmd_line_buffer[].  
      When the buffer is emptied, next backspace sets state to FOUND_A.
      If an end of line character (dsat_s3_val) is found, 
      dsat_process_cmd_line( ) is called with cmd_line_buffer[] as an
      argument, followed by a transition to the HUNT state.
      A buffer over flow causes a transition to the ERROR state.

      The ERROR state does nothing until an end of line character 
      (dsat_s35_val) is found.  Then, the machine returns to the
      HUNT state.

  -------------------------------------------------------------------------*/

  while (processing_cmd_line && 
         ( atprep_dsm_item_ptr != NULL ||
           (atprep_dsm_item_ptr = dsm_dequeue( wm_item_ptr )) != NULL) )
  {
    /*---------------------------------------------------------------------
      Dequeue each item from watermark, extract AT command line, and pass
      to AT command parser.
    ---------------------------------------------------------------------*/

    item_data_ptr = atprep_dsm_item_ptr->data_ptr;
    while ( processing_cmd_line && atprep_dsm_item_ptr->used )
    {
      atprep_dsm_item_ptr->used--;               /* Decrement used count.            */
      cce = *item_data_ptr++;         /* Assign to echo character.        */
      cc = cce & 0x7F ;               /* Only the 7 upper bits are used.  */

        DEBUG(
              MSG_LOW("cce:0x%x, cc:%c, cmd_prep_state: %d",
                      cce, cc, at_cmd_prep_state);
              );

        store_echo( cce, &echo_item_ptr );

      /*-------------------------------------------------------------------
        The following switch statements is a multi-way decison that tests 
        the current state of the AT State Machine and processes the incoming
        characters accordingly. The state machine starts initially in the 
        HUNT state. 
      -------------------------------------------------------------------*/
      switch ( at_cmd_prep_state )
      {
       
      case HUNT:                 
        /*-----------------------------------------------------------------
          In the HUNT state, the characters 'A' or 'a' are looked for.  
          Everything else is ignored. If 'A' or 'a' is detected, the   
          machine goes into the FOUND_A state.                              
        -----------------------------------------------------------------*/
        if ( UPCASE( cc ) == 'A' )
        {
          at_cmd_prep_state = FOUND_A;   /* Transition into the FOUND_A
                                            state. */

          /* Set regional flag to indicate indications should not be
             sent to TE at this time. */
          dsatcmdp_block_indications_flag = TRUE;
        }
        else
        {
          if (echo_item_ptr != NULL)
          {
            echo_item_ptr->used = 0;      /*  Kill echo */
          }
        }
    
        break;
    
       
      case FOUND_A:              
        /*-----------------------------------------------------------------
          In the FOUND_A state, detection of 'T' or 't' will take the state
          machine into the FOUND_AT state; 'A'or 'a' will result in
          a transition looping back to the FOUND_A state; '/' will result
          in the immediate processing of the previous AT command line;
          everything else will result in a transition to HUNT state and
          rearming the SIO autodetect.
        -----------------------------------------------------------------*/
        if ( cc == '/' )
        {
            /* echo it now before process the previous cmd */
            dump_echo( &echo_item_ptr );

          /*--------------------------------------------------------------
            'A/', request for the re-execution of previous command,   
            is detected. Process the previous AT command line     
            immediately.                                              
           --------------------------------------------------------------*/
          dsat_process_cmd_line( at_state, prev_line_buffer );
          at_cmd_prep_state = HUNT;      /* Transition back to the HUNT
                                            state.*/
          processing_cmd_line = FALSE;
        }
        else if ( UPCASE( cc ) == 'T' )
        {
          /*-------------------------------------------------------------- 
            Write 'T' or 't' into the buffer and transition to the
            the FOUND_AT state. 
          --------------------------------------------------------------*/
          build_cmd_ptr = &cmd_line_buffer[0];
          at_cmd_prep_state = FOUND_AT;             
        }
        else if ( UPCASE( cc ) != 'A' )
        {
          /*-------------------------------------------------------------- 
            Transition back to the HUNT state. 
          --------------------------------------------------------------*/
          at_cmd_prep_state = HUNT;          

          /*  Kill echo if not backspace */
          if ( cc != dsat_s5_val)
          {
            if (echo_item_ptr != NULL)
            {
              echo_item_ptr->used = 0;
            }
          }

          /* Clear regional flag to indicate indications can now be
             sent to TE. */
          dsatcmdp_block_indications_flag = FALSE;

          /* Re-arm SIO autodetect mode. */
#ifdef FEATURE_DATA_MM
          (void) ds3g_siolib_rearm_autodetect( );
#else
          (void) ds3g_sio_rearm_autodetect( );
#endif /* FEATURE_DATA_MM */
        }
    
        break;
    
    
      case FOUND_AT:            
        /*-----------------------------------------------------------------
          When in the FOUND_AT state, the incoming stream is written
          to cmd_line_buffer[]. A backspace character will result        
          in the removal of a single character from the                 
          cmd_line_buffer. When EOF is encountered, the content of       
          cmd_line_buffer is processed as an argument and state      
          machine goes back to the HUNT state. The buffer status       
          is being monitored, an overflow will result in a            
          transition into the ERROR state.                             
        -----------------------------------------------------------------*/
        if ( cc != dsat_s3_val )   
        {
          /*---------------------------------------------------------------
            If not backspace, fill buffer.
          ---------------------------------------------------------------*/
          if ( cc != dsat_s5_val )  
          {
            /*-------------------------------------------------------------
              When overflow occurs,  transition into the ERROR state 
              will take place and the buffer will be reinitialized. 
            -------------------------------------------------------------*/
            if ( build_cmd_ptr >= (cmd_line_buffer
                                   + sizeof( cmd_line_buffer ) - 2) )
            {
              at_cmd_prep_state = ERROR;             
              cmd_line_buffer[0] = '\0';   
            }
            /*------------------------------------------------------------
              If no overflow, fill buffer.  
            ------------------------------------------------------------*/
            else if ( cc >= 0x20 && cc <= 0x7E )
            {
              *build_cmd_ptr++ = cc;
            }
          }
    
          /*--------------------------------------------------------------
            Backspace encountered. Remove the most immediate character
            from the buffer.  Go back to FOUND_A state. 
          --------------------------------------------------------------*/
          else  
          {
            if ( --build_cmd_ptr < &cmd_line_buffer[0] )
            {
              build_cmd_ptr = &cmd_line_buffer[0];
              cce = '\0';      /*  Kill echo */
              at_cmd_prep_state = FOUND_A;             
            }
          }
        }
    
        /*-----------------------------------------------------------------
          EOL found, terminate and parse  
        -----------------------------------------------------------------*/
        else                       
        {
          /* Update DSM used to reflect EOL reached.                     */
          /* This avoids issue where software sends null bytes after S3. */
          atprep_dsm_item_ptr->used = 0;

          *build_cmd_ptr = '\0';
          if ( build_cmd_ptr >= (cmd_line_buffer + sizeof( cmd_line_buffer )) )
          {
            /* This should never happen */
            MSG_HIGH ("OVERFLOW", 0, 0, 0);
          }
          
          /*---------------------------------------------------------------
            Update previous command buffer.  Second buffer used as the main 
            buffer can be overwritten before the A/ sequence is processed.  
          ---------------------------------------------------------------*/
          build_cmd_ptr = &cmd_line_buffer[0];
          prev_cmd_ptr = &prev_line_buffer[0];
          while ('\0' != (*prev_cmd_ptr++ = *build_cmd_ptr++ )) {}
          
          /*---------------------------------------------------------------
              Dump echo so it will go out port before the result 
            of the command does (!) Then process the current content of 
            the buffer. 
          ---------------------------------------------------------------*/
          dump_echo( &echo_item_ptr );
    
          dsat_process_cmd_line( at_state, cmd_line_buffer );
          at_cmd_prep_state = HUNT;
          
          processing_cmd_line = FALSE;
        }
    
        break;
    
        
      case ERROR:                 
        /*-----------------------------------------------------------------
          The ERROR state does nothing until an end of line character is 
          found.  Then, the machine returns to the HUNT state.
        -----------------------------------------------------------------*/
        if ( cc == dsat_s3_val )
        {
          /*---------------------------------------------------------------
            Echo line character, send error result code to TE, and
            return to HUNT state.
          ---------------------------------------------------------------*/
    
          dump_echo( &echo_item_ptr );
    
          DSATI_ERROR_MSG( );
          dsat_send_result_code( DSAT_ERROR );
          at_cmd_prep_state = HUNT;          /* Transition back to HUNT. */
        }
        break;
        
      }  /* end switch ( at_cmd_prep_state ) */
    
    }  /* end while ( atprep_dsm_item_ptr ) */
    
    dump_echo( &echo_item_ptr );

    if(!atprep_dsm_item_ptr->used)
    {
      atprep_dsm_item_ptr = dsm_free_buffer( atprep_dsm_item_ptr );
    }
  } /*  end of input from Rm  */
      
  ASSERT( echo_item_ptr == NULL );

  return FALSE;
  
} /* process_at_cmd_mode( ) */

/*===========================================================================

FUNCTION PROCESS_ABORT_CMD_MODE

DESCRIPTION
  This function performs the processing of receive characters to
  abort the current command in progress.  It performs the following
  functions during abort command mode:
  
   - Initiates aborting of command when any character is received
   - Flushes all remaining characters from watermark after abort
     character is received
   - Sets SIO data preprocessor mode to DSATI_AT_CMD_MODE after
     abort character is received

DEPENDENCIES
  None

RETURN VALUE
  Value indicating whether or not call should be aborted:
      TRUE if call should be aborted, FALSE otherwise.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
static boolean process_abort_cmd_mode
(
  dsat_mode_enum_type at_state,         /* Command, online data, or online
                                           command state */
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           abort command data from */
)
{
  boolean abort_call = FALSE;               /* return value for aborting call  */

  
  /*-------------------------------------------------------------------------
    Dequeue all items from watermark and empty each item.
  -------------------------------------------------------------------------*/
  while ( atprep_dsm_item_ptr != NULL ||
          (atprep_dsm_item_ptr = dsm_dequeue( wm_item_ptr )) != NULL )
  {

    if ( atprep_dsm_item_ptr->used > 0)
    {
      atprep_dsm_item_ptr->used = 0;               /* reset used count.            */
      abort_call = TRUE;
    }  

    (void) dsm_free_buffer( atprep_dsm_item_ptr );
    atprep_dsm_item_ptr = NULL;
  } /*  end of input from Rm  */
      
  /*-------------------------------------------------------------------------
    Abort current commmand in progress.  Note, this function only
    calls the handler to abort the command once during the abort
    processing of a command.  This is required since this function
    may be called multiple times during the abort processing of a
    command.
  -------------------------------------------------------------------------*/
  if(abort_call)
  {
    abort_call = dsatcmdp_abort_command( );
  }

  return abort_call;

} /* process_abort_cmd_mode( ) */


#if defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS)
/*===========================================================================

FUNCTION PROCESS_SMS_MSG_ENTRY_MODE

DESCRIPTION
  This function performs the SMS message entry data preprocessing
  necessary to accept an SMS message.  It performs the following
  functions on SMS data entered:
  
   - Echo characters entered based on E command setting
   - Perform line editing functions based on command line editing
     character (S5 parameter) for SMS text mode only
   - Detect <ESC> character and cancel message sending or writing
     command when received
   - Detect <ctrl-Z> to indicate end of message body  
   - Complete processing of SMS send or write command when <ctrl-Z> is
     received
   - Each time <CR> is received the TA sends the
     <CR><LF><greater_than_sign><space> prompt to the TE  (text mode only)
   - Set the SIO data preprocessor mode to DSATI_AT_CMD_MODE once <ctrl-Z>
     or <ESC> is received and return DCD to its previous state
                
DEPENDENCIES
  None

RETURN VALUE
  Always returns FALSE.

SIDE EFFECTS
  None

===========================================================================*/
/* ARGSUSED */
static boolean process_sms_msg_entry_mode
(
  dsat_mode_enum_type at_state,         /* Command, online data, or online
                                           command state */
  dsm_watermark_type *wm_item_ptr       /* Pointer to watermark item to
                                           retrieve SMS message from */
)
{
  dsm_item_type *echo_item_ptr = NULL;        /* echo item                 */
  byte *item_data_ptr;                        /* item from queue data ptr  */  
  static dsm_item_type *msg_item_ptr = NULL;  /* ptr to SMS message item   */
  static byte *msg_ptr;                       /* pointer to SMS message    */
  static byte *start_line_ptr;                /* start of line pointer     */
  byte cc;                                    /* current character         */
  byte cce;                                   /* current echo character    */
  enum                                        /* SMS message entry states  */
  {
    NORMAL_STATE,
    ERROR_STATE
  } sms_msg_entry_state;
 
#if defined (FEATURE_WCDMA) ||  defined (FEATURE_GSM)
#error code not present
#endif /* defined (FEATURE_WCDMA) ||  defined (FEATURE_GSM) */   
  /*----------------------------------------------------------------------
    Get a new buffer to hold the SMS message if one is not already
    allocated.
  ----------------------------------------------------------------------*/
  if ( msg_item_ptr == NULL )
  {
    msg_item_ptr = dsm_new_buffer( DSM_DS_LARGE_ITEM_POOL );
    if ( msg_item_ptr == NULL )
    {
      ERR( "Running out of buffers!",0,0,0 );
    
      /* Notify SMS command processor an error has occurred and
         command should be cancelled. */
      dsatsms_abort_cmd( DSAT_CMS_ME_FAILURE );
      
      /* Return DCD to its state before SMS message entry and set
         SIO data preprocessor back to AT command mode. */      
#ifdef FEATURE_DATA_MM
          (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_SMS_MSG_END );
#else
        (void) ds3g_sio_set_cd_state( DS3G_SIO_SMS_MSG_END );
#endif /* FEATURE_DATA_MM */
      dsatprep_set_preprocessor_mode( DSATI_AT_CMD_MODE );

      return FALSE;
    }

    msg_ptr = start_line_ptr = msg_item_ptr->data_ptr;
  }
  
  /*---------------------------------------------------------------------
    Dequeue each item from watermark, extract SMS message, and pass
    to SMS message handler.
  ---------------------------------------------------------------------*/

  sms_msg_entry_state = NORMAL_STATE;
  while ( atprep_dsm_item_ptr != NULL ||
          (atprep_dsm_item_ptr = dsm_dequeue( wm_item_ptr )) != NULL )
  {

    item_data_ptr = atprep_dsm_item_ptr->data_ptr;
    while ( atprep_dsm_item_ptr->used &&
            msg_item_ptr )
    {
        boolean store_char = TRUE;    /* flag for storing current byte */

      atprep_dsm_item_ptr->used--;              /* Decrement used count.           */
      cce = *item_data_ptr++;        /* Assign to echo character.       */
      cc = cce & 0x7F ;              /* Only the 7 upper bits are used. */
  
      /*------------------------------------------------------------------
        Process data received for SMS message.
      ------------------------------------------------------------------*/
      switch ( sms_msg_entry_state )
      {
      case NORMAL_STATE:
        /*----------------------------------------------------------------
          Normal processing of data for SMS message.
        ----------------------------------------------------------------*/
        if ( ( cc == '\b' )
#ifdef FEATURE_ETSI_SMS 
#error code not present
#endif /* FEATURE_ETSI_SMS  */
          )
        {
          /*--------------------------------------------------------------
            Line editing character (backspace) and SMS text message
            format mode - Perform line editing function.
          --------------------------------------------------------------*/

          if ( msg_ptr > start_line_ptr )
          {
            /* Not at the start of line yet so backup. */
            msg_ptr--;
            msg_item_ptr->used--;
          }
          else
          {
            /*  Kill echo - don't backspace over prompt */        
              store_char = FALSE;
          }
        }
        else if ( cc == CTRL_Z_CHAR )
        {
          /*--------------------------------------------------------------
            Ctrl-Z - Indicates end of message body
          --------------------------------------------------------------*/  
          
          /* Kill echo and send EOM string. */
            store_char = FALSE;
          send_string( eom );

          /* Send SMS message to SMS command handler. */
          dsatsms_send_sms_msg( msg_item_ptr->data_ptr, msg_item_ptr->used );

          /* Return DCD to its state before SMS message entry and set
             SIO data preprocessor back to AT command mode. */      
#ifdef FEATURE_DATA_MM
              (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_SMS_MSG_END );
#else
            (void) ds3g_sio_set_cd_state( DS3G_SIO_SMS_MSG_END );
#endif /* FEATURE_DATA_MM */
          dsatprep_set_preprocessor_mode( DSATI_AT_CMD_MODE );

          /* Free the DSM item holding SMS message. */
          (void) dsm_free_buffer( msg_item_ptr );
          msg_item_ptr = NULL;
        }
        else if ( cc == ESC_CHAR )
        {
          /*--------------------------------------------------------------
            ESC character - Cancel SMS message send or write command
          --------------------------------------------------------------*/
            
          /* Notify SMS handler command has been cancelled. */
          dsatsms_abort_cmd( DSAT_CMS_NONE );
          
          /* Kill echo and send EOM string. */
            store_char = FALSE;
          send_string( eom );

          /* Return DCD to its state before SMS message entry and set
             SIO data preprocessor back to AT command mode. */      
#ifdef FEATURE_DATA_MM
            (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_SMS_MSG_END );
#else
          (void) ds3g_sio_set_cd_state( DS3G_SIO_SMS_MSG_END );
#endif /* FEATURE_DATA_MM */
          dsatprep_set_preprocessor_mode( DSATI_AT_CMD_MODE );

          /* Free the DSM item holding SMS message. */
          (void) dsm_free_buffer( msg_item_ptr );
          msg_item_ptr = NULL;
        }
        else
        {
          /*--------------------------------------------------------------
            Any other IRA character - Write character to message buffer
          --------------------------------------------------------------*/  

          if ( (cc >= 0x01 && cc <= 0x7F) || 
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
               ( cc == '\n' || cc == '\r' ))
          {
#if defined(FEATURE_WCDMA) || defined(FEATURE_GSM)
#error code not present
#endif /* defined(FEATURE_WCDMA) || defined(FEATURE_GSM) */
            
            if ( msg_item_ptr->used < msg_item_ptr->size )
            {
              /* There is room in buffer so save character to buffer. */
              if ( cc == '\r')
              {
                *msg_ptr++ = '\n';
              }
              else
              {
                *msg_ptr++ = cc;
              }
              msg_item_ptr->used++;
            }
            else
            {
              /* Buffer overflow, SMS message is too large. */
              sms_msg_entry_state = ERROR_STATE;
              
              /* Notify SMS handler command error has occurred and command
                 should be aborted. */
              dsatsms_abort_cmd( DSAT_CMS_ME_FAILURE );
      
              /* Free the DSM item holding SMS message. */
              (void) dsm_free_buffer( msg_item_ptr );
              msg_item_ptr = NULL;
              
              /* Return DCD to its state before SMS message entry and set
                 SIO data preprocessor back to AT command mode. */      
#ifdef FEATURE_DATA_MM
                (void) ds3g_siolib_set_cd_state( DS3G_SIOLIB_SMS_MSG_END );
#else
                (void) ds3g_sio_set_cd_state( DS3G_SIO_SMS_MSG_END );
#endif /* FEATURE_DATA_MM */
              dsatprep_set_preprocessor_mode( DSATI_AT_CMD_MODE );
  
              ERR( "SMS message is too large!", 0, 0, 0 );
            }
          }

          if ( ( cc == '\r' )
#ifdef FEATURE_ETSI_SMS 
#error code not present
#endif /* FEATURE_ETSI_SMS  */ 
             )
          {
            /* Carriage return character and SMS text message format
               mode, so generate new line prompt to TE and kill echo of
               carriage return. */
              store_char = FALSE;
            send_string( prompt );
            
            /* Point to start of the new line. */
            start_line_ptr = msg_ptr;
          }
#ifdef FEATURE_ETSI_SMS 
#error code not present
#endif /* FEATURE_ETSI_SMS  */
        }

        break;


      case ERROR_STATE:
        /*----------------------------------------------------------------
          Error processing of data for SMS message. In this state just
          echo the remaining data left in the watermark to empty the
          watermark.
        ----------------------------------------------------------------*/

      if ( cc == '\b' )
      {
        /* Kill echo on backspace. */
          store_char = FALSE;
      }
      else if ( cc == ESC_CHAR || cc == CTRL_Z_CHAR )
      {
        /* Kill echo and send EOM string. */
          store_char = FALSE;
        send_string( eom );
      }          
      else if ( ( cc == '\r' )
#ifdef FEATURE_ETSI_SMS 
#error code not present
#endif /* FEATURE_ETSI_SMS  */
               )
          {
            /* Carriage return character and SMS text message format
               mode, so generate new line prompt to TE and kill echo of
               carriage return. */
            store_char = FALSE;
            send_string( prompt );
          }
#ifdef FEATURE_ETSI_SMS 
#error code not present
#endif /* FEATURE_ETSI_SMS  */        
        break;
        
      }   /* end switch ( ) */
      
        if(store_char)
        {
          store_echo( cce, &echo_item_ptr );
        }
    
    }   /* end while ( atprep_dsm_item_ptr ) */

    dump_echo( &echo_item_ptr );
    /* Get the next packet in the DSM packet chain*/
    atprep_dsm_item_ptr = dsm_free_buffer( atprep_dsm_item_ptr );

  } /*  end of input from Rm  */
      
  ASSERT( echo_item_ptr == NULL );
 
  return FALSE;
  
} /* process_sms_msg_entry_mode( ) */


/*===========================================================================

FUNCTION SEND_STRING

DESCRIPTION
  Sends string passed in to the TE.

DEPENDENCIES
  String length must be short enough to fit into DSM small item.

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void send_string
(
  const char *string_ptr      /* Pointer to NULL terminated string to send */
)
{
  dsm_item_type *item_ptr;
  
  /* Get a DSM item to place prompt in and send prompt. */
  item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );

  if ( item_ptr != NULL )
  {
    /* Copy string to buffer and send response. */
    
    while ( *string_ptr != NULL && item_ptr->used < item_ptr->size )
    {
      item_ptr->data_ptr[item_ptr->used++] = *string_ptr++;
    }
    
    dsatrsp_send_response( item_ptr, FALSE );
  }
  else
  {
    ERR( "Running out of buffers!",0,0,0 );
  }
  
} /* send_string( ) */
#endif /* defined(FEATURE_ETSI_SMS) || defined(FEATURE_CDMA_SMS) */


/*===========================================================================

FUNCTION STORE_ECHO

DESCRIPTION
  This function acquires a small item buffer (if needed), and stores
  the echo character in it.  If the buffer becomes full, the buffer
  is output via dsatrsp_send_response( ), and another buffer acquired.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
static void store_echo
(
  byte c,                         /*  character to echo         */
  dsm_item_type ** echo_item_ptr  /*  output: pointer to buffer */
)
{
  if ( dsat_e_val == 1 )
  {
    /*-----------------------------------------------------------------------
      Check if a free buffer is avalable. If none, require a   
      new buffer space.                                        
    -----------------------------------------------------------------------*/
    if ( *echo_item_ptr == NULL )
    {
      *echo_item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
       if ( *echo_item_ptr == NULL )
       {
         ERR( "Running out of buffers",0,0,0 );
         return;
       }
    }

    /*-----------------------------------------------------------------------
      Stores the echo character into the buffer.               
    -----------------------------------------------------------------------*/
    (*echo_item_ptr)->data_ptr[(*echo_item_ptr)->used++] = c;
      
    if ( (*echo_item_ptr)->used >= (*echo_item_ptr)->size )
    {
      dsatrsp_send_response( *echo_item_ptr, FALSE );
      *echo_item_ptr = dsm_new_buffer( DSM_DS_SMALL_ITEM_POOL );
      if ( *echo_item_ptr == NULL )
      {
        ERR( "Running out of buffers",0,0,0 );
        return;
      }
    }
  }
} /* store_echo( ) */


/*===========================================================================

FUNCTION DUMP_ECHO

DESCRIPTION
  This function outputs the echo characters stored by store_echo( ) 
  to the serial port via  dsatrsp_send_response(), and sets the item 
  pointer argument to NULL.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None

===========================================================================*/
static void dump_echo
(
  dsm_item_type ** echo_item_ptr  /*  buffer containing echo chars  */
)
{
  if ( *echo_item_ptr != NULL )
  {
    if ( (*echo_item_ptr)->used != 0 )
    {
      dsatrsp_send_response( *echo_item_ptr, FALSE );
    }
    else
    {
      (void) dsm_free_buffer( *echo_item_ptr );
    }
    *echo_item_ptr = NULL;
  }
} /* dump_echo( ) */

#endif /* FEATURE_DATA */
