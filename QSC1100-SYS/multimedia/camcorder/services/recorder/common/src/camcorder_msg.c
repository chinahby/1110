/*===========================================================================

                 CAMCORDER MESSAGE 

DESCRIPTION
  This file contains function implementation for CAMCORDER MESSAGES

INITIALIZATION AND SEQUENCING REQUIREMENTS

Copyright (c) 2007 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/* <EJECT> */
/*===========================================================================

                        EDIT HISTORY FOR MODULE

This section contains comments describing changes made to the module.
Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/multimedia/camcorder/services/recorder/6k/common/main/latest/src/camcorder_msg.c#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/27/07   dhaval  Handle the case if memory allocation fail
03/22/07   dhaval  removed banned API usage.
09/04/06   dhaval  Initial version

===========================================================================*/


#include "camcorder_msg.h"
#include "AEEstd.h"

#ifdef FEATURE_VIDEO_ENCODE 
/*===========================================================================

FUNCTION camcorder_msg_clipinfo

DESCRIPTION
  This function prints the clip information.

DEPENDENCIES
  None.

RETURN VALUE
  None.

SIDE EFFECTS
  None.

===========================================================================*/
void camcorder_msg_clipinfo(char * msg_format,uint32 a,uint32 b,uint32 c)
{
#ifdef FEATURE_VIDEO_CLIP_INFO
    char * msg_buf = NULL;                                                      
    uint16 msg_size = 0;
    int16  msgsize_from_printf = 0;
    if (log_msg)
    {  
      const char * format = msg_format;
      /* assuming each argument requires maximum 20 characters */
      msg_size = (uint16)strlen(msg_format) + 3 * 20;
      msg_buf = (char *)malloc(msg_size);

      /* do if allocation success */
      if(msg_buf != NULL)
      {
        msgsize_from_printf = std_strlprintf(msg_buf,msg_size,format,a,b,c);

        /* check that message is copied in buffer or not */
        if(msgsize_from_printf > 0)
        {
          /* if first node in link list */
          if(msg_curr == NULL)
          {
              msg_head = msg_curr = malloc(sizeof(video_fmt_msg_list_type));
          }
          /* Create next node in link list */
          else
          {
              msg_curr->next = malloc(sizeof(video_fmt_msg_list_type));
              msg_curr = msg_curr->next;
          }
          /* if memory allocation of next atom success */
          if(msg_curr != NULL)
          {
            msg_curr->msg_size = msg_size;
            msg_curr->msg = msg_buf;                                
            msg_curr->next = NULL;                                         
          }
          /* memory allocation failed */
          else
          {
            CAMCORDER_MSG_HIGH("allocation failed",0,0,0);
            free(msg_buf);
          }
        }
        /* if std_strlprintf failed then free the message buffer */
        else
        {
            CAMCORDER_MSG_HIGH("std_strlprintf failed",0,0,0);
            free(msg_buf);
        }
      }
    }
#endif
}
#endif /* FEATURE_VIDEO_ENCODE */




