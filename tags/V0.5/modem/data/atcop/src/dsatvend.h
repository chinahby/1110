#ifndef DSATVEND_H
#define DSATVEND_H
/*===========================================================================

                D A T A   S E R V I C E S

                A T  C O M M A N D   
                
                ( V E N D O R  S P E C I F I C )
                
                P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services vendor specific AT command processor.

Copyright (c) 1995,1996,1997 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatvend.h_v   1.0   08 Aug 2002 11:19:44   akhare  $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/src/dsatvend.h#1 $ $DateTime: 2007/11/05 02:30:24 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/17/02   rc      Removed FEATURE wrap FEATURE_DS_SOCKETS around dns 
                   functions.
08/27/01   sb      ATCOP code is now partitioned into multiple VUs. As a 
                   result of the partitioning, some of the files have to be
                   renamed. So making the file/interface name changes.
08/10/01   sjd     Initial release to MSM5200 archives.
06/01/01   sb      created file

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCCLR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcclr command.
  at$qcclr command is used for clearing mobile error log.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    OK : if the command has been successfully executed
    ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Sets parameter value for "SET"  command.
  Sets response in supplied buffer for "READ" and "TEST" commands.
  Calls the necessary functions to clear the error log.

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcclr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDMG_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdmg command.
  at$qcdmg is used to transition the serial port to diagnostic
  monitor.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Transitions the serial port to Diagnostic Monitor.

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdmg_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDMR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdmr command.
  at$qcdmr is used to set the DM baud rate.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DS_ATCOP_OK : if the command has been successfully executed
    DS_ATCOP_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdmr_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

#ifdef FEATURE_DS_SOCKETS_TEST_UDP 
#error code not present
#endif /* FEATURE_DS_SOCKETS_TEST_UDP */


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDNSP_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdnsp command.
  at$qcdnsp is used to set primary DNS IP address.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DS_ATCOP_OK : if the command has been successfully executed
    DS_ATCOP_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdnsp_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATVEND_EXEC_QCDNSS_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcdnss command.
  at$qcdnss is used to set secondary DNS IP address.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DS_ATCOP_OK : if the command has been successfully executed
    DS_ATCOP_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcdnss_cmd
(
  dsat_mode_enum_type mode,               /*  AT command mode:            */
  const dsati_cmd_type *parse_table,      /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,      /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr             /*  Place to put response       */
);


/*===========================================================================
FUNCTION DSATVEND_INIT_DNS_FROM_NV

DESCRIPTION
  This function reads Primary DNS and Secondary DNS from the NV during
  initialization. If these items were never written in NV, the values are
  made as NULL.
    
DEPENDENCIES
  None

RETURN VALUE
  None

SIDE EFFECTS
  None
  
===========================================================================*/
void dsatvend_init_dns_from_nv
(
  void
);


/*===========================================================================

FUNCTION DSATVEND_EXEC_QCTER_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$qcter command.
  at$qcter is used to set current and default TE-DCE baud rate.

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  None

===========================================================================*/
dsat_result_enum_type dsatvend_exec_qcter_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

#ifdef FEATURE_DSAT_BREW_SUPPORT
/*===========================================================================

FUNCTION DSATVEND_EXEC_BREW_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at$brew command.
    at$brew is used to transition control from ATCOP to BREW COP

DEPENDENCIES
  None

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Transitions the serial port to Brew.

===========================================================================*/
/* ARGSUSED */
dsat_result_enum_type dsatvend_exec_brew_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);
#endif /* FEATURE_DSAT_BREW_SUPPORT */

#endif /* FEATURE_DATA */

#endif /* DSATVEND_H */

