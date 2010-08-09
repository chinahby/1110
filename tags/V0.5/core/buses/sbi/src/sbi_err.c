/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                   Serial Bus Interface Module Error Handler

GENERAL DESCRIPTION
  This module provides error handling for the Serial Bus Interface.
  This handling isn't part of the library so that it can be customized by 
  customers as needed.

EXTERNALIZED FUNCTIONS
  sbi_handle_runtime_error
      handles runtime errors reported out of the library

INITIALIZATION AND SEQUENCING REQUIREMENTS
   sbi_init needs to be called before any other operation can pe performed
   on the 3-line serial bus.

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //depot/asic/msmshared/drivers/sbi/SSBI/sbi_err.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/01/04   ih      Fixed EOL in message buffers
01/29/04   ih      Added message buffering to get around diag limitation so 
                   error and warning messages can be displayed correctly
                   instead of just a line number.
04/17/03   jcw     remove ERR call for release builds
3/18/03    jcw     initial release

===========================================================================*/

/*********************************************************************
* Include Files
**********************************************************************/
#include "err.h"
#include "msg.h"

#define MSG_BUFFER_SIZE 50

/* Buffering needed to get around non-constant initializer compiler error.
 * Since diag queues the pointer to the string, consecutive messages will
 * not show up correctly as the buffer gets overwritten. Here we use double 
 * buffering alleviate the symptoms in the case where two consectutive messages
 * are printed.
 */ 
static int  current_buffer = 0;
static char err_msg_buffer0[MSG_BUFFER_SIZE];
static char err_msg_buffer1[MSG_BUFFER_SIZE];
static INLINE void msgcpy(const char *s) {
    uint16 i=0;            
    if(!current_buffer) {
        while(s[i] != 0 && i < sizeof(err_msg_buffer0) - 1) {
            err_msg_buffer0[i]=s[i];    
            i++;
        }
        err_msg_buffer0[i] = '\0';
    }
    else {
        while(s[i] != 0 && i < sizeof(err_msg_buffer1) - 1) {
            err_msg_buffer1[i]=s[i];    
            i++;
        }
        err_msg_buffer1[i] = '\0';
    }
}

/*===========================================================================

FUNCTION sbi_handle_runtime_error

DESCRIPTION
  This function handles runtime errors within the library.  It is external
  to the library so it can be customized as desired.  If this is called, then
  a critical error has occurred in the library.  It will attempt to recover
  gracefully, but this cannot be guaranteed.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void sbi_handle_runtime_error
(
    const char * err_string,
    uint32 parm0,
    uint32 parm1,
    uint32 parm2
)
{
    msgcpy(err_string);
    if(!current_buffer) 
        MSG_FATAL(err_msg_buffer0,parm0,parm1,0);
    else
        MSG_FATAL(err_msg_buffer1,parm0,parm1,0);

    current_buffer = (current_buffer + 1) & 1;
}

/*===========================================================================

FUNCTION sbi_handle_runtime_warning

DESCRIPTION
  This function handles runtime errors within the library.  It is external
  to the library so it can be customized as desired. If this is called, then
  something unexpected has occurred in the library.  The driver will recover
  although some transactions may be lost.

DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
===========================================================================*/
void sbi_handle_runtime_warning
(
    const char * err_string,
    uint32 parm0,
    uint32 parm1,
    uint32 parm2
)
{
    msgcpy(err_string);
    if(!current_buffer) 
        MSG_HIGH(err_msg_buffer0,parm0,parm1,0);
    else
        MSG_HIGH(err_msg_buffer1,parm0,parm1,0);

    current_buffer = (current_buffer + 1) & 1;
}
