#ifndef DSAT707SMSCTAB_H
#define DSAT707SMSCTAB_H
/*===========================================================================

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

               I S - 7 0 7   C O M M A N D   T A B L E S                
                I N T E R N A L   H E A D E R   F I L E


DESCRIPTION
  This file contains the definitions of data structures, defined and
  enumerated constants, and function prototypes required for the
  data services AT command processor command tables that define
  SMS related commands specific to IS-707 mode of operation.
  
   Copyright (c) 2009 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
===========================================================================*/


/*===========================================================================

                      EDIT HISTORY FOR FILE

  $Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/inc/dsat707smsctab.h#1 $ $DateTime: 2009/01/23 10:36:19 $ $Author: nchagant $
		      
  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

when       who     what, where, why
--------   ---     ----------------------------------------------------------
11/13/08   nc      Added Support for QCMGW,QCMSS,QCMGL,QCMGF Commands
06/01/07   sa      Mainlining SMS C2K changes
11/28/03   sb      created the module. CDMA SMS additions

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_CDMA_SMS
#include "dsati.h"

/*=========================================================================*/
/* IS-707 Sms related AT command table */
/*=========================================================================*/
extern const unsigned int       dsat707_sms_table_size;
extern const dsati_cmd_type     dsat707_sms_table [];

/* Data declarations for sms commands */
extern dsat_mixed_param_val_type dsat707sms_qcsmp_val[];
extern const dsat_string_item_type  qcsmp_tid_valstr [][8];

extern const def_list_type      dsat707sms_qcpms_list[];
extern dsat_num_item_type       dsat707sms_qcpms_val[];

extern dsat_num_item_type       dsat707sms_qcnmi_val[];
extern dsat_num_item_type       dsat707sms_qcmgf_val;

#endif /* FEATURE_CDMA_SMS */
#endif /* DSAT707SMSCTAB_H   */

