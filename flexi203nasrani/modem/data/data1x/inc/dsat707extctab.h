#ifndef DSAT707EXTCTAB_H
#define DSAT707EXTCTAB_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S                
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  extended commands specific to IS-707 mode.
  
Copyright (c) 2004 - 2008 by QUALCOMM, Incorporated.
All Rights Reserved.
Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
06/24/08   mga     Merged changes to support CTA timer NV item
04/03/02   rsl     Initial version of file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsatctab.h"

/*=========================================================================*/
/* General IS-707 extended format AT command table */
/*=========================================================================*/

extern const dsati_cmd_type dsat707_ext_para_table[ ];
extern const unsigned int dsat707_ext_para_table_size;

extern const dsati_cmd_type dsat707_ext_action_table[ ];
extern const unsigned int dsat707_ext_action_table_size;

/* Data declarations for extended commands */

extern dsat_num_item_type dsat707_cdr_val;
extern dsat_num_item_type dsat707_cds_val[];
extern dsat_num_item_type dsat707_cfc_val;
extern dsat_string_item_type dsat707_cfg_val[];

#ifdef FEATURE_DS_IS707A_CMUX
extern dsat_num_item_type dsat707_cmux_val[];
extern const word dsat707_cmux_fwd_table[];
extern const word dsat707_cmux_rev_table[];
#else
extern dsat_num_item_type dsat707_cmux_val;
#endif /* FEATURE_DS_IS707A_CMUX */

extern dsat_num_item_type dsat707_cqd_val;
extern dsat_num_item_type dsat707_crc_val;
extern dsat_num_item_type dsat707_crm_val;
extern dsat_num_item_type dsat707_cta_val;
extern dsat_num_item_type dsat707_cxt_val;
extern dsat_num_item_type dsat707_eb_val[];
extern dsat_num_item_type dsat707_efcs_val;
extern dsat_num_item_type dsat707_er_val;
extern dsat_num_item_type dsat707_es_val[];
extern dsat_num_item_type dsat707_esr_val;
extern dsat_num_item_type dsat707_etbm_val[];
extern dsat_num_item_type dsat707_ilrr_val;
extern dsat_string_item_type dsat707_ma_val[];
extern dsat_num_item_type dsat707_mr_val;
extern dsat_string_item_type dsat707_ms_val[];
extern dsat_num_item_type dsat707_mv18r_val;
extern dsat_num_item_type dsat707_mv18s_val[];

#define DS707_CTA_DEFAULT_VALUE 30
#define DS707_CTA_LOWER_LIMIT 0
#define DS707_CTA_UPPER_LIMIT 255

extern dflm_type dsat707_cta_dflm [];

#endif /* FEATURE_DATA_IS707  */
#endif /* DSAT707EXTCTAB_H    */
