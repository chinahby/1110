#ifndef DSATPARM_H
#define DSATPARM_H
/*===========================================================================

                        D A T A   S E R V I C E S

                A T   C O M M A N D   ( P A R A M E T E R 
                
                C O M M A N D S )   P R O C E S S I N G

                E X T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( parameter commands ) processor.

Copyright (c) 1995,1996,1997 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsatparm.h_v   1.1   30 Sep 2002 15:34:08   randrew  $
  $Header: //source/qcom/qct/modem/data/common/commonatcop/main/lite/inc/dsatparm.h#1 $ $DateTime: 2008/04/11 07:54:07 $ $Author: nsivakum $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
05/04/01   SB      created file
08/10/01   sjd     Initial release to MSM5200 archives.
08/27/01   sb	   ATCOP code is now partitioned into multiple VUs. As a 
                   result of the partitioning, some of the files have to be
                   renamed. So making the file/interface name changes.

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA


/*===========================================================================

                      PUBLIC FUNCTION DECLARATIONS

===========================================================================*/

/*===========================================================================

FUNCTION DSATPARM_EXEC_PARAM_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. This function executes parameter / S register
  commands.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    OK : if the command has been successfully executed
    ERROR : if there was any problem in executing the command
    
SIDE EFFECTS
  Sets parameter value for "SET"  command.
  Sets response in supplied buffer for "READ" and "TEST" commands.

===========================================================================*/
dsat_result_enum_type dsatparm_exec_param_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATPARM_EXEC_IPR_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes +IPR command.
  at+ipr is used to specify the data rate at which the DCE will 
  accept the command

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    OK : if the command has been successfully executed
    ERROR : if there was any problem in executing the command
    
SIDE EFFECTS
  Sets parameter value for "SET"  command.
  Sets response in supplied buffer for "READ" and "TEST" commands.

===========================================================================*/
dsat_result_enum_type dsatparm_exec_ipr_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


/*===========================================================================

FUNCTION DSATPARM_EXEC_AMPV_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes at&v command.
  at&v is used to dump configuration parameters.

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_OK : if the command has been successfully executed
    DSAT_ERROR : if there was any problem in executing the command

SIDE EFFECTS
  Sets parameter value for "SET"  command.
  Sets response in supplied buffer for "READ" and "TEST" commands.

===========================================================================*/
dsat_result_enum_type dsatparm_exec_ampv_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);

/*===========================================================================

FUNCTION DSATPARM_EXEC_DTYPE_CMD

DESCRIPTION
  This function takes the result from the command line parser
  and executes it. It executes ATT and ATP commands.
  ATT and ATP are used to select type of dialling (tone or pulse) 
  to be used in subsequent dials. This commands will have no effect 
  in our code

DEPENDENCIES

RETURN VALUE
  returns an enum that describes the result of the command execution.
  possible values:
    DSAT_ERROR : if there are any arguments
    DSAT_OK : if there are no arguments

SIDE EFFECTS
  None
===========================================================================*/
dsat_result_enum_type dsatparm_exec_dtype_cmd
(
  dsat_mode_enum_type mode,             /*  AT command mode:            */
  const dsati_cmd_type *parse_table,    /*  Ptr to cmd in parse table   */
  const tokens_struct_type *tok_ptr,    /*  Command tokens from parser  */
  dsm_item_type *res_buff_ptr           /*  Place to put response       */
);


#endif /* FEATURE_DATA */

#endif /* DSATPARM_H */
