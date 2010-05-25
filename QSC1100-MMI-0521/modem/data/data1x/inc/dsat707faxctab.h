#ifndef DSAT707FAXCTAB_H
#define DSAT707FAXCTAB_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S                
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  FAX related commands specific to IS-707 mode of operation.
  
   Copyright (c) 1995-2009 by QUALCOMM, Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/23/09   sn      Featurized code under FEATURE_ASYNC_DATA_NOOP.
04/03/02   rsl     Initial version of file.

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifndef FEATURE_ASYNC_DATA_NOOP
#ifdef FEATURE_DATA_IS707
#include "dsati.h"
#include "dsatctab.h"

/*=========================================================================*/
/* IS-707 Fax related AT command table */
/*=========================================================================*/

/*  Flag to indicate that an AT+FDT command has been   */
/*  and that a land to mobile FAX image is about to start */
extern boolean dsat707_fdt_seen;

/* Size of the function value. */
extern const unsigned int      dsat707_sizeof_fns_val; 

extern const dsati_cmd_type dsat707_fax_table[ ];
extern const unsigned int dsat707_fax_table_size;

/* Data declarations for fax commands */

extern dsat_num_item_type dsat707_faa_val;
extern dsat_num_item_type dsat707_fap_val[];
extern dsat_num_item_type dsat707_fbo_val;
extern dsat_num_item_type dsat707_fbu_val;
extern dsat_num_item_type dsat707_fcc_val[];
extern dsat_num_item_type dsat707_fcq_val[];
extern dsat_num_item_type dsat707_fcr_val;
extern dsat_num_item_type dsat707_fct_val;
extern dsat_num_item_type dsat707_fea_val;
extern dsat_num_item_type dsat707_ffc_val[];
extern dsat_num_item_type dsat707_fhs_val;
extern dsat_num_item_type dsat707_fie_val;
extern dsat_num_item_type dsat707_fip_val;
extern dsat_num_item_type dsat707_fis_val[];
extern dsat_num_item_type dsat707_flp_val;
extern dsat_num_item_type dsat707_fms_val;
extern dsat_num_item_type dsat707_fnr_val[];
extern dsat_string_item_type dsat707_fns_val[];
extern dsat_num_item_type dsat707_fpp_val;
extern dsat_num_item_type dsat707_fpr_val;
extern dsat_num_item_type dsat707_fps_val;
extern dsat_num_item_type dsat707_frq_val[];
extern dsat_num_item_type dsat707_fry_val;
extern dsat_num_item_type dsat707_fsp_val;

#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_ASYNC_DATA_NOOP */

#endif /* DSAT707FAXCTAB_H   */

