#ifndef DSAT707VENDCTAB_H
#define DSAT707VENDCTAB_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  vendor specific commands specific to IS-707 mode of operation.

Copyright (c) 1995,1996,1997 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1998 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 1999, 2000, 2001 by QUALCOMM, Incorporated.  All Rights Reserved.
Copyright (c) 2002-2006 by QUALCOMM, Incorporated.  All Rights Reserved.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.
  
  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707vendctab.h_v   1.5   08 Jan 2003 12:28:00   sramacha  $
  $Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/inc/dsat707vendctab.h#1 $ $DateTime: 2008/04/11 07:46:55 $ $Author: nsivakum $

when        who    what, where, why
--------    ---    ----------------------------------------------------------
10/20/06    squ    Fixed compile error when disable IS95B_MDR feature.
10/15/02    ak     Added struct/vars for qcchs, which is for control/hold.
10/15/02    ak     Updated FEATURE_JCDMA_DS to FEATURE_JCDMA_DS_1X
09/24/02    atp    Added support for QOS. (1x Release-A feature).
04/03/02    rsl    Initial version of file.

===========================================================================*/


/*===========================================================================
                          INCLUDE FILES
===========================================================================*/
#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsatctab.h"
#include "ds707.h"


/*===========================================================================
                           VARIABLE DECLARATIONS
===========================================================================*/
/*=========================================================================*/
/* IS-707 Vendor specific AT command table */
/*=========================================================================*/

extern const dsati_cmd_type dsat707_vend_para_table[ ];
extern const unsigned int dsat707_vend_para_table_size;

extern const dsati_cmd_type dsat707_vend_action_table[ ];
extern const unsigned int dsat707_vend_action_table_size;
/* Data declarations for vendor specific AT commands */

extern dsat_num_item_type dsat707_qcmdr_val;

#ifdef FEATURE_IS2000_R_SCH
/* TURN SCRM on/off */
extern dsat_num_item_type dsat707_qcscrm_val;
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_IS2000_R_SCH
/* TURN R-SCH throttle on/off */
extern dsat_num_item_type dsat707_qctrtl_val;
#endif /* FEATURE_IS2000_R_SCH */

extern dsat_string_item_type dsat707_qcotc_val[241];

/*  Numeric Config items  */
#ifdef FEATURE_IS2000_CHS
/* Enable/disable Mobile-originated Control/Hold */
extern dsat_num_item_type dsat707_qcchs_val;
#endif /* FEATURE_IS2000_CHS */

#ifdef  FEATURE_DS_QNC
/* Quick Net Connect on/off */
extern dsat_num_item_type dsat707_qcqnc_val;
#else
extern const dsat_num_item_type dsat707_qcqnc_val;
#endif /* FEATURE_DS_QNC */

/* Which s.o. set */
extern dsat_num_item_type dsat707_qcso_val;

extern dsat_num_item_type dsat707_qcvad_val;

#ifdef FEATURE_IS2000_REL_A
extern dsat_num_item_type dsat707_qcqospri_val;
#endif /* FEATURE_IS2000_REL_A */

/*---------------------------------------------------------------------------
   Declarations of constant default, lower bound and upper bound for 
   arguments of AT command with numeric arguments.                         
---------------------------------------------------------------------------*/

extern dflm_type dsat707_qcpknd_dflm[];

extern dflm_type dsat707_qcmdr_dflm[];

#ifdef FEATURE_IS2000_R_SCH
extern dflm_type dsat707_qcscrm_dflm[];
#endif /* FEATURE_IS2000_R_SCH */
#ifdef FEATURE_IS2000_R_SCH
extern dflm_type dsat707_qctrtl_dflm[];
#endif /* FEATURE_IS2000_R_SCH */

#ifdef FEATURE_IS2000_CHS
extern dflm_type dsat707_qcchs_dflm [];
#endif /* FEATURE_IS2000_CHS */

#ifdef FEATURE_DS_QNC
extern dflm_type dsat707_qcqnc_dflm[];
#endif

#ifdef FEATURE_DS_CHOOSE_SO
extern dflm_type dsat707_qcso_dflm[];
#endif

#ifdef FEATURE_IS2000_REL_A
extern dflm_type dsat707_qcqospri_dflm[];
#endif /* FEATURE_IS2000_REL_A */


/*===========================================================================
                          EXTERNAL FUNCTIONS
===========================================================================*/

#endif /* FEATURE_DATA_IS707 */
#endif /* DSAT707VENDCTAB_H  */
