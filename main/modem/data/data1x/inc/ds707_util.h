#ifndef DS707_UTIL_H
#define DS707_UTIL_H
/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*
                       D A T A   S E R V I C E S

                                UTILITIES

GENERAL DESCRIPTION
  This file contains utility functions to operate  

EXTERNALIZED FUNCTIONS


Copyright (c) 2011 by QUALCOMM Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/ds707_pzid.c_v   1.2   24 Jan 2003 16:09:22   akhare  $
  $Header: //source/qcom/qct/modem/data/1x/707/main/lite/inc/ds707_util.h#1 $ $DateTime: 2011/03/30 08:36:03 $ $Author: msankar $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/11   ms      Created the Initial Revision

===========================================================================*/

/*===========================================================================
                     INCLUDE FILES FOR MODULE
===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "customer.h"
#include "fs_fcntl.h"


#define DS_EFS_READ_BUFFER_SZ 128

/*-----------------------------------------------------------------------------
  Structure used to parse the efs file
-----------------------------------------------------------------------------*/

typedef struct
{
    int  fd;                          /*  File descriptor                     */
    char seperator;                  /*  Seperator(;) to be parsed for       */
    char buffer[DS_EFS_READ_BUFFER_SZ];/*  Buffer to read the file into   */
    char *curr;                     /*  pointer to the current location      */
    char *end_pos;                  /*  ponter to the end of the buffer      */
    boolean eof;                    /*  used to indicate the end of file     */
    boolean skip_line;              /*  identifies  comments in the file     */
    boolean eol;                    /*  used to indicate end of line         */
    boolean bol;                    /*  used to indicate begining of the line*/
}ds_efs_token_type; 

/*-----------------------------------------------------------------------------
  Enum to specify the various return values 
  SUCCESS : Success
  EOL     : End of line is reached => record end
  EOF     : End of file is reached => file end => feof
  FAILURE : Failed 
-----------------------------------------------------------------------------*/
typedef enum 
{
  DS_EFS_TOKEN_PARSE_SUCCESS  = 0,
  DS_EFS_TOKEN_PARSE_EOL      = 1,
  DS_EFS_TOKEN_PARSE_EOF      = 2,
  DS_EFS_TOKEN_PARSE_FAILURE  = 3,

  DS_EFS_TOKEN_PARSE_MAX
}ds_efs_token_parse_status_enum_type;



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
int ds_efs_file_init( const char *file_path, ds_efs_token_type *sm );
/*===========================================================================
FUNCTION      DS_EFS_FILE_CLOSE

DESCRIPTION   The function closes file and releases the state machine 

DEPENDENCIES  The file should have opened already.

RETURN VALUE  NONE

SIDE EFFECTS  None.
===========================================================================*/
void ds_efs_file_close( ds_efs_token_type *sm );

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
);
#endif
