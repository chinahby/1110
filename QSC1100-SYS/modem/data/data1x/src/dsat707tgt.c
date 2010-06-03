/*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*

                        D A T A   S E R V I C E S
                A T   C O M M A N D   P R O C E S S O R

          I S - 7 0 7   T A R G E T   C O M M A N D   T A B L E S

GENERAL DESCRIPTION
  This module contains the top level command tables for the IS-707
  target.  It also contains any target specific command tables required
  to support target specific support of AT modem commands.

EXTERNALIZED FUNCTIONS
  None
    
INITIALIZATION AND SEQUENCING REQUIREMENTS
  None

   Copyright (c) 1995 - 2009 by QUALCOMM Incorporated.
   All Rights Reserved.
   Qualcomm Confidential and Proprietary.
*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/


/*===========================================================================

                        EDIT HISTORY FOR MODULE

  This section contains comments describing changes made to the module.
  Notice that changes are listed in reverse chronological order.

  $PVCSPath: L:/src/asw/MM_DATA/vcs/dsat707tgt.c_v   1.3   29 Oct 2002 13:29:42   sramacha  $
  $Header: //source/qcom/qct/modem/data/common/1xatcop/main/lite/src/dsat707tgt.c#6 $ $DateTime: 2009/01/27 05:42:02 $ $Author: nsivakum $


when       who     what, where, why
--------   ---     ----------------------------------------------------------
01/23/09   sn      Fixed featurization issue.
01/21/09   nc      Added support for SMS Commands.
01/02/09   bs      Added support for 1X AT Phonebook commands.
07/09/08   bs      Added support for CDMA NAARefresh.
05/24/02   rsl     Initial release of file.

===========================================================================*/


/*===========================================================================

                     INCLUDE FILES FOR MODULE

===========================================================================*/

#include "comdef.h"
#include "customer.h"

#ifdef FEATURE_DATA

#include "dsati.h"
#include "dsatctab.h"
#include "dsatme.h"

#ifdef FEATURE_DATA_IS707
#include "dsat707extctab.h"
#include "dsat707faxctab.h"
#ifdef FEATURE_HDR
#error code not present
#endif

#ifdef FEATURE_CDMA_SMS
#include "dsatsms.h"
#include "dsat707smsctab.h"
#endif /* FEATURE_CDMA_SMS */

#ifdef FEATURE_DS_MOBILE_IP
#include "dsat707mipctab.h"
#endif
#ifdef FEATURE_DS_PSTATS
#error code not present
#endif
#include "dsat707vendctab.h"
#include "dsatcmif.h"
#include "dsatvoice.h"

/*===========================================================================

                    REGIONAL DEFINITIONS AND DECLARATIONS

===========================================================================*/

/*===========================================================================

            LOCAL DEFINITIONS AND DECLARATIONS FOR MODULE

  This section contains local definitions for constants, macros, types,
  variables and other items needed by this module.

===========================================================================*/

/*---------------------------------------------------------------------------
                           Defaults and limits
---------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
                       Target Specific Command Tables 
--------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------
                         Top Level Command Tables
---------------------------------------------------------------------------*/

dsati_at_cmd_table_entry_type basic_table_entries[ ] =
{
  { dsat_basic_table,        &dsat_basic_table_size },
  { dsat_basic_action_table, &dsat_basic_action_table_size },  
  { NULL, 0 }
};

dsati_at_cmd_table_entry_type sreg_table_entries[ ] =
{
  { dsat_sreg_table, &dsat_sreg_table_size },
  { NULL, 0 }
};

dsati_at_cmd_table_entry_type extended_table_entries[ ] =
{
  { dsat_ext_table, &dsat_ext_table_size },
  { dsat707_ext_para_table, &dsat707_ext_para_table_size },
  { dsat707_ext_action_table, &dsat707_ext_action_table_size },
#ifndef FEATURE_ASYNC_DATA_NOOP
  { dsat707_fax_table, &dsat707_fax_table_size },
#endif /* FEATURE_ASYNC_DATA_NOOP */
  { NULL, 0 }
};

dsati_at_cmd_table_entry_type vendor_table_entries[ ] =
{
  { dsat_vendor_table, &dsat_vendor_table_size },
  { dsat707_vend_para_table, &dsat707_vend_para_table_size },
  { dsat707_vend_action_table, &dsat707_vend_action_table_size },
  #ifdef FEATURE_HDR
#error code not present
  #endif /* FEATURE_HDR */
  #ifdef FEATURE_DS_MOBILE_IP
  { dsat707_mip_table, &dsat707_mip_table_size },
  #endif /* FEATURE_DS_MOBILE_IP */
  #ifdef FEATURE_DS_PSTATS
#error code not present
  #endif /* FEATURE_DS_PSTATS */
#ifdef FEATURE_CDMA_SMS
  { dsat707_sms_table, &dsat707_sms_table_size },
#endif /* FEATURE_CDMA_SMS */
  { NULL, 0 }
};

const dsati_at_cmd_table_type at_cmd_table =
{
  { basic_table_entries,    basic_table_entries },
  { sreg_table_entries,     sreg_table_entries },
  { extended_table_entries, extended_table_entries },
  { vendor_table_entries,   vendor_table_entries }
};


/*---------------------------------------------------------------------------
                      Asynchronous Event Handler Table
---------------------------------------------------------------------------*/

/* the async_event_table should have DSATI_NUM_ASYNC_EVENT_TABLE_ENTRIES 
   number of entries */
const dsati_async_event_table_entry_type async_event_table[ ] =
{
  { DS_AT_STATUS_CMD,       { NULL, 
      dsatme_cmd_status_handler } },
  { DS_AT_CM_CALL_CMD,        { dsatcmif_cm_call_cmd_handler,
      dsatcmif_cm_call_cmd_handler } },
  { DS_AT_CM_CALL_INFO_CMD,   { dsatcmif_cm_call_event_handler,
      dsatcmif_cm_call_event_handler} },
  { DS_AT_TIMER_EXPIRED_CMD,     { dsatvoice_timer_expired_handler,
      dsatvoice_timer_expired_handler } },
#ifdef FEATURE_MMGSDI
  { DS_AT_MMGSDI_INFO_CMD,     { dsatme_mmgsdi_event_handler, 
      dsatme_mmgsdi_event_handler} },
#endif /* FEATURE_MMGSDI */

#ifdef FEATURE_DSAT_CDMA_PBM
  { DS_AT_PBM_CB_CMD,         { NULL,
      dsatme_pbm_cb_cmd_handler } },
#endif /* FEATURE_DSAT_CDMA_PBM */

#ifdef FEATURE_CDMA_SMS
  { DS_AT_SMS_ERR_CMD,    { NULL, dsatsms_err_handler } },
  { DS_AT_SMS_ABT_CMD,    { NULL, dsatsms_abt_handler } },
#endif /* FEATURE_CDMA_SMS */

/* need this here to appease the compiler when none of the
     above features is defined,
     DS_AT_ASYNC_EVENT_END should never be used for a real 
     event, ERR will be thrown if it is used */
  { DS_AT_ASYNC_EVENT_END, { NULL } }
};

#endif /* FEATURE_DATA_IS707 */
#endif /* FEATURE_DATA */
