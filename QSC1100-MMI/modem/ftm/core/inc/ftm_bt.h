/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                              FTM Bluetooth Dispatch

GENERAL DESCRIPTION
  This is the header file for the embedded FTM Bluetooth Dispatch
  commands coming from the DIAG service.

Copyright (c) 2003 by QUALCOMM, Incorporated.  All Rights Reserved.

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $Header: //source/qcom/qct/modem/ftm/core/main/qsc1100/inc/ftm_bt.h#1 $ 
  $DateTime: 2008/04/26 18:30:56 $ 
  $Author: gswiech $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
10/12/04   jac     Mainlined ASYNC packet code.
08/06/03   RY      Initial revision.

===========================================================================*/

#ifndef FTM_BT_H
#define FTM_BT_H


#ifdef FEATURE_FACTORY_TESTMODE
#ifdef FEATURE_BT

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "ftm.h"
#include "ftmdiag.h"

/*===========================================================================
 
                         EXTERNAL GLOBAL VARIABLES

===========================================================================*/

/*===========================================================================

                              DEFINITIONS

===========================================================================*/
typedef enum 
{
  
    FTM_BT_HCI_USER_CMD          = 0,
    FTM_BT_MAX

} ftm_bt_subcmd_type;

/* Bluetooth FTM packet */
typedef PACKED struct
{
  diagpkt_subsys_header_type diag_hdr;
  ftm_cmd_header_type        ftm_hdr;
  byte                       data[1];
} ftm_bt_pkt_type;

/* Bluetooth pkt hdr */
typedef PACKED struct
{
  byte pkt_type;
  word opcode;
  byte param_total_length;
  byte data [1];
} ftm_bt_ext_pkt_type;

/*===========================================================================

                          FUNCTION PROTOTYPES

===========================================================================*/

/*===========================================================================

FUNCTION FTM_BT_DISPATCH

DESCRIPTION
   This function handles requests to run tests and other primitives
   by dispatching the appropriate functions.

===========================================================================*/
ftm_rsp_pkt_type ftm_bt_dispatch( ftm_bt_pkt_type *ftm_bt_pkt );

/*===========================================================================

FUNCTION FTM_BT_HANDLE_EVENT

DESCRIPTION
  This function handles events from BT and creates the apropiate logs.

===========================================================================*/
void ftm_bt_handle_event (void);


#endif /* FEATURE_BT */
#endif /* FEATURE_FACTORY_TESTMODE */
#endif /* FTM_BT_H */
