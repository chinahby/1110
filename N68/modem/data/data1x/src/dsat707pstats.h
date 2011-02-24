#ifndef DSAT707PSTATS_H
#define DSAT707PSTATS_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants and function prototypes required for the
  data services AT command ( IS-707 mode Protocol Statistics commands )
  processor.

EXTERNALIZED FUNCTIONS
dsat707_exec_qcrlpd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPD command.
  Dump RLP protocol statistics.

dsat707_exec_qcrlpr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPR command.
  Reset RLP protocol statistics.
  
dsat707_exec_qcrlpd33_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPD33 command.
  Dump RLP 3 protocol statistics.
  
dsat707_exec_qcrlpr33_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCRLPR33 command.
  Reset RLP 3 protocol statistics.
  
dsat707_exec_qcpppd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPPPD command.
  Dump PPP protocol statistics.
  
dsat707_exec_qcpppr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCPPPR command.
  Reset PPP protocol statistics.
  
dsat707_exec_qcipd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCIPD command.
  Dump IP protocol statistics.
  
dsat707_exec_qcipr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCIPR command.
  Reset IP protocol statistics.
  
dsat707_exec_qcudpd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCUDPD command.
  Dump UDP protocol statistics.
  
dsat707_exec_qcudpr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCUDPR command.
  Reset UDP protocol statistics.
  
dsat707_exec_qctcpd_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTCPD command.
  Dump TCP protocol statistics.
  
dsat707_exec_qctcpr_cmd
  This function takes the result from the command line parser
  and executes it. It executes AT$QCTCPR command.
  Reset TCP protocol statistics.
  
  
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

#ifdef FEATURE_DATA
#ifdef FEATURE_DATA_IS707

#ifdef  FEATURE_DS_PSTATS
#error code not present
#endif /* FEATURE_DS_PSTATS */
#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_DATA */
#endif /* DSAT707PSTATS_H */

