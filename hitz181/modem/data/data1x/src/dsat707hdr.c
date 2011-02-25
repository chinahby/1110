/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This module executes AT commands. It executes IS-707 specific HDR related
  AT commands.

EXTERNALIZED FUNCTIONS
dsat707_exec_qchdrc_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRC command.
  For 1xEV select a fixed DCR value. The default value is F, for
  variable rate mode.

dsat707_exec_qchdrr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRR command.
  For 1xEV, select a fixed reverse rate. Default value is F,
  for variable rate mode.

dsat707_exec_qchdrt_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCHDRT command.
  For 1xEV, toggle early termination. Default value is 1.
  
Copyright (c) 1995,1996,1997 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
04/03/02   rsl     Initial version of file.

===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#ifdef FEATURE_HDR
#error code not present
#endif /* FEATURE_HDR        */
#endif /* FEATURE_DATA_IS707 */
