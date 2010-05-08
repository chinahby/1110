#ifndef FTM_QFUSE_H
#define FTM_QFUSE_H

/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                    F T M        Q F U S E 

GENERAL DESCRIPTION
  This is the header file for the embedded FTM Commands that handle reading
  and programming QFUSE chain

Copyright (c) 2006-2009 by QUALCOMM, Incorporated.  All Rights Reserved.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

$Header: //depot/asic/qsc1100/drivers/efuse/src/ftm_qfuse.h#1 $ $DateTime: 2009/04/01 01:20:02 $ $Author: dkammath $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
03/31/09   ddk     Added updates for ULC.
06-15-06   anb     Created
===========================================================================*/


#include "customer.h"
#include "target.h"

#ifdef FEATURE_FACTORY_TESTMODE
#include "ftmicap.h"
#include "ftm.h"
#include "ftmdiag.h"

#ifdef FTM_HAS_QFUSE
#include "comdef.h"

#define  FTM_MAX_QFUSE_CONFIG_REGISTER 5

/* FTM QFUSE command IDs */
typedef enum
{
  FTM_QFUSE_READ     = 0,
  FTM_QFUSE_WRITE    = 1,
  FTM_QFUSE_MAX      = 0x7FFF       /* To ensure it's 16 bits wide */
} ftm_qfuse_sub_cmd_type;

typedef enum
{
  FTM_QFUSE_CONFIG_CHAIN_SEL             = 0,
  FTM_QFUSE_HW_KEY_CHAIN_SEL             = 1,
  FTM_QFUSE_MEM_REDUN_CHIP_ID_CHAIN_SEL  = 2,
  FTM_QFUSE_MAX_CHAIN_SEL                = 0x7FFF       /* To ensure it's 16 bits wide */
} ftm_qfuse_chain_select_type;

typedef enum
{
  FTM_QFUSE_SUCCESS                 = 0,
  FTM_QFUSE_READ_FAILURE            = 1,
  FTM_QFUSE_WRITE_FAILURE           = 2,
  FTM_QFUSE_CHAIN_SEL_NOT_SUPPORTED = 3,
  FTM_QFUSE_STATUS                  = 0x7FFF       /* To ensure it's 16 bits wide */
} ftm_qfuse_status_type;

/* FTM QFUSE request/response type */
typedef PACKED struct
{
  ftm_diagpkt_subsys_header_type  header;
  ftm_cmd_header_type             ftm_cmd_header;
} ftm_qfuse_pkt_type;

typedef PACKED struct
{
  ftm_diagpkt_subsys_header_type  header;
  ftm_cmd_header_type             ftm_cmd_header;
  ftm_qfuse_chain_select_type     chain_select;
} ftm_qfuse_read_req_type;

typedef PACKED struct
{
  ftm_diagpkt_subsys_header_type  header;
  ftm_cmd_header_type             ftm_cmd_header;
  uint16                          read_status;
  uint32                          config_register[FTM_MAX_QFUSE_CONFIG_REGISTER];
} ftm_qfuse_read_rsp_type;

typedef PACKED struct
{
  ftm_diagpkt_subsys_header_type  header;
  ftm_cmd_header_type             ftm_cmd_header;
  ftm_qfuse_chain_select_type     chain_select;
  uint32                          config_register[FTM_MAX_QFUSE_CONFIG_REGISTER];
} ftm_qfuse_write_req_type;

typedef PACKED struct
{
  ftm_diagpkt_subsys_header_type  header;
  ftm_cmd_header_type             ftm_cmd_header;
  uint16                          write_status;
} ftm_qfuse_write_rsp_type;

/*===========================================================================

FUNCTION FTM_QFUSE_DISPATCH

DESCRIPTION
   This function handles requests to program and read QFUSE.

DEPENDENCIES
   None.

RETURN VALUE
   Response packet.

SIDE EFFECTS
   None.

===========================================================================*/
ftm_rsp_pkt_type ftm_qfuse_dispatch(ftm_qfuse_pkt_type *cmd_ptr);

#endif  /*FTM_HAS_QFUSE */
#endif  /*FEATURE_FACTORY_TESTMODE */
#endif  /* FTM_QFUSE_H */
