/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
               D S 7 0 7  U T I L . C
           
GENERAL DESCRIPTION
  Utility functions for DS module

EXTERNALIZED FUNCTIONS
  DS_EFS_FILE_INIT()
    Intializes the state machine and also opens the file
  DS_EFS_FILE_CLOSE()
    Closes file and releases the state machine 
  DS_EFS_TOKENIZER()
    Reads from the EFS file and returns a token
  DS_CREATE_EFS_CONFIG_FILE()
    Create the EFS config file which tells tools (i.e. QPST,QXDM) about the EFS
    items. The config file will be empty until ds_create_efs_item_file() is
    called.
  DS_CREATE_EFS_ITEM_FILE()
    Places the path of an item file into the specified config file. Again this
    is used by tools to locate the EFS item. If the config file was not set up
    properly this function will exit with the appropriate efs_errno.
  DS_READ_EFS_NV()
    Reads the EFS item based on the item file path passed in.
  DS_WRITE_EFS_NV()
    Writes to the EFS item based on the item file path passed in.

INITIALIZATION AND SEQUENCING REQUIREMENTS
  USING EFS ITEMS
    1. DS_CREATE_EFS_CONFIG_FILE() should be called first in a powerup
       function. Only need to call once per task. Each task shall have a
       different config file.
       Data is using: "/nv/item_files/conf/data_config_info.conf"
    2. DS_CREATE_EFS_ITEM_FILE() should be called next in an init function.
    3. When the EFS item wants to be read, DS_READ_EFS_NV() is called.
       The item file path is used to locate the item file in the EFS.
       A data structure/data type (passed by reference) will be filled with
       the value of the EFS item.
    4. If the read call returns with a ENOENT (file does not exist), then
       user will need to call DS_WRITE_EFS_NV() with default values passed in.

  Copyright (c) 2004 - 2011 by QUALCOMM, Inc.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

$Header: //source/qcom/qct/modem/data/1x/707/main/lite/src/ds707_util.c#1 $ $DateTime: 2011/03/30 08:36:03 $ $Author: msankar $

===========================================================================
when       who        what, where, why
--------   ---      -------------------------------------------------------
03/30/11   ms        Created new file. 

===========================================================================*/
/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"
#include "msg.h"

#include "ds707_util.h"
/* lint fix */

/*= = = = = = = =  = = = = = = = = = =  = = =  = = = = = = = = = = = = = = =
                        MACRO DEFINITIONS
= = = = = = = = = = = = = = =  = = = = =  = = = = = = = = = = = = = = = = =*/

/*===========================================================================
            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE
===========================================================================*/

/*===========================================================================
FUNCTION      DS_EFS_FILE_INIT

DESCRIPTION   The function intializes the state machine and 
              also opens the file

DEPENDENCIES  None.

RETURN VALUE 
              0  : SUCCESS: The file is good, readable, 
                            State Machine Initialized.
              -1 : FAILURE: The file cannot be opened/ readable. 

SIDE EFFECTS  None.
===========================================================================*/
int ds_efs_file_init( const char *file_path, ds_efs_token_type *sm )
{
  
  /*
  ** Initialize the structure variables and open the file in read mode.    
  */
  sm->seperator   = ';';
  sm->curr        = sm->buffer;
  sm->end_pos     = sm->buffer;
  
  sm->eof         = FALSE;
  sm->eol         = FALSE;
  sm->skip_line   = FALSE;
  sm->bol         = TRUE;

  sm->fd = efs_open( file_path, O_RDONLY);
  if(sm->fd != -1) 
  {
    
    return 0;
  }
  else 
  {
    MSG_HIGH("Cannot open file.", 0,0,0);
    return -1;
  }
}
/*===========================================================================
FUNCTION      DS_EFS_FILE_CLOSE

DESCRIPTION   The function closes file and releases the state machine 

DEPENDENCIES  The file should have opened already.

RETURN VALUE  NONE

SIDE EFFECTS  None.
===========================================================================*/
void ds_efs_file_close( ds_efs_token_type *sm )
{
  (void) efs_close( sm->fd );
  return;
}

/*===========================================================================
FUNCTION      DS_EFS_TOKENIZER

DESCRIPTION   The is the function that reads data from the opened file.
              The data read is looked for tokens 
              1. Each token is seperated by ';'
              2. Successive ';' means empty token
              3. Line begining with '#' is comment
              4. '\n' is the end of token and record
              5. Empty line is ignored
              6. Insufficient tokens is a record is considered bad record
              
DEPENDENCIES  File should have already been opened.

RETURN VALUE   
              SUCCESS : Success => Found Token.
                        *begin points to the begining of token.
                        *end points to the end of token.
              EOL     : End of line is reached => record end 
                        => no token extracted
              END     : End of file is reached => file end => feof
                        => no token extracted
              FAILURE : Failed 

SIDE EFFECTS  None.
===========================================================================*/
ds_efs_token_parse_status_enum_type ds_efs_tokenizer
(
  ds_efs_token_type *sm,
  char **begin,
  char **end
)
{
  int bytes_read = 0;
  char *dummy;
  ds_efs_token_parse_status_enum_type retval;

  *begin = 0;
  *end   = 0;
  /*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -*/
 
  
  /*------------------------------------------------------------x------------
     Traversed to end of file => return 
  ---------------------------------------------------------------------------*/
  if( sm->eof ) 
  {
    return DS_EFS_TOKEN_PARSE_EOF;
  }
   
  /*------------------------------------------------------------------------
     Have some bytes to read from the buffer
  ---------------------------------------------------------------------------*/
  while( sm->curr < sm->end_pos ) 
  {

    /*----------------------------------------------------------------------
      Skip over all carriage return characters (\r) added if file was
      editted using a windows machine
    -----------------------------------------------------------------------*/

    if (*sm->curr == '\r')         
    {
      sm->skip_line = FALSE;
      sm->curr++;
      continue;
    }

    /*-----------------------------------------------------------------------
       Lines begining the record with # are comments. Continue to read 
       until we reach the end of file.
    -----------------------------------------------------------------------*/
    if( sm->bol && *sm->curr ==  '#' ) 
    {
      sm->skip_line = TRUE;
      sm->bol = FALSE;
      sm->curr++;
      continue;
    } 

    if( sm->skip_line )                 /* reading a comment */
    {
      if( *sm->curr == '\n' )           /* End of comment */
      {  
        sm->skip_line = FALSE;
        sm->eol = TRUE;
        sm->bol = TRUE;
      }
      sm->curr++;
      continue;                         /*Continue to read until the end of line */
    }
    
    /*--------------------------------------------------------------------------
      Look for the token. If ';' found at the begining then it is 
      an empty token.
      There could be a  case where we hit '\n' while we are looking for a token
      so skip over all the new lines.
    ----------------------------------------------------------------------------*/
    if( *begin == 0 )                   /* Find the beginning of token */
    {                          
      if( *sm->curr == sm->seperator )  /* an empty token */
      {                             

        if( sm->bol == TRUE ) 
        {
          sm->bol = FALSE;
        }
        
        *begin = sm->curr;
        *end   = sm->curr;
        sm->curr++;
        return DS_EFS_TOKEN_PARSE_SUCCESS;
      }

      if( *sm->curr == '\n' )           /* Possibly an empty token */
      {    
        if( sm->eol )                   /* Skip over any successive new lines */
        {     
          sm->curr++;
          continue;
        }
        *begin  = sm->curr;
        *end    = sm->curr;
        sm->eol = TRUE;
        sm->bol = TRUE;
        sm->curr++;
        return DS_EFS_TOKEN_PARSE_SUCCESS;
      }

      /*-------------------------------------------------------------------------
       Before beginning a new token, return an end of record for previous record. 
      --------------------------------------------------------------------------*/
      if( sm->eol ) 
      {                             
        sm->eol = FALSE;
        return DS_EFS_TOKEN_PARSE_EOL;
      }

      *begin = sm->curr;                /* Initialize to beginning of token */
    }
    else if( *sm->curr == sm->seperator || *sm->curr == '\n' )
    {
      *end = sm->curr++;                /* Found end of token */
      
      /*--------------------------------------------------------------------------
         This is a end of line. Save the state and send 
         end of line event when a next token is requested .
      --------------------------------------------------------------------------*/
      if( **end == '\n' ) 
      {       
        sm->eol = TRUE;
        sm->bol = TRUE;
      }
      return DS_EFS_TOKEN_PARSE_SUCCESS;
    }
    
    sm->curr++;
  }/* while */

  /*-------------------------------------------------------------------------- 
    In the middle of token and we ran out characters in the buffer 
  --------------------------------------------------------------------------*/
  if( *begin ) 
  {      
    
    if( *begin != sm->buffer )
    {
      /*---------------------------------------------------------------------- 
        Move the partial token over to beginning of buffer 
      -----------------------------------------------------------------------*/
      /*lint -e732 */
      memcpy( sm->buffer, *begin, (sm->curr - *begin) );
      /*lint +e732 */
      sm->curr = sm->buffer + (sm->curr - *begin);
      *begin = sm->buffer;
    }
    else 
    {
      MSG_HIGH( "Token is larger than DS_EFS_READ_BUFFER_SZ",0,0,0 );
      return DS_EFS_TOKEN_PARSE_FAILURE;
    }
  }
  else 
  {
    /*--------------------------------------------------------------------
      No token or data exists in the buffer 
    ---------------------------------------------------------------------*/
    sm->curr = sm->buffer;
  }
  
  /*----------------------------------------------------------------------
      Read data from the efs file.
  -----------------------------------------------------------------------*/
  {
    /*lint -e732 */
    bytes_read = efs_read( sm->fd, sm->curr, 
                       DS_EFS_READ_BUFFER_SZ - (sm->curr - sm->buffer));
    /*lint +e732 */
    
    if( bytes_read > 0 ) 
    {
      sm->end_pos = sm->curr + bytes_read;
      sm->eof    = FALSE;
      
      if(*begin != 0)
      {
        retval= ds_efs_tokenizer( sm, &dummy, end ); /* Call the function 
                                              again because you could be in the
                                              middle of reading a token */
      }
      else
      {
        retval = ds_efs_tokenizer( sm, begin, end);
      }

      return retval;
    }
    else 
    {

      /*
        No bytes read => reached the end of file.
      */
      if(*begin == 0) 
      {
        sm->eof = 1;
        return DS_EFS_TOKEN_PARSE_EOL;
      }
      else
      {

        /*------------------------------------------------------------------
          If a token was found return the token and 
          when next token is requested send EOF 
        --------------------------------------------------------------------*/
        *end = sm->curr;
        if(bytes_read == 0)
        {
          /*---------------------------------------------------------------
           If the EOF character is missing in the file and the bytes read
           are zero all the time then we are trying to bail out of this.
           
           NOTE: We might have to revisit this later again if different 
           modules
           
          ----------------------------------------------------------------*/
          sm->eof = 1;
          return DS_EFS_TOKEN_PARSE_EOL;
        }
        return DS_EFS_TOKEN_PARSE_SUCCESS;
      }      
    }
  }/* End of bytes read*/
}/* ds_efs_tokenizer() */