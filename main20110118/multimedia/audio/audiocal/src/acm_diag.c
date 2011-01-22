/** 
\file **************************************************************************
*
*                                       A C M    D I A G   F I L E
*
*DESCRIPTION
* This header file contains the implementation of ACM diag dispatcher 
*
*INITIALIZATION REQUIREMENTS:
*acm_diag_init() must be called to initialise ACM module.
*acm_diag_init() must be called ~AFTER~ initialization of SOUND TASK
*
*Copyright (c) 2008 by QUALCOMM, Incorporated.  All Rights Reserved.
*******************************************************************************
*/
/* <EJECT> */
/**
\file ***************************************************************************
*
*                      EDIT HISTORY FOR FILE
*
*  This section contains comments describing changes made to this file.
*  Notice that changes are listed in reverse chronological order.
*   
*  $Header: //source/qcom/qct/multimedia/audio/6k/common/audiocal/QSC1100/main/latest/src/acm_diag.c#3 $ $DateTime: 2008/10/22 01:44:09 $ $Author: markande $
*
*when         who     what, where, why<br>
*--------   ---     ----------------------------------------------------------<br>
*01/25/08   mas    created  file<br>
********************************************************************************
*/
#include "msg.h"
#include "acm_diag.h"
#include "acm.h"
#include <stdlib.h>
#if defined(FEATURE_AVS_AUDIO_TRANSPORT_PROTOCOL)
/**
*this is acm diag dispatch table that needs to be registered with diag 
dispathcer to route the diag packets sent from PC to acm diag dispatcher.
*/
static const diagpkt_user_table_entry_type  acm_diag_table[] =
{
  {
    ACM_CMD_ID_START,
    ACM_CMD_ID_END,
    avsACM_diag_cmd
  }
};

/**
* FUNCTION : acm_diag_init

* DESCRIPTION :
*Registers the subsystem diag dispathcer with diag dispathcer.<br>
* Initializes the acm session.
*
* DEPENDENCIES : acm_diag_table[] should be initialized.<br>
*And acm_diag_table should have proper sub system command.code range and 
pointer to the dispatcher function.
*
* RETURN VALUE : None

* SIDE EFFECTS : None
 */
void acm_diag_init()
{
/**Register command range and diag subsystem dispatcher*/
  DIAGPKT_DISPATCH_TABLE_REGISTER ( DIAG_SUBSYS_AUDIO_SETTINGS,
                                                                    acm_diag_table
                                                                 );
/** Set the pointer to the application callback function which receives the 
request buffer and provides a response*/
  acm_phone_context.receive_req_buffer_ptr = acm_receive_req_buffer;
}

/**
* FUNCTION : avsACM_diag_cmd
*
* DESCRIPTION :
*This is the entry point to ACM when seen from PC.
*and is the sub system diag dispathcer for Audio Calibration manager
*Receives a diag packet ment for ACM and passes to protocol layer and returns 
the response diag packet.
*
* DEPENDENCIES :
*
* RETURN VALUE : returns response diag packet.
*In case of error, returns diag packet with error code DIAG_BAD_CMD_F
*
* SIDE EFFECTS : None
 */
PACKED void * avsACM_diag_cmd(PACKED void *req_ptr, word pkt_len)
{  

  diag_pkt_resp_struct *rsp_diag_pkt_ptr  = NULL;
  diag_pkt_resp_struct *acm_rsp_ptr = NULL;
  diag_pkt_req_struct request; 
  word acm_rsp_pkt_len = 0;
/** Initialize subsystem command code*/
  uint16 command_code = ACM_CMD_ID_START;
  
  MSG_MED("ACM_DIAG: Received diag packet", 0,0,0);
  
  if (req_ptr)
  {
    /**Create an unpacked version of diag packet*/
    memcpy((void *)&request,(void *)req_ptr,pkt_len);
    /**Get command code*/
    command_code = (uint16) diagpkt_subsys_get_cmd_code ((void *) &request);
    /*  Verify is the command code valid */
    if ( command_code < ACM_CMD_ID_START && command_code > ACM_CMD_ID_END)
    {
      /**If not in the range return error*/
      MSG_MED("ACM_DIAG: Invalid command ID in diag packet", 0,0,0);
      rsp_diag_pkt_ptr = (diag_pkt_resp_struct*) diagpkt_err_rsp (DIAG_BAD_CMD_F, req_ptr, pkt_len);
    }
    else
    {
      MSG_MED("ACM_DIAG: Valid command ID in diag packet", 0,0,0);
      /**Send diag packet to protocol layer*/
      atp_receive_diag_pkt(&request,&acm_rsp_ptr,&acm_phone_context);
      /**If we have not received response frame from protocol layer return error*/
      if(acm_rsp_ptr == NULL)
      {
        rsp_diag_pkt_ptr = (diag_pkt_resp_struct*) diagpkt_err_rsp (DIAG_BAD_CMD_F, req_ptr, pkt_len);
      }
      else
      {
        acm_rsp_pkt_len = DIAG_RES_PKT_SIZE + sizeof(diag_pkt_header_struct);
        /**
        Create a diag packet using diag api . It automatically delets the 
        packet after we commit the packet.
        */
        rsp_diag_pkt_ptr = (diag_pkt_resp_struct*) diagpkt_subsys_alloc (
        DIAG_SUBSYS_AUDIO_SETTINGS,(uint16) command_code, acm_rsp_pkt_len);
        if(NULL==rsp_diag_pkt_ptr)
		{
			/*CLOCKWORKS FIX*/
			free(acm_rsp_ptr);
			return NULL;
		}
		memcpy(rsp_diag_pkt_ptr,acm_rsp_ptr,acm_rsp_pkt_len);
        /**Send the diag packet .This will be received on PC side as response 
        packet*/
        diagpkt_commit(rsp_diag_pkt_ptr);
        MSG_MED("ACM_DIAG: Sending response tp PC", 0,0,0);
        /**Free the response packet we received from protocol layer*/
        free(acm_rsp_ptr);
      }
    }  
  }
  else
  {
    rsp_diag_pkt_ptr = (diag_pkt_resp_struct*) diagpkt_err_rsp (DIAG_BAD_CMD_F, req_ptr, pkt_len);
  }
  return (rsp_diag_pkt_ptr);
}

#endif /*FEATURE_AVS_DYNAMIC_CALIBRATION*/
