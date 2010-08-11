/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    F T M        Q F U S E 

GENERAL DESCRIPTION
  This is the file contains code for embedded FTM Commands that handle reading
  and programming QFUSE chain

Copyright (c) 2006-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/efuse/src/ftm_qfuse.c#1 $ $DateTime: 2009/04/01 01:20:02 $ $Author: dkammath $

when          who     what, where, why
--------   ---     ----------------------------------------------------------
03/30/09   dk      Adding updates for ULC.
06-15-06   anb     Created
===========================================================================*/

#include "target.h"    /* Target specific definitions            */
#include "comdef.h"    /* Common stuff                           */

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftm_task.h"

#ifdef FTM_HAS_QFUSE
#include "ftm_qfuse.h"
#include "efuse.h"
#include "efuse_target.h"

/*===========================================================================

FUNCTION       FTM_QFUSE_READ_CONFIG_CHAIN

DESCRIPTION
  This function read QFUSE chain.

DEPENDENCIES
  none

===========================================================================*/
ftm_rsp_pkt_type ftm_qfuse_read_config_chain(const ftm_qfuse_pkt_type *cmd_ptr)
{
  ftm_rsp_pkt_type           rsp_pkt       = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  uint16                     read_ret_code = (uint16) FTM_QFUSE_SUCCESS;
  ftm_qfuse_read_rsp_type    rsp_data;
  efuse_err_enum_type        ret_code;
  uint32                     i;

  rsp_data.header         = cmd_ptr->header;
  rsp_data.ftm_cmd_header = cmd_ptr->ftm_cmd_header;
 
  /* Perform target specific hardware intialization necessary for the efuse read*/
  efuse_target_read_init();

  for (i=0; i<FTM_MAX_QFUSE_CONFIG_REGISTER; i++)
  {
    rsp_data.config_register[i] = 0;
  }
  
  /*lint -save -e545 -e740 */
  ret_code = efuse_read_configuration_chain((uint32 *)&rsp_data.config_register);
  /*lint -restore */

  if (ret_code!=EFUSE_NO_ERR)
    read_ret_code = (uint16) FTM_QFUSE_READ_FAILURE;

  rsp_data.read_status    = read_ret_code;
  rsp_pkt = ftmdiag_create_new_pkt_with_data( (byte*)&rsp_data, (sizeof(ftm_qfuse_read_rsp_type)), NULL, NULL );

  /* Deinitialize target specific hardware intialization done for the efuse read */
  efuse_target_read_deinit();
  
  return rsp_pkt;

} /* End ftm_qfuse_read_config_chain */

/*===========================================================================

FUNCTION       FTM_QFUSE_WRITE_CONFIG_CHAIN

DESCRIPTION
  This function write QFUSE chain.

DEPENDENCIES
  none

===========================================================================*/
ftm_rsp_pkt_type ftm_qfuse_write_config_chain(ftm_qfuse_pkt_type *cmd_ptr)
{
  ftm_rsp_pkt_type         rsp_pkt       = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  uint16                   write_ret_code = (uint16) FTM_QFUSE_SUCCESS;
  ftm_qfuse_write_rsp_type rsp_data;
  efuse_err_enum_type      ret_code;
  ftm_qfuse_write_req_type *req_pkt = (ftm_qfuse_write_req_type *)(void *)cmd_ptr;

  rsp_data.header         = cmd_ptr->header;
  rsp_data.ftm_cmd_header = cmd_ptr->ftm_cmd_header;

  /* Perform target specific hardware intialization necessary for the efuse write */
  efuse_target_write_init();

  /* Before we start writing to the QFUSE register we need to enable PMIC */
  ret_code = efuse_write_item_init();  
  if (ret_code==EFUSE_NO_ERR)
  {
    /*lint -save -e545 -e740 */
    ret_code = efuse_write_configuration_chain((uint32 *)&req_pkt->config_register);
    /*lint -restore */
    (void)efuse_write_item_cleanup();
  }

  FTM_MSG_MED("QFUSE WRITE operation return code:%d", ret_code, 0, 0);

  if (ret_code!=EFUSE_NO_ERR)
    write_ret_code = (uint16) FTM_QFUSE_WRITE_FAILURE;

  rsp_data.write_status    = write_ret_code;
  rsp_pkt = ftmdiag_create_new_pkt_with_data( (byte*)&rsp_data, 
                                              (sizeof(ftm_qfuse_write_rsp_type)), 
                                              NULL,
                                              NULL
                                            );

  /* Deinitialize target specific hardware intialization done for the efuse write */
  efuse_target_write_deinit();
  
  return rsp_pkt;
}

/*===========================================================================

FUNCTION       FTM_QFUSE_READ

DESCRIPTION
  This function read QFUSE chain.

DEPENDENCIES
  none

===========================================================================*/
ftm_rsp_pkt_type ftm_qfuse_read(ftm_qfuse_pkt_type *cmd_ptr)
{
  ftm_rsp_pkt_type         rsp_pkt  = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  uint16                   ret_code = (uint16) FTM_QFUSE_SUCCESS;
  ftm_qfuse_read_rsp_type  rsp_data;
  ftm_qfuse_read_req_type  *req_pkt = (ftm_qfuse_read_req_type *)(void *)cmd_ptr;

  rsp_data.header         = cmd_ptr->header;
  rsp_data.ftm_cmd_header = cmd_ptr->ftm_cmd_header;

  switch (req_pkt->chain_select)
  {
    case FTM_QFUSE_CONFIG_CHAIN_SEL:
      return ftm_qfuse_read_config_chain(cmd_ptr);
      
    case FTM_QFUSE_HW_KEY_CHAIN_SEL:
    case FTM_QFUSE_MEM_REDUN_CHIP_ID_CHAIN_SEL:
      FTM_MSG_ERROR( "FTM QFUSE:Chain Select Not Supported", 0, 0, 0 );
      ret_code = (uint16) FTM_QFUSE_CHAIN_SEL_NOT_SUPPORTED;
      break;
      
    default:
      FTM_MSG_ERROR( "FTM QFUSE:unknown chain select option", 0, 0, 0 );
      rsp_pkt.cmd = FTM_RSP_BAD_CMD;
      break;
  }
  rsp_data.read_status    = ret_code;
  rsp_pkt = ftmdiag_create_new_pkt_with_data( (byte*)&rsp_data,
                                              (sizeof(ftm_qfuse_read_rsp_type)),
                                              NULL,
                                              NULL
                                            );
  return rsp_pkt;

} /* End ftm_qfuse_read */


/*===========================================================================

FUNCTION       FTM_QFUSE_WRITE

DESCRIPTION
  This function read QFUSE chain.

DEPENDENCIES
  none

===========================================================================*/
ftm_rsp_pkt_type ftm_qfuse_write(ftm_qfuse_pkt_type *cmd_ptr)
{
  ftm_rsp_pkt_type          rsp_pkt  = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};
  uint16                    ret_code = (uint16) FTM_QFUSE_SUCCESS;
  ftm_qfuse_write_rsp_type  rsp_data;
  ftm_qfuse_write_req_type  *req_pkt = (ftm_qfuse_write_req_type *)(void *)cmd_ptr;
  
  rsp_data.header         = cmd_ptr->header;
  rsp_data.ftm_cmd_header = cmd_ptr->ftm_cmd_header;

  switch (req_pkt->chain_select)
  {
    case FTM_QFUSE_CONFIG_CHAIN_SEL:      
      return ftm_qfuse_write_config_chain(cmd_ptr);
      
    case FTM_QFUSE_HW_KEY_CHAIN_SEL:
    case FTM_QFUSE_MEM_REDUN_CHIP_ID_CHAIN_SEL:
      FTM_MSG_ERROR( "FTM QFUSE:Chain Select Not Supported", 0, 0, 0 );
      ret_code = (uint16) FTM_QFUSE_CHAIN_SEL_NOT_SUPPORTED;
      break;
      
    default:
      FTM_MSG_ERROR( "FTM QFUSE:unknown chain select option", 0, 0, 0 );
      rsp_pkt.cmd = FTM_RSP_BAD_CMD;
      break;
  }
  
  rsp_data.write_status   = ret_code;
  rsp_pkt = ftmdiag_create_new_pkt_with_data( (byte*)&rsp_data, 
                                              (sizeof(ftm_qfuse_write_rsp_type)),
                                              NULL,
                                              NULL
                                            );
  
  return rsp_pkt;

} /* End ftm_qfuse_write */

/*===========================================================================

FUNCTION FTM_QFUSE_DISPATCH

DESCRIPTION
   This function handles requests to program and read Configuration QFUSE.

DEPENDENCIES
   None.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_qfuse_dispatch(ftm_qfuse_pkt_type *cmd_ptr)
{
  ftm_rsp_pkt_type rsp_pkt = {FTM_RSP_DO_LEGACY, 0, NULL, FALSE};

  FTM_MSG_MED( "FTM QFUSE dispatching packet id: %d", cmd_ptr->ftm_cmd_header.cmd_id, 0, 0 );

  switch( cmd_ptr->ftm_cmd_header.cmd_id )
  {
    case FTM_QFUSE_READ:
      rsp_pkt = ftm_qfuse_read(cmd_ptr);
      break;

    case FTM_QFUSE_WRITE:
      rsp_pkt = ftm_qfuse_write(cmd_ptr);
      break;
    
    default:
      FTM_MSG_ERROR( "FTM QFUSE:unknown command", 0, 0, 0 );
      rsp_pkt.cmd = FTM_RSP_BAD_CMD;
      break;
  }

  return rsp_pkt;
}

#endif /* FTM_HAS_QFUSE */
#endif /* FEATURE_FACTORY_TESTMODE */
