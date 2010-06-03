/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                            FTM Bluetooth Dispatch

GENERAL DESCRIPTION

  This is the FTM file which contains FTM Bluetooth specific commands

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $Header: //source/qcom/qct/wconnect/bthost/ftm/rel/00.00.26/src/ftm_bt.c#3 $
  $DateTime: 2009/01/13 18:46:32 $ 
  $Author: c_vikask $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/13/09   vk      Changed Type casting to log_hdr_type.
05/14/08   sk      Added support for WinMob
10/30/07   dw      Fixed Lint errors
03/21/07   gs      Removed ASSERT statements
01/12/07   gs      Updated logic in ftm_bt_handle_event to process all dsm
                   items on the wm.
01/12/07   gs      Added support for multi-processor target.
06/16/06   jac     Fixed LINT error.
11/13/05   dyc     Compilation warning fix in non-BT.
10/12/04   jac     Mainlined ASYNC packet code.
09/16/04   ra      fixed incorrect return type for ftm_log_malloc call 
                   in ftm_bt_handle_event added ftm_task header file
04/04/04   dp      Remove "quick" commands.
03/01/04   dp      Added "quick" commands for enabling connection to BT test
                   box.
09/16/03   wd      Changed log malloc to use defined for FTM log header size.
09/10/03   wd      Added glue logic for FTM.
08/06/03   RY      Initial revision.

===========================================================================*/

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "customer.h"

#ifdef FEATURE_FACTORY_TESTMODE
#if defined(FEATURE_BT) || defined(FEATURE_WM_BT_FTM)
#include "ftmicap.h"
#include "ftm_task.h"
#include "ftm.h"
#include "ftmdiag.h"


#include "comdef.h"
#include "target.h"
#include "err.h"
#include "ftm_log.h"
#include "ftm_bt.h" 

#ifdef FEATURE_APPS_IMAGE_WINMOB
#include "wm_bthci_ftm.h"
#include "task.h"
#define BT_FTM_CMD_RSP_LEN                  1100 /* approx. 3-DH5 */
#else
#include "bt.h"
#include "bthc.h"
#endif /* FEATURE_APPS_IMAGE_WINMOB */



/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/


/*===========================================================================
 
                   LOCAL GLOBAL VARIABLES and CONSTANTS

===========================================================================*/
#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#else
LOCAL dsm_watermark_type* ftm_to_bt_wm_ptr = 0;
LOCAL dsm_watermark_type* ftm_from_bt_wm_ptr = 0;
#endif /* FEATURE_MULTIPROCESSOR */

/*===========================================================================

                     FUNCTION PROTOTYPES & DEFINITIONS

===========================================================================*/
#ifndef FEATURE_MULTIPROCESSOR
void ftm_bt_send_to_hci( void *data, word length);

/*===========================================================================

FUNCTION FTM_BT_CB

DESCRIPTION
  Call back function for BT task to call when it has data ready for FTM.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
#ifdef FEATURE_DSM_WM_CB
void ftm_bt_cb (struct dsm_watermark_type_s *dsm_wm_ptr, void *data)
#else 
void ftm_bt_cb (void)
#endif /* FEATURE_DSM_WM_CB */
{
  ftm_sig_rex_set(FTM_BT_SIG);
} /* End ftm_bt_cb */
#endif /* !FEATURE_MULTIPROCESSOR */

#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#endif /* FEATURE_MULTIPROCESSOR */

/*===========================================================================

FUNCTION FTM_BT_HANDLE_EVENT

DESCRIPTION
  This function handles events from BT and creates the apropiate logs.

DEPENDENCIES
   An Event must have been triggered from BT before this fucntion is called.

RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_bt_handle_event (void)
{

  #ifdef FTM_HAS_LOGGING
  log_ftm_gen_type *ftm_log_pkt = NULL;
  #endif
  
  #ifdef FEATURE_MULTIPROCESSOR
#error code not present
  #else
  dsm_item_type* dsm_ptr;

  /* get dsm ptr from bt */
  while((dsm_ptr = dsm_dequeue(ftm_from_bt_wm_ptr)) != NULL)
  {
    word pkt_size = 0;

    /* Get size information for BT pkt */
    pkt_size = dsm_length_packet(dsm_ptr);

#ifdef FTM_HAS_LOGGING
    /* now that we know the size, allocate the log memory we need */
    ftm_log_pkt = (log_ftm_gen_type *)ftm_log_malloc (FTM_LOG_BT, FTM_LOG_HEADER_SIZE + pkt_size);

    if (ftm_log_pkt != NULL)
    {
      FTM_MSG_HIGH("Got BT event sending FTM BT log, BT payload is %d bytes.",pkt_size,0,0);

      /* get BT pkt from dsm */
      (void)dsm_pullup(&dsm_ptr, (void*)ftm_log_pkt->data, pkt_size);

      /* send log */
      log_commit( (log_hdr_type*) ftm_log_pkt );
    }
    #endif /* FTM_HAS_LOGGING */
  }
  #endif /* FEATURE_MULTIPROCESSOR */
} /* End ftm_bt_handle_event */


#ifndef FEATURE_MULTIPROCESSOR
/*===========================================================================

FUNCTION FTM_BT_SEND_TO_HCI

DESCRIPTION
  This function passes a Bluetooth HCI command down to the bluetooth layer

DEPENDENCIES


RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_bt_send_to_hci( void *data, word length)
{
     
    dsm_item_type* dsm_ptr;
    
    dsm_ptr = bt_get_free_dsm_ptr( BT_TL_HCI_BB, length );
        
    (void)dsm_pushdown_tail(  &dsm_ptr,
                        data,
                        length,
                        DSM_DS_POOL_SIZE( length ) 
                      );
        
    dsm_enqueue( ftm_to_bt_wm_ptr, &dsm_ptr );
}
#endif /* !FEATURE_MULTIPROCESSOR */



/*===========================================================================

FUNCTION FTM_BT_PREPARE_BT_PACKET

DESCRIPTION
  

DEPENDENCIES


RETURN VALUE
   None.

SIDE EFFECTS
   None.

===========================================================================*/
void ftm_bt_prepare_bt_packet( ftm_bt_ext_pkt_type *packet, 
                               byte pkt_type, 
                               word opcode, 
                               byte length
                             )
{

    packet->pkt_type           = pkt_type;
    packet->opcode             = opcode;
    packet->param_total_length = length;
}




/*===========================================================================

FUNCTION FTM_BT_DISPATCH

DESCRIPTION
   This function handles requests to run tests and other primitives
   by dispatching the appropriate functions.

DEPENDENCIES
   None.

RETURN VALUE
   None.

SIDE EFFECTS
   None.
 
===========================================================================*/
ftm_rsp_pkt_type ftm_bt_dispatch( ftm_bt_pkt_type *ftm_bt_pkt )
{
  word           cmd_len;  
  
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

  ftm_bt_ext_pkt_type   *ftm_bt_hci_req = NULL;


  FTM_MSG_MED("FTM BT dispatching packet id: %d",ftm_bt_pkt->ftm_hdr.cmd_id,0,0); /*lint !e778*/

  #ifdef FEATURE_MULTIPROCESSOR
#error code not present
  #else
  if ( ( ftm_to_bt_wm_ptr == NULL   ) || 
       ( ftm_from_bt_wm_ptr == NULL ) )
  {
    (void)bt_cmd_dc_set_hci_mode( BT_HCIM_STANDARD_HCI_FTM );
    bt_cmd_dc_ftm_get_bt_wm( &ftm_to_bt_wm_ptr, &ftm_from_bt_wm_ptr );

    if ( ( ftm_to_bt_wm_ptr == NULL ) || 
         ( ftm_from_bt_wm_ptr == NULL ) )
    {
      FTM_MSG_ERROR( "FTM BT: Unable to get BT watermarks", 0, 0, 0 );
      return rsp_pkt;
    }

    /* FIXME: Register own callback here */
    ftm_from_bt_wm_ptr->non_empty_func_ptr = &ftm_bt_cb; /*lint !e546*/
  }
  #endif /* FEATURE_MULTIPROCESSOR */

  switch (ftm_bt_pkt->ftm_hdr.cmd_id)
  {
  
  case FTM_BT_HCI_USER_CMD:
    
    /* Pass payload to BT task */;

    cmd_len = ftm_bt_pkt->ftm_hdr.cmd_data_len; 
                                                 
    /* sanity check on cmd_len */
    
    if( cmd_len > 0)
    {
      #ifdef FEATURE_MULTIPROCESSOR
#error code not present
      #else
      ftm_bt_send_to_hci( (void *) ftm_bt_pkt->data, cmd_len);
      #endif /* FEATURE_MULTIPROCESSOR */
    }
    break;
  
  default:
    FTM_MSG_ERROR("Unknown FTM BT Command",0,0,0);
    rsp_pkt.cmd = FTM_RSP_BAD_CMD;
   
    break;
  }


  if( ftm_bt_hci_req != NULL)
  {
      ftm_free( (void *) ftm_bt_hci_req);
  }

  return rsp_pkt;
} /* End ftm_bt_dispatch */

#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#endif /* FEATURE_MULTIPROCESSOR */

#else /* !FEATURE_BT */
#include "comdef.h"     
#endif /* FEATURE_BT */
#endif /* FEATURE_FACTORY_TESTMODE */
