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
  
  $Header: //source/qcom/qct/wconnect/bthost/ftm/rel/00.00.26/inc/ftm_bt.h#2 $ 
  $DateTime: 2009/01/13 11:11:53 $ 
  $Author: hmehra $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/14/08   sk      Added support for WinMob
08/28/07   bk      Added ftm_bt_init() function prototype.   
03/21/07   gs      Added dsm_pool.h #include for 7200 compilation
01/12/07   gs      Added support for multi-processor target.
10/12/04   jac     Mainlined ASYNC packet code.
08/06/03   RY      Initial revision.

===========================================================================*/

#ifndef FTM_BT_H
#define FTM_BT_H

#include "customer.h"

#ifdef FEATURE_FACTORY_TESTMODE
#if defined(FEATURE_BT) || defined(FEATURE_WM_BT_FTM) 

/*===========================================================================

                         INCLUDE FILES FOR MODULE

===========================================================================*/
#include "comdef.h"
#include "target.h"
#include "ftm.h"
#include "ftmdiag.h"

#include "dsm_pool.h"

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

#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#endif

/*===========================================================================

                          FUNCTION PROTOTYPES

===========================================================================*/

#ifdef FEATURE_MULTIPROCESSOR
#error code not present
#endif /* FEATURE_MULTIPROCESSOR */

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
